#include <vector>
#include <cstring>

using namespace std;

class Solution {
public:
    bool canPartition(vector<int>& nums) {
        vector<int> sums(20001, 0);
        int new_flag[20001];
        memset(new_flag, 0, 20001);
        sums[0] = 1;
        int sum = 0;

        for (int i = 0; i < nums.size(); i++) {
            // for (int j = sum; j >= 0; j--) {
            //     if (sums[j] != 0) {
            //         sums[j+nums[i]] = 1;
            //     }
            // }
            memset(new_flag, 0, 20001);
            for (int j = 0; j <= sum; j++) {
                if (sums[j] != 0 && new_flag[j] == 0) {
                    new_flag[j+nums[i]] = (sums[j+nums[i]] == 0);
                    sums[j+nums[i]] = 1;
                }
            }
            sum += nums[i];
        }
        return sum % 2 == 0 && sums[sum/2] == 1;
    }
};

int main() {

    vector<int> input = {1,2,3,4,5,6,7};
    bool ret = Solution{}.canPartition(input);
}