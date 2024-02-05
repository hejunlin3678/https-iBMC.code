/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2021. All rights reserved.
 * Description: asm 对外接口头文件
 * Author: d00384307
 * Create: 2017年5月23日
 * History: 2017年5月23日 d00384307 创建文件
 */

#ifndef __ASM_PUBLIC_H__
#define __ASM_PUBLIC_H__

#include "pme/pme.h"
#include "pme_app/pme_app.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define SLOT_SMM1   1
#define SLOT_SMM2   2

#define OPERATOR_LOG_CLI  "CLI"

    /* 各信号状态定义 */
#define DEFAULT_AS_STATUS           0xff
#define ACTIVE_STATE                0
#define STANDBY_STATE               1
#define ACTIVE_STATE_SIGNAL_FAULT   2

#define FAILOVER_FAIL_FROM_LOCAL   1
#define FAILOVER_FAIL_FROM_REMOTE  2

#define BOARD_IN_OK                 1
#define BOARD_IN_NOK                0

#define BOARD_HEALTH                1
#define BOARD_NOT_HEALTH            0
#define HEALTH_SIGNAL_FAULT         2

#define BOARD_IPMB_CONNECTED        1
#define BOARD_IPMB_DISCONNECTED     0

#define BOARD_FLASH_STATUS_OK       1
#define BOARD_FLASH_STATUS_NOT_OK   0

#define BOARD_NO_CRITICAL_ALARM     1
#define BOARD_CRITICAL_ALARM        0

#define BOARD_PRESENT               1 // 在位
#define BOARD_NOT_PRESENT           0 // 不在位

#define BACKUP_NOT_READY            1
#define BACKUP_READY                0

#define FAILOVER_ALLOW              0
#define FAILOVER_DISALLOW           1

#define REMOTE_NOT_HEALTH           1
#define REMOTE_HEALTH               0

#define ASM_ACTIVE_SIGNAL_FAULT 0
#define ASM_ACTIVE_SIGNAL_NORMAL 1
#define ASM_ACTIVE_SIGNAL_CHECKING 2
#define ASM_REMOTE_HEARTBEAT_FAULT 0
#define ASM_REMOTE_HEARTBEAT_NORMAL 1
#define ASM_REMOTE_HEARTBEAT_CHECKING 2

#define FILESYNC_COMPLETE 100

#define SMM1_IPMB 0x10
#define SMM2_IPMB 0x12
#define ACT_SMM_IPMB 0x20
#define MAINBOARD_EVENT_CODE 0x10000000
#define LSW_EVENT_CODE 0x2a000000
#define PEM_EVENT_CODE 0x03000000
#define FANTRAY_EVENT_CODE 0x04000000
#define EVENT_ALARM_LEVEL_COUNT 3

#define EMM_DEFAULT_SA_MULTI 2

/* BEGIN: Added by l00419687, 2017-12-26, 原因:SMM板上扣卡BMC单板slot号 */
#define SMM1_BMC_BLADE_SLOT_ID       37
#define SMM2_BMC_BLADE_SLOT_ID       38
#define SWI1_SLOT_ID       33
#define SWI4_SLOT_ID       36
#define BLADE_TYPE_COMPUTE_NODE      0
#define BLADE_TYPE_SWI_MODULE        0xC0
/* END: Added by l00419687, 2017-12-26 */
#define FAN_FRU_POSITION_BASE   2
#define PEM_FRU_POSITION_BASE   20
/* 热插拔状态宏定义 */
#define FRU_HOTSWAP_M0              0     /* 说明: 非M0及M7状态表示FRU在位 */
#define FRU_HOTSWAP_M1              1
#define FRU_HOTSWAP_M2              2
#define FRU_HOTSWAP_M3              3
#define FRU_HOTSWAP_M4              4
#define FRU_HOTSWAP_M5              5
#define FRU_HOTSWAP_M6              6
#define FRU_HOTSWAP_M7              7

#define FAN_LOG_NAME "fanspeed_output"

