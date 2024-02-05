/******************************************************************************

                  版权所有 (C), 2003-2006, 华为技术有限公司

******************************************************************************
  文 件 名   :  ipmi.h
  版 本 号   :
  作    者   :
  生成日期   :
  最近修改   :
  功能描述   :  IPMI 相关模块公用头文件
  函数列表   :
  修改历史   :
  1.日    期   :
    作    者   :zongmingli
    修改内容   :

******************************************************************************/

#ifndef __IPMI_H__
#define __IPMI_H__

#include <glib.h>
#include "ipmi_oem.h"

/*用户级别*/
#define RMCP_CALLBACK   1
#define RMCP_USER       2
#define RMCP_OPERATOR   3
#define RMCP_ADMIN      4
#define RMCP_OEM        5

// complete code define
// Command Completed Normally.
#define COMP_CODE_SUCCESS 0x00

// Node Busy.
#define COMP_CODE_BUSY 0xC0

// Invalid Command. Used to indicate an unrecognized or unsupported command.
#define COMP_CODE_INVALID_CMD 0xC1

// Command invalid for given LUN.
#define COMP_CODE_CMD_INVALID 0xC2

// Timeout while processing command. Response unavailable.
#define COMP_CODE_TIMEOUT 0xC3

// Out of space. Command could not be completed because of a lack of
//   storage space required to execute the given command operation.
#define COMP_CODE_MEMOUT 0xC4

// Reservation Canceled or Invalid Reservation ID.
#define COMP_CODE_INVALID_RSVID 0xC5

// Request data truncated.
#define COMP_CODE_DATA_TRUNCATED 0xC6

// Request data length invalid.
#define COMP_CODE_LEN_INVALID 0xC7

// Request data field length limit exceeded.
#define COMP_CODE_DATA_LEN_LARGE 0xC8

// Parameter out of range.
#define COMP_CODE_OUTOF_RANGE 0xC9

// Cannot return number of requested data bytes.
#define COMP_CODE_CANNOT_RET_DATA 0xCA

// Requested Sensor, data, or record not present.
#define COMP_CODE_INVALID_ID 0xCB

// Invalid data field in Request
#define COMP_CODE_INVALID_FIELD 0xCC

// Command illegal for specified sensor or record type.
#define COMP_CODE_ILLEGAL 0xCD

// Command response could not be provided.
#define COMP_CODE_CANNOT_RESPONSE 0xCE

// Cannot execute duplicated request.
#define COMP_CODE_CANNOT_EXEC_DUPREQ 0xCF

// Command response could not be provided. SDR Repository in update mode.
#define COMP_CODE_SDR_UPDATING 0xD0

// Command response could not be provided. Device in firmware update mode.
#define COMP_CODE_FW_UPDATING 0xD1

// Command response could not be provided. BMC initialization or initialization agent in progress.
#define COMP_CODE_FW_INITIALIZATION 0xD2

// Destination unavailable. Cannot deliver request to selected destination.
#define COMP_CODE_DES_UNAVAILABLE 0xD3

// Cannot execute command due to insufficient privilege level or other security-based restriction
#define COMP_CODE_PRIVILEGE_LIMITED 0xD4

// Cannot execute command. Command, or request parameter(s), not supported in present state.
#define COMP_CODE_STATUS_INVALID 0xD5

// Cannot execute command. Parameter is illegal because command sub-function has been disabled or is unavailable
#define COMP_CODE_SUBFUNC_INVALID 0xD6

// Unknown cause
#define COMP_CODE_UNKNOWN 0xFF

// 增加校验错
#define COMP_CODE_CRC_ERROR 0xE0

#define COMP_CODE_DATA_NOT_AVAILABLE 0x80
#define COMP_CODE_CMD_IN_PROCESS 0x82

#define COMP_CODE_PARA_NOT_SUPPORT 0x80
#define COMP_CODE_PARA_IS_SETTING 0x81
#define COMP_CODE_PARA_IS_READONLY 0x82

/*上述错误码之后再整理添加*/

