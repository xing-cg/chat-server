#include"friendmodel.hpp"
#include"db.h"
/* 添加好友关系 */
void FriendModel::insert(int userid, int friendid)
{
    char sql[1024] = {0};
    sprintf(sql, "insert into friend values(%d, %d)", userid, friendid);

    MySQL mysql;
    if(mysql.connect())
    {
        mysql.update(sql);
    }
}
/* 返回用户好友列表 */
vector<User> FriendModel::query(int userid)
{
    char sql[1024] = {0};
    /* 联合查询 */
    sprintf(sql, "SELECT user_.id,user_.name,user_.state FROM user user_ INNER JOIN friend friend_ ON friend_.friendid = user_.id WHERE friend_.userid=%d", userid);
    vector<User> vec;
    MySQL mysql;
    if(mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if(res != nullptr)
        {
            MYSQL_ROW row;
            while(nullptr != (row = mysql_fetch_row(res)))
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                vec.push_back(user);
            }
            mysql_free_result(res);
        }
    }
    return vec;
}