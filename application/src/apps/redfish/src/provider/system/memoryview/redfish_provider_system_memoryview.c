
#include "redfish_provider.h"
#include "redfish_provider_system.h"

LOCAL gint32 get_system_memory_view_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_memory_view_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_memory_view_information(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_system_memory_view_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_memory_view_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,  get_system_memory_view_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_INFORMATION, CLASS_MEMORY, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_memory_view_information, NULL, NULL, ETAG_FLAG_ENABLE}
};


LOCAL gint32 rf_get_memory_view_info_id(OBJ_HANDLE obj_handle, json_object *o_result_jso)
{
    int iRet;
    gchar name[MEMORY_URI_LEN + 1] = {0};
    gchar device_name[MEMORY_DEVICE_LEN + 1] = {0};
    gchar memory_location[MEMORY_LOCATION_LEN + 1] = {0};

    return_val_if_expr(NULL == o_result_jso, VOS_ERR);

    (void)dal_get_property_value_string(obj_handle, PROPERTY_MEM_DEVICENAME, device_name, sizeof(device_name));
    (void)dal_clear_string_blank(device_name, sizeof(device_name));

    (void)dal_get_property_value_string(obj_handle, PROPERTY_MEM_LOCATION, memory_location, sizeof(memory_location));
    (void)dal_clear_string_blank(memory_location, sizeof(memory_location));

    iRet = snprintf_s(name, MEMORY_URI_LEN, MEMORY_URI_LEN - 1, "%s%s", memory_location, device_name);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    json_object_object_add(o_result_jso, RFPROP_COMMON_ID, json_object_new_string(name));

    return VOS_OK;
}


LOCAL gint32 rf_get_memory_view_info_capacity(OBJ_HANDLE obj_handle, json_object *o_result_jso)
{
    guint32 value = 0;
    gchar value_string[PROP_VAL_LENGTH];

    return_val_if_expr(NULL == o_result_jso, VOS_ERR);

    (void)dal_get_property_value_string(obj_handle, PROPERTY_MEM_CAPACITY, value_string, PROP_VAL_LENGTH);

    
    if (rf_string_check(value_string) != VOS_OK) {
        json_object_object_add(o_result_jso, RFPROP_MEMORY_CAPMIB, json_object_new_int(0));
        return VOS_ERR;
    }

    (void)transform_capacity_to_intmb(value_string, strlen(value_string), &value);

    json_object_object_add(o_result_jso, RFPROP_MEMORY_CAPMIB, json_object_new_int(value));

    return VOS_OK;
}


LOCAL gint32 rf_get_memory_view_info_clockspeed(OBJ_HANDLE obj_handle, json_object *o_result_jso)
{
    gint32 ret;
    gint32 value = 0;
    gchar value_string[PROP_VAL_LENGTH];

    return_val_if_expr(NULL == o_result_jso, VOS_ERR);

    (void)dal_get_property_value_string(obj_handle, PROPERTY_MEM_CLOCK_SPEED, value_string, PROP_VAL_LENGTH);

    
    if (rf_string_check(value_string) != VOS_OK) {
        json_object_object_add(o_result_jso, RFPROP_MEMORY_ALLOWSPEEDMHZ, NULL);
        return VOS_ERR;
    }

    ret = redfish_strip_to_int(value_string, sizeof(value_string));
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s, %d: redfish_strip_to_int error", __FUNCTION__, __LINE__);
        json_object_object_add(o_result_jso, RFPROP_MEMORY_ALLOWSPEEDMHZ, NULL);
        return VOS_ERR;
    }

    ret = vos_str2int(value_string, 10, &value, NUM_TPYE_UINT32);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s, %d: vos_str2int error", __FUNCTION__, __LINE__);
        json_object_object_add(o_result_jso, RFPROP_MEMORY_ALLOWSPEEDMHZ, NULL);
        return VOS_ERR;
    }

    json_object_object_add(o_result_jso, RFPROP_MEMORY_ALLOWSPEEDMHZ, json_object_new_int(value));

    return VOS_OK;
}


