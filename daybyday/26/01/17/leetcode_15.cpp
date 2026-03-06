#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;


class Solution {
public:
    void erase_if_count_greater_three(vector<int>& nums) {
        int idx = 0;
        int i = 0;
        while (i < nums.size()) {
            int old_i = i;
            int value = nums[i];
            int value_cnt = 0;
            while (i < nums.size() && nums[i] == value) {
                value_cnt += 1;
                i++;
            }
            int copy_cnt = min(value_cnt, 3);
            for (int j = 0; j < copy_cnt; j++) {
                nums[idx + j] = nums[old_i + j];
            }
            idx += copy_cnt;
        }
        printf("idx = %d\n", idx);
        auto iter = nums.begin();
        advance(iter, idx);
        nums.erase(iter, nums.end());
    }
    vector<vector<int>> threeSum(vector<int>& nums) {
        sort(nums.begin(), nums.end());

        erase_if_count_greater_three(nums);

        vector<vector<int>> ans;
        int i = 0;
        while(i < nums.size()) {
            int target = - nums[i];
            int j = i + 1;
            int k = nums.size() -1;
            vector<int> cur = {nums[i]};
            while (j < k) {
                int sum = nums[j] + nums[k];
                if (nums[j] + nums[k] == target) {
                    ans.push_back({nums[i], nums[j], nums[k]});
                    j++;
                    while (j < k && nums[j] == nums[j-1]) {
                        j++;
                    }
                    k--;
                    while (j < k && nums[k] == nums[k+1]) {
                        k++;
                    }
                } else if (sum > target) {
                    k--;
                    while (j < k && nums[k] == nums[k+1]) {
                        k++;
                    }
                } else {
                    j++;
                    while (j < k && nums[j] == nums[j-1]) {
                        j++;
                    } 
                }
            }
            i++;
            while (i < nums.size() && nums[i] == nums[i-1]) {
                i++;
            }
        }
        return ans;
    }
};

int main() {

    vector<int> input = {2,-3,0,-2,-5,-5,-4,1,2,-2,2,0,2,-4,5,5,-10};
    auto ans = Solution{}.threeSum(input);

    std::greater<int> gr;
    std::less<int> le;
}