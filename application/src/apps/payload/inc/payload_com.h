
#ifndef __PAYLOAD_COM_H__
#define __PAYLOAD_COM_H__

#define PAYLOAD_QUEUE_PATH "./"

#define PAYLOAD_MODULE_NAME "Payload"

#define PAYLOAD_SUCCESS 0   // 成功
#define PAYLOAD_FAIL 1      // 失败
#define PAYLOAD_EACCES 2    // 权限不足
#define PAYLOAD_ERESOURCE 3 // 系统资源访问错误


#define PAYLOAD_INIT_SUCCESS 0
#define PAYLOAD_INIT_FAIL 1
#define PAYLOAD_TIME_UNIT 10 // uint:100ms

#define GET_BIND_OBJECT_FAIL 99
#define FRU_ID_INVALID 100

// XCU CPLD返回的重启原因
#define RESET_BUTTON 8
#define RESET_WATCHDOG 4
#define RESET_BMC_CMD 2
#define RESET_POWER_UP 1

// IPMI SPEC定义的重启原因
#define PUSH_BUTTON_RESET 2
#define WT_EXPIR_RESET 4
#define SOFT_REST 0xA
#define AUTO_POWER_UP_RESET 6
#define UNKOWN_RESET 0

#define PL_OP_LOG_PWR_ON_T "Set FRU%u to power on successfully\r\n"
#define PL_OP_LOG_PWR_OFF_T "Set FRU%u to normal power off successfully\r\n"
#define PL_OP_LOG_PWR_OFF_T1 "Set FRU%u to forced power off successfully\r\n"
#define PL_OP_LOG_COLD_RST_F "FRU%u forced power off the service system,and then power it on failed\r\n"
#define PL_OP_LOG_HARD_RST_F "FRU%u warm reset failed\r\n"
#define PL_OP_LOG_DIAG_INTR_F "FRU%u issue diagnostic interrupt failed\r\n"
#define PL_OP_LOG_SOFT_OFF_T "Set FRU%u to soft power off successfully\r\n"
#define PL_OP_LOG_REST_POLICY_F "Set power restore policy failed\r\n"
#define PL_OP_LOG_PWR_OFF_TM_F "Set normal power off timeout failed\r\n"
#define PL_OP_LOG_WARM_RST_F PL_OP_LOG_HARD_RST_F
#define PL_OP_LOG_BTN_LOCK_F "Set power button lock failed\r\n"
#define PL_OP_LOG_COLD_RST_T "Set FRU%u to forced power off the service system,and then power it on successfully\r\n"
#define PL_OP_LOG_HARD_RST_T "Set FRU%u to warm reset successfully\r\n"
#define PL_OP_LOG_DIAG_INTR_T "Set FRU%u to issue diagnostic interrupt successfully\r\n"
#define PL_OP_LOG_RST_WD_F "Reset watchdog timer failed\r\n"
#define PL_OP_LOG_SET_PWR_LEL_F "Set power level failed\r\n"
#define PL_OP_LOG_PWR_CTRL_F "Set power ctrl failed\r\n"
#define PL_OP_LOG_PWR_CTRL_T "Set power ctrl to %d successfully\r\n"
#define PL_OP_LOG_FRU_ACTIVE_F "Set fru active failed\r\n"
#define PL_OP_LOG_FRU_ACTIVE_T "Set fru active (active=%d, flag=%d) successfully\r\n"
#define PL_OP_LOG_PWR_LEVEL_F "Set power level failed\r\n"
#define PL_OP_LOG_PWR_LEVEL_T "Set power level to %d successfully\r\n"

#define PL_OP_LOG_RST_F "FRU%u forced system reset failed\r\n"
#define PL_OP_LOG_RST_T "Set FRU%u to forced system reset successfully\r\n"

#define PL_OP_LOG_PC_F "FRU%u system power cycle failed\r\n"
#define PL_OP_LOG_PC_T "Set FRU%u to system power cycle successfully\r\n"

#define PL_OP_LOG_PWR_DELAY_MODE_F "Set power up delay mode failed"
#define PL_OP_LOG_PWR_DELAY_MODE_T "Set power up delay mode(%s) successfully"
#define PL_OP_LOG_PWR_DELAY_COUNT_F "Set power up delay time failed"
#define PL_OP_LOG_PWR_DELAY_COUNT_T "Set power up delay time(%.1f s) successfully"

#define TASK_DELAY_50MS 50 // 50ms延时

extern gint32 payload_init(void);
extern gint32 payload_start(void);

extern gchar *pp_policy_to_string(guchar restore_policy);
extern gint32 pp_judge_poweroff_tm_valid(guint32 pwroff_tm);
extern gint32 pp_judge_manu_id_valid_bios(const guint8 *manufacture_id);
extern gint32 pp_get_server_type(guint8 *server_type);

extern gint32 pp_check_active_option(guchar option, guchar fru_device_id);
extern gint32 pp_check_chassis_ctl_option(guchar option, guchar fru_device_id);
extern gint32 hop_check_fru_ctl_option(guchar option, guchar fru_device_id);
void payload_orig_before_ac_check(void);

#endif 
