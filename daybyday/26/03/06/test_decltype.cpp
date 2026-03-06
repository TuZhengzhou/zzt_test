#include <utility>
#include <vector>
#include <string>
#include <iterator>
#include <cassert>
#include <iostream>
void Overloaded(int);
void Overloaded(char);

#define XX(exp) std::cout << #exp << "is: " << exp << std::endl;

#define YY(val, type) \
    std::cout << "std::is_same_v<decltype(" << #val << "), " << #type << ">" << \
    " is: " << std::is_same_v<decltype(val), type> << std::endl;

int main() {
// 1. expr 是未加括号的标识符（变量名），则返回该变量的声明类型，包括 const / volatile 等类型修饰。
// 2. expr 是函数调用，返回 函数的返回类型
// 3. expr 是一般表达式，则根据根据 值类别(value category) 推导。值类别有3种：
//   a.  lvalue（左值）， 有名字、可定位的对象。常见的返回左值的表达式有：(var)给变量加上括号，*ptr对指针解引用（返回指向对象本身）
//   b.  prvalue（pure rvalue），常见的返回纯右值的表达式有：字面量，算数表达式
//   c.  xvalue（expiring value），即将被销毁的对象， static_cast<T&&>或者std::move(x)
// 如果值类别为lvalue，推导类型为T；如果值类别为prvalue，推导类型为T；如果值类别为xvalue，推导类型为T&&

// 1. 没有加括号的标识符
{
    std::cout << "part 1. 没有加括号的标识符. 返回变量本身的类型" << std::endl;
    int     a;          // 1.1 基本类型
    float   b;
    int     c[20];       
    int*    g;
    int&    h = a;
    int&&   i = 1;
    std::vector<int> d; // 1.2 模板类型
    std::string e;
    class MyClass {};   // 1.3 类类型
    MyClass f;
    
    YY(a, int);
    YY(b, float);
    YY(c, int[20]);
    YY(d, std::vector<int>);
    YY(e, std::string);
    YY(f, MyClass);
    YY(g, int*);
    YY(h, int&);
    YY(i, int&&);
    std::cout << "----------------------------------------------------\n" << std::endl;
}

// 2. 函数调用
{
    std::cout << "part 2. 函数调用. decltype 返回的类型跟函数返回值类型相同" << std::endl;
    int& func1();
    int* func2();
    int&& func3();
    int func4();
    int a;
    int& b = a;

    YY(func1(), int&);
    YY(func2(), int*);
    YY(func3(), int&&);
    YY(func4(), int);
    YY(std::forward<int&>(a), int&);
    YY(std::forward<int&&>(a), int&);
    YY(std::move(a), int&&);
    std::cout << "----------------------------------------------------\n" << std::endl;

}

// 3. 表达式
{
    std::cout << "part 3. 表达式. 表达式根据其是否为左值、纯右值、右值引用，依次返回T&, T, T&&" << std::endl;

    int     a;
    int*    pa;
    int&&   d = std::move(a);
    YY((a), int&);          // 3.1 左值
    YY((pa), int*&);
    YY(*pa, int&);
    YY(12, int);            // 3.2 纯右值
    YY(a + 3, int);
    YY(12.0f, float);
    YY(a + 12.0f, float);
    YY(std::forward<int&&>(a), int&&);      // 3.3 将亡值
    YY(std::forward<int*&&>(pa), int*&&);
    YY(std::move(a), int&&);
    YY(std::move(pa), int*&&);
    YY(static_cast<int&&>(a), int&&);
    YY(static_cast<int*&&>(pa), int*&&);
    std::cout << "----------------------------------------------------\n" << std::endl;

}


}

#undef XX
