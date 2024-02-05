

#include "redfish_provider.h"
#include "pme_app/common/certificates_defs.h"
#include "redfish_provider_cert_update.h"

LOCAL gint32 process_import_local_https_crl(const gchar *file_path, json_object *user_data,
    json_object **o_message_jso);
LOCAL void import_cert_set_param_fn(PROVIDER_PARAS_S *i_paras, json_object *user_data);
LOCAL gint32 process_import_remote_https_crl(PROVIDER_PARAS_S *i_paras, const gchar *uri, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 check_import_cert_params(PROVIDER_PARAS_S* i_paras, const gchar* action_name, json_object** o_message_jso);
LOCAL gint32 act_https_cert_export_csr_instance(PROVIDER_PARAS_S *i_paras, const gchar *file_path,
    json_object **o_message_jso, json_object **o_result_jso);
typedef struct tag_rmcp_cipher_suite {
    guint32 id;
    const gchar *auth_algorithm;
    const gchar *integrity_algorithm;
    const gchar *crypt_algorithm;
    const gchar *rf_prop;
} RMCP_CIPHER_SUITE_S;

LOCAL RMCP_CIPHER_SUITE_S g_rmcp_cipher_suites[] = {
    {1,  "RAKP-HMAC-SHA1",   "None",            "None",        "CipherSuite1"},
    {2,  "RAKP-HMAC-SHA1",   "HMAC-SHA1-96",    "None",        "CipherSuite2"},
    {3,  "RAKP-HMAC-SHA1",   "HMAC-SHA1-96",    "AES-CBC-128", "CipherSuite3"},
    {17, "RAKP-HMAC-SHA256", "HMAC-SHA256-128", "AES-CBC-128", "CipherSuite17"}
};

LOCAL const gchar* g_ssh_ciphers_rf[] = {
    "aes128-ctrEnabled", "aes192-ctrEnabled", "aes256-ctrEnabled",
    "aes128-gcm@openssh.comEnabled", "aes256-gcm@openssh.comEnabled", "chacha20-poly1305@openssh.comEnabled"
};

LOCAL const gchar* g_ssh_kexs_rf[] = {
    "curve25519-sha256Enabled", "curve25519-sha256@libssh.orgEnabled",
    "diffie-hellman-group-exchange-sha256Enabled",
#ifdef SECURITY_ENHANCED_COMPATIBLE_BOARD_V1
    "diffie-hellman-group14-sha1Enabled",
    "diffie-hellman-group-exchange-sha1Enabled"
#endif
};

LOCAL const gchar* g_ssh_macs_rf[] = {
    "hmac-sha2-512Enabled", "hmac-sha2-512-etm@openssh.comEnabled",
    "hmac-sha2-256Enabled", "hmac-sha2-256-etm@openssh.comEnabled",
#ifdef SECURITY_ENHANCED_COMPATIBLE_BOARD_V1
    "hmac-sha1Enabled"
#endif
};

LOCAL const gchar* g_ssh_host_keys_rf[] = {
    "ssh-ed25519Enabled", "rsa-sha2-256Enabled", "rsa-sha2-512Enabled",
#ifdef SECURITY_ENHANCED_COMPATIBLE_BOARD_V1
    "ssh-rsaEnabled",
    "ssh-rsa-cert-v01@openssh.comEnabled"
#endif
};

LOCAL const gchar* g_ssl_cipher_suites_rf[] = {
    "ECDHE-RSA-AES256-GCM-SHA384Enabled", "ECDHE-ECDSA-AES256-GCM-SHA384Enabled", "DHE-RSA-AES256-GCM-SHA384Enabled",
    "ECDHE-RSA-AES128-GCM-SHA256Enabled", "ECDHE-ECDSA-AES128-GCM-SHA256Enabled", "DHE-RSA-AES128-GCM-SHA256Enabled",
    "ECDHE-RSA-CHACHA20-POLY1305Enabled", "DHE-RSA-AES128-CCMEnabled", "DHE-RSA-AES256-CCMEnabled",
    "DHE-RSA-CHACHA20-POLY1305Enabled", "ECDHE-ECDSA-AES128-CCMEnabled", "ECDHE-ECDSA-AES256-CCMEnabled",
    "ECDHE-ECDSA-CHACHA20-POLY1305Enabled", "TLS_CHACHA20_POLY1305_SHA256Enabled", "TLS_AES_256_GCM_SHA384Enabled",
    "TLS_AES_128_GCM_SHA256Enabled"
};

LOCAL const gchar*       g_https_cert_action_array[] = {
    RFACTION_HTTPS_CERT_GEN_CSR,
    RFACTION_HTTPS_CERT_IMPORT_SERVER_CERT,
    RFACTION_HTTPS_CERT_IMPORT_CUST_CERT,
    RFACTION_HTTPS_CERT_EXPORT_CSR
};


LOCAL gint32 get_https_cert_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    int iRet;

    gint32 ret;
    gchar change_uri[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));

    
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__));
    

    iRet = snprintf_s(change_uri, sizeof(change_uri), sizeof(change_uri) - 1, MANAGER_HTTPS_CERT_URI, slot_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    *o_result_jso = json_object_new_string((const gchar*)change_uri);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_https_cert_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return get_odata_context(i_paras, o_message_jso, o_result_jso, MANAGER_HTTPS_CERT_METADATA);
}


LOCAL gint32 get_https_cert_actions(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gchar slot_id[MAX_RSC_ID_LEN] = {0};
    json_object *action_jso = NULL;
    
    guint32 i;
    guint32 len;
    gchar               rsc_uri[MAX_URI_LENGTH] = {0};
    

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    gint32 ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = snprintf_s(rsc_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, MANAGER_HTTPS_CERT_URI, slot_id);
    return_val_do_info_if_fail(ret > 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "format string failed, ret is %d", ret));
    

    action_jso = json_object_new_object();
    if (NULL == action_jso) {
        debug_log(DLOG_ERROR, "alloc new object failed");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    len = G_N_ELEMENTS(g_https_cert_action_array);

    for (i = 0; i < len; i++) {
        rf_add_action_prop(action_jso, (const gchar*)rsc_uri, g_https_cert_action_array[i]);
    }

    

    *o_result_jso = action_jso;

    return HTTP_OK;
}


LOCAL gint32 start_gen_csr(CSR_GEN_INFO_S* csr_gen_info)
{
    GSList* input_list = NULL;

    if (csr_gen_info == NULL) {
        debug_log(DLOG_ERROR, "%s:input param error", __FUNCTION__);
        return VOS_ERR;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(csr_gen_info->country_name));
    input_list = g_slist_append(input_list, g_variant_new_string(csr_gen_info->state_name));
    input_list = g_slist_append(input_list, g_variant_new_string(csr_gen_info->city_name));
    input_list = g_slist_append(input_list, g_variant_new_string(csr_gen_info->org_name));
    input_list = g_slist_append(input_list, g_variant_new_string(csr_gen_info->org_unit));
    input_list = g_slist_append(input_list, g_variant_new_string(csr_gen_info->common_name));

    gint32 ret = uip_call_class_method_redfish(csr_gen_info->req_from_webui_flag, csr_gen_info->user_name,
        csr_gen_info->session_ip, 0, SSL_CLASEE_NAME, SSL_METHOD_GENERATECSR, AUTH_ENABLE, 
        csr_gen_info->user_role_priv, input_list, NULL);
    uip_free_gvariant_list(input_list);
    input_list = NULL;

    
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "call method %s failed, ret is %d", SSL_METHOD_GENERATECSR, ret);
    }

    return ret;
}


LOCAL gint32 csr_gen_status_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    OBJ_HANDLE obj_handle = 0;
    GSList *output_list = NULL;
    gint32 csr_status;

    return_val_do_info_if_fail((NULL != origin_obj_path) && (NULL != body_jso) && (NULL != monitor_fn_data) &&
        (NULL != message_obj),
        RF_FAILED, debug_log(DLOG_ERROR, "%s failed, input param error", __FUNCTION__));

    gint32 ret = dal_get_object_handle_nth(SSL_CLASEE_NAME, 0, &obj_handle);
    goto_label_do_info_if_fail(VOS_OK == ret, exit,
        debug_log(DLOG_ERROR, "get object handle for %s failed", SSL_OBJECT_NAME));

    ret = dfl_call_class_method(obj_handle, SSL_METHOD_GETCSRSTATUS, NULL, NULL, &output_list);
    goto_label_do_info_if_fail(VOS_OK == ret, exit,
        debug_log(DLOG_ERROR, "get csr generate status failed, ret is %d", ret));

    csr_status = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));

    uip_free_gvariant_list(output_list);

    if (monitor_fn_data->task_state == RF_TASK_STARTING) {
        monitor_fn_data->task_state = RF_TASK_RUNNING;
        if (csr_status != CSR_GENERATING) {
            ret = start_gen_csr((CSR_GEN_INFO_S*)monitor_fn_data->user_data);
            if (ret != VOS_OK) {
                debug_log(DLOG_ERROR, "%s, start generate csr fail %d", __FUNCTION__, ret);
                goto exit;
            }
            return RF_OK; 
        } else {
            debug_log(DLOG_ERROR, "%s, task_state:%d, csr_status:%d", __FUNCTION__, 
                monitor_fn_data->task_state, csr_status);
        }
    }

    
    switch (csr_status) {
        case CSR_GEN_READY_OR_OK:
            monitor_fn_data->task_state = RF_TASK_COMPLETED;
            monitor_fn_data->task_progress = FILE_DOWNLOAD_RATIO_TOTAL;
            break;

        case CSR_GENERATING:
            monitor_fn_data->task_state = RF_TASK_RUNNING;
            monitor_fn_data->task_progress = FILE_DOWNLOAD_BASE_RATIO;
            break;

        case CSR_GEN_FAILED:
            monitor_fn_data->task_state = RF_TASK_EXCEPTION;
            (void)create_message_info(MSGID_CSR_GEN_FAILED, NULL, message_obj);
            break;
    }

    return RF_OK;

exit:
    monitor_fn_data->task_state = RF_TASK_EXCEPTION;

    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);

    return RF_OK; 
}


LOCAL gint32 https_cert_gen_csr_precheck(guint32 priv, json_object **o_message_jso)
{
    gint32 available_task_id;
    OBJ_HANDLE obj_handle = 0;
    GSList *output_list = NULL;
    gint32 csr_status;

    
    if (0 == (USERROLE_SECURITYMGNT & priv)) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);

        return HTTP_FORBIDDEN;
    }

    
    gint32 ret = dal_get_object_handle_nth(SSL_CLASEE_NAME, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "get object handle for %s failed", SSL_OBJECT_NAME));

    ret = dfl_call_class_method(obj_handle, SSL_METHOD_GETCSRSTATUS, NULL, NULL, &output_list);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "get csr generate status failed, ret is %d", ret));

    csr_status = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));

    uip_free_gvariant_list(output_list);

    if (CSR_GENERATING == csr_status) {
        (void)create_message_info(MSGID_CSR_GENERATING_ERR, NULL, o_message_jso);

        return HTTP_BAD_REQUEST;
    }

    
    
    available_task_id = find_available_task_id();
    return_val_do_info_if_fail(RF_INVALID_TASK_ID != available_task_id, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_TASK_LIMIT_EXCEED, NULL, o_message_jso));
    

    return RF_OK;
}


LOCAL gint32 act_https_cert_gen_csr(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    CSR_GEN_INFO_S *csr_gen_info = NULL;
    TASK_MONITOR_INFO_S *task_monitor_info = NULL;
    errno_t safe_fun_ret;
    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_message_jso) && (NULL != o_result_jso),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    
    gint32 ret = https_cert_gen_csr_precheck(i_paras->user_role_privilege, o_message_jso);
    return_val_do_info_if_fail(ret == RF_OK, ret, debug_log(DLOG_ERROR, "gen csr precheck failed"));

    csr_gen_info = (CSR_GEN_INFO_S *)g_malloc0(sizeof(CSR_GEN_INFO_S));
    goto_label_do_info_if_fail(NULL != csr_gen_info, err_exit, debug_log(DLOG_ERROR, "malloc a new memory failed"));

    
    (void)json_custom_get_elem_str(i_paras->val_jso, RFACTION_PARAM_STATE, csr_gen_info->state_name,
        SSL_PARAMETER_LEN128 + 1);
    (void)json_custom_get_elem_str(i_paras->val_jso, RFACTION_PARAM_COUNTRY, csr_gen_info->country_name,
        SSL_PARAMETER_LEN2 + 1);
    (void)json_custom_get_elem_str(i_paras->val_jso, RFACTION_PARAM_CITY, csr_gen_info->city_name,
        SSL_PARAMETER_LEN128 + 1);
    (void)json_custom_get_elem_str(i_paras->val_jso, RFACTION_PARAM_ORGUNIT, csr_gen_info->org_unit,
        SSL_PARAMETER_LEN64 + 1);
    (void)json_custom_get_elem_str(i_paras->val_jso, RFACTION_PARAM_ORGNAME, csr_gen_info->org_name,
        SSL_PARAMETER_LEN64 + 1);
    (void)json_custom_get_elem_str(i_paras->val_jso, RFACTION_PARAM_COMMON_NAME, csr_gen_info->common_name,
        SSL_PARAMETER_LEN64 + 1);

    
    csr_gen_info->req_from_webui_flag = i_paras->is_from_webui;
    
    safe_fun_ret = strncpy_s(csr_gen_info->user_name, SESSION_IP_LEN, i_paras->user_name, SESSION_IP_LEN - 1);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    safe_fun_ret = strncpy_s(csr_gen_info->session_ip, SESSION_IP_LEN, i_paras->client, SESSION_IP_LEN - 1);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    csr_gen_info->user_role_priv = i_paras->user_role_privilege;

    task_monitor_info = task_monitor_info_new(g_free);
    
    if (task_monitor_info == NULL) {
        debug_log(DLOG_ERROR, "task_monitor_info_new failed");
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        g_free(csr_gen_info);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    

    task_monitor_info->task_progress = TASK_NO_PROGRESS; 
    task_monitor_info->rsc_privilege = USERROLE_SECURITYMGNT;
    task_monitor_info->task_state = RF_TASK_STARTING;
    task_monitor_info->user_data = csr_gen_info;
    ret = create_new_task("csr generation task", csr_gen_status_monitor, task_monitor_info, i_paras->val_jso,
        i_paras->uri, o_result_jso);

    goto_label_do_info_if_fail(VOS_OK == ret, err_exit, task_monitor_info_free(task_monitor_info);
        debug_log(DLOG_ERROR, "create task for csr generation failed"));

    return HTTP_ACCEPTED;

err_exit:
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);

    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL gint32 parse_http_cert_import_result_code(gint32 err_code, json_object **o_message_jso, guchar cert_type)
{
    gint32 ret = HTTP_BAD_REQUEST;
    const gchar *msg_id = NULL;

    switch (err_code) {
            
        case IMPORT_OK:
            ret = HTTP_OK;
            (void)create_message_info(MSGID_CERT_IMPORT_OK, NULL, o_message_jso);
            break;

            
        case IMPORT_FAIL:
            (void)create_message_info(MSGID_CERT_IMPORT_FAILED, NULL, o_message_jso);
            break;

            
        case FILE_MISMATCH:
            (void)create_message_info(MSGID_CERT_MISMATCH, NULL, o_message_jso);
            break;

            
        case CUSTOM_CERT_IMPORT_FAIL:
            (void)create_message_info(MSGID_ENCRYPTED_CERT_IMPORT_FAILED, NULL, o_message_jso);
            break;
            

            
        case CA_MD_TOO_WEAK:
            (void)create_message_info(MSGID_CA_MD_TOO_WEAK, NULL, o_message_jso, "MD5");
            break;
            
        case CA_SHA1_TOO_WEAK:
            (void)create_message_info(MSGID_CA_MD_TOO_WEAK, NULL, o_message_jso, "SHA1");
            break;
        case SSL_CACERT_OVERDUED:
            (void)create_message_info(MSGID_CERT_EXPIRED, NULL, o_message_jso);
            break;
            
            
        default:
            msg_id = (CUSTOM_CERT_TYPE == cert_type) ? MSGID_ENCRYPTED_CERT_IMPORT_FAILED : MSGID_CERT_IMPORT_FAILED;
            (void)create_message_info(msg_id, NULL, o_message_jso);
            debug_log(DLOG_ERROR, "unknown cert import error:%d", err_code);
            break;
            
    }

    return ret;
}


