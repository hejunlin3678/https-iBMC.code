
#include "redfish_provider.h"
#include "redfish_provider_system.h"


#define SEL_LOG_PARAM_SEVERITY_INFO "Informational"
#define SEL_LOG_PARAM_SEVERITY_MINOR "Minor"
#define SEL_LOG_PARAM_SEVERITY_MAJOR "Major"
#define SEL_LOG_PARAM_SEVERITY_CRITICAL "Critical"


LOCAL gint32 get_logservice_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_logservice_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_logservice_max_log_records(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_logservice_enable(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
gint32 get_logservice_datetime(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
gint32 get_logservice_time_offset(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 set_logservice_time_offset(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 act_logservice_clear_log(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_logservice_entries_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_logservice_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_logservice_act_message(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_logservice_action_info_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_logservice_collect_sel_action_info_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 act_logservice_collect_sel(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_logservice_query_sel_log_action_info_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 act_logservice_query_sel_log(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);


LOCAL gboolean get_system_health_status(void);
LOCAL void get_system_health_events(json_object **json_array, gint32 event_from);
LOCAL gint32 get_system_object_type_list(json_object **json_object_array);
LOCAL void get_event_time(guint32 time, json_object **json_object_time);
LOCAL void get_event_severity(guint8 level, json_object **json_object_level);
LOCAL void get_event_code(guint32 code, json_object **json_object_code);
LOCAL void get_message_id(guint32 code, json_object **json_object_message_id);

LOCAL PROPERTY_PROVIDER_S g_system_logservice_provider[] = {
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MAX_LOG_RECORDS, CLASS_SEL, PROPERTY_SEL_MAX_NUM, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_max_log_records, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_LOGSERVICE_ENABLE, CLASS_SEL, RFPROP_SEL_ENANLE, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_logservice_enable, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_LOGSERVICE_DATE_TIME, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_datetime, NULL, NULL, ETAG_FLAG_DISABLE},
    {RFPROP_LOGSERVICE_TIME_OFFSET, CLASS_BMC, PROPERTY_BMC_TIMEZONE_STR, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_logservice_time_offset, set_logservice_time_offset, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ACTIONS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_act_message, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_LOGSERVICE_CLEARLOG_METHOD, CLASS_SEL, METHOD_SEL_CLEAR_SEL, USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID, NULL, NULL, act_logservice_clear_log, ETAG_FLAG_ENABLE},
    {RFPROP_LOGSERVICE_ENTYIES_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_entries_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_OEM, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_oem, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_LOGSERVICE_OEM_HUWEI_ACTION_COLLECT_SEL, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, NULL, NULL, act_logservice_collect_sel, ETAG_FLAG_ENABLE},
    {RFPROP_LOGSERVICE_OEM_HUWEI_ACTION_QUERY_SEL_LOG, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, NULL, NULL, act_logservice_query_sel_log, ETAG_FLAG_ENABLE}
};

LOCAL PROPERTY_PROVIDER_S g_system_logservice_collect_sel_actioninfo_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_collect_sel_action_info_odata_id, NULL, NULL, ETAG_FLAG_ENABLE}
};

LOCAL gint32 get_logservice_collect_sel_action_info_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar action_odata_id[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};

    
    ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_if_fail(VOS_OK == ret, ret);

    
    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s redfish_get_board_slot fail.", __FUNCTION__));

    
    ret = snprintf_s(action_odata_id, sizeof(action_odata_id), sizeof(action_odata_id) - 1,
        RF_SYSTEM_LOGSERVICE_LOG1_OEM_ACTION_COLLECT_SEL_ACTION_INFO, slot_id);
    return_val_do_info_if_fail(ERROR_SNPRINTF < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:snprintf_s fail.", __FUNCTION__));

    *o_result_jso = json_object_new_string(action_odata_id);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail.", __FUNCTION__));

    return HTTP_OK;
}

LOCAL PROPERTY_PROVIDER_S g_system_logservice_data_actioninfo_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_action_info_odata_id, NULL, NULL, ETAG_FLAG_ENABLE}
};


LOCAL gint32 get_logservice_action_info_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar action_odata_id[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};

    
    ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_if_fail(VOS_OK == ret, ret);

    
    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s redfish_get_board_slot fail.", __FUNCTION__));

    
    ret = snprintf_s(action_odata_id, sizeof(action_odata_id), sizeof(action_odata_id) - 1,
        RFPROP_LOGSERVICE_ACTION_ODATA_ID, slot_id);
    return_val_do_info_if_fail(ERROR_SNPRINTF < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:snprintf_s fail.", __FUNCTION__));

    *o_result_jso = json_object_new_string(action_odata_id);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail.", __FUNCTION__));

    return HTTP_OK;
}


LOCAL PROPERTY_PROVIDER_S g_system_logservice_query_sel_log_actioninfo_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_query_sel_log_action_info_odata_id, NULL, NULL, ETAG_FLAG_ENABLE}
};



LOCAL gint32 get_logservice_query_sel_log_action_info_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar action_odata_id[MAX_URI_LENGTH + 1] = {0};
    gchar slot_id[MAX_RSC_ID_LEN + 1] = {0};

    
    ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_if_fail(VOS_OK == ret, ret);

    
    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: redfish_get_board_slot fail, ret is %d.", __func__, ret));

    
    ret = snprintf_s(action_odata_id, sizeof(action_odata_id), sizeof(action_odata_id) - 1,
        RFPROP_LOGSERVICE_QUERY_SEL_ACTION_ODATA_ID, slot_id);
    return_val_do_info_if_fail(ERROR_SNPRINTF < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret is %d.", __func__, ret));

    *o_result_jso = json_object_new_string(action_odata_id);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_string fail, then o_result_json is null.", __func__));

    return HTTP_OK;
}


LOCAL gint32 get_logservice_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar odata_context[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};

    
    ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_if_fail(VOS_OK == ret, ret);

    
    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s redfish_get_board_slot fail.", __FUNCTION__));

    
    ret =
        snprintf_s(odata_context, sizeof(odata_context), sizeof(odata_context) - 1, RFPROP_LOGSERVICE_CONTEXT, slot_id);
    return_val_do_info_if_fail(ERROR_SNPRINTF < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:snprintf_s fail.", __FUNCTION__));

    *o_result_jso = json_object_new_string(odata_context);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail.", __FUNCTION__));

    return HTTP_OK;
}


LOCAL gint32 get_logservice_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar odata_id[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};

    
    ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_if_fail(VOS_OK == ret, ret);

    
    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail.", __FUNCTION__));

    
    ret = snprintf_s(odata_id, sizeof(odata_id), sizeof(odata_id) - 1, RFPROP_LOGSERVICE_ODATA_ID, slot_id);
    return_val_do_info_if_fail(ERROR_SNPRINTF < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:snprintf_s fail.", __FUNCTION__));

    *o_result_jso = json_object_new_string(odata_id);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail.", __FUNCTION__));

    return HTTP_OK;
}


LOCAL gint32 get_logservice_entries_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar odata_id[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};
    json_object *obj_json = NULL;

    
    ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_if_fail(VOS_OK == ret, ret);

    
    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail.\n", __FUNCTION__));

    
    ret = snprintf_s(odata_id, sizeof(odata_id), sizeof(odata_id) - 1, RFPROP_ENTRIES_ODATA_ID, slot_id);
    return_val_do_info_if_fail(ERROR_SNPRINTF < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:snprintf_s fail.", __FUNCTION__));

    obj_json = json_object_new_string(odata_id);
    return_val_do_info_if_fail(NULL != obj_json, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail.\n", __FUNCTION__));

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR, json_object_put(obj_json);
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail.\n", __FUNCTION__));

    (void)json_object_object_add(*o_result_jso, ODATA_ID, obj_json);

    return HTTP_OK;
}


LOCAL gint32 get_logservice_max_log_records(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint16 out_val = 0;

    
    ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_if_fail(VOS_OK == ret, ret);

    
    ret = dal_get_object_handle_nth(g_system_logservice_provider[i_paras->index].pme_class_name, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s dal_get_object_handle_nth fail.\n", __FUNCTION__));

    
    ret =
        dal_get_property_value_uint16(obj_handle, g_system_logservice_provider[i_paras->index].pme_prop_name, &out_val);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s dal_get_property_value_uint32 fail.\n", __FUNCTION__));

    *o_result_jso = json_object_new_int(out_val);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s json_object_new_int fail.\n", __FUNCTION__));

    return HTTP_OK;
}


LOCAL gint32 get_system_logservice_enable(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint8 out_val = 0;
    OBJ_HANDLE obj_handle = 0;

    
    ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_if_fail(VOS_OK == ret, ret);

    
    ret = dal_get_object_handle_nth(g_system_logservice_provider[i_paras->index].pme_class_name, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s dal_get_object_handle_nth fail.\n", __FUNCTION__));

    
    ret = dal_get_property_value_byte(obj_handle, g_system_logservice_provider[i_paras->index].pme_prop_name, &out_val);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s dal_get_property_value_byte fail.\n", __FUNCTION__));

    
    if ((SYETEM_SEL_LOG_ENABLE == out_val) || (SYSTEM_SEL_LOG_DISABLE == out_val)) {
        *o_result_jso = json_object_new_boolean((json_bool)out_val);
        return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s json_object_new_boolean fail.\n", __FUNCTION__));
    } else {
        debug_log(DLOG_DEBUG, "%s:invalid logservice enable value is %d.", __FUNCTION__, out_val);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


gint32 get_logservice_datetime(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;

    
    ret = get_manager_datetime(i_paras, o_message_jso, o_result_jso);
    return_val_do_info_if_fail(HTTP_OK == ret, ret,
        debug_log(DLOG_ERROR, "%s get log service date time failed.", __FUNCTION__));

    return HTTP_OK;
}


gint32 get_logservice_time_offset(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;

    
    ret = get_manager_timezone(i_paras, o_message_jso, o_result_jso);
    return_val_do_info_if_fail(HTTP_OK == ret, ret,
        debug_log(DLOG_ERROR, "%s get log service time zone offset  failed.", __FUNCTION__));

    return HTTP_OK;
}


LOCAL gint32 set_logservice_time_offset(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    
    ret = set_manager_timezone(i_paras, o_message_jso, NULL);
    return_val_do_info_if_fail(HTTP_OK == ret, ret,
        debug_log(DLOG_ERROR, "%s set log service time zone offset  failed.", __FUNCTION__));

    return HTTP_OK;
}


LOCAL gint32 get_logservice_oem_huawei_action(json_object *obj_action)
{
    int iRet;
    gint32 ret;
    gchar slot_id[MAX_RSC_ID_LEN] = {0};
    gchar uri_str[MAX_CHARACTER_NUM] = {0};
    json_object *oem = NULL;
    json_object *huawei = NULL;
    json_object *action_collect_sel = NULL;
    json_object *action_query_sel_log = NULL;

    return_val_if_expr(NULL == obj_action, VOS_ERR);

    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    return_val_if_expr(VOS_OK != ret, VOS_ERR);

    oem = json_object_new_object();
    return_val_if_expr(NULL == oem, VOS_ERR);
    json_object_object_add(obj_action, RFPROP_COMMON_OEM, oem);

    huawei = json_object_new_object();
    return_val_if_expr(NULL == huawei, VOS_ERR);
    json_object_object_add(oem, RFPROP_COMMON_HUAWEI, huawei);

    
    action_collect_sel = json_object_new_object();
    return_val_if_expr(NULL == action_collect_sel, VOS_ERR);
    json_object_object_add(huawei, RFPROP_LOGSERVICE_ACTION_COLLECT_SEL, action_collect_sel);

    iRet = snprintf_s(uri_str, MAX_CHARACTER_NUM, MAX_CHARACTER_NUM - 1,
        RF_SYSTEM_LOGSERVICE_LOG1_OEM_ACTION_COLLECT_SEL_TARGET, slot_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "snprintf_s fail, ret = %d", iRet));
    (void)json_object_object_add(action_collect_sel, RFPROP_TARGET, json_object_new_string(uri_str));
    (void)memset_s(uri_str, MAX_CHARACTER_NUM, 0, MAX_CHARACTER_NUM);

    iRet = snprintf_s(uri_str, MAX_CHARACTER_NUM, MAX_CHARACTER_NUM - 1,
        RF_SYSTEM_LOGSERVICE_LOG1_OEM_ACTION_COLLECT_SEL_ACTION_INFO, slot_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "snprintf_s fail, ret = %d", iRet));
    (void)json_object_object_add(action_collect_sel, RFPROP_ACTION_INFO, json_object_new_string(uri_str));
    (void)memset_s(uri_str, MAX_CHARACTER_NUM, 0, MAX_CHARACTER_NUM);

    
    
    action_query_sel_log = json_object_new_object();
    return_val_if_expr(NULL == action_query_sel_log, VOS_ERR);
    json_object_object_add(huawei, RFPROP_LOGSERVICE_ACTION_QUERY_SEL_LOG, action_query_sel_log);

    iRet = snprintf_s(uri_str, MAX_CHARACTER_NUM, MAX_CHARACTER_NUM - 1,
        RF_SYSTEM_LOGSERVICE_LOG1_OEM_ACTION_QUERY_SEL_LOG_TARGET, slot_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "snprintf_s fail, ret = %d", iRet));
    (void)json_object_object_add(action_query_sel_log, RFPROP_TARGET, json_object_new_string(uri_str));
    (void)memset_s(uri_str, MAX_CHARACTER_NUM, 0, MAX_CHARACTER_NUM);

    iRet = snprintf_s(uri_str, MAX_CHARACTER_NUM, MAX_CHARACTER_NUM - 1,
        RF_SYSTEM_LOGSERVICE_LOG1_OEM_ACTION_QUERY_SEL_LOG_ACTION_INFO, slot_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "snprintf_s fail, ret = %d", iRet));
    (void)json_object_object_add(action_query_sel_log, RFPROP_ACTION_INFO, json_object_new_string(uri_str));
    

    return VOS_OK;
}


LOCAL gint32 get_logservice_act_message(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar slot_id[MAX_RSC_ID_LEN] = {0};
    gchar act_info[MAX_URI_LENGTH] = {0};
    gchar target_info[MAX_URI_LENGTH] = {0};
    json_object *obj_clear_log = NULL;
    json_object *obj_target_info = NULL;
    json_object *obj_act_info = NULL;

    
    ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_if_fail(VOS_OK == ret, ret);

    
    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s redfish_get_board_slot fail.", __FUNCTION__));

    
    ret = snprintf_s(target_info, sizeof(target_info), sizeof(target_info) - 1, RFPROP_LOGSERVICE_TARGET_ID, slot_id);
    return_val_do_info_if_fail(ERROR_SNPRINTF < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s snprintf_s fail.", __FUNCTION__));

    obj_target_info = json_object_new_string(target_info);
    return_val_do_info_if_fail(NULL != obj_target_info, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s json_object_new_string fail.", __FUNCTION__));

    
    ret = snprintf_s(act_info, sizeof(act_info), sizeof(act_info) - 1, RFPROP_LOGSERVICE_ACTIONINFO_ID, slot_id);
    return_val_do_info_if_fail(ERROR_SNPRINTF < ret, HTTP_INTERNAL_SERVER_ERROR, json_object_put(obj_target_info);
        debug_log(DLOG_ERROR, "%s: snprintf_s fail..", __FUNCTION__));

    obj_act_info = json_object_new_string(act_info);
    return_val_do_info_if_fail(NULL != obj_act_info, HTTP_INTERNAL_SERVER_ERROR, json_object_put(obj_target_info);
        debug_log(DLOG_ERROR, "%s json_object_new_string fail.", __FUNCTION__));

    
    obj_clear_log = json_object_new_object();
    return_val_do_info_if_fail(NULL != obj_clear_log, HTTP_INTERNAL_SERVER_ERROR, json_object_put(obj_target_info);
        json_object_put(obj_act_info); debug_log(DLOG_ERROR, "%s json_object_new_object fail.", __FUNCTION__));

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR, json_object_put(obj_target_info);
        json_object_put(obj_act_info); json_object_put(obj_clear_log);
        debug_log(DLOG_ERROR, "%s json_object_new_object fail.", __FUNCTION__));

    (void)json_object_object_add(obj_clear_log, RFPROP_TARGET, obj_target_info);
    (void)json_object_object_add(obj_clear_log, RFPROP_ACTION_INFO, obj_act_info);
    (void)json_object_object_add(*o_result_jso, RFPROP_LOGSERVICE_CLEARLOG, obj_clear_log);

    
    (void)get_logservice_oem_huawei_action(*o_result_jso);
    

    return HTTP_OK;
}


LOCAL gint32 act_logservice_clear_log(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    
    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != i_paras) && (NULL != i_paras->val_jso),
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    
    ret = dal_get_object_handle_nth(CLASS_SEL, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SEL, METHOD_SEL_CLEAR_SEL, AUTH_ENABLE, i_paras->user_role_privilege, NULL, NULL);

    switch (ret) {
            
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
            return RFERR_INSUFFICIENT_PRIVILEGE;

            
        case RFERR_NO_RESOURCE:
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri);
            return HTTP_NOT_FOUND;

        case VOS_OK:
        case RFERR_SUCCESS:
            return HTTP_OK;

            
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 act_logservice_collect_sel_status(json_object **message_obj, guint8 *progress)
{
    return_val_if_expr(NULL == progress || message_obj == NULL, RF_OK);

    if (TRUE == vos_get_file_accessible(EO_LOG_SEL_CSV_TAR_NAME)) {
        *progress = 100;
        return VOS_OK;
    }

    debug_log(DLOG_ERROR, "act_logservice_collect_sel_status error");
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);

    return VOS_ERR;
}


LOCAL gint32 act_logservice_collect_sel_process(EXPORT_ACTION_ARGS *action_args, json_object **o_message_jso)
{
    OBJ_HANDLE obj_handle = 0;
    guint32 ret;
    GSList *input_list = NULL;

    if (o_message_jso == NULL || action_args == NULL) {
        return VOS_ERR;
    }

    input_list = g_slist_append(input_list, g_variant_new_string("/tmp/sel.tar"));

    
    
    ret = (guint32)dal_get_object_handle_nth(CLASS_SEL, 0, &obj_handle);
    ret |= (guint32)uip_call_class_method_redfish(action_args->is_from_webui, action_args->user_name,
        action_args->client, obj_handle, CLASS_SEL, METHOD_SEL_COLLECT_SEL, AUTH_DISABLE, 0, input_list, NULL);
    

    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    input_list = NULL;

    switch (ret) {
        case VOS_OK:
        case RFERR_SUCCESS:

            return VOS_OK;

            
        default:
            debug_log(DLOG_ERROR, "act_logservice_collect_sel_process, error code:%d", ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);

            return VOS_ERR;
    }
}


LOCAL gint32 act_logservice_collect_sel(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    EXPORT_TRANSACTION_INFO_S export_info = { 0 };

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: input param error.", __FUNCTION__, __LINE__));

    export_info.file_id = FILE_ID_SEL_PACKET_TAR;
    export_info.permission = USERROLE_READONLY;
    export_info.need_del = 1;
    export_info.get_status = act_logservice_collect_sel_status;
    export_info.generate_file = act_logservice_collect_sel_process;
    (void)strcpy_s(export_info.src, sizeof(export_info.src), EO_LOG_SEL_CSV_TAR_NAME);
    (void)strncpy_s(export_info.file_category, sizeof(export_info.file_category), FILE_CATEGORY_SELLOG,
        strlen(FILE_CATEGORY_SELLOG));

    return create_export_transaction(i_paras, o_message_jso, o_result_jso, &export_info);
}


LOCAL gint32 get_action_param_start_entry_id(json_object *val_jso, json_object **o_message_jso, json_object *param_json)
{
    json_object *start_entry_id_json = NULL;
    gint32 start_entry_id = 0;

    
    return_val_do_info_if_fail((NULL != val_jso && NULL != o_message_jso && NULL != param_json),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: param is NULL.", __func__));

    
    if (TRUE == json_object_object_get_ex(val_jso, RFACTION_PARAM_START_ENTRY_ID, &start_entry_id_json)) {
        start_entry_id = json_object_get_int(start_entry_id_json);
        (void)json_object_object_add(param_json, EVENT_PARAM_SEL_LAST_ID, json_object_new_int(start_entry_id));
        return VOS_OK;
    } else {
        (void)create_message_info(MSGID_ACT_PARA_MISSING, NULL, o_message_jso,
            RFPROP_LOGSERVICE_OEM_HUWEI_ACTION_QUERY_SEL_LOG, RFACTION_PARAM_START_ENTRY_ID);
        return HTTP_BAD_REQUEST;
    }
}


LOCAL gint32 get_action_param_entries_count(json_object *val_jso, json_object **o_message_jso, json_object *param_json)
{
    json_object *entries_count_json = NULL;
    gint32 entries_count = 0;

    
    return_val_do_info_if_fail((NULL != val_jso && NULL != o_message_jso && NULL != param_json),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: param is NULL.", __func__));

    
    if (TRUE == json_object_object_get_ex(val_jso, RFACTION_PARAM_ENTRIES_COUNT, &entries_count_json)) {
        entries_count = json_object_get_int(entries_count_json);
        (void)json_object_object_add(param_json, EVENT_PARAM_SEL_LIMIT, json_object_new_int(entries_count));
        return VOS_OK;
    } else {
        (void)create_message_info(MSGID_ACT_PARA_MISSING, NULL, o_message_jso,
            RFPROP_LOGSERVICE_OEM_HUWEI_ACTION_QUERY_SEL_LOG, RFACTION_PARAM_ENTRIES_COUNT);
        return HTTP_BAD_REQUEST;
    }
}


LOCAL gint32 get_action_param_sel_level(json_object *val_jso, json_object **o_message_jso, json_object *param_json)
{
    json_object *sel_level_json = NULL;
    const gchar *sel_level = NULL;
    guint32 array_index = 0;
    const gchar* levels_array[] = {SEL_LOG_PARAM_SEVERITY_INFO, SEL_LOG_PARAM_SEVERITY_MINOR, SEL_LOG_PARAM_SEVERITY_MAJOR, SEL_LOG_PARAM_SEVERITY_CRITICAL};

    
    return_val_do_info_if_fail((NULL != val_jso && NULL != o_message_jso && NULL != param_json),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: param is NULL.", __func__));

    
    if (TRUE == json_object_object_get_ex(val_jso, RFACTION_PARAM_SEL_LEVEL, &sel_level_json)) {
        sel_level = dal_json_object_get_str(sel_level_json);
        for (array_index = 0; array_index < G_N_ELEMENTS(levels_array); array_index++) {
            if (0 == g_strcmp0(levels_array[array_index], sel_level)) {
                (void)json_object_object_add(param_json, EVENT_PARAM_SEL_LEVEL, json_object_new_int(array_index));
                return VOS_OK;
            }
        }
        
        (void)create_message_info(MSGID_ACT_PARA_FORMAT_ERR, NULL, o_message_jso, sel_level, RFACTION_PARAM_SEL_LEVEL,
            RFPROP_LOGSERVICE_OEM_HUWEI_ACTION_QUERY_SEL_LOG);
        return HTTP_BAD_REQUEST;
    } else {
        (void)json_object_object_add(param_json, EVENT_PARAM_SEL_LEVEL, json_object_new_int(PARAM_DEFAULT_VALUE));
        return VOS_OK;
    }
}


LOCAL gint32 get_action_param_sel_object_type_id(json_object *val_jso, json_object **o_message_jso,
    json_object *param_json)
{
    gint32 ret = 0;
    json_bool ret_bool = FALSE;
    json_object *sel_object_type_json = NULL;
    gint32 array_index = 0;
    const gchar *sel_object_type = NULL;
    json_object *object_type_list = NULL;
    json_object *object_type_json = NULL;
    json_object *object_type_label_json = NULL;
    json_object *object_type_id_json = NULL;
    const gchar *object_type_label = NULL;
    gint32 object_type_id = 0;

    
    return_val_do_info_if_fail((NULL != val_jso && NULL != o_message_jso && NULL != param_json),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: param is NULL.", __func__));

    
    if (TRUE == json_object_object_get_ex(val_jso, RFACTION_PARAM_SEL_OBJECT_TYPE, &sel_object_type_json)) {
        sel_object_type = dal_json_object_get_str(sel_object_type_json);

        
        ret = get_system_object_type_list(&object_type_list);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s: get_system_object_type_list fail, ret is %d.", __func__, ret));

        for (array_index = 0; array_index < json_object_array_length(object_type_list); array_index++) {
            
            
            object_type_json = json_object_array_get_idx(object_type_list, array_index);
            return_val_do_info_if_fail(
                NULL != object_type_json, HTTP_INTERNAL_SERVER_ERROR, json_object_put(object_type_list); debug_log(
                DLOG_ERROR, "%s: object_type_json is NULL because get_system_object_type_list error.", __func__));

            ret_bool =
                json_object_object_get_ex(object_type_json, EVENT_PARAM_OBJECT_TYPE_LABEL, &object_type_label_json);
            return_val_do_info_if_fail(TRUE == ret_bool, HTTP_INTERNAL_SERVER_ERROR, json_object_put(object_type_list);
                debug_log(DLOG_ERROR, "%s: get object_type_label_json fail because get_system_object_type_list error.",
                __func__));

            ret_bool = json_object_object_get_ex(object_type_json, EVENT_PARAM_OBJECT_TYPE_ID, &object_type_id_json);
            return_val_do_info_if_fail(TRUE == ret_bool, HTTP_INTERNAL_SERVER_ERROR, json_object_put(object_type_list);
                debug_log(DLOG_ERROR, "%s: get object_type_id_json fail because get_system_object_type_list error.",
                __func__));
            

            object_type_label = dal_json_object_get_str(object_type_label_json);
            object_type_id = json_object_get_int(object_type_id_json);

            
            
            if (NULL != object_type_label && 0 == g_strcmp0(object_type_label, sel_object_type)) {
                (void)json_object_object_add(param_json, EVENT_PARAM_SEL_ENTITY, json_object_new_int(object_type_id));
                json_object_put(object_type_list);
                return VOS_OK;
            }
            
        }
        
        
        json_object_put(object_type_list);
        
        (void)create_message_info(MSGID_ACT_PARA_FORMAT_ERR, NULL, o_message_jso, sel_object_type,
            RFACTION_PARAM_SEL_OBJECT_TYPE, RFPROP_LOGSERVICE_OEM_HUWEI_ACTION_QUERY_SEL_LOG);
        return HTTP_BAD_REQUEST;
    } else {
        (void)json_object_object_add(param_json, EVENT_PARAM_SEL_ENTITY, json_object_new_int(PARAM_DEFAULT_VALUE));
        return VOS_OK;
    }
}


LOCAL gint32 get_action_param_sel_begin_time(json_object *val_jso, json_object **o_message_jso, json_object *param_json)
{
    gint32 ret = 0;
    json_object *sel_begin_time_json = NULL;
    const gchar *sel_begin_time = NULL;
    int year = 0;
    int month = 0;
    int day = 0;
    int hour = 0;
    int minute = 0;
    int second = 0;
    struct tm begin_time_gmt;
    gulong begin_time_timestamp = 0;

    
    return_val_do_info_if_fail((NULL != val_jso && NULL != o_message_jso && NULL != param_json),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: param is NULL.", __func__));

    
    if (TRUE == json_object_object_get_ex(val_jso, RFACTION_PARAM_SEL_BEGIN_TIME, &sel_begin_time_json)) {
        
        sel_begin_time = dal_json_object_get_str(sel_begin_time_json);
        ret = sscanf_s(sel_begin_time, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
        return_val_do_info_if_fail(ret > 0, HTTP_BAD_REQUEST,
            (void)create_message_info(MSGID_ACT_PARA_FORMAT_ERR, NULL, o_message_jso, sel_begin_time,
            RFACTION_PARAM_SEL_BEGIN_TIME, RFPROP_LOGSERVICE_OEM_HUWEI_ACTION_QUERY_SEL_LOG));

        (void)memset_s(&begin_time_gmt, sizeof(struct tm), 0, sizeof(struct tm));
        begin_time_gmt.tm_year = year - YEAR_1900;
        begin_time_gmt.tm_mon = month - 1;
        begin_time_gmt.tm_mday = day;
        begin_time_gmt.tm_hour = hour;
        begin_time_gmt.tm_min = minute;
        begin_time_gmt.tm_sec = second;

        ret = dal_gmt_to_timestamp(begin_time_gmt, &begin_time_timestamp);
        return_val_do_info_if_fail((gulong)-1 != begin_time_timestamp && VOS_OK == ret, HTTP_BAD_REQUEST,
            (void)create_message_info(MSGID_ACT_PARA_FORMAT_ERR, NULL, o_message_jso, sel_begin_time,
            RFACTION_PARAM_SEL_BEGIN_TIME, RFPROP_LOGSERVICE_OEM_HUWEI_ACTION_QUERY_SEL_LOG));
        (void)json_object_object_add(param_json, EVENT_PARAM_SEL_BEGIN_TIME,
            json_object_new_int((guint32)begin_time_timestamp));
    } else {
        (void)json_object_object_add(param_json, EVENT_PARAM_SEL_BEGIN_TIME,
            json_object_new_int(PARAM_DEFAULT_VALUE_ZERO));
    }

    return VOS_OK;
}


LOCAL gint32 get_action_param_sel_end_time(json_object *val_jso, json_object **o_message_jso, json_object *param_json)
{
    gint32 ret = 0;
    json_object *sel_end_time_json = NULL;
    const gchar *sel_end_time = NULL;
    int year = 0;
    int month = 0;
    int day = 0;
    int hour = 0;
    int minute = 0;
    int second = 0;
    struct tm end_time_gmt;
    gulong end_time_timestamp = 0;

    
    return_val_do_info_if_fail((NULL != val_jso && NULL != o_message_jso && NULL != param_json),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: param is NULL.", __func__));

    
    if (TRUE == json_object_object_get_ex(val_jso, RFACTION_PARAM_SEL_END_TIME, &sel_end_time_json)) {
        
        sel_end_time = dal_json_object_get_str(sel_end_time_json);
        ret = sscanf_s(sel_end_time, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
        return_val_do_info_if_fail(ret > 0, HTTP_BAD_REQUEST,
            (void)create_message_info(MSGID_ACT_PARA_FORMAT_ERR, NULL, o_message_jso, sel_end_time,
            RFACTION_PARAM_SEL_END_TIME, RFPROP_LOGSERVICE_OEM_HUWEI_ACTION_QUERY_SEL_LOG));

        (void)memset_s(&end_time_gmt, sizeof(struct tm), 0, sizeof(struct tm));
        end_time_gmt.tm_year = year - YEAR_1900;
        end_time_gmt.tm_mon = month - 1;
        end_time_gmt.tm_mday = day;
        end_time_gmt.tm_hour = hour;
        end_time_gmt.tm_min = minute;
        end_time_gmt.tm_sec = second;

        ret = dal_gmt_to_timestamp(end_time_gmt, &end_time_timestamp);
        return_val_do_info_if_fail((gulong)-1 != end_time_timestamp && VOS_OK == ret, HTTP_BAD_REQUEST,
            (void)create_message_info(MSGID_ACT_PARA_FORMAT_ERR, NULL, o_message_jso, sel_end_time,
            RFACTION_PARAM_SEL_END_TIME, RFPROP_LOGSERVICE_OEM_HUWEI_ACTION_QUERY_SEL_LOG));
        (void)json_object_object_add(param_json, EVENT_PARAM_SEL_END_TIME,
            json_object_new_int((guint32)end_time_timestamp));
    } else {
        (void)json_object_object_add(param_json, EVENT_PARAM_SEL_END_TIME,
            json_object_new_int(PARAM_DEFAULT_VALUE_ZERO));
    }

    return VOS_OK;
}


LOCAL gint32 get_action_param_sel_search_string(json_object *val_jso, json_object **o_message_jso,
    json_object *param_json)
{
    json_object *sel_search_string_object = NULL;
    const gchar *sel_search_string = NULL;

    
    return_val_do_info_if_fail((NULL != val_jso && NULL != o_message_jso && NULL != param_json),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: param is NULL.", __func__));

    
    if (TRUE == json_object_object_get_ex(val_jso, RFACTION_PARAM_SEL_SEARCH_STRING, &sel_search_string_object)) {
        sel_search_string = dal_json_object_get_str(sel_search_string_object);
        (void)json_object_object_add(param_json, EVENT_PARAM_SEL_SEARCH_STR, json_object_new_string(sel_search_string));
    } else {
        (void)json_object_object_add(param_json, EVENT_PARAM_SEL_SEARCH_STR,
            json_object_new_string(PARAM_DEFAULT_VALUE_EMPTY_STR));
    }

    return VOS_OK;
}


LOCAL gint32 get_action_param_for_query_sel_logs(json_object *val_jso, json_object **o_message_jso,
    json_object **param_json)
{
    gint32 ret;

    
    return_val_do_info_if_fail((NULL != val_jso && NULL != o_message_jso && NULL != param_json),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: param is NULL.", __func__));

    *param_json = json_object_new_object();
    return_val_do_info_if_fail(NULL != *param_json, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_object fail, then param_json is NULL.", __func__));

    
    (void)json_object_object_add(*param_json, PARAM_FLAG, json_object_new_int(RFACTION_PARAM_GET_EVENT_LOG_DATA));

    
    (void)json_object_object_add(*param_json, EVENT_PARAM_LANG_TYPE, json_object_new_string(EVENT_LANGUAGE_EN));

    
    ret = get_action_param_start_entry_id(val_jso, o_message_jso, *param_json);
    return_val_if_fail(VOS_OK == ret, ret);

    ret = get_action_param_entries_count(val_jso, o_message_jso, *param_json);
    return_val_if_fail(VOS_OK == ret, ret);

    ret = get_action_param_sel_level(val_jso, o_message_jso, *param_json);
    return_val_if_fail(VOS_OK == ret, ret);

    ret = get_action_param_sel_object_type_id(val_jso, o_message_jso, *param_json);
    return_val_if_fail(VOS_OK == ret, ret);

    ret = get_action_param_sel_begin_time(val_jso, o_message_jso, *param_json);
    return_val_if_fail(VOS_OK == ret, ret);

    ret = get_action_param_sel_end_time(val_jso, o_message_jso, *param_json);
    return_val_if_fail(VOS_OK == ret, ret);

    ret = get_action_param_sel_search_string(val_jso, o_message_jso, *param_json);
    return_val_if_fail(VOS_OK == ret, ret);

    return VOS_OK;
}


LOCAL gint32 parse_query_sel_log_error_code(json_object *val_jso, json_object **o_message_jso, gint32 ret)
{
    json_object *param_json = NULL;
    gint32 param_int = 0;
    const gchar *param_str = NULL;
    gchar param_value[MAX_STRBUF_LEN + 1] = {0};

    switch (ret) {
        
        case VOS_OK:
        case RFERR_SUCCESS: {
            return HTTP_OK;
        }
        
        case EVENT_ERR_SEL_LAST_ID_INVALID: {
            (void)json_object_object_get_ex(val_jso, RFACTION_PARAM_START_ENTRY_ID, &param_json);
            param_int = json_object_get_int(param_json);
            (void)snprintf_s(param_value, MAX_STRBUF_LEN + 1, MAX_STRBUF_LEN, "%d", param_int);
            (void)create_message_info(MSGID_ACT_PARA_FORMAT_ERR, NULL, o_message_jso, param_value,
                RFACTION_PARAM_START_ENTRY_ID, RFPROP_LOGSERVICE_OEM_HUWEI_ACTION_QUERY_SEL_LOG);
            return HTTP_BAD_REQUEST;
        }
        
        case EVENT_ERR_SEL_LIMIT_INVALID: {
            (void)json_object_object_get_ex(val_jso, RFACTION_PARAM_ENTRIES_COUNT, &param_json);
            param_int = json_object_get_int(param_json);
            (void)snprintf_s(param_value, MAX_STRBUF_LEN + 1, MAX_STRBUF_LEN, "%d", param_int);
            (void)create_message_info(MSGID_ACT_PARA_FORMAT_ERR, NULL, o_message_jso, param_value,
                RFACTION_PARAM_ENTRIES_COUNT, RFPROP_LOGSERVICE_OEM_HUWEI_ACTION_QUERY_SEL_LOG);
            return HTTP_BAD_REQUEST;
        }
        
        case EVENT_ERR_SEL_LEVEL_INVALID: {
            (void)json_object_object_get_ex(val_jso, RFACTION_PARAM_SEL_LEVEL, &param_json);
            param_str = dal_json_object_get_str(param_json);
            (void)create_message_info(MSGID_ACT_PARA_FORMAT_ERR, NULL, o_message_jso, param_str,
                RFACTION_PARAM_SEL_LEVEL, RFPROP_LOGSERVICE_OEM_HUWEI_ACTION_QUERY_SEL_LOG);
            return HTTP_BAD_REQUEST;
        }
        
        case EVENT_ERR_SEL_ENTITY_INVALID: {
            (void)json_object_object_get_ex(val_jso, RFACTION_PARAM_SEL_OBJECT_TYPE, &param_json);
            param_str = dal_json_object_get_str(param_json);
            (void)create_message_info(MSGID_ACT_PARA_FORMAT_ERR, NULL, o_message_jso, param_str,
                RFACTION_PARAM_SEL_OBJECT_TYPE, RFPROP_LOGSERVICE_OEM_HUWEI_ACTION_QUERY_SEL_LOG);
            return HTTP_BAD_REQUEST;
        }
        
        case EVENT_ERR_SEL_BEGIN_TIME_INVALID: {
            (void)json_object_object_get_ex(val_jso, RFACTION_PARAM_SEL_BEGIN_TIME, &param_json);
            param_str = dal_json_object_get_str(param_json);
            (void)create_message_info(MSGID_ACT_PARA_FORMAT_ERR, NULL, o_message_jso, param_str,
                RFACTION_PARAM_SEL_BEGIN_TIME, RFPROP_LOGSERVICE_OEM_HUWEI_ACTION_QUERY_SEL_LOG);
            return HTTP_BAD_REQUEST;
        }
        
        case EVENT_ERR_SEL_END_TIME_INVALID: {
            (void)json_object_object_get_ex(val_jso, RFACTION_PARAM_SEL_END_TIME, &param_json);
            param_str = dal_json_object_get_str(param_json);
            (void)create_message_info(MSGID_ACT_PARA_FORMAT_ERR, NULL, o_message_jso, param_str,
                RFACTION_PARAM_SEL_END_TIME, RFPROP_LOGSERVICE_OEM_HUWEI_ACTION_QUERY_SEL_LOG);
            return HTTP_BAD_REQUEST;
        }
        
        case EVENT_ERR_SEL_SEARCH_STR_INVALID: {
            (void)json_object_object_get_ex(val_jso, RFACTION_PARAM_SEL_SEARCH_STRING, &param_json);
            param_str = dal_json_object_get_str(param_json);
            (void)create_message_info(MSGID_ACT_PARA_FORMAT_ERR, NULL, o_message_jso, param_str,
                RFACTION_PARAM_SEL_SEARCH_STRING, RFPROP_LOGSERVICE_OEM_HUWEI_ACTION_QUERY_SEL_LOG);
            return HTTP_BAD_REQUEST;
        }
        default: {
            debug_log(DLOG_ERROR, "%s: uip_web_json_interface fail, ret is %d.", __func__, ret);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
    }
}


LOCAL gint32 act_logservice_query_sel_log(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    errno_t safe_fun_ret;
    gint32 ret;
    json_object *param_json = NULL;
    const gchar *tmp_str = NULL;
    gchar param_json_str[PROPERTY_LEN + 1] = {0};
    gchar *output_str = NULL;
    json_object *oem_json = NULL;
    json_object *huawei_json = NULL;
    json_object *sel_log_entries_json = NULL;

    
    return_val_do_info_if_fail((NULL != i_paras && NULL != o_message_jso && NULL != o_result_jso),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: param is NULL.", __func__));

    
    ret = get_action_param_for_query_sel_logs(i_paras->val_jso, o_message_jso, &param_json);
    return_val_do_info_if_fail(VOS_OK == ret, ret, (void)json_object_put(param_json);
        debug_log(DLOG_ERROR, "%s: get_action_param fail, ret is %d.", __func__, ret));
    tmp_str = dal_json_object_to_json_string(param_json);
    safe_fun_ret = strncpy_s(param_json_str, sizeof(param_json_str), tmp_str, sizeof(param_json_str) - 1);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    (void)json_object_put(param_json);

    
    debug_log(DLOG_INFO, "%s: param_json_str is %s.", __func__, param_json_str);
    ret = uip_web_json_interface(RFACTION_PARAM_GET_EVENT, param_json_str, &output_str);

    
    ret = parse_query_sel_log_error_code(i_paras->val_jso, o_message_jso, ret);
    if (HTTP_OK == ret) {
        
        sel_log_entries_json = json_tokener_parse(output_str);
        
        free(output_str);
        
        return_val_do_info_if_fail(NULL != sel_log_entries_json, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s: json_tokener_parse fail, then sel_log_entries_json is null.", __func__));

        oem_json = json_object_new_object();
        return_val_do_info_if_fail(NULL != oem_json, HTTP_INTERNAL_SERVER_ERROR, json_object_put(sel_log_entries_json);
            debug_log(DLOG_ERROR, "%s: json_object_new_object fail, then oem_json is NULL.", __func__));

        huawei_json = json_object_new_object();
        return_val_do_info_if_fail(NULL != huawei_json, HTTP_INTERNAL_SERVER_ERROR,
            json_object_put(sel_log_entries_json);
            json_object_put(oem_json);
            debug_log(DLOG_ERROR, "%s: json_object_new_object fail, then json is NULL.", __func__));

        (void)create_message_info(MSGID_SUCCESS, NULL, o_message_jso);
        (void)json_object_object_add(*o_message_jso, RFPROP_OEM, oem_json);
        (void)json_object_object_add(oem_json, RFPROP_OEM_HUAWEI, huawei_json);
        (void)json_object_object_add(huawei_json, RFPROP_LOGSERVICE_SEL_LOG_ENTRIES, sel_log_entries_json);

        o_result_jso = o_message_jso;
    }

    return ret;
}


LOCAL gboolean get_system_health_status()
{
    gint32 ret;
    guint32 minor_cnt;
    guint32 major_cnt;
    guint32 critical_cnt;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;

    
    
    property_name_list = g_slist_append(property_name_list, PROPERTY_WARNING_MINOR_NUM);
    property_name_list = g_slist_append(property_name_list, PROPERTY_WARNING_MAJOR_NUM);
    property_name_list = g_slist_append(property_name_list, PROPERTY_WARNING_CRITICAL_NUM);

    
    ret = uip_multiget_object_property(CLASS_WARNING, OBJECT_WARNING, property_name_list, &property_value);
    return_val_do_info_if_fail(VOS_OK == ret, FALSE, g_slist_free(property_name_list);
        debug_log(DLOG_ERROR, "%s uip_multiget_object_property fail.", __FUNCTION__));

    
    minor_cnt = g_variant_get_uint32((GVariant *)g_slist_nth_data(property_value, 0));

    
    major_cnt = g_variant_get_uint32((GVariant *)g_slist_nth_data(property_value, 1));

    
    critical_cnt = g_variant_get_uint32((GVariant *)g_slist_nth_data(property_value, 2));

    g_slist_free(property_name_list);
    uip_free_gvariant_list(property_value);

    return_val_if_fail(!(minor_cnt || major_cnt || critical_cnt), TRUE);

    return FALSE;
}


LOCAL gboolean get_other_system_health_status(void)
{
    gint32  ret;
    guint32 minor_cnt;
    guint32 major_cnt;
    guint32 critical_cnt;
    GSList* property_name_list = NULL;
    GSList* property_value_list = NULL;
    
    property_name_list = g_slist_append(property_name_list, g_variant_new_string(PROPERTY_WARNING_MINOR_NUM));
    property_name_list = g_slist_append(property_name_list, g_variant_new_string(PROPERTY_WARNING_MAJOR_NUM));
    property_name_list = g_slist_append(property_name_list, g_variant_new_string(PROPERTY_WARNING_CRITICAL_NUM));
    ret = get_other_smm_property(OBJECT_WARNING, property_name_list, &property_value_list);
    if (ret != RET_OK) {
        g_slist_free_full(property_name_list, (GDestroyNotify)g_variant_unref);
        property_name_list = NULL;
        debug_log(DLOG_ERROR, "get_other_smm_property of Warning Count fail, ret = %d", ret);
        return FALSE;
    }
    
    minor_cnt = g_variant_get_uint32((GVariant*)g_slist_nth_data(property_value_list, 0));
    
    major_cnt = g_variant_get_uint32((GVariant*)g_slist_nth_data(property_value_list, 1));
    
    critical_cnt = g_variant_get_uint32((GVariant*)g_slist_nth_data(property_value_list, 2));

    g_slist_free_full(property_name_list, (GDestroyNotify)g_variant_unref);
    property_name_list = NULL;

    uip_free_gvariant_list(property_value_list);
    debug_log(DLOG_INFO, "minor_cnt = %u, major_cnt = %u, critical_cnt = %u", minor_cnt, major_cnt, critical_cnt);
    if (minor_cnt != 0 || major_cnt != 0 || critical_cnt != 0) {
        return TRUE;
    }
    return FALSE;
}

LOCAL void get_event_time(guint32 time_i, json_object **json_object_time)
{
    gint32 ret;
    gchar event_time_str[MAX_SEL_TIME_LEN] = {0};

    
    ret = dal_get_redfish_datetime_stamp(time_i, event_time_str, sizeof(event_time_str));
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "%s dal_get_redfish_datetime_stamp fail.", __FUNCTION__));

    *json_object_time = json_object_new_string(event_time_str);
    return_do_info_if_fail(NULL != *json_object_time,
        debug_log(DLOG_ERROR, "%s json_object_new_string fail.", __FUNCTION__));

    return;
}

LOCAL void get_event_severity(guint8 level, json_object **json_object_level)
{
    gchar *event_level_str = NULL;

    switch (level) {
        case 0:
            event_level_str = RF_STATUS_OK;
            break;
        
        case 1:
        case 2:
            event_level_str = RF_STATUS_WARNING;
            break;
        
        case 3:
            event_level_str = RF_STATUS_CRITICAL;
            break;

        default:
            debug_log(DLOG_ERROR, "%s input level is invalid.", __FUNCTION__);
            break;
    }

    return_do_info_if_fail(NULL != event_level_str, debug_log(DLOG_ERROR, "%s input level is invalid.", __FUNCTION__));

    *json_object_level = json_object_new_string(event_level_str);
    return_do_info_if_fail(NULL != *json_object_level,
        debug_log(DLOG_ERROR, "%s json_object_new_string fail.", __FUNCTION__));

    return;
}

void get_event_level(guint8 level, json_object **json_object_level)
{
    gchar *severity_level_str = NULL;

    switch (level) {
        case LOGENTRY_STATUS_INFORMATIONAL_CODE:
            severity_level_str = INFORMATIONAL_STRING;
            break;

        case LOGENTRY_STATUS_MINOR_CODE:
            severity_level_str = MINOR_STRING;
            break;

        case LOGENTRY_STATUS_MAJOR_CODE:
            severity_level_str = MAJOR_STRING;
            break;

        case LOGENTRY_STATUS_CRITICAL_CODE:
            severity_level_str = CRITICAL_STRING;
            break;

        default:
            return;
    }

    return_do_info_if_fail(NULL != severity_level_str,
        debug_log(DLOG_ERROR, "%s input level is invalid.", __FUNCTION__));

    *json_object_level = json_object_new_string(severity_level_str);
    return_do_info_if_fail(NULL != *json_object_level,
        debug_log(DLOG_ERROR, "%s json_object_new_string fail.", __FUNCTION__));

    return;
}

LOCAL void get_event_code(guint32 code, json_object **json_object_code)
{
    gint32 ret;
    gchar event_code_str[256] = {0};

    
    ret = snprintf_s(event_code_str, sizeof(event_code_str), sizeof(event_code_str) - 1, "0x%08X", code);
    return_do_info_if_fail(ERROR_SNPRINTF < ret, debug_log(DLOG_ERROR, "%s:snprintf_s fail.", __FUNCTION__));

    *json_object_code = json_object_new_string(event_code_str);
    return_do_info_if_fail(NULL != *json_object_code,
        debug_log(DLOG_ERROR, "%s json_object_new_string fail.", __FUNCTION__));

    return;
}


void get_sel_message_id(OBJ_HANDLE evt_handle, json_object **messageid_object)
{
    gchar evt_name[MAX_MSG_ID_LEN] = {0};
    gchar messageid[SELINFO_DESC_LEN] = {0};
    gchar* major_ver = NULL;
    gchar* minor_ver = NULL;
    gchar* aux_ver = NULL; 
    gint32 ret;

    
    ret = dal_get_property_value_string(evt_handle, PROPERTY_EVENT_INFORMATION_EVENT_NAME, evt_name,
        sizeof(evt_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_string failed, ret:%d", __FUNCTION__, ret);
        goto ERR_EXIT;
    }

    
    ret = get_event_lang_version(&major_ver, &minor_ver, &aux_ver);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get event lang version failed, ret:%d", __FUNCTION__, ret);
        goto ERR_EXIT;
    }
    
    
    ret = snprintf_s(messageid, sizeof(messageid), sizeof(messageid) - 1, "%s.%s.%s.%s", EVT_REGISTRY_PREFIX, major_ver,
        minor_ver, evt_name);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s:snprintf_s fail, ret:%d", __FUNCTION__, ret);
        goto ERR_EXIT;
    }

    *messageid_object = json_object_new_string((const gchar*)messageid);
    return;

ERR_EXIT:
    *messageid_object = NULL;
    return;
}


LOCAL void get_message_id(guint32 code, json_object **json_object_message_id)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    
    ret = dal_get_specific_object_uint32(CLASS_EVENT_INFORMATION, PROPERTY_EVENT_INFORMATION_EVENT_CODE, code,
        &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s dal_get_specific_object_uint32 fail.", __FUNCTION__);
        *json_object_message_id = NULL;
        return;
    }

    get_sel_message_id(obj_handle, json_object_message_id);
    return;
}


LOCAL void get_event_suggestion(const gchar *event_sugg_ptr, json_object *health_event_obj)
{
    json_object *event_sugg_jso = NULL;

    if (VOS_OK == check_string_val_effective(event_sugg_ptr)) {
        event_sugg_jso = json_object_new_string(event_sugg_ptr);
    }

    json_object_object_add(health_event_obj, RFPROP_LOGSERVICE_ENTRY_SUGGESTION, event_sugg_jso);
}


LOCAL gint32 get_healthevents_from_list(SensorHealtheventInfo *healthevent, GSList *output_list)
{
    errno_t str_ret;
    const gchar *para = NULL;
    const gchar *event_desc_ptr = NULL;
    const gchar *event_sugg_ptr = NULL;
    const gchar *event_sensor_name_ptr = NULL;

    if (healthevent == NULL || output_list == NULL) {
        debug_log(DLOG_ERROR, "input param error");
        return RET_ERR;
    }
    healthevent->next_record_id = g_variant_get_uint16((GVariant *)g_slist_nth_data(output_list, 0));   // 0表示list第0个元素
    healthevent->event_time = g_variant_get_uint32((GVariant *)g_slist_nth_data(output_list, 3));   // 3表示list第3个元素
    healthevent->event_level = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 4));    // 4表示list第4个元素
    healthevent->event_code = g_variant_get_uint32((GVariant *)g_slist_nth_data(output_list, 5));   // 5表示list第5个元素
    event_desc_ptr = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 6), NULL);  // 6表示list第6个元素
    event_sugg_ptr = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 7), NULL);  // 7表示list第7个元素
    para = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 8), NULL);    // 8表示list第8个元素
    event_sensor_name_ptr = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 9), NULL);   // 9表示list第9个元素
    healthevent->event_fru_type = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 14));  // 14表示list第14个元素
    str_ret = strcpy_s(healthevent->event_desc, EVENT_STR_LENGTH, event_desc_ptr);
    if (str_ret != EOK) {
        debug_log(DLOG_ERROR, "strncpy_s event_desc fail, str_ret = %d", str_ret);
        return RET_ERR;
    }
    str_ret = strcpy_s(healthevent->event_sugg, EVENT_STR_LENGTH, event_sugg_ptr);
    if (str_ret != EOK) {
        debug_log(DLOG_ERROR, "strncpy_s event_desc fail, str_ret = %d", str_ret);
        return RET_ERR;
    }

    str_ret = strcpy_s(healthevent->event_args, EVENT_STR_LENGTH, para);
    if (str_ret != EOK) {
        debug_log(DLOG_ERROR, "strncpy_s event_args fail, str_ret = %d", str_ret);
        return RET_ERR;
    }
    str_ret = strcpy_s(healthevent->event_sensor_name, EVENT_SENSOR_NAME_LENGTH, event_sensor_name_ptr);
    if (str_ret != EOK) {
        debug_log(DLOG_ERROR, "strncpy_s event_sensor_name fail, str_ret = %d", str_ret);
        return RET_ERR;
    }
    debug_log(DLOG_INFO, "next_record_id = %u, event_time = %u, event_level = %u, \
        event_code = 0x%x, event_desc_ptr = %s, event_sensor_name = %s, event_fru_type = %u",
        healthevent->next_record_id, healthevent->event_time, healthevent->event_level,
        healthevent->event_code, healthevent->event_desc, healthevent->event_sensor_name, healthevent->event_fru_type);
    return RET_OK;
}


