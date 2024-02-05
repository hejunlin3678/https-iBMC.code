
#include "redfish_provider.h"

LOCAL gint32 get_ntp_poll_interval(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 set_ntp_poll_min_interval(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_ntp_poll_max_interval(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_ntp_key_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);

G_LOCK_DEFINE(g_ntp_cert_mutex);


LOCAL gint32 get_ntpproperty_value_byte(const gchar *class_name, const gchar *property_name, guchar *out_val)
{
    gint32 ret;
    OBJ_HANDLE ntp_handle = 0;
    ret = dal_get_object_handle_nth(class_name, 0, &ntp_handle);
    
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_DEBUG, "%s,%d: dal_get_object_handle_nth  fail. ", __FUNCTION__, __LINE__));
    
    ret = dal_get_property_value_byte(ntp_handle, property_name, out_val);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s,%d: dal_get_property_value_byte fail. ", __FUNCTION__, __LINE__));
    return VOS_OK;
}

LOCAL gint32 get_ntpproperty_value_string(const gchar *class_name, const gchar *property_name, gchar *out_val,
    guint32 out_len)
{
    gint32 ret;
    OBJ_HANDLE ntp_handle = 0;
    ret = dal_get_object_handle_nth(class_name, 0, &ntp_handle);
    
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_DEBUG, "%s,%d: dal_get_object_handle_nth  fail. ", __FUNCTION__, __LINE__));
    
    ret = dal_get_property_value_string(ntp_handle, property_name, out_val, out_len);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s,%d: dal_get_property_value_string fail. ", __FUNCTION__, __LINE__));
    return VOS_OK;
}

LOCAL gint32 get_ntp_serviceenabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    guchar serviceenabled = 0;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    
    ret = get_ntpproperty_value_byte(CLASS_NTP, PROPERTY_NTP_ENABLE_STATUS, &serviceenabled);
    
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s,%d: get_ntpproperty_value_byte fail. ", __FUNCTION__, __LINE__));
    
    (1 == serviceenabled) ? (*o_result_jso = json_object_new_boolean(TRUE)) :
                            (*o_result_jso = json_object_new_boolean(FALSE));
    return HTTP_OK;
}

LOCAL gint32 set_ntp_serviceenabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gint32 ret_bool;
    OBJ_HANDLE ntp_handle = 0;
    guchar set_serenabled;
    GSList *input_list = NULL;

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, PROPERTY_PREFERRED_NTP_ENABLE, o_message_jso,
        PROPERTY_PREFERRED_NTP_ENABLE));
    

    ret_bool = json_object_get_boolean(i_paras->val_jso);

    (ret_bool) ? (set_serenabled = 1) : (set_serenabled = 0);
    ret = dal_get_object_handle_nth(CLASS_NTP, 0, &ntp_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s,%d: dal_get_object_handle_nth  fail. ", __FUNCTION__, __LINE__));

    input_list = g_slist_append(input_list, g_variant_new_byte(set_serenabled));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, ntp_handle,
        CLASS_NTP, METHOD_NTP_SET_ENABLE_STATUS, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
            return HTTP_OK;

            
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            debug_log(DLOG_ERROR, "%s,%d: NTP enable setting failed.Return code[%d].", __FUNCTION__, __LINE__, ret);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}

LOCAL gint32 get_preferred_ntp_server(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar prefee_ntp[MANAGER_PREE_NTP_LEN] = {0};
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    
    ret = get_ntpproperty_value_string(CLASS_NTP, PROPERTY_NTP_PREFERRED_SERVER, prefee_ntp, MANAGER_PREE_NTP_LEN);
    
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s,%d: get_ntpproperty_value_string fail. ", __FUNCTION__, __LINE__));
    
    *o_result_jso = json_object_new_string((const char *)prefee_ntp);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_string fail.", __FUNCTION__, __LINE__));

    return HTTP_OK;
}

LOCAL gint32 ntp_server_set(PROVIDER_PARAS_S *i_paras, const gchar *ntp_service, json_object **o_message_jso)
{
    OBJ_HANDLE ntp_handle = 0;
    gint32 ret;
    errno_t g_ret;
    const gchar *ntp_preferred = NULL;
    GSList *input_list = NULL;
    gchar check_ntp_str[MANAGER_PREE_NTP_LEN] = {0};
    ntp_preferred = dal_json_object_get_str(i_paras->val_jso);
    g_ret = strncpy_s(check_ntp_str, sizeof(check_ntp_str), ntp_preferred, strlen(ntp_preferred));
    return_val_do_info_if_fail(EOK == g_ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:strncpy_s fail.", __FUNCTION__, __LINE__));

    
    ret = dal_check_string_is_valid_ascii((const char *)check_ntp_str);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: wrong format.", __FUNCTION__);
        return HTTP_BAD_REQUEST;
    }

    ret = dal_get_object_handle_nth(CLASS_NTP, 0, &ntp_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s,%d: dal_get_object_handle_nth  fail. ", __FUNCTION__, __LINE__));

    input_list = g_slist_append(input_list, g_variant_new_string(ntp_preferred));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, ntp_handle,
        CLASS_NTP, ntp_service, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    if (ret == NTP_PROPERTY_FORMAT_ERROR) {
        debug_log(DLOG_ERROR, "%s: wrong format.", __FUNCTION__);
        return HTTP_BAD_REQUEST;
    }

    return ret;
}

LOCAL gint32 set_preferred_ntp_server(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, PROPERTY_PREFERRED_NTP_SERVER, o_message_jso,
        PROPERTY_PREFERRED_NTP_SERVER));

    ret = ntp_server_set(i_paras, METHOD_NTP_SET_PREFERRED_SERVER, o_message_jso);

    switch (ret) {
        case VOS_OK:
            return HTTP_OK;

        case HTTP_BAD_REQUEST:
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, PROPERTY_PREFERRED_NTP_SERVER, o_message_jso,
                dal_json_object_get_str(i_paras->val_jso), PROPERTY_PREFERRED_NTP_SERVER);
            return HTTP_BAD_REQUEST;

            
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            debug_log(DLOG_ERROR, "%s,%d: NTP server address setting failed.Return code[%d].", __FUNCTION__, __LINE__,
                ret);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}

