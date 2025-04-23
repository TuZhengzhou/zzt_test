#include <iostream>

// How to run this file
// g++ class_size.cpp -o ./build/class_size -std=c++17
// ./build/class_size

class Base {
public:
    long long _a;
    virtual void func() {
        std::cout << "Base func" << std::endl;
    }
};

class Derived : public Base {
// class Derived : virtual public Base {
public:
    long long _b;
    virtual void func() {
        std::cout << "Derived func" << std::endl;
    }
};

class Derived2 : public Base {
// class Derived2 : virtual public Base {
public:
    long long _c;
    void func() override {
        std::cout << "Derived2 func" << std::endl;
    }
};

class Derived3: public Derived2, public Derived {
public:
    long long _d;
    void func() override {
        std::cout << "Derived3 func" << std::endl;
    }
};

class Derived_1 : virtual public Base {
public:
    long long _b;
    void func() override {
        std::cout << "Derived_1 func" << std::endl;
    }
};

class Derived2_1 : virtual public Base {
public:
    long long _c;
    void func() override {
        std::cout << "Derived2_1 func" << std::endl;
    }
};

class Derived3_1: public Derived2_1, public Derived_1 {
public:
    long long _d;
    void func() override {
        std::cout << "Derived3_1 func" << std::endl;
    }
};


int main() {
    

    std::cout << "sizeof(Base) = " << sizeof(Base) << std::endl;            // sizeof(a) + vtable_ptr
    std::cout << "sizeof(Derived) = " << sizeof(Derived) << std::endl;      // sizeof(a) + sizeof(b) + vtable_ptr
    std::cout << "sizeof(Derived2) = " << sizeof(Derived2) << std::endl;    // sizeof(a) + sizeof(c) + vtable_ptr
    std::cout << "sizeof(Derived3) = " << sizeof(Derived3) << std::endl;    // sizeof(a) + sizeof(b) + sizeof(c) + sizeof(d) + 2 * vtable_ptr
    // 由于 Derived2 和 Derived 都各自独立地继承自 Base，Derived3 对象中会存在两个 Base 子对象，它们不会被共享（只有在 virtual 继承下才会共享）

    std::cout << "sizeof(Derived_1) = " << sizeof(Derived_1) << std::endl; // sizeof(a) + sizeof(b) + vtable_ptr * 2
    std::cout << "sizeof(Derived2_1) = " << sizeof(Derived2_1) << std::endl; // sizeof(a) + sizeof(c) + vtable_ptr * 2
    std::cout << "sizeof(Derived3_1) = " << sizeof(Derived3_1) << std::endl; // sizeof(a) + sizeof(b) + sizeof(c) + sizeof(d) + vtable_ptr * 2

    
    // 多继承下的两个 Base 子对象
    // 由于 Derived2 和 Derived 都各自独立地继承自 Base，Derived3 对象中会存在两个 Base 子对象，它们不会被共享（只有在 virtual 继承下才会共享）。
    
    // ✅ 总结：为什么是 56 字节？
    // text
    // 复制
    // 编辑
    // Derived3
    // ├── Derived2 (Base + _c):     16 + 8 = 24
    // ├── Derived  (Base + _b):     16 + 8 = 24
    // └── _d:                        8
    //                              ------
    //                              = 56

    // Base* basePtr = new Derived3();
    Derived3* t = new Derived3();
    t->Derived::_a = 2; // Explicitly specify the path to resolve ambiguity
    t->Derived2::_a = 3;
    t->_b = 5;
    t->_c = 7;
    t->_d = 11;
    printf("%lld, %lld, %lld, %lld, %lld\n", t->Derived::_a, t->Derived2::_a, t->_b, t->_c, t->_d);
    t->Derived::func();
    t->Derived2::func();
    t->func();

    std::cout << "sizeof(t) = " << sizeof(*t) << std::endl; // Changed sizeof(t) to sizeof(*t)

    delete t;

    return 0;
}