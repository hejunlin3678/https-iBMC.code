
#ifndef SOC_BOARD_H
#define SOC_BOARD_H

#include "pme/pme.h"
#include "pme_app/pme_app.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

enum SocBrdMntrState {
    SOCBRD_MNTR_INIT = 0,
    SOCBRD_MNTR_WAIT,
    SOCBRD_MNTR_UPDATE,
    SOCBRD_MNTR_FAIL,
    SOCBRD_MNTR_EXIT // 占位符，状态机代码暂不使用
};

#define SOCBRD_MNTR_ITEM_PERIOD 0
#define SOCBRD_MNTR_ITEM_STATIC 1

#define RSP_INDEX_ERROR_CODE 0

#define MCU_RETRY_TIMES 3

typedef gint32 (*soc_board_mntr_proc)(OBJ_HANDLE obj_handle, PM_CHIP_INFO *info);

typedef struct tag_SocBrdMntrItem {
    guint8 is_static;
    soc_board_mntr_proc func;
} SocBrdMntrItem;


typedef struct tag_SocBrdMntrCB {
    PM_CHIP_INFO *pm_chip_info;
    OBJ_HANDLE handle;
    enum SocBrdMntrState state;
    guint8 id;
    // 支持热插拔
    guint8 exit_flag;
    // 支持下电停止轮询
    guint8 last_power_state;
    guint8 power_state;
    // 初始化失败重试
    guint8 retry_cnt;
    // 单次获取报文更新标志，支持最大256个回调
    guint8 item_table_len;
    const SocBrdMntrItem *item_table;
    guint32 item_updated[8];
    // 下电恢复默认值回调，当前恢复代码量较少，只使用一个回调
    void (*set_default_proc)(OBJ_HANDLE obj_handle, guint16 mask);
    gint32 (*update_firm_ver_proc)(OBJ_HANDLE obj_handle, PM_CHIP_INFO *info, guint8 *updated);
    // SoC主板带外日志收集
    GMutex log_mutex;
    gulong log_task_id;
} SocBrdMntrCB;

void soc_board_init(void);
void soc_board_start(void);
gint32 soc_board_add_object_callback(OBJ_HANDLE obj_handle);
gint32 soc_board_del_object_callback(OBJ_HANDLE obj_handle);
gint32 soc_board_mcu_get_data(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 soc_board_mcu_set_data(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 soc_board_set_host_fault_info(OBJ_HANDLE object_handle, GSList *caller_info,
    GSList *input_list, GSList **output_list);
gint32 upload_boot_cert_string_to_mcu(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 get_boot_cert_status_from_mcu(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 soc_board_start_log(OBJ_HANDLE obj_handle, gpointer user_data);
gint32 collect_soc_board_log_manual(guint32 position, guint8 slot);
gint32 soc_board_set_secureboot_to_mcu(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 soc_board_mcu_get_secureboot(OBJ_HANDLE obj_handle, GSList* caller_info, GSList* input_list,
    GSList** output_list);
void oob_chan_start(void);
gint32 oob_chan_add_object_callback(OBJ_HANDLE obj_handle);
gint32 oob_chan_del_object_callback(OBJ_HANDLE obj_handle);
PM_CHIP_INFO *oob_init_chip_info(OBJ_HANDLE oob_handle);
gint32 write_ram_uuid_atlas800d_g1(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);
gint32 write_ram_sn_atlas800d_g1(OBJ_HANDLE object_handle, gpointer user_data, gchar* property_name,
    GVariant* property_value);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
