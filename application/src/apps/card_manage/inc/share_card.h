

#ifndef __SHARE_CARD_H__
#define __SHARE_CARD_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 


#define SFP_DATA_LEN 24

#define CARD_BYTE_INFO   1
#define WORK_MODE_LEN    4
#define REG_NOSET_FLAG    0

#define OP_CPLD_REG_RETRY_TIMES    3
#define QOS_INFO_RESERVE_LEN    4
#define OPEN_INSERT_OK_WP       0x00
#define CLOSE_INSERT_OK_WP      0xff
#define PANGEA_CARD_POWER_ON_STATE     0x03
#define PANGEA_CARD_STATE              0x01
#define PANGEA_CARD_HOT_PLUG     0  
#define PANGEA_CARD_NOT_HOT_PLUG 1  

#define INVALID_SFP_TEMP        (-128)
#define CONNECTOR_TYPE_LEN       64
#define INVALID_SLOT_ID          255
#define PANGEA_CARD_ABSENT       0
#define PANGEA_CARD_PRESENT      1
#define PANGEA_CARD_PG_OK        1
#define PANGEA_CARD_PST_CHANGE   0
#define PANGEA_CARD_ELABLE_FRAME 8
#define PANGEA_CARD_ELABLE_FRAME_LEN 128
#define GET_ELABEL_DATA_SIZE_PER_FRAME 228
#define SET_ELABEL_DATA_SIZE_PER_FRAME 220  
#define PANGEA_CARD_MAX_ELABLE_LEN 1024
#define PANGEA_CARD_STOP_THREAD   0
#define PANGEA_CARD_START_THREAD  1
#define HW_ACCESS_FAILED          0x8000
#define CARD_TYPE_SENOR_NAME      "CardType"
#define INVALIDE_CARD_TYPE        0xffff
#define CARD_TYPE_CRC_INVALIDE    0xfffe
#define PANGEA_CARD_CHIP_READY    1
#define PANGEA_CARD_CHIP_NOT_READY 0
#define MAX_ELABEL_FRAME_NUM      5
#define MAX_CPLD_I2C_TIME_OUT_VAL 0xff
#define MAX_MCU_LOG_FILE_SIZE     (100 * 1024)
#define MAX_LEN_PER_LOG           128
#define CARD_HW_DESC_LEN          100
#define CARD_MCU_LOG              "mcu_log"
#define CARD_MCU_LOG_DIR          "/data/var/log/pciecard"
#define IOCARD_IIC_READ_BIT      1
#define IOCARD_IIC_PEC_LEN       1
#define IOCARD_IIC_READ_HEADLEN  3
#define IOCARD_IIC_WRITE_HEADLEN 2


typedef enum tag_card_state {
    CARD_STATE_PLUG_OUT = 0, 
    CARD_STATE_ASK_ACTIVE,   
    CARD_STATE_ACTIVED,      
    CARD_STATE_INACTIVING    
} CARD_STATE_E;
typedef struct tag_pangea_card_state_info {
    guint8 slot_id;
    guint8 cur_state;
    guint8 pre_state;
    gboolean is_power_on;
    CARD_STATE_E load_state;
    OBJ_HANDLE mcu_obj;
    OBJ_HANDLE mcu_conn_obj;
    OBJ_HANDLE card_conn_obj;
    OBJ_HANDLE card_action_obj;
} PANGEA_CARD_STATE_INFO;

typedef struct tag_pangea_card_mcu_ver {
    guint8 hw_ver; 
    guint8 pcbver; 
    guint8 softver[32]; 
    guint8 builddata[12]; 
    guint8 buildtime[8]; 
} PANGEA_CARD_MCU_VER;

#pragma pack(push, 1)
typedef struct tag_share_card_1882_temp_rsp {
    gint16 err_code;     
    gint16 cmd_code;     
    gint32 total_length; 
    gint32 length;       
    gint16 data;         
    guint32 crc32;       
} PANGEA_CARD_1822_TEMP_RSP;

typedef struct tag_share_card_sfp_temp_rsp {
    gint16 err_code;           
    gint16 cmd_code;           
    gint32 total_length;       
    gint32 length;             
    gint16 data[SFP_DATA_LEN]; 
    guint32 crc32;             
} PANGEA_CARD_SFP_TEMP_RSP;

typedef struct tag_pangea_card_ipmi_msg_head {
    guint8 slot_id;
    guint8 fru_type0;
    guint8 fru_type1;
    guint8 deviced_id;
    guint8 sdr_type;
    guint16 total_num;
    guint16 cur_num;
} PANGEA_CARD_IPMIMSG_ELABELHEAD;
typedef struct set_card_led_req {
    guint8 chip_addr;
    guint8 cmd;
    guint8 data[4]; 
} SET_CARD_LED_REQ_S;

