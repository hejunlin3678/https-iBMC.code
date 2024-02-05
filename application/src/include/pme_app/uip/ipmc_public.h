#ifndef IPMC_PUBLIC_H
#define IPMC_PUBLIC_H

#ifdef __cplusplus
 #if __cplusplus
extern "C" {
#endif
#endif

#ifdef BD_BIG_ENDIAN

 #define long_by_big_endian(x) x
 #define short_by_big_endian(x) x

#else

#endif

#define MAC_NAME_LEN     128
#define SHORT_STRING_LEN       32
#define LONG_STRING_LEN       1025

/*日志文件的最大容量*/
#define MAX_OPERATE_LOG_FILE_SIZE (100 * 1024L)

/*一条操作日志的最大长度*/
#define MAX_ONE_RECORD_LENGTH 160

/*一页显示的操作日志的最大条数*/
#define ONEPAGE_RECORD_NUMBER 20


/*IPMI命令执行的最大次数*/
#define MAX_SEND_TIME 5

/*fn-cmd   */
#define NETFN_CHASSIS_REQ 0X00
#define NETFN_CHASSIS_RESP 0X01

#define SET_SYSTEM_BOOT_OPTION 0X08
#define GET_SYSTEM_BOOT_OPTIONS 0x09

#define CHASSIS_IDENTIFY 0X04

/* 2. sensor event   */
#define NETFN_SENSOR_EVENT_REQ 0X04
#define NETFN_SENSOR_EVENT_RESP 0X05

#define GET_PEF_PARAMETERS 0x13
#define SET_PEF_PARAMETERS 0x12
#define GET_DEVICE_SDR 0x21
#define GET_DEVICE_SDR_INFO 0x20
#define GET_SDR_RESERVE_REPO 0X22
#define SET_SENSOR_HYSTERESIS 0X24
#define GET_SENSOR_HYSTERESIS 0X25
#define SET_SENSOR_THRESHOLDS 0x26
#define GET_SENSOR_THRESHOLDS 0x27
#define GET_SENSOR_EVENT_ENABLE 0x29
#define GET_SENSOR_EVENT_STATUS 0x2b

#define PEF_ACTION_GLOBAL_CONTROL 2

/* 3. application    */
#define NETFN_APP_REQ 0X06
#define NETFN_APP_RESP 0X07

#define IPMC_SET_USER_ACCESS 0x43
#define IPMC_GET_USER_ACCESS 0x44
#define IPMC_SET_USER_NAME 0x45
#define IPMC_GET_USER_NAME 0x46
#define IPMC_SET_USER_PASSWORD 0x47
#define IPMC_RESET_WATCHDOG 0X22
#define IPMC_SET_WATCHDOG 0X24
#define IPMC_GET_WATCHDOG 0X25
#define BMC_GET_DEVICE_ID 0x01

/* 4. firmware */
#define NETFN_FIRMWARE_REQ 0X08
#define NETFN_FIRMWARE_RESP 0X09

/* 5. storage  */
#define NETFN_STORAGE_REQ 0X0a
#define NETFN_STORAGE_RESP 0X0b

#define CLEAR_SEL 0X47
#define GET_FRU_INFO 0x10
#define GET_FRU_DATA 0x11
#define SET_FRU_DATA 0x12
#define GET_SDR_REPO_INFO 0x20
#define GET_RESERVATION_ID 0x22
#define GET_SDR 0x23
#define GET_SEL_INFO 0x40
#define GET_SEL_ALLOC_INFO 0x41
#define GET_RESERVE_SEL 0x42
#define GET_SEL_ENTRY 0x43
#define GET_SDR_REPOSITORY_TIME 0X28
#define SET_SEL_TIME 0X49
#define SET_UTC_OFFSET 0X5D
#define GET_UTC_OFFSET 0X5C

/* 6. transport  */
#define NETFN_TRANSPORT_REQ 0X0c
#define NETFN_TRANSPORT_RESP 0X0d

#define SET_LAN_CON_PARAMETERS 0x1
#define GET_LAN_CON_PARAMETERS 0x2

/* 7. PICMG   */
#define NETFN_PICMG_REQ 0X2C
#define NETFN_PICMG_RESP 0X2D

#define GET_PICMG_PROPERTIES 0X00
#define GET_LED_PROPERTIES 0X05
#define GET_LED_COLOR_CAPABILITY 0X06
#define SET_LED_STATE 0X07
#define GET_LED_STATE 0X08
#define SET_FAN_LEVEL 0x15
#define GET_FAN_LEVEL 0X16
#define SET_FRUACTIVATION_POLICY 0x0A
#define FRU_CONTROL 0X04
#define SET_FRU_ACTIVATION 0X0C
#define IPMI_QUERY_ROLLBACK_STATUS 0x37
#define IPMI_INITIATE_MANUAL_ROLLBACK 0x38


/* 8. oem    */
#define NETFN_OEM_REQ 0X30
#define NETFN_OEM_RESP 0X31
#define NETFN_PANGEA_OEM 0x32

/*oem cmd    */
#define GET_IPMC_VERSION 0X15
#define GET_MAC_ADDRESS 0X07
#define OEM_CMD_0X90 0X90
#define R3_OEM_CMD 0X91
#define INTERAL_OEM_CMD 0x93

/*oem 0X90 sub_cmd   */
#define WRITE_E_LABLE 0X04
#define READ_E_LABLE 0X05
#define ENABLE_DFT_COMMAND 0X20
#define ENABLE_DFT 0X01
#define DISABLE_DFT 0X00
/*BEGIN: Modified by c00192989, 2012-09-09, PN:DTS2012090900437*/
#define DFT_GET_COMPONENT_INFO 0X29
/*END: Modified by c00192989, 2012-09-09, PN:DTS2012090900437*/

/*oem 0X91 sub_cmd   */
#define SET_FAN_MODE 0X01
#define GET_FAN_MODE 0X02
#define SET_IP_MODE 0X03
#define GET_IP_MODE 0X04
#define FIX_IPADDRESS 0X05
#define COMPONENT_UPGRADE 0X06
#define PORT_80_STATUS 0X07
#define GET_IPMC_HEALTH_STATE 0X08
#define GET_IPMC_HEALTH_EVENT 0X09
#define SET_SERIAL_NUMBER 0X0A
#define GET_SERIAL_NUMBER 0X0B
#define SET_SENSOR_VALUE 0X0C
#define STOP_FEED_WATCHDOG 0X0D
#define GET_DIAGNOSE_INFO 0X0E
#define SET_MACHINE_NAME 0X0F
#define GET_MACHINE_NAME 0x10
#define USER_AUTHENCATION 0X11
#define GET_BOARD_INFO 0X12
#define SET_SERIAL_DIRECTION 0X13
#define GET_BAK_SEL_INFO 0X14

/* BEGIN: Modified by 00176033 ZhouChunQiang, 2011/12/21 */
#define GET_SOL_BLACKBOX_DATA 0x1F
#define SUB_DOWNLOAD_SOL_BLACKBOX 0X00
/* BEGIN: Modified by 00176033 ZhouChunQiang, 2012/2/23 DTS2012021704005 */
#define GET_DOWNLOAD_STATUS     0X01

#define DOWNLOAD_DATA_OK        0x00
#define DOWNLOAD_DATA_RUN       0x01
#define DOWNLOAD_DATA_FAILED    0x02
#define IPMI_OEM_GET_LOCATION_ID 0x1B

#define SET_ROLLBACK 0X15
#define GET_ROLLBACKSTATUS 0X16
#define SET_SERIAL_CONNECTION 0x17
#define GET_SERIAL_CONNECTION 0x18
#define SET_THIRD_SERIAL_CONNECTION 0x19
#define GET_THIRD_SERIAL_CONNECTION 0x1A

/* oem 0x93 sun cmd */
#define IPMC_CONFIGURATION_NCSI_PORT    0x0D
/* BEGIN: Modified by fwx527488, 2018/9/3   PN:DTS2012082800782 */
#define IPMI_OEM_GET_PROPERTY_VALUE   0x36
/* END:   Modified by fwx527488, 2018/9/3 */
#define IPMI_OEM_GET_POWER_SUPPLY_INFO_CMD  0x1E

/* BEGIN: Added by maoali, 2012/9/21 11:26:10   问题单号:DTS2012082800782  */
//SERVICE PARAMETER
#define SERVICE_CONTROL                1
#define SERVICE_PORT                   2
#define SERVICE_SESSION_TIMEOUT        3
#define SERVICE_SESSION_MAXIMUM        4
#define SERVICE_SESSION_ACTIVE_NUM     5
/* BEGIN: add by w00192391, 2012/12/05, DTS2012120507467 */
#define IPMI_OEM_INTERAL_COMMAND               0x93
/* END: add by w00192391, 2012/12/05, DTS2012120507467 */

//
#define SERVICE_FTP     "ftp"
#define SERVICE_SSH     "ssh"
#define SERVICE_KVM     "kvm"
#define SERVICE_VMM     "vmm"
#define SERVICE_CIM     "cim"
#define SERVICE_WSMAN   "wsman"
#define SERVICE_SNMP    "snmp"
#define SERVICE_WEB     "web"
#define SERVICE_WEB_SSL "webssl"
#define SERVICE_LAN     "lan"
#define SERVICE_VIDEO   "video"

#define SERVICE_ENABLED     "enabled"
#define SERVICE_DISABLED    "disabled"
/* END:   Added by maoali, 2012/9/21 11:26:15 */


/* BEGIN: Modified by maoali, 2011/7/18   PN:C01 B02*/
#define SOURCE_COM_HOST   0x0
#define SOURCE_COM_PANEL  0x1
#define SOURCE_COM_3RD    0x2
/***************************************
串口切换定义
 ****************************************/
#define SERIAL_SELECTOR_HOST_SERIAL        0x00
#define SERIAL_SELECTOR_PANEL_CONNECTOR    0x01
#define SERIAL_SELECTOR_BMC_SOL            0x02
#define SERIAL_SELECTOR_THIRD_PARTY        0x03
#define SERIAL_SELECTOR_BMC_DEBUG          0x04
#define SERIAL_SELECTOR_UNCONNECTED        0x05
#define SERIAL_SELECTOR_RAID               0x06
#define SERIAL_SELECTOR_UNKNOWN            0xFF
/* END:   Modified by maoali, 2011/7/18 */
#define GET_PRODUCT_ID 0
#define GET_BOARD_ID 1

/*oem 0X93 sub_cmd   */
#define GET_SCREEN_SNAPSHOT 0x03

#define AUTO_FAN_MODE 0
#define MANUAL_FAN_MODE 1
#define PICMG_ID 0
#define MAX_SERIOAL_NUMBER_LEN 22
#define OEM_FREAM_LEN 18

/*FRU ID*/
#define IPMC_FRU0 0

/*system  area*/
#define SYSTEM_INFO_AREA 6

/*e_lable end of list*/
#define LAST_DATA 0X80
#define MIDDLE_DATA 0X00

/*完成码*/
#define IPMC_COMMAND_NORMALLY 0X00
#define IPMC_NODE_BUSY 0XC0
#define IPMC_INVALID_COMMAND 0XC1
#define IPMC_COMMAND_INVALID 0XC2
#define IPMC_TIMEOUT 0XC3
#define IPMC_OUT_OF_SPACE 0XC4
#define IPMC_INVALID_RESERVATION_ID 0XC5
#define IPMC_REQUEST_DATA_TRUNCATED 0XC6
#define IPMC_DATA_LENGTH_INVALID 0XC7
#define IPMC_FIELD_EXCEEDED 0XC8
#define IPMC_PARAMETER_OUT_OF_RANGE 0XC9
#define IPMC_RETURN_BYRES_ERROR 0XCA
#define IPMC_REQUESTED_ABSENT 0XCB
#define IPMC_INVALID_DATA_FIELD 0XCC
#define IPMC_COMMAND_ILLEGAL 0XCD
#define IPMC_RESPONSE_ERROR 0XCE
#define IPMC_DUPLICATED_REQUEST 0XCF
#define IPMC_RESPONCE_UNPROVIDED1 0XD0
#define IPMC_RESPONCE_UNPROVIDED2 0XD1
#define IPMC_RESPONCE_UNPROVIDED3 0XD2
#define IPMC_DES_UNAVAILABLE 0XD3
#define IPMC_LEVEL_NOT_ENOUGH 0XD4
#define IPMC_UNSUPPORT 0XD5
#define IPMC_PARAMETER_ILLEGAL 0XD6

//增加rollback
#define COMP_CODE_HPM_ROLL_SUCCESS      0x00
#define COMP_CODE_HPM_ROLL_IN_PROGRESS  0x80
#define COMP_CODE_HPM_ROLL_FAILURE      0x81
#define COMP_CODE_HPM_ROLL_OVERRIDDEN   0x82
#define COMP_CODE_HPM_ROLL_DENY         0x83
#define COMP_CODE_HPM_ROLL_INPOSSIABLE  0xD5

/*boot option selector*/
#define BIOS_SET_PROGRESS 0X00
#define BIOS_PROTECT 0X63
#define SHUTDOWN_TIME 0X62
#define BIOS_BOOT_FLAG 0X05

/*lan config selector*/
#define LAN_SET_PROGRESS 0X00
#define LAN_IP_ADDRESS 0X03
#define LAN_IP_ADDR_SRC 0X04
#define LAN_SUBNET_MASK 0X06
#define LAN_DEFAULT_GATEWAY 0X0C
#define LAN_MAC_ADDR 0x05
#define LAN_BMC_LANP_VLAN_ID 0x14
#define LAN_COMMUNITY_STRING 0x10
#define LAN_DESTINATION_TYPE 0x12
#define LAN_DESTINATION_ADDRESSES 0x13
#define LAN_TRAP_PORT 0xC0

/* BEGIN: Added by m00168195, 2013/2/25   问题单号:DTS2013022501690*/
#define LAN_VLAN_ID   0x14
/* END:   Added by m00168195, 2013/2/25 */

/* BEGIN: Modified by qiurenyuan, 2012/6/5   PN:DTS2012060507302*/
/*ncsi */
#define NCSI_PORT_INVALID_STATE   0X80
/* END:   Modified by qiurenyuan, 2012/6/5 */

/* BEGIN: Modified by maoali, 2013/6/20   PN:network Adaptive*/
#define MODE_NAME_LEN       16
#define LAN_MODE_DEDICATE   1 /* 专用 Dedicated */
#define LAN_MODE_SHARED     2 /* 共享 Shared */
#define LAN_MODE_ADAPTIVE   3 /* 自适应 Adaptive */
/* END:   Modified by maoali, 2013/6/20 */

#define MANUFACTURER_ID                     0x0007db
#define MAX_IPv6_LAN                        16
#define DEFAULT_PREFIX                      64
#define INVALID_VAL        0xff   // 初始无效值
/* BEGIN: Added by maoali, 2013/5/6   PN:DTS2013042305950*/

#define USERROLE_USER         2
#define USERROLE_OPERATOR     3
#define USERROLE_ADMIN        4
#define USERROLE_CUST1        5
#define USERROLE_CUST2        6
#define USERROLE_CUST3        7
#define USERROLE_CUST4        8
#define USERROLE_NOACCESS     15

/* END:   Added by maoali, 2013/5/6 */
#define LAN_TRAP_ENABLEMASK 0xC1
#define LAN_TRAP_STATE_SELECTOR 0xC2
#define LAN_TRAP_VERSION_SELECTOR 0xC3
#define LAN_TRAP_SEVERITY_SELECTOR 0xC4
#define LAN_DESTINATION_ADDRESS_COMPATIBLE_SELECTOR 0xD1

#define SELECTOR_TRAP 0x00
#define SELECTOR_SMTP 0x01
/* END:   Modified by maoali, 2011/4/8 */
/*set pef configure*/
#define PEF_SET_PROGRESS 0x00
#define PEF_SET_COMPLETE 0x00
#define PEF_SET_IN_PROGRESS 0x01
#define PEF_SET_COMMIT_WRITE 0x10
#define PEF_SET_RESERVE 0x11

#define IPMC_CLI_ADDRES 0X63
#define IPMC_CLI_LUN 0
#define IPMC_BMC_LUN 0
#define IPMC_BMC_DEFAULT_ADDR 0

#define IPMI_DATA_LEN 32

/*响应帧中数据域的偏移*/
#define IPMI_DATA_OFFSET 6


/*USER PRIVILEGE*/
#define PRIV_UNSPECIFIED 0x0
#define PRIV_CALLBACK 0x1
#define PRIV_USER 0x2
#define PRIV_OPERATOR 0x3
#define PRIV_ADMIN 0x4
#define PRIV_OEM 0x5

#define SYSTEM_MANUFACTURE_NAME 0
#define SYSTEM_NAME 1
#define SYSTEM_VERSION 2
#define SYSTEM_SERIAL_NUMBER 3

/*指示灯最大个数*/
#define IPMC_MAX_LED_NUM 4

#define IPMI_OEM_UNKNOWN 0
#define IPMI_OEM_PICMG 12634
#define IPMI_OEM_HUAWEI 2011
#define DISABLE_CMD 0X02
#define DATA_FILE_PATH(file) "/data/" # file
#define ENV_FILE DATA_FILE_PATH(blacklist)
#define  GENERATE_ERRCODE(arg1, arg2, arg3, arg4) (arg4 | (arg3 << 8) | (arg2 << 16) | (arg1 << 24))

/*错误码*/
#define SEND_MSG_ERROR 0X40000000
#define RECV_MSG_ERROR 0X40000001
#define RSP_DONT_MATCH 0X40000002
#define NO_SDR_RECORD 0X40000003
#define LENGTH_IS_ZERO 0X40000004
#define SENSOR_NO_LINEAR 0X40000005
#define NO_SENSOR_EVENT_TABLE 0X40000006
#define NO_EVENT_DATA_INDEX_TBL 0X40000007
#define INVALID_SENSOR_TYPE 0X40000008
#define INVALID_VALUE_SET 0X40000009
#define VALUE_OUT_RANGE 0X4000000a
#define INVALID_FRU_DATA 0X4000000b
#define BOOT_FLAG_INVALID 0X4000000c
#define MEM_MALLOC_FAIL 0X50000000
#define OPEN_FILE_FAIL 0X50000001
#define OPEN_MUTEX_FILE_FAIL 0X50000002
#define LOCK_FILE_FAIL 0X50000003
#define SYSTEM_CMD_FAIL 0X50000004
#define PTR_IS_NULL 0X50000005
#define INSUFFCIENT_PRIVILEGE_LEVEL 0X60000000

#define FRU_EEAD_ERR 0X70000000
#define FRU_INTERAL_ERR 0X70000001
#define FRU_CHASSIS_ERR 0X70000002
#define FRU_BOARD_ERR 0X70000003
#define FRU_PRODUCT_ERR 0X70000004
#define FRU_MMULTI_ERR 0X70000005

#define CLI_MODULE_PORT 0
#define IPMI_MODULE_PORT 2001
#define RMCP_MODULE_PORT 2002
#define WEB_MODULE_PORT 2003
#define SNMP_TRAP_PORT 2004
#define IPMC_USER_NAME_MAX_LEN 17



#define OPERATOR_LOG_CLI  "CLI"
#define IPMC_ROLE_ID_MAX_LEN 128

#define USB_MGMT_EAR_NOT_PRESENT_LOG "The mounting ear with iBMC directly connected \
management interface is not present.\r\n"

struct valstr
{
    gint32          val;
    const gchar * str;
};

struct cmd_err_str
{
    guchar  fn;
    guchar  cmd;
    struct valstr *error_str;
};

struct cmd_name_str
{
    guchar fn;
    guchar cmd;
    guchar sub_cmd;
    const gchar *  str;
};

typedef struct g_user_pri_info
{
    gchar          username[IPMC_USER_NAME_MAX_LEN];
    guchar user_pri;
} user_privilege_info, *puser_privilege_info;

#define GETDEVICEID_MAXLEN  100

typedef struct output_getdeviceid_t
{
    const guchar arrDeviceid[GETDEVICEID_MAXLEN];
    guchar deviceidlen;

} __attribute__((packed)) OUTPUT_GETDEVICEID_T;

#define OUTPUT_GETDEVICEID_S sizeof(OUTPUT_GETDEVICEID_T)
struct oemvalstr
{
    gint32            oem;
    guint16 val;
    const gchar *   str;
};

typedef struct
{
    guchar  ucSecond;    //秒 0~59
    guchar  ucMinutes;   //分钟0~59
    guchar  ucHours;     //小时0~23
    guchar  ucDay;       //星期1~7
    guchar  ucDate;      //日1~31、30，28润月润年自动计算
    guchar  ucMonth;     //月1~12
    guint16 wYear;
} RtcTime;

typedef struct {
    gboolean input_flag; // 判断是否已输入的标志
    gint32 input_ch; // 输入的字符
    const gchar* prompt; // 输入时的提示字符
} INPUT_CHAR_S;
extern const struct valstr completion_code_vals[];
extern guint32 g_error_code;

extern guchar g_print_level; /*用于打印调试信息*/
#define ALL_DATA 1
#define IPMB_DATA 2
#define COMMON_DATA 3
#define SDR_DATA 4
#define FRU_DATA 5
#define SEL_DATA 6

#define PRINT_IPMB_DATA if ((g_print_level == IPMB_DATA) || (g_print_level == ALL_DATA)) printf
#define PRINT_COMMON_DATA if ((g_print_level == COMMON_DATA) || (g_print_level == ALL_DATA)) printf
#define PRINT_SDR_DATA if ((g_print_level == SDR_DATA) || (g_print_level == ALL_DATA)) printf
#define PRINT_FRU_DATA if ((g_print_level == FRU_DATA) || (g_print_level == ALL_DATA)) printf
#define PRINT_SEL_DATA if ((g_print_level == SEL_DATA) || (g_print_level == ALL_DATA)) printf

#ifndef __min
 #define __min(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef __max
 #define __max(a, b) ((a) > (b) ? (a) : (b))
#endif

const gchar *   ipmc_val_to_str(guchar error_code, guchar fn, guchar cmd);
extern gint32            str_to_int(gchar* str);
extern gulong  str_to_ul(gchar* str);
extern gint32 ipmc_privilege_judge(guchar privilege, const gchar * rolepriv);
extern gint32            get_user_info(void);
extern const gchar *   val_to_str(guint16 val, const struct valstr *vs);
extern RtcTime        gettime_bysecond(guint16 from_year, guint32 s );

extern gint32 str_to_long(glong *v, const gchar *str, glong min, glong max);
extern void print_blade_password_plexity_usage(void);

extern void print_blade_err_info(gchar* location, gint32 err_code);
extern void print_composition_node_id(void);

extern gint32 sort_composition_handle_by_slot(gconstpointer  a, gconstpointer  b);

user_privilege_info* get_user_pri_info(void);
gint32 ipmc_get_gid(gid_t *gid);
gint32 ipmc_get_uid(uid_t *uid);
// 获取父进程的父进程uid
gint32 ipmc_get_grandp_uid(uid_t *uid);
// 获取父进程的父进程gid
gint32 ipmc_get_grandp_gid(gid_t *gid);
// 获取父进程的父进程pid
gint32 ipmc_get_grandp_pid(__pid_t *grand_ppid);
// 获取父进程的uid
gint32 ipmc_get_puid(uid_t *uid);
/* BEGIN: Added by 00356691 zhongqiu, 2017/12/22   PN: AR.SR.SFEA02119723.009.002 支持可编排资源的组合查询*/
#define COMPOSITION_DOMAIN_PREFIX                           "domain"
/* END:   Added by 00356691 zhongqiu, 2017/12/22   PN: AR.SR.SFEA02119723.009.002 支持可编排资源的组合查询*/
gint32 ipmc_get_user_input(const gchar* prompt);
gint32 check_the_input_operation(const gchar* prompt);
gint32 get_component_ps_max_num(guint8 *max_ps_num);
void print_password_complexity_usage(void);
gint32 ipmc_get_user_roleid(gboolean *is_local_user, gchar *role_id, guint32 role_id_len);
void ipmc_delete_file(const gchar *user_name, const gchar *filepath);
#ifdef __cplusplus
 #if __cplusplus
    }
#endif
#endif
#endif
