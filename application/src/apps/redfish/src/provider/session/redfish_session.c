
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "pme_app/uip/uip.h"

#include "redfish_http.h"
#include "redfish_message.h"
#include "redfish_class.h"
#include "redfish_provider.h"


LOCAL gint32 g_verify_account_err_code[] = {
    INVALID_PASSWORD,
    USER_DONT_EXIST,
    UUSER_USERNAME_TOO_LONG,
    UUSER_USERPASS_TOO_LONG,
    USER_NO_ACCESS,
    USER_PASSWORD_EXPIRED,
    USER_LOGIN_LIMITED,
    USER_IS_LOCKED
};


LOCAL gint32 is_web_request_at_mutual_auth(PROVIDER_PARAS_S *i_param);
LOCAL void clear_session_passwd(json_object *body_jso_checked);



LOCAL gint32 generate_post_session_etag(const gchar *uri, const gchar *member_id, json_object **o_header_array_jso,
    json_object *message_array_jso)
{
    int iRet;
    errno_t safe_fun_ret;
    REQUEST_INFO_S *new_resource_info = NULL;
    PROVIDER_PARAS_S provider_param;
    gchar *uri_lower = NULL;
    gchar *id_hash = NULL;
    json_object *resource_jso = NULL;
    json_object *redfish_jso = NULL;
    gint32 ret;
    json_bool ret_json;
    json_object *header_jso = NULL;
    gchar session_id[SESSION_ID_LEN + 1] = {0};

    new_resource_info = (REQUEST_INFO_S *)g_malloc(sizeof(REQUEST_INFO_S));
    return_val_do_info_if_fail(NULL != new_resource_info, DLOG_ERROR, debug_log(DLOG_ERROR, "g_malloc failed"));

    (void)memset_s(new_resource_info, sizeof(REQUEST_INFO_S), 0, sizeof(REQUEST_INFO_S));
    
    (void)memset_s(&provider_param, sizeof(provider_param), 0, sizeof(provider_param));

    id_hash = g_compute_checksum_for_string(G_CHECKSUM_SHA256, member_id, -1);
    return_val_do_info_if_expr(id_hash == NULL, HTTP_INTERNAL_SERVER_ERROR, g_free(new_resource_info);
        new_resource_info = NULL;
        debug_log(DLOG_ERROR, "%s:id_hash g_compute_checksum_for_string failed.", __FUNCTION__));

    safe_fun_ret = strncpy_s(session_id, SESSION_ID_LEN + 1, id_hash, SESSION_ID_LEN);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    g_free(id_hash);
    id_hash = NULL;
    iRet = snprintf_s(new_resource_info->relative_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, "%s/%s", uri, session_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    // 填充新生成资源request_info部分必需值
    // 转换URI信息为小写保存
    uri_lower = g_ascii_strdown(new_resource_info->relative_uri, strlen(new_resource_info->relative_uri));
    goto_label_do_info_if_fail(NULL != uri_lower, EXIT, ret = HTTP_INTERNAL_SERVER_ERROR;
        debug_log(DLOG_ERROR, "%s %d: g_ascii_strdown fail", __FUNCTION__, __LINE__));

    (void)strncpy_s(new_resource_info->uri_lower, MAX_URI_LENGTH, uri_lower, MAX_URI_LENGTH - 1);
    g_free(uri_lower);
    uri_lower = NULL;

    (void)strncpy_s(new_resource_info->headers[0].header_title, MAX_HEADER_TITLE_LEN, RESP_HEADER_X_AUTH_TOKEN,
        strlen(RESP_HEADER_X_AUTH_TOKEN));
    (void)strncpy_s(new_resource_info->headers[0].header_content, MAX_HEADER_CONTENT_LEN, member_id,
        MAX_HEADER_CONTENT_LEN - 1);
    new_resource_info->headers_index.x_auth_token = &new_resource_info->headers[0];

    // 根据URI获取资源配置文件路径
    ret = get_res_info_from_uri(new_resource_info, &provider_param, message_array_jso);
    goto_label_do_info_if_fail(VOS_OK == ret, EXIT,
        debug_log(DLOG_ERROR, "%s %d: get res path fail", __FUNCTION__, __LINE__));

    // 获取新生成会话信息
    ret = verify_xauthtoken_get_provider_param(new_resource_info, &provider_param);
    goto_label_do_info_if_fail(VOS_OK == ret, EXIT,
        debug_log(DLOG_ERROR, "%s %d: verify_xauthtoken_get_session fail", __FUNCTION__, __LINE__);
        g_free(new_resource_info); new_resource_info = NULL;);
    // 载入资源配置文件
    resource_jso = json_object_from_file(new_resource_info->resource_path);
    goto_label_do_info_if_fail((NULL != resource_jso), EXIT, ret = HTTP_NOT_FOUND;
        debug_log(DLOG_ERROR, "%s get resource from file fail", __FUNCTION__));

    // 获取配置文件中资源相关属性
    ret_json = json_object_object_get_ex(resource_jso, REDFISH_OBJECT, &redfish_jso);
    goto_label_do_info_if_fail(ret_json, EXIT, ret = HTTP_NOT_FOUND;
        debug_log(DLOG_ERROR, "%s get REDFISH_OBJECT from file fail", __FUNCTION__); json_object_put(resource_jso));

    // 更新资源对象属性值
    ret = generate_get_response_body(new_resource_info, &provider_param, &redfish_jso, message_array_jso);
    goto_label_do_info_if_fail(ret == VOS_OK, EXIT, json_object_put(resource_jso);
        debug_log(DLOG_ERROR, "%s generate_get_response_body fail", __FUNCTION__));

    // 生成ETag响应头
    ret = generate_etag_response_header(new_resource_info->uri_lower, redfish_jso, NULL, &header_jso);
    if (VOS_OK == ret) {
        (void)json_object_array_add(*o_header_array_jso, header_jso);
    } else {
        debug_log(DLOG_ERROR, "%s %d :generate etag error, ret is %d", __FUNCTION__, __LINE__, ret);
    }

    json_object_put(resource_jso);

EXIT:
    clear_sensitive_info(new_resource_info, sizeof(REQUEST_INFO_S));
    g_free(new_resource_info);
    new_resource_info = NULL;
    return ret;
}



LOCAL void generate_web_session_response_headers(PROVIDER_PARAS_S *i_param, json_object *header_array_jso,
    const gchar* session_id, const gchar* id)
{
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    GSList *caller_info = NULL;
    OBJ_HANDLE obj_handle = 0;
    const gchar *token_str = NULL;
    json_object *header_jso = NULL;
    gint32 ret;
    gchar cookie[MAX_STRBUF_LEN] = {0};

    if (i_param->is_from_webui) {
        // 生成webui头
        header_jso = json_object_new_object();
        json_object_object_add(header_jso, RESP_HEADER_FROM, json_object_new_string(REQ_FROM_WEBUI));
        json_object_array_add(header_array_jso, header_jso);

        // 生成csrf token
        (void)dal_get_object_handle_nth(CLASS_SESSION, 0, &obj_handle);
        input_list = g_slist_append(input_list, g_variant_new_string(session_id));
        caller_info = g_slist_append(caller_info, g_variant_new_string(USER_LOGIN_INTERFACE_REDFISH_STRING));
        caller_info = g_slist_append(caller_info, g_variant_new_string(i_param->user_name));
        caller_info = g_slist_append(caller_info, g_variant_new_string(i_param->client));

        ret = dfl_call_class_method(obj_handle, METHOD_SESSION_GETTOKEN, caller_info, input_list, &output_list);
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        g_slist_free_full(caller_info, (GDestroyNotify)g_variant_unref);
        if (ret == VOS_OK) {
            token_str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
            header_jso = json_object_new_object();
            json_object_object_add(header_jso, RESP_HEADER_TOKEN, json_object_new_string(token_str));
            json_object_array_add(header_array_jso, header_jso);
            uip_free_gvariant_list(output_list);
        }

        ret = snprintf_s(cookie, sizeof(cookie), sizeof(cookie) - 1, "SessionId=%s; Path=/; Secure; Httponly", id);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s failed, ret:%d", __FUNCTION__, ret);
        }
        header_jso = json_object_new_object();
        json_object_object_add(header_jso, RESP_HEADER_SETCOOKIE, json_object_new_string((const gchar*)cookie));
        json_object_array_add(header_array_jso, header_jso);
        clear_sensitive_info(cookie, sizeof(cookie));
    }
}


