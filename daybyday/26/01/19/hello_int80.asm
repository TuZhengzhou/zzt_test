; filename: hello_int80.asm
; 使用: nasm -f elf32 hello_int80.asm -o hello_int80.o && ld -m elf_i386 hello_int80.o -o hello_int80

section .data
    msg db 'Hello World using int 0x80!', 0xA  ; 字符串 + 换行
    len equ $ - msg                            ; 计算字符串长度

section .text
    global _start

_start:
    ; 系统调用: write(int fd, const void *buf, size_t count)
    ; write(1, msg, len)
    mov eax, 4      ; 系统调用号 4 (sys_write)
    mov ebx, 1      ; 文件描述符 1 (标准输出)
    mov ecx, msg    ; 字符串地址
    mov edx, len    ; 字符串长度
    int 0x80        ; 触发系统调用

    ; 系统调用: exit(int status)
    ; exit(0)
    mov eax, 1      ; 系统调用号 1 (sys_exit)
    mov ebx, 0      ; 退出状态码 0
    int 0x80