
#include "redfish_provider.h"

#define POWER_HISTORY_DATA_CSV_TAR_NAME ("/tmp/history.csv")

LOCAL gint32 get_chassis_power_powercontrol(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_chassis_power_powercontrol(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_power_voltages(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_power_powersupplies(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_power_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_power_odatacontext(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_power_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_chassis_power_redundancy(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_chassis_power_redundancy(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
gint32 act_power_reset_historydata(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 act_power_reset_statistics(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
gint32 act_power_collect_history_data(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 act_power_set_psu_supply_source(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso);


LOCAL gint32 get_power_history_data_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_power_history_data_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_power_history_data(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);

LOCAL void __add_power_limit_status(json_object* extend_jso);

LOCAL gint32 get_power_capping_failed_status(json_object *extend_jso, PROVIDER_PARAS_S *i_paras);

LOCAL PROPERTY_PROVIDER_S g_chassis_power_provider[] = {
    {RFPROP_POWER_CONTROL, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_chassis_power_powercontrol, set_chassis_power_powercontrol, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_POWER_VOLTAGES, CLASS_THRESHOLD_SENSOR, PROPERTY_THR_SENSOR_SENSOR_NAME, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_power_voltages, NULL, NULL, ETAG_FLAG_DISABLE},
    
    {RFPROP_POWER_SUPPLIES, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_power_powersupplies, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_POWER_CONTROL_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_power_odataid, NULL, NULL, ETAG_FLAG_DISABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_power_odatacontext, NULL, NULL, ETAG_FLAG_DISABLE},
    {RFPROP_FWINV_OEM, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_power_oem, NULL, NULL, ETAG_FLAG_DISABLE},
    {RFPROP_POWER_REDUNDANCY, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_chassis_power_redundancy, set_chassis_power_redundancy, NULL, ETAG_FLAG_ENABLE},
    
    {RFPROP_POWER_OEM_ACTION_RESET_HISTORY_DATA, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, NULL, NULL, act_power_reset_historydata, ETAG_FLAG_ENABLE},
    {RFPROP_POWER_OEM_ACTION_RESET_STATISTICS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, NULL, NULL, act_power_reset_statistics, ETAG_FLAG_ENABLE},
    {RFPROP_POWER_OEM_ACTION_COLLECT_HISTORY_DATA, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY,
        SYS_LOCKDOWN_ALLOW, NULL, NULL, act_power_collect_history_data, ETAG_FLAG_ENABLE},
    {RFPROP_POWER_OEM_ACTION_SET_PSU_SUPPLY_SOURCE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY,
        SYS_LOCKDOWN_ALLOW, NULL, NULL, act_power_set_psu_supply_source, ETAG_FLAG_ENABLE}
};


LOCAL gint32 get_power_reset_data_actioninfo_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL PROPERTY_PROVIDER_S g_power_reset_data_actioninfo_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_power_reset_data_actioninfo_odataid, NULL, NULL, ETAG_FLAG_ENABLE}
};


LOCAL gint32 get_power_reset_statistics_actioninfo_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL PROPERTY_PROVIDER_S g_power_reset_statistics_actioninfo_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_power_reset_statistics_actioninfo_odataid, NULL, NULL, ETAG_FLAG_ENABLE}
};

LOCAL gint32 get_power_collect_history_data_actioninfo_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL PROPERTY_PROVIDER_S g_power_collect_history_data_actioninfo_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_power_collect_history_data_actioninfo_odataid, NULL, NULL, ETAG_FLAG_ENABLE}
};

LOCAL gint32 get_power_set_psu_supply_source_actioninfo_odataid(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso);
LOCAL PROPERTY_PROVIDER_S g_power_set_psu_supply_source_actioninfo_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        get_power_set_psu_supply_source_actioninfo_odataid, NULL, NULL, ETAG_FLAG_ENABLE}
};

LOCAL SET_POWER_REDUNDANCY_RETURN_MESS set_ps_workmode_return_mess_list[] = {
    {ERR_ACTIVE_PS_ONT_PST, MSGID_POWER_ACTIVE_PS_NOT_PST, NEED_TWO_PARAMETER},
    {ERR_NO_STANDBY_PS_PST, MSGID_POWER_NO_STANDBY_PS_PST, NEED_TWO_PARAMETER},
    {ERR_ACTIVE_PS_ONT_HLS, MSGID_POWER_ACTIVE_PS_NOT_HLS, NEED_TWO_PARAMETER},
    {ERR_CURRENT_POWER_HIGN, MSGID_POWER_CURRENT_POWER_HIGN, NEED_TWO_PARAMETER},
    {ERR_ACTIVE_PS_NUM_NE, MSGID_POWER_ACTIVE_PS_NUM_NE, NEED_TWO_PARAMETER},
    {ERR_PS_NOT_SUPPORT, MSGID_POWER_PS_NOT_SUPPORT, NEED_TWO_PARAMETER},
    {ERR_ACTIVE_STANDBY_MODE_UNSUPPORT, MSGID_POWER_ACTIVE_STANDBY_MODE_UNSUPPORT, NEED_ONE_PARAMETER},
    {ERR_NAR_PS_NOT_SUPPORT, MSGID_POWER_PS_NOT_SUPPORT_NAR, NOT_NEED_PARAMETER}
};


LOCAL gint32 get_chassis_power_odatacontext(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
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

    iRet = snprintf_s(odata_context, sizeof(odata_context), sizeof(odata_context) - 1, CHASSIS_POWER_METADATA, slot_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    *o_result_jso = json_object_new_string((const gchar *)odata_context);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL void add_property_value_int(json_object *obj_jso, const gchar *property, gint32 value, gint32 ret)
{
    if (VOS_OK != ret) {
        json_object_object_add(obj_jso, property, NULL);
    } else {
        json_object_object_add(obj_jso, property, json_object_new_int(value));
    }
}


LOCAL void add_property_value_string(json_object *obj_jso, const gchar *property, gchar *value, gint32 ret)
{
    gint32 i = 0;

    
    if (VOS_OK != ret || NULL == value || 0 == *value || !g_strcmp0(value, "N/A") ||
        !g_ascii_strcasecmp(value, "UnKnown")) {
        
        json_object_object_add(obj_jso, property, NULL);
    } else {
        
        for (i = strlen(value) - 1; i >= 0 && (' ' == value[i] || '\t' == value[i]); i--) {
            value[i] = '\0';
        }

        
        for (; ' ' == *value || '\t' == *value; value++)
            ;

        if ('\0' == *value) {
            json_object_object_add(obj_jso, property, NULL);
        } else {
            json_object_object_add(obj_jso, property, json_object_new_string(value));
        }
    }
}

LOCAL gint32 check_accuracy_format(gchar *format)
{
    if (format == NULL) {
        return VOS_ERR;
    }

    if (g_regex_match_simple(ACCURACY_FORMAT_REGEX, (const gchar *)format, G_REGEX_OPTIMIZE, (GRegexMatchFlags)0) ==
        FALSE) {
        debug_log(DLOG_ERROR, "%s: The power info format[%s] regex fail", __FUNCTION__, format);
        return VOS_ERR;
    }

    return VOS_OK;
}



LOCAL void add_property_value_double(json_object *obj_jso, const gchar *property, gdouble value, gint32 ret,
    gchar *format)
{
    if (VOS_OK != ret) {
        json_object_object_add(obj_jso, property, NULL);
    } else {
        if (check_accuracy_format(format) == VOS_OK) {
            json_object_object_add(obj_jso, property, ex_json_object_new_double(value, format));
        } else {
            json_object_object_add(obj_jso, property, ex_json_object_new_double(value, "%.2f"));
        }
    }
}


LOCAL void add_property_value_uint32(json_object *obj_jso, const gchar *property, guint32 value, gint32 ret)
{
    if (VOS_OK != ret) {
        json_object_object_add(obj_jso, property, NULL);
    } else {
        json_object_object_add(obj_jso, property, json_object_new_int64((int64_t)value));
    }
}


LOCAL gint32 get_power_management_supported(guint8 *ps_num)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    ret = dfl_get_object_handle(OBJ_PRODUCT_COMPONENT, &obj_handle);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get OBJ_PRODUCT_COMPONENT handle fail", __FUNCTION__));

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_COMPOENT_PS_NUM, ps_num);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get PROPERTY_COMPOENT_PS_NUM fail", __FUNCTION__));

    return VOS_OK;
}


LOCAL gint32 get_active_standby_enabled(guint8 *enabled)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    ret = dfl_get_object_handle(OBJ_NAME_PME_SERVICECONFIG, &obj_handle);
    return_val_do_info_if_expr(VOS_OK != ret, ret,
        debug_log(DLOG_ERROR, "%s: get obj_list for OBJ_NAME_PME_SERVICECONFIG fail", __FUNCTION__));

    ret = dal_get_property_value_byte(obj_handle, PROTERY_PME_SERVICECONFIG_POWER, enabled);
    return_val_do_info_if_expr(VOS_OK != ret, ret,
        debug_log(DLOG_ERROR, "%s: get PROTERY_PME_SERVICECONFIG_POWER fail", __FUNCTION__));

    return VOS_OK;
}

LOCAL gint32 get_deep_sleep_mode_enabled(guint8 *enabled)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    ret = dfl_get_object_handle(OBJ_NAME_PME_SERVICECONFIG, &obj_handle);
    return_val_do_info_if_expr(VOS_OK != ret, ret,
        debug_log(DLOG_ERROR, "%s: get obj_list for OBJ_NAME_PME_SERVICECONFIG fail", __FUNCTION__));

    ret = dal_get_property_value_byte(obj_handle, PROTERY_PME_SERVICECONFIG_DEEP_SLEEP_ENABLE, enabled);
    return_val_do_info_if_expr(VOS_OK != ret, ret,
        debug_log(DLOG_ERROR, "%s: get PROTERY_PME_SERVICECONFIG_DEEP_SLEEP_ENABLE fail", __FUNCTION__));

    return VOS_OK;
}


LOCAL void get_powercontrol_expected_redundancy(const gchar *slot_str, json_object *obj_huawei,
    OBJ_HANDLE handle_syspower)
{
    gint32 ret;
    guint8 uint8_value = 0xff;
    guint8 mode = 0xff;

    gchar string_value[POWER_STR_MAX_LEN] = {0};

    json_object *obj = NULL;

    
    ret = get_power_management_supported(&uint8_value);
    return_do_info_if_expr(VOS_OK != ret || 0 == uint8_value,
        json_object_object_add(obj_huawei, RFPROP_POWER_EXPECTED_REDUNDANCY, NULL));

    
    ret = get_active_standby_enabled(&uint8_value);
    return_do_info_if_expr(VOS_OK != ret, json_object_object_add(obj_huawei, RFPROP_POWER_EXPECTED_REDUNDANCY, NULL));

    
    ret = dal_get_property_value_byte(handle_syspower, PROPERTY_EXPECTED_POWER_MODE, &mode);
    return_do_info_if_expr(VOS_OK != ret, json_object_object_add(obj_huawei, RFPROP_POWER_EXPECTED_REDUNDANCY, NULL);
        debug_log(DLOG_ERROR, "%s: get PROPERTY_EXPECTED_POWER_MODE fail", __FUNCTION__));

    obj = json_object_new_object();
    json_object_object_add(obj_huawei, RFPROP_POWER_EXPECTED_REDUNDANCY, obj);
    return_if_expr(NULL == obj);

    
    if (0 == uint8_value || RF_POWER_CONTROL_PSU_LOAD_BALANCE == mode) {
        ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1, RFPROP_REDUNDANCY_ODATA_ID_VALUE,
            slot_str, 0);
        return_do_info_if_expr(0 >= ret, json_object_object_add(obj, RFPROP_ODATA_ID, NULL));
    } else { 
        ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1, RFPROP_REDUNDANCY_ODATA_ID_VALUE,
            slot_str, 1);
        return_do_info_if_expr(0 >= ret, json_object_object_add(obj, RFPROP_ODATA_ID, NULL));
    }

    json_object_object_add(obj, RFPROP_ODATA_ID, json_object_new_string((const gchar *)string_value));
}


LOCAL void add_psu_to_array_nth(json_object *obj_array, guint8 arr_index, const gchar *slot_str)
{
    gint32 ret;
    json_object *obj = NULL;
    gchar string_value[POWER_STR_MAX_LEN] = {0};

    obj = json_object_new_object();
    return_do_info_if_fail(NULL != obj, json_object_array_add(obj_array, NULL));

    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1, URI_FORMAT_PS, slot_str, arr_index);
    if (0 >= ret) {
        json_object_object_add(obj, RFPROP_ODATA_ID, NULL);
    } else {
        json_object_object_add(obj, RFPROP_ODATA_ID, json_object_new_string((const gchar *)string_value));
    }

    json_object_array_add(obj_array, obj);
}


LOCAL void add_psu_to_array_with_mask(json_object *obj_array, guint8 ps_num, const gchar *slot_str, const guint32 *mask)
{
    guint8 i;

    for (i = 0; i < ps_num; i++) {
        if (NULL != mask) {
            if (((*mask) >> i) & 0x01) {
                add_psu_to_array_nth(obj_array, i, slot_str);
            }
        } else {
            add_psu_to_array_nth(obj_array, i, slot_str);
        }
    }
}


LOCAL void get_powercontrol_expected_active_psu(const gchar *slot_str, json_object *obj_huawei,
    OBJ_HANDLE handle_syspower)
{
    guint8 uint8_value = 0xff;
    guint8 mode = 0xff;
    guint8 ps_num = 0xff;
    guint32 uint32_value;

    json_object *obj_array = NULL;

    
    gint32 ret = get_power_management_supported(&ps_num);
    return_do_info_if_expr(VOS_OK != ret || 0 == ps_num,
        json_object_object_add(obj_huawei, RFPROP_POWER_EXPECTED_ACTIVE_PSU, NULL));

    ret = get_active_standby_enabled(&uint8_value);
    return_do_info_if_expr(VOS_OK != ret, json_object_object_add(obj_huawei, RFPROP_POWER_EXPECTED_ACTIVE_PSU, NULL));

    
    ret = dal_get_property_value_byte(handle_syspower, PROPERTY_EXPECTED_POWER_MODE, &mode);
    return_do_info_if_expr(VOS_OK != ret, json_object_object_add(obj_huawei, RFPROP_POWER_EXPECTED_ACTIVE_PSU, NULL);
        debug_log(DLOG_ERROR, "%s: get PROPERTY_EXPECTED_POWER_MODE fail", __FUNCTION__));

    obj_array = json_object_new_array();
    json_object_object_add(obj_huawei, RFPROP_POWER_EXPECTED_ACTIVE_PSU, obj_array);
    return_if_expr(NULL == obj_array);

    
    if (0 == uint8_value || RF_POWER_CONTROL_PSU_LOAD_BALANCE == mode) {
        add_psu_to_array_with_mask(obj_array, ps_num, slot_str, NULL);
    } else {
        ret = dal_get_property_value_uint32(handle_syspower, PROPERTY_EXPECTED_ACTIVE, &uint32_value);
        return_do_info_if_expr(VOS_OK != ret,
            debug_log(DLOG_ERROR, "%s: get PROPERTY_EXPECTED_ACTIVE fail", __FUNCTION__));

        
        uint32_value = ~uint32_value;

        add_psu_to_array_with_mask(obj_array, ps_num, slot_str, &uint32_value);
    }
}


LOCAL void get_powercontrol_deepsleep(json_object *obj_huawei, OBJ_HANDLE handle_syspower)
{
    guint8 uint8_value = 0xff;
    guint32 uint32_value = 0xff;

    const gchar *str = NULL;

    
    gint32 ret = dal_get_product_version_num(&uint32_value);
    return_do_info_if_expr(VOS_OK != ret || PRODUCT_VERSION_V5 > uint32_value,
        json_object_object_add(obj_huawei, RFPROP_POWER_DEEP_SLEEP_MODE, NULL));

    
    ret = get_power_management_supported(&uint8_value);
    return_do_info_if_expr(VOS_OK != ret || 0 == uint8_value,
        json_object_object_add(obj_huawei, RFPROP_POWER_DEEP_SLEEP_MODE, NULL));

    
    ret = get_deep_sleep_mode_enabled(&uint8_value);
    return_do_info_if_expr(VOS_OK != ret || 0 == uint8_value,
        json_object_object_add(obj_huawei, RFPROP_POWER_DEEP_SLEEP_MODE, NULL));

    ret = dal_get_property_value_byte(handle_syspower, PROTERY_SYSPOWER_DEEP_SLEEP_MODE_ENABLE, &uint8_value);
    return_do_info_if_expr(VOS_OK != ret, json_object_object_add(obj_huawei, RFPROP_POWER_DEEP_SLEEP_MODE, NULL);
        debug_log(DLOG_ERROR, "%s: get PROTERY_SYSPOWER_DEEP_SLEEP_MODE_ENABLE fail", __FUNCTION__));

    (0 == uint8_value) ? (str = DISABLED_STRING) : (str = ENABLED_STRING);
    json_object_object_add(obj_huawei, RFPROP_POWER_DEEP_SLEEP_MODE, json_object_new_string(str));
}


LOCAL void get_powercontrol_normal_and_redundancy_state(json_object *obj_huawei, OBJ_HANDLE handle_syspower)
{
    guint8 uint8_value = 0xff;
    OBJ_HANDLE obj_handle = 0;
    const gchar *str = NULL;

    return_do_info_if_expr(obj_huawei == NULL, debug_log(DLOG_ERROR, "%s: obj is NULL", __FUNCTION__));

    gint32 ret = dal_get_object_handle_nth(CLASS_NAME_PME_SERVICECONFIG, 0, &obj_handle);
    return_do_info_if_expr(ret != VOS_OK,
        debug_log(DLOG_ERROR, "%s: get obj_list for OBJ_NAME_PME_SERVICECONFIG fail", __FUNCTION__));

    ret = dal_get_property_value_byte(obj_handle, PROTERY_PME_NORMAL_AND_REDUNDANCY_SUPPORT, &uint8_value);
    return_do_info_if_expr(ret != VOS_OK,
        debug_log(DLOG_ERROR, "%s: get PROTERY_PME_NORMAL_AND_REDUNDANCY_ENABLE fail", __FUNCTION__));

    return_do_info_if_expr(uint8_value == RF_DISABLE,
        json_object_object_add(obj_huawei, RFPROP_POWER_NORMAL_AND_REDUNDANCY, NULL));

    ret = dal_get_property_value_byte(handle_syspower, PROTERY_SYSPOWER_NORMAL_AND_REDUNDANCY_ENABLE, &uint8_value);
    return_do_info_if_expr(ret != VOS_OK,
        debug_log(DLOG_ERROR, "%s: get PROTERY_SYSPOWER_NORMAL_AND_REDUNDANCY_ENABLE fail", __FUNCTION__));

    (uint8_value == RF_DISABLE) ? (str = DISABLED_STRING) : (str = ENABLED_STRING);
    json_object_object_add(obj_huawei, RFPROP_POWER_NORMAL_AND_REDUNDANCY, json_object_new_string(str));
}


LOCAL gint32 get_power_capping_policy_status(json_object *extend_jso, PROVIDER_PARAS_S *i_paras)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret = 0;
    guint8 power_capping_enabled = 0;
    guint8 cap_type = 0xFF;
    guint8 software_type = 0xff;
    OBJ_HANDLE shelf_powercapping_handle = 0;

    return_val_if_expr(NULL == i_paras || NULL == extend_jso, VOS_ERR);

    
    (void)dal_get_object_handle_nth(CLASS_NAME_POWERCAPPING, 0, &obj_handle);
    (void)dal_get_property_value_byte(obj_handle, PROTERY_POWERCAP_ENABLE, &power_capping_enabled);
    (void)dal_get_property_value_byte(obj_handle, PROTERY_POWERCAP_TYPE, &cap_type);
    (void)dal_get_software_type(&software_type);
    if (software_type == MGMT_SOFTWARE_TYPE_PANGEA_MGNT) {
        (void)dfl_get_object_handle(OBJ_NAME_SHELF_POWER_CAPPING, &shelf_powercapping_handle);
        (void)dal_get_property_value_byte(shelf_powercapping_handle,
            PROTERY_SHELF_POWER_CAPPING_ENABLE, &power_capping_enabled);
    }
    if (power_capping_enabled) {
        
        if (cap_type == 1 || software_type == MGMT_SOFTWARE_TYPE_PANGEA_MGNT) {
            json_object_object_add(extend_jso, RFPROP_POWER_LIMIT_ACTIVATED, json_object_new_boolean(TRUE));
            return VOS_OK;
        }
        ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
            CLASS_NAME_POWERCAPPING, METHOD_POWERCAP_GETLIMT_POLICY_STATUS, AUTH_ENABLE, i_paras->user_role_privilege,
            NULL, NULL);
        if (ret == VOS_OK) {
            json_object_object_add(extend_jso, RFPROP_POWER_LIMIT_ACTIVATED, json_object_new_boolean(TRUE));
        } else if (ret == PSM_ERROR_LIMIT_POLICY_STATUS_FAIL) {
            json_object_object_add(extend_jso, RFPROP_POWER_LIMIT_ACTIVATED, json_object_new_boolean(FALSE));
        } else {
            json_object_object_add(extend_jso, RFPROP_POWER_LIMIT_ACTIVATED, NULL);
        }
    } else {
        json_object_object_add(extend_jso, RFPROP_POWER_LIMIT_ACTIVATED, json_object_new_boolean(FALSE));
    }

    return VOS_OK;
}


LOCAL gint32 get_power_capping_failed_status(json_object *extend_jso, PROVIDER_PARAS_S *i_paras)
{
    OBJ_HANDLE obj_handle = 0;
    guint8 power_capping_enabled = 0;
    guint32 failed_status = 0;
    guint8 software_type = 0xff;
    OBJ_HANDLE shelf_powercapping_handle = 0;
    guint8 state = 0;

    return_val_if_expr(NULL == i_paras || NULL == extend_jso, VOS_ERR);

    
    (void)dal_get_object_handle_nth(CLASS_NAME_POWERCAPPING, 0, &obj_handle);
    return_val_if_expr(0 == obj_handle, VOS_ERR);

    (void)dal_get_property_value_byte(obj_handle, PROTERY_POWERCAP_ENABLE, &power_capping_enabled);
    (void)dal_get_software_type(&software_type);
    if (software_type == MGMT_SOFTWARE_TYPE_PANGEA_MGNT) {
        (void)dfl_get_object_handle(OBJ_NAME_SHELF_POWER_CAPPING, &shelf_powercapping_handle);
        (void)dal_get_property_value_byte(shelf_powercapping_handle, PROTERY_SHELF_POWER_CAPPING_ENABLE,
            &power_capping_enabled);
        if (power_capping_enabled == 0) {
            json_object_object_add(extend_jso, RFPROP_POWER_LIMIT_FAILED, NULL);
            return RET_OK;
        }
        (void)dal_get_property_value_byte(shelf_powercapping_handle, PROTERY_SHELF_POWER_CAPPING_STATE, &state);
        if (state == 0) {
            json_object_object_add(extend_jso, RFPROP_POWER_LIMIT_FAILED, json_object_new_boolean(FALSE));
        } else {
            json_object_object_add(extend_jso, RFPROP_POWER_LIMIT_FAILED, json_object_new_boolean(TRUE));
        }
        return RET_OK;
    }
    if (power_capping_enabled != 0) {
        
        (void)dal_get_property_value_uint32(obj_handle, PROTERY_POWERCAP_FAILED, &failed_status);

        
        if (failed_status == 0) {
            json_object_object_add(extend_jso, RFPROP_POWER_LIMIT_FAILED, json_object_new_boolean(FALSE));
        } else {
            json_object_object_add(extend_jso, RFPROP_POWER_LIMIT_FAILED, json_object_new_boolean(TRUE));
        }
    } else {
        json_object_object_add(extend_jso, RFPROP_POWER_LIMIT_FAILED, NULL);
    }

    return VOS_OK;
}

LOCAL void get_bbus_extend_metrics(json_object *obj_huawei)
{
    json_object *bbus_extend = NULL;
    OBJ_HANDLE sys_battery = 0;

    if (obj_huawei == NULL) {
        return;
    }

    gint32 ret = dal_get_object_handle_nth(CLASS_SYS_BATTERY, 0, &sys_battery);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get obj sysbattery fail.\n", __FUNCTION__);
        return;
    }

    bbus_extend = json_object_new_object();
    if (bbus_extend == NULL) {
        return;
    }

    json_object_object_add(obj_huawei, RF_BATTERY_EXTEND_METRICS, bbus_extend);

    rf_add_property_jso_uint32(sys_battery, PROPERTY_SYS_BATTERY_RATED_CAP, RFPROP_BBU_RATED_CAP_WATT_HOUR,
        bbus_extend);

    rf_add_property_jso_uint32(sys_battery, PROPERTY_SYS_BATTERY_REMAIN_CAP, RFPROP_BBU_REMAIN_CAP_WATT_HOUR,
        bbus_extend);

    return;
}

