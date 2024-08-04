#include "duelwidget.h"
#include "ui_duelwidget.h"
#include "fileio.h"
#include <QDateTime>

duelWidget::duelWidget(menu* menuptr, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::duelWidget)
{
    ui->setupUi(this);

    this->menuptr = menuptr;

    need_to_save = false;

    closing = false;  // 用于跟踪是否正在关闭中

    cb = new chessboard;

    show_piece_num = 0;

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &duelWidget::on_forwardBtn_clicked);
}

duelWidget::~duelWidget()
{
    delete ui;
}

void duelWidget::on_returnBtn_clicked()
{
    this->closeEvent(new QCloseEvent);
}

void duelWidget::paintEvent(QPaintEvent *event) {
    // 绘制棋盘
    QPainter painter(this);
    // 抗锯齿
    painter.setRenderHint(QPainter::Antialiasing, true);

    // 15 * 15 网格
    for (int i = 0; i < 15; i ++ ) {
        // 竖线
        painter.drawLine(W_margin + i * block_size, H_margin,
                         H_margin + i * block_size, H_margin + 14 * block_size);

        // 横线
        painter.drawLine(W_margin, H_margin + i * block_size,
                         W_margin + 14 * block_size, H_margin + i * block_size);
    }

    if (is_to_click) {
        // 画上模糊辅助点
        painter.setRenderHint(QPainter::Antialiasing, true);

        if (show_piece_num % 2 == 0)
            painter.setBrush(Qt::black);
        else
            painter.setBrush(Qt::white);

        // 绘画提示落子的圆圈
        painter.drawEllipse(QPointF(W_margin + click_position_col * block_size,
                                    H_margin + click_position_row * block_size),
                            prompt_radius, prompt_radius);
    }

    // 绘画棋子
    for (int i = 0; i < show_piece_num; i ++ ) {
        if (i % 2 == 0)
            painter.setBrush(Qt::black);
        else
            painter.setBrush(Qt::white);

        // 聚焦最后一步显示的棋子，多画一个圈
        if (i == show_piece_num - 1) {
            painter.drawEllipse(QPointF(W_margin + cb->getIthMove(i).second * block_size,
                                        H_margin + cb->getIthMove(i).first * block_size),
                                chesspiece_radius * 1.2, chesspiece_radius * 1.2);
        }
        else {
            painter.drawEllipse(QPointF(W_margin + cb->getIthMove(i).second * block_size,
                                        H_margin + cb->getIthMove(i).first * block_size),
                                chesspiece_radius, chesspiece_radius);
        }
    }
}

void duelWidget::mouseMoveEvent(QMouseEvent* event)
{
    // 人机对战时，回看前几步的时候，不能落子（和练习模式不同）
    if (cb->moveCount() != show_piece_num) {
        is_to_click = false;
        return;
    }

    // 捕捉鼠标位置信息，提示是否可以落子
    int x = event->x(); // 对应第几列 col: 'A' - 'O'
    int y = event->y(); // 对应第几行 row: '1' - '15'

    if (x >= W_margin - block_size / 2 && x <= W_margin + 14 * block_size + block_size / 2 &&
        y >= H_margin - block_size / 2 && y <= H_margin + 14 * block_size + block_size / 2) {
        // 在这个方形区域里面看看是否能落子，第一次缩小范围

        // 考虑每个落子点作为一个方形的中心，通过除法计算出来这个方形区域中和他最近的落子点，round() 四舍五入函数
        click_position_col = round((double)(x - W_margin) / (double)block_size); // 0 - 14
        click_position_row = round((double)(y - H_margin) / (double)block_size); // 0 - 14

        if (show_piece_num == cb->moveCount()) {
            is_to_click = ((x - (W_margin + click_position_col * block_size)) * (x - (W_margin + click_position_col * block_size)) +
                           (y - (H_margin + click_position_row * block_size)) * (x - (H_margin + click_position_row * block_size)) -
                           pos_offset * pos_offset <= 0) &&
                           cb->isReadyToAddMove(click_position_row, click_position_col);
        }
        else {
            is_to_click = ((x - (W_margin + click_position_col * block_size)) * (x - (W_margin + click_position_col * block_size)) +
                           (y - (H_margin + click_position_row * block_size)) * (x - (H_margin + click_position_row * block_size)) -
                           pos_offset * pos_offset <= 0) &&
                           cb->isReadyToAddMoveAfterIthMove(click_position_row, click_position_col, show_piece_num);
        }



//        qDebug() << "click_position_row: " << click_position_row << " || click_position_col" <<  click_position_col << endl
//                 << ((is_to_click == true) ? "is to click" : "is not to click");
    }

    // 更新显示
     update();
}


