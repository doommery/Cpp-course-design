#include "chessboard.h"

chessboard::chessboard(const std::vector<PSS> &movelist)
{
    if (movelist == std::vector<PSS>()) {
        // 创建空局
        for (int i = 0; i < 15; i ++ ) {
            for (int j = 0; j < 15; j ++ ) {
                cb[i][j] = 0;
            }
        }
        isOver = false;
        isBlackToMove = true;
        winner = 0; // 1: 黑胜 0: 平局 -1: 白胜
        moveCnt = 0; // 一共走了多少步
        moveList.clear(); // 置空
    }
    else {
        // 依据导入的文件产生的移动列表创建局面
        moveList = movelist;
        moveCnt = movelist.size();

        // 置空棋盘
        for (int i = 0; i < 15; i ++ ) {
            for (int j = 0; j < 15; j ++ ) {
                cb[i][j] = 0;
            }
        }

        // 添加记录
        int order = 0;
        for (auto i : movelist) {
            if (cb[i.first][i.second] == 0) {
                ++ order;
                cb[i.first][i.second] = (order % 2 == 1 ? 1 : -1) * order;
            }
            else {
                // 棋谱有问题
                qDebug() << "In file chessboard.cpp: chessboard(): error initiating chessboard...\n";
                qDebug() << "The No." << order << " move is not valid!\n";
                qDebug() << "In file chessboard.cpp: chessboard(): process returned.\n";
                return;
            }
        }

        if (order % 2 == 0) {
            isBlackToMove = true;
        }
        else {
            isBlackToMove = false;
        }

        // 判断是否结束对局
        if (order <= 15 * 15) {
            //
            isOver = false;
            winner = 0; // 1: 黑胜 0: 平局 -1: 白胜
            // 平局: 所有格子都被占满了，但是没有五连珠

            //  此处不考虑赢了还接着下的情况，也就是认为只有最后一步才可能决出胜负
            int currentPlayer = moveCnt % 2 == 1 ? 1 : -1;

            // 判定是否胜利
            if (pieceGeneratesFiveInARow(movelist[moveCnt - 1].first, movelist[moveCnt - 1].second, currentPlayer)) {
                isOver = true;
                winner = currentPlayer;
            }

            // 无子可落，判定和棋
            if (!isOver && order == 15 * 15) {
                isOver = true;
                winner = 0;
            }
        }
        else {
            // 步数不合法
            qDebug() << "In file chessboard.cpp: chessboard(): error analyzing movelist...\n";
            qDebug() << "The count of the moves is too large(>15 * 15)!\n";
            qDebug() << "In file chessboard.cpp: chessboard(): process returned.\n";
            isOver = true;
            winner = 0;
            return;
        }
    }
}

int chessboard::moveCount()
{
    return moveCnt;
}

PSS chessboard::getIthMove(int i)
{
    if (i >= 0 && i < moveList.size()) return moveList[i];
    else {
        qDebug() << "In file chessboard.cpp: getIthMove(): error analyzing movelist...\n";
        qDebug() << "The order of the move is invalid!\n";
        qDebug() << "In file chessboard.cpp: getIthMove(): process returned.\n";
        return PSS();
    }
}

bool chessboard::isValidPosition(int row, int col)
{
    return row >= 0 && row <= 14 &&
           col >= 0 && col <= 14;
}

int chessboard::isReadyToAddMove(int row, int col)
{
    return isValidPosition(row, col) && cb[row][col] == 0;
}

int chessboard::isReadyToAddMoveAfterIthMove(int row, int col, int i)
{
    return isValidPosition(row, col) && (cb[row][col] == 0 || cb[row][col] > i || cb[row][col] < - i);
}

