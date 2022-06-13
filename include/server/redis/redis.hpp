#ifndef REDIS_H
#define REDIS_H
#include<hiredis/hiredis.h>
#include<functional>
using namespace std;
class Redis
{
public:
    Redis();
    ~Redis();
public:
    /* 连接Redis服务器 */
    bool connect();
public:
    /* 向指定的redis频道发布消息 */
    bool publish(int channel, string message);
    /* 向指定的redis频道订阅消息 */
    bool subscribe(int channel);
    /* 向指定的redis频道取消订阅消息 */
    bool unsubscribe(int channel);
public:
    /* 在独立线程中接收订阅频道中的消息 */
    void observer_channel_message();
public:
    using MessageCallback = function<void(int, string)>;
    /* 初始化向业务层上报频道消息 的回调函数, 需要用到一个int(频道号), 一个消息内容字符串 */
    void init_notify_handler(MessageCallback cb);

private:
    /* hiredis同步上下文对象, 负责publish消息 */
    redisContext * m_publish_context;
    /* hiredis同步上下文对象, 负责subscribe消息 */
    redisContext * m_subscribe_context;
private:
    /* 回调操作, 收到订阅的消息, 给service层上报 */
    MessageCallback m_notify_handler;
};
#endif
/**
 * 1. 对于"hiredis上下文对象"的理解:
 *   - 相当于一个redis-cli, 存储了连接相关的信息;
 * 2. 为什么要写两个上下文对象?
 *   - 如果上下文对象正在subscribe那么其将会阻塞,
 *   - 所以subscribe和publish需要分开操作;
 */