LOCAL void get_power_capping_cfg_info(PROVIDER_PARAS_S* i_paras, json_object* power_extend)
{
    OBJ_HANDLE power_capping_handle = 0;
    guint8 flag = 0;

    gint32 ret = dal_get_object_handle_nth(CLASS_NAME_POWERCAPPING, 0, &power_capping_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: dal_get_object_handle_nth failed, ret = %d", __FUNCTION__, ret);
        return;
    }

    rf_add_property_jso_uint16(power_capping_handle, PROTERY_POWERCAP_TOPVALUE, RF_MAX_POWER_LIMIT, power_extend);
    rf_add_property_jso_uint16(power_capping_handle, PROTERY_POWERCAP_BASEVALUE, RF_MIN_POWER_LIMIT, power_extend);

    ret = dal_get_property_value_byte(power_capping_handle, PROPERTY_POWERCAP_WHEN_STEADY, &flag);
    if (ret != RET_OK || (flag != TRUE && flag != FALSE)) {
        debug_log(DLOG_DEBUG, "%s: dal_get_property_value_uint16 failed, ret = %d", __FUNCTION__, ret);
        return;
    }
    json_object_object_add(power_extend, RF_POWER_LIMIT_WHEN_STEADY, json_object_new_boolean(flag));
    return;
}
LOCAL void __add_cpu_mem_power(json_object* power_extend)
{
    OBJ_HANDLE me_handle = 0;
    guint16 cpu_power = 0;
    guint16 mem_power = 0;
    guint8 arm_enabled = 0;

    (void)dal_get_object_handle_nth(CLASS_ME_INFO, 0, &me_handle);
    (void)dal_get_property_value_uint16(me_handle, PROPERTY_MEINFO_CPU_CUR_POWER, &cpu_power);
    (void)dal_get_property_value_uint16(me_handle, PROPERTY_MEINFO_MEM_CUR_POWER, &mem_power);

    
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_ARM, &arm_enabled);
    if (arm_enabled != SERVICE_STATE_ENABLE) {
        json_object_object_add(power_extend, RF_POWER_CURRENT_CPU_POWER, json_object_new_int(cpu_power));
        json_object_object_add(power_extend, RF_POWER_CURRENT_MEM_POWER, json_object_new_int(mem_power));
    } else {
        json_object_object_add(power_extend, RF_POWER_CURRENT_CPU_POWER, NULL);
        json_object_object_add(power_extend, RF_POWER_CURRENT_MEM_POWER, NULL);
    }
}

LOCAL void get_power_extend_metrics(json_object *obj_huawei, OBJ_HANDLE handle_syspower, PROVIDER_PARAS_S *i_paras)
{
    gint32 ret;
    json_object *power_extend = NULL;
    gint16 time_offset = 0;
    gdouble total_power;
    guint32 begin_time = 0;
    guint32 max_occurred_time = 0;
    gchar strbuf_begin_time[STRING_LEN_MAX] = {0};
    gchar strbuf_max_occurred_time[STRING_LEN_MAX] = {0};

    return_if_expr(obj_huawei == NULL);
    power_extend = json_object_new_object();
    return_if_expr(power_extend == NULL);

    json_object_object_add(obj_huawei, RF_POWER_EXTEND_METRICS, power_extend);

    (void)dal_get_property_value_double(handle_syspower, PROTERY_POWER_CONSUME, &total_power);
    (void)dal_get_property_value_uint32(handle_syspower, PROTERY_RECORD_BEGIN_TIME, &begin_time);
    (void)dal_get_property_value_uint32(handle_syspower, PROTERY_PEAK_TIME, &max_occurred_time);

    ret = dal_get_localtime_offset(&time_offset);
    if (ret == VOS_OK) {
        (void)redfish_fill_format_time(strbuf_begin_time, sizeof(strbuf_begin_time), &begin_time, &time_offset);
        json_object_object_add(power_extend, RF_POWER_STATISTICS_COLLECTED,
            json_object_new_string((const gchar *)strbuf_begin_time));
        (void)redfish_fill_format_time(strbuf_max_occurred_time, sizeof(strbuf_max_occurred_time), &max_occurred_time,
            &time_offset);
        json_object_object_add(power_extend, RF_POWER_MAX_CONSUMED_POWER_OCCURRED,
            json_object_new_string((const gchar *)strbuf_max_occurred_time));
    } else {
        json_object_object_add(power_extend, RF_POWER_STATISTICS_COLLECTED, NULL);
        json_object_object_add(power_extend, RF_POWER_MAX_CONSUMED_POWER_OCCURRED, NULL);
    }
    __add_cpu_mem_power(power_extend);
    json_object_object_add(power_extend, RF_POWER_TOTAL_CONSUMED_POWER, ex_json_object_new_double(total_power, "%.2f"));

    get_power_capping_cfg_info(i_paras, power_extend);
    
    ret = get_power_capping_policy_status(power_extend, i_paras);
    do_val_if_expr(ret != VOS_OK, json_object_object_add(power_extend, RFPROP_POWER_LIMIT_ACTIVATED, NULL));
    
    ret = get_power_capping_failed_status(power_extend, i_paras);
    do_val_if_expr(ret != VOS_OK, json_object_object_add(power_extend, RFPROP_POWER_LIMIT_FAILED, NULL));
    __add_power_limit_status(power_extend);
}


LOCAL void get_powercontrol_set_work_type_supported(json_object *obj_huawei)
{
    guint8 ps_num = 0;
    guint8 active_standby = 0;
    OBJ_HANDLE obj_power = 0;
    guint8 power_protocol = PROTOCOL_INVALID;

    
    gint32 ret = get_power_management_supported(&ps_num);
    return_do_info_if_expr(VOS_OK != ret || 0 == ps_num,
        json_object_object_add(obj_huawei, RFPROP_POWER_WORKMODE_CONFIGURABLE, NULL));

    
    ret = get_active_standby_enabled(&active_standby);
    return_do_info_if_expr(VOS_OK != ret, json_object_object_add(obj_huawei, RFPROP_POWER_WORKMODE_CONFIGURABLE, NULL));

    ret = dal_get_object_handle_nth(CLASS_NAME_PS, 0, &obj_power);
    if (ret == VOS_OK) {
        (void)dal_get_property_value_byte(obj_power, PROTERY_PS_PROTOCAL, &power_protocol);
    }

    
    if (active_standby || power_protocol == PROTOCOL_CANBUS) {
        json_object_object_add(obj_huawei, RFPROP_POWER_WORKMODE_CONFIGURABLE, json_object_new_boolean(TRUE));
    } else {
        json_object_object_add(obj_huawei, RFPROP_POWER_WORKMODE_CONFIGURABLE, json_object_new_boolean(FALSE));
    }

    return;
}


LOCAL void get_powercontrol_msppenble(json_object *obj_huawei)
{
    OBJ_HANDLE obj_handle;
    guchar mspp = G_MAXUINT8;

    gint32 ret = dal_get_object_handle_nth(CLASS_NAME_MSPP, 0, &obj_handle);
    return_do_info_if_expr(ret != VOS_OK, json_object_object_add(obj_huawei, RFPROP_POWER_DEEP_MSPP_MODE, NULL);
        debug_log(DLOG_DEBUG, "%s: get RFPROP_POWER_DEEP_MSPP_MODE fail", __FUNCTION__));

    (void)dal_get_property_value_byte(obj_handle, PROTERY_MSPP_ENABLE, &mspp);

    if (mspp == ENABLE || mspp == DISABLE) {
        json_object_object_add(obj_huawei, RFPROP_POWER_DEEP_MSPP_MODE, json_object_new_boolean(mspp));
    } else {
        json_object_object_add(obj_huawei, RFPROP_POWER_DEEP_MSPP_MODE, NULL);
    }
}


LOCAL gint32 get_powercontrol_high_power_threshold(json_object *obj_huawei, OBJ_HANDLE handle_syspower)
{
    double threshold;
    guint32 ret = dal_get_property_value_double(handle_syspower, PROTERY_HIGH_POWER_THRESHOLD_WATTS, &threshold);
    if (ret != VOS_OK) {
        debug_log(DLOG_DEBUG, "%s: get PROTERY_HIGH_POWER_THRESHOLD_WATTS fail", __FUNCTION__);
        json_object_object_add(obj_huawei, RFPROP_POWER_HIGH_THRESHOLD, NULL);
        return VOS_ERR;
    }
    if (threshold == INVALID_DATA_DWORD) {
        json_object_object_add(obj_huawei, RFPROP_POWER_HIGH_THRESHOLD, NULL);
    } else {
        json_object_object_add(obj_huawei, RFPROP_POWER_HIGH_THRESHOLD, ex_json_object_new_double(threshold, "%.0f"));
    }
    return VOS_OK;
}

LOCAL gint32 get_powercontrol_preset_limit_in_watts(json_object *obj_huawei)
{
    OBJ_HANDLE pwr_cap_handle = 0;
    guint8 support = DISABLE;

    
    gint32 ret = dal_get_func_ability(CLASS_NAME_POWERCAPPING, PROPERTY_POWERCAP_MANUALSETENABLE, &support);
    if (ret != RET_OK || support == DISABLE) {
        return RET_OK;
    }

    ret = dal_get_object_handle_nth(CLASS_NAME_POWERCAPPING, 0, &pwr_cap_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s : Get object handle of %s failed, ret=%d", __FUNCTION__, CLASS_NAME_POWERCAPPING,
            ret);
        json_object_object_add(obj_huawei, RFPROP_POWER_CONTROL_PRESET_LIMIT_VALUE, NULL);
        return RET_ERR;
    }

    rf_add_property_jso_uint16(pwr_cap_handle, PROTERY_POWERCAP_PRESET_LIMITVALUE,
        RFPROP_POWER_CONTROL_PRESET_LIMIT_VALUE, obj_huawei);

    return RET_OK;
}


LOCAL void get_powercontrol_limit_exception_supported(json_object *obj_huawei)
{
    guint8 support = 0;

    gint32 ret = dal_get_func_ability(CLASS_NAME_POWERCAPPING, PROTERY_POWERCAP_FAILACTION_SUPPORT, &support);
    if (ret != RET_OK) {
        json_object_object_add(obj_huawei, RFPROP_POWER_CONTROL_LIMIT_EXCEPTION_SUPPORTED, NULL);
        return;
    }

    if (support) {
        json_object_object_add(obj_huawei, RFPROP_POWER_CONTROL_LIMIT_EXCEPTION_SUPPORTED,
            json_object_new_boolean(TRUE));
    } else {
        json_object_object_add(obj_huawei, RFPROP_POWER_CONTROL_LIMIT_EXCEPTION_SUPPORTED,
            json_object_new_boolean(FALSE));
    }
}


LOCAL void get_chassis_power_powercontrol_oem(const gchar *slot_id, json_object *o_element_jso,
    OBJ_HANDLE handle_syspower, PROVIDER_PARAS_S *i_paras)
{
    gint32 ret;
    guint32 uint32_value;

    json_object *obj_oem = NULL;
    json_object *obj_huawei = NULL;

    obj_oem = json_object_new_object();
    json_object_object_add(o_element_jso, RFPROP_COMMON_OEM, obj_oem);
    return_if_expr(NULL == obj_oem);

    obj_huawei = json_object_new_object();
    json_object_object_add(obj_oem, RFPROP_COMMON_HUAWEI, obj_huawei);
    return_if_expr(NULL == obj_huawei);

    
    get_power_extend_metrics(obj_huawei, handle_syspower, i_paras);
    

    get_bbus_extend_metrics(obj_huawei);

    
    ret = dal_get_property_value_uint32(handle_syspower, PROTERY_HEAT, &uint32_value);
    add_property_value_uint32(obj_huawei, RFPROP_POWER_CONTROL_HEAT, uint32_value, ret);

    
    get_powercontrol_expected_redundancy(slot_id, obj_huawei, handle_syspower);

    
    get_powercontrol_expected_active_psu(slot_id, obj_huawei, handle_syspower);

    
    get_powercontrol_set_work_type_supported(obj_huawei);
    

    
    get_powercontrol_deepsleep(obj_huawei, handle_syspower);

    get_powercontrol_normal_and_redundancy_state(obj_huawei, handle_syspower);
    
    
    get_powercontrol_msppenble(obj_huawei);
    
    ret = get_powercontrol_high_power_threshold(obj_huawei, handle_syspower);
    return_do_info_if_expr(ret != VOS_OK,
        debug_log(DLOG_DEBUG, "%s: the server did not support high power threshold", __FUNCTION__));

    ret = get_powercontrol_preset_limit_in_watts(obj_huawei);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s : Failed to get preset limit in watts, ret=%d", __FUNCTION__, ret);
    }

    get_powercontrol_limit_exception_supported(obj_huawei);
}


#define RF_POWER_CONTROL_LIMIT_EXCEPTION_VALUE_RESET "Reset"
#define RFPROP_POWER_CONTROL_LIMIT_EXCEPTION_VAL_RESET 2

LOCAL void json_add_power_limit(json_object *obj_jso, OBJ_HANDLE handle_powercapping)
{
    json_object *obj_jso_powerlimit = NULL;
    gchar *string_val = NULL;
    guint8 uint8_value = 0xff;
    guint16 uint16_value = 0;
    gint32 ret_dal;
    guint8 software_type = 0xff;
    OBJ_HANDLE shelf_powercapping_handle = 0;

    (void)dal_get_software_type(&software_type);
    if (software_type == MGMT_SOFTWARE_TYPE_PANGEA_MGNT) {
        (void)dfl_get_object_handle(OBJ_NAME_SHELF_POWER_CAPPING, &shelf_powercapping_handle);
    }

    
    obj_jso_powerlimit = json_object_new_object();

    
    (void)dal_get_property_value_byte(handle_powercapping, PROTERY_POWERCAP_ENABLE, &uint8_value);
    if (software_type == MGMT_SOFTWARE_TYPE_PANGEA_MGNT) {
        (void)dal_get_property_value_byte(shelf_powercapping_handle,
            PROTERY_SHELF_POWER_CAPPING_ENABLE, &uint8_value);
    }

    if (RF_POWER_CONTROL_LIMIT_DISABLE == uint8_value) {
        json_object_object_add(obj_jso_powerlimit, RFPROP_POWER_CONTROL_LIMIT_VALUE, NULL);
    } else {
        ret_dal = dal_get_property_value_uint16(handle_powercapping, PROTERY_POWERCAP_LIMITVALUE, &uint16_value);
        if (software_type == MGMT_SOFTWARE_TYPE_PANGEA_MGNT) {
            ret_dal = dal_get_property_value_uint16(shelf_powercapping_handle,
                PROTERY_SHELF_POWER_CAPPING_VALUE, &uint16_value);
        }
        add_property_value_int(obj_jso_powerlimit, RFPROP_POWER_CONTROL_LIMIT_VALUE, (gint32)uint16_value, ret_dal);
    }

    
    ret_dal = dal_get_property_value_byte(handle_powercapping, PROTERY_POWERCAP_FAILACTION, &uint8_value);
    if (RFPROP_POWER_CONTROL_LIMIT_EXCEPTION_VAL_NOACTION == uint8_value) { // 0表示不下电
        string_val = RF_POWER_CONTROL_LIMIT_EXCEPTION_VALUE_NOACTION;
    } else if (RFPROP_POWER_CONTROL_LIMIT_EXCEPTION_VAL_HARDPOWEROFF == uint8_value) { // 1表示下电
        string_val = RF_POWER_CONTROL_LIMIT_EXCEPTION_VALUE_HARDPOWEROFF;
    } else if (RFPROP_POWER_CONTROL_LIMIT_EXCEPTION_VAL_RESET == uint8_value) { // 2表示重启
        string_val = RF_POWER_CONTROL_LIMIT_EXCEPTION_VALUE_RESET;
    }

    add_property_value_string(obj_jso_powerlimit, RFPROP_POWER_CONTROL_LIMIT_EXCEPTION, string_val, ret_dal);

    
    json_object_object_add(obj_jso, RFPROP_POWER_CONTROL_LIMIT, obj_jso_powerlimit);
}


LOCAL void get_chassis_power_powercontrol_attributes(const gchar *slot_str, json_object *o_result_jso,
    OBJ_HANDLE handle_syspower, OBJ_HANDLE handle_powercapping, gint32 count, PROVIDER_PARAS_S *i_paras)
{
    gchar string_value[POWER_STR_MAX_LEN] = {0};
    guint16 uint16_value = 0;

    gint32 ret;
    
    guchar power_capping_support = DISABLE;
    

    return_if_fail((NULL != slot_str) && (NULL != o_result_jso));

    
    json_object *obj_jso = json_object_new_object();
    
    

    int iRet = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1, "%s%s%s/%d", RF_CHASSIS_UI,
        slot_str, RFPROP_CHASSIS_POWERPCONTROL, count);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    json_object_object_add(obj_jso, RFPROP_POWER_CONTROL_ODATA_ID, json_object_new_string((const gchar *)string_value));

    

    
    (void)memset_s(string_value, sizeof(string_value), 0, sizeof(string_value));
    (void)snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1, "%d", count);
    json_object_object_add(obj_jso, RFPROP_POWER_CONTROL_MEMBER_ID,
        json_object_new_string((const gchar *)string_value));

    
    (void)memset_s(string_value, sizeof(string_value), 0, sizeof(string_value));
    iRet = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1, "%s %d",
        RF_POWER_CONTROL_NAME_VALUE, count + 1);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    json_object_object_add(obj_jso, RFPROP_POWER_CONTROL_NAME, json_object_new_string((const gchar *)string_value));

    
    
    rf_add_property_jso_uint32(handle_syspower, PROTERY_PSTOTAL_POWER, RFPROP_POWER_CONTROL_POWER_CONSUME, obj_jso);
    

    
    json_object *obj_jso_powermetrics = json_object_new_object();

    
    gint32 ret_dal = dal_get_property_value_uint16(handle_syspower, PROTERY_MIN_VALUE, &uint16_value);
    add_property_value_int(obj_jso_powermetrics, RFPROP_POWER_CONTROL_METRICS_MIN_CONSUME, (gint32)uint16_value,
        ret_dal);

    
    ret_dal = dal_get_property_value_uint16(handle_syspower, PROTERY_PEAK_VALUE, &uint16_value);
    add_property_value_int(obj_jso_powermetrics, RFPROP_POWER_CONTROL_METRICS_MAX_CONSUME, (gint32)uint16_value,
        ret_dal);

    
    ret_dal = dal_get_property_value_uint16(handle_syspower, PROTERY_AVERAGE_VALUE, &uint16_value);
    add_property_value_int(obj_jso_powermetrics, RFPROP_POWER_CONTROL_METRICS_AVERAGE_CONSUME, (gint32)uint16_value,
        ret_dal);

    
    json_object_object_add(obj_jso, RFPROP_POWER_CONTROL_METRICS, obj_jso_powermetrics);

    
    
    ret = dal_get_func_ability(CLASS_NAME_POWERCAPPING, PROPERTY_POWERCAP_MANUALSETENABLE,
        &power_capping_support);
    
    if ((VOS_OK != ret) || (DISABLE == power_capping_support)) {
        
        json_object_object_add(obj_jso, RFPROP_POWER_CONTROL_LIMIT, NULL);
    } else {
        json_add_power_limit(obj_jso, handle_powercapping);
    }

    
    get_chassis_power_powercontrol_oem(slot_str, obj_jso, handle_syspower, i_paras);

    
    json_object_array_add(o_result_jso, obj_jso);
}


gint32 check_enclosure_component_type(OBJ_HANDLE chassis_handle, gboolean is_support_node_enclosure)
{
    guchar component_type = 0;
    guchar board_type = 0;
    gchar               class_name[MAX_NAME_SIZE] = {0};

    if (dal_check_if_vsmm_supported()) {
        return RET_OK;
    }

    (void)dfl_get_class_name(chassis_handle, class_name, sizeof(class_name));
    if (g_strcmp0(class_name, CLASS_COMPONENT) != 0) {
        debug_log(DLOG_DEBUG, "[%s] class name %s is not Component.", __FUNCTION__, class_name);
        return RET_ERR;
    }

    gint32 ret = dal_get_property_value_byte(chassis_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &component_type);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "cann't get property value for %s, ret is %d", PROPERTY_COMPONENT_DEVICE_TYPE, ret));

    (void)dal_rf_get_board_type(&board_type);

    
    
    if (is_support_node_enclosure) {
        if (is_node_enc_rsc(chassis_handle, &board_type, NULL, &component_type) ||
            check_pangea_node_to_enc(chassis_handle)) {
            return RET_OK;
        }
    }
    

    
    if (((BOARD_MM == board_type) && (COMPONENT_TYPE_CHASSIS_BACKPLANE == component_type)) ||
        ((BOARD_MM != board_type) && (COMPONENT_TYPE_MAINBOARD == component_type))) {
        ret = VOS_OK;
    } else {
        ret = VOS_ERR;
    }

    return ret;
}


LOCAL gint32 get_chassis_power_powercontrol(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    GSList *obj_list_syspower = NULL;
    GSList *obj_node_syspower = NULL;
    GSList *obj_list_powercapping = NULL;
    GSList *obj_node_powercapping = NULL;
    gint32 count = 0;
    gchar               slot_str[MAX_RSC_ID_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    gint32 ret = check_enclosure_component_type(i_paras->obj_handle, TRUE);
    
    return_val_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR);

    ret = rf_gen_chassis_component_id(NULL, i_paras->obj_handle, slot_str, MAX_RSC_ID_LEN);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:gen chassis component id failed", __FUNCTION__));

    

    
    ret = dfl_get_object_list(CLASS_NAME_AMMETER, &obj_list_syspower);
    if (VOS_OK != ret) {
        debug_log(DLOG_DEBUG, "%s, %d: get obj_list for CLASS_NAME_AMMETER fail.\n", __FUNCTION__, __LINE__);
    }

    ret = dfl_get_object_list(CLASS_NAME_POWERCAPPING, &obj_list_powercapping);
    if (VOS_OK != ret) {
        debug_log(DLOG_DEBUG, "%s, %d: get obj_list for CLASS_NAME_POWERCAPPING fail.\n", __FUNCTION__, __LINE__);
    }

    
    *o_result_jso = json_object_new_array();

    for (obj_node_syspower = obj_list_syspower, obj_node_powercapping = obj_list_powercapping;
        obj_node_syspower && obj_node_powercapping;
        obj_node_syspower = obj_node_syspower->next, obj_node_powercapping = obj_node_powercapping->next) {
        get_chassis_power_powercontrol_attributes(slot_str, *o_result_jso, (OBJ_HANDLE)obj_node_syspower->data,
            (OBJ_HANDLE)obj_node_powercapping->data, count, i_paras);
        count++;
    }

    do_val_if_expr(NULL != obj_list_syspower, g_slist_free(obj_list_syspower));
    do_val_if_expr(NULL != obj_list_powercapping, g_slist_free(obj_list_powercapping));

    return HTTP_OK;
}

typedef enum  {
    POWER_LIMIT_INVALIDS,
    POWER_LIMIT_NODE, // 单节点功率封顶模式
    POWER_LIMIT_MSPP, // 紧急功率封顶模式
    POWER_LIMIT_CHASS_ACTIVE, // 整框功率封顶模式且触发封顶
    POWER_LIMIT_CHASS_INACTIVE,  // 整框功率封顶模式且未触发封顶
    POWER_LIMIT_MAX
}POWER_LIMIT_STATUS;


LOCAL POWER_LIMIT_STATUS __get_power_limit_status(void)
{
    OBJ_HANDLE shelf_handle = 0;
    OBJ_HANDLE mspp_handle = 0;
    guint8 shelf_enable;
    guint8 active_status;
    guint8 mspp_enable;

    // 获取整框封顶状态
    gint32 ret = dal_get_object_handle_nth(CLASS_NAME_SHELF_POWER_CAPPING, 0, &shelf_handle);
    if (ret != RET_OK) {
        return POWER_LIMIT_INVALIDS;
    }
    ret = dal_get_property_value_byte(shelf_handle, PROTERY_SHELF_POWER_CAPPING_ENABLE, &shelf_enable);
    if (ret != RET_OK) {
        return POWER_LIMIT_INVALIDS;
    }
    ret = dal_get_property_value_byte(shelf_handle, PROTERY_SHELF_POWER_CAPPING_ACTIVE_STATE, &active_status);
    if (ret != RET_OK) {
        return POWER_LIMIT_INVALIDS;
    }
    if (shelf_enable == ENABLE) {
        // 如果整框功率封顶使能，再判断有没有触发封顶
        if (active_status == ENABLE) {
            return POWER_LIMIT_CHASS_ACTIVE;
        } else {
            return POWER_LIMIT_CHASS_INACTIVE;
        }
    }

    // 获取紧急功率封顶状态
    ret = dal_get_object_handle_nth(CLASS_NAME_MSPP, 0, &mspp_handle);
    if (ret != RET_OK) {
        return POWER_LIMIT_INVALIDS;
    }
    ret = dal_get_property_value_byte(mspp_handle, PROTERY_MSPP_ENABLE, &mspp_enable);
    if (ret != RET_OK) {
        return POWER_LIMIT_INVALIDS;
    }

    if (mspp_enable == ENABLE) {
        return POWER_LIMIT_MSPP;
    }
    return POWER_LIMIT_NODE;
}

LOCAL void __add_power_limit_status(json_object* extend_jso)
{
    const gchar* POWER_LIMIT_STATUS_STR[POWER_LIMIT_MAX] = {
        NULL,
        "NodePowerLimit",
        "MSPP",
        "ChassisPowerLimitAcitve",
        "ChassisPowerLimitInactive"
    };

    // 非X6000v6产品反回, web识别到null，走原有逻辑处理。
    if (dal_is_aggregation_management_chassis_product() != TRUE) {
        json_object_object_add(extend_jso, RFPROP_POWER_LIMIT_STATUS, NULL);
        return;
    }

    POWER_LIMIT_STATUS power_limit_status;
    power_limit_status = __get_power_limit_status();
    if (power_limit_status >= POWER_LIMIT_MAX || power_limit_status < 0x01) {
        json_object_object_add(extend_jso, RFPROP_POWER_LIMIT_STATUS, NULL);
    } else {
        json_object_object_add(extend_jso, RFPROP_POWER_LIMIT_STATUS,
                               json_object_new_string(POWER_LIMIT_STATUS_STR[power_limit_status]));
    }
    return;
}

LOCAL gboolean __is_support_power_limit_config(void)
{
    if (dal_is_aggregation_management_chassis_product() != TRUE) {
        return TRUE;
    }

    // 获取功率封顶模式。紧急功率封顶 或者 整框功率封顶开启并激活情况下 不支持配置节点功率封顶
    POWER_LIMIT_STATUS power_limit_status = __get_power_limit_status();
    if (power_limit_status == POWER_LIMIT_MSPP || power_limit_status == POWER_LIMIT_CHASS_ACTIVE) {
        return FALSE;
    }
    return TRUE;
}


