

#include "redfish_provider.h"

#include "redfish_event.h"

#define EVENT_CODE_STR_LEN      20

LOCAL gint32 set_evt_subscription_DeliveryRetryPolicy(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso);
LOCAL gint32 set_evt_subscription_Status(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso);
LOCAL gint32 evt_subscription_recover_provider_entry(PROVIDER_PARAS_S *i_paras,
    json_object **o_message_jso, json_object **o_result_jso);

gint32 server_identity_source_trans(guchar *source_index, gchar *str_buf, guint32 str_buf_len,
    IDENTITY_TRANS_DIRECTION direction)
{
    errno_t safe_fun_ret = EOK;
    INT2STR_MAP_S server_identity_source_arr[] = {
        {TRAP_ID_BRDSN,        RF_BRDSN_STR},
        {TRAP_ID_ASSETTAG,     RF_PRODUCT_ASSET_TAG_STR},
        {TRAP_ID_HOSTNAME,     RF_HOST_NAME_STR},
        {TRAP_ID_CHASSIS_NAME,     RF_CHASSIS_NAME_STR},
        {TRAP_ID_CHASSIS_LOCATION,     RF_CHASSIS_LOCATION_STR},
        {TRAP_ID_CHASSIS_SERIAL_NUMBER,     RF_CHASSIS_SERIAL_NUMBER_STR}
    };
    guint32 i;
    guint32 len = G_N_ELEMENTS(server_identity_source_arr);
    guint8 index_tmp;
    const gchar *str_tmp = NULL;

    return_val_do_info_if_fail((NULL != source_index) && (NULL != str_buf), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    
    if (IDENTITY_STR_TO_INDEX == direction) {
        for (i = 0; i < len; i++) {
            if (!g_strcmp0(str_buf, server_identity_source_arr[i].str_val)) {
                *source_index = (guchar)i;

                return VOS_OK;
            }
        }

        debug_log(DLOG_ERROR, "unknown server identity source:%s", str_buf);
    } else if (IDENTITY_INDEX_TO_STR == direction) {
        
        index_tmp = *source_index;

        return_val_do_info_if_expr((index_tmp >= len) || (str_buf_len == 0), VOS_ERR,
            debug_log(DLOG_ERROR, "invalid server identity index:%d or str_buf_len:%d", index_tmp, str_buf_len));

        str_tmp = server_identity_source_arr[index_tmp].str_val;
        safe_fun_ret = strncpy_s(str_buf, str_buf_len, str_tmp, str_buf_len - 1);
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
        return VOS_OK;
    } else {
        debug_log(DLOG_ERROR, "error use %s:invalid direction param:%d", __FUNCTION__, direction);
    }

    return VOS_ERR;
}


LOCAL gint32 set_evt_svc_server_identity_source(PROVIDER_PARAS_S *i_paras, json_object *huawei_jso,
    json_object **o_message_jso)
{
    guchar server_identity_source_index = 0;
    gchar server_identity_source_str[MAX_SERVER_IDENTITY_LEN] = {0};
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    GSList *caller_info = NULL;
    gint32 ret = 0;
    json_object *message_jso = NULL;

    ret = json_custom_get_elem_str(huawei_jso, RFPROP_SERVER_IDENTITY_SOURCE, server_identity_source_str,
        MAX_SERVER_IDENTITY_LEN);
    return_val_do_info_if_fail((VOS_OK == ret), HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "server identity source prop not found"));

    
    (void)server_identity_source_trans(&server_identity_source_index, server_identity_source_str,
        MAX_SERVER_IDENTITY_LEN, IDENTITY_STR_TO_INDEX);

    ret = dal_get_object_handle_nth(CLASS_RF_EVT_SVC, 0, &obj_handle);
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "get object handle for %s failed", CLASS_RF_EVT_SVC);

        create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
        json_object_array_add(*o_message_jso, message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(server_identity_source_index));

    
    caller_info =
        g_slist_append(caller_info, g_variant_new_string(dal_rf_get_op_log_module_name(i_paras->is_from_webui)));
    
    caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->user_name));
    caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->client));

    
    (void)set_redfish_evt_server_identity_source(obj_handle, caller_info, input_list, NULL);
    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(caller_info);

    return HTTP_OK;
}

LOCAL gint32 set_evt_svc_server_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    json_object *huawei_jso = NULL;
    json_object *tmp_jso = NULL;
    gint32 result = HTTP_BAD_REQUEST;
    gint32 ret;

    if ((i_paras->user_role_privilege & USERROLE_BASICSETTING) == 0) {
        debug_log(DLOG_ERROR, "%s failed:insufficient privilege", __FUNCTION__);
        if (!dal_is_customized_by_cmcc()) {
            create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, "Oem/Huawei", o_message_jso, "Oem/Huawei");
        }
        return HTTP_FORBIDDEN;
    }

    
    json_object_object_get_ex(i_paras->val_jso, RFPROP_COMMON_HUAWEI, &huawei_jso);
    if (huawei_jso == NULL) {
        return HTTP_BAD_REQUEST;
    }

    *o_message_jso = json_object_new_array();
    if (*o_message_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_array failed.", __FUNCTION__);
        create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    json_object_object_get_ex(huawei_jso, RFPROP_SERVER_IDENTITY_SOURCE, &tmp_jso);
    if (tmp_jso != NULL) {
        result = set_evt_svc_server_identity_source(i_paras, huawei_jso, o_message_jso);
    }
    tmp_jso = NULL;

    
    json_object_object_get_ex(huawei_jso, RFPROP_EVT_SHIELD_RESOURCE, &tmp_jso);
    if (tmp_jso != NULL) {
        ret = set_evt_svc_shield_resources(i_paras, tmp_jso, *o_message_jso);
        if (ret == HTTP_OK) {
            return HTTP_OK;
        }
    }
    return result;
}


LOCAL gint32 set_evt_svc_service_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guchar post_enabled = 0;
    GSList *input_list = NULL;
    GSList *caller_info = NULL;
    OBJ_HANDLE obj_handle = 0;
    gint32 ret = 0;

    if (0 == (i_paras->user_role_privilege & USERROLE_BASICSETTING)) {
        debug_log(DLOG_ERROR, "%s failed:insufficient privilege", __FUNCTION__);

        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_SERVICE_ENABLED, o_message_jso,
            RFPROP_SERVICE_ENABLED);

        return HTTP_FORBIDDEN;
    }

    
    return_val_do_info_if_fail((NULL != i_paras->val_jso), HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "null type value no supported to config");
        (void)create_message_info(MSGID_PROP_TYPE_ERR, RFPROP_SERVICE_ENABLED, o_message_jso, RF_NULL_STR,
        RFPROP_SERVICE_ENABLED));
    

    post_enabled = json_object_get_boolean(i_paras->val_jso);

    ret = dal_get_object_handle_nth(CLASS_RF_EVT_SVC, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "cann't get object handle for %s", CLASS_RF_EVT_SVC);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    input_list = g_slist_append(input_list, g_variant_new_byte(post_enabled));

    
    caller_info =
        g_slist_append(caller_info, g_variant_new_string(dal_rf_get_op_log_module_name(i_paras->is_from_webui)));
    
    caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->user_name));
    caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->client));

    
    (void)set_redfish_evt_service_enabled(obj_handle, caller_info, input_list, NULL);

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(caller_info);

    return HTTP_OK;
}


