
#ifndef __PCIE_CARD_COMM_H__
#define __PCIE_CARD_COMM_H__


#define MASS_STORAGE_CONTROLLER_CODE 0x01
#define RAID_CONTROLLER_CODE 0x04
#define SCSI_CONTROLLER_CODE 0x07
#define HUAWEI_VENDER_ID 0x19e5
#define BDF_UNPARSED_STATE 0
#define BDF_PARSED_STATE 1
#define DEVICE_STATUS_LEN 8

#define HAVE_THIRD_PCIE_RAID_CARD 1
#define NO_THIRD_PCIE_RAID_CARD 0
#define READ_VPD_DELAY_TIME 1000
#define ACCESS_ARRAY_FOR_GET_MODELNUMBER_VALUE 1
#define MAX_BDF_STR_LEN 64

#define ME_CMD_GET_SECBUS_SUBUS 0x18
#define ME_CMD_GET_SECBUS_RSP_OFFSET 1
#define ME_CMD_GET_SUBBUS_RSP_OFFSET 2
#define ME_CMD_GET_CLASS_CODE 0x08
#define ME_CMD_GET_CLASS_CODE_RSP_OFFSET 3
#define ME_CMD_RESPONSE_LEN 4

#define ME_CMD_CLASS_CODE_BRIDGE 6
#define ME_CMD_CLASS_CODE_NETCARD 2
#define PCEI_DEV_MAX_NUM 32
#define PCEI_FUN_MAX_NUM 8
#define RESET_RESOURCES_LEGACY 132

#define ME_REPONSE_ERR (-2)

#define SMM_PCIE_CARD_LOAD_INTERVAL_MS 3000

#define PCIE_UPDATE_RELEX_CPU() vos_task_delay(10)
#define DELETING_POS_LIST_CHECK_RETRY_TIME 30 


#define MAX_SLOT_COUNT 32

#define MAX_CONNECTOR_TYPE_STRING_LEN 64

#define PCIE_BANDWIDTH_ERROR "PCIe Bandwidth Error"
#define PCIE_BANDWIDTH_DECREASE_WARNING_TASK_NAME "PcieDecBandWidthWarning"
#define PROPERTY_PCIECARD_SEGMENT           "Segment"    

typedef struct tag_pcie_dec_bandwidth_msg {
    guint8 bus;
    guint8 device;
    guint8 function;
    guint8 max_link_bandwidth;
    guint8 cur_link_bankwidth;
    guint8 event_dir;
    guint8 segment;
} PCIE_DEC_BANDWIDTH_MSG;

typedef struct tag_card_type_connector_string {
    guint8 type;
    gchar *type_string;
} CARD_TYPE_TO_STRING_S;

typedef struct tag_FAULT_CODE_RANGE {
    guint16 fault_code_min;
    guint16 fault_code_max;
} FAULT_CODE_RANGE_S;

typedef struct {
    guint16 fault_code;
    guint32 fault_class_mask;
} FAULT_CLASS_S;


typedef struct _fpga_fw_restore_status {
    guint8 status;
    guint8 progress;
    guint8 doing;
} FPGA_FW_RESTORE_STATUS;



typedef enum {
    MCU_ACTION_RESET_CHIP = 1,
    MCU_ACTION_FW_SELFHEALING = 2,
    MCU_ACTION_RESET_MCU = 3,
    MCU_ACTION_RESET_MCU_GPIO = 4,
    ACTION_RESET_CHIP = 5
} MCU_ACTION_TYPE_E;


typedef enum {
    FW_SELFHEALING_IN_PROGERSS = 1,
    FW_SELFHEALING_FAILED = 3,
    FW_SELFHEALING_COMPLETED = 100
} FW_SELFHEALING_STATE_E;

typedef struct tag_DEVICE_STATUS_S {
    guint8 *device_status;
    guint32 length;
} DEVICE_STATUS_S;

typedef struct _PcieAddrInfo {
    guint16 segment;
    guint8 bdfno;
    guint8 socket_id;
    guint8 secbus;
    guint8 subbus;
} PcieAddrInfo;

typedef struct tag_bdf_verification_info {
    guint8 bus_index;
    guint8 dev_index;
    guint8 fun_index;
    guint8 expected_num; // 预期可以找到BDF的端口或芯片等数量，硬件决定
    guint16 vid;
    guint16 did;
} BDF_VERIFICATION_INFO;

