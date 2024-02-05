
#include "redfish_provider.h"
#include "redfish_provider_system.h"

#define MAX_MEMORY_PROP_LEN 128

LOCAL gint32 get_system_memory_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_memory_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_system_memory_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_system_memory_capacity(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_memory_manufacturer(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_memory_clockspeed(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_memory_configclockspeed(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_memory_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_memory_sn(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);


LOCAL gint32 get_system_memory_devicetype(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_memory_datawidth(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_memory_rankcount(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_memory_devicelocator(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_memory_memorylocation(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);


LOCAL gint32 redfish_check_system_memory_uri_effective(const gchar *i_member_id, const gchar *i_uri,
    OBJ_HANDLE *o_obj_handle);

LOCAL void get_memory_location_name(OBJ_HANDLE obj_handle, gchar *name, guint32 len);
LOCAL gint32 get_system_memory_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 get_system_memory_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_system_memory_partnumber(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_memory_basemoduletype(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_memory_fw_version(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_memory_fw_volatile_size_mb(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_memory_fw_persistent_size_mb(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_memory_manufacturer_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_memory_manufacturing_location(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_memory_manufacturing_date(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_memory_metrics(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

gint32 system_memory_etag_del_property(json_object *object)
{
    SPECIAL_PROP_S value[] = {
        {3, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RFPROP_MEMORY_MEDIUM_TEMPERATURE, NULL, NULL}},
        {3, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RFPROP_MEMORY_CONTROL_TEMPERATURE, NULL, NULL}},
        {3, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RFPROP_MEMORY_ECC_COUNT, NULL, NULL}},
        {0, {NULL, NULL, NULL, NULL, NULL}}
    };
    return rsc_del_none_etag_affected_property(object, value, G_N_ELEMENTS(value));
}

LOCAL PROPERTY_PROVIDER_S g_system_memory_provider[] = {
    {RFPROP_MEMORY_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_memory_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMORY_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_memory_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMORY_NAME, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_memory_name, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMORY_CAPMIB, CLASS_MEMORY, PROPERTY_MEM_CAPACITY, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_memory_capacity, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMORY_MANFAC, CLASS_MEMORY, PROPERTY_MEM_MANUFACTURE, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_memory_manufacturer, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMORY_SPEEDMHZ, CLASS_MEMORY, PROPERTY_MEM_CONFIG_CLOCK_SPEED, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_memory_configclockspeed, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMORY_ALLOWSPEEDMHZ, CLASS_MEMORY, PROPERTY_MEM_CLOCK_SPEED, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_memory_clockspeed, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMORY_SN, CLASS_MEMORY, PROPERTY_MEM_SN, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_memory_sn, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_PART_NUMBER, CLASS_MEMORY, PROPERTY_MEM_PART_NUM, USERROLE_READONLY, SYS_LOCKDOWN_NULL,  get_system_memory_partnumber, NULL, NULL, ETAG_FLAG_ENABLE},
    
    {RFPROP_MEMORY_DEVICETYPE, CLASS_MEMORY, PROPERTY_MEM_TYPE, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_memory_devicetype, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMORY_DATAWIDTH, CLASS_MEMORY, PROPERTY_MEM_BIT_WIDTH, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_memory_datawidth, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMORY_RANKCOUNT, CLASS_MEMORY, PROPERTY_MEM_RANK, USERROLE_READONLY, SYS_LOCKDOWN_NULL,  get_system_memory_rankcount, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMORY_DEVICELOCATOR, CLASS_MEMORY, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_memory_devicelocator, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMORY_MEMORYLOCATION, CLASS_MEMORY, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_memory_memorylocation, NULL, NULL, ETAG_FLAG_ENABLE},
    
    {RFPROP_MEMORY_STATUS, CLASS_MEMORY, PROPERTY_MEM_HEALTH, USERROLE_READONLY, SYS_LOCKDOWN_NULL,  get_system_memory_status, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_memory_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_OEM, CLASS_MEMORY, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_memory_oem, NULL, NULL, ETAG_FLAG_ENABLE},
    
    {PROPERTY_MEM_BASEMODULE, CLASS_MEMORY, PROPERTY_MEM_BASEMODULE, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_memory_basemoduletype, NULL, NULL, ETAG_FLAG_ENABLE},
    
    
    {RFPROP_MEMORY_FIRMWARE_VERSION, CLASS_MEMORY, PROPERTY_MEM_FWVERSION, USERROLE_READONLY, SYS_LOCKDOWN_NULL,  get_system_memory_fw_version, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMORY_VOLATILE_CAPACITY, CLASS_MEMORY, PROPERTY_MEM_VOLATILECAPACITY, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_memory_fw_volatile_size_mb, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMORY_PERSISTENT_CAPACITY, CLASS_MEMORY, PROPERTY_MEM_PERSISTENTCAPACITY, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_memory_fw_persistent_size_mb, NULL, NULL, ETAG_FLAG_ENABLE},
    
    {RFPROP_MEMORY_MANUFACTURER_ID, CLASS_MEMORY, PROPERTY_MEM_MANUFACTURER_ID, USERROLE_READONLY,
        SYS_LOCKDOWN_NULL, get_system_memory_manufacturer_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMORY_MANUFACTURING_LOCATION, CLASS_MEMORY, PROPERTY_MEM_MANUFACTURING_LOCATION, USERROLE_READONLY,
        SYS_LOCKDOWN_NULL, get_system_memory_manufacturing_location, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMORY_MANUFACTURING_DATE, CLASS_MEMORY, PROPERTY_MEM_MANUFACTURING_DATE, USERROLE_READONLY,
        SYS_LOCKDOWN_NULL, get_system_memory_manufacturing_date, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMORY_METRICS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY,
        SYS_LOCKDOWN_NULL, get_system_memory_metrics, NULL, NULL, ETAG_FLAG_ENABLE}
};

LOCAL gint32 get_system_memory_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;

    gint32 ret;
    gchar odata_context[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__));

    iRet =
        snprintf_s(odata_context, sizeof(odata_context), sizeof(odata_context) - 1, SYSTEMS_MEMORY_METADATA, slot_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    *o_result_jso = json_object_new_string(odata_context);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}

gint32 transform_capacity_to_intmb(gchar *memory_capacity, guint32 len, guint32 *num)
{
    gint32 ret = -1;

    if (len < 2 || NULL == memory_capacity) {
        return VOS_ERR;
    }

    if (*(memory_capacity + len - 2) == 'G' || *(memory_capacity + len - 2) == 'g') {
        ret = redfish_strip_to_int(memory_capacity, len);
        if (ret != VOS_OK) {
            return VOS_ERR;
        }

        ret = vos_str2int(memory_capacity, 10, num, NUM_TPYE_UINT32);
        if (ret != VOS_OK) {
            return VOS_ERR;
        }

        *num = (*num) * 1024;
    } else if (*(memory_capacity + len - 2) == 'M' || *(memory_capacity + len - 2) == 'm') {
        ret = redfish_strip_to_int(memory_capacity, len);
        if (ret != VOS_OK) {
            return VOS_ERR;
        }

        ret = vos_str2int(memory_capacity, 10, num, NUM_TPYE_UINT32);
        if (ret != VOS_OK) {
            return VOS_ERR;
        }
    }

    return VOS_OK;
}


LOCAL void get_memory_location_name(OBJ_HANDLE obj_handle, gchar *name, guint32 name_len)
{
    int iRet;
    gchar device_name[MEMORY_DEVICE_LEN + 1] = {0};
    gchar memory_location[MEMORY_LOCATION_LEN + 1] = {0};

    (void)dal_get_property_value_string(obj_handle, PROPERTY_MEM_DEVICENAME, device_name, sizeof(device_name));
    (void)dal_clear_string_blank(device_name, sizeof(device_name));

    (void)dal_get_property_value_string(obj_handle, PROPERTY_MEM_LOCATION, memory_location, sizeof(memory_location));
    (void)dal_clear_string_blank(memory_location, sizeof(memory_location));

    iRet = snprintf_s(name, name_len, name_len - 1, "%s%s", memory_location, device_name);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    return;
}

gchar *rfmemory_type_to_rftype(gchar *type, gint32 length)
{
    int i;

    static gchar* type_list[13][2] = {{"DDR", "DDR"}, {"DDR2", "DDR2"}, {"DDR3", "DDR3"}, {"DDR4", "DDR4"}, {"SDRAM", "SDRAM"},
                                      {"LPDDR3", "LPDDR3_SDRAM"}, {"LPDDR4", "LPDDR4"}, {"DDR2 FB-DIMM", "DDR2_SDRAM_FB_DIMM"}, {"FBD2", "DDR2_SDRAM_FB_DIMM_PROBE"},
                                      {"ROM", "ROM"}, {"EEPROM", "ROM"}, {"FEPROM", "ROM"}, {"EPROM", "ROM"}
                                     };

    if (NULL == type) {
        return NULL;
    }

    for (i = 0; i < 13; i++) {
        if (g_strcmp0(type, type_list[i][0]) == 0) {
            return type_list[i][1];
        }
    }

    return NULL;
}



LOCAL gint32 rfmemory_get_bytevalue_by_prop(PROVIDER_PARAS_S *i_paras, gchar *prop, gboolean check_zero,
    json_object **o_message_jso, json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    guchar value = 0;

    
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras) || NULL == prop) {
        
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_check_system_memory_uri_effective(i_paras->member_id, i_paras->uri, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));

    ret = dal_get_property_value_byte(obj_handle, prop, &value);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));

    if (TRUE == check_zero && 0 == value) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_int(value);
    return_val_do_info_if_fail(*o_result_jso != NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));

    return HTTP_OK;
}


LOCAL gint32 redfish_check_system_memory_uri_effective(const gchar *i_member_id, const gchar *i_uri,
    OBJ_HANDLE *o_obj_handle)
{
    int iRet = -1;
    gint32 ret;
    gint32 match_flag = 1;
    gchar device_name[MEMORY_DEVICE_LEN + 1] = {0};
    gchar memory_location[MEMORY_LOCATION_LEN + 1] = {0};
    gchar memory_name[MEMORY_NAME_LEN + 1] = {0};
    guchar memory_onset = 0;

    GSList *obj_list = NULL;
    GSList *obj_node = NULL;

    
    if (NULL == i_member_id || NULL == i_uri) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = dfl_get_object_list(CLASS_MEMORY, &obj_list);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get obj_list fail.\n", __FUNCTION__, __LINE__));

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        (void)memset_s(device_name, sizeof(device_name), 0, sizeof(device_name));
        (void)dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_MEM_DEVICENAME, device_name,
            sizeof(device_name));
        do_if_expr(0 == strlen(device_name), continue);
        (void)dal_clear_string_blank(device_name, sizeof(device_name)); 

        (void)memset_s(memory_location, sizeof(memory_location), 0, sizeof(memory_location));
        (void)dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_MEM_LOCATION, memory_location,
            sizeof(memory_location));
        (void)dal_clear_string_blank(memory_location, sizeof(memory_location));

        (void)memset_s(memory_name, sizeof(memory_name), 0, sizeof(memory_name));
        iRet =
            snprintf_s(memory_name, sizeof(memory_name), sizeof(memory_name) - 1, "%s%s", memory_location, device_name);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
        match_flag = g_ascii_strcasecmp(i_member_id, memory_name);

        break_if_fail(RF_MATCH_OK != match_flag);
    }

    
    
    if (NULL != obj_node) {
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_MEM_PRESENCE, &memory_onset);
    }

    
    if (RF_MATCH_OK != match_flag || 1 != memory_onset || 0 == strlen(memory_name)) {
        g_slist_free(obj_list);
        return HTTP_NOT_FOUND;
    }

    do_info_if_true((NULL != o_obj_handle && NULL != obj_node), (*o_obj_handle = (OBJ_HANDLE)obj_node->data));
    g_slist_free(obj_list);
    return VOS_OK;
}


LOCAL gint32 get_system_memory_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;
    gint32 ret;
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    gchar memory_name[MEMORY_NAME_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_check_system_memory_uri_effective(i_paras->member_id, i_paras->uri, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));
    get_memory_location_name(obj_handle, memory_name, MEMORY_NAME_LEN + 1);

    if (0 == strlen(memory_name)) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = redfish_get_board_slot(slot, sizeof(slot));
    if (VOS_ERR == ret) {
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    iRet = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, URI_FORMAT_SYSTEM_REMEMORYS, slot, memory_name);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    

    *o_result_jso = json_object_new_string((const char *)uri);

    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 get_system_memory_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gchar memory_id[MEMORY_NAME_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_check_system_memory_uri_effective(i_paras->member_id, i_paras->uri, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));

    
    get_memory_location_name(obj_handle, memory_id, MEMORY_NAME_LEN + 1);

    if (0 == strlen(memory_id)) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string(memory_id);
    return HTTP_OK;
}


LOCAL gint32 get_system_memory_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gchar memory_name[MEMORY_NAME_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_check_system_memory_uri_effective(i_paras->member_id, i_paras->uri, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));

    
    get_memory_location_name(obj_handle, memory_name, MEMORY_NAME_LEN + 1);

    if (0 == strlen(memory_name)) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string(memory_name);
    return HTTP_OK;
}


LOCAL gint32 get_system_memory_capacity(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar memory_capacity[MEMORY_CAP_LEN + 1] = {0};
    guint32 num = 0;
    OBJ_HANDLE obj_handle = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_check_system_memory_uri_effective(i_paras->member_id, i_paras->uri, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));

    
    (void)dal_get_property_value_string(obj_handle, g_system_memory_provider[i_paras->index].pme_prop_name,
        memory_capacity, sizeof(memory_capacity));

    if (!g_strcmp0(memory_capacity, MEMORY_UNKNOWN) || !g_strcmp0(memory_capacity, MEMORY_NA) ||
        0 == strlen(memory_capacity)) {
        return HTTP_INTERNAL_SERVER_ERROR;
    } else {
        ret = transform_capacity_to_intmb(memory_capacity, strlen(memory_capacity), &num);
        if (ret != VOS_OK) {
            return HTTP_INTERNAL_SERVER_ERROR;
        }

        *o_result_jso = json_object_new_int(num);
    }

    return HTTP_OK;
}


