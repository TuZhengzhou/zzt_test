#include <cstdio>
#include <iostream>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[]) {

    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <hostname> <port>" << std::endl;
        std::cout << "Example: " << argv[0] << " localhost 5005" << std::endl;
        return 1;
    }
    // 创建一个 socket
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("socket()");
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5005); // 端口号
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 服务端主动 close(socket_fd) 后，立即重启服务端程序绑定同一端口
    // 如果不设置 SO_REUSEADDR 选项，bind() 可能会失败，报错 "Address already in use"
    // 因为操作系统会将该端口保留一段时间以确保所有延迟的数据包都已处理完毕
    int opt = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(socket_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind()");
    }

    if (listen(socket_fd, 5) < 0) {
        perror("listen()");
        return 1;
    }

    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(socket_fd, (sockaddr*)&client_addr, &client_len);
    if (client_fd < 0) {
        perror("accept()");
        return 1;
    }
    printf("Client connected: %s:%d\n",
           inet_ntoa(client_addr.sin_addr),
           ntohs(client_addr.sin_port));

    char buffer[1024] = {0};
    recv(client_fd, buffer, sizeof(buffer), 0);
    std::cout << "Received from client: " << buffer << std::endl;

    send(client_fd, "Hello, Client!", 14, 0);

    close(client_fd);
    close(socket_fd);
}