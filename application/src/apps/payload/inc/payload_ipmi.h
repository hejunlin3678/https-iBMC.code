
#ifndef __PAYLOAD_IPMI_H__
#define __PAYLOAD_IPMI_H__

// chassis control
#define CHASSIS_POWEROFF 0x0
#define CHASSIS_POWERON 0x1
#define CHASSIS_POWERCYCLE 0x2
#define CHASSIS_HARD_RESET 0x3
#define CHASSIS_DIAG_INT 0x4
#define CHASSIS_SOFT_OFF 0x5

#define GRACEFUL_REBOOT_INTERVAL 15000UL

#define HUAWEI_IANA_LSB 0xDB
#define HUAWEI_IANA_MID 0x07
#define HUAWEI_IANA_MSB 0x00

#define PARAM_VERSION 0x01
#define OEM_PARAM_PWR_OFF_TIMEOUT 0x62
#define OEM_PARAM_PWR_DELAY 0x65

#define POWER_LEVEL_DONT_CHANGE_PRESENT_LEVELS 0
#define POWER_LEVEL_COPY_DESIRED_LEVELS_TO_PRESENT_LEVELS 1
#define POWER_LEVEL_POWER_OFF 0
#define POWER_LEVEL_DONT_CHANGE 0xff

#define POWER_STEADY_STATE_POWERDRAW_LEVELS 0
#define POWER_DESIRED_STEADY_STATE_DRAW_LEVELS 1
#define POWER_EARLY_POWER_DRAW_LEVELS 2
#define POWER_DESIRED_EARLY_LEVELS 3
#define POWER_STEADY_POWER_STATE 4

#define ACTIVATE_DEACTIVATE_FRU 0

typedef struct tag_PICMG_RESP_S {
    guint8 completion;
    guint8 picmg_id;
    guint8 data[MAX_PICMG_RESPONCE_DATA_LEN];
} PICMG_RESP_S, *PPICMG_RESP_S;

typedef struct tag_SET_ACTIVATION_POLICY_REQ_S {
    guint8 picmg_id;
    guint8 fru_id;
    guint8 lock_bit_mask;
    guint8 lock_bit;
} SET_ACTIVATION_POLICY_REQ_S, *PSET_ACTIVATION_POLICY_REQ_S;

typedef struct tag_GET_ACTIVATION_POLICY_REQ_S {
    guint8 picmg_id;
    guint8 fru_id;
} GET_ACTIVATION_POLICY_REQ_S, *PGET_ACTIVATION_POLICY_REQ_S;

typedef struct tag_SET_ACTIVATION_REQ_S {
    guint8 picmg_id;
    guint8 fru_id;
    guint8 activation;
} SET_ACTIVATION_REQ_S, *PSET_ACTIVATION_REQ_S;

typedef struct {
    guchar comp_code;

#ifdef BD_BIG_ENDIAN
    guchar reserved : 1;
    guchar restore_policy : 2;
    guchar control_fault : 1;
    guchar power_fault : 1;
    guchar interlock : 1;
    guchar overload : 1;
    guchar power_state : 1;
#else
    guchar power_state : 1;
    guchar overload : 1;
    guchar interlock : 1;
    guchar power_fault : 1;
    guchar control_fault : 1;
    guchar restore_policy : 2;
    guchar reserved : 1;
#endif
    guchar last_pwr_evt;
    guchar misc_chas_state;
    guchar ftpal_button_cap;
} IPMIMSG_GET_CHASSIS_STATE_RESP_T, *PIPMIMSG_GET_CHASSIS_STATE_RESP_T;

#pragma pack(1)
typedef struct {
    guint8 fru_id;
    guint8 button_type;
    guint8 shield_state;
} SET_PWRBUTTON_SHIELD_DATA_S;

typedef struct {
    guint8 fru_id;
    guint8 button_type;
} GET_PWRBUTTON_SHIELD_DATA_S;

typedef struct {
    guint8 manu_id[MANUFAC_ID_LEN];
    guint8 sub_cmd;
    guint16 selector;
    guint16 param_len;
    SET_PWRBUTTON_SHIELD_DATA_S param;
} IPMI_SET_PWRBUTTON_SHIELD;

typedef struct {
    guint8 manu_id[MANUFAC_ID_LEN];
    guint8 sub_cmd;
    guint16 selector;
    guint16 param_len;
    GET_PWRBUTTON_SHIELD_DATA_S param;
} IPMI_GET_PWRBUTTON_SHIELD;
#pragma pack()

extern gint32 pp_ipmi_cmd_set_fru_activation_policy(const void *pdata, gpointer user_data);
extern gint32 pp_ipmi_cmd_get_fru_activation_policy(const void *pdata, gpointer user_data);
extern gint32 pp_ipmi_cmd_set_fru_activation(const void *pdata, gpointer user_data);
extern gint32 pp_ipmi_cmd_set_pwr_restore_policy(const void *pdata, gpointer user_data);
extern gint32 pp_ipmi_cmd_set_pwr_cycle_interval(const void *pdata, gpointer user_data);
extern gint32 pp_ipmi_cmd_get_sys_restart_cause(const void *pdata, gpointer user_data);
extern gint32 pp_ipmi_oem_get_sys_restart_cause(const void *pdata, gpointer user_data);
extern gint32 pp_ipmi_cmd_chassis_ctrl(const void *pdata, gpointer user_data);
extern gint32 pp_ipmi_cmd_fru_ctrl(const void *pdata, gpointer user_data);
extern gint32 pp_ipmi_cmd_set_power_off_timeout(const void *req_msg, gpointer user_data);
extern gint32 pp_ipmi_cmd_set_watch_dog_status(const void *req_msg, gpointer user_data);
gchar *pp_delay_mode_to_string(guint8 delay_mode);
extern gint32 pp_ipmi_cmd_set_power_on_delay(const void *req_msg, gpointer user_data);
extern gint32 pp_ipmi_cmd_get_power_off_timeout(const void *req_msg, gpointer user_data);
extern gint32 pp_ipmi_cmd_get_power_on_delay(const void *req_msg, gpointer user_data);
extern guchar pp_chassis_nmi_interrupt(void);
extern guchar pp_chassis_hard_reset(guchar src_chan_num, guchar restart_cause);
extern guchar pp_chassis_pwr_cycle(guchar src_chan_num, guchar restart_cause);
extern gint32 pp_chassis_pwr_on(guchar src_chan_num, guchar restart_cause);
extern gint32 pp_set_power_level(const void *req_msg, gpointer user_data);
extern gint32 pp_get_power_level(const void *req_msg, gpointer user_data);
extern guchar pp_chassis_pwr_cycle_from_wd(guchar src_chan_num);
extern gint32 ipmi_get_aclost_status(const void *req_msg, gpointer user_data);
extern gint32 ipmi_set_aclost_status(const void *req_msg, gpointer user_data);
gint32 ipmi_cmd_set_pwrbutton_shield_state(const void *req_msg, gpointer user_data);
gint32 ipmi_cmd_get_pwrbutton_shield_state(const void *req_msg, gpointer user_data);
gint32 refresh_pwrbutton_shield_state(void);
gint32 ipmi_set_board_mode(gconstpointer msg_data, gpointer user_data);
#endif  
