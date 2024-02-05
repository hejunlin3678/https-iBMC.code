
#include "redfish_provider.h"

LOCAL gint32 _get_smtp_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _get_smtp_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 _get_smtp_service_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _get_smtp_service_address(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _get_smtp_tls_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 _get_smtp_anonymouse_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _get_smtp_send_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 _get_smtp_send_password(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _get_smtp_send_address(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 _get_smtp_email_subject(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _get_smtp_email_subjectcontains(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _get_smtp_alarm_severity(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_smtp_root_certificate(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _get_smtp_recipient_addresses(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _get_smtp_action(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);

LOCAL gint32 _set_smtp_service_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _set_smtp_service_address(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _set_smtp_tls_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 _set_smtp_anonymouse_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _set_smtp_send_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 _set_smtp_send_password(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _set_smtp_send_address(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 _set_smtp_email_subject(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _set_smtp_email_subjectcontains(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _set_smtp_email_alarm_severity(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _set_smtp_email_recipient_addresses(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 _act_smtp_import_root_cert_entry(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,json_object **o_result_jso);
LOCAL gint32 _act_smtp_sendtest(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);

LOCAL gint32 _get_smtp_actioninfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _get_smtp_server_port(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 _set_smtp_server_port(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_smtp_cert_verificatione_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_smtp_cert_verificatione_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S  g_manager_smtp_provider[] = {
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, _get_smtp_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, _get_smtp_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_SMTP_SERVICEENABLED, CLASS_SMTP_CONFIG, PROPERTY_SMTP_ENABLE, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, _get_smtp_service_enabled, _set_smtp_service_enabled, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_SMTP_CERTVERIFYENABLED, CLASS_SMTP_CONFIG, PROPERTY_SMTP_CERT_VERIFY_ENABLE, USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID, get_smtp_cert_verificatione_enabled, set_smtp_cert_verificatione_enabled, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_SMTP_ANONYMOUSENABLED, CLASS_SMTP_CONFIG, PROPERTY_SMTP_ANONYMOUS_MODE, USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID, _get_smtp_anonymouse_enabled, _set_smtp_anonymouse_enabled, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_SMTP_SERVICEADDRESS, CLASS_SMTP_CONFIG, PROPERTY_SMTP_SERVER_NAME, USERROLE_READONLY,  SYS_LOCKDOWN_FORBID, _get_smtp_service_address, _set_smtp_service_address, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_SMTP_SERVERPORT, CLASS_SMTP_CONFIG, PROPERTY_SMTP_SERVER_PORT, USERROLE_READONLY,
     SYS_LOCKDOWN_FORBID, _get_smtp_server_port, _set_smtp_server_port, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_SMTP_TLSENABLED, CLASS_SMTP_CONFIG, PROPERTY_SMTP_TLS_SEND_MODE, USERROLE_READONLY,  SYS_LOCKDOWN_FORBID, _get_smtp_tls_enabled, _set_smtp_tls_enabled, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_SMTP_SENDERUSERNAME, CLASS_SMTP_CONFIG, PROPERTY_SMTP_LOGIN_NAME, USERROLE_READONLY,  SYS_LOCKDOWN_FORBID, _get_smtp_send_name, _set_smtp_send_name, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_SMTP_SENDERPASSWORD, CLASS_SMTP_CONFIG, PROPERTY_SMTP_LOGIN_NAME, USERROLE_READONLY,  SYS_LOCKDOWN_FORBID, _get_smtp_send_password, _set_smtp_send_password, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_SMTP_SENDERADDRESS, CLASS_SMTP_CONFIG, PROPERTY_SMTP_SENDER_NAME, USERROLE_READONLY,  SYS_LOCKDOWN_FORBID, _get_smtp_send_address, _set_smtp_send_address, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_SMTP_EMAILSUBJECT, CLASS_SMTP_CONFIG, PROPERTY_SMTP_TEMPLET_TOPIC, USERROLE_READONLY,  SYS_LOCKDOWN_FORBID, _get_smtp_email_subject, _set_smtp_email_subject, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_SMTP_EMAILSUBJECTCONTAINS, CLASS_SMTP_CONFIG, MAP_PROPERTY_NULL, USERROLE_READONLY,  SYS_LOCKDOWN_FORBID, _get_smtp_email_subjectcontains, _set_smtp_email_subjectcontains, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_SMTP_ALSRMSEVERITY, CLASS_SMTP_CONFIG, PROPERTY_SMTP_SEND_SEVERITY, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, _get_smtp_alarm_severity, _set_smtp_email_alarm_severity, NULL, ETAG_FLAG_ENABLE},
    {RF_ROOT_CERTIFICATE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        get_smtp_root_certificate, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_SMTP_RECIPIENTADDRESSES, CLASS_SMTP_ITEM_CONFIG, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, _get_smtp_recipient_addresses, _set_smtp_email_recipient_addresses, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ACTIONS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, _get_smtp_action, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_SMTP_POST_ACTION_NAME, CLASS_SMTP_CONFIG, MAP_PROPERTY_NULL, USERROLE_BASICSETTING, SYS_LOCKDOWN_ALLOW, NULL, NULL, _act_smtp_sendtest, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_SMTP_IMPORT_ROOT_CERT,  MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, NULL, NULL, _act_smtp_import_root_cert_entry, ETAG_FLAG_ENABLE}
};

LOCAL PROPERTY_PROVIDER_S  g_manager_actioninfo_smtp_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, _get_smtp_actioninfo_odata_id, NULL, NULL, ETAG_FLAG_ENABLE}
};


LOCAL gint32 __check_need_reset_sender_passwd(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso)
{
    gint32 ret;
    OBJ_HANDLE smtp_handle;
    guint8 anonymous_mode;
    const gchar *smtp_login_pass = NULL;

    ret = dfl_get_object_handle(OBJECT_SMTP_CONFIG, &smtp_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get smtp config handle failed, ret = %d", ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_property_value_byte(smtp_handle, PROPERTY_SMTP_ANONYMOUS_MODE, &anonymous_mode);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get smtp anonymous_mode failed, ret = %d", ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (anonymous_mode != 0) {
        if (get_element_str(i_paras->user_data, RFPROP_MANAGER_SMTP_SENDERPASSWORD, &smtp_login_pass) == FALSE) {
            debug_log(DLOG_ERROR, "%s, missing login password", __FUNCTION__);
            return HTTP_BAD_REQUEST;
        }
    }

    return RET_OK;
}


LOCAL PROPERTY_PROVIDER_S *_rfsmtp_get_self_provider(PROVIDER_PARAS_S *i_paras)
{
    return_val_if_fail((NULL != i_paras) && (i_paras->index >= 0) &&
        (i_paras->index < (gint32)(sizeof(g_manager_smtp_provider) / sizeof(PROPERTY_PROVIDER_S))),
        (PROPERTY_PROVIDER_S *)NULL);
    return &g_manager_smtp_provider[i_paras->index];
}


LOCAL gint32 _rfsmtp_string_check(const gchar *data)
{
    if (NULL == data || 0 == strlen(data) || 0 == g_strcmp0("N/A", data) || 0 == g_strcmp0("Unknown", data)) {
        return VOS_ERR;
    }

    return VOS_OK;
}


LOCAL gint32 _rfsmtp_get_property_string(PROPERTY_PROVIDER_S *self, gint32 num, PROVIDER_PARAS_S *i_paras,
    json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gchar value_string[MAX_CHARACTER_NUM] = {0};

    
    if (NULL == self || NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dal_get_object_handle_nth(self->pme_class_name, num, &obj_handle);
    
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));
    

    
    ret = dal_get_property_value_string(obj_handle, self->pme_prop_name, value_string, MAX_CHARACTER_NUM);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property_data fail.\n", __FUNCTION__, __LINE__));

    
    if (VOS_OK != _rfsmtp_string_check(value_string)) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    *o_result_jso = json_object_new_string((const char *)value_string);
    return_val_do_info_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));

    return HTTP_OK;
}


LOCAL gint32 _rfsmtp_get_property_byte_to_bool(PROPERTY_PROVIDER_S *self, gint32 num, PROVIDER_PARAS_S *i_paras,
    json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guchar value_uchar = 0;

    
    if (NULL == self || NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dal_get_object_handle_nth(self->pme_class_name, num, &obj_handle);
    
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));
    

    
    ret = dal_get_property_value_byte(obj_handle, self->pme_prop_name, &value_uchar);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property_data fail.\n", __FUNCTION__, __LINE__));

    
    if (TRUE == value_uchar || FALSE == value_uchar) {
        *o_result_jso = json_object_new_boolean(value_uchar);
    } else {
        *o_result_jso = NULL;
    }

    return_val_do_info_if_fail(*o_result_jso != NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));

    return HTTP_OK;
}


LOCAL gint32 _get_smtp_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));
    ret = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, MANAGERS_SMTPSERVICE_METADATA, slot);
    return_val_do_info_if_expr(0 >= ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));

    
    *o_result_jso = json_object_new_string((const char *)uri);

    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 _get_smtp_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));
    ret = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, MANAGER_SMTPSERVICE_URI, slot);
    return_val_do_info_if_expr(0 >= ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));

    
    *o_result_jso = json_object_new_string((const char *)uri);

    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 _get_smtp_service_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return _rfsmtp_get_property_byte_to_bool(_rfsmtp_get_self_provider(i_paras), 0, i_paras, o_message_jso,
        o_result_jso);
}



LOCAL gint32 _get_smtp_service_address(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return _rfsmtp_get_property_string(_rfsmtp_get_self_provider(i_paras), 0, i_paras, o_message_jso, o_result_jso);
}


LOCAL gint32 _get_smtp_tls_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    return _rfsmtp_get_property_byte_to_bool(_rfsmtp_get_self_provider(i_paras), 0, i_paras, o_message_jso,
        o_result_jso);
}

