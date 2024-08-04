#include "login.h"
#include "sqlio.h"

#include <QApplication>
#include <QMessageBox>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 连接数据库
    if (!(sqlIO::init_sql())){
        qDebug() << "error open database because"<< db.lastError().text();
        QMessageBox::information(0, "连接提示", "连接数据库失败，系统即将退出！");
        exit(1);
    }

    login w;
    w.show();
    return a.exec();
}
