
#include "redfish_provider.h"


LOCAL gint32 get_vncproperty_value_gint32(const gchar *class_name, const gchar *property_name, gint32 *out_val)
{
    gint32 ret;
    OBJ_HANDLE vnc_handle = 0;
    ret = dal_get_object_handle_nth(class_name, 0, &vnc_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s,%d: dal_get_object_handle_nth  fail. ", __FUNCTION__, __LINE__));
    ret = dal_get_property_value_int32(vnc_handle, property_name, out_val);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s,%d: dal_get_property_value_int32 fail. ", __FUNCTION__, __LINE__));
    return VOS_OK;
}

LOCAL gint32 get_vncproperty_value_guint32(const gchar *class_name, const gchar *property_name, guint32 *out_val)
{
    gint32 ret;
    OBJ_HANDLE vnc_handle = 0;
    ret = dal_get_object_handle_nth(class_name, 0, &vnc_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s,%d: dal_get_object_handle_nth  fail. ", __FUNCTION__, __LINE__));
    ret = dal_get_property_value_uint32(vnc_handle, property_name, out_val);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s,%d: dal_get_property_value_uint32 fail. ", __FUNCTION__, __LINE__));
    return VOS_OK;
}

LOCAL gint32 get_vncproperty_value_byte(const gchar *class_name, const gchar *property_name, guchar *out_val)
{
    gint32 ret;
    OBJ_HANDLE vnc_handle = 0;
    ret = dal_get_object_handle_nth(class_name, 0, &vnc_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s,%d: dal_get_object_handle_nth  fail. ", __FUNCTION__, __LINE__));
    ret = dal_get_property_value_byte(vnc_handle, property_name, out_val);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s,%d: dal_get_property_value_byte fail. ", __FUNCTION__, __LINE__));
    return VOS_OK;
}

LOCAL gint32 get_vncproperty_value_string(const gchar *class_name, const gchar *property_name, gchar *out_val,
    guint32 out_len)
{
    gint32 ret;
    OBJ_HANDLE vnc_handle = 0;
    ret = dal_get_object_handle_nth(class_name, 0, &vnc_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s,%d: dal_get_object_handle_nth  fail. ", __FUNCTION__, __LINE__));
    ret = dal_get_property_value_string(vnc_handle, property_name, out_val, out_len);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s,%d: dal_get_property_value_string fail. ", __FUNCTION__, __LINE__));
    return VOS_OK;
}

LOCAL gint32 get_sslencryption(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    guchar sslencryption = 0;

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    ret = get_vncproperty_value_byte(VNC_CLASS_NAME, PROPERTY_VNC_SSL_STATE, &sslencryption);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: get_vncproperty_value_byte fail. ", __FUNCTION__, __LINE__));
    (1 == sslencryption) ? (*o_result_jso = json_object_new_boolean(TRUE)) :
                           (*o_result_jso = json_object_new_boolean(FALSE));
    return HTTP_OK;
}

LOCAL gint32 set_sslencryption(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    json_bool vnc_bool;
    guchar s_encryption;
    GSList *input_list = NULL;
    OBJ_HANDLE vnc_handle = 0;

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_KVMMGNT, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, PROPERTY_MANAGER_SSLENCRYPTION, o_message_jso,
        PROPERTY_MANAGER_SSLENCRYPTION));

    vnc_bool = json_object_get_boolean(i_paras->val_jso);
    (TRUE == vnc_bool) ? (s_encryption = 1) : (s_encryption = 0);

    ret = dal_get_object_handle_nth(VNC_CLASS_NAME, 0, &vnc_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s,%d: dal_get_object_handle_nth  fail. ", __FUNCTION__, __LINE__));

    input_list = g_slist_append(input_list, g_variant_new_byte(s_encryption));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, vnc_handle,
        VNC_CLASS_NAME, METHOD_VNC_SET_SSL_STATE, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
            return HTTP_OK;
        case VNC_IN_USE:
            (void)create_message_info(MANAGER_VNC_MESSAGE_VNCUSE, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        default:
            debug_log(DLOG_ERROR, "%s: call methed %s return %d", __FUNCTION__, METHOD_VNC_SET_SSL_STATE, ret);
            
            if (dal_is_rpc_exception(ret) == TRUE) {
                create_message_info(MSGID_SERVICE_RESTART_IN_PROGRESS, PROPERTY_MANAGER_SSLENCRYPTION, o_message_jso);
                return HTTP_BAD_REQUEST;
            }
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}

LOCAL gint32 get_keyboardlayout(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar str[PROPERTY_MANAGER_KEYBOARDLAYOUT_LEN] = {0};
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    
    ret = get_vncproperty_value_string(VNC_CLASS_NAME, PROPERTY_VNC_KEYBOARD_LAYOUT, str,
        PROPERTY_MANAGER_KEYBOARDLAYOUT_LEN);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: get_vncproperty_value_string fail. ", __FUNCTION__, __LINE__));
    *o_result_jso = json_object_new_string((const char *)str);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_string fail.", __FUNCTION__, __LINE__));

    return HTTP_OK;
}

LOCAL gint32 set_keyboardlayout(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    const char *str = 0;
    GSList *input_list = NULL;
    OBJ_HANDLE vnc_handle = 0;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_KVMMGNT, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, PROPERTY_MANAGER_VNCKEYLAYOUT, o_message_jso,
        PROPERTY_MANAGER_VNCKEYLAYOUT));

    str = dal_json_object_get_str(i_paras->val_jso);
    return_val_if_expr(NULL == str, HTTP_FORBIDDEN);
    ret = dal_get_object_handle_nth(VNC_CLASS_NAME, 0, &vnc_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s,%d: dal_get_object_handle_nth  fail. ", __FUNCTION__, __LINE__));

    input_list = g_slist_append(input_list, g_variant_new_string(str));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, vnc_handle,
        VNC_CLASS_NAME, METHOD_VNC_SET_KEYBOARD_LAYOUT, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    if (ret == RET_OK) {
        return HTTP_OK;
    }

    debug_log(DLOG_ERROR, "%s: call methed %s return %d", __FUNCTION__, METHOD_VNC_SET_KEYBOARD_LAYOUT, ret);

    
    if (dal_is_rpc_exception(ret) == TRUE) {
        (void)create_message_info(MSGID_SERVICE_RESTART_IN_PROGRESS, PROPERTY_MANAGER_VNCKEYLAYOUT, o_message_jso);
        return HTTP_BAD_REQUEST;
    }

    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL gint32 set_psssword(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    const char *str = 0;
    GSList *input_list = NULL;
    OBJ_HANDLE vnc_handle = 0;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_KVMMGNT, HTTP_FORBIDDEN,
        json_string_clear(i_paras->val_jso);
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, PROPERTY_MANAGER_VNCPASSWORD, o_message_jso,
        PROPERTY_MANAGER_VNCPASSWORD));
    

    str = dal_json_object_get_str(i_paras->val_jso);
    return_val_if_expr(NULL == str, HTTP_FORBIDDEN);
    ret = dal_get_object_handle_nth(VNC_CLASS_NAME, 0, &vnc_handle);
    
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, json_string_clear(i_paras->val_jso);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s,%d: dal_get_object_handle_nth  fail. ", __FUNCTION__, __LINE__));
    

    input_list = g_slist_append(input_list, g_variant_new_string(str));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, vnc_handle,
        VNC_CLASS_NAME, METHOD_VNC_SET_PWD, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    
    json_string_clear(i_paras->val_jso);
    

    switch (ret) {
        case VOS_OK:
            return HTTP_OK;

            
        case VNC_PSWD_LEN_ERR:
            (void)create_message_info(MSGID_INVALID_PSWD_LEN, PROPERTY_MANAGER_VNCPASSWORD, o_message_jso,
                VNC_PASSWORD_LEN_MIN, VNC_PASSWORD_LEN_MAX);
            return HTTP_BAD_REQUEST;

            
        case VNC_PSWD_CMPLX_CHK_FAIL:
            (void)create_message_info(MSGID_PSWD_CMPLX_CHK_FAIL, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;

        default:
            debug_log(DLOG_ERROR, "%s: call methed %s return %d", __FUNCTION__, METHOD_VNC_SET_PWD, ret);
            
            if (dal_is_rpc_exception(ret) == TRUE) {
                create_message_info(MSGID_SERVICE_RESTART_IN_PROGRESS, PROPERTY_MANAGER_VNCPASSWORD, o_message_jso);
                return HTTP_BAD_REQUEST;
            }
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}

LOCAL gint32 get_password_validity_days(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint32 vnc_prop = 0;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = get_vncproperty_value_guint32(VNC_CLASS_NAME, PROPERTY_VNC_PWD_REMAIN_DAYS, &vnc_prop);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: get_vncproperty_value_guint32 fail. ", __FUNCTION__, __LINE__));

    if (vnc_prop >= INVALID_UINT16) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_int(vnc_prop);
    return HTTP_OK;
}

LOCAL gint32 get_vncsession_timeout(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gint32 vnc_timeout = 0;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = get_vncproperty_value_gint32(VNC_CLASS_NAME, PROPERTY_VNC_TIMEOUT, &vnc_timeout);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: get_vncproperty_value_gint32 fail. ", __FUNCTION__, __LINE__));
    *o_result_jso = json_object_new_int(vnc_timeout);
    return HTTP_OK;
}

LOCAL gint32 set_vncsession_timeout(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gint32 vnc_timeout;
    GSList *input_list = NULL;
    OBJ_HANDLE vnc_handle = 0;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_KVMMGNT, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, PROPERTY_MANAGER_VNCTIMOUT, o_message_jso,
        PROPERTY_MANAGER_VNCTIMOUT));
    
    
    if (TRUE == json_object_is_type(i_paras->val_jso, json_type_double)) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, PROPERTY_MANAGER_VNCTIMOUT, o_message_jso,
            dal_json_object_get_str(i_paras->val_jso), PROPERTY_MANAGER_VNCTIMOUT);

        return HTTP_BAD_REQUEST;
    }

    vnc_timeout = json_object_get_int(i_paras->val_jso);

    ret = dal_get_object_handle_nth(VNC_CLASS_NAME, 0, &vnc_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s,%d: dal_get_object_handle_nth  fail. ", __FUNCTION__, __LINE__));

    input_list = g_slist_append(input_list, g_variant_new_int32(vnc_timeout));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, vnc_handle,
        VNC_CLASS_NAME, METHOD_VNC_SET_TIMEOUT, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    if (ret == RET_OK) {
        return HTTP_OK;
    }

    debug_log(DLOG_ERROR, "%s: call methed %s return %d", __FUNCTION__, METHOD_VNC_SET_TIMEOUT, ret);

    
    if (dal_is_rpc_exception(ret) == TRUE) {
        create_message_info(MSGID_SERVICE_RESTART_IN_PROGRESS, PROPERTY_MANAGER_VNCTIMOUT, o_message_jso);
        return HTTP_BAD_REQUEST;        
    }

    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL gint32 get_manager_vnc_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar slot_uri[PSLOT_URI_LEN] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    gint32 ret;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: redfish_get_board_slot fail.", __FUNCTION__, __LINE__));
    ret = snprintf_s(slot_uri, sizeof(slot_uri), sizeof(slot_uri) - 1, URI_FORMAT_VNCSERVICE_ODATAID, slot);
    return_val_do_info_if_fail(0 < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:snprintf_s fail.", __FUNCTION__, __LINE__));
    *o_result_jso = json_object_new_string((const char *)slot_uri);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_string fail.", __FUNCTION__, __LINE__));

    return HTTP_OK;
}

LOCAL gint32 get_manager_vnc_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
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

    ret = snprintf_s(odata_context, sizeof(odata_context), sizeof(odata_context) - 1, URI_FORMAT_VNCSERVICE_CONTEXT,
        slot_id);
    return_val_do_info_if_fail(0 < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:snprintf_s fail.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_string((const char *)odata_context);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_string fail.", __FUNCTION__, __LINE__));

    return HTTP_OK;
}

LOCAL gint32 get_vnc_numberofactivatedsessions(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint32 vnc_num = 0;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = get_vncproperty_value_gint32(VNC_CLASS_NAME, PROPERTY_VNC_CONNECT_NUM, &vnc_num);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: get_vncproperty_value_gint32 fail. ", __FUNCTION__, __LINE__));
    *o_result_jso = json_object_new_int(vnc_num);
    return HTTP_OK;
}

LOCAL gint32 get_sessions_links(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    guint32 idx, count;
    GSList *output_list = NULL;
    struct json_object *jso_dict = NULL;
    struct json_object *jso = NULL;
    struct json_object *vnc_jso = NULL; 
    struct json_object *jso_new = NULL;
    const gchar *session_checksum = NULL;
    gchar strbuf[REDFISH_SESSION_STR_LEN] = {0};
    GSList *input_list = NULL;
    json_bool ret_bool;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    
    input_list = g_slist_append(input_list, g_variant_new_string(i_paras->session_id));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, OBJ_HANDLE_NULL,
        CLASS_SESSION, METHOD_SESSION_GETSESSIONLIST, 0, i_paras->user_role_privilege, input_list, &output_list);
    
    uip_free_gvariant_list(input_list);

    
    switch (ret) {
        case RFERR_NO_RESOURCE: 
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri);
            return HTTP_NOT_FOUND;

        case VOS_OK:
        case RFERR_SUCCESS:
            break;

        default: 
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    const gchar *str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    struct json_object *jso_org = json_tokener_parse(str);
    uip_free_gvariant_list(output_list);
    
    count = json_object_array_length(jso_org);
    json_object *sessions_jso = json_object_new_array();
    return_val_do_info_if_expr(NULL == sessions_jso, HTTP_INTERNAL_SERVER_ERROR, json_object_put(jso_org);
        debug_log(DLOG_ERROR, "%s,%d:	 json_object_new_array fail. ", __FUNCTION__, __LINE__));

    
    goto_label_do_info_if_fail(i_paras->user_role_privilege & USERROLE_USERMGNT, __exit,
                               { debug_log(DLOG_DEBUG, "%s:Permission denied.", __FUNCTION__); });

    for (idx = 0; idx < count; idx++) {
        
        jso_dict = json_object_array_get_idx(jso_org, idx);
        continue_if_expr(NULL == jso_dict);
        
        ret_bool = json_object_object_get_ex(jso_dict, RF_SESSION_TYPE, &vnc_jso);
        continue_if_expr(!ret_bool);
        if (g_strcmp0(dal_json_object_get_str(vnc_jso), USER_LOGIN_INTERFACE_VNC_SHARED) != 0 &&
            g_strcmp0(dal_json_object_get_str(vnc_jso), USER_LOGIN_INTERFACE_VNC_PRIVATE) != 0) {
            continue;
        }

        
        if (json_object_object_get_ex(jso_dict, RF_SESSION_CHECKSUM, &jso)) {
            session_checksum = dal_json_object_get_str(jso);
            continue_if_expr(NULL == session_checksum);

            ret = snprintf_s(strbuf, sizeof(strbuf), sizeof(strbuf) - 1, "%s/%s", SESSIONSERVICE_SESSIONS,
                session_checksum);
            continue_if_expr(0 >= ret);

            jso_new = json_object_new_object();
            continue_if_expr(NULL == jso_new);

            json_object_object_add(jso_new, RFPROP_ODATA_ID, json_object_new_string((const char *)strbuf));

            
            json_object_array_add(sessions_jso, jso_new);
        }
    }

__exit:

    
    json_object_put(jso_org);
    *o_result_jso = json_object_new_object();
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR, json_object_put(sessions_jso);
        debug_log(DLOG_ERROR, "%s,%d: json_object_new_object fail.", __FUNCTION__, __LINE__));

    json_object_object_add(*o_result_jso, RFPROP_SESSIONSERVICE_SESSIONS, sessions_jso);

    return HTTP_OK;
}


