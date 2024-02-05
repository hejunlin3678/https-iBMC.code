

#include "redfish_provider.h"
#include "redfish_provider_with_cucc_customized.h"
#include "redfish_provider_chassis_pciedevices.h"


#define CHIP_NUM_NOT_SUPPORT_D_CARD 1

LOCAL gint32 get_chassis_pciedevices_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_pciedevices_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_pciedevices_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_pciedevices_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_pciedevices_description(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_pciedevices_manufacturer(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_pciedevices_model(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_pciedevices_serialnumber(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_pciedevices_firmwareversion(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_pciedevices_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_pciedevices_oem_property(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_chassis_pciedevices_oem_property(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso);
LOCAL gint32 get_chassis_pciedevices_links(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_pciedevices_interface(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso);
LOCAL gint32 get_chassis_pciedevices_cmcc(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 set_huawei_alarm_indicator_led(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso);
LOCAL gint32 set_huawei_boot_option(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso);
LOCAL gint32 set_huawei_boot_effective(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso);

LOCAL gint32 get_pcie_slave_card_object_handle(OBJ_HANDLE pcie_obj_handle, OBJ_HANDLE *slave_card_obj_handle);
LOCAL void get_pcie_extend_info(OBJ_HANDLE obj_handle, json_object *huawei_jso);


LOCAL void get_pciecard_extend_info(OBJ_HANDLE obj_handle, json_object *huawei_jso);
LOCAL gint32 get_pciecard_extend_firmwareversion(OBJ_HANDLE obj_handle, json_object **o_result_jso);
LOCAL gint32 get_pciecard_extend_version_info(OBJ_HANDLE obj_handle, const gchar *request_prop,
    json_object **o_result_jso);


LOCAL void get_fpgacard_extend_info(OBJ_HANDLE obj_handle, json_object *huawei_jso);


LOCAL void get_sdicard_extend_info(OBJ_HANDLE obj_handle, json_object *huawei_jso);
LOCAL void get_sdicard_extend_m2_presence(OBJ_HANDLE obj_handle, json_object *huawei_jso);
LOCAL void get_sdicard_extend_sodimm_presence(OBJ_HANDLE obj_handle, json_object *huawei_jso);
LOCAL void get_sdicard_extend_storage_ip(OBJ_HANDLE obj_handle, json_object *huawei_jso);
LOCAL void get_sdicard_extend_storage_vlanid(OBJ_HANDLE obj_handle, json_object *huawei_jso);
LOCAL void get_sdicard_extend_boot_option(OBJ_HANDLE obj_handle, json_object *huawei_jso);
LOCAL void get_sdicard_extend_boot_option_valid_type(OBJ_HANDLE obj_handle, json_object *huawei_jso);
LOCAL gboolean is_sdi_sodimm_presence_support(json_object *property_jso);

LOCAL gboolean check_sdicard_extendcard_presence(OBJ_HANDLE sdi_obj_handle);
LOCAL void get_sdicard_extendcard_info(OBJ_HANDLE sdi_obj_handle, json_object *property_jso);
LOCAL void get_sdicard_extend_netcard_info(OBJ_HANDLE extend_netcard_obj_handle, json_object *property_jso);
LOCAL gint32 sort_extendnetcard_handle_by_slot(gconstpointer a, gconstpointer b);


LOCAL void get_sddcard_extend_info(OBJ_HANDLE obj_handle, json_object *huawei_jso);


LOCAL void get_acceleratecard_extend_info(OBJ_HANDLE obj_handle, json_object *huawei_jso);
LOCAL void get_acceleratecard_extend_bomid(OBJ_HANDLE obj_handle, json_object *huawei_jso);


LOCAL void get_m2transformcard_extend_info(OBJ_HANDLE obj_handle, json_object *huawei_jso);
LOCAL gint32 get_chassis_pciedevices_partnumber(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

typedef struct pcie_type_prop_pair {
    const guint8 type;
    const gchar *prop;
} PCIE_TYPE_PROP_PAIR;


LOCAL PCIE_EXTEND_INFO_PROVIDER g_pcie_extend_info_provider[] = {
    {CLASS_PCIE_CARD,           get_pciecard_extend_info},
    {CLASS_GPU_CARD,            NULL},
    {CLASS_PCIE_FPGA_CARD,      get_fpgacard_extend_info},
    {CLASS_PCIE_SDI_CARD,       get_sdicard_extend_info},
    {CLASS_PCIESSDCARD_NAME,    get_sddcard_extend_info},
    {CLASS_ACCLERATE_CARD,      get_acceleratecard_extend_info},
    {CLASS_M2TRANSFORMCARD,     get_m2transformcard_extend_info},
    {CLASS_OCP_CARD,            get_pciecard_extend_info},
    {CLASS_STORAGE_DPU_CARD,    get_storage_dpucard_extend_info}
};

LOCAL PCIE_HUAWEI_HANDLE_METHOD g_pcie_huawei_handle_method[] = {
    {RFPROP_PCIE_ALARM_INDICATELED, set_huawei_alarm_indicator_led},
    {RFPROP_PCIEDEVICES_BOOTOPTION, set_huawei_boot_option},
    {RFPROP_PCIEDEVICES_BOOT_EFFECTIVE, set_huawei_boot_effective}
};

LOCAL GET_SDICARD_BOOT_OPTION g_get_sdicard_boot_option[] = {
    {0, BOOTOPTION_HDD},
    {1, BOOTOPTION_BASE_BOARD_PXE_BOOT},
    {3, BOOTOPTION_BUCKLE_CARD1_PXE_BOOT},
    {5, BOOTOPTION_BUCKLE_CARD2_PXE_BOOT},
    {7, BOOTOPTION_BUCKLE_CARD3_PXE_BOOT},
    {255, BOOTOPTION_UNSET},
};

LOCAL GET_SDICARD_BOOT_VALID_TYPE g_get_sdicard_valid_type[] = {
    {0, BOOTORDER_ONCE},
    {1, BOOTORDER_PERMANENT}
};

LOCAL PROPERTY_PROVIDER_S g_chassis_pciedevices_provider[] = {
    {RFPROP_ODATA_CONTEXT,          MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_pciedevices_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_ID,               MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_pciedevices_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_ID,              MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_pciedevices_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_NAME,            MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_pciedevices_name, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_PCIEDEVICES_DESCRIPTION,    CLASS_PCIECARD_NAME, PROPERTY_PCIECARD_DESC, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_pciedevices_description, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_PCIEDEVICES_MANUFACTURER,   CLASS_PCIECARD_NAME, PROPETRY_PCIECARD_MANUFACTURER, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_pciedevices_manufacturer, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_PCIEDEVICES_MODEL,          MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_pciedevices_model, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_PCIEDEVICES_CARD_MANUFACTURER,          CLASS_PCIECARD_NAME, PROPETRY_PCIECARD_MANUFACTURER,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_pciedevices_cmcc, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_PCIEDEVICES_CARD_MODEL,          CLASS_PCIECARD_NAME, PROPERTY_PCIE_CARD_SLAVE_CARD_NAME,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_pciedevices_cmcc, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_PART_NUMBER,     MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        get_chassis_pciedevices_partnumber, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_PCIEDEVICES_SERIALNUMBER,   MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_pciedevices_serialnumber, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_PCIEDEVICES_FIRMWAREVERSION, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_pciedevices_firmwareversion, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_PCIEDEVICES_STATUS,         CLASS_PCIECARD_NAME, PROPERTY_PCIECARD_HEALTH, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_pciedevices_status, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_PCIEDEVICES_INTERFACE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        get_chassis_pciedevices_interface, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_OEM,             MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        get_chassis_pciedevices_oem_property, set_chassis_pciedevices_oem_property, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_PCIEDEVICES_LINKS,          MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_pciedevices_links, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ACTIONS, "", "", USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_pciedevices_srv_actions, NULL, NULL, ETAG_FLAG_DISABLE},
    {RFACTION_SET_PCIEDEVICES_NMI_ACTION, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_SECURITYMGNT, SYS_LOCKDOWN_FORBID, NULL, NULL, set_pciedevices_nmi_entry, ETAG_FLAG_DISABLE}
};


LOCAL void add_ome_property_firmware_version_from_mcu(OBJ_HANDLE json_obj_handle,  json_object** o_result_jso);
LOCAL void add_ome_property_reading_celsius_from_mcu(OBJ_HANDLE json_obj_handle, json_object* huawei_jso);
LOCAL void add_ome_property_power_consumed_watts_from_mcu(OBJ_HANDLE json_obj_handle, json_object* huawei_jso);
LOCAL void add_ome_property_video_memory_band_width_from_mcu(OBJ_HANDLE json_obj_handle, json_object* huawei_jso);
LOCAL void add_ome_property_rated_power_from_mcu(OBJ_HANDLE json_obj_handle, json_object* huawei_jso);
LOCAL void get_board_serialnumber(OBJ_HANDLE obj_handle, gchar *serialnumber, guint32 size);

LOCAL void add_ome_property_model(OBJ_HANDLE json_obj_handle, json_object** o_result_jso);


LOCAL gint32 redfish_check_pciecard_type_handle(OBJ_HANDLE i_obj_handle, guint8 *card_type, OBJ_HANDLE *o_obj_handle)
{
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guint8 functionclass = 0;
    OBJ_HANDLE component_handle = 0;
    OBJ_HANDLE controller_handle = 0;
    gchar pcie_ref_object[MAX_STRBUF_LEN] = {0};
    gchar pcie_ref_property[MAX_STRBUF_LEN] = {0};
    gchar slave_ref_object[MAX_STRBUF_LEN] = {0};
    gchar slave_ref_property[MAX_STRBUF_LEN] = {0};
    gchar com_devicename[MAX_STRBUF_LEN] = {0};
    gchar slave_name[MAX_STRBUF_LEN] = {0};

    gint32 ret = dal_get_property_value_string(i_obj_handle, PROPERTY_PCIE_CARD_SLAVE_CARD, slave_name,
        sizeof(slave_name));
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_MASS, "%s, %d: get slave card value fail.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_expr(0 == g_strcmp0(slave_name, CLASS_M2TRANSFORMCARD), VOS_ERR,
        debug_log(DLOG_MASS, "%s, %d: M2TransformCard is fail.", __FUNCTION__, __LINE__));

    
    ret = dal_get_property_value_byte(i_obj_handle, PROPERTY_PCIECARD_FUNCTIONCLASS, &functionclass);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_MASS, "%s, %d: get property string value fail.", __FUNCTION__, __LINE__));

    
    if (functionclass == PCIECARD_FUNCTION_RAID) {
        ret = redfish_get_pcie_component_obj_handle(i_obj_handle, &component_handle);
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            debug_log(DLOG_MASS, "%s, %d: get slave card value fail.", __FUNCTION__, __LINE__));

        ret = dal_get_property_value_string(component_handle, PROPERTY_COMPONENT_DEVICE_NAME, com_devicename,
            sizeof(com_devicename));
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            debug_log(DLOG_MASS, "%s, %d: get object list fail.", __FUNCTION__, __LINE__));

        ret = dal_get_specific_object_string(CLASS_RAID_CONTROLLER_NAME, PROPERTY_RAID_CONTROLLER_COMPONENT_NAME,
            com_devicename, &controller_handle);
        if (ret == VOS_OK) {
            *o_obj_handle = controller_handle;
            goto SUCCESS_RETURN;
        }
    } else if (functionclass == PCIECARD_FUNCTION_NETCARD) {
        
        ret = dal_get_specific_object_position(i_obj_handle, CLASS_NETCARD_NAME, o_obj_handle);
        return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get netcard_handle fail.", __FUNCTION__, __LINE__));
        goto SUCCESS_RETURN;
    } else if (functionclass == PCIECARD_FUNCTION_ACCELERATE) {
        
        ret = dal_get_specific_object_position(i_obj_handle, CLASS_ACCLERATE_CARD, o_obj_handle);
        return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get accelerate card fail, error code:%d.\r\n", __FUNCTION__, __LINE__,
            HTTP_INTERNAL_SERVER_ERROR));
        goto SUCCESS_RETURN;
    }
    
    else if (functionclass == PCIECARD_FUNCTION_GPU) {
        ret = dal_get_specific_object_position(i_obj_handle, CLASS_GPU_CARD, o_obj_handle);
        return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get gpu card fail, error code:%d.\r\n", __FUNCTION__, __LINE__,
            HTTP_INTERNAL_SERVER_ERROR));
        *card_type = PCIECARD_FUNCTION_GPU;
        return VOS_OK;
    } else if (functionclass == PCIECARD_FUNCTION_FPGA || functionclass == PCIECARD_FUNCTION_NPU) {
        ret = dal_get_specific_object_position(i_obj_handle, CLASS_PCIE_FPGA_CARD, o_obj_handle);
        return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get %s fail, error code:%d.\r\n", __FUNCTION__, __LINE__,
                (functionclass == PCIECARD_FUNCTION_FPGA) ? "fpga card" : "npu card", HTTP_INTERNAL_SERVER_ERROR));
        goto SUCCESS_RETURN;
    } else {
        
        ret = dfl_get_referenced_property(i_obj_handle, PROPETRY_PCIECARD_SLOT, pcie_ref_object, pcie_ref_property,
            MAX_STRBUF_LEN, MAX_STRBUF_LEN);
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            debug_log(DLOG_MASS, "%s, %d: get refer property fail.", __FUNCTION__, __LINE__));

        ret = dfl_get_object_list(slave_name, &obj_list);
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            debug_log(DLOG_MASS, "%s, %d: get object list fail.", __FUNCTION__, __LINE__));

        for (obj_node = obj_list; obj_node; obj_node = g_slist_next(obj_node)) {
            ret = dfl_get_referenced_property((OBJ_HANDLE)obj_node->data, "Slot", slave_ref_object, slave_ref_property,
                MAX_STRBUF_LEN, MAX_STRBUF_LEN);
            do_if_fail(VOS_OK == ret, continue);

            if (g_strcmp0(pcie_ref_object, slave_ref_object) == 0) {
                *o_obj_handle = (OBJ_HANDLE)obj_node->data;
                *card_type = functionclass;
                g_slist_free(obj_list);
                return VOS_OK;
            }
        }
        g_slist_free(obj_list);
    }
    return VOS_ERR;

SUCCESS_RETURN:
    *card_type = functionclass;
    return VOS_OK;
}

guint8 redfish_get_pciedevice_component_type(OBJ_HANDLE obj_handle)
{
    gchar class_name[MAX_NAME_SIZE] = {0};

    (void)dfl_get_class_name(obj_handle, class_name, MAX_NAME_SIZE);

    if (g_ascii_strcasecmp(class_name, CLASS_OCP_CARD) == 0) {
        return COMPONENT_TYPE_OCP;
    }
    return COMPONENT_TYPE_PCIE_CARD;
}

gint32 redfish_get_pcie_component_obj_handle(OBJ_HANDLE i_obj_handle, OBJ_HANDLE *o_handle)
{
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guint8 presence = 0;
    guint8 component_type = 0;
    guint8 object_component_type;
    OBJ_HANDLE comp_handle = 0;
    gchar comp_ref_object[MAX_NAME_SIZE] = {0};
    gchar ref_property[MAX_NAME_SIZE] = {0};
    gchar ref_ref_object[MAX_NAME_SIZE] = {0};
    gchar ref_ref_property[MAX_NAME_SIZE] = {0};
    gchar pcie_ref_obj[MAX_NAME_SIZE] = {0};
    gchar pcie_ref_prop[MAX_NAME_SIZE] = {0};

    
    gint32 ret = dfl_get_referenced_property(i_obj_handle, PROPETRY_PCIECARD_SLOT, pcie_ref_obj, pcie_ref_prop,
        MAX_NAME_SIZE, MAX_NAME_SIZE);
    return_val_do_info_if_fail(VOS_OK == ret || 0 == strlen(pcie_ref_obj), VOS_ERR,
        debug_log(DLOG_MASS, "%s, %d: get ref object fail.\n", __FUNCTION__, __LINE__));

    ret = dfl_get_object_list(CLASS_COMPONENT, &obj_list);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_MASS, "%s, %d: get obj_list fail.\n", __FUNCTION__, __LINE__));

    object_component_type = redfish_get_pciedevice_component_type(i_obj_handle);
    for (obj_node = obj_list; obj_node; obj_node = g_slist_next(obj_node)) {
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_DEVICE_TYPE, &component_type);
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_PRESENCE, &presence);
        do_if_expr(0 == presence, continue);

        
        do_if_expr(object_component_type != component_type, continue);

        ret = dfl_get_referenced_property((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_DEVICENUM, comp_ref_object,
            ref_property, MAX_NAME_SIZE, MAX_NAME_SIZE);
        do_if_expr(VOS_OK != ret, debug_log(DLOG_MASS, "%s, %d: get referenced property fail.", __FUNCTION__, __LINE__);
            continue);

        
        ret = dfl_get_object_handle(comp_ref_object, &comp_handle);
        do_if_expr(VOS_OK != ret, debug_log(DLOG_MASS, "%s, %d: get obj handle fail.", __FUNCTION__, __LINE__);
            continue);

        
        ret = dfl_get_referenced_property(comp_handle, ref_property, ref_ref_object, ref_ref_property, MAX_NAME_SIZE,
            MAX_NAME_SIZE);
        do_if_expr(VOS_OK != ret, debug_log(DLOG_MASS, "%s, %d: get referenced property fail.", __FUNCTION__, __LINE__);
            continue);

        if (g_strcmp0(ref_ref_object, pcie_ref_obj) == 0) {
            *o_handle = (OBJ_HANDLE)obj_node->data;
            g_slist_free(obj_list);
            return VOS_OK;
        }
    }

    g_slist_free(obj_list);
    return VOS_ERR;
}

LOCAL gint32 compare_slot_get_card_handle(const gchar *classname, guint8 pcie_card_slot, OBJ_HANDLE *handle)
{
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guint8 slot = INVALID_VAL;

    gint32 ret = dfl_get_object_list(classname, &obj_list);
    if (ret != RET_OK) {
        debug_log(DLOG_MASS, "%s, %d: get obj lsit fail, ret = %d", __FUNCTION__, __LINE__, ret);
        return ret;
    }

    for (obj_node = obj_list; obj_node; obj_node = g_slist_next(obj_node)) {
        ret = dal_get_silk_id((OBJ_HANDLE)obj_node->data, PROPERTY_PCIE_CARD_LOGIC_SLOT, PROPETRY_PCIECARD_SLOT, &slot);
        if (ret != RET_OK) {
            continue;
        }
        if (slot == pcie_card_slot) {
            *handle = (OBJ_HANDLE)obj_node->data;
            g_slist_free(obj_list);
            return RET_OK;
        }
    }

    g_slist_free(obj_list);
    return RET_ERR;
}

