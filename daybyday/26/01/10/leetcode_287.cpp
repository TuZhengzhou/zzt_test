#include <vector>

using namespace std;

class Solution {
public:
    int findDuplicate(vector<int>& nums) {
        int size = nums.size();
        int n = size - 1;
        int t = 0;
        for (int i = 1; i <= n; i++) {
            t ^= i;
        }
        for (int i = 0; i < size; i++) {
            t ^= nums[i];
        }
        return t;
    }
};

int main() {
    vector<int> input = {1,2 ,3,4,2};
    int t = Solution{}.findDuplicate(input);
}