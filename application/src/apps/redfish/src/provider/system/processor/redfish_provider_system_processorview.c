
#include "redfish_provider.h"
#include "redfish_provider_system.h"

LOCAL gint32 get_system_processor_view_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_processor_view_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_processor_view_information(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_system_processor_view_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_processor_view_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_processor_view_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_INFORMATION, CLASS_CPU, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_processor_view_information, NULL, NULL, ETAG_FLAG_ENABLE}
};


LOCAL gchar *rfprocessorview_cpu_state_to_string(guchar cpu_state)
{
    if (0 == cpu_state) {
        return ENABLED_STRING;
    } else if (1 == cpu_state) {
        return DISABLED_STRING;
    } else {
        return "\0";
    }
}


LOCAL gint32 rf_get_processor_view_info_id(OBJ_HANDLE obj_handle, json_object *o_result_jso)
{
    gint32 ret;
    guchar cpu_id = 0;
    gchar cpu_id_s[PROP_VAL_LENGTH];

    return_val_if_expr(NULL == o_result_jso, VOS_ERR);

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_CPU_PHYSIC_ID, &cpu_id);
    return_val_if_expr((ret != VOS_OK) || (cpu_id == 0), VOS_ERR);

    (void)snprintf_s(cpu_id_s, PROP_VAL_LENGTH, PROP_VAL_LENGTH - 1, "%u", cpu_id);

    json_object_object_add(o_result_jso, RFPROP_COMMON_ID, json_object_new_string(cpu_id_s));

    return VOS_OK;
}


LOCAL gint32 rf_get_processor_view_info_maxspeed(OBJ_HANDLE obj_handle, json_object *o_result_jso)
{
    gint32 value;
    gchar value_string[PROP_VAL_LENGTH];
    json_object *tmp_obj = NULL;

    return_val_if_expr(NULL == o_result_jso, VOS_ERR);

    (void)dal_get_property_value_string(obj_handle, PROPERTY_CPU_MAX_SPEED, value_string, PROP_VAL_LENGTH);

    
    if (rf_string_check(value_string) != VOS_OK) {
        json_object_object_add(o_result_jso, RFPROP_SYSTEM_PROCESEEOR_MAXSPEED, NULL);
        return VOS_ERR;
    }

    value = rfprocessor_maxspeed_to_intmhz(value_string, strlen(value_string));
    if (0 == value) {
        json_object_object_add(o_result_jso, RFPROP_SYSTEM_PROCESEEOR_MAXSPEED, NULL);
        return VOS_ERR;
    }

    
    tmp_obj = json_object_new_int(value);

    json_object_object_add(o_result_jso, RFPROP_SYSTEM_PROCESEEOR_MAXSPEED, tmp_obj);

    return VOS_OK;
}


LOCAL gint32 rf_get_processor_view_info_socket(OBJ_HANDLE obj_handle, json_object *o_result_jso)
{
    gint32 ret;
    guchar cpu_id = 0;

    return_val_if_expr(NULL == o_result_jso, VOS_ERR);

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_CPU_PHYSIC_ID, &cpu_id);
    if ((ret != VOS_OK) || (cpu_id == 0)) {
        json_object_object_add(o_result_jso, RFPROP_SYSTEM_PROCESEEOR_SOCKET, NULL);
    } else {
        json_object_object_add(o_result_jso, RFPROP_SYSTEM_PROCESEEOR_SOCKET, json_object_new_int(cpu_id - 1));
    }

    return VOS_OK;
}


LOCAL gint32 rf_get_processor_view_info_frequency(OBJ_HANDLE obj_handle, json_object *o_result_jso)
{
    gint32 value;
    gchar value_string[PROP_VAL_LENGTH];
    json_object *tmp_obj = NULL;

    return_val_if_expr(NULL == o_result_jso, VOS_ERR);

    (void)dal_get_property_value_string(obj_handle, PROPERTY_CPU_CURRENT_SPEED, value_string, PROP_VAL_LENGTH);

    
    if (rf_string_check(value_string) != VOS_OK) {
        json_object_object_add(o_result_jso, RFPROP_PROCESSOR_FREQUENCY, NULL);
        return VOS_ERR;
    }

    value = rfprocessor_maxspeed_to_intmhz(value_string, strlen(value_string));
    if (0 == value) {
        json_object_object_add(o_result_jso, RFPROP_PROCESSOR_FREQUENCY, NULL);
        return VOS_ERR;
    }

    
    tmp_obj = json_object_new_int(value);

    json_object_object_add(o_result_jso, RFPROP_PROCESSOR_FREQUENCY, tmp_obj);

    return VOS_OK;
}


