
#include "redfish_provider.h"

LOCAL void parse_sensor_single_number(gboolean reading_seccess, json_object *obj_json, gchar *value_str, gdouble value);
LOCAL void parse_sensor_single_string(gboolean reading_seccess, json_object *obj_json, gchar *value_str,
    gchar *str_value);
LOCAL gint32 get_chassis_thermal_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 get_thermal_oem_huawei(PROVIDER_PARAS_S *i_paras, json_object **huawei_json);
LOCAL gint32 get_total_fan_power_consumption(PROVIDER_PARAS_S *i_paras, gdouble *totol_power);


LOCAL gint32 get_thermal_temperatures(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_thermal_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 get_thermal_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 set_thermal_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 set_fan_speed_mode(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso,
    json_object *val_json, json_object *obj_json_mode, OBJ_HANDLE obj_handle);
LOCAL gint32 set_fan_level_rpm(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso,
    json_object *obj_json_mode, OBJ_HANDLE obj_handle);
LOCAL gint32 set_manual_mode_timeout(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso,
    json_object *obj_json_mode, OBJ_HANDLE obj_handle);
LOCAL gint32 get_chassis_thermal_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 get_inlet_history_temperature_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_inlet_history_temperature_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_inlet_history_temperature(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);


LOCAL gint32 get_clear_history_temp_actioninfo_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL void _fan_mode_env_temp_range(json_object *env_tem_obj);
LOCAL void _fan_mode_speed_custom_fanspeedrange(OBJ_HANDLE obj_handle, const gchar *speed_range_prop,
    json_object *env_tem_obj);
LOCAL void get_cooling_range_value(OBJ_HANDLE obj_handle, const gchar *range_prop_name, guint16 *min_val,
    guint16 *max_val, gchar *min_string, guint32 min_len, gchar *max_string, guint32 max_len);


LOCAL gint32 act_thermal_clear_historyrecord(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE handle = 0;

    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != i_paras) && (NULL != i_paras->val_jso),
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: Entry check error.", __FUNCTION__));

    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = check_enclosure_component_type(i_paras->obj_handle, FALSE);
    if (ret != RET_OK || dal_match_product_id(PRODUCT_ID_PANGEA_V6)) {
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso,
            RFPROP_THERMAL_OEM_ACTION_CLEAR_HISTORY_RECORD);
        return HTTP_NOT_FOUND;
    }
    

    ret = dal_get_object_handle_nth(CLASS_ENV_RECORD, 0, &handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, debug_log(DLOG_ERROR, "%s:get env handle error", __FUNCTION__));

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, handle,
        CLASS_ENV_RECORD, METHOD_ENV_RECORD_CLEAR_HISTORY, AUTH_ENABLE, i_paras->user_role_privilege, NULL, NULL);
    
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s, %d, Clear Thermal_history_temperature is failed!", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL PROPERTY_PROVIDER_S g_thermal_provider[] = {
    
    {RFPROP_COMMON_OEM, COOLINGCLASS, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_thermal_oem, set_thermal_oem, NULL, ETAG_FLAG_ENABLE},
    
    {RFPROP_SENSOR_TEMPERATURES, CLASS_THRESHOLD_SENSOR, PROPERTY_THR_SENSOR_SENSOR_TYPE, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_thermal_temperatures, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SENSOR_FANS, CLASS_THRESHOLD_SENSOR, PROPERTY_THR_SENSOR_SENSOR_TYPE, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_thermal_fans, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SENSOR_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_thermal_odataid, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_thermal_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SYSTEM_SUMMARY_STATUS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_thermal_status, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_THERMAL_OEM_ACTION_CLEAR_HISTORY_RECORD, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID, NULL, NULL, act_thermal_clear_historyrecord, ETAG_FLAG_ENABLE}
};


LOCAL PROPERTY_PROVIDER_S g_clear_history_temp_actioninfo_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_clear_history_temp_actioninfo_odataid, NULL, NULL, ETAG_FLAG_ENABLE}
};



LOCAL gint32 get_clear_history_temp_actioninfo_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar odataid_str[ARRAY_LENTH] = {0};
    gchar slot_str[MAX_RSC_ID_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: input param is NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    // 获取 @odata.id
    ret = rf_gen_chassis_component_id(NULL, i_paras->obj_handle, slot_str, MAX_RSC_ID_LEN);
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    if (check_pangea_node_to_enc(i_paras->obj_handle)) {
        ret = snprintf_s(odataid_str, sizeof(odataid_str), sizeof(odataid_str) - 1,
            URI_RF_THERMAL_OEM_ACTION_CLEAR_ACTION_INFO, "Enc");
    } else {
        ret = snprintf_s(odataid_str, sizeof(odataid_str), sizeof(odataid_str) - 1,
            URI_RF_THERMAL_OEM_ACTION_CLEAR_ACTION_INFO, slot_str);
    }
    if (ret <= 0) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const gchar *)odataid_str);
    return_val_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 _check_smart_custom_setting_mask_support(guchar type)
{
    guint32 setting_mask = G_MAXUINT32;
    gint32 ret;
    static OBJ_HANDLE obj_handle = 0;

    if (obj_handle == 0) {
        ret = dal_get_object_handle_nth(COOLINGCLASS, 0, &obj_handle);
        return_val_if_fail(ret == DFL_OK, ret);
    }

    ret = dal_get_property_value_uint32(obj_handle, COOLING_PROPERTY_CUSTOMSETTING_MASK, &setting_mask);
    return_val_if_fail(ret == DFL_OK, ret);

    if (((setting_mask >> type) & 0x01) == 0) {
        return VOS_ERR;
    } else {
        return VOS_OK;
    }
}


LOCAL void _get_fan_healthrollup(gint32 *fan_health, gint32 *obj_count)
{
    gint32 ret;
    guint8 component_type = 0;
    guint8 health_state = 0;
    guint8 health_tmp = 0;
    gint32 count = 0;
    guint8 presence = 0;

    GSList *obj_list = NULL;
    GSList *obj_node = NULL;

    if (NULL == fan_health) {
        debug_log(DLOG_ERROR, "%s get input param error.", __FUNCTION__);
        return;
    }

    ret = dfl_get_object_list(CLASS_COMPONENT, &obj_list);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s get CLASS_COMPONENT obj_list fail.", __FUNCTION__));

    
    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_DEVICE_TYPE, &component_type);
        
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_PRESENCE, &presence);
        continue_if_expr(COMPONENT_TYPE_FAN != component_type || presence == 0);

        continue_if_expr(COMPONENT_TYPE_FAN != component_type);

        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_HEALTH, &health_tmp);

        health_state = health_tmp > health_state ? health_tmp : health_state;

        count++;
        if (health_tmp != 0) {
            debug_log(DLOG_ERROR, "Component fan unhealth, state is %d.", health_tmp);
        }
    }

    g_slist_free(obj_list);

    do_val_if_expr(count > 0, (count = count - 1));

    do_if_expr(NULL != obj_count, (*obj_count = count));

    *fan_health = health_state;

    return;
}


LOCAL void _get_tem_healthrollup(gint32 *tem_health)
{
    gint32 ret;
    guint8 sensor_type = 0;
    gint32 health_state = -1;
    guint8 reading_status = 0;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    gchar string_value[ARRAY_LENTH];

    
    ret = dfl_get_object_list(CLASS_THRESHOLD_SENSOR, &obj_list);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s get temperatur obj_list fail.", __FUNCTION__));

    
    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_THR_SENSOR_SENSOR_TYPE, &sensor_type);
        continue_if_fail((VOS_OK == ret) && (TEMPERATURE_TYPE == sensor_type));

        ret =
            dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_THR_SENSOR_HEALTH_STATUS, &reading_status);
        continue_if_fail(VOS_OK == ret);

        
        health_state = health_state > (gint32)reading_status ? health_state : (gint32)reading_status;
        if (reading_status != LOGENTRY_STATUS_INFORMATIONAL_CODE) {
            ret = memset_s(string_value, sizeof(string_value), 0, sizeof(string_value));
            if (ret != VOS_OK) {
                continue;
            }
            ret = dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_THR_SENSOR_SENSOR_NAME,
                                                string_value, sizeof(string_value));
            if (ret != VOS_OK) {
                continue;
            }
            debug_log(DLOG_ERROR, "temperature sensor %s unhealth, state is %d.", string_value, reading_status);
        }
    }

    *tem_health = health_state;
    g_slist_free(obj_list);

    return;
}

LOCAL gint32 get_chassis_thermal_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 fan_health = -1;
    gint32 tem_health = -1;
    gint32 healthrollup;
    guint8 tmp;

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    

    _get_tem_healthrollup(&tem_health);
    _get_fan_healthrollup(&fan_health, NULL);

    healthrollup = fan_health > tem_health ? fan_health : tem_health;
    tmp = (guint8)healthrollup;

    (void)get_resource_status_property(NULL, &tmp, NULL, o_result_jso, FALSE);

    return HTTP_OK;
}


LOCAL gint32 get_fan_management_supported(guint8 *fan_num)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    ret = dfl_get_object_handle(OBJ_PRODUCT_COMPONENT, &obj_handle);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get OBJ_PRODUCT_COMPONENT handle fail", __FUNCTION__));

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_COMPOENT_FAN_NUM, fan_num);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get PROPERTY_COMPOENT_PS_NUM fail", __FUNCTION__));

    return VOS_OK;
}


LOCAL void _get_inlet_history_temperature_uri(json_object **huawei_json)
{
    int iRet;
    gint32 ret;
    gchar uri[MAX_URI_LENGTH + 1] = {0};
    gchar slot[MAX_RSC_NAME_LEN + 1] = {0};
    json_object *obj_inlet_history_temperature = NULL;
    json_object *obj_odata_id = NULL;

    
    return_do_info_if_fail(NULL != huawei_json, debug_log(DLOG_ERROR, "%s: input param error.", __FUNCTION__));

    
    ret = redfish_get_board_slot(slot, sizeof(slot) - 1);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s: redfish_get_board_slot fail.", __FUNCTION__));

    
    iRet = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, URI_FORMAT_CHASSIS_INLET_HISTORY_TEMPERATURE, slot);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    obj_odata_id = json_object_new_string((const gchar *)uri);
    obj_inlet_history_temperature = json_object_new_object();
    if (NULL == *huawei_json) {
        *huawei_json = json_object_new_object();
    }

    json_object_object_add(obj_inlet_history_temperature, ODATA_ID, obj_odata_id);
    json_object_object_add(*huawei_json, RFPROP_INLET_HISTORY_TEMPERATURE, obj_inlet_history_temperature);

    return;
}

LOCAL gint32 get_clear_history_record_actions(PROVIDER_PARAS_S *i_paras, json_object **huawei_json)
{
    gint32 ret;
    gchar string_value[ARRAY_LENTH] = {0};
    gchar slot_str[ARRAY_LENTH] = {0};
    json_object *obj_actions = NULL;
    json_object *obj_clear = NULL;
    json_object *obj_target = NULL;
    json_object *obj_action_info = NULL;

    
    if (NULL == *huawei_json) {
        *huawei_json = json_object_new_object();
        return_val_do_info_if_expr(NULL == *huawei_json, VOS_ERR,
            debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__));
    }
    

    
    obj_actions = json_object_new_object();
    return_val_if_expr(NULL == obj_actions, HTTP_INTERNAL_SERVER_ERROR);
    json_object_object_add(*huawei_json, RFPROP_ACTIONS, obj_actions);

    ret = rf_gen_chassis_component_id(NULL, i_paras->obj_handle, slot_str, sizeof(slot_str));
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: Add Action is fail.", __FUNCTION__));

    
    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1, "#%s",
        RFPROP_THERMAL_OEM_ACTION_CLEAR_HISTORY_RECORD);
    return_val_do_info_if_expr(ret < 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: Add Thermal.ClearInletHistoryTemperature is fail.", __FUNCTION__));

    obj_clear = json_object_new_object();
    return_val_do_info_if_expr(NULL == obj_clear, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: New obj_clear object is fail.", __FUNCTION__));
    json_object_object_add(obj_actions, (const gchar *)string_value, obj_clear);

    
    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1,
        URI_RF_THERMAL_OEM_ACTION_CLEAR_TARGET, slot_str);
    return_val_do_info_if_expr(ret < 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: Add target is fail.", __FUNCTION__));

    obj_target = json_object_new_string((const gchar *)string_value);
    return_val_do_info_if_expr(NULL == obj_target, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: New obj_target object is fail.", __FUNCTION__));
    json_object_object_add(obj_clear, RFPROP_TARGET, obj_target);

    
    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1,
        URI_RF_THERMAL_OEM_ACTION_CLEAR_ACTION_INFO, slot_str);
    return_val_do_info_if_expr(ret < 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: Add ActionInfo is fail.", __FUNCTION__));

    obj_action_info = json_object_new_string((const gchar *)string_value);
    return_val_do_info_if_expr(NULL == obj_action_info, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: New obj_action_info object is fail.", __FUNCTION__));
    json_object_object_add(obj_clear, RFPROP_ACTION_INFO, obj_action_info);

    return VOS_OK;
}


LOCAL gint32 get_thermal_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    json_object *huawei_json = NULL;
    guint8 uint8_value = 0;

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    
    
    gint32 ret = get_fan_management_supported(&uint8_value);
    if (VOS_OK == ret && 0 != uint8_value) {
        
        
        ret = get_thermal_oem_huawei(i_paras, &huawei_json);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_DEBUG, "%s, %d: get oem read prop failed.", __FUNCTION__, __LINE__));
        
    } else {
        
        huawei_json = json_object_new_object();
        return_val_do_info_if_fail(NULL != huawei_json, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: new object failed.", __FUNCTION__, __LINE__));
    }
    

    ret = check_enclosure_component_type(i_paras->obj_handle, FALSE);
    if (ret == RET_OK && !dal_match_product_id(PRODUCT_ID_PANGEA_V6)) {
        (void)get_clear_history_record_actions(i_paras, &huawei_json);
        _get_inlet_history_temperature_uri(&huawei_json);
    }

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR, json_object_put(huawei_json);
        debug_log(DLOG_ERROR, "%s, %d: new object failed.", __FUNCTION__, __LINE__));

    
    json_object_object_add(*o_result_jso, RFPROP_COMMON_HUAWEI, huawei_json);

    return HTTP_OK;
}

LOCAL void _smart_mode_num_to_str(guchar fan_smart_mode, json_object **huawei_json)
{
    switch (fan_smart_mode) {
        case COOLING_ENERGY_SAVING_MODE:
            json_object_object_add(*huawei_json, RFPROP_SMART_COOLING_MODE,
                json_object_new_string(RF_SMART_COOLING_ENERGY_SAVE));
            break;

        case COOLING_LOW_NOISE_MODE:
            json_object_object_add(*huawei_json, RFPROP_SMART_COOLING_MODE,
                json_object_new_string(RF_SMART_COOLING_LOW_NOISE));
            break;

        case COOLING_HIGH_PERFORMANCE_MODE:
            json_object_object_add(*huawei_json, RFPROP_SMART_COOLING_MODE,
                json_object_new_string(RF_SMART_COOLING_HIGH_PER));
            break;

        case COOLING_USER_DEFINED_MODE:
            json_object_object_add(*huawei_json, RFPROP_SMART_COOLING_MODE,
                json_object_new_string(RF_SMART_COOLING_CUSTOM));
            break;

            
        case COOLING_LIQUID_MODE:
            json_object_object_add(*huawei_json, RFPROP_SMART_COOLING_MODE,
                json_object_new_string(RF_SMART_COOLING_LIQUID));
            break;

            
        default:
            json_object_object_add(*huawei_json, RFPROP_SMART_COOLING_MODE, NULL);
            debug_log(DLOG_DEBUG, "%s:invalid smart cooling  level is %d", __FUNCTION__, fan_smart_mode);
    }

    return;
}

LOCAL void _get_env_fan_speed_range(OBJ_HANDLE obj_handle, json_object **env_tem_obj)
{
    gint32 ret;
    guint32 n;
    guint32 i;
    gsize parm_temp = 0;
    json_object *tmp_obj = NULL;
    json_object *arr_value = NULL;
    GVariant *speed_upper = NULL;
    const guint8 *speed_upper_value = NULL;

    
    ret = dfl_get_property_value(obj_handle, POLICY_PROPERTY_ENV_RANGE_SPEED_HIGH, &speed_upper);
    return_do_info_if_fail(VOS_OK == ret, json_object_object_add(*env_tem_obj, RFPROP_SMART_COOLING_FAN_SPEED, NULL);
        debug_log(DLOG_ERROR, "%s dfl_get_property_value failed.", __FUNCTION__));

    speed_upper_value = (const guint8 *)g_variant_get_fixed_array(speed_upper, &parm_temp, sizeof(guint8));
    n = (guint32)parm_temp;
    return_do_info_if_fail(NULL != speed_upper_value,
        json_object_object_add(*env_tem_obj, RFPROP_SMART_COOLING_FAN_SPEED, NULL);
        g_variant_unref(speed_upper); debug_log(DLOG_ERROR, "%s:g_variant_get_fixed_array fail.", __FUNCTION__));

    tmp_obj = json_object_new_array();
    return_do_info_if_fail(NULL != tmp_obj, json_object_object_add(*env_tem_obj, RFPROP_SMART_COOLING_FAN_SPEED, NULL);
        g_variant_unref(speed_upper); debug_log(DLOG_ERROR, "%s:json_object_new_array fail.", __FUNCTION__));

    for (i = 0; i < n; i++) {
        arr_value = json_object_new_int(speed_upper_value[i]);
        continue_do_info_if_fail(NULL != arr_value,
            debug_log(DLOG_ERROR, "%s json_object_new_int fail.", __FUNCTION__));

        ret = json_object_array_add(tmp_obj, arr_value);
        continue_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s json_object_array_add fail.", __FUNCTION__);
            json_object_put(arr_value); arr_value = NULL);
        arr_value = NULL;
    }

    json_object_object_add(*env_tem_obj, RFPROP_SMART_COOLING_FAN_SPEED, tmp_obj);
    g_variant_unref(speed_upper);
    return;
}

