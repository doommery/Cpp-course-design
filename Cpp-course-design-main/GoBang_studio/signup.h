#ifndef SIGNUP_H
#define SIGNUP_H

#include <QWidget>
#include <QFileDialog>
#include <QPixmap>

namespace Ui {
class signUp;
}

class signUp : public QWidget
{
    Q_OBJECT

public:
    explicit signUp(QWidget *parent = nullptr);
    ~signUp();

private slots:
    void on_createBtn_clicked();

    void on_returnBtn_clicked();

    void on_codeBtn_clicked();

    void on_pickAvatarBtn_clicked();

    void on_showPwdCheckBox_stateChanged(int arg1);

private:
    Ui::signUp *ui;
    QString identificationCode;
    QString targetEmail;
    QString avatar_url;
};

#endif // SIGNUP_H
