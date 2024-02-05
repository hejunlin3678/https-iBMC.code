
#ifndef __PAYLOAD_PWR_H__
#define __PAYLOAD_PWR_H__

#define TIMEOUT_3M 180 // 单位s
#define CYCLE_INTERVAL_14S 14
#define CYCLE_INTERVAL_6S 6
#define TIMEOUT_ENABLE 0x01
#define TIMEOUT_DISABLE 0x00

// 加上超时后长按powerbutton的6秒钟+扩展2s
#define POWER_OFF_DEFAULT_TIMEOUT 8UL
#define POWER_OFF_ENDLESS_TIMEOUT 0xffffffffUL

#define MAX_CHASSIS_ID 0x00

// 控制单板上下电动作定义(外界发起的控制动作)
#define PAYLOAD_CTRL_POWERNOACTION 0
#define PAYLOAD_CTRL_POWERON 1
#define PAYLOAD_CTRL_POWEROFF 2
#define PAYLOAD_CTRL_FINISHED 3

// 单板上下电控制的状态定义(当前正在被控制上电、下电等)
#define PAYLOAD_CTRL_STATE_NOACTION 0
#define PAYLOAD_CTRL_STATE_ONING 1
#define PAYLOAD_CTRL_STATE_OFFING 2
#define PAYLOAD_CTRL_STATE_WAITINGON 3
#define PAYLOAD_CTRL_STATE_WAITINGOFF 4
#define PAYLOAD_CTRL_STATE_ONINGWAITINGOFF 5
#define PAYLOAD_CTRL_STATE_OFFINGWAITINGON 6

// 单板发生异常掉电
#define PAYLOAD_AC_LOST 1

// 正在进行控制，没有进入稳定状态
#define PAYLOAD_POWER_STATE_IN_CTRL 2

// 上下电控制
#define POWER_ON 1
#define POWER_OFF 0

#define POWER_CYCLE 2
#define POWER_OFF_FORCE 3


#define POWER_MODE_DEFAULT 0
#define POWER_MODE_SINGLE 1


#define RESTART_NONE 0

#define RESTART_PROCESSING 1

#define RESTART_DONE 2

// 电源策略

#define PAYLOAD_ALWAYS_POWER_UP 2

#define PAYLOAD_POWER_RESTORED 1

#define PAYLOAD_ALWAYS_POWER_OFF 0

#define PAYLOAD_ALWAYS_POWER_UP_RO 0xF2

#define POWERSTATE_BEFORE_ACLOST_VALUE_SIZE 1
#define POWER_RESTORE_POLICY_VALUE_SIZE 1

#define CYCLE_STATE_OFF 0
#define CYCLE_STATE_REBOOT 1

#define SATISFY 1
#define UN_SATISFY 0

#define POWER_OFF_DELAY_TIMES 10
#define POWER_ON_DELAY_TIMES 20

#define MAX_POWER_DRAW_NUM 20
#define PP_PME_VER_DELIM "."

// 下电超时使能
#define POWER_OFF_TIMEOUT_ENABLE 2
#define POWER_OFF_TIMEOUT_DISABLE 3

#define FRU_RUNNING_NORMALLY 0  // FRU正常运行中
#define FRU_RUNNING_NOT_READY 1 // FRU启动运行过程中

#define PWR_BUTTON_DISABLE_POWER_ON 0
#define PWR_BUTTON_ENABLE_POWER_ON 1
#define PWR_BUTTON_EN_PWR_ON_OR_OFF 2

#define ACLOST_PWRSTAT "ACLostPwrStat"
#define RESTORE_POLICY "RestorePolicy"
#define RESTART_CAUSE "RestartCause"
#define PWR_CYCLE_CB "PwrCycleCB"
#define POWER_OFF_LOCK "PowerOffLock"

#define POWER_ON_CRITERIA_LOG_INTERVAL 30
#define NO_LOG_HAS_BEEN_RECORD 0
#define LOG_HAS_BEEN_RECORD 1