LOCAL gint32 get_system_memory_partnumber(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar  part_number[MAX_MEMORY_PROP_LEN] = {0};
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_check_system_memory_uri_effective(i_paras->member_id, i_paras->uri, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: redfish_check_system_memory_uri_effective fail.\n", __FUNCTION__, __LINE__));

    
    ret = dal_get_property_value_string(obj_handle, PROPERTY_MEM_PART_NUM, part_number, MAX_MEMORY_PROP_LEN);

    
    return_val_do_info_if_fail(VOS_OK == ret && VOS_OK == check_string_val_effective(part_number),
        HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get dal_get_property_value_string fail.\n", __FUNCTION__, __LINE__));
    

    *o_result_jso = json_object_new_string(part_number);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:json_object_new_string fail.\n", __FUNCTION__, __LINE__));

    return HTTP_OK;
}


LOCAL gint32 get_system_memory_manufacturer(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar memory_manfac[MEMORY_MANFAC_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_check_system_memory_uri_effective(i_paras->member_id, i_paras->uri, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));

    
    (void)dal_get_property_value_string(obj_handle, g_system_memory_provider[i_paras->index].pme_prop_name,
        memory_manfac, sizeof(memory_manfac));

    if (0 == strlen(memory_manfac)) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string(memory_manfac);

    return HTTP_OK;
}

LOCAL gint32 get_system_memory_clockspeed(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar memory_clockspeed[MEMORY_SPEED_LEN + 1] = {0};
    guint32 num = 0;
    OBJ_HANDLE obj_handle = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_check_system_memory_uri_effective(i_paras->member_id, i_paras->uri, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));

    
    (void)dal_get_property_value_string(obj_handle, g_system_memory_provider[i_paras->index].pme_prop_name,
        memory_clockspeed, sizeof(memory_clockspeed));

    
    if (!g_strcmp0(memory_clockspeed, MEMORY_UNKNOWN) || !g_strcmp0(memory_clockspeed, MEMORY_UNKNOWN_2) ||
        !g_strcmp0(memory_clockspeed, MEMORY_NA) || 0 == strlen(memory_clockspeed)) {
        return HTTP_INTERNAL_SERVER_ERROR;
    } else {
        ret = redfish_strip_to_int(memory_clockspeed, sizeof(memory_clockspeed));
        if (VOS_OK != ret) {
            debug_log(DLOG_ERROR, "%s, %d: redfish_strip_to_int error", __FUNCTION__, __LINE__);
            return HTTP_INTERNAL_SERVER_ERROR;
        }

        ret = vos_str2int(memory_clockspeed, 10, &num, NUM_TPYE_UINT32);
        if (ret != VOS_OK) {
            debug_log(DLOG_ERROR, "%s, %d: vos_str2int error", __FUNCTION__, __LINE__);
            return HTTP_INTERNAL_SERVER_ERROR;
        }

        *o_result_jso = json_object_new_array();
        json_object_array_add(*o_result_jso, json_object_new_int(num));
    }

    return HTTP_OK;
}


LOCAL gint32 get_system_memory_configclockspeed(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar memory_configclockspeed[MEMORY_SPEED_LEN + 1] = {0};
    guint32 num = 0;
    OBJ_HANDLE obj_handle = 0;
    gboolean is_customized;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_check_system_memory_uri_effective(i_paras->member_id, i_paras->uri, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));

    
    is_customized = dal_is_customized_by_cmcc();
    if (is_customized) {
        ret = dal_get_property_value_string(obj_handle, PROPERTY_MEM_CLOCK_SPEED, memory_configclockspeed,
            sizeof(memory_configclockspeed));
    } else {
        ret = dal_get_property_value_string(obj_handle, g_system_memory_provider[i_paras->index].pme_prop_name, 
            memory_configclockspeed, sizeof(memory_configclockspeed));
    }
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get memory config clockspeed failed, ret(%d)", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (!g_strcmp0(memory_configclockspeed, MEMORY_UNKNOWN) || !g_strcmp0(memory_configclockspeed, MEMORY_UNKNOWN_2) ||
        !g_strcmp0(memory_configclockspeed, MEMORY_NA) || 0 == strlen(memory_configclockspeed)) {
        return HTTP_INTERNAL_SERVER_ERROR;
    } else {
        ret = redfish_strip_to_int(memory_configclockspeed, sizeof(memory_configclockspeed));
        if (VOS_OK != ret) {
            debug_log(DLOG_ERROR, "%s, %d: redfish_strip_to_int error", __FUNCTION__, __LINE__);
            return HTTP_INTERNAL_SERVER_ERROR;
        }

        ret = vos_str2int(memory_configclockspeed, 10, &num, NUM_TPYE_UINT32);
        if (ret != VOS_OK) {
            debug_log(DLOG_ERROR, "%s, %d: vos_str2int error", __FUNCTION__, __LINE__);
            return HTTP_INTERNAL_SERVER_ERROR;
        }

        *o_result_jso = json_object_new_int(num);
    }

    return HTTP_OK;
}


