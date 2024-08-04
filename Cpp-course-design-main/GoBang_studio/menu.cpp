#include "menu.h"
#include "ui_menu.h"
#include "guidewidget.h"
#include "practicewidget.h"
#include "computerwidget.h"
#include "login.h"
#include "settingswidget.h"
#include "duelwidget.h"

menu::menu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::menu)
{
    ui->setupUi(this);
}

menu::~menu()
{
    delete ui;
}

void menu::set_current_user(userlist::usernode* current_user)
{
    this->current_user = current_user;
    QPixmap avatarPixmap(current_user->avatar_url);
    ui->avatarLabel->setScaledContents(true);
    ui->avatarLabel->setPixmap(current_user->avatar_url);

    ui->usernameLabel->setText(current_user->username);
}

void menu::on_guideBtn_clicked()
{
    // 新手教程
    guideWidget* p = new guideWidget(this);
    p->show();
}

void menu::on_settingsBtn_clicked()
{
    // 设置
    settingsWidget* p = new settingsWidget(current_user, this, this);
    p->show();
}

void menu::on_practiceBtn_clicked()
{
    // 练习模式
    this->hide();
    practiceWidget* p = new class practiceWidget(this);
    p->show();
}

void menu::on_duelBtn_clicked()
{
    // 双人模式
    this->hide();
    duelWidget* p = new class duelWidget(this);
    p->show();
}

void menu::on_computerBtn_clicked()
{
    // 人机对战
    this->hide();
    computerWidget* p = new class computerWidget(this);
    p->show();
}

void menu::on_quitBtn_clicked()
{
    this->close();
}

void menu::on_reloginBtn_clicked()
{
    QMessageBox::information(this, "登陆提示", "登出成功！请重新登录！");
    this->close();
    login* p = new login;
    p->show();
}
