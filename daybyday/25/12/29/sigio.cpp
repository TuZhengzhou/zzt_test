#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>

#include <atomic>
#include <cstring>
#include <iostream>
#include <thread>

constexpr int PORT = 12345;
constexpr int BUF_SIZE = 1024;

static int g_conn_fd = -1; // 服务端已建立连接的 socket

/* ================= SIGIO Handler ================= */
void sigio_handler(int signo) {
  std::cout << "[server] SIGIO handler triggered, pid = " << std::this_thread::get_id() << std::endl;

  char buf[BUF_SIZE] = {0};

  while (true) {
    ssize_t n = recv(g_conn_fd, buf, sizeof(buf) - 1, 0);
    if (n > 0) {
      std::cout << "[server] recv: " << buf << "\n";
      std::string reply = "[server] " + std::string(buf, n);
      send(g_conn_fd, reply.c_str(), reply.size(), 0);
    } else if (errno == EAGAIN) {
      std::cout << "[server] recv EAGAIN\n";
      break;
    } else {
      std::cout << "[server] recv error or close\n";
      break;
    }
  }
}

/* ================= Server Thread ================= */
void server_thread() {
  std::cout << "[server] thread start, pid: " << std::this_thread::get_id() << std::endl;

  //   sigset_t set;
  //   sigemptyset(&set);
  //   sigaddset(&set, SIGIO);
  //   pthread_sigmask(SIG_UNBLOCK, &set, nullptr);

  //   std::cout << "[server] SIGIO unblocked in server thread\n";

  int listen_fd = socket(AF_INET, SOCK_STREAM, 0);

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  

  bind(listen_fd, (sockaddr *)&addr, sizeof(addr));
  listen(listen_fd, 1);

  std::cout << "[server] waiting for connection...\n";

  g_conn_fd = accept(listen_fd, nullptr, nullptr);
  std::cout << "[server] client connected\n";

  /* 注册 SIGIO handler */
  struct sigaction sa {};
  sa.sa_handler = sigio_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGIO, &sa, nullptr);

  /* 设置 socket owner（接收 SIGIO 的进程） */
  // fcntl(g_conn_fd, F_SETOWN, getpid());
  fcntl(g_conn_fd, F_SETOWN, getpid());

  /* 启用信号驱动 I/O */
  int flags = fcntl(g_conn_fd, F_GETFL);
  fcntl(g_conn_fd, F_SETFL, flags | O_ASYNC | O_NONBLOCK);

  /* 服务端主线程仅等待信号 */
  while (true) {
    std::cout << "[server] waiting for SIGIO...\n";

    pause();
  }
}

/* ================= Client Thread ================= */
void client_thread() {
  std::cout << "[client] thread start, pid: " << std::this_thread::get_id() << std::endl;

  //   sigset_t set;
  //   sigemptyset(&set);
  //   sigaddset(&set, SIGIO);
  //   pthread_sigmask(SIG_BLOCK, &set, nullptr);
  sleep(1); // 等待服务端就绪

  int fd = socket(AF_INET, SOCK_STREAM, 0);

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

  connect(fd, (sockaddr *)&addr, sizeof(addr));
  sleep(1);

  const char *msg = "hello from client";
  send(fd, msg, strlen(msg), 0);
  printf("[client] send done\n");

  char buf[BUF_SIZE] = {0};
  ssize_t n = recv(fd, buf, sizeof(buf) - 1, 0);
  if (n > 0) {
    std::cout << "[client] received: " << buf << "\n";
  }

  close(fd);
}

/* ================= main ================= */
int main() {
  //   sigset_t set;
  //   sigemptyset(&set);
  //   sigaddset(&set, SIGIO);
  //   pthread_sigmask(SIG_BLOCK, &set, nullptr);
  std::cout << "[main] main start, pid: " << std::this_thread::get_id() << std::endl;

  std::thread srv(server_thread);
  std::thread cli(client_thread);

  cli.join();
  srv.detach();
}
