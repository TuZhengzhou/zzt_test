#include <vector>

using namespace std;

class Solution {
public:
    int partition(vector<int>& nums, int l, int h, int& pivot_cnt) {
        int pivot = nums[h];
        int idx = l;
        pivot_cnt = 1;
        for (int i = l; i < h; i++) {
            pivot_cnt += nums[i] == pivot;
            if (nums[i] < pivot) {
                swap(nums[idx++], nums[i]);
            }
        }
        swap(nums[idx], nums[h]);

        return idx;
    }

    int findKthLargest(vector<int>& nums, int l, int h, int k) {
        if (l == h) {
            return nums[l];
        }
        int mid_cnt = 0;
        int idx = partition(nums, l, h, mid_cnt);
        int right_cnt = h - idx + 1 - mid_cnt;
        int mid_right_cnt = h - idx + 1;
        if (k > right_cnt && k <= mid_right_cnt) {
            return nums[idx];
        }

        int ret;
        if (mid_right_cnt < k) {
            ret = findKthLargest(nums, l, idx-1, k-mid_right_cnt);
        } else {
            ret = findKthLargest(nums, idx+mid_cnt, h, k); 
        }
        return ret;
    }
    int findKthLargest(vector<int>& nums, int k) {
        return findKthLargest(nums, 0, nums.size()-1, k);
    }
};


int main() {

    vector<int> input = {3,3,3,3,4,3,3,3,3};
    int k = 1;
    int ret = Solution{}.findKthLargest(input, k);
}