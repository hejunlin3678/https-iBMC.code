
#include "redfish_provider.h"


#define TOKEN_VALIDITY_REGEX_PATTERN "^[\\w\\W]{0,128}$"

LOCAL gint32 act_set_device_token(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_setdevtoken_actioninfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S  g_rack_mgnt_session_agent_provider[] = {
    {
        RFPROP_SET_DEV_TOKEN_ACTION,
        MAP_PROPERTY_NULL,
        MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        NULL,
        NULL,
        act_set_device_token,
        ETAG_FLAG_ENABLE
    }
};

LOCAL PROPERTY_PROVIDER_S  g_rack_mgnt_set_dev_token_actioninfo_provider[] = {
    {
        RFPROP_ODATA_ID,
        MAP_PROPERTY_NULL,
        MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_setdevtoken_actioninfo_odata_id,
        NULL,
        NULL,
        ETAG_FLAG_ENABLE
    }
};


LOCAL gint32 act_set_device_token(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    json_object *json_obj_ser_num = NULL;
    json_object *json_obj_token = NULL;
    const gchar *ser_num = NULL;
    const gchar *token = NULL;
    OBJ_HANDLE obj_hnd = 0;
    GSList *input_list = NULL;
    gboolean is_match = FALSE;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = json_object_object_get_ex(i_paras->val_jso, RFPROP_SERIAL_NUMBER, &json_obj_ser_num);
    return_val_do_info_if_expr(FALSE == ret, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_ACT_PARA_MISSING, NULL, o_message_jso, RFPROP_SET_DEV_TOKEN_ACTION,
        RFPROP_SERIAL_NUMBER));
    
    ret = json_object_object_get_ex(i_paras->val_jso, RFPROP_TOKEN, &json_obj_token);
    return_val_do_info_if_expr(FALSE == ret, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_ACT_PARA_MISSING, NULL, o_message_jso, RFPROP_SET_DEV_TOKEN_ACTION,
        RFPROP_TOKEN));

    ser_num = dal_json_object_get_str(json_obj_ser_num);

    
    
    ret = dal_find_valid_devive_obj(ser_num, &obj_hnd);
    if (ret != RET_OK) {
        (void)create_message_info(MSGID_INVALID_ACTION_PARAM_VALUE, NULL, o_message_jso, ser_num, RFPROP_SERIAL_NUMBER);
        debug_log(DLOG_ERROR, "No object with giving ser_num %s was found!", ser_num);
        json_object_clear_string(i_paras->val_jso, RFPROP_TOKEN);
        return HTTP_BAD_REQUEST;
    }
    

    
    if (NULL != json_obj_token) {
        token = dal_json_object_get_str(json_obj_token);

        
        is_match = g_regex_match_simple(TOKEN_VALIDITY_REGEX_PATTERN, token, G_REGEX_OPTIMIZE, (GRegexMatchFlags)0);
        if (is_match != TRUE) {
            (void)create_message_info(MSGID_ACT_PARA_FORMAT_ERR, NULL, o_message_jso, RF_SENSITIVE_INFO, RFPROP_TOKEN,
                RFPROP_SET_DEV_TOKEN_ACTION);
            debug_log(DLOG_ERROR, "%s: token does not match with pattern %s", __FUNCTION__,
                TOKEN_VALIDITY_REGEX_PATTERN);
            json_object_clear_string(i_paras->val_jso, RFPROP_TOKEN);
            return HTTP_BAD_REQUEST;
        }

        input_list = g_slist_append(input_list, g_variant_new_string(token));
    } else {
        
        input_list = g_slist_append(input_list, g_variant_new_string(INVALID_DATA_STRING));
    }
    json_object_clear_string(i_paras->val_jso, RFPROP_TOKEN);

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_hnd,
        CLASS_DEVICE_INFO, METHOD_SET_DEVICE_TOKEN, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);

    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
        case RFERR_SUCCESS:
            return HTTP_OK;

            
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
            return RFERR_INSUFFICIENT_PRIVILEGE;

            
        case RFERR_NO_RESOURCE:
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri);
            return HTTP_NOT_FOUND;

            
        default:
            debug_log(DLOG_ERROR, "%s: Set device(SN=%s) token failed. ret=%d\n", __FUNCTION__, ser_num, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 get_setdevtoken_actioninfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_string(URI_FORMAT_SET_DEV_TOKEN_ACTIONINFO);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);
    return HTTP_OK;
}


