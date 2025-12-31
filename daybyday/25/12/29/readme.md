# 今日总结

今天的 main.cpp 是对 socket 的一个测试。socket 有 5 种 IO 模型，分别是阻塞式IO，非阻塞式IO，IO复用，信号驱动IO和异步IO。

- sigio.cpp 提供了信号驱动IO的示例代码
- aio.cpp 提供了异步IO的示例代码

## 1. 回顾

**sockaddr**创建

- sockaddr_in 跟 sockaddr 有相同的大小，但内部细分了变量，方便赋值(arpa/inet.h)
- sockaddr_in.sin_family 设置inet地址家族, 选择 AF_INET
- sockaddr_in.port 设置端口，注意使用 htonl() 转换为网络端序
- gethostbyname 创建 host entry 条目, 可以方便获取 IP

**服务端**

- socket() 创建 fd, domain 选择 AF_INET, type 选择 SOCK_STEAM 则传输层使用TCP, 选择 SOCK_DGRAM 则使用UDP
- setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)); 设置端口复用, optval 值为 1
- 创建 sockaddr 设置用于监听的IP和端口
- bind() 绑定 sockaddr 和 fd
- listen() 阻塞直至连接达到
- accept() 接收连接, 并设置新连接对应的 socket
- send, recv
- close

**客户端**

- socket() 创建 fd, 与上述过程类似
- 创建 sockaddr 设置想要连接的IP和端口
- connect 连接端口, 阻塞直至连接成功或失败
- send, recv
- close

**文件描述符设置**

- int fcntl (int __fd, int __cmd, ...); 函数原型, 在 fd 上执行 cmd 命令
- fcntl(fd, F_GETFL); 获取当前 flags
- fcntl(fd, F_SETFL, flgas); 设置 flags

**套接字非阻塞设置**

- socket 是否阻塞由其文件描述符的 flags 决定
- 为 flags 添加 O_NONBLOCK, 即可设置 socket 非阻塞

## 2. 今日

### 2.1 SIGIO

sigio 需使用`<signal.h>`头文件

监控**某个文件描述符**上的**某个信号**，

**启用信号驱动IO**

让文件描述符 fd 在状态改变时，发送信号

- int flags = fcntl(fd, F_GETFL);
- fcntl(fd, F_SETFL, flags | O_ASYNC | O_NONBLOCK); O_ASYNC 用于启用信号驱动 IO, 当 socket 读写状态变化时, 产生 IO 信号 SIGIO; 信号处理函数应当尽量快, 避免阻塞, 因此设置 socket 为 O_NONBLOCK, 避免 handler 中读写 socket 时阻塞

**设置socket的信号接收者 owner**

文件描述符 fd 的信号将会发送到 fcntl 设置的进程

- fcntl(fd, F_SETOWN, getpid());    // 设置 socket 产生信号的接收线程. getpid 表示整个进程.
- SIGIO 是进程级信号, 必须设置进程而非线程进行接收
- 在多线程程序中, SIGIO 会被投递到任意一个未屏蔽 SIGIO 的线程

**设置信号动作**

- struct sigaction sa {};
- sa.sa_handler = sigio_handler; 设置 handler 函数, 信号到来时被触发
- sigemptyset(&sa.sa_mask); 设置 handler 执行期间信号屏蔽 mask, 为空则不屏蔽信号
- sa.sa_flags = 0;  使用传统信号语义, 不启用扩展行为

**为IO信号绑定信号动作**

- sigaction(SIGIO, &sa, nullptr);   // 参数二用于设置新信号动作, 参数三用于接收旧信号动作

**SIGIO handler的执行者**

- SIGIO 的 handler 不是某个固定线程执行
- 内核不会新建线程来执行 SIGIO handler, handler 一定是在某个已有线程的上下文中执行
- 内核会注入 handler

    - 若被通知进程的某个线程在用户态, 其就可能被选中用于执行 handler
    - 内核保存选中线程的上下文，跳转执行 handler(可能类似协程?), 执行结束后恢复选中线程上下文, 继续原线程逻辑

**注意事项**

- SIGIO 信号是边缘触发的, 且仅触发一次. 如果 SIGIO 在信号动作设置好前触发, 信号动作中的 handler 将永远不会被执行

### 2.2 AIO

aio 相关函数定义于 `<aio.h>`，编译时需提供 `-lrt` 链接选项（其中 rt 是 real time 的缩写）

**静态对象声明**

