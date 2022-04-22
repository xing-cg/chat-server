#ifndef GROUPUSER_H
#define GROUPUSER_H
#include"user.hpp"
/* 群组用户相对于一般的User多了一个role角色信息 */
class GroupUser : public User
{
public:
    void setRole(string role){ role_ = role; }
    string getRole() const { return role_; }
private:
    string role_;
};

#endif