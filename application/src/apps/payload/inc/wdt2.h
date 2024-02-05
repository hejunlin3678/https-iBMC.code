

#ifndef __WATCHDOG2_H__
#define __WATCHDOG2_H__



#define WATCHDOG_PRE_TIMEOUT_NONE 0
#define WATCHDOG_PRE_TIMEOUT_SMI 1
#define WATCHDOG_PRE_TIMEOUT_NMI 2
#define WATCHDOG_PRE_TIMEOUT_MSG 3

#define WATCHDOG_TIMEOUT_NOACTION 0
#define WATCHDOG_TIMEOUT_RESET 1
#define WATCHDOG_TIMEOUT_PWROFF 2
#define WATCHDOG_TIMEOUT_PWRCYCLE 3

#define WATCHDOG_TIMER_BIOSFRB2 1
#define WATCHDOG_TIMER_BIOSPOST 2
#define WATCHDOG_TIMER_OSLOAD 3
#define WATCHDOG_TIMER_SMSOS 4
#define WATCHDOG_TIMER_OEM 5

#define TIMEOUT_COUNT_CLEAR (0)
#define TIMEOUT_COUNT_INCREASE (1)


#define COMP_CODE_WATCHDOG_NOT_INIT 0x80

#pragma pack(1)


typedef struct tag_SetWatchdogTimer_S {
#ifdef BD_BIG_ENDIAN
    guchar not_log : 1;
    guchar not_stop : 1;
    guchar wk1 : 3;
    guchar timer_use : 3;

    guchar wk2 : 1;
    guchar pre_tmout_int : 3;
    guchar wk3 : 1;
    guchar tmout_action : 3;

    guchar pre_tmout_interval;

    guchar wk4 : 2;
    guchar timer_expir_flag_clear_oem : 1;
    guchar timer_expir_flag_clear_sms_os : 1;
    guchar timer_expir_flag_clear_osload : 1;
    guchar timer_expir_flag_clear_bios_post : 1;
    guchar timer_expir_flag_clear_bios_frb2 : 1;
    guchar wk5 : 1;
#else
    guchar timer_use : 3;
    guchar wk1 : 3;
    guchar not_stop : 1;
    guchar not_log : 1;

    guchar tmout_action : 3;
    guchar wk3 : 1;
    guchar pre_tmout_int : 3;
    guchar wk2 : 1;

    guchar pre_tmout_interval;

    guchar wk5 : 1;
    guchar timer_expir_flag_clear_bios_frb2 : 1;
    guchar timer_expir_flag_clear_bios_post : 1;
    guchar timer_expir_flag_clear_osload : 1;
    guchar timer_expir_flag_clear_sms_os : 1;
    guchar timer_expir_flag_clear_oem : 1;
    guchar wk4 : 2;
#endif

    guchar initial_cnt_l;
    guchar initial_cnt_h;
} IPMIMSG_SET_WATCHDOG_TIMER_TYPE_S, *PIPMIMSG_SET_WATCHDOG_TIMER_TYPE_S;


typedef struct tag_GetWatchdogTimer_S {
    guchar comp_code;

#ifdef BD_BIG_ENDIAN
    guchar not_log : 1;
    guchar not_stop : 1;
    guchar wk1 : 3;
    guchar timer_use : 3;

    guchar wk2 : 1;
    guchar pre_tmout_int : 3;
    guchar wk3 : 1;
    guchar tmout_action : 3;
#else
    guchar timer_use : 3;
    guchar wk1 : 3;
    guchar not_stop : 1;
    guchar not_log : 1;

    guchar tmout_action : 3;
    guchar wk3 : 1;
    guchar pre_tmout_int : 3;
    guchar wk2 : 1;
#endif

    guchar pre_tmout_interval;

#ifdef BD_BIG_ENDIAN
    guchar wk4 : 2;
    guchar timer_expir_flag_clear_oem : 1;
    guchar timer_expir_flag_clear_sms_os : 1;
    guchar timer_expir_flag_clear_osload : 1;
    guchar timer_expir_flag_clear_bios_post : 1;
    guchar timer_expir_flag_clear_bios_frb2 : 1;
    guchar wk5 : 1;
#else
    guchar wk5 : 1;
    guchar timer_expir_flag_clear_bios_frb2 : 1;
    guchar timer_expir_flag_clear_bios_post : 1;
    guchar timer_expir_flag_clear_osload : 1;
    guchar timer_expir_flag_clear_sms_os : 1;
    guchar timer_expir_flag_clear_oem : 1;
    guchar wk4 : 2;
#endif

    guchar initial_cnt_l;
    guchar initial_cnt_h;

    guchar present_cnt_l;
    guchar present_cnt_h;
} IPMIMSG_GET_WATCHDOG_TIMER_TYPE_RESP_S, *PIPMIMSG_GET_WATCHDOG_TIMER_TYPE_RESP_S;