typedef enum {
    NO_BREAK_FLAG = 0,
    DEVICE_BREAK_FLAG,
    FUNCTION_BREAK_FLAG,
} PCIECARD_BDF_VERIFY_BREAK_FLAG;

typedef struct tag_npu_prop_info {
    guint8 *buf;
    size_t buf_size;
    gchar *prop;
} NPU_PROP_INFO;

#define COLLECT_LOG_FROM_MCU_NOW (1)
#define COLLECT_LOG_FROM_MCU_LATER (0)

#define COLLECT_LOG_FROM_MCU_BUSY (1)
#define COLLECT_LOG_FROM_MCU_IDLE (0)
#define COLLECT_LOG_WAIT_IDLE_5_SECONDS (5)

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 


gboolean is_ocp3_card(OBJ_HANDLE card_handle);
void ocp_card_update_health_info(OBJ_HANDLE obj_handle);
gint32 ocp_card_update_root_port_bdf(guint8 pcie_type, guint8 pcie_slot, guint8 *ocp_bus, guint8 *ocp_dev,
    guint8 *ocp_fun);
gint32 ocp_card_recover_device_status(OBJ_HANDLE object_handle, gpointer user_data);
gint32 ipmi_mcu_reset_mcu(OBJ_HANDLE obj_handle, guint8 device_num);
gint32 ipmi_mcu_reset_chip(OBJ_HANDLE obj_handle, guint8 device_num, guint8 oprate_type);
gint32 ipmi_set_device_action(guint8 *resp_buf, gsize resp_buf_len, guint32 manufactureid,
    OBJ_HANDLE obj_handle, guint8 device_num);
gint32 check_mcu_version_info(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info);
void update_reset_mcu_state(PM_CHIP_INFO *chip_info);
gint32 pcie_get_by_peci_from_cpu(INFO_NEED_GET_PCIE_INFO *pcie_info, gpointer pcie_id);
void pcie_card_update_mcu_rtc_time(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info);
gint32 ipmi_mcu_get_reset_mcu_result(OBJ_HANDLE obj_handle, guint8 device_num, guint8 *result_data, guint32 *length,
    guint32 manufactureid);
gint32 pcie_card_mcu_get_reset_result(OBJ_HANDLE obj_handle, guint8 device_num, guint8 chip_number, guint8 *result);
gint32 ipmi_mcu_set_fpga_golden_fw_restore(OBJ_HANDLE obj_handle);
gint32 ipmi_mcu_get_fpga_golden_fw_restore_result(OBJ_HANDLE obj_handle, guint8 *result_data, guint32 result_data_len);
void pcie_card_update_netcard_mac_address_by_smbus(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info, guint8 portnum);
gint32 pcie_card_load_vpd_xml_via_protocol(OBJ_HANDLE handle, guint8 *load_status);
gint32 _find_node_pcie_card_connector(OBJ_HANDLE node_pcie_summary_handle, NODE_PCIE_INFO_S *info,
    OBJ_HANDLE *obj_handle);
gint32 pcie_disk_get_connector_slot_id(guint8 *slot_id, guint32 *connector_id);
void _update_pcie_connector_binded_data(OBJ_HANDLE connector_handle, const NODE_PCIE_INFO_S *node_pcie_info);
gint32 pcie_card_get_vpd_common_connector(guint8 pcie_slot, OBJ_HANDLE *conn_obj);
gint32 pcie_card_get_vpd_protocol_connector(guint8 pcie_slot, OBJ_HANDLE *conn_handle);
const gchar *pcie_card_get_card_type_string(guint8 type_code);
void sync_local_pcie_card_mac_addr_by_port(OBJ_HANDLE local_pcie_card, guint8 port_index);
gint32 pcie_ssd_update_status(OBJ_HANDLE obj_handle, gpointer user_data);
void update_temp_read_state(OBJ_HANDLE obj_handle, guint16 set_value);
void update_power_read_state(OBJ_HANDLE obj_handle, guint16 set_value);
void pcie_card_update_board_id(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info);
void pcie_card_update_bom_id(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info);
void update_ddr_temp_read_state(OBJ_HANDLE obj_handle, guint16 set_value);
void update_sensors_reading_state(OBJ_HANDLE obj_handle, guint16 state);
void pcie_card_update_temperature(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info);
void pcie_card_update_firmware_version(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info);
void update_optical_temp_power_default(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info);
void pcie_card_update_optical_temp_power(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info);
void pcie_card_update_temperature_sensors(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info);
void pcie_card_update_temperature_by_smbus(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info);
void pcie_card_update_netcard_link_status_by_smbus(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info, guint8 portnum);
void pcie_card_update_optical_module_temp_by_smbus(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info);
void pcie_card_update_sn(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info);
void pcie_card_update_pcb_ver(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info);
void pcie_card_update_ddr_temp(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info);
void pcie_card_update_ecc_count(OBJ_HANDLE obj_handle, OBJ_HANDLE slave_obj_handle, PM_CHIP_INFO *pm_chip_info);
void pcie_card_update_memory_capacity(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info);
void pcie_card_update_dimm_temp_by_smbus(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info);
gint32 pcie_card_update_npu_info(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info, NPU_PROP_INFO *info,
    PROTOCOL_HEAD_PARA head_para);
