#include"chatserver.hpp"
#include"json.hpp"
#include"chatservice.hpp"
#include<functional>
#include<string>

using namespace std;
using namespace placeholders;
using json = nlohmann::json;
ChatServer::ChatServer(EventLoop* loop,
                       const InetAddress& listenAddr,
                       const string& nameArg)
    : _server(loop, listenAddr, nameArg), _loop(loop)
{
    /* 注册连接回调 */
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));
    /* 注册消息回调 */
    _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));
    /* 设置线程数量 */
    _server.setThreadNum(4);
}
/* 启动服务 */
void ChatServer::start()
{
    _server.start();
}
/* 以下两个函数： 
 * 连接监听
 * 读写事件监听
 */
/* 上报链接相关信息的回调函数（连接创建，连接断开）*/
void ChatServer::onConnection(const TcpConnectionPtr&conn)
{
    /* 客户端断开连接 */
    if(!conn->connected())conn->shutdown(); //释放socketfd资源
}
/* 上报读写事件相关信息的回调函数*/
void ChatServer::onMessage(const TcpConnectionPtr& conn,
                           Buffer* buffer,
                           Timestamp time)
{
    /* muduo库或从网络上读取的数据首先在数据缓冲区，
     * 收到一个消息后，retrieveAllAsString可以把缓冲区的数据
     * 取出到一个字符串中。
     */
    string buf = buffer->retrieveAllAsString();
    /* 数据的反序列化
     * json中包含message_type，message_id
     */
    json js = json::parse(buf);
    /* 通过js["msgid"]绑定一个回调操作，即一个id一个操作
     * 解析到msgid后获取业务处理器handler，处理器是事先绑定的方法。网络模块看不到，业务模块绑定的。
     * 回调handler时，conn、js对象、time都可以传入。
     * 我们要达到的目的：完全解耦网络模块的代码和业务模块的代码
     */
    auto msgHandler = ChatService::instance()->getHandler(js["msgid"].get<int>());  // js["msgid"] 依旧是json类型，需要转为int
    /* 回调消息绑定好的事件处理器，来执行相应的业务处理 */
    msgHandler(conn, js, time);
}