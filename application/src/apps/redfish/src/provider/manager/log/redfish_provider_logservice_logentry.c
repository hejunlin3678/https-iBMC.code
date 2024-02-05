
#include "redfish_provider.h"
#include "redfish_http.h"

typedef void (*parser_log_handler)(const gchar *buffer, guint32 max_buffer_len, gint32 id, json_object *result_obj);
extern gint32 get_logservice_operatelog_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
extern gint32 get_logservice_runlog_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
extern gint32 get_logservice_securitylog_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);


LOCAL gint32 check_log_member_id_valid(const gchar *member_id, const gchar *log_path)
{
    gint32 ret;
    gint32 log_count = 0;
    gint32 item_index = 0;
    return_val_if_expr(member_id == NULL || log_path == NULL, VOS_ERR);

    ret = get_file_lines(log_path, &log_count);
    return_val_if_expr(ret != VOS_OK, VOS_ERR);

    (void)vos_str2int(member_id, 10, &item_index, NUM_TPYE_INT32);

    return_val_if_expr(item_index == 0 || item_index > log_count, VOS_ERR);

    return VOS_OK;
}


LOCAL gint32 common_get_log_item(PROVIDER_PARAS_S *i_paras, json_object *o_result_jso, gchar *log_path,
    parser_log_handler parser_func, const gchar *context_tpl, const gchar *odata_id_tpl)
{
    int iRet;
    gint32 i = 0;
    gint32 ret;
    gint32 log_count = 0;
    gint32 item_index = 0;
    FILE *pf = NULL;
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    gchar tmp_buf[MAX_BUFF_SIZE] = {0};

    return_val_do_info_if_expr(
        (NULL == o_result_jso || VOS_OK != provider_paras_check(i_paras) || log_path == NULL || parser_func == NULL),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));

    
    (void)vos_str2int(i_paras->member_id, 10, &item_index, NUM_TPYE_INT32);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    iRet = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, odata_id_tpl, slot, item_index);
#pragma GCC diagnostic pop
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    json_object_object_add(o_result_jso, RFPROP_ODATA_ID, json_object_new_string((const gchar *)uri));
    (void)memset_s(uri, sizeof(uri), 0, sizeof(uri));
    
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    iRet = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, context_tpl, slot);
#pragma GCC diagnostic pop
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    json_object_object_add(o_result_jso, RFPROP_ODATA_CONTEXT, json_object_new_string((const gchar *)uri));

    ret = get_file_lines(log_path, &log_count);
    
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s, %d:   memory alloc fail..", __FUNCTION__, __LINE__));
    
    
    pf = fopen(log_path, "r");
    return_val_do_info_if_expr(pf == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:   Open file failed.", __FUNCTION__, __LINE__));

    while (fgets(tmp_buf, MAX_BUFF_SIZE, pf) && i < log_count) {
        if (i + 1 == item_index) {
            parser_func(tmp_buf, sizeof(tmp_buf), i + 1, o_result_jso);
            break;
        }
        i++;
    }

    (void)fclose(pf);
    return RF_OK;
}


