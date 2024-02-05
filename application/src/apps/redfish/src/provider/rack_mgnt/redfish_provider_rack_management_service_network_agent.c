















#include "redfish_provider.h"

LOCAL gint32 act_set_device_ipinfo(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_setdevipinfo_actioninfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

#define MANAGERS_URL "/redfish/v1/Managers"

#ifndef RESP_HEADER_CONTENT_TYPE
#define RESP_HEADER_CONTENT_TYPE "Content-Type"
#endif

#ifndef RESP_HEADER_ETAG
#define RESP_HEADER_ETAG "ETag"
#endif

LOCAL PROPERTY_PROVIDER_S  g_rack_mgnt_network_agent_provider[] = {
    {
        RFPROP_SET_DEV_IPINFO_ACTION,
        MAP_PROPERTY_NULL,
        MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        NULL,
        NULL,
        act_set_device_ipinfo,
        ETAG_FLAG_ENABLE
    }
};

LOCAL PROPERTY_PROVIDER_S  g_rack_mgnt_set_dev_ipinfo_actioninfo_provider[] = {
    {
        RFPROP_ODATA_ID,
        MAP_PROPERTY_NULL,
        MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_setdevipinfo_actioninfo_odata_id,
        NULL,
        NULL,
        ETAG_FLAG_ENABLE
    }
};


LOCAL void _set_op_log(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_dev)
{
    GSList *caller_info = NULL;
    gchar pro_sn[MAX_PRO_SN_LEN + 1] = {0};
    caller_info = g_slist_append(caller_info, g_variant_new_string(USER_LOGIN_INTERFACE_REDFISH_STRING));
    caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->user_name)),
    caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->client));

    (void)dal_get_property_value_string(obj_dev, PROPERTY_DEVICE_INFO_SER_NUM, pro_sn, sizeof(pro_sn));
    proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH,
        "Set the device (SN:%s) IP information through the iRM agent successfully.", pro_sn);
    uip_free_gvariant_list(caller_info);
    return;
}


LOCAL gint32 _find_dev_obj(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE *obj_dev, json_object **o_message_jso)
{
    guint32 ret;
    const gchar *ser_num = NULL;
    json_object *json_obj_ser_num = NULL;
    OBJ_HANDLE obj_dev_tmp = 0;
    guint8 discovered = DEVICE_OFF_LINE;

    return_val_do_info_if_expr((NULL == i_paras) || (NULL == obj_dev) || (NULL == o_message_jso),
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s:Input parameters is illegal!", __FUNCTION__));

    ret = json_object_object_get_ex(i_paras->val_jso, RFPROP_DEVICES_SN, &json_obj_ser_num);
    return_val_do_info_if_expr(FALSE == ret, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_ACT_PARA_MISSING, NULL, o_message_jso, RFPROP_SET_DEV_IPINFO_ACTION,
        RFPROP_DEVICES_SN);
        debug_log(DLOG_ERROR, "%s:The object: %s was not found!", __FUNCTION__, RFPROP_DEVICES_SN));

    ser_num = dal_json_object_get_str(json_obj_ser_num);

    ret = dal_get_specific_object_string(CLASS_DEVICE_INFO, PROPERTY_DEVICE_INFO_SER_NUM, ser_num, &obj_dev_tmp);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_INVALID_ACTION_PARAM_VALUE, NULL, o_message_jso, ser_num, RFPROP_DEVICES_SN);
        debug_log(DLOG_ERROR, "%s:No object with giving ser_num %s was found!", __FUNCTION__, ser_num));

    (void)dal_get_property_value_byte(obj_dev_tmp, PROPERTY_DEVICE_INFO_DISCOVERED, &discovered);
    return_val_do_info_if_fail(DEVICE_ON_LINE == discovered, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_INVALID_ACTION_PARAM_VALUE, NULL, o_message_jso, ser_num, RFPROP_DEVICES_SN);
        debug_log(DLOG_ERROR, "%s:The device ser_num[%s] was offline!", __FUNCTION__, ser_num));

    debug_log(DLOG_DEBUG, "%s:The device ser_num[%s] was found,name is %s!", __FUNCTION__, ser_num,
        dfl_get_object_name(obj_dev_tmp));

    *obj_dev = obj_dev_tmp;
    return VOS_OK;
}