LOCAL gint32 get_card_type_and_slot(const gchar *rsc_uri, gchar **card_class_name, guint8 *slot_id)
{
    gint32 ret = 0;
    gchar *card_slot_str = NULL;
    gchar *end_ptr = NULL;
#define OCP_URI_KEY_STR "OCP"
    if (strcasestr(rsc_uri, OCP_URI_KEY_STR) != NULL) {
        *card_class_name = CLASS_OCP_CARD;
        
        ret = rf_get_regex_match_result_nth(URI_PATTERN_OCP_CARD_RSC, rsc_uri, 1, &card_slot_str);
    } else if (strcasestr(rsc_uri, CLASS_NPU) != NULL) {
        *card_class_name = CLASS_NPU;
        
        ret = rf_get_regex_match_result_nth(URI_PATTERN_NPU_RSC, rsc_uri, 1, &card_slot_str);
    } else {
        *card_class_name = CLASS_PCIECARD_NAME;
        
        ret = rf_get_regex_match_result_nth(URI_PATTERN_PCIE_CARD_RSC, rsc_uri, 1, &card_slot_str);
    }
    return_val_if_expr(ret != VOS_OK, ret);

    
    
    if (card_slot_str == NULL || (card_slot_str[0] == '0' && strlen(card_slot_str) >= STR_LEN_2)) {
        return RET_ERR;
    }

    *slot_id = strtoul(card_slot_str, &end_ptr, BASE_10);
    return_val_do_info_if_fail((end_ptr != NULL) && (*end_ptr == '\0'), VOS_ERR,
        debug_log(DLOG_ERROR, "parse pcie card slot with [%s] failed", card_slot_str);
        g_free(card_slot_str));

    g_free(card_slot_str);
    return RET_OK;
}


LOCAL gint32 redfish_get_pciedevice_info_from_uri(const gchar *rsc_uri, OBJ_HANDLE *o_obj_handle)
{
    gint32 ret;
    guint8 slot_id = 0;
    gchar *card_class_name = NULL;

    if ((rsc_uri == NULL) || (o_obj_handle == NULL)) {
        return RET_ERR;
    }
    ret = get_card_type_and_slot(rsc_uri, &card_class_name, &slot_id);
    if (ret != RET_OK) {
        return RET_ERR;
    }
    // D板NPU，联通定制化场景将D板的NPU视为PCIe资源
    if (g_strcmp0(card_class_name, CLASS_NPU) == 0) {
        if (dal_is_customized_by_cucc() && check_npu_type_match_dboard()) {
            return cucc_customized_get_npu_handle(slot_id, o_obj_handle);
        }
        return RET_ERR;
    }
    // Pcie卡, OCP卡
    ret = compare_slot_get_card_handle(card_class_name, slot_id, o_obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "uri[%s] check failed", rsc_uri);
    }
    return ret;
}


gboolean redfish_check_chassis_pciedevice_uri_effective(const gchar *rsc_uri, OBJ_HANDLE *o_obj_handle)
{
    
    return_val_do_info_if_fail((NULL != rsc_uri) && (NULL != o_obj_handle), FALSE,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    
    gint32 ret = redfish_get_pciedevice_info_from_uri(rsc_uri, o_obj_handle);
    if (ret != RET_OK || *o_obj_handle == INVALID_OBJ_HANDLE) {
        debug_log(DLOG_ERROR, "uri[%s] check failed or card handle is 0", rsc_uri);
        return FALSE;
    }

    return TRUE;
}


gboolean redfish_check_pciedevice_location_devicename(const gchar *location_devicename, OBJ_HANDLE *o_obj_handle)
{
    gchar pcie_memberid[MAX_RSC_ID_LEN] = {0};
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guint8 pcie_presence = 0;
    OBJ_HANDLE component_handle = 0;

    
    gint32 ret = dfl_get_object_list(CLASS_PCIECARD_NAME, &obj_list);
    return_val_do_info_if_fail(VOS_OK == ret, FALSE,
        debug_log(DLOG_MASS, "%s, %d: get obj lsit fail.\n", __FUNCTION__, __LINE__));

    for (obj_node = obj_list; obj_node; obj_node = g_slist_next(obj_node)) {
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPETRY_PCIECARD_PRESENCE, &pcie_presence);
        do_if_expr(VOS_OK != ret || 0 == pcie_presence, continue);

        
        ret = redfish_get_pcie_component_obj_handle((OBJ_HANDLE)obj_node->data, &component_handle);
        do_if_expr(VOS_OK != ret, continue);

        (void)memset_s(pcie_memberid, sizeof(pcie_memberid), 0, sizeof(pcie_memberid));

        
        ret = get_object_obj_location_devicename(component_handle, pcie_memberid, sizeof(pcie_memberid));
        do_if_expr(VOS_OK != ret, continue);

        if (0 == g_ascii_strcasecmp(location_devicename, pcie_memberid)) {
            *o_obj_handle = (OBJ_HANDLE)obj_node->data;
            g_slist_free(obj_list);
            return TRUE;
        }
    }

    g_slist_free(obj_list);

    return FALSE;
}


LOCAL gint32 get_chassis_pciedevices_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar slot_id[MAX_RSC_ID_LEN] = {0};
    gchar str_context[MAX_STRBUF_LEN] = {0};
    

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_MASS, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    OBJ_HANDLE chassis_handle = (OBJ_HANDLE)g_slist_nth_data(i_paras->parent_obj_handle_list, 0);
    gint32 ret = rf_gen_chassis_component_id(NULL, chassis_handle, slot_id, MAX_RSC_ID_LEN);
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__));

    

    ret = snprintf_s(str_context, sizeof(str_context), sizeof(str_context) - 1, RFPROP_CHASSIS_PCIEDEVICES_METADATA,
        slot_id);
    do_if_expr(0 >= ret, debug_log(DLOG_MASS, "%s %d:snprintf_s fail.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_string(str_context);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);
    return HTTP_OK;
}

LOCAL gint32 get_chassis_pciedevice_odata_info(const gchar *member_id, gchar *card_prefix_buf, guchar buf_len,
    guchar *slot_id)
{
    const gchar *slot_id_index = NULL;

    if ((member_id == NULL) || (card_prefix_buf == NULL) || (slot_id == NULL)) {
        return VOS_ERR;
    }
    const gchar* prefix_list[] = {RF_PCIECARD_PREFIX, RF_NPU_PREFIX, RF_OCPCARD_PREFIX};
    for (gsize i = 0; i < G_N_ELEMENTS(prefix_list); i++) {
        slot_id_index = g_str_case_rstr(member_id, prefix_list[i]);
        if (slot_id_index != NULL) {
            if (strcpy_s(card_prefix_buf, buf_len, prefix_list[i]) != EOK) {
                debug_log(DLOG_DEBUG, "%s: strncpy_s fail!", __FUNCTION__);
                return RET_ERR;
            }
            slot_id_index += strlen(prefix_list[i]);
            return vos_str2int(slot_id_index, 10, (void *)slot_id, NUM_TPYE_UCHAR); 
        }
    }
    return RET_ERR;
}

LOCAL gint32 get_chassis_pciedevices_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar slot_id[MAX_RSC_ID_LEN] = {0};
    gchar str_odata_id[MAX_STRBUF_LEN] = {0};
    gchar pcie_memberid[MAX_RSC_ID_LEN] = {0};
    guchar slot_num = 0;
    gchar card_prefix_buf[MAX_STRBUF_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_MASS, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    gint32 ret = get_chassis_pciedevice_odata_info(i_paras->member_id, card_prefix_buf,
        sizeof(card_prefix_buf), &slot_num);
    return_val_do_info_if_fail(ret == VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "parse pcie card slot from %s failed", i_paras->member_id));

    
    OBJ_HANDLE chassis_comp_handle = (OBJ_HANDLE)g_slist_nth_data(i_paras->parent_obj_handle_list, 0);
    ret = rf_gen_chassis_component_id(NULL, chassis_comp_handle, slot_id, MAX_RSC_ID_LEN);
    return_val_do_info_if_fail(ret == VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__));

    ret = snprintf_s(pcie_memberid, MAX_RSC_ID_LEN, MAX_RSC_ID_LEN - 1, "%s%u", card_prefix_buf, slot_num);
    return_val_do_info_if_fail(ret > 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "format pcie card id failed, ret is %d", ret));

    ret = snprintf_s(str_odata_id, sizeof(str_odata_id), sizeof(str_odata_id) - 1, URI_FORMAT_CHASSIS_PCIEDEVICES_URI,
        slot_id, pcie_memberid);
    do_if_expr(0 >= ret, debug_log(DLOG_MASS, "%s %d:snprintf_s fail.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_string(str_odata_id);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}

LOCAL gint32 get_pciecard_nic_type(OBJ_HANDLE obj_handle, gchar *type, guint32 type_size)
{
    OBJ_HANDLE card_handle = 0;
    guint32 func_type;
    GSList* busi_list = NULL;
    GSList* busi_node = NULL;
    OBJ_HANDLE busi_handle;
    OBJ_HANDLE ref_handle = 0;

    gint32 ret = dal_get_specific_object_position(obj_handle, CLASS_NETCARD_NAME, &card_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get netcard object handle failed", __FUNCTION__);
        return ret;
    }

    ret = dfl_get_object_list(ETH_CLASS_NAME_BUSY_ETH, &busi_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s:get busi_list failed", __FUNCTION__);
        return RET_ERR;
    }

    for (busi_node = busi_list; busi_node; busi_node = busi_node->next) {
        busi_handle = (OBJ_HANDLE)busi_node->data;

        ret = dfl_get_referenced_object(busi_handle, BUSY_ETH_ATTRIBUTE_REF_NETCARD, &ref_handle);
        // 过滤掉不是该网卡的网口
        if (ret !=  DFL_OK || card_handle != ref_handle) {
            continue;
        }

        ret = dal_get_property_value_uint32(busi_handle, BUSY_ETH_ATTRIBUTE_NETDEV_FUNCTYPE, &func_type);
        if (ret != RET_OK) {
            continue;
        }

        switch (func_type) {
            case NETDEV_FUNCTYPE_FC:
                ret = strcpy_s(type, type_size, "FC");
                break;
            case NETDEV_FUNCTYPE_IB:
                ret = strcpy_s(type, type_size, "IB");
                break;
            default:
                ret = strcpy_s(type, type_size, "NIC");
                break;
        }
        g_slist_free(busi_list);
        return ret;
    }

    g_slist_free(busi_list);
    return RET_ERR;
}

LOCAL gint32 get_pciecard_type(OBJ_HANDLE obj_handle, gchar *type, guint32 type_size)
{
    guint8 funclass;

    gint32 ret = dal_get_property_value_byte(obj_handle, PROPERTY_PCIECARD_FUNCTIONCLASS, &funclass);
    if (ret != DFL_OK) {
        return ret;
    }

    switch (funclass) {
        case PCIECARD_FUNCTION_RAID:
            ret = strcpy_s(type, type_size, "RAID");
            break;
        case PCIECARD_FUNCTION_NETCARD:
            ret = get_pciecard_nic_type(obj_handle, type, type_size);
            break;
        case PCIECARD_FUNCTION_GPU:
            ret = strcpy_s(type, type_size, "GPU");
            break;
        case PCIECARD_FUNCTION_STORAGE:
            ret = strcpy_s(type, type_size, "SAS");
            break;
        case PCIECARD_FUNCTION_FPGA:
            ret = strcpy_s(type, type_size, "FPGA");
            break;
        default:
            ret = RET_ERR;
            debug_log(DLOG_DEBUG, "%s: unknown pcie card type(%d)", __FUNCTION__, funclass);
            break;
    }

    return ret;
}


LOCAL gint32 get_chassis_pciedevices_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar pcie_memberid[MAX_STRBUF_LEN] = {0};
    gchar card_prefix_buf[MAX_STRBUF_LEN] = {0};
    guchar slot_num = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    if (check_npu_handle_with_cucc_customized(i_paras->obj_handle)) {
        return rf_get_property_value_string_as_jso(i_paras->obj_handle, PROPERTY_NPU_NAME, o_result_jso);
    }

    gint32 ret = get_chassis_pciedevice_odata_info(i_paras->member_id, card_prefix_buf,
        sizeof(card_prefix_buf), &slot_num);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "parse pcie card slot from %s failed", i_paras->member_id));
    ret = snprintf_s(pcie_memberid, MAX_STRBUF_LEN, MAX_STRBUF_LEN - 1, "%s%u", card_prefix_buf, slot_num);
    return_val_do_info_if_fail(ret > 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "format pcie card id failed, ret is %d", ret));

    *o_result_jso = json_object_new_string(pcie_memberid);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}

