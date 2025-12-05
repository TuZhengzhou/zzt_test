#include <atomic>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <map>
#include <memory>
#include <mutex>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

#include "types.h"
#include "../thread-pool.hpp"

std::map<int, File> conn_map_;
std::mutex conn_map_mutex_;
std::atomic<int> uid(1027);

SharedFD ToSharedFD(int fd) {
  int *ptr = new int(fd);
  if (ptr == nullptr) {
    perror("ToSharedFD. ptr == nullptr");
    exit(-1);
  }
  return std::shared_ptr<int>(ptr, [](int *ptr) {
    if (ptr != nullptr) {
      printf("close fd %d\n", *ptr);
      ::close(*ptr);
      delete ptr;
    }
  });
}

SharedFILE ToSharedFile(FILE *ptr) {
  return std::shared_ptr<FILE>(ptr, [](FILE *ptr) {
    if (ptr != nullptr) {
      printf("close FILE\n");
      ::fclose(ptr);
    }
  });
}

SharedFD SockInit() {
  int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
  SharedFD listen_shared = ToSharedFD(listen_fd);

  if (-1 == *listen_shared.get()) {
    perror("socket.");
    fsync(2);
    exit(-1);
  }

  int val = 1;
  setsockopt(*listen_shared.get(), SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(1027);
  hostent *ent = gethostbyname("localhost");
  memcpy(&addr.sin_addr.s_addr, ent->h_addr, ent->h_length);
  if (bind(*listen_shared.get(), (struct sockaddr *)&addr, sizeof(addr))) {
    perror("bind.");
    fsync(2);
    exit(-1);
  }

  if (listen(*listen_shared.get(), 1024)) {
    perror("listen.");
    fsync(2);
    exit(-1);
  }
  return listen_shared;
}

SharedFD EpollInit(SharedFD listen_shared) {
  int epoll_fd = epoll_create1(0);
  SharedFD epoll_shared = ToSharedFD(epoll_fd);

  if (*epoll_shared.get() < 0) {
    perror("epoll_create1");
    exit(-1);
  }

  epoll_event event;
  event.events = EPOLLIN | EPOLLET;
  event.data.fd = *listen_shared.get();
  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, *listen_shared.get(), &event)) {
    perror("epoll_ctl");
  }
  return epoll_shared;
}

// void *recv_sth(int fd, size_t head_len, const size_t data_len,
//                const size_t buffer_size, const char *msg) 
// {
//   int  
// }

int connect_proc(SharedFD &epoll_shared, SharedFD &listen_shared) {
  sockaddr_in addr;
  socklen_t len = sizeof(addr);
  int client_fd = ::accept(*listen_shared.get(), (sockaddr *)&addr, &len);
  if (client_fd < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK)
      return -1;
    perror("accept");
    return -1;
  }

  char buf[64];
  memcpy(buf, &uid, sizeof(uid));
  ::send(client_fd, buf, sizeof(uid), 0); // 先发送唯一标识符

  epoll_event event;
  event.events = EPOLLET | EPOLLIN | EPOLLRDHUP;
  event.data.fd = client_fd;
  if (epoll_ctl(*epoll_shared.get(), EPOLL_CTL_ADD, client_fd, &event)) {
    perror("epoll_ctl");
  }

  std::unique_lock<std::mutex> lock(conn_map_mutex_);
  auto iter = conn_map_.find(uid);
  assert(iter == conn_map_.end());

  // 加入新的文件
  File file;
  memset(&file, 0, sizeof(file));
  file.head_size = sizeof(FileHead);
  conn_map_[uid] = file;
  uid++;

  return 0;
}

// void recv_uuid() {

// }

void recv_header(const epoll_event& event, File& file, char* data, int data_len) {

  memcpy((char *)&file.head + file.head_current, data, data_len);
  file.head_current += data_len;

  printf("server::head +%d, %d/%d\n", data_len, file.head_current,
         file.head_size);

  if (file.head_current == file.head_size) {
    char send_buf[64] = "server::recieve head OK\n";
    ::send(event.data.fd, send_buf, 64, 0);

    file.content_size = file.head.file_len;
    file.content_current = 0;
    file.content = (char *)malloc(file.content_size);
    if (file.content == nullptr) {
      perror("malloc");
    }
  }
}

