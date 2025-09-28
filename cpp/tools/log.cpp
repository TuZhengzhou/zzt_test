#include <string>
#include <iostream>

#include <fmt/core.h>
// g++ log.cpp -o log -lfmt -O3

// A safe wrapper around fmt::format to catch exceptions
template <typename... Args>
inline std::string safe_format(const char *format, Args &&...args)
{
  try
  {
    return fmt::format(format, args...);
  }
  catch (const std::exception &e)
  {
    return fmt::format("[Format Error: {}]: '{}'", e.what(), format);
  }
}

// Main LOG macro
#define LOG(...)                                                          \
  {                                                                       \
    std::cout << __FILE__ << "[" << __func__ << " " << __LINE__ << "]: "; \
    std::cout << safe_format(__VA_ARGS__);                                \
    std::cout << std::endl;                                               \
  }

int main()
{
  LOG("format need 0 arg");
  LOG("format need 2 arg, but provide 1. first is {}, second is {}", 13, 14);
  LOG("format need 2 arg, but provide 1. first is {}, second is {}", 13);
  LOG("format need 0 arg, but provide 1", 13);
}