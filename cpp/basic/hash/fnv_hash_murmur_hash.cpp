#include <cstdint>
#include <string>
#include <iostream>
#include <string>
#include <unordered_set>
#include <random>

// g++ -std=c++14 -O3 fnv_hash_murmur_hash.cpp -o ./build/fnv_hash_murmur_hash -pthread -lbenchmark

// --------------------------------------------------------------------
// Benchmark                          Time             CPU   Iterations
// --------------------------------------------------------------------
// test_fnv1a_hash                 46.0 ns         46.0 ns     14807391
// test_murmurhash3_32             15.1 ns         15.1 ns     46240606
// test_fetch_memory_by_one        4.47 ns         4.47 ns    160078374
// test_fetch_memory_by_four       2.09 ns         2.09 ns    335941358

uint32_t fnv1a_hash(const std::string& str) {
    const uint32_t offset_basis = 2166136261;
    const uint32_t fnv_prime = 16777619;
    const uint8_t* data = reinterpret_cast<const uint8_t*>(str.data());
    const size_t len = str.size();

    uint32_t hash = offset_basis;
    for (size_t i = 0; i < len; ++i) {
        hash ^= data[i];
        hash *= fnv_prime;
    }
    return hash;
}

uint32_t murmurhash3_32(const std::string& str) {
    const uint8_t* data = reinterpret_cast<const uint8_t*>(str.data());
    const size_t len = str.size();
    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;
    const uint32_t r1 = 15;
    const uint32_t r2 = 13;
    const uint32_t m = 5;
    const uint32_t n = 0xe6546b64;

    uint32_t hash = 0;
    const size_t nblocks = len / 4;
    const uint32_t* blocks = reinterpret_cast<const uint32_t*>(data);

    // 处理每个 4 字节块.
    for (size_t i = 0; i < nblocks; i++) {
        uint32_t k = blocks[i];
        k *= c1;
        k = (k << r1) | (k >> (32 - r1));
        k *= c2;

        hash ^= k;
        hash = (hash << r2) | (hash >> (32 - r2));
        hash = hash * m + n;
    }

    // 处理剩余字节
    const uint8_t* tail = data + nblocks * 4;
    uint32_t k1 = 0;
    switch (len & 3) {
        case 3: k1 ^= tail[2] << 16;
        case 2: k1 ^= tail[1] << 8;
        case 1: k1 ^= tail[0];
            k1 *= c1;
            k1 = (k1 << r1) | (k1 >> (32 - r1));
            k1 *= c2;
            hash ^= k1;
    }

    // 最终混合
    hash ^= len;
    hash ^= (hash >> 16);
    hash *= 0x85ebca6b;
    hash ^= (hash >> 13);
    hash *= 0xc2b2ae35;
    hash ^= (hash >> 16);

    return hash;
}


// 生成随机字符串
std::string generateRandomString(size_t length) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string result;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, sizeof(alphanum) - 2);

    for (size_t i = 0; i < length; ++i) {
        result += alphanum[dis(gen)];
    }
    return result;
}

// 测试哈希碰撞率
void testHashCollisionRate(uint32_t (*hashFunc)(const std::string&), const std::string& hashName, size_t numStrings, size_t stringLength) {
    std::unordered_set<uint32_t> hashes;
    size_t collisions = 0;

    for (size_t i = 0; i < numStrings; ++i) {
        std::string randomString = generateRandomString(stringLength);
        uint32_t hash = hashFunc(randomString);

        if (hashes.find(hash) != hashes.end()) {
            collisions++;
        } else {
            hashes.insert(hash);
        }
    }

    double collisionRate = static_cast<double>(collisions) / numStrings * 100.0;
    std::cout << hashName << " 碰撞率: " << collisionRate << "% (" << collisions << " 次碰撞)\n";
}

uint8_t fetch_memory_by_one(const std::string& string) {
    const uint8_t* data = reinterpret_cast<const uint8_t*>(string.data());
    size_t len = string.size();
    uint8_t k = 0;
    for(size_t i = 0; i < len; ++i) {
        k += data[i] >> 1;
    }
    return k;
}

uint32_t fetch_memory_by_four(const std::string& string) {
    const uint8_t* data = reinterpret_cast<const uint8_t*>(string.data());
    size_t len = string.size();
    const size_t nblocks = len / 4;
    const uint32_t* blocks = reinterpret_cast<const uint32_t*>(data);
    uint32_t k = 0;
    for(size_t i = 0; i < nblocks; i += 1) {
        k += blocks[i] >> 1;
    }
    for(size_t i = nblocks * 4; i < len; ++i) {
        k += data[i] >> 1;
    }
    return k;
}

#include <benchmark/benchmark.h>
void test_fnv1a_hash(benchmark::State& state) {
    std::string randomString = generateRandomString(100);
    // printf("fnv1a_hash of size %zu string\n", randomString.size()); // Corrected to use size() instead of c_str()
    for (auto _ : state) {
        benchmark::DoNotOptimize(fnv1a_hash(randomString));
    }
}

BENCHMARK(test_fnv1a_hash);

void test_murmurhash3_32(benchmark::State& state) {
    std::string randomString = generateRandomString(100);
    // printf("murmurhash3_32 of size %zu string\n", randomString.size()); // Corrected to use size() instead of c_str()
    for (auto _ : state) {
        benchmark::DoNotOptimize(murmurhash3_32(randomString));
    }
}
BENCHMARK(test_murmurhash3_32);

// fnv1a_hash 每次读取一个字节
void test_fetch_memory_by_one(benchmark::State& state) {
    std::string randomString = generateRandomString(100);
    for (auto _ : state) {
        uint8_t k = fetch_memory_by_one(randomString);
        benchmark::DoNotOptimize(k);
    }
}
BENCHMARK(test_fetch_memory_by_one);


// murmurhash3_32 每次读取四个字节
void test_fetch_memory_by_four(benchmark::State& state) {
    std::string randomString = generateRandomString(100);
    for (auto _ : state) {
        uint32_t k = fetch_memory_by_four(randomString);
        benchmark::DoNotOptimize(k);
    }
}
BENCHMARK(test_fetch_memory_by_four);

BENCHMARK_MAIN();