LOCAL gint32 get_alternate_ntp_server(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar alternate_ntp[MANAGER_PREE_NTP_LEN] = {0};
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    
    ret = get_ntpproperty_value_string(CLASS_NTP, PROPERTY_NTP_ALTERNATIVE_SERVER, alternate_ntp, MANAGER_PREE_NTP_LEN);
    
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s,%d: get_ntpproperty_value_string fail. ", __FUNCTION__, __LINE__));
    
    *o_result_jso = json_object_new_string((const char *)alternate_ntp);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_string fail.", __FUNCTION__, __LINE__));

    return HTTP_OK;
}

LOCAL gint32 set_alternate_ntp_server(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, PROPERTY_ALTERNATE_NTP_SERVER, o_message_jso,
        PROPERTY_ALTERNATE_NTP_SERVER));

    ret = ntp_server_set(i_paras, METHOD_NTP_SET_ALTERNATIVE_SERVER, o_message_jso);

    switch (ret) {
        case VOS_OK:
            return HTTP_OK;

        case HTTP_BAD_REQUEST:
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, PROPERTY_ALTERNATE_NTP_SERVER, o_message_jso,
                dal_json_object_get_str(i_paras->val_jso), PROPERTY_ALTERNATE_NTP_SERVER);
            return HTTP_BAD_REQUEST;

            
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            debug_log(DLOG_ERROR, "%s,%d: NTP server alternate address setting failed.Return code[%d].", __FUNCTION__,
                __LINE__, ret);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}

