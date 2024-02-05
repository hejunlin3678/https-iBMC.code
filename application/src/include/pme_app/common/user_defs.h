/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved.
 * Description: 用户管理相关的定义。
 * Author: h00282621
 * Create: 2020-3-10
 * Notes: 用于跨模块使用的相关的定义。
 */

#ifndef __USER_DEFINE_H__
#define __USER_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define RESERVED_ROOT_USER_NAME "<root>" // 用于支持非超级用户root
#define ACTUAL_ROOT_USER_NAME "root"


/* ******用户ID、用户名、口令长度范围定义****** */
#define USER_ID_BASE 500          // Linux用户ID基数
#define USER_PASS_MIN_LEN 8    // 用户密码复杂度检查至少长度
#define USER_PASSWORD_CMCC_CUSTOMIZED_MIN_LEN 12 // 移动定制化密码最小长度
#define USER_PASSWORD_SIZE_2 20   // IPMI2.0支持的密码最大长度
#define USER_PASSWORD_SIZE_1_5 16 // IPMI1.5支持的密码最大长度
#define PWSWORDMINLENGH 8         // 密码最小长度，不带结束符
#define PWSWORDMAXLENGH 20        // 密码最大长度，不带结束符
#define USER_MAX_USER_NUMBER 18   // 最大IPMI用户个数
#define USER_MIN_USER_NUMBER 2    // 最小IPMI用户个数
#define MIN_USER_ID 2             // 最大IPMI用户ID
#define MAX_USER_ID 17            // 最小IPMI用户ID

#ifdef BOARD_WITH_EBMC
#define USER_NAME_MAX_LEN 32 // ebmc需要支持32位长度用户
#else
#define USER_NAME_MAX_LEN 16 // 不带结束符的最大用户名长度
#endif

#define USER_PASSWORD_LEN 21 // 带结束符的最大密码长度

#define MAX_USER_PASSWD_LEN 20
#define USER_PWD_MAX_LEN 20

typedef enum {
    FIRST_LOGIN_PLCY_NULL = 0,      // 无效的口令修改策略，仅预置账户未确定策略时为该值
    FIRST_LOGIN_PLCY_PROMPT = 1,    // 仅提示但不强制要求修改
    FIRST_LOGIN_PLCY_FORCE = 2,     // 强制用户修改
} FIRST_LOGIN_PLCY;
#ifndef SECURITY_ENHANCED_COMPATIBLE_BOARD_V3
    #define DEFAULT_FIRST_LOGIN_PLCY FIRST_LOGIN_PLCY_FORCE
    #define DEFAULT_FIRST_M2MLOGIN_PLCY FIRST_LOGIN_PLCY_PROMPT
#else
    #define DEFAULT_FIRST_LOGIN_PLCY    FIRST_LOGIN_PLCY_NULL
    #define DEFAULT_FIRST_M2MLOGIN_PLCY FIRST_LOGIN_PLCY_NULL
#endif

#define FIRST_LOGIN_PLCY_PROMPT_STR "PromptPasswordReset"
#define FIRST_LOGIN_PLCY_FORCE_STR  "ForcePasswordReset"

/* **************用户登陆接口编号*************** */
#define USER_LOGIN_INTERFACE_WEB_OFFSET 0
#define USER_LOGIN_INTERFACE_SNMP_OFFSET 1
#define USER_LOGIN_INTERFACE_IPMI_OFFSET 2
#define USER_LOGIN_INTERFACE_SSH_OFFSET 3
#define USER_LOGIN_INTERFACE_SFTP_OFFSET 4
#define USER_LOGIN_INTERFACE_TELNET_OFFSET 5
#define USER_LOGIN_INTERFACE_LOCAL_OFFSET 6
#define USER_LOGIN_INTERFACE_REDFISH_OFFSET 7
#define USER_LOGIN_INTERFACE_MAX 8

extern gchar *USER_LOGIN_INTERFACE_NAMES[]; // 接口字符串数组