LOCAL gint32 common_get_log_members(PROVIDER_PARAS_S *i_paras, json_object **o_result_jso, const gchar *log_path,
    const gchar *log_uri, const gchar *log_cxt, parser_log_handler parser_func)
{
    int iRet;
    gint32 ret;
    gint32 i = 0;
    FILE *pf = NULL;
    gint32 skip = 0;
    gint32 top = 0;
    gint32 log_count = 0;
    json_object *obj_jso = NULL;
    gchar uri[PSLOT_URI_LEN] = {0};
    gchar cxt[PSLOT_URI_LEN] = {0};
    gchar pslot[MEMORY_SN_LEN] = {0};
    gchar tmp_buf[MAX_BUFF_SIZE] = {0};

    return_val_if_expr((log_path == NULL) || (log_uri == NULL) || (parser_func == NULL) || (o_result_jso == NULL) ||
        (log_cxt == NULL),
        HTTP_INTERNAL_SERVER_ERROR);

    ret = redfish_get_board_slot(pslot, MEMORY_SN_LEN);
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_array();
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:   memory alloc fail. ", __FUNCTION__, __LINE__));

    ret = get_file_lines(log_path, &log_count);
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR, json_object_put(*o_result_jso);
        debug_log(DLOG_ERROR, "%s, %d:   memory alloc fail..", __FUNCTION__, __LINE__));

    skip = i_paras->skip;
    i_paras->skip = INVALID_INTEGER_VALUE;
    top = i_paras->top;
    i_paras->top = INVALID_INTEGER_VALUE;
    top = (skip + top > log_count) ? (log_count - skip) : top;
    
    top = (top < JSON_SCHEMA_MEMBERS_MAX) ? top : JSON_SCHEMA_MEMBERS_MAX;

    pf = fopen(log_path, "r");
    return_val_do_info_if_expr(pf == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: Open file failed.", __FUNCTION__, __LINE__));

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    iRet = snprintf_s(cxt, sizeof(cxt), sizeof(cxt) - 1, log_cxt, pslot);
#pragma GCC diagnostic pop
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    while (fgets(tmp_buf, MAX_BUFF_SIZE, pf) && (i < log_count)) {
        i++;
        if (i > (skip + top)) {
            break;
        }

        if (i > skip) {
            obj_jso = NULL;
            obj_jso = json_object_new_object();
            continue_if_expr(obj_jso == NULL);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
            iRet = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, log_uri, pslot, i);
#pragma GCC diagnostic pop

            do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
            json_object_object_add(obj_jso, RFPROP_ODATA_ID, json_object_new_string((const gchar *)uri));
            json_object_object_add(obj_jso, RFPROP_ODATA_CONTEXT, json_object_new_string((const gchar *)cxt));
            parser_func(tmp_buf, sizeof(tmp_buf), i, obj_jso);
            ret = json_object_array_add(*o_result_jso, obj_jso);
            do_val_if_expr(VOS_OK != ret, json_object_put(obj_jso);
                debug_log(DLOG_ERROR, "%s %d:json_object_array_add fail", __FUNCTION__, __LINE__));
        }
    }
    (void)fclose(pf);

    return HTTP_OK;
}

LOCAL gint32 get_operatelog_entry_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return get_universal_provider_odata_info(i_paras, o_message_jso, o_result_jso,
        RFPROP_MANAGER_LOGSERVICE_OPERATELOG_ENTRIES);
}

LOCAL gint32 get_operatelog_entry_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return get_universal_provider_odata_info(i_paras, o_message_jso, o_result_jso,
        RFPROP_MANAGER_LOGSERVICE_OPERATELOG_CONTEXT);
}


LOCAL void add_common_property(gint32 id, gchar *data, gchar *desp, gchar *log_type, json_object *result_obj)
{
    return_if_expr(result_obj == NULL);
    json_object_object_add(result_obj, RFPROP_ODATA_TYPE, json_object_new_string("#LogEntry.v1_1_1.LogEntry"));
    json_object_object_add(result_obj, RFPROP_COMMON_NAME, json_object_new_string(log_type));
    json_object_object_add(result_obj, RFPROP_LOGSERVICE_ENTRY_TYPE, json_object_new_string(RFPROP_OEM));
    json_object_object_add(result_obj, PROPERTY_LOG_ID, json_object_new_int(id));
    if (strlen(data) == 0) {
        json_object_object_add(result_obj, RFPROP_LOGSERVICE_EVENT_TIME, NULL);
    } else {
        json_object_object_add(result_obj, RFPROP_LOGSERVICE_EVENT_TIME, json_object_new_string(data));
    }
    if (strlen(desp) == 0) {
        json_object_object_add(result_obj, RFPROP_LOGSERVICE_ENTRY_DESCROPTION, NULL);
    } else {
        json_object_object_add(result_obj, RFPROP_LOGSERVICE_ENTRY_DESCROPTION, json_object_new_string(desp));
    }
}


LOCAL void remove_string_space(const gchar *buffer, guint32 max_buffer_len, const gchar **string)
{
    guint32 start;
    guint32 str_len = 0;

    // str_len为buffer的包括空字符在内的实际长度
    while (buffer[str_len] != '\n' && str_len < max_buffer_len) {
        str_len++;
    }

    for (start = 0; start < str_len; start++) {
        if (buffer[start] != 0) {
            break;
        }
    }

    *string = &(buffer[start]);
}