LOCAL gint32 rf_get_memory_view_info_cfgclockspeed(OBJ_HANDLE obj_handle, json_object *o_result_jso)
{
    gint32 ret;
    gint32 value = 0;
    gchar value_string[PROP_VAL_LENGTH];

    return_val_if_expr(NULL == o_result_jso, VOS_ERR);

    (void)dal_get_property_value_string(obj_handle, PROPERTY_MEM_CONFIG_CLOCK_SPEED, value_string, PROP_VAL_LENGTH);

    
    if (rf_string_check(value_string) != VOS_OK) {
        json_object_object_add(o_result_jso, RFPROP_MEMORY_SPEEDMHZ, NULL);
        return VOS_ERR;
    }

    ret = redfish_strip_to_int(value_string, sizeof(value_string));
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s, %d: redfish_strip_to_int error", __FUNCTION__, __LINE__);
        json_object_object_add(o_result_jso, RFPROP_MEMORY_SPEEDMHZ, NULL);
        return VOS_ERR;
    }

    ret = vos_str2int(value_string, 10, &value, NUM_TPYE_UINT32);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s, %d: vos_str2int error", __FUNCTION__, __LINE__);
        json_object_object_add(o_result_jso, RFPROP_MEMORY_SPEEDMHZ, NULL);
        return VOS_ERR;
    }

    json_object_object_add(o_result_jso, RFPROP_MEMORY_SPEEDMHZ, json_object_new_int(value));

    return VOS_OK;
}


LOCAL gint32 rf_get_memory_view_info_rankcount(OBJ_HANDLE obj_handle, json_object *o_result_jso)
{
    gint32 ret;
    guchar value = 0;

    return_val_if_expr(NULL == o_result_jso, VOS_ERR);

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_MEM_RANK, &value);
    if (VOS_OK != ret || 0 == value) {
        json_object_object_add(o_result_jso, RFPROP_MEMORY_RANKCOUNT, NULL);
        return VOS_ERR;
    }

    json_object_object_add(o_result_jso, RFPROP_MEMORY_RANKCOUNT, json_object_new_int(value));

    return VOS_OK;
}


LOCAL gint32 rf_get_memory_view_info_devicetype(OBJ_HANDLE obj_handle, json_object *o_result_jso)
{
    gchar value_string[PROP_VAL_LENGTH];
    gchar *rf_type = NULL;

    return_val_if_expr(NULL == o_result_jso, VOS_ERR);

    (void)dal_get_property_value_string(obj_handle, PROPERTY_MEM_TYPE, value_string, PROP_VAL_LENGTH);

    
    if (rf_string_check(value_string) != VOS_OK) {
        json_object_object_add(o_result_jso, RFPROP_MEMORY_DEVICETYPE, NULL);
        return VOS_ERR;
    }

    rf_type = rfmemory_type_to_rftype(value_string, sizeof(value_string));
    if (NULL == rf_type) {
        json_object_object_add(o_result_jso, RFPROP_MEMORY_DEVICETYPE, NULL);
        return VOS_ERR;
    }

    json_object_object_add(o_result_jso, RFPROP_MEMORY_DEVICETYPE, json_object_new_string(rf_type));

    return VOS_OK;
}

LOCAL gint32 rf_get_memory_view_info_status(OBJ_HANDLE obj_handle, json_object *o_result_jso, guchar power_state)
{
    guchar presence = 0;
    guchar health = 0;
    const gchar *state_str = RF_STATE_ENABLED;
    json_object *status_obj_jso = NULL;

    return_val_if_expr(NULL == o_result_jso, VOS_ERR);

    // power_state取值1表示上电状态
    if (power_state == 1) {
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_MEM_HEALTH, &health);
    } else {
        health = INVALID_DATA_BYTE;
    }

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_MEM_PRESENCE, &presence);
    
    if (0 == presence) {
        state_str = RF_STATE_ABSENT;
        health = INVALID_DATA_BYTE;
    }
    

    (void)get_resource_status_property(&health, NULL, state_str, &status_obj_jso, FALSE);

    if (status_obj_jso != NULL) {
        json_object_object_add(o_result_jso, RFPROP_SYSTEM_STATUS, status_obj_jso);
    } else {
        json_object_object_add(o_result_jso, RFPROP_SYSTEM_STATUS, NULL);
    }

    return VOS_OK;
}


LOCAL gint32 rf_get_memory_view_info_fw_version(OBJ_HANDLE obj_handle, json_object *o_result_jso)
{
    gint32 ret;
    gchar fw_verion[MAX_PROP_VAL_LEN + 1] = {0};

    return_val_if_expr(NULL == o_result_jso, VOS_ERR);

    (void)dal_get_property_value_string(obj_handle, PROPERTY_MEM_FWVERSION, fw_verion, sizeof(fw_verion));

    ret = check_string_val_effective(fw_verion);
    if (ret == VOS_OK) {
        json_object_object_add(o_result_jso, RFPROP_MEMORY_FIRMWARE_VERSION, json_object_new_string(fw_verion));
    } else {
        json_object_object_add(o_result_jso, RFPROP_MEMORY_FIRMWARE_VERSION, NULL);
    }

    return VOS_OK;
}


