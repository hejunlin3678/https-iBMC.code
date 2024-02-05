

#include "redfish_provider.h"

LOCAL gint32 lldp_set_propery_uint16(const gchar *set_method, PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
gint32 check_param_and_privi_validity(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    if (o_message_jso == NULL || o_result_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_expr(i_paras->is_satisfy_privi == 0, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    return VOS_OK;
}


gint32 check_redfish_call_method_result(const gint32 ret, json_object **o_message_jso, const gchar *prop_name,
    const gchar *set_str)
{
    
    switch (ret) {
        case VOS_OK:
        case RFERR_SUCCESS:
            return HTTP_OK;

        case RFERR_WRONG_PARAM: 
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, prop_name, o_message_jso, set_str, prop_name);
            return HTTP_BAD_REQUEST;

        case RFERR_INSUFFICIENT_PRIVILEGE: 
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop_name, o_message_jso, prop_name);
            return HTTP_FORBIDDEN;

        default: 
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            debug_log(DLOG_ERROR, "%s, %d: call method fail %d.", __FUNCTION__, __LINE__, ret);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 get_property_lldpservice_enable(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guchar property_value = 0;

    
    ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    return_val_if_expr(ret != VOS_OK, ret);

    ret = dal_get_object_handle_nth(CLASS_NAME_LLDP_CONFIG, 0, &obj_handle);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get_object_handle fail(%d).\n", __FUNCTION__, __LINE__, ret));

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_LLDP_CONFIG_ENABLE, &property_value);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property_data fail(%d).\n", __FUNCTION__, __LINE__, ret));

    
    *o_result_jso = json_object_new_boolean(property_value);
    return_val_do_info_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_boolean fail.\n", __FUNCTION__, __LINE__));
    return HTTP_OK;
}


LOCAL gint32 set_property_lldpservice_enable(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar value_log[MAX_RSC_ID_LEN] = { 0 };
    OBJ_HANDLE obj_handle = 0;
    json_bool property_state;
    GSList *input_list = NULL;

    
    ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    return_val_if_expr(ret != VOS_OK, ret);

    
    property_state = json_object_get_boolean(i_paras->val_jso);

    ret = dal_get_object_handle_nth(CLASS_NAME_LLDP_CONFIG, 0, &obj_handle);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get_object_handle fail(%d).\n", __FUNCTION__, __LINE__, ret));

    
    input_list = g_slist_append(input_list, g_variant_new_byte((guchar)property_state));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_NAME_LLDP_CONFIG, METHOD_LLDP_CONFIG_LLDP_ENABLE, AUTH_ENABLE, i_paras->user_role_privilege, input_list,
        NULL);
    uip_free_gvariant_list(input_list);

    
    // sizeof(value_log)大小为64，大于格式化后字符串长度，无需判断返回值
    (void)snprintf_s(value_log, sizeof(value_log), sizeof(value_log) - 1, "%d", property_state);

    return check_redfish_call_method_result(ret, o_message_jso, RFPROP_MANAGER_LLDP_ENABLED, value_log);
}


LOCAL gint32 lldp_workmode_string_transduce_code(const gchar *mode_str, guchar *mode_code)
{
    if (strcmp(mode_str, RFVALUE_LLDP_WORKMODE_TX_STR) == 0) {
        *mode_code = RFVALUE_LLDP_WORKMODE_TX_CODE;
    } else if (strcmp(mode_str, RFVALUE_LLDP_WORKMODE_RX_STR) == 0) {
        *mode_code = RFVALUE_LLDP_WORKMODE_RX_CODE;
    } else if (strcmp(mode_str, RFVALUE_LLDP_WORKMODE_TX_RX_STR) == 0) {
        *mode_code = RFVALUE_LLDP_WORKMODE_TX_RX_CODE;
    } else if (strcmp(mode_str, RFVALUE_LLDP_WORKMODE_DISABLED_STR) == 0) {
        *mode_code = RFVALUE_LLDP_WORKMODE_DISABLED_CODE;
    } else {
        return VOS_ERR;
    }

    return VOS_OK;
}


LOCAL gint32 lldp_workmode_code_transduce_string(const guchar mode_code, gchar *mode_str, gint32 str_len)
{
    gint32 ret;

    if (mode_code == RFVALUE_LLDP_WORKMODE_TX_CODE) {
        ret = strcpy_s(mode_str, str_len, RFVALUE_LLDP_WORKMODE_TX_STR);
    } else if (mode_code == RFVALUE_LLDP_WORKMODE_RX_CODE) {
        ret = strcpy_s(mode_str, str_len, RFVALUE_LLDP_WORKMODE_RX_STR);
    } else if (mode_code == RFVALUE_LLDP_WORKMODE_TX_RX_CODE) {
        ret = strcpy_s(mode_str, str_len, RFVALUE_LLDP_WORKMODE_TX_RX_STR);
    } else if (mode_code == RFVALUE_LLDP_WORKMODE_DISABLED_CODE) {
        ret = strcpy_s(mode_str, str_len, RFVALUE_LLDP_WORKMODE_DISABLED_STR);
    } else {
        debug_log(DLOG_ERROR, "%s, %d: workmode code err %d.\n", __FUNCTION__, __LINE__, mode_code);
        return VOS_ERR;
    }

    return_val_do_info_if_expr(ret != 0, VOS_ERR, debug_log(DLOG_ERROR, "%s, %d: fail.\n", __FUNCTION__, __LINE__));

    return VOS_OK;
}


LOCAL gint32 get_property_lldpservice_workmode(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guchar property = 0;
    gchar mode_string[MAX_PROPERTY_VALUE_LEN] = { 0 };

    
    ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    return_val_if_expr(ret != VOS_OK, ret);

    
    ret = dal_get_object_handle_nth(CLASS_NAME_LLDP_CONFIG, 0, &obj_handle);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get_object_handle fail(%d).\n", __FUNCTION__, __LINE__, ret));

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_LLDP_CONFIG_WORKMODE, &property);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property_data fail(%d).\n", __FUNCTION__, __LINE__, ret));

    
    ret = lldp_workmode_code_transduce_string(property, mode_string, sizeof(mode_string));
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: transduce property_data fail.\n", __FUNCTION__, __LINE__));

    
    *o_result_jso = json_object_new_string(mode_string);
    return_val_do_info_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_string fail.\n", __FUNCTION__, __LINE__));
    return HTTP_OK;
}