LOCAL gint32 _get_smtp_anonymouse_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    json_object *json_obj = NULL;
    guchar value;
    ret = _rfsmtp_get_property_byte_to_bool(_rfsmtp_get_self_provider(i_paras), 0, i_paras, o_message_jso, &json_obj);
    
    return_val_do_info_if_fail(HTTP_OK == ret || json_obj == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));
    
    value = !json_object_get_boolean(json_obj);
    *o_result_jso = json_object_new_boolean(value);
    json_object_put(json_obj);
    return_val_do_info_if_fail(*o_result_jso != NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));

    return HTTP_OK;
}



LOCAL gint32 _get_smtp_send_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    return _rfsmtp_get_property_string(_rfsmtp_get_self_provider(i_paras), 0, i_paras, o_message_jso, o_result_jso);
}


LOCAL gint32 _get_smtp_send_password(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL gint32 _get_smtp_send_address(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    return _rfsmtp_get_property_string(_rfsmtp_get_self_provider(i_paras), 0, i_paras, o_message_jso, o_result_jso);
}


LOCAL gint32 _get_smtp_email_subject(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    return _rfsmtp_get_property_string(_rfsmtp_get_self_provider(i_paras), 0, i_paras, o_message_jso, o_result_jso);
}


LOCAL gint32 _get_smtp_email_subjectcontains(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guchar value_byte = 0xff;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dal_get_object_handle_nth(g_manager_smtp_provider[i_paras->index].pme_class_name, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_array();
    return_val_do_info_if_fail(*o_result_jso != NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));

    guint8 software_type = 0;

    (void)dal_get_software_type(&software_type);
    if (MGMT_SOFTWARE_TYPE_EM == software_type) {
        
        ret = dal_get_property_value_byte(obj_handle, PROPERTY_SMTP_TEMPLET_IPADDR, &value_byte);
        do_val_if_expr(VOS_OK == ret && TRUE == value_byte,
            json_object_array_add(*o_result_jso, json_object_new_string(RF_HOST_NAME_STR)));

        
        ret = dal_get_property_value_byte(obj_handle, PROPERTY_SMTP_TEMPLET_CHASSIS_NAME, &value_byte);
        do_val_if_expr(VOS_OK == ret && TRUE == value_byte,
            json_object_array_add(*o_result_jso, json_object_new_string(RF_CHASSIS_NAME_STR)));

        
        ret = dal_get_property_value_byte(obj_handle, PROPERTY_SMTP_TEMPLET_CHASSIS_LOCATION, &value_byte);
        do_val_if_expr(VOS_OK == ret && TRUE == value_byte,
            json_object_array_add(*o_result_jso, json_object_new_string(RF_CHASSIS_LOCATION_STR)));

        
        ret = dal_get_property_value_byte(obj_handle, PROPERTY_SMTP_TEMPLET_CHASSIS_SERIAL_NUMBER, &value_byte);
        do_val_if_expr(VOS_OK == ret && TRUE == value_byte,
            json_object_array_add(*o_result_jso, json_object_new_string(RF_CHASSIS_SERIAL_NUMBER_STR)));
    } else {
        
        ret = dal_get_property_value_byte(obj_handle, PROPERTY_SMTP_TEMPLET_IPADDR, &value_byte);
        do_val_if_expr(VOS_OK == ret && TRUE == value_byte,
            json_object_array_add(*o_result_jso, json_object_new_string(RF_HOST_NAME_STR)));

        
        ret = dal_get_property_value_byte(obj_handle, PROPERTY_SMTP_TEMPLET_BOARD_SN, &value_byte);
        do_val_if_expr(VOS_OK == ret && TRUE == value_byte,
            json_object_array_add(*o_result_jso, json_object_new_string(RF_BRDSN_STR)));

        
        ret = dal_get_property_value_byte(obj_handle, PROPERTY_SMTP_TEMPLET_ASSET, &value_byte);
        do_val_if_expr(VOS_OK == ret && TRUE == value_byte,
            json_object_array_add(*o_result_jso, json_object_new_string(RF_PRODUCT_ASSET_TAG_STR)));
    }

    return HTTP_OK;
}



LOCAL gint32 _get_smtp_alarm_severity(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guchar value_byte = 0xff;
    gchar value_string[MAX_CHARACTER_NUM] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dal_get_object_handle_nth(g_manager_smtp_provider[i_paras->index].pme_class_name, 0, &obj_handle);
    
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));
    
    
    ret = dal_get_property_value_byte(obj_handle, g_manager_smtp_provider[i_paras->index].pme_prop_name, &value_byte);
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));

    ret = redfish_alarm_severities_change(&value_byte, value_string, MAX_CHARACTER_NUM);
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));

    ret = redfish_prase_severity_str_to_jso((const gchar *)value_string, o_result_jso);
    return ret;
}

LOCAL gint32 get_smtp_root_certificate(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    GSList *output_list = NULL;
    const gchar* cert_str_info_arr[] = {
        RFPROP_CERT_ISSUER_INFO, RFPROP_CERT_SUBJECT_INFO, RFPROP_CERT_START_TIME, RFPROP_CERT_EXPIRATION,
        RFPROP_CERT_SN, RFPROP_CERT_SIG_ALGO, RFPROP_CERT_KEY_USAGE
    };
    const guint8 keylen_info_idx = 7;
    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "[%s, %d] : NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    if (!i_paras->is_satisfy_privi) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_SMTP_CONFIG, METHOD_SMTP_GET_CERT_INFO, AUTH_ENABLE,
        i_paras->user_role_privilege, NULL, &output_list);
    if (ret != RET_OK) {
        debug_log(DLOG_MASS, "[%s] Get root certificate info failed, ret:%d", __FUNCTION__, ret);
        uip_free_gvariant_list(output_list);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_object();
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "[%s] Json new object failed", __FUNCTION__);
        uip_free_gvariant_list(output_list);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    for (guint8 i = 0; i < sizeof(cert_str_info_arr) / sizeof(cert_str_info_arr[0]); i++) {
        const gchar* cert_info = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, i), NULL);
        json_object *cert_info_obj = json_object_new_string(cert_info);
        if (cert_info_obj == NULL) {
            debug_log(DLOG_ERROR, "[%s]: New %s obj failed", __FUNCTION__, cert_str_info_arr[i]);
        }
        json_object_object_add(*o_result_jso, cert_str_info_arr[i], cert_info_obj);
    }

    guint32 key_len = g_variant_get_uint32((GVariant *)g_slist_nth_data(output_list, keylen_info_idx));
    json_object* key_len_obj = json_object_new_uint64(key_len);
    if (key_len_obj == NULL) {
            debug_log(DLOG_ERROR, "[%s]: New %s obj failed", __FUNCTION__, RFPROP_CERT_KEY_LEN);
    }
    json_object_object_add(*o_result_jso, RFPROP_CERT_KEY_LEN, key_len_obj);
    
    uip_free_gvariant_list(output_list);

    return HTTP_OK;
}


