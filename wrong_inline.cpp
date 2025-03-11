#include <iostream>
using namespace std;

__attribute__((always_inline)) 
inline void wrong_inline_func(int n) {
    if(n == 0) return;
    
    cout << "wrong_inline_func" << endl;
    wrong_inline_func(n-1);
}

int main() {
    int repeat;
    cin >> repeat;
    wrong_inline_func(repeat);
    int a;
    cout << a;
    return 0;
}