LOCAL gint32 _rf_perform_server_cert_import(guchar from_webui_flag, const gchar *user_name, const gchar *client_ip,
    json_object *body_jso, json_object **o_message_jso)
{
    gint32 cert_file_len;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    OBJ_HANDLE obj_handle;
    gint32 import_result;

    return_val_do_info_if_fail((NULL != user_name) && (NULL != client_ip) && (NULL != body_jso) &&
        (NULL != o_message_jso),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    
    

    (void)dal_set_file_owner(RS_TMP_SERVER_PEM_FILE, REDFISH_USER_UID, APPS_USER_GID);
    (void)dal_set_file_mode(RS_TMP_SERVER_PEM_FILE, (S_IRUSR | S_IWUSR));
    
    gint32 ret = dal_get_object_handle_nth(SSL_CLASEE_NAME, 0, &obj_handle);
    goto_label_do_info_if_fail(VOS_OK == ret, err_exit,
        debug_log(DLOG_ERROR, "get object handle for %s failed.", SSL_OBJECT_NAME));

    
    
    cert_file_len = vos_get_file_length(RS_TMP_SERVER_PEM_FILE);
    input_list = g_slist_append(input_list, g_variant_new_int32(cert_file_len));
    input_list = g_slist_append(input_list, g_variant_new_string(RS_TMP_SERVER_PEM_FILE));
    
    
    ret = uip_call_class_method_redfish(from_webui_flag, user_name, client_ip, obj_handle, SSL_CLASEE_NAME,
        SSL_METHOD_IMPORTPUBLICKEY, AUTH_DISABLE, 0, input_list, &output_list);
    
    uip_free_gvariant_list(input_list);

    goto_label_do_info_if_fail(VOS_OK == ret, err_exit,
        debug_log(DLOG_ERROR, "call method %s failed, ret is %d", SSL_METHOD_IMPORTPUBLICKEY, ret));

    import_result = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
    uip_free_gvariant_list(output_list);

    
    json_object_clear_string(body_jso, RFACTION_PARAM_CERTICATE);

    
    return parse_http_cert_import_result_code(import_result, o_message_jso, SERVER_CERT_TYPE);
    

err_exit:
    
    json_object_clear_string(body_jso, RFACTION_PARAM_CERTICATE);

    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);

    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL gint32 _rf_perform_custom_cert_import(guchar from_webui_flag, const gchar *user_name, const gchar *client_ip,
    json_object *body_jso, json_object **o_message_jso)
{
    const gchar *param_val = NULL;
    OBJ_HANDLE obj_handle = 0;
    gint32 cert_file_len;
    GSList *input_list = NULL;
    GSList *output_list = NULL;

    (void)get_element_str(body_jso, RFACTION_PARAM_PWD, &param_val);

    
    param_val = (param_val) ? param_val : "";

    
    
    (void)dal_set_file_owner(TMP_RS_CUSTOM_CERT_PFX_FILE, REDFISH_USER_UID, APPS_USER_GID);
    (void)dal_set_file_mode(TMP_RS_CUSTOM_CERT_PFX_FILE, (S_IRUSR | S_IWUSR));

    

    gint32 ret = dal_get_object_handle_nth(SSL_CLASEE_NAME, 0, &obj_handle);
    goto_label_do_info_if_fail(VOS_OK == ret, err_exit,
        debug_log(DLOG_ERROR, "get object handle for %s failed", SSL_OBJECT_NAME));

    
    cert_file_len = vos_get_file_length(TMP_RS_CUSTOM_CERT_PFX_FILE);
    

    debug_log(DLOG_DEBUG, "custom cert file size:%d", cert_file_len);

    
    input_list = g_slist_append(input_list, g_variant_new_int32(cert_file_len));
    input_list = g_slist_append(input_list, g_variant_new_string(param_val));
    
    input_list = g_slist_append(input_list, g_variant_new_string(TMP_RS_CUSTOM_CERT_PFX_FILE));
    

    
    ret = uip_call_class_method_redfish(from_webui_flag, user_name, client_ip, obj_handle, SSL_CLASEE_NAME,
        SSL_METHOD_IMPORTCUSTOMCERT, AUTH_DISABLE, 0, input_list, &output_list);
    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);
    

    do_val_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "call method %s failed, ret is %d", SSL_METHOD_IMPORTCUSTOMCERT, ret));

    

    json_object_clear_string(body_jso, RFACTION_PARAM_CERTICATE);
    json_object_clear_string(body_jso, RFACTION_PARAM_PWD);

    
    
    

    if (ret != SSL_CACERT_OVERDUED && ret != CA_MD_TOO_WEAK && ret != CA_SHA1_TOO_WEAK && ret != IMPORT_OK) {
        ret = CUSTOM_CERT_IMPORT_FAIL;
    }
    
    

    
    return parse_http_cert_import_result_code(ret, o_message_jso, CUSTOM_CERT_TYPE);
    

err_exit:

    

    json_object_clear_string(body_jso, RFACTION_PARAM_CERTICATE);
    json_object_clear_string(body_jso, RFACTION_PARAM_PWD);
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);

    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL gint32 cert_import_status_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    gint32 file_transfer_progress = 0;
    gchar *slash_index = NULL;
    const gchar *file_path = NULL;
    gchar               cert_path[MAX_URL_LEN] = {0};
    const gchar *user_name = NULL;
    const gchar *client_ip = NULL;
    gint32 cert_type = 0;
    const gchar *saved_file_path = NULL;
    
    gint32 from_webui_flag = REDFISH_REQ_FROM_OTHER_CLIENT;
    

    gint32 ret = get_file_transfer_progress(&file_transfer_progress);
    goto_label_do_info_if_fail(VOS_OK == ret, err_quit,
        debug_log(DLOG_ERROR, "get file transfer progress failed, ret is %d", ret));

    if (file_transfer_progress < 0) {
        
        (void)parse_file_transfer_err_code(file_transfer_progress, message_obj);
        monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        
    } else {
        
        monitor_fn_data->task_progress = (guchar)(file_transfer_progress * 0.1);

        
        return_val_do_info_if_fail(RF_FINISH_PERCENTAGE == file_transfer_progress, RF_OK,
            (monitor_fn_data->task_state = RF_TASK_RUNNING));

        
        (void)get_element_str(body_jso, RFACTION_PARAM_CERTICATE, &file_path);
        goto_label_do_info_if_expr(file_path == NULL, err_quit,
            debug_log(DLOG_ERROR, "get param %s from body failed", RFACTION_PARAM_CERTICATE));

        
        slash_index = g_strrstr(file_path, SLASH_FLAG_STR);
        goto_label_do_info_if_fail(NULL != slash_index, err_quit,
            debug_log(DLOG_ERROR, "get slash index from file_path failed"));
        

        ret = snprintf_s(cert_path, MAX_URL_LEN, MAX_URL_LEN - 1, "/tmp%s", slash_index);
        goto_label_do_info_if_fail(ret > 0, err_quit,
            debug_log(DLOG_ERROR, "format certificate file path failed, ret is %d", ret));

        (void)get_element_str((json_object *)monitor_fn_data->user_data, RF_LOG_USER_NAME, &user_name);
        (void)get_element_str((json_object *)monitor_fn_data->user_data, RF_LOG_USER_IP, &client_ip);
        (void)get_element_int((json_object *)monitor_fn_data->user_data, RFPROP_CERT_TYPE, &cert_type);
        
        (void)get_element_int((json_object *)monitor_fn_data->user_data, RF_USERDATA_FROM_WEBUI_FLAG, &from_webui_flag);
        
        
        saved_file_path = (CUSTOM_CERT_TYPE == cert_type) ? TMP_RS_CUSTOM_CERT_PFX_FILE : RS_TMP_SERVER_PEM_FILE;
        

        
        if (0 != g_strcmp0(saved_file_path, cert_path)) {
            // redfish模块降权限后，调用代理方法拷贝文件
            ret = proxy_copy_file((guchar)from_webui_flag, user_name, client_ip, cert_path, saved_file_path,
                REDFISH_USER_UID, REDFISH_USER_GID, COPY_FILE_DELETE_FLAG);
            if (VOS_OK != ret) {
                debug_log(DLOG_ERROR, "%s: copy request file failed", __FUNCTION__);
                (void)proxy_delete_file((guchar)from_webui_flag, user_name, client_ip, cert_path);
                (void)proxy_delete_file((guchar)from_webui_flag, user_name, client_ip, saved_file_path);
                goto err_quit;
            }
        }

        

        if (CUSTOM_CERT_TYPE == cert_type) {
            ret = _rf_perform_custom_cert_import((guchar)from_webui_flag, user_name, client_ip, body_jso, message_obj);
        } else {
            ret = _rf_perform_server_cert_import((guchar)from_webui_flag, user_name, client_ip, body_jso, message_obj);
        }

        monitor_fn_data->task_progress = (HTTP_OK == ret) ? RF_FINISH_PERCENTAGE : RF_FILE_TRANSFER_PERCENTAGE;
        monitor_fn_data->task_state = (HTTP_OK == ret) ? RF_TASK_COMPLETED : RF_TASK_EXCEPTION;
    }

    return RF_OK;

err_quit:
    monitor_fn_data->task_state = RF_TASK_EXCEPTION;
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);

    
    return RF_OK;
}


LOCAL gint32 _rf_perform_cert_import(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso,
    guchar cert_type)
{
    const gchar *cert_str = NULL;
    gint32 ret = VOS_ERR;
    const gchar *type_str = NULL;
    gchar            file_path[MAX_URL_LEN] = {0};
    TASK_MONITOR_INFO_S *monitor_fn_data = NULL;
    guchar cert_file_type = 0;
    const gchar *saved_cert_file_path = NULL;
    const gchar *cert_file_regex = NULL;

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_message_jso) && (NULL != o_result_jso),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    if (0 == (USERROLE_SECURITYMGNT & i_paras->user_role_privilege)) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    (void)get_element_str(i_paras->val_jso, RFACTION_PARAM_TYPE, &type_str);

    (void)get_element_str(i_paras->val_jso, RFACTION_PARAM_CERTICATE, &cert_str);
    goto_label_do_info_if_fail(NULL != cert_str, err_exit,
        debug_log(DLOG_ERROR, "cann't get %s from request.", RFACTION_PARAM_CERTICATE));

    gboolean b_ret = check_redfish_running_task_exist(cert_import_status_monitor, i_paras->val_jso, i_paras->uri, NULL);
    return_val_do_info_if_fail(FALSE == b_ret, HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "cert import already task exist");
        (void)create_message_info(MSGID_CERT_IMPORTING, NULL, o_message_jso));

    
    type_str = (NULL != type_str) ? type_str : RFACTION_FORMAT_TEXT;
    if (0 == g_strcmp0(type_str, RFACTION_FORMAT_FILE)) {
        cert_file_regex = (CUSTOM_CERT_TYPE == cert_type) ? CUSTOM_CERT_IMPORT_REGEX : SERVER_CERT_IMPORT_REGEX;
        b_ret = g_regex_match_simple(cert_file_regex, cert_str, G_REGEX_OPTIMIZE, (GRegexMatchFlags)0);
        return_val_do_info_if_fail(TRUE == b_ret, HTTP_BAD_REQUEST,
            debug_log(DLOG_ERROR, "unsupported certificate file path");
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFACTION_PARAM_CERTICATE, o_message_jso, RF_SENSITIVE_INFO,
            RFACTION_PARAM_CERTICATE));
    }

    
    ret = rf_save_import_content_to_file(i_paras, type_str, cert_str, cert_type, file_path, MAX_URL_LEN,
        &cert_file_type, o_message_jso);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "save import content to local file failed, ret is %d", ret));

    
    if (cert_file_type != FILE_TYPE_URI) {
        saved_cert_file_path = (CUSTOM_CERT_TYPE == cert_type) ? TMP_RS_CUSTOM_CERT_PFX_FILE : RS_TMP_SERVER_PEM_FILE;

        ret = dal_check_real_path((const gchar*)file_path);
        goto_label_do_info_if_fail(VOS_OK == ret, err_exit,
            debug_log(DLOG_ERROR, "check real path failed, return %d", ret));

        if (cert_file_type == FILE_TYPE_LOCAL && !dal_check_file_opt_user(i_paras->auth_type,
            (const gchar *)i_paras->user_name, (const gchar *)file_path, (const gchar *)i_paras->user_roleid)) {
            (void)create_message_info(MSGID_NO_PRIV_OPT_FILE, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        }

        // 源文件和临时文件的路径和文件名不能相同
        if (0 != g_strcmp0(saved_cert_file_path, file_path)) {
            // redfish模块降权限后，调用代理方法拷贝文件
            ret = proxy_copy_file(i_paras->is_from_webui, i_paras->user_name, i_paras->client, (const gchar*)file_path,
                saved_cert_file_path, REDFISH_USER_UID, REDFISH_USER_GID, COPY_FILE_DELETE_FLAG);
            if (VOS_OK != ret) {
                debug_log(DLOG_ERROR, "%s: copy request file failed", __FUNCTION__);
                (void)proxy_delete_file(i_paras->is_from_webui, i_paras->user_name, i_paras->client, 
                    (const gchar*)file_path);
                (void)proxy_delete_file(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
                    saved_cert_file_path);
                goto err_exit;
            }
        }

        if (CUSTOM_CERT_TYPE == cert_type) {
            return _rf_perform_custom_cert_import(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
                i_paras->val_jso, o_message_jso);
        } else {
            return _rf_perform_server_cert_import(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
                i_paras->val_jso, o_message_jso);
        }
    }

    monitor_fn_data = task_monitor_info_new((GDestroyNotify)json_object_put);
    goto_label_do_info_if_expr(monitor_fn_data == NULL, err_exit, debug_log(DLOG_ERROR, "dup file path failed"));
    monitor_fn_data->user_data = json_object_new_object();
    goto_label_do_info_if_expr(monitor_fn_data->user_data == NULL, err_exit,
        debug_log(DLOG_ERROR, "dup file path failed");
        task_monitor_info_free(monitor_fn_data));

    json_object_object_add((json_object *)monitor_fn_data->user_data, RF_LOG_USER_NAME,
        json_object_new_string(i_paras->user_name));
    json_object_object_add((json_object *)monitor_fn_data->user_data, RF_LOG_USER_IP,
        json_object_new_string(i_paras->client));
    json_object_object_add((json_object *)monitor_fn_data->user_data, RFPROP_CERT_TYPE, json_object_new_int(cert_type));
    json_object_object_add((json_object *)monitor_fn_data->user_data, RF_USERDATA_FROM_WEBUI_FLAG,
        json_object_new_int(i_paras->is_from_webui));

    monitor_fn_data->rsc_privilege = USERROLE_SECURITYMGNT;
    ret = create_new_task("certificate import task", cert_import_status_monitor, monitor_fn_data, i_paras->val_jso,
        i_paras->uri, o_result_jso);
    goto_label_do_info_if_fail(VOS_OK == ret, err_exit, task_monitor_info_free(monitor_fn_data));

    return HTTP_ACCEPTED;
    

    
err_exit:
    do_val_if_expr(VOS_ERR == ret, (ret = IMPORT_COMMON_ERR));
    parse_http_cert_import_result_code(ret, o_message_jso, cert_type);
    

    return HTTP_BAD_REQUEST;
}


LOCAL gint32 act_https_cert_import_server_cert(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret = _rf_perform_cert_import(i_paras, o_message_jso, o_result_jso, SERVER_CERT_TYPE);
    if (ret != HTTP_ACCEPTED && i_paras != NULL) {
        delete_user_specific_file(i_paras, i_paras->val_jso, RFACTION_PARAM_CERTICATE);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CERTICATE);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_PWD);
    }
    return ret;
}


LOCAL gint32 act_https_cert_import_custom_cert(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret = _rf_perform_cert_import(i_paras, o_message_jso, o_result_jso, CUSTOM_CERT_TYPE);
    if (ret != HTTP_ACCEPTED && i_paras != NULL) {
        delete_user_specific_file(i_paras, i_paras->val_jso, RFACTION_PARAM_CERTICATE);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CERTICATE);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_PWD);
    }
    return ret;
}


LOCAL gint32 _parse_csr_export_result_code(gint32 result_code, json_object **o_message_jso)
{
    gint32 http_ret;

    switch (result_code) {
        case VOS_OK:
            http_ret = HTTP_OK;
            break;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            http_ret = HTTP_FORBIDDEN;
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
            break;

        default:
            http_ret = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_EXPORT_CSR_FAILED, NULL, o_message_jso);
            debug_log(DLOG_ERROR, "export csr file failed, ret is %d", result_code);
            break;
    }

    return http_ret;
}


