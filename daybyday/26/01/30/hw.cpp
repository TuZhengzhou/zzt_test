#include <cstdio>
#include <iostream>
#include <vector>
#include <sstream>
#include <set>
using namespace std;

int main() {
    string line;
    std::getline(cin, line);
    printf("line = %s\n", line.c_str());
    istringstream iss(line);
    vector<int> arr;
    arr.clear();
    int v;
    while (!iss.eof()) {
        printf("iss: [%s]\n", iss.str().c_str());
        printf("flag: %d, %d\n", iss.eof(), iss.fail());
        iss >> v;
        arr.push_back(v);
    }
    int n = arr.size();
    for (int i = 0; i < n; i++) {
        printf("arr[%d] = %d\n", i, arr[i]);
    }
    vector<int> dp(n);
    dp[0] = arr[0];
    dp[1] = max(arr[0], arr[1]);
    for (int i = 2; i < n; i++) {
        dp[i] = max(dp[i-1], dp[i-2]+arr[i]);
    }
    printf("%d\n", dp[n-1]);
}
// 64 位输出请用 printf("%lld")