LOCAL gint32 rf_get_processor_view_info_enabledsetting(OBJ_HANDLE obj_handle, json_object *o_result_jso)
{
    gint32 ret;
    guchar cpu_enabled = 0;

    return_val_if_expr(NULL == o_result_jso, VOS_ERR);

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_CPU_DISABLE_CPU_SW, &cpu_enabled);
    if (ret != VOS_OK) {
        json_object_object_add(o_result_jso, RFPROP_SYSTEM_PROCESEEOR_ENABLEDSETTING, NULL);
    } else {
        json_object_object_add(o_result_jso, RFPROP_SYSTEM_PROCESEEOR_ENABLEDSETTING,
            json_object_new_boolean(cpu_enabled == 0 ? TRUE : FALSE));
    }

    return VOS_OK;
}


LOCAL gint32 rf_get_processor_view_info_temperature(OBJ_HANDLE obj_handle, json_object *o_result_jso)
{
    gint32 ret;
    guchar cpu_id = 0;
    gchar name_string[PROP_VAL_LENGTH] = {0};
    gdouble value = 0.0;
    OBJ_HANDLE sen_obj_handle = 0;
    gchar sensor_name[MAX_NAME_SIZE] = {0};

    GSList *obj_list = NULL;
    GSList *obj_node = NULL;

    return_val_if_expr(o_result_jso == NULL, VOS_ERR);

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_CPU_PHYSIC_ID, &cpu_id);
    return_val_do_info_if_expr(ret != VOS_OK, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d: dal_get_property_value_byte  error.", __FUNCTION__, __LINE__));

    (void)snprintf_s(name_string, PROP_VAL_LENGTH, PROP_VAL_LENGTH - 1, "CPU%u Core Rem", cpu_id);

    ret = dfl_get_object_list(CLASS_THRESHOLD_SENSOR, &obj_list);
    return_val_do_info_if_expr(ret != VOS_OK, VOS_ERR,
        debug_log(DLOG_MASS, "%s, %d: dfl_get_object_list fail.\n", __FUNCTION__, __LINE__));

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        sen_obj_handle = (OBJ_HANDLE)obj_node->data;
        ret = dal_get_property_value_string(sen_obj_handle, PROPERTY_THR_SENSOR_SENSOR_NAME, sensor_name,
            sizeof(sensor_name));
        if (VOS_OK != ret) {
            continue;
        }

        if (0 == g_strcmp0(name_string, sensor_name)) {
            break;
        }
    }

    
    g_slist_free(obj_list);

    ret = rfprocessor_get_sensor_reading(sen_obj_handle, &value);
    if (VOS_OK == ret) {
        json_object_object_add(o_result_jso, RFPROP_SYSTEM_PROCESEEOR_TEMPERATURE,
            ex_json_object_new_double(value, "%.2f"));
    } else {
        json_object_object_add(o_result_jso, RFPROP_SYSTEM_PROCESEEOR_TEMPERATURE, NULL);
    }

    return VOS_OK;
}

LOCAL gint32 rf_get_processor_view_info_status(OBJ_HANDLE obj_handle, json_object *o_result_jso, guchar power_state)
{
    gint32 ret = 0;
    guchar cpu_state = 0;
    guchar enablementstatus = 0;
    guchar presence = 0;
    guchar health = 0;
    const gchar *state_str = NULL;
    json_object *status_obj_jso = NULL;

    return_val_if_expr(NULL == o_result_jso, VOS_ERR);

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_CPU_PRESENCE, &presence);
    // power_state取值为1表示上电状态
    if (power_state == 1) {
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_CPU_HEALTH, &health);
    } else {
        health = INVALID_DATA_BYTE;
    }

    if (TRUE == presence) {
        
        ret = dal_get_property_value_byte(obj_handle, PROPERTY_CPU_FAILURE_ISO, &cpu_state);
        do_if_expr(VOS_OK != ret, debug_log(DLOG_ERROR, "%s, %d: error.", __FUNCTION__, __LINE__); cpu_state = 0xff);

        ret = dal_get_property_value_byte(obj_handle, PROPERTY_CPU_DISABLE_CPU_HW, &enablementstatus);
        do_if_expr(VOS_OK != ret, debug_log(DLOG_ERROR, "%s, %d: error.", __FUNCTION__, __LINE__); cpu_state = 0xff);

        cpu_state = (cpu_state | enablementstatus);
        
        state_str = rfprocessorview_cpu_state_to_string(cpu_state);
    } else {
        
        health = INVALID_DATA_BYTE;
        state_str = RF_STATE_ABSENT;
        
    }

    (void)get_resource_status_property(&health, NULL, state_str, &status_obj_jso, TRUE);

    if (status_obj_jso != NULL) {
        json_object_object_add(o_result_jso, RFPROP_SYSTEM_STATUS, status_obj_jso);
    } else {
        json_object_object_add(o_result_jso, RFPROP_SYSTEM_STATUS, NULL);
    }

    return VOS_OK;
}