LOCAL void fill_healthevent_to_object(SensorHealtheventInfo *healthevent, json_object *health_event_obj)
{
    json_object *property_val = NULL;

    if (healthevent == NULL || health_event_obj == NULL) {
        debug_log(DLOG_ERROR, "input param error");
        return;
    }

    
    property_val = json_object_new_string(RFPROP_LOGSERVICE_EVENT_TYPE_VALUE);
    json_object_object_add(health_event_obj, RFPROP_LOGSERVICE_EVENT_TYPE, property_val);
    property_val = NULL;

    
    property_val = json_object_new_string(RFPROP_LOGSERVICE_ENTRY_TYPE_VALUE);
    json_object_object_add(health_event_obj, RFPROP_LOGSERVICE_ENTRY_TYPE, property_val);
    property_val = NULL;

    
    if (strlen(healthevent->event_sensor_name) > 0) {
        property_val = json_object_new_string(healthevent->event_sensor_name);
        json_object_object_add(health_event_obj, RFPROP_LOGSERVICE_EVENT_SUBJECT, property_val);
        property_val = NULL;
    } else {
        json_object_object_add(health_event_obj, RFPROP_LOGSERVICE_EVENT_SUBJECT, property_val);
    }

    
    get_event_time(healthevent->event_time, &property_val);
    json_object_object_add(health_event_obj, RFPROP_LOGSERVICE_EVENT_TIME, property_val);
    property_val = NULL;

    
    get_event_severity(healthevent->event_level, &property_val);
    json_object_object_add(health_event_obj, RFPROP_LOGSERVICE_EVENT_LEVEL, property_val);
    property_val = NULL;

    
    get_event_level(healthevent->event_level, &property_val);
    json_object_object_add(health_event_obj, PROPERTY_LOG_LEVEL, property_val);
    property_val = NULL;
    
    
    property_val = json_object_new_string(healthevent->event_desc);
    json_object_object_add(health_event_obj, RFPROP_LOGSERVICE_ENTRY_DESCROPTION, property_val);
    property_val = NULL;

    
    
    get_event_suggestion(healthevent->event_sugg, health_event_obj);
    

    
    get_event_code(healthevent->event_code, &property_val);
    json_object_object_add(health_event_obj, RFPROP_LOGSERVICE_EVENT_CODE, property_val);
    property_val = NULL;

    
    get_message_id(healthevent->event_code, &property_val);
    json_object_object_add(health_event_obj, RFPROP_LOGSERVICE_MESSAGE_ID, property_val);
    property_val = NULL;

    
    property_val = json_tokener_parse(healthevent->event_args);
    json_object_object_add(health_event_obj, RFPROP_LOGSERVICE_MESSAGE_ARGS, property_val);

    return;
}


