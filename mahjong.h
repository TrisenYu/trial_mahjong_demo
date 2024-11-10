#ifndef     _MAHJONG_H_
#define     _MAHJONG_H_
#endif //   _MAHJONG_H_

#include <ctime>
#include <random>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <algorithm>
#include <thread>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>

#define DEBUG
#define KANJI_PLACEHOLDER L"%ls"
#define KJPH_ABBR KANJI_PLACEHOLDER /* 汉字占位符的缩略形式 */ 


/// @brief 单张牌的类型。
typedef enum _mahjong_attr {
    WANGUAN,    /* 万 0-8 0-8   */ 
    BAMBOO,     /* 条 0-8 9-17  */ 
    CIRCLE,     /* 筒 0-8 18-26 */ 
    HANZI,      /* 字 0-6 27-33 */ 
    FEATURE
} mahjong_attr;

/// @brief 麻将，自豪地使用拼音命名法。
typedef enum _mahjong_mapped_val {
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
    /* 红中发财白板 */
    HONG_ZHONG, 
    FA_CAI, 
    BAI_BAN,
    /* Plum blossoms, orchids, bamboo, chrysanthemum. */
    EXT_P, EXT_O, EXT_B, EXT_C,
    /* spring, summer, autumn, winter. */
    EXT_SPRING, EXT_SUMMER, EXT_AUTUMN, EXT_WINTER
} mahjong_val;

#define SUIT_LEAP               0x09                        /* 花色的跨度为 9. */
#define CARDS_OF_ONE_PLAYER     0x0e                        /* 每个玩家一开始有13张牌，胡牌至少要有 14 张牌 */
#define MAXN_CARDS_OF_ONE       CARDS_OF_ONE_PLAYER + 4     /* 四个杠可以让牌的数量增加到 18. */
#define CARDS_OF_ALL_PLAYER     0x04 * CARDS_OF_ONE_PLAYER  /* 所有玩家可以控制的牌的总数量。*/
#define CARDS_OF_HEAP           0x54                        /* 136 - CARDS_OF_ONE_PLAYER*4 */
#define ESSENTIAL_CARDS_TYPES   0x22                        /* 不计算花牌，牌的类型有 9*3+4+3 种 */
#define TOTAL_CARDS             0x88                        /* 总共有 136 张牌。 */


inline void singular_byte_printer(unsigned char byte) {
    unsigned char stk[8]={0}, cnt = 0;
    while (byte) { stk[cnt++]=byte&1, byte >>= 1; }
    wprintf(L"%d%d%d%d%d%d%d%d\n", stk[7], stk[6], stk[5], stk[4], stk[3], stk[2], stk[1], stk[0]);
}

/// @brief 小端序(little endian binary)输出值。
/// @param ref 所指向的结构。
/// @param bytes_size 结构大小。默认为一个字节。
inline void LEB_form_printer(const void* ref, size_t bytes_size=sizeof(char)) {
    for (size_t i = 0; i < bytes_size; i++) {
        singular_byte_printer((*((unsigned char*)(ref)+i)));
    }
}

/// @brief 输出大字牌。
/// @param honor 字在本程序中的数值表示。
static void honor_decoder(unsigned char honor) {
    void *choice = NULL;
    switch (honor) {
    case 0: choice = (void *)(L"東風"); break;
    case 1: choice = (void *)(L"西風"); break;
    case 2: choice = (void *)(L"南風"); break;
    case 3: choice = (void *)(L"北風"); break;
    case 4: choice = (void *)(L"红中"); break;
    case 5: choice = (void *)(L"發財"); break;
    case 6: choice = (void *)(L"白板"); break;
    default: return;
    }
    wprintf(KJPH_ABBR, (const wchar_t*)(choice));
}

static void input_decoder(const wchar_t *inp) {
    // TODO
}

/// @brief 输出万条筒的数值。应注意到幺鸡被视为一条。
/// @param val 万条筒的数值。
static void digital_val_convertor(unsigned char val, const wchar_t *inject) {
    void *choice = NULL;
    switch (val) {
    case 0: choice = (void *)(L"一"); break;
    case 1: choice = (void *)(L"二"); break;
    case 2: choice = (void *)(L"三"); break;
    case 3: choice = (void *)(L"四"); break;
    case 4: choice = (void *)(L"五"); break;
    case 5: choice = (void *)(L"六"); break;
    case 6: choice = (void *)(L"七"); break;
    case 7: choice = (void *)(L"八"); break;
    case 8: choice = (void *)(L"九"); break;
    default: return;
    }
    wprintf(KJPH_ABBR KJPH_ABBR, (const wchar_t*)(choice), inject);
}