int chessboard::addMove(int row, int col) // SUCCESS ; NO VALID MOVE// 添加后判断是否获胜
{
    // 判断是否结束
    if (gameIsOver()) {
        qDebug() << "In file chessboard.cpp: addMove(): error analyzing movelist...\n";
        qDebug() << "The count of the moves is too large(>15 * 15)!\n";
        qDebug() << "In file chessboard.cpp: addMove(): process returned.\n";
        return GAME_OVER_ERROR;
    }

    if (isValidPosition(row, col) && cb[row][col] == 0) {
        // 落子处空白，可以落子
        moveList.push_back(MOVE(row, col));
        moveCnt ++;

        int currentPlayer = moveCnt % 2 == 1 ? 1 : -1;
        cb[row][col] = currentPlayer * moveCnt;

        isBlackToMove = moveCnt % 2 == 0 ? 1 : -1;

        // 判定是否胜利        
        if (pieceGeneratesFiveInARow(row, col, currentPlayer)) {
            isOver = true;
            winner = currentPlayer;
        }

        // 无子可落，判定和棋
        if (!isOver && moveCnt == 15 * 15) {
            isOver = true;
            winner = 0;
        }

        return PROGRESS_SUCCESS;
    }
    else {
        // 位置非法或者不可落子
        qDebug() << "In file chessboard.cpp: addMove(): error analyzing movelist...\n";
        qDebug() << "The position of the move is invalid!\n";
        qDebug() << "In file chessboard.cpp: addMove(): process returned.\n";
        return INVALID_POSITION_ERROR;
    }
}

int chessboard::delMove() // 步数>=1才合法
{
    if (moveCnt >= 1) {
        cb[moveList.back().first][moveList.back().second] = 0;
        moveList.pop_back();
        moveCnt -- ;
        isBlackToMove = moveCnt % 2 == 0;
        isOver = false;

//        rating.pop_back(); // 手顺评分

        return PROGRESS_SUCCESS;
    }
    else {
        qDebug() << "In file chessboard.cpp: delMove(): error deleting the last move...\n";
        qDebug() << "In file chessboard.cpp: delMove(): process returned(MOVCNT_NOT_ENOUGH_ERROR).\n";
        return MOVCNT_NOT_ENOUGH_ERROR;
    }
}

chessboard* chessboard::getIthSlice(int i)
{
    if (i > moveCnt) {
        qDebug() << "In file chessboard.cpp: getIthSlice(): error analyzing movelist...\n";
        qDebug() << "The parameter of i is too large!\n";
        qDebug() << "In file chessboard.cpp: getIthSlice(): process returned.\n";
        return this;
    }
    else if (i == moveCnt) {
        return this;
    }
    else if (i >= 0) {
        return new chessboard(std::vector<PSS>(this->moveList.begin(), this->moveList.begin() + i));

    }
    else {
        // i < 0
        qDebug() << "In file chessboard.cpp: getIthSlice(): error analyzing movelist...\n";
        qDebug() << "The parameter of i is not positive!\n";
        qDebug() << "In file chessboard.cpp: getIthSlice(): process returned(this).\n";
        return this;
    }
}