LOCAL void get_system_health_events(json_object **json_array, gint32 event_from)
{
    gint32 ret;
    guint16 record_id = 0x00;
    json_object *health_event_obj = NULL;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    SensorHealtheventInfo *healthevent = NULL;
    healthevent = g_malloc0(sizeof(SensorHealtheventInfo));
    if (healthevent == NULL) {
        debug_log(DLOG_ERROR, "g_malloc0 fail.");
        return;
    }
    
    while (record_id != 0xffff) {
        input_list = g_slist_append(input_list, g_variant_new_uint16(record_id));
        if (event_from == LOCAL_BOARD_EVENT) {
            ret = uip_set_object_property(CLASS_WARNING, OBJECT_WARNING,
                METHOD_WARNING_GET_EVENT, input_list, &output_list);
        } else {
            
            ret = call_other_smm_method(OBJECT_WARNING, METHOD_WARNING_GET_EVENT, NULL, input_list, &output_list);
        }
        uip_free_gvariant_list(input_list);
        input_list = NULL;
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "uip_set_object_property or call_other_smm_method fail, ret = %d", ret);
        }

        
        ret = get_healthevents_from_list(healthevent, output_list);
        uip_free_gvariant_list(output_list);
        output_list = NULL;

        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "get_healthevents_from_list fail, ret = %d", ret);
            g_free(healthevent);
            return;
        }
        record_id = healthevent->next_record_id;
        if (event_from != LOCAL_BOARD_EVENT) {
            if (healthevent->event_fru_type == DEVICE_TYPE_FAN || healthevent->event_fru_type == DEVICE_TYPE_PS) {
                continue;
            }
        }
        
        health_event_obj = json_object_new_object();
        if (health_event_obj == NULL) {
            debug_log(DLOG_ERROR, "json_object_new_object fail.");
            g_free(healthevent);
            return;
        }
        fill_healthevent_to_object(healthevent, health_event_obj);

        
        ret = json_object_array_add(*json_array, health_event_obj);
        if (ret != VOS_OK) {
            json_object_put(health_event_obj);
            debug_log(DLOG_ERROR, "json_object_array_add fail.");
        }
        
        (void)memset_s(healthevent, sizeof(SensorHealtheventInfo), 0, sizeof(SensorHealtheventInfo));
    }
    g_free(healthevent);
    return;
}

