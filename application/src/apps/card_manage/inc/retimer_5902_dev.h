

#ifndef __RETIMER_5902_DEV_H__
#define __RETIMER_5902_DEV_H__

#include "retimer_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

#define RETIMER_5902_TYPE_L 0
#define RETIMER_5902_TYPE_H 1
#define RETIMER_5902_CHIP_L 16
#define RETIMER_5902_CHIP_H 20

#define DMP_MSG_HEAD_LENGTH 12
#define SMBUS_I2C_LUN_VALUE 0x80
#define SMBUS_I2C_ARG_VALUE 0


#define SMBUS_READ_ERROR_CODE_OFFSET 0
#define DMP_MSG_HEAD_LEN_OFFSET 8
#define SMBUS_READ_PRBS_ERROR_CNT_OFFSET 16

#define I2C_SMBUS_READ 1
#define I2C_SMBUS_WRITE 0

#define I2C_SMBUS_BLOCK_MAX 255 
#define I2C_SMBUS_BLOCK_ADD_NUM 2

#define RETIMER_CHN_WITCH_OPEN       1
#define RETIMER_CHN_WITCH_CLOSE      0

#define RETIMER_INIT_SUCCESS     1
#define RETIMER_INIT_FAILED      0

#define SET_TX_FFE_DATA_START    2
#define SET_RX_CTLE_DATA_START   2
#define SET_RX_CTLE_CHAR_OFFSET  8
#define SET_RX_CTLE_SHORT_OFFSET 16

#define SET_REPEATE_DELAY_TIME 1000
#define RT_IDLE_STATUS 0x10 

#define RETIMER_INIT_FILE_PATH "/opt/pme/conf/cdr/riscv_l1fw_cdr.bin"
#define CDR5902_SINGLE_DATA_MAX_LEN 240
#define CDR5902_REQ_LUN             0x80
#define RETIMER_REQ_LEN_ADD         (sizeof(RETIMER_REQ_DATA_HEAD_S) + 1)
#define RETIMER_REQ_LEN             sizeof(RETIMER_REQ_DATA_HEAD_S)

enum SMBUS_CMD {
    GET_HEALTH_OPCODE = 0x1,
    GET_ERR_CODE_OPCODE = 0x2,
    GET_TEMPERATURE_OPCODE = 0x3,
    GET_FW_VERSION_OPCODE = 0x5,
    QUERY_LOG_OPCODE = 0xC,
    UPDATE_FW_OPCODE = 0x18,
    UPDATE_CTRL_OPCODE = 0x19,
    UPDATE_QUERY_OPCODE = 0x1A,
    SET_HOST_TIME_OPCODE = 0x21,
    SET_REG_OPCODE = 0x2A,
    GET_REG_OPCODE = 0x2B,
    FLASH_CHECK_OPCODE = 0x900,
    GET_HEART_OPCODE = 0x901,
    SOFT_RESET_OPCODE = 0x902,
    
    GET_TX_FFE_OPCODE = 0xA01,
    SET_TX_FFE_OPCODE = 0xA02,
    GET_RX_CTLE_OPCODE = 0xA03,
    SET_RX_CTLE_OPCODE = 0xA04,
    SWITCH_SPEED_OPCODE = 0xA05,
    SERDES_ADAPT_OPCODE = 0xA06,
    PRBS_TX_RX_OPCODE = 0xA08,
    CHECK_PRBS_ERR_OPCODE = 0xA09,
    DFX_OPCODE = 0xA0A,
    GET_EYE_DIAG_OPCODE = 0xA0B,
    POWER_OPT_OPCODE = 0xA0E,
    SET_TX_SWING_OPCODE = 0xA10,
};

#define SPEED_2PLL_MODE     0
#define SPEED_FAST_PI_MODE  1


#define SEND_FIRMWARE_FRAME_HEAD        5
#define SEND_LAST_FIRMWARE_FRAME_HEAD   6

union i2c_smbus_data {
    guint8 byte;
    guint16 word;
    guint8 block[I2C_SMBUS_BLOCK_MAX + I2C_SMBUS_BLOCK_ADD_NUM]; 
                                                                 
};

#define RETRY_REQ_TIMES               3
#define RETIMER_DELAY_BETWEEN_COLLECT 10000
#define RETIMER_DELAY_WAIT_PG         3000

#define RETIMER_CORE_NUM    2
#define RETIMER_PORT_NUM    2

#define RETIMER_PCIE_LINK_UP   1
#define RETIMER_PCIE_LINK_DOWN 0

#define RETIMER_LOG_WRITE_INTERVAL_MAX 86400 

#define RETIMER_LOG_PATH_BASE       "/data/var/log/Retimer/"
#define RETIMER_LOG_PATH_RAM        "/RAMLog/"
#define RETIMER_LOG_PATH_PCIE       "/PCIe/"
#define RETIMER_LOG_PATH_SERDES     "/SerDes/"
#define RETIMER_LOG_PATH_BASE_ALL   "/data/var/log/Retimer/*"

#define RETIMER_LOG_PATH_MAX_LEN 256


#define RETIMER_LOG_NUM 6

#define RETIMER_PORT_OK  0
#define RETIMER_PORT_ERR 1

#define RETIMER_ACCESS_FAIL (-2)
#define RETIMER_IS_SHARE_MODULE 1
typedef struct tagLinkStatus {
    guint8 link_status;
    guint8 link_speed;
    guint8 link_width;
} LinkStatus;

typedef struct tagLinkStatusTick {
    guint32 link_status_tick;
    guint32 link_speed_tick;
    guint32 link_width_tick;
} LinkStatusTick;

