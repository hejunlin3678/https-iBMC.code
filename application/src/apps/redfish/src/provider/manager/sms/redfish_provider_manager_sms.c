
#include "redfish_provider.h"

typedef struct ibma_up_update_info {
    const gchar *image_uri;
    const gchar *signature_uri;
    const gchar *crl_uri;
} IBMA_UP_ARGS;

LOCAL gint32 get_bob_service(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);

LOCAL gint32 set_bob_service(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_sms_service_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_cdev_channel_state(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_cdev_channel_state(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 act_manager_refresh_bma(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_manager_smsservice_actions(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_sms_actioninfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_channel_type(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_channel_type(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_manager_sms_provider[] = {
    {
        RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        get_sms_service_odata_id, NULL, NULL, ETAG_FLAG_ENABLE
    },
    {
        SMS_BOB_SERVICE_STR, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_FORBID,
        get_bob_service, set_bob_service, NULL, ETAG_FLAG_ENABLE
    },
    {
        SMS_CDEV_CHANNEL_ENABLED, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_FORBID,
        get_cdev_channel_state, set_cdev_channel_state, NULL, ETAG_FLAG_ENABLE
    },
    {
        SMS_CHANNEL_TYPE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_FORBID,
        get_channel_type, set_channel_type, NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_MANAGER_ACTION, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        get_manager_smsservice_actions, NULL, NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_SMSSERVICE_REFRESH_BMA, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        NULL, NULL, act_manager_refresh_bma, ETAG_FLAG_ENABLE
    }
};

LOCAL PROPERTY_PROVIDER_S g_manager_sms_action_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_sms_actioninfo_odata_id, NULL, NULL, ETAG_FLAG_ENABLE}
};


LOCAL gint32 get_sms_service_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return get_odata_id(i_paras, o_message_jso, o_result_jso, "/redfish/v1/Managers/%s/SmsService");
}


LOCAL gint32 get_sms_alarm_desc(OBJ_HANDLE obj_handle, json_object **json_obj)
{
    gint32 ret;
    gchar alarm_desc[SMS_STRLEN_256] = {0};

    ret = dal_get_property_value_string(obj_handle, PROPERTY_SMS_ALARMDESC, alarm_desc, sizeof(alarm_desc));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get prop fail, %s, %s\n", __FUNCTION__, __LINE__, CLASS_SMS,
        PROPERTY_SMS_ALARMDESC));

    *json_obj = json_object_new_string(alarm_desc);
    return_val_do_info_if_fail(NULL != *json_obj, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_string fail\n", __FUNCTION__, __LINE__));

    return VOS_OK;
}


LOCAL gint32 get_sms_alarm_state(OBJ_HANDLE obj_handle, json_object **json_obj)
{
    gint32 ret;
    guint8 alarm_state = 0;

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SMS_ALARMSTATE, &alarm_state);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get prop fail, %s, %s\n", __FUNCTION__, __LINE__, CLASS_SMS,
        PROPERTY_SMS_ALARMSTATE));

    *json_obj = json_object_new_boolean((int32_t)alarm_state);
    return_val_do_info_if_fail(NULL != *json_obj, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_string fail\n", __FUNCTION__, __LINE__));

    return VOS_OK;
}


LOCAL gint32 get_min_veth_port(OBJ_HANDLE obj_handle, json_object **json_obj)
{
    gint32 ret;
    guint32 port = 0;

    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_SMS_MINPORT, &port);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get prop fail, %s, %s\n", __FUNCTION__, __LINE__, CLASS_SMS,
        PROPERTY_SMS_MAXPORT));

    *json_obj = json_object_new_int((int32_t)port);
    return_val_do_info_if_fail(NULL != *json_obj, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_string fail\n", __FUNCTION__, __LINE__));

    return VOS_OK;
}


LOCAL gint32 get_max_veth_port(OBJ_HANDLE obj_handle, json_object **json_obj)
{
    gint32 ret;
    guint32 port = 0;

    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_SMS_MAXPORT, &port);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get prop fail, %s, %s\n", __FUNCTION__, __LINE__, CLASS_SMS,
        PROPERTY_SMS_MAXPORT));

    *json_obj = json_object_new_int((int32_t)port);
    return_val_do_info_if_fail(NULL != *json_obj, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_string fail\n", __FUNCTION__, __LINE__));

    return VOS_OK;
}


LOCAL gint32 get_mapping_table(json_object **json_obj)
{
    gint32 ret;
    json_object *jso = NULL;
    json_object *tmp_jso = NULL;
    gchar ipaddress[HOST_ADDR_MAX_LEN + 1] = {0};
    guint32 port = 0;
    guint32 vethport = 0;
    guint8 able_value = 0;
    guint8 bobable_value = 0;
    guint8 item_id = 0;

    GSList *obj_list = NULL;
    GSList *obj_node = NULL;

    ret = dfl_get_object_list(CLASS_TRAP_ITEM_CONFIG, &obj_list);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: dfl_get_object_list fail\n", __FUNCTION__, __LINE__));

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        jso = json_object_new_object();
        return_val_do_info_if_fail(NULL != jso, HTTP_INTERNAL_SERVER_ERROR, g_slist_free(obj_list);
            debug_log(DLOG_ERROR, "%s, %d: json_object_new_object fail\n", __FUNCTION__, __LINE__));

        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_TRAP_ITEM_INDEX_NUM, &item_id);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR, g_slist_free(obj_list);
            (void)json_object_put(jso); debug_log(DLOG_ERROR, "%s, %d:  get prop fail, %s, %s\n", __FUNCTION__,
            __LINE__, CLASS_TRAP_ITEM_CONFIG, PROPERTY_TRAP_ITEM_DEST_IPADDR));

        tmp_jso = json_object_new_int((int32_t)item_id);
        return_val_do_info_if_fail(NULL != tmp_jso, HTTP_INTERNAL_SERVER_ERROR, g_slist_free(obj_list);
            (void)json_object_put(jso);
            debug_log(DLOG_ERROR, "%s, %d: json_object_new_int fail\n", __FUNCTION__, __LINE__));
        json_object_object_add(jso, SMS_BOB_ID, tmp_jso);
        tmp_jso = NULL;

        // 接收者地址
        (void)memset_s(ipaddress, sizeof(ipaddress), 0x00, sizeof(ipaddress));

        ret = dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_TRAP_ITEM_DEST_IPADDR, ipaddress,
            sizeof(ipaddress));
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR, g_slist_free(obj_list);
            (void)json_object_put(jso); debug_log(DLOG_ERROR, "%s, %d:  get prop fail, %s, %s\n", __FUNCTION__,
            __LINE__, CLASS_TRAP_ITEM_CONFIG, PROPERTY_TRAP_ITEM_DEST_IPADDR));

        tmp_jso = json_object_new_string(ipaddress);
        return_val_do_info_if_fail(NULL != tmp_jso, HTTP_INTERNAL_SERVER_ERROR, g_slist_free(obj_list);
            (void)json_object_put(jso);
            debug_log(DLOG_ERROR, "%s, %d: json_object_new_string fail\n", __FUNCTION__, __LINE__));
        json_object_object_add(jso, SMS_SERVER_ADDRESS, tmp_jso);
        tmp_jso = NULL;

        // 接收者端口号
        ret = dal_get_property_value_uint32((OBJ_HANDLE)obj_node->data, PROPERTY_TRAP_ITEM_DEST_IPPORT, &port);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR, g_slist_free(obj_list);
            (void)json_object_put(jso); debug_log(DLOG_ERROR, "%s, %d:  get prop fail, %s, %s\n", __FUNCTION__,
            __LINE__, CLASS_TRAP_ITEM_CONFIG, PROPERTY_TRAP_ITEM_DEST_IPADDR));

        tmp_jso = json_object_new_int((int32_t)port);
        return_val_do_info_if_fail(NULL != tmp_jso, HTTP_INTERNAL_SERVER_ERROR, g_slist_free(obj_list);
            (void)json_object_put(jso);
            debug_log(DLOG_ERROR, "%s, %d: json_object_new_int fail\n", __FUNCTION__, __LINE__));
        json_object_object_add(jso, SMS_SERVER_PORT, tmp_jso);
        tmp_jso = NULL;

        // 目的端口号BmaVethIpPort
        ret = dal_get_property_value_uint32((OBJ_HANDLE)obj_node->data, PROPERTY_TRAP_BOB_BMA_VETH_IP_PORT, &vethport);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR, g_slist_free(obj_list);
            (void)json_object_put(jso); debug_log(DLOG_ERROR, "%s, %d:  get prop fail, %s, %s\n", __FUNCTION__,
            __LINE__, CLASS_TRAP_ITEM_CONFIG, PROPERTY_TRAP_BOB_BMA_VETH_IP_PORT));

        tmp_jso = json_object_new_int((int32_t)vethport);
        return_val_do_info_if_fail(NULL != tmp_jso, HTTP_INTERNAL_SERVER_ERROR, g_slist_free(obj_list);
            (void)json_object_put(jso);
            debug_log(DLOG_ERROR, "%s, %d: json_object_new_int fail\n", __FUNCTION__, __LINE__));
        json_object_object_add(jso, SMS_VETH_PORT, tmp_jso);
        tmp_jso = NULL;

        // 是否启用该接收者
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_TRAP_ITEM_ENABLE, &able_value);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR, g_slist_free(obj_list);
            (void)json_object_put(jso); debug_log(DLOG_ERROR, "%s, %d:  get prop fail, %s, %s\n", __FUNCTION__,
            __LINE__, CLASS_TRAP_ITEM_CONFIG, PROPERTY_TRAP_ITEM_DEST_IPADDR));

        if (SNMP_TRAP_ITEM_ENABLE != able_value && SNMP_TRAP_ITEM_DISABLE != able_value) {
            
            debug_log(DLOG_DEBUG, "%s, %d:  prop invalid, %s, %s, %d\n", __FUNCTION__, __LINE__, CLASS_TRAP_ITEM_CONFIG,
                PROPERTY_TRAP_ITEM_ENABLE, able_value);
            
            g_slist_free(obj_list);
            (void)json_object_put(jso);
            return HTTP_INTERNAL_SERVER_ERROR;
        }

        tmp_jso = json_object_new_boolean(able_value);
        return_val_do_info_if_fail(NULL != tmp_jso, HTTP_INTERNAL_SERVER_ERROR, g_slist_free(obj_list);
            (void)json_object_put(jso);
            debug_log(DLOG_ERROR, "%s, %d: json_object_new_boolean fail\n", __FUNCTION__, __LINE__));
        json_object_object_add(jso, SMS_TRAP_ENABLE, tmp_jso);
        tmp_jso = NULL;

        // 是否启用带内通道
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_TRAP_BOB_ENABLE, &bobable_value);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR, g_slist_free(obj_list);
            (void)json_object_put(jso); debug_log(DLOG_ERROR, "%s, %d:  get prop fail, %s, %s\n", __FUNCTION__,
            __LINE__, CLASS_TRAP_ITEM_CONFIG, PROPERTY_TRAP_BOB_ENABLE));

        if (BOB_ENABLE != bobable_value && BOB_DISABLE != bobable_value) {
            
            debug_log(DLOG_DEBUG, "%s, %d:  prop invalid, %s, %s, %d\n", __FUNCTION__, __LINE__, CLASS_TRAP_ITEM_CONFIG,
                PROPERTY_TRAP_BOB_ENABLE, bobable_value);
            
            g_slist_free(obj_list);
            (void)json_object_put(jso);
            return HTTP_INTERNAL_SERVER_ERROR;
        }

        tmp_jso = json_object_new_boolean(bobable_value);
        return_val_do_info_if_fail(NULL != tmp_jso, HTTP_INTERNAL_SERVER_ERROR, g_slist_free(obj_list);
            (void)json_object_put(jso);
            debug_log(DLOG_ERROR, "%s, %d: json_object_new_boolean fail\n", __FUNCTION__, __LINE__));
        json_object_object_add(jso, SMS_BOB_ENABLE, tmp_jso);
        tmp_jso = NULL;

        ret = json_object_array_add(*json_obj, jso);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR, g_slist_free(obj_list);
            (void)json_object_put(jso);
            debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail\n", __FUNCTION__, __LINE__));
        jso = NULL;
    }

    do_val_if_expr(NULL != obj_list, g_slist_free(obj_list));

    return VOS_OK;
}


