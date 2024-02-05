
#include "redfish_provider.h"

LOCAL guint8 get_fan_control_mode(void);

LOCAL void smm_get_thermal_temperature_datas(PROVIDER_PARAS_S *i_paras, json_object *obj_json, gint32 arr_index,
    OBJ_HANDLE obj_node);
LOCAL gint32 smm_get_chassis_enclosure_thermal_temps(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 smm_get_chassis_enclosure_thermal_fans(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 foreach_fan_get_info(OBJ_HANDLE obj_handle, gpointer user_data);
LOCAL gint32 add_fan_status(OBJ_HANDLE obj_handle, guint8 fan_presence, json_object *fan_obj);
LOCAL gint32 add_fan_redundancy_id(OBJ_HANDLE obj_handle, json_object *fan_obj);
LOCAL gint32 add_related_item(OBJ_HANDLE obj_handle, const guint8 fan_presence, json_object *fan_obj);
LOCAL gint32 add_fan_reading(OBJ_HANDLE obj_handle, const guint8 fan_presence,
                             const guint8 fan_slot_id, json_object *fan_obj);
LOCAL gint32 add_fan_oem(OBJ_HANDLE obj_handle, guint8 fan_presence, json_object *fan_obj, gint32 reading);
LOCAL gint32 add_fan_fru(OBJ_HANDLE obj_handle, json_object *p_obj);
LOCAL void add_fan_version(OBJ_HANDLE obj_handle, json_object *p_obj);

LOCAL gint32 smm_set_chassis_enclosure_thermal_fans(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 foreach_fan_set_speed(OBJ_HANDLE obj_handle, PROVIDER_PARAS_S* i_paras, guint32 fan_index,
                                   guint8 speed, json_object **o_message_jso);
LOCAL gint32 smm_get_chassis_enclosure_thermal_redundancy(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 foreach_redundancy_get_info(OBJ_HANDLE obj_handle, gpointer user_data);
LOCAL gint32 add_redundancy_fan_status(guint8 state, guint8 health, json_object *redundancy);
LOCAL gint32 add_redundancy_fan_set(GSList *redundancy_set, json_object *redundancy);
LOCAL gint32 set_fan_control_mode(PROVIDER_PARAS_S *i_paras, json_object *val_jso, json_object *message_jso);
LOCAL gint32 set_fan_smart_mode(PROVIDER_PARAS_S *i_paras, json_object *val_jso, json_object *message_jso);
LOCAL gint32 smm_get_chassis_enclosure_thermal_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 smm_set_chassis_enclosure_thermal_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_chassis_enclosure_thermal_provider[] = {
    { RFPROP_SENSOR_TEMPERATURES, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, smm_get_chassis_enclosure_thermal_temps, NULL, NULL, ETAG_FLAG_DISABLE },
    { RFPROP_SENSOR_FANS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, smm_get_chassis_enclosure_thermal_fans, smm_set_chassis_enclosure_thermal_fans, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_THERMAL_REDUNDANCY, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, smm_get_chassis_enclosure_thermal_redundancy, NULL, NULL, ETAG_FLAG_DISABLE },
    { RFPROP_OEM, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, smm_get_chassis_enclosure_thermal_oem, smm_set_chassis_enclosure_thermal_oem, NULL, ETAG_FLAG_ENABLE }
};

#define AUTO_MODE 0
#define MANUAL_MODE 1

LOCAL guint8 get_fan_control_mode(void)
{
    gint32 ret;
    OBJ_HANDLE handle = 0;
    guint8 control_mode = 0;
    ret = dal_get_object_handle_nth(COOLINGCLASS, 0, &handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get class(%s) object(0th) handle failed, err code %d", __func__, COOLINGCLASS, ret);
        return AUTO_MODE; // 默认自动模式
    }
    (void)dal_get_property_value_byte(handle, COOLING_PROPERTY_MODE, &control_mode);
    return control_mode;
}


LOCAL void smm_get_thermal_temperature_datas(PROVIDER_PARAS_S *i_paras, json_object *obj_json, gint32 arr_index,
    OBJ_HANDLE obj_node)
{
    int iRet;
    guint8 uint8_value = 0;
    gchar string_value[ARRAY_LENTH] = { 0 };

    
    iRet = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1, "%s%s%s/%d", RF_CHASSIS_UI,
        "Enclosure", RFPROP_MANAGER_THERMALTEM, arr_index);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    json_object_object_add(obj_json, RFPROP_SENSOR_ODATA_ID, json_object_new_string(string_value));
    
    (void)snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1, "%d", arr_index);
    json_object_object_add(obj_json, RFPROP_SENSOR_MEMBER_ID, json_object_new_string(string_value));
    
    (void)dal_get_property_value_byte(obj_node, PROPERTY_THR_SENSOR_NUMBER, &uint8_value);
    json_object_object_add(obj_json, RFPROP_SENSOR_SENSOR_NUM, json_object_new_int(uint8_value));
    
    (void)dal_get_property_value_string(obj_node, PROPERTY_THR_SENSOR_SENSOR_NAME, string_value, sizeof(string_value));
    json_object_object_add(obj_json, RFPROP_SENSOR_NAME, json_object_new_string(string_value));
    
    get_value_unc_uc_unr_lnc_lc_lnr(obj_node, obj_json, RFPROP_SENSOR_READING_CELSIUS);
    
    json_object_object_add(obj_json, RFPROP_SENSOR_MIN_READING_RANGE_TEMP, NULL);
    json_object_object_add(obj_json, RFPROP_SENSOR_MAX_READING_RANGE_TEMP, NULL);
    
    get_temperature_status(obj_node, obj_json);
    return;
}

LOCAL gint32 smm_get_chassis_enclosure_thermal_temps(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint count = 0;
    guint8 sensor_type = 0;

    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    json_object *obj_json = NULL;
    json_object *result_json = NULL;
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "NULL pointer.");
        create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    NEW_JSON_ARR(result_json, return HTTP_INTERNAL_SERVER_ERROR);

    
    ret = dfl_get_object_list(CLASS_THRESHOLD_SENSOR, &obj_list);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_OK, json_object_put(result_json);
        debug_log(DLOG_ERROR, "%s, %d: get temperatur obj_list fail.\n", __FUNCTION__, __LINE__));

    
    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_THR_SENSOR_SENSOR_TYPE, &sensor_type);
        if (sensor_type != TEMPERATURE_TYPE) { // SensorType为1时代表温度传感器
            continue;
        }

        obj_json = json_object_new_object();

        
        smm_get_thermal_temperature_datas(i_paras, obj_json, count, (OBJ_HANDLE)obj_node->data);
        json_object_array_add(result_json, obj_json);
        count++;

        obj_json = NULL;
    }
    *o_result_jso = result_json;
    g_slist_free(obj_list);

    return HTTP_OK;
}


