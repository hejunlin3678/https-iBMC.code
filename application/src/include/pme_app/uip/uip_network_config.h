/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : uip_network_config.h
  版 本 号   : 初稿
  作    者   : fuzhengwen 00381872
  生成日期   : 2016年10月21日
  最近修改   :
  功能描述   : uip_network_config.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2016年10月21日
    作    者   : fuzhengwen 00381872
    修改内容   : 创建文件

******************************************************************************/

#ifndef __UIP_NETWORK_CONFIG_H__
#define __UIP_NETWORK_CONFIG_H__

#include <glib.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/*----------------------------------------------*
 * 宏定义                                   *
 *----------------------------------------------*/
#define IP_DN_ADDR_MAX_LEN 128

/*ntp的使能状态宏*/
#define NTP_DISABLED    0
#define NTP_ENABLED     1

/*ntp工作模式宏*/
#define NTP_MODE_MANUAL      1
#define NTP_MODE_DHCPV4      2
#define NTP_MODE_DHCPV6      3

/*ntp身份认证的使能状态*/
#define NTP_AUTH_DISABLED       0
#define NTP_AUTH_ENABLED        1

/*ntp server flag*/
#define NTP_SERVER_ADDR_IPV4    1
#define NTP_SERVER_ADDR_IPV6    2
#define NTP_SERVER_ADDR_DN      3
#define NTP_SERVER_ADDR_NULL    0

/*ntp同步状态*/
#define NTP_SYNC_TIME_DISABLED      0
#define NTP_SYNC_TIME_SUCCESSFUL    1
#define NTP_SYNC_TIME_FAILED        2

/*g_ntpinfo的修改标志*/
#define NTP_PROPERTY_MODIFIED_YES   1
#define NTP_PROPERTY_MODIFIED_ERROR 2
#define NTP_PROPERTY_MODIFIED_NO    3
#define NTP_PROPERTY_FORMAT_ERROR   4

/*组密钥导入状态*/
#define NTP_GROUP_KEY_IMPORTED      1
#define NTP_GROUP_KEY_UNIMPORTED    0

/*时间跳变状态*/
#define SYS_TIME_JUMP_YES           1
#define SYS_TIME_JUMP_NO            0

#define NTP_MODE_BUF_SZIE          10

#define MAX_NTP_SERVER_COUNT 6 // NTP SERVER 的最大个数
#define MAX_EXTRA_NTP_SERVER_COUNT 4 // 除了主备以外的，NTP SERVER 的最大个数
#define MAX_EXTRA_NTP_SERVER_IP_LEN IP_DN_ADDR_MAX_LEN * MAX_EXTRA_NTP_SERVER_COUNT // ntp server ip 数组的最大长度

typedef enum tag_ntp_server_type
{
    NTP_SERVER_PREFERRED,
    NTP_SERVER_ALTERNATIVE,
    NTP_SERVER_THIRD,
    NTP_SERVER_FOURTH,
    NTP_SERVER_FIFTH,
    NTP_SERVER_SIXTH,
    NTP_SERVER_BUTT
}NTP_SERVER_TYPE_E;


/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/

typedef struct ntp_sync_time_status
{
    guint8    enable_status;
    guint8    mode;
    gchar     ntp_server_list[MAX_NTP_SERVER_COUNT][IP_DN_ADDR_MAX_LEN];
    guint8    is_sync_sucessful;        /*1表示成功，0表示失败*/
    guint8    auth_enable;
    guint8    group_key_status;
}NTP_SYNC_TIME_STATUS;

extern gint32 set_ntp_enable_status(const gchar* interface, const gchar* username, const gchar* client_ip, guint8 enable_status);
extern gint32 set_ntp_preferred_server(const gchar* interface, const gchar* username, const gchar* client_ip, const gchar* ppreferred_server);
extern gint32 set_ntp_alternative_server(const gchar* interface, const gchar* username, const gchar* client_ip, const gchar* palternative_server);
extern gint32 set_ntp_extra_server(const gchar* interface, const gchar* username, const gchar* client_ip, const gchar* pextra_server);
extern gint32 set_ntp_server(const gchar* interface, const gchar* username, const gchar* client_ip, const gchar* ppreferred_server, const gchar* palternative_server);
extern gint32 set_ntp_mode(const gchar* interface, const gchar* username, const gchar* client_ip, guint8 mode);
extern gint32 query_sync_time_status(const gchar* interface, const gchar* username, const gchar* client_ip, NTP_SYNC_TIME_STATUS* sync_status);
extern gint32 set_ntp_auth_enable_status(const gchar* interface, const gchar* username, const gchar* client_ip, guint8 auth_type);
extern gint32 import_ntp_group_key(const gchar* interface, const gchar* username, const gchar* client_ip, const gchar* group_key_path);
extern gint32 get_ntp_group_key_status(const gchar* interface, guint8* status);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __UIP_SENSOR_ALARM_H__ */