LOCAL gint32 get_chassis_pciedevices_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar pcie_memberid[MAX_STRBUF_LEN] = {0};
    gchar card_prefix_buf[MAX_STRBUF_LEN] = {0};
    guchar slot_num = 0;
    gboolean is_customized;

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (!i_paras->is_satisfy_privi) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    if (check_npu_handle_with_cucc_customized(i_paras->obj_handle)) {
        return rf_get_property_value_string_as_jso(i_paras->obj_handle, PROPERTY_NPU_NAME, o_result_jso);
    }

    gint32 ret = get_chassis_pciedevice_odata_info(i_paras->member_id, card_prefix_buf,
        sizeof(card_prefix_buf), &slot_num);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "parse pcie card slot from %s failed", i_paras->member_id);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    is_customized = dal_is_customized_by_cmcc();
    if (is_customized) {
        ret = get_pciecard_type(i_paras->obj_handle, card_prefix_buf, sizeof(card_prefix_buf));
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: invalid pcie card type, ret(%d)", __FUNCTION__, ret);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
    }

    ret = snprintf_s(pcie_memberid, MAX_STRBUF_LEN, MAX_STRBUF_LEN - 1, "%s%u", card_prefix_buf, slot_num);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "format pcie card id failed, ret is %d", ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string(pcie_memberid);
    if (*o_result_jso == NULL) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 get_chassis_pciedevices_description(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar str_description[MAX_PCIE_CARD_DESC_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_MASS, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    if (check_npu_handle_with_cucc_customized(i_paras->obj_handle)) {
        *o_result_jso = json_object_new_string(RF_NPU_PREFIX);
        if (*o_result_jso == NULL) {
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        return HTTP_OK;
    }

    gint32 ret = dal_get_property_value_string(i_paras->obj_handle,
        g_chassis_pciedevices_provider[i_paras->index].pme_prop_name, str_description, sizeof(str_description));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get pciedevice description fail.", __FUNCTION__, __LINE__));

    if (0 == strlen(str_description) || (0 == g_strcmp0(str_description, "N/A"))) {
        debug_log(DLOG_MASS, "%s, %d: pciedevice description is invalid.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string(str_description);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}

LOCAL gint32 get_cmcc_customized_manufacturer(OBJ_HANDLE obj_handle, gint32 prop_index, gchar *manu, guint32 manu_size)
{
    OBJ_HANDLE card_handle = 0;
    guint8 card_type = 0;

    gint32 ret = redfish_check_pciecard_type_handle(obj_handle, &card_type, &card_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get object failed", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (card_type == PCIECARD_FUNCTION_NETCARD) {
        ret = dal_get_property_value_string(card_handle, BUSY_NETCARD_CHIP_MANU, manu, manu_size);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: get netcard chip manufacturer failed", __FUNCTION__);
            return ret;
        }
    } else if (card_type == PCIECARD_FUNCTION_RAID) {
        ret = dal_get_property_value_string(card_handle, PROPERTY_RAID_CONTROLLER_MANUFACTURER, manu, manu_size);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: get raid controller manufacturer failed", __FUNCTION__);
            return ret;
        }
    } else {
        ret = dal_get_property_value_string(card_handle, g_chassis_pciedevices_provider[prop_index].pme_prop_name,
            manu, manu_size);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: get %s property failed", __FUNCTION__,
                g_chassis_pciedevices_provider[prop_index].pme_prop_name);
            return ret;
        }

        
        if (strstr(manu, "NVIDIA") != NULL) {
            ret = strcpy_s(manu, manu_size, "Nvidia");
            if (ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strcpy_s failed, ret(%d)", __FUNCTION__, ret);
                return ret;
            }
        }
    }

    return ret;
}


LOCAL gint32 get_chassis_pciedevices_manufacturer(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar str_manufacturer[MAX_STRBUF_LEN] = {0};
    gboolean is_customized;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    is_customized = dal_is_customized_by_cmcc();
    if (is_customized) {
        ret = get_cmcc_customized_manufacturer(i_paras->obj_handle, i_paras->index, str_manufacturer,
            sizeof(str_manufacturer));
    } else if (check_npu_handle_with_cucc_customized(i_paras->obj_handle)) {
        
        ret = dal_get_property_value_string(i_paras->obj_handle, PROPERTY_NPU_MANUFACTURER,
            str_manufacturer, MAX_STRBUF_LEN);
    } else {
        ret = dal_get_property_value_string(i_paras->obj_handle,
            g_chassis_pciedevices_provider[i_paras->index].pme_prop_name, str_manufacturer, sizeof(str_manufacturer));
    }

    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get pciedevice manufacturer fail.", __FUNCTION__, __LINE__));

    if (0 == strlen(str_manufacturer) || (0 == g_strcmp0(str_manufacturer, "N/A"))) {
        debug_log(DLOG_MASS, "%s, %d: pciedevice manufacturer is invalid.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string(str_manufacturer);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);
    return HTTP_OK;
}

LOCAL gint32 get_pciedevices_model_by_card_type(OBJ_HANDLE obj, guint8 card_type, gchar *model, guint32 len)
{
    PCIE_TYPE_PROP_PAIR pairs[] = {
        {PCIECARD_FUNCTION_STORAGE, PROPERTY_PCIESSDCARD_MN},
        {PCIECARD_FUNCTION_NETCARD, BUSY_NETCARD_MODEL},
        {PCIECARD_FUNCTION_GPU,     RFPROP_PCIEDEVICES_MODEL},
        {PCIECARD_FUNCTION_RAID,    PROPERTY_RAID_CONTROLLER_TYPE}
    };
    guint32 i = 0;
    guint32 pairs_len = sizeof(pairs) / sizeof(pairs[0]);
    gchar model_temp[MAX_STRBUF_LEN] = {0};
    for (; i < pairs_len; i++) {
        if (card_type == pairs[i].type) {
            break;
        }
    }
    if (i == pairs_len) {
        debug_log(DLOG_ERROR, "%s: invalid card type %d", __FUNCTION__, card_type);
        return RET_ERR;
    }
    gint32 ret = dal_get_property_value_string(obj, pairs[i].prop, model_temp, MAX_STRBUF_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get property value fail. ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }
    if (pairs[i].type == PCIECARD_FUNCTION_RAID) {
        ret = sprintf_s(model, len, "RAID_%s", model_temp);
    } else {
        ret = sprintf_s(model, len, "%s", model_temp);
    }
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: sprintf_s fail", __FUNCTION__);
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 get_chassis_pciedevices_cmcc(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar prop_val[MAX_STRBUF_LEN] = {0};
    
    if (!dal_is_customized_by_cmcc()) {
        return HTTP_OK;
    }

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    (void)dal_get_property_value_string(i_paras->obj_handle,
        g_chassis_pciedevices_provider[i_paras->index].pme_prop_name, prop_val, sizeof(prop_val));

    if (strlen(prop_val) == 0 || (g_strcmp0(prop_val, "N/A") == 0)) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    *o_result_jso = json_object_new_string(prop_val);
    if (*o_result_jso == NULL) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return HTTP_OK;
}


LOCAL gint32 get_chassis_pciedevices_model(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    guint8 card_type = 0;
    gchar model[MAX_STRBUF_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    if (check_npu_handle_with_cucc_customized(i_paras->obj_handle)) {
        return rf_get_property_value_string_as_jso(i_paras->obj_handle, PROPERTY_NPU_VERSION, o_result_jso);
    }

    
    gint32 ret = redfish_check_pciecard_type_handle(i_paras->obj_handle, &card_type, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get pcie card type fail.", __FUNCTION__, __LINE__));

    if (PCIECARD_FUNCTION_NPU == card_type) {
        add_ome_property_model(i_paras->obj_handle, o_result_jso);
        return HTTP_OK;
    }

    ret = get_pciedevices_model_by_card_type(obj_handle, card_type, model, MAX_STRBUF_LEN);
    if (ret != RET_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if (0 == strlen(model) || (0 == g_strcmp0(model, "N/A"))) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string(model);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}

LOCAL gint32 get_chassis_pciedevices_partnumber(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    if (check_npu_handle_with_cucc_customized(i_paras->obj_handle)) {
        *o_result_jso = NULL;
        return HTTP_OK;
    }
    gchar partnum[MAX_STRBUF_LEN] = {0};

    gint32 ret = rf_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        return ret;
    }

    ret = dal_get_property_value_string(i_paras->obj_handle, PROPERTY_PCIECARD_PART_NUMBER, partnum, sizeof(partnum));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get property value fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (rf_string_check((const gchar*)partnum) == RET_OK) {
        *o_result_jso = json_object_new_string((const gchar*)partnum);
        if (*o_result_jso == NULL) {
            return HTTP_INTERNAL_SERVER_ERROR;
        }
    }

    return HTTP_OK;
}


LOCAL gint32 get_chassis_pciedevices_serialnumber(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    guint8 card_type = 0;
    gchar serialnumber[MAX_STRBUF_LEN] = {0};
    

    
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    if (check_npu_handle_with_cucc_customized(i_paras->obj_handle)) {
        return rf_get_property_value_string_as_jso(i_paras->obj_handle, PROPERTY_NPU_SN, o_result_jso);
    }

    
    
    gint32 ret = redfish_check_pciecard_type_handle(i_paras->obj_handle, &card_type, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get pcie card type fail.", __FUNCTION__, __LINE__));

    
    if ((PCIECARD_FUNCTION_STORAGE == card_type) || (PCIECARD_FUNCTION_GPU == card_type)) {
        ret = dal_get_property_value_string(obj_handle, RFPROP_PCIEDEVICES_SERIALNUMBER, serialnumber,
            sizeof(serialnumber));
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get property value fail.", __FUNCTION__, __LINE__));
    }
    
    else { // 有电子标签的卡获取电子标签中的属性值
        get_board_serialnumber(obj_handle, serialnumber, sizeof(serialnumber));
    }

    
    if (0 == strlen(serialnumber) || 0 == g_strcmp0(serialnumber, "N/A")) {
        if (card_type != PCIECARD_FUNCTION_RAID && card_type != PCIECARD_FUNCTION_NETCARD) {
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        ret = dal_get_property_value_string(obj_handle, PROPERTY_RAID_CONTROLLER_SERIALNUMBER, serialnumber,
            sizeof(serialnumber));
        if (ret != RET_OK || strlen(serialnumber) == 0 || g_strcmp0(serialnumber, "N/A") == 0) {
            return HTTP_INTERNAL_SERVER_ERROR;
        }
    }

    *o_result_jso = json_object_new_string(serialnumber);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}

LOCAL gboolean check_card_type_for_firmwareversion(guint8 card_type)
{
    return (PCIECARD_FUNCTION_NETCARD == card_type) || (PCIECARD_FUNCTION_STORAGE == card_type) ||
        (PCIECARD_FUNCTION_RAID == card_type) || (PCIECARD_FUNCTION_ACCELERATE == card_type) ||
        (PCIECARD_FUNCTION_GPU == card_type) || (PCIECARD_FUNCTION_FPGA == card_type) ||
        (PCIECARD_FUNCTION_NPU == card_type);
}


LOCAL gint32 get_chassis_pciedevices_firmwareversion(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    guint8 card_type = 0;
    gchar firmwareversion[MAX_STRBUF_LEN] = {0};
    gchar firmwareversion_temp[MAX_STRBUF_LEN] = {0};
    gchar shell_id[MAX_STRBUF_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_MASS, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    if (check_npu_handle_with_cucc_customized(i_paras->obj_handle)) {
        return rf_get_property_value_string_as_jso(i_paras->obj_handle, PROPERTY_NPU_FIRMWARE_VERSION, o_result_jso);
    }

    
    if (dal_is_customized_by_cucc()) {
        add_ome_property_firmware_version_from_mcu(i_paras->obj_handle, o_result_jso);
        return HTTP_OK;
    }

    
    gint32 ret = get_pciecard_extend_firmwareversion(i_paras->obj_handle, o_result_jso);
    
    return_val_if_expr(VOS_OK == ret, HTTP_OK);
    

    
    ret = redfish_check_pciecard_type_handle(i_paras->obj_handle, &card_type, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get pcie card type fail.", __FUNCTION__, __LINE__));

    
    
    
    if (check_card_type_for_firmwareversion(card_type)) {
        ret = dal_get_property_value_string(obj_handle, RFPROP_PCIEDEVICES_FIRMWAREVERSION, firmwareversion,
            sizeof(firmwareversion));
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get property value fail.", __FUNCTION__, __LINE__));
    }

    

    if (PCIECARD_FUNCTION_FPGA == card_type || (PCIECARD_FUNCTION_NPU == card_type)) {
        ret = dal_get_property_value_string(obj_handle, PROPERTY_FPGA_CARD_SHELL_ID, shell_id, sizeof(shell_id));
        return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s: get property value fail.", __FUNCTION__));
    }

    if (0 == strlen(firmwareversion) || (0 == g_strcmp0(firmwareversion, "N/A"))) {
        return HTTP_INTERNAL_SERVER_ERROR;
    } else if (0 != strlen(shell_id) && (0 != g_strcmp0(shell_id, "N/A"))) {
        ret =
            snprintf_s(firmwareversion_temp, MAX_STRBUF_LEN, MAX_STRBUF_LEN - 1, "%s (%s)", firmwareversion, shell_id);
        do_if_expr(0 >= ret, debug_log(DLOG_MASS, "%s %d:snprintf_s fail.", __FUNCTION__, __LINE__));
        *o_result_jso = json_object_new_string(firmwareversion_temp);
        do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

        return HTTP_OK;
    }

    

    *o_result_jso = json_object_new_string(firmwareversion);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL void get_chassis_pciedevices_component_property(OBJ_HANDLE obj_handle, gchar *property_name,
    json_object *huawei_jso, const gchar *rf_prop)
{
    OBJ_HANDLE component_handle = 0;
    gchar property_str[MAX_STRBUF_LEN] = {0};

    gint32 ret = redfish_get_pcie_component_obj_handle(obj_handle, &component_handle);
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_MASS, "%s, %d: get component handle fail.", __FUNCTION__, __LINE__));

    ret = dal_get_property_value_string(component_handle, property_name, property_str, sizeof(property_str));
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_MASS, "%s, %d: pciedevice devicename fail.", __FUNCTION__, __LINE__));

    if (0 == strlen(property_str)) {
        return;
    }
    
    (void)json_object_object_add(huawei_jso, rf_prop, json_object_new_string(property_str));

    return;
}


LOCAL void get_chassis_pciedevices_slave_card_property(OBJ_HANDLE obj_handle, gchar *property_name, gchar *rsc_name,
    json_object *o_json_obj)
{
    gchar slavecard[MAX_STRBUF_LEN] = {0};
    OBJ_HANDLE slave_handle = 0;

    // 获取SlaveCard属性对应的子卡类名
    gint32 ret = dal_get_property_value_string(obj_handle, PROPERTY_PCIE_CARD_SLAVE_CARD, slavecard, sizeof(slavecard));
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_MASS, "%s: get slave card name fail.ret=%d", __FUNCTION__, ret));

    // 根据类名找到同一position的对象句柄
    ret = dal_get_specific_object_position(obj_handle, slavecard, &slave_handle);
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_MASS, "%s: get slave card %s obj_handle fail.ret=%d", __FUNCTION__, slavecard, ret));

    // 根据传入的属性名、资源名获取属性值并添加到资源
    ret = rf_obj_handle_data_trans_to_rsc_data(slave_handle, property_name, rsc_name, o_json_obj);
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_MASS, "%s: set %s to %s fail.ret=%d", __FUNCTION__, property_name, rsc_name, ret));

    return;
}


LOCAL gint32 check_mcu_support_enable(OBJ_HANDLE obj_handle)
{
    gchar slavecard[MAX_STRBUF_LEN] = {0};
    OBJ_HANDLE slave_handle = 0;
    guint8 mcu_supported = 0;

    // 获取SlaveCard属性对应的子卡类名
    gint32 ret = dal_get_property_value_string(obj_handle, PROPERTY_PCIE_CARD_SLAVE_CARD, slavecard, sizeof(slavecard));
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_MASS, "%s: get slave card name fail.ret=%d", __FUNCTION__, ret));

    // 根据类名找到同一position的对象句柄
    ret = dal_get_specific_object_position(obj_handle, slavecard, &slave_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_MASS, "%s: get slave card %s obj_handle fail.ret=%d", __FUNCTION__, slavecard, ret));

    ret = dal_get_property_value_byte(slave_handle, PROPERTY_FPGA_CARD_MCUSUPPORTED, &mcu_supported);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get mcu supported fail", __FUNCTION__));
    
    return_val_do_info_if_fail(MCU_SUPPORTED == mcu_supported, VOS_ERR,
        debug_log(DLOG_DEBUG, "%s: pcie card support mcu failed", __FUNCTION__));
    

    return VOS_OK;
}

LOCAL gint32 get_chassis_pciedevices_slave_card_memory_prop(OBJ_HANDLE obj_handle, json_object **o_result_jso)
{
#define MEM_PRESENT 1
    GSList *dimm_list = NULL;
    GSList *obj_node_dimm = NULL;
    json_object *obj_json = NULL;
    guint8 presence = 0;
    gchar capacity_str[PROP_VAL_LENGTH] = {0};
    guint32 capacity_value = 0;

    
    if (NULL == o_result_jso || 0 == obj_handle) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    *o_result_jso = json_object_new_array();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_array fail\n.", __FUNCTION__, __LINE__));

    gint32 ret = dal_get_object_list_position(obj_handle, CLASS_FPGA_MEMORY, &dimm_list);
    return_val_do_info_if_expr(VOS_OK != ret || NULL == dimm_list, VOS_ERR, json_object_put(*o_result_jso);
        debug_log(DLOG_INFO, "%s: get fpga_memory_obj_handle list failed, ret %d", __FUNCTION__, ret));

    for (obj_node_dimm = dimm_list; obj_node_dimm; obj_node_dimm = g_slist_next(obj_node_dimm)) {
        obj_json = json_object_new_object();
        rf_add_property_jso_string((OBJ_HANDLE)obj_node_dimm->data, PROPERTY_MEM_NAME, RFPROP_FPGA_MEMORY_NAME,
            obj_json);
        
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node_dimm->data, PROPERTY_MEM_PRESENCE, &presence);
        json_object_object_add(obj_json, RFPROP_FPGA_MEMORY_STATE,
            json_object_new_string(presence == MEM_PRESENT ? PRESENT_STRING : ABSENT_STRING));
        rf_add_property_jso_string((OBJ_HANDLE)obj_node_dimm->data, PROPERTY_MEM_MANUFACTURE,
            RFPROP_FPGA_MEMORY_MANUFACTURER, obj_json);

        (void)dal_get_property_value_string((OBJ_HANDLE)obj_node_dimm->data, PROPERTY_MEM_CAPACITY, capacity_str,
            sizeof(capacity_str));
        ret = transform_capacity_to_intmb(capacity_str, strlen(capacity_str), &capacity_value);
        do_info_if_true(VOS_OK != ret,
            debug_log(DLOG_INFO, "%s: transform_capacity_to_intmb failed, ret %d", __FUNCTION__, ret));
        json_object_object_add(obj_json, RFPROP_FPGA_MEMORY_CAPACITY_MB, json_object_new_int(capacity_value));
        
        rf_add_property_jso_string((OBJ_HANDLE)obj_node_dimm->data, PROPERTY_MEM_PART_NUM, RFPROP_FPGA_MEMORY_PARTNUM,
            obj_json);
        rf_add_property_jso_string((OBJ_HANDLE)obj_node_dimm->data, PROPERTY_MEM_SN, RFPROP_FPGA_MEMORY_SN, obj_json);

        json_object_array_add(*o_result_jso, obj_json);
    }

    g_slist_free(dimm_list);
    return HTTP_OK;
}

LOCAL void get_chassis_pciedevices_slave_card_chip_ecc_info(OBJ_HANDLE obj_handle, json_object *huawei_jso)
{
    const guint32 *ecc_array = NULL;
    GVariant *get_value = NULL;
    json_object *ecc_jso = NULL;
    gsize chip_num = 0;

    return_do_info_if_expr(NULL == huawei_jso, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    gint32 ret = dfl_get_property_value(obj_handle, PROPERTY_PCIE_CARD_CHIPECC, &get_value);
    return_do_info_if_expr(ret != VOS_OK,
        debug_log(DLOG_ERROR, "%s: property %s not found.", __FUNCTION__, PROPERTY_PCIE_CARD_CHIPECC));

    ecc_array = (const guint32 *)g_variant_get_fixed_array(get_value, &chip_num, sizeof(guint32));
    return_do_info_if_expr(ecc_array == NULL, g_variant_unref(get_value);
        get_value = NULL;
        (void)json_object_object_add(huawei_jso, RFPROP_FPGA_ECC_INFO, NULL));

    ecc_jso = json_object_new_array();
    return_do_info_if_expr(ecc_jso == NULL, g_variant_unref(get_value);
        (void)json_object_object_add(huawei_jso, RFPROP_FPGA_ECC_INFO, NULL));
    
    if (chip_num == CHIP_NUM_NOT_SUPPORT_D_CARD && (ecc_array[CHIP_NUM_NOT_SUPPORT_D_CARD - 1] == INVALID_DATA_DWORD)) {
        debug_log(DLOG_INFO, "%s: %d this Card type does not match.", __FUNCTION__, __LINE__);
    } else {
        for (gsize index_chip = 0; index_chip < chip_num; index_chip++) {
            if (ecc_array[index_chip] == INVALID_DATA_DWORD) {
                (void)json_object_array_add(ecc_jso, NULL);
            } else {
                (void)json_object_array_add(ecc_jso, json_object_new_int(ecc_array[index_chip]));
            }
        }
    }

    (void)json_object_object_add(huawei_jso, RFPROP_FPGA_ECC_INFO, ecc_jso);
    g_variant_unref(get_value);

    return;
}


LOCAL void get_chassis_pciedevices_npu_memory_capacity(OBJ_HANDLE obj_handle, json_object *huawei_jso)
{
    GSList *npu_node = NULL;
    GSList *npu_list = NULL;
    guint64 total_capacity = 0;
    guint32 memory_capacity = 0;
    OBJ_HANDLE npu_handle;

    gint32 ret = dal_get_object_list_position(obj_handle, CLASS_NPU, &npu_list);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s: get npu_obj_handle failed, ret %d", __FUNCTION__, ret);
        return;
    }
	
    for (npu_node = npu_list; npu_node != NULL; npu_node = g_slist_next(npu_node)) {
        npu_handle = (OBJ_HANDLE)npu_node->data;

        ret = dal_get_property_value_uint32(npu_handle, PROPERTY_NPU_MEMORY_CAPACITY_KB, &memory_capacity);
        if (ret != RET_OK) {
            debug_log(DLOG_INFO, "%s: get memory capacity failed, ret %d", __FUNCTION__, ret);
            continue;
        }
        total_capacity += memory_capacity;
    }
    total_capacity = total_capacity / 1024;  // 转换单位：1 MB = 1024 KB
    if (total_capacity == 0) {
        (void)json_object_object_add(huawei_jso, RFPROP_NPU_MEMORY_CAPACITY_MB, NULL);
        if (dal_is_customized_by_cucc()) {
            (void)json_object_object_add(huawei_jso, RFPROP_SENSOR_MEMORY_SIZE, NULL);
        }
    } else {
        (void)json_object_object_add(huawei_jso, RFPROP_NPU_MEMORY_CAPACITY_MB, json_object_new_int64(total_capacity));
        if (dal_is_customized_by_cucc()) {
            (void)json_object_object_add(huawei_jso, RFPROP_SENSOR_MEMORY_SIZE, json_object_new_int64(total_capacity));
        }
    }
    g_slist_free(npu_list);
    return;
}


