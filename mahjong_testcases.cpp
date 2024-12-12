#include <cassert>

#include "mahjong.h"
#include "mahjong_const.h"
#include "trial_mahjong.h"
// clang-format off
unsigned char error_test_case[][ESSENTIAL_CARDS_TYPES] = {{
    // pure error
    4, 4, 4, 4, 4, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0
}, {
    // 13 orphans
    1, 0, 0, 0, 0, 0, 0, 0, 2,
    1, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1,
}, {
    2, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1
}, {
    // pure error
    3, 3, 3, 2, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1
}, {
    // dd abc abc aaa aaa
    0, 0, 0, 0, 0, 0, 4, 2, 2,
    0, 3, 0, 0, 0, 0, 0, 0, 0,
    0, 3, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0
}, {
    // 22 234 234 555 222
    0, 4, 2, 2, 3, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 3, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0
}, {
    // 22 22 33 44 55 22 nn
    0, 4, 2, 2, 2, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 2, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 2, 0, 0, 0, 0
}, {
    // that is normal hu
    0, 0, 1, 1, 4, 1, 1, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 3, 0, 0, 0, 0
}, {
    // 7 对
    0, 0, 0, 2, 0, 0, 0, 0, 0,
    0, 0, 2, 0, 0, 2, 0, 0, 0,
    0, 2, 0, 0, 4, 0, 0, 0, 0,
    0, 0, 2, 0, 0, 0, 0
}, {
    // 234 345 555 123 11
    3, 2, 3, 2, 4, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0
}, {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
}, {
    4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4
}, {
    // 九莲宝灯
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 1, 1, 1, 1, 1, 1, 2, 3,
    0, 0, 0, 0, 0, 0, 0
}, {
    // 九莲宝灯
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    4, 1, 1, 1, 1, 1, 1, 1, 3,
    0, 0, 0, 0, 0, 0, 0
}, {
    // 九莲宝灯
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 1, 1, 1, 1, 1, 1, 1, 4,
    0, 0, 0, 0, 0, 0, 0
}, {
    // 组合龙。
    1, 0, 0, 1, 0, 0, 1, 0, 0,
    0, 1, 0, 0, 1, 0, 0, 1, 0,
    0, 0, 1, 0, 0, 1, 0, 0, 1,
    0, 0, 2, 0, 3, 0, 0
}, {
    // 组合龙。
    1, 0, 0, 1, 1, 1, 2, 0, 0,
    0, 1, 0, 0, 1, 0, 0, 1, 0,
    0, 0, 1, 0, 0, 1, 0, 0, 1,
    0, 0, 2, 0, 0, 0, 0
}, {
    1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 3, 0, 0, 0, 0, 0, 0,
    2, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0
}, {
    // 567 345 456 77 567
    0, 0, 0, 0, 1, 1, 1, 0, 0,
    0, 0, 1, 2, 2, 1, 2, 0, 0,
    0, 0, 0, 0, 1, 1, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0
}, {
    // 567 345 456 77 567
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 2, 2, 2, 2, 2, 2, 2, 2,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0
}, {
    // 567 345 456 77 567
    0, 0, 0, 0, 0, 0, 0, 0, 0, // w
    0, 0, 2, 2, 2, 2, 2, 2, 2, // t
    0, 0, 0, 0, 0, 0, 0, 0, 0, // g
    0, 0, 0, 0, 0, 0, 0        // z
}, {
    // 7 对
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 2, 2, 2, 2, 2, 4, 0, 0,
    0, 0, 0, 0, 0, 0, 0
}, {
    // 4 杠
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 4, 4, 4, 4, 2, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0
}, {
    // 7 对
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    4, 0, 4, 0, 4, 0, 0, 2, 0,
    0, 0, 0, 0, 0, 0, 0
}, {
    // 24? 14 张 1，pure error
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0
}, {
    // 14 张 error
    0, 1, 0, 1, 0, 1, 0, 1, 0,
    1, 0, 1, 0, 1, 0, 1, 0, 1,
    0, 1, 0, 1, 0, 1, 0, 1, 0,
    0, 0, 0, 0, 1, 0, 0
}, {
    // 全空
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0
}, {
    // 组合龙。
    1, 0, 0, 1, 0, 0, 1, 0, 0,
    0, 1, 2, 1, 2, 1, 0, 1, 0,
    0, 0, 1, 0, 0, 1, 0, 0, 1,
    0, 0, 0, 0, 0, 0, 0
}, {
    // 组合龙。
    1, 0, 0, 1, 0, 0, 1, 0, 0,
    2, 2, 1, 1, 1, 0, 0, 1, 0,
    0, 0, 1, 0, 0, 1, 0, 0, 1,
    0, 0, 0, 0, 0, 0, 0
}, {
    // 可胡
    // 123 222  123 22 
    1, 4, 1, 0, 0, 0, 0, 0, 0,
    1, 3, 1, 0, 0, 0, 0, 3, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0
}, {
    // 可胡
    // 456 234 234 1111 33 
    // 抽掉对子后，4 2 2 3 1 1
    // 如果先抽顺子 2 0 0 2 0 0 碰没有处理好
    // 如果先抽碰 0 2 2 0 1 1 顺子也没有处理好。
    // 本质上是要求按顺序处理时，允许碰和顺交替进行。
    4, 2, 4, 3, 1, 1, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0
}, {
    // 111 22 234 333 444
    3, 3, 4, 4, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0
}
};
// clang-format on

