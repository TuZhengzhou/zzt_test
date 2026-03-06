#include <set>
#include <vector>
#include <algorithm>

using namespace std;

class Solution {
public:
    set<vector<int>> ans;
    bool quick(vector<int>& nums, int l, int h, int target) {
        while (l <= h) {
            int mid = (l + h) >> 1;
            if (nums[mid] == target) {
                return true;
            } else if (nums[mid] > target) {
                h = mid - 1;
            } else {
                l = mid + 1;
            }
        }
        return false;
    }

    void eraseOverThree(vector<int>& vec) {
        int idx;
        int cur_val;
        int cur_val_cnt = 0;
        idx = 1; 
        cur_val = vec[0];
        cur_val_cnt = 1;
        int i = 1;
        while (i < vec.size()) {
            while (vec[i] == cur_val && cur_val_cnt >= 3 && i < vec.size()) {
                i++;
            }
            if (vec[i] == cur_val) {
                cur_val_cnt++;
                swap(vec[i], vec[idx]);
                i++;
                idx++;
            } else {
                cur_val = vec[i];
                cur_val_cnt = 1;
                swap(vec[i], vec[idx]);
                i++;
                idx++;
            }
        }
        auto iter = vec.begin();
        advance(iter, idx);
        vec.erase(iter, vec.end());
    }

    vector<vector<int>> threeSum(vector<int>& nums) {
        ans.clear();
        sort(nums.begin(), nums.end());
        eraseOverThree(nums);
        int l = 0;
        int h = nums.size()-1;
        for ( ; l < h; l++ ) {
            for (int nh = h ; nh > l; nh--) {
                if (quick(nums, l+1, nh-1, -(nums[l]+nums[nh]))) {
                    vector<int> t {nums[l], nums[nh], -(nums[l]+nums[nh])};
                    sort(t.begin(), t.end());
                    ans.insert(t);
                }
            }
            // h--;
        }
        vector<vector<int>> ret;
        for (auto& vec: ans) {
            ret.push_back(vec);
        }
        return ret;
    }
};

int main() {
    vector<int> input = {-1,-1,-1,-1,0,1,2,-1,-4};
    auto ret = Solution{}.threeSum(input);
    return 0;
}