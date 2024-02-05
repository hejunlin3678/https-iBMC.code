

#include "redfish_provider.h"
#include "redfish_event.h"
#include "redfish_event_custom.h"

// 固定格式";0:event_number"的格式化输出数量 2
#define EVENT_RECORD_FORMAT_CNT 2
LOCAL pthread_mutex_t g_cmcc_event_records_mutex = PTHREAD_MUTEX_INITIALIZER;

gint32 set_redfish_evt_detail_customized_id(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    guint8 customized_id;
    gint32 ret;

    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s: input list can not be NULL.", __FUNCTION__);
        proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "Set redfish event details customized ID failed.");
        return RET_ERR;
    }

    customized_id = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    if (customized_id >= RF_RSC_CUSTOM_ID_BUTT) {
        debug_log(DLOG_ERROR, "%s: input parameter is error. customized id is %u", __FUNCTION__, customized_id);
        proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "Set redfish event details customized ID failed.");
        return RET_ERR;
    }

    ret = dal_save_property_value_byte_remote(object_handle, PROPERTY_RF_EVT_SVC_EVENT_DETAIL_CUSTOMIZED_ID,
        customized_id, DF_SAVE);
    if (ret != RET_OK) {
        if (customized_id == RF_RSC_CUSTOM_ID_DEDAULT) {
            proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH,
                "Disable custom redfish event details failed.");
        } else {
            proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH,
                "Set redfish event details customized ID(%u) failed.", customized_id);
        }

        return RET_ERR;
    } else {
        if (customized_id == RF_RSC_CUSTOM_ID_DEDAULT) {
            proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH,
                "Disable custom redfish event details successfully.");
        } else {
            proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH,
                "Set redfish event details customized ID(%u) successfully.", customized_id);
        }
        return RET_OK;
    }
}


gint32 delete_simulate_alert_event_record(void)
{
    gint32 ret;
    gsize i, j;
    OBJ_HANDLE obj_handle = INVALID_OBJ_HANDLE;
    gchar **old_event_record_str = NULL;
    gchar **new_event_record_str = NULL;
    gsize old_str_array_len = 0;
    gsize new_str_array_len;
    gchar *str = NULL;
    gchar *str_tmp = NULL;
    guint8 custom_id = 0;
    gint sscanf_cnt;
    gboolean is_simulation = FALSE;
    guint32 event_number = 0;

    ret = dal_get_evt_detail_customized_id(&custom_id);
    if ((ret != RET_OK) || (custom_id != RF_RSC_CUSTOM_ID_CMCC)) {
        return RET_OK;
    }
    ret = dal_get_object_handle_nth(CLASS_RF_EVT_SVC, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: cann't get object handle for %s", __FUNCTION__, CLASS_RF_EVT_SVC);
        return RET_ERR;
    }
    ret = dal_get_property_value_strv(obj_handle, PROPERTY_RF_EVT_SVC_EVENT_RECORDS, &old_event_record_str,
        &old_str_array_len);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    new_str_array_len = old_str_array_len + 1;
    new_event_record_str = (gchar **)g_malloc0(new_str_array_len * sizeof(gchar *));
    if (new_event_record_str == NULL) {
        debug_log(DLOG_ERROR, "%s: alloc str array failed, len is %zu", __FUNCTION__, new_str_array_len);
        goto QUIT;
    }
    for (i = 0, j = 0; i < old_str_array_len; i++) {
        str_tmp = g_strrstr(old_event_record_str[i], ";");
        if (str_tmp == NULL) {
            debug_log(DLOG_ERROR, "%s: str is null", __FUNCTION__);
            goto QUIT;
        }
        sscanf_cnt = sscanf_s(str_tmp, ";%d:%u", &is_simulation, &event_number);
        if (sscanf_cnt != EVENT_RECORD_FORMAT_CNT) {
            debug_log(DLOG_ERROR, "%s: sscanf_s failed, sscanf_cnt = %d", __FUNCTION__, sscanf_cnt);
            goto QUIT;
        }

        if (is_simulation == 1) {
            continue;
        }

        str = (gchar *)g_malloc0(MAX_EVT_TYPE_LEN);
        if (str == NULL) {
            debug_log(DLOG_ERROR, "%s: str is null", __FUNCTION__);
            goto QUIT;
        }
        ret = strncpy_s(str, MAX_EVT_TYPE_LEN, old_event_record_str[i], strlen(old_event_record_str[i]));
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: vos_snprintf_s failed, ret is %d", __FUNCTION__, ret);
            g_free(str);
            goto QUIT;
        }
        new_event_record_str[j++] = str;
    }
    ret = dal_save_property_value_array_string_remote(obj_handle, PROPERTY_RF_EVT_SVC_EVENT_RECORDS,
        (const gchar * const *)new_event_record_str, -1, DF_SAVE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: set EventRecords property failed, ret is %d", __FUNCTION__, ret);
    }