void pcie_card_update_npu_sn(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info);
void pcie_card_update_npu_fireware_version(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info);
void pcie_card_update_npu_reading_celsius(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info);
void pcie_card_update_npu_power_consunmed_watts(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info);
void pcie_card_scan_update_list_task(void *p);
void get_pcie_netcard_port_num(OBJ_HANDLE pcie_obj_handle, guint8 *portnum);
gint32 pcie_find_netcard_related_pcieaddrinfo_handle(OBJ_HANDLE netcard_handle, guint8 bdfno,
    OBJ_HANDLE *out_pcieaddrinfo_handle);
gint32 pcie_mgnt_init(void);
void init_elabel_info(OBJ_HANDLE obj_handle);
gint32 pcie_card_per_init(void);
gint32 pcie_init_action_foreach(OBJ_HANDLE handle, gpointer user_data);
gint32 pcie_mgnt_init_info_by_i2c(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info);
gint32 pcie_mgnt_init_info_by_smbus(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info);
gint32 pcie_mgnt_init_info_by_std_smbus(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info);
void _pcie_card_parse_bdf_thread(void *bdf_info);
gint32 get_pcie_mgnt_init_state(PM_CHIP_INFO *pm_chip_info);
void pcie_card_collect_log(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info);
gint32 pcie_dump_info_from_mcu(const gchar *path);
void *pcie_card_collect_log_task(OBJ_HANDLE obj_handle);
void pcie_card_vpd_operation_log(guint8 slot_id, guint8 option);
void pcie_set_device_status_to_log(const guint8 *dev_status, guint32 dev_status_len);
void pcie_card_clear_fault_code(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info);
void clear_pcie_card_fault(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info);
void set_pcie_card_infos_default_value(PCIE_CARD_GROUP_INFO_S *pciecard_group_info,
    PCIE_CARD_GROUP_SUB_INFO_S *pciecard_sub_group_info, guint8 slot_id);
gint32 update_fault_code(OBJ_HANDLE object_handle, PM_CHIP_INFO *pm_chip_info, guint8 *buf, guint32 buf_len,
    guint32 fault_state);
gint32 update_fault_state(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info, guint8 *health_state,
    guint8 **fault_code_buf, guint32 *fault_code_buf_len);
void pcie_card_update_fault_code(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info);
void pcie_card_update_fault_code_by_smbus(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info);
void pcie_card_update_fault_code_by_std_smbus(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info);
void notice_pcie_info(PCIE_CARD_GROUP_INFO_S *pcie_card_info, const gchar *class_name, const gchar *method_name);
gint32 get_pcieaddr_info(OBJ_HANDLE pcieaddr_handle, PcieAddrInfo *info);
gint32 set_pcie_addr_info(OBJ_HANDLE pcie_addr_handle, guint8 bus, guint8 me_secbus, guint8 me_subbus);
gint32 check_firmware_info(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info);
void info_pcie_card_change(void);
gint32 pcie_get_info_from_me(const INFO_NEED_GET_PCIE_INFO *info, guint8 *pcie_info);
void pcie_card_get_info_fail(OBJ_HANDLE obj_handle);
gint32 get_pcie_card_extra_info(OBJ_HANDLE obj_handle, PCIE_CARD_EXTRA_INFO *p_extra_info);
gint32 pcie_card_get_class_info(const PCIE_CARD_GROUP_BDF_INFO_S *pcie_card_bdf_info,
    PCIE_CARD_GROUP_CLASS_INFO_S *pcie_cards_class_info);
gint32 _find_node_pcie_card_info(const void *pcie_array_info, guint32 array_info_len, guint16 container_type,
    guint16 container_slot, guint16 pcie_slot, const NODE_PCIE_INFO_S **info_out);