/*网络功能码*/
#define NETFN_CHASSIS_REQ 0X00
#define NETFN_CHASSIS_RESP 0X01

#define NETFN_SENSOR_EVENT_REQ 0X04
#define NETFN_SENSOR_EVENT_RESP 0X05

#define NETFN_APP_REQ 0X06
#define NETFN_APP_RESP 0X07

#define NETFN_FIRMWARE_REQ 0X08
#define NETFN_FIRMWARE_RESP 0X09

#define NETFN_STORAGE_REQ 0X0a
#define NETFN_STORAGE_RESP 0X0b

#define NETFN_TRANSPORT_REQ 0X0c
#define NETFN_TRANSPORT_RESP 0X0d

#define NETFN_PICMG_REQ 0X2C
#define NETFN_PICMG_RESP 0X2D

#define NETFN_OEM_REQ 0X30
#define NETFN_OEM_RESP 0X31
#define NETFN_PANGEA_OEM 0x32

#define NETFN_INTEL_2E 0x2E
#define NETFN_INTEL_SENSOR 0x04


/*IPMI 命令定义*/
/*APP*/
#define IPMI_GET_DEVICE_ID 0x01
#define IPMI_COLD_RESET 0x02
#define IPMI_GET_SELF_TEST_RESULTS 0x04
#define IPMI_GET_DEVICE_GUID 0x08
#define IPMI_RESET_WATCHDOG_TIMER 0x22
#define IPMI_SET_WATCHDOG_TIMER 0x24
#define IPMI_GET_WATCHDOG_TIMER 0x25
#define IPMI_SET_BMC_GLOBAL_ENABLES 0x2E
#define IPMI_GET_BMC_GLOBAL_ENABLES 0x2F
#define IPMI_CLEAR_MESSAGE_FLAGS 0x30
#define IPMI_GET_MESSAGE_FLAGS 0x31
#define IPMI_GET_MESSAGE 0x33
#define IPMI_SEND_MESSAGE 0x34
#define IPMI_READ_EVENT_MESSAGE_BUFFER 0x35
#define IPMI_GET_SYSTEM_GUID 0x37
#define IPMI_GET_CHANNEL_AUTHENTICATION_CAPABILITIES 0x38
#define IPMI_GET_SESSION_CHALLEGE 0x39
#define IPMI_ACTIVE_SESSION                           0x3A
#define IPMI_SET_SESSION_LEVEL 0x3B //add for rmcp
#define IPMI_CLOSE_SESSION 0x3C
#define IPMI_GET_SESSION_INFO 0x3D
#define IPMI_GET_AUTH_CODE 0x3F
#define IPMI_SET_CHANNEL_ACCESS 0x40
#define IPMI_GET_CHANNEL_ACCESS 0x41
#define IPMI_GET_CHANNEL_INFO 0x42
#define IPMI_SET_USER_ACCESS_COMMAND 0x43
#define IPMI_GET_USER_ACCESS_COMMAND 0x44
#define IPMI_SET_USER_NAME_COMMAND 0x45
#define IPMI_GET_USER_NAME_COMMAND 0x46
#define IPMI_SET_USER_PASSWORD_COMMAND 0x47
#define IPMI_ACTIVATE_PAYLOAD 0x48
#define IPMI_DEACTIVATE_PAYLOAD 0x49
#define IPMI_GET_PAYLOAD_ACTIVATION_STATUS 0x4A
#define IPMI_GET_PAYLOAD_INSTANCE_INFO 0x4B
#define IPMI_SET_USER_PAYLOAD_ACCESS 0x4C
#define IPMI_GET_USER_PAYLOAD_ACCESS 0x4D
#define IPMI_GET_CHANNEL_PAYLOAD_SUPPORT 0x4E
#define IPMI_GET_CHANNEL_PAYLOAD_VERSION 0x4F
#define IPMI_MASTER_WRITEREAD 0x52
#define IPMI_GET_CHANNEL_CIPHER_SUITES 0x54
#define IPMI_SUSPENDRESUME_PAYLOAD_ENCRYPTION 0x55
#define IPMI_SET_CHANNEL_SECURITY_KEYS 0x56
#define IPMI_GET_SYSTEM_INTERFACE_CAPABILITIES 0x57
#define IPMI_SET_SYSTEM_INFO_PARAMETERS 0x58
#define IPMI_GET_SYSTEM_INFO_PARAMETERS 0x59

