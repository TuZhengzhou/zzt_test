## 快速排序

快速排序是一种 `O(log n)`的原地排序算法，最坏复杂度为 `O(n^2)`。其基本思想是通过一个划分操作（partition），将数组划分为两部分，然后递归地对两部分进行排序。划分操作的目标是选择一个基准元素（pivot），并将数组重新排列，使得所有小于基准元素的元素都在其左侧，所有大于基准元素的元素都在其右侧。最后，基准元素处于其最终位置。

```c++
// 快速排序的双边循环划分实现
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
```

双边循环划分实现中，交换`swap(vec[left], vec[right])`比较好理解，但为何基准元素最后放置在`left`这个位置？此处，用类似数学归纳法的思路说明`left`这个位置的元素总是小于等于基准元素，因而基准元素放在此处是正确的。
1. 初始时，`left`和`right`分别指向数组的两端。此时，`left`位置的元素是基准元素，显然小于等于基准元素。
2. 假设经过若干次迭代，`left`位置的元素小于等于基准元素。
3. 下一次迭代中，`right`首先左移，
    
    - 如果`right`左侧没有比基准元素小的元素，`right`会一直左移直到与`left`相遇，此次迭代退出。此时`left`位置的元素仍然小于等于基准元素。对应`while (left < right && vec[right] >= vec[l]) right--;`退出
    - 如果`right`左侧有比基准元素小的元素（称之为小值），`right`会停在最右小值处。
        
        - 如果最右小值左侧没有比基准元素大的元素（称之为大值），`left`会一直右移直到与`right`相遇，此次迭代退出。此时`left`位置的元素仍然小于等于基准元素。对应`while (left < right && vec[left] <= vec[l]) left++;`退出
        - 如果最右小值左侧有比基准元素大的元素，`left`会停在最左大值处。此时，交换`swap(vec[left], vec[right])`后，`left`位置的元素变为小值，仍然小于等于基准元素。继续进行下次迭代

4. 综上所述，在退出循环前，`left`位置的元素总是小于等于基准元素。退出循环后，交换`swap(vec[l], vec[left])`，将基准元素放在`left`位置是正确的。