LOCAL gint32 get_system_memory_sn(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gchar memory_sn[MEMORY_SN_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_check_system_memory_uri_effective(i_paras->member_id, i_paras->uri, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));

    
    (void)dal_get_property_value_string(obj_handle, g_system_memory_provider[i_paras->index].pme_prop_name, memory_sn,
        sizeof(memory_sn));

    
    if (!g_strcmp0(memory_sn, MEMORY_NO_DIMM) || !g_strcmp0(memory_sn, MEMORY_IS_NULL) || 0 == strlen(memory_sn)) {
        return HTTP_INTERNAL_SERVER_ERROR;
    } else {
        *o_result_jso = json_object_new_string(memory_sn);
    }

    return HTTP_OK;
}



LOCAL gint32 get_system_memory_devicetype(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    gchar  devicetype[MAX_MEMORY_PROP_LEN] = {0};
    gchar *rf_type = NULL;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_check_system_memory_uri_effective(i_paras->member_id, i_paras->uri, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));

    ret = dal_get_property_value_string(obj_handle, g_system_memory_provider[i_paras->index].pme_prop_name, devicetype,
        sizeof(devicetype));
    if (ret != VOS_OK || g_strcmp0(devicetype, "N/A") == 0 || g_strcmp0(devicetype, "null") == 0 ||
        0 == strlen(devicetype)) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    
    rf_type = rfmemory_type_to_rftype(devicetype, sizeof(devicetype));
    return_val_do_info_if_fail(NULL != rf_type, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));

    
    *o_result_jso = json_object_new_string(rf_type);
    

    return_val_do_info_if_fail(*o_result_jso != NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));

    return HTTP_OK;
}