gint32 generate_session_response_headers(PROVIDER_PARAS_S *i_param, gchar *id, json_object **header_array_jso,
    json_object *message_array_jso)
{
    int iRet;
    errno_t safe_fun_ret;
    gint32 ret;
    json_object *header_jso = NULL;
    gchar location[MAX_URI_LENGTH] = {0};
    gchar *id_hash = NULL;
    gchar session_id[SESSION_ID_LEN + 1] = {0};
    json_object *message_info_jso = NULL;
    json_object *message_array_tmp_jso = NULL;

    // 生成公共响应头
    ret = generate_common_response_headers(header_array_jso, i_param->uri, message_array_jso);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: generate_common_response_headers error", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(message_array_jso, message_info_jso));

    // 生成Location头
    id_hash = g_compute_checksum_for_string(G_CHECKSUM_SHA256, id, -1);
    return_val_do_info_if_expr(NULL == id_hash, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: generate_location_headers error", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(message_array_jso, message_info_jso));

    safe_fun_ret = strncpy_s(session_id, SESSION_ID_LEN + 1, id_hash, SESSION_ID_LEN);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    g_free(id_hash);
    iRet = snprintf_s(location, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, "%s/%s", i_param->uri, session_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    header_jso = json_object_new_object();
    json_object_object_add(header_jso, RESP_HEADER_LOCATION, json_object_new_string((const gchar *)location));
    json_object_array_add(*header_array_jso, header_jso);

    generate_web_session_response_headers(i_param, *header_array_jso, (const gchar*)session_id, (const gchar*)id);

    // 非WEB请求生成ETag头，生成失败不阻塞正常返回
    if (!i_param->is_from_webui) {
        // 仅非WEB请求才生成x-auth-token头
        header_jso = json_object_new_object();
        json_object_object_add(header_jso, RESP_HEADER_X_AUTH_TOKEN, json_object_new_string(id));
        json_object_array_add(*header_array_jso, header_jso);

        message_array_tmp_jso = json_object_new_array();
        (void)generate_post_session_etag(i_param->uri, id, header_array_jso, message_array_tmp_jso);
        json_object_put(message_array_tmp_jso);
    }

    clear_sensitive_info(session_id, sizeof(session_id));
    clear_sensitive_info(location, sizeof(location));
    return ret;
}


LOCAL gint32 generate_account_verify_input(guchar from_webui_flag, gint32 user_type, GSList **input_list,
    GSList **caller_info, const gchar *user_name, const gchar *password, const gchar *client)
{
    if (NULL == input_list || NULL == caller_info || NULL == user_name || NULL == password || client == NULL) {
        debug_log(DLOG_ERROR, "%s input param error", __FUNCTION__);
        return VOS_ERR;
    }

    // 填充入参信息,先按照本地用户进行校验
    *input_list = g_slist_append(*input_list, g_variant_new_string(user_name));
    *input_list = g_slist_append(*input_list, g_variant_new_string(password));
    *input_list = g_slist_append(*input_list, g_variant_new_byte(user_type));
    *input_list = g_slist_append(*input_list, g_variant_new_string(client));

    // 填充调用接口信息,用于方法端区分操作
    
    *caller_info = g_slist_append(*caller_info, g_variant_new_string(dal_rf_get_op_log_module_name(from_webui_flag)));
    
    *caller_info = g_slist_append(*caller_info, g_variant_new_string(user_name));
    *caller_info = g_slist_append(*caller_info, g_variant_new_string(client));

    return VOS_OK;
}


gint32 verify_local_account_and_get_info(guchar from_webui_flag, const gchar *user_name, const gchar *password,
    const gchar *client, guchar *o_auth_type, guchar *o_user_id, guchar *o_pri, gchar *o_role_id, guint32 role_id_len,
    gchar *o_real_user, guint32 real_user_len)
{
    errno_t safe_fun_ret = EOK;

    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    GSList *caller_info = NULL;
    OBJ_HANDLE obj_handle;
    const gchar *role_id = NULL;

    return_val_do_info_if_fail(((NULL != o_user_id) && (NULL != o_auth_type) && (NULL != o_pri) && (NULL != o_role_id)),
        VOS_ERR, debug_log(DLOG_ERROR, "%s %d: input param error", __FUNCTION__, __LINE__));

    
    ret = generate_account_verify_input(from_webui_flag, LOG_TYPE_LOCAL, &input_list, &caller_info, user_name, password,
        client);
    return_val_if_expr(ret != VOS_OK, ret);

    // 获取相应用户对象句柄
    ret = dal_get_object_handle_nth(CLASS_USER, 0, &obj_handle);
    
    goto_label_do_info_if_fail((VOS_OK == ret), _exit,
        debug_log(DLOG_ERROR, "%s dal_get_object_handle_nth %s fail, user is %s", __FUNCTION__, METHOD_USER_AUTHUSER,
        user_name));
    
    // 调用AuthUser方法校验账户密码有效性
    ret = uip_call_class_method_redfish(from_webui_flag, user_name, client, obj_handle, CLASS_USER,
        METHOD_USER_AUTHUSER, AUTH_DISABLE, AUTH_PRIV_NONE, input_list, &output_list);
    goto_label_do_info_if_fail((VOS_OK == ret), _exit,
        debug_log(DLOG_ERROR, "%s dfl_call_class_method %s error", __FUNCTION__, METHOD_USER_AUTHUSER));
    

    ret = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
    // 校验成功直接返回成功
    if (VOS_OK == ret) {
        // 本地用户仅能获取到用户ID，认证类型
        *o_user_id = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 2));
        *o_auth_type = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 3));
        *o_pri = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 4));
        role_id = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 5), NULL);
        safe_fun_ret = strncpy_s(o_role_id, role_id_len, role_id, strlen(role_id));
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
        safe_fun_ret = strncpy_s(o_real_user, real_user_len, user_name, strlen(user_name));
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

        debug_log(DLOG_DEBUG, "verify_account_get_info success %d, %d\n", *o_auth_type, *o_user_id);
    }