LOCAL gint32 act_evt_svc_submit_test_event(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret = 0;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    GSList *caller_info = NULL;

    if (0 == (i_paras->user_role_privilege & USERROLE_BASICSETTING)) {
        debug_log(DLOG_ERROR, "%s failed:insufficient privilege", __FUNCTION__);

        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);

        return HTTP_FORBIDDEN;
    }

    ret = dal_get_object_handle_nth(CLASS_RF_EVT_SVC, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "cann't get object handle for %s", CLASS_RF_EVT_SVC);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "Submit Redfish test events failed"));

    input_list = g_slist_append(input_list, g_variant_new_byte(EVENT_MONITOR_MODE_TEST));
    
    input_list = g_slist_append(input_list, g_variant_new_string(dal_json_object_get_str(i_paras->val_jso)));
    
    input_list = g_slist_append(input_list, g_variant_new_byte(0));

    
    caller_info =
        g_slist_append(caller_info, g_variant_new_string(dal_rf_get_op_log_module_name(i_paras->is_from_webui)));
    
    caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->user_name));
    caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->client));

    ret = redfish_post_event_msg(obj_handle, caller_info, input_list, NULL);

    uip_free_gvariant_list(input_list);

    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "post event message failed, ret is %d", ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "Submit Redfish test events failed");
        uip_free_gvariant_list(caller_info);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "Submit Redfish test events successfully.");

    uip_free_gvariant_list(caller_info);

    return HTTP_OK;
}


LOCAL gint32 act_evt_svc_rearm(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    GSList *input_list = NULL;

    if (0 == (i_paras->user_role_privilege & USERROLE_BASICSETTING)) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);

        return HTTP_FORBIDDEN;
    }

    input_list = g_slist_append(input_list, g_variant_new_uint32(REARM_SEQ_ALL));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, OBJ_HANDLE_COMMON,
        CLASS_SEL, METHOD_SEL_REARM_EVENTS, AUTH_DISABLE, 0, input_list, NULL);
    uip_free_gvariant_list(input_list);

    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "call rearm method failed, ret is %d", ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    return HTTP_OK;
}
LOCAL gint32 set_evt_svc_property_value_int32(
    PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso, const gchar *property_name)
{
    gint32 config_value = 0;
    GSList *caller_info = NULL;
    OBJ_HANDLE obj_handle = 0;
    gint32 ret = 0;

    if ((i_paras->user_role_privilege & USERROLE_BASICSETTING) == 0) {
        debug_log(DLOG_ERROR, "%s failed:insufficient privilege", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, property_name, o_message_jso, property_name);
        return HTTP_FORBIDDEN;
    }

    if (i_paras->val_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: null type value no supported to config", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_TYPE_ERR, property_name, o_message_jso, RF_NULL_STR, property_name);
        return HTTP_BAD_REQUEST;
    }

    ret = dal_get_object_handle_nth(CLASS_RF_EVT_SVC, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "cann't get object handle for %s", CLASS_RF_EVT_SVC);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    config_value = json_object_get_int(i_paras->val_jso);

    caller_info = g_slist_append(caller_info,
        g_variant_new_string(dal_rf_get_op_log_module_name(i_paras->is_from_webui)));
    caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->user_name));
    caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->client));

    (void)dal_save_property_value_int32_remote(obj_handle, property_name, config_value, DF_SAVE);
    if (strcmp(property_name, PROPERTY_RF_EVT_SVC_EVENT_RETRY_TIMES) == 0) {
        proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH,
            "Set Redfish event service delivery retry times to %d successfully.", config_value);
    } else if (strcmp(property_name, PROPERTY_RF_EVT_SVC_EVENT_DELAY_INSEC) == 0) {
        proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH,
            "Set Redfish event service delivery retry interval to %d seconds successfully.", config_value);
    }

    uip_free_gvariant_list(caller_info);

    return HTTP_OK;
}

LOCAL gint32 set_evt_svc_delivery_attempts(
    PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    return set_evt_svc_property_value_int32(i_paras, o_message_jso, o_result_jso,
        PROPERTY_RF_EVT_SVC_EVENT_RETRY_TIMES);
}

LOCAL gint32 set_evt_svc_delivery_interval(
    PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    return set_evt_svc_property_value_int32(i_paras, o_message_jso, o_result_jso,
        PROPERTY_RF_EVT_SVC_EVENT_DELAY_INSEC);
}