LOCAL gint32 get_system_memory_datawidth(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    guint16 datawidth = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_check_system_memory_uri_effective(i_paras->member_id, i_paras->uri, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));

    ret = dal_get_property_value_uint16(obj_handle, g_system_memory_provider[i_paras->index].pme_prop_name, &datawidth);
    if (ret != VOS_OK || datawidth == 0) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_int(datawidth);
    return_val_do_info_if_fail(*o_result_jso != NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));

    return HTTP_OK;
}

LOCAL gint32 get_system_memory_rankcount(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return rfmemory_get_bytevalue_by_prop(i_paras, g_system_memory_provider[i_paras->index].pme_prop_name, TRUE,
        o_message_jso, o_result_jso);
}


LOCAL gint32 get_system_memory_devicelocator(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar devicelocator[MAX_MEMORY_PROP_LEN] = {0};
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_check_system_memory_uri_effective(i_paras->member_id, i_paras->uri, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));

    
    ret = dal_get_property_value_string(obj_handle, PROPERTY_MEM_DEVICENAME, devicelocator, sizeof(devicelocator));
    if (VOS_OK != ret || 0 == g_strcmp0(devicelocator, "N/A") || 0 == g_strcmp0(devicelocator, "null") ||
        0 == strlen(devicelocator)) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string(devicelocator);
    return_val_do_info_if_fail(*o_result_jso != NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));

    return HTTP_OK;
}