/// @brief 根据类型和数值输出单张牌上记录的内容。
/// @param attr 牌的二进制属性(0123).
/// @param digital_val 牌的数值。
inline static void mahjong_content_printer(mahjong_attr attr, unsigned char digital_val) {
    switch (attr) {
    case WANGUAN: digital_val_convertor(digital_val, L"萬"); return;
    case BAMBOO: digital_val_convertor(digital_val, L"条"); return;
    case CIRCLE: digital_val_convertor(digital_val, L"筒"); return;
    case HANZI: honor_decoder(digital_val); return;
    default: wprintf(L"--"); return;
    }
}


/// @brief 麻将在个数上相当于5进制，维数是34维的向量。前27维中每9维支持单个单个构成连续的顺子。
/// 可将玩家手上的 (_33, ..., _0) 作为表示向量，f(x) 作为胡牌算子, v(X) 作为胡牌的价值映射，问题转化为...
typedef struct _mahjong_ {
    /**
     * 单张牌在存储结构上的定义。
     * 二进制压缩表示 -> 十进制连续表示 -> 中文原意。
     */
    unsigned char meta; // (valid,shown,attr,val) := (x,0,00,0000)

    /* 析构函数和构造函数。 */

    /// @brief 默认设置为无效、在使用的牌。
    inline _mahjong_() { this->meta = 0xFF; }

    /// @brief 依靠十进制数值初始化这张牌。
    /// @param oct 不含有有效位和展示位信息的数值。 
    inline _mahjong_(unsigned char oct) { 
        unsigned char tmp = oct/9;
        this->meta = (tmp<<4) | ((oct-tmp*9)&0x0F); 
    }
    ~_mahjong_() { }

    /* 获取元数据中的属性或数值。 */
    /// @brief 校验牌自身的类型。如果无效，返回特性。
    inline mahjong_attr get_attr() {
        if (this->meta >> 7) {
            return FEATURE;
        }
        return (mahjong_attr)((this->select_attr()));
    }

    /// @brief 翻转使用位
    inline void flip_inused() { this->meta ^= 0b1000000; }

    /// @brief 从设计的存储结构中选出麻将的类型。
    /// @param val 存储结构。低 4 位作为数值，6-5位作为属性。
    /// @return 属性值。0表示为万，1表示为条，2表示为筒，3表示为字。
    inline unsigned char select_attr() { return (this->meta >> 4) & 3; }
    
    /// @brief 获取使用位的值。
    /// @return 使用位的值。
    inline unsigned char revealer() { return (this->meta >> 6) & 1; }

    /// @brief 获取牌的数值 (eg. 1条,9万,3筒,南风)
    inline unsigned char get_seq_val() { return this->meta & 0x0F; }
    
    /// @brief 作为下标映射.这样 0-33 都可以用
    inline unsigned char idx_val() {
        return this->select_attr()*9 + (this->meta & 0x0F);
    }

    /// @brief 输出这张牌的在现实下的语义。
    inline void self_print() {
        mahjong_attr now = this->get_attr();
        unsigned char remain = this->get_seq_val();
    #ifdef      DEBUG_mahjong_struct
        wprintf(L"%d-%d\n", now, remain);
    #endif //   DEBUG_mahjong_struct
        mahjong_content_printer(now, remain);
    }

    /* 重载运算符 */

    _mahjong_& operator =(const _mahjong_& ref) {
        this->meta = ref.meta;
    #ifdef      DEBUG_mahjong_struct
        LEB_form_printer((const void*)(&ref.meta));
    #endif //   DEBUG_mahjong_struct
        return *this;
    }
    inline bool operator <(const _mahjong_& ref) { 
        switch (((ref.meta >> 6) & 0x1) | this->revealer() << 1) {
        case 1: return 1;
        case 2: return 0;
        default: return this->meta < ref.meta; 
        }
    }
    inline bool operator >(const _mahjong_& ref) { 
        switch (((ref.meta >> 6) & 0x1) | this->revealer() << 1) {
        case 1: return 0;
        case 2: return 1;
        default: return this->meta > ref.meta; 
        }
    }
} mahjong;


