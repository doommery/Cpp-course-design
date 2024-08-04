#include "sqlio.h"

QSqlDatabase db;

sqlIO::sqlIO()
{

}

bool sqlIO::init_sql()
{
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");            //连接本地主机
    db.setDatabaseName("GoBang_studio_db"); //数据库名
    db.setPort(3306);                       //端口号
    db.setUserName("root");                 //用户
    db.setPassword("root");                 //密码
    return db.open();
}

userlist& sqlIO::get_userlist_sql()
{
    QSqlQuery Query;
    QString ExecSentence;

    ExecSentence  = "SELECT * FROM 1_userinfo;";

    userlist current_list;

    Query.exec(ExecSentence); // query里面已经包含了所有查询结果，现在要把他们全部拿出来

    QSqlRecord rec;
    while (Query.next()){
        // next函数特殊点在于，第一次调用next函数的时候返回第一个查找出的记录，此后都是返回下一个记录，不会有遗漏
        rec = Query.record();
        QString username = rec.value(0).toString();
        QString pwd = rec.value(1).toString();
        QString email = rec.value(2).toString();
        QString avatar_url = rec.value(3).toString();
        current_list.push_back(new userlist::usernode(username, pwd, email, avatar_url));
    };

    return current_list;
}

userlist::usernode* sqlIO::get_usernode_by_username_sql(QString username)
{
    QSqlQuery Query;
    QString ExecSentence;

    ExecSentence  = "SELECT * FROM 1_userinfo WHERE username = '";
    ExecSentence += username;
    ExecSentence += "';";

    userlist::usernode *recordToBeFound = nullptr;

    Query.exec(ExecSentence); // query里面已经包含了所有查询结果，现在要把他们全部拿出来

    if (Query.next()){
        // next函数特殊点在于，第一次调用next函数的时候返回第一个查找出的记录，此后都是返回下一个记录，不会有遗漏
        QSqlRecord rec = Query.record();
        QString username = rec.value(0).toString();
        QString pwd = rec.value(1).toString();
        QString email = rec.value(2).toString();
        QString avatar_url = rec.value(3).toString();
        recordToBeFound = new userlist::usernode(username, pwd, email, avatar_url);
    };

    return recordToBeFound;
}

userlist::usernode* sqlIO::get_usernode_by_email_sql(QString email)
{
    QSqlQuery Query;
    QString ExecSentence;

    ExecSentence  = "SELECT * FROM 1_userinfo WHERE email = '";
    ExecSentence += email;
    ExecSentence += "';";

    userlist::usernode *recordToBeFound = nullptr;

    Query.exec(ExecSentence); // query里面已经包含了所有查询结果，现在要把他们全部拿出来

    if (Query.next()){
        // next函数特殊点在于，第一次调用next函数的时候返回第一个查找出的记录，此后都是返回下一个记录，不会有遗漏
        QSqlRecord rec = Query.record();
        QString username = rec.value(0).toString();
        QString pwd = rec.value(1).toString();
        QString email = rec.value(2).toString();
        QString avatar_url = rec.value(3).toString();
        recordToBeFound = new userlist::usernode(username, pwd, email, avatar_url);
    };

    return recordToBeFound;
}

bool sqlIO::add_user_sql(QString username, QString pwd, QString email, QString avatar_url)
{
    QSqlQuery query;
    QString execSentence = "INSERT INTO 1_userinfo (username, pwd, email, avatar_url) "
                           "VALUES (:username, :pwd, :email, :avatar_url)";

    query.prepare(execSentence);
    query.bindValue(":username", username);
    query.bindValue(":pwd", pwd);
    query.bindValue(":email", email);
    query.bindValue(":avatar_url", avatar_url);

    if (query.exec()) {
        return true;
    } else {
        qDebug() << "Insert execution error:" << query.lastError().text();
        return false;
    }
}

bool sqlIO::modify_user_sql(userlist::usernode* new_info)
{
    // 修改用户信息
    // 构造SQL更新查询
    QString updateQuery = QString("UPDATE 1_userinfo SET pwd='%1', email='%2', avatar_url='%3' WHERE username='%4'")
                          .arg(new_info->pwd)
                          .arg(new_info->email)
                          .arg(new_info->avatar_url)
                          .arg(new_info->username);

    qDebug() << "更新查询: " << updateQuery;

    // 执行查询
    QSqlQuery query;
    if(query.exec(updateQuery)){
        qDebug() << "用户信息修改成功";
        return true;
    }
    else{
        qDebug() << "用户信息修改失败: " << query.lastError().text();
        return false;
    }
}

