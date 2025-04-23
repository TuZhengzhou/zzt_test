#include <iostream>
#include <vector>
using namespace std;
// Definition for singly-linked list.
struct ListNode {
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};


class Solution {
public:
    ListNode MLDummyNode;
    ListNode* currp1;
    ListNode* currp2;
    ListNode* mergeList(ListNode* l1, ListNode* l2) {
        if (l1 == nullptr) cout << "l1 nullptr" << endl;
        if (l2 == nullptr) cout << "l2 nullptr" << endl;
        ListNode* curr = &MLDummyNode;
        currp1 = l1;
        currp2 = l2;

        while(currp1 != nullptr && currp2 != nullptr) {
            cout << "Curr: " << curr->val << endl;
            if(currp1->val < currp2->val) {
                curr->next = currp1;
                currp1 = currp1->next;
            } else {
                curr->next = currp2;
                currp2 = currp2->next;
            }
            curr = curr->next;
        }
        if(currp1 == nullptr)   curr->next = currp2;
        else                    curr->next = currp1;

        curr = MLDummyNode.next;
        cout << "Merged" << endl;
        while(curr != nullptr) {
            cout << curr->val << endl;
            curr = curr->next;
        }
        return MLDummyNode.next;
    }
    ListNode* mergeKLists(vector<ListNode*>& lists) {
        int listNum = lists.size();
        if(listNum == 0) return nullptr;
        if(listNum == 1) return lists[0];

        MLDummyNode = ListNode();

        int repeat;
        int i;
        int j;
        while(listNum >= 2) {
            repeat = listNum / 2;
            for(i = 0; i < repeat; i++) {
                lists[i] = mergeList(lists[2*i], lists[2*i+1]);
            }
            j = 2*i;
            while (j < listNum) lists[i++] = lists[j++];
            listNum = i;
        }
        return lists[0];
    }
};

int main() {
    // [[1,4,5],[1,3,4],[2,6]]
    ListNode* l1 = new ListNode(1);
    l1->next = new ListNode(4);
    l1->next->next = new ListNode(5);
    ListNode* l2 = new ListNode(1);
    l2->next = new ListNode(3);
    l2->next->next = new ListNode(4);
    ListNode* l3 = new ListNode(2);
    l3->next = new ListNode(6);
    vector<ListNode*> lists;
    lists.push_back(l1);
    lists.push_back(l2);
    lists.push_back(l3);
    Solution sol;
    ListNode* res = sol.mergeKLists(lists);
    ListNode* next;
    while(res != nullptr) {
        cout << res->val << endl;
        next = res->next;
        delete res;
        res = next;
    }
    
    return 0;
}