LOCAL void get_main_system_health_events(json_object **json_array)
{
    return get_system_health_events(json_array, LOCAL_BOARD_EVENT);
}

LOCAL void get_other_system_health_events(json_object **json_array)
{
    return get_system_health_events(json_array, OTHER_BOARD_EVENT);
}


LOCAL gint32 get_system_object_type_list(json_object **json_array)
{
    errno_t safe_fun_ret;
    gint32 ret;
    json_object *input_json = NULL;
    const gchar *tmp_str = NULL;
    gchar input_str[PROPERTY_LEN + 1] = {0};
    gchar *output_str = NULL;

    
    return_val_do_info_if_fail(NULL != json_array, VOS_ERR, debug_log(DLOG_ERROR, "%s: param is NULL.", __func__));

    
    input_json = json_object_new_object();
    return_val_do_info_if_fail(NULL != input_json, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: json_object_new_object fail, then input_json is NULL.", __func__));

    (void)json_object_object_add(input_json, PARAM_FLAG, json_object_new_int(RFACTION_PARAM_GET_OBJECT_TYPE_LIST));
    tmp_str = dal_json_object_to_json_string(input_json);
    safe_fun_ret = strncpy_s(input_str, sizeof(input_str), tmp_str, sizeof(input_str) - 1);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    json_object_put(input_json);

    
    ret = uip_web_json_interface(RFACTION_PARAM_GET_EVENT, input_str, &output_str);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get object type list fail, ret is %d.", __func__, ret));
    *json_array = json_tokener_parse(output_str);
    
    free(output_str);
    

    return VOS_OK;
}


LOCAL gint32 get_logservice_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    json_object *huawei = NULL;
    json_object *health_event = NULL;
    json_object *object_type_list = NULL;

    
    ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_if_fail(VOS_OK == ret, ret);

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:json_object_new_object fail", __FUNCTION__));

    huawei = json_object_new_object();
    return_val_do_info_if_fail(NULL != huawei, HTTP_OK,
        debug_log(DLOG_ERROR, "%s:json_object_new_object fail", __FUNCTION__));

    health_event = json_object_new_array();
    return_val_do_info_if_fail(NULL != health_event, HTTP_OK,
        json_object_object_add(*o_result_jso, RFPROP_COMMON_MANUFACTURER, huawei);
        debug_log(DLOG_ERROR, "%s:json_object_new_array fail", __FUNCTION__));
    if (dal_check_if_mm_board() == FALSE) {
        if (get_system_health_status()) {
            
            (void)get_main_system_health_events(&health_event);
        }
        
        (void)json_object_object_add(huawei, RFPROP_LOGSERVICE_HEALTH_EVENT, health_event);
    } else {
        if (get_system_health_status()) {
            
            (void)get_main_system_health_events(&health_event);
        }
        if (get_other_system_health_status()) {
            
            (void)get_other_system_health_events(&health_event);
        }
        (void)json_object_object_add(huawei, RFPROP_LOGSERVICE_HEALTH_EVENT, health_event);
    }
    
    (void)get_system_object_type_list(&object_type_list);
    (void)json_object_object_add(huawei, RFPROP_LOGSERVICE_OBJECT_TYPE_LIST, object_type_list);

    (void)json_object_object_add(*o_result_jso, RFPROP_COMMON_MANUFACTURER, huawei);

    return HTTP_OK;
}


