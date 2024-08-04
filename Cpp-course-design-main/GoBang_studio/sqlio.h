#ifndef SQLIO_H
#define SQLIO_H

#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>
#include "userlist.h"

extern QSqlDatabase db;

class sqlIO
{
public:
    sqlIO();

    static bool init_sql();

    static userlist& get_userlist_sql();

    static userlist::usernode* get_usernode_by_username_sql(QString username);

    static userlist::usernode* get_usernode_by_email_sql(QString email);

    static bool add_user_sql(QString username, QString pwd, QString email, QString avatar_url);

    static bool modify_user_sql(userlist::usernode* new_info);
};

#endif // SQLIO_H