QUIT:
    g_strfreev(old_event_record_str);
    g_strfreev(new_event_record_str);

    return RET_OK;
}


LOCAL gint32 get_product_elabel(gchar *property_name, gchar *buff, guint32 buffer_len)
{
    gint32 ret;
    OBJ_HANDLE fru_obj_handle = 0;
    OBJ_HANDLE elabel_obj_handle = 0;

    ret = dal_get_specific_object_byte(CLASS_FRU, PROPERTY_FRU_ID, MOTHER_BOARD_FRU_ID, &fru_obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get fru0 failed, ret: %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    ret = dfl_get_referenced_object(fru_obj_handle, PROPERTY_FRU_ELABEL_RO, &elabel_obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get fru0 elable handle failed, ret: %d", __FUNCTION__, ret);
        return RET_ERR;
    }
    ret = dal_get_property_value_string(elabel_obj_handle, property_name, buff, buffer_len);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get fru0 elable %s failed, ret: %d", __FUNCTION__, property_name, ret);
        return RET_ERR;
    }
    return RET_OK;
}

LOCAL void custom_field_fill_null(json_object *hw_jso)
{
    json_object_object_add(hw_jso, RFPROP_SEPCIFIC_PROBLEM_ID, NULL);
    json_object_object_add(hw_jso, RFPROP_SEPCIFIC_PROBLEM, NULL);
    json_object_object_add(hw_jso, RFPROP_ALARM_STATUS, NULL);
    json_object_object_add(hw_jso, RFPROP_NE_UID, NULL);
    json_object_object_add(hw_jso, RFPROP_NE_NAME, NULL);
    json_object_object_add(hw_jso, RFPROP_NE_TYPE, NULL);
    json_object_object_add(hw_jso, RFPROP_OBJECT_UID, NULL);
    json_object_object_add(hw_jso, RFPROP_OBJECT_NAME, NULL);
    json_object_object_add(hw_jso, RFPROP_OBJECT_TYPE, NULL);
    json_object_object_add(hw_jso, RFPROP_LOCATION_INFO, NULL);
    json_object_object_add(hw_jso, RFPROP_ADD_INFO, NULL);
    return;
}


