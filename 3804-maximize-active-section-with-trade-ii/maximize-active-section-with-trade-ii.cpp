#include <vector>
#include <string>
#include <algorithm>
#include <climits>
using namespace std;

class Solution {
public:
    vector<int> maxActiveSectionsAfterTrade(string s, vector<vector<int>>& queries) {
        int n = s.size();
        const int NEG = INT_MIN / 2;
        const int POS = INT_MAX / 2;

        // 1. Run-length encode s
        vector<int> value_;
        vector<int> startArr, endArr;
        {
            int i = 0;
            while (i < n) {
                int j = i;
                while (j < n && s[j] == s[i]) j++;
                value_.push_back(s[i] == '1' ? 1 : 0);
                startArr.push_back(i);
                endArr.push_back(j - 1);
                i = j;
            }
        }
        int k = value_.size();

        vector<int> run_of(n);
        for (int idx = 0; idx < k; idx++)
            for (int p = startArr[idx]; p <= endArr[idx]; p++)
                run_of[p] = idx;

        int total_ones = 0;
        for (char c : s) if (c == '1') total_ones++;

        vector<int> length(k);
        for (int i = 0; i < k; i++) length[i] = endArr[i] - startArr[i] + 1;

        vector<int> oneLen(k), zeroLen(k), adjSum(k, NEG);
        for (int i = 0; i < k; i++) {
            oneLen[i]  = (value_[i] == 1) ? length[i] : POS;
            zeroLen[i] = (value_[i] == 0) ? length[i] : NEG;
        }
        for (int i = 0; i < k; i++) {
            if (value_[i] == 1) {
                int left  = (i > 0)     ? length[i - 1] : 0;
                int right = (i < k - 1) ? length[i + 1] : 0;
                adjSum[i] = left + right;
            }
        }

        // 2. Sparse tables
        vector<int> LOG(k + 1, 0);
        for (int i = 2; i <= k; i++) LOG[i] = LOG[i / 2] + 1;

        auto build = [&](vector<int>& arr, auto func) {
            int m = arr.size();
            vector<vector<int>> table;
            table.push_back(arr);
            int j = 1;
            while ((1 << j) <= m) {
                vector<int>& prev = table[j - 1];
                int half = 1 << (j - 1);
                int sz = m - (1 << j) + 1;
                vector<int> cur(sz);
                for (int t = 0; t < sz; t++)
                    cur[t] = func(prev[t], prev[t + half]);
                table.push_back(cur);
                j++;
            }
            return table;
        };

        auto minFunc = [](int a, int b) { return min(a, b); };
        auto maxFunc = [](int a, int b) { return max(a, b); };

        vector<vector<int>> min_table  = build(oneLen, minFunc);
        vector<vector<int>> maxz_table = build(zeroLen, maxFunc);
        vector<vector<int>> maxa_table = build(adjSum, maxFunc);

        auto rq = [&](vector<vector<int>>& table, int l, int r, auto func, int def) -> int {
            if (l > r) return def;
            int jj = LOG[r - l + 1];
            return func(table[jj][l], table[jj][r - (1 << jj) + 1]);
        };

        int q = queries.size();
        vector<int> ans(q);
        for (int qi = 0; qi < q; qi++) {
            int l = queries[qi][0], r = queries[qi][1];
            int a = run_of[l];
            int b = run_of[r];
            int gain = 0;

            if (a != b) {
                int clipA = endArr[a] - l + 1;
                int clipB = r - startArr[b] + 1;
                int lo = a + 1, hi = b - 1;

                int minOne  = rq(min_table, lo, hi, minFunc, POS);
                int maxZero = rq(maxz_table, lo, hi, maxFunc, NEG);
                if (value_[a] == 0) maxZero = max(maxZero, clipA);
                if (value_[b] == 0) maxZero = max(maxZero, clipB);

                int tripleMax = rq(maxa_table, a + 2, b - 2, maxFunc, NEG);

                if (lo <= hi && value_[a + 1] == 1) {
                    int leftN = clipA;
                    int rightN = (a + 2 == b) ? clipB : length[a + 2];
                    tripleMax = max(tripleMax, leftN + rightN);
                }

                if (lo <= hi && value_[b - 1] == 1 && (b - 1 != a + 1)) {
                    int rightN = clipB;
                    int leftN = (b - 2 == a) ? clipA : length[b - 2];
                    tripleMax = max(tripleMax, leftN + rightN);
                }

                int cands = max(0, tripleMax);
                if (minOne != POS && maxZero != NEG)
                    cands = max(cands, maxZero - minOne);
                gain = cands;
            }

            ans[qi] = total_ones + gain;
        }
        return ans;
    }
};