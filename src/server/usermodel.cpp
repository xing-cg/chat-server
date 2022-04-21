#include"usermodel.hpp"
#include"db.h"
#include<iostream>
using namespace std;
/* User表的增加方法 */
bool UserModel::insert(User &user)
{
    /* 1.组装SQL语句 */
    char sql[1024] = {0};
    sprintf(sql, "insert into user(name, password, state) values('%s', '%s', '%s')",
        user.getName().c_str(), user.getPassword().c_str(), user.getState().c_str());
    MySQL mysql;
    if(mysql.connect())
    {
        if(mysql.update(sql))
        {
            /* 获取插入成功的用户数据生成的主键id */
            /* 以下的mysql_insert_id是生成id的方法之一 */
            user.setId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }
    /* 注册失败 */
    return false;
}
/* 根据用户号码查询用户信息 */
User UserModel::query(int id)
{
    /* 1.组装SQL语句 */
    char sql[1024] = {0};
    sprintf(sql, "select * from user where id = %d", id);
    MySQL mysql;
    if(mysql.connect())
    {
        /* mysql.query内部申请了资源，处理完成User的构造后，需要free */
        MYSQL_RES *res = mysql.query(sql);  // 此query为MySQL的query，和update同级。
        if(res != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            if(row != nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setPassword(row[2]);
                user.setState(row[3]);
                mysql_free_result(res);
                return user;
            }
        }
    }
    /* 如果没有有效的查询结果，返回一个默认User，id为-1，表示出错 */
    return User();
}
/* 更新用户的状态信息 */
bool UserModel::updateState(User user)
{
    /* 1.组装SQL语句 */
    char sql[1024] = {0};
    sprintf(sql, "update user set state = '%s' where id = %d", user.getState().c_str(), user.getId());
    MySQL mysql;
    if(mysql.connect())
    {
        if(mysql.update(sql))
        {
            return true;
        }
    }
    return false;
}