/******************************************************************************

                  版权所有 (C), 2001-2021, 华为技术有限公司

 ******************************************************************************
  文 件 名   : dalib.h
  版 本 号   : 初稿
  作    者   : m00168195
  生成日期   : 2016年4月12日
  最近修改   :
  功能描述   : dal库对外头文件,【重要】:此头文件只定义对外提供的接口
  函数列表   :
  修改历史   :
  1.日    期   : 2016年4月12日
    作    者   : m00168195
    修改内容   : 创建文件

******************************************************************************/
#ifndef __DALAPI_H__
#define __DALAPI_H__

#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "dal_encrypt.h"
#include "dal_set_agent_object_interface.h"
#include <openssl/x509.h>
#include "dal_macro.h"
#include "json/json.h"
#include "libxml/tree.h"
#include "libxml/parser.h"
#include "libxml/xpath.h"
#include <sys/vfs.h>
#include <openssl/x509v3.h>
#include <curl/curl.h>
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef struct tag_dbd_midavg1_s {
    guint32 dbd_val_num;
    gint8 *dbd_val_buf;
} DBD_MIDAVG1_S;

typedef enum {
    NUM_TYPE_UINT8 = XML_PROPERTY_TYPE_y,
    NUM_TYPE_INT16 = XML_PROPERTY_TYPE_n,
    NUM_TYPE_UINT16 = XML_PROPERTY_TYPE_q,
    NUM_TYPE_INT32 = XML_PROPERTY_TYPE_i,
    NUM_TYPE_UINT32 = XML_PROPERTY_TYPE_u,
    NUM_TYPE_FLOAT = XML_PROPERTY_TYPE_d,
    NUM_TYPE_UNKNOWN = 0xff
} NUMERIC_TYPE_E;

typedef struct tag_http_resp_str {
    gchar *resp_status;
    gchar *resp_head;
    gchar *resp_body;
} HTTP_RESPONSE_DATA_S;

typedef struct {
    const gchar *name;
    guint8      type;
    void        *out;
    guint32     out_len;
} GET_PROP_TYPE_S;

typedef struct in_addr IN_ADDR;
gchar *dal_inet_ntop_ipv4(IN_ADDR in);

gchar *dal_log_time(void);
long dal_get_bmc_uptime(void);
gint32 dal_get_object_handle_nth(const gchar *class_name, guint32 n, OBJ_HANDLE *obj_handle);
gint32 dal_get_refobject_handle_nth(OBJ_HANDLE obj_handle , const gchar *property_name, guint32 n, OBJ_HANDLE *ref_obj_handle);
gint32 dal_get_specific_obj_num_byte(const gchar *class_name, const gchar *prop_name, guchar prop_val, guint8 *p_nums);

/* 通用设置属性 */
gint32 dal_set_property_value_boolean(OBJ_HANDLE obj_handle, const gchar* property_name, gboolean in_prop_val, DF_OPTIONS options);
gint32 dal_set_property_value_byte( OBJ_HANDLE obj_handle, const gchar* property_name, guchar in_prop_val, DF_OPTIONS options);
gint32 dal_set_property_value_int16( OBJ_HANDLE obj_handle, const gchar* property_name, gint16 in_prop_val, DF_OPTIONS options);
gint32 dal_set_property_value_uint16( OBJ_HANDLE obj_handle, const gchar* property_name, guint16 in_prop_val, DF_OPTIONS options);
gint32 dal_set_property_value_int32( OBJ_HANDLE obj_handle, const gchar* property_name, gint32 in_prop_val, DF_OPTIONS options);
gint32 dal_set_property_value_uint32( OBJ_HANDLE obj_handle, const gchar* property_name, guint32 in_prop_val, DF_OPTIONS options);
gint32 dal_set_property_value_int64(OBJ_HANDLE obj_handle, const gchar* property_name, gint64 in_prop_val, DF_OPTIONS options);
gint32 dal_set_property_value_uint64(OBJ_HANDLE obj_handle, const gchar* property_name, guint64 in_prop_val, DF_OPTIONS options);
gint32 dal_set_property_value_double( OBJ_HANDLE obj_handle, const gchar* property_name, gdouble in_prop_val, DF_OPTIONS options);
gint32 dal_set_property_value_string(OBJ_HANDLE obj_handle, const gchar* property_name, const gchar* in_prop_val, DF_OPTIONS options);
gint32 dal_pmbus_chip_byteread(OBJ_HANDLE psu_chip_obj, guchar cmd, guint8* data);
gint32 dal_pmbus_chip_wordread(OBJ_HANDLE psu_chip_obj, guchar cmd, guint16* data);
gint32 dal_pmbus_chip_blkread(OBJ_HANDLE psu_chip_obj, guchar cmd, guchar* data, gint32 data_len);
gint32 dal_pmbus_chip_custom_read(OBJ_HANDLE psu_chip_obj, guchar cmd, guchar* data, gint32 data_len);
gint32 dal_pmbus_chip_bytewrite(OBJ_HANDLE psu_chip_obj, guchar cmd, guint8* data);
gint32 dal_pmbus_chip_wordwrite(OBJ_HANDLE psu_chip_obj, guchar cmd, guint16* data);
gint32 dal_pmbus_chip_blkwrite(OBJ_HANDLE psu_chip_obj, guchar cmd, guint8* data, gint32 data_len);
gint32 dal_pmbus_chip_custom_write(OBJ_HANDLE psu_chip_obj, guchar cmd, guint8* data, gint32 data_len);
gint32 dal_get_custom_string_name(gchar *custom_name, gchar *property_name, gint16 len);
gint32 dal_cmd_custom_specail_string(struct helpinfo *cmd_list, gint16 cmd_num);
gint32 dal_get_custom_ibma_ibmc_string(gchar *custom_ibmc_name, gint16 custom_ibmc_len,
    gchar *custom_ibma_name, gint16 custom_ibma_len);
