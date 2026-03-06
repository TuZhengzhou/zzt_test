#include <stack>
#include <vector>

using namespace std;

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
    vector<int> inorderTraversal(TreeNode* root) {
        stack<TreeNode*> st;
        st.push(root);
        TreeNode* pre = root;

        vector<int> ans;
        while (!st.empty()) {
            TreeNode* top = st.top();
            if (top->left == nullptr && top->right == nullptr) {    // 叶子结点
                pre = top;
                ans.push_back(top->val);
                st.pop();
                continue;
            }
            // 中间节点
            if (pre == top->right) {
                pre = top;
                st.pop();
            }
            else if (top->left == nullptr || pre == top->left) { // 没有左孩子的节点或已经输出左子树
                pre = top;
                ans.push_back(top->val);

                if (top->right != nullptr) st.push(top->right);
                else st.pop();
                continue;
            } else {    // 有左孩子且没有输出
                pre = top;
                st.push(top->left);
            }
            
        }
        return ans;
    }
};

int main() {
    TreeNode a, b, c;
    a.val = 1;
    b.val = 2;
    c.val = 3;
    a.right = &b;
    b.left = & c;
    auto ret = Solution{}.inorderTraversal(&a);
    return 0;
}