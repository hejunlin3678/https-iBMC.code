

#include "redfish_provider.h"
#include "../apps/bmc_global/inc/bmc.h"
#include "redfish_http.h"

#define LDAP_CONTROLLER_ID "ControllerId"
#define LDAP_OBJ_HANDLE "ObjHandle"


void parse_import_crl_common_ret_code(gint32 ret_code, json_object **o_message_jso, gint32 *http_code)
{
    switch (ret_code) {
        case VOS_OK:
            (void)create_message_info(MSGID_SUCCESS, NULL, o_message_jso);
            *http_code = HTTP_OK;
            break;

        
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
            *http_code = HTTP_FORBIDDEN;
            break;

        
        case IMPORT_CRL_ERR_FORMAT_ERR:
            (void)create_message_info(MSGID_CRL_FILE_FORMAT_ERROR, NULL, o_message_jso);
            *http_code = HTTP_BAD_REQUEST;
            break;

        
        case IMPORT_CRL_ERR_ISSUER_VERIFY_FAIL:
            (void)create_message_info(MSGID_ROOT_CERT_MISMATCH, NULL, o_message_jso);
            *http_code = HTTP_BAD_REQUEST;
            break;

        
        case IMPORT_CRL_ERR_ROOT_CERT_NOT_IMPORT:
            (void)create_message_info(MSGID_ROOT_CERT_NOT_IMPORT, NULL, o_message_jso);
            *http_code = HTTP_BAD_REQUEST;
            break;

        
        case IMPORT_CRL_ERR_ISSUING_DATE_INVALID:
            (void)create_message_info(MSGID_CRL_ISSUING_DATE_INVALID, NULL, o_message_jso);
            *http_code = HTTP_BAD_REQUEST;
            break;
        case IMPORT_CRL_ERR_NOT_SUPPORT_CRL_SIGN:
            (void)create_message_info(MSGID_CERT_NOT_SUPPORT_CRL_SIGN, NULL, o_message_jso);
            *http_code = HTTP_BAD_REQUEST;
            break;

        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            *http_code = HTTP_INTERNAL_SERVER_ERROR;
    }
    return;
}


LOCAL gint32 process_import_local_mutual_auth_crl(const gchar *file_path, json_object *user_data,
    json_object **o_message_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    const gchar *user_name = NULL;
    const gchar *client = NULL;
    gint32 user_priv = 0;
    gint32 from_webui_flag = REDFISH_REQ_FROM_OTHER_CLIENT;
    GSList *input_list = NULL;
    gint32 root_cert_id = 0;
    gint32 http_code = HTTP_OK;

    
    goto_label_do_info_if_expr(strlen(file_path) >= MAX_FILEPATH_LENGTH, exit, ret = IMPORT_CERT_INVALID_FILEPATH_ERR;
        debug_log(DLOG_ERROR, "%s: file path length is over %d", __FUNCTION__, MAX_FILEPATH_LENGTH));

    
    goto_label_do_info_if_expr(vos_get_file_length(file_path) > MAX_CRL_FILE_LEN, exit, ret = HTTP_BAD_REQUEST;
        (void)create_message_info(MSGID_CRL_IMPORT_FAILED, NULL, o_message_jso));

    
    ret = check_root_cert_id(user_data, o_message_jso, MUTUAL_AUTH_ROOT_CLASS_NAME, &obj_handle);
    goto_label_do_info_if_expr(ret != VOS_OK, exit,
        debug_log(DLOG_ERROR, "%s: get root cert obj_handle fail, ret:%d", __FUNCTION__, ret));

    
    import_cert_get_common_userdata_fn(&user_name, &client, &user_priv, &from_webui_flag, user_data);
    (void)get_element_int(user_data, RFACTION_PARAM_ROOT_CERT_ID, &root_cert_id);

    
    (void)dal_set_file_owner(file_path, REDFISH_USER_UID, REDFISH_USER_GID);
    (void)dal_set_file_mode(file_path, (S_IRUSR | S_IWUSR));

    input_list = g_slist_append(input_list, g_variant_new_byte((guchar)root_cert_id));
    input_list = g_slist_append(input_list, g_variant_new_string(file_path));

    ret = uip_call_class_method_redfish((guchar)from_webui_flag, user_name, client, obj_handle,
        MUTUAL_AUTH_ROOT_CLASS_NAME, MUTUAL_AUTH_ROOT_CERT_METHOD_IMPORT_CRL, AUTH_ENABLE, user_priv, input_list, NULL);
    uip_free_gvariant_list(input_list);

    do_if_expr(ret != VOS_OK, debug_log(DLOG_ERROR, "%s: call remote method(%s) fail, ret :%d", __FUNCTION__,
        MUTUAL_AUTH_ROOT_CERT_METHOD_IMPORT_CRL, ret));

    parse_import_crl_common_ret_code(ret, o_message_jso, &http_code);
    ret = http_code;

exit:
    do_if_expr(ret != HTTP_OK, vos_rm_filepath(file_path));
    return ret;
}