gint32 write_i2c_val(guchar bus_id, guchar* tx_data, guint32 tx_len);
guchar dal_cal_crc8(guint8 crc, const guint8* buf, const gint32 len);
gint32 dal_cal_crc16(guint16 init_val, const guint8 *p_data, guint16 length, guint16 *crc);
gint32 dal_cal_crc32(guint8 *msg_ptr, guint32 byte_cnt, guint32 old_chk_sum, gint8 init, gint8 last);
gint32 dal_check_crc8(guchar checked_crc, const guchar* buf, const gint32 len);
gint32 dal_verify_respdata_crc32(guint8 *pData, guint32 data_len);
gint32 dal_read_byte_by_pmbus(guchar busid, guchar slave_addr, guchar cmd, guint8* data);
gint32 dal_read_word_by_pmbus(guchar busid, guchar slave_addr, guchar cmd, guint16* data);
gint32 dal_read_block_by_pmbus(guchar busid, guchar slave_addr, guchar cmd, guchar* data, gint32 data_len);
gint32 dal_write_byte_by_pmbus(guchar busid, guchar slave_addr, guchar cmd,guint8* data);
gint32 dal_write_word_by_pmbus(guchar busid, guchar slave_addr, guchar cmd,guint16* data);
gint32 dal_write_block_by_pmbus(guchar busid, guchar slave_addr, guchar cmd,guchar* data, gint32 data_len);
gint32 dal_set_hi_51_ctrl_state(guchar state);
gint32 dal_block_read(const gchar* accessor_name, gchar* info, guint32 info_size);
gint32 dal_get_present_ps_num(gint32* present_ps_num);
gint32 dal_get_product_version_num(guint32 *val);
gint32 dal_get_product_id(guint32 *val);
gboolean dal_match_product_id(guint32 id);
gboolean dal_match_software_type(guint8 type);
gboolean dal_match_board_id(guint32 productId, guint16 boardId);
gboolean dal_match_backplane_type(guint32 productId, guint32 backplaneType);
guint8 dal_get_slotid(void);
gint32 dal_get_storage_node_slotid(void);
gint32 dal_eth_get_out_type_object(guchar type, OBJ_HANDLE* obj);
guint8 dal_eth_get_out_type_groupid(void);
guint8 dal_eth_get_second_out_type_groupid(void);
guint8 dal_get_ncsi_out_type_groupid(gint32 net_type);
gint32 dal_eth_get_inner_object(guint8 ipmb_addr, OBJ_HANDLE *obj);
gint32 dal_eth_get_inner_network(guint8 ipmb_addr, guint32 *ip);
guint8 dal_eth_get_cur_plane_eth_num(void);
guint8 dal_get_uid_location(void);
gint32 dal_get_x86_platform_version_num(guint32* val);
/* 支持PECI获取PCIe设备信息 */
gint32 match_type_with_product_id(guint32 id);
gint32 dal_psmi_i2c_read( guchar busid, guchar slave_addr, guchar cmd, guchar* out_buf, gint32 out_buf_len);
void dal_clear_sensitive_info(void* buf, guint32 len); //DTS2015112600310
gint32 dal_write_i2c_val(guchar bus_id, guchar* tx_data, guint32 tx_len);
gint32 dal_read_i2c_val(guchar bus_id, guchar* tx_data, guint32 tx_len, guchar *output, guint32 out_size);
gint32 dal_i2c_ch_init(guchar bus_id,  guchar addr);
void dal_trim_string(gchar* str, gint32 str_size);
void dal_convert_string_to_hex(guchar* input, guint32 input_size, guchar* output, guint8 output_size);
void dal_convert_hex_to_string(guchar* input, guchar* output, guint32 output_len);
void dal_insert_file_end(FM_FILE_S* stream, gint32 ch);
gint32 dal_check_complexity_status(const gchar* pw);
const gchar* dal_str_has_casesuffix(const gchar* str, const gchar* suffix);

/* Added by f00225994, 2016/4/14   PN:AR-0000245341-005-006 支持口令安全策略 */
gint32 dal_check_A_differs_from_B(const gchar* stringA, guint32 strA_len, gchar* stringB, guint32 strB_len);

gint32 dal_get_file_count_by_keyword(gchar* path, gchar* keyword, guint32* count);
void dal_proxy_operation_log(const gchar* caller_interface, const gchar* username, const gchar* client,
                                    const gchar* target, const gchar* fmt, ...);
gint32 dal_get_property_type(OBJ_HANDLE obj_handle, const gchar* property_name, gchar* out_type);
gint32 dal_get_property_value_byte_by_name(gchar *object_name, gchar *property_name, guint8 *property_value);
gint32 dal_get_property_value_uint32_by_name(gchar *object_name, gchar *property_name, guint32 *property_value);
gint32 dal_get_property_value_all_type(OBJ_HANDLE obj_handle, const gchar *property_name, gchar *buf, guint32 buf_len);
void dal_install_sig_action(void);
gint32 dal_get_extern_value_byte(OBJ_HANDLE obj_handle, const gchar* property_name, guchar* out_val, guint32 option);
gint32 dal_get_extern_value_int16(OBJ_HANDLE obj_handle, const gchar* property_name, gint16* out_val, guint32 option);
gint32 dal_get_extern_value_uint16(OBJ_HANDLE obj_handle, const gchar* property_name, guint16* out_val, guint32 option);
gint32 dal_get_extern_value_int32(OBJ_HANDLE obj_handle, const gchar* property_name, gint32* out_val, guint32 option);
gint32 dal_get_extern_value_uint32(OBJ_HANDLE obj_handle, const gchar* property_name, guint32* out_val, guint32 option);

gint32 dal_get_saved_value_byte(OBJ_HANDLE obj_handle, const gchar* property_name, guchar* out_val,guint32 option);
gint32 dal_get_saved_value_uint16(OBJ_HANDLE obj_handle, const gchar* property_name, guint16* out_val,guint32 option);
gint32 dal_get_saved_value_string(OBJ_HANDLE obj, const gchar* prop_name,
    gchar *out_buff, gsize buff_len, DF_OPTIONS df_opt);
gint32 dal_reg_ex(gchar* i_src, const gchar* i_reg_ex, guint32 i_split_num, gchar o_dst[][DAL_RE_SPLIT_MAX_LEN]);
gint32 dal_get_cert_info(const gchar* file_path, CERT_INFO_T* pcert_issue, CERT_INFO_T* pcert_subject);
gint32 dal_get_cert_info_from_x509(X509 *x509, CERT_INFO_T *pcert_issue, CERT_INFO_T *pcert_subject);
gint32 dal_get_cert_info_mem(const gchar *plaintext, gsize plaintext_len, CERT_INFO_T *pcert_issue, CERT_INFO_T *pcert_subject);
gint32 dal_check_cert_basic_constraints(X509 *x, gint32 is_ca);
gint32 dal_check_x509_ext_valid(X509 *x);
gint32 dal_check_crt_file_ext_valid(const gchar *crt_file);
gint32 dal_check_real_path(const gchar* file_path);
gint32 dal_check_real_path2(const gchar* file_path, const gchar *path_header);