LOCAL gint32 get_system_memory_memorylocation(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    json_object *result_json = NULL;

    *o_result_jso = json_object_new_object();
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    ret = rfmemory_get_bytevalue_by_prop(i_paras, PROPERTY_MEM_CPU_ID, FALSE, o_message_jso, &result_json);
    
    json_object_object_add(*o_result_jso, RFPROP_MEMORY_SOCKET, ret == HTTP_OK ? result_json : NULL);

    ret = rfmemory_get_bytevalue_by_prop(i_paras, PROPERTY_MEM_NODE_ID, FALSE, o_message_jso, &result_json);
    json_object_object_add(*o_result_jso, RFPROP_MEMORY_CONTROLLER, ret == HTTP_OK ? result_json : NULL);

    ret = rfmemory_get_bytevalue_by_prop(i_paras, PROPERTY_MEM_CHANNEL_ID, FALSE, o_message_jso, &result_json);
    json_object_object_add(*o_result_jso, RFPROP_MEMORY_CHANNEL, ret == HTTP_OK ? result_json : NULL);

    ret = rfmemory_get_bytevalue_by_prop(i_paras, PROPERTY_MEM_DIMM_ID, FALSE, o_message_jso, &result_json);
    json_object_object_add(*o_result_jso, RFPROP_MEMORY_SLOT, ret == HTTP_OK ? result_json : NULL);

    return HTTP_OK;
}


LOCAL gint32 get_system_memory_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    guchar health = 4; 
    guchar dcpmm_health = 4;
    guchar power_state = 0;
    

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_check_system_memory_uri_effective(i_paras->member_id, i_paras->uri, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, ret, debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));

    
    (void)dal_get_property_value_byte(obj_handle, g_system_memory_provider[i_paras->index].pme_prop_name, &health);

    
    if (TRUE == rfmemory_type_is_dcpmm(obj_handle) && 0 == health) {
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_MEM_HEALTHSTATUS, &dcpmm_health);
        do_val_if_expr(DCPMM_HEALTH_DEFAULT_VALUE == dcpmm_health, (health = INVALID_DATA_BYTE));
    }
    

    ret = rf_get_system_powerstate(&power_state);
    do_info_if_true(ret != VOS_OK,
        debug_log(DLOG_ERROR, "%s, %d: get system power state fail", __FUNCTION__, __LINE__));
    // power_state取值为1表示上电状态
    do_info_if_true(power_state != 1, (health = INVALID_DATA_BYTE));
    
    ret = get_resource_status_property(&health, NULL, "Enabled", o_result_jso, FALSE);
    if (ret != VOS_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}

