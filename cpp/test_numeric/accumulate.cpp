#include<iostream>
#include<vector>
#include<numeric>
#include<functional>
// #include<ranges> // C++20

// How to run this file
//      g++ accumulate.cpp -o accumulate -std=c++20
//      ./accumulate
// Note: std::hash<int>{} 默认返回原值，因此 fn(num) == num

using namespace std;
int main() {
    auto arraySum = [h = std::hash<int>()] (const vector<int>& v) -> int {
        return accumulate(v.begin(), v.end(), 0, [&h](int a, int b) {
            return a + b;
        });
    };

    auto arrayHash = [] (const vector<int>& arr) -> size_t {
        return accumulate(arr.begin(), arr.end(), 0u, [&](size_t acc, int num) {
            cout << "acc: " << acc << " num: " << num << " fn(num): " << std::hash<int>()(num) << endl;
            return (acc << 1) ^ std::hash<int>()(num);
        });
    };

    vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    cout << "Sum: " << arraySum(v) << endl;
    
    auto h = arrayHash(v);     // 1 ^ 2 ^ 3 ^ 4 ^ 5 ^ 6 ^ 7 ^ 8 ^ 9 ^ 10 ^ 11
    cout << "Hash: " << h << endl;
    return 0;
}