LOCAL void import_mutual_auth_crl_set_userdata(PROVIDER_PARAS_S *i_paras, json_object *user_data)
{
    gint32 root_cert_id = 0;

    import_cert_set_common_userdata_fn(i_paras, user_data);
    (void)get_element_int(i_paras->val_jso, RFACTION_PARAM_ROOT_CERT_ID, &root_cert_id);
    (void)json_object_object_add(user_data, RFACTION_PARAM_ROOT_CERT_ID, json_object_new_int(root_cert_id));
    return;
}


LOCAL gint32 mutual_auth_crl_import_status_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    gint32 ret;
    ret = import_cert_common_status_monitor(origin_obj_path, body_jso, process_import_local_mutual_auth_crl,
        monitor_fn_data, message_obj);
    return ret;
}


LOCAL gint32 process_import_remote_mutual_auth_crl(PROVIDER_PARAS_S *i_paras, const gchar *uri,
    json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    TASK_MONITOR_INFO_S *import_crl_data = NULL;
    json_object *body_jso = NULL;

    ret = rf_start_file_download(i_paras, uri, IMPORT_MUTUAL_AUTH_CRL_TRANSFER_FILEINFO_CODE, o_message_jso);
    return_val_do_info_if_expr(ret != VOS_OK, ret,
        debug_log(DLOG_ERROR, "%s: start crl file download fail", __FUNCTION__));

    import_crl_data = cert_import_info_new(i_paras, import_mutual_auth_crl_set_userdata);
    goto_label_do_info_if_expr(import_crl_data == NULL, err_exit,
        debug_log(DLOG_ERROR, "%s: alloc a new cert_import info failed", __FUNCTION__));

    import_crl_data->task_progress = TASK_NO_PROGRESS; 

    body_jso = i_paras->val_jso;
    import_crl_data->rsc_privilege = USERROLE_USERMGNT;
    ret = create_new_task(IMPORT_MUTUAL_AUTH_CRL_TASK_NAME, mutual_auth_crl_import_status_monitor, import_crl_data,
        body_jso, i_paras->uri, o_result_jso);
    goto_label_do_info_if_expr(ret != RF_OK, err_exit, task_monitor_info_free(import_crl_data);
        debug_log(DLOG_ERROR, "%s: create new task failed", __FUNCTION__));

    return HTTP_ACCEPTED;

err_exit:
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    return HTTP_INTERNAL_SERVER_ERROR;
}


