// 2025.03.27 TU
// 演示惊群现象, 未添加应对措施. 通过 epoll+阻塞socket 的组合实现 "平时休眠, 来连接时唤醒所有worker竞争, 但只有一个成功 accept, 其余阻塞在 accept" 的效果.

// 当多个进程共享同一个监听套接字时，accept是互斥的，即内核会确保只有一个进程成功接受连接，
// 其他进程在调用accept时可能返回错误，或者继续阻塞等待下一个连接。
// 在阻塞模式下，当一个连接被一个进程accept后，其他进程继续阻塞在accept调用，等待下一个连接。
// 在非阻塞模式下，其他进程会直接返回错误（EAGAIN）。

// How to run this code:
//  g++ -o thunder_1 thunder_1.cpp
//  sudo ./thunder_1
//  curl http://localhost:1027 # 发起请求 from another terminal

#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <arpa/inet.h> // For inet_ntoa
#include <sys/epoll.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define PORT 1027
#define WORKERS 3
#define MAX_EVENTS 10

int listen_fd;

void worker_process() {
    // epoll_create() creates a new epoll(7) instance. Since Linux 2.6.8, the size argument
    // is ignored, but must be [greater than zero]; see NOTES below.
    // int epfd = epoll_create1(0);
    int epfd = epoll_create(1);

    int local_listen_fd = dup(listen_fd);

    epoll_event ep_event;
    ep_event.data.fd = local_listen_fd;
    ep_event.events  = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, local_listen_fd, &ep_event);

    while(true) {
        printf("[Worker %d] waiting ... \n", getpid());

        epoll_event events[MAX_EVENTS];
        int n = epoll_wait(epfd, events, MAX_EVENTS, -1);
        if(n == -1) {
            printf("epoll_wait error, error code is %d\n", errno);
            pause();
        }
        printf("[Worker %d] 被唤醒，检测到%d个事件\n", getpid(), n);
        
        for(int i = 0; i < n; i++) {
            if(events[i].data.fd == local_listen_fd) {
                sockaddr_in client_addr;
                socklen_t addr_len = sizeof(client_addr);

                printf("The workers not output [Worker %d] success are blocked\n", getpid());
                int client_fd = accept(local_listen_fd, (struct sockaddr*)&client_addr, &addr_len);
                if(client_fd >= 0) {
                    printf("[Worker %d] success\n", getpid());
                    printf("client_addr: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                    close(client_fd);
                } else {
                    printf("On the block mode, the code will never arrive here\n");
                    if(errno == EAGAIN || errno == EWOULDBLOCK) {
                        printf("errno == EAGAIN || errno == EWOULDBLOCK\n");
                    } else {
                        printf("error, but else\n");
                    }
                }
            }
        }
    }
}

int main() {
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port   = htons(PORT);

    bind(listen_fd, (sockaddr*)&server_addr, sizeof(server_addr));
    listen(listen_fd, SOMAXCONN);

    // 创建worker进程
    for (int i = 0; i < WORKERS; i++) {
        if (fork() == 0) { // 子进程
            worker_process();
            exit(0);
        }
    }

    while (1) pause(); // 主进程挂起
}