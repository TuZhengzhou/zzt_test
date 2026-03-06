; filename: hello_syscall.asm
; 使用: nasm -f elf64 hello_syscall.asm -o hello_syscall.o && ld hello_syscall.o -o hello_syscall

section .data
    msg db 'Hello World using syscall!', 0xA
    len equ $ - msg

section .text
    global _start

_start:
    ; 系统调用: write(int fd, const void *buf, size_t count)
    ; write(1, msg, len)
    mov rax, 1      ; 系统调用号 1 (sys_write)
    mov rdi, 1      ; 文件描述符 1 (标准输出)
    mov rsi, msg    ; 字符串地址
    mov rdx, len    ; 字符串长度
    syscall         ; 触发系统调用

    ; 系统调用: exit(int status)
    ; exit(0)
    mov rax, 60     ; 系统调用号 60 (sys_exit)
    mov rdi, 0      ; 退出状态码
    syscall