#include <fstream>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
// #include <thread>

// #include <benchmark/benchmark.h>
#include <boost/uuid/detail/md5.hpp>

using Clock = std::chrono::steady_clock;
const std::string path = "z_file_test_900MB.bin";

// 将 boost md5::digest_type 转为十六进制字符串
std::string md5_digest_to_hex(const boost::uuids::detail::md5::digest_type& digest) {
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&digest);
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (std::size_t i = 0; i < sizeof(digest); ++i) {
        oss << std::setw(2) << static_cast<unsigned int>(bytes[i]);
    }
    return oss.str();
}

std::pair<std::string, double> md5_streaming(const std::string& path, std::size_t chunk_size) {
    auto t0 = Clock::now();
    std::ifstream ifs(path, std::ios::binary);
    if (!ifs) throw std::runtime_error("failed to open file: " + path);

    boost::uuids::detail::md5 md5;
    std::vector<char> buf(chunk_size);

    while (true) {
        ifs.read(buf.data(), static_cast<std::streamsize>(buf.size()));
        std::streamsize got = ifs.gcount();
        if (got > 0) {
            md5.process_bytes(buf.data(), static_cast<std::size_t>(got));
        }
        if (!ifs) {
            if (ifs.eof()) break;
            throw std::runtime_error("read failed (streaming)");
        }
    }

    boost::uuids::detail::md5::digest_type digest;
    md5.get_digest(digest);
    auto md5_hex = md5_digest_to_hex(digest);

    auto t1 = Clock::now();
    return { md5_hex, std::chrono::duration<double>(t1 - t0).count() };
}

int main() {
    printf("Compile: g++ chunk_size_performance_test.cpp -o chunk_size_performance_test.a -lboost_system -O3\n");

    int repeat = 5;
    for (int i = 1; i <= 65536; i *= 2) {
        double total_duration = 0;
        for (int j = 0; j < repeat; j++) {
            // printf("j = %d\n", j);
            auto [hex, duration] = md5_streaming(path, i * 1024);
            total_duration += duration;
        }
        printf("chuck size: %5d, duration: %.2f second\n", i, total_duration/repeat);
    }
}