LOCAL void get_chassis_pciedevices_chip_temp(OBJ_HANDLE obj_handle, json_object *huawei_jso)
{
#define MCU_GET_FAILED_TEMP_VALUE 0x7fff
#define MCU_GET_INVALID_TEMP_VALUE 0x7ffd
    gint32 ret = VOS_OK;
    OBJ_HANDLE mini_temp_obj_handle = 0;
    json_object *chip_temp_jso = NULL;
    guint16 temp_value = 0;
    guint32 index_chip;
    gchar prop_name[MAX_STRBUF_LEN] = {0};

    return_do_info_if_expr(NULL == huawei_jso, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    chip_temp_jso = json_object_new_array();
    return_do_info_if_expr(chip_temp_jso == NULL,
        (void)json_object_object_add(huawei_jso, RFPROP_FPGA_CHIP_TEMP, NULL));

    for (index_chip = 0; index_chip < 4; index_chip++) {
        (void)memset_s(prop_name, MAX_STRBUF_LEN, 0, MAX_STRBUF_LEN);
        // MAX_STRBUF_LEN大小为128，远大于格式化的字符串大小，无需判断返回值
        (void)snprintf_s(prop_name, MAX_STRBUF_LEN, MAX_STRBUF_LEN - 1, "MINI%u", index_chip);

        ret = dal_get_specific_position_object_string(obj_handle, CLASS_NAME_PERIPHERAL_DEVICE_SENSOR,
            PROPERTY_P_DEVICE_SENSOR_NAME, prop_name, &mini_temp_obj_handle);
        if (VOS_OK == ret) {
            (void)dal_get_property_value_uint16(mini_temp_obj_handle, PROPERTY_P_DEVICE_SENSOR_VALUE, &temp_value);
            if (0 == temp_value || MCU_GET_INVALID_TEMP_VALUE == temp_value ||
                MCU_GET_FAILED_TEMP_VALUE == temp_value) {
                (void)json_object_array_add(chip_temp_jso, NULL);
            } else {
                (void)json_object_array_add(chip_temp_jso, json_object_new_int(temp_value));
            }
        }
    }

    (void)json_object_object_add(huawei_jso, RFPROP_FPGA_CHIP_TEMP, chip_temp_jso);
    return;
}

LOCAL void get_chassis_pciedevices_slave_card_power(OBJ_HANDLE obj_handle, gchar *property_name, gchar *rsc_name,
    json_object *o_json_obj)
{
    gchar slave_card_name[MAX_STRBUF_LEN] = {0};
    OBJ_HANDLE slave_handle = 0;
    gdouble res_data = 0.0;

    gint32 ret = dal_get_property_value_string(obj_handle, PROPERTY_PCIE_CARD_SLAVE_CARD, slave_card_name,
        sizeof(slave_card_name));
    return_do_info_if_expr(VOS_OK != ret,
        debug_log(DLOG_MASS, "%s: get slave card name fail.ret=%d", __FUNCTION__, ret));

    ret = dal_get_specific_object_position(obj_handle, slave_card_name, &slave_handle);
    return_do_info_if_expr(VOS_OK != ret,
        debug_log(DLOG_MASS, "%s: get slave card %s obj_handle fail.ret=%d", __FUNCTION__, slave_card_name, ret));

    ret = dal_get_property_value_double(slave_handle, property_name, &res_data);
    if (VOS_OK == ret) {
        json_object_object_add(o_json_obj, rsc_name, ex_json_object_new_double(res_data, "%.1f"));
    } else {
        json_object_object_add(o_json_obj, rsc_name, NULL);
    }
    return;
}


LOCAL void get_chassis_pciedevices_slave_property(OBJ_HANDLE obj_handle, json_object *huawei_jso)
{
#define HW_ACCESS_IN_UPDATE_PROGRESS 0x4000
#define HW_ACCESS_FAIL_MASK 0x8000
    gchar slavecard[MAX_STRBUF_LEN] = {0};
    OBJ_HANDLE slave_handle = 0;
    guint16 power = 0;
    json_object *memory_json = NULL;

    // 获取SlaveCard属性对应的子卡类名
    gint32 ret = dal_get_property_value_string(obj_handle, PROPERTY_PCIE_CARD_SLAVE_CARD, slavecard, sizeof(slavecard));
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_MASS, "%s: get slave card name fail.ret=%d", __FUNCTION__, ret));

    // 根据类名找到同一position的对象句柄
    ret = dal_get_specific_object_position(obj_handle, slavecard, &slave_handle);
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_MASS, "%s: get slave card %s obj_handle fail.ret=%d", __FUNCTION__, slavecard, ret));

    (void)dal_get_property_value_uint16(slave_handle, PROPERTY_FPGA_CARD_POWER, &power);

    // 判断从MCU读取的功耗值是否合理，不合理显示为null，否则正常显示
    if ((SENSOR_NA_READING & power) == SENSOR_NA_READING ||
        (SENSOR_INVALID_READING & power) == SENSOR_INVALID_READING) {
        (void)json_object_object_add(huawei_jso, RFPROP_POWER, NULL);
    } else {
        
        (void)get_chassis_pciedevices_slave_card_power(obj_handle, PROPERTY_FPGA_CARD_POWERDOUBLE, RFPROP_POWER,
            huawei_jso);
    }

    (void)get_chassis_pciedevices_slave_card_property(obj_handle, PROPERTY_FPGA_CARD_MCUVERSION, RFPROP_MCUVERSION,
        huawei_jso);
    (void) get_chassis_pciedevices_npu_memory_capacity(obj_handle, huawei_jso);
    
    (void)get_chassis_pciedevices_slave_card_chip_ecc_info(slave_handle, huawei_jso);
    

    
    (void)get_chassis_pciedevices_chip_temp(obj_handle, huawei_jso);
    

    
    ret = get_chassis_pciedevices_slave_card_memory_prop(obj_handle, &memory_json);
    return_if_expr(VOS_ERR == ret);
    json_object_object_add(huawei_jso, RFPROP_FPGA_MEMORY, memory_json);
    
}


LOCAL gint32 get_ome_property_function_type(OBJ_HANDLE obj_handle, json_object *huawei_jso)
{
    guint8 i;
    guint8 functype = 0;
    FUNCTION_TYPE_PROVIDER fcuntion_type_info_provider[] = {
        {PCIECARD_FUNCTION_RAID,           RFPROP_FUNCTIONTYPE_RAIDCARD},
        {PCIECARD_FUNCTION_NETCARD,        RFPROP_FUNCTIONTYPE_NETCARD},
        {PCIECARD_FUNCTION_GPU,            RFPROP_FUNCTIONTYPE_GPUCARD},
        {PCIECARD_FUNCTION_STORAGE,        RFPROP_FUNCTIONTYPE_STORAGE},
        {PCIECARD_FUNCTION_SDI,            RFPROP_FUNCTIONTYPE_SDI},
        {PCIECARD_FUNCTION_ACCELERATE,     RFPROP_FUNCTIONTYPE_ACCELERATE},
        {PCIECARD_FUNCTION_PCIE_RISER,     RFPROP_FUNCTIONTYPE_PCIERISER},
        {PCIECARD_FUNCTION_FPGA,           RFPROP_FUNCTIONTYPE_FPGA},
        {PCIECARD_FUNCTION_NPU,            RFPROP_FUNCTIONTYPE_NPUCARD},
        {PCIECARD_FUNCTION_STORAGE_DPU,    RFPROP_FUNCTIONTYPE_STORAGE_DPU}
    };

    if (huawei_jso == NULL) {
        debug_log(DLOG_ERROR, "NULL pointer.");
        return RET_ERR;
    }

    gint32 ret = dal_get_property_value_byte(obj_handle, PROPERTY_PCIECARD_FUNCTIONCLASS, &functype);
    if (ret != RET_OK) {
        functype = 0;
        debug_log(DLOG_ERROR, "%s get function type failed.", dfl_get_object_name(obj_handle));
    }

    
    if (functype == PCIECARD_FUNCTION_UNKNOWN) {
        (void)json_object_object_add(huawei_jso, RFPROP_FUNCTIONTYPE, NULL);
        return RET_OK;
    }

    
    for (i = 0; i < G_N_ELEMENTS(fcuntion_type_info_provider); i++) {
        if (fcuntion_type_info_provider[i].func_type == functype) {
            (void)json_object_object_add(huawei_jso, RFPROP_FUNCTIONTYPE,
                json_object_new_string(fcuntion_type_info_provider[i].function_type));
        }
    }

    
    if (G_N_ELEMENTS(fcuntion_type_info_provider) == i) {
        debug_log(DLOG_MASS, "%s get function class value is error.", dfl_get_object_name(obj_handle));
    }

    return RET_OK;
}

LOCAL void add_ome_property_firmware_version_from_mcu(OBJ_HANDLE json_obj_handle, json_object** o_result_jso)
{
    GSList *npu_node = NULL;
    GSList *npu_list = NULL;
    gchar version[STR_FIRMWARE_LEN] = {0};
    OBJ_HANDLE npu_handle;
    gint32 ret = dal_get_object_list_position(json_obj_handle, CLASS_NPU, &npu_list);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s: Get npu_obj_handle failed, ret %d.", __FUNCTION__, ret);
        return;
    }
    for (npu_node = npu_list; npu_node != NULL; npu_node = g_slist_next(npu_node)) {
        npu_handle = (OBJ_HANDLE)npu_node->data;

        ret = dal_get_property_value_string(npu_handle, PROPERTY_NPU_FIRMWARE_VERSION, version, sizeof(version));
        if (ret != RET_OK) {
            debug_log(DLOG_INFO, "%s: Get firmware version failed, ret %d.", __FUNCTION__, ret);
            continue;
        }
    }

    *o_result_jso = json_object_new_string((const gchar*)version);

    g_slist_free(npu_list);
}

LOCAL void add_ome_property_model(OBJ_HANDLE json_obj_handle, json_object** o_result_jso)
{
    GSList *npu_node = NULL;
    GSList *npu_list = NULL;
    gchar model[MAX_STRBUF_LEN] = {0};
    OBJ_HANDLE npu_handle;
    gint32 ret = dal_get_object_list_position(json_obj_handle, CLASS_NPU, &npu_list);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s: Get npu_obj_handle failed, ret %d.", __FUNCTION__, ret);
        return;
    }
    for (npu_node = npu_list; npu_node != NULL; npu_node = g_slist_next(npu_node)) {
        npu_handle = (OBJ_HANDLE)npu_node->data;

        ret = dal_get_property_value_string(npu_handle, PROPERTY_NPU_MODEL, model, sizeof(model));
        if (ret != RET_OK) {
            debug_log(DLOG_INFO, "%s: Get npu model failed, ret %d.", __FUNCTION__, ret);
            continue;
        }
    }

    *o_result_jso = json_object_new_string((const gchar*)model);
    
    g_slist_free(npu_list);
}

LOCAL void add_ome_property_reading_celsius_from_mcu(OBJ_HANDLE json_obj_handle, json_object* huawei_jso)
{
    GSList *npu_node = NULL;
    GSList *npu_list = NULL;
    gint16 temp = 0;
    gint16 max_temp = 0;
    OBJ_HANDLE npu_handle;
    gint32 ret = dal_get_object_list_position(json_obj_handle, CLASS_NPU, &npu_list);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s: Get npu_obj_handle failed, ret %d.", __FUNCTION__, ret);
        return;
    }
    for (npu_node = npu_list; npu_node != NULL; npu_node = g_slist_next(npu_node)) {
        npu_handle = (OBJ_HANDLE)npu_node->data;

        ret = dal_get_property_value_int16(npu_handle, PROPERTY_NPU_READING_CELSIUS, &temp);
        if (ret != RET_OK) {
            debug_log(DLOG_INFO, "%s: Get readingCelsius failed, ret %d.", __FUNCTION__, ret);
            continue;
        }
        max_temp = temp;
        if (max_temp < temp) {
            max_temp = temp;
        }
    }
    (void)json_object_object_add(huawei_jso, PROPERTY_NPU_READING_CELSIUS, json_object_new_int(max_temp));
    g_slist_free(npu_list);
}

LOCAL void add_ome_property_power_consumed_watts_from_mcu(OBJ_HANDLE json_obj_handle, json_object* huawei_jso)
{
    guint16 power = 0;
    guint32 total_power = 0;
    GSList *npu_node = NULL;
    GSList *npu_list = NULL;
    OBJ_HANDLE npu_handle;
    gint32 ret = dal_get_object_list_position(json_obj_handle, CLASS_NPU, &npu_list);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s: Get npu_obj_handle failed, ret %d.", __FUNCTION__, ret);
        return;
    }
    for (npu_node = npu_list; npu_node != NULL; npu_node = g_slist_next(npu_node)) {
        npu_handle = (OBJ_HANDLE)npu_node->data;

        ret = dal_get_property_value_uint16(npu_handle, PROPERTY_NPU_POWER_CONSUME, &power);
        if (ret != RET_OK) {
            debug_log(DLOG_INFO, "%s: Get PowerConsumedWatts failed, ret %d.", __FUNCTION__, ret);
            continue;
        }
        total_power += power;
    }
    gdouble power_result = (gdouble)total_power / 10;
    (void)json_object_object_add(huawei_jso, PROPERTY_NPU_POWER_CONSUME,
        ex_json_object_new_double(power_result, "%.1f"));
    g_slist_free(npu_list);
}

LOCAL void add_ome_property_video_memory_band_width_from_mcu(OBJ_HANDLE json_obj_handle, json_object* huawei_jso)
{
    guint32 memory = 0;
    GSList *npu_node = NULL;
    GSList *npu_list = NULL;
    OBJ_HANDLE npu_handle;
    gint32 ret = dal_get_object_list_position(json_obj_handle, CLASS_NPU, &npu_list);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s: Get npu_obj_handle failed, ret %d.", __FUNCTION__, ret);
        return;
    }
    for (npu_node = npu_list; npu_node != NULL; npu_node = g_slist_next(npu_node)) {
        npu_handle = (OBJ_HANDLE)npu_node->data;

        ret = dal_get_property_value_uint32(npu_handle, PROPERTY_NPU_MEMORY_BAND_WIDTH, &memory);
        if (ret != RET_OK) {
            debug_log(DLOG_INFO, "%s: Get MemoryBandWidth failed, ret %d.", __FUNCTION__, ret);
            continue;
        }
    }
    gdouble memory_band = (gdouble)memory / 10;
    (void)json_object_object_add(huawei_jso, PROPERTY_NPU_MEMORY_BAND_WIDTH,
        ex_json_object_new_double(memory_band, "%.1f"));
    g_slist_free(npu_list);
}

LOCAL void add_ome_property_rated_power_from_mcu(OBJ_HANDLE json_obj_handle, json_object* huawei_jso)
{
    guint32 rated_power = 0;
    guint64 total_rated_power = 0;
    GSList *npu_node = NULL;
    GSList *npu_list = NULL;
    OBJ_HANDLE npu_handle;
    gint32 ret = dal_get_object_list_position(json_obj_handle, CLASS_NPU, &npu_list);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s: Get npu_obj_handle failed, ret %d.", __FUNCTION__, ret);
        return;
    }
    for (npu_node = npu_list; npu_node != NULL; npu_node = g_slist_next(npu_node)) {
        npu_handle = (OBJ_HANDLE)npu_node->data;

        ret = dal_get_property_value_uint32(npu_handle, PROPERTY_NPU_POWER_CAPACITY, &rated_power);
        if (ret != RET_OK) {
            debug_log(DLOG_INFO, "%s: Get PowerCapacityWatts failed, ret %d.", __FUNCTION__, ret);
            continue;
        }
        total_rated_power += rated_power;
    }
    (void)json_object_object_add(huawei_jso, PROPERTY_NPU_POWER_CAPACITY, json_object_new_int(total_rated_power));
    g_slist_free(npu_list);
}

LOCAL void get_board_serialnumber(OBJ_HANDLE obj_handle, gchar *serialnumber, guint32 size)
{
    OBJ_HANDLE elabel_handle = 0;
    
    gint32 ret = dal_get_specific_object_position(obj_handle, CLASS_ELABEL, &elabel_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_DEBUG, "%s: Get elabel handle failed.", __FUNCTION__);
        return;
    }
    
    ret = dal_get_property_value_string(elabel_handle, PROPERTY_ELABEL_BOARD_SN, serialnumber, size);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: Get elabel board sn failed.", __FUNCTION__);
        return;
    }
}

LOCAL void get_nic_pciecard_type(OBJ_HANDLE obj_handle, json_object *jso)
{
    OBJ_HANDLE busyport_handle = 0;
    guint32 devfunc_type;

    guint32 ret = dal_get_specific_object_position(obj_handle, ETH_CLASS_NAME_BUSY_ETH, &busyport_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get business port object handle failed", __FUNCTION__);
        (void)json_object_object_add(jso, RFPROP_FUNCTIONTYPE, NULL);
        return;
    }

    ret = dal_get_property_value_uint32(busyport_handle, BUSY_ETH_ATTRIBUTE_NETDEV_FUNCTYPE, &devfunc_type);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get business port func type failed", __FUNCTION__);
        (void)json_object_object_add(jso, RFPROP_FUNCTIONTYPE, NULL);
        return;
    }

    switch (devfunc_type) {
        case NETDEV_FUNCTYPE_FC: {
            (void)json_object_object_add(jso, RFPROP_PCIECARDTYPE, json_object_new_string(FUNCTIONTYPE_FC));
            break;
        }
        case NETDEV_FUNCTYPE_IB: {
            (void)json_object_object_add(jso, RFPROP_PCIECARDTYPE, json_object_new_string(FUNCTIONTYPE_IB));
            break;
        }
        default: {
            (void)json_object_object_add(jso, RFPROP_PCIECARDTYPE, json_object_new_string(FUNCTIONTYPE_NETCARD));
            break;
        }
    }
}

LOCAL void get_oem_property_pciecard_type(OBJ_HANDLE obj_handle, json_object *jso)
{
    guint8 functype;
    gint32 ret = dal_get_property_value_byte(obj_handle, PROPERTY_PCIECARD_FUNCTIONCLASS, &functype);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get function type failed.", __FUNCTION__);
        (void)json_object_object_add(jso, RFPROP_FUNCTIONTYPE, NULL);
        return;
    }

    FUNCTION_TYPE_PROVIDER fcuntion_type_info_provider[] = {
        {PCIECARD_FUNCTION_RAID,           FUNCTIONTYPE_RAIDCARD},
        {PCIECARD_FUNCTION_GPU,            FUNCTIONTYPE_GPUCARD},
        {PCIECARD_FUNCTION_STORAGE,        FUNCTIONTYPE_STORAGE},
        {PCIECARD_FUNCTION_SDI,            FUNCTIONTYPE_SDI},
        {PCIECARD_FUNCTION_ACCELERATE,     FUNCTIONTYPE_ACCELERATE},
        {PCIECARD_FUNCTION_PCIE_RISER,     FUNCTIONTYPE_PCIERISER},
        {PCIECARD_FUNCTION_FPGA,           FUNCTIONTYPE_FPGA},
        {PCIECARD_FUNCTION_NPU,            FUNCTIONTYPE_NPU},
        {PCIECARD_FUNCTION_STORAGE_DPU,    FUNCTIONTYPE_STORAGE_DPU}
    };

    if (functype == PCIECARD_FUNCTION_NETCARD) {
        get_nic_pciecard_type(obj_handle, jso);
        return;
    }
    for (size_t i = 0; i < G_N_ELEMENTS(fcuntion_type_info_provider); i++) {
        if (fcuntion_type_info_provider[i].func_type == functype) {
            (void)json_object_object_add(jso, RFPROP_PCIECARDTYPE,
                json_object_new_string(fcuntion_type_info_provider[i].function_type));
            return;
        }
    }

    (void)json_object_object_add(jso, RFPROP_PCIECARDTYPE, NULL);
    debug_log(DLOG_MASS, "%s: get function class value is error.", __FUNCTION__);
    
    return;
}

LOCAL void get_chassis_pciedevice_slotnumber(OBJ_HANDLE obj_handle, json_object *jso)
{
    guint8 slot = INVALID_VAL;

    gint32 ret = dal_get_property_value_byte(obj_handle, PROPERTY_PCIE_CARD_LOGIC_SLOT, &slot);
    if (ret != RET_OK || slot == INVALID_VAL) {
        ret = dal_get_property_value_byte(obj_handle, PROPETRY_PCIECARD_SLOT, &slot);
    }
    if (ret == RET_OK) {
        json_object_object_add(jso, RFPROP_PCIEDEVICES_SLOT_NUM, json_object_new_int((int32_t)slot));
    } else {
        json_object_object_add(jso, RFPROP_PCIEDEVICES_SLOT_NUM, NULL);
    }

    return;
}

