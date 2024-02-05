


#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include <dirent.h>
#include "redfish_message.h"
#include "redfish_util.h"
#include "json_patch_util.h"

static json_object *g_messages_jso = NULL;


LOCAL gint32 _load_reg_msg_file(const char *reg_file_path, json_object *global_reg_jso)
{
    int iRet;
    json_object *reg_file_jso = json_object_from_file(reg_file_path);
    char msg_id_prefix[MSGID_MAX_LEN] = { 0 };
    
    gchar *dot_index = NULL;
    
    const char *registry_prefix = NULL;
    const char *registry_version = NULL;

    check_fail_do_return_val(reg_file_jso, VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed: get registry file failed", __FUNCTION__));

    (void)get_element_str(reg_file_jso, REGISTRY_PREFIX_KEY, &registry_prefix);
    (void)get_element_str(reg_file_jso, REGISTRY_VERSION_KEY, &registry_version);

    check_fail_do_return_val_spec(registry_prefix && registry_version, VOS_ERR, (void)json_object_put(reg_file_jso),
        debug_log(DLOG_ERROR, "get %s or %s from obj failed", REGISTRY_PREFIX_KEY, REGISTRY_VERSION_KEY));

    iRet = snprintf_s(msg_id_prefix, MSGID_MAX_LEN, MSGID_MAX_LEN - 1, "%s.%s", registry_prefix, registry_version);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet));

    dot_index = g_strrstr(msg_id_prefix, ".");
    check_fail_do_return_val_spec(dot_index, VOS_ERR, (void)json_object_put(reg_file_jso),
        debug_log(DLOG_ERROR, "find dot from %s failed", msg_id_prefix));

    
    *dot_index = '\0';
    
    json_object_object_add(global_reg_jso, msg_id_prefix, reg_file_jso);

    return VOS_OK;
}


LOCAL json_object *_get_global_message(void)
{
    return g_messages_jso;
}


/*lint -save -e438 -e550*/
gint32 find_msg_info(const char *msg_id, char *registry_id, guint32 registry_id_len, json_object **msg_jso)
{
    json_object *registries_jso = _get_global_message();
    json_object_iter iter;
    struct lh_table *json_object_retv = NULL;
    json_object *messages_jso = NULL;
    void *p_iter = NULL;

    check_fail_do_return_val(msg_id && registry_id && msg_jso && registries_jso, VOS_ERR,
        debug_log(DLOG_MASS, "%s failed: input param error", __FUNCTION__));

    json_object_retv = json_object_get_object(registries_jso);
    return_val_if_fail(json_object_retv, VOS_ERR);

#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#endif

    iter.entry = json_object_retv->head;
    for (;
        (p_iter =
             ((iter.entry) ? (iter.key = (char *)iter.entry->k, iter.val = (struct json_object *)iter.entry->v) : 0));
        iter.entry = iter.entry->next) {
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif

        (void)json_object_object_get_ex(iter.val, MESSAGES_KEY, &messages_jso);

        if (json_object_object_get_ex(messages_jso, msg_id, msg_jso)) {
            (void)strncpy_s(registry_id, registry_id_len, iter.key, registry_id_len - 1);

            return VOS_OK;
        }
    }

    debug_log(DLOG_ERROR, "cann't find msg info for %s", msg_id);

    return VOS_ERR;
}
/*lint -restore*/


