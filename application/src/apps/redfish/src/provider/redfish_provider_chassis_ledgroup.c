

#include "redfish_provider.h"

#define PROPERTY_OBJECT_MAP_MAX_LINE 2

LOCAL gint32 get_chassis_led_group_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_led_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 set_chassis_led_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_chassis_led_group_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY,
        SYS_LOCKDOWN_NULL, get_chassis_led_group_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_CHASSIS_ALARM_LED_F, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY,
        SYS_LOCKDOWN_NULL, get_chassis_led_status, set_chassis_led_status, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_CHASSIS_IDENTIFY_LED_F, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY,
        SYS_LOCKDOWN_NULL, get_chassis_led_status, set_chassis_led_status, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_CHASSIS_PUSH_LED_F, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY,
        SYS_LOCKDOWN_NULL, get_chassis_led_status, set_chassis_led_status, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_CHASSIS_PULL_LED_F, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY,
        SYS_LOCKDOWN_NULL, get_chassis_led_status, set_chassis_led_status, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_CHASSIS_ALARM_LED_R, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY,
        SYS_LOCKDOWN_NULL, get_chassis_led_status, set_chassis_led_status, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_CHASSIS_IDENTIFY_LED_R, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY,
        SYS_LOCKDOWN_NULL, get_chassis_led_status, set_chassis_led_status, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_CHASSIS_PUSH_LED_R, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY,
        SYS_LOCKDOWN_NULL, get_chassis_led_status, set_chassis_led_status, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_CHASSIS_PULL_LED_R, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY,
        SYS_LOCKDOWN_NULL, get_chassis_led_status, set_chassis_led_status, NULL, ETAG_FLAG_ENABLE},
};


LOCAL gint32 get_chassis_led_group_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int result;
    gint32 ret;
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (!i_paras->is_satisfy_privi) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    ret = redfish_get_board_slot(slot, sizeof(slot));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: function  return err of redfish_slot_id.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    result = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, URI_FORMAT_CHASSIS_LED_GROUP, "Enc");
    if (result <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, result);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const char *)uri);
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 get_chassis_led_info(PROVIDER_PARAS_S* i_paras, gchar *obj_name, guint8 *status)
{
    GSList* output_list = NULL;
    OBJ_HANDLE obj_handle;
    gint32 ret;

    
    ret = dfl_get_object_handle(obj_name, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get object(%s) handle failed, ret=%d.", obj_name, ret);
        return ret;
    }

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        I2C_LED_CLASS_NAME, METHOD_GET_STATE, AUTH_ENABLE, i_paras->user_role_privilege, NULL, &output_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "call class(%s) method(%s) failed, ret(%d).", I2C_LED_CLASS_NAME, METHOD_GET_STATE, ret);
        return RET_ERR;
    }

    *status = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 0));
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    return RET_OK;
}


LOCAL gint32 find_chassis_led_obj_name(guint8 idx, gchar *obj_name, guint8 len)
{
    gint32 ret;
    const char* property_object_map[][PROPERTY_OBJECT_MAP_MAX_LINE] = {
        {RFPROP_CHASSIS_ALARM_LED_F,    "FrameAlarmLed_F"},
        {RFPROP_CHASSIS_IDENTIFY_LED_F, "FrameIdentifyLed_F"},
        {RFPROP_CHASSIS_PUSH_LED_F,     "FramePushLed_F"}, 
        {RFPROP_CHASSIS_PULL_LED_F,     "FramePullLed_F"}, 
        {RFPROP_CHASSIS_ALARM_LED_R,    "FrameAlarmLed_R"},
        {RFPROP_CHASSIS_IDENTIFY_LED_R, "FrameIdentifyLed_R"},
        {RFPROP_CHASSIS_PUSH_LED_R,     "FramePushLed_R"},  
        {RFPROP_CHASSIS_PULL_LED_R,     "FramePullLed_R"}  
    };

    
    for (guint32 i = 0; i < G_N_ELEMENTS(property_object_map); i++) {
        if (!strcmp(g_chassis_led_group_provider[idx].property_name, property_object_map[i][0])) {
            ret = strncpy_s(obj_name, len - 1, property_object_map[i][1], strlen(property_object_map[i][1]));
            return ret;
        }
    }

    return RET_ERR;
}


