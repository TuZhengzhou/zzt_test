#include <vector>
#include <iostream>

using namespace std;
struct ListNode {
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};

class Solution {
public:
    ListNode* reverseKGroup(ListNode* head, int k) {
        if (k == 1) {
            return head;
        }

        ListNode dummyhead;
        dummyhead.next = head;

        ListNode* cur = head;
        ListNode* last_tail = &dummyhead;
        
        while (cur != nullptr) {
            ListNode* start = cur;
            int cnt = 1;
            while (cur->next != nullptr && cnt < k) {
                cur = cur->next;
                cnt++;
            }
            if (cnt < k) {
                break;
            }
            ListNode* next_head = cur->next;

            cur = start;
            ListNode* next = cur->next;
            while (next != next_head) {
                ListNode* next_next = next->next;
                next->next = cur;
                cur = next;
                next = next_next;
            }
            last_tail->next = cur;

            last_tail = start;
            last_tail->next = next_head;
            
            cur = next_head;
        }
        return dummyhead.next;
    }
};

int main() {
    vector<ListNode> vec(5);
    for(int i = 0; i < 5; i++) {
        vec[i].val = i + 1;
        vec[i].next = i == 4 ? nullptr: &vec[i+1];
    }
    int k = 2;
    auto ret = Solution{}.reverseKGroup(&vec[0], k);
    ListNode* cur = ret;
    while (cur != nullptr) {
        printf("%d\n", cur->val);
        cur = cur->next;
    }
}