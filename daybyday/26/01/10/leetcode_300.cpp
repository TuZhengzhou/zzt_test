#include <vector>
#include <string>
#include <cstring>

using namespace std;

class Solution {
public:
    int quick(vector<int>& rec, int l, int r, int target) {
        
        int ans = -1;
        while (l <= r) {
            int mid = (l + r) >> 1;
            if (rec[mid] >= target) {
                ans = mid;
                r = mid - 1;
            } else {
                l = mid + 1;
            }
        }
        return ans;
    }
    // 记录不同长度序列的最小尾部, 随长度单调递增
    int lengthOfLIS(vector<int>& nums) {
        int ans = 1;
        vector<int> rec;
        rec.push_back(nums[0]);
        for (int i = 1; i < nums.size(); i++) {
            // if (nums[i] < rec[0]) {
            //     rec[0] = nums[i];
            //     continue;
            // }

            // for (int j = 0; j < ans-1; j++) {
            //     if (nums[i] > rec[j] && nums[i] < rec[j+1]) {
            //         rec[j+1] = nums[i];
            //         break;
            //     }
            // }

            // if (nums[i] > rec[ans-1]) {
            //     rec.push_back(nums[i]);
            //     ans++;
            // }
            int idx = quick(rec, 0, rec.size()-1, nums[i]);
            if (idx != -1) {
                rec[idx] = nums[i];
            } else {
                rec.push_back(nums[i]);
            }
            
        }
        return rec.size();
    }
};

int main() {
    vector<int> input = {4, 10, 4, 3, 8, 9};
    int ans = Solution{}.lengthOfLIS(input);
}