LOCAL gint32 get_chassis_led_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    guint8 led_state;
    gchar led_name[MAX_OBJECT_NAME_LEN + 1] = {0};
    gint32 ret;

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (!i_paras->is_satisfy_privi) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = find_chassis_led_obj_name(i_paras->index, led_name, sizeof(led_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get chassis alarm led obj name failed, ret=%d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = get_chassis_led_info(i_paras, led_name, &led_state);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get chassis alarm led state failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (led_state == LED_ON) {
        if (g_strcmp0(led_name, "FrameIdentifyLed_F") == 0 || g_strcmp0(led_name, "FrameIdentifyLed_R") == 0) {
            *o_result_jso = json_object_new_string(RF_LED_BLINKING);
        } else {
            *o_result_jso = json_object_new_string(RF_LED_LIT);
        }
    } else {
        *o_result_jso = json_object_new_string(RF_LED_OFF);
    }
    return HTTP_OK;
}


LOCAL gint32 set_chassis_led_info(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, const char *obj_name)
{
    OBJ_HANDLE obj_handle;
    const char *set_led_state = NULL;
    GSList *input_list = NULL;
    gint32 ret;

    ret = dfl_get_object_handle(obj_name, &obj_handle);
    if (ret != RET_OK) {
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, RFPROP_CHASSIS_INDICATELED, o_message_jso,
            RFPROP_CHASSIS_INDICATELED);
        return HTTP_NOT_IMPLEMENTED;
    }

    
    set_led_state = dal_json_object_get_str(i_paras->val_jso);
    
    if (!g_strcmp0(RF_LED_LIT, set_led_state) || !g_strcmp0(RF_LED_BLINKING, set_led_state)) {
        input_list = g_slist_append(input_list, g_variant_new_byte(1));
        input_list = g_slist_append(input_list, g_variant_new_byte(0));
    } else if (!g_strcmp0(RF_LED_OFF, set_led_state)) {
        input_list = g_slist_append(input_list, g_variant_new_byte(0));
        input_list = g_slist_append(input_list, g_variant_new_byte(1));
    } else {
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, RFPROP_CHASSIS_INDICATELED, o_message_jso, set_led_state,
            RFPROP_CHASSIS_INDICATELED);
        return HTTP_BAD_REQUEST;
    }

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        I2C_LED_CLASS_NAME, METHOD_SET_STATE, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "call class(%s) method(%s) failed, ret(%d).", I2C_LED_CLASS_NAME, METHOD_SET_STATE, ret);
    }

    uip_free_gvariant_list(input_list);
    return ret;
}


LOCAL gint32 set_chassis_led_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar led_name[32];  

    
    if (o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (!(i_paras->user_role_privilege & USERROLE_BASICSETTING)) {
        debug_log(DLOG_ERROR, "%s: patch privilege is invalid.", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_CHASSIS_INDICATELED, o_message_jso,
            RFPROP_CHASSIS_INDICATELED);
        return HTTP_FORBIDDEN;
    }

    
    ret = find_chassis_led_obj_name(i_paras->index, led_name, sizeof(led_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get chassis alarm led obj name failed, ret=%d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = set_chassis_led_info(i_paras, o_message_jso, led_name);
    switch (ret) {
        case RFERR_NO_RESOURCE:
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri);
            return HTTP_NOT_FOUND;

        case RET_OK:
        case RFERR_SUCCESS:
            return HTTP_OK;

            
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


gint32 chassis_led_group_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    OBJ_HANDLE component_handle;
    gboolean bool_ret;

    bool_ret = rf_check_chassis_uri_valid_allow_node_enc(i_paras->uri, &component_handle);
    if (!bool_ret) {
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_chassis_led_group_provider;
    *count = sizeof(g_chassis_led_group_provider) / sizeof(PROPERTY_PROVIDER_S);
    return RET_OK;
}