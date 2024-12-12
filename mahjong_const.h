#pragma once
#ifndef _MAHJONG_CONST_H_
#define _MAHJONG_CONST_H_
#endif  // _MAHJONG_CONST_H_

#define DEBUG

#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include <algorithm>
#include <ctime>
#include <iostream>
#include <random>
#include <thread>
#include <vector>
using std::vector;

/// @brief 单张牌的类型。
enum mahjong_attr {
    WANGUAN, /* 万 0-8 0-8   */
    BAMBOO,  /* 条 0-8 9-17  */
    CIRCLE,  /* 筒 0-8 18-26 */
    HANZI,   /* 字 0-6 27-33 */
    FEATURE
};

/// @brief 胡牌种类
enum Hu_type {
    INVALID_HU = -1,   // 没有胡
    NORMAL_HU,         // 正常胡（但是可能会包含大三元大四喜）
    _7_PAIRS,          // 七对子（可能包含连七对）
    _4_QUADRUPLETS,    // 四杠
    ALL_ALONE,         // 全不靠
    _7_ALONE,          // 七星不靠
    KNITTED_STRAIGHT,  // 组合龙
    _13_ORPHANS,       // 13 幺
};
/// @brief 麻将，自豪地使用拼音命名法。
enum mahjong_val {
    /* 万 */
    _1_WAN_,
    _2_WAN_,
    _3_WAN_,
    _4_WAN_,
    _5_WAN_,
    _6_WAN_,
    _7_WAN_,
    _8_WAN_,
    _9_WAN_,
    /* 条 */
    _1_TIAO,
    _2_TIAO,
    _3_TIAO,
    _4_TIAO,
    _5_TIAO,
    _6_TIAO,
    _7_TIAO,
    _8_TIAO,
    _9_TIAO,
    /* 筒 */
    _1_TONG,
    _2_TONG,
    _3_TONG,
    _4_TONG,
    _5_TONG,
    _6_TONG,
    _7_TONG,
    _8_TONG,
    _9_TONG,
    /* 东西南北 */
    DONG_FENG,
    XI_FENG,
    NAN_FENG,
    BEI_FENG,
    /* 中发白 */
    HONG_ZHONG,
    FA_CAI,
    BAI_BAN,
    /* 梅兰竹菊 Plum blossoms, orchids, bamboo, chrysanthemum. */
    EXT_P,
    EXT_O,
    EXT_B,
    EXT_C,
    /* 春夏秋冬 spring, summer, autumn, winter. */
    EXT_SPRING,
    EXT_SUMMER,
    EXT_AUTUMN,
    EXT_WINTER,
    /* 美国麻将可能的拓展。但是还没有实现 */
    EXT_JOKER,
    /* 无效牌。 */
    INVALID_CARD,
};
constexpr unsigned char permute_seq[6][3] = {
    {0, 1, 2},
    {0, 2, 1},
    {1, 0, 2},
    {1, 2, 0},
    {2, 0, 1},
    {2, 1, 0},
};
constexpr unsigned char A33 = 6;
#define SUIT_LEAP             0x09 /* 花色的跨度为 9. */
#define CARDS_OF_BASIC_HU     0x0e /* 胡牌至少要有 14 张牌 */
#define CARDS_OF_HEAP         0x54 /* 136 - 13*4 */
#define ESSENTIAL_CARDS_TYPES 0x22 /* 不计算花牌，牌的类型有 9*3+4+3 种 */
#define BASIC_LIMIT           0x60
#define BASIC_TOTAL_CARDS     0x88                  /* 至少有 136 张牌。 */
#define MAXN_CARDS_OF_HU      CARDS_OF_BASIC_HU + 4 /* 四杠能让牌数变到 18. */
#define CARDS_OF_ALL_PLAYER   0x04 * CARDS_OF_BASIC_HU

enum mahjong_act {
    ACT_PASS,  // 过
    ACT_CHOW,  // 吃
    ACT_PUNG,  // 碰
    ACT_KONG,  // 杠
    ACT_HU,    // 胡
};

vector<unsigned long long> arr_to_4_u64(
    unsigned char arr[ESSENTIAL_CARDS_TYPES]);

constexpr char *hu_table = "compressed_hu_table";

/// @brief 记录单个玩家的操作动作。
typedef struct _once_profile {
    mahjong_act act;
    mahjong_attr atr;
    mahjong_val val;
    unsigned char caller;  // 发起方。
} profile_datum;

typedef struct _profile {
    profile_datum ptr;
    unsigned char cnt;
} total_profile;