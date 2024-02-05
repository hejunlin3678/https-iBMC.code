

#include "redfish_provider.h"
#include "redfish_http.h"
#include "redfish_task.h"

LOCAL PROPERTY_PROVIDER_S g_task_monitor_rsc_provider[] = {
};

LOCAL gchar **get_monitor_create_location_str(void)
{
    static gchar *monitor_create_location_str = NULL;
    return &monitor_create_location_str;
}


gint32 get_task_monitor_id_value(const gchar *uri, gint32 *monitor_id)
{
    gchar task_monitor_resourse[MAX_URI_LENGTH] = {0};
    errno_t str_ret;
    gint32 ret = 0;
    GRegex *regex = NULL;
    GMatchInfo *match_info = NULL;
    gchar *monitor_id_value = NULL;
    gboolean is_match;
    gchar *pattern_lower = NULL;
    gint32 result_val = VOS_ERR;

    str_ret = strncpy_s(task_monitor_resourse, MAX_URI_LENGTH - 1, uri, strlen(uri));
    return_val_do_info_if_fail(str_ret == EOK, VOS_ERR, debug_log(DLOG_ERROR, "%s strncpy_s fail.", __FUNCTION__));

    
    pattern_lower = g_ascii_strdown(task_monitor_resourse, strlen(task_monitor_resourse));
    return_val_do_info_if_fail(NULL != pattern_lower, VOS_ERR,
        debug_log(DLOG_ERROR, "%s g_ascii_strdown fail.", __FUNCTION__));

    regex = g_regex_new(TASK_MONITOR_REGEX, (GRegexCompileFlags)0x0, (GRegexMatchFlags)0x0, NULL);
    is_match = g_regex_match(regex, pattern_lower, (GRegexMatchFlags)0x0, &match_info);
    g_free(pattern_lower);
    do_info_if_true(regex, g_regex_unref(regex));

    
    if (TRUE == is_match) {
        monitor_id_value = g_match_info_fetch(match_info, 1);
        do_info_if_true(match_info, g_match_info_free(match_info); match_info = NULL);
        return_val_do_info_if_fail(NULL != monitor_id_value, VOS_ERR,
            debug_log(DLOG_ERROR, "%s g_match_info_fetch fail.", __FUNCTION__));

        ret = vos_str2int(monitor_id_value, 10, monitor_id, NUM_TPYE_INT32);
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, g_free(monitor_id_value);
            debug_log(DLOG_ERROR, "%s vos_str2int fail.", __FUNCTION__));
        g_free(monitor_id_value);
        result_val = VOS_OK;
    }

    do_info_if_true(match_info, g_match_info_free(match_info));
    return result_val;
}


LOCAL gint32 get_monitor_create_resourse_body(json_object *json_body, json_object **o_message_jso,
    json_object **o_result_jso)
{
    const gchar *create_path = NULL;
    json_object *res = NULL;
    gint32 ret;
    gchar **monitor_create_location_str = get_monitor_create_location_str();

    create_path = dal_json_object_get_str(json_body);
    return_val_do_info_if_fail(NULL != create_path, VOS_ERR,
        debug_log(DLOG_ERROR, "%s dal_json_object_get_str fail.", __FUNCTION__));

    
    ret = rf_validate_rsc_exist(create_path, &res);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s rf_validate_rsc_exist fail.", __FUNCTION__));

    
    (void)json_object_set_object(*o_result_jso, res);
    (void)json_object_put(res);

    if (NULL != *monitor_create_location_str) {
        g_free(*monitor_create_location_str);
        *monitor_create_location_str = NULL;
    }

    *monitor_create_location_str = g_strdup(create_path);
    return_val_do_info_if_fail(NULL != *monitor_create_location_str, VOS_ERR, json_object_put(*o_result_jso);
        *o_result_jso = NULL; debug_log(DLOG_ERROR, "%s g_strdup fail.", __FUNCTION__));

    return HTTP_CREATED;
}

json_object *monitor_create_location(void)
{
    json_object *header_jso = NULL;
    json_object *path_jso = NULL;
    gchar **monitor_create_location_str = get_monitor_create_location_str();

    
    if (NULL == *monitor_create_location_str) {
        return NULL;
    }

    header_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != header_jso, NULL,
        debug_log(DLOG_ERROR, "%s json_object_object_get_ex fail.", __FUNCTION__));

    path_jso = json_object_new_string(*monitor_create_location_str);
    return_val_do_info_if_fail(NULL != path_jso, NULL, json_object_put(header_jso);
        debug_log(DLOG_ERROR, "%s json_object_new_string fail.", __FUNCTION__));

    if (NULL != *monitor_create_location_str) {
        g_free(*monitor_create_location_str);
        *monitor_create_location_str = NULL;
    }

    (void)json_object_object_add(header_jso, RESP_HEADER_LOCATION, path_jso);

    return header_jso;
}


