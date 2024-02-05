
#include "redfish_provider.h"
#include "redfish_provider_system.h"

LOCAL gint32 get_system_memorys_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 get_system_memorys_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 get_system_memorys_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL PROPERTY_PROVIDER_S g_system_memorys_provider[] = {
    
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_memorys_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    
    {RFPROP_MEMBERS_COUNT, CLASS_MEMORY, PROPERTY_MEM_DEVICENAME, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_memorys_count, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS, CLASS_MEMORY, PROPERTY_MEM_DEVICENAME, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_memorys_members, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_memorys_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
};

LOCAL gint32 get_system_memorys_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
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
        snprintf_s(odata_context, sizeof(odata_context), sizeof(odata_context) - 1, SYSTEMS_MEMORYS_METADATA, slot_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    *o_result_jso = json_object_new_string(odata_context);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}

LOCAL gint32 get_system_memorys_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
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
    

    iRet = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, RFPROP_SYSTEM_REMEMORY, slot);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    *o_result_jso = json_object_new_string((const char *)uri);

    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


gint32 get_system_memorys_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gint32 count = 0;
    gchar  device_name[MEMORY_DEVICE_LEN + 1] = {0};
    guchar memory_onset = 0;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = dfl_get_object_list(CLASS_MEMORY, &obj_list);
    
    do_val_if_expr(VOS_OK != ret, debug_log(DLOG_INFO, "%s, %d: get obj_list fail.\n", __FUNCTION__, __LINE__));

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        (void)memset_s(device_name, sizeof(device_name), 0, sizeof(device_name));

        
        
        (void)dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_MEM_DEVICENAME, device_name,
            sizeof(device_name));

        
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_MEM_PRESENCE, &memory_onset);
        do_if_expr(1 != memory_onset || !g_strcmp0(device_name, MEMORY_NA) || 0 == strlen(device_name), continue);
        count++;
    }

    
    *o_result_jso = json_object_new_int(count);
    do_val_if_expr(NULL != obj_list, g_slist_free(obj_list));

    return HTTP_OK;
}


LOCAL gint32 get_system_memorys_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet = -1;
    gint32 ret;
    gchar device_name[MEMORY_DEVICE_LEN + 1] = {0};
    gchar memory_location[MEMORY_LOCATION_LEN + 1] = {0};
    gchar memory_uri[MEMORY_URI_LEN + 1] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    guchar memory_onset = 0;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    json_object *obj_jso = NULL;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = dfl_get_object_list(g_system_memorys_provider[i_paras->index].pme_class_name, &obj_list);
    
    do_val_if_expr(VOS_OK != ret, debug_log(DLOG_ERROR, "%s, %d: get obj_list fail.\n", __FUNCTION__, __LINE__));

    
    *o_result_jso = json_object_new_array();
    
    ret = redfish_get_board_slot(slot, sizeof(slot));
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__);
        
        
        g_slist_free(obj_list);
        
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        (void)memset_s(device_name, sizeof(device_name), 0, sizeof(device_name));
        (void)memset_s(memory_location, sizeof(memory_location), 0, sizeof(memory_location));
        
        (void)dal_get_property_value_string((OBJ_HANDLE)obj_node->data,
            g_system_memorys_provider[i_paras->index].pme_prop_name, device_name, sizeof(device_name));
        dal_clear_string_blank(device_name, sizeof(device_name));

        (void)dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_MEM_LOCATION, memory_location,
            sizeof(memory_location));
        dal_clear_string_blank(memory_location, sizeof(memory_location));

        
        
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_MEM_PRESENCE, &memory_onset);
        do_if_expr(1 != memory_onset || !g_strcmp0(device_name, MEMORY_NA) || 0 == strlen(device_name) ||
            0 == strlen(memory_location),
            continue);

        (void)memset_s(memory_uri, sizeof(memory_uri), 0, sizeof(memory_uri));
        iRet = snprintf_s(memory_uri, sizeof(memory_uri), sizeof(memory_uri) - 1, RFPROP_SYSTEM_REMEMORYSS, slot,
            memory_location, device_name);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
        obj_jso = json_object_new_object();
        json_object_object_add(obj_jso, RFPROP_ODATA_ID, json_object_new_string(memory_uri));
        (void)json_object_array_add(*o_result_jso, obj_jso);
        obj_jso = NULL;
    }

    do_val_if_expr(NULL != obj_list, g_slist_free(obj_list));

    return HTTP_OK;
}


gint32 system_memorys_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
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

    

    *prop_provider = g_system_memorys_provider;
    *count = sizeof(g_system_memorys_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}
