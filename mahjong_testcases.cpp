#include "mahjong.h"

unsigned char error_test_case1[ESSENTIAL_CARDS_TYPES] = {
    // pure error
    4, 4, 4, 4, 4, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0
};
unsigned char error_test_case2[ESSENTIAL_CARDS_TYPES] = {
    // 13 orphans
    1, 0, 0, 0, 0, 0, 0, 0, 2,
    1, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1,
};
unsigned char error_test_case3[ESSENTIAL_CARDS_TYPES] = {
    /* 234234 678678 88 */
    2, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1
};
unsigned char error_test_case4[ESSENTIAL_CARDS_TYPES] = {
    // pure error
    3, 3, 3, 2, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1
};
unsigned char error_test_case5[ESSENTIAL_CARDS_TYPES] = {
    // dd abc abc aaa aaa
    0, 0, 0, 0, 0, 0, 4, 2, 2,
    0, 3, 0, 0, 0, 0, 0, 0, 0,
    0, 3, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0
};
unsigned char error_test_case6[ESSENTIAL_CARDS_TYPES] = {
    // 22 234 234 555 222
    0, 4, 2, 2, 3, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 3, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0
};
unsigned char error_test_case7[ESSENTIAL_CARDS_TYPES] = {
    // 22 22 33 44 55 22 nn
    0, 4, 2, 2, 2, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 2, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 2, 0, 0, 0, 0
};
unsigned char error_test_case8[ESSENTIAL_CARDS_TYPES] = {
    0, 0, 1, 1, 4, 1, 1, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 3, 0, 0, 0, 0
};
unsigned char error_test_case9[ESSENTIAL_CARDS_TYPES] = {
    // 7 对
    0, 0, 0, 2, 0, 0, 0, 0, 0,
    0, 0, 2, 0, 0, 2, 0, 0, 0,
    0, 2, 0, 0, 4, 0, 0, 0, 0,
    0, 0, 2, 0, 0, 0, 0
};
unsigned char error_test_case10[ESSENTIAL_CARDS_TYPES] = {
    // 234 345 555 123 11
    3, 2, 3, 2, 4, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0
};
unsigned char error_test_case11[ESSENTIAL_CARDS_TYPES] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
unsigned char error_test_case12[ESSENTIAL_CARDS_TYPES] = {
    4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4
};
unsigned char error_test_case13[ESSENTIAL_CARDS_TYPES] = {
    // 九莲宝灯
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 1, 1, 1, 1, 1, 1, 2, 3,
    0, 0, 0, 0, 0, 0, 0
};
unsigned char error_test_case14[ESSENTIAL_CARDS_TYPES] = {
    // 九莲宝灯
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    4, 1, 1, 1, 1, 1, 1, 1, 3,
    0, 0, 0, 0, 0, 0, 0
};
unsigned char error_test_case15[ESSENTIAL_CARDS_TYPES] = {
    // 九莲宝灯
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 1, 1, 1, 1, 1, 1, 1, 4,
    0, 0, 0, 0, 0, 0, 0
};
unsigned char error_test_case16[ESSENTIAL_CARDS_TYPES] = {
    // 组合龙。
    1, 0, 0, 1, 0, 0, 1, 0, 0,
    0, 1, 0, 0, 1, 0, 0, 1, 0,
    0, 0, 1, 0, 0, 1, 0, 0, 1,
    0, 0, 2, 0, 3, 0, 0
};
unsigned char error_test_case17[ESSENTIAL_CARDS_TYPES] = {
    // 组合龙。
    1, 0, 0, 1, 1, 1, 2, 0, 0,
    0, 1, 0, 0, 1, 0, 0, 1, 0,
    0, 0, 1, 0, 0, 1, 0, 0, 1,
    0, 0, 2, 0, 0, 0, 0
};
extern info_of_players mamba[4];
extern mahjong heap[CARDS_OF_HEAP];
extern unsigned char player[4][ESSENTIAL_CARDS_TYPES];
extern int heap_cnt, game_over;
extern unitform_rand_gen uniform_gen, dice;

#define expect_1 "expect: 1, res: %d\n"
#define expect_0 "expect:-1, res: %d\n"
inline static void printer(const char *ptr, unsigned char testcase[]) { printf(ptr, hu_checker(testcase)); }
int main(int argc, char *argv[]) {
    printer(expect_0, error_test_case1);
    printer(expect_1, error_test_case2);
    printer(expect_1, error_test_case3);
    printer(expect_0, error_test_case4);
    printer(expect_1, error_test_case5);
    printer(expect_1, error_test_case6);
    printer(expect_1, error_test_case7);
    printer(expect_1, error_test_case8);
    printer(expect_1, error_test_case9);
    printer(expect_1, error_test_case10);
    printer(expect_0, error_test_case11);
    printer(expect_0, error_test_case12);
    printer(expect_1, error_test_case13);
    printer(expect_1, error_test_case14);
    printer(expect_1, error_test_case15);
    printer(expect_1, error_test_case16);
    printer(expect_1, error_test_case17);
#undef expect_1
#undef expect_0

    setlocale(LC_ALL, REQUIRED_LOCAL);
    unsigned char dealer_is = dice.fetch_one();
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
    }
    catch (...) {
        puts("execute failed.");
    }
    return 0*puts("ok");
    return 0;
}