#define SMM_IPMI_CHANNEL_OK_FILE "/dev/shm/ipmi_channel_ok"

#define SMM_BOTH_DUMP_FAILED            (-1)
#define SMM_STDBY_DUMP_FAILED           (-2)
#define SMM_ACTIVE_DUMP_FAILED          (-3)

#define MAX_IPMBETH_LEN 32

#define NEW_JSON_OBJ(obj, exp)   \
    do{ \
        obj = json_object_new_object(); \
        if (obj == NULL) { \
            debug_log(DLOG_ERROR, "[%s] Create new json object failed", __func__); \
            exp; \
        } \
    }while(0);

#define NEW_JSON_ARR(arr, exp)   \
    do{ \
        arr = json_object_new_array(); \
        if (arr == NULL) { \
            debug_log(DLOG_ERROR, "[%s] Create new json array failed", __func__); \
            exp; \
        } \
    }while(0);


#define ERROR_INPUT_PARAMETER_FILENAME   (-2)
#define ERROR_FILE_NOT_EXIST             (-3)
#define ERROR_FILE_LENGTH                (-4)
#define ERROR_NOT_SUPPORT                (-5)
#define ERROR_HOT_SWAP                   (-6)
#define ERROR_COPY_FILE                  (-7)
#define OFFLINECFG_TIME_OUT              (-8)
#define ERROR_INPUT_PARAMETER_BLADETYPE  (-9)

#define LUN_CODE (0)

enum _SITE_TYPE_DEF
{
    ST_ATCA_BOARD,
    ST_POWER_ENTRY,
    ST_SHELF_FRU_INFO,
    ST_DEDICATED_SHMC,
    ST_FANTRAY,
    ST_FAN_FILTER_TRAY,
    ST_ALARM_BOARD,
    ST_ATCA_MODULE,
    ST_PMC,
    ST_RTM,
    ST_POWER_CHASSIS,           //新增电源框类型
    ST_LIU_XCU = 0xC0,             //增加LIU,XCU类型，他们的槽位类型是一样的, 交换模块
    ST_ALL_TYPE = 0xFF
};


/* 倒换发起原因定义 */
typedef enum em_asm_failover_cause{
    ASM_FAILOVER_OCCURRED_BY_INIT_STATE = 0,
    ASM_FAILOVER_OCCURRED_BY_USER_COMMAND,
    ASM_FAILOVER_OCCURRED_BY_SMM_M7,
    ASM_FAILOVER_OCCURRED_BY_SMM_HEALTH,
    ASM_FAILOVER_OCCURRED_BY_CHASSISID_FAIL,
    ASM_FAILOVER_OCCURRED_BY_CFG_FILE_ERR,
    ASM_FAILOVER_OCCURRED_BY_ETH_STATE,
    ASM_FAILOVER_OCCURRED_BY_IPMI_FAIL,
    ASM_FAILOVER_OCCURRED_BY_PROCESS_FAIL,
    ASM_FAILOVER_OCCURRED_BY_CRITICAL_ALARM,
    ASM_FAILOVER_OCCURRED_BY_SPECIAL_ALARM,
    ASM_FAILOVER_OCCURRED_BY_SLOT_ID_FAULT,
    ASM_FAILOVER_OCCURRED_BY_BOARD_UNSTABLE,
    ASM_FAILOVER_TO_STANDBY_BY_UNKNOWN_REASON = (0XFF)
}ASM_FAILOVER_CAUSE;

