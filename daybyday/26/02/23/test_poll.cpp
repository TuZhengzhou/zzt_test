#include <poll.h>
#include <limits.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/unistd.h>
#include <string.h>
#include <sstream>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <thread>

int ready = 0;
std::mutex mu;
std::condition_variable cv;

void server() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr;
    memset(&addr, 0, sizeof(sockaddr_in));
    addr.sin_family = AF_INET;

    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    addr.sin_port = htons(10270);

    if (bind(fd, (sockaddr*)&addr, sizeof(sockaddr_in))) {
        perror("bind");
        return;
    }
    listen(fd, 1024);
    pollfd fds[3];
    sockaddr_in clients[3];
    for (int i = 0; i < 3; i++) {
        memset(&fds[i], 0, sizeof(pollfd));
        socklen_t len = sizeof(sockaddr_in);
        int c = accept(fd, (sockaddr*)&clients[i], &len);
        std::cout << "\taccept: fd = " << c << ", port = " << ntohs(clients[i].sin_port) << std::endl;
        fds[i].fd = c;
        fds[i].events = POLLIN;
    }

    std::unique_lock<std::mutex> lock(mu);
    ready = 1;
    lock.unlock();
    cv.notify_all();

    int n = poll(fds, 3, -1);
    char buffer[1024];
    std::cout << "n = " << n << std::endl;
    for (int i = 0; i < 3; i++) {
        memset(buffer, 0, 1024);
        if (POLLIN & fds[i].revents) {
            size_t r = recv(fds[i].fd, buffer, 1024, 0);
            if (r > 0)
                std::cout << "\ti=" << i << ", fd: " << fds[i].fd << ", recv: " << buffer << std::endl;
        }
    }

}

void client(uint32_t i) {
    sleep(1);
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(sockaddr_in));
    addr.sin_family = AF_INET;

    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    addr.sin_port = htons(10270);
    
    if (connect(fd, (sockaddr*)&addr, sizeof(sockaddr_in))) {
        std::cout << "connect error" << std::endl;
        return;
    } 
    std::cout << "\tconnect: fd = " << fd << ", port = " << ntohs(addr.sin_port) << std::endl;
    
    std::unique_lock<std::mutex> lock(mu);
    cv.wait(lock, [](){
        return ready == 1;
    });

    if (i == 2) {
        std::stringstream ss;
        ss << "hello from " << i;
        std::string str = ss.str();

        send(fd, str.c_str(), str.size(), 0);
    }
    sleep(1);
    close(fd);
}

int main() {
    std::thread ser(server);
    std::thread c1(client, 1);
    std::thread c2(client, 2);
    std::thread c3(client, 3);
    ser.join();
    c1.join();
    c2.join();
    c3.join();
}