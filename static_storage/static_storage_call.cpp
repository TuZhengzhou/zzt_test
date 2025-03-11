#include <iostream>
#include "static_storage_def.h"
using namespace std;

extern int var_external;

// int var_external;  error: multiple definition of `var_external'; /tmp/ccgDUrpC.o:(.data+0x0): first defined here

// static int var_external; error: ‘var_external’ was declared ‘extern’ and later ‘static’ [-fpermissive]

// // 如果想在这个文件中定义单独的 var_external，则应当如下
// // extern int var_external;
// static int var_external;


int main() {
    
    cout << "var_external: " << var_external << endl;

    int repeat = 5;
    for (int i = 0; i < repeat; i++) {
        var_external++;
        func();
    }
    return 0;
}