#include <arpa/inet.h>
#include <cmath>
#include <cstring>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
// #include <string>
#include <iostream>
#include <unistd.h>
#include <linux/tcp.h>

#include "../thread-pool.hpp"
#include "types.h"

long fsize(FILE *fp) {
  long n;
  fpos_t fpos;        //当前位置
  fgetpos(fp, &fpos); //获取当前位置
  fseek(fp, 0, SEEK_END);
  n = ftell(fp);
  fsetpos(fp, &fpos); //恢复之前的位置
  return n;
}

void send_sth(const int fd, Header* head,
              const char *data, const size_t data_len, const size_t buffer_size,
              const char *msg, int id) {

  size_t head_len = sizeof(Header);
  assert(buffer_size > head_len);

  char buf[buffer_size];
  size_t sended_len = 0;
  while (sended_len < data_len) {
    size_t data_size = std::min(buffer_size-head_len, data_len-sended_len);
    size_t tota_size = data_size + head_len;
    head->data_size = data_size;

    memcpy(buf, head, head_len);
    memcpy(buf + head_len, data + sended_len, data_size);

    size_t sended = ::send(fd, buf, tota_size, 0);
    printf("send_sth-%d: +%lu %lu/%lu\n", id, sended - head_len, sended_len, data_len);
    fsync(1);
    assert(sended == tota_size);

    sended_len += sended - head_len;
  }

  memset(buf, 0, buffer_size);
  ::recv(fd, buf, buffer_size-1, 0);
  printf("send_sth. send: %s, recv: %s\n", msg, buf);
  fsync(1);
};

void client(void* arg) {
  int batch_num = *(int*)arg;
  int fd = socket(AF_INET, SOCK_STREAM, 0);

  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(1027);
  hostent *ent = gethostbyname("localhost");
  memcpy(&addr.sin_addr.s_addr, ent->h_addr_list[0], ent->h_length);

  socklen_t len = sizeof(addr);
  if (connect(fd, (sockaddr *)&addr, len)) {
  }

  char buf[64];
  int uuid;
  int real_len;
  
  memset(buf, 0, 64);
  real_len = ::recv(fd, buf, 64, 0);
  assert(real_len == 4);
  memcpy(&uuid, buf, 4);
  printf("uuid = %d\n", uuid);
  Header header;
  header.uuid = uuid;

  FileHead head;

  std::string name = "example.txt";
  FILE *f = fopen(name.data(), "r");
  if (f == nullptr) {
    perror("fopen");
    exit(-1);
  }
  head.file_len = fsize(f);
  memcpy(head.file_name, name.data(), sizeof(name));
  head.batch_num = batch_num;
  send_sth(fd, &header, (char*)&head, sizeof(head), 64, "head", batch_num);

  char* content = (char*)::malloc(head.file_len);
  fread(content, sizeof(char), head.file_len, f);
  fclose(f);
  send_sth(fd, &header, (char*)content, head.file_len, 64, "content", batch_num);

  ::free(content);
  ::close(fd);
  
  delete (int*)arg;
}

int main() {
  ThreadPool pool(4, 0);
  for (int i = 0; i < 20; i++) {
    int* batch_num = new int(i);
    // pool.PushJob(client, batch_num);
    client(batch_num);
  }
  
}