- static char buffer[BUF_SIZE]; 用作异步IO读操作的缓冲区，必须在异步操作完成前一直有效
- static struct aiocb cb; 异步IO控制块，同样必须在异步操作完成前一直有效
- std::memset(&cb, 0, sizeof(cb)); 缓存区清空
- std::memset(buffer, 0, sizeof(buffer)); POSIX 要求：aiocb 中未显式设置的字段必须为 0

为了保障异步操作完成前的有效性，将其声明为static，具有全局生命周期，否则可能访问失效内存导致未定义行为。

**指定异步 I/O 的核心参数**

- cb.aio_fildes = g_conn_fd;    设置要进行异步IO的文件描述符
- cb.aio_buf = buffer;          设置异步IO Buffer
- cb.aio_nbytes = BUF_SIZE;     设置异步IO Buffer Size

**设置异步IO完成通知方式**

- cb.aio_sigevent.sigev_notify = SIGEV_THREAD; 设置**完成通知策略**
- cb.aio_sigevent.sigev_notify_function = aio_completion_handler; 设置回调函数
- cb.aio_sigevent.sigev_value.sival_ptr = &cb; 传递回调上下文. 在 handler 中可用于：获取 aiocb，调用 aio_return(cb)，提交下一次 aio_read 形成链式异步

上述代码可以理解为：当异步 I/O 完成时，由内核（或 AIO 运行库）创建一个线程，调用指定的回调函数

IO包括将数据从网卡写内核缓冲区、以及从内核缓冲区写到用户缓冲区两个阶段。异步IO将等待上述两个阶段均完成，通知libc / AIO runtime 处理

SIGEV_* 的含义是 Signal Event notification type，但：SIGEV_SIGNAL 策略会发送信号；SIGEV_THREAD 策略根本不发信号，而是在I/O 完成后创建（或复用）一个普通线程，在线程中调用用户回调函数

**提交异步读请求**

- aio_read(&cb); 该函数将立即返回不会阻塞。当AIO完成时，内核将创建（或复用）线程以调用 aio_completion_handler

**异步IO完成后回调函数**

- auto* cb = static_cast<struct aiocb*>(sv.sival_ptr); 取回 aiocb 指针

- aio_return(); 返回异步IO结果。返回值含义：n > 0，成功读取 n 字节；n == 0，对端关闭（EOF）；n < 0，错误。注意，**只能调用一次**。调用后，内核 / AIO runtime 认为该请求“已回收”，再次调用行为未定义

- char* buf = (char*)cb->aio_buf; 获取返回数据, aio_buf 类型为 volatile void*，表示可能被内核并发修改，需显式 cast

- std::memset(buf, 0, BUF_SIZE); 重置缓冲区

- aio_read(cb); **链式重新提交异步读**


### 2.3 SIGIO与AIO的区别

> SIGIO的信号跟SIGEV_THREAD的通知有什么区别？为什么说SIGEV_THREAD不是信号？

1. SIGIO **内核级异步信号**（POSIX signal），由内核在 I/O 就绪时**发送**，通过**信号投递机制**进入进程，由 内核选择一个未**屏蔽**该信号的线程 执行 handler。其关键特征是：A. 异步中断当前执行流；B. 执行在 signal handler 上下文；C. 只能调用 async-signal-safe 函数，不能安全使用 malloc / cout / mutex / new

```
I/O 就绪
   ↓
kernel sends SIGIO
   ↓
interrupt normal execution
   ↓
signal handler runs
```

2. SIGEV_THREAD 不是信号，不使用 Unix 信号系统，而是POSIX AIO 定义的一种**完成通知策略**，由 libc / AIO runtime 处理。当I/O 完成后，创建（或复用）一个普通线程，并在线程中调用用户回调函数。

```
I/O 完成
   ↓
AIO runtime
   ↓
spawn worker thread     // 产生工作线程
   ↓
call completion handler
```

3. 为什么不是信号？因为它 不具备信号的任何核心属性：

| 信号特征                 | SIGIO          | SIGEV_THREAD |
| -------------------- | -------------- | ------------ |
| 内核投递                 | 是              | 否            |
| 中断执行流                | 是              | 否            |
| 使用 signal mask       | 是              | 否            |
| sigaction 处理         | 是              | 否            |
| async-signal-safe 限制 | 有              | 无            |
| 处理上下文                | signal handler | 普通线程         |

---

SIGIO 是内核信号，SIGEV_THREAD 是用户态回调机制。前者中断程序，后者像函数调用；前者危险而原始，后者安全而现代。