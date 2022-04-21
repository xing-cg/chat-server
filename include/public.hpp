#ifndef PUBLIC_H
#define PUBLIC_H
/* server和client的公共文件
 */
enum EnMsgType
{
    LOGIN_MSG = 1,  //登录消息id, 与chatservice中的login方法绑定
    LOGIN_MSG_ACK,  //登录响应消息
    REG_MSG,        //注册消息id, 与chatservice中的reg方法绑定
    REG_MSG_ACK     //注册响应消息
};

enum EnLoginErrType
{
    LOGIN_SUCCEESS = 0,
    LOGIN_REPEAT = 1,
    LOGIN_NOTFOUND = 2,
    LOGIN_WRONGPWD = 3
};
#endif