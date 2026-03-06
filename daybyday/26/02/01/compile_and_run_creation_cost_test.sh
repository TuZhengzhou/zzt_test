#!/bin/bash
# compile_and_run.sh

echo "=== 进程和线程创建开销测试 ==="
echo "编译中..."

# 编译
g++ -pthread -O2 -o creation_cost_test creation_cost_test.cpp

if [ $? -ne 0 ]; then
    echo "编译失败!"
    exit 1
fi

echo "编译成功!"
echo ""
echo "运行测试 (可能需要sudo权限)..."
echo ""

# 运行测试
sudo ./creation_cost_test

# 或者不需要sudo运行
# ./creation_cost_test

echo ""
echo "测试完成!"