gint32 pcie_card_get_object_handle_by_idinfo(guint32 boardid, guint32 auxid, guint8 slot_id, guint8 type,
    OBJ_HANDLE *object_handle);
gint32 update_smm_pcie_info(OBJ_HANDLE obj_handle, guint16 *vendor_id, guint16 *device_id, guint16 *sub_vendor_id,
    guint16 *sub_device_id);
void pcie_card_update_info_fail(OBJ_HANDLE obj_handle);
void pcie_card_update_resId_info(OBJ_HANDLE obj_handle, guint8 slot_id);
void pcie_card_update_version_info(OBJ_HANDLE obj_handle);
gint32 check_update_pfinfo_condition(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info);
void pcie_card_update_container_info(OBJ_HANDLE obj_handle);
void update_pcie_card_info_by_std_smbus(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info);
void update_pcie_card_info_by_smbus_reg(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info);
gint32 pcie_card_set_device_status(OBJ_HANDLE handle, gpointer data);
gint32 get_pcie_device_present_status(guint8 pcie_slot, guint8 type, guint8 *present);
gint32 pcie_card_recover_device_status(OBJ_HANDLE obj_handle, gpointer user_data);
gint32 pcie_card_lookup_name(guint16 vender_id, guint16 device_id, guint16 sub_vender_id, guint16 sub_device_id,
    gchar vender[], gint32 len_vender, gchar device[], gint32 len_device);
gint32 get_nvme_protocol(OBJ_HANDLE conn_handle, guint8 *protocol_type);
void pcie_get_backplane_slot_array(const gchar *class_name, GByteArray **slot_array);
gboolean pcie_card_mgnt_ready(void);
guchar pcie_card_get_me_access_type(void);
gint32 pcie_card_get_one_valid_line(FILE *fp, gchar line[], gint32 len);
gint32 pcie_get_index_by_slot(guint8 pcie_type, guint8 slot_idx, guint8 *pcie_index);
gint32 pcie_get_cardtype_by_cardaction(OBJ_HANDLE action_handle, guint8 *pcie_type);
gint32 pcie_card_get_vmd_state(void);
gboolean filter_msg_by_sys_channel(const void *msg_data);
void response_error_process(const guint8 errcode, gint32 *channel_type, gulong *taskdelaytime,
    gint8 *retry_time);
void release_thread_sem(guint8 pcie_type);
gint32 pcie_card_get_type_by_property_name(gchar *property_name, guint8 *pcie_type);
gint32 get_smbios_status_value(guint8 *smbios_status);
gint32 get_pcieaddr_handle_by_type_slot(guint8 Component_type, guint8 slot, OBJ_HANDLE *pcie_addr_handle);
gint32 get_bdf_property_value(OBJ_HANDLE obj_handle, INFO_NEED_GET_PCIE_INFO *info);
gint32 pcie_card_present_status_switch(OBJ_HANDLE object_handle, guint8 pcie_present,
    PCIE_ACTION_PRIV_DATA_S *priv_data);
gint32 pcie_card_block_read(const gchar *accessor_name, guint8 *info, guint32 info_size);
gint32 pcie_hotplug_status_foreach(OBJ_HANDLE handle, gpointer user_data);
void get_slotid_by_handle(OBJ_HANDLE obj_handle, guchar *slot_id);
gint32 pcie_card_get_business_port_handle_by_channel(OBJ_HANDLE pcie_handle, guint8 opt_id, OBJ_HANDLE *port_handle);
gint32 pcie_card_get_optical_module_handle_by_channel(OBJ_HANDLE pcie_handle, guint8 opt_id, OBJ_HANDLE *opt_handle);
gint32 _pf_port_compare(gconstpointer a, gconstpointer b);
guint8 pcie_card_get_power_state(OBJ_HANDLE obj_handle);
void pcie_card_check_optical_present_changed(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info);
gint32 check_is_atlas_smm(void);
GVariant *create_gvar_array(PCIE_CARD_INFO_S *info, guint32 element_num);
guint32 calc_ddr_temp_read_fail_duration(PM_CHIP_INFO *pm_chip_info);
void recover_saved_value(OBJ_HANDLE obj_handle, const char *pro_name, DF_OPTIONS op);
extern gint32 g_pcie_card_load_completed;
void pcie_card_update_log(PM_CHIP_INFO *pm_chip_info, gchar *log_dir_path, guint16 key, guint16 path_len,
    const gchar *log_name_prefix);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif 