LOCAL void _get_env_temp_range(OBJ_HANDLE obj_handle, json_object **env_tem_obj)
{
    gint32 ret;
    guint32 n;
    guint32 i;
    gsize parm_temp = 0;
    json_object *tmp_obj = NULL;
    json_object *arr_value = NULL;
    GVariant *reading_low = NULL;
    const guint8 *reading_low_value = NULL;

    
    ret = dfl_get_property_value(obj_handle, POLICY_PROPERTY_ENV_RANGE_READING_LOW, &reading_low);
    return_do_info_if_fail(VOS_OK == ret, json_object_object_add(*env_tem_obj, RFPROP_SMART_COOLING_TEM_READ, NULL);
        debug_log(DLOG_ERROR, "%s dfl_get_property_value failed.", __FUNCTION__));

    reading_low_value = (const guint8 *)g_variant_get_fixed_array(reading_low, &parm_temp, sizeof(guint8));
    n = (guint32)parm_temp;
    return_do_info_if_fail(NULL != reading_low_value,
        json_object_object_add(*env_tem_obj, RFPROP_SMART_COOLING_TEM_READ, NULL);
        g_variant_unref(reading_low); debug_log(DLOG_ERROR, "%s:g_variant_get_fixed_array fail.", __FUNCTION__));

    tmp_obj = json_object_new_array();
    return_do_info_if_fail(NULL != tmp_obj, json_object_object_add(*env_tem_obj, RFPROP_SMART_COOLING_TEM_READ, NULL);
        g_variant_unref(reading_low); debug_log(DLOG_ERROR, "%s:json_object_new_object fail.", __FUNCTION__));

    for (i = 1; i < n; i++) {
        arr_value = json_object_new_int(reading_low_value[i]);
        continue_do_info_if_fail(NULL != arr_value,
            debug_log(DLOG_ERROR, "%s json_object_new_int fail.", __FUNCTION__));

        ret = json_object_array_add(tmp_obj, arr_value);
        continue_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s json_object_array_add fail.", __FUNCTION__);
            json_object_put(arr_value); arr_value = NULL);
        arr_value = NULL;
    }

    json_object_object_add(*env_tem_obj, RFPROP_SMART_COOLING_TEM_READ, tmp_obj);
    g_variant_unref(reading_low);
    return;
}


LOCAL void _fan_mode_env_temp_speed(OBJ_HANDLE obj_handle, guint8 cooling_com, GSList *policy_list,
    json_object *fan_speed_custom)
{
    gint32 ret;
    guint16 index_value = 0;
    GSList *obj_node = NULL;
    json_object *env_tem_obj = NULL;
    CUSTOM_ENV_COOLING_INFO env_list[] = {
        {COOLING_CUSTOMMODE_INLETTEMP, INLET_OBJ_INDEX, COOLING_PROPERTY_CUSTOM_INLET_SPEED_RANGE, RFPROP_SMART_COOLING_INLET_SPEED},
        {COOLING_CUSTOMMODE_DISK_ENV, DISK_OBJ_INDEX, COOLING_PROPERTY_CUSTOM_DISK_SPEED_RANGE, RFPROP_CUSTOM_DISK_ENV}
    };
    guint32 list_len = G_N_ELEMENTS(env_list);
    guint32 i;

    for (i = 0; i < list_len; i++) {
        if (cooling_com == env_list[i].component_bit) {
            break;
        }
    }

    // 未找到部件信息
    if (i == list_len) {
        return;
    }

    if (_check_smart_custom_setting_mask_support(cooling_com) != VOS_OK) {
        goto EXIT_ADD_TO_CONTAINER;
    }

    env_tem_obj = json_object_new_object();
    if (env_tem_obj == NULL) {
        goto EXIT_ADD_TO_CONTAINER;
    }

    for (obj_node = policy_list; obj_node; obj_node = obj_node->next) {
        (void)dal_get_property_value_uint16((OBJ_HANDLE)obj_node->data, POLICY_PROPERTY_INDEX, &index_value);

        if (index_value == env_list[i].policy_index) {
            _get_env_temp_range((OBJ_HANDLE)obj_node->data, &env_tem_obj);
            _get_env_fan_speed_range((OBJ_HANDLE)obj_node->data, &env_tem_obj);
            break;
        }
    }

    if (obj_node == NULL) {
        json_object_object_add(env_tem_obj, RFPROP_SMART_COOLING_TEM_READ, NULL);
        json_object_object_add(env_tem_obj, RFPROP_SMART_COOLING_FAN_SPEED, NULL);
    }

    _fan_mode_env_temp_range(env_tem_obj);
    _fan_mode_speed_custom_fanspeedrange(obj_handle, env_list[i].speed_range_prop, env_tem_obj);

EXIT_ADD_TO_CONTAINER:
    ret = json_object_object_add(fan_speed_custom, env_list[i].rf_key_name, env_tem_obj);
    if (ret != 0 && env_tem_obj) {
        json_object_put(env_tem_obj);
    }

    return;
}


LOCAL void _fan_mode_speed_custom_cpurange(OBJ_HANDLE obj_handle, json_object *fan_speed_custom)
{
    gint32 ret = 0;
    guint16 cpu_tem_min = 0;
    guint16 cpu_tem_max = 0;
    guint16 cpu_max_value = 0;
    OBJ_HANDLE me_handle = 0;

    if (VOS_OK == _check_smart_custom_setting_mask_support(COOLING_CUSTOMMODE_CUPTEMP)) {
        (void)dal_get_object_handle_nth(CLASS_ME_INFO, 0, &me_handle);
        (void)dal_get_property_value_uint16(me_handle, PROPERTY_MEINFO_TJMAX, &cpu_max_value);

        
        ret = dal_get_property_value_uint16(obj_handle, COOLING_PROPERTY_CUSTOM_CPU_RANGE_MIN, &cpu_tem_min);
        // tjmax值不在正常范围
        do_info_if_true(CPU_TJMAX_MAX_VAL < cpu_max_value || cpu_max_value < CPU_TJMAX_MIN_VAL,
            (cpu_tem_min = CPU_TOBJ_MIN_VAL));

        if (VOS_OK == ret) {
            json_object_object_add(fan_speed_custom, RFPROP_SMART_COOLING_CPUTEM_MIN, json_object_new_int(cpu_tem_min));
        } else {
            json_object_object_add(fan_speed_custom, RFPROP_SMART_COOLING_CPUTEM_MIN, NULL);
            debug_log(DLOG_ERROR, "%s dal_get_cputempmin_property_value_byte failed.", __FUNCTION__);
        }

        
        ret = dal_get_property_value_uint16(obj_handle, COOLING_PROPERTY_CUSTOM_CPU_RANGE_MAX, &cpu_tem_max);
        // tjmax值不在正常范围
        do_info_if_true(CPU_TJMAX_MAX_VAL < cpu_max_value || cpu_max_value < CPU_TJMAX_MIN_VAL,
            (cpu_tem_max = CPU_TOBJ_MAX_VAL));

        if (VOS_OK == ret) {
            json_object_object_add(fan_speed_custom, RFPROP_SMART_COOLING_CPUTEM_MAX, json_object_new_int(cpu_tem_max));
        } else {
            json_object_object_add(fan_speed_custom, RFPROP_SMART_COOLING_CPUTEM_MAX, NULL);
            debug_log(DLOG_ERROR, "%s dal_get_cputempmax_property_value_byte failed.", __FUNCTION__);
        }
    } else {
        json_object_object_add(fan_speed_custom, RFPROP_SMART_COOLING_CPUTEM_MIN, NULL);
        json_object_object_add(fan_speed_custom, RFPROP_SMART_COOLING_CPUTEM_MAX, NULL);
    }
}


LOCAL void _fan_mode_env_temp_range(json_object *env_tem_obj)
{
    guint16 MinValue = INLETTEM_VALUE_MIN;
    guint16 MaxValue = INLETTEM_VALUE_MAX;

    json_object_object_add(env_tem_obj, RFPROP_SMART_COOLING_INLETTEM_MIN, json_object_new_int(MinValue));
    json_object_object_add(env_tem_obj, RFPROP_SMART_COOLING_INLETTEM_MAX, json_object_new_int(MaxValue));

    return;
}

LOCAL void _fan_mode_speed_custom_fanspeedrange(OBJ_HANDLE obj_handle, const gchar *speed_range_prop,
    json_object *env_tem_obj)
{
    gint32 ret;
    guint16 fan_speed_min = 0;
    guint16 fan_speed_max = 0;
    gsize number = 0;
    GVariant *range = NULL;
    const guint16 *range_list = NULL;

    ret = dfl_get_property_value(obj_handle, speed_range_prop, &range);
    if (ret == DFL_OK) {
        range_list = (const guint16 *)g_variant_get_fixed_array(range, &number, sizeof(guint16));
        if (range_list != NULL && number >= RANGE_PROP_VALUE_NUM) {
            fan_speed_min = range_list[0];
            fan_speed_max = range_list[1];

            json_object_object_add(env_tem_obj, FAN_SPEED_MIN, json_object_new_int(fan_speed_min));
            json_object_object_add(env_tem_obj, FAN_SPEED_MAX, json_object_new_int(fan_speed_max));
        } else {
            json_object_object_add(env_tem_obj, FAN_SPEED_MIN, NULL);
            json_object_object_add(env_tem_obj, FAN_SPEED_MAX, NULL);
        }
        g_variant_unref(range);
    } else {
        json_object_object_add(env_tem_obj, FAN_SPEED_MIN, NULL);
        json_object_object_add(env_tem_obj, FAN_SPEED_MAX, NULL);
    }

    return;
}


LOCAL void _get_component_tobj(OBJ_HANDLE obj_handle, guint8 component_bit, json_object *obj_container)
{
    CUSTOM_TOBJ_INFO    component_list[] = {
        {COOLING_CUSTOMMODE_CUPTEMP, COOLING_PROPERTY_CUSTOMCPU_COREMTOBJ, RFPROP_SMART_COOLING_COREM},
        {COOLING_CUSTOMMODE_OUTTEMP, COOLING_PROPERTY_CUSTOMOUTLET_TOBJ, RFPROP_SMART_COOLING_OUTLET},
        {COOLING_CUSTOMMODE_DISK_TOBJ, COOLING_PROPERTY_CUSTOMDISK_TOBJ, RFPROP_CUSTOM_DISK_TOBJ},
        {COOLING_CUSTOMMODE_MEMORY_TOBJ, COOLING_PROPERTY_CUSTOMMEMORY_TOBJ, RFPROP_CUSTOM_MEMORY_TOBJ},
        {COOLING_CUSTOMMODE_PCH_TOBJ, COOLING_PROPERTY_CUSTOMPCH_TOBJ, RFPROP_CUSTOM_PCH_TOBJ},
        {COOLING_CUSTOMMODE_VRD_TOBJ, COOLING_PROPERTY_CUSTOMVRD_TOBJ, RFPROP_CUSTOM_VRD_TOBJ},
        {COOLING_CUSTOMMODE_VDDQ_TOBJ, COOLING_PROPERTY_CUSTOMVDDQ_TOBJ, RFPROP_CUSTOM_VDDQ_TOBJ},
        {COOLING_CUSTOMMODE_NPU_HBM_TOBJ, COOLING_PROPERTY_CUSTOMNPU_HBM_TOBJ, RFPROP_CUSTOM_NPU_HBM_TOBJ},
        {COOLING_CUSTOMMODE_NPU_AICORE_TOBJ, COOLING_PROPERTY_CUSTOMNPU_AICORE_TOBJ, RFPROP_CUSTOM_NPU_AICORE_TOBJ},
        {COOLING_CUSTOMMODE_NPU_BOARD_TOBJ, COOLING_PROPERTY_CUSTOMNPU_BOARD_TOBJ, RFPROP_CUSTOM_NPU_BOARD_TOBJ}
    };
    guint32 list_len = G_N_ELEMENTS(component_list);
    guint32 i;
    gint32 ret_val;
    json_object *tobj_jso = NULL;
    guint8 tobj_int;

    for (i = 0; i < list_len; i++) {
        if (component_bit == component_list[i].component_bit) {
            break;
        }
    }

    // 未找到部件信息
    if (i == list_len) {
        return;
    }

    ret_val = _check_smart_custom_setting_mask_support(component_bit);
    if (ret_val != VOS_OK) {
        goto EXIT_ADD_TO_CONTAINER;
    }

    ret_val = dal_get_property_value_byte(obj_handle, component_list[i].pme_prop_name, &tobj_int);
    if (ret_val != DFL_OK) {
        goto EXIT_ADD_TO_CONTAINER;
    }
    tobj_jso = json_object_new_int(tobj_int);

EXIT_ADD_TO_CONTAINER:
    ret_val = json_object_object_add(obj_container, component_list[i].rf_key_name, tobj_jso);
    if (ret_val != 0 && tobj_jso) {
        json_object_put(tobj_jso);
    }

    return;
}


LOCAL void _get_component_tobj_range(OBJ_HANDLE obj_handle, guint8 component_bit, json_object *obj_container)
{
    CUSTOM_TOBJ_RANGE_INFO component_list[] = {
        {COOLING_CUSTOMMODE_OUTTEMP, COOLING_PROPERTY_CUSTOM_OUTLET_RANGE, RFPROP_SMART_COOLING_OUTLETTEM_MIN, RFPROP_SMART_COOLING_OUTLETTEM_MAX},
        {COOLING_CUSTOMMODE_DISK_TOBJ, COOLING_PROPERTY_CUSTOM_DISK_RANGE, RFPROP_CUSTOM_DISK_TOBJ_MIN, RFPROP_CUSTOM_DISK_TOBJ_MAX},
        {COOLING_CUSTOMMODE_MEMORY_TOBJ, COOLING_PROPERTY_CUSTOM_MEMORY_RANGE, RFPROP_CUSTOM_MEMORY_TOBJ_MIN, RFPROP_CUSTOM_MEMORY_TOBJ_MAX},
        {COOLING_CUSTOMMODE_PCH_TOBJ, COOLING_PROPERTY_CUSTOM_PCH_RANGE, RFPROP_CUSTOM_PCH_TOBJ_MIN, RFPROP_CUSTOM_PCH_TOBJ_MAX},
        {COOLING_CUSTOMMODE_VRD_TOBJ, COOLING_PROPERTY_CUSTOM_VRD_RANGE, RFPROP_CUSTOM_VRD_TOBJ_MIN, RFPROP_CUSTOM_VRD_TOBJ_MAX},
        {COOLING_CUSTOMMODE_VDDQ_TOBJ, COOLING_PROPERTY_CUSTOM_VDDQ_RANGE, RFPROP_CUSTOM_VDDQ_TOBJ_MIN, RFPROP_CUSTOM_VDDQ_TOBJ_MAX},
        {COOLING_CUSTOMMODE_INLETTEMP, COOLING_PROPERTY_CUSTOM_INLET_SPEED_RANGE, FAN_SPEED_MIN, FAN_SPEED_MIN},
        {COOLING_CUSTOMMODE_DISK_ENV, COOLING_PROPERTY_CUSTOM_DISK_SPEED_RANGE, FAN_SPEED_MIN, FAN_SPEED_MIN},
        {COOLING_CUSTOMMODE_NPU_HBM_TOBJ, COOLING_PROPERTY_CUSTOMNPU_HBM_RANGE,
            RFPROP_CUSTOM_NPU_HBM_TOBJ_MIN, RFPROP_CUSTOM_NPU_HBM_TOBJ_MAX},
        {COOLING_CUSTOMMODE_NPU_AICORE_TOBJ, COOLING_PROPERTY_CUSTOMNPU_AICORE_RANGE,
            RFPROP_CUSTOM_NPU_AICORE_TOBJ_MIN, RFPROP_CUSTOM_NPU_AICORE_TOBJ_MAX},
        {COOLING_CUSTOMMODE_NPU_BOARD_TOBJ, COOLING_PROPERTY_CUSTOMNPU_BOARD_RANGE,
            RFPROP_CUSTOM_NPU_BOARD_TOBJ_MIN, RFPROP_CUSTOM_NPU_BOARD_TOBJ_MAX}
    };
    guint32 list_len = G_N_ELEMENTS(component_list);
    guint32 i;
    gint32 ret_val;
    json_object *tobj_min_jso = NULL;
    json_object *tobj_max_jso = NULL;
    guint16             range_list[RANGE_PROP_VALUE_NUM] = {0};
    gsize element_count = 0;

    for (i = 0; i < list_len && (component_bit != component_list[i].component_bit); i++) {
    }

    // 未找到部件信息
    if (i == list_len) {
        return;
    }

    ret_val = _check_smart_custom_setting_mask_support(component_bit);
    if (ret_val != VOS_OK) {
        goto EXIT_ADD_TO_CONTAINER;
    }

    ret_val = dal_get_property_value_uint16v(obj_handle, component_list[i].pme_prop_name, range_list,
        sizeof(range_list), &element_count);
    if (ret_val != VOS_OK || element_count < RANGE_PROP_VALUE_NUM) {
        goto EXIT_ADD_TO_CONTAINER;
    }

    tobj_min_jso = json_object_new_int(range_list[0]);
    tobj_max_jso = json_object_new_int(range_list[1]);

EXIT_ADD_TO_CONTAINER:

    ret_val = json_object_object_add(obj_container, component_list[i].rf_min_name, tobj_min_jso);
    if (ret_val != 0 && tobj_min_jso) {
        json_object_put(tobj_min_jso);
    }

    ret_val = json_object_object_add(obj_container, component_list[i].rf_max_name, tobj_max_jso);
    if (ret_val != 0 && tobj_max_jso) {
        json_object_put(tobj_max_jso);
    }

    return;
}


