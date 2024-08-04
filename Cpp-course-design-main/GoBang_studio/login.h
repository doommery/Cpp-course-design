#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QString>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class login; }
QT_END_NAMESPACE

class login : public QWidget
{
    Q_OBJECT

public:
    login(QWidget *parent = nullptr);
    ~login();

private slots:
    void on_loginBtn_clicked();

    void on_signupBtn_clicked();

    void on_findpwdBtn_clicked();

    void on_showpwdBox_stateChanged(int arg1);

private:
    Ui::login *ui;
};
#endif // LOGIN_H
