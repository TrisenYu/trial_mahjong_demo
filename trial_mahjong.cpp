/**
 * SPDX-LICENSE-IDENTIFIER: GPL2.0-ONLY
 * (C) 2024 Author: <kisfg@hotmail.com>
 *
 * This program is free software: you can redistribute it and/or modify it under
 *the terms of the GNU General Public License as published by the Free Software
 *Foundation, either version 3 of the License, or (at your option) any later
 *version. This program is distributed in the hope that it will be useful, but
 *WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with
 *this program. If not, see <https://www.gnu.org/licenses/>.
 *
 **/
#include "trial_mahjong.h"

trial_mahjongs_in_hand::trial_mahjongs_in_hand(
    unsigned char *array,
    vector<unsigned long long> (*arr)(unsigned char *)) {
    vector<unsigned long long> tmp = arr(array);
    this->w                        = tmp[0];
    this->t                        = tmp[1];
    this->b                        = tmp[2];
    this->f                        = tmp[3];
    this->pw                       = 0x0'444'444'444 - this->w;
    this->pt                       = 0x0'444'444'444 - this->t;
    this->pb                       = 0x0'444'444'444 - this->b;
    this->pf                       = 0x0'444'444'444 - this->f;
    this->event = (profile_datum *)calloc(BASIC_LIMIT, sizeof(profile_datum));
    this->cnt   = 0;
}

static inline short get_cnt(unsigned long long val, unsigned char maxn = 9) {
    short res = 0, tmp = 0, cnt = 0;
    while (val && cnt < maxn) {
        tmp = val & 0x7;
        if (tmp >= 5) { return -1; }
        res += tmp;
        val >>= 4;
        cnt++;
    }
    return res;
}

/// @brief 计算给定类型牌的张数。
/// @param val 某一类型的牌值
/// @return 类型内的牌数。如果牌没有分布在 0~4，则返回 -1。正常返回牌内显示的值。
short trial_mahjongs_in_hand::get_total_cnt() {
    short res    = 0;
    short tmp[4] = {
        get_cnt(this->w),
        get_cnt(this->t),
        get_cnt(this->b),
        get_cnt(this->f, 7)};
    for (int i = 0; i < 4; i++) {
        if (tmp[i] < 0) { return 0; }
        res += tmp[i];
    }
    if (res < CARDS_OF_BASIC_HU || res > MAXN_CARDS_OF_HU) { return 0; }
    return res;
}

/// @brief 十三幺判断
/// @return 判断结果。0为不是，1为是。
unsigned char trial_mahjongs_in_hand::_13_orphans_() {
    long long tmp[4] = {
        static_cast<long long>(this->w),
        static_cast<long long>(this->t),
        static_cast<long long>(this->b),
        static_cast<long long>(this->f)};
#define mask1 _d2v(1) | _d2v(9)
#define mask2 0x0'001'111'111
    tmp[3] -= mask2;
    if (tmp[3] && !is_pair(static_cast<unsigned long long>(tmp[3]))) {
        return 0;
    }

    for (int i = 0; i < 3; i++) {
        tmp[i] -= mask1;
        if (tmp[i] && tmp[i] != _d2v(1) && tmp[i] != _d2v(9)) { return 0; }
    }
#undef mask1
#undef mask2
    return this->get_total_cnt() == CARDS_OF_BASIC_HU;
}
/// @brief 七对子判断
/// @return 直接返回牌值。
unsigned char trial_mahjongs_in_hand::_7_pairs_4_quadruplets() {
    unsigned long long tmp[4] = {this->w, this->t, this->b, this->f};
    unsigned char res[2]      = {0};
    for (int i = 0; i < 4; i++) {
        for (; tmp[i]; tmp[i] >>= 4) {
            unsigned char curr = tmp[i] & 0x7;
            if (!curr) { continue; }
            if (curr & 1) { return 0; }
            res[(curr >> 2)]++;
        }
    }
    if (res[0] > 1) { return res[0] + (res[1] << 1) == 7 ? _7_PAIRS : 0; }
    if (res[1] == 4 && res[0] == 1) { return _4_QUADRUPLETS; }
    if (res[1] == 3 && res[0] == 1) { return _7_PAIRS; }
    return 0;
}