LOCAL guint32 _process_rest_request_rsp(guint32 rsp_code, gchar *request_url, json_object *resp_body_json_obj,
    json_object **o_message_jso)
{
    guint32 ret = VOS_OK;
    json_object *resp_error_obj = NULL;
    json_object *resp_extend_obj = NULL;
    return_val_do_info_if_expr((NULL == request_url) || (NULL == o_message_jso) || (NULL == resp_body_json_obj),
        VOS_ERR, debug_log(DLOG_ERROR, "%s:Input parameters is NULL!", __FUNCTION__));

    switch (rsp_code) {
        case VOS_OK:
        case RFERR_SUCCESS:
            
            break_do_info_if_expr(TRUE ==
                json_object_object_get_ex(resp_body_json_obj, EXTENDED_INFO_KEY, &resp_extend_obj),
                *o_message_jso = json_object_get(resp_extend_obj);
                ret = HTTP_BAD_REQUEST);
            ret = VOS_OK;
            break;

        
        default:
            if (json_object_object_get_ex(resp_body_json_obj, ERROR_KEY, &resp_error_obj)) {
                break_do_info_if_expr(json_object_object_get_ex(resp_error_obj, EXTENDED_INFO_KEY, &resp_extend_obj),
                    *o_message_jso = json_object_get(resp_extend_obj);
                    ret = HTTP_BAD_REQUEST);
            }

            debug_log(DLOG_ERROR, "%s:resp_body_json_obj error, rsp code is %d", __FUNCTION__, rsp_code);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            ret = HTTP_INTERNAL_SERVER_ERROR;
    }

    return ret;
}


LOCAL gint32 _send_get_request_method(PROVIDER_PARAS_S *i_paras, gchar *eth_url, json_object *header_json_obj,
    OBJ_HANDLE obj_dev, GSList **output_list)
{
    GSList *input_list = NULL;
    gint32 ret;

    return_val_do_info_if_expr((NULL == header_json_obj) || (NULL == i_paras) || (NULL == eth_url), VOS_ERR,
        debug_log(DLOG_ERROR, "%s:Input parameters is NULL!", __FUNCTION__));
    // 调用方法
    
    input_list = g_slist_append(input_list, g_variant_new_string(HTTP_GET));
    input_list = g_slist_append(input_list, g_variant_new_string(eth_url));
    input_list = g_slist_append(input_list, g_variant_new_string(dal_json_object_to_json_string(header_json_obj)));
    input_list = g_slist_append(input_list, g_variant_new_string("{}"));

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_dev,
        CLASS_DEVICE_INFO, METHOD_HANDLE_REST_REQUEST, AUTH_ENABLE, i_paras->user_role_privilege, input_list,
        output_list);
    uip_free_gvariant_list(input_list);
    return ret;
}

