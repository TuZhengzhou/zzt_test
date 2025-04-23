#include <iostream>
#include "static_storage_def.h"
using namespace std;

// 下文中，用 _def.cpp 表示 static_storage_def.cpp，用 _call.cpp 表示static_storage_call.cpp

// 测试具有外部连接性的static存储变量 var_external: 在 _def.cpp 中定义，在 _call.cpp 中修改，在 _def.cpp 的 func() 函数中打印
// 测试不具有链接性的static存储变量 var_no_linkage: 在 _def.cpp 的 func() 中定义和修改. 注意到到 var_no_linkage 只有在第一次进入 func 时初始化, 后面进入 func 时延续上次离开 func 时的值

// g++ *.cpp -o build/main

// ./build/main 

// 小注：/033[COLORm 可设置输出文字显示的颜色

// 	    黑	红	绿	黄	蓝	紫	深绿 白
// 前景色	30	31	32	33	34	35	36	37
// 背景色	40	41	42	43	44	45	46	47

// // 如果想在这个文件中定义单独的 var_external，则应当如下
// // extern int var_external;
// static int var_external;

extern int var_external;

int main() {
    
    cout << "var_external: " << var_external << endl;

    int repeat = 5;
    for (int i = 0; i < repeat; i++) {
        var_external++;
        func();
    }
    return 0;
}