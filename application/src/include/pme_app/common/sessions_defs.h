/* 
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved. 
* Description: 会话管理相关的定义。
* Author: h00282621 
* Create: 2020-3-10 
* Notes: 用于跨模块使用的相关的定义。
*/

#ifndef __SESSIONS_DEFINE_H__
#define __SESSIONS_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define SESSION_STR_MAX_LEN   64
#define MAX_TOKEN_LENGTH  64

#define SESSION_TOKEN_LENGTH_V1  16
#define SESSION_TOKEN_LENGTH_V2  32

#define SESSION_ID_LEN    16
#define SESSION_IP_LEN    63
#define SESSION_USER_NAME_MAX_LEN 320  //ldap: username(255) + @(1) + domainname(64)
#define USER_ROLEID_MAX_LEN 31

/* 设置会话超时时长范围 */
#define SESSION_TIMEOUT_MIN                  300
#define SESSION_TIMEOUT_MAX                  28800

#define EXCLUDE_LOGIN_IP "exclude_login_ip"
#define REDFISH_SESSION_STR_LEN    1024

typedef enum {
    SESSION_NOTTIMEOUT = 0,      // 会话未超时
    SESSION_TIMEOUT = 1,         // 会话超时
    SESSION_KICKOUT = 2,         // 会话被踢出
    SESSION_RELOGIN = 3,         // 会话异地登录
    SESSION_KICKOUT_CHGINFO = 4, // 用户名、权限、密码、使能状态信息被修改时,被自动踢出
}LOGOUT_TYPE;

/* 会话管理结构体 */
typedef struct tag_redfish_session_info {
    guchar            session_type;     // 0:HMM登录 1:BMA登录 0x10:WEBUI登录else:其它
    guchar            authenticateType;   //0:  本地用户， 1:  域账户，2: 跳转用户(如:MM跳转BMC WEB)
    guchar            privilege;//管理员:4、操作员:3、普通用户:2、NoAccess:15
    guchar            local_userid; //本地用户时为userid, 非本地用户时为 -1
    gchar             loginipaddress[SESSION_IP_LEN];
    gchar             loginname[SESSION_USER_NAME_MAX_LEN + 1];
    guint8            user_privilege;
    gchar             user_roleid[USER_ROLEID_MAX_LEN + 1];
    gint32            logintime;
    guint32           lastactivetime;
    gchar             csrf_token[MAX_TOKEN_LENGTH + 1];
    gchar             x_auth_token[MAX_TOKEN_LENGTH + 1];
    gchar             session_id[SESSION_ID_LEN + 1];
    gchar             ssotoken[MAX_TOKEN_LENGTH + 1]; //sso单点登录时随机生成的token，用于单点退出
} REDFISH_SESSION_INFO_S;

/* 框内redfish会话类型, ipmi命令提供, 不进行用户和密码鉴权 */
typedef enum {
    INNER_HMM,
    INNER_BMA,
    INNER_MAX,
    REDFISH_WEBUI = 0x10,
    REDFISH_UNKNOWN = 0xFF
} REDFISH_SESSION_TYPE_E;






#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __SESSIONS_DEFINE_H__ */