LOCAL void add_log_info_to_json(json_object *result_obj, const gchar *interf, const gchar *user, const gchar *address)
{
    json_object *oem = NULL;
    json_object *object = NULL;

    oem = json_object_new_object();
    return_if_expr(oem == NULL);
    json_object_object_add(result_obj, RFPROP_OEM, oem);
    object = json_object_new_object();
    return_if_expr(object == NULL);
    json_object_object_add(oem, RFPROP_OEM_HUAWEI, object);

    json_object_object_add(object, PROPERTY_LOG_INTERFACE, json_object_new_string(interf));
    json_object_object_add(object, PROPERTY_LOG_USER, json_object_new_string(user));
    json_object_object_add(object, PROPERTY_LOG_ADDR, json_object_new_string(address));
}


LOCAL void parse_operatelog(const gchar *buffer, guint32 max_buffer_len, gint32 id, json_object *result_obj)
{
    errno_t safe_fun_ret;
    gchar data[TMP_STR_LEN + 1] = {0};
    gchar full_data[TMP_STR_LEN] = {0};
    gchar times[TMP_STR_LEN + 1] = {0};
    gchar interf[TMP_STR_LEN + 1] = {0};
    gchar user[LDAP_USER_NAME_MAX_LEN + LDAP_USERDOMAIN_MAX_LEN + 2] = {0};
    gchar address[TMP_STR_LEN] = {0};
    gchar module[TMP_STR_LEN + 1] = {0};
    gchar desp[MAX_BUFF_SIZE + 1] = {0};
    gchar user_address[LDAP_USER_NAME_MAX_LEN + LDAP_USERDOMAIN_MAX_LEN + TMP_STR_LEN + 3] = {0};
    gchar *temp = NULL;
    gint32 securec_rv;
    const gchar *str = NULL;

    return_if_expr(buffer == NULL || result_obj == NULL);

    remove_string_space(buffer, max_buffer_len, &str);

    
    
    securec_rv = sscanf_s(str, "%[0-9-] %[0-9:] %[^,],%[^,],%[^,],%[^\r\n]", data, sizeof(data) - 1, times,
        sizeof(times) - 1, interf, sizeof(interf) - 1, user_address, sizeof(user_address) - 1, module,
        sizeof(module) - 1, desp, sizeof(desp) - 1);
    do_if_expr(securec_rv <= 0, debug_log(DLOG_INFO, "%s:sscanf_s failed, ret is %d.", __FUNCTION__, securec_rv));

    
    
    if (strlen(data) == 0 && strlen(times) == 0) {
        debug_log(DLOG_INFO, "%s,%d:params is null.", __FUNCTION__, __LINE__);
    } else {
        securec_rv = snprintf_s(full_data, sizeof(full_data), sizeof(full_data) - 1, "%s %s", data, times);
        do_val_if_expr(securec_rv <= 0,
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, securec_rv));
    }
    // 找出最后一个@
    for (temp = user_address + strlen(user_address); user_address != temp; temp--) {
        if ('@' == *temp) {
            *temp = '\0';
            break;
        }
    }
    safe_fun_ret = strncpy_s(address, TMP_STR_LEN, temp + 1, TMP_STR_LEN - 1);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    safe_fun_ret = strncpy_s(user, sizeof(user), user_address, LDAP_USER_NAME_MAX_LEN + LDAP_USERDOMAIN_MAX_LEN + 1);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

    dal_filter_invisible_char(desp);

    add_common_property(id, full_data, desp, LOG_TYPE_NAME_OPERATE, result_obj);

    if (strlen(desp) != 0) {
        add_log_info_to_json(result_obj, (const gchar *)interf, (const gchar *)user, (const gchar *)address);
    } else {
        json_object_object_add(result_obj, RFPROP_OEM, NULL);
    }
}

LOCAL gint32 get_operatelog_entry_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_if_expr(ret != VOS_OK, ret);

    return_val_do_info_if_expr((i_paras->user_role_privilege & USERROLE_SECURITYMGNT) == 0, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    return common_get_log_members(i_paras, o_result_jso, OPERATE_LOG_PATH, RFPROP_MANAGER_LOGSERVICE_OPERATELOG_URI,
        RFPROP_MANAGER_OPERATELOG_ITEM_CONTEXT, parse_operatelog);
}

