#include <iostream>
#include <thread>
#include <vector>

int global_1 = 12;
int global_2;

void func(int id) {
    thread_local static int thread_local_1 = 3;
    thread_local static int thread_local_;
    static int static_1 = 24;
    static int static_2;
    printf("input id = %d\n", id);

    int local = 3;
}

int main() {
    std::vector<std::thread> vec;
    for (int i = 0; i < 3; i++) {
        vec.emplace_back(std::thread(func, i));
    }
    for (auto& t: vec) {
        t.join();
    }
    func(4);
}