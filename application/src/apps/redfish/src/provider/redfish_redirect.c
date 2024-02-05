
#include "pme/pme.h"
#include "pme_app/pme_app.h"

#include "redfish_http.h"
#include "redfish_message.h"
#include "redfish_provider.h"
#include "redfish_defs.h"
#include "redfish_class.h"
#include "json_patch_util.h"

#define REGEX_WHITE_LIST_STRING "^/redfish/v1/Sms/\\d+/URIWhiteList.*$"

LOCAL gint32 sms_get_ipinfo(guint8 sms_id, gchar str_buf[], guint32 str_len);
LOCAL gint32 sms_generate_response(SMS_INFO_S *info, HTTP_RESPONSE_DATA_S *resp_data);
LOCAL gint32 sms_generate_message(SMS_INFO_S *info, gint32 resp_code, json_object *jso_msg);
LOCAL gint32 sms_info_node_remove(SMS_INFO_S *info);
LOCAL gint32 sms_info_node_insert(SMS_INFO_S *info);
LOCAL SMS_INFO_S *sms_info_find(guint8 sms_id);
LOCAL void sms_info_free(SMS_INFO_S *info);
LOCAL gint32 sms_info_flush(SMS_INFO_S *info);
LOCAL SMS_INFO_S *sms_info_new(guint8 sms_id);
LOCAL gint32 sms_parse_uri(const gchar *uri, SMS_INFO_S **sms_info);
LOCAL gint32 sms_parse_request(const gchar *i_request_str, gint32 i_request_len, SMS_INFO_S **sms_info);
LOCAL gint32 sms_info_verify(SMS_INFO_S *info);
LOCAL gint32 sms_info_match_white_list(SMS_INFO_S *info, json_object *jso_array_white);
LOCAL gboolean sms_check_not_in_white_list(const gchar *str_regex, json_object *jso_array_white);
LOCAL gint32 sms_create_white_list(SMS_INFO_S *info);
LOCAL gint32 sms_delete_white_list(SMS_INFO_S *info);
LOCAL gint32 sms_call_forward(SMS_INFO_S *info, HTTP_RESPONSE_DATA_S *resp_data);
LOCAL gint32 sms_process_response(SMS_INFO_S *info, HTTP_RESPONSE_DATA_S *resp_data);

LOCAL SMS_INFO_S *sms_info_list_head = NULL;

G_LOCK_DEFINE(g_sms_host_lock);
LOCAL pthread_mutex_t g_sms_redirect_req_lock = PTHREAD_MUTEX_INITIALIZER;


LOCAL gint32 get_session_jso_by_xauthtoken(const gchar *x_auth_token, json_object **session_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    GSList *caller_info = NULL;
    gchar *session_sha = NULL;
    const gchar *session_str = NULL;

    return_val_do_info_if_fail((NULL != x_auth_token) && (NULL != session_jso), VOS_ERR,
        debug_log(DLOG_DEBUG, "[%s]: input parameters error", __FUNCTION__));

    ret = dal_get_object_handle_nth(CLASS_SESSION, 0, &obj_handle);
    return_val_if_fail(VOS_OK == ret, ret);

    // 计算x-auth-token值的校验和SHA256加密值: x_auth_token --> session_sha --> session_id
    session_sha = g_compute_checksum_for_string(G_CHECKSUM_SHA256, x_auth_token, -1);
    return_val_do_info_if_fail((NULL != session_sha) && (strlen(session_sha) >= SESSION_ID_LEN), VOS_ERR,
        debug_log(DLOG_DEBUG, "[%s]: get %s from request info fail", __FUNCTION__, REQ_HEADER_X_AUTH_TOKEN));

    // 截断取16个字符,即为session_id
    guint32 session_len = strlen(session_sha);
    session_sha[SESSION_ID_LEN] = '\0';

    // 调用user模块"GetOnlineUser"方法
    input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)session_sha));
    caller_info = g_slist_append(caller_info, g_variant_new_string(USER_LOGIN_INTERFACE_REDFISH_STRING));
    caller_info = g_slist_append(caller_info, g_variant_new_int32(REDFISH_REFRESH));
    clear_sensitive_info(session_sha, session_len);
    g_free(session_sha);

    ret = dfl_call_class_method(obj_handle, METHOD_SESSION_GETONLINEUSER, caller_info, input_list, &output_list);

    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    g_slist_free_full(caller_info, (GDestroyNotify)g_variant_unref);

    return_val_do_info_if_fail((VOS_OK == ret), VOS_ERR,
        debug_log(DLOG_DEBUG, "[%s]: dfl_call_class_method %s fail", __FUNCTION__, METHOD_SESSION_GETONLINEUSER));

    session_str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    return_val_do_info_if_fail((NULL != session_str) && (0 != strlen(session_str)), VOS_ERR,
        debug_log(DLOG_ERROR, "[%s]: get session info fail", __FUNCTION__);
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref));

    *session_jso = json_tokener_parse(session_str);

    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

    return VOS_OK;
}


LOCAL gint32 sms_get_ipinfo(guint8 sms_id, gchar str_buf[], guint32 str_len)
{
    int iRet = -1;
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    gchar sms_ipaddr[MAX_IPADDR_LEN] = {0};
    guint32 sms_port = 0;
    return_val_do_info_if_fail((sms_id > 0) && (NULL != str_buf), VOS_ERR,
        debug_log(DLOG_ERROR, "[%s]: input params error.", __FUNCTION__));
    ret = dal_get_specific_object_byte(CLASS_SMS, PROPERTY_SMS_ID, sms_id, &obj_handle);
    return_val_do_info_if_fail((VOS_OK == ret), ret,
        debug_log(DLOG_DEBUG, "[%s]: get object of sms/%d error", __FUNCTION__, sms_id));

    (void)memset_s(str_buf, str_len, 0, str_len);

    if (VOS_OK == dal_get_property_value_string(obj_handle, PROPERTY_SMS_IPADDR, sms_ipaddr, sizeof(sms_ipaddr)) &&
        (strlen(sms_ipaddr) > 0) && VOS_OK == dal_get_property_value_uint32(obj_handle, PROPERTY_SMS_PORT, &sms_port) &&
        (sms_port > 0)) {
        debug_log(DLOG_DEBUG, "[%s]: ip:%s, port:%d", __FUNCTION__, sms_ipaddr, sms_port);
        return_val_do_info_if_expr(0 == str_len, VOS_ERR,
            debug_log(DLOG_ERROR, "%s failed:input param error, str_len is 0.", __FUNCTION__));
        iRet = snprintf_s(str_buf, str_len, str_len - 1, "%s:%u", sms_ipaddr, sms_port);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
        return VOS_OK;
    }

    return VOS_ERR;
}


LOCAL gint32 sms_generate_response(SMS_INFO_S *info, HTTP_RESPONSE_DATA_S *resp_data)
{
    if (info == NULL || resp_data == NULL) {
        debug_log(DLOG_ERROR, "%s: input param invalid", __FUNCTION__);
        return VOS_ERR;
    }

    if (NULL == info->jso_response) {
        sms_base_error(info, HTTP_INTERNAL_SERVER_ERROR, MSGID_INTERNAL_ERR, NULL);
        debug_log(DLOG_ERROR, "[%s]: info->jso_response is NULL", __FUNCTION__);
    }

    generate_redfish_resp_data_from_jso(info->jso_response, resp_data);
    return VOS_OK;
}


