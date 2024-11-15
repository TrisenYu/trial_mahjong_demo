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

#include "mahjong.h"

short trial_mahjongs_in_hand::get_cnt(unsigned long long val) {
    short res = 0, tmp = 0;
    while (val) {
        tmp = val & 0x7;
        if (tmp >= 5) { return -1; }
        res += tmp;
        val >>= 4;
    }
    return res;
}

/// @brief 计算给定类型牌的张数。
/// @param val 某一类型的牌值
/// @return 类型内的牌数。如果牌没有分布在 0~4，则返回 -1。正常返回牌内显示的值。
short trial_mahjongs_in_hand::get_total_cnt() {
    short res    = 0;
    short tmp[4] = {
        this->get_cnt(this->w),
        this->get_cnt(this->t),
        this->get_cnt(this->b),
        this->get_cnt(this->f)};
    for (int i = 0; i < 4; i++) {
        if (tmp[i] < 0) { return -1; }
        res += tmp[i];
    }
    if (res < CARDS_OF_BASIC_HU || res > MAXN_CARDS_OF_HU) { return -1; }
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
        /// TODO: 这里不是等于，而是含有单张，且三者总数加起来为 7
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
            unsigned char a =
                fetch_val(arr[permute_seq[i][j]], _ofs_[0 + j], 0x3);
            unsigned char b =
                fetch_val(arr[permute_seq[i][j]], _ofs_[3 + j], 0x3);
            unsigned char c =
                fetch_val(arr[permute_seq[i][j]], _ofs_[6 + j], 0x3);
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
    for (int i = 0; i < A33; i++) {
        for (int j = 0; j < 3; j++) {
            unsigned long long &tmp = arr[permute_seq[i][j]];
            unsigned char a         = fetch_val(tmp, _ofs_[0 + j], 0x7);
            unsigned char b         = fetch_val(tmp, _ofs_[3 + j], 0x7);
            unsigned char c         = fetch_val(tmp, _ofs_[6 + j], 0x7);
            if (a && b && c) {
                note[permute_seq[i][j]] = 1;
                tmp -= (1ll << _ofs_[0 + j]) | (1ll << _ofs_[3 + j]) |
                       (1ll << _ofs_[6 + j]);
            }
        }
        if (note[0] & note[1] & note[2]) { break; }
    }

    if (!(note[0] & note[1] & note[2])) { return 0; }

    // 1 作为 顺子或者面子，2作为对子。3作为字牌的计数器。
    note[1] = note[2] = 0;
    for (; arr[3] && note[3] < 7; note[3]++, arr[3] >>= 4) {
        unsigned char curr = arr[3] & 0x7;
        if (!curr) { continue; }
        if (curr == 1 || note[(curr & 1) | (curr >> 2)]) { return 0; }
        //  010  100 011
        note[(curr & 1) | (curr >> 2)] = 1;
    }

    if (note[2] && note[1]) { return !arr[0] && !arr[1] && !arr[2]; }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 7; j++) {
            unsigned char a = fetch_val(arr[i], _ofs_[j + 0], 0x7);
            unsigned char b = fetch_val(arr[i], _ofs_[j + 1], 0x7);
            unsigned char c = fetch_val(arr[i], _ofs_[j + 2], 0x7);
            if (!(a >= 1 && b >= 1 && c >= 1 && a <= 4 && b <= 4 && c <= 4)) {
                continue;
            }
            if (note[1]) { return 0; }
            unsigned long long choice = seq_masks[j];
            if (j <= 5 && a == 2) {  // 2111
                unsigned char tmp = fetch_val(arr[i], _ofs_[j + 3], 0x7);
                if (tmp >= 1 && tmp <= 4) { choice = seq_masks[j + 1]; }
            }
            note[1] = 1;
            arr[i] -= choice;
        }

        for (int j = 0; j < 9; j++) {
            unsigned char curr = fetch_val(arr[i], _ofs_[j], 0x7);
            switch (curr) {
            case 0: {
                continue;
            }
            case 2: {
                if (note[2]) { return 0; }
                note[2] = 1;
                arr[i] -= pair_masks[j];
                break;
            }
            case 3:
            case 4: {
                if (note[1]) { return 0; }
                note[1] = 1;
                arr[i] -= (static_cast<unsigned long long>(curr) << _ofs_[j]);
                break;
            }
            default: {
                return 0;
            }
            }
        }
    }
    return note[1] == note[2] && note[2] == 1;
}

// 抽出面子。
void trial_mahjongs_in_hand::trip_functioner(unsigned long long arr[4]) {
    for (int k = 0; k < 3; k++) {
        if (!arr[k]) { continue; }
        for (int l = 0; l < 9; l++) {
            unsigned char curr = fetch_val(arr[k], _ofs_[l], 0x7);
            if (curr >= 3 && curr <= 4) {
                arr[k] -= static_cast<unsigned long long>(curr) << _ofs_[l];
            } else {
                continue;
            }
        }
    }
}
// 抽出顺子
void trial_mahjongs_in_hand::_abc_functioner(unsigned long long arr[4]) {
    for (int k = 0; k < 3; k++) {
        if (!arr[k]) { continue; }
        for (int l = 0; l < 7;) {
            unsigned char a = fetch_val(arr[k], _ofs_[l + 0], 0x7),
                          b = fetch_val(arr[k], _ofs_[l + 1], 0x7),
                          c = fetch_val(arr[k], _ofs_[l + 2], 0x7);
            if (a >= 1 && b >= 1 && c >= 1) {
                arr[k] -= seq_masks[l];
                continue;
            }
            l++;
        }
    }
}

unsigned char trial_mahjongs_in_hand::normal_hu() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 9; j++) {
            if (i == 3 && j >= 7) { break; }
            for (int order = 0; order < 2; order++) {
                unsigned long long arr[4] = {
                    this->w, this->t, this->b, this->f};
                unsigned char tmp = fetch_val(arr[i], _ofs_[j], 0x7);
                // 抽对子。
                if (tmp >= 2) { arr[i] -= pair_masks[j]; }
                if (!order) {
                    this->trip_functioner(arr);
                    this->_abc_functioner(arr);
                } else {
                    this->_abc_functioner(arr);
                    this->trip_functioner(arr);
                }
                if (!(arr[0] | arr[1] | arr[2] | arr[3])) { return 1; }
            }
        }
    }
    return 0;
}
int trial_mahjongs_in_hand::hu_checker() {
    int cnt = this->get_total_cnt();
    if (!cnt) { return INVALID_HU; }
    int res = this->_13_orphans_();
    if (res) { return _13_ORPHANS; }
    res = this->_7_pairs_4_quadruplets();
    if (res) { return res; }
    res = this->_7_alone();
    if (res) { return _7_ALONE; }
    res = this->all_alone();
    if (res) { return ALL_ALONE; }
    res = this->normal_hu();
    if (res) { return NORMAL_HU; }
    res = this->knitted_straight();
    if (res) { return KNITTED_STRAIGHT; }
    return INVALID_HU;
}
