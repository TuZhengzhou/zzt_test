#include <cctype>
#include <iostream>
#include <string>
using namespace std;

using ll = long long;

int getnum_l2r(string& line, int l, ll& ans) {
    int i = l;
    if (line[l] == '-') {
        i++;
    }
    ans = 0;
    for ( ; isdigit(line[i]); i++) {
        ans = ans * 10 + line[i] - '0';
    }
    ans *= line[l] == '-' ? -1: 1;
    return i;
}

int getnum_r2l(string& line, int r, ll& ans) {
    int i = r;
    ans = 0;
    ll h = 1;
    for (; isdigit(line[i]); i--, h *= 10) {
        ans += (line[i] - '0') * h;
    }
    return i;
}

struct Node {
    char op;
    ll coef;
    ll citem;
    Node* left;
    Node* right;
};

void op_recover(string& line) {
    auto iter = line.begin();
    while (iter != line.end()) {
        // printf("%c\n", *iter);
        if (isdigit(*iter) && (*(iter+1) == '(' || *(iter+1) == 'x')) {
            iter = line.insert(iter+1, '*');
            iter++;
            continue;
        }
        if (*iter == ')' && (*(iter+1) == 'x' || *(iter+1) == '(' || isdigit(*(iter+1)) )) {
            iter = line.insert(iter+1, '*');
            iter++;
            continue;
        }
        if (*iter == 'x' && (*(iter+1) == '(' || isdigit(*(iter+1)))) {
            iter = line.insert(iter+1, '*');
            iter++;
            continue;
        }
        iter++;
    }
    // printf("\n");
}

int find_op_idx(string& line, int l, int r) {

    int mul_idx = -1;
    int plu_idx = -1;
    int i = l;
    while (i <= r) {
        if (line[i] == '(') {
            int lcnt = 1;
            int rcnt = 0;
            i++;
            while (lcnt != rcnt) {
                lcnt += line[i] == '(';
                rcnt += line[i] == ')';
                i++;
            };
            continue;
        }
        if (line[i] == '*' && mul_idx == -1) {
            mul_idx = i;
        }
        if (line[i] == '+' && plu_idx == -1) {
            plu_idx = i;
        }
        i++;
    }
    if (plu_idx != -1) return plu_idx;
    if (mul_idx != -1) return mul_idx;
    return -1;
}

void get_single_item(string& line, ll& coef, ll& citem, int l, int r) {
    int i = l;
    while (i <= r) {
        while (line[i] == '(') i++;
        if (line[i] == 'x') {
            coef = 1;
            citem = 0;
            return;
        } else if (isdigit(line[i])) {
            coef = 0;
            getnum_l2r(line, i, citem);
            return;
        }
    }
}

bool valid (string& line, int l, int r) {
    int lcnt = 0;
    int rcnt = 0;
    for (int i = l; i <= r && lcnt >= rcnt; i++) {
        lcnt += line[i] == '(';
        rcnt += line[i] == ')';
    }
    return lcnt == rcnt;
}

Node* build(string& line, int l, int r) {
    Node* node = new Node();
    // 去除括号
    if (line[l] == '(' && line[r] == ')' && valid(line, l+1, r-1)) {
        l++;
        r--;
    }

    int op_idx = find_op_idx(line, l, r);
    if (op_idx == -1) {
        get_single_item(line, node->coef, node->citem, l, r);
        // printf("single_item: l = %d, r = %d. [%lld, %lld]\n", l, r, node->coef, node->citem);
        node->op = '.';
    } else {
        
        node->left = build(line, l, op_idx-1);
        node->right = build(line, op_idx+1, r);
        if (line[op_idx] == '+') {
            node->coef = node->left->coef + node->right->coef;
            node->citem = node->left->citem + node->right->citem;
        } else if (line[op_idx] == '*') {
            if (node->left->coef == 0) {
                node->coef = node->right->coef * node->left->citem;
                node->citem = node->right->citem * node->left->citem;
            } else {
                node->coef = node->left->coef * node->right->citem;
                node->citem = node->left->citem * node->right->citem;
            }
        } else {
            printf("error: 125\n");
        }
        printf("l = %d, r = %d, left: %s, right: %s, op = %c[%d], [%lld, %lld]\n", l, r, line.substr(l, op_idx-l).c_str(), line.substr(op_idx+1, r-op_idx).c_str(), line[op_idx], op_idx, node->coef, node->citem);
    }

    return node;
}

int main() {
    string line;
    cin >> line;
    op_recover(line);

    cout << line << endl;
    int idx_e;
    ll coef = 1;
    ll citem = 0;
    ll citem_right;
    for (size_t i = 0; i < line.size(); i++) {
        // printf("%lu, [%c]\n", i, line[i]);
        if (line[i] == '=') {
            idx_e = i;
            break;
        }
    }
    // printf("ide = %d, %c\n", idx_e, line[idx_e]);
    Node* node = build(line, 0, idx_e - 1);
    // compute(node, coef, citem);
    printf("%lld, %lld\n", node->coef, node->citem);
    getnum_l2r(line, idx_e+1, citem_right);
    printf("%lld\n", citem_right);

    ll ans = (citem_right - node->citem) / node->coef;
    printf("%lld\n", ans);
}