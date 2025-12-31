#include <aio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <thread>

constexpr int PORT = 12345;
constexpr int BUF_SIZE = 1024;

static int g_conn_fd = -1;

/* =============== AIO completion callback =============== */
void aio_completion_handler(union sigval sv) {
    auto* cb = static_cast<struct aiocb*>(sv.sival_ptr);

    ssize_t n = aio_return(cb);
    if (n <= 0) {
        std::cout << "[server] aio read error or connection closed\n";
        return;
    }

    /* 正确处理 volatile aio_buf */
    char* buf = (char*)cb->aio_buf;

    std::string msg(buf, n);
    std::cout << "[server] aio completed, recv: " << msg << "\n";

    std::string reply = "[server] " + msg;
    send(g_conn_fd, reply.c_str(), reply.size(), 0);

    /* 清空用户缓冲区，而不是 aio_buf 本身 */
    std::memset(buf, 0, BUF_SIZE);

    std::cout << "[server] resubmit aio_read\n";
    aio_read(cb);
}

/* ================= Server Thread ================= */
void server_thread() {
    std::cout << "[server] thread start\n";

    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    bind(listen_fd, (sockaddr*)&addr, sizeof(addr));
    listen(listen_fd, 1);

    std::cout << "[server] waiting for connection...\n";
    g_conn_fd = accept(listen_fd, nullptr, nullptr);
    std::cout << "[server] client connected\n";

    static char buffer[BUF_SIZE];
    static struct aiocb cb;

    std::memset(&cb, 0, sizeof(cb));
    std::memset(buffer, 0, sizeof(buffer));

    cb.aio_fildes = g_conn_fd;
    cb.aio_buf = buffer;
    cb.aio_nbytes = BUF_SIZE;

    cb.aio_sigevent.sigev_notify = SIGEV_THREAD;
    cb.aio_sigevent.sigev_notify_function = aio_completion_handler;
    cb.aio_sigevent.sigev_value.sival_ptr = &cb;

    std::cout << "[server] submit first aio_read\n";
    aio_read(&cb);

    while (true) {
        pause();
    }
}

/* ================= Client Thread ================= */
void client_thread() {
    sleep(1);

    int fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    connect(fd, (sockaddr*)&addr, sizeof(addr));

    const char* msg = "hello from client";
    std::cout << "[client] send: " << msg << "\n";
    send(fd, msg, std::strlen(msg), 0);

    char buf[BUF_SIZE] = {};
    ssize_t n = recv(fd, buf, sizeof(buf) - 1, 0);
    if (n > 0) {
        std::cout << "[client] received: " << buf << "\n";
    }

    for (int i = 0; i < 10; i++) {
        send(fd, msg, std::strlen(msg), 0);
        memset(buf, 0, BUF_SIZE);
        n = recv(fd, buf, sizeof(buf) - 1, 0);
        if (n > 0) {
            std::cout << "[client] received: " << buf << "\n";
        }
    }

    close(fd);
}

/* ================= main ================= */
int main() {
    std::thread srv(server_thread);
    std::thread cli(client_thread);

    cli.join();
    srv.detach();
    return 0;
}
