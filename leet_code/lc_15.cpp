#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

class Solution {
    public:
        int findFirstTarget(vector<int>& nums, int low, int high, int target) {
            int l = low;
            int h = high;
            int mid;
            while(l <= h) {
                mid = (l + h) / 2;
                if(nums[mid] == target) {
                    while(mid - 1 >= l && nums[mid-1] == target) mid = mid - 1;
                    return mid;
                }
                if(nums[mid] < target) {
                    l = mid + 1;
                } else {
                    h = mid - 1;
                }
            }
            return -1;
        }
        vector<vector<int>> threeSum(vector<int>& nums) {
            int n = nums.size();
            sort(nums.begin(), nums.end());
            vector<vector<int>> ans;
            // 枚举 a
            for (int first = 0; first < n; ++first) {
                // 需要和上一次枚举的数不相同
                if (first > 0 && nums[first] == nums[first - 1]) {
                    continue;
                }
                // c 对应的指针初始指向数组的最右端
                int third = n - 1;
                int old_third;
                int target = -nums[first];
                // 枚举 b
                for (int second = first + 1; second < third; ++second) {
                    // 需要和上一次枚举的数不相同
                    if (second > first + 1 && nums[second] == nums[second - 1]) {
                        continue;
                    }
                    old_third = third;
                    third = findFirstTarget(nums, second+1, third, target - nums[second]); // 最坏情况下，时间复杂度为O(n^2 log n)
                    // 如果没有满足的数, third 会等于 -1, 继续下一个 second
                    if(third == -1) {
                        third = old_third;
                        continue;
                    }
                    // 如果指针重合，随着 b 后续的增加
                    // 就不会有满足 a+b+c=0 并且 b<c 的 c 了，可以退出循环
                    if (second >= third) {
                        break;
                    }
                    if (nums[second] + nums[third] == target) {
                        ans.push_back({nums[first], nums[second], nums[third]});
                        third = third - 1;
                    }
                }
            }
            return ans;
        }
    };

// How to run this file
//      g++ lc_15.cpp -o lc_15
//      ./lc_15
int main() {
    // vector<int> nums = {-1, 0, 1, 2, -1, -4};
    // [1,-1,-1,0]
    vector<int> nums = {1, -1, -1, 0};
    Solution s;
    vector<vector<int>> ans = s.threeSum(nums);
    for(auto v: ans) {
        for(auto num: v) {
            cout << num << " ";
        }
        cout << endl;
    }
    return 0;
}