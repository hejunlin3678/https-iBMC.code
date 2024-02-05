/******************************************************************************

                  版权所有 (C), 2001-2013, 华为技术有限公司

 ******************************************************************************
  文 件 名   : ipmc_common.h
  版 本 号   : 初稿
  作    者   : w00192391
  生成日期   : 2013年7月4日
  最近修改   :
  功能描述   : 命令行公共头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年7月4日
    作    者   : w00192391
    修改内容   : 创建文件

******************************************************************************/

#ifndef __IPMC_COMMON_H__
#define __IPMC_COMMON_H__

#include <glib.h>
#include <glib/gprintf.h>
#include "property_method_app.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VOS_OK                                       0
#define VOS_ERR                                      1
//#define OBJ_NAME_MAX_LEN                             (64)  /* 对象名称的最大长度 */
#define PARAMETER_MAX_LEN                            64  /* 参数内容的最大长度 */
#define PROTERY_VALUE_LEN                            32  /* 参数内容的最大长度 */
#define TRAP_VERSION_LEN                             10
#define TRAP_STATE_LEN                               10



/* 类名称和对象名称定义 */
/* User */
#define CLASS_NAME_USER                                    CLASS_USER
#define OBJ_NAME_USER                                      CLASS_USER
#define PROPERTY_USER_PWD                                  PROPERTY_USER_PASSWD
#define PROPERTY_USER_ENABLE                               PROPERTY_USER_ISENABLE
#define METHOD_USER_NAME                                   METHOD_USER_SETUSERNAME
#define METHOD_USER_PASSWORD                               METHOD_USER_SETPASSWORD
#define METHOD_USER_PRIVILEGE                              METHOD_USER_SETPRIVILEGE
/* 密码复杂度 */
#define CLASS_NAME_PWDSETTING                              CLASS_PASSWD_SETTING
#define OBJ_NAME_PWDSETTING                                CLASS_PASSWD_SETTING
#define PROPERTY_PWDSETTING_ENABLESTRONG                   PROPERTY_USER_PASSWD_SETTING
#define METHOD_PWDSETTING_ENABLESTRONG                     METHOD_PASSWD_SETTING_SETENABLEPWDCOMPLEXITY

/* 看门狗 */

/* 板载网卡 */
#define CLASS_NAME_PORT                                    ETH_CLASS_NAME_BUSY_ETH
#define PROPERTY_PORT_MACADDR                              BUSY_ETH_GROUP_ATTRIBUTE_MAC
#define PROPERTY_PORT_LINKSTATUS                           BUSY_ETH_ATTRIBUTE_LINK_STATUS
#define PROPERTY_PORT_CARDTYPE                             BUSY_ETH_ATTRIBUTE_CARD_TYPE
#define PROPERTY_PORT_NUM                                  BUSY_ETH_ATTRIBUTE_PORT_NUM

/* DNS*/         //added by zhangheng
#define CLASS_NAME_NSSETTING                               CLASS_NAME_DNSSETTING
#define PROPERTY_NSSETTING_DOMAINNAME                      PROPERTY_DNSSETTING_DOMAINNAME
#define PROPERTY_NSSETTING_DOMAINSERVER                    PROPERTY_DNSSETTING_PRIMARYDOMAIN
#define PROPERTY_NSSETTING_BACKUPSERVER                    PROPERTY_DNSSETTING_BACKUPDOMAIN
#define PROPERTY_NSSETTING_DNSMODE                         MODE
#define PROPERTY_NSSETTING_DNSETH                          PROPERTY_DNSSETTING_DNSETHGROUP
#define PROPERTY_NSSETTING_DNSSTATE                        PROPERTY_DNSSETTING_DNSSTATE
#define METHOD_NSSETTING_DOMAINSERVER                      METHOD_DNSSETTING_PRIMARYDOMAIN
#define METHOD_NSSETTING_BACKUPSERVER                      METHOD_DNSSETTING_BACKUPDOMAIN
#define METHOD_NSSETTING_DNSMODE                           METHOD_DNSSETTING_DNSMODE
#define METHOD_NSSETTING_DNSETH                            METHOD_DNSSETTING_DNSETH
#define METHOD_NSSETTING_DNSSTATE                         METHOD_DNSSETTING_DNSSTATE
#define METHOD_NSSETTING_DOMAINNAME                        METHOD_DNSSETTING_DOMAINNAME