LOCAL gint32 sms_generate_message(SMS_INFO_S *info, gint32 resp_code, json_object *jso_msg)
{
    json_object *jso_body = NULL;
    json_object *jso_error = NULL;
    json_object *jso_extend = NULL;
    json_object *jso_headers = NULL;
    json_object *jso_status = NULL;
    // json_bool b_ret;
    gint32 ret;
    return_val_do_info_if_fail((NULL != info), VOS_ERR,
        debug_log(DLOG_ERROR, "[%s]: input params error.", __FUNCTION__));

    // 重新生成jso_rsponse
    if (NULL != info->jso_response) {
        json_object_put(info->jso_response);
    }

    info->jso_response = json_object_new_object();
    debug_log(DLOG_DEBUG, "[%s]: new jso_response", __FUNCTION__);

    if (NULL != jso_msg) {
        // ResponseBody
        jso_body = json_object_new_object();
        json_object_object_add(info->jso_response, RESPONSE_BODY, jso_body);
        debug_log(DLOG_DEBUG, "[%s]: new %s", __FUNCTION__, RESPONSE_BODY);
        // ResponseBody->error
        jso_error = json_object_new_object();
        json_object_object_add(jso_body, ERROR_KEY, jso_error);
        debug_log(DLOG_DEBUG, "[%s]: new %s.%s", __FUNCTION__, RESPONSE_BODY, ERROR_KEY);
        json_object_object_add(jso_error, CODE_KEY, json_object_new_string(CODE_VAL_GENERAL_STRING));
        json_object_object_add(jso_error, T_MESSAGE_KEY, json_object_new_string(MESSAGE_VAL_GENERAL_STRING));
        // ResponseBody->error->@Message.ExtendedInfo[]
        jso_extend = json_object_new_array();
        json_object_object_add(jso_error, EXTENDED_INFO_KEY, jso_extend);
        debug_log(DLOG_DEBUG, "[%s]: new %s.%s.%s", __FUNCTION__, RESPONSE_BODY, ERROR_KEY, EXTENDED_INFO_KEY);

        json_object_array_add(jso_extend, jso_msg);
    }

    // ResponseBody->ResponseHeaders[]
    jso_headers = json_object_new_array();
    json_object_object_add(info->jso_response, RESPONSE_HEADERS, jso_headers);
    debug_log(DLOG_DEBUG, "[%s]: new %s", __FUNCTION__, RESPONSE_HEADERS);

    ret = generate_common_response_headers(&jso_headers, info->redirect_uri, jso_extend);

    return_val_do_info_if_fail((VOS_OK == ret), VOS_ERR,
        debug_log(DLOG_ERROR, "[%s]:generate %s failed", __FUNCTION__, RESPONSE_HEADERS));

    // ResponseBody->ResponseStatusCode
    ret = generate_response_status(info->resp_code, &jso_status, jso_extend);
    return_val_do_info_if_fail((VOS_OK == ret), VOS_ERR,
        debug_log(DLOG_ERROR, "[%s]:generate %s failed", __FUNCTION__, RESPONSE_STATUS_CODE));
    json_object_object_add(info->jso_response, RESPONSE_STATUS_CODE, jso_status);
    debug_log(DLOG_DEBUG, "[%s]: new %s", __FUNCTION__, RESPONSE_STATUS_CODE);

    return VOS_OK;
}


gint32 sms_base_message(SMS_INFO_S *info, gint32 resp_code, const gchar *msg_id, const gchar *related_property, ...)
{
    json_object *jso_msg = NULL;
    gint32 ret;
    /*lint -save -e438 pclint*/
    va_list args;

    return_val_do_info_if_fail((NULL != info), VOS_ERR,
        debug_log(DLOG_ERROR, "[%s]: input params error.", __FUNCTION__));

    info->resp_code = resp_code;

    if (NULL != msg_id) {
        va_start(args, related_property);
        ret = create_message_vinfo(&jso_msg, msg_id, related_property, args);
        va_end(args);
        return_val_do_info_if_fail((VOS_OK == ret), VOS_ERR,
            debug_log(DLOG_ERROR, "[%s]:create message %d %s failed", __FUNCTION__, resp_code, msg_id));
    }

    ret = sms_generate_message(info, resp_code, jso_msg);

    return_val_do_info_if_fail((VOS_OK == ret), VOS_ERR,
        debug_log(DLOG_ERROR, "[%s]:get %s.%s.%s[] failed", __FUNCTION__, RESPONSE_BODY, ERROR_KEY, EXTENDED_INFO_KEY));

    return VOS_OK;
}


gint32 sms_ibmc_message(SMS_INFO_S *info, gint32 resp_code, const gchar *msg_id, const gchar *related_property, ...)
{
    json_object *jso_msg = NULL;
    gint32 ret;
    /*lint -save -e438 pclint*/
    va_list args;

    return_val_do_info_if_fail((NULL != info), VOS_ERR,
        debug_log(DLOG_ERROR, "[%s]: input params error.", __FUNCTION__));

    info->resp_code = resp_code;

    if (NULL != msg_id) {
        va_start(args, related_property);
        ret = create_message_vinfo(&jso_msg, msg_id, related_property, args);
        va_end(args);
        return_val_do_info_if_fail((VOS_OK == ret), VOS_ERR,
            debug_log(DLOG_ERROR, "[%s]:create message %d %s failed", __FUNCTION__, resp_code, msg_id));
    }

    ret = sms_generate_message(info, resp_code, jso_msg);

    return_val_do_info_if_fail((VOS_OK == ret), VOS_ERR,
        debug_log(DLOG_ERROR, "[%s]:get %s.%s.%s[] failed", __FUNCTION__, RESPONSE_BODY, ERROR_KEY, EXTENDED_INFO_KEY));

    return VOS_OK;
}


LOCAL gint32 sms_info_node_remove(SMS_INFO_S *info)
{
    SMS_INFO_S *node = NULL;

    return_val_do_info_if_fail((NULL != info), VOS_ERR,
        debug_log(DLOG_ERROR, "[%s]: Input params error.", __FUNCTION__));

    if (info == sms_info_list_head) {
        sms_info_list_head = info->next;
        return VOS_OK;
    }

    for (node = sms_info_list_head; NULL != node; node = node->next) {
        if (node->next == info) {
            node->next = info->next;
            info->next = NULL;
            return VOS_OK;
        }
    }

    return VOS_ERR;
}

LOCAL gint32 sms_info_node_insert(SMS_INFO_S *info)
{
    return_val_do_info_if_fail((NULL != info), VOS_ERR,
        debug_log(DLOG_ERROR, "[%s]: Input params error.", __FUNCTION__));

    info->next = sms_info_list_head;
    sms_info_list_head = info;
    return VOS_OK;
}

LOCAL SMS_INFO_S *sms_info_find(guint8 sms_id)
{
    SMS_INFO_S *node = NULL;
    return_val_do_info_if_fail((sms_id > 0), NULL, debug_log(DLOG_ERROR, "[%s]: Input params error.", __FUNCTION__));

    for (node = sms_info_list_head; NULL != node; node = node->next) {
        if (node->id == sms_id) {
            debug_log(DLOG_DEBUG, "[%s]: Find sms host %d: %s.", __FUNCTION__, node->id, node->host);
            return node;
        }
    }

    return NULL;
}


LOCAL void sms_info_free(SMS_INFO_S *info)
{
    return_do_info_if_fail((NULL != info), debug_log(DLOG_ERROR, "[%s]: Input params error.", __FUNCTION__));

    (void)sms_info_node_remove(info);

    if (info->jso_white_list) {
        json_object_put(info->jso_white_list);
        info->jso_white_list = NULL;
    }

    g_free(info);
}


