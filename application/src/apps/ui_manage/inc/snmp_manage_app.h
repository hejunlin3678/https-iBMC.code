#ifndef _SNMPMANAGEAPP_H_
#define _SNMPMANAGEAPP_H_

#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "pme_app/common/public_redfish.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

#define SNMP_RW_USER 0
#define SNMP_RO_USER 1
#define SNMP_USERNAME_MAX_LEN 16
#define SNMP_HEXPASSWORD_MAX_LEN 255
#define SNMP_HEX_USERNAME_LEN (2 * SNMP_USERNAME_MAX_LEN + 4) 
#define SNMP_HEX_PREFIX_LEN 2

#define SNMP_GROUP_MAX_LEN 16
#define SNMP_CONF_MAXLINE_LEN 450
#define SNMP_CONF_MAXLINE_UNTRIMMED (SNMP_CONF_MAXLINE_LEN * 2)
#define SNMP_PRIV_MAX_LEN 6
#define SNMP_USER_MAX_NUM 17
#define SNMP_STATE_DISABLE 0
#define SNMP_STATE_ENABLE 1
#define SNMP_VERSION_V1 0
#define SNMP_VERSION_V2C 1
#define SNMP_VERSION_V3 2
#define SNMP_COMMUNITY_RO 0
#define SNMP_COMMUNITY_RW 1
#define SNMP_VERSION_MAX_LEN 31
#define SNMP_PROPERTY_MAX_LEN 31
#define SNMP_ENGINEID_MAX_LEN 127
#define SNMP_AUTHKYT_MAX_LEN 256
#define SNMP_PID_MAX_LEN 31
#define SNMP_AUTH_LEN 21
#define SNMP_COMMON_SECTION_LEN 127
#define SNMP_ROCOMMIUNITY_IPV4 0
#define SNMP_RWCOMMIUNITY_IPV4 1
#define SNMP_ROCOMMIUNITY_IPV6 2
#define SNMP_RWCOMMIUNITY_IPV6 3
#define SNMP_SERVICE_SLOT 16
#define SNMP_SYS_NAME_OFFSET 8
#define SNMP_SYS_NAME_LEN_OFFSET 7
#define SNMP_SYS_NAME_IPMI_HEAD 4

#define COM2SEC_RWCOMM_DEFAULT "com2sec rwcomm default"
#define COM2SEC_ROCOMM_DEFAULT "com2sec rocomm default"

#define COM2SEC6_RWCOMM_DEFAULT "com2sec6 rwcomm default"
#define COM2SEC6_ROCOMM_DEFAULT "com2sec6 rocomm default"



#define SNMP_PROC_NAME "snmpd"



#define SNMP_TOKENS_PER_LINE 12
#define SNMP_OLDENGINEID_INDEX_LINE 3  // 每行数据中第四个字段为引擎ID
#define SNMP_USERNAMEHEX_INDEX_LINE 4  // 每行数据中第五个字段为用户名
#define SNMP_AUTHPROCOTOL_INDEX_LINE 7 // 每行数据中第七个字段为认证协议
#define SNMP_USERPASSWDHEX_LINE 8      // 每行数据中第九个字段为认证密码
#define SNMP_PRIVPROCOTOL_INDEX_LINE 9 // 每行数据中第十个字段为加密协议

#define SNMP_PRIVPASSWDHEX_LINE 10 // 每行数据中第十个字段为加密密码


#ifdef ARM_PLATFORM
#define SNMP_USER_FILE "/etc/snmp/snmpd.conf"
#define SNMP_PWD_FILE "/etc/snmp/snmpd.conf"
#else
#define SNMP_USER_FILE "./mock/snmpd.conf"
#define SNMP_PWD_FILE "./mock/snmpd.conf"
#endif
#define SNMP_RWUSER "rwuser"
#define SNMP_ROUSER "rouser"
#define SNMP_GROUP "group"
#define SNMP_ERROR_OLDENGINEID "0x80001f880300259ef260f3f28e"

#define REBOOT_SNMP 0
#define STOP_SNMP 1
#define START_SNMP 2
#define SNMP_USER_EXIST 1


#define SNMP_INVALID_DATA_FIELD 0XCC
#define SNMP_USERNAME_NOT_EXIST (-116)

#define SNMP_USERNAME_INVALID (-121)
#define SNMP_NO_PERMISSION (-180)
#define SNMP_USERNAME_TOO_LONG (-181)
#define SNMP_USERPASS_TOO_LONG (-182)