/* cooling */   //added by zhangheng
#define CLASS_NAME_COOLING                                 COOLINGCLASS
#define OBJ_NAME_COOLING                                   COOLINGCLASS
#define PROPERTY_COOLING_MODE                              COOLING_PROPERTY_MODE
#define PROPERTY_COOLING_TIMEOUT                           COOLING_PROPERTY_TIMEOUT
#define PROPERTY_COOLING_LEVEL                             COOLING_PROPERTY_LEVEL
#define PROPERTY_COOLING_MINLEVEL                          COOLING_PROPERTY_DFTLEVEL
#define PROPERTY_COOLING_TYPE                              COOLING_TYPE

/* 网口 */
#define CLASS_NAME_ETHERNETPORT                            ETH_CLASS_NAME_ETH   /*  */
#define PROPERTY_ETHERNETPORT_TYPE                         ETH_ATTRIBUTE_TYPE
#define PROPERTY_ETHERNETPORT_LINKSTATUS                   ETH_ATTRIBUTE_LINK_STATUS
#define PROPERTY_ETHERNETPORT_MAC                          ETH_ATTRIBUTE_MAC
#define PROPERTY_ETHERNETPORT_NAME                         NAME
#define PROPERTY_ETHERNETPORT_GROUPID                      PROPERTY_COMPONENT_GROUPID
#define PROPERTY_ETHERNETPORT_VLANSTATE                    ETH_ATTRIBUTE_VLAN_STATE
#define PROPERTY_ETHERNETPORT_VLANSTATE                    ETH_ATTRIBUTE_VLAN_STATE


/* IP 设置 */
#define CLASS_NAME_ETHGROUP                                ETH_CLASS_NAME_ETHGROUP   /*  */
#define PROPERTY_ETHGROUP_NETMODE                          ETH_GROUP_ATTRIBUTE_NET_MODE
#define PROPERTY_ETHGROUP_NETMODEFLAG                      ETH_GROUP_ATTRIBUTE_NET_MODE_FLAG
#define PROPERTY_ETHGROUP_TYPE                             ETH_GROUP_ATTRIBUTE_TYPE
#define PROPERTY_ETHGROUP_NCSIPORT                         ETH_GROUP_ATTRIBUTE_NCSI_PORT
#define PROPERTY_ETHGROUP_GROUPID                          ETH_ATTRIBUTE_GROUP_ID
#define PROPERTY_ETHGROUP_IPMODE                           ETH_GROUP_ATTRIBUTE_IP_MODE
#define PROPERTY_ETHGROUP_MACADDR                          ETH_GROUP_ATTRIBUTE_MAC
#define PROPERTY_ETHGROUP_IPADDR                           ETH_GROUP_ATTRIBUTE_IP_ADDR
#define PROPERTY_ETHGROUP_SUBMASK                          ETH_GROUP_ATTRIBUTE_SUB_MASK
#define PROPERTY_ETHGROUP_GATEWAY                          ETH_GROUP_ATTRIBUTE_GATEWAY
#define PROPERTY_ETHGROUP_IPV6MODE                         ETH_GROUP_ATTRIBUTE_IPV6_MODE
#define PROPERTY_ETHGROUP_IPV6ADDR                         ETH_GROUP_ATTRIBUTE_IPV6_ADDR
#define PROPERTY_ETHGROUP_IPV6GATEWAY                      ETH_GROUP_ATTRIBUTE_IPV6_GATEWAY
#define PROPERTY_ETHGROUP_IPV6PREFIX                       ETH_GROUP_ATTRIBUTE_IPV6_PREFIX
#define PROPERTY_ETHGROUP_IPV6LOCALLINK                    ETH_GROUP_ATTRIBUTE_IPV6_LOCAL_LINK
#define PROPERTY_ETHGROUP_VLANSTATE                        ETH_GROUP_ATTRIBUTE_VLAN_STATE
#define PROPERTY_ETHGROUP_VLANID                           ETH_GROUP_ATTRIBUTE_VLAN_ID
#define METHOD_ETHGROUP_IPMODE                             ETH_GROUP_METHOD_SET_IPV4_MODE
#define METHOD_ETHGROUP_IPADDR                             ETH_GROUP_METHOD_SET_IPV4_ADDR
#define METHOD_ETHGROUP_SUBMASK                            ETH_GROUP_METHOD_SET_IPV4_MASK
#define METHOD_ETHGROUP_GATEWAY                            ETH_GROUP_METHOD_SET_IPV4_DEFAULT_GATEWAY
#define METHOD_ETHGROUP_IPV4MASKGATEWAY                   ETH_GROUP_METHOD_SET_IPV4_MASK_GATEWAY
#define METHOD_ETHGROUP_IPV6MODE                           ETH_GROUP_METHOD_SET_IPV6_MODE
#define METHOD_ETHGROUP_IPV6ADDR                           ETH_GROUP_METHOD_SET_IPV6_ADDR
#define METHOD_ETHGROUP_IPV6PREFIX                         ETH_GROUP_METHOD_SET_IPV6_PREFIX
#define METHOD_ETHGROUP_IPV6GATEWAY                        ETH_GROUP_METHOD_SET_IPV6_DEFAULT_GATEWAY
#define METHOD_ETHGROUP_IPV6PREFIXGATEWAY                 ETH_GROUP_METHOD_SET_IPV6_IP_PREFIX_GATEWAY
#define METHOD_ETHGROUP_NCSIPORT                           ETH_GROUP_METHOD_SET_NCSI_PORT
#define METHOD_ETHGROUP_ACTIVEPORT                         ETH_GROUP_METHOD_SET_ACTIVE_PORT
#define METHOD_ETHGROUP_NETMODE                            ETH_GROUP_METHOD_SET_NET_MODE
#define METHOD_ETHGROUP_VLANID                             ETH_GROUP_METHOD_SET_VLAN_ID


