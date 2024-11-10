#include "mahjong.h"

/* 实现最基础的胡牌分。 */
// 88 番
/*
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
static int _88_() {
    return 88;
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

// 32 番
/*
    三杠
    混幺九 111 999 南南南 北北 111
*/

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

// 12 番
/*
    全不靠：字牌没满，
    大于5 6789 组成的和牌
    小于5 1234 组成的和牌
    三风刻 要有三个风刻（杠）
    
*/