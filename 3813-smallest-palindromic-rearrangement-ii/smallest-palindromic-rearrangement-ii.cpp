class Solution {
public:
    // CAP must be >= the largest k this problem can ever hand you.
    // Adjust this single constant if the actual constraint on k differs.
    static const long long CAP = 1000001;

    // Exact (never floating-point) count of distinct arrangements of
    // the multiset described by cnt[0..25], capped at CAP+1.
    // Safe to compare directly against any k in [1, CAP].
    long long countWays(array<int, 26> cnt) {
        long long ways = 1;
        long long placedTotal = 0;

        for (int c = 0; c < 26; c++) {
            for (int j = 1; j <= cnt[c]; j++) {
                placedTotal++;
                // Widen to __int128 for the multiply so it can never
                // overflow before the division brings it back down.
                __int128 numerator = (__int128)ways * (__int128)placedTotal;
                ways = (long long)(numerator / j); // always exact — no remainder
                if (ways > CAP) {
                    return CAP + 1; // "big enough" sentinel, safe to stop early
                }
            }
        }
        return ways;
    }

    string smallestPalindrome(string s, int k) {
        long long remaining = k;

        int n = s.size();
        array<int, 26> full{};
        for (char ch : s) full[ch - 'a']++;

        // Odd-length palindromes have exactly one odd-count letter,
        // which sits in the middle and is excluded from the "half".
        char middle = 0;
        array<int, 26> half{};
        for (int c = 0; c < 26; c++) {
            if (full[c] % 2 == 1) middle = 'a' + c;
            half[c] = full[c] / 2;
        }

        int halfLen = n / 2;

        // Quick reject: fewer than k arrangements exist in total.
        if (countWays(half) < remaining) return "";

        string result;
        result.reserve(halfLen);

        for (int pos = 0; pos < halfLen; pos++) {
            bool placed = false;
            for (int c = 0; c < 26; c++) {
                if (half[c] == 0) continue;

                half[c]--; // tentatively place this letter
                long long ways = countWays(half);

                if (ways >= remaining) {
                    result += char('a' + c);
                    placed = true;
                    break; // commit, move to next slot
                } else {
                    remaining -= ways; // skip all arrangements starting with this letter
                    half[c]++;         // undo, try the next letter
                }
            }
            if (!placed) return ""; // defensive guard, shouldn't trigger given the check above
        }

        // Mirror the half (+ middle char) into the full palindrome.
        string reversedHalf = result;
        reverse(reversedHalf.begin(), reversedHalf.end());

        string answer = result;
        if (middle) answer += middle;
        answer += reversedHalf;
        return answer;
    }
};