LOCAL gint32 set_property_lldpservice_workmode(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guchar workmode_code = 0;
    const gchar *workmode_string = NULL;
    GSList *input_list = NULL;

    
    ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    return_val_if_expr(ret != VOS_OK, ret);

    workmode_string = dal_json_object_get_str(i_paras->val_jso);

    
    ret = lldp_workmode_string_transduce_code(workmode_string, &workmode_code);
    if (ret != VOS_OK) {
        (void)create_message_info(MSGID_PROP_ITEM_NOT_IN_LIST, RFPROP_MANAGER_LLDP_WORKMODE, o_message_jso,
            workmode_string, RFPROP_MANAGER_LLDP_WORKMODE);
        return HTTP_BAD_REQUEST;
    }

    ret = dal_get_object_handle_nth(CLASS_NAME_LLDP_CONFIG, 0, &obj_handle);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get_object_handle fail(%d).\n", __FUNCTION__, __LINE__, ret));

    
    input_list = g_slist_append(input_list, g_variant_new_byte((guchar)workmode_code));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_NAME_LLDP_CONFIG, METHOD_LLDP_CONFIG_WORKMODE, AUTH_ENABLE, i_paras->user_role_privilege, input_list,
        NULL);
    uip_free_gvariant_list(input_list);

    return check_redfish_call_method_result(ret, o_message_jso, RFPROP_MANAGER_LLDP_WORKMODE, workmode_string);
}


