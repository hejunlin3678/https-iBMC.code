
#include "redfish_provider.h"

LOCAL PROPERTY_PROVIDER_S *get_self_provider_session(PROVIDER_PARAS_S *i_paras);


LOCAL gint32 check_session_validity(PROVIDER_PARAS_S *i_paras)
{
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    gint32 ret;

    input_list = g_slist_append(input_list, g_variant_new_string(i_paras->member_id));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, OBJ_HANDLE_NULL,
        CLASS_SESSION, METHOD_SESSION_GETSESSIONINFO, 0, i_paras->is_satisfy_privi, input_list, &output_list);
    

    uip_free_gvariant_list(input_list);

    return_val_do_info_if_fail(RFERR_NO_RESOURCE != ret && output_list, HTTP_NOT_FOUND,
        uip_free_gvariant_list(output_list));

    uip_free_gvariant_list(output_list);

    return HTTP_OK;
}

LOCAL gint32 get_session_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    int iRet;
    gchar *id_lower = NULL;

    gchar uri[MAX_URI_LENGTH] = {0};
    PROPERTY_PROVIDER_S *self = get_self_provider_session(i_paras);
    
    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != o_result_jso) && (NULL != self),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__));

    
    
    
    
    

    id_lower = g_ascii_strdown(i_paras->member_id, strlen(i_paras->member_id));
    return_val_if_expr(NULL == id_lower, HTTP_INTERNAL_SERVER_ERROR);
    iRet = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, "%s/%s", SESSIONSERVICE_SESSIONS, id_lower);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    g_free(id_lower);

    

    *o_result_jso = json_object_new_string(uri);
    return HTTP_OK;
}

LOCAL gint32 get_session_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gchar *id_lower = NULL;

    PROPERTY_PROVIDER_S *self = get_self_provider_session(i_paras);
    
    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != o_result_jso) && (NULL != self),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__));

    
    
    
    

    
    id_lower = g_ascii_strdown(i_paras->member_id, strlen(i_paras->member_id));
    return_val_if_expr(NULL == id_lower, HTTP_INTERNAL_SERVER_ERROR);
    *o_result_jso = json_object_new_string(id_lower);

    g_free(id_lower);

    

    return HTTP_OK;
}


LOCAL void get_session_actions(PROVIDER_PARAS_S *i_paras, json_object *huawei_json)
{
    int iRet;
    json_object *actions = NULL;
    json_object *single_action_jso = NULL;
    gchar taget_uri[MAX_URI_LENGTH] = {0};
    gchar actioninfo_uri[MAX_URI_LENGTH] = {0};
    gchar action_name_pointer[MAX_URI_LENGTH] = {0};

    return_if_expr(NULL == huawei_json);

    
    actions = json_object_new_object();
    return_do_info_if_expr(NULL == actions,
        debug_log(DLOG_ERROR, "%s, %d: new json object failed.", __FUNCTION__, __LINE__));

    single_action_jso = json_object_new_object();
    return_do_info_if_expr(NULL == single_action_jso, json_object_put(actions);
        debug_log(DLOG_ERROR, "%s, %d: new json object failed.", __FUNCTION__, __LINE__));
    // MAX_URI_LENGTH大小为512，大于格式化后字符串长度，无需判断返回值
    (void)snprintf_s(action_name_pointer, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, "#%s",
        RFACTION_SESSION_REFRESH_ACTIVATION_STATE);

    iRet = snprintf_s(taget_uri, sizeof(taget_uri), sizeof(taget_uri) - 1,
        RFACTION_SESSION_REFRESH_ACTIVATION_STATE_URI, i_paras->member_id, RFACTION_SESSION_REFRESH_ACTIVATION_STATE);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    iRet = snprintf_s(actioninfo_uri, sizeof(actioninfo_uri), sizeof(actioninfo_uri) - 1,
        RF_SESSION_REFRESH_ACTIVATION_STATE_ACTION_INFO, i_paras->member_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    json_object_object_add(single_action_jso, RFPROP_TARGET, json_object_new_string(taget_uri));

    json_object_object_add(single_action_jso, RFPROP_ACTION_INFO, json_object_new_string(actioninfo_uri));

    json_object_object_add(actions, action_name_pointer, single_action_jso);

    json_object_object_add(huawei_json, JSON_SCHEMA_ACTION, actions);
}


LOCAL void process_session_oem_ret_value(PROVIDER_PARAS_S *i_paras, gint32 http_code, json_object **o_message_jso)
{
    switch (http_code) {
        case HTTP_OK:
            break;
        case HTTP_NOT_FOUND:
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri);
            break;
        case HTTP_FORBIDDEN:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
            break;
        case HTTP_INTERNAL_SERVER_ERROR:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            break;
    }
}