LOCAL const gchar* parse_alarm_stat_json(PROVIDER_PARAS_S* i_paras)
{
    const gchar* str = NULL;
    json_object *huawei_obj = NULL;
    json_object *led_obj = NULL;
    gint32 ret;
    (void)json_object_object_get_ex(i_paras->val_jso, RFPROP_OEM_HUAWEI, &huawei_obj);
    ret = json_object_object_get_ex(huawei_obj, RFPROP_PCIE_ALARM_INDICATELED, &led_obj);
    if (ret == FALSE) {
        return str;
    }

    str = dal_json_object_get_str(led_obj);
    return str;
}


LOCAL gint32 set_pciecard_alarm_led(PROVIDER_PARAS_S* paras, OBJ_HANDLE obj_handle)
{
    gint32 ret;
    
    guint8 led_state[3] = { LOCAL_CONTROLOR, ALARM_LIGHT_OFF, ALARM_LIGHT_ON };
    GSList* input_list = NULL;
    const gchar* state_str = NULL;
    guint8 state = ALARM_LIGHT_BUTT;

    state_str = parse_alarm_stat_json(paras);
    if (!g_strcmp0(state_str, PCIE_ALARM_LIGHT_ON)) {
        state = ALARM_LIGHT_ON;
    } else if (!g_strcmp0(state_str, PCIE_ALARM_LIGHT_OFF)) {
        state = ALARM_LIGHT_OFF;
    } else {
        return RFERR_WRONG_PARAM;
    }

    led_state[1] = state; 
    input_list = g_slist_append(input_list, g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, led_state,
                                                                      sizeof(led_state), sizeof(guint8)));
    
    ret = uip_call_class_method_redfish(paras->is_from_webui, paras->user_name, paras->client, obj_handle,
        CLASS_PANGEA_PCIE_CARD, METHOD_SET_ALARM_LED_STATE, AUTH_ENABLE, paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    return ret;
}


LOCAL gint32 get_pciecard_object(OBJ_HANDLE obj_handle, OBJ_HANDLE* target_obj)
{
    guint8 slot = INVALID_VAL;
    gint32 ret;

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_PCIE_CARD_SLOT_ID, &slot);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get card slot id failed, ret=%d.", ret);
        return ret;
    }

    ret = dal_get_specific_object_byte(CLASS_PANGEA_PCIE_CARD, PROPERTY_PCIE_CARD_SLOT_ID, slot, target_obj);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s get slot(%d) pangea pcie card object failed, ret=%d",
                  dfl_get_object_name(obj_handle), slot, ret);
    }

    return ret;
}


LOCAL void get_pcie_alarm_led_state(OBJ_HANDLE obj_handle, const gchar* rsc_name, json_object* o_json_ob)
{
    gint32 ret;
    guint8 led_state;
    GSList* output_list = NULL;
    json_object* val_jso = NULL;
    guint8 slot_id = 0; 
    OBJ_HANDLE pangea_card_obj = 0; 

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_PCIE_CARD_SLOT_ID, &slot_id);
    if (ret != RET_OK) {
        debug_log(DLOG_MASS, "%s get slot id failed, ret=%d", dfl_get_object_name(obj_handle), ret);
        return;
    }

    ret = dal_get_specific_object_byte(CLASS_PANGEA_PCIE_CARD, PROPERTY_PCIE_CARD_SLOT_ID, slot_id, &pangea_card_obj);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s get slot(%d) pangea pcie card object failed, ret=%d",
                  dfl_get_object_name(obj_handle), slot_id, ret);
        return;
    }

    ret = dfl_call_class_method(pangea_card_obj, METHOD_GET_ALARM_LED_STATE, NULL, NULL, &output_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s get slot(%d) pangea pcie card led state failed, ret=%d",
                  dfl_get_object_name(obj_handle), slot_id, ret);
        return;
    }
    led_state = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 0));
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    if (led_state == ALARM_LIGHT_OFF) {
        val_jso = json_object_new_string(PCIE_ALARM_LIGHT_OFF);
    } else if (led_state == ALARM_LIGHT_ON) {
        val_jso = json_object_new_string(PCIE_ALARM_LIGHT_ON);
    } else {
        debug_log(DLOG_ERROR, " %s get slot(%d) led state %d is incorrect.", dfl_get_object_name(pangea_card_obj),
                  slot_id, led_state);
    }
    json_object_object_add(o_json_ob, rsc_name, val_jso);
}


LOCAL void get_chassis_pciedevices_property(OBJ_HANDLE obj_handle, const gchar* property_name,
                                            const gchar* rsc_name, json_object* o_json_ob)
{
    gint32 ret;
    guint8 slot_id = 0; 
    OBJ_HANDLE pangea_card_obj = 0; 

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_PCIE_CARD_SLOT_ID, &slot_id);
    if (ret != RET_OK) {
        debug_log(DLOG_MASS, "%s get slot id failed, ret=%d", dfl_get_object_name(obj_handle), ret);
        return;
    }

    ret = dal_get_specific_object_byte(CLASS_PANGEA_PCIE_CARD, PROPERTY_PCIE_CARD_SLOT_ID, slot_id, &pangea_card_obj);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s get slot(%d) pangea pcie card object failed, ret=%d",
                  dfl_get_object_name(obj_handle), slot_id, ret);
        return;
    }

    ret = rf_obj_handle_data_trans_to_rsc_data(pangea_card_obj, property_name, rsc_name, o_json_ob);
    if (ret != RET_OK) {
        debug_log(DLOG_MASS, "%s: set %s to %s failed, ret=%d",
                  dfl_get_object_name(pangea_card_obj), property_name, rsc_name, ret);
    }
}

LOCAL void get_property_customized_by_cucc(OBJ_HANDLE json_obj_handle, json_object* huawei_jso)
{
    get_oem_property_pciecard_type(json_obj_handle, huawei_jso);
    get_chassis_pciedevices_npu_memory_capacity(json_obj_handle, huawei_jso);
    add_ome_property_video_memory_band_width_from_mcu(json_obj_handle, huawei_jso);
    add_ome_property_rated_power_from_mcu(json_obj_handle, huawei_jso);
    add_ome_property_power_consumed_watts_from_mcu(json_obj_handle, huawei_jso);
    add_ome_property_reading_celsius_from_mcu(json_obj_handle, huawei_jso);
}

LOCAL gboolean check_card_type_for_oem(guint8 card_type)
{
    return card_type == PCIECARD_FUNCTION_FPGA || card_type == PCIECARD_FUNCTION_NPU ||
        card_type == PCIECARD_FUNCTION_GPU || card_type == PCIECARD_FUNCTION_SDI;
}


LOCAL gint32 get_chassis_pciedevices_oem_property(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint8 card_type = 0;
    OBJ_HANDLE slave_card_obj_handle = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    if (check_npu_handle_with_cucc_customized(i_paras->obj_handle)) {
        return get_cucc_pciedevices_oem_property(i_paras, o_result_jso);
    }

    
    gint32 ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_PCIECARD_FUNCTIONCLASS, &card_type);
    
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get function class fail.", __FUNCTION__, __LINE__));
    

    
    json_object *huawei_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != huawei_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: new object fail.", __FUNCTION__, __LINE__));

    
    (void)get_chassis_pciedevices_component_property(i_paras->obj_handle, PROPERTY_COMPONENT_DEVICE_NAME, huawei_jso,
        RFPROP_DEVICELOCATOR);

    
    get_chassis_pciedevice_slotnumber(i_paras->obj_handle, huawei_jso);

    
    (void)get_chassis_pciedevices_component_property(i_paras->obj_handle, PROPERTY_COMPONENT_LOCATION, huawei_jso,
        RFPROP_POSITION);

    
    get_chassis_pciedevices_property(i_paras->obj_handle, PROPERTY_PANGEA_PCIE_MCUVER, RFPROP_MCUVERSION, huawei_jso);
    get_pcie_alarm_led_state(i_paras->obj_handle, RFPROP_PCIE_ALARM_INDICATELED, huawei_jso);

    if (check_card_type_for_oem(card_type)) {
        // 判断是否支持MCU管理
        ret = check_mcu_support_enable(i_paras->obj_handle);
        if (VOS_OK == ret) {
            // 支持管理的则获取MCU版本以及功耗显示为浮点数
            get_chassis_pciedevices_slave_property(i_paras->obj_handle, huawei_jso);
            
            if (dal_is_customized_by_cucc()) {
                get_property_customized_by_cucc(i_paras->obj_handle, huawei_jso);
            }
        } else {
            
            (void)get_chassis_pciedevices_slave_card_property(i_paras->obj_handle, RFPROP_POWER, RFPROP_POWER,
                huawei_jso);
        }
    }

    
    get_ome_property_function_type(i_paras->obj_handle, huawei_jso);
    

    get_oem_property_pciecard_type(i_paras->obj_handle, huawei_jso);

    
    
    ret = get_pcie_slave_card_object_handle(i_paras->obj_handle, &slave_card_obj_handle);
    if (VOS_OK == ret) {
        
        (void)get_pcie_extend_info(slave_card_obj_handle, huawei_jso);
    }
    

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR, json_object_put(huawei_jso);
        debug_log(DLOG_MASS, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    (void)json_object_object_add(*o_result_jso, RFPROP_COMMON_HUAWEI, huawei_jso);
    return HTTP_OK;
}

LOCAL guint8 get_boot_option_num(const gchar *option, guint8 *boot_num)
{
    guint32 size = sizeof(g_get_sdicard_boot_option) / sizeof(GET_SDICARD_BOOT_OPTION);
    for (guint32 i = 0; i < size; i++) {
        if (g_strcmp0(option, g_get_sdicard_boot_option[i].boot_option) == 0) {
            *boot_num = g_get_sdicard_boot_option[i].boot_order;
            return RET_OK;
        }
    }
    return RET_ERR;
}

LOCAL guint8 get_boot_option_valid_type(const gchar *option, guint8 *valid_type)
{
    guint32 size = sizeof(g_get_sdicard_valid_type) / sizeof(GET_SDICARD_BOOT_VALID_TYPE);
    for (guint32 i = 0; i < size; i++) {
        if (g_strcmp0(option, g_get_sdicard_valid_type[i].valid_type_str) == 0) {
            *valid_type = g_get_sdicard_valid_type[i].valid_type;
            return RET_OK;
        }
    }
    return RET_ERR;
}

LOCAL gint32 set_sdi_boot_options(PROVIDER_PARAS_S* i_paras, OBJ_HANDLE obj_handle, guint8 boot_order,
    guint8 valid_type)
{
    GSList *input_list = NULL;
    input_list = g_slist_append(input_list, g_variant_new_byte(boot_order));
    input_list = g_slist_append(input_list, g_variant_new_byte(valid_type));
    gint32 ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_PCIE_SDI_CARD, METHOD_SET_SDI_BOOT_OPTION, AUTH_DISABLE, i_paras->user_role_privilege,
        input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "set_sdi_boot_order failed, ret = %d", ret);
        return RET_ERR;
    }
    return RET_OK;
}

LOCAL gint32 set_huawei_alarm_indicator_led(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso)
{
    OBJ_HANDLE obj_handle = 0;
    
    gint32 ret = get_pciecard_object(i_paras->obj_handle, &obj_handle);
    if (ret != DFL_OK) {
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, "Oem/Huawei/AlarmIndicatorLed",
            o_message_jso, "Oem/Huawei/AlarmIndicatorLed");
        return HTTP_NOT_IMPLEMENTED;
    }

    ret = set_pciecard_alarm_led(i_paras, obj_handle);
    switch (ret) {
        case VOS_OK:
        case RFERR_SUCCESS:
            return HTTP_OK;

        
        case RFERR_NO_RESOURCE:
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri);
            return HTTP_NOT_FOUND;

        
        case RFERR_WRONG_PARAM:
            return HTTP_BAD_REQUEST;

        
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}

LOCAL gint32 set_huawei_boot_option(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret = dal_get_specific_object_position(i_paras->obj_handle, CLASS_PCIE_SDI_CARD, &obj_handle);
    if (ret != RET_OK) {
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, "Oem/Huawei/BootOption",
            o_message_jso, "Oem/Huawei/BootOption");
        return HTTP_NOT_IMPLEMENTED;
    }

    json_object *huawei = json_object_object_get(i_paras->val_jso, RFPROP_COMMON_HUAWEI);
    json_object *boot_option = json_object_object_get(huawei, RFPROP_PCIEDEVICES_BOOTOPTION);
    guint8 boot_num = 0;
    ret = get_boot_option_num(json_object_get_string(boot_option), &boot_num);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get_boot_option_num failed, ret = %d", ret);
        (void)create_message_info(MSGID_INVALID_OBJ, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }
    guint8 valid_type = 0;
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SDI_CARD_BOOT_VALID_TYPE, &valid_type);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get BootOrderValidType failed, ret = %d", ret);
        (void)create_message_info(MSGID_INVALID_OBJ, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }

    ret = set_sdi_boot_options(i_paras, obj_handle, boot_num, valid_type);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "set sdi card boot order failed");
        (void)create_message_info(MSGID_INVALID_OBJ, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }
    return HTTP_OK;
}

LOCAL gint32 set_huawei_boot_effective(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret = dal_get_specific_object_position(i_paras->obj_handle, CLASS_PCIE_SDI_CARD, &obj_handle);
    if (ret != RET_OK) {
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, "Oem/Huawei/BootEffective",
            o_message_jso, "Oem/Huawei/BootEffective");
        return HTTP_NOT_IMPLEMENTED;
    }

    json_object *huawei = json_object_object_get(i_paras->val_jso, RFPROP_COMMON_HUAWEI);
    json_object *boot_effective = json_object_object_get(huawei, RFPROP_PCIEDEVICES_BOOT_EFFECTIVE);
    guint8 valid_type = 0;
    ret = get_boot_option_valid_type(json_object_get_string(boot_effective), &valid_type);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get_boot_option_valid_type failed, ret = %d", ret);
        (void)create_message_info(MSGID_INVALID_OBJ, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }

    guint8 boot_order = 0;
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SDI_CARD_BOOT_ORDER, &boot_order);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get BootOrder failed, ret = %d", ret);
        (void)create_message_info(MSGID_INVALID_OBJ, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }

    ret = set_sdi_boot_options(i_paras, obj_handle, boot_order, valid_type);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "set sdi card boot order failed");
        (void)create_message_info(MSGID_INVALID_OBJ, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }
    return HTTP_OK;
}

LOCAL gint32 set_huawei_property(PROVIDER_PARAS_S* i_paras, json_object **o_message_jso)
{
    gint32 ret;
    json_object *prop_jso = NULL;
    json_object *message = NULL;
    gint32 result = RET_ERR;
    json_object *huawei = json_object_object_get(i_paras->val_jso, RFPROP_COMMON_HUAWEI);
    if (huawei == NULL) {
        debug_log(DLOG_DEBUG, "property Huawei is null");
        return RET_ERR;
    }

    for (guint32 prop_index = 0; prop_index < G_N_ELEMENTS(g_pcie_huawei_handle_method);
        prop_index++) {
        if (json_object_object_get_ex(huawei, g_pcie_huawei_handle_method[prop_index].prop_name,
            &prop_jso) != TRUE) {
            continue;
        }

        if (json_object_get_type(prop_jso) == json_type_null) {
            continue;
        }

        if (g_pcie_huawei_handle_method[prop_index].set_prop == NULL) {
            debug_log(DLOG_DEBUG, "the function pointer is null. (property: %s)",
                g_pcie_huawei_handle_method[prop_index].prop_name);
            continue;
        }

        ret = g_pcie_huawei_handle_method[prop_index].set_prop(i_paras, &message);
        if (ret != HTTP_OK) {
            if (message != NULL) {
                (void)json_object_array_add(*o_message_jso, message);
            }
            continue;
        }

        // patch操作有一个设置ok，整体返回ok
        result = RET_OK;
    }

    return result;
}


LOCAL gint32 set_chassis_pciedevices_oem_property(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
                                                  json_object** o_result_jso)
{
    
    if (o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    if (!(i_paras->user_role_privilege & USERROLE_BASICSETTING)) {
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, "Oem/Huawei/AlarmIndicatorLed",
            o_message_jso, "Oem/Huawei/AlarmIndicatorLed");
        return HTTP_FORBIDDEN;
    }

    gint32 ret = set_huawei_property(i_paras, o_message_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "set_huawei_property failed. (ret:%d)", ret);
        return HTTP_BAD_REQUEST;
    }

    return HTTP_OK;
}