void duelWidget::mousePressEvent(QMouseEvent *event)
{

    // 鼠标左键，点击落子
    qDebug() << "In file computerWidget.cpp: mousePressEvent(): pressing mouse at position" << event->x() << ", " << event->y() << "\n";
    if (!is_to_click) {
        // 鼠标没有落在模糊落子范围内或者此处有棋子，返回
        qDebug() << "In file computerWidget.cpp: mousePressEvent(): is_to_click: false\n";
        qDebug() << "In file computerWidget.cpp: mousePressEvent(): process returned.\n";
        return;
    }

    // 鼠标落在模糊落子范围内，而且此处没有棋子，准备落子
    qDebug() << "In file computerWidget.cpp: mousePressEvent(): is_to_click: true\n";


//    if (show_piece_num == cb->moveCount()) {
//        // 没有聚焦到除了最后一步以外的手顺，正常添加手顺
//        qDebug() << "In file computerWidget.cpp: mousePressEvent(): show_piece_num == cb->moveCount(): true\n";
//    }
//    else {
//        qDebug() << "In file computerWidget.cpp: mousePressEvent(): show_piece_num == cb->moveCount(): false\n";

//        // 截取前show_piece_num个步数，接着下
//        cb = cb->getIthSlice(show_piece_num);
//    }

    // 再检查是否游戏结束
    if(cb->gameIsOver()) {
        QMessageBox::information(this, "游戏提示", "已经结束嘞！");
        qDebug() << "In file computerWidget.cpp: mousePressEvent(): gameIsOver: false\n";
        qDebug() << "In file computerWidget.cpp: mousePressEvent(): process returned.\n";
        return;
    }

    qDebug() << "In file computerWidget.cpp: mousePressEvent(): gameIsOver: true\n";

    // 落子：1更新cb；2更新tablewidget，加上一行
    need_to_save = true;

    QString currentRow = QString(click_position_row + 'A');
    QString currentCol = QString::number(click_position_col + 1);
    QString currentMove = currentRow + currentCol;

    cb->addMove(click_position_row, click_position_col);

    ui->tableWidget->setRowCount(cb->moveCount());

    show_piece_num = cb->moveCount();

    qDebug() << "In file computerWidget.cpp: mousePressEvent(): addMove: "<< currentMove <<"\n";

    QTableWidgetItem *item = new QTableWidgetItem(currentMove);
    if (cb->moveCount() % 2 == 1) {
        // 奇数行：黑棋
        item->setBackground(Qt::black);
        item->setForeground(Qt::white);
    }
    else {
        // 偶数行：白棋
        item->setBackground(Qt::white);
        item->setForeground(Qt::black);
    }

    ui->tableWidget->setItem(cb->moveCount() - 1, 0, item);

    // 清除当前选择，使得右方表格选中最下面那个格子
    ui->tableWidget->selectionModel()->clearSelection();
    ui->tableWidget->selectionModel()->select(ui->tableWidget->model()->index(cb->moveCount() - 1, 0),
                                             QItemSelectionModel::Select | QItemSelectionModel::Rows);

    // 播放落子音乐
    QMediaPlayer* player = new QMediaPlayer;
    player->setMedia(QUrl("qrc:/sounds/press.mp3"));
    player->play();

//    qDebug() << "In file computerWidget.cpp: mousePressEvent(): process returned.\n";
    // 加上棋子：黑棋or白棋

    repaint();
//    update();

    if (cb->gameIsOver()) {
        QString text = "游戏结束，";
        if (cb->gameWinner() == 1) {
            // 黑胜
            text += "黑方胜利！";
        }
        else if (cb->gameWinner() == -1){
            // 白胜
            text += "白方胜利！";
        }
        else {
            // 平手
            text += "平局。";
        }
        QMessageBox::information(this, "游戏提示", text);
    }
}