/*CHASSIS*/
#define IPMI_GET_CHASSIS_CAPABILITIES 0x00
#define IPMI_GET_CHASSIS_STATUS 0x01
#define IPMI_CHASSIS_CONTROL 0x02
#define IPMI_CHASSIS_IDENTIFY 0x04

#define IPMI_SET_POWER_RESTORE_POLICY 0x06

#define IPMI_GET_SYSTEM_RESTART_CAUSE 0x07
#define IPMI_SET_SYSTEM_BOOT_OPTIONS 0x08
#define IPMI_GET_SYSTEM_BOOT_OPTIONS 0x09

#define IPMI_SET_POWER_CYCLE_INTERVAL 0x0B

/*S/E*/

//------Event Commands----------------------------------------------
#define IPMI_SET_EVENT_RECEIVER 0x00
#define IPMI_GET_EVENT_RECEIVER 0x01
#define IPMI_PLATFORM_EVENT 0x02

//------PEF and Alerting Commands-----------------------------------
#define IPMI_GET_PEF_CAPABILITIES 0x10
#define IPMI_ARM_PEF_POSTPONE_TIMER 0x11
#define IPMI_SET_PEF_CONFIGURATION_PARAMETERS 0x12
#define IPMI_GET_PEF_CONFIGURATION_PARAMETERS 0x13
#define IPMI_SET_LAST_PROCESSED_EVENT_ID 0x14
#define IPMI_GET_LAST_PROCESSED_EVENT_ID 0x15
#define IPMI_ALERT_IMMEDIATE 0x16
#define IPMI_PET_ACKNOWLEDGE 0x17

//------Sensor Device Commands--------------------------------------
#define IPMI_GET_DEVICE_SDR_INFO 0x20
#define IPMI_GET_DEVICE_SDR 0x21
#define IPMI_RESERVE_DEVICE_SDR_REPOSITORY 0x22
#define IPMI_SET_SENSOR_HYSTERESIS 0x24
#define IPMI_GET_SENSOR_HYSTERESIS 0x25
#define IPMI_SET_SENSOR_THRESHOLD 0x26
#define IPMI_GET_SENSOR_THRESHOLD 0x27
#define IPMI_SET_SENSOR_EVENT_ENABLE 0x28
#define IPMI_GET_SENSOR_EVENT_ENABLE 0x29
#define IPMI_REARM_SENSOR_EVENTS 0x2A
#define IPMI_GET_SENSOR_EVENT_STATUS 0x2B
#define IPMI_GET_SENSOR_READING 0x2D
#define IPMI_GET_SENSOR_TYPE 0x2F

/*STORAGE*/
#define IPMI_GET_FRU_INVENTORY_AREA_INFO 0x10
#define IPMI_READ_FRU_DATA 0x11
#define IPMI_WRITE_FRU_DATA 0x12

#define IPMI_GET_SDR_REPOSITORY_INFO 0x20
#define IPMI_GET_SDR_REPOSITORY_ALLOCATION_INFO 0X21
#define IPMI_RESERVE_SDR_REPOSITORY 0x22
#define IPMI_GET_SDR 0x23
#define IPMI_ADD_SDR 0x24
#define IPMI_PARTIAL_ADD_SDR 0x25
#define IPMI_DELETE_SDR 0x26
#define IPMI_CLEAR_SDR_REPOSITORY 0x27
#define IPMI_GET_SDR_REPOSITORY_TIME 0x28
#define IPMI_SET_SDR_REPOSITORY_TIME 0x29
#define IPMI_ENTER_SDR_REPOSITORY_UPDATE_MODE 0x2A
#define IPMI_EXIT_SDR_REPOSITORY_UPDATE_MODE 0x2B
#define IPMI_RUN_INITIALIZATION_AGENT 0x2C

