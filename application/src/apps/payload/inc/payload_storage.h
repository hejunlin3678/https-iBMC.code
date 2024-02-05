
#ifndef APPLICATION_SRC_APPS_PAYLOAD_INC_PAYLOAD_STORAGE_H
#define APPLICATION_SRC_APPS_PAYLOAD_INC_PAYLOAD_STORAGE_H

#include "pme_app/uip/uip_object_interface.h"
#include "payload_hop.h"
#include "wdt2.h"
#include "payload_ipmi.h"
#include "payload_hs.h"
#include "payload_com.h"
#include "payload_pwr.h"


#define DELAY_100_MSEC   100
#define DELAY_200_MSEC   200
#define DELAY_300_MSEC   300
#define DELAY_500_MSEC   500
#define DELAY_1000_MSEC  1000
#define DELAY_1500_MSEC  1500
#define DELAY_2000_MSEC  2000
#define DELAY_3000_MSEC  3000
#define DELAY_2_MINUTES  120000

#define RETRY_3TIMES      3
#define RETRY_19TIMES     19
#define RETRY_20TIMES     20
#define RETRY_30TIMES     30
#define RETRY_50TIMES     50

#define OPEN_DEVICE        1
#define CLOSE_DEVICE       0

#define IMU_POWER_FLAG_DEFAULT  6
#define IMU_POWER_FLAG_FORBID   9  

#define FRU_WORK_ABNORMAL 0
#define FRU_WORK_NORMAL   1

#define NORMAL_PWRDWN   0
#define ABNORMAL_PWRDWN 1

#define DES_POWER_ON 1
#define DES_POWER_NORMAL_OFF 5
#define DES_POWER_ABNORMAL_OFF 4
#define DES_POWER_NOTES_SYS_CLOSE 6
#define DES_POWER_CLEAR 7
#define DES_POWER_NOACTION 0

#define MAX_DEV_ID 7


typedef enum tagCTRLBOARD_ONLINE {
    CTRL_BOARD_0 = 0,
    CTRL_BOARD_1 = 1,
    CTRL_BOARD_ALL = 2,
    CTRL_BOARD_BUTTY = 0xFF
} CTRLBOARD_ONLINE_E;

typedef enum tagPROPER_TYPE {
    PRO_BYTE = 0,
    PRO_INT16,
    PRO_UINT16,
    PRO_INT32,
    PRO_UINT32
} PROPERTYPE_E;

#define BYTE0 0
#define BYTE1 1
#define BYTE2 2
#define BYTE3 3
#define BYTE4 4
#define BYTE5 5
#define BYTE0   0
#define BYTE1   1
#define BYTE2   2
#define BYTE3   3
#define BYTE4   4
#define BYTE5   5

#define BMC_MASK_3  0x03
#define BMC_MASK_F  0x0F
#define BMC_MASK_1C 0x1C
#define BMC_MASK_D0 0xD0

#define SYS_POWER_OFF (0xa0)
#define SYS_POWER_ON  (0xa1)

#define EQUIPTESTMODE_OPEN  1
#define EQUIPTESTMODE_CLOSE 0
#define EQUIPTE_CLOCK_OPEN  7
#define EQUIPTE_CLOCK_CLOSE 6

#define ANALOG_ACDOWN_INTERRUPT 0x2

#define INVALID_VALUE8 0xFF
#define INVALID_VALUE8  0xFF


#define BOOT_FLAG_NOBOOT (0x0)
#define BOOT_FLAG_BIOS (0x1)
#define BOOT_FLAG_OS (0x2)
#define BOOT_FLAG_LASTKO_OK (0x4)

#define P0 STATE_POWER_OFF
#define P1 STATE_ASK_POWER_ON
#define P2 STATE_POWER_ONING
#define P3 STATE_POWER_ONED
#define P4 STATE_ASK_POWER_OFF
#define P5 STATE_POWER_OFFING

#define MAX_P_STATE 6 // P状态的最大个数

#define HANDLED  1
#define UNHANDLE 0


#define CPLD_RAM_SYS_COLD_REBOOT_ADDR (0xCFF9)  
#define CPLD_RAM_SYS_COLD_REBOOT_DATA_AA (0xaa) 