LOCAL gint32 get_bob_service(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    json_object *jso = NULL;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_fail(*o_result_jso != NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  json_object_new_object fail", __FUNCTION__, __LINE__));

    // bob相关信息
    ret = dal_get_object_handle_nth(CLASS_SMS, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(*o_result_jso);
        *o_result_jso = NULL);

    // 获取映射表
    jso = json_object_new_array();
    return_val_do_info_if_fail(jso != NULL, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(*o_result_jso);
        *o_result_jso = NULL; debug_log(DLOG_ERROR, "%s, %d:  json_object_new_array fail", __FUNCTION__, __LINE__));
    get_mapping_table(&jso);
    json_object_object_add(*o_result_jso, SMS_MAPPING_TABLE_STR, jso);
    jso = NULL;

    // 获取端口上限
    get_max_veth_port(obj_handle, &jso);
    json_object_object_add(*o_result_jso, SMS_MAX_PORT, jso);
    jso = NULL;

    // 获取端口下限
    get_min_veth_port(obj_handle, &jso);
    json_object_object_add(*o_result_jso, SMS_MIN_PORT, jso);
    jso = NULL;

    // 获取告警状态
    get_sms_alarm_state(obj_handle, &jso);
    json_object_object_add(*o_result_jso, SMS_ALARM_STATE, jso);
    jso = NULL;

    // 获取告警描述
    get_sms_alarm_desc(obj_handle, &jso);
    json_object_object_add(*o_result_jso, SMS_ALARM_DESC, jso);

    return HTTP_OK;
}


LOCAL gint32 set_sms_alarm_desc(PROVIDER_PARAS_S *i_paras, json_object *item_jso, json_object **o_message_jso)
{
    gint32 ret;
    json_object *obj_jso = NULL;
    const char *str = NULL;
    gchar prop_str[SMS_STRLEN_128] = {0};
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;

    if (snprintf_s(prop_str, sizeof(prop_str), sizeof(prop_str) - 1, "%s/%s/", SMS_BOB_SERVICE_STR, SMS_MAX_PORT) <=
        0) {
        debug_log(DLOG_ERROR, "%s, %d, snprintf_s fail", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (NULL == item_jso) {
        
        debug_log(DLOG_DEBUG, "%s, %d, incorrect value type,  %s", __FUNCTION__, __LINE__, prop_str);
        
        (void)create_message_info(MSGID_PROP_ITEM_TYPE_ERR, prop_str, &obj_jso, RF_VALUE_NULL, prop_str);
        ret = json_object_array_add(*o_message_jso, obj_jso);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail\n", __FUNCTION__, __LINE__));

        return HTTP_BAD_REQUEST;
    }

    str = dal_json_object_get_str(item_jso);
    return_val_do_info_if_fail(NULL != str, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  dal_json_object_get_str fail.", __FUNCTION__, __LINE__));

    input_list = g_slist_append(input_list, g_variant_new_string(str));
    return_val_do_info_if_fail(NULL != input_list, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  json_object_get_int fail.", __FUNCTION__, __LINE__));

    ret = dal_get_object_handle_nth(CLASS_SMS, 0, &obj_handle);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR, uip_free_gvariant_list(input_list));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SMS, METHOD_SMS_SET_ALARM_DESC, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s, %d, set alarm description failed!", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &obj_jso);
        ret = json_object_array_add(*o_message_jso, obj_jso);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail\n", __FUNCTION__, __LINE__));

        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return VOS_OK;
}


LOCAL gint32 set_sms_alarm_state(PROVIDER_PARAS_S *i_paras, json_object *item_jso, json_object **o_message_jso)
{
    gint32 ret;
    json_object *obj_jso = NULL;
    guint8 alarm_state;
    gchar prop_str[SMS_STRLEN_128] = {0};
    OBJ_HANDLE obj_handle = 0;
    gchar val_str[SMS_STRLEN_128] = {0};
    GSList *input_list = NULL;

    if (snprintf_s(prop_str, sizeof(prop_str), sizeof(prop_str) - 1, "%s/%s/", SMS_BOB_SERVICE_STR, SMS_ALARM_STATE) <=
        0) {
        debug_log(DLOG_ERROR, "%s, %d, snprintf_s failed", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (NULL == item_jso) {
        debug_log(DLOG_ERROR, "%s, %d, wrong value type,  %s", __FUNCTION__, __LINE__, prop_str);
        (void)create_message_info(MSGID_PROP_ITEM_TYPE_ERR, prop_str, &obj_jso, RF_VALUE_NULL, prop_str);
        ret = json_object_array_add(*o_message_jso, obj_jso);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_array_add failed\n", __FUNCTION__, __LINE__));

        return HTTP_BAD_REQUEST;
    }

    if (TRUE == json_object_is_type(item_jso, json_type_double)) {
        if (snprintf_s(val_str, sizeof(val_str), sizeof(val_str) - 1, "%lf", json_object_get_double(item_jso)) <= 0) {
            debug_log(DLOG_ERROR, "%s, %d, snprintf_s failed.", __FUNCTION__, __LINE__);
            return HTTP_INTERNAL_SERVER_ERROR;
        }

        (void)create_message_info(MSGID_PROP_ITEM_TYPE_ERR, prop_str, &obj_jso, val_str, prop_str);
        ret = json_object_array_add(*o_message_jso, obj_jso);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_array_add failed\n", __FUNCTION__, __LINE__));

        return HTTP_BAD_REQUEST;
    }

    alarm_state = json_object_get_int(item_jso);
    if ((alarm_state != SMS_ALARM_ON) && (alarm_state != SMS_ALARM_OFF)) {
        debug_log(DLOG_ERROR, "%s, %d, wrong value type,  %s", __FUNCTION__, __LINE__, prop_str);
        (void)create_message_info(MSGID_PROP_ITEM_TYPE_ERR, prop_str, &obj_jso, RF_VALUE_NULL, prop_str);
        ret = json_object_array_add(*o_message_jso, obj_jso);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail\n", __FUNCTION__, __LINE__));

        return HTTP_BAD_REQUEST;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(alarm_state));
    return_val_do_info_if_fail(NULL != input_list, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  json_object_get_int fail.", __FUNCTION__, __LINE__));

    ret = dal_get_object_handle_nth(CLASS_SMS, 0, &obj_handle);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR, uip_free_gvariant_list(input_list));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SMS, METHOD_SMS_SET_ALARM_STATE, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s, %d, Set alarm state failed, ret = %d!", __FUNCTION__, __LINE__, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &obj_jso);
        ret = json_object_array_add(*o_message_jso, obj_jso);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail\n", __FUNCTION__, __LINE__));

        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return VOS_OK;
}


LOCAL gint32 get_bma_veth_port(PROVIDER_PARAS_S *i_paras, json_object *item_jso, json_object **o_message_jso,
    guint32 *port)
{
    gint32 ret;
    gchar val_str[SMS_STRLEN_128] = {0};
    json_object *obj_jso = NULL;
    gint32 ip_port;
    gchar prop_str[SMS_STRLEN_128] = {0};

    if (snprintf_s(prop_str, sizeof(prop_str), sizeof(prop_str) - 1, "%s/%s/", SMS_BOB_SERVICE_STR, SMS_MAX_PORT) <=
        0) {
        debug_log(DLOG_ERROR, "%s, %d, snprintf_s fail", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (NULL == item_jso) {
        debug_log(DLOG_ERROR, "%s, %d, wrong value type,  %s", __FUNCTION__, __LINE__, prop_str);
        (void)create_message_info(MSGID_PROP_ITEM_TYPE_ERR, prop_str, &obj_jso, RF_VALUE_NULL, prop_str);
        ret = json_object_array_add(*o_message_jso, obj_jso);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail\n", __FUNCTION__, __LINE__));
        return HTTP_BAD_REQUEST;
    }

    if (TRUE == json_object_is_type(item_jso, json_type_double)) {
        if (snprintf_s(val_str, sizeof(val_str), sizeof(val_str) - 1, "%lf", json_object_get_double(item_jso)) <= 0) {
            debug_log(DLOG_ERROR, "%s, %d, snprintf_s fail", __FUNCTION__, __LINE__);
            return HTTP_INTERNAL_SERVER_ERROR;
        }

        (void)create_message_info(MSGID_PROP_ITEM_TYPE_ERR, prop_str, &obj_jso, val_str, prop_str);
        ret = json_object_array_add(*o_message_jso, obj_jso);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail\n", __FUNCTION__, __LINE__));
        return HTTP_BAD_REQUEST;
    }

    ip_port = json_object_get_int(item_jso);
    *port = ip_port;

    return VOS_OK;
}


LOCAL gint32 set_veth_port(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, const gchar *method_name,
    OBJ_HANDLE obj_handle, guint32 port)
{
    gint32 ret;
    json_object *obj_jso = NULL;
    GSList *input_list = NULL;

    input_list = g_slist_append(input_list, g_variant_new_uint32(port));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SMS, method_name, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);

    uip_free_gvariant_list(input_list);

    // 正确响应则返回
    return_val_if_expr(VOS_OK == ret, VOS_OK);

    // 异常响应
    debug_log(DLOG_ERROR, "%s, %d, set veth port failed, port = %d, ret = %d", __FUNCTION__, __LINE__, port, ret);
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &obj_jso);
    ret = json_object_array_add(*o_message_jso, obj_jso);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail, ret = %d.\n", __FUNCTION__, __LINE__, ret));

    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL gint32 set_veth_max_min_port(PROVIDER_PARAS_S *i_paras, json_object *item_jso_max, json_object *item_jso_min,
    json_object **o_message_jso)
{
    gint32 ret;
    guint32 ip_port_max = 0;
    guint32 ip_port_min = 0;
    OBJ_HANDLE obj_handle = 0;
    gchar prop_str[SMS_STRLEN_128] = {0};
    json_object *obj_jso = NULL;

    if (item_jso_max == NULL || item_jso_min == NULL) {
        debug_log(DLOG_ERROR, "%s, %d: NULL Pointer. ", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &obj_jso);
        json_object_array_add(*o_message_jso, obj_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (snprintf_s(prop_str, sizeof(prop_str), sizeof(prop_str) - 1, "%s/%s/ or %s/%s/", SMS_BOB_SERVICE_STR,
        SMS_MAX_PORT, SMS_BOB_SERVICE_STR, SMS_MIN_PORT) <= 0) {
        debug_log(DLOG_ERROR, "%s, %d, snprintf_s fail", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    // 获取最大端口
    ret = get_bma_veth_port(i_paras, item_jso_max, o_message_jso, &ip_port_max);

    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d: get BMA veth max port failed, ret = %d.", __FUNCTION__, __LINE__, ret));

    // 校验最大端口范围
    if ((SMTP_TRAP_MIN_PORT > ip_port_max) || (SMTP_TRAP_MAX_PORT < ip_port_max)) {
        debug_log(DLOG_ERROR, "%s max port is invalid, max_port is %d\n", __FUNCTION__, ip_port_max);
        goto ERR;
    }

    // 获取最小端口
    ret = get_bma_veth_port(i_paras, item_jso_min, o_message_jso, &ip_port_min);

    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d: get BMA veth min port failed, ret = %d.", __FUNCTION__, __LINE__, ret));

    // 校验最小端口范围
    if ((SMTP_TRAP_MIN_PORT > ip_port_min) || (SMTP_TRAP_MAX_PORT < ip_port_min)) {
        debug_log(DLOG_ERROR, "%s min port is invalid, min_port is %d\n", __FUNCTION__, ip_port_min);
        goto ERR;
    }

    // 校验大小关系
    if (ip_port_max < ip_port_min) {
        debug_log(DLOG_ERROR, "%s max port (%d) is not bigger than min port(%d).\n", __FUNCTION__, ip_port_max,
            ip_port_min);
        goto ERR;
    }

    // 判断是否至少有16个端口
    if (ip_port_max - ip_port_min + 1 < SNMP_BMA_VETH_PORT_NUMBER) {
        
        debug_log(DLOG_ERROR, "%s port number is %d less than %d.\n", __FUNCTION__, ip_port_max - ip_port_min + 1,
            SNMP_BMA_VETH_PORT_NUMBER);
        
        goto ERR;
    }

    ret = dal_get_object_handle_nth(CLASS_SMS, 0, &obj_handle);
    return_val_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR);

    ret = set_veth_port(i_paras, o_message_jso, METHOD_SMS_SET_MAX_PORT, obj_handle, ip_port_max);

    return_val_do_info_if_fail(VOS_OK == ret, ret,
        debug_log(DLOG_ERROR, "%s, %d: set max port for %s fail, port = %d, ret = %d", __FUNCTION__, __LINE__,
        CLASS_SMS, ip_port_max, ret));

    ret = set_veth_port(i_paras, o_message_jso, METHOD_SMS_SET_MIN_PORT, obj_handle, ip_port_min);
    return_val_do_info_if_fail(VOS_OK == ret, ret,
        debug_log(DLOG_ERROR, "%s, %d: failed to set min port for %s, port = %d, ret = %d", __FUNCTION__, __LINE__,
        CLASS_SMS, ip_port_min, ret));
    return VOS_OK;

ERR:

    (void)create_message_info(MSGID_PROP_INVALID_VALUE, prop_str, &obj_jso, MSGID_PROP_INVALID_VALUE, prop_str);
    ret = json_object_array_add(*o_message_jso, obj_jso);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail\n", __FUNCTION__, __LINE__));

    return HTTP_BAD_REQUEST;
}


LOCAL gint32 set_veth_max_port(PROVIDER_PARAS_S *i_paras, json_object *item_jso_max, json_object **o_message_jso)
{
    guint32 ip_port_max = 0;
    guint32 ip_port_min = 0;
    OBJ_HANDLE obj_handle = 0;
    gchar prop_str[SMS_STRLEN_128] = {0};
    json_object *obj_jso = NULL;

    if (item_jso_max == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL Pointer. ", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &obj_jso);
        json_object_array_add(*o_message_jso, obj_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (snprintf_s(prop_str, sizeof(prop_str), sizeof(prop_str) - 1, "%s/%s/", SMS_BOB_SERVICE_STR, SMS_MAX_PORT) <=
        0) {
        debug_log(DLOG_ERROR, "%s, %d, snprintf_s fail", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    // 获取最大端口
    gint32 ret = get_bma_veth_port(i_paras, item_jso_max, o_message_jso, &ip_port_max);
    return_val_do_info_if_fail(VOS_OK == ret, ret,
        debug_log(DLOG_ERROR, "%s, %d: get BMA veth max port failed, ret = %d.", __FUNCTION__, __LINE__, ret));

    // 校验最大端口范围
    if ((SMTP_TRAP_MIN_PORT > ip_port_max) || (SMTP_TRAP_MAX_PORT < ip_port_max)) {
        debug_log(DLOG_ERROR, "%s max port is invalid, max_port is %d\n", __FUNCTION__, ip_port_max);
        goto ERR;
    }

    ret = dal_get_object_handle_nth(CLASS_SMS, 0, &obj_handle);
    return_val_if_expr(ret != VOS_OK, ret);

    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_SMS_MINPORT, &ip_port_min);
    return_val_do_info_if_fail(VOS_OK == ret, ret,
        debug_log(DLOG_ERROR, "%s: get PROPERTY_SMS_MINPORT for %s fail, ret = %d", __FUNCTION__, CLASS_SMS, ret));

    // 校验大小关系以及判断是否至少有16个端口
    if (ip_port_max < ip_port_min || (ip_port_max - ip_port_min + 1) < SNMP_BMA_VETH_PORT_NUMBER) {
        debug_log(DLOG_ERROR, "%s: max port %d, min port %d is invalid", __FUNCTION__, ip_port_max, ip_port_min);
        goto ERR;
    }

    // 设置最大端口值
    ret = set_veth_port(i_paras, o_message_jso, METHOD_SMS_SET_MAX_PORT, obj_handle, ip_port_max);
    return_val_do_info_if_fail(VOS_OK == ret, ret,
        debug_log(DLOG_ERROR, "%s: set max port for %s fail, port = %d, ret = %d", __FUNCTION__,
        CLASS_SMS, ip_port_max, ret));

    return VOS_OK;

ERR:

    (void)create_message_info(MSGID_PROP_INVALID_VALUE, prop_str, &obj_jso, MSGID_PROP_INVALID_VALUE, prop_str);
    ret = json_object_array_add(*o_message_jso, obj_jso);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR, 
        debug_log(DLOG_ERROR, "%s: json_object_array_add fail", __FUNCTION__));

    return HTTP_BAD_REQUEST;
}


LOCAL gint32 set_veth_min_port(PROVIDER_PARAS_S *i_paras, json_object *item_jso_min, json_object **o_message_jso)
{
    guint32 ip_port_max = 0;
    guint32 ip_port_min = 0;
    OBJ_HANDLE obj_handle = 0;
    gchar prop_str[SMS_STRLEN_128] = {0};
    json_object *obj_jso = NULL;

    if (item_jso_min == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL Pointer. ", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &obj_jso);
        json_object_array_add(*o_message_jso, obj_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (snprintf_s(prop_str, sizeof(prop_str), sizeof(prop_str) - 1, "%s/%s/", SMS_BOB_SERVICE_STR, SMS_MIN_PORT) <=
        0) {
        debug_log(DLOG_ERROR, "%s, %d, snprintf_s fail", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    // 获取最小端口
    gint32 ret = get_bma_veth_port(i_paras, item_jso_min, o_message_jso, &ip_port_min);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d: get BMA veth min port failed, ret = %d.", __FUNCTION__, __LINE__, ret));

    // 校验最小端口范围
    if ((SMTP_TRAP_MIN_PORT > ip_port_min) || (SMTP_TRAP_MAX_PORT < ip_port_min)) {
        debug_log(DLOG_ERROR, "%s min port is invalid, max_port is %d\n", __FUNCTION__, ip_port_min);
        goto ERR;
    }

    ret = dal_get_object_handle_nth(CLASS_SMS, 0, &obj_handle);
    return_val_if_expr(ret != VOS_OK, ret);

    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_SMS_MAXPORT, &ip_port_max);
    return_val_do_info_if_fail(VOS_OK == ret, ret,
        debug_log(DLOG_ERROR, "%s, %d: get PROPERTY_SMS_MAXPORT for %s fail, ret = %d", __FUNCTION__, __LINE__,
        CLASS_SMS, ret));

    // 校验大小关系
    if (ip_port_max < ip_port_min) {
        debug_log(DLOG_ERROR, "%s max port (%d) is not bigger than min port(%d).\n", __FUNCTION__, ip_port_max,
            ip_port_min);
        goto ERR;
    }

    // 判断是否至少有16个端口
    if (SNMP_BMA_VETH_PORT_NUMBER > ip_port_max - ip_port_min + 1) {
        debug_log(DLOG_ERROR, "%s: max port(%d) - min port(%d) + 1 < %d .\n", __FUNCTION__, ip_port_max, ip_port_min,
            SNMP_BMA_VETH_PORT_NUMBER);
        goto ERR;
    }

    // 设置最小端口值
    ret = set_veth_port(i_paras, o_message_jso, METHOD_SMS_SET_MIN_PORT, obj_handle, ip_port_min);

    return_val_do_info_if_fail(VOS_OK == ret, ret,
        debug_log(DLOG_ERROR, "%s, %d: set min port for %s fail, port = %d, ret = %d", __FUNCTION__, __LINE__,
        CLASS_SMS, ip_port_min, ret));

    return VOS_OK;

ERR:

    (void)create_message_info(MSGID_PROP_INVALID_VALUE, prop_str, &obj_jso, MSGID_PROP_INVALID_VALUE, prop_str);
    ret = json_object_array_add(*o_message_jso, obj_jso);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail\n", __FUNCTION__, __LINE__));

    return HTTP_BAD_REQUEST;
}


LOCAL gint32 update_trap_item_bma_veth_port(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso)
{
    gint32 ret;
    json_object *obj_jso = NULL;
    guint32 min_port = 0;
    OBJ_HANDLE obj_handle = 0;
    guint8 item_num;
    OBJ_HANDLE trap_item = 0;
    GSList *input_list = NULL;

    ret = dal_get_object_handle_nth(CLASS_SMS, 0, &obj_handle);
    return_val_if_expr(ret != VOS_OK, ret);
    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_SMS_MINPORT, &min_port);
    return_val_do_info_if_fail(VOS_OK == ret, ret,
        debug_log(DLOG_ERROR, "%s, %d: get min port for %s fail,ret = %d", __FUNCTION__, __LINE__, CLASS_SMS, ret));

    debug_log(DLOG_DEBUG, "%s begin to update BMA veth port", __FUNCTION__);

    // 遍历通道，其BMA端口号由最小端口开始递增
    for (item_num = 0; item_num < SNMP_TRAP_ITEM_NUMBER; ++item_num) {
        ret = dal_get_specific_object_byte(CLASS_TRAP_ITEM_CONFIG, PROPERTY_TRAP_ITEM_INDEX_NUM, item_num, &trap_item);
        continue_do_info_if_expr(VOS_OK != ret, debug_log(DLOG_ERROR,
            "%s get CLASS_TRAP_ITEM_CONFIG handle failed: ret = %d, index = %d.\n", __FUNCTION__, ret, item_num));

        input_list = g_slist_append(input_list, g_variant_new_uint32(min_port + item_num));

        ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, trap_item,
            CLASS_TRAP_ITEM_CONFIG, METHOD_TRAP_ITEM_SET_BMA_VETH_PORT, AUTH_ENABLE, i_paras->user_role_privilege,
            input_list, NULL);

        uip_free_gvariant_list(input_list);

        input_list = NULL;

        if (VOS_OK != ret) {
            debug_log(DLOG_ERROR, "%s, %d, Set min veth port failed, port = %d, item_num = %d, ret = %d", __FUNCTION__,
                __LINE__, min_port, item_num, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &obj_jso);
            ret = json_object_array_add(*o_message_jso, obj_jso);
            return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
                debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail\n", __FUNCTION__, __LINE__));
        }

        debug_log(DLOG_DEBUG, "%s set trap item BMA veth port successfully, item_num = %d, port = %d", __FUNCTION__,
            item_num, min_port + item_num);
    }

    return VOS_OK;
}


LOCAL gint32 set_bob_service(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gint32 succ_flag = 0;
    json_bool jso_ret;
    json_bool jso_ret_min;
    json_bool jso_ret_max;
    json_object *tmp_obj = NULL;
    json_object *tmp_obj_min = NULL;
    json_object *tmp_obj_max = NULL;

    if ((VOS_OK != provider_paras_check(i_paras)) || (NULL == o_message_jso)) {
        debug_log(DLOG_ERROR, "%s %d NULL Pointer!.", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV,
        g_manager_sms_provider[i_paras->index].property_name, o_message_jso,
        g_manager_sms_provider[i_paras->index].property_name));

    return_val_do_info_if_fail(NULL != i_paras->val_jso, HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "%s, %d, wrong value type", __FUNCTION__, __LINE__));

    *o_message_jso = json_object_new_array();
    return_val_do_info_if_fail(NULL != *o_message_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  json_object_new_array fail\n", __FUNCTION__, __LINE__));

    // 端口上限和端口下限
    jso_ret_max = json_object_object_get_ex(i_paras->val_jso, SMS_MAX_PORT, &tmp_obj_max);
    jso_ret_min = json_object_object_get_ex(i_paras->val_jso, SMS_MIN_PORT, &tmp_obj_min);
    if ((TRUE == jso_ret_max) && (TRUE == jso_ret_min)) {
        ret = set_veth_max_min_port(i_paras, tmp_obj_max, tmp_obj_min, o_message_jso);
        do_if_expr(VOS_OK == ret, succ_flag++);
    }

    // 只设置端口上限
    if ((TRUE == jso_ret_max) && (FALSE == jso_ret_min)) {
        ret = set_veth_max_port(i_paras, tmp_obj_max, o_message_jso);
        do_if_expr(VOS_OK == ret, succ_flag++);
    }

    // 只设置端口下限
    if ((FALSE == jso_ret_max) && (TRUE == jso_ret_min)) {
        ret = set_veth_min_port(i_paras, tmp_obj_min, o_message_jso);
        do_if_expr(VOS_OK == ret, succ_flag++);
    }

    ret = update_trap_item_bma_veth_port(i_paras, o_message_jso);
    do_if_expr(VOS_OK == ret, succ_flag++);

    // 告警状态
    jso_ret = json_object_object_get_ex(i_paras->val_jso, SMS_ALARM_STATE, &tmp_obj);
    if (TRUE == jso_ret) {
        ret = set_sms_alarm_state(i_paras, tmp_obj, o_message_jso);
        do_if_expr(VOS_OK == ret, succ_flag++);
    }

    tmp_obj = NULL;

    // 告警描述
    jso_ret = json_object_object_get_ex(i_paras->val_jso, SMS_ALARM_DESC, &tmp_obj);
    if (TRUE == jso_ret) {
        ret = set_sms_alarm_desc(i_paras, tmp_obj, o_message_jso);
        do_if_expr(VOS_OK == ret, succ_flag++);
    }

    return_val_if_expr(succ_flag <= 0, HTTP_BAD_REQUEST);

    return HTTP_OK;
}


LOCAL gint32 get_cdev_channel_state(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guchar cdev_enabled = ENABLED;
    json_bool is_enabled;

    return_val_do_info_if_expr(provider_paras_check(i_paras) != VOS_OK || o_message_jso == NULL || o_result_jso == NULL,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dal_get_object_handle_nth(CLASS_SMS, 0, &obj_handle);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s: get object handle of class %s failed, ret :%d", __FUNCTION__, CLASS_SMS, ret));

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SMS_CDEV_CHANNEL_ENABLED, &cdev_enabled);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:get property %s failed, ret:%d", __FUNCTION__, PROPERTY_SMS_CDEV_CHANNEL_ENABLED,
        ret));

    is_enabled = (cdev_enabled == ENABLED ? TRUE : FALSE);
    *o_result_jso = json_object_new_boolean(is_enabled);
    return_val_do_info_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_string failed", __FUNCTION__));

    return HTTP_OK;
}


LOCAL gint32 set_cdev_channel_state(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    json_bool cdev_enabled;
    GSList *input_list = NULL;

    return_val_do_info_if_expr(provider_paras_check(i_paras) != VOS_OK || o_message_jso == NULL || o_result_jso == NULL,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__));

    ret = dal_get_object_handle_nth(CLASS_SMS, 0, &obj_handle);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s: get object handle of class %s failed, ret :%d", __FUNCTION__, CLASS_SMS, ret));

    cdev_enabled = json_object_get_boolean(i_paras->val_jso);
    if (cdev_enabled) {
        
        ret = rf_inner_session_id_check((const gchar *)i_paras->session_id);
        return_val_do_info_if_expr(ret == VOS_OK, HTTP_BAD_REQUEST,
            debug_log(DLOG_ERROR, "%s: cannot open cdev channel by inband channel request", __FUNCTION__);
            (void)create_message_info(MSGID_FORBID_SET_PROP_BY_INBAND_CHANNEL, SMS_CDEV_CHANNEL_ENABLED, o_message_jso,
            SMS_CDEV_CHANNEL_ENABLED));

        input_list = g_slist_append(input_list, g_variant_new_byte(ENABLED));
    } else {
        input_list = g_slist_append(input_list, g_variant_new_byte(DISABLE));
    }

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SMS, METHOD_SMS_SET_CDEV_CHANNEL_ENABLED, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    switch (ret) {
        case VOS_OK:
            return HTTP_OK;
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
            return HTTP_FORBIDDEN;
        default:
            debug_log(DLOG_ERROR, "%s: call %s failed, ret:%d", __FUNCTION__, METHOD_SMS_SET_CDEV_CHANNEL_ENABLED, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, SMS_CDEV_CHANNEL_ENABLED, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}

#define MAX_CHANNEL_TYPE_NUM 5
const gchar *gChTypeStr[MAX_CHANNEL_TYPE_NUM] = {
    "PCIe-Veth",         // Veth 通道类型为PCIe
    "PCIe-Cdev",         // Cdev 通道类型为PCIe
    "USB-Linux",
    "USB-Windows",
    "USB-VmWare"
};


LOCAL gint32 get_channel_type(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint8 channel_type;
    GSList* output_list = NULL;

    if (provider_paras_check(i_paras) != VOS_OK || o_message_jso == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (!i_paras->is_satisfy_privi) {
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = dal_get_object_handle_nth(CLASS_SMS, 0, &obj_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_DEBUG, "%s: get object handle of class %s failed, ret :%d", __FUNCTION__, CLASS_SMS, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dfl_call_class_method(obj_handle, METHOD_SMS_GET_CHANNEL_TYPE, NULL, NULL, &output_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s]: dfl_call_class_method failed! %d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    channel_type = g_variant_get_byte((GVariant*)g_slist_nth_data(output_list, 0));
    uip_free_gvariant_list(output_list);

    if (channel_type >= MAX_CHANNEL_TYPE_NUM) {
        debug_log(DLOG_ERROR, "[%s]: channel_type %d error!\n", __FUNCTION__, channel_type);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string(gChTypeStr[channel_type]);
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_string failed", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 set_channel_type(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    const char *str = NULL;
    guint8 index;
    GSList *input_list = NULL;

    if (provider_paras_check(i_paras) != VOS_OK || o_message_jso == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    str = dal_json_object_get_str(i_paras->val_jso);

    for (index = 0; index < MAX_CHANNEL_TYPE_NUM; index++) {
        if (g_strcmp0(str, gChTypeStr[index]) == 0) {
            break;
        }
    }

    debug_log(DLOG_DEBUG, "%s: index=%d input:%s", __FUNCTION__, index, str);
    // 1710暂不支持USB模式 1711暂只支持linux系统的USB模式
#ifdef ARM64_HI1711_ENABLED
    if (index > USB_LINUX) {
#else
    if (index >= USB_LINUX) {
#endif
        (void)create_message_info(MSGID_PROP_INVALID_VALUE, SMS_CHANNEL_TYPE, o_message_jso, str, SMS_CHANNEL_TYPE);
        return HTTP_BAD_REQUEST;
    }

    ret = dal_get_object_handle_nth(CLASS_SMS, 0, &obj_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_DEBUG, "%s: get object handle of class %s failed, ret :%d", __FUNCTION__, CLASS_SMS, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(index));
    input_list = g_slist_append(input_list, g_variant_new_uint32(DF_SAVE));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SMS, METHOD_SMS_SET_CHANNEL_TYPE, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret == VOS_OK) {
        return HTTP_OK;
    } else if (ret == COMP_CODE_STATUS_INVALID) {
        // U盘连接时不允许设置通道类型为USB通道
        (void)create_message_info(MSGID_UMS_IS_EXCLUSIVELY_USED, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    } else if (ret == RFERR_INSUFFICIENT_PRIVILEGE) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    } else {
        debug_log(DLOG_ERROR, "%s: call %s failed, ret:%d", __FUNCTION__, METHOD_SMS_SET_CHANNEL_TYPE, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, SMS_CHANNEL_TYPE, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 get_sms_actioninfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    ret = get_odata_id(i_paras, o_message_jso, o_result_jso, ACTION_INFO_REFRESH_INSTALLABLE_BMA);
    if (ret != HTTP_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 get_manager_smsservice_actions(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    gchar sms_target_uri[PSLOT_URI_LEN] = {0};
    gint32 ret;
    json_object *action_jso = NULL;
    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = redfish_get_board_slot(slot, sizeof(slot));
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: function return err of redfish_slot_id.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = snprintf_s(sms_target_uri, sizeof(sms_target_uri), sizeof(sms_target_uri) - 1, URI_FORMAT_SMSSERVICE, slot);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    action_jso = json_object_new_object();
    if (action_jso == NULL) {
        debug_log(DLOG_ERROR, "alloc new object failed");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    rf_add_action_prop(action_jso, sms_target_uri, RFPROP_SMSSERVICE_REFRESH_BMA);
    *o_result_jso = action_jso;
    return HTTP_OK;
}


LOCAL gint32 __check_input_parameter(json_object *i_val_jso, json_object **o_message_jso)
{
    if (json_object_has_key(i_val_jso, RFACTION_PARAM_IMAGE_URI) == TRUE) {
        debug_log(DLOG_ERROR, "%s:parameter %s is redundant.", __FUNCTION__, RFACTION_PARAM_IMAGE_URI);
        (void)create_message_info(MSGID_IBMA_DELETE_PROP_REDUNDANT, RFACTION_PARAM_IMAGE_URI, o_message_jso,
            RFACTION_PARAM_IMAGE_URI);
        return RET_ERR;
    }
    if (json_object_has_key(i_val_jso, RFPROP_MANAGER_SMS_ACTION_PARAM_SI_URI) == TRUE) {
        debug_log(DLOG_ERROR, "%s:parameter %s is redundant.", __FUNCTION__, 
            RFPROP_MANAGER_SMS_ACTION_PARAM_SI_URI);
        (void)create_message_info(MSGID_IBMA_DELETE_PROP_REDUNDANT, RFPROP_MANAGER_SMS_ACTION_PARAM_SI_URI, 
            o_message_jso, RFPROP_MANAGER_SMS_ACTION_PARAM_SI_URI);
        return RET_ERR;
    }
    if (json_object_has_key(i_val_jso, RFPROP_MANAGER_SMS_ACTION_PARAM_CRL_URI) == TRUE) {
        debug_log(DLOG_ERROR, "%s:parameter %s is redundant.", __FUNCTION__, 
            RFPROP_MANAGER_SMS_ACTION_PARAM_CRL_URI);
        (void)create_message_info(MSGID_IBMA_DELETE_PROP_REDUNDANT, RFPROP_MANAGER_SMS_ACTION_PARAM_CRL_URI, 
            o_message_jso, RFPROP_MANAGER_SMS_ACTION_PARAM_CRL_URI);
        return RET_ERR;
    }
   
    return RET_OK;
}


LOCAL gint32 delete_installable_bma(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    const gchar *dest_file = NULL;
    
    ret = __check_input_parameter(i_paras->val_jso, o_message_jso);
    if (ret != RET_OK) {
        return HTTP_BAD_REQUEST;
    }

    gint32 bma_flag = get_installable_bma_state();
    if (bma_flag != VOS_OK) {
        if (bma_flag == BMA_FILE_NOT_EXIST) {
            return HTTP_OK;
        }
        debug_log(DLOG_ERROR, "%s: installable bma flag = %d.", __FUNCTION__, bma_flag);
        (void)create_message_info(MSGID_ACT_NOT_SUPPORTED, RFPROP_SMSSERVICE_REFRESH_BMA, o_message_jso,
            RFPROP_SMSSERVICE_REFRESH_BMA);
        return HTTP_BAD_REQUEST;
    }

    ret = dfl_get_object_handle(OBJ_NAME_PRIVILEGEAGENT, &obj_handle);
    if (ret != VOS_OK) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: get obj_handle fail, ret = %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    input_list = g_slist_append(input_list, g_variant_new_string(INSTALLABLE_BMA_PATH));
    if (input_list == NULL) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: input_list append delete bma file fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    dest_file = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 0), NULL);
    if (dest_file == NULL || strlen(dest_file) >= MAX_FILENAME_LENGTH) {
        debug_log(DLOG_ERROR, "%s: parameter error", __FUNCTION__);
        dest_file = NULL;
        return HTTP_BAD_REQUEST;
    }

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        OBJ_NAME_PRIVILEGEAGENT, METHOD_PRIVILEGEAGENT_DELETE_FILE, AUTH_DISABLE, i_paras->user_role_privilege,
        input_list, NULL);

    uip_free_gvariant_list(input_list);

    if (ret != VOS_OK) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: Delete installable bma failed, ret = %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 __get_installable_bma_up_resource(json_object *i_val_jso, IBMA_UP_ARGS *up_args,
                                               json_object **o_message_jso)
{
    if (get_element_str(i_val_jso, RFACTION_PARAM_IMAGE_URI, &(up_args->image_uri)) == FALSE) {
        debug_log(DLOG_ERROR, "%s: wrong value type, %s", __FUNCTION__, RFACTION_PARAM_IMAGE_URI);
        (void)create_message_info(MSGID_PROP_MISSING, RFACTION_PARAM_IMAGE_URI, o_message_jso,
            RFACTION_PARAM_IMAGE_URI);
        return RET_ERR;
    }
    if (get_element_str(i_val_jso, RFPROP_MANAGER_SMS_ACTION_PARAM_SI_URI, &(up_args->signature_uri)) == FALSE) {
        debug_log(DLOG_ERROR, "%s: wrong value type, %s", __FUNCTION__, RFPROP_MANAGER_SMS_ACTION_PARAM_SI_URI);
        (void)create_message_info(MSGID_PROP_MISSING, RFPROP_MANAGER_SMS_ACTION_PARAM_SI_URI, o_message_jso,
            RFPROP_MANAGER_SMS_ACTION_PARAM_SI_URI);
        return RET_ERR;
    }
    if (get_element_str(i_val_jso, RFPROP_MANAGER_SMS_ACTION_PARAM_CRL_URI, &(up_args->crl_uri)) == FALSE) {
        debug_log(DLOG_ERROR, "%s: wrong value type, %s", __FUNCTION__, RFPROP_MANAGER_SMS_ACTION_PARAM_CRL_URI);
        (void)create_message_info(MSGID_PROP_MISSING, RFPROP_MANAGER_SMS_ACTION_PARAM_CRL_URI, o_message_jso,
            RFPROP_MANAGER_SMS_ACTION_PARAM_CRL_URI);
        return RET_ERR;
    }
   
    return RET_OK;
}

LOCAL gboolean __check_file_name_length(const gchar *url_path)
{
    const gchar *pStr = NULL;
    
    pStr = strrchr(url_path, '/');
    if (pStr == NULL) {
        debug_log(DLOG_ERROR, "%s:pStr is NULL.", __FUNCTION__);
        return FALSE;
    }

    if (strlen(pStr + 1) > MAX_FILENAME_LEN) {
        debug_log(DLOG_ERROR, "%s:file name length is too long.", __FUNCTION__);
        return FALSE;
    }

    return TRUE;
}


LOCAL gboolean __check_update_bma_file_name(const IBMA_UP_ARGS *up_args, json_object **o_message_jso)
{  
    if (__check_file_name_length(up_args->image_uri) != TRUE) {
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFACTION_PARAM_IMAGE_URI, o_message_jso,
            RF_SENSITIVE_INFO, RFACTION_PARAM_IMAGE_URI);
        return FALSE;
    }

    if (__check_file_name_length(up_args->crl_uri) != TRUE) {
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFPROP_MANAGER_SMS_ACTION_PARAM_CRL_URI, o_message_jso,
            RF_SENSITIVE_INFO, RFPROP_MANAGER_SMS_ACTION_PARAM_CRL_URI);
        return FALSE;
    }

    if (__check_file_name_length(up_args->signature_uri) != TRUE) {
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFPROP_MANAGER_SMS_ACTION_PARAM_SI_URI, o_message_jso,
            RF_SENSITIVE_INFO, RFPROP_MANAGER_SMS_ACTION_PARAM_SI_URI);
        return FALSE;
    }
    return TRUE;
}


LOCAL gint32 __check_installable_bma_update_uri(const IBMA_UP_ARGS *up_args, json_object **o_message_jso)
{
    gint32 b_ret;
    
    
    if (__check_update_bma_file_name(up_args, o_message_jso) != TRUE) {
        return HTTP_BAD_REQUEST;
    }
    
    
    b_ret = g_regex_match_simple(BMA_ACTION_LOCAL_IM_URI_REGEX, up_args->image_uri, G_REGEX_OPTIMIZE,
        (GRegexMatchFlags)0);
    if (b_ret == TRUE) {
        b_ret = g_regex_match_simple(BMA_ACTION_LOCAL_SI_URI_REGEX, up_args->signature_uri, G_REGEX_OPTIMIZE,
        (GRegexMatchFlags)0);
        if (b_ret != TRUE) {
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFPROP_MANAGER_SMS_ACTION_PARAM_SI_URI, o_message_jso,
                RF_SENSITIVE_INFO, RFPROP_MANAGER_SMS_ACTION_PARAM_SI_URI);
            return HTTP_BAD_REQUEST;
        }

        b_ret = g_regex_match_simple(BMA_ACTION_LOCAL_CRL_URI_REGEX, up_args->crl_uri, G_REGEX_OPTIMIZE,
            (GRegexMatchFlags)0);
        if (b_ret != TRUE) {
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFPROP_MANAGER_SMS_ACTION_PARAM_CRL_URI, o_message_jso,
                RF_SENSITIVE_INFO, RFPROP_MANAGER_SMS_ACTION_PARAM_CRL_URI);
            return HTTP_BAD_REQUEST;
        }
        
        return RET_OK;
    }
    
    
    b_ret = g_regex_match_simple(BMA_ACTION_REMOTE_IM_URI_REGEX, up_args->image_uri, G_REGEX_OPTIMIZE,
        (GRegexMatchFlags)0);
    if (b_ret != TRUE) {
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFACTION_PARAM_IMAGE_URI, o_message_jso,
            RF_SENSITIVE_INFO, RFACTION_PARAM_IMAGE_URI);
        return HTTP_BAD_REQUEST;
    }

    b_ret = g_regex_match_simple(BMA_ACTION_REMOTE_SI_URI_REGEX, up_args->signature_uri, G_REGEX_OPTIMIZE,
        (GRegexMatchFlags)0);
    if (b_ret != TRUE) {
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFPROP_MANAGER_SMS_ACTION_PARAM_SI_URI, o_message_jso,
            RF_SENSITIVE_INFO, RFPROP_MANAGER_SMS_ACTION_PARAM_SI_URI);
        return HTTP_BAD_REQUEST;
    }

    b_ret = g_regex_match_simple(BMA_ACTION_REMOTE_CRL_URI_REGEX, up_args->crl_uri, G_REGEX_OPTIMIZE,
        (GRegexMatchFlags)0);
    if (b_ret != TRUE) {
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFPROP_MANAGER_SMS_ACTION_PARAM_CRL_URI, o_message_jso,
            RF_SENSITIVE_INFO, RFPROP_MANAGER_SMS_ACTION_PARAM_CRL_URI);
        return HTTP_BAD_REQUEST;
    }
    
    return RET_OK;
}


LOCAL gint32 __parse_upgrade_installable_bma_errcode(gint32 ret, json_object **o_message_jso)
{
    switch (ret) {
        case UMS_WR_ERR_IN_UPGRADING:
            (void)create_message_info(MSGID_TASK_DUMP_EXPORT_UPGRADING_ERR, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;

        case UMS_WR_ERR_BUSY:
            (void)create_message_info(MSGID_UMS_IS_EXCLUSIVELY_USED, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;

        case UMS_WR_ERR_BMC_NOT_SUPPORT:
            (void)create_message_info(MSGID_BMC_NOT_SUPPORT_IBMA, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;

        default: 
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL void __parse_installable_bma_upgrade_task_errcode(gint32 ret, json_object **message_obj)
{
    guint32 i;
    INT2STR_MAP_S upgrade_error_array[] = {
        { FIND_CLASS_ERROR,  MSGID_MONITOR_TASK_FAIL },
        { IMAGEURI_IS_ILLEGAL, MSGID_IBMA_UPGRADE_URI_ILLEGAL },
        { CMS_AUTH_ERROR,  MSGID_IBMA_UPGRADE_CMS_AUTH_ERROR },
        { IBMA_UPGRADE_INVALID_PACAKE, MSGID_IBMA_UPGRADE_PACAKE_INVALID},
        { IBMA_OF_ARM_NEEDED, MSGID_IBMA_PLATFORM_NOT_MATCH_BMC},
        { IBMA_OF_X86_NEEDED, MSGID_IBMA_PLATFORM_NOT_MATCH_BMC}
    };

    for (i = 0; i < G_N_ELEMENTS(upgrade_error_array); i++) {
        if (ret == upgrade_error_array[i].int_val) {
            (void)create_message_info(upgrade_error_array[i].str_val, NULL, message_obj);
            return;
        }
    }

    (void)create_message_info(MSGID_MONITOR_TASK_FAIL, NULL, message_obj);
}


LOCAL gint32 upgrade_installable_bma_status_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    OBJ_HANDLE ibma_obj_handle = INVALID_OBJ_HANDLE;
    gint32 update_progress;
    gint32 trans_file_progress;
    gchar trans_file_name[MAX_FILE_NAME] = { 0 };
    gchar progress_str[MAX_MESSAGE_ARGS_LEN] = {0};
    gint32 ret;

    // 入参校验
    if (origin_obj_path == NULL || body_jso == NULL || monitor_fn_data == NULL || message_obj == NULL) {
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__);
        return RF_FAILED;
    }
    
    // 获取句柄
    ret = dal_get_object_handle_nth(CLASS_IBMA_UMS_NAME, 0, &ibma_obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get object handle for %s failed", __FUNCTION__, CLASS_IBMA_UMS_NAME);
        return RFERR_NO_RESOURCE;
    }
    
    // 获取升级进度信息
    (void)dal_get_property_value_int32(ibma_obj_handle, PROPERTY_IBMA_UMS_UPGRADE_PROGRESS, &update_progress);
    if (update_progress == INSTALLABLE_IBMA_UPGRADE_NO_PROGRESS) {
        (void)dal_get_property_value_int32(ibma_obj_handle, PROPERTY_IBMA_UMS_TRANSFER_PROGRESS, &trans_file_progress);
        (void)dal_get_property_value_string(ibma_obj_handle, PROPERTY_IBMA_UMS_TRANSFER_FILE_NAME, trans_file_name,
            sizeof(trans_file_name));
        ret = parse_file_transfer_err_code(trans_file_progress, message_obj);
        if (ret != RET_OK) {
            monitor_fn_data->task_state = RF_TASK_EXCEPTION;
            debug_log(DLOG_ERROR, "%s:ret = %d", __FUNCTION__, ret);
            return RET_OK;
        }
        ret = snprintf_s(progress_str, sizeof(progress_str), sizeof(progress_str) - 1, 
            "%s:%d%%", trans_file_name, trans_file_progress);
        if (ret <= RET_OK) {
            debug_log(DLOG_ERROR, "%s: snprintf_s failed", __FUNCTION__);
        }
        (void)create_message_info(MSGID_FILE_TRANSFER_PROGRESS, NULL, message_obj,
            (const gchar *)progress_str);
        monitor_fn_data->task_state = RF_TASK_RUNNING;
    } else if (update_progress == RF_FINISH_PERCENTAGE) {
        monitor_fn_data->task_state = RF_TASK_COMPLETED;
        monitor_fn_data->task_progress = RF_FINISH_PERCENTAGE;
        (void)create_message_info(MSGID_IBMA_UPGRADE_OK, NULL, message_obj);
    } else if (update_progress < 0){
        monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        __parse_installable_bma_upgrade_task_errcode(update_progress, message_obj);
    } else {
        // 设置任务正在执行
        monitor_fn_data->task_state = RF_TASK_RUNNING;
        monitor_fn_data->task_progress = update_progress;
        (void)create_message_info(MSGID_IBMA_UPGRADE_RUN, NULL, message_obj);
    }

    return RET_OK;
}


LOCAL gint32 __create_upgrade_installable_ibma_monitor_task(PROVIDER_PARAS_S *i_paras,
    json_object **o_result_jso)
{
    gint32 ret;
    TASK_MONITOR_INFO_S *task_monitor_info = NULL;

    task_monitor_info = task_monitor_info_new(NULL);
    if (task_monitor_info == NULL) {
        debug_log(DLOG_ERROR, "%s: task_monitor_info_new failed", __FUNCTION__);
        return RET_ERR;    
    }
    
    task_monitor_info->task_progress = TASK_NO_PROGRESS;  
    task_monitor_info->rsc_privilege = USERROLE_BASICSETTING;
    ret = create_new_task(INSTALLABLE_BMA_UPGRADE_TASK_DESC, upgrade_installable_bma_status_monitor, 
        task_monitor_info, i_paras->val_jso, i_paras->uri, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: create new task failed", __FUNCTION__);
        task_monitor_info_free(task_monitor_info);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 upgrade_installable_bma(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = INVALID_OBJ_HANDLE;
    GSList *input_list = NULL;
    gint32 bma_flag;
    IBMA_UP_ARGS up_args;
    
    // 判断是否支持升级 
    bma_flag = get_installable_bma_state();
    if (bma_flag != RET_OK && bma_flag != BMA_FILE_NOT_EXIST) {
        debug_log(DLOG_ERROR, "%s: upgrade_installable_bma bma flag = %d.", __FUNCTION__, bma_flag);
        (void)create_message_info(MSGID_ACT_NOT_SUPPORTED, RFPROP_SMSSERVICE_REFRESH_BMA, o_message_jso,
            RFPROP_SMSSERVICE_REFRESH_BMA);
        return HTTP_BAD_REQUEST;
    }
   
    
    memset_s(&up_args, sizeof(IBMA_UP_ARGS), 0, sizeof(IBMA_UP_ARGS));
    if (__get_installable_bma_up_resource(i_paras->val_jso, &up_args, o_message_jso) != RET_OK) {
        clear_sensitive_info(&up_args, sizeof(IBMA_UP_ARGS));
        return HTTP_BAD_REQUEST;
    }
  
    
    if (__check_installable_bma_update_uri(&up_args, o_message_jso) != RET_OK) {
        clear_sensitive_info(&up_args, sizeof(IBMA_UP_ARGS));
        return HTTP_BAD_REQUEST;
    }
  
    
    input_list = g_slist_append(input_list, g_variant_new_string(up_args.image_uri));
    input_list = g_slist_append(input_list, g_variant_new_string(up_args.signature_uri));
    input_list = g_slist_append(input_list, g_variant_new_string(up_args.crl_uri));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_IBMA_UMS_NAME, UMS_METHOD_UPDATE_INSTALLABLE_IBMA, AUTH_DISABLE, i_paras->user_role_privilege, 
        input_list, NULL);
    uip_free_gvariant_list(input_list);
    clear_sensitive_info(&up_args, sizeof(IBMA_UP_ARGS));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: call UMS_METHOD_UPDATE_INSTALLABLE_IBMA fail, ret = %d.", __FUNCTION__, ret);
        return __parse_upgrade_installable_bma_errcode(ret, o_message_jso);
    }
    
    
    ret = __create_upgrade_installable_ibma_monitor_task(i_paras, o_result_jso);
    if (ret != RET_OK) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    return HTTP_ACCEPTED;
}


LOCAL gint32 act_manager_refresh_bma(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
#define MODE_DELETE "Delete"
#define MODE_UPGRADE "Upgrade"

    gint32 ret = 0;
    const gchar *str_mode = NULL;
    GSList *caller_info = NULL;
    gchar custom_name[LOG_SPECIAL_STRING_LEN_MAX] = {"iBMA"};

    
    if (i_paras == NULL || o_result_jso == NULL || o_message_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (dal_get_custom_string_name(custom_name, BMC_CUSTOM_LOGIBMA_NAME, sizeof(custom_name)) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_custom_string_name failed.", __FUNCTION__);
    }

    
    if ((i_paras->user_role_privilege & USERROLE_BASICSETTING) == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: has no USERROLE_BASICSETTING privilege.", __FUNCTION__);
        return HTTP_FORBIDDEN;
    }

    
    if (get_element_str(i_paras->val_jso, RFPROP_MANAGER_SMS_ACTION_PARAM_MODE, &str_mode) == FALSE) {
        debug_log(DLOG_ERROR, "%s: wrong value type, %s", __FUNCTION__, RFPROP_MANAGER_SMS_ACTION_PARAM_MODE);
        (void)create_message_info(MSGID_PROP_MISSING, RFPROP_MANAGER_SMS_ACTION_PARAM_MODE, o_message_jso,
            RFPROP_MANAGER_SMS_ACTION_PARAM_MODE);
        return HTTP_BAD_REQUEST;
    }

    if (g_strcmp0(str_mode, MODE_DELETE) == 0) {
        
        ret = delete_installable_bma(i_paras, o_message_jso);
        
        caller_info = g_slist_append(caller_info, 
            g_variant_new_string(dal_rf_get_op_log_module_name(i_paras->is_from_webui)));
        caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->user_name));
        caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->client));
        if (ret == HTTP_OK) {
            (void)proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "%s installable %s successfully",
                str_mode, custom_name);
        } else {
            (void)proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "%s installable %s failed",
                str_mode, custom_name);
        }
        uip_free_gvariant_list(caller_info);
    } else if (g_strcmp0(str_mode, MODE_UPGRADE) == 0) {
        
        ret = upgrade_installable_bma(i_paras, o_message_jso, o_result_jso);
    }
    
    return ret;
}


gint32 sms_refresh_bma_action_info_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gboolean ret;
    if (i_paras == NULL || prop_provider == NULL || count == NULL) {
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    ret = redfish_check_manager_uri_valid(i_paras->uri);
    if (ret != TRUE) {
        debug_log(DLOG_ERROR, "%s: URI check failed.", __FUNCTION__);
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_manager_sms_action_provider;
    *count = G_N_ELEMENTS(g_manager_sms_action_provider);

    return VOS_OK;
}


gint32 sms_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gboolean ret;
    guchar board_type = 0;

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    ret = redfish_get_board_type(&board_type);
    return_val_if_expr(VOS_OK != ret, HTTP_NOT_FOUND);
    return_val_if_expr(BOARD_SWITCH == board_type, HTTP_NOT_FOUND);

    ret = redfish_check_manager_uri_valid(i_paras->uri);
    if (!ret) {
        debug_log(DLOG_ERROR, "%s %d redfish_check_manager_uri_valid %s", __FUNCTION__, __LINE__, i_paras->uri);

        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_manager_sms_provider;
    *count = sizeof(g_manager_sms_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}