LOCAL gint32 foreach_fan_get_info(OBJ_HANDLE obj_handle, gpointer user_data)
{
    json_object **o_result_jso = (json_object **)user_data;
    guint8 fan_slot_id = 0;
    gint32 member_id;
    gchar fan_name[MAX_NAME_SIZE] = { 0 };
    guint8 fan_presence = 0;
    gchar odata_id_str[MAX_URI_LEN + 1] = { 0 };
    gint32 reading = 0;
    json_object *fan_obj = NULL;
    json_object *obj = NULL;
    gint32 ret;
    gchar member_id_str[MAX_STRBUF_LEN] = { 0 };

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_FAN_PRESENT, &fan_presence);

    
    fan_obj = json_object_new_object();
    if (fan_obj == NULL) {
        debug_log(DLOG_ERROR, "[%s] Create new json object failed", __func__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    member_id = json_object_array_length(*o_result_jso);
    (void)snprintf_s(odata_id_str, MAX_URI_LEN + 1, MAX_URI_LEN, "/redfish/v1/Chassis/Enclosure/Thermal#/Fans/%d",
        member_id);
    obj = json_object_new_string(odata_id_str);
    if (obj == NULL) {
        debug_log(DLOG_ERROR, "[%s] Create new json object for odata id failed", __func__);
        json_object_put(fan_obj);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    json_object_object_add(fan_obj, RFPROP_ODATA_ID, obj);

    
    (void)snprintf_s(member_id_str, sizeof(member_id_str), sizeof(member_id_str) - 1, "%d", member_id);
    obj = json_object_new_string(member_id_str);
    if (obj == NULL) {
        debug_log(DLOG_ERROR, "[%s] Create new json object for member id failed", __func__);
        json_object_put(fan_obj);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    json_object_object_add(fan_obj, MEMBERID_STRING, obj);

    
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_FAN_SLOT, &fan_slot_id);
    (void)snprintf_s(fan_name, sizeof(fan_name), sizeof(fan_name) - 1, "Fan%u", fan_slot_id);
    obj = json_object_new_string(fan_name);
    if (obj == NULL) {
        debug_log(DLOG_ERROR, "[%s] Create new json object for fan name failed", __func__);
        json_object_put(fan_obj);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    json_object_object_add(fan_obj, RFPROP_SENSOR_NAME, obj);

    
    ret = add_fan_status(obj_handle, fan_presence, fan_obj);
    if (ret != RET_OK) {
        json_object_put(fan_obj);
        return ret;
    }

    
    ret = add_fan_reading(obj_handle, fan_presence, fan_slot_id, fan_obj);
    if (ret != RET_OK) {
        json_object_put(fan_obj);
        return ret;
    }

    
    ret = add_related_item(obj_handle, fan_presence, fan_obj);
    if (ret != RET_OK) {
        json_object_put(fan_obj);
        return ret;
    }

    
    ret = add_fan_redundancy_id(obj_handle, fan_obj);
    if (ret != RET_OK) {
        json_object_put(fan_obj);
        return ret;
    }

    
    if (fan_presence == BLADE_PRESENCE) {
        obj = json_object_new_string("Percent");
        if (obj == NULL) {
            debug_log(DLOG_ERROR, "[%s] Create new json object failed", __func__);
            json_object_put(fan_obj);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        json_object_object_add(fan_obj, "ReadingUnits", obj);
    }

    
    ret = add_fan_oem(obj_handle, fan_presence, fan_obj, reading);
    if (ret != VOS_OK) {
        json_object_put(fan_obj);
    }

    json_object_array_add(*o_result_jso, fan_obj);
    return RET_OK;
}


LOCAL gint32 add_fan_reading(OBJ_HANDLE obj_handle, const guint8 fan_presence,
                             const guint8 fan_slot_id, json_object *fan_obj)
{
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    OBJ_HANDLE handle = 0;
    gint32 reading;
    json_object *obj = NULL;

    if (fan_presence != BLADE_PRESENCE) {
        return RET_OK;
    }

    ret = dal_get_object_handle_nth(COOLINGCLASS, 0, &handle);
    if (ret != RET_OK)  {
        debug_log(DLOG_ERROR, "[%s] Get class[%s] object0 handle failed, err code %d", __func__, COOLINGCLASS, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    input_list = g_slist_append(input_list, g_variant_new_int32(fan_slot_id));
    ret = dfl_call_class_method(handle, METHOD_COOLING_FAN_STATE, NULL, input_list, &output_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s] Call class[%s] method[%s] failed, err code %d",
            __func__, COOLINGCLASS, METHOD_COOLING_FAN_STATE, ret);
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    reading = g_variant_get_int32((GVariant*)g_slist_nth_data(output_list, 1));
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    input_list = NULL;
    output_list = NULL;

    if (reading != 0xff) { 
        obj = json_object_new_int(reading);
        if (obj == NULL) {
            debug_log(DLOG_ERROR, "[%s] Create new json object failed", __func__);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
    }

    json_object_object_add(fan_obj, "Reading", obj);
    return RET_OK;
}


LOCAL gint32 add_related_item(OBJ_HANDLE obj_handle, const guint8 fan_presence, json_object *fan_obj)
{
    int iRet = -1;
    json_object *obj = NULL;
    json_object *odata = NULL;
    gchar odata_id_str[MAX_URI_LEN + 1] = { 0 };
    const gchar *blade_name = NULL;
    guint8 redundancy_id = 0;
    OBJ_HANDLE redundancy_handle = 0;
    gint32 ret;
    GVariant *device_array = NULL;
    GVariantIter iter;
    GVariant *device = NULL;
    guint8 ipmb_addr = 0;
    OBJ_HANDLE blade_handle = 0;
    gint32 rf_ret = VOS_OK;
    guint8 blade_type = 0;
    gchar hmm_name[STRING_LEN_MAX] = { 0 };

    if (fan_presence != BLADE_PRESENCE) {
        return RET_OK;
    }
    // Get redundancy id
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_FAN_FANGROUP, &redundancy_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Get property(%s) of %s failed, err code %d", __func__, PROPERTY_FAN_FANGROUP,
            dfl_get_object_name(obj_handle), ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    ret = dal_get_specific_object_byte(CLASS_NAME_FAN_REDUNDANCY, PROPERTY_FAN_REDUNDANCY_ID, redundancy_id,
        &redundancy_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Get %s object by property(%s) failed, id is %u, err code %d", __func__,
            CLASS_NAME_FAN_REDUNDANCY, PROPERTY_FAN_REDUNDANCY_ID, redundancy_id, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    json_object *related_item = json_object_new_array();
    if (related_item == NULL) {
        debug_log(DLOG_ERROR, "[%s] json_object_new_array failed", __func__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    (void)dfl_get_property_value(redundancy_handle, PROPERTY_FAN_REDUNDANCY_DEV_SLOT, &device_array);
    g_variant_iter_init(&iter, device_array);
    
    while (NULL != (device = g_variant_iter_next_value(&iter))) {
        
        ipmb_addr = (guint8)g_variant_get_uint32(device);
        ret =
            dal_get_specific_object_byte(CLASS_NAME_IPMBETH_BLADE, IPMB_PROPERTY_SLAVE_ADDR, ipmb_addr, &blade_handle);
        g_variant_unref(device);
        if (ret != VOS_OK) {
            debug_log(DLOG_ERROR, "[%s] Get %s object failed, ipmb addr is %u", __func__, CLASS_NAME_IPMBETH_BLADE,
                ipmb_addr);
            rf_ret = HTTP_INTERNAL_SERVER_ERROR;
            break;
        }

        
        blade_name = dfl_get_object_name(blade_handle);
        (void)dal_get_property_value_byte(blade_handle, PROTERY_IPMBETH_BLADE_BLADETYPE, &blade_type);
        if (blade_type == 0x03 && blade_name != NULL) { 
            (void)snprintf_s(hmm_name, sizeof(hmm_name), sizeof(hmm_name) - 1, "HMM%c", blade_name[3]);
            blade_name = hmm_name;
        }

        
        obj = json_object_new_object();
        if (obj == NULL) {
            debug_log(DLOG_ERROR, "[%s] Create new json object failed", __func__);
            rf_ret = HTTP_INTERNAL_SERVER_ERROR;
            break;
        }

        
        iRet = snprintf_s(odata_id_str, MAX_URI_LEN + 1, MAX_URI_LEN, "/redfish/v1/Chassis/%s", blade_name);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
        odata = json_object_new_string(odata_id_str);
        if (odata == NULL) {
            debug_log(DLOG_ERROR, "[%s] Create new json object failed", __func__);
            rf_ret = HTTP_INTERNAL_SERVER_ERROR;
            json_object_put(obj);
            break;
        }

        
        json_object_object_add(obj, RFPROP_ODATA_ID, odata);
        
        json_object_array_add(related_item, obj);
    }

    g_variant_unref(device_array);
    if (rf_ret != RET_OK) { // failed
        json_object_put(related_item);
        return rf_ret;
    }

    json_object_object_add(fan_obj, "RelatedItem", related_item);
    return RET_OK;
}


LOCAL void add_fan_version(OBJ_HANDLE obj_handle, json_object *p_obj)
{
    guint8 pcb_version = 0;
    guint8 software_version = 0;
    gchar pcb_version_str[STRING_LEN_MAX] = { 0 };
    gchar software_version_str[STRING_LEN_MAX] = { 0 };

    
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_FAN_PCB_VERSION, &pcb_version);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_FAN_SOFT_VERSION, &software_version);
    
    pcb_version_str[0] = pcb_version + 'A';
    (void)snprintf_s(software_version_str, sizeof(software_version_str), sizeof(software_version_str) - 1, "%u",
        software_version);

    json_object_object_add(p_obj, "PcbVersion", json_object_new_string(pcb_version_str));
    json_object_object_add(p_obj, "SoftwareVersion", json_object_new_string(software_version_str));
}


LOCAL gint32 add_fan_fru(OBJ_HANDLE obj_handle, json_object *p_obj)
{
    guint8 fan_slot = 0;
    guint32 fan_fru_position;
    OBJ_HANDLE fru_handle = 0;
    OBJ_HANDLE elabel_handle = 0;
    gint32 ret;
    json_object *obj = NULL;
    json_object *value_obj = NULL;
    json_object *fru_obj = NULL;
    const gchar *value_str = NULL;
    gsize value_str_len = 0;
    GSList *property_name_list = NULL;
    GSList *property_value_list = NULL;

    fru_obj = json_object_new_object();
    if (fru_obj == NULL) {
        return RET_ERR;
    }
    
    (void)dal_get_property_value_byte(obj_handle, FAN_SLOT, &fan_slot);
    fan_fru_position = fan_slot - 1 + FAN_FRU_POSITION_BASE;
    ret = dal_get_specific_object_uint32(CLASS_FRU, PROPERTY_POSITION, fan_fru_position, &fru_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get class(%s) object by property %s(%u) failed, err code %d", __func__, CLASS_FRU,
            PROPERTY_POSITION, fan_fru_position, ret);
        (void)json_object_put(fru_obj);
        return RET_ERR;
    }
    
    ret = dfl_get_referenced_object(fru_handle, PROPERTY_FRU_ELABEL_RO, &elabel_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s] Get fan fru referenced elabel object failed, err code %d", __func__, ret);
        (void)json_object_put(fru_obj);
        return RET_ERR;
    }
    
    property_name_list = g_slist_append(property_name_list, PROPERTY_ELABEL_BOARD_ID);
    property_name_list = g_slist_append(property_name_list, PROPERTY_ELABEL_BOARD_MFG_TIME);
    property_name_list = g_slist_append(property_name_list, PROPERTY_ELABEL_BOARD_MFG_NAME);
    property_name_list = g_slist_append(property_name_list, PROPERTY_ELABEL_BOARD_PRO_NAME);
    property_name_list = g_slist_append(property_name_list, PROPERTY_ELABEL_BOARD_SN);
    property_name_list = g_slist_append(property_name_list, PROPERTY_ELABEL_BOARD_PN);
    property_name_list = g_slist_append(property_name_list, PROPERTY_ELABEL_PRODUCT_MFG_NAME);
    property_name_list = g_slist_append(property_name_list, PROPERTY_ELABEL_PRODUCT_NAME);
    property_name_list = g_slist_append(property_name_list, PROPERTY_ELABEL_PRODUCT_SN);
    property_name_list = g_slist_append(property_name_list, PROPERTY_ELABEL_PRODUCT_PN);
    property_name_list = g_slist_append(property_name_list, PROPERTY_ELABEL_PRODUCT_VERSION);
    ret = dfl_multiget_property_value(elabel_handle, property_name_list, &property_value_list);
    g_slist_free(property_name_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s] Get fan fru info failed, err code %d", __func__, ret);
        (void)json_object_put(fru_obj);
        return RET_ERR;
    }
    
    obj = json_object_new_object();
    
    value_str = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 0), &value_str_len);
    if (0 != value_str_len) {
        value_obj = json_object_new_string(value_str);
    } else {
        value_obj = NULL;
    }
    json_object_object_add(obj, "FruFileId", value_obj);
    
    value_str = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 1), &value_str_len);
    if (0 != value_str_len) {
        value_obj = json_object_new_string(value_str);
    } else {
        value_obj = NULL;
    }
    json_object_object_add(obj, "ManufacturingDate", value_obj);
    
    value_str = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 2), &value_str_len);
    if (0 != value_str_len) {
        value_obj = json_object_new_string(value_str);
    } else {
        value_obj = NULL;
    }
    json_object_object_add(obj, "Manufacturer", value_obj);
    
    value_str = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 3), &value_str_len);
    if (0 != value_str_len) {
        value_obj = json_object_new_string(value_str);
    } else {
        value_obj = NULL;
    }
    json_object_object_add(obj, "ProductName", value_obj);
    
    value_str = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 4), &value_str_len);
    if (0 != value_str_len) {
        value_obj = json_object_new_string(value_str);
    } else {
        value_obj = NULL;
    }
    json_object_object_add(obj, "SerialNumber", value_obj);
    
    value_str = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 5), &value_str_len);
    if (0 != value_str_len) {
        value_obj = json_object_new_string(value_str);
    } else {
        value_obj = NULL;
    }
    json_object_object_add(obj, "PartNumber", value_obj);
    
    json_object_object_add(fru_obj, "Board", obj);
    
    obj = json_object_new_object();
    
    value_str = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 6), &value_str_len);
    if (0 != value_str_len) {
        value_obj = json_object_new_string(value_str);
    } else {
        value_obj = NULL;
    }
    json_object_object_add(obj, "Manufacturer", value_obj);
    
    value_str = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 7), &value_str_len);
    if (0 != value_str_len) {
        value_obj = json_object_new_string(value_str);
    } else {
        value_obj = NULL;
    }
    json_object_object_add(obj, "ProductName", value_obj);
    
    value_str = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 8), &value_str_len);
    if (0 != value_str_len) {
        value_obj = json_object_new_string(value_str);
    } else {
        value_obj = NULL;
    }
    json_object_object_add(obj, "SerialNumber", value_obj);
    
    value_str = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 9), &value_str_len);
    if (0 != value_str_len) {
        value_obj = json_object_new_string(value_str);
    } else {
        value_obj = NULL;
    }

    json_object_object_add(obj, "PartNumber", value_obj);
    
    value_str = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 10), &value_str_len);
    if (value_str_len != 0) {
        value_obj = json_object_new_string(value_str);
    } else {
        value_obj = NULL;
    }
    json_object_object_add(obj, "Version", value_obj);
    
    json_object_object_add(fru_obj, "Product", obj);

    g_slist_free_full(property_value_list, (GDestroyNotify)g_variant_unref);
    
    json_object_object_add(p_obj, "Fru", fru_obj);
    return VOS_OK;
}