#define IPMI_GET_SEL_INFO 0x40
#define IPMI_GET_SEL_ALLOCATION_INFO 0x41
#define IPMI_RESERVE_SEL 0x42
#define IPMI_GET_SEL_ENTRY 0x43
#define IPMI_ADD_SEL_ENTRY 0x44
#define IPMI_CLEAR_SEL 0x47
#define IPMI_GET_SEL_TIME 0x48
#define IPMI_SET_SEL_TIME 0x49
#define IPMI_GET_UTC_OFFSET 0X5C
#define IPMI_SET_UTC_OFFSET 0X5D

/*TRANSPORT*/
#define IPMI_SET_LAN_CONFIGURATION_PARAMETERS 0x01
#define IPMI_GET_LAN_CONFIGURATION_PARAMETERS 0x02
#define IPMI_SET_SOL_CONFIGURATION_PARAMETERS 0x21
#define IPMI_GET_SOL_CONFIGURATION_PARAMETERS 0x22

/*PICMG*/
#define IPMI_GET_PICMG_PROPERTIES 0x00
#define IPMI_GET_ADDRESS_INFO 0x01
#define IPMI_GET_SHELF_ADDRESS_INFO 0x02
#define IPMI_SET_SHELF_ADDRESS_INFO 0x03
#define IPMI_FRU_CONTROL 0x04
#define IPMI_GET_FRU_LED_PROPERTIES 0x05
#define IPMI_GET_LED_COLOR_CAPABILITIES 0x06
#define IPMI_SET_FRU_LED_STATE 0x07
#define IPMI_GET_FRU_LED_STATE 0x08
#define IPMI_SET_IPMB_STATE 0x09
#define IPMI_SET_FRU_ACTIVATION_POLICY 0x0A
#define IPMI_GET_FRU_ACTIVATION_POLICY 0x0B
#define IPMI_SET_FRU_ACTIVATION 0x0C
#define IPMI_GET_DEVICE_LOCATOR_RECORD_ID 0x0D
#define IPMI_SET_PORT_STATE 0x0E
#define IPMI_GET_PORT_STATE 0x0F
#define IPMI_COMPUTE_POWER_PROPERTIES 0x10
#define IPMI_SET_POWER_LEVEL 0x11
#define IPMI_GET_POWER_LEVEL 0x12
#define IPMI_RENEGOTIATE_POWER 0x13

/* BEGIN: Added by h00164462, 2010/7/19, PN:BU1D00491 */
#define IPMI_GET_FAN_SPEED_PROPERTIES 0x14
/* END: Added by h00164462, 2010/7/19, PN:BU1D00491 */
#define IPMI_SET_FAN_LEVEL 0x15
#define IPMI_GET_FAN_LEVEL 0x16

#define IPMI_FRU_CONTROL_CAPABILITIES 0x1E
#define IPMI_GET_TARGET_UPGRADE_CAPABILITIES 0x2E
#define IPMI_GET_COMPONENT_PROPERTIES 0x2F
#define IPMI_ABORT_FIRMWARE_UPGRADE 0x30
#define IPMI_INITIATE_UPGRADE_ACTION 0x31
#define IPMI_UPLOAD_FIRMWARE_BLOCK 0x32
#define IPMI_FINISH_FIRMWARE_UPLOAD 0x33
#define IPMI_GET_UPGRADE_STATUS 0x34
#define IPMI_ACTIVATE_FIRMWARE 0x35
#define IPMI_QUERY_SELFTEST_RESULTS 0x36
#define IPMI_QUERY_ROLLBACK_STATUS 0x37
#define IPMI_INITIATE_MANUAL_ROLLBACK 0x38


#define IPMI_REQ_OFFSET 7     /*IPMI消息中头尾的长度共7字节*/
#define CHANNEL_LEN_LIMIT 255


typedef struct tg_ipmi_session
{
    guint32 session_id;
    gchar user_ip[47];        //用户IP,兼容IPV6
    gchar user_name[32];      //用户名，记日志需要
    guint16 user_port;      //客户端的port，DTS2017102318954
} IPMI_SESSION_S;


