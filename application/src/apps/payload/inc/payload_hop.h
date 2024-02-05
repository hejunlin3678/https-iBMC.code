

#ifndef __PAYLOAD_HOP_H__
#define __PAYLOAD_HOP_H__

#define LOOKUP_POWER_ON_RETRY_NUM 100
#define POWER_ON_RETRY_NUM 3
#define SENSOR_SCAN_CYCLE 5000
#define SENSOR_SCAN_CYCLE_V6 8000

#define ERR_CODE_SUCCESS 0
#define ERR_CODE_ABORT 1

#define BIOS_MEASURE_PROGRESS    1
#define BIOS_MEASURE_DONE        0
#define TCM_ENABLE               1
#define TCM_DISABLE              0
#define TCM_DEFAULT              0Xff

#define INSERT_SIGNAL_VALID 0x00
#define __DEBUG__
#ifdef __DEBUG__

#define INDEX_POWER_BUTTON_EVENT 0
#define INDEX_UID_BUTTON_EVENT 1
#endif

#define ACPI_S0_G0_WORKING 0
#define ACPI_S3_SLEEPING 3
#define ACPI_S4_SUSPEND_TO_DISK 4
#define ACPI_S5_G2_SOFT_OFF 5
#define ACPI_S4_S5_SOFT_OFF 6

#define BUT_EVT_TRUE 1
#define BUT_EVT_FALSE 0

#define COUNT_SECOND_ONE_DAY (60 * 60 * 24UL)
#define OUT_OF_RANGE (-2)

#define SW_BIOS_ACTION_HW 0
#define SW_BIOS_ACTION_TIMER 1
#define SW_BIOS_ACTION_WDT 2

#define PUSH_PWR_BTN_WAIT 0
#define PUSH_PWR_BTN_YES 1
#define PUSH_PWR_BTN_NO 2

#define DETECT_TIME 5
#define FRU_DETECT_TIME 100UL
#define FRU_CHECK_INTERVAL_TIME 500UL

#define FRU_HOP_ON_FAILED_BY_BIOS_ERR 2

typedef gint32 (*insert_checker)(guint8, guchar *);

typedef struct tag_PinMateCB {
    guint32 m_cnt;
    guchar m_pin_mated;
    insert_checker m_fp_chker;
} PIN_MATED_CB_T, *PPIN_MATED_CB_T;

typedef gint32 (*get_criteria_checker)(guint8, guchar *);
typedef struct tag_CriteriaCB {
    guint32 m_cnt;
    guchar m_criteria_mated;
    get_criteria_checker m_fp_chker;
} CRITERIA_CB_T, *PCRITERIA_CB_T;

extern guint8 g_push_pwr_btn_flag;
extern guchar hop_get_pwrpg_signal(guchar fru_device_id);
extern guchar hop_get_pwr_signal(guchar fru_device_id);
extern guchar hop_off(guchar fru_device_id);
extern guchar hop_on(guchar fru_device_id);
extern gint32 hop_reset(guchar fru_device_id);
extern guchar hop_force_off(guchar fru_device_id);
extern guchar hop_get_fru_site_num(guchar fruid);
extern guchar hop_is_fru_present(guchar fruid);
extern gint32 hop_check_power_status(guchar fruid);
extern guint32 hop_check_host_reset(guchar fruid);
extern gint32 hop_chassis_hard_reset(void);
extern void hop_get_acpi_status(guchar fru_device_id, guchar *val);
extern void hop_fru_hard_reset(guchar fru_device_id);
extern void hop_check_fru_pin_task(void);
extern void hop_criteria_check_task(void);
extern guchar hop_get_imana_reset_type(void);
extern guchar hop_criteria_checker_init(void);
extern guchar hop_pin_checker_init(void);
extern gint32 hop_trigger_nmi_interrupt(void);
extern gint32 hop_get_power_before_ac_lost(guchar *val);
extern gint32 hop_clear_system_reset_flag(void);
extern gint32 hop_get_system_reset_flag(guchar *val);
extern gint32 hop_set_pwr_button_lock(guint8 button_lock, guint8 persist_mode);
extern gint32 hop_get_pwr_button_event(guchar *val);
extern gint32 hop_clear_pwr_button_event(void);
extern gint32 hop_get_pwr_button_lock_stat(guchar *val);
extern gint32 hop_set_pwr_flag(guchar fru_device_id, const gchar *property_name, guint8 value);
extern void hop_set_pwr_off_abort(guchar value);
extern guchar hop_get_pwr_off_abort(void);
extern gint32 hop_clear_system_reset_src(void);
extern guint32 hop_get_host_start_timeout(void);
extern void hop_set_pwrbtn_on_flag(guchar value);
extern guchar hop_get_pwrbtn_on_flag(void);
extern void hop_set_reset_event_from_pwr(guchar value);
extern guchar hop_get_reset_event_from_pwr(void);
extern guchar hop_fru_warm_reset(guchar fru_device_id, guchar channel, guchar restart_cause);
extern gint32 hop_set_pwr_sel_event(const gchar *obj_name, const gchar *property_name, guint8 value);
extern gint32 hop_get_power_flag(guchar fru_device_id, const gchar *property_name, guchar *val);
extern void hop_clear_power_flag(guchar fru_device_id, const gchar *property_name);
extern gint32 hop_ctrl_bios_switch(guchar fru_device_id, guint8 action);
extern gint32 hop_set_power_before_ac_lost(guchar val);
extern gint32 hop_is_bios_boot_ok(guchar fru_device_id);
extern gint32 hop_clear_bios_boot_status(guchar fru_device_id);
extern void hop_set_listen_sel_flag(guchar fru_device_id, const gchar *property_name, guint8 value);
extern gint32 hop_synch_pwr_button_lock(void);
extern gint32 hop_powercycle(guchar fru_device_id);
extern gint32 hop_diag_interrupt(guchar fru_device_id);
extern void hop_sem4_init(void);
gint32 hop_get_forbid_poweron_flag(guint8 *forbid_poweron_flag);
gint32 hop_get_bbu_present(void);
gboolean is_arm_enable(void);
extern SEMID g_power_operate_sem4;

#endif 