LOCAL gint32 get_extra_ntp_server(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar ntp_server[MANAGER_PREE_NTP_LEN] = {0};
    
    return_val_do_info_if_expr(
        (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    
    ret = get_ntpproperty_value_string(CLASS_NTP, PROPERTY_NTP_EXTRA_SERVER, ntp_server, MANAGER_PREE_NTP_LEN);

    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s: get_ntpproperty_value_string fail. ", __FUNCTION__));

    *o_result_jso = json_object_new_string((const char *)ntp_server);
    return_val_do_info_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail.", __FUNCTION__));

    return HTTP_OK;
}

LOCAL gint32 set_extra_ntp_server(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    
    return_val_do_info_if_expr(
        (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, PROPERTY_EXTRA_NTP_SERVER, o_message_jso,
        PROPERTY_EXTRA_NTP_SERVER));

    ret = ntp_server_set(i_paras, METHOD_NTP_SET_EXTRA_SERVER, o_message_jso);

    switch (ret) {
        case VOS_OK:
            return HTTP_OK;

        case HTTP_BAD_REQUEST:
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, PROPERTY_EXTRA_NTP_SERVER, o_message_jso,
                dal_json_object_get_str(i_paras->val_jso), PROPERTY_EXTRA_NTP_SERVER);
            return HTTP_BAD_REQUEST;

            
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            debug_log(DLOG_ERROR, "%s,%d: NTP server address setting failed.Return code[%d].", __FUNCTION__, __LINE__,
                ret);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}

LOCAL gint32 get_ntp_server_conut(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    guchar ntp_serverCount = 0;

    
    return_val_do_info_if_expr(
        (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = get_ntpproperty_value_byte(CLASS_NTP, PROPERTY_NTP_SERVER_COUNT, &ntp_serverCount);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:get PROPERTY_NTP_SERVER_COUNT failed.", __FUNCTION__));
    *o_result_jso = json_object_new_int(ntp_serverCount);
    return_val_do_info_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:json_object_new_int fail.", __FUNCTION__));
    return HTTP_OK;
}


LOCAL gint32 get_operating_mode(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    guchar mode = 0;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    
    ret = get_ntpproperty_value_byte(CLASS_NTP, PROPERTY_NTP_MODE, &mode);
    
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s,%d: get_ntpproperty_value_byte fail. ", __FUNCTION__, __LINE__));
    
    if (MANAGER_NTP_MANUAL == mode) {
        *o_result_jso = json_object_new_string(MANAGER_NTPSERVICE_MODE_MANUAL);
    } else if (MANAGER_NTP_DHCPV4 == mode) {
        *o_result_jso = json_object_new_string(MANAGER_NTPSERVICE_MODE_DHCPV4);
    } else if (MANAGER_NTP_DHCPV6 == mode) {
        *o_result_jso = json_object_new_string(MANAGER_NTPSERVICE_MODE_DHCPV6);
    }

    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_string fail.", __FUNCTION__, __LINE__));
    return HTTP_OK;
}

LOCAL gint32 set_operating_mode(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    const gchar *ntp_mode = NULL;
    gint8 operating_mode = 0;
    GSList *input_list = NULL;
    OBJ_HANDLE ntp_handle = 0;
    OBJ_HANDLE obj_handle = 0;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, PROPERTY_OPERATING_MODE, o_message_jso,
        PROPERTY_OPERATING_MODE));
    ntp_mode = dal_json_object_get_str(i_paras->val_jso);
    
    
    do_if_expr(0 == g_strcmp0(MANAGER_NTPSERVICE_MODE_MANUAL, ntp_mode), (operating_mode = MAMAGER_NTP_MANUAL));
    do_if_expr(0 == g_strcmp0(MANAGER_NTPSERVICE_MODE_DHCPV4, ntp_mode), (operating_mode = MAMAGER_NTP_DHCPV4));
    do_if_expr(0 == g_strcmp0(MANAGER_NTPSERVICE_MODE_DHCPV6, ntp_mode), (operating_mode = MAMAGER_NTP_DHCPV6));
    
    ret = dal_get_object_handle_nth(CLASS_NTP, 0, &ntp_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s,%d: dal_get_object_handle_nth  fail. ", __FUNCTION__, __LINE__));

    
    ret = dal_eth_get_out_type_object(OUTTER_GROUP_TYPE, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s,%d: dal_get_object_handle_nth  fail. ", __FUNCTION__, __LINE__));

    if (0 != g_strcmp0(MANAGER_NTPSERVICE_MODE_MANUAL, ntp_mode)) {
        
        ret = verify_ip_protocol_enabled(obj_handle, ntp_mode);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST,
            (void)create_message_info(MSGID_MODIFY_FAILED_WITH_IPVER, PROPERTY_OPERATING_MODE, o_message_jso,
            PROPERTY_OPERATING_MODE, ntp_mode));
        
        ret = verify_ip_mode(obj_handle, ntp_mode);
        return_val_do_info_if_expr(VOS_OK == ret, HTTP_BAD_REQUEST,
            (void)create_message_info(MSGID_STATIC_IP_MODE, PROPERTY_OPERATING_MODE, o_message_jso, ntp_mode));
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(operating_mode));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, ntp_handle,
        CLASS_NTP, METHOD_NTP_SET_MODE, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
            return HTTP_OK;

            
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            debug_log(DLOG_ERROR, "%s,%d: NTP server operating mode setting failed.Return code[%d].", __FUNCTION__,
                __LINE__, ret);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 get_server_authentication(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guchar authentication = 0;
    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }
    
    ret = get_ntpproperty_value_byte(CLASS_NTP, PROPERTY_NTP_AUTH_ENABLE_STATUS, &authentication);
    
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get_ntpproperty_value_byte fail,ret=%d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    (authentication) ? (*o_result_jso = json_object_new_boolean(TRUE)) :
                       (*o_result_jso = json_object_new_boolean(FALSE));
    return HTTP_OK;
}


LOCAL gint32 set_server_authentication(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    json_bool ret_bool;
    guchar set_authentication;
    GSList *input_list = NULL;
    OBJ_HANDLE ntp_handle = 0;
    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    if ((i_paras->user_role_privilege & USERROLE_BASICSETTING) == 0) {
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, PROPERTY_SERVER_AUTHENTICATION, o_message_jso,
            PROPERTY_SERVER_AUTHENTICATION);
        return HTTP_FORBIDDEN;
    }
    
    ret_bool = json_object_get_boolean(i_paras->val_jso);

    (ret_bool) ? (set_authentication = 1) : (set_authentication = 0);
    ret = dal_get_object_handle_nth(CLASS_NTP, 0, &ntp_handle);
    if (ret != RET_OK) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: dal_get_object_handle_nth  fail, ret=%d.", __FUNCTION__, ret);
        return RET_ERR;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(set_authentication));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, ntp_handle,
        CLASS_NTP, METHOD_NTP_SET_AUTH_ENABLE_STATUS, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case RET_OK:
            return HTTP_OK;

            
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            debug_log(DLOG_ERROR, "%s,%d: Certificate authentication setup failed.Return code[%d].", __FUNCTION__,
                __LINE__, ret);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 get_manager_ntpservice_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
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
    ret = snprintf_s(slot_uri, sizeof(slot_uri), sizeof(slot_uri) - 1, URI_FORMAT_NTPSERVICE_ODATAID, slot);
    return_val_do_info_if_fail(0 < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:snprintf_s fail.", __FUNCTION__, __LINE__));
    *o_result_jso = json_object_new_string((const char *)slot_uri);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_string fail.", __FUNCTION__, __LINE__));

    return HTTP_OK;
}

LOCAL gint32 get_manager_ntpservice_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
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

    ret = snprintf_s(odata_context, sizeof(odata_context), sizeof(odata_context) - 1, URI_FORMAT_NTPSERVICE_CONTEXT,
        slot_id);
    return_val_do_info_if_fail(0 < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:snprintf_s fail.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_string((const char *)odata_context);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_string fail.", __FUNCTION__, __LINE__));

    return HTTP_OK;
}


LOCAL gint32 get_manager_ntpservice_actions(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    json_object *import_key = NULL;
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    gchar ntp_target_uri[PSLOT_URI_LEN] = {0};
    gchar ntp_info_import[PSLOT_URI_LEN] = {0};
    gint32 ret;
    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = redfish_get_board_slot(slot, sizeof(slot));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: function  return err of redfish_slot_id,ret=%d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = snprintf_s(ntp_target_uri, sizeof(ntp_target_uri), sizeof(ntp_target_uri) - 1, MANAGER_NTPIMPORT_KEY, slot);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s:snprintf_s fail,ret=%d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret =
        snprintf_s(ntp_info_import, sizeof(ntp_info_import), sizeof(ntp_info_import) - 1, MANAGER_NTPACTION_INFO, slot);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s:snprintf_s fail,ret=%d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    import_key = json_object_new_object();
    if (import_key == NULL) {
        debug_log(DLOG_ERROR, "%s:json_object_new_object fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    json_object_object_add(import_key, RFPROP_TARGET, json_object_new_string((const char *)ntp_target_uri));
    json_object_object_add(import_key, ACTION_INFO, json_object_new_string((const char *)ntp_info_import));
    *o_result_jso = json_object_new_object();
    if (*o_result_jso == NULL) {
        json_object_put(import_key);
        debug_log(DLOG_ERROR, "%s:json_object_new_object fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    json_object_object_add(*o_result_jso, MANAGER_NTPSERVICE_NTPKEY, import_key);
    return HTTP_OK;
}


LOCAL gint32 ntp_import_certificate_local(const gchar *ntp_str, json_object **o_message_jso, PROVIDER_PARAS_S *i_paras)
{
    gint32 ret;
    GSList *input_list = NULL;
    OBJ_HANDLE ntp_handle;

    ret = dal_get_object_handle_nth(CLASS_NTP, 0, &ntp_handle);
    if (ret != RET_OK) {
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s:uip_call_class_method_redfish fail,ret=%d.", __FUNCTION__, ret);
        return RET_ERR;
    }

    
    input_list = g_slist_append(input_list, g_variant_new_string(ntp_str));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, ntp_handle,
        CLASS_NTP, METHOD_NTP_IMPORT_GROUP_KEY, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    
    
    json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);

    
    switch (ret) {
            
        case RET_OK:
            (void)create_message_info(MAGID_UPLOAD_NTPGROUPKEY_SUCCESSFULLY, NULL, o_message_jso);
            return RET_OK;

        case KEY_TOO_BIG:
            (void)create_message_info(MSGID_NTPGROUPKEY_TOOLARGE, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        case KEY_FILE_NOT_SUPPORTED:
            (void)create_message_info(MSGID_NTPGROUPKEY_NOT_SUPPORTED, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;

        default:
            (void)create_message_info(MSGID_UPLOAD_NTPGROUPKEY_FAILED, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
            
    }
}


LOCAL gint32 ntp_import_certificate_text(const gchar *ntp_str, json_object **o_message_jso, PROVIDER_PARAS_S *i_paras)
{
    gint32 ntp_file_len;
    gboolean gb_ret;
    gint32 ret;

    ntp_file_len = strlen(ntp_str);

    G_LOCK(g_ntp_cert_mutex);
    
    gb_ret = g_file_set_contents(NTPSERVICE_FILE_PATH, ntp_str, ntp_file_len, NULL);
    if (gb_ret == FALSE) {
        debug_log(DLOG_ERROR, "%s:g_file_set_contents fail.", __FUNCTION__);
        G_UNLOCK(g_ntp_cert_mutex);
        goto err_exit;
    }

    (void)g_chmod(NTPSERVICE_FILE_PATH, S_IRUSR | S_IWUSR); // 设置权限600
    ret = ntp_import_certificate_local(NTPSERVICE_FILE_PATH, o_message_jso, i_paras);
    G_UNLOCK(g_ntp_cert_mutex);

    return ret;
err_exit:
    
    json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    return RET_ERR;
}


LOCAL gint32 get_file_download_process(TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    gint32 ret;
    gint32 process = 0;
    ret = get_file_transfer_progress(&process);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:get_file_transfer_progress failed", __FUNCTION__);
        goto err_exit;
    }

    
    if (process < 0) {
        debug_log(DLOG_ERROR, "%s:get_file_transfer_progress err, process=%d.", __FUNCTION__, process);
        (void)parse_file_transfer_err_code(process, message_obj);
        monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        return RET_ERR;
    }

    
    if (process != RF_FINISH_PERCENTAGE) {
        monitor_fn_data->task_state = RF_TASK_RUNNING;
        monitor_fn_data->task_progress = (guchar)(process * 0.1); // 0.1：文件传输显示为总体进度的10%
        return RET_ERR;
    }
    return RET_OK;
err_exit:
    monitor_fn_data->task_state = RF_TASK_EXCEPTION;
    (void)create_message_info(MSGID_UPLOAD_NTPGROUPKEY_FAILED, NULL, message_obj);
    return RET_ERR;
}

LOCAL gint32 task_ntp_import_certificate_remote(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    gint32 ret;
    gchar *ntp_key_name = NULL;
    gchar  ntp_path[NTP_FILE_NAME_LEN] = {0};
    const gchar *ntp_str = NULL;
    PROVIDER_PARAS_S *ntp_info = (PROVIDER_PARAS_S *)monitor_fn_data->user_data;
    monitor_fn_data->task_state = RF_TASK_RUNNING;

    ret = get_file_download_process(monitor_fn_data, message_obj);
    if (ret != RET_OK) { // 远程传输未完成，直接返回
        debug_log(DLOG_ERROR, "%s:get_file_download_process not finished or failed.", __FUNCTION__);
        return RET_OK;
    }

    
    (void)get_element_str(ntp_info->val_jso, RFACTION_PARAM_CONTENT, &ntp_str);
    if (ntp_str == NULL) {
        goto err_exit;
    }
    ntp_key_name = g_strrstr(ntp_str, "/");
    if (ntp_key_name == NULL) {
        goto err_exit;
    }
    ret = snprintf_s(ntp_path, sizeof(ntp_path), sizeof(ntp_path) - 1, NTPSERVICE_TMP, ntp_key_name);
    if (ret <= 0) {
        goto err_exit;
    }
    ret = ntp_import_certificate_local(ntp_path, message_obj, ntp_info);
    if (ret == RET_OK) {
        monitor_fn_data->task_state = RF_TASK_COMPLETED;
        monitor_fn_data->task_progress = RF_FINISH_PERCENTAGE;
        return RET_OK;
    }

err_exit:
    monitor_fn_data->task_state = RF_TASK_EXCEPTION;
    monitor_fn_data->task_progress = RF_FILE_TRANSFER_PERCENTAGE;
    (void)create_message_info(MSGID_UPLOAD_NTPGROUPKEY_FAILED, NULL, message_obj);
    return RET_OK;
}


LOCAL gint32 ntp_transfer_groupkey_file(const gchar *ntp_str, PROVIDER_PARAS_S *i_paras)
{
    gint32 ret;
    OBJ_HANDLE file_transfer_handle;
    GSList *input_list = NULL;
    gchar ntp_certificate[NTP_FILE_NAME_LEN] = {0};

    ret = dal_get_object_handle_nth(CLASS_NAME_TRANSFERFILE, 0, &file_transfer_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "cann't get objhandle for %s", OBJ_NAME_TRANSFERFILE);
        return HTTP_BAD_REQUEST;
    }

    ret = snprintf_s(ntp_certificate, sizeof(ntp_certificate), sizeof(ntp_certificate) - 1, "download;%d;%s",
        NTP_KEY_FILE_ID, ntp_str);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s:snprintf_s fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    input_list = g_slist_append(input_list, g_variant_new_string((const char *)ntp_certificate));
    input_list = g_slist_append(input_list, g_variant_new_string(i_paras->session_id));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        file_transfer_handle, CLASS_NAME_TRANSFERFILE, METHOD_INITRIAL_FILE_TRANSFER, AUTH_DISABLE,
        i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:uip_call_class_method_redfish fail.", __FUNCTION__);
        return HTTP_BAD_REQUEST;
    }
    return RET_OK;
}

LOCAL gint32 ntp_download_info_task_create(PROVIDER_PARAS_S *i_paras, json_object **o_result_jso)
{
    gint32 ret;
    PROVIDER_PARAS_S *ntp_info = NULL;
    errno_t str_ret;
    TASK_MONITOR_INFO_S *task_ntp_file_monitor_info = NULL;

    
    task_ntp_file_monitor_info = task_monitor_info_new(g_free);
    if (task_ntp_file_monitor_info == NULL) {
        debug_log(DLOG_ERROR, "%s: task_monitor_info_new failed", __FUNCTION__);
        goto err_exit;
    }

    task_ntp_file_monitor_info->task_progress = TASK_NO_PROGRESS; // 配置成255，默认不显示进度
    task_ntp_file_monitor_info->rsc_privilege = USERROLE_BASICSETTING;

    ntp_info = (PROVIDER_PARAS_S *)g_malloc0(sizeof(PROVIDER_PARAS_S));
    if (ntp_info == NULL) {
        debug_log(DLOG_ERROR, "%s: g_malloc0 failed", __FUNCTION__);
        goto err_exit;
    }
    task_ntp_file_monitor_info->user_data = ntp_info;

    str_ret = memcpy_s(ntp_info, sizeof(PROVIDER_PARAS_S), i_paras, sizeof(PROVIDER_PARAS_S));
    if (str_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s failed", __FUNCTION__);
        goto err_exit;
    }

    
    ret = create_new_task("ntp certificate import ", task_ntp_import_certificate_remote, task_ntp_file_monitor_info,
        i_paras->val_jso, i_paras->uri, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: create_new_task failed", __FUNCTION__);
        goto err_exit;
    }

    return RET_OK;
err_exit:
    task_monitor_info_free(task_ntp_file_monitor_info);
    return HTTP_BAD_REQUEST;
}


LOCAL gint32 ntp_download_certificate(const gchar *ntp_str, json_object **o_message_jso,
    PROVIDER_PARAS_S *i_paras, json_object **o_result_jso)
{
    gint32 ret;

    ret = ntp_transfer_groupkey_file(ntp_str, i_paras);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: ntp_transfer_groupkey_file failed", __FUNCTION__);
        goto err_exit;
    }

    ret = ntp_download_info_task_create(i_paras, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: ntp_download_info_task_create failed", __FUNCTION__);
        goto err_exit;
    }

    return HTTP_ACCEPTED;

err_exit:
    json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
    if (ret == HTTP_INTERNAL_SERVER_ERROR) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    } else {
        (void)create_message_info(MSGID_UPLOAD_NTPGROUPKEY_FAILED, NULL, o_message_jso);
    }
    return ret;
}


LOCAL gint32 ntp_import_certificate_uri(const gchar *ntp_str, json_object **o_message_jso,
    PROVIDER_PARAS_S *i_paras, json_object **o_result_jso)
{
    gint32 ret;
    gint32 ntp_type = 0;

    ret = redfish_check_uri_type(ntp_str, &ntp_type);
    if (ret == RET_ERR) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
        debug_log(DLOG_ERROR, "%s:redfish_check_uri_type fail,ret=%d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (ntp_type != URI_TYPE_LOCAL) {
        ret = ntp_download_certificate(ntp_str, o_message_jso, i_paras, o_result_jso);
    } else {
        
        if (ntp_str == NULL || dal_check_real_path(ntp_str) != RET_OK) {
            (void)create_message_info(MSGID_INVALID_PATH, NULL, o_message_jso, ntp_str, RFACTION_PARAM_CONTENT);
            return HTTP_BAD_REQUEST;
        }

        if (!dal_check_file_opt_user(i_paras->auth_type, (const gchar *)i_paras->user_name, ntp_str,
            (const gchar *)i_paras->user_roleid)) {
            (void)create_message_info(MSGID_NO_PRIV_OPT_FILE, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        }
        ret = ntp_import_certificate_local(ntp_str, o_message_jso, i_paras);
    }
    return ret;
}


LOCAL gint32 act_manager_ntpservice_actions(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    const gchar *ntp_str = NULL;
    const gchar *type_str = NULL;
    gint32 ret;
    gboolean b_ret;
    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    if ((USERROLE_BASICSETTING & i_paras->user_role_privilege) == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    (void)get_element_str(i_paras->val_jso, RFACTION_PARAM_CONTENT, &ntp_str);
    if (ntp_str == NULL) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s:cann't get %s from request.", __FUNCTION__, RFACTION_PARAM_CONTENT);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    (void)get_element_str(i_paras->val_jso, RFACTION_PARAM_TYPE, &type_str);
    if (type_str == NULL) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s:cann't get %s from request.", __FUNCTION__, RFACTION_PARAM_TYPE);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (g_strcmp0(type_str, NTP_MANAGER_TYPE_TEXT) == 0) {
        
        ret = ntp_import_certificate_text(ntp_str, o_message_jso, i_paras);
    } else {
        
        b_ret = g_regex_match_simple(NTPKEY_FILE_REGEX, ntp_str, G_REGEX_OPTIMIZE, (GRegexMatchFlags)0);
        if (b_ret == FALSE) {
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFACTION_PARAM_CONTENT, o_message_jso, RF_SENSITIVE_INFO,
                RFACTION_PARAM_CONTENT);
            return HTTP_BAD_REQUEST;
        }
        
        
        ret = ntp_import_certificate_uri(ntp_str, o_message_jso, i_paras, o_result_jso);
    }

    switch (ret) {
        case RET_OK:
            return HTTP_OK;

            
        case HTTP_ACCEPTED:
            return HTTP_ACCEPTED;

        case HTTP_BAD_REQUEST:
            return HTTP_BAD_REQUEST;

        default:
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}

LOCAL gint32 act_manager_ntpservice_actions_entry(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret = act_manager_ntpservice_actions(i_paras, o_message_jso, o_result_jso);
    if (ret != HTTP_ACCEPTED && i_paras != NULL && i_paras->val_jso != NULL) {
        delete_user_specific_file(i_paras, i_paras->val_jso, RFACTION_PARAM_CONTENT);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
    }
    return ret;
}


LOCAL PROPERTY_PROVIDER_S g_ntpservice_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_ntpservice_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_ntpservice_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {PROPERTY_PREFERRED_NTP_ENABLE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_ntp_serviceenabled, set_ntp_serviceenabled, NULL, ETAG_FLAG_ENABLE},
    {PROPERTY_PREFERRED_NTP_SERVER, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_preferred_ntp_server, set_preferred_ntp_server, NULL, ETAG_FLAG_ENABLE},
    {PROPERTY_ALTERNATE_NTP_SERVER, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_alternate_ntp_server, set_alternate_ntp_server, NULL, ETAG_FLAG_ENABLE},
    {PROPERTY_EXTRA_NTP_SERVER, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_extra_ntp_server, set_extra_ntp_server, NULL, ETAG_FLAG_ENABLE},
    {PROPERTY_NTP_SERVER_COUNT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_ntp_server_conut, NULL, NULL, ETAG_FLAG_ENABLE},
    {PROPERTY_OPERATING_MODE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_operating_mode, set_operating_mode, NULL, ETAG_FLAG_ENABLE},
    
    {PROPERTY_NTP_MIN_POLLTNTERVAL, CLASS_NTP, PROPERTY_NTP_MINIMUM_POLLING_INTERVAL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_ntp_poll_interval, set_ntp_poll_min_interval, NULL, ETAG_FLAG_ENABLE},
    {PROPERTY_NTP_MAX_POLLTNTERVAL, CLASS_NTP, PROPERTY_NTP_MAXIMUM_POLLING_INTERVAL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_ntp_poll_interval, set_ntp_poll_max_interval, NULL, ETAG_FLAG_ENABLE},
    
    {PROPERTY_NTP_KEY_STATUS, CLASS_NTP, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_ntp_key_status,
     NULL, NULL, ETAG_FLAG_ENABLE},
    {PROPERTY_SERVER_AUTHENTICATION, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID,
     get_server_authentication, set_server_authentication, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_ACTION, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
     get_manager_ntpservice_actions, NULL, NULL, ETAG_FLAG_ENABLE},
    {NTPSERVICE_ACTIONS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID, NULL, NULL,
     act_manager_ntpservice_actions_entry, ETAG_FLAG_ENABLE},
};


LOCAL gint32 _rf_check_poll_interval(gint32 type, gint32 poll_value, json_object *user_data)
{
    gint32 ret = 0;
    json_object *obj_minpoll = NULL;
    json_object *obj_maxpoll = NULL;

    guint8 u8_minpoll = INVALID_VAL;
    guint8 u8_maxpoll = INVALID_VAL;

    gint32 minpoll = INVALID_VAL;
    gint32 maxpoll = INVALID_VAL;

    return_val_if_expr(NULL == user_data, VOS_ERR);

    
    if (TRUE == json_object_object_get_ex(user_data, PROPERTY_NTP_MIN_POLLTNTERVAL, &obj_minpoll)) {
        do_val_if_expr(json_type_int == json_object_get_type(obj_minpoll), (
            minpoll = json_object_get_int(obj_minpoll) > NTP_MINPOLL ? json_object_get_int(obj_minpoll) : NTP_MINPOLL));
        do_val_if_expr(json_type_null == json_object_get_type(obj_minpoll), (minpoll = INVALID_VAL));
    } else {
        
        ret = get_ntpproperty_value_byte(CLASS_NTP, PROPERTY_NTP_MINIMUM_POLLING_INTERVAL, &u8_minpoll);
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            debug_log(DLOG_DEBUG, "%s,%d: get_ntpproperty_value_byte fail. ", __FUNCTION__, __LINE__));
        minpoll = u8_minpoll;
        
        do_val_if_expr(minpoll == NTP_DEFAULT_BYTE, (minpoll = NTP_DEFAULT_MINPOLL));
        
    }

    
    if (TRUE == json_object_object_get_ex(user_data, PROPERTY_NTP_MAX_POLLTNTERVAL, &obj_maxpoll)) {
        do_val_if_expr(json_type_int == json_object_get_type(obj_maxpoll), (
            maxpoll = json_object_get_int(obj_maxpoll) < NTP_MAXPOLL ? json_object_get_int(obj_maxpoll) : NTP_MAXPOLL));
        do_val_if_expr(json_type_null == json_object_get_type(obj_maxpoll), (maxpoll = INVALID_VAL));
    } else {
        ret = get_ntpproperty_value_byte(CLASS_NTP, PROPERTY_NTP_MAXIMUM_POLLING_INTERVAL, &u8_maxpoll);
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            debug_log(DLOG_DEBUG, "%s,%d: get_ntpproperty_value_byte fail. ", __FUNCTION__, __LINE__));
        maxpoll = u8_maxpoll;
        
        do_val_if_expr(maxpoll == NTP_DEFAULT_BYTE, (maxpoll = NTP_DEFAULT_MAXPOLL));
        
    }
    
    if (obj_minpoll != NULL) {
        if (json_object_get_int(obj_minpoll) < NTP_MINPOLL || json_object_get_int(obj_minpoll) > NTP_MAXPOLL) {
            ret = get_ntpproperty_value_byte(CLASS_NTP, PROPERTY_NTP_MINIMUM_POLLING_INTERVAL, &u8_minpoll);
            return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
                debug_log(DLOG_DEBUG, "%s,%d: get_ntpproperty_value_byte fail. ", __FUNCTION__, __LINE__));
            minpoll = u8_minpoll;
        }
    }
    
    
    if (obj_maxpoll != NULL) {
        if (json_object_get_int(obj_maxpoll) < NTP_MINPOLL || json_object_get_int(obj_maxpoll) > NTP_MAXPOLL) {
            ret = get_ntpproperty_value_byte(CLASS_NTP, PROPERTY_NTP_MAXIMUM_POLLING_INTERVAL, &u8_maxpoll);
            return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
                debug_log(DLOG_DEBUG, "%s,%d: get_ntpproperty_value_byte fail. ", __FUNCTION__, __LINE__));
            maxpoll = u8_maxpoll;
        }
    }
    
    

    if (INVALID_VAL == poll_value) {
        return VOS_OK;
    }

    // 检查最小值
    if (type == 0) {
        if ((maxpoll == INVALID_VAL) || (poll_value <= maxpoll)) {
            return VOS_OK;
        }
    } else { // 检查最大值
        if ((minpoll == INVALID_VAL) || (poll_value >= minpoll)) {
            return VOS_OK;
        }
    }

    return VOS_ERR;
}

