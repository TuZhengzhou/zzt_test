#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <ctime>

using namespace std;

// How to run this file
// g++ friend.cpp -o friend
// ./friend

/*
    本文件定义了 student 类，包含 name 和 age 两个成员变量，以及一个 virtual toString 方法
    通过重载 << 运算符，可以直接输出 student 对象

    本文件还定义了 student_plus 类，继承自 student 类，增加了一个 plus 成员变量
    为 student_plus 类定义了新的 virtual toString 方法, 输出 name, age 和 plus 三个成员变量

    本文件执行以下测试:
     1. 创建一个 student 指针, 指向 student 
        2. 通过 <<, 输出 student 指针指向的对象
        3. 通过 toString, 输出 student 指针指向的对象
     4. student 指针指向 student_plus 对象
        5. 通过 <<, 输出 student 指针指向的对象
        6. 通过 toString, 输出 student 指针指向的对象

    测试以下特性:
        1. virtual:  指向派生类对象的基类指针, 可以通过基类指针调用派生类的方法, 前提是该方法定义为 virtual
        2. friend: operator<< 可以访问类的私有成员. 但 operator<< 不是类的成员函数, 因此对于基类指针, 无法调用派生类的 operator<<

    本文件还定义了 Time 类，获取当前时间，并提供了输出时间的方法
    本文件还测试了悬垂指针的问题

*/

class Time {
    public:
        // 构造函数：获取当前时间（默认本地时间，use_utc=true时为UTC时间）
        explicit Time(bool use_utc = false) {
            const std::time_t now = std::time(nullptr);
            set_time(now, use_utc);
        }
    
        // 构造函数：从指定时间戳初始化
        Time(std::time_t timestamp, bool use_utc = false) {
            set_time(timestamp, use_utc);
        }
    
        // 获取各部分数值
        int year() const    { return tm_.tm_year + 1900; }
        int month() const   { return tm_.tm_mon + 1; }
        int day() const     { return tm_.tm_mday; }
        int hour() const    { return tm_.tm_hour; }
        int minute() const  { return tm_.tm_min; }
        int second() const  { return tm_.tm_sec; }
    
        // 转换为字符串（格式：YYYY-MM-DD HH:MM:SS）
        std::string toString() const {
            std::ostringstream oss;
            oss << std::setfill('0')
                << std::setw(4) << year() << "-"
                << std::setw(2) << month() << "-"
                << std::setw(2) << day() << " "
                << std::setw(2) << hour() << ":"
                << std::setw(2) << minute() << ":"
                << std::setw(2) << second();
            return oss.str();
        }

        friend ostream& operator<<(ostream& os, const Time& t) {
            os << t.toString() << endl;
            return os;
        }
    
    private:
        std::tm tm_{};
    
        void set_time(std::time_t timestamp, bool use_utc) {
            // 线程安全的时间转换
            std::tm* tmp = use_utc ? gmtime_r(&timestamp, &tm_) : localtime_r(&timestamp, &tm_);
            if (!tmp) {
                throw std::runtime_error("Failed to convert time");
            }
        }
    };

class student {
    private:
        string _name;
        int _age;
    public:
        student(string name, int age): _name(name), _age(age) {};
        virtual ~student() = default;
        
        string name() const;
        int age() const {return _age;};

        virtual string toString() const { 
            ostringstream oss;
            oss << "name = " << this->name() << ", age = " << this->age();
            return oss.str();
        };

        friend ostream& operator<<(ostream& os, const student& stu);
};

string student::name() const {
    // return _name; 
    return student::_name;
}

ostream& operator<<(ostream& os, const student& stu) {
    os << "name = " << stu.name() << ", age = " << stu.age();
    return os;
    // os << stu.toString() << endl;
    // return os;
}

class student_plus : public student
{
private:
    int _plus;
public:
    student_plus(string name, int age, int plus);
    virtual ~student_plus() = default;

    virtual string toString() const { 
        ostringstream oss;
        oss << "name = " << this->name() << ", age = " << this->age() << ", plus = " << this->_plus;
        return oss.str();
    };
    friend ostream& operator<<(ostream& os, const student_plus& stu_p);
};

student_plus::student_plus(string name, int age, int plus) : student(name, age), _plus(plus) {};

ostream& operator<<(ostream& os, const student_plus& stu_p) {
    os << "name = " << stu_p.name() << ", age = " << stu_p.age() << ", plus = " << stu_p._plus;
    return os;
}


int* createDangLingPointer() {
    int a = 12;
    return &a;
}

int main(int agrc, char* agrv []) {
    int * b = createDangLingPointer();
    // *b = 10; // Segmentation fault (core dumped)

    student stu = student("Tom", 12);
    student_plus stu_p = student_plus("Jerry", 13, 14);

    student* stu_ptr = &stu;
    Time local_time;
    cout << "pointer to student: " << endl;
    cout << std::setw(20) << "Output by <<: " << *stu_ptr << endl;
    cout << std::setw(20) << "Output by toString: " << stu_ptr->toString().c_str() << endl << endl;

    stu_ptr = &stu_p;
    cout << "pointer to student_plus: " << endl;
    cout << std::setw(20) << "Output by <<: " << *stu_ptr << endl;
    cout << std::setw(20) << "Output by toString: " << stu_ptr->toString().c_str() << endl;
    
    return 0;
}