/*新增接口*/
/*************************************************
目标类型枚举
 **************************************************/
typedef enum
{
    IPMI_SMM,   /*IPMIChannel->ChanType:0*/
    IPMI_ME,    /*IPMIChannel->ChanType:1*/
    IPMI_MMC,   /*IPMIChannel->ChanType:2*/
    IPMI_HOST,  /*IPMIChannel->ChanType:3*/
    IPMI_LAN,   /*IPMIChannel->ChanType:4*/
    IPMI_BASE,  /*IPMIChannel->ChanType:5*/
    IPMI_FABRIC,/*IPMIChannel->ChanType:6*/
    IPMI_FCOE,   /*IPMIChannel->ChanType:7*/
	IPMI_BMC    /*IPMIChannel->ChanType:8*/
} TARGET_TYPE_E;

/*APP发送IPMI请求时需要填写的数据内容*/
typedef struct tag_ipmi_req_msg_head_s
{
    TARGET_TYPE_E target_type;          //需要将此消息发送到哪里，eg:smm,me,mmc
    guint8 target_instance;             //默认为0，如MMC、ME有多个需要填写
    guint8 lun    : 2;
    guint8 netfn  : 6;
    guint8 cmd;
    guint8 src_len;                     //src_data的有效长度
}IPMI_REQ_MSG_HEAD_S;

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/*****************************************************************************
 函 数 名  : get_ipmi_target_type
 功能描述  : APP调用此函数获取IPMI消息的目标类型
 输入参数  : gpointer ipmi_msg
 输出参数  : 无
 返 回 值  : TARGET_TYPE_E

 修改历史      :
  1.日    期   : 2013年8月16日
    作    者   : zongmingli
    修改内容   : 新生成函数

*****************************************************************************/
extern TARGET_TYPE_E get_ipmi_target_type(gconstpointer ipmi_msg);

/*****************************************************************************
 函 数 名  : get_ipmi_chan_num
 功能描述  : APP调用此函数获取IPMI消息来自哪个通道
 输入参数  : gpointer ipmi_msg
 输出参数  : 无
 返 回 值  : gint32 查询成功返回通道号，失败返回-1

 修改历史      :
  1.日    期   : 2013年8月16日
    作    者   : zongmingli
    修改内容   : 新生成函数

*****************************************************************************/
extern gint32 get_ipmi_chan_num(gconstpointer ipmi_msg);

/*****************************************************************************
 函 数 名  : get_ipmi_rmcp_info
 功能描述  : 当IPMI命令来之LAN时，APP调用此函数获session信息(用户名，用户IP)

 输入参数  : gpointer ipmi_msg
 输出参数  : 无
 返 回 值  : IPMI_SESSION_S*

 修改历史      :
  1.日    期   : 2013年8月16日
    作    者   : zongmingli
    修改内容   : 新生成函数

*****************************************************************************/
extern const IPMI_SESSION_S* get_ipmi_rmcp_info(gconstpointer ipmi_msg);

/*****************************************************************************
 函 数 名  : get_ipmi_req_data
 功能描述  : APP调用此函数获取IPMI消息的数据部分，即src_data
 输入参数  : gpointer ipmi_msg
 输出参数  : 无
 返 回 值  : const guint8*

 修改历史      :
  1.日    期   : 2013年8月16日
    作    者   : zongmingli
    修改内容   : 新生成函数

*****************************************************************************/
extern const guint8* get_ipmi_src_data(gconstpointer ipmi_msg);


/*****************************************************************************
 函 数 名  : get_ipmi_src_data_len
 功能描述  : APP调用此函数获取IPMI消息src_data的长度
 输入参数  : gconstpointer ipmi_msg
 输出参数  : 无
 返 回 值  : extern guint8

 修改历史      :
  1.日    期   : 2013年8月19日
    作    者   : zongmingli
    修改内容   : 新生成函数

*****************************************************************************/
extern guint8 get_ipmi_src_data_len(gconstpointer ipmi_msg);


