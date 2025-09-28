#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <random>
#include <chrono>
#include <iomanip>
#include <string>
#include <thread>
#include <stdexcept>

#include <boost/uuid/detail/md5.hpp>  // boost::uuids::detail::md5

// g++ large_file_md5.cpp -o large_file_md5 -lboost_system -lpthread -O3


using Clock = std::chrono::steady_clock;

// 将 boost md5::digest_type 转为十六进制字符串
static std::string md5_digest_to_hex(const boost::uuids::detail::md5::digest_type& digest) {
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&digest);
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (std::size_t i = 0; i < sizeof(digest); ++i) {
        oss << std::setw(2) << static_cast<unsigned int>(bytes[i]);
    }
    return oss.str();
}

// 生成 size_bytes 大小的文件（内容为伪随机字节，按 8MB 块写入）
static void generate_file(const std::string& path, std::uint64_t size_bytes) {
    std::ofstream ofs(path, std::ios::binary | std::ios::trunc);
    if (!ofs) throw std::runtime_error("failed to open output file for writing: " + path);

    constexpr std::size_t kChunk = 8 * 1024 * 1024; // 8MB
    std::vector<unsigned char> buf(kChunk);

    std::mt19937 rng(123456789);
    std::uniform_int_distribution<int> dist(0, 255);
    for (auto& b : buf) b = static_cast<unsigned char>(dist(rng));

    std::uint64_t remaining = size_bytes;
    while (remaining > 0) {
        std::size_t to_write = static_cast<std::size_t>(std::min<std::uint64_t>(remaining, kChunk));
        ofs.write(reinterpret_cast<const char*>(buf.data()), to_write);
        if (!ofs) throw std::runtime_error("write failed while generating file");
        remaining -= to_write;
    }
    ofs.flush();
    if (!ofs) throw std::runtime_error("flush failed");
}

// 方式（1）：一次性全部读入再计算 MD5（不预设 data 大小）
static std::pair<std::string, double> md5_all_in_memory_1(const std::string& path) {
    auto t0 = Clock::now();

    std::ifstream ifs(path, std::ios::binary);
    if (!ifs) throw std::runtime_error("failed to open file: " + path);

    // 使用 istreambuf_iterator 直接读入容器，不提前预设容量
    std::vector<char> data(
        (std::istreambuf_iterator<char>(ifs)),
        std::istreambuf_iterator<char>()
    );

    boost::uuids::detail::md5 md5;
    if (!data.empty()) {
        md5.process_bytes(data.data(), data.size());
    }

    boost::uuids::detail::md5::digest_type digest;
    md5.get_digest(digest);
    auto md5_hex = md5_digest_to_hex(digest);

    auto t1 = Clock::now();
    return { md5_hex, std::chrono::duration<double>(t1 - t0).count() };
}

// 方式（1）：一次性全部读入再计算 MD5
static std::pair<std::string, double> md5_all_in_memory_2(const std::string& path) {
    auto t0 = Clock::now();
    std::ifstream ifs(path, std::ios::binary);
    if (!ifs) throw std::runtime_error("failed to open file: " + path);

    ifs.seekg(0, std::ios::end);
    std::uint64_t size = static_cast<std::uint64_t>(ifs.tellg());
    ifs.seekg(0, std::ios::beg);

    std::vector<char> data(size);
    if (size > 0) {
        ifs.read(data.data(), static_cast<std::streamsize>(size));
        if (!ifs) throw std::runtime_error("read failed (all-in-memory)");
    }

    boost::uuids::detail::md5 md5;
    md5.process_bytes(data.data(), data.size());
    boost::uuids::detail::md5::digest_type digest;
    md5.get_digest(digest);
    auto md5_hex = md5_digest_to_hex(digest);

    auto t1 = Clock::now();
    return { md5_hex, std::chrono::duration<double>(t1 - t0).count() };
}


// 方式（2）：按块读取，边读边处理
static std::pair<std::string, double> md5_streaming(const std::string& path, std::size_t chunk_size) {
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

//.\test.exe z_file_test_900MB.bin --no-generate --chunk-size=1048576
int main(int argc, char* argv[]) {

    std::cout << "Boost version: " << BOOST_VERSION << std::endl;
    try {
        // std::ios::sync_with_stdio(false);
        // std::cin.tie(nullptr);

        const std::string file_path = (argc >= 2) ? argv[1] : "z_file_test_900MB.bin";
        bool generate = true;
        std::size_t chunk_size = 2 * 1024 * 1024; // 默认 2MB

        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--no-generate") {
                generate = false;
            }
            else if (arg.rfind("--chunk-size=", 0) == 0) {
                std::size_t val = std::stoull(arg.substr(13));
                if (val == 0) throw std::invalid_argument("chunk size must > 0");
                chunk_size = val;
            }
        }

        const std::uint64_t file_size = 900ULL * 1024ULL * 1024ULL; // 900 MB

        if (generate) {
            std::cout << "=== Step 1: Generating file (" << file_size << " bytes) ===\n";
            auto t0 = Clock::now();
            generate_file(file_path, file_size);
            auto t1 = Clock::now();
            std::cout << "File generated: " << file_path
                << " in " << std::chrono::duration<double>(t1 - t0).count() << " s\n\n";
        }
        else {
            std::cout << "[Skip] File generation.\n";
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "=== Step 2: MD5 (all-in-memory istreambuf_iterator) ===\n";
        auto [md5_all_1, t_all_1] = md5_all_in_memory_1(file_path);
        std::cout << "MD5: " << md5_all_1 << "\nTime: " << t_all_1 << " s\n\n";

        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "=== Step 2: MD5 (all-in-memory reserve size) ===\n";
        auto [md5_all_2, t_all_2] = md5_all_in_memory_2(file_path);
        std::cout << "MD5: " << md5_all_2 << "\nTime: " << t_all_2 << " s\n\n";

        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "=== Step 3: MD5 (streaming, chunk=" << chunk_size << " bytes) ===\n";
        auto [md5_stream, t_stream] = md5_streaming(file_path, chunk_size);
        std::cout << "MD5: " << md5_stream << "\nTime: " << t_stream << " s\n\n";

        if (md5_all_2 == md5_stream) {
            std::cout << "[OK] MD5 matches.\n";
        }
        else {
            std::cout << "[WARN] MD5 differs!\n";
        }
        return 0;
    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }
}