LOCAL gint32 csr_export_status_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    gint32 transfer_status = 0;
    GSList *caller_info = NULL;
    const gchar *user_name = NULL;
    const gchar *user_ip = NULL;
    
    gint32 from_webui_flag = REDFISH_REQ_FROM_OTHER_CLIENT;
    

    gint32 ret = get_file_transfer_progress(&transfer_status);
    goto_label_do_info_if_fail(VOS_OK == ret, err_quit,
        debug_log(DLOG_ERROR, "get file transfer progress failed, ret is %d", ret));

    (void)get_element_str((json_object *)monitor_fn_data->user_data, RF_LOG_USER_NAME, &user_name);
    (void)get_element_str((json_object *)monitor_fn_data->user_data, RF_LOG_USER_IP, &user_ip);
    
    (void)get_element_int((json_object *)monitor_fn_data->user_data, RF_USERDATA_FROM_WEBUI_FLAG, &from_webui_flag);

    caller_info =
        g_slist_append(caller_info, g_variant_new_string(dal_rf_get_op_log_module_name((guchar)from_webui_flag)));
    

    caller_info = g_slist_append(caller_info, g_variant_new_string(user_name));
    caller_info = g_slist_append(caller_info, g_variant_new_string(user_ip));
    if (transfer_status < 0) {
        (void)parse_file_transfer_err_code(transfer_status, message_obj);
        monitor_fn_data->task_state = RF_TASK_EXCEPTION;

        proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "Export CSR failed");
        uip_free_gvariant_list(caller_info);

        return RF_OK;
    } else {
        monitor_fn_data->task_progress = transfer_status;

        monitor_fn_data->task_state = (RF_FINISH_PERCENTAGE == transfer_status) ? RF_TASK_COMPLETED : RF_TASK_RUNNING;

        do_val_if_expr(RF_FINISH_PERCENTAGE == transfer_status,
            proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "Export CSR successfully"));
        uip_free_gvariant_list(caller_info);

        return RF_OK;
    }

err_quit:
    monitor_fn_data->task_state = RF_TASK_EXCEPTION;
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);

    proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "Export CSR failed");
    uip_free_gvariant_list(caller_info);

    
    return RF_OK;
}


LOCAL gint32 act_https_cert_export_csr(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    const gchar *file_path = NULL;

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_message_jso) && (NULL != o_result_jso),
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    if ((i_paras->user_role_privilege & USERROLE_SECURITYMGNT) == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    (void)get_element_str(i_paras->val_jso, RFACTION_PARAM_CONTENT, &file_path);
    return_val_do_info_if_fail(NULL != file_path, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    // 判断URI是否为目录名
    if (file_path[0] == '/' && g_file_test(file_path, G_FILE_TEST_IS_DIR) == TRUE) {
        (void)create_message_info(MSGID_NOT_ALLOW_OVERWRITE_DIR, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }

    if (file_path[0] == '/' && dal_check_real_path2(file_path, TMP_PATH_WITH_SLASH) != RET_OK) {
        (void)create_message_info(MSGID_INVALID_PATH, RFACTION_PARAM_CONTENT, o_message_jso, RF_SENSITIVE_INFO,
            RFACTION_PARAM_CONTENT);
        return HTTP_BAD_REQUEST;
    }

    return act_https_cert_export_csr_instance(i_paras, file_path, o_message_jso, o_result_jso);
}

LOCAL gint32 act_https_cert_export_csr_instance(PROVIDER_PARAS_S *i_paras, const gchar *file_path,
    json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    GSList *input_list = NULL;
    TASK_MONITOR_INFO_S *monitor_fn_data = NULL;

    input_list = g_slist_append(input_list, g_variant_new_string(file_path));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, OBJ_HANDLE_COMMON,
        SSL_CLASEE_NAME, SSL_METHOD_EXPORT_CSR, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    if (file_path[0] == '/' && ret == VOS_OK) {
        ret = dal_change_file_owner(i_paras->auth_type, file_path, i_paras->user_name, S_IRUSR | S_IWUSR);
        if (ret != VOS_OK) {
            debug_log(DLOG_ERROR, "change file owner failed,ret is %d", ret);
            (void)proxy_delete_file(0, NULL, NULL, file_path);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        (void)create_message_info(MSGID_SUCCESS, NULL, o_message_jso);
        return HTTP_OK;
    }

    
    // 3.2 任务重复性检查
    gboolean b_ret = check_redfish_running_task_exist(csr_export_status_monitor, i_paras->val_jso, i_paras->uri, NULL);
    if (FALSE != b_ret) {
        // 日志打印
        debug_log(DLOG_ERROR, "%s failed:running task exist", __FUNCTION__);

        // 创建消息
        (void)create_message_info(MSGID_TASK_CSR_EXPORT_ERR, NULL, o_message_jso);

        // 返回结果
        return HTTP_BAD_REQUEST;
    }

    

    ret = _parse_csr_export_result_code(ret, o_message_jso);
    
    return_val_do_info_if_fail(HTTP_OK == ret, ret, debug_log(DLOG_ERROR, "export csr failed, ret is %d", ret));
    

    monitor_fn_data = task_monitor_info_new((GDestroyNotify)json_object_put);
    
    if (NULL == monitor_fn_data) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "task_monitor_info_new failed");
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    

    monitor_fn_data->user_data = (void *)json_object_new_object();
    return_val_do_info_if_fail(NULL != monitor_fn_data->user_data, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "alloc new json object failed");
        task_monitor_info_free(monitor_fn_data); (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    json_object_object_add((json_object *)monitor_fn_data->user_data, RF_LOG_USER_NAME,
        json_object_new_string(i_paras->user_name));
    json_object_object_add((json_object *)monitor_fn_data->user_data, RF_LOG_USER_IP,
        json_object_new_string(i_paras->client));
    
    json_object_object_add((json_object *)monitor_fn_data->user_data, RF_USERDATA_FROM_WEBUI_FLAG,
        json_object_new_int((int32_t)i_paras->is_from_webui));
    

    monitor_fn_data->rsc_privilege = USERROLE_SECURITYMGNT;
    ret = create_new_task("csr export task", csr_export_status_monitor, monitor_fn_data, i_paras->val_jso, i_paras->uri,
        o_result_jso);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "create csr export task failed, ret is %d", ret);
        task_monitor_info_free(monitor_fn_data); (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    return HTTP_ACCEPTED;
}


LOCAL gint32 packet_query_ssl_cert_param(GSList **prop_name_list)
{
    return_val_do_info_if_expr(prop_name_list == NULL, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: invalid param", __FUNCTION__));

    *prop_name_list = g_slist_append(*prop_name_list, SSL_SUBJECT_INFO);
    *prop_name_list = g_slist_append(*prop_name_list, SSL_ISSUER_INFO);
    *prop_name_list = g_slist_append(*prop_name_list, SSL_START_TIME);
    *prop_name_list = g_slist_append(*prop_name_list, SSL_EXPIRATION);
    *prop_name_list = g_slist_append(*prop_name_list, SSL_SN);
    *prop_name_list = g_slist_append(*prop_name_list, SSL_SIG_ALGO);
    *prop_name_list = g_slist_append(*prop_name_list, SSL_KEY_USAGE);
    *prop_name_list = g_slist_append(*prop_name_list, SSL_KEY_LENGTH);

    return VOS_OK;
}


LOCAL gint32 x509_cert_info_pack(OBJ_HANDLE object_handle, json_object **cert_info_jso)
{
    GSList *prop_name_list = NULL;
    GSList *prop_val_list = NULL;
    json_object *info_jso = NULL;
    const gchar *subject_str = NULL;
    const gchar *issuer_str = NULL;
    const gchar *start_time_str = NULL;
    const gchar *expiration_str = NULL;
    const gchar *serial_number_str = NULL;
    const gchar *key_usage_str = NULL;
    const gchar *sig_algo_str = NULL;
    guint32 key_len;

    
    gint32 ret = packet_query_ssl_cert_param(&prop_name_list);
    return_val_do_info_if_expr(ret != VOS_OK, ret,
        debug_log(DLOG_ERROR, "%s: packet_query_ssl_cert_param failed", __FUNCTION__));

    ret = dfl_multiget_property_value(object_handle, prop_name_list, &prop_val_list);
    g_slist_free(prop_name_list);
    return_val_do_info_if_fail(ret == VOS_OK, VOS_ERR,
        debug_log(DLOG_ERROR, "%s has failed:cann't get mutiple properties", __FUNCTION__));

    info_jso = json_object_new_object();
    return_val_do_info_if_fail(ret == VOS_OK, VOS_ERR, uip_free_gvariant_list(prop_val_list);
        debug_log(DLOG_ERROR, "%s has failed:alloc a new jso failed", __FUNCTION__));
    subject_str = g_variant_get_string((GVariant *)g_slist_nth_data(prop_val_list, 0), NULL);
    issuer_str = g_variant_get_string((GVariant *)g_slist_nth_data(prop_val_list, 1), NULL);
    start_time_str = g_variant_get_string((GVariant *)g_slist_nth_data(prop_val_list, 2), NULL);
    expiration_str = g_variant_get_string((GVariant *)g_slist_nth_data(prop_val_list, 3), NULL);
    serial_number_str = g_variant_get_string((GVariant *)g_slist_nth_data(prop_val_list, 4), NULL);
    sig_algo_str = g_variant_get_string((GVariant *)g_slist_nth_data(prop_val_list, 5), NULL);
    key_usage_str = g_variant_get_string((GVariant *)g_slist_nth_data(prop_val_list, 6), NULL);
    key_len = g_variant_get_uint32((GVariant *)g_slist_nth_data(prop_val_list, 7));

    json_object_object_add(info_jso, RFPROP_HTTPS_CERT_SUBJECT_INFO, json_object_new_string(subject_str));
    json_object_object_add(info_jso, RFPROP_HTTPS_CERT_ISSUER_INFO, json_object_new_string(issuer_str));
    json_object_object_add(info_jso, RFPROP_HTTPS_CERT_START_TIME, json_object_new_string(start_time_str));
    json_object_object_add(info_jso, RFPROP_HTTPS_CERT_EXPIRATION, json_object_new_string(expiration_str));
    json_object_object_add(info_jso, RFPROP_HTTPS_CERT_SN, json_object_new_string(serial_number_str));
    json_object_object_add(info_jso, RFPROP_HTTPS_CERT_SIG_ALGO, json_object_new_string(sig_algo_str));
    json_object_object_add(info_jso, RFPROP_HTTPS_CERT_KEY_USAGE, json_object_new_string(key_usage_str));
    json_object_object_add(info_jso, RFPROP_HTTPS_CERT_KEY_LEN, json_object_new_int(key_len));

    uip_free_gvariant_list(prop_val_list);

    *cert_info_jso = info_jso;

    return VOS_OK;
}


LOCAL gboolean get_x509_default_flag(OBJ_HANDLE object_handle)
{
    gchar issuer_info[CERT_INFO_LEN] = {0};
    gint32 ret = dal_get_property_value_string(object_handle, SSL_ISSUER_INFO, issuer_info, sizeof(issuer_info));
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get issuer prop val fail:%d", __FUNCTION__, ret);
        return FALSE;
    }

    CERT_INFO_T cert_info = {0};
    // 由于预置证书判断条件只关心C,O,CN,故要求这三项必须要有内容, 其他项跳过
    gint32 out_ver_num = sscanf_s(issuer_info, "CN=%[^,], OU%*[^,], O=%[^,], L%*[^,], S%*[^,], C=%[^,]",
        cert_info.common_name, sizeof(cert_info.common_name),
        cert_info.org, sizeof(cert_info.org),
        cert_info.country, sizeof(cert_info.country));
    if (out_ver_num != 3) { // 预置证书判断条件只关心3项: C,O,CN
        debug_log(DLOG_ERROR, "%s: parse issuer_info fail %d", __FUNCTION__, out_ver_num);
        return FALSE;
    }
    
    _cleanup_gslist_ GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    ret = dfl_get_object_list(CLASS_PRE_SSL_SERVER_INFO, &obj_list);
    if (ret != RET_OK || obj_list == NULL) {
        return FALSE;
    }

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        OBJ_HANDLE obj_handle = (OBJ_HANDLE)obj_node->data;
 
        gchar common_name[CERT_ITEM_LEN_64] = {0};
        gchar org[CERT_ITEM_LEN_64] = {0};
        gchar country[CERT_ITEM_LEN_2] = {0};

        (void)dal_get_property_value_string(obj_handle, SSL_COMMON_NAME, common_name, sizeof(common_name));
        (void)dal_get_property_value_string(obj_handle, SSL_ORG_NAME, org, sizeof(org));
        (void)dal_get_property_value_string(obj_handle, SSL_COUNRTY, country, sizeof(country));

         if (g_strcmp0(common_name, cert_info.common_name) == 0 &&
            g_strcmp0(org, cert_info.org) == 0 &&
            g_strcmp0(country, cert_info.country) == 0) {
            return TRUE;
        }
    }

    return FALSE;
}


LOCAL gint32 x509_cert_info_fetch_func(OBJ_HANDLE object_handle, gpointer user_data)
{
    guchar cert_type = 0;
    json_object *cert_info_jso = (json_object *)user_data;
    json_object *x509_cert_jso = NULL;
    json_object *intermediate_cert_array_jso = NULL;

    gint32 ret = dal_get_property_value_byte(object_handle, SSL_CERT_TYPE, &cert_type);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_OK,
        debug_log(DLOG_ERROR, "get property %s failed during object foreach", SSL_CERT_TYPE));

    
    return_val_if_expr(CERT_TYPE_RESERVED == cert_type, VOS_OK);

    ret = x509_cert_info_pack(object_handle, &x509_cert_jso);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_OK, debug_log(DLOG_ERROR, "pack cert info failed"));

    if (CERT_TYPE_SERVER == cert_type) {
        // 添加是否是预置证书的标志
        json_object_object_add(x509_cert_jso, RFPROP_HTTPS_CERT_IS_DEFAULT_CERT,
            json_object_new_boolean(get_x509_default_flag(object_handle)));
        json_object_object_add(cert_info_jso, RFPROP_HTTPS_CERT_SEVER_CERT, x509_cert_jso);
    } else if (CERT_TYPE_INTERMEDIATE == cert_type) {
        (void)json_object_object_get_ex(cert_info_jso, RFPROP_HTTPS_CERT_INTER_CERT, &intermediate_cert_array_jso);
        
        do_val_if_fail(NULL != intermediate_cert_array_jso, intermediate_cert_array_jso = json_object_new_array();
            json_object_object_add(cert_info_jso, RFPROP_HTTPS_CERT_INTER_CERT, intermediate_cert_array_jso));

        ret = json_object_array_add(intermediate_cert_array_jso, x509_cert_jso);
        return_val_do_info_if_fail(0 == ret, VOS_OK, (void)json_object_put(x509_cert_jso));
    } else if (CERT_TYPE_ROOT == cert_type) {
        json_object_object_add(cert_info_jso, RFPROP_HTTPS_CERT_ROOT_CERT, x509_cert_jso);
    } else {
        (void)json_object_put(x509_cert_jso);
        debug_log(DLOG_ERROR, "unknown cert type :%d", cert_type);
    }

    return VOS_OK;
}


LOCAL gint32 get_https_cert_x509_info(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    json_object *cert_info_jso = NULL;

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_message_jso) && (NULL != o_result_jso),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    cert_info_jso = json_object_new_object();
    (void)dfl_foreach_object(SSL_CLASEE_NAME, (OBJECT_FOREACH_FUNC)x509_cert_info_fetch_func, (gpointer)cert_info_jso,
        NULL);

    *o_result_jso = cert_info_jso;

    return HTTP_OK;
}


LOCAL gint32 get_https_cert_csr_info(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    GSList *output_list = NULL;
    const gchar *csr_string = NULL;

    
    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_message_jso) && (NULL != o_result_jso) &&
        (0 != i_paras->is_satisfy_privi),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));
    

    gint32 ret = dal_get_object_handle_nth(SSL_CLASEE_NAME, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "get object handle for %s failed", SSL_OBJECT_NAME));

    ret = dfl_call_class_method(obj_handle, SSL_METHOD_GET_CSR_STRING, NULL, NULL, &output_list);
    
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "call method %s failed, ret is %d", SSL_METHOD_GET_CSR_STRING, ret));

    csr_string = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    *o_result_jso = json_object_new_string(csr_string);

    uip_free_gvariant_list(output_list);

    return HTTP_OK;
}