typedef struct tagRetimerPCIeStatus {
    LinkStatus link_status[RETIMER_CORE_NUM][RETIMER_CORE_NUM];
    LinkStatusTick link_status_tick[RETIMER_CORE_NUM][RETIMER_CORE_NUM];
    gint32 initial_flg; 
    
    gchar *ram_log_name[RETIMER_LOG_NUM + 1];
    gchar *pcie_log_name[RETIMER_LOG_NUM + 1];
    gchar *serdes_log_name[RETIMER_LOG_NUM + 1];
} RetimerPCIeStatus;

typedef struct tagRetimerRespDataHead {
    guint16 error_code;
    guint16 opcode;
    guint32 total_len;
    guint32 length;
    guint8 data[0];
} __attribute__((packed)) RetimerRespDataHead;

guint8 read_retimer_routine_open_status(void);
void write_retimer_routine_open_status(guint8 num);

gint32 retimer_smbus_access(OBJ_HANDLE obj_handle, guint8 read_write, guint8 reg, gulong size,
    union i2c_smbus_data *data);
gint32 retimer_smbus_write_block(OBJ_HANDLE obj_handle, guint8 reg, gulong len, const guint8 *values);
gint32 retimer_smbus_read_block(OBJ_HANDLE obj_handle, guint8 reg, gulong len, guint8 *values);
gint32 retimer_get_info_by_i2c(OBJ_HANDLE obj_handle, SMBUS_REQ *req, gint32 req_data_len, gint8 *data_info,
    gint32 buf_len);
gint32 retimer_get_info(OBJ_HANDLE obj_handle, SMBUS_REQ *req, gint32 req_data_len, gint8 *data_info, gint32 buf_len);

gint32 retimer_get_busy_info(OBJ_HANDLE obj_handle, guint8 reg, guint8 *values);
gint32 retimer_waite_for_idle(OBJ_HANDLE obj_handle);
gint32 retimer_get_waite_flag_5902(OBJ_HANDLE obj_handle, gint8 *data_info, gint32 buf_len);

gint32 retimer_set_reg_info_5902(OBJ_HANDLE obj_handle, guint32 regAddr, guint32 data_info);
gint32 retimer_get_reg_info_5902(OBJ_HANDLE obj_handle, guint32 regAddr, gint8 *data_info, gint32 buf_len);
gint32 retimer_get_tx_ffe_5902(OBJ_HANDLE obj_handle, guint8 channel_id, gint8 *data_info, gint32 buf_len);
gint32 retimer_set_tx_ffe_5902(OBJ_HANDLE obj_handle, SET_TX_FFE_DEBUG *tx_ffe, gint8 *data_info, gint32 buf_len);
gint32 retimer_get_rx_ctle_5902(OBJ_HANDLE obj_handle, guint8 channel_id, gint8 *data_info, gint32 buf_len);
gint32 retimer_set_rx_ctle_5902(OBJ_HANDLE obj_handle, SET_RX_CTLE_DEBUG *rx_ctle, gint8 *data_info, gint32 buf_len);
gint32 retimer_get_eye_diag_5902(OBJ_HANDLE obj_handle, guint8 channel_id, gint8 *data_info, gint32 buf_len);
gint32 retimer_request_serdes_main_log(OBJ_HANDLE retimer);

gint32 retimer_switch_channel(OBJ_HANDLE obj_handle, guint32 flag);


gint32 retimer_get_temperature_5902(OBJ_HANDLE obj_handle, gint16 *temp_data);
gint32 retimer_cdr_init(OBJ_HANDLE obj_handle);
gint32 retimer_get_heartbeat_5902(OBJ_HANDLE obj_handle);
gint32 retimer_set_speed(OBJ_HANDLE obj_handle, guint8 channel_id, guint8 mode, guint8 speed);
gint32 retimer_serdes_adapt(OBJ_HANDLE obj_handle, guint8 channel_id, guint8 lane_num);
gint32 retimer_prbs_send_recv_5902(OBJ_HANDLE obj_handle, PRBS_SEND_RECV *prbs);
gint32 retimer_get_prbs_error_5902(OBJ_HANDLE obj_handle, PRBS_ERROR_CHECK *prbs, gint8 *data_info, gint32 buf_len);
gint32 retimer_set_tx_swing_5902(OBJ_HANDLE obj_handle, SET_TX_SWING *tx_swing);
gint32 retimer_get_version(OBJ_HANDLE obj_handle, gint8 *data_info, gint32 buf_len);
gint32 retimer_check_lane_loss(OBJ_HANDLE obj_handle);
gint32 retimer_set_host_time(OBJ_HANDLE obj_handle);
gint32 retimer_set_prbs_register(OBJ_HANDLE obj_handle, guint8 channel_id, guint8 lane_num);
gint32 retimer_prbs_optical_module_pwr(OBJ_HANDLE obj_handle);
void retimer_stop_adapt(void);
gint32 retimer_get_and_clear_chip_health(OBJ_HANDLE obj_handle);

void check_get_smm_retimer_logs(OBJ_HANDLE obj_handle);
gint32 retimer_get_and_store_log(OBJ_HANDLE obj_handle, const gchar *filename);
gint32 retimer_get_ram_log(OBJ_HANDLE obj_handle, RetimerPCIeStatus *retimer_pcie_status);
gint32 retimer_get_log_name(gchar **name_arr, gint32 arr_size, gchar *name_buff, gint32 buff_len);
gboolean retimer_is_need_scan_task(OBJ_HANDLE obj_handle);
gboolean check_retimer_powerstate_is_ok(OBJ_HANDLE obj_handle);
void retimer_disable_lane(OBJ_HANDLE obj_handle);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 

#endif 