LOCAL PROPERTY_PROVIDER_S g_operatelog_entry[] = {
    {RFPROP_MEMBERS_COUNT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_operatelog_count, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_operatelog_entry_members, NULL, NULL, ETAG_FLAG_ENABLE },
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_operatelog_entry_odataid, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_operatelog_entry_context, NULL, NULL, ETAG_FLAG_ENABLE},
};

gint32 manager_provider_operatelog_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gboolean ret;

    return_val_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count), HTTP_INTERNAL_SERVER_ERROR);

    ret = redfish_check_manager_uri_valid(i_paras->uri);
    
    
    if (ret == FALSE && dal_check_if_mm_board() == FALSE) {
        debug_log(DLOG_ERROR, "URI:%s check failed.", i_paras->uri);
        return HTTP_NOT_FOUND;
    } else if (ret == FALSE && dal_check_if_mm_board() == TRUE) {
        debug_log(DLOG_DEBUG, "URI:%s check failed.", i_paras->uri);
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_operatelog_entry;
    *count = sizeof(g_operatelog_entry) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}

gint32 get_operatelog_item_details(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    gint32 ret;

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_rsc_jso) && (NULL != o_err_array_jso), RF_FAILED,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    return_val_if_expr((i_paras->user_role_privilege & USERROLE_SECURITYMGNT) == 0, RF_INSUFFICIENT_PRIV);

    ret = check_log_member_id_valid(i_paras->member_id, OPERATE_LOG_PATH);
    return_val_if_expr(VOS_OK != ret, RF_RSC_NOT_FOUND);

    
    return common_get_log_item(i_paras, o_rsc_jso, OPERATE_LOG_PATH, parse_operatelog,
        RFPROP_MANAGER_OPERATELOG_ITEM_CONTEXT, RFPROP_MANAGER_LOGSERVICE_OPERATELOG_URI);
    
}

LOCAL gint32 get_runlog_entry_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return get_universal_provider_odata_info(i_paras, o_message_jso, o_result_jso,
        RFPROP_MANAGER_LOGSERVICE_RUNLOG_ENTRIES);
}

LOCAL gint32 get_runlog_entry_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return get_universal_provider_odata_info(i_paras, o_message_jso, o_result_jso,
        RFPROP_MANAGER_LOGSERVICE_RUNLOG_CONTEXT);
}


LOCAL void parse_runlog(const gchar *buffer, guint32 max_buffer_len, gint32 id, json_object *result_obj)
{
    gchar data[TMP_STR_LEN] = {0};
    gchar times[TMP_STR_LEN] = {0};
    gchar level[TMP_STR_LEN] = {0};
    gchar desp[MAX_BUFF_SIZE] = {0};
    json_object *huawei = NULL;
    json_object *oem = NULL;
    gint32 securec_rv;
    const gchar *str = NULL;
    const gchar *desp_end = NULL;

    return_if_expr(buffer == NULL || result_obj == NULL);

    remove_string_space(buffer, max_buffer_len, &str);

    securec_rv = sscanf_s(str, "%[0-9-] %[0-9:] %[A-Za-z] : %[^\r\n]", data, sizeof(data), times, sizeof(times),
        level, sizeof(level), desp, sizeof(desp));
    do_if_expr(securec_rv <= 0, debug_log(DLOG_INFO, "%s:sscanf_s failed, ret is %d.", __FUNCTION__, securec_rv));
    if (!g_utf8_validate(desp, -1, &desp_end)) {
        
        debug_log(DLOG_INFO, "%s:invalid utf8 string [%s]", __FUNCTION__, str);
        desp[desp_end - desp] = 0;
    }
    if (strlen(data) == 0 && strlen(times) == 0) {
        debug_log(DLOG_INFO, "%s,%d:params is null.", __FUNCTION__, __LINE__);
    } else {
        securec_rv = snprintf_s(data, sizeof(data), sizeof(data) - 1, "%s %s", data, times);
        do_val_if_expr(securec_rv <= 0,
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, securec_rv));
    }

    add_common_property(id, data, desp, LOG_TYPE_NAME_RUN, result_obj);

    if (strlen(desp) != 0) {
        oem = json_object_new_object();
        return_if_expr(oem == NULL);
        json_object_object_add(result_obj, RFPROP_OEM, oem);

        huawei = json_object_new_object();
        return_if_expr(huawei == NULL);
        json_object_object_add(oem, RFPROP_OEM_HUAWEI, huawei);
        json_object_object_add(huawei, PROPERTY_LOG_LEVEL, json_object_new_string((const gchar *)level));
    } else {
        json_object_object_add(result_obj, RFPROP_OEM, NULL);
    }
}

