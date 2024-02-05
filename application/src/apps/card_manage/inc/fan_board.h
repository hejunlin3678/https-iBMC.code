

#ifndef __FAN_BOARD__
#define __FAN_BOARD__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 




extern gint32 fan_board_init(void);
extern gint32 fan_board_reload_info(void);
extern gint32 fan_board_start(void);
extern gint32 fan_board_add_object_callback(OBJ_HANDLE object_handle);
extern gint32 fan_board_del_object_callback(OBJ_HANDLE object_handle);
extern gint32 fan_board_dump_info(const gchar *path);
void fan_board_updata_card_info(void);
extern void fan_mcu_dump_log(const gchar *path);
gint32 method_sync_fanboard_info(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);










#define FAN_BOARD_DUMPINFO_MAX_LEN 255
#define INTELLIGENT_FAN_LOG_LEN 2048

#define ENABLE_FANBOARD_LOG_MASK (0x1 << 0)  
#define ENABLE_FANBOARD_TIME_MASK (0x1 << 1) 

enum {
    FAN_STATE = 0x00,
    FAN1_SPEED = 0x01,
    FAN2_SPEED = 0x02,
    FAN3_SPEED = 0x03,
    FAN4_SPEED = 0x04,
    FAN5_SPEED = 0x05,
    FAN_PCB_ID = 0x06,
    FAN_VERSION = 0x07,
    FAN_REAL_PWM_RATE = 0x0A,
    FAN_SET_PWM_RATE = 0x0B,
    FAN_STATE2 = 0x0C,
    FAN_TEMP = 0x0D,
    FAN_TEMP_ALARM_H = 0x0E,
    FAN_TEMP_ALARM_L = 0x0F,
    FAN_TYPE = 0x10,
    
    FAN_STATE_WR = 0x20,
    FAN1_SPEED_WR = 0x21,
    FAN2_SPEED_WR = 0x22,
    FAN3_SPEED_WR = 0x23,
    FAN4_SPEED_WR = 0x24,
    FAN5_SPEED_WR = 0x25,
    FAN_PCB_ID_WR = 0x26,
    FAN_VERSION_WR = 0x27,
    FAN_REAL_PWM_RATE_WR = 0x2A,
    FAN_SET_PWM_RATE_WR = 0x2B,
    FAN_STATE2_WR = 0x2C,
    FAN_TEMP_WR = 0x2D,
    FAN_TEMP_ALARM_H_WR = 0x2E,
    FAN_TEMP_ALARM_L_WR = 0x2F,
    FAN_TYPE_WR = 0x30,
    FAN_LED_WR = 0x31,
    
    FAN_TIME_WR = 0x32,
    FAN_ELABEL_WR = 0x34,
    FAN_LOG_WR = 0x35,
};

typedef struct fan_board_sd5000_time_s {
    guint8 year;  
    guint8 month; 
    guint8 day;   
    guint8 hour;  
    guint8 min;   
    guint8 sec;   
} FAN_BOARD_SD5000_TIME;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 

#endif