_exit:

    if (NULL != input_list) {
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    }

    if (NULL != output_list) {
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    }

    if (NULL != caller_info) {
        g_slist_free_full(caller_info, (GDestroyNotify)g_variant_unref);
    }

    return ret;
}


LOCAL gint32 verify_ldap_account_and_get_info(guchar from_webui_flag, const gchar *user_name, const gchar *password,
    const gchar *client, guchar *o_auth_type, guchar *o_user_id, guchar *o_pri, gchar *o_role_id, guint32 role_id_len,
    gchar *o_real_user, guint32 real_user_len)
{
    int iRet = -1;
    errno_t safe_fun_ret;

    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    GSList *caller_info = NULL;
    OBJ_HANDLE obj_handle;
    const gchar *role_id = NULL;
    guint8 domain_flag;
    const gchar *ldap_domain = NULL;

    return_val_do_info_if_fail(((NULL != o_user_id) && (NULL != o_auth_type) && (NULL != o_pri) &&
        (NULL != o_role_id) && (NULL != o_auth_type)),
        VOS_ERR, debug_log(DLOG_ERROR, "%s %d: input param error", __FUNCTION__, __LINE__));
    ret = generate_account_verify_input(from_webui_flag, LOG_TYPE_LDAP_AUTO_MATCH, &input_list, &caller_info, user_name,
        password, client);
    return_val_if_expr(ret != VOS_OK, ret);

    // 获取相应用户对象句柄
    ret = dal_get_object_handle_nth(CLASS_USER, 0, &obj_handle);
    goto_label_do_info_if_fail((VOS_OK == ret), _exit,
        debug_log(DLOG_ERROR, "%s dal_get_object_handle_nth fail, user is %s", __FUNCTION__, user_name));

    ret = uip_call_class_method_redfish(from_webui_flag, user_name, client, obj_handle, CLASS_USER,
        METHOD_USER_AUTHUSER, AUTH_DISABLE, AUTH_PRIV_NONE, input_list, &output_list);
    

    
    goto_label_do_info_if_fail((VOS_OK == ret), _exit, ret = VOS_ERR;
        debug_log(DLOG_ERROR, "%s dfl_call_class_method %s error, value %d", __FUNCTION__, METHOD_USER_AUTHUSER, ret));

    ret = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
    goto_label_do_info_if_fail((VOS_OK == ret), _exit,
        debug_log(DLOG_ERROR, "%s dfl_call_class_method %s ret value error, value %d", __FUNCTION__,
        METHOD_USER_AUTHUSER, ret));
    

    // LDAP的有效组ID集
    *o_user_id = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 2));
    role_id = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 3), NULL);
    safe_fun_ret = strncpy_s(o_role_id, role_id_len, role_id, strlen(role_id));
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    *o_auth_type = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 5));
    
    domain_flag = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 6));
    *o_pri = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 8)); // 此处是8大权限集

    if (USER_NAME_NOT_CONTAIN_DOMAIN == domain_flag) {
        goto_label_do_info_if_expr(real_user_len == 0, _exit, ret = VOS_ERR;
            debug_log(DLOG_ERROR, "%s failed:input param error, real_user_len is 0.", __FUNCTION__));

        ldap_domain = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 7), NULL);
        iRet = snprintf_s(o_real_user, real_user_len, real_user_len - 1, "%s@%s", user_name, ldap_domain);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    } else {
        safe_fun_ret = strncpy_s(o_real_user, real_user_len, user_name, strlen(user_name));
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    }

    
    debug_log(DLOG_DEBUG, "verify_account_get_info ldap %s %d, %d, %d, %s\n", o_real_user, *o_auth_type, *o_user_id,
        *o_pri, role_id);

_exit:

    if (NULL != input_list) {
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    }

    if (NULL != output_list) {
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    }

    if (NULL != caller_info) {
        g_slist_free_full(caller_info, (GDestroyNotify)g_variant_unref);
    }

    return ret;
}

gint32 verify_account_and_get_info(guchar from_webui_flag, const gchar *user_name, const gchar *password,
    const gchar *client, guchar *o_auth_type, guchar *o_user_id, guchar *o_pri, gchar *o_role_id, guint32 role_id_len,
    gchar *o_real_user, guint32 real_user_len)
{
    gint32 ret[VERIFY_ACCOUNT_RET_NUM] = {0};
    gint32 verify_ret = 0;
    gint32 i;
    guint32 j = 0;
    gint32 max_code = -1;

    
    ret[VERIFY_LOCAL_ACCOUNT_RET] = verify_local_account_and_get_info(from_webui_flag, user_name, password, client,
        o_auth_type, o_user_id, o_pri, o_role_id, role_id_len, o_real_user, real_user_len);
    return_val_if_expr(ret[VERIFY_LOCAL_ACCOUNT_RET] == VOS_OK, ret[VERIFY_LOCAL_ACCOUNT_RET]);

    
    ret[VERIFY_KERBEROS_ACCOUNT_RET] = verify_special_ldap_account_and_get_info(from_webui_flag, LOG_TYPE_KRB_SERVER,
        user_name, password, client, o_auth_type, o_user_id, o_pri, o_role_id, role_id_len, o_real_user, real_user_len);
    return_val_if_expr(ret[VERIFY_KERBEROS_ACCOUNT_RET] == VOS_OK, ret[VERIFY_KERBEROS_ACCOUNT_RET]);

    
    ret[VERIFY_LDAP_ACCOUNT_RET] = verify_ldap_account_and_get_info(from_webui_flag, user_name, password, client,
        o_auth_type, o_user_id, o_pri, o_role_id, role_id_len, o_real_user, real_user_len);
    return_val_if_expr(ret[VERIFY_LDAP_ACCOUNT_RET] == VOS_OK, ret[VERIFY_LDAP_ACCOUNT_RET]);
    

    for (i = 0; i < VERIFY_ACCOUNT_RET_NUM; i++) {
        verify_ret = ret[i];
        
        for (j = 0; j < G_N_ELEMENTS(g_verify_account_err_code); j++) {
            // 如果ret数组中包含多个可识别的错误码，则取其中数字值最大的错误码
            if ((verify_ret == g_verify_account_err_code[j] || dal_is_rpc_exception(verify_ret) == TRUE) &&
                (verify_ret > max_code)) {
                max_code = verify_ret;
            }
        }
        
    }

    return max_code;
    
}


