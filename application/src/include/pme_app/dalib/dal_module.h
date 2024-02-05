/*******************************************************************
                   Copyright 2008 - 2013, Huawei Tech. Co., Ltd.
                             ALL RIGHTS RESERVED

Filename      : dal_module.h
Author        : z00356691
Creation time : 2017/4/21
Description   : APP间公共接口定义

Version       : 1.0
********************************************************************/

#ifndef __DAL_MODULE_H__
#define __DAL_MODULE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "libxml/tree.h"
#include "json/json.h"
#include "pme/dflib/dflstatic.h"

typedef struct {
    guint8 port_param;
    guint8 eth_type;
} VLAN_CONFIG_PARAM_ETH_MAP;

gint32 dal_get_user_role_priv_enable(const gchar* user_name, const gchar* priv_property_name, guchar* priv_enable);
gint32 dal_convert_raid_bdf_info(const gchar* bdf_info, guint8  info_len, guint8* bdf, guint32 bdf_len);


typedef struct tag_Timezone_City
{
    guint8 city_num;
    gchar  area[AREA_CITY_NAME_SZ];
    gchar  city[TIMEZONE_CITY_NUM][AREA_CITY_NAME_SZ];
}TZ_CITY;

typedef struct {
    unsigned int cmd;
    unsigned int clk_sel;    /* 时钟选择，0：外部时钟 1：内部时钟 */
    unsigned int clk_edge_sel; /* 0：上升沿发送 1：下降沿发送 */
    unsigned int dri_power; /* 驱动能力配置 */
} ETH_DRV_RMII_CLK_CFG;

#define SMALL_BUFFER_SIZE       256
#define USER_ENPASSWD_LENGTH    32
#define MAX_SIDE_STR_LEN        1024
#define MAX_ETH_OBJ_NUM         256
#define MAX_ETH_PORT_NUM        16

#define REDFISH_REQ_FOR_WEB_AUTH               2
#define REDFISH_REQ_FROM_WEBUI                 1
#define REDFISH_REQ_FROM_OTHER_CLIENT          0
#define MAX_LOG_INTERFACE_MODULE_LEN           64

#define MODE_STR_LEN 64

/* 被加密的数据最大长度，底层实现和USER_ENPASSWD_LENGTH耦合，当前保持一致 */
#define ENCRYPTED_DATA_MAX_LEN       (USER_ENPASSWD_LENGTH - 1)
#define LOCAL_BUS_OBJ_NAME_LEN 64
#define ETH_DRV_RMII_CLK_SEL	0x00000003 /* RMII选择外部或内部时钟 */
#define ETH_P_ALL	0x0003
#define SIOCDEVDRVCMD 0x89F4
gint32 get_snmp_hash_protocol(const guint8 protocol, gulong* hashtype, guint32 hashtype_len, guint32 is_auth);
void convert_bytes_to_string(const guchar* input, gsize len, guchar* output);
void convert_string_to_bytes(const guchar* input, gsize input_len, guchar* output);
gint32 get_plain_workkey(const guint8* component, gint32 component_size, const gchar* encrypt_workkey, gint32 encrypt_workkey_len,
    guint8* plain_workkey, gint32 plain_workkey_size, guchar padding);