LOCAL gint32 check_precisealarm_param(json_object *val_jso, json_object **o_message_jso, PRECISE_ALARM_S *precise_alarm)
{
    errno_t safe_fun_ret = EOK;
    gint32 bool_ret = 0;
    const gchar *type = NULL;
    gint32 sub_num = 0;
    const gchar *event_code = NULL;

    json_object *obj_json = NULL;

    return_val_if_expr(NULL == precise_alarm, VOS_ERR);

    
    bool_ret = json_object_object_get_ex(val_jso, RFACTION_PARAM_TYPE, &obj_json);
    if (FALSE == bool_ret || NULL == obj_json) {
        debug_log(DLOG_ERROR, "%s, %d, wrong value type,  %s", __FUNCTION__, __LINE__, RFACTION_PARAM_TYPE);
        (void)create_message_info(MSGID_PROP_MISSING, RFACTION_PARAM_TYPE, o_message_jso, RFACTION_PARAM_TYPE);
        return HTTP_BAD_REQUEST;
    }

    
    type = dal_json_object_get_str(obj_json);
    return_val_do_info_if_fail(NULL != type, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: dal_json_object_get_str fail\n", __FUNCTION__, __LINE__));

    obj_json = NULL;

    
    (void)json_object_object_get_ex(val_jso, RFACTION_PRECISEALARM_SUBJECTINDEX, &obj_json);

    if (NULL != obj_json) {
        return_val_do_info_if_expr(json_type_int != json_object_get_type(obj_json), HTTP_BAD_REQUEST,
            (void)create_message_info(MSGID_ACT_PARA_TYPE_ERR, RFACTION_PRECISEALARM_SUBJECTINDEX, o_message_jso,
            dal_json_object_get_str(obj_json), RFACTION_PRECISEALARM_SUBJECTINDEX, RFACTION_PRECISEALARM));
        sub_num = json_object_get_int(obj_json);
    }

    obj_json = NULL;

    
    (void)json_object_object_get_ex(val_jso, RFACTION_PRECISEALARM_EVENTCODE, &obj_json);

    if (NULL != obj_json) {
        event_code = dal_json_object_get_str(obj_json);
    }

    if (0 != g_strcmp0(type, RFACTION_PRECISEALARM_TYPE_CANCEL)) {
        if (NULL == event_code) {
            debug_log(DLOG_ERROR, "%s, %d, wrong value type,  %s", __FUNCTION__, __LINE__,
                RFACTION_PRECISEALARM_EVENTCODE);
            (void)create_message_info(MSGID_PROP_MISSING, RFACTION_PRECISEALARM_EVENTCODE, o_message_jso,
                RFACTION_PRECISEALARM_EVENTCODE);
            return HTTP_BAD_REQUEST;
        }

        // 判断code:0x**ffffffff.  是否同时传有subjectindex
        bool_ret = g_regex_match_simple(RFACTION_PRECISEALARM_EVENT_CODE_REGEX, event_code, G_REGEX_OPTIMIZE,
            (GRegexMatchFlags)0);
        // 全部/主体全部告警
        if (TRUE == bool_ret) {
            return_val_do_info_if_expr(0 != sub_num, HTTP_BAD_REQUEST,
                (void)create_message_info(MSGID_PROP_NOT_REQUIRED, RFACTION_PRECISEALARM_SUBJECTINDEX, o_message_jso,
                RFACTION_PRECISEALARM_SUBJECTINDEX));
        }
    } else {
        // 判断是否携带event_code or subjectindex
        return_val_do_info_if_expr(NULL != event_code, HTTP_BAD_REQUEST,
            (void)create_message_info(MSGID_PROP_NOT_REQUIRED, RFACTION_PRECISEALARM_EVENTCODE, o_message_jso,
            RFACTION_PRECISEALARM_EVENTCODE));
        return_val_do_info_if_expr(0 != sub_num, HTTP_BAD_REQUEST,
            (void)create_message_info(MSGID_PROP_NOT_REQUIRED, RFACTION_PRECISEALARM_SUBJECTINDEX, o_message_jso,
            RFACTION_PRECISEALARM_SUBJECTINDEX));
    }

    (void)strncpy_s(precise_alarm->action_type, PROPERTY_NAME_LENGTH, type, PROPERTY_NAME_LENGTH - 1);
    // 0xffff ffff
    do_val_if_expr(NULL != event_code,
        (void)strncpy_s(precise_alarm->event_code, PROPERTY_NAME_LENGTH, event_code, PROPERTY_NAME_LENGTH - 1);
        (void)strncpy_s(precise_alarm->event_code_high, PROPERTY_NAME_LENGTH, event_code, 4 + 2);
        (void)strncpy_s(precise_alarm->event_code_low, PROPERTY_NAME_LENGTH, "0x", strlen("0x"));
        safe_fun_ret = strncat_s(precise_alarm->event_code_low, PROPERTY_NAME_LENGTH, event_code + 4 + 2, 4));
    return_val_do_info_if_expr(safe_fun_ret != EOK, HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    precise_alarm->subobject_index = sub_num;

    return VOS_OK;
}


LOCAL gint32 act_evt_svc_precisealarm(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret = 0;
    gint32 ret2 = 0;
    gint32 bool_ret = 0;
    PRECISE_ALARM_S precise_alarm;

    GSList *input_list = NULL;
    guint32 eventcode_high = 0;
    guint32 eventcode_lower = 0;
    guint32 eventcode = 0;
    gint32 subject_indx = 0;
    guchar ismock = 0;
    guchar isalarm = 0;

    gchar msg_eventcode[PROPERTY_NAME_LENGTH] = {0};
    gchar msg_subject_indx[PROPERTY_NAME_LENGTH] = {0};

    if (0 == (i_paras->user_role_privilege & USERROLE_BASICSETTING)) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    (void)memset_s(&precise_alarm, sizeof(precise_alarm), 0, sizeof(precise_alarm));
    ret = check_precisealarm_param(i_paras->val_jso, o_message_jso, &precise_alarm);
    return_val_if_expr(VOS_OK != ret, ret);

    debug_log(DLOG_DEBUG, "%s, %d,precise_alarm.eventcode :%s, %s %s", __FUNCTION__, __LINE__, precise_alarm.event_code,
        precise_alarm.event_code_high, precise_alarm.event_code_low);

    
    //
    if (0 != g_strcmp0(RFACTION_PRECISEALARM_TYPE_CANCEL, precise_alarm.action_type)) {
        ret = vos_str2int(precise_alarm.event_code_high, 16, &eventcode_high, NUM_TPYE_UINT32);
        ret2 = vos_str2int(precise_alarm.event_code_low, 16, &eventcode_lower, NUM_TPYE_UINT32);

        do_val_if_expr(VOS_OK != ret || VOS_OK != ret2,
            debug_log(DLOG_DEBUG, "%s, %d,ret:%d", __FUNCTION__, __LINE__, ret));

        eventcode = eventcode_high << 16 | eventcode_lower;

        bool_ret = g_regex_match_simple(RFACTION_PRECISEALARM_EVENT_CODE_REGEX, precise_alarm.event_code,
            G_REGEX_OPTIMIZE, (GRegexMatchFlags)0);
        // 全部/主体全部告警
        if (TRUE == bool_ret) {
            subject_indx = 0;
        } else { // 普通某条告警
            subject_indx = precise_alarm.subobject_index;
            // 校验EventCode/SubojectIndex
            // 底层方法调用判断
        }

        // 产生
        if (0 == g_strcmp0(RFACTION_PRECISEALARM_TYPE_ASSERT, precise_alarm.action_type)) {
            isalarm = ENABLE;
        } else if (0 == g_strcmp0(RFACTION_PRECISEALARM_TYPE_DEASSERT, precise_alarm.action_type)) { // 恢复
            isalarm = DISABLE;
        }
        
        ismock = ENABLE;
        
    } else { // 取消模拟告警
        eventcode = 0xffffffff;
        subject_indx = 0;
        isalarm = DISABLE;
        
        ismock = DISABLE;
        
    }

    
    input_list = g_slist_append(input_list, g_variant_new_uint32(eventcode));
    input_list = g_slist_append(input_list, g_variant_new_byte(ismock));
    input_list = g_slist_append(input_list, g_variant_new_uint16(subject_indx));
    // 产生告警，恢复告警
    input_list = g_slist_append(input_list, g_variant_new_byte(isalarm));
    

    debug_log(DLOG_DEBUG, "%s, %d, eventcode :0x%x,  subject_indx:%d,  isalarm:%d, ismock:%d", __FUNCTION__, __LINE__,
        eventcode, subject_indx, isalarm, ismock);

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, OBJ_HANDLE_COMMON,
        CLASS_EVENT_INFORMATION, METHOD_EVENT_MONITOR_PRESICE_ALARM_SIMULATE, AUTH_DISABLE, 0, input_list, NULL);
    uip_free_gvariant_list(input_list);

    
    (void)snprintf_s(msg_eventcode, PROPERTY_NAME_LENGTH, PROPERTY_NAME_LENGTH - 1, "0x%08X", eventcode);
    
    (void)snprintf_s(msg_subject_indx, PROPERTY_NAME_LENGTH, PROPERTY_NAME_LENGTH - 1, "%d", subject_indx);

    
    switch (ret) {
        case VOS_OK:
        case RFERR_SUCCESS:
            return HTTP_OK;
            // 系统忙
        case INTERNAL_PROCESS_QUEUE_BUSY:
            (void)create_message_info(MSGID_RSC_IN_USE, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        case INVALID_EVENT_CODE_VAL:
            
        case SUBJECT_EVENT_OUT_OF_RANGE:
            
            debug_log(DLOG_ERROR, "%s, %d, wrong ret :%d, ", __FUNCTION__, __LINE__, ret);
            (void)create_message_info(MSGID_PROP_INVALID_VALUE, RFACTION_PRECISEALARM_EVENTCODE, o_message_jso,
                (const gchar *)msg_eventcode, RFACTION_PRECISEALARM_EVENTCODE);
            return HTTP_BAD_REQUEST;

        case INVALID_SUBJECT_INDEX_VAL:
            debug_log(DLOG_ERROR, "%s, %d, wrong ret :%d, ", __FUNCTION__, __LINE__, ret);
            (void)create_message_info(MSGID_PROP_INVALID_VALUE, RFACTION_PRECISEALARM_SUBJECTINDEX, o_message_jso,
                (const gchar *)msg_subject_indx, RFACTION_PRECISEALARM_SUBJECTINDEX);
            return HTTP_BAD_REQUEST;

        default: 
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            debug_log(DLOG_ERROR, "%s, %d, wrong ret :%d, ", __FUNCTION__, __LINE__, ret);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}

LOCAL gint32 get_shieldalert_param(json_object* val_jso, json_object** o_message_jso, SHIELD_ALARM_S* shield_msg)
{
    gint32 ret;
    gint32 shield_enabled = 0;
    const  gchar* event_code = NULL;

    ret = get_element_boolean(val_jso, RF_STATE_ENABLED, &shield_enabled);
    if (ret == FALSE) {
        debug_log(DLOG_ERROR, "%s: wrong value type,  %s", __FUNCTION__, RF_STATE_ENABLED);
        (void)create_message_info(MSGID_ACT_PARA_MISSING, RF_STATE_ENABLED, o_message_jso,
            RFACTION_SHIELD_SYSTEM_ALERT, RF_STATE_ENABLED);
        return HTTP_BAD_REQUEST;
    }

    
    (void)get_element_str(val_jso, RFACTION_PRECISEALARM_EVENTCODE, &event_code);
    if (event_code == NULL) {
        debug_log(DLOG_ERROR, "%s: wrong event_code value type, %s", __FUNCTION__, RFACTION_PRECISEALARM_EVENTCODE);
        (void)create_message_info(MSGID_ACT_PARA_MISSING, RFACTION_PRECISEALARM_EVENTCODE, o_message_jso, 
            RFACTION_SHIELD_SYSTEM_ALERT, RFACTION_PRECISEALARM_EVENTCODE);
        return HTTP_BAD_REQUEST;
    }
    shield_msg->shield_state = (shield_enabled == TRUE) ? SHIELD_EVENT : UNSHIELD_EVENT;
    (void)strncpy_s(shield_msg->event_code, PROPERTY_NAME_LENGTH, event_code, strlen(event_code));
    (void)strncpy_s(shield_msg->event_code_high, PROPERTY_NAME_LENGTH, event_code, 6); // 6表示“0x”长度加高位4字节
    (void)strncpy_s(shield_msg->event_code_low, PROPERTY_NAME_LENGTH, "0x", strlen("0x"));
    (void)strncat_s(shield_msg->event_code_low, PROPERTY_NAME_LENGTH, event_code + 6, 4); // 6即起始位置，4取低位4字节
    return RET_OK;
}

LOCAL gint32 act_evt_svc_shieldalert(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso)
{
    gint32 ret;
    GSList* input_list = NULL;
    SHIELD_ALARM_S shield_msg;
    guint32 eventcode;
    guint32 eventcode_high = 0;
    guint32 eventcode_low = 0;
    gchar msg_eventcode[PROPERTY_NAME_LENGTH] = {0};

    if (provider_paras_check(i_paras) != RET_OK || o_message_jso == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: para error", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if ((i_paras->user_role_privilege & USERROLE_BASICSETTING) == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    (void)memset_s(&shield_msg, sizeof(shield_msg), 0, sizeof(shield_msg));
    ret = get_shieldalert_param(i_paras->val_jso, o_message_jso, &shield_msg);
    if (ret != RET_OK) {
        return ret;
    }

    if (vos_str2int(shield_msg.event_code_high, 16, &eventcode_high, NUM_TPYE_UINT32) != RET_OK || // 16表示转为十六进制
        vos_str2int(shield_msg.event_code_low, 16, &eventcode_low, NUM_TPYE_UINT32) != RET_OK) { // 16表示转为十六进制
        debug_log(DLOG_ERROR, "%s: vos_str2int failed", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    eventcode = (eventcode_high << 16) | eventcode_low;  // 16表示事件码原高位左移16位和低字节拼接成完成原始事件码
    (void)snprintf_s(msg_eventcode, PROPERTY_NAME_LENGTH, PROPERTY_NAME_LENGTH - 1, "0x%08X", eventcode);
    input_list = g_slist_append(input_list, g_variant_new_uint32(eventcode));
    input_list = g_slist_append(input_list, g_variant_new_byte(shield_msg.shield_state));
    input_list = g_slist_append(input_list, g_variant_new_byte(SHIELD_AC_PER_SAVE));
    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, OBJ_HANDLE_COMMON, 
        CLASS_EVENT_INFORMATION, METHOD_EVENT_MONITOR_SHIELD, AUTH_DISABLE, 0, input_list, NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case RET_OK:
            return HTTP_OK;

        case INVALID_EVENT_CODE_VAL:
            (void)create_message_info(MSGID_PROP_INVALID_VALUE, RFACTION_PRECISEALARM_EVENTCODE, 
                o_message_jso, (const gchar*)msg_eventcode, RFACTION_PRECISEALARM_EVENTCODE);
            return HTTP_BAD_REQUEST;

        default:    
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            debug_log(DLOG_ERROR, "%s call shield alarm method failed, ret is %d ", __FUNCTION__, ret);	
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}

LOCAL PROPERTY_PROVIDER_S g_event_service_provider[] = {
    {
        RFPROP_COMMON_OEM,
        "", "",
        USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID,
        NULL,
        set_evt_svc_server_oem,
        NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_SERVICE_ENABLED,
        "", "",
        USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID,
        NULL,
        set_evt_svc_service_enabled,
        NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFACTION_SUBMIT_TEST_EVENT,
        "", "",
        USERROLE_BASICSETTING,
        SYS_LOCKDOWN_ALLOW,
        NULL,
        NULL,
        act_evt_svc_submit_test_event,
        ETAG_FLAG_ENABLE
    },
    {
        RFACTION_REARM,
        "", "",
        USERROLE_BASICSETTING,
        SYS_LOCKDOWN_ALLOW,
        NULL,
        NULL,
        act_evt_svc_rearm,
        ETAG_FLAG_ENABLE
    },
    
    {
        RFACTION_PRECISEALARM,
        "", "",
        USERROLE_BASICSETTING,
        SYS_LOCKDOWN_ALLOW,
        NULL,
        NULL,
        act_evt_svc_precisealarm,
        ETAG_FLAG_ENABLE
    },
    {
        RFACTION_SHIELD_SYSTEM_ALERT,
        "", "",
        USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, 
        NULL,
        NULL,
        act_evt_svc_shieldalert,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_DELIVERY_RETRY_ATTEMPTS,
        "", "",
        USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID,
        NULL,
        set_evt_svc_delivery_attempts,
        NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_DELIVERY_RETRY_INTERVAL,
        "", "",
        USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID,
        NULL,
        set_evt_svc_delivery_interval,
        NULL,
        ETAG_FLAG_ENABLE
    },
};


gint32 evt_svc_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    
    return_val_if_fail(NULL != i_paras && NULL != prop_provider && NULL != count, HTTP_INTERNAL_SERVER_ERROR);

    *prop_provider = g_event_service_provider;
    *count = sizeof(g_event_service_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}


LOCAL gint32 set_evt_subscription_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    const gchar *evt_subscription_context = NULL;
    guint32 len;
    GSList *input_list = NULL;
    GSList *caller_info = NULL;

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_message_jso) && (NULL != o_result_jso),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    if (0 == (i_paras->user_role_privilege & USERROLE_BASICSETTING)) {
        debug_log(DLOG_ERROR, "%s failed:insufficient privilege", __FUNCTION__);

        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_CONTEXT, o_message_jso, RFPROP_CONTEXT);

        return HTTP_FORBIDDEN;
    }

    evt_subscription_context = dal_json_object_get_str(i_paras->val_jso);
    if (evt_subscription_context == NULL) {
        debug_log(DLOG_ERROR, "%s failed:get null string from input param.", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    len = strlen(evt_subscription_context);
    
    return_val_do_info_if_fail(len < MAX_COMMON_PROP_VAL_BUF_LEN, HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "event subscription context length exceed, current len:%u", len);
        (void)create_message_info(MSGID_PROP_VAL_EXC_MAXLEN, RFPROP_CONTEXT, o_message_jso, evt_subscription_context,
        RFPROP_CONTEXT, MAX_COMMON_PROP_VAL_LEN_STR));

    input_list = g_slist_append(input_list, g_variant_new_string(evt_subscription_context));
    
    caller_info =
        g_slist_append(caller_info, g_variant_new_string(dal_rf_get_op_log_module_name(i_paras->is_from_webui)));
    
    caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->user_name));
    caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->client));

    (void)set_redfish_evt_subscription_ctx(i_paras->obj_handle, caller_info, input_list, NULL);
    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(caller_info);

    return HTTP_OK;
}