LOCAL gint32 get_domain_byte(const gchar *domain_str, guint8 *domain_byte, guint8 *domain_byte_num)
{
    gint32 ret_val = VOS_OK;
    OBJ_HANDLE obj_handle = 0;
    guint8 ldap_enable = G_MAXUINT8;
    guint8 ldap_id = G_MAXUINT8;
    guint8 krb_enable = G_MAXUINT8;
    gchar ldap_domain[LDAP_USERDOMAIN_MAX_LEN + 1] = {0};
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guint8 i = 0;
    guint8 domain_num;

    return_val_if_expr(NULL == domain_str || NULL == domain_byte || NULL == domain_byte_num, VOS_ERR);

    
    (void)dal_get_object_handle_nth(LDAPCOMMON_CLASS_NAME, 0, &obj_handle);
    (void)dal_get_property_value_byte(obj_handle, LDAPCOMMON_ATTRIBUTE_ENABLE, &ldap_enable);

    
    (void)dal_get_object_handle_nth(KRBCOMMON_CLASS_NAME, 0, &obj_handle);
    (void)dal_get_property_value_byte(obj_handle, KRBCOMMON_ATTRIBUTE_ENABLE, &krb_enable);

    if (0 == g_strcmp0(domain_str, RF_SESSION_DO_LOCAL_IBMC)) {
        domain_byte[0] = LOG_TYPE_LOCAL;
        domain_num = 1;
    } else if (0 == g_strcmp0(domain_str, RF_SESSION_DO_AUTO_MATCH)) {
        
        return_val_do_info_if_fail(ENABLE == ldap_enable || ENABLE == krb_enable, VOS_OK, *domain_byte = LOG_TYPE_LOCAL;
            *domain_byte_num = 1); 
        
        domain_byte[0] = LOG_TYPE_AUTO_MATCH;
        domain_num = 1;
    }
    
    else if (0 == g_strcmp0(domain_str, RF_SESSION_DO_KRB_SSO)) {
        // 此处为查询KRB使能状态
        return_val_do_info_if_fail(ENABLE == krb_enable, VOS_ERR,
            debug_log(DLOG_ERROR, "%s: SSO failed, krb is not elabled", __FUNCTION__));
        domain_byte[0] = LOG_TYPE_LDAP_KRB_SSO;
        domain_num = 1;
    }
    
    else if (NULL != strstr(domain_str, USER_KRB_POSTFIX)) {
        return_val_do_info_if_fail(ENABLE == krb_enable, VOS_ERR,
            debug_log(DLOG_ERROR, "%s: krb is not elabled", __FUNCTION__));
        domain_byte[0] = LOG_TYPE_KRB_SERVER;
        domain_num = 1;
    } else {
        
        return_val_if_fail(ENABLE == ldap_enable, VOS_ERR);
        
        return_val_if_expr(0 == strlen(domain_str), VOS_ERR);
        
        ret_val = dfl_get_object_list(LDAP_CLASS_NAME, &obj_list);
        return_val_do_info_if_fail(VOS_OK == ret_val, VOS_ERR,
            debug_log(DLOG_ERROR, "%s:get ldap object list error", __FUNCTION__));

        for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
            ret_val = dal_get_property_value_string((OBJ_HANDLE)obj_node->data, LDAP_ATTRIBUTE_USER_DOMAIN, ldap_domain,
                sizeof(ldap_domain));
            continue_if_fail(VOS_OK == ret_val);
            
            if (!g_strcmp0(ldap_domain, domain_str)) {
                ret_val = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, LDAP_ATTRIBUTE_ID, &ldap_id);
                continue_if_fail(VOS_OK == ret_val);
                // LOG_TYPE_LDAP_SERVER1 是2，依次类推
                domain_byte[i] = ldap_id + 1;
                i++;
                break_if_fail(i < MAX_DOMAIN_BYTE_NUM);
            }
        }
        g_slist_free(obj_list);
        domain_num = i;
        
    }

    return_val_if_expr(0 == domain_num, VOS_ERR);
    *domain_byte_num = domain_num;

    return VOS_OK;
}


gint32 verify_special_ldap_account_and_get_info(guchar from_webui_flag, guint8 domain, const gchar *user_name,
    const gchar *password, const gchar *client, guchar *o_auth_type, guchar *o_user_id, guchar *o_pri, gchar *o_role_id,
    guint32 role_id_len, gchar *o_real_user, guint32 real_user_len)
{
    int iRet = -1;
    errno_t safe_fun_ret;
    gint32 ret_val = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    GSList *caller_info = NULL;
    OBJ_HANDLE obj_handle = 0;
    const gchar *str_tmp = NULL;
    gsize str_len = 0;
    guint8 user_name_flag;

    return_val_do_info_if_fail(((NULL != o_user_id) && (NULL != o_auth_type) && (NULL != o_pri) &&
        (NULL != o_role_id) && (NULL != o_auth_type)),
        VOS_ERR, debug_log(DLOG_ERROR, "%s %d: input param error", __FUNCTION__, __LINE__));

    return_val_if_fail((domain >= LOG_TYPE_LDAP_SERVER1 && domain <= LOG_TYPE_LDAP_SERVER6) ||
        (domain == LOG_TYPE_KRB_SERVER) || (domain == LOG_TYPE_LDAP_KRB_SSO) || (domain == LOG_TYPE_SSO_LOCAL) ||
        (domain == LOG_TYPE_SSO_LDAP),
        VOS_ERR);
    goto_label_do_info_if_expr(real_user_len == 0, _exit,
        debug_log(DLOG_ERROR, "%s failed:input param error, real_user_len is 0.", __FUNCTION__));

    
    ret_val =
        generate_account_verify_input(from_webui_flag, domain, &input_list, &caller_info, user_name, password, client);
    return_val_if_expr(ret_val != VOS_OK, ret_val);

    ret_val = dal_get_object_handle_nth(CLASS_USER, 0, &obj_handle);
    goto_label_do_info_if_fail((VOS_OK == ret_val), _exit,
        debug_log(DLOG_ERROR, "%s dal_get_object_handle_nth fail, user is %s", __FUNCTION__, user_name));

    ret_val = uip_call_class_method_redfish(from_webui_flag, user_name, client, obj_handle, CLASS_USER,
        METHOD_USER_AUTHUSER, AUTH_DISABLE, AUTH_PRIV_NONE, input_list, &output_list);
    goto_label_do_info_if_fail((VOS_OK == ret_val), _exit, ret_val = VOS_ERR; debug_log(DLOG_ERROR,
        "%s dfl_call_class_method %s error, value %d", __FUNCTION__, METHOD_USER_AUTHUSER, ret_val));
    

    ret_val = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
    goto_label_do_info_if_fail((VOS_OK == ret_val), _exit,
        debug_log(DLOG_ERROR, "%s dfl_call_class_method %s ret value error, value %d", __FUNCTION__,
        METHOD_USER_AUTHUSER, ret_val));

    *o_auth_type = domain;
    *o_user_id = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 2)); // 对于域用户，此处是有效的组ID集合
    *o_pri = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 8)); // 对于域用户，此处是8大权限集
    str_tmp = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 3), &str_len);
    safe_fun_ret = strncpy_s(o_role_id, role_id_len, str_tmp, str_len);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    
    user_name_flag = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 6));
    str_tmp = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 7), NULL);
    if (user_name_flag == USER_NAME_NEED_POST_BACK) {
        safe_fun_ret = strncpy_s(o_real_user, real_user_len, str_tmp, real_user_len - 1);
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    } else if (user_name_flag == USER_NAME_NOT_CONTAIN_DOMAIN) {
        iRet = snprintf_s(o_real_user, real_user_len, real_user_len - 1, "%s@%s", user_name, str_tmp);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    } else {
        safe_fun_ret = strncpy_s(o_real_user, real_user_len, user_name, real_user_len - 1);
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    }

    
_exit:

    if (NULL != input_list) {
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    }

    if (NULL != output_list) {
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    }

    if (NULL != caller_info) {
        g_slist_free_full(caller_info, (GDestroyNotify)g_variant_unref);
    }

    return ret_val;
}


