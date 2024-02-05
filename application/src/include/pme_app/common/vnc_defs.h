/* 
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved. 
* Description: VNC管理相关的定义。
* Author: h00282621 
* Create: 2020-3-10 
* Notes: 用于跨模块使用的相关的定义。
*/

#ifndef __VNC_DEFINE_H__
#define __VNC_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MAX_VNC_LAYOUT_NAME_LENGTH  2

#define VNC_KEY_MIN_LEN    1         /*VNC KEY最小长度*/
#define VNC_KEY_MAX_LEN    8         /*VNC KEY最大长度*/
#define VNC_MIN_TIMEOUT    0         /*VNC KEY最小超时时间*/
#define VNC_MAX_TIMEOUT    480      /*VNC KEY最大超时时间*/


/*VNC操作码*/
#define VNC_OK                           0   /*设置成功*/
#define VNC_ERR                          1   /*设置失败*/
#define VNC_KEY_ERR1                     2   /*设置的密码长度不符合要求（1-8位）*/
#define VNC_KEY_ERR2                     3   /*设置的密码不符合复杂度要求*/
#define VNC_ILLEGAL_VALUE_ERR            4   /*设置的值为非法无效值*/
#define VNC_TIMEOUT_ERR1                 5   /*超出有效值范围*/
#define VNC_PORT_CHECK_FAIL              6   /*端口检查失败*/
#define VNC_PORT_VALUE_ERR               7   /*设置的端口号不在有效值范围之内*/
#define VNC_VALIDITY_ERR1                8   /*设置密码有效期错误*/
#define VNC_AUTH_LOCK                    9   /*连续输入多次错误密码,登录锁定*/
#define VNC_WRONG_PERMIT_RULE            10  /*错误的登录规则*/
#define VNC_RESTRICTED                   11  /*登录受限*/
#define VNC_IN_USE                       12  /*VNC正在使用*/
#define VNC_AUTH_FAIL                    13  /*VNC校验失败*/
#define VNC_PWD_EXPIRED                  14  /*VNC密码过期*/
#define VNC_LOGIN_CONFLICT               15  /*VNC登录冲突*/
#define VNC_CONNECT_FAIL                 16  /*VNC连接异常*/

#define VNC_VALIDITY_MIN_VALUE 0
#define VNC_VALIDITY_MAX_VALUE 365

#define VNC_SSL_OFF            0
#define VNC_SSL_ON             1
#define VNC_SUPPORT_OFF        0
#define VNC_SUPPORT_ON         1
#define VNC_PASSWORD_EXPIRED   1 // 密码过期
#define VNC_PASSWORD_UNEXPIRED 0 // 密码未过期

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __VNC_DEFINE_H__ */
