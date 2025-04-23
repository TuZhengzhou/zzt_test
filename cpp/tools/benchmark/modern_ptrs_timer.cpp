#include <iostream>
#include <chrono>
#include <memory>

// g++ -std=c++14 modern_ptrs_timer.cpp -o modern_ptrs_timer -O3
// ./modern_ptrs_timer

// 确保所测试的代码是你想要测试的部分. 例如
//     在功能测试时避免测试代码被编译器优化掉.
//     在性能测试时，则应当使用-O3编译器选项来优化代码.

// 在这个例子中, 使用 -O3 编译器选项时, make_shared 会比 new + shared_ptr 快. （这才是真实的性能）
//     root@DESKTOP-2P0I081:~/code/zzt_test# g++ -std=c++14 benchmark_test.cpp -o benchmark_test -O3
//     root@DESKTOP-2P0I081:~/code/zzt_test# ./benchmark_test
//     Make_shared
//     Elapsed time: 221 microseconds (0 ms)
//     New shared_ptr
//     Elapsed time: 298 microseconds (0 ms)
//     Unique_ptr
//     Elapsed time: 82 microseconds (0 ms)
//     New unique_ptr
//     Elapsed time: 84 microseconds (0 ms)


// 不使用 -O3编译器选项时, make_shared 会比 new + shared_ptr 慢.（这是错误的性能）
//     root@DESKTOP-2P0I081:~/code/zzt_test# g++ -std=c++14 benchmark_test.cpp -o benchmark_test
//     root@DESKTOP-2P0I081:~/code/zzt_test# ./benchmark_test
//     Make_shared
//     Elapsed time: 786 microseconds (0 ms)
//     New shared_ptr
//     Elapsed time: 449 microseconds (0 ms)
//     Unique_ptr
//     Elapsed time: 661 microseconds (0 ms)
//     New unique_ptr
//     Elapsed time: 672 microseconds (0 ms)

struct Timer {
public:
    std::chrono::high_resolution_clock::time_point start;
    Timer() : start(std::chrono::high_resolution_clock::now()) {}

    ~Timer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        int64_t ms = duration / 1000;
        std::cout << "Elapsed time: " << duration << " microseconds (" << ms << " ms)" << std::endl;
    }
};

struct Vector2 {
public:
    float x, y, z;
};

int main() {

    std::cout << "Make_shared" << std::endl;
    {
        std::array<std::shared_ptr<Vector2>, 10000> vec;
        Timer t;
        for(int i = 0; i < 10000; ++i) {
            vec[i] = std::make_shared<Vector2>();
        }
    }

    std::cout << "New shared_ptr" << std::endl;
    {
        std::array<std::shared_ptr<Vector2>, 10000> vec;
        Timer t;
        for(int i = 0; i < 10000; ++i) {
            vec[i] = std::shared_ptr<Vector2>(new Vector2());
        }
    }

    std::cout << "Unique_ptr" << std::endl;
    {
        std::array<std::unique_ptr<Vector2>, 10000> vec;
        Timer t;
        for(int i = 0; i < 10000; ++i) {
            vec[i] = std::make_unique<Vector2>();
        }
    }

    std::cout << "New unique_ptr" << std::endl;
    {
        std::array<std::unique_ptr<Vector2>, 10000> vec;
        Timer t;
        for(int i = 0; i < 10000; ++i) {
            vec[i] = std::unique_ptr<Vector2>(new Vector2());
        }
    }
}