LOCAL PROPERTY_PROVIDER_S g_https_cert_provider[] = {
    {
        RFPROP_ODATA_ID,
        "", "",
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_https_cert_odataid,
        NULL,
        NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_ODATA_CONTEXT,
        "", "",
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_https_cert_odata_context,
        NULL,
        NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_ACTIONS,
        "", "",
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_https_cert_actions,
        NULL,
        NULL,
        ETAG_FLAG_DISABLE
    },
    {
        RFACTION_HTTPS_CERT_GEN_CSR,
        "", "",
        USERROLE_SECURITYMGNT,
        SYS_LOCKDOWN_FORBID,
        NULL,
        NULL,
        act_https_cert_gen_csr,
        ETAG_FLAG_ENABLE
    },
    {
        RFACTION_HTTPS_CERT_IMPORT_SERVER_CERT,
        "", "",
        USERROLE_SECURITYMGNT,
        SYS_LOCKDOWN_FORBID,
        NULL,
        NULL,
        act_https_cert_import_server_cert,
        ETAG_FLAG_ENABLE
    },
    {
        RFACTION_HTTPS_CERT_IMPORT_CUST_CERT,
        "", "",
        USERROLE_SECURITYMGNT,
        SYS_LOCKDOWN_FORBID,
        NULL,
        NULL,
        act_https_cert_import_custom_cert,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_HTTPS_CERT_X509_CERT_INFO,
        "", "",
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_https_cert_x509_info,
        NULL,
        NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_HTTPS_CERT_CSR_INFO,
        "", "",
        USERROLE_SECURITYMGNT,
        SYS_LOCKDOWN_NULL,
        get_https_cert_csr_info,
        NULL,
        NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFACTION_HTTPS_CERT_EXPORT_CSR,
        "", "",
        USERROLE_SECURITYMGNT,
        SYS_LOCKDOWN_FORBID,
        NULL,
        NULL,
        act_https_cert_export_csr,
        ETAG_FLAG_ENABLE
    }
};


gint32 https_cert_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gboolean b_ret;

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count), VOS_ERR,
        debug_log(DLOG_ERROR, "input param error"));

    b_ret = redfish_check_manager_uri_valid(i_paras->uri); // 校验用户输入的URI是否有效
    if (TRUE == b_ret) {
        *prop_provider = g_https_cert_provider;
        *count = sizeof(g_https_cert_provider) / sizeof(PROPERTY_PROVIDER_S);
        return VOS_OK;
    } else {
        return HTTP_NOT_FOUND;
    }
}

LOCAL const gchar* g_security_svc_action_array[] = {
    RFACTION_UPDATE_MASTERKEY,
    RFACTION_SECURITY_SERVICE_IMPORT_TRUST_CERT,
    RFACTION_SECURITY_SERVICE_DELETE_TRUST_CERT,
    RFACTION_SECURITY_SERVICE_IMPORT_CRL,
    RFACTION_SECURITY_SERVICE_DELETE_CRL
};


LOCAL gint32 get_security_svc_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;

    gint32 ret;
    gchar change_uri[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__));
    
    iRet = snprintf_s(change_uri, sizeof(change_uri), sizeof(change_uri) - 1, MANAGER_SECURITY_SVC_URI, slot_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    *o_result_jso = json_object_new_string((const gchar*)change_uri);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_security_svc_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;

    gint32 ret;
    gchar change_uri[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));

    
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__));
    

    iRet = snprintf_s(change_uri, sizeof(change_uri), sizeof(change_uri) - 1, MANAGER_SECURITY_SVC_METADATA, slot_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    *o_result_jso = json_object_new_string((const gchar*)change_uri);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_security_svc_security_control_version(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
#define SECURITY_LEVEL1 1
#define SECURITY_LEVEL2 2
#define SECURITY_LEVEL3 3

    guint8 security_level;
    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (!i_paras->is_satisfy_privi) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }
#ifdef SECURITY_ENHANCED_COMPATIBLE_BOARD_V1
    security_level = SECURITY_LEVEL1;
#else
#ifdef SECURITY_ENHANCED_COMPATIBLE_BOARD_V2
    security_level = SECURITY_LEVEL2;
#else
    security_level = SECURITY_LEVEL3;
#endif
#endif
    *o_result_jso = json_object_new_int(security_level);
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s:json_object_new_int fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 get_security_svc_session_token_length(PROVIDER_PARAS_S* i_paras, 
                                                  json_object** o_message_jso, 
                                                  json_object** o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    guint32 prop_val = 0;

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (!i_paras->is_satisfy_privi) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    gint32 ret = dal_get_object_handle_nth(CLASS_NAME_PRODUCT, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:dal_get_object_handle_nth failed, ret=%d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_PRODUCT_SESSION_TOKEN_LENGTH, &prop_val);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get %s.%s failed, ret=%d.", __FUNCTION__, 
                  CLASS_NAME_PRODUCT, PROPERTY_PRODUCT_SESSION_TOKEN_LENGTH, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_int(prop_val);
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s:json_object_new_int fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
  
    return HTTP_OK;
}


LOCAL gint32 set_security_svc_session_token_length(PROVIDER_PARAS_S* i_paras, 
                                                  json_object** o_message_jso, 
                                                  json_object** o_result_jso)
{
    guint32 prop_val;
    GSList* input_list = NULL;
    
    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (json_object_get_type(i_paras->val_jso) != json_type_int) {
        debug_log(DLOG_INFO, "%s: json type error.", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_TYPE_ERR, RFPROPERTY_SESSION_TOKEN_LENGTH, o_message_jso, 
            dal_json_object_get_str(i_paras->val_jso), RFPROPERTY_SESSION_TOKEN_LENGTH);
        return HTTP_BAD_REQUEST;
    }
    
    prop_val = json_object_get_int(i_paras->val_jso);
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_uint32(prop_val));
    gint32 ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, 0, 
        CLASS_NAME_PRODUCT, METHOD_PRODUCT_SET_SESSION_TOKEN_LENGTH, AUTH_ENABLE, 
        i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case RET_OK:
            return HTTP_OK;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROPERTY_SESSION_TOKEN_LENGTH,
                o_message_jso, RFPROPERTY_SESSION_TOKEN_LENGTH);
            return HTTP_BAD_REQUEST;

        default:
            debug_log(DLOG_ERROR, "%s: call method %s failed, ret = %d", __FUNCTION__,
                RFPROPERTY_SESSION_TOKEN_LENGTH, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 get_security_svc_mk_auto_update_interval(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint32 prop_val = 0;
    guchar software_type = 0;
    (void)dal_get_software_type(&software_type);
    if (software_type == MGMT_SOFTWARE_TYPE_EM) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dal_get_object_handle_nth(CLASS_SECURITY_ENHANCE, 0, &obj_handle);
    return_val_do_info_if_expr(ret != DFL_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: dal_get_object_handle_nth failed, ret=%d.", __FUNCTION__, ret));

    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_SECURITY_MASTERKEY_UPDATE_INTERVAL, &prop_val);
    return_val_do_info_if_fail(ret == VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get %s.%s failed, ret=%d.", __FUNCTION__, dfl_get_object_name(obj_handle),
        PROPERTY_SECURITY_MASTERKEY_UPDATE_INTERVAL, ret));

    *o_result_jso = json_object_new_int(prop_val);
    return_val_do_info_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:json_object_new_int fail", __FUNCTION__));

    return HTTP_OK;
}


LOCAL gint32 set_security_svc_mk_auto_update_interval(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint32 update_interval;
    GSList *input_list = NULL;
    guchar MgmtSoftWareType = 0;
    (void)dal_get_software_type(&MgmtSoftWareType);
    if (MgmtSoftWareType == MGMT_SOFTWARE_TYPE_EM) {
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, RFPROP_MK_AUTO_UPDATE_INTERVAL, o_message_jso,
            RFPROP_MK_AUTO_UPDATE_INTERVAL);
        debug_log(DLOG_INFO, "%s: em not support.", __FUNCTION__);
        return HTTP_NOT_IMPLEMENTED;
    }

    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (json_object_get_type(i_paras->val_jso) != json_type_int) {
        debug_log(DLOG_INFO, "%s: json type error.", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_TYPE_ERR, RFPROP_MK_AUTO_UPDATE_INTERVAL, o_message_jso,
            dal_json_object_get_str(i_paras->val_jso), RFPROP_MK_AUTO_UPDATE_INTERVAL);
        return HTTP_BAD_REQUEST;
    }
    update_interval = json_object_get_int(i_paras->val_jso);

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_uint32(update_interval));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, 0,
        CLASS_SECURITY_ENHANCE, METHOD_SECURITY_ENHANCE_SETMASTERKEYUPDATEINTERVAL, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
            return HTTP_OK;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_MK_AUTO_UPDATE_INTERVAL, o_message_jso,
                RFPROP_MK_AUTO_UPDATE_INTERVAL);
            return HTTP_BAD_REQUEST;

        default:
            debug_log(DLOG_ERROR, "%s: ret = %d", __FUNCTION__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 get_security_svc_https_transfer_cert_verification(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint8 prop_val = 0;

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dal_get_object_handle_nth(CLASS_NAME_PRODUCT, 0, &obj_handle);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:dal_get_object_handle_nth failed, ret=%d.", __FUNCTION__, ret));

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_PRODUCT_HTTPS_TRANSFER_CERT_VERIFY, &prop_val);
    return_val_do_info_if_fail(ret == VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get %s.%s failed, ret=%d.", __FUNCTION__, CLASS_NAME_PRODUCT,
        PROPERTY_PRODUCT_HTTPS_TRANSFER_CERT_VERIFY, ret));

    *o_result_jso = (prop_val == DISABLE) ? json_object_new_boolean(0) : json_object_new_boolean(1);
    return_val_do_info_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:json_object_new_int fail", __FUNCTION__));

    return HTTP_OK;
}


LOCAL void get_single_cert_info(OBJ_HANDLE obj_handle, json_object *cert_info_jso)
{
    gchar cert_crl_path[MAX_FILEPATH_LENGTH] = {0};
    gchar crl_start_time[CERT_ITEM_LEN_128] = {0};
    gchar crl_expire_time[CERT_ITEM_LEN_128] = {0};
    json_object* server_cert_jso = NULL;

    (void)rf_add_property_jso_byte(obj_handle, PROPERTY_HTTPS_SERVER_ROOTCERT_ID, PROPERTY_SECURITY_SERVICE_CERT_ID, 
        cert_info_jso);
    (void)rf_add_property_jso_string(obj_handle, PROPERTY_HTTPS_SERVER_ROOTCERT_USAGE, 
        PROPPETY_SECURITY_SERVICE_HTTPS_USAGE, cert_info_jso);
    
    
    server_cert_jso = json_object_new_object();
    if (server_cert_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__);
        return;
    }
    json_object_object_add(cert_info_jso, RPPROP_SERVER_CERT, server_cert_jso);

    (void)rf_add_property_jso_string(obj_handle, PROPERTY_HTTPS_SERVER_ROOTCERT_ISSUEBY, 
        PROPERTY_SECURITY_SERVICE_ISSUE_BY, server_cert_jso);
    (void)rf_add_property_jso_string(obj_handle, PROPERTY_HTTPS_SERVER_ROOTCERT_ISSUETO, 
        PROPERTY_SECURITY_SERVICE_ISSUE_TO, server_cert_jso);
    (void)rf_add_property_jso_string(obj_handle, PROPERTY_HTTPS_SERVER_ROOTCERT_VALIDFROM, 
        PROPERTY_SECURITY_SERVICE_VALID_FROM, server_cert_jso);
    (void)rf_add_property_jso_string(obj_handle, PROPERTY_HTTPS_SERVER_ROOTCERT_VALIDTO, 
        PROPERTY_SECURITY_SERVICE_VALID_TO, server_cert_jso);
    (void)rf_add_property_jso_string(obj_handle, PROPERTY_HTTPS_SERVER_ROOTCERT_SN, 
        PROPERTY_SECURITY_SERVICE_SERIAL_NUMBER, server_cert_jso);
    (void)rf_add_property_jso_string(obj_handle, PROPERTY_HTTPS_SERVER_ROOTCERT_SIGALGORITHM, 
        PROPERTY_SECURITY_SERVICE_SIG_ALGO, server_cert_jso);
    (void)rf_add_property_jso_string(obj_handle, PROPERTY_HTTPS_SERVER_ROOTCERT_KEYUSAGE, 
        PROPERTY_SECURITY_SERVICE_KEY_USAGE, server_cert_jso);
    (void)rf_add_property_jso_uint32(obj_handle, PROPERTY_HTTPS_SERVER_ROOTCERT_KEYLENGTH, 
        PROPERTY_SECURITY_SERVICE_KEY_LENGTH, server_cert_jso);
    (void)rf_add_property_jso_string(obj_handle, PROPERTY_HTTPS_SERVER_ROOTCERT_FINGERPRINT, 
        PROPERTY_HTTPS_SERVER_ROOTCERT_FINGERPRINT, server_cert_jso);

    (void)dal_get_property_value_string(obj_handle, PROPERTY_HTTPS_SERVER_ROOTCERT_CRL_FILEPATH, cert_crl_path, 
        sizeof(cert_crl_path));
    parse_single_value(TRUE, server_cert_jso, PROPERTY_SECURITY_SERVICE_IS_IMPORT_CRL, 
        (gchar)(g_strcmp0(cert_crl_path, "") != VOS_OK), NULL);

    (void)dal_get_property_value_string(obj_handle, PROPERTY_HTTPS_SERVER_ROOTCERT_CRL_STARTTIME, crl_start_time, 
        sizeof(crl_start_time));
    parse_single_value(g_strcmp0((const gchar*)crl_start_time, ""), server_cert_jso,
        PROPPETY_SECURITY_SERVICE_CRL_VALID_FROM, FALSE, (gchar*)crl_start_time);

    (void)dal_get_property_value_string(obj_handle, PROPERTY_HTTPS_SERVER_ROOTCERT_CRL_EXPIRETIME, crl_expire_time, 
        sizeof(crl_expire_time));
    parse_single_value(g_strcmp0((const gchar*)crl_expire_time, ""), server_cert_jso,
        PROPPETY_SECURITY_SERVICE_CRL_VALID_TO, FALSE, (gchar*)crl_expire_time);
    return;
}