/* 倒换失败原因定义 */
typedef enum em_asm_failover_fail_cause{
    ASM_FAILOVER_SUCCESS = 0x00,
    ASM_FAILOVER_CANNOT_BECAUSE_STANDBY_NOT_PRESENT = (0x01),
    ASM_FAILOVER_CANNOT_BECAUSE_OTHER_SMM_NOT_HEALTH,
    ASM_FAILOVER_CANNOT_BECAUSE_NETWORK_DISCONNECT,
    ASM_FAILOVER_CANNOT_BECAUSE_LOCAL_SMM_HAS_CRITICAL_EVENT,
    ASM_FAILOVER_CANNOT_BECAUSE_OTHER_SMM_HAS_CRITICAL_EVENT,
    ASM_FAILOVER_CANNOT_BECAUSE_OTHER_SMM_INIT,
    ASM_FAILOVER_CANNOT_BECAUSE_ASM_RESTRAIN,               // 倒换抑制
    ASM_FAILOVER_CANNOT_BECAUSE_BOARD_IN_NOT_OK,            // 本板未插稳
    ASM_FAILOVER_CANNOT_BECAUSE_SWITCH_FILE_NOT_READY,      // 交换备份未完成
    ASM_FAILOVER_CANNOT_BECAUSE_BOARD_NOT_ACTIVE,           // 10 非主板不允许执行降备
    ASM_FAILOVER_CANNOT_BECAUSE_STANDBY_FLASH_NOT_ENOUGH,   // 对板flash空间不足
    ASM_FAILOVER_CANNOT_BECAUSE_STANDBY_HAS_CRITICAL_ALARM, // 对板有致命告警
    ASM_FAILOVER_CANNOT_BECAUSE_LOCAL_HEATBEAT_ABNORMAL,       // cpld 心跳异常

    ASM_FAILOVER_BUTT = 0x20,                               // 在xml注册的主备倒换抑制项从这里开始
    ASM_FAILOVER_CANNOT_BECAUSE_OTHER_REASON
}ASM_FAILOVER_FAIL_CAUSE;

typedef struct tg_IPMIEthMsg {
    guint8 lun;
    guint8 netfun;
    guint8 slave_addr;
    guint8 cmd;
    guint8 src_len;
    guint8 *data;
} IPMIMSG_BLADE_T;

typedef struct {
    OBJ_HANDLE handle;
    guint8 slave_addr;
}IPMIMSG_BLADE_COMMON;
#pragma pack(1)

typedef struct _tag_alarm_event_msg
{
    guint32 event_code; //新事件码
    guint8 segment_id;
    guint8 group_id;
    guint8 subject_id;
    guint32 mutex_code;
    guint16 event_sequence;
    guint32 event_timestamp;
    guint8 event_severity;
    guint8 event_type;
    guint8 trigger_type;
    guint8 cascading;
    guint32 data_length;
    guint8 *data;
    gchar subject_name[MAX_EVENT_SUBJECT_LEN]; // location + subject_name + add_info
    guint8 sensor_type;//新增字段放最后，保证主备前段数据结构定义兼容
} ALARM_EVENT_MSG;
#pragma pack()

gint32 get_board_active_state(guint8 *as_status);
gboolean is_smm_active(void);
gboolean is_smm_standby(void);
gint32 get_active_board_floatip(gchar *ipaddr, guint32 ipaddr_size);
void format_string_as_upper_head(gchar *buffer, gsize str_len);
gint32 get_other_board_present(guint32 *present_status);
extern gint32 ipmc_get_board_id(guint16 *board_id);

/* 属性同步接口 */
gint32 property_sync_is_property_value_changed(OBJ_HANDLE obj_handle, GSList* caller_info, GSList* input_list, GSList** output_list);
gint32 property_sync_common_handler_ext(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list, DF_OPTIONS options);
gint32 property_sync_common_handler(OBJ_HANDLE obj_handle, GSList* caller_info, GSList* input_list, GSList** output_list);
gint32 property_sync_hw_common_handler(OBJ_HANDLE obj_handle, GSList *caller_info,
    GSList *input_list, GSList **output_list);