gint32 check_root_cert_id(json_object *request_body, json_object **o_message_jso, const gchar *class_name,
    OBJ_HANDLE *cert_handle)
{
    gint32 ret;
    gint32 input_cert_id = -1;
    OBJ_HANDLE obj_handle = 0;
    gchar cert_serial_num[ARRAY_LENTH] = {0};
    gchar cert_id_str[MAX_RSC_ID_LEN] = {0};

    if (request_body == NULL || o_message_jso == NULL || class_name == NULL) {
        debug_log(DLOG_ERROR, "%s : input param is null", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    (void)get_element_int(request_body, RFACTION_PARAM_ROOT_CERT_ID, &input_cert_id);
    ret = dal_get_specific_object_byte(class_name, MUTUAL_AUTH_ROOT_CERT_ID, (guchar)input_cert_id, &obj_handle);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : dal_get_specific_object_byte fail, ret:%d", __FUNCTION__, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    (void)dal_get_property_value_string(obj_handle, MUTUAL_AUTH_ROOT_CERT_SERIALNUMBER, cert_serial_num,
        sizeof(cert_serial_num));
    if (strlen(cert_serial_num) == 0) {
        (void)snprintf_s(cert_id_str, sizeof(cert_id_str), sizeof(cert_id_str) - 1, "%d", input_cert_id);
        (void)create_message_info(MSGID_ROOT_CERT_ID_INVALID, NULL, o_message_jso, (const gchar *)cert_id_str);
        return HTTP_BAD_REQUEST;
    }

    if (cert_handle != NULL) {
        *cert_handle = obj_handle;
    }
    return VOS_OK;
}


gint32 act_import_mutual_auth_crl(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret = 0;
    IMPORT_CERT_PARAMS_S params;

    
    return_val_do_info_if_expr(i_paras == NULL || o_message_jso == NULL || o_result_jso == NULL,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param invalid", __FUNCTION__));

    if ((i_paras->user_role_privilege & USERROLE_USERMGNT) == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, PROPERTY_ACCOUNT_SERVICE_IMPORT_CRL, o_message_jso);
        ret = HTTP_FORBIDDEN;
        goto exit;
    }

    
    ret = is_support_mutual_auth(o_message_jso, NULL);
    do_if_expr(ret != VOS_OK, goto exit);

    
    ret = check_root_cert_id(i_paras->val_jso, o_message_jso, MUTUAL_AUTH_ROOT_CLASS_NAME, NULL);
    do_if_expr(ret != VOS_OK, goto exit);

    
    ret =
        is_import_cert_or_crl_task_exist(mutual_auth_crl_import_status_monitor, i_paras, FILE_TYPE_CRL, o_message_jso);
    do_if_expr(ret != VOS_OK, goto exit);

    
    set_import_cert_params(&params, FILE_TYPE_CRL, import_mutual_auth_crl_set_userdata,
        process_import_local_mutual_auth_crl, process_import_remote_mutual_auth_crl, IMPORT_MUTUAL_AUTH_CRL_TMP_PATH);
    ret = process_cert_or_crl_import(i_paras, o_message_jso, o_result_jso, &params);
    
    if (ret == HTTP_OK || ret == HTTP_ACCEPTED || *o_message_jso != NULL) {
        goto exit;
    }

    
    
    switch (ret) {
        case IMPORT_CERT_URI_DISMATCH_ERR:
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFACTION_PARAM_CONTENT, o_message_jso, RF_SENSITIVE_INFO,
                RFACTION_PARAM_CONTENT);
            debug_log(DLOG_ERROR, "%s: uri for importing CRL file is illegal", __FUNCTION__);
            ret = HTTP_BAD_REQUEST;
            break;

        case IMPORT_CERT_INVALID_FILEPATH_ERR:
        case IMPORT_CERT_FILE_LEN_EXCEED_ERR:
            (void)create_message_info(MSGID_CRL_IMPORT_FAILED, NULL, o_message_jso);
            ret = HTTP_BAD_REQUEST;
            break;

        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            debug_log(DLOG_ERROR, "%s: import CRL to mutual auth fail, ret:%d", __FUNCTION__, ret);
            ret = HTTP_INTERNAL_SERVER_ERROR;
    }

exit:
    
    if (ret != HTTP_ACCEPTED) {
        delete_user_specific_file(i_paras, i_paras->val_jso, RFACTION_PARAM_CONTENT);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
    }
    return ret;
}


