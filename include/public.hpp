#ifndef PUBLIC_H
#define PUBLIC_H
/* server和client的公共文件
 */
enum EnMsgType
{
    LOGIN_MSG = 1,  //登录消息id, 与chatservice中的login方法绑定
    LOGIN_MSG_ACK,      //登录响应消息
    REG_MSG,            //注册消息id, 与chatservice中的reg方法绑定
    REG_MSG_ACK,        //注册响应消息
    ONE_CHAT_MSG,       //聊天消息
    ADD_FRIEND_MSG,     //添加好友消息

    CREATE_GROUP_MSG,   //创建群组消息
    ADD_GROUP_MSG,      //加入群组消息
    GROUP_CHAT_MSG      //群发聊天消息
};

enum EnLoginErrType
{
    LOGIN_SUCCEESS = 0,
    LOGIN_REPEAT = 1,
    LOGIN_NOTFOUND = 2,
    LOGIN_WRONGPWD = 3
};
#endif