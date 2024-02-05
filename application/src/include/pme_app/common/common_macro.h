/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : common_macro.h
  版 本 号   : 初稿
  作    者   : m00168195
  生成日期   : 2015年11月16日
  最近修改   :
  功能描述   : 公共模块头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2015年11月16日
    作    者   : m00168195
    修改内容   : 创建文件
  2.日    期   : 20190425
  作    者   : c00426586
  修改内容   : UADP185368 【SP安全需求】修改支持使用文件夹方式升级SP

******************************************************************************/
#ifndef __COMMON_MACRO_H__
#define __COMMON_MACRO_H__
#include <glib-2.0/glib.h>
#include "pme_app/property_method/property_method_product_id.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* ------------------------UT编译---------------------- */
#ifdef UNIT_TEST
#undef FD_ZERO
#undef FD_SET
#undef FD_ISSET
#define FD_ZERO(a) 1
#define FD_SET(a, b) 1
#define FD_ISSET(a, b) 1

#define debug_printf_v2(format, arg...) \
    do                                  \
    {                                   \
        printf(format, ##arg);          \
        printf("\r\n");                 \
    } while (0)

    extern int unit_test_temp_counter;
    extern int unit_test_num_loop;

#undef FOREVER
#define FOREVER unit_test_num_loop--

#undef debug_log
#define debug_log(level, format, arg...) /*lint -save -e506 */  \
    do {                                                        \
        printf("[%s:%d %s]", __FILE__, __LINE__, __FUNCTION__); \
        switch (level) {                                        \
        case DLOG_MASS:                                         \
            printf("[%5s] ", "MASS");                           \
            break;                                              \
        case DLOG_DEBUG:                                        \
            printf("[%5s] ", "DEBUG");                          \
            break;                                              \
        case DLOG_INFO:                                         \
            printf("[%5s] ", "INFO");                           \
            break;                                              \
        case DLOG_ERROR:                                        \
            printf("[%5s] ", "ERROR");                          \
            break;                                              \
        default:                                                \
            break;                                              \
        }                                                       \
        printf(format, ##arg);                                  \
        printf("\r\n");                                         \
    } while (0) /*lint -restore */
#undef method_operation_log
#define method_operation_log(caller_info, target, arg, ...)

#undef operation_log
#define operation_log(interface, username, client, target, fmt, arg...)

#undef strategy_log
#define strategy_log(level, fmt, arg...)

#undef ipmi_operation_log
#define ipmi_operation_log(reg_msg, format, arg...)

#else
#undef FOREVER
#define FOREVER 1

#define debug_printf_v2(format, arg...) \
    do {                                \
        debug_printf(format, ##arg);    \
    } while (0)
#endif

/* --------------------- 函数式宏定义      ------------------- */
#ifndef do_if_fail
#define do_if_fail(expr, val)    if(!(expr)) {val;}
#endif


#ifndef do_if_true
#define do_if_true(expr,val)  if(expr) {val;}
#endif


#ifndef do_if_expr
#define do_if_expr(expr, val)    if(expr) {val;}
#endif

/*expr不成立时，执行info，返回val。info可以是一条或多条C语句，如free(p);g_printf("hello world!\n");*/
#ifndef do_val_if_fail
#define do_val_if_fail(expr, val) do { \
        if (!(expr)){\
            val;} \
    } while (0)
#endif

/*expr成立时，执行info，返回val。info可以是一条或多条C语句，如free(p);g_printf("hello world!\n");*/
#ifndef do_val_if_expr
#define do_val_if_expr(expr, val) do { \
        if (expr){\
            val;} \
    } while (0)
#endif

#ifndef do_info_if_true
#define do_info_if_true(expr,info) do { \
        if(expr){\
            info;\
        }\
    }while(0)
#endif

#ifndef break_if_fail
#define break_if_fail(expr) if (!(expr)) break
#endif

#ifndef break_if_expr
#define break_if_expr(expr) if (expr) break
#endif

#ifndef break_do_info_if_fail
#define break_do_info_if_fail(expr, info) if(!(expr)) {info; break;}
#endif

#ifndef break_do_info_if_expr
#define break_do_info_if_expr(expr, info) if(expr) {info; break;}
#endif

/*expr不成立时，执行info，返回val。info可以是一条或多条C语句，如free(p);g_printf("hello world!\n");*/
#ifndef return_val_do_info_if_fail
#define return_val_do_info_if_fail(expr, val, info) do { \
        if (!(expr)){\
            info; \
            return val;} \
    } while (0)
#endif

/*expr成立时，执行info，返回val。info可以是一条或多条C语句，如free(p);g_printf("hello world!\n");*/
#ifndef return_val_do_info_if_expr
#define return_val_do_info_if_expr(expr, val, info) do { \
        if (expr){\
            info; \
            return val;} \
    } while (0)
#endif

/*expr不成立时，执行info，返回。info可以是一条或多条C语句，如free(p);g_printf("hello world!\n");*/
#ifndef return_do_info_if_fail
#define return_do_info_if_fail(expr, info) do { \
        if (!(expr)){\
            info; \
            return;} \
    } while (0)
#endif

/*expr成立时，执行info，返回。info可以是一条或多条C语句，如free(p);g_printf("hello world!\n");*/
#ifndef return_do_info_if_expr
#define return_do_info_if_expr(expr, info) do { \
        if (expr){\
            info; \
            return;} \
    } while (0)
#endif

// 使用的时候注意是否有资源未释放，有则不能用此宏
#ifndef return_val_if_fail
#define return_val_if_fail(expr, val) if (!(expr)) return val
#endif

#ifndef return_val_if_expr
#define return_val_if_expr(expr, val) if (expr) return val
#endif

#ifndef return_if_fail
#define return_if_fail(expr) if (!(expr)) return
#endif

#ifndef return_if_expr
#define return_if_expr(expr) if (expr) return
#endif

#ifndef goto_if_fail
#define goto_if_fail(expr, target) if (!(expr)) goto target
#endif

#ifndef goto_if_expr
#define goto_if_expr(expr, target) if (expr) goto target
#endif

#define goto_label_do_info_if_fail(expr, label, info) do { \
        if (!(expr)){\
            info; \
            goto label;} \
    } while (0)

#define goto_label_do_info_if_expr(expr, label, info) do { \
        if (expr){\
            info; \
            goto label;} \
    } while (0)


/*
  条件判断
*/
#define check_success_do(expr, some) do{ \
        if ((expr)){ \
            some;} \
    }while(0)

#define check_success_goto(expr, target) if ((expr)) goto target



#define check_success_return_val(expr, val) if ((expr)) return val


#define check_success_do_return_val(expr, val, some) do { \
        if ((expr)){\
            some; \
            return val;} \
    } while (0)


#define check_success_do_and_goto(expr, some, target) do { \
        if ((expr)){\
            (some); goto target;} \
    } while (0)

#define check_fail_return_val(expr, val) if (!(expr)) return val

#define check_fail_do_return_val(expr, val, some) do { \
        if (!(expr)){\
            some; \
            return val;} \
    } while (0)

#define check_fail_do_return_val_spec(expr, val, some1, some2) do { \
        if (!(expr)){\
            some1; \
            some2; \
            return val;} \
    } while (0)

#define check_fail_do_return(expr,some) do { \
        if (!(expr)){\
            some; \
            return;} \
    } while (0)

#define check_fail_return(expr) if (!(expr)) return

#define check_fail_do_goto(expr, target, some) do { \
        if (!(expr)){\
            (some); \
            goto target;} \
    } while (0)

#define check_fail_goto(expr, target) if (!(expr)) goto target

#define check_fail_do_and_goto(expr, some, target) do { \
        if (!(expr)){\
            (some); goto target;} \
    } while (0)

#define check_fail_continue(expr) if (!(expr)) continue

#define check_fail_do(expr, some) do { \
        if (!(expr)){\
            (some);} \
    } while (0)

#ifndef check_fail_do_continue
#define check_fail_do_continue(expr,some) if (!(expr)) {\
        some;\
        continue;\
    }
#endif

#define check_success_continue(expr) if (expr) continue

#define check_success_do_continue(expr,some) if (expr) {\
        some;\
        continue;\
    }

#define continue_do_info_if_fail(expr, info) if(!(expr)) {info; continue;}

#define continue_do_info_if_expr(expr, info) if((expr)) {info; continue;}

#ifndef continue_if_expr
#define continue_if_expr(expr) if((expr)) {continue;}
#endif


#ifndef offsetof
#define offsetof(type, member) ((size_t) &((type *)0)->member)
#endif


#define GET_ARRAY_ITEMS(x) (sizeof(x)/sizeof((x[0])))
#define BMC_GET_BIT_VAL(x) ((x) & 0x01)


//定义位有效码
#define  PBIT0 0x01
#define  PBIT1 0x02
#define  PBIT2 0x04
#define  PBIT3 0x08
#define  PBIT4 0x10
#define  PBIT5 0x20
#define  PBIT6 0x40
#define  PBIT7 0x80
#define  PBIT8 0x100
#define  PBIT9 0x200
#define  PBIT10 0x400
#define  PBIT11 0x800
#define  PBIT12 0x1000
#define  PBIT13 0x2000
#define  PBIT14 0x4000

// 定义位屏蔽码
#define  NBIT0 0xfe
#define  NBIT1 0xfd
#define  NBIT2 0xfb
#define  NBIT3 0xf7
#define  NBIT4 0xef
#define  NBIT5 0xdf
#define  NBIT6 0xbf
#define  NBIT7 0x7f



/* ------------------------平台兼容定义------------------ */

/* 指针与整型变量之间的转换宏定义 */
#define POINTER_TO_INT8(p) ((gint8)(intptr_t)(p))
#define POINTER_TO_INT16(p) ((gint16)(intptr_t)(p))
#define POINTER_TO_INT32(p) ((gint32)(intptr_t)(p))
#define POINTER_TO_UINT8(p) ((guint8)(uintptr_t)(p))
#define POINTER_TO_UINT16(p) ((guint16)(uintptr_t)(p))
#define POINTER_TO_UINT32(p) ((guint32)(uintptr_t)(p))

#define INT8_TO_POINTER(i) ((gpointer)(intptr_t)(i))
#define INT16_TO_POINTER(i) ((gpointer)(intptr_t)(i))
#define INT32_TO_POINTER(i) ((gpointer)(intptr_t)(i))
#define UINT8_TO_POINTER(u) ((gpointer)(uintptr_t)(u))
#define UINT16_TO_POINTER(u) ((gpointer)(uintptr_t)(u))
#define UINT32_TO_POINTER(u) ((gpointer)(uintptr_t)(u))

/* OBJ_HANDLE的格式化参数 */
#ifdef ARM64_HI1711_ENABLED
#define OBJ_HANDLE_FORMAT "lu"
#else
#define OBJ_HANDLE_FORMAT "d"
#endif

