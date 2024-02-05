

#include "redfish_provider.h"
#include "../apps/bmc_global/inc/bmc.h"
#include "redfish_http.h"


struct url_user_data {
    json_object *target_jso;
    GHashTable *hash_table;
};
LOCAL PROPERTY_PROVIDER_S *get_self_provider_account_service(PROVIDER_PARAS_S *i_paras);



LOCAL gint32 get_oem_huawei_read_prop(PROVIDER_PARAS_S *i_paras, json_object **huawei_json);
LOCAL gint32 get_oem_huawei_mutual_authentication_info(PROVIDER_PARAS_S *i_paras, json_object *huawei_json);
LOCAL gint32 get_oem_huawei_actions(json_object *huawei_json);

LOCAL void parse_single_value_int(gboolean reading_success, json_object *obj_json, gchar *value_str, gint32 int_value);
LOCAL gint32 self_return_function(PROVIDER_PARAS_S *i_paras, gint32 ret_code, json_object **o_message_jso,
    const gchar *property_name);
LOCAL void get_set_accountservice_oem_ret_code(gint32 count, gint32 ret, gint32 *ret_code);

LOCAL gint32 set_security_banner_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, json_object *obj_json_mode);
LOCAL gint32 set_security_banner(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso,
    json_object *obj_json_mode);
LOCAL gint32 set_tls_version(PROVIDER_PARAS_S *i_paras, json_object *prop, json_object **o_message_jso);
LOCAL gint32 set_password_complexity_check(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, json_object *obj_json_mode);
LOCAL gint32 set_enable_user_mgnt(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso,
    json_object *obj_json_mode);
LOCAL gint32 set_pwdexpiredtime_and_minimumpwdage(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, json_object *obj_json_mode);
LOCAL gint32 set_old_pwd_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso,
    json_object *obj_json_mode);
LOCAL gint32 set_ssh_password_authentication(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, json_object *obj_json_mode);
LOCAL gint32 set_emergency_login_user(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, json_object *obj_json_mode);
LOCAL gint32 set_two_factor_authentication_state(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, json_object *obj_json_mode);
LOCAL gint32 set_certificate_revocation_check(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, json_object *obj_json_mode);
LOCAL gint32 set_cert_overdue_warn_time(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, json_object *obj_json_mode);
LOCAL gint32 process_import_local_root_cert(const gchar *file_path, json_object *user_data,
    json_object **o_message_jso);
LOCAL gint32 set_CRL_verification_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object *obj_json);


LOCAL gint32 get_accountservice_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 set_accountservice_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 act_import_root_certificate_entry(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 act_delete_root_certificate(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 set_account_inactive(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso,
    json_object *obj_json_mode);

LOCAL gint32 set_system_lockdown_state(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, json_object *obj_json_mode);


LOCAL gint32 get_accountservice_lockout_threshold(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return redfish_get_property_value_uint16(get_self_provider_account_service(i_paras), i_paras, o_message_jso,
        o_result_jso);
}


LOCAL gint32 get_accountservice_lockout_duration(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return redfish_get_property_value_uint16(get_self_provider_account_service(i_paras), i_paras, o_message_jso,
        o_result_jso);
}


LOCAL gint32 get_accountservice_password_min_len(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    guint8 min_length = 0;
    gint32 ret;
    PROPERTY_PROVIDER_S *self = get_self_provider_account_service(i_paras);
    guint8 default_min_len;
    
    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != o_result_jso) && (NULL != self),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input parm error", __FUNCTION__));
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, RFPROP_ACCOUNTSERVICE_MIN_PSWLEN, o_message_jso));

    if (dal_is_customized_by_cmcc()) {
        default_min_len = USER_PASSWORD_CMCC_CUSTOMIZED_MIN_LEN;
    } else {
        default_min_len = USER_PASS_MIN_LEN;
    }

    ret = dfl_get_object_handle(OBJ_PASSWD_SETTING, &obj_handle);
    if (ret == DFL_OK) {
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_MIN_PASSWD_LENGTH, &min_length);
    }
    
    if (min_length < default_min_len || min_length > PWSWORDMAXLENGH) {
        min_length = default_min_len;
    }
    *o_result_jso = json_object_new_int((gint32)min_length);

    return HTTP_OK;
}


LOCAL gint32 set_passwd_min_len(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
 
                                json_object** o_result_jso, guint8 min_length)
{
    OBJ_HANDLE obj_handle = 0;
    _cleanup_gvariant_slist_full_ GSList* input_list = NULL;
    json_object* message_temp_jso = NULL;

    // 系统锁定检查
    gint32 ret = redfish_system_lockdown_check(SYS_LOCKDOWN_FORBID);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s %d: system check lock down failed", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_SYSLOCKDOWN_FORBID, RFPROP_ACCOUNTSERVICE_MIN_PSWLEN, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso);
        return HTTP_BAD_REQUEST;
    }
    ret = dfl_get_object_handle(CLASS_PASSWD_SETTING, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s get PasswdSetting handle failed", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    input_list = g_slist_append(input_list, g_variant_new_byte(min_length));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
                                        CLASS_PASSWD_SETTING, METHOD_PASSWD_SETTING_SETMINPASSWDLENGTH, AUTH_ENABLE,
                                        i_paras->user_role_privilege, input_list, NULL);
    return self_return_function(i_paras, ret, o_message_jso, RFPROP_ACCOUNTSERVICE_MIN_PSWLEN);
}


LOCAL gint32 set_accountservice_password_min_len(PROVIDER_PARAS_S* i_paras, 
    json_object** o_message_jso, json_object** o_result_jso)
{
    
    if (provider_paras_check(i_paras) != VOS_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    guint8 min_length = LOW((guint32)json_object_get_int(i_paras->val_jso));
    debug_log(DLOG_ERROR, "get min_length = %d", min_length);
    *o_message_jso = json_object_new_array();
    if (*o_message_jso == NULL) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "New json array failed.");
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    gint32 ret_code = set_passwd_min_len(i_paras, o_message_jso, o_result_jso, min_length);
    return ret_code;
}

LOCAL gint32 get_accountservice_password_max_len(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    PROPERTY_PROVIDER_S *self = get_self_provider_account_service(i_paras);

    
    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != o_result_jso) && (NULL != self),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input parm error", __FUNCTION__));
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, RFPROP_ACCOUNTSERVICE_MAX_PSWLEN, o_message_jso));

    
    *o_result_jso = json_object_new_int(USER_PASSWORD_SIZE_2);

    return HTTP_OK;
}


LOCAL gint32 set_accountservice_lockout_threshold(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    return redfish_set_property_value_uint16(get_self_provider_account_service(i_paras), i_paras, o_message_jso,
        o_result_jso, METHOD_SECURITY_ENHANCE_SETAUTHFAILMAX_EXTENSION, USERROLE_SECURITYMGNT);
    
}

LOCAL gint32 set_accountservice_lockout_duration(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    return redfish_set_property_value_uint16(get_self_provider_account_service(i_paras), i_paras, o_message_jso,
        o_result_jso, METHOD_SECURITY_ENHANCE_SETAUTHFAILLOCKTIME_EXTENSION, USERROLE_SECURITYMGNT);
    
}


LOCAL PROPERTY_PROVIDER_S accountservice_provider[] = {
    {
        RFPROP_ACCOUNTSERVICE_MIN_PSWLEN,
        CLASS_PASSWD_SETTING, PROPERTY_MIN_PASSWD_LENGTH,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        get_accountservice_password_min_len,
        set_accountservice_password_min_len,
        NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_ACCOUNTSERVICE_MAX_PSWLEN,
        CLASS_SECURITY_ENHANCE, "\0",
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_accountservice_password_max_len,
        NULL,
        NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_ACCOUNTSERVICE_LOCKOUTTHRESHOLD,
        CLASS_SECURITY_ENHANCE, PROPERTY_AUTH_FAIL_MAX_EXTENSION,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        get_accountservice_lockout_threshold,
        set_accountservice_lockout_threshold,
        NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_ACCOUNTSERVICE_LOCKOUTDURATION,
        CLASS_SECURITY_ENHANCE, PROPERTY_AUTH_FAIL_LOCK_TIME_EXTENSION,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        get_accountservice_lockout_duration,
        set_accountservice_lockout_duration,
        NULL, ETAG_FLAG_ENABLE
    },
    
    {
        RFPROP_COMMON_OEM,
        CLASS_USER, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_ALLOW,
        get_accountservice_oem,
        set_accountservice_oem,
        NULL, ETAG_FLAG_ENABLE
    },
    {
        PROPERTY_ACCOUNT_SERVICE_IMPORT_ROOT_CERT,
        MUTUAL_AUTH_ROOT_CLASS_NAME, MAP_PROPERTY_NULL,
        USERROLE_SECURITYMGNT,
        SYS_LOCKDOWN_FORBID,
        NULL, NULL,
        act_import_root_certificate_entry, ETAG_FLAG_ENABLE
    },
    {
        PROPERTY_ACCOUNT_SERVICE_DELETE_ROOT_CERT,
        MUTUAL_AUTH_ROOT_CLASS_NAME, MAP_PROPERTY_NULL,
        USERROLE_SECURITYMGNT,
        SYS_LOCKDOWN_FORBID,
        NULL, NULL,
        act_delete_root_certificate, ETAG_FLAG_ENABLE
    },
    
    {
        PROPERTY_ACCOUNT_SERVICE_IMPORT_CRL,
        MUTUAL_AUTH_ROOT_CLASS_NAME, MAP_PROPERTY_NULL,
        USERROLE_SECURITYMGNT,
        SYS_LOCKDOWN_FORBID,
        NULL, NULL,
        act_import_mutual_auth_crl, ETAG_FLAG_ENABLE
    },
    
    
    {
        PROPERTY_ACCOUNT_SERVICE_DELETE_CRL,
        MUTUAL_AUTH_ROOT_CLASS_NAME, MAP_PROPERTY_NULL,
        USERROLE_SECURITYMGNT,
        SYS_LOCKDOWN_FORBID,
        NULL, NULL,
        act_delete_mutual_auth_crl, ETAG_FLAG_ENABLE
    }
};


LOCAL void parse_single_value_int(gboolean reading_success, json_object *obj_json, gchar *value_str, gint32 int_value)
{
    
    return_do_info_if_fail(NULL != obj_json && NULL != value_str,
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    if (!reading_success) {
        json_object_object_add(obj_json, value_str, NULL);
    } else {
        json_object_object_add(obj_json, value_str, json_object_new_int(int_value));
    }

    return;
}


LOCAL gint32 get_accountservice_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    json_object *huawei_json = NULL;

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi & USERROLE_READONLY, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, RFPROP_COMMON_OEM, o_message_jso));

    
    ret = get_oem_huawei_read_prop(i_paras, &huawei_json);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get oem read prop failed.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR, json_object_put(huawei_json);
        debug_log(DLOG_ERROR, "%s, %d: new object failed.", __FUNCTION__, __LINE__));

    
    json_object_object_add(*o_result_jso, RFPROP_OEM_HUAWEI, huawei_json);

    return HTTP_OK;
}

