#include <iostream>
#include <vector>
using namespace std;
class Solution {
public:
    vector<vector<int>> D = {
        {0, -1},
        {0,  1},
        {-1, 0},
        {1,  0}
    };

    int m, n, l;
    bool dfs(vector<vector<char>>& board, vector<vector<bool>>& used, string word, int start, int x, int y) {
        if(board[x][y] != word[start]) return false;
        if(start == word.size() - 1) return true;

        // 没用过 且 相等
        int dx, dy;
        for(auto d: D) {
            dx = d[0];
            dy = d[1];
            if(x+dx >= 0 && x+dx < m && y+dy >= 0 && y+dy < n && used[x+dx][y+dy] == false) {
                used[x+dx][y+dy] = true;
                if (dfs(board, used, word, start+1, x+dx, y+dy)) return true;
                used[x+dx][y+dy] = false;
            }
        }
        return false;
    }
    bool exist(vector<vector<char>>& board, string word) {
        m = board.size();
        n = board[0].size();
        l = word.size();

        vector<vector<bool>> used(m, vector<bool>(n, false));

        for(int i = 0; i < m; i++) {
            for(int j = 0; j < n; j++) {
                if(board[i][j] == word[0]) {
                    used[i][j] = true;
                    if(dfs(board, used, word, 0, i, j))
                        return true;
                    used[i][j] = false;
                }
            }
        }
        return false;
    }
};

// [["A","A","A","A","A","A"],["A","A","A","A","A","A"],["A","A","A","A","A","A"],["A","A","A","A","A","A"],["A","A","A","A","A","A"],["A","A","A","A","A","A"]]
// "AAAAAAAAAAAAAAB"

int main() {
    Solution sol;
    vector<vector<char>> board = {
        {'A', 'A', 'A', 'A', 'A', 'A'},
        {'A', 'A', 'A', 'A', 'A', 'A'},
        {'A', 'A', 'A', 'A', 'A', 'A'},
        {'A', 'A', 'A', 'A', 'A', 'A'},
        {'A', 'A', 'A', 'A', 'A', 'A'},
        {'A', 'A', 'A', 'A', 'A', 'A'}
    };
    string word = "AAAAAAAAAAAAAAB";
    cout << sol.exist(board, word) << endl;
    return 0;
}