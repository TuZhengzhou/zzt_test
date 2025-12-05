#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "thread-pool.hpp"

const std::string body = "Hello, ApacheBench!\n";
const std::string response =
  "HTTP/1.1 200 OK\r\n"
  "Content-Type: text/plain\r\n"
  "Content-Length: " + std::to_string(body.size()) + "\r\n"
  "Connection: close\r\n"
  "\r\n" +
  body;

void setfdisblock(int fd, bool isblock)
{
  int flags = fcntl(fd, F_GETFL);
  if (flags < 0)
    return;
  if (isblock)
    flags &= ~O_NONBLOCK;
  else
    flags |= O_NONBLOCK;

  if (fcntl(fd, F_SETFL, flags) < 0)
    perror("fcntl set");
}

int socket_wrapper(int _level, int _optname)
{
  int listen_fd = ::socket(AF_INET, SOCK_STREAM, 0);
  if (listen_fd < 0)
  {
    perror("socket");
    exit(-1);
  }

  int opt = 1;
  if (setsockopt(listen_fd, _level, _optname, &opt, sizeof(opt)) < 0)
  {
    perror("setsockopt");
    exit(-1);
  }

  return listen_fd;
}

void bind_wrapper(int listen_fd, int port)
{
  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);

  if (bind(listen_fd, (sockaddr *)&addr, sizeof(addr)) < 0)
  {
    perror("bind");
    exit(-1);
  }
}

void listen_wrapper(int fd)
{
  if (listen(fd, 1024) < 0)
  {
    perror("listen");
    exit(-1);
  }
}

struct ServerArg {
    int epoll_fd;
    int fd;
};

void server_func(void* arg) {
  ServerArg* sarg = (ServerArg*)arg;

  epoll_event ev;
  ev.events = EPOLLOUT | EPOLLET;
  ev.data.fd = sarg->fd;
  unsigned long long sum = 0;
  for (int i = 0; i < 1000000; i++) {
      sum += i;
  }
  printf("sum = %llu\n", sum);
  epoll_ctl(sarg->epoll_fd, EPOLL_CTL_MOD, sarg->fd, &ev);

  delete sarg;
}

struct ArgTypeConn {
  int listen_fd;
  int epoll_fd;
};

void process_connect(ArgTypeConn& conn_arg) {
  // 接收所有等待的连接（ET模式下要循环）
  while (true)
  {
    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(conn_arg.listen_fd, (sockaddr *)&client_addr, &client_len);
    if (client_fd < 0)
    {
      if (errno == EAGAIN || errno == EWOULDBLOCK)
        break;
      perror("accept");
      break;
    }

    setfdisblock(client_fd, false);
    epoll_event ev;
    ev.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
    ev.data.fd = client_fd;
    if (epoll_ctl(conn_arg.epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) < 0)
    {
      perror("epoll_ctl add client_fd");
      close(client_fd);
      continue;
    }

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
    std::cout << "Accepted connection from " << client_ip << ":" << ntohs(client_addr.sin_port) << std::endl;
  }
}

struct ArgTypeRdHup {
  int fd;
};
void process_rdhup(ArgTypeRdHup& rdhup_arg) {
  // 客户端关闭
  std::cout << "EPOLLRDHUP. Client closed fd " << rdhup_arg.fd << std::endl;
  close(rdhup_arg.fd);
}

struct ArgTypeIN {
  int fd;
  int epoll_fd;
  ThreadPool* pool_ptr;
};

void process_in(ArgTypeIN& in_arg) {
  // 循环读取所有可读数据（ET模式）
  char buffer[4096];
  while (true)
  {
    ssize_t n = read(in_arg.fd, buffer, sizeof(buffer));
    if (n > 0)
    {
      // 收到HTTP请求后直接注册写事件
      ServerArg* sarg = new ServerArg;
      sarg->epoll_fd = in_arg.epoll_fd;
      sarg->fd = in_arg.fd;
      in_arg.pool_ptr->PushJob(server_func, sarg);
      // server_func(sarg);
    }
    else if (n == 0)
    {
      // 客户端关闭
      std::cout << "Read return 0. Client closed fd " << in_arg.fd << std::endl;
      close(in_arg.fd);
      break;
    }
    else
    {
      if (errno == EAGAIN || errno == EWOULDBLOCK)
        break;
      perror("read");
      close(in_arg.fd);
      break;
    }
  }
  printf("Recv: %s\n", buffer);
}

struct ArgTypeOut {
  int fd;
  int epoll_fd;
  epoll_event* event_ptr;
};

void process_out(ArgTypeOut& out_arg) {
  ssize_t total = 0;
  while (total < (ssize_t)response.size())
  {
    ssize_t n = write(out_arg.fd, response.data() + total, response.size() - total);
    printf("write fd: %d, data size = %ld\n", out_arg.fd, n);
    if (n > 0)
      total += n;
    else if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
      break;
    else
    {
      perror("write");
      close(out_arg.fd);
      total = -1;
      break;
    }
  }

  if (total >= 0)
  {
      epoll_ctl(out_arg.epoll_fd, EPOLL_CTL_DEL, out_arg.fd, out_arg.event_ptr);
      ::close(out_arg.fd);
  }
}