int chessboard::pieceSequentialNeighbours(int row, int col, int player, int direction)
{
    // direction定义
    /*         2
     *       3 ^ 1
     *        \|/
     *      4<-o->0
     *        /|\
     *       5 v 7
     *         6
     * 函数参数表示（不包含此棋子）有向方向上的棋子个数，一个斜线上最多连珠要两个相加，与i相对的方向是(i + 4) % 8
     * 记得加上1，并且判定位置是否合法。
     */

    int i = 1;
    // player: 1 black, -1 white
    if (player == 1) {
        switch (direction) {
        case 0:
            for (; i <= 4; i ++ ) {
                if (!(isValidPosition(row, col + i) && cb[row][col + i] > 0)) {
                    i -- ;
                    break;
                }
            }
            break;
        case 1:
            for (; i <= 4; i ++ ) {
                if (!(isValidPosition(row - i, col + i) && cb[row - i][col + i] > 0)) {
                    i -- ;
                    break;
                }
            }
            break;
        case 2:
            for (; i <= 4; i ++ ) {
                if (!(isValidPosition(row - i, col) && cb[row - i][col] > 0)) {
                    i -- ;
                    break;
                }
            }
            break;
        case 3:
            for (; i <= 4; i ++ ) {
                if (!(isValidPosition(row - i, col - i) && cb[row - i][col - i] > 0)) {
                    i -- ;
                    break;
                }
            }
            break;
        case 4:
            for (; i <= 4; i ++ ) {
                if (!(isValidPosition(row, col - i) && cb[row][col - i] > 0)) {
                    i -- ;
                    break;
                }
            }
            break;
        case 5:
            for (; i <= 4; i ++ ) {
                if (!(isValidPosition(row + i, col - i) && cb[row + i][col - i] > 0)) {
                    i -- ;
                    break;
                }
            }
            break;
        case 6:
            for (; i <= 4; i ++ ) {
                if (!(isValidPosition(row + i, col) && cb[row + i][col] > 0)) {
                    i -- ;
                    break;
                }
            }
            break;
        case 7:
            for (; i <= 4; i ++ ) {
                if (!(isValidPosition(row + i, col + i) && cb[row + i][col + i] > 0)) {
                    i -- ;
                    break;
                }
            }
            break;
        }
    }
    else if (player == -1) {
        switch (direction) {
        case 0:
            for (; i <= 4; i ++ ) {
                if (!(isValidPosition(row, col + i) && cb[row][col + i] < 0)) {
                    i -- ;
                    break;
                }
            }
            break;
        case 1:
            for (; i <= 4; i ++ ) {
                if (!(isValidPosition(row - i, col + i) && cb[row - i][col + i] < 0)) {
                    i -- ;
                    break;
                }
            }
            break;
        case 2:
            for (; i <= 4; i ++ ) {
                if (!(isValidPosition(row - i, col) && cb[row - i][col] < 0)) {
                    i -- ;
                    break;
                }
            }
            break;
        case 3:
            for (; i <= 4; i ++ ) {
                if (!(isValidPosition(row - i, col - i) && cb[row - i][col - i] < 0)) {
                    i -- ;
                    break;
                }
            }
            break;
        case 4:
            for (; i <= 4; i ++ ) {
                if (!(isValidPosition(row, col - i) && cb[row][col - i] < 0)) {
                    i -- ;
                    break;
                }
            }
            break;
        case 5:
            for (; i <= 4; i ++ ) {
                if (!(isValidPosition(row + i, col - i) && cb[row + i][col - i] < 0)) {
                    i -- ;
                    break;
                }
            }
            break;
        case 6:
            for (; i <= 4; i ++ ) {
                if (!(isValidPosition(row + i, col) && cb[row + i][col] < 0)) {
                    i -- ;
                    break;
                }
            }
            break;
        case 7:
            for (; i <= 4; i ++ ) {
                if (!(isValidPosition(row + i, col + i) && cb[row + i][col + i] < 0)) {
                    i -- ;
                    break;
                }
            }
            break;
        }
    }
    return i;
}

bool chessboard::pieceGeneratesFiveInARow(int row, int col, int player)
{
    // 当落子的时候会判断，或者导入文件的时候会判断，或者构造函数中构建棋局的时候判断
    bool ok = false;
    for (int direction = 0; direction < 4; direction ++ ) {
        if (pieceSequentialNeighbours(row, col, player, direction) +   // 有向方向direction
            pieceSequentialNeighbours(row, col, player, direction + 4) // 有向方向direction的反方向
            + 1 // 还要带上自身
            >= 5) {
            ok = true;
            break;
        }
    }
    return ok;
}

int chessboard::gameIsOver() // 1 结束了（同时更新winner和isOver） 0 未结束
{
    if (moveCnt < 9) {
        // 至少要9步才可能结束，同时保证能够调用到最后一个手顺，要根据最后一个手顺来进行判断
        return isOver = false;
    }
    else {
        // 判断最后一步是否制胜或平手
        return isOver;
    }
}

int chessboard::gameWinner() // 1: 黑胜 0: 平局 -1: 白胜
{
    return winner;
}

//void chessboard::initNeighbourArrayWithIthMoves(int (*neighbour)[15], int n)
//{
//    for (int i = 0; i < 15; ++ i)
//        for (int j = 0; j < 15; ++ j)
//            neighbour[i][j] = 0;

//    // neighbour为0：不能放子或者不考虑，为1：考虑近邻
//    for (auto move : moveList) {
//        if (n -- ) {
//            for (int i = 0; i < 7; ++ i) {
//                for (int j = 0; j < 7; ++ j) {
//                    if (isReadyToAddMove(move.first - 3 + i, move.second - 3 + j)) {
//                        neighbour[move.first - 3 + i][move.second - 3 + j] = 1;
//                    }
//                }
//            }
//        }
//    }
//}

////int mmm(int (*x)[15]) {
////    return x[13][1];
////}

//void chessboard::initNeighbourArray(int (*neighbour)[15])
//{
//    for (int i = 0; i < 15; ++ i)
//        for (int j = 0; j < 15; ++ j)
//            neighbour[i][j] = 0;