#define WATCH_DOG_CPLD_DIR  0x26 
#define GET_WATCH_DOG_TIMES   10 
#define DFT_MODE_CPLD_ADDR 0x33A 
#define NOT_DFT_MODE_FLAG    0x0
#define WATCH_DOG_OPEN       0x1 
#define WATCH_DOG_CLOSED     0x0 


#define HAED_RESET_BIOS_TIME_OUT    48     
#define WARM_RESET_BIOS_TIME_OUT    12     
#define BIOS_START_OK               1
#define SYS_DISK_RESET_MAX_TIME     5      
#define RESET_WAIT_TIME             5000   
#define DFT_SYSTEM_DISK_RESET_TEST  1      
#define DFT_WARM_RESET_MAX_TIMES    30     
#define DFT_HARD_RESET_MAX_TIMES    5      
#define CPLD_RAM_BIOS_START_ADDR    0xD62F 
#define CPLD_RAM_BIOS_START_DATA_55 0x55
#define SYS_DISK_POW_STATE_OFF      0x0        
#define SYS_DISK_POW_STATE_ON       0x1        
#define DFT_WARM_RESET_REASON       0xa7       
#define DFT_HARD_RESET_REASON       0x04       
#define DFT_BIOS_WARM_RESET         0x0f       
#define DFT_OS_RESET                93         

typedef struct tag_OS_RESET_PARA_S {
    guint8 reset_type;        
    guint8 reset_system_disk; 
    guint8 reset_times;       
    guint8 para_num;          
} OS_RESET_PARA_S, *POS_RESET_PARA_S;

typedef enum tagDFT_OSRESET_TEST_TYPR {
    DFT_WARM_RESET = 0, 
    DFT_HARD_RESET,     
} DFT_OSRESET_TEST_TYPR_E;

typedef enum tagSWITCH_LOADLINE_TYPR_E {
    CLOSE_LOADLINE = 0,
    OPEN_LOADLINE
} SWITCH_LOADLINE_TYPR_E;

typedef enum tagFORCE_POWEROFF_TYPE {
    SIM_ACDOWN = 0,
    WRITE_POWEROFF_REGISTER
} FORCE_POWEROFF_TYPE;

typedef enum tagSTARTDOG_STATE {
    DOG_NEED_CLOSE = 0, 
    DOG_NEED_OPEN,      
    DOG_NONEED_OPERATE  
} STARTDOG_STATE_E;

#define BMC_OPEN_DEVICE (0x00)
#define BMC_FORCE_PWROFF_DEVICE (0x02)
#define BMC_NORMAL_PWROFF_DEVICE (0x03)

#define IPMI_OEM_NETFN 0x32

#define MAX_PLANE_NUM (2)
#define MAX_MACHINE_STATE (6)

#define NUMBER_0  0
#define NUMBER_1  1
#define NUMBER_2  2
#define NUMBER_3  3
#define NUMBER_10 10

#define LIMTT_TIME (300000)
#define LIMTT_TIME_01_HOUR (3600000)

#define BBU_ID_0 (0x00)

#define READY_OK  (1)
#define READY_NOK (0)
#define NUMBER_0   0

#define LED_OPEN  (1)
#define LED_CLOSE (0)

#define FLAGTURE (1)
#define FLAGFALSE (0)

#define CTRL_STATES_STANDBY (0x1)
#define CTRL_STATES_POWEROK (0x0)


#define BIT_VAL_1 (1)
#define BIT_VAL_0 (0)

#define LONGBUTTONEVENT    (0xAA) 
#define LONGBUTTONEVENTCLR (0x00) 

#define BIT0 (0)
#define BIT1 (1)
#define BIT2 (2)
#define BIT3 (3)
#define BIT4 (4)
#define BIT5 5

#define LENGTH_32 (32)

#define BBU_ID_0   (0x00)
#define BBU_ID_1   (0x01)
#define BBU_ID_2   (0x02)
#define BBU_ID_3   (0x03)
#define BBU_ID_ALL (0x04)


#define SLOT_A (0)
#define SLOT_B (1)
#define SLOT_INVALID (0xFF)


typedef enum tagButton_HANDLE_CAUSE {
    CTRL_BUTTON_UP = 1,    // 正常上电直接短按钮脉冲上电，cpld会开PS_ON
    CTRL_BUTTON_OFF,       // 通知逻辑报中断给BIOS走备电，存储节点直接写短按钮
    CTRL_BUTTON_FORCE_OFF, // 强制下电：大西洋北冰洋要走备电；北冰洋计算节点不需要备电，直接写短按钮；存储节点下电自上而下的，通知BMC下电时已经保内存。
    CTRL_BUTTON_BIOS,      // 备电完成，BIOS通知BMC关机，BMC直接写短按钮，cpld直接关PS_ON
    CTRL_BUTTON_BUTT
} CTRL_BUTTON_HANDLE_E;