gint32 dal_verify_cert_info(const gchar* path, const gchar* key_file);
gint32 dal_verify_cert_info_from_mem(const gchar *cert_text, gsize cert_size, const gchar *key_text, gsize key_size);
gint32 dal_resolve_cert_into_pem(const gchar *p_wd, const gchar *src_crt, const gchar *dst_pem,
    gboolean with_private_key);
gint32 dal_get_addr_type(const gchar* host_addr, guchar* type);
gint32 dal_check_specific_channel_serial_loopback(guint8 uart_chan, guint8 uart_port, guint8 backup_chan);
gint32 dal_check_serial_loopback(void);
xmlDocPtr dal_open_xml_doc(const gchar *filename);
xmlXPathObjectPtr dal_parse_xml_file(xmlDocPtr doc, xmlChar* xpath);
void dal_get_xml_node_content(xmlXPathObjectPtr xpath_object, gchar* string, guint32 string_len, guint32 nth);
void dal_free_xml_parser_object(xmlDocPtr doc, xmlXPathObjectPtr xpath_object);
void dal_cleanup_xml_parser(void);

/* Modified by h00422363, 2018/7/2 11:8:11   问题单号:DTS2018070310550 中端FPGA卡告警事件以故障码显示时，没有结束符，因此优化告警描述，添加结束符处理 */
/* Added for PN:DTS2016112301508 by wwx383734, 2016/11/23 */
void dal_formated_str(gchar* xml_content, GSList* list, gchar** buf, gchar end_char);
void dal_format_alarm_custom_name(gchar *xml_content, size_t content_len, gchar **buf);

void dal_convert_raw_to_formated(xmlDocPtr doc,
                                 guint8* raw_data,
                                 guint32 data_size,
                                 guint8 split,
                                 const gchar* lang,
                                 const gchar* inode,
                                 gchar** buf,
                                 gchar* xml_template,
                                 const gchar* serial_num,
                                 const gchar* part_num);

/* 支持BOM编码带出 */
void dal_append_bom_sn(const gchar* silk_name, gchar* xml_content, guint32 content_len);
/* 查询时间优化及部件拔出不带出SN-bom */
void dal_get_bom_sn_by_monitor(const gchar* monitor_name, gchar* part_num, guint32 part_num_len, gchar* serial_num, guint32 serial_num_len);
void dal_strcat_bom_sn(const gchar* serial_num, const gchar* part_num, gchar* xml_content, guint32 content_len);
gint32 dal_certinfo_time_to_second (const gchar* cert_time, gulong* sencond_time);
void dal_get_event_trigger_mode_string(guint8 trigger_mode, gchar* str_buf, guint32 buf_len);
const gchar* dal_get_subject_type_string(guint32 subject_type);
void dal_get_event_desc_prefix(gint32 trigmode, const gchar* eventsubject, gint32 eventcode, gchar* trigmode_str, guint32 trigmode_str_len);
void dal_get_event_trigger_mode_string(guint8 trigger_mode, gchar * str_buf, guint32 buf_len);
gint32 dal_check_power_type(OBJ_HANDLE obj_handle,gint32* power_type);
DBD_MIDAVG1_S* dal_dbd_create_debounce_cfg(gint8 default_val);
void dal_dbd_delete_debounce_cfg(DBD_MIDAVG1_S* dbd_cfg);
gint32 dal_dbd_get_debounced_value(DBD_MIDAVG1_S* dbd_cfg, gint8 new_val, gint8* dbd_val);
void dal_get_pd_id_by_slot_and_enclosure(const gchar* ctrl_obj_name, guint8 slot, guint16 enclosure_id, guint8* id);
gboolean dal_check_ipmi_req_huawei_iana(const guint8 iana[], size_t buf_len);
guint8 dal_set_ipmi_resp_huawei_iana(guint8 iana[], size_t buf_len);
void dal_check_obj_deleted_status(gchar* class_name, gchar* obj_prefix);
void dal_check_obj_added_status(gchar* class_name, gchar* obj_prefix, guint32 list_len);
gint32 dal_check_string_is_valid_ascii(const gchar* str);
gint32 dal_check_valid_ascii(const gchar *str, guint32 str_len);
void dal_fdm_output_fomart_log(FDM_LOG_TYPE log_type, FDM_LOG_LEVEL log_level, const gchar* log_fmt, ...);
guint32 dal_time_to_string(guint32 d_time, gchar* buffer, guint32 max_len);
void dal_cache_property_string(json_object* resource_jso, const gchar* redfish_prop, gchar* dest, guint32 destlen);
void dal_update_property_string(json_object *resource_jso, const gchar *redfish_prop, OBJ_HANDLE pme_handle,
    const gchar *pme_prop);
void dal_cache_property_long(json_object *resource_jso, const gchar *redfish_prop, guint64 *dest);
void dal_cache_property_uint32(json_object* resource_jso, gchar* redfish_prop, guint32* dest);
void dal_update_property_uint32(json_object* resource_jso, gchar* redfish_prop, OBJ_HANDLE pme_handle, gchar* pme_prop);
void dal_cache_property_uint16(json_object* resource_jso, gchar* redfish_prop, guint16* dest);
void dal_update_property_uint16(json_object* resource_jso, gchar* redfish_prop, OBJ_HANDLE pme_handle, gchar* pme_prop);
void dal_cache_property_uint32_8f(json_object* resource_jso, gchar* redfish_prop, guint32* dest);
void dal_update_property_uint32_8f(json_object* resource_jso, gchar* redfish_prop, OBJ_HANDLE pme_handle, gchar* pme_prop);
void dal_cache_property_byte(json_object* resource_jso, gchar* redfish_prop, guint8* dest);
void dal_update_property_byte(json_object* resource_jso, gchar* redfish_prop, OBJ_HANDLE pme_handle, gchar* pme_prop);
void dal_cache_property_double(json_object* resource_jso, gchar* redfish_prop, gdouble* dest);
void dal_update_property_double(json_object* resource_jso, gchar* redfish_prop, OBJ_HANDLE pme_handle, gchar* pme_prop);
gint32 dal_parser_string_from_url(gchar *url_string, guint32 url_len, URL_STRUCT_INFO_S *url_info);
void dal_remove_space_in_string(gchar* string_src, gint32 buf_len);
guint8 dal_get_pd_slots_and_enclosure_by_ids(const gchar* ctrl_obj_name, const guint8* ids, guint8* slots, guint16* enclosures, guint8 count);
DBD_MIDAVG1_S* dal_dbd_create_debounce_cfg(gint8 default_val);
void dal_dbd_delete_debounce_cfg(DBD_MIDAVG1_S* dbd_cfg);
gint32 dal_dbd_get_debounced_value(DBD_MIDAVG1_S* dbd_cfg, gint8 new_val, gint8* dbd_val);
gint32 dal_get_bmc_reset_type(gint32* reset_type);
void dal_set_curlopt_peer_ssl_verify(CURL *pCurl, gboolean verify_peer_ssl, guint8 cert_id);
gint32 dal_send_http_msg_with_multi_certs(CURL_REQ_INFO *req_info, CURL_RESPONSE_INFO **resp_info,
    CURL_OPT_INFO *option);