LOCAL gint32 rf_get_memory_view_info_fw_volatile_size_mb(OBJ_HANDLE obj_handle, json_object *o_result_jso)
{
    guint32 volatile_size = 0;
    guint32 persistent_size = 0;

    return_val_if_expr(NULL == o_result_jso, VOS_ERR);

    (void)dal_get_property_value_uint32(obj_handle, PROPERTY_MEM_VOLATILECAPACITY, &volatile_size);
    (void)dal_get_property_value_uint32(obj_handle, PROPERTY_MEM_PERSISTENTCAPACITY, &persistent_size);

    if (0 != (volatile_size + persistent_size)) {
        json_object_object_add(o_result_jso, RFPROP_MEMORY_VOLATILE_CAPACITY, json_object_new_int(volatile_size));
    } else {
        json_object_object_add(o_result_jso, RFPROP_MEMORY_VOLATILE_CAPACITY, NULL);
    }

    return VOS_OK;
}


LOCAL gint32 rf_get_memory_view_info_fw_persistent_size_mb(OBJ_HANDLE obj_handle, json_object *o_result_jso)
{
    guint32 volatile_size = 0;
    guint32 persistent_size = 0;

    return_val_if_expr(NULL == o_result_jso, VOS_ERR);

    (void)dal_get_property_value_uint32(obj_handle, PROPERTY_MEM_VOLATILECAPACITY, &volatile_size);
    (void)dal_get_property_value_uint32(obj_handle, PROPERTY_MEM_PERSISTENTCAPACITY, &persistent_size);

    if (0 != (volatile_size + persistent_size)) {
        json_object_object_add(o_result_jso, RFPROP_MEMORY_PERSISTENT_CAPACITY, json_object_new_int(persistent_size));
    } else {
        json_object_object_add(o_result_jso, RFPROP_MEMORY_PERSISTENT_CAPACITY, NULL);
    }

    return VOS_OK;
}


