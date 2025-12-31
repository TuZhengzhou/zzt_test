#include <chrono>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>   // open()
#include <iostream>
#include <unistd.h>  // read(), close(), sleep()

void call_funcs_in_unistd() {

  auto begin = std::chrono::steady_clock::now();
  int fd = open(__FILE__, O_RDONLY);

  int len = lseek(fd, 0, SEEK_END); // get file size
  lseek(fd, 0, SEEK_SET);           // recover file offset

  char *buffer = (char *)malloc(len + 1);
  memset(buffer, 0, len + 1);
  long rdlen = read(fd, buffer, len);
  printf("we use open, and read %s for %ld bytes\n %s\n", __FILE__, rdlen,
         (char *)nullptr);

  sleep(1);
  auto end = std::chrono::steady_clock::now();
  long duration_millisecs =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - begin)
          .count();
  printf("sleep for 1 second, but the real cost is %ld millisecs, read takes "
         "%ld millisecs\n",
         duration_millisecs, duration_millisecs - 1000);

  close(fd);
}

#include <stdio.h>

void call_funcs_in_stdio() {

  auto begin = std::chrono::steady_clock::now();
  FILE *f = fopen(__FILE__, "rw");
  fseek(f, 0, SEEK_END);
  long len = ftell(f);
  fseek(f, 0, SEEK_SET);

  char *buffer = (char *)malloc(len + 1);
  memset(buffer, 0, len + 1);
  fread(buffer, sizeof(char), len, f);
  printf("we use open, and read %s for %ld bytes\n %s\n", __FILE__, len,
         (char *)nullptr);

  sleep(1);
  auto end = std::chrono::steady_clock::now();
  long duration_millisecs =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - begin)
          .count();
  printf("sleep for 1 second, but the real cost is %ld millisecs, read takes "
         "%ld millisecs\n",
         duration_millisecs, duration_millisecs - 1000);

  fclose(f);
}

int main() {
  call_funcs_in_unistd();
  call_funcs_in_stdio();
}