LOCAL gint32 set_powercontrol_powerlimit_enable(SET_POWER_LIMIT_INFO_S *power_limit_info, guint8 enable)
{
    gint32 ret;
    guint8 uint8_value = 0;

    GSList *input_list = NULL;
    json_object *obj_jso = NULL;
    guint8 software_type = 0xff;
    OBJ_HANDLE shelf_powercapping_handle = 0;

    (void)dfl_get_object_handle(OBJ_NAME_SHELF_POWER_CAPPING, &shelf_powercapping_handle);
    (void)dal_get_software_type(&software_type);

    if (__is_support_power_limit_config() == FALSE) {
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, RFPROP_POWER_CONTROL_LIMIT_VALUE, &obj_jso,
            RFPROP_POWER_CONTROL_LIMIT_VALUE);
        json_object_array_add(power_limit_info->o_message_jso, obj_jso);
        return HTTP_NOT_IMPLEMENTED;
    }

    
    (void)dal_get_property_value_byte(power_limit_info->handle, PROTERY_POWERCAP_ENABLE, &uint8_value);
    if (software_type == MGMT_SOFTWARE_TYPE_PANGEA_MGNT) {
        (void)dal_get_property_value_byte(shelf_powercapping_handle, PROTERY_SHELF_POWER_CAPPING_ENABLE, &uint8_value);
    }

    if (enable == uint8_value) {
        
        
        do_val_if_expr(RF_POWER_CONTROL_LIMIT_DISABLE == enable, (power_limit_info->set_succ_num)++);
        return VOS_OK;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(enable));

    
    
    if (software_type == MGMT_SOFTWARE_TYPE_PANGEA_MGNT) {
        ret = uip_call_class_method_redfish(power_limit_info->i_paras->is_from_webui,
            power_limit_info->i_paras->user_name, power_limit_info->i_paras->client,
            shelf_powercapping_handle, CLASS_NAME_SHELF_POWER_CAPPING, METHOD_SET_SHELF_POWER_CAPPING_ENABLE,
            AUTH_DISABLE, power_limit_info->i_paras->user_role_privilege, input_list, NULL);
    } else {
        ret = uip_call_class_method_redfish(power_limit_info->i_paras->is_from_webui,
            power_limit_info->i_paras->user_name, power_limit_info->i_paras->client,
            power_limit_info->handle, CLASS_NAME_POWERCAPPING, METHOD_POWERCAP_ENABLE,
            AUTH_ENABLE, power_limit_info->i_paras->user_role_privilege, input_list, NULL);
    }
    

    
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
            do_val_if_expr(RF_POWER_CONTROL_LIMIT_DISABLE == enable, (power_limit_info->set_succ_num)++);
            return VOS_OK;

        default: 
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &obj_jso);
            json_object_array_add(power_limit_info->o_message_jso, obj_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}

LOCAL void insert_fail_action(const gchar *str_limitaction, GSList **input_list)
{
    
    if (g_strcmp0(str_limitaction, RF_POWER_CONTROL_LIMIT_EXCEPTION_VALUE_NOACTION) == 0) {
        *input_list = g_slist_append(*input_list, g_variant_new_byte(0));
    } else if (g_strcmp0(str_limitaction, RF_POWER_CONTROL_LIMIT_EXCEPTION_VALUE_HARDPOWEROFF) == 0) {
        *input_list = g_slist_append(*input_list, g_variant_new_byte(1));
    } else if (g_strcmp0(str_limitaction, RF_POWER_CONTROL_LIMIT_EXCEPTION_VALUE_RESET) == 0) {
        *input_list = g_slist_append(*input_list, g_variant_new_byte(RFPROP_POWER_CONTROL_LIMIT_EXCEPTION_VAL_RESET));
    }
    return;
}


LOCAL gint32 set_powercontrol_property_limit(SET_POWER_LIMIT_INFO_S *power_limit_info, guint16 uint_limit)
{
    gint32 ret;
    gchar str_value[POWER_STR_MAX_LEN] = {0};
    gchar str_value2[POWER_STR_MAX_LEN] = {0};

    GSList *input_list = NULL;
    json_object *obj_jso = NULL;
    guint8 software_type = 0xff;
    OBJ_HANDLE shelf_powercapping_handle = 0;

    if (__is_support_power_limit_config() == FALSE) {
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, RFPROP_POWER_CONTROL_LIMIT_VALUE, &obj_jso,
            RFPROP_POWER_CONTROL_LIMIT_VALUE);
        json_object_array_add(power_limit_info->o_message_jso, obj_jso);
        return HTTP_NOT_IMPLEMENTED;
    }

    (void)dal_get_software_type(&software_type);
    input_list = g_slist_append(input_list, g_variant_new_uint16(uint_limit));

    
    if (software_type == MGMT_SOFTWARE_TYPE_PANGEA_MGNT) {
        (void)dfl_get_object_handle(OBJ_NAME_SHELF_POWER_CAPPING, &shelf_powercapping_handle);
        ret = uip_call_class_method_redfish(power_limit_info->i_paras->is_from_webui,
            power_limit_info->i_paras->user_name, power_limit_info->i_paras->client,
            shelf_powercapping_handle, CLASS_NAME_SHELF_POWER_CAPPING, METHOD_SET_SHELF_POWER_CAPPING_VALUE,
            AUTH_DISABLE, power_limit_info->i_paras->user_role_privilege, input_list, NULL);
    } else {
        ret = uip_call_class_method_redfish(power_limit_info->i_paras->is_from_webui,
            power_limit_info->i_paras->user_name, power_limit_info->i_paras->client,
            power_limit_info->handle, CLASS_NAME_POWERCAPPING, METHOD_POWERCAP_LIMITVALUE,
            AUTH_ENABLE, power_limit_info->i_paras->user_role_privilege, input_list, NULL);
    }

    
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
            (power_limit_info->set_succ_num)++;
            return VOS_OK;

        case USER_PARAMETER_OUT_OF_RANGE:
            
            (void)snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1,
                "PowerControl/%d/PowerLimit/LimitInWatts", power_limit_info->arr_index);
            
            (void)snprintf_s(str_value2, sizeof(str_value2), sizeof(str_value2) - 1, "%u", uint_limit);
            (void)create_message_info(MSGID_PROP_NOT_IN_LIST, (const gchar *)str_value, &obj_jso,
                (const gchar *)str_value2, (const gchar *)str_value);
            json_object_array_add(power_limit_info->o_message_jso, obj_jso);
            return VOS_OK;

        default: 
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &obj_jso);
            json_object_array_add(power_limit_info->o_message_jso, obj_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 set_powercontrol_property_limitaction(SET_POWER_LIMIT_INFO_S *power_limit_info,
    const gchar *str_limitaction)
{
    guint8 arm_enabled = 0;
    gchar str_value[POWER_STR_MAX_LEN] = {0};

    GSList *input_list = NULL;
    json_object *obj_jso = NULL;
    guint8 support = POWER_CAP_SUPPORT;

    if (str_limitaction == NULL) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &obj_jso);
        json_object_array_add(power_limit_info->o_message_jso, obj_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    (void)snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1, "PowerControl/%d/PowerLimit/LimitException",
        power_limit_info->arr_index);

    (void)dal_get_func_ability(CLASS_NAME_POWERCAPPING, PROTERY_POWERCAP_FAILACTION_SUPPORT, &support); // 获取失败, 默认为支持
    
    return_val_do_info_if_expr(support == POWER_CAP_NOT_SUPPORT, VOS_OK,
        (void)create_message_info(MSGID_ACT_NOT_SUPPORTED, (const gchar *)str_value, &obj_jso, str_limitaction);
        json_object_array_add(power_limit_info->o_message_jso, obj_jso));
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_ARM, &arm_enabled);

    
    if (g_strcmp0(str_limitaction, RF_POWER_CONTROL_LIMIT_EXCEPTION_VALUE_NOACTION) &&
        g_strcmp0(str_limitaction, RF_POWER_CONTROL_LIMIT_EXCEPTION_VALUE_HARDPOWEROFF) &&
        !((arm_enabled == SERVICE_STATE_ENABLE) &&
        (g_strcmp0(str_limitaction, RF_POWER_CONTROL_LIMIT_EXCEPTION_VALUE_RESET) == 0))) {
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, (const gchar *)str_value, &obj_jso, str_limitaction,
            (const gchar *)str_value);
        json_object_array_add(power_limit_info->o_message_jso, obj_jso);
        return VOS_OK;
    }
    insert_fail_action(str_limitaction, &input_list);
 
    
    
    gint32 ret = uip_call_class_method_redfish(power_limit_info->i_paras->is_from_webui, power_limit_info->i_paras->
        user_name, power_limit_info->i_paras->client, power_limit_info->handle, CLASS_NAME_POWERCAPPING,
        METHOD_POWERCAP_FAILACTION, AUTH_ENABLE, power_limit_info->i_paras->user_role_privilege, input_list, NULL);
    
 
    
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
            (power_limit_info->set_succ_num)++;
            return VOS_OK;

        case COMP_CODE_STATUS_INVALID:
        case PSM_ERROR_NOT_SUPPORT_CAPPING:
        case COMP_CODE_UNKNOWN:
            (void)create_message_info(MSGID_ACT_NOT_SUPPORTED, str_value, &obj_jso, str_limitaction, str_value);
            json_object_array_add(power_limit_info->o_message_jso, obj_jso);
            return VOS_OK;

        default: 
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &obj_jso);
            json_object_array_add(power_limit_info->o_message_jso, obj_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 check_redundancy_odata_id(SET_POWER_LIMIT_INFO_S *patch_info, const gchar *slot_str,
    const gchar *odata_id_str, guint8 *redundancy_index)
{
    gint32 ret;
    guint8 i;
    gchar str_value[POWER_STR_MAX_LEN] = {0};

    return_val_if_expr(slot_str == NULL || odata_id_str == NULL || redundancy_index == NULL, VOS_ERR);

    json_object *obj_msg = NULL;

    for (i = 0; i < RF_POWER_REDUNDANCY_ARRAY_LEN; i++) {
        ret = snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1, RFPROP_REDUNDANCY_ODATA_ID_VALUE,
            slot_str, i);
        return_val_if_expr(0 >= ret, VOS_ERR);

        return_val_do_info_if_expr(!g_strcmp0(str_value, odata_id_str), VOS_OK, ((*redundancy_index) = i));
    }

    ret = snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1, REDUNDANCY_PROP_PATH, patch_info->arr_index);
    return_val_if_expr(0 >= ret, VOS_ERR);

    (void)create_message_info(MSGID_POWER_INVALID_ODATA_ID, (const gchar *)str_value, &obj_msg,
        (const gchar *)str_value, odata_id_str);
    json_object_array_add(patch_info->o_message_jso, obj_msg);

    return VOS_ERR;
}


LOCAL gboolean is_valid_psu_odata_id(SET_POWER_LIMIT_INFO_S* patch_info, const gchar* slot_str,
    const gchar* odata_id_str, guint8* ps_ids, guint8 ps_idx, guint8 ps_num)
{
    gint32 ret;
    guint8 j;
    gchar* pos = NULL;
    gchar str_value[POWER_STR_MAX_LEN] = {0};
    json_object* obj_msg = NULL;

    for (j = 0; j < ps_num; j++) {
        ret = sprintf_s(str_value, sizeof(str_value), RFPROP_POWER_SUPPLY_ODATA_ID_VALUE, slot_str, j);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: sprintf_s failed, ret = %d", __FUNCTION__, ret);
            return FALSE;
        }

        if (g_strcmp0(str_value, odata_id_str) == 0) {
            pos = g_strrstr(odata_id_str, "/");
            if (pos == NULL) {
                return FALSE;
            }
            ret = vos_str2int(pos + 1, 10, &ps_ids[ps_idx], NUM_TPYE_UCHAR);
            if (ret != VOS_OK) {
                debug_log(DLOG_ERROR, "%s: vos_str2int failed, ret = %d", __FUNCTION__, ret);
                return FALSE;
            }
            break;
        }
    }

    if (j == ps_num) {
        (void)dal_get_property_value_string(patch_info->i_paras->obj_handle,
            g_chassis_power_provider[patch_info->i_paras->index].pme_prop_name, str_value, sizeof(str_value));
        if (g_strcmp0((const char*)str_value, RFPROP_POWER_CONTROL) == 0) {
            ret = sprintf_s(str_value, sizeof(str_value), ACTIVE_PSU_ODATAID_PROP_PATH, patch_info->arr_index, ps_idx);
        } else {
            ret = sprintf_s(str_value, sizeof(str_value), RFPROP_REDUNDANCY_SET_ODATA_PATH,
                patch_info->arr_index, ps_idx);
        }
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: sprintf_s failed, ret = %d", __FUNCTION__, ret);
            return FALSE;
        }
        (void)create_message_info(MSGID_POWER_INVALID_ODATA_ID, (const gchar *)str_value, &obj_msg,
            (gchar *)str_value, odata_id_str);
        json_object_array_add(patch_info->o_message_jso, obj_msg);
        return FALSE;
    }

    return TRUE;
}


LOCAL gint32 check_active_psu_odata_id(SET_POWER_LIMIT_INFO_S *patch_info, const gchar *slot_str,
    json_object *obj_active_psu, guint8 ps_num, guint8 *ps_ids)
{
    guint8 i, j;
    gint32 arr_len;
    gchar str_value[POWER_STR_MAX_LEN] = {0};
    const gchar *str = NULL;
    json_object *elem = NULL;
    json_object *elem_odataid = NULL;
    json_bool bool_jso = FALSE;

    json_object *obj_msg = NULL;

    arr_len = json_object_array_length(obj_active_psu);
    return_val_do_info_if_expr((guint8)arr_len > ps_num, VOS_ERR,
        (void)snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1, ACTIVE_PSU_PROP_PATH,
        patch_info->arr_index);
        (void)create_message_info(MSGID_POWER_ACTIVE_PS_EXCEEDS_NUM, str_value, &obj_msg, str_value);
        json_object_array_add(patch_info->o_message_jso, obj_msg));

    for (i = 0; i < (guint8)arr_len; i++) {
        elem = json_object_array_get_idx(obj_active_psu, i);
        return_val_if_expr(NULL == elem, VOS_ERR);

        
        return_val_do_info_if_expr((0 == json_object_object_length(elem) && NULL == elem->_userdata), VOS_ERR,
            (void)snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1,
            "PowerControl/%d/Oem/Huawei/ExpectedActivePSU/%u", patch_info->arr_index, i);
            (void)create_message_info(MSGID_ITEM_PROP_FEW, str_value, &obj_msg, str_value);
            (void)json_object_array_add(patch_info->o_message_jso, obj_msg));
        

        bool_jso = json_object_object_get_ex(elem, RFPROP_ODATA_ID, &elem_odataid);
        if (!bool_jso) {
            continue;
        }

        str = dal_json_object_get_str(elem_odataid);
        return_val_if_expr(NULL == str, VOS_ERR);
        if (!is_valid_psu_odata_id(patch_info, slot_str, str, ps_ids, i, ps_num)) {
            return VOS_ERR;
        }
    }

    for (i = 0; i < (guint8)arr_len; i++) {
        for (j = i + 1; j < (guint8)arr_len; j++) {
            return_val_do_info_if_expr(ps_ids[j] == ps_ids[i] && ps_ids[i] != 0xff, VOS_ERR,
                (void)snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1, ACTIVE_PSU_PROP_PATH,
                patch_info->arr_index);
                (void)create_message_info(MSGID_POWER_DUP_ACTIVE_PSU, str_value, &obj_msg, str_value);
                json_object_array_add(patch_info->o_message_jso, obj_msg));
        }
    }

    return VOS_OK;
}


LOCAL void check_redundancy_activepsu_deletion(json_object *obj_redundancy, json_object *obj_huawei,
    gboolean *redundancy_deletion, gboolean *activepsu_deletion)
{
    json_bool bool_jso = FALSE;
    GSList *prop_list = NULL;
    GSList *prop_node = NULL;

    
    if (NULL != obj_redundancy) {
        bool_jso = json_object_get_user_data(obj_redundancy, (void **)&prop_list);
        return_if_expr(!bool_jso);

        for (prop_node = prop_list; prop_node; prop_node = prop_node->next) {
            if (0 == g_strcmp0(RFPROP_ODATA_ID, (const gchar *)prop_node->data)) {
                *redundancy_deletion = TRUE;
                break;
            }
        }
    }

    
    if (NULL != obj_huawei) {
        prop_list = NULL;
        prop_node = NULL;
        bool_jso = json_object_get_user_data(obj_huawei, (void **)&prop_list);
        return_if_expr(!bool_jso);

        for (prop_node = prop_list; prop_node; prop_node = prop_node->next) {
            if (0 == g_strcmp0(RFPROP_POWER_EXPECTED_ACTIVE_PSU, (const gchar *)prop_node->data)) {
                *activepsu_deletion = TRUE;
                break;
            }
        }
    }
}

LOCAL gint32 check_set_work_mode_common_ret(SET_POWER_LIMIT_INFO_S *patch_info, gint32 ret, gint32 *is_implemented)
{
    gint ret_local;
    gchar str_value_redundancy[POWER_STR_MAX_LEN] = {0};
    gchar str_value_nar[POWER_STR_MAX_LEN] = {0};

    
    ret_local = snprintf_s(str_value_redundancy, sizeof(str_value_redundancy), sizeof(str_value_redundancy) - 1,
        REDUNDANCY_PROP_PATH, patch_info->arr_index);
    return_val_if_expr(ret_local <= 0, VOS_ERR);
    ret_local = snprintf_s(str_value_nar, sizeof(str_value_nar), sizeof(str_value_nar) - 1, DEEP_NAR_PROP_PATH,
        patch_info->arr_index);
    return_val_if_expr(ret_local <= 0, VOS_ERR);

    json_object *obj_msg = NULL;
    switch (ret) {
        case VOS_OK:
        case RFERR_SUCCESS:
            (patch_info->set_succ_num)++;
            return VOS_OK;

        
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, &obj_msg);
            break;

        
        case RFERR_NO_RESOURCE:
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &obj_msg, patch_info->i_paras->uri);
            break;

        case ERR_NAR_NOT_SUPPORT:
            (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, (const gchar *)str_value_nar, &obj_msg,
                (const gchar *)str_value_nar);
            *is_implemented = HTTP_NOT_IMPLEMENTED;
            break;

        case ERR_NAR_BALANCE_CONFLICT:
            (void)create_message_info(MSGID_FAN_ATTRIBUTE_CONFLICT, NULL, &obj_msg, (const gchar *)str_value_nar,
                ENABLED_STRING, (const gchar *)str_value_redundancy);
            break;

        default:

            return VOS_ERR;
    }

    (void)json_object_array_add(patch_info->o_message_jso, obj_msg);
    return VOS_OK;
}

LOCAL gint32 check_set_work_mode_ret(SET_POWER_LIMIT_INFO_S *patch_info, gint32 ret, gint32 *is_implemented)
{
    gint ret_local;
    gchar str_value[POWER_STR_MAX_LEN] = {0};
    gchar str_value2[POWER_STR_MAX_LEN] = {0};

    
    ret_local =
        snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1, REDUNDANCY_PROP_PATH, patch_info->arr_index);
    return_val_if_expr(ret_local <= 0, VOS_ERR);
    ret_local =
        snprintf_s(str_value2, sizeof(str_value2), sizeof(str_value2) - 1, ACTIVE_PSU_PROP_PATH, patch_info->arr_index);
    return_val_if_expr(ret_local <= 0, VOS_ERR);

    
    ret_local = check_set_work_mode_common_ret(patch_info, ret, is_implemented);
    return_val_if_expr(ret_local == VOS_OK, ret_local);

    json_object *obj_msg = NULL;
    switch (ret) {
        case ERR_ACTIVE_PS_ONT_PST:
            (void)create_message_info(MSGID_POWER_ACTIVE_PS_NOT_PST, (const gchar *)str_value2, &obj_msg,
                (const gchar *)str_value2, (const gchar *)str_value);
            break;

        case ERR_NO_STANDBY_PS_PST:
            (void)create_message_info(MSGID_POWER_NO_STANDBY_PS_PST, (const gchar *)str_value2, &obj_msg,
                (const gchar *)str_value2, (const gchar *)str_value);
            break;

        case ERR_ACTIVE_PS_ONT_HLS:
            (void)create_message_info(MSGID_POWER_ACTIVE_PS_NOT_HLS, (const gchar *)str_value2, &obj_msg,
                (const gchar *)str_value2, (const gchar *)str_value);
            break;

        case ERR_CURRENT_POWER_HIGN:
            (void)create_message_info(MSGID_POWER_CURRENT_POWER_HIGN, (const gchar *)str_value2, &obj_msg,
                (const gchar *)str_value2, (const gchar *)str_value);
            break;

        case ERR_ACTIVE_PS_NUM_NE:
            (void)create_message_info(MSGID_POWER_ACTIVE_PS_NUM_NE, (const gchar *)str_value2, &obj_msg,
                (const gchar *)str_value2, (const gchar *)str_value);
            break;

        case ERR_PS_NOT_SUPPORT:
            (void)create_message_info(MSGID_POWER_PS_NOT_SUPPORT, (const gchar *)str_value2, &obj_msg,
                (const gchar *)str_value2, (const gchar *)str_value);
            break;

        case ERR_ACTIVE_STANDBY_MODE_UNSUPPORT:
            (void)create_message_info(MSGID_POWER_ACTIVE_STANDBY_MODE_UNSUPPORT, (const gchar *)str_value,
                &obj_msg, (const gchar *)str_value);
            break;

        case ERR_NAR_VOLT_ERROR:
            (void)create_message_info(MSGID_POWER_NAR_VOLTAGE_NOT_MATCH, (const gchar *)str_value, &obj_msg,
                (const gchar *)str_value);
            break;

        case ERR_NAR_PS_NOT_SUPPORT:
            (void)create_message_info(MSGID_POWER_PS_NOT_SUPPORT_NAR, NULL, &obj_msg);
            break;

        default: 
            debug_log(DLOG_ERROR, "%s: ret = %d", __FUNCTION__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &obj_msg);
            break;
    }

    json_object_array_add(patch_info->o_message_jso, obj_msg);
    return VOS_OK;
}


LOCAL guint8 get_powercontrol_input_normal_and_redundancy(json_object *obj_nar)
{
    gint32 ret = VOS_OK;
    guint8 uint8_value = 0;
    OBJ_HANDLE obj_handle = 0;

    if (obj_nar != NULL) {
        return 0 == g_strcmp0(dal_json_object_get_str(obj_nar), DISABLED_STRING) ? 0 : 1;
    } else {
        ret = dal_get_object_handle_nth(CLASS_NAME_AMMETER, 0, &obj_handle);
        return_val_do_info_if_expr(ret != VOS_OK, uint8_value,
            debug_log(DLOG_ERROR, "%s: get obj_list for OBJ_NAME_PME_SERVICECONFIG fail", __FUNCTION__));

        ret = dal_get_property_value_byte(obj_handle, PROTERY_SYSPOWER_NORMAL_AND_REDUNDANCY_ENABLE, &uint8_value);
        return_val_do_info_if_expr(ret != VOS_OK, uint8_value,
            debug_log(DLOG_ERROR, "%s: get PROTERY_SYSPOWER_NORMAL_AND_REDUNDANCY_ENABLE fail", __FUNCTION__));
        return uint8_value;
    }
}


LOCAL gint32 get_powercontrol_predictedstatus_input_list(SET_POWER_LIMIT_INFO_S *patch_info, GSList **input_list,
    json_object *obj_nar, json_object *obj_active_psu, guint8 redundancy_index, const gchar *slot_str, guint8 ps_num)
{
    gint32 ret;
    guint8 nar_value;
    gint32 arr_len;
    guint8 *ps_ids = NULL;
    gint32 i = 0;
    json_object *obj_msg = NULL;

    nar_value = get_powercontrol_input_normal_and_redundancy(obj_nar);

    
    if (redundancy_index == 0) {
        *input_list = g_slist_append(*input_list, (gpointer)g_variant_new_byte(RF_POWER_CONTROL_PSU_LOAD_BALANCE));
        *input_list = g_slist_append(*input_list, (gpointer)g_variant_new_byte(nar_value));
    } else {
        
        arr_len = json_object_array_length(obj_active_psu);
        return_val_do_info_if_expr(arr_len == 0, VOS_ERR, debug_log(DLOG_ERROR, "%s: arr_len is 0", __FUNCTION__));
        ps_ids = (guint8 *)g_malloc(sizeof(guint8) * arr_len);
        return_val_do_info_if_expr(ps_ids == NULL, VOS_ERR, debug_log(DLOG_ERROR, "%s: g_malloc0 fail", __FUNCTION__);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &obj_msg);
            json_object_array_add(patch_info->o_message_jso, obj_msg));

        (void)memset_s(ps_ids, arr_len, 0xff, arr_len);
        ret = check_active_psu_odata_id(patch_info, slot_str, obj_active_psu, ps_num, ps_ids);
        return_val_do_info_if_expr(ret != VOS_OK, ret, g_free(ps_ids);
            debug_log(DLOG_ERROR, "%s: check_active_psu_odata_id fail", __FUNCTION__));

        *input_list = g_slist_append(*input_list, (gpointer)g_variant_new_byte(RF_POWER_CONTROL_PSU_ACTIVE_STANDBY));
        *input_list = g_slist_append(*input_list, (gpointer)g_variant_new_byte(nar_value));

        for (i = 0; i < arr_len; i++) {
            if (ps_ids[i] == 0xff) {
                continue;
            }
            *input_list = g_slist_append(*input_list, (gpointer)g_variant_new_byte(ps_ids[i]));
        }

        g_free(ps_ids);
    }
    return VOS_OK;
}