/* 上下电控制 */

/* 告警 */
#define CLASS_NAME_WARNING                                 CLASS_WARNING
#define OBJ_NAME_WARNING                                   OBJECT_WARNING
#define PROTERY_WARNING_MINOR                              PROPERTY_WARNING_MINOR_NUM
#define PROTERY_WARNING_MAJOR                              PROPERTY_WARNING_MAJOR_NUM
#define PROTERY_WARNING_CRITICAL                           PROPERTY_WARNING_CRITICAL_NUM

/* ENTITY类的对象及属性 */



/* EVENT DESCP类的对象及属性 */


/* BlackBox */
#define CLASS_NAME_DIAGNOSE                                CLASS_DIAGNOSE_NAME
#define OBJ_NAME_DIAGNOSE                                  OBJ_DIAGNOSE_NAME
#define PROPERTY_DIAGNOSE_BLACKBOXSTATE                    PROPERTY_BLACKBOX_STATE
#define PROPERTY_DIAGNOSE_BLACKBOXPROCESSBAR               PROPERTY_BLACKBOX_PROCESSBAR
#define METHOD_DIAGNOS_SYSTEMCOM                           METHOD_DIAGNOS_GETSYSTEMCOM
#define METHOD_DIAGNOS_PORT80                              METHOD_DIAGNOS_GETPORT80
#define METHOD_DIAGNOS_BLACKBOX                            METHOD_DIAGNOS_GETBLACKBOX


/* BlackBox */

/* TRAP */
#define CLASS_NAME_TRAP                                    CLASS_TRAP_CONFIG
#define OBJ_NAME_TRAP                                      OBJECT_TRAP_CONFIG
#define PROTERY_TRAP_STATE                                 PROPERTY_TRAP_ENABLE
#define PROTERY_TRAP_VERSION                               PROPERTY_TRAP_VERSION
#define PROTERY_TRAP_SEVERITY                              PROPERTY_TRAP_SEND_SEVERITY
#define PROTERY_TRAP_COMMUNITY                             PROPERTY_TRAP_COMMUNITY_NAME
#define METHOD_TRAP_ENABLE                                 METHOD_TRAP_SET_ENABLE
#define METHOD_TRAP_SEVERITY                               METHOD_TRAP_SET_SEND_SEVERITY
#define METHOD_TRAP_VERSION                                METHOD_TRAP_SET_VERSION
#define METHOD_TRAP_COMMINITY                              METHOD_TRAP_SET_COMMUNITY_NAME