void recv_content(SharedFD epoll_shared, const epoll_event& event, File& file, char* data, int data_len) {
  char buf[64];
  memcpy(buf, data, data_len);
  buf[data_len] = 0;
  printf("server::content-%lu, %s\n", file.head.batch_num, buf);

  memcpy((char *)file.content + file.content_current, data, data_len);
  file.content_current += data_len;
  printf("server::content-%lu, %d/%d\n", file.head.batch_num,
         file.content_current, file.content_size);

  if (file.content_current == file.content_size) {
    char send_buf[64] = "server::recieve head OK\n";
    ::send(event.data.fd, send_buf, 64, 0);

    std::string name = file.head.file_name;
    name += std::to_string(file.head.batch_num);

    FILE* f = ::fopen(name.data(), "w");
    if (f == nullptr) {
      printf("f open fail\n");
      exit(-1);
    }

    std::unique_ptr<FILE, void (*)(FILE *)> f_unique(f, [](FILE *ptr) {
      if (ptr != nullptr)
        ::fclose(ptr);
    });
    char buf[file.content_size + 1];
    memcpy(buf, file.content, file.content_size);
    buf[file.content_size] = 0;

    // printf("fwrite-%lu: ", file.head.batch_num);
    // for (int i = 0; i < file.content_size; i++) {
    //   printf("[%d-%c]", *(char*)(file.content + i), *(char*)(file.content + i));
    // }

    printf("\n");
    fwrite(file.content, sizeof(char), file.content_size, f_unique.get());

    free(file.content);
    file.content = nullptr;
    epoll_ctl(*epoll_shared.get(), EPOLL_CTL_DEL, event.data.fd, nullptr);
    ::close(event.data.fd);

    // std::unique_lock<std::mutex> lock(conn_map_mutex_);
    // conn_map_.erase(event.data.fd);
  }
}

void recv_message(int fd, char* const buf, const size_t buffer_size) {
  size_t recv_hd = 0;
  while (recv_hd < sizeof(Header)) {
    int len = ::recv(fd, buf+recv_hd, sizeof(Header) - recv_hd, 0);
    assert(len >= 0);
    recv_hd += len;
  }
  Header* header = (Header*)buf;
  int data_len = header->data_size;

  int recv_con = 0;
  while (recv_con < data_len) {
    int len = ::recv(fd, buf + sizeof(header) + recv_con, data_len - recv_con, 0);
    assert(len >= 0);
    recv_con += len;
  }
}

void do_server_proc(SharedFD epoll_shared, const epoll_event& event) {
  char buf[64];
  while (true) {
    memset(buf, 0, 64);
    recv_message(event.data.fd, buf, 64);
    Header* header = (Header*)buf;

    char* data = buf + sizeof(Header);
    int data_len = header->data_size;
    int uuid = header->uuid;

    std::unique_lock<std::mutex> lock(conn_map_mutex_);
    auto iter = conn_map_.find(uuid);
    assert(iter != conn_map_.end());

    File &file = iter->second;
    if (file.head_current < file.head_size) {
      recv_header(event, file, data, data_len);
    } else if (file.content_current < file.content_size) {
      recv_content(epoll_shared, event, file, data, data_len);
      if (file.content_current == file.content_size) {
        break;
      }
    } else {
      perror("recv from a finished socket\n");
      exit(-1);
    }
  }
}

void server_proc(epoll_event *events, SharedFD epoll_shared,
                 SharedFD listen_shared, int n) {
  for (int i = 0; i < n; i++) {
    if (events[i].data.fd == *listen_shared.get()) {
      if (connect_proc(epoll_shared, listen_shared) < 0) {
        break;
      }
    } else if (events[i].events & EPOLLRDHUP) {
      ::close(events[i].data.fd);
      std::unique_lock<std::mutex> lock(conn_map_mutex_);
      conn_map_.erase(events[i].data.fd);
    } else if (events[i].events & EPOLLIN) {
      do_server_proc(epoll_shared, events[i]);
    }
  }
}

void server() {

  SharedFD listen_shared = SockInit();

  int flags = fcntl(*listen_shared.get(), F_GETFL);
  if (flags < 0) {
    perror("fcntr");
    exit(-1);
  }
  fcntl(*listen_shared.get(), F_SETFL, flags | SOCK_NONBLOCK);

  SharedFD epoll_shared = EpollInit(listen_shared);

  epoll_event events[1024];
  while (true) {
    size_t n = epoll_wait(*epoll_shared.get(), events, 1024, 0);
    server_proc(events, epoll_shared, listen_shared, n);
  }
}

int main() { server(); }