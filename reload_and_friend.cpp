#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <ctime>

using namespace std;

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
    os << "name = " << stu.name() << ", age = " << stu.age() << endl;
    return os;
    // os << stu.toString() << endl;
    // return os;
}

class sutudent_plus : public student
{
private:
    int plus;
public:
    sutudent_plus(string _name, int _age, int _plus);
    virtual ~sutudent_plus() = default;

    virtual string toString() const { 
        ostringstream oss;
        oss << "name = " << this->name() << ", age = " << this->age() << ", plus = " << this->plus;
        return oss.str();
    };
    // friend ostream& operator<<(ostream& os, const sutudent_plus& stu_p);
};

sutudent_plus::sutudent_plus(string _name, int _age, int _plus) : student(_name, _age), plus(_plus) {};

ostream& operator<<(ostream& os, const sutudent_plus& stu_p) {
    // os << "name = " << stu_p.name() << ", age = " << stu_p.age() << ", plus = " << stu_p.plus << endl;
    // return os;
    os << stu_p.toString() << endl;
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
    
    Time local_time;
    cout << stu << "some more at " << local_time << endl;

    std::ofstream fout;
    fout.open("reload_and_friend.txt");
    fout << stu << "write to file finish at " << local_time << endl;

    auto stu_p = sutudent_plus("name", 13, 14);
    student& stu_r = stu_p;
    cout << stu_r << endl;

    int a;
    cout << "Uninitialized a: " << a << endl;
    
    return 0;
}