LOCAL gint32 get_oem_huawei_read_prop(PROVIDER_PARAS_S *i_paras, json_object **huawei_json)
{
    OBJ_HANDLE obj_handle = 0;
    guchar password_complexity_check_enable = 0;
    guchar ssh_security_enable = 0;
    guchar exclude_user = 0;
    guchar product_auth_enable = 0;
    gchar user_name[ARRAY_LENTH] = {0};
    gchar content[MOTD_FILE_MAX_LEN + 1] = {0};
    json_object *ldap_service = NULL;
    guint16 tls_version = 0;
    guint32 inact_time_limit = 0;
    guchar enable_user_mgnt = 0;
    guint32 pwd_expired_time = 0;
    guint32 minimum_pwd_age = 0;
    guchar old_pwd_count = 0;
    guchar system_lockdown_support = 0;
    guchar system_lockdown_state = 0;
    guint32 cert_overdue_warn_time = 0;
    json_object *tls_jso = NULL;
    guint32 cli_session_timeout = 0;
    json_object *krb_service = NULL;

    
    *huawei_json = json_object_new_object();
    return_val_do_info_if_fail(NULL != *huawei_json, VOS_ERR,
        debug_log(DLOG_ERROR, "%d: json_object_new_object failed.", __LINE__));

    gint32 ret = dal_get_object_handle_nth(OBJ_NAME_BMC, 0, &obj_handle);
    ret += dal_get_property_value_uint32(obj_handle, PROPERTY_BMC_CLI_SESSION_TIMEOUT, &cli_session_timeout);
    parse_single_value_int(ret == RET_OK, *huawei_json, PROP_ACCOUNT_CLI_SESSION_TIMEOUT,
        (gint32)cli_session_timeout);

    
    
    
    ret = dfl_get_object_handle(CLASS_PASSWD_SETTING, &obj_handle);
    ret += dal_get_property_value_byte(obj_handle, PROPERTY_USER_PASSWD_SETTING, &password_complexity_check_enable);
    parse_single_value(VOS_OK == ret, *huawei_json, PROPERTY_ACCOUNT_SERVICE_PASSWORD_COMPLEXITY_ENABLED,
        password_complexity_check_enable, NULL);

    // 获取不活动用户时间周期
    ret = dfl_get_object_handle(CLASS_SECURITY_ENHANCE, &obj_handle);
    ret += dal_get_property_value_uint32(obj_handle, PROPERTY_SECURITY_USER_INACT_TIME_LIMIT, &inact_time_limit);
    json_object_object_add(*huawei_json, PROP_ACCOUNT_INACTIVEA, 
        ((ret != RET_OK) ? NULL : json_object_new_int(inact_time_limit)));

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SECURITY_ENHANCE_PASSWORD_AUTHENTICATION,
        &ssh_security_enable);
    parse_single_value(VOS_OK == ret, *huawei_json, PROPERTY_ACCOUNT_SERVICE_SSH_AUTH_ENABLED, ssh_security_enable,
        NULL);

    
    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_SECURITY_CERT_OVERDUE_WARN_TIME, &cert_overdue_warn_time);
    do_val_if_expr(VOS_OK == ret, json_object_object_add(*huawei_json, PROPERTY_ACCOUNT_SERVICE_CERT_OVERDUE_WARN_TIME,
        json_object_new_int(cert_overdue_warn_time)));

    
    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_ENABLE_USER_MGNT, &enable_user_mgnt);
    parse_single_value(VOS_OK == ret, *huawei_json, PROPERTY_ACCOUNT_SERVICE_OS_USER_MANAGEMENT_ENABLED,
        enable_user_mgnt, NULL);

    
    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_PWD_EXPIRED_TIME, &pwd_expired_time);
    parse_single_value_int(VOS_OK == ret && 0 != pwd_expired_time, *huawei_json,
        PROPERTY_ACCOUNT_SERVICE_PASSWORD_VALIDITY_DAYS, (gint32)pwd_expired_time);

    
    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_SECURITY_MINIMUMPWDAGE, &minimum_pwd_age);
    parse_single_value_int(VOS_OK == ret && 0 != minimum_pwd_age, *huawei_json,
        PROPERTY_ACCOUNT_SERVICE_MINIMUM_PASSWORD_AGE_DAYS, (gint32)minimum_pwd_age);

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_OLD_PWD_COUNT, &old_pwd_count);
    parse_single_value_int(VOS_OK == ret && 0 != old_pwd_count, *huawei_json,
        PROPERTY_ACCOUNT_SERVICE_PREVIOUS_PASSWORDS_DISALLOWED_COUNT, (gint32)old_pwd_count);
    

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SECURITY_BANNERSTATE, &ssh_security_enable);
    parse_single_value(VOS_OK == ret, *huawei_json, PROPERTY_ACCOUNT_SERVICE_SECURITY_BANNER_ENABLED,
        ssh_security_enable, NULL);

    
    ret = dal_get_property_value_string(obj_handle, PROPERTY_SECURITY_BANNERCONTENT, content, sizeof(content));
    parse_single_value(VOS_OK == ret, *huawei_json, PROPERTY_ACCOUNT_SERVICE_SECURITY_BANNER, FALSE, (gchar*)content);

    
    ret = dal_get_property_value_string(obj_handle, PROPERTY_SECURITY_D_BANNERCONTENT, content, sizeof(content));
    parse_single_value(VOS_OK == ret, *huawei_json, PROPERTY_ACCOUNT_SERVICE_DEFAULT_SECURITY_BANNER, 
        FALSE, (gchar*)content);

    
    
    tls_jso = json_object_new_array();
    ret = dal_get_property_value_uint16(obj_handle, PROPERTY_TLS_VERSION, &tls_version);
    if (ret == VOS_OK && tls_jso != NULL) {
        if (tls_version & TLS_1_2_MASK) {
            json_object_array_add(tls_jso, json_object_new_string(RFPROP_TLS_12));
        }
        if (tls_version & TLS_1_3_MASK) {
            json_object_array_add(tls_jso, json_object_new_string(RFPROP_TLS_13));
        }
    }

    json_object_object_add(*huawei_json, PROPERTY_ACCOUNT_SERVICE_TLS, tls_jso);
    

    
    if (i_paras->user_role_privilege & USERROLE_USERMGNT) {
        
        gint32 ret_user = dal_get_property_value_byte(obj_handle, PROPERTY_EXCLUDE_USER, &exclude_user);
        
        ret = dal_get_specific_object_byte(CLASS_USER, PROPERTY_USER_ID, exclude_user, &obj_handle);
        if (VOS_OK == ret) {
            ret = dal_get_property_value_string(obj_handle, PROPERTY_USER_NAME, user_name, sizeof(user_name));
        }

        parse_single_value((VOS_OK == ret_user && 0 == exclude_user) || VOS_OK == ret, *huawei_json,
            PROPERTY_ACCOUNT_SERVICE_EMERGENCY_LOHIN_USER, FALSE, (gchar*)user_name);
    } else {
        parse_single_value(FALSE, *huawei_json, PROPERTY_ACCOUNT_SERVICE_EMERGENCY_LOHIN_USER, FALSE, 
            (gchar*)user_name);
    }

    if (i_paras->user_role_privilege & USERROLE_USERMGNT) {
        
        ret = dfl_get_object_handle(BMC_PRODUCT_NAME_APP, &obj_handle);
        ret += dal_get_property_value_byte(obj_handle, PROPERTY_MUTUAL_AUTHENTICATION_ENABLE, &product_auth_enable);
        if (VOS_OK != ret || TRUE != product_auth_enable) {
            
            json_object_object_add(*huawei_json, PROPERTY_ACCOUNT_SERVICE_TWO_FACTOR_AUTH_INFO, NULL);
        } else {
            
            ret = get_oem_huawei_mutual_authentication_info(i_paras, *huawei_json);
            if (VOS_OK != ret) {
                json_object_object_add(*huawei_json, PROPERTY_ACCOUNT_SERVICE_TWO_FACTOR_AUTH_INFO, NULL);
            }
        }
    } else {
        json_object_object_add(*huawei_json, PROPERTY_ACCOUNT_SERVICE_TWO_FACTOR_AUTH_INFO, NULL);
    }

    
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROPERTY_PME_SERVICECONFIG_SYSTEM_LOCKDOWN_SUPPORT,
        &system_lockdown_support);
    if (PME_SERVICE_SUPPORT == system_lockdown_support) {
        ret = dal_get_func_ability(OBJ_SECURITY_ENHANCE, PROPERTY_SECURITY_SYSTEMLOCKDOWNSTATE, &system_lockdown_state);
        parse_single_value(VOS_OK == ret, *huawei_json, PROPERTY_ACCOUNT_SERVICE_SYSTEM_LOCKDOWN_ENABLED,
            system_lockdown_state, NULL);
    }

    
    ret = get_oem_huawei_actions(*huawei_json);
    if (VOS_OK != ret) {
        json_object_object_add(*huawei_json, PROPERTY_ACCOUNT_SERVICE_ACTIONS, NULL);
    }

    
    ldap_service = json_object_new_object();
    if (NULL != ldap_service) {
        json_object_object_add(ldap_service, PROPERTY_LDAP_SERVICE_ODATA_ID,
            json_object_new_string(PROPERTY_LDAP_SERVICE_URL));
        json_object_object_add(*huawei_json, PROPERTY_LDAP_SERVICE, ldap_service);
    }

    
    if (is_kerberos_support()) {
        krb_service = json_object_new_object();
        if (krb_service != NULL) {
            json_object_object_add(krb_service, PROPERTY_KRB_SERVICE_ODATA_ID,
                json_object_new_string(PROPERTY_KRB_SERVICE_URL));
            json_object_object_add(*huawei_json, PROPERTY_KRB_SERVICE, krb_service);
        }
    }
    

    return VOS_OK;
}