gint32 get_session_agent(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    gint32 ret;
    OBJ_HANDLE obj_device_info = 0;
    GSList *device_info_list = NULL;
    GSList *device_info_node = NULL;
    guint8 discovered = DEVICE_OFF_LINE;
    json_object *session_info_arr_jso = NULL;
    json_object *dev_session_info_jso = NULL;
    gchar           device_sn[MAX_PRO_SN_LEN + 1] = {0};
    guint8 token_status = TOKEN_INVALID;

    if (o_rsc_jso == NULL || o_err_array_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return RF_FAILED;
    }

    
    session_info_arr_jso = json_object_new_array();
    return_val_do_info_if_fail(NULL != session_info_arr_jso, RF_FAILED,
        debug_log(DLOG_ERROR, "%s: alloc new json array failed", __FUNCTION__));

    json_object_object_add(o_rsc_jso, RFPROP_DEVICES_SESSION_INFO, session_info_arr_jso);

    ret = dfl_get_object_list(CLASS_DEVICE_INFO, &device_info_list);
    return_val_do_info_if_fail(VOS_OK == ret, RF_OK,
        debug_log(DLOG_DEBUG, "%s: Get device info obj list failed, ret=%d", __FUNCTION__, ret));

    for (device_info_node = device_info_list; NULL != device_info_node; device_info_node = device_info_node->next) {
        obj_device_info = (OBJ_HANDLE)device_info_node->data;
        (void)dal_get_property_value_byte(obj_device_info, PROPERTY_DEVICE_INFO_DISCOVERED, &discovered);
        continue_if_expr(DEVICE_OFF_LINE == discovered);

        
        (void)dal_get_property_value_string(obj_device_info, PROPERTY_DEVICE_INFO_SER_NUM, device_sn,
            sizeof(device_sn));
        continue_do_info_if_expr(0 == strlen(device_sn), debug_log(DLOG_DEBUG, "%s: serial number of %s is empty",
            __FUNCTION__, dfl_get_object_name(obj_device_info)));
        (void)dal_get_property_value_byte(obj_device_info, PROPERTY_DEVICE_INFO_TOKEN_STATUS, &token_status);

        
        dev_session_info_jso = json_object_new_object();
        json_object_object_add(dev_session_info_jso, RFPROP_DEVICES_SN,
            json_object_new_string((const gchar *)device_sn));
        json_object_object_add(dev_session_info_jso, RFPROP_TOKEN_AVAILABLE, json_object_new_boolean(token_status));

        
        ret = json_object_array_add(session_info_arr_jso, dev_session_info_jso);
        continue_do_info_if_expr(VOS_OK != ret, json_object_put(dev_session_info_jso);
            debug_log(DLOG_ERROR, "%s: add session info object for device(sn=%s) failed", __FUNCTION__, device_sn));
    }
    g_slist_free(device_info_list);

    return RF_OK;
}


gint32 rack_mgnt_session_agent_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    
    return_val_if_fail(NULL != i_paras && NULL != prop_provider && NULL != count, HTTP_INTERNAL_SERVER_ERROR);

    
    return_val_do_info_if_expr(FALSE == rf_support_rack_mgnt_agent_service(), HTTP_NOT_FOUND,
        debug_log(DLOG_MASS, "%s, %d: not support", __FUNCTION__, __LINE__));

    *prop_provider = g_rack_mgnt_session_agent_provider;
    *count = sizeof(g_rack_mgnt_session_agent_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}


gint32 rack_mgnt_set_dev_token_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    
    return_val_do_info_if_expr(FALSE == rf_support_rack_mgnt_agent_service(), HTTP_NOT_FOUND,
        debug_log(DLOG_MASS, "%s, %d: not support", __FUNCTION__, __LINE__));

    *prop_provider = g_rack_mgnt_set_dev_token_actioninfo_provider;
    *count = sizeof(g_rack_mgnt_set_dev_token_actioninfo_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}
