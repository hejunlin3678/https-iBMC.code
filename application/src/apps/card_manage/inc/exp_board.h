

#ifndef __EXP_BOARD_H__
#define __EXP_BOARD_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 



#define SAS_EXPBOARD_LOCALLED_TYPE 0x04
#define SAS_EXPBOARD_ALARMLED_TYPE 0x02
#define SAS_EXPBOARD_POWER_REG_ADDR 0x90
#define SAS_EXPBOARD_PRESENT_REG_ADDR 0x91

#define SAS_EXPBOARD_MAX_NUM 8 
#define ARCTIC_SAS_EXPBOARD_V2_BOARDID 19 
#define PACIFIC_SAS_EXPBOARD_V2_BOARDID 7 
#define PAC1220V6_SAS_EXPBOARD_V2_BOARDID 10 

#define SAS_EXPBOARD_CORE_CHIP_ADDR 0x7a         
#define SAS_EXPBOARD_CORE_CHIP_QUERY_CMD 0xc2    
#define SAS_EXPBOARD_CORE_CHIP_DATA_LEN 0x03     
#define SAS_EXPBOARD_CORE_CHIP_CMD_DEFAULT 0x00  
#define SAS_EXPBOARD_CORE_CHIP_CMD_TAG 0x00      
#define SAS_EXPBOARD_CORE_CHIP_POWER_ON 0x01     


typedef struct tag_sas_expboard_op_s {
    guint8 huawei_iana_id[3]; // 3个字节表示厂商信息 0xdb 0x07 0x00
    guint8 sub_cmd;           // 0x5a
    guint8 op_type;           // 0x08电源 ; 0x09灯
    guint8 fru_type_id[2];    // 2个字节的部件类型定义， 低字节在前，参见“部件类型定义”
    guint8 reserved;          // 保留位，默认为0x00
    guint8 slot_id;           // 槽位号
    guint8 sub_op_type;       // 操作的类型ID
    guint8 state;             // 目标状态
} SAS_EXPBOARD_OP_S;

typedef enum tagCTRL_CPLD_ARCARD_POWER_E {
    CTRL_ARCARD_POWER_OFF = 0,     // 下电
    CTRL_ARCARD_POWER_ON = 1,      // 上电
    CTRL_ARCARD_RESET = 2,         // 强制复位
    CTRL_ARCARD_FACTORY_RESET = 3, // 恢复出厂设置
    CTRL_ARCARD_INVALID_ACTION = 4 // 无效操作
} CTRL_CPLD_ARCARD_POWER_E;

typedef enum tagCTRL_EXP_CHIP_POWER_E {
    EXP_CHIP_POWER_OFF = 0,             
    EXP_CHIP_POWER_ON = 1,              
    EXP_CHIP_RESET = 2,                 
    EXP_CHIP_INVALID_ACTION = 0xff      
} CTRL_EXP_CHIP_POWER_E;

typedef enum tagREAD_EXP_CHIP_CMD_E {
    READ_TEMP_CMD = 0x60,     
    READ_STATUS_CMD = 0xf0    
} CTRL_EXP_CHIP_CMD_E;

typedef enum tagREAD_EXP_CHIP_LEN_E {
    READ_STATUS_LEN = 1,     
    READ_TEMP_LEN = 2,       
    READ_DATA_COM_LEN = 5    
} SAS_EXPBOARD_CHIP_DATA_LEN_E;

typedef struct tag_sas_chip_op_s {
    guint8 addr;        
    guint8 cmd;         
    guint8 cnt;         
    guint8 subCmdLow;   
    guint8 subCmdHigh;  
    guint8 cmdTag;      
    guint8 pec;         
} SAS_EXPBOARD_CHIP_OP_S;


extern gint32 exp_board_init(void);
extern gint32 exp_board_start(void);
extern gint32 exp_board_add_object_callback(OBJ_HANDLE object_handle);
extern gint32 exp_board_del_object_callback(OBJ_HANDLE object_handle);
extern gint32 update_pcie_function_resource(json_object *resource_jso);
gint32 hdd_board_method_sync_property(const OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 method_sas_expboard_property_sync(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);


gint32 ipmi_cmd_set_sas_expboard_led(const void *msg_data, gpointer user_data);
gint32 ipmi_cmd_set_sas_expboard_powerstate(const void *msg_data, gpointer user_data);


gint32 debug_get_exp_board_led(GSList *input_list);
gint32 set_sas_expboard_pcie_fault(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 method_arcard_control(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 sas_exp_chip_power_ctrl(const guint16 device_num, const guint8 cmd_code, guint8 *comp_code);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif 
