// #include <iostream>
// #include <cassert>
// #include "singleton.h"


// class Impl: public Singleton<Impl> {
// public:
//     int v = 3;
// private:
//     Impl();
// };

// int main() {
//     Impl& var1 = Impl::instance();
//     Impl& var2 = Impl::instance();
//     // Impl var3;
//     assert(&var1 == &var2);

//     std::cout << "var1.v = " << var1.v << std::endl;
//     var1.v = 4;
//     std::cout << "change var1.v to " << var1.v << std::endl;
//     std::cout << "var2.v = " << var2.v << std::endl;
// }

#include "singleton.h"


class Impl: public Singleton<Impl> {
public:
    int v = 3;
};

class Impl2 {
public:
    int v = 255;
};

int main() {
    Impl var3;
    Impl2& var1 = Singleton<Impl2>::instance();
}