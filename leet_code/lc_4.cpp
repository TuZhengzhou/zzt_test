#include <iostream>
#include <vector>

using namespace std;
class Solution {
    public:
        double findMedianSortedArrays(vector<int>& nums1, vector<int>& nums2) {
            int len1 = nums1.size();
            int len2 = nums2.size();
            if(len1 == 0)   return len2 % 2 == 1 ? nums2[(len2-1) / 2] : (nums2[(len2-1)/ 2] + nums2[(len2-1)/ 2 + 1]) / 2.0;
            if(len2 == 0)   return len1 % 2 == 1 ? nums1[(len1-1) / 2] : (nums1[(len1-1)/ 2] + nums1[(len1-1)/ 2 + 1]) / 2.0;
    
            return 0;
        }
    };

int main() {
    Solution sol;
    vector<int> nums1 = {};
    vector<int> nums2 = {2,3};
    cout << sol.findMedianSortedArrays(nums1, nums2) << endl;
    return 0;
}