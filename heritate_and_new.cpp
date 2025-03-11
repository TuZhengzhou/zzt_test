#include <iostream>
#include <cstring>

using namespace std;

class base {
private:
    char* label;
    int   value;
public:
    base(const char* l = "null", int v = 0);    // 析构函数
    base(const base& origin);                   // 复制构造函数
    virtual ~base();
    base& operator=(const base& r);             // 赋值运算符
};

base::base(const char* l, int v) {
    label = new char[std::strlen(l) + 1];
    std::strcpy(label, l);
    value = v;
}

base::base(const base& origin) {
    label = new char[strlen(origin.label) + 1];
    strcpy(label, origin.label);
    value = origin.value;
}

base::~base() {
    delete [] label;
}

base& base::operator=(const base& r) {
    if (this == &r) 
        return *this;
    
    delete [] this->label;
    this->label = new char[strlen(r.label) + 1];
    strcpy(label, r.label);
    this->value = r.value;

    return *this;
}

class drive1: public base {
private:
    char* color;
public:
    drive1(const char* l = "null", int v = 0, const char* c = "red");
    drive1(drive1& origin);
    virtual ~drive1();
    drive1& operator=(const drive1& r);
};

drive1::drive1(const char* l, int v, const char* c) : base(l, v) {
    color = new char[strlen(c) + 1];
    strcpy(color, c);
}

drive1::drive1(drive1& origin) : base(origin) {
    color = new char[strlen(origin.color) + 1];
    strcpy(color, origin.color);
}

// 派生类析构函数自动调用基类的析构函数，故其自身的职责是对派
// 生类构造函数执行工作的进行清理。因此，hasDMA析构函数必须释放
// 指针style管理的内存，并依赖于baseDMA的析构函数来释放指针label管
// 理的内存。
drive1::~drive1() {
    delete [] color;
}

drive1& drive1::operator=(const drive1& origin) {
    if (this == &origin) 
        return *this;

    base::operator=(origin); // 即 *this = r;
    delete [] this->color;
    color = new char[strlen(origin.color) + 1];
    strcpy(color, origin.color);

    return *this;
}

int main() {
    
}