LOCAL gint32 sms_info_flush(SMS_INFO_S *info)
{
    return_val_do_info_if_fail((NULL != info), VOS_ERR,
        debug_log(DLOG_ERROR, "[%s]: Input params error.", __FUNCTION__));

    if (NULL != info->jso_request) {
        json_object_clear_string(info->jso_request, REQUEST_BODY);
        json_object_put(info->jso_request);
        info->jso_request = NULL;
    }

    if (NULL != info->jso_response) {
        json_object_put(info->jso_response);
        info->jso_response = NULL;
    }

    info->resp_code = 0;

    (void)memset_s(info->method, sizeof(info->method), 0, sizeof(info->method));

    (void)memset_s(info->session_id, sizeof(info->session_id), 0, sizeof(info->session_id));

    (void)memset_s(info->relative_uri, sizeof(info->relative_uri), 0, sizeof(info->relative_uri));

    (void)memset_s(info->redirect_uri, sizeof(info->redirect_uri), 0, sizeof(info->redirect_uri));

    return VOS_OK;
}

LOCAL SMS_INFO_S *sms_info_new(guint8 sms_id)
{
    SMS_INFO_S *info = NULL;
    guint32 ret;
    guint32 i;
    guint32 prop_len;
    gsize parm_temp = 0;
    GVariant *prop_val = NULL;
    const gchar **white_list = NULL;

    return_val_do_info_if_fail((sms_id > 0), NULL, debug_log(DLOG_ERROR, "[%s]: Input params error.", __FUNCTION__));

    info = (SMS_INFO_S *)g_malloc(sizeof(SMS_INFO_S));

    return_val_do_info_if_fail((NULL != info), NULL,
        debug_log(DLOG_ERROR, "[%s]: Allocate memory for info failed", __FUNCTION__));

    (void)memset_s(info, sizeof(SMS_INFO_S), 0, sizeof(SMS_INFO_S));

    info->id = sms_id;

    // 获取BMA的Host信息: "192.168.45.55:40443"
    ret = sms_get_ipinfo(sms_id, info->host, sizeof(info->host));

    goto_label_do_info_if_fail(VOS_OK == ret, __error,
        debug_log(DLOG_INFO, "[%s]: Get ipinfo of sms/%d %s error", __FUNCTION__, info->id, info->redirect_uri));

    info->jso_white_list = json_object_new_array();

    
    (void)dal_get_specific_object_byte(CLASS_SMS, PROPERTY_SMS_ID, sms_id, &info->sms_obj_handle);
    ret = dfl_get_property_value(info->sms_obj_handle, PROPERTY_SMS_REDIRECT_WHITELIST, &prop_val);
    goto_if_expr(ret != DFL_OK, __error);

    white_list = g_variant_get_strv(prop_val, &parm_temp);
    prop_len = (guint32)parm_temp;
    goto_label_do_info_if_expr(NULL == white_list, __error,
        debug_log(DLOG_ERROR, "[%s]: g_variant_get_strv failed", __FUNCTION__);
        g_variant_unref(prop_val));

    for (i = 0; i < prop_len; i++) {
        json_object_array_add(info->jso_white_list, json_object_new_string(white_list[i]));
    }
    g_free(white_list);
    g_variant_unref(prop_val);
    

    debug_log(DLOG_DEBUG, "[%s]: sms/%d, uri: %s", __FUNCTION__, info->id, info->redirect_uri);

    (void)sms_info_node_insert(info);

    return info;
__error:

    if (NULL != info->jso_white_list) {
        json_object_put(info->jso_white_list);
    }

    if (NULL != info) {
        g_free(info);
    }

    return NULL;
}


gint32 set_default_white_list(OBJ_HANDLE obj_handle, gpointer user_data, gchar *property_name, GVariant *property_value)
{
    gchar sms_type[PROP_VAL_MAX_LENGTH] = {0};
    guint8 register_state;
    GSList *input_list = NULL;
    gint32 ret;
    const gchar* default_white_list[] = {
        "^/redfish/v1/Sms/\\d+/UpdateService.*$",
        "^/redfish/v1/Sms/\\d+/TaskService.*$"
    };

    (void)dal_get_property_value_string(obj_handle, PROPERTY_SMS_TYPE, sms_type, PROP_VAL_MAX_LENGTH);
    return_val_if_expr(g_strcmp0(PROPERTY_VALUE_SMS_TYPE_INBAND, sms_type), VOS_OK);

    register_state = g_variant_get_byte(property_value);
    return_val_if_fail(0 == register_state, VOS_OK);

    input_list = g_slist_append(input_list, g_variant_new_strv((const gchar * const *)default_white_list, 2));
    ret = dfl_call_class_method(obj_handle, METHOD_SMS_SET_REDIRECT_WHITELIST, NULL, input_list, NULL);
    do_if_fail(DFL_OK == ret,
        debug_log(DLOG_ERROR, "%s:call %s fail,ret = %d", __FUNCTION__, METHOD_SMS_SET_REDIRECT_WHITELIST, ret));

    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    do_if_expr(DFL_OK == ret, debug_log(DLOG_INFO, "%s:set default white list.", __FUNCTION__));

    return VOS_OK;
}


gint32 ConfigureSmsHost(OBJ_HANDLE obj_handle, gpointer user_data, gchar *property_name, GVariant *property_value)
{
    SMS_INFO_S *info = NULL;
    guint8 sms_id = 0;
    gint32 ret;

    return_val_do_info_if_fail((0 != obj_handle), VOS_ERR,
        debug_log(DLOG_ERROR, "[%s]: Input params error.", __FUNCTION__));

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SMS_ID, &sms_id);

    return_val_do_info_if_fail((VOS_OK == ret) && (sms_id > 0), VOS_ERR,
        debug_log(DLOG_ERROR, "[%s]: get sms id failed.", __FUNCTION__));
    G_LOCK(g_sms_host_lock);

    info = sms_info_find(sms_id);
    if (NULL == info) {
        info = sms_info_new(sms_id);
    }

    return_val_do_info_if_fail((NULL != info), VOS_ERR, G_UNLOCK(g_sms_host_lock);
        debug_log(DLOG_INFO, "[%s]: Allocate memory for sms failed", __FUNCTION__));

    // 获取BMA的Host信息: "192.168.45.55:40443"
    ret = sms_get_ipinfo(info->id, info->host, sizeof(info->host));

    do_val_if_fail(VOS_OK == ret,
        debug_log(DLOG_DEBUG, "[%s]: sms/%d exist but can not get host info", __FUNCTION__, info->id);
        sms_info_free(info));

    G_UNLOCK(g_sms_host_lock);

    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_DEBUG, "[%s]: Get ipinfo of sms/%d error", __FUNCTION__, sms_id));
    return VOS_OK;
}