//    // neighbour为0：不能放子或者不考虑，为1：考虑近邻
//    for (auto move : moveList) {
//        for (int i = 0; i < 7; ++ i) {
//            for (int j = 0; j < 7; ++ j) {
//                if (isReadyToAddMove(move.first - 3 + i, move.second - 3 + j)) {
//                    neighbour[move.first - 3 + i][move.second - 3 + j] = 1;
//                }
//            }
//        }
//    }
//}

int chessboard::evaluate() {
    // 这里是一个简单的示例，实际上你需要根据棋局实现自己的评估逻辑
    if (gameIsOver()) {
        if (winner == 1)
            return max_rating; // 黑胜
        else if (winner == -1)
            return - max_rating; // 白胜
        else
            return 0; // 平局
    }

    // 计算活三活四

    // 棋盘评分初始为0
    int score = 0;

    // 遍历整个棋盘计算评分
    for (int row = 0; row < 15; row++) {
        for (int col = 0; col < 15; col++) {
            // 对于棋盘上的每个黑子或白子，检查它们能形成的活三、活四等情况
            if (cb[row][col] != 0) {
                int player = cb[row][col] > 0 ? 1 : -1;  // 根据值的正负判断是黑棋还是白棋

                // 活三、活四的分数
                int live_three_score = 800;
                int live_four_score = 5000;

                // 检查每个方向
                for (int direction = 0; direction < 4; ++ direction) {
                    int line_length = pieceSequentialNeighbours(row, col, player, direction) +
                                      pieceSequentialNeighbours(row, col, player, direction + 4) + 1;

                    // 计算活三
                    if (line_length == 3) {
                        score += player * live_three_score;
                    }

                    // 计算活四
                    if (line_length == 4) {
                        score += player * live_four_score;
                    }
                }
            }
        }
    }

    return score;
}

//// 计算深度默认为4

//// (已删除)(先初始化：initNeighbourArray()，然后：)
//// 黑方评分调用：cb->alphaBeta(depth, -max_rating, max_rating, false);
//// 白方评分调用：cb->alphaBeta(depth, -max_rating, max_rating, true);
//// 调用：cb->alphaBeta(depth, -max_rating, max_rating, cb->moveCount() % 2)
//int chessboard::alphaBeta(/*int (*neighbour)[15], */int depth, int alpha, int beta, bool maximizingPlayer) {
////    qDebug() << "In file chessboard.cpp: alphaBeta: " << QString(" ").repeated(depth) << "end depth: " << depth;
//    if (depth == 0 || isOver) {
//        return evaluate();
//    }

//    // 在调用之前使用这些代码，初始化一个neighbor数组，大量减少重复初始化，整理成initNeighbourArray()
//////    int neighbour[15][15];
//////    // 专门用于alpha beta剪枝算法，无需关注棋盘边缘地方，可能需要关注的地方是初始化。可以设定在棋盘中央9个格子设为可探索区域
//////    // 根据经验，一般只用考虑与已经落的棋子的切比雪夫距离为3的格子，可能可以减少计算消耗（仅仅在调用alpha beta剪枝算法的时候才初始化，
//////    // 否则在落子的时候添加，在删除手顺的时候删除标记 非常麻烦

//////    /*
//////     * x x x x x x x
//////     * x x x x x x x
//////     * x x x x x x x
//////     * x x x o x x x
//////     * x x x x x x x
//////     * x x x x x x x
//////     * x x x x x x x
//////     */

////    for (int i = 0; i < 15; ++ i)
////        for (int j = 0; j < 15; ++ j)
////            neighbour[i][j] = 0;

////    // neighbour为0：不能放子或者不考虑，为1：考虑近邻
////    for (auto move : moveList) {
////        for (int i = 0; i < 7; ++ i) {
////            for (int j = 0; j < 7; ++ j) {
////                if (isReadyToAddMove(move.first - 3 + i, move.second - 3 + j)) {
////                    neighbour[move.first - 3 + i][move.second - 3 + j] = 1;
////                }
////            }
////        }
////    }


//    if (maximizingPlayer) {
//        int maxEval = - max_rating;
//        for (int row = 0; row < 15; ++row) {
//            for (int col = 0; col < 15; ++col) {
//                if (isReadyToAddMove(row, col)) {

