/**
 * SPDX-LICENSE-IDENTIFIER: GPL2.0-ONLY
 * (C) 2024 Author: <kisfg@hotmail.com>
 * 
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License 
 * as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 * 
 * See the GNU General Public License for more details. 
 * You should have received a copy of the GNU General Public License along with this program. 
 * If not, see <https://www.gnu.org/licenses/>.
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
    unsigned char arr[ESSENTIAL_CARDS_TYPES],
    unsigned char src, 
    unsigned char dst,
    unsigned char val
) { for (;val; arr[src]--, val--) { arr[dst]++; } }


/// @brief 以实参判断是否按 147 258 369 分散花色而成牌。
/// @param val 牌的数值。
/// @param ref 牌的参考花色。如 `_1_WAN_`, `_1_TIAO` 或最后的 `_1_TONG`。
/// @param id 花色数组。形如 id[3]。
/// @param idx 在不靠的条件下，截至调用时对应花色的数量数组。形如 idx[3]。
/// @return 判断结果。0 表示成立，1 表示不成立。
inline static unsigned char is_diffused(
    int val, int ref, 
    unsigned char id[3], 
    unsigned char idx[3]
) {
#define __1_4_7__(val) ((val) == 1 || (val) == 4 || (val) == 7)
#define __2_5_8__(val) ((val) == 2 || (val) == 5 || (val) == 8)
#define mask1 0x01
#define mask2 0x02
    unsigned char curr = 0x0;
    if (__1_4_7__(val - ref + 1)) {} 
    else if (__2_5_8__(val - ref + 1)) { curr = mask1; } 
    else { curr = mask2; }
    unsigned char peer1 = curr ^ mask1, peer2 = curr ^ mask2;

    if (peer1 == 0x3) { peer1 ^= curr; }
    if (peer2 == 0x3) { peer2 ^= curr; }
    /* 当前的值要与先前的同花色的一致，否则不符合胡牌条件。 */  
    if (((~id[curr] && id[curr] != ref)) || id[peer1] == ref || id[peer2] == ref) {
        return 1;
    }
    id[curr] = ref, idx[curr]++;
    return 0;
#undef mask1
#undef mask2
#undef __1_4_7__
#undef __2_5_8__

}

/// @brief 判断是否是全不靠或者七星不靠(lesser_honors_knitted_tiles)等全单类型。
/// @param arr 牌的分布数组。
/// @return 判断结果成立与否。
inline unsigned char all_alone(
    unsigned char arr[ESSENTIAL_CARDS_TYPES],
    unsigned char cnts[6]
) {    
    if (cnts[2] || cnts[3] || cnts[4] || cnts[5] != CARDS_OF_ONE_PLAYER) { 
        return 0; 
    }
    unsigned char id[3] = {0xFF, 0xFF, 0xFF}, idx[3] = {0}, hnt = 0;
    for (int i = _1_WAN_; i < BAI_BAN; i++) {
        if (!arr[i]) { continue; }
        switch (i) {
        case DONG_FENG...BAI_BAN: { hnt++; break; }
        default: { // 三色状态压缩
            if (is_diffused(i, i/9, id, idx)) { return 0; }
            break;
        }
        }
    }
    if (hnt < 5) { return 0; }
    return idx[0] + idx[1] + idx[2] + hnt == CARDS_OF_ONE_PLAYER;
}


