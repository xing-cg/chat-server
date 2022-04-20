#ifndef CHATSERVICE_H
#define CHATSERVICE_H
#include<muduo/net/TcpConnection.h>
#include<unordered_map>
#include<functional>

using namespace std;
using namespace muduo;
using namespace muduo::net;
#include"json.hpp"
using json = nlohmann::json;
/* 表示处理消息的事件回调方法类型 */
using MsgHandler = std::function<void(const TcpConnectionPtr&, json&, Timestamp)>;
/* 聊天服务器业务类
 * 用映射关系来存储消息id和具体处理函数
 * 此类有一个实例就够了，所以采用单例模式
 */
class ChatService
{
public:
    /* 获取单例对象的接口函数 */
    static ChatService* instance();
    /* 处理登录业务 */
    void login(const TcpConnectionPtr &conn, json &js, Timestamp time);
    /* 处理注册业务 */
    void reg(const TcpConnectionPtr &conn, json &js, Timestamp time);
    /* 获取消息对应的处理器 */
    MsgHandler getHandler(int msgid);
private:
    ChatService();
    /* 存储消息id和其对应的业务处理方法 */
    unordered_map<int, MsgHandler> _msgHandlerMap;
};
#endif