LOCAL gint32 set_evt_subscription_http_headers(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    const gchar *http_headers_str = NULL;
    guint32 len;
    gchar               max_len_buf[MAX_STRBUF_LEN] = {0};
    GSList *input_list = NULL;
    GSList *caller_info = NULL;
    
    GSList *invalid_prop_list = NULL;
    

    if (0 == (i_paras->user_role_privilege & USERROLE_BASICSETTING)) {
        debug_log(DLOG_ERROR, "%s failed:insufficient privilege", __FUNCTION__);

        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_HTTP_HEADERS, o_message_jso, RFPROP_HTTP_HEADERS);

        return HTTP_FORBIDDEN;
    }

    
    
    (void)json_object_get_user_data(i_paras->val_jso, (void **)&invalid_prop_list);
    return_val_do_info_if_fail(NULL == invalid_prop_list, HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "invalid http headers, %s failed", __FUNCTION__));
    

    http_headers_str = dal_json_object_get_str(i_paras->val_jso);

    return_val_do_info_if_fail((NULL != http_headers_str), HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    len = strlen(http_headers_str);
    (void)snprintf_s(max_len_buf, MAX_STRBUF_LEN, MAX_STRBUF_LEN - 1, "%d", MAX_EVT_HEADERS_SIZE - 1);
    return_val_do_info_if_fail(len < MAX_EVT_HEADERS_SIZE, HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "http headers length exceeded, current len:%u", len);
        (void)create_message_info(MSGID_PROP_VAL_EXC_MAXLEN, RFPROP_HTTP_HEADERS, o_message_jso, http_headers_str,
        RFPROP_HTTP_HEADERS, max_len_buf));

    input_list = g_slist_append(input_list, g_variant_new_string(http_headers_str));
    
    caller_info =
        g_slist_append(caller_info, g_variant_new_string(dal_rf_get_op_log_module_name(i_paras->is_from_webui)));
    
    caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->user_name));
    caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->client));

    (void)set_redfish_evt_subscription_http_headers(i_paras->obj_handle, caller_info, input_list, NULL);
    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(caller_info);

    return HTTP_OK;
}