LOCAL gint32 get_ntp_poll_interval(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    guchar value = 0;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    

    ret = get_ntpproperty_value_byte(g_ntpservice_provider[i_paras->index].pme_class_name,
        g_ntpservice_provider[i_paras->index].pme_prop_name, &value);
    
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s,%d: get_ntpproperty_value_byte fail. ", __FUNCTION__, __LINE__));
    
    
    if (NTP_DEFAULT_BYTE == value) {
        if (0 == g_strcmp0(g_ntpservice_provider[i_paras->index].property_name, PROPERTY_NTP_MIN_POLLTNTERVAL)) {
            value = NTP_DEFAULT_MINPOLL;
        } else {
            value = NTP_DEFAULT_MAXPOLL;
        }
    }
    *o_result_jso = json_object_new_int(value);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_string fail.", __FUNCTION__, __LINE__));
    return HTTP_OK;
}


LOCAL gint32 set_ntp_poll_min_interval(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint32 value;
    json_type type;
    GSList *input_list = NULL;
    OBJ_HANDLE ntp_handle;
    gchar min_value[PROP_VAL_LENGTH] = {0};
    gchar max_value[PROP_VAL_LENGTH] = {0};

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    type = json_object_get_type(i_paras->val_jso);

    return_val_do_info_if_expr(json_type_int != type, HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "%s,%d:json_object_get_type fail.", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_PROP_TYPE_ERR, g_ntpservice_provider[i_paras->index].property_name,
        o_message_jso, type == json_type_null ? RF_VALUE_NULL : dal_json_object_get_str(i_paras->val_jso),
        g_ntpservice_provider[i_paras->index].property_name));

    
    value = json_object_get_int(i_paras->val_jso);
    
    return_val_do_info_if_expr(NTP_MINPOLL > value || NTP_MAXPOLL < value, HTTP_BAD_REQUEST,
        (void)snprintf_s(min_value, sizeof(min_value), sizeof(min_value) - 1, "%d", NTP_MINPOLL);
        (void)snprintf_s(max_value, sizeof(max_value), sizeof(max_value) - 1, "%d", NTP_MAXPOLL);
        (void)create_message_info(MSGID_PROP_VALUE_OUTOFRANGE, g_ntpservice_provider[i_paras->index].property_name,
        o_message_jso, dal_json_object_get_str(i_paras->val_jso), g_ntpservice_provider[i_paras->index].property_name,
        min_value, max_value));
    

    ret = _rf_check_poll_interval(0, value, i_paras->user_data);
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_POLL_INTERVAL_FAILED, g_ntpservice_provider[i_paras->index].property_name,
        o_message_jso, g_ntpservice_provider[i_paras->index].property_name, PROPERTY_NTP_MAX_POLLTNTERVAL,
        PROPERTY_NTP_MIN_POLLTNTERVAL));

    
    ret = dal_get_object_handle_nth(CLASS_NTP, 0, &ntp_handle);
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s, %d:dal_get_object_handle_nth fail.", __FUNCTION__, __LINE__));
    input_list = g_slist_append(input_list, g_variant_new_byte((guchar)value));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, ntp_handle,
        CLASS_NTP, METHOD_NTP_SET_MIN_POLLING_INTERVAL, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, g_ntpservice_provider[i_paras->index].property_name,
                o_message_jso, g_ntpservice_provider[i_paras->index].property_name);
            return HTTP_BAD_REQUEST;

        case VOS_OK:
            return HTTP_OK;

            
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            debug_log(DLOG_ERROR, "%s,%d: NTP enable setting failed.Return code[%d].", __FUNCTION__, __LINE__, ret);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 set_ntp_poll_max_interval(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint32 value;
    json_type type;
    GSList *input_list = NULL;
    OBJ_HANDLE ntp_handle;
    gchar max_value[PROP_VAL_LENGTH] = {0};
    gchar min_value[PROP_VAL_LENGTH] = {0};

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    type = json_object_get_type(i_paras->val_jso);

    return_val_do_info_if_expr(json_type_int != type, HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "%s,%d:json_object_get_type fail.", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_PROP_TYPE_ERR, g_ntpservice_provider[i_paras->index].property_name,
        o_message_jso, type == json_type_null ? RF_VALUE_NULL : dal_json_object_get_str(i_paras->val_jso),
        g_ntpservice_provider[i_paras->index].property_name));

    
    value = json_object_get_int(i_paras->val_jso);
    
    return_val_do_info_if_expr(NTP_MINPOLL > value || NTP_MAXPOLL < value, HTTP_BAD_REQUEST,
        (void)snprintf_s(min_value, sizeof(min_value), sizeof(min_value) - 1, "%d", NTP_MINPOLL);
        (void)snprintf_s(max_value, sizeof(max_value), sizeof(max_value) - 1, "%d", NTP_MAXPOLL);
        (void)create_message_info(MSGID_PROP_VALUE_OUTOFRANGE, g_ntpservice_provider[i_paras->index].property_name,
        o_message_jso, dal_json_object_get_str(i_paras->val_jso), g_ntpservice_provider[i_paras->index].property_name,
        min_value, max_value));
    

    ret = _rf_check_poll_interval(1, value, i_paras->user_data);
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_POLL_INTERVAL_FAILED, g_ntpservice_provider[i_paras->index].property_name,
        o_message_jso, g_ntpservice_provider[i_paras->index].property_name, PROPERTY_NTP_MAX_POLLTNTERVAL,
        PROPERTY_NTP_MIN_POLLTNTERVAL));

    
    ret = dal_get_object_handle_nth(CLASS_NTP, 0, &ntp_handle);
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s, %d:dal_get_object_handle_nth fail.", __FUNCTION__, __LINE__));

    input_list = g_slist_append(input_list, g_variant_new_byte((guchar)value));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, ntp_handle,
        CLASS_NTP, METHOD_NTP_SET_MAX_POLLING_INTERVAL, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
            return HTTP_OK;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, g_ntpservice_provider[i_paras->index].property_name,
                o_message_jso, g_ntpservice_provider[i_paras->index].property_name);
            return HTTP_BAD_REQUEST;

            
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            debug_log(DLOG_ERROR, "%s,%d: NTP enable setting failed.Return code[%d].", __FUNCTION__, __LINE__, ret);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}

