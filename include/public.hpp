#ifndef PUBLIC_H
#define PUBLIC_H
/* server和client的公共文件 */

enum EnMsgType  //En表示Enum枚举
{
    LOGIN_MSG = 1,      //登录 消息id, 与chatservice中的login方法绑定
    LOGIN_MSG_ACK,      //登录响应

    REG_MSG,            //注册 消息id, 与chatservice中的reg方法绑定
    REG_MSG_ACK,        //注册响应

    ADD_FRIEND_MSG,     //添加好友
    ONE_CHAT_MSG,       //一对一聊天

    CREATE_GROUP_MSG,   //创建群组
    ADD_GROUP_MSG,      //加入群组
    GROUP_CHAT_MSG      //群聊
};

enum EnLoginErrType
{
    LOGIN_SUCCEESS = 0,
    LOGIN_REPEAT = 1,
    LOGIN_NOTFOUND = 2,
    LOGIN_WRONGPWD = 3
};
#endif