gint32 dal_set_property_byte_try_agent(OBJ_HANDLE obj_handle, const gchar * property_name, guchar in_prop_val, DF_OPTIONS options);
gint32 dal_get_bma_connect_state(guint8 *bma_connect_state);
gint32 dal_update_specific_resource_from_bma(const gchar* resource_type);
gint32 dal_exec_dft_action(OBJ_HANDLE action_obj, gint32 block_offset, guint32 block_len);
gint32 dal_exec_dft_action_read_byte(OBJ_HANDLE action_obj, gint32 block_offset, guint8* action_data);
OBJ_HANDLE dft_get_action_obj(OBJ_HANDLE dft_obj, guint32 action_num);
gint32 dal_get_time_offset(const gchar* tz, gint16* time_offset);
gint32 dal_convert_time_offset_to_utc_or_gmt(gint16 time_offset, gint32 tz_format, gchar* tz, guint32 sz);
gint32 dal_check_dedicated_port_exist(gint32 port_num);
gint32 dal_get_dedicate_or_aggregation_port_silk_num(OBJ_HANDLE eth_group_handle, guint8* silk_num);
gint32 dal_convert_timeoffset_to_utc(gint16 time_offset, gchar* tz, guint32 sz);
/* 支持SMM加载PCIe标卡 */
gint32 dal_get_anchor_related_connector(OBJ_HANDLE anchor_handle, OBJ_HANDLE* connector_handle);
gint32 dal_get_connector_related_anchor(OBJ_HANDLE connector_handle, OBJ_HANDLE* anchor_handle);
gint32 dal_get_compositionzone_info(OBJ_HANDLE obj_handle, const gchar *property_name, gchar* string_buf, guint32 string_len);
void dal_print_resource_compositions_list(OBJ_HANDLE obj_handle);
gint32 dal_get_identifier_related_endpoint(OBJ_HANDLE identifier_handle, OBJ_HANDLE* ep_handle);
gint32 dal_get_specific_position_object_byte(OBJ_HANDLE   obj_in,
                                                        const gchar* class_name,
                                                        const gchar* property_name,
                                                        guchar       prop_val,
                                                        OBJ_HANDLE*  obj_out);
gint32 dal_get_specific_position_object_string(OBJ_HANDLE   obj_in,
                                                        const gchar* class_name,
                                                        const gchar* property_name,
                                                        const gchar* prop_val,
                                                        OBJ_HANDLE*  obj_out);
gint32 dal_set_other_module_property_value(OBJ_HANDLE obj_handle, const gchar* property_name,
                                                         GVariant* var, DF_OPTIONS options);
gint32 dal_set_other_module_property_value_string(OBJ_HANDLE obj_handle, const gchar *property_name,
    const gchar *in_prop_val, DF_OPTIONS options);
gint32 dal_get_specific_object_backward_position(OBJ_HANDLE ref_handle, const gchar* class_name, OBJ_HANDLE* obj_out);
gint32 dal_get_container_specific_handle_list(OBJ_HANDLE container_handle,
                                                       const gchar* class_name,
                                                       GCompareFunc compare_func,
                                                       gconstpointer data,
                                                       GSList** handle_list);
gint32 dal_frudev_opt_eeprom_wp(OBJ_HANDLE object_handle, const gchar* prop_name, guint8 direction);
gint32 dal_update_string_fmtor_array_prop(OBJ_HANDLE obj_handle, const gchar* prop_name);
gint32 dal_update_inactuser_start_time( const gchar* user_name );
gint32 dal_get_veth_addr( gchar* bmc_veth_ip, guint32 bmc_veth_ip_len);
gint32 dal_ip_format_v4_check(const gchar* ipv4);
/* 支持节点配置机框拓扑 */
const gchar* dal_get_pcie_topo_code_string(PCIeTopoCode code);
const gchar* dal_rf_get_op_log_module_name(guchar req_src_type);
void dal_print_hex_array(const guchar* hex_array, guint32 len, gchar* result_buf, guint32 result_buf_len);
gint32 dal_sms_forward_redfish_request(const gchar* request_str, gint32 request_len, gchar* response_str, gint32 response_len);
gint32 dal_get_bma_resource_jso(const gchar* uri, json_object** resource_jso);
/* 使用编译宏控制APP的大小 */
gint32 dal_class_method_callback_return_ok(const OBJ_HANDLE group_obj, GSList* caller_info, GSList* input_list, GSList** output_list);
gint32 dal_class_method_callback_return_error(const OBJ_HANDLE group_obj, GSList* caller_info, GSList* input_list, GSList** output_list);
gint32 dal_ipmicmd_callback_rsp_status_invalid(gconstpointer req_msg, gpointer user_data);
/* 通用功能，从SensorAlarm模块移至公共库  PN:AR.SR.SFEA02130925.003.001 支持采集的数据列表*/
gint32 dal_common_variant_to_int32(GVariant* v, gint32* value, gchar* type);
gint32 dal_common_variant_to_uint32(GVariant* v, guint32* value, gchar* type);
gint32 dal_common_variant_to_double(GVariant* v, gdouble* value);
gint32 dal_get_property_gvariant_string(GVariant* v, const gchar* format_str, gchar* str_out, guint32 str_out_len);
gint32 dal_get_ref_object_handle_list(OBJ_HANDLE obj_handle, const gchar* property_name, GSList** obj_list, guint32* obj_count );
gint32 dal_common_get_default_variant_value(GVariant* in_v, GVariant** out_v);
gint32 dal_enter_ft_st(void);
gint32 dal_eth_get_name(OBJ_HANDLE eth_obj, gchar* eth_name, gsize eth_name_len);

