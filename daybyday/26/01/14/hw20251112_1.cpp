#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <climits>

using namespace std;

vector<int> int2bits(int a) {
    vector<int> ans(8, 0);
    for (int i = 0; i < 8; i++) {
        ans[i] = a % 2;
        a /= 2;
    }
    return ans;
}


int main() {
    int k;
    int m;
    cin >> k >> m;
    int value;
    vector<int> bits;
    while (cin >> value) {
        auto ret = int2bits(value);
        bits.insert(bits.end(), ret.begin(), ret.end());
    }
    int n = bits.size();
    int ans_pos = -1;
    int ans_len = INT_MAX;
    int ans_dist = INT_MAX;
    //printf("k = %d, m = %d, n = %d\n", k, m, n);

    int i = 1;
    while (i <= n) {
        while(i <= n && bits[(m+i) % n] == 0) {
            i++;
        }
        if (i > n) break;
        int begin = i;
        while (i <= n && bits[(m+i) % n] == 1) {
            i++;
        }
        int len = i - begin;
        int j = (m + begin) % n;
        int dist = j > m ? j - m : j + n - m;
        //printf("begin = %d, i = %d, len = %d, j = %d, dist = %d\n", begin, i, len, j, dist);

        if (len >= k && (len < ans_len || (len == ans_len && dist < ans_dist))) {
            ans_pos = j;
            ans_len = len;
            ans_dist = dist;
            //printf("here we change. ans_pos = %d, ans_len = %d, ans_dist = %d\n", ans_pos, ans_len, ans_dist);
        }
    }
    printf("%d", ans_pos);
    fflush(stdout);
}
// 64 位输出请用 printf("%lld")