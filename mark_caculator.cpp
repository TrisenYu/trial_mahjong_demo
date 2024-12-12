#include "mahjong.h"

/* 实现最基础的胡牌分。 */
// 88 番
/*
    有点繁琐过头了。
    大四喜：东东东(东) 南南南(南) 西西西(西) 北北北(北) pp
    大三元：中中中(中) 发发发(发) 白白白(白) abc pp
    绿一色：23468 发 中的任何牌构成的胡牌
            222 333 444 666 发发
    九莲宝灯：111 2345678 999  外加一张任何同花色的都行
            111 123 456 789 99
            111 22 345 678 999
            11 123 345 678 999
            111 234 456 789 99
            111 234 55 678 999
            11 123 456 678 999
            111 234 567 789 99
            111 234 567 88 999
            11 123 456 789 999
    连七对
    十三幺
    四杠
*/
inline int basic_check(
    unsigned char arr[ESSENTIAL_CARDS_TYPES],
    unsigned char cnts[6]) {
    /// 保证牌的数量处在 [14, 18] 之内。
    /* arr: 个数桶; ref: 只要求值为1，能区分出花色; func：宏或者函数。 */
    for (int i = _1_WAN_; i <= BAI_BAN; i++) {
        if (arr[i] >= 5) { return -1; }
        cnts[5] += arr[i], cnts[arr[i]]++;
    }
    if (cnts[5] > 18 || cnts[5] < 14) { return 0; }
    return 1;
}

inline int _88_(
    unsigned char arr[ESSENTIAL_CARDS_TYPES],
    unsigned char cnt[6]) {
    /// 保证牌的数量处在 [14, 18] 之内。
    /* arr: 个数桶; ref: 只要求值为1，能区分出花色; func：宏或者函数。 */
    if (!basic_check(arr, cnt)) { return 0; }
    if (cnt[5] == 14) {
        // 十三幺检测。
        if (_13_orphans_checker(arr, cnt)) { return 88; }
        // 检查连七对。
        unsigned char flag = _7_pairs_or_4_quadruplet(arr, cnt);
        if (flag) {
            // 11 22 33 44 55 66 77
        }
        // 检查九莲宝灯。
        return 0;
    }

    // 检查连七对
    if (!cnt[1] || !cnt[3]) {}

    return 0;
}
// 64 番
/*
    清幺九：只含幺九的杠/刻子/对
    小四喜：东东东南南南西西西北北 abc|aaa 包含三个风刻/杠，一个风对。杂色顺刻。
    小三元：包含两个元刻，一个元对。
    字一色：全是字构成的胡牌。
    四暗刻：四个暗刻+1个对。
    一色双龙会：123 123 55 789 789 同色
*/

// 48 番
/*
    同色4同顺 123 123 123 123 pp
    同色4节高 111 222 333 444 55
*/
static int _48_(unsigned arr[ESSENTIAL_CARDS_TYPES], unsigned char cnt[6]) {
    return 0;
}

// 32 番
/*
    三杠
    混幺九 111 999 南南南 北北 111
*/
static int _32_(
    unsigned char arr[ESSENTIAL_CARDS_TYPES],
    unsigned char cnt[6]) {
    if (!basic_check(arr, cnt)) { return 0; }
    if (cnt[4] == 3) { return 0 /* TODO */; }
    return 0;
}

// 24 番
/*
    七对 除去连七对的特例。
    七星不靠
    全双刻 2468 构成的胡牌。不区分花色。
    清一色 同一花色形成的胡牌。刨掉九莲宝灯。
    同色3同顺 345 345 345 aaa|abc bb
    同色3节高 333 444 555 aaa|abc bb
    全大 789 789 77 888 999 混色，全是大的构成的胡牌
    全小 123 构成的胡牌
    全中 456 构成的胡牌
*/

// 16 番
/*
    清龙：123 456 789 aaa|abc aa 前三个顺同色。
    三色双龙会： (123 789) [123 789] 55 三个混色。
    杂色三节高：花色递增{123 234 456} [777] (88)
    全带5 有每个花色且每个花色都有 5，且雀头靠5形成胡牌。
    三同刻：三个刻除了颜色不同数是一样的。
    三暗刻：门前清可能。
*/

/// TODO: 感觉还需要附上说明.
// 12 番
/*
    全不靠：字牌没满，
    大于5 6789 组成的和牌
    小于5 1234 组成的和牌
    三风刻 要有三个风刻（杠）
*/
static inline int _12_(
    unsigned char arr[ESSENTIAL_CARDS_TYPES],
    unsigned char cnt[6]) {
#define greater_than_3(val) ((val) >= 3)
#define test_wind(ptr)                                                         \
    do {                                                                       \
        int __cnt = 0;                                                         \
        for (int __i = DONG_FENG; __i < HONG_ZHONG; __i++) {                   \
            __cnt += greater_than_3(arr[__i]);                                 \
        }                                                                      \
        *ptr = __cnt == 3;                                                     \
    } while (0)

    if (all_alone(arr, cnt)) {
        int counter = 0;
        for (int i = DONG_FENG; i <= BAI_BAN; i++) { counter += arr[i]; }
        if (counter == 7) { return 24; }
        return 12;
    }

    unsigned char flag = 0, glaf = 0;
    test_wind(&flag);
    if (arr[_5_WAN_] || arr[_5_TIAO] || arr[_5_TONG]) {
        if (!flag) { return 0; }
        goto end;
    }
    flag = 0;
    for (int k = _1_WAN_; k < DONG_FENG; k += SUIT_LEAP) {
        for (int i = 0, j = 6; i < 5; i++, j++) {
            if ((arr[i + k] && glaf) || (arr[j + k] && flag)) { return 0; }
            if (arr[i + k]) { flag = 1; }
            if (arr[j + k]) { glaf = 1; }
        }
    }
end:
    return /* todo */ 0;
}

// 8 番
/*
    花龙 123 456 789 在不同的花色。
    推不倒：把胡的所有牌逐个倒过来看还是一样的牌。分布：筒：1234589 条：245689
   外加白板。
    杠上开花：自己杠后摸到牌胡
    海底捞月：最后一轮，别人打出一张牌然后胡
    妙手回春：摸到牌堆最后一张牌胡
    抢杠和：别人要杠但自己胡

 */
// 6 番
/*
    碰碰和
    混一色
    三色三步高
    五门齐
    全求人
    双暗杠 / 一明一暗
    双键刻
*/

// 4 番
/*
    全带幺： 每个花色都带 1或9
    门前清（不求人）
    双明杠：
    和绝张：当某个花色在整个牌局中只剩下最后一张且从[牌堆/他人]手上[取出/打出]造成胡牌时，就是和绝张。
*/

// 2 番
/*
    箭/门风/
    平和 四幅顺子造成和牌
    四归一
    断幺
    单暗杠
    双同刻
    双暗刻
*/

// 1 番
/*
    喜相逢：两幅不同花色值一样的顺子。
    连六： 234567 花色内构成的连续两个顺子。
    老少副：123 789 花色内同时存在.
    缺一门：少一类牌。
    无字：胡牌中没有风箭
    边张：123或者789 某个具体的花色内只有两者中的一个。
    坎张：靠听顺子中间的牌来胡。
    单钓将：只用 1 张牌来听胡而成的胡牌。
    自摸
    花牌。
    一般高：花色相同的两个顺子。
*/