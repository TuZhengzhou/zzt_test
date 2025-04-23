#include <iostream>
#include <string.h>
// 声明 Type array[0] 是合法的, 但是 sizeof(array) == 0

// 在结构体中, 将 Type array[0] 作为结构体 S 的最后一个成员是合法的, 称为柔性数组
// 柔性数组的长度为 0, 但是可以通过 malloc 分配内存, 用于存储数据, 并且可以通过柔性数组访问数据
// 通常在数据报文中使用柔性数组, 用于存储变长数据. 且需要一个 int 来表示柔性数组长度

struct header {
    unsigned int otherInfo;
    int len;
    char data[0];   // 柔性数组, sizeof(data) == 0. GNU C 扩展
};

int main() {
    char data[0];
    printf("%lu\n", sizeof(data));   // 0
    struct header* p = (struct header*)malloc(sizeof(struct header) + strlen("hello world") + 1);
    strcpy(p->data, "hello world");
    printf("%s\n", p->data);
    return 0;
}