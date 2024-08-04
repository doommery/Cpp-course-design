#ifndef FILEIO_H
#define FILEIO_H

#include <vector>
#include <utility>
#include <QString>
#include <QDebug>
#include <algorithm>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QDebug>
#include "chessboard.h"
// 读取棋谱，保存棋谱


class fileIO
{
public:
    fileIO();

    void readChessManual(QString fileName, std::vector<std::pair<short, short>>& movelist);

    QString getChessManual(chessboard* cb);
};

#endif // FILEIO_H
