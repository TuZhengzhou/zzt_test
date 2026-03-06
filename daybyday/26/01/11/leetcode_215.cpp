#include <vector>
#include <limits.h>

using namespace std;

class Solution {
public:
    int partition(vector<int>& nums, int l, int r, int old_pivot) {
        // int pivot = nums[r];
        // if (pivot == old_pivot) {
        //     swap(nums[l], nums[r]);
        //     return l;
        // }
        
        // int idx = l;    // 下一个放置小于 pivot 元素的位置 
        // for (int i = l; i < r; i++) {
        //     if (nums[i] < pivot) {
        //         swap(nums[idx], nums[i]);
        //         idx++;
        //     }
        // }
        // swap(nums[idx], nums[r]);
        int pivot = nums[l];
        if (pivot == old_pivot) {
            return l;
        }
        
        int idx = r;    // 下一个放置大于 pivot 元素的位置 
        for (int i = r; i > l; i--) {
            if (nums[i] > pivot) {
                swap(nums[idx], nums[i]);
                idx--;
            }
        }
        swap(nums[idx], nums[l]);
        return idx;
    }
    int findKthLargest(vector<int>& nums, int k, int l, int r, int old_pivot) {
        if (l == r) {
            return nums[l];
        }
        int idx = partition(nums, l, r, old_pivot);
        int rignt_num = r - idx;
        if (rignt_num+1 == k) {
            return nums[idx];
        } else if (rignt_num+1 > k) {
            return findKthLargest(nums, k, idx + 1, r, nums[idx]);
        } else {
            return findKthLargest(nums, k-rignt_num-1, l, idx - 1, nums[idx]);
        }
    }
    int findKthLargest(vector<int>& nums, int k) {
        return findKthLargest(nums, k, 0, nums.size()-1, INT_MAX);
    }
};

// // class Solution {
// // public:
// //     int partition(vector<int>& nums, int l, int r) {
// //         int pivot = nums[r];
// //         int i = l;  // i指向下一个要放置小于pivot元素的位置
        
// //         for (int j = l; j < r; j++) {
// //             if (nums[j] > pivot) {  // 找第k大，所以用大于号
// //                 swap(nums[i], nums[j]);
// //                 i++;
// //             }
// //         }
// //         swap(nums[i], nums[r]);
// //         return i;  // 返回pivot的最终位置
// //     }
    
// //     int findKthLargest(vector<int>& nums, int k, int l, int r) {
// //         if (l == r) {
// //             return nums[l];
// //         }
        
// //         int pivotIndex = partition(nums, l, r);
// //         int leftSize = pivotIndex - l + 1;  // pivot及其左边有多少元素
        
// //         if (leftSize == k) {
// //             return nums[pivotIndex];
// //         } else if (leftSize > k) {
// //             // 第k大元素在左边部分
// //             return findKthLargest(nums, k, l, pivotIndex - 1);
// //         } else {
// //             // 第k大元素在右边部分，注意更新k值
// //             return findKthLargest(nums, k - leftSize, pivotIndex + 1, r);
// //         }
// //     }
    
// //     int findKthLargest(vector<int>& nums, int k) {
// //         return findKthLargest(nums, k, 0, nums.size() - 1);
// //     }
// // };

int main() {
    // vector<int> input = {3,2,3,1,2,4,5,5,6,7,7,8,2,3,1,1,1,10,11,5,6,2,4,7,8,5,6};
    vector<int> input = {7,6,5,4,3,2,1};
    int k = 2;
    int ret = Solution{}.findKthLargest(input, k);
    return 0;
}