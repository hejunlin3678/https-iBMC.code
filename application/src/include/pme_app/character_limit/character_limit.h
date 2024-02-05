#ifndef __CHARACTER_LIMIT_H__
#define __CHARACTER_LIMIT_H__

/*输入字符限制头文件*/

/*DNS域名称限制字符*/
#define DNS_DOMAINNAME_LIMIT_CHARACTER    ""

/*DNS域名称最大长度*/
#define DNS_DOMAINNAME_MAX_LEN            67

/*DNS域名称最小长度*/
#define DNS_DOMAINNAME_MIN_LEN            0

/*DNS域服务器限制字符*/
#define DNS_DOMAINSERVER_LIMIT_CHARACTER    ""

/*DNS域服务器最大长度*/
#define DNS_DOMAINSERVER_MAX_LEN            65

/*DNS域服务器最小长度*/
#define DNS_DOMAINSERVER_MIN_LEN            0

/*NTP服务器限制字符*/
#define NTP_DOMAINSERVER_LIMIT_CHARACTER    ""

/*NTP域服务器最大长度*/
#define NTP_DOMAINSERVER_MAX_LEN            255

/*NTP域服务器最小长度*/
#define NTP_DOMAINSERVER_MIN_LEN            0

/*LDAP域文件夹限制字符*/
#define LDAP_GROUPFOLDER_LIMIT_CHARACTER   ""

/*LDAP域文件夹最大长度*/
#define LDAP_GROUPFOLDER_MAX_LEN       255

/*LDAP用户域文件夹最大长度*/
#define LDAP_USERFOLDER_MAX_LEN        255

/*LDAP域控制地址限制字符*/
#define LDAP_HOSTADDR_LIMIT_CHARACTER   ""

/*LDAP域控制地址最大长度*/
#define LDAP_HOSTADDR_MAX_LEN          255

/*LDAP用户域限制字符*/
#define LDAP_USERDOMAIN_LIMIT_CHARACTER   ""

/*LDAP用户域最大长度*/
#define LDAP_USERDOMAIN_MAX_LEN          255

/* LDAP用户域最小长度 */
#define LDAP_USERDOMAIN_MIN_LEN    0

/* LDAP用户域点号中间段长度 */
#define LDAP_USERDOMAIN_SECTION_MAX_LEN    63

/*LDAP组域限制字符*/
#define LDAP_GROUPDOMAIN_LIMIT_CHARACTER   ""

/*LDAP组域最大长度*/
#define LDAP_GROUPDOMAIN_MAX_LEN          255

/*LDAP组域最小长度*/
#define LDAP_GROUPDOMAIN_MIN_LEN          0

/*LDAP组名限制字符*/
#define LDAP_GROUPNAME_LIMIT_CHARACTER   ""

/*LDAP组名最大长度*/
#define LDAP_GROUPNAME_MAX_LEN          255

/* LDAP组最大长度, 256为内存余量 */
#define LDAP_GROUPDN_MAX_LEN (LDAP_GROUPDOMAIN_MAX_LEN + LDAP_GROUPFOLDER_MAX_LEN + LDAP_GROUPNAME_MAX_LEN + 256)

/* LDAP用户组最大长度, 256为内存余量 */
#define LDAP_USERDN_MAX_LEN (LDAP_USERFOLDER_MAX_LEN + LDAP_USERDOMAIN_MAX_LEN + 256)

/*LDAP组名最小长度*/
#define LDAP_GROUPNAME_MIN_LEN          0

/* Kerberos SID最大长度 */
#define KRB_GROUPSID_MAX_LEN    255

/*用户名限制字符*/
#define USER_USERNAME_LIMIT_CHARACTER      ":,\\%&<>/\"' "

/*用户名最大长度*/
#define USER_USERNAME_MAX_LEN        16

/*用户名最小长度*/
#define USER_USERNAME_MIN_LEN        0

/*密码限制字符*/
#define USER_PASSWORD_LIMIT_CHARACTER      ""

/*密码最大长度*/
#define USER_PASSWORD_MAX_LEN         20

/*密码最小长度*/
#define USER_PASSWORD_MIN_LEN         0

/*主机名限制字符*/
#define HOST_NAME_LIMIT_CHARACTER    "`~!@#$%^&*()_=+[{]}\\|;:' ,<.>/?\""

/*主机名最大长度*/
#define HOST_NAME_MAX_LEN            64

/*主机名最小长度*/
#define HOST_NAME_MIN_LEN            1

/*SMTP TRAP 团体名限制字符*/
#define SMTP_TRAP_COMMUNITY_LIMIT_CHARACTER   " "

/*SMTP TRAP 团体名最大长度*/
#define SMTP_TRAP_COMMUNITY_MAX_LEN    32

/*SMTP TRAP 团体名最小长度*/
#define SMTP_TRAP_COMMUNITY_MIN_LEN    1

#define SMTP_TRAP_MAX_PORT            65535
#define SMTP_TRAP_MIN_PORT            1
#define SYSLOG_MIN_PORT            0
/*SMTP 用户名限制字符*/
#define SMTP_USERNAME_LIMIT_CHARACTER   ""

