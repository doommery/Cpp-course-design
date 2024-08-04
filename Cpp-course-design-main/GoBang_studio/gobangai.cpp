#include "gobangai.h"

// Key-val分值表
std::vector<Pattern> patterns = {
    { "11111",  50000 },
    { "011110", 4320 },
    { "011100", 720 },
    { "001110", 720 },
    { "011010", 720 },
    { "010110", 720 },
    { "11110",  720 },
    { "01111",  720 },
    { "11011",  720 },
    { "10111",  720 },
    { "11101",  720 },
    { "001100", 120 },
    { "001010", 120 },
    { "010100", 120 },
    { "000100", 20 },
    { "001000", 20 },
};

//黑、白棋的zobrist表，内存64bit随机数
ULL zobrist_black[N][N], zobrist_white[N][N];

//zobrist-list
std::unordered_map<ULL,int> zobrist_list;

//Ac自动机初始化
AcString* acs = new AcString(patterns);

//获取64bit随机数
ULL getRandom64() {
    return (ULL)rand() | ((ULL)rand() << 15) | ((ULL)rand() << 30) | ((ULL)rand() << 45) | ((ULL)rand() << 60);
}

void initZobrist()
{
    for(int i = 1; i <= M; ++ i)
        for(int j = 1; j <= M; ++ j){
            zobrist_black[i][j] = getRandom64();
            zobrist_white[i][j] = getRandom64();
        }
}

GoBangAI::GoBangAI(std::vector<point>& state, bool nextisBlack,int depthmax)
{
    aistep  = alphaBetaAlgorithm(state, nextisBlack, 0, -INF, INF, depthmax);
}

GoBangAI::~GoBangAI() {}

GoBangAI::point GoBangAI::getStep()
{
    return aistep;
}

int GoBangAI::evluateSituation(std::vector<point>& state, bool isBlack)
{
    //获取当前局面的key
    ULL nowkey = getZobristKey(state);

    //如果已有cache就直接返回
    if(zobrist_list.count(nowkey)) {
        return isBlack ? zobrist_list[nowkey] : - zobrist_list[nowkey];
    }

    //初始化棋盘记录数组g
    int nowisBlack = isBlack ? 1 : -1;
    int g[N][N];
    memset(g, 0, sizeof g);

    for (auto i : state)
        if (i.isBlack)
            g[i.x][i.y] = 1;
        else if (!i.isBlack)
            g[i.x][i.y] = -1;

    //对于新加入的点，先使用以新点为中心的8联通优化
    std::vector<point> befState;
    befState.assign(state.begin(),state.end() - 1);
    //获取未加入新点前的key
    ULL befkey = getZobristKey(befState);
    auto nowpoint = state.back();
    //如果为加入的点已被cache，就在其基础上仅改动其8联通方向的分值
    if(zobrist_list.count(befkey)){
        int befval = isBlack ? zobrist_list[befkey] : - zobrist_list[befkey];
        int x = nowpoint.x, y = nowpoint.y;
        int dir = -1;
        while ( ++ dir < 4) {
            int sign = -1;
            for (int i = 1; i < 8; ++i) {
                int tx = x + sign * i * dx[dir],
                    ty = y + sign * i * dy[dir];

                if (!isValidPosition(tx, ty))
                    break;

                if(g[tx][ty] == 0)
                    continue;

                if (g[tx][ty] == - nowisBlack)
                    befval=befval + getScoreInDir(!isBlack, tx, ty, befState, dir) - getScoreInDir(!isBlack, tx, ty, state, dir);

                if (g[tx][ty] == nowisBlack)
                    befval=befval - getScoreInDir(isBlack, tx, ty, befState, dir) + getScoreInDir(isBlack, tx, ty, state, dir);
            }
            sign = - sign;
            for (int i = 1; i < 8; ++ i) {
                int tx = x + sign * i * dx[dir],
                    ty = y + sign * i * dy[dir];

                if (!isValidPosition(tx, ty))
                    break;

                if(g[tx][ty] == 0)
                    continue;

                if (g[tx][ty] == - nowisBlack)
                    befval = befval + getScoreInDir(!isBlack, tx, ty, befState, dir) - getScoreInDir(!isBlack, tx, ty, state, dir);

                if (g[tx][ty] == nowisBlack)
                    befval = befval - getScoreInDir(isBlack, tx, ty, befState, dir) + getScoreInDir(isBlack, tx, ty, state, dir);
            }
        }
        befval += getScore(isBlack,nowpoint.x, nowpoint.y,g);
        if (isBlack)
            zobrist_list[nowkey] = befval;
        else{
            zobrist_list[nowkey] = -befval;
        }
        return befval;
    }

    //如果未记录，就评估去全局
    int pointBLK = 0, pointWIT =0;

    for (auto i : state) {
        if (i.isBlack == 1)
            pointBLK += getScore(1, i.x, i.y, g);
    }

    for (auto i : state) {
        if (i.isBlack == 0)
            pointWIT += getScore(0, i.x, i.y, g);
    }

    int val = 0;
    if (isBlack){
        val = pointBLK - pointWIT;
        zobrist_list[nowkey] = val;
    }
    else{
        val = pointWIT - pointBLK;
        zobrist_list[nowkey] = -val;
    }
    return val;
}