gint32 get_event_detail_customized_by_cmcc(RF_EVT_MSG_INFO *evt_info, json_object *hw_jso)
{
    gint32 ret;
    gchar product_sn[PROP_VAL_MAX_LENGTH] = {0};
    gchar product_asset[PROP_VAL_MAX_LENGTH] = {0};
    json_object *jso = NULL;
    gchar event_id_buf[MAX_STRBUF_LEN] = {0};

    
    if (strcmp(evt_info->event_type, RF_ALERT_EVT_STR) != 0) {
        custom_field_fill_null(hw_jso);
        return RET_OK;
    }
    ret = add_str_to_jso(hw_jso, (const gchar *)(evt_info->subject_location), RFPROP_EVT_SUBJECT);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: add_str_to_jso %s fail, ret is %d", __FUNCTION__, RFPROP_LOCATION_INFO, ret);
        return RET_ERR;
    }
    // 产生和消除的ID一致，如果是消除，需要减一
    if (evt_info->evt_state == 0) {
        evt_info->evt_id = evt_info->evt_id - 1;
    }
    ret = snprintf_s(event_id_buf, MAX_STRBUF_LEN, MAX_STRBUF_LEN - 1, "0x%08X", evt_info->evt_id);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "format event id failed, ret is %d", ret);
        return RET_ERR;
    }
    jso = json_object_new_string((const char *)event_id_buf);
    json_object_object_add(hw_jso, RFPROP_SEPCIFIC_PROBLEM_ID, jso);

    jso = json_object_new_string(evt_info->msg_id);
    if (jso == NULL) {
        debug_log(DLOG_ERROR, "%s: new string %s fail.", __FUNCTION__, evt_info->msg_id);
        return RET_ERR;
    }
    json_object_object_add(hw_jso, RFPROP_SEPCIFIC_PROBLEM, jso);
    json_object_object_add(hw_jso, RFPROP_ALARM_STATUS, json_object_new_int(evt_info->evt_state));

    ret = get_product_elabel(PROPERTY_ELABEL_PRODUCT_SN, product_sn, PROP_VAL_MAX_LENGTH);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get product serial number failed, ret is %d", __FUNCTION__, ret);
        return RET_ERR;
    }
    ret = get_product_elabel(PROPERTY_ELABEL_PRODUCT_ASSET_TAG, product_asset, PROP_VAL_MAX_LENGTH);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get product asset tag failed, ret is %d", __FUNCTION__, ret);
        return RET_ERR;
    }
    ret = add_str_to_jso(hw_jso, (const gchar *)product_sn, RFPROP_NE_UID);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    ret = add_str_to_jso(hw_jso, (const gchar *)product_asset, RFPROP_NE_NAME);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    jso = json_object_new_string("server");
    if (jso == NULL) {
        debug_log(DLOG_ERROR, "%s: new string server fail.", __FUNCTION__);
        return RET_ERR;
    }
    json_object_object_add(hw_jso, RFPROP_NE_TYPE, jso);

    jso = json_object_new_string(evt_info->subject_name);
    if (jso == NULL) {
        debug_log(DLOG_ERROR, "%s: new string %s fail.", __FUNCTION__, evt_info->subject_name);
        return RET_ERR;
    }
    json_object_object_add(hw_jso, RFPROP_OBJECT_UID, jso);
    json_object_object_add(hw_jso, RFPROP_OBJECT_NAME, json_object_get(jso));

    jso = json_object_new_string(dal_get_subject_type_string(evt_info->subject_type));
    if (jso == NULL) {
        debug_log(DLOG_ERROR, "%s: new string subject type(%d) fail.", __FUNCTION__, evt_info->subject_type);
        return RET_ERR;
    }
    json_object_object_add(hw_jso, RFPROP_OBJECT_TYPE, jso);

    ret = add_str_to_jso(hw_jso, (const gchar *)(evt_info->subject_location), RFPROP_LOCATION_INFO);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: add_str_to_jso %s fail, ret is %d", __FUNCTION__, RFPROP_LOCATION_INFO, ret);
        return RET_ERR;
    }
    // 该字段CMCC定制暂时无具体内容，先写死为""
    json_object_object_add(hw_jso, RFPROP_ADD_INFO, json_object_new_string(RF_STR_EMPTY));
    
    return RET_OK;
}


gint32 replace_event_severity_customized_by_cmcc(RF_EVT_MSG_INFO *evt_info, json_object *evt_msg_jso)
{
    gint32 ret;
    if (strcmp(evt_info->event_type, RF_ALERT_EVT_STR) != 0) {
        return RET_OK;
    }

    switch (evt_info->severity) {
        case EVENT_SEVERITY_NORMAL:
            ret = add_str_to_jso(evt_msg_jso, INFO_STRING, SEVERITY_KEY);
            break;
        case EVENT_SEVERITY_MINOR:
            ret = add_str_to_jso(evt_msg_jso, MINOR_STRING, SEVERITY_KEY);
            break;
        case EVENT_SEVERITY_MAJOR:
            ret = add_str_to_jso(evt_msg_jso, MAJOR_STRING, SEVERITY_KEY);
            break;
        case EVENT_SEVERITY_CRITICAL:
            ret = add_str_to_jso(evt_msg_jso, CRITICAL_STRING, SEVERITY_KEY);
            break;
        default:
            ret = RET_ERR;
            break;
    }
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: add_str_to_jso severity:%u failed, ret is %d", __FUNCTION__, evt_info->severity,
            ret);
    }
    return ret;
}


LOCAL guint32 find_event_slot_id_smm(guint8 src_type, guint8 slot_id)
{
#define SMM_SLOT_1 1
#define SMM_SLOT_2 2

    guint8 slot;
    if (slot_id != SMM1_SLOT_ID && slot_id != SMM2_SLOT_ID) {
        return slot_id;
    }
    if (src_type == EVENT_RESOURCE_HMM_STANDBY) {
        slot = (slot_id == SMM1_SLOT_ID) ? SMM_SLOT_2 : SMM_SLOT_1;
    } else {
        slot = (slot_id == SMM1_SLOT_ID) ? SMM_SLOT_1 : SMM_SLOT_2;
    }
    return slot;
}



