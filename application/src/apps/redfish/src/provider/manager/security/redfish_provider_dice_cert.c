

#ifdef ARM64_HI1711_ENABLED
#include "redfish_provider.h"

#define RFACTION_DICE_CERT_EXPORT_CSR "DiceCert.ExportDiceCSR"
#define RFACTION_DICE_CERT_IMPORT_CERT "DiceCert.ImportDiceCertificate"           // 安全核bootLoader固件证书
#define RFACTION_DICE_CERT_EXPORT_CERT_CHAIN "DiceCert.ExportDiceCertficateChain" // DICE 证书链
#define EXPORT_CSR 0
#define EXPORT_CERT_CHAIN 1
#define ACT_TYPE "act_type"
#define TMP_DICE_CERT_FILE "/tmp/dice_cert.cer"
#define DICE_CSR_FILE "/tmp/dice_csr.csr"


#define DICE_CERT_IMPORT_REGEX "^((https|sftp|nfs|cifs|scp)://.{1,1000}|/tmp/.{1,246})\\.(crt|cer|pem|der)$"
void create_dice_cert_export_err_message(gint32 transfer_status, json_object **message_obj);

LOCAL const gchar*       g_dice_cert_action_array[] = {
    RFACTION_DICE_CERT_EXPORT_CSR,
    RFACTION_DICE_CERT_IMPORT_CERT,
    RFACTION_DICE_CERT_EXPORT_CERT_CHAIN
};

LOCAL gint32 parse_dice_cert_import_result_code(gint32 err_code, json_object **o_message_jso)
{
    gint32 ret;
    const gchar *msg_id = NULL;

    switch (err_code) {
        
        case FILE_MISMATCH:
            (void)create_message_info(MSGID_CERT_MISMATCH, NULL, o_message_jso);
            ret = HTTP_BAD_REQUEST;
            break;

            
        case IMPORT_FAIL:
            ret = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_CERT_IMPORT_FAILED, NULL, o_message_jso);
            break;

            
        case IMPORT_OK:
            ret = HTTP_OK;
            (void)create_message_info(MSGID_CERT_IMPORT_OK, NULL, o_message_jso);
            break;

            
        case VOS_ERR_MA_CERT_NO_MATCH_DEVICE:
            ret = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_CERT_NO_MATCH_DEVICE, NULL, o_message_jso);
            break;

            
        case VOS_ERR_MA_FORMAT_ERR:
            ret = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_CA_CERTIFICATE_FORMAT_ERR, NULL, o_message_jso);
            break;

            
        case VOS_ERR_MA_CERT_EXPIRE:
            ret = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_CERT_EXPIRED, NULL, o_message_jso);
            break;

        default:
            msg_id = MSGID_CERT_IMPORT_FAILED;
            (void)create_message_info(msg_id, NULL, o_message_jso);
            ret = HTTP_BAD_REQUEST;
            debug_log(DLOG_ERROR, "unknown cert import error:%d", err_code);
            break;
    }

    return ret;
}

LOCAL void clean_and_write_dice_file_operation_log(GSList *caller_info, gint8 act_type, gint8 success_flg)
{
    if (EXPORT_CSR == act_type) { // 导出 DICE CSR文件
        proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "Export DICE CSR %s",
            (TRUE == success_flg) ? "successfully" : "failed");
        (void)proxy_delete_file(0, NULL, NULL, DICE_CSR_FILE);
    } else { // 导出证书链
        proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "Export DICE certificate chain %s",
            (TRUE == success_flg) ? "successfully" : "failed");
    }
}

LOCAL void record_dice_operation_log(const gchar *user_name, const gchar *client_ip, const gchar *print_str,
    guchar from_webui_flag)
{
    GSList *caller_info = NULL;

    caller_info =
        g_slist_append(caller_info, g_variant_new_string(dal_rf_get_op_log_module_name((guchar)from_webui_flag)));
    caller_info = g_slist_append(caller_info, g_variant_new_string(user_name));
    caller_info = g_slist_append(caller_info, g_variant_new_string(client_ip));
    proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "%s", print_str);
    uip_free_gvariant_list(caller_info);
}