LOCAL gint32 sms_parse_uri(const gchar *uri, SMS_INFO_S **sms_info)
{
    int iRet = -1;
    SMS_INFO_S *info = NULL;
    guint32 ret = VOS_ERR;
    gboolean b_ret;
    GRegex *regex = NULL;
    GMatchInfo *match_info = NULL;
    gchar *str_uri = NULL;
    gchar *str_id = NULL;
    guint32 n;
    guint32 regex_num;
    guint8 sms_id = 0;

    return_val_do_info_if_fail((NULL != uri) && (NULL != sms_info), VOS_ERR,
        debug_log(DLOG_ERROR, "[%s]: Input params error.", __FUNCTION__));

    str_uri = g_ascii_strdown(uri, strlen(uri));

    goto_label_do_info_if_fail((NULL != str_uri), __exit,
        debug_log(DLOG_ERROR, "[%s]: Allocate memory for uri failed", __FUNCTION__));

    regex = g_regex_new(REF_SMS_REGEX_MATCH, (GRegexCompileFlags)0, (GRegexMatchFlags)0, NULL);

    goto_label_do_info_if_fail(NULL != regex, __exit,
        debug_log(DLOG_ERROR, "[%s]: Allocate a new regex failed", __FUNCTION__));

    b_ret = g_regex_match(regex, str_uri, (GRegexMatchFlags)0, &match_info);
    
    goto_label_do_info_if_fail(TRUE == b_ret, __exit,
        debug_log(DLOG_DEBUG, "[%s]: The regex match for uri %s failed", __FUNCTION__, str_uri));
    
    
    regex_num = g_match_info_get_match_count(match_info);

    debug_log(DLOG_DEBUG, "[%s]: regex match count: %d", __FUNCTION__, regex_num);

    goto_label_do_info_if_fail((2 == regex_num), __exit,
        debug_log(DLOG_DEBUG, "[%s]: Regex match count error: %d", __FUNCTION__, regex_num));

    str_id = g_match_info_fetch(match_info, 1);

    goto_label_do_info_if_fail((NULL != str_id) && (VOS_OK == vos_str2int(str_id, 10, &sms_id, NUM_TPYE_UCHAR)), __exit,
        debug_log(DLOG_DEBUG, "[%s]: Match sms id from %s error", __FUNCTION__, str_id));

    info = sms_info_find(sms_id);
    if (NULL == info) {
        info = sms_info_new(sms_id);
    }

    goto_label_do_info_if_fail((NULL != info), __exit,
        debug_log(DLOG_ERROR, "[%s]: Allocate memory for info failed", __FUNCTION__));

    sms_info_flush(info);
    // 记录至info->relative_uri
    strncpy_s(info->relative_uri, sizeof(info->relative_uri), uri, sizeof(info->relative_uri) - 1);

    // URI重定向
    n = strlen(SMS_PREFIX) + strlen(str_id);
    if ('\0' != uri[n]) {
        
        iRet = snprintf_s(info->redirect_uri, sizeof(info->redirect_uri), sizeof(info->redirect_uri) - 1, "%s/%s/%u%s",
            SERVICE_ROOT_PATH, SMS_TAG, sms_id, &uri[n]);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
        
    } else {
        strncpy_s(info->redirect_uri, sizeof(info->redirect_uri), SERVICE_ROOT_PATH, strlen(SERVICE_ROOT_PATH));
    }

    // 移除最后的'/'符
    n = strlen(info->redirect_uri);
    if ((n > 0) && ('/' == info->redirect_uri[n - 1])) {
        info->redirect_uri[n - 1] = '\0';
    }

    debug_log(DLOG_DEBUG, "[%s]: sms/%d info->redirect_uri: %s", __FUNCTION__, info->id, info->redirect_uri);

    *sms_info = info;

    ret = VOS_OK;

__exit:

    if (NULL != str_uri) {
        g_free(str_uri);
    }

    if (NULL != str_id) {
        g_free(str_id);
    }

    if (NULL != match_info) {
        g_match_info_free(match_info);
    }

    if (NULL != regex) {
        g_regex_unref(regex);
    }

    return ret;
}

LOCAL gint32 sms_parse_request(const gchar *i_request_str, gint32 i_request_len, SMS_INFO_S **sms_info)
{
    json_object *request_jso = NULL;

    json_object *headers_jso = NULL;
    json_object *jso_new = NULL;
    json_object *jso_val = NULL;

    const gchar *str_tmp = NULL;

    SMS_INFO_S *info = NULL;
    gint32 ret = VOS_ERR;

    gchar request_ip[MAX_IPADDR_LEN];

    return_val_do_info_if_fail((NULL != i_request_str) && (i_request_len > 0) && (NULL != sms_info), VOS_ERR,
        debug_log(DLOG_ERROR, "[%s]: input param error", __FUNCTION__));

    // 转换请求字符串为JSON对象
    request_jso = json_tokener_parse(i_request_str);

    return_val_do_info_if_fail(request_jso != NULL, VOS_ERR, debug_log(DLOG_ERROR, "%s:json parse fail", __FUNCTION__));

    // "RelativeUri": "/redfish/v1/..."
    json_bool b_ret = json_object_object_get_ex(request_jso, RELATIVE_URI, &jso_val);

    goto_label_do_info_if_fail((b_ret && (json_type_string == json_object_get_type(jso_val))), __exit,
        debug_log(DLOG_ERROR, "[%s]: get %s error", __FUNCTION__, RELATIVE_URI));

    str_tmp = dal_json_object_get_str(jso_val);

    goto_label_do_info_if_fail((NULL != str_tmp), __exit,
        debug_log(DLOG_ERROR, "[%s]: dal_json_object_get_str failed", __FUNCTION__));

    // 获取(查找或新建)有效的sms_info
    goto_label_do_info_if_fail((VOS_OK == sms_parse_uri(str_tmp, &info)) && (NULL != info), __exit,
        debug_log(DLOG_DEBUG, "[%s]: Get ipinfo of %s error", __FUNCTION__, str_tmp));

    // 添加/覆盖RelativeUri
    json_object_object_add(request_jso, RELATIVE_URI, json_object_new_string(info->redirect_uri));

    // 获取"RequestMethod"
    b_ret = json_object_object_get_ex(request_jso, REQUEST_METHOD, &jso_val);

    goto_label_do_info_if_fail((b_ret && (json_type_string == json_object_get_type(jso_val))), __exit,
        debug_log(DLOG_ERROR, "[%s]: get %s error", __FUNCTION__, REQUEST_METHOD));

    str_tmp = dal_json_object_get_str(jso_val);
    goto_label_do_info_if_fail((NULL != str_tmp), __exit,
        debug_log(DLOG_ERROR, "[%s]: dal_json_object_get_str failed", __FUNCTION__));
    // 记录至info->method
    strncpy_s(info->method, sizeof(info->method), str_tmp, strlen(str_tmp));

    // 获取Headers: "RequestHeaders" : [{"Host":"192.168.45.55:40443"},...]
    b_ret = json_object_object_get_ex(request_jso, REQUEST_HEADERS, &headers_jso);

    goto_label_do_info_if_fail((b_ret && (json_type_array == json_object_get_type(headers_jso))), __exit,
        debug_log(DLOG_ERROR, "[%s]: get %s error", __FUNCTION__, REQUEST_HEADERS));
    // 添加/覆盖RequestHeaders.SmsId
    jso_new = json_object_new_object();
    goto_label_do_info_if_fail((NULL != jso_new), __exit,
        debug_log(DLOG_ERROR, "[%s]: new jso for %s error", __FUNCTION__, REQUEST_SMS));

    json_object_object_add(jso_new, REQUEST_SMS, json_object_new_int(info->id));
    json_object_array_add(headers_jso, jso_new);
    debug_log(DLOG_DEBUG, "[%s]: add header %s", __FUNCTION__, dal_json_object_to_json_string(jso_new));
    jso_new = NULL;

    // 添加/覆盖RequestHeaders.Host
    gint nth = ex_json_object_array_get_object_item_nth(headers_jso, REQ_HEADER_HOST);

    goto_label_do_info_if_expr((nth < 0), __exit,
        debug_log(DLOG_ERROR, "[%s]: get %s error", __FUNCTION__, REQ_HEADER_HOST));

    jso_new = json_object_new_object();
    goto_label_do_info_if_fail((NULL != jso_new), __exit,
        debug_log(DLOG_ERROR, "[%s]: new jso for %s error", __FUNCTION__, REQ_HEADER_HOST));

    json_object_object_add(jso_new, REQ_HEADER_HOST, json_object_new_string(info->host));
    json_object_array_put_idx(headers_jso, nth, jso_new);
    debug_log(DLOG_DEBUG, "[%s]: add header %s", __FUNCTION__, dal_json_object_to_json_string(jso_new));

    
    jso_new = NULL;
    ret = json_custom_get_elem_str(request_jso, REQUEST_IP, request_ip, MAX_IPADDR_LEN);
    goto_label_do_info_if_fail((VOS_OK == ret), __exit,
        debug_log(DLOG_ERROR, "[%s]: parse %s error", __FUNCTION__, REQ_HEADER_REQUESTER_ADDR));

    jso_new = json_object_new_object();
    goto_label_do_info_if_fail((NULL != jso_new), __exit,
        debug_log(DLOG_ERROR, "[%s]: new jso for %s error", __FUNCTION__, REQ_HEADER_REQUESTER_ADDR));
    json_object_object_add(jso_new, REQ_HEADER_REQUESTER_ADDR, json_object_new_string((const gchar *)request_ip));
    json_object_array_add(headers_jso, jso_new);
    

    // 将request_jso移至sms_info
    info->jso_request = json_object_get(request_jso);

    *sms_info = info;

    ret = VOS_OK;

__exit:

    if (request_jso != NULL) {
        if (ret != RET_OK) {
            json_object_clear_string(request_jso, REQUEST_BODY);
        }
        json_object_put(request_jso);
    }

    return ret;
}