LOCAL gint32 get_mutual_authentication_cert_info(OBJ_HANDLE obj_handle, guchar *cert_id, CERT_INFO_T *cert_info)
{
    gint32 ret;
    guint8 hash_id = 0;
    guint32 hash_val = 0;
    _cleanup_gvariant_slist_full_ GSList *input_list = NULL;

    
    ret = dal_get_property_value_byte(obj_handle, MUTUAL_AUTH_ROOT_CERT_ID, cert_id);
    return_val_do_info_if_expr(ret != VOS_OK, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get MUTUAL_AUTH_ROOT_CERT_ID failed", __FUNCTION__));

    (void)dal_get_property_value_string(obj_handle, MUTUAL_AUTH_ROOT_CERT_ISSUEBY, cert_info->issuer_info,
        sizeof(cert_info->issuer_info));
    (void)dal_get_property_value_string(obj_handle, MUTUAL_AUTH_ROOT_CERT_ISSUETO, cert_info->subject_info,
        sizeof(cert_info->subject_info));
    (void)dal_get_property_value_string(obj_handle, MUTUAL_AUTH_ROOT_CERT_VALIDFROM, cert_info->not_before,
        sizeof(cert_info->not_before));
    (void)dal_get_property_value_string(obj_handle, MUTUAL_AUTH_ROOT_CERT_VALIDTO, cert_info->not_after,
        sizeof(cert_info->not_after));
    (void)dal_get_property_value_string(obj_handle, MUTUAL_AUTH_ROOT_CERT_SERIALNUMBER, cert_info->serial,
        sizeof(cert_info->serial));

    if (!g_strcmp0(cert_info->serial, "")) {
        return VOS_ERR;
    }

    (void)dal_get_property_value_string(obj_handle, MUTUAL_AUTH_ROOT_CERT_SIGALGO, cert_info->sig_algo,
        sizeof(cert_info->sig_algo));
    if (!g_strcmp0(cert_info->sig_algo, "")) {
        
        (void)dal_get_property_value_byte(obj_handle, MUTUAL_AUTH_ROOT_CERT_HASHID, &hash_id);
        (void)dal_get_property_value_uint32(obj_handle, MUTUAL_AUTH_ROOT_CERT_HASHVALUE, &hash_val);
        input_list = g_slist_append(input_list, g_variant_new_byte(hash_id));
        input_list = g_slist_append(input_list, g_variant_new_uint32(hash_val));
        ret = dfl_call_class_method(obj_handle, MUTUAL_AUTH_ROOT_CERT_METHOD_REINIT, NULL, input_list, NULL);
        return_val_do_info_if_expr(ret != VOS_OK, VOS_ERR,
            debug_log(DLOG_ERROR, "%s: failed to reinit cert", __FUNCTION__));
        (void)dal_get_property_value_string(obj_handle, MUTUAL_AUTH_ROOT_CERT_SIGALGO, cert_info->sig_algo,
            sizeof(cert_info->sig_algo));
    }
    (void)dal_get_property_value_string(obj_handle, MUTUAL_AUTH_ROOT_CERT_KEYUSAGE, cert_info->key_usage,
        sizeof(cert_info->key_usage));
    (void)dal_get_property_value_uint32(obj_handle, MUTUAL_AUTH_ROOT_CERT_KEYLENGTH, &(cert_info->key_len));

    return VOS_OK;
}


LOCAL void add_rootcert_info_to_jso_array(OBJ_HANDLE obj_handle, json_object *jso_arr)
{
    CERT_INFO_T cert_info;
    guchar cert_id = 0;
    gchar cert_crl_path[MAX_FILEPATH_LENGTH] = {0};
    gint32 ret;
    json_object *root_authenticate_json = NULL;
    gchar crl_start_time[CERT_ITEM_LEN_128] = {0};
    gchar crl_expire_time[CERT_ITEM_LEN_128] = {0};

    ret = get_mutual_authentication_cert_info(obj_handle, &cert_id, &cert_info);
    return_if_expr(ret != VOS_OK);
    
    root_authenticate_json = json_object_new_object();
    return_do_info_if_expr(root_authenticate_json == NULL,
        debug_log(DLOG_ERROR, "%s: json_object_new_object fail", __FUNCTION__));

    
    json_object_object_add(root_authenticate_json, PROPERTY_ACCOUNT_SERVICE_CERT_ID, json_object_new_int(cert_id));
    
    parse_single_value(VOS_OK != g_strcmp0(cert_info.issuer_info, ""), root_authenticate_json,
        PROPERTY_ACCOUNT_SERVICE_ISSUE_BY, FALSE, cert_info.issuer_info);

    parse_single_value(VOS_OK != g_strcmp0(cert_info.subject_info, ""), root_authenticate_json,
        PROPERTY_ACCOUNT_SERVICE_ISSUE_TO, FALSE, cert_info.subject_info);
    parse_single_value(VOS_OK != g_strcmp0(cert_info.not_before, ""), root_authenticate_json,
        PROPERTY_ACCOUNT_SERVICE_VALID_FROM, FALSE, cert_info.not_before);

    parse_single_value(VOS_OK != g_strcmp0(cert_info.not_after, ""), root_authenticate_json,
        PROPERTY_ACCOUNT_SERVICE_VALID_TO, FALSE, cert_info.not_after);

    parse_single_value(VOS_OK != g_strcmp0(cert_info.serial, ""), root_authenticate_json,
        PROPERTY_ACCOUNT_SERVICE_SERIAL_NUMBER, FALSE, cert_info.serial);

    json_object_object_add(root_authenticate_json, PROPERTY_ACCOUNT_SERVICE_SIG_ALGO,
        json_object_new_string(cert_info.sig_algo));

    json_object_object_add(root_authenticate_json, PROPERTY_ACCOUNT_SERVICE_KEY_USAGE,
        json_object_new_string(cert_info.key_usage));

    json_object_object_add(root_authenticate_json, PROPERTY_ACCOUNT_SERVICE_KEY_LENGTH,
        json_object_new_int((gint32)cert_info.key_len));

    (void)dal_get_property_value_string(obj_handle, MUTUAL_AUTH_ROOT_CERT_CRL_FILEPATH, cert_crl_path,
        sizeof(cert_crl_path));

    (void)dal_get_property_value_string(obj_handle, MUTUAL_AUTH_ROOT_CERT_CRL_START_TIME, crl_start_time,
        sizeof(crl_start_time));

    (void)dal_get_property_value_string(obj_handle, MUTUAL_AUTH_ROOT_CERT_CRL_EXPIRE_TIME, crl_expire_time,
        sizeof(crl_expire_time));

    parse_single_value(TRUE, root_authenticate_json, PROPERTY_ACCOUNT_SERVICE_IS_IMPORT_CRL,
        (gchar)(g_strcmp0(cert_crl_path, "") != VOS_OK), NULL);

    parse_single_value(g_strcmp0(crl_start_time, ""), root_authenticate_json, PROPPETY_ACCOUNT_SERVICE_CRL_VALID_FROM,
        FALSE, (gchar*)crl_start_time);

    parse_single_value(g_strcmp0(crl_expire_time, ""), root_authenticate_json, PROPPETY_ACCOUNT_SERVICE_CRL_VALID_TO,
        FALSE, (gchar*)crl_expire_time);

    
    json_object_array_add(jso_arr, root_authenticate_json);
}


LOCAL gint32 get_oem_huawei_mutual_authentication_info(PROVIDER_PARAS_S *i_paras, json_object *huawei_json)
{
    gint32 ret;
    _cleanup_gslist_ GSList *cert_info_list = NULL;
    GSList *cert_one_node = NULL;
    json_object *authenticate_json = NULL;
    json_object *root_authenticate_arr = NULL;
    OBJ_HANDLE obj_handle;
    guchar two_auth_enable = 0;
    guchar none_cert_check = 0;
    guchar crl_check_enable = 0;

    
    return_val_do_info_if_expr((huawei_json == NULL || provider_paras_check(i_paras) != VOS_OK), VOS_ERR,
        debug_log(DLOG_ERROR, "%d: NULL pointer.", __LINE__);
        json_object_put(huawei_json));

    
    authenticate_json = json_object_new_object();
    return_val_do_info_if_expr(authenticate_json == NULL, VOS_ERR,
        debug_log(DLOG_ERROR, "%d: new json object failed.", __LINE__));

    
    ret = dfl_get_object_handle(MUTUAL_AUTH_CLASS_NAME, &obj_handle);
    ret =
        (gint32)((guint32)ret | (guint32)dal_get_property_value_byte(obj_handle, MUTUAL_AUTH_STATE, &two_auth_enable));
    parse_single_value(ret == VOS_OK, authenticate_json, PROPERTY_ACCOUNT_SERVICE_TWO_FACTOR_ENABLED, two_auth_enable,
        NULL);

    
    ret = dal_get_property_value_byte(obj_handle, MUTUAL_AUTH_OCSP, &none_cert_check);
    parse_single_value(ret == VOS_OK, authenticate_json, PROPERTY_ACCOUNT_SERVICE_CERT_CHECK_ENABLED, none_cert_check,
        NULL);

    
    ret = dal_get_property_value_byte(obj_handle, MUTUAL_AUTH_CRL, &crl_check_enable);
    parse_single_value(ret == VOS_OK, authenticate_json, PROPERTY_ACCOUNT_SERVICE_CRL_CHECK_ENABLED,
        (gchar)crl_check_enable, NULL);

    
    root_authenticate_arr = json_object_new_array();
    goto_label_do_info_if_expr(root_authenticate_arr == NULL, exit,
        debug_log(DLOG_DEBUG, "%d: new json array failed.", __LINE__));

    
    ret = dfl_get_object_list(MUTUAL_AUTH_ROOT_CLASS_NAME, &cert_info_list);
    goto_label_do_info_if_expr(ret != VOS_OK, exit,
        debug_log(DLOG_DEBUG, " %d: get authenticate obj_handle failed.", __LINE__));

    
    for (cert_one_node = cert_info_list; cert_one_node; cert_one_node = cert_one_node->next) {
        add_rootcert_info_to_jso_array((OBJ_HANDLE)cert_one_node->data, root_authenticate_arr);
    }

exit:
    
    json_object_object_add(authenticate_json, PROPERTY_ACCOUNT_SERVICE_ROOT_CERT, root_authenticate_arr);
    
    json_object_object_add(huawei_json, PROPERTY_ACCOUNT_SERVICE_TWO_FACTOR_AUTH_INFO, authenticate_json);

    return VOS_OK;
}

LOCAL gint32 get_oem_huawei_actions(json_object *huawei_json)
{
    
    return_val_do_info_if_fail(NULL != huawei_json, VOS_ERR, debug_log(DLOG_ERROR, "%d: NULL pointer.", __LINE__));

    
    json_object *actions = json_object_new_object();
    return_val_do_info_if_expr(actions == NULL, VOS_ERR,
        debug_log(DLOG_ERROR, "%d: new json object failed.", __LINE__));
    json_object *action_input = json_object_new_object();
    
    return_val_do_info_if_expr(action_input == NULL, VOS_ERR, json_object_put(actions);
        debug_log(DLOG_ERROR, "%d: new json object failed.", __LINE__));
    json_object_object_add(action_input, PROPERTY_ACCOUNT_SERVICE_TARGET,
        json_object_new_string(PROPERTY_ACCOUNT_SERVICE_IMPORT_ROOT_CERT_VALUE));
    json_object_object_add(action_input, PROPERTY_ACCOUNT_SERVICE_ACTION_INFO,
        json_object_new_string(PROPERTY_ACCOUNT_SERVICE_IMPORT_INFO));
    json_object_object_add(actions, PROPERTY_ACCOUNT_SERVICE_IMPORT_ROOT_CERT_KEY, action_input);
    
    json_object *action_delete = json_object_new_object();
    return_val_do_info_if_expr(action_delete == NULL, VOS_ERR, json_object_put(actions);
        debug_log(DLOG_ERROR, "%d: new json object failed.", __LINE__));
    json_object_object_add(action_delete, PROPERTY_ACCOUNT_SERVICE_TARGET,
        json_object_new_string(PROPERTY_ACCOUNT_SERVICE_DELETE_ROOT_CERT_VALUE));
    json_object_object_add(action_delete, PROPERTY_ACCOUNT_SERVICE_ACTION_INFO,
        json_object_new_string(PROPERTY_ACCOUNT_SERVICE_DELETE_INFO));
    json_object_object_add(actions, PROPERTY_ACCOUNT_SERVICE_DELETE_ROOT_CERT_KEY, action_delete);

    
    json_object *action_crl = json_object_new_object();
    return_val_do_info_if_expr(action_crl == NULL, VOS_ERR, json_object_put(actions);
        debug_log(DLOG_ERROR, "%d: new json object failed.", __LINE__));
    json_object_object_add(action_crl, PROPERTY_ACCOUNT_SERVICE_TARGET,
        json_object_new_string(PROPERTY_ACCOUNT_SERVICE_IMPORT_CRL_VALUE));
    json_object_object_add(action_crl, PROPERTY_ACCOUNT_SERVICE_ACTION_INFO,
        json_object_new_string(PROPERTY_ACCOUNT_SERVICE_IMPORT_CRL_ACTION_INFO));
    json_object_object_add(actions, PROPERTY_ACCOUNT_SERVICE_IMPORT_CRL_KEY, action_crl);

    
    json_object *action_crl_del = json_object_new_object();
    json_object_object_add(action_crl_del, PROPERTY_ACCOUNT_SERVICE_TARGET,
        json_object_new_string(PROPERTY_ACCOUNT_SERVICE_DELETE_CRL_VALUE));
    json_object_object_add(action_crl_del, PROPERTY_ACCOUNT_SERVICE_ACTION_INFO,
        json_object_new_string(PROPERTY_ACCOUNT_SERVICE_DELETE_CRL_ACTION_INFO));
    json_object_object_add(actions, PROPERTY_ACCOUNT_SERVICE_DELETE_CRL_KEY, action_crl_del);
    
    json_object_object_add(huawei_json, PROPERTY_ACCOUNT_SERVICE_ACTIONS, actions);

    return VOS_OK;
}


LOCAL gint32 self_return_function(PROVIDER_PARAS_S *i_paras, gint32 ret_code, json_object **o_message_jso,
    const gchar *property_name)
{
    json_object *message_temp_jso = NULL;

    switch (ret_code) {
        case VOS_OK:
        case RFERR_SUCCESS:
            return HTTP_OK;
            
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, property_name, &message_temp_jso);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            return RFERR_INSUFFICIENT_PRIVILEGE;
            
        case RFERR_NO_RESOURCE:
            (void)create_message_info(MSGID_RSC_MISSING_URI, property_name, &message_temp_jso, i_paras->uri);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            return HTTP_NOT_FOUND;
        case USER_SET_PASS_COMPLEXITY_CHECK_UNSUPPORT:
            (void)create_message_info(MSGID_USER_FORBID_SET_PASS_COMPLEXITY_CHECK, NULL, &message_temp_jso);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            return HTTP_BAD_REQUEST;
            
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, property_name, &message_temp_jso);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 set_cli_session_timeout(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, json_object** o_result_jso,
    json_object* obj_json_mode)
{
    OBJ_HANDLE obj_handle = 0;
    _cleanup_gvariant_slist_full_ GSList* input_list = NULL;
    guint32 cli_session_timeout;
    json_object* message_temp_jso = NULL;

    
    gint32 ret = redfish_system_lockdown_check(SYS_LOCKDOWN_FORBID);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s %d: system check failed", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_SYSLOCKDOWN_FORBID, PROP_ACCOUNT_CLI_SESSION_TIMEOUT, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso);
        return HTTP_BAD_REQUEST;
    }

    
    if (json_type_int != json_object_get_type(obj_json_mode)) {
        debug_log(DLOG_INFO, "%s json objetc type error", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_TYPE_ERR, PROP_ACCOUNT_CLI_SESSION_TIMEOUT,
            &message_temp_jso, dal_json_object_get_str(obj_json_mode), PROP_ACCOUNT_CLI_SESSION_TIMEOUT);
        (void)json_object_array_add(*o_message_jso, message_temp_jso);
        return HTTP_BAD_REQUEST;
    }

    
    ret = dfl_get_object_handle(OBJ_NAME_BMC, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_INFO, "%s get handle %s failed", __FUNCTION__, OBJ_NAME_BMC);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    cli_session_timeout = json_object_get_int(obj_json_mode);
    input_list = g_slist_append(input_list, g_variant_new_uint32(cli_session_timeout));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        BMC_CLASEE_NAME, METHOD_BMC_SET_NOTIMEOUT, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, NULL);

    return self_return_function(i_paras, ret, o_message_jso, PROP_ACCOUNT_CLI_SESSION_TIMEOUT);
}


LOCAL void get_set_accountservice_oem_ret_code(gint32 count, gint32 ret, gint32 *ret_code)
{
    if (HTTP_OK == ret || RFPROP_COOLING_SET_COUNT == count) {
        *ret_code = ret;
    } else {
        *ret_code = (HTTP_OK == *ret_code) ? *ret_code : ret;
    }
}
LOCAL void set_accountservice_oem_extra(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, json_object *val_json, gint32 count, gint32 *ret_code)
{
    json_object *obj_json_mode = NULL;
    gint32 ret;

    
    if (json_object_has_key(val_json, PROPERTY_ACCOUNT_SERVICE_PASSWORD_VALIDITY_DAYS) == TRUE ||
        json_object_has_key(val_json, PROPERTY_ACCOUNT_SERVICE_MINIMUM_PASSWORD_AGE_DAYS) == TRUE) {
        ret = set_pwdexpiredtime_and_minimumpwdage(i_paras, o_message_jso, o_result_jso, val_json);
        *ret_code = ret;
    }

    
    if (json_object_object_get_ex(val_json, PROPERTY_ACCOUNT_SERVICE_PREVIOUS_PASSWORDS_DISALLOWED_COUNT,
        &obj_json_mode) == TRUE) {
        ret = set_old_pwd_count(i_paras, o_message_jso, o_result_jso, obj_json_mode);
        *ret_code = ret;
    }
    

    
    if (json_object_object_get_ex(val_json, PROPERTY_ACCOUNT_SERVICE_SECURITY_BANNER_ENABLED, &obj_json_mode) == TRUE) {
        ret = set_security_banner_enabled(i_paras, o_message_jso, o_result_jso, obj_json_mode);
        *ret_code = ret;
    }

    
    if (json_object_object_get_ex(val_json, PROPERTY_ACCOUNT_SERVICE_SECURITY_BANNER, &obj_json_mode) == TRUE) {
        ret = set_security_banner(i_paras, o_message_jso, o_result_jso, obj_json_mode);
        *ret_code = ret;
    }

    
    if (json_object_object_get_ex(val_json, PROPERTY_ACCOUNT_SERVICE_TLS, &obj_json_mode)) {
        ret = set_tls_version(i_paras, obj_json_mode, o_message_jso);
        *ret_code = ret;
    }

    // 设置用户不活动周期时间
    if (json_object_object_get_ex(val_json, PROP_ACCOUNT_INACTIVEA, &obj_json_mode) == TRUE) {
        ret = set_account_inactive(i_paras, o_message_jso, o_result_jso, obj_json_mode);
        get_set_accountservice_oem_ret_code(count, ret, ret_code);
    }

    // 设置CLI会话自动断开时间
    if (json_object_object_get_ex(val_json, PROP_ACCOUNT_CLI_SESSION_TIMEOUT, &obj_json_mode) == TRUE) {
        ret = set_cli_session_timeout(i_paras, o_message_jso, o_result_jso, obj_json_mode);
        get_set_accountservice_oem_ret_code(count, ret, ret_code);
    }

    
    if (json_object_object_get_ex(val_json, PROPERTY_ACCOUNT_SERVICE_SSH_AUTH_ENABLED, &obj_json_mode) == TRUE) {
        ret = set_ssh_password_authentication(i_paras, o_message_jso, o_result_jso, obj_json_mode);
        get_set_accountservice_oem_ret_code(count, ret, ret_code);
    }

    
    if (json_object_object_get_ex(val_json, PROPERTY_ACCOUNT_SERVICE_EMERGENCY_LOHIN_USER, &obj_json_mode) == TRUE) {
        ret = set_emergency_login_user(i_paras, o_message_jso, o_result_jso, obj_json_mode);
        get_set_accountservice_oem_ret_code(count, ret, ret_code);
    }
    return;
}

