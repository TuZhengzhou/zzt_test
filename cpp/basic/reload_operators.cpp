#include <iostream>
#include <vector>
#include <string>

/*
    relod_operator 命名空间中记录了 operator(), operator[], operator-> 的重载

    tricky 命名空间中记录了 operator() 的重载函数和构造函数参数列表相同的情形. 注意：

        构造函数 和 () 运算符重载 是完全不同的机制：
        构造函数用于初始化对象。 Class() 语法用于创建对象。
        () 运算符重载用于使对象可以像函数一样被调用。Class(init args)(calling args) 语法用于调用对象。
        它们的调用方式不同，因此即使参数列表相同，也不会冲突。
        通过语法和语义上的区别，可以清晰地区分它们。

*/
// How to run this file
//      g++ reload_operators.cpp -o reload_operators
//      ./reload_operators

namespace relod_operator {
    // 重载 () 运算符（函数调用运算符）
    // () 运算符的重载使得对象可以像函数一样被调用。这种对象称为 函数对象（Functor）。
    // 语法
    // 返回值类型 operator()(参数列表);

    // operator() 的重载允许有多个参数
    class Adder {
    public:
        Adder() = default;
        int operator()(int a, int b) const {
            return a + b;
        }
    };

    // 重载 [] 运算符（下标运算符）
    // [] 运算符的重载使得对象可以像数组一样通过下标访问元素。the `[]` operator only accepts a single parameter.
    // 语法
    //  返回值类型& operator[](参数类型 index);
    //  const 返回值类型& operator[](参数类型 index) const;  // const 版本

    // operator[] 的重载只允许有一个表示 index 的参数
    class Array {
    public:
        Array(std::initializer_list<int> list) : data(list) {};
        // const 版本
        const int& operator[](size_t index) const {
            if (index >= data.size()) {
                throw std::out_of_range("Index out of range");
            }
            return data[index];
        }
    private:
        std::vector<int> data;
    };

    // 重载 -> 运算符（成员访问运算符）
    // -> 运算符的重载使得对象可以像指针一样通过 -> 访问成员。
    // 语法
    //  返回值类型* operator->();
    //  const 返回值类型* operator->() const;  // const 版本

    // operator-> 的重载不允许有参数
    class MyClass {
        public:
            void print(std::string str) {
                std::cout << str << std::endl;
            }
    };
    class wrapper {
        public:
            MyClass* operator->() {
                return &obj;
            }
        private:
            MyClass obj;
    };
}

namespace tricky {
    class Adder {
    public:
        // 构造函数
        Adder(int a, int b) {
            std::cout << "Constructor called with: " << a << ", " << b << std::endl;
        }

        // 重载 () 运算符
        int operator()(int a, int b) const {
            std::cout << "operator() called with: " << a << ", " << b << std::endl;
            return a + b;
        }
    };
}

int main() {
    std::cout << "reload operator(). relod_operator::Adder()(1,2) = " << relod_operator::Adder()(1,2) << std::endl;
    std::cout << "reload operator(). relod_operator::Adder().operator()(1,2) = " << relod_operator::Adder().operator()(1,2) << std::endl;
    relod_operator::Adder adder;
    std::cout << "reload operator(). adder(1,2) = " << adder(1,2) << std::endl;
    std::cout << "reload operator(). adder.operator()(1,2) = " << adder.operator()(1,2) << std::endl;

    std::cout << "reload operator[]. relod_operator::Array({1, 2, 3, 4, 5, 6})[2] = " << relod_operator::Array({1, 2, 3, 4, 5, 6})[2] << std::endl;
    std::cout << "reload operator[]. relod_operator::Array({1, 2, 3, 4, 5, 6}).operator[](2) = " << relod_operator::Array({1, 2, 3, 4, 5, 6}).operator[](2) << std::endl;
    relod_operator::Array arr = {1, 2, 3, 4, 5, 6};
    std::cout << "reload operator[]. arr[2] = " << arr[2] << std::endl;
    std::cout << "reload operator[]. arr.operator[](2) = " << arr.operator[](2) << std::endl;

    
    relod_operator::wrapper()->print(std::string("reload operator->. relod_operator::wrapper()->print"));
    relod_operator::wrapper().operator->()->print(std::string("reload operator->. relod_operator::wrapper().operator->()->print"));
    relod_operator::wrapper w;
    w->print(std::string("reload operator->. w->print"));
    w.operator->()->print(std::string("reload operator->. w.operator->()->print"));

    tricky::Adder(1,2);                     // Constructor called with: 1, 2. 
    tricky::Adder(1, 2)(3, 4);
    tricky::Adder(1, 2).operator()(3, 4);
    
    return 0;
}