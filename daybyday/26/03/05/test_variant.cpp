#include <variant>
#include <string>
#include <iostream>

int main() {
    std::variant<int, float, std::string> v;
    v = std::string("hello");
    
    if (std::holds_alternative<int>(v)) {
        std::cout << "int: " << std::get<int>(v) << std::endl;
    }

    std::visit([](auto&& arg){
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>)
            std::cout << "int with value " << arg << '\n';
        else if constexpr (std::is_same_v<T, long>)
            std::cout << "long with value " << arg << '\n';
        else if constexpr (std::is_same_v<T, double>)
            std::cout << "double with value " << arg << '\n';
        else if constexpr (std::is_same_v<T, std::string>)
            std::cout << "std::string with value " << arg << '\n';
        else
            std::cout << "non-exhaustive visitor!" << "\n";
    }, v);
}