LOCAL void _fan_mode_speed_custom(OBJ_HANDLE obj_handle, json_object **huawei_json)
{
    json_object *fan_speed_custom = NULL;
    GSList *obj_list = NULL;

    fan_speed_custom = json_object_new_object();
    return_do_info_if_fail(NULL != fan_speed_custom,
        json_object_object_add(*huawei_json, RFPROP_SMART_COOLING_CUSTOM, NULL);
        debug_log(DLOG_ERROR, "%s:json_object_new_object fail.", __FUNCTION__));

    _get_component_tobj(obj_handle, COOLING_CUSTOMMODE_CUPTEMP, fan_speed_custom);
    _fan_mode_speed_custom_cpurange(obj_handle, fan_speed_custom);

    _get_component_tobj(obj_handle, COOLING_CUSTOMMODE_OUTTEMP, fan_speed_custom);
    _get_component_tobj_range(obj_handle, COOLING_CUSTOMMODE_OUTTEMP, fan_speed_custom);

    _get_component_tobj(obj_handle, COOLING_CUSTOMMODE_DISK_TOBJ, fan_speed_custom);
    _get_component_tobj_range(obj_handle, COOLING_CUSTOMMODE_DISK_TOBJ, fan_speed_custom);

    _get_component_tobj(obj_handle, COOLING_CUSTOMMODE_MEMORY_TOBJ, fan_speed_custom);
    _get_component_tobj_range(obj_handle, COOLING_CUSTOMMODE_MEMORY_TOBJ, fan_speed_custom);

    _get_component_tobj(obj_handle, COOLING_CUSTOMMODE_PCH_TOBJ, fan_speed_custom);
    _get_component_tobj_range(obj_handle, COOLING_CUSTOMMODE_PCH_TOBJ, fan_speed_custom);

    _get_component_tobj(obj_handle, COOLING_CUSTOMMODE_VRD_TOBJ, fan_speed_custom);
    _get_component_tobj_range(obj_handle, COOLING_CUSTOMMODE_VRD_TOBJ, fan_speed_custom);

    _get_component_tobj(obj_handle, COOLING_CUSTOMMODE_VDDQ_TOBJ, fan_speed_custom);
    _get_component_tobj_range(obj_handle, COOLING_CUSTOMMODE_VDDQ_TOBJ, fan_speed_custom);

    _get_component_tobj(obj_handle, COOLING_CUSTOMMODE_NPU_HBM_TOBJ, fan_speed_custom);
    _get_component_tobj_range(obj_handle, COOLING_CUSTOMMODE_NPU_HBM_TOBJ, fan_speed_custom);

    _get_component_tobj(obj_handle, COOLING_CUSTOMMODE_NPU_AICORE_TOBJ, fan_speed_custom);
    _get_component_tobj_range(obj_handle, COOLING_CUSTOMMODE_NPU_AICORE_TOBJ, fan_speed_custom);

    _get_component_tobj(obj_handle, COOLING_CUSTOMMODE_NPU_BOARD_TOBJ, fan_speed_custom);
    _get_component_tobj_range(obj_handle, COOLING_CUSTOMMODE_NPU_BOARD_TOBJ, fan_speed_custom);

    (void)dfl_get_object_list(POLICY1_CLASS, &obj_list);

    _fan_mode_env_temp_speed(obj_handle, COOLING_CUSTOMMODE_INLETTEMP, obj_list, fan_speed_custom);
    _fan_mode_env_temp_speed(obj_handle, COOLING_CUSTOMMODE_DISK_ENV, obj_list, fan_speed_custom);

    do_if_expr(obj_list, g_slist_free(obj_list));

    json_object_object_add(*huawei_json, RFPROP_SMART_COOLING_CUSTOM, fan_speed_custom);

    return;
}


LOCAL void _get_temperature_health_group_value(json_object **huawei_json)
{
    gint32 ret;
    guint8 sensor_type = 0;
    guint8 reading_status = 0;
    gint32 health_state = -1;
    guint8 tmp;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    json_object *tem_obj = NULL;
    json_object *status_obj = NULL;
    gint32 count = 0;
    gchar string_value[ARRAY_LENTH];

    
    ret = dfl_get_object_list(CLASS_THRESHOLD_SENSOR, &obj_list);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s get temperatur obj_list fail.", __FUNCTION__));

    
    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_THR_SENSOR_SENSOR_TYPE, &sensor_type);
        continue_if_fail((VOS_OK == ret) && (TEMPERATURE_TYPE == sensor_type));

        ret =
            dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_THR_SENSOR_HEALTH_STATUS, &reading_status);
        continue_if_fail(VOS_OK == ret);

        
        health_state = health_state > (gint32)reading_status ? health_state : (gint32)reading_status;
        count += 1;
        if (reading_status != LOGENTRY_STATUS_INFORMATIONAL_CODE) {
            ret = memset_s(string_value, sizeof(string_value), 0, sizeof(string_value));
            if (ret != VOS_OK) {
                continue;
            }
            ret = dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_THR_SENSOR_SENSOR_NAME,
                                                string_value, sizeof(string_value));
            if (ret != VOS_OK) {
                continue;
            }
            debug_log(DLOG_ERROR, "temperature sensor %s unhealth, state is %d.", string_value, reading_status);
        }
    }

    g_slist_free(obj_list);
    tmp = (guint8)health_state;

    tem_obj = json_object_new_object();
    return_do_info_if_fail(NULL != tem_obj,
        (void)json_object_object_add(*huawei_json, RFPROP_THERMAL_TEMPARATURE_SUMMARY, NULL);
        debug_log(DLOG_ERROR, "%s: new tem_obj failed.", __FUNCTION__));

    (void)json_object_object_add(tem_obj, RFPROP_THERMAL_SENSOR_COUNT, json_object_new_int(count));
    (void)get_resource_status_property(NULL, &tmp, NULL, &status_obj, TRUE);
    (void)json_object_object_add(tem_obj, RFPROP_SYSTEM_SUMMARY_STATUS, status_obj);
    (void)json_object_object_add(*huawei_json, RFPROP_THERMAL_TEMPARATURE_SUMMARY, tem_obj);

    return;
}

LOCAL void _get_fan_health_group_value(json_object **huawei_json)
{
    gint32 fan_health = -1;
    gint32 tem_health = -1;
    gint32 obj_count = 0;
    gint32 healthrollup;
    guint8 tmp;
    json_object *tem_obj = NULL;
    json_object *status_obj = NULL;

    _get_fan_healthrollup(&fan_health, &obj_count);

    healthrollup = fan_health > tem_health ? fan_health : tem_health;
    tmp = (guint8)healthrollup;

    tem_obj = json_object_new_object();
    return_do_info_if_fail(NULL != tem_obj,
        (void)json_object_object_add(*huawei_json, RFPROP_THERMAL_FAN_SUMMARY, NULL);
        debug_log(DLOG_ERROR, "%s: new tem_obj failed.", __FUNCTION__));

    (void)json_object_object_add(tem_obj, RFPROP_THERMAL_SENSOR_COUNT, json_object_new_int(obj_count));
    (void)get_resource_status_property(NULL, &tmp, NULL, &status_obj, TRUE);
    (void)json_object_object_add(tem_obj, RFPROP_SYSTEM_SUMMARY_STATUS, status_obj);
    (void)json_object_object_add(*huawei_json, RFPROP_THERMAL_FAN_SUMMARY, tem_obj);

    return;
}


LOCAL void _get_cooling_medium(OBJ_HANDLE obj_handle, json_object **huawei_json)
{
    guint8 medium = G_MAXUINT8;
    const gchar *medium_str = NULL;

    (void)dal_get_property_value_byte(obj_handle, COOLING_PROPERTY_MEDIUM, &medium);

    if (COOLING_MEDIUM_AIR == medium) {
        medium_str = RFPROP_VALUE_AIRCOOLED;
    } else if (COOLING_MEDIUM_LIQUID == medium) {
        medium_str = RFPROP_VALUE_LIQUIDCOOLED;
    }

    json_object_object_add(*huawei_json, RFPROP_COOLING_MEDIUM,
        (medium_str == NULL) ? NULL : json_object_new_string(medium_str));

    return;
}


LOCAL gint32 get_thermal_oem_huawei(PROVIDER_PARAS_S *i_paras, json_object **huawei_json)
{
    OBJ_HANDLE obj_handle;
    guchar out_val = 0;
    guint32 time_out = 0;
    gdouble totol_power = 0.0;
    gchar fan_mode_string[ARRAY_LENTH] = {0};

    OBJ_HANDLE event_handle = 0;
    guint8 event_detail_custom_id = RF_RSC_CUSTOM_ID_DEDAULT;

    
    return_val_do_info_if_fail(VOS_OK == provider_paras_check(i_paras), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%d: NULL pointer.", __LINE__));

    
    
    gint32 ret = dfl_get_object_handle(COOLINGCLASS, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%d: get object handle failed.", __LINE__));
    

    
    *huawei_json = json_object_new_object();
    return_val_do_info_if_fail(NULL != *huawei_json, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%d: new object failed.", __LINE__));

    
    ret = dal_get_property_value_byte(obj_handle, COOLING_PROPERTY_MODE, &out_val);
    int iRet;
    if (RFPROP_COOLING_MANUAL == out_val) {
        // sizeof(fan_mode_string)大小为128，大于格式化字符串长度，无需判断返回值
        iRet = snprintf_s(fan_mode_string, sizeof(fan_mode_string), sizeof(fan_mode_string) - 1, "%s",
            RFPROP_COOLING_MODE_MANUAL);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    } else {
        // sizeof(fan_mode_string)大小为128，大于格式化字符串长度，无需判断返回值
        iRet = snprintf_s(fan_mode_string, sizeof(fan_mode_string), sizeof(fan_mode_string) - 1, "%s",
            RFPROP_COOLING_MODE_AUTOMATIC);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    }

    parse_sensor_single_string(VOS_OK == ret, *huawei_json, RFPROP_COOLING_MODE, fan_mode_string);
    
    ret = dal_get_property_value_byte(obj_handle, COOLING_PROPERTY_LEVEL, &out_val);
    parse_sensor_single_number(VOS_OK == ret, *huawei_json, RFPROP_COOLING_LEVEL, out_val);
    
    ret = dal_get_property_value_uint32(obj_handle, COOLING_PROPERTY_TIMEOUT, &time_out);
    parse_sensor_single_number(VOS_OK == ret, *huawei_json, RFPROP_COOLING_TIMEOUT, time_out);
    
    (void)dal_get_property_value_byte(obj_handle, COOLING_PROPERTY_DFTLEVEL, &out_val);
    json_object_object_add(*huawei_json, RFPROP_COOLING_MINIMAL_LEVEL,
        (out_val <= FANSPD_MAX_VAL) ? json_object_new_int(out_val) : NULL);
    json_object_object_add(*huawei_json, RFPROP_COOLING_MAXIMUM_LEVEL,
        json_object_new_int(FANSPD_MAX_VAL)); // 最大转速属性 MaxLimitLevel 配置不合理，不能使用，写死为100

    
    ret = get_total_fan_power_consumption(i_paras, &totol_power);
    parse_sensor_single_number((VOS_OK == ret && 0 != totol_power), *huawei_json, RFPROP_COOLING_FAN_POWER,
        totol_power);

    // 获取温度传感器健康状态的聚合值
    _get_temperature_health_group_value(huawei_json);
    // 复用事件上报的CMCC定制化属性更新FanSummary的显示 CMCC定制此处不显示FanSummary信息
    (void)dal_get_object_handle_nth(CLASS_RF_EVT_SVC, 0, &event_handle);
    (void)dal_get_property_value_byte(event_handle, PROPERTY_RF_EVT_SVC_EVENT_DETAIL_CUSTOMIZED_ID,
        &event_detail_custom_id);
    // 移动定制不显示，联通和不定制场景下显示FanSummary
    if (event_detail_custom_id != RF_RSC_CUSTOM_ID_CMCC) {
        _get_fan_health_group_value(huawei_json);
    }
    _get_cooling_medium(obj_handle, huawei_json);
    
    
    ret = dal_get_property_value_byte(obj_handle, COOLING_PROPERTY_SMARTCOOLINGENABLE, &out_val);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_OK,
        json_object_object_add(*huawei_json, RFPROP_SMART_COOLING_MODE, NULL);
        json_object_object_add(*huawei_json, RFPROP_SMART_COOLING_CUSTOM, NULL);
        debug_log(DLOG_ERROR, "dal_get_property_value_byte fail, ret is %d.", ret));
    return_val_do_info_if_fail(0x1 == out_val, VOS_OK,
        json_object_object_add(*huawei_json, RFPROP_SMART_COOLING_MODE, NULL);
        json_object_object_add(*huawei_json, RFPROP_SMART_COOLING_CUSTOM, NULL));
    

    
    ret = dal_get_property_value_byte(obj_handle, COOLING_THERMALLEVEL, &out_val);
    if (VOS_OK == ret) {
        _smart_mode_num_to_str(out_val, huawei_json);
    } else {
        json_object_object_add(*huawei_json, RFPROP_SMART_COOLING_MODE, NULL);
        debug_log(DLOG_ERROR, "get_thermal_oem: get %s fail, ret: %d", COOLING_THERMALLEVEL, ret);
    }

    
    _fan_mode_speed_custom(obj_handle, huawei_json);
    

    return VOS_OK;
}

LOCAL gint32 get_total_fan_power_consumption(PROVIDER_PARAS_S *i_paras, gdouble *totol_power)
{
    gint32 ret;
    OBJ_HANDLE obj_handle;
    GSList *output_list = NULL;

    
    return_val_do_info_if_fail(VOS_OK == provider_paras_check(i_paras), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    ret = dfl_get_object_handle(COOLINGCLASS, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:get cooling class obj_handle failed.", __FUNCTION__, __LINE__));

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        COOLINGCLASS, METHOD_COOLING_GET_FAN_POWER, AUTH_ENABLE, i_paras->user_role_privilege, NULL, &output_list);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "get fan totol power failed, ret is %d", ret));

    *totol_power = g_variant_get_double((GVariant *)g_slist_nth_data(output_list, 0));
    uip_free_gvariant_list(output_list);

    return VOS_OK;
}

LOCAL void get_fan_part_num(OBJ_HANDLE obj_handle, json_object **fan_obj)
{
    gint32 ret;
    gchar part_number[ARRAY_LENTH] = {0};
    json_object *json_body = NULL;

    ret = dal_get_property_value_string(obj_handle, FAN_PROPERTY_PART_NUM, part_number, sizeof(part_number));

    
    return_do_info_if_fail(VOS_OK == ret && VOS_OK == check_string_val_effective((const gchar *)part_number),
        json_object_object_add(*fan_obj, RFPROP_PART_NUMBER, NULL);
        debug_log(DLOG_DEBUG, "%s, %d:dal_get_property_value_string failed.", __FUNCTION__, __LINE__));
    

    json_body = json_object_new_string((const gchar *)part_number);
    return_do_info_if_fail(NULL != json_body, json_object_object_add(*fan_obj, RFPROP_PART_NUMBER, NULL);
        debug_log(DLOG_ERROR, "%s, %d:json_object_new_string failed.", __FUNCTION__, __LINE__));

    json_object_object_add(*fan_obj, RFPROP_PART_NUMBER, json_body);
    return;
}


LOCAL void get_fan_speed_percent(OBJ_HANDLE obj_handle, json_object *huawei_jso, gchar *fan_f_r)
{
    json_object *speed_percent = NULL;
    gchar fan_mode[PROP_VAL_LENGTH] = {0};
    OBJ_HANDLE fan_type_obj = 0;
    gint32 ret;
    const gchar *max_speed_prop = NULL;
    const gchar *current_speed_prop = NULL;
    guint32 max_fan_speed = 0;
    guint16 current_fan_speed = 0;
    guint8 fan_speed_percent;

    if (dal_match_product_id(PRODUCT_ID_PANGEA_V6)) {
        debug_log(DLOG_DEBUG, "%s: Current board cannot support fan speed percent.", __FUNCTION__);
        return;
    }

    if (0 == g_strcmp0(fan_f_r, "F")) {
        max_speed_prop = FANTYPE_F_SPEED;
        current_speed_prop = FAN_PROPERTY_FRPM;
    } else if (0 == g_strcmp0(fan_f_r, "R")) {
        max_speed_prop = FANTYPE_R_SPEED;
        current_speed_prop = FAN_PROPERTY_RRPM;
    } else {
        // 单风扇前和后速度是相同的，随便取一个
        max_speed_prop = FANTYPE_F_SPEED;
        current_speed_prop = FAN_PROPERTY_FRPM;
    }

    (void)dal_get_property_value_string(obj_handle, FAN_PROPERTY_MODEL, fan_mode, sizeof(fan_mode));

    // 找到对应型号风扇的对象
    ret = dal_get_specific_object_string(FANTYPECLASS, FANTYPE_CHARACTOR_NAME, (const gchar *)fan_mode, &fan_type_obj);
    goto_label_do_info_if_fail(DFL_OK == ret, exit,
        debug_log(DLOG_DEBUG, "%s: get fan type object failed", __FUNCTION__));

    (void)dal_get_property_value_uint16(obj_handle, current_speed_prop, &current_fan_speed);
    (void)dal_get_property_value_uint32(fan_type_obj, max_speed_prop, &max_fan_speed);

    if (max_fan_speed == 0) {
        debug_log(DLOG_ERROR, "%s: max_fan_speed is 0", __FUNCTION__);
        return;
    }

    fan_speed_percent = current_fan_speed * 100 / max_fan_speed;
    fan_speed_percent = (fan_speed_percent > FANSPD_MAX_VAL) ? FANSPD_MAX_VAL : fan_speed_percent;
    speed_percent = json_object_new_int((gint32)fan_speed_percent);
exit:
    json_object_object_add(huawei_jso, RFPROP_COOLING_SPEED_RATIO, speed_percent);
}


LOCAL void get_fan_slot_id(OBJ_HANDLE obj_handle, json_object *huawei_jso)
{
    guint8 fanslot = 0;

    if (huawei_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL Pointer", __FUNCTION__);
        return;
    }

    gint32 ret = dal_get_property_value_byte(obj_handle, FAN_SLOT, &fanslot);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s : get Fan slotid failed, ret is %d", __FUNCTION__, ret);
        (void)json_object_object_add(huawei_jso, RFPROP_FAN_SLOT_NUM, NULL);
        (void)json_object_object_add(huawei_jso, RFPROP_COOLING_SLOT_ID, NULL);
        return;
    }

    (void)json_object_object_add(huawei_jso, RFPROP_FAN_SLOT_NUM, json_object_new_int((int32_t)fanslot));
    (void)json_object_object_add(huawei_jso, RFPROP_COOLING_SLOT_ID, json_object_new_int(fanslot));
    return;
}

