#include <vector>
#include <iostream>

using namespace std;

void merge(vector<int>& left, int l_left, int r_left,\
           vector<int>& right, int l_right, int r_right) {
    if (l_left > r_left || l_right > r_right) {
        return;
    }
    vector<int> temp;
    temp.reserve((r_left - l_left + 1) + (r_right - l_right + 1));
    int i = l_left;
    int j = l_right;
    while (i <= r_left && j <= r_right) {
        if (left[i] <= right[j]) {
            temp.push_back(left[i++]);
        } else {
            temp.push_back(right[j++]);
        }
    }
    while (i <= r_left) {
        temp.push_back(left[i++]);
    }
    while (j <= r_right) {
        temp.push_back(right[j++]);
    }
    int k;
    for (k = 0; k < r_left - l_left + 1; k++) {
        left[l_left + k] = temp[k];
    }
    for (k = 0; k < r_right - l_right + 1; k++) {
        right[l_right + k] = temp[r_left - l_left + 1 + k];
    }
}

void merge_sort(vector<int>& nums, int l, int r) {

    for (int i = 1; i < nums.size(); i *= 2) {
        int base = l;
        while (base <= r) {
            int left_l = base;
            int left_r = min(left_l + i - 1, r);
            int right_l = left_r + 1;
            int right_r = min(right_l + i - 1, r);
            merge(nums, left_l, left_r, nums, right_l, right_r);
            base += right_r - left_l + 1;
        }
    }
}

int main() {
    vector<int> input = {3,4,5,1,2,6,7};
    merge_sort(input, 0, input.size() - 1);

    return 0;
}