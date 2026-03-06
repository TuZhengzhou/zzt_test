#include <iostream>
#include <string>
#include <stdint.h>
#include <cassert>

template <typename T>
class MustInitT {
    T t;
    unsigned char init = 0;

    MustInitT(T v) {
        
        assert(init);
    }
};

class UnInit {
public:
    enum UnInitType {
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL
    };
    UnInit(const std::string& name): name(name) {};

    const std::string& GetName() const { return this->name; };
    int GetID() const { return this->id; };
    UnInitType GetType() const { return type; };
private: 
    std::string name;
    int id;
    UnInitType type;
};

int main() {
    UnInit un_init("");
    std::cout << "The id of un_init is [" << un_init.GetID() << "]\n";
    std::cout << "The type of un_init is [" << un_init.GetType() << "]\n";
}