LOCAL gint32 get_evt_subscription_action(PROVIDER_PARAS_S *i_paras, json_object *actions)
{
#define TARGET_LENTH 128
    if (i_paras == NULL || actions == NULL) {
        debug_log(DLOG_ERROR, "%s: input NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    gchar *end_pos = NULL;
    gint32 subscription_id = (gint32)strtol(i_paras->member_id, &end_pos, 10);
        
    if (subscription_id < RF_EVT_SUBSCRIPTION_ID_MIN || subscription_id > RF_EVT_SUBSCRIPTION_ID_MAX) {
        debug_log(DLOG_ERROR, "%s: subscription id(%d) is invalid, str=\"%s\"",
            __FUNCTION__, subscription_id, i_paras->member_id);
        return HTTP_BAD_REQUEST;
    }
    gchar targerStr[TARGET_LENTH] = {0};
    gint32 ret = snprintf_s(targerStr, TARGET_LENTH, TARGET_LENTH - 1, RFPROP_EVENT_ACTION_URL, subscription_id);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s error, ret=%d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    json_object *target = json_object_new_object();
    if (target == NULL) {
        debug_log(DLOG_ERROR, "%s: New object failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    json_object *method = json_object_new_object();
    if (method == NULL) {
        debug_log(DLOG_ERROR, "%s: New object failed.", __FUNCTION__);
        json_object_put(target);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    json_object *action_info = json_object_new_object();
    if (action_info == NULL) {
        debug_log(DLOG_ERROR, "%s: New object failed.", __FUNCTION__);
        json_object_put(target);
        json_object_put(method);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    json_object_object_add(target, RFPROP_TARGET, json_object_new_string(targerStr));
    json_object_object_add(action_info, RFPROP_ACTION_INFO, json_object_new_string(URI_EVENT_RESUME_ACTION_INFO));
    json_object_object_add(method, RFPROP_EVENT_ACTION_METHOD, target);
    json_object_object_add(method, RFPROP_EVENT_ACTION_METHOD, action_info);
    json_object_object_add(actions, RFPROP_ACTIONS, method);
    return HTTP_OK;
}

LOCAL PROPERTY_PROVIDER_S g_event_subscription_provider[] = {
    {
        RFPROP_CONTEXT,
        "", "",
        USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID,
        NULL,
        set_evt_subscription_context,
        NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_HTTP_HEADERS,
        "", "",
        USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID,
        NULL,
        set_evt_subscription_http_headers,
        NULL,
        ETAG_FLAG_ENABLE
    },
    {
        EVENTDESTINATION_RECSUME,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_BASICSETTING,
        SYS_LOCKDOWN_ALLOW,
        NULL, NULL,
        evt_subscription_recover_provider_entry,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_DELIVERY_RETRY_POLICY,
        "", "",
        USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID,
        NULL,
        set_evt_subscription_DeliveryRetryPolicy,
        NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_EVENT_STATUS,
        "", "",
        USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID,
        NULL,
        set_evt_subscription_Status,
        NULL,
        ETAG_FLAG_ENABLE
    },
};


gint32 evt_subscription_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gint32 ret = 0;
    OBJ_HANDLE obj_handle = 0;
    guchar event_subscription_id = 0;
    gchar               evt_destination_buf[MAX_EVT_DESTINATION_LEN] = {0};

    
    return_val_if_fail(NULL != i_paras && NULL != prop_provider && NULL != count, HTTP_INTERNAL_SERVER_ERROR);

    ret = vos_str2int(i_paras->member_id, 10, &event_subscription_id, NUM_TPYE_UCHAR);
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "trans %s to uchar type failed", i_paras->member_id);

        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_specific_object_byte(CLASS_EVT_SUBSCRIPTION, PROPERTY_EVT_SUBSCRIPTION_ID, event_subscription_id,
        &obj_handle);
    return_val_do_info_if_fail((VOS_OK == ret), HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "cann't find event subscription with id %s", i_paras->member_id));

    (void)dal_get_property_value_string(obj_handle, PROPERTY_EVT_SUBSCRIPTION_DES, evt_destination_buf,
        MAX_EVT_DESTINATION_LEN);
    return_val_do_info_if_fail('\0' != *evt_destination_buf, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "cann't find event subscription with id %s", i_paras->member_id));

    i_paras->obj_handle = obj_handle;

    *prop_provider = g_event_subscription_provider;
    *count = sizeof(g_event_subscription_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}


json_object* get_event_list(EVT_TIME_INFO time, gint32 sub_id, gint32 type)
{
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;

    debug_log(DLOG_DEBUG, "%s: starttime = %u endtime = %u, sub_id = %d", __FUNCTION__, time.start, time.end, sub_id);
    gint32 ret = dal_get_object_handle_nth(CLASS_SEL, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get Sel obj failed, ret = %d", __FUNCTION__, ret);
        return NULL;
    }
    input_list = g_slist_append(input_list, g_variant_new_uint32(time.start));
    input_list = g_slist_append(input_list, g_variant_new_uint32(time.end));
    input_list = g_slist_append(input_list, g_variant_new_int32(sub_id));
    input_list = g_slist_append(input_list, g_variant_new_int32(type));

    ret = dfl_call_class_method(obj_handle, METHOD_QUERY_EVENT_LIST_BY_DATE, NULL, input_list, &output_list);
    if (ret != RET_OK) {
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        debug_log(DLOG_ERROR, "%s: call sensor method failed, ret = %d", __FUNCTION__, ret);
        return NULL;
    }
    const gchar* event_list = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    json_object *event_data = json_tokener_parse(event_list);
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    return event_data;
}

void delete_event_log(EVT_TIME_INFO time, gint32 sub_id, gint32 type)
{
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;

    gint32 ret = dal_get_object_handle_nth(CLASS_SEL, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get Sel obj failed, ret = %d", __FUNCTION__, ret);
        return;
    }
    input_list = g_slist_append(input_list, g_variant_new_int32(sub_id));
    input_list = g_slist_append(input_list, g_variant_new_uint32(time.start));
    input_list = g_slist_append(input_list, g_variant_new_uint32(time.end));
    input_list = g_slist_append(input_list, g_variant_new_int32(type));
    ret = dfl_call_class_method(obj_handle, METHOD_DELETE_EVENT_LIST, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: delete event log failed", __FUNCTION__);
    }
}

LOCAL gint32 set_subscription_to_enable(OBJ_HANDLE subscription, json_object **o_message_jso, PROVIDER_PARAS_S *i_paras)
{
    gchar state[MAX_EVT_STATE_LEN] = { 0 };
    gint32 ret =
        dal_get_property_value_string(subscription, PROPERTY_EVT_SUBSCRIPTION_EVT_STATE, state, MAX_EVT_STATE_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get property %s failed, ret=%d", PROPERTY_EVT_SUBSCRIPTION_EVT_STATE, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if (strcmp(state, STATE_VALUE_DISABLED) == 0) {
        (void)create_message_info(MSGID__EVENT_SUBCRIPTION_DISABLED, NULL, o_message_jso, "ResumeSubscription");
        return HTTP_BAD_REQUEST;
    }
    if (strcmp(state, STATUS_VALUE_STANDBY_OFFLINE) == 0) {
        return change_state_value(i_paras, subscription, STATE_VALUE_EANBLED);
    }
    return HTTP_OK;
}

LOCAL gint32 evt_subscription_recover_provider_entry(PROVIDER_PARAS_S *i_paras,
    json_object **o_message_jso, json_object **o_result_jso)
{
    i_paras->is_satisfy_privi = i_paras->user_role_privilege &
        g_event_subscription_provider[i_paras->index].require_privilege;
    gint32 ret = rf_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: rf_check_para_and_priv failed, ret= %d.", __FUNCTION__, ret);
        return ret;
    }
    OBJ_HANDLE obj_handle = 0;
    gchar *end_pos = NULL;
    gint32 subscription_id = (gint32)strtol(i_paras->member_id, &end_pos, 10);
        
    if (subscription_id < RF_EVT_SUBSCRIPTION_ID_MIN || subscription_id > RF_EVT_SUBSCRIPTION_ID_MAX) {
        debug_log(DLOG_ERROR, "%s: subscription id(%d) is invalid, str=%s",
            __FUNCTION__, subscription_id, i_paras->member_id);
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri);
        return HTTP_NOT_FOUND;
    }
    ret = dal_get_specific_object_byte(CLASS_EVT_SUBSCRIPTION,
        PROPERTY_EVT_SUBSCRIPTION_ID, (guint8)subscription_id, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "cann't find event subscription with id, id=%d", subscription_id);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    gchar event_des[MAX_EVT_DESTINATION_LEN] = {0};
    
    (void)dal_get_property_value_string(obj_handle, PROPERTY_EVT_SUBSCRIPTION_DES, event_des,
        MAX_EVT_DESTINATION_LEN);
    if (event_des[0] == '\0') {
        debug_log(DLOG_ERROR, "the Destination of subdescription is null");
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri);
        return HTTP_NOT_FOUND;
    }
    ret = set_subscription_to_enable(obj_handle, o_message_jso, i_paras);
    if (ret != HTTP_OK) {
        debug_log(DLOG_ERROR, "the event subscription is disabled, id=%d", subscription_id);
        return ret;
    }
    return resume_suspended_event(o_message_jso, i_paras, obj_handle, subscription_id);
}


