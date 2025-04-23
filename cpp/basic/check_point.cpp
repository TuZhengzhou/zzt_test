#include <iostream>
#include <random>
#include <algorithm>

/*
    断点可以细分为
        条件断点：条件表达式为真. x == 3
        计数断点：经过该断点 n 次后触发. 12
        触发断点：指定的另一个断点触发后才会触发. file.cpp line XXX
        日志断点：不停止程序运行，只打印日志. the value is: {v}

    一个断点可以同时是以上几种类型。注意，如果是日志断点，则不会停止程序运行，即使它是其他类型的断点。
*/

int func(int a, int b) {
    return a + b;
}

// ./check_point
int main() {
    std::vector<int> vec(100, 0);

    std::vector<int>::iterator it = vec.begin();
    int cnt = 0;
    while (it != vec.end()) {
        *it = cnt++;
        std::cout << *it << std::endl;
        ++it;
    }

    it = vec.begin();
    
    while (it != vec.end()) {
        int x = *it;
        func(x, cnt);
        std::cout << x << std::endl;
        ++it;
        cnt++;
        
    }

    return 0;
}