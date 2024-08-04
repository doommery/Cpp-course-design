#include "settingswidget.h"
#include "ui_settingswidget.h"
#include "sqlio.h"
#include "login.h"
#include "inputchecker.h"

settingsWidget::settingsWidget(userlist::usernode* current_user, menu* menu_ptr, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::settingsWidget)
{
    ui->setupUi(this);
    this->current_user = current_user;
    this->menu_ptr = menu_ptr;

    this->setStyleSheet("background-color: #F5F5F5;");

    ui->usernameEdit->setText(current_user->username);
    ui->usernameEdit->setReadOnly(true);

    ui->pwdEdit->setText(current_user->pwd);
    ui->pwdEdit->setReadOnly(true);
    ui->pwdEdit->setEchoMode(QLineEdit::Password);

    ui->emailEdit->setText(current_user->email);
    ui->emailEdit->setReadOnly(true);

    QPixmap avatarPixmap(current_user->avatar_url);
    ui->avatarLabel->setScaledContents(true);
    ui->avatarLabel->setPixmap(current_user->avatar_url);
}

settingsWidget::~settingsWidget()
{
    delete ui;
}

void settingsWidget::on_returnBtn_clicked()
{
    this->close();
}

void settingsWidget::on_showPwdCheckBox_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked) {
        // 当复选框被勾选，设置文本框为正常文本模式
        ui->newPwdEdit->setEchoMode(QLineEdit::Normal);
        ui->repeatNewPwdEdit->setEchoMode(QLineEdit::Normal);
    }
    else {
        // 当复选框未勾选，设置文本框为密码模式
        ui->newPwdEdit->setEchoMode(QLineEdit::Password);
        ui->repeatNewPwdEdit->setEchoMode(QLineEdit::Password);
    }
}

void settingsWidget::on_pickAvatarBtn_clicked()
{
    // 更换头像
    // 选择头像
    // 获取应用程序所在目录，并附加子目录 "local"
    QString initialPath = QDir(QCoreApplication::applicationDirPath()).filePath("local");

    // 检查 "local" 文件夹是否存在，如果不存在，则创建
    QDir dir(initialPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString fileName = QFileDialog::getOpenFileName(
        this,
        "选择头像",
        initialPath,
        "Images (*.png *.xpm *.jpg *.jpeg *.bmp *.gif)");

    // 检查用户是否真的选择了一个文件
    if (fileName.isEmpty()) {
        QMessageBox::information(this, "文件提示", "未选中任何图片");
        return;
    }
    else {
        userlist::usernode* new_info = new userlist::usernode(current_user->username,
                                                              current_user->pwd,
                                                              current_user->email,
                                                              current_user->avatar_url);
        new_info->avatar_url = fileName;
        qDebug() << "avatar_url: " << fileName;

        // 在数据库中更改记录
        if(!sqlIO::modify_user_sql(new_info)){
            QMessageBox::warning(this, QObject::tr("警告"), QObject::tr("头像更改失败"), QMessageBox::Yes);
            return;
        }

        QPixmap avatarPixmap(fileName);
        ui->avatarLabel->setScaledContents(true);
        ui->avatarLabel->setPixmap(new_info->avatar_url);

        current_user = new_info;

        menu_ptr->set_current_user(current_user);

        QMessageBox::warning(this, QObject::tr("恭喜"),QObject::tr("头像更改成功！"), QMessageBox::Yes);
    }
}

void settingsWidget::on_modifyBtn_clicked()
{
    // 更改密码
    QString newPwd, repeatNewPwd;
    if(ui->newPwdEdit->text().isEmpty()){
        ui->newPwdEdit->setFocus();
        return;
    }
    if(ui->repeatNewPwdEdit->text().isEmpty()){
        ui->repeatNewPwdEdit->setFocus();
        return;
    }

    newPwd = ui->newPwdEdit->text().toUtf8().data();
    repeatNewPwd = ui->repeatNewPwdEdit->text().toUtf8().data();

    if (newPwd != repeatNewPwd) {
        QMessageBox::information(this, "修改密码提示", "新密码前后不一致，请重试！");
        return;
    }

    if (!inputchecker::check_settingsWidget_modifyBtn(newPwd)) {
        QMessageBox::information(this, "输入提示", "您的输入不合法，密码只能输入A~Z,a~z,0~9。");
        return;
    }

    userlist::usernode* new_info = new userlist::usernode(current_user->username,
                                                          newPwd,
                                                          current_user->email,
                                                          current_user->avatar_url);

    // 在数据库中更改记录
    if(!sqlIO::modify_user_sql(new_info)){
        QMessageBox::warning(this, QObject::tr("警告"), QObject::tr("密码更改失败，请重试！"), QMessageBox::Yes);
        return;
    }

    QMessageBox::information(this, QObject::tr("恭喜"),QObject::tr("密码更改成功，请重新登录！"), QMessageBox::Yes);

    menu_ptr->close();

    login* p = new login;
    p->show();
}
