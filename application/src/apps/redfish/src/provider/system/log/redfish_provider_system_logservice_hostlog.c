
#include "redfish_provider.h"
#include "redfish_provider_system.h"

LOCAL gint32 get_logservice_hostlog_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_logservice_hostlog_datetime(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_logservice_hostlog_time_offset(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_logservice_hostlog_time_offset(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 act_logservice_hostlog_push_log(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 get_logservice_hostlog_act_message(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_logservice_hostlog_action_info_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);



LOCAL PROPERTY_PROVIDER_S g_system_logservice_hostlog_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_hostlog_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_LOGSERVICE_DATE_TIME, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_hostlog_datetime, NULL, NULL, ETAG_FLAG_DISABLE},
    {RFPROP_LOGSERVICE_TIME_OFFSET, CLASS_BMC, PROPERTY_BMC_TIMEZONE_STR, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_hostlog_time_offset, set_logservice_hostlog_time_offset, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ACTIONS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_hostlog_act_message, NULL, NULL, ETAG_FLAG_ENABLE},

    {RFPROP_LOGSERVICE_HOSTLOG_PUSH_METHOD, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID,  NULL, NULL, act_logservice_hostlog_push_log, ETAG_FLAG_ENABLE}
};

LOCAL PROPERTY_PROVIDER_S g_system_logservice_hostlog_data_actioninfo_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_hostlog_action_info_odata_id, NULL, NULL, ETAG_FLAG_ENABLE}
};

LOCAL gint32 __is_logservice_hostlog_service_configured(void)
{
    OBJ_HANDLE storage_diagnose_handle = 0;
    guint8 pd_log_enable = 0;

    // 没有配置StorageDiagnoseCfg对象的，返回不支持
    if (dal_get_object_handle_nth(CLASS_STORAGE_DIAGNOSE, 0, &storage_diagnose_handle) != RET_OK) {
        return FALSE;
    }

    if (storage_diagnose_handle == 0) {
        return FALSE;
    }

    // 获取误码诊断功能属性看是否支持该功能
    (void)dal_get_property_value_byte(storage_diagnose_handle, PROPERTY_STORAGE_DIAGNOSE_PD_LOG_ENABLE, &pd_log_enable);
    if (pd_log_enable != TRUE) {
        return FALSE;
    }

    return TRUE;
}


gint32 check_logservice_hostlog_service_support(const gchar *session_id)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    GSList *input_list = NULL;

    return_val_do_info_if_fail((NULL != session_id), VOS_ERR,
        debug_log(DLOG_INFO, "[%s] input param error", __FUNCTION__));

    if (__is_logservice_hostlog_service_configured() == FALSE) {
        return RET_ERR;
    }

    ret = dal_get_object_handle_nth(CLASS_SESSION, 0, &obj_handle);
    return_val_if_fail(VOS_OK == ret, VOS_ERR);

    input_list = g_slist_append(input_list, g_variant_new_string(session_id));

    // 调用方法判断session_id是不是框内redfish会话
    ret = dfl_call_class_method(obj_handle, METHOD_SESSION_ISREDFISHINNERSESSION, NULL, input_list, NULL);

    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);

    return_val_do_info_if_fail((VOS_OK == ret), VOS_ERR,
        debug_log(DLOG_INFO, "[%s]: dfl_call_class_method %s fail", __FUNCTION__,
        METHOD_SESSION_ISREDFISHINNERSESSION));

    return VOS_OK;
}


LOCAL gint32 get_logservice_hostlog_action_info_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar action_odata_id[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};

    
    ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_if_fail(VOS_OK == ret, ret);

    
    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s redfish_get_board_slot fail.", __FUNCTION__));

    
    ret = snprintf_s(action_odata_id, sizeof(action_odata_id), sizeof(action_odata_id) - 1,
        RFPROP_LOGSERVICE_HOSTLOG_ACTION_ODATA_ID, slot_id);
    return_val_do_info_if_fail(ERROR_SNPRINTF < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s:snprintf_s fail.", __FUNCTION__));

    *o_result_jso = json_object_new_string(action_odata_id);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s:json_object_new_string fail.", __FUNCTION__));

    return HTTP_OK;
}


LOCAL gint32 get_logservice_hostlog_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar odata_id[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};

    
    ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_if_fail(VOS_OK == ret, ret);

    
    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s:redfish_get_board_slot fail.", __FUNCTION__));

    
    ret = snprintf_s(odata_id, sizeof(odata_id), sizeof(odata_id) - 1, RFPROP_LOGSERVICE_HOTLOG_ODATA_ID, slot_id);
    return_val_do_info_if_fail(ERROR_SNPRINTF < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s:snprintf_s fail.", __FUNCTION__));

    *o_result_jso = json_object_new_string(odata_id);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s:json_object_new_string fail.", __FUNCTION__));

    return HTTP_OK;
}


