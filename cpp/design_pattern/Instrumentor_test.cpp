#include <iostream>
#include <cmath>
#include "Instrumentor.h"

#define ENABLE_INSTRUMENTOR 1
#ifdef ENABLE_INSTRUMENTOR
#define PROFILE_SCOPE(name) InstrumentationTimer timer##__LINE__(name)
#define PROFILE_FUNCTION() PROFILE_SCOPE(__PRETTY_FUNCTION__) // or __FUNCSIG__ for MSVC
#else
#define PROFILE_SCOPE(name)
#define PROFILE_FUNCTION()
#endif

void FunctionA() {
    PROFILE_FUNCTION();
    // Simulate some work
    for (int i = 0; i < 100000; ++i) {
        std::cout << "Hello world #" << i << std::endl;
    }
    return;
}

void FunctionB() {
    PROFILE_FUNCTION();
    // Simulate some work
    for (int i = 0; i < 100000; ++i) {
        double a = sqrt(i);
        std::cout << "Square root of " << i << " is " << a << std::endl;
    }
    return;
}

void Function() {
    PROFILE_FUNCTION();
    // Simulate some work
    FunctionA();
    FunctionB();
    return;
}

// g++ -std=c++14 Instrumentor_test.cpp -o Instrumentor_test -O3
// ./Instrumentor_test

int main() {
    Instrumentor::Get().BeginSession("Test Session", "results.json");

    Function();

    Instrumentor::Get().EndSession();

    return 0;
}