LOCAL gint32 get_runlog_entry_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_if_expr(ret != VOS_OK, ret);

    return common_get_log_members(i_paras, o_result_jso, RUN_LOG_PATH, RFPROP_MANAGER_LOGSERVICE_RUNLOG_URI,
        RFPROP_MANAGER_RUNLOG_ITEM_CONTEXT, parse_runlog);
}

LOCAL PROPERTY_PROVIDER_S g_runlog_entry[] = {
    {RFPROP_MEMBERS_COUNT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_runlog_count, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,  get_runlog_entry_members, NULL, NULL, ETAG_FLAG_ENABLE },
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_runlog_entry_odataid, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_runlog_entry_context, NULL, NULL, ETAG_FLAG_ENABLE},
};

gint32 manager_provider_runlog_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gboolean ret;

    return_val_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count), HTTP_INTERNAL_SERVER_ERROR);

    ret = redfish_check_manager_uri_valid(i_paras->uri);
    
    
    if (ret == FALSE && dal_check_if_mm_board() == FALSE) {
        debug_log(DLOG_ERROR, "URI:%s check failed.", i_paras->uri);
        return HTTP_NOT_FOUND;
    } else if (ret == FALSE && dal_check_if_mm_board() == TRUE) {
        debug_log(DLOG_DEBUG, "URI:%s check failed.", i_paras->uri);
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_runlog_entry;
    *count = sizeof(g_runlog_entry) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}

gint32 get_runlog_item_details(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    gint32 ret;

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_rsc_jso) && (NULL != o_err_array_jso), RF_FAILED,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    ret = check_log_member_id_valid(i_paras->member_id, RUN_LOG_PATH);
    return_val_if_expr(VOS_OK != ret, RF_RSC_NOT_FOUND);

    
    return common_get_log_item(i_paras, o_rsc_jso, RUN_LOG_PATH, parse_runlog, RFPROP_MANAGER_RUNLOG_ITEM_CONTEXT,
        RFPROP_MANAGER_LOGSERVICE_RUNLOG_URI);
    
}

LOCAL gint32 get_securitylog_entry_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return get_universal_provider_odata_info(i_paras, o_message_jso, o_result_jso,
        RFPROP_MANAGER_LOGSERVICE_SECURITYLOG_ENTRIES);
}

LOCAL gint32 get_securitylog_entry_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return get_universal_provider_odata_info(i_paras, o_message_jso, o_result_jso,
        RFPROP_MANAGER_LOGSERVICE_SECURITYLOG_CONTEXT);
}