/* guint64 16进制输出的格式化参数 */
#ifdef ARM64_HI1711_ENABLED
#define G_GUINT64_HEX_FORMAT "lx"
#else
#define G_GUINT64_HEX_FORMAT "llx"
#endif


/* ------------------------通用定义---------------------- */
/* 使能/禁用 */
#define DISABLE 0
#define ENABLE  1
#define DISABLED 0
#define ENABLED  1

/* 无效数据 */
#define INVALID_DATA_BYTE       0xFF
#define INVALID_DATA_WORD       0xFFFF
#define INVALID_DATA_DWORD      0xFFFFFFFF
#define INVALID_DATA_DDWORD     0xFFFFFFFFFFFFFFFF
#define INVALID_DATA_STRING     "N/A"
#define INVALID_DATA_STRING_VALUE     "Unknown"
#define INVALID_UINT32   0xFFFFFFFF
#define INVALID_UINT16   0xFFFF
#define INVALID_UINT8    0xFF
#define INVALID_DATA_EMP_STR ""

/* 一键收集公共文件权限 */
#define COMMON_DUMP_FILE_UMASK 0640

/* 消息队列读取消息超时时间 */
#define MSG_RECV_TIMEOUT_1S   1000000 // 用于从队列接收数据的超时时间，单位us

#define NUM_0 0
#define NUM_1 1
#define NUM_2 2
#define NUM_3 3
#define NUM_4 4
#define NUM_5 5
#define NUM_6 6
#define NUM_7 7
#define NUM_8 8
#define NUM_9 9
#define NUM_10 10

#define BMC_CHIP_NAME_HI1710           "Hi1710"
#define BMC_CHIP_NAME_HI1711           "Hi1711"
#define BMC_CHIP_TYPE_HI1710           0x00
#define BMC_CHIP_TYPE_HI1711           0X01
#define BMC_CHIP_TYPE_UNKNOWN          0xff


/* PME版本号相关 */
#ifdef ARM64_HI1711_ENABLED
#define PME_VERSION_NUMBER_LENGTH  4     /* AA.BB.CC.DD四个字段 */
#else
#define PME_VERSION_NUMBER_LENGTH  2     /* 1710版本号字段 */
#endif

enum PME_VERSION_SUBSCRIPT{        // 1711新的版本号四个字段
    VERSION_PME_MAJOR     = 0,
    VERSION_PME_MINOR     = 1,
    VERSION_RELEASE_MAJOR = 2,
    VERSION_RELEASE_MINOR = 3
};

#define PRODUCT_VERSION_V5 5
#define PRODUCT_VERSION_V3 3
#define PRODUCT_VERSION_V6 6

#define PRODUCT_ID_KUNPENG_SERVER      0X0a
#define MANUFAC_ID_LEN 3   /* 厂商ID数组长度 */
#define HUAWEI_MFG_ID      0x0007db   /* 华为的厂商   ID */
#define HUAWEI_IANA        0x0007DBUL /* 与上面的ID等同，待优化删除 */
#define OEM_MANUFACTURE_ID 0x0007DBUL /* 与上面的ID等同，待优化删除 */
#define ALI_MANUFACTUE_ID  0xA54C00   /* 阿里的厂商ID */


/* 对象名称/属性名称/方法名称等长度最大值 */
#define OBJ_NAME_MAX_LEN       64   /* 对象名称的最大长度 */
#define MAX_PROPERTY_VALUE_LEN 129  /* 属性值的最大长度 */
#define METHOD_NAME_MAX_LEN    32
#define CONNECTOR_BOM_MAX_LEN  128

/* 路径/文件、命令等长度最大值 */
#define MAX_FILEPATH_LENGTH    256  /* 文件路径最大长度 */
#define FILE_NAME_BUFFER_SIZE  256  /* 与上面的定义类似，待优化删除 */
#define MAX_FILENAME_LENGTH    256  /* 文件名最大长度 */
#define MAX_FILENAME_LEN       200  /* 文件名最大长度 */
#define SQL_STR_MAXLENGTH      3536 /* 拼装SQL语句的最大长度 */
#define MAX_CMD_BUF_LEN        256  /* 拼装shell命令的最大长度 */
#define MAX_CMD_STR_LENGTH     256  /* 拼装shell命令的最大长度 */
#define CMD_MAX_LEN 256   /* shell命令字符串长度 */
#define TMP_STR_LEN 128
#define TMP_STR_MAX 256
#define MAX_WEB_LIMIT_OFFSET 4096
#define USERBUFSIZE 100  // 定义用户提供的buf大小不小于100
#define FULL_PERCENT 100.0

#define MAX_UID_LENGTH 64

/******************************************* BEGIN 错误码定义 *********************************************/
/* 高四位是模块错误号，中间两位是子模块错误号，低两位是错误码*/
/* 缓冲区大小, 尽量复用如下定义 */
#define MAX_BUFF_SIZE       4096
#define MAX_CMD_LENGTH      256
#define MAX_CONFIG_LEN      256
#define MAX_FILE_NAME_LEN   256
#define MAX_LINE_SIZE       512

/* 手动模式风扇最大超时时间 */
#define MAX_FAN_TIMEOUT 100000000

#define TIMESTAMP_LEN   128
#define URL_MAX_LENGTH  2048

#define MEMORY_LOCATION_LEN 32
#define MACADDRESS_LEN 64

#define SIZE_4K 4096
#define SIZE_8K 8192
#define DUMP_FILE_PATH_LEN 128

#define MAX_RECURSIVE_DEPTH 10 /*最大递归深度*/


/* linux系统/内核相关 */
#define DYNAMIC_MEM_ENTRY   "/proc/dynmem"
#define DEV_TMP_DIR         "/dev/shm"

#define SYSCTL_REG_BASE_ADDR   0x20000000

/* 函数返回值 */
#define RET_OK  0
#define RET_ERR (-1)
#define RET_ERR_INVALID_PARAM (-2)

/* ------------------------通用定义---------------------- */

/* ------------------TCE相关定义，待删除----------------- */
#define BMC_BOARD_ID_TCE               0x30
#define EQUIPMENMT_BACKPLANE_TCE       0x07
#define NORMAL_BACKPLANE_4U            0x0
#define NORMAL_BACKPLANE_8U            0x2

#define SLOT_ID_UP               38
#define SLOT_ID_DOWN             37
/* ------------------TCE相关定义，待删除----------------- */

#define ATLANTIC_MM_BOARD_ID    0x30  /* 大西洋直通管理板 */
#define ATLANTIC_BOARD_ID       0x1B  /* 大西洋 */
#define PACIFIC_BOARD_ID        0x17  /* 太平洋 */
#define ARCTIC_BOARD_ID         0x90  /* 北冰洋 */
#define ARCTIC_BOARD_ID_2       0x93  /* 北冰洋 */
#define CST1020V6_BOARD_ID      0x9c  /* 全宽计算X86 */
#define CST1220V6_BOARD_ID      0x9f  /* 全宽计算ARM */
#define CST0210V6_BOARD_ID      0xa3  /* XPU管理板 ARM */
#define PAC1220V6_BOARD_ID      0xf3  /* 东海 */
  
#define SERVICE_NETCONFIG_ENABLE    1 // huanghan 不属于服务配置，需要单独考虑


/******************************************* BEGIN 错误码定义 *********************************************/

/* BEGIN: Modified by zhanglei wx382755, 2017/5/24   PN:DTS2017052312683 */
#define INVALID_IP_ADDRESS                   0x10
/* END:   Modified by zhanglei wx382755, 2017/5/24 */
/******************************************* END 错误码 *********************************************/
#define REDFISH_REQ_FOR_WEB_AUTH               2
#define REDFISH_REQ_FROM_WEBUI                 1
#define REDFISH_REQ_FROM_OTHER_CLIENT          0
#define MAX_LOG_INTERFACE_MODULE_LEN           64                 /* 操作日志记录模块名最大长度 */

#define HMM_BORAD  "HMM"
#define REARM_REQUEST "Actions/Oem/Huawei/EventService.Rearm"

#define KEEP_PWR_STATUS_AFTER_LEAKAGE   0
#define POWER_OFF_AFTER_LEAKAGE         1

/******************************************* BEGIN switch_card模块 *********************************************/

/* BEGIN: Added by zengrenchang(z00296903), 2017/4/21   问题单号:AR-0000276591-001-004*/
#define SWITCH_PLANE_NO_RESPONE			(-10)
#define SWITCH_PLANE_RESPONE_ERR		(-11)
#define SET_SWITCH_PLANE_PARA_INVALID	(-12)
#define SET_SWITCH_PLANE_LAN_PARA_LEN_ERR	(-13)
#define SET_SWITCH_PLANE_IP_FORMAT_ERR 		(-14)
#define SET_SWITCH_PLANE_MASK_FORMAT_ERR 	(-15)
#define SET_SWITCH_PLANE_GW_FORMAT_ERR   	(-16)
/* END:   Added by zengrenchang(z00296903), 2017/4/21 */
/******************************************* END switch_card模块 *********************************************/



/*BEGIN RedefinedSymbol_2016_07_05*/
#define  INTF_OK 0


/* BEGIN: Added by chenqianqian c00416525, 2018/1/22 获取单板版本信息*/
#define BLADE_VERSION_MAX_LEN   4096
/* END: Added by chenqianqian c00416525, 2018/1/22 */

/* BEGIN: Added for PN:DTS2015110310680  by w00208253, 2015/12/18 */
#define RECORD_FILE "/data/var/log/pme/record.xml"
/* END:   Added for PN:DTS2015110310680  by w00208253, 2015/12/18 */



/* BEGIN: Modified by linwei, 2018/12/28   问题单号:UADP120859 */
#define ERR_EXCEED_MAX_LEN       (-2)
/* END: Modified by linwei, 2018/12/28   问题单号:UADP120859 */


#define MAX_DEVICE_ID_CFG_CNT   32

#define PS_VERSION_LEN 64
#define AGENT_KEY_STRING "Agent"

#define MANAGEMENT_TYPE     2

#define MAX_VER_DATA_LEN 512

#define RESP_MSG_MAX_DATA_LEN 248
/*从ME获取信息时尝试的次数*/
#define RETRY_TIME 3

#define SHRARE_FUR_UPGRADE      1
#define NO_SHRARE_FUR_UPGRADE   0

#define DEFAULT_CLI_SESSION_TIMEOUT 15
#define MAX_CLI_SESSION_TIMEOUT 480

#define MAX_USER_ID_STR_LEN             2
#define MAX_SLOT_ID_STR_LEN             2
#define REDFISH_REFRESH        1
#define REDFISH_NOT_REFRESH    2


