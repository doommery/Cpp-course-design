#include "forgetpwd.h"
#include "ui_forgetpwd.h"
#include "login.h"
#include "userlist.h"
#include "sqlio.h"
#include "tcpemail.h"
#include "inputchecker.h"

forgetpwd::forgetpwd(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::forgetpwd)
{
    ui->setupUi(this);
}

forgetpwd::~forgetpwd()
{
    delete ui;
}

void forgetpwd::on_backButton_clicked()
{
    this->close();
    login* p = new login;
    p->show();
}

void forgetpwd::on_submitButton_clicked()
{
    // 找回密码
    QString username = ui->usernameEdit->text();
    QString email = ui->emailEdit->text();
//    inputChecker inputchecker;
    if(username.isEmpty()){
        ui->usernameEdit->setFocus();
        return;
    }
    if(email.isEmpty()){
        ui->emailEdit->setFocus();
        return;
    }
    username = username.toUtf8().data();
    email = email.toUtf8().data();

    if (!inputchecker::check_forgetpwd_submitButton(username, email)) {
        QMessageBox::information(this, "输入提示", "您的输入不合法！");
        return;
    }

    userlist::usernode* tmp = sqlIO::get_usernode_by_username_sql(username);

    if(!tmp){
        QMessageBox::warning(NULL,QObject::tr("警告"),QObject::tr("该用户名不存在"),QMessageBox::Yes);
        return;
    }
    if(tmp->email != email){
        QMessageBox::warning(NULL,QObject::tr("警告"),QObject::tr("用户名或邮箱错误！"),QMessageBox::Yes);
        return;
    }
    free(tmp);

    tcp_pwd m(tmp->pwd, email);

    QMessageBox::warning(NULL,QObject::tr("警告"),QObject::tr("邮件已经发送，请及时查收。"),QMessageBox::Yes);
}