LOCAL gint32 get_property_lldpservice_tx_delay(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint16 property_value = 0;

    
    ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    return_val_if_expr(ret != VOS_OK, ret);

    ret = dal_get_object_handle_nth(CLASS_NAME_LLDP_CONFIG, 0, &obj_handle);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get_object_handle fail(%d).\n", __FUNCTION__, __LINE__, ret));

    
    ret = dal_get_property_value_uint16(obj_handle, PROPERTY_LLDP_CONFIG_TX_DELAY, &property_value);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property_data fail(%d).\n", __FUNCTION__, __LINE__, ret));

    
    *o_result_jso = json_object_new_int(property_value);
    return_val_do_info_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_int fail.\n", __FUNCTION__, __LINE__));
    return HTTP_OK;
}


LOCAL gint32 set_property_lldpservice_tx_delay(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return lldp_set_propery_uint16(METHOD_LLDP_CONFIG_TX_DELAY, i_paras, o_message_jso, o_result_jso);
}


LOCAL gint32 get_property_lldpservice_tx_interval(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint16 property_value = 0;

    
    ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    return_val_if_expr(ret != VOS_OK, ret);

    ret = dal_get_object_handle_nth(CLASS_NAME_LLDP_CONFIG, 0, &obj_handle);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get_object_handle fail(%d).\n", __FUNCTION__, __LINE__, ret));

    
    ret = dal_get_property_value_uint16(obj_handle, PROPERTY_LLDP_CONFIG_TX_INTERVAL, &property_value);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property_data fail(%d).\n", __FUNCTION__, __LINE__, ret));

    
    *o_result_jso = json_object_new_int(property_value);
    return_val_do_info_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_int fail.\n", __FUNCTION__, __LINE__));
    return HTTP_OK;
}


LOCAL gint32 set_property_lldpservice_tx_interval(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return lldp_set_propery_uint16(METHOD_LLDP_CONFIG_TX_INTERVAL, i_paras, o_message_jso, o_result_jso);
}


LOCAL gint32 get_property_lldpservice_tx_hold(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint16 property_value = 0;

    
    ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    return_val_if_expr(ret != VOS_OK, ret);

    ret = dal_get_object_handle_nth(CLASS_NAME_LLDP_CONFIG, 0, &obj_handle);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get_object_handle fail(%d).\n", __FUNCTION__, __LINE__, ret));

    
    ret = dal_get_property_value_uint16(obj_handle, PROPERTY_LLDP_CONFIG_TX_HOLD, &property_value);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property_data fail(%d).\n", __FUNCTION__, __LINE__, ret));

    
    *o_result_jso = json_object_new_int(property_value);
    return_val_do_info_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_int fail.\n", __FUNCTION__, __LINE__));
    return HTTP_OK;
}


LOCAL gint32 set_property_lldpservice_tx_hold(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return lldp_set_propery_uint16(METHOD_LLDP_CONFIG_TX_HOLD, i_paras, o_message_jso, o_result_jso);
}


LOCAL gint32 get_manager_lldpservice_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar slot[MAX_RSC_ID_LEN] = { 0 };
    gchar uri[MAX_URI_LENGTH] = { 0 };

    
    ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    return_val_if_expr(ret != VOS_OK, ret);

    
    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__));

    ret = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, LLDP_URI_ODATAID, slot);
    do_val_if_expr(ret <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret));
    *o_result_jso = json_object_new_string((const char *)uri);

    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s, json_object_new_string fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 is_support_lldp_service(void)
{
    gint32 ret;
    guchar support_flag = 0;

    
    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_LLDP, &support_flag);
    return_val_do_info_if_expr((ret != VOS_OK), HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s, %d: get LLDPSupport fail(%d).\n", __FUNCTION__, __LINE__, ret));
    return_val_if_expr((support_flag != SERVICE_NETCONFIG_ENABLE), HTTP_NOT_FOUND);

    return VOS_OK;
}