LOCAL gint32 set_accountservice_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret = 0;
    json_object *val_json = NULL;
    json_object *obj_json_mode = NULL;
    json_object *obj_json_cert_mode = NULL;
    gint32 ret_code = HTTP_BAD_REQUEST;
    gint32 count;
    json_bool ret_bool;

    
    return_val_do_info_if_expr(provider_paras_check(i_paras) != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    
    
    ret_bool = json_object_object_get_ex(i_paras->val_jso, RFPROP_COMMON_HUAWEI, &val_json);
    return_val_if_expr(!ret_bool || (val_json == NULL), HTTP_BAD_REQUEST);
    
    
    *o_message_jso = json_object_new_array();
    return_val_do_info_if_expr(*o_message_jso == NULL, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s, %d: new array failed.", __FUNCTION__, __LINE__));

    
    count = json_object_object_length(val_json);
    if (json_object_object_get_ex(val_json, PROPERTY_ACCOUNT_SERVICE_TWO_FACTOR_AUTH_INFO, &obj_json_mode) == TRUE) {
        count += json_object_object_length(obj_json_mode) - 1;
    }

    
    if (json_object_object_get_ex(val_json, PROPERTY_ACCOUNT_SERVICE_SYSTEM_LOCKDOWN_ENABLED, &obj_json_mode) == TRUE) {
        
        ret = set_system_lockdown_state(i_paras, o_message_jso, o_result_jso, obj_json_mode);
        get_set_accountservice_oem_ret_code(count, ret, &ret_code);
    }

    
    if (json_object_object_get_ex(val_json, PROPERTY_ACCOUNT_SERVICE_PASSWORD_COMPLEXITY_ENABLED, &obj_json_mode) ==
        TRUE) {
        ret = set_password_complexity_check(i_paras, o_message_jso, o_result_jso, obj_json_mode);
        ret_code = ret;
    }

    
    if (json_object_object_get_ex(val_json, PROPERTY_ACCOUNT_SERVICE_OS_USER_MANAGEMENT_ENABLED, &obj_json_mode) ==
        TRUE) {
        ret = set_enable_user_mgnt(i_paras, o_message_jso, o_result_jso, obj_json_mode);
        ret_code = ret;
    }

    set_accountservice_oem_extra(i_paras, o_message_jso, o_result_jso, val_json, count, &ret_code);

    if (json_object_object_get_ex(val_json, PROPERTY_ACCOUNT_SERVICE_TWO_FACTOR_AUTH_INFO, &obj_json_mode) == TRUE) {
        
        do_info_if_true(json_object_object_get_ex(obj_json_mode, PROPERTY_ACCOUNT_SERVICE_TWO_FACTOR_ENABLED,
            &obj_json_cert_mode) == TRUE,
            ret = set_two_factor_authentication_state(i_paras, o_message_jso, o_result_jso, obj_json_cert_mode);
            get_set_accountservice_oem_ret_code(count, ret, &ret_code));

        
        do_info_if_true(json_object_object_get_ex(obj_json_mode, PROPERTY_ACCOUNT_SERVICE_CERT_CHECK_ENABLED,
            &obj_json_cert_mode) == TRUE,
            ret = set_certificate_revocation_check(i_paras, o_message_jso, o_result_jso, obj_json_cert_mode);
            get_set_accountservice_oem_ret_code(count, ret, &ret_code));

        
        do_info_if_true(json_object_object_get_ex(obj_json_mode, PROPERTY_ACCOUNT_SERVICE_CRL_CHECK_ENABLED,
            &obj_json_cert_mode) == TRUE,
            ret = set_CRL_verification_enabled(i_paras, o_message_jso, obj_json_cert_mode);
            get_set_accountservice_oem_ret_code(count, ret, &ret_code));
    }

    
    if (json_object_object_get_ex(val_json, PROPERTY_ACCOUNT_SERVICE_CERT_OVERDUE_WARN_TIME, &obj_json_mode) == TRUE) {
        ret = set_cert_overdue_warn_time(i_paras, o_message_jso, o_result_jso, obj_json_mode);
        get_set_accountservice_oem_ret_code(count, ret, &ret_code);
    }

    return ret_code;
}


LOCAL gint32 set_security_banner_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, json_object *obj_json_mode)
{
    OBJ_HANDLE obj_handle = 0;
    _cleanup_gvariant_slist_full_ GSList *input_list = NULL;
    guint8 security_banner_enabled;
    json_object *message_temp_jso = NULL;
    const gchar *prop_str = "Oem/Huawei/SecurityBannerEnabled";

    
    if (NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    // 系统锁定检查
    gint32 ret = redfish_system_lockdown_check(SYS_LOCKDOWN_FORBID);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST,
        debug_log(DLOG_INFO, "%s %d: system check failed", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_SYSLOCKDOWN_FORBID, prop_str, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    
    ret = dfl_get_object_handle(OBJ_SECURITY_ENHANCE, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s get security handle failed", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    security_banner_enabled = json_object_get_boolean(obj_json_mode) ? 1 : 0;
    input_list = g_slist_append(input_list, g_variant_new_byte(security_banner_enabled));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SECURITY_ENHANCE, METHOD_SECURITY_ENHANCE_SETBANNERSTATE, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, NULL);

    return self_return_function(i_paras, ret, o_message_jso, prop_str);
}


LOCAL gint32 set_security_banner(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso,
    json_object *obj_json_mode)
{
    OBJ_HANDLE obj_handle = 0;
    _cleanup_gvariant_slist_full_ GSList *input_list = NULL;
    json_object *message_temp_jso = NULL;
    gchar max_len_str[16] = {0};
    const gchar *prop_str = "Oem/Huawei/SecurityBanner";

    
    if (NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    gint32 ret = dfl_get_object_handle(OBJ_SECURITY_ENHANCE, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST,
        debug_log(DLOG_INFO, "%s %d: system check failed", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_SYSLOCKDOWN_FORBID, prop_str, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    
    if (json_object_get_string_len(obj_json_mode) > MOTD_FILE_MAX_LEN) {
        (void)snprintf_s(max_len_str, sizeof(max_len_str), sizeof(max_len_str) - 1, "%d", MOTD_FILE_MAX_LEN);
        (void)create_message_info(MSGID_PROP_VAL_EXC_MAXLEN, prop_str, &message_temp_jso, "", prop_str, 
            (const gchar*)max_len_str);
        (void)json_object_array_add(*o_message_jso, message_temp_jso);
        debug_log(DLOG_INFO, "%s input banner too long", __FUNCTION__);
        return HTTP_BAD_REQUEST;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(dal_json_object_get_str(obj_json_mode)));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SECURITY_ENHANCE, METHOD_SECURITY_ENHANCE_SETBANNERCONTENT, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, NULL);

    return self_return_function(i_paras, ret, o_message_jso, prop_str);
}


LOCAL gint32 set_tls_version(PROVIDER_PARAS_S *i_paras, json_object *prop, json_object **o_message_jso)
{
    _cleanup_gvariant_slist_full_ GSList *input_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    gint32 loop_tag_i;
    gint32 array_count;
    gint32 dup_index = 0;
    guint16 tls_version = 0xffff & ~TLS_1_0_MASK & ~TLS_1_1_MASK & ~TLS_1_2_MASK & ~TLS_1_3_MASK;
    const gchar *tls_str = NULL;
    json_object *message_jso = NULL;
    gchar name_parameter[MAX_CHARACTER_NUM] = {0};
    json_object *message_temp_jso = NULL;
    const gchar *prop_str = "Oem/Huawei/TlsVersion";

    if (NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras) || prop == NULL) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    // 系统锁定检查
    gint32 ret = redfish_system_lockdown_check(SYS_LOCKDOWN_FORBID);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST,
        debug_log(DLOG_INFO, "%s %d: system check failed", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_SYSLOCKDOWN_FORBID, prop_str, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    array_count = json_object_array_length(prop);
    
    if (array_count == 0 || array_count > TLS_VERSION_SUPPORT_COUNT) {
        (void)create_message_info(MSGID_ARRAY_ITEM_EXCEED, prop_str, &message_jso, prop_str);
        (void)json_object_array_add(*o_message_jso, message_jso);
        debug_log(DLOG_INFO, "%s, array length error %d.", __FUNCTION__, array_count);
        return HTTP_BAD_REQUEST;
    }

    if (RF_OK == json_array_duplicate_check(prop, &dup_index)) {
        (void)snprintf_s(name_parameter, sizeof(name_parameter), sizeof(name_parameter) - 1, "Oem/Huawei/TlsVersion/%d",
            dup_index);

        (void)create_message_info(MSGID_PROP_ITEM_DUPLICATE, prop_str, &message_jso, (const gchar*)name_parameter);
        (void)json_object_array_add(*o_message_jso, message_jso);
        debug_log(DLOG_INFO, "%s, json array duplicate", __FUNCTION__);
        return HTTP_BAD_REQUEST;
    }

    for (loop_tag_i = 0; loop_tag_i < array_count; loop_tag_i++) {
        tls_str = NULL;
        tls_str = dal_json_object_get_str(json_object_array_get_idx(prop, loop_tag_i));
        if (tls_str == NULL) {
            debug_log(DLOG_ERROR, "%s: dal_json_object_get_str fail.", __FUNCTION__);
            continue;
        }
        if (strcmp(tls_str, RFPROP_TLS_12) == 0) {
            tls_version |= TLS_1_2_MASK;
        } else if (strcmp(tls_str, RFPROP_TLS_13) == 0) {
            tls_version |= TLS_1_3_MASK;
        } else {
            (void)create_message_info(MSGID_PROP_ITEM_NOT_IN_LIST, prop_str, &message_jso, tls_str, prop_str);
            (void)json_object_array_add(*o_message_jso, message_jso);
            debug_log(DLOG_INFO, "%s, tls_version item error %s", __FUNCTION__, tls_str);
            return HTTP_BAD_REQUEST;
        }
    }

    debug_log(DLOG_DEBUG, "%s, tls_version = %d", __FUNCTION__, tls_version);

    (void)dal_get_object_handle_nth(CLASS_SECURITY_ENHANCE, 0, &obj_handle);
    input_list = g_slist_append(input_list, g_variant_new_uint16(tls_version));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SECURITY_ENHANCE, METHOD_SECURITY_ENHANCE_SETTLSVERSION, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, NULL);
    if (ret == TLS_VERSION_UNSUPPORT_ERR) {
        debug_log(DLOG_INFO, "%s not support tls version error", __FUNCTION__);
        (void)create_message_info(MSGID_TLS_VERSION_CONFIGURATION_UNSUPPORT, prop_str, o_message_jso);
        return HTTP_BAD_REQUEST;
    }

    if (ret == TLS_ERR_WITH_MUTUAL_AUTHENTICATIONS) {
        debug_log(DLOG_INFO, "set tls ver fail with mutual auth enable");
        (void)create_message_info(MSGID_MODIFY_TLS_FAIL_WITH_TWO_FACTORCERTIFICATION, prop_str, o_message_jso);
        return HTTP_BAD_REQUEST;
    }

    return self_return_function(i_paras, ret, o_message_jso, prop_str);
}


LOCAL gint32 set_password_complexity_check(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, json_object *obj_json_mode)
{
    OBJ_HANDLE obj_handle = 0;
    _cleanup_gvariant_slist_full_ GSList *input_list = NULL;
    _cleanup_gvariant_slist_full_ GSList* output_list = NULL;
    gboolean password_complexity_check;
    json_object *message_temp_jso = NULL;
    guint8 state;
    const gchar *prop_str = "Oem/Huawei/PasswordComplexityCheckEnabled";

    
    if (NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    // 系统锁定检查
    gint32 ret = redfish_system_lockdown_check(SYS_LOCKDOWN_FORBID);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST,
        debug_log(DLOG_INFO, "%s %d: system check failed", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_SYSLOCKDOWN_FORBID, prop_str, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    
    ret = dfl_get_object_handle(CLASS_PASSWD_SETTING, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, prop_str, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    
    password_complexity_check = json_object_get_boolean(obj_json_mode);
    if (password_complexity_check) {
        state = RFPROP_MANAGER_STATE_TRUE;
    } else {
        state = RFPROP_MANAGER_STATE_FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(state));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_PASSWD_SETTING, METHOD_PASSWD_SETTING_SETENABLEPWDCOMPLEXITY, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, &output_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, call %s faild, ret = %d.", __FUNCTION__, 
            METHOD_PASSWD_SETTING_SETENABLEPWDCOMPLEXITY, ret);
        return self_return_function(i_paras, ret, o_message_jso, prop_str);
    }

    ret = g_variant_get_int32((GVariant*)g_slist_nth_data(output_list, 0));
    return self_return_function(i_paras, ret, o_message_jso, prop_str);
}


LOCAL gint32 set_enable_user_mgnt(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso,
    json_object *obj_json_mode)
{
    OBJ_HANDLE obj_handle = 0;
    gboolean enable_user_mgnt;
    _cleanup_gvariant_slist_full_ GSList *input_list = NULL;
    guchar state;
    json_object *message_temp_jso = NULL;
    const gchar *prop_str = "Oem/Huawei/OSUserManagementEnabled";

    
    return_val_do_info_if_fail(NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error.", __FUNCTION__));

    // 系统锁定检查
    gint32 ret = redfish_system_lockdown_check(SYS_LOCKDOWN_FORBID);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST,
        debug_log(DLOG_INFO, "%s %d: system check failed", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_SYSLOCKDOWN_FORBID, prop_str, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    
    ret = dal_get_object_handle_nth(CLASS_SECURITY_ENHANCE, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get %s obj handle fail(%d).", __FUNCTION__, CLASS_SECURITY_ENHANCE, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    
    enable_user_mgnt = json_object_get_boolean(obj_json_mode);
    if (enable_user_mgnt) {
        state = RFPROP_MANAGER_STATE_TRUE;
    } else {
        state = RFPROP_MANAGER_STATE_FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(state));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SECURITY_ENHANCE, METHOD_SECURITY_ENHANCE_SETENABLEUSERMGNT, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, NULL);

    return self_return_function(i_paras, ret, o_message_jso, prop_str);
}


LOCAL gint32 set_pwdexpiredtime_and_minimumpwdage(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, json_object *obj_json_mode)
{
    OBJ_HANDLE obj_handle = 0;
    guint32 pwd_expired_time = 0;
    guint32 minimum_pwd_age = 0;
    json_object *pwd_expired_time_json = NULL;
    json_object *minimum_pwd_age_json = NULL;
    _cleanup_gvariant_slist_full_ GSList *input_list = NULL;
    json_object *message_temp_jso = NULL;
    guint32 pwdvaldays_flag = 0;
    guint32 minpwdagedays_flag = 0;
    guint32 ret_val = 0;
    guint32 ret_code = 0;
    const gchar *pwdvaild_str = "Oem/Huawei/PasswordValidityDays";
    const gchar *pwdage_str = "Oem/Huawei/MinimumPasswordAgeDays";

    
    return_val_do_info_if_fail(o_message_jso != NULL && provider_paras_check(i_paras) == VOS_OK,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error.", __FUNCTION__));

    // 系统锁定检查
    gint32 ret = redfish_system_lockdown_check(SYS_LOCKDOWN_FORBID);
    return_val_do_info_if_fail(ret == VOS_OK, HTTP_BAD_REQUEST,
        debug_log(DLOG_INFO, "%s %d: system check failed", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_SYSLOCKDOWN_FORBID, NULL, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    
    ret = dal_get_object_handle_nth(CLASS_SECURITY_ENHANCE, 0, &obj_handle);
    return_val_do_info_if_fail(ret == VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get %s obj handle fail(%d).", __FUNCTION__, CLASS_SECURITY_ENHANCE, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    
    
    if (json_object_object_get_ex(obj_json_mode, PROPERTY_ACCOUNT_SERVICE_PASSWORD_VALIDITY_DAYS,
        &pwd_expired_time_json) == TRUE) {
        if (json_object_is_type(pwd_expired_time_json, json_type_null)) {
            pwd_expired_time = 0;
        } else {
            pwd_expired_time = (guint32)json_object_get_int(pwd_expired_time_json);
        }
        pwdvaldays_flag = 1;
    } else {
        (void)dal_get_property_value_uint32(obj_handle, PROPERTY_PWD_EXPIRED_TIME, &pwd_expired_time);
    }
    input_list = g_slist_append(input_list, g_variant_new_uint32(pwd_expired_time));

    
    
    if (json_object_object_get_ex(obj_json_mode, PROPERTY_ACCOUNT_SERVICE_MINIMUM_PASSWORD_AGE_DAYS,
        &minimum_pwd_age_json) == TRUE) {
        if (json_object_is_type(minimum_pwd_age_json, json_type_null)) {
            minimum_pwd_age = 0;
        } else {
            minimum_pwd_age = (guint32)json_object_get_int(minimum_pwd_age_json);
        }
        minpwdagedays_flag = 1;
    } else {
        (void)dal_get_property_value_uint32(obj_handle, PROPERTY_SECURITY_MINIMUMPWDAGE, &minimum_pwd_age);
    }
    input_list = g_slist_append(input_list, g_variant_new_uint32(minimum_pwd_age));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SECURITY_ENHANCE, METHOD_SECURITY_ENHANCE_SETEXPIREDTIMEANDMINIMUMPWDAGE, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, NULL);

    return_val_do_info_if_expr(ret == USER_TIME_LIMIT_UNREASONABLE, HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "%s: minimum_pwd_age(%d) is no less than pwd_expired_time(%d) minus 10", __func__,
        minimum_pwd_age, pwd_expired_time);
        (void)create_message_info(MSGID_MINPWDAGE_AND_PWDVALIDITY_RESTRICT_EACH_OTHER, NULL, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    if (pwdvaldays_flag != 0 && minpwdagedays_flag != 0) {
        ret_val = (guint32)self_return_function(i_paras, ret, o_message_jso, pwdvaild_str);
        ret_code = (guint32)self_return_function(i_paras, ret, o_message_jso, pwdage_str);
        return (gint32)(ret_val & ret_code);
    } else if (pwdvaldays_flag != 0 && minpwdagedays_flag == 0) {
        return self_return_function(i_paras, ret, o_message_jso, pwdvaild_str);
    } else if (pwdvaldays_flag == 0 && minpwdagedays_flag != 0) {
        return self_return_function(i_paras, ret, o_message_jso, pwdage_str);
    }
    // 请求消息体中PasswordValidityDays和MinimumPasswordAgeDays两个资源都不进行设置的时候走该分支返回
    return self_return_function(i_paras, ret, o_message_jso, NULL);
}


LOCAL gint32 set_old_pwd_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso,
    json_object *obj_json_mode)
{
    OBJ_HANDLE obj_handle = 0;
    guchar old_pwd_count;
    _cleanup_gvariant_slist_full_ GSList *input_list = NULL;
    json_object *message_temp_jso = NULL;
    const gchar *prop_str = "Oem/Huawei/PreviousPasswordsDisallowedCount";

    
    return_val_do_info_if_fail(NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error.", __FUNCTION__));

    // 系统锁定检查
    gint32 ret = redfish_system_lockdown_check(SYS_LOCKDOWN_FORBID);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST,
        debug_log(DLOG_INFO, "%s %d: system check failed", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_SYSLOCKDOWN_FORBID, prop_str, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    
    ret = dal_get_object_handle_nth(CLASS_SECURITY_ENHANCE, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s get security handle failed %d", __FUNCTION__, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    
    if (json_object_is_type(obj_json_mode, json_type_null)) {
        old_pwd_count = 0;
    } else {
        old_pwd_count = (guchar)json_object_get_int(obj_json_mode);
    }
    input_list = g_slist_append(input_list, g_variant_new_byte(old_pwd_count));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SECURITY_ENHANCE, METHOD_SECURITY_ENHANCE_SETOLDPWDCOUNT, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, NULL);

    return self_return_function(i_paras, ret, o_message_jso, prop_str);
}


LOCAL gint32 set_account_inactive(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso,
    json_object *obj_json_mode)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    _cleanup_gvariant_slist_full_ GSList *input_list = NULL;
    guint32 inactive_day;
    json_object *message_temp_jso = NULL;

    
    if (NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    // 系统锁定检查
    ret = redfish_system_lockdown_check(SYS_LOCKDOWN_FORBID);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST,
        debug_log(DLOG_INFO, "%s %d: system check failed", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_SYSLOCKDOWN_FORBID, PROP_OEM_ACCOUNT_INACTIVEA, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    
    return_val_do_info_if_fail(json_type_int == json_object_get_type(obj_json_mode), HTTP_BAD_REQUEST,
        debug_log(DLOG_INFO, "%s json objetc type error", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_TYPE_ERR, PROP_OEM_ACCOUNT_INACTIVEA, &message_temp_jso,
        dal_json_object_get_str(obj_json_mode), PROP_ACCOUNT_INACTIVEA);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    
    ret = dfl_get_object_handle(CLASS_SECURITY_ENHANCE, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s get security handle error", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    
    inactive_day = json_object_get_int(obj_json_mode);

    input_list = g_slist_append(input_list, g_variant_new_uint32(inactive_day));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SECURITY_ENHANCE, METHOD_SECURITY_ENHANCE_SETUSERINACTTIMELIMIT, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, NULL);
    
    return_val_do_info_if_expr(USER_PARAMETER_OUT_OF_RANGE == ret, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, PROP_OEM_ACCOUNT_INACTIVEA, &message_temp_jso,
        dal_json_object_get_str(obj_json_mode), PROP_OEM_ACCOUNT_INACTIVEA);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    return self_return_function(i_paras, ret, o_message_jso, PROP_OEM_ACCOUNT_INACTIVEA);
}


LOCAL gint32 set_ssh_password_authentication(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, json_object *obj_json_mode)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    _cleanup_gvariant_slist_full_ GSList *input_list = NULL;
    gboolean ssh_password_authentication;
    guint8 state;
    json_object *message_temp_jso = NULL;
    const gchar *prop_str = "Oem/Huawei/SSHPasswordAuthenticationEnabled";

    
    if (NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    // 系统锁定检查
    ret = redfish_system_lockdown_check(SYS_LOCKDOWN_FORBID);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST,
        debug_log(DLOG_INFO, "%s %d: system check failed", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_SYSLOCKDOWN_FORBID, prop_str, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    
    ret = dfl_get_object_handle(CLASS_SECURITY_ENHANCE, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s get security handle failed ret %d", __FUNCTION__, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, prop_str, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    
    ssh_password_authentication = json_object_get_boolean(obj_json_mode);
    if (ssh_password_authentication) {
        state = RFPROP_MANAGER_STATE_TRUE;
    } else {
        state = RFPROP_MANAGER_STATE_FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(state));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SECURITY_ENHANCE, METHOD_SECURITY_ENHANCE_SETSSHPASSWORDAUTHENTICATION, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, NULL);

    return self_return_function(i_paras, ret, o_message_jso, prop_str);
}

LOCAL gint32 set_emergency_login_user(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, json_object *json_mode)
{
    int iRet;
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    guchar exclude_user = 0;
    guchar privilege = 0;
    gchar user_name[ARRAY_LENTH] = {0};
    json_object *message_temp_jso = NULL;
    guchar is_user_enabled = 0;
    const gchar *prop_str = "Oem/Huawei/EmergencyLoginUser";

    
    if (NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if ((i_paras->user_role_privilege & USERROLE_USERMGNT) == 0) {
        debug_log(DLOG_ERROR, "%s, privilege not usermgmt", __FUNCTION__);
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, prop_str, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso);
        return HTTP_FORBIDDEN;
    }

    // 系统锁定检查
    ret = redfish_system_lockdown_check(SYS_LOCKDOWN_FORBID);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST,
        debug_log(DLOG_INFO, "%s %d: system check failed", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_SYSLOCKDOWN_FORBID, prop_str, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    
    iRet = snprintf_s(user_name, sizeof(user_name), sizeof(user_name) - 1, "%s", dal_json_object_get_str(json_mode));
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    
    if (VOS_OK != g_strcmp0(user_name, "")) {
        
        ret = dal_get_specific_object_string(CLASS_USER, PROPERTY_USER_NAME, (const gchar*)user_name, &obj_handle);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST,
            debug_log(DLOG_INFO, "%s, get user %s handle failed", __FUNCTION__, user_name);
            (void)create_message_info(MSGID_INVALID_USERNAME, NULL, &message_temp_jso);
            (void)json_object_array_add(*o_message_jso, message_temp_jso));
        
        ret = dal_get_property_value_byte(obj_handle, PROPERTY_USER_PRIVILEGE, &privilege);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_INFO, "%s, get user privilege failed ret %d", __FUNCTION__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, prop_str, &message_temp_jso);
            (void)json_object_array_add(*o_message_jso, message_temp_jso));

        
        ret = dal_get_property_value_byte(obj_handle, PROPERTY_USER_ISENABLE, &is_user_enabled);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_INFO, "%s, get user isenabled failed ret %d", __FUNCTION__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, prop_str, &message_temp_jso);
            (void)json_object_array_add(*o_message_jso, message_temp_jso));

        return_val_do_info_if_fail(RMCP_ADMIN == privilege && RF_ENABLE == is_user_enabled, HTTP_BAD_REQUEST,
            debug_log(DLOG_INFO, "%s, user not enabled", __FUNCTION__);
            (void)create_message_info(MSGID_EMERGENCY_USER_FAILED, NULL, &message_temp_jso);
            (void)json_object_array_add(*o_message_jso, message_temp_jso));
        
        
        ret = dal_get_property_value_byte(obj_handle, PROPERTY_USER_ID, &exclude_user);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_INFO, "%s, get user id failed", __FUNCTION__);
            (void)create_message_info(MSGID_INTERNAL_ERR, prop_str, &message_temp_jso);
            (void)json_object_array_add(*o_message_jso, message_temp_jso));
    } 
    
    ret = dfl_get_object_handle(CLASS_SECURITY_ENHANCE, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s, get security handle failed", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, prop_str, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    
    input_list = g_slist_append(input_list, g_variant_new_byte(exclude_user));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SECURITY_ENHANCE, METHOD_SECURITY_ENHANCE_SETEXCLUDEUSER, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, NULL);

    uip_free_gvariant_list(input_list);

    return self_return_function(i_paras, ret, o_message_jso, prop_str);
}

LOCAL gint32 set_two_factor_authentication_state(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, json_object *obj_json_mode)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guchar product_auth_enable = 0;
    json_object *message_temp_jso = NULL;
    GSList *input_list = NULL;
    gboolean two_factor_authentication;
    guint8 state;
    const gchar *prop_str = "Oem/Huawei/TwoFactorAuthenticationInformation/TwoFactorAuthenticationStateEnabled";

    
    return_val_do_info_if_expr(NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    // 系统锁定检查
    ret = redfish_system_lockdown_check(SYS_LOCKDOWN_FORBID);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST,
        debug_log(DLOG_INFO, "%s %d: system check failed", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_SYSLOCKDOWN_FORBID, prop_str, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    
    ret = dfl_get_object_handle(BMC_PRODUCT_NAME_APP, &obj_handle);
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, prop_str, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_MUTUAL_AUTHENTICATION_ENABLE, &product_auth_enable);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, prop_str, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    return_val_do_info_if_expr(FALSE == product_auth_enable, HTTP_NOT_IMPLEMENTED,
        (void)create_message_info(MSGID_CERTIFICATE_2FA_NOT_SUPPORTED, NULL, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    
    two_factor_authentication = json_object_get_boolean(obj_json_mode);
    if (two_factor_authentication) {
        state = RFPROP_MANAGER_STATE_TRUE;
    } else {
        state = RFPROP_MANAGER_STATE_FALSE;
    }

    
    ret = dfl_get_object_handle(MUTUAL_AUTH_CLASS_NAME, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, prop_str, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    
    input_list = g_slist_append(input_list, g_variant_new_byte(state));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        MUTUAL_AUTH_CLASS_NAME, MUTUAL_AUTH_METHOD_SET_STATE, AUTH_ENABLE, i_paras->user_role_privilege, input_list,
        NULL);
    uip_free_gvariant_list(input_list);

    
    if (VOS_ERR_MA_NO_ISSUER == ret) {
        (void)create_message_info(MSGID_CERTIFICATE_FAILED_ENABLE_CERTIFICATE, NULL, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso);
        return HTTP_BAD_REQUEST;
    }

    
    if (ret == ENABLE_MUTUAL_AUTHENTICATIONS_ERR_WITH_TLS) {
        (void)create_message_info(MSGID_ENABLE_TWO_FACTORCERTIFICATION_WITH_TLS, NULL, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso);
        return HTTP_BAD_REQUEST;
    }

    return self_return_function(i_paras, ret, o_message_jso, prop_str);
}

LOCAL gint32 set_certificate_revocation_check(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, json_object *obj_json_mode)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    gboolean certificate_revocation_check;
    guint8 state;
    guchar product_auth_enable = 0;
    json_object *message_temp_jso = NULL;
    const gchar *prop_str = "Oem/Huawei/TwoFactorAuthenticationInformation/CertificateRevocationCheckEnabled";

    
    if (NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    // 系统锁定检查
    ret = redfish_system_lockdown_check(SYS_LOCKDOWN_FORBID);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST,
        debug_log(DLOG_INFO, "%s %d: system check failed", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_SYSLOCKDOWN_FORBID, prop_str, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    
    ret = dfl_get_object_handle(BMC_PRODUCT_NAME_APP, &obj_handle);
    ret += dal_get_property_value_byte(obj_handle, PROPERTY_MUTUAL_AUTHENTICATION_ENABLE, &product_auth_enable);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, prop_str, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    if (FALSE == product_auth_enable) {
        (void)create_message_info(MSGID_CERTIFICATE_2FA_NOT_SUPPORTED, NULL, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso);
        return HTTP_NOT_IMPLEMENTED;
    }

    
    ret = dfl_get_object_handle(MUTUAL_AUTH_CLASS_NAME, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, prop_str, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    
    certificate_revocation_check = json_object_get_boolean(obj_json_mode);
    if (certificate_revocation_check) {
        state = RFPROP_MANAGER_STATE_TRUE;
    } else {
        state = RFPROP_MANAGER_STATE_FALSE;
    }

    
    input_list = g_slist_append(input_list, g_variant_new_byte(state));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        MUTUAL_AUTH_CLASS_NAME, MUTUAL_AUTH_METHOD_SET_OCSP, AUTH_ENABLE, i_paras->user_role_privilege, input_list,
        NULL);
    uip_free_gvariant_list(input_list);

    return self_return_function(i_paras, ret, o_message_jso, prop_str);
}


LOCAL gint32 set_CRL_verification_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object *obj_json)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    gboolean crl_enabled;
    guchar product_auth_enable = 0;
    json_object *tmp_message_jso = NULL;
    const gchar *prop_str = "Oem/Huawei/TwoFactorAuthenticationInformation/CrlVerificationEnabled";

    
    return_val_do_info_if_expr(o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__));

    
    ret = redfish_system_lockdown_check(SYS_LOCKDOWN_FORBID);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_BAD_REQUEST,
        debug_log(DLOG_INFO, "%s: system check failed", __FUNCTION__);
        (void)create_message_info(MSGID_SYSLOCKDOWN_FORBID, prop_str, &tmp_message_jso);
        (void)json_object_array_add(*o_message_jso, tmp_message_jso));

    
    (void)dfl_get_object_handle(BMC_PRODUCT_NAME_APP, &obj_handle);
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_MUTUAL_AUTHENTICATION_ENABLE, &product_auth_enable);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, prop_str, &tmp_message_jso);
        (void)json_object_array_add(*o_message_jso, tmp_message_jso));

    return_val_do_info_if_expr(product_auth_enable == 0, HTTP_NOT_IMPLEMENTED,
        (void)create_message_info(MSGID_CERTIFICATE_2FA_NOT_SUPPORTED, NULL, &tmp_message_jso);
        (void)json_object_array_add(*o_message_jso, tmp_message_jso));

    
    ret = dfl_get_object_handle(MUTUAL_AUTH_CLASS_NAME, &obj_handle);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, prop_str, &tmp_message_jso);
        (void)json_object_array_add(*o_message_jso, tmp_message_jso));
    
    crl_enabled = json_object_get_boolean(obj_json);
    if (crl_enabled) {
        input_list = g_slist_append(input_list, g_variant_new_byte(RFPROP_MANAGER_STATE_TRUE));
    } else {
        input_list = g_slist_append(input_list, g_variant_new_byte(RFPROP_MANAGER_STATE_FALSE));
    }

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        MUTUAL_AUTH_CLASS_NAME, MUTUAL_AUTH_METHOD_SET_CRL, AUTH_ENABLE, i_paras->user_role_privilege, input_list,
        NULL);
    uip_free_gvariant_list(input_list);

    return self_return_function(i_paras, ret, o_message_jso, prop_str);
}


LOCAL gint32 set_cert_overdue_warn_time(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, json_object *obj_json_mode)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    guint32 cert_overdue_warn_time;
    json_object *message_temp_jso = NULL;

    // 系统锁定检查
    ret = redfish_system_lockdown_check(SYS_LOCKDOWN_FORBID);
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_BAD_REQUEST,
        debug_log(DLOG_INFO, "%s %d: system check failed", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_SYSLOCKDOWN_FORBID, PROPERTY_OEM_SECURITY_CERT_OVERDUE_WARN_TIME,
        &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    
    return_val_do_info_if_expr(json_type_int != json_object_get_type(obj_json_mode), HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_TYPE_ERR, PROPERTY_OEM_SECURITY_CERT_OVERDUE_WARN_TIME, &message_temp_jso,
        dal_json_object_get_str(obj_json_mode), PROPERTY_ACCOUNT_SERVICE_CERT_OVERDUE_WARN_TIME);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    
    ret = dfl_get_object_handle(CLASS_SECURITY_ENHANCE, &obj_handle);
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, PROPERTY_OEM_SECURITY_CERT_OVERDUE_WARN_TIME, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    
    cert_overdue_warn_time = json_object_get_int(obj_json_mode);

    input_list = g_slist_append(input_list, g_variant_new_uint32(cert_overdue_warn_time));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SECURITY_ENHANCE, METHOD_SECURITY_ENHANCE_SETCERTOVERDUEWARNTIME, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    

    return_val_do_info_if_expr(USER_PARAMETER_OUT_OF_RANGE == ret, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, PROPERTY_OEM_SECURITY_CERT_OVERDUE_WARN_TIME,
        &message_temp_jso, dal_json_object_get_str(obj_json_mode), PROPERTY_ACCOUNT_SERVICE_CERT_OVERDUE_WARN_TIME);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    return self_return_function(i_paras, ret, o_message_jso, PROPERTY_OEM_SECURITY_CERT_OVERDUE_WARN_TIME);
}


TASK_MONITOR_INFO_S *cert_import_info_new(PROVIDER_PARAS_S *i_paras, import_cert_set_userdata_fn set_userdata_fn)
{
    json_object *user_data_jso = NULL;
    TASK_MONITOR_INFO_S *monitor_info;
    monitor_info = task_monitor_info_new((GDestroyNotify)cert_import_info_free_func);
    
    if (monitor_info == NULL) {
        debug_log(DLOG_ERROR, "task_monitor_info_new failed");
        return NULL;
    }
    

    user_data_jso = json_object_new_object();
    return_val_do_info_if_expr(user_data_jso == NULL, NULL, debug_log(DLOG_ERROR, "alloc a new json object failed");
        task_monitor_info_free(monitor_info));

    
    set_userdata_fn(i_paras, user_data_jso);
    monitor_info->user_data = user_data_jso;

    return monitor_info;
}


LOCAL gint32 _get_download_file_path(json_object *body_jso, gchar *file_path, guint32 buffer_len)
{
    const gchar *remote_uri = NULL;
    gchar *file_name = NULL;

    if (file_path == NULL || buffer_len <= 0) {
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__);
        return RET_ERR;
    }

    (void)get_element_str(body_jso, RFACTION_PARAM_CONTENT, &remote_uri);
    return_val_do_info_if_expr(remote_uri == NULL, VOS_ERR, debug_log(DLOG_ERROR, "get download file path fail"));

    file_name = g_strrstr(remote_uri, "/");
    gint32 ret = snprintf_s(file_path, buffer_len, buffer_len - 1, "/tmp/%s", file_name);
    return_val_do_info_if_expr(ret <= 0, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret:%d", __FUNCTION__, ret));

    return VOS_OK;
}


gint32 import_cert_common_status_monitor(const char *origin_obj_path, json_object *body_jso,
    process_local_cert_import_fn local_import_fn, TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
#define DOWNLOAD_FILE_PATH_LEN 512
#define TASK_PROGRESS_10 10
    gint32 ret;
    gint32 file_down_progress = 0;
    gchar download_file_path[DOWNLOAD_FILE_PATH_LEN] = {0};

    return_val_do_info_if_expr((origin_obj_path == NULL) || (body_jso == NULL) || (local_import_fn == NULL) ||
        (monitor_fn_data == NULL) || (message_obj == NULL),
        RF_FAILED, debug_log(DLOG_ERROR, "input param error");
        json_object_clear_string(body_jso, RFACTION_PARAM_CONTENT));

    monitor_fn_data->task_state = RF_TASK_RUNNING;

    ret = get_file_transfer_progress(&file_down_progress);
    if (file_down_progress < 0) {
        debug_log(DLOG_ERROR, "file_down_progress = %d", file_down_progress);
        monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        monitor_fn_data->task_progress = 0;
        
        (void)parse_file_transfer_err_code(file_down_progress, message_obj);
        goto exit;
        
    }

    debug_log(DLOG_DEBUG, "----------file down progress :%d---------", file_down_progress);

    
    if (file_down_progress != RF_FINISH_PERCENTAGE) {
        monitor_fn_data->task_progress = (guchar)(file_down_progress * 0.1);
        return RF_OK;
    } else {
        
        ret = _get_download_file_path(body_jso, download_file_path, sizeof(download_file_path));
        goto_label_do_info_if_expr(ret != VOS_OK, exit, monitor_fn_data->task_state = RF_TASK_EXCEPTION;
            monitor_fn_data->task_progress = TASK_PROGRESS_10;
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj));

        ret = local_import_fn(download_file_path, (json_object *)monitor_fn_data->user_data, message_obj);
        if (ret == HTTP_OK) {
            monitor_fn_data->task_state = RF_TASK_COMPLETED;
            monitor_fn_data->task_progress = 100;
        } else {
            monitor_fn_data->task_state = RF_TASK_EXCEPTION;
            monitor_fn_data->task_progress = TASK_PROGRESS_10;
            debug_log(DLOG_ERROR, "import cert or crl from /tmp dictionary to server failed. ret = %d", ret);
        }
    }

exit:
    
    json_object_clear_string(body_jso, RFACTION_PARAM_CONTENT);
    return RF_OK;
}