#define USER_LOGIN_INTERFACE_VALUE_MASK 0xDF // 本地用户目前只有7个接口,修改为8个接口(redfish)
// 后插板只支持ipmi local权限bit0-bit7 分别表示"Web", "SNMP", "IPMI","SSH", "SFTP", "", "Local", "Redfish"
#define USER_LOGIN_IPMI_LOCAL_VALUE_MASK (1 << USER_LOGIN_INTERFACE_IPMI_OFFSET | 1 << USER_LOGIN_INTERFACE_LOCAL_OFFSET)
#define LDAP_LOGIN_INTERFACE_VALUE_MASK 0x89       // LDAP用户目前只有3个接口, bit 0/3/7(redfish)
#define KRB_LOGIN_INTERFACE_VALUE_MASK 0x81         // kerberos用户目前只有1个接口，bit0(web)， 禁止redfish登录
#define USER_LOGIN_INTERFACE_SET_MASK 0xFFFFFF00   // 目前只有7个接口,修改为8个接口(redfish)
#define USER_LOGIN_INTERFACE_DEF_MASK 0xFFFFFFDF   // 默认关闭不安全的telnet接口 bit5=0，其他默认支持
#define USER_LOGIN_INTERFACE_ALL_ENABLE 0xFFFFFFFF // 所有接口都支持, 逃生用户专用

#define USER_LOGIN_INTERFACE_SET_BIT(value, offset) ((value) | ((guint32)(0x1) << (offset)))               // 设置bit
#define USER_LOGIN_INTERFACE_GET_BIT(value, offset) (((value) & ((guint32)(0x1) << (offset))) >> (offset)) // 读取bit
#define USER_LOGIN_INTERFACE_CLR_BIT(value, offset) ((value) & (~((guint32)(0x1) << (offset))))            // 清空bit

/* 用户登陆接口使能状态 */
#define USER_LOGIN_INTERFACE_ENABLE 1
#define USER_LOGIN_INTERFACE_DISABLE 0

#define USER_LOGIN_INTERFACE_IPMI_STRING "IPMI"
#define USER_LOGIN_INTERFACE_WEB_STRING "WEB"
#define USER_LOGIN_INTERFACE_LCD_STRING "LCD"
#define USER_LOGIN_INTERFACE_CLI_STRING "CLI"
#define USER_LOGIN_INTERFACE_REDFISH_STRING "Redfish"
#define USER_LOGIN_INTERFACE_WEB_REDFISH_STRING "Redfish-WebUI"
#define USER_LOGIN_INTERFACE_INNER_REDFISH_STRING "Redfish-Inner"
#define USER_LOGIN_INTERFACE_GUI_STRING "GUI"
#define USER_LOGIN_INTERFACE_GUI_SSO_STRING "GUI(SSO)"
#define USER_LOGIN_INTERFACE_PANEL "PANEL"
#define USER_LOGIN_INTERFACE_KVM_STRING "KVM"
#define USER_LOGIN_INTERFACE_KVM_SHARED "KVM(Shared)"
#define USER_LOGIN_INTERFACE_KVM_PRIVATE "KVM(Private)"
#define USER_LOGIN_INTERFACE_VNC_STRING     "VNC"
#define USER_LOGIN_INTERFACE_VNC_SHARED     "VNC(Shared)"
#define USER_LOGIN_INTERFACE_VNC_PRIVATE    "VNC(Private)"

/* ***************用户角色定义*************** */
#define UUSER_ROLE_USER 2
#define UUSER_ROLE_OPER 3
#define UUSER_ROLE_ADMIN 4
#define UUSER_ROLE_CUSt1 5
#define UUSER_ROLE_CUSt2 6
#define UUSER_ROLE_CUSt3 7
#define UUSER_ROLE_CUSt4 8
#define UUSER_ROLE_NOACCESS 15

#define USER_ROLE_ADMIN "Administrator"
#define USER_ROLE_OPERATOR "Operator"
#define USER_ROLE_COMMONUSER "Commonuser"
#define USER_ROLE_CUSTOMROLE1 "CustomRole1"
#define USER_ROLE_CUSTOMROLE2 "CustomRole2"
#define USER_ROLE_CUSTOMROLE3 "CustomRole3"
#define USER_ROLE_CUSTOMROLE4 "CustomRole4"
#define USER_ROLE_NOACCESS "Noaccess"


/* 管理员和操作员的Privilege */
#define ADMIN_PRIVILEGE 4
#define OPERATOR_PRIVILEGE 3


/* linux保留用户的组ID */
#define ROOT_USER_GID 0
#define SSHD_USER_GID 74
#define SNMPD_USER_GID 95
#define IPMI_USER_GID 96
#define KVM_USER_GID 97
#define APACHE_GID 98
#define DISCOVERY_USER_GID 100 /* DISCOVERY用户 */
#define COMM_USER_GID 101      /* 降权限app使用的用户 */
#define REDFISH_USER_GID 102
#define APPS_USER_GID 103 
#define OPERATOR_GID 200
#define ADMINISTRATOR_GID 204