LOCAL gint32 _get_smtp_recipient_addresses(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint32 count = 0;
    gint32 id_index;
    guchar memberid = 0;
    gchar  memberid_string[MAX_RSC_ID_LEN] = {0};
    OBJ_HANDLE obj_handle = 0;
    PROPERTY_PROVIDER_S prop;
    json_object *json_obj = NULL;
    json_object *json_obj_item = NULL;
    errno_t securec_rv;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dfl_get_object_count(g_manager_smtp_provider[i_paras->index].pme_class_name, &count);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_array();
    return_val_do_info_if_fail(*o_result_jso != NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));

    for (id_index = 0; (guint32)id_index < count; id_index++) {
        json_obj_item = json_object_new_object();
        // 获取MemberId
        ret = dal_get_object_handle_nth(g_manager_smtp_provider[i_paras->index].pme_class_name, id_index, &obj_handle);
        continue_do_info_if_fail(VOS_OK == ret, (void)json_object_put(json_obj_item);
            debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));

        ret = dal_get_property_value_byte(obj_handle, PROPERTY_SMTP_ITEM_INDEX_NUM, &memberid);
        continue_do_info_if_fail(VOS_OK == ret, (void)json_object_put(json_obj_item);
            debug_log(DLOG_ERROR, "%s, %d: get dal_get_property_value_byte fail.\n", __FUNCTION__, __LINE__));

        (void)snprintf_s(memberid_string, MAX_RSC_ID_LEN, MAX_RSC_ID_LEN - 1, "%u", memberid);

        json_object_object_add(json_obj_item, MEMBERID_STRING, json_object_new_string((const char *)memberid_string));

        // 获取Enabled
        (void)memset_s(&prop, sizeof(PROPERTY_PROVIDER_S), 0, sizeof(PROPERTY_PROVIDER_S));
        securec_rv = memcpy_s(prop.pme_class_name, MAX_CLASS_NAME,
            g_manager_smtp_provider[i_paras->index].pme_class_name, MAX_CLASS_NAME);
        do_val_if_expr(securec_rv != EOK, debug_log(DLOG_ERROR, "%s, %d: memcpy_s  error.", __FUNCTION__, __LINE__));
        securec_rv =
            memcpy_s(prop.pme_prop_name, MAX_PROP_NAME, PROPERTY_SMTP_ITEM_ENABLE, sizeof(PROPERTY_SMTP_ITEM_ENABLE));
        do_val_if_expr(securec_rv != EOK, debug_log(DLOG_ERROR, "%s, %d: memcpy_s  error.", __FUNCTION__, __LINE__));
        (void)_rfsmtp_get_property_byte_to_bool(&prop, id_index, i_paras, o_message_jso, &json_obj);
        json_object_object_add(json_obj_item, ENABLED_STRING, json_obj);
        json_obj = NULL;

        // 获取EmailAddress
        (void)memset_s(&prop, sizeof(PROPERTY_PROVIDER_S), 0, sizeof(PROPERTY_PROVIDER_S));
        securec_rv = memcpy_s(prop.pme_class_name, MAX_CLASS_NAME,
            g_manager_smtp_provider[i_paras->index].pme_class_name, MAX_CLASS_NAME);
        do_val_if_expr(securec_rv != EOK, debug_log(DLOG_ERROR, "%s, %d: memcpy_s  error.", __FUNCTION__, __LINE__));
        securec_rv = memcpy_s(prop.pme_prop_name, MAX_PROP_NAME, PROPERTY_SMTP_ITEM_EMAIL_NAME,
            sizeof(PROPERTY_SMTP_ITEM_EMAIL_NAME));
        do_val_if_expr(securec_rv != EOK, debug_log(DLOG_ERROR, "%s, %d: memcpy_s  error.", __FUNCTION__, __LINE__));
        (void)_rfsmtp_get_property_string(&prop, id_index, i_paras, o_message_jso, &json_obj);
        json_object_object_add(json_obj_item, RFPROP_MANAGER_SMTP_EMAILADDRESS, json_obj);
        json_obj = NULL;

        // 获取Description
        (void)memset_s(&prop, sizeof(PROPERTY_PROVIDER_S), 0, sizeof(PROPERTY_PROVIDER_S));
        securec_rv = memcpy_s(prop.pme_class_name, MAX_CLASS_NAME,
            g_manager_smtp_provider[i_paras->index].pme_class_name, MAX_CLASS_NAME);
        do_val_if_expr(securec_rv != EOK, debug_log(DLOG_ERROR, "%s, %d: memcpy_s  error.", __FUNCTION__, __LINE__));
        securec_rv = memcpy_s(prop.pme_prop_name, MAX_PROP_NAME, PROPERTY_SMTP_ITEM_EMAIL_DESC,
            sizeof(PROPERTY_SMTP_ITEM_EMAIL_DESC));
        do_val_if_expr(securec_rv != EOK, debug_log(DLOG_ERROR, "%s, %d: memcpy_s  error.", __FUNCTION__, __LINE__));

        (void)_rfsmtp_get_property_string(&prop, id_index, i_paras, o_message_jso, &json_obj);
        (void)json_object_object_add(json_obj_item, RFPROP_MANAGER_SMTP_DESCRIPTION, json_obj);
        json_obj = NULL;

        ret = json_object_array_add(*o_result_jso, json_obj_item);
        do_val_if_expr(VOS_OK != ret, json_object_put(json_obj_item);
            debug_log(DLOG_ERROR, "%s, %d: json_object_array_add  error.", __FUNCTION__, __LINE__));
        json_obj_item = NULL;
    }

    return HTTP_OK;
}