#pragma pack()


#pragma pack(1)
typedef struct tag_WdtWdiConfig_S {
    // 设置复位预中断定时器Set Pre-Interrupt-Reset Timer命令中携带有2个字节的信息(死机原因),供HOST查询
    guint8 sp1;
    guint8 sp2;

    // Timer Use (TU) 定时器用途
    guint8 tu : 3;
    // Pre-timeout Interrupt Mode (PIM) 预中断方式，当PIM为0或者保留值时，接口逻辑不发出中断信号，
    // 当PIM为SMI、NMI、IRQ类型时，通过SERIRQ发出相应的中断消息
    guint8 pim : 3;
    // Timeout Action (TA) 超时后动作
    guint8 ta : 3;
    // Watchdog Pre-Timeout (WPTO) 看门狗预中断计时溢出：当看门狗定时器到达预中断时刻，此位置1，将根据配置向HOST发出中断
    guint8 wpto : 1;
} WDI_CONFIG, *PWDI_CONFIG;
#pragma pack()

#define WDI_IOC_MAGIC 'w'
#define WDI_CMD_IRQ _IOWR(WDI_IOC_MAGIC, 0, WDI_CONFIG)

#define WDI_OPT_INTERFACE "/dev/wdi"

#define WATCH_DOG2 "Watchdog2"
#define WDT2_ALIAS 0x00000001L

#define WATCHDOG_TIMER_BIOSFRB2_STR "BIOS FRB2"
#define WATCHDOG_TIMER_BIOSPOST_STR "BIOS/POST"
#define WATCHDOG_TIMER_OSLOAD_STR "OS Load"
#define WATCHDOG_TIMER_SMSOS_STR "SMS/OS"
#define WATCHDOG_TIMER_OEM_STR "OEM"


typedef struct tag_Watchdog2_S {
    volatile guchar not_log : 1; // 是否记录SEL， 1: 不记录
    volatile guchar running : 1; // 1 : 看门狗正在运行, 0 : 看门狗已经停止
    volatile guchar reserved : 3;
    volatile guchar timer_use : 3; // 看门狗用途 not_log=FALSE, vaild

    volatile guchar pre_tmout_int_f : 1;  // 预超时已经到了 标志 1: 已预超时
    volatile guchar pre_tmout_int : 3;    // 预超时中断类型
    volatile guchar pre_tmout_int_en : 1; // 预超时中断使能/禁止 1: 使能
    volatile guchar tmout_action : 3;     // 超时行为类型0-no action, 1-reset, 2-pwr off, 3-pwr cycle

    volatile guchar pre_tmout_interval; // '1'-base 超时之前多久算预超时 ，单位 : 秒

    volatile guchar wk4 : 2;
    volatile guchar timer_expir_flag_oem : 1;       // 超时标志 oem
    volatile guchar timer_expir_flag_sms_os : 1;    // 超时标志 sms/os
    volatile guchar timer_expir_flag_osload : 1;    // 超时标志 osload
    volatile guchar timer_expir_flag_bios_post : 1; // 超时标志 post
    volatile guchar timer_expir_flag_bios_frb2 : 1; // 超时标志 frb2
    volatile guchar wk5 : 1;

    volatile guint16 initial_countdown; // initial cntval 100ms/cnt
    volatile guint16 current_countdown; // current cnt
} WATCHDOG2_S, *PWATCHDOG2_S;


#define TIME_STR_FMT "%Y-%m-%d %T"

#define TIME_STR_LEN 19
#define TIME_STR_BUFF_LEN (TIME_STR_LEN + 1)

typedef struct {
    guint32 total_cnt;
    time_t timestamp;
} WATCHDOG2_KICKING_INFO_S;

gint32 wdt_hs_evt_process(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name, GVariant *property_value);
void wdt_task_init(void);
void wdt_stop(void);
gint32 wdt_set_timer_method(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
void wdt_obj_init(void);
void wdt_restore(void);
void wdt_kicking_info_init(void);
gboolean is_watchdog_running(void);
void wdt_create_timer(void);
WATCHDOG2_KICKING_INFO_S get_wdt_kicking_info(void);
gulong get_watchdog_tick(void);
WATCHDOG2_S get_watchdog_info(void);
void wdt_restart(void);
guchar wdt_set_timer(const IPMIMSG_SET_WATCHDOG_TIMER_TYPE_S *pSet);
const gchar *wdt_timer_use_to_str(guchar timer_use);
const gchar *wdt_timeout_action_to_str(guchar timeout_action);
gint32 set_ipc_check_value(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);
gint32 set_ipc_debug_flag(GSList *input_list);
#endif 
