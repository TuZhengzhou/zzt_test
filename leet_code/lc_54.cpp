#include <iostream>
#include <vector>
using namespace std;

// How to run this file
// g++ lc_54.cpp -o lc_54
// ./lc_54

class Solution {
    public:
        vector<int> spiralOrder(vector<vector<int>>& matrix) {
            vector<int> ans;
            int m = matrix.size();
            int n = matrix[0].size();
    
            vector<vector<int>> D = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
            int i = 0; int j = 0;
            int d = 0;
            int rl = 0;
            int rh = m-1;
            int cl = 0;
            int ch = n-1;
            while(true) {
                int rdelta = D[d][0];
                int cdelta = D[d][1];
                while(i >= rl && i <= rh && j >= cl && j <= ch) {
                    ans.push_back(matrix[i][j]);
                    i += rdelta;
                    j += cdelta;
                }
                i -= rdelta;
                j -= cdelta;
                switch (d) {
                    case 0:
                        rl += 1;
                        break;
                    case 1:
                        ch -= 1;
                        break;
                    case 2:
                        rh -= 1;
                        break;
                    default:
                        cl += 1;
                        break;
                }
                d = (d+1) % 4;
                i += D[d][0];
                j += D[d][1];
                if(i < rl || i > rh || j < cl || j > ch) {
                    break;
                }
            }
            return ans;
        }
    };

int main() {
    Solution s;
    vector<vector<int>> matrix = {{1,2,3,4},{5,6,7,8},{9,10,11,12}};
    vector<int> ans = s.spiralOrder(matrix);
    for(int i = 0; i < ans.size(); i++) {
        cout << ans[i] << " ";
    }
    cout << endl;
    return 0;
}