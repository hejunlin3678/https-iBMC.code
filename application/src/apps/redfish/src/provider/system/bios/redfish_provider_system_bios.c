
#include "redfish_provider.h"
#include "redfish_provider_system.h"

LOCAL gint32 get_system_bios_odatacontext(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_bios_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_bios_attributes(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_bios_settings(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_bios_actions(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 act_system_bios_reset(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_system_bios_registryversion(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __act_system_bios_change_password(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso);
LOCAL gint32 get_system_bios_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_sytem_bios_provider[] = {
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_bios_odatacontext, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_bios_odataid, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ATTRIBUTES, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_bios_attributes, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SETTINGS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_bios_settings, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ACTIONS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_bios_actions, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SYSTEM_BIOS_ACTION_RESET, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, NULL, NULL, act_system_bios_reset, ETAG_FLAG_ENABLE},
    {RF_BIOS_REG_VERSION, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_bios_registryversion, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SYSTEM_BIOS_ACTION_CHANGE_PASSWROD, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID, NULL, NULL, __act_system_bios_change_password, ETAG_FLAG_ENABLE},
    {RFPROP_OEM, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY,
        SYS_LOCKDOWN_NULL, get_system_bios_oem, NULL, NULL, ETAG_FLAG_ENABLE}
};

LOCAL gint32 get_bios_reset_actioninfo_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL PROPERTY_PROVIDER_S g_bios_reset_actioninfo_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_bios_reset_actioninfo_odataid, NULL, NULL, ETAG_FLAG_ENABLE}
};

LOCAL const gchar *g_bios_action_array[] = {
    RFPROP_SYSTEM_BIOS_ACTION_RESET,
    RFPROP_SYSTEM_BIOS_ACTION_CHANGE_PASSWROD
};


LOCAL gint32 get_system_bios_registryversion(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar str_value[MAX_RSC_ID_LEN] = {0};
    gint32 ret;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = rf_bios_get_registryversion(str_value, MAX_RSC_ID_LEN, RF_REGISTRY_VERSION_DOT);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "rf_bios_get_registryversion fail");

        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const gchar*)str_value);
    return_val_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_system_bios_odatacontext(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar slot_str[MAX_RSC_ID_LEN] = {0};
    gchar string_value[MAX_RSC_ID_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    // 获取 @odata.context
    ret = redfish_get_board_slot(slot_str, sizeof(slot_str) - 1);
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1, RF_BIOS_ODATA_CONTEXT, slot_str);
    return_val_if_expr(ret < 0, HTTP_INTERNAL_SERVER_ERROR);

    *o_result_jso = json_object_new_string((const gchar*)string_value);
    return_val_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_system_bios_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar string_value[MAX_RSC_ID_LEN] = {0};
    gchar slot_str[MAX_RSC_ID_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    // 获取 @odata.id
    ret = redfish_get_board_slot(slot_str, sizeof(slot_str) - 1);
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1, RF_BIOS_ODATA_ID, slot_str);
    return_val_if_expr(ret < 0, HTTP_INTERNAL_SERVER_ERROR);

    *o_result_jso = json_object_new_string((const gchar*)string_value);
    return_val_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_system_bios_attributes(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    const gchar *str = NULL;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    gint32 ret;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dfl_get_object_handle(OBJ_NAME_BIOS, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get obj_handle for OBJ_NAME_BIOS fail.", __FUNCTION__));

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_string(BIOS_JSON_FILE_CURRENT_VALUE_NAME));

    ret = dfl_call_class_method(obj_handle, METHOD_BIOS_GET_JSON_FILE, NULL, input_list, &output_list);

    
    uip_free_gvariant_list(input_list);

    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s: METHOD_BIOS_GET_JSON_FILE fail", __FUNCTION__);
        uip_free_gvariant_list(output_list));

    str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    return_val_do_info_if_fail((NULL != str) && (strlen(str) != 0), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: g_variant_get_string fail", __FUNCTION__);
        uip_free_gvariant_list(output_list));

    *o_result_jso = json_tokener_parse(str);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_tokener_parse fail", __FUNCTION__);
        uip_free_gvariant_list(output_list));
    uip_free_gvariant_list(output_list);

    return HTTP_OK;
}


