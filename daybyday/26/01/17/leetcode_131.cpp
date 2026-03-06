#include <vector>
#include <string>

using namespace std;

class Solution {
public:
    bool isPalindrome(string& s, int i, int j) {
        while(i < j && s[i] == s[j]) {
            i++;
            j--;
        }
        return i >= j;
    }

    void dfs(vector<vector<string>>& ans, vector<string>& cur, vector<vector<int>>& recs, int idx, string& s) {
        if (idx == s.size()) {
            ans.push_back(cur);
            return;
        }

        for (auto cur_tail: recs[idx]) {
            cur.push_back(s.substr(idx, cur_tail-idx+1));
            dfs(ans, cur, recs, cur_tail + 1, s);
            cur.pop_back();
        }
    }

    vector<vector<string>> partition(string s) {
        vector<vector<int>> recs(s.size());
        for (int i = 0; i < s.size(); i++) {
            for (int j = i; j < s.size(); j++) {
                if (isPalindrome(s, i, j)) {
                    recs[i].push_back(j);
                }
            }
        }
        vector<vector<string>> ans;
        vector<string> cur;
        dfs(ans, cur, recs, 0, s);

        return ans;
    }
};

int main() {
    auto ret = Solution{}.partition("aab");
}