/*****************************************************************************
 函 数 名  : ipmi_send_request
 功能描述  : APP调用此函数发送IPMI请求
 输入参数  : IPMI_REQ_MSG_HEAD_S* header  请求消息头部，APP自己填写
             gpointer req_data            请求消息data
             gsize resp_size              响应data长度
             gboolean wait_response       是否需要等待返回响应
 输出参数  : gpointer* response           响应消息地址 (注:APP调用完需要释放!!!!)
 返 回 值  : extern gint32

 修改历史      :
  1.日    期   : 2013年8月19日
    作    者   : zongmingli
    修改内容   : 新生成函数

*****************************************************************************/
extern gint32 ipmi_send_request(IPMI_REQ_MSG_HEAD_S* header, gconstpointer req_data, gsize resp_size, gpointer* response, gboolean wait_response);

/*****************************************************************************
 函 数 名  : ipmi_send_proxy_message
 功能描述  : APP调用此接口转发IPMI消息到其他控制器处理
 输入参数  : IPMI_REQ_MSG_HEAD_S* header
             gconstpointer req_data
 输出参数  : 无
 返 回 值  : extern gint32

 修改历史      :
  1.日    期   : 2014年6月3日
    作    者   : zongmingli
    修改内容   : 新生成函数

*****************************************************************************/
extern gint32 ipmi_send_proxy_message(IPMI_REQ_MSG_HEAD_S* header, gconstpointer req_data);

/*****************************************************************************
 函 数 名  : ipmi_send_response
 功能描述  : APP调用此函数发送IPMI响应
 输入参数  : gpointer req_msg    请求消息
             guin8 resp_len      响应消息data长度
             guint8*resp_data    响应消息data
 输出参数  : 无
 返 回 值  : extern gint32

 修改历史      :
  1.日    期   : 2013年8月19日
    作    者   : zongmingli
    修改内容   : 新生成函数

*****************************************************************************/
extern gint32 ipmi_send_response(gconstpointer req_msg, guint8 resp_len, guint8*resp_data ) ;

/*****************************************************************************
 函 数 名  : ipmi_simple_response
 功能描述  : 返回一个简单的IPMI响应
 输入参数  : gconstpointer req_msg  请求消息
             guint8 comp_code       返回码
 输出参数  : 无
 返 回 值  : extern gint32

 修改历史      :
  1.日    期   : 2013年8月19日
    作    者   : zongmingli
    修改内容   : 新生成函数

*****************************************************************************/
extern gint32 ipmi_send_simple_response(gconstpointer req_msg,guint8 comp_code);

/*****************************************************************************
 函 数 名  : get_ipmi_netfn
 功能描述  : APP调用此函数获取IPMI消息的网络功能码
 输入参数  : gconstpointer ipmi_msg
 输出参数  : 无
 返 回 值  : guint8

 修改历史      :
  1.日    期   : 2013年8月22日
    作    者   : zongmingli
    修改内容   : 新生成函数

*****************************************************************************/
extern guint8 get_ipmi_netfn(gconstpointer ipmi_msg);

/*****************************************************************************
 函 数 名  : get_ipmi_cmd
 功能描述  : APP调用此函数获取IPMI消息的命令字
 输入参数  : gconstpointer ipmi_msg
 输出参数  : 无
 返 回 值  : guint8

 修改历史      :
  1.日    期   : 2013年8月22日
    作    者   : zongmingli
    修改内容   : 新生成函数

*****************************************************************************/
extern guint8 get_ipmi_cmd(gconstpointer ipmi_msg);

/*****************************************************************************
 函 数 名  : get_ipmi_lun
 功能描述  : APP调用此函数获取IPMI消息的src_lun
 输入参数  : gconstpointer ipmi_msg
 输出参数  : 无
 返 回 值  : guint8

 修改历史      :
  1.日    期   : 2013年8月22日
    作    者   : zongmingli
    修改内容   : 新生成函数

*****************************************************************************/
extern guint8 get_ipmi_src_lun(gconstpointer ipmi_msg);

