#include <vector>
#include <limits.h>
using namespace std;


class Solution {
public:
    int coinChange(vector<int>& coins, int amount) {
        if (amount == 0) {
            return 0;
        }
        vector<int> marker(amount+1, INT_MAX);
        marker[0] = 0;
        for (int i = 1; i <= amount; i++) {
            for (int j = 0; j < coins.size(); j++) {
                if (i - coins[j] >= 0 && marker[i - coins[j]] < INT_MAX) {
                    marker[i] = min(marker[i], marker[i - coins[j]] + 1);
                }
            }
        }
        return marker[amount] == INT_MAX ? -1 : marker[amount];
    }
};

int main() {
    vector<int> input = {1,2,5};
    int ans = Solution{}.coinChange(input, 11);
}