// 七星不靠，要求字全。
unsigned char trial_mahjongs_in_hand::_7_alone() {
    unsigned char note[3]     = {0};
    unsigned long long tmp[3] = {this->w, this->t, this->b};
    for (int i = 0; i < A33; i++) {
        for (int j = 0; j < 3; j++) {
            note[j] = fetch_val(tmp[permute_seq[i][j]], _ofs_[0 + j], 0x3) +
                      fetch_val(tmp[permute_seq[i][j]], _ofs_[3 + j], 0x3) +
                      fetch_val(tmp[permute_seq[i][j]], _ofs_[6 + j], 0x3);
        }
        if (note[0] & note[1] & note[2]) { break; }
    }
    if (!(note[0] & note[1] & note[2]) || (note[0] + note[1] + note[2]) != 7) {
        return 0;
    }
    return 0x0'001'111'111 == this->f;
}

// 全不靠 147 258 369 | abcx efg 中单抽抽够 13 张听其中的任何一张还没有在手上出现过的牌。
unsigned char trial_mahjongs_in_hand::all_alone() {
    unsigned long long arr[4] = {this->w, this->t, this->b, this->f};
    unsigned char cnt = 0, note[3] = {0};
    while (arr[3]) {
        unsigned char tmp = arr[3] & 0x7;
        if (tmp > 1) { return 0; }
        cnt += tmp;
        arr[3] >>= 4;
    }
    for (int i = 0; i < A33; i++) {
        for (int j = 0; j < 3; j++) {
            // mask 严格是 3
            unsigned long long tmp = arr[permute_seq[i][j]];

            unsigned char a = fetch_val(tmp, _ofs_[0 + j], 0x3);
            unsigned char b = fetch_val(tmp, _ofs_[3 + j], 0x3);
            unsigned char c = fetch_val(tmp, _ofs_[6 + j], 0x3);
            if (a > 1 || b > 1 || c > 1) { return 0; }
            if (a == 1 && b == 1 && c == 1) {
                note[permute_seq[i][j]] = 1;
                break;
            }
        }
        if ((note[0] & note[1] & note[2])) { break; }
    }
    if (!(note[0] & note[1] & note[2])) { return 0; }

    return cnt + (note[0] + note[1] + note[2]) * 3 == CARDS_OF_BASIC_HU;
}

