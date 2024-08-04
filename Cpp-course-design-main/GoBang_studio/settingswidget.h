#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>
#include <QFileDialog>
#include <QPixmap>
#include "userlist.h"
#include "menu.h"

namespace Ui {
class settingsWidget;
}

class settingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit settingsWidget(userlist::usernode* current_user, menu* menu_ptr, QWidget *parent = nullptr);
    ~settingsWidget();

private slots:
    void on_returnBtn_clicked();

    void on_showPwdCheckBox_stateChanged(int arg1);

    void on_pickAvatarBtn_clicked();

    void on_modifyBtn_clicked();

private:
    Ui::settingsWidget *ui;
    userlist::usernode* current_user;
    menu* menu_ptr;
};

#endif // SETTINGSWIDGET_H