gboolean is_slot_eth_name_right(guint8 slot_id, gchar *eth_name);
void dal_common_reboot_request_process(REBOOT_CTRL ctrl);
gint32 dal_uart_config(guint32 uart, guint32 port, guint32 baud, gint32* fd);
gchar* dal_clear_string_blank(gchar* string_src, guint32 string_size);
gint32 dal_get_file_name(const gchar* file_path, gchar* file_name, size_t buf_len);
gint32 dal_get_fm_file_accessible(const gchar *short_name, const gchar *module_name, FM_POSITION_E position);
gint32 dal_get_logical_drive_object_handle(guint8 ctrl_id, guint16 ld_id, OBJ_HANDLE* obj_handle);
gboolean dal_test_controller_vendor(guint8 type_id, guint8 vendor_id);
gboolean dal_test_controller_vendor_by_ctrl_id(guint8 ctrl_id, guint8 vendor_id);
gboolean dal_check_ld_name_too_long(guint8 ctrl_id, gsize len);
gsize dal_get_ld_name_max_len(guint8 ctrl_id);
gint32 dal_get_ctrl_id_by_ld_handle(OBJ_HANDLE ld_handle, guint8 *ctrl_id);
gboolean dal_test_pd_under_which_vendor(OBJ_HANDLE pd_handle, guint8 vendor_id);
gboolean dal_check_support_logical_drive(OBJ_HANDLE ctrl_obj);
guint8 dal_raid_config_ready(void);
gint32 dal_delaymode_int2str(guint8 delay_mode, gchar* delay_mode_str, guint32 str_size);
gint32 dal_delaymode_str2int(const gchar* delay_mode_str, guint8* delay_mode);
guint8 dal_active_mode_str2int(const gchar *active_mode_str);
guint8 dal_bios_active_mode_str2int(const gchar *active_mode_str, GHashTable *active_mode_tbl);
gint32 generate_mode_str(guint8 activate_mode, GHashTable *active_mode_tbl, gchar *result, guint32 length);
gint32 dal_set_active_mode_tbl(const char *file_data, const char *file_name, GHashTable **active_mode_tbl);
extern gboolean dal_is_smm_support_multiplane(void);
gboolean dal_is_support_multiplane(void);
extern gboolean dal_is_dft_backplane(void);
gboolean get_dft_eth_num(GSList *obj_list, gchar *eth_name, guint8 slot_id, guint8 *eth_num, guint16* p_vlan_id);
gboolean dal_is_support_eth_function_specific_propery(const gchar* property_name, guint8 function_type);
gboolean dal_is_support_eth_define_specific_propery(const gchar* property_name);
gint32 dal_parse_sp_update_conf(guint8 shift, gchar* strline, gchar* buff, guint32 buff_len);
guint32 dal_get_ipaddr(gint32 type, const gchar *eth_name);
gint32 dal_fill_ipmi_get_bmc_config_req_common_field(gconstpointer msg_data,
    const guint8 expected_data_len,
    const guint8 para_selector,
    BMC_CONFIGURATION_REQ_S *req,
    BMC_CONFIGURATION_RSP_S *rsp);