LOCAL gint32 get_security_svc_https_cert_info(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso)
{
    GSList *obj_cert_list = NULL;
    GSList *obj_cert_node = NULL;
    json_object *cert_info_jso = NULL;
    OBJ_HANDLE obj_handle;
    gchar cert_sn[CERT_ITEM_LEN_64] = {0};

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    // 用户权限判断: 需满足只读权限，否则返回403，无权限访问
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    // 获取CA证书句柄列表
    gint32 ret = dfl_get_object_list(CLASS_HTTPS_SERVER_ROOTCERT, &obj_cert_list);
    if (ret != VOS_OK) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: get object list fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_array();
    if (*o_result_jso == NULL) {
        g_slist_free(obj_cert_list);
        debug_log(DLOG_ERROR, "%s json_object_new_array fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    // 循环获取证书信息
    for (obj_cert_node = obj_cert_list; obj_cert_node != NULL; obj_cert_node = obj_cert_node->next) {
        obj_handle = (OBJ_HANDLE)obj_cert_node->data;

        // 获取序列号，确认证书是否已导入
        (void)memset_s(cert_sn, sizeof(cert_sn), 0, sizeof(cert_sn));
        (void)dal_get_property_value_string(obj_handle, PROPERTY_HTTPS_SERVER_ROOTCERT_SN, cert_sn, sizeof(cert_sn));
        if (strlen(cert_sn) == 0) {
            continue;
        }

        // 创建新的对象
        cert_info_jso = json_object_new_object();
        if (cert_info_jso == NULL) {
            debug_log(DLOG_ERROR, "%s: json_object_new_object fail", __FUNCTION__);
            continue;
        }
        // 获取HTTPS传输服务器根证书及吊销列表属性值，并添加到对象中
        get_single_cert_info(obj_handle, cert_info_jso);
        json_object_array_add(*o_result_jso, cert_info_jso);
        cert_info_jso = NULL;
    }

    g_slist_free(obj_cert_list);
    return HTTP_OK;
}


LOCAL gint32 set_security_svc_https_transfer_cert_verification(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint8 prop_val;
    GSList *input_list = NULL;
    gchar slot_id[MAX_RSC_ID_LEN] = {0};
    guint8 old_val = 0;
    OBJ_HANDLE obj_handle = 0;

    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    gint32 ret = dal_get_object_handle_nth(CLASS_NAME_PRODUCT, 0, &obj_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: get object handle failed, ret:%d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_PRODUCT_HTTPS_TRANSFER_CERT_VERIFY, &old_val);
    prop_val = json_object_get_boolean(i_paras->val_jso) ? ENABLE : DISABLE;

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(prop_val));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle, 
        CLASS_NAME_PRODUCT, METHOD_PRODUCT_SET_HTTPS_CERT_VERIFY, AUTH_ENABLE, 
        i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
            
            if (prop_val != old_val) {
                (void)redfish_get_board_slot(slot_id, sizeof(slot_id));
                if (dal_rf_rsc_evt_log(RF_RSC_UPDATED_EVT, TRUE, MANAGER_SECURITY_SVC_URI, 
                    (const gchar*)slot_id) != VOS_OK) {
                    debug_log(DLOG_ERROR, "%s: send redfish event failed, ret:%d", __FUNCTION__, ret);
                }
            }
            return HTTP_OK;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, PROPERTY_PRODUCT_HTTPS_TRANSFER_CERT_VERIFY,
                o_message_jso, PROPERTY_PRODUCT_HTTPS_TRANSFER_CERT_VERIFY);
            return HTTP_BAD_REQUEST;

        default:
            debug_log(DLOG_ERROR, "%s: call method %s failed, ret = %d", __FUNCTION__,
                PROPERTY_PRODUCT_HTTPS_TRANSFER_CERT_VERIFY, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}

LOCAL gint32 add_link_certupdate_service(const gchar *slot, json_object *link_jso)
{
    gint32 ret;
    gchar change_uri[MAX_URI_LENGTH] = {0};
    json_object *odata_id_jso = NULL;

    ret = snprintf_s(change_uri, sizeof(change_uri), sizeof(change_uri) - 1, MANAGER_CERTUPDATE_SVC_URI, slot);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    odata_id_jso = json_object_new_object();
    if (odata_id_jso == NULL) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    json_object_object_add(odata_id_jso, RFPROP_ODATA_ID, json_object_new_string((const gchar*)change_uri));
    json_object_object_add(link_jso, RFPROP_CERTUPDATE_SVC, odata_id_jso);

    return RET_OK;
}

LOCAL gint32 add_link_tpcm_service(const gchar *slot_id, json_object *link_jso)
{
#ifdef ARM64_HI1711_ENABLED
    
    OBJ_HANDLE obj_handle = 0;
    _cleanup_gvariant_slist_full_ GSList* output_list = NULL;
    gint32 ret = dfl_get_object_handle(TPCM_CLASS_BASEINFO, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "get %s handle failed ret = %d", TPCM_CLASS_BASEINFO, ret);
        return RET_OK;
    }
    ret = dfl_call_class_method(obj_handle, METHOD_TPCM_GETTPCMSPPORTSTATE, NULL, NULL, &output_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "call method failed ret = %d", ret);
        return RET_OK;
    }
    guint8 tcm_state = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 0));
    if (tcm_state != 1) {
        debug_log(DLOG_ERROR, "tpcm check faild, tcm_state = %u", tcm_state);
        return RET_OK;
    }
    gchar change_uri[MAX_URI_LENGTH] = {0};
    gint32 iRet = snprintf_s(change_uri, sizeof(change_uri), sizeof(change_uri) - 1, MANAGER_TPCM_URI, slot_id);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "snprintf_s fail, ret = %d", iRet);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    json_object *odata_id_jso = json_object_new_object();
    if (odata_id_jso == NULL) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    json_object_object_add(link_jso, "TpcmService", odata_id_jso);
    json_object_object_add(odata_id_jso, RFPROP_ODATA_ID, json_object_new_string((const gchar*)change_uri));
#endif
    return RET_OK;
}