/* linux保留用户的ID */
#define ROOT_USER_UID ROOT_USER_GID
#define SNMPD_USER_UID SNMPD_USER_GID
#define IPMI_USER_UID IPMI_USER_GID
#define KVM_USER_UID KVM_USER_GID
#define APACHE_UID APACHE_GID
#define DISCOVERY_USER_UID DISCOVERY_USER_GID
#define COMM_USER_UID COMM_USER_GID
#define REDFISH_USER_UID REDFISH_USER_GID
#define SSHD_USER_UID SSHD_USER_GID
#define ADMINISTRATOR_USER_UID 502

/* 用户名和组名相同 */
#define SSHD_USER_NAME "sshd"
#define SNMPD_USER_NAME "snmpd_user"
#define IPMI_USER_NAME "ipmi_user"
#define KVM_USER_NAME "kvm_user"
#define APACHE_USER_NAME "apache"
#define DISCOVERY_USER_NAME "discovery_user"
#define FTP_USER_NAME "ftp"
#define COMM_USER_NAME "comm_user"
#define REDFISH_USER_NAME "redfish_user"

#define SNMPD_GROUP_NAME SNMPD_USER_NAME
#define IPMI_GROUP_NAME IPMI_USER_NAME
#define KVM_GROUP_NAME KVM_USER_NAME
#define APACHE_GROUP_NAME APACHE_USER_NAME
#define DISCOVERY_GROUP_NAME DISCOVERY_USER_NAME
#define COMM_GROUP_NAME COMM_USER_NAME
#define REDFISH_GROUP_NAME REDFISH_USER_NAME
#define APPS_GROUP_NAME "apps"
#define NOBODY_GROUP_NAME "nobody"

/* ********特定用户类型相关定义******* */
#define LDAP_USER_ID_BASE 1000
#define LDAP_USER_MAX_COUNT 32
#define LDAP_USER_NAME_MAX_LEN 255

#define USER_KRB_POSTFIX "(KRB)"
#define USER_KRB_POSTFIX_LEN 5
#define USER_NAME_CONTAIN_DOMAIN 0
#define USER_NAME_NOT_CONTAIN_DOMAIN 1
#define USER_NAME_NEED_POST_BACK 2


/* ***************登录类型定义************ */
#define LOG_TYPE_LOCAL 0
#define LOG_TYPE_LDAP_AUTO_MATCH 1 // 自动匹配认证
#define LOG_TYPE_LDAP_SERVER1 2
#define LOG_TYPE_LDAP_SERVER2 3
#define LOG_TYPE_LDAP_SERVER3 4
#define LOG_TYPE_LDAP_SERVER4 5
#define LOG_TYPE_LDAP_SERVER5 6
#define LOG_TYPE_LDAP_SERVER6 7
#define LOG_TYPE_LDAP_KRB_SSO 12
#define LOG_TYPE_KRB_SERVER 13
#define LOG_TYPE_SSO_LOCAL 14
#define LOG_TYPE_SSO_LDAP 15
#define LOG_TYPE_AUTO_MATCH 0xfe // 校验范围包括本地用户和LDAP用户

/* ***********用户账户锁定时长*********** */
#define USER_LOCK_TIME_DEFAULT 300   // 5分钟(即300秒)
#define USER_AUTH_FAIL_MAX_DEFAULT 5 // 失败最大次数
#define USER_LOCK 1                  // 已锁定
#define USER_UNLOCK 0                // 未锁定

/* ************用户操作定义************** */
#define DISABLE_RMCP_USER 0
#define ENABLE_RMCP_USER 1
#define OPERATION_DISABLE_USER 0
#define OPERATION_ENABLE_USER 1
#define OPERATION_SET_PASSWD 2
#define OPERATION_TEST_PASSWD 3
#define OPERATION_DEFAULT 4
#define OPERATION_SET_SNMP_PRIVACY_PASSWD 5
#define OPERATION_TEST_SNMP_PRIVACY_PASSWD 6


#define USER_PUBLIC_KEY_TYPE_DELETE 0
#define USER_PUBLIC_KEY_TYPE_FILE 1
#define USER_PUBLIC_KEY_TYPE_TEXT 2


#define USER_MGNT_DISABLE 0
#define USER_MGNT_ENABLE 1


/* ************内部错误码定义************* */
#define UUSER_USERNAME_INVALID (-121)
#define UUSER_NO_PERMISSION (-180)
#define UUSER_USERNAME_TOO_LONG (-181)
#define UUSER_USERNAME_EMPTY (-181)
#define UUSER_USERPASS_TOO_LONG (-182)
#define UUSER_USERPASS_EMPTY (-183)
#define UUSER_LINUX_DEFAULT_USER (-184)
#define UUSER_PASS_COMPLEXITY_FAIL (-186) // 口令复杂度检测


