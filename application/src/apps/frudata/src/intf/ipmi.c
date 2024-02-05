

#include "intf/ipmi.h"
#include "protocol.h"
#include "media/e2p_compact.h"
#include "media/e2p_large.h"
#include "elabel/elabel.h"
#include "elabel/base.h"
#include "component/component.h"
#include "component/base.h"
#include "component/sdi.h"
#include "component/mezz.h"
#include "prepare_fru.h"
#include "pme_app/uip/ipmc_public.h"
#include "media/port_rate.h"
#include "pme_app/dalib/dal_obj_priv_data.h"

#define SDI_CARD_BOARDID_3_0 0X5E
#define SDI_CARD_BOARDID_3_5 0XAC
#define SDI_CARD_BOARDID_3_6 0xAB
#define SDI_CARD_BOARDID_5_0 0xA5
#define SDI_CARD_BOARDID_5_1 0xA9
#define INFO_RACK_UCOUNT 0x1C
#define INFO_RACK_EMPTY_SN 0x1D
#define INFO_RACK_TYPICAL_CONF_SN 0x1E
#define INFO_RACK_MODEL 0x1F
#define INFO_SDI_EP_INIT_STATE 0x21
#define INFO_RACK_BASIC_RACK_SN 0x22
#define INFO_COMPONENT_UID 0x23
#define INFO_RACK_FUNCTION 0x24
#define INFO_DEVICE_TYPE 0x25
#define INFO_SDI_RESET_LINKAGE_POLICY 0x27
#define INFO_MANU 99
#define TYPE_FRU_ID 1
#define TYPE_POSITION_ID 2
#define TYPE_SILK_TEXT 3

#define RESERVED_VALUE 0
#define CUSTOMIZE_RSP_CNT 3
#define ENABLE_CUSTOMIZE_FLG 1
#define TAISHAN_CUSTOMIZE_FLG 2
#define KUNPENG_CUSTOMIZE_FLG 3
#define GET_CUSTOMIZE_FLG_RSP 2
#define RESP_DATA_HEADER_LEN 5
#define END_OF_LIST_INDEX 4
#define DATA_START_INDEX 5
#define TYPE_PCIE_EP_STATUS 2

#pragma pack(1)

typedef struct {
    guint8 custom_flag;
    gint is_customize;
    gchar *log_str;
} CUSTOM_MESSAGE;
#pragma pack()

typedef struct {
    guint16 link_status;
    guint32 speed;
    guint32 duplex;
    guint32 stp_state;
} LSW_SWITCH_PORT_INFO;

LOCAL CUSTOM_MESSAGE g_custom_message_table[] = {
    {ENABLE_CUSTOMIZE_FLG, CUSTOM_PRODUCT_NAME, "Enable customize successfully"},
    {TAISHAN_CUSTOMIZE_FLG, NEW_PRODUCT_NAME, "Enable New Product Name customize successfully"},
    {KUNPENG_CUSTOMIZE_FLG, KUNPENG_CUSTOMIZE_FLG, "Enable Kunpeng customize successfully"}
};
LOCAL void __log_dft_write(gchar buf[MAX_LOG_BUF_SIZE], guint32 len,
    const DFT_WRITE_ELABEL_REQ_S *dft_write_elabel_req);
LOCAL gint32 __get_other_fru_info(const void *pdata, FRU_PRIV_PROPERTY_S *fru_priv,
    const READ_FRU_DATA_REQ_S *read_fru_data_req, guint8 *response, guint32 response_size);
LOCAL gint32 __ipmi_get_pcie_card_numb(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6], guint8 device_type,
    guint8 device_ori_type, guint8 device_num, guint8 max_num);
LOCAL gint32 __ipmi_get_device_presence(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6], guint8 device_type,
    guint8 device_ori_type, guint8 device_num, guint8 max_num);
LOCAL gint32 __ipmi_get_device_health_status(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6],
    guint8 device_type, guint8 device_ori_type, guint8 device_num, guint8 max_num);
LOCAL gint32 __ipmi_get_device_property(guint8 property_type, const void *msg_data,
    guint8 resp_data[DEVICE_INFO_LEN + 6], guint8 device_type, guint8 device_num);
LOCAL gint32 __ipmi_get_device_location(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6], guint8 device_type,
    guint8 device_ori_type, guint8 device_num, guint8 max_num);
LOCAL gint32 __ipmi_get_device_function(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6], guint8 device_type,
    guint8 device_ori_type, guint8 device_num, guint8 max_num);
LOCAL gint32 __ipmi_get_device_name(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6], guint8 device_type,
    guint8 device_ori_type, guint8 device_num, guint8 max_num);
LOCAL gint32 __ipmi_get_device_groupid(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6], guint8 device_type,
    guint8 device_ori_type, guint8 device_num, guint8 max_num);
LOCAL gint32 __ipmi_get_device_manu_info(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6],
    guint8 device_type, guint8 device_ori_type, guint8 device_num, guint8 max_num);
LOCAL gint32 __ipmi_get_device_boardid(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6], guint8 device_type,
    guint8 device_ori_type, guint8 device_num, guint8 max_num);
LOCAL gint32 __ipmi_get_device_physical_num(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6],
    guint8 device_type, guint8 device_ori_type, guint8 device_num, guint8 max_num);
LOCAL gint32 __ipmi_get_device_fruid(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6], guint8 device_type,
    guint8 device_ori_type, guint8 device_num, guint8 max_num);
LOCAL gint32 __ipmi_get_mezz_pcie_connect_info(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6],
    guint8 device_type, guint8 device_ori_type, guint8 device_num, guint8 max_num);
LOCAL gint32 __ipmi_get_sdi_storage_ip_info(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6],
    guint8 device_type, guint8 device_ori_type, guint8 device_num, guint8 max_num);
LOCAL gint32 __ipmi_get_sdi_boot_order_info(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6],
    guint8 device_type, guint8 device_ori_type, guint8 device_num, guint8 max_num);
LOCAL gint32 __ipmi_get_sdi_os_loaded_info(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6],
    guint8 device_type, guint8 device_ori_type, guint8 device_num, guint8 max_num);
LOCAL gint32 __ipmi_get_sdi_set_ip_status(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6],
    guint8 device_type, guint8 device_ori_type, guint8 device_num, guint8 max_num);
LOCAL gint32 __ipmi_get_rack_ucount(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6], guint8 device_type,
    guint8 device_ori_type, guint8 device_num, guint8 max_num);
LOCAL gint32 __ipmi_get_rack_empty_sn(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6], guint8 device_type,
    guint8 device_ori_type, guint8 device_num, guint8 max_num);
LOCAL gint32 __ipmi_get_rack_typical_conf_sn(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6],
    guint8 device_type, guint8 device_ori_type, guint8 device_num, guint8 max_num);
LOCAL gint32 __ipmi_get_rack_model(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6], guint8 device_type,
    guint8 device_ori_type, guint8 device_num, guint8 max_num);
LOCAL gint32 ipmi_get_rack_function(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6], guint8 device_type,
    guint8 device_ori_type, guint8 device_num, guint8 max_num);
LOCAL gint32 __ipmi_get_basic_rack_sn(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6], guint8 device_type,
    guint8 device_ori_type, guint8 device_num, guint8 max_num);
LOCAL gint32 ipmi_get_device_type(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6], guint8 device_type,
    guint8 device_ori_type, guint8 device_num, guint8 max_num);
LOCAL gint32 __ipmi_get_sdi_ep_init_state(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6],
    guint8 device_type, guint8 device_ori_type, guint8 device_num, guint8 max_num);
LOCAL gint32 __ipmi_get_fruid_by_device_type(guint8 read_offset, guint8 read_len, guint8 *resp_data, guint8 *resp_len);
LOCAL gint32 __ipmi_get_fruid_by_device_num(guint8 read_offset, guint8 read_len, guint8 group_id,
    guint8 component_device_type, guint8 *resp_data, guint8 *resp_len);
LOCAL gint32 __ipmi_get_fruid_by_binded_object(guint8 device_num, guint8 group_id, guint8 component_device_type,
    guint8 *resp_data, guint8 *resp_len);
LOCAL void __ipmi_clear_fru_priv(FRU_PRIV_PROPERTY_S *fru_priv);
LOCAL gint32 __get_resp_of_sdi_presence_state(guint8 dev_num, guint8 *dev_presence);
LOCAL gint32 ipmi_get_component_uid(const void *msg_data, guint8 resp_data[DEVICE_INFO_RSP_LEN], guint8 device_type,
    guint8 device_ori_type, guint8 device_num, guint8 max_num);
LOCAL gint32 get_lsw_switch_port_info(guint8 port_id, guint8 *resp, guint8 *len);
LOCAL gint32 ipmi_get_lsw_switch_port_info(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6],
    guint8 device_type, guint8 device_ori_type, guint8 device_num, guint8 max_num);
LOCAL gboolean is_specific_sdi_component(OBJ_HANDLE obj_component, guint8 dev_num,
    guint8 *boardid_list, gint32 list_len);
LOCAL gint32 check_dft_status(void);
LOCAL gint32 __ipmi_cmd_get_sdicard_reset_linkage(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6],
    guint8 device_type, guint8 device_ori_type, guint8 device_num, guint8 max_num);

static guint8 g_max_slot_num = 0; 

static guint16 g_blade_board_id[MAX_BLADE_SLOT_NUM] = {
    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff
}; 

GET_DEVICE_INFO_S g_get_device_info_func[] = {
    {MAX_NUM, __ipmi_get_pcie_card_numb},
    {PRESENCE_STATUS, __ipmi_get_device_presence},
    {HEALTH_STATUS, __ipmi_get_device_health_status},
    {INFO_LOCATION, __ipmi_get_device_location},
    {INFO_FUNCTION, __ipmi_get_device_function},
    {INFO_DEVICENAME, __ipmi_get_device_name},
    {INFO_GROUPID, __ipmi_get_device_groupid},
    {INFO_MANU, __ipmi_get_device_manu_info},
    {INFO_BOARDID, __ipmi_get_device_boardid},
    {INFO_PHYSICALNUMBER, __ipmi_get_device_physical_num},
    {INFO_FRU_DEVICEID, __ipmi_get_device_fruid},
    {INFO_PCIE_CONNECT, __ipmi_get_mezz_pcie_connect_info},
    {INFO_SDI_STORAGE_IP, __ipmi_get_sdi_storage_ip_info},
    {INFO_SDI_BOOT_ORDER, __ipmi_get_sdi_boot_order_info},
    {INFO_SDI_OS_LOADED, __ipmi_get_sdi_os_loaded_info},
    {INFO_SDI_SET_IP_STATUS, __ipmi_get_sdi_set_ip_status},
    {INFO_RACK_UCOUNT, __ipmi_get_rack_ucount},
    {INFO_RACK_EMPTY_SN, __ipmi_get_rack_empty_sn},
    {INFO_RACK_TYPICAL_CONF_SN, __ipmi_get_rack_typical_conf_sn},
    {INFO_RACK_MODEL, __ipmi_get_rack_model},
    {INFO_SDI_EP_INIT_STATE, __ipmi_get_sdi_ep_init_state},
    {INFO_RACK_BASIC_RACK_SN, __ipmi_get_basic_rack_sn},
    {INFO_COMPONENT_UID, ipmi_get_component_uid},
    {INFO_LSW_SET_PORT, ipmi_get_lsw_switch_port_info},
    {INFO_RACK_BASIC_RACK_SN, __ipmi_get_basic_rack_sn},
    {INFO_RACK_FUNCTION, ipmi_get_rack_function},
    {INFO_DEVICE_TYPE, ipmi_get_device_type},
    {INFO_SDI_RESET_LINKAGE_POLICY, __ipmi_cmd_get_sdicard_reset_linkage}
};

LOCAL gint32 ipmi_get_component_uid(const void *msg_data, guint8 resp_data[DEVICE_INFO_RSP_LEN], guint8 device_type,
    guint8 device_ori_type, guint8 device_num, guint8 max_num)
{
    OBJ_HANDLE obj_handle = 0;
    const gint32 default_data_start_index = 5;
    const gint32 device_info_index_type = 0;
    const gint32 device_info_index_num = 1;
    const gint32 device_info_index_groupid = 2;
    component_union component_index = { 0 };

    component_index.device_info[device_info_index_type] = device_type;
    component_index.device_info[device_info_index_num] = device_num;
    component_index.device_info[device_info_index_groupid] = 1;

    gint32 ret = dfl_get_binded_object(CLASS_COMPONENT, (guint32)component_index.component_alias_id, &obj_handle);
    if (ret != DFL_OK) {
        resp_data[default_data_start_index] = 0;
        debug_log(DLOG_ERROR, "%s: dfl_get_binded_object failed. ret(%d)", __FUNCTION__, ret);
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }

    gchar unique_id[DEVICE_INFO_LEN] = {0};
    ret = dal_get_property_value_string(obj_handle, PROPERTY_COMPONENT_UNIQUE_ID, unique_id, sizeof(unique_id));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_string fail %d", __FUNCTION__, ret);
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }

    if (dal_check_component_uid_valid((const gchar *)unique_id) != TRUE) {
        debug_log(DLOG_ERROR, "%s: component unique id is invalid", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, COMP_CODE_STATUS_INVALID);
    }

    errno_t safe_fun_ret = strncpy_s((gchar *)&resp_data[default_data_start_index],
        DEVICE_INFO_RSP_LEN - default_data_start_index, unique_id, strlen(unique_id));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }

    return ipmi_send_response(msg_data, default_data_start_index + strlen(unique_id), resp_data);
}


LOCAL void __log_dft_write(gchar buf[MAX_LOG_BUF_SIZE], guint32 len, const DFT_WRITE_ELABEL_REQ_S *dft_write_elabel_req)
{
    if (dft_write_elabel_req == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return;
    }

    guint32 offset = 0;
    const gchar *p = (const gchar *)dft_write_elabel_req;
    guint32 i = 0;
    for (; i < len - 1; i++) {
        offset += snprintf_truncated_s(buf + offset, MAX_LOG_BUF_SIZE - offset, "%02x-", p[i]);
    }

    (void)snprintf_s(buf + offset, MAX_LOG_BUF_SIZE - offset, MAX_LOG_BUF_SIZE - offset - 1, "%02x", p[i]);

    return;
}

