#include "signup.h"
#include "ui_signup.h"
#include "login.h"
#include "userlist.h"
#include "sqlio.h"
#include "tcpemail.h"
#include "inputchecker.h"

signUp::signUp(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::signUp)
{
    ui->setupUi(this);
    avatar_url = "your\\file\\path\\to\\default_avatar.jpg"; // need completion
}

signUp::~signUp()
{
    delete ui;
}

void signUp::on_createBtn_clicked()
{
    // 注册
    QString username, pwd, repeatPwd, code;
    if(ui->usernameEdit->text().isEmpty()){
        ui->usernameEdit->setFocus();
        return;
    }
    if(ui->pwdEdit->text().isEmpty()){
        ui->pwdEdit->setFocus();
        return;
    }
    if(ui->repeatPwdEdit->text().isEmpty()){
        ui->repeatPwdEdit->setFocus();
        return;
    }
    if(ui->emailEdit->text().isEmpty()){
        ui->emailEdit->setFocus();
        return;
    }
    if(ui->codeEdit->text().isEmpty()){
        ui->codeEdit->setFocus();
        return;
    }

    username = ui->usernameEdit->text().toUtf8().data();
    pwd = ui->pwdEdit->text().toUtf8().data();
    repeatPwd = ui->repeatPwdEdit->text().toUtf8().data();

    if(!inputchecker::check_signup_createBtn(username, pwd, repeatPwd, targetEmail)){
        QMessageBox::warning(NULL,QObject::tr("警告"),QObject::tr("您的输入不合法，用户名、密码只能输入A~Z,a~z,0~9。"),QMessageBox::Yes);
        return;
    }

    if(pwd != repeatPwd){
        QMessageBox::warning(NULL,QObject::tr("警告"),QObject::tr("前后密码不一致，请重试。"),QMessageBox::Yes);
        return;
    }

    userlist::usernode* tmp = sqlIO::get_usernode_by_username_sql(username);

    if(tmp){
        QMessageBox::warning(NULL,QObject::tr("警告"),QObject::tr("该用户已经被注册，请更换您的用户名。"),QMessageBox::Yes);
        return;
    }

    if(ui->emailEdit->text().toUtf8() != targetEmail){
        QMessageBox::warning(NULL,QObject::tr("警告"),QObject::tr("您更改了要注册的邮箱，请重新获取对应的验证码。"),QMessageBox::Yes);
        return;
    }

    if(targetEmail == ""){
        QMessageBox::warning(NULL,QObject::tr("警告"),QObject::tr("您尚未获取验证码，请获取验证码后再注册。"),QMessageBox::Yes);
        return;
    }

    code = ui->codeEdit->text().toUtf8().data();

    if(code != identificationCode){
        QMessageBox::warning(NULL,QObject::tr("警告"),QObject::tr("您的验证码不正确，请重新输入。"),QMessageBox::Yes);
        ui->codeEdit->clear();
        return;
    }

    // 在数据库中加入记录
    if(!sqlIO::add_user_sql(username, pwd, targetEmail, avatar_url)){
        QMessageBox::warning(NULL,QObject::tr("警告"),QObject::tr("用户插入失败"),QMessageBox::Yes);
        return;
    }

    QMessageBox::warning(NULL,QObject::tr("恭喜"),QObject::tr("注册成功！"),QMessageBox::Yes);
    this->close();
    login* p = new login;
    p->show();
}

void signUp::on_returnBtn_clicked()
{
    // 返回到login
    this->close();
    login* p = new login;
    p->show();
}

void signUp::on_codeBtn_clicked()
{
    // 发送验证码
    if(ui->emailEdit->text().isEmpty()){
        ui->emailEdit->setFocus();
        QMessageBox::warning(NULL,QObject::tr("警告"),QObject::tr("请输入邮箱！"),QMessageBox::Yes);
        return;
    }

    targetEmail = ui->emailEdit->text().toUtf8().data();

    // 输入检查
    if(!inputchecker::check_signup_codeBtn(targetEmail)){
        QMessageBox::warning(NULL,QObject::tr("警告"),QObject::tr("您的输入不合法，或者不为支持的邮箱类型（仅支持qq/126/163）。"),QMessageBox::Yes);
        targetEmail = "";
        return;
    }

    if(sqlIO::get_usernode_by_email_sql(targetEmail)){
        QMessageBox::warning(NULL,QObject::tr("警告"),QObject::tr("该邮箱已经被注册，请勿重复注册。"),QMessageBox::Yes);
        targetEmail = "";
        return;
    }

    identificationCode = QUuid::createUuid().toString().remove("{").remove("}").remove("-").right(6);
    tcp_code m(identificationCode, targetEmail);
}

void signUp::on_pickAvatarBtn_clicked()
{
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
        avatar_url = fileName;
        qDebug() << "avatar_url: " << fileName;
        QPixmap avatarPixmap(fileName);
        ui->avatarLabel->setScaledContents(true);
        ui->avatarLabel->setPixmap(avatar_url);
    }
}

void signUp::on_showPwdCheckBox_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked) {
        // 当复选框被勾选，设置文本框为正常文本模式
        ui->pwdEdit->setEchoMode(QLineEdit::Normal);
        ui->repeatPwdEdit->setEchoMode(QLineEdit::Normal);
    } else {
        // 当复选框未勾选，设置文本框为密码模式
        ui->pwdEdit->setEchoMode(QLineEdit::Password);
        ui->repeatPwdEdit->setEchoMode(QLineEdit::Password);
    }
}