LOCAL gint32 set_power_powercontrol_predictedstatus_pre_check(SET_POWER_LIMIT_INFO_S *patch_info,
    gint32 *is_implemented, const gchar *str_value, const gchar *str_value2, guint8 *ps_num, json_object *obj_nar)
{
    json_object *obj_msg = NULL;
    guint8 uint8_value = 0xff;

    gchar str_value_nar[POWER_STR_MAX_LEN] = {0};
    
    int iRet = snprintf_s(str_value_nar, sizeof(str_value_nar), sizeof(str_value_nar) - 1, DEEP_NAR_PROP_PATH,
        patch_info->arr_index);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    
    gint32 ret;
    ret = get_power_management_supported(ps_num);
    return_val_do_info_if_expr(ret != VOS_OK || *ps_num == 0, VOS_ERR, *is_implemented = HTTP_NOT_IMPLEMENTED;
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, str_value, &obj_msg, str_value);
        (void)json_object_array_add(patch_info->o_message_jso, obj_msg));

    
    ret = get_active_standby_enabled(&uint8_value);
    return_val_do_info_if_expr(ret != VOS_OK || uint8_value == 0, VOS_ERR,
        (void)create_message_info(MSGID_POWER_ACTIVE_STANDBY_DISABLED, str_value, &obj_msg, str_value, str_value2);
        (void)json_object_array_add(patch_info->o_message_jso, obj_msg));

    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_NORMAL_AND_REDUNDANCY_SUPPORT, &uint8_value);
    return_val_do_info_if_expr((obj_nar != NULL) && (uint8_value == DISABLED), VOS_ERR,
        *is_implemented = HTTP_NOT_IMPLEMENTED;
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, str_value_nar, &obj_msg, str_value_nar);
        (void)json_object_array_add(patch_info->o_message_jso, obj_msg));

    return VOS_OK;
}


LOCAL void get_board_slot_pangea(gchar* slot_str, gint32 slot_len)
{
    gint32 ret;
    guint8 software_id = 0xff;

    (void)dal_get_software_type(&software_id);
    if (software_id == MGMT_SOFTWARE_TYPE_PANGEA_PACIFIC_CTRL ||
        software_id == MGMT_SOFTWARE_TYPE_PANGEA_ARCTIC_CTRL) {
        ret = snprintf_s(slot_str, slot_len, slot_len - 1, "%s", "Enc");
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "Get format board slot failed, ret is %d", ret);
        }
    } else if (software_id == MGMT_SOFTWARE_TYPE_PANGEA_MGNT) {
        ret = snprintf_s(slot_str, slot_len, slot_len - 1, "%s", BOARD_ENCLO_SLOT_FORMAT);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "Get format board slot failed, ret is %d", ret);
        }
    }

    return;
}


LOCAL void set_power_powercontrol_predictedstatus(SET_POWER_LIMIT_INFO_S *patch_info, json_object *obj_redundancy,
    json_object *obj_active_psu, json_object *obj_nar, json_object *obj_huawei, gint32 *is_implemented)
{
    gint32 ret;
    gint32 arr_len = 0;
    guint8 ps_num = 0;
    guint8 redundancy_index = 0xff;

    gboolean redundancy_deletion = FALSE;
    gboolean activepsu_deletion = FALSE;

    gchar slot_str[MAX_RSC_ID_LEN] = {0};
    gchar str_value[POWER_STR_MAX_LEN] = {0};
    gchar str_value2[POWER_STR_MAX_LEN] = {0};

    json_object *obj_odata_id = NULL;
    json_object *obj_msg = NULL;
    GSList *input_list = NULL;

    
    return_if_expr(obj_redundancy == NULL && obj_active_psu == NULL);

    
    ret = snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1, REDUNDANCY_PROP_PATH, patch_info->arr_index);
    return_if_expr(ret <= 0);

    ret =
        snprintf_s(str_value2, sizeof(str_value2), sizeof(str_value2) - 1, ACTIVE_PSU_PROP_PATH, patch_info->arr_index);
    return_if_expr(ret <= 0);

    
    check_redundancy_activepsu_deletion(obj_redundancy, obj_huawei, &redundancy_deletion, &activepsu_deletion);
    return_if_expr(redundancy_deletion || activepsu_deletion);

    
    do_val_if_expr(obj_redundancy != NULL,
        (void)json_object_object_get_ex(obj_redundancy, RFPROP_ODATA_ID, &obj_odata_id));
    return_if_expr(obj_odata_id == NULL && obj_active_psu == NULL);

    
    return_do_info_if_expr(obj_odata_id == NULL && obj_active_psu != NULL,
        (void)create_message_info(MSGID_POWER_EXPECTED_REDUNDANCY_UNSET, (const gchar *)str_value, &obj_msg,
        (const gchar *)str_value, (const gchar *)str_value2);
        json_object_array_add(patch_info->o_message_jso, obj_msg));

    ret = set_power_powercontrol_predictedstatus_pre_check(patch_info, is_implemented, str_value, str_value2, &ps_num,
        obj_nar);
    return_if_expr(ret != VOS_OK);

    ret = redfish_get_board_slot(slot_str, sizeof(slot_str) - 1);
    return_if_expr(ret != VOS_OK);

    get_board_slot_pangea(slot_str, MAX_RSC_ID_LEN);

    
    ret = check_redundancy_odata_id(patch_info, slot_str, dal_json_object_get_str(obj_odata_id), &redundancy_index);
    return_if_expr(ret != VOS_OK);

    
    return_do_info_if_expr(redundancy_index == 0 && obj_active_psu != NULL, (void)create_message_info(
        MSGID_POWER_ACTIVE_PS_NOT_NECESSARY, (const gchar *)str_value2, &obj_msg, (const gchar *)str_value2);
        json_object_array_add(patch_info->o_message_jso, obj_msg));

    
    if (obj_active_psu != NULL) {
        arr_len = json_object_array_length(obj_active_psu);
    }
    return_do_info_if_expr(redundancy_index != 0 && (NULL == obj_active_psu || arr_len == 0),
        (void)create_message_info(MSGID_POWER_ACTIVE_PS_NECESSARY, (const gchar *)str_value2, &obj_msg,
        (const gchar *)str_value2);
        json_object_array_add(patch_info->o_message_jso, obj_msg));

    ret = get_powercontrol_predictedstatus_input_list(patch_info, &input_list, obj_nar, obj_active_psu,
        redundancy_index, slot_str, ps_num);
    return_if_expr(ret != VOS_OK);

    
    ret = uip_call_class_method_redfish(patch_info->i_paras->is_from_webui, patch_info->i_paras->user_name,
        patch_info->i_paras->client, 0, CLASS_NAME_AMMETER, METHOD_SET_PLANTFORM_WORKMODE, AUTH_DISABLE,
        patch_info->i_paras->user_role_privilege, input_list, NULL);

    uip_free_gvariant_list(input_list);

    check_set_work_mode_ret(patch_info, ret, is_implemented);
}


LOCAL void set_power_powercontrol_deepsleep(SET_POWER_LIMIT_INFO_S *patch_info, json_object *obj_deep_sleep,
    gint32 *is_implemented)
{
    gint32 ret;
    guint8 uint8_value = 0;

    const gchar *str = NULL;
    gchar str_value[POWER_STR_MAX_LEN] = {0};

    json_object *obj_msg = NULL;
    GSList *input_list = NULL;

    return_if_expr(NULL == obj_deep_sleep);

    str = dal_json_object_get_str(obj_deep_sleep);
    do_val_if_expr(!g_strcmp0(str, ENABLED_STRING), (uint8_value = DEEP_SLEEP_MODE_ENABLED));

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(uint8_value));

    
    ret = uip_call_class_method_redfish(patch_info->i_paras->is_from_webui, patch_info->i_paras->user_name,
        patch_info->i_paras->client, 0, CLASS_NAME_AMMETER, METHOD_SET_DEEP_SLEEP_MODE, AUTH_ENABLE,
        patch_info->i_paras->user_role_privilege, input_list, NULL);
    

    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
            (patch_info->set_succ_num)++;
            return;

            
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, &obj_msg);
            json_object_array_add(patch_info->o_message_jso, obj_msg);
            return;

            
        case RFERR_NO_RESOURCE:
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &obj_msg, patch_info->i_paras->uri);
            json_object_array_add(patch_info->o_message_jso, obj_msg);
            return;

            
        case BOARD_NOT_SUPPORT_DEEP_SLEEP:
        case PS_NOT_SUPPORT_DEEP_SLEEP:
        case MM_BOARD_NOT_SUPPORT_DEEP_SLEEP:
            ret = snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1, DEEP_SLEEP_PROP_PATH,
                patch_info->arr_index);
            return_if_expr(0 >= ret);

            (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, (const gchar *)str_value, &obj_msg,
                (const gchar *)str_value);
            (void)json_object_array_add(patch_info->o_message_jso, obj_msg);
            
            *is_implemented = HTTP_NOT_IMPLEMENTED;
            
            return;

        default: 
            debug_log(DLOG_ERROR, "%s: ret = %d", __FUNCTION__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &obj_msg);
            json_object_array_add(patch_info->o_message_jso, obj_msg);
            return;
    }
}


LOCAL gint32 get_powercontrol_only_nar_input_list(GSList **input_list, guint8 nar)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint8 mode = 0;
    guint32 active_mask = 0;
    guint8 ps_num = 0;

    (void)get_power_management_supported(&ps_num);

    ret = dal_get_object_handle_nth(CLASS_NAME_AMMETER, 0, &obj_handle);
    return_val_do_info_if_expr(ret != VOS_OK, ret,
        debug_log(DLOG_ERROR, "%s: get obj_list for OBJ_NAME_PME_SERVICECONFIG fail", __FUNCTION__));

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_EXPECTED_POWER_MODE, &mode);
    return_val_do_info_if_expr(ret != VOS_OK, ret,
        debug_log(DLOG_ERROR, "%s: get PROPERTY_EXPECTED_POWER_MODE fail", __FUNCTION__));

    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_EXPECTED_ACTIVE, &active_mask);
    return_val_do_info_if_expr(ret != VOS_OK, ret,
        debug_log(DLOG_ERROR, "%s: get PROPERTY_EXPECTED_ACTIVE fail", __FUNCTION__));

    *input_list = g_slist_append(*input_list, (gpointer)g_variant_new_byte(mode));
    *input_list = g_slist_append(*input_list, (gpointer)g_variant_new_byte(nar));

    for (guint32 i = 0; i < ps_num; i++) {
        if (0 == (active_mask & (1 << i))) {
            *input_list = g_slist_append(*input_list, (gpointer)g_variant_new_byte(i));
        }
    }

    return VOS_OK;
}

LOCAL gint32 set_power_powercontrol_only_nar(SET_POWER_LIMIT_INFO_S *patch_info, json_object *obj_nar,
    gint32 *is_implemented)
{
    gint32 ret;
    guint8 uint8_value = 0;
    json_object *obj_msg = NULL;

    const gchar *str = NULL;
    GSList *input_list = NULL;

    gchar str_value_nar[POWER_STR_MAX_LEN] = {0};

    return_val_if_expr(obj_nar == NULL || is_implemented == NULL, VOS_ERR);

    
    ret = snprintf_s(str_value_nar, sizeof(str_value_nar), sizeof(str_value_nar) - 1, DEEP_NAR_PROP_PATH,
        patch_info->arr_index);
    do_val_if_expr(ret <= EOK, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret));

    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_NORMAL_AND_REDUNDANCY_SUPPORT, &uint8_value);
    return_val_do_info_if_expr(uint8_value == DISABLED, VOS_ERR, *is_implemented = HTTP_NOT_IMPLEMENTED;
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, str_value_nar, &obj_msg, str_value_nar);
        (void)json_object_array_add(patch_info->o_message_jso, obj_msg));

    str = dal_json_object_get_str(obj_nar);
    return_val_if_expr(NULL == str, VOS_ERR);

    if (0 == g_strcmp0(str, ENABLED_STRING)) {
        uint8_value = RF_ENABLE;
    } else {
        uint8_value = RF_DISABLE;
    }

    ret = get_powercontrol_only_nar_input_list(&input_list, uint8_value);
    return_val_if_expr(ret != VOS_OK, ret);

    ret = uip_call_class_method_redfish(patch_info->i_paras->is_from_webui, patch_info->i_paras->user_name,
        patch_info->i_paras->client, 0, CLASS_NAME_AMMETER, METHOD_SET_PLANTFORM_WORKMODE, AUTH_DISABLE,
        patch_info->i_paras->user_role_privilege, input_list, NULL);

    uip_free_gvariant_list(input_list);

    check_set_work_mode_ret(patch_info, ret, is_implemented);

    return VOS_OK;
}


LOCAL void set_power_powercontrol_ps_work_mode(SET_POWER_LIMIT_INFO_S *patch_info, json_object *obj_redundancy,
    json_object *obj_active_psu, json_object *obj_nar, json_object *obj_huawei, gint32 *is_implemented)
{
    if (obj_redundancy == NULL && obj_active_psu == NULL) {
        (void)set_power_powercontrol_only_nar(patch_info, obj_nar, is_implemented);
    } else {
        set_power_powercontrol_predictedstatus(patch_info, obj_redundancy, obj_active_psu, obj_nar, obj_huawei,
            is_implemented);
    }
}


LOCAL gint32 set_power_powercontrol_highpower(SET_POWER_LIMIT_INFO_S *patch_info, json_object *obj_high_power)
{
    gint32 ret;
    double threshold;
    json_object *obj_msg = NULL;
    GSList *input_list = NULL;
    OBJ_HANDLE power_capping_handle = 0;
    guint16 topvalue;
    gchar str_value[POWER_STR_MAX_LEN] = {0};
    gchar str_prop[PROP_VAL_LENGTH] = {0};
    gchar str_topvalue[PROP_VAL_LENGTH] = {0};

    ret = snprintf_s(str_prop, sizeof(str_prop), sizeof(str_prop) - 1, HIGH_POWER_PROP_PATH, patch_info->arr_index);
    return_val_do_info_if_expr(ret <= 0, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret));

    // 类型不为int返回类型错误
    if (json_object_get_type(obj_high_power) != json_type_int) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, (const gchar *)str_prop, &obj_msg,
            dal_json_object_get_str(obj_high_power), (const gchar *)str_prop);
        (void)json_object_array_add(patch_info->o_message_jso, obj_msg);
        return VOS_ERR;
    }
    threshold = json_object_get_int(obj_high_power);

    ret = dal_get_object_handle_nth(CLASS_NAME_POWERCAPPING, 0, &power_capping_handle);
    return_val_do_info_if_fail(ret == VOS_OK, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get powercapping object failed, ret = %d", __FUNCTION__, ret));

    ret = dal_get_property_value_uint16(power_capping_handle, PROTERY_POWERCAP_TOPVALUE, &topvalue);
    return_val_do_info_if_fail(ret == VOS_OK, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get powercapping topvalue failed, ret = %d", __FUNCTION__, ret));

    if (threshold > topvalue || threshold < MINIMUM_ALLOWABLE_POWER) {
        ret = snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1, "%f", threshold);
        return_val_do_info_if_expr(ret <= 0, VOS_ERR,
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret));

        ret = snprintf_s(str_topvalue, sizeof(str_topvalue), sizeof(str_topvalue) - 1, "%u", topvalue);
        return_val_do_info_if_expr(ret <= 0, VOS_ERR,
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret));

        (void)create_message_info(MSGID_PROP_VALUE_OUTOFRANGE, (const gchar *)str_prop, &obj_msg,
            (const gchar *)str_value, (const gchar *)str_prop, "1", (const gchar *)str_topvalue);
        json_object_array_add(patch_info->o_message_jso, obj_msg);
        return VOS_ERR;
    }

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_double(threshold));
    ret = uip_call_class_method_redfish(patch_info->i_paras->is_from_webui, patch_info->i_paras->user_name,
        patch_info->i_paras->client, 0, CLASS_NAME_AMMETER, METHOD_SET_POWER_HIGH_THRESHOLD, AUTH_ENABLE,
        patch_info->i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    switch (ret) {
        case VOS_OK:
            (patch_info->set_succ_num)++;
            return VOS_OK;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, (const gchar *)str_prop, &obj_msg,
                (const gchar *)str_prop);
            (void)json_object_array_add(patch_info->o_message_jso, obj_msg);
            return VOS_ERR;

        default: 
            debug_log(DLOG_ERROR, "%s: ret = %d", __FUNCTION__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &obj_msg);
            json_object_array_add(patch_info->o_message_jso, obj_msg);
            return VOS_ERR;
    }
}


LOCAL void set_powercapping_preset_limit_value(SET_POWER_LIMIT_INFO_S *patch_info, json_object *obj_huawei)
{
    gint32 ret;
    gint32 preset_limit;
    json_bool bool_preset_limit;
    json_object *obj_jso_preset_limit = NULL;
    json_object *obj_msg = NULL;
    GSList *input_list = NULL;
    gchar str_prop[PROP_VAL_LENGTH] = { 0 };

    bool_preset_limit =
        json_object_object_get_ex(obj_huawei, RFPROP_POWER_CONTROL_PRESET_LIMIT_VALUE, &obj_jso_preset_limit);
    if (bool_preset_limit != TRUE) {
        return;
    }

    ret = snprintf_s(str_prop, sizeof(str_prop), sizeof(str_prop) - 1, PRESET_LIMIT_IN_WATTS_PROP_PATH,
        patch_info->arr_index);
    do_val_if_expr(ret <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret));

    if (json_object_get_type(obj_jso_preset_limit) != json_type_int) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, (const gchar *)str_prop, &obj_msg,
            dal_json_object_get_str(obj_jso_preset_limit), (const gchar *)str_prop);
        (void)json_object_array_add(patch_info->o_message_jso, obj_msg);
        return;
    }

    // 设置功耗封顶预设值的接口被频繁调用，且仅供机柜内部网络使用，不记录操作日志
    if (is_the_request_from_rack_inner_device(patch_info->i_paras->client) == FALSE) {
        debug_log(DLOG_INFO, "%s: client=%s is not rack inner device, unable to set preset limit value=%d",
            __FUNCTION__, patch_info->i_paras->client, json_object_get_int(obj_jso_preset_limit));
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, (const gchar *)str_prop, &obj_msg,
            (const gchar *)str_prop);
        (void)json_object_array_add(patch_info->o_message_jso, obj_msg);
        return;
    }

    preset_limit = json_object_get_int(obj_jso_preset_limit);
    input_list = g_slist_append(input_list, g_variant_new_uint16(preset_limit));
    ret = uip_call_class_method_redfish(patch_info->i_paras->is_from_webui, patch_info->i_paras->user_name,
        patch_info->i_paras->client, patch_info->handle, CLASS_NAME_POWERCAPPING, METHOD_PRESET_POWERCAP_LIMITVALUE,
        AUTH_ENABLE, patch_info->i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    switch (ret) {
        case RET_OK:
            (patch_info->set_succ_num)++;
            return;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, (const gchar *)str_prop, &obj_msg,
                (const gchar *)str_prop);
            (void)json_object_array_add(patch_info->o_message_jso, obj_msg);
            return;

        default:
            debug_log(DLOG_ERROR, "%s: ret = %d", __FUNCTION__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &obj_msg);
            json_object_array_add(patch_info->o_message_jso, obj_msg);
            return;
    }
}


LOCAL void set_power_powercontrol_metrics_extended(SET_POWER_LIMIT_INFO_S* patch_info, json_object* obj_huawei)
{
    json_object *obj_metrics = NULL;
    json_object *obj_flag = NULL;
    json_object *obj_msg = NULL;
    GSList* input_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    gchar prop_name[POWER_STR_MAX_LEN * 2] = {0};

    
    int iRet = snprintf_s(prop_name, sizeof(prop_name), sizeof(prop_name) - 1, POWER_LIMIT_WHEN_STEADY_PATH,
        patch_info->arr_index);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    json_object_object_get_ex(obj_huawei, RF_POWER_EXTEND_METRICS, &obj_metrics);
    if (obj_metrics == NULL) {
        return;
    }

    json_object_object_get_ex(obj_metrics, RF_POWER_LIMIT_WHEN_STEADY, &obj_flag);
    if (obj_flag == NULL) {
        return;
    }

    gint32 ret = dal_get_object_handle_nth(CLASS_NAME_POWERCAPPING, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_object_handle_nth failed. ret = %d", __FUNCTION__, ret);
        create_message_info(MSGID_INTERNAL_ERR, NULL, &obj_msg);
        json_object_array_add(patch_info->o_message_jso, obj_msg);
        return;
    }

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(json_object_get_boolean(obj_flag)));

    ret = uip_call_class_method_redfish(patch_info->i_paras->is_from_webui, patch_info->i_paras->user_name,
        patch_info->i_paras->client, obj_handle, CLASS_NAME_POWERCAPPING, METHOD_POWERCAP_WHEN_STEADY, AUTH_DISABLE,
        patch_info->i_paras->user_role_privilege, input_list, NULL);

    uip_free_gvariant_list(input_list);

    switch (ret) {
        case RET_OK:
            (patch_info->set_succ_num)++;
            return;

        case ERR_POWER_LIMIT_NOT_SUPPORT:
            create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, prop_name, &obj_msg, prop_name);
            break;

        
        case RFERR_INSUFFICIENT_PRIVILEGE:
            create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, &obj_msg);
            break;

        
        default:
            debug_log(DLOG_ERROR, "%s: ret = %d", __FUNCTION__, ret);
            create_message_info(MSGID_INTERNAL_ERR, NULL, &obj_msg);
            break;
    }
    json_object_array_add(patch_info->o_message_jso, obj_msg);
    return;
}


LOCAL void set_power_powercontrol_oem(SET_POWER_LIMIT_INFO_S *patch_info, gint32 *is_implemented)
{
    json_bool bool_jso;
    json_bool bool_redundany;
    json_bool bool_active_psu;
    json_bool bool_deep_sleep;
    json_bool bool_nar;
    json_bool bool_high_power;

    json_object *obj_oem = NULL;
    json_object *obj_huawei = NULL;
    json_object *obj_redundancy = NULL;
    json_object *obj_active_psu = NULL;
    json_object *obj_deep_sleep = NULL;
    json_object *obj_nar = NULL;
    json_object *obj_high_power = NULL;

    
    bool_jso = json_object_object_get_ex(patch_info->arr_elem_jso, RFPROP_FWINV_OEM, &obj_oem);
    return_if_fail(bool_jso);

    
    bool_jso = json_object_object_get_ex(obj_oem, RFPROP_FWINV_HUAWEI, &obj_huawei);
    return_if_fail(bool_jso);

    
    bool_redundany = json_object_object_get_ex(obj_huawei, RFPROP_POWER_EXPECTED_REDUNDANCY, &obj_redundancy);

    
    bool_active_psu = json_object_object_get_ex(obj_huawei, RFPROP_POWER_EXPECTED_ACTIVE_PSU, &obj_active_psu);

    
    bool_deep_sleep = json_object_object_get_ex(obj_huawei, RFPROP_POWER_DEEP_SLEEP_MODE, &obj_deep_sleep);

    
    bool_nar = json_object_object_get_ex(obj_huawei, RFPROP_POWER_NORMAL_AND_REDUNDANCY, &obj_nar);

    bool_high_power = json_object_object_get_ex(obj_huawei, RFPROP_POWER_HIGH_THRESHOLD, &obj_high_power);
    if (bool_redundany || bool_active_psu || bool_nar) {
        set_power_powercontrol_ps_work_mode(patch_info, obj_redundancy, obj_active_psu, obj_nar, obj_huawei,
            is_implemented);
    }

    if (bool_deep_sleep) {
        set_power_powercontrol_deepsleep(patch_info, obj_deep_sleep, is_implemented);
    }

    if (bool_high_power) {
        (void)set_power_powercontrol_highpower(patch_info, obj_high_power);
    }

    set_powercapping_preset_limit_value(patch_info, obj_huawei);
    set_power_powercontrol_metrics_extended(patch_info, obj_huawei);
}