LOCAL void add_settings_etag(json_object *jso_settings)
{
    const gchar *str = NULL;

    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;

    gchar *check_sum = NULL;
    gchar etag_tmp[ETAG_LEN - 4] = {0};

    
    gint32 ret = dfl_get_object_handle(OBJ_NAME_BIOS, &obj_handle);
    goto_label_do_info_if_fail(VOS_OK == ret, err_out,
        debug_log(DLOG_ERROR, "%s: get obj_handle for OBJ_NAME_BIOS fail.", __FUNCTION__));

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_string(BIOS_JSON_FILE_CURRENT_VALUE_NAME));

    ret = dfl_call_class_method(obj_handle, METHOD_BIOS_GET_JSON_FILE, NULL, input_list, &output_list);

    
    uip_free_gvariant_list(input_list);

    goto_label_do_info_if_fail(VOS_OK == ret, err_out,
        debug_log(DLOG_DEBUG, "%s: METHOD_BIOS_GET_JSON_FILE fail", __FUNCTION__));

    str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    goto_label_do_info_if_fail((NULL != str) && (0 != strlen(str)), err_out,
        debug_log(DLOG_ERROR, "%s: g_variant_get_string fail", __FUNCTION__));

    // 以资源对象字符串计算校验和sha256加密值
    check_sum = g_compute_checksum_for_string(G_CHECKSUM_SHA256, str, strlen(str));
    goto_label_do_info_if_fail(NULL != check_sum, err_out,
        debug_log(DLOG_ERROR, "%s: g_compute_checksum_for_string fail", __FUNCTION__));

    // 取check_sum前8个字节
    errno_t safe_fun_ret = strncpy_s(etag_tmp, ETAG_LEN - 4, check_sum, ETAG_LEN - 5);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

    json_object_object_add(jso_settings, "ETag", json_object_new_string((const gchar*)etag_tmp));

    g_free(check_sum);
    check_sum = NULL;
    uip_free_gvariant_list(output_list);
    output_list = NULL;

    return;

err_out:
    do_val_if_expr(NULL != output_list, uip_free_gvariant_list(output_list));
    do_val_if_expr(NULL != check_sum, g_free(check_sum));

    json_object_object_add(jso_settings, "ETag", NULL);
}


LOCAL void add_settings_messages_and_time(json_object *jso_settings)
{
    const gchar *str = NULL;
    gint32 ret;

    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;

    json_object *jso_result = NULL;
    json_object *jso_messages = NULL;
    json_object *jso_time = NULL;
    json_bool bool_messages = FALSE;
    json_bool bool_time = FALSE;

    
    ret = dfl_get_object_handle(OBJ_NAME_BIOS, &obj_handle);
    goto_label_do_info_if_fail(VOS_OK == ret, err_out,
        debug_log(DLOG_ERROR, "%s: get obj_handle for OBJ_NAME_BIOS fail.", __FUNCTION__));

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_string(BIOS_JSON_FILE_RESULT_NAME));

    ret = dfl_call_class_method(obj_handle, METHOD_BIOS_GET_JSON_FILE, NULL, input_list, &output_list);

    
    uip_free_gvariant_list(input_list);

    goto_label_do_info_if_fail(VOS_OK == ret, err_out,
        debug_log(DLOG_DEBUG, "%s: METHOD_BIOS_GET_JSON_FILE fail", __FUNCTION__));

    str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    goto_label_do_info_if_fail((NULL != str) && (0 != strlen(str)), err_out,
        debug_log(DLOG_ERROR, "%s: g_variant_get_string fail", __FUNCTION__));

    
    jso_result = json_tokener_parse(str);
    goto_label_do_info_if_fail(NULL != jso_result, err_out,
        debug_log(DLOG_ERROR, "%s: json_tokener_parse failed", __FUNCTION__));

    
    bool_messages = json_object_object_get_ex(jso_result, "Messages", &jso_messages);
    if (bool_messages) {
        if (jso_messages == NULL) {
            json_object_object_add(jso_settings, "Messages", json_object_new_array());
        } else {
            json_object_object_add(jso_settings, "Messages", json_object_get(jso_messages));
        }
    } else {
        json_object_object_add(jso_settings, "Messages", json_object_new_array());
        debug_log(DLOG_ERROR, "%s: get Messages property value failed", __FUNCTION__);
    }

    
    bool_time = json_object_object_get_ex(jso_result, "Time", &jso_time);
    if (bool_time) {
        json_object_object_add(jso_settings, "Time", json_object_get(jso_time));
    } else {
        debug_log(DLOG_ERROR, "%s: get Time property value failed", __FUNCTION__);
    }

