#include "userlist.h"

userlist::userlist() {
    head = new usernode();
    len = 0;
}

userlist::~userlist()
{
    usernode *tmp = head, *tmp_next;
    while (tmp) {
        tmp_next = tmp->next;
        free(tmp);
        tmp = tmp_next;
    }
}

int userlist::getLen()
{
    return len;
}

userlist::usernode* userlist::getIthNode(int i) // i: 1-len共len个数中任意一个数
{
    usernode* tmp = head;
    for (int j = 0; j < i; ++ j) {
        tmp = tmp->next;
    }
    return tmp;
}

// 记得检查空指针
void userlist::push_front(userlist::usernode* node)
{
    if (node) {
        ++ len;
        node->next = head->next;
        head->next = node;
    }
}

void userlist::push_front(QString username, QString pwd, QString email, QString avatar_url)
{
    usernode* new_node = new usernode(username, pwd, email, avatar_url, head->next);
    if (new_node) {
        ++ len;
        head->next = new_node;
    }
}

void userlist::push_back(usernode* node)
{
    if (node) {
        getIthNode(len ++ )->next = node;
        node->next = nullptr;
    }
}

void userlist::push_back(QString username, QString pwd, QString email, QString avatar_url)
{
    usernode* new_node = new usernode(username, pwd, email, avatar_url, nullptr);
    if (new_node) {
        getIthNode(len ++ )->next = new_node;
        new_node->next = nullptr;
    }
}

userlist::usernode* userlist::pop_front()
{
    if (len) {
        usernode* to_pop = head->next;
        head->next = head->next->next;
        return to_pop;
    }
    return nullptr;
}

userlist::usernode* userlist::pop_back()
{
    if (len) {
        usernode* pre_to_pop = getIthNode( -- len);
        usernode* to_pop = pre_to_pop->next;
        pre_to_pop->next = nullptr;
        return to_pop;
    }
    return nullptr;
}