LOCAL gint32 root_cert_import_status_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    gint32 ret = import_cert_common_status_monitor(origin_obj_path, body_jso, 
        process_import_local_root_cert, monitor_fn_data, message_obj);
    
    if (ret == RF_OK && (monitor_fn_data->task_state == RF_TASK_COMPLETED ||
        monitor_fn_data->task_state == RF_TASK_EXCEPTION || monitor_fn_data->task_state == RF_TASK_KILLED)) {
        json_object_clear_string(body_jso, RFACTION_PARAM_CONTENT);
    }
    return ret;
}


gint32 is_import_cert_or_crl_task_exist(task_status_monitor_fn monitor_fn, PROVIDER_PARAS_S *i_paras,
    IMPORT_CERT_FILE_TYPE file_type, json_object **o_message_jso)
{
    gboolean b_ret;
    const gchar *msg_id = NULL;

    
    b_ret = check_redfish_running_task_exist(monitor_fn, i_paras->val_jso, i_paras->uri, NULL);
    if (b_ret) {
        switch (file_type) {
            case FILE_TYPE_ROOT_CERT:
            case FILE_TYPE_CLIENT_CERT:
                msg_id = MSGID_CERT_IMPORTING;
                debug_log(DLOG_ERROR, "%s failed:cert import task already exist", __FUNCTION__);
                break;

            default:
                msg_id = MSGID_CRL_IMPORTING;
                debug_log(DLOG_ERROR, "%s failed: crl import task already exist", __FUNCTION__);
                break;
        }
        (void)create_message_info(msg_id, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }
    

    return VOS_OK;
}


gint32 check_cert_import_by_text(const gchar *content_text, IMPORT_CERT_PARAMS_S *params, json_object **o_message_jso)
{
    gsize file_len = 0;
    IMPORT_CERT_FILE_TYPE file_type = params->file_type;
    _cleanup_gfree_ guchar *cert_content = NULL;

    
    gsize max_file_len = (file_type == FILE_TYPE_ROOT_CERT || file_type == FILE_TYPE_CLIENT_CERT) ?
        MAX_SERVER_CERT_FILE_LEN : MAX_CRL_FILE_LEN;

    if (file_type == FILE_TYPE_CLIENT_CERT) {
        cert_content = g_base64_decode(content_text, &file_len);
        if (cert_content == NULL || file_len == 0) {
            debug_log(DLOG_ERROR, "%s: decode client certificate failed.", __FUNCTION__);
            return HTTP_INTERNAL_SERVER_ERROR;
        }

        content_text = (const gchar *)cert_content;
    } else {
        file_len = strlen(content_text);
    }

    if (file_len > max_file_len) {
        debug_log(DLOG_ERROR, "%s: file size exceed, size is :%zu.", __FUNCTION__, file_len);
        return IMPORT_CERT_FILE_LEN_EXCEED_ERR;
    }

    
    gboolean b_ret = g_file_set_contents(params->tmp_file_path, content_text, file_len, NULL);
    if (b_ret == FALSE) {
        (void)create_message_info(MSGID_INTERNAL_ERR, PROPERTY_ACCOUNT_SERVICE_IMPORT_ROOT_CERT, o_message_jso);
        debug_log(DLOG_ERROR, "%s: save certificate file failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    (void)chmod(params->tmp_file_path, (S_IRUSR | S_IWUSR));
    (void)chown(params->tmp_file_path, REDFISH_USER_UID, APPS_USER_GID);

    return RET_OK;
}


gint32 rf_start_file_download(PROVIDER_PARAS_S *i_paras, const gchar *remote_uri, gint32 transferfileinfo_code,
    json_object **o_message_jso)
{
    gint32 ret;
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle;
    gchar uri_out[ARRAY_LENTH * 8] = {0};

    
    ret = dfl_get_object_handle(OBJ_NAME_TRANSFERFILE, &obj_handle);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_BAD_REQUEST,
        (void)parse_file_transfer_err_code(UNKNOWN_ERROR, o_message_jso));

    
    ret =
        snprintf_s(uri_out, sizeof(uri_out), sizeof(uri_out) - 1, "download;%d;%s", transferfileinfo_code, remote_uri);
    return_val_do_info_if_expr(ret <= 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : snprintf_s fail, ret:%d", __FUNCTION__, ret);
        create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    input_list = g_slist_append(input_list, g_variant_new_string((const gchar*)uri_out));
    (void)memset_s(uri_out, sizeof(uri_out), 0, sizeof(uri_out));
    input_list = g_slist_append(input_list, g_variant_new_string(i_paras->session_id));
    ret = uip_redfish_call_class_method_with_userinfo(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        (i_paras->auth_type == LOG_TYPE_LOCAL), (const gchar *)i_paras->user_roleid, obj_handle,
        CLASS_NAME_TRANSFERFILE, METHOD_INITRIAL_FILE_TRANSFER, AUTH_DISABLE, i_paras->user_role_privilege, input_list,
        NULL);
    uip_free_gvariant_list(input_list);

    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "METHOD_INITRIAL_FILE_TRANSFER ret = %d", ret);
        (void)parse_file_transfer_err_code(ret, o_message_jso);
        return HTTP_BAD_REQUEST;
    }
    return RET_OK;
}


LOCAL gint32 process_import_remote_root_cert(PROVIDER_PARAS_S *i_paras, const gchar *uri, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    TASK_MONITOR_INFO_S *cert_import_data = NULL;
    json_object *body_jso = NULL;

    ret = rf_start_file_download(i_paras, uri, PROPERTY_ACCOUNT_SERVICE_CERTIFICATE_URI_ROOT_CODE, o_message_jso);
    return_val_do_info_if_expr(ret != VOS_OK, ret, debug_log(DLOG_ERROR, "%s: start file download fail", __FUNCTION__));

    cert_import_data = cert_import_info_new(i_paras, import_cert_set_common_userdata_fn);
    goto_label_do_info_if_expr(cert_import_data == NULL, err_exit,
        debug_log(DLOG_ERROR, "%s: alloc a new cert_import info failed", __FUNCTION__));

    cert_import_data->task_progress = TASK_NO_PROGRESS; 
    cert_import_data->rsc_privilege = USERROLE_USERMGNT;

    body_jso = i_paras->val_jso;
    ret = create_new_task(PROPERTY_ACCOUNT_SERVICE_CERTIFICATE_URI_IMPORT, root_cert_import_status_monitor,
        cert_import_data, body_jso, i_paras->uri, o_result_jso);
    goto_label_do_info_if_expr(ret != RF_OK, err_exit, task_monitor_info_free(cert_import_data);
        debug_log(DLOG_ERROR, "%s: create new task failed", __FUNCTION__));

    return HTTP_ACCEPTED;

err_exit:
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL gint32 parse_import_root_cert_ret_code(gint32 ret, json_object **o_message_jso, const gchar *property_name)
{
    gint32 http_code;

    switch (ret) {
        
        case VOS_OK:
        case VOS_ERR_MA_IDEL:
            http_code = HTTP_OK;
            (void)create_message_info(MSGID_CERTIFICATE_UPLOAD_SUCCESS, property_name, o_message_jso);
            break;

        
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, property_name, o_message_jso);
            http_code = RFERR_INSUFFICIENT_PRIVILEGE;
            break;

        
        case VOS_ERR_MA_REACH_MAX:
            http_code = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_CERTIFICATE_FULL, property_name, o_message_jso);
            break;

        
        case VOS_ERR_MA_CERT_EXSIT:
            http_code = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_CERTIFICATE_EXISTS, property_name, o_message_jso);
            break;

        
        case VOS_ERR_MA_FORMAT_ERR:
            http_code = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_CERTIFICATE_FORMAT_ERR, property_name, o_message_jso);
            break;
        
        case VOS_ERR_MA_KEY_USAGE_CERT_SIGN_ERR:
        
        case VOS_ERR_MA_BASIC_CONSTRAINTS_ERR:
            http_code = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_CERTIFICATE_CONSTRAINTS_ERR, property_name, o_message_jso);
            break;

        
        default:
            debug_log(DLOG_ERROR, "unknown cert import error:%d", ret);
            http_code = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_CERT_IMPORT_FAILED, property_name, o_message_jso);
            break;
    }

    return http_code;
}


