#include"redis.hpp"
#include<iostream>
#include<thread>
using namespace std;
Redis::Redis()
    : m_publish_context(nullptr), m_subscribe_context(nullptr)
{

}
Redis::~Redis()
{
    if(m_publish_context != nullptr)
    {
        redisFree(m_publish_context);
    }
    if(m_subscribe_context != nullptr)
    {
        redisFree(m_subscribe_context);
    }
}
bool Redis::connect()
{
    m_publish_context = redisConnect("127.0.0.1", 6379);
    if(nullptr == m_publish_context)
    {
        cerr << "connect redis failed!" << endl;
        return false;
    }

    m_subscribe_context = redisConnect("127.0.0.1", 6379);
    if(nullptr == m_subscribe_context)
    {
        cerr << "connect redis failed!" << endl;
        return false;
    }

    /**
     * 由于subscribe操作是阻塞的, 
     * 在实际的使用环境下, 不可能因为一个订阅操作去阻塞一个服务器, 
     * 所以要用一个单独的线程来完成监听频道上的事件,
     * 有消息则给业务层进行上报; 
     */
    thread t(
        [&]() { observer_channel_message();} );
    t.detach();

    cout << "connect redis-server success!" << endl;
    return true;
}
bool Redis::publish(int channel, string message)
{
    /* 相当于向redis-server发送命令, reply接收命令执行结果 */
    redisReply * reply = (redisReply*)redisCommand(
        m_publish_context, "PUBLISH %d %s", channel, message.c_str());
    if(nullptr == reply)
    {
        cerr << "publish command failed!" << endl;
        return false;
    }
    freeReplyObject(reply);
    return true;
}
bool Redis::subscribe(int channel)
{
    if(REDIS_ERR == redisAppendCommand(
        m_subscribe_context, "SUBSCRIBE %d", channel))
    {
        cerr << "subscribe command failed!" << endl;
        return false;
    }
    /* 循环发送缓冲区内容, 直到发送完毕 */
    int done = 0;
    while(!done)
    {
        if(REDIS_ERR == redisBufferWrite(m_subscribe_context, &done))
        {
            cerr << "subscribe command failed!" << endl;
            return false;
        }
    }
    /**
     * 这里不做redisReply的操作,
     * 这是个阻塞的操作, 放在observer_channel_message中做;
     */
    return true;
}
bool Redis::unsubscribe(int channel)
{
    if(REDIS_ERR == redisAppendCommand(
        m_subscribe_context, "UNSUBSCRIBE %d", channel))
    {
        cerr << "unsubscribe command failed!" << endl;
        return false;
    }
    /* 循环发送缓冲区内容, 直到发送完毕 */
    int done = 0;
    while(!done)
    {
        if(REDIS_ERR == redisBufferWrite(m_subscribe_context, &done))
        {
            cerr << "unsubscribe command failed!" << endl;
            return false;
        }
    }
    /**
     * 这里不做redisReply的操作,
     * 这是个阻塞的操作, 放在observer_channel_message中做;
     */
    return true;
}
/**
 * Redis频道如果有消息, 则有三个字段, 
 * 对应的是redisGetReply返回的reply->element[0],[1],[2];
 * 本项目的element[1]对应的是频道号;
 * 本项目的element[2]对应的是消息体;
 */
void Redis::observer_channel_message()
{
    redisReply *reply = nullptr;
    int res = REDIS_ERR;
    while(REDIS_OK==(res = redisGetReply(
                                m_subscribe_context, (void**)&reply)))
    {
        if(reply != nullptr &&
           reply->element[2] != nullptr &&
           reply->element[2]->str != nullptr)
        {
            /* 给业务层上报频道上发生的消息, 即频道号+消息体 */
            m_notify_handler(atoi(reply->element[1]->str),
                             reply->element[2]->str );
        }
        freeReplyObject(reply);
    }
    if(res == REDIS_ERR)
    {
        cerr << "redisGetReply err" << endl;
        return;
    }
    cerr << "observer_channel_message quit" << endl;
}
void Redis::init_notify_handler(MessageCallback cb)
{
    m_notify_handler = cb;
}
/**
 * redisCommand和redisAppendCommand的区别:
 *  1. redisAppendCommand只是把命令先写到本地缓存中;
 *  2. 写到缓存之后还需要调用redisBufferWrite把缓存中的命令发送到Redis服务器;
 *  3. 最后, 如果要获得reply, 还需要调用redisGetReply获取结果, 这个操作对于subscribe是阻塞的;
 *  4. 由于publish操作一般不会阻塞, 所以直接调用redisCommand;
 *  5. 由于subscribe操作最后的redisGetReply将会阻塞, 所以我们把这几个步骤单独写出来, 粒度减小, 追求效率;
 */