LOCAL gint32 rfmemory_get_minvoltagemillivolt(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    guint16 minvoltagemillivolt = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = redfish_check_system_memory_uri_effective(i_paras->member_id, i_paras->uri, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, ret, debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));

    ret = dal_get_property_value_uint16(obj_handle, PROPERTY_MEM_MINIMUM_VOLTAGE, &minvoltagemillivolt);
    if (ret != VOS_OK || minvoltagemillivolt == 0) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_int(minvoltagemillivolt);
    return_val_do_info_if_fail(*o_result_jso != NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));

    return HTTP_OK;
}

LOCAL gint32 rfmemory_get_technology(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    gchar  technology2[MAX_MEMORY_PROP_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = redfish_check_system_memory_uri_effective(i_paras->member_id, i_paras->uri, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, ret, debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));

    ret = dal_get_property_value_string(obj_handle, PROPERTY_MEM_TECHNOLOGY2, technology2, sizeof(technology2));
    if (VOS_OK != ret || 0 == g_strcmp0(technology2, "N/A") || 0 == g_strcmp0(technology2, "null") ||
        0 == strlen(technology2)) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string(technology2);
    return_val_do_info_if_fail(*o_result_jso != NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));

    return HTTP_OK;
}


LOCAL gint32 rfmemory_get_typedetail(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    gchar  type_detail[MAX_MEMORY_PROP_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = redfish_check_system_memory_uri_effective(i_paras->member_id, i_paras->uri, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, ret, debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));

    ret = dal_get_property_value_string(obj_handle, PROPERTY_MEM_TEC, type_detail, sizeof(type_detail));
    if (VOS_OK != ret || 0 == g_strcmp0(type_detail, "N/A") || 0 == g_strcmp0(type_detail, "null") ||
        0 == strlen(type_detail)) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string(type_detail);
    return_val_do_info_if_fail(*o_result_jso != NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));

    return HTTP_OK;
}


LOCAL gint32 rfmemory_get_position(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    gchar  holder[MAX_MEMORY_PROP_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = redfish_check_system_memory_uri_effective(i_paras->member_id, i_paras->uri, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, ret, debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));

    // 容器对应bmc 的Location
    ret = dal_get_property_value_string(obj_handle, PROPERTY_MEM_LOCATION, holder, sizeof(holder));
    if (VOS_OK != ret || 0 == g_strcmp0(holder, "N/A") || 0 == g_strcmp0(holder, "null") || 0 == strlen(holder)) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string(holder);
    return_val_do_info_if_fail(*o_result_jso != NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));

    return HTTP_OK;
}