////                // 改进：只考虑近邻手顺
////                if (neighbour[row][col]) {
//                    addMove(row, col);
////                    int neighbourExtend[15][15];
////                    initNeighbourArray(neighbourExtend);
//                    int eval = alphaBeta(/*neighbourExtend, */depth - 1, alpha, beta, false);
//                    delMove();
//                    maxEval = std::max(maxEval, eval);
//                    alpha = std::max(alpha, eval);
//                    // beta cut-off
//                    if (beta <= alpha)
//                        break;
//                }
//            }
//            // beta cut-off
//            if (beta <= alpha)
//                break;
//        }
//        return maxEval;
//    }
//    else {
//        int minEval = max_rating;
//        for (int row = 0; row < 15; ++row) {
//            for (int col = 0; col < 15; ++col) {
//                if (isReadyToAddMove(row, col)) {
////                if (neighbour[row][col]) {
//                    addMove(row, col);
////                    int neighbourExtend[15][15];
////                    initNeighbourArray(neighbourExtend);
//                    int eval = alphaBeta(/*neighbourExtend, */depth - 1, alpha, beta, true);
//                    delMove();
//                    minEval = std::min(minEval, eval);
//                    beta = std::min(beta, eval);

//                    // alpha cut-off
//                    if (beta <= alpha)
//                        break;
//                }
//            }

//            // alpha cut-off
//            if (beta <= alpha)
//                break;
//        }
//        return minEval;
//    }
////    qDebug() << "In file chessboard.cpp: alphaBeta: " << QString(" ").repeated(depth) << "end depth: " << depth;
//}


int chessboard::alphaBeta(/*int (*neighbour)[15], */int depth, int alpha, int beta, bool maximizingPlayer) {
    if (depth == 0 || isOver) {
        return evaluate();
    }

    if (maximizingPlayer) {
        int maxEval = - max_rating;
        for (int row = 0; row < 15; ++row) {
            for (int col = 0; col < 15; ++col) {
                if (isReadyToAddMove(row, col)) {
                    addMove(row, col);
                    int eval = alphaBeta(depth - 1, alpha, beta, false);
                    delMove();
                    maxEval = std::max(maxEval, eval);
                    alpha = std::max(alpha, eval);
                    // beta cut-off
                    if (beta <= alpha)
                        break;
                }
            }
            // beta cut-off
            if (beta <= alpha)
                break;
        }
        return maxEval;
    }
    else {
        int minEval = max_rating;
        for (int row = 0; row < 15; ++row) {
            for (int col = 0; col < 15; ++col) {
                if (isReadyToAddMove(row, col)) {
                    addMove(row, col);
                    int eval = alphaBeta(depth - 1, alpha, beta, true);
                    delMove();
                    minEval = std::min(minEval, eval);
                    beta = std::min(beta, eval);

                    // alpha cut-off
                    if (beta <= alpha)
                        break;
                }
            }

            // alpha cut-off
            if (beta <= alpha)
                break;
        }
        return minEval;
    }
}

GoBangAI::point* chessboard::getAI(int depthcfg/* = 4*/)
{
    static int x = 0;
    if (!x)
        initZobrist();

    ++ x;

    // 初始化steps
    std::vector<GoBangAI::point> steps = std::vector<GoBangAI::point>();

    bool isBlack = true;

    for (auto i : moveList) {
        steps.push_back({isBlack, i.first + 1, i.second + 1}); // 这里加一个参数-INF？
        isBlack = !isBlack;
    }

//    for (auto i : steps) {
//        qDebug() << (i.color ? "true" : "false") << ", " << i.x << ", " << i.y << ", " << i.score;
//    }

    GoBangAI::point aistep = {isBlack, 0, 0, - max_rating};

    //IDA*（迭代加深）优化，从底层开始搜，如果有杀棋则提前返回
    for(int i = 2; i <= depthcfg; i += 2){
        auto ai = new GoBangAI(steps, isBlack, i);
        auto tmpstep = ai->getStep();

        //更新aistep
        if(tmpstep.score > aistep.score)
            aistep = tmpstep;

        delete ai;

        //如果有杀棋
        if(aistep.score > 1440)
            break;
    }

    return new GoBangAI::point(aistep.isBlack, aistep.x - 1, aistep.y - 1, aistep.score);
}