const unsigned char permute_seq[6][3] = {
    {0, 1, 2}, {0, 2, 1}, {1, 0, 2}, 
    {1, 2, 0}, {2, 0, 1}, {2, 1, 0},
}, a33=6;
/// @brief 是否是组合龙
/// @param arr 牌的数量桶
/// @param cnts 不区分花色，牌的统计特性。
/// @return 
inline unsigned char knitted_straight(
    unsigned char arr[ESSENTIAL_CARDS_TYPES],
    unsigned char cnts[6]
) {
// 检查是否是同列均有。
#define column_aux(arr, ref, i) \
    (arr[ref+0+i] && arr[ref+3+i] && arr[ref+6+i])
// 同列-1。
#define column_sub(arr, ref, i) \
    do { arr[ref+0+i]--; arr[ref+3+i]--; arr[ref+6+i]--; } while(0)
// 同列+1
#define column_add(arr, ref, i) \
    do { arr[ref+0+i]++; arr[ref+3+i]++; arr[ref+6+i]++; } while(0)

    if (cnts[5] > MAXN_CARDS_OF_ONE || cnts[5] < CARDS_OF_ONE_PLAYER) { 
        return 0; 
    }
    /* 0: pair, 1: pung/kong, 2: chow */
    int cnt[3] = {0}, id[3] = {-1, -1, -1}, a[3];
    int seq_suit, quadruple =0;
    unsigned char res = 0, flag = 0, ok = 0;

    for (int i = 0; i < a33; i++) {
        for (int j = 0; j < 3; j++) {
            a[j] = _1_WAN_+SUIT_LEAP*permute_seq[i][j];
            if (!column_aux(arr, a[j], j)) {
                ok = 1;
                break;
            }
        }
        if (ok) {
            ok = 0;
            continue;
        }
        flag = 1;
        break;
    }
    if (!flag) {
        return 0;
    }
    for (int i = 0; i < 3; i++) {
        column_sub(arr, a[i], i);   
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 7; j ++) {
            if (!is_a_seq(arr, a[i], j)) { continue; }
            if (cnt[2]) { goto recover_seq; }
            cnt[2]++; id[2] = j; seq_suit = a[i];
            arr[a[i]+j]--, arr[a[i]+j+1]--, arr[a[i]+j+2]--;
        }
    }

    for (int i = _1_WAN_; i < BAI_BAN; i++) {
        if (!arr[i]) { continue; }
        switch (arr[i]) {
        case 2: {
            if (cnt[0]) { goto recover_pair; }
            cnt[0]++; id[0] = i;
            arr[i] = 0; break;
        }
        case 3: 
        case 4: {
            if (cnt[1]) { goto recover_multiple; }
            cnt[1]++; id[1] = i;
            quadruple = arr[i] & 4;
            arr[i] = 0; break;
        }
        default: { continue; }
        }
    }
    res = (cnt[1] + cnt[2] == 1) && cnt[0] == 1;
    // 恢复
recover_multiple:
    if (id[1]+1 > 0) {
        arr[id[1]] = quadruple == 4 ? 4 : 3;
    }
recover_pair:
    if (id[0]+1 > 0) {
        arr[id[0]] = 2;
    }
recover_seq:
    if (id[2]+1 > 0) {
        for (int i = 0; i < 3; i ++) {
            arr[seq_suit+id[2]+i]++;
        }
    }
recover_columns: // 一定有
    for (int i = 0; i < 3; i++) {
        column_add(arr, a[i], i);
    }
    return res;
#undef column_aux
#undef column_sub
#undef column_add
}

/// @brief 十三幺判断
/// @param arr 牌的数量桶。
/// @return 是否为 13幺。
inline unsigned char _13_orphans_checker(
    unsigned char arr[ESSENTIAL_CARDS_TYPES],
    unsigned char cnts[6]
) {
#define check_label0(val) ((val) < 1)
#define check_label1(val) ((val) == 2)
/* arr: 个数桶; ref: 只要求值为1，能区分出花色; func：宏或者函数。 */
#define homomorphiser(arr, ref, func) (func(arr[ref]) || func(arr[ref+8]))
#define honor_checker(arr, ptr, func) \
do { \
    for (size_t i = DONG_FENG; i < BAI_BAN; i++) { \
        *ptr |= func(arr[i]); \
    } \
} while(0)

#define check_all_suits(arr, func) (\
    homomorphiser(arr, _1_WAN_, func) || \
    homomorphiser(arr, _1_TIAO, func) || \
    homomorphiser(arr, _1_TONG, func) \
)

    if (cnts[3] || cnts[4] || cnts[5] != CARDS_OF_ONE_PLAYER) {
        return 0;
    }
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
inline unsigned char _7_pairs_or_4_quadruplet(
    unsigned char arr[ESSENTIAL_CARDS_TYPES], 
    unsigned char cnts[6]
) {
    if (cnts[1] || cnts[3]) { 
        return 0; 
    }
    // 七对
    if (cnts[2] > 1) {
        return cnts[2] + (cnts[4]<<1) == 7;
    }
    // 四杠
    return cnts[2] == 1 && cnts[4] == 4;
}