/*SMTP 用户名最大长度*/
#define SMTP_USERNAME_MAX_LEN    64

/*SMTP 用户名最小长度*/
#define SMTP_USERNAME_MIN_LEN    1

/*SMTP 密码限制字符*/
#define SMTP_USERPASSWORD_LIMIT_CHARACTER   ""

/*SMTP  密码加密后最大长度*/
#define SMTP_USERPASSWORD_ENC_MAX_LEN    512

/*SMTP  密码最大长度*/
#define SMTP_USERPASSWORD_MAX_LEN    50

/*SMTP  密码最小长度*/
#define SMTP_USERPASSWORD_MIN_LEN    1

/*SMTP 发送者限制字符*/
#define SMTP_SENDERNAME_LIMIT_CHARACTER   ""

/*SMTP  发送者最大长度*/
#define SMTP_SENDERNAME_MAX_LEN    255

/*SMTP  发送者最小长度*/
#define SMTP_SENDERNAME_MIN_LEN    0

/*SMTP 邮件主题限制字符*/
#define SMTP_TEMPLETTOPIC_LIMIT_CHARACTER   ""

/*SMTP 邮件主题最大长度*/
#define SMTP_TEMPLETTOPIC_MAX_LEN    255

/*SMTP 邮件主题最小长度*/
#define SMTP_TEMPLETTOPIC_MIN_LEN    1

/*SMTP 目标邮件地址限制字符*/
#define SMTP_EMAILADDR_LIMIT_CHARACTER   ""

/*SMTP 目标邮件地址最大长度*/
#define SMTP_EMAILADDR_MAX_LEN    255

/*SMTP 目标邮件地址最小长度*/
#define SMTP_EMAILADDR_MIN_LEN    0
#define SMTP_SERVER_MAX_LEN    50
#define SMTP_SERVERADDR_LIMIT_CHARACTER   "`~!@#$%^&*()_=+[{]}\\|;' ,<>/?\""

/*SMTP 邮件说明限制字符*/
#define SMTP_EMAILDEC_LIMIT_CHARACTER   ""

/*SMTP 邮件说明最大长度*/
#define SMTP_EMAILDEC_MAX_LEN    255

/*SMTP 邮件说明最小长度*/
#define SMTP_EMAILDEC_MIN_LEN    0

/*SNMP 只读团体名限制字符*/
#define SNMP_ROCOMMUNITY_LIMIT_CHARACTER   " "

/*SNMP 只读团体名最大长度*/
#define SNMP_ROCOMMUNITY_MAX_LEN    32

/*SNMP 只读团体名最小长度*/
#define SNMP_ROCOMMUNITY_MIN_LEN    1

/*SNMP 团体名超长口令最小长度*/
#define SNMP_LONG_PASSWORD_MIN_LEN    16

/*SNMP 读写团体名限制字符*/
#define SNMP_RWCOMMUNITY_LIMIT_CHARACTER   " "

/*SNMP 读写团体名最大长度*/
#define SNMP_RWCOMMUNITY_MAX_LEN    32

/*SNMP 读写团体名最小长度*/
#define SNMP_RWCOMMUNITY_MIN_LEN    1

/** BEGIN: Added by l00272597, 2017/4/17 PN:DTS2017041113422 */
#define SNMP_STANDARD_SYS_CONTACT_MAX_LEN    255
#define SNMP_STANDARD_SYS_NAME_MAX_LEN       255
#define SNMP_STANDARD_SYS_LOCATION_MAX_LEN   255
/** END:   Added by l00272597, 2017/4/17 */

/*设备位置限制字符*/
#define DEVICE_LOCATION_LIMIT_CHARACTER   ""

/*设备位置最大长度*/
#define DEVICE_LOCATION_MAX_LEN    64

/*设置位置最小长度*/
#define DEVICE_LOCATION_MIN_LEN    0

/*远程管理限制字符*/
#define TELE_MANAGE_LIMIT_CHARACTER   ""

/*远程管理最大长度*/
#define TELE_MANAGE_MAX_LEN    64

/*远程管理最小长度*/
#define TELE_MANAGE_MIN_LEN    0

/* BEGIN: Added for PN:AR0D988348-8024-4f5b-94BE-6B33CA76E4E1  TPM/TCM卡 by l00205093, 2015/11/23 */
/*安全模块字符串最大长度*/
#define SECURITY_MODULE_MAX_STRING_LENGTH 200
/* END:   Added by l00205093, 2015/11/23 */
#define NTP_DOMAIN_LIMIT_CHARACTER  "~`!@#$%^&*()_+={}[]|\\;\"'<>,?/ " // ntp ip地址和域名限制字符
#define NTP_HOSTADDR_LIMIT_CHARACTER ""

#define SYSTEM_MANAGER_INFO_LIMIT_CHARACTER "%"

#endif /* __CHARACTER_LIMIT_H__ */