gint32 dal_send_http_msg_with_curl(CURL_REQ_INFO *req_info, CURL_RESPONSE_INFO **resp_info, gchar *eth,
    gboolean verify_peer_ssl, guint8 cert_id);
gint32 dal_send_multi_http_msgs_with_curl(const CURL_REQ_INFO* req_infos, guint32 count, const gchar* eth, CURL_RESPONSE_INFO** resp_infos);
gint32 dal_get_func_ability(const gchar* class_name, const gchar* property_name, guchar* function_Enable);
void dal_free_curl_request_info(CURL_REQ_INFO** req_info);
void dal_free_curl_multi_request_infos(CURL_REQ_INFO** req_infos, guint32 count);
void dal_free_curl_response_info(CURL_RESPONSE_INFO** resp_info);
void dal_free_curl_multi_response_infos(CURL_RESPONSE_INFO** resp_infos, guint32 count);
gint32 dal_dynamic_add_array_property(OBJECT_ITEM object_item, const gchar* prop_name,
                const gchar* array_type, guint32 arraylen, gchar prop_val[MAX_DESC_PARA_NUM][PROP_VAL_LENGTH + 1]);
gint32 dal_dynamic_create_monitor_object_item(OBJECT_FILE obj_file, EVENT_MONITOR_INFO* monitor_info);
gint32 dal_rf_rsc_evt_log(RSC_EVT_TYPE rf_evt_type, gboolean is_ignore_ssl_verify, const gchar *origin_rsc_fmt, ...);
gint32 dal_get_evt_detail_customized_id(guint8 *custom_id);
gboolean dal_is_customized_by_cmcc(void);
gboolean dal_is_customized_by_cucc(void);
gint32 dal_get_custom_brand_flag(guint8 *custom_brand_flag);
gboolean dal_is_customized_machine(void);
gint32 dal_post_redfish_evt_msg(RF_EVT_MSG_INFO* evt_msg_info);
gint32 dal_get_server_identity(guint8 trap_identity, gchar *str_buf, gsize buf_len);
gint32 dal_get_server_location(gchar * str_buf, gsize buf_len);
gint32 dal_get_server_type(guchar* server_type);
gint32 dal_get_software_type(guchar *software_type);
gint32 dal_get_redfish_datetime_stamp(gulong d_time, gchar *buffer, guint32 max_len);
gboolean dal_is_leap_year(guint16 year);
gint32 dal_check_date_time(guint16 year, guint8 month, guint8 date, guint8 hour, guint8 minute, guint8 second);
gint32 dal_parse_redfish_datetime_to_stamp(const gchar* datetime, gulong* timestamp);
gint32 dal_parse_manufacture_date_to_stamp(const gchar *datetime, gulong *timestamp);
gint32 dal_get_file_list_at_path_by_ext(const gchar* db_dir_path, gchar* file_ext, GSList** p_db_name_list);
guint8 dal_get_raid_mgnt_support_status(void);
gint32 dal_rf_get_board_type(guchar* board_type);
gint32 dal_rf_get_board_slot(gchar* slot_string, gint32 slot_len);
gint32 dal_check_transfer_protocol_supported(const gchar* transfer_uri);
guint32 dal_url_code_convert(gchar* dst_ptr, guint32 dst_len, const gchar* src_ptr, guint32 src_len, guint32 obj_type);
void dal_init_openssl(void);
gint32 dal_pfx2pem_mem(const gchar *src_file, gchar *plaintext, gsize plaintext_size, gsize *plaintext_len);
void dal_init_curl(void);
gulong dal_get_self_thread_id(void);
gint32 dal_check_file_size_exceeded(const gchar* filepath, guint32 limited_size);
gint32 dal_get_aes_padding(guint8* padding);
gint32 dal_check_shell_special_character(const gchar *cmd_str);
gint32 dal_get_tmpfs_statfs(struct statfs* p_tmp_fs, gchar* path);
gint32 dal_parse_domain_name_to_ip(const gchar* domain_name, gchar* ip_addr, guint32 buf_sz, gint32* ipType);
/* 检查网络地址的合法性，包括IPv4、IPv6和域名的检查 */
gint32 dal_check_net_addr_valid(const gchar* net_addr, guint8* addr_type);
gint32 dal_get_localtime_offset(gint16 *time_offset);
gint32 dal_localtime_r(const time_t* p_time_stamp, struct tm* p_tm);
GVariant* dal_tlv_to_gvariant(const void* tlv_raw_data, const GVariantType* type, gint32 iformat);
gint32 dal_gvariant_to_tlv(GVariant* gvar_data, guint8* mem_buf, guint16 buf_len, guint8 iformat);
gint32 dal_get_system_guid_string(gchar* guid_data,guint32 guid_len);
/* 将GMT时间(0时区，不考虑夏令时)转化为时间戳 */
gint32 dal_gmt_to_timestamp(struct tm gmt, gulong* time_stamp);
time_t dal_tick_get_seconds(void);
gint32 dal_set_property_value_array_byte(OBJ_HANDLE obj_handle, const gchar* property_name,
       const guchar* elements, guint32 n_elements, DF_OPTIONS options);
gint32 dal_set_property_value_array_uint16(OBJ_HANDLE obj_handle, const gchar* property_name,
       const guint16* elements, guint32 n_elements, DF_OPTIONS options);
gint32 dal_set_property_value_array_uint32(OBJ_HANDLE obj_handle, const gchar* property_name,
       const guint32* elements, guint32 n_elements, DF_OPTIONS options);
gint32 dal_set_property_value_array_string(OBJ_HANDLE obj_handle, const gchar* property_name,
        const gchar* const*  strv, gssize  length, DF_OPTIONS options);
gint32 dal_set_property_value_array_double(OBJ_HANDLE obj_handle, const gchar *property_name,
        const gdouble *elements, guint32 n_elements, DF_OPTIONS options);

gint32 dal_move_file(const gchar *dest_path, const gchar *src_path);