LOCAL gint32 get_session_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    gint32 user_id = 0;
    guint8 initial_flag = 0;
    guint8 support_keepalive = 0;
    guint32 days_remain = 0;
    gchar strbuf[STRING_LEN_MAX] = "\0";
    const gchar *str = NULL;
    const gchar *session_checksum = NULL;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    struct json_object *jso_dict = NULL; 
    struct json_object *jso = NULL;
    gboolean is_self = FALSE;
    gint16 time_zone = 0; 
    guint32 time_now;
    gchar last_login_time_str[STRING_LEN_MAX] = {0};
    gint32 http_code = HTTP_OK;
    PROPERTY_PROVIDER_S *self = get_self_provider_session(i_paras);
    
    json_object *o_oem_huawei_jso = NULL;
    
    
    gchar *pstr = NULL;
    gchar *result = NULL;
    gint role_id = 0;
    gchar role_name[64] = {0};
    json_object *role_array = NULL;
    
    gint sscanf_cnt = 0;
    json_object *auth_jso = NULL;
    gint32 auth_type = 0;
    gint32 iRet;
    
    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != o_result_jso) && (NULL != self),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__));
    
    ret = dal_get_object_handle_nth(self->pme_class_name, 0, &obj_handle);

    goto_label_do_info_if_fail(VOS_OK == ret, __exit, http_code = HTTP_INTERNAL_SERVER_ERROR;
        debug_log(DLOG_ERROR, "%s: dal_get_object_handle_nth error", __FUNCTION__));

    input_list = g_slist_append(input_list, g_variant_new_string(i_paras->member_id));
    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        self->pme_class_name, METHOD_SESSION_GETSESSIONINFO, AUTH_DISABLE, i_paras->user_role_privilege, input_list,
        &output_list);
    uip_free_gvariant_list(input_list);
    
    goto_label_do_info_if_expr(RFERR_NO_RESOURCE == ret, __exit, { http_code = HTTP_NOT_FOUND; });

    
    goto_label_do_info_if_fail(VOS_OK == ret, __exit, { http_code = HTTP_INTERNAL_SERVER_ERROR; });

    
    str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    
    jso_dict = json_tokener_parse(str);
    uip_free_gvariant_list(output_list);
    
    if (json_object_object_get_ex(jso_dict, RF_SESSION_CHECKSUM, &jso)) {
        session_checksum = dal_json_object_get_str(jso);
        
        is_self = (0 == g_strcmp0(session_checksum, i_paras->session_id)) ? TRUE : FALSE;
    }

    
    goto_label_do_info_if_expr((!i_paras->is_satisfy_privi) && (TRUE != is_self), __exit,
                               { http_code = HTTP_FORBIDDEN; });

    
    o_oem_huawei_jso = json_object_new_object();
    
    goto_label_do_info_if_expr(NULL == o_oem_huawei_jso, __exit, { http_code = HTTP_INTERNAL_SERVER_ERROR; });
    

    if (json_object_object_get_ex(jso_dict, RF_SESSION_NAME, &jso)) {
        json_object_object_add(o_oem_huawei_jso, "UserAccount", json_object_new_string(dal_json_object_get_str(jso)));
    }

    if (json_object_object_get_ex(jso_dict, RF_SESSION_LOGIN_TIME, &jso)) {
        
        ret = redfish_get_format_time(i_paras, NULL, &time_zone);

        
        goto_label_do_info_if_expr(RFERR_INSUFFICIENT_PRIVILEGE == ret, __exit, {
            http_code = HTTP_FORBIDDEN;
        });

        goto_label_do_info_if_fail(VOS_OK == ret, __exit, http_code = HTTP_INTERNAL_SERVER_ERROR;
            debug_log(DLOG_ERROR, "%s: get timezone error", __FUNCTION__));

        gulong value = ULONG_MAX;
        if (dal_strtoul(dal_json_object_get_str(jso), &value, DECIMAL_NUM) != RET_OK) {
            debug_log(DLOG_ERROR, "%s:dal_strtoi failed", __FUNCTION__);
            http_code = HTTP_INTERNAL_SERVER_ERROR;
            goto __exit;
        }
        
        time_now = (guint32)value;
        
        // time_now += (time_zone * 60);

        ret = redfish_fill_format_time(strbuf, sizeof(strbuf), &time_now, &time_zone);

        goto_label_do_info_if_fail(VOS_OK == ret, __exit, http_code = HTTP_INTERNAL_SERVER_ERROR;
            debug_log(DLOG_ERROR, "%s: fill timezone error", __FUNCTION__));

        json_object_object_add(o_oem_huawei_jso, "LoginTime", json_object_new_string(strbuf));
    }

    
    if (i_paras->web_user_last_login_info.request_is_create) {
        if (i_paras->web_user_last_login_info.user_last_login_time &&
            INVALID_DATA_DWORD != i_paras->web_user_last_login_info.user_last_login_time) {
            ret = redfish_fill_format_time(last_login_time_str, sizeof(last_login_time_str),
                &(i_paras->web_user_last_login_info.user_last_login_time), &time_zone);
            do_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s: last  login time fill timezone error", __FUNCTION__));
            json_object_object_add(o_oem_huawei_jso, PROPERTY_USER_LAST_LOGIN_TIME,
                json_object_new_string(last_login_time_str));
        } else {
            json_object_object_add(o_oem_huawei_jso, PROPERTY_USER_LAST_LOGIN_TIME, NULL);
        }
        if (strlen(i_paras->web_user_last_login_info.user_last_login_ip) &&
            g_strcmp0((const gchar *)i_paras->web_user_last_login_info.user_last_login_ip, INVALID_DATA_STRING)) {
            json_object_object_add(o_oem_huawei_jso, PROPERTY_USER_LAST_LOGIN_IP,
                json_object_new_string(i_paras->web_user_last_login_info.user_last_login_ip));
        } else {
            json_object_object_add(o_oem_huawei_jso, PROPERTY_USER_LAST_LOGIN_IP, NULL);
        }
    }
    

    
    
    if (json_object_object_get_ex(jso_dict, RF_SESSION_USERID, &jso) &&
        json_object_object_get_ex(jso_dict, RF_SESSION_AUTH_TYPE, &auth_jso)) {
        user_id = json_object_get_int(jso);
        auth_type = json_object_get_int(auth_jso);
        if (LOG_TYPE_LOCAL == auth_type && user_id > 0) {
            
            json_object_object_add(o_oem_huawei_jso, PFPROP_USER_ID, json_object_new_int(user_id));
            (void)dal_get_specific_object_byte(CLASS_USER, PROPERTY_USER_ID, (guchar)user_id, &obj_handle);
            ret = dal_get_property_value_uint32(obj_handle, PROPERTY_USER_PWDVALIDDAYS, &days_remain);
            if ((ret == VOS_OK) && (0xffff != days_remain)) {
                json_object_object_add(o_oem_huawei_jso, PFPROP_USER_VALID_DAYS, json_object_new_int(days_remain));
            } else {
                json_object_object_add(o_oem_huawei_jso, PFPROP_USER_VALID_DAYS, NULL);
            }

            (void)dal_get_property_value_byte(obj_handle, PROPERTY_USER_INITIALSTATE, &initial_flag);
            json_object_object_add(o_oem_huawei_jso, PFPROP_ACCOUNT_INSECURE_PROMPT_ENABLED,
                json_object_new_boolean(initial_flag));
        } else {
            json_object_object_add(o_oem_huawei_jso, PFPROP_USER_ID, NULL);
            json_object_object_add(o_oem_huawei_jso, PFPROP_USER_VALID_DAYS, NULL);
            json_object_object_add(o_oem_huawei_jso, PFPROP_ACCOUNT_INSECURE_PROMPT_ENABLED, NULL);
        }
    }
    

    if (json_object_object_get_ex(jso_dict, RF_SESSION_IP, &jso)) {
        json_object_object_add(o_oem_huawei_jso, "UserIP", json_object_new_string(dal_json_object_get_str(jso)));
    }

    if (json_object_object_get_ex(jso_dict, RF_SESSION_TYPE, &jso)) {
        json_object_object_add(o_oem_huawei_jso, "UserTag", json_object_new_string(dal_json_object_get_str(jso)));
        
        if (strcmp(dal_json_object_get_str(jso), USER_LOGIN_INTERFACE_GUI_STRING) == 0 ||
            strcmp(dal_json_object_get_str(jso), USER_LOGIN_INTERFACE_GUI_SSO_STRING) == 0) {
            support_keepalive = 1;
        }
        
    }

    json_object_object_add(o_oem_huawei_jso, "MySession", json_object_new_boolean(is_self));
    
    if (json_object_object_get_ex(jso_dict, RF_SESSION_ROLEID, &jso)) {
        pstr = strbuf;
        role_array = json_object_new_array();
        iRet = strncpy_s(strbuf, sizeof(strbuf), dal_json_object_get_str(jso), strlen(dal_json_object_get_str(jso)));
        do_val_if_expr(iRet != EOK, debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, iRet));

        while (NULL != (result = strtok_s(pstr, ",", &pstr)) && role_array != NULL) {
            role_id = 0;
            sscanf_cnt = sscanf_s(result, "%d", &role_id);
            do_if_expr(1 != sscanf_cnt, debug_log(DLOG_ERROR, "sscanf_s failed. sscanf_cnt = %d\n", sscanf_cnt));
            (void)dal_get_specific_object_byte(CLASS_USERROLE, PROPERTY_USERROLE_ID, (guchar)role_id, &obj_handle);
            ret = dal_get_property_value_string(obj_handle, PROPERTY_USERROLE_ROLENAME, role_name, sizeof(role_name));
            if (ret == VOS_OK) {
                json_object_array_add(role_array, json_object_new_string(role_name));
            }
        }

        json_object_object_add(o_oem_huawei_jso, "UserRole", role_array);
    }

    

    do_info_if_true(support_keepalive, get_session_actions(i_paras, o_oem_huawei_jso));

    *o_result_jso = json_object_new_object();
    if (*o_result_jso == NULL) {
        http_code = HTTP_INTERNAL_SERVER_ERROR;
    } else {
        json_object_object_add(*o_result_jso, RFPROP_COMMON_HUAWEI, o_oem_huawei_jso);
    }
    