LOCAL void parse_securitylog(const gchar *buffer, guint32 max_buffer_len, gint32 id, json_object *result_obj)
{
    gchar data[TMP_STR_LEN] = {0};
    gchar l_times[TMP_STR_LEN] = {0};
    gchar times[TMP_STR_LEN] = {0};
    gchar interf[TMP_STR_LEN] = {0};
    gchar host[TMP_STR_LEN] = {0};
    gchar desp[MAX_BUFF_SIZE] = {0};
    json_object *oem = NULL;
    json_object *huawei = NULL;
    gint32 securec_rv;
    const gchar *str = NULL;
    const gchar *desp_end = NULL;

    return_if_expr(buffer == NULL || result_obj == NULL);

    remove_string_space(buffer, max_buffer_len, &str);

    securec_rv = sscanf_s(str, "%[0-9-]T%[^ ] %[^ ] %[^: ]: %[^\r\n]", data, sizeof(data), l_times, sizeof(l_times),
        host, sizeof(host), interf, sizeof(interf), desp, sizeof(desp));
    do_if_expr(securec_rv <= 0, debug_log(DLOG_INFO, "%s:sscanf_s failed, ret is %d.", __FUNCTION__, securec_rv));
    if (!g_utf8_validate(desp, -1, &desp_end)) {
        
        debug_log(DLOG_INFO, "%s:invalid utf8 string [%s]", __FUNCTION__, str);
        desp[desp_end - desp] = 0;
    }
    // 再次解析times
    securec_rv = sscanf_s(l_times, "%[0-9:]", times, sizeof(times));
    do_if_expr(securec_rv <= 0, debug_log(DLOG_INFO, "%s:sscanf_s failed, ret is %d.", __FUNCTION__, securec_rv));
    if (strlen(data) == 0 && strlen(times) == 0) {
        debug_log(DLOG_INFO, "%s,%d:params is null.", __FUNCTION__, __LINE__);
    } else {
        securec_rv = snprintf_s(data, sizeof(data), sizeof(data) - 1, "%s %s", data, times);
        do_val_if_expr(securec_rv <= 0,
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, securec_rv));
    }

    add_common_property(id, data, desp, LOG_TYPE_NAME_SECURITY, result_obj);

    if (strlen(desp) != 0) {
        oem = json_object_new_object();
        return_if_expr(oem == NULL);

        json_object_object_add(result_obj, RFPROP_OEM, oem);
        huawei = json_object_new_object();
        return_if_expr(huawei == NULL);
        json_object_object_add(oem, RFPROP_OEM_HUAWEI, huawei);
        json_object_object_add(huawei, PROPERTY_LOG_INTERFACE, json_object_new_string((const gchar *)interf));
        json_object_object_add(huawei, PROPERTY_LOG_HOST, json_object_new_string((const gchar *)host));
    } else {
        json_object_object_add(result_obj, RFPROP_OEM, NULL);
    }
}

LOCAL gint32 get_securitylog_entry_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_if_expr(ret != VOS_OK, ret);

    return_val_do_info_if_expr((i_paras->user_role_privilege & USERROLE_SECURITYMGNT) == 0, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    return common_get_log_members(i_paras, o_result_jso, SECURITY_LOG_PATH, RFPROP_MANAGER_LOGSERVICE_SECURITYLOG_URI,
        RFPROP_MANAGER_SECURITYLOG_ITEM_CONTEXT, parse_securitylog);
}

LOCAL PROPERTY_PROVIDER_S g_securitylog_entry[] = {
    {RFPROP_MEMBERS_COUNT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_securitylog_count, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_securitylog_entry_members, NULL, NULL, ETAG_FLAG_ENABLE },
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_securitylog_entry_odataid, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_securitylog_entry_context, NULL, NULL, ETAG_FLAG_ENABLE},
};

gint32 manager_provider_securitylog_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gboolean ret;

    return_val_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count), HTTP_INTERNAL_SERVER_ERROR);

    ret = redfish_check_manager_uri_valid(i_paras->uri);
    
    
    if (ret == FALSE && dal_check_if_mm_board() == FALSE) {
        debug_log(DLOG_ERROR, "URI:%s check failed.", i_paras->uri);
        return HTTP_NOT_FOUND;
    } else if (ret == FALSE && dal_check_if_mm_board() == TRUE) {
        debug_log(DLOG_DEBUG, "URI:%s check failed.", i_paras->uri);
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_securitylog_entry;
    *count = sizeof(g_securitylog_entry) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}

gint32 get_securitylog_item_details(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    gint32 ret;

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_rsc_jso) && (NULL != o_err_array_jso), RF_FAILED,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    return_val_if_expr((i_paras->user_role_privilege & USERROLE_SECURITYMGNT) == 0, RF_INSUFFICIENT_PRIV);

    ret = check_log_member_id_valid(i_paras->member_id, SECURITY_LOG_PATH);
    return_val_if_expr(VOS_OK != ret, RF_RSC_NOT_FOUND);

    
    return common_get_log_item(i_paras, o_rsc_jso, SECURITY_LOG_PATH, parse_securitylog,
        RFPROP_MANAGER_SECURITYLOG_ITEM_CONTEXT, RFPROP_MANAGER_LOGSERVICE_SECURITYLOG_URI);
    
}
