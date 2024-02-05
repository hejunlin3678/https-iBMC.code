

#ifndef __RETIMER_DS280_DEV_H__
#define __RETIMER_DS280_DEV_H__

#include "retimer_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

#define CDR_MICRO3_I2C_TX_DEV_ADDR              0x18
#define CDR_MICRO2_I2C_TX_DEV_ADDR              0x1b
#define CDR_MICRO3_I2C_RX_DEV_ADDR              0x24
#define CDR_MICRO2_I2C_RX_DEV_ADDR              0x27

#define CDR_I2C_DATA_LEN                        0x01
#define CDR_I2C_OFFSET_BYTE_LEN                 0x01
#define CDR_I2C_INFO_LEN                        0x100

#define CDR_SINGLE_CHANNEL_SET                  0x01
#define CDR_MULTI_CHANNEL_SET                   0x03

#define CDR_CHANNEL_CONFIG_ID_OFFSET            0xEF
#define CDR_VERSION_ID_OFFSET                   0xF0
#define CDR_DEVICE_ID_OFFSET                    0xF1
#define CDR_CHANNEL_SHARE_VERSION_OFFSET        0xF3
#define CDR_VENDOR_ID_OFFSET                    0xFE
#define CDR_CHANNEL_SET_OFFSET                  0xFC
#define CDR_BROADCAST_ENABLE_OFFSET             0xFF
#define CDR_CHANNEL_RESET_OFFSET                0x00
#define CDR_RESET_OFFSET                        0x0A
#define CDR_POST_CURSOR_OFFSET                  0x3F
#define CDR_PREV_CURSOR_OFFSET                  0x3E
#define CDR_MAIN_CURSOR_OFFSET                  0x3D
#define CDR_STANDARD_DATA_RATE_OFFSET           0x2F
#define CDR_RX_ADAPT_MODE_OFFSET                0x31
#define CDR_CTLE_OFFSET                         0x2D
#define CDR_CTLE_BOOST_OFFSET                   0x03
#define CDR_DFE_OFFSET                          0x1E
#define CDR_DEFAULT_VENDOR_ID_VALUE             0x03

#define CDR_HEO_VEO_OFFSET                      0x2B
#define CDR_POWER_SET_OFFSET                    0x18
#define CDR_FIR_OFFSET                          0x2E

#define CDR_RESET_SHARED_REG                    0x04
#define CDR_ENABLE_TEMP_DETECTION               0x0F
#define CDR_ENABLE_CHANNEL_TEMP                 0x19
#define CDR_CONFIG_ADC                          0x0C
#define CDR_TEMP_LOW_BYTE                       0x0D
#define CDR_TEMP_HIGH_BYTE                      0x0E
#define CDR_HORIZONTAL_EYE                      0x27
#define CDR_VERTICAL_EYE                        0x28
#define CDR_FORCE_SIGNAL_DETECTION              0x14


#define CDR_RESET_VALUE                         0xC
#define CDR_DERESET_VALUE                       0x0
#define CDR_RESET_MASK                          0xC

#define CDR_READ            0
#define CDR_WRITE           1

#define CDR_TX_CFG          0
#define CDR_RX_CFG          1

#define MAC_ID_MAX          8

#define WIRE_COPP_LEN_1M        1
#define WIRE_COPP_LEN_3M        3
#define WIRE_COPP_LEN_5M        5

#define CDR_RESET_IN_USE_ADDR       0x2680
#define CDR_CFG_INFO_ADDR           0x269C
#define CDR_CFG_INFO_LEN            0x4
#define CDR_NUM_MAX                 0x4
#define CDR_LOGIC_LANE_NUM          8

#define CDR_LOGIC_LANE_MAX          7
#define CDR_MACRO_NUM               2
#define CDR_LANE_NUM                4
#define CDR_DIRECT_NUM              2

#define CDR_CH_MASK                 0x4

enum cdr_work_mode {
    CDR_MODE_2PLL,
    CDR_MODE_FASTPI
};

#define EXT_BOARD_IN_USE_ADDR       0x2688

enum ext_board_type {
    BOARD_NO_CDR,
    BOARD_USE_CDR
};



enum cdr_power_down_type {
    CDR_POWER_DOWN_LANE,
    CDR_POWER_DOWN_MACRO,
    CDR_POWER_DOWN_LANE_TX,
    CDR_POWER_DOWN_LANE_RX,
    CDR_POWER_DOWN_PLL
};

enum cdr_power_opt {
    CDR_POWER_DOWN,
    CDR_POWER_ON
};

typedef enum {
    CDR_1G_RATE  = 0,
    CDR_10G_RATE = 1,
    CDR_25G_RATE = 2
} cdr_rate_e;

typedef struct {
    guint8 cdr_chan;
    guint8 lane_speed;
    gint8 ffe_main;
    gint8 ffe_post1;
    gint8 ffe_pre1;
} CDR_DS280_CFG;

typedef struct {
    guint8 reg;
    guint8 data;
    guint8 mask;
} DS280_SMBUS_WRITE;

gint32 retimer_ds280_config(OBJ_HANDLE obj_handle, CDR_DS280_CFG *cdr_cfg);

gint32 retimer_get_reg_info_ds280(OBJ_HANDLE obj_handle, guint32 reg_addr, gint8 *data_info, gint32 buf_len);
gint32 retimer_get_tx_ffe_ds280(OBJ_HANDLE obj_handle, guint8 channel_id, gint8 *data_info, gint32 buf_len);
gint32 retimer_set_tx_ffe_ds280(OBJ_HANDLE obj_handle, SET_TX_FFE_DEBUG *tx_ffe, gint8 *data_info, gint32 buf_len);
gint32 retimer_get_temperature_ds280(OBJ_HANDLE obj_handle, gint16 *temp_data);
gint32 retimer_get_eye_diag_ds280(OBJ_HANDLE obj_handle, guint8 channel_id, gint8 *data_info, gint32 buf_len);
gint32 retimer_get_prbs_error_ds280(OBJ_HANDLE obj_handle, PRBS_ERROR_CHECK *prbs, gint8 *data_info, gint32 buf_len);
gint32 retimer_prbs_send_recv_ds280(OBJ_HANDLE obj_handle, PRBS_SEND_RECV *prbs);
gint32 retimer_get_heartbeat_ds280(OBJ_HANDLE obj_handle);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 

#endif