#define CLASS_NAME_TRAPITEM                                CLASS_TRAP_ITEM_CONFIG
#define OBJ_NAME_TRAPITEM                                  CLASS_TRAP_ITEM_CONFIG
#define PROTERY_TRAPITEM_INDEX                             PROPERTY_TRAP_ITEM_INDEX_NUM
#define PROTERY_TRAPITEM_DESTSTATE                         PROPERTY_TRAP_ITEM_ENABLE
#define PROTERY_TRAPITEM_PORT                              PROPERTY_TRAP_ITEM_DEST_IPPORT
#define PROTERY_TRAPITEM_ADDR                              PROPERTY_TRAP_ITEM_DEST_IPADDR
#define METHOD_TRAPITEM_ENABLE                             METHOD_TRAP_ITEM_SET_ENABLE
#define METHOD_TRAPITEM_PORT                               METHOD_TRAP_ITEM_SET_IP_PORT
#define METHOD_TRAPITEM_DESTADDR                           METHOD_TRAP_ITEM_SET_IP_ADDR
#define METHOD_TRAPITEM_TRAPTEST                           METHOD_TRAP_ITEM_TRAPTEST

/* SMTP */
#define CLASS_NAME_SMTP                                    CLASS_SMTP_CONFIG
#define OBJ_NAME_SMTP                                      OBJECT_SMTP_CONFIG
#define PROTERY_SMTP_ENABLE                               PROPERTY_SMTP_ENABLE
#define PROTERY_SMTP_SEVERITY                                 PROPERTY_SMTP_SEND_SEVERITY
#define PROTERY_SMTP_LOGINTYPE                               PROPERTY_SMTP_ANONYMOUS_MODE
#define PROTERY_SMTP_LOGINNAME                              PROPERTY_SMTP_LOGIN_NAME
#define PROTERY_SMTP_SERVER                                 PROPERTY_SMTP_SERVER_NAME
#define PROTERY_SMTP_SENDERNAME                               PROPERTY_SMTP_SENDER_NAME
#define METHOD_SMTP_ENABLE                                 METHOD_SMTP_SET_ENABLE
#define METHOD_SMTP_SEVERITY                            METHOD_SMTP_SET_SEND_SEVERITY
#define METHOD_SMTP_LOGINTYPE                                METHOD_SMTP_SET_ANONYMOUS_MODE
#define METHOD_SMTP_LOGINNAME                              METHOD_SMTP_SET_LOGIN_NAME
#define METHOD_SMTP_LOGINPASSWD                                 METHOD_SMTP_SET_LOGIN_PASSWD
#define METHOD_SMTP_SERVER                               METHOD_SMTP_SET_SMTP_SERVER
#define METHOD_SMTP_SENDERNAME                              METHOD_SMTP_SET_SENDER_NAME

#define CLASS_NAME_SMTPITEM                                CLASS_SMTP_ITEM_CONFIG
#define PROTERY_SMTPITEM_INDEX                             PROPERTY_SMTP_ITEM_INDEX_NUM
#define PROTERY_SMTPITEM_ENABLE                         PROPERTY_SMTP_ITEM_ENABLE
#define PROTERY_SMTPITEM_EMAILNAME                              PROPERTY_SMTP_ITEM_EMAIL_NAME
#define PROTERY_SMTPITEM_EMAILDESC                         PROPERTY_SMTP_ITEM_EMAIL_DESC
#define METHOD_SMTPITEM_ENABLE                           METHOD_SMTP_ITEM_SET_ENABLE
#define METHOD_SMTPITEM_EMAILNAME                             METHOD_SMTP_ITEM_SET_EMAIL_NAME
#define METHOD_SMTPITEM_EMAILDESC                             METHOD_SMTP_ITEM_SET_EMAIL_DESC