LOCAL gint32 get_max_lanes_for_pciedevices_interface(OBJ_HANDLE obj_handle, json_object **o_result_jso,
    const gchar *str_maxlanes)
{
    gchar lane[LINK_WIDTH_SIZE] = {0};

    gint32 ret = dal_get_property_value_string(obj_handle, PROPETRY_PCIECARD_LINK_WIDTH_ABILITY,
        lane, sizeof(lane));
    if (ret != DFL_OK || rf_string_check(lane) != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get property link width ability failed(%d)", __FUNCTION__, ret);
        json_object_object_add(*o_result_jso, (const gchar*)str_maxlanes, NULL);
    } else {
        gulong value = ULONG_MAX;
        if (dal_strtoul(lane + 1, &value, DECIMAL_NUM) != RET_OK) {
            debug_log(DLOG_ERROR, "%s:dal_strtoi failed", __FUNCTION__);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        if (value < 0 || value > ULONG_MAX) {
            debug_log(DLOG_ERROR, "value is out of range, value is %lu", value);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        guint32 max_lanes = (guint32)value;
        json_object_object_add(*o_result_jso, (const gchar*)str_maxlanes, json_object_new_int((int32_t)max_lanes));
    }
    return HTTP_OK;
}
 
LOCAL gint32 get_lanes_in_use_for_pciedevices_interface(OBJ_HANDLE obj_handle, json_object **o_result_jso)
{
    gchar lane[LINK_WIDTH_SIZE] = {0};

    (void)memset_s(lane, sizeof(lane), 0, sizeof(lane));
    gint32 ret = dal_get_property_value_string(obj_handle, PROPETRY_PCIECARD_LINK_WIDTH, lane, sizeof(lane));
    if (ret != DFL_OK || rf_string_check(lane) != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get property link width failed(%d)", __FUNCTION__, ret);
        json_object_object_add(*o_result_jso, RFPROP_PCIEDEVICES_LANES_IN_USE, NULL);
    } else {
        gulong value = ULONG_MAX;
        if (dal_strtoul(lane + 1, &value, DECIMAL_NUM) != RET_OK) {
            debug_log(DLOG_ERROR, "%s:dal_strtoi failed", __FUNCTION__);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        if (value < 0 || value > ULONG_MAX) {
            debug_log(DLOG_ERROR, "value is out of range, value is %lu", value);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        guint32 lane_in_use = (guint32)value;
        json_object_object_add(*o_result_jso, RFPROP_PCIEDEVICES_LANES_IN_USE,
            json_object_new_int((int32_t)lane_in_use));
    }
    return HTTP_OK;
}


LOCAL gint32 get_chassis_pciedevices_interface(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso)
{
    gchar* string_maxlanes = RFPROP_PCIEDEVICES_MAX_LANES;

    if (dal_is_customized_by_cmcc()) {
        string_maxlanes = RFPROP_PCIEDEVICES_MAX_LANES_CMCC;
    }

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_MASS, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (!i_paras->is_satisfy_privi) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    *o_result_jso = json_object_new_object();
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (check_npu_handle_with_cucc_customized(i_paras->obj_handle)) {
        json_object_object_add(*o_result_jso, RFPROP_PCIEDEVICES_MAX_LANES_CUCC, NULL);
        json_object_object_add(*o_result_jso, RFPROP_PCIEDEVICES_LANES_IN_USE, NULL);
        return HTTP_OK;
    }

    gint32 ret = get_max_lanes_for_pciedevices_interface(i_paras->obj_handle, o_result_jso, string_maxlanes);
    if (ret != HTTP_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = get_lanes_in_use_for_pciedevices_interface(i_paras->obj_handle, o_result_jso);

    return ret;
}


LOCAL void get_eth_pf_info(OBJ_HANDLE obj_handle, json_object *huawei_jso)
{
    json_object *obj_array = NULL;
    json_object *node_array = NULL;

    GSList *output_list = NULL;
    guint16 pfvf = 0;
    guint16 port = 0;
    gchar mac_addr[MACADDRESS_LEN + 1] = {0};
    gsize str_len = 0;
    gsize pfport_info_size = 0;
    const PFPORT_INFO_S *p_pfport_info_tmp = NULL;
    GVariant *property_value = NULL;
    errno_t safe_fun_ret = EOK;

    return_if_expr((obj_handle == 0) || (huawei_jso == NULL));
    debug_log(DLOG_INFO, "%s: obj_handle is %s", __FUNCTION__, dfl_get_object_name(obj_handle));

    gint32 ret = dfl_call_class_method(obj_handle, EXTEND_PCIECARD_METHOD_GET_PFINFO, NULL, NULL, &output_list);
    return_do_info_if_expr(((ret != VOS_OK) || (output_list == NULL)), json_object_put(obj_array);
        debug_log(DLOG_ERROR, "%s: dfl_call_class_method failed, ret = %d.", __FUNCTION__, ret));

    property_value = (GVariant *)g_slist_nth_data(output_list, 0);
    p_pfport_info_tmp =
        (const PFPORT_INFO_S *)g_variant_get_fixed_array(property_value, &pfport_info_size, sizeof(guint8));
    if (p_pfport_info_tmp == NULL) {
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        return;
    }

    obj_array = json_object_new_array();
    do_if_expr(NULL == obj_array, g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref); return );

    for (int i = 0; i < BUSSINESS_PORT_PFINFO_MAX_CNT; i++) {
        pfvf = p_pfport_info_tmp[i].pf;
        port = p_pfport_info_tmp[i].physical_port;
        (void)memset_s(mac_addr, MACADDRESS_LEN + 1, 0, MACADDRESS_LEN + 1);
        safe_fun_ret = strncpy_s(mac_addr, MACADDRESS_LEN + 1, p_pfport_info_tmp[i].mac_addr,
            strlen(p_pfport_info_tmp[i].mac_addr));
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

        if (pfvf == 0xffff) {
            debug_log(DLOG_INFO, " pfvf %d, i = %d mac_addr: %s\n", pfvf, i, mac_addr);
            continue;
        }

        debug_log(DLOG_INFO, "%s: pf %d mac %s len %" G_GSIZE_FORMAT, __FUNCTION__, pfvf, mac_addr, str_len);

        node_array = json_object_new_object();
        if (node_array == NULL) {
            debug_log(DLOG_ERROR, "json_object_new_object node_array fail\n");
            continue;
        }
        // 对外呈现的时候是port1，2，3，4，内部使用0，1，2，3，故在对外显示的时候+ 1操作；
        json_object_object_add(node_array, BUSSINESS_PORT, json_object_new_int((int)(port + 1)));
        json_object_object_add(node_array, BUSSINESS_PORT_PFID, json_object_new_int((int)pfvf));
        json_object_object_add(node_array, BUSSINESS_PORT_PERMANENT_MAC, json_object_new_string(mac_addr));

        json_object_array_add(obj_array, node_array);

        node_array = NULL;
    }

    json_object_object_add(huawei_jso, BUSSINESS_PORT_PFMACINFO, obj_array);
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    return;
}


LOCAL gint32 get_chassis_pciedevices_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint8 health = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_MASS, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    gint32 ret = dal_get_property_value_byte(i_paras->obj_handle,
        g_chassis_pciedevices_provider[i_paras->index].pme_prop_name, &health);
    health = (VOS_OK == ret) ? health : HEALTH_NORMAL;

    const gchar *state_str = (0 == i_paras->obj_handle) ? RF_STATE_ABSENT : RF_STATE_ENABLED;

    (void)get_resource_status_property(&health, NULL, state_str, o_result_jso, FALSE);
    return HTTP_OK;
}

LOCAL void get_chassis_pciedevice_links_odata_id(OBJ_HANDLE chassis_handle, json_object *o_result_jso)
{
    json_object *json_obj = NULL;
    json_object *json_string = NULL;
    gchar str_odata_id[MAX_STRBUF_LEN + 1] = {0};
    gchar slot[MAX_RSC_ID_LEN + 1] = {0};

    gint32 ret = rf_gen_chassis_component_id(NULL, chassis_handle, slot, MAX_RSC_ID_LEN + 1);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_MASS, "%s, %d: get slot id error.\n", __FUNCTION__, __LINE__));

    ret = snprintf_s(str_odata_id, sizeof(str_odata_id), sizeof(str_odata_id) - 1, URI_FORMAT_CHASSIS, slot);
    return_do_info_if_fail(0 < ret, debug_log(DLOG_MASS, "%s, %d: snprintf_s fail.", __FUNCTION__, __LINE__));

    json_string = json_object_new_string(str_odata_id);
    return_do_info_if_fail(NULL != json_string,
        debug_log(DLOG_MASS, "%s, %d: json object new string fail.", __FUNCTION__, __LINE__));

    json_obj = json_object_new_object();
    return_do_info_if_fail(NULL != json_obj, json_object_put(json_string);
        debug_log(DLOG_MASS, "%s, %d: json object new object fail.", __FUNCTION__, __LINE__));

    (void)json_object_object_add(json_obj, RFPROP_ODATA_ID, json_string);
    ret = json_object_array_add(o_result_jso, json_obj);
    do_if_fail(0 == ret, json_object_put(json_obj));

    return;
}


gint32 redfish_check_pciecard_netcard_get_handle(OBJ_HANDLE i_pcie_handle, OBJ_HANDLE *o_netcard_handle)
{
    guint8 card_type = 0;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    gchar pcie_ref_obj[MAX_NAME_SIZE] = {0};
    gchar pcie_ref_property[MAX_NAME_SIZE] = {0};
    gchar netcard_ref_object[MAX_NAME_SIZE] = {0};
    gchar netcard_ref_property[MAX_NAME_SIZE] = {0};

    
    gint32 ret = dfl_get_referenced_property(i_pcie_handle, PROPETRY_PCIECARD_SLOT, pcie_ref_obj, pcie_ref_property,
        MAX_NAME_SIZE, MAX_NAME_SIZE);
    return_val_do_info_if_fail(ret == VOS_OK, VOS_ERR,
        debug_log(DLOG_MASS, "%s, %d: get referenced property fail.", __FUNCTION__, __LINE__));

    
    ret = dfl_get_object_list(CLASS_NETCARD_NAME, &obj_list);
    return_val_do_info_if_fail(ret == VOS_OK, VOS_ERR,
        debug_log(DLOG_MASS, "%s, %d: get obj list fail.", __FUNCTION__, __LINE__));

    for (obj_node = obj_list; obj_node; obj_node = g_slist_next(obj_node)) {
        ret = dal_get_property_value_byte((OBJ_HANDLE)(obj_node->data), PROPERTY_NETCARD_CARDTYPE, &card_type);
        do_if_fail(VOS_OK == ret, continue);

        if ((card_type == NET_TYPE_PCIE) || (card_type == NET_TYPE_OCP_CARD) || (card_type == NET_TYPE_OCP2_CARD)) {
            ret = dfl_get_referenced_property((OBJ_HANDLE)(obj_node->data), PROPERTY_NETCARD_SLOT_ID,
                netcard_ref_object, netcard_ref_property, MAX_NAME_SIZE, MAX_NAME_SIZE);
            do_if_fail(ret == VOS_OK, continue);

            
            if (g_strcmp0(pcie_ref_obj, netcard_ref_object) == 0) {
                *o_netcard_handle = (OBJ_HANDLE)(obj_node->data);
                g_slist_free(obj_list);
                return VOS_OK;
            }
        }
    }

    g_slist_free(obj_list);
    return VOS_ERR;
}

LOCAL void chassis_pciedevices_add_odata_id_to_array(OBJ_HANDLE chassis_handle, gchar *member_id, guint8 num,
    json_object *jso_array, guint32 id_len)
{
    json_object *json_obj = NULL;
    json_object *json_string = NULL;
    gchar str_odata_id[MAX_STRBUF_LEN + 1] = {0};
    gchar slot[MAX_RSC_ID_LEN + 1] = {0};

    return_if_expr(sizeof(str_odata_id) < id_len);
    gint32 ret = rf_gen_chassis_component_id(NULL, chassis_handle, slot, MAX_RSC_ID_LEN + 1);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_MASS, "%s, %d: get slot id error.\n", __FUNCTION__, __LINE__));

    ret = snprintf_s(str_odata_id, sizeof(str_odata_id), sizeof(str_odata_id) - 1, URI_FORMAT_PCIEFUNCTION_URI_NUMBER,
        slot, member_id, num);
    return_do_info_if_fail(0 < ret, debug_log(DLOG_MASS, "%s, %d: snprintf_s fail.\n", __FUNCTION__, __LINE__));

    json_obj = json_object_new_object();
    return_do_info_if_fail(NULL != json_obj,
        debug_log(DLOG_MASS, "%s, %d: json object new object fail.", __FUNCTION__, __LINE__));

    json_string = json_object_new_string(str_odata_id);
    return_do_info_if_fail(NULL != json_string, json_object_put(json_obj);
        debug_log(DLOG_MASS, "%s, %d: json object new string fail.", __FUNCTION__, __LINE__));

    (void)json_object_object_add(json_obj, RFPROP_ODATA_ID, json_string);

    ret = json_object_array_add(jso_array, json_obj);
    do_if_fail(0 == ret, json_object_put(json_obj));

    return;
}

LOCAL void chassis_pciedevice_netcard_odata_id_to_array(OBJ_HANDLE chassis_handle, OBJ_HANDLE pcie_handle,
    gchar *member_id, guint32 member_id_len, json_object *o_result_jso)
{
    guint8 all_dbf_number = 0;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    const gchar *object_name = NULL;
    gchar ref_card_name[MAX_RSC_ID_LEN] = {0};
    guint8 bdf_number = 0;
    OBJ_HANDLE netcard_handle = 0;

    return_do_info_if_expr(0 == member_id_len, debug_log(DLOG_MASS, "%s: member_id_len is null.", __FUNCTION__));

    gint32 ret = redfish_check_pciecard_netcard_get_handle(pcie_handle, &netcard_handle);
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_MASS, "%s, %d: get object handle fail.", __FUNCTION__, __LINE__));

    object_name = dfl_get_object_name(netcard_handle);
    if (!object_name) {
        debug_log(DLOG_MASS, "%s, %d: get object name fail.", __FUNCTION__, __LINE__);
        return;
    }

    ret = dfl_get_object_list(ETH_CLASS_NAME_BUSY_ETH, &obj_list);
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_MASS, "%s, %d: get object list fail.", __FUNCTION__, __LINE__));

    
    for (obj_node = obj_list; obj_node; obj_node = g_slist_next(obj_node)) {
        ret = dal_get_property_value_string((OBJ_HANDLE)(obj_node->data), BUSY_ETH_ATTRIBUTE_REF_NETCARD, ref_card_name,
            sizeof(ref_card_name));
        continue_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_MASS, "%s:get ref card fail.", __FUNCTION__));

        debug_log(DLOG_INFO, "object name:[%s], ref card name [%s]", object_name, ref_card_name);

        if (0 == g_strcmp0(object_name, ref_card_name)) {
            ret = dal_get_property_value_byte((OBJ_HANDLE)(obj_node->data), PROPERTY_BDF_NO, &bdf_number);
            do_if_fail(VOS_OK == ret, continue);
            
            all_dbf_number |= 1 << bdf_number;
        }
    }

    g_slist_free(obj_list);

    debug_log(DLOG_INFO, "%s:all bdf number is %d", __FUNCTION__, all_dbf_number);

    
    for (gint32 i = 1; i <= 8; i++) {
        if (all_dbf_number & 0x1) {
            (void)chassis_pciedevices_add_odata_id_to_array(chassis_handle, member_id, i, o_result_jso, member_id_len);
        }

        all_dbf_number >>= 1;
    }

    return;
}

gint32 get_chassis_pciedevices_function_odata_id(OBJ_HANDLE chassis_handle, OBJ_HANDLE i_pcie_handle,
    json_object *o_result_jso)
{
    guint8 functionclass = 0;
    gchar member_id[MAX_STRBUF_LEN + 1] = {0};
    guchar pcie_card_slot = INVALID_VAL;
    const gchar *card_prefix_ptr = NULL;

    
    return_val_if_fail(0 != i_pcie_handle, VOS_OK);

    card_prefix_ptr = get_chassis_pciedevices_cardtype(i_pcie_handle);
    gint32 ret = dal_get_property_value_byte(i_pcie_handle, PROPERTY_PCIE_CARD_LOGIC_SLOT, &pcie_card_slot);
    if (ret != RET_OK || pcie_card_slot == INVALID_VAL) {
        ret = dal_get_property_value_byte(i_pcie_handle, PROPERTY_PCIE_CARD_SLOT_ID, &pcie_card_slot);
    }
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "cann't get property value for [%s]", PROPERTY_PCIE_CARD_SLOT_ID));

    ret = snprintf_s(member_id, MAX_STRBUF_LEN + 1, MAX_STRBUF_LEN, "%s%u", card_prefix_ptr, pcie_card_slot);
    return_val_do_info_if_fail(ret > 0, VOS_ERR,
        debug_log(DLOG_MASS, "%s, %d: location devicename fail .", __FUNCTION__, __LINE__));

    
    ret = dal_get_property_value_byte(i_pcie_handle, PROPERTY_PCIECARD_FUNCTIONCLASS, &functionclass);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get property string value fail.", __FUNCTION__, __LINE__));

    
    if (PCIECARD_FUNCTION_NETCARD == functionclass) {
        (void)chassis_pciedevice_netcard_odata_id_to_array(chassis_handle, i_pcie_handle, member_id, sizeof(member_id),
            o_result_jso);
        return VOS_OK;
    }

    
    (void)chassis_pciedevices_add_odata_id_to_array(chassis_handle, member_id, RFPROP_PCIEFUNCTION_MEMBERID,
        o_result_jso, sizeof(member_id));
    return VOS_OK;
}


LOCAL gint32 get_chassis_pciedevices_links(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    json_object *chassis_json_array = NULL;
    json_object *function_json_array = NULL;
    OBJ_HANDLE chassis_handle;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_MASS, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    chassis_handle = (OBJ_HANDLE)g_slist_nth_data(i_paras->parent_obj_handle_list, 0);

    
    *o_result_jso = json_object_new_object();
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    chassis_json_array = json_object_new_array();
    do_if_expr(NULL == chassis_json_array, json_object_put(*o_result_jso); *o_result_jso = NULL;
        return HTTP_INTERNAL_SERVER_ERROR);

    
    (void)get_chassis_pciedevice_links_odata_id(chassis_handle, chassis_json_array);
    (void)json_object_object_add(*o_result_jso, RFPROP_CHASSIS, chassis_json_array);

    function_json_array = json_object_new_array();
    do_if_expr(NULL == function_json_array, json_object_put(*o_result_jso); *o_result_jso = NULL;
        return HTTP_INTERNAL_SERVER_ERROR);

    if (!check_npu_handle_with_cucc_customized(i_paras->obj_handle)) {
        
        (void)get_chassis_pciedevices_function_odata_id(chassis_handle, i_paras->obj_handle, function_json_array);
    }
    (void)json_object_object_add(*o_result_jso, RFPROP_CHASSIS_PCIEFUNCTIONS, function_json_array);
    return HTTP_OK;
}

gint32 chassis_pciedevice_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gboolean b_ret;
    OBJ_HANDLE chassis_comp_handle = OBJ_HANDLE_COMMON;

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    b_ret = redfish_check_chassis_uri_valid(i_paras->uri, &chassis_comp_handle);
    if (FALSE == b_ret || (0 == chassis_comp_handle)) {
        return HTTP_NOT_FOUND;
    }

    
    return_val_if_fail(VOS_OK == check_enclosure_component_type(chassis_comp_handle, FALSE), HTTP_NOT_FOUND);
    

    i_paras->parent_obj_handle_list = g_slist_append(i_paras->parent_obj_handle_list, (gpointer)chassis_comp_handle);

    
    if (redfish_check_chassis_pciedevice_uri_effective(i_paras->uri, &(i_paras->obj_handle))) {
        *prop_provider = g_chassis_pciedevices_provider;
        *count = sizeof(g_chassis_pciedevices_provider) / sizeof(PROPERTY_PROVIDER_S);
        return VOS_OK;
    }

    return HTTP_NOT_FOUND;
}


gint32 chassis_pciedevice_etag_del_property(json_object *object)
{
    SPECIAL_PROP_S value[] = {
        {3, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RFPROP_POWER, NULL, NULL}},
        {0, {NULL, NULL, NULL, NULL, NULL}}
    };
    return rsc_del_none_etag_affected_property(object, value, G_N_ELEMENTS(value));
}


