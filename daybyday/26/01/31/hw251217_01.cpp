#include <cmath>
#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
// #includ
using namespace std;
using ll = long long;


int find_first(vector<ll>& recs, int val) {
    int l = 0;
    int r = recs.size() - 1;
    int ans = -1;
    while (l <= r) {
        int mid = (l + r) >> 1;
        if (recs[mid] == val) {
            ans = mid;
            r = mid - 1;
        } else if (recs[mid] < val) {
            l = mid + 1;
        } else {
            r = mid - 1;
        }
    }
    return ans;
}

int find_last(vector<ll>& recs, int val) {
    int l = 0;
    int r = recs.size() - 1;
    int ans = -1;
    while (l <= r) {
        int mid = (l + r) >> 1;
        if (recs[mid] == val) {
            ans = mid;
            l = mid + 1;
        } else if (recs[mid] < val) {
            l = mid + 1;
        } else {
            r = mid - 1;
        }
    }
    return ans;
}

ll my_cnt(vector<ll>& recs, int val) {
    int l = find_first(recs, val);
    if (l == -1) return 0;
    int r = find_last(recs, val);
    return r - l + 1;
}


void proc(vector<ll>& recs, ll base, ll stable, ll& cnt, ll& peak) {
    cnt = 0;
    peak = 0;
    if (base == 1 || base == 0) {
        ll single_cnt = my_cnt(recs, base + stable);
        cnt += single_cnt;
        peak = max(peak, single_cnt);
    } else {
        ll part1 = base;
        while (part1 + stable <= recs[recs.size()-1]) {
            ll single_cnt = my_cnt(recs, part1 + stable);
            cnt += single_cnt;
            peak = max(peak, single_cnt);
            part1 *= base;
        }
    }
}


int main() {
    ll n, m;
    cin >> n >> m;
    vector<ll> recs(n);
    for (int i = 0; i < n; i++) {
        cin >> recs[i];
    }
    for (int i = 0; i < m; i++) {
        ll base, stable;
        cin >> base >> stable;
        ll cnt, peak;        
        proc(recs, base, stable, cnt, peak);
        printf("%lld %lld\n", cnt, peak);
        
    }
}
// 64 位输出请用 printf("%lld")