gint32 dal_copy_file( const gchar* dest_path, const gchar* src_path );
gpointer gv_ref_2_copy(gconstpointer src, gpointer data);
gint32 dal_file_lock(const gchar* file_name, gboolean is_created);
void dal_file_unlock(gint32 handle);
gint32 dal_base64_encode(guchar *input, gint32 input_length, gchar *output, gint32 output_length);
gint32 dal_get_random_string(gchar *buffer, gint32 buffer_length, gint32 length);
// 201705108246 需求支持PARNUM查询, 20190723 RAOJIANZHONG 00267466 START
gint32 dal_get_card_bom(OBJ_HANDLE comp_handle, guint8 dev_type, gchar* part_num, guint32 part_num_len);
gint32 dal_get_card_sn(OBJ_HANDLE comp_handle, gchar * serial_num, guint32 serial_num_len);
guint8 dal_class_to_devicetype(const gchar * classname);
gchar * _dal_devicetype_to_class(guint8 dev_type);
// 201705108246 需求支持PARNUM查询, 20190723 RAOJIANZHONG 00267466 END
gint32 dal_set_json_array_to_obj_handle(OBJ_HANDLE obj_handle, const gchar* prop, json_object* arr_jso, DF_OPTIONS options);
gint32 dal_get_byte_array_with_separator(OBJ_HANDLE obj_handle, const gchar* property_name,
       gchar* separator, gchar* out_val, guint32 out_len);
gint32 check_input_smart_mode_valid(guchar input_mode);
guint8 dal_get_position_level(guint32 position);

/* 刷新FRU的Replace时间，掉电持久化 */
gint32 dal_refresh_replace_time(OBJ_HANDLE obj_handle, const gchar* property_name);

/* Modified by zhangxiong z00454788, 2019/3/30   问题单号:DTS2019032006859 挪动标签的位置，导致功耗封顶界面无法获取到标签对应服务器的信息 */
gint32 dal_find_valid_devive_obj(const gchar * serial_num, OBJ_HANDLE* obj_handle);

/*
 * Description: 基于PME框架文件管理机制的文件拷贝
 */
gint32 dal_fm_file_copy(const gchar *dst_short_name, const gchar *src_file_path);

/*****************************************************************************
 函 数 名: dal_modify_config_file
 功能描述  : 1、修改配置文件中某一条目的内容，形如XXX XXXXXXX的格式
          2、待修改的配置文件必须先通过user_fm_init注册
 输入参数: const gchar *file_name 不包含路径的文件名称
          const gchar* item 待修改的条目
          const gchar* content 待修改的内容
 输出参数: 无

*****************************************************************************/
gint32 dal_modify_config_file(const gchar *file_name, const gchar* item, const gchar* content);

/*****************************************************************************
 函 数 名: dal_split_algorithm_prop
 功能描述  : 1、分割算法属性，将算法名称和对应的使能状态分割
          2、ssh加密算法和ssl加密套件共用
 输入参数: const gchar* algorithm_prop 原始属性值
          size_t length 目标字符串的最大长度
 输出参数: gchar* algorithm_str 分割后的目标字符串
           gboolean* enable_status 该算法的使能状
*****************************************************************************/
gint32 dal_split_algorithm_prop(const gchar* algorithm_prop, gchar* algorithm_str, size_t length, gboolean* enable_status);

/*****************************************************************************
 函 数 名: dal_get_property_value_strv
 功能描述  : 获取字符串数组类型（as）的属性值。需由调用函数通过g_strfreev释放str_vector！！！
 输入参数:OBJ_HANDLE obj_handle
          const gchar* property_name
 输出参数: gchar*** str_vector
          gsize* str_vector_len

*****************************************************************************/
gint32 dal_get_property_value_strv(OBJ_HANDLE obj_handle, const gchar* property_name, gchar*** str_vector, gsize* str_vector_len);

/*****************************************************************************
 功能描述  : 获取数组类型（ay）的属性值，o_element_count为实际的元素数量，为NULL时表示不关注元素数量
  1.日    期   : 2019年11月14日
    作    者   : zwx382755
    修改内容   : UADP451986、UADP451985、UADP451983、UADP451984、UADP451982、UADP466485、UADP466484 新增
*****************************************************************************/
gint32 dal_get_property_value_bytev(OBJ_HANDLE obj_handle, const gchar* property_name,
    guint8 *buffer, guint32 buffer_size, gsize *o_element_count);

/*****************************************************************************
 功能描述  : 获取数组类型（aq）的属性值，o_element_count为实际的元素数量，为NULL时表示不关注元素数量
  1.日    期   : 2019年11月14日
    作    者   : zwx382755
    修改内容   : UADP451986、UADP451985、UADP451983、UADP451984、UADP451982、UADP466485、UADP466484 新增
*****************************************************************************/
gint32 dal_get_property_value_uint16v(OBJ_HANDLE obj_handle, const gchar* property_name,
    guint16 *buffer, guint32 buffer_size, gsize *o_element_count);

gint32 dal_get_property_value_doublev(OBJ_HANDLE obj_handle, const gchar *property_name,
    gdouble *buffer_q, guint32 buffer_size, gsize *o_element_count);
/*****************************************************************************
 函 数 名: dal_save_property_value_byte_remote
 功能描述  : APP模块降权限进程使用！！！设置并保存byte类型属性值
           DF_OPTIONS options取值仅限：DF_SAVE、DF_SAVE_PERMANENT、DF_SAVE_TEMPORARY
 输入参数: OBJ_HANDLE obj_handle 对象句柄
           const gchar* property_name  对象属性名
           guchar in_prop_val 对象的属性值 byte类型
           DF_OPTIONS options 持久化选项
 输出参数: 无
*****************************************************************************/
gint32 dal_save_property_value_byte_remote(OBJ_HANDLE obj_handle, const gchar* property_name,
                                 guchar in_prop_val, DF_OPTIONS options);

/*****************************************************************************
 函 数 名: dal_save_property_value_uint32_remote
 功能描述  : APP模块降权限进程使用！！！设置并保存uint32类型属性值
           DF_OPTIONS options取值仅限：DF_SAVE、DF_SAVE_PERMANENT、DF_SAVE_TEMPORARY
 输入参数: OBJ_HANDLE obj_handle 对象句柄
           const gchar* property_name  对象属性名
           guint32 in_prop_val 对象的属性值 uint32类型
           DF_OPTIONS options 持久化选项
 输出参数: 无
*****************************************************************************/
gint32 dal_save_property_value_uint32_remote(OBJ_HANDLE obj_handle, const gchar* property_name,
                                   guint32 in_prop_val, DF_OPTIONS options);