void get_processor_cache_size(OBJ_HANDLE obj_handle, const gchar *prop_name, const gchar *rf_prop_name,
    json_object *o_result_jso)
{
    gint32 ret;
    guint32 prop_val = 0;

    ret = dal_get_property_value_uint32(obj_handle, prop_name, &prop_val);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: get property %s fail, ret:%d", __FUNCTION__, prop_name, ret);
        json_object_object_add(o_result_jso, rf_prop_name, NULL);
        return;
    }

    
    json_object_object_add(o_result_jso, rf_prop_name, (prop_val == 0 ? NULL : json_object_new_int(prop_val)));
}


LOCAL void get_processor_core_and_thread_num(OBJ_HANDLE obj_handle, json_object *o_result_jso)
{
    gint32 ret;
    guint16 core_num = 0;
    guint16 thread_num = 0;

    ret = dal_get_property_value_uint16(obj_handle, PROPERTY_CPU_CORE_COUNT, &core_num);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: get property %s fail ,ret:%d", __FUNCTION__, PROPERTY_CPU_CORE_COUNT, ret);
    }

    ret = dal_get_property_value_uint16(obj_handle, PROPERTY_CPU_THREAD_COUNT, &thread_num);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: get property %s fail, ret:%d", __FUNCTION__, PROPERTY_CPU_THREAD_COUNT, ret);
    }

    
    json_object_object_add(o_result_jso, RFPROP_SYSTEM_PROCESEEOR_TOTALCORES,
        (core_num == 0 ? NULL : json_object_new_int(core_num)));
    json_object_object_add(o_result_jso, RFPROP_SYSTEM_PROCESEEOR_TOTALTHTREADS,
        (thread_num == 0 ? NULL : json_object_new_int(thread_num)));
}


LOCAL void get_cpu_model(OBJ_HANDLE obj_handle, json_object* o_result_jso)
{
    gint32  ret;
    gchar   model_string[MAX_PROPERTY_VALUE_LEN] = {0};
    gboolean is_customized;
    gchar value_string[MAX_PROPERTY_VALUE_LEN] = {0};
    json_object* property_jso = NULL;

    ret = dal_get_property_value_string(obj_handle, PROPERTY_CPU_VERSION, value_string, MAX_PROPERTY_VALUE_LEN);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: failed to get cpu model, object(%s) ret(%d)", __FUNCTION__,
            dfl_get_object_name(obj_handle), ret);
        return;
    }

    is_customized = dal_is_customized_by_cmcc();
    if (is_customized) {
        
        ret = format_cpu_model_by_cmcc(value_string, model_string, sizeof(model_string));
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: failed format cpu model", __FUNCTION__);
            return;
        }

        property_jso = json_object_new_string((const gchar*)model_string);
        if (property_jso == NULL) {
            debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
            return;
        }
    } else {
        property_jso = json_object_new_string((const gchar*)value_string);
        if (property_jso == NULL) {
            debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
            return;
        }
    }

    json_object_object_add(o_result_jso, RFPROP_SYSTEM_PROCESEEOR_MODEL, property_jso);        
    return;
}


