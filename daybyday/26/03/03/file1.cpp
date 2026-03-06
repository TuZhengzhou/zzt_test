#include <iostream>
#include "file1.h"

static thread_local int func_var = 1;

void func1() {
    static int func1_var = 3;
    std::cout << "a : " << func_var << std::endl;
    int a;
}