LOCAL gint32 get_dice_cert_chain_export_progress(gint32 *progress)
{
    GSList *output_list = NULL;
    gint32 ret;
    guint8 status;

    if (NULL == progress) {
        return VOS_ERR;
    }

    ret = uip_call_class_method(NULL, NULL, NULL, BMC_CLASEE_NAME, BMC_OBJECT_NAME,
        METHOD_GET_DICE_CERT_CHAIN_EXPORT_STATE, NULL, &output_list);
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "Get dice cert chain export state failed.ret = %d", ret);
        return ret;
    }

    status = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 0));
    debug_log(DLOG_DEBUG, "Get dice cert chain export state = %d", status); 
    switch (status) { // status总是正值, 但在失败场景时, 我们希望返回负值.
        case ACT_COMPLETE:
            *progress = RF_FINISH_PERCENTAGE;
            break;
        case ACT_FAILED:
            *progress = -1;
            break;
        case DICE_PRECHECK_FAILED:
        case DICE_GET_CERT_0_FAILED:
        case DICE_GET_CERT_1_FAILED:
        case DICE_GET_LAST_CERT_FAILED:
        case DICE_TRANSFER_FILE_FAILED:
            *progress = -status; // 所有的失败都以负值表示
            break;
        case ACT_IN_PROCESS:
        default:
            *progress = 50; // 未导完按50%算
            break;
    }

    
    uip_free_gvariant_list(output_list);
    return VOS_OK;
}

void create_dice_cert_export_err_message(gint32 transfer_status, json_object **message_obj)
{
    if (transfer_status == -DICE_TRANSFER_FILE_FAILED) {
        (void)create_message_info(MSGID_FILE_TRANSFER_ERR_DESC, NULL, message_obj, CURL_PEER_SSL_VERIFY_FAILED_STR);
    } else if (transfer_status == -DICE_GET_CERT_0_FAILED) {
        (void)create_message_info(MSGID_FILE_TRANSFER_ERR_DESC, NULL, message_obj, DICE_GET_CERT_FAILED_STR);
    } else {
        (void)create_message_info(MSGID_EXPORT_CSR_FAILED, NULL, message_obj);
    }
}


LOCAL gint32 dice_file_export_status_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    gint32 ret;
    gint32 transfer_status = 0;
    GSList *caller_info = NULL;
    const gchar *user_name = NULL;
    const gchar *user_ip = NULL;
    gint32 act_type = 0;

    (void)get_element_str((json_object *)monitor_fn_data->user_data, RF_LOG_USER_NAME, &user_name);
    (void)get_element_str((json_object *)monitor_fn_data->user_data, RF_LOG_USER_IP, &user_ip);
    (void)get_element_int((json_object *)monitor_fn_data->user_data, ACT_TYPE, &act_type);

    if (act_type == EXPORT_CSR) {
        ret = get_file_transfer_progress(&transfer_status);
        goto_label_do_info_if_fail(ret == VOS_OK, err_quit,
            debug_log(DLOG_ERROR, "get file transfer progress failed, ret = %d", ret));
    } else {
        ret = get_dice_cert_chain_export_progress(&transfer_status);
        goto_label_do_info_if_fail(ret == VOS_OK, err_quit,
            debug_log(DLOG_ERROR, "Get dice cert chain export state failed.ret = %d", ret));
    }
    caller_info = g_slist_append(caller_info, g_variant_new_string(USER_LOGIN_INTERFACE_REDFISH_STRING));
    caller_info = g_slist_append(caller_info, g_variant_new_string(user_name));
    caller_info = g_slist_append(caller_info, g_variant_new_string(user_ip));
    debug_log(DLOG_INFO, "transfer_status = %d", transfer_status);
    if (transfer_status < 0) {
        if (act_type == EXPORT_CSR) {
            (void)parse_file_transfer_err_code(transfer_status, message_obj);
        } else {
            create_dice_cert_export_err_message(transfer_status, message_obj);
        }
        goto err_quit;
    } 
    monitor_fn_data->task_progress = transfer_status;
    if (transfer_status == RF_FINISH_PERCENTAGE) {
        monitor_fn_data->task_state = RF_TASK_COMPLETED;
        json_object_clear_string(body_jso, RFACTION_PARAM_CONTENT);
        clean_and_write_dice_file_operation_log(caller_info, act_type, TRUE);
    } else {
        monitor_fn_data->task_state = RF_TASK_RUNNING;
    }
    uip_free_gvariant_list(caller_info);
    return RF_OK;

