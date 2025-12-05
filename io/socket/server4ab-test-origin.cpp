// #include <arpa/inet.h>
// #include <netinet/in.h>
// #include <sys/socket.h>
// #include <unistd.h>

// #include <cstring>
// #include <iostream>
// #include <string>

// int main(int argc, char* argv[]) {
//     int port = 8080;
//     if (argc >= 2) {
//         port = std::stoi(argv[1]);
//     }

//     int listen_fd = ::socket(AF_INET, SOCK_STREAM, 0);
//     if (listen_fd < 0) {
//         perror("socket");
//         return 1;
//     }

//     // 允许端口复用，方便重新启动服务器
//     int opt = 1;
//     if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
//         perror("setsockopt");
//         return 1;
//     }

//     sockaddr_in addr{};
//     addr.sin_family = AF_INET;
//     addr.sin_addr.s_addr = htonl(INADDR_ANY);
//     addr.sin_port = htons(port);

//     if (bind(listen_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
//         perror("bind");
//         return 1;
//     }

//     if (listen(listen_fd, 1024) < 0) {
//         perror("listen");
//         return 1;
//     }

//     std::cout << "Listening on port " << port << "..." << std::endl;

//     // 准备一个固定 HTTP 响应
//     const std::string body = "Hello, ApacheBench!\n";
//     const std::string response =
//         "HTTP/1.1 200 OK\r\n"
//         "Content-Type: text/plain\r\n"
//         "Content-Length: " + std::to_string(body.size()) + "\r\n"
//         "Connection: keep-alive\r\n"
//         "\r\n" +
//         body;

//     int acc_cnt = 0;
//     while (true) {
//         sockaddr_in client_addr{};
//         socklen_t client_len = sizeof(client_addr);
//         int client_fd = ::accept(listen_fd, (sockaddr*)&client_addr, &client_len);
//         if (client_fd < 0) {
//             perror("accept");
//             continue;
//         }

//         printf("acc_cnt = %d start, client_fd = %d\n", ++acc_cnt, client_fd);

//         // 简单读一下请求，但不解析
//         char buf[4096];
//         ssize_t n = ::read(client_fd, buf, sizeof(buf));
//         if (n <= 0) {
//             printf("n <= 0\n");
//             ::close(client_fd);
//             continue;
//         }
//         printf("Finish read\n");

//         // 直接写回响应
//         ssize_t total = 0;
//         while (total < (ssize_t)response.size()) {
//             ssize_t written = ::write(client_fd, response.data() + total, response.size() - total);
//             if (written <= 0) {
//                 break;
//             }
//             total += written;
//         }

//         // 这里为了简单起见，直接关闭连接（不复用）
//         ::close(client_fd);
//         printf("acc_cnt = %d end\n", acc_cnt);
//     }

//     ::close(listen_fd);
//     return 0;
// }


#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h> // 新增：处理SIGPIPE

#include <cstring>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    // 关键优化1：屏蔽SIGPIPE信号，避免写关闭连接时进程退出
    signal(SIGPIPE, SIG_IGN);

    int port = 8080;
    if (argc >= 2) {
        port = std::stoi(argv[1]);
    }

    int listen_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket");
        return 1;
    }

    // 允许端口复用，方便重新启动服务器
    int opt = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        return 1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    if (bind(listen_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }

    if (listen(listen_fd, 1024) < 0) {
        perror("listen");
        return 1;
    }

    std::cout << "Listening on port " << port << "..." << std::endl;

    // 准备一个固定 HTTP 响应
    const std::string body = "Hello, ApacheBench!\n";
    const std::string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: " + std::to_string(body.size()) + "\r\n"
        "Connection: keep-alive\r\n"
        "\r\n" +
        body;

    int acc_cnt = 0;
    while (true) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int client_fd = ::accept(listen_fd, (sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        // 优化2：打印客户端IP+端口，便于定位哪个连接出问题
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        int client_port = ntohs(client_addr.sin_port);
        acc_cnt++;
        printf("acc_cnt = %d start, client_fd = %d, client: %s:%d\n", 
               acc_cnt, client_fd, client_ip, client_port);

        int err = 0;
        socklen_t err_len = sizeof(err);
        if (getsockopt(client_fd, SOL_SOCKET, SO_ERROR, &err, &err_len) < 0) {
            perror("getsockopt");
            exit(-1);
        }
        // err != 0 表示连接异常（如ECONNRESET）
        if (err != 0) {
            printf("err != 0\n");
            ::close(client_fd);
            continue;
        } else {
            printf("err == 0\n");
        }

        // 简单读一下请求，但不解析
        char buf[4096];
        ssize_t n = ::read(client_fd, buf, sizeof(buf));
        if (n <= 0) {
            // 优化3：区分read返回0（正常关闭）和-1（异常错误）
            if (n == 0) {
                printf("acc_cnt = %d, client_fd = %d: client normal close (read return 0)\n",
                       acc_cnt, client_fd);
            } else { // n == -1
                printf("acc_cnt = %d, client_fd = %d: read error (return -1), errno=%d: %s\n",
                       acc_cnt, client_fd, errno, strerror(errno));
            }
            ::close(client_fd);
            continue;
        }
        printf("acc_cnt = %d, client_fd = %d: Finish read, read %zd bytes\n", 
               acc_cnt, client_fd, n);

        // 直接写回响应
        ssize_t total = 0;
        bool write_ok = true;
        while (total < (ssize_t)response.size()) {
            ssize_t written = ::write(client_fd, response.data() + total, response.size() - total);
            if (written <= 0) {
                // 优化4：记录write失败的原因
                printf("acc_cnt = %d, client_fd = %d: write error (return %zd), errno=%d: %s\n",
                       acc_cnt, client_fd, written, errno, strerror(errno));
                write_ok = false;
                break;
            }
            total += written;
        }

        // 这里为了简单起见，直接关闭连接（不复用）
        ::close(client_fd);
        if (write_ok) {
            printf("acc_cnt = %d end, client_fd = %d: response sent successfully\n", 
                   acc_cnt, client_fd);
        } else {
            printf("acc_cnt = %d end, client_fd = %d: response send failed\n", 
                   acc_cnt, client_fd);
        }
    }

    ::close(listen_fd);
    return 0;
}