/* BEGIN for Redfish : Added by lwx377954 2016-8-26 */
#define RFERR_INSUFFICIENT_PRIVILEGE      (0x000000 | 403)
#define RFERR_WRONG_PARAM                 (0x000000 | 400)
#define RFERR_NO_RESOURCE                 (0x000000 | 404)
#define RFERR_GENERAL                     (0x000000 | 500)
#define RFERR_SUCCESS                     (0x000000 | 200)
/* END for Redfish : Added by lwx377954 2016-8-26 */


/* BEGIN: Added by z00352904, 2016/11/24 20:36:4  PN:AR-0000264509-001-001 */
#define RELATIVE_URI                 ("RelativeUri")
#define REQUEST_METHOD               ("RequestMethod")
#define REQUEST_BODY                 ("RequestBody")
#define REQUEST_HEADERS              ("RequestHeaders")
#define REQUEST_IP                   ("RequestIp")
#define REQUEST_SERVICE_ROOT         ("ServiceRoot")
#define RESPONSE_HEADERS             ("ResponseHeaders")
#define RESPONSE_STATUS_CODE         ("ResponseStatusCode")
#define RESPONSE_BODY                ("ResponseBody")

/* HTTP请求头 */
#define REQ_HEADER_CONTENT_TYPE      ("Content-Type")
#define REQ_HEADER_ODATA_VERSION     ("OData-Version")
#define REQ_HEADER_HOST              ("Host")
#define REQ_HEADER_ORIGIN            ("Origin")
#define REQ_HEADER_IF_MATCH          ("If-Match")
#define REQ_HEADER_IF_NONE_MATCH     ("If-None-Match")
#define REQ_HEADER_X_AUTH_TOKEN      ("X-Auth-Token")
#define REQ_HEADER_FROM               ("From")
#define REQ_HEADER_TOKEN               ("Token")
#define REQ_HEADER_COOKIE            ("Cookie")
#define REQ_HEADER_REAUTH_KEY    ("Reauth-Key")
#define REQ_HEADER_ACCEPT            ("Accept")
#define REQ_HEADER_BASIC_AUTH        ("Authorization")
#define REQ_HEADER_BASIC_CONTENT     ("Basic ")
#define REQ_HEADER_CONTENT_DISPOSITION      ("Content-Disposition")
#define REQ_HEADER_REQUESTER_ADDR    ("Requester-Addr")
#define REQ_HEADER_CONTENT_LENGTH      ("Content-Length")


/* HTTP响应头 */
#define RSP_HEADER_CACHE_CONTROL      ("Cache-Control")
#define RSP_HEADER_EXPIRES            ("Expires")

#define RSP_HEADER_CACHE_CONTROL_DEFAULT      ("max-age=0, no-cache, no-store, must-revalidate")
#define RSP_HEADER_EXPIRES_DEFAULT            ("0")
#define CURRENT_ODATA_VERSION                 ("4.0")

/* 支持的HTTP方法 */
#define HTTP_GET                   ("GET")
#define HTTP_CREATE                ("CREATE")
#define HTTP_ACTION                ("ACTION")
#define HTTP_PATCH                 ("PATCH")
#define HTTP_PUT                   ("PUT")
#define HTTP_DELETE                ("DELETE")
#define HTTP_POST                  ("POST")
#define HTTP_ALL                   ("ALL")

typedef enum {
    E_METHOD_NULL,
    E_METHOD_GET,
    E_METHOD_CREATE,
    E_METHOD_ACTION,
    E_METHOD_PATCH,
    E_METHOD_PUT,
    E_METHOD_DELETE,
    E_METHOD_POST,
    E_METHOD_ALL
} METHOD_TYPE_E;

#define REQ_SESSION_ID ("SessionId=")
#define REQ_FROM_WEBUI ("WebUI")



#define MAX_STATUS_STRING_LEN        70
#define MAX_HEADER_NUM               13
#define MAX_HEADER_TITLE_LEN         64
#define MAX_HEADER_CONTENT_LEN       384
#define MAX_RESPONSE_BODY_LEN        40960
#define MAX_RESPONSE_STRING_LEN      51200//目前文件最大46k
#define MAX_URI_LENGTH               512
#define MAX_URI_LENGTH_STR           "512"
#define MAX_REQUEST_BODY_LEN         (100 * 1024)   //最大支持100k
#define USER_DOMAIN_NAME_MAX_LEN     32


/* BEGIN: Added by z00352904, 2016/12/1 17:22:5  PN:AR-0000264509-001-001 */
#define RSC_NOT_EXIST   -2

#define ODATA_TYPE_MAX_LEN   64
#define BDF_INFO_NUM   4
#define BDF_INFO_LEN   32
#define MAX_DID_VID_LEN     32
#define MAX_PCIEPATH_LEN    256
#define OBJ_NAME_LENGTH   32
#define PROP_VAL_LENGTH   64
#define PROP_VAL_MAX_LENGTH   512
#define MAX_IP_NUM   128
#define MAX_DISK_NUM   512
#define MAX_WAIT_TIME   120
#define MAX_DYNAMIC_OBJ_NUM     128
#define MAX_INFO_LEN     248
#define MAX_SPAN_CNT   8
#define MAX_ID_LEN   4
#define OBJ_ADD_WAIT_TIME   5 * 1000
#define NCSI_VERSION_LEN  4
#define NET_CARD_FIRMWARE_VER_LEN    16
#define VRD_VERSION_LENGTH 128
#define PCI_DID_VID_LEN     4

#define DCPMM_TECHNOLOGY_NAME  "Intel persistent memory"
/* BEGIN: Added by luyan l00446261, 2019/02/28 问题单号:iBMC DTS2019022801458 修改AEP内存判断方式 */
#define DCPMM_TECHNOLOGY_TYPE  1
/* END:   Added by luyan l00446261, 2019/02/28 */
#define DCPMM_HEALTH_DEFAULT_VALUE   0xF0


#define BUSINESSPORT_CARD_TYPE_VIR                   6
#define BUSINESSPORT_CARD_TYPE_TEAM                  7
#define BUSINESSPORT_CARD_TYPE_BRIDGE                8
#define BUSINESSPORT_CARD_TYPE_MEZZCARD              9

#define UPDATE_RESOURCE_TYPE_ETHERNET                0
#define UPDATE_RESOURCE_TYPE_FC                      1

#define NETDEV_FUNCTYPE_ETHERNET                     (1UL)
#define NETDEV_FUNCTYPE_FC                           (2UL)
/* BEGIN: Modified by fwx469456, 2017/12/04 14:42:6   PN:AR.SR.SFEA02109379.004.004  */
#define NETDEV_FUNCTYPE_IB                           (32UL)
/* END:   Modified by fwx469456, 2017/11/30 14:42:38 */
/* END:   Modified by jwx372839, 2017/6/2 14:43:9 */

/* BEGIN: Modified by jwx372839, 2017/8/23 9:32:10   问题单号:AREA02109379-006-001 */
#define NETCARD_BUSIPORT_TYPE_UNKNOWN                0
#define NETCARD_BUSIPORT_TYPE_ETH                    1
#define NETCARD_BUSIPORT_TYPE_FC                     2
#define NETCARD_BUSIPORT_TYPE_ETH_FC                 3
/* BEGIN: Modified by fwx469456, 2017/11/30 14:39:23   PN:AR.SR.SFEA02109379.004.004 */
#define NETCARD_BUSIPORT_TYPE_ISCSI                  4
#define NETCARD_BUSIPORT_TYPE_FCOE                   8
#define NETCARD_BUSIPORT_TYPE_OPA                    16
#define NETCARD_BUSIPORT_TYPE_IB                     32
/* END:   Modified by fwx469456, 2017/11/30 14:42:38 */

/* END:   Modified by jwx372839, 2017/8/23 9:32:13 */

#define BUSINESSPORT_LINKSTATE_UP                    "LinkUp"
#define BUSINESSPORT_LINKSTATE_DOWN                  "LinkDown"
#define BUSINESSPORT_LINKSTATE_NOLINK                "NoLink"
/* END:   Modified by jwx372839, 2017/4/9 10:8:58 */

#define BUSINESSPORT_CARD_TYPE_INDEX_TEAM            "/Team/"
#define BUSINESSPORT_CARD_TYPE_INDEX_BRIDGE          "/Bridge/"
//
/* END:   Added by z00352904, 2016/12/1 17:22:11*/