LOCAL gint32 set_vnc_loginrule(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gint32 loop_tag_i;
    guint32 loop_tag_j = 0;
    guchar permit_role = 0;
    GSList *input_list = NULL;
    json_object *one_role = NULL;
    const gchar *one_role_str = NULL;
    const gchar* permit_role_map[] = {"Rule1", "Rule2", "Rule3"};
    gint32 int_repeat_flag[3] = {0};
    gchar name_parameter[ARRAY_LENTH] = {0};
    OBJ_HANDLE vnc_handle = 0;

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_KVMMGNT, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_MANAGER_PATCH_LOGIN, o_message_jso,
        RFPROP_MANAGER_PATCH_LOGIN));

    
    gint32 array_count = json_object_array_length(i_paras->val_jso);
    if (3 < array_count) {
        (void)create_message_info(MSGID_ARRAY_ITEM_EXCEED, RFPROP_MANAGER_PATCH_LOGIN, o_message_jso,
            RFPROP_MANAGER_PATCH_LOGIN);
        return HTTP_BAD_REQUEST;
    }

    for (loop_tag_i = 0; loop_tag_i < array_count; loop_tag_i++) {
        one_role = json_object_array_get_idx(i_paras->val_jso, loop_tag_i);
        one_role_str = dal_json_object_get_str(one_role);

        for (loop_tag_j = 0; loop_tag_j < 3; loop_tag_j++) {
            if (VOS_OK == g_strcmp0(permit_role_map[loop_tag_j], one_role_str)) {
                if (int_repeat_flag[loop_tag_j]) {
                    
                    
                    (void)snprintf_s(name_parameter, sizeof(name_parameter), sizeof(name_parameter) - 1, "LoginRule/%d",
                        loop_tag_i);
                    
                    (void)create_message_info(MSGID_PROP_ITEM_DUPLICATE, NULL, o_message_jso, 
                        (const char *)name_parameter);
                    return HTTP_BAD_REQUEST;
                }

                permit_role += (1 << loop_tag_j);
                
                int_repeat_flag[loop_tag_j]++;
            }
        }
    }

    ret = dal_get_object_handle_nth(VNC_CLASS_NAME, 0, &vnc_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s,%d: dal_get_object_handle_nth	fail. ", __FUNCTION__, __LINE__));

    
    input_list = g_slist_append(input_list, g_variant_new_byte(permit_role));

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, vnc_handle,
        VNC_CLASS_NAME, METHOD_VNC_SET_PERMIT_RULEIDS, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret == RET_OK) {
        return HTTP_OK;
    }

    debug_log(DLOG_ERROR, "%s: call methed %s return %d", __FUNCTION__, METHOD_VNC_SET_PERMIT_RULEIDS, ret);

    
    if (dal_is_rpc_exception(ret) == TRUE) {
        (void)create_message_info(MSGID_SERVICE_RESTART_IN_PROGRESS, RFPROP_MANAGER_PATCH_LOGIN, o_message_jso);
        return HTTP_BAD_REQUEST;        
    }

    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    return HTTP_INTERNAL_SERVER_ERROR;
}