LOCAL gint32 _get_smtp_action(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gchar slot[SNMP_STRLEN_32] = {0};
    gchar uri[SNMP_STRLEN_256] = {0};
    json_object *tmp_json = NULL;
    json_object *jso = NULL;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    gint32 ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_object fail\n", __FUNCTION__, __LINE__));

    // 获取target的信息
    if (snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, RFPROP_MANAGER_SMTP_ACTION_TARGETURI, slot) <= 0) {
        debug_log(DLOG_ERROR, "%s, %d: snprintf_s fail\n", __FUNCTION__, __LINE__);
        (void)json_object_put(jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    tmp_json = json_object_new_string((const char *)uri);
    return_val_do_info_if_fail(NULL != tmp_json, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(jso);
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_string fail\n", __FUNCTION__, __LINE__));
    json_object_object_add(jso, RFPROP_TARGET, tmp_json);
    tmp_json = NULL;

    // 获取actioninfo的信息
    (void)memset_s(uri, sizeof(uri), 0x00, sizeof(uri));

    if (snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, RFPROP_MANAGER_SMTP_ACTION_INFOURI, slot) <= 0) {
        debug_log(DLOG_ERROR, "%s, %d: snprintf_s fail\n", __FUNCTION__, __LINE__);
        (void)json_object_put(jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    tmp_json = json_object_new_string((const char *)uri);
    return_val_do_info_if_fail(NULL != tmp_json, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(jso);
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_string fail\n", __FUNCTION__, __LINE__));
    json_object_object_add(jso, RFPROP_MANAGER_SMTP_ACTION_INFONAME, tmp_json);

    // 生成action信息
    *o_result_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(jso);
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_object fail\n", __FUNCTION__, __LINE__));

    json_object_object_add(*o_result_jso, RFPROP_MANAGER_SMTP_ACTION_NAME, jso);

    // 生成导入根证书的action的对象
    ret = add_all_action_prop(*o_result_jso, RFPROP_MANAGER_SMTP_IMPORT_ROOT_CERT, slot,
        RFACTION_SMTP_IMPORT_CERT_TARGETURI, RFACTION_SMTP_IMPORT_CERT_INFOURI);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Add smtp_import_cert action object fail");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}



LOCAL gint32 _rfsmtp_set_propery_bool(const gchar *set_method, const gchar *prop, PROVIDER_PARAS_S *i_paras,
    json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    json_bool set_value;

    GSList *input_list = NULL;
    GSList *output_list = NULL;

    
    if (NULL == set_method || NULL == prop || NULL == o_result_jso || NULL == o_message_jso ||
        VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    set_value = json_object_get_boolean(i_paras->val_jso);

    
    ret = dal_get_object_handle_nth(g_manager_smtp_provider[i_paras->index].pme_class_name, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    input_list = g_slist_append(input_list, g_variant_new_byte(set_value));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        g_manager_smtp_provider[i_paras->index].pme_class_name, set_method, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, &output_list);
    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    
    switch (ret) {
        case RFERR_SUCCESS:
        case VOS_OK:
            return HTTP_OK;

        case RFERR_INSUFFICIENT_PRIVILEGE: 
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop, o_message_jso, prop);
            return HTTP_FORBIDDEN;

        default: 
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}

LOCAL gint32 _handle_rfsmtp_set_propery_string_ret(gint32 ret, const gchar *prop, json_object **o_message_jso,
    const gchar *value)
{
    
    switch (ret) {
        case VOS_OK:
        case RFERR_SUCCESS:
            return HTTP_OK;

        case RFERR_INSUFFICIENT_PRIVILEGE: 
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop, o_message_jso, prop);
            return HTTP_FORBIDDEN;

        case RF_SENSOR_MODULE_FAILED: 
            if (g_strcmp0(prop, RFPROP_MANAGER_SMTP_SENDERPASSWORD) == 0) {
                (void)create_message_info(MSGID_PROP_FORMAT_ERR, prop, o_message_jso, RF_SENSITIVE_INFO, prop);
            } else {
                (void)create_message_info(MSGID_PROP_FORMAT_ERR, prop, o_message_jso, value, prop);
            }
            return HTTP_BAD_REQUEST;

        case RF_SENSOR_MODULE_STATE_ERR: 
            (void)create_message_info(MSGID_ANONYMOUS_SET_ERR, prop, o_message_jso);
            return HTTP_BAD_REQUEST;

        default: 
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}



LOCAL gint32 _rfsmtp_set_propery_string(const gchar *set_method, const gchar *prop, PROVIDER_PARAS_S *i_paras,
    json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    const gchar *set_value = NULL;

    GSList *input_list = NULL;
    GSList *output_list = NULL;

    
    if (NULL == set_method || NULL == prop || NULL == o_result_jso || NULL == o_message_jso ||
        VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (json_type_string != json_object_get_type(i_paras->val_jso)) {
        if (g_strcmp0(prop, RFPROP_MANAGER_SMTP_SENDERPASSWORD) == 0) {
            (void)create_message_info(MSGID_PROP_TYPE_ERR, prop, o_message_jso,
                (i_paras->val_jso == NULL) ? RF_VALUE_NULL : RF_SENSITIVE_INFO, prop);
        } else {
            (void)create_message_info(MSGID_PROP_TYPE_ERR, prop, o_message_jso,
                (i_paras->val_jso == NULL) ? RF_VALUE_NULL : dal_json_object_get_str(i_paras->val_jso), prop);
        }
        return HTTP_BAD_REQUEST;
    }

    
    set_value = dal_json_object_get_str(i_paras->val_jso);

    
    ret = dal_get_object_handle_nth(g_manager_smtp_provider[i_paras->index].pme_class_name, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    input_list = g_slist_append(input_list, g_variant_new_string(set_value));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        g_manager_smtp_provider[i_paras->index].pme_class_name, set_method, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, &output_list);
    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    return _handle_rfsmtp_set_propery_string_ret(ret, prop, o_message_jso, set_value);
}


LOCAL gint32 _set_smtp_service_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return _rfsmtp_set_propery_bool(METHOD_SMTP_SET_ENABLE, g_manager_smtp_provider[i_paras->index].property_name,
        i_paras, o_message_jso, o_result_jso);
}



LOCAL gint32 _set_smtp_service_address(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    ret = __check_need_reset_sender_passwd(i_paras, o_message_jso);
    if (ret == HTTP_BAD_REQUEST) { 
        (void)create_message_info(MSGID_MODIFY_FAIL_LACK_PROPERTY,
            g_manager_smtp_provider[i_paras->index].property_name, o_message_jso, RFPROP_MANAGER_SMTP_SERVICEADDRESS,
            RFPROP_MANAGER_SMTP_SENDERPASSWORD);
    }

    if (ret != RET_OK) {
        return ret;
    }

    return _rfsmtp_set_propery_string(METHOD_SMTP_SET_SMTP_SERVER,
        g_manager_smtp_provider[i_paras->index].property_name, i_paras, o_message_jso, o_result_jso);
}


LOCAL gint32 _set_smtp_tls_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    return _rfsmtp_set_propery_bool(METHOD_SMTP_TLS_SEND_MODE, g_manager_smtp_provider[i_paras->index].property_name,
        i_paras, o_message_jso, o_result_jso);
}


LOCAL gint32 _set_smtp_anonymouse_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    json_bool set_value;

    GSList *input_list = NULL;
    GSList *output_list = NULL;

    
    if (NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    set_value = !json_object_get_boolean(i_paras->val_jso);

    
    ret = dal_get_object_handle_nth(g_manager_smtp_provider[i_paras->index].pme_class_name, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));
    input_list = g_slist_append(input_list, g_variant_new_byte(set_value));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        g_manager_smtp_provider[i_paras->index].pme_class_name, METHOD_SMTP_SET_ANONYMOUS_MODE, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, &output_list);

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    
    switch (ret) {
        case RFERR_SUCCESS:
        case VOS_OK:
            return HTTP_OK;

        case RFERR_INSUFFICIENT_PRIVILEGE: 
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV,
                g_manager_smtp_provider[i_paras->index].property_name, o_message_jso,
                g_manager_smtp_provider[i_paras->index].property_name);
            return HTTP_FORBIDDEN;

        default: 
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 _set_smtp_send_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;

    ret = __check_need_reset_sender_passwd(i_paras, o_message_jso);
    if (ret == HTTP_BAD_REQUEST) {
        (void)create_message_info(MSGID_MODIFY_FAIL_LACK_PROPERTY,
            g_manager_smtp_provider[i_paras->index].property_name, o_message_jso, RFPROP_MANAGER_SMTP_SENDERUSERNAME,
            RFPROP_MANAGER_SMTP_SENDERPASSWORD);
    }

    if (ret != RET_OK) {
        return ret;
    }

    return _rfsmtp_set_propery_string(METHOD_SMTP_SET_LOGIN_NAME, g_manager_smtp_provider[i_paras->index].property_name,
        i_paras, o_message_jso, o_result_jso);
}


LOCAL gint32 _set_smtp_send_password(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;

    ret = _rfsmtp_set_propery_string(METHOD_SMTP_SET_LOGIN_PASSWD,
        g_manager_smtp_provider[i_paras->index].property_name, i_paras, o_message_jso, o_result_jso);
    json_string_clear(i_paras->val_jso);
    return ret;
}


LOCAL gint32 _set_smtp_send_address(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    return _rfsmtp_set_propery_string(METHOD_SMTP_SET_SENDER_NAME,
        g_manager_smtp_provider[i_paras->index].property_name, i_paras, o_message_jso, o_result_jso);
}


LOCAL gint32 _set_smtp_email_subject(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    return _rfsmtp_set_propery_string(METHOD_SMTP_SET_TEMPLET_TOPIC,
        g_manager_smtp_provider[i_paras->index].property_name, i_paras, o_message_jso, o_result_jso);
}


#define EM_IDENTITY_LENGTH 4 // EM发送smtp消息时的主机标识最大可选4个，hostname, chassisname, chassislocation,
                             // chassisSerialNumber
#define OTHER_BOARD_IDENTITY_LENGTH 3 // 非EM发送smtp消息时的主机标识最大可选3个，boardSN, product_assettag, hostname
LOCAL gint32 _set_smtp_email_subjectcontains(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint32 input_length;
    gint32 id_index = 0;
    const gchar *value_string = NULL;
    json_object *o_message_jso_temp = NULL;
    json_object *o_value_jso_temp = NULL;
    PROVIDER_PARAS_S i_paras_temp;
    gchar error_value[MAX_STRBUF_LEN] = {0};

    guchar host_name_flag = 0;
    guchar product_assettag_flag = 0;
    guchar boardsn_flag = 0;
    guchar chassis_name_flag = 0;
    guchar chassis_location_flag = 0;
    guchar chassis_serial_number_flag = 0;

    
    if (NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    // 权限判断
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_MANAGER_SMTP_EMAILSUBJECTCONTAINS, o_message_jso,
        RFPROP_MANAGER_SMTP_EMAILSUBJECTCONTAINS));

    // 依赖框架拦截类型
    input_length = json_object_array_length(i_paras->val_jso);
    guint8 software_type = 0;

    (void)dal_get_software_type(&software_type);
    if (MGMT_SOFTWARE_TYPE_EM == software_type) {
        if (input_length > EM_IDENTITY_LENGTH) {
            (void)create_message_info(MSGID_ARRAY_ITEM_EXCEED, RFPROP_MANAGER_SMTP_EMAILSUBJECTCONTAINS, o_message_jso,
                RFPROP_MANAGER_SMTP_EMAILSUBJECTCONTAINS);
            debug_log(DLOG_ERROR, "%s, %d: input_length too large.", __FUNCTION__, __LINE__);
            return HTTP_BAD_REQUEST;
        }
    } else {
        if (input_length > OTHER_BOARD_IDENTITY_LENGTH) {
            (void)create_message_info(MSGID_ARRAY_ITEM_EXCEED, RFPROP_MANAGER_SMTP_EMAILSUBJECTCONTAINS, o_message_jso,
                RFPROP_MANAGER_SMTP_EMAILSUBJECTCONTAINS);
            debug_log(DLOG_ERROR, "%s, %d: input_length too large.", __FUNCTION__, __LINE__);
            return HTTP_BAD_REQUEST;
        }
    }

    // 重复检测
    if (VOS_OK == json_array_duplicate_check(i_paras->val_jso, &id_index)) {
        ret = snprintf_s(error_value, MAX_STRBUF_LEN, MAX_STRBUF_LEN - 1, "%s/%d",
            RFPROP_MANAGER_SMTP_EMAILSUBJECTCONTAINS, id_index);
        return_val_do_info_if_expr(0 >= ret, HTTP_INTERNAL_SERVER_ERROR,
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

        (void)create_message_info(MSGID_PROP_ITEM_DUPLICATE, (const char *)error_value, o_message_jso,
            (const char *)error_value);
        debug_log(DLOG_ERROR, "%s, %d: item duplicate.", __FUNCTION__, __LINE__);
        return HTTP_BAD_REQUEST;
    }

    *o_message_jso = json_object_new_array();

    // 获取用户设置值
    for (id_index = 0; id_index < json_object_array_length(i_paras->val_jso); id_index++) {
        o_value_jso_temp = json_object_array_get_idx(i_paras->val_jso, id_index);
        do_if_expr(NULL == o_value_jso_temp, continue);
        value_string = dal_json_object_get_str(o_value_jso_temp);
        if (g_strcmp0(JSON_NULL_OBJECT_STR, value_string) == 0) {
            ret = snprintf_s(error_value, MAX_STRBUF_LEN, MAX_STRBUF_LEN - 1, "%s/%d",
                RFPROP_MANAGER_SMTP_EMAILSUBJECTCONTAINS, id_index);
            return_val_do_info_if_expr(ret <= 0, HTTP_INTERNAL_SERVER_ERROR,
                (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &o_message_jso_temp);
                (void)json_object_array_add(*o_message_jso, o_message_jso_temp));

            create_message_info(MSGID_PROP_TYPE_ERR, (const char *)error_value, &o_message_jso_temp,
                JSON_NULL_OBJECT_STR, (const char *)error_value);
            (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
            return HTTP_BAD_REQUEST;
        }

        (void)dal_get_software_type(&software_type);
        if (MGMT_SOFTWARE_TYPE_EM == software_type) {
            do_if_expr(0 == g_strcmp0(RF_HOST_NAME_STR, value_string), host_name_flag++; continue);
            do_if_expr(0 == g_strcmp0(RF_CHASSIS_NAME_STR, value_string), chassis_name_flag++; continue);
            do_if_expr(0 == g_strcmp0(RF_CHASSIS_LOCATION_STR, value_string), chassis_location_flag++; continue);
            do_if_expr(0 == g_strcmp0(RF_CHASSIS_SERIAL_NUMBER_STR, value_string), chassis_serial_number_flag++;
                continue);
        } else {
            do_if_expr(0 == g_strcmp0(RF_HOST_NAME_STR, value_string), host_name_flag++; continue);
            do_if_expr(0 == g_strcmp0(RF_PRODUCT_ASSET_TAG_STR, value_string), product_assettag_flag++; continue);
            do_if_expr(0 == g_strcmp0(RF_BRDSN_STR, value_string), boardsn_flag++; continue);
        }
    }

    // 构造结构体
    (void)memset_s(&i_paras_temp, sizeof(i_paras_temp), 0, sizeof(i_paras_temp));

    (void)memcpy_s(&i_paras_temp, sizeof(i_paras_temp), i_paras, sizeof(PROVIDER_PARAS_S));

    // 设置chassisname  理论不会失败
    i_paras_temp.val_jso = json_object_new_boolean(chassis_name_flag);
    return_val_do_info_if_expr(NULL == i_paras_temp.val_jso, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &o_message_jso_temp);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp));
    ret = _rfsmtp_set_propery_bool(METHOD_SMTP_SET_TEMPLET_CHASSIS_NAME,
        g_manager_smtp_provider[i_paras->index].property_name, &i_paras_temp, &o_message_jso_temp, o_result_jso);
    do_val_if_expr(HTTP_OK != ret || NULL != o_message_jso_temp,
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp); o_message_jso_temp = NULL);
    (void)json_object_put(i_paras_temp.val_jso);
    i_paras_temp.val_jso = NULL;

    // 设置chassislocation  理论不会失败
    i_paras_temp.val_jso = json_object_new_boolean(chassis_location_flag);
    return_val_do_info_if_expr(NULL == i_paras_temp.val_jso, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &o_message_jso_temp);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp));
    ret = _rfsmtp_set_propery_bool(METHOD_SMTP_SET_TEMPLET_CHASSIS_LOCATION,
        g_manager_smtp_provider[i_paras->index].property_name, &i_paras_temp, &o_message_jso_temp, o_result_jso);
    do_val_if_expr(HTTP_OK != ret || NULL != o_message_jso_temp,
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp); o_message_jso_temp = NULL);
    (void)json_object_put(i_paras_temp.val_jso);
    i_paras_temp.val_jso = NULL;

    // 设置chassisserialnumber  理论不会失败
    i_paras_temp.val_jso = json_object_new_boolean(chassis_serial_number_flag);
    return_val_do_info_if_expr(NULL == i_paras_temp.val_jso, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &o_message_jso_temp);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp));
    ret = _rfsmtp_set_propery_bool(METHOD_SMTP_SET_TEMPLET_CHASSIS_SERIAL_NUMBER,
        g_manager_smtp_provider[i_paras->index].property_name, &i_paras_temp, &o_message_jso_temp, o_result_jso);
    do_val_if_expr(HTTP_OK != ret || NULL != o_message_jso_temp,
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp); o_message_jso_temp = NULL);
    (void)json_object_put(i_paras_temp.val_jso);
    i_paras_temp.val_jso = NULL;

    // 设置hostname  理论不会失败
    i_paras_temp.val_jso = json_object_new_boolean(host_name_flag);
    return_val_do_info_if_expr(NULL == i_paras_temp.val_jso, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &o_message_jso_temp);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp));
    ret = _rfsmtp_set_propery_bool(METHOD_SMTP_SET_TEMPLET_IPADDR,
        g_manager_smtp_provider[i_paras->index].property_name, &i_paras_temp, &o_message_jso_temp, o_result_jso);
    do_val_if_expr(HTTP_OK != ret || NULL != o_message_jso_temp,
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
        o_message_jso_temp = NULL);
    (void)json_object_put(i_paras_temp.val_jso);
    i_paras_temp.val_jso = NULL;

    // 设置productassettag 理论不会失败
    i_paras_temp.val_jso = json_object_new_boolean(product_assettag_flag);
    return_val_do_info_if_expr(NULL == i_paras_temp.val_jso, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &o_message_jso_temp);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp));

    ret = _rfsmtp_set_propery_bool(METHOD_SMTP_SET_TEMPLET_ASSET, g_manager_smtp_provider[i_paras->index].property_name,
        &i_paras_temp, &o_message_jso_temp, o_result_jso);
    do_val_if_expr(HTTP_OK != ret || NULL != o_message_jso_temp,
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
        o_message_jso_temp = NULL);
    (void)json_object_put(i_paras_temp.val_jso);
    i_paras_temp.val_jso = NULL;

    // 设置BoardSn 理论不会失败
    i_paras_temp.val_jso = json_object_new_boolean(boardsn_flag);
    return_val_do_info_if_expr(NULL == i_paras_temp.val_jso, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &o_message_jso_temp);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp));

    ret = _rfsmtp_set_propery_bool(METHOD_SMTP_SET_TEMPLET_BOARD_SN,
        g_manager_smtp_provider[i_paras->index].property_name, &i_paras_temp, &o_message_jso_temp, o_result_jso);
    do_val_if_expr(HTTP_OK != ret || NULL != o_message_jso_temp,
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
        o_message_jso_temp = NULL);
    (void)json_object_put(i_paras_temp.val_jso);

    return HTTP_OK;
}



