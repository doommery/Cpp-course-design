#ifndef TCPEMAIL_H
#define TCPEMAIL_H

#include <QtCore>
#include <QObject>
#include <QCoreApplication>
#include <QTcpSocket>
#include <QString>
#include <QTextStream>
#include <QDebug>
#include <QAbstractSocket>
#include <QDateTime>
#include <QDate>
#include <QLocale>
#include <QMessageBox>

class tcp
{
protected:
    QTcpSocket* tcpSocket;
    QString email,
            title,
            content,
            annotation,
            senderEmail,
            senderKey;

public:
    tcp(QString email);

    virtual void setTitle() = 0;

    virtual void setContent() = 0;

    virtual void setAnnotation() = 0;

    int ReadFromHost(QString ExpectedReplyFromHost);

    void CheckConnectState();

    void SendHeloSis();

    void SendAuthLogin();

    void SendMyEmailAddress();

    void SendAuthCode();

    void SendEmailFrom();

    void SendRcptTo();

    void StartSendContent();

    void SendContent();

    void SendingStateConfirm();
};

// 发送验证码
class tcp_code : tcp
{
private:
    QString code;

public:
    tcp_code(QString code, QString email);

    void setTitle();

    void setContent();

    void setAnnotation();
};

// 找回密码
class tcp_pwd : tcp
{
private:
    QString pwd;

public:
    tcp_pwd(QString pwd, QString email);

    void setTitle();

    void setContent();

    void setAnnotation();
};

#endif // TCPEMAIL_H