// 组合龙 147 258 369 全，其它不管。即剩下一碰/顺和一对。
unsigned char trial_mahjongs_in_hand::knitted_straight() {
    // 挑出 147 258 369
    // 挑出顺子、碰、对子。
    unsigned long long arr[4] = {this->w, this->t, this->b, this->f};
    unsigned char note[4]     = {0};
    unsigned int *ptr         = (unsigned int *)note;
    for (int i = 0; i < A33; i++) {
        *ptr = 0;
        for (int j = 0; j < 3; j++) {
            unsigned long long &tmp = arr[permute_seq[i][j]];

            unsigned char a = fetch_val(tmp, _ofs_[0 + j], 0x7);
            unsigned char b = fetch_val(tmp, _ofs_[3 + j], 0x7);
            unsigned char c = fetch_val(tmp, _ofs_[6 + j], 0x7);
            if (!(a | b | c)) { continue; }
            note[permute_seq[i][j]] = 1;
            tmp -= (1ll << _ofs_[0 + j]) | (1ll << _ofs_[3 + j]) |
                   (1ll << _ofs_[6 + j]);
        }
        if (*ptr) { break; }
    }
    if (!*ptr) { return 0; }

    // 下标 0 对应到面子顺子或者杠，1 对应到对子。2 对应到字牌的计数器。
    note[0] = note[1] = 0;
    for (; arr[3] && note[3] < 7; note[3]++, arr[3] >>= 4) {
        unsigned char curr = arr[3] & 0x7;
        if (!curr) { continue; }
        unsigned char choice = (curr == 2);
        if (curr == 1 || note[choice]) { return 0; }
        //  010  100 011
        note[choice] = 1;
    }

    if (note[0] && note[1]) { return !arr[0] && !arr[1] && !arr[2]; }
    for (int i = 0; i < 3; i++) {
#define internal_auxiliary(__a, __j)                                           \
    do {                                                                       \
        switch (__a) {                                                         \
        case 1: {                                                              \
            return 0;                                                          \
        }                                                                      \
        case 2: {                                                              \
            if (note[1]) { return 0; }                                         \
            note[1] = 1;                                                       \
            arr[i] -= pair_mask[__j];                                          \
            break;                                                             \
        }                                                                      \
        case 3:                                                                \
        case 4: {                                                              \
            if (note[0]) { return 0; }                                         \
            note[0] = 1;                                                       \
            arr[i] -= static_cast<unsigned long long>(__a) << _ofs_[__j];      \
            break;                                                             \
        }                                                                      \
        }                                                                      \
    } while (0)
        for (int j = 0; j < 7; j++) {
            unsigned char a = fetch_val(arr[i], _ofs_[j + 0], 0x7);
            if (!a) { continue; }
            unsigned char b = fetch_val(arr[i], _ofs_[j + 1], 0x7);
            unsigned char c = fetch_val(arr[i], _ofs_[j + 2], 0x7);
            if (!(a && b && c)) {
                internal_auxiliary(a, j);
                continue;
            }
            unsigned long long choice = seq_masks[j];
            if (!(j <= 5 && a == 2)) {  // 2111
                if (note[0]) { return 0; }
                note[0] = 1;
                arr[i] -= choice;
                continue;
            }
            unsigned char tmp = fetch_val(arr[i], _ofs_[j + 3], 0x7);
            if (tmp >= 1 && tmp <= 4) {
                choice = seq_masks[j + 1];
                if (note[1]) { return 0; }
                note[1] = 1;
                arr[i] -= pair_mask[j];
            }
            if (note[0]) { return 0; }
            note[0] = 1;
            arr[i] -= choice;
        }
        unsigned char a = fetch_val(arr[i], _ofs_[7], 0x7);
        unsigned char b = fetch_val(arr[i], _ofs_[8], 0x7);
        internal_auxiliary(a, 7);
        internal_auxiliary(b, 8);
#undef internal_auxiliary
    }
    return note[0] == note[1] && note[1] == 1;
}

