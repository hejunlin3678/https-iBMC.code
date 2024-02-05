#ifndef __UIP_USER_H__
#define __UIP_USER_H__

#ifdef __cplusplus
extern "C" {
#endif

enum UIP_USER_ROLL
{
    UIP_USER_COMMON = 2,
    UIP_USER_OPERATOR = 3,
    UIP_USER_ADMIN = 4,
    UIP_USER_CUSTOMROLE1 = 5,
    UIP_USER_CUSTOMROLE2 = 6,
    UIP_USER_CUSTOMROLE3 = 7,
    UIP_USER_CUSTOMROLE4 = 8,
    UIP_USER_NO_ACCESS = 15
};


// 1 登录iMana             admin operate user
// 2 配置iMana             admin operate
// 3 查看用户          admin operate user
// 4 配置用户          admin
// 5 清除数据          admin operate
// 6 服务器控制操作   admin operate
// 7 访问控制台       admin operate user
// 8 预留                        admin operate
// 9 检测告警配置     admin operate
// 10 命令控制台         admin operate
// 11 恢复出厂配置    admin operate
// 12 功耗封顶配置    admin operate
// 13 数据下载         admin operate
// 14 踢人                    admin

enum UIP_USER_PERMIT
{
    UIP_PERMIT_LOGIN_IMANA        = 1,
    UIP_PERMIT_CONFIGURE_IMANA    = 2,
    UIP_PERMIT_VIEW_USER          = 3,
    UIP_PERMIT_CONFIGURE_USER     = 4,
    UIP_PERMIT_CLEAR_DATA         = 5,
    UIP_PERMIT_CONTROL_IMANA      = 6,
    UIP_PERMIT_VISITOR_CONSOLE    = 7,
    UIP_PERMIT_RESERVE            = 8,
    UIP_PERMIT_TEST_TRAP          = 9,
    UIP_PERMIT_CONSOLE_SHELL      = 10,
    UIP_PERMIT_RESTORE_DEFAULT    = 11,
    UIP_PERMIT_POWER_CAPPING      = 12,
    UIP_PERMIT_DOWNLOAD_DATA      = 13,
    UIP_PERMIT_LOGOUT_USER        = 14,
    UIP_PERMIT_DOWNLOAD_BLACKBOX  = 15,
    UIP_PERMIT_BUTT               = 16
};


#define UIP_ACTION_READ                    1
#define UIP_ACTION_WRITE                   2

#define IPMC_USER_PASSWORD_LEN 21
#define IPMC_MAX_USER_NUMBER 18
#define IPMC_CHANNEL_NUMBER_0 0
#define IPMC_CHANNEL_NUMBER_1 1


/*用户管理相关宏定义*/
#define SET_PASSWORD 0X02

/* BEGIN: Added for PN:AR-0000245341-005-004 支持域账号登录CLI  by l00205093, 2016/5/27 */
#define LDAP_USER_FILE "/dev/shm/uip_ldap_users"
#define LDAP_USER_NAME_LEN 300
#define LDAP_USER_FROM_IP 128
#define LDAP_USER_ROLE_ID_NUM 32
typedef struct _LDAP_USER
{
    guint8 used;  // 0 未使用; 1 已使用
    gint32 uid;
    gchar username[LDAP_USER_NAME_LEN];
    gchar fromip[LDAP_USER_FROM_IP];
    guint8 serverid;
    guint8 groupid;
    guint8 privilege;
    guint8 roleid[LDAP_USER_ROLE_ID_NUM];
}LDAP_USER;
/* END:   Added by l00205093, 2016/5/27 */

extern gint32 uip_get_privilege(const gchar* user_name, guchar * user_privilege);
extern gint32 uip_check_user_authority(const gchar* user_name, guchar function, guchar action, gboolean * permit);
extern gchar * uip_get_user_name_by_id(gint32 id);
extern gint32 uip_get_isenable_by_id(gint32 id, guchar * enable);

extern gint32 uip_get_privilege_by_id(gint32 id, guchar * priviege);

extern gint32 uip_get_user_handle_by_name(const gchar* user_name, OBJ_HANDLE * user_handle);
extern gint32 uip_set_user_name(guchar user_id, const gchar* user_name);

extern gint32 uip_set_user_password(guchar user_id, const gchar* passwd);
extern gint32 uip_set_user_enable(guchar user_id);
extern gint32 uip_set_user_disable(guchar user_id);
extern gint32 uip_set_user_privilege(guchar user_id, guchar privilege);

gint32 uip_set_password_complexity(guchar enable_state);

/* BEGIN: Added by huanghan (h00282621), 2017/4/9   问题单号:AR-0000276587-001-001*/
extern gint32 uip_check_lock_status(const gchar* user_name, guint8* lock_state);
/* END:   Added by huanghan (h00282621), 2017/4/9 */

/* BEGIN: Modified by maoali, 2015/8/3   PN:AR-0000953547 登陆规则增强*/
extern gint32 uip_check_login_limit_status(guint8 type, const gchar* user_name, const gchar * ip_addr);
extern gint32 uip_ibmc_check_password_status(const gchar* user);
/* END:   Modified by maoali, 2015/8/3 */
/* BEGIN: Added for PN:AR-0000245341-007-001 支持用户最小化访问接口 by l00205093, 2016/4/17 */
extern guint32 uip_is_login_interface_enable(const gchar* username, guint32 offset);
/* END:   Added by l00205093, 2016/4/17 */
/* BEGIN: Added for PN:AR-0000245341-005-004 支持域账号登录CLI  by l00205093, 2016/5/27 */
extern gint32 uip_auth_ldap_user(const gchar* user_name, const gchar* pw, const gchar* ip_addr,
                          guint8* out_privilege, guint8* out_server_id, guint8* out_group_id, guint8 out_roleid[LDAP_USER_ROLE_ID_NUM],
                          guint8* need_domain, gchar* out_domain, gint32 out_domain_len);
extern guint32 uip_is_ldap_login_interface_enable(const guint8 serverid, const guint8 groupid, guint32 offset);
extern gint32 uip_alloc_ldap_uid(void);
extern gint32 uip_read_ldap_user(gint32 uid, LDAP_USER* usr);
extern gint32 uip_renew_ldap_user(LDAP_USER* usr);
/* END:   Added by l00205093, 2016/5/27 */

extern gint32 uip_get_product_version(guint32* ver);

/* BEGIN: AREA02109777-001-001. Modified by f00381872, 2017/9/19 */
extern gint32 uip_update_user_inact_starttime(const gchar * user_name);
/* END: AREA02109777-001-001. Modified by f00381872, 2017/9/19 */
extern gint32 uip_get_uid_by_user_info(const gchar* user_name, const gchar* ipaddr, guint8 server_id, guint8 group_id, gint32* uid);

extern gint32 uip_is_need_reset_passsword(const gchar *user_name);

#ifdef __cplusplus
}
#endif

#endif
