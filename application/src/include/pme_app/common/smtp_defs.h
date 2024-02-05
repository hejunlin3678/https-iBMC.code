/* 
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved. 
* Description: SMTP协议相关的定义。
* Author: h00282621 
* Create: 2020-3-10 
* Notes: 用于跨模块使用的相关的定义。
*/

#ifndef __SMTP_DEFINE_H__
#define __SMTP_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


#define SMTP_ERR_ACCOUNT             (-1001)       //发件人账户不存在
#define SMTP_ERR_CONNECT             (-1002)       //连接服务器失败
#define SMTP_ERR_QUIT                (-1003)       //邮件服务器响应失败
#define SMTP_ERR_ADDRESS             (-1004)       //发件人地址错误
#define SMTP_ERR_NAME_PASSWORD       (-1005)       //发件人用户名或密码错误
#define SMTP_ERR_SEND_MSG            (-1006)       //邮件发送失败
#define SMTP_ERR_RECEIVE_ADDRESS     (-1007)       //告警邮件接收地址错误
#define SMTP_ERR_REQUIRED_AUTH       (-1008)       //发件人身份认证失败或服务器不支持匿名
#define SMTP_ERR_SERVER_ADDR_INVALID (-1009)    //服务器地址不合法
#define SMTP_ERR_ADDRESS_DISABLED    (-1010)    //告警邮件接收地址未使能


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __SMTP_DEFINE_H__ */