LOCAL gint32 rfmemory_get_chipbitwidth(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar memory_chipbitwidth[MAX_MEMORY_PROP_LEN] = {0};
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    
    ret = redfish_check_system_memory_uri_effective(i_paras->member_id, i_paras->uri, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:  fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_property_value_string(obj_handle, PROPERTY_MEM_CHIP_BIT_WIDTH, memory_chipbitwidth,
        sizeof(memory_chipbitwidth));

    if (strlen(memory_chipbitwidth) == 0 || !g_ascii_strcasecmp(memory_chipbitwidth, "N/A") ||
        !g_ascii_strcasecmp(memory_chipbitwidth, "null") || !g_ascii_strcasecmp(memory_chipbitwidth, "UnKnown")) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string(memory_chipbitwidth);
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s:  fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return HTTP_OK;
}


gint32 get_system_memory_temperature(OBJ_HANDLE obj_handle, gchar *prop, gchar *rf_prop, json_object *huawei)
{
    gdouble d_value;
    gint32 ret;
    return_val_if_expr(0 == obj_handle || NULL == prop || NULL == rf_prop || NULL == huawei,
        HTTP_INTERNAL_SERVER_ERROR);

    ret = dal_get_property_value_double(obj_handle, prop, &d_value);
    if (VOS_OK == ret && SENSOR_NA_READING != d_value) {
        json_object_object_add(huawei, rf_prop, ex_json_object_new_double(d_value, "%.2f"));
    } else {
        json_object_object_add(huawei, rf_prop, NULL);
    }

    return VOS_OK;
}

LOCAL gint32 get_system_memory_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    json_object *huawei = NULL;
    json_object *result_json = NULL;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_object();
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    huawei = json_object_new_object();
    return_val_if_expr(NULL == huawei, HTTP_INTERNAL_SERVER_ERROR); // 不用释放*o_result_jso
    json_object_object_add(*o_result_jso, RFPROP_COMMON_HUAWEI, huawei);

    ret = rfmemory_get_minvoltagemillivolt(i_paras, o_message_jso, &result_json);
    json_object_object_add(huawei, RFPROP_MEMORY_MINVOLTAGE, ret == HTTP_OK ? result_json : NULL);

    
    ret = rfmemory_get_technology(i_paras, o_message_jso, &result_json);
    json_object_object_add(huawei, RFPROP_MEMORY_TECHNOLOGY, ret == HTTP_OK ? result_json : NULL);

    ret = rfmemory_get_typedetail(i_paras, o_message_jso, &result_json);
    json_object_object_add(huawei, RFPROP_MEMORY_TYPE_DETAIL, ret == HTTP_OK ? result_json : NULL);
    

    ret = rfmemory_get_position(i_paras, o_message_jso, &result_json);
    json_object_object_add(huawei, RFPROP_MEMORY_POSITION, ret == HTTP_OK ? result_json : NULL);

    ret = rfmemory_get_chipbitwidth(i_paras, o_message_jso, &result_json);
    json_object_object_add(huawei, RFPROP_MEMORY_CHIP_BIT_WIDTH, (ret == HTTP_OK) ? result_json : NULL);

    
    rf_add_property_jso_string(i_paras->obj_handle, PROPERTY_MEM_TYPE, PROPERTY_MEM_TYPE, huawei);
    
    
    rf_add_property_jso_byte(i_paras->obj_handle, PROPERTY_MEM_REMAINLIFE, RFPROP_MEMORY_REMAINING_SERVICE_LIFE,
        huawei);
    get_system_memory_temperature(i_paras->obj_handle, PROPERTY_MEM_MEDIATEMP, RFPROP_MEMORY_MEDIUM_TEMPERATURE,
        huawei);
    get_system_memory_temperature(i_paras->obj_handle, PROPERTY_MEM_CONTROLLERTEMP, RFPROP_MEMORY_CONTROL_TEMPERATURE,
        huawei);
    
    rf_add_property_jso_string(i_paras->obj_handle, PROPERTY_MEM_ORIGINAL_PART_NUM, RFPROP_MEMORY_ORIGINAL_PART_NUM,
        huawei);

    rf_add_property_jso_string(i_paras->obj_handle, PROPERTY_MEM_BOM_NUM, RFPROP_MEMORY_BOM_NUM, huawei);
    rf_add_property_jso_uint32(i_paras->obj_handle, PROPERTY_MEM_ECC_COUNT, RFPROP_MEMORY_ECC_COUNT, huawei);
    return HTTP_OK;
}


LOCAL gint32 get_system_memory_basemoduletype(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar memory_basemodule[MEMORY_MANFAC_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_check_system_memory_uri_effective(i_paras->member_id, i_paras->uri, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));

    
    (void)dal_get_property_value_string(obj_handle, g_system_memory_provider[i_paras->index].pme_prop_name,
        memory_basemodule, sizeof(memory_basemodule));

    ret = check_string_val_effective(memory_basemodule);
    if (VOS_OK != ret) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string(memory_basemodule);
    debug_log(DLOG_MASS, "%s, %d: memory_basemoduletype[%s]", __FUNCTION__, __LINE__, memory_basemodule);

    return HTTP_OK;
}


LOCAL gint32 get_system_memory_fw_version(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar memory_version[MEMORY_MANFAC_LEN + 1] = {0};
    gint32 ret;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    (void)dal_get_property_value_string(i_paras->obj_handle, g_system_memory_provider[i_paras->index].pme_prop_name,
        memory_version, sizeof(memory_version));

    ret = check_string_val_effective(memory_version);
    if (VOS_OK != ret) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string(memory_version);

    return HTTP_OK;
}

LOCAL gint32 get_system_memory_fw_volatile_size_mb(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint32 volatile_size = 0;
    guint32 persistent_size = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    (void)dal_get_property_value_uint32(i_paras->obj_handle, g_system_memory_provider[i_paras->index].pme_prop_name,
        &volatile_size);
    (void)dal_get_property_value_uint32(i_paras->obj_handle, PROPERTY_MEM_PERSISTENTCAPACITY, &persistent_size);
    return_val_if_expr(0 == volatile_size && 0 == persistent_size, HTTP_INTERNAL_SERVER_ERROR);

    *o_result_jso = json_object_new_int(volatile_size);

    return HTTP_OK;
}


LOCAL gint32 get_system_memory_fw_persistent_size_mb(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint32 volatile_size = 0;
    guint32 persistent_size = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    (void)dal_get_property_value_uint32(i_paras->obj_handle, g_system_memory_provider[i_paras->index].pme_prop_name,
        &persistent_size);
    (void)dal_get_property_value_uint32(i_paras->obj_handle, PROPERTY_MEM_VOLATILECAPACITY, &volatile_size);

    return_val_if_expr(0 == persistent_size && 0 == volatile_size, HTTP_INTERNAL_SERVER_ERROR);
    *o_result_jso = json_object_new_int(persistent_size);

    return HTTP_OK;
}