LOCAL gint32 __check_fru_info_validattion(const void *pdata, FRU_PRIV_PROPERTY_S *fru_priv,
    const READ_FRU_DATA_REQ_S *read_fru_data_req, guint8 *response, guint32 response_size)
{
    if (pdata == NULL || fru_priv == NULL || read_fru_data_req == NULL || response == NULL) {
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        return RET_ERR;
    }
    if (response_size < MAX_READ_FRU_DATA_LEN) {
        debug_log(DLOG_ERROR, "%s: response size is insufficient: %d.", __FUNCTION__, response_size);
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 __get_fru_info_by_type(const void *pdata, FRU_PRIV_PROPERTY_S *fru_priv, FRU_FILE_S *fru_file)
{
    if (fru_priv->fru_property.is_sup_dft == 0) {
        errno_t safe_fun_ret = memmove_s(fru_file, sizeof(FRU_FILE_S), fru_priv->fru_file, sizeof(FRU_FILE_S));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        }
        return RET_OK;
    }

    FRU_INFO_S *fru_info = (FRU_INFO_S *)g_malloc0(sizeof(FRU_INFO_S));
    if (fru_info == NULL) {
        return RET_ERR;
    }

    TARGET_TYPE_E target_type = get_ipmi_target_type(pdata);
    if (fru_priv->fru_property.parser_protocol == HUAWEI_PARSER && target_type == IPMI_SMM) {
        
        
        sync_elabel_to_fru(fru_info, fru_priv->elabel_info);
    } else {
        
        
        (void)create_fru_data(fru_info, (fru_priv->eeprom_format == EEPROM_FORMAT_TIANCHI) ?
            fru_priv->fru_file->header.frudata :
            fru_priv->fru_file->frudata);
    }
    generate_fru_file(fru_priv->eeprom_format, fru_info, fru_file);

    dal_destroy_fru_data(fru_info);

    return RET_OK;
}

LOCAL gint32 __get_other_fru_info(const void *pdata, FRU_PRIV_PROPERTY_S *fru_priv,
    const READ_FRU_DATA_REQ_S *read_fru_data_req, guint8 *response, guint32 response_size)
{
    if (__check_fru_info_validattion(pdata, fru_priv, read_fru_data_req, response, response_size) != RET_OK) {
        return RET_ERR;
    }

    
    FRU_FILE_S fru_file = { 0 };
    if (__get_fru_info_by_type(pdata, fru_priv, &fru_file) != RET_OK) {
        return ipmi_send_simple_response(pdata, COMP_CODE_UNKNOWN);
    }

    guint16 fru_len = (fru_priv->eeprom_format == EEPROM_FORMAT_TIANCHI) ? FRU_MAX_LEN : fru_file.header.fru_head.len;
    if (fru_len < read_fru_data_req->offset) {
        debug_log(DLOG_ERROR, "offset is wrong,cnt = %d,fru_len = %d,offset = %d", read_fru_data_req->cnt, fru_len,
            read_fru_data_req->offset);
        return ipmi_send_simple_response(pdata, COMP_CODE_UNKNOWN);
    }

    
    READ_FRU_DATA_RESP_S *read_fru_data_resp = (READ_FRU_DATA_RESP_S *)response;
    read_fru_data_resp->completion = COMP_CODE_SUCCESS;
    read_fru_data_resp->cnt =
        MIN(MAX_READ_FRU_DATA_LEN, MIN(read_fru_data_req->cnt, fru_len - read_fru_data_req->offset));

    guint8 *frudata = (fru_priv->eeprom_format == EEPROM_FORMAT_TIANCHI) ? fru_file.header.frudata : fru_file.frudata;
    errno_t safe_fun_ret = memmove_s(read_fru_data_resp->content, MAX_READ_FRU_DATA_LEN,
        frudata + read_fru_data_req->offset, read_fru_data_resp->cnt);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }

    return ipmi_send_response(pdata, read_fru_data_resp->cnt + 2, response);
}

LOCAL gint32 __get_rack_ucount(guint8 input_device_type, guint8 device_index, guint8 *ucount)
{
    if ((input_device_type != COMPONENT_TYPE_RACK) || (device_index != 1)) {
        return COMP_CODE_CMD_INVALID;
    }

    OBJ_HANDLE obj_rack_asset_mgmt = OBJ_HANDLE_COMMON;
    gint32 ret = dal_get_object_handle_nth(CLASS_RACK_ASSET_MGMT, 0, &obj_rack_asset_mgmt);
    if (ret != RET_OK) {
        return COMP_CODE_DATA_NOT_AVAILABLE;
    }

    (void)dal_get_property_value_byte(obj_rack_asset_mgmt, PROPERTY_RACK_ASSET_MGMT_U_COUNT, ucount);
    return RET_OK;
}

LOCAL gint32 __get_rack_info_str(const gchar *property_name, guint8 input_device_type, guint8 device_index,
    gchar *value, guint32 size)
{
    if ((input_device_type != COMPONENT_TYPE_RACK) || (device_index != 1)) {
        return COMP_CODE_CMD_INVALID;
    }

    OBJ_HANDLE obj_rack_asset_mgmt = OBJ_HANDLE_COMMON;
    gint32 ret = dal_get_object_handle_nth(CLASS_RACK_ASSET_MGMT, 0, &obj_rack_asset_mgmt);
    if (ret != RET_OK) {
        return COMP_CODE_DATA_NOT_AVAILABLE;
    }

    ret = dal_get_property_value_string(obj_rack_asset_mgmt, property_name, value, size);
    if (ret != RET_OK) {
        return COMP_CODE_DATA_NOT_AVAILABLE;
    }

    return COMP_CODE_SUCCESS;
}

LOCAL gint32 __ipmi_get_pcie_card_numb(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6], guint8 device_type,
    guint8 device_ori_type, guint8 device_num, guint8 max_num)
{
    const guint8 resp_len = 6;

    if (device_type != DEVICE_TYPE_PCIE_CARD) {
        resp_data[5] = max_num;
        return ipmi_send_response(msg_data, resp_len, resp_data);
    }

    OBJ_HANDLE object_handle = 0;
    (void)dal_get_object_handle_nth(OBJ_PRODUCT_COMPONENT, 0, &object_handle);

    guint8 pcie_num = 0;
    (void)dal_get_property_value_byte(object_handle, PROPERTY_COMPOENT_PCIE_CARD_NUM, &pcie_num);
    resp_data[5] = pcie_num;

    return ipmi_send_response(msg_data, resp_len, resp_data);
}


LOCAL gint32 __get_resp_of_sdi_presence_state(guint8 dev_num, guint8 *dev_presence)
{
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;

    gint32 ret = dfl_get_object_list(CLASS_COMPONENT, &obj_list);
    if (ret != DFL_OK || obj_list == NULL) {
        *dev_presence = 0;
        return COMP_CODE_DATA_NOT_AVAILABLE;
    }
    for (obj_node = obj_list; obj_node != NULL; obj_node = obj_node->next) {
        guint8 boardid_list[5] = { SDI_CARD_BOARDID_3_5, SDI_CARD_BOARDID_3_6,
            SDI_CARD_BOARDID_3_0, SDI_CARD_BOARDID_5_0, SDI_CARD_BOARDID_5_1};
        if (is_specific_sdi_component((OBJ_HANDLE)obj_node->data, dev_num,
            boardid_list, sizeof(boardid_list)) == FALSE) {
            continue;
        } else {
            *dev_presence = 1;
            g_slist_free(obj_list);
            return RET_OK;
        }
    }
    g_slist_free(obj_list);
    *dev_presence = 0;
    return RET_OK;
}

LOCAL gint32 __ipmi_get_device_presence(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6], guint8 device_type,
    guint8 device_ori_type, guint8 device_num, guint8 max_num)
{
    guint8 end_flag = (guint8)-1;
    guint8 presence = 0;
    gint32 ret;

    if (device_num == 0xff && device_ori_type == DEVICE_TYPE_SDI_CARD) {
        ret = __get_resp_of_sdi_presence_state(device_num, &presence);
    } else {
        ret = get_device_presence(device_type, device_num, &presence, &end_flag);
    }

    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s get_device_presence fail.", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, ret);
    }

    resp_data[5] = (guint8)presence;
    const guint8 resp_len = 6;

    return ipmi_send_response(msg_data, resp_len, resp_data);
}

LOCAL gint32 __ipmi_get_device_health_status(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6],
    guint8 device_type, guint8 device_ori_type, guint8 device_num, guint8 max_num)
{
    guint8 end_flag = (guint8)-1;
    gint32 status = 5; // 5: indicates the unknown health status.

    gint32 ret = get_device_health_status(device_type, device_num, &status, &end_flag);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s get_device_health_status fail.", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, ret);
    }

    resp_data[5] = (guint8)status;
    const guint8 resp_len = 6;

    return ipmi_send_response(msg_data, resp_len, resp_data);
}

LOCAL gint32 __ipmi_get_device_property(guint8 property_type, const void *msg_data,
    guint8 resp_data[DEVICE_INFO_LEN + 6], guint8 device_type, guint8 device_num)
{
    const guint8 *req_data = get_ipmi_src_data(msg_data);
    if (req_data == NULL) {
        debug_log(DLOG_ERROR, "Get ipmi src data failed");
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    const gint32 read_offset = req_data[7];
    const guint8 read_len = req_data[8];
    guint8 end_flag = (guint8)-1;
    gchar ret_buf[DEVICE_INFO_LEN + 1] = {0};

    gint32 ret = get_device_property(property_type, device_type, device_num, ret_buf, sizeof(ret_buf), read_offset,
        read_len, &end_flag);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s get_device_property fail.", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }

    resp_data[4] = end_flag;
    errno_t safe_fun_ret = strncpy_s((gchar *)&resp_data[5], DEVICE_INFO_LEN + 6 - 5, ret_buf, strlen(ret_buf));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }

    return ipmi_send_response(msg_data, 5 + strlen(ret_buf), resp_data);
}

LOCAL gint32 __ipmi_get_device_location(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6], guint8 device_type,
    guint8 device_ori_type, guint8 device_num, guint8 max_num)
{
    return __ipmi_get_device_property(DEVICE_PROPERTY_TYPE_LOCATION, msg_data, resp_data, device_type, device_num);
}

LOCAL gint32 __ipmi_get_device_function(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6], guint8 device_type,
    guint8 device_ori_type, guint8 device_num, guint8 max_num)
{
    return __ipmi_get_device_property(DEVICE_PROPERTY_TYPE_FUNCTION, msg_data, resp_data, device_type, device_num);
}

LOCAL gint32 __ipmi_get_device_name(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6], guint8 device_type,
    guint8 device_ori_type, guint8 device_num, guint8 max_num)
{
    return __ipmi_get_device_property(DEVICE_PROPERTY_TYPE_NAME, msg_data, resp_data, device_type, device_num);
}

LOCAL gint32 __ipmi_get_device_groupid(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6], guint8 device_type,
    guint8 device_ori_type, guint8 device_num, guint8 max_num)
{
    const guint8 *req_data = get_ipmi_src_data(msg_data);
    if (req_data == NULL) {
        debug_log(DLOG_ERROR, "Get ipmi src data failed");
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    const gint32 read_offset = req_data[7];
    if ((read_offset < 0) || (read_offset > DEVICE_INFO_LEN)) {
        return RET_ERR;
    }

    const guint8 read_len = req_data[8];
    if ((read_len < 0) || (read_len > DEVICE_INFO_LEN + 1)) {
        return RET_ERR;
    }

    guint8 end_flag = (guint8)-1;
    gchar groupid = 0;
    gint32 ret = get_device_groupid(device_type, device_num, &groupid, &end_flag);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s get_device_groupid fail.", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }

    resp_data[5] = (guint8)groupid;
    const guint8 resp_len = 6;

    return ipmi_send_response(msg_data, resp_len, resp_data);
}

LOCAL gint32 __ipmi_get_device_manu_info(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6],
    guint8 device_type, guint8 device_ori_type, guint8 device_num, guint8 max_num)
{
    const guint8 *req_data = get_ipmi_src_data(msg_data);
    if (req_data == NULL) {
        debug_log(DLOG_ERROR, "Get ipmi src data failed");
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    const gint32 read_offset = req_data[7];
    const guint8 read_len = req_data[8];
    guint8 end_flag = (guint8)-1;
    guint8 presence = COMPONENT_NOT_IN_PRESENT;

    gint32 ret = get_device_presence(device_type, device_num, &presence, &end_flag);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s get_device_presence fail", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, ret);
    }

    if (presence == COMPONENT_NOT_IN_PRESENT) {
        debug_log(DLOG_ERROR, "%s device not presence.", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, COMP_CODE_DATA_NOT_AVAILABLE);
    }

    end_flag = (guint8)-1;
    gchar uc_manu_info[DEVICE_INFO_LEN + 1] = {0};
    ret = get_device_manu_info(device_type, device_num, uc_manu_info, sizeof(uc_manu_info), read_offset, read_len,
        &end_flag);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s get_device_manu_info fail.", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }

    resp_data[4] = end_flag;
    errno_t safe_fun_ret =
        strncpy_s((gchar *)&resp_data[5], DEVICE_INFO_LEN + 6 - 5, uc_manu_info, strlen(uc_manu_info));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }
    return ipmi_send_response(msg_data, 5 + strlen(uc_manu_info), resp_data);
}

LOCAL gint32 __ipmi_get_device_boardid(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6], guint8 device_type,
    guint8 device_ori_type, guint8 device_num, guint8 max_num)
{
    guint8 boardid[2] = {0};
    gint32 ret = get_device_boardid(device_type, device_num, boardid);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s get_device_boardid fail.", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }

    resp_data[5] = (guint8)boardid[0];
    resp_data[6] = (guint8)boardid[1];
    const guint8 resp_len = 7;

    return ipmi_send_response(msg_data, resp_len, resp_data);
}

LOCAL gint32 __ipmi_get_device_physical_num(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6],
    guint8 device_type, guint8 device_ori_type, guint8 device_num, guint8 max_num)
{
    guint8 boardid[2] = {0};
    gint32 ret = get_device_physical_num(device_type, device_num, boardid);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s get_device_physical_num fail.", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }

    resp_data[5] = (guint8)boardid[0];
    resp_data[6] = (guint8)boardid[1];
    const guint8 resp_len = 7;

    return ipmi_send_response(msg_data, resp_len, resp_data);
}