typedef enum {
    BOARD_SLAVE = 0x6,
    BOARD_MASTER = 0x9,
    BOARD_MOS_ERROR
} BOARD_MOS_E;

typedef enum {
    OPEN_BBU,
    CLOSE_BBU
} BBU_OPERATION;

#define CREATE_TASK 0
#define DELETE_TASK 1

#define BOARDCTRL_CUR_RESET   1
#define BOARDCTRL_LAST_RESET  2
#define BOARDCTRL_LLAST_RESET 3

#define BOARDCTRL_RESETRESRECORD_CYCLE 5000
#define HEART_BEAT_CNT_ONEDAY 17280

typedef struct tagBOARDCTRL_RESET_INFO_S {
    guint8 reset_reason_cur;   
    guint8 reset_reason_last;  
    guint8 reset_reason_llast; 
    guint8 reset_reason_assit; 
    guint8 reset_reason_cnt;   
} BOARDCTRL_RESET_INFO_S;

typedef struct tagBOARDCTRL_RESET_REASON_S {
    guint32 reset_value;
    const char *reset_desc;
} BOARDCTRL_RESET_REASON_S;

typedef enum tagDAL_SYS_POWER_CONTROL_E {
    DAL_BOARD_NORMAL_DOWN,   
    DAL_BOARD_ABNORMAL_DOWN, 
    DAL_SYS_NORMAL_DOWN,     
    DAL_SYS_ABNORMAL_DOWN,   
    DAL_SYS_UP,              
    DAL_JBOD_DOWN,           
    
    DAL_NOTIFY_POWER_DOWN_MODE, 
    DAL_IP_ENCL_NRC_TEMP_DOWN, 
    DAL_SYS_POWER_CONTROL_BUTT
} DAL_SYS_POWER_CONTROL_E;

typedef struct tag_power_ctrl_ipmi {
    guint8 fru_type0;
    guint8 fru_type1;
    guint8 fru_id;
    guint8 device_id;
    guint8 sdr_type;
    guint8 power_ctrl_mode;
} POWER_CTRL_MODE_IPMI;

gint32 is_pw_on_condit_satised(void);
gboolean is_cooling_satised(void);
gboolean is_support_storage_payload(void);
gint32 check_first_poweron_cond(void);
guint8 get_offing_state_flag(void);
void set_offing_state_flag(guint8 flag);
void ctrl_power_button_handle(CTRL_BUTTON_HANDLE_E button_event);
void sim_acdown_for_power_cycle(void);
gint32 get_storpayload_property_byte(const char* property_name, guint8* out);
gint32 set_storpayload_property_byte(const char* property_name, guint8* value_addr, DF_OPTIONS options);
gint32 ctrl_bbu_running_state(gint32 bbu_id, guint8 operation);
void pp_printf_abnormal_pwrstate(void);
gint32 hop_get_max_ps_num(gint32 *max_ps_num);
void pwr_on_task_command(guint8 cmd);
gint32 handle_bios_off_cmd(void);
void prepare_before_poweron(void);
void prepare_after_poweroff(void);
void pp_event_check_init(void);
void pp_operate_shining_led(guint8 operate);
gint32 pp_set_storage_node_power(guint8 operation);
void boardctrl_resetreason_record_init(void);
gint32 debug_storage_power_info(GSList *input_list);
void boardctrl_recordsys_coldreboot(void);
void config_os_watch_dog(void);
gint32 register_os_reset_dft_item(OBJ_HANDLE handle, guint32 dft_id);
void pp_high_temperature_check_init(void);
gint32 pp_set_imuforbid_pwron_flag(guint8 flag);
gint32 pp_get_imuforbid_pwron_flag(guint8 *val);
gint32 pp_bios_pwroff_handler(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 pp_method_get_pwroff_flag(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 handle_sys_power_off_ipmi(gconstpointer pdata, gpointer user_data);
void bbu_running_state_operate(guint8 operate);
void payload_switch_loadline(guint8 action);
#endif