LOCAL gint32 _generate_basic_author_header(PROVIDER_PARAS_S *i_paras, json_object **header_json_obj)
{
    gchar       header_auth_str[MAX_HEADER_CONTENT_LEN] = {0};
    gchar *authorization_content = NULL;
    json_object *user_name_obj = NULL;
    json_object *password_obj = NULL;
    guint32 ret;

    return_val_do_info_if_expr((NULL == header_json_obj) || (NULL == i_paras), VOS_ERR,
        debug_log(DLOG_ERROR, "%s:Input parameters is illegal!", __FUNCTION__));
    // 获取用户名
    ret = json_object_object_get_ex(i_paras->val_jso, RFPROP_DEVICE_USER_NAME, &user_name_obj);
    return_val_do_info_if_expr(FALSE == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s:User_name was not found!", __FUNCTION__));

    // 获取密码
    ret = json_object_object_get_ex(i_paras->val_jso, RFPROP_DEVICE_PASSWORD, &password_obj);
    return_val_do_info_if_expr(FALSE == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s:Password was not found!", __FUNCTION__));

    
    json_object_object_add(*header_json_obj, RESP_HEADER_CONTENT_TYPE,
        json_object_new_string("application/json;charset=utf-8"));

    
    ret = snprintf_s(header_auth_str, sizeof(header_auth_str), sizeof(header_auth_str) - 1, "%s:%s",
        dal_json_object_get_str(user_name_obj), dal_json_object_get_str(password_obj));
    return_val_do_info_if_fail(ret > 0, VOS_ERR,
        debug_log(DLOG_ERROR, "%s:Header_auth_str generate error,ret is %d.", __FUNCTION__, ret));

    authorization_content = g_base64_encode((guchar *)header_auth_str, strlen(header_auth_str));

    (void)memset_s(header_auth_str, sizeof(header_auth_str), 0, sizeof(header_auth_str));

    return_val_do_info_if_expr(NULL == authorization_content, VOS_ERR,
        debug_log(DLOG_ERROR, "%s:Header_auth_str is error!", __FUNCTION__));

    ret = snprintf_s(header_auth_str, sizeof(header_auth_str), sizeof(header_auth_str) - 1, "Basic %s",
        authorization_content);
    if (ret <= 0) {
        (void)memset_s(authorization_content, strlen(authorization_content), 0, strlen(authorization_content));
        g_free(authorization_content);
        authorization_content = NULL;
        debug_log(DLOG_ERROR, "%s:Header_auth_str generate error, ret is %d.", __FUNCTION__, ret);
        return RET_ERR;
    }

    json_object_object_add(*header_json_obj, REQ_HEADER_BASIC_AUTH,
        json_object_new_string((const char *)header_auth_str));

    (void)memset_s(authorization_content, strlen(authorization_content), 0, strlen(authorization_content));
    (void)memset_s(header_auth_str, sizeof(header_auth_str), 0, sizeof(header_auth_str));
    g_free(authorization_content);
    authorization_content = NULL;

    return VOS_OK;
}


LOCAL gint32 _get_if_match_by_url(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_dev, gchar *eth_url, gchar *if_match,
    guint32 if_match_len, json_object **o_message_jso)
{
    errno_t safe_fun_ret;
    json_object *header_json_obj = NULL;
    gint32 ret;
    GSList *output_list = NULL;
    const gchar *resp_body_str = NULL;
    json_object *resp_body_json_obj = NULL;
    json_object *resp_header_json_obj = NULL;
    const gchar *resp_header_json_str = NULL;
    json_object *e_tag_json_obj = NULL;
    const gchar *if_match_tmp = NULL;
    guint32 rsp_code;

    return_val_do_info_if_expr((NULL == i_paras) || (NULL == eth_url) || (NULL == if_match) || (NULL == o_message_jso),
        VOS_ERR, debug_log(DLOG_ERROR, "%s:Input parameters is illegal!", __FUNCTION__));

    header_json_obj = json_object_new_object();
    ret = _generate_basic_author_header(i_paras, &header_json_obj);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        do_info_if_true(NULL != header_json_obj, (void)json_object_put(header_json_obj));
        debug_log(DLOG_ERROR, "%s:Generate basic author head failed, ret is %d", __FUNCTION__, ret));

    // 调用方法
    
    ret = _send_get_request_method(i_paras, eth_url, header_json_obj, obj_dev, &output_list);
    json_object_clear_string(header_json_obj, REQ_HEADER_BASIC_AUTH);
    (void)json_object_put(header_json_obj);

    goto_label_do_info_if_fail(VOS_OK == ret && NULL != output_list, error_exit,
        debug_log(DLOG_ERROR, "%s:call method %s failed, ret is %d", __FUNCTION__, METHOD_HANDLE_REST_REQUEST, ret));

    rsp_code = g_variant_get_uint32((GVariant *)g_slist_nth_data(output_list, 0));
    resp_header_json_str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 1), NULL);
    resp_body_str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 2), NULL);
    resp_body_json_obj = json_tokener_parse(resp_body_str);
    resp_header_json_obj = json_tokener_parse(resp_header_json_str);
    (void)uip_free_gvariant_list(output_list);

    goto_label_do_info_if_fail(NULL != resp_header_json_obj, error_exit,
        debug_log(DLOG_ERROR, "%s %d: parse manager respone head string fail", __FUNCTION__, __LINE__));

    ret = _process_rest_request_rsp(rsp_code, eth_url, resp_body_json_obj, o_message_jso);
    return_val_do_info_if_expr(VOS_OK != ret, ret, (void)json_object_put(resp_body_json_obj);
        (void)json_object_put(resp_header_json_obj);
        debug_log(DLOG_ERROR, "%s:method %s request error, ret is %d", __FUNCTION__, METHOD_HANDLE_REST_REQUEST, ret));

    ret = json_object_object_get_ex(resp_header_json_obj, RESP_HEADER_ETAG, &e_tag_json_obj);
    goto_label_do_info_if_expr(FALSE == ret, error_exit,
        debug_log(DLOG_ERROR, "%s %d: parse manager respone string fail", __FUNCTION__, __LINE__));

    if_match_tmp = dal_json_object_get_str(e_tag_json_obj);

    debug_log(DLOG_DEBUG, "%s %d: Etag:%s.", __FUNCTION__, __LINE__, if_match_tmp);

    safe_fun_ret = memcpy_s(if_match, if_match_len, if_match_tmp, strlen(if_match_tmp));
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    do_if_fail(NULL == resp_header_json_obj, (void)json_object_put(resp_header_json_obj));
    do_if_fail(NULL == resp_body_json_obj, (void)json_object_put(resp_body_json_obj));

    return HTTP_OK;

