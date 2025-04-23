#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <ctime>
#include "code_split_head.hpp"
#include "inc2.hpp"

using namespace std;
long size1 = 1999;

int main(int argc, char *argv[])
{
    code_split_head s;
    cout << "Input s.a" << endl;
    cin >> s.a;
    cout << "Input s.b" << endl;
    cin >> s.b;

    cout << "Result of add(s) is " << add(s) << endl;
    cout << "Result of sub(s) is " << sub(s) << endl;

    vector<int> numbers(size1);
    std::srand(std::time(0));
    std::generate(numbers.begin(), numbers.end(), std::rand);
    int count1 = std::count_if(numbers.begin(), numbers.end(), [](int x)
                               { return x % 3 == 0; });
    cout << "Count of numbers divisible by 3 (count1): " << count1 << endl;

    int count2 = 0;
    int count13 = 0;
    std::for_each(
        numbers.begin(),
        numbers.end(),
        [&count2, &count13](int x) -> void
        { count2 += x % 3 == 0; count13 += x % 13 == 0; });
    cout << "Count of numbers divisible by 3 (count2): " << count2 << endl;
    cout << "Count of numbers divisible by 13 (count13): " << count13 << endl;

    return 0;
}