gint32 get_ntp_key_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GSList *output_list = NULL;
    guchar key_status;

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = dal_get_object_handle_nth(CLASS_NTP, 0, &obj_handle);
    if (ret != RET_OK) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s:dal_get_object_handle_nth fail,ret=%d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_NTP, METHOD_NTP_GET_GROUP_KEY_STATUS, AUTH_DISABLE, i_paras->user_role_privilege, NULL, &output_list);

    
    if (output_list == NULL) {
        debug_log(DLOG_DEBUG, "%s:output_list is null.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    key_status = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 0));
    uip_free_gvariant_list(output_list);

    switch (ret) {
        case RET_OK:
            if (key_status == 1) {
                *o_result_jso = json_object_new_string(NTP_KEY_UPLOADED);
            } else {
                *o_result_jso = json_object_new_string(NTP_KEY_NOT_UPLOADED);
            }
            return HTTP_OK;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
            return HTTP_FORBIDDEN;

            
        default:
            debug_log(DLOG_DEBUG, "%s: GetNTPGroupKeyStatus.Return code[%d].", __FUNCTION__, ret);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
    
}


gint32 managers_provider_ntpservice_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gboolean ret;
    return_val_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count), HTTP_INTERNAL_SERVER_ERROR);
    ret = redfish_check_manager_uri_valid(i_paras->uri);
    return_val_do_info_if_expr(TRUE != ret, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s,%d:URI check failed. ", __FUNCTION__, __LINE__));

    *prop_provider = g_ntpservice_provider;
    *count = sizeof(g_ntpservice_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}
