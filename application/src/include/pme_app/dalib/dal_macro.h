/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : dal_macro.h
  版 本 号   : 初稿
  作    者   : m00168195
  生成日期   : 2016年4月12日
  最近修改   :
  功能描述   : dal库对外提供宏定义【重要】:此头文件只定义对外提供的宏定义
  函数列表   :
  修改历史   :
  1.日    期   : 2016年4月12日
    作    者   : m00168195
    修改内容   : 创建文件

******************************************************************************/
#ifndef __DAL_MACRO__H__
#define __DAL_MACRO__H__

#define XML_PROPERTY_TYPE_y     1
#define XML_PROPERTY_TYPE_n     2
#define XML_PROPERTY_TYPE_q     3
#define XML_PROPERTY_TYPE_i     4
#define XML_PROPERTY_TYPE_u     5
#define XML_PROPERTY_TYPE_d     6
#define XML_PROPERTY_TYPE_s     7
#define XML_PROPERTY_TYPE_aq    8
#define XML_PROPERTY_TYPE_as    9
#define XML_PROPERTY_TYPE_ay    10

#define CPU_MEM_HOT_MINOR_LANG_STR "lang_d0100003B_str"
#define CPU_MEM_HOT_MINOR_DE_LANG_STR "lang_d0100003C_str"
#define CPU_MEM_HOT_MAJOR_LANG_STR  "lang_d0100003D_str"
#define CPU_MEM_HOT_MAJOR_DE_LANG_STR "lang_d0100003E_str"
#define CPU_AEP_HOT_MINOR_LANG_STR "lang_d01000045_str"
#define CPU_AEP_HOT_MINOR_DE_LANG_STR "lang_d01000046_str"

#define PROP_BYTE_TYPE      0
#define PROP_INT16_TYPE     1
#define PROP_UINT16_TYPE    2
#define PROP_INT32_TYPE     3
#define PROP_UINT32_TYPE    4
#define PROP_STRING_TYPE    5

/* BEGIN: Added by z00352904, 2015/11/30 16:41:19   问题单号:DTS2015113004880  重复代码清理 */
#define PWD_PASS_SPECIAL "`~!@#$%^&*()-_=+|\\[{}];:'\",<.>/? " // 有点问题
#define NULLKEY          "x"                                   // 空用户密码

#define free_mem_set_null(data) \
    do {                        \
        {                       \
            g_free(data);       \
            data = NULL;        \
        }                       \
    } while (0)
/* END:   Added by z00352904, 2015/11/30 16:41:22 */

#define DAL_RE_SPLIT_MAX_LEN 512
#define DAL_RE_SPLIT_MAX_NUM 10

#define CERT_INFO_LEN     (512 + 1)
#define CERT_ITEM_LEN_128 (128 + 1)
#define CERT_ITEM_LEN_64  (64 + 1)
#define CERT_ITEM_LEN_2   (2 + 1)
#define SFC_FLASH_ID_LEN   8

#define ETH_DRV_PHY_ID_GET 0x0000000b

#pragma pack(1)
typedef struct tag_cert_info {
    gchar country[CERT_ITEM_LEN_2];       // 2
    gchar state[CERT_ITEM_LEN_128];       // 128 ub_state_name
    gchar location[CERT_ITEM_LEN_128];    // 128 ub_locality_name
    gchar org[CERT_ITEM_LEN_64];          // 64 ub_organization_name
    gchar org_unit[CERT_ITEM_LEN_64];     // 64 ub_organization_unit_name
    gchar common_name[CERT_ITEM_LEN_64];  // 64 ub_common_name
    gchar email[CERT_ITEM_LEN_128];       // 128 ub_email_address
    gchar serial[CERT_ITEM_LEN_64];       // 64 ub_serial_number
    gchar not_before[CERT_ITEM_LEN_128];  
    gchar not_after[CERT_ITEM_LEN_128];   
    gchar subject_info[CERT_INFO_LEN];    // 使用者信息
    gchar issuer_info[CERT_INFO_LEN];     // 颁发者信息
    gchar key_usage[CERT_INFO_LEN];    // 秘钥用法
    gchar sig_algo[CERT_ITEM_LEN_64];     // 签名算法
    guint32 key_len;                // 秘钥长度
    gchar hash_val[CERT_ITEM_LEN_64]; // 哈希值
    gchar fingerprint_sha256[CERT_ITEM_LEN_64]; // SHA256计算的证书指纹
} CERT_INFO_T;

typedef struct tag_crl_info {
    gchar start_time[CERT_ITEM_LEN_128]; // 有效期开始时间
    gchar expire_time[CERT_ITEM_LEN_128]; // 有效期结束时间
} CRL_INFO_T;