LOCAL gboolean check_smm_chassis_event_type(guint8 subject_type)
{
    if (dal_check_if_mm_board() == FALSE) {
        return FALSE;
    }

    if (subject_type == COMPONENT_TYPE_PS ||
        subject_type == COMPONENT_TYPE_FAN ||
        subject_type == COMPONENT_TYPE_CHASSIS_BACKPLANE ||
        subject_type == COMPONENT_TYPE_MESH_CARD ||
        subject_type == COMPONENT_TYPE_SYSTEM) {
        return TRUE;
    }
    return FALSE;
}

LOCAL gint32 get_cmcc_monitor_name(RF_EVT_MSG_INFO *evt_info, gchar* cmcc_monitor_name, gsize name_size)
{
    gint32 ret;
    guint8 slot_id;
    guint8 raw_slot_id;

    if (evt_info == NULL) {
        debug_log(DLOG_ERROR, "%s: evt_info is null pointer", __FUNCTION__);
        return RET_ERR;
    }
    if (cmcc_monitor_name == NULL) {
        debug_log(DLOG_ERROR, "%s: new_monitor_name is null pointer", __FUNCTION__);
        return RET_ERR;
    }
    debug_log(DLOG_INFO, "name = %s, subject_type = %u, rsc_type = %u, evt_subject = %s, sub_name = %s, location = %s",
        evt_info->monitor_name, evt_info->subject_type, evt_info->evt_rsc_type,
        evt_info->evt_subject, evt_info->subject_name, evt_info->subject_location);

    if (!dal_check_if_mm_board() || check_smm_chassis_event_type(evt_info->subject_type)) {
        ret = snprintf_truncated_s(cmcc_monitor_name, name_size, "%s", evt_info->monitor_name);
    } else {
        raw_slot_id = dal_get_slotid();
        slot_id = find_event_slot_id_smm(evt_info->evt_rsc_type, raw_slot_id);
        
        ret = snprintf_truncated_s(cmcc_monitor_name, name_size, "%s-HMM%u", evt_info->monitor_name, slot_id);
    }
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_truncated_s failed, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }
    debug_log(DLOG_INFO, "%s: new_monitor_name is %s", __FUNCTION__, cmcc_monitor_name);
    return RET_OK;
}

