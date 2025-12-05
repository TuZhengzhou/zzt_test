### 使用 nm 查看二进制文件的符号表

```
make
nm test
```

- D: 数据段，全局初始化数据
- B: BSS 段，全局未初始化数据
- d: 线程数据段，threadlocal 静态初始化数据
- b: 线程BSS段，threadlocal 静态未初始化数据

不止是 threadlocal 静态初始化/未初始化数据在 d/b 段，静态初始化/未初始化数据也在 d/b 段