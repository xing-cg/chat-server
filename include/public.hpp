#ifndef PUBLIC_H
#define PUBLIC_H
/* server和client的公共文件
 */
enum EnMsgType
{
    LOGIN_MSG = 1,  //登录消息id, 与chatservice中的login方法绑定
    REG_MSG,        //注册消息id, 与chatservice中的reg方法绑定
    REG_MSG_ACK     //注册相应消息
};
#endif