LOCAL gint32 get_task_monitor_body(json_object *task_monitor_body, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    json_object *json_body = NULL;
    json_object *extended_jso = NULL;
    json_object *tmp_jso = NULL;
    const gchar *extended_info_str = NULL;

    
    if (json_object_object_get_ex(task_monitor_body, RF_MONITOR_CREATE_RESOURSE, &json_body)) {
        ret = get_monitor_create_resourse_body(json_body, o_message_jso, o_result_jso);
        return_val_do_info_if_fail(VOS_ERR != ret, VOS_ERR,
            debug_log(DLOG_ERROR, "%s get_monitor_create_volume_body fail.", __FUNCTION__));
    } else {
        (void)create_message_info(MSGID_MONITOR_TASK_COMPLETE, NULL, o_message_jso);
        (void)json_object_object_add(*o_result_jso, RF_TASK_MESSAGES_PROP, *o_message_jso);
        if (json_object_object_get_ex(task_monitor_body, RF_MONITOR_EXTENDED_INFO, &json_body)) {
            if (json_object_object_get_ex(json_body, RF_MONITOR_EXTENDED_FILENAME, &extended_jso)) {
                extended_info_str = dal_json_object_get_str(extended_jso);
                tmp_jso = json_object_from_file(extended_info_str);
#pragma GCC diagnostic push // require GCC 4.6
#pragma GCC diagnostic ignored "-Wcast-qual"
                json_object_object_foreach0(tmp_jso, name, val)
                {
                    json_object_object_add(*o_result_jso, name, json_object_get(val));
                }
#pragma GCC diagnostic pop // require GCC 4.6
                json_object_put(tmp_jso);
            }
        }
        ret = RF_OK;
    }

    return ret;
}


gint32 get_task_monitor_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    gint32 ret;
    gint32 monitor_id = 0;
    json_object *monitor_json = NULL;
    const gchar *string_value = NULL;
    json_bool ret_bool;
    guint8 task_need_priv = 0;
    const gchar *task_owner = NULL;
    json_object *o_message_jso = NULL;

    if (o_rsc_jso == NULL || o_err_array_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = get_task_monitor_id_value(i_paras->uri, &monitor_id);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s get_task_monitor_id_value fail.", __FUNCTION__));

    
    monitor_json = get_task_monitor_need_info(monitor_id, &task_need_priv, &task_owner);
    return_val_do_info_if_fail(NULL != monitor_json, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s get_task_rsc_for_monitor fail.", __FUNCTION__));

    
    if (check_user_priv_for_query_task(i_paras, task_need_priv, task_owner) == FALSE) {
        json_object_put(monitor_json);
        return HTTP_FORBIDDEN;
    }

    
    ret_bool = get_element_str(monitor_json, RF_TASK_STATE_PROP, &string_value);
    return_val_do_info_if_fail(ret_bool, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(monitor_json);
        debug_log(DLOG_ERROR, "%s get_element_str fail.", __FUNCTION__));

    
    if (0 == g_strcmp0(string_value, RF_MONITOR_TASK_EXCEPTION)) {
        
        (void)create_message_info(MSGID_MONITOR_TASK_FAIL, NULL, &o_message_jso);
        (void)json_object_object_add(o_rsc_jso, RF_TASK_MESSAGES_PROP, o_message_jso);
        ret = RF_OK;
    } else if (0 == g_strcmp0(string_value, RF_MONITOR_TASK_RUNNING)) {
        
        (void)create_message_info(MSGID_MONITOR_TASK_RUN, NULL, &o_message_jso);
        (void)json_object_object_add(o_rsc_jso, RF_TASK_MESSAGES_PROP, o_message_jso);
        ret = HTTP_ACCEPTED;
    } else {
        
        ret = get_task_monitor_body(monitor_json, &o_message_jso, &o_rsc_jso);
        return_val_do_info_if_fail(VOS_ERR != ret, RF_RSC_NOT_FOUND, (void)json_object_put(monitor_json);
            debug_log(DLOG_ERROR, "%s get_task_monitor_body fail.", __FUNCTION__));
    }

    (void)json_object_put(monitor_json);

    return ret;
}


gint32 task_monitor_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gint32 monitor_id = 0;
    gint32 ret;
    json_object *json_body = NULL;

    
    return_val_do_info_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count),
        HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: task_monitor_provider_entry input NULL pointer.", __FUNCTION__));

    
    ret = get_task_monitor_id_value(i_paras->uri, &monitor_id);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s get_task_monitor_id_value fail.", __FUNCTION__));

    
    
    json_body = get_task_rsc_from_id(monitor_id, NULL, NULL);
    return_val_do_info_if_fail(NULL != json_body, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s get_task_rsc_from_id fail.", __FUNCTION__));

    (void)json_object_put(json_body);

    *prop_provider = g_task_monitor_rsc_provider;
    
    *count = 0;
    ;

    return VOS_OK;
}