int main(int argc, char *argv[])
{
  ThreadPool pool(4, 0);
  int port = 8080;
  if (argc >= 2)
    port = std::stoi(argv[1]);

  int listen_fd = socket_wrapper(SOL_SOCKET, SO_REUSEADDR);
  setfdisblock(listen_fd, false);
  bind_wrapper(listen_fd, port);
  listen_wrapper(listen_fd);
  std::cout << "Listening fd " << listen_fd << " on port " << port << "..." << std::endl;

  int epoll_fd = epoll_create1(0);
  if (epoll_fd < 0)
  {
    perror("epoll_create1");
    return -1;
  }

  epoll_event listen_event;
  listen_event.events = EPOLLIN | EPOLLET;
  listen_event.data.fd = listen_fd;
  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &listen_event) < 0)
  {
    perror("epoll_ctl add listen_fd");
    return -1;
  }

  std::vector<epoll_event> events(10240);

  while (true)
  {
    int nready = epoll_wait(epoll_fd, events.data(), events.size(), -1);
    if (nready == -1) {
      if (errno == EINTR)
        continue;
      perror("epoll_wait");
      break;
    }

    for (int i = 0; i < nready; ++i)
    {
      int fd = events[i].data.fd;

      if (fd == listen_fd)
      {
        ArgTypeConn conn_arg;
        conn_arg.listen_fd = listen_fd;
        conn_arg.epoll_fd = epoll_fd;
        process_connect(conn_arg);
      }
      else if (events[i].events & EPOLLRDHUP) {
        ArgTypeRdHup rdhup_arg;
        rdhup_arg.fd = fd;
        process_rdhup(rdhup_arg);
      }
      else if (events[i].events & EPOLLIN)
      {
        ArgTypeIN in_arg;
        in_arg.epoll_fd = epoll_fd;
        in_arg.fd = fd;
        in_arg.pool_ptr = &pool;
        process_in(in_arg);
      }
      else if (events[i].events & EPOLLOUT)
      {
        ArgTypeOut out_arg;
        out_arg.fd = fd;
        out_arg.epoll_fd = epoll_fd;
        out_arg.event_ptr = &events[i];
        process_out(out_arg);
      }
      else if (events[i].events & (EPOLLHUP | EPOLLERR))
      {
        std::cout << "EPOLLHUP/ERR on fd " << fd << std::endl;
        close(fd);
      }
    }
  }

  close(listen_fd);
  close(epoll_fd);
  return 0;
}


// #include <arpa/inet.h>
// #include <fcntl.h>
// #include <netinet/in.h>
// #include <sys/epoll.h>
// #include <sys/socket.h>
// #include <unistd.h>

// #include <cstring>
// #include <iostream>
// #include <memory>
// #include <string>
// #include <vector>

// // 准备一个固定 HTTP 响应
// const std::string body = "Hello, ApacheBench!\n";
// const std::string response =
//   "HTTP/1.1 200 OK\r\n"
//   "Content-Type: text/plain\r\n"
//   "Content-Length: " + std::to_string(body.size()) + "\r\n"
//   "Connection: close\r\n"
//   "\r\n" +
//   body;

// void setfdisblock(int fd, bool isblock)
// {
//   int flags = fcntl(fd, F_GETFL);
//   if(flags < 0)
//     return;
//   if(isblock) // 阻塞
//   {
//     flags &= ~O_NONBLOCK;
//   }
//   else // 非阻塞
//   {
//     flags |= O_NONBLOCK;
//   }  
  
//   if(fcntl(fd, F_SETFL, flags)<0)
//     perror("fcntl set");
// }

// int socket_wrapper(int _level, int _optname) {
//   int listen_fd = ::socket(AF_INET, SOCK_STREAM, 0);
//   if (listen_fd < 0) {
//     perror("socket");
//     exit(-1);
//   }

//   // 允许端口复用，方便重新启动服务器
//   int opt = 1;
//   if (setsockopt(listen_fd, _level, _optname, &opt, sizeof(opt)) < 0) {
//     perror("setsockopt");
//     exit(-1);
//   }

//   return listen_fd;
// }

// void bind_wrapper(int listen_fd, int port) {
//   std::unique_ptr<sockaddr_in> addr = std::make_unique<sockaddr_in>();
//   addr->sin_family = AF_INET;
//   addr->sin_addr.s_addr = htonl(INADDR_ANY);
//   addr->sin_port = htons(port);

//   if (bind(listen_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
//     perror("bind");
//     exit(-1);
//   }
// }

// void listen_wrapper(int fd) {
//   if (listen(fd, 1024) < 0) {
//     perror("listen");
//     exit(-1);
//   }
// }

// int main(int argc, char* argv[]) {
//   int port = 8080;
//   if (argc >= 2) {
//     port = std::stoi(argv[1]);
//   }

//   int listen_fd = socket_wrapper(SOL_SOCKET, SO_REUSEADDR);
//   bind_wrapper(listen_fd, port);
//   listen_wrapper(listen_fd);
//   std::cout << "Listening fd " << listen_fd << " on port " << port << "..." << std::endl;

//   int epoll_fd = epoll_create1(0);
//   epoll_event listen_event;
//   listen_event.events = EPOLLIN | EPOLLET;
//   listen_event.data.fd = listen_fd;
//   if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &listen_event) < 0) {
//     perror("epoll_ctl");
//     return -1;
//   }

//   std::vector<epoll_event> events;

//   int acc_cnt = 0;
//   while (true) {
//     int nready = epoll_wait(epoll_fd, &events[0], -1, -1);
//     if(nready == -1)
//     {
//       perror("epoll_wait");
//       exit(-1);
//     }

//     for (int i = 0; i < nready; i++) {
//       if (events[i].data.fd == listen_fd) {
//         int client = 
//       }
//     }
    
//   }

  

//   ::close(listen_fd);
//   return 0;
// }