typedef struct eth_drv_phy_id_get {
    guint32 ulCmd;
    guint32 phyid;
}ETH_DRV_PHY_ID_GET_S;

typedef enum csr_req_index_type {
    CSR_REQ_INDEX_COUNTRY = 0, /* 枚举类型定义本身无意义，完全是为了snmp编码方便 */
    CSR_REQ_INDEX_STATE,
    CSR_REQ_INDEX_LOCATION,
    CSR_REQ_INDEX_ORG_NAME,
    CSR_REQ_INDEX_ORG_UNIT,
    CSR_REQ_INDEX_COMMON_NAME,
    CSR_REQ_INDEX_BUTT
} CSR_REQ_INDEX_ENUM;

enum SSL_COMMON_ERRCODE {
    SSL_SUCCESS,
    SSL_UNKNOWN_ERROR = 0x00020000,             // 未知错误
    SSL_INVALID_PARAMETER = 0x00020001,         // 无效参数
    SSL_SENSOR_NOT_EXIST = 0x00020003,          // 此传感器不存在
    SSL_INVALID_DATA = 0x00020004,              // 无效数据
    SSL_SCANNING_DISABLE = 0x00020005,          // 禁止扫描
    SSL_SDR_INITIAL_FAILED = 0x00020006,        // SDR初始化失败
    SSL_INITIAL_FAILED = 0x00020007,            // 初始化失败
    SSL_IPMI_MESSAGE_SEND_FAILED = 0x00020008,  // 消息请求发送失败
    SSL_INVALID_USERNAME = 0x00020009,          // 无效的用户名
    SSL_USER_NOT_EXIST = 0x0002000A,            // 该用户不存在
    SSL_FUNCTION_NOT_SUPPORT = 0x0002000B,      // 功能不支持
    SSL_DEVICE_NOT_PRESENT = 0x0002000C,        // 设备不在位
    SSL_INTERNAL_ERROR = 0x0002000D,            // 内部错误
    SSL_CMD_RESPONSE_TIMEOUT = 0x0002000E,      // 命令响应超时
    SSL_NO_DATA = 0x0002000F,                   // 无数据,数据为空
    SSL_WEAK_CA_MD = 0x00020010,                 // 加密方式太弱--MD5
    SSL_WEAK_CA_SHA1 = 0x00020011                // 加密方式太弱--SHA1
};

#pragma pack()
#define MAX_TMP_STRING_LEN  2000
#define CERT_MAX_SIZE       (100 * 1024)  // 证书的最大大小限制
#define CERT_TMP_PATH       "/tmp/"
#define DIGIT_CHARACTER(ch) ((ch) >= '0' && (ch) <= '9')
/* BEGIN: AREA02109379-001-003. Modified by f00381872, 2017/8/14 */
#define HEX_CHARACTER(ch) (((ch) >= 'a' && (ch) <= 'f') || ((ch) >= 'A' && (ch) <= 'F'))
/* END: AREA02109379-001-003. Modified by f00381872, 2017/8/14 */
#define HOST_ADDR_TYPE_IPV4   0
#define HOST_ADDR_TYPE_IPV6   1
#define HOST_ADDR_TYPE_DOMAIN 2
#define HOST_ADDR_MAX_LEN     255
/* ipv4地址由数字和点号组成，ipv6地址由数字和冒号组成 */
#define DIGIT_FLAG (1 << 0) /* 数字 */
#define DOT_FLAG   (1 << 1) /* 点号 */
#define COLON_FLAG (1 << 2) /* 冒号 */
/* BEGIN: AREA02109379-001-003. Modified by f00381872, 2017/8/14 */
#define HEX_FLAG (1 << 3) /* 16进制字母 */
/* END: AREA02109379-001-003. Modified by f00381872, 2017/8/14 */
#define OTHER_FLAG (1 << 4) /* 其他字符 */
/* BEGIN: 2019-2-22 tianpeng twx572344 PN:DTS2019022200813 */
#define KEYTABLE_MAX_SIZE (1024 * 1024)  // 秘钥表限制大小为1M
/* END:   2019-2-22 tianpeng twx572344 PN:DTS2019022200813 */

/* 串口环回检测相关 */
#define COM_DEBUG_PORT       2
#define UART_DEBUG_CHAN      UART2
#ifdef ARM64_HI1711_ENABLED
#define UART_NOTUSED_CHAN    UART7
#else
#define UART_NOTUSED_CHAN    UART4
#endif
#define UART_DEBUG_BUAD_RATE 115200
#define MAX_SERIAL_BUF_SIZE  20
#define UART7                7

