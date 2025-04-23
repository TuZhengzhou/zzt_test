#include <iostream>
#include <string>

// How to run this file
//      g++ wstring_example.cpp -o wstring_example
//      ./wstring_example

int main() {
    // 使用string处理英文
    std::string s = "Hello!";
    std::cout << s[1] << std::endl; // 输出 'e'

    // 使用wstring处理中文
    std::wstring ws = L"你好！";
    std::wcout.imbue(std::locale("chs"));
    std::wcout << ws[1] << std::endl; // 输出 '好'

    return 0;
}