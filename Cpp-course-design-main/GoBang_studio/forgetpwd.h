#ifndef FORGETPWD_H
#define FORGETPWD_H

#include <QWidget>

namespace Ui {
class forgetpwd;
}

class forgetpwd : public QWidget
{
    Q_OBJECT

public:
    explicit forgetpwd(QWidget *parent = nullptr);
    ~forgetpwd();

private slots:
    void on_backButton_clicked();

    void on_submitButton_clicked();

private:
    Ui::forgetpwd *ui;
};

#endif // FORGETPWD_H