LOCAL gint32 set_evt_subscription_DeliveryRetryPolicy(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso)
{
    const gchar*        drp_str = NULL;
    i_paras->is_satisfy_privi = i_paras->user_role_privilege &
        g_event_subscription_provider[i_paras->index].require_privilege;
    gint32 ret = rf_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: rf_check_para_and_priv failed, ret= %d.", __FUNCTION__, ret);
        return ret;
    }

    drp_str = json_object_get_string(i_paras->val_jso);
    if (drp_str == NULL) {
        debug_log(DLOG_ERROR, "%s failed:get null string from input param.", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if (check_property_delivery_retry_policy(drp_str) != TRUE) {
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST,
            RFPROP_DELIVERY_RETRY_POLICY, o_message_jso, drp_str, RFPROP_DELIVERY_RETRY_POLICY);
        debug_log(DLOG_ERROR, "DeliveryRetryPolicy(%s) not valied", drp_str);
        return HTTP_BAD_REQUEST;
    }
    return change_drp_value(i_paras, i_paras->obj_handle, drp_str);
}

LOCAL gint32 set_evt_subscription_Status(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso)
{
    const gchar *state = NULL;
    json_object *state_json = NULL;
    
    i_paras->is_satisfy_privi = i_paras->user_role_privilege &
        g_event_subscription_provider[i_paras->index].require_privilege;
    gint32 ret = rf_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: rf_check_para_and_priv failed, ret= %d.", __FUNCTION__, ret);
        return ret;
    }
    if (!json_object_object_get_ex(i_paras->val_jso, RFPROP_EVENT_STATE, &state_json)) {
        debug_log(DLOG_ERROR, "the state of input is null");
        return HTTP_BAD_REQUEST;
    }
    state = json_object_get_string(state_json);
    if (check_property_status(state) != TRUE) {
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST,
            RFPROP_EVENT_STATE, o_message_jso, state, RFPROP_EVENT_STATE);
        debug_log(DLOG_ERROR, "State(%s) not valied", state);
        return HTTP_BAD_REQUEST;
    }
    return change_state_value(i_paras, i_paras->obj_handle, state);
}