LOCAL gint32 process_import_local_root_cert(const gchar *file_path, json_object *user_data, json_object **o_message_jso)
{
    gint32 ret;
    GSList *input_list = NULL;
    GSList *obj_cert_list = NULL;
    GSList *obj_cert_node = NULL;
    guchar cert_id = 0;
    gchar cert_serial_num[ARRAY_LENTH] = {0};
    OBJ_HANDLE obj_handle = 0;
    const gchar *user_name = NULL;
    const gchar *client = NULL;
    gint32 user_priv = 0;
    gint32 from_webui_flag = REDFISH_REQ_FROM_OTHER_CLIENT;

    
    goto_label_do_info_if_expr(strlen(file_path) >= MAX_FILEPATH_LENGTH, exit, ret = IMPORT_CERT_INVALID_FILEPATH_ERR;
        debug_log(DLOG_ERROR, "%s: file path length is over %d", __FUNCTION__, MAX_FILEPATH_LENGTH));

    
    ret = dfl_get_object_list(MUTUAL_AUTH_ROOT_CLASS_NAME, &obj_cert_list);
    goto_label_do_info_if_expr(ret != VOS_OK, exit, ret = HTTP_INTERNAL_SERVER_ERROR;
        (void)create_message_info(MSGID_INTERNAL_ERR, PROPERTY_ACCOUNT_SERVICE_IMPORT_ROOT_CERT, o_message_jso);
        debug_log(DLOG_ERROR, "%s, %d: get object list fail.\n", __FUNCTION__, __LINE__));

    for (obj_cert_node = obj_cert_list; obj_cert_node; obj_cert_node = obj_cert_node->next) {
        (void)memset_s(cert_serial_num, sizeof(cert_serial_num), 0, sizeof(cert_serial_num));
        ret = dal_get_property_value_string((OBJ_HANDLE)obj_cert_node->data, MUTUAL_AUTH_ROOT_CERT_SERIALNUMBER,
            cert_serial_num, sizeof(cert_serial_num));
        if (ret != VOS_OK || g_strcmp0(cert_serial_num, "") != VOS_OK) {
            continue;
        }

        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_cert_node->data, MUTUAL_AUTH_ROOT_CERT_ID, &cert_id);
        obj_handle = (OBJ_HANDLE)obj_cert_node->data;
        debug_log(DLOG_DEBUG, "cert id is: %d .", cert_id);
        break;
    }

    g_slist_free(obj_cert_list);
    
    goto_label_do_info_if_expr(cert_id == 0, exit, ret = HTTP_BAD_REQUEST; debug_log(DLOG_ERROR, "cert id full.");
        (void)create_message_info(MSGID_CERTIFICATE_FULL, PROPERTY_ACCOUNT_SERVICE_IMPORT_ROOT_CERT, o_message_jso));

    
    import_cert_get_common_userdata_fn(&user_name, &client, &user_priv, &from_webui_flag, user_data);

    
    (void)dal_set_file_owner(file_path, REDFISH_USER_UID, APPS_USER_GID);
    (void)dal_set_file_mode(file_path, (S_IRUSR | S_IWUSR));

    
    input_list = g_slist_append(input_list, g_variant_new_byte(cert_id));
    input_list = g_slist_append(input_list, g_variant_new_string(file_path));
    
    ret = uip_call_class_method_redfish((guchar)from_webui_flag, user_name, client, obj_handle,
        MUTUAL_AUTH_ROOT_CLASS_NAME, MUTUAL_AUTH_ROOT_CERT_METHOD_IMPORT, AUTH_ENABLE, user_priv, input_list, NULL);
    
    uip_free_gvariant_list(input_list);
    ret = parse_import_root_cert_ret_code(ret, o_message_jso, PROPERTY_ACCOUNT_SERVICE_IMPORT_ROOT_CERT);