error_exit:
    do_if_fail(NULL == resp_header_json_obj, (void)json_object_put(resp_header_json_obj));
    do_if_fail(NULL == resp_body_json_obj, (void)json_object_put(resp_body_json_obj));
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL gint32 _get_patch_res_url(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_dev, gchar *request_url, gchar *res_url,
    guint32 res_url_len, json_object **o_message_jso)
{
    errno_t safe_fun_ret;
    json_object *header_json_obj = NULL;
    gint32 ret;
    GSList *output_list = NULL;
    const gchar *resp_body_str = NULL;
    json_object *resp_body_json_obj = NULL;
    json_object *resp_managers_arry_obj = NULL;
    json_object *member_json_obj = NULL;
    json_object *res_json_obj = NULL;
    const gchar *res_url_tmp = NULL;
    guint32 rsp_code;

    return_val_do_info_if_expr((NULL == i_paras) || (NULL == request_url) || (NULL == res_url) ||
        (NULL == o_message_jso),
        VOS_ERR, debug_log(DLOG_ERROR, "%s:Input parameters is illegal!", __FUNCTION__));

    header_json_obj = json_object_new_object();
    ret = _generate_basic_author_header(i_paras, &header_json_obj);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        do_info_if_true(NULL != header_json_obj, (void)json_object_put(header_json_obj));
        debug_log(DLOG_ERROR, "%s:Generate basic author head failed, ret is %d", __FUNCTION__, ret));

    // 调用方法
    
    ret = _send_get_request_method(i_paras, request_url, header_json_obj, obj_dev, &output_list);
    json_object_clear_string(header_json_obj, REQ_HEADER_BASIC_AUTH);
    (void)json_object_put(header_json_obj);

    goto_label_do_info_if_fail((VOS_OK == ret) && (NULL != output_list), error_exit,
        debug_log(DLOG_ERROR, "%s:call method %s failed, ret is %d", __FUNCTION__, METHOD_HANDLE_REST_REQUEST, ret));

    rsp_code = g_variant_get_uint32((GVariant *)g_slist_nth_data(output_list, 0));
    resp_body_str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 2), NULL);
    resp_body_json_obj = json_tokener_parse(resp_body_str);
    uip_free_gvariant_list(output_list);

    goto_label_do_info_if_fail(NULL != resp_body_json_obj, error_exit,
        debug_log(DLOG_ERROR, "%s %d: parse manager respone body string fail", __FUNCTION__, __LINE__));

    ret = _process_rest_request_rsp(rsp_code, request_url, resp_body_json_obj, o_message_jso);
    return_val_do_info_if_fail(VOS_OK == ret, ret, json_object_put(resp_body_json_obj);
        debug_log(DLOG_ERROR, "%s:method %s request error, ret is %d", __FUNCTION__, METHOD_HANDLE_REST_REQUEST, ret));

    ret = json_object_object_get_ex(resp_body_json_obj, RFPROP_MANAGERCOLLECTION_MEMBERS, &resp_managers_arry_obj);
    goto_label_do_info_if_expr(FALSE == ret, error_exit,
        debug_log(DLOG_ERROR, "%s %d: parse manager respone string fail", __FUNCTION__, __LINE__));

    
    member_json_obj = json_object_array_get_idx(resp_managers_arry_obj, 0);

    ret = json_object_object_get_ex(member_json_obj, RFPROP_ODATA_ID, &res_json_obj);
    goto_label_do_info_if_expr(FALSE == ret, error_exit,
        debug_log(DLOG_ERROR, "%s %d: parse manager respone string fail", __FUNCTION__, __LINE__));

    res_url_tmp = dal_json_object_get_str(res_json_obj);
    debug_log(DLOG_INFO, "%s: ret:%d,rsc_url:%s.", __FUNCTION__, ret, res_url_tmp);

    safe_fun_ret = memcpy_s(res_url, res_url_len, res_url_tmp, strlen(res_url_tmp));
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

    do_if_fail(NULL == resp_body_json_obj, (void)json_object_put(resp_body_json_obj));
    return HTTP_OK;

