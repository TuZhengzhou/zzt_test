#include <string>
#include <vector>
#include <iterator>
// #include <format>	// C++20, gcc 13+, clang 16+
#include <benchmark/benchmark.h>
#include <fmt/core.h>
#include <fmt/printf.h>

// git clone https://github.com/google/benchmark.git
// cd benchmark
// git clone https://github.com/google/googletest.git googletest
// mkdir build && cd build
// cmake -DCMAKE_BUILD_TYPE=RELEASE ..
// make -j4
// # 如果想全局安装就接着运行下面的命令
// sudo make install
//
// git clone https://github.com/fmtlib/fmt.git
// cd fmt
// mkdir build && cd build
// cmake -DCMAKE_BUILD_TYPE=RELEASE ..
// make -j4
// # 如果想全局安装就接着运行下面的命令
// sudo make install

// ➜  benchmark git:(main) ✗ ./format_test
// 2025-09-11T20:03:36+08:00
// Running ./format_test
// Run on (4 X 3187.21 MHz CPU s)
// CPU Caches:
//   L1 Data 48 KiB (x2)
//   L1 Instruction 32 KiB (x2)
//   L2 Unified 1280 KiB (x2)
//   L3 Unified 18432 KiB (x1)
// Load Average: 0.48, 0.28, 0.17
// ---------------------------------------------------------------------
// Benchmark                           Time             CPU   Iterations
// ---------------------------------------------------------------------
// test_sprintf                      129 ns          129 ns      5555238
// test_fmt_sprintf                 8.32 ns         8.32 ns     80703635
// test_fmt_format                   101 ns          101 ns      7023518
// test_fmt_format_to                222 ns          222 ns      3096332
// test_fmt_format_to_reserve        116 ns          116 ns      6031464

// g++ format_test.cpp -o format_test -std=c++2a -pthread -lbenchmark -lfmt -O3
// ./format_test

void test_sprintf(benchmark::State &state)
{
	for (auto _ : state)
	{
		char buffer[128];
		sprintf(buffer, "Value: %d, Float: %.2f, String: %s", 1, 3.14159, "hello");
		buffer[0] = 'a';
	}
}

void test_fmt_sprintf(benchmark::State &state)
{
	for (auto _ : state)
	{
		char buffer[128];
		fmt::sprintf(buffer, "Value: %d, Float: %.2f, String: %s", 1, 3.14159, "hello");
		buffer[0] = 'a';
	}
}

// std::format is available in C++20 and later
// format is type-safe.
// sprintf is not type-safe, although is faster.
// sprintf_s is safer than sprintf, but only available in MSVC, not gcc/clang.
void test_fmt_format(benchmark::State &state)
{
	for (auto _ : state)
	{
		auto result = fmt::format("Value: {}, Float: {:.2f}, String: {}", 1, 3.14159, "hello");
		result[0] = 'a';
	}
}

void test_fmt_format_to(benchmark::State &state)
{
	for (auto _ : state)
	{
		std::vector<char> vec;
		fmt::format_to(std::back_inserter(vec), "Value: {}, Float: {:.2f}, String: {}", 1, 3.14159, "hello");
		vec.push_back('\0');
	}
}

void test_fmt_format_to_reserve(benchmark::State &state)
{
	for (auto _ : state)
	{
		std::vector<char> vec;
		vec.reserve(64);
		fmt::format_to(std::back_inserter(vec), "Value: {}, Float: {:.2f}, String: {}", 1, 3.14159, "hello");
		vec.push_back('\0');
	}
}

// void test_std_format(benchmark::State &state)
// {
// 	for (auto _ : state)
// 	{
// 		auto result = std::format("Value: {}, Float: {:.2f}, String: {}", 1, 3.14159, "hello");
// 		result[0] = 'a';
// 	}
// }

// void test_std_format_to(benchmark::State &state)
// {
//   for (auto _ : state)
//   {
//     std::vector<char> vec;
//     std::format_to(std::back_inserter(vec), "Value: {}, Float: {:.2f}, String: {}", 1, 3.14159, "hello");
//     vec.push_back('\0');
//   }
// }

// void test_std_format_to_reserve(benchmark::State &state)
// {
//   for (auto _ : state)
//   {
//     std::vector<char> vec;
//     vec.reserve(64);
//     std::format_to(std::back_inserter(vec), "Value: {}, Float: {:.2f}, String: {}", 1, 3.14159, "hello");
//     vec.push_back('\0');
//   }
// }

BENCHMARK(test_sprintf);
BENCHMARK(test_fmt_sprintf);
BENCHMARK(test_fmt_format);
BENCHMARK(test_fmt_format_to);
BENCHMARK(test_fmt_format_to_reserve);
// BENCHMARK(test_std_format);
// BENCHMARK(test_std_format_to);
// BENCHMARK(test_std_format_to_reserve);

BENCHMARK_MAIN();