#define RH8100_SINGLE_SYS 1
#define RH8100_MASTER_BMC 1

#define DFT_FAN_DEFAULT_LEVEL_VAL_INVALID 0xFF // 50% speed
#define DFT_FAN_WAIT_FOR_LOOPBACK 10           // 3s noloop 3-5s loop 8s


typedef enum {
    
    POWER_ON_FIRMWARE_ACTIVE_INDEX = 0,       // BMC主动下电场景，升级CPLD、VRD需要AC生效
    POWER_ON_STRATEGY_INDEX = 31        // 上电开机策略index，该bit为0代表执行通电开机策略，1代表不执行通电开机策略
} UNIT_POWER_ON_SCENE_IDX_T;

enum {
    PP_BOOT_ORDER_MTD3 = 0,
    PP_BOOT_ORDER_MTD4,
    PP_BOOT_AREA_MAX_CNT
};

enum {
    PP_POWER_OFF_FAIL = 0x01,         // 下电失败
    PP_SHORT_PUSH_POWRE_OFF_T = 0x02, // 已经短按已下电
    PP_LONG_PUSH_POWRE_OFF_T = 0x04,  // 已经长按已下电
    PP_SHORT_PUSH_POWRE_OFF_F = 0x08  // 已经短按未下电
};

typedef struct tag_FruPwrCtrl_S {
    guchar m_pwr_state;  
    guchar m_action;     
    guchar m_ctrl_state; 
} FRU_PWR_CB_S, *PFRU_PWR_CB_S;

typedef struct tag_PwrCtrlEvent_S {
    guchar action;
    guchar power_state;
} PWR_CTRL_EVT_S, *PPWR_CTRL_EVT_S;

typedef struct tag_PwrCycleCB_S {
    guint32 interval;
    guchar cycle_in_progress;
} PWR_CYCLE_CB_S, *PPWR_CYCLE_CB_S;

typedef struct tag_RestartCause_S {
    guchar restart_state;
    guchar restart_action;
    guchar restart_action_chan;
    guchar restart_cause;
    guchar restart_chan;
} RESTART_CAUSE_S, *PRESTART_CAUSE_S;

typedef struct tag_PwrCtrlFault_S {
    guint32 on_fault : 1;
    guint32 off_fault : 1;
    guint32 : 0;
} PWR_CTRL_FAULT_S, *PPWR_CTRL_FAULT_S;

// 与chassis control相关的属性和方法都归属于这个结构
// 此结构为全局性唯一
typedef struct tag_ChassisPayload_S {
    guint32 m_power_off_timeout;
    guint32 m_power_cycle_interval;
    guchar m_is_host_up;
    guchar m_is_pwr_restore_in_progress;
    guchar m_pwr_off_lock;
    guint8 m_pwr_restore_policy;
    guint8 m_pwr_state_before_ac_lost;
    guint8 m_check_insert_en;
    guint8 m_pwr_delay_mode;
    guint32 m_pwr_delay_count;
    guint32 m_power_off_timeout_en;
    TIMERID m_host_check_timer;
    TASKID m_power_cycle_task_id;
    RESTART_CAUSE_S m_restart_cause;
    PWR_CTRL_FAULT_S m_power_ctrl_fault;
    guint8 m_pwr_timeout_count;
} PAYLOAD_S, *P_PAYLOAD_S;

// 与fru control相关的属性和方法都归属于这个结构
// 此结构为全局性唯一，但是成员变量必须为每个fru都分配一个
// 为什么不每个fru一个结构对象呢?这是因为此结构的成员有些需要持久化保存
// 而持久化保存的模式可能不一致，如果每个fru一个结构对象，那么持久化
// 不好处理
typedef struct tag_FruPayload_S {
    GAsyncQueue **m_power_ctrl_evt_qid;
    guint8 *m_pwr_off_lock;
    PFRU_PWR_CB_S m_pwr_ctrl_cb;
    PPWR_CYCLE_CB_S m_pwr_cycle_cb;
} FRU_PAYLOAD_S, *PFRU_PAYLOAD_S;