LOCAL gint32 sms_info_verify_basic_auth(SMS_INFO_S *info, const gchar *authorization, guint32 *user_privilege)
{
    gint32 ret;
    
    REQUEST_INFO_S *request_info = NULL;

    
    PROVIDER_PARAS_S provider_param;
    json_object *message_info_jso = NULL;

    gint32 valid_head_num = 0;

    if (NULL == info || NULL == info->jso_request || NULL == authorization || NULL == user_privilege) {
        debug_log(DLOG_ERROR, "param error ");
        return VOS_ERR;
    }

    
    request_info = (REQUEST_INFO_S *)g_malloc(sizeof(REQUEST_INFO_S));
    return_val_do_info_if_expr((NULL == request_info), VOS_ERR, debug_log(DLOG_ERROR, "g_malloc failed"));
    (void)memset_s(request_info, sizeof(REQUEST_INFO_S), 0, sizeof(REQUEST_INFO_S));
    (void)memset_s(&provider_param, sizeof(provider_param), 0, sizeof(provider_param));

    // 获取请求客户端IP
    ret = json_custom_get_elem_str(info->jso_request, REQUEST_IP, request_info->request_ip, MAX_IPADDR_LEN);
    return_val_do_info_if_expr((VOS_OK != ret), VOS_ERR, debug_log(DLOG_ERROR, "json_custom_get_elem_str error ");
        g_free(request_info));
    
    
    (void)strncpy_s(request_info->headers[valid_head_num].header_title, MAX_HEADER_TITLE_LEN, REQ_HEADER_BASIC_AUTH,
        sizeof(REQ_HEADER_BASIC_AUTH));
    (void)strncpy_s(request_info->headers[valid_head_num].header_content, MAX_HEADER_CONTENT_LEN, authorization,
        MAX_HEADER_CONTENT_LEN - 1);

    
    request_info->headers_index.basic_auth = &request_info->headers[valid_head_num];
    

    ret = verify_basic_auth_get_provider_param(request_info, &provider_param, &message_info_jso);
    json_object_put(message_info_jso);

    if (VOS_OK != ret) {
        switch (ret) {
            case USER_UNSUPPORT:
            case INVALID_PASSWORD:
            case USER_DONT_EXIST:
            case UUSER_USERNAME_TOO_LONG:
            case UUSER_USERPASS_TOO_LONG:
                sms_base_error(info, HTTP_UNAUTHORIZED, MSGID_LOGIN_AUTH_FAILED, NULL);
                break;

            case USER_NO_ACCESS:
                sms_base_error(info, HTTP_UNAUTHORIZED, MSGID_NO_ACCESS, NULL);
                break;

            case USER_PASSWORD_EXPIRED:
                sms_base_error(info, HTTP_UNAUTHORIZED, MSGID_USER_PSWD_EXPIRED, NULL);
                break;

            case USER_LOGIN_LIMITED:
                sms_base_error(info, HTTP_UNAUTHORIZED, MSGID_USER_LOGIN_RESTRICTED, NULL);
                break;

            default:
                sms_base_error(info, HTTP_UNAUTHORIZED, MSGID_LOGIN_ATTEMPT_FAILED, NULL);
        }

        debug_log(DLOG_ERROR, "%s %d:  fail, ret is 0x%x", __FUNCTION__, __LINE__, ret);
    }

    *user_privilege = provider_param.user_role_privilege;
    
    free_request_info(request_info);
    
    return ret;
}