gint32 system_provider_logservice_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gint32 ret;

    ret = redfish_check_system_uri_valid(i_paras->uri);
    return_val_if_fail(ret, HTTP_NOT_FOUND);

    *prop_provider = g_system_logservice_provider;
    *count = sizeof(g_system_logservice_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}


gint32 system_provider_logservice_action_info_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret;

    ret = redfish_check_system_uri_valid(i_paras->uri);
    return_val_if_fail(ret, HTTP_NOT_FOUND);

    *prop_provider = g_system_logservice_data_actioninfo_provider;
    *count = sizeof(g_system_logservice_data_actioninfo_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}



gint32 system_provider_logservice_collect_sel_action_info_entry(PROVIDER_PARAS_S *i_paras,
    PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gint32 ret;

    ret = redfish_check_system_uri_valid(i_paras->uri);
    return_val_if_fail(ret, HTTP_NOT_FOUND);

    *prop_provider = g_system_logservice_collect_sel_actioninfo_provider;
    *count = sizeof(g_system_logservice_collect_sel_actioninfo_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}



gint32 system_provider_logservice_query_sel_log_action_info_entry(PROVIDER_PARAS_S *i_paras,
    PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gint32 ret;

    ret = redfish_check_system_uri_valid(i_paras->uri);
    return_val_if_fail(ret, HTTP_NOT_FOUND);

    *prop_provider = g_system_logservice_query_sel_log_actioninfo_provider;
    *count = sizeof(g_system_logservice_query_sel_log_actioninfo_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}
