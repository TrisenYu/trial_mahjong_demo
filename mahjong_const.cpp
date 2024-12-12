#include "mahjong_const.h"
vector<unsigned long long> arr_to_4_u64(
    unsigned char arr[ESSENTIAL_CARDS_TYPES]) {
    vector<unsigned long long> res;
    unsigned long long tmp = 0;
    for (int k = 0; k < 3; k++) {
        tmp = 0;
        for (int i = 0; i < 9; i++) {
            tmp |= static_cast<unsigned long long>(arr[k * 9 + i]) << (i << 2);
        }
        res.push_back(tmp);
    }
    tmp = 0;
    for (int i = DONG_FENG; i <= BAI_BAN; i++) {
        tmp |= static_cast<unsigned long long>(arr[i])
               << ((i - DONG_FENG) << 2);
    }
    res.push_back(tmp);
    return res;
}