LOCAL void get_shield_event_code(OBJ_HANDLE obj_handle, json_object* obj_json)
{
    gint32 ret;
    guint32 event_code = 0;
    gchar event_code_str[EVENT_CODE_STR_LEN] = {0};

    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_EVENT_INFORMATION_EVENT_CODE, &event_code);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_byte failed, ret = %d", __FUNCTION__, ret);
        json_object_object_add(obj_json, RFPROP_SHIELD_EVENT_CODE, NULL);
        return;
    }

    ret = snprintf_s(event_code_str, sizeof(event_code_str), sizeof(event_code_str) - 1, "0x%08X", event_code);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s failed", __FUNCTION__);
        json_object_object_add(obj_json, RFPROP_SHIELD_EVENT_CODE, NULL);
        return;
    }

    json_object_object_add(obj_json, RFPROP_SHIELD_EVENT_CODE, json_object_new_string(event_code_str));
}


LOCAL void get_evt_shield_system_alert(json_object* hw_jso)
{
    gint32 ret;
    guchar is_enable = 0;
    GSList* obj_list = NULL;
    GSList* obj_node = NULL;
    OBJ_HANDLE obj_handle = 0;

    json_object* obj_json = NULL;
    json_object* shield_alert_jso = NULL;

    shield_alert_jso = json_object_new_array();
    if (shield_alert_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_array failed", __FUNCTION__);
        json_object_object_add(hw_jso, RFPROP_SHIELD_SYSTEM_ALERT, NULL);
        return;
    }

    json_object_object_add(hw_jso, RFPROP_SHIELD_SYSTEM_ALERT, shield_alert_jso);

    ret = dfl_get_object_list(CLASS_EVENT_INFORMATION, &obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get obj_list failed(ret=%d)", __FUNCTION__, ret);
        return;
    }

    for (obj_node = obj_list; obj_node != NULL; obj_node = obj_node->next) {
        obj_handle = (OBJ_HANDLE)(intptr_t)(obj_node->data);
        ret = dal_get_property_value_byte(obj_handle, PROPERTY_EVENT_INFORMATION_ENABLE, &is_enable);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s: dal_get_property_value_byte failed(ret=%d)", __FUNCTION__, ret);
            continue;
        }

        
        if (is_enable == ENABLE) {
            continue; 
        }

        obj_json = json_object_new_object();
        json_object_array_add(shield_alert_jso, obj_json);

        
        get_shield_event_code(obj_handle, obj_json);

        
        rf_add_property_jso_string(obj_handle, PROPERTY_EVENT_INFORMATION_EVENT_NAME,
            RFPROP_SHIELD_EVENT_NAME, obj_json);
    }
    g_slist_free(obj_list);
}


gint32 get_evt_svc_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    guchar server_identity_source = 0;
    guchar post_enabled = 0;
    gint32 delivery_retry_attempts = 0;
    gint32 delivery_retry_interval_secs = 0;
    gint32 ret;
    OBJ_HANDLE obj_handle;
    gchar        server_identity_source_str[MAX_SERVER_IDENTITY_LEN] = {0};
    json_object *oem_jso = NULL;
    json_object *hw_jso = NULL;

    if (i_paras == NULL || o_rsc_jso == NULL || o_err_array_jso == NULL) {
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__);
        return RF_FAILED;
    }

    ret = dal_get_object_handle_nth(CLASS_RF_EVT_SVC, 0, &obj_handle);
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "get object handle for class %s failed", CLASS_RF_EVT_SVC);

        return RF_FAILED;
    }

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_RF_EVT_SVC_SERVER_IDENTITY_SRC, &server_identity_source);
    ret = server_identity_source_trans(&server_identity_source, server_identity_source_str, MAX_SERVER_IDENTITY_LEN,
        IDENTITY_INDEX_TO_STR);
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "cann't get server identity source str with index %d", server_identity_source);

        return RF_FAILED;
    }

    oem_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != oem_jso, RF_FAILED, debug_log(DLOG_ERROR, "alloc new json object failed"));

    hw_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != hw_jso, RF_FAILED, debug_log(DLOG_ERROR, "alloc new json object failed");
        (void)json_object_put(oem_jso));

    
    json_object_object_add(o_rsc_jso, RFPROP_COMMON_OEM, oem_jso);
    json_object_object_add(oem_jso, RFPROP_COMMON_HUAWEI, hw_jso);
    json_object_object_add(hw_jso, RFPROP_SERVER_IDENTITY_SOURCE,
        json_object_new_string((const gchar *)server_identity_source_str));
    
    
    get_evt_svc_shield_resources(hw_jso);

    
    get_evt_shield_system_alert(hw_jso);

    
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_RF_EVT_SVC_SERVICE_ENABLED, &post_enabled);
    json_object_object_add(o_rsc_jso, RFPROP_SERVICE_ENABLED, json_object_new_boolean(post_enabled));

    
    (void)dal_get_property_value_int32(obj_handle, PROPERTY_RF_EVT_SVC_EVENT_RETRY_TIMES, &delivery_retry_attempts);
    json_object_object_add(o_rsc_jso, RFPROP_DELIVERY_RETRY_ATTEMPTS, json_object_new_int(delivery_retry_attempts));

    
    (void)dal_get_property_value_int32(obj_handle, PROPERTY_RF_EVT_SVC_EVENT_DELAY_INSEC,
        &delivery_retry_interval_secs);
    json_object_object_add(o_rsc_jso, RFPROP_DELIVERY_RETRY_INTERVAL,
        json_object_new_int(delivery_retry_interval_secs));

    return RF_OK;
}

LOCAL PROPERTY_PROVIDER_S g_evt_subscription_col_provider[] = {
     }
;


gint32 evt_subscription_col_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    
    return_val_if_fail(NULL != i_paras && NULL != prop_provider && NULL != count, HTTP_INTERNAL_SERVER_ERROR);

    *prop_provider = g_evt_subscription_col_provider;
    
    *count = 0;
    

    return VOS_OK;
}


