#include "guidewidget.h"
#include "ui_guidewidget.h"

guideWidget::guideWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::guideWidget)
{
    ui->setupUi(this);
    this->setStyleSheet("background-color: #F5F5F5;");
}

guideWidget::~guideWidget()
{
    delete ui;
}

void guideWidget::on_returnBtn_clicked()
{
    this->close();
}
