#include"chatserver.hpp"
#include<iostream>
using namespace std;

#include<signal.h>
#include"chatservice.hpp"
#include<muduo/base/Logging.h>  // LOG_ERROR <<
using namespace muduo;

/* 处理服务器Ctrl+C结束后，重置user状态信息 */
void resetHandler(int)
{
    ChatService::instance()->reset();
    LOG_INFO << "服务器因SIGINT信号退出，用户状态已重置。";
    exit(0);
}
int main()
{
    signal(SIGINT, resetHandler);
    EventLoop loop;
    InetAddress addr("127.0.0.1", 6000);
    ChatServer server(&loop, addr, "ChatServer");
    ChatService::instance()->reset();
    server.start();
    loop.loop();    //开启事件循环
    return 0;
}