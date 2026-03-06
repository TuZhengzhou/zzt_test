#include <chrono>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

void func(int max_events, int timeout_sec) {
    int epoll_fd, sock_fd;
    int timeout_ms = timeout_sec * 1000;
    struct epoll_event ev, events[max_events];
    
    // 1. 创建 epoll 实例
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }
    
    // 2. 创建 socket 示例（这里以监听socket为例）
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    
    // 设置非阻塞
    int flags = fcntl(sock_fd, F_GETFL, 0);
    fcntl(sock_fd, F_SETFL, flags | O_NONBLOCK);
    
    // 绑定和监听
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(8080);
    
    if (bind(sock_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    
    listen(sock_fd, SOMAXCONN);
    
    // 3. 添加事件到 epoll
    ev.events = EPOLLIN | EPOLLET;  // 监听可读事件，边缘触发模式
    ev.data.fd = sock_fd;
    
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_fd, &ev) == -1) {
        perror("epoll_ctl: add");
        exit(EXIT_FAILURE);
    }
    
    printf("Listening on port 8080 with %d ms timeout...\n", timeout_ms);
    
    // 4. 事件循环
    while (1) {
        int nfds = epoll_wait(epoll_fd, events, max_events, timeout_ms);
        if (nfds == -1) {
            perror("epoll_wait");
            break;
        } else if (nfds == 0) {
            // 超时处理
            printf("Timeout! No events in %d ms\n", timeout_ms);
            break;
        }
    }
    close(epoll_fd);
    close(sock_fd);
}

int main() {
  auto start = ::std::chrono::steady_clock::now();
  sleep(1);

  int upper = 0x10000000;
  long long sum = 0;
  for (int i = 0; i < upper; i++) {
    sum += i;
  }
  auto finish = ::std::chrono::steady_clock::now();
  std::chrono::milliseconds elapsed_seconds =
      std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
  std::cout << "elapsed_seconds: " << elapsed_seconds.count() << " ms"
            << std::endl;
}