LOCAL gint32 add_cpu_info(json_object *info_array, guchar power_state)
{
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    json_object *cpu_jso = NULL;
    OBJ_HANDLE obj_handle;

    
    return_val_do_info_if_expr(dfl_get_object_list(CLASS_CPU, &obj_list) != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s: get obj_list fail", __FUNCTION__));

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        obj_handle = (OBJ_HANDLE)obj_node->data;
        cpu_jso = json_object_new_object();
        continue_if_expr(cpu_jso == NULL);
        json_object_array_add(info_array, cpu_jso);

        continue_if_expr(rf_get_processor_view_info_id(obj_handle, cpu_jso) != VOS_OK);

        json_object_object_add(cpu_jso, PROPERTY_CPU_PROCESSORTYPE, json_object_new_string(PROPERTY_CPU_CPU));
        rf_add_property_jso_string(obj_handle, PROPERTY_CPU_ARCHITECTURE, RFPROP_SYSTEM_PROCESEEOR_ARCHITECTURE,
            cpu_jso);
        rf_add_property_jso_string(obj_handle, PROPERTY_CPU_INSTRUCTIONSET, RFPROP_SYSTEM_PROCESEEOR_INSTRUCTIONSET,
            cpu_jso);
        rf_add_property_jso_string(obj_handle, PROPERTY_CPU_MANUFACTURER, RFPROP_SYSTEM_PROCESEEOR_MANUFACTURER,
            cpu_jso);
        get_cpu_model((OBJ_HANDLE)obj_node->data, cpu_jso);
        rf_add_property_jso_string(obj_handle, PROPERTY_CPU_PROCESSORID,
            RFPROP_SYSTEM_PROCESEEOR_IDENTIFICATIONREGISTERS, cpu_jso);

        (void)rf_get_processor_view_info_maxspeed(obj_handle, cpu_jso);
        get_processor_core_and_thread_num(obj_handle, cpu_jso);
        (void)rf_get_processor_view_info_socket(obj_handle, cpu_jso);
        get_processor_cache_size(obj_handle, PROPERTY_CPU_L1CACHE, RFPROP_SYSTEM_PROCESEEOR_ONECACHE, cpu_jso);
        get_processor_cache_size(obj_handle, PROPERTY_CPU_L2CACHE, RFPROP_SYSTEM_PROCESEEOR_TWOCACHE, cpu_jso);
        get_processor_cache_size(obj_handle, PROPERTY_CPU_L3CACHE, RFPROP_SYSTEM_PROCESEEOR_THREECACHE, cpu_jso);
        rf_add_property_jso_string(obj_handle, PROPERTY_CPU_DEVICENAME, RFPROP_DEVICELOCATOR, cpu_jso);
        rf_add_property_jso_string(obj_handle, PROPERTY_CPU_LOCATION, RFPROP_POSITION, cpu_jso);
        rf_add_property_jso_string(obj_handle, PROPERTY_CPU_PART_NUM, RFPROP_PART_NUMBER, cpu_jso);

        rf_get_processor_view_info_temperature(obj_handle, cpu_jso);
        rf_get_processor_view_info_enabledsetting(obj_handle, cpu_jso);
        rf_get_processor_view_info_frequency(obj_handle, cpu_jso);
        rf_add_property_jso_string(obj_handle, PROPERTY_CPU_MEMORY_TEC, RFPROP_SYSTEM_PROCESEEOR_OTHERPARAMETERS,
            cpu_jso);
        rf_get_processor_view_info_status(obj_handle, cpu_jso, power_state);
        rf_add_property_jso_string(obj_handle, PROPERTY_CPU_SN, RFPROP_SERIAL_NUMBER, cpu_jso); 
    }
    g_slist_free(obj_list);
    return HTTP_OK;
}


LOCAL gint32 get_npu_rsc_id(OBJ_HANDLE obj_handle, json_object *npu_jso)
{
    gchar npu_name[MAX_OBJECT_NAME_LEN] = {0};
    gint32 ret;
    if (dal_is_pcie_npu() == TRUE) {
        ret = construct_npu_name(obj_handle, npu_name, sizeof(npu_name));;
    } else {
        ret = dal_get_property_value_string(obj_handle, PROPERTY_NPU_NAME, npu_name, sizeof(npu_name));
    }
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s:get npu name failed, ret:%d", __FUNCTION__, ret);
        return VOS_ERR;
    }

    format_xpu_member_id(npu_name, sizeof(npu_name));
    json_object_object_add(npu_jso, RFPROP_COMMON_ID, json_object_new_string((const gchar *)npu_name));
    return VOS_OK;
}


LOCAL void get_npu_status(OBJ_HANDLE obj_handle, json_object *npu_jso, guchar power_state)
{
    gint32 ret;
    guchar health = 0;
    guchar state = 0;
    const gchar *state_p = NULL;
    json_object *status_obj = NULL;
    guchar presence = 0;

    if (power_state == 1) {
        ret = dal_get_property_value_byte(obj_handle, PROPERTY_NPU_HEALTH, &health);
        do_if_expr(ret != VOS_OK, debug_log(DLOG_ERROR, "%s: get npu health failed, ret:%d", __FUNCTION__, ret);
            health = INVALID_DATA_BYTE);
    } else {
        health = INVALID_DATA_BYTE;
    }

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_NPU_PRESENCE, &presence);
    if (presence == 1) {
        state_p = rfprocessorview_cpu_state_to_string(state);
    } else {
        state_p = ABSENT_STRING;
    }

    get_resource_status_property(&health, NULL, state_p, &status_obj, TRUE);
    json_object_object_add(npu_jso, RFPROP_STATUS, status_obj);
    return;
}