LOCAL gint32 get_vnc_add_loginrule(json_object *rul_array_json, const gchar *slot, guint8 memberid)
{
    gint32 ret;
    json_object *tmp_json = NULL;
    gchar uri[VNC_STRLEN_256] = {0};

    if (snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, URI_FORMAT_LOGRULE, slot, memberid) <= 0) {
        debug_log(DLOG_ERROR, "%s, %d: snprintf_s fail\n", __FUNCTION__, __LINE__);
        return VOS_ERR;
    }

    tmp_json = json_object_new_object();
    return_val_do_info_if_fail(NULL != tmp_json, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_object fail\n", __FUNCTION__, __LINE__));

    json_object_object_add(tmp_json, RFPROP_ODATA_ID, json_object_new_string((const char *)uri));
    ret = json_object_array_add(rul_array_json, tmp_json);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, (void)json_object_put(tmp_json);
        debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail\n", __FUNCTION__, __LINE__));

    return VOS_OK;
}


LOCAL gint32 get_vnc_loginrule_odataid(json_object *rul_array_json)
{
    gint32 ret;
    gchar slot[SNMP_STRLEN_32] = {0};
    OBJ_HANDLE obj_handle = 0;
    guint8 prop_value = 0;

    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_if_expr(VOS_OK != ret, VOS_ERR);
    ret = dal_get_object_handle_nth(VNC_CLASS_NAME, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d:  get object handle for %s fail", __FUNCTION__, __LINE__, CLASS_SNMP));
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_USER_PERMIT_RULE_IDS, &prop_value);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d:  get prop fail, %s, %s\n", __FUNCTION__, __LINE__, VNC_CLASS_NAME,
        "PermitRuleIds"));

    if (prop_value & 0x01) {
        ret = get_vnc_add_loginrule(rul_array_json, slot, LOGRULE_1);
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            debug_log(DLOG_ERROR, "%s, %d: get_vnc_add_loginrule fail\n", __FUNCTION__, __LINE__));
    }

    if (prop_value & 0x02) {
        ret = get_vnc_add_loginrule(rul_array_json, slot, LOGRULE_2);
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            debug_log(DLOG_ERROR, "%s, %d: get_vnc_add_loginrule fail\n", __FUNCTION__, __LINE__));
    }

    if (prop_value & 0x04) {
        ret = get_vnc_add_loginrule(rul_array_json, slot, LOGRULE_3);
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            debug_log(DLOG_ERROR, "%s, %d: get_vnc_add_loginrule fail\n", __FUNCTION__, __LINE__));
    }

    return VOS_OK;
}


