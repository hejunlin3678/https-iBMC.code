

#include "redfish_provider.h"

LOCAL gint32 get_chassis_boards_arcard_actioninfo(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_chassis_boards_arcard_actioninfo_provider[] = {
    {RFPROP_ODATA_ID, CLASS_COMPONENT, PROPERTY_COMPONENT_DEVICE_TYPE, USERROLE_READONLY,
        SYS_LOCKDOWN_ALLOW, get_chassis_boards_arcard_actioninfo, NULL, NULL, ETAG_FLAG_ENABLE}
};


LOCAL gboolean redfish_find_arcard_component(const gchar* origin_uri, gchar* slot_str, guint32 buf_len)
{
    GSList* obj_list = NULL;
    GSList* obj_node = NULL;
    guint8 component_type = 0;
    guint8 fru_id = 0;
    guint8 presence = 0;
    gchar device_name[MAX_STRBUF_LEN] = {0};
    gchar board_location[MAX_STRBUF_LEN] = {0};
    gchar uri_substr[MAX_STRBUF_LEN] = {0};
    gchar* res = NULL;

    
    gint32 ret = dfl_get_object_list(CLASS_COMPONENT, &obj_list);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    for (obj_node = obj_list; obj_node; obj_node = g_slist_next(obj_node)) {
        
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_DEVICE_TYPE, &component_type);
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_PRESENCE, &presence);
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_FRUID, &fru_id);
        if (presence == 0 || fru_id == 0xff || component_type != COMPONENT_TYPE_AR_CARD) {
            continue;
        }

        (void)memset_s(device_name, sizeof(device_name), 0, sizeof(device_name));
        (void)memset_s(board_location, sizeof(board_location), 0, sizeof(board_location));

        (void)dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_DEVICE_NAME,
                                            device_name, sizeof(device_name));
        (void)dal_clear_string_blank(device_name, sizeof(device_name));

        (void)dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_LOCATION,
                                            board_location, sizeof(board_location));
        (void)dal_clear_string_blank(board_location, sizeof(board_location));

        ret = snprintf_s(uri_substr, sizeof(uri_substr), sizeof(uri_substr) - 1, "%s%s", board_location, device_name);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s : snprintf_s fail.", __FUNCTION__);
            continue;
        }

        
        ret = rf_get_regex_match_result_nth(uri_substr, origin_uri, 1, &res);
        if (ret != RET_OK) {
            continue;
        }
        
        ret = strncpy_s(slot_str, buf_len, uri_substr, strlen(uri_substr));
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "copy slot info failed");
            continue;
        }
        g_slist_free(obj_list);

        return RET_OK;
    }
    debug_log(DLOG_ERROR, "%s : object not found.\n", __FUNCTION__);
    g_slist_free(obj_list);

    return RET_ERR;
}


LOCAL gint32 get_chassis_boards_arcard_actioninfo(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso)
{
    gint32 ret;
    gchar string_value[ARRAY_LENTH] = {0};
    gchar slot_str[MAX_RSC_ID_LEN] = {0};
    gchar member_id[MAX_STRBUF_LEN] = {0};

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (!i_paras->is_satisfy_privi) {
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: Privilege error.", __FUNCTION__);
        return HTTP_FORBIDDEN;
    }

    
    ret = rf_gen_chassis_component_id(NULL, i_paras->obj_handle, slot_str, MAX_RSC_ID_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Get slot failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = redfish_find_arcard_component(i_paras->uri, member_id, sizeof(member_id));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Get member_id failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1,
                     RF_CHASSIS_OEM_ACTION_ARCARD_CONTROL_ACTIONINFO, slot_str, member_id);
    if (ret <= 0) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const gchar*)string_value);
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: Get result failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


gint32 chassis_boards_arcard_control_actioninfo_provider_entry(PROVIDER_PARAS_S* i_paras,
    PROPERTY_PROVIDER_S** prop_provider, guint32* count)
{
    gboolean ret;

    ret = redfish_check_chassis_uri_valid(i_paras->uri, &i_paras->obj_handle);
    if (ret != TRUE || i_paras->obj_handle == OBJ_HANDLE_COMMON) {
        debug_log(DLOG_ERROR, "%s: URL is not correct.", __FUNCTION__);
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_chassis_boards_arcard_actioninfo_provider;
    *count = sizeof(g_chassis_boards_arcard_actioninfo_provider) / sizeof(PROPERTY_PROVIDER_S);

    return RET_OK;
}
