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

#include <sys/mman.h>   // mmap, munmap
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
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

    int fd = open(path.c_str(), O_RDONLY);
    if (fd == -1) throw std::runtime_error("failed to open file: " + path);
    struct stat st;
    if (fstat(fd, &st) == -1) {
        close(fd);
        throw std::runtime_error("failed to stat file: " + path);
    }
    posix_fadvise(fd, 0, 0, POSIX_FADV_SEQUENTIAL); // 建议内核按顺序读取
    std::uint64_t size = static_cast<std::uint64_t>(st.st_size);
    std::vector<char> buf(chunk_size);
    boost::uuids::detail::md5 md5;
    std::uint64_t remaining = size;
    while (remaining > 0) {
        std::size_t to_read = static_cast<std::size_t>(std::min<std::uint64_t>(remaining, chunk_size));
        ssize_t got = read(fd, buf.data(), to_read);
        if (got == -1) {
            close(fd);
            throw std::runtime_error("read failed (streaming)");
        }
        if (got == 0) break; // EOF
        md5.process_bytes(buf.data(), static_cast<std::size_t>(got));
        remaining -= static_cast<std::uint64_t>(got);
    }
    close(fd);
    boost::uuids::detail::md5::digest_type digest;
    md5.get_digest(digest);
    auto md5_hex = md5_digest_to_hex(digest);

    auto t1 = Clock::now();
    return { md5_hex, std::chrono::duration<double>(t1 - t0).count() };
}

// 方式（3）：mmap + madvise + 按块读取
static std::pair<std::string, double> md5_mmap_streaming(const std::string& path, std::size_t chunk_size) {
    auto t0 = Clock::now();
    int fd = open(path.c_str(), O_RDONLY);
    if (fd == -1) throw std::runtime_error("failed to open file: " + path);
    // 获取文件大小
    struct stat st;
    if (fstat(fd, &st) == -1) {
        close(fd);
        throw std::runtime_error("failed to stat file: " + path);
    }
    std::uint64_t size = static_cast<std::uint64_t>(st.st_size);
    
    char* data = (char*)mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (data == MAP_FAILED) {
        close(fd);
        throw std::runtime_error("mmap failed: " + path);
    }
    close(fd);
    // 建议内核按顺序读取
    madvise(data, size, MADV_SEQUENTIAL);
    boost::uuids::detail::md5 md5;
    std::uint64_t offset = 0;
    while (offset < size) {
        std::size_t to_process = static_cast<std::size_t>(std::min<std::uint64_t>(chunk_size, size - offset));
        md5.process_bytes(data + offset, to_process);
        offset += to_process;
    }
    munmap(data, size);
    boost::uuids::detail::md5::digest_type digest;
    md5.get_digest(digest);
    auto md5_hex = md5_digest_to_hex(digest);
    auto t1 = Clock::now();
    return { md5_hex, std::chrono::duration<double>(t1 - t0).count() };
}

// 分块 mmap 