LOCAL gint32 _get_messages_json_object(const gchar *path, json_object **messages_jso)
{
    errno_t safe_fun_ret = EOK;
    struct dirent *entry = NULL;
    DIR *dirp = NULL;
    char msg_reg_file_name[MAX_FILE_NAME] = { 0 };
    char ext_name[MAX_FILE_NAME] = { 0 };
    int len = 0;
    int i = 0;
    json_object *registries_jso = NULL;

    dirp = opendir(path);
    if (NULL == dirp) {
        debug_log(DLOG_ERROR, "%s : folder not found.", __FUNCTION__);
        return VOS_ERR;
    }

    registries_jso = json_object_new_object();
    if (registries_jso == NULL) {
        debug_log(DLOG_ERROR, "alloc new json object failed");
        (void)closedir(dirp);
        return VOS_ERR;
    }

    while (1) {
        if (NULL == (entry = readdir(dirp))) {
            break;
        }

        len = strlen(entry->d_name);
        if ((len >= MAX_FILE_NAME) || (len < MIN_FILE_NAME)) {
            continue;
        }

        (void)memset_s(ext_name, sizeof(ext_name), 0, sizeof(ext_name));

        for (i = len - 1; i > 0; i--) {
            if (entry->d_name[i] == DOT_CHAR) {
                (void)strncpy_s(ext_name, len, entry->d_name + i + 1, len - i);
                break;
            }
        }

        if (0 == g_strcmp0(ext_name, JSON_FILE_EXT)) {
            (void)strncpy_s(msg_reg_file_name, MAX_FILE_NAME, path, MAX_FILE_NAME - 1);
            safe_fun_ret = strncat_s(msg_reg_file_name, MAX_FILE_NAME, entry->d_name, strlen(entry->d_name));
            return_val_do_info_if_expr(safe_fun_ret != EOK, VOS_ERR, (void)closedir(dirp);
                json_object_put(registries_jso);
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

            if (VOS_OK != _load_reg_msg_file(msg_reg_file_name, registries_jso)) {
                debug_log(DLOG_ERROR, "_load_reg_msg_file failed. filename(%s->%s)", msg_reg_file_name, entry->d_name);
                continue;
            }

            debug_log(DLOG_DEBUG, "%s %d:_load_reg_msg_file . filename(%s)\r\n", __FUNCTION__, __LINE__, entry->d_name);
        }
    }

    (void)closedir(dirp);

    *messages_jso = registries_jso;

    return VOS_OK;
}


void redfish_message_init(void)
{
    gint32 ret;

    if (NULL == g_messages_jso) {
        ret = _get_messages_json_object(REDFISH_MSG_REGISTRY_DIR, &g_messages_jso);
        return_do_info_if_fail(RF_OK == ret,
            debug_log(DLOG_ERROR, "%s : _get_messages_json_object error", __FUNCTION__));
    }

    return;
}


LOCAL gint32 _create_odata_type(json_object **odata_type_jso)
{
    return_val_do_info_if_fail((NULL != odata_type_jso), VOS_ERR,
        debug_log(DLOG_ERROR, "%s input param fail", __FUNCTION__));
    const gchar *odata_type_val_str = "#Message.v1_0_0.Message";
    *odata_type_jso = json_object_new_string(odata_type_val_str);

    return VOS_OK;
}


static char *_limit_string_length(const char *msg_str, const int max_length)
{
    errno_t safe_fun_ret;
    int length = 0;
    char *str_tmp = NULL;
    
    const char *null_str = RF_NULL_STR;
    const char str_end[MAX_MESSAGE_ARGS_LEN] = OMISSION_STR;
    guint32 omission_str_size;

    
    if (NULL == msg_str) {
        str_tmp = (char *)g_malloc(MESSAGE_MAX_LENGTH);
        check_fail_do_return_val(str_tmp, NULL, debug_log(DLOG_ERROR, "str_tmp is null."));

        safe_fun_ret = strcpy_s(str_tmp, MESSAGE_MAX_LENGTH, null_str);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        }

        return str_tmp;
    } else {
        length = strlen((const char *)msg_str);

        str_tmp = (char *)malloc(length + 1);
        check_fail_do_return_val(str_tmp, NULL, debug_log(DLOG_ERROR, "str_tmp is null."));

        safe_fun_ret = strncpy_s(str_tmp, length + 1, msg_str, strlen(msg_str));
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

        if (length < max_length) {
            return str_tmp;
        }

        omission_str_size = strlen(str_end) + 1;
        safe_fun_ret = strncpy_s((str_tmp + max_length - omission_str_size), length + 1, str_end, strlen(str_end));
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
        str_tmp[max_length - 1] = '\0';
    }
    
    return str_tmp;
}