typedef struct tag_GET_POWER_INFO_S {
    guint8 cur_power_level;
    guint8 delay_to_stable;
    guint8 max_power_level;
    guint8 power_multiplier;
    guint8 power_draw[MAX_POWER_DRAW_NUM];
} GET_POWER_INFO_S;

typedef struct tag_DftFruReset_S {
    gpointer user_data;
    guint8 fruid;
    guint8 timeout;
} DFT_FRU_RESET_S, *PDFT_FRU_RESET_S;

extern GET_POWER_INFO_S g_get_power_info;
extern guchar pp_payload_init(void);
extern guchar pp_get_power_off_normal_flag(void);
extern void pp_set_power_off_normal_flag(guchar value);
extern void pp_set_pwr_state_before_ac_lost(gchar fru_device_id, guint8 pwr_state);
extern guchar pp_fru_pwr_ctrl(guchar fru_device_id, guchar ctrl);
extern guint32 pp_get_pwr_off_timeout(guchar fru_device_id);
extern guint32 pp_get_pwr_off_timeout_en(guchar fru_device_id);
extern void pp_set_pwr_off_timeout(guint32 timeout);
extern void pp_set_pwr_on_delay_param(guint8 delay_mode, guint32 delay_count);
extern void pp_get_pwr_on_delay_param(guint8 *delay_mode, guint32 *delay_count);
extern void pp_set_pwr_off_timeout_en(guint32 timeout);
extern guchar pp_get_pwr_state(guchar fru_device_id);
extern void pp_cycle_task(void);
extern void pp_set_restore_policy(guchar policy);
extern guchar pp_get_restore_policy(void);
extern void pp_do_pwr_restore(guint8 ser_type);
extern guchar pp_is_power_ctrl_failed(void);
extern guchar pp_is_insert_check_en(guchar fru_device_id);
extern void pp_set_power_ctrl_fault(guchar fru_device_id, gint32 action, guchar isFailed);
extern void pp_set_restart_action(guchar channel, guchar value);
extern void pp_set_restart_cause(void);
extern void pp_clear_restart_action(void);
extern void pp_get_restart_cause(guchar *pchannel, guchar *pvalue);
extern void pp_set_restart_state(guchar fruid, guchar val);
extern void pp_set_power_cycle_interval(guchar seconds);
extern guint32 pp_get_power_cycle_interval(void);
extern guchar pp_is_pwr_off_locked(guint8 fru_id);
extern void pp_set_host_down(void);
extern void pp_set_host_up(void);
extern guchar pp_is_host_up(void);
extern guchar pp_is_pwr_restore_in_progress(guchar fru_device_id);
extern void pp_set_pwr_restore_in_progress(guchar fru_device_id, guchar in_progress);
extern guchar pp_is_power_on_criteria_met(guchar fru_device_id);
extern guchar pp_is_pwr_button_test_in_progress(void);
extern void pp_set_pwr_off_lock(guint8 fru_id, guchar lock);
extern void pp_start_host_checker(void);
extern void pp_stop_host_checker(void);
extern void pp_cycle_init(void);
extern guchar pp_init(void);
extern void pp_ctrl_init(void);
extern void pp_start(void);
extern guchar pp_is_cycle_in_progress(guchar fruid);
extern void pp_save_cycle_param(void);
extern void pp_set_cycle_progress_state(guchar fruid, guint32 val);
extern void pp_set_cycle_interval(guchar fruid, guint32 val);
extern void pp_update_reset_flag(guint8 reset_flag);
extern gint32 pp_check_src_from_reset(void);
extern void pp_update_fru_pwr_state(guint8 fru_id);
extern gint32 pp_fru_pwr_on(guchar src_chan_num, guchar fru_device_id, guchar restart_cause);
extern void pp_update_ctrl_fault(void);
extern gint32 pp_dump_cmd(GSList *input_list);
extern gint32 pp_set_host_start(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
extern gint32 pp_method_poweronlock_clear(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 pp_method_pwr_on(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
extern gint32 pp_method_pwr_on_ext(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 pp_method_pwr_off(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
extern gint32 pp_method_force_pwr_off(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 pp_method_pwr_cycle(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
extern gint32 pp_method_pwr_hard_reset(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 pp_method_chassis_diag_interrupt(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 pp_method_soft_off(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
extern gint32 pp_method_set_pwr_restore_policy(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 pp_method_exe_pwr_restore_policy(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 pp_method_set_pwr_cycle_interval(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 pp_method_set_pwr_off_timeout(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 pp_method_set_pwr_off_timeout_en(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 pp_method_power_cycle(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 pp_method_cold_reset(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 pp_method_warm_reset(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 pp_method_fru_diag_interrupt(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 pp_method_graceful_reboot(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 pp_method_fru_control(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 pp_set_pwr_button_lock(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 pp_get_pwr_button_lock(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 pp_method_set_pwr_ctrl(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 pp_method_fru_active(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 pp_method_set_pwr_level(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 pp_method_set_pwrdelay_mode(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 pp_method_set_pwrdelay_count(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 pp_method_set_forbid_poweron_flag(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 pp_method_set_board_powerdrop_flag(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 payload_get_xcu_restart_cause(guchar *pchannel, guchar *pvalue, guchar fru_id);
extern gint32 pp_clear_powergd_status_after_power_on(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);
extern void pp_do_pwr_restore_process(void);
extern gint32 pp_method_dft_reset_os(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

extern guint8 pp_action_according_to_powerlevel(guint8 fru_id, guint8 pow_level, guint8 server_flag);
extern void pp_update_restart_value(void);
extern void pp_deal_with_restart_cause_flag(void);
extern void payload_get_aclost_status(guchar *ACLostStatus);
extern void payload_set_aclost_status(guchar ACLostStatus);
extern void pp_set_power_on_by_ipmi_flag(guchar value);
extern guchar pp_get_power_on_by_ipmi_flag(void);
extern gint32 set_chassis_last_power_event(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);
extern void set_wol_by_saved_value(guint8 wol_state);
extern gint32 pp_apci_status_change_proc(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);
extern gint32 get_vddq_check_forcepwroff_flag(guchar *flag);
extern gint32 pp_method_ac_cycle(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
extern gint32 pwr_restore_action_done_method(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

extern gint32 config_composition_forbid_power_on_status(OBJ_HANDLE object_handle, gpointer user_data,
    gchar *property_name, GVariant *property_value);

gint32 pp_method_clear_restart_value(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 pp_allow_set_power_restore_policy(void);
gint32 method_set_pwr_config(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 pp_method_set_power_on_strategy_exceptions(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
FRU_PAYLOAD_S *pp_relate_fru_payload_info(void);
void pp_task_power(void *param);
void pp_set_vrd_validate_method(const gchar *method_name, guint8 method_value);
gint32 pp_get_remote_upgrade_bios_state(void);
#ifdef DFT_ENABLED
void task_os_fast_power_cycle_test(void);
#endif
void pp_dft_init(void);
void pp_send_evt(guchar fruid, guchar action, guchar power_state);
void pp_on_evt(guchar fruid, PPWR_CTRL_EVT_S power_ctrl_event);
void pp_ctrl_task(guchar fruid);
gint32 pp_set_powerstate_after_bmc_reset(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 pp_method_set_thermal_trip_power_policy(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 pp_method_set_thermal_trip_time_delay(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
#ifdef ARM64_HI1711_ENABLED
gint32 check_bios_measured_result(void);
#endif
#endif 