LOCAL gint32 _set_smtp_email_alarm_severity(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gchar set_value_string[MAX_CHARACTER_NUM] = {0};
    errno_t str_ret;
    guchar set_value_byte = 0;

    GSList *input_list = NULL;
    GSList *output_list = NULL;

    
    if (NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    str_ret = redfish_get_severity_from_jso(i_paras->val_jso, set_value_string, sizeof(set_value_string));
    return_val_do_info_if_fail(EOK == str_ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    ret = redfish_alarm_severities_change(&set_value_byte, set_value_string, 0);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    ret = dal_get_object_handle_nth(g_manager_smtp_provider[i_paras->index].pme_class_name, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    input_list = g_slist_append(input_list, g_variant_new_byte(set_value_byte));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        g_manager_smtp_provider[i_paras->index].pme_class_name, METHOD_SMTP_SET_SEND_SEVERITY, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, &output_list);

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    switch (ret) {
        case VOS_OK:
        case RFERR_SUCCESS:
            return HTTP_OK;

        case RFERR_INSUFFICIENT_PRIVILEGE: 
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV,
                g_manager_smtp_provider[i_paras->index].property_name, o_message_jso,
                g_manager_smtp_provider[i_paras->index].property_name);
            return HTTP_FORBIDDEN;

        default: 
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}

LOCAL gint32 _set_smtp_email_recipient_addresses(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint32 vos_ret = 0;
    guint32 recipient_input_count = 0;
    guint32 recipient_count = 0;
    guint32 id_index;
    gint32 http_ret = HTTP_BAD_REQUEST;
    json_object *item_jso = NULL;
    json_object *value_jso = NULL;
    json_object *o_message_jso_temp = NULL;
    const gchar *json_str = NULL;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;

    gchar prop_string[MAX_CHARACTER_NUM] = {0};

    
    if (NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    // 权限判断
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_MANAGER_SMTP_RECIPIENTADDRESSES, o_message_jso,
        RFPROP_MANAGER_SMTP_RECIPIENTADDRESSES));

    // 获取输入对象数组的长度
    if (json_type_array == json_object_get_type(i_paras->val_jso)) {
        recipient_input_count = json_object_array_length(i_paras->val_jso);
    }

    // 传入数组为空
    return_val_do_info_if_expr((0 == recipient_input_count), HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_ARRAY_REQUIRED_ITEM, RFPROP_MANAGER_SMTP_RECIPIENTADDRESSES, o_message_jso,
        RFPROP_MANAGER_SMTP_RECIPIENTADDRESSES);
        debug_log(DLOG_ERROR, "%s, %d: controller count is miss.\n", __FUNCTION__, __LINE__));

    // 获取当前BMC item个数
    ret = dfl_get_object_count(g_manager_smtp_provider[i_paras->index].pme_class_name, &recipient_count);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    if (recipient_input_count > recipient_count) {
        (void)create_message_info(MSGID_ARRAY_ITEM_EXCEED, RFPROP_MANAGER_SMTP_RECIPIENTADDRESSES, o_message_jso,
            RFPROP_MANAGER_SMTP_RECIPIENTADDRESSES);
        debug_log(DLOG_ERROR, "%s, %d: input_length too large.", __FUNCTION__, __LINE__);
        return HTTP_BAD_REQUEST;
    }

    // 生成消息体数组
    *o_message_jso = json_object_new_array();
    return_val_do_info_if_expr(NULL == *o_message_jso, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    // 遍历设置传入对象
    for (id_index = 0; id_index < recipient_input_count; id_index++) {
        obj_handle = 0;
        ret = dal_get_object_handle_nth(g_manager_smtp_provider[i_paras->index].pme_class_name, id_index, &obj_handle);
        return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &o_message_jso_temp);
            (void)json_object_array_add(*o_message_jso, o_message_jso_temp));

        // 获取数组中的一个对象,依赖框架对null 做处理
        item_jso = json_object_array_get_idx(i_paras->val_jso, id_index);
        // 设置Enabled 值
        if (TRUE == json_object_object_get_ex(item_jso, ENABLED_STRING, &value_jso)) {
            input_list = g_slist_append(input_list, g_variant_new_byte((guchar)json_object_get_boolean(value_jso)));
            ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
                g_manager_smtp_provider[i_paras->index].pme_class_name, METHOD_SMTP_ITEM_SET_ENABLE, AUTH_ENABLE,
                i_paras->user_role_privilege, input_list, NULL);
            uip_free_gvariant_list(input_list);
            input_list = NULL;
            do_val_if_expr(VOS_OK == ret, (http_ret = HTTP_OK));
            return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
                (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &o_message_jso_temp);
                (void)json_object_array_add(*o_message_jso, o_message_jso_temp));
        }

        // 设置EmailAddress 值
        if (TRUE == json_object_object_get_ex(item_jso, RFPROP_MANAGER_SMTP_EMAILADDRESS, &value_jso)) {
            
            json_str = dal_json_object_get_str(value_jso);
            return_val_do_info_if_expr(NULL == json_str, HTTP_INTERNAL_SERVER_ERROR,
                (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &o_message_jso_temp);
                (void)json_object_array_add(*o_message_jso, o_message_jso_temp));
            

            input_list = g_slist_append(input_list, g_variant_new_string(json_str));
            ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
                g_manager_smtp_provider[i_paras->index].pme_class_name, METHOD_SMTP_ITEM_SET_EMAIL_NAME, AUTH_ENABLE,
                i_paras->user_role_privilege, input_list, NULL);
            uip_free_gvariant_list(input_list);
            input_list = NULL;

            vos_ret = snprintf_s(prop_string, MAX_CHARACTER_NUM, MAX_CHARACTER_NUM - 1, "%s/%u/%s",
                g_manager_smtp_provider[i_paras->index].property_name, id_index, RFPROP_MANAGER_SMTP_EMAILADDRESS);
            do_val_if_expr(0 >= vos_ret, debug_log(DLOG_ERROR, "%s, %d: snprintf_s error.", __FUNCTION__, __LINE__));

            
            do_val_if_expr(VOS_OK == ret, (http_ret = HTTP_OK));
            do_val_if_expr(RF_SENSOR_MODULE_FAILED == ret,
                http_ret = (http_ret == HTTP_OK ? HTTP_OK : HTTP_BAD_REQUEST);
                (void)create_message_info(MSGID_PROP_FORMAT_ERR, prop_string, &o_message_jso_temp, json_str,
                prop_string);
                (void)json_object_array_add(*o_message_jso, o_message_jso_temp); o_message_jso_temp = NULL);

            return_val_do_info_if_expr(VOS_OK != ret && RF_SENSOR_MODULE_FAILED != ret, HTTP_INTERNAL_SERVER_ERROR,
                (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &o_message_jso_temp);
                (void)json_object_array_add(*o_message_jso, o_message_jso_temp));
        }

        // 设置Description 值
        if (TRUE == json_object_object_get_ex(item_jso, RFPROP_MANAGER_SMTP_DESCRIPTION, &value_jso)) {
            
            json_str = dal_json_object_get_str(value_jso);
            return_val_do_info_if_expr(NULL == json_str, HTTP_INTERNAL_SERVER_ERROR,
                (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &o_message_jso_temp);
                (void)json_object_array_add(*o_message_jso, o_message_jso_temp));
            

            input_list = g_slist_append(input_list, g_variant_new_string(json_str));
            ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
                g_manager_smtp_provider[i_paras->index].pme_class_name, METHOD_SMTP_ITEM_SET_EMAIL_DESC, AUTH_ENABLE,
                i_paras->user_role_privilege, input_list, NULL);
            uip_free_gvariant_list(input_list);
            input_list = NULL;

            vos_ret = snprintf_s(prop_string, MAX_CHARACTER_NUM, MAX_CHARACTER_NUM - 1, "%s/%u/%s",
                g_manager_smtp_provider[i_paras->index].property_name, id_index, RFPROP_MANAGER_SMTP_DESCRIPTION);
            do_val_if_expr(0 >= vos_ret, debug_log(DLOG_ERROR, "%s, %d: snprintf_s error.", __FUNCTION__, __LINE__));

            
            do_val_if_expr(VOS_OK == ret, (http_ret = HTTP_OK));
            do_val_if_expr(RF_SENSOR_MODULE_FAILED == ret,
                http_ret = (http_ret == HTTP_OK ? HTTP_OK : HTTP_BAD_REQUEST);
                (void)create_message_info(MSGID_PROP_FORMAT_ERR, prop_string, &o_message_jso_temp, json_str,
                prop_string);
                (void)json_object_array_add(*o_message_jso, o_message_jso_temp); o_message_jso_temp = NULL);

            return_val_do_info_if_expr(VOS_OK != ret && RF_SENSOR_MODULE_FAILED != ret, HTTP_INTERNAL_SERVER_ERROR,
                (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &o_message_jso_temp);
                (void)json_object_array_add(*o_message_jso, o_message_jso_temp));
        }
    }

    
    if (TRUE == json_object_object_get_ex(i_paras->user_data, RFPROP_MANAGER_SMTP_RECIPIENTADDRESSES, &value_jso) &&
        TRUE == check_array_object(value_jso)) {
        (void)create_message_info(MSGID_ARRAY_REQUIRED_ITEM, RFPROP_MANAGER_SMTP_RECIPIENTADDRESSES,
            &o_message_jso_temp, RFPROP_MANAGER_SMTP_RECIPIENTADDRESSES);
        ret = json_object_array_add(*o_message_jso, o_message_jso_temp);
        do_val_if_expr(VOS_OK != ret, json_object_put(o_message_jso_temp));
        http_ret = HTTP_BAD_REQUEST;
    }

    return http_ret;
}