LOCAL gint32 get_logservice_hostlog_datetime(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    
    ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_if_fail(VOS_OK == ret, ret);

    
    ret = get_manager_datetime(i_paras, o_message_jso, o_result_jso);
    return_val_do_info_if_fail(HTTP_OK == ret, ret,
        debug_log(DLOG_INFO, "%s get log service date time failed.", __FUNCTION__));

    return HTTP_OK;
}


LOCAL gint32 get_logservice_hostlog_time_offset(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    
    ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_if_fail(VOS_OK == ret, ret);

    
    ret = get_manager_timezone(i_paras, o_message_jso, o_result_jso);
    return_val_do_info_if_fail(HTTP_OK == ret, ret,
        debug_log(DLOG_INFO, "%s get log service time zone offset  failed.", __FUNCTION__));

    return HTTP_OK;
}


LOCAL gint32 set_logservice_hostlog_time_offset(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    
    ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_if_fail(VOS_OK == ret, ret);

    
    ret = set_manager_timezone(i_paras, o_message_jso, NULL);
    return_val_do_info_if_fail(HTTP_OK == ret, ret,
        debug_log(DLOG_INFO, "%s set log service time zone offset  failed.", __FUNCTION__));

    return HTTP_OK;
}


LOCAL gint32 get_logservice_hostlog_act_message(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar slot_id[MAX_RSC_ID_LEN] = {0};
    gchar act_info[MAX_URI_LENGTH] = {0};
    gchar target_info[MAX_URI_LENGTH] = {0};
    json_object *obj_hostlog_log = NULL;
    json_object *obj_target_info = NULL;
    json_object *obj_act_info = NULL;
    json_object *huawei = NULL;
    json_object *oem = NULL;

    
    ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_if_fail(VOS_OK == ret, ret);

    
    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s redfish_get_board_slot fail.", __FUNCTION__));

    
    ret = snprintf_s(target_info, sizeof(target_info), sizeof(target_info) - 1, RFPROP_LOGSERVICE_HOSTLOG_TARGET_ID,
        slot_id);
    return_val_do_info_if_fail(ERROR_SNPRINTF < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s snprintf_s fail.", __FUNCTION__));

    obj_target_info = json_object_new_string(target_info);
    return_val_do_info_if_fail(NULL != obj_target_info, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s json_object_new_string fail.", __FUNCTION__));

    
    ret =
        snprintf_s(act_info, sizeof(act_info), sizeof(act_info) - 1, RFPROP_LOGSERVICE_HOSTLOG_ACTIONINFO_ID, slot_id);
    return_val_do_info_if_fail(ERROR_SNPRINTF < ret, HTTP_INTERNAL_SERVER_ERROR, json_object_put(obj_target_info);
        debug_log(DLOG_INFO, "%s: snprintf_s fail..", __FUNCTION__));

    obj_act_info = json_object_new_string(act_info);
    return_val_do_info_if_fail(NULL != obj_act_info, HTTP_INTERNAL_SERVER_ERROR, json_object_put(obj_target_info);
        debug_log(DLOG_INFO, "%s json_object_new_string fail.", __FUNCTION__));

    
    obj_hostlog_log = json_object_new_object();
    return_val_do_info_if_fail(NULL != obj_hostlog_log, HTTP_INTERNAL_SERVER_ERROR, json_object_put(obj_target_info);
        json_object_put(obj_act_info); debug_log(DLOG_INFO, "%s json_object_new_object fail.", __FUNCTION__));

    huawei = json_object_new_object();
    return_val_do_info_if_fail(NULL != obj_hostlog_log, HTTP_INTERNAL_SERVER_ERROR, json_object_put(obj_target_info);
        json_object_put(obj_act_info); json_object_put(obj_hostlog_log);
        debug_log(DLOG_INFO, "%s json_object_new_object fail.", __FUNCTION__));

    oem = json_object_new_object();
    return_val_do_info_if_fail(NULL != obj_hostlog_log, HTTP_INTERNAL_SERVER_ERROR, json_object_put(obj_target_info);
        json_object_put(obj_act_info); json_object_put(obj_hostlog_log); json_object_put(huawei);
        debug_log(DLOG_INFO, "%s json_object_new_object fail.", __FUNCTION__));

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR, json_object_put(obj_target_info);
        json_object_put(obj_act_info); json_object_put(obj_hostlog_log); json_object_put(huawei); json_object_put(oem);
        debug_log(DLOG_INFO, "%s json_object_new_object fail.", __FUNCTION__));

    (void)json_object_object_add(obj_hostlog_log, RFPROP_TARGET, obj_target_info);
    (void)json_object_object_add(obj_hostlog_log, RFPROP_ACTION_INFO, obj_act_info);
    (void)json_object_object_add(huawei, RFPROP_LOGSERVICE_HOSTLOG_PUSH, obj_hostlog_log);
    (void)json_object_object_add(oem, RFPROP_COMMON_MANUFACTURER, huawei);
    (void)json_object_object_add(*o_result_jso, RFPROP_COMMON_OEM, oem);

    return HTTP_OK;
}


LOCAL gint32 act_logservice_hostlog_push_log(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    json_object *logdata_type_jso = NULL;
    json_object *logdata_jso = NULL;

    json_object *logdata_file_suffix_jso = NULL;

    
    
    if (VOS_OK != provider_paras_check(i_paras) || NULL == o_message_jso || NULL == o_result_jso) {
        debug_log(DLOG_ERROR, "%s provider_paras_check fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (__is_logservice_hostlog_service_configured() == FALSE) {
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri);
        return HTTP_NOT_FOUND;
    }

    debug_log(DLOG_DEBUG, "%s handle   %s", __FUNCTION__, dal_json_object_get_str(i_paras->val_jso));
    // 检查"Type": "SMART"
    gint32 ret = json_object_object_get_ex(i_paras->val_jso, RF_LOGSERVICE_LOGDATA_TYPE, &logdata_type_jso);
    return_val_do_info_if_fail(ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    return_val_do_info_if_fail(0 ==
        g_ascii_strcasecmp(RF_LOGSERVICE_LOGDATA_TYPE_SMART, dal_json_object_get_str(logdata_type_jso)),
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    ret = json_object_object_get_ex(i_paras->val_jso, RF_LOGSERVICE_LOGDATA, &logdata_jso);
    return_val_do_info_if_fail(ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    const gchar *logdata_str = dal_json_object_get_str(logdata_jso);
    return_val_do_info_if_fail(NULL != logdata_str, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    ret = json_object_object_get_ex(i_paras->val_jso, RF_LOGSERVICE_FILE_SUFFIX, &logdata_file_suffix_jso);
    return_val_do_info_if_fail(ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    const gchar *logdate_file_suffix_str = dal_json_object_get_str(logdata_file_suffix_jso);
    return_val_do_info_if_fail(NULL != logdate_file_suffix_str, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    debug_log(DLOG_DEBUG, "%s uip_call_class_method_redfish   param  %s  %s", __FUNCTION__, logdata_str,
        logdate_file_suffix_str);
    input_list = g_slist_append(input_list, g_variant_new_string(logdata_str));
    input_list = g_slist_append(input_list, g_variant_new_string(logdate_file_suffix_str));

    
    ret = dal_get_object_handle_nth(CLASS_STORAGE_DIAGNOSE, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, RF_ACTION_NOT_SUPPORTED,
        (void)create_message_info(MSGID_ACT_NOT_SUPPORTED, NULL, o_message_jso, RFPROP_LOGSERVICE_HOSTLOG_PUSH_METHOD);
        uip_free_gvariant_list(input_list));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_STORAGE_DIAGNOSE, METHOD_STORAGE_DIAGNOSE_HANDLE_POST_MESSAGE, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, NULL);
    uip_free_gvariant_list(input_list);
    debug_log(DLOG_DEBUG, "%s uip_call_class_method_redfish  return %d", __FUNCTION__, ret);
    switch (ret) {
        case VOS_OK:
        case RFERR_SUCCESS:
            return HTTP_OK;

            
        case RFERR_NO_RESOURCE:
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri);
            return HTTP_NOT_FOUND;

            
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
            return RFERR_INSUFFICIENT_PRIVILEGE;

            
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


gint32 system_provider_logservice_hostlog_hostlog_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret;

    
    return_val_if_fail(NULL != i_paras && NULL != prop_provider && NULL != count, HTTP_INTERNAL_SERVER_ERROR);

    ret = redfish_check_system_uri_valid(i_paras->uri);
    return_val_if_fail(ret, HTTP_NOT_FOUND);

    ret = check_logservice_hostlog_service_support(i_paras->session_id);
    return_val_if_fail((VOS_OK == ret), HTTP_NOT_FOUND);

    *prop_provider = g_system_logservice_hostlog_provider;
    *count = sizeof(g_system_logservice_hostlog_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}


gint32 system_provider_logservice_hostlog_action_info_entry(PROVIDER_PARAS_S *i_paras,
    PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gint32 ret;

    
    return_val_if_fail(NULL != i_paras && NULL != prop_provider && NULL != count, HTTP_INTERNAL_SERVER_ERROR);

    ret = check_logservice_hostlog_service_support(i_paras->session_id);
    return_val_if_fail((VOS_OK == ret), HTTP_NOT_FOUND);

    ret = redfish_check_system_uri_valid(i_paras->uri);
    return_val_if_fail(ret, HTTP_NOT_FOUND);

    *prop_provider = g_system_logservice_hostlog_data_actioninfo_provider;
    *count = sizeof(g_system_logservice_hostlog_data_actioninfo_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}