bool GoBangAI::isValidPosition(int x, int y)
{
    return !(x < 1 || x > M || y < 1 || y > M);
}

int GoBangAI::getScoreInDir(bool isBlack,int x,int y,std::vector<point> state,int dir)
{
    int g[N][N];
    memset(g, 0, sizeof g);
    for (auto i : state)
        if (i.isBlack)
            g[i.x][i.y] = 1;
        else if (!i.isBlack)
            g[i.x][i.y] = -1;

    int ans = 0;
    int nowisBlack = isBlack ? 1 : -1;
    std::deque<int> S;
    S.push_back(1);
    int sign = -1;
    //获取该点dir方向的棋子排布
    for (int i = 1; i < 4; ++i) {
        int tx = x + sign * i * dx[dir], ty = y + sign * i * dy[dir];
        if (!isValidPosition(tx, ty)){
            S.push_front(2);
            break;
        }
        if (g[tx][ty] == 0) {
            S.push_front(0);
        }
        if (g[tx][ty] == nowisBlack)
            S.push_front(1);
        if (g[tx][ty] == -nowisBlack) {
            S.push_front(2);
            break;
        }
    }
    sign = -sign;
    for (int i = 1; i <4; ++i) {
        int tx = x + sign * i * dx[dir], ty = y + sign * i * dy[dir];
        if (!isValidPosition(tx, ty)){
            S.push_back(2);
            break;
        }
        if (g[tx][ty] == 0) {
            S.push_back(0);
        }
        if (g[tx][ty] == nowisBlack)
            S.push_back(1);
        if (g[tx][ty] == -nowisBlack) {
            S.push_back(2);
            break;
        }
    }
    //转为String
    std::string s = "";
    for(auto i : S) {
        s += '0' + i;
    }
    //使用Ac自动机进行匹配
    std::vector<int> tmp = acs->ACSearch(s);
    for (int j = 0; j < (int)tmp.size(); ++ j) {
        ans += patterns[tmp[j]].score;
    }
    return ans;
}
int GoBangAI::getScore(bool isBlack, int x, int y, int g[][N])
{
    int ans = 0;
    int dir = -1;
    int nowisBlack = isBlack ? 1 : -1;

    //获取8联通方向的棋子排布
    while (++dir < 4) {
        std::deque<int> S;
        S.push_back(1);
        int sign = -1;
        for (int i = 1; i < 4; ++ i) {
            int tx = x + sign * i * dx[dir], ty = y + sign * i * dy[dir];
            if (!isValidPosition(tx, ty)){
                S.push_front(2);
                break;
            }
            if (g[tx][ty] == 0) {
                S.push_front(0);
            }
            if (g[tx][ty] == nowisBlack)
                S.push_front(1);
            if (g[tx][ty] == -nowisBlack) {
                S.push_front(2);
                break;
            }
        }
        sign = -sign;
        for (int i = 1; i <4; ++i) {
            int tx = x + sign * i * dx[dir], ty = y + sign * i * dy[dir];
            if (!isValidPosition(tx, ty)){
                S.push_back(2);
                break;
            }
            if (g[tx][ty] == 0) {
                S.push_back(0);
            }
            if (g[tx][ty] == nowisBlack)
                S.push_back(1);
            if (g[tx][ty] == -nowisBlack) {
                S.push_back(2);
                break;
            }
        }
        std::string s = "";
        for(auto i:S){
            s+='0'+i;
        }
        //Ac自动机匹配
        std::vector<int> tmp = acs->ACSearch(s);
        for (int j = 0; j < (int)tmp.size(); j++) {
            ans += patterns[tmp[j]].score;
        }
    }

    return ans;
}
std::stack<GoBangAI::point> GoBangAI::getPossibleNextStep(std::vector<point>& state, bool nowisBlack)
{
    const int dtx[8] = { -1, -1, -1, 0, 1, 1,  1,  0 };
    const int dty[8] = { -1,  0,  1, 1, 1, 0, -1, -1 };

    // A: 已经落下的棋子的位置和他们的邻居，用于防止重复添加以及防止添加已有落子位置
    // S: 评分+可落子位置
    std::set<PII> S, A;

    //将已有点存入集合A
    for (auto i : state)
        A.insert({ i.x, i.y });

    for (auto t : state) {
        for (int i = 0; i < 8; i++) {
            // (tx, ty): 落子点的周围8个格子
            int tx = t.x + dtx[i], ty = t.y + dty[i];

            if (!isValidPosition(tx, ty))
                continue;

            if (A.count({ tx, ty }))
                continue;

            A.insert({ tx, ty });

            // S插进去一个，让下一步的当前子的val尽可能的大
            // 探索下一步
            state.push_back({ nowisBlack, tx, ty });
            S.insert({ evluateSituation(state, nowisBlack), tx * N + ty });
            // 回溯到上一步，接着探索
            state.pop_back();
        }
    }

    // 将最后结果加入stack中（从小到大）
    std::stack<point> ans;
    for (auto i : S) {
        int x = i.second / N, y = i.second % N;
        ans.push({ nowisBlack, x, y });
    }
    return ans;
}
GoBangAI::point GoBangAI::alphaBetaAlgorithm(std::vector<point>& state, bool nowisBlack, int depth, int alpha, int beta, int maxdepth)
{
    //到达底层返回
    if(depth == maxdepth){
        point nextpt = state[state.size() - maxdepth];
        nextpt.score = evluateSituation(state, nextpt.isBlack);
        return nextpt;
    }

    //获取最优点
    std::stack<point> nextPossibleStep = getPossibleNextStep(state, nowisBlack);

    point maxV = {nowisBlack, -1, -1, -INF}, minV= {nowisBlack, -1, -1, INF};
    int nowval = evluateSituation(state, nowisBlack);
    int cnt = 0;
    while(nextPossibleStep.size()){
        // 此处可以限制思考局面个数，此处为7
        if(++ cnt > 100 && maxdepth > 4)
            break;

        point tpstep = nextPossibleStep.top();
        nextPossibleStep.pop();

        //-----
        state.push_back(tpstep);

        //更新zobrist_list，保存已经搜索过的局面
        int tkey = getZobristKey(state);

        if(!zobrist_list.count(tkey)) {
            if(nowisBlack)
                zobrist_list[tkey] = nowval ^ zobrist_black[tpstep.x][tpstep.y];
            else
                zobrist_list[tkey] = nowval ^ zobrist_white[tpstep.x][tpstep.y];
        }

        point val = alphaBetaAlgorithm(state, !nowisBlack, depth + 1, alpha, beta, maxdepth);
        state.pop_back();
        //-----

        // Alpha-Beta剪枝
//        if (depth % 2 == 0){
        if (!nowisBlack) {
            // 对应Max层
            if(val.score > maxV.score) {
                maxV = val;
            }

            alpha = std::max(alpha, maxV.score);
        }
        else{
            // 对应Min层
            if(val.score < minV.score) {
                minV = val;
            }

            beta = std::min(beta, minV.score);
        }

        //alpha >= beta进行剪枝操作
        if(alpha >= beta)
            break;
    }

    // max层返回maxv，min层返回minv
    if (!nowisBlack)
        return maxV;
    else
        return minV;
}

// 获取state局面的ZobristKey
ULL GoBangAI::getZobristKey(std::vector<GoBangAI::point> state)
{
    ULL key = 0;

    for(auto i : state){
        if(i.isBlack){
            key = key ^ zobrist_black[i.x][i.y];
        }
        else{
            key = key ^ zobrist_white[i.x][i.y];
        }
    }

    return key;
}

