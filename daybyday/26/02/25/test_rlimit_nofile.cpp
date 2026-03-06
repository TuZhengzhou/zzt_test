
#include <cassert>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <sys/select.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>


int main(int argc, char** argv) {
  struct rlimit rlim;
  int r = getrlimit(RLIMIT_NOFILE, &rlim);
  if (r == -1) {
    perror("getrlimit");
  }

  std::vector<int> fds;
  do {
    int fd = open("test_rlimit_nofile.cpp", O_APPEND);
    printf("\t fd: %d\n", fd);
    if (fd == -1) {
      assert(errno == EMFILE);
      printf("open fail, errno = %d, EMFILE = %d\n", errno, EMFILE);
      break;
    }
  } while (true);
  getchar();
  getchar();
  std::cout << "RLIMIT_NOFILE rlim_cur = " << rlim.rlim_cur << std::endl \
            << "RLIMIT_NOFILE rlim_max = " << rlim.rlim_max << std::endl;
  for (auto fd: fds) {
    close(fd);
  }

  fd_set fd_s;
}