LOCAL gint32 set_chassis_power_powercontrol_attributes(SET_POWER_LIMIT_INFO_S *power_limit_info)
{
    gint32 ret_num = VOS_OK;
    gint32 ret = VOS_OK;

    gint32 uint_limit = 0;
    gchar str_value[POWER_STR_MAX_LEN] = {0};
    gchar str_value2[POWER_STR_MAX_LEN] = {0};

    const gchar *str_limitaction = NULL;

    json_object *obj_jso_powerlimit = NULL;
    json_object *obj_jso_limit = NULL;
    json_object *obj_jso_limitaction = NULL;
    json_object *obj_jso = NULL;

    json_bool bool_jso_powerlimit;
    json_bool bool_jso_limit;
    json_bool bool_jso_limitaction;
    json_object *message = NULL;
    gchar prowerctl_json[100] = {0};
    guchar manual_set_enable = FALSE;

    
    return_val_do_info_if_expr((0 == json_object_object_length(power_limit_info->arr_elem_jso) &&
        NULL == power_limit_info->arr_elem_jso->_userdata),
        HTTP_BAD_REQUEST,
        (void)snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1, "PowerControl/%d",
        power_limit_info->arr_index);
        (void)create_message_info(MSGID_ITEM_PROP_FEW, str_value, &message, str_value);
        (void)json_object_array_add(power_limit_info->o_message_jso, message));
    

    
    set_power_powercontrol_oem(power_limit_info, &ret_num);
    
    do_val_if_expr(501 != ret_num, (ret_num = VOS_OK));

    
    bool_jso_powerlimit =
        json_object_object_get_ex(power_limit_info->arr_elem_jso, RFPROP_POWER_CONTROL_LIMIT, &obj_jso_powerlimit);

    
    return_val_do_info_if_fail(bool_jso_powerlimit, ret_num,
        debug_log(DLOG_ERROR, "%s, %d: get obj_jso_powerlimit error.", __FUNCTION__, __LINE__));
    ret_num = VOS_OK;
    

    
    bool_jso_limit = json_object_object_get_ex(obj_jso_powerlimit, RFPROP_POWER_CONTROL_LIMIT_VALUE, &obj_jso_limit);
    bool_jso_limitaction =
        json_object_object_get_ex(obj_jso_powerlimit, RFPROP_POWER_CONTROL_LIMIT_EXCEPTION, &obj_jso_limitaction);

    
    return_val_do_info_if_fail(bool_jso_limit || bool_jso_limitaction, VOS_OK,
        debug_log(DLOG_ERROR, "%s, %d: get obj_jso_limit or obj_jso_limitaction error.", __FUNCTION__, __LINE__));

    
    (void)snprintf_s(prowerctl_json, sizeof(prowerctl_json), sizeof(prowerctl_json) - 1,
        "PowerControl/%d/PowerLimit/LimitInWatts", power_limit_info->arr_index);
    (void)dal_get_func_ability(CLASS_NAME_POWERCAPPING, PROPERTY_POWERCAP_MANUALSETENABLE, &manual_set_enable);
    if (FALSE == manual_set_enable) {
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, (const gchar *)prowerctl_json, &message,
            (const gchar *)prowerctl_json);
        (void)json_object_array_add(power_limit_info->o_message_jso, message);
        return HTTP_BAD_REQUEST;
    }
    

    if (bool_jso_limit) {
        
        if (TRUE == json_object_is_type(obj_jso_limit, json_type_double)) {
            
            (void)snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1,
                "PowerControl/%d/PowerLimit/LimitInWatts", power_limit_info->arr_index);
            
            (void)snprintf_s(str_value2, sizeof(str_value2), sizeof(str_value2) - 1, "%lf",
                json_object_get_double(obj_jso_limit));
            (void)create_message_info(MSGID_PROP_TYPE_ERR, (const gchar *)str_value, &obj_jso,
                (const gchar *)str_value2, (const gchar *)str_value);
            json_object_array_add(power_limit_info->o_message_jso, obj_jso);
        }
        
        else if (TRUE == json_object_is_type(obj_jso_limit, json_type_null)) {
            ret = set_powercontrol_powerlimit_enable(power_limit_info, RF_POWER_CONTROL_LIMIT_DISABLE);
            do_val_if_expr(VOS_OK != ret, return ret);
        } else {
            
            ret = set_powercontrol_powerlimit_enable(power_limit_info, RF_POWER_CONTROL_LIMIT_ENABLE);
            do_val_if_expr(VOS_OK != ret, return ret);

            
            uint_limit = json_object_get_int(obj_jso_limit);
            
            do_val_if_expr(uint_limit > G_MAXUINT16 || uint_limit < 0, (void)snprintf_s(str_value, sizeof(str_value),
                sizeof(str_value) - 1, "PowerControl/%d/PowerLimit/LimitInWatts", power_limit_info->arr_index);
                (void)snprintf_s(str_value2, sizeof(str_value2), sizeof(str_value2) - 1, "%d", uint_limit);
                (void)create_message_info(MSGID_PROP_NOT_IN_LIST, str_value, &obj_jso, str_value2, str_value);
                json_object_array_add(power_limit_info->o_message_jso, obj_jso); return VOS_ERR);
            
            ret = set_powercontrol_property_limit(power_limit_info, (guint16)uint_limit);
            do_val_if_expr(VOS_OK != ret, return ret);
        }
    }

    if (bool_jso_limitaction) {
        
        if (TRUE == json_object_is_type(obj_jso_limitaction, json_type_null)) {
            (void)memset_s(str_value, sizeof(str_value), 0, sizeof(str_value));
            
            (void)snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1,
                "PowerControl/%d/PowerLimit/LimitException", power_limit_info->arr_index);
            
            obj_jso = NULL;
            (void)create_message_info(MSGID_PROP_TYPE_ERR, (const gchar *)str_value, &obj_jso, RF_VALUE_NULL,
                (const gchar *)str_value);
            json_object_array_add(power_limit_info->o_message_jso, obj_jso);

            return VOS_OK;
        }

        
        str_limitaction = dal_json_object_get_str(obj_jso_limitaction);
        ret = set_powercontrol_property_limitaction(power_limit_info, str_limitaction);
        do_val_if_expr(VOS_OK != ret, return ret);
    }

    
    return ret_num;
}


LOCAL gint32 check_chassis_power_powercontrol_access(PROVIDER_PARAS_S *i_paras,
    json_object **o_message_jso)
{
    guint8 software_type = 0xff;

    (void)dal_get_software_type(&software_type);
    if (software_type == MGMT_SOFTWARE_TYPE_PANGEA_ALANTIC_CTRL) {
        debug_log(DLOG_ERROR, "Product(%d) not support", software_type);
        return HTTP_NOT_IMPLEMENTED;
    }

    
    if (o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (!(i_paras->user_role_privilege & USERROLE_POWERMGNT)) {
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_POWER_CONTROL,
            o_message_jso, RFPROP_POWER_CONTROL);
        return HTTP_FORBIDDEN;
    }

    return RET_OK;
}


LOCAL gint32 set_chassis_power_powercontrol(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    gint32 obj_num = 0;
    SET_POWER_LIMIT_INFO_S power_limit_info;
    gint32 i;

    gint32 ret = check_chassis_power_powercontrol_access(i_paras, o_message_jso);
    if (ret != RET_OK) {
        return ret;
    }

    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_POWERMGNT, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_POWER_CONTROL, o_message_jso,
        RFPROP_POWER_CONTROL));

    
    ret = dfl_get_object_list(CLASS_NAME_POWERCAPPING, &obj_list);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s, %d: get obj_list fail.\n", __FUNCTION__, __LINE__));

    
    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        obj_num++;
    }

    
    gint32 arr_len = json_object_array_length(i_paras->val_jso);
    return_val_do_info_if_fail(0 != arr_len, HTTP_BAD_REQUEST, g_slist_free(obj_list);
        (void)create_message_info(MSGID_PROP_MISSING, RFPROP_POWER_CONTROL, o_message_jso, RFPROP_POWER_CONTROL));

    
    return_val_do_info_if_fail(arr_len <= obj_num, HTTP_BAD_REQUEST, g_slist_free(obj_list);
        obj_list = NULL;
        (void)create_message_info(MSGID_ARRAY_ITEM_EXCEED, RFPROP_POWER_CONTROL, o_message_jso, RFPROP_POWER_CONTROL));
    

    *o_message_jso = json_object_new_array();

    (void)memset_s(&power_limit_info, sizeof(power_limit_info), 0, sizeof(power_limit_info));
    power_limit_info.i_paras = i_paras;
    power_limit_info.o_message_jso = *o_message_jso;

    for (i = 0, obj_node = obj_list; i < arr_len; i++, obj_node = obj_node->next) {
        
        power_limit_info.arr_index = i;
        power_limit_info.arr_elem_jso = json_object_array_get_idx(i_paras->val_jso, i);
        
        continue_if_expr(NULL == power_limit_info.arr_elem_jso);
        
        power_limit_info.handle = (OBJ_HANDLE)obj_node->data;

        ret = set_chassis_power_powercontrol_attributes(&power_limit_info);
        return_val_do_info_if_fail(VOS_OK == ret, ret, g_slist_free(obj_list));
    }

    do_val_if_expr(NULL != obj_list, g_slist_free(obj_list);
        obj_list = NULL);

    
    return (0 != power_limit_info.set_succ_num ? HTTP_OK : HTTP_BAD_REQUEST);
}


LOCAL gint32 get_chassis_power_voltages_name(OBJ_HANDLE handle, json_object *obj_json)
{
    guint32 ret;
    gchar string_value[POWER_STR_MAX_LEN] = {0};

    if (NULL == obj_json) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    (void)memset_s(string_value, sizeof(string_value), 0, sizeof(string_value));
    ret = dal_get_property_value_string(handle, PROPERTY_DIS_SENSOR_SENSOR_NAME, string_value, POWER_STR_MAX_LEN);
    if (VOS_OK != ret || (*string_value == 0)) {
        json_object_object_add(obj_json, RFPROP_SENSOR_NAME, NULL);
    } else {
        json_object_object_add(obj_json, RFPROP_SENSOR_NAME, json_object_new_string((const gchar *)string_value));
    }

    return HTTP_OK;
}


LOCAL gint32 get_chassis_power_voltages_sensornumber(OBJ_HANDLE handle, json_object *obj_json)
{
    guint32 ret;
    guint8 uint8_value = 0;

    if (NULL == obj_json) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_property_value_byte(handle, PROPERTY_THR_SENSOR_NUMBER, &uint8_value);
    if (VOS_OK != ret) {
        json_object_object_add(obj_json, RFPROP_SENSOR_SENSOR_NUM, NULL);
    } else {
        json_object_object_add(obj_json, RFPROP_SENSOR_SENSOR_NUM, json_object_new_int(uint8_value));
    }

    return HTTP_OK;
}


LOCAL gint32 get_chassis_power_voltages_readingvolts(OBJ_HANDLE handle, json_object *obj_json, gint32 sensor_status)
{
    guint32 ret;
    gdouble double_value = 0.0;

    if (NULL == obj_json) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_property_value_double(handle, PROPERTY_THR_SENSOR_READING_CONVERT, &double_value);
    if ((VOS_OK != ret) || (VOS_OK != sensor_status)) {
        json_object_object_add(obj_json, RFPROP_SENSOR_READINGVOLTS, NULL);
    } else {
        json_object_object_add(obj_json, RFPROP_SENSOR_READINGVOLTS, ex_json_object_new_double(double_value, "%.2f"));
    }

    return HTTP_OK;
}


LOCAL gint32 get_chassis_power_voltages_Threshold(OBJ_HANDLE handle, json_object *obj_json, guint16 readable,
    gchar *sensor_convert, gchar *sensor_convert_name)
{
    guint32 ret;
    gdouble double_value = 0.0;

    if (NULL == obj_json || NULL == sensor_convert || NULL == sensor_convert_name) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_property_value_double(handle, sensor_convert, &double_value);
    if ((VOS_OK == ret) && (readable)) {
        json_object_object_add(obj_json, sensor_convert_name, ex_json_object_new_double(double_value, "%.2f"));
    } else {
        json_object_object_add(obj_json, sensor_convert_name, NULL);
    }

    return HTTP_OK;
}


LOCAL gint32 get_chassis_power_voltages_status(OBJ_HANDLE handle, json_object *obj_json,
    EVENT_WORK_STATE_S *sensor_reading_status, gint32 *sensor_status)
{
    gint32 ret;
    guint8 uint8_value = 0;
    json_object *obj_json_status = NULL;

    // 获取 status->Health
    ret = dal_get_property_value_byte(handle, PROPERTY_THR_SENSOR_HEALTH_STATUS, &uint8_value);
    return_val_do_info_if_fail(VOS_OK == ret, DFL_ERR,
        debug_log(DLOG_ERROR, "%s, %d: get obj_list fail.\n", __FUNCTION__, __LINE__));

    // 获取 status->State
    if (!sensor_reading_status->disable_scanning || !sensor_reading_status->disable_scanning_local ||
        !sensor_reading_status->disable_all || !sensor_reading_status->initial_update_progress ||
        !sensor_reading_status->disable_access_error) {
        (void)get_resource_status_property(&uint8_value, NULL, RFPROP_SENSOR_DISABLED, &obj_json_status, FALSE);
        *sensor_status = VOS_ERR;
    } else {
        (void)get_resource_status_property(&uint8_value, NULL, RFPROP_SENSOR_ENABLED, &obj_json_status, FALSE);
        *sensor_status = VOS_OK;
    }

    json_object_object_add(obj_json, RFPROP_SENSOR_STATUS, obj_json_status);

    return HTTP_OK;
}


LOCAL gint32 get_chassis_power_voltages_attribute(const gchar *chassis_id, OBJ_HANDLE handle,
    json_object **o_message_jso, json_object **o_result_jso, gint g_index)
{
    int iRet;
    gchar   string_value[ARRAY_LENTH] = {0};
    guint8 uint8_reading = 0;
    guint8 capabilities = 0;
    gint32 ret;
    gint32 sensor_status = 0;

    json_object *obj_json = NULL;
    SENSOR_CAPABILITY_S *sensor_cap = NULL;
    guint16 mask = 0;
    SENSOR_READINGMASK_BIT *reading_mask = NULL;
    EVENT_WORK_STATE_S *sensor_reading_status = NULL;

    if ((NULL == chassis_id) || NULL == o_result_jso || NULL == o_message_jso) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_property_value_byte(handle, PROPERTY_THS_SENSOR_READING_STATUS, &uint8_reading);
    do_val_if_expr(VOS_OK != ret, (uint8_reading = 0));
    sensor_reading_status = (EVENT_WORK_STATE_S *)&uint8_reading;

    ret = dal_get_property_value_uint16(handle, PROPERTY_THR_SENSOR_READING_MASK, &mask);
    do_val_if_expr(VOS_OK != ret, (mask = 0));
    reading_mask = (SENSOR_READINGMASK_BIT *)&mask;

    ret = dal_get_property_value_byte(handle, PROPERTY_THR_SENSOR_CAPABILITIES, &capabilities);
    do_val_if_expr(VOS_OK != ret, (capabilities = 0));
    sensor_cap = (SENSOR_CAPABILITY_S *)&capabilities;

    obj_json = json_object_new_object();

    iRet = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1, "%s%s%s/%d", RF_CHASSIS_UI,
        chassis_id, REDFISH_CHASSIS_POWERVOLTAGES, g_index);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    json_object_object_add(obj_json, RFPROP_POWER_CONTROL_ODATA_ID,
        json_object_new_string((const gchar *)string_value));

    // 获取 MemberId
    (void)memset_s(string_value, sizeof(string_value), 0, sizeof(string_value));
    (void)snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1, "%d", g_index);
    json_object_object_add(obj_json, RFPROP_SENSOR_MEMBER_ID, json_object_new_string((const gchar *)string_value));

    // 获取 Name传感器名称
    (void)get_chassis_power_voltages_name(handle, obj_json);

    // 获取 SensorNumber传感器号
    (void)get_chassis_power_voltages_sensornumber(handle, obj_json);

    // 获取 Status
    (void)get_chassis_power_voltages_status(handle, obj_json, sensor_reading_status, &sensor_status);

    // 获取 ReadingVolts读数
    (void)get_chassis_power_voltages_readingvolts(handle, obj_json, sensor_status);

    if (!((SENSOR_CAP_READABLE_SETTABLE == sensor_cap->threshold_access_support) ||
        (SENSOR_CAP_READABLE == sensor_cap->threshold_access_support))) {
        json_object_object_add(obj_json, RFPROP_SENSOR_UPPER_THRESHOLD_NON_CRITICAL, NULL);
        json_object_object_add(obj_json, RFPROP_SENSOR_UPPER_THRESHOLD_CRITICAL, NULL);
        json_object_object_add(obj_json, RFPROP_SENSOR_UPPER_THRESHOLD_FATAL, NULL);
        json_object_object_add(obj_json, RFPROP_SENSOR_LOWER_THRESHOLD_NON_CRITICAL, NULL);
        json_object_object_add(obj_json, RFPROP_SENSOR_LOWER_THRESHOLD_CRITICAL, NULL);
        json_object_object_add(obj_json, RFPROP_SENSOR_LOWER_THRESHOLD_FATAL, NULL);
    } else {
        // 获取 UpperThresholdNonCritical轻微上门限  null
        (void)get_chassis_power_voltages_Threshold(handle, obj_json, reading_mask->readable_upper_noncritical,
            PROPERTY_THR_SENSOR_UNC_CONVERT, RFPROP_SENSOR_UPPER_THRESHOLD_NON_CRITICAL);

        // 获取 UpperThresholdCritical严重上门限  null
        (void)get_chassis_power_voltages_Threshold(handle, obj_json, reading_mask->readable_upper_critical,
            PROPERTY_THR_SENSOR_UC_CONVERT, RFPROP_SENSOR_UPPER_THRESHOLD_CRITICAL);

        // 获取 UpperThresholdFatal紧急上门限  null
        (void)get_chassis_power_voltages_Threshold(handle, obj_json, reading_mask->readable_upper_nonrecoverable,
            PROPERTY_THR_SENSOR_UNR_CONVERT, RFPROP_SENSOR_UPPER_THRESHOLD_FATAL);

        // 获取 LowerThresholdNonCritical轻微下门限  null
        (void)get_chassis_power_voltages_Threshold(handle, obj_json, reading_mask->readable_lower_noncritical,
            PROPERTY_THR_SENSOR_LNC_CONVERT, RFPROP_SENSOR_LOWER_THRESHOLD_NON_CRITICAL);

        // 获取 LowerThresholdCritical严重下门限  null
        (void)get_chassis_power_voltages_Threshold(handle, obj_json, reading_mask->readable_lower_critical,
            PROPERTY_THR_SENSOR_LC_CONVERT, RFPROP_SENSOR_LOWER_THRESHOLD_CRITICAL);

        // 获取 LowerThresholdFatal紧急下门限
        (void)get_chassis_power_voltages_Threshold(handle, obj_json, reading_mask->readable_lower_nonrecoverable,
            PROPERTY_THR_SENSOR_LNR_CONVERT, RFPROP_SENSOR_LOWER_THRESHOLD_FATAL);
    }

    // 获取 MinReadingRange读数最小值
    json_object_object_add(obj_json, RFPROP_SENSOR_MIN_READING_RANGE, NULL);

    // 获取 MaxReadingRange读数最大值
    json_object_object_add(obj_json, RFPROP_SENSOR_MAX_READING_RANGE, NULL);

    json_object_array_add(*o_result_jso, obj_json);

    return VOS_OK;
}


LOCAL gint32 get_chassis_power_voltages(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint32 ret_thresholdsensor;
    GSList *obj_list_thresholdsensor = NULL;
    GSList *obj_node_thresholdsensor = NULL;

    guint8 uint8_sensortype_voltages = 0;
    gchar               slot_id[MAX_MEM_ID_LEN] = {0};

    gint g_index = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = rf_gen_chassis_component_id(NULL, i_paras->obj_handle, slot_id, MAX_MEM_ID_LEN);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:gen chassis component id failed", __FUNCTION__));

    
    *o_result_jso = json_object_new_array();

    
    ret_thresholdsensor = dfl_get_object_list(CLASS_THRESHOLD_SENSOR, &obj_list_thresholdsensor);
    do_val_if_expr(VOS_OK != ret_thresholdsensor,
        debug_log(DLOG_ERROR, "%s, %d: get obj_list fail.\n", __FUNCTION__, __LINE__));

    for (obj_node_thresholdsensor = obj_list_thresholdsensor; obj_node_thresholdsensor;
        obj_node_thresholdsensor = obj_node_thresholdsensor->next) {
        // 获取 sensortype，对比找寻为“02h”的，即为电压属性
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node_thresholdsensor->data, PROPERTY_THR_SENSOR_SENSOR_TYPE,
            &uint8_sensortype_voltages);
        if (VOS_OK != ret) {
            do_val_if_expr(NULL != obj_list_thresholdsensor, g_slist_free(obj_list_thresholdsensor);
                json_object_put(*o_result_jso); *o_result_jso = NULL);
            return HTTP_INTERNAL_SERVER_ERROR;
        }

        if (VOLTAGE_TYPE == uint8_sensortype_voltages) {
            (void)get_chassis_power_voltages_attribute(slot_id, (OBJ_HANDLE)obj_node_thresholdsensor->data,
                o_message_jso, o_result_jso, g_index);
            g_index++;
        }
    }

    do_val_if_expr(NULL != obj_list_thresholdsensor, g_slist_free(obj_list_thresholdsensor));

    return HTTP_OK;
}



LOCAL void get_chassis_power_powersupplies_fmver(json_object *obj_jso, OBJ_HANDLE handle_onepower)
{
    gint32 ret_dal;
    gint32 ret_dal2;

    gchar string_value[POWER_STR_MAX_LEN] = {0};
    gchar string_value2[POWER_STR_MAX_LEN] = {0};

    gchar *fmver = string_value;
    gchar *ver = string_value2;

    ret_dal = dal_get_property_value_string(handle_onepower, PROTERY_PS_FWVERSION, string_value, sizeof(string_value));
    ret_dal2 = dal_get_property_value_string(handle_onepower, PROTERY_PS_VERSION, string_value2, sizeof(string_value2));
    
    if (VOS_OK == ret_dal2) {
        dal_trim_string(ver, POWER_STR_MAX_LEN);
    }

    if (VOS_OK == ret_dal) {
        dal_trim_string(fmver, POWER_STR_MAX_LEN);
    }

    do_val_if_expr(0 != strlen(ver) && g_strcmp0(ver, "N/A"),
        json_object_object_add(obj_jso, RFPROP_POWER_SUPPLIES_FIRMWARE_VER, json_object_new_string(ver));
        return );
    do_val_if_expr(0 != strlen(fmver) && g_strcmp0(fmver, "N/A"),
        json_object_object_add(obj_jso, RFPROP_POWER_SUPPLIES_FIRMWARE_VER, json_object_new_string(fmver));
        return );
    json_object_object_add(obj_jso, RFPROP_POWER_SUPPLIES_FIRMWARE_VER, NULL);
}




LOCAL void get_chassis_power_powersupplies_status(json_object *o_element_jso, OBJ_HANDLE handle_onepower)
{
    gint32 ret = VOS_OK;
    guint8 uint8_value = 0xff;
    gchar *string_val = NULL;
    json_object *obj_jso_status = NULL;

    
    do_val_if_expr(0 != handle_onepower,
        (void)dal_get_property_value_byte(handle_onepower, PROTERY_PS_PRESENCE, &uint8_value));

    
    if (0 == handle_onepower || 0 == uint8_value) {
        
        uint8_value = 0xff; // 不在位health为null
        
        string_val = RF_STATE_ABSENT;
    } else {
        
        ret = dal_get_property_value_byte(handle_onepower, PROPERTY_ACTUAL_ACTIVE, &uint8_value);
        return_do_info_if_expr(VOS_OK != ret,
            debug_log(DLOG_ERROR, "%s: get PROPERTY_ACTUAL_ACTIVE fail", __FUNCTION__);
            json_object_object_add(o_element_jso, RF_PROPERTY_STATUS, NULL));

        if (PROPERTY_ACTUAL_ACTIVE_VALUE_ENABLED == uint8_value) { // 0对应主用电源
            string_val = ENABLED_STRING;
        } else {
            string_val = RF_POWER_SUPPLIES_STATUS_STATE_VALUE_STANDBY;
        }

        
        ret = dal_get_property_value_byte(handle_onepower, PROTERY_PS_STATUS, &uint8_value);
        return_do_info_if_expr(VOS_OK != ret, debug_log(DLOG_ERROR, "%s: get PROTERY_PS_STATUS fail", __FUNCTION__);
            json_object_object_add(o_element_jso, RF_PROPERTY_STATUS, NULL));
    }

    
    (void)get_resource_status_property(&uint8_value, NULL, string_val, &obj_jso_status, FALSE);

    
    json_object_object_add(o_element_jso, RF_PROPERTY_STATUS, obj_jso_status);
}

LOCAL gchar* get_psu_input_status_str(guint8 input_status)
{
    if (input_status == PSU_INPUT_STATUS_NORMAL) {
        return PS_VOLTAGE_STATUS_NORMAL_STR;
    } else if (input_status == PSU_INPUT_STATUS_OVERVOLTAGE) {
        return PS_VOLTAGE_STATUS_OVER_STR;
    } else if (input_status == PSU_INPUT_STATUS_UNDERVOLTAGE) {
        return PS_VOLTAGE_STATUS_UNDER_STR;
    } else if (input_status == PSU_INPUT_STATUS_POWER_OFF) {
        return PS_VOLTAGE_STATUS_POWER_OFF_STR;
    } else {
        return NULL;
    }
}

LOCAL void add_powersupplies_oem_for_canbus(OBJ_HANDLE obj_onepower, json_object* obj_huawei)
{
    gint32 ret;
    guint8 uint8_value;
    gchar* string_val = NULL;

    uint8_value = PSU_INPUT_STATUS_INVALID;
    ret = dal_get_property_value_byte(obj_onepower, PROTERY_PS_INPUT_A_STATUS, &uint8_value);
    add_property_value_string(obj_huawei, RFPROP_PS_INPUT_A_STATUS, get_psu_input_status_str(uint8_value), ret);

    uint8_value = PSU_INPUT_STATUS_INVALID;
    ret = dal_get_property_value_byte(obj_onepower, PROTERY_PS_INPUT_B_STATUS, &uint8_value);
    add_property_value_string(obj_huawei, RFPROP_PS_INPUT_B_STATUS, get_psu_input_status_str(uint8_value), ret);

    ret = dal_get_property_value_byte(obj_onepower, PROTERY_PS_BATTERY_PRES_STATE, &uint8_value);
    string_val = NULL;
    if (uint8_value == CANBUS_DISABLE_VALUE) {
        // 0xAA表示电池在位
        string_val = PRESENT_STRING;
    } else if (uint8_value == CANBUS_ENABLE_VALUE) {
        // 0x55表示电池不在位
        string_val = ABSENT_STRING;
    }
    add_property_value_string(obj_huawei, RFPROP_PS_BATTERY_PRES_STATE, string_val, ret);

    ret = dal_get_property_value_byte(obj_onepower, PROTERY_PS_FORBIDDEN_WALKIN_STATE, &uint8_value);
    if (uint8_value == 1) {
        json_object_object_add(obj_huawei, RFPROP_PS_EOD_ALARM_STATUS, json_object_new_boolean(TRUE));
    } else {
        json_object_object_add(obj_huawei, RFPROP_PS_EOD_ALARM_STATUS, json_object_new_boolean(FALSE));
    }
}