gint32 property_sync_common_nosave_handler(OBJ_HANDLE obj_handle, GSList* caller_info, GSList* input_list, GSList** output_list);
gint32 property_sync_save_temporary_handler(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 check_sync_dest_communication_status(guint8 slot_id, guint8 *state);
gint32 get_remote_sync_srv_ip_port(guint8 slot_id, gchar* ip_addr, size_t ip_addr_len, guint16 *port);
gint32 get_current_smm_slotID(guint8* slot_id);
gint32 get_other_smm_slotID(guint8* slot_id);
gint32 get_other_smm_slave_addr(guint8 *slave_addr);
gint32 get_board_master_addr(gchar* blade_name, guint8 *ipmb_addr);
gint32 smm_is_dft_backplane_type(void);
gint32 get_ipmi_target_type_instance(guint8 slave_addr,guint8 *type,guint8 *instance);
/* 精准告警事件解析 */
void alarm_event_msg_format(gchar *src, gchar **dst);
/* 精准告警事件解析 */
extern gint32 get_blade_slot_bit_map(gchar *blade_name, guint32 *slot_bit_map);
gint32 smm_get_maximum_blade_power_capping_value(OBJ_HANDLE current_handle, guint16* remaind_value);
gint32 get_board_presence(guint8 ipmb_addr, guint8 *presence);
gint32 get_internal_ip_segment(guint32 *ip);
gint32 get_board_internal_ip(guint8 ipmb_addr, gchar *ip_str, gsize ip_str_lenth);
gint32 get_board_slave_addr(gchar *blade_name, guint8 *ipmb_addr);
gint32 get_smm_index(guint8 *index);
gboolean is_compute_node(const OBJ_HANDLE object_handle);
gboolean is_current_node(guint8 slot);
gboolean is_swi_module(const OBJ_HANDLE object_handle);
extern gint32 foreach_smm_health_event(gint32 (*func)(GSList *, gpointer), gpointer data);
extern gint32 smm_get_health_events_count(guint32 *minor_events, guint32 *major_events, guint32 *critical_events);
extern gint32 count_single_smm_health_event(GSList *event_msg, gpointer data);
extern gint32 foreach_blade_health_event(gint32 (*func)(const ALARM_EVENT_MSG *msg, const guint8 *msg_data, gpointer data), guint8 ipmb_addr, gpointer data);
extern gint32 count_single_blade_health_event(const ALARM_EVENT_MSG *msg, const guint8 *msg_data, gpointer data);
extern guchar get_pem_num(gchar *location);
extern guchar get_fantray_num(gchar *location);
extern guchar get_blade_num(const gchar *location);
extern gint32 call_other_smm_method(const gchar *object_name, const gchar *method_name, GSList *user_data_list, GSList *input_list, GSList **output_list);
extern guchar get_blade_ipmbaddr(guchar num);
extern guchar get_blade_slotid(guchar slave_addr);
extern gint32 get_smm_blade_presence(OBJ_HANDLE object_handle, guint8* presence);
extern gint32 get_other_smm_property(const gchar* object_name, GSList* input_list, GSList** output_list);
extern gint32 get_current_smm_slave_addr(guint8* slave_addr);
extern gint32 fill_ipmi_target_info(guint8 ipmb_addr, IPMI_REQ_MSG_HEAD_S *header);
extern gint32 get_power_on_controll_timeout(gint32* timeout);
extern gint32 get_compute_node_count(gint32 *count);
extern gint32 fill_remote_method_call_user_data(GSList** user_data_list);
extern gchar ipmi_get_cooling_medium(guint8 slot_id, gint32 *medium);
extern guint8 get_swi_id_from_model_str(const gchar *swi_model);
extern gint32 get_user_name_pwd(OBJ_HANDLE obj_handle, gchar *user_name, gsize name_buffer_size, gchar *user_pw, gsize pw_buffer_size);
extern gint32 is_available_user(OBJ_HANDLE handle,guint8 is_other_smm);
extern gint32 socket_transfer_file(guint8 is_call_othersmm, const gchar* src_path, const gchar* dest_path);
guint8 get_ctrls_present_num(void);
guint8 get_board_slot_id(void);
guint8 get_board_max_slot_id(void);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __ASM_PUBLIC_H__ */