LOCAL gint32 __ipmi_get_device_fruid(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6], guint8 device_type,
    guint8 device_ori_type, guint8 device_num, guint8 max_num)
{
    guint8 fru_id = MAX_FRU_ID;
    gint32 ret = get_device_fruid(device_type, device_num, &fru_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s get_device_fruid fail.", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }

    resp_data[5] = fru_id;
    const guint8 resp_len = 6;

    return ipmi_send_response(msg_data, resp_len, resp_data);
}

LOCAL gint32 __ipmi_get_mezz_pcie_connect_info(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6],
    guint8 device_type, guint8 device_ori_type, guint8 device_num, guint8 max_num)
{
    const guint8 *req_data = get_ipmi_src_data(msg_data);
    if (req_data == NULL) {
        debug_log(DLOG_ERROR, "Get ipmi src data failed");
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    const gint32 read_offset = req_data[7];
    const guint8 read_len = req_data[8];
    guint8 end_flag = (guint8)-1;
    guint8 mezz_pcie_connect[32] = {0};
    guint8 mezz_pcie_connectlen = 0;

    gint32 ret = get_mezz_pcie_connect_info(device_type, device_num, mezz_pcie_connect, sizeof(mezz_pcie_connect),
        read_offset, read_len, &end_flag, &mezz_pcie_connectlen);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s getMezzPcieConnectInfo fail.", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }

    if (mezz_pcie_connectlen > sizeof(mezz_pcie_connect)) {
        mezz_pcie_connectlen = (guint8)sizeof(mezz_pcie_connect);
    }

    resp_data[4] = end_flag;
    errno_t safe_fun_ret =
        memcpy_s((char *)&resp_data[5], DEVICE_INFO_LEN + 6 - 5, mezz_pcie_connect, mezz_pcie_connectlen);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d.", __FUNCTION__, safe_fun_ret);
    }
    const guint8 resp_len = 5 + mezz_pcie_connectlen;

    return ipmi_send_response(msg_data, resp_len, resp_data);
}

LOCAL gint32 __ipmi_get_sdi_storage_ip_info(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6],
    guint8 device_type, guint8 device_ori_type, guint8 device_num, guint8 max_num)
{
    guint8 set_status = 0;
    guint32 ipv4_addr = 0;
    guint16 vlan_id = 0;

    gint32 ret = get_sdi_storage_ip_info(device_ori_type, device_num, &set_status, &ipv4_addr, &vlan_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s get_sdi_storage_ip_info fail.", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, ret);
    }

    resp_data[4] = 0;
    resp_data[5] = set_status;
    resp_data[6] = LONGB0(ipv4_addr);
    resp_data[7] = LONGB1(ipv4_addr);
    resp_data[8] = LONGB2(ipv4_addr);
    resp_data[9] = LONGB3(ipv4_addr);
    resp_data[10] = LONGB0(vlan_id);
    resp_data[11] = LONGB1(vlan_id);
    const guint8 resp_len = 12;

    return ipmi_send_response(msg_data, resp_len, resp_data);
}

LOCAL gint32 __ipmi_get_sdi_boot_order_info(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6],
    guint8 device_type, guint8 device_ori_type, guint8 device_num, guint8 max_num)
{
    guint8 sdi_boot_order = 0;
    guint8 vaild_type  = 0xff;
    gint32 ret = get_sdi_boot_order_info(device_ori_type, device_num, &sdi_boot_order, &vaild_type);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s get_sdi_boot_order_info fail.", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, ret);
    }

    resp_data[4] = 0;
    resp_data[5] = sdi_boot_order;
    guint8 resp_len = 6;
    if (vaild_type != 0xff) {
        resp_data[6] = vaild_type;
        resp_len++;
    }
  
    return ipmi_send_response(msg_data, resp_len, resp_data);
}

LOCAL gint32 __ipmi_get_sdi_os_loaded_info(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6],
    guint8 device_type, guint8 device_ori_type, guint8 device_num, guint8 max_num)
{
    guint8 os_loaded = 0;
    gint32 ret = get_sdi_os_loaded_info(device_ori_type, device_num, &os_loaded);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s get_sdi_os_loaded_info fail.", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, ret);
    }

    resp_data[4] = 0;
    resp_data[5] = os_loaded;
    const guint8 resp_len = 6;

    return ipmi_send_response(msg_data, resp_len, resp_data);
}

LOCAL gint32 __ipmi_get_sdi_set_ip_status(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6],
    guint8 device_type, guint8 device_ori_type, guint8 device_num, guint8 max_num)
{
    guint8 set_status = 0;
    guint32 ipv4_addr = 0;
    guint16 vlan_id = 0;

    gint32 ret = get_sdi_set_ip_status(device_ori_type, device_num, &set_status, &ipv4_addr, &vlan_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s get_sdi_set_ip_status fail.", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, ret);
    }

    resp_data[4] = 0;
    resp_data[5] = set_status;
    resp_data[6] = LONGB0(ipv4_addr);
    resp_data[7] = LONGB1(ipv4_addr);
    resp_data[8] = LONGB2(ipv4_addr);
    resp_data[9] = LONGB3(ipv4_addr);
    resp_data[10] = LONGB0(vlan_id);
    resp_data[11] = LONGB1(vlan_id);
    const guint8 resp_len = 12;

    return ipmi_send_response(msg_data, resp_len, resp_data);
}

LOCAL gint32 __ipmi_get_rack_ucount(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6], guint8 device_type,
    guint8 device_ori_type, guint8 device_num, guint8 max_num)
{
    guint8 ucount = 0;
    gint32 ret = __get_rack_ucount(device_type, device_num, &ucount);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get_rack_ucount fail", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, ret);
    }
    resp_data[END_OF_LIST_INDEX] = 0;
    resp_data[DATA_START_INDEX] = ucount;
    const guint8 resp_len = (guint8)(RESP_DATA_HEADER_LEN + sizeof(guint8));

    return ipmi_send_response(msg_data, resp_len, resp_data);
}

LOCAL gint32 __ipmi_get_rack_empty_sn(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6], guint8 device_type,
    guint8 device_ori_type, guint8 device_num, guint8 max_num)
{
    gint32 ret = __get_rack_info_str(PROPERTY_RACK_ASSET_MGMT_EMPTY_RACK_SN, device_type, device_num,
        (gchar *)(resp_data + RESP_DATA_HEADER_LEN), MAX_PROPERTY_VALUE_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get rack empty sn fail", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, ret);
    }
    resp_data[END_OF_LIST_INDEX] = 0;
    const guint8 resp_len = (guint8)(RESP_DATA_HEADER_LEN + strlen((gchar *)(resp_data + RESP_DATA_HEADER_LEN)));

    return ipmi_send_response(msg_data, resp_len, resp_data);
}

LOCAL gint32 __ipmi_get_rack_typical_conf_sn(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6],
    guint8 device_type, guint8 device_ori_type, guint8 device_num, guint8 max_num)
{
    gint32 ret = __get_rack_info_str(PROPERTY_RACK_ASSET_MGMT_TYPICAL_CONFIGURATION_SN, device_type, device_num,
        (gchar *)(resp_data + RESP_DATA_HEADER_LEN), MAX_PROPERTY_VALUE_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get rack typical conf sn fail", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, ret);
    }
    resp_data[END_OF_LIST_INDEX] = 0;
    const guint8 resp_len = (guint8)(RESP_DATA_HEADER_LEN + strlen((gchar *)(resp_data + RESP_DATA_HEADER_LEN)));

    return ipmi_send_response(msg_data, resp_len, resp_data);
}

LOCAL gint32 __ipmi_get_rack_model(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6], guint8 device_type,
    guint8 device_ori_type, guint8 device_num, guint8 max_num)
{
    gint32 ret = __get_rack_info_str(PROPERTY_RACK_ASSET_MGMT_RACK_MODEL, device_type, device_num,
        (gchar *)(resp_data + RESP_DATA_HEADER_LEN), MAX_PROPERTY_VALUE_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get rack model fail", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, ret);
    }
    resp_data[END_OF_LIST_INDEX] = 0;
    const guint8 resp_len = (guint8)(RESP_DATA_HEADER_LEN + strlen((gchar *)(resp_data + RESP_DATA_HEADER_LEN)));

    return ipmi_send_response(msg_data, resp_len, resp_data);
}

LOCAL gint32 ipmi_get_rack_function(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6], guint8 device_type,
    guint8 device_ori_type, guint8 device_num, guint8 max_num)
{
    gint32 ret = __get_rack_info_str(PROPERTY_RACK_ASSET_MGMT_RACK_FUNCTION, device_type, device_num,
        (gchar *)(resp_data + RESP_DATA_HEADER_LEN), MAX_PROPERTY_VALUE_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get rack function fail", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, ret);
    }
    resp_data[END_OF_LIST_INDEX] = 0;
    const guint8 resp_len = RESP_DATA_HEADER_LEN + strlen((gchar *)(resp_data + RESP_DATA_HEADER_LEN));

    return ipmi_send_response(msg_data, resp_len, resp_data);
}


LOCAL gint32 ipmi_get_device_type(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6], guint8 device_type,
    guint8 device_ori_type, guint8 device_num, guint8 max_num)
{
    OBJ_HANDLE product_object = 0;
    guint32 product_unique_id = 0;
    gint32 ret = 0;
    guint8 value = 0;
 
    if ((device_type != COMPONENT_TYPE_MAINBOARD) || (device_num != 1)) {
        debug_log(DLOG_ERROR, "%s : device_type[%d] or device_num[%d] is invalid",
            __FUNCTION__, device_type, device_num);
        return COMP_CODE_CMD_INVALID;
    }
    ret = dfl_get_object_handle(CLASS_NAME_PRODUCT, &product_object);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: Get object %s handle failed! ret = %d", __FUNCTION__, CLASS_NAME_PRODUCT, ret);
        return COMP_CODE_DATA_NOT_AVAILABLE;
    }
    ret = dal_get_property_value_uint32(product_object, PROPERTY_PRODUCT_UNIQUE_ID, &product_unique_id);
    if (ret != RET_OK) {
        return COMP_CODE_DATA_NOT_AVAILABLE;
    }
 
    value = product_unique_id & 0xff;
 
    resp_data[END_OF_LIST_INDEX] = 0;
    resp_data[DATA_START_INDEX] = value;
 
    const guint8 resp_len = RESP_DATA_HEADER_LEN + sizeof(guint8);
 
    return ipmi_send_response(msg_data, resp_len, resp_data);
}

LOCAL gint32 __ipmi_get_basic_rack_sn(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6], guint8 device_type,
    guint8 device_ori_type, guint8 device_num, guint8 max_num)
{
    gint32 ret = __get_rack_info_str(PROPERTY_RACK_ASSET_MGMT_BASIC_RACK_SN, device_type, device_num,
        (gchar *)(resp_data + RESP_DATA_HEADER_LEN), MAX_PROPERTY_VALUE_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get basic rack sn fail", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, ret);
    }
    resp_data[END_OF_LIST_INDEX] = 0;
    const guint8 resp_len = (guint8)(RESP_DATA_HEADER_LEN + strlen((gchar *)(resp_data + RESP_DATA_HEADER_LEN)));

    return ipmi_send_response(msg_data, resp_len, resp_data);
}

LOCAL gboolean is_specific_sdi_component(OBJ_HANDLE obj_component,
    guint8 dev_num, guint8 *boardid_list, gint32 list_len)
{
    guint8 typecode = 0;
    guint8 com_dev_num = 0;
    guint16 board_id = 0;

    (void)dal_get_property_value_byte(obj_component, PROPERTY_COMPONENT_DEVICE_TYPE, &typecode);
    if (typecode != DEVICE_TYPE_SDI_CARD) {
        return FALSE;
    }

    (void)dal_get_property_value_byte(obj_component, PROPERTY_COMPONENT_DEVICENUM, &com_dev_num);
    if (dev_num != INVALID_UINT8 && dev_num != com_dev_num) {
        return FALSE;
    }

    gint32 ret = dal_get_extern_value_uint16(obj_component, PROPERTY_COMPONENT_BOARDID, &board_id, DF_HW);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s:get boardid failed ret = %d", __FUNCTION__, ret);
        return FALSE;
    }

    
    for (guint8 i = 0; i < list_len; ++i) {
        if (board_id == boardid_list[i]) {
            return TRUE;
        }
    }
    debug_log(DLOG_ERROR, "%s:sdi card boardid = %d", __FUNCTION__, board_id);
    return FALSE;
}

LOCAL gint32 get_sdi_ep_init_state_from_cpld(guint8 device_num, guint8 *inited_state)
{
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guint8 inited_count = 0;
    guint8 sdi_count = 0;

    gint32 ret = dfl_get_object_list(CLASS_COMPONENT, &obj_list);
        if (ret != DFL_OK || obj_list == NULL) {
            return COMP_CODE_DATA_NOT_AVAILABLE;
        }

        for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
            guint8 state = 0;
            guint8 boardid_list[3] = {SDI_CARD_BOARDID_3_5, SDI_CARD_BOARDID_3_6, SDI_CARD_BOARDID_3_0};
            if (is_specific_sdi_component((OBJ_HANDLE)obj_node->data, device_num,
                boardid_list, sizeof(boardid_list)) == FALSE) {
                continue;
            }

            sdi_count++;

            ret = __get_init_state_from_related_accessor((OBJ_HANDLE)obj_node->data,
                PROPERTY_COMPONENT_BOARDID, &state);
            if (ret != RET_OK) {
                continue;
            }
            if (state) {
                inited_count++;
            }
        }
        g_slist_free(obj_list);
        if (sdi_count > 0) {
            
            *inited_state = (inited_count == sdi_count);
        } else {
            
            return  COMP_CODE_DATA_NOT_AVAILABLE;
        }
    return RET_OK;
}


