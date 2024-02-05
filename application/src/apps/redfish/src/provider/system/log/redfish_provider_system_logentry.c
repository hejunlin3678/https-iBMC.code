
#include "redfish_provider.h"
#include "redfish_provider_system.h"

LOCAL SAVE_SEL_DATA get_sel_info = {};
LOCAL gint32 get_logentry_severity(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_logentry_level(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_logentry_eventtimestamp(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_logentry_messageid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_logentry_messageargs(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_logentry_eventid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_logentry_message(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_system_logentry_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_logentry_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_logentry_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_system_logentry_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_logentry_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_logentry_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_logentry_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_logentry_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_NAME, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_logentry_name, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SEL_SEVERITY, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logentry_severity, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_OEM, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logentry_level, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SEL_EVENTTIMESTAMP, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logentry_eventtimestamp, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SEL_MESSAGEID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logentry_messageid, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SEL_MESSAGEARGS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logentry_messageargs, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SEL_EVENTID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logentry_eventid, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SEL_CREATED, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,  get_logentry_eventtimestamp, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SEL_MESSAGE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logentry_message, NULL, NULL, ETAG_FLAG_ENABLE},
};

LOCAL gint32 get_logentry_severity(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 severity = 0;
    gint32 ret;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    return_val_do_info_if_fail(0 != strlen(get_sel_info.severity), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: Attribute has no value. ", __FUNCTION__, __LINE__));
    ret = vos_str2int(get_sel_info.severity, 10, &severity, NUM_TPYE_INT32);
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:vos_str2int fail", __FUNCTION__, __LINE__));

    
    if (0 == severity) {
        *o_result_jso = json_object_new_string(LOGENTRY_SEVERITY_OK);
    } else if (2 == severity || 1 == severity) {
        *o_result_jso = json_object_new_string(LOGENTRY_SEVERITY_WARNING);
    }
    
    else if (3 == severity) {
        *o_result_jso = json_object_new_string(LOGENTRY_SEVERITY_CRITICAL);
    } else {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_string fail", __FUNCTION__, __LINE__));

    return HTTP_OK;
}

LOCAL gint32 get_logentry_level(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 level = 0;
    gint32 ret;
    json_object *huawei = NULL;
    json_object *level_obj = NULL;

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    return_val_do_info_if_fail(0 != strlen(get_sel_info.severity), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: Attribute has no value. ", __FUNCTION__, __LINE__));

    ret = vos_str2int(get_sel_info.severity, 10, &level, NUM_TPYE_INT32);
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:vos_str2int fail", __FUNCTION__, __LINE__));

    huawei = json_object_new_object();
    return_val_do_info_if_fail(NULL != huawei, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:New object fail", __FUNCTION__));

    (void)get_event_level(level, &level_obj);
    return_val_do_info_if_fail(NULL != level_obj, HTTP_INTERNAL_SERVER_ERROR, json_object_put(huawei);
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer of json_object_level.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR, json_object_put(huawei);
        json_object_put(level_obj); debug_log(DLOG_ERROR, "%s:New o_result_jso fail", __FUNCTION__));

    (void)json_object_object_add(huawei, PROPERTY_LOG_LEVEL, level_obj);
    (void)json_object_object_add(*o_result_jso, RFPROP_COMMON_MANUFACTURER, huawei);

    return HTTP_OK;
}


LOCAL gint32 get_logentry_eventtimestamp(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    return_val_do_info_if_fail(0 != strlen(get_sel_info.creat_time), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: Attribute has no value. ", __FUNCTION__, __LINE__));
    *o_result_jso = json_object_new_string(get_sel_info.creat_time);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_string fail", __FUNCTION__, __LINE__));
    return HTTP_OK;
}

LOCAL gint32 get_logentry_messageid(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, json_object** o_result_jso)
{
    OBJ_HANDLE obj_handle;
    guint32 eventid = 0;
    guint32 oldeventid = 0;
    gint32 ret;

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    return_val_do_info_if_fail(0 != strlen(get_sel_info.event_code) || 0 != strlen(get_sel_info.old_event_code),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s,%d: Attribute has no value. ", __FUNCTION__, __LINE__));
    ret = vos_str2int(get_sel_info.event_code, 10, &eventid, NUM_TPYE_UINT32);
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:vos_str2int fail", __FUNCTION__, __LINE__));

    if (eventid != 0) {
        ret = dal_get_specific_object_uint32(CLASS_EVENT_INFORMATION, PROPERTY_EVENT_INFORMATION_EVENT_CODE, eventid,
            &obj_handle);
    } else {
        ret = vos_str2int(get_sel_info.old_event_code, 10, &oldeventid, NUM_TPYE_UINT32);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:vos_str2int fail, ret:%d",  __FUNCTION__, ret);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        ret = dal_get_specific_object_uint32(CLASS_EVENT_INFORMATION, PROPERTY_EVENT_INFORMATION_OLD_EVENT_CODE,
            oldeventid, &obj_handle);
    }

    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_specific_object_uint32 failed, ret:%d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    get_sel_message_id(obj_handle, o_result_jso);
    return HTTP_OK;
}


LOCAL gint32 get_logentry_messageargs(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint32 event_id = 0;
    gint32 arg = 0;
    gint32 i = 0;
    gchar str_arg[SELINFO_DESC_LEN] = {}; // 此处不需要初始化，参数必然是以0xBA开始，while循环检测到0xba后会初始化

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    *o_result_jso = json_object_new_array();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:   memory alloc fail. ", __FUNCTION__, __LINE__));
    return_val_do_info_if_fail(0 != strlen(get_sel_info.event_code), HTTP_OK,
        debug_log(DLOG_ERROR, "%s,%d: Attribute has no value. ", __FUNCTION__, __LINE__));
    ret = vos_str2int(get_sel_info.event_code, 10, &event_id, NUM_TPYE_INT32);
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_OK,
        debug_log(DLOG_ERROR, "%s,%d:vos_str2int fail", __FUNCTION__, __LINE__));
    return_val_if_fail(0 != event_id, HTTP_OK);

    while ('\0' != get_sel_info.event_desc[arg]) {
        if (EVENT_EXT_INFO_SPLIT_CHAR != (guint8)get_sel_info.event_desc[arg]) {
            str_arg[i++] = get_sel_info.event_desc[arg++];
        }

        
        if (EVENT_EXT_INFO_SPLIT_CHAR == (guint8)get_sel_info.event_desc[arg] || '\0' == get_sel_info.event_desc[arg]) {
            if (NULL == strstr(str_arg, EVENT_LANGUAGE_ITEM_NAME_PREFIX) &&
                NULL == strstr(str_arg, EVENT_LANGUAGE_ITEM_NAME_SUFFIX) && 0 != strlen(str_arg)) {
                ret = json_object_array_add(*o_result_jso, json_object_new_string(str_arg));
                do_val_if_expr(VOS_OK != ret,
                    debug_log(DLOG_ERROR, "%s %d:json_object_array_add fail", __FUNCTION__, __LINE__));
            }

            if ('\0' != get_sel_info.event_desc[arg]) {
                arg++;
            }

            i = 0;
            (void)memset_s(str_arg, sizeof(str_arg), 0, sizeof(str_arg));
        }
    }

    return HTTP_OK;
}

LOCAL gint32 get_logentry_eventid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 event_id = 0;
    gint32 event_id_old = 0;
    gchar event_code_str[RFPROP_SEL_EVENTCODELEN] = {0};
    gint32 ret;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    return_val_do_info_if_fail(0 != strlen(get_sel_info.event_code) || 0 != strlen(get_sel_info.event_code),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s,%d: Attribute has no value. ", __FUNCTION__, __LINE__));
    ret = vos_str2int(get_sel_info.event_code, 10, &event_id, NUM_TPYE_INT32);
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:vos_str2int fail", __FUNCTION__, __LINE__));

    if (0 == event_id) {
        ret = vos_str2int(get_sel_info.old_event_code, 10, &event_id_old, NUM_TPYE_INT32);
        return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s,%d:vos_str2int fail", __FUNCTION__, __LINE__));

        ret = snprintf_s(event_code_str, sizeof(event_code_str), sizeof(event_code_str) - 1, "0x%08X", event_id_old);
    } else {
        ret = snprintf_s(event_code_str, sizeof(event_code_str), sizeof(event_code_str) - 1, "0x%08X", event_id);
    }

    return_val_do_info_if_fail(0 < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:snprintf_s fail.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_string(event_code_str);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_string fail", __FUNCTION__, __LINE__));

    return HTTP_OK;
}

LOCAL gint32 get_logentry_message(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 trigger_mode = 0;
    gint32 event_id = 0;
    char *desc_buf = NULL;
    gchar desc_full_buf[EVENT_LANGUAGE_CONTENT_MAX_LENGTH] = {0};
    static xmlDocPtr event_lang_handle = NULL;
    gchar filename[SELINFO_XML_FILENAME_LEN] = {0};
    gint32 ret;
    errno_t safe_fun_ret = EOK;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    return_val_do_info_if_fail(0 != strlen(get_sel_info.trigmode) || 0 != strlen(get_sel_info.event_code),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s,%d: Attribute has no value. ", __FUNCTION__, __LINE__));
    ret = vos_str2int(get_sel_info.trigmode, 10, &trigger_mode, NUM_TPYE_INT32);
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:vos_str2int fail", __FUNCTION__, __LINE__));
    ret = vos_str2int(get_sel_info.event_code, 10, &event_id, NUM_TPYE_INT32);
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:vos_str2int fail", __FUNCTION__, __LINE__));
    
    ret = snprintf_s(filename, sizeof(filename), sizeof(filename) - 1, SELINFI_FILE_PATH, EVENT_LANGUAGE_FILE_PATH,
        EVENT_LANGUAGE_FILE);
    return_val_do_info_if_fail(0 < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:snprintf_s fail.", __FUNCTION__, __LINE__));
    

    
    if (NULL == event_lang_handle) {
        event_lang_handle = dal_open_xml_doc(filename);
        check_fail_do_return_val(NULL != event_lang_handle, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "init xml parser from %s failed", filename));
    }

    
    dal_get_event_desc_prefix(trigger_mode, get_sel_info.eventsubject, event_id, desc_full_buf, sizeof(desc_full_buf));
    
    
    dal_convert_raw_to_formated(event_lang_handle, (guint8 *)get_sel_info.event_desc, strlen(get_sel_info.event_desc),
        EVENT_EXT_INFO_SPLIT_CHAR, EVENT_LANGUAGE_EN, EVENT_LANGUAGE_DESC_NODE, &desc_buf, NULL, get_sel_info.sn,
        get_sel_info.pn);
    
    if (NULL != desc_buf) {
        safe_fun_ret = strncat_s(desc_full_buf, sizeof(desc_full_buf), desc_buf, strlen(desc_buf));
        g_free(desc_buf);
        do_val_if_expr(safe_fun_ret != 0,
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    }

    do_if_expr(NULL == desc_buf, return HTTP_INTERNAL_SERVER_ERROR);
    *o_result_jso = json_object_new_string(desc_full_buf);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_string fail", __FUNCTION__, __LINE__));
    
    return HTTP_OK;
}


LOCAL errno_t copy_non_null_str_to_buffer(gchar *str_dest, size_t dest_buffer_len, gchar *str_src)
{
    gint32 ret = 0;

    if (str_src != NULL) {
        ret = strncpy_s(str_dest, dest_buffer_len, str_src, strlen(str_src));
    }
    // 如果str_src指针为NULL,则将str_dest数组设成全0
    else {
        (void)memset_s(str_dest, dest_buffer_len, 0, dest_buffer_len);
    }

    return ret;
}


LOCAL gint32 save_log_info(gchar **select_result, gint32 column)
{
    errno_t str_ret;
    guint32 time_sel = 0;
    gint32 ret;

    
    str_ret = copy_non_null_str_to_buffer(get_sel_info.severity, sizeof(get_sel_info.severity), select_result[column]);
    return_val_do_info_if_expr(EOK != str_ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s,%d:strncpy_s failed. ", __FUNCTION__, __LINE__));
    str_ret = copy_non_null_str_to_buffer(get_sel_info.subject_type, sizeof(get_sel_info.subject_type),
        select_result[column + 1]);
    return_val_do_info_if_expr(EOK != str_ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s,%d:strncpy_s failed. ", __FUNCTION__, __LINE__));
    str_ret = copy_non_null_str_to_buffer(get_sel_info.event_desc, sizeof(get_sel_info.event_desc),
        select_result[column + 2]);
    return_val_do_info_if_expr(EOK != str_ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s,%d:strncpy_s failed. ", __FUNCTION__, __LINE__));
    str_ret = copy_non_null_str_to_buffer(get_sel_info.event_code, sizeof(get_sel_info.event_code),
        select_result[column + 5]);
    return_val_do_info_if_expr(EOK != str_ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s,%d:strncpy_s failed. ", __FUNCTION__, __LINE__));
    str_ret = copy_non_null_str_to_buffer(get_sel_info.old_event_code, sizeof(get_sel_info.old_event_code),
        select_result[column + 6]);
    return_val_do_info_if_expr(EOK != str_ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s,%d:strncpy_s failed. ", __FUNCTION__, __LINE__));
    
    ret = vos_str2int(select_result[column + 4], 10, &time_sel, NUM_TPYE_UINT32);
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:vos_str2int fail", __FUNCTION__, __LINE__));
    (void)dal_get_redfish_datetime_stamp(time_sel, get_sel_info.creat_time, sizeof(get_sel_info.creat_time));
    
    str_ret =
        copy_non_null_str_to_buffer(get_sel_info.trigmode, sizeof(get_sel_info.trigmode), select_result[column + 3]);
    return_val_do_info_if_expr(EOK != str_ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s,%d:strncpy_s failed. ", __FUNCTION__, __LINE__));
    str_ret = copy_non_null_str_to_buffer(get_sel_info.eventsubject, sizeof(get_sel_info.eventsubject),
        select_result[column + 7]);
    return_val_do_info_if_expr(EOK != str_ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s,%d:strncpy_s failed. ", __FUNCTION__, __LINE__));
    
    str_ret = copy_non_null_str_to_buffer(get_sel_info.sn, sizeof(get_sel_info.sn), select_result[column + 8]);
    return_val_do_info_if_expr(EOK != str_ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s,%d:strncpy_s sn failed. ", __FUNCTION__, __LINE__));
    str_ret = copy_non_null_str_to_buffer(get_sel_info.pn, sizeof(get_sel_info.pn), select_result[column + 9]);
    return_val_do_info_if_expr(EOK != str_ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s,%d:strncpy_s pn failed. ", __FUNCTION__, __LINE__));
    
    

    return VOS_OK;
}

LOCAL gint32 search_log_info(PROVIDER_PARAS_S *i_paras)
{
    gint32 ret = 0;
    gint32 event_id = 0;
    sqlite3 *eo_sqlite_db = NULL;
    gint32 eo_sqlite_lock = -1;
    gchar **select_result = NULL;
    gint32 nrow = 0;
    gint32 ncolumn = 0;
    gchar *errmsg = 0;
    gchar sql_buf[EVENT_LANGUAGE_CONTENT_MAX_LENGTH] = {0};

    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s,%d:get_timezone failed. ", __FUNCTION__, __LINE__));

    
    eo_sqlite_lock = open(EO_SQLITE_SYNC_FILE, O_RDONLY);
    return_val_if_expr(eo_sqlite_lock < 0, VOS_ERR);
    db_sqlite_lock(eo_sqlite_lock);
    (void)db_sqlite_open(EO_SQLITE_DB_FILE, &eo_sqlite_db);
    db_sqlite_unlock(eo_sqlite_lock);
    return_val_do_info_if_expr(0 == eo_sqlite_db, VOS_ERR, close(eo_sqlite_lock));

    ret = vos_str2int(i_paras->member_id, 10, &event_id, NUM_TPYE_INT32);
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR, close(eo_sqlite_lock);
        debug_log(DLOG_ERROR, "%s,%d:vos_str2int fail", __FUNCTION__, __LINE__));
    
    
    ret = snprintf_s(sql_buf, EVENT_LANGUAGE_CONTENT_MAX_LENGTH, EVENT_LANGUAGE_CONTENT_MAX_LENGTH - 1,
        SELINFO_SELECT_SQL, EO_SQLITE_TABLE_NAME, event_id);
    
    
    return_val_do_info_if_fail(0 < ret, HTTP_INTERNAL_SERVER_ERROR, close(eo_sqlite_lock);
        debug_log(DLOG_ERROR, "%s,%d:snprintf_s fail.", __FUNCTION__, __LINE__));
    db_sqlite_lock(eo_sqlite_lock);
    ret = sqlite3_get_table(eo_sqlite_db, sql_buf, &select_result, &nrow, &ncolumn, &errmsg);
    db_sqlite_unlock(eo_sqlite_lock);
    return_val_do_info_if_expr(SQLITE_OK != ret, VOS_ERR, db_sqlite_close(eo_sqlite_db); close(eo_sqlite_lock));
    
    return_val_do_info_if_expr(!redfish_check_system_uri_valid(i_paras->uri) || 0 == nrow, HTTP_NOT_FOUND,
        sqlite3_free_table(select_result);
        db_sqlite_close(eo_sqlite_db); close(eo_sqlite_lock));
    
    ret = save_log_info(select_result, ncolumn);
    return_val_do_info_if_expr(VOS_ERR == ret, VOS_ERR, sqlite3_free_table(select_result);
        db_sqlite_close(eo_sqlite_db); close(eo_sqlite_lock);
        debug_log(DLOG_ERROR, "%s,%d:save_log_info failed. ", __FUNCTION__, __LINE__));
    sqlite3_free_table(select_result);
    db_sqlite_close(eo_sqlite_db);
    close(eo_sqlite_lock);
    return VOS_OK;
}

LOCAL gint32 get_system_logentry_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar log_name[MAX_URI_LENGTH] = {0};
    gint32 ret;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    ret = snprintf_s(log_name, sizeof(log_name), sizeof(log_name) - 1, SYSTEM_LOGENTRY_NAME, i_paras->member_id);
    return_val_do_info_if_fail(0 < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:snprintf_s fail.", __FUNCTION__, __LINE__));
    *o_result_jso = json_object_new_string(log_name);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_string fail", __FUNCTION__, __LINE__));

    return HTTP_OK;
}

LOCAL gint32 get_system_logentry_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    *o_result_jso = json_object_new_string(i_paras->member_id);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_string fail", __FUNCTION__, __LINE__));

    return HTTP_OK;
}

LOCAL gint32 get_system_logentry_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar slot_uri[PSLOT_URI_LEN] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    gint32 ret;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:%d redfish_get_board_slot fail.", __FUNCTION__, __LINE__));
    ret =
        snprintf_s(slot_uri, sizeof(slot_uri), sizeof(slot_uri) - 1, SYSTEM_LOGENTRY_ODATAID, slot, i_paras->member_id);
    return_val_do_info_if_fail(0 < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:snprintf_s fail.", __FUNCTION__, __LINE__));
    *o_result_jso = json_object_new_string(slot_uri);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_string fail", __FUNCTION__, __LINE__));

    return HTTP_OK;
}

LOCAL gint32 get_system_logentry_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar odata_context[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    
    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__));
    ret = snprintf_s(odata_context, sizeof(odata_context), sizeof(odata_context) - 1, SYSTEM_LOGENTRY_CONTEXT, slot_id);
    return_val_do_info_if_fail(0 < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:snprintf_s fail.", __FUNCTION__, __LINE__));
    *o_result_jso = json_object_new_string(odata_context);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_string fail.", __FUNCTION__, __LINE__));

    return HTTP_OK;
}

gint32 system_provider_log_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gint32 ret;
    return_val_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count), HTTP_INTERNAL_SERVER_ERROR);
    
    ret = search_log_info(i_paras);
    return_val_do_info_if_expr(HTTP_NOT_FOUND == ret, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s,%d:URI check failed. ", __FUNCTION__, __LINE__));
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:search_log_info failed. ", __FUNCTION__, __LINE__));
    *prop_provider = g_logentry_provider;
    *count = sizeof(g_logentry_provider) / sizeof(PROPERTY_PROVIDER_S);
    return HTTP_OK;
}
