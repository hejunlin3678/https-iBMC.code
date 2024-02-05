

#ifndef __CARD_MANAGE_MODULES_H__
#define __CARD_MANAGE_MODULES_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 




#define MAINBOARD_PCHMODEL_MAX_LEN 64



#define BWU_WAVE_QUE_TYPE_ADD 0
#define BWU_WAVE_QUE_TYPE_DEL 1


#define DEVICE_INFO_POWER_CONTROL 21

#define AVAILABLE 1
#define NOT_AVAILABLE 0

#define FIRST_CPU 1
#define SECEND_CPU 2
#define THIRD_CPU 3
#define FOURTH_CPU 4
#define RESOURCE_VALID 1

#define SET_BWU_WAVE_TITLE_DELAY 1000
#define SET_BWU_WAVE_TITLE_RETRY 5
#define DEVICE_NAME_SPLIT_FLAG "$"
#define DEVICE_NAME_ADD_LOCATION_FLAG "#"
#define OCP_DEF_THES_TEMP 0x35000

#define PCIE_CARD_NAME_MAX_LEN  30

typedef enum tag_hotswap_state_s {
    HOTSWAP_M0 = 0,
    HOTSWAP_M1,
    HOTSWAP_M2,
    HOTSWAP_M3,
    HOTSWAP_M4,
    HOTSWAP_M5,
    HOTSWAP_M6
} HOTSWAP_STATE_S;

#pragma pack(1)
typedef struct tag_get_netcard_chip_info_request_info_s {
    guint32 manufacturer : 24; // 产商信息
    guint32 sub_command : 8;   // 子命令
    guint8 blade_id;
    guint8 parameter;
    guint8 offset;
    guint8 length;
} GET_NETCARD_CHIP_INFO_REQ_S;

typedef struct tag_get_netcard_chip_info_response_info_s {
    guint32 comp_code : 8;     // 产商信息
    guint32 manufacturer : 24; // 子命令
    guint8 end_flag;
    guint8 data[200];
} GET_NETCARD_CHIP_INFO_RSP_S;
#pragma pack()

#pragma pack(1)
typedef struct tag_set_component_powers_reqs_s {
    guint32 manufacturer : 24; // 产商信息
    guint32 sub_command : 8;   // 子命令
    guint8 device_type;
    guint8 device_info;
    guint8 pre_reserved;
    guint16 device_num;
    guint8 reserved;
    guint16 offset;
    guint8 length;
    guint8 cmd_code;
} SET_COMPONENT_POWERS_REQS_S;

typedef struct tag_set_component_powers_resp_s {
    guint32 comp_code : 8;     // 产商信息
    guint32 manufacturer : 24; // 产商信息
} COMPONENT_POWERS_RESP_S;
#pragma pack()

#pragma pack(1)
// 用于动态保存网络带宽使用率图表数据
typedef struct tag_bport_bwu_wave_info {
    STAT_S *stat_whole_life;
    STAT_S *stat_one_cycle;

    guint8 silk_num;
    OBJ_HANDLE card_handle;
} BPORT_BWU_WAVE_INFO;

// 用于动态生成网络带宽使用率图表
typedef struct tag_netcard_bwu_wave_info {
    WAVE_S *wave_handle;

    guint8 port_num;
    OBJ_HANDLE card_handle;
} NETCARD_BWU_WAVE_INFO;
// 框架回调接口里不能有锁，在网卡新增和移除场景，用队列的处理方法达到加锁的目的
typedef struct tag_net_bwu_wave_que_msg {
    guint8 que_type;
    OBJ_HANDLE card_handle;
} NET_BWU_WAVE_QUE_MSG;
#pragma pack()

typedef gint32 (*pcie_card_log_handler_t)(const gchar *path);

typedef struct pcie_card_log_provider {
    const gchar card_name[PCIE_CARD_NAME_MAX_LEN];
    pcie_card_log_handler_t pfn_get;
} PCIE_CARD_LOG_PROVIDER;