/*****************************************************************************
 函 数 名: dal_save_property_value_int32_remote
 功能描述  : APP模块降权限进程使用！！！设置并保存int32类型属性值
           DF_OPTIONS options取值仅限：DF_SAVE、DF_SAVE_PERMANENT、DF_SAVE_TEMPORARY
 输入参数: OBJ_HANDLE obj_handle 对象句柄
           const gchar* property_name  对象属性名
           gint32 in_prop_val 对象的属性值 int32类型
           DF_OPTIONS options 持久化选项
 输出参数: 无
*****************************************************************************/
gint32 dal_save_property_value_int32_remote(OBJ_HANDLE obj_handle, const gchar* property_name,
                                   gint32 in_prop_val, DF_OPTIONS options);

/*****************************************************************************
 函 数 名: dal_save_property_value_string_remote
 功能描述  : APP模块降权限进程使用！！！设置并保存string类型属性值
           DF_OPTIONS options取值仅限：DF_SAVE、DF_SAVE_PERMANENT、DF_SAVE_TEMPORARY
输入参数: OBJ_HANDLE obj_handle 对象句柄
          const gchar* property_name  对象属性名
          const gchar* in_prop_val 对象的属性值 字符串类型的指针
          DF_OPTIONS options 持久化选项
输出参数: 无
*****************************************************************************/
gint32 dal_save_property_value_string_remote(OBJ_HANDLE obj_handle, const gchar* property_name,
                                   const gchar* in_prop_val, DF_OPTIONS options);

/*****************************************************************************
 函 数 名: dal_save_property_value_uint16_remote
 功能描述  : APP模块降权限进程使用！！！设置并保存uint16类型属性值
           DF_OPTIONS options取值仅限：DF_SAVE、DF_SAVE_PERMANENT、DF_SAVE_TEMPORARY
输入参数: OBJ_HANDLE obj_handle 对象句柄
          const gchar* property_name  对象属性名
          guint16 in_prop_val 对象的属性值 uint16类型
          DF_OPTIONS options 持久化选项
输出参数: 无
*****************************************************************************/
gint32 dal_save_property_value_uint16_remote(OBJ_HANDLE obj_handle, const gchar* property_name,
                                    const guint16 in_prop_val, DF_OPTIONS options);

gint32 dal_save_property_value_array_string_remote(OBJ_HANDLE obj_handle, const gchar* property_name,
                                    const gchar* const*	strv, gssize  length, DF_OPTIONS options);

gint32 dal_save_property_value_array_byte_remote(OBJ_HANDLE obj_handle, const gchar* property_name,
                                    const guchar* elements, guint32 n_elements, DF_OPTIONS options);

gint32 dal_save_property_value_gvariant_remote(OBJ_HANDLE obj_handle, const gchar* property_name,
                                    GVariant* value, DF_OPTIONS options);

gint32 dal_save_property_value_byte_remote_v2(OBJ_HANDLE obj_handle, const gchar* property_name,
                                   const guchar in_prop_val, DF_OPTIONS options);

gint32 dal_save_property_value_uint16_remote_v2(OBJ_HANDLE obj_handle, const gchar* property_name,
                                   const guint16 in_prop_val, DF_OPTIONS options);

/*
 Description: APP降权限使用，带重试
 Note: 设置持久化属性时，APP可能正好从root权限降为非root权限，导致设置失败，因此增加重试。
 */
gint32 dal_save_property_value_uint32_remote_with_retry(OBJ_HANDLE obj_handle, const gchar * property_name,
    guint32 in_prop_val, DF_OPTIONS options, guint8 max_retry_num);

