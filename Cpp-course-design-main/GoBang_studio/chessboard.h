#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <vector>
#include <stack>
#include <utility>
#include <QString>
#include <QDebug>
#include <algorithm>
#include <acstring.h>
#include <gobangai.h>

#define MOVE(r, c) std::pair<short, short>(r, c)

typedef std::pair<short, short> PSS;

#define PROGRESS_SUCCESS 0
#define GAME_OVER_ERROR -1
#define INVALID_POSITION_ERROR -2
#define MOVCNT_NOT_ENOUGH_ERROR -3

const int H_margin = 30;
const int W_margin = 30;
const int block_size = 50;
const int pos_offset = block_size * 0.45; // 模糊距离，落子点点击范围
const int chesspiece_radius = 20;
const int prompt_radius = chesspiece_radius * 0.3;
const int max_rating = 0x3f3f3f3f;

// 位置坐标：先row（大写字母'A'-'O'）后col（阿拉伯数字1-15）
// chessboard类中所有位置参数均为0-14的数字，只有存入文件中和导入tablewidget才会转化为给人看的坐标

class chessboard
{
private:
    int cb[15][15];
    // 在调用的时候申请空间并且初始化

    // int neighbour[15][15];
    // 专门用于alpha beta剪枝算法，无需关注棋盘边缘地方，可能需要关注的地方是初始化。可以设定在棋盘中央9个格子设为可探索区域
    // 根据经验，一般只用考虑与已经落的棋子的切比雪夫距离为3的格子，可能可以减少计算消耗（仅仅在调用alpha beta剪枝算法的时候才初始化，
    // 否则在落子的时候添加，在删除手顺的时候删除标记 非常麻烦

    /*
     * x x x x x x x
     * x x x x x x x
     * x x x x x x x
     * x x x o x x x
     * x x x x x x x
     * x x x x x x x
     * x x x x x x x
     */
    bool isOver;
    bool isBlackToMove;
    int winner; // 1: 黑胜 0: 平局 -1: 白胜
    int moveCnt; // 一共走了多少步

    std::vector<PSS> moveList; // 手顺列表
    std::vector<double> rating; // 手顺评分

public:
    chessboard(const std::vector<PSS>& movelist = std::vector<PSS>());

    int moveCount();

    // 加上返回 winner  isover 的函数

    PSS getIthMove(int i);

    bool isValidPosition(int row, int col);

    int isReadyToAddMove(int row, int col);

    int isReadyToAddMoveAfterIthMove(int row, int col, int i);

    int addMove(int row, int col); // SUCCESS ; NO VALID MOVE
    // 添加后判断是否获胜

    int delMove(); // 步数>=1才合法

    chessboard* getIthSlice(int i);

    int pieceSequentialNeighbours(int row, int col, int player, int direction); // player: 1 black, -1 white

    bool pieceGeneratesFiveInARow(int row, int col, int player);

    int gameIsOver(); // 1 结束了（同时更新winner和isOver） 0 未结束

    int gameWinner(); // 1: 黑胜 0: 平局 -1: 白胜

//    void initNeighbourArrayWithIthMoves(int (*neighbour)[15], int n);

//    void initNeighbourArray(int (*neighbour)[15]);

    // 下面是算法评分系统部分，

    int evaluate();

    int alphaBeta(/*int (*neighbour)[15], */int depth, int alpha, int beta, bool maximizingPlayer);

    GoBangAI::point* getAI(int depthcfg = 4);

};

#endif // CHESSBOARD_H