LOCAL PROPERTY_PROVIDER_S g_manager_lldp_provider[] = {
    { RFPROP_ODATA_ID,                 MAP_PROPERTY_NULL,      MAP_PROPERTY_NULL,                USERROLE_READONLY, SYS_LOCKDOWN_NULL,   get_manager_lldpservice_odata_id,     NULL,                                 NULL, ETAG_FLAG_ENABLE },
    { RFPROP_MANAGER_LLDP_ENABLED,     CLASS_NAME_LLDP_CONFIG, PROPERTY_LLDP_CONFIG_ENABLE,      USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_property_lldpservice_enable,      set_property_lldpservice_enable,      NULL, ETAG_FLAG_ENABLE },
    { RFPROP_MANAGER_LLDP_WORKMODE,    CLASS_NAME_LLDP_CONFIG, PROPERTY_LLDP_CONFIG_WORKMODE,    USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_property_lldpservice_workmode,    set_property_lldpservice_workmode,    NULL, ETAG_FLAG_ENABLE },
    { RFPROP_MANAGER_LLDP_TX_DELAY,    CLASS_NAME_LLDP_CONFIG, PROPERTY_LLDP_CONFIG_TX_DELAY,    USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_property_lldpservice_tx_delay,    set_property_lldpservice_tx_delay,    NULL, ETAG_FLAG_ENABLE },
    { RFPROP_MANAGER_LLDP_TX_INTERVAL, CLASS_NAME_LLDP_CONFIG, PROPERTY_LLDP_CONFIG_TX_INTERVAL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_property_lldpservice_tx_interval, set_property_lldpservice_tx_interval, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_MANAGER_LLDP_TX_HOLD,     CLASS_NAME_LLDP_CONFIG, PROPERTY_LLDP_CONFIG_TX_HOLD,     USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_property_lldpservice_tx_hold,     set_property_lldpservice_tx_hold,     NULL, ETAG_FLAG_ENABLE }
};


LOCAL gint32 lldp_set_propery_uint16(const gchar *set_method, PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint32 set_value;
    gchar value_log[MAX_RSC_ID_LEN] = { 0 };
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;

    
    if (set_method == NULL || o_result_jso == NULL || o_message_jso == NULL ||
        provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (json_object_get_type(i_paras->val_jso) != json_type_int) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, g_manager_lldp_provider[i_paras->index].property_name,
            o_message_jso, dal_json_object_get_str(i_paras->val_jso),
            g_manager_lldp_provider[i_paras->index].property_name);
        return HTTP_BAD_REQUEST;
    }

    
    set_value = json_object_get_int(i_paras->val_jso);

    
    ret = dal_get_object_handle_nth(g_manager_lldp_provider[i_paras->index].pme_class_name, 0, &obj_handle);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get_object_handle fail(%d).\n", __FUNCTION__, __LINE__, ret));

    input_list = g_slist_append(input_list, g_variant_new_uint16((guint16)set_value));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        g_manager_lldp_provider[i_paras->index].pme_class_name, set_method, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, NULL);

    uip_free_gvariant_list(input_list);

    
    // sizeof(value_log)大小为64，大于格式化后字符串长度，无需判断返回值
    (void)snprintf_s(value_log, sizeof(value_log), sizeof(value_log) - 1, "%d", set_value);

    return check_redfish_call_method_result(ret, o_message_jso, g_manager_lldp_provider[i_paras->index].property_name,
        value_log);
}


gint32 manager_lldpservice_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret;

    
    return_val_if_expr((i_paras == NULL) || (prop_provider == NULL) || (count == NULL), HTTP_INTERNAL_SERVER_ERROR);

    
    ret = is_support_lldp_service();
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s: cannot support lldp service ", __FUNCTION__));

    
    ret = redfish_check_manager_uri_valid(i_paras->uri);
    return_val_do_info_if_expr(ret != TRUE, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s: invalid uri is %s", __FUNCTION__, i_paras->uri));

    *prop_provider = g_manager_lldp_provider;
    *count = sizeof(g_manager_lldp_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}