void duelWidget::closeEvent(QCloseEvent *event)
{
    if (!closing) {
        if (need_to_save) {
            QMessageBox::StandardButton resBtn = QMessageBox::question(this, "保存棋谱提示",
                                                                        "棋谱未保存，是否离开？",
                                                                        QMessageBox::No | QMessageBox::Yes,
                                                                        QMessageBox::Yes);
            if (resBtn == QMessageBox::Yes) {
                closing = true;  // 设置标志变量为 true
                // 确认关闭
                event->accept();
                close();  // 关闭窗口
                qDebug() << "event->accept()";
                menuptr->show();
            }
            else {
                event->ignore();
                qDebug() << "event->ignore()";
                return;
            }
        }
        else {
            // 确认关闭
            event->accept();
            close();  // 关闭窗口
            qDebug() << "event->accept()";
            menuptr->show();
        }
    }
    else {
        event->accept();  // 已经在关闭中，接受关闭事件
    }
}

void duelWidget::on_openBtn_clicked()
{
    if (need_to_save) {
        QMessageBox::StandardButton resBtn = QMessageBox::question(this, "保存棋谱提示",
                                                                    "棋谱未保存，是否打开新棋谱？",
                                                                    QMessageBox::No | QMessageBox::Yes,
                                                                    QMessageBox::Yes);
        if (resBtn == QMessageBox::No) {
            return;
        }
    }

    // 读取棋谱
    // 获取应用程序所在目录，并附加子目录 "local"
    QString initialPath = QDir(QCoreApplication::applicationDirPath()).filePath("local");

    // 检查 "local" 文件夹是否存在，如果不存在，则创建
    QDir dir(initialPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString fileName = QFileDialog::getOpenFileName(
        this,
        "选择文件",
        initialPath,
        "Text files (*.txt)");

    // 检查用户是否真的选择了一个文件
    if (fileName.isEmpty()) {
        QMessageBox::information(this, "文件提示", "未选中任何棋谱");
        return;
    }

    std::vector<std::pair<short, short>> newMoveList = std::vector<std::pair<short, short>>();

    fileIO* file_IO = new fileIO;
    file_IO->readChessManual(fileName, newMoveList);

    qDebug() << "In file practicewidget.cpp: mousePressEvent(): end readChessManual\n";
    cb = new chessboard(newMoveList);
    qDebug() << "In file practicewidget.cpp: mousePressEvent(): end new chessboard\n";

    // 更新tablewidget
    int movelist_size = newMoveList.size();

    qDebug() << "In file practicewidget.cpp: mousePressEvent(): movelist_size: "<< movelist_size <<"\n";

    ui->tableWidget->setRowCount(movelist_size);

    QTableWidgetItem *item;
    for (int i = 0; i < movelist_size; i ++ ) {
        QString currentRow = QString(newMoveList[i].first + 'A');
        QString currentCol = QString::number(newMoveList[i].second + 1);
        QString currentMove = currentRow + currentCol;

        qDebug() << "In file practicewidget.cpp: mousePressEvent(): addMove: "<< currentMove <<"\n";

        item = new QTableWidgetItem(currentMove);
        if (i % 2 == 0) {
            // （对人来说）奇数行：黑棋
            item->setBackground(Qt::black);
            item->setForeground(Qt::white);
        }
        else {
            // 偶数行：白棋
            item->setBackground(Qt::white);
            item->setForeground(Qt::black);
        }

        ui->tableWidget->setItem(i, 0, item);
    }

    show_piece_num = cb->moveCount();

    update();

    QMessageBox::information(this, "导入棋谱", "导入棋谱完毕！");
}

void duelWidget::on_saveBtn_clicked()
{
    // 保存棋谱
    // 定义文件路径
    QString folderPath = QCoreApplication::applicationDirPath() + "/local";

    QString dateTime = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");

    QString filePath = folderPath + "/" + dateTime + "_duel.txt";

    // 检查 "local" 文件夹是否存在，如果不存在，则创建
    QDir dir;
    if (!dir.exists(folderPath)) {
        if (!dir.mkpath(folderPath)) {
            qDebug() << "无法创建文件夹:" << folderPath;
            QMessageBox::warning(this, "文件提示", "创建本地文件夹失败，请重试！");
            return;
        }
    }

    // 创建和写入文件
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "无法打开文件:" << filePath;
        return;
    }

    // 写入字符串
    fileIO fio;
    QString content = fio.getChessManual(cb);

    QTextStream out(&file);
    out << content;
    file.close();

    qDebug() << "文件已成功创建和写入:" << filePath;

    QMessageBox::warning(this, tr("文件提示"), tr("棋谱保存成功！"), QMessageBox::Yes);

    need_to_save = false;
}