LOCAL gint32 add_fan_speedratio(json_object *p_obj, gint32 reading)
{
    json_object *obj = NULL;

    obj = json_object_new_int(reading);
    if (obj == NULL) {
        debug_log(DLOG_ERROR, "[%s] Create new json object failed", __func__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if (reading == 0xff) {
        json_object_put(obj);
        obj = NULL; 
    }
    json_object_object_add(p_obj, "SpeedRatio", obj);

    return HTTP_OK;
}

/*****************************************************************************
函 数 名  : add_fan_oem
功能描述  : 添加风扇oem信息
输入参数  : PROVIDER_PARAS_S* i_paras
PROPERTY_PROVIDER_S** prop_provider
guint32* count
输出参数  : 无
返 回 值  : gint32

1.日    期   : 2018年1月10日
作    者   : likesong l00422028
修改内容   : 新生成函数

*****************************************************************************/
LOCAL gint32 add_fan_oem(OBJ_HANDLE obj_handle, guint8 presence, json_object *fan_obj, gint32 reading)
{
    guint8 control_mode = 0;
    json_object *oem_obj = NULL;
    json_object *huawei_obj = NULL;
    json_object *speed_obj = NULL;
    guint8 fan_speed = 0;

    
    if (presence == BLADE_PRESENCE) {
        huawei_obj = json_object_new_object();
        oem_obj = json_object_new_object();
        json_object_object_add(oem_obj, "Huawei", huawei_obj);
        
        control_mode = get_fan_control_mode();
        if (control_mode == MANUAL_MODE) {
            (void)dal_get_property_value_byte(obj_handle, PROPERTY_FAN_SPEED, &fan_speed);
            speed_obj = json_object_new_int(fan_speed);
            json_object_object_add(huawei_obj, "CustomFanSpeed", speed_obj);
        }
        
        add_fan_fru(obj_handle, huawei_obj);
        
        add_fan_version(obj_handle, huawei_obj);
        
        (void)add_fan_speedratio(huawei_obj, reading);
        
        json_object_object_add(fan_obj, "Oem", oem_obj);
        
        rf_add_property_jso_byte(obj_handle, FAN_SLOT, RFPROP_FAN_SLOT_NUM, huawei_obj);
    }

    huawei_obj = json_object_new_object();
    oem_obj = json_object_new_object();
    json_object_object_add(oem_obj, "Huawei", huawei_obj);
    
    control_mode = get_fan_control_mode();
    if (control_mode == MANUAL_MODE) {
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_FAN_SPEED, &fan_speed);
        speed_obj = json_object_new_int(fan_speed);
        json_object_object_add(huawei_obj, "CustomFanSpeed", speed_obj);
    }

    
    if (add_fan_fru(obj_handle, huawei_obj) != RET_OK) {
        debug_log(DLOG_DEBUG, "[%s] Get fan fru info failed.", __func__);
    }

    
    add_fan_version(obj_handle, huawei_obj);
    
    json_object_object_add(fan_obj, "Oem", oem_obj);

    return RET_OK;
}


LOCAL gint32 add_fan_status(OBJ_HANDLE obj_handle, guint8 fan_state, json_object *fan_obj)
{
    json_object *fan_status = NULL;
    gchar *state_str[] = { ABSENT_STRING, ENABLED_STRING };
    guint8 fan_health = 0;

    
    if (fan_state >= sizeof(state_str) / sizeof(gchar *)) {
        debug_log(DLOG_ERROR, "[%s] Get fan state failed, fan state is %u", __func__, fan_state);
        fan_state = 0;
    }
    
    if (fan_state == 0) { // 不在位时，健康值为null
        fan_health = 0xff;
    } else {
        (void)dal_get_property_value_byte(obj_handle, FAN_PROPERTY_HEALTH, &fan_health);
    }
    
    get_resource_status_property(&fan_health, NULL, state_str[fan_state], &fan_status, TRUE);
    
    json_object_object_add(fan_obj, "Status", fan_status);
    return VOS_OK;
}


LOCAL gint32 add_fan_redundancy_id(OBJ_HANDLE obj_handle, json_object *fan_obj)
{
    json_object *redundancy_array = NULL;
    json_object *redundancy = NULL;
    json_object *obj = NULL;
    gchar odata_id_str[MAX_URI_LEN + 1] = { 0 };
    guint8 redundancy_id = 0;
    gint32 ret;

    NEW_JSON_ARR(redundancy_array, return HTTP_INTERNAL_SERVER_ERROR);
    redundancy = json_object_new_object();
    if (redundancy == NULL) {
        json_object_put(redundancy_array);
        debug_log(DLOG_ERROR, "[%s] New json object failed", __func__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_FAN_FANGROUP, &redundancy_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Get property(%s) of %s failed, err code %d", __func__, PROPERTY_FAN_FANGROUP,
            dfl_get_object_name(obj_handle), ret);
        json_object_put(redundancy_array);
        json_object_put(redundancy);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    (void)snprintf_s(odata_id_str, MAX_URI_LEN + 1, MAX_URI_LEN, "/redfish/v1/Chassis/Enclosure/Thermal#/Redundancy/%u",
        redundancy_id - 1);
    obj = json_object_new_string(odata_id_str);
    if (obj == NULL) {
        debug_log(DLOG_ERROR, "[%s] Create new json object failed", __func__);
        json_object_put(redundancy_array);
        json_object_put(redundancy);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    json_object_object_add(redundancy, RFPROP_ODATA_ID, obj);
    json_object_array_add(redundancy_array, redundancy);
    json_object_object_add(fan_obj, "Redundancy", redundancy_array);
    return RET_OK;
}


LOCAL gint32 smm_get_chassis_enclosure_thermal_fans(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint8 software_type = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "NULL pointer.");
        create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (!i_paras->is_satisfy_privi) {
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    (void)dal_get_software_type(&software_type);
    if (software_type == MGMT_SOFTWARE_TYPE_PANGEA_MGNT) {
        
        ret = dal_get_specific_object_byte(CLASS_COMPONENT, PROPERTY_COMPONENT_DEVICE_TYPE,
            COMPONENT_TYPE_MAINBOARD, &i_paras->obj_handle);
        if (ret != RET_OK || i_paras->obj_handle == 0) {
            return HTTP_NOT_FOUND;
        }

        ret = get_thermal_fans(i_paras, o_message_jso, o_result_jso);
    } else {
        *o_result_jso = json_object_new_array();
        if (*o_result_jso == NULL) {
            debug_log(DLOG_ERROR, "new object failed.");
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        ret = dfl_foreach_object(CLASS_NAME_SMM_FAN, foreach_fan_get_info, o_result_jso, NULL);
    }

    if (ret != RET_OK) {
        return ret;
    }
    return HTTP_OK;
}

LOCAL gint32 foreach_fan_set_speed(OBJ_HANDLE obj_handle, PROVIDER_PARAS_S *i_paras, guint32 fan_index, guint8 speed,
    json_object **o_message_jso)
{
    gint32 ret;
    GSList *input_list = NULL;
    OBJ_HANDLE cooling_handle = 0;
    guint8 fan_presence = 0;
    json_object *msg_jso = NULL;
    gchar property_name[MAX_URI_LEN] = { 0 };
    gchar index_str[STRING_LEN_MAX] = { 0 };

    if (speed == 0) { // 不设置此风扇
        return HTTP_BAD_REQUEST;
    }
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_FAN_PRESENT, &fan_presence);
    if (fan_presence != BLADE_PRESENCE) {
        (void)snprintf_s(property_name, sizeof(property_name), sizeof(property_name) - 1, "Fans/%u/CustomFanSpeed",
            fan_index);
        (void)snprintf_s(index_str, sizeof(index_str), sizeof(index_str) - 1, "%u", fan_index + 1);
        (void)create_message_info(MSGID_FAN_NOT_PRESENT, property_name, &msg_jso, index_str);
        json_object_array_add(*o_message_jso, msg_jso);
        return HTTP_BAD_REQUEST;
    }
    // 设置风速
    ret = dal_get_object_handle_nth(COOLINGCLASS, 0, &cooling_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get coolingapp object failed, err code %d", __func__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    input_list = g_slist_append(input_list, g_variant_new_byte(speed));
    input_list = g_slist_append(input_list, g_variant_new_byte(fan_index + 1));

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, cooling_handle,
        COOLINGCLASS, METHOD_COOLING_SINGLE_LEVEL, AUTH_DISABLE, AUTH_PRIV_NONE, input_list, NULL);
    

    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != VOS_OK) { // 设置失败不继续设置其他风扇
        debug_log(DLOG_ERROR, "[%s] Set fan[%u] speed failed, err code %d", __func__, fan_index + 1, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return HTTP_OK;
}


LOCAL gint32 smm_set_chassis_enclosure_thermal_fans(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    json_object *iter = NULL;
    json_object *obj = NULL;
    guint32 fan_count = 0;
    guint8 *fan_speed_array = NULL;
    json_object *msg_obj = NULL;
    gchar property_name[MAX_URI_LEN] = { 0 };
    gchar array_length_range[STRING_LEN_MAX] = { 0 };
    gboolean b_set_fan_speed = FALSE;
    gint32 rf_ret = HTTP_BAD_REQUEST;
    GSList *fan_handle_list = NULL;
    OBJ_HANDLE fan_handle = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "NULL pointer.");
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_SENSOR_FANS, o_message_jso,
        RFPROP_SENSOR_FANS));

    
    if (dal_match_software_type(MGMT_SOFTWARE_TYPE_PANGEA_MGNT) == TRUE) {
        debug_log(DLOG_ERROR, "[%s] Set manual fan speed is denied.", __func__);
        (void)create_message_info(MSGID_ACCESS_DENIED, NULL, o_message_jso, (const gchar*)i_paras->uri);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    json_object *fan_array = i_paras->val_jso;
    gint32 ret = dfl_get_object_count(CLASS_NAME_SMM_FAN, &fan_count);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get smm fan class object count failed, err code %d", __func__, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    gint32 fan_array_len = json_object_array_length(fan_array);
    if (fan_array_len == 0 || (guint32)fan_array_len > fan_count) {
        debug_log(DLOG_ERROR, "[%s] Input fan array length is %u, expect length > 0 and length <= %u", __func__,
            fan_array_len, fan_count);
        (void)snprintf_s(array_length_range, sizeof(array_length_range), sizeof(array_length_range) - 1, "1 ~ %u",
            fan_count);
        (void)create_message_info(MSGID_PROP_WRONG_ARRAY_LENGTH, RFPROP_SENSOR_FANS, o_message_jso, array_length_range);
        return HTTP_BAD_REQUEST;
    }
    
    
    fan_speed_array = (guint8 *)g_malloc0(sizeof(guint8) * (fan_count + 1));
    if (fan_speed_array == NULL) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    *o_message_jso = json_object_new_array();
    guint32 fan_number = 0;
    json_object_array_foreach(fan_array, iter)
    {
        
        (void)json_object_object_get_ex(iter, "Oem", &obj);
        (void)json_object_object_get_ex(obj, "Huawei", &obj);
        ret = json_object_object_get_ex(obj, "CustomFanSpeed", &obj);
        if (ret) {
            if (json_object_get_type(obj) != json_type_int) {
                (void)snprintf_s(property_name, sizeof(property_name), sizeof(property_name) - 1,
                    "Fans/%u/Oem/Huawei/CustomFanSpeed", fan_number);
                (void)create_message_info(MSGID_PROP_TYPE_ERR, property_name, &msg_obj, dal_json_object_get_str(obj),
                    property_name);
                json_object_array_add(*o_message_jso, msg_obj);
            } else {
                fan_speed_array[fan_number] = json_object_get_int(obj);
                b_set_fan_speed = TRUE;
            }
        }
        fan_number++;
    }
    
    if (b_set_fan_speed) {
        
        if (get_fan_control_mode() == AUTO_MODE) {
            debug_log(DLOG_DEBUG, "[%s] Trying to set fan speed while control mode is auto", __func__);
            (void)create_message_info(MSGID_SET_FAN_SPEED_IN_AUTO_MODE, NULL, &msg_obj);
            json_object_array_add(*o_message_jso, msg_obj);
            g_free(fan_speed_array);
            return HTTP_FORBIDDEN;
        }
        
        ret = dfl_get_object_list(CLASS_NAME_SMM_FAN, &fan_handle_list);
        if (VOS_OK != ret) {
            debug_log(DLOG_ERROR, "[%s] Get class[%s] object list failed, err code %d", __func__, CLASS_NAME_SMM_FAN,
                ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &msg_obj);
            json_object_array_add(*o_message_jso, msg_obj);
            g_free(fan_speed_array);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        fan_number = 0;
        for (GSList *fan_handle_node = fan_handle_list; fan_handle_node; fan_handle_node = fan_handle_node->next) {
            fan_handle = (OBJ_HANDLE)(fan_handle_node->data);
            
            ret = foreach_fan_set_speed(fan_handle, i_paras, fan_number, fan_speed_array[fan_number], o_message_jso);
            
            if (HTTP_INTERNAL_SERVER_ERROR == ret) {
                (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &msg_obj);
                json_object_array_add(*o_message_jso, msg_obj);
                g_free(fan_speed_array);
                g_slist_free(fan_handle_list);
                return ret;
            } else if (HTTP_OK == ret) { // 有风扇设置成功
                rf_ret = HTTP_OK;
            }
            fan_number++;
        }
    }
    g_free(fan_speed_array);
    g_slist_free(fan_handle_list);
    return rf_ret;
}

LOCAL gint32 foreach_redundancy_get_info(OBJ_HANDLE obj_handle, gpointer user_data)
{
    json_object **o_result_jso = (json_object **)user_data;
    json_object *redundancy = NULL;
    json_object *obj = NULL;
    gint32 member_id;
    gchar *mode = NULL;
    guint8 max_fan_num = 0;
    guint8 min_fan_num = 0;
    guint8 state = 0;
    guint8 health = 0;
    GSList *redundancy_set = NULL;
    gchar odata_id_str[MAX_URI_LEN + 1] = { 0 };
    gchar member_id_str[STRING_LEN_MAX] = { 0 };
    GVariant *fan_slot_array = NULL;
    GVariantIter fan_slot_iter;
    GVariant *fan_slot = NULL;
    gint32 ret;

    
    member_id = json_object_array_length(*o_result_jso);
    
    mode = "N+1";
    
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_FAN_REDUNDANCY_MAX_FAN, &max_fan_num);
    
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_FAN_REDUNDANCY_MIN_FAN, &min_fan_num);
    
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_FAN_REDUNDANCY_HEALTH, &health);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_FAN_REDUNDANCY_STATUS, &state);
    
    (void)dfl_get_property_value(obj_handle, PROPERTY_FAN_REDUNDANCY_FAN_SLOT, &fan_slot_array);
    g_variant_iter_init(&fan_slot_iter, fan_slot_array);
    while (NULL != (fan_slot = g_variant_iter_next_value(&fan_slot_iter))) {
        redundancy_set = g_slist_append(redundancy_set, fan_slot);
    }
    redundancy = json_object_new_object();
    
    (void)snprintf_s(odata_id_str, MAX_URI_LEN + 1, MAX_URI_LEN, "/redfish/v1/Chassis/Enclosure/Thermal#/Redundancy/%d",
        member_id);
    obj = json_object_new_string(odata_id_str);
    if (obj == NULL) {
        debug_log(DLOG_ERROR, "[%s] Create json object faied", __FUNCTION__);
        json_object_put(redundancy);
        g_variant_unref(fan_slot_array);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    json_object_object_add(redundancy, RFPROP_ODATA_ID, obj);
    
    (void)snprintf_s(member_id_str, sizeof(member_id_str), sizeof(member_id_str) - 1, "%d", member_id);
    obj = json_object_new_string(member_id_str);
    if (obj == NULL) {
        debug_log(DLOG_ERROR, "[%s] Create json object faied", __FUNCTION__);
        json_object_put(redundancy);
        g_variant_unref(fan_slot_array);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    json_object_object_add(redundancy, "MemberId", obj);
    
    obj = json_object_new_string("SystemFans");
    if (obj == NULL) {
        debug_log(DLOG_ERROR, "[%s] Create json object faied", __FUNCTION__);
        json_object_put(redundancy);
        g_variant_unref(fan_slot_array);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    json_object_object_add(redundancy, "Name", obj);
    
    obj = json_object_new_string(mode);
    if (obj == NULL) {
        debug_log(DLOG_ERROR, "[%s] Create json object faied", __FUNCTION__);
        json_object_put(redundancy);
        g_variant_unref(fan_slot_array);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    json_object_object_add(redundancy, "Mode", obj);
    
    obj = json_object_new_int(max_fan_num);
    if (obj == NULL) {
        debug_log(DLOG_ERROR, "[%s] Create json object faied", __FUNCTION__);
        json_object_put(redundancy);
        g_variant_unref(fan_slot_array);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    json_object_object_add(redundancy, "MaxNumSupported", obj);
    
    obj = json_object_new_int(min_fan_num);
    if (obj == NULL) {
        debug_log(DLOG_ERROR, "[%s] Create json object faied", __FUNCTION__);
        json_object_put(redundancy);
        g_variant_unref(fan_slot_array);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    json_object_object_add(redundancy, "MinNumNeeded", obj);
    
    ret = add_redundancy_fan_status(state, health, redundancy);
    if (ret != VOS_OK) {
        json_object_put(redundancy);
        g_variant_unref(fan_slot_array);
        return ret;
    }
    
    ret = add_redundancy_fan_set(redundancy_set, redundancy);
    if (ret != VOS_OK) {
        json_object_put(redundancy);
        g_variant_unref(fan_slot_array);
        return ret;
    }
    g_slist_free_full(redundancy_set, (GDestroyNotify)g_variant_unref);
    g_variant_unref(fan_slot_array);
    
    json_object_array_add(*o_result_jso, redundancy);
    return VOS_OK;
}

LOCAL gint32 add_redundancy_fan_status(guint8 state, guint8 health, json_object *redundancy)
{
    json_object *status_obj = NULL;
    json_object *obj = NULL;
    gchar *state_str[] = { ENABLED_STRING, DISABLED_STRING };
    gchar *health_str[] = { OK_STRING, WARNING_STRING, CRITICAL_STRING };

    status_obj = json_object_new_object();
    if (status_obj == NULL) {
        debug_log(DLOG_ERROR, "[%s] Create json object failed", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    if (state >= sizeof(state_str) / sizeof(gchar *)) {
        state = 0;
    }
    obj = json_object_new_string(state_str[state]);
    if (obj == NULL) {
        debug_log(DLOG_ERROR, "[%s] Create json object failed", __FUNCTION__);
        json_object_put(status_obj);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    json_object_object_add(status_obj, "State", obj);
    
    if (health >= sizeof(health_str) / sizeof(gchar *)) {
        health = 0;
    }
    if (state == 1) { // 冗余失效时，健康状态固定设置为OK
        health = 0;
    }
    obj = json_object_new_string(health_str[health]);
    if (obj == NULL) {
        debug_log(DLOG_ERROR, "[%s] Create json object failed", __FUNCTION__);
        json_object_put(status_obj);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    json_object_object_add(status_obj, "Health", obj);
    
    json_object_object_add(redundancy, "Status", status_obj);
    return VOS_OK;
}

LOCAL gint32 add_redundancy_fan_set(GSList *redundancy_set, json_object *redundancy)
{
    json_object *redundancy_set_obj = NULL;
    json_object *obj = NULL;
    json_object *odata_obj = NULL;
    GSList *cur_node = NULL;
    guint32 slot_id = 0;
    gchar odata_id_str[MAX_URI_LEN + 1] = { 0 };

    redundancy_set_obj = json_object_new_array();
    if (redundancy_set_obj == NULL) {
        debug_log(DLOG_ERROR, "[%s] Create json object failed", __func__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    cur_node = redundancy_set;
    while (cur_node != NULL) {
        slot_id = g_variant_get_uint32((GVariant *)(cur_node->data));
        obj = json_object_new_object();
        if (obj == NULL) {
            debug_log(DLOG_ERROR, "[%s] Create json object failed", __FUNCTION__);
            json_object_put(redundancy_set_obj);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        (void)snprintf_s(odata_id_str, MAX_URI_LEN + 1, MAX_URI_LEN, "/redfish/v1/Chassis/Enclosure/Thermal#/Fans/%u",
            slot_id - 1);
        odata_obj = json_object_new_string(odata_id_str);
        if (odata_obj == NULL) {
            debug_log(DLOG_ERROR, "[%s] Create json object failed", __FUNCTION__);
            json_object_put(redundancy_set_obj);
            json_object_put(obj);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        json_object_object_add(obj, RFPROP_ODATA_ID, odata_obj);
        json_object_array_add(redundancy_set_obj, obj);
        cur_node = cur_node->next;
    }
    json_object_object_add(redundancy, "RedundancySet", redundancy_set_obj);
    return VOS_OK;
}


LOCAL gint32 smm_get_chassis_enclosure_thermal_redundancy(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint8 software_type = 0;

    (void)dal_get_software_type(&software_type);
    if (software_type == MGMT_SOFTWARE_TYPE_PANGEA_MGNT) {
        
        return RET_OK;
    }

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "NULL pointer.");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    *o_result_jso = json_object_new_array();
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "new object failed."));
    ret = dfl_foreach_object(CLASS_NAME_FAN_REDUNDANCY, foreach_redundancy_get_info, o_result_jso, NULL);
    if (ret != VOS_OK) {
        json_object_put(*o_result_jso);
        return ret;
    }
    return HTTP_OK;
}


LOCAL void smm_smart_mode_num_to_str(guchar fan_smart_mode, json_object *huawei_json)
{
    switch (fan_smart_mode) {
        case LOW_FAN_SPEED_MODE:
            json_object_object_add(huawei_json, RFPROP_SMART_COOLING_MODE,
                json_object_new_string(RF_SMART_COOLING_LOW_POWER));
            break;

        case HIGH_FAN_SPEED_MODE:
            json_object_object_add(huawei_json, RFPROP_SMART_COOLING_MODE,
                json_object_new_string(RF_SMART_COOLING_HIGH_POWER));
            break;

        case MIDDLE_FAN_SPEED_MODE:
            json_object_object_add(huawei_json, RFPROP_SMART_COOLING_MODE,
                json_object_new_string(RF_SMART_COOLING_MED_POWER));
            break;

        case COOLING_ENERGY_SAVING_MODE:
            json_object_object_add(huawei_json, RFPROP_SMART_COOLING_MODE,
                json_object_new_string(RF_SMART_COOLING_ENERGY_SAVE));
            break;

        case COOLING_LOW_NOISE_MODE:
            json_object_object_add(huawei_json, RFPROP_SMART_COOLING_MODE,
                json_object_new_string(RF_SMART_COOLING_LOW_NOISE));
            break;

        case COOLING_HIGH_PERFORMANCE_MODE:
            json_object_object_add(huawei_json, RFPROP_SMART_COOLING_MODE,
                json_object_new_string(RF_SMART_COOLING_HIGH_PER));
            break;

        case COOLING_USER_DEFINED_MODE:
            json_object_object_add(huawei_json, RFPROP_SMART_COOLING_MODE,
                json_object_new_string(RF_SMART_COOLING_CUSTOM));
            break;

        default:
            json_object_object_add(huawei_json, RFPROP_SMART_COOLING_MODE, NULL);
            debug_log(DLOG_ERROR, "%s:invalid smart cooling  level is %d", __FUNCTION__, fan_smart_mode);
    }
}


LOCAL void _get_smm_fan_healthrollup(guint8 *fan_health, gint32 *obj_count)
{
    gint32 ret;
    guint8 health_tmp = 0;
    gint32 count = 0;
    guint8 software_type = 0;

    GSList *obj_list = NULL;
    GSList *obj_node = NULL;

    ret = dal_get_software_type(&software_type);
    if (ret != RET_OK) {
        return;
    }

    if (software_type == MGMT_SOFTWARE_TYPE_PANGEA_MGNT) {
        ret = dfl_get_object_list(FANCLASS, &obj_list);
    } else {
        ret = dfl_get_object_list(CLASS_NAME_SMM_FAN, &obj_list);
    }

    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s get CLASS_COMPONENT obj_list fail.", __FUNCTION__);
        return;
    }

    
    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, FAN_PROPERTY_HEALTH, &health_tmp);

        if (health_tmp > *fan_health) {
            *fan_health = health_tmp;
        }
        count++;
    }

    g_slist_free(obj_list);
    if (obj_count != NULL) {
        *obj_count = count;
    }
}


LOCAL void _get_smm_fan_health_group_value(json_object **huawei_json)
{
    guint8 fan_health = 0;
    gint32 obj_count = 0;
    json_object *tem_obj = NULL;
    json_object *status_obj = NULL;

    _get_smm_fan_healthrollup(&fan_health, &obj_count);

    tem_obj = json_object_new_object();
    return_do_info_if_fail(NULL != tem_obj,
        (void)json_object_object_add(*huawei_json, RFPROP_THERMAL_FAN_SUMMARY, NULL);
        debug_log(DLOG_ERROR, "%s: new tem_obj failed.", __FUNCTION__));

    (void)json_object_object_add(tem_obj, RFPROP_COMMON_COUNT, json_object_new_int(obj_count));
    (void)get_resource_status_property(NULL, &fan_health, NULL, &status_obj, TRUE);
    (void)json_object_object_add(tem_obj, RFPROP_SYSTEM_SUMMARY_STATUS, status_obj);
    (void)json_object_object_add(*huawei_json, RFPROP_THERMAL_FAN_SUMMARY, tem_obj);

    return;
}


LOCAL gint32 smm_get_chassis_enclosure_thermal_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    json_object *obj = NULL;
    json_object *huawei_obj = NULL;
    OBJ_HANDLE cooling_handle = 0;
    guint8 control_mode = 0;
    guint8 smart_mode = 0;
    gint32 ret;
    guint8 custom_id = RF_RSC_CUSTOM_ID_DEDAULT;
    
    if ((o_result_jso == NULL) || (o_message_jso == NULL) || (provider_paras_check(i_paras) != VOS_OK)) {
        debug_log(DLOG_ERROR, "NULL pointer.");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    
    huawei_obj = json_object_new_object();
    *o_result_jso = json_object_new_object();
    json_object_object_add(*o_result_jso, RFPROP_OEM_HUAWEI, huawei_obj);
    
    ret = dal_get_object_handle_nth(COOLINGCLASS, 0, &cooling_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get class[%s] object0 failed, err code %d", __func__, COOLINGCLASS, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    (void)dal_get_property_value_byte(cooling_handle, COOLING_PROPERTY_MODE, &control_mode);
    if (control_mode == AUTO_MODE) {
        obj = json_object_new_string(RFPROP_COOLING_MODE_AUTOMATIC);
        (void)dal_get_property_value_byte(cooling_handle, COOLING_THERMALLEVEL, &smart_mode);
        smm_smart_mode_num_to_str(smart_mode, huawei_obj);
    } else {
        obj = json_object_new_string(RFPROP_COOLING_MODE_MANUAL);
        json_object_object_add(huawei_obj, RFPROP_SMART_COOLING_MODE, NULL);
    }
    json_object_object_add(huawei_obj, "FanSpeedAdjustmentMode", obj);

    ret = dal_get_evt_detail_customized_id(&custom_id);
    if ((ret == RET_OK) && (custom_id == RF_RSC_CUSTOM_ID_CMCC)) {
        return HTTP_OK;
    }

    
    (void)_get_smm_fan_health_group_value(&huawei_obj);

    return HTTP_OK;
}


LOCAL gint32 set_fan_control_mode(PROVIDER_PARAS_S *i_paras, json_object *val_jso, json_object *message_jso)
{
    const gchar *control_mode = NULL;
    GSList *input_list = NULL;
    json_object *msg_jso = NULL;
    json_object *obj = NULL;
    gint32 ret;
    OBJ_HANDLE cooling_handle = 0;
    ret = json_object_object_get_ex(val_jso, "FanSpeedAdjustmentMode", &obj);
    if (!ret) {
        return HTTP_BAD_REQUEST;
    }
    control_mode = dal_json_object_get_str(obj);
    
    input_list = NULL;
    ret = dal_get_object_handle_nth(COOLINGCLASS, 0, &cooling_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get class(%s) object(0th) failed, err code %d", __func__, COOLINGCLASS, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &msg_jso);
        (void)json_object_array_add(message_jso, msg_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if (strcmp(control_mode, RFPROP_COOLING_MODE_AUTOMATIC) == 0) {
        input_list = g_slist_append(input_list, g_variant_new_byte(0)); // 自动
    } else if (strcmp(control_mode, RFPROP_COOLING_MODE_MANUAL) == 0) {
        input_list = g_slist_append(input_list, g_variant_new_byte(1)); // 手动
    } else {
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, "Oem/Huawei/FanSpeedAdjustmentMode", &msg_jso, control_mode,
            "Oem/Huawei/FanSpeedAdjustmentMode");
        (void)json_object_array_add(message_jso, msg_jso);
        return HTTP_BAD_REQUEST;
    }

    
    input_list = g_slist_append(input_list, g_variant_new_uint32(0)); // 暂无作用
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, cooling_handle,
        COOLINGCLASS, METHOD_COOLING_MODE, AUTH_DISABLE, AUTH_PRIV_NONE, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    

    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Call class(%s) method(%s) failed, err code %d", __func__, COOLINGCLASS,
            METHOD_COOLING_MODE, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &msg_jso);
        (void)json_object_array_add(message_jso, msg_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return HTTP_OK;
}


LOCAL gint32 set_fan_smart_mode(PROVIDER_PARAS_S *i_paras, json_object *val_jso, json_object *message_jso)
{
    const gchar *smart_cooling_mode = NULL;
    json_object *msg_jso = NULL;
    OBJ_HANDLE cooling_handle = 0;
    json_object *obj = NULL;
    gint32 ret;
    GSList *input_list = NULL;
    const gchar *smart_cool_err_mes = "Oem/Huawei/FanSmartCoolingMode";
    ret = json_object_object_get_ex(val_jso, RFPROP_SMART_COOLING_MODE, &obj);
    if (!ret) {
        return HTTP_BAD_REQUEST;
    }
    smart_cooling_mode = dal_json_object_get_str(obj);
    if (0 == g_strcmp0(smart_cooling_mode, RF_SMART_COOLING_ENERGY_SAVE)) {
        input_list = g_slist_append(input_list, g_variant_new_byte(COOLING_ENERGY_SAVING_MODE));
    } else if (0 == g_strcmp0(smart_cooling_mode, RF_SMART_COOLING_LOW_NOISE)) {
        input_list = g_slist_append(input_list, g_variant_new_byte(COOLING_LOW_NOISE_MODE));
    } else if (0 == g_strcmp0(smart_cooling_mode, RF_SMART_COOLING_HIGH_PER)) {
        input_list = g_slist_append(input_list, g_variant_new_byte(COOLING_HIGH_PERFORMANCE_MODE));
    } else {
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, smart_cool_err_mes, &msg_jso, smart_cooling_mode,
            smart_cool_err_mes);
        (void)json_object_array_add(message_jso, msg_jso);
        return HTTP_BAD_REQUEST;
    }
    ret = dal_get_object_handle_nth(COOLINGCLASS, 0, &cooling_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get class(%s) object(0th) failed, err code %d", __func__, COOLINGCLASS, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &msg_jso);
        (void)json_object_array_add(message_jso, msg_jso);
        uip_free_gvariant_list(input_list);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, cooling_handle,
        COOLINGCLASS, METHOD_SMART_COOLING_MODE, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    switch (ret) {
        case VOS_OK:
            return HTTP_OK;

            
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, smart_cool_err_mes, &msg_jso, smart_cool_err_mes);
            (void)json_object_array_add(message_jso, msg_jso);
            return HTTP_FORBIDDEN;

            
        case ERROR_UNENABLE_SMARTMODE:
            (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, smart_cool_err_mes, &msg_jso, smart_cool_err_mes);
            (void)json_object_array_add(message_jso, msg_jso);
            return HTTP_NOT_IMPLEMENTED;
            
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &msg_jso);
            (void)json_object_array_add(message_jso, msg_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}

LOCAL gboolean check_fan_smart_cooling_mode(json_object *huawei, json_object **o_message_jso)
{
    json_object *obj = NULL;
    gboolean ret;
    const gchar *smart_cooling_mode = NULL;
    json_object *msg_jso = NULL;
    const gchar *smart_cool_err_mes = "Oem/Huawei/FanSmartCoolingMode";
    ret = json_object_object_get_ex(huawei, RFPROP_SMART_COOLING_MODE, &obj);
    if (!ret) {
        return FALSE;
    }
    smart_cooling_mode = dal_json_object_get_str(obj);
    
    if (0 != g_strcmp0(smart_cooling_mode, RF_SMART_COOLING_ENERGY_SAVE) &&
        0 != g_strcmp0(smart_cooling_mode, RF_SMART_COOLING_LOW_NOISE) &&
        0 != g_strcmp0(smart_cooling_mode, RF_SMART_COOLING_HIGH_PER)) {
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, smart_cool_err_mes, &msg_jso, smart_cooling_mode,
            smart_cool_err_mes);
        (void)json_object_array_add(*o_message_jso, msg_jso);
        
        json_object_object_del(huawei, RFPROP_SMART_COOLING_MODE);
        return FALSE;
    }
    return TRUE;
}


LOCAL gint32 smm_set_chassis_enclosure_thermal_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    json_object *huawei = NULL;
    gint32 rf_ret = HTTP_BAD_REQUEST;
    json_object *obj = NULL;
    gboolean set_control_mode;
    gboolean set_smart_mode;
    guint8 software_type = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "NULL pointer.");
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_OEM, o_message_jso, RFPROP_OEM));
    (void)json_object_object_get_ex(i_paras->val_jso, RFPROP_OEM_HUAWEI, &huawei);
    *o_message_jso = json_object_new_array();
    
    set_smart_mode = check_fan_smart_cooling_mode(huawei, o_message_jso);
    set_control_mode = json_object_object_get_ex(huawei, RFPROP_FAN_ADJUSTMENT_MODE, &obj);
    if (set_smart_mode && set_control_mode && !strcmp(dal_json_object_get_str(obj), RFPROP_COOLING_MODE_MANUAL)) {
        json_object *msg_jso = NULL;
        (void)create_message_info(MSGID_SMART_MODE_CONFLICT_WITH_CONTROL_MODE, "Oem/Huawei", &msg_jso);
        (void)json_object_array_add(*o_message_jso, msg_jso);
        return HTTP_BAD_REQUEST;
    }
    
    ret = set_fan_control_mode(i_paras, huawei, *o_message_jso);
    if (ret == HTTP_INTERNAL_SERVER_ERROR) {
        return ret;
    } else if (HTTP_OK == ret) {
        rf_ret = HTTP_OK;
    }

    
    (void)dal_get_software_type(&software_type);
    if (software_type == MGMT_SOFTWARE_TYPE_PANGEA_MGNT) {
        json_object* msg_jso = NULL;
        debug_log(DLOG_ERROR, "[%s]: Current board cannot support smart cooling mode.", __FUNCTION__);
        (void)create_message_info(MSGID_ACCESS_DENIED, "Oem/Huawei", &msg_jso, (const gchar*)i_paras->uri);
        (void)json_object_array_add(*o_message_jso, msg_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = set_fan_smart_mode(i_paras, huawei, *o_message_jso);
    if (ret == HTTP_INTERNAL_SERVER_ERROR) {
        return ret;
    } else if (HTTP_OK == ret) {
        rf_ret = HTTP_OK;
    }

    return rf_ret;
}


gint32 smm_chassis_enclosure_thermal_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    guint8 as_status = 0;
    (void)get_board_active_state(&as_status);
    if (!redfish_check_smm_chassis_uri_valid(i_paras->uri) || (as_status != ACTIVE_STATE)) {
        return HTTP_NOT_FOUND;
    }
    *prop_provider = g_chassis_enclosure_thermal_provider;
    *count = sizeof(g_chassis_enclosure_thermal_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}


gint32 smm_chassis_enclosure_thermal_rsc(PROVIDER_PARAS_S* i_paras, json_object* o_rsc_jso,
                                         json_object* o_err_array_jso, gchar** o_rsp_body_str)
{
    OBJ_HANDLE event_handle = 0;
    guint8 event_detail_custom_id = RF_RSC_CUSTOM_ID_DEDAULT;

    if ((i_paras == NULL) || (o_rsc_jso == NULL) || (o_err_array_jso == NULL)) {
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__);
        return RF_FAILED;
    }

    (void)provider_get_prop_values(i_paras, &o_rsc_jso, o_err_array_jso,
                                   g_chassis_enclosure_thermal_provider,
                                   G_N_ELEMENTS(g_chassis_enclosure_thermal_provider));

    // 复用事件上报的CMCC定制化属性显示FanSummary资源信息 非CMCC定制直接返回
    (void)dal_get_object_handle_nth(CLASS_RF_EVT_SVC, 0, &event_handle);
    (void)dal_get_property_value_byte(event_handle, PROPERTY_RF_EVT_SVC_EVENT_DETAIL_CUSTOMIZED_ID,
                                      &event_detail_custom_id);
    if (event_detail_custom_id != RF_RSC_CUSTOM_ID_CMCC) {
        return RF_OK;
    }

    
    (void)_get_smm_fan_health_group_value(&o_rsc_jso);

    return RF_OK;
}

/*****************************************************************************
函 数 名  : smm_chassis_enclosure_thermal_etag_del_property
功能描述  : 去掉不计算etag的属性
输入参数  : PROVIDER_PARAS_S* i_paras
PROPERTY_PROVIDER_S** prop_provider
guint32* count
输出参数  : 无
返 回 值  : gint32

1.日    期   : 2018年1月10日
作    者   : likesong l00422028
修改内容   : 新生成函数

**************************************************************************** */
gint32 smm_chassis_enclosure_thermal_etag_del_property(json_object *object)
{
    SPECIAL_PROP_S value[] = {
        { 2, { RFPROP_SENSOR_FANS, RFPROP_STATUS, NULL, NULL, NULL } },
        { 2, { RFPROP_SENSOR_FANS, RFPROP_SENSOR_READING, NULL, NULL, NULL } },
        { 0, { NULL, NULL, NULL, NULL, NULL } }
    };
    return rsc_del_none_etag_affected_property(object, value, G_N_ELEMENTS(value));
}