LOCAL gint32 __ipmi_get_sdi_ep_init_state(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6],
    guint8 device_type, guint8 device_ori_type, guint8 device_num, guint8 max_num)
{
    guint8 inited_state = 0;
    OBJ_HANDLE sdi_obj = 0;
    guint8 mcu_support = 0;
    
    if (device_ori_type != DEVICE_TYPE_SDI_CARD) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_CMD_INVALID);
    }
    gint32 ret = dal_get_specific_object_byte(CLASS_PCIE_SDI_CARD, PROPERTY_CARD_SLOT, device_num, &sdi_obj);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get PCIeSDICard object fail", __FUNCTION__);
    }

    ret = dal_get_property_value_byte(sdi_obj, PROPERTY_SDI_CARD_MCUSUPPORTED, &mcu_support);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get PCIeSDICard McuSupported fail", __FUNCTION__);
    }
    if (mcu_support == 1) {
        ret = get_sdi_ep_init_state_from_mcu(sdi_obj, &inited_state);
    } else {
        ret = get_sdi_ep_init_state_from_cpld(device_num, &inited_state);
    }

    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : get_sdi_ep_init_state fail.ret= %d", __FUNCTION__, ret);
        return ipmi_send_simple_response(msg_data, ret);
    }

    resp_data[DATA_START_INDEX] = inited_state;
    const size_t resp_len = RESP_DATA_HEADER_LEN + sizeof(guint8);
    return ipmi_send_response(msg_data, resp_len, resp_data);
}

LOCAL gint32 __ipmi_get_fruid_by_device_type(guint8 read_offset, guint8 read_len, guint8 *resp_data, guint8 *resp_len)
{
    GSList *fru_list = NULL;
    gint32 ret = dfl_get_object_list(CLASS_FRU, &fru_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s dfl_get_object_list failed.", __FUNCTION__);
        return ret;
    }

    guint32 fru_num = 0;
    ret = dfl_get_object_count(CLASS_FRU, &fru_num);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s dfl_get_object_count failed, ret = %d.", __FUNCTION__, ret);
        g_slist_free(fru_list);
        return ret;
    }

    guint16 fru_index = 0;
    guint8 end_flag = 0;
    guint16 resp_index = 0;
    for (GSList *node = fru_list; node; node = node->next) {
        if (fru_index < read_offset) {
            fru_index++;
            continue;
        }

        if (fru_index > (read_offset + read_len)) {
            end_flag = 1;
            break;
        }

        OBJ_HANDLE obj_handle = (OBJ_HANDLE)node->data;
        guint8 fru_id = MAX_FRU_ID;
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_FRU_ID, &fru_id);
        resp_data[5 + resp_index++] = fru_id;
        if ((resp_index != (DEVICE_INFO_LEN - 1)) && (resp_index != read_len)) {
            fru_index++;
            continue;
        }

        if (fru_num > (read_offset + read_len)) {
            end_flag = 1;
        }

        break;
    }

    g_slist_free(fru_list);

    if (resp_index == 0) {
        return RET_ERR;
    }

    *resp_len = resp_index + 5;
    resp_data[4] = end_flag;

    return RET_OK;
}

LOCAL gint32 __ipmi_get_fruid_by_device_num(guint8 read_offset, guint8 read_len, guint8 group_id,
    guint8 component_device_type, guint8 *resp_data, guint8 *resp_len)
{
    GSList *com_list = NULL;
    gint32 ret = dfl_get_object_list(CLASS_COMPONENT, &com_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s dfl_get_object_list failed.", __FUNCTION__);
        return ret;
    }

    GSList *fru_id_list = NULL;
    for (GSList *node = com_list; node; node = node->next) {
        OBJ_HANDLE obj_handle = (OBJ_HANDLE)node->data;
        guint8 dev_type = 0;
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &dev_type);

        
        guint8 grp_id = group_id;
        if (grp_id != CM_GROUP_ID_ALL) {
            (void)dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_GROUPID, &grp_id);
        }

        guint8 fru_id = INVALID_FRU_ID;
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_FRUID, &fru_id);

        
        if ((component_device_type == dev_type) && (group_id == grp_id) && (fru_id != INVALID_FRU_ID)) {
            fru_id_list = g_slist_append(fru_id_list, (gpointer)g_variant_new_byte(fru_id));
        }
    }

    g_slist_free(com_list);

    if (fru_id_list == NULL) {
        return RET_ERR;
    }

    guint32 fru_id_count = g_slist_length(fru_id_list);
    
    if ((fru_id_count == 0) || (read_offset >= fru_id_count)) {
        g_slist_free_full(fru_id_list, (GDestroyNotify)g_variant_unref);
        return RET_ERR;
    }

    
    guint8 end_flag = 0;
    if ((read_offset + read_len) < fru_id_count) {
        end_flag = 1;
    }

    
    guint16 resp_index = 0;
    for (GSList *node = g_slist_nth(fru_id_list, read_offset); node; node = node->next) {
        guint8 fru_id = g_variant_get_byte((GVariant *)node->data);
        resp_data[5 + resp_index++] = fru_id;

        
        if (resp_index == read_len) {
            break;
        }
    }

    *resp_len = resp_index + 5;
    resp_data[4] = end_flag;
    g_slist_free_full(fru_id_list, (GDestroyNotify)g_variant_unref);

    return RET_OK;
}

LOCAL gint32 __ipmi_get_fruid_by_binded_object(guint8 device_num, guint8 group_id, guint8 component_device_type,
    guint8 *resp_data, guint8 *resp_len)
{
    component_union component_index = { 0 };
    component_index.device_info[0] = component_device_type;
    component_index.device_info[1] = device_num;
    component_index.device_info[2] = group_id;
    component_index.device_info[3] = 0;

    OBJ_HANDLE obj_handle = 0;
    gint32 ret = dfl_get_binded_object(CLASS_COMPONENT, (guint32)component_index.component_alias_id, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s dfl_get_binded_object failed. ret(%d), component alias(%d)", __FUNCTION__, ret,
            component_index.component_alias_id);
        return ret;
    }

    guint8 fru_id = MAX_FRU_ID;
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_FRUID, &fru_id);

    resp_data[4] = 0;
    resp_data[5] = fru_id;
    *resp_len = 6;

    return RET_OK;
}

LOCAL gint32 __validation_check_fruid_info(const guint8 *req_data, const void *msg_data, gpointer user_data)
{
    if (req_data == NULL) {
        debug_log(DLOG_ERROR, "get_ipmi_src_data failed");
        return RET_ERR;
    }
    if ((req_data[0] != 0xdb) || (req_data[1] != 0x07) || (req_data[2] != 0x00)) {
        return RET_ERR;
    }
    return RET_OK;
}


gint32 ipmi_oem_get_fruid_info(const void *msg_data, gpointer user_data)
{
    const guint8 *req_data = get_ipmi_src_data(msg_data);
    if (__validation_check_fruid_info(req_data, msg_data, user_data) != RET_OK) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }

    guint8 component_device_type = req_data[5];
    gint32 ret = is_component_type_support_fru(component_device_type);
    if (ret != RET_OK) {
        return ipmi_send_simple_response(msg_data, ret);
    }

    guint8 device_num = req_data[7]; 
    if ((component_device_type == DEVICE_TYPE_HDD_BACKPLANE) && ((device_num & 0xff) != CM_DEVICE_NUM_ALL)) {
        get_device_num_by_index(&device_num, device_num, component_device_type);
    }

    guint8 resp_len;

    
    guint8 group_id = req_data[6];

    
    guint8 read_offset = req_data[9];
    guint8 read_len = req_data[10];

    guint8 resp_data[DEVICE_INFO_LEN + 4] = {0};
    resp_data[0] = COMP_CODE_SUCCESS;
    resp_data[1] = 0xdb;
    resp_data[2] = 0x07;
    resp_data[3] = 0;
    if (component_device_type == CM_DEVICE_TYPE_ALL) {
        ret = __ipmi_get_fruid_by_device_type(read_offset, read_len, resp_data, &resp_len);
        if (ret != RET_OK) {
            return ipmi_send_simple_response(msg_data, COMP_CODE_OUTOF_RANGE);
        }

        return ipmi_send_response(msg_data, resp_len, resp_data);
    }

    if ((device_num & 0xff) == CM_DEVICE_NUM_ALL) { 
        ret = __ipmi_get_fruid_by_device_num(read_offset, read_len, group_id, component_device_type, resp_data,
            &resp_len);
        if (ret != RET_OK) {
            return ipmi_send_simple_response(msg_data, COMP_CODE_OUTOF_RANGE);
        }

        return ipmi_send_response(msg_data, resp_len, resp_data);
    }

    ret = __ipmi_get_fruid_by_binded_object(device_num, group_id, component_device_type, resp_data, &resp_len);
    if (ret != RET_OK) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_OUTOF_RANGE);
    }

    return ipmi_send_response(msg_data, resp_len, resp_data);
}


LOCAL gboolean is_aggregation_slave_power_fru(guint8 fru_type)
{
    if (dal_is_aggregation_management_chassis_product() == TRUE &&
        dal_is_aggregation_management_master_node() == FALSE && fru_type == PS_MODULE) {
        return TRUE;
    }
    return FALSE;
}

gint32 ipmi_read_frudata(const void *pdata, gpointer user_data)
{
    if (pdata == NULL) {
        return RET_ERR;
    }

    const READ_FRU_DATA_REQ_S *read_fru_data_req = (const READ_FRU_DATA_REQ_S *)get_ipmi_src_data(pdata);
    if (read_fru_data_req == NULL) {
        debug_log(DLOG_ERROR, "get_ipmi_src_data failed");
        return ipmi_send_simple_response(pdata, COMP_CODE_UNKNOWN);
    }

    
    if (is_support_fru_id(read_fru_data_req->fru_id) == FALSE) {
        debug_log(DLOG_ERROR, "%s: fruid invalid", __FUNCTION__);
        return ipmi_send_simple_response(pdata, COMP_CODE_OUTOF_RANGE);
    }

    
    FRU_PRIV_PROPERTY_S *fru_priv = NULL;
    OBJ_HANDLE fru_handle = 0;
    gint32 ret = get_fru_priv_data_by_id(read_fru_data_req->fru_id, &fru_priv, &fru_handle);
    if (ret != RET_OK || fru_priv == NULL) {
        debug_log(DLOG_ERROR, "%s: call get_backup_label_by_id failed, error = %d", __FUNCTION__, ret);
        return ipmi_send_simple_response(pdata, COMP_CODE_DES_UNAVAILABLE);
    }

    guint8 response[MAX_READ_FRU_DATA_LEN + 2] = {0};
    if (fru_priv->fru_property.fru_type == AMC_CONTROLLER) {
        put_obj_priv_data(fru_handle, (gpointer)fru_priv);
        return ipmi_send_simple_response(pdata, COMP_CODE_SUCCESS);
    }

    if (fru_priv->fru_property.fru_type != MMC_CONTROLLER &&
        is_aggregation_slave_power_fru(fru_priv->fru_property.fru_type) == FALSE) {
        return __get_other_fru_info(pdata, fru_priv, read_fru_data_req, response, sizeof(response));
    }

    guint32 req_len = get_ipmi_src_data_len(pdata);
    guint32 resp_len = MAX_READ_FRU_DATA_LEN + 2;
    ret = send_peripheral_elabel_cmd(fru_priv->fru_property.rimm_obj_handle, (const guint8 *)pdata,
        req_len + get_ipmi_head_len(), response, &resp_len);
    put_obj_priv_data(fru_handle, (gpointer)fru_priv);
    if (ret != 0) {
        return ipmi_send_simple_response(pdata, COMP_CODE_UNKNOWN);
    }

    debug_log(DLOG_DEBUG, "fru_id %d response[0] %d", read_fru_data_req->fru_id, response[0]);

    return ipmi_send_response(pdata, resp_len, response);
}

gint32 ipmi_write_frudata(const void *pdata, gpointer user_data)
{
    return ipmi_send_simple_response(pdata, COMP_CODE_INVALID_CMD);
}

gint32 ipmi_get_fru_inventory(const void *pdata, gpointer user_data)
{
    if (pdata == NULL) {
        return RET_ERR;
    }

    const GET_FRU_AREA_REQ_S *fru_area_req = (const GET_FRU_AREA_REQ_S *)get_ipmi_src_data(pdata);
    if (fru_area_req == NULL) {
        debug_log(DLOG_ERROR, "get_ipmi_src_data failed");
        return ipmi_send_simple_response(pdata, COMP_CODE_UNKNOWN);
    }

    
    if (is_support_fru_id(fru_area_req->fru_id) == FALSE) {
        debug_log(DLOG_ERROR, "%s: fruid invalid", __FUNCTION__);
        return ipmi_send_simple_response(pdata, COMP_CODE_OUTOF_RANGE);
    }

    
    FRU_PRIV_PROPERTY_S *fru_priv = NULL;
    OBJ_HANDLE fru_handle = 0;
    gint32 ret = get_fru_priv_data_by_id(fru_area_req->fru_id, &fru_priv, &fru_handle);
    if ((ret != RET_OK) || (fru_priv == NULL)) {
        debug_log(DLOG_ERROR, "%s: call get_backup_label_by_id failed, error = %d", __FUNCTION__, ret);
        return ipmi_send_simple_response(pdata, COMP_CODE_DES_UNAVAILABLE);
    }

    if (fru_priv->fru_property.fru_type == AMC_CONTROLLER) {
        
        put_obj_priv_data(fru_handle, (gpointer)fru_priv);
        return ipmi_send_simple_response(pdata, COMP_CODE_SUCCESS);
    }

    GET_FRU_AREA_RESP_S resp = { 0 };
    resp.completion = COMP_CODE_SUCCESS;
    resp.size = fru_priv->fru_property.frudev_inv_size;
    resp.reserve = 0;
    resp.word_access = FALSE;
    put_obj_priv_data(fru_handle, (gpointer)fru_priv);
    return ipmi_send_response(pdata, sizeof(GET_FRU_AREA_RESP_S), (guint8 *)&resp);
}