LOCAL void add_null_object_for_powersupplies_oem(json_object* obj_huawei)
{
    json_object_object_add(obj_huawei, RFPROP_POWER_SUPPLIES_PROTOCOL, NULL);
    json_object_object_add(obj_huawei, RFPROP_POWER_SUPPLIES_ACTIVESTANDBY, NULL);
    json_object_object_add(obj_huawei, RFPROP_POWER_SUPPLIES_INPUT_POWER, NULL);
    json_object_object_add(obj_huawei, RFPROP_POWER_SUPPLIES_INPUT_CURRENT, NULL);
    json_object_object_add(obj_huawei, RFPROP_POWER_SUPPLIES_OUTPUT_POWER, NULL);
    json_object_object_add(obj_huawei, RFPROP_POWER_SUPPLIES_OUTPUT_CURRENT, NULL);
    json_object_object_add(obj_huawei, RFPROP_POWER_SUPPLIES_OUTPUT_VOLTAGE, NULL);
    json_object_object_add(obj_huawei, RFPROP_POWER_SUPPLIES_INLET_TEMPERATURE, NULL);
    json_object_object_add(obj_huawei, RFPROP_POWER_SUPPLIES_INNER_TEMPERATURE, NULL);
    json_object_object_add(obj_huawei, RFPROP_DEVICELOCATOR, NULL);
    json_object_object_add(obj_huawei, RFPROP_POWER_SUPPLIES_SLOTNUM, NULL);
    json_object_object_add(obj_huawei, RFPROP_POWER_SUPPLIES_MANUFACTUREDATE, NULL);
    json_object_object_add(obj_huawei, RFPROP_POWER_SUPPLIES_CURRENT_CAPACITY, NULL);
    json_object_object_add(obj_huawei, RFPROP_POWER_SUPPLIES_TOTAL_RUNNING_TIME, NULL);
    json_object_object_add(obj_huawei, RFPROP_POWER_SUPPLIES_FREQUENCY, NULL);
    json_object_object_add(obj_huawei, RFPROP_POWER_SUPPLIES_POWERSUPPLY_CHANNEL, NULL);
    json_object_object_add(obj_huawei, RFPROP_POWER_SUPPLIES_VIN_CHANNEL_A, NULL);
    json_object_object_add(obj_huawei, RFPROP_POWER_SUPPLIES_VIN_CHANNEL_B, NULL);
    json_object_object_add(obj_huawei, RFPROP_POSITION, NULL);
}

LOCAL void add_protocol_for_powersupplies_oem(json_object* obj_huawei, OBJ_HANDLE handle_onepower)
{
    gint32 ret_dal;
    guint8 uint8_value = 0xff;
    gchar* string_val = NULL;

    ret_dal = dal_get_property_value_byte(handle_onepower, PROTERY_PS_PROTOCAL, &uint8_value);

    
    if (uint8_value == PROTOCOL_PSMI) {
        string_val = PROTERY_PS_PROTOCOL_PSMI_STR;
    } else if (uint8_value == PROTOCOL_PMBUS || uint8_value == PROTOCOL_5500_PMBUS ||
        uint8_value == PROTOCOL_8100_PMBUS) {
        string_val = PROTERY_PS_PROTOCOL_PMBUS_STR;
    } else if (uint8_value == PROTOCOL_CANBUS) {
        string_val = PROTERY_PS_PROTOCOL_CANBUS_STR;
    }

    add_property_value_string(obj_huawei, RFPROP_POWER_SUPPLIES_PROTOCOL, string_val, ret_dal);
}

LOCAL void add_active_standby_for_powersupplies_oem(json_object* obj_huawei, OBJ_HANDLE handle_onepower)
{
    gint32 ret_dal;
    guint8 uint8_value = 0xff;
    gchar* string_val = NULL;

    ret_dal = dal_get_property_value_byte(handle_onepower, PROPERTY_ACTUAL_ACTIVE, &uint8_value);

    string_val = NULL;

    if (RF_POWER_CONTROL_PSU_ACTIVE_POWER == uint8_value) {
        string_val = RF_POWER_CONTROL_PSU_ACTIVE_POWER_STR;
    } else if (RF_POWER_CONTROL_PSU_STANDBY_POWER == uint8_value) {
        string_val = RF_POWER_CONTROL_PSU_STANDBY_POWER_STR;
    }

    add_property_value_string(obj_huawei, RFPROP_POWER_SUPPLIES_ACTIVESTANDBY, string_val, ret_dal);
}

LOCAL void add_power_supply_channel_for_powersupplies_oem(json_object* obj_huawei, OBJ_HANDLE handle_onepower)
{
    gint32 ret_dal;
    guint8 uint8_value = 0xff;
    gchar* string_val = NULL;

    ret_dal = dal_get_property_value_byte(handle_onepower, PROTERY_PS_POWERSUPPLY_CHANNEL, &uint8_value);

    string_val = NULL;
    if (uint8_value == PROPERTY_POWERSUPPLY_CHANNEL_VALUE_MAIN_CIRCUIT) {
        string_val = RF_POWERSUPPLY_CHANNEL_VALUE_MAIN_CIRCUIT;
    } else if (uint8_value == PROPERTY_POWERSUPPLY_CHANNEL_VALUE_BACKUP_CIRCUIT) {
        string_val = RF_POWERSUPPLY_CHANNEL_VALUE_BACKUP_CIRCUIT;
    }
    add_property_value_string(obj_huawei, RFPROP_POWER_SUPPLIES_POWERSUPPLY_CHANNEL, string_val, ret_dal);
}


LOCAL void get_chassis_power_powersupplies_oem(json_object *o_element_jso, OBJ_HANDLE handle_onepower)
{
    gint32 ret_dal;
    guint8 uint8_value;
    gdouble double_value = 0.0;
    gchar string_value[POWER_STR_MAX_LEN] = {0};

    json_object *obj_oem = NULL;
    json_object *obj_huawei = NULL;

    obj_oem = json_object_new_object();
    json_object_object_add(o_element_jso, RFPROP_FWINV_OEM, obj_oem);
    return_if_expr(NULL == obj_oem);

    obj_huawei = json_object_new_object();
    json_object_object_add(obj_oem, RFPROP_FWINV_HUAWEI, obj_huawei);
    return_if_expr(NULL == obj_huawei);

    
    if (handle_onepower == 0) {
        add_null_object_for_powersupplies_oem(obj_huawei);
        return;
    }

    
    add_protocol_for_powersupplies_oem(obj_huawei, handle_onepower);

    
    add_active_standby_for_powersupplies_oem(obj_huawei, handle_onepower);

    
    ret_dal = dal_get_property_value_double(handle_onepower, PROTERY_PS_INPUTPOWER, &double_value);
    add_property_value_int(obj_huawei, RFPROP_POWER_SUPPLIES_INPUT_POWER, (gint32)(double_value + 0.5), ret_dal);

    
    ret_dal = dal_get_property_value_double(handle_onepower, PROTERY_PS_IIN, &double_value);
    add_property_value_double(obj_huawei, RFPROP_POWER_SUPPLIES_INPUT_CURRENT, double_value, ret_dal,
        ACCURACY_FORMAT_3F);

    
    ret_dal = dal_get_property_value_double(handle_onepower, PROTERY_PS_OUTPUTPOWER, &double_value);
    add_property_value_int(obj_huawei, RFPROP_POWER_SUPPLIES_OUTPUT_POWER, (gint32)(double_value + 0.5), ret_dal);

    
    ret_dal = dal_get_property_value_double(handle_onepower, PROTERY_PS_IOUT, &double_value);
    add_property_value_double(obj_huawei, RFPROP_POWER_SUPPLIES_OUTPUT_CURRENT, double_value, ret_dal,
        ACCURACY_FORMAT_3F);

    
    ret_dal = dal_get_property_value_double(handle_onepower, PROTERY_PS_INLETTEMP, &double_value);
    add_property_value_double(obj_huawei, RFPROP_POWER_SUPPLIES_INLET_TEMPERATURE, double_value, ret_dal,
        ACCURACY_FORMAT_3F);

    
    ret_dal = dal_get_property_value_double(handle_onepower, PROTERY_PS_INNERTEMP, &double_value);
    add_property_value_double(obj_huawei, RFPROP_POWER_SUPPLIES_INNER_TEMPERATURE, double_value, ret_dal,
        ACCURACY_FORMAT_3F);

    
    ret_dal = dal_get_property_value_double(handle_onepower, PROTERY_PS_VOUT, &double_value);
    add_property_value_double(obj_huawei, RFPROP_POWER_SUPPLIES_OUTPUT_VOLTAGE, double_value, ret_dal,
        ACCURACY_FORMAT_3F);

    
    ret_dal = dal_get_property_value_string(handle_onepower, PROTERY_PS_DEVICENAME, string_value, sizeof(string_value));
    add_property_value_string(obj_huawei, RFPROP_DEVICELOCATOR, string_value, ret_dal);

    
    ret_dal = dal_get_property_value_byte(handle_onepower, PROTERY_PS_SLOT_ID, &uint8_value);
    add_property_value_uint32(obj_huawei, RFPROP_POWER_SUPPLIES_SLOTNUM, (guint32)uint8_value, ret_dal);

    rf_add_property_jso_string(handle_onepower, PROTERY_PS_MANUFACTUREDATE, RFPROP_POWER_SUPPLIES_MANUFACTUREDATE,
        obj_huawei);

    rf_add_property_jso_uint16(handle_onepower, PROTERY_PS_I_OUT_RATE, RFPROP_POWER_SUPPLIES_CURRENT_CAPACITY,
        obj_huawei);

    rf_add_property_jso_uint32(handle_onepower, PROTERY_PS_TOTAL_RUNNING_TIME, RFPROP_POWER_SUPPLIES_TOTAL_RUNNING_TIME,
        obj_huawei);

    ret_dal = dal_get_property_value_double(handle_onepower, PROTERY_PS_FREQUENCY, &double_value);
    add_property_value_double(obj_huawei, RFPROP_POWER_SUPPLIES_FREQUENCY, double_value, ret_dal, ACCURACY_FORMAT_3F);

    add_power_supply_channel_for_powersupplies_oem(obj_huawei, handle_onepower);

    ret_dal = dal_get_property_value_double(handle_onepower, PROTERY_PS_VIN_CHANNEL_A, &double_value);
    add_property_value_double(obj_huawei, RFPROP_POWER_SUPPLIES_VIN_CHANNEL_A, double_value, ret_dal,
        ACCURACY_FORMAT_3F);

    ret_dal = dal_get_property_value_double(handle_onepower, PROTERY_PS_VIN_CHANNEL_B, &double_value);
    add_property_value_double(obj_huawei, RFPROP_POWER_SUPPLIES_VIN_CHANNEL_B, double_value, ret_dal,
        ACCURACY_FORMAT_3F);

    
    ret_dal = dal_get_property_value_string(handle_onepower, PROTERY_PS_LOCATION, string_value, sizeof(string_value));
    add_property_value_string(obj_huawei, RFPROP_POSITION, string_value, ret_dal);

    uint8_value = PROTOCOL_INVALID;
    (void)dal_get_property_value_byte(handle_onepower, PROTERY_PS_PROTOCAL, &uint8_value);
    if (uint8_value == PROTOCOL_CANBUS) {
        add_powersupplies_oem_for_canbus(handle_onepower, obj_huawei);
    }
}


LOCAL void get_chassis_power_supply_redundancy(json_object *obj_elem, const gchar *slot_str, guint8 work_mode)
{
    gint32 ret;
    gchar string_value[POWER_STR_MAX_LEN] = {0};
    json_object *obj_redundancy_array = NULL;
    json_object *obj_redundancy_elem = NULL;
    json_object *obj = NULL;

    obj_redundancy_array = json_object_new_array();
    json_object_object_add(obj_elem, RFPROP_POWER_REDUNDANCY, obj_redundancy_array);
    return_if_expr(NULL == obj_redundancy_array);

    obj_redundancy_elem = json_object_new_object();
    json_object_array_add(obj_redundancy_array, obj_redundancy_elem);
    return_if_expr(NULL == obj_redundancy_elem);

    
    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1, RFPROP_REDUNDANCY_ODATA_ID_VALUE,
        slot_str, work_mode);
    (0 >= ret) ? (obj = NULL) : (obj = json_object_new_string((const gchar *)string_value));

    json_object_object_add(obj_redundancy_elem, RFPROP_ODATA_ID, obj);
}


LOCAL void get_chassis_power_supply_nth(json_object *obj_array, OBJ_HANDLE handle_onepower, const gchar *slot_str,
    guint8 arr_index, guint8 work_mode)
{
    gchar string_value[POWER_STR_MAX_LEN] = {0};
    gdouble double_value = 0.0;
    guint16 uint16_value = 0;
    guint8 uint8_value = 0xff;
    gchar *string_val = NULL;

    json_object *obj = NULL;

    
    json_object *obj_elem = json_object_new_object();
    return_do_info_if_fail(obj_elem != NULL, json_object_array_add(obj_array, NULL));

    
    gint32 ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1,
        URI_FORMAT_PS, slot_str, arr_index);
    (ret <= 0) ? (obj = NULL) : (obj = json_object_new_string((const gchar *)string_value));

    json_object_object_add(obj_elem, RFPROP_ODATA_ID, obj);

    
    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1, "%u", arr_index);
    (ret <= 0) ? (obj = NULL) : (obj = json_object_new_string((const gchar *)string_value));

    json_object_object_add(obj_elem, MEMBERID_STRING, obj);

    
    ret = sprintf_s(string_value, sizeof(string_value), POWER_SUPPLY_UNIT_NAME "%d",
        dal_match_product_id(PRODUCT_ID_PANGEA_V6) ? arr_index : (arr_index + 1));
    (ret <= 0) ? (obj = NULL) : (obj = json_object_new_string((const gchar *)string_value));

    json_object_object_add(obj_elem, RFPROP_COMMON_NAME, obj);

    
    get_chassis_power_powersupplies_status(obj_elem, handle_onepower);

    if (handle_onepower == 0) {
        
        json_object_object_add(obj_elem, RFPROP_POWER_SUPPLIES_TYPE, NULL);
        json_object_object_add(obj_elem, RFPROP_POWER_SUPPLIES_INPUT_VOLTAGE, NULL);
        json_object_object_add(obj_elem, RFPROP_POWER_SUPPLIES_CAPACITY, NULL);
        json_object_object_add(obj_elem, RFPROP_POWER_SUPPLIES_MODEL, NULL);
        json_object_object_add(obj_elem, RFPROP_POWER_SUPPLIES_FIRMWARE_VER, NULL);
        json_object_object_add(obj_elem, RFPROP_POWER_SUPPLIES_SERIAL_NUM, NULL);
        get_chassis_power_supply_redundancy(obj_elem, slot_str, work_mode);
        json_object_object_add(obj_elem, RFPROP_POWER_SUPPLIES_MANUFACTURER, NULL);
        
        json_object_object_add(obj_elem, RFPROP_PART_NUMBER, NULL);
        
    } else {
        
        ret = dal_get_property_value_byte(handle_onepower, PROTERY_PS_INPUTMODE, &uint8_value);
        if (uint8_value == PROTERY_PS_INPUTMODE_VALUE_DC) { // 0对应DC
            string_val = RF_POWER_SUPPLIES_TYPE_VALUE_DC;
        } else if (uint8_value == PROTERY_PS_INPUTMODE_VALUE_AC) { // 1对应AC
            string_val = RF_POWER_SUPPLIES_TYPE_VALUE_AC;
        } else if (uint8_value == PROTERY_PS_INPUTMODE_VALUE_AC_OR_DC) { // 2对应ACorDC
            string_val = RF_POWER_SUPPLIES_TYPE_VALUE_AC_OR_DC;
        }

        add_property_value_string(obj_elem, RFPROP_POWER_SUPPLIES_TYPE, string_val, ret);

        
        ret = dal_get_property_value_double(handle_onepower, PROTERY_PS_VIN, &double_value);
        add_property_value_int(obj_elem, RFPROP_POWER_SUPPLIES_INPUT_VOLTAGE, (gint32)double_value, ret);

        
        ret = dal_get_property_value_uint16(handle_onepower, PROTERY_PS_RATING, &uint16_value);
        add_property_value_int(obj_elem, RFPROP_POWER_SUPPLIES_CAPACITY, (gint32)uint16_value, ret);

        
        ret = dal_get_property_value_string(handle_onepower, PROTERY_PS_MODEL, string_value, sizeof(string_value));
        add_property_value_string(obj_elem, RFPROP_POWER_SUPPLIES_MODEL, string_value, ret);

        
        get_chassis_power_powersupplies_fmver(obj_elem, handle_onepower);

        
        ret = dal_get_property_value_string(handle_onepower, PROTERY_PS_SN, string_value, sizeof(string_value));
        add_property_value_string(obj_elem, RFPROP_POWER_SUPPLIES_SERIAL_NUM, string_value, ret);

        
        get_chassis_power_supply_redundancy(obj_elem, slot_str, work_mode);

        
        ret =
            dal_get_property_value_string(handle_onepower, PROTERY_PS_MANUFACTURER, string_value, sizeof(string_value));
        add_property_value_string(obj_elem, RFPROP_POWER_SUPPLIES_MANUFACTURER, string_value, ret);

        
        ret = dal_get_property_value_string(handle_onepower, PROTERY_PS_PART_NUM, string_value, sizeof(string_value));
        add_property_value_string(obj_elem, RFPROP_PART_NUMBER, string_value, ret);
        
    }

    
    get_chassis_power_powersupplies_oem(obj_elem, handle_onepower);

    
    json_object_array_add(obj_array, obj_elem);
}


LOCAL gint32 get_chassis_power_powersupplies(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint8 i;
    guint8 psu_num = 0;
    guint8 work_mode = 0xff;

    gchar slot_str[MAX_RSC_ID_LEN] = {0};

    OBJ_HANDLE obj_handle = 0;

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    
    ret = check_enclosure_component_type(i_paras->obj_handle, TRUE);
    
    return_val_if_fail(ret == VOS_OK, HTTP_INTERNAL_SERVER_ERROR);
    

    
    ret = get_power_management_supported(&psu_num);
    return_val_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR);

    
    return_val_do_info_if_expr(psu_num == 0, HTTP_OK, *o_result_jso = json_object_new_array(););

    
    ret = dfl_get_object_handle(OBJ_NAME_AMMETER, &obj_handle);
    return_val_do_info_if_expr(ret != VOS_OK, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get OBJ_PRODUCT_COMPONENT handle fail", __FUNCTION__));

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_ACTUAL_POWER_MODE, &work_mode);
    return_val_do_info_if_expr(ret != VOS_OK, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get PROPERTY_ACTUAL_POWER_MODE fail", __FUNCTION__));

    ret = rf_gen_chassis_component_id(NULL, i_paras->obj_handle, slot_str, MAX_RSC_ID_LEN);
    return_val_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR);

    
    *o_result_jso = json_object_new_array();
    return_val_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);

    for (i = 0; i < psu_num; i++) {
        ret = get_ps_handle_by_slot(i, &obj_handle);
        do_val_if_expr(ret != VOS_OK, obj_handle = 0;);

        get_chassis_power_supply_nth(*o_result_jso, obj_handle, slot_str, i, work_mode);
    }

    return HTTP_OK;
}


LOCAL void add_redundancy_set(json_object *obj_elem, const gchar *slot_str, guint8 ps_num, guchar work_mode)
{
    json_object *obj_array = NULL;
    OBJ_HANDLE syspower_object_handle = 0;
    guint32 mask = 0;

    obj_array = json_object_new_array();
    json_object_object_add(obj_elem, RFPROP_POWER_REDUNDANCY_SET, obj_array);
    return_if_expr(NULL == obj_array);

    // 如果主备供电，只显示备用电源
    if (work_mode == POWER_CFG_MASTER_SPARE) {
        (void)dfl_get_object_handle(OBJ_NAME_AMMETER, &syspower_object_handle);
        (void)dal_get_property_value_uint32(syspower_object_handle, PROPERTY_EXPECTED_ACTIVE, &mask);
        add_psu_to_array_with_mask(obj_array, ps_num, slot_str, &mask);
    } else {
        add_psu_to_array_with_mask(obj_array, ps_num, slot_str, NULL);
    }
}


LOCAL void add_redundancy_elem_nth(json_object *obj_array, const gchar *slot_str, guint8 arr_index, guint8 ps_num,
    guchar work_mode)
{
    gint32 ret;
    guint8 uint8_value = 0xff;
    json_object *obj_elem = NULL;
    json_object *obj = NULL;
    const gchar *str = NULL;
    gchar string_value[POWER_STR_MAX_LEN] = {0};
    OBJ_HANDLE obj_handle = 0;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guchar component_type = 0;
    guchar devicenum = 0;

    
    obj_elem = json_object_new_object();
    return_do_info_if_fail(NULL != obj_elem, json_object_array_add(obj_array, NULL));

    
    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1, RFPROP_REDUNDANCY_ODATA_ID_VALUE,
        slot_str, arr_index);
    (0 >= ret) ? (obj = NULL) : (obj = json_object_new_string((const gchar *)string_value));

    json_object_object_add(obj_elem, RFPROP_ODATA_ID, obj);

    
    json_object_object_add(obj_elem, RFPROP_ODATA_TYPE, json_object_new_string(RFPROP_REDUNDANCY_ODATA_TYPE_VALUE));

    
    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1, "%u", arr_index);
    (0 >= ret) ? (obj = NULL) : (obj = json_object_new_string((const gchar *)string_value));

    json_object_object_add(obj_elem, MEMBERID_STRING, obj);

    
    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1, RF_POWER_REDUNDANCY_NAME_VALUE,
        arr_index + 1);
    (0 >= ret) ? (obj = NULL) : (obj = json_object_new_string((const gchar *)string_value));

    json_object_object_add(obj_elem, RFPROP_COMMON_NAME, obj);

    
    (0 == arr_index) ? (str = RF_POWER_REDUNDANCY_MODE_SHARING) : (str = RF_POWER_REDUNDANCY_MODE_FAILOVER);
    json_object_object_add(obj_elem, RFPROP_POWER_REDUNDANCY_MODE, json_object_new_string(str));

    
    json_object_object_add(obj_elem, RFPROP_POWER_REDUNDANCY_MAX_NUM_SUPPORTED, json_object_new_int(ps_num));

    
    json_object_object_add(obj_elem, RFPROP_POWER_REDUNDANCY_MIN_NUM_NEEDED,
        json_object_new_int(RF_POWER_REDUNDANCY_MIN_NUM_NEEDED_VALUE));

    
    add_redundancy_set(obj_elem, slot_str, ps_num, work_mode);

    
    if (arr_index == work_mode) {
        ret = dfl_get_object_list(CLASS_COMPONENT, &obj_list);
        
        do_if_expr(0 != ret, debug_log(DLOG_ERROR, "%s, %d: dfl_get_object_list fail.\n", __FUNCTION__, __LINE__));
        
        do_if_expr(NULL == obj_list, debug_log(DLOG_MASS, "%s, %d: get obj_list fail.\n", __FUNCTION__, __LINE__));

        for (obj_node = obj_list; obj_node; obj_node = g_slist_next(obj_node)) {
            (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_DEVICE_TYPE,
                &component_type);
            (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_DEVICENUM, &devicenum);
            break_do_info_if_expr((COMPONENT_SPECIAL_DEVICE_NUM == devicenum) && (COMPONENT_TYPE_PS == component_type),
                (obj_handle = (OBJ_HANDLE)obj_node->data));
        }

        do_if_expr(NULL != obj_list, g_slist_free(obj_list));

        if (0 != obj_handle) {
            (void)dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_HEALTH, &uint8_value);
        }
    }
    (void)get_resource_status_property(&uint8_value, NULL, ENABLED_STRING, &obj, FALSE);
    json_object_object_add(obj_elem, RF_PROPERTY_STATUS, obj);

    
    json_object_array_add(obj_array, obj_elem);
}


LOCAL gint32 convert_redundancyset_to_ps_id(SET_POWER_REDUNDANCY_INFO_S *patch_info, json_object *obj_jso_redundancyset,
                                            guint8 *ps_ids, guint8 *ps_num)
{
    json_object *elem = NULL;
    json_object *elem_odataid = NULL;
    json_bool bool_jso = FALSE;
    const gchar *str = NULL;
    gchar slot_str[MAX_RSC_ID_LEN] = {0};
    gint32 arr_len = json_object_array_length(obj_jso_redundancyset);

    gint32 ret = get_power_management_supported(ps_num);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get power num fail.");
        return RET_ERR;
    }

    ret = redfish_get_board_slot(slot_str, sizeof(slot_str) - 1);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get slot str fail.");
        return RET_ERR;
    }
    for (guint8 i = 0; i < (guint8)arr_len; i++) {
        elem = json_object_array_get_idx(obj_jso_redundancyset, i);
        if (elem == NULL || json_object_object_length(elem) == 0) {
            debug_log(DLOG_ERROR, "property item few");
            return RET_ERR;
        }
        bool_jso = json_object_object_get_ex(elem, RFPROP_POWER_REDUNDANCY_ODATA_ID, &elem_odataid);
        if (bool_jso == FALSE) {
            continue;
        }
        str = dal_json_object_get_str(elem_odataid);
        if (str == NULL) {
            return RET_ERR;
        }
        if (is_valid_psu_odata_id((SET_POWER_LIMIT_INFO_S *)patch_info, slot_str, str, ps_ids, i, *ps_num) == FALSE) {
            return RET_ERR;
        }
    }
    return RET_OK;
}


LOCAL void set_failover_input_para(guint8 *ps_ids, gint32 arr_len, guint8 ps_num, guint8 nar_value, GSList **input_list)
{
    *input_list = g_slist_append(*input_list, g_variant_new_byte(RF_POWER_CONTROL_PSU_ACTIVE_STANDBY));
    *input_list = g_slist_append(*input_list, g_variant_new_byte(nar_value));
    // ps_ids是备用电源的合集，需要传入主用电源id
    for (guint8 id = 0; id < ps_num; id++) {
        guint8 is_redundancy_ps = FALSE;
        for (guint8 j = 0; j < arr_len; j++) {
            if (id == ps_ids[j]) {
                is_redundancy_ps = TRUE;
            }
        }
        if (is_redundancy_ps == FALSE) {
            *input_list = g_slist_append(*input_list, g_variant_new_byte(id));
        }
    }
    return;
}