#define SNMP_AUTH_MD5 ".1.3.6.1.6.3.10.1.1.2"
#define SNMP_AUTH_SHA1 ".1.3.6.1.6.3.10.1.1.3"
#define SNMP_AUTH_SHA256 ".1.3.6.1.6.3.10.1.1.5"
#define SNMP_AUTH_SHA384 ".1.3.6.1.6.3.10.1.1.6"
#define SNMP_AUTH_SHA512 ".1.3.6.1.6.3.10.1.1.7"
#define SNMP_PRIV_DES ".1.3.6.1.6.3.10.1.2.2"
#define SNMP_PRIV_AES ".1.3.6.1.6.3.10.1.2.4"
#define SNMP_PRIV_AES256 ".1.3.6.1.4.1.14832.1.4"
#define SMMP_COMMUNITY_ATLEASTLEN 8 // 用户密码复杂度检查至少长度



#define VIEW_OPERATOR "view_operator"
#define GROUP_OPERATOR "group_operator"
#define VIEW_ACCESS_OPERATOUR \
    "view     view_operator  included .1\n\
view     view_operator  excluded .1.3.6.1.6.3.15.1.2.2\n\
view     view_operator  excluded .1.3.6.1.6.3.16\n\
access   group_operator \"\"  usm     priv    exact   view_operator  view_operator  none\n\n"

#define VIEW_USER "view_user"
#define GROUP_USER "group_user"
#define VIEW_ACCESS_USER \
    "view     view_user      included .1\n\
view     view_user      excluded .1.3.6.1.6.3.15.1.2.2\n\
view     view_user      excluded .1.3.6.1.6.3.16\n\
access   group_user     \"\"  usm     priv    exact   view_user      none           none\n\n"


typedef struct tag_snmp_agent_addr {
    gchar ipv4_out[MAX_IPV4_LEN];
    gchar ipv4_bma[MAX_IPV4_LEN];
    gchar ipv4_float[MAX_IPV4_LEN];
    gchar ipv4_maint[MAX_IPV4_LEN]; // FusionPoD近端运维IP
    gchar ipv6_out[MAX_IPV6_LEN];
    gchar ipv6_bma[MAX_IPV6_LEN];
    gchar ipv6_float[MAX_IPV6_LEN];
    gchar ip_local_link[MAX_IPV6_LEN];
    gchar eth_name[STRING_LEN_MAX];
    gint32 port;
} SNMP_AGENT_ADDR_S;

extern GMutex g_snmp_mutex;
extern GMutex g_snmpcomm_mutex;
extern guint8 g_rollback_state;
extern GAsyncQueue *g_snmpSendQueue;
extern volatile gboolean g_rebootSnmp;
extern guchar g_snmpd_enable_flag;
extern guint8 g_snmpcomm_update_state;
extern guchar g_https_enable_flag;
extern GSList *g_revert_owner_list;

typedef struct tag_snmp_user {
    gchar username[SNMP_USERNAME_MAX_LEN + 1];
    gint32 user_priv;
    gchar user_group[SNMP_GROUP_MAX_LEN + 1];
} SNMP_USER_S;

typedef struct tag_snmp_version {
    const gchar *name;
    const gchar *property_name;
} SNMP_VERSION_S;

typedef struct tag_ip_ethname_map_info {
    const gchar *ip;
    const gchar *eth_name;
} IP_ETHNAME_MAP_INFO_S;

typedef struct _file_owner_info_ {
    gchar file_name[MAX_FILEPATH_LENGTH + 1];
    uid_t own_uid;
    gid_t own_gid;
} file_owner_info;

gint32 kill_process_by_pid(gchar *pid, guint8 pid_len);
gint32 check_process_exist_status(gchar *app_name, gchar *pid, gint32 pid_size);
gint32 snmp_start_app_by_pid(void);
gint32 kill_process_by_name(gchar *app_name);
gint32 snmp_start_app_by_name(gchar *app_name);
void app_supervisory_task(void);
gpointer revceive_operate_snmpd_command(gpointer data);
gint32 snmp_configuration_init(void);
gint32 UpdateSnmpPrivacyProtocol(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);
gint32 UpdateSnmpCommunity(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);
gint32 RollbackSnmpCommunity(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);
void snmp_wait_community_rollback(void);
gint32 ipmi_cmd_set_snmp_sys_name(const void *req_msg, gpointer user_data);
gint32 ipmi_cmd_get_snmp_sys_name(const void *req_msg, gpointer user_data);
gint32 SetSnmpAgentAddr(OBJ_HANDLE obj_handle, gpointer user_data, gchar *property_name, GVariant *property_value);
void revert_file_owner_on_exit(void);
gint32 third_party_proc_hashtable_remove(const gchar *proc_kw);
gint32 third_party_proc_check_exist(const gchar *proc_kw);
gint32 third_party_proc_hashtable_insert(gchar *proc_kw, guint32 pid);
gint32 snmp_add_username_info(const gchar *user_name);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
