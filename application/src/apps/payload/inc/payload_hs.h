

#ifndef __PAYLOAD_HS_H__
#define __PAYLOAD_HS_H__

// hotswap event, include cause and hotswap state
#define HSE(cause, hotswap) (guchar)((guchar)((cause) << 4) | (hotswap))




#define FRU_HS_NORMAL_STATE_CHANGE 0


#define FRU_HS_SET_FRU_ACTIVATION 1


#define FRU_HS_HANDLE_SWITCH 2


#define FRU_HS_PROGRAMMATIC_ACTION 3


#define FRU_HS_COMMUNICATION_LOST 4


#define FRU_HS_PHYSICALLY_SEPARATE 5


#define FRU_HS_EXTRACTION 6


#define FRU_HS_PROVIDED_INFORMATION 7


#define FRU_HS_INVALID_HA_DETECTED 8


#define FRU_HS_UNEXPECTED_DEACTIVATION 9


#define FRU_HS_POWER_FAILURE 10


#define FRU_HS_CAUSE_UNKNOWN 0x0F

#define MAX_HS_STATE 7

#define HALF_BYTE 4

#define HOTSWAP_ACTIVATE_FRU 1
#define HOTSWAP_DEACTIVATE_FRU 0

#define MAX_POWER_DRAW 20
#define HOTSWAP_TEMP_BUF_LEN 20

#define CHECK_NUM 10
#define SENSOR_TYPE_FRUHOTSWAP 0xF0

#define LOG_POWER_ON_DELAY "The server is in delay mode %s, with a delay of %.1f seconds to power on.\n"

typedef struct tag_Hotswap_S {
    guchar last_hotswap_state;
    guchar cur_hotswap_state;
    guchar hotswap_cause;

    // 对于"一次性条件"，要考虑清除时机
    guchar pin_mated : 1;               // 1=插稳了
    guchar activated : 1;               // 1=收到set fru activation(activate)     (一次性条件)
    guchar deactivated : 1;             // 1=收到set fru activation(deactivate)   (一次性条件)
    guchar insertion_criteria_met : 1;  // 1=满足插入条件
    guchar extraction_criteria_met : 1; // 1=满足拔出条件
    guchar activate_completed : 1;      // 1=激活完成                             (一次性条件)
    guchar deactivate_completed : 1;    // 1=去激活完成                           (一次性条件)
    guchar setpowerlevel_0 : 1;         // 1=收到set power level(level=0)         (一次性条件)

    guchar communication_lost : 1;       // 1=IPMC与各FRU通讯正常 (考虑到FRU上也有微处理器的情况)
    guchar power_failure : 1;            // 1=单板下电了/掉电了                    (一次性条件)
    guchar unexpected_deactivation : 1;  // 1=非预期的去激活                       (一次性条件)
    guchar activate_lockbit : 1;         // 1=阻止M1->M2  (有条件设置、清除)
    guchar deactivate_lockbit : 1;       // 1=阻止M4->M5  (有条件设置、清除)
    guchar power_alloced : 1;            // 1=收到set power level (level!=0)       (一次性条件)
    guchar unnormal_status_recovery : 1; // 1=收到unnormal status recovery (一次性条件)
    guchar reserved : 1;
} FRU_HOTSWAP_STATE_S, *PFRU_HOTSWAP_STATE_S;

typedef struct tag_PowerLevel_S {
    guchar dynamic_power_cfg;
    guchar power_level;
    guchar delay_to_stable;
    guchar power_multiplier;
    guchar max_power_level;
    guchar power_draw[MAX_POWER_DRAW];
} FRU_POWER_LEVEL_S, *PFRU_POWER_LEVEL_S;

typedef struct tagPowerDelay_S {
    guchar delay_mode;
    guchar delay_count[4];
} POWER_DELAY_S, *PPOWER_DELAY_S;

typedef struct tag_FRU_S {
    FRU_HOTSWAP_STATE_S hotswap;
    GAsyncQueue *hotswap_event_qid;
} FRU_HOTSWAP_S, *PFRU_HOTSWAP_S;

enum FRU_Events {
    FRU_PIN_MATED,

    FRU_ACTIVATED_DEACTIVATED, // 1

    FRU_INSERTION_CRITERIA_MET,  // 2
    FRU_EXTRACTION_CRITERIA_MET, // 3

    FRU_SET_M1_LOCKBIT, // 4
    FRU_SET_M4_LOCKBIT, // 5

    FRU_ACTIVATED_COMPLETED,   // 6
    FRU_DEACTIVATED_COMPLETED, // 7

    FRU_SETPOWERLEVEL0, // 8

    FRU_COMMUNICATION_LOST, // 9
    FRU_POWER_FAILURE,      // 10

    FRU_UNEXPECTED_DEACTIVATION, // 11

    FRU_UNNOMAL_STATE_RECOVERY // 12
};

typedef struct tag_FRUEvent_S {
    guchar fruid;
    guchar event;
    guchar value : 4;
    guchar casue : 4;
    guchar count; // 老化计数
} FRU_EVENT_S, *PFRU_EVENT_S;

extern guint32 g_max_fru_id;

extern gint32 hs_com_lost_evt_process(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);
extern void hs_send_evt(guchar fru_id, PFRU_EVENT_S pevent);
extern void hse_activate_completed(guchar fruid);
extern void hse_communication_lost(guchar fruid, guchar value);
extern void hse_deactivate_completed(guchar fruid);
extern void hse_pwr_failure(guchar fruid, guchar value);
extern guchar hse_fru_activate(guchar fruid, guchar activate, guchar flag);
extern guchar hse_fru_activate_policy(guchar fruid, guchar mask, guchar bits);
extern void hse_pwr_level0(guchar fruid);
extern void hse_unexpected_deactivation(guchar fruid);
extern guchar hs_get_cur_state(guchar fruid);
extern guchar hs_get_activate_policy(guchar fruid, guchar *policy);
extern gint32 hs_is_auto_management(void);
extern gint32 hs_is_fru_managed(guchar fru_id);
extern guint32 hs_get_fru_num(void);
extern gint32 hs_dump_cmd(GSList *input_list);
extern guchar hs_init(void);
extern void hs_start(void);
extern void hs_process_init(void);
extern void hs_state_restore(void);
extern guchar hs_fru_init(void);
extern void hs_thread_init(void);
extern gint32 hse_unnormal_status_recovery(guchar fruid);
extern void hs_do_default_mode_delay(gboolean is_logged);
extern void hs_do_pwr_on_delay(void);
extern gint32 hs_get_stateless_auto_power_on(void);
extern void osca_hotswap_fix_task(void);
extern void update_hotswap_fix_flag(void);
extern gint32 close_nand_flash_debug(GSList *input_list);
extern gint32 set_nand_flash_value(GSList *input_list);

#endif 