LOCAL gint32 solve_ps_workmode_return_value(SET_POWER_REDUNDANCY_INFO_S *patch_info, gint32 ret)
{
    gchar str_prop_mode[PROP_VAL_LENGTH] = {0};
    gchar str_prop_set[PROP_VAL_LENGTH] = {0};
    gchar str_prop_redundancy[PROP_VAL_LENGTH] = {0};
    json_object *obj_msg = NULL;
    gint32 ret_val1, ret_val2, ret_val3;

    if (ret == RET_OK) {
        return RET_OK;
    }

    ret_val1 = snprintf_s(str_prop_mode, sizeof(str_prop_mode), sizeof(str_prop_mode) - 1, RFPROP_REDUNDANCY_MODE_PATH,
                                patch_info->arr_index);
    ret_val2 = snprintf_s(str_prop_set, sizeof(str_prop_set), sizeof(str_prop_set) - 1, RFPROP_REDUNDANCY_SET_PATH,
                                patch_info->arr_index);
    ret_val3 = snprintf_s(str_prop_redundancy, sizeof(str_prop_redundancy), sizeof(str_prop_redundancy) - 1,
                                RFPROP_REDUNDANCY_PATH, patch_info->arr_index);
    if (ret_val1 <= 0 || ret_val2 <= 0 || ret_val3 <= 0) {
        debug_log(DLOG_ERROR, "snprintf_s fail, ret1 = %d, ret2 = %d, ret3 = %d", ret_val1, ret_val2, ret_val3);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &obj_msg);
        json_object_array_add(patch_info->o_message_jso, obj_msg);
        return RET_ERR;
    }

    for (guint32 i = 0; i < G_N_ELEMENTS(set_ps_workmode_return_mess_list); i++) {
        if (set_ps_workmode_return_mess_list[i].error_code != ret) {
            continue;
        }
        if (set_ps_workmode_return_mess_list[i].parameter_cnt == NEED_TWO_PARAMETER) {
            (void)create_message_info((const gchar *)set_ps_workmode_return_mess_list[i].error_type,
                (const gchar *)str_prop_redundancy, &obj_msg, (const gchar *)str_prop_set,
                (const gchar *)str_prop_mode);
        } else if (set_ps_workmode_return_mess_list[i].parameter_cnt == NEED_ONE_PARAMETER) {
            (void)create_message_info((const gchar *)set_ps_workmode_return_mess_list[i].error_type,
                (const gchar *)str_prop_redundancy, &obj_msg, (const gchar *)str_prop_mode);
        } else if (set_ps_workmode_return_mess_list[i].parameter_cnt == NOT_NEED_PARAMETER) {
            (void)create_message_info((const gchar *)set_ps_workmode_return_mess_list[i].error_type,
                (const gchar *)str_prop_redundancy, &obj_msg);
        }
        (void)json_object_array_add(patch_info->o_message_jso, obj_msg);
        return RET_ERR;
    }

    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &obj_msg);
    (void)json_object_array_add(patch_info->o_message_jso, obj_msg);

    return RET_ERR;
}

LOCAL gint32 get_redundancy_mode_and_set_json_obj(SET_POWER_REDUNDANCY_INFO_S *patch_info, json_object **obj_jso_mode,
                                                  json_object **obj_jso_redundancyset)
{
    json_bool ret_bool;
    gchar str_prop[PROP_VAL_LENGTH] = {0};
    gint32 ret_val;
    json_object* obj_msg = NULL;

    ret_bool = json_object_object_get_ex(patch_info->arr_elem_jso, RFPROP_POWER_REDUNDANCY_MODE, obj_jso_mode);
    ret_val = snprintf_s(str_prop, sizeof(str_prop), sizeof(str_prop) - 1, RFPROP_REDUNDANCY_MODE_PATH,
                                patch_info->arr_index);
    if (ret_val < 0) {
        debug_log(DLOG_ERROR, "snprintf_s fail, ret = %d", ret_val);
        return RET_ERR;
    }

    if (ret_bool == FALSE || *obj_jso_mode == NULL) {
        debug_log(DLOG_ERROR, "get obj_jso_mode error.");
        (void)create_message_info(MSGID_MODIFY_FAIL_LACK_PROPERTY, (const gchar *)str_prop,
            &obj_msg, (const gchar *)str_prop, (const gchar *)str_prop);
        json_object_array_add(patch_info->o_message_jso, obj_msg);
        return RET_ERR;
    }
    
    if (g_strcmp0(dal_json_object_get_str(*obj_jso_mode), RF_POWER_REDUNDANCY_MODE_SHARING) == 0) {
        return RET_OK;
    }
    ret_bool = json_object_object_get_ex(patch_info->arr_elem_jso, RFPROP_POWER_REDUNDANCY_SET, obj_jso_redundancyset);
    ret_val = snprintf_s(str_prop, sizeof(str_prop), sizeof(str_prop) - 1, RFPROP_REDUNDANCY_SET_PATH,
                                patch_info->arr_index);
    if (ret_val < 0) {
        debug_log(DLOG_ERROR, "snprintf_s fail, ret = %d", ret_val);
        return RET_ERR;
    }
    if (ret_bool == FALSE || *obj_jso_redundancyset == NULL || json_object_array_length(*obj_jso_redundancyset) == 0) {
        debug_log(DLOG_ERROR, "get obj_jso_redundancyset error.");
        (void)create_message_info(MSGID_MODIFY_FAIL_LACK_PROPERTY, (const gchar *)str_prop,
            &obj_msg, (const gchar *)str_prop, (const gchar *)str_prop);
        json_object_array_add(patch_info->o_message_jso, obj_msg);
        return RET_ERR;
    }
    
    return RET_OK;
}


LOCAL gint32 set_power_redundancy_ps_work_mode(SET_POWER_REDUNDANCY_INFO_S *patch_info)
{
    gint32 arr_len;
    guint8 *ps_ids = NULL;
    GSList *input_list = NULL;
    json_object *obj_jso_mode = NULL;
    json_object *obj_jso_redundancyset = NULL;
    guint8 ps_num = 0;
    guint8 nar_value = get_powercontrol_input_normal_and_redundancy(NULL);

    gint32 ret = get_redundancy_mode_and_set_json_obj(patch_info, &obj_jso_mode, &obj_jso_redundancyset);
    if (ret != RET_OK) {
        return RET_ERR;
    }
    
    if (g_strcmp0(dal_json_object_get_str(obj_jso_mode), RF_POWER_REDUNDANCY_MODE_SHARING) == 0) {
        // 设置负载均衡,默认都是主用电源,不在意RedundancySet属性
        input_list = g_slist_append(input_list, g_variant_new_byte(RF_POWER_CONTROL_PSU_LOAD_BALANCE));
        input_list = g_slist_append(input_list, g_variant_new_byte(nar_value));
    } else if (g_strcmp0(dal_json_object_get_str(obj_jso_mode), RF_POWER_REDUNDANCY_MODE_FAILOVER) == 0) {
        // 设置电源冗余模式为主备供电
        arr_len = json_object_array_length(obj_jso_redundancyset);
        if (arr_len == 0) {
            debug_log(DLOG_ERROR, "arr_len is 0");
            return RET_ERR;
        }
        ps_ids = (guint8 *)g_malloc(sizeof(guint8) * arr_len);
        if (ps_ids == NULL) {
            debug_log(DLOG_ERROR, "g_malloc fail");
            return RET_ERR;
        }
        (void)memset_s(ps_ids, arr_len, 0xff, arr_len);
        ret = convert_redundancyset_to_ps_id(patch_info, obj_jso_redundancyset, ps_ids, &ps_num);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "convert redundancyset to ps id fail");
            g_free(ps_ids);
            return RET_ERR;
        }
        set_failover_input_para(ps_ids, arr_len, ps_num, nar_value, &input_list);
        g_free(ps_ids);
    } else {
        debug_log(DLOG_ERROR, "not sharing or failover work mode.");
        return RET_ERR;
    }

    ret = uip_call_class_method_redfish(patch_info->i_paras->is_from_webui, patch_info->i_paras->user_name,
                                        patch_info->i_paras->client, 0, CLASS_NAME_AMMETER,
                                        METHOD_SET_PLANTFORM_WORKMODE, AUTH_DISABLE,
                                        patch_info->i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    return solve_ps_workmode_return_value(patch_info, ret);
}


LOCAL gint32 set_chassis_power_redundancy(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
                                          json_object **o_result_jso)
{
    gint32 ret;
    SET_POWER_REDUNDANCY_INFO_S power_redundancy_info;

    if (o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "para null pointer.");
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_POWERMGNT, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_POWER_REDUNDANCY, o_message_jso,
        RFPROP_POWER_REDUNDANCY));
    
    int len = json_object_array_length(i_paras->val_jso);
    (void)memset_s(&power_redundancy_info, sizeof(power_redundancy_info), 0, sizeof(power_redundancy_info));
    *o_message_jso = json_object_new_array();
    power_redundancy_info.i_paras = i_paras;
    power_redundancy_info.o_message_jso = *o_message_jso;

    for (int i = 0; i < len; i++) {
        power_redundancy_info.arr_index = i;
        power_redundancy_info.arr_elem_jso = json_object_array_get_idx(i_paras->val_jso, i);
        ret = set_power_redundancy_ps_work_mode(&power_redundancy_info);
        if (ret == RET_ERR) {
            debug_log(DLOG_ERROR, "set power work mode failed.");
            return HTTP_BAD_REQUEST;
        }
    }

    return HTTP_OK;
}


LOCAL gint32 get_chassis_power_redundancy(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint8 psu_num = 0;
    OBJ_HANDLE obj_handle = 0;
    guchar work_mode = 0;
    gchar slot_str[MAX_RSC_ID_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = get_power_management_supported(&psu_num);
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    
    return_val_do_info_if_expr(0 == psu_num, HTTP_OK, (*o_result_jso = json_object_new_array()));

    
    
    ret = check_enclosure_component_type(i_paras->obj_handle, TRUE);
    
    return_val_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR);
    

    ret = rf_gen_chassis_component_id(NULL, i_paras->obj_handle, slot_str, MAX_RSC_ID_LEN);
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    
    *o_result_jso = json_object_new_array();
    return_val_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR);

    
    ret = dfl_get_object_handle(OBJ_NAME_AMMETER, &obj_handle);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get OBJ_PRODUCT_COMPONENT handle fail", __FUNCTION__));

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_EXPECTED_POWER_MODE, &work_mode);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get PROPERTY_EXPECTED_POWER_MODE fail", __FUNCTION__));

    // 只显示当前电源模式
    add_redundancy_elem_nth(*o_result_jso, slot_str, work_mode, psu_num, work_mode);

    return HTTP_OK;
}


LOCAL gint32 get_chassis_power_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;
    gchar string_value[ARRAY_LENTH] = {0};
    gchar slot_str[MAX_RSC_ID_LEN] = {0};
    gint32 ret;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    

    
    // 获取 @odata.id
    ret = rf_gen_chassis_component_id(NULL, i_paras->obj_handle, slot_str, MAX_RSC_ID_LEN);
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);
    iRet = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1, "%s%s/%s", RF_CHASSIS_UI, slot_str,
        RFPROP_CASSIS_POWER);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    

    *o_result_jso = json_object_new_string((const gchar *)string_value);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}

LOCAL gint32 get_obj_action(json_object* obj_actions, const gchar* slot_str, const gchar* rf_prop,
    const gchar* rf_target, const gchar* rf_action_info)
{
    gint32 ret;
    gchar string_value[POWER_STR_MAX_LEN * 2] = {0};
    json_object* obj_action = NULL;
    json_object* obj_target = NULL;
    json_object* obj_action_info = NULL;

    
    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1,
        "#%s", rf_prop);
    if (ret < 0) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    obj_action = json_object_new_object();
    json_object_object_add(obj_actions, (const gchar*)string_value, obj_action);
    if (obj_action == NULL) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1, rf_target, slot_str);
#pragma GCC diagnostic pop
    if (ret < 0) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    obj_target = json_object_new_string((const gchar*)string_value);
    json_object_object_add(obj_action, RFPROP_TARGET, obj_target);
    if (obj_target == NULL) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1, rf_action_info, slot_str);
#pragma GCC diagnostic pop
    if (ret < 0) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    obj_action_info = json_object_new_string((const gchar*)string_value);
    json_object_object_add(obj_action, RFPROP_ACTION_INFO, obj_action_info);
    if (obj_action_info == NULL) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return RET_OK;
}

LOCAL gint32 get_power_oem_actions(json_object* obj_huawei, const gchar* slot_str)
{
    gint32 ret;
    json_object* obj_actions = NULL;
    guint8 enabled = DISABLE;
    OBJ_HANDLE obj_handle = INVALID_OBJ_HANDLE;

    obj_actions = json_object_new_object();
    json_object_object_add(obj_huawei, RFPROP_ACTIONS, obj_actions);
    if (obj_actions == NULL) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = get_obj_action(obj_actions, slot_str, RFPROP_POWER_OEM_ACTION_RESET_HISTORY_DATA,
        RF_POWER_OEM_ACTION_RESET_TARGET, RF_POWER_OEM_ACTION_RESET_ACTION_INFO);
    if (ret != RET_OK) {
        return ret;
    }

    
    ret = get_obj_action(obj_actions, slot_str, RFPROP_POWER_OEM_ACTION_RESET_STATISTICS,
        RF_POWER_OEM_ACTION_RESET_STATISTICS_TARGET, RF_POWER_OEM_ACTION_RESET_STATISTICS_ACTION_INFO);
    if (ret != RET_OK) {
        return ret;
    }

    
    ret = get_obj_action(obj_actions, slot_str, RFPROP_POWER_OEM_ACTION_COLLECT_HISTORY_DATA,
        RF_POWER_OEM_ACTION_COLLECT_HISTORY_DATA_TARGET, RF_POWER_OEM_ACTION_COLLECT_HISTORY_DATA_ACTION_INFO);
    if (ret != RET_OK) {
        return ret;
    }

    
    ret = dal_get_object_handle_nth(CLASS_NAME_PME_SERVICECONFIG, 0, &obj_handle);
    if (ret == RET_OK) {
        (void)dal_get_property_value_byte(obj_handle, PROTERY_PME_PSU_SOURCE_SWITCH_SUPPORT, &enabled);
    }

    if (enabled) {
        ret = get_obj_action(obj_actions, slot_str, RFPROP_POWER_OEM_ACTION_SET_PSU_SUPPLY_SOURCE,
            RF_POWER_OEM_ACTION_SET_PSU_SUPPLY_SOURCE_TARGET, RF_POWER_OEM_ACTION_SET_PSU_SUPPLY_SOURCE_ACTION_INFO);
        if (ret != RET_OK) {
            return ret;
        }
    }

    return RET_OK;
}



LOCAL gint32 get_chassis_power_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    
	
    if (!(check_enclosure_component_type(i_paras->obj_handle, FALSE) == RET_OK ||
        check_pangea_node_to_enc(i_paras->obj_handle) == TRUE)) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    

    

    
    *o_result_jso = json_object_new_object();
    return_val_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR);

    
    json_object *obj_huawei = json_object_new_object();
    json_object_object_add(*o_result_jso, RFPROP_FWINV_HUAWEI, obj_huawei);
    return_val_if_expr(NULL == obj_huawei, HTTP_INTERNAL_SERVER_ERROR);

    gchar slot_str[POWER_STR_MAX_LEN] = {0};
    gint32 ret = rf_gen_chassis_component_id(NULL, i_paras->obj_handle, slot_str, POWER_STR_MAX_LEN);
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    
    ret = get_power_oem_actions(obj_huawei, slot_str);
    if (ret != RET_OK) {
        return ret;
    }

    
    gchar string_value[POWER_STR_MAX_LEN * 2] = {0};
    memset_s(string_value, sizeof(string_value), 0, sizeof(string_value));
    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1, URI_FORMAT_CHASSIS_POWER_HISTORY_DATA, slot_str);
    do_val_if_expr(ret <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret));

    json_object *obj_odata_id = json_object_new_string((const gchar *)string_value);
    json_object *obj_power_history_data = json_object_new_object();
    json_object_object_add(obj_power_history_data, ODATA_ID, obj_odata_id);
    json_object_object_add(obj_huawei, RFPROP_POWER_HISTORY_DATA, obj_power_history_data);

    return HTTP_OK;
}


gint32 act_power_reset_historydata(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    guint8 software_type = 0;

    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != i_paras) && (NULL != i_paras->val_jso),
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    (void)dal_get_software_type(&software_type);
    if (MGMT_SOFTWARE_TYPE_EM != software_type) {
        if (check_pangea_node_to_enc(i_paras->obj_handle) == FALSE) {
        
        
            ret = check_enclosure_component_type(i_paras->obj_handle, FALSE);
		    
            if (ret != RET_OK) {
                (void)create_message_info(MSGID_ACT_NOT_SUPPORTED, NULL,
                    o_message_jso, RFPROP_POWER_OEM_ACTION_RESET_HISTORY_DATA);
                return HTTP_NOT_IMPLEMENTED;
            }
        
        }
    }
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, OBJ_HANDLE_NULL,
        CLASS_NAME_AMMETER, METHOD_PSTOTAL_CLEARRECORD, AUTH_ENABLE, i_paras->user_role_privilege, NULL, NULL);

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
            debug_log(DLOG_ERROR, "%s, %d: ret = %d.\n", __FUNCTION__, __LINE__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 act_power_reset_statistics(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != i_paras) && (NULL != i_paras->val_jso),
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    if (check_pangea_node_to_enc(i_paras->obj_handle) == FALSE) {
    
    
        ret = check_enclosure_component_type(i_paras->obj_handle, FALSE);
	    
        if (ret != RET_OK) {
            (void)create_message_info(MSGID_ACT_NOT_SUPPORTED, NULL,
                o_message_jso, RFPROP_POWER_OEM_ACTION_RESET_STATISTICS);
            return HTTP_NOT_IMPLEMENTED;
        }
    
    }

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, OBJ_HANDLE_NULL,
        CLASS_NAME_AMMETER, METHOD_AMMETER_RESET, AUTH_ENABLE, i_paras->user_role_privilege, NULL, NULL);

    switch (ret) {
        case VOS_OK:
        case RFERR_SUCCESS:
            return HTTP_OK;

            
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
            return RFERR_INSUFFICIENT_PRIVILEGE;

            
        case RFERR_NO_RESOURCE:
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri);
            return HTTP_NOT_FOUND;

            
        default:
            debug_log(DLOG_ERROR, "%s, %d: ret = %d.\n", __FUNCTION__, __LINE__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}



LOCAL gint32 collect_history_data_status(json_object **message_obj, guint8 *progress)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    gint32 retValue;
    gint8 status_byte = 0;
    return_val_if_expr(NULL == progress || message_obj == NULL, RF_OK);

    ret = dal_get_object_handle_nth(CLASS_NAME_AMMETER, 0, &obj_handle);
    retValue =
        dal_get_property_value_byte(obj_handle, PROTERY_SYSPOWER_COLLECT_HISTORY_DATA_STATUS, (guint8 *)&status_byte);
    if (VOS_OK == ret && VOS_OK == retValue) {
        if (0 <= status_byte && 100 >= status_byte) {
            *progress = status_byte;
            return VOS_OK;
        }
    }

    debug_log(DLOG_ERROR, "act_collect_history_data_process, error code:%d, code2:%d", ret, retValue);
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);

    return VOS_ERR;
}


