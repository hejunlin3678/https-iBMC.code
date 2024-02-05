

#include "redfish_provider.h"

LOCAL gint32 get_manager_fpcservice_memory_context(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso) 
{
    return get_universal_provider_odata_info(i_paras, o_message_jso,
        o_result_jso, RFPROP_MANAGER_FPCSERVICE_MEMORY_CONTEXT);
}

LOCAL gint32 get_manager_fpcservice_memory_odataid(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso) 
{
    return get_universal_provider_odata_info(i_paras, o_message_jso,
        o_result_jso, RFPROP_MANAGER_FPCSERVICE_MEMORY);
}

LOCAL gint32 get_manager_fpcservice_memory_count(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso)
{
    return get_system_memorys_count(i_paras, o_message_jso, o_result_jso);
}
LOCAL void __get_fpcservice_memory_records(GSList* obj_list, json_object** o_result_jso)
{
    GSList* obj_node = NULL;
    json_object* obj_jso = NULL;
    gchar device_name[MEMORY_DEVICE_LEN + 1] = {0};
    gchar serial_number[MEMORY_SN_LEN + 1] = {0};
    guint8 memory_onset = 0;
    guint8 socket = 0;
    guint8 channel = 0;
    guint8 slot = 0;
    guint8 health_score = 0;
    guint8 isolation_status = 0;
    gint16 time_zone = 0;
    guint32 time_stamp = 0;

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        memset_s(device_name, sizeof(device_name), 0, sizeof(device_name));
        memset_s(serial_number, sizeof(serial_number), 0, sizeof(serial_number));
        (void)dal_get_property_value_string((OBJ_HANDLE)obj_node->data,
            PROPERTY_MEM_NAME, device_name, sizeof(device_name));
        dal_clear_string_blank(device_name, sizeof(device_name));
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_MEM_PRESENCE, &memory_onset);
        
        if (memory_onset != PRESENT_STATE || !g_strcmp0(device_name, MEMORY_NA) || strlen(device_name) == 0) {
            continue;
        }

        obj_jso = json_object_new_object();
        (void)dal_get_property_value_uint32((OBJ_HANDLE)obj_node->data, PROPERTY_MEM_LAST_PREDICT_TIME, &time_stamp);
        json_object_object_add(obj_jso, RFPROP_FPCSERVICE_MEMORY_TIMESTAMP, json_object_new_int(time_stamp));

        (void)dal_get_localtime_offset(&time_zone);
        json_object_object_add(obj_jso, RFPROP_FPCSERVICE_MEMORY_TIMEZONE, json_object_new_int(time_zone));

        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_MEM_CPU_ID, &socket);
        json_object_object_add(obj_jso, RFPROP_FPCSERVICE_MEMORY_SOCKET, json_object_new_int(socket));

        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_MEM_CHANNEL_ID, &channel);
        json_object_object_add(obj_jso, RFPROP_FPCSERVICE_MEMORY_CHANNEL, json_object_new_int(channel));

        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_MEM_DIMM_ID, &slot);
        json_object_object_add(obj_jso, RFPROP_FPCSERVICE_MEMORY_SLOT, json_object_new_int(slot));

        json_object_object_add(obj_jso, RFPROP_FPCSERVICE_MEMORY_DEVICELOCATOR, json_object_new_string(device_name));

        (void)dal_get_property_value_string((OBJ_HANDLE)obj_node->data,
            PROPERTY_MEM_SN, serial_number, sizeof(serial_number));
        json_object_object_add(obj_jso, RFPROP_FPCSERVICE_MEMORY_SN, json_object_new_string(serial_number));

        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_MEM_HEALTHSCORE, &health_score);
        json_object_object_add(obj_jso, RFPROP_FPCSERVICE_MEMORY_HEALTHSCORE, json_object_new_int(health_score));

        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data,
            PROPERTY_MEM_LAST_ISOLATION_STATUS, &isolation_status);
        json_object_object_add(obj_jso,
            RFPROP_FPCSERVICE_MEMORY_LAST_ISOLATION_STATUS, json_object_new_int(isolation_status));

        (void)json_object_array_add(*o_result_jso, obj_jso);
        obj_jso = NULL;
    }
}
LOCAL gint32 get_manager_fpcservice_memory_records(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso)
{
    gint32 ret;
    GSList* obj_list = NULL;

    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = dfl_get_object_list(CLASS_MEMORY, &obj_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get obj_list fail.", __FUNCTION__);
    }
    *o_result_jso = json_object_new_array();

    __get_fpcservice_memory_records(obj_list, o_result_jso);
    if (obj_list != NULL) {
        g_slist_free(obj_list);
    }

    return HTTP_OK;
}

LOCAL PROPERTY_PROVIDER_S g_manager_fpcservice_memory_provider[] = {
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_fpcservice_memory_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_fpcservice_memory_odataid, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_FPCSERVICE_MEMORY_COUNT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_fpcservice_memory_count, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_FPCSERVICE_MEMORY_RECORDS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_fpcservice_memory_records, NULL, NULL, ETAG_FLAG_ENABLE}
};
gint32 manager_provider_fpcservice_memory_entry(PROVIDER_PARAS_S* i_paras,
    PROPERTY_PROVIDER_S** prop_provider, guint32* count)
{
    gboolean ret;
    guint8 fpc_service_enable = 0;

    ret = redfish_check_manager_uri_valid(i_paras->uri);
    if (ret != TRUE) {
        return HTTP_NOT_FOUND;
    }
    
    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG,
        PROPERTY_PME_SERVICECONFIG_FPC_SUPPORT, &fpc_service_enable);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: get PMEServiceConfig obj's FPCSupport failed, ret = %d.", __FUNCTION__, ret);
        return HTTP_NOT_FOUND;
    }
    if (fpc_service_enable != 1) {
        debug_log(DLOG_DEBUG, "%s:This resource is not enabled by license, fpc_service_enable = %d.",
            __FUNCTION__, fpc_service_enable);
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_manager_fpcservice_memory_provider;
    *count = sizeof(g_manager_fpcservice_memory_provider) / sizeof(PROPERTY_PROVIDER_S);

    return RET_OK;
}
