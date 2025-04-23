/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode() : val(0), left(nullptr), right(nullptr) {}
 *     TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
 *     TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
 * };
 */

#include <iostream>
using namespace std;

// How to run this file
//      g++ lc_437.cpp -o lc_437
//      ./lc_437

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};

class Solution {
    public:
        int target;
        int dfs(TreeNode* root, int targetSum, bool empty) {
            int ans = 0;
            if((!empty && targetSum == 0))  ans++;
            
            if(root == nullptr) return 0;
            
            ans = ans + dfs(root->left, targetSum - root->val, false);
            ans = ans + dfs(root->left, target, true);
            
            ans = ans + dfs(root->right, targetSum - root->val, false);
            ans = ans + dfs(root->right, target, true);
            
            return ans;
        }
        int pathSum(TreeNode* root, int targetSum) {
            if(root == nullptr) return 0;
    
            target = targetSum;
            return dfs(root, targetSum, true);
        }
    };


int main() {
    Solution sol;
    // 根据给定的数组创建一棵树, 数组是层序遍历的结果
    // [-5,5,-3,3,2,null,0,3,-2,null,1]
    TreeNode* root = new TreeNode(-5);
    root->left = new TreeNode(5);
    root->right = new TreeNode(-3);
    root->left->left = new TreeNode(3);
    root->left->right = new TreeNode(2);
    root->right->left = nullptr;
    root->right->right = new TreeNode(0);
    root->left->left->left = new TreeNode(3);
    root->left->left->right = new TreeNode(-2);
    root->left->right->left = nullptr;
    root->left->right->right = new TreeNode(1);
    cout << sol.pathSum(root, 0) << endl;

    return 0;
}