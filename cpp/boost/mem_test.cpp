#include "large_file_md5.h"
#include "/root/codes/zzt_test/data-structure/tree/binary-tree/thread-binary-tree/tools.h"

int main(int argc, char **argv)
{
  printf("Compile: g++ mem_test.cpp -o mem_test.a -lboost_system -lpthread -O3\n");
  printf("Usange: ./mem_test.a --no-generate --test=[0-4]\n");

  std::cout << "Boost version: " << BOOST_VERSION << std::endl;
  try
  {
    // std::ios::sync_with_stdio(false);
    // std::cin.tie(nullptr);

    const std::string file_path = "z_file_test_900MB.bin";
    bool generate = true;
    int test_id = 0;

    for (int i = 1; i < argc; ++i)
    {
      std::string arg = argv[i];
      if (arg == "--no-generate")
      {
        generate = false;
      }
      else if (arg.rfind("--test=", 0) == 0)
      {
        test_id = std::stoi(arg.substr(7));
        if (test_id < 0 || test_id > 4)
          throw std::invalid_argument("test id must be in [0,4]");
      }
    }

    const std::uint64_t file_size = 900ULL * 1024ULL * 1024ULL; // 900 MB

    if (generate)
    {
      std::cout << "=== Step 1: Generating file (" << file_size << " bytes) ===\n";
      auto t0 = Clock::now();
      generate_file(file_path, file_size);
      auto t1 = Clock::now();
      std::cout << "File generated: " << file_path
                << " in " << std::chrono::duration<double>(t1 - t0).count() << " s\n\n";
    }
    else
    {
      std::cout << "[Skip] File generation.\n";
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));

    if (test_id == 0 || test_id == 1)
    {
      std::cout << "=== Step 2: MD5 (all-in-memory istreambuf_iterator) ===\n";
      auto [md5_all_1, t_all_1] = md5_all_in_memory_1(file_path);
      std::cout << "MD5: " << md5_all_1 << "\nTime: " << t_all_1 << " s\n\n";
      int fd = open(file_path.c_str(), O_RDONLY);
      posix_fadvise(fd, 0, 0, POSIX_FADV_DONTNEED); // 建议内核丢弃缓存
    }

    if (test_id == 0 || test_id == 2)
    {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      std::cout << "=== Step 2: MD5 (all-in-memory reserve size) ===\n";
      auto [md5_all_2, t_all_2] = md5_all_in_memory_2(file_path);
      std::cout << "MD5: " << md5_all_2 << "\nTime: " << t_all_2 << " s\n\n";
      int fd = open(file_path.c_str(), O_RDONLY);
      posix_fadvise(fd, 0, 0, POSIX_FADV_DONTNEED); // 建议内核丢弃缓存
    }

    if (test_id == 0 || test_id == 3)
    {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      std::size_t chunk_size = 2 * 1024 * 1024; // 默认 2MB
      std::cout << "=== Step 3: MD5 (streaming, chunk=" << chunk_size / 1024 << " KB) ===\n";
      auto [md5_stream, t_stream] = md5_streaming(file_path, chunk_size);
      std::cout << "MD5: " << md5_stream << "\nTime: " << t_stream << " s\n\n";
      int fd = open(file_path.c_str(), O_RDONLY);
      posix_fadvise(fd, 0, 0, POSIX_FADV_DONTNEED); // 建议内核丢弃缓存
    }

    if (test_id == 0 || test_id == 4)
    {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      std::size_t chunk_size = 2 * 1024 * 1024; // 默认 2MB
      std::cout << "=== Step 3: MD5 (mmap_streaming, chunk=" << chunk_size / 1024 << " KB) ===\n";
      auto [md5_mmap_stream, t_mmap_stream] = md5_mmap_streaming(file_path, chunk_size);
      std::cout << "MD5: " << md5_mmap_stream << "\nTime: " << t_mmap_stream << " s\n\n";
      int fd = open(file_path.c_str(), O_RDONLY);
      posix_fadvise(fd, 0, 0, POSIX_FADV_DONTNEED); // 建议内核丢弃缓存
    }

    int pid = getpid();
    std::cout << "Process ID: " << pid << std::endl;
    float memory_usage = GetMemoryUsage(pid);
    std::cout << "memory_usage: " << memory_usage << " MB" << std::endl;
  } catch (const std::exception &ex) {
    std::cerr << "Error: " << ex.what() << "\n";
    return 1;
  }
  
  return 0;
}