LOCAL gint32 update_event_record(RF_EVT_MSG_INFO *evt_info, gboolean is_simulation, guint32 event_number,
    guint e_method)
{
    gint32 ret;
    gsize i, j;
    OBJ_HANDLE obj_handle = INVALID_OBJ_HANDLE;
    gchar **old_event_record_str = NULL;
    gchar **new_event_record_str = NULL;
    gsize old_str_array_len = 0;
    gsize new_str_array_len;
    gchar *str = NULL;
    gchar *str_tmp = NULL;
    
    gchar monitor_name[MAX_NAME_SIZE + 8] = {0};
    gint sscanf_cnt;
    gboolean is_simulation_tmp = FALSE;
    guint32 event_number_tmp = 0;

    ret = dal_get_object_handle_nth(CLASS_RF_EVT_SVC, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: cann't get object handle for %s", __FUNCTION__, CLASS_RF_EVT_SVC);
        return RET_ERR;
    }

    ret = dal_get_property_value_strv(obj_handle, PROPERTY_RF_EVT_SVC_EVENT_RECORDS, &old_event_record_str,
        &old_str_array_len);
    if (ret != RET_OK) {
        return RET_ERR;
    };

    // 删除: null+删除一个=0; 新增: null+新增一个=2
    new_str_array_len = (e_method == E_METHOD_DELETE) ? old_str_array_len : old_str_array_len + 2;
    new_event_record_str = (gchar **)g_malloc0(new_str_array_len * sizeof(gchar *));
    if (new_event_record_str == NULL) {
        debug_log(DLOG_ERROR, "%s: alloc str array failed, len is %zu", __FUNCTION__, new_str_array_len);
        ret = RET_ERR;
        goto QUIT;
    }

    debug_log(DLOG_DEBUG, "%s: new_str_array_len: %zu ", __FUNCTION__, new_str_array_len);
    if (get_cmcc_monitor_name(evt_info, monitor_name, sizeof(monitor_name)) != RET_OK) {
        ret = RET_ERR;
        goto QUIT;
    }

    for (i = 0, j = 0; i < old_str_array_len; i++) {
        if (e_method == E_METHOD_DELETE && g_str_has_prefix(old_event_record_str[i], monitor_name)) {
            str_tmp = g_strrstr(old_event_record_str[i], ";");
            if (str_tmp == NULL) {
                debug_log(DLOG_ERROR, "%s: str_tmp is null", __FUNCTION__);
                ret = RET_ERR;
                goto QUIT;
            }
            sscanf_cnt = sscanf_s(str_tmp, ";%d:%u", &is_simulation_tmp, &event_number_tmp);
            if (sscanf_cnt != EVENT_RECORD_FORMAT_CNT) {
                debug_log(DLOG_ERROR, "%s: sscanf_s failed, sscanf_cnt = %d", __FUNCTION__, sscanf_cnt);
                ret = RET_ERR;
                goto QUIT;
            }

            if (is_simulation == is_simulation_tmp) {
                debug_log(DLOG_DEBUG, "%s: %s, %d match, continue", __FUNCTION__, evt_info->monitor_name,
                    is_simulation);
                continue;
            }
        }
        str = (gchar *)g_malloc0(MAX_EVT_TYPE_LEN);
        if (str == NULL) {
            debug_log(DLOG_ERROR, "%s: str is null", __FUNCTION__);
            ret = RET_ERR;
            goto QUIT;
        }

        ret = snprintf_s(str, MAX_EVT_TYPE_LEN, MAX_EVT_TYPE_LEN - 1, "%s", old_event_record_str[i]);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: vos_snprintf_s failed, ret is %d", __FUNCTION__, ret);
            g_free(str);
            ret = RET_ERR;
            goto QUIT;
        }
        new_event_record_str[j++] = str;
    }
    // 新增
    if (e_method == E_METHOD_POST) {
        str = (gchar *)g_malloc0(MAX_EVT_TYPE_LEN);
        if (str == NULL) {
            debug_log(DLOG_ERROR, "%s: str is null", __FUNCTION__);
            ret = RET_ERR;
            goto QUIT;
        }

        ret = snprintf_s(str, MAX_EVT_TYPE_LEN, MAX_EVT_TYPE_LEN - 1, "%s;%d:%u", monitor_name, is_simulation,
            event_number);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: vos_snprintf_s failed, ret is %d", __FUNCTION__, ret);
            g_free(str);
            ret = RET_ERR;
            goto QUIT;
        }
        new_event_record_str[j] = str;
    }
    ret = dal_save_property_value_array_string_remote(obj_handle, PROPERTY_RF_EVT_SVC_EVENT_RECORDS,
        (const gchar * const *)new_event_record_str, -1, DF_SAVE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: set EventRecords property failed, ret is %d", __FUNCTION__, ret);
    }
QUIT:
    g_strfreev(old_event_record_str);
    g_strfreev(new_event_record_str);

    return ret;
}