LOCAL gint32 verify_account_and_get_info_by_domain(guchar from_webui_flag, json_object *body_checked,
    const gchar *user_name, const gchar *password, const gchar *client, guchar *o_auth_type, guchar *o_user_id,
    guchar *o_pri, gchar *o_role_id, guint32 role_id_len, gchar *o_real_user, guint32 real_user_len,
    json_object **message_info_jso)
{
    gint32 ret_val;
    json_object *oem_jso = NULL;
    json_object *huawei_jso = NULL;
    json_object *domain_jso = NULL;
    guint8 domain_byte[MAX_DOMAIN_BYTE_NUM] = {G_MAXINT8, G_MAXINT8, G_MAXINT8, G_MAXINT8, G_MAXINT8, G_MAXINT8};
    gchar json_pointer[PROP_VAL_LENGTH] = {0};
    guint8 domain_byte_num = 0;
    guint8 i = 0;

    return_val_do_info_if_fail(((NULL != message_info_jso) && (NULL != body_checked) && (NULL != o_user_id) &&
        (NULL != o_auth_type) && (NULL != o_pri) && (NULL != o_role_id) && (NULL != o_auth_type)),
        VOS_ERR, debug_log(DLOG_ERROR, "%s %d: input param error", __FUNCTION__, __LINE__));

    // 没有oem信息，认为是传统的校验方式
    goto_label_do_info_if_fail(TRUE == json_object_object_get_ex(body_checked, RFPROP_OEM, &oem_jso), exit,
        debug_log(DLOG_MASS, "%s: not have oem", __FUNCTION__);
        domain_byte_num++; domain_byte[0] = LOG_TYPE_AUTO_MATCH);

    // 有oem必须输完整的值，否则报属性缺失
    goto_label_do_info_if_fail(TRUE == json_object_object_get_ex(oem_jso, RFPROP_OEM_HUAWEI, &huawei_jso), error_exit,
        debug_log(DLOG_ERROR, "%s: not contain the specified field", __FUNCTION__);
        ret_val = HTTP_BAD_REQUEST; (void)snprintf_s(json_pointer, sizeof(json_pointer), sizeof(json_pointer) - 1,
        "%s/%s", RFPROP_OEM, RFPROP_OEM_HUAWEI);
        (void)create_message_info(MSGID_PROP_MISSING, json_pointer, message_info_jso, json_pointer));

    goto_label_do_info_if_fail(TRUE == json_object_object_get_ex(huawei_jso, DOMAIN, &domain_jso), error_exit,
        debug_log(DLOG_ERROR, "%s: not have domain", __FUNCTION__);
        ret_val = HTTP_BAD_REQUEST; (void)snprintf_s(json_pointer, sizeof(json_pointer), sizeof(json_pointer) - 1,
        "%s/%s/%s", RFPROP_OEM, RFPROP_OEM_HUAWEI, DOMAIN);
        (void)create_message_info(MSGID_PROP_MISSING, json_pointer, message_info_jso, json_pointer));

    ret_val = get_domain_byte(dal_json_object_get_str(domain_jso), domain_byte, &domain_byte_num);
    goto_label_do_info_if_fail(VOS_OK == ret_val, error_exit,
        debug_log(DLOG_ERROR, "%s: invalid domain is %s", __FUNCTION__, dal_json_object_get_str(domain_jso));
        ret_val = HTTP_BAD_REQUEST; (void)snprintf_s(json_pointer, sizeof(json_pointer), sizeof(json_pointer) - 1,
        "%s/%s/%s", RFPROP_OEM, RFPROP_OEM_HUAWEI, DOMAIN);
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, json_pointer, message_info_jso,
        dal_json_object_get_str(domain_jso), json_pointer));

exit:
    if (domain_byte_num == 1) {
        switch (domain_byte[0]) {
            case LOG_TYPE_AUTO_MATCH:
                ret_val = verify_account_and_get_info(from_webui_flag, user_name, password, client, o_auth_type,
                    o_user_id, o_pri, o_role_id, role_id_len, o_real_user, real_user_len);
                break;

            case LOG_TYPE_LOCAL:
                ret_val = verify_local_account_and_get_info(from_webui_flag, user_name, password, client, o_auth_type,
                    o_user_id, o_pri, o_role_id, role_id_len, o_real_user, real_user_len);
                break;

            case LOG_TYPE_LDAP_SERVER1:
            case LOG_TYPE_LDAP_SERVER2:
            case LOG_TYPE_LDAP_SERVER3:
            case LOG_TYPE_LDAP_SERVER4:
            case LOG_TYPE_LDAP_SERVER5:
            case LOG_TYPE_LDAP_SERVER6:

            
            case LOG_TYPE_LDAP_KRB_SSO:
                *o_auth_type = domain_byte[0];
                ret_val = verify_special_ldap_account_and_get_info(from_webui_flag, domain_byte[0], user_name, password,
                    client, o_auth_type, o_user_id, o_pri, o_role_id, role_id_len, o_real_user, real_user_len);
                break;

            
            case LOG_TYPE_KRB_SERVER:
                *o_auth_type = domain_byte[0];
                ret_val = verify_special_ldap_account_and_get_info(from_webui_flag, domain_byte[0], user_name, password,
                    client, o_auth_type, o_user_id, o_pri, o_role_id, role_id_len, o_real_user, real_user_len);
                break;

            default:
                debug_log(DLOG_ERROR, "%s: invalid domain byte is %d", __FUNCTION__, domain_byte[0]);
                ret_val = VOS_ERR;
        }
    }
    
    else {
        for (i = 0; i < domain_byte_num; i++) {
            *o_auth_type = domain_byte[i];
            ret_val = verify_special_ldap_account_and_get_info(from_webui_flag, domain_byte[i], user_name, password,
                client, o_auth_type, o_user_id, o_pri, o_role_id, role_id_len, o_real_user, real_user_len);
            break_if_expr(VOS_OK == ret_val);
        }
    }

error_exit:
    return ret_val;
}