LOCAL gint32 get_security_svc_links(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 iRet;
    gchar change_uri[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};
    json_object *odata_id_jso = NULL;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    gint32 ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    iRet = snprintf_s(change_uri, sizeof(change_uri), sizeof(change_uri) - 1, MANAGER_HTTPS_CERT_URI, slot_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    json_object *link_jso = json_object_new_object();
    do_if_expr(link_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    odata_id_jso = json_object_new_object();
    do_if_expr(odata_id_jso == NULL, (void)json_object_put(link_jso); return HTTP_INTERNAL_SERVER_ERROR);

    json_object_object_add(link_jso, RFPROP_SECURITY_SVC_HTTPS_CERT, odata_id_jso);
    json_object_object_add(odata_id_jso, RFPROP_ODATA_ID, json_object_new_string((const gchar*)change_uri));

    *o_result_jso = link_jso;


#ifdef ARM64_HI1711_ENABLED // 增加DICE 链接
    iRet = snprintf_s(change_uri, sizeof(change_uri), sizeof(change_uri) - 1, MANAGER_DICE_CERT_URI, slot_id);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    odata_id_jso = json_object_new_object();
    if (odata_id_jso == NULL) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    json_object_object_add(link_jso, "DiceCert", odata_id_jso);
    json_object_object_add(odata_id_jso, RFPROP_ODATA_ID, json_object_new_string((const gchar*)change_uri));
#endif
    

    ret = add_link_certupdate_service(slot_id, link_jso);
    if (ret != RET_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    // 增加TPCM 链接
    ret = add_link_tpcm_service(slot_id, link_jso);
    if (ret != RET_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 fill_algorithm_jso(json_object **result_jso, const gchar *property_name, const gchar **rf_list,
    gsize rf_list_len)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gboolean enable_status = FALSE;
    gchar **algorithm_vector = NULL;
    gsize vector_len = 0;
    gsize i;
    gsize j = 0;
    gchar algorithm_str[MAX_ALGORITHM_STR_LENGTH] = {0};
    gchar algorithm_rf_prop[MAX_ALGORITHM_STR_LENGTH] = {0};
    json_object *tmp_jso = NULL;
    json_bool enable_list[MAX_ALGORITHM_NUM] = {FALSE};

    return_val_do_info_if_expr(NULL == result_jso || NULL == property_name || NULL == rf_list, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: invalid input.", __FUNCTION__));

    ret = dal_get_object_handle_nth(CLASS_SECURITY_ENHANCE, 0, &obj_handle);
    return_val_if_expr(DFL_OK != ret, ret);

    ret = dal_get_property_value_strv(obj_handle, property_name, &algorithm_vector, &vector_len);
    return_val_if_expr(VOS_OK != ret, ret);

    tmp_jso = json_object_new_object();
    return_val_do_info_if_expr(NULL == tmp_jso, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed.", __FUNCTION__);
        g_strfreev(algorithm_vector));

    for (i = 0; i < vector_len; i++) {
        debug_log(DLOG_DEBUG, "%s: algorithm_vector[%" G_GSIZE_FORMAT "] =  %s", __FUNCTION__, i, algorithm_vector[i]);

        
        (void)memset_s(algorithm_str, sizeof(algorithm_str), 0, sizeof(algorithm_str));

        return_val_do_info_if_expr(VOS_OK !=
            dal_split_algorithm_prop(algorithm_vector[i], algorithm_str, sizeof(algorithm_str), &enable_status),
            VOS_ERR, (void)json_object_put(tmp_jso);
            g_strfreev(algorithm_vector));

        
        (void)memset_s(algorithm_rf_prop, sizeof(algorithm_rf_prop), 0, sizeof(algorithm_rf_prop));

        ret = snprintf_s(algorithm_rf_prop, sizeof(algorithm_rf_prop), sizeof(algorithm_rf_prop) - 1, "%sEnabled",
            algorithm_str);
        return_val_do_info_if_expr(ret <= 0, VOS_ERR, (void)json_object_put(tmp_jso); g_strfreev(algorithm_vector));

        
        for (j = 0; j < rf_list_len; j++) {
            if (0 == g_strcmp0(rf_list[j], algorithm_rf_prop)) {
                enable_list[j] = enable_status;
                break;
            }
        }
    }

    
    for (i = 0; i < rf_list_len; i++) {
        json_object_object_add(tmp_jso, rf_list[i], json_object_new_boolean(enable_list[i]));
    }

    *result_jso = tmp_jso;
    g_strfreev(algorithm_vector);
    return VOS_OK;
}


LOCAL gint32 get_security_svc_ssh_ciphers(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = fill_algorithm_jso(o_result_jso, PROPERTY_SECURITY_SSH_CIPHERS, g_ssh_ciphers_rf,
        GET_ARRAY_ITEMS(g_ssh_ciphers_rf));
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get prop(%s) failed.", __FUNCTION__, PROPERTY_SECURITY_SSH_CIPHERS));

    return HTTP_OK;
}


LOCAL gint32 get_security_svc_ssh_kexs(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = fill_algorithm_jso(o_result_jso, PROPERTY_SECURITY_SSH_KEXS, g_ssh_kexs_rf, GET_ARRAY_ITEMS(g_ssh_kexs_rf));
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get prop(%s) failed.", __FUNCTION__, PROPERTY_SECURITY_SSH_KEXS));

    return HTTP_OK;
}


LOCAL gint32 get_security_svc_ssh_MACs(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = fill_algorithm_jso(o_result_jso, PROPERTY_SECURITY_SSH_MACS, g_ssh_macs_rf, GET_ARRAY_ITEMS(g_ssh_macs_rf));
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get prop(%s) failed.", __FUNCTION__, PROPERTY_SECURITY_SSH_MACS));

    return HTTP_OK;
}


LOCAL gint32 get_security_svc_ssh_host_keys(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = fill_algorithm_jso(o_result_jso, PROPERTY_SECURITY_SSH_HOST_KEYS, g_ssh_host_keys_rf,
        GET_ARRAY_ITEMS(g_ssh_host_keys_rf));
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get prop(%s) failed.", __FUNCTION__, PROPERTY_SECURITY_SSH_HOST_KEYS));

    return HTTP_OK;
}


LOCAL gint32 get_security_svc_ssl_cipher_suites(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = fill_algorithm_jso(o_result_jso, PROPERTY_SECURITY_SSL_CIPHER_SUITES, g_ssl_cipher_suites_rf,
        GET_ARRAY_ITEMS(g_ssl_cipher_suites_rf));
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get prop(%s) failed.", __FUNCTION__, PROPERTY_SECURITY_SSL_CIPHER_SUITES));

    return HTTP_OK;
}


LOCAL gint32 add_rmcp_cipher_suite_jso(guint32 id, json_bool enable_status, json_object *result_jso)
{
    json_object *cipher_suite = NULL;
    guint32 i;
    const gchar *auth_algorithm = NULL;
    const gchar *integrity_algorithm = NULL;
    const gchar *crypt_algorithm = NULL;
    const gchar *rf_prop = NULL;

    return_val_do_info_if_expr(NULL == result_jso, VOS_ERR, debug_log(DLOG_ERROR, "%s: invalid input.", __FUNCTION__));

    for (i = 0; i < GET_ARRAY_ITEMS(g_rmcp_cipher_suites); i++) {
        if (g_rmcp_cipher_suites[i].id == id) {
            auth_algorithm = g_rmcp_cipher_suites[i].auth_algorithm;
            integrity_algorithm = g_rmcp_cipher_suites[i].integrity_algorithm;
            crypt_algorithm = g_rmcp_cipher_suites[i].crypt_algorithm;
            rf_prop = g_rmcp_cipher_suites[i].rf_prop;
            break;
        }
    }

    return_val_do_info_if_expr(i == GET_ARRAY_ITEMS(g_rmcp_cipher_suites), VOS_ERR,
        debug_log(DLOG_ERROR, "%s: invalid id = %d.", __FUNCTION__, id));

    cipher_suite = json_object_new_object();
    return_val_do_info_if_expr(NULL == cipher_suite, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed.", __FUNCTION__));

    
    json_object_object_add(cipher_suite, ENABLED_STRING, json_object_new_boolean(enable_status));

    
    json_object_object_add(cipher_suite, RFPROP_AUTHENTICATION_ALGORITHM, json_object_new_string(auth_algorithm));
    json_object_object_add(cipher_suite, RFPROP_INTEGRITY_ALGORITHM, json_object_new_string(integrity_algorithm));
    json_object_object_add(cipher_suite, RFPROP_CONFIDENTIALITY_ALGORITHM, json_object_new_string(crypt_algorithm));

    json_object_object_add(result_jso, rf_prop, cipher_suite);
    return VOS_OK;
}


LOCAL gint32 fill_rmcp_cipher_suites_jso(json_object **result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint32 enable_flag = 0;
    json_object *tmp_jso = NULL;

    return_val_do_info_if_expr(NULL == result_jso, VOS_ERR, debug_log(DLOG_ERROR, "%s: invalid input.", __FUNCTION__));

    ret = dal_get_object_handle_nth(CLASS_SECURITY_ENHANCE, 0, &obj_handle);
    return_val_if_expr(DFL_OK != ret, ret);

    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_SECURITY_RMCP_CIPHER_SUITES, &enable_flag);
    return_val_if_expr(DFL_OK != ret, ret);

    tmp_jso = json_object_new_object();
    return_val_do_info_if_expr(NULL == tmp_jso, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed.", __FUNCTION__));

    ret = add_rmcp_cipher_suite_jso(1, 0 == (enable_flag & BIT(1)) ? FALSE : TRUE, tmp_jso);
    return_val_do_info_if_expr(VOS_OK != ret, ret, (void)json_object_put(tmp_jso));

    ret = add_rmcp_cipher_suite_jso(2, 0 == (enable_flag & BIT(2)) ? FALSE : TRUE, tmp_jso);
    return_val_do_info_if_expr(VOS_OK != ret, ret, (void)json_object_put(tmp_jso));

    ret = add_rmcp_cipher_suite_jso(3, 0 == (enable_flag & BIT(3)) ? FALSE : TRUE, tmp_jso);
    return_val_do_info_if_expr(VOS_OK != ret, ret, (void)json_object_put(tmp_jso));

    ret = add_rmcp_cipher_suite_jso(17, 0 == (enable_flag & BIT(17)) ? FALSE : TRUE, tmp_jso);
    return_val_do_info_if_expr(VOS_OK != ret, ret, (void)json_object_put(tmp_jso));

    *result_jso = tmp_jso;
    return VOS_OK;
}


LOCAL gint32 get_security_svc_rmcp_cipher_suites(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = fill_rmcp_cipher_suites_jso(o_result_jso);
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get RMCP cipher suites failed.", __FUNCTION__));

    return HTTP_OK;
}


LOCAL gint32 generate_algorithm_strv(json_object *origin_jso, const gchar **rf_list, gsize rf_list_len,
    gchar **result_strv, gsize result_max_len, gssize *result_len)
{
    gint32 ret = 0;
    json_bool enable_status = FALSE;
    gsize i;
    gssize total_num = 0;

    return_val_do_info_if_expr(NULL == origin_jso || NULL == rf_list || NULL == result_strv || NULL == result_len,
        VOS_ERR, debug_log(DLOG_ERROR, "%s: invalid input.", __FUNCTION__));

    return_val_do_info_if_expr(rf_list_len > result_max_len, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: vector len(%" G_GSIZE_FORMAT ") is too long.", __FUNCTION__, rf_list_len));

    for (i = 0; i < rf_list_len; i++) {
        
        continue_if_expr(TRUE != get_element_boolean(origin_jso, rf_list[i], &enable_status));

        if (TRUE == enable_status) {
            ret = snprintf_s(result_strv[total_num], MAX_ALGORITHM_STR_LENGTH, MAX_ALGORITHM_STR_LENGTH - 1, "%s on",
                rf_list[i]);
        } else {
            ret = snprintf_s(result_strv[total_num], MAX_ALGORITHM_STR_LENGTH, MAX_ALGORITHM_STR_LENGTH - 1, "%s off",
                rf_list[i]);
        }

        continue_do_info_if_expr(ret <= 0, debug_log(DLOG_ERROR, "%s %d: snprintf_s failed.", __FUNCTION__, __LINE__));

        debug_log(DLOG_DEBUG, "%s: result_strv[%" G_GSSIZE_FORMAT "] = %s.", __FUNCTION__, total_num,
            result_strv[total_num]);

        total_num++;
    }

    
    return_val_do_info_if_expr(0 == total_num, VOS_ERR_NOTSUPPORT,
        debug_log(DLOG_ERROR, "%s: no valid prop value.", __FUNCTION__));

    *result_len = total_num;

    return VOS_OK;
}


LOCAL gint32 call_security_svc_method(PROVIDER_PARAS_S *i_paras, const gchar **rf_list, gsize rf_list_len,
    const gchar *method_name)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    gchar* strv[MAX_ALGORITHM_NUM] = {NULL};
    gssize strv_len = 0;
    gchar *str_mem = NULL;
    gsize i;

    return_val_do_info_if_expr(NULL == i_paras || NULL == rf_list || NULL == method_name ||
        rf_list_len > MAX_ALGORITHM_NUM,
        VOS_ERR, debug_log(DLOG_ERROR, "%s: invalid input.", __FUNCTION__));

    ret = dal_get_object_handle_nth(CLASS_SECURITY_ENHANCE, 0, &obj_handle);
    return_val_if_expr(VOS_OK != ret, VOS_ERR);

    
    str_mem = (gchar *)g_malloc0(rf_list_len * MAX_ALGORITHM_STR_LENGTH);
    return_val_if_expr(NULL == str_mem, VOS_ERR);

    
    for (i = 0; i < rf_list_len; i++) {
        strv[i] = str_mem + MAX_ALGORITHM_STR_LENGTH * i;
    }

    ret = generate_algorithm_strv(i_paras->val_jso, rf_list, rf_list_len, strv, GET_ARRAY_ITEMS(strv), &strv_len);
    return_val_do_info_if_expr(VOS_OK != ret, ret, g_free(str_mem));

    input_list = g_slist_append(input_list, g_variant_new_strv((const gchar * const *)strv, strv_len));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SECURITY_ENHANCE, method_name, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);

    
    g_free(str_mem);
    uip_free_gvariant_list(input_list);

    
    return_val_do_info_if_expr(VOS_ERR_OPFAIL == ret, ret,
        debug_log(DLOG_ERROR, "%s: method(%s) try to disable all algorithms or cipher suites.", __FUNCTION__,
        method_name));

    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: method(%s) failed.", __FUNCTION__, method_name));

    return VOS_OK;
}


LOCAL gint32 generate_response_for_set_algorithms(gint32 ret, json_object **o_message_jso, gboolean is_algorithm)
{
    if (NULL == o_message_jso) {
        
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    switch (ret) {
        case VOS_OK:
            return HTTP_OK;

        
        case VOS_ERR_NOTSUPPORT: 
            
            return HTTP_BAD_REQUEST;

        
        case VOS_ERR_OPFAIL: 
            if (TRUE == is_algorithm) {
                (void)create_message_info(MSGID_ALL_ALGORITHMS_DISABLED, NULL, o_message_jso);
            } else {
                (void)create_message_info(MSGID_ALL_CIPHER_SUITES_DISABLED, NULL, o_message_jso);
            }
            return HTTP_BAD_REQUEST;

        
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 set_seucrity_svc_ssh_ciphers(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    
    return_val_do_info_if_expr(VOS_OK != provider_paras_check(i_paras), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s: invalid i_paras.", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    
    return_val_do_info_if_expr(0 == (i_paras->user_role_privilege & USERROLE_SECURITYMGNT), HTTP_FORBIDDEN,
        debug_log(DLOG_INFO, "%s: user dont have security privilege", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_SSH_CIPHERS, o_message_jso, RFPROP_SSH_CIPHERS));

    ret = call_security_svc_method(i_paras, g_ssh_ciphers_rf, GET_ARRAY_ITEMS(g_ssh_ciphers_rf),
        METHOD_SECURITY_ENHANCE_SET_SSH_CIPHER);

    return generate_response_for_set_algorithms(ret, o_message_jso, TRUE);
}


LOCAL gint32 set_seucrity_svc_ssh_kexs(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    
    return_val_do_info_if_expr(VOS_OK != provider_paras_check(i_paras), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s: invalid i_paras.", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    
    return_val_do_info_if_expr(0 == (i_paras->user_role_privilege & USERROLE_SECURITYMGNT), HTTP_FORBIDDEN,
        debug_log(DLOG_INFO, "%s: user dont have security privilege", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_SSH_KEXS, o_message_jso, RFPROP_SSH_KEXS));

    ret = call_security_svc_method(i_paras, g_ssh_kexs_rf, GET_ARRAY_ITEMS(g_ssh_kexs_rf),
        METHOD_SECURITY_ENHANCE_SET_SSH_KEX);

    return generate_response_for_set_algorithms(ret, o_message_jso, TRUE);
}


LOCAL gint32 set_seucrity_svc_ssh_MACs(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    
    return_val_do_info_if_expr(VOS_OK != provider_paras_check(i_paras), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s: invalid i_paras.", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    
    return_val_do_info_if_expr(0 == (i_paras->user_role_privilege & USERROLE_SECURITYMGNT), HTTP_FORBIDDEN,
        debug_log(DLOG_INFO, "%s: user dont have security privilege", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_SSH_MACS, o_message_jso, RFPROP_SSH_MACS));

    ret = call_security_svc_method(i_paras, g_ssh_macs_rf, GET_ARRAY_ITEMS(g_ssh_macs_rf),
        METHOD_SECURITY_ENHANCE_SET_SSH_MAC);

    return generate_response_for_set_algorithms(ret, o_message_jso, TRUE);
}


LOCAL gint32 set_seucrity_svc_ssh_host_keys(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    
    return_val_do_info_if_expr(VOS_OK != provider_paras_check(i_paras), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s: invalid i_paras.", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    
    return_val_do_info_if_expr(0 == (i_paras->user_role_privilege & USERROLE_SECURITYMGNT), HTTP_FORBIDDEN,
        debug_log(DLOG_INFO, "%s: user dont have security privilege", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_SSH_HOST_KEYS, o_message_jso,
        RFPROP_SSH_HOST_KEYS));

    ret = call_security_svc_method(i_paras, g_ssh_host_keys_rf, GET_ARRAY_ITEMS(g_ssh_host_keys_rf),
        METHOD_SECURITY_ENHANCE_SET_SSH_HOST_KEY);

    return generate_response_for_set_algorithms(ret, o_message_jso, TRUE);
}


LOCAL gint32 set_seucrity_svc_ssl_cipher_suites(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    
    return_val_do_info_if_expr(VOS_OK != provider_paras_check(i_paras), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s: invalid i_paras.", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    
    return_val_do_info_if_expr(0 == (i_paras->user_role_privilege & USERROLE_SECURITYMGNT), HTTP_FORBIDDEN,
        debug_log(DLOG_INFO, "%s: user dont have security privilege", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_SSL_CIPHER_SUITES, o_message_jso,
        RFPROP_SSL_CIPHER_SUITES));

    ret = call_security_svc_method(i_paras, g_ssl_cipher_suites_rf, GET_ARRAY_ITEMS(g_ssl_cipher_suites_rf),
        METHOD_SECURITY_ENHANCE_SET_SSL_CIPHER_SUITE);

    return generate_response_for_set_algorithms(ret, o_message_jso, FALSE);
}


LOCAL gint32 generate_rmcp_cipher_suites_prop(json_object *val_jso, guint32 *enable_flag)
{
    json_object *cipher_suite = NULL;
    json_bool ret_bool = FALSE;
    gsize i;
    json_bool enable_status = FALSE;
    gsize total_num = 0;

    return_val_do_info_if_expr(NULL == val_jso || NULL == enable_flag, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: invalid input.", __FUNCTION__));

    for (i = 0; i < GET_ARRAY_ITEMS(g_rmcp_cipher_suites); i++) {
        cipher_suite = NULL;
        ret_bool = json_object_object_get_ex(val_jso, g_rmcp_cipher_suites[i].rf_prop, &cipher_suite);
        continue_if_fail(ret_bool && (NULL != cipher_suite));

        continue_if_expr(FALSE == get_element_boolean(cipher_suite, ENABLED_STRING, &enable_status));

        total_num++;

        if (TRUE == enable_status) {
            *enable_flag |= BIT(g_rmcp_cipher_suites[i].id);
        } else {
            *enable_flag &= (G_MAXUINT32 - BIT(g_rmcp_cipher_suites[i].id));
        }
    }

    
    return_val_do_info_if_expr(0 == total_num, VOS_ERR_NOTSUPPORT,
        debug_log(DLOG_ERROR, "%s: no valid prop value.", __FUNCTION__));

    return VOS_OK;
}


LOCAL gint32 set_seucrity_svc_rmcp_cipher_suites(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint32 enable_flag = 0;
    GSList *input_list = NULL;

    
    return_val_do_info_if_expr(VOS_OK != provider_paras_check(i_paras), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: invalid i_paras.", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    
    return_val_do_info_if_expr(0 == (i_paras->user_role_privilege & USERROLE_SECURITYMGNT), HTTP_FORBIDDEN,
        debug_log(DLOG_INFO, "%s: user dont have security privilege", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_RMCP_CIPHER_SUITES, o_message_jso,
        RFPROP_RMCP_CIPHER_SUITES));

    ret = dal_get_object_handle_nth(CLASS_SECURITY_ENHANCE, 0, &obj_handle);
    goto_if_expr(VOS_OK != ret, exit);

    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_SECURITY_RMCP_CIPHER_SUITES, &enable_flag);
    goto_if_expr(VOS_OK != ret, exit);

    ret = generate_rmcp_cipher_suites_prop(i_paras->val_jso, &enable_flag);
    goto_if_expr(VOS_OK != ret, exit);

    input_list = g_slist_append(input_list, g_variant_new_uint32(enable_flag));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SECURITY_ENHANCE, METHOD_SECURITY_ENHANCE_SET_RMCP_CIPHER_SUITE, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, NULL);

    uip_free_gvariant_list(input_list);

exit:
    debug_log(DLOG_INFO, "%s: set seucrity svc rmcp cipher suites failed", __FUNCTION__);
    return generate_response_for_set_algorithms(ret, o_message_jso, FALSE);
}


LOCAL gint32 get_security_actions(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    guint32 i;
    guint32 len;
    gint32 ret;
    gchar slot_id[MAX_RSC_ID_LEN] = {0};
    gchar rsc_uri[MAX_URI_LENGTH] = {0};
    json_object *action_jso = NULL;
    guchar MgmtSoftWareType = 0;
    (void)dal_get_software_type(&MgmtSoftWareType);
    if (MgmtSoftWareType == MGMT_SOFTWARE_TYPE_EM) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: redfish_get_board_slot error. ret is %d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = snprintf_s(rsc_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, MANAGER_SECURITY_SVC_URI, slot_id);
    return_val_do_info_if_fail(ret > 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: format string failed, ret is %d", __FUNCTION__, ret));

    action_jso = json_object_new_object();
    if (action_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: alloc new object failed", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    len = G_N_ELEMENTS(g_security_svc_action_array);
    for (i = 0; i < len; i++) {
        rf_add_action_prop(action_jso, (const gchar*)rsc_uri, g_security_svc_action_array[i]);
    }

    *o_result_jso = action_jso;
    return HTTP_OK;
}


LOCAL gint32 update_master_key_precheck(guint32 priv, json_object **o_message_jso)
{
    gint32 ret;
    gint32 available_task_id;
    OBJ_HANDLE obj_handle = 0;
    GSList *output_list = NULL;
    guchar update_status;

    if ((USERROLE_SECURITYMGNT & priv) == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = dal_get_object_handle_nth(CLASS_SECURITY_ENHANCE, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: get object handle for %s failed", __FUNCTION__, CLASS_SECURITY_ENHANCE));

    ret = dfl_call_class_method(obj_handle, METHOD_SECURITY_ENHANCE_GETUPDATESTATUS, NULL, NULL, &output_list);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: get master key status failed, ret is %d", __FUNCTION__, ret));

    update_status = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 0));
    uip_free_gvariant_list(output_list);
    if (update_status == MK_UPDATING) {
        (void)create_message_info(MSGID_MASTERKEY_UPDATINGLIMIT, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    } else if (update_status == MK_UPDATED) {
        (void)create_message_info(MSGID_MASTERKEY_UPDATENOTALLOWED, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }

    
    available_task_id = find_available_task_id();
    return_val_do_info_if_fail(available_task_id != RF_INVALID_TASK_ID, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_TASK_LIMIT_EXCEED, NULL, o_message_jso));

    return RF_OK;
}


LOCAL gint32 update_master_key_status_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GSList *output_list = NULL;
    guchar update_status;
    guchar update_progress;

    return_val_do_info_if_fail((origin_obj_path != NULL) && (body_jso != NULL) && (monitor_fn_data != NULL) &&
        (message_obj != NULL),
        RF_FAILED, debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__));

    ret = dal_get_object_handle_nth(CLASS_SECURITY_ENHANCE, 0, &obj_handle);
    goto_label_do_info_if_fail(ret == VOS_OK, exit,
        debug_log(DLOG_ERROR, "%s: get object handle for %s failed", __FUNCTION__, SSL_OBJECT_NAME));

    ret = dfl_call_class_method(obj_handle, METHOD_SECURITY_ENHANCE_GETUPDATESTATUS, NULL, NULL, &output_list);
    goto_label_do_info_if_fail(ret == VOS_OK, exit,
        debug_log(DLOG_ERROR, "%s: get update status failed, ret is %d", __FUNCTION__, ret));

    update_status = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 0));
    update_progress = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 1));
    uip_free_gvariant_list(output_list);
    switch (update_status) {
        case MK_UPDATING:
            monitor_fn_data->task_state = RF_TASK_RUNNING;
            monitor_fn_data->task_progress = update_progress;
            break;

        case MK_UPDATE_FAILED:
            monitor_fn_data->task_state = RF_TASK_EXCEPTION;
            monitor_fn_data->task_progress = update_progress;
            (void)create_message_info(MSGID_MONITOR_TASK_FAIL, NULL, message_obj);
            break;

        case MK_UPDATE_READY_OR_OK:
        default:
            monitor_fn_data->task_state = RF_TASK_COMPLETED;
            monitor_fn_data->task_progress = RF_FINISH_PERCENTAGE;
            break;
    }

    return RF_OK;

exit:
    monitor_fn_data->task_state = RF_TASK_EXCEPTION;
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);
    return RF_OK;
}


LOCAL void *update_master_key_thread_func(void *data)
{
    gint32 ret;
    KEY_UPDATE_INFO_S *key_update_info = (KEY_UPDATE_INFO_S *)data;

    (void)prctl(PR_SET_NAME, (uintptr_t) "rfUpdateMasterKey");
    debug_log(DLOG_DEBUG, "%s: update master key begin", __FUNCTION__);
    ret = uip_call_class_method_redfish(key_update_info->req_from_webui_flag, key_update_info->user_name,
        key_update_info->session_ip, 0, CLASS_SECURITY_ENHANCE, METHOD_SECURITY_ENHANCE_UPDATEMASTERKEY, AUTH_ENABLE,
        key_update_info->user_role_priv, NULL, NULL);
    g_free(key_update_info);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: call method %s failed, ret is %d", __FUNCTION__,
            METHOD_SECURITY_ENHANCE_UPDATEMASTERKEY, ret);
    }
    debug_log(DLOG_DEBUG, "%s: update master key successfully", __FUNCTION__);
    return NULL;
}


LOCAL gint32 act_security_svc_update_masterkey(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    errno_t safe_fun_ret;
    gulong thread_id = 0;
    TASK_MONITOR_INFO_S *task_monitor_info = NULL;
    KEY_UPDATE_INFO_S *key_update_info = NULL;
    guchar MgmtSoftWareType = 0;
    (void)dal_get_software_type(&MgmtSoftWareType);
    if (MgmtSoftWareType == MGMT_SOFTWARE_TYPE_EM) {
        (void)create_message_info(MSGID_ACT_NOT_SUPPORTED, RFACTION_UPDATE_MASTERKEY, o_message_jso,
            RFACTION_UPDATE_MASTERKEY);
        return HTTP_BAD_REQUEST;
    }

    
    ret = update_master_key_precheck(i_paras->user_role_privilege, o_message_jso);
    return_val_do_info_if_fail(RF_OK == ret, ret,
        debug_log(DLOG_ERROR, "%s: update master key precheck failed", __FUNCTION__));

    key_update_info = (KEY_UPDATE_INFO_S *)g_malloc0(sizeof(KEY_UPDATE_INFO_S));
    return_val_do_info_if_expr(key_update_info == NULL, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: malloc a new memory for KEY_UPDATE_INFO_S failed", __FUNCTION__));

    key_update_info->req_from_webui_flag = i_paras->is_from_webui;
    key_update_info->user_role_priv = i_paras->user_role_privilege;
    safe_fun_ret = strncpy_s(key_update_info->user_name, SESSION_IP_LEN, i_paras->user_name, SESSION_IP_LEN - 1);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret));
    safe_fun_ret = strncpy_s(key_update_info->session_ip, SESSION_IP_LEN, i_paras->client, SESSION_IP_LEN - 1);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret));

    
    ret = vos_task_create(&thread_id, "update master key thread", (TASK_ENTRY)update_master_key_thread_func,
        key_update_info, DEFAULT_PRIORITY);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR, g_free(key_update_info);
        debug_log(DLOG_ERROR, "%s: create master key update thread failed", __FUNCTION__));

    task_monitor_info = task_monitor_info_new(NULL);
    if (task_monitor_info == NULL) {
        debug_log(DLOG_ERROR, "%s: task_monitor_info_new failed", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    task_monitor_info->task_progress = TASK_NO_PROGRESS; 
    task_monitor_info->rsc_privilege = USERROLE_SECURITYMGNT;
    ret = create_new_task("Update Master Key Task", update_master_key_status_monitor, task_monitor_info,
        i_paras->val_jso, i_paras->uri, o_result_jso);

    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR, task_monitor_info_free(task_monitor_info);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: create task for update master key failed", __FUNCTION__));

    return HTTP_ACCEPTED;
}


LOCAL gint32 svc_crl_import_status_monitor(const gchar *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    gint32 ret;

    ret = import_cert_common_status_monitor(origin_obj_path, body_jso, process_import_local_https_crl, monitor_fn_data,
        message_obj);
    return ret;
}


LOCAL gint32 process_import_local_https_crl(const gchar *file_path, json_object *user_data,
    json_object **o_message_jso)
{
    gint32 ret;
    const gchar *user_name = NULL;
    const gchar *client = NULL;
    gint32 user_priv = 0;
    gint32 from_webui_flag = REDFISH_REQ_FROM_OTHER_CLIENT;
    GSList *input_list = NULL;
    gint32 root_cert_id = 0;
    gint32 http_code = HTTP_OK;
    gint32 cert_usage = 0;

    
    if (strlen(file_path) >= MAX_FILEPATH_LENGTH) {
        debug_log(DLOG_ERROR, "%s: file path length is over %d", __FUNCTION__, MAX_FILEPATH_LENGTH);
        ret = IMPORT_CERT_INVALID_FILEPATH_ERR;
        goto exit;
    }

    
    if (vos_get_file_length(file_path) > MAX_CRL_FILE_LEN) {
        debug_log(DLOG_ERROR, "%s: file is large than %d", __FUNCTION__, MAX_FILEPATH_LENGTH);
        ret =  IMPORT_CERT_FILE_LEN_EXCEED_ERR;
        goto exit;
    }

    
    import_cert_get_common_userdata_fn(&user_name, &client, &user_priv, &from_webui_flag, user_data);
    (void)get_element_int(user_data, RFACTION_PARAM_ROOT_CERT_ID, &root_cert_id);
    (void)get_element_int(user_data, RFACTION_PARAM_ROOT_CERT_USAGE, &cert_usage);

    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)root_cert_id));
    input_list = g_slist_append(input_list, g_variant_new_string(file_path));
    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)cert_usage));

    ret = uip_call_class_method_redfish((guint8)from_webui_flag, user_name, client, 0, CLASS_HTTPS_SERVER_ROOTCERT,
        HTTPS_SERVER_ROOTCERT_METHOD_IMPORT_CRL, AUTH_DISABLE, user_priv, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: call remote method(%s) fail, ret :%d", __FUNCTION__,
            HTTPS_SERVER_ROOTCERT_METHOD_IMPORT_CRL, ret);
    }

    parse_import_crl_common_ret_code(ret, o_message_jso, &http_code);
    ret = http_code;
