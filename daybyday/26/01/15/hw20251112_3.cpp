#include <vector>
#include <iostream>
#include <climits>

using namespace std;

int main() {
    int n, m;
    cin >> n >> m;
    printf("n = %d, m = %d\n", n, m);

    vector<int> vec1(n);
    vector<int> vec2(m);

    for (int i = 0; i < n; i++) {
        cin >> vec1[i];
    }
    for (int i = 0; i < m; i++) {
        cin >> vec2[i];
    }

    vector<vector<int>> dp(m, vector<int>(n, 0));
    dp[0][0] = abs(vec1[0] - vec2[0]);
    for (int i = 1; i < n; i++) {
        dp[0][i] = max(dp[0][i-1], abs(vec2[0] - vec1[i]));
    }
    for (int i = 1; i < m; i++) {
        int pre_max = 0;
        for (int j = 0; j < n; j++) {
            dp[i][j] = max(pre_max + abs(vec2[i] - vec1[j]), max(dp[i-1][j], dp[i][j-1]));
            pre_max = max(pre_max, dp[i-1][j]);
        }
    }
    printf("%d", dp[m-1][n-1]);

    // printf("%d", max_v);
    return 0;
}