LOCAL gint32 get_fan_oem(OBJ_HANDLE obj_handle, json_object **fan_oem, gchar *fan_f_r, double speed_rpm)
{
    gint32 ret;
    json_object *fan_oem_object = NULL;
    gchar fan_location[ARRAY_LENTH] = {0};

    
    ret = dal_get_property_value_string(obj_handle, FAN_PROPERTY_LOCATION, fan_location, sizeof(fan_location));

    *fan_oem = json_object_new_object();
    return_val_do_info_if_fail(*fan_oem != NULL, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d:new object failed.", __FUNCTION__, __LINE__));
    fan_oem_object = json_object_new_object();
    return_val_do_info_if_fail(fan_oem_object != NULL, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d:new object failed.", __FUNCTION__, __LINE__);
        json_object_put(*fan_oem));

    parse_sensor_single_string(ret == VOS_OK, fan_oem_object, RFPROP_COOLING_POSITION, fan_location);
    
    get_fan_speed_percent(obj_handle, fan_oem_object, fan_f_r);
    

    get_fan_slot_id(obj_handle, fan_oem_object);
    json_object_object_add(*fan_oem, RFPROP_COMMON_HUAWEI, fan_oem_object);
    return VOS_OK;
}


LOCAL gint32 get_env_temp_component_info(guint8 component_bit, CUSTOM_ENV_TEMP_SET_INFO *env_temp_info)
{
    CUSTOM_ENV_TEMP_SET_INFO component_list[] = {
        {COOLING_CUSTOMMODE_INLETTEMP, INLET_OBJ_INDEX, RF_JSON_POINTER_INLET_ENV, RF_JSON_POINTER_INLET_ENV_SPEED, RF_JSON_POINTER_INLET_ENV_TEMP, RF_JSON_POINTER_INLET_SPD_DEL, RF_JSON_POINTER_INLET_TEMP_DEL, COOLING_PROPERTY_CUSTOM_INLET_SPEED_RANGE},
        {COOLING_CUSTOMMODE_DISK_ENV, DISK_OBJ_INDEX, RF_JSON_POINTER_DISK_ENV, RF_JSON_POINTER_DISK_ENV_SPEED, RF_JSON_POINTER_DISK_ENV_TEMP, RF_JSON_POINTER_DISK_SPD_DEL, RF_JSON_POINTER_DISK_TEMP_DEL, COOLING_PROPERTY_CUSTOM_DISK_SPEED_RANGE}
    };
    guint32 env_list_len = G_N_ELEMENTS(component_list);
    guint32 i;
    errno_t errno_ret;

    for (i = 0; i < env_list_len; i++) {
        if (component_bit == component_list[i].component_bit) {
            break;
        }
    }

    if (i == env_list_len) {
        return VOS_ERR;
    }

    errno_ret = memcpy_s(env_temp_info, sizeof(CUSTOM_ENV_TEMP_SET_INFO), &(component_list[i]),
        sizeof(CUSTOM_ENV_TEMP_SET_INFO));
    if (errno_ret != EOK) {
        return VOS_ERR;
    }

    return VOS_OK;
}


LOCAL gint32 fill_env_temp_set_input_list(json_object *obj_json_mode, CUSTOM_ENV_TEMP_SET_INFO *env_temp_info,
    GSList **input_list, json_object **o_message_jso)
{
    json_object *temp_array = NULL;
    json_object *speed_array = NULL;
    json_object *element_obj_jso = NULL;
    json_object *msg_jso = NULL;
    const gchar *smart_cool_fan_err = env_temp_info->json_p_speed;
    const gchar *smart_cool_tem_err = env_temp_info->json_p_temp;
    size_t temp_count, speed_count;
    size_t i, j;
    guint8          range_prop[RANGE_PROP_VALUE_NUM][ARRAY_MAXSIZE] = {0};
    OBJ_HANDLE obj_handle = 0;
    guint16 min_speed_value = 0;
    guint16 max_speed_value = 0;
    gchar           int_val_buf[RANGE_PROP_VALUE_NUM][PROP_VAL_LENGTH] = {0};

    
    json_bool ret_bool = json_object_object_get_ex(obj_json_mode, RFPROP_SMART_COOLING_TEM_READ, &temp_array);
    return_val_do_info_if_fail(ret_bool == TRUE, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_MISSING, smart_cool_tem_err, &msg_jso, smart_cool_tem_err);
        (void)json_object_array_add(*o_message_jso, msg_jso));

    
    temp_count = json_object_array_length(temp_array);
    return_val_do_info_if_fail((temp_count <= ARRAY_MAXSIZE - 1) && (temp_count >= INLET_LEVEL_MIN), HTTP_BAD_REQUEST,
        debug_log(DLOG_DEBUG, "%s: invalid length:%zu", __FUNCTION__, temp_count);
        (void)create_message_info(MSGID_ARRAY_ITEM_EXCEED, smart_cool_tem_err, &msg_jso, smart_cool_tem_err);
        (void)json_object_array_add(*o_message_jso, msg_jso));

    for (i = 0, j = 0; i < temp_count; i++) {
        element_obj_jso = json_object_array_get_idx(temp_array, i);
        continue_if_fail(element_obj_jso != NULL);

        
        return_val_do_info_if_fail(json_object_get_type(element_obj_jso) == json_type_int, HTTP_BAD_REQUEST,
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, smart_cool_tem_err, &msg_jso,
            dal_json_object_get_str(element_obj_jso), smart_cool_tem_err);
            (void)json_object_array_add(*o_message_jso, msg_jso));

        range_prop[0][j++] = json_object_get_int(element_obj_jso);
    }

    
    
    (void)dal_get_object_handle_nth(COOLINGCLASS, 0, &obj_handle);
    get_cooling_range_value(obj_handle, env_temp_info->pme_range_prop, &min_speed_value, &max_speed_value,
        int_val_buf[0], PROP_VAL_LENGTH, int_val_buf[1], PROP_VAL_LENGTH);
    

    
    ret_bool = json_object_object_get_ex(obj_json_mode, RFPROP_SMART_COOLING_FAN_SPEED, &speed_array);
    return_val_do_info_if_fail(ret_bool == TRUE, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_MISSING, smart_cool_fan_err, &msg_jso, smart_cool_fan_err);
        (void)json_object_array_add(*o_message_jso, msg_jso));

    
    speed_count = json_object_array_length(speed_array);
    return_val_do_info_if_fail((speed_count <= ARRAY_MAXSIZE - 1) && (speed_count > INLET_LEVEL_MIN), HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_ARRAY_ITEM_EXCEED, smart_cool_fan_err, &msg_jso, smart_cool_fan_err);
        (void)json_object_array_add(*o_message_jso, msg_jso));

    for (i = 0, j = 0; i < speed_count; i++) {
        element_obj_jso = json_object_array_get_idx(speed_array, i);
        continue_if_fail(element_obj_jso != NULL);

        
        return_val_do_info_if_fail(json_object_get_type(element_obj_jso) == json_type_int, HTTP_BAD_REQUEST,
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, smart_cool_fan_err, &msg_jso,
            dal_json_object_get_str(element_obj_jso), smart_cool_fan_err);
            (void)json_object_array_add(*o_message_jso, msg_jso));

        range_prop[1][j++] = json_object_get_int(element_obj_jso);

        
        return_val_do_info_if_fail(range_prop[1][j - 1] >= min_speed_value && range_prop[1][j - 1] <= max_speed_value,
            HTTP_BAD_REQUEST,
            (void)create_message_info(MSGID_PROP_VALUE_OUTOFRANGE, smart_cool_fan_err, &msg_jso,
            dal_json_object_get_str(element_obj_jso), smart_cool_fan_err, int_val_buf[0], int_val_buf[1]);
            (void)json_object_array_add(*o_message_jso, msg_jso));
        
    }

    *input_list = g_slist_append(*input_list,
        g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, range_prop[0], temp_count, sizeof(guint8)));
    *input_list = g_slist_append(*input_list,
        g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, range_prop[1], speed_count, sizeof(guint8)));

    return VOS_OK;
}


LOCAL gint32 get_env_temp_object_handle(CUSTOM_ENV_TEMP_SET_INFO *env_temp_info, OBJ_HANDLE *obj_handle,
    json_object **o_message_jso)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    json_object *tmp_err = NULL;
    guint16 index_value = 0;

    
    ret = dfl_get_object_list(POLICY1_CLASS, &obj_list);
    return_val_do_info_if_expr(ret != DFL_OK || obj_list == NULL, HTTP_NOT_IMPLEMENTED,
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, env_temp_info->json_p_obj, &tmp_err,
        env_temp_info->json_p_obj);
        (void)json_object_array_add(*o_message_jso, tmp_err));

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        // 初始化默认对象句柄
        do_if_expr(obj_node == obj_list, (*obj_handle = (OBJ_HANDLE)obj_node->data));
        (void)dal_get_property_value_uint16((OBJ_HANDLE)obj_node->data, POLICY_PROPERTY_INDEX, &index_value);
        break_do_info_if_expr(index_value == env_temp_info->policy_index, (*obj_handle = (OBJ_HANDLE)obj_node->data));
    }
    g_slist_free(obj_list);

    return VOS_OK;
}


LOCAL gint32 set_smart_cooling_env_temp(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object *obj_json_mode, guint8 component_bit)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    json_bool del_flag = FALSE;
    GSList *input_list = NULL;
    json_object *tmp_err = NULL;
    CUSTOM_ENV_TEMP_SET_INFO env_temp_info;

    // 未找到部件信息
    return_val_do_info_if_expr(get_env_temp_component_info(component_bit, &env_temp_info) != VOS_OK,
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &tmp_err);
        (void)json_object_array_add(*o_message_jso, tmp_err));

    
    return_val_do_info_if_fail(g_strcmp0(dal_json_object_get_str(obj_json_mode), JSON_NULL_OBJECT_STR) != 0,
        HTTP_BAD_REQUEST, debug_log(DLOG_DEBUG, "%s:empty object", __FUNCTION__));

    
    (void)check_delete_property_by_class(i_paras->val_jso, env_temp_info.json_p_temp_del, &del_flag);
    return_val_do_info_if_fail(del_flag != TRUE, HTTP_BAD_REQUEST,
        debug_log(DLOG_DEBUG, "%s:temp is delete", __FUNCTION__));

    (void)check_delete_property_by_class(i_paras->val_jso, env_temp_info.json_p_speed_del, &del_flag);
    return_val_do_info_if_fail(del_flag != TRUE, HTTP_BAD_REQUEST,
        debug_log(DLOG_DEBUG, "%s:speed is delete", __FUNCTION__));

    ret = fill_env_temp_set_input_list(obj_json_mode, &env_temp_info, &input_list, o_message_jso);
    return_val_if_expr(ret != VOS_OK, ret);

    ret = get_env_temp_object_handle(&env_temp_info, &obj_handle, o_message_jso);
    return_val_do_info_if_expr(ret != VOS_OK, ret, uip_free_gvariant_list(input_list));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        POLICY1_CLASS, METHOD_COOLING_SET_ENVTEMPSPD, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
            return HTTP_OK;

            
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, env_temp_info.json_p_obj, &tmp_err,
                env_temp_info.json_p_obj);
            (void)json_object_array_add(*o_message_jso, tmp_err);
            return HTTP_FORBIDDEN;

            
        case ERROR_ENVRANGE_NONASC:
            (void)create_message_info(MSGID_CUSTOM_TEM_INVALID, env_temp_info.json_p_temp, &tmp_err,
                env_temp_info.json_p_temp);
            (void)json_object_array_add(*o_message_jso, tmp_err);
            return HTTP_BAD_REQUEST;

            
        case ERROR_SPDARR_NONASC:
            (void)create_message_info(MSGID_CUSTOM_FAN_SPEED_INVALID, env_temp_info.json_p_speed, &tmp_err,
                env_temp_info.json_p_speed);
            (void)json_object_array_add(*o_message_jso, tmp_err);
            return HTTP_BAD_REQUEST;

            
        case ERROR_ENVARRSIZE_INVALID_SPDARRSIZE:
            (void)create_message_info(MSGID_CUSTOM_INVALID, NULL, &tmp_err, env_temp_info.json_p_temp,
                env_temp_info.json_p_speed);
            (void)json_object_array_add(*o_message_jso, tmp_err);
            return HTTP_BAD_REQUEST;

            
        default:
            
            
            (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, env_temp_info.json_p_obj, &tmp_err,
                env_temp_info.json_p_obj);
            (void)json_object_array_add(*o_message_jso, tmp_err);
            return HTTP_NOT_IMPLEMENTED;
            
    }
}

