#include "tcpemail.h"

tcp::tcp(QString email)
{
    tcpSocket = new QTcpSocket();
    this->email = email;
    senderEmail = QString("***yourEmail***@qq.com"); // need completion
    senderKey = QString("***yourCorrespondingKey***"); // need completion
}

int tcp::ReadFromHost(QString ExpectedReplyFromHost)
{
    tcpSocket->waitForReadyRead(5000);
    QString buffer = tcpSocket->readAll();
    qDebug() << buffer << endl;
    if(buffer.contains(ExpectedReplyFromHost.toLatin1())){
        return 1;
    }
    return 0;
}

void tcp::CheckConnectState()
//确认与服务器的连接，并且发送helo sis
{
    tcpSocket->waitForReadyRead(5000);
    QString buffer = tcpSocket->readAll();
    if(tcpSocket->state() == QAbstractSocket::ConnectedState){
        SendHeloSis();
    }
    else{
        QMessageBox::warning(NULL,QObject::tr("Warning"),QObject::tr("连接邮件服务失败！"), QMessageBox::Yes);
    }
}

void tcp::SendHeloSis()
// 发送helo sis，等待host回复
{
    QString str = "helo sis\r\n";
    qDebug()<<"Smtp::SendHeloSis  向服务器发送了:"<<str;
    tcpSocket->write(str.toLatin1());
    tcpSocket->waitForBytesWritten(5000);

    if(ReadFromHost("250 OK\r\n")){
        SendAuthLogin();
    }
}

void tcp::SendAuthLogin()
{
    QString str = "auth login\r\n";
    qDebug()<<"Smtp::SendAuthLogin  向服务器发送了:"<<str;
    tcpSocket->write(str.toLatin1());
    tcpSocket->waitForBytesWritten(5000);

    if(ReadFromHost("334 VXNlcm5hbWU6\r\n")){
        SendMyEmailAddress();
    }
}

void tcp::SendMyEmailAddress()
{
    QString str = QString("\r\n").prepend(senderEmail.toLatin1().toBase64());
    qDebug()<<"Smtp::SendMyEmailAddress  向服务器发送了:"<<str;
    tcpSocket->write(str.toLatin1());
    tcpSocket->waitForBytesWritten(5000);

    if(ReadFromHost("334 UGFzc3dvcmQ6\r\n")){
        SendAuthCode();
    }
}

void tcp::SendAuthCode()
{
    QString str = QString("\r\n").prepend(senderKey.toLatin1().toBase64());
    qDebug()<<"Smtp::SendAuthCode  向服务器发送了:"<<str;
    tcpSocket->write(str.toLatin1());
    tcpSocket->waitForBytesWritten(5000);

    if(ReadFromHost("235 Authentication successful\r\n")){
        SendEmailFrom();
    }
}

void tcp::SendEmailFrom()
{
    QString str = QString("mail from:<%1>\r\n").arg(senderEmail);
    qDebug()<<"Smtp::SendEmailFrom  向服务器发送了:"<<str;
    tcpSocket->write(str.toLatin1());
    tcpSocket->waitForBytesWritten(5000);

    if(ReadFromHost("250 OK")){
        SendRcptTo();
    }
}

void tcp::SendRcptTo()
{
    QString str = QString("rcpt to:<%1>\r\n").arg(email);
    qDebug()<<"Smtp::SendRcptTo  向服务器发送了:"<<str;
    tcpSocket->write(str.toLatin1());
    tcpSocket->waitForBytesWritten(5000);

    if(ReadFromHost("250 OK\r\n")){
        StartSendContent();
    }
}

void tcp::StartSendContent()
{
    QString str = "data\r\n";
    qDebug()<<"Smtp::StartSendContent  向服务器发送了:"<<str;
    tcpSocket->write(str.toLatin1());
    tcpSocket->waitForBytesWritten(5000);

    if(ReadFromHost("354 End data with <CR><LF>.<CR><LF>")){
        SendContent();
    }
}

void tcp::SendContent()
{
    QString Time_Now = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");

    qDebug()<<"向服务器发送：　" + annotation;

    tcpSocket->write(annotation.toUtf8());
    tcpSocket->waitForBytesWritten(5000);

    if(ReadFromHost("250 OK: queued as")){
        SendingStateConfirm();
    }
}

void tcp::SendingStateConfirm()
{
    QString str = "quit\r\n";
    tcpSocket->write(str.toUtf8());
    tcpSocket->waitForBytesWritten(5000);
    //221 Bye
}

tcp_code::tcp_code(QString code, QString email) : tcp(email)
{
    this->code = code;

    setTitle();
    setContent();
    setAnnotation();

    tcpSocket->connectToHost("smtp.qq.com",25);
    tcpSocket->waitForConnected(3000);
    CheckConnectState();
}

void tcp_code::setTitle()
{
    // 标题
    title = "GoBang_studio注册验证码";
}

void tcp_code::setContent()
{
    // 内容
    content = "亲爱的用户:\r\n欢迎您注册GoBang_studio，您的验证码为"
            + code
            + ",请勿将此验证码泄露给其他人，若您未注册系统，请忽略此邮件。\r\n\r\n本邮件于"
            + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd")
            + "自动生成，请勿回复。";
}

void tcp_code::setAnnotation()
{
    // 注释
    annotation = QString("subject:GoBang_Studio验证码\r\n"
                          "FROM:GoBang_Studio <%1>\r\n"
                          "TO:%2\r\n"
                          "\r\n"
                          "%3\r\n"
                          "\r\n"
                          ".\r\n").arg(senderEmail).arg(email).arg(content);
}

tcp_pwd::tcp_pwd(QString pwd, QString email) : tcp(email)
{
    this->pwd = pwd;

    setTitle();
    setContent();
    setAnnotation();

    tcpSocket->connectToHost("smtp.qq.com",25);
    tcpSocket->waitForConnected(3000);
    CheckConnectState();
}

void tcp_pwd::setTitle()
{
    // 标题
    title = "GoBang_studio 找回密码";
}

void tcp_pwd::setContent()
{
    // 内容
    content = "亲爱的用户:\r\n欢迎您游玩GoBang_studio，您的密码为" +
              pwd +
              ",请勿将密码泄露给其他人，若您未注册GoBang_studio，请忽略此邮件。\r\n\r\n本邮件于" +
              QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd") +
              "自动生成，请勿回复。";
}

void tcp_pwd::setAnnotation()
{
    // 注释
    annotation = QString("subject:GoBang_Studio 找回密码\r\n"
                          "FROM:GoBang_Studio <%1>\r\n"
                          "TO:%2\r\n"
                          "\r\n"
                          "%3\r\n"
                          "\r\n"
                          ".\r\n").arg(senderEmail).arg(email).arg(content);
}
