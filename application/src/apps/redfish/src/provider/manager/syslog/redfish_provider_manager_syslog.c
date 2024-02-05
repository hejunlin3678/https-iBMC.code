

#include "redfish_provider.h"
#include "../apps/sensor_alarm/inc/remote_syslog.h"
#include "../apps/bmc_global/inc/bmc.h"

gint32 add_all_action_prop(json_object *action_obj, const gchar *action_name, const gchar *slot, const gchar *target,
    const gchar *actioninfo);
LOCAL gint32 __get_manager_syslog_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __get_manager_syslog_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __get_manager_syslog_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __get_manager_syslog_msg_format(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __get_manager_syslog_id_source(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __get_manager_syslog_alarm_severity(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __get_manager_syslog_transmission_protocol(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __get_manager_syslog_authenticate_mode(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __get_manager_syslog_root_certificate(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __get_manager_syslog_client_certificate(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __get_manager_syslog_servers(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __set_manager_syslog_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __set_manager_syslog_msg_format(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __set_manager_syslog_id_source(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __set_manager_syslog_alarm_severity(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __set_manager_syslog_transmission_protocol(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __set_manager_syslog_authenticate_mode(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __set_manager_syslog_servers(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __get_manager_syslog_ation(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __act_syslog_import_root_cert_entry(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __act_syslog_import_client_cert_entry(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __act_syslog_submit_test_event(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _get_syslog_crl_actioninfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_delete_crl_actioninfo_odata(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 act_syslog_import_crl_entry(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 act_syslog_delete_crl_entry(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 _get_manager_syslog_crl_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _get_manager_syslog_crl_start_time(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _get_manager_syslog_crl_expire_time(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_manager_syslog_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, __get_manager_syslog_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, __get_manager_syslog_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SERVICE_ENABLED, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, __get_manager_syslog_enabled, __set_manager_syslog_enabled, NULL, ETAG_FLAG_ENABLE},
    {RF_SYSLOG_MSG_FORMAT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, __get_manager_syslog_msg_format, __set_manager_syslog_msg_format, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SERVER_IDENTITY_SOURCE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, __get_manager_syslog_id_source, __set_manager_syslog_id_source, NULL, ETAG_FLAG_ENABLE},
    {RF_ALARM_SEVERITY, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, __get_manager_syslog_alarm_severity, __set_manager_syslog_alarm_severity, NULL, ETAG_FLAG_ENABLE},
    {RF_TRAN_PROTOCOL, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, __get_manager_syslog_transmission_protocol, __set_manager_syslog_transmission_protocol, NULL, ETAG_FLAG_ENABLE},
    {RF_AUTH_MODE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, __get_manager_syslog_authenticate_mode, __set_manager_syslog_authenticate_mode, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SYSLOG_CRL_ENABLED, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, _get_manager_syslog_crl_enabled, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SYSLOG_CRL_VALID_FROM, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, _get_manager_syslog_crl_start_time, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SYSLOG_CRL_VALID_TO, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, _get_manager_syslog_crl_expire_time, NULL, NULL, ETAG_FLAG_ENABLE},
    
    {RF_ROOT_CERTIFICATE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, __get_manager_syslog_root_certificate, NULL, NULL, ETAG_FLAG_ENABLE},
    {RF_CLIENT_CERTIFICATE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, __get_manager_syslog_client_certificate, NULL, NULL, ETAG_FLAG_ENABLE},
    
    {RFPROP_SYSLOG_SERVERS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, __get_manager_syslog_servers, __set_manager_syslog_servers, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ACTIONS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, __get_manager_syslog_ation, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SYSLOG_IMPORT_ROOT_CERT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, NULL, NULL, __act_syslog_import_root_cert_entry, ETAG_FLAG_ENABLE},
    {RFPROP_SYSLOG_IMPORT_CLIENT_CERT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, 
        NULL, NULL, __act_syslog_import_client_cert_entry, ETAG_FLAG_ENABLE},
    {RFPROP_SYSLOG_IMPORT_CRL, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, NULL, NULL, act_syslog_import_crl_entry, ETAG_FLAG_ENABLE},
    {RFPROP_SYSLOG_DELETE_CRL, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, NULL, NULL, act_syslog_delete_crl_entry, ETAG_FLAG_ENABLE},
    {RFPROP_SYSLOG_TEST, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, NULL, NULL, __act_syslog_submit_test_event, ETAG_FLAG_ENABLE}
};


LOCAL gint32 __get_manager_syslog_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    gchar syslog_uri[MAX_URI_LENGTH] = {0};

    return_val_do_info_if_expr((NULL == o_result_jso) || (NULL == o_message_jso) ||
        (VOS_OK != provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_slot(slot, MAX_RSC_NAME_LEN);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get slot fail", __FUNCTION__));

    ret = snprintf_s(syslog_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, URI_FORMAT_MANAGER_SYSLOG, slot);
    return_val_do_info_if_fail(ret > 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: snprintf_s syslog uri fail", __FUNCTION__));

    *o_result_jso = json_object_new_string((const char *)syslog_uri);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: new syslog uri  json string fail", __FUNCTION__));

    return HTTP_OK;
}


LOCAL gint32 __get_manager_syslog_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    gchar syslog_context[MAX_URI_LENGTH] = {0};

    return_val_do_info_if_expr((NULL == o_result_jso) || (NULL == o_message_jso) ||
        (VOS_OK != provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_slot(slot, MAX_RSC_NAME_LEN);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get slot fail", __FUNCTION__));

    ret = snprintf_s(syslog_context, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, MANAGERS_SYSLOG_METADATA, slot);
    return_val_do_info_if_expr(VOS_OK >= ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: snprintf_s syslog context fail", __FUNCTION__));

    *o_result_jso = json_object_new_string((const char *)syslog_context);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: new syslog context  json string fail", __FUNCTION__));

    return HTTP_OK;
}


LOCAL gint32 __get_manager_syslog_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint8 syslog_enabled = 0;

    return_val_do_info_if_expr((NULL == o_result_jso) || (NULL == o_message_jso) ||
        (VOS_OK != provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    (void)dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_SYSLOG_MGNT_ENABLE_STATE, &syslog_enabled);

    
    return_val_do_info_if_expr((SYSLOG_MGNT_ENABLE != syslog_enabled) && (SYSLOG_MGNT_DISABLE != syslog_enabled),
        HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s: invalid syslog enabled is %d", __FUNCTION__, syslog_enabled));
    
    *o_result_jso = json_object_new_boolean((gint32)syslog_enabled);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: new syslog enabled fail", __FUNCTION__));

    return HTTP_OK;
}


LOCAL gint32 __syslog_msg_fmt_int2str(guint8 msg_format, gchar *msg_format_str, gint32 msg_fmt_len)

{
    INT2STR_MAP_S message_format_arr[] = {{SYSLOG_MGNT_MSG_FORMAT_CUSTOM, RF_SYSLOG_MSG_FORMAT_CUSTOM},
                                          {SYSLOG_MGNT_MSG_FORMAT_RFC3164, RF_SYSLOG_MSG_FORMAT_RFC3164}};
    const gchar *temp_const = NULL;
    gint32 ret;

    return_val_do_info_if_expr(msg_format >= ARRAY_SIZE(message_format_arr), VOS_ERR,
        debug_log(DLOG_ERROR, "%s: invalid message format index(%d)", __FUNCTION__, msg_format));

    temp_const = message_format_arr[msg_format].str_val;
    ret = strncpy_s(msg_format_str, msg_fmt_len, temp_const, strlen(temp_const));
    return_val_do_info_if_expr(ret != VOS_OK, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: strncpy_s msg_format_str failed", __FUNCTION__));

    return VOS_OK;
}


LOCAL gint32 __syslog_msg_fmt_str2int(gchar *msg_format_str, guint8 *msg_format)

{
    INT2STR_MAP_S message_format_arr[] = {{SYSLOG_MGNT_MSG_FORMAT_CUSTOM, RF_SYSLOG_MSG_FORMAT_CUSTOM},
                                          {SYSLOG_MGNT_MSG_FORMAT_RFC3164, RF_SYSLOG_MSG_FORMAT_RFC3164}};
    guint32 i;

    for (i = 0; i < G_N_ELEMENTS(message_format_arr); i++) {
        if (g_strcmp0(message_format_arr[i].str_val, msg_format_str) == 0) {
            *msg_format = message_format_arr[i].int_val;
            return VOS_OK;
        }
    }
    debug_log(DLOG_ERROR, "%s: unknown message format(%s)", __FUNCTION__, msg_format_str);

    return VOS_ERR;
}


LOCAL gint32 __get_manager_syslog_msg_format(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint8 msg_format_id = 0;
    gchar msg_format_str[PROP_VAL_LENGTH] = {0};

    return_val_do_info_if_expr((o_result_jso == NULL) || (o_message_jso == NULL) ||
        (provider_paras_check(i_paras) != VOS_OK),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    
    return_val_do_info_if_expr(!(i_paras->is_satisfy_privi), HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_SYSLOG_MGNT_MSG_FORMAT, &msg_format_id);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get %s prop failed, ret = %d", __FUNCTION__, PROPERTY_SYSLOG_MGNT_MSG_FORMAT, ret));

    ret = __syslog_msg_fmt_int2str(msg_format_id, msg_format_str, PROP_VAL_LENGTH);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: invalid message format(%d)", __FUNCTION__, msg_format_id));

    *o_result_jso = json_object_new_string((const char *)msg_format_str);
    return_val_do_info_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: new message format json string failed.", __FUNCTION__));

    return HTTP_OK;
}


LOCAL gint32 __get_manager_syslog_id_source(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint32 id_source = 0;
    guint8 id_source_uint8;
    gchar id_source_str[PROP_VAL_LENGTH] = {0};

    return_val_do_info_if_expr((NULL == o_result_jso) || (NULL == o_message_jso) ||
        (VOS_OK != provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    (void)dal_get_property_value_uint32(i_paras->obj_handle, PROPERTY_SYSLOG_MGNT_MSG_IDENTITY, &id_source);

    
    id_source_uint8 = (guint8)(id_source - 1);

    ret = server_identity_source_trans(&id_source_uint8, id_source_str, PROP_VAL_LENGTH, IDENTITY_INDEX_TO_STR);
    
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s: invalid identity source is %d", __FUNCTION__, id_source++));
    
    *o_result_jso = json_object_new_string((const char *)id_source_str);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: new id source fail", __FUNCTION__));

    return HTTP_OK;
}


gint32 redfish_prase_severity_str_to_jso(const gchar *alarm_severity_str, json_object **o_result_jso)
{
    if (g_strcmp0(alarm_severity_str, NONE_STRING) != 0) {
        *o_result_jso = json_object_new_string(alarm_severity_str);
        return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s: new object fail.", __FUNCTION__));
    } else {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 __get_manager_syslog_alarm_severity(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint32 alarm_severity = 0;
    gchar alarm_severity_str[PROP_VAL_LENGTH] = {0};

    return_val_do_info_if_expr((NULL == o_result_jso) || (NULL == o_message_jso) ||
        (VOS_OK != provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    (void)dal_get_property_value_uint32(i_paras->obj_handle, PROPERTY_SYSLOG_MGNT_MSG_SEVE_MASK, &alarm_severity);

    ret = redfish_alarm_severities_change((guint8 *)&alarm_severity, alarm_severity_str, PROP_VAL_LENGTH);
    
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s: invalid alarm severity is %d.", __FUNCTION__, alarm_severity));
    

    ret = redfish_prase_severity_str_to_jso((const gchar *)alarm_severity_str, o_result_jso);
    return ret;
}


LOCAL gint32 __syslog_transmission_protocol_trans(guint8 *protocol_int, gchar *protocol_str, gint32 protocol_str_en,
    RF_SYSLOG_TRANS_DIRECTION direction)
{
    INT2STR_MAP_S transmission_protocol_arr[] = {
        {SYSLOG_MGNT_NET_PTL_UDP,        RF_SYSLOG_PROTOCOL_UDP},
        {SYSLOG_MGNT_NET_PTL_TCP,        RF_SYSLOG_PROTOCOL_TCP},
        {SYSLOG_MGNT_NET_PTL_TLS,        RF_SYSLOG_PROTOCOL_TLS},
    };
    guint32 len = G_N_ELEMENTS(transmission_protocol_arr);
    guint32 i = 0;
    guint8 index_temp = 0;
    const gchar *str_tmp = NULL;
    errno_t str_ret = EOK;

    return_val_do_info_if_fail((NULL != protocol_int) && (NULL != protocol_str), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    if (SYSLOG_PROP_INT_TO_STR == direction) {
        index_temp = *protocol_int - 1;
        return_val_do_info_if_fail(index_temp < len, VOS_ERR,
            debug_log(DLOG_ERROR, "invalid transmission protocol index:%d", *protocol_int));

        str_tmp = transmission_protocol_arr[index_temp].str_val;

        str_ret = strncpy_s(protocol_str, protocol_str_en, str_tmp, strlen(str_tmp));
        return_val_do_info_if_fail(str_ret == EOK, VOS_ERR,
            debug_log(DLOG_ERROR, "%s :strncpy_s protocol_str fail", __FUNCTION__));

        return VOS_OK;
    } else if (SYSLOG_PROP_STR_TO_INT == direction) {
        for (i = 0; i < len; i++) {
            if (0 == g_strcmp0(transmission_protocol_arr[i].str_val, protocol_str)) {
                *protocol_int = transmission_protocol_arr[i].int_val;

                return VOS_OK;
            }
        }

        debug_log(DLOG_ERROR, "unknown transmission protocol :%s", protocol_str);
    } else {
        debug_log(DLOG_ERROR, "error use %s:invalid direction param:%d", __FUNCTION__, direction);
    }

    return VOS_ERR;
}


LOCAL gint32 __get_manager_syslog_transmission_protocol(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint8 tra_protocol = 0;
    gchar tra_protocol_str[PROP_VAL_LENGTH] = {0};

    return_val_do_info_if_expr((NULL == o_result_jso) || (NULL == o_message_jso) ||
        (VOS_OK != provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    (void)dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_SYSLOG_MGNT_NET_PROTOCOL, &tra_protocol);

    ret =
        __syslog_transmission_protocol_trans(&tra_protocol, tra_protocol_str, PROP_VAL_LENGTH, SYSLOG_PROP_INT_TO_STR);
    
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s: invalid tra_protocol is %d.", __FUNCTION__, tra_protocol));
    
    *o_result_jso = json_object_new_string((const char *)tra_protocol_str);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: new tra protocol json string fail.", __FUNCTION__));

    return HTTP_OK;
}


LOCAL gint32 __syslog_authenticate_mode_trans(guint8 *auth_mode_int, gchar *auth_mode_str, gint32 auth_str_len,
    RF_SYSLOG_TRANS_DIRECTION direction)

{
    INT2STR_MAP_S authenticate_mode_arr[] = {
        {SYSLOG_MGNT_AUTH_TYPE_ONE,         RF_AUTH_MODE_ONE_WAY},
        {SYSLOG_MGNT_AUTH_TYPE_TWO,        RF_AUTH_MODE_TWO_WAY}
    };
    errno_t str_ret = EOK;
    guint32 len = G_N_ELEMENTS(authenticate_mode_arr);
    guint32 i = 0;
    guint8 index_temp = 0;
    const gchar *str_tmp = NULL;

    if (SYSLOG_PROP_INT_TO_STR == direction) {
        index_temp = *auth_mode_int - 1;
        return_val_do_info_if_fail(index_temp < len, VOS_ERR,
            debug_log(DLOG_ERROR, "invalid authenticate mode index:%d", index_temp));

        str_tmp = authenticate_mode_arr[index_temp].str_val;

        str_ret = strncpy_s(auth_mode_str, auth_str_len, str_tmp, strlen(str_tmp));
        return_val_do_info_if_expr(EOK != str_ret, VOS_ERR,
            debug_log(DLOG_ERROR, "%s :strncpy_s auth_mode_str fail", __FUNCTION__));

        return VOS_OK;
    } else if (SYSLOG_PROP_STR_TO_INT == direction) {
        for (i = 0; i < len; i++) {
            if (0 == g_strcmp0(authenticate_mode_arr[i].str_val, auth_mode_str)) {
                *auth_mode_int = authenticate_mode_arr[i].int_val;

                return VOS_OK;
            }
        }

        debug_log(DLOG_ERROR, "unknown authenticate mode :%s", auth_mode_str);
    }

    return VOS_ERR;
}


LOCAL gint32 __get_manager_syslog_authenticate_mode(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint8 auth_mode = 0;
    gchar auth_mode_str[PROP_VAL_LENGTH] = {0};

    return_val_do_info_if_expr((NULL == o_result_jso) || (NULL == o_message_jso) ||
        (VOS_OK != provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    (void)dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_SYSLOG_MGNT_AUTH_TYPE, &auth_mode);

    ret = __syslog_authenticate_mode_trans(&auth_mode, auth_mode_str, PROP_VAL_LENGTH, SYSLOG_PROP_INT_TO_STR);
    
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s: invalid auth mode is %d", __FUNCTION__, auth_mode));
    
    *o_result_jso = json_object_new_string((const char *)auth_mode_str);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: new auth mode json string fail.", __FUNCTION__));

    return HTTP_OK;
}


LOCAL gint32 __parse_cert_info_from_outputlist(GSList *output_list, RF_SYSLOG_CERTIFICATE *certificate_info)
{
    if ((output_list == NULL) || (certificate_info == NULL)) {
        debug_log(DLOG_ERROR, "%s: invalid param", __FUNCTION__);
        return VOS_ERR;
    }

    
    certificate_info->issuer_str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    certificate_info->subject_str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 1), NULL);
    certificate_info->start_time_str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 2), NULL);
    certificate_info->expiration_str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 3), NULL);
    certificate_info->serial_number_str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 4), NULL);
    certificate_info->sig_algo_str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 5), NULL);
    certificate_info->key_usage_str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 6), NULL);
    certificate_info->key_len = g_variant_get_uint32((GVariant *)g_slist_nth_data(output_list, 7));

    return VOS_OK;
}


LOCAL void __get_cert_info_from_output(GSList *output_list, json_object **object)
{
    RF_SYSLOG_CERTIFICATE certificate_info;
    json_object *issuer = NULL;
    json_object *subject = NULL;
    json_object *start_time = NULL;
    json_object *expiration = NULL;
    json_object *serial_number = NULL;
    gchar serial_number_string[PROP_VAL_LENGTH] = {0};
    gchar *g_serial_number = NULL;
    errno_t str_ret;
    json_object *sig_algo = NULL;
    json_object *key_usage = NULL;
    json_object *key_len = NULL;
    gint32 ret;

    return_do_info_if_expr((output_list == NULL) || (object == NULL),
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    return_do_info_if_expr(json_object_get_type(*object) != json_type_object,
        debug_log(DLOG_ERROR, "%s: is not json object", __FUNCTION__));

    ret = __parse_cert_info_from_outputlist(output_list, &certificate_info);
    return_do_info_if_expr(ret != VOS_OK,
        debug_log(DLOG_ERROR, "%s: __parse_cert_info_from_outputlist failed", __FUNCTION__));

    
    str_ret = strncpy_s(serial_number_string, PROP_VAL_LENGTH, certificate_info.serial_number_str,
        strlen(certificate_info.serial_number_str));
    return_do_info_if_fail(str_ret == EOK, debug_log(DLOG_ERROR, "%s: strncpy_s fail", __FUNCTION__));

    g_serial_number = g_strstrip((gchar *)serial_number_string);

    issuer = json_object_new_string(certificate_info.issuer_str);
    do_info_if_true(issuer == NULL, debug_log(DLOG_ERROR, "%s: new issuer fail", __FUNCTION__));

    subject = json_object_new_string(certificate_info.subject_str);
    do_info_if_true(subject == NULL, debug_log(DLOG_ERROR, "%s: new subject fail", __FUNCTION__));

    start_time = json_object_new_string(certificate_info.start_time_str);
    do_info_if_true(start_time == NULL, debug_log(DLOG_ERROR, "%s: new start_time fail", __FUNCTION__));

    expiration = json_object_new_string(certificate_info.expiration_str);
    do_info_if_true(expiration == NULL, debug_log(DLOG_ERROR, "%s: new expiration fail", __FUNCTION__));

    serial_number = json_object_new_string(g_serial_number);
    do_info_if_true(serial_number == NULL, debug_log(DLOG_ERROR, "%s: new serial_number fail", __FUNCTION__));

    sig_algo = json_object_new_string(certificate_info.sig_algo_str);
    do_info_if_true(sig_algo == NULL, debug_log(DLOG_ERROR, "%s: new sig_algo fail", __FUNCTION__));

    key_usage = json_object_new_string(certificate_info.key_usage_str);
    do_info_if_true(key_usage == NULL, debug_log(DLOG_ERROR, "%s: new key_usage fail", __FUNCTION__));

    key_len = json_object_new_int(certificate_info.key_len);
    do_info_if_true(key_len == NULL, debug_log(DLOG_ERROR, "%s: new key_len fail", __FUNCTION__));

    
    json_object_object_add(*object, RFPROP_CERT_ISSUER_INFO, issuer);
    json_object_object_add(*object, RFPROP_CERT_SUBJECT_INFO, subject);
    json_object_object_add(*object, RFPROP_CERT_START_TIME, start_time);
    json_object_object_add(*object, RFPROP_CERT_EXPIRATION, expiration);
    json_object_object_add(*object, RFPROP_CERT_SN, serial_number);
    json_object_object_add(*object, RFPROP_CERT_SIG_ALGO, sig_algo);
    json_object_object_add(*object, RFPROP_CERT_KEY_USAGE, key_usage);
    json_object_object_add(*object, RFPROP_CERT_KEY_LEN, key_len);
    return;
}


LOCAL gint32 __get_manager_syslog_root_certificate(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;

    return_val_do_info_if_expr((NULL == o_result_jso) || (NULL == o_message_jso) ||
        (VOS_OK != provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    // 权限不足返回500，因为需要填充null
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s: Insufficient permissions", __FUNCTION__));

    input_list = g_slist_append(input_list, g_variant_new_byte(SYSLOG_CERT_TYPE_HOST));

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_SYSLOG_CONFIG, METHOD_SYSLOG_MGNT_GET_CERT_INFO, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, &output_list);

    uip_free_gvariant_list(input_list);
    if (VOS_OK == ret) {
        *o_result_jso = json_object_new_object();
        return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s: new object_fail", __FUNCTION__);
            uip_free_gvariant_list(output_list));

        __get_cert_info_from_output(output_list, o_result_jso);
    } else {
        debug_log(DLOG_MASS, "%s: get root certificate return %d", __FUNCTION__, ret);

        uip_free_gvariant_list(output_list);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    uip_free_gvariant_list(output_list);

    return HTTP_OK;
}


LOCAL gint32 __get_manager_syslog_client_certificate(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;

    return_val_do_info_if_expr((NULL == o_result_jso) || (NULL == o_message_jso) ||
        (VOS_OK != provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    // 权限不足返回500，因为需要填充null
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s: Insufficient permissions", __FUNCTION__));

    input_list = g_slist_append(input_list, g_variant_new_byte(SYSLOG_CERT_TYPE_CLIENT));

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_SYSLOG_CONFIG, METHOD_SYSLOG_MGNT_GET_CERT_INFO, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, &output_list);

    uip_free_gvariant_list(input_list);
    if (VOS_OK == ret) {
        *o_result_jso = json_object_new_object();
        return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s: new object_fail", __FUNCTION__);
            uip_free_gvariant_list(output_list));

        __get_cert_info_from_output(output_list, o_result_jso);
    } else {
        debug_log(DLOG_MASS, "%s: get client certificate return %d", __FUNCTION__, ret);

        uip_free_gvariant_list(output_list);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    uip_free_gvariant_list(output_list);

    return HTTP_OK;
}


LOCAL void __syslog_item_get_logtype_json_array(guint8 log_type, json_object **log_type_arr)
{
    gint32 ret = 0;
    json_object *log_type_sg = NULL;
    INT2STR_MAP_S logtype_arr[] = {
        {SYSLOG_MGNT_SRC_OP,        RF_SYSLOG_ITEM_LOG_OP},
        {SYSLOG_MGNT_SRC_SEC,       RF_SYSLOG_ITEM_LOG_SEC},
        {SYSLOG_MGNT_SRC_SEL,       RF_SYSLOG_ITEM_LOG_SEL},
    };
    guint32 i;
    guint32 len = G_N_ELEMENTS(logtype_arr);

    return_do_info_if_expr(NULL == log_type_arr, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    
    for (i = 0; i < len; i++) {
        if (log_type & (guint32)logtype_arr[i].int_val) {
            log_type_sg = json_object_new_string(logtype_arr[i].str_val);

            ret = json_object_array_add(*log_type_arr, log_type_sg);
            do_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s: array add op log fail", __FUNCTION__);
                (void)json_object_put(log_type_sg));

            log_type_sg = NULL;
        }
    }
    

    return;
}


LOCAL void __get_each_syslog_server_info(guint32 i, json_object **array)
{
    OBJ_HANDLE obj_handle = 0;
    json_object *object = NULL;
    json_object *jso_prop = NULL;
    RF_SYSLOG_ITEM syslog_item;
    guint8 ser_index = 0;
    gint32 ret;

    return_do_info_if_expr(NULL == array, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    (void)dal_get_object_handle_nth(CLASS_SYSLOG_ITEM_CONFIG, i, &obj_handle);

    object = json_object_new_object();
    return_do_info_if_expr(NULL == object, debug_log(DLOG_ERROR, "%s: new object fail,i = %d", __FUNCTION__, i));

    
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_REMOTE_SYSLOG_INDEX, &ser_index);
    ret = snprintf_s(syslog_item.member_id, PROP_VAL_LENGTH, PROP_VAL_LENGTH - 1, "%u", ser_index);
    if (VOS_OK >= ret) {
        debug_log(DLOG_ERROR, "%s: snprintf_s member id fail", __FUNCTION__);
    } else {
        jso_prop = json_object_new_string((const char *)syslog_item.member_id);
        do_info_if_true(NULL == jso_prop, debug_log(DLOG_ERROR, "%s: new member id fail,i : %d", __FUNCTION__, i));
    }
    json_object_object_add(object, MEMBER_ID, jso_prop);
    jso_prop = NULL;

    
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_REMOTE_SYSLOG_ENABLE_STATE, &syslog_item.enabled);
    if ((SYSLOG_MGNT_ENABLE == syslog_item.enabled) || (SYSLOG_MGNT_DISABLE == syslog_item.enabled)) {
        jso_prop = json_object_new_boolean((gint32)syslog_item.enabled);
        
        do_info_if_true(NULL == jso_prop, debug_log(DLOG_DEBUG, "%s: new enabled fail,i : %d", __FUNCTION__, i));
        
    }
    json_object_object_add(object, ENABLED_STRING, jso_prop);
    jso_prop = NULL;

    
    
    (void)dal_get_property_value_string(obj_handle, PROPERTY_REMOTE_SYSLOG_DEST_ADDR, syslog_item.address,
        sizeof(syslog_item.address));
    

    jso_prop = json_object_new_string((const char *)syslog_item.address);
    do_info_if_true(NULL == jso_prop, debug_log(DLOG_ERROR, "%s: new address fail,i : %d", __FUNCTION__, i));

    json_object_object_add(object, RF_PROPERTY_ADDR, jso_prop);
    jso_prop = NULL;

    
    (void)dal_get_property_value_uint32(obj_handle, PROPERTY_REMOTE_SYSLOG_DEST_PORT, &syslog_item.port);

    jso_prop = json_object_new_int((gint32)syslog_item.port);
    do_info_if_true(NULL == jso_prop, debug_log(DLOG_ERROR, "%s: new port fail,i : %d", __FUNCTION__, i));

    json_object_object_add(object, RFPROP_SYSLOG_ITEM_PORT, jso_prop);
    jso_prop = NULL;

    
    (void)dal_get_property_value_uint32(obj_handle, PROPERTY_REMOTE_SYSLOG_LOG_SRC_MASK, &syslog_item.log_type_int);
    jso_prop = json_object_new_array();
    if (NULL == jso_prop) {
        debug_log(DLOG_ERROR, "%s: new log_type array fail,i : %d", __FUNCTION__, i);
    } else {
        __syslog_item_get_logtype_json_array(syslog_item.log_type_int, &jso_prop);
    }
    json_object_object_add(object, RFPROP_SYSLOG_ITEM_LOG_TYPE, jso_prop);

    ret = json_object_array_add(*array, object);
    do_info_if_true(VOS_OK != ret, debug_log(DLOG_ERROR, "%s: array add fail,i : %d", __FUNCTION__, i);
        (void)json_object_put(object));

    return;
}


LOCAL gint32 __get_manager_syslog_servers(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint32 server_number = 0;
    guint32 i;

    return_val_do_info_if_expr((NULL == o_result_jso) || (NULL == o_message_jso) ||
        (VOS_OK != provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dfl_get_object_count(CLASS_SYSLOG_ITEM_CONFIG, &server_number);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get syslog item config count fail", __FUNCTION__));

    *o_result_jso = json_object_new_array();
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: new array fail", __FUNCTION__));

    for (i = 0; i < server_number; i++) {
        __get_each_syslog_server_info(i, o_result_jso);
    }

    return HTTP_OK;
}


gint32 add_all_action_prop(json_object *action_obj, const gchar *action_name, const gchar *slot, const gchar *target,
    const gchar *actioninfo)
{
    gint32 ret;
    gchar action_name_pointer[PROP_VAL_LENGTH] = {0};
    gchar target_uri[MAX_URI_LENGTH] = {0};
    gchar action_info_uri[MAX_URI_LENGTH] = {0};
    json_object *target_jso = NULL;
    json_object *action_info_jso = NULL;
    json_object *object = NULL;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    ret = snprintf_s(target_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, target, slot);
#pragma GCC diagnostic pop

    return_val_do_info_if_expr(VOS_OK >= ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: snprintf_s %s target failed", __FUNCTION__, action_name));

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    ret = snprintf_s(action_info_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, actioninfo, slot);
#pragma GCC diagnostic pop

    return_val_do_info_if_expr(VOS_OK >= ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: snprintf_s %s action info failed", __FUNCTION__, action_name));

    ret = snprintf_s(action_name_pointer, PROP_VAL_LENGTH, PROP_VAL_LENGTH - 1, "#%s", action_name);
    return_val_do_info_if_expr(VOS_OK >= ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: snprintf_s %s action name pointer failed", __FUNCTION__, action_name));

    target_jso = json_object_new_string((const char *)target_uri);
    return_val_do_info_if_expr(NULL == target_jso, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: new %s target json failed", __FUNCTION__, action_name));

    action_info_jso = json_object_new_string((const char *)action_info_uri);
    return_val_do_info_if_expr(NULL == action_info_jso, VOS_ERR, (void)json_object_put(target_jso);
        debug_log(DLOG_ERROR, "%s: new %s action info json failed", __FUNCTION__, action_name));

    object = json_object_new_object();
    return_val_do_info_if_expr(NULL == object, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: new %s object json failed", __FUNCTION__, action_name);
        (void)json_object_put(target_jso); (void)json_object_put(action_info_jso));
    json_object_object_add(object, RFPROP_TARGET, target_jso);
    json_object_object_add(object, RFPROP_ACTION_INFO, action_info_jso);

    json_object_object_add(action_obj, (const gchar *)action_name_pointer, object);

    return VOS_OK;
}


LOCAL gint32 __get_manager_syslog_ation(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar slot[PROP_VAL_LENGTH] = {0};

    return_val_do_info_if_expr((o_result_jso == NULL) || (o_message_jso == NULL) ||
        (provider_paras_check(i_paras) != VOS_OK),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: new action object fail", __FUNCTION__));

    
    ret = redfish_get_board_slot(slot, PROP_VAL_LENGTH);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get slot fail", __FUNCTION__));

    // 生成导入根证书的action的对象
    ret = add_all_action_prop(*o_result_jso, RFPROP_SYSLOG_IMPORT_ROOT_CERT, slot, RFACTION_SYSLOG_IMPORT_ROOT_CERT,
        RFACTION_INFO_SYSLOG_IMPORT_ROOT_CERT);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: add root cert object fail", __FUNCTION__));

    // 生成导入本地证书的action的对象
    ret = add_all_action_prop(*o_result_jso, RFPROP_SYSLOG_IMPORT_CLIENT_CERT, slot, RFACTION_SYSLOG_IMPORT_CLIENT_CERT,
        RFACTION_INFO_SYSLOG_IMPORT_CLIENT_CERT);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: add client cert object fail", __FUNCTION__));

    // 生成test的action对象
    ret = add_all_action_prop(*o_result_jso, RFPROP_SYSLOG_TEST, slot, RFACTION_SYSLOG_TEST, RFACTION_INFO_SYSLOG_TEST);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: add test object fail", __FUNCTION__));

    
    ret = add_all_action_prop(*o_result_jso, RFPROP_SYSLOG_IMPORT_CRL, slot, RFACTION_SYSLOG_IMPORT_CRL,
        RFACTION_INFO_SYSLOG_IMPORT_CRL);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: add import crl obhject fail", __FUNCTION__));

    
    ret = add_all_action_prop(*o_result_jso, RFPROP_SYSLOG_DELETE_CRL, slot, RFACTION_SYSLOG_DELETE_CRL,
        RFACTION_INFO_SYSLOG_DELETE_CRL);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: add delete crl object fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 __set_manager_syslog_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint8 enabled;
    GSList *input_list = NULL;

    return_val_do_info_if_expr((NULL == o_message_jso) || (VOS_OK != provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    enabled = (guint8)json_object_get_boolean(i_paras->val_jso);

    input_list = g_slist_append(input_list, g_variant_new_byte(enabled));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_SYSLOG_CONFIG, METHOD_SYSLOG_MGNT_SET_ENABLE_STATE, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
            break;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_SERVICE_ENABLED, o_message_jso,
                RFPROP_SERVICE_ENABLED);
            return HTTP_BAD_REQUEST;

        default:
            debug_log(DLOG_ERROR, "%s: set service enabled return %d", __FUNCTION__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 __set_manager_syslog_msg_format(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int ret;
    guint8 msg_format_id = 0;
    const gchar *msg_format = NULL;
    gchar msg_format_str[PROP_VAL_LENGTH] = {0};
    GSList *input_list = NULL;

    return_val_do_info_if_expr((o_message_jso == NULL) || (provider_paras_check(i_paras) != VOS_OK),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    msg_format = dal_json_object_get_str(i_paras->val_jso);
    return_val_do_info_if_expr(msg_format == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get json content failed.", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    ret = strncpy_s(msg_format_str, PROP_VAL_LENGTH, msg_format, strlen(msg_format));
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: strncpy_s failed.", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    ret = __syslog_msg_fmt_str2int(msg_format_str, &msg_format_id);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: invalid message format str(%s)", __FUNCTION__, msg_format_str);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    input_list = g_slist_append(input_list, g_variant_new_byte(msg_format_id));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_SYSLOG_CONFIG, METHOD_SYSLOG_MGNT_SET_MSG_FORMAT, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
            break;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RF_SYSLOG_MSG_FORMAT, o_message_jso,
                RF_SYSLOG_MSG_FORMAT);
            return HTTP_BAD_REQUEST;

        default:
            debug_log(DLOG_ERROR, "%s: set message format failed, ret = %d", __FUNCTION__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 __set_manager_syslog_id_source(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint8 id_source = 0;
    const gchar *id_source_str = NULL;
    GSList *input_list = NULL;
    gchar id_source_string[PROP_VAL_LENGTH] = {0};
    errno_t str_ret;

    return_val_do_info_if_expr((NULL == o_message_jso) || (VOS_OK != provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    id_source_str = dal_json_object_get_str(i_paras->val_jso);

    str_ret = strncpy_s(id_source_string, PROP_VAL_LENGTH, id_source_str, strlen(id_source_str));
    return_val_do_info_if_fail(EOK == str_ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail.", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    ret = server_identity_source_trans(&id_source, id_source_string, sizeof(id_source_string), IDENTITY_STR_TO_INDEX);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "%s: identity string to int fail", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, RFPROP_SERVER_IDENTITY_SOURCE, o_message_jso, id_source_str,
        RFPROP_SERVER_IDENTITY_SOURCE));
    guint8 software_type = 0;

    (void)dal_get_software_type(&software_type);
    if (MGMT_SOFTWARE_TYPE_EM == software_type) {
        if ((id_source > TRAP_ID_CHASSIS_SERIAL_NUMBER) || (id_source < TRAP_ID_HOSTNAME)) {
            debug_log(DLOG_ERROR, "input para invalid: trap_identity %d", id_source);
            return HTTP_BAD_REQUEST;
        }
    } else {
        if (id_source > TRAP_ID_HOSTNAME) {
            debug_log(DLOG_ERROR, "input para invalid: trap_identity %d", id_source);
            return HTTP_BAD_REQUEST;
        }
    }
    // 加1是因为此处的主机标识是从1开始的，而转换函数中的是从0 开始的
    id_source = id_source + 1;

    input_list = g_slist_append(input_list, g_variant_new_uint32((guint32)id_source));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_SYSLOG_CONFIG, METHOD_SYSLOG_MGNT_SET_MSG_IDENTITY, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, NULL);

    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
            break;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_SERVER_IDENTITY_SOURCE, o_message_jso,
                RFPROP_SERVER_IDENTITY_SOURCE);
            return HTTP_BAD_REQUEST;

        case VOS_ERR_NOTSUPPORT:
            (void)create_message_info(MSGID_SERVER_ID_MODIFY_UNSUPPORTED, RFPROP_SERVER_IDENTITY_SOURCE, o_message_jso);
            return HTTP_BAD_REQUEST;

        default:
            debug_log(DLOG_ERROR, "%s: set syslog Server Identity Source return %d", __FUNCTION__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


gint32 redfish_get_severity_from_jso(json_object *jso_obj, gchar *alarm_severity_str, size_t buffer_len)
{
    const gchar *input_jso_str = NULL;
    errno_t ret;

    if (jso_obj == NULL) {
        input_jso_str = NONE_STRING;
    } else {
        input_jso_str = dal_json_object_get_str(jso_obj);
        if (NULL == input_jso_str) {
            return VOS_ERR;
        }
    }

    ret = strncpy_s(alarm_severity_str, buffer_len, input_jso_str, strlen(input_jso_str));
    return ret;
}


LOCAL gint32 __set_manager_syslog_alarm_severity(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint8 alarm_severity = 0;
    gchar alarm_severity_str[PROP_VAL_LENGTH] = {0};
    errno_t temp_str;
    GSList *input_list = NULL;

    return_val_do_info_if_expr((NULL == o_message_jso) || (VOS_OK != provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    temp_str = redfish_get_severity_from_jso(i_paras->val_jso, alarm_severity_str, sizeof(alarm_severity_str));
    return_val_do_info_if_fail(EOK == temp_str, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret: %d.", __FUNCTION__, temp_str);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    ret = redfish_alarm_severities_change(&alarm_severity, alarm_severity_str, 0);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: invalid alarm serverty str is %s", __FUNCTION__, alarm_severity_str);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    input_list = g_slist_append(input_list, g_variant_new_uint32((guint32)alarm_severity));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_SYSLOG_CONFIG, METHOD_SYSLOG_MGNT_SET_MSG_SEVE_MASK, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, NULL);

    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
            break;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RF_ALARM_SEVERITY, o_message_jso, RF_ALARM_SEVERITY);
            return HTTP_BAD_REQUEST;

        default:
            debug_log(DLOG_ERROR, "%s: set syslog Server Identity Source return %d", __FUNCTION__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 __set_manager_syslog_transmission_protocol(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint8 tra_protocol = 0;
    const gchar *tra_protocol_str = NULL;
    GSList *input_list = NULL;
    errno_t str_ret;
    gchar tra_protocol_string[PROP_VAL_LENGTH] = {0};

    return_val_do_info_if_expr((NULL == o_message_jso) || (VOS_OK != provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    tra_protocol_str = dal_json_object_get_str(i_paras->val_jso);
    
    return_val_do_info_if_expr(NULL == tra_protocol_str, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get json content fail.", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    

    str_ret = strncpy_s(tra_protocol_string, PROP_VAL_LENGTH, tra_protocol_str, strlen(tra_protocol_str));
    return_val_do_info_if_fail(EOK == str_ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail.", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    ret = __syslog_transmission_protocol_trans(&tra_protocol, tra_protocol_string, 0, SYSLOG_PROP_STR_TO_INT);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "%s: identity string to int fail", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, RF_TRAN_PROTOCOL, o_message_jso, tra_protocol_str,
        RF_TRAN_PROTOCOL));

    input_list = g_slist_append(input_list, g_variant_new_byte(tra_protocol));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_SYSLOG_CONFIG, METHOD_SYSLOG_MGNT_SET_NET_PROTOCOL, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, NULL);

    uip_free_gvariant_list(input_list);
    switch (ret) {
        case VOS_OK:
            break;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RF_TRAN_PROTOCOL, o_message_jso, RF_TRAN_PROTOCOL);
            return HTTP_BAD_REQUEST;

        default:
            debug_log(DLOG_ERROR, "%s: set Transmission Protocol return %d", __FUNCTION__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 __set_manager_syslog_authenticate_mode(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint8 auth_mode = 0;
    const gchar *auth_mode_str = NULL;
    GSList *input_list = NULL;
    errno_t str_ret;
    gchar auth_mode_string[PROP_VAL_LENGTH] = {0};

    return_val_do_info_if_expr((NULL == o_message_jso) || (VOS_OK != provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    auth_mode_str = dal_json_object_get_str(i_paras->val_jso);
    
    return_val_do_info_if_expr(NULL == auth_mode_str, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get json content fail.", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    

    str_ret = strncpy_s(auth_mode_string, PROP_VAL_LENGTH, auth_mode_str, strlen(auth_mode_str));
    return_val_do_info_if_fail(EOK == str_ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail.", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    ret = __syslog_authenticate_mode_trans(&auth_mode, auth_mode_string, 0, SYSLOG_PROP_STR_TO_INT);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "%s: identity string to int fail", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, RF_AUTH_MODE, o_message_jso, auth_mode_str, RF_AUTH_MODE));

    input_list = g_slist_append(input_list, g_variant_new_byte(auth_mode));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_SYSLOG_CONFIG, METHOD_SYSLOG_MGNT_SET_AUTH_TYPE, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, NULL);

    uip_free_gvariant_list(input_list);
    switch (ret) {
        case VOS_OK:
            break;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RF_AUTH_MODE, o_message_jso, RF_AUTH_MODE);
            return HTTP_BAD_REQUEST;

        default:
            debug_log(DLOG_ERROR, "%s: set AuthenticateMode return %d", __FUNCTION__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 __syslog_set_item_enabled(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, json_object *object,
    json_object **message_array)
{
    gint32 ret;
    gint32 result = VOS_ERR;
    json_object *message = NULL;
    gint32 enabled = 0;
    GSList *input_list = NULL;

    return_val_do_info_if_expr((NULL == message_array) || (VOS_OK != provider_paras_check(i_paras)) || (NULL == object),
        result, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    ret = get_element_boolean(object, ENABLED_STRING, &enabled);
    
    return_val_if_fail(TRUE == ret, result);

    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)enabled));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SYSLOG_ITEM_CONFIG, METHOD_REMOTE_SYSLOG_SET_ENABLE_STATE, AUTH_DISABLE, 0, input_list, NULL);

    uip_free_gvariant_list(input_list);
    switch (ret) {
        case VOS_OK:
            result = VOS_OK;
            break;

        default:
            debug_log(DLOG_ERROR, "%s: set item Enabled return %d", __FUNCTION__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message);
            (void)json_object_array_add(*message_array, message);
    }

    return result;
}


LOCAL gint32 __syslog_set_item_address(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, json_object *object,
    json_object **message_array, gint32 i)
{
    gint32 ret;
    gint32 result = VOS_ERR;
    json_object *message = NULL;
    const gchar *address = NULL;
    GSList *input_list = NULL;
    gchar json_pointer[PROP_VAL_LENGTH] = {0};

    return_val_do_info_if_expr((NULL == message_array) || (VOS_OK != provider_paras_check(i_paras)) || (NULL == object),
        result, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    ret = get_element_str(object, RF_PROPERTY_ADDR, &address);
    
    return_val_if_fail(TRUE == ret, result);

    
    ret = snprintf_s(json_pointer, PROP_VAL_LENGTH, PROP_VAL_LENGTH - 1, "SyslogServers/%d/Address", i);
    return_val_do_info_if_expr(VOS_OK >= ret, result,
        debug_log(DLOG_ERROR, "%s: snprintf_s address json pointer fail", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message);
        (void)json_object_array_add(*message_array, message));

    input_list = g_slist_append(input_list, g_variant_new_string(address));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SYSLOG_ITEM_CONFIG, METHOD_REMOTE_SYSLOG_SET_DEST_ADDR, AUTH_DISABLE, 0, input_list, NULL);

    uip_free_gvariant_list(input_list);
    switch (ret) {
        case VOS_OK:
            result = VOS_OK;
            break;

        // 返回-1或-2，代表非法的地址
        case RF_ERROR:
        case RET_ERR_INVALID_PARAM:
            (void)create_message_info(MSGID_INVALID_SYSLOG_ADDRESS, (const gchar *)json_pointer, &message, address,
                (const gchar *)json_pointer);
            (void)json_object_array_add(*message_array, message);
            break;

        default:
            debug_log(DLOG_ERROR, "%s: set %d item Address return %d", __FUNCTION__, i, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message);
            (void)json_object_array_add(*message_array, message);
    }

    return result;
}


LOCAL gint32 __syslog_set_item_port(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, json_object *object,
    json_object **message_array, gint32 i)
{
    gint32 ret;
    gint32 result = VOS_ERR;
    json_object *message = NULL;
    json_object *port_jso = NULL;
    gint32 server_port;
    GSList *input_list = NULL;
    gchar json_pointer[PROP_VAL_LENGTH] = {0};

    return_val_do_info_if_expr((NULL == message_array) || (VOS_OK != provider_paras_check(i_paras)) || (NULL == object),
        result, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    ret = json_object_object_get_ex(object, RFPROP_SYSLOG_ITEM_PORT, &port_jso);
    return_val_if_fail(TRUE == ret, result);

    if (json_type_int != json_object_get_type(port_jso)) {
        ret = snprintf_s(json_pointer, PROP_VAL_LENGTH, PROP_VAL_LENGTH - 1, "SyslogServers/%d/Port", i);
        return_val_do_info_if_expr(VOS_OK >= ret, result, debug_log(DLOG_ERROR, "%s: snprintf_s fail.", __FUNCTION__));

        (void)create_message_info(MSGID_PROP_ITEM_TYPE_ERR, (const gchar *)json_pointer, &message,
            dal_json_object_get_str(port_jso), (const gchar *)json_pointer);
        (void)json_object_array_add(*message_array, message);

        return result;
    }

    server_port = json_object_get_int(port_jso);

    input_list = g_slist_append(input_list, g_variant_new_uint32((guint32)server_port));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SYSLOG_ITEM_CONFIG, METHOD_REMOTE_SYSLOG_SET_DEST_PORT, AUTH_DISABLE, 0, input_list, NULL);

    uip_free_gvariant_list(input_list);
    switch (ret) {
        case VOS_OK:
            result = VOS_OK;
            break;

        default:
            debug_log(DLOG_ERROR, "%s: set %d item Port return %d", __FUNCTION__, i, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message);
            (void)json_object_array_add(*message_array, message);
    }

    return result;
}


LOCAL gint32 __get_logype_by_array(json_object *logtype_arr, guint32 *log_type, json_object **message_array,
    gint32 ser_index)
{
    gint32 array_len;
    gint32 i;
    json_object *element_jso = NULL;
    const gchar *log_type_str = NULL;
    json_object *message = NULL;
    gint32 ret = 0;
    gchar json_pointer[PROP_VAL_LENGTH] = {0};

    return_val_do_info_if_expr((NULL == log_type) || (NULL == logtype_arr), VOS_ERR,
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    *log_type = 0;

    array_len = json_object_array_length(logtype_arr);

    for (i = 0; i < array_len; i++) {
        element_jso = json_object_array_get_idx(logtype_arr, i);
        log_type_str = dal_json_object_get_str(element_jso);

        ret =
            snprintf_s(json_pointer, PROP_VAL_LENGTH, PROP_VAL_LENGTH - 1, "SyslogServers/%d/LogType/%d", ser_index, i);
        continue_do_info_if_fail(VOS_OK < ret, debug_log(DLOG_ERROR, "%s: snprintf_s fail.", __FUNCTION__));

        if (0 == g_strcmp0(log_type_str, RF_SYSLOG_ITEM_LOG_OP)) {
            *log_type = *log_type + SYSLOG_MGNT_SRC_OP;
        } else if (0 == g_strcmp0(log_type_str, RF_SYSLOG_ITEM_LOG_SEC)) {
            *log_type = *log_type + SYSLOG_MGNT_SRC_SEC;
        } else if (0 == g_strcmp0(log_type_str, RF_SYSLOG_ITEM_LOG_SEL)) {
            *log_type = *log_type + SYSLOG_MGNT_SRC_SEL;
        } else if (0 == g_strcmp0(JSON_NULL_OBJECT_STR, log_type_str)) {
            (void)create_message_info(MSGID_PROP_ITEM_TYPE_ERR, (const gchar *)json_pointer, &message, log_type_str,
                (const gchar *)json_pointer);
            (void)json_object_array_add(*message_array, message);

            return VOS_ERR;
        } else {
            (void)create_message_info(MSGID_PROP_ITEM_NOT_IN_LIST, (const gchar *)json_pointer, &message, log_type_str,
                (const gchar *)json_pointer);
            (void)json_object_array_add(*message_array, message);

            return VOS_ERR;
        }

        (void)memset_s(json_pointer, PROP_VAL_LENGTH, 0, PROP_VAL_LENGTH);
    }

    return VOS_OK;
}


LOCAL gint32 __syslog_set_item_log_type(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, json_object *object,
    json_object **message_array, gint32 i)
{
    gint32 ret;
    gint32 result = VOS_ERR;
    json_object *message = NULL;
    json_object *log_type_array = NULL;
    gint32 logtype_array_len = 0;
    gchar json_pointer_array[PROP_VAL_LENGTH] = {0};
    gchar json_pointer[PROP_VAL_LENGTH] = {0};
    guint32 log_type = 0;
    gint32 array_duplicate_index = 0;
    GSList *input_list = NULL;

    return_val_do_info_if_expr((NULL == message_array) || (VOS_OK != provider_paras_check(i_paras)) || (NULL == object),
        result, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    
    ret = snprintf_s(json_pointer_array, PROP_VAL_LENGTH, PROP_VAL_LENGTH - 1, "SyslogServers/%d/LogType", i);
    return_val_do_info_if_expr(VOS_OK >= ret, result,
        debug_log(DLOG_ERROR, "%s: snprintf_s log type json pointer fail", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message);
        (void)json_object_array_add(*message_array, message));

    if (json_object_object_get_ex(object, RFPROP_SYSLOG_ITEM_LOG_TYPE, &log_type_array)) {
        logtype_array_len = json_object_array_length(log_type_array);

        
        return_val_do_info_if_expr(logtype_array_len > 3, VOS_ERR,
            (void)create_message_info(MSGID_ARRAY_ITEM_EXCEED, (const gchar *)json_pointer_array, &message,
            (const gchar *)json_pointer_array);
            (void)json_object_array_add(*message_array, message));

        
        ret = json_array_duplicate_check(log_type_array, &array_duplicate_index);
        return_val_do_info_if_expr(RF_OK == ret, result,
            (void)snprintf_s(json_pointer, PROP_VAL_LENGTH, PROP_VAL_LENGTH - 1, "SyslogServers/%d/LogType/%d", i,
            array_duplicate_index);
            (void)create_message_info(MSGID_PROP_ITEM_DUPLICATE, json_pointer, &message, json_pointer);
            (void)json_object_array_add(*message_array, message));

        // 从log type 数组中获取值，失败生成消息
        ret = __get_logype_by_array(log_type_array, &log_type, message_array, i);
        return_val_if_fail(VOS_OK == ret, result);

        input_list = g_slist_append(input_list, g_variant_new_uint32(log_type));

        ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
            CLASS_SYSLOG_ITEM_CONFIG, METHOD_REMOTE_SYSLOG_SET_LOG_SRC_MASK, AUTH_DISABLE, 0, input_list, NULL);

        uip_free_gvariant_list(input_list);

        switch (ret) {
            case VOS_OK:
                result = VOS_OK;
                break;

            default:
                debug_log(DLOG_ERROR, "%s: set syslog item log type return %d. i : %d", __FUNCTION__, ret, i);
                (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message);
                (void)json_object_array_add(*message_array, message);
        }
    }

    // 没有LogType的key直接返回
    return result;
}


LOCAL gint32 __syslog_set_single_server_item(PROVIDER_PARAS_S *i_paras, json_object *object,
    json_object **message_array, gint32 ser_index)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 result = VOS_ERR;
    gint32 ret;

    return_val_do_info_if_expr((NULL == message_array) || (VOS_OK != provider_paras_check(i_paras)) || (NULL == object),
        VOS_ERR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    (void)dal_get_object_handle_nth(CLASS_SYSLOG_ITEM_CONFIG, ser_index, &obj_handle);

    
    ret = __syslog_set_item_enabled(i_paras, obj_handle, object, message_array);
    do_info_if_true(VOS_OK == ret, (result = VOS_OK));

    
    ret = __syslog_set_item_address(i_paras, obj_handle, object, message_array, ser_index);
    do_info_if_true(VOS_OK == ret, (result = VOS_OK));

    
    ret = __syslog_set_item_port(i_paras, obj_handle, object, message_array, ser_index);
    do_info_if_true(VOS_OK == ret, (result = VOS_OK));

    
    ret = __syslog_set_item_log_type(i_paras, obj_handle, object, message_array, ser_index);
    do_info_if_true(VOS_OK == ret, (result = VOS_OK));

    return result;
}


LOCAL gint32 __set_manager_syslog_servers(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint32 array_len;
    guint32 syslog_item_count = 0;
    gint32 i;
    json_object *object = NULL;
    gint32 result = HTTP_BAD_REQUEST;
    gint32 null_object_count = 0; // 空对象的个数
    json_object *message = NULL;

    return_val_do_info_if_expr((NULL == o_message_jso) || (VOS_OK != provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_SECURITYMGNT, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_SYSLOG_SERVERS, o_message_jso,
        RFPROP_SYSLOG_SERVERS));

    
    ret = dfl_get_object_count(CLASS_SYSLOG_ITEM_CONFIG, &syslog_item_count);
    return_val_do_info_if_fail(DFL_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get syslog item count fail.", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    array_len = json_object_array_length(i_paras->val_jso);
    // 数组长度越界的错误
    return_val_do_info_if_expr(array_len > (gint32)syslog_item_count, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_ARRAY_ITEM_EXCEED, RFPROP_SYSLOG_SERVERS, o_message_jso,
        RFPROP_SYSLOG_SERVERS));

    
    *o_message_jso = json_object_new_array();
    return_val_do_info_if_expr(NULL == *o_message_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: new message array fail.", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    for (i = 0; i < array_len; i++) {
        object = json_object_array_get_idx(i_paras->val_jso, i);
        
        continue_if_expr(NULL == object);
        
        
        if (0 == g_strcmp0(JSON_NULL_OBJECT_STR, dal_json_object_to_json_string(object))) {
            if (NULL == object->_userdata) {
                null_object_count++;
            }

            continue;
        }

        ret = __syslog_set_single_server_item(i_paras, object, o_message_jso, i);
        do_info_if_true(VOS_OK == ret, (result = HTTP_OK));
    }

    
    return_val_do_info_if_expr(null_object_count == array_len, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_ARRAY_REQUIRED_ITEM, RFPROP_SYSLOG_SERVERS, &message, RFPROP_SYSLOG_SERVERS);
        (void)json_object_array_add(*o_message_jso, message));

    return result;
}


LOCAL gint32 __parse_cert_result_code(gint32 method_ret, json_object **message_info, gint32 type)
{
    gint32 result = HTTP_BAD_REQUEST;

    return_val_do_info_if_expr(NULL == message_info, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    switch (method_ret) {
        case VOS_OK:
            result = HTTP_OK;
            (void)create_message_info(MSGID_CERT_IMPORT_OK_NONE, NULL, message_info);
            break;

        
        case RF_ERROR:
            // 返回-1，表示本地证书的密码错误，或者路径不合法
        case SSL_INVALID_DATA:
            // 证书文件不存在或者检查本地证书失败
        case SSL_NO_DATA:
            // 对于type是URI的场景，路径不是以/tmp  打头
        case SSL_INTERNAL_ERROR:
            // 密钥用法不满足
        case VOS_ERR_MA_KEY_USAGE_CERT_SIGN_ERR:
            // 基本约束不满足
        case VOS_ERR_MA_BASIC_CONSTRAINTS_ERR:
            // 证书文件太大等场景
        case SSL_INVALID_PARAMETER:
            // 无效的证书文件
            if (SYSLOG_CERT_TYPE_HOST == type) {
                (void)create_message_info(MSGID_CERT_IMPORT_FAILED, NULL, message_info);
            } else {
                (void)create_message_info(MSGID_ENCRYPTED_CERT_IMPORT_FAILED, NULL, message_info);
            }
            break;
        
        case SSL_WEAK_CA_MD:
            // 若签名算法（MD5）证书无效
            (void)create_message_info(MSGID_CA_MD_TOO_WEAK, NULL, message_info, "MD5");
            break;
        case SSL_CACERT_OVERDUED:
            (void)create_message_info(MSGID_CERT_EXPIRED, NULL, message_info);
            break;
        default:
            // 未识别到的返回码
            result = HTTP_INTERNAL_SERVER_ERROR;
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_info);
            debug_log(DLOG_ERROR, "%s:  unknown cert import error:0x%x", __FUNCTION__, method_ret);
    }

    return result;
}


LOCAL gint32 __file_transfer_prep_check(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint8 transfering_flag = 0xff;
    gint32 available_task_id;

    return_val_do_info_if_expr((NULL == i_paras) || (NULL == o_message_jso), VOS_ERR,
        debug_log(DLOG_ERROR, "%s: NULL Pointer", __FUNCTION__));

    // 检查是否有其他的文件正在传输
    ret = dal_get_object_handle_nth(CLASS_NAME_TRANSFERFILE, 0, &obj_handle);
    goto_label_do_info_if_fail(VOS_OK == ret, err_exit,
        debug_log(DLOG_ERROR, "cann't get object for %s", CLASS_NAME_TRANSFERFILE));

    ret = dal_get_property_value_byte(obj_handle, PROTERY_IS_TRANSFERING, &transfering_flag);
    goto_label_do_info_if_fail(VOS_OK == ret, err_exit,
        debug_log(DLOG_ERROR, "cann't get object property %s", PROTERY_IS_TRANSFERING));
    
    return_val_do_info_if_fail(FALSE == transfering_flag, HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "upgrade failed, another file transfer task is running");
        (void)create_message_info(MSGID_FILE_DOWNLOAD_TASK_OCCUPIED, NULL, o_message_jso));

    
    // 无可用的task id 返回错误
    available_task_id = find_available_task_id();
    return_val_do_info_if_fail(RF_INVALID_TASK_ID != available_task_id, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_TASK_LIMIT_EXCEED, NULL, o_message_jso));
    

    return VOS_OK;

err_exit:
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);

    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL TASK_MONITOR_INFO_S *__file_trasfer_monitor_info_new(PROVIDER_PARAS_S *i_paras, gint32 cert_type)
{
    TASK_MONITOR_INFO_S *monitor_info = NULL;
    json_object *user_data_jso = NULL;

    return_val_do_info_if_expr(NULL == i_paras, NULL, debug_log(DLOG_ERROR, "%s: NULL Pointer", __FUNCTION__));

    monitor_info = task_monitor_info_new((GDestroyNotify)monitor_info_free_func);
    
    if (NULL == monitor_info) {
        debug_log(DLOG_ERROR, "task_monitor_info_new failed");
        return NULL;
    }
    

    user_data_jso = json_object_new_object();
    if (NULL == user_data_jso) {
        debug_log(DLOG_ERROR, "alloc a new json object failed");

        task_monitor_info_free(monitor_info);

        return NULL;
    }

    json_object_object_add(user_data_jso, RF_LOG_USER_NAME, json_object_new_string(i_paras->user_name));
    json_object_object_add(user_data_jso, RF_LOG_USER_IP, json_object_new_string(i_paras->client));
    // 区分根证书和本地证书
    json_object_object_add(user_data_jso, RFPROP_SYSLOG_CERT_TYPE, json_object_new_int(cert_type));
    
    json_object_object_add(user_data_jso, RF_USERDATA_FROM_WEBUI_FLAG,
        json_object_new_int((gint32)i_paras->is_from_webui));
    

    monitor_info->user_data = user_data_jso;

    return monitor_info;
}


LOCAL gint32 __import_syslog_cert(json_object *body_jso, json_object *user_data)
{
    gint32 ret;
    const gchar *remote_uri = NULL;
    const gchar *user_name = NULL;
    const gchar *pwd = "NULL";
    const gchar *client = NULL;
    GSList *input_list = NULL;
    gchar *slash_index = NULL;
    gchar cert_path[MAX_URI_LENGTH] = {0};
    gint32 cert_type = 0;
    
    gint32 from_webui_flag = REDFISH_REQ_FROM_OTHER_CLIENT;
    

    return_val_do_info_if_expr((NULL == body_jso) || (NULL == user_data), RF_ERROR,
        debug_log(DLOG_ERROR, "%s: NULL Pointer", __FUNCTION__));

    (void)get_element_str(user_data, RF_LOG_USER_NAME, &user_name);
    (void)get_element_str(user_data, RF_LOG_USER_IP, &client);
    (void)get_element_int(user_data, RFPROP_SYSLOG_CERT_TYPE, &cert_type);
    (void)get_element_str(body_jso, RFACTION_PARAM_CONTENT, &remote_uri);
    
    (void)get_element_int(body_jso, RF_USERDATA_FROM_WEBUI_FLAG, &from_webui_flag);
    

    
    
    return_val_do_info_if_expr((NULL == remote_uri) || (NULL == user_name) || (NULL == client), RF_ERROR,
        json_object_clear_string(body_jso, RFACTION_PARAM_CONTENT);
        json_object_clear_string(body_jso, RFACTION_PARAM_PWD);
        debug_log(DLOG_ERROR, "%s: NULL Pointer", __FUNCTION__));
    

    return_val_do_info_if_expr(cert_type < 0 || cert_type > G_MAXUINT8, RF_ERROR,
        debug_log(DLOG_ERROR, "%s: NULL Pointer", __FUNCTION__));

    if (SYSLOG_CERT_TYPE_CLIENT == cert_type) {
        ret = get_element_str(body_jso, RFACTION_PARAM_PWD, &pwd);
        // 没有传密码，认为密码是空字符串
        do_info_if_true(FALSE == ret, (pwd = ""));
    }

    
    return_val_do_info_if_expr(NULL == pwd, RF_ERROR, json_object_clear_string(body_jso, RFACTION_PARAM_CONTENT);
        json_object_clear_string(body_jso, RFACTION_PARAM_PWD);
        debug_log(DLOG_ERROR, "%s: NULL Pointer", __FUNCTION__));
    
    

    slash_index = g_strrstr(remote_uri, SLASH_FLAG_STR);
    
    return_val_do_info_if_fail(NULL != slash_index, RF_FAILED,
        json_object_clear_string(body_jso, RFACTION_PARAM_CONTENT);
        json_object_clear_string(body_jso, RFACTION_PARAM_PWD);
        debug_log(DLOG_ERROR, "%s: get file name failed", __FUNCTION__));
    

    ret = snprintf_s(cert_path, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, "/tmp%s", slash_index);
    
    return_val_do_info_if_expr(VOS_OK >= ret, RF_ERROR, json_object_clear_string(body_jso, RFACTION_PARAM_CONTENT);
        json_object_clear_string(body_jso, RFACTION_PARAM_PWD);
        debug_log(DLOG_ERROR, "%s: snprintf_s failed", __FUNCTION__));
    

    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)cert_type));
    input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)cert_path));
    input_list = g_slist_append(input_list, g_variant_new_string(pwd));

    
    ret = uip_call_class_method_redfish((guchar)from_webui_flag, user_name, client, 0, CLASS_SYSLOG_CONFIG,
        METHOD_SYSLOG_MGNT_IMPORT_CERT, AUTH_DISABLE, 0, input_list, NULL);
    uip_free_gvariant_list(input_list);
    pwd = NULL;
    

    
    json_object_clear_string(body_jso, RFACTION_PARAM_CONTENT);
    json_object_clear_string(body_jso, RFACTION_PARAM_PWD);
    

    return ret;
}


LOCAL gint32 __import_syslog_cert_status_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    gint32 ret;
    gint32 file_down_progress = 0;
    gint32 cert_type = 0xff;

    return_val_do_info_if_expr((origin_obj_path == NULL) || (body_jso == NULL) || (monitor_fn_data == NULL) ||
        (message_obj == NULL),
        RF_OK, debug_log(DLOG_ERROR, "%s: NULL Pointer", __FUNCTION__));

    monitor_fn_data->task_state = RF_TASK_RUNNING;

    ret = get_file_transfer_progress(&file_down_progress);
    goto_label_do_info_if_fail(ret == VOS_OK, err_exit,
        debug_log(DLOG_ERROR, "get upgrade packege download progress failed"));

    (void)get_element_int((json_object *)monitor_fn_data->user_data, RFPROP_SYSLOG_CERT_TYPE, &cert_type);

    
    // 文件传输失败，生成证书导入失败的错误消息
    if (file_down_progress < 0) {
        (void)parse_file_transfer_err_code(file_down_progress, message_obj);

        monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        monitor_fn_data->task_progress = 0;
        json_object_clear_string(body_jso, RFACTION_PARAM_CONTENT);
        json_object_clear_string(body_jso, RFACTION_PARAM_PWD);
        return RF_OK;
    }
    

    debug_log(DLOG_DEBUG, "----------file down progress :%d---------", file_down_progress);

    if (file_down_progress != RF_FINISH_PERCENTAGE) {
        monitor_fn_data->task_progress = (guchar)(file_down_progress * 0.1);

        return RF_OK;
    } else {
        monitor_fn_data->task_progress = 10;

        // 执行证书导入动作
        ret = __import_syslog_cert(body_jso, (json_object *)monitor_fn_data->user_data);

        // 解析导入证书的错误码
        (void)__parse_cert_result_code(ret, message_obj, cert_type);

        if (ret == VOS_OK) {
            monitor_fn_data->task_state = RF_TASK_COMPLETED;
            monitor_fn_data->task_progress = 100;
        } else {
            monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        }
        json_object_clear_string(body_jso, RFACTION_PARAM_CONTENT);
        json_object_clear_string(body_jso, RFACTION_PARAM_PWD);
        return RF_OK;
    }

err_exit:
    monitor_fn_data->task_state = RF_TASK_EXCEPTION;
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);
    json_object_clear_string(body_jso, RFACTION_PARAM_CONTENT);
    json_object_clear_string(body_jso, RFACTION_PARAM_PWD);
    return RF_OK;
}


gint32 __call_file_trasfer(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, gint32 *result, gint32 file_id)
{
    gint32 ret;
    GSList *input_list = NULL;
    
    // 这里将URI长度改为1024，和文件传输统一
    gchar file_transfer_uri[MAX_URL_LEN] = {0};
    
    const gchar *remote_uri = NULL;

    return_val_do_info_if_expr((NULL == o_message_jso) || (VOS_OK != provider_paras_check(i_paras)) || (NULL == result),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    ret = get_element_str(i_paras->val_jso, RFACTION_PARAM_CONTENT, &remote_uri);
    goto_label_do_info_if_fail(TRUE == ret, err_exit,
        debug_log(DLOG_ERROR, "cann't get %s from request.", RFACTION_PARAM_CONTENT));

    // 文件传输预检测
    ret = __file_transfer_prep_check(i_paras, o_message_jso);
    return_val_if_fail(VOS_OK == ret, ret);

    
    ret = snprintf_s(file_transfer_uri, sizeof(file_transfer_uri), sizeof(file_transfer_uri) - 1, "download;%d;%s",
        file_id, remote_uri);
    // URI超长,生成一个证书导入失败的消息
    if (ret <= 0) {
        debug_log(DLOG_DEBUG, "%s: snprintf_s fail", __FUNCTION__);
        (void)create_message_info(MSGID_CERT_IMPORT_FAILED, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }
    
    input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)file_transfer_uri));
    (void)memset_s(file_transfer_uri, MAX_URL_LEN, 0, MAX_URL_LEN);
    input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)i_paras->session_id));

    *result = uip_redfish_call_class_method_with_userinfo(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        (i_paras->auth_type == LOG_TYPE_LOCAL), (const gchar *)i_paras->user_roleid, 0, CLASS_NAME_TRANSFERFILE,
        METHOD_INITRIAL_FILE_TRANSFER, AUTH_DISABLE, 0, input_list, NULL);

    uip_free_gvariant_list(input_list);
    return VOS_OK;
err_exit:

    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL gint32 __import_root_cert_from_remot_uri(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint32 result = 0;
    TASK_MONITOR_INFO_S *file_trasfer_monitor_data = NULL;

    return_val_do_info_if_expr((NULL == i_paras) || (NULL == o_message_jso), VOS_ERR,
        debug_log(DLOG_ERROR, "%s: NULL Pointer", __FUNCTION__));

    ret = __call_file_trasfer(i_paras, o_message_jso, &result, SYSLOG_ROOT_CERT_FILE_ID);
    return_val_do_info_if_fail(VOS_OK == ret, ret, debug_log(DLOG_ERROR, "%s: call file trasfer failed", __FUNCTION__));

    if (VOS_OK != result) {
        
        (void)parse_file_transfer_err_code(result, o_message_jso);
        

        return HTTP_BAD_REQUEST;
    } else {
        file_trasfer_monitor_data = __file_trasfer_monitor_info_new(i_paras, SYSLOG_CERT_TYPE_HOST);
        goto_label_do_info_if_fail(NULL != file_trasfer_monitor_data, err_exit,
            debug_log(DLOG_ERROR, "alloc a new file trasfer monitor info failed"));

        file_trasfer_monitor_data->task_progress = TASK_NO_PROGRESS;
        file_trasfer_monitor_data->rsc_privilege = USERROLE_SECURITYMGNT;
        ret = create_new_task(TASK_IMPORT_ROOT_CERT, __import_syslog_cert_status_monitor,
            file_trasfer_monitor_data, i_paras->val_jso, i_paras->uri, o_result_jso);

        goto_label_do_info_if_fail(RF_OK == ret, err_exit, task_monitor_info_free(file_trasfer_monitor_data);
            debug_log(DLOG_ERROR, "%s: create new task failed", __FUNCTION__));
    }

    return HTTP_ACCEPTED;

err_exit:

    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);

    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL gint32 __act_syslog_import_root_cert(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    const gchar *content = NULL;
    const gchar *type = NULL;
    GSList *input_list = NULL;
    const gchar *cert_path = NULL;
    gint32 uri_type = 0xff;
    gint64 file_len = 0;
    
    gboolean b_ret;
    

    return_val_do_info_if_fail((i_paras != NULL) && (o_message_jso != NULL) && (o_result_jso != NULL),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    // 调用方法的前面处理了部分逻辑，所以鉴权放在入口处
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_SECURITYMGNT, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = get_element_str(i_paras->val_jso, RFACTION_PARAM_TYPE, &type);
    goto_label_do_info_if_fail(ret == TRUE, err_exit,
        debug_log(DLOG_ERROR, "cann't get %s from request.", RFACTION_PARAM_TYPE));

    ret = get_element_str(i_paras->val_jso, RFACTION_PARAM_CONTENT, &content);
    goto_label_do_info_if_fail(ret == TRUE, err_exit,
        debug_log(DLOG_ERROR, "cann't get %s from request.", RFACTION_PARAM_CONTENT));

    
    b_ret = check_redfish_running_task_exist(__import_syslog_cert_status_monitor, i_paras->val_jso, i_paras->uri, NULL);
    return_val_do_info_if_fail(FALSE == b_ret, HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "certificate import task already exist");
        (void)create_message_info(MSGID_CERT_IMPORTING, NULL, o_message_jso));
    

    // 如果type是text，保存为零时文件
    if (g_strcmp0(RFACTION_PARAM_VALUE_TEXT, type) == 0) {
        
        
        if (strlen(content) > 1 * 1024 * 1024) { // 证书文件大小门限为1M
            (void)create_message_info(MSGID_CERT_IMPORT_FAILED, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        }
        
        

        // 证书内容保存为临时文件
        ret = g_file_set_contents(TMP_SYSLOG_ROOT_CERT_FILE, content, strlen(content), NULL);
        goto_label_do_info_if_fail(ret, err_exit,
            debug_log(DLOG_ERROR, "save certificate info to %s failed.", TMP_SYSLOG_ROOT_CERT_FILE));
        // 只需读权限即可，解析证书只需要读权限
        (void)chmod(TMP_SYSLOG_ROOT_CERT_FILE, S_IRUSR);

        cert_path = TMP_SYSLOG_ROOT_CERT_FILE;
    } else if (g_strcmp0(RFACTION_PARAM_VALUE_URI, type) == 0) {
        
        b_ret = g_regex_match_simple(SERVER_CERT_IMPORT_REGEX, content, G_REGEX_OPTIMIZE, (GRegexMatchFlags)0);
        return_val_do_info_if_fail(b_ret == TRUE, HTTP_BAD_REQUEST,
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFACTION_PARAM_CONTENT, o_message_jso, RF_SENSITIVE_INFO,
                RFACTION_PARAM_CONTENT));
        

        // 判断uri的类型
        ret = redfish_check_uri_type(content, &uri_type);
        goto_label_do_info_if_fail(ret == VOS_OK, err_exit,
            debug_log(DLOG_ERROR, "%s: get uri type failed.", __FUNCTION__));

        if (uri_type == URI_TYPE_LOCAL) {
            
            ret = dal_check_real_path(content);
            if (ret != VOS_OK) {
                debug_log(DLOG_ERROR, "%s :file real_path is illegal! return %d", __FUNCTION__, ret);
                (void)create_message_info(MSGID_CERT_IMPORT_FAILED, NULL, o_message_jso);
                return HTTP_BAD_REQUEST;
            }

            if (!dal_check_file_opt_user(i_paras->auth_type, (const gchar *)i_paras->user_name, content,
                (const gchar *)i_paras->user_roleid)) {
                (void)create_message_info(MSGID_NO_PRIV_OPT_FILE, NULL, o_message_jso);
                json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
                return HTTP_BAD_REQUEST;
            }

            

            
            
            if (vos_get_file_accessible(content) == TRUE) {
                file_len = vos_get_file_length(content);
                // 文件大于1M，不继续
                // 此处使用代理方法删除文件
                if (file_len > 1 * 1024 * 1024) {
                    (void)proxy_delete_file(i_paras->is_from_webui, i_paras->user_name, i_paras->client, content);
                    (void)create_message_info(MSGID_CERT_IMPORT_FAILED, NULL, o_message_jso);
                    json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
                    return HTTP_BAD_REQUEST;
                }
            }
            
            

            cert_path = content;
        } else if (uri_type == URI_TYPE_REMOTE) {
            return __import_root_cert_from_remot_uri(i_paras, o_message_jso, o_result_jso);
        } else {
            goto_label_do_info_if_fail(ret, err_exit, debug_log(DLOG_ERROR, "%s: invalid uri type.", __FUNCTION__));
        }
    } else {
        goto_label_do_info_if_fail(ret, err_exit, debug_log(DLOG_ERROR, "%s: invalid type is %s.", __FUNCTION__, type));
    }

    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)SYSLOG_CERT_TYPE_HOST));
    input_list = g_slist_append(input_list, g_variant_new_string(cert_path));
    input_list = g_slist_append(input_list, g_variant_new_string("NULL")); // class中为3个参数

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_SYSLOG_CONFIG, METHOD_SYSLOG_MGNT_IMPORT_CERT, AUTH_DISABLE, 0, input_list, NULL);
    uip_free_gvariant_list(input_list);

    
    do_val_if_expr(ret != VOS_OK, debug_log(DLOG_ERROR, "%s failed:result error code :%d", __FUNCTION__, ret));
    

    // 隐私数据清除

    return __parse_cert_result_code(ret, o_message_jso, SYSLOG_CERT_TYPE_HOST);

err_exit:
    
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);

    return HTTP_INTERNAL_SERVER_ERROR;
}

LOCAL gint32 __act_syslog_import_root_cert_entry(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret = __act_syslog_import_root_cert(i_paras, o_message_jso, o_result_jso);
    if (ret != HTTP_ACCEPTED && i_paras != NULL) {
        delete_user_specific_file(i_paras, i_paras->val_jso, RFACTION_PARAM_CONTENT);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
    }
    return ret;
}


LOCAL gint32 __import_client_cert_from_remot_uri(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint32 result = 0;
    TASK_MONITOR_INFO_S *file_trasfer_monitor_data = NULL;

    return_val_do_info_if_expr((NULL == i_paras) || (NULL == o_message_jso), VOS_ERR,
        debug_log(DLOG_ERROR, "%s: NULL Pointer", __FUNCTION__));

    ret = __call_file_trasfer(i_paras, o_message_jso, &result, SYSLOG_CLIENT_CERT_FILE_ID);
    
    // 文件传输错误，直接返回
    return_val_do_info_if_fail(VOS_OK == ret, ret, debug_log(DLOG_ERROR, "%s: call file trasfer failed", __FUNCTION__));
    
    if (VOS_OK != result) {
        
        (void)parse_file_transfer_err_code(result, o_message_jso);
        

        return HTTP_BAD_REQUEST;
    } else {
        file_trasfer_monitor_data = __file_trasfer_monitor_info_new(i_paras, SYSLOG_CERT_TYPE_CLIENT);
        goto_label_do_info_if_fail(NULL != file_trasfer_monitor_data, err_exit,
            debug_log(DLOG_ERROR, "alloc a new file trasfer monitor info failed"));

        file_trasfer_monitor_data->task_progress = TASK_NO_PROGRESS;
        file_trasfer_monitor_data->rsc_privilege = USERROLE_SECURITYMGNT;
        ret = create_new_task(TASK_IMPORT_SYSLOG_CLIENT_CERT, __import_syslog_cert_status_monitor,
            file_trasfer_monitor_data, i_paras->val_jso, i_paras->uri, o_result_jso);

        goto_label_do_info_if_fail(RF_OK == ret, err_exit, task_monitor_info_free(file_trasfer_monitor_data);
            debug_log(DLOG_ERROR, "%s: create new task failed", __FUNCTION__));
    }

    return HTTP_ACCEPTED;

err_exit:

    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);

    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL gint32 __act_syslog_import_client_cert(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    const gchar *content = NULL;
    const gchar *type = NULL;
    const gchar *pwd = NULL;
    gsize cert_len = 0;
    guchar *cert_content = NULL;
    GSList *input_list = NULL;
    const gchar *cert_path = NULL;
    gint32 uri_type = 0xff;
    gint64 file_len = 0;
    
    gboolean b_ret;
    

    return_val_do_info_if_fail((i_paras != NULL) && (o_message_jso != NULL) && (o_result_jso != NULL),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_SECURITYMGNT, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    

    ret = get_element_str(i_paras->val_jso, RFACTION_PARAM_TYPE, &type);
    goto_label_do_info_if_fail(ret == TRUE, err_exit,
        debug_log(DLOG_ERROR, "cann't get %s from request.", RFACTION_PARAM_TYPE));

    ret = get_element_str(i_paras->val_jso, RFACTION_PARAM_CONTENT, &content);
    goto_label_do_info_if_fail(ret == TRUE, err_exit,
        debug_log(DLOG_ERROR, "cann't get %s from request.", RFACTION_PARAM_CONTENT));

    ret = get_element_str(i_paras->val_jso, RFACTION_PARAM_PWD, &pwd);
    // 没传密码认为是空字符串
    do_info_if_true(ret == FALSE, (pwd = ""));

    
    b_ret = check_redfish_running_task_exist(__import_syslog_cert_status_monitor, i_paras->val_jso, i_paras->uri, NULL);
    
    return_val_do_info_if_fail(FALSE == b_ret, HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "%s failed:cert import task already exists", __FUNCTION__);
        (void)create_message_info(MSGID_CERT_IMPORTING, NULL, o_message_jso));
    
    

    if (g_strcmp0(RFACTION_PARAM_VALUE_TEXT, type) == 0) {
        
        
        if (strlen(content) > 1 * 1024 * 1024) { // 证书文件大小门限为1M
            (void)create_message_info(MSGID_ENCRYPTED_CERT_IMPORT_FAILED, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        }
        
        
        // 解码
        cert_content = g_base64_decode(content, &cert_len);
        return_val_do_info_if_fail((0 != cert_len) && (NULL != cert_content), HTTP_BAD_REQUEST,
            (void)create_message_info(MSGID_ENCRYPTED_CERT_IMPORT_FAILED, NULL, o_message_jso);
            g_free(cert_content);
            debug_log(DLOG_ERROR, "import custom cert failed, content is not base64 encoded."));

        // 保存为零时文件
        ret = g_file_set_contents(TMP_SYSLOG_CLIENT_CERT_FILE, (const gchar *)cert_content, (gssize)cert_len, NULL);
        goto_label_do_info_if_fail(ret, err_exit,
            debug_log(DLOG_ERROR, "save certificate info to %s failed.", TMP_SYSLOG_ROOT_CERT_FILE));

        // 只需读权限即可
        (void)chmod(TMP_SYSLOG_CLIENT_CERT_FILE, S_IRUSR);

        cert_path = TMP_SYSLOG_CLIENT_CERT_FILE;
    } else if (g_strcmp0(RFACTION_PARAM_VALUE_URI, type) == 0) {
        
        b_ret = g_regex_match_simple(CUSTOM_CERT_IMPORT_REGEX, content, G_REGEX_OPTIMIZE, (GRegexMatchFlags)0);
        
        return_val_do_info_if_fail(TRUE == b_ret, HTTP_BAD_REQUEST,
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFACTION_PARAM_CONTENT, o_message_jso, RF_SENSITIVE_INFO,
            RFACTION_PARAM_CONTENT));
        

        ret = redfish_check_uri_type(content, &uri_type);
        goto_label_do_info_if_fail(ret == VOS_OK, err_exit,
            debug_log(DLOG_ERROR, "%s: get uri type failed.", __FUNCTION__));

        if (uri_type == URI_TYPE_LOCAL) {
            
            ret = dal_check_real_path(content);
            if (ret != VOS_OK) {
                debug_log(DLOG_ERROR, "%s :file real_path is illegal! return %d", __FUNCTION__, ret);
                json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_PWD);
                
                (void)create_message_info(MSGID_ENCRYPTED_CERT_IMPORT_FAILED, NULL, o_message_jso);
                
                return HTTP_BAD_REQUEST;
            }

            if (!dal_check_file_opt_user(i_paras->auth_type, (const gchar *)i_paras->user_name, content,
                (const gchar *)i_paras->user_roleid)) {
                (void)create_message_info(MSGID_NO_PRIV_OPT_FILE, NULL, o_message_jso);
                return HTTP_BAD_REQUEST;
            }

            
            
            if (vos_get_file_accessible(content) == TRUE) {
                file_len = vos_get_file_length(content);
                // 文件大于1M，不继续
                if (file_len > 1 * 1024 * 1024) {
                    (void)proxy_delete_file(i_paras->is_from_webui, i_paras->user_name, i_paras->client, content);
                    (void)create_message_info(MSGID_ENCRYPTED_CERT_IMPORT_FAILED, NULL, o_message_jso);
                    return HTTP_BAD_REQUEST;
                }
            }
            

            cert_path = content;
        } else if (uri_type == URI_TYPE_REMOTE) {
            return __import_client_cert_from_remot_uri(i_paras, o_message_jso, o_result_jso);
        } else {
            goto_label_do_info_if_fail(ret, err_exit,
                debug_log(DLOG_ERROR, "%s: invalid uri type.", __FUNCTION__));
        }

        
    } else {
        goto_label_do_info_if_fail(ret, err_exit, debug_log(DLOG_ERROR, "%s: invalid type is %s.", __FUNCTION__, type));
    }

    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)SYSLOG_CERT_TYPE_CLIENT));
    input_list = g_slist_append(input_list, g_variant_new_string(cert_path));
    input_list = g_slist_append(input_list, g_variant_new_string(pwd));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_SYSLOG_CONFIG, METHOD_SYSLOG_MGNT_IMPORT_CERT, AUTH_DISABLE, 0, input_list, NULL);
    uip_free_gvariant_list(input_list);
    pwd = NULL;
    do_info_if_true(NULL != cert_content, g_free(cert_content); cert_content = NULL);

    return __parse_cert_result_code(ret, o_message_jso, SYSLOG_CERT_TYPE_CLIENT);

err_exit:
    do_info_if_true(NULL != cert_content, g_free(cert_content); cert_content = NULL);
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);

    return HTTP_INTERNAL_SERVER_ERROR;
}

LOCAL gint32 __act_syslog_import_client_cert_entry(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret = __act_syslog_import_client_cert(i_paras, o_message_jso, o_result_jso);
    if (ret != HTTP_ACCEPTED && i_paras != NULL) {
        delete_user_specific_file(i_paras, i_paras->val_jso, RFACTION_PARAM_CONTENT);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_PWD);
    }
    return ret;
}


LOCAL gint32 __act_syslog_submit_test_event(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    const gchar *memer_id = NULL;
    guint32 ser_index = 0;
    gint32 ret;
    GSList *input_list = NULL;

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_message_jso) && (NULL != o_result_jso),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_SECURITYMGNT, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = get_element_str(i_paras->val_jso, MEMBER_ID, &memer_id);
    goto_label_do_info_if_fail(TRUE == ret, err_exit,
        debug_log(DLOG_ERROR, "%s: cann't get %s from request.", __FUNCTION__, MEMBER_ID));

    ret = vos_str2int(memer_id, 10, &ser_index, NUM_TPYE_UINT32);
    goto_label_do_info_if_fail(VOS_OK == ret, err_exit,
        debug_log(DLOG_ERROR, "%s: invalid member id is %s.", __FUNCTION__, memer_id));

    ret = dal_get_object_handle_nth(CLASS_SYSLOG_ITEM_CONFIG, ser_index, &obj_handle);
    goto_label_do_info_if_fail(VOS_OK == ret, err_exit,
        debug_log(DLOG_ERROR, "%s: can't get object handle ,member id is %s.", __FUNCTION__, memer_id));

    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)ser_index));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SYSLOG_ITEM_CONFIG, METHOD_REMOTE_SYSLOG_SET_TEST_TRIGGER, AUTH_DISABLE, 0, input_list, NULL);

    uip_free_gvariant_list(input_list);
    switch (ret) {
        case VOS_OK:
            break;

        case RF_ERROR:
            // 使能未打开或者地址为空字符串
            (void)create_message_info(MSGID_SYSLOG_TEST_FAILED, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;

        default:
            debug_log(DLOG_ERROR, "%s: submit test on %d fail, return %d.", __FUNCTION__, ser_index, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);

            return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;

err_exit:
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);

    return HTTP_INTERNAL_SERVER_ERROR;
}


gint32 manager_syslog_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gint32 ret;
    guint8 board_type = 0;
    OBJ_HANDLE obj_handle = 0;

    
    ret = redfish_get_board_type(&board_type);
    return_val_if_fail(VOS_OK == ret, HTTP_NOT_FOUND);
    return_val_if_fail(BOARD_SWITCH != board_type, HTTP_NOT_FOUND);

    
    ret = redfish_check_manager_uri_valid(i_paras->uri);
    return_val_do_info_if_fail(TRUE == ret, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s: invalid uri is %s", __FUNCTION__, i_paras->uri));

    
    (void)dal_get_object_handle_nth(CLASS_SYSLOG_CONFIG, 0, &obj_handle);
    i_paras->obj_handle = obj_handle;

    *prop_provider = g_manager_syslog_provider;
    *count = G_N_ELEMENTS(g_manager_syslog_provider);

    return VOS_OK;
}
// 结束syslog服务资源

// 开始根证书的actioninfo资源

LOCAL gint32 __get_syslog_root_cert_actioninfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    gchar syslog_root_cert_uri[MAX_URI_LENGTH] = {0};

    return_val_do_info_if_expr((NULL == o_result_jso) || (NULL == o_message_jso) ||
        (VOS_OK != provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_slot(slot, MAX_RSC_NAME_LEN);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get slot fail", __FUNCTION__));

    ret = snprintf_s(syslog_root_cert_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, RFACTION_INFO_SYSLOG_IMPORT_ROOT_CERT,
        slot);
    return_val_do_info_if_expr(VOS_OK >= ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: snprintf_s syslog uri fail", __FUNCTION__));

    *o_result_jso = json_object_new_string((const char *)syslog_root_cert_uri);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: new syslog uri  json string fail", __FUNCTION__));

    return HTTP_OK;
}

// 资源数组
LOCAL PROPERTY_PROVIDER_S g_syslog_import_root_cert_actioninfo_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, __get_syslog_root_cert_actioninfo_odata_id, NULL, NULL, ETAG_FLAG_ENABLE}
};


gint32 manager_syslog_import_root_cert_actioninfo_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 retVal;
    guint8 board_type = 0;

    
    retVal = redfish_get_board_type(&board_type);
    return_val_if_fail(VOS_OK == retVal, HTTP_NOT_FOUND);
    return_val_if_fail(BOARD_SWITCH != board_type, HTTP_NOT_FOUND);

    
    retVal = redfish_check_manager_uri_valid(i_paras->uri);
    return_val_do_info_if_fail(TRUE == retVal, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s: invalid uri is %s", __FUNCTION__, i_paras->uri));
    *prop_provider = g_syslog_import_root_cert_actioninfo_provider;
    *count = G_N_ELEMENTS(g_syslog_import_root_cert_actioninfo_provider);

    return HTTP_OK;
}

// 开始本地证书的actioninfo

LOCAL gint32 __get_syslog_client_cert_actioninfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    gchar syslog_client_cert_uri[MAX_URI_LENGTH] = {0};

    return_val_do_info_if_expr((NULL == o_result_jso) || (NULL == o_message_jso) ||
        (VOS_OK != provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_slot(slot, MAX_RSC_NAME_LEN);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get slot fail", __FUNCTION__));

    ret = snprintf_s(syslog_client_cert_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1,
        RFACTION_INFO_SYSLOG_IMPORT_CLIENT_CERT, slot);
    return_val_do_info_if_expr(VOS_OK >= ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: snprintf_s syslog uri fail", __FUNCTION__));

    *o_result_jso = json_object_new_string((const char *)syslog_client_cert_uri);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: new syslog uri  json string fail", __FUNCTION__));

    return HTTP_OK;
}

LOCAL PROPERTY_PROVIDER_S g_syslog_import_client_cert_actioninfo_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, __get_syslog_client_cert_actioninfo_odata_id, NULL, NULL, ETAG_FLAG_ENABLE}
};


gint32 manager_syslog_import_client_cert_actioninfo_entry(PROVIDER_PARAS_S *i_paras,
    PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gint32 result;
    guint8 board_type = 0;

    
    result = redfish_get_board_type(&board_type);
    return_val_if_fail(VOS_OK == result, HTTP_NOT_FOUND);
    return_val_if_fail(BOARD_SWITCH != board_type, HTTP_NOT_FOUND);

    
    result = redfish_check_manager_uri_valid(i_paras->uri);
    return_val_do_info_if_fail(TRUE == result, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s: invalid uri is %s", __FUNCTION__, i_paras->uri));
    *prop_provider = g_syslog_import_client_cert_actioninfo_provider;
    *count = G_N_ELEMENTS(g_syslog_import_client_cert_actioninfo_provider);

    return HTTP_OK;
}

// 开始发送测试事件的actioninfo

LOCAL gint32 __get_syslog_test_actioninfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    gchar syslog_test_uri[MAX_URI_LENGTH] = {0};

    return_val_do_info_if_expr((NULL == o_result_jso) || (NULL == o_message_jso) ||
        (VOS_OK != provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_slot(slot, MAX_RSC_NAME_LEN);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get slot fail", __FUNCTION__));

    ret = snprintf_s(syslog_test_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, RFACTION_INFO_SYSLOG_TEST, slot);
    return_val_do_info_if_expr(VOS_OK >= ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: snprintf_s syslog uri fail", __FUNCTION__));

    *o_result_jso = json_object_new_string((const char *)syslog_test_uri);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: new syslog uri  json string fail", __FUNCTION__));

    return HTTP_OK;
}

LOCAL PROPERTY_PROVIDER_S g_syslog_test_actioninfo_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, __get_syslog_test_actioninfo_odata_id, NULL, NULL, ETAG_FLAG_ENABLE}
};


gint32 manager_syslog_submit_test_actioninfo_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 res;
    guint8 board_type = 0;

    
    res = redfish_get_board_type(&board_type);
    return_val_if_fail(VOS_OK == res, HTTP_NOT_FOUND);
    return_val_if_fail(BOARD_SWITCH != board_type, HTTP_NOT_FOUND);

    
    res = redfish_check_manager_uri_valid(i_paras->uri);
    return_val_do_info_if_fail(TRUE == res, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s: invalid uri is %s", __FUNCTION__, i_paras->uri));
    *prop_provider = g_syslog_test_actioninfo_provider;
    *count = G_N_ELEMENTS(g_syslog_test_actioninfo_provider);

    return HTTP_OK;
}


LOCAL PROPERTY_PROVIDER_S g_syslog_import_crl_actioninfo_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, _get_syslog_crl_actioninfo_odata_id, NULL, NULL, ETAG_FLAG_ENABLE}
};


LOCAL gint32 _get_syslog_crl_actioninfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return get_odata_id(i_paras, o_message_jso, o_result_jso, RFACTION_INFO_SYSLOG_IMPORT_CRL);
}


LOCAL PROPERTY_PROVIDER_S g_syslog_delete_crl_actioninfo_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_delete_crl_actioninfo_odata, NULL, NULL, ETAG_FLAG_ENABLE}
};


LOCAL gint32 get_delete_crl_actioninfo_odata(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return get_odata_id(i_paras, o_message_jso, o_result_jso, RFACTION_INFO_SYSLOG_DELETE_CRL);
}


gint32 manager_syslog_import_crl_actioninfo_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret;
    guchar board_type = 0;

    
    ret = redfish_get_board_type(&board_type);
    return_val_if_expr(ret != VOS_OK || board_type == BOARD_SWITCH, HTTP_NOT_FOUND);

    
    return_val_do_info_if_expr(redfish_check_manager_uri_valid(i_paras->uri) == FALSE, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s: invalid uri(%s)", __FUNCTION__, i_paras->uri));

    *prop_provider = g_syslog_import_crl_actioninfo_provider;
    *count = G_N_ELEMENTS(g_syslog_import_crl_actioninfo_provider);
    return HTTP_OK;
}


gint32 manager_syslog_delete_crl_actioninfo_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    guchar board_type = 0;

    
    gint32 ret = redfish_get_board_type(&board_type);
    if (ret != RET_OK || board_type == BOARD_SWITCH) {
        return HTTP_NOT_FOUND;
    }

    
    if (redfish_check_manager_uri_valid(i_paras->uri) == FALSE) {
        debug_log(DLOG_ERROR, "%s: invalid uri(%s)", __FUNCTION__, i_paras->uri);
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_syslog_delete_crl_actioninfo_provider;
    *count = G_N_ELEMENTS(g_syslog_delete_crl_actioninfo_provider);
    return HTTP_OK;
}


LOCAL gint32 process_import_local_crl(const gchar *file_path, json_object *user_data, json_object **o_message_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    const gchar *user_name = NULL;
    const gchar *client = NULL;
    gint32 user_priv = 0;
    gint32 is_from_webui_flag = 0;
    GSList *input_list = NULL;
    gint32 ret_code = HTTP_OK;

    
    goto_label_do_info_if_expr(strlen(file_path) >= MAX_FILEPATH_LENGTH, EXIT, ret = IMPORT_CERT_INVALID_FILEPATH_ERR;
        debug_log(DLOG_ERROR, "%s: file path length is over %d", __FUNCTION__, MAX_FILEPATH_LENGTH));

    
    goto_label_do_info_if_expr(vos_get_file_length(file_path) > MAX_CRL_FILE_LEN, EXIT, ret = HTTP_BAD_REQUEST;
        (void)create_message_info(MSGID_CRL_IMPORT_FAILED, NULL, o_message_jso));
    
    ret = dal_get_object_handle_nth(CLASS_SYSLOG_CONFIG, 0, &obj_handle);
    goto_label_do_info_if_expr(ret != VOS_OK, EXIT, ret = HTTP_INTERNAL_SERVER_ERROR;
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: get class %s object handle fail, ret:%d", __FUNCTION__, CLASS_SYSLOG_CONFIG, ret));

    
    import_cert_get_common_userdata_fn(&user_name, &client, &user_priv, &is_from_webui_flag, user_data);

    
    (void)dal_set_file_owner(file_path, REDFISH_USER_UID, APPS_USER_GID);
    (void)dal_set_file_mode(file_path, (S_IRUSR | S_IWUSR));

    input_list = g_slist_append(input_list, g_variant_new_string(file_path));
    ret = uip_call_class_method_redfish((guchar)is_from_webui_flag, user_name, client, obj_handle, CLASS_SYSLOG_CONFIG,
        METHOD_SYSLOG_MGNT_IMPORT_CRL, AUTH_ENABLE, user_priv, input_list, NULL);
    uip_free_gvariant_list(input_list);

    do_if_expr(ret != VOS_OK, debug_log(DLOG_ERROR, "%s: call remote mothod(%s) fail, ret :%d", __FUNCTION__,
        METHOD_SYSLOG_MGNT_IMPORT_CRL, ret));

    parse_import_crl_common_ret_code(ret, o_message_jso, &ret_code);
    ret = ret_code;

EXIT:
    do_if_expr(ret != HTTP_OK, vos_rm_filepath(file_path));
    return ret;
}


LOCAL gint32 syslog_import_crl_status_monitor(const gchar *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    return import_cert_common_status_monitor(origin_obj_path, body_jso, process_import_local_crl, monitor_fn_data,
        message_obj);
}


LOCAL gint32 process_import_remote_crl(PROVIDER_PARAS_S *i_paras, const gchar *uri, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    TASK_MONITOR_INFO_S *import_crl_info = NULL;
    json_object *body_jso = NULL;

    ret = rf_start_file_download(i_paras, uri, IMPORT_SYSLOG_CRL_TRANSFER_FILEINFO_CODE, o_message_jso);
    return_val_do_info_if_expr(ret != VOS_OK, ret,
        debug_log(DLOG_ERROR, "%s: start crl file download fail, ret:%d", __FUNCTION__, ret));

    import_crl_info = cert_import_info_new(i_paras, import_cert_set_common_userdata_fn);
    goto_label_do_info_if_expr(import_crl_info == NULL, err_exit,
        debug_log(DLOG_ERROR, "%s: alloc new cert_import info failed", __FUNCTION__));
    import_crl_info->task_progress = TASK_NO_PROGRESS; 
    import_crl_info->rsc_privilege = USERROLE_SECURITYMGNT;

    body_jso = i_paras->val_jso;
    ret = create_new_task(IMPORT_SYSLOG_CRL_TASK_NAME, syslog_import_crl_status_monitor, import_crl_info, body_jso,
        i_paras->uri, o_result_jso);
    goto_label_do_info_if_expr(ret != RF_OK, err_exit, task_monitor_info_free(import_crl_info);
        debug_log(DLOG_ERROR, "%s: create new task failed(%d)", __FUNCTION__, ret));

    return HTTP_ACCEPTED;

err_exit:
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL gint32 act_syslog_import_crl(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    IMPORT_CERT_PARAMS_S param;
    
    return_val_do_info_if_expr(i_paras == NULL || o_message_jso == NULL || o_result_jso == NULL,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: invalid parameter", __FUNCTION__));

    if ((i_paras->user_role_privilege & USERROLE_SECURITYMGNT) == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        ret = HTTP_FORBIDDEN;
        goto exit;
    }

    
    ret = is_import_cert_or_crl_task_exist(syslog_import_crl_status_monitor, i_paras, FILE_TYPE_SYSLOG_CRL,
        o_message_jso);
    do_if_expr(ret != VOS_OK, goto exit);

    
    set_import_cert_params(&param, FILE_TYPE_SYSLOG_CRL, import_cert_set_common_userdata_fn, process_import_local_crl,
        process_import_remote_crl, IMPORT_SYSLOG_CRL_TMP_PATH);
    ret = process_cert_or_crl_import(i_paras, o_message_jso, o_result_jso, &param);
    
    if (ret == HTTP_OK || ret == HTTP_ACCEPTED || *o_message_jso != NULL) {
        goto exit;
    }

    
    switch (ret) {
        case IMPORT_CERT_URI_DISMATCH_ERR:
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFACTION_PARAM_CONTENT, o_message_jso, RF_SENSITIVE_INFO,
                RFACTION_PARAM_CONTENT);
            ret = HTTP_BAD_REQUEST;
            break;

        case IMPORT_CERT_FILE_LEN_EXCEED_ERR:
        case IMPORT_CERT_INVALID_FILEPATH_ERR:
            (void)create_message_info(MSGID_CRL_IMPORT_FAILED, NULL, o_message_jso);
            ret = HTTP_BAD_REQUEST;
            break;
        case IMPORT_CRL_ERR_NOT_SUPPORT_CRL_SIGN:
            (void)create_message_info(MSGID_CERT_NOT_SUPPORT_CRL_SIGN, NULL, o_message_jso);
            ret = HTTP_BAD_REQUEST;
            break;

        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            debug_log(DLOG_ERROR, "%s: import CRL to syslog fail, ret:%d", __FUNCTION__, ret);
            ret = HTTP_INTERNAL_SERVER_ERROR;
    }

exit:
    return ret;
}

LOCAL gint32 act_syslog_import_crl_entry(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, 
    json_object **o_result_jso)
{
    gint32 ret = act_syslog_import_crl(i_paras, o_message_jso, o_result_jso);
    if (ret != HTTP_ACCEPTED && i_paras != NULL) {
        delete_user_specific_file(i_paras, i_paras->val_jso, RFACTION_PARAM_CONTENT);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
    }
    return ret;
}


LOCAL gint32 act_syslog_delete_crl(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    if (i_paras == NULL || o_message_jso == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: invalid parameter", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if ((i_paras->user_role_privilege & USERROLE_SECURITYMGNT) == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    gint32 ret = is_import_cert_or_crl_task_exist(syslog_import_crl_status_monitor, i_paras, FILE_TYPE_SYSLOG_CRL,
        o_message_jso);
    if (ret != VOS_OK) {
        return ret;
    }

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_SYSLOG_CONFIG, METHOD_SYSLOG_MGNT_DELETE_CRL, AUTH_ENABLE,
        i_paras->user_role_privilege, NULL, NULL);
    if (ret == RET_OK) {
        ret = HTTP_OK;
    } else if (ret == DELETE_CRL_ERR_CRL_NOT_EXIST) {
        (void)create_message_info(MSGID_CRL_NOT_EXISTED, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: syslog CRL not exist, ret:%d", __FUNCTION__, ret);
        ret = HTTP_BAD_REQUEST;
    } else {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: delete CRL to syslog fail, ret:%d", __FUNCTION__, ret);
        ret = HTTP_INTERNAL_SERVER_ERROR;
    }

    return ret;
}

LOCAL gint32 act_syslog_delete_crl_entry(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return act_syslog_delete_crl(i_paras, o_message_jso, o_result_jso);
}


LOCAL gint32 _get_manager_syslog_crl_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guchar crl_enabled = 0;
    
    return_val_do_info_if_expr(o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_SYSLOG_CRL_VERIFICATION_ENABLED, &crl_enabled);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get syslog CRLVerificationEnabled fail, ret:%d", __FUNCTION__, ret));

    if (crl_enabled == ENABLED) {
        *o_result_jso = json_object_new_boolean(TRUE);
    } else {
        *o_result_jso = json_object_new_boolean(FALSE);
    }

    return HTTP_OK;
}


LOCAL gint32 _get_syslog_crl_property_str(PROVIDER_PARAS_S *i_paras, const gchar *origin_prop, const gchar *rf_prop,
    json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar str_val[CERT_ITEM_LEN_128] = {0};

    
    return_val_do_info_if_expr(o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dal_get_property_value_string(i_paras->obj_handle, origin_prop, str_val, sizeof(str_val));
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get property %s fail, ret:%d", __FUNCTION__, rf_prop, ret));

    if (strlen(str_val) == 0) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const gchar *)str_val);
    return HTTP_OK;
}


LOCAL gint32 _get_manager_syslog_crl_start_time(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return _get_syslog_crl_property_str(i_paras, PROPERTY_SYSLOG_CRL_START_TIME, RFPROP_SYSLOG_CRL_VALID_FROM,
        o_message_jso, o_result_jso);
}


LOCAL gint32 _get_manager_syslog_crl_expire_time(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return _get_syslog_crl_property_str(i_paras, PROPERTY_SYSLOG_CRL_EXPIRE_TIME, RFPROP_SYSLOG_CRL_VALID_TO,
        o_message_jso, o_result_jso);
}