LOCAL gint32 sms_info_verify(SMS_INFO_S *info)
{
    json_object *session_jso = NULL;
    json_object *headers_jso = NULL;
    json_object *jso_itm = NULL;
    json_object *jso_val = NULL;

    const gchar *str_tmp = NULL;
    gint32 ret = VOS_ERR;
    json_bool b_ret;

    guint32 user_privilege = 0;
    gint32 x_auth_token_nth;
    gint32 basic_nth;

    return_val_do_info_if_fail((NULL != info) && (NULL != info->jso_request), VOS_ERR,
        debug_log(DLOG_ERROR, "[%s]: Input params error.", __FUNCTION__));
    // 获取RequestHeaders
    b_ret = json_object_object_get_ex(info->jso_request, REQUEST_HEADERS, &headers_jso);

    goto_label_do_info_if_fail((b_ret && (json_type_array == json_object_get_type(headers_jso))), __exit,
        sms_base_error(info, HTTP_INTERNAL_SERVER_ERROR, MSGID_INTERNAL_ERR, NULL);
        debug_log(DLOG_ERROR, "[%s]: get %s error", __FUNCTION__, REQUEST_HEADERS));

    // 获取 RequestHeaders.X-Auth-Token, 异常时作401处理
    x_auth_token_nth = ex_json_object_array_get_object_item_nth(headers_jso, REQ_HEADER_X_AUTH_TOKEN);
    basic_nth = ex_json_object_array_get_object_item_nth(headers_jso, REQ_HEADER_BASIC_AUTH);
    if (x_auth_token_nth >= 0) {
        jso_itm = json_object_array_get_idx(headers_jso, x_auth_token_nth);
        goto_label_do_info_if_fail((NULL != jso_itm), __exit,
            sms_base_error(info, HTTP_UNAUTHORIZED, MSGID_NO_VALID_SESSION, NULL);
            debug_log(DLOG_ERROR, "[%s]: get %s jso error", __FUNCTION__, REQ_HEADER_X_AUTH_TOKEN));

        b_ret = ex_json_object_object_get_keyval(jso_itm, NULL, &jso_val);
        goto_label_do_info_if_fail((b_ret && (NULL != jso_val) && (json_type_string == json_object_get_type(jso_val))),
            __exit, sms_base_error(info, HTTP_UNAUTHORIZED, MSGID_NO_VALID_SESSION, NULL);
            debug_log(DLOG_ERROR, "[%s]: get %s error", __FUNCTION__, REQ_HEADER_X_AUTH_TOKEN));

        str_tmp = dal_json_object_get_str(jso_val);

        // X-Auth-Token->session_jso->user_privilege:
        goto_label_do_info_if_fail((NULL != str_tmp), __exit,
            sms_base_error(info, HTTP_UNAUTHORIZED, MSGID_NO_VALID_SESSION, NULL);
            debug_log(DLOG_DEBUG, "[%s]: get x-auth-token error", __FUNCTION__));

        goto_label_do_info_if_fail((VOS_OK == get_session_jso_by_xauthtoken(str_tmp, &session_jso)) &&
            (NULL != session_jso),
            __exit, sms_base_error(info, HTTP_UNAUTHORIZED, MSGID_NO_VALID_SESSION, NULL);
            debug_log(DLOG_DEBUG, "[%s]: get session jso by x-auth-token error", __FUNCTION__));
        // 获取session_id
        b_ret = json_object_object_get_ex(session_jso, RF_SESSION_ID, &jso_val);

        goto_label_do_info_if_fail((b_ret && (json_type_string == json_object_get_type(jso_val))), __exit,
            sms_base_error(info, HTTP_UNAUTHORIZED, MSGID_NO_VALID_SESSION, NULL);
            debug_log(DLOG_ERROR, "[%s]: get %s error", __FUNCTION__, RF_SESSION_ID));

        str_tmp = dal_json_object_get_str(jso_val);
        goto_label_do_info_if_fail((NULL != str_tmp), __exit,
            sms_base_error(info, HTTP_UNAUTHORIZED, MSGID_NO_VALID_SESSION, NULL);
            debug_log(DLOG_ERROR, "[%s]: jso get %s error", __FUNCTION__, RF_SESSION_ID));

        strncpy_s(info->session_id, sizeof(info->session_id), str_tmp, strlen(str_tmp));

        // 获取user_privilege, 异常时作403处理
        b_ret = json_object_object_get_ex(session_jso, RF_SESSION_USER_PRI, &jso_val);
        goto_label_do_info_if_fail((b_ret && (json_type_int == json_object_get_type(jso_val))), __exit,
            sms_base_error(info, HTTP_FORBIDDEN, MSGID_INSUFFICIENT_PRIVILEGE, NULL);
            debug_log(DLOG_ERROR, "[%s]: get %s error", __FUNCTION__, RF_SESSION_USER_PRI));

        user_privilege = (guint32)json_object_get_int(jso_val);
    } else if (basic_nth >= 0) {
        jso_itm = json_object_array_get_idx(headers_jso, basic_nth);
        goto_label_do_info_if_fail((NULL != jso_itm), __exit,
            sms_base_error(info, HTTP_UNAUTHORIZED, MSGID_LOGIN_AUTH_FAILED, NULL);
            debug_log(DLOG_ERROR, "[%s]: get %s jso error", __FUNCTION__, REQ_HEADER_X_AUTH_TOKEN));

        b_ret = ex_json_object_object_get_keyval(jso_itm, NULL, &jso_val);
        goto_label_do_info_if_fail((b_ret && (NULL != jso_val) && (json_type_string == json_object_get_type(jso_val))),
            __exit, sms_base_error(info, HTTP_UNAUTHORIZED, MSGID_LOGIN_AUTH_FAILED, NULL);
            debug_log(DLOG_ERROR, "[%s]: get %s error", __FUNCTION__, REQ_HEADER_BASIC_AUTH));

        str_tmp = dal_json_object_get_str(jso_val);

        b_ret = sms_info_verify_basic_auth(info, str_tmp, &user_privilege);
        goto_label_do_info_if_fail((VOS_OK == b_ret), __exit,
            debug_log(DLOG_ERROR, "[%s]: jso get %s error", __FUNCTION__, REQ_HEADER_BASIC_AUTH));
    } else {
        sms_base_error(info, HTTP_UNAUTHORIZED, MSGID_NO_VALID_SESSION, NULL);
        debug_log(DLOG_ERROR, "[%s]: get %s error", __FUNCTION__, REQ_HEADER_X_AUTH_TOKEN);
        goto __exit;
    }

    if (0 == g_ascii_strcasecmp(info->method, HTTP_GET)) {
        goto_label_do_info_if_fail((user_privilege & USERROLE_READONLY), __exit,
            sms_base_error(info, HTTP_FORBIDDEN, MSGID_INSUFFICIENT_PRIVILEGE, NULL);
            debug_log(DLOG_ERROR, "[%s]: %s NOT match to USERROLE_BASICSETTING", __FUNCTION__, RF_SESSION_USER_PRI));
    } else if (0 == g_ascii_strcasecmp(info->method, HTTP_PATCH)) {
        goto_label_do_info_if_fail((user_privilege & USERROLE_BASICSETTING), __exit,
            sms_base_error(info, HTTP_FORBIDDEN, MSGID_INSUFFICIENT_PRIVILEGE, NULL);
            debug_log(DLOG_ERROR, "[%s]: %s NOT match to USERROLE_BASICSETTING", __FUNCTION__, RF_SESSION_USER_PRI));
    } else {
        goto_label_do_info_if_fail((user_privilege & USERROLE_BASICSETTING), __exit,
            sms_base_error(info, HTTP_FORBIDDEN, MSGID_INSUFFICIENT_PRIVILEGE, NULL);
            debug_log(DLOG_ERROR, "[%s]: %s NOT match to USERROLE_BASICSETTING", __FUNCTION__, RF_SESSION_USER_PRI));
    }

    // 白名单过滤, 不在允许列表中的URI访问返回404资源不存在
    ret = sms_info_match_white_list(info, info->jso_white_list);

    goto_label_do_info_if_fail((VOS_OK == ret), __exit,
        sms_base_error(info, HTTP_NOT_FOUND, MSGID_RSC_MISSING_URI, NULL, info->relative_uri);
        debug_log(DLOG_ERROR, "[%s]: URI %s NOT match to white list", __FUNCTION__, info->relative_uri));

    ret = VOS_OK; //  返回VOS_OK做进一步的透传响应处理: CALL CLASS METHOD.
__exit:

    if (NULL != session_jso) {
        json_object_put(session_jso);
    }

    return ret;
}


LOCAL gint32 sms_info_match_white_list(SMS_INFO_S *info, json_object *jso_array_white)
{
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    const gchar *str_regex = NULL;
    json_object *jso_iter = NULL;
    gint32 ret = VOS_ERR;

    return_val_do_info_if_fail((NULL != info) && (strlen(info->redirect_uri) > 0) && (NULL != jso_array_white), VOS_ERR,
        debug_log(DLOG_ERROR, "[%s]: Input params error.", __FUNCTION__));

    if (strlen(info->session_id) > 0) {
        ret = dal_get_object_handle_nth(CLASS_SESSION, 0, &obj_handle);

        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            debug_log(DLOG_ERROR, "[%s]: Get %s object handle error.", __FUNCTION__, CLASS_SESSION));
        // 调用方法判断session_id是不是框内redfish会话
        input_list = g_slist_append(input_list, g_variant_new_string(info->session_id));
        ret = dfl_call_class_method(obj_handle, METHOD_SESSION_ISREDFISHINNERSESSION, NULL, input_list, NULL);
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        // 若session_id为框内会话, 无需白名单验证, 直接返回OK.
        return_val_do_info_if_expr((VOS_OK == ret), VOS_OK,
            debug_log(DLOG_DEBUG, "[%s]: Call class method %s OK", __FUNCTION__, METHOD_SESSION_ISREDFISHINNERSESSION));
    }

    // 遍历白名单
    json_object_array_foreach(jso_array_white, jso_iter)
    {
        str_regex = dal_json_object_get_str(jso_iter);
        // 满足白名单正则匹配, 返回OK.
        return_val_do_info_if_expr((NULL != str_regex) &&
            (TRUE == g_regex_match_simple(str_regex, info->redirect_uri, (GRegexCompileFlags)0, (GRegexMatchFlags)0)),
            VOS_OK, debug_log(DLOG_DEBUG, "[%s]: %s regex match to %s", __FUNCTION__, info->redirect_uri, str_regex));
    }
    // 匹配失败
    debug_log(DLOG_ERROR, "[%s]: %s regex match error", __FUNCTION__, info->redirect_uri);

    return VOS_ERR;
}

