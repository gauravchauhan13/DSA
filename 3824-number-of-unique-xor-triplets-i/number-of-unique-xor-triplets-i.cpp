class Solution {
public:
    int uniqueXorTriplets(vector<int>& nums) {
        int n = nums.size();
        if (n == 1) return 1;
        if (n == 2) return 2;
        int b = 32 - __builtin_clz(n); // bit length of n
        return 1 << b;

    }
};