LOCAL gint32 generate_new_event_record_id(guint32 *new_event_record_id)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = INVALID_OBJ_HANDLE;
    guint32 event_record_id = INVALID_EVENT_RECORD_ID;

    ret = dal_get_object_handle_nth(CLASS_RF_EVT_SVC, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: cann't get object handle for %s", __FUNCTION__, CLASS_RF_EVT_SVC);
        return RET_ERR;
    }

    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_RF_EVT_SVC_EVENT_RECORD_ID, &event_record_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: cann't get property value for [%s:%s], ret is %d", __FUNCTION__,
            dfl_get_object_name(obj_handle), PROPERTY_RF_EVT_SVC_EVENT_RECORD_ID, ret);
        return RET_ERR;
    }

    // 已分配的事件编号如果反转为0，或刚初始化，需要再加一
    event_record_id++;
    if (event_record_id == INVALID_EVENT_RECORD_ID) {
        event_record_id++;
    }

    *new_event_record_id = event_record_id;
    ret = dal_save_property_value_uint32_remote(obj_handle, PROPERTY_RF_EVT_SVC_EVENT_RECORD_ID, event_record_id,
        DF_SAVE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: cann't set property value for [%s:%s], ret is %d", __FUNCTION__,
            dfl_get_object_name(obj_handle), PROPERTY_RF_EVT_SVC_EVENT_RECORD_ID, ret);
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 find_event_record(RF_EVT_MSG_INFO *evt_info, gboolean is_simulation, guint32 *event_number)
{
    gsize i;
    gint32 ret;
    OBJ_HANDLE obj_handle;
    gchar **event_record_str = NULL;
    gsize str_array_len = 0;
    gchar *str = NULL;
    
    gchar monitor_name[MAX_NAME_SIZE + 8] = {0};
    gint sscanf_cnt;
    gboolean is_simulation_tmp = FALSE;
    guint32 event_number_tmp = 0;

    ret = dal_get_object_handle_nth(CLASS_RF_EVT_SVC, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: cann't get object handle for %s, ret %d", __FUNCTION__, CLASS_RF_EVT_SVC, ret);
        return RET_ERR;
    }

    ret = dal_get_property_value_strv(obj_handle, PROPERTY_RF_EVT_SVC_EVENT_RECORDS, &event_record_str, &str_array_len);
    if (ret != RET_OK) {
        return RET_ERR;
    }
    ret = get_cmcc_monitor_name(evt_info, monitor_name, sizeof(monitor_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get_cmcc_monitor_namefailed, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }
    for (i = 0; i < str_array_len; i++) {
        if (event_record_str[i] != NULL && g_str_has_prefix(event_record_str[i], monitor_name)) {
            str = g_strrstr(event_record_str[i], ";");
            if (str == NULL) {
                ret = RET_ERR;
                break;
            }
            sscanf_cnt = sscanf_s(str, ";%d:%u", &is_simulation_tmp, &event_number_tmp);
            if (sscanf_cnt != EVENT_RECORD_FORMAT_CNT) {
                debug_log(DLOG_ERROR, "%s: sscanf_s failed, sscanf_cnt = %d", __FUNCTION__, sscanf_cnt);
                ret = RET_ERR;
                break;
            }

            if (is_simulation == is_simulation_tmp) {
                *event_number = event_number_tmp;
                break;
            }
        }
    }
    if (event_record_str != NULL) {
        g_strfreev(event_record_str);
    }
    return ret;
}


LOCAL gint32 fill_event_id(json_object *evt_msg_jso, guint32 event_number)
{
    gint32 ret;
    gchar event_id_buf[MAX_STRBUF_LEN] = {0};
    json_object *str_jso = NULL;

    ret = snprintf_s(event_id_buf, MAX_STRBUF_LEN, MAX_STRBUF_LEN - 1, "0x%08X", event_number);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: format event id failed, ret is %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    str_jso = json_object_new_string((const char *)event_id_buf);
    json_object_object_add(evt_msg_jso, RFPROP_EVT_ID, str_jso);
    return RET_OK;
}

LOCAL gint32 find_update_event_record(RF_EVT_MSG_INFO *evt_info, gboolean is_simulation,
    guint32 *new_event_record_id)
{
    gint32 ret;
    guint32 event_number = INVALID_EVENT_RECORD_ID;

    if (evt_info == NULL) {
        debug_log(DLOG_ERROR, "evt_info is NULL");
        return RET_ERR;
    }
    if (new_event_record_id == NULL) {
        debug_log(DLOG_ERROR, "new_event_record_id is NULL");
        return RET_ERR;
    }
    ret = find_event_record(evt_info, is_simulation, &event_number);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "find_event_number failed, ret is %d", ret);
        return ret;
    }

    if (event_number == INVALID_EVENT_RECORD_ID) {
        // 找不到，重新分配一个事件编号
        ret = generate_new_event_record_id(&event_number);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "resign_current_event_number failed, ret is %d", ret);
            return ret;
        }

        if (evt_info->evt_state == EVENT_OBJ_STATE_ASSERT) {
            ret = update_event_record(evt_info, is_simulation, event_number, E_METHOD_POST);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "update_event_record failed, ret is %d", ret);
                return ret;
            }
        }
    } else if (evt_info->evt_state == EVENT_OBJ_STATE_DEASSERT) {
        ret = update_event_record(evt_info, is_simulation, event_number, E_METHOD_DELETE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "update_event_record failed, ret is %d", ret);
            return ret;
        }
    }
    *new_event_record_id = event_number;
    debug_log(DLOG_DEBUG, "event number: %u", event_number);

    return RET_OK;
}