LOCAL gint32 get_pcie_slave_card_object_handle(OBJ_HANDLE pcie_obj_handle, OBJ_HANDLE *slave_card_obj_handle)
{
    guint8 slot_id = 0;
    gchar slave_card[MAX_CLASS_NAME + 1] = {0};

    
    return_val_do_info_if_fail(NULL != slave_card_obj_handle, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: Input param error.", __func__));

    
    gint32 ret = dal_get_property_value_byte(pcie_obj_handle, PROPERTY_PCIE_CARD_SLOT_ID, &slot_id);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get prop PcieCardSlot fail(%d).", __func__, ret));

    
    ret = dal_get_property_value_string(pcie_obj_handle, PROPERTY_PCIE_CARD_SLAVE_CARD, slave_card, sizeof(slave_card));
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get prop SlaveCard fail(%d).", __func__, ret));

    if (0 == strlen(slave_card)) {
        
        *slave_card_obj_handle = pcie_obj_handle;
    } else {
        
        ret = dal_get_specific_object_byte(slave_card, PROPERTY_CARD_SLOT, slot_id, slave_card_obj_handle);
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            debug_log(DLOG_DEBUG, "%s: get object handle of class %s fail(%d), slot_id is %d.", __func__, slave_card,
            ret, slot_id));
    }

    return VOS_OK;
}


LOCAL void get_pcie_extend_info(OBJ_HANDLE obj_handle, json_object *huawei_jso)
{
    guint32 i;
    gchar class_name[MAX_CLASS_NAME + 1] = {0};

    
    return_do_info_if_fail(NULL != huawei_jso, debug_log(DLOG_ERROR, "%s: input param error.", __func__));

    gint32 ret = dfl_get_class_name(obj_handle, class_name, (MAX_CLASS_NAME + 1));
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s: get object name fail(%d).", __func__, ret));

    
    for (i = 0; i < G_N_ELEMENTS(g_pcie_extend_info_provider); i++) {
        break_if_expr(0 == g_strcmp0(class_name, g_pcie_extend_info_provider[i].class_name));
    }
    return_do_info_if_fail(i < G_N_ELEMENTS(g_pcie_extend_info_provider),
        debug_log(DLOG_ERROR, "%s: invalid class name %s.", __func__, class_name));

    
    if (NULL != g_pcie_extend_info_provider[i].pfn_get) {
        (void)g_pcie_extend_info_provider[i].pfn_get(obj_handle, huawei_jso);
    }
}


LOCAL void get_pciecard_extend_info(OBJ_HANDLE obj_handle, json_object *huawei_jso)
{
    json_object *prop_jso = NULL;

    
    return_do_info_if_fail(NULL != huawei_jso, debug_log(DLOG_ERROR, "%s: input param error.", __func__));

    
    (void)rf_add_property_jso_string(obj_handle, PROPERTY_PCIE_CARD_SLAVE_CARD_NAME, RFPROP_PCIEDEVICES_PRODUCT_NAME,
        huawei_jso);

    
    (void)rf_add_property_jso_uint16_hex(obj_handle, PROPERTY_PCIE_CARD_BOARD_ID, RFPROP_PCIEDEVICES_BOARDID,
        huawei_jso);

    
    (void)rf_add_property_jso_string(obj_handle, PROPERTY_PCIE_CARD_PCB_VER, RFPROP_PCIEDEVICES_PCB_VERSION,
        huawei_jso);

    
    (void)get_pciecard_extend_version_info(obj_handle, RFPROP_PCIEDEVICES_CPLD_VERSION, &prop_jso);
    (void)json_object_object_add(huawei_jso, RFPROP_PCIEDEVICES_CPLD_VERSION, prop_jso);
}


LOCAL gint32 get_pciecard_extend_version_info(OBJ_HANDLE obj_handle, const gchar *request_prop,
    json_object **o_result_jso)
{
    gchar firmware[MAX_STRBUF_LEN + 1] = {0};
    OBJ_HANDLE firmware_obj_handle = 0;
    guint8 classifications = 0;
    gchar location[MAX_STRBUF_LEN + 1] = {0};
    gchar version[MAX_STRBUF_LEN + 1] = {0};
    gchar prop_val[PROPERTY_LEN + 1] = {0};

    
    return_val_do_info_if_fail(NULL != request_prop && NULL != o_result_jso, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: input param error.", __func__));

    
    gint32 ret = dal_get_property_value_string(obj_handle, PROPERTY_PCIE_CARD_FIRMWARE_OBJ, firmware, sizeof(firmware));
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get prop Firmware fail(%d).", __func__, ret));

    
    ret = dfl_get_object_handle(firmware, &firmware_obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_DEBUG, "%s: get object handle of class Firmware fail(%d), object name is %s.", __func__, ret,
        firmware));

    
    ret = dal_get_property_value_byte(firmware_obj_handle, PROPERTY_SOFTWARE_CLASSIFICATIONS, &classifications);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get prop Classifications fail(%d).", __func__, ret));

    if (FW_CLASSIFICATIONS_BACKPLANE_CPLD == classifications || FW_CLASSIFICATIONS_CPLD == classifications) {
        
        return_val_do_info_if_fail(0 == g_strcmp0(RFPROP_PCIEDEVICES_CPLD_VERSION, request_prop), VOS_ERR,
            debug_log(DLOG_DEBUG, "%s: device is a CPLD, while requested prop is not CPLDVersion.", __func__));
    } else {
        
        return_val_do_info_if_fail(0 == g_strcmp0(RFPROP_PCIEDEVICES_FIRMWAREVERSION, request_prop), VOS_ERR,
            debug_log(DLOG_DEBUG, "%s: device is a firmware, while requested prop is not FirmwareVersion.", __func__));
    }

    
    ret = dal_get_property_value_string(firmware_obj_handle, PROPERTY_SOFTWARE_VERSIONSTRING, version, sizeof(version));
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get prop VersionString fail(%d).", __func__, ret));
    return_val_do_info_if_expr(0 == strlen(version) || 0 == g_strcmp0(version, STRING_VALUE_IS_NA), VOS_ERR,
        debug_log(DLOG_DEBUG, "%s: invalid value %s for prop VersionString.", __func__, version));

    
    ret = dal_get_property_value_string(firmware_obj_handle, PROPERTY_SOFTWARE_LOCATION, location, sizeof(location));
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get prop Location fail(%d).", __func__, ret));

    if (0 == strlen(location) || 0 == g_strcmp0(location, STRING_VALUE_IS_NA)) {
        ret = snprintf_s(prop_val, sizeof(prop_val), sizeof(prop_val) - 1, "%s", version);
        do_val_if_expr(ret <= 0, debug_log(DLOG_DEBUG, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret));
    } else {
        ret = snprintf_s(prop_val, sizeof(prop_val), sizeof(prop_val) - 1, "(%s)%s", location, version);
        do_val_if_expr(ret <= 0, debug_log(DLOG_DEBUG, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret));
    }

    *o_result_jso = json_object_new_string(prop_val);
    return_val_do_info_if_fail(NULL != *o_result_jso, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: json_object_new_string fail.", __func__));

    return VOS_OK;
}


LOCAL gint32 get_pciecard_extend_firmwareversion(OBJ_HANDLE pcie_obj_handle, json_object **o_result_jso)
{
    OBJ_HANDLE slave_card_obj_handle = 0;
    gchar slave_card_class_name[MAX_CLASS_NAME + 1] = {0};

    
    gint32 ret = get_pcie_slave_card_object_handle(pcie_obj_handle, &slave_card_obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get object handle of slave card fail(%d)", __func__, ret));

    
    ret = dfl_get_class_name(slave_card_obj_handle, slave_card_class_name, (MAX_CLASS_NAME + 1));
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get object name fail(%d).", __func__, ret));

    
    return_val_if_fail(0 == g_strcmp0(CLASS_PCIE_CARD, slave_card_class_name), VOS_ERR);

    
    ret = get_pciecard_extend_version_info(slave_card_obj_handle, RFPROP_PCIEDEVICES_FIRMWAREVERSION, o_result_jso);
    return ret;
}


LOCAL void get_fpgacard_extend_info(OBJ_HANDLE obj_handle, json_object *huawei_jso)
{
    
    return_do_info_if_fail(NULL != huawei_jso, debug_log(DLOG_ERROR, "%s: input param error.", __func__));

    
    (void)rf_add_property_jso_string(obj_handle, PROPERTY_FPGA_CARD_NAME, RFPROP_PCIEDEVICES_PRODUCT_NAME, huawei_jso);

    
    (void)rf_add_property_jso_uint16_hex(obj_handle, PROPERTY_FPGA_CARD_BOARDID, RFPROP_PCIEDEVICES_BOARDID,
        huawei_jso);

    
    (void)rf_add_property_jso_string(obj_handle, PROPERTY_FPGA_CARD_PCBVER, RFPROP_PCIEDEVICES_PCB_VERSION, huawei_jso);

    return;
}


LOCAL void get_sdicard_extend_info(OBJ_HANDLE obj_handle, json_object *huawei_jso)
{
    json_object *property_jso = NULL;

    
    return_do_info_if_fail(NULL != huawei_jso, debug_log(DLOG_ERROR, "%s: input param error.", __func__));

    
    (void)rf_add_property_jso_string(obj_handle, PROPERTY_SDI_CARD_BOARD_NAME, RFPROP_PCIEDEVICES_PRODUCT_NAME,
        huawei_jso);

    
    (void)rf_add_property_jso_uint16_hex(obj_handle, PROPERTY_SDI_CARD_BOARDID, RFPROP_PCIEDEVICES_BOARDID, huawei_jso);

    
    (void)rf_add_property_jso_string(obj_handle, PROPERTY_PCIE_CARD_PCB_VER, RFPROP_PCIEDEVICES_PCB_VERSION,
        huawei_jso);

    
    (void)get_sdicard_extend_m2_presence(obj_handle, huawei_jso);

    if (TRUE == is_sdi_sodimm_presence_support(huawei_jso)) {
        
        (void)get_sdicard_extend_sodimm_presence(obj_handle, huawei_jso);
    }

    
    (void)get_sdicard_extend_storage_ip(obj_handle, huawei_jso);

    
    (void)get_sdicard_extend_storage_vlanid(obj_handle, huawei_jso);

    
    (void)get_sdicard_extend_boot_option(obj_handle, huawei_jso);

    
    (void)get_sdicard_extend_boot_option_valid_type(obj_handle, huawei_jso);

    property_jso = json_object_new_array();
    return_do_info_if_fail(NULL != property_jso,
        (void)json_object_object_add(huawei_jso, RFPROP_PCIEDEVICES_EXTENDCARD_INFO, json_object_new_array());
        debug_log(DLOG_ERROR, "%s: json_object_new_array fail.", __func__));

    
    if (TRUE == check_sdicard_extendcard_presence(obj_handle)) {
        (void)get_sdicard_extendcard_info(obj_handle, property_jso);
    }

    (void)json_object_object_add(huawei_jso, RFPROP_PCIEDEVICES_EXTENDCARD_INFO, property_jso);
    return;
}


LOCAL void get_sdicard_extend_m2_presence(OBJ_HANDLE obj_handle, json_object *huawei_jso)
{
    guint8 m2_presence = 0;
    gboolean m2_device1_presence;
    gboolean m2_device2_presence;
    json_object *property_jso = NULL;

    
    return_do_info_if_fail(NULL != huawei_jso, debug_log(DLOG_ERROR, "%s: input param error.", __func__));

    
    gint32 ret = dal_get_property_value_byte(obj_handle, PROPERTY_SDI_CARD_M2, &m2_presence);
    return_do_info_if_fail(VOS_OK == ret,
        (void)json_object_object_add(huawei_jso, RFPROP_PCIEDEVICES_M2DEVICE1_PRESENCE, NULL);
        (void)json_object_object_add(huawei_jso, RFPROP_PCIEDEVICES_M2DEVICE2_PRESENCE, NULL);
        debug_log(DLOG_ERROR, "%s: get prop M2Presence fail(%d).", __func__, ret));

    switch (m2_presence) {
        
        case 1:
            m2_device1_presence = TRUE;
            m2_device2_presence = FALSE;
            break;
        
        case 2:
            m2_device1_presence = FALSE;
            m2_device2_presence = TRUE;
            break;
        
        case 3:
            m2_device1_presence = TRUE;
            m2_device2_presence = TRUE;
            break;
        
        default:
            m2_device1_presence = FALSE;
            m2_device2_presence = FALSE;
            break;
    }

    property_jso = json_object_new_boolean(m2_device1_presence);
    do_if_fail(NULL != property_jso,
        debug_log(DLOG_ERROR, "%s: json_object_new_boolean for m2 device1 fail.", __func__));
    (void)json_object_object_add(huawei_jso, RFPROP_PCIEDEVICES_M2DEVICE1_PRESENCE, property_jso);

    property_jso = json_object_new_boolean(m2_device2_presence);
    do_if_fail(NULL != property_jso,
        debug_log(DLOG_ERROR, "%s: json_object_new_boolean for m2 device2 fail.", __func__));
    (void)json_object_object_add(huawei_jso, RFPROP_PCIEDEVICES_M2DEVICE2_PRESENCE, property_jso);

    return;
}


LOCAL void get_sdicard_extend_sodimm_presence(OBJ_HANDLE obj_handle, json_object *huawei_jso)
{
    guint8 sodimm_presence = 0;
    gboolean sodimm00_presence;
    gboolean sodimm10_presence;
    json_object *property_jso = NULL;

    
    return_do_info_if_fail(NULL != huawei_jso, debug_log(DLOG_ERROR, "%s: input param error.", __func__));

    
    gint32 ret = dal_get_property_value_byte(obj_handle, PROPERTY_SDI_CARD_DIMM, &sodimm_presence);
    return_do_info_if_fail(VOS_OK == ret,
        (void)json_object_object_add(huawei_jso, RFPROP_PCIEDEVICES_SODIMM00_PRESENCE, NULL);
        (void)json_object_object_add(huawei_jso, RFPROP_PCIEDEVICES_SODIMM10_PRESENCE, NULL);
        debug_log(DLOG_ERROR, "%s: get prop SODIMMPresence fail(%d).", __func__, ret));

    switch (sodimm_presence) {
        
        case 1:
            sodimm00_presence = TRUE;
            sodimm10_presence = FALSE;
            break;
        
        case 2:
            sodimm00_presence = FALSE;
            sodimm10_presence = TRUE;
            break;
        
        case 3:
            sodimm00_presence = TRUE;
            sodimm10_presence = TRUE;
            break;
        
        default:
            sodimm00_presence = FALSE;
            sodimm10_presence = FALSE;
            break;
    }

    property_jso = json_object_new_boolean(sodimm00_presence);
    do_if_fail(NULL != property_jso, debug_log(DLOG_ERROR, "%s: json_object_new_boolean for SODIMM00 fail.", __func__));
    (void)json_object_object_add(huawei_jso, RFPROP_PCIEDEVICES_SODIMM00_PRESENCE, property_jso);

    property_jso = json_object_new_boolean(sodimm10_presence);
    do_if_fail(NULL != property_jso, debug_log(DLOG_ERROR, "%s: json_object_new_boolean for SODIMM10 fail.", __func__));
    (void)json_object_object_add(huawei_jso, RFPROP_PCIEDEVICES_SODIMM10_PRESENCE, property_jso);

    return;
}


LOCAL void get_sdicard_extend_storage_ip(OBJ_HANDLE obj_handle, json_object *huawei_jso)
{
    guint32 ip_val = 0;
    gchar ip_addr[IPV4_IP_STR_SIZE + 1] = {0};
    json_object *property_jso = NULL;

    
    return_do_info_if_fail(NULL != huawei_jso, debug_log(DLOG_ERROR, "%s: input param error.", __func__));

    
    gint32 ret = dal_get_property_value_uint32(obj_handle, PROPERTY_SDI_CARD_STORAGE_IP_ADDR, &ip_val);
    return_do_info_if_fail(VOS_OK == ret,
        (void)json_object_object_add(huawei_jso, RFPROP_PCIEDEVICES_STORAGE_MANAGEMENT_IP, NULL);
        debug_log(DLOG_ERROR, "%s: get prop StorageIpAddr fail(%d).", __func__, ret));

    
    (void)snprintf_s(ip_addr, sizeof(ip_addr), sizeof(ip_addr) - 1, "%d.%d.%d.%d", ip_val & 0xFF, (ip_val >> 8) & 0xFF,
        (ip_val >> 16) & 0xFF, (ip_val >> 24) & 0xFF);

    property_jso = json_object_new_string(ip_addr);
    do_if_fail(NULL != property_jso, debug_log(DLOG_ERROR, "%s: json_object_new_string fail.", __func__));

    (void)json_object_object_add(huawei_jso, RFPROP_PCIEDEVICES_STORAGE_MANAGEMENT_IP, property_jso);

    return;
}


LOCAL void get_sdicard_extend_storage_vlanid(OBJ_HANDLE obj_handle, json_object *huawei_jso)
{
    guint16 vlanid = 0;
    json_object *property_jso = NULL;

    
    return_do_info_if_fail(NULL != huawei_jso, debug_log(DLOG_ERROR, "%s: input param error.", __func__));

    
    gint32 ret = dal_get_property_value_uint16(obj_handle, PROPERTY_SDI_CARD_STORAGE_VLAN, &vlanid);
    return_do_info_if_fail(VOS_OK == ret,
        (void)json_object_object_add(huawei_jso, RFPROP_PCIEDEVICES_STORAGE_MANAGEMENT_VLANID, NULL);
        debug_log(DLOG_ERROR, "%s: get prop StorageIpVlan fail(%d).", __func__, ret));

    
    vlanid = ((vlanid & 255) << 8) + ((vlanid >> 8) & 255);
    return_do_info_if_fail(vlanid > 0 && vlanid < STORAGE_MANAGEMENT_VLANID_INVALID,
        (void)json_object_object_add(huawei_jso, RFPROP_PCIEDEVICES_STORAGE_MANAGEMENT_VLANID, NULL);
        debug_log(DLOG_DEBUG, "%s: invalid value %d for prop StorageIpVlan.", __func__, vlanid));

    property_jso = json_object_new_int(vlanid);
    do_if_fail(NULL != property_jso, debug_log(DLOG_ERROR, "%s: json_object_new_int fail.", __func__));

    (void)json_object_object_add(huawei_jso, RFPROP_PCIEDEVICES_STORAGE_MANAGEMENT_VLANID, property_jso);

    return;
}

LOCAL void get_sdicard_extend_boot_option_valid_type(OBJ_HANDLE obj_handle, json_object *huawei_jso)
{
    guint8 boot_order_valid_type = 0;
    json_object *property_jso = NULL;
    gint32 ret = dal_get_property_value_byte(obj_handle, PROPERTY_SDI_CARD_BOOT_VALID_TYPE, &boot_order_valid_type);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get prop BootOrderValidType fail(%d).", ret);
        return;
    }

    
    property_jso = json_object_new_string(BOOTORDER_ONCE);
    for (guint8 i = 0; i < G_N_ELEMENTS(g_get_sdicard_valid_type); i++) {
        if (boot_order_valid_type == g_get_sdicard_valid_type[i].valid_type) {
            property_jso = json_object_new_string(g_get_sdicard_valid_type[i].valid_type_str);
            break;
        }
    }

    if (property_jso == NULL) {
        debug_log(DLOG_ERROR, "json_object_new_string fail.");
    }

    (void)json_object_object_add(huawei_jso, RFPROP_PCIEDEVICES_BOOT_EFFECTIVE, property_jso);

    return;
}


LOCAL void get_sdicard_extend_boot_option(OBJ_HANDLE obj_handle, json_object *huawei_jso)
{
    guint8 boot_order = 0;
    json_object *property_jso = NULL;

    
    return_do_info_if_fail(NULL != huawei_jso, debug_log(DLOG_ERROR, "%s: input param error.", __func__));

    
    gint32 ret = dal_get_property_value_byte(obj_handle, PROPERTY_SDI_CARD_BOOT_ORDER, &boot_order);
    return_do_info_if_fail(VOS_OK == ret, (void)json_object_object_add(huawei_jso, RFPROP_PCIEDEVICES_BOOTOPTION, NULL);
        debug_log(DLOG_ERROR, "%s: get prop BootOrder fail(%d).", __func__, ret));

    
    property_jso = json_object_new_string(BOOTOPTION_HDD);
    for (guint8 i = 0; i < G_N_ELEMENTS(g_get_sdicard_boot_option); i++) {
        if (boot_order == g_get_sdicard_boot_option[i].boot_order) {
            property_jso = json_object_new_string(g_get_sdicard_boot_option[i].boot_option);
            break;
        }
    }
    do_if_fail(NULL != property_jso, debug_log(DLOG_ERROR, "%s: json_object_new_string fail.", __func__));

    (void)json_object_object_add(huawei_jso, RFPROP_PCIEDEVICES_BOOTOPTION, property_jso);

    return;
}


LOCAL gboolean is_sdi_sodimm_presence_support(json_object *property_jso)
{
#define PROPERTY_SDICARD_BOARDID_V3 "0x00d0"
#define PROPERTY_SDICARD_BOARDID_V5 "0x00d1"
#define PROPERTY_SDICARD_BOARDID_FD "0x00fd"
#define PROPERTY_SDICARD_BOARDID_5E "0x005e"

    gboolean ret = FALSE;
    json_bool ret_val;
    json_object *new_jso = NULL;
    const gchar *boardid = NULL;

    return_val_do_info_if_fail(NULL != property_jso, FALSE, debug_log(DLOG_ERROR, "%s: input param error.", __func__));

    ret_val = json_object_object_get_ex(property_jso, RFPROP_PCIEDEVICES_BOARDID, &new_jso);
    return_val_do_info_if_expr(ret_val != TRUE, FALSE,
        debug_log(DLOG_ERROR, "%s: json_object_object_get_ex error.ret is %d.", __func__, ret_val));

    boardid = dal_json_object_get_str(new_jso);
    return_val_do_info_if_expr(boardid == NULL, FALSE,
        debug_log(DLOG_ERROR, "%s: dal_json_object_get_str error.", __func__));

    if (0 == g_strcmp0(boardid, PROPERTY_SDICARD_BOARDID_V3) || 0 == g_strcmp0(boardid, PROPERTY_SDICARD_BOARDID_V5) ||
        0 == g_strcmp0(boardid, PROPERTY_SDICARD_BOARDID_FD)) {
        ret = TRUE;
    }
    return ret;
}


LOCAL gboolean check_sdicard_extendcard_presence(OBJ_HANDLE sdi_obj_handle)
{
    guint8 netcard_presence = 0;

    
    gint32 ret = dal_get_property_value_byte(sdi_obj_handle, PROPERTY_SDI_CARD_NETCARD, &netcard_presence);
    return_val_do_info_if_fail(VOS_OK == ret, FALSE,
        debug_log(DLOG_ERROR, "%s: get prop NetCardPresence fail(%d).", __func__, ret));

    return_val_do_info_if_fail(netcard_presence >= 1 && netcard_presence <= 3, FALSE,
        debug_log(DLOG_DEBUG, "%s: extend card of SDI card is absence.", __func__));

    return TRUE;
}


LOCAL void get_sdicard_extendcard_info(OBJ_HANDLE sdi_obj_handle, json_object *property_jso)
{
    guint8 slot_id = 0;
    OBJ_HANDLE pcie_obj_handle = 0;
    const gchar *pcie_obj_name = NULL;
    gchar basecard_obj_name[MAX_OBJECT_NAME_LEN] = {0};
    GSList *card_list = NULL;
    GSList *list_item = NULL;
    OBJ_HANDLE object_handle = 0;

    
    return_do_info_if_fail(NULL != property_jso, debug_log(DLOG_ERROR, "%s: input param error.", __func__));

    
    gint32 ret = dal_get_property_value_byte(sdi_obj_handle, PROPERTY_PCIESSDCARD_SLOT, &slot_id);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s: get prop PcieCardSlot fail(%d).", __func__, ret));

    
    ret = dal_get_specific_object_byte(CLASS_PCIE_CARD, PROPERTY_PCIE_CARD_SLOT_ID, slot_id, &pcie_obj_handle);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR,
        "%s: get object handle of class PcieCard fail(%d), slot_id is %d.", __func__, ret, slot_id));

    
    pcie_obj_name = dfl_get_object_name(pcie_obj_handle);
    return_do_info_if_fail(NULL != pcie_obj_name, debug_log(DLOG_ERROR, "%s: get object name fail.", __func__));

    
    ret = dfl_get_object_list(CLASS_EXTEND_NETCARD_NAME, &card_list);
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "%s: Get extendcard handle list failed! Ret = %d.\n", __func__, ret));

    card_list = g_slist_sort(card_list, (GCompareFunc)sort_extendnetcard_handle_by_slot);

    for (list_item = card_list; list_item; list_item = g_slist_next(list_item)) {
        object_handle = (OBJ_HANDLE)list_item->data;

        
        ret = dal_get_property_value_string(object_handle, PROPETRY_EXTEND_PCIECARD_BASECARD, basecard_obj_name,
            MAX_OBJECT_NAME_LEN);
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s: Get extendcard basecard fail!, ret = %d.\n", __func__, ret));

        
        do_info_if_true(0 == g_strcmp0(pcie_obj_name, basecard_obj_name),
            get_sdicard_extend_netcard_info(object_handle, property_jso));
    }

    g_slist_free(card_list);

    return;
}