/* added by h00256973  2019/06/25  for 1711 BMC */
#ifdef ARM64_HI1711_ENABLED
gint32 dal_get_bmc_last_upgrde_info(guint16 *p_status);
gint32 dal_get_bmc_secure_boot_info(SECURE_BOOT_INFO_S *p_secure_boot_info);
gint32 dal_set_bmc_upgrade_flag(guint8 upgrade_flag);
gint32 dal_verify_partner_h2p_info(void);
gint32 dal_get_efuse_state_info(guint8 *efuse_state);
gint32 dal_set_customer_info(void);
gint32 dal_get_bios_mux_status(guint16 *p_status);
gint32 dal_set_bios_mux_ctrl(guint8 bios_ctrl);
gint32 dal_get_bios_verify_result(guint16 *p_result);
gint32 dal_set_bios_verify_ctrl(guint8 bios_type);
gint32 dal_send_recv_msg_with_m3(const guint8 *req_msg, guint16 req_msg_len, guint8 *resp_msg, guint16 *p_resp_msg_len);
gint32 dal_kmc_update_hw_rootkey(void);
gint32 dal_kmc_master_key_encode(const guint8 *plaintext, guint32 plainLen, guint8 *ciphertext, guint32 *cipherLen);
gint32 dal_kmc_master_key_decode(const guint8 *ciphertext, guint32 cipherLen, guint8 *plaintext, guint32 *plainLen);
#endif
void dal_replace_caller_interface_redfishweb_to_web(GSList* caller_info, gchar *interface, guint32 length);
void dal_replace_caller_user_name(GSList* caller_info, const gchar *username);
guint8 dal_get_call_method_type(GSList* caller_info_in, GSList** caller_info_out);
gint32 dal_get_port_silknum(guchar card_type, guchar eth_num, guchar port_num, guchar* silk_num);
gdouble dal_get_time_interval(clock_t time_begin, clock_t time_end);
gint32 dal_compare_psu_slot_id(gconstpointer a,  gconstpointer b);
guint8 dal_get_leakdetect_support(void);
gint32 dal_check_comp_result_by_operator(guint32 actual_value, guint32 expect_value, guint8 op);
gboolean dal_is_rpc_exception(gint32 rpc_result);
gint32 dal_check_upgrade_is_in_progress(void);
guint8 dal_get_dft_enable(void);
gint32 dal_open_CRL_file(const gchar *file_path, FILE **fp, X509_CRL **x);
void dal_close_CRL_file(FILE* fp, X509_CRL* crl);
gint32 dal_verify_CRL_issuer(X509_CRL *crl, X509 *issuer);
gint32 dal_get_CRL_revoked_info(X509_CRL *crl, STACK_OF(X509_REVOKED) * *revoked_stack);
gint32 dal_get_asn_date(const ASN1_TIME *asn1_tm, gchar *date, guint32 date_len);
gint32 dal_get_file_path_list(const gchar *dir_name, GSList **file_names);
gint32 dal_open_x509_file(const gchar *path, FILE **fp, X509 **x);
void dal_close_x509_file(FILE *fp, X509 *x);
gint32 dal_get_CRL_info(X509_CRL *x, CRL_INFO_T *crl_info);
gint32 dal_verify_CRL_issuing_date(const gchar *old_started_time, const gchar *new_started_time);
gint32 dal_get_bmc_chip_platform_type(void);
gint32 dal_encrypt_data(const gchar *obj_name, const gchar *plaintext, gchar **ct, gsize ct_max_len, gsize *ct_len);
gint32 dal_encrypt_data_local(const gchar *obj_name, const gchar *component_property, const gchar *workkey_property, const gchar *plaintext, gchar **ct, gsize ct_max_len, gsize *ct_len);
gint32 dal_decrypt_data(const gchar *obj_name, const gchar *component_property, const gchar *ciphertext, gchar **plaintext, gsize pt_max_len, gsize *pt_len);
gboolean dal_check_mk_update(OBJ_HANDLE obj_handle, const gchar *key_id_prop, guint32 new_key_id);
gboolean dal_check_if_masterkey_id_set(OBJ_HANDLE obj_handle, const gchar* mk_property);
gboolean dal_check_if_mm_board(void);
gboolean dal_check_if_blade_server_board(void);
gboolean dal_check_if_vsmm_supported(void);
guint8 dal_get_master_vsmm_slot_id(void);
guint8 dal_get_vsmm_ms_status(void);
gboolean dal_is_decimal_numeric_str(const gchar* str, NUMERIC_TYPE_E type);
gint32 dal_get_bmc_service_ipaddr(guchar group_type, gchar *ipv4, guint32 ipv4_len, gchar *ipv6, guint32 ipv6_len);
gint32 dal_get_inband_net_service_ipaddr(gchar *ipv4, guint32 ipv4_len, gchar *ipv6, guint32 ipv6_len);
gint32 dal_get_multi_bmc_service_ipv4addr(guchar group_type, gchar* ipv4, guint32 ipv4_total_len, guchar* ipv4_num);
gint32 dal_get_multi_bmc_service_ipv6addr(guchar group_type, gchar* ipv6, guint32 ipv6_total_len, guchar* ipv6_num);
gint32 dal_check_valid_key(const gchar* key);
guint8 get_loopback_confirmed_flag(guint8 max_try_times);
guint8 dal_get_ft_mode_flag(void);
gboolean is_dft_enable_status(void);
gint32 dal_read_file(const gchar *filename, void *buf, size_t size);
gint32 dal_write_buf_to_tmp_file(guint8 *buf, guint32 buf_len, const gchar *filename);
gint32 dal_write_file(const gchar *file_name, void *buf, size_t size);
gint32 dal_get_uid_and_gid(const gchar *user_name, guint32 *uid, guint32 *gid);
gboolean dal_check_file_owner(const gchar *user_name, const gchar *file_path, gboolean is_local_user);
gboolean dal_check_file_opt_user(guchar auth_type, const gchar* user_name, const gchar* file_path, const gchar* user_role_id);
gint32 dal_get_user_role_from_provider(const gchar *user_role_id, guchar *user_role);
gint32 dal_set_file_owner(const gchar* file_path, guint32 uid, guint32 gid);
gint32 dal_set_file_mode(const gchar* file_path, guint32 file_mode);
gint32 dal_delete_file(const gchar* file_path);
gint32 dal_change_file_owner(guchar auth_type, const gchar* file_path, const gchar* user_name, guint32 file_mode);
guint8 get_retimer_status_ready(OBJ_HANDLE retimer_obj);
gint32 dal_retimer_wait_5902_chip_idle(OBJ_HANDLE retimer, gint8 retry_count);
gint32 dal_retimer_read_5902_i2c_data(RETIMER_DRV_INTF_S *read_data);
void dal_format_device_sn(const gchar* device_sn_str, gchar* dst_str, guint32 dst_len);
/* 判断group_obj待设置的ip和掩码与BMC上其他网络的ip和掩码是否网段重叠 */
gint32 dal_check_ipv4_addr_duplicate(OBJ_HANDLE group_obj, const gchar *ipv4_addr, const gchar *subnet_mask);
gboolean is_h2m_user_need_reset_password(guint8 user_id);
gboolean is_m2m_user_login_denied(guint8 user_id, gboolean in_white_list);
guint8 dal_str_mul_repeat_div(const gchar* str, guint8 len);
gint32 dal_can_blk_write_read(const gchar* chip_name, gconstpointer input_data, gint32 input_length, gpointer output_data, gint32 output_length);
gint32 PEM_to_X509(const gchar *pem_str, X509 **cert, size_t str_size);
gint32 X509_to_PEM(X509 *cert, gchar **pem_str);
gint32 PEM_to_EVP_PKEY(gchar *pem_str, EVP_PKEY **cert, size_t str_size);
gint32 dal_check_realpath_by_fd(int fd, const gchar *desfilepath, const gchar *realpath_prefix);
void dal_ipmi_log_with_bios_set_check(const void *req_msg, MLOG_LEVEL_E severity, const gchar* field,
                                      const gchar *fmt, ...)__attribute__((format(printf, 4, 5)));
void proxy_record_maintenance_log(gchar severity, const gchar* field, const gchar* msg);
void dal_fetch_and_remove_file_caller(GSList *caller_info, BMC_FILE_CALLER_S *file_caller);
gint32 dal_fileowner_matchs_caller(BMC_FILE_CALLER_S *file_caller, const gchar *filepath);
gint32 dal_overwriteable_by_caller(BMC_FILE_CALLER_S *file_caller, const gchar *filepath, guint8 type);
gint32 dal_change_fileowner_to_caller(BMC_FILE_CALLER_S *file_caller, const gchar *filepath);
void init_http_response_data(HTTP_RESPONSE_DATA_S* response);
void destory_http_response_data(HTTP_RESPONSE_DATA_S* response);
gint32 dal_is_directory(const gchar *path);
gint32 dal_increase_buf_size(gchar** buf, gsize origin_size, gsize new_size);
gint32 isspecial(gint32 c);
gint32 __dal_check_is_special_mem_alarm(const gchar *lang_str);
gint32 dal_get_obj_list_by_position_level(const gchar *class_name, GSList **obj_list, guint32 position,
    guint8 position_level);
