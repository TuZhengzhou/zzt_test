#include <vector>
#include <iostream>
#include "func.h"

void func(int a) {
    std::cout << "[func] input a: " << a << std::endl;
}


void STRU::member_func(int a) {
    this->a = 100;
    std::cout << "[STRU::member_func]. a: " << this->a << std::endl;
}

namespace SPA {
void spa_func() {
    std::cout << "[spa_func]. " << std::endl;
}
}