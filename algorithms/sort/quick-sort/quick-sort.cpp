#include <iostream>
#include <vector>

using namespace std;

int partition(vector<int>& vec, int l, int r) {
    int left = l;
    int right = r;
    while (left < right) {
        while (left < right && vec[right] >= vec[l]) right--;
        while (left < right && vec[left] <= vec[l]) left++;
        if (left < right) swap(vec[left], vec[right]);
    }
    swap(vec[l], vec[left]);
    return left;
}

int quick_sort(vector<int>& vec, int l, int r) {
    if (l >= r) return 0;
    int p = partition(vec, l, r);
    quick_sort(vec, l, p - 1);
    quick_sort(vec, p + 1, r);
    return 0;
}

int main() {
    vector<int> vec = {3, 6, 8, 10, 1, 2, 1};
    quick_sort(vec, 0, vec.size() - 1);
    for (int v : vec) {
        cout << v << ' ';
    }
    cout << endl;
    return 0;
}