exit:
    do_if_expr(ret != HTTP_OK, vos_rm_filepath(file_path));
    return ret;
}


gint32 process_cert_or_crl_import(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso,
    IMPORT_CERT_PARAMS_S *import_cert_params)
{
    const gchar *import_type = NULL;
    const gchar *content_str = NULL;
    gint32 ret;
    gboolean is_local_import = FALSE;
    json_object *user_data = NULL;
    IMPORT_CERT_FILE_TYPE file_type;

    return_val_do_info_if_expr(import_cert_params->local_cert_import_fn == NULL ||
        import_cert_params->remote_cert_import_fn == NULL || import_cert_params->set_userdata_fn == NULL ||
        import_cert_params->tmp_file_path == NULL,
        VOS_ERR, debug_log(DLOG_ERROR, "%s: input params invalid", __FUNCTION__));

    
    (void)get_element_str(i_paras->val_jso, RFACTION_PARAM_CONTENT, &content_str);
    return_val_do_info_if_expr(content_str == NULL, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, RFACTION_PARAM_CONTENT, o_message_jso);
        debug_log(DLOG_ERROR, "failed to get %s from request.", RFACTION_PARAM_CERTICATE));
    
    (void)get_element_str(i_paras->val_jso, RFACTION_PARAM_TYPE, &import_type);
    return_val_do_info_if_expr(import_type == NULL, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, RFACTION_PARAM_TYPE, o_message_jso);
        debug_log(DLOG_ERROR, "failed to get %s from request.", RFACTION_PARAM_TYPE));

    file_type = import_cert_params->file_type;
    
    if (g_strcmp0(import_type, PROPERTY_ACCOUNT_SERVICE_CERTIFICATE_URI) == VOS_OK) {
        
        ret = check_cert_import_by_uri(content_str, file_type, &is_local_import);
        return_val_if_expr(ret != VOS_OK, ret);

        
        if (is_local_import == FALSE) {
            return import_cert_params->remote_cert_import_fn(i_paras, content_str, o_message_jso, o_result_jso);
        }
    } else if (g_strcmp0(import_type, PROPERTY_ACCOUNT_SERVICE_CERTIFICATE_TEXT) == VOS_OK) {
        ret = check_cert_import_by_text(content_str, import_cert_params, o_message_jso);
        return_val_if_expr(ret != VOS_OK, ret);
    }

    user_data = json_object_new_object();
    return_val_do_info_if_expr(user_data == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: new user_data fail", __FUNCTION__);
        create_message_info(MSGID_INTERNAL_ERR, PROPERTY_ACCOUNT_SERVICE_IMPORT_ROOT_CERT, o_message_jso));

    import_cert_params->set_userdata_fn(i_paras, user_data);
    if (is_local_import) {
        
        if (!dal_check_file_opt_user(i_paras->auth_type, (const gchar *)i_paras->user_name, content_str,
            (const gchar *)i_paras->user_roleid)) {
            (void)create_message_info(MSGID_NO_PRIV_OPT_FILE, PROPERTY_ACCOUNT_SERVICE_IMPORT_ROOT_CERT, o_message_jso);
            json_object_put(user_data);
            return HTTP_BAD_REQUEST;
        }

        ret = import_cert_params->local_cert_import_fn(content_str, user_data, o_message_jso);
    } else {
        
        ret = import_cert_params->local_cert_import_fn(import_cert_params->tmp_file_path, user_data, o_message_jso);
    }

    json_object_put(user_data);
    return ret;
}


void set_import_cert_params(IMPORT_CERT_PARAMS_S *param, IMPORT_CERT_FILE_TYPE file_type,
    import_cert_set_userdata_fn set_userdata_fn, process_local_cert_import_fn local_import_fn,
    process_remote_cert_import_fn remote_ipmort_fn, gchar *tmp_file_path)
{
    param->file_type = file_type;
    param->set_userdata_fn = set_userdata_fn;
    param->local_cert_import_fn = local_import_fn;
    param->remote_cert_import_fn = remote_ipmort_fn;
    param->tmp_file_path = tmp_file_path;
    return;
}

