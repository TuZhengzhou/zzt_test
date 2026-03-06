#include <stdlib.h>
#include <iostream>


// Definition for singly-linked list.
struct ListNode {
    int val;
    ListNode *next;
    ListNode(int x) : val(x), next(NULL) {}
};

class Solution {
public:
    ListNode *detectCycle(ListNode *head) {
        if (head == nullptr) {
            return nullptr;
        }

        ListNode* fast = head;
        ListNode* slow = head;
        while (fast != nullptr && fast->next != nullptr && fast->next->next != nullptr && fast != slow) {
            fast = fast->next->next;
            slow = slow->next;
            printf("fast: %d, slow: %d\n", fast->val, slow->val);
        }
        if (fast == nullptr || fast->next == nullptr || fast->next->next == nullptr) {
            return nullptr;
        }
        ListNode* ans = head;
        while (ans != slow) {
            ans = ans->next;
            slow = slow->next;
            printf("ans: %d, slow: %d\n", ans->val, slow->val);
        }
        return ans;
    }
};

int main() {
    ListNode a(3);
    ListNode b(2);
    ListNode c(0);
    ListNode d(-4);
    a.next = &b;
    b.next = &c;
    c.next = &d;
    d.next = &b;
    ListNode* ret = Solution{}.detectCycle(&a);

    return 0;
}