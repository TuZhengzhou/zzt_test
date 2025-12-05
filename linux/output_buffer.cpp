// 这个程序会在终端疯狂输出数据，即使你按下 Ctrl + C，它也不会立刻停，而是还要“多刷一阵子”才停下来。

// bigwrite.cpp
#include <unistd.h>     // write
#include <signal.h>     // sigaction
#include <string.h>     // memset, strerror
#include <errno.h>      // errno
#include <stdio.h>      // perror
#include <stdlib.h>     // malloc, strtoull

static volatile sig_atomic_t g_stop = 0;

static void on_sigint(int) {
    g_stop = 1;
    // 只用 async-signal-safe 的 write 发一条提示
    const char msg[] = "\n[SIGINT] received: will stop after current write...\n";
    (void)write(STDERR_FILENO, msg, sizeof(msg) - 1);
}

int main(int argc, char** argv) {
    // 默认每次写 1 MiB，可通过 argv[1] 覆盖（单位：字节）
    size_t block = 1u << 20; // 1 MiB
    if (argc > 1) {
        unsigned long long v = strtoull(argv[1], nullptr, 0);
        if (v > 0) block = (size_t)v;
    }

    // 安装 SIGINT 处理器，并开启 SA_RESTART 演示“难以立刻被打断”
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = on_sigint;
    sa.sa_flags   = SA_RESTART; // 关键：阻塞的 write 被信号打断后会自动重启
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGINT, &sa, nullptr) != 0) {
        perror("sigaction");
        return 1;
    }

    // 准备一块要写的缓冲（最后放个换行，方便视觉上看到分隔）
    char* buf = (char*)malloc(block);
    if (!buf) {
        perror("malloc");
        return 1;
    }
    memset(buf, 'A', block);
    buf[block - 1] = '\n';

    // 疯狂往 STDOUT 写大块数据
    for (;;) {
        if (g_stop) break;

        ssize_t n = write(STDOUT_FILENO, buf, block);
        if (n < 0) {
            if (errno == EINTR) {
                // 理论上有 SA_RESTART 不太会到这，但保留以示范差异
                // 再次判断停止标志后继续/退出
                continue;
            }
            perror("write");
            break;
        }
    }

    const char bye[] = "\n[Exit] done.\n";
    (void)write(STDERR_FILENO, bye, sizeof(bye) - 1);
    free((void*)buf);
    return 0;
}