LOCAL gint32 set_smart_cooling_mode_cpu(PROVIDER_PARAS_S *i_paras, json_object *cpu_obj, json_object **o_message_jso)
{
    gint32 ret;
    gint32 cpu_value;
    guint16 max_value = 0;
    guint16 min_value = 0;
    OBJ_HANDLE cooling_handle = 0;
    json_object *message_temp_jso = NULL;
    GSList *input_list = NULL;
    const gchar *smart_cool_err_mes = "Oem/Huawei/FanSpeedCustom/CPUTargetTemperatureCelsius";

    gchar max_value_s[PROP_VAL_LENGTH] = {0};
    gchar min_value_s[PROP_VAL_LENGTH] = {0};

    
    return_val_do_info_if_fail(json_type_int == json_object_get_type(cpu_obj), HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, smart_cool_err_mes, &message_temp_jso,
        dal_json_object_get_str(cpu_obj), smart_cool_err_mes);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    ret = dfl_get_object_handle(COOLINGCLASS, &cooling_handle);
    if (ret != DFL_OK) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    cpu_value = json_object_get_int(cpu_obj);
    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)cpu_value));

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, cooling_handle,
        COOLINGCLASS, METHOD_COOLING_SET_CPU_TOBJ, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
            return HTTP_OK;

            
        case ERROR_UNENABLE_SMARTMODE:

            
        case ERROR_SETTING_UNSUPPORT:
            
            
            (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, smart_cool_err_mes, &message_temp_jso,
                smart_cool_err_mes);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            return HTTP_NOT_IMPLEMENTED;
            

            
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, smart_cool_err_mes, &message_temp_jso,
                smart_cool_err_mes);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            return HTTP_FORBIDDEN;
        case ERROR_INVALID_TOBJ_VALUE:
            (void)dal_get_property_value_uint16(cooling_handle, COOLING_PROPERTY_CUSTOM_CPU_RANGE_MAX, &max_value);
            (void)dal_get_property_value_uint16(cooling_handle, COOLING_PROPERTY_CUSTOM_CPU_RANGE_MIN, &min_value);
            (void)snprintf_s(min_value_s, sizeof(min_value_s), sizeof(min_value_s) - 1, "%u", min_value);
            (void)snprintf_s(max_value_s, sizeof(max_value_s), sizeof(max_value_s) - 1, "%u", max_value);
            (void)create_message_info(MSGID_PROP_VALUE_OUTOFRANGE, smart_cool_err_mes, &message_temp_jso,
                dal_json_object_get_str(cpu_obj), smart_cool_err_mes, (const gchar*)min_value_s,
                (const gchar*)max_value_s);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            return HTTP_BAD_REQUEST;
            
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_temp_jso);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 set_smart_cooling_mode_outlet(PROVIDER_PARAS_S *i_paras, json_object *outlet_obj,
    json_object **o_message_jso)
{
    gint32 ret;
    gint32 outlet_value;
    guint16 min_value = 0;
    guint16 max_value = 0;
    gsize number = 0;

    GVariant *range = NULL;
    const guint16 *range_list = NULL;

    OBJ_HANDLE obj_handle = 0;
    json_object *message_temp_jso = NULL;

    gchar max_value_s[PROP_VAL_LENGTH] = {0};
    gchar min_value_s[PROP_VAL_LENGTH] = {0};

    GSList *input_list = NULL;
    const gchar *smart_cool_err_mes = "Oem/Huawei/FanSpeedCustom/OutletTargetTemperatureCelsius";

    
    return_val_do_info_if_fail(json_type_int == json_object_get_type(outlet_obj), HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, smart_cool_err_mes, &message_temp_jso,
        dal_json_object_get_str(outlet_obj), smart_cool_err_mes);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    outlet_value = json_object_get_int(outlet_obj);

    
    (void)dal_get_object_handle_nth(COOLINGCLASS, 0, &obj_handle);
    ret = dfl_get_property_value(obj_handle, COOLING_PROPERTY_CUSTOM_OUTLET_RANGE, &range);
    if (VOS_OK == ret) {
        range_list = (const guint16 *)g_variant_get_fixed_array(range, &number, sizeof(guint16));
        if (NULL != range_list && number >= 2) {
            min_value = range_list[0];
            max_value = range_list[1];
        }

        g_variant_unref(range);
    }

    // 参数范围不合法
    if (outlet_value < min_value || outlet_value > max_value) {
        (void)snprintf_s(min_value_s, sizeof(min_value_s), sizeof(min_value_s) - 1, "%u", min_value);
        (void)snprintf_s(max_value_s, sizeof(max_value_s), sizeof(max_value_s) - 1, "%u", max_value);
        (void)create_message_info(MSGID_PROP_VALUE_OUTOFRANGE, smart_cool_err_mes, &message_temp_jso,
            dal_json_object_get_str(outlet_obj), smart_cool_err_mes, (const gchar *)min_value_s,
            (const gchar *)max_value_s);
        (void)json_object_array_add(*o_message_jso, message_temp_jso);
        return HTTP_BAD_REQUEST;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)outlet_value));

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        COOLINGCLASS, METHOD_COOLING_SET_OUTTEMP_TOBJ, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
            
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, smart_cool_err_mes, &message_temp_jso,
                smart_cool_err_mes);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            return HTTP_FORBIDDEN;

        case VOS_OK:
            return HTTP_OK;

            
        case ERROR_UNENABLE_SMARTMODE:
            
        case ERROR_SETTING_UNSUPPORT:
            
            
            (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, smart_cool_err_mes, &message_temp_jso,
                smart_cool_err_mes);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            return HTTP_NOT_IMPLEMENTED;
            

            
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_temp_jso);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 set_smart_cooling_mode(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object *obj_json_mode,
    OBJ_HANDLE obj_handle, json_object *val_json)
{
    gint32 ret;
    const gchar *smart_cooling_mode = NULL;
    json_object *message_temp_jso = NULL;
    const gchar *smart_cool_err_mes = "Oem/Huawei/FanSmartCoolingMode";
    GSList *input_list = NULL;
    guchar input_cooling_mode = 0;

    
    return_val_do_info_if_fail(VOS_OK == provider_paras_check(i_paras) && NULL != obj_json_mode && NULL != val_json,
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    smart_cooling_mode = dal_json_object_get_str(obj_json_mode);
    if (0 == g_strcmp0(smart_cooling_mode, RF_SMART_COOLING_ENERGY_SAVE)) {
        input_cooling_mode = COOLING_ENERGY_SAVING_MODE;
    } else if (0 == g_strcmp0(smart_cooling_mode, RF_SMART_COOLING_LOW_NOISE)) {
        input_cooling_mode = COOLING_LOW_NOISE_MODE;
    } else if (0 == g_strcmp0(smart_cooling_mode, RF_SMART_COOLING_HIGH_PER)) {
        input_cooling_mode = COOLING_HIGH_PERFORMANCE_MODE;
    } else if (0 == g_strcmp0(smart_cooling_mode, RF_SMART_COOLING_CUSTOM)) {
        input_cooling_mode = COOLING_USER_DEFINED_MODE;
    }
    
    
    else if (0 == g_strcmp0(smart_cooling_mode, RF_SMART_COOLING_LIQUID)) {
        input_cooling_mode = COOLING_LIQUID_MODE;
    }
    
    else {
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, smart_cool_err_mes, &message_temp_jso, smart_cooling_mode,
            smart_cool_err_mes);
        (void)json_object_array_add(*o_message_jso, message_temp_jso);
        return HTTP_BAD_REQUEST;
    }

    ret = check_input_smart_mode_valid(input_cooling_mode);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, smart_cool_err_mes, &message_temp_jso, smart_cooling_mode,
        smart_cool_err_mes);
        (void)json_object_array_add(*o_message_jso, message_temp_jso););

    input_list = g_slist_append(input_list, g_variant_new_byte(input_cooling_mode));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        COOLINGCLASS, METHOD_SMART_COOLING_MODE, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
            return HTTP_OK;

            
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, smart_cool_err_mes, &message_temp_jso,
                smart_cool_err_mes);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            return HTTP_FORBIDDEN;

            
        case ERROR_UNENABLE_SMARTMODE:
            
            (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, smart_cool_err_mes, &message_temp_jso,
                smart_cool_err_mes);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            return HTTP_NOT_IMPLEMENTED;

            
            
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_temp_jso);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL void get_cooling_range_value(OBJ_HANDLE obj_handle, const gchar *range_prop_name, guint16 *min_val,
    guint16 *max_val, gchar *min_string, guint32 min_len, gchar *max_string, guint32 max_len)
{
    gint32 ret_val;
    gsize element_count;
    guint16     range_list[RANGE_PROP_VALUE_NUM] = {0};

    ret_val =
        dal_get_property_value_uint16v(obj_handle, range_prop_name, range_list, sizeof(range_list), &element_count);
    if (ret_val != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get %s.%s failed, ret = %d", __FUNCTION__, dfl_get_object_name(obj_handle),
            range_prop_name, ret_val);
        return;
    }

    if (element_count < RANGE_PROP_VALUE_NUM) {
        debug_log(DLOG_ERROR, "%s: invalid range count is %" G_GSIZE_FORMAT, __FUNCTION__, element_count);
        return;
    }

    if (min_val) {
        *min_val = range_list[0];
    }
    if (max_val) {
        *max_val = range_list[1];
    }
    ret_val = snprintf_truncated_s(min_string, min_len, "%d", range_list[0]);
    if (ret_val <= 0) {
        debug_log(DLOG_DEBUG, "%s: snprintf_s min value failed", __FUNCTION__);
    }
    ret_val = snprintf_truncated_s(max_string, max_len, "%d", range_list[1]);
    if (ret_val <= 0) {
        debug_log(DLOG_DEBUG, "%s: snprintf_s max value failed", __FUNCTION__);
    }

    return;
}


LOCAL gint32 get_tobj_compoent_info(guint8 component_bit, CUSTOM_TOBJ_SET_INFO *tobj_com_info)
{
    CUSTOM_TOBJ_SET_INFO component_list[] = {
        {COOLING_CUSTOMMODE_DISK_TOBJ, RF_JSON_POINTER_DISK_TOBJ, METHOD_COOLING_SET_DISKTEMP_TOBJ, COOLING_PROPERTY_CUSTOM_DISK_RANGE},
        {COOLING_CUSTOMMODE_MEMORY_TOBJ, RF_JSON_POINTER_MEMORY_TOBJ, METHOD_COOLING_SET_MEMORYTEMP_TOBJ, COOLING_PROPERTY_CUSTOM_MEMORY_RANGE},
        {COOLING_CUSTOMMODE_PCH_TOBJ, RF_JSON_POINTER_PCH_TOBJ, METHOD_COOLING_SET_PCHTEMP_TOBJ, COOLING_PROPERTY_CUSTOM_PCH_RANGE},
        {COOLING_CUSTOMMODE_VRD_TOBJ, RF_JSON_POINTER_VRD_TOBJ, METHOD_COOLING_SET_VRDTEMP_TOBJ, COOLING_PROPERTY_CUSTOM_VRD_RANGE},
        {COOLING_CUSTOMMODE_VDDQ_TOBJ, RF_JSON_POINTER_VDDQ_TOBJ, METHOD_COOLING_SET_VDDQTEMP_TOBJ, COOLING_PROPERTY_CUSTOM_VDDQ_RANGE},
        {COOLING_CUSTOMMODE_NPU_HBM_TOBJ, RF_JSON_POINTER_NPU_HBM_TOBJ,
         METHOD_COOLING_SET_NPU_HBMTEMP_TOBJ, COOLING_PROPERTY_CUSTOMNPU_HBM_RANGE},
        {COOLING_CUSTOMMODE_NPU_AICORE_TOBJ, RF_JSON_POINTER_NPU_AICORE_TOBJ,
         METHOD_COOLING_SET_NPU_AICORETEMP_TOBJ, COOLING_PROPERTY_CUSTOMNPU_AICORE_RANGE},
        {COOLING_CUSTOMMODE_NPU_BOARD_TOBJ, RF_JSON_POINTER_NPU_BOARD_TOBJ,
         METHOD_COOLING_SET_NPU_BOARDTEMP_TOBJ, COOLING_PROPERTY_CUSTOMNPU_BOARD_RANGE}
    };
    guint32 list_len = G_N_ELEMENTS(component_list);
    guint32 i;
    errno_t errno_ret;

    for (i = 0; i < list_len; i++) {
        if (component_bit == component_list[i].component_bit) {
            break;
        }
    }

    if (i == list_len) {
        return VOS_ERR;
    }

    errno_ret =
        memcpy_s(tobj_com_info, sizeof(CUSTOM_TOBJ_SET_INFO), &(component_list[i]), sizeof(CUSTOM_TOBJ_SET_INFO));
    if (errno_ret != EOK) {
        return VOS_ERR;
    }

    return VOS_OK;
}


LOCAL gint32 create_set_tobj_message(gint32 ret, json_object **o_message_jso, const gchar *json_p,
    const gchar *json_val_str, OBJ_HANDLE obj_handle, const gchar *range_prop)
{
    json_object *message_temp_jso = NULL;
    gchar       range_val[RANGE_PROP_VALUE_NUM][PROP_VAL_LENGTH] = {0};

    switch (ret) {
        case VOS_OK:
            return HTTP_OK;

        
        case ERROR_UNENABLE_SMARTMODE:
        case ERROR_SETTING_UNSUPPORT:
            (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, json_p, &message_temp_jso, json_p);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            return HTTP_NOT_IMPLEMENTED;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, json_p, &message_temp_jso, json_p);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            return HTTP_FORBIDDEN;

        case RF_PROP_TYPE_ERR:
            (void)create_message_info(MSGID_PROP_TYPE_ERR, json_p, &message_temp_jso, json_val_str, json_p);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            return HTTP_BAD_REQUEST;

        // 设置的值不在范围内
        case ERROR_INVALID_TOBJ_VALUE:
            get_cooling_range_value(obj_handle, range_prop, NULL, NULL, range_val[0], PROP_VAL_LENGTH, range_val[1],
                PROP_VAL_LENGTH);
            (void)create_message_info(MSGID_PROP_VALUE_OUTOFRANGE, json_p, &message_temp_jso, json_val_str, json_p,
                range_val[0], range_val[1]);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            return HTTP_BAD_REQUEST;

        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_temp_jso);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 set_smart_cooling_tobj(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object *json_prop,
    guint8 component_bit)
{
    CUSTOM_TOBJ_SET_INFO tobj_com_info;
    gint32 ret, json_int_val;
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    json_object *message_temp_jso = NULL;
    const gchar *json_p = NULL;
    const gchar *json_val_str = NULL;

    // 未找到部件信息
    return_val_do_info_if_expr(get_tobj_compoent_info(component_bit, &tobj_com_info) != VOS_OK,
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    json_p = tobj_com_info.json_pointer_name;
    json_val_str = (json_prop == NULL) ? RF_VALUE_NULL : dal_json_object_get_str(json_prop);

    goto_label_do_info_if_expr(json_object_get_type(json_prop) != json_type_int, EXIT_CREATE_MESSAGE,
        ret = RF_PROP_TYPE_ERR;);

    ret = dal_get_object_handle_nth(COOLINGCLASS, 0, &obj_handle);
    goto_label_do_info_if_expr(ret != DFL_OK, EXIT_CREATE_MESSAGE, ret = ERROR_SETTING_UNSUPPORT;);

    json_int_val = json_object_get_int(json_prop);
    goto_label_do_info_if_expr(json_int_val < 0 || json_int_val > G_MAXUINT8, EXIT_CREATE_MESSAGE,
        ret = ERROR_INVALID_TOBJ_VALUE;);

    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)json_int_val));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        COOLINGCLASS, tobj_com_info.pme_method_name, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    do_if_expr(ret != VOS_OK, debug_log(DLOG_ERROR, "%s: call %s.%s failed, ret = %d", __FUNCTION__,
        dfl_get_object_name(obj_handle), tobj_com_info.pme_method_name, ret));
EXIT_CREATE_MESSAGE:

    return create_set_tobj_message(ret, o_message_jso, json_p, json_val_str, obj_handle, tobj_com_info.pme_range_prop);
}


