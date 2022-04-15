#ifndef CHATSERVER_H
#define CHATSERVER_H
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
using namespace muduo;
using namespace muduo::net;
class ChatServer
{
public:
    /* 初始化聊天服务器对象 */
    ChatServer(EventLoop* loop,
               const InetAddress& listenAddr,
               const string& nameArg);
    /* 启动服务 */
    void start();
private:
    /* 要注册两个方法
     * 给TcpServer注册新用户的连接、连接断开的、已连接用户的可读写事件
     */
    void onConnection();
    TcpServer _server;
    EventLoop *_loop;

};

#endif