inline unsigned char check_zero_in_arr(unsigned char arr[ESSENTIAL_CARDS_TYPES]) {
    for (int i = _1_WAN_; i < BAI_BAN; i++) { if (arr[i]) { return 0; } }
    return 1;
}


/// @brief 进入前已经保证有 14~17 张牌。要求判断是不是普通的胡牌牌型。
///        总体为递归实现。在牌数正确的情况下，深度受剪枝限制，最深为 5 层。但
/// @todo: 改进复杂度。
/// @param arr 牌数桶
/// @param pair 进入判断时，对子的数目
/// @param sequence 顺子数。
/// @param triplet 碰数。
/// @return 0 表示非胡牌，1 表示牌型是胡牌牌型。
unsigned char static normal_checker(
    unsigned char arr[ESSENTIAL_CARDS_TYPES],
    unsigned char pair, 
    unsigned char sequence, 
    unsigned char triplet
) {

// 检查某种花色是否判断完成。
#define is_val_not_full_zero(arr, ref) (        \
     arr[ref+0] || arr[ref+1] || arr[ref+2] ||  \
     arr[ref+3] || arr[ref+4] || arr[ref+5] ||  \
     arr[ref+6] || arr[ref+7] || arr[ref+8]     \
)

#define self_sub(ptr) ((*(ptr))--)
#define self_add(ptr) ((*(ptr))++)

// 同时操控数组中连续的三个。
#define triplet_functor(ptr, func)      \
do {                                    \
    for (size_t __ = 0; __ < 3; __++) { \
        func(ptr + __);                 \
    }                                   \
} while(0)

/// 函数内执行回溯的注入宏。后续不加下划线的参数表示不会对其对应的位置加额外处理。
#define reoccur(arr, idx, val, par, seq, trip)  \
do {                                            \
    arr[idx] -= val;                            \
    if (normal_checker(arr, par, seq, trip)) {  \
        arr[idx] += val; return 1;              \
    }                                           \
    arr[idx] += val;                            \
} while(0)

/// 处理非四杠和七对的杠，碰、对。
#define remedy_for_remainder(arr, idx, _pair_, _seq_, _trip_)   \
do {                                                            \
    switch (arr[idx]) {                                         \
    case 0: break;                                              \
    case 2: {                                                   \
        reoccur(arr, idx, 2, _pair_+1, _seq_, _trip_);          \
        break;                                                  \
    }                                                           \
    case 3: {                                                   \
        reoccur(arr, idx, 3, _pair_, _seq_, _trip_+1);          \
        break;                                                  \
    }                                                           \
    case 4: {                                                   \
        reoccur(arr, idx, 2, _pair_+1, _seq_, _trip_);          \
        reoccur(arr, idx, 4, _pair_, _seq_, _trip_+1);          \
        break;                                                  \
    }                                                           \
    default: return 0;                                          \
    } \
} while(0)

