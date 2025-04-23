// class Solution {
// private:
//     int partition(vector<int>& nums, int l, int h) {
//         int l_old = l;
//         int h_old = h;
//         int x = nums[l];
//         while(l < h) {
//             while(l < h && nums[h] > x) h--;
//             // if(l < h) nums[l++] = nums[h];
//             while(l < h && nums[l] < x) l++;
//             // if(l < h) nums[h--] = nums[l];
//             if(l < h) swap(nums[l], nums[h]);
//         }
//         // nums[l] = x;
//         // if(nums[l_old] == nums[h_old]) return (l_old + h_old) / 2;

//         return l;
//     }
//     int findKthLargest(vector<int>& nums, int k, int l, int h) {
//         int mid = partition(nums, l, h);
//         if(k == h - mid + 1)    return nums[mid];
//         if(k > h - mid + 1)     return findKthLargest(nums, k + mid - h - 1, l, mid-1);
//         else                    return findKthLargest(nums, k, mid + 1, h);
//     }

// public:
//     int findKthLargest(vector<int>& nums, int k) {
//        return findKthLargest(nums, k, 0, nums.size()-1);
//     }
// };
#include <iostream>
#include <vector>
using namespace std;

class Solution {
    public:
        int quickselect(vector<int> &nums, int l, int r, int k) {
            if (l == r)
                return nums[k];
            int partition = nums[l], i = l, j = r;
            while (i < j) {
                while (j > i && nums[j] > partition) j--;
                while (j > i && nums[i] < partition) i++;

                if (i < j) {
                    swap(nums[i], nums[j]);
                    i++;
                    j--;
                }
            }
            if (k <= j)return quickselect(nums, l, j, k);
            else return quickselect(nums, j+1, r, k);
        }
    
        int findKthLargest(vector<int> &nums, int k) {
            int n = nums.size();
            return quickselect(nums, 0, n - 1, n - k);
        }
    };


int main() {
    Solution sol;
    vector<int> nums = {3,4,5,5,6,2,1,3,2};
    cout << sol.findKthLargest(nums, 5) << endl;
    return 0;
}
    