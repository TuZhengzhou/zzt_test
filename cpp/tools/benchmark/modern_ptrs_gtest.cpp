#include <benchmark/benchmark.h>
#include <memory>

// g++ -std=c++14 -O3 modern_ptrs_gtest.cpp -o modern_ptrs_gtest -pthread -lbenchmark
// ./modern_ptrs_gtest

// Note: -O3 is important for performance testing. As we only cares the performance of release version.
// --------------------------------------------------------------
// Benchmark                    Time             CPU   Iterations
// --------------------------------------------------------------
// test_make_shared          14.9 ns         14.9 ns     46698017
// test_new_shared_ptr       18.2 ns         18.2 ns     38545937
// test_make_unique          6.12 ns         6.12 ns    112729223
// test_new_unique_ptr       6.15 ns         6.15 ns    110828235

void test_make_shared(benchmark::State& state) {
    for (auto _ : state) {
        auto ptr = std::make_shared<int>(42);
        benchmark::DoNotOptimize(ptr);
    }
}
BENCHMARK(test_make_shared);

void test_new_shared_ptr(benchmark::State& state) {
    for (auto _ : state) {
        auto ptr = std::shared_ptr<int>(new int(42));
        benchmark::DoNotOptimize(ptr);
    }
}
BENCHMARK(test_new_shared_ptr);

void test_make_unique(benchmark::State& state) {
    for (auto _ : state) {
        auto ptr = std::make_unique<int>(42);
        benchmark::DoNotOptimize(ptr);
    }
}
BENCHMARK(test_make_unique);
void test_new_unique_ptr(benchmark::State& state) {
    for (auto _ : state) {
        auto ptr = std::unique_ptr<int>(new int(42));
        benchmark::DoNotOptimize(ptr);
    }
}
BENCHMARK(test_new_unique_ptr);
BENCHMARK_MAIN();