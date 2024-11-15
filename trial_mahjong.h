#ifndef _TRIAL_MAHJONG_H_
#define _TRIAL_MAHJONG_H_
#endif

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

/// @brief (val >> i) & mask;
/// @param val
/// @param i
/// @param mask
/// @return (val >> i) & mask;
static inline unsigned short
fetch_val(unsigned long long val, int i, int mask) {
    return (val >> i) & mask;
}
// 将万条筒和风箭都压到 [1-9]
enum trial_type {
    suit_w,  // 万
    suit_t,  // 条
    suit_b,  // 饼
    suit_z,  // 字
    suit_h,  // 花牌
    suit_j,  // 季节
    suit_l   // 癞子
};
#define __leap__  0x04
#define __empty__ 0x0'000'000'000
#define __w__     __empty__
#define __t__     0x1'000'000'000
#define __b__     0x2'000'000'000
#define __h__     0x3'000'000'000
#define __s__     0x4'000'000'000
#define __m__     0x5'000'000'000
#define __1__     0x0'000'000'001 /* 往下为数值 */
#define __2__     0x0'000'000'010
#define __3__     0x0'000'000'100
#define __4__     0x0'000'001'000
#define __5__     0x0'000'010'000
#define __6__     0x0'000'100'000
#define __7__     0x0'001'000'000
#define __8__     0x0'010'000'000
#define __9__     0x0'100'000'000

/// clang-format off
// 这个只能传入 1-9 之内的数字。
#define _d2v(v) (__##v##__)
//! clang-format on

// clang-format off
// 可以通过抽取每个花色内的模式来加速判断模式是否胡。
// 同时应该要注意到一种花色能分解完全只取决于如何划分顺子。
// 如果没有顺子，就只需要考虑如何抽取杠。
constexpr unsigned char _ofs_[] = {
    0x00,
    0x04,
    0x08,
    0x0c,
    0x10,
    0x14,
    0x18,
    0x1c,
    0x20
};
constexpr unsigned long long pair_masks[9] = {
    0x0'000'000'002,  // #define _11_
    0x0'000'000'020,  // #define _22_
    0x0'000'000'200,  // #define _33_
    0x0'000'002'000,  // #define _44_
    0x0'000'020'000,  // #define _55_
    0x0'000'200'000,  // #define _66_
    0x0'002'000'000,  // #define _77_
    0x0'020'000'000,  // #define _88_
    0x0'200'000'000,  // #define _99_
}, seq_masks[7] = {
    0x0'000'000'111,  // #define _123_
    0x0'000'001'110,  // #define _234_
    0x0'000'011'100,  // #define _345_
    0x0'000'111'000,  // #define _456_
    0x0'001'110'000,  // #define _567_
    0x0'011'100'000,  // #define _678_
    0x0'111'000'000   // #define _789_
}, trip_mask[9] = {
    0x0'000'000'003,  // #define _111_ 
    0x0'000'000'030,  // #define _222_ 
    0x0'000'000'300,  // #define _333_ 
    0x0'000'003'000,  // #define _444_ 
    0x0'000'030'000,  // #define _555_ 
    0x0'000'300'000,  // #define _666_ 
    0x0'003'000'000,  // #define _777_ 
    0x0'030'000'000,  // #define _888_ 
    0x0'300'000'000   // #define _999_ 
}, kong_mask[9] = {
    0x0'000'000'004,  // #define _1111_ 
    0x0'000'000'040,  // #define _2222_ 
    0x0'000'000'400,  // #define _3333_ 
    0x0'000'004'000,  // #define _4444_ 
    0x0'000'040'000,  // #define _5555_ 
    0x0'000'400'000,  // #define _6666_ 
    0x0'004'000'000,  // #define _7777_ 
    0x0'040'000'000,  // #define _8888_ 
    0x0'400'000'000   // #define _9999_ 
};
constexpr unsigned long long _sp_mask[3] = {
    _d2v(1)|_d2v(4)|_d2v(7), 
    _d2v(2)|_d2v(5)|_d2v(8), 
    _d2v(3)|_d2v(6)|_d2v(9)
};

// clang-format on

static unsigned char is_pair(unsigned long long val) {
    for (int i = 0; i < 9; i++) {
        if (val == pair_masks[i]) { return 1; }
    }
    return 0;
}

class trial_mahjongs_in_hand {
public:
    unsigned long long w;  // wan
    unsigned long long t;  // tiao
    unsigned long long b;  // bing
    unsigned long long f;  // feng

    // 3*34/8= 13 < 4 * 8 = 32 < 34
    trial_mahjongs_in_hand() {
        this->w = this->t = this->f = this->b = __empty__;
    }
    trial_mahjongs_in_hand(
        unsigned long long wang,
        unsigned long long tiao,
        unsigned long long bing,
        unsigned long long feng)
        : w(wang), t(tiao), b(bing), f(feng) {}
    short get_cnt(unsigned long long val);
    short get_total_cnt();
    int hu_checker();

protected:
    void trip_functioner(unsigned long long arr[4]);
    void _abc_functioner(unsigned long long arr[4]);
    unsigned char _13_orphans_();
    unsigned char _7_pairs_4_quadruplets();
    unsigned char _7_alone();
    unsigned char all_alone();
    unsigned char knitted_straight();
    unsigned char normal_hu();
};