/*****************************************************************************
 函 数 名  : get_ipmi_head_len
 功能描述  : APP调用此函数获取IPMI消息的头部结构体长度
 输出参数  : 无
 返 回 值  : extern guint8

 修改历史      :
  1.日    期   : 2014年7月29日
    作    者   : zongmingli
    修改内容   : 新生成函数

*****************************************************************************/
extern guint8 get_ipmi_head_len(void);

/*****************************************************************************
 函 数 名  : get_ipmi_src_seq
 功能描述  : APP调用此函数获取IPMI消息的src_seq
 输入参数  : gconstpointer ipmi_msg  
 输出参数  : 无
 返 回 值  : extern guint8
 
 修改历史      :
  1.日    期   : 2014年11月11日
    作    者   : zongmingli
    修改内容   : 新生成函数

*****************************************************************************/
extern guint8 get_ipmi_src_seq(gconstpointer ipmi_msg);

/*****************************************************************************
 函 数 名  : get_ipmi_src_addr
 功能描述  : APP调用此函数获取IPMI消息的src_addr
 输入参数  : gconstpointer ipmi_msg
 输出参数  : 无
 返 回 值  : guint8

 修改历史      :
  1.日    期   : 2013年8月22日
    作    者   : zongmingli
    修改内容   : 新生成函数

*****************************************************************************/
extern guint8 get_ipmi_src_addr(gconstpointer ipmi_msg);

#define ipmi_operation_log(reg_msg, format, arg...) \
do  \
{   \
    TARGET_TYPE_E macro_target_type = get_ipmi_target_type(reg_msg);\
    const IPMI_SESSION_S *macro_ipmi_session_ptr = NULL;\
    const gchar *macro_module_name = dfl_get_module_name();\
    macro_ipmi_session_ptr = get_ipmi_rmcp_info(reg_msg);\
    const gchar *macro_user_name = macro_ipmi_session_ptr->user_name;\
    gchar macro_client_buf[65];\
    gchar *macro_client = macro_client_buf;\
    const gchar *macro_target_type_name[] ={\
        "HMM",\
        "ME",\
        "MMC",\
        "HOST",\
        "LAN"\
    };\
    gchar *macro_default_name = "N/A";\
    guint32 macro_index = (guint32)macro_target_type;\
    macro_index -= (guint32)IPMI_SMM;\
    if (macro_index == 0x9)\
    {\
        macro_index = 0x3;\
    }\
    memset_s(macro_client_buf, sizeof(macro_client_buf), 0, sizeof(macro_client_buf));\
    if (macro_ipmi_session_ptr->user_name[0] != 0)\
    {\
        (void)snprintf_s(macro_client_buf, sizeof(macro_client_buf) - 1,sizeof(macro_client_buf) - 1, "%s", macro_ipmi_session_ptr->user_ip);\
    }\
    else\
    {\
        macro_user_name = macro_default_name;\
        if (macro_index >= (sizeof(macro_target_type_name)/sizeof(macro_target_type_name[0])))\
        {\
            macro_client = macro_default_name;\
        }\
        else\
        {\
            (void)snprintf_s(macro_client_buf, sizeof(macro_client_buf) - 1,sizeof(macro_client_buf) - 1, "%s", macro_target_type_name[macro_index]);\
        }\
    }\
    operation_log("IPMI", macro_user_name, macro_client, macro_module_name, format, ##arg);\
} while (0)


/*****************************************************************************
 函 数 名  : ipmimsg_to_callerinfo
 功能描述  : 从IPMI请求消息中提取callerinfo。APP可通过此API获取到callerinfo，再调用 method_operation_log 记录操作日志. 做到操作日志归一化.
 输入参数  : gconstpointer ipmi_msg
 输出参数  : 无
 返 回 值  : callerinfo 链表

 修改历史	   :
  1.日	  期   : 2014年3月14日
	作	  者   : loujianping
	修改内容   : 新生成函数
*****************************************************************************/
extern GSList* ipmimsg_to_callerinfo(const void* req_msg);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __IPMI_H__ */