gint32 replace_event_id_customized_by_cmcc_cucc(RF_EVT_MSG_INFO *evt_info, json_object *evt_msg_jso)
{
    gint32 ret;
    guint32 event_number = INVALID_EVENT_RECORD_ID;
    gboolean is_simulation = FALSE;
    guchar mode;
    guint8 status = STANDBY_STATE;

    if (dal_check_if_mm_board() == TRUE) {
        (void)get_board_active_state(&status);
        if (status == STANDBY_STATE) {
            return RET_OK;
        }
    }
    if (strcmp(evt_info->event_type, RF_ALERT_EVT_STR) != 0) {
        return RET_OK;
    }

    mode = (evt_info->evt_origin_type) & (EVENT_MONITOR_MODE_LOW_BIT_MASK);
    if (mode == EVENT_MONITOR_MODE_MANUAL) {
        is_simulation = TRUE;
    }

    // 正常级别告警，直接分配一个新的事件编号
    if (evt_info->severity == EVENT_SEVERITY_NORMAL) {
        (void)pthread_mutex_lock(&g_cmcc_event_records_mutex);
        ret = generate_new_event_record_id(&event_number);
        (void)pthread_mutex_unlock(&g_cmcc_event_records_mutex);
        if (ret != RET_OK) {
            return ret;
        }
        goto QUIT_OK;
    }
    (void)pthread_mutex_lock(&g_cmcc_event_records_mutex);
    ret = find_update_event_record(evt_info, is_simulation, &event_number);
    (void)pthread_mutex_unlock(&g_cmcc_event_records_mutex);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "find_update_event_record failed, ret is %d", ret);
        return ret;
    }
    debug_log(DLOG_DEBUG, "%s: event number: %u", __FUNCTION__, event_number);
QUIT_OK:
    ret = fill_event_id(evt_msg_jso, event_number);
    return ret;
}

LOCAL void custom_field_fill_null_cucc(json_object *hw_jso)
{
    json_object_object_add(hw_jso, RFPROP_SEPCIFIC_PROBLEM_ID, NULL);
    json_object_object_add(hw_jso, RFPROP_SEPCIFIC_PROBLEM, NULL);
    json_object_object_add(hw_jso, RFPROP_ALARM_STATUS, NULL);
    json_object_object_add(hw_jso, RFPROP_LOCATION_INFO, NULL);
    return;
}
 

gint32 get_event_detail_customized_by_cucc(RF_EVT_MSG_INFO *evt_info, json_object *hw_jso)
{
    gint32 ret;
    gchar event_id_buf[MAX_STRBUF_LEN] = {0};
 
    
    if (strcmp(evt_info->event_type, RF_ALERT_EVT_STR) != 0) {
        custom_field_fill_null_cucc(hw_jso);
        return RET_OK;
    }

    ret = snprintf_s(event_id_buf, MAX_STRBUF_LEN, MAX_STRBUF_LEN - 1, "0x%08X", evt_info->evt_id);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: format event id failed, ret is %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    ret = add_str_to_jso(hw_jso, (const gchar *)(event_id_buf), RFPROP_SEPCIFIC_PROBLEM_ID);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: add_str_to_jso %s fail, ret is %d", __FUNCTION__, RFPROP_SEPCIFIC_PROBLEM_ID, ret);
        return RET_ERR;
    }
 
    ret = add_str_to_jso(hw_jso, (const gchar *)(evt_info->msg_id), RFPROP_SEPCIFIC_PROBLEM);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: add_str_to_jso %s fail, ret is %d", __FUNCTION__, RFPROP_SEPCIFIC_PROBLEM, ret);
        return RET_ERR;
    }
 
    json_object_object_add(hw_jso, RFPROP_ALARM_STATUS, json_object_new_int(evt_info->evt_state));
 
    ret = add_str_to_jso(hw_jso, (const gchar *)(evt_info->subject_location), RFPROP_LOCATION_INFO);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: add_str_to_jso %s fail, ret is %d", __FUNCTION__, RFPROP_LOCATION_INFO, ret);
        return RET_ERR;
    }
    
    return RET_OK;
}