LOCAL gint32 set_smart_cooling_mode_custom(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object *obj_json_mode, json_object *val_json)
{
    gint32 ret = 0;
    json_object *message_temp_jso = NULL;
    json_object *json_prop = NULL;
    gint32 return_array[NUMBER_THERMAL_OEM] = {0};
    gint32 arr_subscript = 0;
    guint32 cnt;
    CUSTOM_TOBJ_INFO component_list[] = {
        {COOLING_CUSTOMMODE_DISK_TOBJ, NULL, RFPROP_CUSTOM_DISK_TOBJ},
        {COOLING_CUSTOMMODE_MEMORY_TOBJ, NULL, RFPROP_CUSTOM_MEMORY_TOBJ},
        {COOLING_CUSTOMMODE_PCH_TOBJ, NULL, RFPROP_CUSTOM_PCH_TOBJ},
        {COOLING_CUSTOMMODE_VRD_TOBJ, NULL, RFPROP_CUSTOM_VRD_TOBJ},
        {COOLING_CUSTOMMODE_VDDQ_TOBJ, NULL, RFPROP_CUSTOM_VDDQ_TOBJ},
        {COOLING_CUSTOMMODE_NPU_HBM_TOBJ, NULL, RFPROP_CUSTOM_NPU_HBM_TOBJ},
        {COOLING_CUSTOMMODE_NPU_AICORE_TOBJ, NULL, RFPROP_CUSTOM_NPU_AICORE_TOBJ},
        {COOLING_CUSTOMMODE_NPU_BOARD_TOBJ, NULL, RFPROP_CUSTOM_NPU_BOARD_TOBJ}
    };

    
    return_val_do_info_if_fail(VOS_OK == provider_paras_check(i_paras) && NULL != obj_json_mode && NULL != val_json,
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    
    if (json_object_object_get_ex(obj_json_mode, RFPROP_SMART_COOLING_COREM, &json_prop)) {
        ret = set_smart_cooling_mode_cpu(i_paras, json_prop, o_message_jso);
        
        return_array[arr_subscript++] = ret;
        
        json_prop = NULL;
    }

    
    if (json_object_object_get_ex(obj_json_mode, RFPROP_SMART_COOLING_OUTLET, &json_prop)) {
        ret = set_smart_cooling_mode_outlet(i_paras, json_prop, o_message_jso);
        return_array[arr_subscript++] = ret;
        json_prop = NULL;
    }
    

    
    if (json_object_object_get_ex(obj_json_mode, RFPROP_SMART_COOLING_INLET_SPEED, &json_prop)) {
        ret = set_smart_cooling_env_temp(i_paras, o_message_jso, json_prop, COOLING_CUSTOMMODE_INLETTEMP);
        
        return_array[arr_subscript++] = ret;
        
        json_prop = NULL;
    }

    for (cnt = 0; cnt < sizeof(component_list) / sizeof(CUSTOM_TOBJ_INFO); cnt++) {
        if (json_object_object_get_ex(obj_json_mode, component_list[cnt].rf_key_name, &json_prop)) {
            ret = set_smart_cooling_tobj(i_paras, o_message_jso, json_prop, component_list[cnt].component_bit);
            return_array[arr_subscript++] = ret;
            json_prop = NULL;
        }
    }

    if (json_object_object_get_ex(obj_json_mode, RFPROP_CUSTOM_DISK_ENV, &json_prop)) {
        ret = set_smart_cooling_env_temp(i_paras, o_message_jso, json_prop, COOLING_CUSTOMMODE_DISK_ENV);
        return_array[arr_subscript++] = ret;
    }

    return (arr_subscript == 0) ? (HTTP_BAD_REQUEST) : (return_value_judgment(arr_subscript, return_array));
    
}

LOCAL gint32 set_thermal_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle;
    json_object *val_json = NULL;
    json_object *obj_json_mode = NULL;
    json_object *smart_cool_obj = NULL;
    gint32 ret_code = HTTP_BAD_REQUEST;
    gint32 count;
    guint8 uint8_value = 0;
    json_bool ret_bool;
    gint32 return_array[NUMBER_THERMAL_OEM] = {0};
    gint32 arr_subscript = 0;

    
    return_val_do_info_if_fail(VOS_OK == provider_paras_check(i_paras), HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    
    ret = get_fan_management_supported(&uint8_value);
    return_val_do_info_if_expr(VOS_OK != ret || 0 == uint8_value, HTTP_NOT_IMPLEMENTED,
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, RFPROP_COMMON_OEM, o_message_jso,
        RFPROP_COMMON_OEM));

    

    
    
    ret_bool = json_object_object_get_ex(i_paras->val_jso, RFPROP_COMMON_HUAWEI, &val_json);
    return_val_if_fail(ret_bool && (NULL != val_json), HTTP_BAD_REQUEST);
    

    
    ret = dfl_get_object_handle(COOLINGCLASS, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s, %d:get cooling class obj_handle failed.", __FUNCTION__, __LINE__));

    
    *o_message_jso = json_object_new_array();
    return_val_do_info_if_fail(NULL != *o_message_jso, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s, %d: new array failed.", __FUNCTION__, __LINE__));

    count = json_object_object_length(val_json);

    
    if (json_object_object_get_ex(val_json, RFPROP_COOLING_LEVEL, &obj_json_mode) == TRUE) {
        ret = set_fan_level_rpm(i_paras, o_message_jso, o_result_jso, obj_json_mode, obj_handle);
        
        return_array[arr_subscript] = ret;
        arr_subscript++;
        
    }

    
    if (json_object_object_get_ex(val_json, RFPROP_COOLING_MODE, &obj_json_mode) == TRUE) {
        ret = set_fan_speed_mode(i_paras, o_message_jso, o_result_jso, val_json, obj_json_mode, obj_handle);
        if (VOS_ERR == ret && HTTP_OK != ret_code) {
            return HTTP_BAD_REQUEST;
        }

        
        ret_code = ret;
        

        
        return_array[arr_subscript] = ret_code;
        arr_subscript++;
        
    }

    
    if (json_object_object_get_ex(val_json, RFPROP_COOLING_MODE, &obj_json_mode) == FALSE &&
        json_object_object_get_ex(val_json, RFPROP_COOLING_TIMEOUT, &obj_json_mode) == TRUE) {
        ret = set_manual_mode_timeout(i_paras, o_message_jso, o_result_jso, obj_json_mode, obj_handle);
        if (HTTP_OK == ret || RFPROP_COOLING_SET_COUNT == count) {
            ret_code = ret;
        } else {
            ret_code = (HTTP_OK == ret_code) ? ret_code : ret;
        }

        
        return_array[arr_subscript] = ret_code;
        arr_subscript++;
        
    }

    
    
    if (json_object_object_get_ex(val_json, RFPROP_SMART_COOLING_MODE, &smart_cool_obj)) {
        ret = set_smart_cooling_mode(i_paras, o_message_jso, smart_cool_obj, obj_handle, val_json);

        
        return_array[arr_subscript] = ret;
        arr_subscript++;
        
        smart_cool_obj = NULL;
    }

    
    if (json_object_object_get_ex(val_json, RFPROP_SMART_COOLING_CUSTOM, &smart_cool_obj)) {
        ret = set_smart_cooling_mode_custom(i_paras, o_message_jso, smart_cool_obj, val_json);

        
        return_array[arr_subscript] = ret;
        arr_subscript++;
        
    }

    

    ret_code = (0 == arr_subscript) ? (HTTP_BAD_REQUEST) : (return_value_judgment(arr_subscript, return_array));
    
    return ret_code;
}

LOCAL gint32 set_fan_speed_mode(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso,
    json_object *val_json, json_object *obj_json_mode, OBJ_HANDLE obj_handle)
{
    int iRet;
    gint32 ret;
    GSList *input_list = NULL;
    gchar fan_mode[ARRAY_LENTH] = {0};
    guint8 state = 0;
    gint32 time_out = RFPROP_COOLING_DEFAULT_TIME;
    json_object *message_temp_jso = NULL;
    const gchar *mode_err_mes = "Oem/Huawei/FanSpeedAdjustmentMode";
    const gchar *timeout_err_mes = "Oem/Huawei/FanManualModeTimeoutSeconds";

    
    return_val_do_info_if_fail(VOS_OK == provider_paras_check(i_paras) && NULL != obj_json_mode,
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));
    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, mode_err_mes, &message_temp_jso, mode_err_mes);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    iRet = snprintf_s(fan_mode, sizeof(fan_mode), sizeof(fan_mode) - 1, "%s", dal_json_object_get_str(obj_json_mode));
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    debug_log(DLOG_DEBUG, "%s, %d: fan mode is: %s.", __FUNCTION__, __LINE__, fan_mode);

    
    if ((VOS_OK == g_strcmp0(RFPROP_COOLING_MODE_AUTOMATIC, fan_mode)) &&
        (json_object_object_get_ex(val_json, RFPROP_COOLING_TIMEOUT, &obj_json_mode) == TRUE)) {
        (void)create_message_info(MSGID_FAN_ATTRIBUTE_CONFLICT, NULL, &message_temp_jso, mode_err_mes,
            RFPROP_COOLING_MODE_AUTOMATIC, timeout_err_mes);
        (void)json_object_array_add(*o_message_jso, message_temp_jso);
        return VOS_ERR;
    }

    if (VOS_OK == g_strcmp0(RFPROP_COOLING_MODE_AUTOMATIC, fan_mode)) {
        state = RFPROP_COOLING_AUTOMATIC;
    } else if (VOS_OK == g_strcmp0(RFPROP_COOLING_MODE_MANUAL, fan_mode)) {
        
        
        if ((json_object_object_get_ex(val_json, RFPROP_COOLING_TIMEOUT, &obj_json_mode) == TRUE)) {
            
            return_val_do_info_if_fail(json_type_int == json_object_get_type(obj_json_mode), HTTP_BAD_REQUEST,
                (void)create_message_info(MSGID_PROP_FORMAT_ERR, timeout_err_mes, &message_temp_jso,
                dal_json_object_get_str(obj_json_mode), timeout_err_mes);
                (void)json_object_array_add(*o_message_jso, message_temp_jso));
            time_out = json_object_get_int(obj_json_mode);
        }

        
        state = RFPROP_COOLING_MANUAL;
    } else {
        (void)create_message_info(MSGID_VALUE_OUT_OF_RANGE, NULL, &message_temp_jso, (const gchar *)fan_mode);
        (void)json_object_array_add(*o_message_jso, message_temp_jso);
        return HTTP_BAD_REQUEST;
    }

    
    input_list = g_slist_append(input_list, g_variant_new_byte(state));
    input_list = g_slist_append(input_list, g_variant_new_uint32((guint32)time_out));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        COOLINGCLASS, METHOD_COOLING_MODE, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
            
        case RFERR_NO_RESOURCE:
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_temp_jso, i_paras->uri);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            return HTTP_NOT_FOUND;

        case VOS_OK:
        case RFERR_SUCCESS:
            return HTTP_OK;

            
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_temp_jso);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}

LOCAL gint32 set_fan_level_rpm(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso,
    json_object *obj_json_mode, OBJ_HANDLE obj_handle)
{
    gint32 ret;
    GSList *input_list = NULL;
    gint32 fan_level;
    json_object *message_temp_jso = NULL;
    GVariant *mini_value = NULL;
    guint8 min_fan_level;
    const gchar *level_err_mes = "Oem/Huawei/FanSpeedLevelPercents";
    gchar min_fan_value[PROP_VAL_LENGTH] = {0};
    gchar max_fan_value[PROP_VAL_LENGTH] = {0};
    gchar fan_value[PROP_VAL_LENGTH] = {0};

    
    return_val_do_info_if_fail(VOS_OK == provider_paras_check(i_paras) && NULL != obj_json_mode,
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));
    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, level_err_mes, &message_temp_jso, level_err_mes);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    
    
    return_val_do_info_if_fail(json_type_int == json_object_get_type(obj_json_mode), HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, level_err_mes, &message_temp_jso,
        dal_json_object_get_str(obj_json_mode), level_err_mes);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));
    

    
    ret = uip_get_object_property(COOLINGCLASS, COOLINGCLASS, COOLING_PROPERTY_DFTLEVEL, &mini_value);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso);
        debug_log(DLOG_ERROR, "%s, %d: get mini fan level failed .", __FUNCTION__, __LINE__));

    min_fan_level = g_variant_get_byte(mini_value);
    g_variant_unref(mini_value);

    fan_level = json_object_get_int(obj_json_mode);

    debug_log(DLOG_DEBUG, "%s, %d: fan level is: %d.", __FUNCTION__, __LINE__, fan_level);

    
    if ((guint32)fan_level < min_fan_level || (guint32)fan_level > FANSPD_MAX_VAL) {
        
        (void)snprintf_s(fan_value, sizeof(fan_value), sizeof(fan_value) - 1, "%d", fan_level);
        (void)snprintf_s(min_fan_value, sizeof(min_fan_value), sizeof(min_fan_value) - 1, "%u", min_fan_level);
        (void)snprintf_s(max_fan_value, sizeof(max_fan_value), sizeof(max_fan_value) - 1, "%d", FANSPD_MAX_VAL);
        
        (void)create_message_info(MSGID_PROP_VALUE_OUTOFRANGE, level_err_mes, &message_temp_jso,
            (const gchar *)fan_value, level_err_mes, (const gchar *)min_fan_value, (const gchar *)max_fan_value);
        
        
        (void)json_object_array_add(*o_message_jso, message_temp_jso);
        return HTTP_BAD_REQUEST;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte((guchar)fan_level));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        COOLINGCLASS, METHOD_COOLING_LEVEL, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
            
        case RFERR_NO_RESOURCE:
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_temp_jso, i_paras->uri);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            return HTTP_NOT_FOUND;

        case RFERR_SUCCESS:
        case VOS_OK:
            return HTTP_OK;

            
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_temp_jso);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}

