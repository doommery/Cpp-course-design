// 此处坐标范围[1-15][1-15]，两个都要减一
#ifndef GOBANGAI_H
#define GOBANGAI_H

#include <algorithm>
#include <cstring>
#include <deque>
#include <iostream>
#include <queue>
#include <set>
#include <stack>
#include "acstring.h"
#include <QDebug>

typedef  long long ULL;
typedef std::pair<int, int> PII;

const int INF = 0x3f3f3f3f,
          N = 20,
          M = 15,
          dx[] = { 1, 0, 1, 1 },
          dy[] = { 0, 1, 1, -1 }; // 8联通方向偏移

// Key-val分值表
extern std::vector<Pattern> patterns;

// 黑、白棋的zobrist表，内存64bit随机数
extern ULL zobrist_black[N][N], zobrist_white[N][N];

// zobrist-list
extern std::unordered_map<ULL,int> zobrist_list;

// Ac自动机
extern AcString* acs;

// 获取64bit随机数
ULL getRandom64();

// 初始化Zobrist哈希表
void initZobrist();

class GoBangAI {
public:
    struct point {
        bool isBlack; // 1--black,0--white
        int x, y, score;

        point()
        {
            this->isBlack = 0;
            this->x = 1;
            this->y = 1;
            this->score = 0;
        }

        // 构造函数
        point(bool isBlack, int x, int y)
        {
            this->isBlack = isBlack;
            this->x = x;
            this->y = y;
            this->score = 0;
        }

        point(bool isBlack, int x, int y, int score)
        {
            this->isBlack = isBlack;
            this->x = x;
            this->y = y;
            this->score = score;
        }
    };

    //初始化，传入当前局面，该走的颜色，搜索深度
    GoBangAI(std::vector<point>& state, bool nextisBlack, int depthmax);

    ~GoBangAI();

    // 获得AI计算得到的手顺（含评分）
    point getStep();

private:
    // AI计算得到的在这一局面下的最佳走法
    point aistep;

    // Alpha-Beta剪枝算法
    GoBangAI::point alphaBetaAlgorithm(std::vector<point>& state, bool nowisBlack, int depth, int alpha, int beta, int maxdepth);

    // 局面评估
    int evluateSituation(std::vector<point>& state, bool nextisBlack);

    // 获取某点在局面为某点的各个联通方向的分数
    int getScore(bool isBlack, int x, int y, int g[][N]);

    // 获取某点的某方向分数
    int getScoreInDir(bool isBlack, int x, int y, std::vector<point> state, int dir);

    // 判断位置是否在棋盘中
    bool isValidPosition(int x, int y);

    //用于获得当前层最优可落点并返回按分值从大到小排序后的节点
    std::stack<point> getPossibleNextStep(std::vector<point>& state, bool nowisBlack);

    // 获得当前局面的ZorbristKey
    static ULL getZobristKey(std::vector<point> state);
};

#endif // GOBANGAI_H
