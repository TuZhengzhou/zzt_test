#include <execinfo.h> // backtrace
#include <iostream>
#include <cxxabi.h>

std::string demangle(std::string mangle_name)
{
    // preprocess
    size_t pos_lbracket, pos_add, pos_rbracket;
    {
        pos_lbracket = mangle_name.find('(');
        pos_add = mangle_name.find('+');
        pos_rbracket = mangle_name.find(')');
    }
    // printf("pos_lbracket = %zu, pos_add = %zu, pos_rbracket = %zu\n", pos_lbracket, pos_add, pos_rbracket);
    // fflush(stdout);

    if (pos_lbracket == std::string::npos || pos_add == std::string::npos || pos_rbracket == std::string::npos || (pos_add == pos_lbracket + 1))
        return mangle_name;

    std::string ret;
    // demangle
    {
        std::string name = mangle_name.substr(pos_lbracket + 1, pos_add - pos_lbracket - 1);
        std::string offset = mangle_name.substr(pos_add, pos_rbracket - pos_add - 1);

        auto demangle = abi::__cxa_demangle(name.c_str(), nullptr, nullptr, nullptr);
        // printf("typeinfo of demangle is %s\n", abi::__cxa_demangle(typeid(demangle).name(), nullptr, nullptr, nullptr));

        if (demangle != nullptr)
        {
            // std::cout << "demangle is " << demangle << std::endl;
            ret = abi::__cxa_demangle(name.c_str(), nullptr, nullptr, nullptr) + mangle_name.substr(pos_add, pos_rbracket - pos_add - 1);
            free(demangle);
        }
        else
        {
            ret = name + "()" + mangle_name.substr(pos_add, pos_rbracket - pos_add - 1);
        }
    }

    return ret;
}

void print_backtrace()
{
    const int max_frames = 10;
    void *addrlist[max_frames + 1];

    // 获取回溯地址
    int addrlen = backtrace(addrlist, max_frames);
    char **strings;
    strings = backtrace_symbols(addrlist, addrlen);

    printf("addrlen = %d\n", addrlen);

    for (int i = 0; i < addrlen; i++)
    {
        // printf("symbol: %s, addr: %p\n", strings[i], addrlist[i]);
        printf("demangle: %s\n", demangle(std::string(strings[i])).c_str());
    }
}

void dummy_func()
{
    print_backtrace();
}

int main()
{
    dummy_func();
    return 0;
}
