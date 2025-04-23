#include <stdio.h>

// addr2line 默认是解析非-PIE 地址，且需要 -g 生成调试信息
// 编译参数:
//    -g 生成调试信息
//    -no-pie 禁用位置无关可执行文件
// bash:
//    g++ -g -no-pie test_addr2line.cpp -o test_addr2line
//    ./test_addr2line                   // 运行
//    dmesg | grep test_addr2line        // 记录输出中 ip 后的地址为 ADDR
//    addr2line -e test_addr2line -f -C ADDR   // 查看函数名和行号
// 
// Explain:
//    -f: 显示函数名
//    -C: 将被 C++ 编译"mangle"的符号名还原为可读的原始函数名。

int func(int a, int b)
{
  return a / b;
}

int main()
{
  int x = 10;
  int y = 0;
  printf("%d / %d = %d\n", x, y, func(x, y));
  return 0;
}