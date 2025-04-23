#include <iostream>
#include <stack>
#include <string>
using namespace std;

// How to run this file
//      g++ lc_394.cpp -o lc_394
//      ./lc_394

class Solution {
    public:
        string decodeString(string s) {
            stack<string> S;
            string ans;
            for(int i = 0; i < s.size(); i++) {
                if(isdigit(s[i])) {
                    int j = i+1;
                    while(j < s.size() && isdigit(s[j])) j++;
                    S.push(s.substr(i, j-i));
                    i = j-1;
                }
                else if(s[i] == '[') continue;
                else if(isalpha(s[i])) {
                    int j = i+1;
                    while(j < s.size() && isalpha(s[j])) j++;
                    if(S.empty()) {
                        ans += s.substr(i, j-i);
                    } else {
                        string pre_s = S.top();
                        if(isdigit(pre_s[0])) 
                            S.push(s.substr(i, j-i));
                        else {
                            S.pop();
                            S.push(pre_s + s.substr(i, j-i));
                        }
                    }
                    i = j-1;
                }
                else if(s[i] == ']') {
                    string module_s = S.top();
                    S.pop();
                    int repeat = atoi(S.top().c_str());
                    S.pop();
    
                    string final_s;
                    for(int i = 0; i < repeat; i++) 
                        final_s += module_s;
                    
                    if(S.empty()) {
                        ans += final_s;
                    } else {
                        string pre_s = S.top();
                        if(isdigit(pre_s[0])) 
                            S.push(final_s);
                        else {
                            S.pop();
                            S.push(pre_s + final_s);
                        }
                    }
                }
            }
            return ans;
        }
    };

int main() {
    Solution sol;
    string s = "3[a]2[bc]";
    cout << sol.decodeString(s) << endl;
    s = "3[a2[cde]b]";
    cout << sol.decodeString(s) << endl;
    return 0;
}