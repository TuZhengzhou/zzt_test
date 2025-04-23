#pragma once    
// #pragma once 是一个非标准的预处理指令，用于防止头文件被多次包含。它的作用类似于传统的 
// include guard（`#ifndef`、`#define` 和 `#endif`），但语法更简洁。

#ifndef DEF1_H
#define DEF1_H

// inline 的作用是允许在多个翻译单元中定义同一个函数或变量，而不会导致链接错误。
// 这在头文件中定义函数或变量时非常有用，因为头文件可能会被多个源文件包含。
inline int def1 = 1;
// const int def1 = 1;
// int def1 = 1;

// if we define def1 as `int def1 = 1;`, then we will get the following error: 
// /usr/bin/ld: /tmp/ccjgwSiY.o:(.data+0x0): multiple definition of `def1'; /tmp/cchCMXjY.o:(.data+0x0): first defined here

// because the def1 is defined in the header file, and the header file is included in multiple source files.
// define `int def1 = 1;` make def1 has external linkage, so it will be defined in multiple source files. Note that 在同一作用域内，变量的定义声明只能有一次 (加 external 的引用声明可以出现多次)，否则会引发重定义错误。

// so we should define def1 as `const int def1 = 1;` to avoid the error, because the const global variable has internal linkage by default. so different
// source files will have their own copy of def1.

// also we can define def1 as `inline int def1 = 1;` to avoid the error, because the inline global variable has internal linkage by default. and more over, inline 使得多个定义可以共存，编译器会随机选择其中一个作为最终的定义。

// Note: 

// 预处理器将头文件的内容包含到每个源文件中的机理如下：

// 1. **预处理阶段**：当编译器处理源文件时，预处理器会逐行扫描代码。遇到`#include "header.h"`指令时，预处理器会将`header.h`的全部内容直接插入到该指令的位置，形成一个临时的编译单元文件。
// 2. **多文件独立处理**：每个源文件（`.cpp`）在编译时都是独立处理的。若多个源文件都包含同一个头文件，则每个源文件都会在编译时生成一个包含该头文件内容的独立副本。
// 3. **物理复制行为**：这种机制本质是文本替换，头文件内容会原封不动地插入到每个包含它的源文件中，导致所有包含该头文件的源文件都拥有头文件中的完整代码副本。

// 使用`#ifndef`可避免同一源文件重复包含头文件，但无法解决多源文件包含同一头文件导致的潜在问题。C++通过默认赋予全局`const`变量内部链接性，使得每个源文件中的定义独立，从而避免违反单定义规则。若需全局共享常量，应使用`extern`声明结合单一定义（C++中更推荐使用`inline`变量或命名空间）。


struct def1
{
    /* data */
};

#endif