/* BEGIN: fdm3.0,日志优化. Added by j00298878, 2016/12/11 */
#define MAX_SINGLE_FDM_OUT_LENGTH  (4 * 1024) // 最大不能超过8k
#define MAX_SINGLE_FDM_LINE_LENGTH (1 * 1024) // 最大不能超过1k
#define MAX_TIME_STRING_LEN        128

/*nameserver的最大数*/
#define  DNS_SERVER_CNT 3

/* DNS服务器索引值 */
#define PREFERRED_DNS_SERVER_INDEX  0
#define ALTERNATE_DNS_SERVER_INDEX  1
#define TERTIARY_DNS_SERVER_INDEX   2
typedef enum tag_fdm_log_type {
    FDM_LOG_TYPE_NONE = 0,
    FDM_LOG_TYPE_EVENT,    // 系统事件， 上下电、caterror，内存ras信息等
    FDM_LOG_TYPE_PROCESS,  // 诊断过程 , 诊断工作的流程
    FDM_LOG_TYPE_RESULT,   // 诊断结果 ，故障FRU 信息
    FDM_LOG_TYPE_OUTPUT,   // 诊断输出，code码等
    FDM_LOG_TYPE_MAX,
} FDM_LOG_TYPE;

typedef enum tag_fdm_log_level {
    FDM_LOG_LV_NONE = 0,
    FDM_LOG_LV_NORNAL,
    FDM_LOG_LV_MINOR,
    FDM_LOG_LV_MAJOR,
    FDM_LOG_LV_CRTITICAL,
    FDM_LOG_LV_MAX,
} FDM_LOG_LEVEL;

/* END: fdm3.0,日志优化. Added by j00298878, 2016/12/11 */

// cifs://user:pass@192.168.27.25/cifs/image.iso
#define MAX_URL_INFO_STRING_LEN 256
typedef struct url_struct_info_s {
    gchar protocol[MAX_URL_INFO_STRING_LEN];
    gchar username[MAX_URL_INFO_STRING_LEN];
    gchar pw_name[MAX_URL_INFO_STRING_LEN];
    gchar serverip[MAX_URL_INFO_STRING_LEN];
    gchar portnum[MAX_URL_INFO_STRING_LEN];
    gchar filepath[MAX_URL_INFO_STRING_LEN];
    gchar filename[MAX_URL_INFO_STRING_LEN];
} URL_STRUCT_INFO_S;

/* BEGIN: added by h00256973  2019/06/25  for 1711 BMC */
#ifdef ARM64_HI1711_ENABLED
typedef struct tag_secureboot_info_s {
    guint16 boot_mode;
    guint16 boot_partition;
    guint16 recover_status;
} SECURE_BOOT_INFO_S;
#endif

#ifndef TPCM_LOG_MAX_LEN
#define TPCM_LOG_MAX_LEN (1024*2)  /* TPCM日志最大长度, 当前长度设置为2*1024byte，注意最大应当小于4K */
#endif
typedef struct tag_tpcm_info_s {
    guint16 			resp_code;
    guint8 			    data[TPCM_LOG_MAX_LEN];
    guint32             len;
} TPCMInfo;

typedef struct sfc_flash_id {
    guint8 cs0_id[SFC_FLASH_ID_LEN];
    guint8 cs1_id[SFC_FLASH_ID_LEN];
}SFC_FLASH_ID_S;

#define DLOG_LIMIT_60               60
#define DLOG_LIMIT_30               30
#define DLOG_LIMIT_10               10
#define DLOG_LIMIT_3MIN             180
#define DLOG_LIMIT_10MIN            600
#define DLOG_LIMIT_30MIN            1800
#define DLOG_LIMIT_1H               3600
/* Description: 当failed为TRUE时打印日志并限制频率，最大每limit秒打印一次 */
#define debug_log_limit_when_fail(limit, failed, format, arg...)                    \
    do {                                                                            \
        static gulong last_log_at = 0;                                              \
        if (!(failed)) {                                                            \
            last_log_at = 0;                                                        \
        } else if ((vos_tick_get() - last_log_at) > ((limit) * 1000)) {             \
            debug_log(DLOG_ERROR, format, ##arg);                                   \
            last_log_at = vos_tick_get();                                           \
        }                                                                           \
    } while (0)

#define ipmi_operation_log_limit(limit, reg_msg, format, arg...)                    \
    do {                                                                            \
        static gulong last_log_at = 0;                                              \
        if ((vos_tick_get() - last_log_at) > ((limit) * 1000)) {                    \
            ipmi_operation_log(reg_msg, format, ##arg);                             \
            last_log_at = vos_tick_get();                                           \
        }                                                                           \
    } while (0)
/* END: added by h00256973  2019/06/25  for 1711 BMC */

#endif /* __DAL_MACRO__H__ */