LOCAL gint32 time_second_to_str(guint32 time_sec, gchar* buff, guint32 buff_len)
{
    time_t lt = time_sec;
    struct tm *tb = NULL;

    if (buff == NULL || buff_len == 0) {
        return RET_ERR;
    }

    if (memset_s(buff, buff_len, 0, buff_len) != EOK) {
        debug_log(DLOG_ERROR, "%s : memset_s fail", __FUNCTION__);
        return RET_ERR;
    }

    struct tm local_time;
    (void)memset_s(&local_time, sizeof(local_time), 0, sizeof(local_time));
    tb = localtime_r(&lt, &local_time);
    if (tb == NULL) {
        debug_log(DLOG_ERROR, "%s : localtime convert fail", __FUNCTION__);
        return RET_ERR;
    }

    if (strftime(buff, buff_len - 1, "%F %X", &local_time) < 0) {
        debug_log(DLOG_ERROR, "%s : convert time fail", __FUNCTION__);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 rf_get_npu_ecc_record(OBJ_HANDLE npu_handle, guint32* out_ecc_num, json_object* out_time_obj)
{
#define TIME_STAMP_LEN_MAX 32
    gint32 ret;
    guint32 cnt;
    gsize prop_size = 0;
    gchar date_time[TIME_STAMP_LEN_MAX] = {0};
    GVariant *prop_var = NULL;
    const guint32* ecc_time = NULL;

    (void)dal_get_property_value_uint32(npu_handle, PROPERTY_NPU_HISTORY_ECC_COUNT, out_ecc_num);
    debug_log(DLOG_DEBUG, "%s ecc num(%d).", dfl_get_object_name(npu_handle), *out_ecc_num);

    if (*out_ecc_num > 0) {
        ret = dfl_get_property_value(npu_handle, PROPERTY_NPU_HISTORY_ECC_TIME, &prop_var);
        if (ret != DFL_OK || prop_var == NULL) {
            debug_log(DLOG_ERROR, "get %s history ecc failed, ret=%d", dfl_get_object_name(npu_handle), ret);
            goto EXIT;
        }

        ecc_time = (const guint32*)g_variant_get_fixed_array(prop_var, &prop_size, sizeof(guint32));
        if (ecc_time == NULL || prop_size < *out_ecc_num) {
            debug_log(DLOG_ERROR, "get ecc_time fail or ecc_num(%u) exceed.", *out_ecc_num);
            goto EXIT;
        }

        for (cnt = 0; cnt < *out_ecc_num; cnt++) {
            ret = time_second_to_str(ecc_time[cnt], date_time, sizeof(date_time));
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "time_second_to_str(%u) fail.", ecc_time[cnt]);
                break;
            }
            debug_log(DLOG_DEBUG, "get %s history ecc time(%d:%s)",
                dfl_get_object_name(npu_handle), ecc_time[cnt], date_time);
            json_object_array_add(out_time_obj, json_object_new_string(date_time));
        }
    }
EXIT:
    if (prop_var != NULL) {
        g_variant_unref(prop_var);
    }

    return RET_OK;
}


LOCAL void rf_get_npu_history_ecc(OBJ_HANDLE obj_handle, json_object* npu_jso, guchar power_state)
{
    gint32 ret;
    guint32 ecc_num = 0;
    json_object *time_obj = NULL;
    guchar presence = 0;

    do {
        time_obj = json_object_new_array();
        if (time_obj == NULL) {
            debug_log(DLOG_ERROR, "%s: json_object_new_array failed", __FUNCTION__);
            break;
        }

        (void)dal_get_property_value_byte(obj_handle, PROPERTY_NPU_PRESENCE, &presence);
        if (!power_state || !presence) {
            break;
        }

        ret = rf_get_npu_ecc_record(obj_handle, &ecc_num, time_obj);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: get_npu_ecc_num_time failed(%d)", __FUNCTION__, ret);
            break;
        }
    } while (0);

    
    json_object_object_add(npu_jso, RFPROP_SYSTEM_PROCESSOR_NPU_HISTORY_ECC_NUM, json_object_new_int(ecc_num));
    json_object_object_add(npu_jso, RFPROP_SYSTEM_PROCESSOR_NPU_HISTORY_ECC_TIME, time_obj);
    return;
}


