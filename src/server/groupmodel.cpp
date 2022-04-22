#include"groupmodel.hpp"
#include"db.h"
/* 创建群组 */
bool GroupModel::createGroup(Group &group)
{
    char sql[1024] = {0};
    sprintf(sql, "insert into allgroup(groupname, grouodesc) values('%s', '%s')",
            group.getName().c_str(), group.getDesc().c_str());
    MySQL mysql;
    if(mysql.connect())
    {
        if(mysql.update(sql))
        {
            group.setId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }
    return false;
}
/* 用户加入群组 */
void GroupModel::addGroup(int userid, int groupid, string role)
{
    char sql[1024] = {0};
    sprintf(sql, "insert into groupuser values(%d, %d, '%s')",
            groupid, userid, role.c_str());
    MySQL mysql;
    if(mysql.connect())
    {
        mysql.update(sql);
    }
}
/* 查询用户所在群组信息 */
vector<Group> GroupModel::queryGroups(int userid)
{
    char sql[1024] = {0};
    sprintf(sql, "select group_.id, group_.groupname, group_.groupdesc from allgroup group_ \
            INNER JOIN groupuser groupuser_ ON group_.id = groupuser_.groupid \
            WHERE groupuser_.userid=%d", userid);
    vector<Group> groupVec;

    MySQL mysql;
    if(mysql.connect())
    {
        MYSQL_RES * res = mysql.query(sql);
        if(res != nullptr)
        {
            MYSQL_ROW row;
            /* 查出userid所有的群组信息 */
            while(nullptr != (row = mysql_fetch_row(res)))
            {
                Group group;
                group.setId(atoi(row[0]));
                group.setName(row[1]);
                group.setDesc(row[2]);
                groupVec.push_back(group);
            }
            mysql_free_result(res);
        }
    }
    for(Group &group : groupVec)
    {
        sprintf(sql, "select user_.id, user_.name, user_.state, groupuser_.role from user user_ \
                INNER JOIN groupuser groupuser_ \
                ON groupuser_.userid = user_.id \
                WHERE groupuser_.groupid=%d", group.getId());
        MYSQL_RES * res = mysql.query(sql);
        if(res != nullptr)
        {
            MYSQL_ROW row;
            while(nullptr != (mysql_fetch_row(res)))
            {
                GroupUser user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                user.setRole(row[3]);
                group.getUsers().push_back(user);
            }
            mysql_free_result(res);
        }
    }
    return groupVec;
}
/* 根据指定的groupid查询群组用户id列表，除userid自身 */
vector<int> GroupModel::queryGroupUsers(int userid, int groupid)
{
    char sql[1024] = {0};
    sprintf(sql, "select userid from groupuser \
            WHERE groupid = %d and userid != %d", groupid, userid);
    vector<int> groupUsersIdVec;
    MySQL mysql;
    if(mysql.connect())
    {
        MYSQL_RES * res = mysql.query(sql);
        if(res != nullptr)
        {
            MYSQL_ROW row;
            while(nullptr != (row = mysql_fetch_row(res)))
            {
                groupUsersIdVec.push_back(atoi(row[0]));
            }
            mysql_free_result(res);
        }
    }
    return groupUsersIdVec;
}