gint32 card_manage_init(void);
gint32 card_manage_start(void);
gint32 card_manage_add_object_callback(const gchar *class_name, OBJ_HANDLE object_handle);
gint32 card_manage_add_complete_callback(guint32 position);
gint32 card_manage_del_object_callback(const gchar *class_name, OBJ_HANDLE object_handle);
gint32 card_manage_del_complete_callback(guint32 position);
gint32 card_manage_dump_info(const gchar *path);
gint32 card_manage_reboot_request_process(REBOOT_CTRL ctrl);
gint32 card_manage_payload_event_callback(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);
gint32 card_manage_updata_card_info(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);

gint32 method_card_manage_set_fanboard_led(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

gint32 card_manage_reload_fan_info(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

void net_card_set_device_status_default_value(void);

gint32 card_manage_set_net_from_bma(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

gint32 card_manage_set_firmver_from_mctp(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 card_manage_set_netcard_sn(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 card_manage_set_vid_did_from_mctp(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

gint32 card_manage_set_pch_model_from_bios(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

gint32 get_card_cpu_id(OBJ_HANDLE crad_handle, guchar *cpu_id);

gint32 net_card_init(void);

gint32 card_record_rollback(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);

gint32 init_board(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list);

gint32 hddbackplane_set_hdd_temp(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

gint32 net_card_add_object_callback(OBJ_HANDLE obj_handle);
gint32 net_card_del_object_callback(OBJ_HANDLE object_handle);
gint32 get_pcie_card_resId(void);
gint32 cpu_id_logical_to_physical(guint8 logic_id, guint8 *phsical_id);

gint32 ipmi_get_net_card_chip_info(const void *msg_data, gpointer user_data);



gint32 card_manage_update_bandwidth_usage_wave(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
void network_bandwidth_usage_waveform_init(void);



gint32 method_netcard_bandwidth_usage_wave_record_clear(OBJ_HANDLE object_handle, GSList *caller_info,
    GSList *input_list, GSList **output_list);

gint32 netcard_bandwidth_usage_rollback(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

gint32 ipmi_cmd_component_power_ctrl(const void *msg_data, gpointer user_data);


gint32 net_card_set_eid(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list);



gint32 card_manage_get_os_reset_flag(guint8 *reset_state);
void card_manage_update_os_reset_flag(guint8 reset_state);

gint32 card_manage_set_pcie_link_ability(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 card_manage_set_pcie_link_info(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 card_manage_set_pcie_chip_temp(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 card_manage_set_pcie_error_info(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

gint32 card_manage_set_lldp_enable(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

gint32 card_manage_set_pcie_desc(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 card_manage_restore_fpga_card_percent(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 card_manage_restore_fpga_card(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 card_manage_operate_virturl_fpga_card(guint8 slot_in, guint8 flg_in);
gint32 card_manage_waite_virturl_fpga_card(guint8 slot_in, guint32 position, guint8 flg_in, OBJ_HANDLE *obj_handle);
gint32 check_third_pcie_raid_card(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

gint32 card_manage_set_lldp_over_ncsi_enable(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 card_manage_set_netcard_chip_temp(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 card_manage_set_netcard_manu_id(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 net_card_set_pre_plug(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 net_card_set_default_config(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);
gint32 net_card_set_ncsi_package_id(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

void card_manage_update_fault_state(guint8 fault_state);
void card_update_property_string(json_object *resource_jso, gchar *redfish_prop, OBJ_HANDLE pme_handle,
    gchar *pme_prop);
void card_update_rootbdf_property_string_with_limit(json_object *bdf_num_jso, gchar *redfish_prop,
    OBJ_HANDLE card_handle, gchar *card_prop);
gint32 net_card_recover_device_status(OBJ_HANDLE obj_handle, gpointer user_data);

void card_replace_record_detect(void);
gint32 net_card_set_rootbdf(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 process_as_status_change(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
                                GVariant *property_value);

gint32 update_bmc_build_time_init(void);




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif 