err_quit:
    monitor_fn_data->task_state = RF_TASK_EXCEPTION;
    json_object_clear_string(body_jso, RFACTION_PARAM_CONTENT);
    do_if_expr(*message_obj == NULL, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj));
    clean_and_write_dice_file_operation_log(caller_info, act_type, FALSE);
    uip_free_gvariant_list(caller_info);
    return RF_OK;
}


LOCAL gint32 act_export_csr(PROVIDER_PARAS_S *i_paras, const gchar *file_path, json_object **o_message_jso,
    GSList *input_list)
{
    gint32 ret;

    // 判断URI是否为目录名
    if (file_path[0] == '/' && g_file_test(file_path, G_FILE_TEST_IS_DIR) == TRUE) {
        (void)create_message_info(MSGID_NOT_ALLOW_OVERWRITE_DIR, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }
    ret = g_regex_match_simple(CSR_EXPORT_FILE_REGEX, file_path, G_REGEX_OPTIMIZE, (GRegexMatchFlags)0);
    if (ret != TRUE) {
        (void)create_message_info(MSGID_INVALID_PATH, NULL, o_message_jso, RF_SENSITIVE_INFO, RFACTION_PARAM_CONTENT);
        return HTTP_BAD_REQUEST;
    }

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, OBJ_HANDLE_COMMON,
        BMC_CLASEE_NAME, METHOD_EXPORT_DICE_CSR, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    if (file_path[0] == '/' && ret == VOS_OK) {
        // 更新文件属主与权限
        ret = dal_set_file_owner(file_path, APACHE_UID, APACHE_GID);
        do_if_expr(ret == VOS_OK,
            ret = dal_change_file_owner(i_paras->auth_type, file_path, i_paras->user_name, S_IRUSR | S_IWUSR));
        if (ret != VOS_OK) {
            debug_log(DLOG_ERROR, "change file owner failed,ret is %d", ret);
            (void)proxy_delete_file(0, NULL, NULL, file_path);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        (void)create_message_info(MSGID_SUCCESS, NULL, o_message_jso);
    }
    return VOS_OK;
}


LOCAL gint32 dice_cert_export_file(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso,
    gint32 act_type)
{
    gint32 ret = 0;
    const gchar *file_path = NULL;
    const gchar *nonce_str_data = NULL;
    GSList *input_list = NULL;
    gboolean b_ret;

    if ((i_paras->user_role_privilege & USERROLE_SECURITYMGNT) == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        ret = HTTP_FORBIDDEN;
        goto EXIT;
    }

    (void)get_element_str(i_paras->val_jso, RFACTION_PARAM_CONTENT, &file_path);
    goto_label_do_info_if_expr(NULL == file_path, EXIT, ret = HTTP_INTERNAL_SERVER_ERROR;
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    input_list = g_slist_append(input_list, g_variant_new_string(file_path));
    if (act_type == EXPORT_CSR) { // 导CSR
        ret = act_export_csr(i_paras, (const gchar *)file_path, o_message_jso, input_list);
        goto_label_do_info_if_expr(ret != VOS_OK, EXIT, uip_free_gvariant_list(input_list));
    } else { // 导证书链
        (void)get_element_str(i_paras->val_jso, "Nonce", &nonce_str_data);
        if (nonce_str_data == NULL) {
            ret = HTTP_BAD_REQUEST;
            create_message_info(MSGID_ACT_PARA_MISSING, NULL, o_message_jso, RFACTION_DICE_CERT_EXPORT_CSR, "Nonce");
            debug_log(DLOG_ERROR, "%s failed:nonce is null", __FUNCTION__);
            uip_free_gvariant_list(input_list);
            goto EXIT;
        }

        input_list = g_slist_append(input_list, g_variant_new_string(nonce_str_data));
        input_list = g_slist_append(input_list, g_variant_new_byte(i_paras->auth_type));
        ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
            OBJ_HANDLE_COMMON, BMC_CLASEE_NAME, METHOD_EXPORT_DICE_CERT_CHAIN, AUTH_ENABLE,
            i_paras->user_role_privilege, input_list, NULL);
    }
    uip_free_gvariant_list(input_list);
    if ((file_path[0] == '/') && (ret == RET_OK) && (act_type == EXPORT_CSR)) {
        (void)create_message_info(MSGID_SUCCESS, NULL, o_message_jso);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
        return HTTP_OK;
    }

    goto_label_do_info_if_expr((VOS_OK != ret), EXIT, ret = HTTP_BAD_REQUEST;
        (void)create_message_info(MSGID_EXPORT_CSR_FAILED, NULL, o_message_jso); debug_log(DLOG_ERROR,
        "export %s file failed(ret=%d).", ((act_type == EXPORT_CSR) ? "csr" : "cert chain"), ret));

    // 3.2 任务重复性检查
    b_ret = check_redfish_running_task_exist(dice_file_export_status_monitor, i_paras->val_jso, i_paras->uri, NULL);
    if (b_ret != FALSE) {
        // 日志打印
        debug_log(DLOG_ERROR, "%s failed:running task exist", __FUNCTION__);

        // 创建消息
        (void)create_message_info(MSGID_TASK_CSR_EXPORT_ERR, NULL, o_message_jso);
        // 返回结果
        ret = HTTP_BAD_REQUEST;
        goto EXIT;
    }

    TASK_MONITOR_INFO_S *monitor_fn_data = task_monitor_info_new((GDestroyNotify)json_object_put);
    if (monitor_fn_data == NULL) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s:task_monitor_info_new failed.", __FUNCTION__);
        ret = HTTP_INTERNAL_SERVER_ERROR;
        goto EXIT;
    }

    monitor_fn_data->user_data = (void *)json_object_new_object();
    goto_label_do_info_if_expr(monitor_fn_data->user_data == NULL, EXIT, ret = HTTP_INTERNAL_SERVER_ERROR;
        debug_log(DLOG_ERROR, "%s:alloc new json object failed.", __FUNCTION__);
        task_monitor_info_free(monitor_fn_data); (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    json_object_object_add(monitor_fn_data->user_data, RF_LOG_USER_NAME, json_object_new_string(i_paras->user_name));
    json_object_object_add(monitor_fn_data->user_data, RF_LOG_USER_IP, json_object_new_string(i_paras->client));
    json_object_object_add((json_object *)monitor_fn_data->user_data, ACT_TYPE, json_object_new_int(act_type));

    monitor_fn_data->rsc_privilege = USERROLE_SECURITYMGNT;
    ret = create_new_task("dice file export task", dice_file_export_status_monitor, monitor_fn_data, i_paras->val_jso,
        i_paras->uri, o_result_jso);
    goto_label_do_info_if_expr(ret != VOS_OK, EXIT, ret = HTTP_INTERNAL_SERVER_ERROR;
        debug_log(DLOG_ERROR, "create export task failed, ret is %d", ret); task_monitor_info_free(monitor_fn_data);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    return HTTP_ACCEPTED;

EXIT:
    json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
    record_dice_operation_log(i_paras->user_name, i_paras->client, 
        (act_type == EXPORT_CSR) ? "Export DICE CSR failed" : "Export DICE certificate chain failed",
        i_paras->is_from_webui);
    return ret;
}


LOCAL gint32 act_dice_cert_export_csr(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    if (i_paras == NULL) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (o_message_jso == NULL || o_result_jso == NULL) {
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
        record_dice_operation_log(i_paras->user_name, i_paras->client, "Export DICE CSR failed",
            i_paras->is_from_webui);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return dice_cert_export_file(i_paras, o_message_jso, o_result_jso, EXPORT_CSR);
}


LOCAL gint32 act_dice_cert_export_cert_chain(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    if (i_paras == NULL) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (o_message_jso == NULL || o_result_jso == NULL) {
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
        record_dice_operation_log(i_paras->user_name, i_paras->client, "Export DICE certificate chain failed",
            i_paras->is_from_webui);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return dice_cert_export_file(i_paras, o_message_jso, o_result_jso, EXPORT_CERT_CHAIN);
}


LOCAL gint32 get_dice_cert_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar change_uri[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: redfish get board slot id failed, ret = %d.", __FUNCTION__, __LINE__, ret));

    ret = snprintf_s(change_uri, sizeof(change_uri), sizeof(change_uri) - 1, MANAGER_DICE_CERT_URI, slot_id);
    return_val_do_info_if_expr(ret <= VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : snprintf_s fail, ret = %d", __FUNCTION__, ret));

    *o_result_jso = json_object_new_string((const char *)change_uri);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_dice_cert_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
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
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: redfish get board slot id failed, ret = %d.", __FUNCTION__, __LINE__, ret));

    ret = snprintf_s(change_uri, sizeof(change_uri), sizeof(change_uri) - 1, MANAGER_DICE_CERT_METADATA, slot_id);
    return_val_do_info_if_expr(ret <= VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : snprintf_s fail, ret = %d", __FUNCTION__, ret));

    *o_result_jso = json_object_new_string((const char *)change_uri);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_dice_cert_actions(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar slot_id[MAX_RSC_ID_LEN] = {0};
    json_object *action_jso = NULL;
    guint32 i;
    guint32 len;
    gchar    rsc_uri[MAX_URI_LENGTH] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: parameter is NULL.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s, %d: get_board_slot failed.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    ret = snprintf_s(rsc_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, MANAGER_DICE_CERT_URI, slot_id);
    return_val_do_info_if_fail(ret > 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "format string failed, ret is %d", ret));

    action_jso = json_object_new_object();
    if (NULL == action_jso) {
        debug_log(DLOG_ERROR, "alloc new object failed");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    len = G_N_ELEMENTS(g_dice_cert_action_array);
    for (i = 0; i < len; i++) {
        rf_add_action_prop(action_jso, (const gchar *)rsc_uri, g_dice_cert_action_array[i]);
    }
    *o_result_jso = action_jso;

    return HTTP_OK;
}


LOCAL gint32 dice_cert_import(gint32 from_webui_flag, const gchar *user_name, const gchar *client_ip,
    json_object *body_jso, json_object **o_message_jso)
{
    GSList *input_list = NULL;
    gint32 ret;

    return_val_do_info_if_fail((NULL != user_name) && (NULL != client_ip) && (NULL != body_jso) &&
        (NULL != o_message_jso),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    
    (void)dal_set_file_owner(TMP_DICE_CERT_FILE, REDFISH_USER_UID, APPS_USER_GID);
    (void)dal_set_file_mode(TMP_DICE_CERT_FILE, (S_IRUSR | S_IWUSR));

    
    input_list = g_slist_append(input_list, g_variant_new_string(TMP_DICE_CERT_FILE));
    ret = uip_call_class_method_redfish((guchar)from_webui_flag, user_name, client_ip, OBJ_HANDLE_COMMON,
        BMC_CLASEE_NAME, METHOD_IMPORT_DICE_CERT, AUTH_DISABLE, 0, input_list, NULL);
    uip_free_gvariant_list(input_list);
    return parse_dice_cert_import_result_code(ret, o_message_jso);
}


LOCAL gint32 dice_cert_import_status_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    gint32 file_transfer_progress = 0;
    const gchar *file_path = NULL;
    gchar               cert_path[MAX_URL_LEN] = {0};
    const gchar *user_name = NULL;
    const gchar *client_ip = NULL;
    gint32 from_webui_flag = REDFISH_REQ_FROM_OTHER_CLIENT;

    
    gint32 ret = get_file_transfer_progress(&file_transfer_progress);
    goto_label_do_info_if_expr(VOS_OK != ret, err_quit,
        debug_log(DLOG_ERROR, "get file transfer progress failed, ret is %d", ret));
    (void)get_element_str((json_object *)monitor_fn_data->user_data, RF_LOG_USER_NAME, &user_name);
    (void)get_element_str((json_object *)monitor_fn_data->user_data, RF_LOG_USER_IP, &client_ip);
    (void)get_element_int((json_object *)monitor_fn_data->user_data, RF_USERDATA_FROM_WEBUI_FLAG, &from_webui_flag);
    if (file_transfer_progress < 0) {
        (void)parse_file_transfer_err_code(file_transfer_progress, message_obj);
        monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        json_object_clear_string(body_jso, RFACTION_PARAM_CERTICATE);
        record_dice_operation_log(user_name, client_ip, "Import DICE certificate failed", from_webui_flag);
        return RF_OK;
    } else {
        
        monitor_fn_data->task_progress = (guchar)(file_transfer_progress * 0.5);

        
        return_val_do_info_if_expr(RF_FINISH_PERCENTAGE != file_transfer_progress, RF_OK,
            (monitor_fn_data->task_state = RF_TASK_RUNNING));

        
        (void)get_element_str(body_jso, RFACTION_PARAM_CERTICATE, &file_path);
        goto_label_do_info_if_expr(NULL == file_path, err_quit,
            debug_log(DLOG_ERROR, "get param %s from body failed", RFACTION_PARAM_CERTICATE));

        gchar *slash_index = g_strrstr(file_path, SLASH_FLAG_STR);
        goto_label_do_info_if_expr(NULL == slash_index, err_quit,
            debug_log(DLOG_ERROR, "get slash index from file_path failed"));

        ret = snprintf_s(cert_path, MAX_URL_LEN, MAX_URL_LEN - 1, "/tmp%s", slash_index);
        goto_label_do_info_if_expr(ret <= 0, err_quit,
            debug_log(DLOG_ERROR, "format certificate file path failed, ret is %d", ret));

        if (0 != g_strcmp0(TMP_DICE_CERT_FILE, cert_path)) {
            // redfish模块降权限后，调用代理方法拷贝文件
            ret = proxy_copy_file((guchar)from_webui_flag, user_name, client_ip, cert_path, TMP_DICE_CERT_FILE,
                REDFISH_USER_UID, REDFISH_USER_GID, COPY_FILE_DELETE_FLAG);
            if (VOS_OK != ret) {
                debug_log(DLOG_ERROR, "%s: copy request file to %s failed", __FUNCTION__, TMP_DICE_CERT_FILE);
                (void)proxy_delete_file((guchar)from_webui_flag, user_name, client_ip, cert_path);
                (void)proxy_delete_file((guchar)from_webui_flag, user_name, client_ip, TMP_DICE_CERT_FILE);
                goto err_quit;
            }
        }

        ret = dice_cert_import(from_webui_flag, user_name, client_ip, body_jso, message_obj);
        monitor_fn_data->task_progress = (HTTP_OK == ret) ? RF_FINISH_PERCENTAGE : RF_FILE_TRANSFER_PERCENTAGE;
        monitor_fn_data->task_state = (HTTP_OK == ret) ? RF_TASK_COMPLETED : RF_TASK_EXCEPTION;
        json_object_clear_string(body_jso, RFACTION_PARAM_CERTICATE);
        return RF_OK;
    }

err_quit:
    monitor_fn_data->task_state = RF_TASK_EXCEPTION;
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);
    record_dice_operation_log(user_name, client_ip, "Import DICE certificate failed", from_webui_flag);
    json_object_clear_string(body_jso, RFACTION_PARAM_CERTICATE);
    
    return RF_OK;
}


LOCAL gint32 act_dice_cert_import_cert(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    const gchar *cert_str = NULL;
    gint32 ret = VOS_ERR;
    const gchar *type_str = NULL;
    gchar            file_path[MAX_URL_LEN] = {0};
    TASK_MONITOR_INFO_S *monitor_fn_data = NULL;
    guchar cert_file_type = 0;
    gboolean b_ret;

    
    return_val_do_info_if_expr(0 == (USERROLE_SECURITYMGNT & i_paras->user_role_privilege), HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    (void)get_element_str(i_paras->val_jso, RFACTION_PARAM_TYPE, &type_str);
    (void)get_element_str(i_paras->val_jso, RFACTION_PARAM_CERTICATE, &cert_str);
    goto_label_do_info_if_fail(NULL != cert_str, err_exit,
        debug_log(DLOG_ERROR, "cann't get %s from request.", RFACTION_PARAM_CERTICATE));

    b_ret = check_redfish_running_task_exist(dice_cert_import_status_monitor, i_paras->val_jso, i_paras->uri, NULL);
    return_val_do_info_if_fail(FALSE == b_ret, HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "cert import already task exist");
        (void)create_message_info(MSGID_CERT_IMPORTING, NULL, o_message_jso));

    
    type_str = (NULL != type_str) ? type_str : RFACTION_FORMAT_FILE;
    if (0 == g_strcmp0(type_str, RFACTION_FORMAT_TEXT)) {
        return_val_do_info_if_fail(TRUE == b_ret, HTTP_BAD_REQUEST,
            debug_log(DLOG_ERROR, "unsupported property value.");
            (void)create_message_info(MSGID_PROP_TYPE_ERR, RFACTION_PARAM_CERTICATE, o_message_jso, cert_str,
            RFACTION_PARAM_CERTICATE));
    }

    if (0 == g_strcmp0(type_str, RFACTION_FORMAT_FILE)) {
        
        b_ret = g_regex_match_simple(DICE_CERT_IMPORT_REGEX, cert_str, G_REGEX_OPTIMIZE, (GRegexMatchFlags)0);
        return_val_do_info_if_fail(TRUE == b_ret, HTTP_BAD_REQUEST,
            debug_log(DLOG_ERROR, "unsupported certificate file path");
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFACTION_PARAM_CERTICATE, o_message_jso, RF_SENSITIVE_INFO,
            RFACTION_PARAM_CERTICATE));
    }

    
    ret = rf_save_import_content_to_file(i_paras, type_str, cert_str, DICE_CERT_TYPE, file_path, MAX_URL_LEN,
        &cert_file_type, o_message_jso);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "save import content to local file failed, ret is %d", ret));

    
    if (cert_file_type != FILE_TYPE_URI) {
        ret = dal_check_real_path((const gchar *)file_path);
        goto_label_do_info_if_expr(VOS_OK != ret, err_exit,
            debug_log(DLOG_ERROR, "check real path failed, return %d", ret));

        if (cert_file_type == FILE_TYPE_LOCAL && !dal_check_file_opt_user(i_paras->auth_type,
            (const gchar *)i_paras->user_name, (const gchar *)file_path, (const gchar *)i_paras->user_roleid)) {
            (void)create_message_info(MSGID_NO_PRIV_OPT_FILE, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        }
        if (0 != g_strcmp0(TMP_DICE_CERT_FILE, file_path)) {
            // redfish模块降权限后，调用代理方法拷贝文件
            ret = proxy_copy_file((guchar)i_paras->is_from_webui, i_paras->user_name, i_paras->client, file_path,
                TMP_DICE_CERT_FILE, REDFISH_USER_UID, REDFISH_USER_GID, COPY_FILE_DELETE_FLAG);
            goto_label_do_info_if_expr(VOS_OK != ret, err_exit,
                debug_log(DLOG_ERROR, "%s: copy request file to %s failed", __FUNCTION__, TMP_DICE_CERT_FILE);
                (void)proxy_delete_file((guchar)i_paras->is_from_webui, i_paras->user_name, i_paras->client, file_path);
                (void)proxy_delete_file((guchar)i_paras->is_from_webui, i_paras->user_name, i_paras->client,
                TMP_DICE_CERT_FILE));
        }
        return dice_cert_import(i_paras->is_from_webui, i_paras->user_name, i_paras->client, i_paras->val_jso,
            o_message_jso);
    }

    monitor_fn_data = task_monitor_info_new((GDestroyNotify)json_object_put);
    monitor_fn_data->user_data = json_object_new_object();
    goto_label_do_info_if_expr(NULL == monitor_fn_data->user_data, err_exit,
        debug_log(DLOG_ERROR, "dup file path failed");
        task_monitor_info_free(monitor_fn_data));

    json_object_object_add((json_object *)monitor_fn_data->user_data, RF_LOG_USER_NAME,
        json_object_new_string(i_paras->user_name));
    json_object_object_add((json_object *)monitor_fn_data->user_data, RF_LOG_USER_IP,
        json_object_new_string(i_paras->client));
    json_object_object_add((json_object *)monitor_fn_data->user_data, RF_USERDATA_FROM_WEBUI_FLAG,
        json_object_new_int(i_paras->is_from_webui));

    monitor_fn_data->rsc_privilege = USERROLE_SECURITYMGNT;
    ret = create_new_task("dice_cert_import", dice_cert_import_status_monitor, monitor_fn_data, i_paras->val_jso,
        i_paras->uri, o_result_jso);
    goto_label_do_info_if_expr(VOS_OK != ret, err_exit, task_monitor_info_free(monitor_fn_data));

    return HTTP_ACCEPTED;

err_exit:
    parse_dice_cert_import_result_code(ret, o_message_jso);
    record_dice_operation_log(i_paras->user_name, i_paras->client, "Import DICE certificate failed",
        i_paras->is_from_webui);

    return HTTP_BAD_REQUEST;
}


LOCAL gint32 act_dice_cert_import_cert_entry(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    if (i_paras == NULL || i_paras->val_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: input param is invalid.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (o_message_jso == NULL || o_result_jso == NULL || i_paras == NULL) {
        debug_log(DLOG_ERROR, "%s: input param is invalid.", __FUNCTION__);
        delete_user_specific_file(i_paras, i_paras->val_jso, RFACTION_PARAM_IMAGE_URI);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_IMAGE_URI);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = act_dice_cert_import_cert(i_paras, o_message_jso, o_result_jso);
    if (ret != HTTP_ACCEPTED) {
        delete_user_specific_file(i_paras, i_paras->val_jso, RFACTION_PARAM_IMAGE_URI);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_IMAGE_URI);
    }
    return ret;
}

LOCAL PROPERTY_PROVIDER_S g_dice_cert_provider[] = {
    {
        RFPROP_ODATA_ID,
        "", "",
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_dice_cert_odataid, // 证书资源ID
        NULL,
        NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_ODATA_CONTEXT,
        "", "",
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_dice_cert_odata_context, // 证书资源odata_context
        NULL,
        NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_ACTIONS,
        "", "",
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_dice_cert_actions, // 证书资源的Actions链接
        NULL,
        NULL,
        ETAG_FLAG_DISABLE
    },
    {
        RFACTION_DICE_CERT_IMPORT_CERT,
        "", "",
        USERROLE_SECURITYMGNT,
        SYS_LOCKDOWN_FORBID,
        NULL,
        NULL,
        act_dice_cert_import_cert_entry, // 导入DICE证书
        ETAG_FLAG_ENABLE
    },
    {
        RFACTION_DICE_CERT_EXPORT_CSR,
        "", "",
        USERROLE_SECURITYMGNT,
        SYS_LOCKDOWN_FORBID,
        NULL,
        NULL,
        act_dice_cert_export_csr, // 导出CSR
        ETAG_FLAG_ENABLE
    },
    {
        RFACTION_DICE_CERT_EXPORT_CERT_CHAIN,
        "", "",
        USERROLE_SECURITYMGNT,
        SYS_LOCKDOWN_FORBID,
        NULL,
        NULL,
        act_dice_cert_export_cert_chain, // 导出证书链
        ETAG_FLAG_ENABLE
  }
};


gint32 dice_cert_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gboolean b_ret;

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count), VOS_ERR,
        debug_log(DLOG_ERROR, "input param error"));

    b_ret = redfish_check_manager_uri_valid(i_paras->uri); // 校验用户输入的URI是否有效
    if (TRUE == b_ret) {
        *prop_provider = g_dice_cert_provider;
        *count = sizeof(g_dice_cert_provider) / sizeof(PROPERTY_PROVIDER_S);
        return VOS_OK;
    } else {
        return HTTP_NOT_FOUND;
    }
}


LOCAL gint32 get_dice_cert_actioninfo_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar               dice_cert_uri[MAX_URI_LENGTH] = {0};
    gchar               action_info_uri[MAX_URI_LENGTH] = {0};
    gchar               slot_id[MAX_RSC_ID_LEN] = {0};
    gint32 ret;

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_message_jso) && (NULL != o_result_jso),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    (void)redfish_get_board_slot(slot_id, MAX_RSC_ID_LEN);

    ret = snprintf_s(dice_cert_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, MANAGER_HTTPS_CERT_URI, slot_id);
    return_val_do_info_if_fail(ret > 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "format string failed, ret is %d", ret));

    ret = rf_update_rsc_actioninfo_uri(g_dice_cert_action_array, G_N_ELEMENTS(g_dice_cert_action_array), i_paras->uri,
        dice_cert_uri, action_info_uri, MAX_URI_LENGTH);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "update action info uri (%s) failed", i_paras->uri));

    *o_result_jso = json_object_new_string((const gchar *)action_info_uri);

    return HTTP_OK;
}

LOCAL PROPERTY_PROVIDER_S g_dice_cert_action_info_provider[] = {
    {
        RFPROP_ODATA_ID,
        "", "",
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_dice_cert_actioninfo_odataid,
        NULL,
        NULL,
        ETAG_FLAG_ENABLE
    }
};


gint32 dice_cert_actioninfo_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gboolean b_ret;

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count), VOS_ERR,
        debug_log(DLOG_ERROR, "input param error"));

    b_ret = redfish_check_manager_uri_valid(i_paras->uri); // 校验用户输入的URI是否有效
    if (TRUE == b_ret) {
        *prop_provider = g_dice_cert_action_info_provider;
        *count = sizeof(g_dice_cert_action_info_provider) / sizeof(PROPERTY_PROVIDER_S);
        return VOS_OK;
    } else {
        return HTTP_NOT_FOUND;
    }
}
#endif