/* LED */
#define CLASS_NAME_LED                                     LED_CLASS_NAME
#define OBJ_NAME_UID_LED                                   UID_LED //UID等
#define PROTERY_LED_FRUID                                   FRU_ID
#define PROTERY_LED_MODE                                   MODE
#define PROTERY_LED_NAME                                   NAME
#define PROTERY_LED_STATE                                  STATE
#define PROTERY_LED_COLOR                                  COLOR
#define PROTERY_LED_ON_DURATION                      ON_DURATION
#define PROTERY_LED_OFF_DURATION                    OFF_DURATION
#define PROTERY_LED_COLORCAP                               COLOR_CAPABILITIE
#define PROTERY_LED_DEFAULTLCSCOLOR                        DEFAULT_LCS_COLOR
#define PROTERY_LED_DEFAULTOSCOLOR                         DEFAULT_OS_COLOR

/* Sensor */
//sensor name      | value      | unit       | status| lnr       | lc        | lnc       | unc     | uc         | unr        | phys  | nhys
#define CLASS_NAME_ANALOG_SENSOR                            CLASS_THRESHOLD_SENSOR
#define CLASS_NAME_DISCRETE_SENSOR                          CLASS_DISCERETE_SENSOR
#define PROTERY_SENSOR_NUMBER                               PROPERTY_THR_SENSOR_NUMBER
#define PROTERY_SENSOR_NAME                                 PROPERTY_THR_SENSOR_SENSOR_NAME
#define PROTERY_SENSOR_VALUE                                PROPERTY_THR_SENSOR_READING_CONVERT
#define PROTERY_SENSOR_READINGRAW                           PROPERTY_THR_SENSOR_READING_RAW
#define PROTERY_SENSOR_UNIT                                 PROPERTY_THR_SENSOR_UNIT_STR
#define PROTERY_SENSOR_STATUS                               PROPERTY_THR_SENSOR_ASSERT_STATUS
#define PROTERY_SENSOR_CAP                                  PROPERTY_THR_SENSOR_CAPABILITIES
#define PROTERY_SENSOR_TYPE                                 PROPERTY_THR_SENSOR_SENSOR_TYPE
#define PROTERY_SENSOR_TYPESTR                              PROPERTY_THR_SENSOR_SENSOR_TYPESTR
#define PROTERY_SENSOR_TOLERANCE                            PROPERTY_THR_SENSOR_TOLERANCE
#define PROTERY_SENSOR_LNR                                  PROPERTY_THR_SENSOR_LNR_CONVERT
#define PROTERY_SENSOR_LC                                   PROPERTY_THR_SENSOR_LC_CONVERT
#define PROTERY_SENSOR_LNC                                  PROPERTY_THR_SENSOR_LNC_CONVERT
#define PROTERY_SENSOR_UNC                                  PROPERTY_THR_SENSOR_UNC_CONVERT
#define PROTERY_SENSOR_UC                                   PROPERTY_THR_SENSOR_UC_CONVERT
#define PROTERY_SENSOR_UNR                                  PROPERTY_THR_SENSOR_UNR_CONVERT
#define PROTERY_SENSOR_PHYS                                 PROPERTY_THR_SENSOR_P_HYS_CONVERT
#define PROTERY_SENSOR_NHYS                                 PROPERTY_THR_SENSOR_N_HYS_CONVERT
#define PROTERY_SENSOR_READING_MASK                         PROPERTY_THR_SENSOR_READING_MASK
#define PROTERY_SENSOR_READING_STATUS                       PROPERTY_THS_SENSOR_READING_STATUS
#define PROTERY_SENSOR_ASSERTION_MASK                       PROPERTY_THR_SENSOR_ASSERT_MASK
#define PROTERY_SENSOR_DEASSERTION_MASK                     PROPERTY_THR_SENSOR_DEASSERT_MASK

#define PROTERY_SENSOR_ENTITY_ID                            PROPERTY_THR_SENSOR_ENTITY_ID
#define PROTERY_SENSOR_ENTITY_INSTANCE                      PROPERTY_THR_SENSOR_ENTITY_INSTANCE



