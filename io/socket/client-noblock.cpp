#include <cassert>
#include <condition_variable>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <memory>
#include <mutex>
#include <fcntl.h>
#include <thread>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "thread-pool.hpp"

// 头文件 / 目录    缩写全称                                    核心用途
// arpa/            Advanced Research Projects Agency       归属于 ARPANET 衍生的网络接口
// inet.h           Internet                                互联网协议（IP/TCP/UDP）操作
// netinet/         Network + Internet                      TCP/IP 协议栈核心定义
// netdb.h          Network Database                        网络域名 / 服务查询
// unistd.h         UNIX Standard                           UNIX 标准系统调用

const int opened_port = 1027;
bool server_ready = false;
bool server_fail = false;
std::mutex mutex;
std::condition_variable server_cv;

void SetNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL);
    assert((flags & O_NONBLOCK) == 0);

    // if (flags < 0) {}
    flags |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flags);

    int flags_n = fcntl(fd, F_GETFL);
    assert((flags_n & O_NONBLOCK));
}

sockaddr_in create_sockaddr(const char* hostname, int port) {
    hostent* server = gethostbyname("localhost");
    /* if server is nullptr */
    printf("server addr: %s:%d\n", inet_ntoa(*(in_addr *)server->h_addr), port);

    sockaddr_in addr_in;
    addr_in.sin_family = AF_INET;
    addr_in.sin_port = htons(opened_port);     // 固定端口号
    memcpy(&addr_in.sin_addr.s_addr, server->h_addr, server->h_length);

    return addr_in;
}

void client(bool nonblock) {

    std::unique_lock<std::mutex> lock(mutex);
    server_cv.wait(lock, [](){
        return server_ready || server_fail;
    });
    lock.unlock();

    if (server_fail) {
        return;
    }

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    printf("client::server_fd = %d\n", server_fd);
    if (nonblock) {
        SetNonBlocking(server_fd);
        printf("Set socket to nonblock\n");
    }

    sockaddr_in addr_in = create_sockaddr("localhost", opened_port);

    char buf[64];
    int ret = connect(server_fd, (sockaddr*)&addr_in, sizeof(addr_in));
    if (ret == 0) {
        printf("client::connect ok. nonblock = %d\n", nonblock);
        ::recv(server_fd, buf, 64, 0);
        printf("recv: %s\n", buf);
    } else {
        if (errno == EINPROGRESS) {
            printf("client::connect need more check. nonblock = %d\n", nonblock);
        } else {
            printf("client::connect fail. nonblock = %d\n", nonblock);
        }
    }
}

void server_func(void* arg) {
    int* client_fd_ptr = static_cast<int*>(arg);
    
    char buf[64] = "hello";
    ::send(*client_fd_ptr, buf, 5, 0); 
    ::close(*client_fd_ptr);
}

void server(ThreadPool* pool) {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    printf("server::listen_fd = %d\n", listen_fd);

    int optval = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    sockaddr_in addr_in = create_sockaddr("localhost", opened_port);
    int bind_ret = bind(listen_fd, (sockaddr*)&addr_in, sizeof(addr_in));
    if (bind_ret == -1) {
        printf("bind_ret error\n");
        server_fail = true;
        server_cv.notify_all();
        return;
    }

    listen(listen_fd, 1024);

    std::unique_lock<std::mutex> lock(mutex);
    server_ready = true;
    server_cv.notify_all();
    lock.unlock();

    int cnt = 0;
    while (cnt < 2) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(listen_fd, (sockaddr *)&client_addr, &client_len);
        printf("server::client_fd: %d, accept connet from: %s:%d\n", client_fd, inet_ntoa(*(in_addr *)&client_addr.sin_addr.s_addr), client_addr.sin_port);

        pool->PushJob(server_func, &client_fd);
        
        cnt++;
    }
    ::close(listen_fd);
}



int main() {
    ThreadPool pool(4, 0);
    auto thread_server = std::thread(server, &pool);
    auto thread_client1 = std::thread(client, true);
    auto thread_client2 = std::thread(client, false);
    thread_client1.join();
    thread_client2.join();
    thread_server.join();
}