error_exit:
    do_if_fail(NULL == resp_body_json_obj, (void)json_object_put(resp_body_json_obj));
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL gint32 _act_para_verify(json_object *request_body, json_object **o_message_jso)
{
#define PWD_VALIDITY_REGEX_PATTERN "^[0-9a-zA-Z_ `~!@#$%^&*()-=+{}|;,'.:\"(.*)\"?/<>\\-\\.]{0,48}$"
    gint32 ret;
    json_object *action_para_tmp = NULL;
    gboolean is_match;
    const gchar *tmpStr = NULL;

    return_val_do_info_if_fail((NULL != request_body) && (NULL != o_message_jso), HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s:Input para was NULL!", __FUNCTION__));

    // 获取SN信息,都为空返回失败
    ret = json_object_object_get_ex(request_body, RFPROP_DEVICES_SN, &action_para_tmp);
    return_val_do_info_if_expr(FALSE == ret, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_ACT_PARA_MISSING, NULL, o_message_jso, RFPROP_SET_DEV_IPINFO_ACTION,
        RFPROP_DEVICES_SN);
        debug_log(DLOG_ERROR, "%s:Serial number was not found!", __FUNCTION__));

    // 获取用户名字段,都为空返回失败
    ret = json_object_object_get_ex(request_body, RFPROP_DEVICE_USER_NAME, &action_para_tmp);
    return_val_do_info_if_expr(FALSE == ret, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_ACT_PARA_MISSING, NULL, o_message_jso, RFPROP_SET_DEV_IPINFO_ACTION,
        RFPROP_DEVICE_USER_NAME);
        debug_log(DLOG_ERROR, "%s:Username was not found!", __FUNCTION__));

    // 获取密码字段,都为空返回失败
    ret = json_object_object_get_ex(request_body, RFPROP_DEVICE_PASSWORD, &action_para_tmp);
    return_val_do_info_if_expr(FALSE == ret, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_ACT_PARA_MISSING, NULL, o_message_jso, RFPROP_SET_DEV_IPINFO_ACTION,
        RFPROP_DEVICE_PASSWORD);
        debug_log(DLOG_ERROR, "%s:Password were not found!", __FUNCTION__));

    tmpStr = dal_json_object_get_str(action_para_tmp);
    
    is_match = g_regex_match_simple(PWD_VALIDITY_REGEX_PATTERN, tmpStr, G_REGEX_OPTIMIZE, (GRegexMatchFlags)0);
    if (is_match != TRUE) {
        (void)create_message_info(MSGID_ACT_PARA_FORMAT_ERR, NULL, o_message_jso, RF_SENSITIVE_INFO,
            RFPROP_DEVICE_PASSWORD, RFPROP_SET_DEV_IPINFO_ACTION);
        debug_log(DLOG_ERROR, "%s: pwd does not match with pattern %s", __FUNCTION__, PWD_VALIDITY_REGEX_PATTERN);
        return HTTP_BAD_REQUEST;
    }

    // 获取IP信息,都为空返回失败
    ret = json_object_object_get_ex(request_body, RFPROP_DEVICE_IPV4ADDR, &action_para_tmp);
    ret += json_object_object_get_ex(request_body, RFPROP_DEVICE_IPV6ADDR, &action_para_tmp);
    ret += json_object_object_get_ex(request_body, RFPROP_DEVICE_IPV6_GATEWAY, &action_para_tmp);

    return_val_do_info_if_expr(FALSE == ret, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_ACT_PARA_MISSING, NULL, o_message_jso, RFPROP_SET_DEV_IPINFO_ACTION,
        RFPROP_DEVICE_IPV4ADDR);
        debug_log(DLOG_ERROR, "%s:IPv4 IPv6 and IPv6Gateway were not found!", __FUNCTION__));
    return VOS_OK;
}

LOCAL gint32 get_set_device_ipinfo_eth_url(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, OBJ_HANDLE obj_dev,
    gchar* eth_url, guint32 eth_url_size)
{
    gint32 ret;
    errno_t safe_fun_ret;
    gchar odata_id[MAX_URI_LENGTH] = {0};

    // 查找manger_id中的url
    ret = _get_patch_res_url(i_paras, obj_dev, MANAGERS_URL, odata_id, sizeof(odata_id), o_message_jso);
    if (ret != HTTP_OK) {
        debug_log(DLOG_ERROR, "%s:Get url of mangers error, ret is %d!", __FUNCTION__, ret);
        return ret;
    }            
    
    // 查找eth_id中的url
    safe_fun_ret = strncat_s(odata_id, sizeof(odata_id), "/EthernetInterfaces", strlen("/EthernetInterfaces"));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        return HTTP_BAD_REQUEST;
    }       

    ret = _get_patch_res_url(i_paras, obj_dev, odata_id, eth_url, eth_url_size, o_message_jso);
    if (ret != HTTP_OK) {
        debug_log(DLOG_ERROR, "%s:Get url of eth error, ret is %d!", __FUNCTION__, ret);
        return ret;
    }

    return RET_OK;
}