gint32 dal_is_sensitive_info(const gchar *class_name, const gchar *property_name, GSList *sensitive_info_table);
gint32 dal_sensitive_info_table_init(GSList **sensitive_info_table);
gint32 dal_get_ibmc_reset_cause(gint32* reset_cause);
void dal_restore_dft_path(void);
gint32 dal_try_get_me_status(guint8 try_cnt, guint8 succ_cnt, guint8* me_status);
gint32 dal_try_force_me_recovery(guint8 try_cnt);
gint32 dal_get_interface_maxspeed_str2int(const gchar *str, guint32 *o_speed);
const gchar *dal_get_vlan_name(guint32 config_port);
const gchar *dal_get_vlan_name_by_eth_type(guint8 eth_type);
gint32 dal_get_ncsi_vlan_info(OBJ_HANDLE eth_group, guint32 *ncsi_vlan_state, guint16 *ncsi_vlan_id);
gint32 dal_get_ethgroup_vlan_info(OBJ_HANDLE eth_group, guint32 *group_vlan_state, guint16 *group_vlan_id);
gboolean dal_is_dedicated_pcie_mgnt_port_existed(void);
gint32 dal_get_bma_id(json_object* resource_jso, char* bma_id, size_t bma_id_len);
const char* dal_get_bdf_info_by_bma(json_object* resource_jso, const gchar* rf_bdf_str);
gboolean dal_is_pcie_npu(void);
gint32 dal_check_support_pciedevices(gboolean *issupport);
gint32 dal_get_board_and_product_id(guint8 *board_id, guint32 *product_id);
gboolean dal_is_tianchi_server(void);
gint32 dal_get_component_type_by_fruid(guint8 fru_id, guint8 *comp_type);
gboolean dal_is_pangea_enterprise_board(void);
gboolean dal_is_pangea_board(guint8 pangea_board);
gboolean dal_is_compute_storage_smm_board(void);

gint32 dal_read_xml_root_node(const gchar *file_path, xmlNode **root_element, xmlDoc **doc);
gint32 dal_get_xml_node_context(xmlNode *node, gchar *content, size_t content_len);
gint32 dal_get_memory_channel_logical_id_by_physical_id(guint8 phy_id, guint8 *logi_id);
gint32 dal_get_board_id(guint8* board_id);
gint32 dal_get_ethtool_stringset(gint32 socket_fd, gchar *if_name, gpointer* strings, guint32 *statistics_count);
extern const gchar* g_trigger_mode_str[2][EVENT_MONITOR_MODE_MAX_NUM + 1];
gint32 get_custom_manufacturer(gchar *custom_manu, gint32 buff_size);
gint32 dal_get_array_list_by_ctrl(const gchar *ref_ctrl_obj_name, GSList **obj_list);
gboolean dal_check_ctrl_using_subarray(const gchar *ctrl_name);
gboolean dal_is_pangea_pacific_series(void);
gint32 dal_get_specific_component_obj(guint8 dev_type, guint8 dev_num, OBJ_HANDLE *obj_handle);
gboolean dal_is_cst_full_compute_node(void);
gboolean dal_is_cst_half_node(void);
gint32 dal_eth_rmii_clk_cfg(guint8 eth_num, ETH_DRV_RMII_CLK_CFG *rmii_cfg);
gboolean check_npu_type_match_dboard(void);
gint32 dal_get_silk_id(OBJ_HANDLE obj_handle, const gchar *prop_A, const gchar *prop_B, guint8 *Value);
gboolean dal_is_three_oceans(void);
gint32 dal_destroy_fru_data(FRU_INFO_S *fru_info);
gboolean dal_is_donghai_3u(void);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /*end of __DAL_MODULE_H__*/