LOCAL gint32 get_vnc_loginrule(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_array();
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:   memory alloc fail. ", __FUNCTION__, __LINE__));
    
    (void)get_vnc_loginrule_odataid(*o_result_jso);

    return HTTP_OK;
}

LOCAL gint32 get_manager_vnc_session_mode(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 session_mode = 255;
    const gchar *session_mode_str = NULL;
    gint32 ret;
    GSList *list = NULL;
    GSList *node = NULL;
    OBJ_HANDLE obj_handle = 0;
    gchar prop_val[PROP_VAL_MAX_LENGTH] = {0};

    return_val_do_info_if_expr(NULL == i_paras || NULL == o_message_jso || NULL == o_result_jso,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s : NULL pointer", __FUNCTION__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dfl_get_object_list(VNCLINK_CLASS_NAME, &list);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_OK,
        debug_log(DLOG_DEBUG, "%s, %d: 'get object list fail.'\n", __FUNCTION__, __LINE__));

    for (node = list; node; node = g_slist_next(node)) {
        obj_handle = (OBJ_HANDLE)node->data;
        
        (void)dal_get_property_value_string(obj_handle, VNCLINK_PROPERTY_IP, prop_val, PROP_VAL_MAX_LENGTH);

        if (0 == g_strcmp0(prop_val, "0")) {
            continue;
        }

        (void)dal_get_property_value_int32(obj_handle, VNCLINK_PROPERTY_MODE, &session_mode);

        if (PROPVAL_MODE_SHARED_NUM == session_mode) {
            session_mode_str = PROPVAL_MODE_SHARED;
            break;
        } else if (PROPVAL_MODE_EXCLUSIVE_NUM == session_mode) {
            session_mode_str = PROPVAL_MODE_EXCLUSIVE;
            break;
        }
    }

    g_slist_free(list);

    return_val_if_expr(255 == session_mode, HTTP_INTERNAL_SERVER_ERROR);

    *o_result_jso = json_object_new_string(session_mode_str);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_string fail.", __FUNCTION__, __LINE__));

    return HTTP_OK;
}