gboolean dal_check_kmc_hardware_rootkey_support(void);
gint32 dal_get_language_set(OBJ_HANDLE obj_handle, gboolean filter, gchar *out_val, guint32 out_len);
gint32 dal_set_language_set(OBJ_HANDLE obj_handle, const gchar *in_prop_val, DF_OPTIONS options);
gint32 dal_pattern_check_language_set(OBJ_HANDLE obj_handle, const gchar *in_prop_val);
void X509_get_key_usage_str(X509 *pX509, gchar *pkey_usage, guint32 len);
gint32 dal_file_compare(const gchar *src_path, const gchar *des_path);
gint32 dal_timestamp_unix_to_local(const guint32 unix_stamp, guint32* local_stamp);
gint32 dal_timestamp_local_to_unix(const guint32 local_stamp, guint32* unix_stamp);
gint32 dal_check_cert_validity_time(const gchar* tmp_file_path);
gint32 dal_check_cert_time_x509(const X509 *x509);
gint32 dal_get_sfc_flash_id(SFC_FLASH_ID_S* sfc_id);
guint32 dal_get_manu_id(void);
gboolean dal_check_manu_id(guint32 manu_id, guint32 iana);
gboolean dal_set_ipmi_resp_man_id(guint8 *iana);
gboolean dal_check_ar_card_support(void);
gboolean dal_check_timestamp_timeout(gulong curr_timestamp, gulong last_timestamp, guint64 diff_threshold);
gint32 dal_check_cert_algorithm(const gchar* tmp_file_path);
gint32 dal_common_get_prop_value(OBJ_HANDLE handle, const gchar *name, guint8 type, void *out, guint32 out_len);
gboolean dal_is_aggregation_management_chassis_product(void);
gboolean dal_is_aggregation_management_master_node(void);
gint32 dal_get_sys_all_power_rate(OBJ_HANDLE object_handle, guint16 *ps_rate_sum);

gint32 dal_open_check_realpath(const gchar *pathname, guint32 flags, mode_t mode);
FILE* dal_fopen_check_realpath(const gchar *path, const gchar *mode, const gchar *expected_path);
gint32 dal_check_realpath_before_open(const gchar *file_path);
gint32 dal_check_realpath_after_open(const gint32 fd, const gchar *file_path);
gint32 is_arch_ft(void);
gboolean dal_is_cpld_validating(void);
void init_gcov_signal_for_test(void);
gint32 dal_get_file_nums_in_directory(const char* dir_name, const char* suffix);
guint32 dal_get_directory_size(const char* dir_name);
void dal_get_smbios_status(guint8 *smbios_status);
gboolean dal_check_component_uid_valid(const gchar* component_uid);
gint32 dal_check_board_special_me_type(void);
guint8 dal_get_physical_channel_id(guint8 channel);
gboolean dal_board_comm_by_can(void);
gboolean dal_need_forward_to_active_by_can(void);
gint32 dal_forward_method_to_active_by_can(OBJ_HANDLE object_handle, const gchar* method_name, GSList* caller_info,
    GSList* input_list, GSList** output_list, guint32 timeout_ms);
gint32 dal_get_powerstate(guint8* power_state);
gint32 dal_get_smm_master_channel(OBJ_HANDLE* obj_handle);
gint32 dal_get_active_smm_instance(guint8 *instance);
gint32 dal_get_business_silkport(guchar card_type, guchar eth_num, guchar port_num, guchar *silk_num);
gint32 dal_init_i2c(guint8 bus_id, guint32 speed, guint8 mode, guint8 addr);
gint32 dal_check_cert_path_valid(const gchar *cert_path);
gint32 dal_check_cert_overdue_status(guint8 smtp_root_cert_id, const gchar *not_before, const gchar *not_after);
guint8 get_bios_flash_num(void);
void unlock_bios_flash_switch(gint32 is_remote);
gint32 bios_flash_switch(guint8 flashIndex, gint32 is_remote);

void dal_filter_invisible_char(gchar* fiter_string);

#define NUMBER_BASE10 10    // 基于十进制转换
gint32 dal_strtol(const char *input, glong *output, int base);
gint32 dal_strtoi(const char *input, gint32 *output, int base);
gint32 dal_strtoul(const char *input, gulong *output, int base);

typedef struct {
    gchar* full_name; // 需要支持备份的文件文件名(全路径)
    gchar* back_name; // 备份后的文件名

    guint32 file_owner; // 文件属主
    guint32 file_group; // 文件属组
    guint32 file_mode;  // 文件权限

    gint32 (*file_verify)(const gchar* file_name); // 用于对每个文件合法性的校验, 传入参数为文件名或备份文件名
} BACKUP_FILE_INFO;

/*
 * Description: 文件备份机制，初始化接口
 * Note:
 *     初始化时除了将提供的需要备份的文件列表加入到链表中以外，同时还会利用注册的文件合法性函数进行如下操作:
 *     1. 如果该文件校验通过，则默认认为该文件没有被篡改，使用该文件生成备份文件和SHA256文件
 *     2. 如果该文件校验失败，则认为该文件被篡改，使用备份文件覆盖该文件
 */
void dal_backup_file_init(const BACKUP_FILE_INFO* file_info, gint32 file_info_size);

/*
 * Description: 文件备份机制，文件备份接口
 * Note:
 *      文件生成或者修改时, 调用该接口更新备份文件
 */
gint32 dal_backup_file(const gchar* full_file_name);

/*
 * Description: 文件备份机制，备份文件删除接口
 * Note:
 *      文件删除时，调用该接口删除备份文件
 */
gint32 dal_delete_backup_file(const gchar* full_file_name);

/*
 * Description: 文件备份机制，备份检查接口
 * Note:
 *      文件进行使用时(文件读或其他使用场景)，通过备份文件检查当前文件是否被篡改
 *      备份文件的SHA256值，源文件中的SHA256值，以及SHA256文件中的SHA256值，存在一个和其他两个不一致时，
 *      更新不一致的文件, 如果三个文件的SHA256值都不一样，则检查失败
 */
gint32 dal_perform_backup_check_and_restore(const gchar* full_file_name);
void dal_backup_foreach_check(void);
void dal_backup_registration_file(const BACKUP_FILE_INFO* file_info);
json_bool get_element_str(json_object *jso, const gchar *element_name, const gchar **str);
void dal_get_raid_controller_rootbdf(OBJ_HANDLE obj_handle, guint8 *bus, guint8 *device, guint8 *func, guint8 *segment);
gint32 dal_check_netcard_rootbdf(OBJ_HANDLE netcard_handle, guint8 segment, guint8 bus, guint8 dev, guint8 func);

const gchar* dal_json_object_get_str(struct json_object *obj);
const gchar* dal_json_object_to_json_string(struct json_object *obj);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DFLAPI_H__ */