unsigned char trial_mahjongs_in_hand::normal_hu(
    unsigned char pair,
    unsigned char seq_trip) {
#define remained_part(ptr, i)                                                  \
    do {                                                                       \
        unsigned char a = fetch_val(*ptr, _ofs_[i], 0x7);                      \
        switch (a) {                                                           \
        case 0: break;                                                         \
        case 1: return 0;                                                      \
        case 2: {                                                              \
            *ptr -= pair_mask[i];                                              \
            if (this->normal_hu(pair + 1, seq_trip)) {                         \
                *ptr += pair_mask[i];                                          \
                return 1;                                                      \
            }                                                                  \
            *ptr += pair_mask[i];                                              \
            break;                                                             \
        }                                                                      \
        case 3: {                                                              \
            *ptr -= trip_mask[i];                                              \
            if (this->normal_hu(pair, seq_trip + 1)) {                         \
                *ptr += trip_mask[i];                                          \
                return 1;                                                      \
            }                                                                  \
            *ptr += trip_mask[i];                                              \
            break;                                                             \
        }                                                                      \
        case 4: {                                                              \
            *ptr -= kong_mask[i];                                              \
            if (this->normal_hu(pair, seq_trip + 1)) {                         \
                *ptr += kong_mask[i];                                          \
                return 1;                                                      \
            }                                                                  \
            *ptr += pair_mask[i];                                              \
            if (this->normal_hu(pair + 1, seq_trip)) {                         \
                *ptr += pair_mask[i];                                          \
                return 1;                                                      \
            }                                                                  \
            *ptr += pair_mask[i];                                              \
            break;                                                             \
        }                                                                      \
        }                                                                      \
    } while (0)
#define loop_handler(ptr)                                                      \
    do {                                                                       \
        for (int i = 0; i < 7; i++) {                                          \
            unsigned char a = fetch_val(*ptr, _ofs_[i], 0x7);                  \
            if (!a) { continue; }                                              \
            unsigned char b = fetch_val(*ptr, _ofs_[i + 1], 0x7);              \
            unsigned char c = fetch_val(*ptr, _ofs_[i + 2], 0x7);              \
            if (a >= 1 && b >= 1 && c >= 1) {                                  \
                *ptr -= seq_masks[i];                                          \
                if (this->normal_hu(pair, seq_trip + 1)) {                     \
                    *ptr += seq_masks[i];                                      \
                    return 1;                                                  \
                }                                                              \
                *ptr += seq_masks[i];                                          \
            }                                                                  \
            remained_part(ptr, i);                                             \
        }                                                                      \
    } while (0)
#define one_suite(ptr)                                                         \
    do {                                                                       \
        loop_handler(ptr);                                                     \
        remained_part(ptr, 7);                                                 \
        remained_part(ptr, 8);                                                 \
    } while (0)

    if (pair > 1 || seq_trip > 4) { return 0; }
    if (pair == 1 && seq_trip == 4) {
        return !this->w && !this->t && !this->b && !this->f;
    }
    if (this->w) { one_suite(&this->w); }
    if (this->t) { one_suite(&this->t); }
    if (this->b) { one_suite(&this->b); }
    for (int i = 0; i < 7; i++) { remained_part(&this->f, i); }

    return !this->w && !this->t && !this->b && !this->f;
#undef remained_part
#undef loop_handler
#undef one_suite
}

int trial_mahjongs_in_hand::is_in_table() {
    this->w, this->t, this->b, this->f;
    // 前几个等价于 5^9 内挑组件出来判断.
    // 后一个相当于从 5^7 的范围内挑。
    // 但是归根到底存起来总共就需要穷举 12*8 = 96 bits 的位向量。
    return INT_MAX;
}

void trial_mahjongs_in_hand::add_item_to_table(int res) {
    std::fstream f;
    f.open(hu_table, std::ios::out | std::ios::app);
    if (!f.is_open()) {
        puts("unable to open file!");
        return;
    }
    /// TODO
    f << "\n";
}

int trial_mahjongs_in_hand::hu_checker() {
    int res = this->is_in_table();
    if (res != INT_MAX) { return res; }
    if (!this->get_total_cnt()) {
        res = INVALID_HU;
        goto terminated;
    }
    if (this->_13_orphans_()) {
        res = _13_ORPHANS;
        goto terminated;
    }
    res = this->_7_pairs_4_quadruplets();
    if (res) { goto terminated; }
    if (this->_7_alone()) {
        res = _7_ALONE;
        goto terminated;
    }
    if (this->all_alone()) {
        res = ALL_ALONE;
        goto terminated;
    }
    if (this->normal_hu()) {
        res = NORMAL_HU;
        goto terminated;
    }
    if (this->knitted_straight()) {
        res = KNITTED_STRAIGHT;
        goto terminated;
    }
terminated:
    this->add_item_to_table(res);
    return res;
}

/// @brief
/// @param inp 玩家手牌
/// @param id 玩家
/// @param arr 所有人的操作历史。
static void evaluator(unsigned long long inp[4], unsigned char id) {
    // clang-format off
    short condition[4] = {
        get_cnt(inp[0]), 
        get_cnt(inp[1]), 
        get_cnt(inp[2]), 
        get_cnt(inp[3])
    };
    // clang-format on

    unsigned char domain = 0;
    short rec            = -1;
    // 参考所有人的操作历史。
}