LOCAL gint32 get_system_memory_manufacturer_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar memory_module_manufacturer_id[MEMORY_MANUFACTURER_ID_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (!(i_paras->is_satisfy_privi)) {
        debug_log(DLOG_ERROR, "%s: Check privilege fail.", __FUNCTION__);
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    ret = redfish_check_system_memory_uri_effective(i_paras->member_id, i_paras->uri, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get object handle fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    (void)dal_get_property_value_string(obj_handle,
        g_system_memory_provider[i_paras->index].pme_prop_name,
        memory_module_manufacturer_id,
        sizeof(memory_module_manufacturer_id));

    *o_result_jso = json_object_new_string(memory_module_manufacturer_id);

    return HTTP_OK;
}


LOCAL gint32 get_system_memory_manufacturing_location(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar memory_location_code[MEMORY_MANUFACTURING_LOCATION_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: invalid param.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (!(i_paras->is_satisfy_privi)) {
        debug_log(DLOG_ERROR, "%s: Check privilege fail.", __FUNCTION__);
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    ret = redfish_check_system_memory_uri_effective(i_paras->member_id, i_paras->uri, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get object handle fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    (void)dal_get_property_value_string(obj_handle,
        g_system_memory_provider[i_paras->index].pme_prop_name, memory_location_code,
        sizeof(memory_location_code));

    *o_result_jso = json_object_new_string(memory_location_code);

    return HTTP_OK;
}


LOCAL gint32 get_system_memory_manufacturing_date(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar memory_module_manufacturing_date[MEMORY_MANUFACTURING_DATE_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (!(i_paras->is_satisfy_privi)) {
        debug_log(DLOG_ERROR, "%s: Check privilege fail.", __FUNCTION__);
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    ret = redfish_check_system_memory_uri_effective(i_paras->member_id, i_paras->uri, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get object handle fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    (void)dal_get_property_value_string(obj_handle,
        g_system_memory_provider[i_paras->index].pme_prop_name,
        memory_module_manufacturing_date,
        sizeof(memory_module_manufacturing_date));

    *o_result_jso = json_object_new_string(memory_module_manufacturing_date);

    return HTTP_OK;
}


LOCAL gint32 get_system_memory_metrics(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar metrics_uri[MAX_URI_LENGTH] = {0};
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    gchar slot_id[MAX_RSC_ID_LEN] = {0};
    gchar memory_name[MEMORY_NAME_LEN + 1] = {0};
    json_object* odata_id_obj = NULL;

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: invalid param.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (!(i_paras->is_satisfy_privi)) {
        debug_log(DLOG_ERROR, "%s: Check privilege fail.", __FUNCTION__);
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    ret = redfish_check_system_memory_uri_effective(i_paras->member_id, i_paras->uri, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get object handle fail, ret:%d", __FUNCTION__, ret);
        return HTTP_NOT_FOUND;
    }

    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get boad slot id fail, ret:%d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    get_memory_location_name(obj_handle, memory_name, MEMORY_NAME_LEN + 1);
    if (strlen(memory_name) == 0) {
        debug_log(DLOG_ERROR, "%s: memory_name len is 0.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = snprintf_s(metrics_uri, sizeof(metrics_uri), sizeof(metrics_uri) - 1, URI_FORMAT_SYSTEM_REMEMORY_METRICS,
        slot_id, memory_name, "");
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret:%d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    odata_id_obj = json_object_new_object();
    json_object_object_add(odata_id_obj, RFPROP_ODATA_ID, json_object_new_string(metrics_uri));
    *o_result_jso = odata_id_obj;
    
    return HTTP_OK;
}


gint32 system_memory_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gint32 ret;
    guchar board_type = 0;
    
    gboolean bool_ret;

    
    ret = redfish_get_x86_enable_type(&board_type);
    return_val_if_fail(VOS_OK == ret, HTTP_NOT_FOUND);
    return_val_if_fail(DISABLE != board_type, HTTP_NOT_FOUND);
    

    bool_ret = redfish_check_system_uri_valid(i_paras->uri);
    if (!bool_ret) {
        return HTTP_NOT_FOUND;
    }

    

    
    ret = redfish_check_system_memory_uri_effective(i_paras->member_id, i_paras->uri, &i_paras->obj_handle);
    
    if (ret != VOS_OK) {
        return ret;
    }

    *prop_provider = g_system_memory_provider;
    *count = sizeof(g_system_memory_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}


gboolean rfmemory_type_is_dcpmm(OBJ_HANDLE obj_handle)
{
    guint8 technology2_type = 0;

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_MEM_TECHNOLOGY2_TYPE, &technology2_type);

    if (DCPMM_TECHNOLOGY_TYPE == technology2_type) {
        return TRUE;
    }

    return FALSE;
}