gint32 act_delete_mutual_auth_crl(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
#define FILE_NOT_EXIT 1
    gint32 ret = HTTP_INTERNAL_SERVER_ERROR;
    gint32 root_cert_id = 0;
    OBJ_HANDLE obj_handle = 0;
    _cleanup_uip_gvariant_list_ GSList *input_list = NULL;
    
    if (i_paras == NULL || o_message_jso == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "input param is null");
        goto exit;
    }

    
    ret = is_support_mutual_auth(o_message_jso, NULL);
    if (ret != RET_OK) {
        goto exit;
    }
    
    (void)get_element_int(i_paras->val_jso, RFACTION_PARAM_ROOT_CERT_ID, &root_cert_id);

    
    ret = check_root_cert_id(i_paras->val_jso, o_message_jso, MUTUAL_AUTH_ROOT_CLASS_NAME, &obj_handle);
    if (ret != RET_OK) {
        goto exit;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(root_cert_id));

    ret = uip_call_class_method_redfish((guchar)i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        MUTUAL_AUTH_ROOT_CLASS_NAME, MUTUAL_AUTH_ROOT_CERT_METHOD_DELETE_CRL, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, NULL);
exit:
    
    switch (ret) {
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
            debug_log(DLOG_ERROR, "privilege check failed");
            break;
        case RET_OK:
            (void)create_message_info(MSGID_SUCCESS, NULL, o_message_jso);
            ret = HTTP_OK;
            break;
        case FILE_NOT_EXIT:
            (void)create_message_info(MSGID_FILE_NOT_EXIST, NULL, o_message_jso);
            ret = HTTP_NOT_FOUND;
            break;
        case HTTP_BAD_REQUEST:
            break;
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            debug_log(DLOG_ERROR, "Delete CRL for mutual root%d fail, ret:%d", root_cert_id, ret);
            ret = HTTP_INTERNAL_SERVER_ERROR;
    }
    return ret;
}


LOCAL PROPERTY_PROVIDER_S g_accountservice_provider_import_crl_actioninfo[] = {};

gint32 accountservice_provider_import_crl_actioninfo_entry(PROVIDER_PARAS_S *i_paras,
    PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    
    return_val_if_expr(i_paras == NULL || prop_provider == NULL || count == NULL, HTTP_INTERNAL_SERVER_ERROR);

    *prop_provider = g_accountservice_provider_import_crl_actioninfo;
    *count = 0;

    return VOS_OK;
}


LOCAL gint32 get_import_crl_actioninfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return get_ldap_controller_action_info_odata_id(i_paras, o_message_jso, o_result_jso,
        PROPERTY_LDAP_SERVICE_COLLECTION_MEMBER, PROPERTY_LDAP_SERVICE_IMPORT_CRL_ACTIONINFO);
}


LOCAL PROPERTY_PROVIDER_S g_ldapservice_controller_importcrl_actioninfo_provide[] = {
    {
        PROPERTY_LDAP_SERVICE_ODATA_ID,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_import_crl_actioninfo_odata_id,
        NULL,
        NULL, ETAG_FLAG_ENABLE
    }
};

gint32 ldapservice_controller_provider_import_crl_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret;
    guchar controller_id = 0;
    OBJ_HANDLE obj_handle = 0;
    gchar uri_full_str[ARRAY_LENTH] = {0};

    return_val_if_expr(i_paras == NULL || prop_provider == NULL || count == NULL, HTTP_INTERNAL_SERVER_ERROR);

    ret = snprintf_s(uri_full_str, sizeof(uri_full_str), sizeof(uri_full_str) - 1, "%s", i_paras->uri);
    return_val_if_expr(ret <= 0, HTTP_INTERNAL_SERVER_ERROR);

    uri_full_str[strlen(i_paras->uri) - strlen(PROPERTY_LDAP_SERVICE_IMPORT_CRL_ACTIONINFO)] = '\0';

    
    ret = get_controller_id(uri_full_str, &controller_id, &obj_handle);
    return_val_if_expr(ret != VOS_OK, HTTP_NOT_FOUND);

    *prop_provider = g_ldapservice_controller_importcrl_actioninfo_provide;
    *count = G_N_ELEMENTS(g_ldapservice_controller_importcrl_actioninfo_provide);
    return VOS_OK;
}


