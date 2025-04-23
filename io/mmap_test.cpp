#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

int main() {
    int fd = open("savetime.txt", O_RDWR);

    unsigned char* addr = (unsigned char*)mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // 内存映射后, 文件内容会被映射到内存中, 通过addr指针可以直接访问文件内容
    // 可以直接修改文件内容, 即使不通过显式的write函数
    // 通过 DMA 方式, 读写文件内容, 无需 CPU 参与, 效率更高

    for(int i = 0; i < 20; i++) {
        *(addr + i) = 'a' + i;
    }

    printf("lc_54.cpp: %s\n", addr);

    // char buf[4096];
    // read(fd, buf, 4096);
    // printf("savetime.txt: %s\n", buf);

    return 0;
}