LOCAL void __ipmi_clear_fru_priv(FRU_PRIV_PROPERTY_S *fru_priv)
{
    guchar soft_ware_type = MGMT_SOFTWARE_TYPE_UNKNOW;
    (void)dal_get_software_type(&soft_ware_type);
    if (soft_ware_type == MGMT_SOFTWARE_TYPE_EM) {
        if (fru_priv->fru_property.fru_type == FRU_TYPE_BACKPLANE) {
            (void)memset_s(&(fru_priv->elabel_info->chassis), sizeof(ELABEL_CHASSIS_AREA_S), 0,
                sizeof(ELABEL_CHASSIS_AREA_S));
        }
    } else {
        (void)memset_s(fru_priv->elabel_info, sizeof(ELABEL_INFO_S), 0, sizeof(ELABEL_INFO_S));
    }

    fru_priv->elabel_info->board.manufacture.len = (guint16)strlen(DEFAULT_MFG_HUAWEI);
    (void)strncpy_s((gchar *)(fru_priv->elabel_info->board.manufacture.data),
        sizeof(fru_priv->elabel_info->board.manufacture.data), DEFAULT_MFG_HUAWEI, strlen(DEFAULT_MFG_HUAWEI));
    fru_priv->elabel_info->board.manufacture.crc =
        arith_make_crc_checksum(0, (const guchar *)&(fru_priv->elabel_info->board.manufacture.len),
        (fru_priv->elabel_info->board.manufacture.len + sizeof(guint16)));

    fru_priv->elabel_info->board.file_id.len = (guint16)strlen(fru_priv->fru_property.fru_ver);
    (void)strncpy_s((gchar *)(fru_priv->elabel_info->board.file_id.data),
        sizeof(fru_priv->elabel_info->board.file_id.data), fru_priv->fru_property.fru_ver,
        sizeof(fru_priv->elabel_info->board.file_id.data) - 1);
    fru_priv->elabel_info->board.file_id.crc =
        arith_make_crc_checksum(0, (const guchar *)&(fru_priv->elabel_info->board.file_id.len),
        (fru_priv->elabel_info->board.file_id.len + sizeof(guint16)));

    fru_priv->elabel_info->product.manufacture.len = (guint16)strlen(DEFAULT_MFG_HUAWEI);
    (void)strncpy_s((gchar *)(fru_priv->elabel_info->product.manufacture.data),
        sizeof(fru_priv->elabel_info->product.manufacture.data), DEFAULT_MFG_HUAWEI, strlen(DEFAULT_MFG_HUAWEI));
    fru_priv->elabel_info->product.manufacture.crc =
        arith_make_crc_checksum(0, (const guchar *)&(fru_priv->elabel_info->product.manufacture.len),
        (fru_priv->elabel_info->product.manufacture.len + sizeof(guint16)));

    fru_priv->elabel_info->product.file_id.len = (guint16)strlen(fru_priv->fru_property.fru_ver);
    (void)strncpy_s((gchar *)(fru_priv->elabel_info->product.file_id.data),
        sizeof(fru_priv->elabel_info->product.file_id.data), fru_priv->fru_property.fru_ver,
        sizeof(fru_priv->elabel_info->product.file_id.data) - 1);
    fru_priv->elabel_info->product.file_id.crc =
        arith_make_crc_checksum(0, (const guchar *)&(fru_priv->elabel_info->product.file_id.len),
        (fru_priv->elabel_info->product.file_id.len + sizeof(guint16)));

    
    if (fru_priv->fru_property.frudev_type == STORAGE_TYPE_MCU) {
        fru_priv->fru_property.dft_operate_type = MCU_CLEAR_ELABEL;
    }
}

LOCAL void clear_fru_priv_data(OBJ_HANDLE fru_handle, FRU_PRIV_PROPERTY_S *fru_priv)
{
    debug_log(DLOG_INFO, "begin to clear eeprom for %s", dfl_get_object_name(fru_priv->fru_property.frudev_handle));

    __ipmi_clear_fru_priv(fru_priv);

    (void)vos_thread_sem4_v(fru_priv->elabel_syn_semid);
    debug_log(DLOG_INFO, "clear eeprom end for %s", dfl_get_object_name(fru_priv->fru_property.frudev_handle));

    clear_mezz_mac(fru_priv);

    e2p_cafe_clear_all_record(fru_priv->cafe_info);

    (void)clear_chassis_fru_in_raw_eeprom(fru_handle, fru_priv);

    if (fru_priv->fru_property.is_cont_sys_area != 0) {
        (void)memset_s(fru_priv->system_info, sizeof(DATA_LENGTH_S) * SYS_AREA_ELABEL_NUM, 0,
            sizeof(DATA_LENGTH_S) * SYS_AREA_ELABEL_NUM);
        (void)vos_thread_sem4_v(fru_priv->update_sys_syn_semid);
    }
}

gint32 ipmi_dft_clear_elabel(const void *pdata, gpointer user_data)
{
    if (pdata == NULL) {
        debug_log(DLOG_ERROR, "input param error, pdata is NULL");
        return RET_ERR;
    }

    const UPDATE_CLEAR_ELABEL_REQ_S *dft_clear_elabel_req = (const UPDATE_CLEAR_ELABEL_REQ_S *)get_ipmi_src_data(pdata);
    if (dft_clear_elabel_req == NULL) {
        debug_log(DLOG_ERROR, "get_ipmi_src_data failed.");
        return ipmi_send_simple_response(pdata, COMP_CODE_UNKNOWN);
    }

    
    if (is_support_fru_id(dft_clear_elabel_req->fru_id) == FALSE) {
        debug_log(DLOG_ERROR, "%s: fruid invalid", __FUNCTION__);
        ipmi_operation_log(pdata, "Clear fru%d E-label data failed", dft_clear_elabel_req->fru_id);
        return ipmi_send_simple_response(pdata, COMP_CODE_OUTOF_RANGE);
    }

    
    FRU_PRIV_PROPERTY_S *fru_priv = NULL;
    OBJ_HANDLE fru_handle = 0;
    gint32 ret = get_fru_priv_data_by_id(dft_clear_elabel_req->fru_id, &fru_priv, &fru_handle);
    if ((ret != RET_OK) || (fru_priv == NULL)) {
        debug_log(DLOG_ERROR, "%s: call get_backup_label_by_id failed, error = %d", __FUNCTION__, ret);
        ipmi_operation_log(pdata, "Clear fru%d E-label data failed", dft_clear_elabel_req->fru_id);
        return ipmi_send_simple_response(pdata, COMP_CODE_DES_UNAVAILABLE);
    }

    if ((dft_clear_elabel_req->option != FRUDATA_START_COMMAND) &&
        (dft_clear_elabel_req->option != FRUDATA_GET_STATUS)) {
        put_obj_priv_data(fru_handle, (gpointer)fru_priv);
        ipmi_operation_log(pdata, "Clear fru%d E-label data failed", dft_clear_elabel_req->fru_id);
        return ipmi_send_simple_response(pdata, COMP_CODE_INVALID_FIELD);
    }

    if (!check_support_elabel(fru_priv->fru_property.fru_type)) {
        put_obj_priv_data(fru_handle, (gpointer)fru_priv);
        return ipmi_send_simple_response(pdata, COMP_CODE_STATUS_INVALID);
    }

    if (fru_priv->fru_property.fru_type == AMC_CONTROLLER) {
        
        put_obj_priv_data(fru_handle, (gpointer)fru_priv);
        ipmi_operation_log(pdata, "Clear fru%d E-label data successfully", dft_clear_elabel_req->fru_id);
        return ipmi_send_simple_response(pdata, COMP_CODE_SUCCESS);
    }

    UPDATE_CLEAR_ELABEL_RESP_S resp_data = { 0 };
    if (fru_priv->fru_property.fru_type == MMC_CONTROLLER) {
        
        guint32 req_len = get_ipmi_src_data_len(pdata);
        guint32 resp_len = 3;
        ret = send_peripheral_elabel_cmd(fru_priv->fru_property.rimm_obj_handle, (const guint8 *)pdata,
            req_len + get_ipmi_head_len(), (guint8 *)&resp_data, &resp_len);
        if ((ret == RET_OK) && (resp_data.comp_code == COMP_CODE_SUCCESS)) {
            ipmi_operation_log(pdata, "Clear fru%d E-label data successfully", dft_clear_elabel_req->fru_id);
        } else {
            ipmi_operation_log(pdata, "Clear fru%d E-label data failed", dft_clear_elabel_req->fru_id);
        }
        put_obj_priv_data(fru_handle, (gpointer)fru_priv);
        return ipmi_send_response(pdata, 2, (guint8 *)&resp_data);
    }

    
    if (fru_priv->fru_property.is_sup_dft == 0) {
        put_obj_priv_data(fru_handle, (gpointer)fru_priv);
        ipmi_operation_log(pdata, "Clear fru%d E-label data failed", dft_clear_elabel_req->fru_id);
        return ipmi_send_simple_response(pdata, COMP_CODE_OUTOF_RANGE);
    }

    clear_fru_priv_data(fru_handle, fru_priv);

    ipmi_operation_log(pdata, "Clear fru%d E-label data successfully", dft_clear_elabel_req->fru_id);
    put_obj_priv_data(fru_handle, (gpointer)fru_priv);
    resp_data.comp_code = COMP_CODE_SUCCESS;
    resp_data.progress = SYN_UPDAT_COMPLETED;
    return ipmi_send_response(pdata, 2, (guint8 *)&resp_data);
}

LOCAL gboolean check_fru_id_support_write(const DFT_WRITE_ELABEL_REQ_S *dft_write_elabel_req)
{
#define TEMP_TIANCHI_EXU_FRUID 64
#define TEMP_TIANCHI_STU_FRUID 67
#define TIANCHI_BMC_FRUID 80
    // 临时方案导致电子标签写入失败，需要临时过滤天池组件FruId
    if (dft_write_elabel_req->fru_id >= TEMP_TIANCHI_EXU_FRUID &&
        dft_write_elabel_req->fru_id <= TEMP_TIANCHI_STU_FRUID) {
        return TRUE;
    }

    if (dft_write_elabel_req->area == FRU_AREA_EXTENDELABLE &&
        !is_support_extern_lable(dft_write_elabel_req->fru_id)) {
        debug_log(DLOG_ERROR, "%s:fru_id = [%d] fruid is not support extern lable, ", __FUNCTION__,
            dft_write_elabel_req->fru_id);
        return FALSE;
    }
    return TRUE;
}

LOCAL gboolean check_fru_id_support_read(const DFT_READ_ELABEL_REQ_S *dft_read_elabel_req)
{
    // 临时方案导致电子标签读取失败，需要临时过滤天池组件FruId
    if (dft_read_elabel_req->fru_id >= TEMP_TIANCHI_EXU_FRUID &&
        dft_read_elabel_req->fru_id <= TEMP_TIANCHI_STU_FRUID) {
        return TRUE;
    }

    if ((dft_read_elabel_req->area == FRU_AREA_EXTENDELABLE || dft_read_elabel_req->area == FRU_INTERNAL_EXTEND_AREA ||
        dft_read_elabel_req->area == FRU_CHASSIS_EXTEND_AREA || dft_read_elabel_req->area == FRU_PRODUCT_EXTEND_AREA) &&
        !is_support_extern_lable(dft_read_elabel_req->fru_id)) {
        debug_log(DLOG_ERROR, "%s:fru_id = [%d] fruid is not support extern lable, ", __FUNCTION__,
            dft_read_elabel_req->fru_id);
        return FALSE;
    }
    return TRUE;
}

LOCAL gint32 write_elabel_by_fru_type(const void *pdata, FRU_PRIV_PROPERTY_S *fru_priv,
    const DFT_WRITE_ELABEL_REQ_S *dft_write_elabel_req, guint8 req_len, const gchar *buf)
{
    gint32 ret;

    if (fru_priv->fru_property.fru_type == AMC_CONTROLLER) {
        
        ipmi_operation_log(pdata, "Write fru%d E-label data(RAW:%s) successed", dft_write_elabel_req->fru_id, buf);
        return ipmi_send_simple_response(pdata, COMP_CODE_SUCCESS);
    }

    if (fru_priv->fru_property.fru_type == MMC_CONTROLLER) {
        
        guint32 resp_len = 2 + MAX_READ_FRU_DATA_LEN;
        req_len = get_ipmi_src_data_len(pdata);
        guint8 resp_data[MAX_READ_FRU_DATA_LEN + 2] = {0};
        ret = send_peripheral_elabel_cmd(fru_priv->fru_property.rimm_obj_handle, (const guint8 *)pdata,
            req_len + get_ipmi_head_len(), resp_data, &resp_len);
        if ((ret == RET_OK) && (resp_data[0] == COMP_CODE_SUCCESS)) {
            ipmi_operation_log(pdata, "Write fru%d E-label data(RAW:%s) successed", dft_write_elabel_req->fru_id, buf);
            return ipmi_send_response(pdata, resp_len, resp_data);
        }

        ipmi_operation_log(pdata, "Write fru%d E-label data(RAW:%s) failed", dft_write_elabel_req->fru_id, buf);
        return ipmi_send_simple_response(pdata, COMP_CODE_UNKNOWN);
    }

    
    if ((!fru_priv->fru_property.is_sup_dft) ||
        ((!fru_priv->fru_property.is_cont_sys_area) && (dft_write_elabel_req->area == FRU_AREA_SYSTEMINFO))) {
        ipmi_operation_log(pdata, "Write fru%d E-label data(RAW:%s) failed", dft_write_elabel_req->fru_id, buf);
        return ipmi_send_simple_response(pdata, COMP_CODE_OUTOF_RANGE);
    }

    ret = update_elabel_info(fru_priv, dft_write_elabel_req, req_len);
    if (ret == RET_OK) {
        ipmi_operation_log(pdata, "Write fru%d E-label data(RAW:%s) successed", dft_write_elabel_req->fru_id, buf);
    } else {
        ipmi_operation_log(pdata, "Write fru%d E-label data(RAW:%s) failed", dft_write_elabel_req->fru_id, buf);
    }

    return ipmi_send_simple_response(pdata, ret);
}

