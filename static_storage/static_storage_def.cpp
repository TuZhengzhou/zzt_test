#include <iostream>
using namespace std;

int var_external = 1;

int static var_internal = 2;

int func() {
    static int var_no_linkage = 3;
    cout << "func::var_no_linkage(modified in \033[31m static_storage_def.cpp::func \033[30m): " << var_no_linkage++ << endl;

    cout << "func::var_external(modified in \033[32m static_storage_call.cpp::main \033[30m): " << var_external << endl;
    return 0;
}