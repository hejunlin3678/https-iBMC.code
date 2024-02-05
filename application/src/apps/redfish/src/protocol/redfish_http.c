
#include "pme/pme.h"
#include "pme_app/pme_app.h"

#include "redfish_http.h"
#include "redfish_message.h"
#include "redfish_defs.h"
#include "redfish_class.h"
#include "json_patch_util.h"
#include "pme_app/smm/smm_public.h"
#include "redfish_forward.h"

#define SET_FLOAT_IP_ADDR "floateth2"

#define SET_FLOAT_IP "floatip"

LOCAL gint32 handle_get(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param, HTTP_RESPONSE_DATA_S *response);
LOCAL gint32 handle_patch(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param, HTTP_RESPONSE_DATA_S *response);
LOCAL gint32 handle_create(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param, HTTP_RESPONSE_DATA_S *response);
LOCAL gint32 handle_delete(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param, HTTP_RESPONSE_DATA_S *response);
LOCAL gint32 handle_action(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param, HTTP_RESPONSE_DATA_S *response);
LOCAL gint32 verify_ifmatch(const REQUEST_INFO_S *request_info, json_object *redfish_jso);
LOCAL gint32 verify_ifnonematch(const REQUEST_INFO_S *request_info, json_object *o_header_jso);

LOCAL gint32 generate_get_response_headers(guint8 from_webui, json_object *i_resource_jso, REQUEST_INFO_S *request_info,
    json_object **i_header_array_jso, json_object *message_array_jso);

LOCAL gint32 generate_members_next_link(const REQUEST_INFO_S *request_info, json_object *response_jso, gint32 index,
    gint32 count);
LOCAL gint32 get_skip_and_top(const REQUEST_INFO_S *request_info, json_object **message_info_jso, gint32 *skip,
    gint32 *top, guint32 *prara_flag);
LOCAL gint32 skip_and_top_param_verify(gint32 member_count, json_object **message_info_jso, gint32 *skip, gint32 *top,
    guint32 para_flag);


LOCAL gint32 generate_action_response_message(property_handler_t pfn_action, PROVIDER_PARAS_S *i_paras,
    json_object **task_jso, json_object **message_array_jso);



LOCAL gint32 expand_period_hyperlinks(guint32 depth, json_object *redfish_jso);

LOCAL gint32 url_decode_str(const gchar *url_pwd, gchar *uncode_pwd, size_t len);


LOCAL const REQUEST_HANDLER_S request_handle[] = {
    {E_METHOD_NULL, NULL},
    {E_METHOD_GET, handle_get},
    {E_METHOD_CREATE, handle_create},
    {E_METHOD_ACTION, handle_action},
    {E_METHOD_PATCH, handle_patch},
    {E_METHOD_PUT, NULL},
    {E_METHOD_DELETE, handle_delete},
    {E_METHOD_POST, NULL},
    {E_METHOD_ALL, NULL}
};


LOCAL HTTP_HEADER_S common_headers[] = {
    {RESP_HEADER_ODATA_VERSION,   "4.0"},
    {RESP_HEADER_CONTENT_TYPE,    "application/json;charset=utf-8"}
};

LOCAL guint8 g_sys_lockdown_state = 0;

LOCAL GHashTable *g_http_resp_title_hash = NULL;


LOCAL GHashTable *g_http_request_heard_hash = NULL;

LOCAL AUTH_FREE_S g_authenticated_free_uri[] = {
    {E_METHOD_ALL, "/redfish"},
    
    
    {E_METHOD_GET, "/redfish/v1"},
    
    {E_METHOD_ALL, ODATA_PREFIX},
    {E_METHOD_ALL, METADATA_URI_PREFIX},
    {E_METHOD_CREATE, "/redfish/v1/SessionService/Sessions"},
    {E_METHOD_NULL, NULL}
};

LOCAL RE_AUTH_S g_reauthentication_uri[] = {
    {E_METHOD_CREATE, "^/redfish/v1/AccountService/Accounts$", NULL, RE_AUTHENTICATION_ADD_USER},
    {E_METHOD_DELETE, "^/redfish/v1/AccountService/Accounts/[0-9]+$", NULL, RE_AUTHENTICATION_DELETE_USER},
    {E_METHOD_PATCH, "^/redfish/v1/AccountService/Accounts/[0-9]+$", NULL, RE_AUTHENTICATION_SET_USER},
    {E_METHOD_PATCH, "^/redfish/v1/AccountService/LdapService/LdapControllers/[0-9]+$", NULL, RE_AUTHENTICATION_SET_LDAP},
    {E_METHOD_ACTION, "^/redfish/v1/AccountService/LdapService/LdapControllers/[0-9]+$", ACTION_TAIL_LDAP_IMPORT_CRL, RE_AUTHENTICATION_IMPORT_LDAP_CRL},
    {E_METHOD_ACTION, "^/redfish/v1/AccountService/LdapService/LdapControllers/[0-9]+$", ACTION_TAIL_LDAP_DELETE_CRL,
        RE_AUTHENTICATION_DELETE_LDAP_CRL},
    {E_METHOD_PATCH, "^/redfish/v1/AccountService/KerberosService/KerberosControllers/[0-9]+$", NULL, RE_AUTHENTICATION_SET_KERBEROS},
    {E_METHOD_ACTION, "^/redfish/v1/AccountService/KerberosService/KerberosControllers/[0-9]+$", ACTION_TAIL_KRB_IMPORT_KEY_TABLE, RE_AUTHENTICATION_IMPORT_KERBEROS_KEY},
    {E_METHOD_PATCH, "^/redfish/v1/AccountService/Roles/[a-zA-Z0-9]+$", NULL, RE_AUTHENTICATION_SET_CUSTOMIZED_ROLE},
    {E_METHOD_ACTION, "^/redfish/v1/AccountService/Accounts/[0-9]+$", ACTION_TAIL_IMPORT_SSH_PUBLIC_KEY, RE_AUTHENTICATION_IMPORT_SSH_PUBLIC_KEY},
    {E_METHOD_ACTION, "^/redfish/v1/AccountService/Accounts/[0-9]+$", "Oem/Huawei/Actions/Account.DeleteSSHPublicKey", RE_AUTHENTICATION_DELETE_SSH_PUBLIC_KEY},
    {E_METHOD_PATCH, "^/redfish/v1/Managers/[a-zA-Z0-9]+/VNCService$", PROPERTY_MANAGER_VNCPASSWORD, RE_AUTHENTICATION_SET_VNC_PASSWORD},
    {E_METHOD_ACTION, "^/redfish/v1/Managers/[a-zA-Z0-9]+/SnmpService$", "Actions/SnmpService.ConfigSnmpV3PrivPasswd", RE_AUTHENTICATION_SET_SNMPV3_PASSWORD},
    {E_METHOD_CREATE, "^/redfish/v1/AccountService/Oem/Huawei/Nodes$", NULL, RE_AUTHENTICATION_ADD_NODE_USER},
    {E_METHOD_PATCH, "^/redfish/v1/accountservice/oem/huawei/nodes/[0-9]+_slot[0-9]+$", NULL, RE_AUTHENTICATION_SET_NODE_USER},
    {E_METHOD_DELETE, "^/redfish/v1/accountservice/oem/huawei/nodes/[0-9]+_slot[0-9]+$", NULL, RE_AUTHENTICATION_DELETE_NODE_USER},
    {E_METHOD_ACTION, "^/redfish/v1/AccountService/LdapService/LdapControllers/[0-9]+$", ACTION_TAIL_LDAP_IMPORT_CERT, RE_AUTHENTICATION_IMPORT_LDAP_CERTIFICATE},
    {E_METHOD_NULL, NULL}
};


#ifndef SECURITY_ENHANCED_COMPATIBLE_BOARD_V3
LOCAL AUTH_FREE_S g_password_reset_free_uri[] = {
    // 在强制修改密码情况下，允许用户配置自身
    {E_METHOD_PATCH, "^/redfish/v1/accountservice/accounts/[a-zA-Z0-9]+$"},
    // 配置自身权限，包含管理用户自身的SSH公钥
    {E_METHOD_ACTION, "^/redfish/v1/accountservice/accounts/[a-zA-Z0-9]+$"},
    {E_METHOD_NULL, NULL}
};
#endif

LOCAL const gchar* g_large_response_uri_pattern[] = {
    BIOS_ATTRIBUTE_REGISTRY_REGEX,
    URI_PATTERN_DATA_DA_REPORT
};


LOCAL gint32 get_resp_title_hash(GHashTable **resp_hash)
{
    return_val_if_fail((NULL != g_http_resp_title_hash) && (NULL != resp_hash), VOS_ERR);
    *resp_hash = g_http_resp_title_hash;
    return VOS_OK;
}


LOCAL gint32 is_static_rsc_uri(const gchar *uri_lower)
{
    

    // 对于jsonschemas、odata metadata、 registrystore、 schemastore需单独处理
    // 仅支持GET;配置文件中不配置AllowedOperation;
    if (g_str_has_prefix(uri_lower, SCHEMA_PREFIX) || g_str_has_prefix(uri_lower, ODATA_PREFIX) ||
        g_str_has_prefix(uri_lower, METADATA_URI_PREFIX) || g_str_has_prefix(uri_lower, REGISTRY_STORE_PREFIX) ||
        g_str_has_prefix(uri_lower, SCHEMASTORE_PREFIX)) {
        debug_log(DLOG_DEBUG, "%s %d: this is a specific uri, %s", __FUNCTION__, __LINE__, uri_lower);
        return VOS_OK;
    }

    

    return VOS_ERR;
}


LOCAL gint32 is_large_response_uri(const gchar *uri_lower)
{
    guint32 i;

    for (i = 0; i < G_N_ELEMENTS(g_large_response_uri_pattern); i++) {
        if (g_large_response_uri_pattern[i] != NULL && g_regex_match_simple(g_large_response_uri_pattern[i], uri_lower,
            G_REGEX_OPTIMIZE, (GRegexMatchFlags)0) == TRUE) {
            return VOS_OK;
        }
    }

    return VOS_ERR;
}


void generate_redfish_resp_data_from_jso(json_object *response_jso, HTTP_RESPONSE_DATA_S *response)
{
    json_object *jso_val = NULL;

    if (response_jso == NULL || response == NULL) {
        return;
    }

    if (json_object_object_get_ex(response_jso, RESPONSE_BODY, &jso_val) == TRUE && jso_val != NULL) {
        if (json_object_get_type(jso_val) == json_type_string) {
            response->resp_body = g_strdup(dal_json_object_get_str(jso_val));
        } else {
            response->resp_body = g_strdup(json_object_to_json_string_ext(jso_val, JSON_C_TO_STRING_NOSLASHESCAPE));
        }
        jso_val = NULL;
    }

    if (json_object_object_get_ex(response_jso, RESPONSE_HEADERS, &jso_val) == TRUE && jso_val != NULL) {
        response->resp_head = g_strdup(json_object_to_json_string_ext(jso_val, JSON_C_TO_STRING_NOSLASHESCAPE));
        jso_val = NULL;
    }

    if (json_object_object_get_ex(response_jso, RESPONSE_STATUS_CODE, &jso_val) == TRUE && jso_val != NULL) {
        response->resp_status = g_strdup(dal_json_object_get_str(jso_val));
    }
    return;
}

typedef struct {
    const gchar uri_pattern[MAX_URI_LENGTH];
    gboolean include_uri;
    const gchar path_suffix[MAX_FILEPATH_LENGTH];
} SPECIFIC_URI_RESOURCE;

LOCAL SPECIFIC_URI_RESOURCE g_specific_uri_resource[] = {
    {"^/redfish$", TRUE, "/index.json"},
    {"^/redfish/v1/jsonschemas/.+", TRUE, "/index.json"},
    {"^/redfish/v1/odata$", TRUE, "/index.json"},
    {"^/redfish/v1/registries/base\\.v1_0_0$", TRUE, "/index.json"},
    {"^/redfish/v1/registries/ibmc\\.v1_0_0$", TRUE, "/index.json"},
    {"^/redfish/v1/registrystore/messages/.+\\.json$", TRUE, ""},
    {"^/redfish/v1/schemastore/.+\\.json$", TRUE, ""},
    {"^/redfish/v1/\\$metadata$", FALSE, "/redfish/v1/metadata/index.xml"},
    {"^/redfish/v1/\\$metadata#.*", FALSE, "/redfish/v1/metadata/index.xml"}
};


LOCAL gint32 get_resource_path_from_uri(const gchar *uri_lower, gchar *resource_path, guint32 buffer_len)
{
    gint32 iRet;
    gchar file_name[MAX_PATH_LENGTH] = {0};
    gchar *resolved_path = NULL;

    gint32 i = 0;
    for (; i < ARRAY_SIZE(g_specific_uri_resource); ++i) {
        if (g_regex_match_simple(g_specific_uri_resource[i].uri_pattern, uri_lower, 0, 0)) {
            break;
        }
    }
    if (i >= ARRAY_SIZE(g_specific_uri_resource)) {
        iRet = snprintf_s(file_name, MAX_PATH_LENGTH - 1, MAX_PATH_LENGTH - 2, "%s%s/%s", CONFG_FILE_PREFIX, uri_lower,
            CONFG_FILE_SUFFIX);
    } else {
        iRet = sprintf_s(file_name, sizeof(file_name), CONFG_FILE_PREFIX"%s%s",
            g_specific_uri_resource[i].include_uri ? uri_lower : "", g_specific_uri_resource[i].path_suffix);
    }

    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s failed, ret:%d", __FUNCTION__, iRet);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (g_file_test((const gchar *)file_name, G_FILE_TEST_IS_REGULAR) == FALSE) {
        return HTTP_NOT_FOUND;
    }

    // 对外部URI拼接生成的路径进行realpath转换,必须转换前后保持一致,否则认为输入非法
    resolved_path = realpath(file_name, NULL);
    if ((resolved_path == NULL) || (g_strcmp0(file_name, resolved_path) != 0)) {
        debug_log(DLOG_ERROR, "%s %d: check file name and real path fail, file_name is %s",
            __FUNCTION__, __LINE__, file_name);

        if (resolved_path != NULL) {
            g_free(resolved_path);
        }
        return HTTP_NOT_FOUND;
    }

    g_free(resolved_path);
    iRet = strncpy_s(resource_path, buffer_len, file_name, strlen(file_name));
    if (iRet != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s failed, ret:%d", __FUNCTION__, iRet);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return VOS_OK;
}


