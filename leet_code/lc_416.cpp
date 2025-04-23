#include <iostream>
#include <vector>
#include <unordered_set>
#include <algorithm>
using namespace std;
class Solution {
    public:
        bool canPartition(vector<int>& nums) {
            int sum = 0;
            int len = nums.size();
            for(int i = 0; i < len; i++) sum += nums[i];
    
            cout << sum << endl;
            if(sum % 2 == 1) return false;
    
            unordered_set<int> uset;
            unordered_set<int> uset1;
            for(int i = 0; i < len; i++) {
                uset1.clear();
                for(auto& v: uset) {
                    if(uset.find(nums[i] + v) == uset.end()) uset1.insert(nums[i] + v);
                }
                for(auto& v: uset1) {
                    uset.insert(v);
                }
                if(uset.find(nums[i]) == uset.end()) uset.insert(nums[i]);
            }
            for_each(uset.begin(), uset.end(), [sum](int value) {
                if(value == sum / 2) {
                    cout << "Find" << endl;
                }
            });
            cout << sum / 2 << endl;
            return uset.find(sum / 2) != uset.end();
        }
    };

int main() {
    Solution sol;
    vector<int> nums = {4,4,4,4,4,4,4,4,8,8,8,8,8,8,8,8,12,12,12,12,12,12,12,12,16,16,16,16,16,16,16,16,20,20,20,20,20,20,20,20,24,24,24,24,24,24,24,24,28,28,28,28,28,28,28,28,32,32,32,32,32,32,32,32,97,99};
    cout << sol.canPartition(nums) << endl;
    return 0;
}