err_out:
    do_val_if_expr(NULL != output_list, uip_free_gvariant_list(output_list));
    do_val_if_expr(NULL != jso_result, json_object_put(jso_result));

    do_val_if_expr(!bool_messages, json_object_object_add(jso_settings, "Messages", json_object_new_array()));
    do_val_if_expr(!bool_time, json_object_object_add(jso_settings, "Time", NULL));
}


LOCAL void add_settings_settingsobject(json_object *jso_settings)
{
    gint32 ret;
    gchar string_value[MAX_RSC_ID_LEN] = {0};
    gchar slot_str[MAX_RSC_ID_LEN] = {0};

    json_object *jso = NULL;

    jso = json_object_new_object();
    json_object_object_add(jso_settings, "SettingsObject", jso);
    return_do_info_if_fail(NULL != jso, debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__));

    ret = redfish_get_board_slot(slot_str, sizeof(slot_str) - 1);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s: redfish_get_board_slot failed", __FUNCTION__);
        json_object_object_add(jso, RFPROP_ODATA_ID, NULL));

    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1, RF_BIOS_SETTINGS_ODATA_ID, slot_str);
    return_do_info_if_fail(ret > 0, debug_log(DLOG_ERROR, "%s: snprintf_s failed", __FUNCTION__);
        json_object_object_add(jso, RFPROP_ODATA_ID, NULL));

    json_object_object_add(jso, RFPROP_ODATA_ID, json_object_new_string((const gchar*)string_value));
}


LOCAL gint32 get_system_bios_settings(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_object fail.", __FUNCTION__));

    
    json_object_object_add(*o_result_jso, "@odata.type", json_object_new_string("#Settings.v1_0_2.Settings"));

    
    add_settings_etag(*o_result_jso);

    
    add_settings_messages_and_time(*o_result_jso);

    
    add_settings_settingsobject(*o_result_jso);

    return HTTP_OK;
}