LOCAL gint32 get_system_memory_view_information(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    json_object *node_object = NULL;
    guchar power_state = 0;

    
    return_val_do_info_if_expr(o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_array();
    return_val_do_info_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:json_object_new_array fail", __FUNCTION__));

    
    ret = dfl_get_object_list(g_system_memory_view_provider[i_paras->index].pme_class_name, &obj_list);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s, %d: get obj_list fail.\n", __FUNCTION__, __LINE__));

    
    ret = rf_get_system_powerstate(&power_state);
    do_val_if_expr(ret != VOS_OK, debug_log(DLOG_ERROR, "%s, %d: get system power state fail", __FUNCTION__, __LINE__));

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        node_object = json_object_new_object();
        continue_if_expr(node_object == NULL);

        ret = json_object_array_add(*o_result_jso, node_object);
        continue_do_info_if_expr(ret != VOS_OK, json_object_put(node_object));

        rf_get_memory_view_info_id((OBJ_HANDLE)obj_node->data, node_object);

        rf_get_memory_view_info_capacity((OBJ_HANDLE)obj_node->data, node_object);

        rf_add_property_jso_string((OBJ_HANDLE)obj_node->data, PROPERTY_MEM_MANUFACTURE, RFPROP_MEMORY_MANFAC,
            node_object);

        rf_get_memory_view_info_clockspeed((OBJ_HANDLE)obj_node->data, node_object);

        rf_get_memory_view_info_cfgclockspeed((OBJ_HANDLE)obj_node->data, node_object);

        rf_add_property_jso_string((OBJ_HANDLE)obj_node->data, PROPERTY_MEM_SN, RFPROP_MEMORY_SN, node_object);
        rf_add_property_jso_string((OBJ_HANDLE)obj_node->data, PROPERTY_MEM_PART_NUM, RFPROP_PART_NUMBER, node_object);

        rf_get_memory_view_info_devicetype((OBJ_HANDLE)obj_node->data, node_object);

        rf_add_property_jso_uint16((OBJ_HANDLE)obj_node->data, PROPERTY_MEM_BIT_WIDTH, RFPROP_MEMORY_DATAWIDTH,
            node_object);

        rf_get_memory_view_info_rankcount((OBJ_HANDLE)obj_node->data, node_object);

        rf_add_property_jso_string((OBJ_HANDLE)obj_node->data, PROPERTY_MEM_DEVICENAME, RFPROP_MEMORY_DEVICELOCATOR,
            node_object);
        rf_add_property_jso_string((OBJ_HANDLE)obj_node->data, PROPERTY_MEM_BASEMODULE, PROPERTY_MEM_BASEMODULE,
            node_object);
        
        (void)rf_get_memory_view_info_fw_version((OBJ_HANDLE)obj_node->data, node_object);
        (void)rf_get_memory_view_info_fw_volatile_size_mb((OBJ_HANDLE)obj_node->data, node_object);
        (void)rf_get_memory_view_info_fw_persistent_size_mb((OBJ_HANDLE)obj_node->data, node_object);
        (void)rf_add_property_jso_byte((OBJ_HANDLE)obj_node->data, PROPERTY_MEM_REMAINLIFE,
            RFPROP_MEMORY_REMAINING_SERVICE_LIFE, node_object);
        (void)get_system_memory_temperature((OBJ_HANDLE)obj_node->data, PROPERTY_MEM_MEDIATEMP,
            RFPROP_MEMORY_MEDIUM_TEMPERATURE, node_object);
        (void)get_system_memory_temperature((OBJ_HANDLE)obj_node->data, PROPERTY_MEM_CONTROLLERTEMP,
            RFPROP_MEMORY_CONTROL_TEMPERATURE, node_object);
        

        rf_add_property_jso_byte((OBJ_HANDLE)obj_node->data, PROPERTY_MEM_CPU_ID, RFPROP_MEMORY_SOCKET, node_object);
        rf_add_property_jso_byte((OBJ_HANDLE)obj_node->data, PROPERTY_MEM_NODE_ID, RFPROP_MEMORY_CONTROLLER,
            node_object);
        rf_add_property_jso_byte((OBJ_HANDLE)obj_node->data, PROPERTY_MEM_CHANNEL_ID, RFPROP_MEMORY_CHANNEL,
            node_object);
        rf_add_property_jso_byte((OBJ_HANDLE)obj_node->data, PROPERTY_MEM_DIMM_ID, RFPROP_MEMORY_SLOT, node_object);

        rf_add_property_jso_uint16((OBJ_HANDLE)obj_node->data, PROPERTY_MEM_MINIMUM_VOLTAGE, RFPROP_MEMORY_MINVOLTAGE,
            node_object);
        
        rf_add_property_jso_string((OBJ_HANDLE)obj_node->data, PROPERTY_MEM_TECHNOLOGY2, RFPROP_MEMORY_TECHNOLOGY,
            node_object);
        rf_add_property_jso_string((OBJ_HANDLE)obj_node->data, PROPERTY_MEM_TEC, RFPROP_MEMORY_TYPE_DETAIL,
            node_object);
        
        rf_add_property_jso_string((OBJ_HANDLE)obj_node->data, PROPERTY_MEM_LOCATION, RFPROP_MEMORY_POSITION,
            node_object);

        
        rf_add_property_jso_string((OBJ_HANDLE)obj_node->data, PROPERTY_MEM_TYPE, PROPERTY_MEM_TYPE, node_object);
        

        rf_get_memory_view_info_status((OBJ_HANDLE)obj_node->data, node_object, power_state);

        rf_add_property_jso_string((OBJ_HANDLE)obj_node->data, PROPERTY_MEM_ORIGINAL_PART_NUM,
            RFPROP_MEMORY_ORIGINAL_PART_NUM, node_object);
        rf_add_property_jso_string((OBJ_HANDLE)obj_node->data, PROPERTY_MEM_BOM_NUM, RFPROP_MEMORY_BOM_NUM,
            node_object);
    }

    g_slist_free(obj_list);

    return HTTP_OK;
}


LOCAL gint32 get_system_memory_view_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;
    gint32 ret;
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_slot(slot, sizeof(slot));

    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));

    iRet = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, RFPROP_SYSTEM_MEMORY_VIEW, slot);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    *o_result_jso = json_object_new_string((const char *)uri);

    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 get_system_memory_view_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
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

    iRet = snprintf_s(odata_context, sizeof(odata_context), sizeof(odata_context) - 1, SYSTEMS_MEMORY_VIEW_METADATA,
        slot_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    *o_result_jso = json_object_new_string(odata_context);

    return_val_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


gint32 system_memory_view_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
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

    *prop_provider = g_system_memory_view_provider;
    *count = sizeof(g_system_memory_view_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}
