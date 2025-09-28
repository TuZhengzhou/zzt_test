#include <string>
#include <vector>
#include <iterator>
#include <format>	// C++20, gcc 13+, clang 16+
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

// ---------------------------------------------------------------------
// Benchmark                           Time             CPU   Iterations
// ---------------------------------------------------------------------
// test_sprintf                      119 ns          119 ns      5712845
// test_fmt_sprintf                 8.27 ns         8.27 ns     85166117
// test_fmt_format                  98.4 ns         98.4 ns      7159363
// test_fmt_format_to                187 ns          187 ns      3758547
// test_fmt_format_to_reserve        113 ns          113 ns      5918464
// test_std_format                   114 ns          114 ns      6196429
// test_std_format_to                196 ns          196 ns      3614934
// test_std_format_to_reserve        117 ns          117 ns      595145

// g++ format_test.cpp -o format_test -std=c++20 -pthread -lbenchmark -lfmt -O3
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

void test_std_format(benchmark::State &state)
{
	for (auto _ : state)
	{
		auto result = std::format("Value: {}, Float: {:.2f}, String: {}", 1, 3.14159, "hello");
		result[0] = 'a';
	}
}

void test_std_format_to(benchmark::State &state)
{
  for (auto _ : state)
  {
    std::vector<char> vec;
    std::format_to(std::back_inserter(vec), "Value: {}, Float: {:.2f}, String: {}", 1, 3.14159, "hello");
    vec.push_back('\0');
  }
}

void test_std_format_to_reserve(benchmark::State &state)
{
  for (auto _ : state)
  {
    std::vector<char> vec;
    vec.reserve(64);
    std::format_to(std::back_inserter(vec), "Value: {}, Float: {:.2f}, String: {}", 1, 3.14159, "hello");
    vec.push_back('\0');
  }
}

BENCHMARK(test_sprintf);
BENCHMARK(test_fmt_sprintf);
BENCHMARK(test_fmt_format);
BENCHMARK(test_fmt_format_to);
BENCHMARK(test_fmt_format_to_reserve);
BENCHMARK(test_std_format);
BENCHMARK(test_std_format_to);
BENCHMARK(test_std_format_to_reserve);

BENCHMARK_MAIN();