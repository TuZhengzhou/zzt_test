#include <iostream>
#include <thread>
#include <string>

// How to run this code:
// g++ -std=c++11 -pthread std_thread.cpp -o std_thread

int flag = 0;

int print_aux_info_when_waiting() {
    while(!flag) {
        std::cout << "Waiting for Inputs ... " << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}

typedef int (*Func)();

int main() {

    Func func = print_aux_info_when_waiting;

    std::thread t(func);

    std::cin.get();
    flag = 1;
    t.join();
    std::cout << "Exiting ..." << std::endl;

    std::cin.get();

    return 0;
}