LOCAL gint32 _create_message_id(const gchar *odata_id_str, const gchar *message_id, json_object **message_id_jso)
{
    int iRet;
    gchar message_id_str[MESSAGE_MAX_LENGTH] = { 0 };
    const gchar *dot_str = DOT_STR;

    return_val_do_info_if_fail((NULL != odata_id_str) && (NULL != message_id) && (NULL != message_id_jso), VOS_ERR,
        debug_log(DLOG_ERROR, "%s input param fail", __FUNCTION__));

    iRet = snprintf_s(message_id_str, MESSAGE_MAX_LENGTH, MESSAGE_MAX_LENGTH - 1, "%s%s%s", odata_id_str, dot_str,
        message_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet));

    *message_id_jso = json_object_new_string(message_id_str);

    return VOS_OK;
}


LOCAL gint32 _create_related_property(const gchar *related_value, json_object **relate_property_jso)
{
    int iRet = -1;
    gchar related_property_str[MESSAGE_MAX_LENGTH] = { 0 };

    if (relate_property_jso == NULL) {
        debug_log(DLOG_ERROR, "%s input param fail", __FUNCTION__);
        return RET_ERR;
    }

    *relate_property_jso = json_object_new_array();
    if (*relate_property_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_array fail", __FUNCTION__);
        return RET_ERR;
    }

    if (related_value) {
        const gchar *related_properties_prefix = "#/";
        iRet = snprintf_s(related_property_str, MESSAGE_MAX_LENGTH, MESSAGE_MAX_LENGTH - 1, "%s%s",
            related_properties_prefix, related_value);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet));

        (void)json_object_array_add(*relate_property_jso, json_object_new_string(related_property_str));
    }

    return VOS_OK;
}


LOCAL gint32 _get_severity(json_object *messages_jso, json_object **severity_jso)
{
    

    json_bool json_object_ret; // 新增加

    

    return_val_do_info_if_fail((NULL != messages_jso) && (NULL != severity_jso), VOS_ERR,
        debug_log(DLOG_ERROR, "%s input param fail", __FUNCTION__));

    

    // 修改意见: 获取json_object_object_get_ex()的返回值，并进行判断。为false 返回VOS_ERR
    json_object_ret = json_object_object_get_ex(messages_jso, SEVERITY_KEY, severity_jso);
    return_val_do_info_if_fail(json_object_ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: ret:%d 'get json_object_object_get_ex fail' ", __FUNCTION__, VOS_ERR));

    

    *severity_jso = json_object_get(*severity_jso);
    return VOS_OK;
}


LOCAL gint32 _get_resolution(json_object *messages_jso, json_object **resolution_jso)
{
    

    json_bool json_object_ret; // 新增加

    

    return_val_do_info_if_fail((NULL != messages_jso) && (NULL != resolution_jso), VOS_ERR,
        debug_log(DLOG_ERROR, "%s input param fail", __FUNCTION__));

    

    // 修改意见: 获取json_object_object_get_ex()的返回值，并进行判断。为false 返回VOS_OK
    json_object_ret = json_object_object_get_ex(messages_jso, RESOLUTION_KEY, resolution_jso);
    return_val_do_info_if_fail(json_object_ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: ret:%d 'get json_object_object_get_ex fail' ", __FUNCTION__, VOS_ERR));

    

    *resolution_jso = json_object_get(*resolution_jso);
    return VOS_OK;
}


LOCAL gint32 _converge_message(json_object *odata_type_val_jso, json_object *message_id_val_jso,
    json_object *related_properties_val_jso, json_object *message_val_jso, json_object *message_args_val_jso,
    json_object *severity_val_jso, json_object *resolution_val_jso, json_object **message_info_jso)
{
    return_val_do_info_if_fail((NULL != message_info_jso), VOS_ERR,
        debug_log(DLOG_ERROR, "%s input param fail", __FUNCTION__));
    *message_info_jso = json_object_new_object();
    
    return_val_do_info_if_fail((NULL != *message_info_jso), VOS_ERR,
        debug_log(DLOG_ERROR, "%s message_info_jso fail", __FUNCTION__));
    

    json_object_object_add(*message_info_jso, ODATA_TYPE_KEY, odata_type_val_jso);
    json_object_object_add(*message_info_jso, MESSAGE_ID_KEY, message_id_val_jso);
    json_object_object_add(*message_info_jso, RELATED_PROP_KEY, related_properties_val_jso);
    json_object_object_add(*message_info_jso, MESSAGE_KEY, message_val_jso);
    json_object_object_add(*message_info_jso, MESSAGE_ARGS_KEY, message_args_val_jso);
    json_object_object_add(*message_info_jso, SEVERITY_KEY, severity_val_jso);
    json_object_object_add(*message_info_jso, RESOLUTION_KEY, resolution_val_jso);

    return VOS_OK;
}


LOCAL void format_nth_message_arg(gchar *str_dest, guint32 dest_max, const gchar *arg_str, gint32 pos)
{
    errno_t safe_fun_ret = EOK;
    gchar pos_buf[MAX_NUM_BUF_LEN + 1] = {0};
    // 拼接Message中的参数字符串%n
    (void)snprintf_s(pos_buf, sizeof(pos_buf), sizeof(pos_buf) - 1, "%%%d", pos);

    gchar* format_str = g_strdup(str_dest);
    if (format_str == NULL) {
        debug_log(DLOG_ERROR, "%s: dup string failed", __FUNCTION__);
        return;
    }

    gchar *index_str = g_strstr_len(format_str, strlen(format_str), pos_buf);
    if (index_str == NULL) {
        debug_log(DLOG_ERROR, "%s:find %s from %s failed", __FUNCTION__, pos_buf, format_str);
        goto exit;
    }

    *index_str = '\0';

    (void)memset_s(str_dest, dest_max, 0, dest_max);
    safe_fun_ret = strncat_s(str_dest, dest_max, format_str, strlen(format_str));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s message head fail %d", __FUNCTION__, safe_fun_ret);
        goto exit;
    }

    safe_fun_ret = strncat_s(str_dest, dest_max, arg_str, strlen(arg_str));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s arg_str fail %d", __FUNCTION__, safe_fun_ret);
        goto exit;
    }

    gchar *src = index_str + strlen(pos_buf);
    safe_fun_ret = strncat_s(str_dest, dest_max, src, strlen(src));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s message tail fail %d", __FUNCTION__, safe_fun_ret);
        goto exit;
    }

