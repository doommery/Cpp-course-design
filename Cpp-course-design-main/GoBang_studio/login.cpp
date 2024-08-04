#include "login.h"
#include "ui_login.h"
#include "menu.h"
#include "sqlio.h"
#include "signup.h"
#include "forgetpwd.h"
#include "inputchecker.h"

login::login(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::login)
{
    ui->setupUi(this);
}

login::~login()
{
    delete ui;
}

void login::on_loginBtn_clicked()
{
    // 登录
    QString username, pwd;

    if(ui->usernameEdit->text().isEmpty()){
        ui->usernameEdit->setFocus();
        return;
    }
    if(ui->pwdEdit->text().isEmpty()){
        ui->pwdEdit->setFocus();
        return;
    }

    username = ui->usernameEdit->text().toUtf8().data();
    pwd = ui->pwdEdit->text().toUtf8().data();

    if (!inputchecker::check_login_loginBtn(username, pwd)) {
        QMessageBox::information(this, "输入提示", "您的输入不合法，用户名、密码只能输入A~Z,a~z,0~9。");
        return;
    }

    // 整个数据库
    userlist::usernode* current_user = sqlIO::get_usernode_by_username_sql(username);
    if (current_user) {
        if (current_user->pwd != pwd) {
            QMessageBox::information(this, "登陆提示", "密码错误");
            return;
        }
        else{
            QMessageBox::information(this, "登陆提示", "登陆成功！");
            this->close();
            menu* p = new menu;
            p->set_current_user(current_user);
            p->show();
            return;
        }
    }
    else {
        QMessageBox::information(this, "登陆提示", "新账号请先注册！");
        return;
    }
}

void login::on_signupBtn_clicked()
{
    // 注册
    this->close();
    signUp* p = new signUp;
    p->show();
}

void login::on_findpwdBtn_clicked()
{
    // 找回密码
    this->close();
    forgetpwd* p = new forgetpwd;
    p->show();
}

void login::on_showpwdBox_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked) {
        // 当复选框被勾选，设置文本框为正常文本模式
        ui->pwdEdit->setEchoMode(QLineEdit::Normal);
    } else {
        // 当复选框未勾选，设置文本框为密码模式
        ui->pwdEdit->setEchoMode(QLineEdit::Password);
    }
}
