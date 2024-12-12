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

// 不完全信息博弈。迟早得用概率论。
#include "mahjong.h"

mahjong heap[CARDS_OF_HEAP];
unsigned char player[4][ESSENTIAL_CARDS_TYPES];
int heap_cnt = 0, game_over;
info_of_players mamba[4];

/* 判胡 */
/// @brief 通配符转换。这个可能还得结合距离函数做最适配计算。
/// @todo 做了那如何恢复呢？
/// @param arr 当前不含通配符的牌目数组。
/// @param src 通配符的表示。
/// @param dst 待转换的表示。
static inline void match_transitor(
    unsigned char arr[ESSENTIAL_CARDS_TYPES], unsigned char src,
    unsigned char dst, unsigned char val) {
    for (; val; arr[src]--, val--) { arr[dst]++; }
}

/// @brief 以实参判断是否按 147 258 369 分散花色而成牌。
/// @param val 牌的数值。
/// @param ref 牌的参考花色。如 `_1_WAN_`, `_1_TIAO` 或最后的 `_1_TONG`。
/// @param id 花色数组。形如 id[3]。
/// @param idx 在不靠的条件下，截至调用时对应花色的数量数组。形如 idx[3]。
/// @return 判断结果。0 表示不成立，1 表示成立。
inline static unsigned char is_diffused(
    int val, int ref, unsigned char id[3], unsigned char idx[3]) {
#define __1_4_7__(val) ((val) == 1 || (val) == 4 || (val) == 7)
#define __2_5_8__(val) ((val) == 2 || (val) == 5 || (val) == 8)
#define mask1          0x01
#define mask2          0x02

    unsigned char curr = 0x0;
    if (__1_4_7__(val - ref + 1)) {
    } else if (__2_5_8__(val - ref + 1)) {
        curr = mask1;
    } else {
        curr = mask2;
    }
    unsigned char peer1 = curr ^ mask1, peer2 = curr ^ mask2;

    if (peer1 == 0x3) { peer1 ^= curr; }
    if (peer2 == 0x3) { peer2 ^= curr; }
    /* 当前的值要与先前的同花色的一致，否则不符合此类型的胡牌条件。 */
    if (((~id[curr] && id[curr] != ref)) || id[peer1] == ref ||
        id[peer2] == ref) {
        return 0;
    }
    id[curr] = ref, idx[curr]++;
    return 1;

#undef mask1
#undef mask2
#undef __1_4_7__
#undef __2_5_8__
}