exit:
    g_free(format_str);
    return;
}


LOCAL gint32 _create_message(json_object *message_registry, json_object **message_jso, va_list args)
{
    errno_t safe_fun_ret;
    
    gint32 num_args;
    json_object *num_args_jso = NULL;
    json_object *base_message_val_jso = NULL;
    const gchar *base_message_val_str = NULL;
    gchar message_val_str[MESSAGE_MAX_LENGTH] = { 0 };
    int arg_index;
    guint8 software_type = 0;
    

    json_bool json_object_ret; // 新增加

    

    

    // 修改意见: 获取json_object_object_get_ex()的返回值，并进行判断。为false 返回VOS_ERR
    json_object_ret = json_object_object_get_ex(message_registry, NUMBER_OF_ARGS_KEY, &num_args_jso);
    return_val_do_info_if_fail(json_object_ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: ret:%d 'get json_object_object_get_ex fail' ", __FUNCTION__, VOS_ERR));

    

    num_args = json_object_get_int(num_args_jso);

    

    // 修改意见: 获取json_object_object_get_ex()的返回值，并进行判断。为false 返回VOS_ERR
    json_object_ret = json_object_object_get_ex(message_registry, MESSAGE_KEY, &base_message_val_jso);
    return_val_do_info_if_fail(json_object_ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: ret:%d 'get json_object_object_get_ex fail' ", __FUNCTION__, VOS_ERR));

    
    base_message_val_str = dal_json_object_get_str(base_message_val_jso);
    safe_fun_ret =
        strncpy_s(message_val_str, sizeof(message_val_str), base_message_val_str, sizeof(message_val_str) - 1);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

    
    for (arg_index = 0; arg_index < num_args; arg_index++) {
        const gchar *arg_str = NULL;
        arg_str = (const gchar *)va_arg(args, gchar *);
        gchar *format_str = NULL;

        (void)dal_get_software_type(&software_type);
        if (MGMT_SOFTWARE_TYPE_EM == software_type) { 
            format_str = _limit_string_length(arg_str, MAX_PROP_VAL_LEN_EM);
        } else {
            format_str = _limit_string_length(arg_str, MAX_MESSAGE_ARGS_LEN);
        }

        if (format_str != NULL) {
            (void)format_nth_message_arg(message_val_str, MESSAGE_MAX_LENGTH, format_str, arg_index + 1);
            g_free(format_str);
        }
    }

    *message_jso = json_object_new_string(message_val_str);

    return VOS_OK;
}


LOCAL void _create_args_array(json_object *message_registry, json_object **message_args_val_jso, va_list args)
{
    gint32 num_args;
    json_object *num_args_jso = NULL;
    gint32 arg_index;
    gint32 ret = 0;
    
    json_bool json_object_ret; // 新增加
    

    json_object *str_jso = NULL;
    guint8 software_type = 0;

    

    json_object_ret = json_object_object_get_ex(message_registry, NUMBER_OF_ARGS_KEY, &num_args_jso);
    return_do_info_if_fail(json_object_ret,
        debug_log(DLOG_ERROR, "%s %d : get json_object_object_get_ex fail ", __FUNCTION__, __LINE__));

    

    num_args = json_object_get_int(num_args_jso);

    *message_args_val_jso = json_object_new_array();
    
    return_do_info_if_fail(*message_args_val_jso,
        debug_log(DLOG_ERROR, "%s %d: json_object_new_array fail", __FUNCTION__, __LINE__));

    for (arg_index = 0; arg_index < num_args; arg_index++) {
        const gchar *arg_str = NULL;
        gchar *format_str = NULL;

        arg_str = (const gchar *)va_arg(args, gchar *);
        
        (void)dal_get_software_type(&software_type);
        if (MGMT_SOFTWARE_TYPE_EM == software_type) { 
            format_str = _limit_string_length(arg_str, MAX_PROP_VAL_LEN_EM);
        } else
        
        {
            format_str = _limit_string_length(arg_str, MAX_MESSAGE_ARGS_LEN);
        }
        str_jso = json_object_new_string(format_str);

        ret = json_object_array_add(*message_args_val_jso, str_jso);
        do_val_if_fail(0 == ret, (void)json_object_put(str_jso));

        g_free(format_str);
    }

    return;
}


gint32 create_message_vinfo(json_object **message_info_jso, const gchar *message_id, const gchar *related_property,
    va_list argv)
{
    json_object *base_message_jso = NULL;
    json_object *odata_type_val_jso = NULL;
    json_object *message_id_val_jso = NULL;
    json_object *related_properties_val_jso = NULL;
    json_object *message_val_jso = NULL;
    json_object *message_args_val_jso = NULL;
    json_object *severity_val_jso = NULL;
    json_object *resolution_val_jso = NULL;
    char registry_id[MSGID_MAX_LEN] = { 0 };

    /*lint -save -e438 pclint*/
    va_list args;

    if (NULL == message_id) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return VOS_ERR;
    }

    gint32 ret = find_msg_info(message_id, registry_id, MSGID_MAX_LEN, &base_message_jso);
    check_fail_do_return_val(RF_OK == ret, RF_FAILED, debug_log(DLOG_ERROR, "cann't find message %s", message_id));

    
    ret = _create_odata_type(&odata_type_val_jso);
    do_val_if_expr(VOS_OK != ret, debug_log(DLOG_ERROR, "%s: ret:%d _create_odata_type fail", __FUNCTION__, ret);
        goto ERR_EXIT);

    ret = _create_message_id(registry_id, message_id, &message_id_val_jso);
    do_val_if_expr(VOS_OK != ret, debug_log(DLOG_ERROR, "%s: ret:%d _create_message_id fail", __FUNCTION__, ret);
        goto ERR_EXIT);

    ret = _create_related_property(related_property, &related_properties_val_jso);
    do_val_if_expr(VOS_OK != ret, debug_log(DLOG_ERROR, "%s: ret:%d _create_related_property fail", __FUNCTION__, ret);
        goto ERR_EXIT);

    errno_t safe_fun_ret = memmove_s(&args, sizeof(va_list), &argv, sizeof(va_list));
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        ret = (gint32)safe_fun_ret; goto ERR_EXIT);

    ret = _create_message(base_message_jso, &message_val_jso, args);
    va_end(args);
    do_val_if_expr(VOS_OK != ret, debug_log(DLOG_ERROR, "%s: ret:%d _create_message fail", __FUNCTION__, ret);
        goto ERR_EXIT);

    safe_fun_ret = memmove_s(&args, sizeof(va_list), &argv, sizeof(va_list));
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        ret = (gint32)safe_fun_ret; goto ERR_EXIT);

    _create_args_array(base_message_jso, &message_args_val_jso, args);
    va_end(args);

    ret = _get_severity(base_message_jso, &severity_val_jso);
    do_val_if_expr(VOS_OK != ret, debug_log(DLOG_ERROR, "%s: ret:%d _get_severity fail", __FUNCTION__, ret);
        goto ERR_EXIT);

    ret = _get_resolution(base_message_jso, &resolution_val_jso);
    do_val_if_expr(VOS_OK != ret, debug_log(DLOG_ERROR, "%s: ret:%d _get_resolution fail", __FUNCTION__, ret);
        goto ERR_EXIT);

    
    ret = _converge_message(odata_type_val_jso, message_id_val_jso, related_properties_val_jso, message_val_jso,
        message_args_val_jso, severity_val_jso, resolution_val_jso, message_info_jso);
    do_val_if_expr(VOS_OK != ret, debug_log(DLOG_ERROR, "%s: ret:%d _converge_message fail", __FUNCTION__, ret);
        goto ERR_EXIT);

    if (strcmp(message_id, MSGID_PASSWORD_NEED_RESET) == 0) {
        json_object_object_add(*message_info_jso, "PasswordChangeRequired", json_object_new_boolean(TRUE));
    }

    return VOS_OK;