LOCAL void get_sdicard_extend_netcard_info(OBJ_HANDLE extend_netcard_obj_handle, json_object *property_jso)
{
    guint8 slot_id = 0;
    json_object *extend_card_jso = NULL;
    OBJ_HANDLE extend_pciecard_obj_handle = 0;

    
    return_do_info_if_fail(NULL != property_jso, debug_log(DLOG_ERROR, "%s: input param error.", __func__));

    
    gint32 ret = dal_get_property_value_byte(extend_netcard_obj_handle, CLASS_EXTEND_NETCARD_SLOT, &slot_id);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s: get prop SlotId fail(%d).", __func__, ret));

    
    ret = dal_get_specific_object_byte(CLASS_EXTEND_PCIECARD_NAME, PROPERTY_PCIE_CARD_SLOT_ID, slot_id,
        &extend_pciecard_obj_handle);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR,
        "%s: get object handle of class ExtendPcieCard fail(%d), slot_id is %d.", __func__, ret, slot_id));

    extend_card_jso = json_object_new_object();
    return_do_info_if_fail(NULL != extend_card_jso,
        debug_log(DLOG_ERROR, "%s: json_object_new_object fail.", __func__));

    
    (void)rf_add_property_jso_string(extend_pciecard_obj_handle, PROPERTY_PCIE_CARD_SLAVE_CARD_NAME,
        RFPROP_PCIEDEVICES_PRODUCT_NAME, extend_card_jso);

    
    (void)rf_add_property_jso_uint16_hex(extend_pciecard_obj_handle, PROPERTY_PCIE_CARD_BOARD_ID,
        RFPROP_PCIEDEVICES_BOARDID, extend_card_jso);

    
    (void)rf_add_property_jso_string(extend_pciecard_obj_handle, PROPERTY_CARD_BOARDNAME, RFPROP_PCIEDEVICES_BOARD_NAME,
        extend_card_jso);

    
    (void)rf_add_property_jso_string(extend_pciecard_obj_handle, PROPERTY_PCIE_CARD_PCB_VER,
        RFPROP_PCIEDEVICES_PCB_VERSION, extend_card_jso);

    
    (void)rf_add_property_jso_uint16_hex(extend_pciecard_obj_handle, PROPERTY_PCIE_CARD_DID,
        RFPROP_PCIEFUNCTION_DEVICEID, extend_card_jso);

    
    (void)rf_add_property_jso_uint16_hex(extend_pciecard_obj_handle, PROPERTY_PCIE_CARD_VID,
        RFPROP_PCIEFUNCTION_VENDORID, extend_card_jso);

    
    (void)rf_add_property_jso_uint16_hex(extend_pciecard_obj_handle, PROPERTY_PCIE_CARD_SDID,
        RFPROP_PCIEFUNCTION_SUBSYSTEMID, extend_card_jso);

    
    (void)json_object_object_add(extend_card_jso, PROPERTY_SDI_CARD_SLOT, json_object_new_int(slot_id));

    
    (void)rf_add_property_jso_uint16_hex(extend_pciecard_obj_handle, PROPERTY_PCIE_CARD_SVID,
        RFPROP_PCIEFUNCTION_SUBSYSTEMVENDORID, extend_card_jso);

    
    (void)rf_add_property_jso_string(extend_netcard_obj_handle, BUSY_NETCARD_MANU,
        RFPROP_PCIEDEVICES_EXTENDCARD_MANUFACTURER, extend_card_jso);

    
    (void)rf_add_property_jso_string(extend_netcard_obj_handle, BUSY_NETCARD_CHIP_MANU,
        RFPROP_PCIEDEVICES_EXTENDCARD_CHIPMANUFACTURER, extend_card_jso);

    
    (void)rf_add_property_jso_string(extend_netcard_obj_handle, PROPERTY_NETCARD_DESC,
        RFPROP_PCIEDEVICES_EXTENDCARD_DESCRIPTION, extend_card_jso);

    
    (void)rf_add_property_jso_string(extend_netcard_obj_handle, BUSY_NETCARD_MODEL, RFPROP_PCIEDEVICES_EXTENDCARD_MODEL,
        extend_card_jso);

    
    (void)get_eth_pf_info(extend_pciecard_obj_handle, extend_card_jso);
    

    
    (void)json_object_array_add(property_jso, extend_card_jso);

    return;
}


LOCAL gint32 sort_extendnetcard_handle_by_slot(gconstpointer a, gconstpointer b)
{
    OBJ_HANDLE handle_a;
    OBJ_HANDLE handle_b;
    guchar slot_a = 0;
    guchar slot_b = 0;

    if (NULL == a) {
        return (a != b) ? -1 : 0;
    } else if (NULL == b) {
        return a != b;
    } else {
        handle_a = (OBJ_HANDLE)a;
        handle_b = (OBJ_HANDLE)b;
    }

    (void)dal_get_property_value_byte(handle_a, CLASS_EXTEND_NETCARD_SLOT, &slot_a);
    (void)dal_get_property_value_byte(handle_b, CLASS_EXTEND_NETCARD_SLOT, &slot_b);

    return slot_a - slot_b;
}


LOCAL void get_sddcard_extend_info(OBJ_HANDLE obj_handle, json_object *huawei_jso)
{
    
    return_do_info_if_fail(NULL != huawei_jso, debug_log(DLOG_ERROR, "%s: input param error.", __func__));

    
    (void)rf_add_property_jso_string(obj_handle, PROPERTY_PCIESSDCARD_NAME, RFPROP_PCIEDEVICES_PRODUCT_NAME,
        huawei_jso);

    
    (void)rf_add_property_jso_byte(obj_handle, PROPERTY_HDD_REMNANT_MEDIA_WEAROUT,
        RFPROP_PCIEDEVICES_REMAINING_WEAR_RATE_PERCENT, huawei_jso);

    return;
}


LOCAL void get_acceleratecard_extend_info(OBJ_HANDLE obj_handle, json_object *huawei_jso)
{
    
    return_do_info_if_fail(NULL != huawei_jso, debug_log(DLOG_ERROR, "%s: input param error.", __func__));

    
    (void)rf_add_property_jso_string(obj_handle, PROPERTY_CARD_NAME, RFPROP_PCIEDEVICES_PRODUCT_NAME, huawei_jso);

    
    (void)rf_add_property_jso_uint16_hex(obj_handle, PROPERTY_CARD_BOARDID, RFPROP_PCIEDEVICES_BOARDID, huawei_jso);

    
    (void)rf_add_property_jso_string(obj_handle, PROPERTY_CARD_PCBVER, RFPROP_PCIEDEVICES_PCB_VERSION, huawei_jso);

    
    (void)get_acceleratecard_extend_bomid(obj_handle, huawei_jso);

    return;
}


LOCAL void get_acceleratecard_extend_bomid(OBJ_HANDLE obj_handle, json_object *huawei_jso)
{
    guchar bomid = 0;
    gchar bomid_hex[MAX_STRBUF_LEN + 1] = {0};
    json_object *property_jso = NULL;

    
    return_do_info_if_fail(NULL != huawei_jso, debug_log(DLOG_ERROR, "%s: input param error.", __func__));

    
    gint32 ret = dal_get_property_value_byte(obj_handle, PROPERTY_ACCELERATE_CARD_BOMID, &bomid);
    return_do_info_if_fail(VOS_OK == ret, (void)json_object_object_add(huawei_jso, RFPROP_PCIEDEVICES_BOMID, NULL);
        debug_log(DLOG_ERROR, "%s: get prop BomId fail(%d).", __func__, ret));
    return_do_info_if_fail(0 != bomid && INVALID_DATA_BYTE != bomid,
        (void)json_object_object_add(huawei_jso, RFPROP_PCIEDEVICES_BOMID, NULL);
        debug_log(DLOG_DEBUG, "%s: invalid value %d for prop BomId.", __func__, bomid));

    
    (void)snprintf_s(bomid_hex, sizeof(bomid_hex), sizeof(bomid_hex) - 1, "0x%04x", bomid);

    property_jso = json_object_new_string(bomid_hex);
    do_if_fail(NULL != property_jso, debug_log(DLOG_ERROR, "%s: json_object_new_string fail.", __func__));

    (void)json_object_object_add(huawei_jso, RFPROP_PCIEDEVICES_BOMID, property_jso);

    return;
}


LOCAL void get_m2transformcard_extend_info(OBJ_HANDLE obj_handle, json_object *huawei_jso)
{
    
    return_do_info_if_fail(NULL != huawei_jso, debug_log(DLOG_ERROR, "%s: input param error.", __func__));

    
    (void)rf_add_property_jso_string(obj_handle, PROPERTY_CARD_NAME, RFPROP_PCIEDEVICES_PRODUCT_NAME, huawei_jso);

    
    (void)rf_add_property_jso_uint16_hex(obj_handle, PROPERTY_CARD_BOARDID, RFPROP_PCIEDEVICES_BOARDID, huawei_jso);

    
    (void)rf_add_property_jso_string(obj_handle, PROPERTY_CARD_PCBVER, RFPROP_PCIEDEVICES_PCB_VERSION, huawei_jso);

    
    (void)rf_add_property_jso_presence(obj_handle, PROPERTY_M2TRANSFORMCARD_SSD1PRESENCE,
        RFPROP_PCIEDEVICES_SSD1_PRESENCE, huawei_jso);

    
    (void)rf_add_property_jso_presence(obj_handle, PROPERTY_M2TRANSFORMCARD_SSD2PRESENCE,
        RFPROP_PCIEDEVICES_SSD2_PRESENCE, huawei_jso);

    return;
}


LOCAL gint32 get_pciedevice_collection_odata_id(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
                                                                 json_object** o_result_jso)
{
    gint32 ret = get_odata_id(i_paras, o_message_jso, o_result_jso, URI_FORMAT_CHASSIS_PCIEDEVICE_COLLECTION);

    return ret;
}


LOCAL gint32 get_pciedevice_collection_members_count(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
                                                                        json_object** o_result_jso)
{
    size_t count;
    json_object* jso_pciedevices = NULL;

    if (provider_paras_check(i_paras) != VOS_OK || o_message_jso == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    jso_pciedevices = json_object_new_array();
    return_val_do_info_if_expr(jso_pciedevices == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: alloc new json array failed", __FUNCTION__));

    redfish_get_chassis_pciedevice_odata_id(jso_pciedevices, CLASS_PCIECARD_NAME);
    if (dal_is_customized_by_cucc() && check_npu_type_match_dboard()) {
        redfish_get_chassis_pciedevice_odata_id(jso_pciedevices, CLASS_NPU);
    }
    redfish_get_chassis_pciedevice_odata_id(jso_pciedevices, CLASS_OCP_CARD);
    count = json_object_array_length(jso_pciedevices);
    (void)json_object_put(jso_pciedevices);

    *o_result_jso = json_object_new_int(count);
    return_val_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);
    return HTTP_OK;
}


LOCAL gint32 get_pciedevice_collection_members(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
                                                               json_object** o_result_jso)
{
    if (provider_paras_check(i_paras) != VOS_OK || o_message_jso == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_array();
    return_val_do_info_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: alloc new json array failed", __FUNCTION__));

    redfish_get_chassis_pciedevice_odata_id(*o_result_jso, CLASS_PCIECARD_NAME);
    if (dal_is_customized_by_cucc() && check_npu_type_match_dboard()) {
        redfish_get_chassis_pciedevice_odata_id(*o_result_jso, CLASS_NPU);
    }
    redfish_get_chassis_pciedevice_odata_id(*o_result_jso, CLASS_OCP_CARD);

    return HTTP_OK;
}

LOCAL PROPERTY_PROVIDER_S g_chassis_pciedevice_collection_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_pciedevice_collection_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS_COUNT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_pciedevice_collection_members_count, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_pciedevice_collection_members, NULL, NULL, ETAG_FLAG_ENABLE}
};


gint32 chassis_pciedevices_provider_entry(PROVIDER_PARAS_S* i_paras, PROPERTY_PROVIDER_S** prop_provider,
                                                           guint32* count)
{
    gint32 ret;
    gboolean ret_result;
    gboolean issupport = TRUE;

    return_val_do_info_if_expr((i_paras == NULL) || (prop_provider == NULL) || (count == NULL),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    
    ret = dal_check_support_pciedevices(&issupport);
    if (ret != RET_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if (issupport == FALSE) {
        return HTTP_NOT_FOUND;
    }

    ret_result = redfish_check_chassis_uri_valid(i_paras->uri, &i_paras->obj_handle);
    if (ret_result == FALSE) {
        return HTTP_NOT_FOUND;
    }

    
    return_val_if_fail(check_enclosure_component_type(i_paras->obj_handle, FALSE) == VOS_OK, HTTP_NOT_FOUND);

    *prop_provider = g_chassis_pciedevice_collection_provider;
    *count = G_N_ELEMENTS(g_chassis_pciedevice_collection_provider);
    return VOS_OK;
}