gint32 ipmi_dft_write_elabel(const void *pdata, gpointer user_data)
{
    if (pdata == NULL) {
        debug_log(DLOG_ERROR, "input param error, pdata is NULL");
        return RET_ERR;
    }

    const DFT_WRITE_ELABEL_REQ_S *dft_write_elabel_req = (const DFT_WRITE_ELABEL_REQ_S *)get_ipmi_src_data(pdata);
    if (dft_write_elabel_req == NULL) {
        debug_log(DLOG_ERROR, "get_ipmi_src_data failed");
        return ipmi_send_simple_response(pdata, COMP_CODE_UNKNOWN);
    }

    guint8 req_len = get_ipmi_src_data_len(pdata);
    gchar buf[MAX_LOG_BUF_SIZE] = {0};
    __log_dft_write(buf, req_len, dft_write_elabel_req);

    
    if ((is_support_fru_id(dft_write_elabel_req->fru_id) == FALSE) ||
        (!check_fru_id_support_write(dft_write_elabel_req))) {
        debug_log(DLOG_ERROR, "%s: fruid invalid, fru_id = %d", __FUNCTION__, dft_write_elabel_req->fru_id);
        ipmi_operation_log(pdata, "Write fru%d E-label data(RAW:%s) failed", dft_write_elabel_req->fru_id, buf);
        return ipmi_send_simple_response(pdata, COMP_CODE_OUTOF_RANGE);
    }

    
    FRU_PRIV_PROPERTY_S *fru_priv = NULL;
    OBJ_HANDLE fru_handle = 0;
    gint32 ret = get_fru_priv_data_by_id(dft_write_elabel_req->fru_id, &fru_priv, &fru_handle);
    if (ret != RET_OK || fru_priv == NULL) {
        debug_log(DLOG_ERROR, "%s: call get_backup_label_by_id failed, error = %d", __FUNCTION__, ret);
        ipmi_operation_log(pdata, "Write fru%d E-label data(RAW:%s) failed", dft_write_elabel_req->fru_id, buf);
        ret = ipmi_send_simple_response(pdata, COMP_CODE_DES_UNAVAILABLE);
        return ret;
    }

    if (!check_support_elabel(fru_priv->fru_property.fru_type)) {
        put_obj_priv_data(fru_handle, (gpointer)fru_priv);
        return ipmi_send_simple_response(pdata, COMP_CODE_STATUS_INVALID);
    }

    ret = write_elabel_by_fru_type(pdata, fru_priv, dft_write_elabel_req, req_len, buf);
    put_obj_priv_data(fru_handle, (gpointer)fru_priv);
    return ret;
}

LOCAL gint32 read_elabel_by_fru_type(const void *pdata, FRU_PRIV_PROPERTY_S *fru_priv,
    const DFT_READ_ELABEL_REQ_S *dft_read_elabel_req)
{
    guint8 end_flag = 0;
    guint32 resp_len = 0;
    guint8 resp_data[MAX_READ_FRU_DATA_LEN + 2] = {0};
    gint32 ret;

    if (fru_priv->fru_property.fru_type == AMC_CONTROLLER) {
        
        return ipmi_send_simple_response(pdata, COMP_CODE_SUCCESS);
    }

    if (fru_priv->fru_property.fru_type == MMC_CONTROLLER) {
        
        guint32 req_len = get_ipmi_src_data_len(pdata);
        resp_len = MAX_READ_FRU_DATA_LEN + 2;
        ret = send_peripheral_elabel_cmd(fru_priv->fru_property.rimm_obj_handle, (const guint8 *)pdata,
            req_len + get_ipmi_head_len(), resp_data, &resp_len);
        if (ret != 0) {
            return ipmi_send_simple_response(pdata, COMP_CODE_UNKNOWN);
        }

        return ipmi_send_response(pdata, resp_len, resp_data);
    }

    
    if (!fru_priv->fru_property.is_sup_dft) {
        debug_log(DLOG_ERROR, "%s: Not support dft", __FUNCTION__);
        return ipmi_send_simple_response(pdata, COMP_CODE_OUTOF_RANGE);
    }

    ret = read_elabel_info(&resp_data[2], sizeof(resp_data) - 2, &resp_len, &end_flag, fru_priv, dft_read_elabel_req);
    if (ret != COMP_CODE_SUCCESS) {
        return ipmi_send_simple_response(pdata, ret);
    }

    resp_data[0] = COMP_CODE_SUCCESS;
    resp_data[1] = end_flag;
    return ipmi_send_response(pdata, 2 + resp_len, resp_data);
}

gint32 ipmi_dft_read_elabel(const void *pdata, gpointer user_data)
{
    if (pdata == NULL) {
        return RET_ERR;
    }

    const DFT_READ_ELABEL_REQ_S *dft_read_elabel_req = (const DFT_READ_ELABEL_REQ_S *)get_ipmi_src_data(pdata);
    if (dft_read_elabel_req == NULL) {
        debug_log(DLOG_ERROR, "get_ipmi_src_data failed");
        return ipmi_send_simple_response(pdata, COMP_CODE_UNKNOWN);
    }

    
    if ((is_support_fru_id(dft_read_elabel_req->fru_id) == FALSE) ||
        (!check_fru_id_support_read(dft_read_elabel_req))) {
        debug_log(DLOG_ERROR, "%s: fruid invalid", __FUNCTION__);
        return ipmi_send_simple_response(pdata, COMP_CODE_OUTOF_RANGE);
    }

    
    FRU_PRIV_PROPERTY_S *fru_priv = NULL;
    OBJ_HANDLE fru_handle = 0;
    gint32 ret = get_fru_priv_data_by_id(dft_read_elabel_req->fru_id, &fru_priv, &fru_handle);
    if ((ret != RET_OK) || (fru_priv == NULL)) {
        debug_log(DLOG_ERROR, "%s: call get_backup_label_by_id failed, error = %d", __FUNCTION__, ret);
        return ipmi_send_simple_response(pdata, COMP_CODE_DES_UNAVAILABLE);
    }

    if (!check_support_elabel(fru_priv->fru_property.fru_type)) {
        put_obj_priv_data(fru_handle, (gpointer)fru_priv);
        return ipmi_send_simple_response(pdata, COMP_CODE_STATUS_INVALID);
    }

    ret = read_elabel_by_fru_type(pdata, fru_priv, dft_read_elabel_req);
    put_obj_priv_data(fru_handle, (gpointer)fru_priv);
    return ret;
}

LOCAL gboolean get_and_check_fru_priv(const UPDATE_CLEAR_ELABEL_REQ_S *update_elabel_req, OBJ_HANDLE *fru_handle,
    FRU_PRIV_PROPERTY_S **fru_priv)
{
    FRU_PRIV_PROPERTY_S *fru_priv_tmp = NULL;
    gint32 ret = get_fru_priv_data_by_id(update_elabel_req->fru_id, &fru_priv_tmp, fru_handle);
    if ((ret != RET_OK) || (fru_priv_tmp == NULL)) {
        debug_log(DLOG_ERROR, "%s: call get_backup_label_by_id failed, error = %d", __FUNCTION__, ret);
        return FALSE;
    }

    *fru_priv = fru_priv_tmp;
    return TRUE;
}

LOCAL gboolean __check_support_option(guint8 option)
{
    if (option != FRUDATA_START_COMMAND && option != FRUDATA_GET_STATUS) {
        return FALSE;
    }
    return TRUE;
}

LOCAL void __frudata_updata_status(UPDATE_CLEAR_ELABEL_RESP_S *resp_data, FRU_PRIV_PROPERTY_S *fru_priv,
    gboolean updata_ok)
{
    resp_data->progress = fru_priv->update_status;
    if (updata_ok) {
        resp_data->result = UPDATA_ELABEL_OK;
        
        (void)vos_thread_sem4_v(fru_priv->update_syn_semid); 
        (void)dal_set_property_value_byte(fru_priv->fru_property.frudev_handle, PROPERTY_FRUDEV_HEALTH, FRU_HEALTH_OK,
            DF_NONE);
    } else {
        resp_data->result = (guint8)fru_priv->update_fault;
    }
    resp_data->comp_code = COMP_CODE_SUCCESS;
}

LOCAL gint32 __forward_ipmi_cmd(const void *pdata, FRU_PRIV_PROPERTY_S *fru_priv, guint8 fru_id)
{
    guint32 req_len = get_ipmi_src_data_len(pdata);
    guint32 resp_len = MAX_READ_FRU_DATA_LEN + 2;
    guint8 resp_ipmi_data[MAX_READ_FRU_DATA_LEN + 2] = {0};
    gint32 ret = send_peripheral_elabel_cmd(fru_priv->fru_property.rimm_obj_handle, (const guint8 *)pdata,
        req_len + get_ipmi_head_len(), resp_ipmi_data, &resp_len);
    if ((ret == RET_OK) && (resp_ipmi_data[0] == COMP_CODE_SUCCESS)) {
        ipmi_operation_log(pdata, "Update fru%d E-label data successfully", fru_id);
    } else {
        ipmi_operation_log(pdata, "Update fru%d E-label data failed", fru_id);
    }

    return ipmi_send_response(pdata, resp_len, resp_ipmi_data);
}

#define ARM_ENABLE_FLAG          1
#define RESPONSE_LEN_ARM_DISABLE 2
#define RESPONSE_LEN_ARM_ENABLE  3

LOCAL guint8 __get_ipmi_response_len(void)
{
    guchar prop_value = 0;

    
    gint32 ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_ARM, &prop_value);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: get PMEServiceConfig obj's ARMEnable failed.", __FUNCTION__);
        return RESPONSE_LEN_ARM_DISABLE;
    }

    
    if (prop_value == ARM_ENABLE_FLAG || dal_match_product_id(PRODUCT_ID_PANGEA_V6)) {
        return RESPONSE_LEN_ARM_ENABLE;
    }

    return RESPONSE_LEN_ARM_DISABLE;
}

LOCAL gint32 update_elabel_by_fru_type(const void *pdata, FRU_PRIV_PROPERTY_S *fru_priv,
    const UPDATE_CLEAR_ELABEL_REQ_S *update_elabel_req)
{
    UPDATE_CLEAR_ELABEL_RESP_S resp_data = { 0 };
    if (update_elabel_req->option == FRUDATA_GET_STATUS) {
        __frudata_updata_status(&resp_data, fru_priv, FALSE);
        guint8 response_len = __get_ipmi_response_len();
        
        ipmi_operation_log(pdata, "Update fru%d E-label data successfully", update_elabel_req->fru_id);
        return ipmi_send_response(pdata, response_len, (guint8 *)&resp_data);
    }

    if (fru_priv->fru_property.fru_type == AMC_CONTROLLER) {
        
        ipmi_operation_log(pdata, "Update fru%d E-label data successfully", update_elabel_req->fru_id);
        return ipmi_send_simple_response(pdata, COMP_CODE_SUCCESS);
    }

    if (fru_priv->fru_property.fru_type == MMC_CONTROLLER) {
        
        gint32 ret = __forward_ipmi_cmd(pdata, fru_priv, update_elabel_req->fru_id);
        return ret;
    }

    
    if (!fru_priv->fru_property.is_sup_dft) {
        ipmi_operation_log(pdata, "Update fru%d E-label data failed", update_elabel_req->fru_id);
        return ipmi_send_simple_response(pdata, COMP_CODE_OUTOF_RANGE);
    }

    sync_elabel_to_fru(fru_priv->fru_info, fru_priv->elabel_info);
    fru_priv->update_status = SYN_UPDATING;

    __frudata_updata_status(&resp_data, fru_priv, TRUE);
    ipmi_operation_log(pdata, "Update fru%d E-label data successfully", update_elabel_req->fru_id);

    return ipmi_send_response(pdata, sizeof(UPDATE_CLEAR_ELABEL_RESP_S) - 1, (guint8 *)&resp_data);
}

gint32 ipmi_dft_update_elabel(const void *pdata, gpointer user_data)
{
    if (pdata == NULL) {
        debug_log(DLOG_ERROR, "input param error, pdata is NULL");
        return RET_ERR;
    }
    const UPDATE_CLEAR_ELABEL_REQ_S *update_elabel_req = (const UPDATE_CLEAR_ELABEL_REQ_S *)get_ipmi_src_data(pdata);
    if (update_elabel_req == NULL) {
        debug_log(DLOG_ERROR, "get_ipmi_src_data failed");
        return ipmi_send_simple_response(pdata, COMP_CODE_UNKNOWN);
    }

    
    if (is_support_fru_id(update_elabel_req->fru_id) == FALSE) {
        debug_log(DLOG_ERROR, "%s: fruid invalid", __FUNCTION__);
        ipmi_operation_log(pdata, "Update fru%d E-label data failed", update_elabel_req->fru_id);
        return ipmi_send_simple_response(pdata, COMP_CODE_OUTOF_RANGE);
    }

    
    FRU_PRIV_PROPERTY_S *fru_priv = NULL;
    OBJ_HANDLE fru_handle = 0;
    if (!get_and_check_fru_priv(update_elabel_req, &fru_handle, &fru_priv)) {
        ipmi_operation_log(pdata, "Update fru%d E-label data failed", update_elabel_req->fru_id);
        return ipmi_send_simple_response(pdata, COMP_CODE_DES_UNAVAILABLE);
    }

    if (!check_support_elabel(fru_priv->fru_property.fru_type)) {
        put_obj_priv_data(fru_handle, (gpointer)fru_priv);
        return ipmi_send_simple_response(pdata, COMP_CODE_STATUS_INVALID);
    }

    if (!__check_support_option(update_elabel_req->option)) {
        put_obj_priv_data(fru_handle, (gpointer)fru_priv);
        ipmi_operation_log(pdata, "Update fru%d E-label data failed", update_elabel_req->fru_id);
        return ipmi_send_simple_response(pdata, COMP_CODE_INVALID_FIELD);
    }

    gint32 ret = update_elabel_by_fru_type(pdata, fru_priv, update_elabel_req);
    put_obj_priv_data(fru_handle, (gpointer)fru_priv);
    return ret;
}


LOCAL gint32 get_system_area_fru_id(guint8 *fru_id)
{
    OBJ_HANDLE obj_handle = 0;

    gint32 ret = dfl_get_object_handle(BMC_PRODUCT_NAME_APP, &obj_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "Get object handle failed.");
        return ret;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_PRODUCT_SYSTEM_AREA_FRU_ID, fru_id);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "Get fru id failed.");
    }

    return ret;
}