void duelWidget::on_tableWidget_cellClicked(int row, int column)
{
    // 点击手顺表格格子
    // 显示到这一步的手顺的棋子
    show_piece_num = row + 1;

    // 更新显示
    update();

    // 和practice不同的是，practice中到达非最后一步，落子会删除后面的，新加一步，这里不行！
}

void duelWidget::on_forwardBtn_clicked()
{
    // 后一个手顺
    if (show_piece_num == cb->moveCount()) {
        timer->stop();
        return;
    }

    show_piece_num ++ ;
    // 清除当前选择，使得右方表格选中最下一个格子
    ui->tableWidget->selectionModel()->clearSelection();
    ui->tableWidget->selectionModel()->select(ui->tableWidget->model()->index(show_piece_num - 1, 0),
                                              QItemSelectionModel::Select | QItemSelectionModel::Rows);

    update();

    // 播放落子音乐
    QMediaPlayer* player = new QMediaPlayer;
    player->setMedia(QUrl("qrc:/sounds/press.mp3"));
    player->play();
}

void duelWidget::on_superForwardBtn_clicked()
{

    // 最后一个手顺
    if (show_piece_num == cb->moveCount()) {
        return;
    }

    show_piece_num = cb->moveCount();
    // 清除当前选择，使得右方表格选中最下一个格子
    ui->tableWidget->selectionModel()->clearSelection();
    ui->tableWidget->selectionModel()->select(ui->tableWidget->model()->index(show_piece_num - 1, 0),
                                              QItemSelectionModel::Select | QItemSelectionModel::Rows);

    update();

    // 播放落子音乐
    QMediaPlayer* player = new QMediaPlayer;
    player->setMedia(QUrl("qrc:/sounds/press.mp3"));
    player->play();
}

void duelWidget::on_backwardBtn_clicked()
{
    // 前一个手顺
    if (cb->moveCount() == 0 || show_piece_num == 0 || show_piece_num == 1) {
        return;
    }

    show_piece_num -- ;
    // 清除当前选择，使得右方表格选中最下一个格子
    ui->tableWidget->selectionModel()->clearSelection();
    ui->tableWidget->selectionModel()->select(ui->tableWidget->model()->index(show_piece_num - 1, 0),
                                              QItemSelectionModel::Select | QItemSelectionModel::Rows);

    update();
}

void duelWidget::on_superBackwardBtn_clicked()
{
    // 最前一个手顺
    if (cb->moveCount() == 0 || show_piece_num == 0 || show_piece_num == 1) {
        return;
    }

    show_piece_num = 1;
    // 清除当前选择，使得右方表格选中最下一个格子
    ui->tableWidget->selectionModel()->clearSelection();
    ui->tableWidget->selectionModel()->select(ui->tableWidget->model()->index(show_piece_num - 1, 0),
                                              QItemSelectionModel::Select | QItemSelectionModel::Rows);

    update();
}

void duelWidget::on_playBtn_clicked()
{
    // 从当前手顺开始播放
    ui->playBtn->setEnabled(false);
    qDebug() << "In file practicewidget.cpp: on_playBtn_clicked(): start playing\n";

    timer->start(1000); // 每秒触发一次

    ui->playBtn->setEnabled(true);
}

void duelWidget::on_regretBtn_clicked()
{
    // 悔棋一下子悔棋一步！
    if (cb->moveCount() == 0) {
        QMessageBox::information(this, "悔棋提示", "已经是最前局面，不能再悔棋！");
        return;
    }

    cb = cb->getIthSlice(cb->moveCount() - 1);

    show_piece_num = cb->moveCount();

    ui->tableWidget->setRowCount(show_piece_num);

    QMessageBox::information(this, "悔棋提示", "悔棋成功");

    update();
}

void duelWidget::on_startBtn_clicked()
{
    // 重新开始
    cb = new chessboard;
    show_piece_num = 0;
    ui->tableWidget->setRowCount(0);

    update();
}
