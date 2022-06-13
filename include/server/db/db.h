#ifndef DB_H
#define DB_H
#include<mysql/mysql.h>
#include<string>
using namespace std;
class MySQL
{
private:
    MYSQL *m_conn;
public:
    /* 初始化数据库连接 */
    MySQL();
    /* 释放数据库连接资源 */
    ~MySQL();
public:
    /* 获取连接 */
    MYSQL * getConnection();
public:
    /* 连接数据库 */
    bool connect();
public:
    /* 查询操作 */
    MYSQL_RES * query(string sql);
public:
    /* 更新操作 */
    bool update(string sql);
};
#endif