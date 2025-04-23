#include <iostream>
#include <string>
#include <vector>
#include <cxxabi.h>

std::string demangle(const char * name_mangled) {
  char* ptr = abi::__cxa_demangle(name_mangled, nullptr, nullptr, nullptr);
  std::string ret = std::string(ptr);
  free(ptr);  // the caller is responsible to release the memory.

  return ret;
}

template<typename T>
const std::string type_name() {
  std::string name_;
#if defined(__clang__) || defined(__GNUC__)
  name_ = std::string(__PRETTY_FUNCTION__);
#elif defined(_MSC_VER)
  name_ = std::string(__FUNCSIG__);
#else
  name_ = std::string(__FUNCTION__);
#endif
  size_t pos_eq = name_.find('=');
  size_t pos_right_end = name_.find_last_of(';');

  std::string name = name_.substr(pos_eq+2, pos_right_end - pos_eq - 2);
  return name;

  return "Function not support for windows now";

}


template<typename T>
void func(T& p, const char* desc) {
  std::cout << desc << " func::type of p: " << type_name<decltype(p)>() << std::endl;
  std::cout << desc << " func::传入左值" << std::endl;
}

template<typename T>
void func(T&& p, const char* desc) {
  std::cout << desc << " func::type of p: " << type_name<decltype(p)>() << std::endl;
  std::cout << desc << " func::传入右值" << std::endl;
}

template<typename T>
void wrapper(T&& p) {
  std::cout << "wrapper::type of p: " << type_name<decltype(p)>() << std::endl;

  func(p, "NO FORWARD");
  func(std::forward<T>(p), "FORWARD");
  putchar('\n');
}

int main() {
    
    int a[10];
    int* b = new int[10];
    std::vector<int> c = {1, 2, 3, 4, 5};
    int& d = a[0];

    auto t = type_name<decltype(a)>();
    std::cout << "type_name of a: " << type_name<decltype(a)>() << std::endl;
    std::cout << "typeid(a).name(): " << demangle(typeid(a).name()) << std::endl << std::endl;

    std::cout << "type_name of b: " << type_name<decltype(b)>() << std::endl;
    std::cout << "typeid(b).name(): " << demangle(typeid(b).name()) << std::endl << std::endl;

    std::cout << "type_name of c: " << type_name<decltype(c)>() << std::endl;
    std::cout << "typeid(c).name(): " << demangle(typeid(c).name()) << std::endl << std::endl;

    std::cout << "type_name of d: " << type_name<decltype(d)>() << std::endl;
    std::cout << "typeid(d).name(): " << demangle(typeid(d).name()) << std::endl << std::endl;

    int x = 10;
    wrapper(x);
    wrapper(20); // 传入右值

    return 0;
}