#include <stdio.h>

int main() {
    volatile int i = 10;
    int a = i;
    printf("i = %d\n", a);

    // 使用内联汇编的约束，告诉编译器我要向 i 所在的内存写值
    __asm__ __volatile__ (
        "movl $0x20, (%0)\n\t"   // 把 0x20 写到内存地址 (%0) 中
        :                        // 无输出操作数
        : "r" (&i)               // 输入操作数：把 &i 作为寄存器 (r) 传入
        : "memory"               // 告诉编译器此汇编会改写内存
    );

    int b = i;
    printf("i = %d\n", b);
    return 0;
}


/*
尝试复现 https://www.runoob.com/w3cnote/c-volatile-keyword.html 中代码

但复现没有成功

#include <stdio.h>
 
void main()
{
    volatile int i = 10;
    int a = i;
 
    printf("i = %d", a);
    __asm {
        mov dword ptr [ebp-4], 20h
    }
 
    int b = i;
    printf("i = %d", b);
}

*/