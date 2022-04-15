#ifndef CHATSERVER_H
#define CHATSERVER_H
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
using namespace muduo;
using namespace muduo::net;
/* 聊天服务器的主类 */
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
    /* 上报链接相关信息的回调函数（连接创建，连接断开）*/
    void onConnection(const TcpConnectionPtr&);
    /* 上报读写事件相关信息的回调函数*/
    void onMessage(const TcpConnectionPtr&,
                            Buffer*,
                            Timestamp);
    /* 组合的muduo库，实现服务器功能的类对象 */
    TcpServer _server;
    /* 指向事件循环对象的指针 */
    EventLoop *_loop;

};

#endif