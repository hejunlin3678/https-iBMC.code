
#ifndef __RETIMER_MANAGE_H__
#define __RETIMER_MANAGE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

#define SMBUS_RESP_HEAD 13
#define RETRY_CNT 100

#define RSP_MAX_BUF_LEN             256
#define REQ_MAX_BUF_LEN             256

#define RETRY_REQ_TIMES          3
#define RETIMER_DELAY               (2*1000)
#define RETIMER_DELAY_BY_POWER_ON   (30*1000)
#define RETIMER_UPGRADING           1

#define DIEID_START_ADDR            0x28000B00
#define DIEID_TOTAL_LENGTH          64
#define DIEID_SINGLE_LENGTH         12
#define DIEID_TOTAL_NUM             16

#define DMP_CMD_NPU                 0x98
#define FW_VER_MAX_LEN              128
#define DMP_OP_CODE                 0x06
#define CDR_CMD_LEN                 12

#define GET_ARM_CDR_CMD             0x17
#define DMP_GET_CDR_FW_VERSION_TEMPRATURE 0x30
#define DMP_LAST_REQ                0x80
#define DMP_MAX_LENGTH              20
#define RESQ_IPMI_SIZE                  148

#define DMP_REQ_HEAD_INDEX0 0
#define DMP_REQ_HEAD_INDEX1 1
#define DMP_REQ_HEAD_INDEX2 2

#define CDR_IPMB_INDEX_INVALID  0xff
#define RESPONSE_DATA_INDEX0    12
#define HIGH_8BIT_OFFSET        8
#define RETRY_LOG_RETRY_TIMES   10

#define RETIMER_ADAPT_SUCCEED       1
#define RETIMER_ADAPT_FAILED        0

#define retimer_err_log_limit(slot, ret, format, arg...)             \
    do {                                                             \
        static gint32 pre_ret[RETIMER_DEVICE_NUM + 1] = {0};         \
        if ((slot) >= 0 && (slot) < RETIMER_DEVICE_NUM + 1) {        \
            if (ret != pre_ret[slot] && ret != RET_OK) {             \
                debug_log(DLOG_ERROR, format, ##arg);                \
            }                                                        \
            pre_ret[slot] = ret;                                     \
        }                                                            \
    } while (0)

#pragma pack(1)
typedef struct tagRETIMER_RESP_DATA_S {
    guchar byte_cnt;
    guint16 err_code;
    guint16 opcode;
    guint32 total_len;
    guint32 len;
    guint32 data[DIEID_TOTAL_NUM + 1];
} RETIMER_RESP_DATA_S;

typedef struct dmp_req_head {
    guint8 hw_head[3];   // 公司规范IPMI头
    guint8 cmd;          // 子命令字
    guint8 lun;          // 具体含义参见PCIE带外管理规范
    guint8 para;         // opcode的补充参数
    guint8 op_cmd;       // 操作命令字
    guint8 op_fun;       // 操作功能码
    guint32 offset;      // 请求数据偏移
    guint32 data_length; // 请求数据长度
    guint8 data;
} DMP_REQ;

#pragma pack()

// 单帧的长度最大是240字节
#define MAX_SMBUS_LOG_LEN 240

/*lint -e1501 */
#pragma pack(1)
typedef struct tagRetimerReqDataHead {
    guchar lun;
    guchar arg;
    guint16 opcode;
    guint32 offset;
    guint32 length;
    guchar data[0];
} RetimerReqDataHead;
#pragma pack()
/*lint +e1501 */

typedef enum tag_OPTICAL_TYPE_E {
    OPT_25G = 0,
    AOC_25G,
    OPT_40G,
    DAC_100G,
    OPT_100G,
    AOC_100G,
    OPT_BUTT = 0xff,
} OPTICAL_TYPE_E;

typedef enum tag_CDR_CHANNEL_SPEED_E {
    CHANNEL_SPEED_10G = 0,
    CHANNEL_SPEED_25G,
    CHANNEL_SPEED_DS280 = 0x50,
    CHANNEL_SPEED_BUTT = 0xff,
} CDR_CHANNEL_SPEED_E;

typedef struct tag_CDR_TX_FFE {
    gint16 pre2;
    gint16 pre1;
    gint16 main0;
    gint16 post1;
    gint16 post2;
} CDR_TX_FFE;

typedef struct tag_OPT_CDR_ADAPT {
    gchar opt_idenntify[3];
    gchar opt_connect_type[3];
    guint16 opt_trans_length;
    OPTICAL_TYPE_E opt_type;
    CDR_CHANNEL_SPEED_E cdr_speed;
    CDR_TX_FFE tx_ffe;
} OPT_CDR_ADAPT;

#define PART_NUMBER_LEN       32
#define PART_NUMBER_HUAWEI    "03023XDN"
#define PART_NUMBER_LIXIN     "L99QF075-SD-R"

gint32 retimer_notify_upging(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 retimer_channel_switch(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 retimer_set_upg_status(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 retimer_set_npu_cdr_temp(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 retimer_set_npu_cdr_ver(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 retimer_method_sync_info(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 retimer_set_upg_result(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 ipmi_retimer_get_upgrade_err_slot(const void *msg_data, gpointer user_data);
gint32 retimer_manage_start(void);
void notify_retimer_power_state(guint8 power_state);
void notify_retimer_sys_reset_state(guint8 sys_reset_state);
gint32 retimer_get_logs_dump(void);
gint32 retimer_add_object_callback(OBJ_HANDLE obj_handle);
gint32 retimer_del_object_callback(OBJ_HANDLE obj_handle);
gint32 retimer_channel_switch_by_handle(OBJ_HANDLE object_handle, guchar channel_switch);
gint32 retimer_smbus_read_data(OBJ_HANDLE retimer, RETIMER_REQ_DATA_HEAD_S *req, guchar *resp, guint32 resp_len);
guint8 retimer_get_all_status_ready(void);
void retimer_release_i2c(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif 