LOCAL void generate_operation_user_full_name(guint8 auth_type, const gchar *user_name, gchar *full_user_name,
    guint32 full_name_size)
{
#define OP_LOG_SSO_ID "SSO user(KRB)"

    gchar domain_name[TMP_STR_MAX] = {0};
    OBJ_HANDLE obj_handle = 0;
    gint32 ret_val = VOS_OK;

    return_do_info_if_expr(user_name == NULL || full_user_name == NULL || full_name_size == 0,
        debug_log(DLOG_ERROR, "%s: input para error", __FUNCTION__));

    // SSO认证失败时，前端下发的用户名是ST字符串，修改支持操作日志记录合理
    if (auth_type == LOG_TYPE_LDAP_KRB_SSO) {
        (void)memset_s(full_user_name, full_name_size, 0, full_name_size);
        (void)strncpy_s(full_user_name, full_name_size, OP_LOG_SSO_ID, strlen(OP_LOG_SSO_ID));
    } else if (auth_type == LOG_TYPE_KRB_SERVER) {
        ret_val = dal_get_specific_object_byte(LDAP_CLASS_NAME, LDAP_ATTRIBUTE_ID, LOG_TYPE_KRB_SERVER, &obj_handle);
        return_do_info_if_expr(VOS_OK != ret_val,
            debug_log(DLOG_ERROR, "%s: get krb server handle failed, ret is %d", __FUNCTION__, ret_val));

        if (NULL == strrchr(user_name, '@')) {
            ret_val =
                dal_get_property_value_string(obj_handle, LDAP_ATTRIBUTE_USER_DOMAIN, domain_name, sizeof(domain_name));
            return_do_info_if_expr(VOS_OK != ret_val,
                debug_log(DLOG_ERROR, "%s: get krb server handle failed, ret is %d", __FUNCTION__, ret_val));

            ret_val = snprintf_truncated_s(full_user_name, full_name_size, "%s@%s%s", user_name, domain_name,
                USER_KRB_POSTFIX);
            do_val_if_expr(ret_val <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret_val));
        } else {
            ret_val = snprintf_truncated_s(full_user_name, full_name_size, "%s%s", user_name, USER_KRB_POSTFIX);
            do_val_if_expr(ret_val <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret_val));
        }
    } else if (auth_type >= LOG_TYPE_LDAP_SERVER1 && auth_type <= LOG_TYPE_LDAP_SERVER6) {
        ret_val = dal_get_specific_object_byte(LDAP_CLASS_NAME, LDAP_ATTRIBUTE_ID, auth_type - 1, &obj_handle);
        return_do_info_if_expr(VOS_OK != ret_val,
            debug_log(DLOG_ERROR, "%s: get ldap server handle failed, ret is %d", __FUNCTION__, ret_val));

        ret_val =
            dal_get_property_value_string(obj_handle, LDAP_ATTRIBUTE_USER_DOMAIN, domain_name, sizeof(domain_name));
        return_do_info_if_expr(VOS_OK != ret_val,
            debug_log(DLOG_ERROR, "%s: get ldap server domain name failed, ret is %d", __FUNCTION__, ret_val));

        ret_val = snprintf_s(full_user_name, full_name_size, full_name_size - 1, "%s@%s", user_name, domain_name);
        return_do_info_if_expr(ret_val <= 0,
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret_val));
    } else {
        // 本地用户
        ret_val = strncpy_s(full_user_name, full_name_size, user_name, strlen(user_name));
        do_val_if_expr(ret_val != EOK, debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, ret_val));
    }

    return;
}


LOCAL void create_retval_ibmc_message(gint32 ret_val, json_object **message_info_jso)
{
    return_if_expr(NULL == message_info_jso);

    switch (ret_val) {
            
        case USER_UNSUPPORT:
        case INVALID_PASSWORD:
        case USER_DONT_EXIST:
        case UUSER_USERNAME_TOO_LONG:
        case UUSER_USERPASS_TOO_LONG:
            create_message_info(MSGID_LOGIN_AUTH_FAILED, NULL, message_info_jso);
            break;

        case USER_NO_ACCESS:
            create_message_info(MSGID_NO_ACCESS, NULL, message_info_jso);
            break;

        case USER_PASSWORD_EXPIRED:
            create_message_info(MSGID_USER_PSWD_EXPIRED, NULL, message_info_jso);
            break;

        case USER_LOGIN_LIMITED:
            create_message_info(MSGID_USER_LOGIN_RESTRICTED, NULL, message_info_jso);
            break;

        case USER_IS_LOCKED:
            create_message_info(MSGID_USER_LOCKED, NULL, message_info_jso);
            break;
        case USER_LDAP_LOGIN_FAIL:
            create_message_info(MSGID_LDAP_AUTH_FAIL, NULL, message_info_jso);
            break;
        default:
            (void)create_message_info(MSGID_LOGIN_ATTEMPT_FAILED, NULL, message_info_jso);
            
    }

    return;
}

LOCAL void record_loginfail_security_log(gboolean is_from_webui, const gchar *user_name, const gchar *ip)
{
    if (is_from_webui) {
        security_log("%s(%s) login failed", user_name, ip);
    }
}