LOCAL gboolean sms_check_not_in_white_list(const gchar *str_regex, json_object *jso_array_white)
{
    const gchar *str_item = NULL;
    json_object *jso_iter = NULL;

    return_val_do_info_if_fail((NULL != str_regex) && (NULL != jso_array_white), VOS_ERR,
        debug_log(DLOG_ERROR, "[%s]: Input params error.", __FUNCTION__));

    json_object_array_foreach(jso_array_white, jso_iter)
    {
        str_item = dal_json_object_get_str(jso_iter);
        if (0 == g_ascii_strcasecmp(str_item, str_regex)) {
            debug_log(DLOG_DEBUG, "[%s]: %s already exist in white list", __FUNCTION__, str_regex);
            return VOS_ERR;
        }
    }

    return VOS_OK;
}

LOCAL gint32 sms_create_white_list(SMS_INFO_S *info)
{
    json_object *jso_array = NULL;
    json_object *jso_val = NULL;
    GSList *input_list = NULL;

    return_val_do_info_if_fail((NULL != info) && (NULL != info->jso_white_list) && (NULL != info->jso_request), VOS_ERR,
        debug_log(DLOG_ERROR, "[%s]: input param error", __FUNCTION__));

    json_bool b_ret = json_object_object_get_ex(info->jso_request, REQUEST_BODY, &jso_val);

    return_val_do_info_if_fail((b_ret && (json_type_string == json_object_get_type(jso_val))), VOS_ERR,
        debug_log(DLOG_ERROR, "[%s]: get %s error %d", __FUNCTION__, REQUEST_BODY, b_ret));

    jso_array = json_tokener_parse(dal_json_object_get_str(jso_val));
    if (jso_array == NULL) {
        debug_log(DLOG_ERROR, "%s: json_tokener_parse fail", __FUNCTION__);
        return RET_ERR;
    }

    if (json_object_get_type(jso_array) != json_type_array) {
        json_object_put(jso_array);
        debug_log(DLOG_ERROR, "%s: jso_array not array", __FUNCTION__);
        return RET_ERR;
    }

    json_object_put(info->jso_white_list);
    info->jso_white_list = json_object_new_array();

    guint32 n = json_object_array_length(jso_array);

    
    const gchar **white_list = (const gchar **)g_malloc0(sizeof(gchar *) * (n + 1));
    return_val_do_info_if_expr(NULL == white_list, VOS_ERR, json_object_put(jso_array);
        debug_log(DLOG_ERROR, "[%s]: g_malloc0 white_list failed", __FUNCTION__));

    guint32 j = 0;
    for (guint32 i = 0; i < n; i++) {
        jso_val = json_object_array_get_idx(jso_array, i);
        if (NULL == jso_val) {
            debug_log(DLOG_ERROR, "json_object_array_get_idx %d fail", n);
            continue;
        }

        const gchar *str_regex = dal_json_object_get_str(jso_val);
        white_list[j++] = str_regex;

        if (VOS_OK == sms_check_not_in_white_list(str_regex, info->jso_white_list)) {
            debug_log(DLOG_DEBUG, "[%s]: White list add %s", __FUNCTION__, dal_json_object_get_str(jso_val));
            json_object_array_add(info->jso_white_list, json_object_get(jso_val));
        }
    }

    input_list = g_slist_append(input_list, g_variant_new_strv((const gchar * const *)white_list, (gssize)j));
    gint32 ret = dfl_call_class_method(info->sms_obj_handle, METHOD_SMS_SET_REDIRECT_WHITELIST, NULL, input_list, NULL);
    if (ret == DFL_OK) {
        debug_log(DLOG_INFO, "%s: call %s successfully", __FUNCTION__, METHOD_SMS_SET_REDIRECT_WHITELIST);
    } else {
        debug_log(DLOG_ERROR, "%s: call %s fail, ret = %d", __FUNCTION__, METHOD_SMS_SET_REDIRECT_WHITELIST, ret);
    }

    g_free(white_list);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    
    json_object_put(jso_array);
    return VOS_OK;
}

LOCAL gint32 sms_delete_white_list(SMS_INFO_S *info)
{
    GSList *input_list = NULL;
    gint32 ret;
    gchar **white_list = NULL;

    return_val_do_info_if_fail((NULL != info) && (NULL != info->jso_white_list), VOS_ERR,
        debug_log(DLOG_ERROR, "[%s]: input param error", __FUNCTION__));

    json_object_put(info->jso_white_list);

    info->jso_white_list = json_object_new_array();

    debug_log(DLOG_DEBUG, "[%s]: sms/%d White list delete OK", __FUNCTION__, info->id);

    
    white_list = (gchar **)g_malloc0(sizeof(gchar *) * 2);
    return_val_do_info_if_expr(NULL == white_list, VOS_OK,
        debug_log(DLOG_ERROR, "[%s]: g_malloc0 white_list failed", __FUNCTION__));

    white_list[0] = (gchar *)g_malloc0(1);
    return_val_do_info_if_expr(NULL == white_list[0], VOS_OK, g_free(white_list); white_list = NULL;
        debug_log(DLOG_ERROR, "[%s]: g_malloc0 white_list failed", __FUNCTION__));

    input_list = g_slist_append(input_list, g_variant_new_strv((const gchar * const *)white_list, 1));

    ret = dfl_call_class_method(info->sms_obj_handle, METHOD_SMS_SET_REDIRECT_WHITELIST, NULL, input_list, NULL);
    if (ret == DFL_OK) {
        debug_log(DLOG_INFO, "%s: call %s successfully", __FUNCTION__, METHOD_SMS_SET_REDIRECT_WHITELIST);
    } else {
        debug_log(DLOG_ERROR, "%s: call %s fail, ret = %d", __FUNCTION__, METHOD_SMS_SET_REDIRECT_WHITELIST, ret);
    }

    g_free(white_list[0]);
    white_list[0] = NULL;
    g_free(white_list);
    white_list = NULL;
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    input_list = NULL;
    

    return VOS_OK;
}

LOCAL gint32 sms_call_forward(SMS_INFO_S *info, HTTP_RESPONSE_DATA_S *resp_data)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    GVariant *gv_tmp = NULL;
    const gchar *request_str = NULL;
    gint32 request_len;
    json_object *jso_resp = NULL;
    const guint8 retry_times = HTTPS_REQUEST_FORWARD_BMA_MIN_RETRY;

    if (info == NULL || info->jso_request == NULL || resp_data == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__);
        return VOS_ERR;
    }

    request_str = dal_json_object_to_json_string(info->jso_request);
    return_val_do_info_if_fail((NULL != request_str), VOS_ERR,
        sms_base_error(info, HTTP_INTERNAL_SERVER_ERROR, MSGID_INTERNAL_ERR, NULL);
        debug_log(DLOG_ERROR, "[%s]: failed to parse request jso to string.", __FUNCTION__));

    request_len = strlen(request_str);

    // CLASS_SMS, METHOD_SMS_FORWARD_REDFISH_REQUEST
    ret = dal_get_object_handle_nth(CLASS_SMS, 0, &obj_handle);
    return_val_if_expr(ret != VOS_OK, VOS_ERR);
    //
    input_list = g_slist_append(input_list, g_variant_new_string(request_str));
    input_list = g_slist_append(input_list, g_variant_new_int32(request_len));
    input_list = g_slist_append(input_list, g_variant_new_byte(retry_times));
    //
    ret = dfl_call_class_method(obj_handle, METHOD_SMS_FORWARD_REDFISH_REQUEST, NULL, input_list, &output_list);

    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);

    return_val_do_info_if_fail((VOS_OK == ret) && (NULL != output_list), VOS_ERR,
        debug_log(DLOG_ERROR, "[%s]: failed to call class method %s.", __FUNCTION__,
        METHOD_SMS_FORWARD_REDFISH_REQUEST));
    gv_tmp = (GVariant *)g_slist_nth_data(output_list, 0);
    return_val_do_info_if_fail((NULL != gv_tmp), VOS_ERR,
        debug_log(DLOG_ERROR, "[%s]: class method %s output NULL", __FUNCTION__, METHOD_SMS_FORWARD_REDFISH_REQUEST);
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref));

    
    jso_resp = json_tokener_parse(g_variant_get_string(gv_tmp, NULL));
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

    if (jso_resp == NULL) {
        debug_log(DLOG_ERROR, "%s: parse response str to json object failed", __FUNCTION__);
        return VOS_ERR;
    }

    generate_redfish_resp_data_from_jso(jso_resp, resp_data);
    json_object_put(jso_resp);
    return VOS_OK;
}