LOCAL gint32 _act_smtp_sendtest(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gint32 item_no = 0;
    const gchar *str = NULL;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;

    json_bool bool_ret;
    json_object *obj_json = NULL;

    
    if ((VOS_OK != provider_paras_check(i_paras)) || (NULL == o_message_jso)) {
        debug_log(DLOG_ERROR, "%s %d NULL Pointer!.", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    if (NULL == i_paras->val_jso) {
        debug_log(DLOG_ERROR, "%s, %d, wrong value type,  %s", __FUNCTION__, __LINE__, SNMP_TRAPITEM_ID);
        (void)create_message_info(MSGID_PROP_MISSING, MEMBERID_STRING, o_message_jso, MEMBERID_STRING);

        return HTTP_BAD_REQUEST;
    }

    
    bool_ret = json_object_object_get_ex(i_paras->val_jso, MEMBERID_STRING, &obj_json);

    return_val_do_info_if_expr(FALSE == bool_ret, HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "%s, %d, wrong value type,  %s", __FUNCTION__, __LINE__, SNMP_TRAPITEM_ID);
        (void)create_message_info(MSGID_PROP_MISSING, MEMBERID_STRING, o_message_jso, MEMBERID_STRING));

    
    str = dal_json_object_get_str(obj_json);
    return_val_do_info_if_fail(NULL != str, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: dal_json_object_get_str fail\n", __FUNCTION__, __LINE__));

    
    ret = vos_str2int(str, 10, &item_no, NUM_TPYE_INT32);
    if (VOS_OK != ret || item_no < 0 || item_no > G_MAXUINT8) {
        // 转换失败 输入参数错误
        debug_log(DLOG_ERROR, "%s, %d, vos_str2int fail", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, MEMBERID_STRING, o_message_jso, str, MEMBERID_STRING);
        (void)create_message_info(MSGID_ACT_PARA_FORMAT_ERR, NULL, o_message_jso, str, MEMBERID_STRING,
            RFPROP_MANAGER_SMTP_POST_ACTION_NAME);
        return HTTP_BAD_REQUEST;
    }

    
    
    input_list = g_slist_append(input_list, g_variant_new_byte(item_no));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SMTP_CONFIG, METHOD_SMTP_SEND_TEST, AUTH_ENABLE, i_paras->user_role_privilege, input_list, &output_list);
    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    
    switch (ret) {
        case VOS_OK:
        case RFERR_SUCCESS:
            return HTTP_OK;

        case SMTP_ERR_ACCOUNT:       
        case SMTP_ERR_NAME_PASSWORD: 
        case SMTP_ERR_ADDRESS:       

            (void)create_message_info(MSGID_SERVICE_INCORRECT_SENDERINFO_ERR, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;

        case SMTP_ERR_REQUIRED_AUTH: 
        case SMTP_ERR_CONNECT:       
        case SMTP_ERR_QUIT:          

            (void)create_message_info(MSGID_SERVICE_SMTP_CONNECTTION_ERR, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;

        case RF_ERROR:                 
        case SMTP_ERR_SEND_MSG:        
        case SMTP_ERR_RECEIVE_ADDRESS: 

            (void)create_message_info(MSGID_SERVICE_SENDING_EMAIL_ERR, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        
        case SMTP_ERR_ADDRESS_DISABLED: 
            (void)create_message_info(MSGID_FEATURE_BE_DISABLED, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        
        default: 
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }

    
}


LOCAL gint32 parse_cert_result_code(gint32 method_ret, json_object **message_info)
{
    gint32 result = HTTP_BAD_REQUEST;

    if (message_info == NULL) {
        debug_log(DLOG_ERROR, "NULL pointer.");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    switch (method_ret) {
        case RET_OK:
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
            (void)create_message_info(MSGID_CERT_IMPORT_FAILED, NULL, message_info);
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
            debug_log(DLOG_ERROR, "Unknown cert import error:0x%x", method_ret);
    }

    return result;
}

LOCAL gint32 check_local_cert_path(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, const gchar *content)
{
    gint32 ret;

    ret = dal_check_real_path(content);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "file real_path is illegal! return %d", ret);
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
        gint64 file_len = vos_get_file_length(content);
        // 文件大于1M，不继续
        // 此处使用代理方法删除文件
        if (file_len > IMPORT_CERTIFICATE_MAX_LEN) {
            (void)proxy_delete_file(i_paras->is_from_webui, i_paras->user_name, i_paras->client, content);
            (void)create_message_info(MSGID_CERT_IMPORT_FAILED, NULL, o_message_jso);
            json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
            return HTTP_BAD_REQUEST;
        }
    }

    return RET_OK;
}

LOCAL gint32 import_smtp_cert_check_param(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, const gchar **type, const gchar **content)
{
    gint32 ret;

    if ((i_paras == NULL) || (o_message_jso == NULL) || (o_result_jso == NULL)) {
        debug_log(DLOG_ERROR, "Input param error");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (!(i_paras->user_role_privilege & USERROLE_SECURITYMGNT)) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = get_element_str(i_paras->val_jso, RFACTION_PARAM_TYPE, type);
    if (ret != TRUE) {
        debug_log(DLOG_ERROR, "Cann't get %s from request", RFACTION_PARAM_TYPE);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
         return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = get_element_str(i_paras->val_jso, RFACTION_PARAM_CONTENT, content);
    if (ret != TRUE) {
        debug_log(DLOG_ERROR, "Cann't get %s from request", RFACTION_PARAM_CONTENT);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
         return HTTP_INTERNAL_SERVER_ERROR;
    }

    return RET_OK;
}

LOCAL gint32 import_smtp_cert(json_object *body_jso, json_object *user_data)
{
    gint32 ret;
    const gchar *remote_uri = NULL;
    const gchar *user_name = NULL;
    const gchar *client = NULL;
    GSList *input_list = NULL;
    gchar *slash_index = NULL;
    gchar cert_path[MAX_URI_LENGTH] = {0};
    gint32 from_webui_flag = REDFISH_REQ_FROM_OTHER_CLIENT;

    if (body_jso == NULL || user_data == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL Pointer", __FUNCTION__);
        return RET_ERR;
    }

    (void)get_element_str(user_data, RF_LOG_USER_NAME, &user_name);
    (void)get_element_str(user_data, RF_LOG_USER_IP, &client);
    (void)get_element_str(body_jso, RFACTION_PARAM_CONTENT, &remote_uri);
    (void)get_element_int(body_jso, RF_USERDATA_FROM_WEBUI_FLAG, &from_webui_flag);

    slash_index = g_strrstr(remote_uri, SLASH_FLAG_STR);
    if (slash_index == NULL) {
        debug_log(DLOG_ERROR, "%s: get file name failed", __FUNCTION__);
        json_object_clear_string(body_jso, RFACTION_PARAM_CONTENT);
        return RET_ERR;
    }

    ret = snprintf_s(cert_path, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, "/tmp%s", slash_index);
    if (ret <= 0) {
        json_object_clear_string(body_jso, RFACTION_PARAM_CONTENT);
        debug_log(DLOG_ERROR, "%s: snprintf_s failed", __FUNCTION__);
        return RET_ERR;
    }

    input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)cert_path));
    ret = uip_call_class_method_redfish((guchar)from_webui_flag, user_name, client, 0, CLASS_SMTP_CONFIG,
        METHOD_SMTP_MGNT_IMPORT_CERT, AUTH_DISABLE, 0, input_list, NULL);
    uip_free_gvariant_list(input_list);

    json_object_clear_string(body_jso, RFACTION_PARAM_CONTENT);

    return ret;
}

LOCAL TASK_MONITOR_INFO_S* file_trasfer_monitor_info_new(PROVIDER_PARAS_S *i_paras)
{
    TASK_MONITOR_INFO_S *monitor_info = NULL;
    json_object *user_data_jso = NULL;

    monitor_info = task_monitor_info_new((GDestroyNotify)monitor_info_free_func);
    if (monitor_info == NULL) {
        debug_log(DLOG_ERROR, "task_monitor_info_new failed");
        return NULL;
    }

    user_data_jso = json_object_new_object();
    if (user_data_jso == NULL) {
        debug_log(DLOG_ERROR, "alloc a new json object failed");
        task_monitor_info_free(monitor_info);
        return NULL;
    }

    json_object_object_add(user_data_jso, RF_LOG_USER_NAME, json_object_new_string(i_paras->user_name));
    json_object_object_add(user_data_jso, RF_LOG_USER_IP, json_object_new_string(i_paras->client));
    json_object_object_add(user_data_jso, RF_USERDATA_FROM_WEBUI_FLAG,
        json_object_new_int((gint32)i_paras->is_from_webui));

    monitor_info->user_data = user_data_jso;

    return monitor_info;
}

LOCAL gint32 import_smtp_cert_status_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    gint32 ret;
    gint32 file_down_progress = 0;

    monitor_fn_data->task_state = RF_TASK_RUNNING;

    ret = get_file_transfer_progress(&file_down_progress);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get upgrade packege download progress failed");
        goto err_exit;
    }

    // 文件传输失败，生成证书导入失败的错误消息
    if (file_down_progress < 0) {
        (void)parse_file_transfer_err_code(file_down_progress, message_obj);
        monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        monitor_fn_data->task_progress = 0;
        json_object_clear_string(body_jso, RFACTION_PARAM_CONTENT);
        return RET_OK;
    }

    debug_log(DLOG_DEBUG, "----------file down progress :%d---------", file_down_progress);

    if (file_down_progress != RF_FINISH_PERCENTAGE) {
        monitor_fn_data->task_progress = (guchar)(file_down_progress * 0.1); // 进度更新为文件传输进度的0.1
        return RET_OK;
    } else {
        monitor_fn_data->task_progress = 10; // 进度更新为10
        // 执行证书导入动作
        ret = import_smtp_cert(body_jso, (json_object *)monitor_fn_data->user_data);
        // 解析导入证书的错误码
        parse_cert_result_code(ret, message_obj);

        if (ret == RET_OK) {
            monitor_fn_data->task_state = RF_TASK_COMPLETED;
            monitor_fn_data->task_progress = 100; // 进度更新为100
        } else {
            monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        }
        json_object_clear_string(body_jso, RFACTION_PARAM_CONTENT);
        return RET_OK;
    }

err_exit:
    monitor_fn_data->task_state = RF_TASK_EXCEPTION;
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);
    json_object_clear_string(body_jso, RFACTION_PARAM_CONTENT);
    return RET_OK;
}

