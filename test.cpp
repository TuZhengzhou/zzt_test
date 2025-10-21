#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <iostream>
#include <memory>

int main() {
    size_t page_size = sysconf(_SC_PAGESIZE); // 获取系统页大小（通常为4096字节）
    printf("页大小: %zu 字节\n", page_size);

    // 申请1字节内存（实际会按页分配）
    void *ptr = mmap(NULL, 1, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap失败");
        return 1;
    }

    // 检查地址是否页对齐（地址 % 页大小 == 0）
    printf("mmap返回地址: %p\n", ptr);
    printf("是否页对齐: %s\n", ((unsigned long)ptr % page_size == 0) ? "是" : "否");

    munmap(ptr, page_size); // 释放时需用实际分配的页大小
    return 0;
}