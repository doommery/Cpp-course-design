#ifndef DUELWIDGET_H
#define DUELWIDGET_H

#include <QWidget>
#include <QString>
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QMediaPlayer>
#include <QCoreApplication>
#include <QDir>
#include <QTimer>
#include <cmath>
#include "menu.h"
#include "chessboard.h"

namespace Ui {
class duelWidget;
}

class duelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit duelWidget(menu* menuptr, QWidget *parent = nullptr);
    ~duelWidget();

private slots:
    void on_returnBtn_clicked();

    void paintEvent(QPaintEvent *event);

    void mouseMoveEvent(QMouseEvent* event);

    void mousePressEvent(QMouseEvent *event);

    void closeEvent(QCloseEvent *event);

    void on_openBtn_clicked();

    void on_saveBtn_clicked();

    void on_tableWidget_cellClicked(int row, int column);

    void on_forwardBtn_clicked();

    void on_superForwardBtn_clicked();

    void on_backwardBtn_clicked();

    void on_superBackwardBtn_clicked();

    void on_playBtn_clicked();

    void on_startBtn_clicked();

    void on_regretBtn_clicked();

private:
    Ui::duelWidget *ui;
    menu* menuptr;
    int click_position_row, click_position_col;
    bool is_to_click; // 可点击，点击后落子
    chessboard* cb;
    int show_piece_num; // 展示多少个棋子，一般全展示（cb->moveCount()），点击tablewidget中除了最后一步的时候显示到这一步之前，点击下面的<和>也可调整聚焦的行
    QTimer* timer;

    // 重写close后需要用的
    bool need_to_save; // （没有导入文件）落子  或者 导入文件后，添加落子  后会变成true
    // 或者退出时需要保存的时候根据这个判断，

    bool closing;  // 用于跟踪是否正在关闭中
};

#endif // DUELWIDGET_H
