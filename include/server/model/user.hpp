#ifndef USER_H
#define USER_H
#include<string>
using namespace std;
/* 属于映射类
 * 匹配User表的ORM类
 */
class User
{
public:
    User(int id = -1, string name="", string password="", string state="offline")
        : id_(id), name_(name), password_(password), state_(state)
    {
        
    }
    void setId(int id)
    {
        id_ = id;
    }
    void setName(string name)
    {
        name_ = name;
    }
    void setPassword(string password)
    {
        password_ = password;
    }
    void setState(string state)
    {
        state_ = state;
    }
    int getId() const
    {
        return id_;
    }
    string getName() const
    {
        return name_;
    }
    string getPassword() const
    {
        return password_;
    }
    string getState() const
    {
        return state_;
    }
private:
    int id_;
    string name_;
    string password_;
    string state_;

};
#endif