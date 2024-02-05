

#ifndef __RETIMER_COMMON_H__
#define __RETIMER_COMMON_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

#define RETIMER_MSG_DATA_LEN 243
#define SMBUS_READ_DATA_OFFSET 12

#define RETIMER_MSG_REPEATE_NUM 3
#define RETIMER_DEVICE_NUM 8
#define REQUEST_OFFSET 0x0
#define RETIMER_LANE_NUM 8
#define RETIMER_CHANNEL_NUM 16
#define PRBS_ON_OFF_REG_NUM 2

#define BUFFER_MAX_LEN 256


enum ROUTINE_STATUS {
    RETIMER_OPEN_ROUTINE           = 0x0,
    RETIMER_START_CLOSE_ROUTINE    = 0x1,
    RETIMER_ALREADY_CLOSE_ROUTINE  = 0x2,
};

typedef struct tag_SMBUS_REQ {
    guint8 lun;
    guint8 arg;
    guint16 opcode;
    guint32 offset;
    guint32 length;
    guint8 data[RETIMER_MSG_DATA_LEN];
} SMBUS_REQ;

typedef struct {
    guint8 channel_id;
    guint8 lane_num; 
    gint16 pre2;
    gint16 pre1;
    guint16 main0;
    gint16 post1;
    gint16 post2;
} SET_TX_FFE_DEBUG;

typedef struct {
    guint8 channel_id;
    guint8 lane_num; 
    guint16 enable;
    guint32 gain;
    guint32 boost;
    guint16 squelch;
    guint16 zero;
} SET_RX_CTLE_DEBUG;

typedef struct {
    guint8 channel_id;
    guint8 lane_num;
    guint8 prbs_type;
    guint8 prbs_dir;
} PRBS_SEND_RECV;

typedef struct {
    guint8 channel_id;
    guint8 lane_num;
} PRBS_ERROR_CHECK;

typedef struct {
    guint8 channel_id;
    guint8 lane_num;
    guint8 swing;
} SET_TX_SWING;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 

#endif
