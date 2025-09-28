// x86-64 Linux, C++20, freestanding demo
// 仅使用内联汇编发起系统调用：write(1, msg, len) 与 exit(0)

/*
g++ -std=c++20 -ffreestanding -fno-exceptions -fno-rtti \
    -fno-stack-protector -nostdlib -nostartfiles -nodefaultlibs \
    -no-pie -Wl,-e,_start -s \
    freestanding_hello.cpp -o freestanding_hello

./freestanding_hello


验证“独立式”特征的小技巧
ldd ./freestanding_hello
# 预期输出：    not a dynamic executable

*/

extern "C" void _start();  // 链接入口

namespace {

// 三参数系统调用（如 write）
inline long syscall3(long n, long a1, long a2, long a3) {
    long ret;
    asm volatile (
        "syscall"
        : "=a"(ret)
        : "a"(n), "D"(a1), "S"(a2), "d"(a3)
        : "rcx", "r11", "memory"
    );
    return ret;
}

// 退出系统调用（SYS_exit = 60）
[[noreturn]] inline void sys_exit(int code) {
    asm volatile (
        "syscall"
        :
        : "a"(60), "D"(code)   // rax=60, rdi=code
        : "rcx", "r11", "memory"
    );
    __builtin_unreachable();
}

} // namespace

extern "C" void _start() {
    static const char msg[] = "Hello from freestanding C++\n";

    // SYS_write = 1, rdi=fd(1=stdout), rsi=buf, rdx=len
    syscall3(1, 1, (long)msg, sizeof(msg) - 1);

    sys_exit(0);
}