__exit:

    process_session_oem_ret_value(i_paras, http_code, o_message_jso);
    
    do_info_if_true(NULL != jso_dict, json_object_put(jso_dict));
    
    do_info_if_true(HTTP_OK != http_code, {
        json_object_put(o_oem_huawei_jso);
        o_oem_huawei_jso = NULL;
    });
    return http_code;
}


LOCAL gint32 action_session_refresh_activation_state(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint32 alive_time = 0;
    json_bool is_ok;
    const gchar *mode = NULL;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_message_jso) && (NULL != o_result_jso),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    
    if (strcmp(i_paras->member_id, i_paras->session_id) != 0) {
        (void)create_message_info(MSGID_UNSUPPORT_TO_REFRESH_ANOTHER_SESSION, NULL, o_message_jso);
        debug_log(DLOG_DEBUG, "%s sessionid not match", __FUNCTION__);
        return HTTP_BAD_REQUEST;
    }

    
    if (!i_paras->is_from_webui) {
        (void)create_message_info(MSGID_ACT_NOT_SUPPORTED, NULL, o_message_jso,
            RFACTION_SESSION_REFRESH_ACTIVATION_STATE);
        return HTTP_BAD_REQUEST;
    }

    is_ok = get_element_str(i_paras->val_jso, RFPROP_SESSION_ACTIVATE_MODE, &mode);
    return_val_do_info_if_expr(!is_ok, HTTP_BAD_REQUEST, debug_log(DLOG_DEBUG, "%s get element failed", __FUNCTION__);
        (void)create_message_info(MSGID_ACT_PARA_MISSING, RFPROP_SESSION_ACTIVATE_MODE, o_message_jso,
        RFACTION_SESSION_REFRESH_ACTIVATION_STATE, RFPROP_SESSION_ACTIVATE_MODE));

    if (strcmp(mode, SESSION_DEACTIVATE) == 0) {
        // 会话延迟10秒失效
        alive_time = 10;
    } else {
        // 以最大会话超时时长激活会话
        (void)dal_get_object_handle_nth(CLASS_SESSION, 0, &obj_handle);
        ret = dal_get_property_value_int32(obj_handle, PROPERTY_SESSION_TIMEOUT, &alive_time);
        return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_DEBUG, "%s get session timeout failed", __FUNCTION__);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    }

    input_list = g_slist_append(input_list, g_variant_new_string(i_paras->member_id));
    input_list = g_slist_append(input_list, g_variant_new_int32(alive_time));

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_SESSION, METHOD_SESSION_SETSESSIONBEAT, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, &output_list);
    

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    if (ret == USER_UNSUPPORT) {
        (void)create_message_info(MSGID_ACT_PARA_NOT_SUPPORTED, NULL, o_message_jso, RFPROP_SESSION_ACTIVATE_MODE,
            RFACTION_SESSION_REFRESH_ACTIVATION_STATE);
        return HTTP_BAD_REQUEST;
    } else if (ret == USER_COMMAND_NORMALLY) {
        return HTTP_OK;
    } else {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL PROPERTY_PROVIDER_S session_provider[] = {
    {RFPROP_ODATA_ID, CLASS_SESSION, MAP_PROPERTY_NULL, USERROLE_USERMGNT, SYS_LOCKDOWN_NULL, get_session_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_ID, CLASS_SESSION, MAP_PROPERTY_NULL, USERROLE_USERMGNT, SYS_LOCKDOWN_NULL, get_session_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_OEM, CLASS_SESSION, MAP_PROPERTY_NULL, USERROLE_USERMGNT, SYS_LOCKDOWN_NULL, get_session_oem, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFACTION_SESSION_REFRESH_ACTIVATION_STATE, CLASS_SESSION, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, NULL, NULL, action_session_refresh_activation_state, ETAG_FLAG_ENABLE}
};

LOCAL PROPERTY_PROVIDER_S *get_self_provider_session(PROVIDER_PARAS_S *i_paras)
{
    return_val_if_fail((NULL != i_paras) && (i_paras->index >= 0) &&
        (i_paras->index < (gint32)(sizeof(session_provider) / sizeof(PROPERTY_PROVIDER_S))),
        (PROPERTY_PROVIDER_S *)NULL);
    return &session_provider[i_paras->index];
}


gint32 session_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gint32 ret;
    
    return_val_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count), HTTP_INTERNAL_SERVER_ERROR);
    
    ret = check_session_validity(i_paras);
    return_val_if_fail(HTTP_OK == ret, ret);

    *prop_provider = session_provider;
    *count = sizeof(session_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}


LOCAL gint32 get_session_actioninfo_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;
    gchar session_uri[MAX_URI_LENGTH] = {0};

    iRet = snprintf_s(session_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, RF_SESSION_REFRESH_ACTIVATION_STATE_ACTION_INFO,
        i_paras->member_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    *o_result_jso = json_object_new_string(session_uri);

    return HTTP_OK;
}

LOCAL PROPERTY_PROVIDER_S g_session_action_info_provider[] = {
    {
        RFPROP_ODATA_ID,
        "", "",
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_session_actioninfo_odataid,
        NULL,
        NULL,
        ETAG_FLAG_ENABLE
    }
};


gint32 session_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    return_val_do_info_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    *prop_provider = g_session_action_info_provider;
    *count = sizeof(g_session_action_info_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}
