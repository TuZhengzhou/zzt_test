#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <arpa/inet.h> // For inet_ntoa

/*
演示传统应对惊群的方法，通过 epoll+非阻塞socket 的组合实现 "平时休眠, 来连接时唤醒所有worker竞争, 但只有一个成功 accept, 其余返回 EAGAIN" 的效果。

虽然只有一个成功，但大量进程被唤醒会导致上下文切换和资源浪费. 在 thunder_3.cpp 中将介绍更好的解决方案。

### 测试方法

#### 1. 编译运行
```bash
g++ -o thunder_2 thunder_2.cpp
sudo ./thunder_2
```

#### 2. 发起测试请求（新终端）
```bash
# 单次连接测试
curl http://localhost:1027
```

#### 3. 观察输出结果
```
[Worker 12345] 等待事件...
[Worker 12346] 等待事件...
[Worker 12347] 等待事件...
[Worker 12348] 等待事件...

（发起请求后）
[Worker 12345] 被唤醒，检测到1个事件
[Worker 12346] 被唤醒，检测到1个事件
[Worker 12347] 被唤醒，检测到1个事件
[Worker 12348] 被唤醒，检测到1个事件
Worker 12345 accept失败：资源暂不可用
Worker 12346 accept失败：资源暂不可用
Worker 12347 accept失败：资源暂不可用
===> Worker 12348 抢到连接!
```

---

### 关键机制说明

1. **非阻塞socket**：
   - 通过`set_nonblock(listen_fd)`设置非阻塞模式
   - 确保多个worker的`accept`调用立即返回，不会永久阻塞

2. **epoll事件传播**：
   - 所有worker监听同一个socket的可读事件
   - 当新连接到达时，内核会通知所有epoll实例

3. **竞争过程**：
   - 被唤醒的worker同时调用`accept`
   - 只有一个worker能成功获取连接
   - 其他worker因连接已被取走而返回`EAGAIN`

---

### 验证工具

#### 1. 使用strace跟踪系统调用
```bash
strace -f -e epoll_wait,accept ./thunder
```
观察多个进程同时被`epoll_wait`唤醒并调用`accept`

#### 2. 查看进程状态变化
```bash
watch -n 0.1 'ps -o pid,state,cmd | grep thunder'
```
观察进程从`S`（休眠）到`R`（运行）的状态切换

#### 3. 监控上下文切换
```bash
vmstat 1
```
关注`cs`列的数值变化，惊群发生时上下文切换次数会明显增加

---

### 优化对比

添加互斥锁后的版本（在`accept`前后加锁）：
pthread_mutex_lock(&lock); // Lock before accessing shared resource
int client_fd = accept(...);
pthread_mutex_unlock(&lock);
```
此时输出将变为：
```
[Worker 12345] 被唤醒，检测到1个事件
===> Worker 12345 抢到连接!
[Worker 12346] 被唤醒，检测到0个事件  # 事件已被处理
```
通过锁机制消除了无效的竞争

---

这个演示清晰地展示了：
1. 无锁时多个worker被同时唤醒
2. 只有一个worker成功处理连接
3. 其他worker产生无效的系统调用
4. 通过互斥锁可以优化这个过程
*/

#define PORT 1027
#define WORKERS 3
#define MAX_EVENTS 10
int listen_fd;
pthread_mutex_t lock;

// 设置非阻塞模式
void set_nonblock(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void worker_process() {
    // 每个worker创建自己的epoll实例
    int epfd = epoll_create1(0);

    int local_listen_fd = dup(listen_fd); // duplicate a file descriptor
    // int local_listen_fd = listen_fd; // 实际应继承主进程的socket

    struct epoll_event ev;
    ev.events = EPOLLIN; // 监听可读事件
    ev.data.fd = local_listen_fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, local_listen_fd, &ev);

    while (1) {
        printf("[Worker %d] 等待事件...\n", getpid());
        struct epoll_event events[MAX_EVENTS];
        
        // 阻塞等待事件（平时休眠状态）
        int n = epoll_wait(epfd, events, MAX_EVENTS, -1);
        printf("[Worker %d] 被唤醒，检测到%d个事件\n", getpid(), n);

        for (int i = 0; i < n; i++) {
            if (events[i].data.fd == local_listen_fd) {
                struct sockaddr_in client_addr;
                socklen_t len = sizeof(client_addr);
                
                // 非阻塞accept
                int client_fd = accept(local_listen_fd, (struct sockaddr*)&client_addr, &len);
                if (client_fd >= 0) {
                    printf("\033[31m===> Worker %d 抢到连接!\033[0m\n", getpid());
                    printf("client_addr: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                    close(client_fd);
                } else {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        printf("Worker %d accept失败：资源暂不可用\n", getpid());
                    } else {
                        perror("accept错误");
                    }
                }
            }
        }
    }
}

int main() {
    // 主进程创建监听 socket
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    
    bind(listen_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(listen_fd, SOMAXCONN);
    set_nonblock(listen_fd); // 关键：设置非阻塞模式

    // 创建worker进程
    for (int i = 0; i < WORKERS; i++) {
        if (fork() == 0) { // 子进程
            worker_process();
            exit(0);
        }
    }

    while (1) pause(); // 主进程挂起
}