/// @brief 判断是否是全不靠或者七星不靠(lesser_honors_knitted_tiles)等全单类型。
/// @param arr 牌的分布数组。
/// @return 判断结果成立与否。
unsigned char all_alone(
    unsigned char arr[ESSENTIAL_CARDS_TYPES],
    unsigned char cnts[6]) {
    unsigned int *ptr = (unsigned int *)(cnts + 2);
    // 必须要是 00 00 00 0e
    if (*ptr != 0x0e'00'00'00) { return 0; }
    unsigned char id[3] = {0xFF, 0xFF, 0xFF}, idx[3] = {0}, hnt = 0;
    for (int i = _1_WAN_; i <= BAI_BAN; i++) {
        if (!arr[i]) { continue; }
        switch (i) {
        case DONG_FENG ... BAI_BAN: {
            hnt++;
            break;
        }
        default: {  // 三色状态抽象
            if (!is_diffused(i, i / 9, id, idx)) { return 0; }
            break;
        }
        }
    }
    if (hnt < 5) { return 0; }
    return idx[0] + idx[1] + idx[2] + hnt == CARDS_OF_BASIC_HU;
}

/// @brief 是否是组合龙
/// @param arr 牌的数量桶
/// @param cnts 不区分花色，牌的统计特性。
/// @return
// payload 构造：
// 1 x x 1 x x 1 -> 1 x 1 2 1 x 1 -> 1 2 1 2 1 x 1
// 每次先抽出 147 258 369，这个时候剩下 0 2 1 1 1 x 0
// 如果没有先枚举对而是先枚举顺子，就消不掉对和顺子。
unsigned char knitted_straight(
    unsigned char arr[ESSENTIAL_CARDS_TYPES],
    unsigned char cnts[6]) {
/// 检查是否是同列均有。
#define column_aux(ptr, ref, i)                                                \
    (ptr[ref + 0 + i] && ptr[ref + 3 + i] && ptr[ref + 6 + i])

#define column_act(ptr, ref, i, func)                                          \
    do {                                                                       \
        func(ptr + ref + i + 0);                                               \
        func(ptr + ref + i + 3);                                               \
        func(ptr + ref + i + 6);                                               \
    } while (0)

    if (cnts[5] > MAXN_CARDS_OF_HU || cnts[5] < CARDS_OF_BASIC_HU) { return 0; }
    unsigned char tmp[ESSENTIAL_CARDS_TYPES];
    memcpy(tmp, arr, ESSENTIAL_CARDS_TYPES);
    /* 0: pair, 1: pung/kong, 2: chow */
    int cnt[3] = {0}, id[3] = {-1, -1, -1}, a[3], quadruple = 0;
    unsigned char flag = 0, nok = 0;

    for (int i = 0; i < A33; i++) {
        for (int j = 0; j < 3; j++) {
            a[j] = _1_WAN_ + SUIT_LEAP * permute_seq[i][j];
            if (!column_aux(tmp, a[j], j)) {
                nok = 1;
                break;
            }
        }
        if (nok) {
            nok = 0;
            continue;
        }
        flag = 1;
        break;
    }
    if (!flag) { return 0; }
    for (int i = 0; i < 3; i++) {
        column_act(tmp, a[i], i, self_sub);
        for (int j = 0; j < 7; j++) {
            if (!is_a_seq(tmp, a[i], j)) { continue; }
            if (cnt[2]) { return 0; }
            unsigned char choice = j;
            // clang-format off
            choice += j <= 5 && tmp[a[i]+j] == 2 && is_a_seq(tmp, a[i], j+1);
            // clang-format on
            cnt[2]++, id[2] = choice;
            seq_functor(tmp + a[i] + choice, self_sub);
        }
    }
    for (int i = _1_WAN_; i <= BAI_BAN; i++) {
        if (tmp[i] < 2 || tmp[i] > 4) { continue; }
        unsigned char choice = tmp[i] != 2;
        if (cnt[choice]) { return 0; }
        cnt[choice]++, id[choice] = i, tmp[i] = 0;
        quadruple = choice ? (tmp[i] & 4) : quadruple;
    }
    return (cnt[1] + cnt[2] == 1) && cnt[0] == 1;

#undef column_aux
#undef column_sub
#undef column_add
}

/// @brief 十三幺判断
/// @param arr 牌的数量桶。
/// @return 是否为 13幺。
unsigned char _13_orphans_checker(
    unsigned char arr[ESSENTIAL_CARDS_TYPES],
    unsigned char cnts[6]) {
#define check_label0(val) ((val) < 1)
#define check_label1(val) ((val) == 2)
// ptr: 个数桶; ref: 只要求值为1，能区分出花色; func：宏或者函数。
#define homomorphiser(ptr, ref, func) (func(ptr[ref]) || func(ptr[ref + 8]))
#define honor_checker(array, ptr, func)                                        \
    do {                                                                       \
        for (size_t i = DONG_FENG; i <= BAI_BAN; i++) {                        \
            *ptr |= func(array[i]);                                            \
        }                                                                      \
    } while (0)
// 检查所有花色
#define check_all_suits(arr, func)                                             \
    (homomorphiser(arr, _1_WAN_, func) || homomorphiser(arr, _1_TIAO, func) || \
     homomorphiser(arr, _1_TONG, func))
    if (cnts[3] || cnts[4] || cnts[5] != CARDS_OF_BASIC_HU) { return 0; }
    unsigned char state = check_all_suits(arr, check_label0);
    honor_checker(arr, &state, check_label0);
    if (state) { return 0; }

    state = check_all_suits(arr, check_label1);
    honor_checker(arr, &state, check_label1);
    return state;

#undef honor_checker
#undef homomorphiser
#undef check_label1
#undef check_label0
#undef check_all_suits
}

/// @brief 判断七对或者四个杠。
/// @param arr 牌的数量桶。
/// @param cnts 牌值的分布情况。
/// @return 返回1表示七个对子。返回零表明非七对型。
unsigned char _7_pairs_or_4_quadruplet(
    unsigned char arr[ESSENTIAL_CARDS_TYPES],
    unsigned char cnts[6]) {
    if (cnts[1] || cnts[3]) { return 0; }
    // 七对极端情况下可能会有 1111 3333 5555 88 所以还得借助后面的力量。
    if (cnts[2] > 1) { return cnts[2] + (cnts[4] << 1) == 7; }
    // 四杠一定有一个对。
    return cnts[2] && (cnts[4] >= 3 || cnts[4] < 5);
}

inline unsigned char check_zero_in_arr(
    unsigned char arr[ESSENTIAL_CARDS_TYPES]) {
    // 34 = 32 + 2
    unsigned short *fin             = (unsigned short *)(arr);
    unsigned long long *examinee[4] = {
        (unsigned long long *)(arr + 2),
        (unsigned long long *)(arr + 10),
        (unsigned long long *)(arr + 18),
        (unsigned long long *)(arr + 26)};
    return !(
        static_cast<unsigned long long>(*fin) | *examinee[0] | *examinee[1] |
        *examinee[2] | *examinee[3]);
}

/// @brief 进入前已经保证有 14~17 张牌。要求判断是不是普通的胡牌牌型。
///        总体为回溯实现。在牌数正确的情况下，深度受剪枝限制，最深为 5
///        层。但应该还能优化调用过程?
/// @todo: 改进复杂度。
/// @param arr 牌数桶
/// @param pair 进入判断时，对子的数目
/// @param seq_trip 顺子/碰数。
/// @return 0 表示非胡牌，1 表示牌型是胡牌牌型。
unsigned char static normal_checker(
    unsigned char arr[ESSENTIAL_CARDS_TYPES],
    unsigned char pair,
    unsigned char seq_trip) {
/// 检查某种花色是否判断完成。
#define is_val_not_full_zero(array, ref)                                       \
    (*((unsigned long long *)(array + ref)) || array[ref + 8])

/// 函数内执行回溯的注入宏。后续不加下划线的参数表示不会对其对应的位置加额外处理。
#define backtrace(arr, idx, val, par, trip)                                    \
    do {                                                                       \
        arr[idx] -= val;                                                       \
        if (normal_checker(arr, par, trip)) {                                  \
            arr[idx] += val;                                                   \
            return 1;                                                          \
        }                                                                      \
        arr[idx] += val;                                                       \
    } while (0)

/// 处理非四杠和七对的杠，碰、对。
#define remedy_for_remainder(arr, idx, _pair_, _seq_trip_)                     \
    do {                                                                       \
        switch (arr[idx]) {                                                    \
        case 0: break;                                                         \
        case 2: {                                                              \
            backtrace(arr, idx, 2, _pair_ + 1, _seq_trip_);                    \
            break;                                                             \
        }                                                                      \
        case 3: {                                                              \
            backtrace(arr, idx, 3, _pair_, _seq_trip_ + 1);                    \
            break;                                                             \
        }                                                                      \
        case 4: {                                                              \
            backtrace(arr, idx, 4, _pair_, _seq_trip_ + 1);                    \
            backtrace(arr, idx, 2, _pair_ + 1, _seq_trip_);                    \
            break;                                                             \
        }                                                                      \
        default: {                                                             \
            return 0;                                                          \
        }                                                                      \
        }                                                                      \
    } while (0)

/// 处理顺子和其余情况。
/// 抽离顺子后，处理碰和对。
#define handle_inside_a_loop(arr, ref, _pair_, _seq_trip_)                     \
    do {                                                                       \
        for (size_t _idx_ = 0; _idx_ < 7; _idx_++) {                           \
            if (!arr[ref + _idx_]) { continue; }                               \
            if (is_a_seq(arr, ref, _idx_)) {                                   \
                seq_functor(arr + ref + _idx_, self_sub);                      \
                if (normal_checker(arr, _pair_, _seq_trip_ + 1)) {             \
                    seq_functor(arr + ref + _idx_, self_add);                  \
                    return 1;                                                  \
                }                                                              \
                seq_functor(arr + ref + _idx_, self_add);                      \
            }                                                                  \
            remedy_for_remainder(arr, ref + _idx_, _pair_, _seq_trip_);        \
        }                                                                      \
    } while (0)

    if (pair > 1 || seq_trip > 4) { return 0; }
    if (pair == 1 && seq_trip == 4) { return check_zero_in_arr(arr); }

    for (int i = _1_WAN_; i < DONG_FENG; i += SUIT_LEAP) {
        if (is_val_not_full_zero(arr, i)) { /* 检查万条筒 */
            handle_inside_a_loop(arr, i + 0, pair, seq_trip);
            /* 注意到 8 万和 9 万的碰没有被处理到。其它花色同理。*/
            remedy_for_remainder(arr, i + 7, pair, seq_trip);
            remedy_for_remainder(arr, i + 8, pair, seq_trip);
        }
    }
    for (int i = DONG_FENG; i <= BAI_BAN; i++) { /* 检查字 */
        if (!arr[i]) { continue; }
        if (arr[i] == 1) { return 0; }
        unsigned char curr = (arr[i] == 2), val = arr[i];
        // 宏展开并不能存储先前进入的值。
        backtrace(arr, i, val, pair + curr, seq_trip + !curr);
    }
    return check_zero_in_arr(arr);
#undef is_val_not_full_zero
#undef backtrace
#undef remedy_for_remainder
#undef handle_inside_a_loop
}

/// @brief 传一个待判胡牌数组。如果不符合胡牌规范，返回 -1
/// @param arr 胡牌数组
/// @return 胡牌类型
int hu_checker(unsigned char arr[ESSENTIAL_CARDS_TYPES]) {
    /// TODO: 有表格则加入查表环节。
    /// 将可能的胡牌情况通过哈希单射打成表，这样可以通过复查表来完成块速完成判断。
    // int end_type           = INVALID_HU;

    unsigned char _cnts[6] = {0};  // (0,1,2,3,4) || total.
    for (int i = _1_WAN_; i <= BAI_BAN; i++) {
        if (arr[i] >= 5) { return INVALID_HU; }
        _cnts[5] += arr[i], _cnts[arr[i]]++;
    }
    if (_cnts[5] < 14 || _cnts[5] > 18) { return INVALID_HU; }
    if (_7_pairs_or_4_quadruplet(arr, _cnts)) {
        if (_cnts[4] <= 3) { return _7_PAIRS; }
        return _4_QUADRUPLETS;
    }
    if (all_alone(arr, _cnts)) { return ALL_ALONE; }
    if (_13_orphans_checker(arr, _cnts)) { return _13_ORPHANS; }

    if (normal_checker(arr, 0, 0)) { return NORMAL_HU; }
    return !knitted_straight(arr, _cnts) ? INVALID_HU : KNITTED_STRAIGHT;
    // 将牌型按照函数加入到表内，以加快下一次查询。
}

/// @brief 输出单张牌是什么
/// @param val
inline static void print_cards(unsigned char val) {
    int tmp = val / 9, remain = val - tmp * 9;
    mahjong_content_printer((mahjong_attr)(tmp), remain);
}

unitform_rand_gen uniform_gen /* 针对洗牌的随机数生成器 */;
unitform_rand_gen dice(0, 3) /* 决定庄家的随机数生成器 */;

/// @brief 将一次洗牌视为从元素个数为 136 的置换群中选出一个排列
/// @TODO: 做其它类型的适配。比如通配符/含有花牌/含有jocker。
///        可能需要函数指针来完成这个事情。同时结构内部的类型也需要变换。
void permutate_cards() {
    using std::sort;
    unsigned char cards[ESSENTIAL_CARDS_TYPES], q[BASIC_TOTAL_CARDS];
    unsigned char cnt = 0, curr, round = 0;
    for (int i = _1_WAN_; i <= BAI_BAN; i++) { cards[i] = 4; }
    while (!(cnt == BASIC_TOTAL_CARDS)) {
        curr = uniform_gen.fetch_one() % ESSENTIAL_CARDS_TYPES;
        if (!cards[curr]) { continue; }
        q[cnt++] = curr, cards[curr]--;
    }
    for (curr = 0, cnt = 1; round < 3; cnt++) {
        player[curr][q[cnt - 1]]++;
        if (cnt % 4 == 0) { curr = (curr + 1) & 0b11; }
        if (cnt % 16 == 0) { round++; }
    }
    for (int i = 0 * (cnt--); i < 4; i++) {
        player[i][q[cnt++]]++;
        mamba[i].cards = player[i];
    }
    // 剩下给牌堆。
    while (heap_cnt < CARDS_OF_HEAP) { heap[heap_cnt++] = mahjong(q[cnt++]); }

#ifdef DEBUG
    for (int i = 0; i < BASIC_TOTAL_CARDS; i++) {
        print_cards(q[i]), wprintf(KJPH_ABBR, L" ");
        if (i && (i % ESSENTIAL_CARDS_TYPES == ESSENTIAL_CARDS_TYPES - 1))
            wprintf(KJPH_ABBR, L"\n");
    }

    for (int i = 0 * wprintf(KJPH_ABBR, L"\n"); i < 4; i++) {
        wprintf(L"玩家%d的牌: ", i);
        for (int j = _1_WAN_; j <= BAI_BAN; j++) {
            if (!player[i][j]) continue;
            for (int k = 0, cnt = player[i][j]; k < cnt; k++) {
                mahjong(j).self_print();
                wprintf(KJPH_ABBR, L" ");
            }
        }
        wprintf(KJPH_ABBR, L"\n\n");
    }
    for (int i = 0; i < CARDS_OF_HEAP; i++) {
        heap[i].self_print();
        wprintf(KJPH_ABBR, L" ");
    }
    wprintf(KJPH_ABBR, L"\n");

#endif  // DEBUG
}

/// @brief 从牌堆中取牌。如果没有，设置游戏结束。
/// @param participant (玩家)
static inline void fetch_card_from_heap(info_of_players &participant) {
    if (~heap_cnt) {
        game_over = 1;
        return;
    }
    participant.curr_card = heap[heap_cnt--];
    // 首先检查是否能使自身胡牌。
    // 其次按照已有碰上是否形成杠、是否构成暗杠、是否构成暗刻来决策。
    // 否则形成单牌、对子或者顺子。
}

/**
 * 牌堆来的牌视为概率均等。
 * 桌面上其余玩家打的牌视为利于自身向胡牌靠近的一类/手误打出去的牌。
 * 对评估来说，最方便的写法应该是哪个接近最终的胡牌模式就往哪个靠。
 * 也需要担心目的胡牌被中途截断的情形。
 */
static inline void evaluator(
    unsigned char arr[ESSENTIAL_CARDS_TYPES],
    unsigned char note[ESSENTIAL_CARDS_TYPES]) {
    // 13 -[34]-> 14 -[13]-> 13.
    // 一步以内在听牌，听牌了也不能保证能胡。
    // 其它情况需要多步到达听牌态。
    // 转移应当考虑玩家已掌握的所有牌面信息。

    // c0 --[i0]-> c1 --[i1]-> ... --[it]-> ct --[1,13]-> ch
    // ..476-i0    ..476-i1                 ..[475,464]
    // 第二行意为保持原牌面不变，将从牌堆中取到的牌直接打出。

    // f{i}: c{i} -> c{i+1}, 映射应该与当前的所有牌的分布有关。

    // 考虑所有变化的情况。不考虑保持不变的操作。
    // 同时也应该注意到最快到胡的好处？
    // 总体只需要评估 1-9 如何组建，还有只能靠碰杠以及对来消耗掉的字牌。
    // 策略上应该尽量保留一种花色，同时能占有字的占有字。
    unsigned char val[4] = {0};
    for (int k = 0; k < 3; k++) {
        for (int i = _1_WAN_; i <= _9_WAN_; i++) { val[k] += arr[i + k * 9]; }
    }
    for (int i = DONG_FENG; i <= BAI_BAN; i++) { val[3] += arr[i]; }
    unsigned char choice = 0, backup = 0xFF;
    for (int i = 0; i < 4; i++) {
        if (val[i] < backup) { backup = val[i], choice = i; }
    }
    // 决定出应该要刨除哪一个花色。
    // 但是存在一种比较均衡的情况，而后续来牌都是选中抛除的花色。
    // 比方说：4 + [3] + 3 + 3，然后一直来条
    // 决定要丢什么颜色理应由到胡方式决定。

    return;
}