extern info_of_players mamba[4];
extern mahjong heap[CARDS_OF_HEAP];
extern unsigned char player[4][ESSENTIAL_CARDS_TYPES];
extern int heap_cnt, game_over;
extern unitform_rand_gen uniform_gen, dice;
static int __cnt__ = 1;
inline static void printer(const char *ptr, unsigned char testcase[]) {
    printf(ptr, __cnt__, hu_checker(testcase));
    __cnt__++;
}

int main(int argc, char *argv[]) {
#define expect_1 "%2d expect: *, res: %2d\n"
#define expect_0 "%2d expect:-1, res: %2d\n"
    printer(expect_0, error_test_case[0]);
    printer(expect_1, error_test_case[1]);
    printer(expect_1, error_test_case[2]);
    printer(expect_0, error_test_case[3]);
    printer(expect_1, error_test_case[4]);
    printer(expect_1, error_test_case[5]);
    printer(expect_1, error_test_case[6]);
    printer(expect_1, error_test_case[7]);
    printer(expect_1, error_test_case[8]);
    printer(expect_1, error_test_case[9]);
    printer(expect_0, error_test_case[10]);
    printer(expect_0, error_test_case[11]);
    printer(expect_1, error_test_case[12]);
    printer(expect_1, error_test_case[13]);
    printer(expect_1, error_test_case[14]);
    printer(expect_1, error_test_case[15]);
    printer(expect_1, error_test_case[16]);
    printer(expect_1, error_test_case[17]);
    printer(expect_1, error_test_case[18]);
    printer(expect_0, error_test_case[19]);
    printer(expect_1, error_test_case[20]);
    printer(expect_1, error_test_case[21]);
    printer(expect_1, error_test_case[22]);
    printer(expect_1, error_test_case[23]);
    printer(expect_0, error_test_case[24]);
    printer(expect_0, error_test_case[25]);
    printer(expect_0, error_test_case[26]);
    printer(expect_1, error_test_case[27]);
    printer(expect_1, error_test_case[28]);
    printer(expect_1, error_test_case[29]);
    printer(expect_1, error_test_case[30]);
    printer(expect_1, error_test_case[31]);
#undef expect_1
#undef expect_0
    setlocale(LC_ALL, REQUIRED_LOCAL);
    unsigned char dealer_is    = dice.fetch_one();
    mamba[dealer_is].is_dealer = 1;
    wprintf(L"当前庄家：%d\n", dealer_is);

    /// TODO: 也许偏重在计算？并不是执行出牌的逻辑。
    // std::thread gaming_ai[3];
    // for (size_t i = 0; i < 3; i ++)
    //     gaming_ai[i] = std::thread(pseudo_ai_constructor, i);
    // for (size_t i = 0; i < 3; i ++)
    //     gaming_ai[i].join();

    try {
        permutate_cards();
        vector<unsigned long long> curr = arr_to_4_u64(error_test_case[5]);
        trial_mahjongs_in_hand cur(curr[0], curr[1], curr[2], curr[3]);
        printf("expect 1, res: %d\n", cur.hu_checker());
    } catch (...) { puts("execute failed."); }

    trial_mahjongs_in_hand tmp(
        // 1111 234 234 44 5555
        0x0'424'240'000,
        0x0'000'000'000,
        0x0'000'000'000,
        0x0'000'000'000);
    printf(
        "size: %ld. expected: 1, res: %d\n",
        sizeof(trial_mahjongs_in_hand),
        tmp.hu_checker());
    for (int i = 0; i < 32; i++) {
        trial_mahjongs_in_hand temp(error_test_case[i]);
        unsigned long long tmp[4] = {temp.w, temp.t, temp.b, temp.f};
        printf("round %2d res: %2d\n", 1 + i, temp.hu_checker());
        assert(tmp[0] == temp.w);
        assert(tmp[1] == temp.t);
        assert(tmp[2] == temp.b);
        assert(tmp[3] == temp.f);
    }
    return 0 * puts("ok");
}