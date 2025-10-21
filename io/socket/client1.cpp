#include <cstdio>
#include <iostream>
#include <cstring>
#include <memory>

#include <arpa/inet.h>
#include <netdb.h>
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

    // 设置服务器信息并连接
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2])); // 端口号

    // hostent* server = gethostbyname("localhost");
    hostent* server = gethostbyname(argv[1]);
    // hostent* server = gethostbyname("127.0.0.1");
    if (server == nullptr) {
        perror("gethostbyname()");
        return 1;
    }
    printf("服务器IP: %s\n", inet_ntoa(*(in_addr*)server->h_addr));
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    int result = connect(socket_fd, (sockaddr*)&server_addr, sizeof(server_addr));
    if (result < 0) {
        perror("connect()");
    }

    // 发送消息
    send(socket_fd, "Hello, Server!", 14, 0);
    
    char buffer[1024] = {0};
    recv(socket_fd, buffer, sizeof(buffer), 0);
    std::cout << "Received from server: " << buffer << std::endl;

    close(socket_fd);
}