LOCAL gint32 add_npu_info(json_object *info_array, guchar power_state)
{
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    json_object *npu_jso = NULL;
    guchar power_on = ENABLED;
    OBJ_HANDLE obj_handle;

    gint32 ret = dfl_get_object_list(CLASS_NPU, &obj_list);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_OK,
        debug_log(DLOG_DEBUG, "%s:get object list failed, ret:%d", __FUNCTION__, ret));

    for (obj_node = obj_list; obj_node != NULL; obj_node = obj_node->next) {
        npu_jso = json_object_new_object();
        continue_do_info_if_expr(npu_jso == NULL, debug_log(DLOG_ERROR, "%s:new json object failed", __FUNCTION__));
        json_object_array_add(info_array, npu_jso);

        obj_handle = (OBJ_HANDLE)obj_node->data;

        
        ret = get_npu_rsc_id(obj_handle, npu_jso);
        continue_do_info_if_expr(ret != VOS_OK, json_object_put(npu_jso));

        
        json_object_object_add(npu_jso, RFPROP_SYSTEM_PROCESSOR_TYPE, json_object_new_string("NPU"));

        
        rf_add_property_jso_string(obj_handle, PROPERTY_NPU_ARCHITECTURE,
            RFPROP_SYSTEM_PROCESEEOR_ARCHITECTURE, npu_jso);

        
        rf_add_property_jso_string(obj_handle, RFPROP_SYSTEM_PROCESEEOR_INSTRUCTIONSET,
            PROPERTY_NPU_INSTRCTIONSET, npu_jso);

        
        rf_add_property_jso_string(obj_handle, PROPERTY_NPU_MANUFACTURER,
            RFPROP_SYSTEM_PROCESEEOR_MANUFACTURER, npu_jso);

        
        rf_add_property_jso_string(obj_handle, PROPERTY_NPU_VERSION, RFPROP_SYSTEM_PROCESEEOR_MODEL, npu_jso);

        
        rf_add_property_jso_string(obj_handle, PROPERTY_NPU_FIRMWARE_VERSION,
            RFPROP_SYSTEM_PROCESSOR_NPU_FWVERSION, npu_jso);

        
        get_npu_temp_or_power(obj_handle, PROPERTY_NPU_POWER, RFPROP_SYSTEM_PROCESSOR_NPU_POWER, npu_jso);

        
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_NPU_POWER_ON, &power_on);
        json_object_object_add(npu_jso, RFPROP_SYSTEM_PROCESSOR_NPU_POWERON,
            json_object_new_boolean((power_on == ENABLED) ? TRUE : FALSE));

        
        rf_add_property_jso_string(obj_handle, PROPERTY_NPU_SN, RFPROP_SYSTEM_PROCESSOR_NPU_SERIALNUMBER, npu_jso);

        
        get_npu_temp_or_power(obj_handle, PROPERTY_NPU_AICORE_TEMP, RFPROP_SYSTEM_PROCESSOR_NPU_AICORE_TEMP, npu_jso);

        
        get_npu_temp_or_power(obj_handle, PROPERTY_NPU_HBM_TEMP, RFPROP_SYSTEM_PROCESSOR_NPU_HBM_TEMP, npu_jso);

        
        get_npu_temp_or_power(obj_handle, PROPERTY_NPU_NIMBUS_TEMP, RFPROP_SYSTEM_PROCESSOR_NPU_NIMBUS_TEMP, npu_jso);

        
        rf_add_property_jso_string(obj_handle, PROPERTY_NPU_COMPUTE_CAPABILITY,
            RFPROP_SYSTEM_PROCESSOR_NPU_COMPUTE_CAPABILITY, npu_jso);

        
        get_npu_temp_or_power(obj_handle, PROPERTY_NPU_AICORE_MAX_SPEED,
            RFPROP_SYSTEM_PROCESSOR_NPU_AICORE_MAX_SPEED, npu_jso);

        
        get_npu_utl_oem_huawei(obj_handle, npu_jso);

        
        get_npu_ecc_info(obj_handle, npu_jso);

        
        get_npu_memory_info(obj_handle, npu_jso);

        
        get_npu_eth_info(obj_handle, npu_jso);

        
        get_npu_status(obj_handle, npu_jso, power_state);

        
        rf_get_npu_history_ecc(obj_handle, npu_jso, power_state);
    }

    g_slist_free(obj_list);
    return HTTP_OK;
}


LOCAL gint32 get_processor_view_gpu_info_id(OBJ_HANDLE obj_handle, json_object* json_node)
{
    gint32 ret;
    guchar slot = 0;
    gchar  gpu_id[MAX_RSC_NAME_LEN] = {0};

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_GPU_CARD_SLOT, &slot);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_byte failed, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    
    ret = sprintf_s(gpu_id, sizeof(gpu_id), MEMBER_ID_FORMAT_SYSTEM_REPROCESSORS_GPU, slot);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: sprintf_s failed, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    json_object_object_add(json_node, RFPROP_COMMON_ID, json_object_new_string((const char *)gpu_id));
    return RET_OK;
}


