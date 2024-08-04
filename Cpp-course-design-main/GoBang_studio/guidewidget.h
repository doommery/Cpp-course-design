#ifndef GUIDEWIDGET_H
#define GUIDEWIDGET_H

#include <QWidget>

namespace Ui {
class guideWidget;
}

class guideWidget : public QWidget
{
    Q_OBJECT

public:
    explicit guideWidget(QWidget *parent = nullptr);
    ~guideWidget();

private slots:
    void on_returnBtn_clicked();

private:
    Ui::guideWidget *ui;
};

#endif // GUIDEWIDGET_H
