#ifndef MENU_H
#define MENU_H

#include <QWidget>
#include "userlist.h"

namespace Ui {
class menu;
}

class menu : public QWidget
{
    Q_OBJECT

public:
    explicit menu(QWidget *parent = nullptr);
    ~menu();

    void set_current_user(userlist::usernode* current_user);

private slots:
    void on_guideBtn_clicked();

    void on_settingsBtn_clicked();

    void on_practiceBtn_clicked();

    void on_duelBtn_clicked();

    void on_computerBtn_clicked();

    void on_quitBtn_clicked();

    void on_reloginBtn_clicked();

private:
    Ui::menu *ui;
    userlist::usernode* current_user;
};

#endif // MENU_H
