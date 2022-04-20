#include"chatservice.hpp"
#include"public.hpp"
#include<muduo/base/Logging.h>  // LOG_ERROR <<
using namespace muduo;
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
    LOG_INFO << "do login service.";
}
/* 处理注册业务 */
void ChatService::reg(const TcpConnectionPtr &conn,
                      json &js, Timestamp time)
{
    LOG_INFO << "do reg service.";
}