gint32 get_res_info_from_uri(REQUEST_INFO_S *i_request_info, PROVIDER_PARAS_S *provider_param,
    json_object *message_array_jso)
{
    int iRet = -1;
    gint32 ret;
    gchar *flag = NULL;
    json_object *message_info_jso = NULL;
    gchar *resource_lower = NULL;
    RESOURCE_PROVIDER_S **res_provider = NULL;

    if (i_request_info == NULL || provider_param == NULL) {
        debug_log(DLOG_ERROR, "[%s]: input param error", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        ret = json_object_array_add(message_array_jso, message_info_jso);
        
        if (ret != 0) {
            json_object_put(message_info_jso);
        }
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    res_provider = &(i_request_info->res_provider);
    
    ret = get_resource_provider(i_request_info->uri_lower, res_provider);
    if (ret == VOS_OK && *res_provider != NULL) {
        // resource_path统一转换为小写后作为路径,避免数组中配置时有大写字母出现
        resource_lower = g_ascii_strdown((*res_provider)->resource_path, strlen((*res_provider)->resource_path));
        if (resource_lower == NULL) {
            debug_log(DLOG_ERROR, "%s: g_ascii_strdown fail, uri is %s", __FUNCTION__, resource_lower);
            goto ERR_EXIT;
        }

        iRet = snprintf_s(i_request_info->resource_path, MAX_PATH_LENGTH, MAX_PATH_LENGTH - 1, "%s%s/%s",
            CONFG_FILE_PREFIX, resource_lower, CONFG_FILE_SUFFIX);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
        g_free(resource_lower);
    } else {
        
        ret = get_resource_path_from_uri(i_request_info->uri_lower, i_request_info->resource_path, MAX_PATH_LENGTH);
        debug_log(DLOG_ERROR, "%s: get_resource_provider fail, no uri pattern is match", __FUNCTION__);
        if (ret != VOS_OK) {
            goto ERR_EXIT;
        }
    }

    flag = g_strrstr(i_request_info->uri_lower, "/");
    
    if (flag == NULL || strlen(flag) >= MAX_MEM_ID_LEN) {
        debug_log(DLOG_ERROR, "%s: member id is too long, uri:%s, flag:%s", __FUNCTION__, i_request_info->uri_lower,
            flag);
        goto ERR_EXIT;
    }
    

    (void)strncpy_s(i_request_info->member_id, MAX_MEM_ID_LEN, flag + 1, MAX_MEM_ID_LEN - 1);
    (void)strncpy_s(provider_param->member_id, MAX_MEM_ID_LEN, i_request_info->member_id, MAX_MEM_ID_LEN - 1);
    (void)strncpy_s(provider_param->uri, MAX_URI_LENGTH, i_request_info->relative_uri, MAX_URI_LENGTH - 1);
    (void)strncpy_s(provider_param->client, SESSION_IP_LEN, i_request_info->request_ip, SESSION_IP_LEN - 1);
    ret = get_skip_and_top(i_request_info, &message_info_jso, &provider_param->skip, &provider_param->top, NULL);
    if (ret != VOS_OK) {
        (void)json_object_array_add(message_array_jso, message_info_jso);
        return HTTP_BAD_REQUEST;
    }

    return VOS_OK;

ERR_EXIT:
    (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_info_jso, i_request_info->relative_uri);
    (void)json_object_array_add(message_array_jso, message_info_jso);
    return HTTP_NOT_FOUND;
}


LOCAL gint32 _get_class_name_from_schema(const char *rsc_path, char *class_buff, gint32 buff_len)
{
    json_object *red_obj_jso = NULL;
    json_object *type_jso = NULL;
    const char *flag = NULL;

    return_val_if_fail(rsc_path, VOS_ERR);
    return_val_if_fail(NULL != class_buff, VOS_ERR);

    
    json_object *resource_jso = json_object_from_file(rsc_path);
    return_val_do_info_if_fail(VOS_OK != resource_jso, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: load json file failed", __FUNCTION__));

    (void)json_object_object_get_ex(resource_jso, REDFISH_OBJECT, &red_obj_jso);
    return_val_do_info_if_fail(NULL != red_obj_jso, VOS_ERR, json_object_put(resource_jso);
        debug_log(DLOG_ERROR, "%s: get json obj:%s failed", __FUNCTION__, REDFISH_OBJECT));

    
    (void)json_object_object_get_ex(red_obj_jso, ODATA_TYPE_KEY, &type_jso);
    return_val_do_info_if_fail(NULL != type_jso, VOS_ERR, json_object_put(resource_jso);
        debug_log(DLOG_ERROR, "%s: get json obj:%s failed", __FUNCTION__, ODATA_TYPE_KEY));

    
    const char *type_str = json_object_get_string(type_jso);
    flag = strrchr(type_str, '#');
    return_val_do_info_if_fail(NULL != flag, VOS_ERR, json_object_put(resource_jso);
        debug_log(DLOG_ERROR, "%s: dal_json_object_get_str faild", __FUNCTION__));
    
    errno_t safe_fun_ret = strncpy_s(class_buff, buff_len, flag + 1, strlen(flag) - 1);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    
    json_object_put(resource_jso);
    debug_log(DLOG_DEBUG, "%s: redfish class name:%s", __FUNCTION__, class_buff);
    return VOS_OK;
}

LOCAL void _parse_get_retvalue(gint32 ret, int *status_code, const char *rsc_uri, json_object *message_array_jso)
{
    json_object *message_info_jso = NULL;

    switch (ret) {
        case RF_OK:
        case HTTP_OK:
            
        case HTTP_ACCEPTED:
        case HTTP_CREATED:
        case HTTP_BAD_REQUEST:
            
            *status_code = ret;
            break;

            
        case RF_RSC_NOT_FOUND:
            *status_code = HTTP_NOT_FOUND;
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_info_jso, rsc_uri);
            break;

        case RF_QUERY_NOT_SUPPORTED:
            *status_code = HTTP_NOT_IMPLEMENTED;
            (void)create_message_info(MSGID_QUERY_NOT_SUPPORTED_RSC, NULL, &message_info_jso);
            break;

        case RF_QUERY_TYPE_ERROR:
        case RF_QUERY_OUTOF_RANGE:
            *status_code = HTTP_BAD_REQUEST;
            break;

            
        case RF_INSUFFICIENT_PRIV:
        case HTTP_FORBIDDEN:
            *status_code = HTTP_FORBIDDEN;
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, &message_info_jso);
            break;

            
            
        case RF_RSC_IN_USE:
            *status_code = HTTP_BAD_REQUEST;
            break;

            
        default:
            
            *status_code = HTTP_INTERNAL_SERVER_ERROR;
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
            debug_log(DLOG_ERROR, "unknown error code:%d", ret);

            break;
    }

    do_val_if_expr(message_info_jso, (void)json_object_array_add(message_array_jso, message_info_jso));

    return;
}


LOCAL gint32 check_rsc_entry_accessiable(const REQUEST_INFO_S *i_request_info, PROVIDER_PARAS_S *i_param,
    PROPERTY_PROVIDER_S **provider, guint32 *count, json_object *o_message_array_jso)
{
    gint32 ret;
    json_object *message_info_jso = NULL;

    
    if (i_request_info->res_provider->resource_provider_fn != NULL) {
        ret = i_request_info->res_provider->resource_provider_fn(i_param, provider, count);
        // mark: 补充场景过滤
        // ....
        switch (ret) {
            case VOS_OK:
            case HTTP_OK:
                return VOS_OK;

            case HTTP_NOT_FOUND:
                (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_info_jso, i_request_info->relative_uri);
                json_object_array_add(o_message_array_jso, message_info_jso);
                return HTTP_NOT_FOUND;

            default:
                (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
                json_object_array_add(o_message_array_jso, message_info_jso);
                return HTTP_INTERNAL_SERVER_ERROR;
        }
    }

    return VOS_OK;
}


gint32 generate_get_response_body(const REQUEST_INFO_S *i_request_info, PROVIDER_PARAS_S *i_param,
    json_object **o_resource_jso, json_object *o_message_array_jso)
{
    PROPERTY_PROVIDER_S *provider = NULL;
    guint32 count = 0;
    json_object *message_info_jso = NULL;

    
    return_val_do_info_if_fail(((NULL != o_resource_jso) && (NULL != *o_resource_jso) && (NULL != i_param)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s %d: input param error", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(o_message_array_jso, message_info_jso));
    
    
    RESOURCE_PROVIDER_S *resource_provider = i_request_info->res_provider;
    if (resource_provider == NULL) {
        // 找不到Provider层入口，说明仅需返回配置文件即可，不需更新资源文件属性值
        return VOS_OK;
    }

    gint32 ret = check_rsc_entry_accessiable(i_request_info, i_param, &provider, &count, o_message_array_jso);
    if (ret != VOS_OK) {
        return ret;
    }

    if (resource_provider->do_get) {
        (void)fill_odata_head_info(i_request_info->uri_lower, *o_resource_jso);
        
        gint32 get_ret = resource_provider->do_get(i_param, *o_resource_jso, o_message_array_jso, NULL);

        _parse_get_retvalue(get_ret, &ret, i_request_info->relative_uri, o_message_array_jso);

        return ret;
    }

    return_val_do_info_if_fail(provider, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s %d: cann't get provider for %s", __FUNCTION__, __LINE__,
        i_request_info->relative_uri);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(o_message_array_jso, message_info_jso));

    
    ret = provider_get_prop_values(i_param, o_resource_jso, o_message_array_jso, provider, count);
    do_val_if_expr(VOS_OK != ret, return ret);
    

    return VOS_OK;
}


LOCAL gint32 generate_post_create_session_response_body(PROVIDER_PARAS_S *i_param, REQUEST_INFO_S *new_resource_info,
    gint32 uri_len, const gchar *member_id)
{
    int iRet;
    errno_t safe_fun_ret;
    gchar *id_hash = NULL;
    gchar session_id[SESSION_ID_LEN + 1] = {0};
    gint32 ret;

    id_hash = g_compute_checksum_for_string(G_CHECKSUM_SHA256, member_id, -1);
    return_val_do_info_if_expr(id_hash == NULL, VOS_ERR,
        debug_log(DLOG_ERROR, "%s:id_hash g_compute_checksum_for_string failed.", __FUNCTION__));
    safe_fun_ret = strncpy_s(session_id, SESSION_ID_LEN + 1, id_hash, SESSION_ID_LEN);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    g_free(id_hash);
    iRet = snprintf_s(new_resource_info->relative_uri, uri_len, uri_len - 1, "%s/%s", i_param->uri, session_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    (void)strncpy_s(new_resource_info->headers[0].header_title, MAX_HEADER_TITLE_LEN, RESP_HEADER_X_AUTH_TOKEN,
        strlen(RESP_HEADER_X_AUTH_TOKEN));
    (void)strncpy_s(new_resource_info->headers[0].header_content, MAX_HEADER_CONTENT_LEN, member_id,
        MAX_HEADER_CONTENT_LEN - 1);
    new_resource_info->headers_index.x_auth_token = &new_resource_info->headers[0];

    // 获取新生成会话信息
    ret = verify_xauthtoken_get_provider_param(new_resource_info, i_param);
    return_val_do_info_if_fail(VOS_OK == ret, ret,
        debug_log(DLOG_ERROR, "%s %d: verify_xauthtoken_get_session fail", __FUNCTION__, __LINE__));

    return VOS_OK;
}


LOCAL gint32 generate_post_create_response_body(PROVIDER_PARAS_S *i_param, const gchar *member_id,
    json_object **out_redfish_jso, json_object *message_array_jso)
{
    int iRet = -1;
    REQUEST_INFO_S *new_resource_info = NULL;
    gchar *uri_lower = NULL;
    json_object *resource_jso = NULL;
    json_object *redfish_jso = NULL;
    gint32 ret;
    json_bool ret_json;

    new_resource_info = (REQUEST_INFO_S *)g_malloc0(sizeof(REQUEST_INFO_S));
    return_val_do_info_if_fail((NULL != new_resource_info), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s %d: malloc REQUEST_INFO_S memory fail", __FUNCTION__, __LINE__));

    (void)memset_s(new_resource_info, sizeof(REQUEST_INFO_S), 0, sizeof(REQUEST_INFO_S));

    debug_log(DLOG_DEBUG, "%s : i_param->uri: %s\n", __FUNCTION__, i_param->uri);

    // if 特殊处理
    if (0 == g_ascii_strcasecmp(SESSIONS_PREFIX, i_param->uri)) {
        ret = generate_post_create_session_response_body(i_param, new_resource_info, MAX_URI_LENGTH, member_id);
        
        goto_label_do_info_if_fail(VOS_OK == ret, EXIT, (ret = HTTP_INTERNAL_SERVER_ERROR));
        
    }
    // 通用流程
    else {
        iRet = snprintf_s(new_resource_info->relative_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, "%s/%s", i_param->uri,
            member_id);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    }

    // 填充新生成资源request_info部分必需值
    // 转换URI信息为小写保存
    uri_lower = g_ascii_strdown(new_resource_info->relative_uri, strlen(new_resource_info->relative_uri));
    goto_label_do_info_if_fail(NULL != uri_lower, EXIT, ret = HTTP_INTERNAL_SERVER_ERROR;
        debug_log(DLOG_ERROR, "%s %d: change uri to lower fail", __FUNCTION__, __LINE__));
    (void)strncpy_s(new_resource_info->uri_lower, MAX_URI_LENGTH, uri_lower, MAX_URI_LENGTH - 1);
    g_free(uri_lower);

    // 根据URI获取资源配置文件路径
    ret = get_res_info_from_uri(new_resource_info, i_param, message_array_jso);
    goto_label_do_info_if_fail(VOS_OK == ret, EXIT,
        debug_log(DLOG_ERROR, "%s %d: get_res_info_from_uri fail", __FUNCTION__, __LINE__));

    // 载入资源配置文件
    resource_jso = json_object_from_file(new_resource_info->resource_path);
    goto_label_do_info_if_fail((NULL != resource_jso), EXIT, ret = HTTP_NOT_FOUND;
        debug_log(DLOG_ERROR, "%s get resource from file fail", __FUNCTION__));

    // 获取配置文件中资源相关属性
    ret_json = json_object_object_get_ex(resource_jso, REDFISH_OBJECT, &redfish_jso);
    goto_label_do_info_if_fail(ret_json, EXIT, ret = HTTP_NOT_FOUND; json_object_put(resource_jso);
        debug_log(DLOG_ERROR, "%s get REDFISH_OBJECT from file fail", __FUNCTION__));

    // 更新资源对象属性值
    ret = generate_get_response_body(new_resource_info, i_param, &redfish_jso, message_array_jso);
    goto_label_do_info_if_fail(VOS_OK == ret, EXIT, json_object_put(resource_jso);
        debug_log(DLOG_ERROR, "%s generate_get_response_body fail ret:%d", __FUNCTION__, ret));

    *out_redfish_jso = json_object_get(redfish_jso);

    json_object_put(resource_jso);

EXIT:
    clear_sensitive_info(new_resource_info, sizeof(REQUEST_INFO_S));
    g_free(new_resource_info);

    return ret;
}


LOCAL void replace_copyright(const gchar *uri_low, json_object *resource_jso)
{
    const gchar *copyright_str = NULL;
    OBJ_HANDLE obj_handle;
    gchar copyright_buf[PROP_VAL_MAX_LENGTH] = {0};
    gchar temp_buf[PROP_VAL_MAX_LENGTH] = {0};
    gchar default_copyright[PROP_VAL_MAX_LENGTH] = {0};
    // Copyright 的固定前缀
    guchar copyright_prefix[] = {0x43, 0x6F, 0x70, 0x79, 0x72, 0x69, 0x67, 0x68, 0x74, 0x20, 0xC2, 0xA9, 0x20, 0x00};

    if (g_str_has_prefix(uri_low, SCHEMASTORE_PREFIX) != TRUE) {
        return;
    }

    gint32 ret_val = get_element_str(resource_jso, RFPROP_COPYRIGHT, &copyright_str);
    if (ret_val != TRUE) {
        return;
    }

    gchar *copyright_low = g_ascii_strdown(copyright_str, strlen(copyright_str));
    if (copyright_low == NULL) {
        return;
    }

    if (strstr(copyright_low, "huawei") == NULL) {
        g_free(copyright_low);
        return;
    }
    g_free(copyright_low);

    ret_val = dal_get_object_handle_nth(BMC_CONTACT_NAME, 0, &obj_handle);
    if (ret_val != DFL_OK) {
        return;
    }

    ret_val = dal_get_property_value_string(obj_handle, BMC_COPYRIGHT, copyright_buf, sizeof(copyright_buf));
    ret_val += dal_get_property_value_string(obj_handle, PROPERTY_CONTACT_D_COPYRIGHT, default_copyright,
        sizeof(default_copyright));
    if (ret_val != DFL_OK || strcmp(copyright_buf, default_copyright) == 0) {
        return;
    }

    // 非空字符串，拼接 固定前缀
    if (strlen(copyright_buf) != 0) {
        ret_val = snprintf_truncated_s(temp_buf, sizeof(temp_buf), "%s%s", copyright_prefix, copyright_buf);
        return_if_expr(ret_val <= 0);
    }
    // copyright_buf为空字符串时，temp_buf也为空
    json_object *copyright_jso = json_object_new_string(temp_buf);
    ret_val = json_object_object_add(resource_jso, RFPROP_COPYRIGHT, copyright_jso);
    if (ret_val != 0 && copyright_jso) {
        json_object_put(copyright_jso);
    }
}


LOCAL gint32 generate_specific_response_body(const REQUEST_INFO_S *i_request_info, json_object **o_resource_jso,
    json_object *o_message_array_jso)
{
    FILE *file = NULL;
    gchar *file_buff = NULL;
    guint32 read_len;
    json_object *message_info_jso = NULL;
    gchar *resolved_path = NULL;

    
    if (NULL == i_request_info) {
        debug_log(DLOG_ERROR, "%s: i_request_info parameter is null", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    

    resolved_path = realpath(i_request_info->resource_path, NULL);
    if (NULL == resolved_path) {
        debug_log(DLOG_ERROR, "%s: realpath get resolved path fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (g_str_has_prefix(i_request_info->uri_lower, METADATA_URI_PREFIX)) {
        // metadata文件为xml格式,需要特殊处理
        file_buff = (gchar *)g_malloc0(MAX_RESPONSE_BODY_LEN + 1);
        file = fopen(resolved_path, "rt");
        g_free(resolved_path);
        return_val_do_info_if_fail((NULL != file), HTTP_INTERNAL_SERVER_ERROR, g_free(file_buff); file_buff = NULL;
            debug_log(DLOG_ERROR, "%s: fopen fail", __FUNCTION__);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
            json_object_array_add(o_message_array_jso, message_info_jso));
        read_len = fread(file_buff, sizeof(guint8), MAX_RESPONSE_BODY_LEN, file);
        (void)fclose(file);
        file = NULL;
        return_val_do_info_if_fail((0 != read_len), HTTP_INTERNAL_SERVER_ERROR, g_free(file_buff); file_buff = NULL;
            debug_log(DLOG_ERROR, "%s: fread fail,ret is %d", __FUNCTION__, read_len);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
            json_object_array_add(o_message_array_jso, message_info_jso));
        debug_log(DLOG_DEBUG, "%s file_buff: %s", __FUNCTION__, file_buff);
        *o_resource_jso = json_object_new_string((const gchar *)file_buff);
        g_free(file_buff);
        file_buff = NULL;
    } else {
        // 其他特殊文件.直接从文件中读取即可
        *o_resource_jso = json_object_from_file(resolved_path);
        g_free(resolved_path);
        return_val_do_info_if_fail((NULL != *o_resource_jso), HTTP_NOT_FOUND,
            debug_log(DLOG_ERROR, "%s: json_object_from_file fail", __FUNCTION__);
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_info_jso, i_request_info->relative_uri);
            json_object_array_add(o_message_array_jso, message_info_jso));
        replace_copyright(i_request_info->uri_lower, *o_resource_jso);
    }

    return VOS_OK;
}


gint32 generate_common_response_headers(json_object **header_array_jso, const gchar *uri,
    json_object *message_array_jso)
{
    gint32 ret = VOS_OK;
    json_object *header_jso = NULL;
    guint32 array_index;
    json_object *message_info_jso = NULL;
    guint32 len = sizeof(common_headers) / sizeof(common_headers[0]);

    return_val_do_info_if_fail(((NULL != uri) && (NULL != header_array_jso) && (NULL != *header_array_jso)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s %d: input param error", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(message_array_jso, message_info_jso););

    for (array_index = 0; array_index < len; array_index++) {
        continue_if_expr_true(0 == common_headers[array_index].header_title[0]);

        // metadata的content-type为xml,需要特殊处理

        
        header_jso = json_object_new_object();
        
        return_val_do_info_if_fail(header_jso, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s %d: json_object_new_object fail", __FUNCTION__, __LINE__));
        
        

        if (0 == g_strcmp0(common_headers[array_index].header_title, RESP_HEADER_CONTENT_TYPE) &&
            g_str_has_prefix(uri, METADATA_URI_PREFIX)) {
            json_object_object_add(header_jso, common_headers[array_index].header_title,
                json_object_new_string("application/xml;charset=utf-8"));
        } else {
            json_object_object_add(header_jso, common_headers[array_index].header_title,
                json_object_new_string(common_headers[array_index].header_content));
        }

        
        ret = json_object_array_add(*header_array_jso, header_jso);
        
        if (VOS_OK != ret) {
            
            json_object_put(header_jso);
            
            debug_log(DLOG_ERROR, "%s %d: json_object_array_add fail", __FUNCTION__, __LINE__);
        }
    }

    return VOS_OK;
}


LOCAL gint32 _add_custom_response_headers(json_object *res_header, json_object *custom_header)
{
    json_object *object_val = NULL;

    return_val_if_expr(NULL == res_header && NULL == custom_header, VOS_ERR);

#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#endif
    json_object_object_foreach0(custom_header, name, val)
    {
        object_val = json_object_new_object();
        continue_if_expr(NULL == object_val);

        json_object_object_add(object_val, name, json_object_get(val));

        (void)json_object_array_add(res_header, object_val);
    }
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif

    return VOS_OK;
}


LOCAL gint32 generate_rsc_etag_header(const gchar *rsc_val, json_object **o_header_jso)
{
    gchar etag[ETAG_LEN] = {0};
    gchar *check_sum = NULL;
    gchar etag_tmp[ETAG_LEN - 4] = {0};
    errno_t securec_rv;

    if (rsc_val == NULL || o_header_jso == NULL) {
        debug_log(DLOG_ERROR, "%s:NULL pointer", __FUNCTION__);
        return VOS_ERR;
    }

    // 以资源对象字符串计算校验和sha256加密值
    check_sum = g_compute_checksum_for_string(G_CHECKSUM_SHA256, rsc_val, strlen(rsc_val));
    if (check_sum == NULL) {
        debug_log(DLOG_ERROR, "%s: call g_compute_checksum_for_string fail", __FUNCTION__);
        return VOS_ERR;
    }

    // 取check_sum前8个字节
    securec_rv = strncpy_s(etag_tmp, ETAG_LEN - 4, check_sum, ETAG_LEN - 5);
    if (securec_rv != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, securec_rv);
    }
    g_free(check_sum);

    // 生成etag字符串
    securec_rv = snprintf_s(etag, ETAG_LEN, ETAG_LEN - 1, "W/\"%s\"", etag_tmp);
    if (securec_rv <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, securec_rv);
    }

    *o_header_jso = json_object_new_object();
    if (*o_header_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object error", __FUNCTION__);
        return VOS_ERR;
    }

    json_object_object_add(*o_header_jso, RESP_HEADER_ETAG, json_object_new_string((const gchar *)etag));
    return VOS_OK;
}


gint32 generate_etag_response_header(const gchar *uri_lower, json_object *i_redfish_jso,
    RESOURCE_PROVIDER_S *resource_provider, json_object **o_header_jso)
{
    json_object *temp_jso = NULL;
    PROPERTY_PROVIDER_S *provider = NULL;
    guint32 array_index;
    guint32 count;
    PROVIDER_PARAS_S i_paras;

    return_val_do_info_if_fail((NULL != uri_lower) && (NULL != i_redfish_jso) && (NULL != o_header_jso), VOS_ERR,
        debug_log(DLOG_ERROR, "input param error"));

    const gchar *redfish_resource = json_object_get_string(i_redfish_jso);

    
    // 如果非特定URI，则遍历寻找provider数组，从中判断资源属性是否需要计入Etag计算之中
    if (is_static_rsc_uri(uri_lower) != VOS_OK) {
        temp_jso = json_tokener_parse(redfish_resource);
        return_val_do_info_if_fail(NULL != temp_jso, VOS_ERR,
            debug_log(DLOG_ERROR, "%s %d: json_tokener_parse fail", __FUNCTION__, __LINE__));

        gchar *flag = g_strrstr(uri_lower, "/");
        
        return_val_do_info_if_fail(NULL != flag && (strlen(flag) < MAX_MEM_ID_LEN), VOS_ERR, json_object_put(temp_jso);
            debug_log(DLOG_ERROR, "%s %d: member id is too long", __FUNCTION__, __LINE__));
        

        (void)memset_s(&i_paras, sizeof(i_paras), 0, sizeof(i_paras));
        (void)strncpy_s(i_paras.member_id, MAX_MEM_ID_LEN, flag + 1, MAX_MEM_ID_LEN - 1);

        
        (void)strncpy_s(i_paras.uri, MAX_URI_LENGTH, uri_lower, MAX_URI_LENGTH - 1);
        

        
        
        if (resource_provider == NULL) {
            (void)get_resource_provider(uri_lower, &resource_provider);
        }
        if (resource_provider != NULL && resource_provider->resource_provider_fn &&
            resource_provider->resource_provider_fn(&i_paras, &provider, &count) == VOS_OK && provider != NULL) {
            
            do_val_if_expr(NULL != resource_provider->etag_del_prop, (void)resource_provider->etag_del_prop(temp_jso));
            

            // 遍历资源属性，如果不支持Etag计算则从对象中剔除
            for (array_index = 0; array_index < count; array_index++) {
                do_val_if_expr(ETAG_FLAG_ENABLE != provider[array_index].etag_flag,
                    json_object_object_del(temp_jso, provider[array_index].property_name));
            }
        }

        
        
        
        (void)rf_provider_paras_free(&i_paras);
        
        redfish_resource = dal_json_object_get_str(temp_jso);
    }

    gint32 ret = generate_rsc_etag_header(redfish_resource, o_header_jso);
    if (temp_jso != NULL) {
        json_object_put(temp_jso);
    }
    return ret;
}


LOCAL gint32 generate_link_header_by_odata_type(const gchar *odata_type, json_object **header_jso)
{
    errno_t securec_rv;
    const gchar *flag = NULL;
    gchar schema_name[MAX_MEM_ID_LEN] = {0};
    gchar link_head[MAX_URI_LENGTH] = {0};

    if (odata_type == NULL || header_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__);
        return VOS_ERR;
    }

    flag = strstr(odata_type, ".");
    if (flag == NULL || flag <= odata_type) {
        debug_log(DLOG_ERROR, "%s:parse odata.type failed", __FUNCTION__);
        return VOS_ERR;
    }

    securec_rv = strncpy_s(schema_name, MAX_MEM_ID_LEN, odata_type + 1, flag - odata_type - 1);
    if (securec_rv != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, securec_rv);
        return VOS_ERR;
    }

    // 生成link头，即schema文件路径
    securec_rv = snprintf_s(link_head, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, "<%s/%s.json>;rel=describedby", LINK_PREFIX,
        schema_name);
    if (securec_rv <= 0) {
        debug_log(DLOG_ERROR, "%s:snprintf_s fail, ret= %d", __FUNCTION__, securec_rv);
        return VOS_ERR;
    }

    *header_jso = json_object_new_object();
    if (*header_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__);
        return VOS_ERR;
    }

    json_object_object_add(*header_jso, RESP_HEADER_LINK, json_object_new_string((const gchar *)link_head));
    return VOS_OK;
}


LOCAL gint32 generate_link_response_header(json_object *i_redfish_jso, json_object **o_header_jso)
{
    json_bool ret_json;
    json_object *type_jso = NULL;
    const gchar *type_str = NULL;

    return_val_do_info_if_fail((NULL != i_redfish_jso) && (NULL != o_header_jso), VOS_ERR,
        debug_log(DLOG_ERROR, "%s %d: input param error", __FUNCTION__, __LINE__));

    ret_json = json_object_object_get_ex(i_redfish_jso, ODATA_TYPE_KEY, &type_jso);
    return_val_do_info_if_fail(ret_json, VOS_ERR,
        debug_log(DLOG_ERROR, "%s %d: json_object_object_get_ex %s error", __FUNCTION__, __LINE__, ODATA_TYPE_KEY));

    // 从odata.type中提取schema文件名
    type_str = dal_json_object_get_str(type_jso);
    return_val_do_info_if_fail(type_str, VOS_ERR,
        debug_log(DLOG_ERROR, "%s %d: dal_json_object_get_str odata type is null", __FUNCTION__, __LINE__));

    if (generate_link_header_by_odata_type(type_str, o_header_jso) == VOS_OK) {
        return VOS_OK;
    } else {
        debug_log(DLOG_ERROR, "%s: generate link response header failed", __FUNCTION__);
        return VOS_ERR;
    }
}


LOCAL gint32 generate_allow_response_header(json_object *i_allow_jso, json_object **o_header_jso)
{
    errno_t safe_fun_ret = EOK;
    gint32 allow;
    gchar allow_info[MAX_METHOD_NAME_LEN * 4] = {0};
    
    guint32 max_method_len;

    return_val_do_info_if_fail((NULL != i_allow_jso) && (NULL != o_header_jso), VOS_ERR,
        debug_log(DLOG_ERROR, "%s %d: input param error", __FUNCTION__, __LINE__));

    allow = json_object_get_int(i_allow_jso);
    max_method_len = sizeof(allow_info);
    if ((guint32)allow & ALLOW_GET) {
        safe_fun_ret = strncat_s(allow_info, max_method_len, HTTP_GET, max_method_len - 1);
        return_val_do_info_if_expr(safe_fun_ret != EOK, VOS_ERR,
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
        safe_fun_ret = strncat_s(allow_info, max_method_len, ",", max_method_len - 1);
        return_val_do_info_if_expr(safe_fun_ret != EOK, VOS_ERR,
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    }

    if ((guint32)allow & ALLOW_PATCH) {
        safe_fun_ret = strncat_s(allow_info, max_method_len, HTTP_PATCH, max_method_len - 1);
        return_val_do_info_if_expr(safe_fun_ret != EOK, VOS_ERR,
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
        safe_fun_ret = strncat_s(allow_info, max_method_len, ",", max_method_len - 1);
        return_val_do_info_if_expr(safe_fun_ret != EOK, VOS_ERR,
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    }

    if ((guint32)allow & ALLOW_POST) {
        safe_fun_ret = strncat_s(allow_info, max_method_len, HTTP_POST, max_method_len - 1);
        return_val_do_info_if_expr(safe_fun_ret != EOK, VOS_ERR,
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
        safe_fun_ret = strncat_s(allow_info, max_method_len, ",", max_method_len - 1);
        return_val_do_info_if_expr(safe_fun_ret != EOK, VOS_ERR,
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    }

    if ((guint32)allow & ALLOW_DELETE) {
        safe_fun_ret = strncat_s(allow_info, max_method_len, HTTP_DELETE, max_method_len - 1);
        return_val_do_info_if_expr(safe_fun_ret != EOK, VOS_ERR,
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
        safe_fun_ret = strncat_s(allow_info, max_method_len, ",", max_method_len - 1);
        return_val_do_info_if_expr(safe_fun_ret != EOK, VOS_ERR,
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    }

    
    if (allow_info[0] != 0 && strlen(allow_info) < max_method_len) {
        allow_info[strlen(allow_info) - 1] = '\0'; // 清除末尾的","
        *o_header_jso = json_object_new_object();
        
        return_val_do_info_if_fail(*o_header_jso, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s %d: json_object_new_object fail", __FUNCTION__, __LINE__));
        
        json_object_object_add(*o_header_jso, RESP_HEADER_ALLOW, json_object_new_string((const gchar *)allow_info));
        return VOS_OK;
    } else {
        debug_log(DLOG_ERROR, "generate_allow_response_header configure info is error");
        return VOS_ERR;
    }
}


LOCAL void add_extern_response_head(json_object *head_array, const gchar *uri_lower)
{
    gint32 ret;
    gint32 task_id = 0;
    TASK_RSC_S *task_rsc = NULL;
    json_object *response_header = NULL;
    json_object *object_val = NULL;

    return_do_info_if_expr(NULL == head_array || json_type_array != json_object_get_type(head_array) ||
        NULL == uri_lower,
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__));

    ret = get_task_monitor_id_value(uri_lower, &task_id);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_DEBUG, "%s: url is not task", __FUNCTION__));

    task_rsc = get_task_rsc_structure(task_id);
    return_do_info_if_expr(NULL == task_rsc, debug_log(DLOG_DEBUG, "%s: get_task_rsc_structure failed", __FUNCTION__));

    response_header = task_rsc->monitor_fn_data->response_header;
    return_do_info_if_expr(NULL == response_header, debug_log(DLOG_DEBUG, "%s: response_header  null", __FUNCTION__));

#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#endif
    json_object_object_foreach0(response_header, name, val)
    {
        object_val = json_object_new_object();
        continue_if_expr(NULL == object_val);

        json_object_object_add(object_val, name, json_object_get(val));

        (void)json_object_array_add(head_array, object_val);
    }
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif
}


LOCAL gint32 generate_get_response_headers(guint8 from_webui, json_object *i_resource_jso, REQUEST_INFO_S *request_info,
    json_object **o_header_array_jso, json_object *message_array_jso)
{
    gint32 ret;
    json_object *redfish_jso = NULL;
    json_object *allow_jso = NULL;
    json_object *header_jso = NULL;
    json_bool ret_json;
    json_object *message_info_jso = NULL;

    if (i_resource_jso == NULL || o_header_array_jso == NULL || *o_header_array_jso == NULL || request_info == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        (void)json_object_array_add(message_array_jso, message_info_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = generate_common_response_headers(o_header_array_jso, request_info->uri_lower, message_array_jso);
    return_val_do_info_if_fail(VOS_OK == ret, ret,
        debug_log(DLOG_ERROR, "%s %d :generate_common_response_headers error", __FUNCTION__, __LINE__));

    // 获取redfish资源对象
    ret_json = json_object_object_get_ex(i_resource_jso, REDFISH_OBJECT, &redfish_jso);
    return_val_do_info_if_fail(ret_json, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s %d :json_object_object_get_ex %s error", __FUNCTION__, __LINE__, REDFISH_OBJECT);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        (void)json_object_array_add(message_array_jso, message_info_jso));

    // 非WEB发起的请求才生成Etag头信息
    if (!from_webui) {
        ret = generate_etag_response_header(request_info->uri_lower, redfish_jso, request_info->res_provider,
            &header_jso);
        if (ret == VOS_OK) {
            (void)json_object_array_add(*o_header_array_jso, header_jso);
        } else {
            debug_log(DLOG_ERROR, "%s %d :generate etag response header error, ret is %d", __FUNCTION__, __LINE__, ret);
        }
        header_jso = NULL;
    }

    header_jso = monitor_create_location();
    if (header_jso != NULL) {
        (void)json_object_array_add(*o_header_array_jso, header_jso);
        header_jso = NULL;
    }

    add_extern_response_head(*o_header_array_jso, request_info->uri_lower);

    // 生成Link头信息
    ret = generate_link_response_header(redfish_jso, &header_jso);
    if (ret == VOS_OK) {
        (void)json_object_array_add(*o_header_array_jso, header_jso);
    } else {
        debug_log(DLOG_ERROR, "%s %d :generate_link_response_header error, ret is %d", __FUNCTION__, __LINE__, ret);
    }

    // 从资源配置文件对象获取Allow头信息
    ret_json = json_object_object_get_ex(i_resource_jso, ALLOWED_OPERATION, &allow_jso);
    
    if (!ret_json) {
        debug_log(DLOG_ERROR, "json_object_object_get_ex ALLOWED_OPERATION error");
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        (void)json_object_array_add(message_array_jso, message_info_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    

    // 生成Allow头信息
    header_jso = NULL;
    ret = generate_allow_response_header(allow_jso, &header_jso);
    if (ret == VOS_OK) {
        (void)json_object_array_add(*o_header_array_jso, header_jso);
    } else {
        debug_log(DLOG_ERROR, "%s :generate_allow_response_header error, ret is %d", __FUNCTION__, ret);
    }
    return ret;
}


LOCAL gint32 generate_specific_response_headers(guint8 from_webui, json_object *i_resource_jso, const gchar *uri_lower,
    json_object **o_header_array_jso, json_object *message_array_jso)
{
    gint32 ret;
    json_object *allow_jso = NULL;
    json_object *header_jso = NULL;
    json_object *message_info_jso = NULL;

    
    if ((NULL == i_resource_jso) || (NULL == o_header_array_jso) || (NULL == *o_header_array_jso)) {
        debug_log(DLOG_ERROR, "%s %d :input param error", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        (void)json_object_array_add(message_array_jso, message_info_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    

    ret = generate_common_response_headers(o_header_array_jso, uri_lower, message_array_jso);
    return_val_do_info_if_fail(VOS_OK == ret, ret,
        debug_log(DLOG_ERROR, "%s %d :generate_common_response_headers error", __FUNCTION__, __LINE__));

    // WEB请求不生成Etag头信息
    if (!from_webui) {
        
        ret = generate_etag_response_header(uri_lower, i_resource_jso, NULL, &header_jso);
        if (ret == VOS_OK) {
            (void)json_object_array_add(*o_header_array_jso, header_jso);
        } else {
            debug_log(DLOG_ERROR, "%s:generate_etag_response_header error, ret is %d", __FUNCTION__, ret);
        }

        header_jso = NULL;
    }

    // 生成Link头信息
    ret = generate_link_response_header(i_resource_jso, &header_jso);
    if (ret == VOS_OK) {
        
        (void)json_object_array_add(*o_header_array_jso, header_jso);
    } else {
        debug_log(DLOG_ERROR, "%s:%s generate_link_response_header error, ret is %d", __FUNCTION__, uri_lower, ret);
    }

    header_jso = NULL;

    // 只允许GET操作
    allow_jso = json_object_new_int(1);

    // 生成Allow头信息
    ret = generate_allow_response_header(allow_jso, &header_jso);
    json_object_put(allow_jso);

    if (ret == VOS_OK) {
        (void)json_object_array_add(*o_header_array_jso, header_jso);
    } else {
        debug_log(DLOG_ERROR, "%s:%sgenerate_allow_response_header error", __FUNCTION__, uri_lower);
    }

    
    return VOS_OK;
    
}


LOCAL gint32 generate_patch_response_headers(guint8 from_webui, json_object *redfish_jso, REQUEST_INFO_S *request_info,
    json_object **o_header_array_jso, json_object *message_array_jso)
{
    gint32 ret;
    json_object *header_jso = NULL;
    json_object *message_info_jso = NULL;
    
    gint32 json_object_ret = VOS_OK; // 新增加，用于判断json_object_array_add的返回值
    
    
    if (redfish_jso == NULL || o_header_array_jso == NULL || *o_header_array_jso == NULL || request_info == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        ret = json_object_array_add(message_array_jso, message_info_jso);
        if (ret != VOS_OK) {
            json_object_put(message_info_jso);
        }
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = generate_common_response_headers(o_header_array_jso, request_info->uri_lower, message_array_jso);
    return_val_do_info_if_fail(VOS_OK == ret, ret,
        debug_log(DLOG_ERROR, "%s %d :generate_common_response_headers error", __FUNCTION__, __LINE__));
    
    // 生成Etag头信息
    if (!from_webui) {
        ret = generate_etag_response_header(request_info->uri_lower, redfish_jso, request_info->res_provider,
            &header_jso);
        if (VOS_OK == ret) {
            
            json_object_ret = json_object_array_add(*o_header_array_jso, header_jso);
            // 修改意见: header_jso需要释放
            if (0 != json_object_ret) {
                json_object_put(header_jso); // 释放header_jso 分配的空间
                // header_jso = NULL;           // 将指针置为空;
                ret = VOS_ERR; // 返回VOS_ERR;
                debug_log(DLOG_ERROR, "%s %d : json_object_array_add error, ret is %d", __FUNCTION__, __LINE__, ret);
            }

            
        } else {
            debug_log(DLOG_ERROR, "%s %d :generate_etag_response_header error, ret is %d", __FUNCTION__, __LINE__, ret);
        }
    }

    return ret;
}


gint32 generate_response_status(gint32 response_code, json_object **status_jso, json_object *message_array_jso)
{
    int iRet;
    gint32 ret;
    gint32 ret_add;
    gchar status_str[MAX_STATUS_STRING_LEN] = {0};
    gchar *title_str = NULL;
    GHashTable *resp_title_hash = NULL;
    json_object *message_info_jso = NULL;

    
    if (NULL == status_jso) {
        debug_log(DLOG_ERROR, "status_jso is null");
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);

        ret_add = json_object_array_add(message_array_jso, message_info_jso);
        if (VOS_OK != ret_add) {
            json_object_put(message_info_jso);
            debug_log(DLOG_ERROR, "%s %d :json_object_array_add error, ret_add is %d", __FUNCTION__, __LINE__, ret_add);
        }

        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = get_resp_title_hash(&resp_title_hash);
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "get_resp_title_hash fail");
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);

        ret_add = json_object_array_add(message_array_jso, message_info_jso);
        if (VOS_OK != ret_add) {
            json_object_put(message_info_jso);
            debug_log(DLOG_ERROR, "%s %d :json_object_array_add error, ret_add is %d", __FUNCTION__, __LINE__, ret_add);
        }

        return HTTP_INTERNAL_SERVER_ERROR;
    }

    title_str = (gchar *)g_hash_table_lookup(resp_title_hash, GINT_TO_POINTER(response_code));
    if (NULL == title_str) {
        debug_log(DLOG_ERROR, "g_hash_table_lookup error");
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);

        ret_add = json_object_array_add(message_array_jso, message_info_jso);
        if (VOS_OK != ret_add) {
            json_object_put(message_info_jso);
            debug_log(DLOG_ERROR, "%s %d :json_object_array_add error, ret_add is %d", __FUNCTION__, __LINE__, ret_add);
        }

        return HTTP_INTERNAL_SERVER_ERROR;
    }

    

    iRet = snprintf_s(status_str, MAX_STATUS_STRING_LEN, MAX_STATUS_STRING_LEN - 1, "%d %s", response_code, title_str);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    *status_jso = json_object_new_string((const gchar *)status_str);
    return VOS_OK;
}


LOCAL gint32 generate_error_response(gint32 response_code, const REQUEST_INFO_S *request_info,
    json_object *message_array_jso, HTTP_RESPONSE_DATA_S *resp)
{
    gint32 ret;
    json_object *header_array_jso = NULL;
    json_object *status_jso = NULL;
    json_object *err_message_jso = NULL;
    json_object *resource_jso = NULL;
    json_object *allow_jso = NULL;
    json_object *header_jso = NULL;
    
    json_bool json_getex;
    
    return_val_do_info_if_fail(NULL != request_info->uri_lower, VOS_ERR,
        debug_log(DLOG_ERROR, "%s : input uri param error", __FUNCTION__));

    if (resp == NULL) {
        debug_log(DLOG_ERROR, "%s: resp is NULL", __FUNCTION__);
        return VOS_ERR;
    }

    header_array_jso = json_object_new_array();
    // 处理 header_array_jso == NULL
    if (header_array_jso == NULL) {
        debug_log(DLOG_ERROR, "%s :json_object_new_array fail", __FUNCTION__);
        return VOS_ERR;
    }

    ret = generate_common_response_headers(&header_array_jso, request_info->uri_lower, message_array_jso);
    
    // 如果返回码为405,则需要把Allow头返回
    if (response_code == HTTP_METHOD_NOT_ALLOWED) {
        if (is_static_rsc_uri(request_info->uri_lower) == VOS_OK) {
            allow_jso = json_object_new_int(1);
            (void)generate_allow_response_header(allow_jso, &header_jso);
            json_object_array_add(header_array_jso, header_jso);
            json_object_put(allow_jso);
        } else {
            resource_jso = json_object_from_file(request_info->resource_path);
            json_getex = json_object_object_get_ex(resource_jso, ALLOWED_OPERATION, &allow_jso);
            
            if (json_getex == FALSE) {
                debug_log(DLOG_ERROR, "%s:get %s from template file failed", __FUNCTION__, ALLOWED_OPERATION);
                json_object_put(resource_jso);
                json_object_put(header_array_jso);
                return VOS_ERR;
            }
            
            (void)generate_allow_response_header(allow_jso, &header_jso);
            json_object_array_add(header_array_jso, header_jso);
            json_object_put(resource_jso);
        }
    }
    

    if (ret == VOS_OK) {
        resp->resp_head = g_strdup(json_object_to_json_string_ext(header_array_jso, JSON_C_TO_STRING_NOSLASHESCAPE));
    } else {
        debug_log(DLOG_ERROR, "%s: generate common response header failed", __FUNCTION__);
    }
    json_object_put(header_array_jso);

    ret = generate_response_status(response_code, &status_jso, message_array_jso);
    if (ret == VOS_OK) {
        resp->resp_status = g_strdup(dal_json_object_get_str(status_jso));
        json_object_put(status_jso);
    } else {
        debug_log(DLOG_ERROR, "%s:generate response status failed", __FUNCTION__);
    }

    if ((message_array_jso != NULL) && (json_object_array_length(message_array_jso) != 0)) {
        ret = generate_error_message_info(message_array_jso, &err_message_jso);
        if (ret == VOS_OK) {
            resp->resp_body = g_strdup(json_object_to_json_string_ext(err_message_jso, JSON_C_TO_STRING_NOSLASHESCAPE));
            json_object_put(err_message_jso);
        } else {
            debug_log(DLOG_ERROR, "%s:generate error message info failed", __FUNCTION__);
        }
    }

    return ret;
}



LOCAL gint32 provider_set_prop_values(PROVIDER_PARAS_S *i_paras, json_object_iter *prop_iter,
    json_object *message_array_jso, const PROPERTY_PROVIDER_S *provider, guint32 count)
{
    gint32 ret = HTTP_BAD_REQUEST;
    guint32 array_index;
    gint32 array_len;
    gint32 msg_index;
    property_handler_t pfn_set;
    json_object *message_info_jso = NULL;

    pfn_set = find_property_provider(provider, count, prop_iter->key, &array_index);
    if (!pfn_set) {
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, prop_iter->key, &message_info_jso, prop_iter->key);

        ret = HTTP_NOT_IMPLEMENTED;

        goto quit;
    }

    // 判断系统锁定模式下是否允许set操作
    ret = redfish_system_lockdown_check(provider[array_index].syslock_allow);
    goto_label_do_info_if_fail(VOS_OK == ret, quit, ret = HTTP_BAD_REQUEST; debug_log(DLOG_INFO,
        "%s %d:action fail, system check failed: %s", __FUNCTION__, __LINE__, provider[array_index].property_name);
        (void)create_message_info(MSGID_SYSLOCKDOWN_FORBID, prop_iter->key, &message_info_jso));

    // 填充入参, 不填充 权限满足标识 变量,方法统一采用uip层控制权限
    i_paras->val_jso = prop_iter->val;

    i_paras->index = array_index;
    ret = provider[array_index].pfn_set(i_paras, &message_info_jso, &(prop_iter->val));
    
    if (ret == RF_IGNORE_CUR_OPERATION_RESULT && message_info_jso != NULL) {
        json_object_put(message_info_jso);
        message_info_jso = NULL;
    }

quit:

    if (NULL != message_info_jso) {
        if (json_type_array == json_object_get_type(message_info_jso)) {
            array_len = json_object_array_length(message_info_jso);

            for (msg_index = 0; msg_index < array_len; msg_index++) {
                (void)json_object_array_add(message_array_jso,
                    json_object_get(json_object_array_get_idx(message_info_jso, msg_index)));
            }

            json_object_put(message_info_jso);
        } else {
            (void)json_object_array_add(message_array_jso, message_info_jso);
        }
    }

    return ret;
}



LOCAL gint32 perform_patch_operation(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param,
    json_object *valid_props_jso, json_object *message_array_jso)
{
    gint32 ret = HTTP_BAD_REQUEST; // 如果没有一个属性更新成功，则返回400
    gint32 provider_ret;
    RESOURCE_PROVIDER_S *resource_provider = NULL;
    PROPERTY_PROVIDER_S *provider = NULL;
    json_object *message_info_jso = NULL;
    guint32 count = 0;
    json_object_iter prop_iter;

    
    return_val_if_fail(NULL != message_array_jso, HTTP_INTERNAL_SERVER_ERROR);
    

    
    i_param->user_data = valid_props_jso;
    

    // 此处用于消除foreach编译告警"assignment discards 'const' qualifier from pointer target type"
#pragma GCC diagnostic push // require GCC 4.6
#pragma GCC diagnostic ignored "-Wcast-qual"
    
    // 遍历有效属性，调用接口执行修改动作，修改成功不处理，修改失败记录对应错误消息
    provider_ret = get_resource_provider(request_info->uri_lower, &resource_provider);
    return_val_do_info_if_fail(VOS_OK == provider_ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s %d: get resource provider fail", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        (void)json_object_array_add(message_array_jso, message_info_jso));

    if (resource_provider->resource_provider_fn) {
        provider_ret = resource_provider->resource_provider_fn(i_param, &provider, &count);
        return_val_do_info_if_fail(VOS_OK == provider_ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s %d: get resource provider_fn fail", __FUNCTION__, __LINE__);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
            (void)json_object_array_add(message_array_jso, message_info_jso));
    }

    
    json_object_object_foreach0(valid_props_jso, name, val)
    {
        prop_iter.key = name;
        prop_iter.val = val;

        
        
        provider_ret = provider_set_prop_values(i_param, &prop_iter, message_array_jso, provider, count);
        
        
        if (provider_ret == RF_IGNORE_CUR_OPERATION_RESULT) {
            continue;
        }

        
        // 添加返回值判断，修改失败记录在无效属性对象中
        if (provider_ret == HTTP_OK) {
            ret = VOS_OK;
        }

        if ((ret != VOS_OK) && (provider_ret == HTTP_NOT_IMPLEMENTED)) {
            ret = provider_ret;
        }

        // 如果返回为500等,直接返回,其他4xx返回值均返回400,通过自定义消息来区分
        if (provider_ret == HTTP_INTERNAL_SERVER_ERROR) {
            ret = provider_ret;
            break;
        }

        
    }
    
#pragma GCC diagnostic pop // require GCC 4.6

    // 此处兼容官方schema定义可写,但当前iBMC不支持写的场景,此条件下如果patch多个属性均失败,则返回400
    if ((json_object_array_length(message_array_jso) > 1) && (HTTP_NOT_IMPLEMENTED == ret)) {
        ret = HTTP_BAD_REQUEST;
    }

    return ret;
}


LOCAL gint32 get_expand_levels(const gchar *expand_period_value, gint32 *levels)
{
    gchar *head_flag = NULL;
    gchar *tail_flag = NULL;
    gint32 ret;

    return_val_do_info_if_fail((NULL != expand_period_value) && (NULL != levels), VOS_ERR,
        debug_log(DLOG_ERROR, "[%s]: input parms error", __FUNCTION__));

    head_flag = g_strrstr(expand_period_value, QUERY_EXPAND_LEVELS);
    return_val_do_info_if_fail(NULL != head_flag, VOS_ERR,
        debug_log(DLOG_INFO, "[%s]: No $levels in expand query para.", __FUNCTION__));

    head_flag = g_strrstr(expand_period_value, "=");
    return_val_do_info_if_fail(NULL != head_flag, VOS_ERR,
        debug_log(DLOG_ERROR, "[%s]: No = found in expand query para.", __FUNCTION__));
    *head_flag = '\0';
    head_flag++;

    // 查询参数中的')'
    tail_flag = g_strrstr(head_flag, URL_REVERSE_BRACKETS);
    return_val_do_info_if_fail(NULL != tail_flag, VOS_ERR,
        debug_log(DLOG_ERROR, "[%s]: No ) found in expand query para.", __FUNCTION__));

    *tail_flag = '\0';

    ret = vos_str2int(head_flag, 10, levels, NUM_TPYE_INT32);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "[%s]: Get levels para error, levels str is %s,ret is %d.", __FUNCTION__, head_flag,
        ret));
    //  不支持levels为0的查询
    return_val_do_info_if_expr(*levels == 0, VOS_ERR,
        debug_log(DLOG_ERROR, "[%s]: Levels is not support, the value is %d.", __FUNCTION__, *levels));

    return VOS_OK;
}


LOCAL gint32 parse_expand_query_param(const REQUEST_INFO_S *request_info, json_object **message_info_jso,
    gint32 *expand_type, gint32 *levels)
{
    gint32 idx;
    gint32 ret = VOS_OK;
    gchar type = 0;

    return_val_do_info_if_fail((NULL != request_info) && (NULL != message_info_jso) && (NULL != expand_type) &&
        (NULL != levels),
        VOS_ERR, debug_log(DLOG_ERROR, "[%s]: input parms error", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_info_jso));

    for (idx = 0; idx < request_info->query_param_num; idx++) {
        continue_if_fail(0 == g_ascii_strcasecmp(request_info->query_params[idx].key, QUERY_EXPAND));
        debug_log(DLOG_DEBUG, "[%s] expand query_param[%d]:%s,value is %s.", __FUNCTION__, idx,
            request_info->query_params[idx].key, request_info->query_params[idx].value);

        type = request_info->query_params[idx].value[0];

        // 执行expand查询参数
        switch (type) {
            case EXPAND_ASTERISK:
                *expand_type = ASTERISK;

                // 暂时对'*'和'~'不支持，返回ERROR
                (void)create_message_info(MSGID_QUERY_NOT_SUPPORTED_RSC, NULL, message_info_jso);
                return VOS_ERR;

            case EXPAND_TILDE:
                *expand_type = TILDE;

                // 暂时对'*'和'~'不支持，返回ERROR
                (void)create_message_info(MSGID_QUERY_NOT_SUPPORTED_RSC, NULL, message_info_jso);
                return VOS_ERR;

            case EXPAND_PERIOD:
                *expand_type = PERIOD;

                // 只有一个'.'则退出查找, 默认*level = 1
                if (strlen(request_info->query_params[idx].value) == 1) {
                    *levels = 1;
                } else {
                    ret = get_expand_levels(request_info->query_params[idx].value, levels);
                    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, 
                        debug_log(DLOG_ERROR, "%s %d: get_expand_levels ret is %d.", __FUNCTION__, __LINE__, ret);
                        (void)create_message_info(MSGID_QUERY_PARA_TYPE_ERR, NULL, message_info_jso,
                            request_info->query_params[idx].value, QUERY_EXPAND));
                }
                break;
            default:
                // 有expand参数，但是参数值错误
                (void)create_message_info(MSGID_QUERY_PARA_TYPE_ERR, NULL, message_info_jso,
                    request_info->query_params[idx].value, QUERY_EXPAND);
                return VOS_ERR;
        }
    }
    return VOS_OK;
}


LOCAL gint32 expand_arry_hyperlinks(json_object *redfish_jso, const char *key, json_object *array, guint32 depth)
{
    const gchar *resource_uri = NULL;
    json_object *array_iter = NULL;
    gint32 ret = VOS_OK;
    json_object *rsc_jso = NULL;
    json_type t;
    gint32 array_len = 0;
    gint32 i = 0;
    json_object *array_item_tmp = NULL;

    return_val_do_info_if_fail((NULL != array) && (NULL != redfish_jso) && (NULL != key), VOS_ERR,
        debug_log(DLOG_ERROR, "[%s]: input parms error", __FUNCTION__));

    t = json_object_get_type(array);
    return_val_do_info_if_fail(json_type_array == t, VOS_ERR,
        debug_log(DLOG_ERROR, "[%s]: redfish_jso is not json_type_object, t is %d", __FUNCTION__, t));

    array_iter = json_object_array_get_idx(array, 0);
    if ((json_type_object == json_object_get_type(array_iter)) &&
        (NAVIGATION_PROP_OBJ_LEN == json_object_object_length(array_iter)) &&
        (json_object_has_key(array_iter, ODATA_ID))) {
        array_len = json_object_array_length(array);

        for (i = 0; i < array_len; i++) {
            array_iter = json_object_array_get_idx(array, i);

            return_val_do_info_if_fail((json_type_object == json_object_get_type(array_iter)) &&
                (NAVIGATION_PROP_OBJ_LEN == json_object_object_length(array_iter)) &&
                (json_object_has_key(array_iter, ODATA_ID)),
                VOS_ERR, debug_log(DLOG_ERROR, "[%s]: The array item is not support expand.", __FUNCTION__));

            (void)get_element_str(array_iter, ODATA_ID, &resource_uri);
            debug_log(DLOG_DEBUG, "[%s]: resource_uri is %s.", __FUNCTION__, resource_uri);

            ret = rf_validate_rsc_exist(resource_uri, &rsc_jso);
            continue_do_info_if_fail(VOS_OK == ret,
                debug_log(DLOG_ERROR, "[%s]: expand rsc:%s failed, ret is %d", __FUNCTION__, resource_uri, ret));

            json_object_array_put_idx(array, i, rsc_jso);
        }

        json_object_object_add(redfish_jso, key, json_object_get(array));
    }

    // 如果是 object_type,长度大于1，则对obj重新进行递归调用处理，判断数组中是否有满足展开条件的超链接
    if ((json_type_object == json_object_get_type(array_iter)) &&
        (NAVIGATION_PROP_OBJ_LEN < json_object_object_length(array_iter))) {
        json_object_array_foreach(array, array_item_tmp)
        {
            ret = expand_period_hyperlinks(depth, array_item_tmp);
            return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
                debug_log(DLOG_ERROR, "[%s]: expand_period_hyperlinks error %d", __FUNCTION__, ret));
        }
    }

    return VOS_OK;
}


LOCAL gint32 expand_period_hyperlinks(guint32 depth, json_object *redfish_jso)
{
    gint32 ret = VOS_OK;
    const gchar *resource_uri = NULL;
    json_type t;
    json_object *rsc_jso = NULL;

    return_val_if_fail(NULL != redfish_jso, VOS_ERR);

    depth++;
    return_val_do_info_if_fail(depth < MAX_RECURSIVE_DEPTH, VOS_ERR,
        debug_log(DLOG_ERROR, "[%s]: failed:max recursive depth exceed, current depth is %d", __FUNCTION__, depth));

    t = json_object_get_type(redfish_jso);
    return_val_do_info_if_fail(json_type_object == t, VOS_ERR,
        debug_log(DLOG_ERROR, "[%s]: redfish_jso is not json_type_object, t is %d", __FUNCTION__, t));

#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#endif
    json_object_object_foreach0(redfish_jso, name, val)
    {
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif
        t = json_object_get_type(val);
        if (t == json_type_object) {
            // 仅对json_obj类型且元素仅有一个@odata.id的值进行展开
            if (NAVIGATION_PROP_OBJ_LEN == json_object_object_length(val) && json_object_has_key(val, ODATA_ID)) {
                (void)get_element_str(val, ODATA_ID, &resource_uri);
                debug_log(DLOG_DEBUG, "[%s]: resource_uri is:%s, length(val) is %d, key(val, ODATA_ID) is %d.",
                    __FUNCTION__, resource_uri, json_object_object_length(val), json_object_has_key(val, ODATA_ID));

                ret = rf_validate_rsc_exist(resource_uri, &rsc_jso);
                continue_do_info_if_fail(VOS_OK == ret,
                    debug_log(DLOG_ERROR, "[%s]: expand rsc:%s failed, ret is %d", __FUNCTION__, resource_uri, ret));

                json_object_object_add(redfish_jso, name, rsc_jso);
                continue;
            }

            // 对于其他类型的json_obj进行递归调用进一步处理
            ret = expand_period_hyperlinks(depth, val);
            return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
                debug_log(DLOG_ERROR, "[%s]: expand_period_hyperlinks error %d", __FUNCTION__, ret));
        } else if (t == json_type_array) {
            ret = expand_arry_hyperlinks(redfish_jso, name, val, depth);
            return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
                debug_log(DLOG_ERROR, "[%s]: expand_arry_hyperlinks error %d", __FUNCTION__, ret));
        } else {
            continue;
        }
    }

    return VOS_OK;
}


LOCAL gint32 perform_expand_query_hyperlinks(json_object *redfish_jso, json_object **message_info_jso,
    gint32 expand_type, gint32 levels)
{
#define EXPAND_LEVELS_RANGE "1"
    gint32 ret = VOS_OK;
    gchar str_num[STRING_LEN_MAX] = {0};

    return_val_do_info_if_fail((NULL != redfish_jso) && (EXPAND_INIT != expand_type) && (NULL != message_info_jso),
        VOS_ERR, debug_log(DLOG_ERROR, "[%s]: input parms error", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_info_jso));

    switch (expand_type) {
        case PERIOD:

            // 当前仅支持levels为1，其余返回超过参数限制
            (void)snprintf_s(str_num, sizeof(str_num), sizeof(str_num) - 1, "%d", levels);
            return_val_do_info_if_fail(levels == 1, VOS_ERR,
                (void)create_message_info(MSGID_QUERY_OUT_OF_RANGE, NULL, message_info_jso, (const gchar *)str_num,
                QUERY_EXPAND, EXPAND_LEVELS_RANGE);
                debug_log(DLOG_DEBUG, "[%s]: perform expand levles is %d", __FUNCTION__, levels));

            ret = expand_period_hyperlinks(0, redfish_jso);
            return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
                debug_log(DLOG_ERROR, "[%s]: expand_period_hyperlinks error", __FUNCTION__);
                (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_info_jso));
            break;

        case ASTERISK:

        case TILDE:

        default:
            debug_log(DLOG_ERROR, "[%s]: Input parm is not support.", __FUNCTION__);
            (void)create_message_info(MSGID_QUERY_NOT_SUPPORTED_RSC, NULL, message_info_jso);
            return VOS_ERR;
    }

    return VOS_OK;
}

LOCAL gint32 process_object_filter_handler(const PROVIDER_PARAS_S *i_param, const REQUEST_INFO_S *request_info,
    json_object *redfish_jso, json_object *message_array_jso)
{
    gint32 skip = 0;
    gint32 top = 0;
    json_object *members_jso = NULL;
    json_object *members_cnt_jso = NULL;
    json_object *members_new_jso = NULL;
    json_bool ret_json;
    gint32 head;
    gint32 tail;
    gint32 idx;
    gint32 members_cnt;
    gint32 ret;
    json_object *message_info_jso = NULL;
    guint32 para_flag = PARA_INIT;

    return_val_if_fail(NULL != message_array_jso, HTTP_INTERNAL_SERVER_ERROR);

    return_val_do_info_if_fail((NULL != request_info) && (NULL != redfish_jso), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "[%s]: input parms error", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(message_array_jso, message_info_jso));

    ret = get_skip_and_top(request_info, &message_info_jso, &skip, &top, &para_flag);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_BAD_REQUEST,
        json_object_array_add(message_array_jso, message_info_jso));

    ret_json = json_object_object_get_ex(redfish_jso, JSON_SCHEMA_MEMBERS, &members_jso);
    //  不存在集合资源，有skip或者top返回错误，没有skip和top直接返回OK，后面处理expand参数
    if (!ret_json || (NULL == members_jso)) {
        if ((para_flag & TOP_EXIT) || (para_flag & SKIP_EXIT)) {
            // 找不到Members说明不是集合成员，则返回不支持查询参数
            debug_log(DLOG_DEBUG, "[%s]: json_object_object_get_ex JSON_SCHEMA_MEMBERS fail, uri is %s", __FUNCTION__,
                request_info->uri_lower);
            (void)create_message_info(MSGID_QUERY_NOT_SUPPORTED_RSC, NULL, &message_info_jso);
            json_object_array_add(message_array_jso, message_info_jso);
            return HTTP_BAD_REQUEST;
        } else {
            // 其它正常的资源返回OK, 不影响其它操作.
            return VOS_OK;
        }
    }

    ret_json = json_object_object_get_ex(redfish_jso, JSON_SCHEMA_MEMBERS_COUNT, &members_cnt_jso);
    return_val_do_info_if_fail(ret_json && (NULL != members_cnt_jso), HTTP_BAD_REQUEST,
        debug_log(DLOG_DEBUG, "[%s]: json_object_object_get_ex JSON_SCHEMA_MEMBERS_COUNT fail, uri is %s", __FUNCTION__,
        request_info->uri_lower);
        (void)create_message_info(MSGID_QUERY_NOT_SUPPORTED_RSC, NULL, &message_info_jso);
        json_object_array_add(message_array_jso, message_info_jso));

    // 查询参数大于3个，目前仅支持skip,top,expand
    return_val_do_info_if_fail(request_info->query_param_num <= MULTI_KEY_MAX_NUM, HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "[%s]: query_param_num: %d", __FUNCTION__, request_info->query_param_num);
        (void)create_message_info(MSGID_QUERY_NOT_SUPPORTED_RSC, NULL, &message_info_jso);
        json_object_array_add(message_array_jso, message_info_jso));

    members_cnt = json_object_get_int(members_cnt_jso);
    if (members_cnt < 1) {
        json_object_object_add(redfish_jso, JSON_SCHEMA_MEMBERS, json_object_new_array());

        return VOS_OK;
    }

    
    ret = skip_and_top_param_verify(members_cnt, &message_info_jso, &skip, &top, para_flag);
    

    return_val_do_info_if_fail((VOS_OK == ret), HTTP_BAD_REQUEST,
        json_object_array_add(message_array_jso, message_info_jso));

    head = skip;
    tail = ((head + top) < members_cnt) ? (head + top) : members_cnt;

    debug_log(DLOG_DEBUG, "[%s]: $skip=%d&$top=%d, range:%d-%d", __FUNCTION__, skip, top, head, tail);

    
    if (i_param->skip != INVALID_INTEGER_VALUE && i_param->top != INVALID_INTEGER_VALUE) {
        members_new_jso = json_object_new_array();

        for (idx = head; idx < tail; idx++) {
            
            json_object_array_add(members_new_jso, json_object_get(json_object_array_get_idx(members_jso, idx)));
            
        }

        json_object_object_add(redfish_jso, JSON_SCHEMA_MEMBERS, members_new_jso);
    }

    // 分页处理:
    (void)generate_members_next_link(request_info, redfish_jso, tail, members_cnt);

    return VOS_OK;
}


LOCAL gint32 process_object_expand_handler(const REQUEST_INFO_S *request_info, json_object *redfish_jso,
    json_object *message_array_jso)
{
    gint32 expand_type = EXPAND_INIT;
    gint32 levels = 0;
    gint32 ret;
    json_object *message_info_jso = NULL;

    return_val_do_info_if_fail((NULL != request_info) && (NULL != redfish_jso) && (NULL != message_array_jso),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "[%s]: input parms error", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(message_array_jso, message_info_jso));

    ret = parse_expand_query_param(request_info, &message_info_jso, &expand_type, &levels);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST,
        json_object_array_add(message_array_jso, message_info_jso);
        debug_log(DLOG_DEBUG, "[%s] parse_expand_query_param is error.", __FUNCTION__));

    // 没有expand参数不作处理,返回跳过expand处理
    return_val_do_info_if_fail(expand_type != EXPAND_INIT, VOS_OK,
        debug_log(DLOG_DEBUG, "[%s] no expand para in url.", __FUNCTION__));

    // 有expand参数，根据expand和Levels来遍历redfish_jso中的o_data_id，并进行展开
    ret = perform_expand_query_hyperlinks(redfish_jso, &message_info_jso, expand_type, levels);

    return_val_do_info_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST,
        json_object_array_add(message_array_jso, message_info_jso);
        debug_log(DLOG_DEBUG, "[%s] parse_expand_query_param is error.", __FUNCTION__));

    return VOS_OK;
}


LOCAL gint32 perform_query_operation(const PROVIDER_PARAS_S *i_param, const REQUEST_INFO_S *request_info,
    json_object *response_jso, json_object *message_array_jso)
{
    json_object *redfish_jso = NULL;

    gint32 ret = VOS_OK;
    json_object *message_info_jso = NULL;
    gint32 i = 0;
    gint32 k = 0;

    
    return_val_if_fail(NULL != message_array_jso, HTTP_INTERNAL_SERVER_ERROR);

    return_val_do_info_if_fail((NULL != request_info) && (NULL != response_jso) &&
        (TRUE == json_object_object_get_ex(response_jso, RESPONSE_BODY, &redfish_jso)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "[%s]: input parms error", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(message_array_jso, message_info_jso));
    

    // 有重复的查询参数
    if ((request_info->query_param_num >= 2) && (request_info->query_param_num <= MAX_KEYS_NUM)) {
        for (i = 0; i < request_info->query_param_num; i++) {
            for (k = i + 1; k < request_info->query_param_num; k++)
                return_val_do_info_if_expr(
                    (0 == g_ascii_strcasecmp(request_info->query_params[i].key, request_info->query_params[k].key)),
                    HTTP_BAD_REQUEST,
                    debug_log(DLOG_ERROR, "[%s]: %s = %s", __FUNCTION__, request_info->query_params[i].key,
                    request_info->query_params[k].key);
                    (void)create_message_info(MSGID_QUERY_NOT_SUPPORTED_RSC, NULL, &message_info_jso);
                    json_object_array_add(message_array_jso, message_info_jso));
        }
    }

    if (0 == g_ascii_strncasecmp(request_info->uri_lower, DATA_ACQ_REPORT_PREFIX, strlen(DATA_ACQ_REPORT_PREFIX))) {
        // 查询校验参数个数
        return_val_do_info_if_fail(request_info->query_param_num <= MAX_KEYS_NUM, HTTP_BAD_REQUEST,
            debug_log(DLOG_ERROR, "[%s]: query_param_num: %d", __FUNCTION__, request_info->query_param_num);
            (void)create_message_info(MSGID_QUERY_NOT_SUPPORTED_RSC, NULL, &message_info_jso);
            json_object_array_add(message_array_jso, message_info_jso));

        // 资源指定一个参数或不指定参数返回ok
        return VOS_OK;
    } else {
        ret = process_object_filter_handler(i_param, request_info, redfish_jso, message_array_jso);
        return_val_do_info_if_fail(VOS_OK == ret, ret,
            debug_log(DLOG_DEBUG, "[%s] process_object_filter_handler is error.", __FUNCTION__));

        ret = process_object_expand_handler(request_info, redfish_jso, message_array_jso);
        return_val_do_info_if_fail(VOS_OK == ret, ret,
            debug_log(DLOG_DEBUG, "[%s] process_object_expand_handler is error.", __FUNCTION__));
    }

    return VOS_OK;
}


LOCAL gint32 get_skip_and_top(const REQUEST_INFO_S *request_info, json_object **message_info_jso, gint32 *skip,
    gint32 *top, guint32 *prara_flag)
{
    gint32 idx;
    gint32 ret;
    guint32 filter_para_flag = FALSE;

    return_val_do_info_if_expr((NULL == request_info) || (NULL == message_info_jso) || (NULL == skip) || (NULL == top),
        VOS_ERR, debug_log(DLOG_ERROR, "[%s]: input parms error", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_info_jso));

    (*skip) = 0;
    for (idx = 0; (idx < request_info->query_param_num) && (idx < MAX_KEYS_NUM); idx++) {
        debug_log(DLOG_DEBUG, "[%s] query_param[%d]:%s", __FUNCTION__, idx, request_info->query_params[idx].key);

        if (0 == g_ascii_strcasecmp(request_info->query_params[idx].key, QUERY_SKIP)) {
            // 执行skip查询参数
            ret = vos_str2int(request_info->query_params[idx].value, 10, skip, NUM_TPYE_INT32);
            return_val_do_info_if_expr((VOS_OK != ret), VOS_ERR,
                (void)create_message_info(MSGID_QUERY_PARA_TYPE_ERR, NULL, message_info_jso,
                request_info->query_params[idx].value, QUERY_SKIP));
            filter_para_flag |= SKIP_EXIT;
        } else if (0 == g_ascii_strcasecmp(request_info->query_params[idx].key, QUERY_TOP)) {
            // 执行top查询参数
            ret = vos_str2int(request_info->query_params[idx].value, 10, top, NUM_TPYE_INT32);
            return_val_do_info_if_expr((VOS_OK != ret), VOS_ERR,
                (void)create_message_info(MSGID_QUERY_PARA_TYPE_ERR, NULL, message_info_jso,
                request_info->query_params[idx].value, QUERY_TOP));
            filter_para_flag |= TOP_EXIT;
        }
    }

    if (FALSE == (filter_para_flag & TOP_EXIT)) {
        (*top) = JSON_SCHEMA_MEMBERS_MAX;
    }

    if (NULL != prara_flag) {
        *prara_flag = filter_para_flag;
    }
    return VOS_OK;
}


LOCAL gint32 skip_and_top_param_verify(gint32 member_count, json_object **message_info_jso, gint32 *skip, gint32 *top,
    guint32 para_flag)
{
    gchar str_num[STRING_LEN_MAX] = {0};
    gchar str_tmp[MAX_URI_LENGTH] = {0};

    return_val_do_info_if_fail((NULL != message_info_jso) && (NULL != skip) && (NULL != top), VOS_ERR,
        debug_log(DLOG_ERROR, "[%s]: input parms error", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_info_jso));

    if ((para_flag & TOP_EXIT) == FALSE) {
        // 如果查询参数没有top:
        (*top) =
            ((member_count - (*skip)) < JSON_SCHEMA_MEMBERS_MAX) ? (member_count - (*skip)) : JSON_SCHEMA_MEMBERS_MAX;
    }

    if ((*skip) < 0 || (*skip) >= member_count) {
        (void)snprintf_s(str_num, sizeof(str_num), sizeof(str_num) - 1, "%d", (*skip));
        (void)snprintf_s(str_tmp, sizeof(str_tmp), sizeof(str_tmp) - 1, "%d-%d", 0, member_count - 1);
        (void)create_message_info(MSGID_QUERY_OUT_OF_RANGE, NULL, message_info_jso, (const gchar *)str_num, QUERY_SKIP,
            (const gchar *)str_tmp);
        return VOS_ERR;
    }

    if ((*top) < 1 || (*top) > (member_count - (*skip))) {
        (void)snprintf_s(str_num, sizeof(str_num), sizeof(str_num) - 1, "%d", (*top));
        (void)snprintf_s(str_tmp, sizeof(str_tmp), sizeof(str_tmp) - 1, "%d-%d", 1, (member_count - (*skip)));
        (void)create_message_info(MSGID_QUERY_OUT_OF_RANGE, NULL, message_info_jso, (const gchar *)str_num, QUERY_TOP,
            (const gchar *)str_tmp);
        return VOS_ERR;
    }

    // 对输入的top参数作32条分页限制:
    (*top) = ((*top) < JSON_SCHEMA_MEMBERS_MAX) ? (*top) : JSON_SCHEMA_MEMBERS_MAX;

    return VOS_OK;
}


LOCAL gint32 get_odata_id_from_response(json_object *response_jso, gchar *odata_id, size_t odata_id_sz)
{
    gint32 ret;
    json_object *odataid_jso = NULL;
    gint32 str_len;

    return_val_do_info_if_expr(NULL == response_jso || NULL == odata_id, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: NULL point", __FUNCTION__));
    ret = json_object_object_get_ex(response_jso, RF_PROPERTY_ODATA_ID, &odataid_jso);
    str_len = json_object_get_string_len(odataid_jso);
    return_val_do_info_if_expr(0 == ret || NULL == odataid_jso || 0 == str_len, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:get property @odata.id fail", __FUNCTION__));
    ret = strncpy_s(odata_id, odata_id_sz - 1, dal_json_object_get_str(odataid_jso), str_len);
    return_val_do_info_if_expr(ret != EOK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret:%d", __FUNCTION__, ret));

    return HTTP_OK;
}



LOCAL gint32 generate_members_next_link(const REQUEST_INFO_S *request_info, json_object *response_jso, gint32 idx,
    gint32 count)
{
    int iRet = -1;
    gint32 skip = 0;
    gint32 top = 0;
    gchar str_tmp[MAX_URI_LENGTH] = {0};
    gchar odata_id[MAX_URI_LENGTH] = {0} ;

    return_val_if_fail(NULL != response_jso, VOS_ERR);

    // 不是最后一页, 作分页处理:
    if (idx < count) {
        iRet = get_odata_id_from_response(response_jso, odata_id, sizeof(odata_id));
        return_val_if_fail(HTTP_OK == iRet, VOS_ERR);
        skip = idx;
        top = ((count - skip) < JSON_SCHEMA_MEMBERS_MAX) ? (count - skip) : JSON_SCHEMA_MEMBERS_MAX;

        iRet = snprintf_s(str_tmp, sizeof(str_tmp), sizeof(str_tmp) - 1, "%s?$skip=%d&$top=%d", odata_id, skip, top);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

        json_object_object_add(response_jso, JSON_SCHEMA_MEMBERS_NEXT_LINK,
            json_object_new_string((const gchar *)str_tmp));

        debug_log(DLOG_DEBUG, "[%s]: next link is \"%s\"", __FUNCTION__, str_tmp);
    }

    return VOS_OK;
}


LOCAL void generate_link_header_with_rsp_str(const gchar *rsp_body, json_object *header_array_jso)
{
    gint32 ret;
    const gchar *odata_type_start = NULL;
    const gchar *val_start = NULL;
    const gchar *val_end = NULL;
    gchar odata_type_str[MAX_MEM_ID_LEN] = {0};
    json_object *header_jso = NULL;

    
    odata_type_start = strstr(rsp_body, ODATA_TYPE_KEY);
    if (odata_type_start == NULL) {
        debug_log(DLOG_ERROR, "%s:missing %s in response body", __FUNCTION__, ODATA_TYPE_KEY);
        return;
    }

    
    val_start = strstr(odata_type_start, "#");
    if (val_start == NULL) {
        debug_log(DLOG_ERROR, "%s:missing %s in response body", __FUNCTION__, ODATA_TYPE_KEY);
        return;
    }

    val_end = strstr(val_start, "\"");
    if (val_end == NULL) {
        debug_log(DLOG_ERROR, "%s:missing %s in response body", __FUNCTION__, ODATA_TYPE_KEY);
        return;
    }

    ret = strncpy_s(odata_type_str, sizeof(odata_type_str), val_start, val_end - val_start);
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "%s:call strncpy_s failed, ret:%d", __FUNCTION__, ret);
        return;
    }

    ret = generate_link_header_by_odata_type((const gchar *)odata_type_str, &header_jso);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s:generate link header failed", __FUNCTION__);
    } else {
        json_object_array_add(header_array_jso, header_jso);
    }

    return;
}


LOCAL gint32 generate_get_headers_with_resp_str(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param,
    const gchar *rsp_body, json_object *header_array_jso, json_object *message_array_jso)
{
    json_object *rsc_template_jso = NULL;
    json_object *header_jso = NULL;
    json_object *msg_jso = NULL;
    json_object *allow_jso = NULL;

    
    gint32 ret =
        generate_common_response_headers(&header_array_jso, (const gchar *)request_info->uri_lower, message_array_jso);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: call generate_common_response_headers failed", __FUNCTION__);
        goto ERR_EXIT;
    }

    
    
    if (i_param->is_from_webui == 0) {
        ret = generate_rsc_etag_header(rsp_body, &header_jso);
        if (ret == VOS_OK) {
            (void)json_object_array_add(header_array_jso, header_jso);
        } else {
            debug_log(DLOG_ERROR, "%s: generate rsc etag header failed, ret:%d", __FUNCTION__, ret);
        }
    }

    
    header_jso = NULL;
    header_jso = monitor_create_location();
    if (header_jso != NULL) {
        (void)json_object_array_add(header_array_jso, header_jso);
    }

    add_extern_response_head(header_array_jso, request_info->uri_lower);

    
    generate_link_header_with_rsp_str(rsp_body, header_array_jso);

    
    rsc_template_jso = json_object_from_file((const gchar *)request_info->resource_path);
    if (rsc_template_jso == NULL) {
        goto ERR_EXIT;
    }
    if (json_object_object_get_ex(rsc_template_jso, ALLOWED_OPERATION, &allow_jso) == FALSE) {
        debug_log(DLOG_ERROR, "%s:missing %s in resource template json", __FUNCTION__, ALLOWED_OPERATION);
        json_object_put(rsc_template_jso);
        goto ERR_EXIT;
    }

    header_jso = NULL;
    ret = generate_allow_response_header(allow_jso, &header_jso);
    json_object_put(rsc_template_jso);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: generate allow response header failed", __FUNCTION__);
        goto ERR_EXIT;
    }
    (void)json_object_array_add(header_array_jso, header_jso);
    return VOS_OK;

ERR_EXIT:
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &msg_jso);
    (void)json_object_array_add(message_array_jso, msg_jso);
    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL void update_rsp_str(const gchar *odata_info, gchar **rsp_str)
{
    gchar *new_rsp = NULL;
    guint32 new_rsp_len;
    gchar *flag = NULL;
    gint32 ret;

    
    flag = strstr(*rsp_str, "{");
    
    if (flag == NULL || strlen(flag) <= 1) {
        debug_log(DLOG_DEBUG, "%s: rsp body is null or not a object", __FUNCTION__);
        return;
    }

    new_rsp_len = strlen(odata_info) + strlen(*rsp_str) + 1;
    new_rsp = (gchar *)g_malloc0(new_rsp_len);
    if (new_rsp == NULL) {
        debug_log(DLOG_ERROR, "%s: g_malloc0 failed", __FUNCTION__);
        return;
    }

    ret = snprintf_s(new_rsp, new_rsp_len, new_rsp_len - 1, "{%s%s", odata_info, flag + 1);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s failed, ret:%d", __FUNCTION__, ret);
        g_free(new_rsp);
        return;
    }

    
    g_free(*rsp_str);
    *rsp_str = new_rsp;
    return;
}


LOCAL void add_odata_info(gchar **rsp_str, const gchar *resource_path)
{
    gboolean b_ret;
    json_object *rsc_template = NULL;
    json_object *redfish_obj = NULL;
    const gchar *odata_context = NULL;
    const gchar *odata_type = NULL;
    
    gchar buffer[MAX_RSC_ID_LEN * 3] = {0};

    if (rsp_str == NULL || *rsp_str == NULL || resource_path == NULL) {
        debug_log(DLOG_DEBUG, "%s: NULL pointer", __FUNCTION__);
        return;
    }

    rsc_template = json_object_from_file(resource_path);
    if (rsc_template == NULL) {
        debug_log(DLOG_ERROR, "%s: parse resource template file to json failed", __FUNCTION__);
        return;
    }

    b_ret = json_object_object_get_ex(rsc_template, REDFISH_OBJECT, &redfish_obj);
    if (b_ret != TRUE || redfish_obj == NULL) {
        debug_log(DLOG_DEBUG, "%s: not found %s in resource template file", __FUNCTION__, REDFISH_OBJECT);
        json_object_put(rsc_template);
        return;
    }

    (void)get_element_str(redfish_obj, RFPROP_ODATA_CONTEXT, &odata_context);
    (void)get_element_str(redfish_obj, RFPROP_ODATA_TYPE, &odata_type);

    add_val_str_to_rsp_body(RFPROP_ODATA_CONTEXT, odata_context, FALSE, buffer, sizeof(buffer));
    add_val_str_to_rsp_body(RFPROP_ODATA_TYPE, odata_type, FALSE, buffer, sizeof(buffer));
    json_object_put(rsc_template);

    if (strlen(buffer) == 0) {
        return;
    }

    update_rsp_str((const gchar *)buffer, rsp_str);
    return;
}


LOCAL gint32 check_params_before_do_get(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param,
    json_object *message_array_jso)
{
    PROPERTY_PROVIDER_S *provider = NULL;
    guint32 count = 0;
    gint32 ret;
    json_object *message_jso = NULL;

    if (request_info->res_provider == NULL) {
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_jso, (const gchar *)request_info->relative_uri);
        json_object_array_add(message_array_jso, message_jso);
        return HTTP_NOT_FOUND;
    }

    
    ret = check_rsc_entry_accessiable(request_info, i_param, &provider, &count, message_array_jso);
    if (ret != VOS_OK) {
        return ret;
    }

    
    if (request_info->res_provider->do_get == NULL) {
        debug_log(DLOG_ERROR, "%s: do_get callback func is NULL, please check resource regsitry info for this uri:%s",
            __FUNCTION__, request_info->uri_lower);
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_jso, (const gchar *)request_info->relative_uri);
        json_object_array_add(message_array_jso, message_jso);
        return HTTP_NOT_FOUND;
    }

    return VOS_OK;
}


LOCAL gint32 get_response_body(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param, json_object *message_array_jso,
    gchar **rsp_str)
{
    json_object *message_jso = NULL;
    gint32 ret;
    gint32 ret_code = 0;

    
    ret = request_info->res_provider->do_get(i_param, NULL, message_array_jso, rsp_str);
    
    if (ret != RF_OK && ret != HTTP_OK && ret != HTTP_ACCEPTED && ret != HTTP_CREATED) {
        
        _parse_get_retvalue(ret, &ret_code, (const gchar *)request_info->relative_uri, message_array_jso);
        
        if (*rsp_str != NULL) {
            g_free(*rsp_str);
        }
        return ret_code;
    }

    
    if (*rsp_str == NULL) {
        debug_log(DLOG_ERROR, "%s: rsc provider return %d, but response body str is NULL", __FUNCTION__, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
        json_object_array_add(message_array_jso, message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    add_odata_info(rsp_str, (const gchar *)request_info->resource_path);
    return VOS_OK;
}


LOCAL gint32 generate_get_response_direct_from_provider(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param,
    HTTP_RESPONSE_DATA_S *response, json_object *message_array_jso)
{
    json_object *message_jso = NULL;
    gint32 ret;
    gchar *rsp_str = NULL;
    gint32 ret_code;
    json_object *header_array_jso = NULL;
    json_object *status_jso = NULL;

    ret = check_params_before_do_get(request_info, i_param, message_array_jso);
    if (ret != VOS_OK) {
        return ret;
    }

    ret = get_response_body(request_info, i_param, message_array_jso, &rsp_str);
    if (ret != VOS_OK) {
        return ret;
    }
    response->resp_body = rsp_str;

    
    header_array_jso = json_object_new_array();
    if (header_array_jso == NULL) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
        json_object_array_add(message_array_jso, message_jso);
        ret = HTTP_INTERNAL_SERVER_ERROR;
        goto ERR_EXIT;
    }
    ret = generate_get_headers_with_resp_str(request_info, i_param, rsp_str, header_array_jso, message_array_jso);
    if (ret != VOS_OK) {
        json_object_put(header_array_jso);
        goto ERR_EXIT;
    }
    response->resp_head = g_strdup(json_object_to_json_string_ext(header_array_jso, JSON_C_TO_STRING_NOSLASHESCAPE));
    json_object_put(header_array_jso);

    
    ret_code = (ret == VOS_OK) ? HTTP_OK : ret;
    ret = generate_response_status(ret_code, &status_jso, message_array_jso);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s:generate response status failed, ret:%d", __FUNCTION__, ret);
        goto ERR_EXIT;
    }
    response->resp_status = g_strdup(dal_json_object_get_str(status_jso));
    json_object_put(status_jso);
    return VOS_OK;

ERR_EXIT:
    
    destory_http_response_data(response);
    return ret;
}


LOCAL gint32 handle_get(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param, HTTP_RESPONSE_DATA_S *response)
{
    gint32 ret;
    json_object *response_jso = NULL;
    json_object *resource_jso = NULL;
    json_object *header_array_jso = NULL;
    json_object *status_jso = NULL;
    json_object *message_array_jso = NULL;
    json_object *message_info_jso = NULL;
    json_object *redfish_jso = NULL;
    json_bool ret_json;
    gint32 response_code;
    gint32 create_response_code = 0;
    gint32 status_code;

    message_array_jso = json_object_new_array();
    
    return_val_do_info_if_fail(NULL != message_array_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: message_array_jso is null", __FUNCTION__));
    

    if (i_param == NULL || request_info == NULL || response == NULL) {
        response_code = HTTP_INTERNAL_SERVER_ERROR;
        debug_log(DLOG_ERROR, "%s: response_str is null", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(message_array_jso, message_info_jso);
        goto error_exit;
    }

    
    if (is_large_response_uri((const gchar *)request_info->uri_lower) == VOS_OK) {
        ret = generate_get_response_direct_from_provider(request_info, i_param, response, message_array_jso);
        if (ret == VOS_OK) {
            goto normal_exit;
        } else {
            response_code = ret;
            goto error_exit;
        }
    }

    response_jso = json_object_new_object();
    if (response_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: new json object failed", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(message_array_jso, message_info_jso);
        response_code = HTTP_INTERNAL_SERVER_ERROR;
        goto error_exit;
    }

    // 特定URI需要单独分支处理
    if (is_static_rsc_uri(request_info->uri_lower) == VOS_OK) {
        ret = generate_specific_response_body(request_info, &resource_jso, message_array_jso);
        goto_label_do_info_if_fail((VOS_OK == ret), error_exit,
            debug_log(DLOG_ERROR, "%s generate_specific_response_body fail", __FUNCTION__);
            response_code = ret);
        json_object_object_add(response_jso, RESPONSE_BODY, json_object_get(resource_jso));

        header_array_jso = json_object_new_array();
        ret = generate_specific_response_headers(i_param->is_from_webui, resource_jso, request_info->uri_lower,
            &header_array_jso, message_array_jso);
        goto_label_do_info_if_fail((VOS_OK == ret), error_exit,
            debug_log(DLOG_ERROR, "%s generate response headers fail, uri is %s", __FUNCTION__,
            request_info->uri_lower);
            response_code = ret; json_object_put(header_array_jso));
        json_object_object_add(response_jso, RESPONSE_HEADERS, header_array_jso);
    } else {
        // 载入资源配置文件
        resource_jso = json_object_from_file(request_info->resource_path);
        goto_label_do_info_if_fail((NULL != resource_jso), error_exit, response_code = HTTP_NOT_FOUND;
            debug_log(DLOG_ERROR, "%s get resource from file fail", __FUNCTION__);
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_info_jso, request_info->relative_uri);
            (void)json_object_array_add(message_array_jso, message_info_jso));

        // 获取配置文件中资源相关属性
        ret_json = json_object_object_get_ex(resource_jso, REDFISH_OBJECT, &redfish_jso);
        goto_label_do_info_if_fail(ret_json, error_exit, response_code = HTTP_NOT_FOUND;
            debug_log(DLOG_ERROR, "%s get REDFISH_OBJECT from file fail", __FUNCTION__);
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_info_jso, request_info->relative_uri);
            (void)json_object_array_add(message_array_jso, message_info_jso));

        // 更新资源对象中属性值
        ret = generate_get_response_body(request_info, i_param, &redfish_jso, message_array_jso);
        
        goto_label_do_info_if_fail(((VOS_OK == ret) || (HTTP_CREATED == ret) || (HTTP_ACCEPTED == ret)), error_exit,
            debug_log(DLOG_ERROR, "%s update response body fail", __FUNCTION__);
            response_code = ret);
        create_response_code = ret;
        

        // 增加引用计数,避免出现double free
        json_object_object_add(response_jso, RESPONSE_BODY, json_object_get(redfish_jso));

        // 生成GET操作响应头信息
        header_array_jso = json_object_new_array();
        ret = generate_get_response_headers(i_param->is_from_webui, resource_jso, request_info, &header_array_jso,
            message_array_jso);
        goto_label_do_info_if_fail((VOS_OK == ret), error_exit,
            debug_log(DLOG_ERROR, "%s generate response headers fail", __FUNCTION__);
            response_code = ret; json_object_put(header_array_jso));
        json_object_object_add(response_jso, RESPONSE_HEADERS, header_array_jso);
    }

    
    // 处理@odata.nextLink

    ret = perform_query_operation(i_param, request_info, response_jso, message_array_jso);
    goto_label_do_info_if_fail((VOS_OK == ret), error_exit,
        debug_log(DLOG_ERROR, "%s update response body fail", __FUNCTION__);
        response_code = ret);

    // 非WEB发起的Redfish请求，校验ifnonematch请求头
    if ((!i_param->is_from_webui) && (create_response_code == VOS_OK)) {
        ret = verify_ifnonematch(request_info, header_array_jso);
        goto_label_do_info_if_fail((VOS_OK == ret) || (HTTP_NOT_MODIFIED == ret), error_exit,
            debug_log(DLOG_ERROR, "%s %d:verify_ifnonematch fail", __FUNCTION__, __LINE__);
            response_code = ret);

        check_success_do(ret == HTTP_NOT_MODIFIED, create_response_code = ret;
            json_object_object_del(response_jso, RESPONSE_BODY));
    }

    
    // 生成OK 状态码等状态信息
    
    status_code = (VOS_OK == create_response_code) ? HTTP_OK : create_response_code;
    ret = generate_response_status(status_code, &status_jso, message_array_jso);
    

    
    goto_label_do_info_if_fail((VOS_OK == ret), error_exit,
        debug_log(DLOG_ERROR, "%s generate response status fail", __FUNCTION__);
        response_code = ret);
    
    json_object_object_add(response_jso, RESPONSE_STATUS_CODE, status_jso);

    // JSON对象转换为字符串返回
    generate_redfish_resp_data_from_jso(response_jso, response);
    goto normal_exit;

error_exit:
    (void)generate_error_response(response_code, request_info, message_array_jso, response);

normal_exit:
    if (resource_jso != NULL) {
        json_object_put(resource_jso);
    }
    if (response_jso != NULL) {
        json_object_put(response_jso);
    }
    json_object_put(message_array_jso);

    return VOS_OK; // 可以正常生成相应错误消息，也属于处理OK
}



LOCAL void _delete_odata_type_prop(json_object *msg_array)
{
    int i;
    int length;
    json_object *object_iter = NULL;

    return_do_info_if_fail(json_type_array == json_object_get_type(msg_array),
        debug_log(DLOG_ERROR, "object type is not array"));

    length = json_object_array_length(msg_array);

    for (i = 0; i < length; i++) {
        object_iter = json_object_array_get_idx(msg_array, i);
        json_object_object_del(object_iter, ODATA_TYPE_KEY);
    }

    return;
}

LOCAL gint32 get_delete_property_from_message(json_object *deleted_jso, json_object *message_jso)
{
    gint32 ret = 0;
    gint32 message_length;
    gint32 i_index;
    json_object *related_jso = NULL;
    json_object *index_jso = NULL;
    json_object *tmp_jso = NULL;
    json_object *del_array = NULL;

    del_array = json_object_new_array();
    return_val_do_info_if_fail(NULL != del_array, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: del_array is null.", __FUNCTION__));

    json_object_object_add(deleted_jso, PROP_DELETED_PROPS, del_array);
    message_length = json_object_array_length(message_jso);
    for (i_index = 0; i_index < message_length; i_index++) {
        index_jso = json_object_array_get_idx(message_jso, i_index);
        ret = json_object_object_get_ex(index_jso, PROP_RELATED_PROPS, &related_jso);
        if (!ret) {
            debug_log(DLOG_ERROR, "%s: RelatedProperties not found.", __FUNCTION__);
            return VOS_ERR;
        }

        tmp_jso = json_object_array_get_idx(related_jso, 0);
        json_object_array_add(del_array, json_object_get(tmp_jso));
    }

    return VOS_OK;
}


LOCAL gint32 handle_patch(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param, HTTP_RESPONSE_DATA_S *response)
{
    gint32 ret;
    json_object *resource_jso = NULL;
    json_object *valid_props_jso = NULL;
    json_object *response_jso = NULL;
    json_object *header_array_jso = NULL;
    json_object *status_jso = NULL;
    json_object *del_jso = NULL;

    json_object *message_info_jso = NULL;
    json_object *message_array_jso = NULL;

    gint32 response_code = HTTP_OK;
    json_bool ret_json;
    json_object *redfish_jso = NULL;
    char redfish_class[MAX_CLASS_NAME_LEN] = {0};

    message_array_jso = json_object_new_array();
    
    return_val_do_info_if_fail(NULL != message_array_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: message_array_jso is null", __FUNCTION__));
    
    response_jso = json_object_new_object();
    if (i_param == NULL || request_info == NULL || response == NULL) {
        response_code = HTTP_INTERNAL_SERVER_ERROR;
        debug_log(DLOG_ERROR, "%s: response_str is null", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        (void)json_object_array_add(message_array_jso, message_info_jso);
        goto error_exit;
    }

    // 生成资源对象,用于生成ETag
    // 载入资源配置文件
    resource_jso = json_object_from_file(request_info->resource_path);
    goto_label_do_info_if_fail((NULL != resource_jso), error_exit, response_code = HTTP_NOT_FOUND;
        debug_log(DLOG_ERROR, "%s get resource from file fail", __FUNCTION__);
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_info_jso, request_info->relative_uri);
        (void)json_object_array_add(message_array_jso, message_info_jso));

    // 获取配置文件中资源相关属性
    ret_json = json_object_object_get_ex(resource_jso, REDFISH_OBJECT, &redfish_jso);
    goto_label_do_info_if_fail(ret_json, error_exit, response_code = HTTP_NOT_FOUND;
        debug_log(DLOG_ERROR, "%s get %s from file fail", __FUNCTION__, REDFISH_OBJECT);
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_info_jso, request_info->relative_uri);
        (void)json_object_array_add(message_array_jso, message_info_jso));

    // 更新资源对象中属性值
    ret = generate_get_response_body(request_info, i_param, &redfish_jso, message_array_jso);
    goto_label_do_info_if_fail((VOS_OK == ret), error_exit,
        debug_log(DLOG_ERROR, "%s update response body fail", __FUNCTION__);
        response_code = ret);

    // 非WEB发起的Redfish请求，校验ifmatch即etag请求头
    if (!i_param->is_from_webui && i_param->auth_type != AUTH_TYPE_PERMIT_DINED) { 
        ret = verify_ifmatch(request_info, redfish_jso);
        goto_label_do_info_if_fail(VOS_OK == ret, etag_fail_exit,
            debug_log(DLOG_ERROR, "%s %d:verify_ifmatch fail", __FUNCTION__, __LINE__);
            response_code = ret);
    }

    
    
    ret = _get_class_name_from_schema((const gchar *)request_info->resource_path, redfish_class, sizeof(redfish_class));
    do_info_if_true(VOS_OK != ret, debug_log(DLOG_ERROR, "%s: _get_class_name_from_schema fail", __FUNCTION__));
    ret_json = redfish_patch_format_check((const gchar *)redfish_class, (const gchar *)request_info->request_body,
        &valid_props_jso, message_array_jso);
    // 当前暂时直接从请求体中获取属性,后续格式检查合入则替换
    goto_label_do_info_if_fail(TRUE == ret_json, error_exit,
        debug_log(DLOG_ERROR, "%s: redfish_patch_format_check fail", __FUNCTION__);
        response_code = HTTP_BAD_REQUEST);
    
    
    del_jso = json_object_new_object();
    goto_label_do_info_if_fail(NULL != del_jso, error_exit, debug_log(DLOG_ERROR, "%s: del_jso is null.", __FUNCTION__);
        response_code = HTTP_INTERNAL_SERVER_ERROR; (void)json_object_put(valid_props_jso);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        (void)json_object_array_add(message_array_jso, message_info_jso));

    i_param->delete_data = del_jso;
    ret = get_delete_property_from_message(del_jso, message_array_jso);
    do_info_if_true(VOS_OK != ret, debug_log(DLOG_ERROR, "%s: get_delete_property_from_message fail", __FUNCTION__));
    

    // 调用接口执行修改属性的下层资源接口
    ret = perform_patch_operation(request_info, i_param, valid_props_jso, message_array_jso);
    (void)json_object_put(valid_props_jso);

    goto_label_do_info_if_fail((VOS_OK == ret), error_exit,
        debug_log(DLOG_ERROR, "%s %d:no prop perform patch operation successfully", __FUNCTION__, __LINE__);
        response_code = ret);

    ret = generate_get_response_body(request_info, i_param, &redfish_jso, message_array_jso);
    goto_label_do_info_if_fail((VOS_OK == ret), error_exit,
        debug_log(DLOG_ERROR, "%s update response body fail", __FUNCTION__);
        response_code = ret);

etag_fail_exit:
    // 生成响应头信息
    header_array_jso = json_object_new_array();
    
    ret = generate_patch_response_headers(i_param->is_from_webui, redfish_jso, request_info, &header_array_jso,
        message_array_jso);
    
    goto_label_do_info_if_fail((VOS_OK == ret), error_exit,
        debug_log(DLOG_ERROR, "%s generate response headers fail", __FUNCTION__);
        response_code = ret; json_object_put(header_array_jso));
    json_object_object_add(response_jso, RESPONSE_HEADERS, header_array_jso);

    
    // 成功状态下,如果不存在响应消息，生成NO_CONTENT响应码；否则生成响应体消息
    if (HTTP_OK == response_code) {
        
        json_object_object_add(response_jso, RESPONSE_BODY, json_object_get(redfish_jso));
        

        if (json_object_array_length(message_array_jso) != 0) {
            // 浮动ip允许设置为空，如果设置成功，则不需要提示格式错误
            if ((strstr(request_info->uri_lower, SET_FLOAT_IP_ADDR) != NULL) ||
                (strstr(request_info->uri_lower, SET_FLOAT_IP) != NULL)) {
                json_object_put(message_array_jso);
            } else {
                _delete_odata_type_prop(message_array_jso);
                
                json_object_object_add(redfish_jso, EXTENDED_INFO_KEY, message_array_jso);
                
            }
            message_array_jso = NULL;
        }
    }

    
    // 生成响应码、状态信息
    ret = generate_response_status(response_code, &status_jso, message_array_jso);
    
    goto_label_do_info_if_fail((VOS_OK == ret), error_exit,
        debug_log(DLOG_ERROR, "%s generate response status fail", __FUNCTION__);
        response_code = ret);
    
    json_object_object_add(response_jso, RESPONSE_STATUS_CODE, status_jso);

    // json对象转化为字符串
    generate_redfish_resp_data_from_jso(response_jso, response);

    goto normal_exit;

error_exit:
    (void)generate_error_response(response_code, request_info, message_array_jso, response);

normal_exit:

    if (NULL != resource_jso) {
        json_object_put(resource_jso);
    }
    do_if_expr(NULL != response_jso, json_object_put(response_jso));

    do_if_expr(NULL != message_array_jso, json_object_put(message_array_jso));
    if (NULL != del_jso) {
        json_object_put(del_jso);
    }

    return VOS_OK;
}


LOCAL void _get_member_rsc_path(const char *collection_rsc_path, char *member_rsc_path, guint32 path_len)
{
    errno_t safe_fun_ret;
    gchar *pos = NULL;

    return_do_info_if_fail(collection_rsc_path && member_rsc_path,
        debug_log(DLOG_ERROR, "%s %d:input param error", __FUNCTION__, __LINE__));

    safe_fun_ret = strncpy_s(member_rsc_path, path_len, collection_rsc_path, strlen(collection_rsc_path));
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    pos = g_strrstr(member_rsc_path, CONFG_FILE_SUFFIX);
    
    return_do_info_if_fail(pos, debug_log(DLOG_ERROR, "%s %d:cann't find %s", __FUNCTION__, __LINE__,
        CONFG_FILE_SUFFIX));
    

    *pos = '\0';

    safe_fun_ret = strncat_s(member_rsc_path, path_len, SCHEMA_MEMBER_SUFFIX, strlen(SCHEMA_MEMBER_SUFFIX));
    do_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

    debug_log(DLOG_DEBUG, "%s %d:member resource path", __FUNCTION__, __LINE__);

    return;
}


LOCAL gint32 handle_create(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param, HTTP_RESPONSE_DATA_S *response)
{
    gint32 ret;
    json_bool b_ret;
    gint32 response_code;
    json_object *message_array_jso = NULL;
    json_object *response_jso = NULL;
    json_object *header_array_jso = NULL;
    json_object *body_jso = NULL;
    json_object *status_jso = NULL;
    REDFISH_OPR_INFO_S *opr_fn_info = NULL;
    gchar member_id[MAX_MEM_ID_LEN] = {0};
    json_object *message_info_jso = NULL;
    
    char redfish_class[MAX_CLASS_NAME_LEN] = {0};
    json_object *body_jso_checked = NULL;
    
    
    char member_rsc_path[MAX_PATH_LENGTH] = {0};
    
    gint32 status_code;

    message_array_jso = json_object_new_array();
    
    do_val_if_expr(NULL == message_array_jso,
        debug_log(DLOG_ERROR, "%s %d: json_object_new_array fail", __FUNCTION__, __LINE__);
        return VOS_ERR);
    

    
    if (i_param == NULL || request_info == NULL || response == NULL) {
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__);
        response_code = HTTP_INTERNAL_SERVER_ERROR;
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        (void)json_object_array_add(message_array_jso, message_info_jso);
        goto error_exit;
    }

    // 获取创建操作相关函数等信息
    ret = get_opr_fn_info(request_info->relative_uri, &opr_fn_info);

    goto_label_do_info_if_fail(VOS_OK == ret, error_exit, response_code = HTTP_BAD_REQUEST;
        debug_log(DLOG_ERROR, "%s %d: get_opr_fn_info fail", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_RSC_CANNOT_CREATED, NULL, &message_info_jso);
        (void)json_object_array_add(message_array_jso, message_info_jso));
    
    

    // 系统锁定检查
    ret = redfish_system_lockdown_check(opr_fn_info->syslock_allow);
    goto_label_do_info_if_fail(VOS_OK == ret, error_exit, response_code = HTTP_BAD_REQUEST;
        debug_log(DLOG_INFO, "%s %d:create fail, system check failed", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_SYSLOCKDOWN_FORBID, NULL, &message_info_jso);
        (void)json_object_array_add(message_array_jso, message_info_jso));

    
    _get_member_rsc_path(request_info->resource_path, member_rsc_path, MAX_PATH_LENGTH);
    (void)_get_class_name_from_schema((const gchar *)member_rsc_path, redfish_class, sizeof(redfish_class));
    
    b_ret = redfish_post_format_check((const gchar *)redfish_class, (const gchar *)request_info->request_body, NULL,
        &body_jso_checked, message_array_jso);
    if (TRUE != b_ret) {
        debug_log(DLOG_ERROR, "%s: redfish_post_format_check fail", __FUNCTION__);
        response_code = HTTP_BAD_REQUEST;
        goto error_exit;
    }

    
    // 执行创建动作
    ret = opr_fn_info->do_create(i_param, body_jso_checked, member_id, MAX_MEM_ID_LEN, message_array_jso);
    json_object_put(body_jso_checked);
    debug_log(DLOG_DEBUG, "%s %d: do_create done", __FUNCTION__, __LINE__);
    goto_label_do_info_if_fail(VOS_OK == ret, error_exit, response_code = ret;
        debug_log(DLOG_ERROR, "%s %d: create operation fail", __FUNCTION__, __LINE__));

    // 生成响应头信息
    header_array_jso = json_object_new_array();
    ret = opr_fn_info->do_generate_create_headers(i_param, (gchar *)member_id, &header_array_jso, message_array_jso);
    goto_label_do_info_if_fail((VOS_OK == ret) || (HTTP_ACCEPTED == ret), error_exit,
        debug_log(DLOG_ERROR, "%s %d: do_generate_create_headers operation fail", __FUNCTION__, __LINE__);
        response_code = ret; json_object_put(header_array_jso));

    
    response_jso = json_object_new_object();
    goto_label_do_info_if_fail(NULL != response_jso, error_exit,
        debug_log(DLOG_ERROR, "%s %d: json_object_new_object fail", __FUNCTION__, __LINE__);
        response_code = HTTP_INTERNAL_SERVER_ERROR;
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        (void)json_object_array_add(message_array_jso, message_info_jso); json_object_put(header_array_jso));
    

    json_object_object_add(response_jso, RESPONSE_HEADERS, header_array_jso);

    // 生成相应响应码状态信息
    status_code = (VOS_OK == ret) ? HTTP_CREATED : HTTP_ACCEPTED;
    ret = generate_response_status(status_code, &status_jso, message_array_jso);
    goto_label_do_info_if_fail((VOS_OK == ret), error_exit,
        debug_log(DLOG_ERROR, "%s %d: generate_response_status fail", __FUNCTION__, __LINE__);
        response_code = ret);
    json_object_object_add(response_jso, RESPONSE_STATUS_CODE, status_jso);

    if (HTTP_ACCEPTED == status_code) {
        body_jso = i_param->val_jso;
    } else {
        
        (void)generate_post_create_response_body(i_param, member_id, &body_jso, message_array_jso);
        
    }

    json_object_object_add(response_jso, RESPONSE_BODY, body_jso);

    // 将JSON对象转换为字符串
    generate_redfish_resp_data_from_jso(response_jso, response);
    json_object_put(response_jso);
    json_object_put(message_array_jso);
    return VOS_OK;

error_exit:
    (void)generate_error_response(response_code, request_info, message_array_jso, response);
    json_object_put(response_jso);
    json_object_put(message_array_jso);
    return VOS_OK; // 可以正常生成相应错误消息，也属于处理OK
}


LOCAL gint32 handle_delete(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param, HTTP_RESPONSE_DATA_S *response)
{
    gint32 ret;
    gint32 response_code;
    json_object *message_array_jso = NULL;
    json_object *response_jso = NULL;
    json_object *header_array_jso = NULL;
    json_object *status_jso = NULL;
    REDFISH_OPR_INFO_S *opr_fn_info = NULL;
    json_object *message_info_jso = NULL;
    json_object *body_jso = NULL;
    gchar collection_uri[MAX_URI_LENGTH] = {0};
    gchar *uri_flag = NULL;
    gint32 status_code;

    
    message_array_jso = json_object_new_array();
    return_val_do_info_if_fail(NULL != message_array_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: message_array_jso is null", __FUNCTION__));

    response_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != response_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: response_jso is null", __FUNCTION__);
        json_object_put(message_array_jso));
    

    if (i_param == NULL || request_info == NULL || response == NULL) {
        response_code = HTTP_INTERNAL_SERVER_ERROR;
        debug_log(DLOG_ERROR, "%s: response_str is null", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        (void)json_object_array_add(message_array_jso, message_info_jso);
        goto error_exit;
    }

    (void)strncpy_s(collection_uri, MAX_URI_LENGTH, request_info->relative_uri, MAX_URI_LENGTH - 1);
    uri_flag = g_strrstr(collection_uri, "/");
    goto_label_do_info_if_fail(NULL != uri_flag, error_exit, response_code = HTTP_NOT_FOUND;
        debug_log(DLOG_ERROR, "%s %d: get_opr_fn_info fail", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_info_jso, request_info->relative_uri);
        (void)json_object_array_add(message_array_jso, message_info_jso));
    *uri_flag = '\0';
    debug_log(DLOG_DEBUG, "%s %d: collection uri: %s", __FUNCTION__, __LINE__, collection_uri);

    // 获取操作对应函数指针数组
    ret = get_opr_fn_info((const gchar *)collection_uri, &opr_fn_info);
    goto_label_do_info_if_fail(VOS_OK == ret, error_exit, response_code = HTTP_METHOD_NOT_ALLOWED;
        debug_log(DLOG_ERROR, "%s %d: get_opr_fn_info fail", __FUNCTION__, __LINE__));

    // 系统锁定检查
    ret = redfish_system_lockdown_check(opr_fn_info->syslock_allow);
    goto_label_do_info_if_fail(VOS_OK == ret, error_exit, response_code = HTTP_BAD_REQUEST;
        debug_log(DLOG_INFO, "%s %d:delete fail, system check failed", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_SYSLOCKDOWN_FORBID, NULL, &message_info_jso);
        (void)json_object_array_add(message_array_jso, message_info_jso));

    ret = opr_fn_info->do_delete(i_param, message_array_jso);
    goto_label_do_info_if_fail((VOS_OK == ret), error_exit, response_code = ret;
        debug_log(DLOG_ERROR, "%s %d:delete fail", __FUNCTION__, __LINE__));

    // 生成对应的响应头
    header_array_jso = json_object_new_array();
    ret =
        opr_fn_info->do_generate_delete_headers(i_param, &header_array_jso, request_info->uri_lower, message_array_jso);
    goto_label_do_info_if_fail((VOS_OK == ret) || (HTTP_ACCEPTED == ret), error_exit,
        response_code = HTTP_INTERNAL_SERVER_ERROR;
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        (void)json_object_array_add(message_array_jso, message_info_jso); json_object_put(header_array_jso);
        debug_log(DLOG_ERROR, "%s %d:do_generate_delete_headers fail", __FUNCTION__, __LINE__));
    json_object_object_add(response_jso, RESPONSE_HEADERS, header_array_jso);

    // 生成响应码、响应状态
    status_code = (VOS_OK == ret) ? HTTP_OK : HTTP_ACCEPTED;
    ret = generate_response_status(status_code, &status_jso, message_array_jso);
    goto_label_do_info_if_fail((VOS_OK == ret), error_exit, response_code = HTTP_INTERNAL_SERVER_ERROR;
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        (void)json_object_array_add(message_array_jso, message_info_jso);
        debug_log(DLOG_ERROR, "%s %d:generate_response_status fail", __FUNCTION__, __LINE__));
    json_object_object_add(response_jso, RESPONSE_STATUS_CODE, status_jso);

    if (HTTP_ACCEPTED == status_code) {
        body_jso = i_param->val_jso;
        json_object_object_add(response_jso, RESPONSE_BODY, body_jso);
    } else if (0 != json_object_array_length(message_array_jso)) {
        (void)generate_error_message_info(message_array_jso, &body_jso);
        json_object_object_add(response_jso, RESPONSE_BODY, body_jso);
    }

    // 转换JSON对象为对应的字符串
    generate_redfish_resp_data_from_jso(response_jso, response);
    goto normal_exit;

error_exit:

    (void)generate_error_response(response_code, request_info, message_array_jso, response);

normal_exit:
    json_object_put(response_jso);
    json_object_put(message_array_jso);
    return VOS_OK;
}


void add_monitor_location_header(json_object *rsc_jso, json_object *header_array_jso)
{
    const gchar *rsc_odata_id = NULL;
    json_object *location_jso = NULL;
    gint ret;
    errno_t rsc_tmp;
    gchar rsc_location[MAX_URI_LENGTH] = {0};

    return_do_info_if_fail((NULL != rsc_jso) && (NULL != header_array_jso),
        debug_log(DLOG_INFO, "%s failed, input param error", __FUNCTION__));

    (void)get_element_str(rsc_jso, ODATA_ID, &rsc_odata_id);
    return_do_info_if_fail((NULL != rsc_odata_id),
        debug_log(DLOG_ERROR, "%s failed, cann't get rsc odata id", __FUNCTION__));

    location_jso = json_object_new_object();
    return_do_info_if_fail((NULL != location_jso),
        debug_log(DLOG_ERROR, "%s failed, alloc new json object failed", __FUNCTION__));
    
    rsc_tmp = strncpy_s(rsc_location, sizeof(rsc_location) - 1, rsc_odata_id, strlen(rsc_odata_id));
    return_do_info_if_fail(rsc_tmp == EOK, json_object_put(location_jso);
        debug_log(DLOG_ERROR, "%s strncpy_s fail.", __FUNCTION__));
    rsc_tmp = EOK;

    rsc_tmp = strncat_s(rsc_location, sizeof(rsc_location) - 1, "/Monitor", strlen("/Monitor"));
    return_do_info_if_fail(rsc_tmp == EOK, json_object_put(location_jso);
        debug_log(DLOG_ERROR, "%s strncat_s fail.", __FUNCTION__));

    json_object_object_add(location_jso, RESP_HEADER_LOCATION, json_object_new_string((const gchar *)rsc_location));
    
    ret = json_object_array_add(header_array_jso, location_jso);
    return_do_info_if_fail(0 == ret, (void)json_object_put(location_jso);
        debug_log(DLOG_ERROR, "%s failed, add object to array jso failed", __FUNCTION__));

    return;
}


LOCAL gint32 _parse_action_retvalue(gint32 rf_ret, const gchar *action_name, json_object **message_jso)
{
    gint32 ret;

    switch (rf_ret) {
        case RF_OK:
            ret = VOS_OK;
            break;

        case RF_ACTION_NOT_SUPPORTED:
            ret = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_ACT_NOT_SUPPORTED, NULL, message_jso, action_name);
            break;

        default:
            ret = HTTP_INTERNAL_SERVER_ERROR;
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_jso);
            break;
    }

    return ret;
}


LOCAL gint32 check_action_name(REQUEST_INFO_S *request_info, json_object *message_array_jso, PROVIDER_PARAS_S *i_param)
{
    
    gint32 rf_ret;
    gint32 ret;
    gint32 array_add_ret;

    json_object *resource_jso = NULL;
    json_object *message_info_jso = NULL;
    json_object *tmp1_jso = NULL;

    const gchar *flag = NULL;

    
    resource_jso = json_object_from_file(request_info->resource_path);
    return_val_do_info_if_expr(NULL == resource_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: load json file fail", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(message_array_jso, message_info_jso));

    (void)json_object_object_get_ex(resource_jso, REDFISH_OBJECT, &tmp1_jso);
    return_val_do_info_if_expr(NULL == tmp1_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get json obj:%s failed", __FUNCTION__, REDFISH_OBJECT);
        json_object_put(resource_jso); (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(message_array_jso, message_info_jso));

    rf_ret = rf_resolve_action_name_from_rsc_jso(tmp1_jso, request_info->action_name, request_info->action_name,
        MAX_ACTION_NAME_LEN);

    
    flag = g_str_case_rstr(request_info->relative_uri, ACTION_SEGMENT);
    if (NULL == flag) {
        rf_ret = RF_FAILED;
    } else {
        flag += strlen(ACTION_SEGMENT);
    }

    (void)json_object_put(resource_jso);

    ret = _parse_action_retvalue(rf_ret, flag, &message_info_jso);
    if (NULL != message_info_jso) {
        array_add_ret = json_object_array_add(message_array_jso, message_info_jso);
        do_val_if_fail(0 == array_add_ret, (void)json_object_put(message_info_jso));
    }

    return ret;
    
}


LOCAL gint32 handle_action(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param, HTTP_RESPONSE_DATA_S *response)
{
    gint32 ret;
    RESOURCE_PROVIDER_S *resource_provider = NULL;
    PROPERTY_PROVIDER_S *provider = NULL;
    json_object *message_info_jso = NULL;
    json_object *message_array_jso = NULL;
    json_object *header_array_jso = NULL;
    json_object *response_jso = NULL;
    json_object *status_jso = NULL;
    json_object *body_jso = NULL;
    json_object *val_jso = NULL;
    guint32 count;
    guint32 array_index;
    gint32 response_code;
    json_bool b_ret;
    char redfish_class[MAX_CLASS_NAME_LEN] = {0};
    property_handler_t pfn_action;
    json_object *task_jso = NULL;

    message_array_jso = json_object_new_array();
    if (message_array_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: new json object array failed", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    response_jso = json_object_new_object();
    if (i_param == NULL || request_info == NULL || response == NULL || response_jso == NULL) {
        response_code = HTTP_INTERNAL_SERVER_ERROR;
        debug_log(DLOG_ERROR, "%s: invalid parametes or new json object failed", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        (void)json_object_array_add(message_array_jso, message_info_jso);
        goto error_exit;
    }

    
    ret = check_action_name(request_info, message_array_jso, i_param);
    goto_label_do_info_if_fail(VOS_OK == ret, error_exit, (response_code = ret));
    

    ret = get_resource_provider(request_info->uri_lower, &resource_provider);
    goto_label_do_info_if_fail(VOS_OK == ret, error_exit, response_code = HTTP_INTERNAL_SERVER_ERROR; debug_log(
        DLOG_ERROR, "%s %d: get resource provider fail, rsc uri:%s", __FUNCTION__, __LINE__, request_info->uri_lower);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        (void)json_object_array_add(message_array_jso, message_info_jso));

    ret = resource_provider->resource_provider_fn(i_param, &provider, &count);
    goto_label_do_info_if_fail(VOS_OK == ret, error_exit, response_code = HTTP_NOT_FOUND;
        debug_log(DLOG_ERROR, "%s %d: resource_provider_fn fail", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_info_jso, i_param->uri);
        (void)json_object_array_add(message_array_jso, message_info_jso));

    pfn_action = find_property_provider(provider, count, request_info->action_name, &array_index);
    goto_label_do_info_if_fail(NULL != pfn_action, error_exit, response_code = HTTP_BAD_REQUEST; debug_log(DLOG_ERROR,
        "%s %d: no action found, action name is %s", __FUNCTION__, __LINE__, request_info->action_name);
        (void)create_message_info(MSGID_ACT_NOT_SUPPORTED, NULL, &message_info_jso,
        (const gchar *)request_info->action_name);
        (void)json_object_array_add(message_array_jso, message_info_jso));

    (void)_get_class_name_from_schema((const gchar *)request_info->resource_path, redfish_class, sizeof(redfish_class));
    b_ret = redfish_post_format_check(redfish_class, request_info->request_body,
        (const char *)request_info->action_name, &val_jso, message_array_jso);
    debug_log(DLOG_DEBUG, "[%s]  redfish_post_format_check: %d", __FUNCTION__, b_ret);

    if (TRUE != b_ret) {
        response_code = HTTP_BAD_REQUEST;
        goto error_exit;
    }

    ret = HTTP_BAD_REQUEST;

    // 系统锁定检查，检查action是否能执行
    ret = redfish_system_lockdown_check(provider[array_index].syslock_allow);
    goto_label_do_info_if_fail(VOS_OK == ret, error_exit, response_code = HTTP_BAD_REQUEST; debug_log(DLOG_INFO,
        "%s %d:action fail, system check failed: %s", __FUNCTION__, __LINE__, provider[array_index].property_name);
        (void)create_message_info(MSGID_SYSLOCKDOWN_FORBID, NULL, &message_info_jso);
        (void)json_object_array_add(message_array_jso, message_info_jso));

    // 填充入参, 不填充 权限满足标识 变量,方法统一采用uip层控制权限
    i_param->val_jso = val_jso;
    i_param->index = array_index;
    debug_log(DLOG_DEBUG, "[%s] run action of \"%s\"", __FUNCTION__, request_info->action_name);

    
    ret = generate_action_response_message(pfn_action, i_param, &task_jso, &message_array_jso);
    
    goto_label_do_info_if_fail((HTTP_OK == ret) || (HTTP_ACCEPTED == ret), error_exit, response_code = ret;
        debug_log(DLOG_ERROR, "%s:action fail, ret:%d", __FUNCTION__, ret); json_object_put(val_jso));

    json_object_put(val_jso);
    // 生成响应码、响应状态
    ret = generate_response_status(ret, &status_jso, message_array_jso);
    goto_label_do_info_if_fail((VOS_OK == ret), error_exit, response_code = HTTP_INTERNAL_SERVER_ERROR;
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        (void)json_object_array_add(message_array_jso, message_info_jso));
    json_object_object_add(response_jso, RESPONSE_STATUS_CODE, status_jso);

    // 生成对应的响应头
    header_array_jso = json_object_new_array();
    ret = generate_common_response_headers(&header_array_jso, request_info->relative_uri, message_array_jso);
    goto_label_do_info_if_fail((VOS_OK == ret), error_exit, response_code = HTTP_INTERNAL_SERVER_ERROR;
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        (void)json_object_array_add(message_array_jso, message_info_jso); json_object_put(header_array_jso));

    
    (void)_add_custom_response_headers(header_array_jso, i_param->custom_header);
    do_if_expr((NULL != i_param->custom_header), json_object_put(i_param->custom_header);
        (i_param->custom_header = NULL));
    

    json_object_object_add(response_jso, RESPONSE_HEADERS, header_array_jso);

    
    // 生成SUCCESS消息体
    if ((NULL == task_jso) && (0 == json_object_array_length(message_array_jso))) {
        (void)create_message_info(MSGID_SUCCESS, NULL, &message_info_jso);
        (void)json_object_array_add(message_array_jso, message_info_jso);
    } else {
        
        json_object_object_add(response_jso, RESPONSE_BODY, task_jso);
        add_monitor_location_header(task_jso, header_array_jso);
    }

    
    if (0 != json_object_array_length(message_array_jso)) {
        (void)generate_error_message_info(message_array_jso, &body_jso);
        json_object_object_add(response_jso, RESPONSE_BODY, body_jso);
    }

    // 转换JSON对象为对应的字符串
    generate_redfish_resp_data_from_jso(response_jso, response);
    goto normal_exit;
error_exit:

    (void)generate_error_response(response_code, request_info, message_array_jso, response);
normal_exit:
    json_object_put(message_array_jso);
    json_object_put(response_jso);
    return VOS_OK;
}


LOCAL gint32 generate_action_response_message(property_handler_t pfn_action, PROVIDER_PARAS_S *i_paras,
    json_object **task_jso, json_object **message_array_jso)
{
    gint32 ret;
    json_object *message_info_jso = NULL;
    gint32 msg_index = 0;
    gint32 array_len = 0;

    return_val_do_info_if_fail(
        (NULL != pfn_action && NULL != message_array_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    ret = pfn_action(i_paras, &message_info_jso, task_jso);
    
    // 添加返回值判断，修改失败记录在无效属性对象中
    if (NULL != message_info_jso) {
        if (json_type_array == json_object_get_type(message_info_jso)) {
            array_len = json_object_array_length(message_info_jso);

            for (msg_index = 0; msg_index < array_len; msg_index++) {
                (void)json_object_array_add(*message_array_jso,
                    json_object_get(json_object_array_get_idx(message_info_jso, msg_index)));
            }

            json_object_put(message_info_jso);
        } else {
            (void)json_object_array_add(*message_array_jso, message_info_jso);
        }
    }

    
    return ret;
}


LOCAL gint32 is_free_of_authentication(const gchar *uri, METHOD_TYPE_E method_type)
{
    gint32 array_index;
    gsize len;

    return_val_do_info_if_fail((NULL != uri), VOS_ERR,
        debug_log(DLOG_ERROR, "%s %d: is_free_of_authentication input error", __FUNCTION__, __LINE__));

    // 从免鉴权uri列表中匹配当前url,匹配时忽略大小写
    for (array_index = 0; E_METHOD_NULL != g_authenticated_free_uri[array_index].method_type; array_index++) {
        len = (strlen(g_authenticated_free_uri[array_index].uri) >= strlen(uri)) ?
            (strlen(g_authenticated_free_uri[array_index].uri)) :
            (strlen(uri));

        if (g_authenticated_free_uri[array_index].method_type == E_METHOD_ALL) {
            if (0 == g_ascii_strncasecmp(g_authenticated_free_uri[array_index].uri, uri, len)) {
                return VOS_OK;
            }
        } else {
            if ((0 == g_ascii_strncasecmp(g_authenticated_free_uri[array_index].uri, uri, len)) &&
                (g_authenticated_free_uri[array_index].method_type == method_type)) {
                return VOS_OK;
            }
        }
    }

    debug_log(DLOG_DEBUG, "%s %d: is_free_of_authentication verify %s fail", __FUNCTION__, __LINE__, uri);
    return VOS_ERR;
}


LOCAL gint32 is_valid_query_param(gchar *param)
{
    if (0 == g_ascii_strcasecmp(param, QUERY_TOP)) {
        return VOS_OK;
    }

    
    if (0 == g_ascii_strcasecmp(param, QUERY_SKIP) || 0 == g_ascii_strcasecmp(param, QUERY_EXPAND)) {
        return VOS_OK;
    }
    

    
    if (0 == g_ascii_strncasecmp(param, COMPONENT_TYPE, strlen(COMPONENT_TYPE)) ||
        0 == g_ascii_strncasecmp(param, METRIC_TYPE, strlen(METRIC_TYPE)) ||
        0 == g_ascii_strncasecmp(param, START_TIME, strlen(START_TIME)) ||
        0 == g_ascii_strncasecmp(param, END_TIME, strlen(END_TIME)) ||
        0 == g_ascii_strncasecmp(param, SER_NUM, strlen(SER_NUM))) {
        return VOS_OK;
    }
    

    return VOS_ERR;
}


LOCAL gint32 get_query_params_string(REQUEST_INFO_S *request_info, gchar** param_str, json_object **message_info_jso)
{
    gchar *head_flag = NULL;

    head_flag = strstr(request_info->relative_uri, "?");
    if (head_flag == NULL) {
        debug_log(DLOG_ERROR, "%s: no ? found in relative uri", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_info_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    *head_flag = '\0';

    head_flag = strstr(request_info->uri_lower, "?");
    if (head_flag == NULL) {
        debug_log(DLOG_ERROR, "%s: no ? found in lower uri", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_info_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    *head_flag = '\0';
    
    *param_str = head_flag + 1;

    return HTTP_OK;
}


LOCAL gint32 get_query_params_from_uri(REQUEST_INFO_S *request_info, json_object *message_array_jso)
{
    gchar query_info[MAX_URI_LENGTH + 1] = {0};
    gchar *head_flag = NULL;
    gchar *tail_flag = NULL;
    json_object *message_info_jso = NULL;
    PARAM_INFO_S kv = { { 0 }, { 0 } };
    gchar *param_key = NULL;
    gchar *param_val = NULL;

    gint32 ret = get_query_params_string(request_info, &head_flag, &message_info_jso);
    if (ret != HTTP_OK) {
        json_object_array_add(message_array_jso, message_info_jso);
        return ret;
    }

    if (url_decode_str(head_flag, query_info, MAX_URI_LENGTH) != RET_OK) {
        
        debug_log(DLOG_ERROR, "%s: URL decode query info failed", __FUNCTION__);
        (void)create_message_info(MSGID_QUERY_NOT_SUPPORTED_RSC, NULL, &message_info_jso);
        json_object_array_add(message_array_jso, message_info_jso);
        return HTTP_BAD_REQUEST;
    }
    debug_log(DLOG_DEBUG, "%s %d: query_info: %s", __FUNCTION__, __LINE__, query_info);

    head_flag = query_info;

    while ((NULL != head_flag) && ('\0' != *head_flag)) {
        tail_flag = strstr(head_flag, "=");
        return_val_do_info_if_fail(NULL != tail_flag, HTTP_BAD_REQUEST,
            debug_log(DLOG_DEBUG, "%s %d: no = found in lower uri", __FUNCTION__, __LINE__);
            (void)create_message_info(MSGID_QUERY_PARA_FORMAT_ERR, NULL, &message_info_jso, RF_VALUE_NULL, head_flag);
            json_object_array_add(message_array_jso, message_info_jso));
        *tail_flag = '\0';
        tail_flag++;

        (void)strncpy_s(kv.key, MAX_PARAM_LEN, head_flag, MAX_PARAM_LEN - 1);
        debug_log(DLOG_DEBUG, "%s %d: query param name: %s", __FUNCTION__, __LINE__, head_flag);
        head_flag = tail_flag;

        tail_flag = strstr(head_flag, "&");
        if (tail_flag != NULL) {
            *tail_flag = '\0';
        }

        (void)strncpy_s(kv.value, MAX_PARAM_LEN, head_flag, MAX_PARAM_LEN - 1);
        debug_log(DLOG_DEBUG, "%s %d: query param value: %s", __FUNCTION__, __LINE__, kv.value);

        // 如果为有效查询参数，则记录参数及值
        if (VOS_OK == is_valid_query_param(kv.key)) {
            // 如果传入多个相同的合法查询参数则返回不支持
            return_val_do_info_if_fail(MAX_KEYS_NUM > request_info->query_param_num, HTTP_NOT_IMPLEMENTED,
                debug_log(DLOG_DEBUG, "%s %d: no = found in lower uri", __FUNCTION__, __LINE__);
                (void)create_message_info(MSGID_QUERY_NOT_SUPPORTED_RSC, NULL, &message_info_jso);
                json_object_array_add(message_array_jso, message_info_jso));
            (void)strncpy_s(request_info->query_params[request_info->query_param_num].key, MAX_PARAM_LEN, kv.key,
                MAX_PARAM_LEN - 1);
            (void)strncpy_s(request_info->query_params[request_info->query_param_num].value, MAX_PARAM_LEN, kv.value,
                MAX_PARAM_LEN - 1);
            request_info->query_param_num++;
        } else {
            // 根据规范如果以$开头的查询参数不支持,返回501,其他非$开头的查询参数忽略
            if (*kv.key == '$') {
                (void)create_message_info(MSGID_QUERY_NOT_SUPPORTED_RSC, NULL, &message_info_jso);
                json_object_array_add(message_array_jso, message_info_jso);
                return HTTP_NOT_IMPLEMENTED;
            }

            do_if_fail(request_info->uri_params,
                (request_info->uri_params = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free)));
            return_val_do_info_if_fail(request_info->uri_params, HTTP_INTERNAL_SERVER_ERROR,
                debug_log(DLOG_ERROR, "%s %d: params hash table create fail", __FUNCTION__, __LINE__);
                (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
                json_object_array_add(message_array_jso, message_info_jso));
            param_key = g_strdup(kv.key);
            return_val_do_info_if_fail(param_key, HTTP_INTERNAL_SERVER_ERROR,
                debug_log(DLOG_ERROR, "%s %d: params key dup fail", __FUNCTION__, __LINE__);
                (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
                json_object_array_add(message_array_jso, message_info_jso));
            param_val = g_strdup(kv.value);
            return_val_do_info_if_fail(param_val, HTTP_INTERNAL_SERVER_ERROR,
                debug_log(DLOG_ERROR, "%s %d: params val dup fail", __FUNCTION__, __LINE__);
                g_free(param_key); (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
                json_object_array_add(message_array_jso, message_info_jso));
            g_hash_table_insert(request_info->uri_params, param_key, param_val);
        }

        if (tail_flag == NULL) {
            break;
        }
        head_flag = ++tail_flag;
    }

    return VOS_OK;
}


LOCAL gint32 get_action_name(METHOD_TYPE_E method_type, REQUEST_INFO_S *o_request_info,
    json_object *o_message_array_jso)
{
    gint32 ret;
    gchar *flag = NULL;
    json_object *message_info_jso = NULL;
    const gchar* action_str;
    return_val_if_expr(method_type != E_METHOD_POST, VOS_OK);

    flag = g_strrstr(o_request_info->uri_lower, "/oem/huawei/actions/");
    if (flag != NULL) {
        action_str = &o_request_info->relative_uri[flag - o_request_info->uri_lower] + strlen("/oem/huawei/actions/");
    } else {
        flag = g_strrstr(o_request_info->uri_lower, "/actions/");
        if (flag == NULL) {
            return RET_OK;
        }
        action_str = &o_request_info->relative_uri[flag - o_request_info->uri_lower] + strlen("/actions/");
    }

    if (action_str == NULL || strlen(action_str) >= MAX_ACTION_NAME_LEN) {
        (void)create_message_info(MSGID_ACT_NOT_SUPPORTED, NULL, &message_info_jso, action_str);
        json_object_array_add(o_message_array_jso, message_info_jso);
        return HTTP_BAD_REQUEST;
    }

    ret = snprintf_s(o_request_info->action_name, MAX_ACTION_NAME_LEN, MAX_ACTION_NAME_LEN - 1, "%s",
        &o_request_info->relative_uri[flag - o_request_info->uri_lower + 1]);

    return_val_do_info_if_expr(ret < 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s %d: snprintf_s error", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(o_message_array_jso, message_info_jso));

    *flag = '\0';

    debug_log(DLOG_DEBUG, "[%s]: URI: %s, Action: %s", __FUNCTION__, o_request_info->uri_lower,
        o_request_info->action_name);

    return VOS_OK;
}


LOCAL void get_http_headers(json_object *request_jso, REQUEST_INFO_S *o_request_info)
{
    gint32 offset = 0;
    guint32 head_num;
    guint32 head_array_len;
    guint32 valid_head_num = 0;
    gchar *head_lower = NULL;
    json_object *iter_jso = NULL;
    json_object *headers_jso = NULL;
    gchar *session_id_str = NULL;

    return_if_expr(request_jso == NULL || o_request_info == NULL);

    // 获取请求头信息
    (void)json_object_object_get_ex(request_jso, REQUEST_HEADERS, &headers_jso);
    if (json_object_get_type(headers_jso) != json_type_array) {
        debug_log(DLOG_ERROR, "%s: json type of http request headers is not json array", __FUNCTION__);
        return;
    }

    head_array_len = json_object_array_length(headers_jso);

    for (head_num = 0; head_num < head_array_len; head_num++) {
        iter_jso = json_object_array_get_idx(headers_jso, head_num);

        continue_do_info_if_fail(NULL != iter_jso,
            debug_log(DLOG_ERROR, "json_object_array_get_idx %d fail", head_num));

        // 此处用于消除foreach编译告警"assignment discards 'const' qualifier from pointer target type"
#pragma GCC diagnostic push // require GCC 4.6
#pragma GCC diagnostic ignored "-Wcast-qual"
#pragma GCC diagnostic ignored "-Wcast-align"
        json_object_object_foreach0(iter_jso, name, val)
        {
            continue_if_expr(name == NULL);
            offset = -1;
            head_lower = NULL;
            head_lower = g_ascii_strdown(name, strlen(name));
            continue_if_expr(head_lower == NULL);

            offset = GPOINTER_TO_INT(g_hash_table_lookup(g_http_request_heard_hash, head_lower));
            free(head_lower);
            continue_if_expr(offset <= 0);
            offset -= HEAD_INDEX_OFFSET;

            (void)strncpy_s(o_request_info->headers[valid_head_num].header_title, MAX_HEADER_TITLE_LEN, name,
                MAX_HEADER_TITLE_LEN - 1);
            if (!g_ascii_strcasecmp(name, "cookie")) {
                session_id_str = strstr((gchar *)dal_json_object_get_str(val), REQ_SESSION_ID);
                if (session_id_str != NULL) {
                    (void)strncpy_s(o_request_info->headers[valid_head_num].header_content, MAX_HEADER_CONTENT_LEN,
                        session_id_str, MAX_HEADER_CONTENT_LEN - 1);
                }
            } else {
                (void)strncpy_s(o_request_info->headers[valid_head_num].header_content, MAX_HEADER_CONTENT_LEN,
                    dal_json_object_get_str(val), MAX_HEADER_CONTENT_LEN - 1);
            }

            *((HTTP_HEADER_S **)((guint8 *)&o_request_info->headers_index + offset)) =
                &o_request_info->headers[valid_head_num];
            valid_head_num++;
            return_do_info_if_expr(valid_head_num >= MAX_HEADER_NUM,
                debug_log(DLOG_ERROR, "header number extends %d.", MAX_HEADER_NUM));
        }
#pragma GCC diagnostic pop // require GCC 4.6
    }
}


LOCAL METHOD_TYPE_E request_method_to_type(const gchar *method)
{
    METHOD_TYPE_E method_type = E_METHOD_NULL;

    if (0 == g_ascii_strcasecmp(method, HTTP_GET)) {
        method_type = E_METHOD_GET;
    } else if (0 == g_ascii_strcasecmp(method, HTTP_PATCH)) {
        method_type = E_METHOD_PATCH;
    } else if (0 == g_ascii_strcasecmp(method, HTTP_POST)) {
        method_type = E_METHOD_POST;
    } else if (0 == g_ascii_strcasecmp(method, HTTP_DELETE)) {
        method_type = E_METHOD_DELETE;
    } else if (0 == g_ascii_strcasecmp(method, HTTP_CREATE)) {
        method_type = E_METHOD_CREATE;
    } else if (0 == g_ascii_strcasecmp(method, HTTP_ACTION)) {
        method_type = E_METHOD_ACTION;
    } else if (0 == g_ascii_strcasecmp(method, HTTP_PUT)) {
        method_type = E_METHOD_PUT;
    } else if (0 == g_ascii_strcasecmp(method, HTTP_ALL)) {
        method_type = E_METHOD_ALL;
    }

    return method_type;
}


LOCAL gint32 parse_request_string(const gchar *i_request_str, REQUEST_INFO_S *o_request_info,
    json_object *o_message_array_jso)
{
    errno_t safe_fun_ret = EOK;
    gint32 ret;
    gint32 lower_uri_length;
    gint32 relative_uri_length;
    json_object *request_jso = NULL;
    gchar method[MAX_METHOD_NAME_LEN] = {0};
    gchar *flag = NULL;
    json_object *message_info_jso = NULL;
    gchar *uri_lower = NULL;
    gchar uri_buf[MAX_URI_LENGTH] = {0};
    METHOD_TYPE_E method_type;

    return_val_do_info_if_fail(((NULL != i_request_str) && (NULL != o_request_info) && (NULL != o_message_array_jso)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s %d: input param error", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(o_message_array_jso, message_info_jso));

    // 转换请求字符串为JSON对象
    request_jso = json_tokener_parse(i_request_str);
    goto_label_do_info_if_fail((request_jso != NULL), error_exit,
        debug_log(DLOG_ERROR, "%s %d: parse request string fail", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(o_message_array_jso, message_info_jso); ret = HTTP_INTERNAL_SERVER_ERROR);

    // 获取URI信息
    ret = json_custom_get_elem_str(request_jso, RELATIVE_URI, o_request_info->relative_uri, MAX_URI_LENGTH);
    goto_label_do_info_if_fail(VOS_OK == ret, error_exit,
        debug_log(DLOG_ERROR, "%s %d: get relative_uri fail", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(o_message_array_jso, message_info_jso); ret = HTTP_INTERNAL_SERVER_ERROR);

    debug_log(DLOG_DEBUG, "%s %d: %s %s", __FUNCTION__, __LINE__, RELATIVE_URI, o_request_info->relative_uri);

    // 转换URI信息为小写保存
    relative_uri_length = strlen(o_request_info->relative_uri);
    uri_lower = g_ascii_strdown(o_request_info->relative_uri, relative_uri_length);
    goto_label_do_info_if_fail(NULL != uri_lower, error_exit,
        debug_log(DLOG_ERROR, "%s %d: change uri to lower fail", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(o_message_array_jso, message_info_jso); ret = HTTP_INTERNAL_SERVER_ERROR);
    lower_uri_length = strlen(uri_lower);
    (void)strncpy_s(o_request_info->uri_lower, MAX_URI_LENGTH, uri_lower, MAX_URI_LENGTH - 1);
    g_free(uri_lower);
    uri_lower = NULL;
    
    // 获取请求方法
    ret = json_custom_get_elem_str(request_jso, REQUEST_METHOD, method, MAX_METHOD_NAME_LEN);
    goto_label_do_info_if_fail(VOS_OK == ret, error_exit, debug_log(DLOG_ERROR, "get request_method fail");
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(o_message_array_jso, message_info_jso); ret = HTTP_INTERNAL_SERVER_ERROR);
    debug_log(DLOG_DEBUG, "%s %d: %s %s", __FUNCTION__, __LINE__, REQUEST_METHOD, method);

    method_type = request_method_to_type(method);

    
    ret = get_action_name(method_type, o_request_info, o_message_array_jso);
    goto_if_fail(VOS_OK == ret, error_exit);
    

    // 如果是POST操作且action name不为空，则为ACTION操作;否则为CREATE操作
    if (method_type == E_METHOD_POST) {
        if (o_request_info->action_name[0] != 0) {
            o_request_info->method_type = E_METHOD_ACTION;
            (void)strncpy_s(o_request_info->request_method, MAX_METHOD_NAME_LEN, HTTP_ACTION, strlen(HTTP_ACTION));
        } else {
            o_request_info->method_type = E_METHOD_CREATE;
            (void)strncpy_s(o_request_info->request_method, MAX_METHOD_NAME_LEN, HTTP_CREATE, strlen(HTTP_CREATE));
        }
    } else {
        o_request_info->method_type = method_type;
        (void)strncpy_s(o_request_info->request_method, MAX_METHOD_NAME_LEN, method, MAX_METHOD_NAME_LEN - 1);
    }

    // 非GET操作不支持查询参数，直接返回不支持信息
    flag = strstr(o_request_info->uri_lower, "?");
    if ((NULL != flag) && (o_request_info->method_type != E_METHOD_GET)) {
        (void)create_message_info(MSGID_QUERT_NOT_SUPPORTED, NULL, &message_info_jso);
        json_object_array_add(o_message_array_jso, message_info_jso);
        ret = HTTP_BAD_REQUEST;
        goto error_exit;
    }

    if (NULL != flag) {
        ret = get_query_params_from_uri(o_request_info, o_message_array_jso);

        goto_if_expr(VOS_OK != ret, error_exit);
    }

    // 如果以/结尾则去除/
    if (o_request_info->relative_uri[relative_uri_length - 1] == '/') {
        o_request_info->relative_uri[relative_uri_length - 1] = 0;
    }

    // 如果以/结尾则去除/
    if (o_request_info->uri_lower[lower_uri_length - 1] == '/') {
        o_request_info->uri_lower[lower_uri_length - 1] = 0;
    }

    if ((o_request_info->method_type == E_METHOD_CREATE) && (g_str_has_suffix(o_request_info->uri_lower, "/members"))) {
        lower_uri_length = strlen(o_request_info->uri_lower);
        (void)strncpy_s(uri_buf, MAX_URI_LENGTH, o_request_info->uri_lower, lower_uri_length - strlen("/members"));

        if (is_opr_fn_uri((const gchar *)uri_buf)) {
            safe_fun_ret = strncpy_s(o_request_info->uri_lower, MAX_URI_LENGTH, o_request_info->uri_lower,
                lower_uri_length - strlen("/members"));
            do_val_if_expr(safe_fun_ret != EOK,
                debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
            safe_fun_ret = strncpy_s(o_request_info->relative_uri, MAX_URI_LENGTH, o_request_info->relative_uri,
                lower_uri_length - strlen("/members"));
            do_val_if_expr(safe_fun_ret != EOK,
                debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
        }
    }

    // 获取请求客户端IP
    ret = json_custom_get_elem_str(request_jso, REQUEST_IP, o_request_info->request_ip, MAX_IPADDR_LEN);
    goto_label_do_info_if_fail(VOS_OK == ret, error_exit, debug_log(DLOG_ERROR, "parse request_ip fail");
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(o_message_array_jso, message_info_jso); ret = HTTP_INTERNAL_SERVER_ERROR);

    // 获取请求体字符串
    ret = json_custom_get_elem_str(request_jso, REQUEST_BODY, o_request_info->request_body, MAX_REQUEST_BODY_LEN);
    goto_label_do_info_if_fail(VOS_OK == ret, error_exit, debug_log(DLOG_ERROR, "parse request_body fail");
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(o_message_array_jso, message_info_jso); ret = HTTP_INTERNAL_SERVER_ERROR);

    // 获取请求头信息
    get_http_headers(request_jso, o_request_info);
error_exit:

    if (NULL != request_jso) {
        (void)json_object_put(request_jso);
        request_jso = NULL;
    }

    return ret;
}


gint32 verify_basic_auth_get_provider_param(const REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *provider_param,
    json_object **message_info_jso)
{
    gchar  header_content[MAX_HEADER_CONTENT_LEN] = {0};
    gchar *header_content_strstrip = NULL;
    guchar *authorization_content = NULL;
    gchar *authorization_content_compare = NULL;
    gchar *username = NULL;
    gchar *pass = NULL;
    gsize output_len = 0;
    gchar role_id[USER_ROLEID_MAX_LEN + 1] = {0};
    gchar real_user[SESSION_USER_NAME_MAX_LEN + 1] = {0};
    guchar user_id = 0;
    guchar pri = 0;
    errno_t str_ret;
    GSList *caller_info = NULL;

    check_fail_return_val(request_info, RF_FAILED);

    // 从请求体中解析basic auth值
    return_val_do_info_if_expr(request_info->headers_index.basic_auth == NULL, RF_FAILED,
        (void)memset_s(header_content, sizeof(header_content), 0, sizeof(header_content));
        debug_log(DLOG_DEBUG, "Get basic auth head fail."));
    (void)strncpy_s(header_content, sizeof(header_content), request_info->headers_index.basic_auth->header_content,
        strlen(request_info->headers_index.basic_auth->header_content));

    // 校验合法性校验前缀Basic
    gint32 ret = g_ascii_strncasecmp(header_content, REQ_HEADER_BASIC_CONTENT, strlen(REQ_HEADER_BASIC_CONTENT));

    
    return_val_do_info_if_expr(ret != VOS_OK, RF_FAILED,
        (void)memset_s(header_content, sizeof(header_content), 0, sizeof(header_content));
        create_message_info(MSGID_LOGIN_AUTH_FAILED, NULL, message_info_jso));
    

    // 解码获取用户名密码
    authorization_content =
        g_base64_decode((const gchar *)header_content + strlen(REQ_HEADER_BASIC_CONTENT), &output_len);
    
    return_val_do_info_if_expr(NULL == authorization_content, RF_FAILED,
        (void)memset_s(header_content, sizeof(header_content), 0, sizeof(header_content));
        create_message_info(MSGID_LOGIN_AUTH_FAILED, NULL, message_info_jso));
    

    // 将header_content 中前缀Basic置为空格，并去掉前后空格以获取传入有效的编码值
    str_ret = memset_s(header_content, sizeof(header_content), ' ', strlen(REQ_HEADER_BASIC_CONTENT));
    // 失败打印日志，不用返回，在字符比较处会处理返回
    do_val_if_expr(str_ret != EOK, debug_log(DLOG_ERROR, "%s, %d: memset_s  error.", __FUNCTION__, __LINE__));
    header_content_strstrip = g_strstrip((gchar *)header_content);

    // 将用户名密码重新编码
    authorization_content_compare =
        g_base64_encode(authorization_content, strlen((const gchar *)authorization_content));
    
    return_val_do_info_if_expr(NULL == authorization_content_compare, ret,
        (void)memset_s(authorization_content, output_len, 0, output_len);
        g_free(authorization_content);
        (void)memset_s(header_content, sizeof(header_content), 0, sizeof(header_content));
        create_message_info(MSGID_LOGIN_AUTH_FAILED, NULL, message_info_jso));
    

    // 重新对比编码值以消除解码函数1.自动过滤编码中空格2.编码后追加少于等于3个字符依然正常解码的影响
    
    return_val_do_info_if_expr(0 != g_strcmp0(header_content_strstrip, authorization_content_compare), VOS_ERR,
        (void)memset_s(header_content, sizeof(header_content), 0, sizeof(header_content));
        (void)memset_s(authorization_content, output_len, 0, output_len); (void)memset_s(authorization_content_compare,
        strlen(authorization_content_compare), 0, strlen(authorization_content_compare)); g_free(authorization_content);
        g_free(authorization_content_compare); debug_log(DLOG_ERROR, "%s: g_strcmp0 fail", __FUNCTION__);
        create_message_info(MSGID_LOGIN_AUTH_FAILED, NULL, message_info_jso));
    (void)memset_s(authorization_content_compare, strlen(authorization_content_compare), 0,
        strlen(authorization_content_compare));
    
    g_free(authorization_content_compare);

    // 获取用户名与密码
    username = strtok_r((char *)authorization_content, ":", &pass);
    
    return_val_do_info_if_expr(NULL == username || NULL == pass, RF_FAILED,
        (void)memset_s(header_content, sizeof(header_content), 0, sizeof(header_content));
        (void)memset_s(authorization_content, output_len, 0, output_len); g_free(authorization_content);
        create_message_info(MSGID_LOGIN_AUTH_FAILED, NULL, message_info_jso));
    

    // 释放内存，清空密码数据，失败记录日志，不异常处理，无影响
    (void)memset_s(header_content, sizeof(header_content), 0, sizeof(header_content));

    // 校验账户密码，并取出用户角色与用户域信息
    
    ret = verify_account_and_get_info(provider_param->is_from_webui, username, pass, request_info->request_ip,
        &provider_param->auth_type, &user_id, &pri, role_id, USER_ROLEID_MAX_LEN + 1, real_user, sizeof(real_user));
    
    if (VOS_OK == ret) {
        (void)strncpy_s(provider_param->user_name, SESSION_USER_NAME_MAX_LEN + 1, username, strlen(username));
        (void)strncpy_s(provider_param->client, SESSION_IP_LEN, request_info->request_ip,
            strlen(request_info->request_ip));
        
        (void)strncpy_s(provider_param->user_roleid, sizeof(provider_param->user_roleid), role_id,
            sizeof(provider_param->user_roleid) - 1);
        
        provider_param->user_role_privilege = pri;

        memset_s(authorization_content, output_len, 0, output_len);
        g_free(authorization_content);
        return RF_OK;
    }

    
    caller_info =
        g_slist_append(caller_info, g_variant_new_string(dal_rf_get_op_log_module_name(provider_param->is_from_webui)));
    
    caller_info = g_slist_append(caller_info, g_variant_new_string(username));
    caller_info = g_slist_append(caller_info, g_variant_new_string(request_info->request_ip));
    proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "User(%s) authentication failed\n", username);
    g_slist_free_full(caller_info, (GDestroyNotify)g_variant_unref);
    memset_s(authorization_content, output_len, 0, output_len);
    g_free(authorization_content);

    if (VOS_OK != ret) {
        switch (ret) {
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
            default:
                if (dal_is_rpc_exception(ret) == TRUE) {
                    create_message_info(MSGID_INTERNAL_ERR, NULL, message_info_jso);
                } else {
                    (void)create_message_info(MSGID_LOGIN_ATTEMPT_FAILED, NULL, message_info_jso);
                }
        }

        debug_log(DLOG_ERROR, "%s %d:  fail, ret is 0x%x", __FUNCTION__, __LINE__, ret);
    }

    return ret;
}


gint32 verify_xauthtoken_get_provider_param(const REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *provider_param)
{
    errno_t safe_fun_ret;
    gchar         header_content[MAX_HEADER_CONTENT_LEN] = {0};
    gchar *id_hash = NULL;
    gchar         session_id[SESSION_ID_LEN + 1] = {0};
    gint32 ret;

    // 从请求体中解析x-auth-token值
    return_val_do_info_if_expr((request_info->headers_index.x_auth_token == NULL), RF_FAILED,
        debug_log(DLOG_DEBUG, "Get %s from request info fail", REQ_HEADER_X_AUTH_TOKEN));
    (void)strncpy_s(header_content, sizeof(header_content), request_info->headers_index.x_auth_token->header_content,
        strlen(request_info->headers_index.x_auth_token->header_content));

    // 计算x-auth-token值的校验和SHA256加密值，用于查询对应会话
    id_hash = g_compute_checksum_for_string(G_CHECKSUM_SHA256, header_content, -1);
    return_val_do_info_if_fail((NULL != id_hash), RF_FAILED,
        clear_sensitive_info(header_content, sizeof(header_content));
        debug_log(DLOG_DEBUG, "%s %d: get %s from request info fail", __FUNCTION__, __LINE__, REQ_HEADER_X_AUTH_TOKEN));

    safe_fun_ret = strncpy_s(session_id, SESSION_ID_LEN + 1, id_hash, SESSION_ID_LEN);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    g_free(id_hash);
    clear_sensitive_info(header_content, sizeof(header_content));

    ret = get_provider_param_from_session((const gchar *)session_id, provider_param);
    clear_sensitive_info(session_id, sizeof(session_id));
    return ret;
}


LOCAL gint32 web_token_check(const REQUEST_INFO_S *request_info, const PROVIDER_PARAS_S *provider_param)
{
    gchar *token = NULL;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    GSList *caller_info = NULL;
    const gchar *token_str = NULL;
    OBJ_HANDLE obj_handle = 0;

    return_val_if_expr((request_info == NULL || provider_param == NULL), RF_FAILED);

    return_val_do_info_if_expr(request_info->headers_index.token == NULL, RF_FAILED,
        debug_log(DLOG_DEBUG, "Get token head fail."));
    token = request_info->headers_index.token->header_content;

    (void)dal_get_object_handle_nth(CLASS_SESSION, 0, &obj_handle);
    input_list = g_slist_append(input_list, g_variant_new_string(provider_param->session_id));
    caller_info = g_slist_append(caller_info, g_variant_new_string(USER_LOGIN_INTERFACE_REDFISH_STRING));
    caller_info = g_slist_append(caller_info, g_variant_new_string(provider_param->user_name));
    caller_info = g_slist_append(caller_info, g_variant_new_string(provider_param->client));

    gint32 ret = dfl_call_class_method(obj_handle, METHOD_SESSION_GETTOKEN, caller_info, input_list, &output_list);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    g_slist_free_full(caller_info, (GDestroyNotify)g_variant_unref);
    return_val_do_info_if_expr(ret != VOS_OK, RF_FAILED, debug_log(DLOG_ERROR, "Get token fail, ret = %d.", ret));
    token_str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    ret = (strcmp(token_str, token) == 0) ? RF_OK : RF_FAILED;
    uip_free_gvariant_list(output_list);

    return ret;
}

LOCAL void create_ibmc_message_for_web_reauthentication(gint32 ret, json_object **message_info)
{
    return_if_expr(NULL == message_info);
    switch (ret) {
        case USER_NO_ACCESS:
            create_message_info(MSGID_AUTH_USER_NO_ACCESS, NULL, message_info);
            break;

        case USER_PASSWORD_EXPIRED:
            create_message_info(MSGID_AUTH_USER_PSWD_EXPIRED, NULL, message_info);
            break;

        case USER_LOGIN_LIMITED:
            create_message_info(MSGID_AUTH_USER_RESTRICTED, NULL, message_info);
            break;

        case USER_IS_LOCKED:
            create_message_info(MSGID_AUTH_USER_LOCKED, NULL, message_info);
            break;

        default:
            create_message_info(MSGID_LOGIN_AUTH_FAILED, NULL, message_info);
    }
    return;
}


LOCAL gint32 url_decode_char(const gchar *url_str, gchar *decode_char)
{
    
    return vos_str2int(&url_str[1], BASE_16, decode_char, NUM_TPYE_CHAR);
}


LOCAL gint32 url_decode_str(const gchar *url_pwd, gchar *uncode_pwd, size_t len)
{
    gchar url_str[URL_CODE_CHAR_LEN + 1] = {0};
    gchar decode_char = 0;
    gint32 pwd_len = 0;
    gint32 ret;
    size_t i = 0;

    return_val_do_info_if_expr(uncode_pwd == NULL || url_pwd == NULL || len < strlen(url_pwd), VOS_ERR,
        debug_log(DLOG_ERROR, "%s: param error", __FUNCTION__));

    while (i < strlen(url_pwd)) {
        
        if (url_pwd[i] != '%') {
            uncode_pwd[pwd_len++] = url_pwd[i++];
            continue;
        }

        
        ret = strncpy_s(url_str, sizeof(url_str), &url_pwd[i], URL_CODE_CHAR_LEN);
        return_val_do_info_if_expr(ret != VOS_OK, VOS_ERR, debug_log(DLOG_ERROR, "%s: strncpy_s error", __FUNCTION__));

        
        ret = url_decode_char(url_str, &decode_char);
        return_val_do_info_if_expr(ret != VOS_OK, VOS_ERR,
            debug_log(DLOG_ERROR, "%s: url_decode_char error", __FUNCTION__));

        uncode_pwd[pwd_len++] = decode_char;
        
        i += URL_CODE_CHAR_LEN;
    }
    return VOS_OK;
}

LOCAL void check_reauthentication_uri(const REQUEST_INFO_S *request_info, gint32 *indx)
{
    gchar *pattern_lower = NULL;
    gboolean is_match;
    GRegex *regex = NULL;
    GMatchInfo *match_info = NULL;
    gint32 idx;

    return_if_expr(request_info == NULL || indx == NULL);

    // 遍历URI数组，二次校验匹配
    for (idx = 0; g_reauthentication_uri[idx].method_type != E_METHOD_NULL; idx++) {
        continue_if_expr(g_reauthentication_uri[idx].method_type != request_info->method_type);

        continue_if_expr((request_info->method_type == E_METHOD_ACTION) &&
            (strcmp(request_info->action_name, g_reauthentication_uri[idx].property)));
        pattern_lower = g_ascii_strdown(g_reauthentication_uri[idx].uri, strlen(g_reauthentication_uri[idx].uri));
        continue_if_expr(pattern_lower == NULL);

        // 正则匹配
        regex = g_regex_new((const gchar *)pattern_lower, (GRegexCompileFlags)0x0, (GRegexMatchFlags)0x0, NULL);
        is_match = g_regex_match(regex, request_info->uri_lower, (GRegexMatchFlags)0x0, &match_info);
        g_free(pattern_lower);
        do_info_if_true(match_info, g_match_info_free(match_info));
        do_info_if_true(regex, g_regex_unref(regex));

        break_if_expr((is_match == TRUE) && (g_reauthentication_uri[idx].method_type == E_METHOD_ACTION ||
            g_reauthentication_uri[idx].property == NULL ||
            strstr(request_info->request_body, g_reauthentication_uri[idx].property)));
    }
    *indx = idx;
}


LOCAL gboolean is_legal_need_clear_uri(gint32 idx)
{
    if (g_reauthentication_uri[idx].property == NULL) {
        return FALSE;
    }

    if (g_strcmp0(g_reauthentication_uri[idx].property, ACTION_TAIL_LDAP_IMPORT_CRL) == 0 ||
        g_strcmp0(g_reauthentication_uri[idx].property, ACTION_TAIL_KRB_IMPORT_KEY_TABLE) == 0 ||
        g_strcmp0(g_reauthentication_uri[idx].property, ACTION_TAIL_IMPORT_SSH_PUBLIC_KEY) == 0 ||
        g_strcmp0(g_reauthentication_uri[idx].property, ACTION_TAIL_LDAP_IMPORT_CERT) == 0) {
        return TRUE;
    }

    return FALSE;
}


LOCAL void clear_uploaded_file(const REQUEST_INFO_S *request_info, const PROVIDER_PARAS_S *provider_param, gint32 idx)
{
    gint32 ret;
    const gchar *content = NULL;
    json_object *request_body_jso = NULL;

    // 非web发送的请求，直接返回
    if (provider_param->is_from_webui == REDFISH_REQ_FROM_OTHER_CLIENT) {
        debug_log(DLOG_DEBUG, "%s: from other client, skip.", __FUNCTION__);
        return;
    }

    if (is_legal_need_clear_uri(idx) == FALSE) {
        debug_log(DLOG_DEBUG, "%s: not import file.", __FUNCTION__);
        return;
    }

    request_body_jso = json_tokener_parse(request_info->request_body);
    (void)get_element_str(request_body_jso, PROPERTY_ACCOUNT_SERVICE_ROOT_CONTENT, &content);
    if (content == NULL || content[0] != '/') {
        debug_log(DLOG_DEBUG, "%s:no content, or not start with /.", __FUNCTION__);
        goto EXIT;
    }

    ret = dal_check_real_path(content);
    if (ret != VOS_OK) {
        debug_log(DLOG_DEBUG, "%s: real_path failed.", __FUNCTION__);
        goto EXIT;
    }

    if (dal_check_file_opt_user(provider_param->auth_type, (const gchar *)provider_param->user_name, content,
        (const gchar *)provider_param->user_roleid) == FALSE) {
        debug_log(DLOG_DEBUG, "%s: dal_check_file_opt_user fail.", __FUNCTION__);
        goto EXIT;
    }

    (void)proxy_delete_file(provider_param->is_from_webui, provider_param->user_name, provider_param->client, content);
    debug_log(DLOG_DEBUG, "%s: delete succuss.", __FUNCTION__);

EXIT:
    (void)json_object_put(request_body_jso);
    return;
}


LOCAL gint32 get_reauth_key(const REQUEST_INFO_S *request_info, gchar *uncode_pwd, size_t len)
{
    json_object* request_body_json = NULL;
    json_object* value_jso = NULL;
    const char* reauth_key_str = NULL;
    gchar *reauth_key = NULL;
    gint32 ret;

    request_body_json = json_tokener_parse((const gchar*)(request_info->request_body));
    if (request_body_json == NULL) {
        debug_log(DLOG_ERROR, "%s: json_tokener_parse fail.", __FUNCTION__);
        return RET_ERR;
    }

    ret = json_object_object_get_ex(request_body_json, RF_SESSION_REAUTH_KEY, &value_jso);
    if (ret == FALSE || value_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_object_get_ex fail.", __FUNCTION__);
        goto EXIT;
    }

    
    reauth_key_str = dal_json_object_get_str(value_jso);
    if (reauth_key_str == NULL) {
        debug_log(DLOG_ERROR, "%s: dal_json_object_get_str failed", __FUNCTION__);
        goto EXIT;
    }

    reauth_key = g_strdup(reauth_key_str);
    if (reauth_key == NULL) {
        debug_log(DLOG_ERROR, "%s, g_strdup failed", __FUNCTION__);
        goto EXIT;
    }

    
    json_object_clear_string(request_body_json, RF_SESSION_REAUTH_KEY);
    json_object_object_del(request_body_json, RF_SESSION_REAUTH_KEY);

    
    ret = url_decode_str(reauth_key, uncode_pwd, len);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: decode reauth key head fail. ret = %d", __FUNCTION__, ret);
        goto EXIT;
    }
    ret = RET_OK;
EXIT:
    json_object_clear_string(request_body_json, RF_SESSION_REAUTH_KEY);
    json_object_put(request_body_json);
    if (reauth_key != NULL) {
        clear_sensitive_info(reauth_key, strlen(reauth_key));
        g_free(reauth_key);
    }
    return ret;
}


LOCAL gint32 web_reauthentication(const REQUEST_INFO_S *request_info, const PROVIDER_PARAS_S *provider_param)
{
    gint32 idx = 0;
    gint32 ret;
    guchar pri = 0;
    guchar user_id = 0;
    guchar auth_type = 0;
    gchar role_id[USER_ROLEID_MAX_LEN + 1] = {0};
    gchar real_user[SESSION_USER_NAME_MAX_LEN + 1] = {0};
    gchar reauth_key[MAX_HEADER_CONTENT_LEN] = {0};
    GSList *caller_info = NULL;
    gchar ldap_user_name[SESSION_USER_NAME_MAX_LEN + 1] = {0};
    gchar *temp = NULL;

    check_reauthentication_uri(request_info, &idx);

    return_val_if_expr(g_reauthentication_uri[idx].method_type == E_METHOD_NULL, RF_OK);

    ret = get_reauth_key(request_info, reauth_key, sizeof(reauth_key));
    if (ret != VOS_OK) {
        clear_uploaded_file(request_info, provider_param, idx);
        debug_log(DLOG_ERROR, "%s: get_reauth_key fail.", __FUNCTION__);
        return RF_FAILED;
    }

    if (provider_param->auth_type == LOG_TYPE_LOCAL) {
        
        ret = verify_local_account_and_get_info(provider_param->is_from_webui, provider_param->user_name, reauth_key,
            provider_param->client, &auth_type, &user_id, &pri, role_id, USER_ROLEID_MAX_LEN + 1, real_user,
            SESSION_USER_NAME_MAX_LEN + 1);
    } else if (provider_param->auth_type == LOG_TYPE_KRB_SERVER) {
        
        (void)strncpy_s(ldap_user_name, SESSION_USER_NAME_MAX_LEN + 1, provider_param->user_name,
            SESSION_USER_NAME_MAX_LEN);
        temp = strstr(ldap_user_name, "(KRB)");

        do_if_expr(temp != NULL, *temp = '\0'; temp = NULL);
        ret = verify_special_ldap_account_and_get_info(provider_param->is_from_webui, provider_param->auth_type,
            ldap_user_name, reauth_key, provider_param->client, &auth_type, &user_id, &pri, role_id,
            USER_ROLEID_MAX_LEN + 1, real_user, SESSION_USER_NAME_MAX_LEN + 1);
        
    } else {
        
        (void)strncpy_s(ldap_user_name, SESSION_USER_NAME_MAX_LEN + 1, provider_param->user_name,
            SESSION_USER_NAME_MAX_LEN);
        for (temp = ldap_user_name + strlen(ldap_user_name) - 1; ldap_user_name != temp; temp--) {
            break_do_info_if_expr(*temp == '@', (*temp = '\0'));
        }

        
        ret = verify_special_ldap_account_and_get_info(provider_param->is_from_webui, provider_param->auth_type,
            ldap_user_name, reauth_key, provider_param->client, &auth_type, &user_id, &pri, role_id,
            USER_ROLEID_MAX_LEN + 1, real_user, SESSION_USER_NAME_MAX_LEN + 1);
        
    }
    if (ret != VOS_OK) {
        
        caller_info = g_slist_append(caller_info,
            g_variant_new_string(dal_rf_get_op_log_module_name(provider_param->is_from_webui)));
        
        caller_info = g_slist_append(caller_info, g_variant_new_string(provider_param->user_name));
        caller_info = g_slist_append(caller_info, g_variant_new_string(request_info->request_ip));
        
        proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH,
            "Failed to %s, because the current user password is incorrect or the account is locked\n",
            g_reauthentication_uri[idx].operation_name);
        
        g_slist_free_full(caller_info, (GDestroyNotify)g_variant_unref);
        clear_uploaded_file(request_info, provider_param, idx);
    }
    clear_sensitive_info(reauth_key, sizeof(reauth_key));

    return ret;
}


LOCAL gint32 get_session_logout_type(const char *session_id)
{
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    GSList *caller_info = NULL;
    OBJ_HANDLE obj_handle;

    return_val_if_expr(session_id == NULL, VOS_ERR);

    ret = dal_get_object_handle_nth(CLASS_SESSION, 0, &obj_handle);
    return_val_if_fail(VOS_OK == ret, ret);

    // input_list中为session_id,即校验和加密值
    input_list = g_slist_append(input_list, g_variant_new_string(session_id));
    caller_info = g_slist_append(caller_info, g_variant_new_string(USER_LOGIN_INTERFACE_REDFISH_STRING));
    caller_info = g_slist_append(caller_info, g_variant_new_int32(REDFISH_REFRESH));

    // 调用user模块"GetOnlineUser"方法，判断是否存在有效会话，存在则获取会话信息
    ret = dfl_call_class_method(obj_handle, METHOD_SESSION_GETLOGOUTTYPE, caller_info, input_list, &output_list);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    g_slist_free_full(caller_info, (GDestroyNotify)g_variant_unref);
    return_val_do_info_if_expr((VOS_OK != ret), VOS_ERR,
        debug_log(DLOG_ERROR, "%s : dfl_call_class_method %s fail", __FUNCTION__, METHOD_SESSION_GETLOGOUTTYPE));

    ret = (gint32)g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 0));
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

    return ret;
}

#ifndef SECURITY_ENHANCED_COMPATIBLE_BOARD_V3
LOCAL gint32 is_free_reset_password_request(const gchar *uri, METHOD_TYPE_E method_type)
{
    gint32 idx;
    gchar *pattern_lower = NULL;
    GRegex *regex = NULL;
    gboolean is_match;
    GMatchInfo *match_info = NULL;

    for (idx = 0; g_password_reset_free_uri[idx].method_type != E_METHOD_NULL; idx++) {
        if (g_password_reset_free_uri[idx].method_type != method_type) {
            continue;
        }

        pattern_lower = g_ascii_strdown(g_password_reset_free_uri[idx].uri, strlen(g_password_reset_free_uri[idx].uri));
        if (pattern_lower == NULL) {
            continue;
        }

        regex = g_regex_new((const gchar *)pattern_lower, (GRegexCompileFlags)0x0, (GRegexMatchFlags)0x0, NULL);
        is_match = g_regex_match(regex, uri, (GRegexMatchFlags)0x0, &match_info);
        g_free(pattern_lower);
        if (match_info != NULL) {
            g_match_info_free(match_info);
        }
        if (regex != NULL) {
            g_regex_unref(regex);
        }

        if (is_match == TRUE) {
            debug_log(DLOG_DEBUG, "%s: request %s is free reset password", __FUNCTION__, uri);
            return VOS_OK;
        }
    }
    return VOS_ERR;
}

LOCAL gboolean __is_password_need_reset(const REQUEST_INFO_S *request_info, const PROVIDER_PARAS_S *provider_param)
{
    gint32 ret;
    OBJ_HANDLE user_handle = 0;
    guint8 user_id;

    if (provider_param->auth_type != 0) {
        return FALSE;
    }

    ret = dal_get_specific_object_string(CLASS_USER, PROPERTY_USER_NAME, provider_param->user_name, &user_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: invalid input user name %s, ret %d", __FUNCTION__, provider_param->user_name, ret);
        return FALSE;
    }

    (void)dal_get_property_value_byte(user_handle, PROPERTY_USER_ID, &user_id);
    ret = is_free_reset_password_request(request_info->uri_lower, request_info->method_type);
    if (ret == VOS_OK) {
        return FALSE; // 白名单中的接口不管机机接口还是人机接口，都允许操作
    }

    if (provider_param->is_from_webui) {
        return is_h2m_user_need_reset_password(user_id);
    } else {
        return is_m2m_user_login_denied(user_id, FALSE);
    }
}
#endif


LOCAL gint32 get_auth_token_from_cookie(gchar* cookie, gchar* token, gint32 token_len) 
{
    gchar* token_start_pos = NULL;
    gchar* token_end_pos = NULL;
    size_t len;
  
    token_start_pos = strstr(cookie, REQ_SESSION_ID);
    if (token_start_pos == NULL) {
        debug_log(DLOG_DEBUG, "%s: Failed to obtain the session id from the cookie!", __FUNCTION__);
        return VOS_ERR;
    }

    len = strlen(token_start_pos) - strlen(REQ_SESSION_ID);
    token_end_pos = strstr(token_start_pos, ";");
    if (token_end_pos == NULL) {
        errno_t safe_fun_ret = strncpy_s(token, token_len, token_start_pos + strlen(REQ_SESSION_ID), len);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        }
    } else {
        errno_t safe_fun_ret = strncpy_s(token, token_len, token_start_pos + strlen(REQ_SESSION_ID), 
            len - strlen(token_end_pos));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        }
    }

    return VOS_OK;
}


LOCAL gint32 verify_cookie_get_provider_param(const REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *provider_param,
    json_object **message_info_jso)
{
    errno_t safe_fun_ret;
    gint32 ret;
    gchar *tmp = NULL;
    gchar *cookie = NULL;
    guint8 mutual_state = 0;
    OBJ_HANDLE obj_handle = 0;
    gchar         session[SESSION_ID_LEN + 1] = {0};
    gchar         token[MAX_HEADER_CONTENT_LEN] = {0};

    return_val_do_info_if_expr((request_info == NULL || provider_param == NULL || message_info_jso == NULL), RF_FAILED,
        (void)create_message_info(MSGID_NO_VALID_SESSION, NULL, message_info_jso));

    // 获取cookie值
    return_val_do_info_if_expr(request_info->headers_index.cookie == NULL, RF_FAILED,
        debug_log(DLOG_DEBUG, "Get cookie head fail.");
        (void)create_message_info(MSGID_NO_VALID_SESSION, NULL, message_info_jso));
    cookie = request_info->headers_index.cookie->header_content;

    // 获取x-auth-token
    ret = get_auth_token_from_cookie(cookie, token, MAX_HEADER_CONTENT_LEN);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "Get %s from request info fail", REQ_SESSION_ID);
        (void)create_message_info(MSGID_NO_VALID_SESSION, NULL, message_info_jso);
        return RF_FAILED;
    }

    // 计算x-auth-token值的校验和SHA256加密值，用于查询对应会话
    tmp = g_compute_checksum_for_string(G_CHECKSUM_SHA256, token, -1);
    return_val_do_info_if_fail((NULL != tmp), RF_FAILED,
        debug_log(DLOG_DEBUG, "%s %d: get %s from request info fail", __FUNCTION__, __LINE__, REQ_HEADER_X_AUTH_TOKEN);
        (void)create_message_info(MSGID_NO_VALID_SESSION, NULL, message_info_jso));
    safe_fun_ret = strncpy_s(session, SESSION_ID_LEN + 1, tmp, SESSION_ID_LEN);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    g_free(tmp);
    tmp = NULL;

    // Session校验
    ret = get_provider_param_from_session((const gchar *)session, provider_param);
    clear_sensitive_info(session, sizeof(session));
    if (ret != VOS_OK) {
        debug_log(DLOG_DEBUG, "Verify session faild, ret=%d.", ret);
        switch (get_session_logout_type(token)) {
            case SESSION_KICKOUT:
                (void)create_message_info(MSGID_SESSION_KICKOUT, NULL, message_info_jso);
                break;

            case SESSION_RELOGIN:
                (void)create_message_info(MSGID_SESSION_RELOGIN, NULL, message_info_jso);
                break;

            case SESSION_KICKOUT_CHGINFO:
                (void)create_message_info(MSGID_SESSION_CHANGED, NULL, message_info_jso);
                break;

            case SESSION_TIMEOUT:
            default:
                (void)create_message_info(MSGID_SESSION_TIMEOUT, NULL, message_info_jso);
        }
        clear_sensitive_info(token, sizeof(token));
        return RF_FAILED;
    }

    if (request_info->method_type != E_METHOD_GET) {
        // 校验Token，防止CSRF攻击
        ret = web_token_check(request_info, provider_param);
        return_val_do_info_if_expr(ret != RF_OK, RF_FAILED,
            (void)create_message_info(MSGID_NO_VALID_SESSION, NULL, message_info_jso);
            debug_log(DLOG_ERROR, "Verify token fail, ret=%d.", ret);
            clear_sensitive_info(token, MAX_HEADER_CONTENT_LEN));

        (void)dfl_get_object_handle(MUTUAL_AUTH_CLASS_NAME, &obj_handle);
        (void)dal_get_property_value_byte(obj_handle, MUTUAL_AUTH_STATE, &mutual_state);
        
        // WEB界面二次认证, 开启双因素时不进行认证；SSO登录的用户不进行认证
        if (!mutual_state && provider_param->auth_type != LOG_TYPE_LDAP_KRB_SSO) {
            
            
            safe_fun_ret =
                strncpy_s(provider_param->client, SESSION_IP_LEN, request_info->request_ip, SESSION_IP_LEN - 1);
            do_if_expr(safe_fun_ret != EOK,
                debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
            
            ret = web_reauthentication(request_info, provider_param);
            
            
            return_val_do_info_if_expr(ret != RF_OK, RF_FAILED,
                create_ibmc_message_for_web_reauthentication(ret, message_info_jso);
                debug_log(DLOG_ERROR, "Reauthenticate fail, ret=%d.", ret); clear_sensitive_info(token, sizeof(token)));
            
        }
    }

    clear_sensitive_info(token, sizeof(token));
    return RF_OK;
}


LOCAL gint32 verify_accept(const REQUEST_INFO_S *request_info)
{
    gchar *header_content = NULL;
    gint32 header_length;

    return_val_do_info_if_expr(request_info->headers_index.accept == NULL, VOS_OK,
        debug_log(DLOG_DEBUG, "Get accept head fail.")); // 找不到请求头，默认通过校验
    header_content = request_info->headers_index.accept->header_content;
    header_length = strlen(header_content);

    debug_log(DLOG_DEBUG, "accept header content is %s", header_content);

    if ((0 == g_strcmp0(header_content, "*/*")) || (0 == g_strcmp0(header_content, "application/*"))) {
        return VOS_OK;
    }

    // 对于metadata，type为xml;对于其他资源，均为json
    if (g_str_has_suffix(request_info->uri_lower, METADATA_URI_PREFIX)) {
        // 此处字符串其他地方不涉及,直接使用比较直观,不定义宏
        if ((0 == g_ascii_strncasecmp(header_content, "application/xml",
            ((header_length >= (strlen("application/xml"))) ? (header_length) : (strlen("application/xml"))))) ||
            (0 == g_ascii_strncasecmp(header_content, "application/xml;charset=utf-8",
            ((header_length >= (strlen("application/xml"))) ? (header_length) : (strlen("application/xml")))))) {
            return VOS_OK;
        } else {
            return VOS_ERR;
        }
    } else {
        if ((0 == g_ascii_strncasecmp(header_content, "application/json",
            ((header_length >= (strlen("application/json"))) ? (header_length) : (strlen("application/json"))))) ||
            (0 == g_ascii_strncasecmp(header_content, "application/json;charset=utf-8",
            ((header_length >= (strlen("application/json;charset=utf-8"))) ?
            (header_length) :
            (strlen("application/json;charset=utf-8")))))) {
            return VOS_OK;
        } else {
            return VOS_ERR;
        }
    }
}


LOCAL gint32 verify_contenttype(const REQUEST_INFO_S *request_info)
{
    gchar *header_content = NULL;

    return_val_do_info_if_expr(request_info->headers_index.content_type == NULL, VOS_OK,
        debug_log(DLOG_DEBUG, "Get content type head fail.")); // 找不到请求头，默认通过校验
    header_content = request_info->headers_index.content_type->header_content;

    if ((TRUE == g_regex_match_simple(RE_APPLICATION_JSON_WITHOUT_CHARSET, header_content, (GRegexCompileFlags)0,
        (GRegexMatchFlags)0)) ||
        (TRUE == g_regex_match_simple(RE_APPLICATION_JSON_WITH_CHARSET_UTF8, header_content, (GRegexCompileFlags)0,
        (GRegexMatchFlags)0))) {
        return VOS_OK;
    } else {
        debug_log(DLOG_DEBUG, "content-type header content is %s", header_content);
        return VOS_ERR;
    }
}


LOCAL gint32 verify_odataversion(const REQUEST_INFO_S *request_info)
{
    gchar *header_content = NULL;
    gint32 ret;
    OBJ_HANDLE obj_handle;
    gchar odata_version[MAX_HEADER_CONTENT_LEN] = {0};

    return_val_do_info_if_expr(request_info->headers_index.odata_version == NULL, VOS_OK,
        debug_log(DLOG_DEBUG, "Get odata version head fail.")); // 找不到请求头，默认通过校验
    header_content = request_info->headers_index.odata_version->header_content;

    ret = dal_get_object_handle_nth(CLASS_REDFISH, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_OK,
        debug_log(DLOG_ERROR, "%s : get %s obj handle fail", __FUNCTION__, CLASS_REDFISH););

    (void)dal_get_property_value_string(obj_handle, PROPERTY_REDFISH_ODATA_VERSION, odata_version,
        MAX_HEADER_CONTENT_LEN);

    if (0 == g_strcmp0(header_content, odata_version)) {
        return VOS_OK;
    } else {
        debug_log(DLOG_DEBUG, "odata-version header content is %s", header_content);
        return VOS_ERR;
    }
}


LOCAL gint32 verify_ifmatch(const REQUEST_INFO_S *request_info, json_object *redfish_jso)
{
    gint32 ret;
    gchar header_content[MAX_HEADER_CONTENT_LEN] = {0};
    const gchar *etag_str = NULL;
    json_object *etag_jso = NULL;
    json_object *etag_head_jso = NULL;
    json_bool ret_json;

    return_val_do_info_if_expr(NULL == redfish_jso || NULL == request_info, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : input param error", __FUNCTION__));

    return_val_do_info_if_expr(request_info->headers_index.if_match == NULL, HTTP_PRECONDITION_FAILED,
        debug_log(DLOG_DEBUG, "%s : get ifmatch head fail", __FUNCTION__));
    (void)strncpy_s(header_content, sizeof(header_content), request_info->headers_index.if_match->header_content,
        strlen(request_info->headers_index.if_match->header_content));

    ret =
        generate_etag_response_header(request_info->uri_lower, redfish_jso, request_info->res_provider, &etag_head_jso);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        clear_sensitive_info(header_content, sizeof(header_content));
        debug_log(DLOG_ERROR, "%s : generate etag fail", __FUNCTION__));

    ret_json = json_object_object_get_ex(etag_head_jso, RESP_HEADER_ETAG, &etag_jso);
    return_val_do_info_if_fail(ret_json, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : get etag value fail", __FUNCTION__);
        json_object_put(etag_head_jso); clear_sensitive_info(header_content, sizeof(header_content)));

    etag_str = dal_json_object_get_str(etag_jso);
    return_val_do_info_if_fail(NULL != etag_str, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : get etag string fail", __FUNCTION__);
        json_object_put(etag_head_jso); clear_sensitive_info(header_content, sizeof(header_content)));

    if (0 == g_strcmp0(header_content, etag_str)) {
        ret = VOS_OK;
    } else {
        ret = HTTP_PRECONDITION_FAILED;
    }

    clear_sensitive_info(header_content, sizeof(header_content));
    json_object_put(etag_head_jso);
    return ret;
}


LOCAL gint32 verify_ifnonematch(const REQUEST_INFO_S *request_info, json_object *o_header_jso)
{
    gint32 ret;
    gchar header_content[MAX_HEADER_CONTENT_LEN] = {0};
    const gchar *etag_str = NULL;
    json_object *etag_jso = NULL;
    json_object *array_iter = NULL;
    json_type t;
    json_bool ret_json;
    gint32 array_len;
    gint32 i;

    return_val_do_info_if_expr(NULL == o_header_jso || NULL == request_info, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : input param error", __FUNCTION__));

    return_val_do_info_if_expr(request_info->headers_index.if_none_match == NULL, VOS_OK,
        debug_log(DLOG_DEBUG, "%s : get ifnonematch head fail", __FUNCTION__));
    (void)strncpy_s(header_content, sizeof(header_content), request_info->headers_index.if_none_match->header_content,
        strlen(request_info->headers_index.if_none_match->header_content));

    t = json_object_get_type(o_header_jso);
    return_val_do_info_if_fail(json_type_array == t, VOS_ERR,
        debug_log(DLOG_ERROR, "[%s]: o_header_jso is not json_type_array, t is %d", __FUNCTION__, t));

    array_len = json_object_array_length(o_header_jso);
    for (i = 0; i < array_len; i++) {
        array_iter = json_object_array_get_idx(o_header_jso, i);
        if (json_object_has_key(array_iter, RESP_HEADER_ETAG)) {
            ret_json = json_object_object_get_ex(array_iter, RESP_HEADER_ETAG, &etag_jso);
            return_val_do_info_if_fail(ret_json, VOS_OK,
                debug_log(DLOG_ERROR, "%s : get etag value fail", __FUNCTION__));

            etag_str = dal_json_object_get_str(etag_jso);
            return_val_do_info_if_fail(NULL != etag_str, VOS_OK,
                debug_log(DLOG_ERROR, "%s : get etag string fail", __FUNCTION__));

            break;
        }
    }

    return_val_do_info_if_fail(NULL != etag_str, VOS_OK, debug_log(DLOG_ERROR, "%s : get etag fail", __FUNCTION__));

    if (0 == g_strcmp0(header_content, etag_str)) {
        ret = HTTP_NOT_MODIFIED;
    } else {
        ret = VOS_OK;
    }

    return ret;
}


LOCAL gint32 verify_request_headers(const REQUEST_INFO_S *request_info)
{
    gint32 ret;

    ret = verify_accept(request_info);
    return_val_do_info_if_fail((VOS_OK == ret), HTTP_NOT_ACCEPTABLE,
        debug_log(DLOG_ERROR, "verify_accept failed")); // HTTP规范

    ret = verify_contenttype(request_info);
    return_val_do_info_if_fail((VOS_OK == ret), HTTP_UNSUPPORTED_MEDIA_TYPE,
        debug_log(DLOG_ERROR, "verify_contenttype failed")); // HTTP规范

    ret = verify_odataversion(request_info);
    return_val_do_info_if_fail((VOS_OK == ret), HTTP_PRECONDITION_FAILED,
        debug_log(DLOG_ERROR, "verify_odataversion failed")); // Redfish规范

    return ret;
}


LOCAL gint32 get_required_allow(const REQUEST_INFO_S *request_info, guint32 *required_allow)
{
    // 获取对应操作方法比特位表示
    if (request_info->method_type == E_METHOD_GET) {
        *required_allow = ALLOW_GET;
    } else if (request_info->method_type == E_METHOD_PATCH) {
        *required_allow = ALLOW_PATCH;
    } else if (request_info->method_type == E_METHOD_ACTION || request_info->method_type == E_METHOD_CREATE) {
        *required_allow = ALLOW_POST;
    } else if (request_info->method_type == E_METHOD_DELETE) {
        *required_allow = ALLOW_DELETE;
    } else {
        debug_log(DLOG_ERROR, "%s %d: unsupported http method", __FUNCTION__, __LINE__);
        return VOS_ERR;
    }

    return VOS_OK;
}


LOCAL gint32 verify_allow_operation(const REQUEST_INFO_S *request_info, json_object *message_array_jso)
{
    json_object *resource_jso = NULL;
    json_object *message_info_jso = NULL;
    json_object *allow_jso = NULL;
    json_bool ret_json;
    guint32 allow;
    guint32 required_allow;
    gint32 ret;

    // 对于jsonschemas odata metadata registries registrystore schemastore仅支持GET
    if (VOS_OK == is_static_rsc_uri(request_info->uri_lower)) {
        if (request_info->method_type == E_METHOD_GET) {
            return VOS_OK;
        } else {
            return HTTP_METHOD_NOT_ALLOWED;
        }
    }

    // 获取对应操作方法比特位表示
    ret = get_required_allow(request_info, &required_allow);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_NOT_IMPLEMENTED,
        debug_log(DLOG_ERROR, "%s %d: get_required_allow fail", __FUNCTION__, __LINE__));

    resource_jso = json_object_from_file(request_info->resource_path);
    return_val_do_info_if_fail(NULL != resource_jso, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s %d: get resource from file fail", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_info_jso, request_info->relative_uri);
        json_object_array_add(message_array_jso, message_info_jso));

    ret_json = json_object_object_get_ex(resource_jso, ALLOWED_OPERATION, &allow_jso);
    return_val_do_info_if_fail(ret_json, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s %d: get ALLOWED_OPERATION fail", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(message_array_jso, message_info_jso); json_object_put(resource_jso));

    allow = json_object_get_int(allow_jso);
    json_object_put(resource_jso);
    return ((required_allow & allow) > 0) ? VOS_OK : HTTP_METHOD_NOT_ALLOWED;
}


void redfish_http_init(void)
{
    INT2STR_MAP_S req_header[] = {
        {offsetof(HTTP_HEADER_INDEX_S, content_type), REQ_HEADER_CONTENT_TYPE},
        {offsetof(HTTP_HEADER_INDEX_S, odata_version), REQ_HEADER_ODATA_VERSION},
        {offsetof(HTTP_HEADER_INDEX_S, origin), REQ_HEADER_ORIGIN},
        {offsetof(HTTP_HEADER_INDEX_S, if_match), REQ_HEADER_IF_MATCH},
        {offsetof(HTTP_HEADER_INDEX_S, if_none_match), REQ_HEADER_IF_NONE_MATCH},
        {offsetof(HTTP_HEADER_INDEX_S, x_auth_token), REQ_HEADER_X_AUTH_TOKEN},
        {offsetof(HTTP_HEADER_INDEX_S, from), REQ_HEADER_FROM},
        {offsetof(HTTP_HEADER_INDEX_S, token), REQ_HEADER_TOKEN},
        {offsetof(HTTP_HEADER_INDEX_S, cookie), REQ_HEADER_COOKIE},
        {offsetof(HTTP_HEADER_INDEX_S, reauth_key), REQ_HEADER_REAUTH_KEY},
        {offsetof(HTTP_HEADER_INDEX_S, accept), REQ_HEADER_ACCEPT},
        {offsetof(HTTP_HEADER_INDEX_S, basic_auth), REQ_HEADER_BASIC_AUTH},
    };

    if (!g_http_resp_title_hash) {
        g_http_resp_title_hash = g_hash_table_new(g_direct_hash, g_direct_equal);
        g_hash_table_insert(g_http_resp_title_hash, GINT_TO_POINTER(HTTP_OK), (char *)"OK");
        g_hash_table_insert(g_http_resp_title_hash, GINT_TO_POINTER(HTTP_CREATED), (char *)"Created");
        g_hash_table_insert(g_http_resp_title_hash, GINT_TO_POINTER(HTTP_ACCEPTED), (char *)"Accepted");
        g_hash_table_insert(g_http_resp_title_hash, GINT_TO_POINTER(HTTP_NO_CONTENT), (char *)"No Content");
        g_hash_table_insert(g_http_resp_title_hash, GINT_TO_POINTER(HTTP_MOVED_PERMANENTLY),
            (char *)"Moved Permanently");
        g_hash_table_insert(g_http_resp_title_hash, GINT_TO_POINTER(HTTP_FOUND), (char *)"Found");
        g_hash_table_insert(g_http_resp_title_hash, GINT_TO_POINTER(HTTP_NOT_MODIFIED), (char *)"Not Modified");
        g_hash_table_insert(g_http_resp_title_hash, GINT_TO_POINTER(HTTP_BAD_REQUEST), (char *)"Bad Request");
        g_hash_table_insert(g_http_resp_title_hash, GINT_TO_POINTER(HTTP_UNAUTHORIZED), (char *)"Unauthorized");
        g_hash_table_insert(g_http_resp_title_hash, GINT_TO_POINTER(HTTP_FORBIDDEN), (char *)"Forbidden");
        g_hash_table_insert(g_http_resp_title_hash, GINT_TO_POINTER(HTTP_NOT_FOUND), (char *)"Not Found");
        g_hash_table_insert(g_http_resp_title_hash, GINT_TO_POINTER(HTTP_METHOD_NOT_ALLOWED),
            (char *)"Method Not Allowed");
        g_hash_table_insert(g_http_resp_title_hash, GINT_TO_POINTER(HTTP_NOT_ACCEPTABLE), (char *)"Not Acceptable");
        g_hash_table_insert(g_http_resp_title_hash, GINT_TO_POINTER(HTTP_CONFLICT), (char *)"Conflict");
        g_hash_table_insert(g_http_resp_title_hash, GINT_TO_POINTER(HTTP_GONE), (char *)"Gone");
        g_hash_table_insert(g_http_resp_title_hash, GINT_TO_POINTER(HTTP_LENGTH_REQUIRED), (char *)"Length Required");
        g_hash_table_insert(g_http_resp_title_hash, GINT_TO_POINTER(HTTP_PRECONDITION_FAILED),
            (char *)"Precondition Failed");
        g_hash_table_insert(g_http_resp_title_hash, GINT_TO_POINTER(HTTP_UNSUPPORTED_MEDIA_TYPE),
            (char *)"Unsupported Media Type");
        g_hash_table_insert(g_http_resp_title_hash, GINT_TO_POINTER(HTTP_INTERNAL_SERVER_ERROR),
            (char *)"Internal Server Error");
        g_hash_table_insert(g_http_resp_title_hash, GINT_TO_POINTER(HTTP_NOT_IMPLEMENTED), (char *)"Not Implemented");
        g_hash_table_insert(g_http_resp_title_hash, GINT_TO_POINTER(HTTP_SERVICE_UNAVAILABLE),
            (char *)"Service Unavailable");
    }

    if (!g_http_request_heard_hash) {
        g_http_request_heard_hash = g_hash_table_new_full(g_str_hash, g_str_equal, free, NULL);

        for (gint32 i = 0; i < G_N_ELEMENTS(req_header); i++) {
            gchar *head_lower = g_ascii_strdown(req_header[i].str_val, strlen(req_header[i].str_val));
            if (head_lower != NULL) {
                g_hash_table_insert(g_http_request_heard_hash, head_lower,
                    GINT_TO_POINTER(HEAD_INDEX_OFFSET + req_header[i].int_val));
            }
        }
    }

    return;
}

void free_request_info(REQUEST_INFO_S *request_info)
{
    if (request_info == NULL) {
        return;
    }

    clear_sensitive_info(request_info, sizeof(REQUEST_INFO_S));
    g_free(request_info);
}


LOCAL gint32 generate_param_jso(REQUEST_INFO_S* request_info, 
    PROVIDER_PARAS_S* provider_param, json_object* message_array_jso)
{
    json_object* param_jso = NULL;
    json_bool ret_bool;
    json_object* message_info_jso = NULL;
    gint32 i;

    provider_param->user_data = json_object_new_object(); 
    for (i = 0; i < MAX_KEYS_NUM && strlen(request_info->query_params[i].key); i++) {
        ret_bool = json_object_object_get_ex(provider_param->user_data, 
            request_info->query_params[i].key, &param_jso);
        if (ret_bool == FALSE || param_jso == NULL) {
            json_object_object_add(provider_param->user_data, request_info->query_params[i].key, 
                json_object_new_string(request_info->query_params[i].value));                         
        } else {
            (void)create_message_info(MSGID_QUERY_NOT_SUPPORTED_RSC, NULL, &message_info_jso);
            (void)json_object_array_add(message_array_jso, message_info_jso);
            debug_log(DLOG_ERROR, "%s Duplicate attribute %s", __FUNCTION__, request_info->query_params[i].key);
            json_object_put(provider_param->user_data);
            return HTTP_BAD_REQUEST;
        }
    }
    return HTTP_OK;
}


LOCAL gint32 __none_free_auth_request_proc(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *provider_param,
    json_object *message_array_jso, json_object *message_info_jso)
{
    gint32 ret;

    if (provider_param->is_from_webui) {
        ret = verify_cookie_get_provider_param(request_info, provider_param, &message_info_jso);
        if (ret != RET_OK) {
            (void)json_object_array_add(message_array_jso, message_info_jso);
            return HTTP_UNAUTHORIZED;
        }
    } else if (request_info->headers_index.x_auth_token != NULL) {
        // x-auth-token鉴权，鉴权成功获取session信息
        ret = verify_xauthtoken_get_provider_param(request_info, provider_param);
        if (ret != RET_OK) {
            (void)create_message_info(MSGID_NO_VALID_SESSION, NULL, &message_info_jso);
            (void)json_object_array_add(message_array_jso, message_info_jso);
            return HTTP_UNAUTHORIZED;
        }
    } else if (request_info->headers_index.basic_auth != NULL) {
        ret = verify_basic_auth_get_provider_param(request_info, provider_param, &message_info_jso);
        if (dal_is_rpc_exception(ret) == TRUE) {
            (void)json_object_array_add(message_array_jso, message_info_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
        }

        if (ret != RET_OK) {
            (void)json_object_array_add(message_array_jso, message_info_jso);
            return HTTP_UNAUTHORIZED;
        }
    } else {
        (void)create_message_info(MSGID_NO_VALID_SESSION, NULL, &message_info_jso);
        (void)json_object_array_add(message_array_jso, message_info_jso);
        return HTTP_UNAUTHORIZED;
    }

#ifndef SECURITY_ENHANCED_COMPATIBLE_BOARD_V3
    // 鉴权通过后检查若为首次登录场景，则返回禁止操作 HTTP_FORBIDDEN
    gboolean pswd_rst_flag = __is_password_need_reset(request_info, provider_param);
    debug_log(DLOG_DEBUG, "%s: uri %s, pswd_rst_flag=%d", __FUNCTION__, request_info->uri_lower, pswd_rst_flag);
    if (pswd_rst_flag == TRUE) {
        (void)create_message_info(MSGID_PASSWORD_NEED_RESET, NULL, &message_info_jso);
        (void)json_object_array_add(message_array_jso, message_info_jso);
        return HTTP_FORBIDDEN;
    }
#endif

    return HTTP_OK;
}


LOCAL gint32 forward_request(guint8 software_type, REQUEST_INFO_S *request_info,
    PROVIDER_PARAS_S *provider_param, HTTP_RESPONSE_DATA_S *response)
{
    gint32 ret = HANDLE_LOCAL;
    if (MGMT_SOFTWARE_TYPE_EM == software_type) {
        ret = forward_for_em(request_info, provider_param, response);
    } else if (software_type == MGMT_SOFTWARE_TYPE_PANGEA_PACIFIC_CTRL ||
        software_type == MGMT_SOFTWARE_TYPE_PANGEA_MGNT) {
        ret = forward_for_public_device(request_info, provider_param, response);
    }
    return ret;
}

/*****************************************************************************
 函 数 名  : process_request
 功能描述  : 处理请求函数
 输入参数  : const gchar* request_str, gint32 request_len
 输出参数  : gchar* response_str, gint32 response_len
 返 回 值  : VOS_ERR/VOS_OK

 修改历史      :
  1.日    期   : 2016年08月01日
    作    者   : z00352904
    修改内容   : 新生成函数

**************************************************************************** */
LOCAL gint32 process_request(const gchar *request_str, HTTP_RESPONSE_DATA_S *response)
{
    gint32 ret = VOS_OK;
    http_handler handler;
    json_object *message_info_jso = NULL;
    gint32 response_code;
    PROVIDER_PARAS_S provider_param;

    // 新建异常消息JSON对象，用于异常场景消息返回
    json_object *message_array_jso = json_object_new_array();
    if (message_array_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: new json object array failed", __FUNCTION__);
        return VOS_ERR;
    }
    // 初始化结构体变量
    
    REQUEST_INFO_S *request_info = (REQUEST_INFO_S *)g_malloc0(sizeof(REQUEST_INFO_S));
    goto_label_do_info_if_fail((NULL != request_info), error_exit, response_code = VOS_ERR;
        debug_log(DLOG_ERROR, "%s g_malloc failed", __FUNCTION__));

    
    if (request_str == NULL || response == NULL) {
        response_code = HTTP_INTERNAL_SERVER_ERROR;
        debug_log(DLOG_ERROR, "%s input param error", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        (void)json_object_array_add(message_array_jso, message_info_jso);
        goto error_exit;
    }

    // 解析请求体字符串信息，保存至结构体变量
    
    ret = parse_request_string(request_str, request_info, message_array_jso);
    
    goto_label_do_info_if_fail((VOS_OK == ret), error_exit, response_code = ret;
        debug_log(DLOG_ERROR, "%s parse_request_string fail", __FUNCTION__));
    // 判断是否需要满足条件才能访问此URI
    
    guint8 software_type = 0;
    (void)dal_get_software_type(&software_type);

    if (MGMT_SOFTWARE_TYPE_EM == software_type) {
        if (check_uri_shield_condition(request_info, response)) { // 条件满足，屏蔽此URI
            do_if_true(request_info->uri_params, g_hash_table_unref(request_info->uri_params));
            free_request_info(request_info);
            json_object_put(message_array_jso);
            return VOS_OK;
        }
    }

    

    (void)memset_s(&provider_param, sizeof(PROVIDER_PARAS_S), 0, sizeof(PROVIDER_PARAS_S));
    provider_param.uri_params = request_info->uri_params;
    if (request_info->headers_index.from != NULL &&
        0 == strcmp(request_info->headers_index.from->header_content, REQ_FROM_WEBUI)) {
        
        provider_param.is_from_webui = REDFISH_REQ_FROM_WEBUI;
        
    }

    
    if (is_free_of_authentication(request_info->relative_uri, request_info->method_type) != RET_OK) {
        ret = __none_free_auth_request_proc(request_info, &provider_param, message_array_jso, message_info_jso);
        if (ret != HTTP_OK) {
            response_code = ret;
            goto error_exit;
        }
    }

    // 其他请求头校验
    
    ret = verify_request_headers(request_info);
    
    goto_label_do_info_if_fail((VOS_OK == ret), error_exit, response_code = ret;
        debug_log(DLOG_ERROR, "%s verify_request_headers fail", __FUNCTION__));

    
    ret = forward_request(software_type, request_info, &provider_param, response);
    if (ret != HANDLE_LOCAL) {
        do_if_true(request_info->uri_params, g_hash_table_unref(request_info->uri_params));
        free_request_info(request_info);
        json_object_put(message_array_jso);
        return ret;
    }
    
    // 根据URI获取资源配置文件路径
    
    ret = get_res_info_from_uri(request_info, &provider_param, message_array_jso);
    
    goto_label_do_info_if_fail(VOS_OK == ret, error_exit, response_code = ret;
        debug_log(DLOG_ERROR, "%s %d: get_res_info_from_uri fail", __FUNCTION__, __LINE__));

    // 校验资源文件是否支持对应HTTP操作
    
    ret = verify_allow_operation(request_info, message_array_jso);
    
    goto_label_do_info_if_fail((VOS_OK == ret), error_exit, response_code = ret;
        debug_log(DLOG_ERROR, "%s verify_allow_operation fail", __FUNCTION__));

    // 找到对应HTTP处理方法
    
    handler = request_handle[request_info->method_type].handler;
    
    goto_label_do_info_if_expr((handler == NULL), error_exit, response_code = ret;
        debug_log(DLOG_ERROR, "%s find_handler fail", __FUNCTION__));

    
    // 将URI中参数填充到请求体user_data中
    if (!g_ascii_strncasecmp(request_info->uri_lower, DATA_ACQ_REPORT_PREFIX, strlen(DATA_ACQ_REPORT_PREFIX))) {
        ret = generate_param_jso(request_info, &provider_param, message_array_jso);
        if (ret != HTTP_OK) {
            response_code = HTTP_BAD_REQUEST;
            goto error_exit;
        } 
    }

    

    // 执行响应处理方法，获取响应信息
    
    ret = handler(request_info, &provider_param, response);

    
    
    do_if_true(!g_ascii_strncasecmp(request_info->uri_lower, DATA_ACQ_REPORT_PREFIX, strlen(DATA_ACQ_REPORT_PREFIX)),
        json_object_put(provider_param.user_data));
    
    

    
    rf_provider_paras_free(&provider_param);
    

    do_if_true(request_info->uri_params, g_hash_table_unref(request_info->uri_params));
    free_request_info(request_info);
    
    json_object_put(message_array_jso);
    return ret;

error_exit:

    // 错误消息场景:1、同一错误码对应固定错误消息 2、同一错误码可能对应不同错误消息 3、仅错误码，不需错误消息
    (void)generate_error_response(response_code, request_info, message_array_jso, response);

    do_if_true(request_info && request_info->uri_params, g_hash_table_unref(request_info->uri_params));
    
    free_request_info(request_info);
    

    json_object_put(message_array_jso);
    return VOS_OK;
}


LOCAL void _rf_debug_log_request(const gchar *req_str)
{
    json_object *req_jso = NULL;
    const gchar *relative_uri = NULL;
    const gchar *req_method = NULL;
    const gchar *req_body = NULL;
    guint32 req_size;
    guint32 body_size;
    gchar               print_buf[MAX_BUFF_SIZE] = {0};
    
    gint32 ret;

    return_if_expr(get_dbg_log_outlvl() < DLOG_INFO);

    return_do_info_if_fail((NULL != req_str), debug_log(DLOG_INFO, "redfish process request: no information found."));

    
    req_jso = json_tokener_parse(req_str);

    req_size = strlen(req_str);
    (void)get_element_str(req_jso, RELATIVE_URI, &relative_uri);
    (void)get_element_str(req_jso, REQUEST_METHOD, &req_method);
    (void)get_element_str(req_jso, REQUEST_BODY, &req_body);

    body_size = (NULL == req_body) ? 0 : strlen(req_body);

    ret = snprintf_s(print_buf, sizeof(print_buf), sizeof(print_buf) - 1,
        "redfish process request: %s %s, req size [%u], body size [%u].", (NULL == req_method) ? "Unknown" : req_method,
        (NULL == relative_uri) ? "Unknown" : relative_uri, req_size, body_size);
    json_object_put(req_jso);

    return_do_info_if_fail(ret > 0,
        debug_log(DLOG_INFO, "redfish process request: format redfish request info failed."));

    debug_log(DLOG_INFO, "%s", print_buf);

    return;
}


LOCAL void _rf_debug_log_response(HTTP_RESPONSE_DATA_S *response)
{
    gchar print_buf[MAX_BUFF_SIZE] = {0};

    return_if_expr(DLOG_INFO > get_dbg_log_outlvl());

    if (response == NULL) {
        debug_log(DLOG_INFO, "%s: redfish process request: no information found", __FUNCTION__);
        return;
    }

    
    guint32 resp_body_size = (response->resp_body == NULL) ? 0 : strlen(response->resp_body);
    const gchar *resp_status = (response->resp_status == NULL) ? "" : response->resp_status;
    gint32 ret = snprintf_s(print_buf, sizeof(print_buf), sizeof(print_buf) - 1,
        "redfish process request: status code [%s], response body size [%u].", resp_status, resp_body_size);
    if (ret <= 0) {
        debug_log(DLOG_INFO, "redfish process request: format redfish response info failed");
    }

    debug_log(DLOG_INFO, "%s", print_buf);
    return;
}


gint32 method_process_http_request(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret;
    GVariant *request = NULL;
    gsize request_len;
    const gchar *request_str = NULL;
    HTTP_RESPONSE_DATA_S response;

    return_val_do_info_if_fail(NULL != input_list, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: input_list=NULL.", __FUNCTION__));
    
    return_val_do_info_if_fail(NULL != output_list, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: output_list = NULL.", __FUNCTION__));
    

    request = (GVariant *)g_slist_nth_data(input_list, 0);
    return_val_do_info_if_fail(NULL != request, VOS_ERR, debug_log(DLOG_ERROR, "%s: request=NULL.", __FUNCTION__));

    request_str = g_variant_get_string(request, &request_len);

    return_val_do_info_if_fail(NULL != request_str, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: request_str=NULL.", __FUNCTION__));

    
    _rf_debug_log_request(request_str);
    

    init_http_response_data(&response);
    ret = process_request(request_str, &response);

    
    _rf_debug_log_response(&response);
    

    if (ret != VOS_OK || (response.resp_status == NULL) || (strlen(response.resp_status) == 0)) {
        debug_log(DLOG_ERROR, "%s: process request failed or response status is NULL, ret:%d", __FUNCTION__, ret);
        destory_http_response_data(&response);
        return VOS_ERR;
    }

    *output_list =
        g_slist_append(*output_list, g_variant_new_string((response.resp_body == NULL) ? "" : response.resp_body));
    *output_list =
        g_slist_append(*output_list, g_variant_new_string((response.resp_head == NULL) ? "" : response.resp_head));
    *output_list =
        g_slist_append(*output_list, g_variant_new_string((response.resp_status == NULL) ? "" : response.resp_status));
    destory_http_response_data(&response);
    return VOS_OK;
}


gint32 redfish_system_lockdown_check(const guint8 allow)
{
    guint8 lockdown_support = 0;
    gint32 ret;

    if (DISABLE == g_sys_lockdown_state) {
        return VOS_OK;
    }

    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROPERTY_PME_SERVICECONFIG_SYSTEM_LOCKDOWN_SUPPORT,
        &lockdown_support);
    return_val_do_info_if_fail(VOS_ERR != ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get system lockdown support failed\n", __FUNCTION__));
    return_val_if_fail(PME_SERVICE_SUPPORT == lockdown_support, VOS_OK);

    return allow == SYS_LOCKDOWN_FORBID ? VOS_ERR : VOS_OK;
}


gint32 redfish_system_lockdown_state_changed(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value)
{
    g_sys_lockdown_state = g_variant_get_byte(property_value);
    debug_log(DLOG_INFO, "%s: system lockdown state changed to %d\n", __FUNCTION__, g_sys_lockdown_state);
    return VOS_OK;
}


void redfish_system_lockdown_init(void)
{
    gint32 ret;

    ret = dal_get_func_ability(CLASS_SECURITY_ENHANCE, PROPERTY_SECURITY_SYSTEMLOCKDOWNSTATE, &g_sys_lockdown_state);
    do_if_expr(VOS_OK != ret, debug_log(DLOG_ERROR, "%s: get system lockdown status failed\n", __FUNCTION__));

    return;
}
