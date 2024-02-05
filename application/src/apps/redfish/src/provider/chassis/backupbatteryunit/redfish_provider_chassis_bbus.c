

#include "redfish_provider.h"

LOCAL gint32 get_chassis_bbus_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_bbus_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_chassis_bbus_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_chassis_bbus_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_bbus_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS_COUNT, CLASS_BBU_MODULE_NAME, PROPERTY_BBU_MODULE_ID, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_bbus_count, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS, CLASS_BBU_MODULE_NAME, PROPERTY_BBU_MODULE_ID, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_bbus_members, NULL, NULL, ETAG_FLAG_ENABLE},
};

LOCAL gint32 get_chassis_bbus_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int result;
    gint32 ret;
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};

    
    return_val_do_info_if_expr(
        (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_expr(!i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));

    result = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, RFPROP_CHASSIS_BBUS, slot);
    do_val_if_expr(result <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, result));

    *o_result_jso = json_object_new_string((const char *)uri);

    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}

LOCAL gint32 get_chassis_bbus_count(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, json_object** o_result_jso)
{
    gint32 ret;
    GSList* obj_list = NULL;

    
    return_val_do_info_if_expr(
        (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_expr(!i_paras->is_satisfy_privi, HTTP_FORBIDDEN, 
                               (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
                                
    ret = dfl_get_object_list(g_chassis_bbus_provider[i_paras->index].pme_class_name, &obj_list);
    if (ret != RET_OK) {
        *o_result_jso = json_object_new_int(0);
        return HTTP_OK;
    }

    
    *o_result_jso = json_object_new_int((gint32)g_slist_length(obj_list));
    g_slist_free(obj_list);
    
    return HTTP_OK;
}

LOCAL gint32 get_chassis_bbus_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int result;
    gint32 ret;
    guchar bbu_id;
    gchar bbus_uri[MAX_URI_LENGTH + 1] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    json_object *obj_jso = NULL;

    
    return_val_do_info_if_expr(
        (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_expr(!i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    *o_result_jso = json_object_new_array();

    
    ret = dfl_get_object_list(g_chassis_bbus_provider[i_paras->index].pme_class_name, &obj_list);
    return_val_if_expr(ret == ERRCODE_OBJECT_NOT_EXIST, HTTP_OK);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s, %d: get obj_list fail.\n", __FUNCTION__, __LINE__));

    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR, g_slist_free(obj_list);
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        bbu_id = 0;
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data,
            g_chassis_bbus_provider[i_paras->index].pme_prop_name, &bbu_id);
        continue_if_expr(ret != VOS_OK);

        result = snprintf_s(bbus_uri, sizeof(bbus_uri), sizeof(bbus_uri) - 1, URI_FORMAT_CHASSIS_BBU, slot, bbu_id);
        do_val_if_expr(result <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, result));

        obj_jso = json_object_new_object();
        json_object_object_add(obj_jso, RFPROP_ODATA_ID, json_object_new_string(bbus_uri));
        json_object_array_add(*o_result_jso, obj_jso);
        obj_jso = NULL;
    }

    g_slist_free(obj_list);
    return HTTP_OK;
}

gint32 chassis_bbus_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gint32 ret;
    OBJ_HANDLE component_handle = 0;
    gboolean bool_ret;

    bool_ret = redfish_check_chassis_uri_valid(i_paras->uri, &component_handle);
    do_if_expr(!bool_ret, return HTTP_NOT_FOUND);

    ret = check_enclosure_component_type(component_handle, FALSE);
    return_val_if_expr(ret != VOS_OK, HTTP_NOT_FOUND);

    *prop_provider = g_chassis_bbus_provider;
    *count = sizeof(g_chassis_bbus_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}

void get_system_overview_battery(json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE sys_battery = 0;

    if (o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: Input pram can not be null ", __FUNCTION__);
        return;
    }

    *o_result_jso = json_object_new_object();
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: new json object failed ", __FUNCTION__);
        return;
    }

    ret = dal_get_object_handle_nth(CLASS_SYS_BATTERY, 0, &sys_battery);
    if (ret != VOS_OK) {
        debug_log(DLOG_DEBUG, "%s: Get obj sysbattery fail", __FUNCTION__);
        return;
    }

    rf_add_property_jso_uint32(sys_battery, PROPERTY_SYS_BATTERY_RATED_CAP, RFPROP_BBU_RATED_CAP_WATT_HOUR,
        *o_result_jso);

    rf_add_property_jso_uint32(sys_battery, PROPERTY_SYS_BATTERY_REMAIN_CAP, RFPROP_BBU_REMAIN_CAP_WATT_HOUR,
        *o_result_jso);

    rf_add_property_jso_byte(sys_battery, PROPERTY_SYS_BATTERY_BAT_PRES_COUNTS, RFPROP_BBU_PRESNET_NUMBER,
        *o_result_jso);

    return;
}