LOCAL gint32 import_root_cert_from_remot_uri(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint32 result = 0;
    TASK_MONITOR_INFO_S *file_trasfer_monitor_data = NULL;

    ret = __call_file_trasfer(i_paras, o_message_jso, &result, SMPT_ROOT_CERT_FILE_ID);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: call file trasfer failed", __FUNCTION__);
        return ret;
    }

    if (result != RET_OK) {
        (void)parse_file_transfer_err_code(result, o_message_jso);
        return HTTP_BAD_REQUEST;
    } else {
        file_trasfer_monitor_data = file_trasfer_monitor_info_new(i_paras);
        if (file_trasfer_monitor_data == NULL) {
            debug_log(DLOG_ERROR, "alloc a new file trasfer monitor info failed");
            goto err_exit;
        }

        file_trasfer_monitor_data->task_progress = TASK_NO_PROGRESS;
        file_trasfer_monitor_data->rsc_privilege = USERROLE_SECURITYMGNT;
        ret = create_new_task(TASK_IMPORT_ROOT_CERT, import_smtp_cert_status_monitor,
            file_trasfer_monitor_data, i_paras->val_jso, i_paras->uri, o_result_jso);
        if (ret != RET_OK) {
            task_monitor_info_free(file_trasfer_monitor_data);
            debug_log(DLOG_ERROR, "%s: create new task failed", __FUNCTION__);
            goto err_exit;
        }
    }

    return HTTP_ACCEPTED;

