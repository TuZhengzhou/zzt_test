#include <iostream>
#include <vector>
using namespace std;

class Solution {
public:
    vector<int> used;
    vector<vector<int>> ans;
    vector<int> perm;
    int n;
    void dfs(vector<int>& nums) {
        if(perm.size() == n) ans.emplace_back(perm);

        for(int i = 0; i < perm.size(); i++) {
            cout << "perm[" << i << "] = " << perm[i] << endl;
        }

        for(int i = 0; i < n; i++) {
            if(used[i] == 1) continue;

            perm.push_back(nums[i]);
            used[i] = 1;
            dfs(nums);
            used[i] = 0;
            perm.pop_back();
        }
        return;
    }
    vector<vector<int>> permute(vector<int>& nums) {
        n = nums.size();
        
        for(int i = 0; i < n; i++) used.push_back(0);

        dfs(nums);
        return ans;
    }
};

int main() {
    Solution sol;
    vector<int> nums = {1, 2, 3};
    vector<vector<int>> ans = sol.permute(nums);
    for(int i = 0; i < ans.size(); i++) {
        for(int j = 0; j < ans[i].size(); j++) {
            cout << ans[i][j] << " ";
        }
        cout << endl;
    }
    return 0;
}