#define json_object_object_foreach0(obj,key,val) \
    char *key = NULL;\
    struct json_object *val = NULL; \
    struct lh_entry *entry ## key; \
    struct lh_entry *entry_next ## key = NULL; \
    struct lh_table* obj_ref = json_object_get_object(obj);\
    if(obj_ref)\
        for(entry ## key = obj_ref->head; \
            (entry ## key ? ( \
                              key = (char*)entry ## key->k, \
                              val = (struct json_object*)entry ## key->v, \
                              entry_next ## key = entry ## key->next, \
                              entry ## key) : 0); \
            entry ## key = entry_next ## key)\
            /* END:   Added by z00352904, 2016/11/24 20:36:7*/


/* BEGIN: Added by zhongqiu 00356691, 2016/11/25   PN:AR-0000267460-001-003*/
/*新增升级相关的错误码*/
#define UP_COMM_FILE_NOT_EXIST_STR             "the upgrade file does not exist."
#define UP_COMM_FILE_ERROR_STR                 "invalid upgrade file."
#define UP_COMM_LITTLE_MEM_STR                 "insufficient memory capacity for the upgrade."
#define UP_COMM_MAIN_MAX_STR                   "an error occurred during the upgrade."
/* BEGIN: Modified by z00356691 zhongqiu, 2016/12/30   问题单号:DTS2016122602058 */
#define UP_OS_POWER_ERROR_STR                  "power off the OS, and then start the BIOS upgrade."
/* END: Modified by z00356691 zhongqiu, 2016/12/30   问题单号:DTS2016122602058 */
#define UP_COMM_FIRMWARE_MISMATCH_STR          "the firmware version does not match."
#define UP_COMM_FILE_FORMAT_ERROR_STR          "the upgrade file format is incorrect."
#define UP_COMM_FILE_LENGTH_ERROR_STR          "the upgrade file is too large."
    /* Start by huangjiaqing DTS2017071815582  20170801 */
#define UP_COMM_FILE_MISMATCH_STR   "The upgrade file does not match the device to be upgraded."
#define UP_COMM_WHITEBRAND_FILE_MISMATCH_STR   "The version of the update pack does not match the current web interface version."
#define UP_COMM_ME_ERROR_STR   "The BIOS upgrade fails because the ME status is not correct."
#define UP_COMM_UNKNOWN_ERROR_STR   "Unknown error."
#define UP_COMM_POWERING_ON_ERR_STR   "Powering on... Failed to upgrade the firmware."
#define UP_COMM_VERSION_EARLY_ON_ERR_STR   "The upgrade package version is too early."

    /* Start by huangjiaqing DTS2017071815582  20170801 */
/* BEGIN: Added by wenghongzhang, 2018/07/04  DTS2018041801401     */
#define UP_COMM_UPGRADE_FAILED   		"Upgrade failed."
/* END: Added by wenghongzhang, 2018/07/04  DTS2018041801401     */
/*BEGIN Modify by w00416917 电源升级新增错误码          for DTS2019032607199 2019/05/15*/
#define UP_PSU_INNER_ERR_STR        "internal error."
#define UP_PSU_UNSUPPORT_ERR_STR    "current status unsupported."
#define UP_PSU_NUM_ERR_STR          "the remaining PSUs that are working properly cannot provide the power required by the entire system."
#define UP_PSU_HPM_ERR_STR          "the PSU model does not support the upgrade or does not match the upgrade file."
#define UP_PSU_NOT_UP_ERR_STR       "is not upgraded."
/*END Modify by w00416917 电源升级新增错误码          for DTS2019032607199 2019/05/15*/


// Redfish SP相关文件(由于ui_manage模块要用到此文件作为创建文件代理接口的白名单，所以移到此处)
#define SP_OSINSTALL_PARA_PATH "/data/opt/pme/save/sposinstallpara"
#define SP_OSINSTALL_PARA_NAME "osinstall para"

#define ACTION_STATUS    1
#define NOACTION_STATUS  0

#define END_OF_TABLE_TYPE 0x7f


#define JSON_SCHEMA_DEFINITION          "definitions"
#define JSON_SCHEMA_REFERENCE           "$ref"
#define JSON_SCHEMA_ANYOF               "anyOf"
#define JSON_SCHEMA_READONLY            "readonly"

#define JSON_SCHEMA_TYPE                "type"
#define JSON_SCHEMA_PROPERTY            "properties"
#define JSON_SCHEMA_ODATAID             "@odata.id"
#define JSON_SCHEMA_ACTION              "Actions"
#define JSON_SCHEMA_TARGET              "target"
#define JSON_SCHEMA_CREATE_REQUIRED     "requiredOnCreate"

/* BEGIN: Added by z00356691 zhongqiu, 2017/3/2  支持动态参数检查 PN:AR------Redfish事件上报*/
#define JSON_SCHEMA_PATTERN_PROPERTIES            "patternProperties"
#define JSON_SCHEMA_ADDITIONAL_PROPERTIES         "additionalProperties"
/* END:   Added by z00356691 zhongqiu, 2017/3/2  支持动态参数检查 PN:AR------Redfish事件上报*/

#define JSON_SCHEMA_FORMAT              "format"
#define JSON_SCHEMA_FORMAT_URI          "uri"
#define JSON_SCHEMA_FORMAT_DATETIME     "date-time"

#define JSON_SCHEMA_PATTERN             "pattern"
#define JSON_SCHEMA_MINIMUM             "minimum"
#define JSON_SCHEMA_MAXIMUM             "maximum"

#define JSON_SCHEMA_ENUM                "enum"

#define JSON_SCHEMA_MEMBERS             "Members"
#define JSON_SCHEMA_ITEMS               "items"



#define NAME_KEY                        "Name"
#define LANGUAGE_KEY                    "Language"
#define REGISTRY_PREFIX_KEY             "RegistryPrefix"
#define REGISTRY_VERSION_KEY            "RegistryVersion"
#define OWNING_ENTITY_KEY               "OwningEntity"
#define MESSAGE                         ("Messages")
#define REDFISH_COPYRIGHT_KEY           "@Redfish.Copyright"
#define ODATA_TYPE_KEY                  "@odata.type"

#define RFPROP_COMON_CONTROLLER_PKT_STAT "ControllerPacketStatistics"
#define RFPROP_COMMON_HUAWEI            "Huawei"
#define RFPROP_COMMON_OEM			    "Oem"
#define RF_VALUE_NULL                   "null"

#define ERROR_RESPONSE "HTTP/1.1 400 Bad Request"
#define ERROR_RESPONSE_UNAVAILABLE "HTTP/1.1 503 Service Unavailable"
#define MAX_JSON_POINTER_PATH_LEN    512

#define MAX_MEM_ID_LEN               128
#define STRING_LEN_MAX				 32

/*导入导出BMC和BIOS配置的默认路径*/
#define GET_CONFIG_FILE_NAME "/dev/shm/config.xml"
#define GET_CONFIG_BIOS_FILE_NAME "/dev/shm/bios.xml"

#define SUPPORT 1
#define NOT_SUPPORT 0

/* BEGIN: Added by z00356691 zhongqiu, 2017/2/20   PN:AR------Redfish事件上报*/
#define MAX_MSG_ID_LEN             128
#define MAX_SERVER_IDENTITY_LEN    65
#define MAX_SERVER_LOCATION_LEN    65
#define MAX_RF_TIME_STAMP_LEN      64
#define MAX_RSC_URI_LEN            256
#define MAX_EVT_TYPE_LEN           128
#define MAX_MSG_PARAM_LEN          512
#define MAX_EVT_DESTINATION_LEN    1025
#define MAX_EVT_HEADERS_SIZE       1025
#define MAX_EVT_CONTEXT_LEN        256
#define MAX_EVT_SUBJECT_DETAIL_LEN 256
#define MAX_EVT_LOCATION_LEN       384
#define MAX_EVT_STATE_LEN          32
#define MAX_EVT_DRP_LEN            64

#define MAX_EVT_DESC_LEN      1024
#define NAME_LEN  64

typedef enum {
    SHIELD_NO_PER_SAVE = 0,
    SHIELD_RESET_PER_SAVE,
    SHIELD_AC_PER_SAVE,
} SHIELD_SAVE_OPTION;

typedef enum {
    SHIELD_EVENT = 0,
    UNSHIELD_EVENT,
} SHIELD_STATE;

/* 
RF_EVT_MSG_INFO结构体内存在函数dal_post_redfish_evt_msg中用于拷贝复制，后续新增非固定内存大小的成员时，
需要将该成员指向的内存也复制一份
*/
typedef struct tag_redfish_evt_msg_info
{
    /* 事件消息Id */
    gchar       msg_id[MAX_MSG_ID_LEN];

    /* 消息参数列表字符串:以;;分隔 */
    gchar       msg_params[MAX_MSG_PARAM_LEN];

    /* 事件类型 */
    gchar       event_type[MAX_EVT_TYPE_LEN];

    /* 事件码 */
    guint32     evt_id;

    /* EventMonitor对象名称 */
    gchar monitor_name[MAX_NAME_SIZE + 1];

    /* 事件产生时间戳 */
    gchar       event_time_stamp[MAX_RF_TIME_STAMP_LEN];

    /* 事件源地址 */
    gchar       origin_rsc[MAX_URI_LENGTH];

    /* 事件触发类型:正常事件或测试事件 */
    guchar      evt_origin_type;

    /* 事件主体精准信息 */
    gchar       evt_subject[MAX_EVT_SUBJECT_DETAIL_LEN];

    /* 事件来源,传感器,redfish扫描线程,HMM备板;当来自于HMM备板时修改ServerLocation */
    guchar      evt_rsc_type;
    gchar sn[NAME_LEN];  // 事件主体对应的部件序列号
    gchar pn[NAME_LEN];  // 事件主体对应的部件号

    /* 事件主体类型 */
    guint8      subject_type;
    /* 事件主体名称 */
    gchar       subject_name[MAX_NAME_SIZE + 1];
    /* 事件位置 */
    gchar       subject_location[MAX_EVT_LOCATION_LEN + 1];
    /* 事件状态产生/消除 */
    guint8 evt_state;
    /* 事件告警级别 */
    guint8 severity;
    /* 发送消息时是否强制忽略接收端证书校验 */
    gboolean ignore_ssl_cert_verify;
} RF_EVT_MSG_INFO;

#define MAX_RF_EVT_TYPE_NUM            5
#define MAX_RF_EVT_MSG_ID_NUM          500
#define MAX_RF_EVT_ORIGIN_RSC_NUM      400

#define RF_RSC_ADDED_EVT_STR           "ResourceAdded"
#define RF_RSC_UPDATED_EVT_STR         "ResourceUpdated"
#define RF_RSC_REMOVED_EVT_STR         "ResourceRemoved"
#define RF_RSC_STATUS_CHANGED_EVT_STR  "StatusChange"
#define RF_ALERT_EVT_STR               "Alert"

// 注:其它类型的资源事件消息Id和事件类型字符串一致，状态变化事件Id单独列出
#define RF_RSC_STATUS_CHANGED_MSG_ID   "ResourceStatusChanged"

enum RedfishRscCustomID {
    RF_RSC_CUSTOM_ID_DEDAULT = 0, /* 不显示 */
    RF_RSC_CUSTOM_ID_CMCC = 1,    /* 显示CMCC定制的字段 */
    RF_RSC_CUSTOM_ID_CUCC = 2,    /* 显示CUCC定制的字段 */
    RF_RSC_CUSTOM_ID_BUTT
};

typedef enum tag_rsc_evt_type {
    RF_RSC_ADDED_EVT,
    RF_RSC_REMOVED_EVT,
    RF_RSC_UPDATED_EVT,
    RF_RSC_STATUS_CHANGED_EVT,
    RF_ALERT_EVT,
    RF_EVT_END      /*保持最后一个成员，此位置后勿添加*/
} RSC_EVT_TYPE;

typedef struct tag_rsc_evt_cfg {
    RSC_EVT_TYPE    evt_type;
    const gchar*    evt_type_str;
    const gchar*    evt_msg_id;
} RSC_EVT_CFG_S;


#define RF_BRDSN_STR                   "BoardSN"
#define RF_PRODUCT_ASSET_TAG_STR       "ProductAssetTag"
#define RF_HOST_NAME_STR               "HostName"
#define RF_CHASSIS_NAME_STR               "ChassisName"
#define RF_CHASSIS_LOCATION_STR               "ChassisLocation"
#define RF_CHASSIS_SERIAL_NUMBER_STR               "ChassisSerialNumber"

typedef enum tag_identity_trans_direction {
    IDENTITY_INDEX_TO_STR,
    IDENTITY_STR_TO_INDEX
} IDENTITY_TRANS_DIRECTION;

typedef struct CURL_RESP_ST
{
    gchar*     ptr;
    size_t     len;
} CURL_RESP_STR_S;


#define URI_JSON_POINTER_FLAG       "#"
#define  BMC_GUID_LEN                           16

#define NA_STR                         "N/A"


#define MAX_COMMON_PROP_VAL_LEN           255
#define MAX_COMMON_PROP_VAL_BUF_LEN       (MAX_COMMON_PROP_VAL_LEN + 1)
#define MAX_COMMON_PROP_VAL_LEN_STR       "255"

#define MAX_THREAD_NAME_LEN            32         /*框架限制线程名长度32，超过时串口有打印*/

#define MAX_STATUS_PROP_PATH_LEN            128
#define ACCOUNTSERVICE_ACCOUNTS                               "/redfish/v1/AccountService/Accounts"
#define SNMPSERVICE_PATH                                      "/redfish/v1/Managers/%s/SnmpService"

#define MAX_SWI_IPV6_GROUP_NUM 16  //交换板最大支持设置16个IPV6地址和IPV6路由
#define IPMI_SET_MEMBRD_CONFIG_ERROR_CODE   0xfe    //设置内存板PROPERTY_MEMBOARD_CONFIG_ERROR_CODE,上报vmse error
#define MAX_COMP_DATA_BLOCK_LEN 1024
#define BUSSINESS_PORT_PFINFO_MAX_CNT   32
#define LINK_DOWN    0
#define LINK_UP      1
#define TMP_PATH_WITH_SLASH     "/tmp/"
#define TMP_PATH_NO_SLASH       "/tmp"
#define DEV_SHM_PATH_WITH_SLASH "/dev/shm/"



/* BEGIN: Modified by jwx372839, 2017/3/10 19:25:40   问题单号:AR-0000276593-005-007 */
#define SET_CPU_UTILISE_TYPE      0
#define SET_MEM_UTILISE_TYPE      1
#define SET_DISK_UTILISE_TYPE     2
/* END:   Modified by jwx372839, 2017/3/10 19:27:31 */


/* END:   Modified by zhanglei wx382755, 2017/4/20 */

/* BEGIN: Modified by wwx390838, 2017/5/12 16:43:42   PN:DTS2017050201900 */
#define PROP_MESSAGES "Messages"
#define PROP_RELATED_PROPS "RelatedProperties"
/* BEGIN: 2018-10-24 田鹏twx572344 问题单号：DTS2018101702360 */
#define PROP_DELETED_PROPS "DeletedProperties"
/* END:   2018-10-24 田鹏twx572344 问题单号：DTS2018101702360 */

#define CALLER_INFO_IPMI "IPMI"
#define CALLER_INFO_ADMINISTRATOR "Administrator"
#define CALLER_INFO_HOST "HOST"
#define CALLER_INFO_SNMP "SNMP"

/* BEGIN: Added by zhongqiu 00356691, 2017/6/13   PN:AR-支持G5500 REST接口*/
#define CHASSIS_TYPE_MASK                            0xf0
#define BMC_TYPE_MASK                                0x0f
/* END:   Added by zhongqiu 00356691, 2017/6/13   PN:AR-支持G5500 REST接口*/

/* BEGIN: Added by 00356691 zhongqiu, 2017/7/17   问题单号:支持SMM加载PCIe标卡 */


#define MAX_PCIE_CARD_DESC_LEN                       256
#define MAX_NODE_NUM                                 8
#define MAX_PCIE_INFO_ARRAY_LEN                      512      /*当前最大32张PCIe卡 * 7个属性= 224*/
#define ALL_OBJECT_INFO_RESULT_HEADER_LEN            5        /*查询所有指定类所有对象信息返回结果头的长度:00+DB0700+<结束帧>*/

/* END:   Added by 00356691 zhongqiu, 2017/7/17   PN:支持SMM加载PCIe标卡*/

/* BEGIN: Added by 00356691 zhongqiu, 2017/11/24   PN:AR.SR.SFEA02119723.009.001 支持节点配置机框拓扑*/
typedef enum
{
    PCIeTopo_OK = 0, /*Common success return code, the same as VOS_OK.*/
    PCIeTopo_CONFIG_TASK_RUNNING, /*Another topo config task exists.*/
    PCIeTopo_CONFIG_FINISHED = 0x02, /*Topo config finished.*/
    PCIeTopo_CONFIG_TIMEOUT /*Topo config timeout.*/
}PCIeTopoCode;

// AC启动后配置  TOP ，禁止上电时,由CPLD 提供值
#define CONFIG_TO_FORBID_POWER_ON  (6*1000)

#define PCIeTopo_CONFIG_WAIT_RETRY_TIMES             20
#define MAX_COMP_DOMAIN_NUM                          2
#define COMP_DOMAIN_ALL                              255




#define MAX_PRINT_STR_LEN                            1024

/* BEGIN: Added by l00389091, 2017/11/17  DTS2017111712982 */
#define MODULE_NAME_SNMP "SNMP"
/* END: l00389091, 2017/11/17 */
/* BEGIN: Added by z00356691 zhongqiu, 2017/4/25   问题单号:DTS2017030814281 */

/* END: Added by z00356691 zhongqiu, 2017/4/25   问题单号:DTS2017030814281 */

/* BEGIN: Added by zhongqiu 00356691, 2017/5/19   PN:DTS2017042206147*/
#define MAX_OBJECT_NAME_LEN                          64
/* END:   Added by zhongqiu 00356691, 2017/5/19   PN:DTS2017042206147*/

/* BEGIN: Added by 00356691 zhongqiu, 2019/2/12   PN:DTS2019011008463 版本信息同步处理增强 */
#define MAX_MCU_FW_VER_LEN                 32     /* MCU固件版本长度，当前为3段式，如1.7.0 */
/* END:   Added by 00356691 zhongqiu, 2019/2/12   PN:DTS2019011008463 版本信息同步处理增强 */

/* BEGIN: Added by linzhen, 2017/05/22 14:24   问题单号:iBMC DTS2017032703884         */
/* 一键收集新增返回值 MD_EONUPGRADE:正在升级     MD_ENOSPACE:空间不足*/
#define MD_EONUPGRADE   DEF_ERROR_NUMBER(ERR_MODULE_ID_MD, -1000)
#define MD_ENOSPACE   DEF_ERROR_NUMBER(ERR_MODULE_ID_MD, -1001)

/* 分区映射  从tmpfs_monitor.h移动到此*/

/* END:   Added by linzhen, 2017/05/22 14:24 */



/* BEGIN: AREA02109379-001-003. Modified by f00381872, 2017/8/14 */
#define HOST_DOMAIN_NAME_MAX_LEN        255
/* END: AREA02109379-001-003. Modified by f00381872, 2017/8/14 */



#define RESOURCE_ID_PCIE_CARD_BELONG_TO_ALL_CPU 0xFC


/* BEGIN: Modified by m00168195, 2017/9/4   PN:	DTS2017083106563*/
#define CHANNEL_PECI   0x2
#define CHANNEL_ME     0x1
#define CHANNEL_SET    0x0
#define CHANNEL_TEST   0x1
/* END:   Modified by m00168195, 2017/9/4 */

#define BUFF_LEN 128

/* BEGIN: Added by 00356691 zhongqiu, 2017/8/25   问题单号:支持1K机框背板电子标签管理 */
#define TXB                 0x0B // transaction begin
#define TXE                 0x0E // transaction end

#define INODE_ADD           0x0A
#define INODE_DEL           0x0D

#define INODE_VALID         0x3A
#define INODE_INVALID       0xFF

#define OFFSET_MULTIPLES    8
#define E2P_CAFE_MIN_SIZE   256

#define DATA_TYPE(data) ((data) >> 6) & 0x03
#define CAFE_DATA_LEN(data)  (data) & 0x3F
#define CAFE_ERR_BASE               0xcafe
#define CAFE_ERR_MAGIC_NUM          0xcb00
#define CAFE_ERR_BLK_SIZE           0xcb01
#define CAFE_ERR_CAFE_VER           0xcb02
#define CAFE_ERR_RESERVE_FAILED     0xcb03
#define CAFE_ERR_DATA_SIZE          0xcb04

#define CAFE_INIT_ACTION_NONE       0x00
#define CAFE_INIT_ACTION_FORMAT     0x01

#define CAFE_KEY_MAX_LEN            8

typedef struct tag_cafe_data {
    guint8 type;
    guint8 len;
    guint8 *data;
}CAFE_DATA_S;

typedef struct tag_cafe_record {
    CAFE_DATA_S *key;
    CAFE_DATA_S *value;
    guint8 inode;
}CAFE_RECORD_S;

/*EEPROM CAFE 键值定义*/
typedef enum E2P_CAFE_KEY {
    ChType,
    ChPN,
    ChSN,
    MDate,
    BName,
    BSN,
    BPN,
    PName,
    PPN,
    PSN = 10,
    AssetTag,
    EncLoc,
    TopoPCIe,
    ChSubnet,
    CAFE_KEY_MAX = 255  /*此后勿添加新枚举成员*/
}E2P_CAFE_KEY_E;

#define CAFE_INDEX_CHTYPE            "ChType"
#define CAFE_INDEX_CHPN              "ChPN"
#define CAFE_INDEX_CHSN              "ChSN"
#define CAFE_INDEX_MDATE             "MDate"
#define CAFE_INDEX_BNAME             "BName"
#define CAFE_INDEX_BSN               "BSN"
#define CAFE_INDEX_BPN               "BPN"
#define CAFE_INDEX_PNAME             "PName"
#define CAFE_INDEX_PPN               "PPN"
#define CAFE_INDEX_PSN               "PSN"
#define CAFE_INDEX_ASSETTAG          "AssetTag"
#define CAFE_INDEX_ENCLOC            "EncLoc"
#define CAFE_INDEX_TOPOPCIE          "TopoPCIe"
#define CAFE_INDEX_CHSUBNET          "ChSubnet"
/* BEGIN: Added by 00356691 zhongqiu, 2017/11/30   PN:AR.SR.SFEA02119723.009.001 支持节点配置机框拓扑*/
#define CAFE_INDEX_TOPOMD2           "TopoMd2"  /*2槽配置字段*/
#define CAFE_INDEX_FORMAT_DATE       "FmtDate"  /*使用3字节存储CAFE分区格式化时间:如171208表示2017年12月08日*/
/* END:   Added by 00356691 zhongqiu, 2017/11/30   PN:AR.SR.SFEA02119723.009.001 支持节点配置机框拓扑*/

/* BEGIN: Added by 00356691 zhongqiu, 2017/12/8   PN:AR.SR.SFEA02119723.009.001 支持节点配置机框拓扑*/
#define CAFE_FORMAT_DATA_VAL_LEN            3
#define CAFE_DESC_MAX_LEN                   256
/* END:   Added by 00356691 zhongqiu, 2017/12/8   PN:AR.SR.SFEA02119723.009.001 支持节点配置机框拓扑*/

typedef struct tag_cafe_key_to_fru_loc_info {
    /*CAFE信息键值*/
    const guchar        cafe_key;

    /*FRU区域位置信息，无效值为FRU_FILED_INVALID*/
    const gint32        fru_loc_area;

    /*FRU块位置信息，无效值为FRU_AREA_INVALID*/
    const gint32        fru_loc_filed;

    /*CAFE键值字符串*/
    const gchar*        cafe_key_str;
}CAFE_KEY_TO_FRU_LOC_INFO_S;

#define TYPE_BINARY         0x00
#define TYPE_BCD_PLUS       0x01
#define TYPE_6BIT_ASCII     0x02
#define TYPE_8BIT_ASCII     0x03

#define OPEN_WP 1
#define CLOSE_WP 0
/* eeprom写保护打开状态 */
#define EEPROM_WP_STATUS_ON 1

enum tag_error_code {
    FRUDATA_OK = 0,

    ERROR_CODE_PARAMETER_INVALID = DEF_ERROR_NUMBER(ERR_MODULE_ID_FRU, 0),
    ERROR_CODE_CANNOT_OPEN_FILE,
    ERROR_CODE_FILE_READ_FAIL,
    ERROR_CODE_FILE_WRITE_FAIL,
    ERROR_CODE_CRC_DIFFERENT,
    ERROR_CODE_CHECKSUM_DIFFERENT,
    ERROR_CODE_WRITE_E2P_ERROR,
    ERROR_CODE_READ_E2P_ERROR,
    ERROR_CODE_HEADER_ERROR
};

/* 按位设置禁止上电标志的值 */
enum {
    UPGRADE_ALLOW_POWER_ON = 0,
    UPGRADE_BIOS_FORBID_POWER_ON,
    UPGRADE_CPLD_FORBID_POWER_ON,
    UPGRADE_VRD_FORBID_POWER_ON = 4,
};
enum {
    BIOS_FORBID_POWER_ON_MASK = 1,
    CPLD_FORBID_POWER_ON_MASK,
    VRD_FORBID_POWER_ON_MASK,
};

/* 例如Atmel AT24C08C之类的EEPROM，使用1字节寻址，一页只能写256字节，超过256字节需要换页，第N页地址为A(2*(N-1))，即A0,A2,A4,A6 */
#define CAFE_EEPROM_PAGE_SIZE          256

#define CAFE_RW_MODE_COMMON            0
#define CAFE_RW_MODE_PAGE              1

#define PCIE_TOPO_VAL_SPLITTER         ";"

/* END: Added by 00356691 zhongqiu, 2017/8/25   问题单号:支持1K机框背板电子标签管理 */

/* BEGIN: Added by d00381887, 2017/9/13   AR-0000276585-017-001 支持PCIe Repeater配置 */
#define REG_CONFIG_OK           1
#define REG_CONFIG_ERROR        0

#define REG_CONFIG_DATA_LEN     256

#define CONFIG_ALL_REPEATER_FLAG        0
#define CONFIG_ONE_REPEATER_FLAG        1

#define POWER_CTRL_STATUS_ALLOW     0x01
#define POWER_CTRL_STATUS_FORBID    0x00
/* END: Added by d00381887, 2017/9/13   AR-0000276585-017-001 支持PCIe Repeater配置 */

/* BEGIN: Added by d00381887, 2017/9/13   AR-0000276585-017-001 支持PCIe Repeater配置 */

/* END: Added by d00381887, 2017/9/13   AR-0000276585-017-001 支持PCIe Repeater配置 */

/* BEGIN: Added by 00356691 zhongqiu, 2017/10/15   PN:支持G5500半宽拓扑配置*/
#define MAX_COMPOSITION_SETTING_CNT             16
/* END:   Added by 00356691 zhongqiu, 2017/10/15   PN:支持G5500半宽拓扑配置*/

/* BEGIN: Added by zhongqiu 00356691, 2019/4/26   PN:DTS2019042607055 iRM回合高优先级检视意见处理 */
#define MB_CRC_LEN 2
/* END:   Added by zhongqiu 00356691, 2019/4/26   PN:DTS2019042607055 iRM回合高优先级检视意见处理 */

#define CONNECTOR_MODBUS_DYNAMIC    "ModbusDynamic"     /* Modbus动态加载的连接器类型 */

/* BEGIN: Added by Yangshigui YWX386910, 2017/8/26   问题单号:AR-0000276585-011-003 */
#define PRR_MGNT_CHIP_OBJ_NAME          "PRRMgntChip"
#define PRR_READ_CHIP_OBJ_NAME          "PRRReadChip"

#define FABRIC_INFO_MGNT_OBJ_NAME      "FabricInfoMgnt"

#define PRR_I2C0_MODE_SEL_ACC_NAME      "PRRMgntAccessor"
#define PRR_EEP_SEL_ACC_NAME            "PRREepSelectAccessor"
#define PRR_VOLT_ADJ_ACC_NAME           "PRRVoltAdjustAccessor"

#define CPLD_CHIP_NAME                  "cpld1"

#define FABRIC_PAYLOAD_OBJ_NAME         "FabricPayload"

#define PRR_SVID_ADJUSTABLE_FLAG_START_BIT  18
#define PRR_SVID_ADJUSTABLE_FLAG_MASK       0x03
#define PRR_SVID_ADJUST_VALUE_START_BIT     6
#define PRR_SVID_ADJUST_VALUE_MASK          0x0f

#define PRR_I2C_SET_SALVE_VALUE             0x00
#define PRR_I2C_SET_CHANNEL_FOR_SAVE_ACCESS 0x0f

#define PRR_I2C_MODE_DEFAULT_VALUE          0x01
#define PRR_I2C_SET_CHANNEL_DEFAULT         0x03

#define PRR_I2C_READY_STATUS_REG_ADDR       0x8c
#define PRR_I2C_READY_STATUS_MASK           0x40
/* END:   Added by Yangshigui YWX386910, 2017/8/26 */




#define MAX_NODE_SLOT_NUM                       128
typedef struct tag_node_valid_info
{
	/* 本板生效模式 */
	gint8 local_node_valid_mode;
    /* 节点生效模式，数组下标为节点槽位号，目前最大值为32 */
	gint8 node_valid_mode[MAX_NODE_SLOT_NUM + 1];
}NODE_REBOOT_INFO_S;

/* -200±X；thrift特有错误码，原因是被调用的远程函数抛异常, PME版本在2020/10月份把原-200进行精细化如下
  (-190 ~ -199):TApplicationExceptionType
  (-201 ~ -207):TTransportExceptionType */
enum DF_RPC_EXCEPTION_TYPE {
    RPC_CORRUPTED_DATA = -207,
    RPC_BAD_ARGS = -206,
    RPC_INTERRUPTED = -205,
    RPC_END_OF_FILE = -204,
    RPC_TIMED_OUT = -203,
    RPC_ALREADY_OPEN = -202,
    RPC_NOT_OPEN = -201,
    RPC_UNKNOWN = -200,
    RPC_UNKNOWN_METHOD = -199,
    RPC_INVALID_MESSAGE_TYPE = -198,
    RPC_WRONG_METHOD_NAME = -197,
    RPC_BAD_SEQUENCE_ID = -196,
    RPC_MISSING_RESULT = -195,
    RPC_INTERNAL_ERROR = -194,
    RPC_PROTOCOL_ERROR = -193,
    RPC_INVALID_TRANSFORM = -192,
    RPC_INVALID_PROTOCOL = -191,
    RPC_UNSUPPORTED_CLIENT_TYPE = -190
};

/* BEGIN: Added by 00356691 zhongqiu, 2017/9/15   PN:支持传感器重命名*/
#define MAX_STRING_FMT_LENGTH                  256
#define MAX_FMT_ARG_BUF_LEN                    64
#define MAX_FMT_ARG_NUM                        5
#define FMT_ARG_PROP_PATTERN                   "Param%d"
/* END:   Added by 00356691 zhongqiu, 2017/9/15   PN:支持传感器重命名*/

/* BEGIN: Added by 00356691 zhongqiu, 2018/1/16   PN:DTS2018010202317 资源URI优化*/
#define INVALID_SLOT                           255
/* END:   Added by 00356691 zhongqiu, 2018/1/16   PN:DTS2018010202317 资源URI优化*/

/* BEGIN: Added by d00381887, 2018/2/27    问题单号:SR.SF-0000276585.006  支持机框部件的故障汇聚节点BMC */
#define MAX_IPV4_LEN 16
#define DEFAULT_RPC_PROXY_PORT 51000
/* END: Added by d00381887, 2018/2/27 */


/* BEGIN: Added by d00381887, 2017/9/24   问题单号: DTS2017092400656 */
#define FANBOARD_FW_TYPE_CPLD                   0
#define FANBOARD_FW_TYPE_C51                    1
/* END: Added by d00381887, 2017/9/24   问题单号: DTS2017092400656 */

#define INVALID_EP_IDENTIFIER_ID                255

/*BEGIN: Modified by d00381887, 2017/10/27, PN: DTS2017102510274 */
/* BEGIN: Added by c00346034, 2018/07/18,  PN:SR.SFEA02130837.004.011 */

/*BEGIN: Modified by d00381887, 2017/10/27, PN: DTS2017102510274*/

/*BEGIN: Modified by d00381887, 2017/11/27, AR.SR.SF-0000276585.002.011 支持FPGA卡芯片的功耗查询 */
#pragma pack(1)
typedef struct tag_ipmimsg_property_info_resp_s
{
    guint8 comp_code;	//返回码；
    guint8 oem_info[3]; //OEM信息；
    guint8 end_list;	//保留位；
    guint8 data[0];		//数据位首地址；
}IPMIMSG_PROPERTY_INFO_RESP_S;

typedef struct tag_ipmi_property_request_data_info_s
{
    guint16 property_id;	//属性对应ID值；
    guint8 property_type;	//属性值类型；
    guint8 property_length;	//属性值长度；
    guint8 property_value[0];//属性值数据首地址；
}RESP_IPMI_PROPERTY_INFO_S;
#pragma pack()
/*END: Modified by d00381887, 2017/11/27, AR.SR.SF-0000276585.002.011 支持FPGA卡芯片的功耗查询 */

#define ME_ACCESS_TYPE_SMLINK                  0
#define ME_ACCESS_TYPE_SMBUS_PECI          1
#define ME_ACCESS_TYPE_QS                 2
#define ME_ACCESS_TYPE_QZ                 3

#define ME_IN_RECOVER 1
#define ME_IN_NORMAL  2
#define ME_RESP_ERROR (-2)

/* BEGIN: Added by c00346034, 2018/06/27,  PN:DTS2018062710216  */
#define PECI_GET_ID_MAXTRY 20
/* BEGIN: Added by c00346034, 2018/06/27,  PN:DTS2018062710216  */


/* BEGIN: Added by c00346034, 2018/06/13,  PN:AR.SR.SFEA02137099.001.017, 通过PECI获取PCIe卡的vid,did regoffset地址0x2c */
#define PECI_SUBVENDOR_ID_OFFSET 0x2c
/* END:   Added by c00346034, 2018/06/13,  PN:AR.SR.SFEA02137099.001.017, 支持PECI获取PCIe设备信息 */

#define DEV_STAT_COMMON_LEN 6

#define DEV_STAT_1_IPMI_OFFSET 6
#define DEV_STAT_2_IPMI_OFFSET 7
#define DEV_STAT_3_IPMI_OFFSET 8
#define DEV_STAT_4_IPMI_OFFSET 9
#define DEV_STAT_5_IPMI_OFFSET 10
#define DEV_STAT_6_IPMI_OFFSET 11
#define DEV_STAT_7_IPMI_OFFSET 12
#define DEV_STAT_8_IPMI_OFFSET 13
#define DEV_STAT_9_IPMI_OFFSET 14
#define DEV_STAT_10_IPMI_OFFSET 15
#define DEV_STAT_11_IPMI_OFFSET 16
#define DEV_STAT_12_IPMI_OFFSET 17
#define DEV_STAT_13_IPMI_OFFSET 18
#define DEV_STAT_14_IPMI_OFFSET 19

#define DEV_STAT_1_INDEX 0
#define DEV_STAT_2_INDEX 1
#define DEV_STAT_3_INDEX 2
#define DEV_STAT_4_INDEX 3
#define DEV_STAT_5_INDEX 4
#define DEV_STAT_6_INDEX 5
#define DEV_STAT_7_INDEX 6
#define DEV_STAT_8_INDEX 7
#define DEV_STAT_9_INDEX 8
#define DEV_STAT_10_INDEX 9
#define DEV_STAT_11_INDEX 10
#define DEV_STAT_12_INDEX 11
#define DEV_STAT_13_INDEX 12
#define DEV_STAT_14_INDEX 13

/* BEGIN: Added by d00381887, 2017/12/18, AR.SR.SFEA02119723.006.002 支持识别媒质类型 */
#define MEDIA_NOT_PRESENT   0
#define MEDIA_PRESENT       1
/* BEGIN: Added by d00381887, 2017/12/18, AR.SR.SFEA02119723.006.002 */


/* BEGIN: Added by l00389091, 2017/11/17  DTS2017111712982 */
#define BIOS_UPGRADING 1
#define POWER_ON_FAILED_BY_BIOS_UP ((BIOS_UPGRADING<<8)|COMP_CODE_STATUS_INVALID)
/* END: l00389091, 2017/11/17 */

#define CPLD_UPGRADING 2
#define POWER_ON_FAILED_BY_CPLD_UP ((CPLD_UPGRADING << 8) | COMP_CODE_STATUS_INVALID)

#define VRD_FORBID_POWER_ON 3
#define POWER_ON_FAILED_BY_VRD_UP ((VRD_FORBID_POWER_ON << 8) | COMP_CODE_STATUS_INVALID)

/* BEGIN: Modified by h00422363, 2017/12/21 16:10:42   问题单号:AR.SR.SFEA02109778.005.001  支持模拟精准告警 */
#define MAX_SUBJECT_INDEX 65535   // subjectindex最大值
#define MAX_LOG_ARR_LEN    512   // 日志数组最大长度
#define IN_OFFING_STATE 3
#define POWER_ON_FAILED_BY_IN_OFFING ((IN_OFFING_STATE) | COMP_CODE_STATUS_INVALID)
/* END:   Modified by h00422363, 2017/12/21 16:10:58 */

/* BEGIN: Modified by 00356691 zhongqiu, 2018/2/28   问题单号:DTS2018022306680 */
/* BEGIN: Modified by zwx406488 zhutengjiao, 2017/5/22 20:8:11   问题单号:  DTS2017042802883 */
#define MD_EPARA         DEF_ERROR_NUMBER(ERR_MODULE_ID_MD, -1)
#define MD_EBUSY         DEF_ERROR_NUMBER(ERR_MODULE_ID_MD, -7)
#ifdef DEF_ERROR_NUMBER
#undef DEF_ERROR_NUMBER // Hi1711平台PME定义了同名宏，APP层使用以APP层定义的宏为准
#define DEF_ERROR_NUMBER(module_id, err_num) (-1 * ((module_id) * ERR_MODULE_RANGE_SIZE + (err_num)))
#endif
/* END:   Modified by zwx406488 zhutengjiao, 2017/5/22 20:8:14 */
/* END: Modified by 00356691 zhongqiu, 2018/2/28   问题单号:DTS2018022306680 */



#define CACHEFS_ENALBED     1
#define CACHEFS_DISABLED    0


/*END: Added by l00375984, 2017/2/2, PN:DTS2017072708211 */


/* BEGIN: Added by z00416979, 2018/5/20  从diagnose模块移至公共头文件定义 PN:AR.SR.SFEA02130925.003.001 支持采集的数据列表 */
#define SQLITE3_PATH    "/usr/sbin/sqlite3"
/* END:   Added by z00416979, 2018/5/20  */

#define CACHEFS_ENALBED     1
#define CACHEFS_DISABLED    0

#define RPC_SERVER_PORT 51000
#define MAX_IPV4_LEN 16
#define MAX_IPV6_LEN 64
#define MAX_SERVICE_IP_NUM  8
#define VOS_TICK_PER_SECOND 1000
#define VOS_SECOND_PER_MINUTE 60
#define VOS_SECOND_PER_TEN_MINUTE 600

typedef struct sync_rpc_ip_port_s
{
    gchar ip[MAX_IPV4_LEN];    /* 监听IP */
    guint32 port;    /* 监听端口 */
}SYNC_RPC_IP_PORT_S;
typedef enum _tagUPDATE_TYPE
{
    UPDATE_UNKNOWN = 0,
    UPDATE_SELF,
    UPDATE_OTHER,
    UPDATE_BOTH,
    UPDATE_SHELF_FAN,
}UPDATE_TYPE;

typedef struct upgrade_remote_info
{
    gchar file_src_path[MAX_FILEPATH_LENGTH + 1];
    gchar ip[MAX_IPV4_LEN];
    guint16 port;
    guchar slot_id;
    gboolean remote_flag;
}UPGRADE_REMOTE_INFO;

typedef enum _tagRSYNC_STATE
{
    SYNC_UNKNOWN = 0,
    SYNC_TRANSFERING,
    SYNC_FINISHED,
    SYNC_ERROR,
}RSYNC_STATE;

/*SMM板主升备前文件传输详细信息*/
typedef struct tag_REMOTE_UPGRADE_INFO
{
    //向备用板上传升级文件的进度
    guint8   file_upload_progress;
    //向备用板上传文件的状态
    guint8    file_upload_status;
}REMOTE_UPGRADE_INFO;

/*SMM板主升备前文件传输状态*/
typedef enum tag_FILE_TRANS_STATUS
{
    FILE_TRANS_NO_TASK,
    FILE_TRANS_UNDERWAY,
    FILE_TRANS_OK,
    FILE_TRANS_FAILED,
    FILE_TRANS_TIME_OUT
}FILE_TRANS_STATUS;

/*SMM板对单板的管理状态*/
typedef enum tag_BLADE_MANAGE_STATUS
{
    MANAGE_OFF,
    MANAGE_ON,
}BLADE_MANAGE_STATUS;

/* BEGIN: Added by l00422028, 2017/12/13, SMM_MGNT需要本模块注册并处理的IPMI消息 */
enum BD_MSG_TYPE { // SMM_MGNT接收消息的类型枚举
    BD_MSG_UNSPECIFY,
    BD_MSG_PLATFORM_EVENT,
    BD_MSG_OEM_EVENT,
};

/* END: Added by l00422028, 2017/12/13*/

/*单板通信状态*/
#define BLADE_COMM_CHECK_OK             0x0        //单板状态检查正常
#define BLADE_COMMUNICATION_LOST        0x01       //单板通信丢失(M7)
#define BLADE_NOT_PRESENT               0x02       //单板不在位（M0)

/*设置和查询单板用户信息时的错误码*/
#define ERR_BLADE_USER_SET_OK           0x10        //设置用户信息成功
#define ERR_BLADE_USER_GET_OK           0x11        //查询用户信息成功

#define ERR_BLADE_USER_ALREADY_EXIST    0x13        //用户已存在
#define ERR_BLADE_USER_NOT_EXIST        0x14        //用户不存在
#define ERR_BLADE_USER_SET_FULL         0x15        //用户已满，不可以再添加
#define ERR_BLADE_USER_PARAM_ERROR      0x16        //参数非法（添加或删除root用户，修改密码、权限）

#define ERR_SHELF_INNER_SUBNET_PARAM_ERROR      0x20        //参数非法（设置机框内网网段IP地址参数错误）
#define ERR_SHELF_INNER_SUBNET_NOT_CHANGE      0x21         //内网网段不需要修改
#define ERR_SHELF_INNER_SUBNET_CONFLICT        0x22         //内网网段不需要修改

/*光模块在位状态*/
#define OPTICAL_MODULE_NOT_PRESENT  0
#define OPTICAL_MODULE_PRESENT      1
#define OPTICAL_FAULT_ALARM_TRIG    1
#define OPTICAL_FAULT_ALARM_CLEAR   0
#define OPTICAL_RX_LOS_SIGNAL_TRIG  1
#define OPTICAL_RX_LOS_SIGNAL_CLAER 0
/*1-设置为禁用，端口关闭；0-取消禁用，端口打开*/
#define OPTICAL_TX_DISABLE          1
#define OPTICAL_TX_ENABLE           0

/* chassis的长、宽、高，单位为mm，毫米 */
typedef struct tag_chassis_dimensions {
    guint32 depthMm;
    guint32 widthMm;
    guint32 heightMm;
} CHASSIS_DIMENSIONS_S;

#define MAX_EXTEND_FIELD_LEN    512
#define BYTE_ARRAY_META_LEN     32
#define REQ_MAX_DATA_LEN 256

#define  MAX_PRO_SN_LEN         128 /* 服务器设备最大的序列号长度 */

#define RETIMER_CHN_OPEN            0
#define RETIMER_CHN_CLOSE           1

/* BEGIN: Modified by zhangxiong z00454788, 2019/6/19   问题单号:DTS2019061215040 linklocal地址能够curl访问 */
#define GET_BIT_GUINT32_VALUE(VAL,N)    (((VAL) >> (N)) & (guint32)1)
#define CLR_BIT_GUINT32_VALUE(VAL,N)    ((VAL) & (~(((guint32)1)<<(N))))
#define SET_BIT_GUINT32_VALUE(VAL,N)    ((VAL) | (guint32)1 << (N))
/* END: Modified by zhangxiong z00454788, 2019/6/19   问题单号:DTS2019061215040 linklocal地址能够curl访问 */



#define SWI_CFG_FILE_MODE         0755
#define SWI_SLAVEADDR_BASE        0xC0
#define CFG_FILE_PATH   "/data/opt/pme/conf/bladecfg"
#define SWI_BASE_STR        "Base"
#define SWI_FABRIC_STR        "Fabric"
#define SWI_FCOE_STR        "Fcoe"
#define CFG_CURRENT_FLAG    "current"
#define SWITCH_CFG_FILE_BAK_PATH         "bakcfg"
#define CFG_FILE_PATH_MAX_NUM  6 /*需要保存的文件个数*/

#define URI_FORMAT_PS                                   "/redfish/v1/Chassis/%s/Power#/PowerSupplies/%d"
#define URI_FORMAT_FAN                                  "/redfish/v1/Chassis/%s/Thermal#/Fans/%d"
#define URI_FORMAT_CPU_PRE                              "/redfish/v1/Systems/%s/Processors"
#define URI_FORMAT_DIMM_PRE                             "/redfish/v1/Systems/%s/Memory"
#define URI_FORMAT_BOARD_PRE                            "/redfish/v1/Chassis/%s/Boards"
#define URI_FORMAT_CHASSIS_PRE                          "/redfish/v1/Chassis/%s"
#define URI_FORMAT_DISK_PRE                             "/redfish/v1/Chassis/%s/Drives"
#define RF_SYSTEMS_URI                                  "/redfish/v1/Systems/"


#define MUTUAL_AUTH_STATE_ERR 1

#define LOAD_XML_WAIT_OBJ_TIME      5
#define CHECK_OBJ_PERIOD            2000

#define LOG_MAX_LENGTH              512
#define COPY_FILE_DELETE_FLAG  1
#define COPY_FILE_RESERVE_FLAG 0

#define CALLER_INFO_NUM     3
#define CALLER_INTERFACE_OFFSET 0
#define CALLER_USERNAME_OFFSET 1
#define CALLER_CLIENT_OFFSET 2
#define CALLER_LOCAL_OFFSET 3
#define CALLER_ROLEID_OFFSET 4
#define CALLER_INFO_SIZE_WITH_USER_INFO 5

#define PCB_VERSION_LEN     10

#define RFACTION_BMC_ACTIVE_MODE_RESET               "ResetBMC"
#define RFACTION_BMC_ACTIVE_MODE_IMMEDIATELY         "Immediately"
#define RFACTION_BMC_ACTIVE_MODE_RESET_OS            "RestOS"
#define RFACTION_BMC_ACTIVE_MODE_RESET_SERVER        "RestServer"
#define RFACTION_BMC_ACTIVE_MODE_RESET_MM            "RestMM"

/* 华为公司ID */
#define MANUFACTURE_HUAWEI       0xdb0700
// retimer 5902
#define DELAY_BETWEEN_TWO_REQ     10
typedef enum {
    RETIMER_STAT_IDLE = 0x10,
    RETIMER_STAT_BUSY = 0x11,
    RETIMER_STAT_BUTT = 0xff
} SMBUS_BUSY_STAT_5902;

typedef enum {
    RETIMER_CMD_CODE_RW_REQ = 0x20,
    RETIMER_CMD_CODE_RW_RSP = 0x21,
    RETIMER_CMD_CODE_GETSTAT = 0x22
} RETIMER_SMBUS_CMD_CODE;

typedef enum {
    RETIMER_5902_UPG_IDLE = 0x0,
    RETIMER_5902_UPG_ON_GOING = 0x1,
    RETIMER_5902_UPG_DONE = 0x2,
    RETIMER_5902_UPG_VALID = 0x3,
} RETIMER_5902_UPG_STATUS;

typedef struct tagRETIMER_DRV_INTF_S {
    OBJ_HANDLE handle;
    guchar cmd;
    guchar *data;
    guint data_len;
    guchar retry_count;
} RETIMER_DRV_INTF_S;
typedef struct tagRETIMER_REQ_DATA_HEAD_S {
    guchar lun;
    guchar arg;
    guint16 opcode;
    guint32 offset;
    guint32 length;
} RETIMER_REQ_DATA_HEAD_S;


#define RETIMER_TYPE_5902L 0
#define RETIMER_TYPE_5902H 1
#define RETIMER_TYPE_5902L_NAME "CDR"
#define RETIMER_TYPE_5902H_NAME "Retimer"

#define RETIMER_IDLE 0
#define RETIMER_BUSY 1
#define RETIMER_UPG_RESULT_OK 0
#define RETIMER_UPG_RESULT_ERR 1
#define NOTIFY_DELAY_TIME        2000

typedef enum {
    RETIMER_CMD_GET_TEMP = 0x3,
    RETIMER_CMD_GET_FW_VER = 0x5,
    RETIMER_CMD_GET_LOG = 0xC,
    RETIMER_CMD_UPG_TRANS_FW = 0x18,
    RETIMER_CMD_UPG_CTRL = 0x19,
    RETIMER_CMD_UPG_GET_RES = 0x1A,
    RETIMER_CMD_SYNC_TIME = 0x21,
    RETIMER_CMD_REG_WRITE = 0x2A,
    RETIMER_CMD_REG_READ = 0x2B,
    RETIMER_CMD_FLASH_CHECK = 0x900,
    RETIMER_CMD_GET_HEART_BT = 0x901,
    RETIMER_CMD_INVALID,
    RETIMER_CMD_GET_SERDES_MAIN_INFO = 0xA0A,
    RETIMER_CMD_POWER_OPT = 0xA0E,
    RETIMER_CMD_GET_PCIE_STATUS = 0xB01,
    RETIMER_CMD_GET_PCIE_LTSSM = 0xB04,
    RETIMER_CMD_GET_PCIE_TRACE = 0xB06
} SMBUS_OPCODE_5902;

typedef enum {
    IPMI_OEM_BMC_CFG_LEAK_POLICY = 1,
    IPMI_OEM_BMC_CFG_MAINT_IP,
    IPMI_OEM_BMC_CFG_MAINT_SUBMASK,
    IPMI_OEM_BMC_CFG_IRM_HEARTBEAT_ENABLE = 11,
    IPMI_OEM_BMC_CFG_PCIE_NCSI_SLOTID = 12,
    IPMI_OEM_BMC_CFG_EXPECTED_MODULE_COUNT = 13,
    IPMI_OEM_BMC_CFG_WORKLOAD_NEEDED_PS_COUNT = 32,
    IPMI_OEM_BMC_CFG_CUSTOM_ALARM = 38
}IPMI_OEM_BMC_CFG_PARAM_SEL_E;

typedef struct tag_bmc_configuration_req {
    guint8  huawei_manu_id[MANUFAC_ID_LEN];
    guint8  sub_cmd;
    guint16 para_selector;
    guint16 cmd_length;
    const guint8 *cmd_data;
} BMC_CONFIGURATION_REQ_S;

typedef struct tag_bmc_configuration_rsp {
    guint8  completion_code;
    guint8  huawei_manu_id[MANUFAC_ID_LEN];
    guint8  data[1];
} BMC_CONFIGURATION_RSP_S;

typedef struct tag_bmc_file_caller {
    gboolean is_valid;
    gchar user_name[TMP_STR_MAX];
    gchar user_role[TMP_STR_MAX];
    gboolean is_local_user;
} BMC_FILE_CALLER_S;

/* lint -restore */
typedef enum tagBBU_STATE_TYPE_E {
    BBU_STATE_OFFLINE      = 0,   /* BBU 离线 初始化状态 */
    BBU_STATE_INIT         = 1,   /* BBU 初始化状态 */
    BBU_STATE_IDLE_OK      = 2,   /* 空闲，即不充电，也不放电 ，同时无故障 ，不一定满足备电要求 */
    BBU_STATE_ON_CHARGE    = 3,   /* 正在充电 */
    BBU_STATE_CHARGE_FULL  = 4,   /* 电池组充满电 */
    BBU_STATE_ON_DISCHARGE = 5,   /* 正在放电 */
    BBU_STATE_ONLINETEST   = 6,   /* 在线测试 */
    BBU_STATE_PROTECT      = 7,   /* 保护状态 */
    BBU_STATE_SHUT_DOWN    = 8,   /* 关机状态 */
    BBU_STATE_SLEEP        = 9,   /* 休眠状态 */
    BBU_STATE_BUTTY
} BBU_STATE_TYPE_E;

#define BYTE_BITS_NUM           8

#define BINARY_NUM              2
#define DECIMAL_NUM             10
#define HEX_NUM                 16

/* 用于解析Set/Get BMC Configuration的IPMI命令（netfn=0x30, cmd=0x93, subcmd=0x5a或0x5b） */
#define MANU_ID_1_INDEX         0
#define MANU_ID_2_INDEX         1
#define MANU_ID_3_INDEX         2
#define SUB_CMD_INDEX           3
#define PARAM_SEL_LOW_INDEX     4
#define PARAM_SEL_HIGH_INDEX    5
#define CMD_LEN_LOW_INDEX       6
#define CMD_LEN_HIGH_INDEX      7
#define CMD_DATA_START_INDEX    8

/* 网卡MAC地址有6个字节 */
#define MAC_LEN             6

// 定义事件通知的类型
#define EVENT_ADD 0 // 插入事件
#define EVENT_DEL 1 // 拔出事件

typedef enum {
    RF_SERVICE_ROOT = 0,    /* redfish根url: redfish/v1 */
    RF_SYSTEM_POWER = 1     /* 查询功率的url：redfish/v1/power */
}RF_INTERFACE_TYPE_E;

#define TLS_VERSION_UNSUPPORT_ERR                  (-3)
#define ENABLE_MUTUAL_AUTHENTICATIONS_ERR_WITH_TLS (-4)
#define TLS_ERR_WITH_MUTUAL_AUTHENTICATIONS        (-5)
#define TLS_1_0_MASK  0x01
#define TLS_1_1_MASK  0x02
#define TLS_1_2_MASK  0x04
#define TLS_1_3_MASK  0x08
#define TLS_MASK_MAX  (TLS_1_3_MASK)
#define TLS_VERSION_SUPPORT_COUNT  2

#define INNER_CREATE_FILE 0
#define OUTER_UPLOAD_FILE 1

#define FRU_TYPE_OCP3     83
#define SINGLE_MULTI_HEAD_LEN   5
#define MAX_MULTI_LEN    1024
#define MASK_BIT7     128
#define MAX_SINGLE_MULTI_LEN   256
#define ERR_CONNECTION (-200)
typedef struct {
    // refer to Table 16-2 from IPMI-Platform Management FRU Information Storage Definition-rev1.2.pdf
    guint8 rec_type;
    // bit 7 from the second byte of the MultiRecord aera 
    guint8 end_flag;
    guint8 len;
    guint8 rec_crc;
    guint8 head_crc;
    guint8 data[MAX_SINGLE_MULTI_LEN];
} ELABEL_MULTIRECORD_SINGLE_S;

#define PSU_FRU_PRODUCT_VERSION_BIT_MAP 0x00000001U

#define USB_PWD_INVALID_LEN (-2)
#define DEFAULT_BIOS_FLASH_COUNT   1

#define OOB_ERROR_CODE_NA "Error code: NA"
#define BIOS_VERIFY_ERROR_CODE 0x55AA  /* bios校验失败错误返回码 */

#ifdef __cplusplus
#if __cplusplus
}


#endif
#endif /* __cplusplus */

#endif /* __COMMON_MACRO_H__ */