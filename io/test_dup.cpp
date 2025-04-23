/*
    int dup(int oldfd);

    The  dup() system call creates a copy of the file descriptor oldfd, us‐
    ing the lowest-numbered unused file descriptor for the new descriptor.

    After a successful return, the old and new file descriptors may be used
    interchangeably.   They  refer  to  the same open file description (see
    open(2)) and thus share [file offset] and [file status flags]; for example,
    if the file offset is modified by using lseek(2) on one of the file de‐
    scriptors, the offset is also changed for the other.

    The two file descriptors do not share [file descriptor flags] (the close-
    on-exec  flag).   The close-on-exec flag (FD_CLOEXEC; see fcntl(2)) for
    the duplicate descriptor is off.

    与不使用 dup 的区别：
        资源隔离：
            每个子进程操作独立的文件描述符，避免因关闭或修改影响其他进程。
            内核socket的引用计数正确维护，直到所有副本关闭才释放资源。
        灵活性：
            子进程可单独设置非阻塞模式或调整其他socket选项。
*/

// 代码中：
// 1. 通过新旧文件描述符交替写入验证偏移量共享
// 2. 通过修改 O_APPEND 标志验证状态标志共享
// 3. 通过设置 FD_CLOEXEC 验证描述符标志独立

// **关键概念解释**：

// 1. **文件偏移量 (File Offset)**：
//    - 表示文件读/写操作开始的位置
//    - 通过 `read()`/`write()` 自动更新，或通过 `lseek()` 显式修改
//    - 共享特性：dup 产生的文件描述符共享同一个偏移量

// 2. **文件状态标志 (File Status Flags)**：
//    - 控制文件操作行为的设置（通过 `open()` 或 `fcntl(F_SETFL)` 设置）
//    - 包含：O_RDONLY、O_APPEND、O_NONBLOCK 等
//    - 共享特性：dup 产生的文件描述符共享这些标志

// 3. **文件描述符标志 (File Descriptor Flags)**：
//    - 只影响描述符本身行为的设置（通过 `fcntl(F_SETFD)` 设置）
//    - 目前唯一标志是 FD_CLOEXEC（控制 exec 时是否关闭文件描述符）
//    - 独立特性：每个文件描述符有自己独立的设置

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
    
    // - 打开一个临时文件，使用O_CREAT | O_RDWR，模式设为适当权限。 
    int fd = open("tmp.txt", O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    // - 写入一些内容到oldfd，然后读取其偏移量是否更新。
    const char* content = "Hello, world!";
    write(fd, content, strlen(content));

    off_t offset = lseek(fd, 0, SEEK_CUR);
    printf("Offset of fd after write to fd: %ld\n", offset);

    // - 调用dup得到newfd。
    int newfd = dup(fd);
    if (newfd == -1) {
        perror("dup");
        return 1;
    }

    // - 用oldfd写入更多内容，然后用newfd读取，看是否从正确的位置开始。
    offset = lseek(fd, 0, SEEK_CUR);
    printf("Offset of newfd after write to fd: %ld\n", offset);

    const char* more_content = "More content";
    write(fd, more_content, strlen(more_content));
    offset = lseek(newfd, 0, SEEK_CUR);
    printf("Offset of newfd after write more to fd: %ld\n", offset);

    lseek(newfd, 0, SEEK_SET);
    char buffer[1024] = {0};
    read(newfd, buffer, sizeof(buffer));
    printf("Read from newfd: %s\n\n", buffer);

    // - 使用fcntl获取oldfd和newfd的文件状态标志，检查是否相同。
    int flags1 = fcntl(fd, F_GETFL);
    int flags2 = fcntl(newfd, F_GETFL);
    printf("Status Flags of fd: %d\n", flags1);
    printf("Status Flags of newfd: %d\n", flags2);

    // - 修改其中一个的状态标志，比如添加O_APPEND，再检查另一个是否也改变。
    fcntl(fd, F_SETFL, flags1 | O_APPEND);
    int flags3_ = fcntl(fd, F_GETFL);
    printf("Status Flags of fd after setting fd: %d\n", flags3_);
    int flags3 = fcntl(newfd, F_GETFL);
    printf("Status Flags of newfd after setting fd: %d\n\n", flags3);

    // - 分别设置FD_CLOEXEC标志，查看是否互相不影响。
    int flags4 = fcntl(fd, F_GETFD);
    int flags5 = fcntl(newfd, F_GETFD);
    printf("File Descripter Flags of fd: %d\n", flags4);
    printf("File Descripter Flags of newfd: %d\n", flags5);

    fcntl(fd, F_SETFD, FD_CLOEXEC);
    int flags7 = fcntl(fd, F_GETFD);
    printf("File Descripter Flags of fd after setting fd: %d\n", flags7);
    int flags6 = fcntl(newfd, F_GETFD);
    printf("File Descripter Flags of newfd after setting fd: %d\n\n", flags6);

    // 关闭文件描述符, 删除临时文件
    close(fd);
    close(newfd);
    remove("tmp.txt");

    return 0;
}