LOCAL gint32 get_set_device_ipinfo_header(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, OBJ_HANDLE obj_dev,
    gchar* eth_url, gchar** header_str)
{
    gint32 ret;
    gchar if_match[MAX_HEADER_CONTENT_LEN] = {0};
    const gchar* header_json_str = NULL;
    json_object* header_json_obj = NULL;

    // 获取指定url的ifmatch字段 该函数内有使用到PASSWORD信息，RFPROP_DEVICE_PASSWORD资源需在此之后释放
    ret = _get_if_match_by_url(i_paras, obj_dev, eth_url, if_match, sizeof(if_match), o_message_jso);
    if (!((ret == HTTP_OK) && (strlen(if_match) != 0))) {
        debug_log(DLOG_ERROR, "%s:Get if macth of patch url error, ret is %d!", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    // 生成头部信息
    header_json_obj = json_object_new_object();
    if (header_json_obj == NULL) {
        return RET_ERR;
    }
    ret = _generate_basic_author_header(i_paras, &header_json_obj);
    if (ret != RET_OK) {
        (void)json_object_put(header_json_obj);
        debug_log(DLOG_ERROR, "%s:Generate basic author is error, ret is %d!", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    } 

    
    json_object_object_add(header_json_obj, REQ_HEADER_IF_MATCH, json_object_new_string((const gchar*)if_match));

    header_json_str = dal_json_object_to_json_string(header_json_obj);

    *header_str = (gchar*)g_malloc0(strlen(header_json_str) + 1);
    if (*header_str == NULL) {
        json_object_clear_string(header_json_obj, REQ_HEADER_BASIC_AUTH);
        (void)json_object_put(header_json_obj);
        return RET_ERR;
    }
    ret = strcpy_s(*header_str, strlen(header_json_str) + 1, header_json_str);
    if (ret != EOK) {
        g_free(*header_str);
        *header_str = NULL;
        json_object_clear_string(header_json_obj, REQ_HEADER_BASIC_AUTH);
        (void)json_object_put(header_json_obj);
        return RET_ERR;
    }
    json_object_clear_string(header_json_obj, REQ_HEADER_BASIC_AUTH);
    (void)json_object_put(header_json_obj);
    return RET_OK;
}

LOCAL gint32 get_set_device_ipinfo_body(PROVIDER_PARAS_S* i_paras, gchar** body_str)
{
    gint32 ret;
    json_object* body_json_obj = NULL;
    const gchar* body_json_str = NULL;

    body_json_obj = json_tokener_parse(dal_json_object_get_str(i_paras->val_jso));
    json_object_object_del(body_json_obj, RFPROP_DEVICES_SN);
    json_object_object_del(body_json_obj, RFPROP_DEVICE_USER_NAME);
    json_object_object_del(body_json_obj, RFPROP_DEVICE_PASSWORD);
    body_json_str = dal_json_object_to_json_string(body_json_obj);

    *body_str = (gchar*)g_malloc0(strlen(body_json_str) + 1);
    if (*body_str == NULL) {
        (void)json_object_put(body_json_obj);
        return RET_ERR;
    }
    ret = strcpy_s(*body_str, strlen(body_json_str) + 1, body_json_str);
    if (ret != EOK) {
        g_free(*body_str);
        json_object_put(body_json_obj);
        return RET_ERR;
    }
    (void)json_object_put(body_json_obj);
    return RET_OK;
}

LOCAL gint32 patch_device_ip(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, OBJ_HANDLE obj_dev,
    gchar* eth_url, guint32* rsp_code, json_object** resp_body_json_obj)
{
    gint32 ret;
    gchar* header_json_str = NULL;
    gchar* body_json_str = NULL;
    GSList* input_list = NULL;
    GSList* output_list = NULL;
    const gchar* resp_body_str = NULL;

    // 获取首部字符串
    ret = get_set_device_ipinfo_header(i_paras, o_message_jso, obj_dev, eth_url, &header_json_str);
    if (ret != RET_OK) {
        json_object_clear_string(i_paras->val_jso, RFPROP_DEVICE_PASSWORD);
        return ret;
    }

    json_object_clear_string(i_paras->val_jso, RFPROP_DEVICE_PASSWORD);

    // 构造消息体
    ret = get_set_device_ipinfo_body(i_paras, &body_json_str);
    if (ret != RET_OK) {
        g_free(header_json_str);
        header_json_str = NULL;
        return ret;
    }

    debug_log(DLOG_DEBUG, "%s:The body json:%s.", __FUNCTION__, body_json_str);
   
    // 调用方法
    
    input_list = g_slist_append(input_list, g_variant_new_string(HTTP_PATCH));
    input_list = g_slist_append(input_list, g_variant_new_string((const gchar*)eth_url));
    input_list = g_slist_append(input_list, g_variant_new_string(header_json_str));
    input_list = g_slist_append(input_list, g_variant_new_string(body_json_str));
    g_free(header_json_str);
    header_json_str = NULL;
    g_free(body_json_str);
    body_json_str = NULL;

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_dev,
        CLASS_DEVICE_INFO, METHOD_HANDLE_REST_REQUEST, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, &output_list);
    uip_free_gvariant_list(input_list);

    if (ret != RET_OK && output_list == NULL) {
        debug_log(DLOG_ERROR, "%s:call method %s failed, ret is %d", __FUNCTION__, METHOD_HANDLE_REST_REQUEST, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    *rsp_code = g_variant_get_uint32((GVariant*)g_slist_nth_data(output_list, 0));
    resp_body_str = g_variant_get_string((GVariant*)g_slist_nth_data(output_list, 2), NULL);
    *resp_body_json_obj = json_tokener_parse(resp_body_str);
    uip_free_gvariant_list(output_list);
    return RET_OK;
}

/*****************************************************************************
 功能描述  : 设置设备Ipinfo 的action方法
 输入参数  : PROVIDER_PARAS_S* i_paras, json_object** o_message_jso
 输出参数  : json_object** o_result_jso
 返 回 值  : 无
 修改历史      :
  1.日    期   : 2018年12月17日
    作    者   : z00454788
    修改内容   : 新生成函数

  2.日    期   : 2019年6月20日
    作    者   : zhangxiong z00454788
    修改内容   : DTS2019061806459 修改patch请求的响应解析
*****************************************************************************/
LOCAL gint32 act_set_device_ipinfo(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, json_object** o_result_jso)
{
    gint32      ret;
    gchar eth_url[MAX_URI_LENGTH] = {0};
    OBJ_HANDLE  obj_dev = 0;
    guint32     rsp_code = HTTP_OK;
    json_object* resp_body_json_obj = NULL;

    
    if ((NULL == o_result_jso) || (NULL == o_message_jso) || (VOS_OK != provider_paras_check(i_paras))) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if ((i_paras->user_role_privilege & USERROLE_BASICSETTING) == 0) {
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_SET_DEV_IPINFO_ACTION,
            o_message_jso, RFPROP_SET_DEV_IPINFO_ACTION);
        return HTTP_FORBIDDEN;
    }

    ret = _act_para_verify(i_paras->val_jso, o_message_jso);
    if (ret != RET_OK) {
        json_object_clear_string(i_paras->val_jso, RFPROP_DEVICE_PASSWORD);
        debug_log(DLOG_ERROR, "%s:_act_para_verify is error, ret is %d", __FUNCTION__, ret);
        return ret;
    }

    
    ret = _find_dev_obj(i_paras, &obj_dev, o_message_jso);
    if (ret != RET_OK) {
        json_object_clear_string(i_paras->val_jso, RFPROP_DEVICE_PASSWORD);
        debug_log(DLOG_ERROR, "%s:find device obj is error, ret is %d!", __FUNCTION__, ret);
        return ret;
    }

    // 获取指定url
    ret = get_set_device_ipinfo_eth_url(i_paras, o_message_jso, obj_dev, eth_url, sizeof(eth_url));
    if (ret != RET_OK) {
        json_object_clear_string(i_paras->val_jso, RFPROP_DEVICE_PASSWORD);
        return ret;
    }
    debug_log(DLOG_DEBUG, "%s:eth_url:%s.", __FUNCTION__, eth_url);

    ret = patch_device_ip(i_paras, o_message_jso, obj_dev, eth_url, &rsp_code, &resp_body_json_obj);
    if (ret != RET_OK || resp_body_json_obj == NULL) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s %d: parse manager respone body string fail", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = _process_rest_request_rsp(rsp_code, eth_url, resp_body_json_obj, o_message_jso);
    return_val_do_info_if_fail(VOS_OK == ret, ret, (void)json_object_put(resp_body_json_obj);
        debug_log(DLOG_ERROR, "%s:method %s request error, ret is %d", __FUNCTION__, METHOD_HANDLE_REST_REQUEST, ret));

    _set_op_log(i_paras, obj_dev);
    (void)json_object_put(resp_body_json_obj);

    return HTTP_OK;
}


LOCAL gint32 get_setdevipinfo_actioninfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    if ((NULL == o_result_jso) || (NULL == o_message_jso) || (VOS_OK != provider_paras_check(i_paras))) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_string(URI_FORMAT_SET_DEV_IPINFO_ACTIONINFO);
    return_val_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


gint32 get_network_agent(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    gint32 ret;
    OBJ_HANDLE obj_device_info = 0;
    GSList *device_info_list = NULL;
    GSList *device_info_node = NULL;
    guint8 discovered = DEVICE_OFF_LINE;
    gint32 devices_online_num;
    json_object *devs_ipinfo_arr_jso = NULL;
    json_object *dev_ipinfo_jso = NULL;
    gchar device_sn[MAX_PRO_SN_LEN + 1] = {0};
    gchar ip_address[IPV4_IP_STR_SIZE + 1] = {0};
    gchar ipv6_address[IPV6_IP_STR_SIZE + 1] = {0};

    if (o_rsc_jso == NULL || o_err_array_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return RF_FAILED;
    }

    
    devs_ipinfo_arr_jso = json_object_new_array();
    return_val_do_info_if_fail(NULL != devs_ipinfo_arr_jso, RF_FAILED,
        debug_log(DLOG_ERROR, "[%s]: alloc new json array failed", __FUNCTION__));

    json_object_object_add(o_rsc_jso, RFPROP_DEVICES_IPINFO, devs_ipinfo_arr_jso);

    ret = dfl_get_object_list(CLASS_DEVICE_INFO, &device_info_list);
    return_val_do_info_if_fail(VOS_OK == ret, RF_OK,
        debug_log(DLOG_DEBUG, "[%s]: Get device info list failed", __FUNCTION__));

    for (device_info_node = device_info_list; NULL != device_info_node; device_info_node = device_info_node->next) {
        obj_device_info = (OBJ_HANDLE)device_info_node->data;
        (void)dal_get_property_value_byte(obj_device_info, PROPERTY_DEVICE_INFO_DISCOVERED, &discovered);
        continue_if_expr(DEVICE_OFF_LINE == discovered);

        
        (void)dal_get_property_value_string(obj_device_info, PROPERTY_DEVICE_INFO_SER_NUM, device_sn,
            sizeof(device_sn));
        continue_do_info_if_expr(0 == strlen(device_sn),
            debug_log(DLOG_INFO, "[%s]: device_sn is empty, iteam is invalid.", __FUNCTION__));
        (void)dal_get_property_value_string(obj_device_info, PROPERTY_DEVICE_INFO_IP_ADDR, ip_address,
            sizeof(ip_address));
        (void)dal_get_property_value_string(obj_device_info, PROPERTY_DEVICE_INFO_IPV6_ADDR, ipv6_address,
            sizeof(ipv6_address));

        
        dev_ipinfo_jso = json_object_new_object();
        json_object_object_add(dev_ipinfo_jso, RFPROP_DEVICES_SN, json_object_new_string((const gchar *)device_sn));
        json_object_object_add(dev_ipinfo_jso, RFPROP_DEVICE_IPV4ADDR,
            VOS_ERR == rf_string_check((const gchar *)ip_address) ? NULL :
                                                                    json_object_new_string((const gchar *)ip_address));
        json_object_object_add(dev_ipinfo_jso, RFPROP_DEVICE_IPV6ADDR,
            VOS_ERR == rf_string_check((const gchar *)ipv6_address) ?
            NULL :
            json_object_new_string((const gchar *)ipv6_address));
        ret = json_object_array_add(devs_ipinfo_arr_jso, dev_ipinfo_jso);
        continue_do_info_if_expr(VOS_OK != ret,
            debug_log(DLOG_ERROR, "[%s][%d] json_object_array_add error  \n", __FUNCTION__, __LINE__);
            json_object_put(dev_ipinfo_jso));
    }
    g_slist_free(device_info_list);
    device_info_list = NULL;

    devices_online_num = json_object_array_length(devs_ipinfo_arr_jso);
    json_object_object_add(o_rsc_jso, RFPROP_DEVICES_DISCOVERED_COUNT, json_object_new_int(devices_online_num));

    return RF_OK;
}


gint32 rack_mgnt_network_agent_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    
    return_val_if_fail(NULL != i_paras && NULL != prop_provider && NULL != count, HTTP_INTERNAL_SERVER_ERROR);

    
    return_val_do_info_if_expr(FALSE == rf_support_rack_mgnt_agent_service(), HTTP_NOT_FOUND,
        debug_log(DLOG_MASS, "%s, %d: not support", __FUNCTION__, __LINE__));

    *prop_provider = g_rack_mgnt_network_agent_provider;
    *count = sizeof(g_rack_mgnt_network_agent_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}


gint32 rack_mgnt_set_dev_ipinfo_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras,
    PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    return_val_do_info_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed: input param error", __FUNCTION__));

    
    return_val_do_info_if_expr(FALSE == rf_support_rack_mgnt_agent_service(), HTTP_NOT_FOUND,
        debug_log(DLOG_MASS, "%s, %d: not support", __FUNCTION__, __LINE__));

    *prop_provider = g_rack_mgnt_set_dev_ipinfo_actioninfo_provider;
    *count = sizeof(g_rack_mgnt_set_dev_ipinfo_actioninfo_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}
