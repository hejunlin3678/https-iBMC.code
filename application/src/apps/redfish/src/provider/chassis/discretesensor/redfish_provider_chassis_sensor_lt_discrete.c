
#include "redfish_provider.h"

LOCAL gint32 get_chassis_discretesensor_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_discretesensor_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_discretesensor_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_chassis_discretesensor_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_discretesensor_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_discretesensor_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {URI_FORMAT_CHASSIS_SENSORLIST_MEMBER, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_discretesensor_members, NULL, NULL, ETAG_FLAG_ENABLE}
};


LOCAL gint32 get_chassis_discretesensor_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    ret = get_odata_context(i_paras, o_message_jso, o_result_jso, URI_FORMAT_CHASSIS_SENSORLIST_DISCRETE);
    do_if_expr(HTTP_OK != ret, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_chassis_discretesensor_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    ret = get_odata_id(i_paras, o_message_jso, o_result_jso, URI_FORMAT_CHASSIS_SENSORLIST_DISCRETE);
    do_if_expr(HTTP_OK != ret, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}

LOCAL gint32 get_chassis_discrete_sensor_status(OBJ_HANDLE handle, const gchar *property, json_object *obj_json,
    const gchar *rf_property)
{
    guint32 ret;
    guint16 value = 0;
    guint16 reading_raw = 0;
    guint8 reading_status = 0;
    gchar string_status[ARRAY_LENTH] = {0};
    EVENT_WORK_STATE_S *sensor_reading_status = NULL;

    if (NULL == obj_json || NULL == property) {
        debug_log(DLOG_ERROR, "%s : NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_property_value_byte(handle, PROPERTY_THS_SENSOR_READING_STATUS, &reading_status);
    do_val_if_expr(VOS_OK != ret, (reading_status = 0));
    sensor_reading_status = (EVENT_WORK_STATE_S *)&reading_status;
    (void)dal_get_property_value_uint16(handle, PROPERTY_THR_SENSOR_READING_RAW, &reading_raw);
    ret = dal_get_property_value_uint16(handle, property, &value);
    if (VOS_OK != ret || !sensor_reading_status->disable_scanning || !sensor_reading_status->disable_scanning_local ||
        !sensor_reading_status->disable_all || !sensor_reading_status->initial_update_progress ||
        !sensor_reading_status->disable_access_error || 0 != (reading_raw & 0xFF00)) {
        
        json_object_object_add(obj_json, rf_property, NULL);
    } else {
        (void)snprintf_s(string_status, sizeof(string_status), sizeof(string_status) - 1, "0x%04x", value);
        json_object_object_add(obj_json, rf_property, json_object_new_string(string_status));
    }
    
    return HTTP_OK;
}


LOCAL gint32 get_chassis_discretesensor_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    json_object *obj_json = NULL;
    GSList *obj_list_discretesensor = NULL;
    GSList *obj_node_discretesensor = NULL;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    *o_result_jso = json_object_new_array();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_array fail.\n.", __FUNCTION__));

    // 更新门限传感器
    ret = dfl_get_object_list(CLASS_DISCERETE_SENSOR, &obj_list_discretesensor);
    return_val_if_expr(obj_list_discretesensor == NULL, HTTP_OK);
    return_val_do_info_if_expr(DFL_OK != ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get discretesensor list fail.\n.", __FUNCTION__));

    for (obj_node_discretesensor = obj_list_discretesensor; obj_node_discretesensor;
        obj_node_discretesensor = obj_node_discretesensor->next) {
        obj_json = json_object_new_object();

        // 获取 Name传感器名称
        rf_add_property_jso_string((OBJ_HANDLE)obj_node_discretesensor->data, PROPERTY_DIS_SENSOR_SENSOR_NAME,
            RFPROP_SENSOR_NAME, obj_json);

        
        // 获取 sensor id
        rf_add_property_jso_byte((OBJ_HANDLE)obj_node_discretesensor->data, PROPERTY_DIS_SENSOR_NUMBER,
            RFPROP_SENSOR_ID, obj_json);
        

        // 获取 status
        (void)get_chassis_discrete_sensor_status((OBJ_HANDLE)obj_node_discretesensor->data,
            PROPERTY_THR_SENSOR_ASSERT_STATUS, obj_json, RFPROP_SENSOR_STATUS);

        json_object_array_add(*o_result_jso, obj_json);
    }

    g_slist_free(obj_list_discretesensor);
    return HTTP_OK;
}


gint32 chassis_discretesensor_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    OBJ_HANDLE chassis_handle;
    gint32 ret;

    if (FALSE == redfish_check_chassis_uri_valid(i_paras->uri, &chassis_handle)) {
        return HTTP_NOT_FOUND;
    }

    
    ret = check_enclosure_component_type(chassis_handle, FALSE);
    
    return_val_if_fail(VOS_OK == ret, HTTP_NOT_FOUND);

    *prop_provider = g_chassis_discretesensor_provider;
    *count = sizeof(g_chassis_discretesensor_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}