LOCAL gint32 set_manual_mode_timeout(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso,
    json_object *obj_json_mode, OBJ_HANDLE obj_handle)
{
    gint32 ret;
    GSList *input_list = NULL;
    gint32 time_out;
    json_object *message_temp_jso = NULL;
    const gchar *timeout_err_mes = "Oem/Huawei/FanManualModeTimeoutSeconds";

    
    return_val_do_info_if_fail(VOS_OK == provider_paras_check(i_paras) && NULL != obj_json_mode,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, timeout_err_mes, &message_temp_jso,
        timeout_err_mes);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    
    
    return_val_do_info_if_fail(json_type_int == json_object_get_type(obj_json_mode), HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, timeout_err_mes, &message_temp_jso,
        dal_json_object_get_str(obj_json_mode), timeout_err_mes);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));
    

    time_out = json_object_get_int(obj_json_mode);
    debug_log(DLOG_DEBUG, "%s, %d: Manual Mode Time out is: %u.", __FUNCTION__, __LINE__, time_out);

    input_list = g_slist_append(input_list, g_variant_new_byte(RFPROP_COOLING_MANUAL));
    input_list = g_slist_append(input_list, g_variant_new_uint32((guint32)time_out));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        COOLINGCLASS, METHOD_COOLING_MODE, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
        case RFERR_SUCCESS:
            return HTTP_OK;

            
        case RFERR_NO_RESOURCE:
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_temp_jso, i_paras->uri);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            return HTTP_NOT_FOUND;

            
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_temp_jso);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 get_chassis_thermal_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;

    gint32 ret;
    gchar odata_context[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = rf_gen_chassis_component_id(NULL, i_paras->obj_handle, slot_id, MAX_RSC_ID_LEN);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__));

    iRet =
        snprintf_s(odata_context, sizeof(odata_context), sizeof(odata_context) - 1, CHASSIS_THERMAL_METADATA, slot_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    *o_result_jso = json_object_new_string((const gchar *)odata_context);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL void parse_sensor_single_number(gboolean reading_seccess, json_object *obj_json, gchar *value_str, gdouble value)
{
    
    return_do_info_if_fail((NULL != obj_json && NULL != value_str),
        debug_log(DLOG_ERROR, "%s, %d: check params fail.\n", __FUNCTION__, __LINE__));

    if (reading_seccess) {
        json_object_object_add(obj_json, value_str, ex_json_object_new_double(value, "%.2f"));
    } else {
        json_object_object_add(obj_json, value_str, NULL);
    }

    return;
}


LOCAL void parse_sensor_single_string(gboolean reading_seccess, json_object *obj_json, gchar *value_str,
    gchar *str_value)
{
    
    return_do_info_if_fail((NULL != obj_json && NULL != value_str),
        debug_log(DLOG_ERROR, "%s, %d: check params fail.\n", __FUNCTION__, __LINE__));

    if (reading_seccess) {
        json_object_object_add(obj_json, value_str, json_object_new_string(str_value));
    } else {
        json_object_object_add(obj_json, value_str, NULL);
    }

    return;
}


LOCAL void parse_sensor_value(guint16 reading_mask, gdouble lnr, gdouble lc, gdouble lnc, gdouble unc, gdouble uc,
    gdouble unr, json_object *obj_json)
{
    SENSOR_READINGMASK_BIT *sensor_reading_mask = NULL;
    sensor_reading_mask = (SENSOR_READINGMASK_BIT *)&reading_mask;

    
    return_do_info_if_fail(NULL != obj_json,
        debug_log(DLOG_ERROR, "%s, %d: check params fail.\n", __FUNCTION__, __LINE__));

    parse_sensor_single_number(sensor_reading_mask->readable_lower_noncritical, obj_json,
        RFPROP_SENSOR_LOWER_THRESHOLD_NON_CRITICAL, lnc);
    parse_sensor_single_number(sensor_reading_mask->readable_lower_critical, obj_json,
        RFPROP_SENSOR_LOWER_THRESHOLD_CRITICAL, lc);
    parse_sensor_single_number(sensor_reading_mask->readable_lower_nonrecoverable, obj_json,
        RFPROP_SENSOR_LOWER_THRESHOLD_FATAL, lnr);
    parse_sensor_single_number(sensor_reading_mask->readable_upper_noncritical, obj_json,
        RFPROP_SENSOR_UPPER_THRESHOLD_NON_CRITICAL, unc);
    parse_sensor_single_number(sensor_reading_mask->readable_upper_critical, obj_json,
        RFPROP_SENSOR_UPPER_THRESHOLD_CRITICAL, uc);
    parse_sensor_single_number(sensor_reading_mask->readable_upper_nonrecoverable, obj_json,
        RFPROP_SENSOR_UPPER_THRESHOLD_FATAL, unr);

    return;
}


void get_value_unc_uc_unr_lnc_lc_lnr(OBJ_HANDLE obj_node, json_object *obj_json, gchar *value_str)
{
    gdouble value = 0.0;
    gdouble lnr = 0.0;
    gdouble lc = 0.0;
    gdouble lnc = 0.0;
    gdouble unc = 0.0;
    gdouble uc = 0.0;
    gdouble unr = 0.0;
    guint16 reading_mask = 0;
    guint8 reading_status = 0;
    guint8 capabilities = 0;
    EVENT_WORK_STATE_S *sensor_reading_status = NULL;
    SENSOR_CAPABILITY_S *sensor_cap = NULL;

    
    return_do_info_if_fail((NULL != obj_json && NULL != value_str),
        debug_log(DLOG_ERROR, "%s, %d: check params fail.\n", __FUNCTION__, __LINE__));

    
    (void)dal_get_property_value_byte(obj_node, PROPERTY_THS_SENSOR_READING_STATUS, &reading_status);
    sensor_reading_status = (EVENT_WORK_STATE_S *)&reading_status;

    
    (void)dal_get_property_value_double(obj_node, PROPERTY_THR_SENSOR_READING_CONVERT, &value);

    
    if (!sensor_reading_status->disable_scanning || !sensor_reading_status->disable_scanning_local ||
        !sensor_reading_status->disable_all ||
        !sensor_reading_status->initial_update_progress 
        || !sensor_reading_status->disable_access_error) {
        json_object_object_add(obj_json, value_str, NULL);
    } else {
        json_object_object_add(obj_json, value_str, ex_json_object_new_double(value, "%.2f"));
    }

    (void)dal_get_property_value_double(obj_node, PROPERTY_THR_SENSOR_UNR_CONVERT, &unr);
    (void)dal_get_property_value_double(obj_node, PROPERTY_THR_SENSOR_UC_CONVERT, &uc);
    (void)dal_get_property_value_double(obj_node, PROPERTY_THR_SENSOR_UNC_CONVERT, &unc);
    (void)dal_get_property_value_double(obj_node, PROPERTY_THR_SENSOR_LNR_CONVERT, &lnr);
    (void)dal_get_property_value_double(obj_node, PROPERTY_THR_SENSOR_LC_CONVERT, &lc);
    (void)dal_get_property_value_double(obj_node, PROPERTY_THR_SENSOR_LNC_CONVERT, &lnc);
    (void)dal_get_property_value_uint16(obj_node, PROPERTY_THR_SENSOR_READING_MASK, &reading_mask);
    (void)dal_get_property_value_byte(obj_node, PROPERTY_THR_SENSOR_CAPABILITIES, &capabilities);
    sensor_cap = (SENSOR_CAPABILITY_S *)&capabilities;

    if (!((SENSOR_CAP_READABLE_SETTABLE == sensor_cap->threshold_access_support) ||
        (SENSOR_CAP_READABLE == sensor_cap->threshold_access_support))) {
        json_object_object_add(obj_json, RFPROP_SENSOR_UPPER_THRESHOLD_NON_CRITICAL, NULL);
        json_object_object_add(obj_json, RFPROP_SENSOR_UPPER_THRESHOLD_CRITICAL, NULL);
        json_object_object_add(obj_json, RFPROP_SENSOR_UPPER_THRESHOLD_FATAL, NULL);
        json_object_object_add(obj_json, RFPROP_SENSOR_LOWER_THRESHOLD_NON_CRITICAL, NULL);
        json_object_object_add(obj_json, RFPROP_SENSOR_LOWER_THRESHOLD_CRITICAL, NULL);
        json_object_object_add(obj_json, RFPROP_SENSOR_LOWER_THRESHOLD_FATAL, NULL);
    } else {
        parse_sensor_value(reading_mask, lnr, lc, lnc, unc, uc, unr, obj_json);
    }

    return;
}


void get_temperature_status(OBJ_HANDLE obj_node, json_object *obj_json)
{
    guint8 reading_status = 0;
    guint8 uint8_value = 0;
    EVENT_WORK_STATE_S *sensor_reading_status = NULL;
    json_object *obj_json_status = NULL;

    
    return_do_info_if_fail((NULL != obj_json),
        debug_log(DLOG_ERROR, "%s, %d: check params fail.\n", __FUNCTION__, __LINE__));

    
    (void)dal_get_property_value_byte(obj_node, PROPERTY_THS_SENSOR_READING_STATUS, &reading_status);
    sensor_reading_status = (EVENT_WORK_STATE_S *)&reading_status;

    
    (void)dal_get_property_value_byte(obj_node, PROPERTY_THR_SENSOR_HEALTH_STATUS, &uint8_value);

    
    if (!sensor_reading_status->disable_scanning || !sensor_reading_status->disable_scanning_local ||
        !sensor_reading_status->disable_all ||
        !sensor_reading_status->initial_update_progress 
        || !sensor_reading_status->disable_access_error) {
        get_resource_status_property(&uint8_value, NULL, RFPROP_SENSOR_DISABLED, &obj_json_status, FALSE);
    } else {
        get_resource_status_property(&uint8_value, NULL, RFPROP_SENSOR_ENABLED, &obj_json_status, FALSE);
    }

    json_object_object_add(obj_json, RFPROP_SENSOR_STATUS, obj_json_status);

    return;
}


LOCAL void get_thermal_temperature_datas(PROVIDER_PARAS_S *i_paras, json_object *obj_json, gint arr_index,
    OBJ_HANDLE obj_node)
{
    int iRet = -1;
    gint32 ret;
    guint8 uint8_value = 0;
    gchar string_value[ARRAY_LENTH] = {0};
    gchar slot_str[MAX_RSC_ID_LEN] = {0};

    
    return_do_info_if_fail((NULL != obj_json),
        debug_log(DLOG_ERROR, "%s, %d: check params fail.\n", __FUNCTION__, __LINE__));

    
    
    ret = rf_gen_chassis_component_id(NULL, i_paras->obj_handle, slot_str, MAX_RSC_ID_LEN);
    if (VOS_OK != ret) {
        json_object_object_add(obj_json, RFPROP_SENSOR_ODATA_ID, NULL);
        debug_log(DLOG_ERROR, "%s, %d: redfish_get_board_slot fail,odata.id is NULL.\n", __FUNCTION__, __LINE__);
    } else {
        iRet = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1, "%s%s%s/%d", RF_CHASSIS_UI,
            slot_str, RFPROP_MANAGER_THERMALTEM, arr_index);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
        json_object_object_add(obj_json, RFPROP_SENSOR_ODATA_ID, json_object_new_string((const gchar *)string_value));
    }

    

    
    (void)memset_s(string_value, sizeof(string_value), 0, sizeof(string_value));
    (void)snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1, "%d", arr_index);
    json_object_object_add(obj_json, RFPROP_SENSOR_MEMBER_ID, json_object_new_string((const gchar *)string_value));

    
    ret = dal_get_property_value_byte(obj_node, PROPERTY_THR_SENSOR_NUMBER, &uint8_value);
    parse_sensor_single_number(!ret, obj_json, RFPROP_SENSOR_SENSOR_NUM, uint8_value);

    
    (void)memset_s(string_value, sizeof(string_value), 0, sizeof(string_value));
    ret = dal_get_property_value_string(obj_node, PROPERTY_THR_SENSOR_SENSOR_NAME, string_value, sizeof(string_value));
    parse_sensor_single_string(!ret, obj_json, RFPROP_SENSOR_NAME, string_value);

    
    get_value_unc_uc_unr_lnc_lc_lnr(obj_node, obj_json, RFPROP_SENSOR_READING_CELSIUS);

    
    json_object_object_add(obj_json, RFPROP_SENSOR_MIN_READING_RANGE_TEMP, NULL);
    json_object_object_add(obj_json, RFPROP_SENSOR_MAX_READING_RANGE_TEMP, NULL);

    
    get_temperature_status(obj_node, obj_json);

    return;
}


LOCAL gint32 get_thermal_temperatures(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint count = 0;
    guint8 sensor_type = 0;

    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    json_object *obj_json = NULL;

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    *o_result_jso = json_object_new_array();

    
    ret = dfl_get_object_list(CLASS_THRESHOLD_SENSOR, &obj_list);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_OK,
        debug_log(DLOG_ERROR, "%s, %d: get temperatur obj_list fail.\n", __FUNCTION__, __LINE__));

    
    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_THR_SENSOR_SENSOR_TYPE, &sensor_type);

        if (TEMPERATURE_TYPE != sensor_type) { // SensorType为1时代表温度传感器
            continue;
        }

        obj_json = json_object_new_object();

        
        get_thermal_temperature_datas(i_paras, obj_json, count, (OBJ_HANDLE)obj_node->data);
        json_object_array_add(*o_result_jso, obj_json);
        count++;

        obj_json = NULL;
    }

    g_slist_free(obj_list);

    return HTTP_OK;
}


LOCAL void get_fan_status(OBJ_HANDLE obj_sensor_node, OBJ_HANDLE obj_fan_node, json_object *obj_json,
    const gchar *f_or_r)
{
    gint32 ret = 0;
    guint8 health_value = 0;
    guint8 presence_value = 0;
    gdouble double_value = 0.0;
    json_object *obj_json_status = NULL;
    guint8 reading_status = 0;

    
    return_do_info_if_fail((NULL != obj_json && NULL != f_or_r),
        debug_log(DLOG_ERROR, "%s, %d: check params fail.\n", __FUNCTION__, __LINE__));

    if (VOS_OK == g_strcmp0("F", f_or_r)) {
        (void)dal_get_property_value_byte(obj_fan_node, FAN_PROPERTY_PRESENT, &presence_value);
    } else if (VOS_OK == g_strcmp0("R", f_or_r)) {
        (void)dal_get_property_value_byte(obj_fan_node, FAN_R_PROPERTY_PRESENT, &presence_value);
    }
    
    else {
        // 如果不是F或者R，说明不是双胞胎风扇
        (void)dal_get_property_value_byte(obj_fan_node, FAN_PROPERTY_PRESENT, &presence_value);
    }
    

    
    (void)dal_get_property_value_byte(obj_sensor_node, PROPERTY_THS_SENSOR_READING_STATUS, &reading_status);
    EVENT_WORK_STATE_S* status_ptr = (EVENT_WORK_STATE_S*)&reading_status;
    if (status_ptr->disable_scanning && status_ptr->disable_scanning_local && status_ptr->disable_all) {
        ret = dal_get_property_value_double(obj_sensor_node, PROPERTY_THR_SENSOR_READING_CONVERT, &double_value);
    }

    
    (void)dal_get_property_value_byte(obj_fan_node, FAN_PROPERTY_HEALTH, &health_value);

    if (RFPROP_SENSOR_PRESENCE_ABSENT == presence_value) {
        health_value = RFPROP_SENSOR_PRESENCE_OTHER;
        (void)get_resource_status_property(&health_value, NULL, RFPROP_SENSOR_ABSENT, &obj_json_status, FALSE);
    } else if (!ret && double_value) {
        (void)get_resource_status_property(&health_value, NULL, RFPROP_SENSOR_ENABLED, &obj_json_status, FALSE);
    } else {
        debug_log(DLOG_ERROR, "%s: fan standby, sensor name %s, ret = %d, value = %.2f.",
                  __FUNCTION__, dfl_get_object_name(obj_fan_node), ret, double_value);
        (void)get_resource_status_property(&health_value, NULL, RFPROP_SENSOR_STAND_BY_OFFLINE, &obj_json_status,
            FALSE);
    }

    
    json_object_object_add(obj_json, RFPROP_SENSOR_STATUS, obj_json_status);

    return;
}


LOCAL gint32 get_fan_name_from_inner_name(guint32 fan_index, gchar *name_array[],
    gboolean is_twins, gchar name[], gsize name_len)
{
    if (name_len <= 1) {
        debug_log(DLOG_ERROR, "%s: Input param: name_len(%" G_GSIZE_FORMAT ") error.", __FUNCTION__, name_len);
        return RET_ERR;
    }

    gint32 ret = snprintf_s(name, name_len, name_len - 1, "Fan%u", fan_index);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s to name failed, ret %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    if (is_twins) {
        ret = EOK;
        if (g_strcmp0("F", name_array[1]) == 0) {
            ret = strcat_s(name, name_len, " Front");
        } else if (g_strcmp0("R", name_array[1]) == 0) {
            ret = strcat_s(name, name_len, " Rear");
        }

        if (ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strcat_s to name failed, ret %d", __FUNCTION__, ret);
            return RET_ERR;
        }
    }

    return RET_OK;
}


LOCAL gint32 get_fan_name_from_sensor_name(OBJ_HANDLE sensor_handle, gchar name[], gsize name_len)
{
    gchar sensor_name[MAX_NAME_SIZE] = {0};
    gchar *context = NULL;
    gchar *array[ARRAY_LENTH] = {NULL};
    gint32 array_index = 0;

    if (name_len <= 1) {
        debug_log(DLOG_ERROR, "%s: Input param: name_len(%" G_GSIZE_FORMAT ") error.", __FUNCTION__, name_len);
        return RET_ERR;
    }

    gint32 ret = dal_get_property_value_string(sensor_handle,
        PROPERTY_THR_SENSOR_SENSOR_NAME, sensor_name, sizeof(sensor_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get %s %s failed, ret %d", __FUNCTION__,
            dfl_get_object_name(sensor_handle), PROPERTY_THR_SENSOR_SENSOR_NAME, ret);
        return ret;
    }

    
    gchar *token = strtok_s(sensor_name, " ", &context);
    while (token != NULL) {
        array[array_index++] = token;
        token = strtok_s(NULL, " ", &context);
    }

    if (array_index < 2) {    
        debug_log(DLOG_ERROR, "%s: strtok sensor name result error", __FUNCTION__);
        return RET_ERR;
    }

    ret = snprintf_s(name, name_len, name_len - 1, "Fan%s", array[1]);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s to name failed, ret %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 get_thermal_fan_datas(OBJ_HANDLE chassis_handle, json_object **obj_json, gint arr_index,
    OBJ_HANDLE obj_sensor_node, OBJ_HANDLE obj_fan_node, gboolean is_twins)
{
    int iRet;
    guint16 fan_id = 0;
    gchar string_value[ARRAY_LENTH] = {0};
    gchar slot_str[MAX_RSC_ID_LEN] = {0};

    gchar name_string[MAX_NAME_SIZE] = {0};
    gint32 array_index = 0;
    gchar* array[ARRAY_LENTH] = {NULL};
    gchar *pointer = NULL;
    gchar *nexttok = NULL;
    guint32 fan_index = 0;
    json_object *fan_oem = NULL;

    
    (void)memset_s(string_value, sizeof(string_value), 0, sizeof(string_value));
    gint32 ret = dal_get_property_value_string(obj_sensor_node, PROPERTY_THR_SENSOR_INNER_NAME, string_value,
        sizeof(string_value));
    return_val_do_info_if_fail((VOS_OK == ret), RET_ERR,
        debug_log(DLOG_ERROR, "%s get InnerName failed! \n", __FUNCTION__));

    
    
    iRet = snprintf_s(name_string, sizeof(name_string), sizeof(name_string) - 1, "%s", string_value);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    pointer = strtok_s(name_string, "_", &nexttok);

    while (NULL != pointer) {
        
        array[array_index++] = pointer;
        pointer = strtok_s(NULL, "_", &nexttok);
    }

    if (array_index < 2) {    
        debug_log(DLOG_ERROR, "%s: strtok sensor name result error", __FUNCTION__);
        return RET_ERR;
    }

    
    ret = sscanf_s(array[0], "FAN%d", &fan_index);
    return_val_do_info_if_fail(ret > 0, RET_ERR,
        debug_log(DLOG_ERROR, "%s: call sscanf_s failed, ret is %d", __FUNCTION__, ret));

    
    ret = dal_get_property_value_uint16(obj_fan_node, FAN_INDEX, &fan_id);
    return_val_do_info_if_fail((VOS_OK == ret), RET_ERR,
        debug_log(DLOG_ERROR, "%s get fanID failed! \n", __FUNCTION__));

    
    return_val_if_fail((fan_id == fan_index), RET_ERR);

    *obj_json = json_object_new_object();
    return_val_do_info_if_fail(NULL != *obj_json, RET_ERR,
        debug_log(DLOG_ERROR, "%s, %d: new object fail", __FUNCTION__, __LINE__));

    
    
    ret = rf_gen_chassis_component_id(NULL, chassis_handle, slot_str, MAX_RSC_ID_LEN);
    if (VOS_OK != ret) {
        json_object_object_add(*obj_json, RFPROP_SENSOR_ODATA_ID, NULL);
        debug_log(DLOG_ERROR, "%s, %d: redfish_get_board_slot fail,odata.id is NULL.\n", __FUNCTION__, __LINE__);
    } else {
        iRet = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1, "%s%s%s/%d", RF_CHASSIS_UI,
            slot_str, RFPROP_MANAGER_THERMALFAN, arr_index);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
        json_object_object_add(*obj_json, RFPROP_SENSOR_ODATA_ID, json_object_new_string((const gchar *)string_value));
    }

    

    
    (void)snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1, "%d", arr_index);
    parse_sensor_single_string(TRUE, *obj_json, RFPROP_SENSOR_MEMBER_ID, string_value);

    
    (void)memset_s(string_value, sizeof(string_value), 0, sizeof(string_value));

    
    if (dal_match_software_type(MGMT_SOFTWARE_TYPE_PANGEA_ARCTIC_CTRL)) {
        ret = get_fan_name_from_sensor_name(obj_sensor_node, string_value, sizeof(string_value));
    } else {
        ret = get_fan_name_from_inner_name(fan_index, array, is_twins, string_value, sizeof(string_value));
    }

    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: redfish get fan name failed", __FUNCTION__);
    }

    parse_sensor_single_string(TRUE, *obj_json, RFPROP_SENSOR_NAME, string_value);

    
    rf_add_property_jso_string(obj_fan_node, FAN_PROPERTY_MODEL, RFPROP_MODEL, *obj_json);
    

    
    get_value_unc_uc_unr_lnc_lc_lnr(obj_sensor_node, *obj_json, RFPROP_SENSOR_READING);

    
    json_object_object_add(*obj_json, RFPROP_SENSOR_MIN_READING_RANGE, NULL);
    json_object_object_add(*obj_json, RFPROP_SENSOR_MAX_READING_RANGE, NULL);

    
    get_fan_status(obj_sensor_node, obj_fan_node, *obj_json, array[1]);

    
    json_object_object_add(*obj_json, RFPROP_SENSOR_READING_UNITS, json_object_new_string(RFPROP_SENSOR_RPMS));
    
    
    get_fan_part_num(obj_fan_node, obj_json);
    
    
    double speed_rpm = json_object_get_double(json_object_object_get(*obj_json, RFPROP_SENSOR_READING));
    ret = get_fan_oem(obj_fan_node, &fan_oem, array[1], speed_rpm);
    
    if (ret == RET_OK) {
        json_object_object_add(*obj_json, RFPROP_COMMON_OEM, fan_oem);
    } else {
        json_object_object_add(*obj_json, RFPROP_COMMON_OEM, NULL);
    }

    

    return RET_OK;
}