gint32 create_session(PROVIDER_PARAS_S *i_param, json_object *body_jso_checked, gchar *o_id, guint32 o_id_len,
    json_object *o_message_array_jso)
{
    errno_t safe_fun_ret = EOK;
    gint32 ret;
    OBJ_HANDLE obj_handle;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    GSList *caller_info = NULL;
    json_object *val_jso = NULL;
    const gchar *user_name = NULL;
    const gchar *word = NULL; // 密码
    json_bool is_ret;
    gchar role_id[USER_ROLEID_MAX_LEN + 1] = {0};
    guchar auth_type = 0;
    guchar user_id = 0;
    guchar pri = 0;
    json_object *message_info_jso = NULL;
    const gchar *id = NULL;
    gchar real_user[SESSION_USER_NAME_MAX_LEN + 1] = {0};
    guint32 last_login_time = 0;
    gchar last_login_ip[SESSION_IP_LEN];
    OBJ_HANDLE user_obj_handle = 0;

    gchar full_user_name[LDAP_USER_NAME_MAX_LEN + USER_KRB_POSTFIX_LEN + 1] = {0}; // 增加(KRB)五个字符空间

    // 开启双因素后 web接口只能通过证书认证的方式创建session
    goto_label_do_info_if_expr(is_web_request_at_mutual_auth(i_param) == VOS_OK, error_exit, ret = HTTP_BAD_REQUEST;
        debug_log(DLOG_ERROR, "%s create web session not allowed when mutual auth enable", __FUNCTION__);
        clear_session_passwd(body_jso_checked);
        (void)create_message_info(MSGID_RSC_CANNOT_CREATED, NULL, &message_info_jso);
        json_object_array_add(o_message_array_jso, message_info_jso));

    // 获取用户名
    is_ret = json_object_object_get_ex(body_jso_checked, USERNAME, &val_jso);
    
    goto_label_do_info_if_fail(is_ret, error_exit, ret = HTTP_INTERNAL_SERVER_ERROR;
        debug_log(DLOG_ERROR, "%s json_object_object_get_ex %s error", __FUNCTION__, USERNAME);
        clear_session_passwd(body_jso_checked); (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(o_message_array_jso, message_info_jso));
    
    user_name = dal_json_object_get_str(val_jso);
    val_jso = NULL;

    // 获取密码
    is_ret = json_object_object_get_ex(body_jso_checked, PASSWORD, &val_jso);
    
    goto_label_do_info_if_fail(is_ret, error_exit, ret = HTTP_INTERNAL_SERVER_ERROR;
        debug_log(DLOG_ERROR, "%s json_object_object_get_ex %s error", __FUNCTION__, USERNAME);
        clear_session_passwd(body_jso_checked); (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(o_message_array_jso, message_info_jso));
    
    word = dal_json_object_get_str(val_jso);

    
    // 校验账户,同时获取部分信息用于添加会话
    
    ret = verify_account_and_get_info_by_domain(i_param->is_from_webui, body_jso_checked, user_name, word,
        i_param->client, &auth_type, &user_id, &pri, role_id, USER_ROLEID_MAX_LEN + 1, real_user,
        SESSION_USER_NAME_MAX_LEN + 1, &message_info_jso);
    
    
    clear_session_passwd(body_jso_checked);

    generate_operation_user_full_name(auth_type, user_name, full_user_name, sizeof(full_user_name));

    // 填充调用信息，用于方法内记录操作日志，接口填充Redfish
    
    caller_info =
        g_slist_append(caller_info, g_variant_new_string(dal_rf_get_op_log_module_name(i_param->is_from_webui)));
    caller_info = g_slist_append(caller_info, (VOS_OK == ret) ? g_variant_new_string((const gchar *)real_user) :
                                                                g_variant_new_string((const gchar *)full_user_name));
    caller_info = g_slist_append(caller_info, g_variant_new_string(i_param->client));
    

    goto_label_do_info_if_fail(VOS_OK == ret, error_exit,
        proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "%s(%s) login failed\n", full_user_name,
        i_param->client);
        do_if_fail(NULL == message_info_jso, (void)json_object_array_add(o_message_array_jso, message_info_jso));
        debug_log(DLOG_ERROR, "%s : verify_account_and_get_info_by_domain fail", __FUNCTION__));

    
    if (LOG_TYPE_LOCAL == auth_type && i_param->is_from_webui) {
        i_param->web_user_last_login_info.request_is_create = TRUE;
        // 根据user id 获取对象，再获取上次登录的时间及IP
        (void)dal_get_specific_object_byte(CLASS_USER, PROPERTY_USER_ID, user_id, &user_obj_handle);
        ret = dal_get_property_value_uint32(user_obj_handle, PROPERTY_USER_LAST_LOGIN_TIME, &last_login_time);
        do_val_if_expr(VOS_OK == ret, (i_param->web_user_last_login_info.user_last_login_time = last_login_time));

        ret = dal_get_property_value_string(user_obj_handle, PROPERTY_USER_LAST_LOGIN_IP, last_login_ip,
            sizeof(last_login_ip));
        do_val_if_expr(VOS_OK == ret, (void)strncpy_s(i_param->web_user_last_login_info.user_last_login_ip,
            STRING_LEN_MAX + 1, (const gchar *)last_login_ip, STRING_LEN_MAX));
    }

    
    // 获取会话类对象句柄
    ret = dal_get_object_handle_nth(CLASS_SESSION, 0, &obj_handle);
    goto_label_do_info_if_fail(VOS_OK == ret, error_exit, ret = HTTP_INTERNAL_SERVER_ERROR;
        debug_log(DLOG_ERROR, "%s dal_get_object_handle_nth error", __FUNCTION__);
        (void)create_message_info(MSGID_ACCOUNT_NO_EXIST, NULL, &message_info_jso);
        (void)json_object_array_add(o_message_array_jso, message_info_jso));

    
    // 填充AddSession方法需要的入参信息
    input_list = g_slist_append(input_list, g_variant_new_byte(auth_type));
    input_list = g_slist_append(input_list, g_variant_new_byte(pri)); // privilege,角色对应的8大权限
    input_list = g_slist_append(input_list, g_variant_new_string(i_param->client));
    input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)real_user));
    input_list = g_slist_append(input_list, g_variant_new_byte(user_id)); // ldap用户为有效的group id集合
    input_list = g_slist_append(input_list, g_variant_new_byte(
        i_param->is_from_webui ? REDFISH_WEBUI : REDFISH_UNKNOWN)); // 浏览器类型,redfish会话当前不涉及
    input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)role_id)); // 角色ID

    
    do_val_if_expr((REDFISH_REQ_FROM_WEBUI == i_param->is_from_webui),
        (i_param->is_from_webui = REDFISH_REQ_FOR_WEB_AUTH));
    ret = uip_call_class_method_redfish(i_param->is_from_webui, (const gchar *)real_user, i_param->client, obj_handle,
        CLASS_SESSION, METHOD_SESSION_ADDSESSION, AUTH_DISABLE, AUTH_PRIV_NONE, input_list, &output_list);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    

    goto_label_do_info_if_fail(VOS_OK == ret, error_exit,
        debug_log(DLOG_ERROR, "%s: call method %s error, ret = 0x%x", __FUNCTION__, METHOD_SESSION_ADDSESSION, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(o_message_array_jso, message_info_jso); ret = HTTP_INTERNAL_SERVER_ERROR);

    // 获取返回值,为x-auth-token的checksum sha256加密值
    id = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    // 如果无有效id返回,则认为会话创建已满,其他场景方法返回值直接就为错误码
    if (NULL == id || 0 == strlen(id)) {
        debug_log(DLOG_DEBUG, "%s : session is full", __FUNCTION__);
        ret = HTTP_BAD_REQUEST;
        (void)create_message_info(MSGID_SESSION_LIMIT_EXCEED, NULL, &message_info_jso);
        (void)json_object_array_add(o_message_array_jso, message_info_jso);
    } else {
        safe_fun_ret = strncpy_s(o_id, o_id_len, id, strlen(id));
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    }

error_exit:

    do_info_if_true(NULL != output_list, g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref));
    do_info_if_true(NULL != caller_info, g_slist_free_full(caller_info, (GDestroyNotify)g_variant_unref));

    if ((VOS_OK != ret) && (HTTP_INTERNAL_SERVER_ERROR != ret) && (NULL == message_info_jso)) {
        record_loginfail_security_log((gboolean)i_param->is_from_webui, user_name, (const gchar *)i_param->client);
        create_retval_ibmc_message(ret, &message_info_jso);
        debug_log(DLOG_ERROR, "%s %d: verify_account_and_get_info fail, ret is 0x%x", __FUNCTION__, __LINE__, ret);
        (void)json_object_array_add(o_message_array_jso, message_info_jso);
        ret = HTTP_BAD_REQUEST;
    }

    return ret;
}


LOCAL gint32 is_web_request_at_mutual_auth(PROVIDER_PARAS_S *i_param)
{
    OBJ_HANDLE mutual_auth_obj = 0;
    guint8 mutual_state = 0;

    if (i_param->is_from_webui == 0) {
        return VOS_ERR;
    }

    (void)dal_get_object_handle_nth(MUTUAL_AUTH_CLASS_NAME, 0, &mutual_auth_obj);
    (void)dal_get_property_value_byte(mutual_auth_obj, MUTUAL_AUTH_STATE, &mutual_state);

    return mutual_state == 1 ? VOS_OK : VOS_ERR;
}


