#include "inputchecker.h"

inputchecker::inputchecker()
{

}


int inputchecker::inDigit_Alpha(QString s)
{
    // 数字+字母(+下划线)
    // 可修改为要求长度大于等于6位
    int size = s.size();

    int tag = AVAILABLE;
    for (int i = 0; i < size; i++)
    {
        if (/*s[i] == '_' || */
            (s[i] >= 'a' && s[i] <= 'z') ||
            (s[i] >= 'A' && s[i] <= 'Z') ||
            (s[i] >= '0' && s[i] <= '9'))
            continue;
        else
            return tag = NOT_AVAILABLE;
    }
    return tag = AVAILABLE;
}

int inputchecker::inEmail(QString s)
{
    // 邮箱
    int size = s.size();
    if (!(s[0] >= 'a' && s[0] <= 'z') && !(s[0] >= 'A' && s[0] <= 'Z') && !(s[0] >= '0' && s[0] <= '9')) {
        return 0;
    }
    int position = 0;
    for(int i = 1; i < size; ++ i)
    {
        if (!(s[i] >= 'a' && s[i] <= 'z') && !(s[i] >= 'A' && s[i] <= 'Z') && !(s[i] >= '0' && s[i] <= '9')) {
            if(s[i] == '@'){
                position = i + 1;
                break;
            }
            else{
                return 0;
            }
        }
    }
    if(!position){
        return 0;
    }
    QStringList SS = s.split("@");
    if(SS.size() != 2){
        return 0;
    }
    if(SS[1] != "126.com" && SS[1] != "163.com" && SS[1] != "qq.com") {
        return 0;
    }
    return 1;
}

// login界面
int inputchecker::check_login_loginBtn(QString username, QString pwd)
{
    return inDigit_Alpha(username) && inDigit_Alpha(pwd);
}

// signup 界面
int inputchecker::check_signup_codeBtn(QString email)
{
    return inEmail(email);
}

int inputchecker::check_signup_createBtn(QString username, QString pwd, QString repwd, QString email)
{
    return inDigit_Alpha(username) && inDigit_Alpha(pwd) && inDigit_Alpha(repwd) && inEmail(email);
}

// forgetpwd 界面
int inputchecker::check_forgetpwd_submitButton(QString username, QString email)
{
    return inDigit_Alpha(username) && inEmail(email);
}

// settingsWidget 界面
int inputchecker::check_settingsWidget_modifyBtn(QString newPwd)
{
    return inDigit_Alpha(newPwd);
}

