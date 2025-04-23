#include <benchmark/benchmark.h>
#include <array>

// g++ array_read_gtest.cpp -o array_read_gtest -pthread -lbenchmark -O3
// ./array_read_gtest
// ---------------------------------------------------------------
// Benchmark                     Time             CPU   Iterations
// ---------------------------------------------------------------
// bench_array_operator      0.191 ns        0.191 ns   3657952205
// bench_array_at            0.193 ns        0.193 ns   3666389942
// bench_array_get           0.282 ns        0.282 ns   2320200227
 
constexpr int len = 6;
 
// constexpr function具有inline属性，你应该把它放在头文件中
constexpr auto my_pow(const int i)
{
    return i * i;
}
 
// 使用operator[]读取元素，依次存入1-6的平方
static void bench_array_operator(benchmark::State& state)
{
    std::array<int, len> arr;
    constexpr int i = 1;
    for (auto _: state) {
        arr[5] = my_pow(i+5);
        benchmark::DoNotOptimize(arr);
    }
}
BENCHMARK(bench_array_operator);
 
// 使用at()读取元素，依次存入1-6的平方
static void bench_array_at(benchmark::State& state)
{
    std::array<int, len> arr;
    constexpr int i = 1;
    for (auto _: state) {
        arr.at(5) = my_pow(i+5);
        benchmark::DoNotOptimize(arr);
    }
}
BENCHMARK(bench_array_at);
 
// std::get<>(array)是一个constexpr function，它会返回容器内元素的引用，并在编译期检查数组的索引是否正确
static void bench_array_get(benchmark::State& state)
{
    std::array<int, len> arr;
    constexpr int i = 1;
    for (auto _: state) {
        std::get<5>(arr) = my_pow(i+5);
        benchmark::DoNotOptimize(arr);
    }
}
BENCHMARK(bench_array_get);
 
BENCHMARK_MAIN();