gint32 ipmi_dft_custom(const void *pdata, gpointer user_data)
{
    guint8 fru_id = 0;

    if (pdata == NULL) {
        return RET_ERR;
    }

    const CUSTOM_REQ_S *custom_req = (const CUSTOM_REQ_S *)get_ipmi_src_data(pdata);
    if (custom_req == NULL) {
        ipmi_operation_log(pdata, "Set customize failed");
        return ipmi_send_simple_response(pdata, COMP_CODE_INVALID_FIELD);
    }

    FRU_PRIV_PROPERTY_S *fru_priv = NULL;
    gint32 ret = get_system_area_fru_id(&fru_id);
    if (ret != RET_OK) {
        ipmi_operation_log(pdata, "Set customize failed");
        return ipmi_send_simple_response(pdata, COMP_CODE_DES_UNAVAILABLE);
    }

    OBJ_HANDLE fru_handle = 0;
    ret = get_fru_priv_data_by_id(fru_id, &fru_priv, &fru_handle);
    if ((ret != DFL_OK) || (fru_priv == NULL)) {
        debug_log(DLOG_ERROR, "%s: call get_fru_priv_data_by_id failed, error = %d", __FUNCTION__, ret);
        ipmi_operation_log(pdata, "Set customize failed");
        return ipmi_send_simple_response(pdata, COMP_CODE_DES_UNAVAILABLE);
    }

    guint8 flag = 0;
    
    for (size_t i = 0; i < sizeof(g_custom_message_table) / sizeof(CUSTOM_MESSAGE); ++i) {
        if (g_custom_message_table[i].custom_flag == custom_req->custom_flag) {
            flag = 1;
            ipmi_operation_log(pdata, "%s", g_custom_message_table[i].log_str);
            if (fru_priv->eeprom_format == EEPROM_FORMAT_TIANCHI) {
                fru_priv->system_info->tc_system_desc_st.is_customize = g_custom_message_table[i].is_customize;
            } else {
                fru_priv->system_info->system_area_st.is_customize = g_custom_message_table[i].is_customize;
            }
            break;
        }
    }

    if (flag == 0) {
        ipmi_operation_log(pdata, "Disable customize successfully");
        if (fru_priv->eeprom_format == EEPROM_FORMAT_TIANCHI) {
            fru_priv->system_info->tc_system_desc_st.is_customize = DEFAULT_PRODUCT_NAME;
        } else {
            fru_priv->system_info->system_area_st.is_customize = DEFAULT_PRODUCT_NAME;
        }
    }

    (void)vos_thread_sem4_v(fru_priv->update_sys_syn_semid);
    put_obj_priv_data(fru_handle, (gpointer)fru_priv);

    return ipmi_send_simple_response(pdata, COMP_CODE_SUCCESS);
}


gint32 ipmi_get_dft_custom_cmd(const void *pdata, gpointer user_data)
{
    const GET_CUSTOM_REQ_S *custom_req = NULL;
    guint8 resp_data[CUSTOMIZE_RSP_CNT] = {0};
    FRU_PRIV_PROPERTY_S *fru_priv = NULL;
    OBJ_HANDLE fru_handle = 0;
    gint32 ret = RET_OK;
    if (pdata == NULL) {
        return RET_ERR;
    }
    
    custom_req = (const GET_CUSTOM_REQ_S *)get_ipmi_src_data(pdata);
    if (custom_req == NULL) {
        return ipmi_send_simple_response(pdata, COMP_CODE_INVALID_FIELD);
    }

    
    ret = get_fru_priv_data_by_id(0, &fru_priv, &fru_handle);
    if ((ret != DFL_OK) || (fru_priv == NULL)) {
        debug_log(DLOG_ERROR, "%s: call get_fru_priv_data_by_id failed, error = %d!(line:%d)\r\n", __FUNCTION__, ret,
            __LINE__);
        return ipmi_send_simple_response(pdata, COMP_CODE_DES_UNAVAILABLE);
    }
    resp_data[0] = COMP_CODE_SUCCESS;
    resp_data[1] = RESERVED_VALUE;
    resp_data[GET_CUSTOMIZE_FLG_RSP] = (fru_priv->eeprom_format == EEPROM_FORMAT_TIANCHI) ?
        fru_priv->system_info->tc_system_desc_st.is_customize :
        fru_priv->system_info->system_area_st.is_customize;
    put_obj_priv_data(fru_handle, (gpointer)fru_priv);
    return ipmi_send_response(pdata, sizeof(resp_data), resp_data);
}


gint32 ipmi_dft_get_component_pos(const void *msg_data, gpointer user_data)
{
    const guint8 *req_data = get_ipmi_src_data(msg_data);
    if (req_data == NULL) {
        debug_log(DLOG_ERROR, "get_ipmi_src_data failed");
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }

    errno_t safe_fun_ret;
    gint32 result;
    guint8 data_len = get_ipmi_src_data_len(msg_data);
    guint8 position_type = req_data[1];
    DFT_COMPONENT_POSN_INFO_S position_info = { 0 };
    switch (position_type) {
        case TYPE_FRU_ID:
            if (data_len != 3) { 
                return ipmi_send_simple_response(msg_data, COMP_CODE_LEN_INVALID);
            }

            position_info.fru_id = req_data[2];
            result = get_position_info_by_fruid(&position_info);
            break;

        case TYPE_POSITION_ID:
            if (data_len != 6) { 
                return ipmi_send_simple_response(msg_data, COMP_CODE_LEN_INVALID);
            }
            safe_fun_ret = memmove_s(&position_info.position_id, sizeof(guint32), req_data + 2, sizeof(guint32));
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            }
            result = get_position_info_by_position_id(&position_info);
            break;
        case TYPE_SILK_TEXT:
            safe_fun_ret =
                memmove_s(position_info.silk_text, sizeof(position_info.silk_text), req_data + 2, data_len - 2);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            }
            result = get_position_info_by_silk(&position_info);
            break;
        default:
            return ipmi_send_simple_response(msg_data, COMP_CODE_OUTOF_RANGE);
    }

    if (result != RET_OK) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_OUTOF_RANGE);
    }

    guint8 response_data_len = (guint8)(1 + 1 + 4 + strlen(position_info.silk_text));

    guint8 resp_data[128] = {0};
    resp_data[0] = COMP_CODE_SUCCESS;
    safe_fun_ret = memmove_s(resp_data + 1, sizeof(resp_data) - 1, &position_info, sizeof(position_info));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }
    return ipmi_send_response(msg_data, response_data_len, resp_data);
}

gint32 ipmi_dft_get_comp_present(const void *msg_data, gpointer user_data)
{
    const guint8 *req_data = get_ipmi_src_data(msg_data);
    if (req_data == NULL) {
        debug_log(DLOG_ERROR, "get_ipmi_src_data failed");
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }

    guint8 comp_id = req_data[4];
    guint8 resp_data[5] = {0};
    resp_data[0] = COMP_CODE_SUCCESS;
    resp_data[1] = 0xdb;
    resp_data[2] = 0x07;
    resp_data[3] = 0;

    if (comp_id != SESA_COMPONENT_ID) {
        resp_data[4] = (guint8)get_fru_present_by_position((guint32)comp_id);
        return ipmi_send_response(msg_data, sizeof(resp_data), resp_data);
    }

    OBJ_HANDLE obj_handle = 0;
    gint32 ret = dfl_get_object_handle(OBJ_NAME_SESA_STATUS_MON, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: call get_comp_present failed, error = %d", __FUNCTION__, ret);
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_REG_VALUE, &resp_data[4]);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: call get_comp_present failed, error = %d", __FUNCTION__, ret);
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    return ipmi_send_response(msg_data, sizeof(resp_data), resp_data);
}


gint32 ipmi_compute_power_properties(const void *msg_data, gpointer user_data)
{
    return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_CMD);
}

gint32 ipmi_get_addr_info(const void *msg_data, gpointer user_data)
{
    const guint8 *req_data = get_ipmi_src_data(msg_data);
    guint8 data_len = get_ipmi_src_data_len(msg_data);
    guint8 fruid = 0;
    if (data_len != 1) {
        if (req_data == NULL) {
            debug_log(DLOG_ERROR, "req_data is NULL");
            return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
        }
        fruid = req_data[1];
    }

    
    if (is_support_fru_id(fruid) == FALSE) {
        debug_log(DLOG_ERROR, "%s: fruid invalid", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, COMP_CODE_OUTOF_RANGE);
    }

    
    FRU_PRIV_PROPERTY_S *fru_priv = NULL;
    OBJ_HANDLE fru_handle = 0;
    gint32 ret = get_fru_priv_data_by_id(fruid, &fru_priv, &fru_handle);
    if ((ret != DFL_OK) || (fru_priv == NULL)) {
        debug_log(DLOG_ERROR, "%s: call get_ipmb_info failed, error = %d", __FUNCTION__, ret);
        return ipmi_send_simple_response(msg_data, COMP_CODE_DES_UNAVAILABLE);
    }

    if (data_len != 1 && data_len != 2) {
        put_obj_priv_data(fru_handle, (gpointer)fru_priv);
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    guint8 resp_data[8] = {0};
    resp_data[0] = COMP_CODE_SUCCESS;
    resp_data[1] = PICMG_IDENTIFIER;
    resp_data[2] = fru_priv->fru_property.ipmb_addr >> 1;
    resp_data[3] = fru_priv->fru_property.ipmb_addr;
    if ((get_ipmi_target_type(msg_data) == IPMI_HOST) || (get_ipmi_target_type(msg_data) == IPMI_LAN)) {
        resp_data[3] = 0x20;
    }
    resp_data[4] = 0xff;                             
    resp_data[5] = fru_priv->fru_property.fru_id;    
    resp_data[6] = get_blade_location(fru_priv);     
    resp_data[7] = fru_priv->fru_property.site_type; 
    put_obj_priv_data(fru_handle, (gpointer)fru_priv);

    return ipmi_send_response(msg_data, 8, resp_data);
}

gint32 ipmi_get_device_locator_record_id(const void *msg_data, gpointer user_data)
{
    const guint8 *req_data = get_ipmi_src_data(msg_data);
    if (req_data == NULL) {
        debug_log(DLOG_ERROR, "get_ipmi_src_data failed");
        return ipmi_send_simple_response(msg_data, COMP_CODE_DES_UNAVAILABLE);
    }

    
    guint8 fru_id = req_data[1];
    if (is_support_fru_id(fru_id) == FALSE) {
        debug_log(DLOG_ERROR, "%s: fruid invalid", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, COMP_CODE_OUTOF_RANGE);
    }

    
    FRU_PRIV_PROPERTY_S *fru_priv = NULL;
    OBJ_HANDLE fru_handle = 0;
    gint32 ret = get_fru_priv_data_by_id(fru_id, &fru_priv, &fru_handle);
    if ((ret != DFL_OK) || (fru_priv == NULL)) {
        debug_log(DLOG_ERROR, "%s: call get_device_locator_record_id failed, error = %d", __FUNCTION__, ret);
        return ipmi_send_simple_response(msg_data, COMP_CODE_DES_UNAVAILABLE);
    }

    guint8 resp_data[4] = {0};
    resp_data[0] = COMP_CODE_SUCCESS;
    resp_data[1] = fru_id;
    resp_data[2] = LOW(fru_priv->fru_property.locator_record_id);
    resp_data[3] = HIGH(fru_priv->fru_property.locator_record_id);
    put_obj_priv_data(fru_handle, (gpointer)fru_priv);

    return ipmi_send_response(msg_data, 4, resp_data);
}


gint32 ipmi_fru_control_capabilities(const void *msg_data, gpointer user_data)
{
    const guint8 *req_data = get_ipmi_src_data(msg_data);
    if (req_data == NULL) {
        debug_log(DLOG_ERROR, "get_ipmi_src_data failed");
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }

    
    guint8 fru_id = req_data[1];
    if (is_support_fru_id(fru_id) == FALSE) {
        debug_log(DLOG_ERROR, "%s: fruid invalid", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, COMP_CODE_OUTOF_RANGE);
    }

    
    FRU_PRIV_PROPERTY_S *fru_priv = NULL;
    OBJ_HANDLE fru_handle = 0;
    gint32 ret = get_fru_priv_data_by_id(fru_id, &fru_priv, &fru_handle);
    if ((ret != DFL_OK) || (fru_priv == NULL)) {
        debug_log(DLOG_ERROR, "%s: call fru_control_capabilities failed, error = %d", __FUNCTION__, ret);
        return ipmi_send_simple_response(msg_data, COMP_CODE_DES_UNAVAILABLE);
    }

    guint8 resp_data[3] = {0};
    resp_data[0] = COMP_CODE_SUCCESS;
    resp_data[1] = fru_id;
    resp_data[2] = 1 << FRU_CONTROL_GRACEFUL_REBOOT;
    put_obj_priv_data(fru_handle, (gpointer)fru_priv);

    return ipmi_send_response(msg_data, 3, resp_data);
}

LOCAL gint32 get_port_info(guint8 port_id, LSW_SWITCH_PORT_INFO *port_info, GVariant **rx_crc_list)
{
    OBJ_HANDLE obj = 0;

    gint32 ret = dal_get_specific_object_uint32(CLASS_LSW_PORT_ATTR_LOCAL_CP, PROPERTY_LSW_LOGIC_PORT_NUM,
        port_id, &obj);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get switch port(%u) object failed ret(%d).", port_id, ret);
        return ret;
    }

    (void)dal_get_property_value_uint16(obj, PROPERTY_LSW_LINK_STATUS, &(port_info->link_status));
    (void)dal_get_property_value_uint32(obj, PROPERTY_LSW_SPEED, &(port_info->speed));
    (void)dal_get_property_value_uint32(obj, PROPERTY_LSW_DUPLEX, &(port_info->duplex));
    (void)dal_get_property_value_uint32(obj, PROPERTY_LSW_STP_STATE, &(port_info->stp_state));

    ret = dfl_get_property_value(obj, PROPERTY_LSW_RXCRC, rx_crc_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get property value failed ret=%d", ret);
    }

    return ret;
}


