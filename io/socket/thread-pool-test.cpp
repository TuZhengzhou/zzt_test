#include <iostream>
#include <vector>

#include "thread-pool.hpp"

void func(void* args) {
    printf("%d\n", *(int*)args);
}

int main() {
    ThreadPool pool(4, 0);

    int upper = 10000;
    std::vector<int> vec(upper);
    
    for (int i = 0; i < upper; i++) {
        vec[i] = i;
        pool.PushJob(func, &vec[i]);
    }
}