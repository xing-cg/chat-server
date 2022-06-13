#ifndef USERMODEL_H
#define USERMODEL_H
#include"user.hpp"
/* User表的数据操作类 */
class UserModel
{
public:
    /* User表的增加方法 */
    bool insert(User &user);
public:
    /* 根据用户号码查询用户信息 */
    User query(int id);
public:
    /* 更新用户的状态信息 */
    bool updateState(User user);
    /* 重置所有用户的状态信息 */
    void resetAllState();
};
#endif