LOCAL gint32 sms_info_body_json_verify(SMS_INFO_S *info)
{
    json_object *jso_val = NULL;
    const gchar *str_tmp = NULL;
    json_object *body_jso = NULL;
    json_bool b_ret;

    if (info == NULL || info->jso_request == NULL) {
        debug_log(DLOG_INFO, "%s: request_str is NULL.", __FUNCTION__);
        return RET_OK;
    }
    if (g_ascii_strcasecmp(info->method, HTTP_GET) == 0) {
        debug_log(DLOG_INFO, "%s: method is Get.", __FUNCTION__);
        return RET_OK;
    }
    
    b_ret = json_object_object_get_ex(info->jso_request, REQUEST_BODY, &jso_val);
    if (b_ret && jso_val != NULL) {
        str_tmp = dal_json_object_get_str(jso_val);
        if (str_tmp == NULL) {
            debug_log(DLOG_INFO, "%s:dal_json_object_get_str request_body error.", __FUNCTION__);
            return RET_OK;
        }
        // 转换字符串为JSON对象
        body_jso = json_tokener_parse(str_tmp);
        if (body_jso == NULL) {
            debug_log(DLOG_ERROR, "%s:json_tokener_parse body_jso error.", __FUNCTION__);
            return RET_ERR;
        }
        json_object_put(body_jso);
    }
    
    return RET_OK;
}


LOCAL gint32 sms_process_response(SMS_INFO_S *info, HTTP_RESPONSE_DATA_S *resp_data)
{
    if (info == NULL || resp_data == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__);
        return VOS_ERR;
    }

    (void)sms_info_verify(info);
    if (info->resp_code != VOS_OK || info->jso_response != NULL) {
        debug_log(DLOG_DEBUG, "[%s]: handle resp_code: %d", __FUNCTION__, info->resp_code);
        return sms_generate_response(info, resp_data);
    }

    if (sms_info_body_json_verify(info) != RET_OK) {
        sms_base_error(info, HTTP_BAD_REQUEST, MSGID_MALFORMED_JSON, NULL);
        sms_generate_response(info, resp_data);
        debug_log(DLOG_ERROR, "%s:sms_info_body_verify error.", __FUNCTION__);
        return RET_OK;
    }

    if (TRUE ==
        g_regex_match_simple(REGEX_WHITE_LIST_STRING, info->redirect_uri, (GRegexCompileFlags)0, (GRegexMatchFlags)0)) {
        if (0 == g_ascii_strcasecmp(info->method, HTTP_POST)) {
            if (VOS_OK == sms_create_white_list(info)) {
                sms_base_error(info, HTTP_CREATED, MSGID_CREATED, NULL);
            } else {
                sms_ibmc_error(info, HTTP_BAD_REQUEST, MSGID_RSC_CANNOT_CREATED, NULL);
            }
        } else if (0 == g_ascii_strcasecmp(info->method, HTTP_DELETE)) {
            if (VOS_OK == sms_delete_white_list(info)) {
                sms_base_error(info, HTTP_OK, MSGID_SUCCESS, NULL);
            } else {
                sms_base_error(info, HTTP_INTERNAL_SERVER_ERROR, MSGID_INTERNAL_ERR, NULL);
            }
        } else {
            sms_base_error(info, HTTP_METHOD_NOT_ALLOWED, "", NULL);
        }

        sms_generate_response(info, resp_data);
        return VOS_OK;
    }

    return sms_call_forward(info, resp_data);
}

LOCAL gint32 sms_redirect(const gchar *request_str, gint32 request_len, HTTP_RESPONSE_DATA_S *resp_data)
{
    SMS_INFO_S *info = NULL;

    if ((VOS_OK == sms_parse_request(request_str, request_len, &info)) && (NULL != info)) {
        if (VOS_OK == sms_process_response(info, resp_data)) {
            debug_log(DLOG_DEBUG, "[%s]: sms/%d %s redirect [OK]", __FUNCTION__, info->id, info->redirect_uri);
        } else {
            sms_base_error(info, HTTP_NOT_FOUND, MSGID_RSC_MISSING_URI, NULL, info->relative_uri);
            sms_generate_response(info, resp_data);
            debug_log(DLOG_DEBUG, "[%s]: sms/%d %s redirect [ERR]", __FUNCTION__, info->id, info->relative_uri);
        }

        (void)sms_info_flush(info);
        return VOS_OK;
    }

    debug_log(DLOG_DEBUG, "[%s]: NOT sms redirect", __FUNCTION__);
    return VOS_ERR;
}

gint32 method_redirect_http_request(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    GVariant *request = NULL;
    gsize request_len;
    const gchar *request_str = NULL;
    gint32 ret;
    HTTP_RESPONSE_DATA_S resp_data;

    return_val_do_info_if_fail(NULL != input_list, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: input_list=NULL.", __FUNCTION__));
    
    return_val_do_info_if_fail(NULL != output_list, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: output_list = NULL.", __FUNCTION__));
    

    request = (GVariant *)g_slist_nth_data(input_list, 0);
    return_val_do_info_if_fail(NULL != request, VOS_ERR, debug_log(DLOG_ERROR, "%s: request=NULL.", __FUNCTION__));

    request_str = g_variant_get_string(request, &request_len);

    return_val_do_info_if_fail(NULL != request_str, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: request_str=NULL.", __FUNCTION__));

    init_http_response_data(&resp_data);
    
    (void)pthread_mutex_lock(&g_sms_redirect_req_lock);
    ret = sms_redirect(request_str, request_len, &resp_data);
    (void)pthread_mutex_unlock(&g_sms_redirect_req_lock);
    

    
    if (ret != VOS_OK || resp_data.resp_status == NULL || strlen(resp_data.resp_status) == 0) {
        destory_http_response_data(&resp_data);
        debug_log(DLOG_DEBUG, "%s: sms redirect failed(ret:%d) or response is NULL", __FUNCTION__, ret);
        return VOS_ERR;
    }

    
    *output_list =
        g_slist_append(*output_list, g_variant_new_string((resp_data.resp_body == NULL) ? "" : resp_data.resp_body));
    *output_list =
        g_slist_append(*output_list, g_variant_new_string((resp_data.resp_head == NULL) ? "" : resp_data.resp_head));
    *output_list = g_slist_append(*output_list,
        g_variant_new_string((resp_data.resp_status == NULL) ? "" : resp_data.resp_status));

    destory_http_response_data(&resp_data);
    return VOS_OK;
}