gint32 get_controller_crl_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    guchar crl_enabled_status = 0;
    guchar server_id = 0;
    OBJ_HANDLE obj_handle = 0;

    
    return_val_do_info_if_expr(o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    if (get_controller_id(i_paras->uri, &server_id, &obj_handle) != VOS_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_property_value_byte(obj_handle, LDAP_ATTRIBUTE_CRL_ENABLED, &crl_enabled_status);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get LDAP controller CRLVerificationEnabled fail, ret:%d", __FUNCTION__, ret));

    if (crl_enabled_status == ENABLED) {
        *o_result_jso = json_object_new_boolean(TRUE);
    } else {
        *o_result_jso = json_object_new_boolean(FALSE);
    }

    return HTTP_OK;
}


LOCAL gint32 get_ldap_crl_property_str(PROVIDER_PARAS_S *i_paras, const gchar *origin_prop, const gchar *rf_prop,
    json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar str_val[CERT_ITEM_LEN_128] = {0};
    guchar server_id = 0;
    OBJ_HANDLE obj_handle = 0;

    
    return_val_do_info_if_expr(o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    if (get_controller_id(i_paras->uri, &server_id, &obj_handle) != VOS_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_property_value_string(obj_handle, origin_prop, str_val, sizeof(str_val));
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get LDAP controller %s fail, ret :%d", __FUNCTION__, rf_prop, ret));

    if (strlen(str_val) == 0) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const gchar *)str_val);
    return HTTP_OK;
}


gint32 get_controller_crl_start_time(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    return get_ldap_crl_property_str(i_paras, LDAP_ATTRIBUTE_CRL_START_TIME, PROPERTY_LDAP_SERVICE_CRL_VALID_FROM,
        o_message_jso, o_result_jso);
}


gint32 get_controller_crl_expire_time(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return get_ldap_crl_property_str(i_paras, LDAP_ATTRIBUTE_CRL_EXPIRE_TIME, PROPERTY_LDAP_SERVICE_CRL_VALID_TO,
        o_message_jso, o_result_jso);
}


LOCAL void import_ldap_crl_set_userdata(PROVIDER_PARAS_S *i_paras, json_object *user_data)
{
    json_object *obj_jso = NULL;

    import_cert_set_common_userdata_fn(i_paras, user_data);

    (void)json_object_object_get_ex(i_paras->val_jso, LDAP_CONTROLLER_ID, &obj_jso);
    json_object_object_add(user_data, LDAP_CONTROLLER_ID, json_object_new_int(json_object_get_int(obj_jso)));

    (void)json_object_object_get_ex(i_paras->val_jso, LDAP_OBJ_HANDLE, &obj_jso);
    json_object_object_add(user_data, LDAP_OBJ_HANDLE, json_object_new_int(json_object_get_int(obj_jso)));
    return;
}


LOCAL gint32 process_ldap_import_local_crl(const gchar *file_path, json_object *user_data, json_object **o_message_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    json_object *val_jso = NULL;
    gint64 obj_handle_int = 0;
    gint32 server_id = 0;
    const gchar *user_name = NULL;
    const gchar *client = NULL;
    gint32 user_priv = 0;
    gint32 is_from_webui = 0;
    GSList *input_list = NULL;
    gint32 http_code = HTTP_OK;

    
    goto_label_do_info_if_expr(strlen(file_path) >= MAX_FILEPATH_LENGTH, exit, ret = IMPORT_CERT_INVALID_FILEPATH_ERR;
        debug_log(DLOG_ERROR, "%s: file path length is over %d", __FUNCTION__, MAX_FILEPATH_LENGTH));

    
    goto_label_do_info_if_expr(vos_get_file_length(file_path) > MAX_CRL_FILE_LEN, exit, ret = HTTP_BAD_REQUEST;
        (void)create_message_info(MSGID_CRL_IMPORT_FAILED, NULL, o_message_jso));

    
    if (get_element_int(user_data, LDAP_CONTROLLER_ID, &server_id) == FALSE ||
        json_object_object_get_ex(user_data, LDAP_OBJ_HANDLE, &val_jso) == FALSE) {
        
        debug_log(DLOG_ERROR, "%s: get controller id or object handle fail", __FUNCTION__);
        ret = HTTP_INTERNAL_SERVER_ERROR;
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        goto exit;
    }

    obj_handle_int = json_object_get_int64(val_jso);
    obj_handle = (OBJ_HANDLE)obj_handle_int;
    
    import_cert_get_common_userdata_fn(&user_name, &client, &user_priv, &is_from_webui, user_data);

    
    (void)chmod(file_path, (S_IRUSR | S_IWUSR));

    input_list = g_slist_append(input_list, g_variant_new_byte((guchar)server_id));
    input_list = g_slist_append(input_list, g_variant_new_string(file_path));
    ret = uip_call_class_method_redfish((guchar)is_from_webui, user_name, client, obj_handle, LDAP_CLASS_NAME,
        LDAP_METHOD_IMPORT_CRL, AUTH_ENABLE, user_priv, input_list, NULL);
    uip_free_gvariant_list(input_list);

    do_if_expr(ret != VOS_OK,
        debug_log(DLOG_ERROR, "%s: call remote method %s fail, ret:%d", __FUNCTION__, LDAP_METHOD_IMPORT_CRL, ret));

    parse_import_crl_common_ret_code(ret, o_message_jso, &http_code);
    ret = http_code;

exit:
    do_if_expr(ret != HTTP_OK, vos_rm_filepath(file_path));
    return ret;
}


LOCAL gint32 ldap_import_crl_status_monitor(const gchar *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    return import_cert_common_status_monitor(origin_obj_path, body_jso, process_ldap_import_local_crl, monitor_fn_data,
        message_obj);
}


LOCAL gint32 process_ldap_import_remote_crl(PROVIDER_PARAS_S *i_paras, const gchar *uri, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    TASK_MONITOR_INFO_S *import_crl_info = NULL;
    json_object *body_jso = NULL;

    ret = rf_start_file_download(i_paras, uri, IMPORT_LDAP_CRL_TRANSFER_FILEINFO_CODE, o_message_jso);
    return_val_do_info_if_expr(ret != VOS_OK, ret,
        debug_log(DLOG_ERROR, "%s: start crl file download fail, ret:%d", __FUNCTION__, ret));

    import_crl_info = cert_import_info_new(i_paras, import_ldap_crl_set_userdata);
    goto_label_do_info_if_expr(import_crl_info == NULL, err_exit,
        debug_log(DLOG_ERROR, "%s: alloc new cert_import_info fail", __FUNCTION__));
    import_crl_info->task_progress = TASK_NO_PROGRESS; 

    body_jso = i_paras->val_jso;
    import_crl_info->rsc_privilege = USERROLE_USERMGNT;
    ret = create_new_task(IMPORT_LDAP_CRL_TASK_NAME, ldap_import_crl_status_monitor, import_crl_info, body_jso,
        i_paras->uri, o_result_jso);
    goto_label_do_info_if_expr(ret != RF_OK, err_exit, task_monitor_info_free(import_crl_info);
        debug_log(DLOG_ERROR, "%s: create new task failed(%d)", __FUNCTION__, ret));

    return HTTP_ACCEPTED;

err_exit:
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    return HTTP_INTERNAL_SERVER_ERROR;
}


gint32 act_import_ldap_crl(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 retv;
    guchar controller_id = 0;
    OBJ_HANDLE obj_handle = 0;
    IMPORT_CERT_PARAMS_S param;

    
    retv = import_ldap_cert_check_param(i_paras, o_message_jso, o_result_jso, &controller_id, &obj_handle);
    do_if_expr(retv != VOS_OK, goto exit);

    
    retv = is_import_cert_or_crl_task_exist(ldap_import_crl_status_monitor, i_paras, FILE_TYPE_LDAP_CRL, o_message_jso);
    do_if_expr(retv != VOS_OK, goto exit);

    
    json_object_object_add(i_paras->val_jso, LDAP_CONTROLLER_ID, json_object_new_int(controller_id));
    json_object_object_add(i_paras->val_jso, LDAP_OBJ_HANDLE, json_object_new_int64(obj_handle));

    
    set_import_cert_params(&param, FILE_TYPE_LDAP_CRL, import_ldap_crl_set_userdata, process_ldap_import_local_crl,
        process_ldap_import_remote_crl, IMPORT_LDAP_CRL_TMP_PATH);
    retv = process_cert_or_crl_import(i_paras, o_message_jso, o_result_jso, &param);
    
    if (retv == HTTP_OK || retv == HTTP_ACCEPTED || *o_message_jso != NULL) {
        goto exit;
    }

    
    switch (retv) {
        case IMPORT_CERT_URI_DISMATCH_ERR:
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFACTION_PARAM_CONTENT, o_message_jso, RF_SENSITIVE_INFO,
                RFACTION_PARAM_CONTENT);
            debug_log(DLOG_ERROR, "%s: uri of import file is illegal", __FUNCTION__);
            retv = HTTP_BAD_REQUEST;
            break;

        case IMPORT_CERT_FILE_LEN_EXCEED_ERR:
        case IMPORT_CERT_INVALID_FILEPATH_ERR:
            (void)create_message_info(MSGID_CRL_IMPORT_FAILED, NULL, o_message_jso);
            retv = HTTP_BAD_REQUEST;
            break;

        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            debug_log(DLOG_ERROR, "%s: import CRL to Ldap controller %u fail, ret:%d", __FUNCTION__, controller_id,
                retv);
            retv = HTTP_INTERNAL_SERVER_ERROR;
    }

exit:
    
    if (retv != HTTP_ACCEPTED && i_paras != NULL && i_paras->val_jso != NULL) {
        delete_user_specific_file(i_paras, i_paras->val_jso, RFACTION_PARAM_CONTENT);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
    }
    return retv;
}