LOCAL void get_processor_view_gpu_status(OBJ_HANDLE obj_handle, json_object* json_node)
{
    gint32 ret;
    guchar gpu_health = 0;
    json_object* json_status = NULL;

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_GPU_CARD_HEALTH, &gpu_health);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_byte failed, ret = %d", __FUNCTION__, ret);
        json_object_object_add(json_node, RFPROP_SYSTEM_PROCESEEOR_STATUS, NULL);
        return;
    }

    (void)get_resource_status_property(&gpu_health, NULL, "Enabled", &json_status, TRUE);
    json_object_object_add(json_node, RFPROP_SYSTEM_PROCESEEOR_STATUS, json_status);
}


LOCAL gint32 get_processor_view_gpu_info(json_object* o_result_jso)
{
    gint32 ret;
    GSList* obj_list = NULL;
    GSList* obj_gpu = NULL;
    json_object* json_node = NULL;
    OBJ_HANDLE obj_handle = 0;

    
    ret = dfl_get_object_list(CLASS_GPU_CARD, &obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: dfl_get_object_list(%s) failed, ret = %d", __FUNCTION__, CLASS_GPU_CARD, ret);
        return HTTP_OK;
    }

    for (obj_gpu = obj_list; obj_gpu; obj_gpu = obj_gpu->next) {
        obj_handle = (OBJ_HANDLE)(intptr_t)(obj_gpu->data);
        if (!gpu_is_present(obj_handle)) {
            continue;
        }

        json_node = json_object_new_object();
        if (json_node == NULL) {
            debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__);
            continue;
        }

        
        ret = get_processor_view_gpu_info_id(obj_handle, json_node);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: get_processor_view_gpu_info_id failed, ret = %d", __FUNCTION__, ret);
            json_object_put(json_node);
            continue;
        }

        
        json_object_object_add(json_node, RFPROP_SYSTEM_PROCESEEOR_TYPE, json_object_new_string("GPU"));

        
        rf_add_property_jso_string(obj_handle, PROPERTY_GPU_CARD_MANU,
            RFPROP_SYSTEM_PROCESEEOR_MANUFACTURER, json_node);

        
        rf_add_property_jso_string(obj_handle, PROPERTY_GPU_CARD_MODEL, RFPROP_SYSTEM_PROCESEEOR_MODEL, json_node);

        
        get_processor_view_gpu_status(obj_handle, json_node);

        
        rf_add_property_jso_string(obj_handle, PROPERTY_GPU_CARD_FIRM_VER,
            RFPROP_GPU_CARD_FIRMWARE_VERSION, json_node);

        
        get_gpu_oem_huawei(obj_handle, json_node);

        (void)json_object_array_add(o_result_jso, json_node);
    }

    g_slist_free(obj_list);
    return HTTP_OK;
}


LOCAL gint32 get_system_processor_view_information(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guchar sys_power_state = 0;

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_array();
    return_val_do_info_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:json_object_new_array fail", __FUNCTION__));

    
    ret = rf_get_system_powerstate(&sys_power_state);
    do_val_if_expr(ret != VOS_OK, debug_log(DLOG_ERROR, "%s: get system power state fail", __FUNCTION__));

    
    ret = add_cpu_info(*o_result_jso, sys_power_state);
    if (ret != HTTP_OK) {
        goto ERR_EXIT;
    }

    
    ret = add_npu_info(*o_result_jso, sys_power_state);
    if (ret != HTTP_OK) {
        goto ERR_EXIT;
    }

    
    ret = get_processor_view_gpu_info(*o_result_jso);
    if (ret != HTTP_OK) {
        goto ERR_EXIT;
    }

    return HTTP_OK;
ERR_EXIT:
    json_object_put(*o_result_jso);
    *o_result_jso = NULL;
    return ret;
}


