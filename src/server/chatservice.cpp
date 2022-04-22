#include"chatservice.hpp"
#include"public.hpp"
#include<muduo/base/Logging.h>  // LOG_ERROR <<
using namespace muduo;

#include<vector>
using namespace std;

ChatService* ChatService::instance()
{
    static ChatService service;
    return &service;
}
/* 注册消息以及对应的Handler回调操作
 * 对unordered_map成员变量进行初始化
 * 这是业务模块的核心，也是对项目进行解耦的核心
 */
ChatService::ChatService()
{
    /* 键：消息id - 值：函数对象 */
    _msgHandlerMap.insert({LOGIN_MSG, std::bind(&ChatService::login, this, _1, _2, _3)});
    _msgHandlerMap.insert({REG_MSG, std::bind(&ChatService::reg, this, _1, _2, _3)});
    _msgHandlerMap.insert({ONE_CHAT_MSG, std::bind(&ChatService::oneChat, this, _1, _2, _3)});
    _msgHandlerMap.insert({ADD_FRIEND_MSG, std::bind(&ChatService::addFriend, this, _1, _2, _3)});
}
/* 业务重置方法，通常在服务器异常退出时调用 */
void ChatService::reset()
{
    /* 把所有online用户的状态置为offline */
    _userModel.resetAllState();
}
/* 获取消息对应的处理器 */
MsgHandler ChatService::getHandler(int msgid)
{
    /* 记录错误日志，msgid没有对应的事件处理回调时 */
    auto it = _msgHandlerMap.find(msgid);
    if(it == _msgHandlerMap.end())
    {
        /* 没有找到处理器时，返回一个默认的处理器，空操作。
         * 如此设计的好处，即使没有找到程序也不会因此挂掉，进行空操作，继续运行。
         * 因为需要获取函数参数msgid，所以[=]按值获取。
         */
        return [=](const TcpConnectionPtr&, json&, Timestamp)
        {
            LOG_ERROR << "msgid: " << msgid << " can't find handler";
        };
    }
    else
    {
        return _msgHandlerMap[msgid];
    }
}
/* 处理登录业务 */
void ChatService::login(const TcpConnectionPtr &conn,
                        json &js, Timestamp time)
{
    /* 从json参数获取账号、密码信息 */
    int id = js["id"].get<int>();
    string password = js["password"];
    User user = _userModel.query(id);

    json response;
    response["msgid"] = LOGIN_MSG_ACK;
    if(user.getId() == id && user.getPassword() == password)
    {
        if(user.getState() == "online")
        {
            /* 该用户已经登录在线，不允许重复登陆 */
            response["errno"] = LOGIN_REPEAT;
            response["errmsg"] = "该用户已经登录";
        }
        else if(user.getState() == "offline")
        {
            /* 登陆成功 */
            {
                /* 记录用户连接信息 */
                lock_guard<mutex> lock(_connMutex);
                _userConnectionMap.insert({id, conn});
            }
            /* 更新用户状态信息 */
            user.setState("online");
            _userModel.updateState(user);

            response["errno"] = LOGIN_SUCCEESS;
            response["id"] = user.getId();
            response["name"] = user.getName();
            /* 查询该用户是否在离线时未收到的消息 */
            vector<string> offlineMsgVec = _offlineMsgModel.query(id);
            if(!offlineMsgVec.empty())
            {
                response["offlinemsg"] = offlineMsgVec;
                /* 把该用户的所有离线消息从从数据中删除掉 */
                _offlineMsgModel.remove(id);
            }
            /* 查询该用户的好友信息，并返回 */
            vector<User> userVec = _friendModel.query(id);
            if(!userVec.empty())
            {
                vector<string> friendJsonInfoVec;
                for(User &user : userVec)
                {
                    json js;
                    js["id"] = user.getId();
                    js["name"] = user.getName();
                    js["state"] = user.getState();
                    friendJsonInfoVec.push_back(js.dump());
                }
                response["friends"] = friendJsonInfoVec;
            }
        }
    }
    else if(user.getId() != id)
    {
        /* 登录失败，用户不存在 */
        response["errno"] = LOGIN_NOTFOUND;
        response["errmsg"] = "用户不存在";
    }
    else if(user.getPassword() != password)
    {
        /* 登录失败，密码不匹配 */
        response["errno"] = LOGIN_WRONGPWD;
        response["errmsg"] = "密码验证失败";
    }
    conn->send(response.dump());
}
/* 处理注册业务 */
void ChatService::reg(const TcpConnectionPtr &conn,
                      json &js, Timestamp time)
{
    /* LOG_INFO << "do reg service."; */
    string name = js["name"];
    string password = js["password"];

    User user;
    user.setName(name);
    user.setPassword(password);
    bool state = _userModel.insert(user);
    json response;
    response["msgid"] = REG_MSG_ACK;
    if(state)
    {
        /* 注册成功 */
        response["errno"] = 0;
        response["id"] = user.getId();
    }
    else
    {
        /* 注册失败 */
        response["errno"] = 1;
    }
    conn->send(response.dump());
}
/* 处理客户端异常退出 */
void ChatService::clientCloseException(const TcpConnectionPtr & conn)
{
    /* 查找 */
    lock_guard<mutex> lock(_connMutex);
    User user;
    for(auto it = _userConnectionMap.begin(); it != _userConnectionMap.end(); ++it)
    {
        if(it->second == conn)
        {
            /* 从map表删除用户的连接信息 */
            user.setId(it->first);
            _userConnectionMap.erase(it);
            break;
        }
    }
    /* 更新用户的状态信息 */
    if(user.getId() != -1)
    {
        user.setState("offline");
        _userModel.updateState(user);
    }
}
/* 一对一聊天业务 */
void ChatService::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int to = js["to"].get<int>();
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnectionMap.find(to);
        if(it != _userConnectionMap.end())
        {
            /* 接收方在线，转发消息 */
            /* 服务器主动推送消息给接收方 */
            it->second->send(js.dump());    // it->second 表示 
            return;
        }
    }
    /* 接收方离线，存储离线消息 */
    _offlineMsgModel.insert(to, js.dump());
}
/* 添加好友业务 */
void ChatService::addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();

    /* 存储好友信息 -> include"friendmodel.hpp" */
    _friendModel.insert(userid, friendid);
}

/* 创建群组业务 */
void ChatService::createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    string groupName = js["groupname"];
    string groupDesc = js["groupdesc"];

    /* 存储新创建的群组信息 */
    Group group(-1, groupName, groupDesc);
    if(_groupModel.createGroup(group))
    {
        /* 把创建人加入群组 */
        _groupModel.addGroup(userid, group.getId(), "Creator");
    }
}
/* 加入群组业务 */
void ChatService::addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    _groupModel.addGroup(userid, groupid, "Normal");
}
/* 群组聊天业务 */
void ChatService::groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    vector<int> useridVec = _groupModel.queryGroupUsers(userid, groupid);

    bool online = false;
    for(int id : useridVec)
    {
        {
            lock_guard<mutex> lock(_connMutex);
            auto it = _userConnectionMap.find(id);
            if(it != _userConnectionMap.end())
            {
                online = true;
                it->second->send(js.dump());
            }
        }
        if(!online)
        {
            /* 存储离线群消息 */
            _offlineMsgModel.insert(id, js.dump());
        }
        online = false;
    }
}