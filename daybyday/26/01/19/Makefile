hello_int80: hello_int80.asm
	nasm -f elf32 hello_int80.asm -o hello_int80.o && ld -m elf_i386 hello_int80.o -o hello_int80

hello_syscall: hello_syscall.asm
	nasm -f elf64 hello_syscall.asm -o hello_syscall.o && ld hello_syscall.o -o hello_syscall

PHONY: clean

clean:
	rm -rf hello_int80 hello_int80.o hello_syscall hello_syscall.o