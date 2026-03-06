#!/bin/bash
# compile_and_run.sh

echo "编译测试程序..."
g++ -pthread -O2 -o context_switch_test context_switch_test.cpp

if [ $? -ne 0 ]; then
    echo "编译失败!"
    exit 1
fi

echo "运行测试..."
echo "注意：进程切换测试需要运行较长时间"
echo ""

# 运行测试
sudo ./context_switch_test  # 可能需要sudo权限来设置CPU亲和性

# 如果不需要sudo，可以注释上面一行，使用下面这行
# ./context_switch_test