LOCAL gint32 act_collect_history_data_process(EXPORT_ACTION_ARGS *action_args, json_object **o_message_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    GSList *input_list = NULL;

    if (o_message_jso == NULL || action_args == NULL) {
        return VOS_ERR;
    }

    ret = dal_get_object_handle_nth(CLASS_NAME_AMMETER, 0, &obj_handle);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_ACT_NOT_SUPPORTED, NULL, o_message_jso,
        RFPROP_POWER_OEM_ACTION_COLLECT_HISTORY_DATA);
        debug_log(DLOG_ERROR, "%s: get syspower obj failed, ret: %d", __FUNCTION__, ret));

    input_list = g_slist_append(input_list, g_variant_new_string(POWER_HISTORY_DATA_CSV_TAR_NAME));

    
    
    ret = uip_call_class_method_redfish(action_args->is_from_webui, action_args->user_name, action_args->client,
        obj_handle, CLASS_NAME_AMMETER, METHOD_COLLECT_HISTOTY_DATA, AUTH_DISABLE, 0, input_list, NULL);
    

    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
        case RFERR_SUCCESS:
            return VOS_OK;
        case RFERR_NO_RESOURCE:
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, action_args->uri);
            return HTTP_NOT_FOUND;
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
            return HTTP_FORBIDDEN;
            
        default:
            debug_log(DLOG_ERROR, "act_collect_history_data_process, error code:%d", ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


gint32 act_power_collect_history_data(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    EXPORT_TRANSACTION_INFO_S export_info = { 0 };

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: input param error.", __FUNCTION__, __LINE__));

    export_info.file_id = FILE_ID_HISTORY_DATA_SCV;
    export_info.permission = USERROLE_POWERMGNT;
    export_info.need_del = 1;
    export_info.get_status = collect_history_data_status;
    export_info.generate_file = act_collect_history_data_process;
    (void)strncpy_s(export_info.src, sizeof(export_info.src), POWER_HISTORY_DATA_CSV_TAR_NAME,
        strlen(POWER_HISTORY_DATA_CSV_TAR_NAME));
    (void)strncpy_s(export_info.file_category, sizeof(export_info.file_category), FILE_CATEGORY_POWER_HISTORY_DATA,
        strlen(FILE_CATEGORY_POWER_HISTORY_DATA));

    return create_export_transaction(i_paras, o_message_jso, o_result_jso, &export_info);
}

typedef struct {
    gulong start_timestamp;
    guint8 supply_source;
} SET_SUPPLY_SOURCE_USERDATA;

LOCAL gint32 set_supply_source_status_monitor(const char* origin_obj_path, json_object* body_jso,
    TASK_MONITOR_INFO_S* monitor_fn_data,
    json_object** message_obj)
{
#define SET_SUPPLY_SOURCE_TIMEOUT_THRESHOLD 300000 // 超时时间5分钟
    gint32 ret;
    GSList* obj_list = NULL;
    GSList* obj_node = NULL;
    OBJ_HANDLE obj_onepower;
    guint8 current_supply_channel;
    guint8 expected_supply_channel;
    guint8 completed_psu_count = 0;
    guint8 total_psu_count;
    gulong cur_timestamp;
    gulong start_timestamp;

    monitor_fn_data->task_state = RF_TASK_RUNNING;
    monitor_fn_data->task_progress = 1;

    expected_supply_channel = ((SET_SUPPLY_SOURCE_USERDATA*)monitor_fn_data->user_data)->supply_source;

    ret = dfl_get_object_list(CLASS_NAME_PS, &obj_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s get psu object list error:%d", __FUNCTION__, ret);
        return ret;
    }

    total_psu_count = g_slist_length(obj_list);
    for (obj_node = obj_list; obj_node != NULL; obj_node = obj_node->next) {
        obj_onepower = (OBJ_HANDLE)obj_node->data;

        ret = dal_get_property_value_byte(obj_onepower, PROTERY_PS_POWERSUPPLY_CHANNEL, &current_supply_channel);
        if ((ret == RET_OK) && (current_supply_channel == expected_supply_channel)) {
            completed_psu_count++;
        }
    }
    g_slist_free(obj_list);

    if (completed_psu_count == total_psu_count) {
        monitor_fn_data->task_state = RF_TASK_COMPLETED;
        monitor_fn_data->task_progress = RF_FINISH_PERCENTAGE;
        return RET_OK;
    }

    monitor_fn_data->task_progress = (completed_psu_count * RF_FINISH_PERCENTAGE) / total_psu_count;

    cur_timestamp = vos_tick_get();
    start_timestamp = ((SET_SUPPLY_SOURCE_USERDATA*)monitor_fn_data->user_data)->start_timestamp;

    ret = dal_check_timestamp_timeout(cur_timestamp, start_timestamp, SET_SUPPLY_SOURCE_TIMEOUT_THRESHOLD);
    if (ret == FALSE) {
        monitor_fn_data->task_state = RF_TASK_RUNNING;
        return RET_OK;
    }

    monitor_fn_data->task_state = RF_TASK_EXCEPTION;
    create_message_info(MSGID_POWER_SET_PSU_SUPPLY_SOURCE_TIMEOUT, NULL, message_obj);
    return RET_OK;
}

LOCAL void process_err_code(gint32 err_code, json_object** o_message_jso)
{
    switch (err_code)
    {
        case ERR_NOT_ALL_BBU_ON_CHARGING:
            (void)create_message_info(MSGID_NOT_ALL_BBU_ON_CHARGING, NULL, o_message_jso);
            break;
        default:
            (void)create_message_info(MSGID_OPERATION_FAILED, NULL, o_message_jso);
            break;
    }
}

LOCAL gint32 create_new_task_for_set_supply_source(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso, guint8 supply_source)
{
#define RF_SET_SUPPLY_SOURCE_TASK_NAME "Set Psu Supply Source Task"
    gint32 ret;
    gint32 ret_code = RET_ERR;
    GSList* input_list = NULL;
    GSList* output_list = NULL;
    TASK_MONITOR_INFO_S* set_supply_source_info = NULL;
    SET_SUPPLY_SOURCE_USERDATA* user_data = NULL;

    // call method
    input_list = g_slist_append(input_list, g_variant_new_byte(supply_source));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        OBJ_HANDLE_NULL, CLASS_NAME_AMMETER, METHOD_SET_PSU_SUPPLY_SOURCE_CFG, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, &output_list);
    uip_free_gvariant_list(input_list);

    if (output_list != NULL) {
        ret_code = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
        uip_free_gvariant_list(output_list);
    }

    if (ret != RET_OK || ret_code != RET_OK) {
        (void)process_err_code(ret_code, o_message_jso);
        debug_log(DLOG_ERROR, "%s: call method %s failed, ret=%d", __FUNCTION__, METHOD_SET_PSU_SUPPLY_SOURCE_CFG, ret);
        return HTTP_BAD_REQUEST;
    }

    // create monitor task
    set_supply_source_info = task_monitor_info_new((GDestroyNotify)g_free);
    if (set_supply_source_info == NULL) {
        debug_log(DLOG_ERROR, "%s: task_monitor_info_new failed", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    user_data = g_malloc0(sizeof(SET_SUPPLY_SOURCE_USERDATA));
    if (user_data == NULL) {
        debug_log(DLOG_ERROR, "%s: g_malloc0 failed", __FUNCTION__);
        task_monitor_info_free(set_supply_source_info);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    user_data->start_timestamp = vos_tick_get();
    user_data->supply_source = supply_source;
    set_supply_source_info->user_data = user_data;
    set_supply_source_info->task_progress = TASK_NO_PROGRESS;
    set_supply_source_info->rsc_privilege = USERROLE_POWERMGNT;

    ret = create_new_task((const gchar*)RF_SET_SUPPLY_SOURCE_TASK_NAME, set_supply_source_status_monitor,
        set_supply_source_info, i_paras->val_jso, i_paras->uri, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: create_new_task failed, ret=%d", __FUNCTION__, ret);
        task_monitor_info_free(set_supply_source_info);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}

LOCAL gint32 act_power_set_psu_supply_source(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_pme_service_conf = OBJ_HANDLE_COMMON;
    guint8 enabled = DISABLE;
    json_bool ret_bool;
    json_object* value_jso = NULL;
    guint8 supply_source = PSU_SUPPLY_MAIN;

    if (provider_paras_check(i_paras) != RET_OK || o_message_jso == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_object_handle_nth(CLASS_NAME_PME_SERVICECONFIG, 0, &obj_pme_service_conf);
    if (ret != RET_OK) {
        (void)create_message_info(MSGID_ACT_NOT_SUPPORTED, NULL, o_message_jso,
            RFPROP_POWER_OEM_ACTION_SET_PSU_SUPPLY_SOURCE);
        return HTTP_BAD_REQUEST;
    }

    (void)dal_get_property_value_byte(obj_pme_service_conf, PROTERY_PME_PSU_SOURCE_SWITCH_SUPPORT, &enabled);
    if (!enabled) {
        (void)create_message_info(MSGID_ACT_NOT_SUPPORTED, NULL, o_message_jso,
            RFPROP_POWER_OEM_ACTION_SET_PSU_SUPPLY_SOURCE);
        return HTTP_BAD_REQUEST;
    }

    if ((i_paras->user_role_privilege & USERROLE_POWERMGNT) == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret_bool = json_object_object_get_ex(i_paras->val_jso, RF_POWER_OEM_ACTION_PARAM_SUPPLY_SOURCE, &value_jso);
    if (ret_bool == FALSE || value_jso == NULL) {
        (void)create_message_info(MSGID_ACT_PARA_MISSING, NULL, o_message_jso,
            RFPROP_POWER_OEM_ACTION_SET_PSU_SUPPLY_SOURCE, RF_POWER_OEM_ACTION_PARAM_SUPPLY_SOURCE);
        return HTTP_BAD_REQUEST;
    }

    if (g_strcmp0(RF_POWERSUPPLY_CHANNEL_VALUE_MAIN_CIRCUIT, dal_json_object_get_str(value_jso)) == 0) {
        supply_source = PSU_SUPPLY_MAIN;
    } else if (g_strcmp0(RF_POWERSUPPLY_CHANNEL_VALUE_BACKUP_CIRCUIT, dal_json_object_get_str(value_jso)) == 0) {
        supply_source = PSU_SUPPLY_BACKUP;
    } else {
        (void)create_message_info(MSGID_ACT_PARA_TYPE_ERR, NULL, o_message_jso, dal_json_object_get_str(value_jso),
            RF_POWER_OEM_ACTION_PARAM_SUPPLY_SOURCE, RFPROP_POWER_OEM_ACTION_SET_PSU_SUPPLY_SOURCE);
        return HTTP_BAD_REQUEST;
    }

    ret = create_new_task_for_set_supply_source(i_paras, o_message_jso, o_result_jso, supply_source);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: create_new_task failed, ret=%d", __FUNCTION__, ret);
        return ret;
    }

    return HTTP_ACCEPTED;
}


gint32 chassis_power_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gboolean ret;

    
    if (check_pangea_uri_in_blade(URI_PATTERN_CHASSIS_BLADE_POWER, i_paras->uri)) {
        return HTTP_NOT_FOUND;
    }

    
    ret = rf_check_chassis_uri_valid_allow_node_enc(i_paras->uri, &i_paras->obj_handle);
    if (!ret || i_paras->obj_handle == 0) {
        return HTTP_NOT_FOUND;
    }

    if (dal_check_if_vsmm_supported()) {
    } else {
        
        
        ret = check_enclosure_component_type(i_paras->obj_handle, TRUE);
        if (ret != RET_OK) {
            return HTTP_NOT_FOUND;
        }
    }

    *prop_provider = g_chassis_power_provider;
    *count = sizeof(g_chassis_power_provider) / sizeof(PROPERTY_PROVIDER_S);

    update_irm_inner_access_node_timestamp(i_paras->client, RF_SYSTEM_POWER);

    return VOS_OK;
}


gint32 chassis_enc_power_provider_entry(PROVIDER_PARAS_S* i_paras, PROPERTY_PROVIDER_S** prop_provider, guint32* count)
{
    gint32 ret;
    ret = dal_get_specific_object_byte(CLASS_COMPONENT, PROPERTY_COMPONENT_DEVICE_TYPE,
        COMPONENT_TYPE_MAINBOARD, &i_paras->obj_handle);
    if (ret != RET_OK || check_enclosure_component_type(i_paras->obj_handle, TRUE) != RET_OK) {
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_chassis_power_provider;
    *count = sizeof(g_chassis_power_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}


gint32 chassis_power_etag_del_property(json_object *object)
{
    SPECIAL_PROP_S value[] = {
        {2, {RFPROP_POWER_CONTROL, RFPROP_POWER_CONTROL_POWER_CONSUME, NULL, NULL, NULL}},
        {2, {RFPROP_POWER_CONTROL, RFPROP_POWER_CONTROL_METRICS, RFPROP_POWER_CONTROL_METRICS_AVERAGE_CONSUME, NULL, NULL}},
        {2, {RFPROP_POWER_CONTROL, RFPROP_POWER_CONTROL_TOTAL_CONSUMED_POWER_OEM_PATH, NULL, NULL, NULL}},
        {2, {RFPROP_POWER_CONTROL, RFPROP_POWER_CONTROL_STATIS_COLL_PATH, NULL, NULL, NULL}},
        {2, {RFPROP_POWER_CONTROL, RFPROP_POWER_CONTROL_MAX_CONSUME_OCC_PATH, NULL, NULL, NULL}},
        {2, {RFPROP_POWER_VOLTAGES, RFPROP_SENSOR_READINGVOLTS, NULL, NULL, NULL}},
        {2, {RFPROP_POWER_CONTROL, RFPROP_POWER_CONTROL_HEAT_OEM_PATH, NULL, NULL, NULL}},
        {2, {RFPROP_POWER_CONTROL, RF_POWER_EXTEND_METRICS_OBJECT RF_POWER_STATISTICS_COLLECTED, NULL, NULL, NULL}},
        {2, {RFPROP_POWER_CONTROL, RF_POWER_EXTEND_METRICS_OBJECT RFPROP_POWER_LIMIT_ACTIVATED, NULL, NULL, NULL}},
        {2, {RFPROP_POWER_CONTROL, RF_POWER_EXTEND_METRICS_OBJECT RFPROP_POWER_LIMIT_FAILED, NULL, NULL, NULL}},
        {2, {RFPROP_POWER_CONTROL, RF_POWER_EXTEND_METRICS_OBJECT RF_POWER_MAX_CONSUMED_POWER_OCCURRED, NULL, NULL, NULL}},
        {2, {RFPROP_POWER_CONTROL, RF_POWER_EXTEND_METRICS_OBJECT RF_POWER_CURRENT_CPU_POWER, NULL, NULL, NULL}},
        {2, {RFPROP_POWER_CONTROL, RF_POWER_EXTEND_METRICS_OBJECT RF_POWER_CURRENT_MEM_POWER, NULL, NULL, NULL}},
        {2, {RFPROP_POWER_CONTROL, RF_POWER_EXTEND_METRICS_OBJECT RF_POWER_TOTAL_CONSUMED_POWER, NULL, NULL, NULL}},
        {2, {RFPROP_POWER_CONTROL, RF_POWER_EXTEND_METRICS_OBJECT RF_MAX_POWER_LIMIT, NULL, NULL, NULL}},
        {2, {RFPROP_POWER_CONTROL, RF_POWER_EXTEND_METRICS_OBJECT RF_MIN_POWER_LIMIT, NULL, NULL, NULL}},
        {2, {RFPROP_POWER_SUPPLIES, RFPROP_POWER_SUPPLIES_INPUT_VOLTAGE, NULL, NULL, NULL}},
        {2, {RFPROP_POWER_SUPPLIES, RFPROP_POWER_SUPPLIES_INLET_TEMPERATURE_OEM_PATH, NULL, NULL, NULL}},
        {2, {RFPROP_POWER_SUPPLIES, RFPROP_POWER_SUPPLIES_INNER_TEMPERATURE_OEM_PATH, NULL, NULL, NULL}},
        {2, {RFPROP_POWER_SUPPLIES, RFPROP_POWER_SUPPLIES_INPUT_POWER_OEM_PATH, NULL, NULL, NULL}},
        {2, {RFPROP_POWER_SUPPLIES, RFPROP_POWER_SUPPLIES_INPUT_CURRENT_OEM_PATH, NULL, NULL, NULL}},
        {2, {RFPROP_POWER_SUPPLIES, RFPROP_POWER_SUPPLIES_OUTPUT_POWER_OEM_PATH, NULL, NULL, NULL}},
        {2, {RFPROP_POWER_SUPPLIES, RFPROP_POWER_SUPPLIES_OUTPUT_CURRENT_OEM_PATH, NULL, NULL, NULL}},
        {2, {RFPROP_POWER_SUPPLIES, RFPROP_POWER_SUPPLIES_OUTPUT_VOLTAGE_OEM_PATH, NULL, NULL, NULL}},
        {2, {RFPROP_POWER_SUPPLIES, RFPROP_POWER_SUPPLIES_CURRENT_CAPACITY_OEM_PATH, NULL, NULL, NULL}},
        {2, {RFPROP_POWER_SUPPLIES, RFPROP_POWER_SUPPLIES_TOTAL_RUNNING_TIME_OEM_PATH, NULL, NULL, NULL}},
        {2, {RFPROP_POWER_SUPPLIES, RFPROP_POWER_SUPPLIES_FREQUENCY_OEM_PATH, NULL, NULL, NULL}},
        {2, {RFPROP_POWER_SUPPLIES, RFPROP_POWER_SUPPLIES_VIN_CHANNEL_A_OEM_PATH, NULL, NULL, NULL}},
        {2, {RFPROP_POWER_SUPPLIES, RFPROP_POWER_SUPPLIES_VIN_CHANNEL_B_OEM_PATH, NULL, NULL, NULL}},
        {2, {RFPROP_POWER_CONTROL, RF_BATTERY_EXTEND_METRICS_OEM_PATH, NULL, NULL, NULL}},
        {0, {NULL, NULL, NULL, NULL, NULL}}
    };

    return rsc_del_none_etag_affected_property(object, value, G_N_ELEMENTS(value));
}



LOCAL gint32 get_power_reset_data_actioninfo_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar string_value[ARRAY_LENTH] = {0};
    gchar slot_str[MAX_RSC_ID_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    // 获取 @odata.id
    ret = rf_gen_chassis_component_id(NULL, i_paras->obj_handle, slot_str, MAX_RSC_ID_LEN);
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1,
        RF_POWER_OEM_ACTION_RESET_ACTION_INFO, slot_str);
    return_val_if_expr(ret < 0, HTTP_INTERNAL_SERVER_ERROR);

    *o_result_jso = json_object_new_string((const gchar *)string_value);
    return_val_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_power_reset_statistics_actioninfo_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar slot_str[MAX_RSC_ID_LEN] = {0};
    gchar string_value[ARRAY_LENTH] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    // 获取 @odata.id
    ret = rf_gen_chassis_component_id(NULL, i_paras->obj_handle, slot_str, MAX_RSC_ID_LEN);
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1,
        RF_POWER_OEM_ACTION_RESET_STATISTICS_ACTION_INFO, slot_str);
    return_val_if_expr(ret < 0, HTTP_INTERNAL_SERVER_ERROR);

    *o_result_jso = json_object_new_string((const gchar *)string_value);
    return_val_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_power_collect_history_data_actioninfo_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar string_value[ARRAY_LENTH] = {0};
    gchar slot_str[MAX_RSC_ID_LEN] = {0};
    gint32 ret;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    // 获取 @odata.id
    ret = rf_gen_chassis_component_id(NULL, i_paras->obj_handle, slot_str, MAX_RSC_ID_LEN);
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1,
        RF_POWER_OEM_ACTION_COLLECT_HISTORY_DATA_ACTION_INFO, slot_str);
    return_val_if_expr(ret < 0, HTTP_INTERNAL_SERVER_ERROR);

    *o_result_jso = json_object_new_string((const gchar *)string_value);
    return_val_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}

LOCAL gint32 get_power_set_psu_supply_source_actioninfo_odataid(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso)
{
    gchar string_value[ARRAY_LENTH] = {0};
    gchar slot_str[MAX_RSC_ID_LEN] = {0};
    gint32 ret;
    OBJ_HANDLE obj_handle = OBJ_HANDLE_COMMON;
    guint8 enabled = DISABLE;

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (i_paras->is_satisfy_privi != TRUE) {
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = dal_get_object_handle_nth(CLASS_NAME_PME_SERVICECONFIG, 0, &obj_handle);
    if (ret != RET_OK) {
        return HTTP_NOT_FOUND;
    }

    (void)dal_get_property_value_byte(obj_handle, PROTERY_PME_PSU_SOURCE_SWITCH_SUPPORT, &enabled);
    if (!enabled) {
        return HTTP_NOT_FOUND;
    }

    // 获取 @odata.id
    ret = rf_gen_chassis_component_id(NULL, i_paras->obj_handle, slot_str, MAX_RSC_ID_LEN);
    if (ret != RET_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1,
        RF_POWER_OEM_ACTION_SET_PSU_SUPPLY_SOURCE_ACTION_INFO, slot_str);
    if (ret < 0) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const gchar*)string_value);
    if (*o_result_jso == NULL) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 redfish_check_chasssi_uri_and_compnent_type(PROVIDER_PARAS_S *i_paras)
{
    gboolean ret;
    gint32 check_ret;

    
    if (dal_match_software_type(MGMT_SOFTWARE_TYPE_EM) || dal_match_software_type(MGMT_SOFTWARE_TYPE_PANGEA_MGNT)) {
        ret = redfish_check_smm_chassis_uri_valid(i_paras->uri);
        check_ret = dal_get_specific_object_byte(CLASS_COMPONENT, PROPERTY_COMPONENT_DEVICE_TYPE,
            COMPONENT_TYPE_MAINBOARD, &i_paras->obj_handle);
        if (!ret || check_ret != RET_OK) {
            debug_log(DLOG_ERROR, "get COMPONENT_TYPE_MAINBOARD(check_ret=%d) or check smm chassis(ret=%s) fail.",
                check_ret, (ret == TRUE) ? "true" : "false");
            return HTTP_NOT_FOUND;
        }
    } else if (dal_match_product_id(PRODUCT_ID_PANGEA_V6)) {
        
        ret = check_chassis_uri_pangea_enc(i_paras->uri);
        check_ret = dal_get_specific_object_byte(CLASS_COMPONENT, PROPERTY_COMPONENT_DEVICE_TYPE,
            COMPONENT_TYPE_CHASSIS_BACKPLANE, &i_paras->obj_handle);
        if (!ret || check_ret != RET_OK) {
            debug_log(DLOG_ERROR, "get COMPONENT_OBJ_HANDLE(check_ret=%d) or check chassis_uri(ret=%s) fail.",
                check_ret, (ret == TRUE) ? "true" : "false");
            return HTTP_NOT_FOUND;
        }
    } else {
        
        ret = redfish_check_chassis_uri_valid(i_paras->uri, &i_paras->obj_handle);
        if (!ret || i_paras->obj_handle == 0) {
            debug_log(DLOG_ERROR, "get COMPONENT_OBJ_HANDLE or check chassis_uri(ret=%s) fail.",
                (ret == TRUE) ? "true" : "false");
            return HTTP_NOT_FOUND;
        }

        check_ret = check_enclosure_component_type(i_paras->obj_handle, FALSE);
        if (check_ret != RET_OK) {
            debug_log(DLOG_ERROR, "check_enclosure_component_type fail, ret = %d.", check_ret);
            return HTTP_NOT_FOUND;
        }
    }
    return RET_OK;
}


gint32 power_reset_data_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret = redfish_check_chasssi_uri_and_compnent_type(i_paras);
    if (ret != RET_OK) {
        return ret;
    }

    *prop_provider = g_power_reset_data_actioninfo_provider;
    *count = sizeof(g_power_reset_data_actioninfo_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}



LOCAL PROPERTY_PROVIDER_S g_power_history_data_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_power_history_data_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_power_history_data_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_HISTORY_DATA, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_power_history_data, NULL, NULL, ETAG_FLAG_DISABLE}
};



LOCAL gint32 get_power_history_data_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;
    gint32 ret;
    gchar uri[MAX_URI_LENGTH + 1] = {0};
    gchar slot[MAX_RSC_NAME_LEN + 1] = {0};

    
    return_val_do_info_if_fail(NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error.", __FUNCTION__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = rf_gen_chassis_component_id(NULL, i_paras->obj_handle, slot, MAX_RSC_NAME_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: function  return err of redfish_slot_id.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    iRet = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, URI_FORMAT_CHASSIS_POWER_HISTORY_DATA, slot);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    *o_result_jso = json_object_new_string((const char *)uri);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_string fail", __FUNCTION__));

    return HTTP_OK;
}


LOCAL gint32 get_power_history_data_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;
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

    iRet = snprintf_s(odata_context, sizeof(odata_context), sizeof(odata_context) - 1,
        CHASSIS_POWER_HISTORY_DATA_METADATA, slot);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    *o_result_jso = json_object_new_string((const char *)odata_context);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_string fail", __FUNCTION__));

    return HTTP_OK;
}


LOCAL gint32 get_power_history_data(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    int iRet;
    gint32 ret;
    gchar file_path[MAX_FILE_PATH_LENGTH + 1] = {0};
    gchar* labels[] = {RFPROP_POWER_DATA, RFPROP_POWER_AVERAGE, RFPROP_POWER_PEAK};

    
    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_message_jso) && (NULL != o_result_jso),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error.", __FUNCTION__));

    
    iRet = snprintf_s(file_path, MAX_FILE_PATH_LENGTH + 1, MAX_FILE_PATH_LENGTH, "%s%s", OPT_PME_PRAM_FOLDER,
        POWER_HISTORY_DATA_FILE);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    
    
    
    ret = read_history_data_from_file(file_path, sizeof(labels) / sizeof(char *), labels, o_result_jso,
        READ_HISTORY_UINT16);
    
    
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s: read_history_data_from_file file, ret is %d.", __FUNCTION__, ret));
    
    return HTTP_OK;
}


gint32 power_history_data_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gint32 ret = redfish_check_chasssi_uri_and_compnent_type(i_paras);
    if (ret != RET_OK) {
        return ret;
    }

    *prop_provider = g_power_history_data_provider;
    *count = sizeof(g_power_history_data_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}


gint32 power_reset_statistics_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret = redfish_check_chasssi_uri_and_compnent_type(i_paras);
    if (ret != RET_OK) {
        return ret;
    }

    *prop_provider = g_power_reset_statistics_actioninfo_provider;
    *count = sizeof(g_power_reset_statistics_actioninfo_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}


gint32 power_collect_history_data_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras,
    PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gint32 ret = redfish_check_chasssi_uri_and_compnent_type(i_paras);
    if (ret != RET_OK) {
        return ret;
    }

    *prop_provider = g_power_collect_history_data_actioninfo_provider;
    *count = sizeof(g_power_collect_history_data_actioninfo_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}

gint32 power_set_psu_supply_source_actioninfo_provider_entry(PROVIDER_PARAS_S* i_paras,
    PROPERTY_PROVIDER_S** prop_provider, guint32* count)
{
    gboolean ret;
    guint8 software_type = 0;
    OBJ_HANDLE obj_handle = OBJ_HANDLE_COMMON;
    guint8 enabled = DISABLE;

    (void)dal_get_software_type(&software_type);
    if (software_type == MGMT_SOFTWARE_TYPE_EM) {
        ret = redfish_check_smm_chassis_uri_valid(i_paras->uri);
        if (ret == FALSE) {
            return HTTP_NOT_FOUND;
        }
    } else {
        ret = redfish_check_chassis_uri_valid(i_paras->uri, &i_paras->obj_handle);
        if (ret == FALSE || i_paras->obj_handle == 0) {
            return HTTP_NOT_FOUND;
        }

        ret = check_enclosure_component_type(i_paras->obj_handle, FALSE);
        if (ret != RET_OK) {
            return HTTP_NOT_FOUND;
        }
    }

    ret = dal_get_object_handle_nth(CLASS_NAME_PME_SERVICECONFIG, 0, &obj_handle);
    if (ret != RET_OK) {
        return HTTP_NOT_FOUND;
    }

    (void)dal_get_property_value_byte(obj_handle, PROTERY_PME_PSU_SOURCE_SWITCH_SUPPORT, &enabled);
    if (!enabled) {
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_power_set_psu_supply_source_actioninfo_provider;
    *count = sizeof(g_power_set_psu_supply_source_actioninfo_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}


LOCAL gint32 get_power_consumed_watts(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    GSList *obj_list_syspower = NULL;
    GSList *obj_node_syspower = NULL;
    guint32 uint32_value = 0;
    guint32 consumedWatts = 0;

    
    if (NULL == o_result_jso || NULL == *o_result_jso || NULL == o_message_jso ||
        VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s,%d: NULL pointer.", __FUNCTION__, __LINE__);
        return VOS_ERR;
    }

    
    ret = dfl_get_object_list(CLASS_NAME_AMMETER, &obj_list_syspower);
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s, %d: get obj_list for CLASS_NAME_AMMETER fail.\n", __FUNCTION__, __LINE__));

    for (obj_node_syspower = obj_list_syspower; obj_node_syspower; obj_node_syspower = obj_node_syspower->next) {
        
        ret += dal_get_property_value_uint32((OBJ_HANDLE)obj_node_syspower->data, PROTERY_PSTOTAL_POWER, &uint32_value);
        consumedWatts += uint32_value;
    }

    add_property_value_int(*o_result_jso, RFPROP_POWER_CONTROL_POWER_CONSUME, consumedWatts, ret);

    do_val_if_expr(NULL != obj_list_syspower, g_slist_free(obj_list_syspower));

    return HTTP_OK;
}


LOCAL gint32 get_power_supply(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    guint8 psu_num = 0;
    OBJ_HANDLE obj_handle = 0;
    guint32 capacityWatts = 0;
    guint32 presenceNum = 0;
    guint32 onepower_num = 0;

    
    gint32 ret = dfl_get_object_handle(OBJ_PRODUCT_COMPONENT, &obj_handle);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get OBJ_PRODUCT_COMPONENT handle fail", __FUNCTION__));

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_COMPOENT_PS_NUM, &psu_num);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get PROPERTY_COMPOENT_PS_NUM fail", __FUNCTION__));

    
    if (psu_num == 0) {
        (void)dfl_get_object_count(CLASS_NAME_PS, &onepower_num);
        if (onepower_num == 0) {
            json_object_object_add(*o_result_jso, RFPROP_POWER_SUPPLIES_CAPACITY, json_object_new_int(0));
            json_object_object_add(*o_result_jso, RFPROP_POWER_PRESENT_PSU_NUM, json_object_new_int(0));
            return HTTP_OK;
        } else {
            psu_num = onepower_num;
        }
    }

    for (guint8 i = 0; i < psu_num; i++) {
        ret = get_ps_handle_by_slot(i, &obj_handle);
        continue_do_info_if_expr(ret != VOS_OK,
            debug_log(DLOG_INFO, "%s: get PROTERY_PS_DEVICENAME fail", __FUNCTION__));

        
        guint16 ps_rate = 0;
        ret = dal_get_property_value_uint16(obj_handle, PROTERY_PS_RATING, &ps_rate);
        do_if_expr(ret != VOS_OK, debug_log(DLOG_ERROR, "%s: get PROTERY_PS_RATING fail", __FUNCTION__));
        capacityWatts += ps_rate;

        
        guint8 ps_present = 0;
        ret = dal_get_property_value_byte(obj_handle, PROTERY_PS_PRESENCE, &ps_present);
        continue_do_info_if_expr(ret != VOS_OK,
            debug_log(DLOG_ERROR, "%s: get PROTERY_PS_PRESENCE fail", __FUNCTION__));
        do_if_expr(ps_present == PRESENT_STATE, presenceNum++);
    }

    json_object_object_add(*o_result_jso, RFPROP_POWER_SUPPLIES_CAPACITY, json_object_new_int(capacityWatts));
    json_object_object_add(*o_result_jso, RFPROP_POWER_PRESENT_PSU_NUM, json_object_new_int(presenceNum));

    return HTTP_OK;
}


LOCAL void add_prop_power_alarm_threshold(json_object *o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    ret = dal_get_object_handle_nth(CLASS_NAME_AMMETER, 0, &obj_handle);
    return_do_info_if_expr(ret != VOS_OK, json_object_object_add(o_result_jso, RFPROP_POWER_HIGH_THRESHOLD, NULL);
        debug_log(DLOG_ERROR, "%s: dal_get_object_handle_nth, ret:%d", __FUNCTION__, ret));

    ret = get_powercontrol_high_power_threshold(o_result_jso, obj_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_DEBUG, "%s: the server did not support high power threshold", __FUNCTION__);
    }
    return;
}


void get_system_overview_power(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;

    *o_result_jso = json_object_new_object();
    return_do_info_if_fail(NULL != *o_result_jso,
        debug_log(DLOG_ERROR, "%s,%d: json_object_new_object fail. ", __FUNCTION__, __LINE__));

    ret = get_power_consumed_watts(i_paras, o_message_jso, o_result_jso);
    do_if_expr(HTTP_OK != ret, debug_log(DLOG_ERROR, "%s: get_power_consumed_watts fail", __FUNCTION__));

    ret = get_power_supply(i_paras, o_message_jso, o_result_jso);
    do_if_expr(HTTP_OK != ret, debug_log(DLOG_ERROR, "%s: get_power_supply fail", __FUNCTION__));

    add_prop_power_alarm_threshold(*o_result_jso);
    return;
}
