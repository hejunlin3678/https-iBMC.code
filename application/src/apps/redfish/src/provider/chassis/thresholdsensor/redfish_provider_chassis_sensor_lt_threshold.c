
#include "redfish_provider.h"

LOCAL gint32 get_chassis_thresholdsensor_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_thresholdsensor_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_thresholdsensor_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_chassis_thresholdsensor_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_thresholdsensor_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_thresholdsensor_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {URI_FORMAT_CHASSIS_SENSORLIST_MEMBER, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_thresholdsensor_members, NULL, NULL, ETAG_FLAG_ENABLE}
};


LOCAL gint32 get_chassis_thresholdsensor_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    ret = get_odata_context(i_paras, o_message_jso, o_result_jso, URI_FORMAT_CHASSIS_SENSORLIST_THRESHOLD);
    do_if_expr(HTTP_OK != ret, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_chassis_thresholdsensor_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    ret = get_odata_id(i_paras, o_message_jso, o_result_jso, URI_FORMAT_CHASSIS_SENSORLIST_THRESHOLD);
    do_if_expr(HTTP_OK != ret, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


gint32 get_chassis_sensor_threshold(OBJ_HANDLE handle, const gchar *property_name, json_object *obj_json,
    const gchar *rf_property, guint16 readable, SENSOR_CAPABILITY_S *sensor_cap)
{
    gint32 ret;
    gdouble double_value = 0.0;

    if (property_name == NULL || obj_json == NULL || rf_property == NULL) {
        debug_log(DLOG_ERROR, "%s : NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (!((sensor_cap->threshold_access_support == SENSOR_CAP_READABLE_SETTABLE) ||
        (sensor_cap->threshold_access_support == SENSOR_CAP_READABLE))) {
        json_object_object_add(obj_json, rf_property, NULL);
        return HTTP_OK;
    }

    ret = dal_get_property_value_double(handle, property_name, &double_value);
    if ((ret == VOS_OK) && (readable)) {
        json_object_object_add(obj_json, rf_property, ex_json_object_new_double(double_value, "%.3f"));
    } else {
        json_object_object_add(obj_json, rf_property, NULL);
    }

    return HTTP_OK;
}


LOCAL gint32 parse_sensor_status(guint16 status, gchar *status_buffer)
{
    SENSOR_STATE_BIT_T *sensor_state = NULL;

    if (NULL == status_buffer) {
        return VOS_ERR;
    }

    sensor_state = (SENSOR_STATE_BIT_T *)&status;

    if (sensor_state->assert_nonrecoverable_lgl) {
        (void)snprintf_s(status_buffer, SENSOR_STATUS_LEN + 1, SENSOR_STATUS_LEN, "%s", "nr");
        return VOS_OK;
    } else if (sensor_state->assert_nonrecoverable_ugh) {
        (void)snprintf_s(status_buffer, SENSOR_STATUS_LEN + 1, SENSOR_STATUS_LEN, "%s", "nr");
        return VOS_OK;
    } else if (sensor_state->assert_critical_lgl) {
        (void)snprintf_s(status_buffer, SENSOR_STATUS_LEN + 1, SENSOR_STATUS_LEN, "%s", "cr");
        return VOS_OK;
    } else if (sensor_state->assert_critical_ugh) {
        (void)snprintf_s(status_buffer, SENSOR_STATUS_LEN + 1, SENSOR_STATUS_LEN, "%s", "cr");
        return VOS_OK;
    } else if (sensor_state->assert_noncritical_lgl) {
        (void)snprintf_s(status_buffer, SENSOR_STATUS_LEN + 1, SENSOR_STATUS_LEN, "%s", "nc");
        return VOS_OK;
    } else if (sensor_state->assert_noncritical_ugh) {
        (void)snprintf_s(status_buffer, SENSOR_STATUS_LEN + 1, SENSOR_STATUS_LEN, "%s", "nc");
        return VOS_OK;
    }

    (void)snprintf_s(status_buffer, SENSOR_STATUS_LEN + 1, SENSOR_STATUS_LEN, "%s", "ok");
    return VOS_OK;
}


LOCAL gint32 get_chassis_sensor_status(OBJ_HANDLE handle, json_object *obj_json,
    EVENT_WORK_STATE_S *sensor_reading_status, gint32 *sensor_status)
{
    gint32 ret = VOS_OK;
    gchar status_buf[SENSOR_STATUS_LEN + 1] = {0};
    guint16 assert_status = 0;
    json_object *obj_json_status = NULL;

    // 获取 status->State
    if (!sensor_reading_status->disable_scanning || !sensor_reading_status->disable_scanning_local ||
        !sensor_reading_status->disable_all || !sensor_reading_status->initial_update_progress ||
        !sensor_reading_status->disable_access_error) {
        *sensor_status = VOS_ERR;
        
        json_object_object_add(obj_json, RFPROP_SENSOR_STATUS, NULL);
    } else {
        *sensor_status = VOS_OK;
        ret = dal_get_property_value_uint16(handle, PROPERTY_THR_SENSOR_ASSERT_STATUS, &assert_status);
        return_val_do_info_if_fail(VOS_OK == ret, DFL_ERR,
            debug_log(DLOG_ERROR, "%s : get %s fail.\n", __FUNCTION__, PROPERTY_THR_SENSOR_ASSERT_STATUS));
        ret = parse_sensor_status(assert_status, status_buf);
        return_val_do_info_if_fail(VOS_OK == ret, DFL_ERR,
            debug_log(DLOG_ERROR, "%s : get sensor status fail.\n", __FUNCTION__));
        obj_json_status = json_object_new_string(status_buf);
        do_if_expr(NULL == obj_json_status, debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__));

        json_object_object_add(obj_json, RFPROP_SENSOR_STATUS, obj_json_status);
        
    }

    return HTTP_OK;
}


LOCAL gint32 get_chassis_sensor_reading(OBJ_HANDLE handle, json_object *obj_json, gint32 sensor_status)
{
    guint32 ret;
    gdouble double_value = 0.0;

    if (NULL == obj_json) {
        debug_log(DLOG_ERROR, "%s : NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_property_value_double(handle, PROPERTY_THR_SENSOR_READING_CONVERT, &double_value);
    if ((VOS_OK != ret) || (VOS_OK != sensor_status)) {
        json_object_object_add(obj_json, RFPROP_SENSOR_READING_VALUE, NULL);
    } else {
        json_object_object_add(obj_json, RFPROP_SENSOR_READING_VALUE, ex_json_object_new_double(double_value, "%.3f"));
    }

    return HTTP_OK;
}


LOCAL gint32 get_chassis_thresholdsensor_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint16 mask = 0;
    guint8 capabilities = 0;
    guint8 reading_status = 0;
    json_object *obj_json = NULL;
    gint32 sensor_status = 0;
    SENSOR_CAPABILITY_S *sensor_cap = NULL;
    SENSOR_READINGMASK_BIT *reading_mask = NULL;
    EVENT_WORK_STATE_S *sensor_reading_status = NULL;
    GSList *obj_list_thresholdsensor = NULL;
    GSList *obj_node_thresholdsensor = NULL;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    *o_result_jso = json_object_new_array();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_array fail\n.", __FUNCTION__, __LINE__));

    // 更新门限传感器
    ret = dfl_get_object_list(CLASS_THRESHOLD_SENSOR, &obj_list_thresholdsensor);
    return_val_do_info_if_expr(VOS_ERR == ret || NULL == obj_list_thresholdsensor, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get thresholdsensor list fail.\n.", __FUNCTION__));

    for (obj_node_thresholdsensor = obj_list_thresholdsensor; obj_node_thresholdsensor;
        obj_node_thresholdsensor = obj_node_thresholdsensor->next) {
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node_thresholdsensor->data,
            PROPERTY_THS_SENSOR_READING_STATUS, &reading_status);
        do_val_if_expr(VOS_OK != ret, (reading_status = 0));
        sensor_reading_status = (EVENT_WORK_STATE_S *)&reading_status;

        ret = dal_get_property_value_uint16((OBJ_HANDLE)obj_node_thresholdsensor->data,
            PROPERTY_THR_SENSOR_READING_MASK, &mask);
        do_val_if_expr(VOS_OK != ret, (mask = 0));
        reading_mask = (SENSOR_READINGMASK_BIT *)&mask;

        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node_thresholdsensor->data, PROPERTY_THR_SENSOR_CAPABILITIES,
            &capabilities);
        do_val_if_expr(VOS_OK != ret, (capabilities = 0));
        sensor_cap = (SENSOR_CAPABILITY_S *)&capabilities;

        obj_json = json_object_new_object();

        // 获取 Name传感器名称
        rf_add_property_jso_string((OBJ_HANDLE)obj_node_thresholdsensor->data, PROPERTY_THR_SENSOR_SENSOR_NAME,
            RFPROP_SENSOR_NAME, obj_json);

        
        // 获取 sensor id
        rf_add_property_jso_byte((OBJ_HANDLE)obj_node_thresholdsensor->data, PROPERTY_THR_SENSOR_NUMBER,
            RFPROP_SENSOR_ID, obj_json);
        

        // 获取 status
        (void)get_chassis_sensor_status((OBJ_HANDLE)obj_node_thresholdsensor->data, obj_json, sensor_reading_status,
            &sensor_status);

        // 获取 Unit
        rf_add_property_jso_string((OBJ_HANDLE)obj_node_thresholdsensor->data, PROPERTY_THR_SENSOR_UNIT_STR,
            RFPROP_SENSOR_UNIT, obj_json);

        // 获取 ReadingValue读数
        (void)get_chassis_sensor_reading((OBJ_HANDLE)obj_node_thresholdsensor->data, obj_json, sensor_status);

        // 获取 UpperThresholdNonCritical轻微上门限
        (void)get_chassis_sensor_threshold((OBJ_HANDLE)obj_node_thresholdsensor->data, PROPERTY_THR_SENSOR_UNC_CONVERT,
            obj_json, RFPROP_SENSOR_UPPER_THRESHOLD_NON_CRITICAL, reading_mask->readable_upper_noncritical, sensor_cap);

        // 获取 UpperThresholdCritical严重上门限
        (void)get_chassis_sensor_threshold((OBJ_HANDLE)obj_node_thresholdsensor->data, PROPERTY_THR_SENSOR_UC_CONVERT,
            obj_json, RFPROP_SENSOR_UPPER_THRESHOLD_CRITICAL, reading_mask->readable_upper_critical, sensor_cap);

        // 获取 UpperThresholdFatal紧急上门限
        (void)get_chassis_sensor_threshold((OBJ_HANDLE)obj_node_thresholdsensor->data, PROPERTY_THR_SENSOR_UNR_CONVERT,
            obj_json, RFPROP_SENSOR_UPPER_THRESHOLD_FATAL, reading_mask->readable_upper_nonrecoverable, sensor_cap);

        // 获取 LowThresholdNonCritical轻微下门限
        (void)get_chassis_sensor_threshold((OBJ_HANDLE)obj_node_thresholdsensor->data, PROPERTY_THR_SENSOR_LNC_CONVERT,
            obj_json, RFPROP_SENSOR_LOWER_THRESHOLD_NON_CRITICAL, reading_mask->readable_lower_noncritical, sensor_cap);

        // 获取 LowThresholdCritical严重下门限
        (void)get_chassis_sensor_threshold((OBJ_HANDLE)obj_node_thresholdsensor->data, PROPERTY_THR_SENSOR_LC_CONVERT,
            obj_json, RFPROP_SENSOR_LOWER_THRESHOLD_CRITICAL, reading_mask->readable_lower_critical, sensor_cap);

        // 获取 LowThresholdFatal紧急下门限
        (void)get_chassis_sensor_threshold((OBJ_HANDLE)obj_node_thresholdsensor->data, PROPERTY_THR_SENSOR_LNR_CONVERT,
            obj_json, RFPROP_SENSOR_LOWER_THRESHOLD_FATAL, reading_mask->readable_lower_nonrecoverable, sensor_cap);

        json_object_array_add(*o_result_jso, obj_json);
    }

    g_slist_free(obj_list_thresholdsensor);
    return HTTP_OK;
}


gint32 threshold_sensor_etag_del_property(json_object *object)
{
    SPECIAL_PROP_S value[] = {
        {2, {URI_FORMAT_CHASSIS_SENSORLIST_MEMBER, RFPROP_SENSOR_READING_VALUE, NULL, NULL, NULL}},
        {0, {NULL, NULL, NULL, NULL, NULL}}
    };

    return rsc_del_none_etag_affected_property(object, value, G_N_ELEMENTS(value));
}


gint32 chassis_thresholdsensor_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    OBJ_HANDLE chassis_handle;
    gint32 ret;

    if (FALSE == redfish_check_chassis_uri_valid(i_paras->uri, &chassis_handle)) {
        debug_log(DLOG_ERROR, "%s: uri is not valid.\n", __FUNCTION__);
        return HTTP_NOT_FOUND;
    }

    
    ret = check_enclosure_component_type(chassis_handle, FALSE);
    
    return_val_if_fail(VOS_OK == ret, HTTP_NOT_FOUND);

    *prop_provider = g_chassis_thresholdsensor_provider;
    *count = sizeof(g_chassis_thresholdsensor_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}
