#ifndef USERLIST_H
#define USERLIST_H

#include <QString>

class userlist
{
public:
    struct usernode {
        QString username;
        QString pwd;
        QString email;
        QString avatar_url; // 头像绝对位置
        usernode* next;

        usernode() {
            this->username = "";
            this->pwd = "";
            this->email = "";
            this->avatar_url = ""; // 这里是不是写默认位置？
            this->next = nullptr;
        }

        usernode(QString username, QString pwd, QString email, QString avatar_url, usernode* next = nullptr) {
            this->username = username;
            this->pwd = pwd;
            this->email = email;
            this->avatar_url = avatar_url;
            this->next = next;
        }
    };

private:
    usernode* head; // 头指针，不存任何信息，位次为0
    int len; // 长度，链表存了多少节点（非head）

public:
    userlist();

    ~userlist();

    int getLen();

    usernode* getIthNode(int i); // i: 1-len共len个数中任意一个数

    // 记得检查空指针
    void push_front(usernode* node);

    void push_front(QString username, QString pwd, QString email, QString avatar_url);

    void push_back(usernode* node);

    void push_back(QString username, QString pwd, QString email, QString avatar_url);

    usernode* pop_front();

    usernode* pop_back();
};

#endif // USERLIST_H
