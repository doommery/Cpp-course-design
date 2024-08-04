#ifndef INPUTCHECKER_H
#define INPUTCHECKER_H

#include <QString>
#include <QRegExp>
#include <QStringList>
#include "ctype.h"

#define NOT_AVAILABLE 0
#define AVAILABLE 1

class inputchecker
{
private:
    static int inDigit_Alpha(QString s);
    static int inEmail(QString s);

public:
    inputchecker();

    // login界面
    static int check_login_loginBtn(QString username, QString pwd);

    // signup 界面
    static int check_signup_codeBtn(QString email);
    static int check_signup_createBtn(QString username, QString pwd, QString repwd, QString email);

    // forgetpwd 界面
    static int check_forgetpwd_submitButton(QString username, QString email);

    // settingsWidget 界面
    static int check_settingsWidget_modifyBtn(QString newPwd);
};

#endif // INPUTCHECKER_H