/// 处理顺子和其余情况。
#define handle_inside_a_loop(arr, ref, _pair_, _seq_, _trip_)   \
do {                                                            \
    for (size_t _idx_ = 0; _idx_ < 7; _idx_++) {                \
        if (!arr[ref+_idx_]) { continue; }                      \
        if (is_a_seq(arr, ref, _idx_)) {                        \
            triplet_functor(arr+ref+_idx_, self_sub);           \
            if (normal_checker(arr, _pair_, _seq_+1, _trip_)) { \
                triplet_functor(arr+ref+_idx_, self_add);       \
                return 1;                                       \
            }                                                   \
            triplet_functor(arr+ref+_idx_, self_add);           \
        }                                                       \
        /* 处理碰和对。*/                                         \
        remedy_for_remainder(arr, ref+_idx_, _pair_, _seq_, _trip_); \
    } \
} while(0)

    if (pair > 1 || (sequence + triplet > 4)) { return 0; }
    if (pair == 1 && (sequence + triplet == 4)) {
        return check_zero_in_arr(arr);
    }
    
    for (int i = _1_WAN_; i < DONG_FENG; i += SUIT_LEAP) {
        if (is_val_not_full_zero(arr, i)) { /* 检查万条筒 */
            handle_inside_a_loop(arr, i+0, pair, sequence, triplet);
            /* 注意到 8 万和 9 万的碰没有被处理到。其它花色同理。*/
            remedy_for_remainder(arr, i+7, pair, sequence, triplet);
            remedy_for_remainder(arr, i+8, pair, sequence, triplet);
        }
    }
    for (int i = DONG_FENG; i < BAI_BAN; i++) { /* 检查字 */
        if (arr[i] == 1) { return 0; }
        else if (arr[i] == 2) {
            reoccur(arr, i, 2, pair+1, sequence, triplet);
        }
        else if (arr[i] > 2) {
            reoccur(arr, i, arr[i], pair, sequence, triplet+1);
        }
    }
    return 0;
#undef is_val_not_full_zero
#undef self_sub
#undef self_add
#undef triplet_functor
#undef reoccur
#undef remedy_for_remainder
#undef handle_inside_a_loop
}


/// @brief 传一个待判胡牌数组。如果不符合胡牌规范，返回 -1
/// @param arr 胡牌数组
/// @return 胡牌番数
int hu_checker(unsigned char arr[ESSENTIAL_CARDS_TYPES]) {
    /// TODO: 将可能的胡牌情况通过哈希单射打成表，这样可以通过复查表来完成块速完成判断。

    unsigned char _cnts[6] = {0}; // (0,1,2,3,4) || total.
    int has_triplet = 0;
    for (int i = _1_WAN_; i < BAI_BAN; i++) {
        if (arr[i] >= 5) { return -1; }
        _cnts[5] += arr[i], _cnts[arr[i]]++;
    }
    if (_7_pairs_or_4_quadruplet(arr, _cnts) ||
        all_alone(arr, _cnts) ||
        _13_orphans_checker(arr, _cnts)) {
        return 1;
    }
    if (_cnts[5] < 14 || _cnts[5] >= 18) {
        return -1;
    }
    if (normal_checker(arr, 0, 0, 0)) {
        return 1;
    }

    return !knitted_straight(arr, _cnts) ? ~0 : 1;
}


/// @brief 输出单张牌是什么
/// @param val 
inline static void print_cards(unsigned char val) {
    int tmp = val/9, remain = val - tmp*9;
    mahjong_content_printer((mahjong_attr)(tmp), remain);
}

unitform_rand_gen uniform_gen /* 针对洗牌的随机数生成器 */, dice(0, 3) /* 决定庄家的随机数生成器 */;