/// @brief 均匀随机数生成器。
class unitform_rand_gen {
public:
    unitform_rand_gen() {
        this->e.seed(this->rd());
        this->u = std::uniform_int_distribution<int>(0, TOTAL_CARDS-1); // 左闭右闭区间
    }
    /// @brief 前闭后闭区间来设置均匀分布的取值。
    /// @param l 左端点。取到。
    /// @param r 右端点。取到。
    unitform_rand_gen(int l, int r) {
        this->e.seed(this->rd());
        this->u = std::uniform_int_distribution<int>(l, r);
    }
    unsigned char fetch_one() { return this->u(this->e); }
private:
    std::random_device rd;
    std::mt19937 e;
    std::uniform_int_distribution<int> u;
};


/// @brief 玩家信息。
class info_of_players {
public:
    /// @todo 统计玩家的失败次数，过多安排点好牌。
    unsigned char *cards=NULL; // 手上的牌数。
    /* 碰 吃 杠队列。 */
    unsigned char is_dealer=0; // 是否是庄家
    mahjong curr_card;   // 当前从牌堆获取到的牌。

    info_of_players() { this->record_initializer(); }
    info_of_players(unsigned char arr[ESSENTIAL_CARDS_TYPES], unsigned char is_dealer) {
        this->record_initializer();
        this->cards = arr;
    }

    /// @brief 内部的访问限定上只能是不超过游戏进行时的值. 而存在杠的情况就可以额外写入冗余的空间。
    /// @param x 下标位置。
    /// @return 下标指向的牌。
    unsigned char& operator[](int x) {
        if (x < 0 || x >= ESSENTIAL_CARDS_TYPES) {
            throw "out of boundary";
        }
        return cards[x];
    }

    void discard_a_card() {
        // 接入决策逻辑以判别该出哪张牌。
    }

private:
    void record_initializer() {
        for (size_t i = 0; i < ESSENTIAL_CARDS_TYPES; i++) {
            this->recite_record[i] = 4;
        }
    }
    unsigned char recite_record[ESSENTIAL_CARDS_TYPES]; // 已知牌的情况。
    /*
    后期实现考虑加入对对手的概率预测算法。
    unsigned char last_one[MAXN_CARDS_OF_ONE], 
                  next_one[MAXN_CARDS_OF_ONE], 
                  opposite[MAXN_CARDS_OF_ONE]; 
    */
};


extern unitform_rand_gen uniform_gen /* 针对洗牌的随机数生成器 */, dice /* 决定庄家的随机数生成器 */;
extern mahjong heap[];
extern unsigned char player[4][ESSENTIAL_CARDS_TYPES];
extern int heap_cnt, game_over;
extern info_of_players mamba[4];


inline unsigned char all_alone(
    unsigned char arr[ESSENTIAL_CARDS_TYPES],
    unsigned char cnts[6]
);
inline unsigned char _13_orphans_checker(
    unsigned char arr[ESSENTIAL_CARDS_TYPES],
    unsigned char cnts[6]
);
inline unsigned char _7_pairs_or_4_quadruplet(
    unsigned char arr[ESSENTIAL_CARDS_TYPES], 
    unsigned char cnts[6]
);

static unsigned char normal_checker(
    unsigned char arr[ESSENTIAL_CARDS_TYPES],
    unsigned char pair, 
    unsigned char sequence, 
    unsigned char triplet
);


int hu_checker(unsigned char arr[ESSENTIAL_CARDS_TYPES]);
void permutate_cards();

#define is_a_seq(arr, ref, i) (arr[ref+i] > 0 && arr[ref+i+1] > 0 && arr[ref+i+2] > 0)
#define homomorphizer(arr, ref, i, ptr, func)                   \
do {                                                            \
    while (is_a_seq(arr, ref, i)) {                             \
        func(arr+ref+i), func(arr+ref+i+1), func(arr+ref+i+2);  \
        (*(ptr))++;                                             \
    }                                                           \
} while(0)


#ifdef      _WIN32
#define REQUIRED_LOCAL "chs"
#elifdef    _WIN64
#define REQUIRED_LOCAL "chs"
#else
#define REQUIRED_LOCAL "zh_CN"
#endif //   OS_TYPE