LOCAL PROPERTY_PROVIDER_S g_vncservice_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_vnc_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_vnc_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {PROPERTY_MANAGER_VNCTIMOUT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_vncsession_timeout, set_vncsession_timeout, NULL, ETAG_FLAG_ENABLE},
    {PROPERTY_MANAGER_VNCPASSVAL_DAYS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_password_validity_days, NULL, NULL, ETAG_FLAG_ENABLE},
    {PROPERTY_MANAGER_SSLENCRYPTION, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_sslencryption, set_sslencryption, NULL, ETAG_FLAG_ENABLE},
    {PROPERTY_MANAGER_VNCPASSWORD, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, NULL, set_psssword, NULL, ETAG_FLAG_ENABLE},
    {PROPERTY_MANAGER_VNCKEYLAYOUT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_keyboardlayout, set_keyboardlayout, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_PATCH_LOGIN, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_vnc_loginrule, set_vnc_loginrule, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_LINKS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_sessions_links, NULL, NULL, ETAG_FLAG_ENABLE},
    {PROPERTY_MANAGER_VNCNUM_SESSION, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_vnc_numberofactivatedsessions, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_VNC_SESSION_MODE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_vnc_session_mode, NULL, NULL, ETAG_FLAG_ENABLE},
};

gint32 managers_provider_vncservice_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gboolean ret;
    guchar vnc_enabled = 0;
    return_val_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count), HTTP_INTERNAL_SERVER_ERROR);
    
    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_VNC, &vnc_enabled);
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: dal_get_func_ability fail.\n", __FUNCTION__, __LINE__));
    return_val_if_expr(1 != vnc_enabled, HTTP_NOT_FOUND);
    ret = redfish_check_manager_uri_valid(i_paras->uri);
    return_val_do_info_if_expr(TRUE != ret, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s,%d:URI check failed. ", __FUNCTION__, __LINE__));

    *prop_provider = g_vncservice_provider;
    *count = sizeof(g_vncservice_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}