LOCAL gint32 act_import_root_certificate(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret = 0;
    IMPORT_CERT_PARAMS_S params;

    
    return_val_do_info_if_expr((i_paras == NULL) || (o_message_jso == NULL) || (o_result_jso == NULL),
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    if ((i_paras->user_role_privilege & USERROLE_USERMGNT) == 0) {
        ret = HTTP_FORBIDDEN;
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        goto exit;
    }

    
    ret = is_support_mutual_auth(o_message_jso, PROPERTY_ACCOUNT_SERVICE_IMPORT_ROOT_CERT);
    do_if_expr(ret != VOS_OK, goto exit);

    
    ret =
        is_import_cert_or_crl_task_exist(root_cert_import_status_monitor, i_paras, FILE_TYPE_ROOT_CERT, o_message_jso);
    do_if_expr(ret != VOS_OK, goto exit);

    
    set_import_cert_params(&params, FILE_TYPE_ROOT_CERT, import_cert_set_common_userdata_fn,
        process_import_local_root_cert, process_import_remote_root_cert, PROPERTY_ACCOUNT_SERVICE_ROOT_CERT_PATH);
    ret = process_cert_or_crl_import(i_paras, o_message_jso, o_result_jso, &params);
    
    if (ret == HTTP_OK || ret == HTTP_ACCEPTED || *o_message_jso != NULL) {
        goto exit;
    }

    
    
    switch (ret) {
        case IMPORT_CERT_URI_DISMATCH_ERR:
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFACTION_PARAM_CONTENT, o_message_jso, RF_SENSITIVE_INFO,
                RFACTION_PARAM_CONTENT);
            debug_log(DLOG_ERROR, "%s: uri of import file is illegal", __FUNCTION__);
            ret = HTTP_BAD_REQUEST;
            break;

        case IMPORT_CERT_FILE_LEN_EXCEED_ERR:
        case IMPORT_CERT_INVALID_FILEPATH_ERR:
            (void)create_message_info(MSGID_CERT_IMPORT_FAILED, PROPERTY_ACCOUNT_SERVICE_IMPORT_ROOT_CERT,
                o_message_jso);
            ret = HTTP_BAD_REQUEST;
            break;

        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, PROPERTY_ACCOUNT_SERVICE_IMPORT_ROOT_CERT, o_message_jso);
            debug_log(DLOG_ERROR, "%s: import root cert fail, ret:%d", __FUNCTION__, ret);
            ret = HTTP_INTERNAL_SERVER_ERROR;
    }

exit:
    return ret;
}

LOCAL gint32 act_import_root_certificate_entry(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret = act_import_root_certificate(i_paras, o_message_jso, o_result_jso);
    if (ret != HTTP_ACCEPTED && i_paras != NULL) {
        delete_user_specific_file(i_paras, i_paras->val_jso, RFACTION_PARAM_CONTENT);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
    }
    return ret;
}


LOCAL gint32 act_delete_root_certificate(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gboolean bret;
    gint32 cert_id;
    json_object *val_json = NULL;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    guchar product_auth_enable = 0;
    gchar cert_serial_num[ARRAY_LENTH] = {0};

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    ret = dfl_get_object_handle(BMC_PRODUCT_NAME_APP, &obj_handle);
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_MUTUAL_AUTHENTICATION_ENABLE, &product_auth_enable);
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    return_val_do_info_if_expr(product_auth_enable == FALSE, HTTP_NOT_IMPLEMENTED,
        create_message_info(MSGID_CERTIFICATE_2FA_NOT_SUPPORTED, NULL, o_message_jso));

    
    bret = json_object_object_get_ex(i_paras->val_jso, PROPERTY_ACCOUNT_SERVICE_CERT_ID, &val_json);
    return_val_do_info_if_fail(TRUE == bret, HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "%s %d: json_object_object_get_ex is fail,ret = %d", __FUNCTION__, __LINE__, ret));

    
    return_val_do_info_if_fail(json_type_int == json_object_get_type(val_json), HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, PROPERTY_ACCOUNT_SERVICE_CERT_ID, o_message_jso,
        dal_json_object_get_str(val_json), PROPERTY_ACCOUNT_SERVICE_CERT_ID));

    
    cert_id = json_object_get_int(val_json);
    return_val_do_info_if_fail(0 <= cert_id && cert_id <= G_MAXUINT8, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, PROPERTY_ACCOUNT_SERVICE_CERT_ID, o_message_jso,
        dal_json_object_get_str(val_json), PROPERTY_ACCOUNT_SERVICE_CERT_ID));
    

    debug_log(DLOG_DEBUG, "%s %d: The certificate will be deleted, CertId = %d", __FUNCTION__, __LINE__, cert_id);

    
    ret = dal_get_specific_object_byte(MUTUAL_AUTH_ROOT_CLASS_NAME, MUTUAL_AUTH_ROOT_CERT_ID, (guint8)cert_id,
        &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: Get obj handle failed.", __FUNCTION__, __LINE__));
    
    (void)dal_get_property_value_string(obj_handle, MUTUAL_AUTH_ROOT_CERT_SERIALNUMBER, cert_serial_num,
        sizeof(cert_serial_num));
    return_val_do_info_if_fail(VOS_OK != g_strcmp0(cert_serial_num, ""), HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_CERTIFICATE_NOT_EXIST, PROPERTY_ACCOUNT_SERVICE_DELETE_ROOT_CERT,
        o_message_jso));

    
    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)cert_id));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        MUTUAL_AUTH_ROOT_CLASS_NAME, MUTUAL_AUTH_ROOT_CERT_METHOD_DELETE, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, &output_list);
    uip_free_gvariant_list(input_list);
    // vos_ok代表参数检查正确 具体是否成功还需要判断outputlist
    if (ret == VOS_OK && g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0)) != VOS_OK) {
        ret = MUTUAL_AUTH_STATE_ERR; // 当前只有开启双因素会导致失败
    }
    uip_free_gvariant_list(output_list);
    switch (ret) {
            
        case VOS_OK:
            ret = HTTP_OK;
            (void)create_message_info(MSGID_CERTIFICATE_DELETE_SUCCESS, NULL, o_message_jso,
                dal_json_object_get_str(val_json));
            break;

            
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, PROPERTY_ACCOUNT_SERVICE_DELETE_ROOT_CERT,
                o_message_jso);
            ret = RFERR_INSUFFICIENT_PRIVILEGE;
            break;
        case MUTUAL_AUTH_STATE_ERR:
            (void)create_message_info(MSGID_MUTUAL_AUTH_ROOT_CERT_IN_USE, PROPERTY_ACCOUNT_SERVICE_DELETE_ROOT_CERT,
                o_message_jso);
            ret = HTTP_BAD_REQUEST;
            break;
            
        default:
            ret = HTTP_INTERNAL_SERVER_ERROR;
            (void)create_message_info(MSGID_INTERNAL_ERR, PROPERTY_ACCOUNT_SERVICE_DELETE_ROOT_CERT, o_message_jso);
            debug_log(DLOG_ERROR, "unknown cert import error:%d", ret);
            break;
    }

    return ret;
}


LOCAL PROPERTY_PROVIDER_S *get_self_provider_account_service(PROVIDER_PARAS_S *i_paras)
{
    return_val_if_fail((NULL != i_paras) && (i_paras->index >= 0) &&
        (i_paras->index < (gint32)(sizeof(accountservice_provider) / sizeof(PROPERTY_PROVIDER_S))),
        (PROPERTY_PROVIDER_S *)NULL);
    return &accountservice_provider[i_paras->index];
}


gint32 accountservice_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    
    return_val_if_fail(NULL != i_paras && NULL != prop_provider && NULL != count, HTTP_INTERNAL_SERVER_ERROR);

    *prop_provider = accountservice_provider;
    *count = sizeof(accountservice_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}

LOCAL PROPERTY_PROVIDER_S accountservice_provider_import[] = {
};


gint32 accountservice_provider_import_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    
    return_val_if_fail(NULL != i_paras && NULL != prop_provider && NULL != count, HTTP_INTERNAL_SERVER_ERROR);

    *prop_provider = accountservice_provider_import;
    
    *count = 0;
    
    return VOS_OK;
}

LOCAL PROPERTY_PROVIDER_S g_accountservice_provider_delete[] = {};


gint32 accountservice_provider_delete_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    
    return_val_if_fail(NULL != i_paras && NULL != prop_provider && NULL != count, HTTP_INTERNAL_SERVER_ERROR);

    *prop_provider = g_accountservice_provider_delete;
    
    *count = 0;
    
    return VOS_OK;
}


LOCAL void _add_list_data_to_json_array(gpointer data, gpointer user_data)
{
    gchar *uri = (gchar *)data;
    struct url_user_data *url_data = (struct url_user_data *)user_data;
    gint32 ret;
    json_object *item_jso = NULL;

    return_do_info_if_fail((NULL != data) && (NULL != user_data),
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    
    if (NULL != g_strrstr(uri, ACTION_SEGMENT)) {
        ret = rf_validate_action_uri(uri, url_data->hash_table);

        return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_DEBUG, "action uri %s is not valid", uri));
    }

    item_jso = json_object_new_string(uri);
    ret = json_object_array_add(url_data->target_jso, item_jso);
    do_val_if_fail(0 == ret, debug_log(DLOG_ERROR, "add item to json array failed, ret is %d", ret);
        (void)json_object_put(item_jso));

    return;
}


LOCAL void _replace_dynamic_rsc_id(json_object *jso, GHashTable *hash_table)
{
    json_type t;
    json_object *object_iter = NULL;
    json_object *new_target_jso = NULL;
    const gchar *target_uri = NULL;
    GSList *uri_list = NULL;
    gint32 ret = VOS_OK;
    json_object *target_jso = NULL;

    t = json_object_get_type(jso);
    return_if_fail(json_type_object == t);

#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#endif
    json_object_object_foreach0(jso, name, val)
    {
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif
        t = json_object_get_type(val);

        
        if (0 != g_strcmp0(name, RFPROP_TARGETS)) {
            if ((json_type_object != t) && (json_type_array != t)) {
                continue;
            } else if (json_type_object == t) {
                _replace_dynamic_rsc_id(val, hash_table);
            } else {
                json_object_array_foreach(val, object_iter)
                {
                    _replace_dynamic_rsc_id(object_iter, hash_table);

                    
                    target_jso = NULL;
                    (void)json_object_object_get_ex(object_iter, RFPROP_TARGETS, &target_jso);

                    if ((json_type_array == json_object_get_type(target_jso)) &&
                        (0 == json_object_array_length(target_jso))) {
                        json_object_array_delete_idx(val, object_iter_i);

                        
                        object_iter_i--;
                    }
                }
            }
        } else {
            return_do_info_if_fail(json_type_array == t,
                debug_log(DLOG_ERROR, "Target property is not of array type, please check"));

            json_object_array_foreach(val, object_iter)
            {
                target_uri = NULL;
                uri_list = NULL;

                target_uri = dal_json_object_get_str(object_iter);

                ret = fill_rsc_id_for_uri_pattern(target_uri, &uri_list, hash_table);
                continue_if_expr(VOS_OK != ret);

                
                do_val_if_fail(NULL != new_target_jso, (new_target_jso = json_object_new_array()));

                struct url_user_data user_data;
                user_data.target_jso = new_target_jso;
                user_data.hash_table = hash_table;
                g_slist_foreach(uri_list, (GFunc)_add_list_data_to_json_array, &user_data);
                g_slist_free_full(uri_list, (GDestroyNotify)g_free);
            }

            
            do_val_if_expr(NULL != new_target_jso, json_object_object_add(jso, RFPROP_TARGETS, new_target_jso));
        }
    }

    return;
}


gint32 get_privilege_map_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    if (i_paras == NULL || o_rsc_jso == NULL || o_err_array_jso == NULL) {
        debug_log(DLOG_DEBUG, "%s failed:input param error", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (0 == (USERROLE_USERMGNT & i_paras->user_role_privilege)) {
        debug_log(DLOG_ERROR, "%s failed:insufficient privilege", __FUNCTION__);

        return RF_INSUFFICIENT_PRIV;
    }

    // 生成临时hashtable，使用完之后，释放内存。
    // 这样可以保证此接口在并发场景下，正常使用。
    GHashTable *hash_table = url_hash_table_init();
    if (NULL == hash_table) {
        return RF_CREATE_FAILED;
    }
    _replace_dynamic_rsc_id(o_rsc_jso, hash_table);
    url_hash_table_destroy(hash_table);

    return RF_OK;
}


LOCAL gint32 set_system_lockdown_state(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, json_object *obj_json_mode)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    guint8 system_lockdown_enabled;
    json_object *message_temp_jso = NULL;
    guint8 system_lockdown_support = 0;
    const gchar *prop_str = "Oem/Huawei/SystemLockDownEnabled";

    
    if (NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROPERTY_PME_SERVICECONFIG_SYSTEM_LOCKDOWN_SUPPORT,
        &system_lockdown_support);
    if (system_lockdown_support == PME_SERVICE_UNSUPPORT) {
        debug_log(DLOG_INFO, "%s system lockdown unsupport.", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_UNKNOWN, prop_str, &message_temp_jso, prop_str);
        (void)json_object_array_add(*o_message_jso, message_temp_jso);
        return HTTP_BAD_REQUEST;
    }

    
    ret = dfl_get_object_handle(CLASS_SECURITY_ENHANCE, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s get security handle failed ret %d", __FUNCTION__, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, prop_str, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    system_lockdown_enabled = json_object_get_boolean(obj_json_mode) ? 1 : 0;
    input_list = g_slist_append(input_list, g_variant_new_byte(system_lockdown_enabled));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SECURITY_ENHANCE, METHOD_SECURITY_ENHANCE_SET_SYSTEM_LOCKDOWN, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, NULL);
    uip_free_gvariant_list(input_list);

    return self_return_function(i_paras, ret, o_message_jso, prop_str);
}