gint32 get_evt_subscription_collection_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso,
    json_object *o_err_array_jso, gchar **o_rsp_body_str)
{
    int iRet = -1;
    GSList *obj_list = NULL;
    GSList *obj_iter = NULL;
    OBJ_HANDLE obj_handle = 0;
    gint32 ret = 0;
    guchar member_id = 0;
    gchar             member_uri[MAX_URI_LENGTH] = {0};
    gchar             destination_buf[MAX_EVT_DESTINATION_LEN] = {0};
    json_object *members_arr_jso = NULL;
    json_object *member_jso = NULL;
    gint32 member_cnt = 0;

    if (i_paras == NULL || o_rsc_jso == NULL || o_err_array_jso == NULL) {
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__);
        return RF_FAILED;
    }

    ret = dfl_get_object_list(CLASS_EVT_SUBSCRIPTION, &obj_list);
    return_val_do_info_if_fail(VOS_OK == ret, RF_FAILED,
        debug_log(DLOG_ERROR, "get object list for class %s failed", CLASS_EVT_SUBSCRIPTION));

    members_arr_jso = json_object_new_array();
    return_val_do_info_if_fail(NULL != members_arr_jso, RF_FAILED, debug_log(DLOG_ERROR, "alloc new json array failed");
        g_slist_free(obj_list));

    json_object_object_add(o_rsc_jso, RFPROP_MEMBERS, members_arr_jso);

    for (obj_iter = obj_list; NULL != obj_iter; obj_iter = obj_iter->next) {
        obj_handle = (OBJ_HANDLE)obj_iter->data;

        (void)memset_s(destination_buf, MAX_EVT_DESTINATION_LEN, 0, MAX_EVT_DESTINATION_LEN);
        (void)dal_get_property_value_string(obj_handle, PROPERTY_EVT_SUBSCRIPTION_DES, destination_buf,
            MAX_EVT_DESTINATION_LEN);

        
        check_success_continue('\0' == destination_buf[0]);

        (void)dal_get_property_value_byte(obj_handle, PROPERTY_EVT_SUBSCRIPTION_ID, &member_id);

        (void)memset_s(member_uri, MAX_URI_LENGTH, 0, MAX_URI_LENGTH);
        
        iRet = snprintf_s(member_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, "%s/%u", EVT_SUBSCRIPTION_URI_PATTERN,
            member_id);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
        

        member_jso = json_object_new_object();
        continue_do_info_if_fail((NULL != member_jso), debug_log(DLOG_ERROR, "alloc new json object failed"));

        json_object_object_add(member_jso, ODATA_ID, json_object_new_string((const gchar *)member_uri));

        ret = json_object_array_add(members_arr_jso, member_jso);
        if (ret != 0) {
            (void)json_object_put(member_jso);
            debug_log(DLOG_ERROR, "add json object to json array failed, ret is %d", ret);

            continue;
        }
    }

    g_slist_free(obj_list);

    member_cnt = json_object_array_length(members_arr_jso);
    json_object_object_add(o_rsc_jso, RFPROP_MEMBERS_COUNT, json_object_new_int(member_cnt));

    return RF_OK;
}

LOCAL void get_evt_property_from_obj(OBJ_HANDLE obj_handle, json_object *o_rsc_jso)
{
    gint32 ret;
    gchar *properts[] = {PROPERTY_EVT_SUBSCRIPTION_DES, PROPERTY_EVT_SUBSCRIPTION_EVT_CTX,
        PROPERTY_EVT_SUBSCRIPTION_EVT_PROTOCOL, PROPERTY_EVT_SUBSCRIPTION_EVT_TYPES, PROPERTY_EVT_SUBSCRIPTION_EVT_DRP,
        PROPERTY_EVT_SUBSCRIPTION_MSG_ID};
    gchar *rfprop[] = {RFPROP_EVT_DESTINATION, RFPROP_CONTEXT, RFPROP_PROTOCOL, RFPROP_EVT_TYPES,
        RFPROP_DELIVERY_RETRY_POLICY, RFPROP_MSG_IDS};
    for (gint32 i = 0; i < G_N_ELEMENTS(properts); i++) {
        ret = rf_obj_handle_data_to_rsc_data(obj_handle, properts[i], rfprop[i], o_rsc_jso);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "rf_obj_handle_data_to_rsc_data faile, rp_property=%s, ret=%d", rfprop[i], ret);
        }
    }
    json_object *status = json_object_new_object();
    if (status == NULL) {
        debug_log(DLOG_ERROR, "Failed to create the fan json object - malloc failed.");
        return;
    }
    ret = rf_obj_handle_data_to_rsc_data(obj_handle, PROPERTY_EVT_SUBSCRIPTION_EVT_STATE,
        RFPROP_EVENT_STATE, status);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "rf_obj_handle_data_to_rsc_data faile, rp_property=%s", RFPROP_EVENT_STATE);
        json_object_put(status);
        return;
    }
    (void)json_object_object_add(o_rsc_jso, RFPROP_EVENT_STATUS, status);
}


gint32 get_evt_subscription_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    int iRet = -1;
    OBJ_HANDLE obj_handle = 0;
    gchar               name_buf[MAX_STRBUF_LEN] = {0};
    guchar event_subscription_id = 0;
    gint32 ret = 0;
    GVariant *origin_rsc_array = NULL;
    const gchar **uri_array;
    guint32 i = 0;
    json_object *rsc_uri_array_jso = NULL;
    json_object *item_jso = NULL;

    if (i_paras == NULL || o_rsc_jso == NULL || o_err_array_jso == NULL) {
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__);
        return RF_FAILED;
    }

    
    return_val_if_expr(vos_str2int(i_paras->member_id, 10, &event_subscription_id, NUM_TPYE_UCHAR), RF_FAILED);
    (void)dal_get_specific_object_byte(CLASS_EVT_SUBSCRIPTION, PROPERTY_EVT_SUBSCRIPTION_ID, event_subscription_id,
        &obj_handle);
    
    json_object_object_add(o_rsc_jso, RFPROP_ID, json_object_new_string(i_paras->member_id));

    
    iRet = snprintf_s(name_buf, MAX_STRBUF_LEN, MAX_STRBUF_LEN - 1, "EventSubscription %s", i_paras->member_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    json_object_object_add(o_rsc_jso, RFPROP_NAME, json_object_new_string((const gchar *)name_buf));

    get_evt_property_from_obj(obj_handle, o_rsc_jso);
    
    ret = get_evt_subscription_action(i_paras, o_rsc_jso);
    if (ret != HTTP_OK) {
        debug_log(DLOG_ERROR, "refresh actions fail, status=%d", ret);
    }
    rsc_uri_array_jso = json_object_new_array();
    return_val_do_info_if_fail(NULL != rsc_uri_array_jso, RF_FAILED,
        debug_log(DLOG_ERROR, "alloc new json array failed"));

    
    (void)dfl_get_property_value(obj_handle, PROPERTY_EVT_SUBSCRIPTION_ORIGIN_RSC, &origin_rsc_array);
    uri_array = g_variant_get_strv(origin_rsc_array, NULL);
    
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#endif
    if ((1 != g_strv_length((gchar **)uri_array)) || (0 != g_strcmp0(RF_STR_EMPTY, uri_array[0]))) {
        for (i = 0; NULL != uri_array[i]; i++) {
            item_jso = json_object_new_object();
            continue_do_info_if_fail(NULL != item_jso, debug_log(DLOG_ERROR, "alloc new json object failed"));

            json_object_object_add(item_jso, ODATA_ID, json_object_new_string(uri_array[i]));
            ret = json_object_array_add(rsc_uri_array_jso, item_jso);

            continue_do_info_if_fail(0 == ret, (void)json_object_put(item_jso));
        }
    }
// require GCC 4.6
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif
    

    g_free(uri_array);

    g_variant_unref(origin_rsc_array);

    json_object_object_add(o_rsc_jso, PROPERTY_EVT_SUBSCRIPTION_ORIGIN_RSC, rsc_uri_array_jso);

    

    return RF_OK;
}