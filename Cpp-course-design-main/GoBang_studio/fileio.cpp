#include "fileio.h"

fileIO::fileIO() {}

// 此处不加上const，因为要修改movelist
void fileIO::readChessManual(QString fileName, std::vector<std::pair<short, short>>& movelist)
{
    // 第一行：记录一共多少手顺，0-15*15
    // 第二行及以后：落子位置，行+空格+列，行：'A'-'O' 列：'1'-'15'
    movelist.clear();

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
       qDebug() << "Cannot open file for reading: " << file.errorString();
       return;
    }

    QTextStream in(&file);
//    while (!in.atEnd()) {
//       QString line = in.readLine();
//       qDebug() << line;
//    }

    QString line;
    int moveCount = 0;

    // 读取第一行，了解后续有多少行落子记录
    if (!in.atEnd()) {
        line = in.readLine();
        moveCount = line.toInt();
        qDebug() << "Total moves:" << moveCount;
    }

    // 读取每一行的落子位置
    while ((moveCount -- ) && !in.atEnd()) {
        line = in.readLine();
        QStringList parts = line.split(' ');
        if (parts.size() == 2) {
            int row = (parts[0].toStdString().c_str())[0] - 'A';
            int col = parts[1].toInt() - 1;
            movelist.push_back(std::pair<short, short>(row, col));
            qDebug() << "add move(row:" << row << ", column: " << col << ")\n";
        }
    }

    file.close();
}

QString fileIO::getChessManual(chessboard* cb)
{
    int moveCnt = cb->moveCount();

    QString res = QString::number(moveCnt) + "\n";

    for (int i = 0; i < moveCnt; i ++ ) {
        res += QString(cb->getIthMove(i).first + 'A') +
               " " +
               QString::number(cb->getIthMove(i).second + 1) +
               "\n";
    }

    return res;
}