typedef struct tag_log_file_info {
    gchar log_dir[64];        
    gchar log_file_path[256]; 
} LOG_FILE_INFO;

typedef struct tag_sys_time {
    gushort year;
    guchar month;   
    guchar day;     
    guchar hours;   
    guchar minutes; 
    guchar second;  
} SYS_TIME;

typedef struct tag_share_card_type {
    guint8 slot_id;
    guint8 fru_type0;
    guint8 fru_type1;
    guint8 device_id;
    guint8 sdr_type;
} CARD_TYPE_IPMIMSG_INFO;

typedef struct tag_card_type_rsp {
    guint8 completion_code;
    guint8 card_type[4]; 
} CARD_TYPE_IPMIMSG_RSP;

typedef struct tag_set_card_type {
    guint8 slot_id;
    guint8 fru_type0;
    guint8 fru_type1;
    guint8 device_id;
    guint8 sdr_type;
    guint8 fru_type_id;
    guint8 board_id;
} SET_CARD_TYPE_IPMIMSG_S;
#pragma pack(pop)

typedef struct tagQOS_INFO_S {
    guint16 mode;  
    guint16 speed; 
    guint8 reserve[QOS_INFO_RESERVE_LEN];
} QOS_INFO_S;

#define IOCARD_MCU_VER_LEN sizeof(PANGEA_CARD_MCU_VER)
#define MCU_VER_HEAD_LENGTH 2

typedef enum tag_1822_CORE_CHIP_CMD {
    CORE_CHIP_CHIP_CLOCK_CMD = 0x02, 
    CORE_CHIP_CHIP_TEMP_CMD = 0x03,  
    CORE_CHIP_FPGA_TEMP_CMD = 0x04,  
    CORE_CHIP_SFP_TEMP_CMD = 0x10,   
    CORE_CHIP_PHY_TEMP_CMD = 0x15,   
    CORE_CHIP_INVALID_CMD = 0xFF
} CARD_CHIP_CMD_E;

typedef enum tagCARD_DESC_FLAG {
    CARD_DESC_FLAG_INIT       = 0x00,
    CARD_DESC_FLAG_VALID      = 0x55,
    CARD_DESC_FLAG_FAILED     = 0x56,
    CARD_DESC_FLAG_NOTSUPPORT = 0x57
} CARD_DESC_FLAG_E;


gint32 pangea_card_action_add_object_callback(OBJ_HANDLE obj_handle);
gint32 pangea_card_action_del_object_callback(OBJ_HANDLE obj_handle);
gint32 pangea_card_add_object_callback(OBJ_HANDLE obj_handle);
gint32 pangea_card_del_object_callback(OBJ_HANDLE obj_handle);
gint32 init_pangea_card_action(OBJ_HANDLE obj_handle, gpointer user_data);
gint32 pangea_card_get_mcu_ver(OBJ_HANDLE obj_handle);
gint32 pangea_card_power_on(PANGEA_CARD_STATE_INFO *card_state);
gint32 pangea_card_get_connector_list(gchar *conn_type, GSList **conn_list);
void pangea_card_update_byte_info(const gchar *property_name, guint8 operate_cmd);
gint32 pangea_card_update_version(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 pangea_card_set_alarm_led(OBJ_HANDLE card_obj, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 pangea_card_get_alarm_led(OBJ_HANDLE card_obj, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 set_card_time_out_val(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 reset_cpld_core(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 pangea_card_get_elable(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 method_read_data_from_mcu(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 pangea_card_reset_mcu(guint8 slot_id, guint8 status);
gint32 pangea_card_ipmi_reset_mcu(gconstpointer msg_data, gpointer user_data);
gint32 pangea_card_update_temp(OBJ_HANDLE obj_handle);
gint32 reset_card_chip(GSList *input_list);
gint32 get_card_powergood(GSList *input_list);
gint32 set_card_hilink(GSList *input_list);
gint32 set_card_type(GSList *input_list);
gint32 set_card_workmode(GSList *input_list);
gint32 ipmi_cmd_set_pangea_card_elabel(gconstpointer msg_data, gpointer user_data);
gint32 ipmi_cmd_get_pangea_card_elabel(gconstpointer msg_data, gpointer user_data);
gint32 ipmi_cmd_get_pangea_card_type(gconstpointer msg_data, gpointer user_data);
gint32 ipmi_cmd_set_pangea_card_type(gconstpointer msg_data, gpointer user_data);
gint32 pangea_card_start(void);
gint32 pangea_card_init(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 

#endif 
