#include "large_file_md5.h"

int main(int argc, char* argv[]) {

    printf("Compile: g++ main.cpp -o main.a -lboost_system -lpthread -O3\n");
    printf("Usange: ./main.a --no-generate --chunk-size=2048 (default KB)\n");

    std::cout << "Boost version: " << BOOST_VERSION << std::endl;
    try {
        // std::ios::sync_with_stdio(false);
        // std::cin.tie(nullptr);

        const std::string file_path = "z_file_test_900MB.bin";
        bool generate = true;
        std::size_t chunk_size = 2 * 1024 * 1024; // 默认 2MB

        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--no-generate") {
                generate = false;
            }
            else if (arg.rfind("--chunk-size=", 0) == 0) {
                std::size_t val = std::stoull(arg.substr(13));
                if (val == 0) throw std::invalid_argument("chunk size must > 0");
                chunk_size = val * 1024;
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
        std::cout << "=== Step 3: MD5 (streaming, chunk=" << chunk_size/1024 << " KB) ===\n";
        auto [md5_stream, t_stream] = md5_streaming(file_path, chunk_size);
        std::cout << "MD5: " << md5_stream << "\nTime: " << t_stream << " s\n\n";

        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "=== Step 3: MD5 (mmap_streaming, chunk=" << chunk_size/1024 << " KB) ===\n";
        auto [md5_mmap_stream, t_mmap_stream] = md5_mmap_streaming(file_path, chunk_size);
        std::cout << "MD5: " << md5_mmap_stream << "\nTime: " << t_mmap_stream << " s\n\n";

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