ERR_EXIT:

    
    (void)json_object_put(odata_type_val_jso);
    (void)json_object_put(message_id_val_jso);
    (void)json_object_put(related_properties_val_jso);
    (void)json_object_put(message_val_jso);
    (void)json_object_put(message_args_val_jso);
    (void)json_object_put(severity_val_jso);
    (void)json_object_put(resolution_val_jso);

    return ret;
    
    /*lint -restore*/
}


gint32 create_message_info(const gchar *message_id, const gchar *related_property, json_object **message_info_jso, ...)
{
    gint32 ret;
    /*lint -save -e438 pclint*/
    va_list args;

    if (NULL == message_id || NULL == message_info_jso) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return VOS_ERR;
    }
    va_start(args, message_info_jso);
    ret = create_message_vinfo(message_info_jso, message_id, related_property, args);
    va_end(args);

    return ret;
}


gint32 generate_error_message_info(json_object *extend_jso, json_object **err_message_jso)
{
    return_val_if_fail((NULL != extend_jso && NULL != err_message_jso), VOS_ERR);

    if (json_object_array_length(extend_jso) > 0) {
        json_object *error_body_jso = json_object_new_object();
        

        return_val_do_info_if_fail((NULL != error_body_jso), VOS_ERR,
            debug_log(DLOG_ERROR, "%s memory allocation fail", __FUNCTION__));
        
        json_object_object_add(error_body_jso, CODE_KEY, json_object_new_string(CODE_VAL_GENERAL_STRING));
        json_object_object_add(error_body_jso, T_MESSAGE_KEY, json_object_new_string(MESSAGE_VAL_GENERAL_STRING));
        json_object_object_add(error_body_jso, EXTENDED_INFO_KEY, json_object_get(extend_jso)); // 需要增加引用计数

        *err_message_jso = json_object_new_object();
        json_object_object_add(*err_message_jso, ERROR_KEY, error_body_jso);
        debug_log(DLOG_DEBUG, "%s : err message info: %s", __FUNCTION__, dal_json_object_get_str(*err_message_jso));
        return VOS_OK;
    }

    return VOS_ERR;
}


