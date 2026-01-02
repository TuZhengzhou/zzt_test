- 检查 ZOO_READONLY_STATE 符号在哪个库中定义：`nm -D /usr/lib/x86_64-linux-gnu/libzookeeper_mt.so.2 | grep ZOO_READONLY_STATE`

    - `nm`: List symbol names in object files
    - `-D`: [-D|--dynamic], set that the object file is dynamic lib
    - `--demangle|-C`: Demangle C++ symbols (make them readable)