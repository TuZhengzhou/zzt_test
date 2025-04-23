#include <string>
#include <iostream>


int main() {

    using namespace std;

    __is_fast_hash<std::string> is_fast_hash;

    std::cout << "is_fast_hash<hash<bool>>: " << __is_fast_hash<hash<bool>>() << std::endl;
    std::cout << "is_fast_hash<hash<char>>: " << __is_fast_hash<hash<char>>() << std::endl;
    std::cout << "is_fast_hash<hash<int>>: " << __is_fast_hash<hash<int>>() << std::endl;
    std::cout << "is_fast_hash<hash<double>>: " << __is_fast_hash<hash<double>>() << std::endl;
    std::cout << "is_fast_hash<hash<long double>>: " << __is_fast_hash<hash<long double>>() << std::endl;
    std::cout << "is_fast_hash<hash<std::string>>: " << __is_fast_hash<hash<std::string>>() << std::endl;

    return 0;
}