char *get_single_message_str(json_object *single_err_jso)
{
    json_object *ext_info = NULL;
    json_object *err_jso = NULL;
    gint32 ret;
    char *err_str = NULL;

    check_fail_return_val(single_err_jso, NULL);

    ext_info = json_object_new_array();
    
    (void)json_object_array_add(ext_info, json_object_get(single_err_jso));

    ret = generate_error_message_info(ext_info, &err_jso);
    check_fail_do_return_val_spec(RF_OK == ret, NULL, debug_log(DLOG_ERROR, "create error message failed."),
        (void)json_object_put(ext_info));

    err_str = g_strdup(dal_json_object_get_str(err_jso));

    (void)json_object_put(err_jso);

    return err_str;
}


gint32 check_event_msg_id_validity(const gchar *msg_id)
{
    errno_t safe_fun_ret;
    gchar *dot_flag_index = NULL;
    gchar msg_id_dup[MAX_MSG_ID_LEN] = { 0 };
    gint32 ret;
    gchar registry_id[MSGID_MAX_LEN] = { 0 };
    json_object *msg_jso = NULL;

    return_val_do_info_if_fail((NULL != msg_id), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    
    if (FALSE == g_str_has_prefix(msg_id, EVT_REGISTRY_PREFIX)) {
        debug_log(DLOG_ERROR, "unsupported event message id:%s", msg_id);

        return VOS_ERR;
    }
    

    safe_fun_ret = strncpy_s(msg_id_dup, sizeof(msg_id_dup), msg_id, sizeof(msg_id_dup) - 1);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

    dot_flag_index = g_strrstr(msg_id_dup, DOT_STR);
    return_val_do_info_if_fail((NULL != dot_flag_index), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed:cann't find dot index for %s", __FUNCTION__, msg_id_dup));

    *dot_flag_index = '\0';

    
    ret = find_msg_info(dot_flag_index + 1, registry_id, MSGID_MAX_LEN, &msg_jso);
    
    if ((VOS_OK != ret) || (0 != g_strcmp0(registry_id, msg_id_dup))) {
        debug_log(DLOG_ERROR, "cann't find message info for %s", msg_id);

        return VOS_ERR;
    }

    return VOS_OK;
}


gint32 get_ret_message(gint32 ret_val, RET_MSG_INFO_MAP *info_map, gsize map_size, json_object *msg_array)
{
    json_object *ld_message = NULL;
    gint32 response;

    if (info_map == NULL || map_size == 0 || msg_array == NULL) {
        debug_log(DLOG_ERROR, "%s:input param error", __FUNCTION__);
        return HTTP_BAD_REQUEST;
    }

    guint32 i;
    for (i = 0; i < map_size; i++) {
        if (ret_val != info_map[i].ret_val) {
            continue;
        }

        if (create_message_info(info_map[i].msg_id, info_map[i].prop_name, &ld_message, info_map[i].param1,
            info_map[i].param2, info_map[i].param3) != RET_OK) {
            debug_log(DLOG_ERROR, "%s:create_message_info fail, prop-%s, param1-%s, param2-%s, param3-%s", __FUNCTION__,
                info_map[i].prop_name, info_map[i].param1, info_map[i].param2, info_map[i].param3);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        break;
    }

    if (i == map_size) {
        if (create_message_info(MSGID_INTERNAL_ERR, NULL, &ld_message) != RET_OK) {
            debug_log(DLOG_ERROR, "%s:create_message_info fail", __FUNCTION__);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        response = HTTP_INTERNAL_SERVER_ERROR;
    } else {
        response = info_map[i].response_code;
    }

    if ((json_object_array_add(msg_array, ld_message)) != RET_OK) {
        (void)json_object_put(ld_message);
        debug_log(DLOG_ERROR, "%s:json_object_array_add fail", __FUNCTION__);
    }

    return response;
}

