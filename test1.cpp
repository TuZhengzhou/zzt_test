#include <iostream>
#include <sstream>

 
int main()
{
    std::istringstream s("1 2 3 4");
    int n;
 
    std::cout << std::boolalpha << "s is " << static_cast<bool>(s) << '\n';
    while (s >> n)
        std::cout << n << '\n';
    std::cout << "s is " << static_cast<bool>(s) << '\n';

    std::string input = "1,2,3,4,5,6 7";
    std::istringstream iss(input);
    std::string token;

    // 以','为分隔符，循环读取每个子串
    std::string last_token;
    while (getline(iss, token, ',')) {
        size_t space_pos = token.find(' ');
        if (space_pos != std::string::npos) {
            last_token = token.substr(space_pos);
        }
        std::cout << (stoi(token)) << '\n';
    }
    // 获取当前读取位置
    std::streamsize pos = iss.tellg();
    if (pos != -1) { // 检查是否仍有未读取内容
        // 截取剩余字符串（从当前位置到结尾）
        std::string remaining = iss.str().substr(pos);
        std::cout << "剩余字符: " << remaining << std::endl; // 输出: 56,78,90
    } else {
        std::cout << "已读取完所有内容" << std::endl;
    }
    std::cout << "final one " <<(stoi(last_token)) << '\n';
}