/// @brief 将一次洗牌视为从元素个数为 136 的置换群中选出一个排列
void permutate_cards() {
    using std::sort;
    unsigned char cards[ESSENTIAL_CARDS_TYPES], q[TOTAL_CARDS], cnt = 0, curr, round = 0;
    for (int i = _1_WAN_; i < BAI_BAN; i++) { cards[i] = 4; }
    while (!(cnt == TOTAL_CARDS)) {
    back:
        curr = uniform_gen.fetch_one() % ESSENTIAL_CARDS_TYPES;
        if (!cards[curr]) { goto back; }
        q[cnt++] = curr, cards[curr]--;
    }
    for (curr = 0, cnt = 1; round < 3; cnt++) { 
        player[curr][q[cnt-1]]++;
        if (cnt % 4 == 0) curr = (curr + 1) % 4;
        if (cnt % 16 == 0) round++;
    }
    for (int i = 0 * (cnt--); i < 4; i++) {
        player[i][q[cnt++]]++;
        mamba[i].cards = player[i];
    }
    // 剩下给牌堆。
    while (heap_cnt < CARDS_OF_HEAP) { 
        heap[heap_cnt++] = mahjong(q[cnt++]); 
    }

#ifdef DEBUG
    for (int i = 0; i < TOTAL_CARDS; i++) {
        print_cards(q[i]), wprintf(KJPH_ABBR, L" ");
        if (i && (i%ESSENTIAL_CARDS_TYPES == ESSENTIAL_CARDS_TYPES-1))
            wprintf(KJPH_ABBR, L"\n");
    }

    for (int i = 0*wprintf(KJPH_ABBR, L"\n"); i < 4; i++) {
        wprintf(L"玩家%d的牌: ", i);
        // 
        for (int j = _1_WAN_; j < BAI_BAN; j++) {
            if (!player[i][j]) continue;
            for (int k = 0, cnt = player[i][j]; k < cnt; k++) 
                mahjong(j).self_print(), wprintf(KJPH_ABBR, L" "); 
        }
        wprintf(KJPH_ABBR, L"\n\n");
    }
    for (int i = 0; i < CARDS_OF_HEAP; i++) {
        heap[i].self_print(), wprintf(KJPH_ABBR, L" ");
    }
    wprintf(KJPH_ABBR, L"\n");
#endif // DEBUG
}

/// @brief 从牌堆中取牌。如果没有，设置游戏结束。
/// @param participant (玩家)
static inline void fetch_card_from_heap(info_of_players &participant) {
    if (~heap_cnt) {
        game_over = 1;
        return;
    }
    participant.curr_card = heap[heap_cnt--];
}


// 判断听牌的依据是什么？
// 预测的方向如何实现？
/// TODO: 如何将玩家手上的牌数桶转化为特定规则下的单射哈夫曼表示？
///       尽可能减少计算的重复、加快是否胡牌的计算。

/* 番数计算器 */
static int mark_caculator(unsigned char arr[ESSENTIAL_CARDS_TYPES]) {
    // 特例有点多
    // 这个看起来也相当困难。
    // 此外还要将游戏过程中的状态汇总进来考虑。
    
    
    // 是否门前清？是否海底捞月/河底捞鱼？是否全求人？是否对对碰？是否自摸？是否
    // 牌型上的判别：大三元？小三元？十三幺？连七对？九莲宝灯？
    // 和绝张：依靠牌池上已亮明的某个三张的最后一张牌来胡牌
    // 门风刻。字风和位风一致。
    // 圈风刻。暂时不算？
    // 三色双龙会？
    return 0;
}

/* 游戏人机：转人工。*/
// 决策算法
static void hello_world() {
    // 牌堆不可见，其它玩家不可见。只有其它玩家打出来的牌和通过碰杠吃所形成的牌和玩家手上打出的牌和持有的牌可见。
    // 在初态评估牌局是相当困难的，但是应该能通过手上的牌和可能的胡牌状态确定要先打哪一张牌？
    unsigned char cnt[4] = {0};
    // 开局哪个少出哪个。

}

// 评分算法
static int evaluater() {
    return 0;
}

/// TODO: 可能需要换存储的数据结构。
///       专门用碰队列、吃队列、杠队列来维护手上的牌。
///       但是这个可能不方便决策的计算？
static void pseudo_ai_constructor(int id) {
    /// pass
    // 注意到出现对
}

// 该打牌了。