gint32 act_delete_ldap_crl(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
#define FILE_NOT_EXIST 1
    gint32 retv;
    guchar controller_id = 0;
    OBJ_HANDLE obj_handle = 0;
    _cleanup_uip_gvariant_list_ GSList *input_list = NULL;
    
    retv = import_ldap_cert_check_param(i_paras, o_message_jso, o_result_jso, &controller_id, &obj_handle);
    if (retv != RET_OK) {
        goto exit;
    }
    input_list = g_slist_append(input_list, g_variant_new_byte((guchar)controller_id));
    retv = uip_call_class_method_redfish((guchar)i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        obj_handle, LDAP_CLASS_NAME, LDAP_METHOD_DELETE_CRL, AUTH_ENABLE, i_paras->user_role_privilege, input_list,
        NULL);
exit:
    
    switch (retv) {
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
            debug_log(DLOG_ERROR, "privilege check failed");
            break;
        case RET_OK:
            (void)create_message_info(MSGID_SUCCESS, NULL, o_message_jso);
            retv = HTTP_OK;
            break;
        case FILE_NOT_EXIST:
            (void)create_message_info(MSGID_FILE_NOT_EXIST, NULL, o_message_jso);
            retv = HTTP_NOT_FOUND;
            break;
        case HTTP_NOT_FOUND:
            debug_log(DLOG_ERROR, "uri of delete file is illegal");
            break;
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            debug_log(DLOG_ERROR, "%s: Delete CRL to Ldap controller %u fail, ret:%d", __FUNCTION__, controller_id,
                retv);
            retv = HTTP_INTERNAL_SERVER_ERROR;
    }
    return retv;
}