LOCAL gboolean check_enc_fan(const gchar* uri, OBJ_HANDLE fan_handle)
{
#define BLADE_FAN 3
    guint8 locality = 0;
    
    if (g_ascii_strcasecmp(uri, "/redfish/v1/chassis/enc/thermal") != 0) {
        return TRUE;
    }

    (void)dal_get_property_value_byte(fan_handle, FAN_PROPERTY_LOCALITY, &locality);
    return (locality == BLADE_FAN) ? FALSE : TRUE;
}


gint32 get_thermal_fans(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, json_object** o_result_jso)
{
    gint count = 0;
    guint8 sensor_type = 0;
    guint32 fan_count = 0;
    guint32 fan_sensor_count = 0;
    gboolean is_twins = FALSE;

    GSList *obj_sensor_list = NULL;
    GSList *obj_fan_list = NULL;
    GSList *obj_sensor_node = NULL;
    GSList *obj_fan_node = NULL;
    guchar board_type = 0;
    guint8 uint8_value = 0;

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    *o_result_jso = json_object_new_array();

    
    
    gint32 ret = get_fan_management_supported(&uint8_value);
    return_val_if_expr(VOS_OK != ret || 0 == uint8_value, HTTP_OK);
    

    
    ret = redfish_get_board_type(&board_type);
    if ((VOS_OK != ret) || (BOARD_SWITCH == board_type)) {
        return HTTP_OK;
    }

    

    
    ret = dfl_get_object_list(CLASS_THRESHOLD_SENSOR, &obj_sensor_list);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_OK,
        debug_log(DLOG_ERROR, "%s, %d: obj_sensor_list fail.\n", __FUNCTION__, __LINE__));

    ret = dfl_get_object_list(FANCLASS, &obj_fan_list);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_OK, g_slist_free(obj_sensor_list); obj_sensor_list = NULL;
        debug_log(DLOG_MASS, "%s, %d: get fan obj_list fail.\n", __FUNCTION__, __LINE__));

    
    (void)dfl_get_object_count(FANCLASS, &fan_count);

    for (obj_sensor_node = obj_sensor_list; obj_sensor_node; obj_sensor_node = obj_sensor_node->next) {
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_sensor_node->data, PROPERTY_THR_SENSOR_SENSOR_TYPE,
            &sensor_type);

        if (sensor_type == SENSOR_TYPE_FAN) {
            fan_sensor_count++;
        }
    }

    
    do_info_if_true(fan_count != fan_sensor_count, (is_twins = TRUE));
    

    
    for (obj_sensor_node = obj_sensor_list; obj_sensor_node; obj_sensor_node = obj_sensor_node->next) {
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_sensor_node->data, PROPERTY_THR_SENSOR_SENSOR_TYPE,
            &sensor_type);

        if (sensor_type != SENSOR_TYPE_FAN) {
            continue;
        }

        
        for (obj_fan_node = obj_fan_list; obj_fan_node; obj_fan_node = obj_fan_node->next) {
            
            if (check_enc_fan(i_paras->uri, (OBJ_HANDLE)obj_fan_node->data) == FALSE) {
                continue;
            }

            json_object *obj_json = NULL;
            
            ret = get_thermal_fan_datas(i_paras->obj_handle, &obj_json, count, (OBJ_HANDLE)obj_sensor_node->data,
                (OBJ_HANDLE)obj_fan_node->data, is_twins);
            if (ret != RET_OK) {
                continue;
            }

            json_object_array_add(*o_result_jso, obj_json);
            count++;
        }
    }

    g_slist_free(obj_sensor_list);
    g_slist_free(obj_fan_list);

    return HTTP_OK;
}


LOCAL gint32 get_chassis_thermal_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;
    gchar string_value[ARRAY_LENTH] = {0};
    gchar slot_str[MAX_RSC_ID_LEN] = {0};
    gint32 ret;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_DEBUG, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    

    // 获取 @odata.id
    ret = rf_gen_chassis_component_id(NULL, i_paras->obj_handle, slot_str, MAX_RSC_ID_LEN);

    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);
    iRet = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1, "%s%s%s", "/redfish/v1/Chassis/",
        slot_str, "/Thermal");
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    *o_result_jso = json_object_new_string((const gchar *)string_value);

    return HTTP_OK;
}


gint32 thermal_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    
    gboolean ret;
    
    ret = rf_check_chassis_uri_valid_allow_node_enc(i_paras->uri, &i_paras->obj_handle);
    
    if (FALSE == ret || 0 == i_paras->obj_handle) {
        return HTTP_NOT_FOUND;
    }

    
    ret = check_enclosure_component_type(i_paras->obj_handle, TRUE);
    

    return_val_if_fail(VOS_OK == ret, HTTP_NOT_FOUND);
    

    *prop_provider = g_thermal_provider;
    *count = sizeof(g_thermal_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}


LOCAL gboolean redfish_check_chassis_pattern_recource(gchar *uri_string, const gchar* pattern_str)
{
    gint32 iRet;
    gint32 chassis_uri_len;
    gint32 chassis_uri_prefix_len;
    gchar chassis_uri_prefix[MAX_URI_LENGTH];

    if (uri_string == NULL || pattern_str == NULL) {
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__);
        return FALSE;
    }
    chassis_uri_len = strlen(uri_string);
    (void)memset_s(chassis_uri_prefix, MAX_URI_LENGTH, 0, MAX_URI_LENGTH);
    iRet = snprintf_s(chassis_uri_prefix, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, URI_FORMAT_CHASSIS, pattern_str);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
        return FALSE;
    }
    chassis_uri_prefix_len = strlen(chassis_uri_prefix);
    if (chassis_uri_len >= chassis_uri_prefix_len) {
        if (g_ascii_strncasecmp(uri_string, chassis_uri_prefix, chassis_uri_prefix_len) == 0) {
            if ((*(uri_string + chassis_uri_prefix_len) == '\0') || (*(uri_string + chassis_uri_prefix_len) == '/')) {
                return TRUE;
            }
        }
    }
    return FALSE;
}


gint32 get_thermal_src(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    OBJ_HANDLE event_handle = 0;
    guint8 event_detail_custom_id = RF_RSC_CUSTOM_ID_DEDAULT;
    guint8 uint8_value = 0;
    gint32 ret;

    return_val_do_info_if_expr((i_paras == NULL) || (o_rsc_jso == NULL) || (o_err_array_jso == NULL), RF_FAILED,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    if (dal_check_if_vsmm_supported() &&
        redfish_check_chassis_pattern_recource(i_paras->uri, RF_ENCLOSURE_CHASSIS_PREFIX)) {
        debug_log(DLOG_INFO, "%s resource not support", __FUNCTION__);
        return RF_RSC_NOT_FOUND;
    }
    (void)provider_get_prop_values(i_paras, &o_rsc_jso, o_err_array_jso, g_thermal_provider,
        G_N_ELEMENTS(g_thermal_provider));

    // 复用事件上报的CMCC定制化属性显示FanSummary资源信息 非CMCC定制直接返回
    (void)dal_get_object_handle_nth(CLASS_RF_EVT_SVC, 0, &event_handle);
    (void)dal_get_property_value_byte(event_handle, PROPERTY_RF_EVT_SVC_EVENT_DETAIL_CUSTOMIZED_ID,
        &event_detail_custom_id);
    return_val_if_expr(event_detail_custom_id != RF_RSC_CUSTOM_ID_CMCC, VOS_OK);

    
    ret = get_fan_management_supported(&uint8_value);
    return_val_if_expr(ret != VOS_OK || uint8_value == 0, VOS_OK);

    _get_fan_health_group_value(&o_rsc_jso);

    return VOS_OK;
}


gint32 chassis_thermal_etag_del_property(json_object *object)
{
    SPECIAL_PROP_S value[] = {
        {3, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RFPROP_COOLING_TIMEOUT, NULL, NULL}},
        {3, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RFPROP_COOLING_FAN_POWER, NULL, NULL}},
        {2, {RFPROP_SENSOR_TEMPERATURES, RFPROP_SENSOR_READING_CELSIUS, NULL, NULL, NULL}},
        {2, {RFPROP_SENSOR_FANS, RFPROP_SENSOR_READING, NULL, NULL, NULL}},
                               
        {2, {RFPROP_SENSOR_FANS, SPEED_RATIO_JSON_POINTER, NULL, NULL, NULL}},
                               
        {0, {NULL, NULL, NULL, NULL, NULL}}
    };
    return rsc_del_none_etag_affected_property(object, value, G_N_ELEMENTS(value));
}


LOCAL PROPERTY_PROVIDER_S g_thermal_inlet_history_temperature_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_inlet_history_temperature_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_inlet_history_temperature_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_HISTORY_DATA, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_inlet_history_temperature, NULL, NULL, ETAG_FLAG_DISABLE}
};



LOCAL gint32 get_inlet_history_temperature_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar uri[MAX_URI_LENGTH + 1] = {0};
    gchar slot[MAX_RSC_NAME_LEN + 1] = {0};

    
    return_val_do_info_if_fail(NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error.", __FUNCTION__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot, sizeof(slot) - 1);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: redfish_get_board_slot fail.", __FUNCTION__));

    if (check_pangea_node_to_enc(i_paras->obj_handle)) {
        ret = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, URI_FORMAT_CHASSIS_INLET_HISTORY_TEMPERATURE, "Enc");
    } else {
        ret = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, URI_FORMAT_CHASSIS_INLET_HISTORY_TEMPERATURE, slot);
    }
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }

    *o_result_jso = json_object_new_string((const char *)uri);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_string fail", __FUNCTION__));

    return HTTP_OK;
}


LOCAL gint32 get_inlet_history_temperature_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar odata_context[MAX_URI_LENGTH + 1] = {0};
    gchar slot[MAX_RSC_NAME_LEN + 1] = {0};

    
    return_val_do_info_if_fail(NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error.", __FUNCTION__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: redfish_get_board_slot fail.", __FUNCTION__));

    if (check_pangea_node_to_enc(i_paras->obj_handle)) {
        ret = snprintf_s(odata_context, sizeof(odata_context), sizeof(odata_context) - 1,
            CHASSIS_INLET_HISTORY_TEMPERATURE_METADATA, "Enc");
    } else {
        ret = snprintf_s(odata_context, sizeof(odata_context), sizeof(odata_context) - 1,
            CHASSIS_INLET_HISTORY_TEMPERATURE_METADATA, slot);
    }
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }

    *o_result_jso = json_object_new_string((const char *)odata_context);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_string fail", __FUNCTION__));

    return HTTP_OK;
}


LOCAL gint32 get_inlet_history_temperature(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar file_path[MAX_FILE_PATH_LENGTH + 1] = {0};
    gchar* labels[] = {RFPROP_INLET_TEMPERATURE};

    
    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_message_jso) && (NULL != o_result_jso),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error.", __FUNCTION__));

    
    // MAX_FILE_PATH_LENGTH大小为256，大于格式化后字符串长度，无需判断返回值
    int iRet = snprintf_s(file_path, MAX_FILE_PATH_LENGTH + 1, MAX_FILE_PATH_LENGTH, "%s%s", OPT_PME_PRAM_FOLDER,
        INLET_HISTORY_TEMPERATURE_FILE);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    
    
    
    gint32 ret = read_history_data_from_file(file_path, sizeof(labels) / sizeof(char *), labels, o_result_jso,
        READ_HISTORY_UINT16);
    

    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s: read_history_data_from_file file, ret is %d.", __FUNCTION__, ret));

    return HTTP_OK;
}


gint32 thermal_inlet_history_temperature_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret = redfish_check_chassis_uri_valid(i_paras->uri, &i_paras->obj_handle);
    if (ret != TRUE || dal_match_product_id(PRODUCT_ID_PANGEA_V6)) {
        debug_log(DLOG_ERROR, "Leave thermal_inlet_history_temperature_provider_entry HTTP_NOT_FOUND");
        return HTTP_NOT_FOUND;
    }

    ret = check_enclosure_component_type(i_paras->obj_handle, FALSE);
    if (ret != RET_OK) {
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_thermal_inlet_history_temperature_provider;
    *count = sizeof(g_thermal_inlet_history_temperature_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}


gint32 thermal_clear_history_temp_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras,
    PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gint32 ret = redfish_check_chassis_uri_valid(i_paras->uri, &i_paras->obj_handle);
    if (ret != TRUE || i_paras->obj_handle == 0) {
        return HTTP_NOT_FOUND;
    }

    
    ret = check_enclosure_component_type(i_paras->obj_handle, FALSE);
    if (ret != RET_OK || dal_match_product_id(PRODUCT_ID_PANGEA_V6) == TRUE) {
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_clear_history_temp_actioninfo_provider;
    *count = sizeof(g_clear_history_temp_actioninfo_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}


void get_system_overview_inlet_temp_info(json_object *o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    EVENT_WORK_STATE_S *sensor_reading_status = NULL;
    guint8 reading_status = 0;
    gdouble value = 0.0;
    guint16 mask = 0;
    SENSOR_READINGMASK_BIT *reading_mask = NULL;
    guchar capabilities = 0;
    SENSOR_CAPABILITY_S *sensor_cap = NULL;

    ret = dal_get_specific_object_string(CLASS_THRESHOLD_SENSOR, PROPERTY_THR_SENSOR_INNER_NAME,
        SENSER_INLET_TEMP_INNER_NAME, &obj_handle);
    return_do_info_if_expr(ret != VOS_OK, debug_log(DLOG_INFO, "%s: get inlet temp handle fail.\n", __FUNCTION__));

    
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_THS_SENSOR_READING_STATUS, &reading_status);
    sensor_reading_status = (EVENT_WORK_STATE_S *)&reading_status;

    
    (void)dal_get_property_value_double(obj_handle, PROPERTY_THR_SENSOR_READING_CONVERT, &value);

    
    if (sensor_reading_status->disable_scanning && sensor_reading_status->disable_scanning_local &&
        sensor_reading_status->disable_all &&
        sensor_reading_status->initial_update_progress 
        && sensor_reading_status->disable_access_error) {
        json_object_object_add(o_result_jso, RFPROP_SYSTEM_OVERVIEW_INLETTEMP,
            ex_json_object_new_double(value, "%.2f"));
    } else {
        json_object_object_add(o_result_jso, RFPROP_SYSTEM_OVERVIEW_INLETTEMP, NULL);
    }

    
    (void)dal_get_property_value_uint16(obj_handle, PROPERTY_THR_SENSOR_READING_MASK, &mask);
    reading_mask = (SENSOR_READINGMASK_BIT *)&mask;

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_THR_SENSOR_CAPABILITIES, &capabilities);
    sensor_cap = (SENSOR_CAPABILITY_S *)&capabilities;

    (void)get_chassis_sensor_threshold(obj_handle, PROPERTY_THR_SENSOR_UNC_CONVERT, o_result_jso,
        RFPROP_INLETTEMP_MINOR_ALARM_THRESHOLD, reading_mask->readable_upper_noncritical, sensor_cap);

    (void)get_chassis_sensor_threshold(obj_handle, PROPERTY_THR_SENSOR_UC_CONVERT, o_result_jso,
        RFPROP_INLETTEMP_MAJOR_ALARM_THRESHOLD, reading_mask->readable_upper_critical, sensor_cap);
}


void get_system_overview_present_fan_num(json_object **o_result_jso)
{
    gint32 ret;
    guint8 component_type = 0;
    guint8 present = 0;
    gint32 count = 0;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;

    ret = dfl_get_object_list(CLASS_COMPONENT, &obj_list);
    return_do_info_if_expr(ret != VOS_OK,
        debug_log(DLOG_ERROR, "%s:%d: get CLASS_COMPONENT obj_list fail.", __FUNCTION__, __LINE__));

    
    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_DEVICE_TYPE, &component_type);

        continue_if_expr(component_type != COMPONENT_TYPE_FAN);

        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_PRESENCE, &present);

        continue_if_expr(present == 0);

        count++;
    }

    g_slist_free(obj_list);

    
    if (count > 0) {
        count = count - 1;
    }

    *o_result_jso = json_object_new_int(count);

    return;
}


void get_system_overview_fansummary(json_object **o_result_jso)
{
    gint32 ret;
    guint8 uint8_value = 0;

    ret = get_fan_management_supported(&uint8_value);
    if (VOS_OK == ret && 0 != uint8_value) {
        _get_fan_health_group_value(o_result_jso);
    }

    return;
}