LOCAL void clear_session_passwd(json_object *body_jso_checked)
{
    return_if_expr(body_jso_checked == NULL);
    json_object_clear_string(body_jso_checked, RFACTION_PARAM_PWD);
    json_object_object_del(body_jso_checked, PASSWORD);
}

LOCAL gint32 delete_specified_session(PROVIDER_PARAS_S *i_param, OBJ_HANDLE obj_handle, json_object *session_jso)
{
    GSList *input_list = NULL;
    const gchar *temp_str = NULL;
    json_object *value_jso = NULL;
    gint32 ret;

    return_val_do_info_if_fail((NULL != i_param && NULL != session_jso), VOS_ERR,
        debug_log(DLOG_ERROR, "%s %d: input param error", __FUNCTION__, __LINE__));

    (void)json_object_object_get_ex(session_jso, RF_SESSION_ID, &value_jso);
    temp_str = dal_json_object_get_str(value_jso);
    input_list = g_slist_append(input_list, g_variant_new_string(temp_str));
    temp_str = NULL;
    value_jso = NULL;

    (void)json_object_object_get_ex(session_jso, RF_SESSION_TYPE, &value_jso);
    temp_str = dal_json_object_get_str(value_jso);
    input_list = g_slist_append(input_list, g_variant_new_string(temp_str));
    temp_str = NULL;
    value_jso = NULL;

    (void)json_object_object_get_ex(session_jso, RF_SESSION_NAME, &value_jso);
    temp_str = dal_json_object_get_str(value_jso);
    input_list = g_slist_append(input_list, g_variant_new_string(temp_str));
    temp_str = NULL;
    value_jso = NULL;

    (void)json_object_object_get_ex(session_jso, RF_SESSION_IP, &value_jso);
    temp_str = dal_json_object_get_str(value_jso);
    input_list = g_slist_append(input_list, g_variant_new_string(temp_str));

    
    ret = uip_call_class_method_redfish(i_param->is_from_webui, i_param->user_name, i_param->client, obj_handle,
        CLASS_SESSION, METHOD_SESSION_LOGOUTUSER, AUTH_DISABLE, AUTH_PRIV_NONE, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s %d: call class method %s failed, ret=%d.", __FUNCTION__, __LINE__,
        METHOD_SESSION_LOGOUTUSER, ret));
    

    return VOS_OK;
}


gint32 delete_session(PROVIDER_PARAS_S *i_param, json_object *message_array_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    json_object *message_info_jso = NULL;
    const gchar *session_list_str = NULL;
    json_object *session_collection_jso = NULL;
    json_object *session_jso = NULL;
    json_object *value_jso = NULL;
    gint32 len;
    gint32 array_index;
    json_bool ret_json;

    return_val_do_info_if_fail((NULL != i_param), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s input param error", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(message_array_jso, message_info_jso));

    (void)dal_get_object_handle_nth(CLASS_SESSION, 0, &obj_handle);

    // 判断是否删除本会话,如果是则调用DelSession方法退出会话
    if (0 == g_strcmp0(i_param->session_id, i_param->member_id)) {
        input_list = g_slist_append(input_list, g_variant_new_string(i_param->session_id));

        
        ret = uip_call_class_method_redfish(i_param->is_from_webui, i_param->user_name, i_param->client, obj_handle,
            CLASS_SESSION, METHOD_SESSION_DELSESSION, AUTH_DISABLE, AUTH_PRIV_NONE, input_list, NULL);
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s dfl_call_class_method %s error, ret is 0x%x", __FUNCTION__,
            METHOD_SESSION_DELSESSION, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
            json_object_array_add(message_array_jso, message_info_jso));
        

        (void)create_message_info(MSGID_SUCCESS, NULL, &message_info_jso);
        (void)json_object_array_add(message_array_jso, message_info_jso);
        return VOS_OK;
    }

    // 非删除本会话场景，继续向下执行，判断是否具有用户管理权限
    if (0 == (i_param->user_role_privilege & USERROLE_USERMGNT)) {
        debug_log(DLOG_DEBUG, "%s %d: user %s has no privilege to delete session %s", __FUNCTION__, __LINE__,
            i_param->user_name, i_param->member_id);
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, &message_info_jso);
        (void)json_object_array_add(message_array_jso, message_info_jso);
        return HTTP_FORBIDDEN;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(i_param->session_id));

    // 调用方法获取会话集合
    
    ret = dfl_call_class_method(obj_handle, METHOD_SESSION_GETSESSIONLIST, NULL, input_list, &output_list);
    
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s dfl_call_class_method %s error, ret is 0x%x", __FUNCTION__,
        METHOD_SESSION_GETSESSIONLIST, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(message_array_jso, message_info_jso));

    session_list_str = g_variant_get_string((GVariant *)(g_slist_nth_data(output_list, 0)), NULL);
    return_val_do_info_if_fail((NULL != session_list_str) && (0 != strlen(session_list_str)),
        HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s dfl_call_class_method %s error, output list ret is 0x%x", __FUNCTION__,
        METHOD_SESSION_GETSESSIONLIST, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(message_array_jso, message_info_jso);
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref));

    session_collection_jso = json_tokener_parse(session_list_str);
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

    // 遍历所有会话信息，通过session_id查找会话是否存在
    len = json_object_array_length(session_collection_jso);

    for (array_index = 0; array_index < len; array_index++) {
        session_jso = json_object_array_get_idx(session_collection_jso, array_index);
        return_val_do_info_if_fail((NULL != session_jso), HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s json_object_array_get_idx error", __FUNCTION__);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
            json_object_array_add(message_array_jso, message_info_jso); json_object_put(session_collection_jso));

        ret_json = json_object_object_get_ex(session_jso, RF_SESSION_CHECKSUM, &value_jso);
        return_val_do_info_if_fail(ret_json && (NULL != value_jso), HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s json_object_object_get_ex error", __FUNCTION__);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
            json_object_array_add(message_array_jso, message_info_jso); json_object_put(session_collection_jso));

        if (0 == g_strcmp0(i_param->member_id, dal_json_object_get_str(value_jso))) {
            break;
        }

        session_jso = NULL;
        value_jso = NULL;
    }

    // 不存在则直接返回404
    return_val_do_info_if_fail(array_index < len, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s %d :member id %s is illegal", __FUNCTION__, __LINE__, i_param->member_id);
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_info_jso, i_param->uri);
        (void)json_object_array_add(message_array_jso, message_info_jso); json_object_put(session_collection_jso));

    // 存在则调用方法删除会话
    ret = delete_specified_session(i_param, obj_handle, session_jso);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s delete session by method failed", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(message_array_jso, message_info_jso); json_object_put(session_collection_jso));

    json_object_put(session_collection_jso);
    (void)create_message_info(MSGID_SUCCESS, NULL, &message_info_jso);
    (void)json_object_array_add(message_array_jso, message_info_jso);
    return VOS_OK;
}


gint32 generate_del_session_response_headers(PROVIDER_PARAS_S *i_param, json_object **header_array_jso,
    const gchar *uri, json_object *message_array_jso)
{
    gint32 ret;

    // 生成公共响应头
    ret = generate_common_response_headers(header_array_jso, uri, message_array_jso);

    return ret;
}