exit:
    if (ret != HTTP_OK) {
        (void)proxy_delete_file(0, NULL, NULL, file_path);
    }

    return ret;
}


LOCAL gint32 process_import_remote_https_crl(PROVIDER_PARAS_S *i_paras, const gchar *uri, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    TASK_MONITOR_INFO_S *import_crl_data = NULL;
    json_object *body_jso = NULL;

    ret = rf_start_file_download(i_paras, uri, IMPORT_SECURITY_SERVICE_CRL_TRANSFER_FILEINFO_CODE, o_message_jso);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: start crl file download fail", __FUNCTION__);
        return ret;
    }

    import_crl_data = cert_import_info_new(i_paras, import_cert_set_param_fn);
    if (import_crl_data == NULL) {
        debug_log(DLOG_ERROR, "%s: alloc a new cert_import info failed", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    import_crl_data->task_progress = TASK_NO_PROGRESS; 
    import_crl_data->rsc_privilege = USERROLE_SECURITYMGNT;
    body_jso = i_paras->val_jso;
    ret = create_new_task(IMPORT_HTTPS_CRL_TASK_NAME, svc_crl_import_status_monitor, import_crl_data, body_jso,
        i_paras->uri, o_result_jso);
    if (ret != RF_OK) {
        debug_log(DLOG_ERROR, "%s: create new task failed", __FUNCTION__);
        task_monitor_info_free(import_crl_data);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return HTTP_ACCEPTED;
}


LOCAL gint32 parse_security_svc_import_crl_retcode(json_object **o_message_jso, gint32 ret)
{
    switch (ret) {
        case IMPORT_CERT_URI_DISMATCH_ERR:
            (void)create_message_info(MSGID_CRL_IMPORT_FAILED, NULL, o_message_jso);
            debug_log(DLOG_ERROR, "%s: uri for importing CRL file is illegal", __FUNCTION__);
            return HTTP_BAD_REQUEST;
        case IMPORT_CRL_ERR_FORMAT_ERR:
            (void)create_message_info(MSGID_CRL_FILE_FORMAT_ERROR, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        case IMPORT_CRL_ERR_ROOT_CERT_NOT_IMPORT:
            (void)create_message_info(MSGID_ROOT_CERT_NOT_IMPORT, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        case IMPORT_CRL_ERR_ISSUER_VERIFY_FAIL:
            (void)create_message_info(MSGID_ROOT_CERT_MISMATCH, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        case IMPORT_CERT_INVALID_FILEPATH_ERR:
        case IMPORT_CERT_FILE_LEN_EXCEED_ERR:
            (void)create_message_info(MSGID_CRL_IMPORT_FAILED, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        
        case IMPORT_CRL_ERR_ISSUING_DATE_INVALID:
            (void)create_message_info(MSGID_CRL_ISSUING_DATE_INVALID, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 act_security_svc_import_crl(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    IMPORT_CERT_PARAMS_S params;
    gint32 ret;
    
    if (i_paras == NULL || o_message_jso == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: input param invalid", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if ((i_paras->user_role_privilege & USERROLE_SECURITYMGNT) == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        ret = HTTP_FORBIDDEN;
        goto EXIT;
    }

    ret = check_import_cert_params(i_paras, RFACTION_SECURITY_SERVICE_IMPORT_CRL, o_message_jso);
    if (ret != VOS_OK) {
        ret = HTTP_BAD_REQUEST;
        goto EXIT;
    }

    
    ret = is_import_cert_or_crl_task_exist(svc_crl_import_status_monitor, i_paras, FILE_TYPE_CRL, o_message_jso);
    if (ret != VOS_OK) {
        goto EXIT;
    }

    
    set_import_cert_params(&params, FILE_TYPE_CRL, import_cert_set_param_fn, process_import_local_https_crl,
        process_import_remote_https_crl, PROPERTY_SECURITY_SERVICE_HTTPS_CRL_PATH);
    ret = process_cert_or_crl_import(i_paras, o_message_jso, o_result_jso, &params);
    
    if (ret == HTTP_OK || ret == HTTP_ACCEPTED || *o_message_jso != NULL) {
        goto EXIT;
    }

    ret = parse_security_svc_import_crl_retcode(o_message_jso, ret);
EXIT:
    return ret;
}

LOCAL gint32 act_security_svc_import_crl_entry(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, 
    json_object **o_result_jso)
{
    gint32 ret = act_security_svc_import_crl(i_paras, o_message_jso, o_result_jso);
    if (ret != HTTP_ACCEPTED && i_paras != NULL) {
        delete_user_specific_file(i_paras, i_paras->val_jso, RFACTION_PARAM_CONTENT);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
    }
    return ret;
}


LOCAL gint32 parse_delete_trust_cert_ret_code(json_object **o_message_jso, gint32 ret)
{
    switch (ret) {
        
        case VOS_OK:
            (void)create_message_info(MSGID_SUCCESS, NULL, o_message_jso);
            return HTTP_OK;
        case VOS_ERR_MA_CERT_NOT_EXIST:
            (void)create_message_info(MSGID_CA_CERTIFICATE_NOT_EXIST, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            debug_log(DLOG_ERROR, "unknown cert import error:%d", ret);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 act_security_svc_delete_crl(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint32 cert_id;
    gint32 ret_code;
    OBJ_HANDLE obj_handle = 0;
    json_object *val_json = NULL;
    GSList *input_list = NULL;

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if ((i_paras->user_role_privilege & USERROLE_SECURITYMGNT) == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    (void)json_object_object_get_ex(i_paras->val_jso, RFACTION_PARAM_ROOT_CERT_ID, &val_json);

    
    if (json_type_int != json_object_get_type(val_json)) {
        debug_log(DLOG_ERROR, "%s: property is not of int type!", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFACTION_PARAM_ROOT_CERT_ID, o_message_jso,
            dal_json_object_get_str(val_json), RFACTION_PARAM_ROOT_CERT_ID);
        return HTTP_BAD_REQUEST;
    }
    cert_id = json_object_get_int(val_json);

    
    ret = dal_get_specific_object_byte(CLASS_HTTPS_SERVER_ROOTCERT, PROPERTY_HTTPS_SERVER_ROOTCERT_ID, (guint8)cert_id,
        &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Get obj handle failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)cert_id));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_HTTPS_SERVER_ROOTCERT, HTTPS_SERVER_ROOTCERT_METHOD_DELETE_CRL, AUTH_DISABLE,
        i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    ret_code = parse_delete_trust_cert_ret_code(o_message_jso, ret);

    return ret_code;
}


LOCAL gint32 act_security_svc_delete_trust_cert(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    json_object *val_json = NULL;
    gboolean bret;
    gint32 cert_id;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    gint32 ret_code;

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if ((i_paras->user_role_privilege & USERROLE_SECURITYMGNT) == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    bret = json_object_object_get_ex(i_paras->val_jso, RFACTION_PARAM_ROOT_CERT_ID, &val_json);
    if (bret != TRUE) {
        debug_log(DLOG_ERROR, "%s: json_object_object_get_ex is fail", __FUNCTION__);
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri);
        return HTTP_BAD_REQUEST;
    }
    if (json_type_int != json_object_get_type(val_json)) {
        debug_log(DLOG_ERROR, "%s: property is not of int type!", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFACTION_PARAM_ROOT_CERT_ID, o_message_jso,
            dal_json_object_get_str(val_json), RFACTION_PARAM_ROOT_CERT_ID);
        return HTTP_BAD_REQUEST;
    }

    cert_id = json_object_get_int(val_json);
    debug_log(DLOG_DEBUG, "%s: The certificate will be deleted, CertId = %d", __FUNCTION__, cert_id);

    
    ret = dal_get_specific_object_byte(CLASS_HTTPS_SERVER_ROOTCERT, PROPERTY_HTTPS_SERVER_ROOTCERT_ID, (guint8)cert_id,
        &obj_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: Get obj handle failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)cert_id));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_HTTPS_SERVER_ROOTCERT, HTTPS_SERVER_ROOTCERT_METHOD_DELETE_ROOTCERT, AUTH_DISABLE,
        i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    ret_code = parse_delete_trust_cert_ret_code(o_message_jso, ret);

    return ret_code;
}

gint32 parse_import_ca_cert_result(gint32 ret, json_object **o_message_jso)
{
    switch (ret) {
        case VOS_OK:
            (void)create_message_info(MSGID_SUCCESS, NULL, o_message_jso);
            return HTTP_OK;
        case VOS_ERR_MA_FORMAT_ERR:
            (void)create_message_info(MSGID_CA_CERTIFICATE_FORMAT_ERR, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;

        case VOS_ERR_MA_KEY_USAGE_CERT_SIGN_ERR:
        case VOS_ERR_MA_BASIC_CONSTRAINTS_ERR:
            (void)create_message_info(MSGID_CA_CERTIFICATE_CONSTRAINTS_ERR, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        case VOS_ERR_MA_REACH_MAX:
            (void)create_message_info(MSGID_CA_EXCEED_LIMIT_ERR, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        case VOS_ERR_MA_CERT_EXSIT:
            (void)create_message_info(MSGID_CA_CERTIFICATE_ALREADY_EXISTS, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        
        default:
            debug_log(DLOG_ERROR, "unknown cert import error:%d", ret);
            (void)create_message_info(MSGID_CERT_IMPORT_FAILED, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 process_import_local_https_cert(const gchar *file_path, json_object *user_data,
    json_object **o_message_jso)
{
    gint32 ret;
    GSList *input_list = NULL;
    const gchar *user_name = NULL;
    const gchar *client = NULL;
    gint32 user_priv = 0;
    gint32 from_webui_flag = REDFISH_REQ_FROM_OTHER_CLIENT;
    gint32 root_cert_id = 0;
    gint32 cert_usage = 0;

    
    if (strlen(file_path) >= MAX_FILEPATH_LENGTH) {
        debug_log(DLOG_ERROR, "%s: file path length is over %d or file is oversized", __FUNCTION__,
            MAX_FILEPATH_LENGTH);
        ret = IMPORT_CERT_INVALID_FILEPATH_ERR;
        goto exit;
    }

    if (vos_get_file_length(file_path) > CERT_MAX_SIZE) {
        debug_log(DLOG_ERROR, "%s: file is oversized", __FUNCTION__);
        ret = IMPORT_CERT_FILE_LEN_EXCEED_ERR;
        goto exit;
    }

    
    import_cert_get_common_userdata_fn(&user_name, &client, &user_priv, &from_webui_flag, user_data);
    (void)get_element_int(user_data, RFACTION_PARAM_ROOT_CERT_ID, &root_cert_id);
    (void)get_element_int(user_data, RFACTION_PARAM_ROOT_CERT_ID, &root_cert_id);
    (void)get_element_int(user_data, RFACTION_PARAM_ROOT_CERT_USAGE, &cert_usage);

    
    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)root_cert_id));
    input_list = g_slist_append(input_list, g_variant_new_string(file_path));
    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)cert_usage));

    ret = uip_call_class_method_redfish((guint8)from_webui_flag, user_name, client, 0, CLASS_HTTPS_SERVER_ROOTCERT,
        HTTPS_SERVER_ROOTCERT_METHOD_IMPORT_ROOTCERT, AUTH_DISABLE, user_priv, input_list, NULL);
    uip_free_gvariant_list(input_list);

    ret = parse_import_ca_cert_result(ret, o_message_jso);
exit:
    if (ret != HTTP_OK) {
        (void)proxy_delete_file(0, NULL, NULL, file_path);
    }
    return ret;
}


LOCAL gint32 ca_cert_import_status_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    gint32 ret;

    ret = import_cert_common_status_monitor(origin_obj_path, body_jso, process_import_local_https_cert, monitor_fn_data,
        message_obj);
    return ret;
}


LOCAL gint32 process_import_remote_https_cert(PROVIDER_PARAS_S *i_paras, const gchar *remote_uri,
    json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    TASK_MONITOR_INFO_S *cert_import_data = NULL;
    json_object *body_jso = NULL;

    ret =
        rf_start_file_download(i_paras, remote_uri, IMPORT_SECURITY_SERVICE_CRT_TRANSFER_FILEINFO_CODE, o_message_jso);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: start file download fail", __FUNCTION__);
        return ret;
    }

    cert_import_data = cert_import_info_new(i_paras, import_cert_set_param_fn);
    if (cert_import_data == NULL) {
        debug_log(DLOG_ERROR, "%s: alloc a new cert_import info failed", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    cert_import_data->task_progress = TASK_NO_PROGRESS; 
    cert_import_data->rsc_privilege = USERROLE_SECURITYMGNT;

    body_jso = i_paras->val_jso;
    ret = create_new_task(IMPORT_HTTPS_SERVER_CRT_TASK_NAME, ca_cert_import_status_monitor, cert_import_data, body_jso,
        i_paras->uri, o_result_jso);
    if (ret != RF_OK) {
        debug_log(DLOG_ERROR, "%s: create new task failed", __FUNCTION__);
        task_monitor_info_free(cert_import_data);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_ACCEPTED;
}


LOCAL gint32 https_cert_import_status_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    gint32 ret;

    ret = import_cert_common_status_monitor(origin_obj_path, body_jso, process_import_local_https_cert, monitor_fn_data,
        message_obj);
    return ret;
}

LOCAL void import_cert_set_param_fn(PROVIDER_PARAS_S *i_paras, json_object *user_data)
{
    json_object* jso_certid = NULL;
    const gchar* cert_usage = NULL;
    guint8 cert_usage_int;

    if (user_data == NULL) {
        debug_log(DLOG_ERROR, "%s: json user_data is invalid", __FUNCTION__);
        return;
    }
    
    import_cert_set_common_userdata_fn(i_paras, user_data);
    if (json_object_object_get_ex(i_paras->val_jso, RFACTION_PARAM_ROOT_CERT_ID, &jso_certid) == TRUE) {
        json_object_object_add(user_data, RFACTION_PARAM_ROOT_CERT_ID, json_object_get(jso_certid));
        
        cert_usage_int = INVALID_DATA_BYTE;
    } else {
        
        json_object_object_add(user_data, RFACTION_PARAM_ROOT_CERT_ID, json_object_new_int(CERT_ID_AUTO_SEARCH_FLAG));
        (void)get_element_str(i_paras->val_jso, RFACTION_PARAM_ROOT_CERT_USAGE, &cert_usage);
        if (g_strcmp0(cert_usage, RF_CERT_USAGE_EVT_SUBSCRIPTION) == 0) {
            cert_usage_int = CERT_USAGE_EVT_SUBSCRIPTION;
        } else {
            cert_usage_int = CERT_USAGE_FILE_TRANSFER;
        }
    }

    json_object_object_add(user_data, RFACTION_PARAM_ROOT_CERT_USAGE, json_object_new_int((gint32)cert_usage_int));
    return;
}


LOCAL gint32 check_import_cert_params(PROVIDER_PARAS_S* i_paras, const gchar* action_name, json_object** o_message_jso)
{
    json_bool b_ret;
    json_object *int_jso = NULL;
    json_object *usage = NULL;

    b_ret = json_object_object_get_ex(i_paras->val_jso, RFACTION_PARAM_ROOT_CERT_ID, &int_jso);
    
    if ((b_ret) && (json_type_int != json_object_get_type(int_jso))) {
        debug_log(DLOG_ERROR, "Missing %s in jso or property is not of int type!", RFACTION_PARAM_ROOT_CERT_ID);
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFACTION_PARAM_ROOT_CERT_ID, o_message_jso, 
            dal_json_object_get_str(int_jso), RFACTION_PARAM_ROOT_CERT_ID);
        return VOS_ERR;
    }

    
    if ((!b_ret) && (json_object_object_get_ex(i_paras->val_jso, RFACTION_PARAM_ROOT_CERT_USAGE, &usage) == FALSE)) {
        (void)create_message_info(MSGID_ACT_PARA_MISSING, RFACTION_PARAM_ROOT_CERT_USAGE, o_message_jso, action_name,
            RFACTION_PARAM_ROOT_CERT_USAGE);
        return VOS_ERR;
    }

    return VOS_OK;
}


LOCAL gint32 act_security_svc_import_trust_cert(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    IMPORT_CERT_PARAMS_S params;

    
    if ((i_paras == NULL) || (o_message_jso == NULL) || (o_result_jso == NULL)) {
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if ((i_paras->user_role_privilege & USERROLE_SECURITYMGNT) == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        ret = HTTP_FORBIDDEN;
        goto EXIT;
    }

    if (check_import_cert_params(i_paras, RFACTION_SECURITY_SERVICE_IMPORT_TRUST_CERT, o_message_jso) != VOS_OK) {
        ret = HTTP_BAD_REQUEST;
        goto EXIT;
    }

    
    ret =
        is_import_cert_or_crl_task_exist(https_cert_import_status_monitor, i_paras, FILE_TYPE_ROOT_CERT, o_message_jso);
    if (ret != VOS_OK) {
        goto EXIT;
    }

    set_import_cert_params(&params, FILE_TYPE_ROOT_CERT, import_cert_set_param_fn, process_import_local_https_cert,
        process_import_remote_https_cert, PROPERTY_SECURITY_SERVICE_HTTPS_CERT_PATH);
    ret = process_cert_or_crl_import(i_paras, o_message_jso, o_result_jso, &params);
    
    if (ret == HTTP_OK || ret == HTTP_ACCEPTED || *o_message_jso != NULL) {
        goto EXIT;
    }

    switch (ret) {
        case IMPORT_CERT_URI_DISMATCH_ERR:
        case IMPORT_CERT_FILE_LEN_EXCEED_ERR:
        case IMPORT_CERT_INVALID_FILEPATH_ERR:
            (void)create_message_info(MSGID_CERT_IMPORT_FAILED, NULL, o_message_jso);
            ret = HTTP_BAD_REQUEST;
            break;
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            debug_log(DLOG_ERROR, "%s: import root cert fail, ret:%d", __FUNCTION__, ret);
            ret = HTTP_INTERNAL_SERVER_ERROR;
            break;
    }

EXIT:
    return ret;
}

LOCAL gint32 act_security_svc_import_trust_cert_entry(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, 
    json_object **o_result_jso)
{
    gint32 ret = act_security_svc_import_trust_cert(i_paras, o_message_jso, o_result_jso);
    if (ret != HTTP_ACCEPTED && i_paras != NULL) {
        delete_user_specific_file(i_paras, i_paras->val_jso, RFACTION_PARAM_CONTENT);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
    }
    return ret;
}


LOCAL gint32 get_sol_auto_os_lock_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint8 lock_enable = 0;

    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dfl_get_object_handle(OBJ_SOL_MANAGEMENT, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get obj handle failed, ret = %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SOL_OS_AUTO_LOCK_ENABLED, &lock_enable);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get property failed, ret = %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_boolean((json_bool)lock_enable);
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_boolean failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 set_sol_auto_os_lock_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    GSList *input_list = NULL;
    json_bool lock_enabled;
    OBJ_HANDLE obj_handle;

    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if ((i_paras->user_role_privilege & USERROLE_SECURITYMGNT) == 0) {
        debug_log(DLOG_INFO, "%s: user dont have security privilege", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, REPROP_SOL_AUTO_OS_LOCK_ENABLED, o_message_jso,
            REPROP_SOL_AUTO_OS_LOCK_ENABLED);
        return HTTP_FORBIDDEN;
    }

    lock_enabled = json_object_get_boolean(i_paras->val_jso) ? ENABLE : DISABLE;

    ret = dfl_get_object_handle(OBJ_SOL_MANAGEMENT, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get obj handle failed, ret = %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)lock_enabled));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_NAME_SOL_MANAGEMENT, METHOD_SET_OS_AUTO_LOCK_ENABLED, AUTH_DISABLE, i_paras->user_role_privilege,
        input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: call method failed, ret = %d", __FUNCTION__, ret);
        return HTTP_BAD_REQUEST;
    }

    return HTTP_OK;
}


LOCAL gint32 get_sol_auto_os_lock_key(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint8 lock_key = 0;

    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dfl_get_object_handle(OBJ_SOL_MANAGEMENT, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get obj handle failed, ret = %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SOL_OS_AUTO_LOCK_KEY, &lock_key);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get property failed, ret = %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_int((json_bool)lock_key);
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_int failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 set_sol_auto_os_lock_key(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
#define KEY_CTRL_D 0
#define KEY_EXIT 1

    gint32 ret;
    GSList *input_list = NULL;
    gint32 lock_key;
    OBJ_HANDLE obj_handle;

    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if ((i_paras->user_role_privilege & USERROLE_SECURITYMGNT) == 0) {
        debug_log(DLOG_INFO, "%s: user dont have security privilege", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, REPROP_SOL_AUTO_OS_LOCK_KEY, o_message_jso,
            REPROP_SOL_AUTO_OS_LOCK_KEY);
        return HTTP_FORBIDDEN;
    }

    lock_key = json_object_get_int(i_paras->val_jso);
    if (lock_key != KEY_CTRL_D && lock_key != KEY_EXIT) {
        (void)create_message_info(MSGID_PROPERTY_VALUE_ERR, REPROP_SOL_AUTO_OS_LOCK_KEY, o_message_jso,
            REPROP_SOL_AUTO_OS_LOCK_KEY);
        return HTTP_BAD_REQUEST;
    }

    ret = dfl_get_object_handle(OBJ_SOL_MANAGEMENT, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get obj handle failed, ret = %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(lock_key));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_NAME_SOL_MANAGEMENT, METHOD_SET_OS_AUTO_LOCK_KEY, AUTH_DISABLE, i_paras->user_role_privilege, input_list,
        NULL);
    uip_free_gvariant_list(input_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: call method failed, ret = %d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}

LOCAL PROPERTY_PROVIDER_S g_security_svc_provider[] = {
    {
        RFPROP_ODATA_ID,
        "", "",
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_security_svc_odataid,
        NULL,
        NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_ODATA_CONTEXT,
        "", "",
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_security_svc_odata_context,
        NULL,
        NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_SECURITY_CONTROL_VERSION,
        "", "",
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_security_svc_security_control_version,
        NULL,
        NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROPERTY_SESSION_TOKEN_LENGTH,
        "", "",
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID, 
        get_security_svc_session_token_length,
        set_security_svc_session_token_length,
        NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_SECURITY_CONTROL_VERSION,
        "", "",
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_security_svc_security_control_version,
        NULL,
        NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_MK_AUTO_UPDATE_INTERVAL,
        "", "",
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        get_security_svc_mk_auto_update_interval,
        set_security_svc_mk_auto_update_interval,
        NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_HTTPS_TRANSFER_CERT_VERIFICATION,
        "", "",
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        get_security_svc_https_transfer_cert_verification,
        set_security_svc_https_transfer_cert_verification,
        NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_REMOTE_HTTPS_SERVER_CERT_CHAIN_IFNO,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_security_svc_https_cert_info,
        NULL,
        NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_LINKS,
        "", "",
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_security_svc_links,
        NULL,
        NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_SSH_CIPHERS,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        get_security_svc_ssh_ciphers,
        set_seucrity_svc_ssh_ciphers,
        NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_SSH_KEXS,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        get_security_svc_ssh_kexs,
        set_seucrity_svc_ssh_kexs,
        NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_SSH_MACS,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        get_security_svc_ssh_MACs,
        set_seucrity_svc_ssh_MACs,
        NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_SSH_HOST_KEYS,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        get_security_svc_ssh_host_keys,
        set_seucrity_svc_ssh_host_keys,
        NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_SSL_CIPHER_SUITES,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        get_security_svc_ssl_cipher_suites,
        set_seucrity_svc_ssl_cipher_suites,
        NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_RMCP_CIPHER_SUITES,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        get_security_svc_rmcp_cipher_suites,
        set_seucrity_svc_rmcp_cipher_suites,
        NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_ACTIONS,
        "", "",
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_security_actions,
        NULL,
        NULL,
        ETAG_FLAG_DISABLE
    },
    {
        RFACTION_UPDATE_MASTERKEY,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_SECURITYMGNT,
        SYS_LOCKDOWN_FORBID,
        NULL,
        NULL,
        act_security_svc_update_masterkey,
        ETAG_FLAG_DISABLE
    },
    {
        RFACTION_SECURITY_SERVICE_IMPORT_TRUST_CERT,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_SECURITYMGNT,
        SYS_LOCKDOWN_FORBID,
        NULL,
        NULL,
        act_security_svc_import_trust_cert_entry,
        ETAG_FLAG_DISABLE
    },
    {
        RFACTION_SECURITY_SERVICE_DELETE_TRUST_CERT,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_SECURITYMGNT,
        SYS_LOCKDOWN_FORBID,
        NULL,
        NULL,
        act_security_svc_delete_trust_cert,
        ETAG_FLAG_DISABLE
    },
    {
        RFACTION_SECURITY_SERVICE_IMPORT_CRL,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_SECURITYMGNT,
        SYS_LOCKDOWN_FORBID,
        NULL, NULL,
        act_security_svc_import_crl_entry,
        ETAG_FLAG_DISABLE
    },
    {
        RFACTION_SECURITY_SERVICE_DELETE_CRL,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_SECURITYMGNT,
        SYS_LOCKDOWN_FORBID,
        NULL, NULL,
        act_security_svc_delete_crl,
        ETAG_FLAG_DISABLE
    },
    {
        REPROP_SOL_AUTO_OS_LOCK_ENABLED,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        get_sol_auto_os_lock_enabled,
        set_sol_auto_os_lock_enabled,
        NULL,
        ETAG_FLAG_ENABLE
    },
    {
        REPROP_SOL_AUTO_OS_LOCK_KEY,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        get_sol_auto_os_lock_key,
        set_sol_auto_os_lock_key,
        NULL,
        ETAG_FLAG_ENABLE
    }
};


LOCAL gint32 get_security_srv_actioninfo(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar action_info_uri[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};

    return_val_do_info_if_fail((i_paras != NULL) && (o_message_jso != NULL) && (o_result_jso != NULL),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__));

    (void)redfish_get_board_slot(slot_id, MAX_RSC_ID_LEN);

    ret = snprintf_s(uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, MANAGER_SECURITY_SVC_URI, slot_id);
    return_val_do_info_if_fail(ret > 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: format string failed, ret is %d", __FUNCTION__, ret));

    ret = rf_update_rsc_actioninfo_uri(g_security_svc_action_array, G_N_ELEMENTS(g_security_svc_action_array),
        i_paras->uri, uri, action_info_uri, MAX_URI_LENGTH);
    return_val_do_info_if_fail(ret == VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: update action info uri (%s) failed", __FUNCTION__, i_paras->uri));

    *o_result_jso = json_object_new_string((const gchar*)action_info_uri);

    return HTTP_OK;
}

LOCAL PROPERTY_PROVIDER_S g_security_svc_actioninfo_provider[] = {
    {
        RFPROP_ODATA_ID,
        MAP_PROPERTY_NULL,
        MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_security_srv_actioninfo,
        NULL, NULL,
        ETAG_FLAG_ENABLE
    }
};


gint32 security_svc_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gboolean b_ret;

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count), VOS_ERR,
        debug_log(DLOG_ERROR, "input param error"));

    b_ret = redfish_check_manager_uri_valid(i_paras->uri); // 校验用户输入的URI是否有效
    if (b_ret == TRUE) {
        *prop_provider = g_security_svc_provider;
        *count = sizeof(g_security_svc_provider) / sizeof(PROPERTY_PROVIDER_S);
        return VOS_OK;
    } else {
        return HTTP_NOT_FOUND;
    }
}


gint32 security_srv_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gboolean b_ret;

    return_val_do_info_if_fail((i_paras != NULL) && (prop_provider != NULL) && (count != NULL), VOS_ERR,
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__));

    b_ret = redfish_check_manager_uri_valid(i_paras->uri);
    if (b_ret == TRUE) {
        *prop_provider = g_security_svc_actioninfo_provider;
        *count = sizeof(g_security_svc_actioninfo_provider) / sizeof(PROPERTY_PROVIDER_S);
        return VOS_OK;
    } else {
        return HTTP_NOT_FOUND;
    }
}


LOCAL gint32 get_https_cert_actioninfo_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar               https_cert_uri[MAX_URI_LENGTH] = {0};
    gchar               action_info_uri[MAX_URI_LENGTH] = {0};
    gchar               slot_id[MAX_RSC_ID_LEN] = {0};
    gint32 ret;

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_message_jso) && (NULL != o_result_jso),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    (void)redfish_get_board_slot(slot_id, MAX_RSC_ID_LEN);

    ret = snprintf_s(https_cert_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, MANAGER_HTTPS_CERT_URI, slot_id);
    return_val_do_info_if_fail(ret > 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "format string failed, ret is %d", ret));

    ret = rf_update_rsc_actioninfo_uri(g_https_cert_action_array, G_N_ELEMENTS(g_https_cert_action_array), i_paras->uri,
        https_cert_uri, action_info_uri, MAX_URI_LENGTH);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "update action info uri (%s) failed", i_paras->uri));

    *o_result_jso = json_object_new_string((const gchar*)action_info_uri);

    return HTTP_OK;
}

LOCAL PROPERTY_PROVIDER_S g_https_cert_action_info_provider[] = {
    {
        RFPROP_ODATA_ID,
        "", "",
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_https_cert_actioninfo_odataid,
        NULL,
        NULL,
        ETAG_FLAG_ENABLE
    }
};


gint32 https_cert_actioninfo_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gboolean b_ret;

    return_val_do_info_if_fail((i_paras != NULL) && (prop_provider != NULL) && (count != NULL), VOS_ERR,
        debug_log(DLOG_ERROR, "input param error"));

    b_ret = redfish_check_manager_uri_valid(i_paras->uri); // 校验用户输入的URI是否有效
    if (b_ret == TRUE) {
        *prop_provider = g_https_cert_action_info_provider;
        *count = sizeof(g_https_cert_action_info_provider) / sizeof(PROPERTY_PROVIDER_S);
        return VOS_OK;
    } else {
        return HTTP_NOT_FOUND;
    }
}