/* **********IPMI接口返回码************* */
#define USER_NAME_EXIST 0x80
#define USER_DONT_EXIST 0x81
#define INVALID_PASSWORD 0x82          /* 用户密码错误 */
#define USER_IS_LOCKED 0x83            /* 用户被锁定 */
#define USER_PASS_COMPLEXITY_FAIL 0x84 /* 口令复杂度检测 */
#define USER_UNLOCK_FAIL 0x85
#define PERMIT_RULE_INVALID 0x86
#define USER_MGNT_OPERATION_DISABLED 0x87 /* 用户管理操作未使能 */
#define USER_NO_ACCESS 0x88               /* 用户无权访问 */
#define USER_PASSWORD_EXPIRED 0x89        /* 口令过期 */
#define USER_LOGIN_LIMITED 0x90           /* 登录受限 */
#define USER_TIME_LIMIT_UNREASONABLE 0x91 /* 密码有效期设置短于密码最短使用期限 */
#define USER_PUBLIC_KEY_FORMAT_ERROR 0x92 /* 公钥格式错误，支持RFC4716，Openssh格式 */
#define USER_CANNT_SET_SAME_PASSWORD 0x93 /* 开启了历史密码检测，还设置历史密码 */
#define USER_SET_PASSWORD_EMPTY 0X94
#define USER_SET_PASSWORD_TOO_WEAK 0X95 /* 密码在弱口令字典 */
#define USER_PASSWORD_NEED_RESET 0x96   // 用户的密码是初始密码或者是其他管理员设置，人机接口使用前必须先重设再使用
#define USER_NODE_BUSY 0xc0
#define USER_COMMAND_NORMALLY 0x00
#define USER_REQ_LENGTH_INVALID 0xC7
#define USER_PARAMETER_OUT_OF_RANGE 0xC9
#define USER_INVALID_DATA_FIELD 0XCC
#define USER_DATA_LENGTH_INVALID 0xC7
#define USER_UNSUPPORT 0xD5
#define USER_DELETE_TRAPV3_USER_UNSUPPORT 0xD7   // 用户被设置为SNMP Trap V3的用户，不能删除
#define USER_DELETE_EXCLUDE_USER_UNSURPPORT 0xD9 // 用户被设置为逃生用户，不能删除
#define USER_LDAP_LOGIN_FAIL 0xDA // LDAP用户登录失败

/* 设置snmp鉴权算法时需要重置鉴权密码和加密密码，此处用于区分哪个密码设置出错 */
#define USER_AUTH_PWD_BASE          0x100
#define USER_PRIVACY_PWD_BASE       0x200

/* **********redfish接口错误码********** */
#define USER_PRIVILEGE_WRONG 9000
#define USER_NAME_RESTRICTED 9001
#define USER_PRIV_RESTRICTED 9003
#define USER_ROLE_RESTRICTED 9004
#define EXCLUDE_USER_WRONG 9005
#define USER_LAST_ADMIN_ERR 9006
#define USER_CREATE_CONFILIC_ERR 9007
#define USER_SET_PASS_COMPLEXITY_CHECK_UNSUPPORT 9008
#define SNMP_MAX_NAME_LEN 64
#define SNMP_MAX_PASSWORD_LEN 127
typedef struct tag_ui_user {
    gshort user_id;                                         /* 用户ID */
    gchar user_name[SNMP_MAX_NAME_LEN + 1];                 /* 用户名 */
    gchar pass_word[SNMP_MAX_PASSWORD_LEN + 1];             /* 用户密码(AES),此处保持明文密码 */
    gchar snmp_privacy_password[SNMP_MAX_PASSWORD_LEN + 1]; /* 加密密码(AES),此处保持明文密码 */
    gchar privilege[3];                                     /* ipmi用户在各个ipmi通道上的权限 */
    gchar flag;                                             /* 是否已经同步 */
    gint gid;                                               /* linux用户组id(暂仅用在linux中) */
    guint8 enable_state;
} UI_MANAGE_USER_S;

/* **********密钥材料更新相关宏********** */
typedef enum {
    USER_ENCRYPTION_COMPONENT_NOT_UPDATED = 0,      // 密钥材料未更新
    USER_ENCRYPTION_COMPONENT_UPDATING = 1,         // 密钥材料更新中
    USER_ENCRYPTION_COMPONENT_UPDATED = 2,          // 密钥材料更新完成
} USER_SECURITY_COMPONENT_UPDATED_STATUS;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __USER_DEFINE_H__ */