LOCAL gint32 get_system_processor_view_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;
    gint32 ret;
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_slot(slot, sizeof(slot));

    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));

    iRet = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, RFPROP_SYSTEM_REPROCESSOR_VIEW, slot);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    *o_result_jso = json_object_new_string((const char *)uri);

    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 get_system_processor_view_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
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

    
    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__));

    iRet = snprintf_s(odata_context, sizeof(odata_context), sizeof(odata_context) - 1, SYSTEMS_PROCE_VIEW_METADATA,
        slot_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    *o_result_jso = json_object_new_string(odata_context);

    return_val_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL void rsc_del_eth_statistic(json_object* object)
{
    json_object* info_obj = NULL;
    json_object* info_nth_obj = NULL;
    json_object* eth_obj = NULL;
    guint32  array_len;
    guint32  array_index;

    if (object == NULL) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return;
    }

    if (json_object_object_get_ex(object, RFPROP_INFORMATION, &info_obj) != TRUE) {
        return;
    }

    if (json_object_get_type(info_obj) != json_type_array) {
        return;
    }

    array_len = json_object_array_length(info_obj);
    for (array_index = 0; array_index < array_len; array_index++) {
        info_nth_obj = json_object_array_get_idx(info_obj, array_index);
        if (json_object_object_get_ex(info_nth_obj, RFPROP_SYSTEM_PROCESSOR_NPU_ETH, &eth_obj) != TRUE) {
            continue;
        }

        (void)json_object_object_del(eth_obj, RFPROP_SYSTEM_PROCESSOR_NPU_RX_PACKETS);
        (void)json_object_object_del(eth_obj, RFPROP_SYSTEM_PROCESSOR_NPU_TX_PACKETS);
        (void)json_object_object_del(eth_obj, RFPROP_SYSTEM_PROCESSOR_NPU_DROPPED_PACKETS);
    }
}


gint32 system_process_view_etag_del_property(json_object *object)
{
    SPECIAL_PROP_S value[] = {
        {2, {RFPROP_INFORMATION, RFPROP_SYSTEM_PROCESEEOR_TEMPERATURE, NULL, NULL, NULL}},
        {2, {RFPROP_INFORMATION, RFPROP_SYSTEM_PROCESSOR_NPU_AICORE_TEMP, NULL, NULL, NULL}},
        {2, {RFPROP_INFORMATION, RFPROP_SYSTEM_PROCESSOR_NPU_HBM_TEMP, NULL, NULL, NULL}},
        {2, {RFPROP_INFORMATION, RFPROP_SYSTEM_PROCESSOR_NPU_NIMBUS_TEMP, NULL, NULL, NULL}},
        {2, {RFPROP_INFORMATION, RFPROP_SYSTEM_PROCESSOR_NPU_POWER, NULL, NULL, NULL}},
        {2, {RFPROP_INFORMATION, RFPROP_SYSTEM_PROCESSOR_NPU_CTRL_CPU_USAGE_PERCENT, NULL, NULL, NULL}},
        {2, {RFPROP_INFORMATION, RFPROP_SYSTEM_PROCESSOR_NPU_AICORE_USAGE_PERCENT, NULL, NULL, NULL}},
        {2, {RFPROP_INFORMATION, RFPROP_NPU_MEM_UTL, NULL, NULL, NULL}},
        {2, {RFPROP_INFORMATION, RFPROP_NPU_HPM_UTL, NULL, NULL, NULL}},
        {2, {RFPROP_INFORMATION, RFPROP_NPU_MEM_BW, NULL, NULL, NULL}},
        {2, {RFPROP_INFORMATION, RFPROP_NPU_HBM_BW, NULL, NULL, NULL}},
        {2, {RFPROP_INFORMATION, RFPROP_NPU_AICPU_UTL, NULL, NULL, NULL}},
        {2, {RFPROP_INFORMATION, RFPROP_SYSTEM_PROCESSOR_NPU_MEMORY_ON_CHIP, NULL, NULL, NULL}},
        {3, {RFPROP_INFORMATION, RFPROP_GPU_CARD_POWER, RFPROP_GPU_CARD_POWER_DRAW_WATTS, NULL, NULL}},
        {3, {RFPROP_INFORMATION, RFPROP_GPU_CARD_THERMALS, RFPROP_GPU_CARD_PRIMARY_GPU_TEMP_CELSIUS, NULL, NULL}},
        {3, {RFPROP_INFORMATION, RFPROP_GPU_CARD_THERMALS, RFPROP_GPU_CARD_MEMORY_TEMP_CELISIUS, NULL, NULL}},
        {2, {RFPROP_INFORMATION, RFPROP_GPU_CARD_UTILIZATION, NULL, NULL, NULL}},
        {0, {NULL, NULL, NULL, NULL, NULL}}
    };

    rsc_del_eth_statistic(object);
    return rsc_del_none_etag_affected_property(object, value, G_N_ELEMENTS(value));
}


gint32 system_processor_view_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret;
    guchar board_type = 0;
    gboolean bool_ret;

    ret = redfish_get_x86_enable_type(&board_type);
    return_val_if_fail(VOS_OK == ret, HTTP_NOT_FOUND);
    return_val_if_fail(DISABLE != board_type, HTTP_NOT_FOUND);

    bool_ret = redfish_check_system_uri_valid(i_paras->uri);
    if (!bool_ret) {
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_system_processor_view_provider;
    *count = sizeof(g_system_processor_view_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}
