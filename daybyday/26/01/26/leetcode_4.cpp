#include <vector>

using namespace std;

class Solution {
public:
    int find_kth_num(vector<int>& nums1, vector<int>& nums2, int l1, int l2, int k) {
        if (l1 == nums1.size() -1) {
            return nums2[l2 + k - 1];
        }
        if (l2 == nums2.size()-1) {
            return nums1[l1 + k - 1];
        }
        if (k == 1) {
            return min(nums1[0], nums2[0]);
        }
        int idx1 = min(l1 + k/2 - 1, (int)nums1.size()-1);
        int idx2 = min(l2 + k/2 - 1, (int)nums2.size()-1);
        int ret;
        if (nums1[idx1] <= nums2[idx2]) {
            int cnt = idx1 - l1 + 1;
            ret = find_kth_num(nums1, nums2, idx1+1, l2, k - cnt);
        } else {
            int cnt = idx2 - l2 + 1;
            ret = find_kth_num(nums1, nums2, l1, idx2 + 1, k - cnt);
        }
        return ret;
    }
    double findMedianSortedArrays(vector<int>& nums1, vector<int>& nums2) {
        int cnt_total = nums1.size() + nums2.size();
        double ret;
        if (cnt_total % 2) {
            ret = find_kth_num(nums1, nums2, 0, 0, cnt_total / 2 + 1);
        } else {
            double n1 = find_kth_num(nums1, nums2, 0, 0, cnt_total / 2);
            double n2 = find_kth_num(nums1, nums2, 0, 0, cnt_total / 2 + 1);
            ret = (n1 + n2) / 2;
        }
        return ret;
    }
};