LOCAL gint32 get_system_bios_actions(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gchar slot_str[MAX_RSC_ID_LEN] = {0};
    gchar system_uri[MAX_URI_LENGTH] = {0};
    json_object* jso = NULL;
    gint32 ret;
    guint32 i;

    // 入参检查,检查不过返回500，内部错误，不填充消息体
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (i_paras->is_satisfy_privi == 0) {
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = redfish_get_board_slot(slot_str, sizeof(slot_str) - 1);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: redfish_get_board_slot failed", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    // 创建action连接
    ret = snprintf_s(system_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, RF_BIOS_ODATA_ID, slot_str);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "format string failed, ret is %d", ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    jso = json_object_new_object();
    if (jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    for (i = 0; i < G_N_ELEMENTS(g_bios_action_array); i++) {
        rf_add_action_prop(jso, (const gchar*)system_uri, g_bios_action_array[i]);
    }

    *o_result_jso = jso;

    return HTTP_OK;
}


LOCAL gint32 act_system_bios_reset(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != i_paras) && (NULL != i_paras->val_jso),
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dfl_get_object_handle(OBJ_NAME_BIOS, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get obj_handle for OBJ_NAME_BIOS fail.", __FUNCTION__));

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_NAME_BIOS, METHOD_BIOS_RESET, AUTH_ENABLE, i_paras->user_role_privilege, NULL, NULL);
    switch (ret) {
        case VOS_OK:
            ret = HTTP_OK;
            break;

            
        case RFERR_NO_RESOURCE:
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri);
            ret = HTTP_BAD_REQUEST;
            break;

            
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
            ret = HTTP_BAD_REQUEST;
            break;

        default: 
            debug_log(DLOG_ERROR, "%s: ret = %d", __FUNCTION__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            ret = HTTP_INTERNAL_SERVER_ERROR;
            break;
    }

    return ret;
    
}


LOCAL gint32 get_bios_reset_actioninfo_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar string_value[MAX_RSC_ID_LEN] = {0};
    gchar slot_str[MAX_RSC_ID_LEN] = {0};
    gint32 ret;

    
    if (NULL == o_message_jso || NULL == o_result_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    // 获取 @odata.id
    ret = redfish_get_board_slot(slot_str, sizeof(slot_str) - 1);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: redfish_get_board_slot fail.", __FUNCTION__));

    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1, RF_BIOS_ACTION_RESET_ACTION_INFO,
        slot_str);
    return_val_do_info_if_fail(ret > 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: snprintf_s fail.", __FUNCTION__));

    *o_result_jso = json_object_new_string((const gchar*)string_value);
    return_val_do_info_if_fail(*o_result_jso != NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_string fail.", __FUNCTION__));

    return HTTP_OK;
}


gint32 system_bios_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gboolean ret;
    gint32 ret_int;
    guchar board_type = 0;
    guint32 product_ver = 0;
    guchar arm_enable = 0;

    ret = redfish_check_system_uri_valid(i_paras->uri);
    if (!ret) {
        return HTTP_NOT_FOUND;
    }

    
    ret_int = redfish_get_x86_enable_type(&board_type);
    return_val_do_info_if_fail(VOS_OK == ret_int, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get board_type fail", __FUNCTION__));
    return_val_if_fail(DISABLE != board_type, HTTP_NOT_FOUND);

    ret_int = dal_get_product_version_num(&product_ver);
    return_val_do_info_if_fail(VOS_OK == ret_int, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: dal_get_product_version_num fail", __FUNCTION__));

    
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_ARM, &arm_enable);
    // arm产品支持通过redfish配置bios setup菜单
    return_val_if_expr(((DISABLE == arm_enable) && (PRODUCT_VERSION_V5 > product_ver)), HTTP_NOT_FOUND);
    

    *prop_provider = g_sytem_bios_provider;
    *count = sizeof(g_sytem_bios_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}


gint32 bios_reset_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gboolean ret;
    gint32 ret_int;
    guchar board_type = 0;
    guint32 product_ver = 0;

    ret = redfish_check_system_uri_valid(i_paras->uri);
    if (!ret) {
        return HTTP_NOT_FOUND;
    }

    
    ret_int = redfish_get_x86_enable_type(&board_type);
    return_val_do_info_if_fail(VOS_OK == ret_int, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get board_type fail", __FUNCTION__));
    return_val_if_fail(DISABLE != board_type, HTTP_NOT_FOUND);

    ret_int = dal_get_product_version_num(&product_ver);
    return_val_do_info_if_fail(VOS_OK == ret_int, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: dal_get_product_version_num fail", __FUNCTION__));
    return_val_if_fail(product_ver > PRODUCT_VERSION_V3, HTTP_NOT_FOUND);

    *prop_provider = g_bios_reset_actioninfo_provider;
    *count = sizeof(g_bios_reset_actioninfo_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}

LOCAL gint32 __parse_change_password_method_retcode(json_object** o_message_jso, gint32 ret_code, const gchar *pwd_name)
{
    if (ret_code == RET_OK) {
        return HTTP_OK;
    }

    switch (ret_code) {
        case RFERR_INSUFFICIENT_PRIVILEGE:
            create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
            return HTTP_FORBIDDEN;

        case METHOD_BIOS_CHANGE_PWD_UNSUPPORTED:
            (void)create_message_info(MSGID_PROP_INVALID_VALUE, RF_BIOS_PASSWORD_NAME, o_message_jso, pwd_name,
                RF_BIOS_PASSWORD_NAME);
            return HTTP_BAD_REQUEST;

        case METHOD_BIOS_CHANGE_PWD_LEN_INVALID:
            (void)create_message_info(MSGID_INVALID_PSWD_LEN, NULL, o_message_jso,
                BIOS_PASSWORD_MIN_LEN_STR, BIOS_PASSWORD_MAX_LEN_STR);
            return HTTP_BAD_REQUEST;

        default:
            (void)create_message_info(MSGID_INVALID_PAWD, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
    }
}

LOCAL void __bios_change_pwd_clear_sensitive_info(json_object* jso)
{
    json_object_clear_string(jso, RF_BIOS_OLD_PASSWORD);
    json_object_clear_string(jso, RF_BIOS_NEW_PASSWORD);
}

LOCAL gint32 __act_system_bios_change_password(PROVIDER_PARAS_S* paras,
    json_object** o_message_jso, json_object** o_result_jso)
{
    json_bool b_ret;
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GSList* input_list = NULL;
    const gchar *pwd_name = NULL;
    const gchar *pwd_str = NULL;
    guint8 i;
    const gchar *pwd_rf_name [] = {
        RF_BIOS_OLD_PASSWORD,
        RF_BIOS_NEW_PASSWORD
    };

    if (o_message_jso == NULL || paras == NULL || paras->val_jso == NULL) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dfl_get_object_handle(OBJ_NAME_BIOS, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get obj_handle for OBJ_NAME_BIOS failed[%d].", __FUNCTION__, ret);
        __bios_change_pwd_clear_sensitive_info(paras->val_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    b_ret = get_element_str(paras->val_jso, RF_BIOS_PASSWORD_NAME, &pwd_name);
    if (b_ret != TRUE) {
        debug_log(DLOG_ERROR, "%s: get %s failed.", __FUNCTION__, RF_BIOS_PASSWORD_NAME);
        ret = METHOD_BIOS_CHANGE_PWD_UNSUPPORTED;
        goto OUT;
    }
    input_list = g_slist_append(input_list, g_variant_new_string(pwd_name));

    for (i = 0; i < ARRAY_SIZE(pwd_rf_name); i++) {
        b_ret = get_element_str(paras->val_jso, pwd_rf_name[i], &pwd_str);
        if (b_ret != TRUE) {
            debug_log(DLOG_ERROR, "%s: get %s failed.", __FUNCTION__, pwd_rf_name[i]);
            ret = METHOD_BIOS_CHANGE_PWD_LEN_INVALID;
            goto OUT;
        }
        input_list = g_slist_append(input_list, g_variant_new_string(pwd_str));
    }

    ret = uip_call_class_method_redfish(paras->is_from_webui, paras->user_name, paras->client, obj_handle,
        BIOS_CLASS_NAME, METHOD_BIOS_CHANGE_PWD, AUTH_ENABLE, paras->user_role_privilege, input_list, NULL);

OUT:
    uip_free_gvariant_list(input_list);
    __bios_change_pwd_clear_sensitive_info(paras->val_jso);
    return __parse_change_password_method_retcode(o_message_jso, ret, pwd_name);
}


LOCAL gint32 add_system_bios_oem_huawei_odata_id(json_object *huawei_jso, const gchar *slot, const gchar *oem_resource)
{
    gint32 ret;
    gchar oem_odata_id[MAX_URI_LENGTH] = {0};

    ret = sprintf_s(oem_odata_id, sizeof(oem_odata_id), RF_SYSTEM_BIOS_OEM_HUAWEI_ODATA_ID, slot, oem_resource);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: sprintf_s fail, ret: %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    json_object *odata_id_jso = json_object_new_object();
    if (odata_id_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__);
        return RET_ERR;
    }

    json_object_object_add(huawei_jso, oem_resource, odata_id_jso);
    json_object_object_add(odata_id_jso, ODATA_ID, json_object_new_string(oem_odata_id));

    return RET_OK;
}


LOCAL gint32 get_system_bios_oem_huawei(json_object *huawei_jso)
{
    gint32 ret;
    gchar slot[MAX_RSC_ID_LEN] = {0};

    ret = redfish_get_board_slot(slot, sizeof(slot));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: redfish_get_board_slot failed", __FUNCTION__);
        return RET_ERR;
    }

    ret = add_system_bios_oem_huawei_odata_id(huawei_jso, slot, RFPROP_BOOT_CERTIFICATES);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: add_system_bios_oem_huawei_odata_id failed", __FUNCTION__);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 get_system_bios_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    if (o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: invalid param", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (i_paras->is_satisfy_privi == 0) {
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    json_object *huawei_jso = json_object_new_object();
    if (huawei_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = get_system_bios_oem_huawei(huawei_jso);
    if (ret != RET_OK) {
        json_object_put(huawei_jso);
        debug_log(DLOG_ERROR, "%s: get_system_bios_oem_huawei failed", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    json_object *oem = json_object_new_object();
    if (oem == NULL) {
        json_object_put(huawei_jso);
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    json_object_object_add(oem, RFPROP_OEM_HUAWEI, huawei_jso);
    *o_result_jso = oem;

    return HTTP_OK;
}