err_exit:
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);

    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL gint32 _act_smtp_import_root_cert(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    const gchar *content = NULL;
    const gchar *type = NULL;
    GSList *input_list = NULL;
    const gchar *cert_path = NULL;
    gint32 uri_type = 0xff;
    gboolean b_ret;

    gint32 ret = import_smtp_cert_check_param(i_paras, o_message_jso, o_result_jso, &type, &content);
    if (ret != RET_OK) {
        return ret;
    }

    // 如果type是text，保存为临时文件
    if (g_strcmp0(RFACTION_PARAM_VALUE_TEXT, type) == 0) {
        if (strlen(content) > IMPORT_CERTIFICATE_MAX_LEN) { // 证书文件大小门限为1M
            (void)create_message_info(MSGID_CERT_IMPORT_FAILED, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        }

        // 证书内容保存为临时文件
        ret = g_file_set_contents(TMP_SMTP_ROOT_CERT_FILE, content, strlen(content), NULL);
        if (ret != TRUE) {
            debug_log(DLOG_ERROR, "save certificate info to %s failed.", TMP_SMTP_ROOT_CERT_FILE);
            goto err_exit;
        }
        // 只需读权限即可，解析证书只需要读权限
        (void)chmod(TMP_SMTP_ROOT_CERT_FILE, S_IRUSR);

        cert_path = TMP_SMTP_ROOT_CERT_FILE;
    } else if (g_strcmp0(RFACTION_PARAM_VALUE_URI, type) == 0) {
        b_ret = g_regex_match_simple(SERVER_CERT_IMPORT_REGEX, content, G_REGEX_OPTIMIZE, (GRegexMatchFlags)0);
        if (b_ret != TRUE) {
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFACTION_PARAM_CONTENT, o_message_jso, RF_SENSITIVE_INFO,
                RFACTION_PARAM_CONTENT);
            return HTTP_BAD_REQUEST;
        }

        // 判断uri的类型
        ret = redfish_check_uri_type(content, &uri_type);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Get uri type failed.");
            goto err_exit;
        }

        if (uri_type == URI_TYPE_LOCAL) {
            ret = check_local_cert_path(i_paras, o_message_jso, content);
            if (ret != RET_OK) {
                return ret;
            }            
            cert_path = content;
        } else if (uri_type == URI_TYPE_REMOTE) {
            return import_root_cert_from_remot_uri(i_paras, o_message_jso, o_result_jso);
        } else {
            if (ret != TRUE) {
                debug_log(DLOG_ERROR, "invalid uri type.");
                goto err_exit;
            }
        }
    } else {
        if (ret != TRUE) {
            debug_log(DLOG_ERROR, "invalid type is %s", type);
            goto err_exit;
         }
    }

    input_list = g_slist_append(input_list, g_variant_new_string(cert_path));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_SMTP_CONFIG, METHOD_SMTP_MGNT_IMPORT_CERT, AUTH_DISABLE, 0, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "call method(%s) failed, ret:%d", METHOD_SMTP_MGNT_IMPORT_CERT, ret);
    }

    // 隐私数据清除
    return parse_cert_result_code(ret, o_message_jso);

err_exit:
    
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);

    return HTTP_INTERNAL_SERVER_ERROR;
}

LOCAL gint32 _act_smtp_import_root_cert_entry(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret = _act_smtp_import_root_cert(i_paras, o_message_jso, o_result_jso);
    if (ret != HTTP_ACCEPTED && i_paras != NULL && i_paras->val_jso != NULL) {
        delete_user_specific_file(i_paras, i_paras->val_jso, RFACTION_PARAM_CONTENT);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
    }
    return ret;
}



gint32 manager_smtp_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gint32 ret;
    guchar board_type = 0;

    ret = redfish_get_board_type(&board_type);
    return_val_if_fail(VOS_OK == ret, HTTP_NOT_FOUND);
    return_val_if_fail((BOARD_SWITCH != board_type), HTTP_NOT_FOUND);

    if (redfish_check_manager_uri_valid(i_paras->uri)) {
        *prop_provider = g_manager_smtp_provider;
        *count = sizeof(g_manager_smtp_provider) / sizeof(PROPERTY_PROVIDER_S);
        return VOS_OK;
    }

    return HTTP_NOT_FOUND;
}


LOCAL gint32 _get_smtp_actioninfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));
    ret = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, MANAGER_SMTPSERVICE_ACTIONINFO_URI, slot);
    return_val_do_info_if_expr(0 >= ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));

    
    *o_result_jso = json_object_new_string((const char *)uri);

    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


gint32 manager_smtp_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret;
    guchar board_type = 0;

    ret = redfish_get_board_type(&board_type);
    return_val_if_fail(VOS_OK == ret, HTTP_NOT_FOUND);
    return_val_if_fail(BOARD_SWITCH != board_type, HTTP_NOT_FOUND);

    if (redfish_check_manager_uri_valid(i_paras->uri)) {
        *prop_provider = g_manager_actioninfo_smtp_provider;
        *count = sizeof(g_manager_actioninfo_smtp_provider) / sizeof(PROPERTY_PROVIDER_S);
        return VOS_OK;
    }

    return HTTP_NOT_FOUND;
}


LOCAL gint32 _rfsmtp_set_propery_int32(const gchar *set_method, const gchar *prop, PROVIDER_PARAS_S *i_paras,
    json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gint32 set_value;

    GSList *input_list = NULL;
    GSList *output_list = NULL;

    
    if (set_method == NULL || prop == NULL || o_result_jso == NULL || o_message_jso == NULL ||
        provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "Invalid prameter.");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    set_value = json_object_get_int(i_paras->val_jso);

    ret = dal_get_object_handle_nth(g_manager_smtp_provider[i_paras->index].pme_class_name, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get object(%s) handle failed. ret = %d",
            g_manager_smtp_provider[i_paras->index].pme_class_name, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    input_list = g_slist_append(input_list, g_variant_new_int32(set_value));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        g_manager_smtp_provider[i_paras->index].pme_class_name, set_method, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, &output_list);
    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    
    switch (ret) {
        case RFERR_SUCCESS:
        case VOS_OK:
            return HTTP_OK;

        case RFERR_INSUFFICIENT_PRIVILEGE: 
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop, o_message_jso, prop);
            return HTTP_FORBIDDEN;

        default: 
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 _rfsmtp_get_property_int32(PROPERTY_PROVIDER_S *self, gint32 num, PROVIDER_PARAS_S *i_paras,
    json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gint32 value_uchar = 0;

    
    if (self == NULL || o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "Invalid prameter.");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (!(i_paras->is_satisfy_privi)) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = dal_get_object_handle_nth(self->pme_class_name, num, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get object(%s) handle failed. ret = %d",
            g_manager_smtp_provider[i_paras->index].pme_class_name, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_property_value_int32(obj_handle, self->pme_prop_name, &value_uchar);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get property_data fail. ret = %d ", ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    *o_result_jso = json_object_new_int(value_uchar);
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "o_result_jso is NULL.");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 _get_smtp_server_port(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    return _rfsmtp_get_property_int32(_rfsmtp_get_self_provider(i_paras), 0, i_paras, o_message_jso, o_result_jso);
}


LOCAL gint32 _set_smtp_server_port(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    return _rfsmtp_set_propery_int32(METHOD_SMTP_SET_SERVER_PORT, g_manager_smtp_provider[i_paras->index].property_name,
        i_paras, o_message_jso, o_result_jso);
}


LOCAL gint32 set_smtp_cert_verificatione_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return _rfsmtp_set_propery_bool(METHOD_SMTP_SET_CERT_VERIFY_ENABLE, g_manager_smtp_provider[i_paras->index].property_name,
        i_paras, o_message_jso, o_result_jso);
}


LOCAL gint32 get_smtp_cert_verificatione_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return _rfsmtp_get_property_byte_to_bool(_rfsmtp_get_self_provider(i_paras), 0, i_paras, o_message_jso,
        o_result_jso);
}