/* SEL */
#define CLASS_NAME_SEL                                     CLASS_SEL
#define OBJ_NAME_SEL                                       CLASS_SEL
#define PROTERY_SEL_VERSION                                PROPERTY_SEL_VERSION
#define PROTERY_SEL_CURRENTNUM                             PROPERTY_SEL_CURRENT_NUM
#define PROTERY_SEL_MAXNUM                                 PROPERTY_SEL_MAX_NUM
#define PROTERY_SEL_LASTADDTIME                            PROPERTY_SEL_ADD_TIME
#define PROTERY_SEL_LASTDELTIME                            PROPERTY_SEL_DEL_TIME
#define PROTERY_SEL_UNITSIZE                               PROPERTY_SEL_UNIT_SIZE
#define METHOD_SEL_CLEAR                                   METHOD_SEL_CLEAR_SEL  //直接调用，无入参、出参
#define METHOD_SEL_GETEVENTINFO                            METHOD_SEL_GET_EVENT_INFO

/* 上下电 */
/* FRU 上下电 */      //added by zhangheng
#define CLASS_NAME_FRUPAYLOAD                              CLASS_FRUPAYLOAD
#define OBJ_NAME_FRUPAYLOAD                                OBJECT_FRUPAYLOAD
#define PROTERY_FRUPAYLOAD_FRUID                           PROPERTY_FRU_PAYLOAD_FRUID
#define METHOD_FRUPAYLOAD_COLDRESET                        METHOD_COLD_RESET
#define METHOD_FRUPAYLOAD_GRACEFULREBOOT                   METHOD_GRACE_REBOOT

#define CLASS_NAME_CHASSISPAYLOAD                          CLASS_CHASSISPAYLOAD
#define OBJ_NAME_CHASSISPAYLOAD                            OBJECT_CHASSISPAYLOAD
#define PROTERY_CHASSISPAYLOAD_POWEROFFTIME                PROPERTY_PAYLOAD_PWROFF_TM
#define PROTERY_CHASSISPAYLOAD_POWERPOLICY                 PROPERTY_POWER_REST_POLICY
#define PROTERY_CHASSISPAYLOAD_POWERSTATE                  PROPERTY_PAYLOAD_CHASSPWR_STATE
#define METHOD_CHASSISPAYLOAD_POWEROFFTIME                 METHOD_SET_PWR_OFF_TM
#define METHOD_CHASSISPAYLOAD_POWERON                      METHOD_PWR_ON
#define METHOD_CHASSISPAYLOAD_POWEROFF                     METHOD_PWR_OFF
#define METHOD_CHASSISPAYLOAD_POWERPOLICY                  METHOD_SET_PWR_REST_POLICY

#define CLASS_NAME_HOTSWAP                                 CLASS_HOTSWAP
#define OBJ_NAME_HOTSWAP                                   OBJECT_HOTSWAP
#define PROTERY_HOTSWAP_STATE                              PROPERTY_HS_CRU_STATE

enum {
    UPGRADE_MODE_FINISH = 0, //不在升级中
    UPGRADE_MODE_PREPARE = 1,
    UPGRADE_MODE_PROCESS = 2,
    UPGRADE_MODE_ACTIVITY = 3,
    UPGRADE_MODE_BUTT
};

#define COMP_CODE_HPM_ROLL_SUCCESS      0x00
#define COMP_CODE_HPM_ROLL_IN_PROGRESS  0x80
#define COMP_CODE_HPM_ROLL_FAILURE      0x81
#define COMP_CODE_HPM_ROLL_OVERRIDDEN   0x82
#define COMP_CODE_HPM_ROLL_DENY         0x83
#define COMP_CODE_HPM_ROLL_INPOSSIABLE  0xD5

#define OBJ_NAME_SERVICE_WEB                               "WEB"

#define UIP_SYS_CMD_LEN				256

/* BEGIN: Added by z00355831, 2016/11/08   PN:  DTS2016110801517   */
#define EVENT_DESC_LENGTH           1024
/* END:   Added by z00355831, 2016/11/08 */


#define uip_log(level, format, arg...) 

#define uip_return_val_if_fail(expr, val) return_val_do_info_if_fail(expr, val, uip_log(DLOG_ERROR, "return %d", val))

extern gint32 uip_verifiy_subnet_mask_addr(gchar *pAddr);
extern gint32 uip_get_timezone(gint16 *time_zone);
extern gint32 uip_parse_sel_time(guint32 time_value, gchar *time_str, guint32 time_str_len);



#ifdef __cplusplus
}
#endif

#endif /* __IPMC_COMMON_H__ */