LOCAL gint32 get_lsw_switch_port_info(guint8 port_id, guint8 *resp, guint8 *len)
{
    LSW_SWITCH_PORT_INFO port_info = { 0 };
    port_info.link_status = 0xff;
    port_info.speed = 0xffff;
    port_info.duplex = 0xffff;
    port_info.stp_state = 0xffff;
    errno_t safe_fun_ret;
    guint8 start_index = 5; 
    GVariant *rx_crc_list = NULL;
    const guint32 *rx_array = NULL;
    gsize size = 0;

    if (resp == NULL) {
        debug_log(DLOG_ERROR, "resp is null");
        return RET_ERR;
    }

    gint32 ret = get_port_info(port_id, &port_info, &rx_crc_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get port(%u) info failed, ret(%d)", port_id, ret);
        return ret;
    }
    rx_array = (const guint32 *)g_variant_get_fixed_array(rx_crc_list, &size, sizeof(guint32));
    if ((rx_array == NULL) || (size != 2)) { 
        debug_log(DLOG_ERROR, "Get rx errors array  failed");
        g_variant_unref(rx_crc_list);
        return ret;
    }

    resp[start_index++] = port_info.link_status;
    resp[start_index++] = LONGB0(port_info.speed);
    resp[start_index++] = LONGB1(port_info.speed);
    resp[start_index++] = port_info.duplex;
    resp[start_index++] = port_info.stp_state;
    safe_fun_ret = memcpy_s(&resp[start_index], sizeof(guint64), rx_array, sizeof(guint64));
    g_variant_unref(rx_crc_list);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "memcpy_s failrd, ret = %d", safe_fun_ret);
        return safe_fun_ret;
    }

    *len = 26; 

    return RET_OK;
}

LOCAL gint32 ipmi_get_lsw_switch_port_info(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6],
    guint8 device_type, guint8 device_ori_type, guint8 device_num, guint8 max_num)
{
    if (device_type != COMPONENT_TYPE_LSW) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_OUTOF_RANGE);
    }

    guint8 resp_len = 0;
    gint32 ret = get_lsw_switch_port_info(device_num, resp_data, &resp_len);
    if (ret != RET_OK) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }
    return ipmi_send_response(msg_data, resp_len, resp_data);
}


gint32 ipmi_get_device_info(const void *msg_data, gpointer user_data)
{
    const guint8 *req_data = get_ipmi_src_data(msg_data);
    if (req_data == NULL) {
        debug_log(DLOG_ERROR, "get_ipmi_src_data failed");
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }

    guint8 max_mum = 0;
    guint8 device_type = req_data[4];
    gint32 ret = get_device_max_num(device_type, &max_mum);
    if (ret != RET_OK) {
        return ipmi_send_simple_response(msg_data, ret);
    }

    ret = check_device_type(device_type);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "The para is wrong, device_type = %d", device_type);
        return ipmi_send_simple_response(msg_data, COMP_CODE_OUTOF_RANGE);
    }

    guint8 device_num = req_data[5];
    guint8 cond_filed = req_data[6];
    gboolean is_device_num_in_range = check_device_num_range(device_type, device_num);
    if (is_device_num_in_range && device_num > max_mum && cond_filed != MAX_NUM) {
        debug_log(DLOG_ERROR, "%s The para is wrong, device_num = %d", __FUNCTION__, device_num);
        return ipmi_send_simple_response(msg_data, COMP_CODE_OUTOF_RANGE);
    }

    guint8 device_ori_type = 0;
    ret = correct_device_attri(&device_type, &device_ori_type, &device_num);
    if (ret != RET_OK) {
        return ipmi_send_simple_response(msg_data, ret);
    }

    guint8 resp_data[DEVICE_INFO_LEN + 6] = {0};
    resp_data[0] = COMP_CODE_SUCCESS;
    resp_data[1] = 0xdb;
    resp_data[2] = 0x07;
    resp_data[3] = 0;
    resp_data[4] = 0x00;
    resp_data[6] = 0x00;

    guint8 func_size = (guint8)(sizeof(g_get_device_info_func) / sizeof(GET_DEVICE_INFO_S));
    for (guint8 i = 0; i < func_size; i++) {
        if (g_get_device_info_func[i].cond_filed == cond_filed) {
            return g_get_device_info_func[i].exec_func(msg_data, resp_data, device_type, device_ori_type, device_num,
                max_mum);
        }
    }

    debug_log(DLOG_DEBUG, "%s cond_filed = %d", __FUNCTION__, cond_filed);
    return ipmi_send_simple_response(msg_data, COMP_CODE_OUTOF_RANGE);
}

LOCAL gint32 __set_blade_board_id(const guint8 *req_data, const void *msg_data, guint8 *resp_data, guint8 resp_data_len)
{
#define TYPE_LEN 2
#define PRE_LEN 6
    guint8 data_len = get_ipmi_src_data_len(msg_data);
    if (data_len < (g_max_slot_num)*TYPE_LEN + PRE_LEN) {
        debug_log(DLOG_ERROR, "blade board id info data lenth err.");
        ipmi_operation_log(msg_data, "Set blade info parameters failed");
        *resp_data = COMP_CODE_LEN_INVALID;
        return RET_ERR;
    }

    guint8 idx;
    for (guint8 uloop = 0; uloop < g_max_slot_num; uloop++) {
        idx = PRE_LEN + TYPE_LEN * uloop;
        g_blade_board_id[uloop] = req_data[idx] | (req_data[idx + 1] << 8);
    }

    return RET_OK;
}

LOCAL void __ipmi_set_blade_info_operation_log(const void *msg_data)
{
    ipmi_operation_log(msg_data,
        "Set blade boardId info parameters "
        "success,raw:[0x%x-0x%x-0x%x-0x%x-0x%x-0x%x-0x%x-0x%x-0x%x-0x%x-0x%x-0x%x-0x%x-0x%x-0x%x-0x%x]",
        g_blade_board_id[0], g_blade_board_id[1], g_blade_board_id[2], g_blade_board_id[3], g_blade_board_id[4],
        g_blade_board_id[5], g_blade_board_id[6], g_blade_board_id[7], g_blade_board_id[8], g_blade_board_id[9],
        g_blade_board_id[10], g_blade_board_id[11], g_blade_board_id[12], g_blade_board_id[13], g_blade_board_id[14],
        g_blade_board_id[15]);
}


gint32 ipmi_set_blade_info_parameters(const void *msg_data, gpointer user_data)
{
#define RESP_LEN 4
    if (msg_data == NULL) {
        return RET_ERR;
    }

    const guint8 *req_data = get_ipmi_src_data(msg_data);
    if (req_data == NULL) {
        debug_log(DLOG_ERROR, "Get req_data failed");
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    guint8 resp_data[RESP_LEN] = {0};
    OBJ_HANDLE obj_handle = 0;
    gint32 ret = dfl_get_object_handle(BMC_OBJECT_NAME, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get BMC object failed: %d.", ret);
        resp_data[0] = COMP_CODE_DES_UNAVAILABLE;
        ipmi_operation_log(msg_data, "Set blade info parameters failed");
        return ipmi_send_response(msg_data, 4, resp_data);
    }

    guint32 manufactureid = 0;
    (void)dal_get_property_value_uint32(obj_handle, BMC_MANU_ID_NAME, &manufactureid);

    guint32 imana = MAKE_DWORD(0, req_data[2], req_data[1], req_data[0]);
    if (imana != manufactureid) {
        debug_log(DLOG_ERROR, "Manufacture ID mismatched.");
        resp_data[0] = COMP_CODE_INVALID_FIELD;
        ipmi_operation_log(msg_data, "Set blade info parameters failed");
        return ipmi_send_response(msg_data, 4, resp_data);
    }

    resp_data[1] = LONGB0(manufactureid);
    resp_data[2] = LONGB1(manufactureid);
    resp_data[3] = LONGB2(manufactureid);

    guint8 para_selector = req_data[4];
    if (para_selector != BLADE_INFO_PARA_BOARDID) {
        resp_data[0] = COMP_CODE_DATA_NOT_AVAILABLE;
        ipmi_operation_log(msg_data, "Set blade info parameters failed");
        return ipmi_send_response(msg_data, 4, resp_data);
    }

    g_max_slot_num = req_data[5];
    if (g_max_slot_num > MAX_BLADE_SLOT_NUM) {
        g_max_slot_num = MAX_BLADE_SLOT_NUM;
    }

    if (__set_blade_board_id(req_data, msg_data, resp_data, RESP_LEN) != RET_OK) {
        return ipmi_send_response(msg_data, RESP_LEN, resp_data);
    }

    resp_data[0] = COMP_CODE_SUCCESS;
    __ipmi_set_blade_info_operation_log(msg_data);

    return ipmi_send_response(msg_data, 4, resp_data);
}


gint32 ipmi_get_blade_info_parameters(const void *msg_data, gpointer user_data)
{
    if (msg_data == NULL) {
        return RET_ERR;
    }

    const guint8 *req_data = get_ipmi_src_data(msg_data);
    if (req_data == NULL) {
        debug_log(DLOG_ERROR, "Get req_data failed!");
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    guint8 resp_data[255] = {0};
    OBJ_HANDLE obj_handle = 0;
    gint32 ret = dfl_get_object_handle(BMC_OBJECT_NAME, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get BMC object failed: %d.", ret);
        resp_data[0] = COMP_CODE_DES_UNAVAILABLE;
        return ipmi_send_response(msg_data, 4, resp_data);
    }

    guint32 manufactureid = 0;
    (void)dal_get_property_value_uint32(obj_handle, BMC_MANU_ID_NAME, &manufactureid);

    guint32 imana = MAKE_DWORD(0, req_data[2], req_data[1], req_data[0]);
    if (imana != manufactureid) {
        debug_log(DLOG_ERROR, "Manufacture ID mismatched.");
        resp_data[0] = COMP_CODE_INVALID_FIELD;
        return ipmi_send_response(msg_data, 4, resp_data);
    }

    resp_data[1] = LONGB0(manufactureid);
    resp_data[2] = LONGB1(manufactureid);
    resp_data[3] = LONGB2(manufactureid);
    guint8 para_selector = req_data[4];
    if (para_selector != BLADE_INFO_PARA_BOARDID) {
        resp_data[0] = COMP_CODE_DATA_NOT_AVAILABLE;
        return ipmi_send_response(msg_data, 4, resp_data);
    }

    resp_data[4] = g_max_slot_num;
    for (guint8 uloop = 0; uloop < g_max_slot_num; uloop++) {
        resp_data[5 + uloop * 2] = g_blade_board_id[uloop] & 0xff;
        resp_data[6 + uloop * 2] = (g_blade_board_id[uloop] >> 8) & 0xff;
    }

    resp_data[0] = COMP_CODE_SUCCESS;
    guint8 resp_len = 2 * g_max_slot_num + 5;
    return ipmi_send_response(msg_data, resp_len, resp_data);
}


LOCAL gint32 check_dft_status(void)
{
    gint32 ret = 0;
    guint8 value = 0;
    static OBJ_HANDLE dft_handle = 0;
    if (dft_handle == 0) {
        (void)dal_get_object_handle_nth(CLASS_DFT_STATUS, 0, &dft_handle);
    }

    
    ret = dal_get_property_value_byte(dft_handle, PROPERTY_DFT_MODE_FLAG, &value);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_property_value %s result=%d", __FUNCTION__, PROPERTY_DFT_MODE_FLAG, ret);
        return RET_ERR;
    }
    if (value != IN_DFT_MODE) {
        debug_log(DLOG_ERROR, "%s: dal_get_property_value %s=%hhu", __FUNCTION__, PROPERTY_DFT_MODE_FLAG, value);
        return RET_ERR;
    }

    return RET_OK;
}


gint32 ipmi_set_k_device_type(gconstpointer req_msg, gpointer user_data)
{
    gint32 ret = 0;
    const SET_DEVICE_TYPE_REQ_S *set_device_req = NULL;
    SET_DEVICE_TYPE_RESP_S set_device_resp = { 0 };
    guint8 comp_code = COMP_CODE_INVALID_FIELD;

    // 判断是否是装备模式
    ret = check_dft_status();
    if (ret != RET_OK) {
        goto IPMI_SET_DEVICE_TYPE_ERR;
    }

    set_device_req = (const SET_DEVICE_TYPE_REQ_S *)get_ipmi_src_data(req_msg);
    if (set_device_req == NULL) {
        goto IPMI_SET_DEVICE_TYPE_ERR;
    }
    if (set_device_req->manufacturer != HUAWEI_MFG_ID) {
        goto IPMI_SET_DEVICE_TYPE_ERR;
    }
    guint8 msg_data_len = get_ipmi_src_data_len(req_msg);
    if (set_device_req->length != (msg_data_len - (guint8)SET_DEVICE_TYPE_REQ_FIXED_LEN)) {
        debug_log(DLOG_ERROR, "%s: src data len wrong, req_len=%d, ipmi_data_len=%d",
            __FUNCTION__, set_device_req->length, msg_data_len - (guint8)SET_DEVICE_TYPE_REQ_FIXED_LEN);
        goto IPMI_SET_DEVICE_TYPE_ERR;
    }

    ret = ipmi_cmd_set_device_type_str(req_msg, set_device_req, DEVICE_TYPE_LEN_LIMIT);
    if (ret != RET_OK) {
        comp_code = ret;
        goto IPMI_SET_DEVICE_TYPE_ERR;
    }

    set_device_resp.comp_code = COMP_CODE_SUCCESS;
    set_device_resp.manufacturer = set_device_req->manufacturer;

    return ipmi_send_response(req_msg, sizeof(set_device_resp), (guint8 *)&set_device_resp);

IPMI_SET_DEVICE_TYPE_ERR:
    ipmi_operation_log(req_msg, "Set Device Type failed");
    return ipmi_send_simple_response(req_msg, comp_code);
}


LOCAL gint32 __ipmi_cmd_get_sdicard_reset_linkage(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6],
    guint8 device_type, guint8 device_ori_type, guint8 device_num, guint8 max_num)
{
#define LAST_FRAME_FLAG_INDEX 4
#define RESET_LINKAGE_POLICY_INDEX 5
    guint8 reset_linkage_mode = 0;
    gint32 ret;

    if (device_ori_type != DEVICE_TYPE_SDI_CARD) {
        debug_log(DLOG_ERROR, "%s device_type = %d", __FUNCTION__, device_type);
        return ipmi_send_simple_response(msg_data, COMP_CODE_CMD_INVALID);
    }
    ret = get_sdi_reset_linkage_info(device_ori_type, device_num, &reset_linkage_mode);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s get_sdi_reset_linkage_info fail, ret = %d", __FUNCTION__, ret);
        return ipmi_send_simple_response(msg_data, ret);
    }

    resp_data[LAST_FRAME_FLAG_INDEX] = 0;   // 0代表是最后一帧
    resp_data[RESET_LINKAGE_POLICY_INDEX] = reset_linkage_mode;
    const guint8 resp_len = 6;

    return ipmi_send_response(msg_data, resp_len, resp_data);
}
