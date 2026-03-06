#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <limits.h>
#include <iostream>
#include <string.h>

int main() {
    int fd = open("/root/codes/zzt_test/daybyday/26/03/05/open_compile.cpp", O_RDONLY);
    
    struct stat st;
    int ret = fstat(fd, &st);
    
    // std::cout << "fsize = " << st.st_size << std::endl;
    std::vector<char> buf_vec(st.st_size);
    ssize_t size = read(fd, buf_vec.data(), st.st_size);
    if (size == -1) {
        std::cout << "error is: " << strerror(errno) << std::endl;
    }
    std::string str(buf_vec.begin(), buf_vec.end());

    std::cout << str << std::endl;
}