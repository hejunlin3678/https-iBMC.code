
#include "redfish_provider.h"
#include "redfish_util.h"

LOCAL gint32 set_data_acquisition_service_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_data_acquisition_service_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 act_da_svc_export(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 export_table_status_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj);
LOCAL gint32 act_da_svc_clear(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_data_acquisition_report_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 act_da_data_filtering(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);

LOCAL gint32 get_data_acquisition_service_health(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_dataacquisition_provider[] = {
    {PROPERTY_DA_SERVICE_ENABLE, CLASS_DA_SERVICE, PROPERTY_DA_SVC_ENABLE, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_data_acquisition_service_enabled, set_data_acquisition_service_enabled, NULL,  ETAG_FLAG_ENABLE},

    {PROP_DA_SERVICE_HEALTH, MAP_PROPERTY_NULL, RF_VALUE_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_data_acquisition_service_health, NULL, NULL, ETAG_FLAG_ENABLE},

    {RF_DA_ACQ_REPORT, RF_VALUE_NULL, RF_VALUE_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_data_acquisition_report_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},

    {RFACTION_EXPORT_TABLE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_DIAGNOSEMGNT, SYS_LOCKDOWN_ALLOW, NULL, NULL, act_da_svc_export,  ETAG_FLAG_ENABLE},

    {RFACTION_CLEAR_TABLE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_DIAGNOSEMGNT, SYS_LOCKDOWN_FORBID, NULL, NULL, act_da_svc_clear,  ETAG_FLAG_ENABLE},
    
    {RFACTION_DATA_FILTERING, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_DIAGNOSEMGNT, SYS_LOCKDOWN_FORBID, NULL, NULL, act_da_data_filtering,  ETAG_FLAG_ENABLE}
    
};


LOCAL gint32 get_data_acquisition_report_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_object();
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    json_object_object_add(*o_result_jso, RFPROP_ODATA_ID, json_object_new_string(RF_DA_ACQ_REPORT_URI));
    return HTTP_OK;
}


LOCAL gint32 clear_table_status_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    gint32 ret;
    OBJ_HANDLE clear_handle;
    guint8 clear_state = CLEAR_RUNNING;

    return_val_do_info_if_fail((NULL != origin_obj_path) && (NULL != monitor_fn_data) && (NULL != message_obj),
        RF_FAILED, debug_log(DLOG_ERROR, "input param error"));

    // 调用获取对应内存的方法
    ret = dal_get_object_handle_nth(CLASS_DA_SERVICE, 0, &clear_handle);
    goto_label_do_info_if_fail(VOS_OK == ret, exception_exit,
        debug_log(DLOG_ERROR, "cann't get objhandle for %s", OBJ_NAME_DASERVICE));

    ret = dal_get_property_value_byte(clear_handle, PROPERTY_DA_SVC_DB_DATAPOINT_CLEAR_STATE, &clear_state);
    goto_label_do_info_if_fail(VOS_OK == ret, exception_exit,
        debug_log(DLOG_ERROR, "cann't get property  for %s", PROPERTY_DA_SVC_DB_DATAPOINT_CLEAR_STATE));

    // 根据内存值设置monitor_fn_data
    switch (clear_state) {
        case CLEAR_NOT_START:
            monitor_fn_data->task_state = RF_TASK_RUNNING;

            break;

        case CLEAR_RUNNING:
            monitor_fn_data->task_state = RF_TASK_RUNNING;
            break;

        case CLEAR_SUCCEED:
            // 设置task_monitor_info
            monitor_fn_data->task_state = RF_TASK_COMPLETED;
            monitor_fn_data->task_progress = RF_FINISH_PERCENTAGE;
            (void)create_message_info(MSGID_SUCCESS, NULL, message_obj);
            break;

        default:
            monitor_fn_data->task_state = RF_TASK_EXCEPTION;
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);
            break;
    }

    return RF_OK;

exception_exit:
    monitor_fn_data->task_progress = TASK_NO_PROGRESS;
    monitor_fn_data->task_state = RF_TASK_EXCEPTION;
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);
    return RF_OK;
}


LOCAL gint32 act_da_svc_clear(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret = RET_ERR;
    OBJ_HANDLE export_handle = 0;
    TASK_MONITOR_INFO_S *monitor_info = NULL;
    guint8 clear_state = G_MAXUINT8;

    goto_label_do_info_if_fail((NULL != i_paras) && (NULL != o_message_jso) && (NULL != o_result_jso) &&
        (NULL != i_paras->val_jso),
        err_exit, debug_log(DLOG_ERROR, "input param error."));

    if (0 == (i_paras->user_role_privilege & USERROLE_DIAGNOSEMGNT)) {
        debug_log(DLOG_ERROR, "%s failed:insufficient privilege", __FUNCTION__);

        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, RFACTION_CLEAR_TABLE, o_message_jso);

        return HTTP_FORBIDDEN;
    }

    
    //   //调用获取对应内存的方法，如果内存值为1就返回失败

    ret = dal_get_object_handle_nth(CLASS_DA_SERVICE, 0, &export_handle);
    goto_label_do_info_if_fail(VOS_OK == ret, err_exit,
        debug_log(DLOG_ERROR, "cann't get objhandle for %s", OBJ_NAME_DASERVICE));

    (void)dal_get_property_value_byte(export_handle, PROPERTY_DA_SVC_DB_DATAPOINT_CLEAR_STATE, &clear_state);

    if (clear_state == CLEAR_RUNNING) {
        (void)create_message_info(MSGID_LAST_TASK_NOT_OVER, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, export_handle,
        CLASS_DA_SERVICE, METHOD_CLEAR_TABLE, AUTH_DISABLE, AUTH_PRIV_NONE, NULL, NULL);
    goto_label_do_info_if_fail((VOS_OK == ret), err_exit,
        debug_log(DLOG_ERROR, "dfl_call_class_method %s.%s. Ret is %d\n", CLASS_DA_SERVICE, METHOD_CLEAR_TABLE, ret));
    

    monitor_info = task_monitor_info_new(NULL);

    goto_label_do_info_if_fail((NULL != monitor_info), err_exit,
        debug_log(DLOG_ERROR, "task_monitor_info_new failed "));

    monitor_info->task_progress = TASK_NO_PROGRESS;
    monitor_info->rsc_privilege = USERROLE_DIAGNOSEMGNT;

    ret = create_new_task(RF_CLEAR_TABLE_TASK_DESC, clear_table_status_monitor, monitor_info, i_paras->val_jso,
        i_paras->uri, o_result_jso);
    goto_label_do_info_if_fail(RF_OK == ret, err_exit, task_monitor_info_free(monitor_info);
        debug_log(DLOG_ERROR, "create new task failed"));

    return HTTP_ACCEPTED;

err_exit:
    if (ret == ERRCODE_ROUTE_NOT_FOUND) {
        ret = HTTP_BAD_REQUEST;
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    }  else if (ret == VOS_ERR_OBJ_UNAVAILABLE) {
        ret = HTTP_BAD_REQUEST;
        (void)create_message_info(MSGID_SERVICE_RESTART_IN_PROGRESS, NULL, o_message_jso);
    } else {
        ret = HTTP_INTERNAL_SERVER_ERROR;
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    }
    return ret;
}


LOCAL void export_csv_info_free_func(void *user_data)
{
    int iRet;
    gchar *file_name = (gchar *)user_data;
    gchar file_path[MAX_FILEPATH_LENGTH] = {0};
    iRet =
        snprintf_s(file_path, sizeof(file_path), sizeof(file_path) - 1, "%s/%s.csv", DA_SERVICE_FILE_PATH, file_name);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    // 删除导出的csv文件
    (void)proxy_delete_file(0, NULL, NULL, file_path);

    g_free(user_data);

    return;
}

LOCAL gint32 check_param_and_role_validity(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    if (o_message_jso == NULL || o_result_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if ((i_paras->user_role_privilege & USERROLE_DIAGNOSEMGNT) == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, RFACTION_EXPORT_TABLE, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    return RET_OK;
}

LOCAL gint32 check_da_svc_export_status(OBJ_HANDLE export_handle, const gchar *datasource_state_name,
    json_object **o_message_jso)
{
    gint32 ret;
    guint8 export_state = G_MAXUINT8;

    if (g_strcmp0(datasource_state_name, PROPERTY_DA_SVC_DB_TAG_EXPORT_STATE) == 0 ||
        g_strcmp0(datasource_state_name, PROPERTY_DA_SVC_DB_METRIC_EXPORT_STATE) == 0) {
        ret = dal_get_property_value_byte(export_handle, PROPERTY_DA_SVC_DB_ITEM_REPAIR_STATE, &export_state);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "cann't get property value for %s, ret = %d", PROPERTY_DA_SVC_DB_ITEM_REPAIR_STATE,
                ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
        }

        if (export_state != 0) {
            (void)create_message_info("Item db repairing", NULL, o_message_jso);
            return HTTP_SERVICE_UNAVAILABLE;
        }
    }

    ret = dal_get_property_value_byte(export_handle, datasource_state_name, &export_state);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "cann't get property value for %s", datasource_state_name);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    debug_log(DLOG_DEBUG, "last task name= %s,state=%d", datasource_state_name, export_state);

    if (export_state == EXPORT_RUNNING) {
        (void)create_message_info(MSGID_LAST_TASK_NOT_OVER, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }
    return RET_OK;
}

LOCAL gint32 get_da_svc_datasource_name(json_object *body_jso, const gchar **datasource_state_name)
{
    const gchar *data_source = NULL;
    (void)get_element_str(body_jso, RFACTION_PARAM_DATASOURCE, &data_source);
    if (data_source == NULL) {
        debug_log(DLOG_ERROR, "can't get data_source ");
        return RET_ERR;
    }

    if (g_strcmp0(data_source, DA_SERVICE_METRIC_TABLE) == 0) {
        *datasource_state_name = PROPERTY_DA_SVC_DB_METRIC_EXPORT_STATE;
    } else if (g_strcmp0(data_source, DA_SERVICE_TAG_TABLE) == 0) {
        *datasource_state_name = PROPERTY_DA_SVC_DB_TAG_EXPORT_STATE;
    } else if (g_strcmp0(data_source, DA_SERVICE_DATAPOINT_TABLE) == 0) {
        *datasource_state_name = PROPERTY_DA_SVC_DB_DATAPOINT_EXPORT_STATE;
    } else {
        debug_log(DLOG_ERROR, "datasource_state_name is illegal ");
        return RET_ERR;
    }
    return RET_OK;
}

LOCAL void make_da_svc_err_message_info(gint32 *ret_val, json_object **o_message_jso)
{
    if ((*ret_val == VOS_ERR_NOT_ENOUGH_MEM) || (*ret_val == ERRCODE_ROUTE_NOT_FOUND)) {
        *ret_val = HTTP_BAD_REQUEST;
        (void)create_message_info(MSGID_OPERATION_FAILED, NULL, o_message_jso);
    } else if (*ret_val == VOS_ERR_OBJ_UNAVAILABLE) {
        *ret_val = HTTP_BAD_REQUEST;
        (void)create_message_info(MSGID_SERVICE_RESTART_IN_PROGRESS, NULL, o_message_jso);
    } else {
        *ret_val = HTTP_INTERNAL_SERVER_ERROR;
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    }
}


LOCAL gint32 act_da_svc_export(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    OBJ_HANDLE export_handle;
    const gchar *datasource_state_name = NULL;

    gint32 ret = check_param_and_role_validity(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        return ret;
    }

    if (get_da_svc_datasource_name(i_paras->val_jso, &datasource_state_name) != RET_OK) {
        goto err_exit;
    }

    ret = dal_get_object_handle_nth(CLASS_DA_SERVICE, 0, &export_handle);
    goto_label_do_info_if_fail(ret == DFL_OK, err_exit,
        debug_log(DLOG_ERROR, "cann't get object handle for %s", OBJ_NAME_DASERVICE));

    ret = check_da_svc_export_status(export_handle, datasource_state_name, o_message_jso);
    if (ret != RET_OK) {
        return ret;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(datasource_state_name));
    input_list = g_slist_append(input_list, g_variant_new_byte(i_paras->auth_type));
    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, export_handle,
        CLASS_DA_SERVICE, METHOD_EXPORT_TABLE, AUTH_DISABLE, AUTH_PRIV_NONE, input_list, &output_list);
    goto_label_do_info_if_fail((ret == VOS_OK), err_exit,
        debug_log(DLOG_ERROR, "dfl_call_class_method %s.%s. Ret is %d\n", CLASS_DA_SERVICE, METHOD_EXPORT_TABLE, ret);
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref));
    uip_free_gvariant_list(input_list);

    const gchar *file_name = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    

    TASK_MONITOR_INFO_S *monitor_info = task_monitor_info_new((GDestroyNotify)export_csv_info_free_func);
    goto_label_do_info_if_fail((monitor_info != NULL), err_exit, debug_log(DLOG_ERROR, "task_monitor_info_new failed ");
        uip_free_gvariant_list(output_list));
    monitor_info->user_data = g_strdup(file_name);
    goto_label_do_info_if_fail((monitor_info->user_data != NULL), err_exit, task_monitor_info_free(monitor_info);
        debug_log(DLOG_ERROR, "dup string for %s failed", file_name); uip_free_gvariant_list(output_list));
    uip_free_gvariant_list(output_list);
    monitor_info->task_progress = TASK_NO_PROGRESS; 
    monitor_info->rsc_privilege = USERROLE_DIAGNOSEMGNT;
    
    ret = create_new_task(RF_EXPORT_CSV_TASK_DESC, export_table_status_monitor, monitor_info, i_paras->val_jso,
        i_paras->uri, o_result_jso);
    goto_label_do_info_if_fail(ret == RF_OK, err_exit, task_monitor_info_free(monitor_info);
        debug_log(DLOG_ERROR, "create new task failed"));

    return HTTP_ACCEPTED;

err_exit:
    make_da_svc_err_message_info(&ret, o_message_jso);
    return ret;
}


LOCAL gint32 export_table_status_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
#define MAX_ERROR_MESSAGE_SIZE 128

    errno_t safe_fun_ret = EOK;
    const gchar *data_source = NULL;
    const gchar *datasource_state_name = NULL;
    OBJ_HANDLE export_handle;
    guint8 export_state = EXPORT_RUNNING;
    gchar export_error_message[MAX_ERROR_MESSAGE_SIZE] = {0};
    return_val_do_info_if_fail((NULL != origin_obj_path) && (NULL != monitor_fn_data) && (NULL != message_obj),
        RF_FAILED, debug_log(DLOG_ERROR, "input param error"));

    // 调用获取对应内存的方法

    (void)get_element_str(body_jso, RFACTION_PARAM_DATASOURCE, &data_source);

    goto_label_do_info_if_fail((NULL != data_source), exit, debug_log(DLOG_ERROR, "can't get data_source "));

    
    //   //调用获取对应内存的方法，如果内存值为1就返回失败
    if (0 == g_strcmp0(data_source, DA_SERVICE_METRIC_TABLE)) {
        datasource_state_name = PROPERTY_DA_SVC_DB_METRIC_EXPORT_STATE;
    } else if (0 == g_strcmp0(data_source, DA_SERVICE_TAG_TABLE)) {
        datasource_state_name = PROPERTY_DA_SVC_DB_TAG_EXPORT_STATE;
    } else if (0 == g_strcmp0(data_source, DA_SERVICE_DATAPOINT_TABLE)) {
        datasource_state_name = PROPERTY_DA_SVC_DB_DATAPOINT_EXPORT_STATE;
    } else {
        debug_log(DLOG_ERROR, "datasource_state_name is illegal ");
        goto exception_exit;
    }

    gint32 ret = dal_get_object_handle_nth(CLASS_DA_SERVICE, 0, &export_handle);
    goto_label_do_info_if_fail(VOS_OK == ret, exception_exit,
        debug_log(DLOG_ERROR, "cann't get objhandle for %s", OBJ_NAME_DASERVICE));

    (void)dal_get_property_value_byte(export_handle, datasource_state_name, &export_state);

    debug_log(DLOG_ERROR, "monitor:datasource name= %s,state=%d", datasource_state_name, export_state);

    // 根据内存值设置monitor_fn_data
    switch (export_state) {
        case EXPORT_NOT_START:
            monitor_fn_data->task_state = RF_TASK_RUNNING;
            break;

        case EXPORT_RUNNING:
            monitor_fn_data->task_state = RF_TASK_RUNNING;
            break;

        case EXPORT_SUCCEED:
            // 设置task_monitor_info
            monitor_fn_data->task_state = RF_TASK_COMPLETED;
            monitor_fn_data->task_progress = RF_FINISH_PERCENTAGE;
            monitor_fn_data->response_header = json_object_new_object();
            json_object_object_add(monitor_fn_data->response_header, REQ_HEADER_CONTENT_TYPE,
                json_object_new_string(RF_CONTENT_TYPE_OCTET_STREAM));
            json_object_object_add(monitor_fn_data->response_header, REQ_HEADER_CONTENT_DISPOSITION,
                json_object_new_string((const gchar *)monitor_fn_data->user_data));
            (void)create_message_info(MSGID_SUCCESS, NULL, message_obj);
            break;

        case EXPORT_FAILED:
            monitor_fn_data->task_state = RF_TASK_EXCEPTION;
            safe_fun_ret = strncat_s(export_error_message, MAX_ERROR_MESSAGE_SIZE, DA_SERVICE_EXPORT_ERROR,
                strlen(DA_SERVICE_EXPORT_ERROR));
            do_if_expr(safe_fun_ret != EOK,
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
            (void)create_message_info(MSGID_EXPORT_TABLE_FAIL, NULL, message_obj, (gchar *)export_error_message);
            break;

        default:
            monitor_fn_data->task_state = RF_TASK_EXCEPTION;
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);
            break;
    }

    return RF_OK;

exception_exit:
    monitor_fn_data->task_state = RF_TASK_EXCEPTION;
    monitor_fn_data->task_progress = TASK_NO_PROGRESS;
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);
exit:
    return RF_OK;
}

LOCAL gint32 check_filter_parameter(json_object *filter_jso, GSList **input_list)
{
    gint32 ret;
    json_object *start_time_jso = NULL;
    json_object *end_time_jso = NULL;
    json_object *comp_type_jso = NULL;
    json_object *metric_type_jso = NULL;
    json_object *metric_item_jso = NULL;
    gint32 metric_item_num = 0;
    gint32 metric_item_count = 0;
    gint32 start_time = 0;
    gint32 end_time = 0;
    gint32 comp_type = INVALID_DATA_BYTE;
    guint32 metric_type = INVALID_DATA_BYTE;
    guint32 metric_bit = 1;
    const gchar *type = NULL;
    guint32 i_index = 0;
    gchar*          metric_type_map[] = {DA_REPORT_METRIC_TYPE_VOL, DA_REPORT_METRIC_TYPE_TEMP, DA_REPORT_METRIC_TYPE_PERFORMANCE,
                                         DA_REPORT_METRIC_TYPE_HEALTH, DA_REPORT_METRIC_TYPE_CURRENT, DA_REPORT_METRIC_TYPE_LOG, DA_REPORT_METRIC_TYPE_SMART};
    COMP_TYPE_MAP   component_map[] = {{DA_REPORT_COMPONENT_CPU, COMPONENT_TYPE_CPU},
                                       {DA_REPORT_COMPONENT_MEM, COMPONENT_TYPE_MEMORY},
                                       {DA_REPORT_COMPONENT_DISK, COMPONENT_TYPE_HARDDISK},
                                       {DA_REPORT_COMPONENT_MAIN_BOARD, COMPONENT_TYPE_MAINBOARD},
                                       {DA_REPORT_COMPONENT_PSU, COMPONENT_TYPE_PS},
                                       {DA_REPORT_COMPONENT_CHASSIS, COMPONENT_TYPE_CHASSIS},
                                       {DA_REPORT_COMPONENT_SYSTEM, COMPONENT_TYPE_SYSTEM},
                                       {DA_REPORT_COMPONENT_FAN, COMPONENT_TYPE_FAN}};

    return_val_do_info_if_fail(NULL != input_list && NULL != filter_jso, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    ret = json_object_object_get_ex(filter_jso, RF_PROPERTY_STARTTIME, &start_time_jso);
    if (ret && NULL != start_time_jso) {
        start_time = json_object_get_int(start_time_jso);
    }

    ret = json_object_object_get_ex(filter_jso, RF_PROPERTY_ENDTIME, &end_time_jso);
    if (ret && NULL != end_time_jso) {
        end_time = json_object_get_int(end_time_jso);
    }

    ret = json_object_object_get_ex(filter_jso, RF_PROPERTY_COMP_TYPE, &comp_type_jso);
    if (ret) {
        
        comp_type = 0xFE;
        if (NULL != comp_type_jso) {
            type = dal_json_object_get_str(comp_type_jso);
            for (i_index = 0; i_index < G_N_ELEMENTS(component_map); i_index++) {
                break_do_info_if_expr(0 == g_ascii_strcasecmp(type, component_map[i_index].type_str),
                    (comp_type = component_map[i_index].type_num));
            }
        }
    }

    ret = json_object_object_get_ex(filter_jso, RF_PROPERTY_METRIC_TYPE, &metric_type_jso);
    if (ret) {
        metric_type = INVALIDTYPE;
        if (NULL != metric_type_jso) {
            metric_item_num = json_object_array_length(metric_type_jso);
            for (metric_item_count = 0; metric_item_count < metric_item_num; metric_item_count++) {
                metric_item_jso = json_object_array_get_idx(metric_type_jso, metric_item_count);
                type = dal_json_object_get_str(metric_item_jso);
                for (i_index = 0; i_index < G_N_ELEMENTS(metric_type_map); i_index++) {
                    break_do_info_if_expr(0 == g_ascii_strcasecmp(type, metric_type_map[i_index]),
                        (metric_type |= metric_bit << i_index));
                }
            }
        }
    }

    *input_list = g_slist_append(*input_list, (gpointer)g_variant_new_byte(comp_type));
    *input_list = g_slist_append(*input_list, (gpointer)g_variant_new_byte(metric_type));
    *input_list = g_slist_append(*input_list, (gpointer)g_variant_new_uint32(start_time));
    *input_list = g_slist_append(*input_list, (gpointer)g_variant_new_uint32(end_time));

    return VOS_OK;
}

LOCAL void *data_filtering_func(void *data)
{
    gint32 index_i;
    gint32 item_num;
    gint32 index_j = 0;
    gint32 result_item_num = 0;
    gint32 ret;
    json_object *item_jso = NULL;
    json_object *item_result_jso = NULL;
    json_object *result_array_jso = NULL;
    json_object *result_data_jso = NULL;
    const gchar *result_str = NULL;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    DATA_FILTERINF_NEED_INFO *data_filter_info = NULL;
    gchar file_name[MAX_FILEPATH_LENGTH] = {0};
    OBJ_HANDLE obj_handle = 0;

    (void)prctl(PR_SET_NAME, (uintptr_t) "dataFiltering");
    return_val_do_info_if_fail(NULL != data, NULL, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    data_filter_info = (DATA_FILTERINF_NEED_INFO *)data;
    
    ret = snprintf_s(file_name, MAX_FILEPATH_LENGTH, MAX_FILEPATH_LENGTH - 1, "datafilteringresult_%lu.json",
        vos_tick_get());
    goto_label_do_info_if_fail((0 < ret), exit,
        debug_log(DLOG_ERROR, "%s: Failed to generate the file name.", __FUNCTION__));
    ret =
        snprintf_s(data_filter_info->file_name, MAX_FILEPATH_LENGTH, MAX_FILEPATH_LENGTH - 1, "/dev/shm/%s", file_name);
    goto_label_do_info_if_fail((0 < ret), exit,
        debug_log(DLOG_ERROR, "%s: Failed to combine the file name.", __FUNCTION__));
    result_array_jso = json_object_new_array();

    item_num = json_object_array_length(data_filter_info->body_jso);
    for (index_i = 0; index_i < item_num; index_i++) {
        item_jso = json_object_array_get_idx(data_filter_info->body_jso, index_i);

        ret = check_filter_parameter(item_jso, &input_list);
        goto_label_do_info_if_fail((VOS_OK == ret), exit,
            debug_log(DLOG_ERROR, "%s: check_filter_criteria failed.", __FUNCTION__));

        
        ret = dal_get_object_handle_nth(CLASS_DA_SERVICE, 0, &obj_handle);
        goto_label_do_info_if_fail((VOS_OK == ret), exit, uip_free_gvariant_list(input_list);
            debug_log(DLOG_ERROR, "%s:find object fail.", __FUNCTION__));
        ret = uip_call_class_method_redfish(data_filter_info->is_from_webui, data_filter_info->user_name,
            data_filter_info->client, data_filter_info->obj_handle, CLASS_DA_SERVICE, METHOD_GET_TABLE_INFO,
            AUTH_ENABLE, data_filter_info->user_role_privilege, input_list, &output_list);
        uip_free_gvariant_list(input_list);
        input_list = NULL;
        continue_do_info_if_fail(VOS_OK == ret, uip_free_gvariant_list(output_list); (output_list = NULL);
            debug_log(DLOG_MASS, "%s: call method %s failed, ret is %d", __FUNCTION__, METHOD_GET_TABLE_INFO, ret));

        result_str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
        item_result_jso = json_tokener_parse(result_str);

        
        result_item_num = json_object_array_length(item_result_jso);
        for (index_j = 0; index_j < result_item_num; index_j++) {
            ret = json_object_array_add(result_array_jso,
                json_object_get(json_object_array_get_idx(item_result_jso, index_j)));
            goto_label_do_info_if_fail((VOS_OK == ret), exit, uip_free_gvariant_list(output_list);
                json_object_put(item_result_jso); debug_log(DLOG_ERROR, "%s:find object fail.", __FUNCTION__));
        }

        json_object_put(item_result_jso);
        uip_free_gvariant_list(output_list);
        output_list = NULL;
        item_result_jso = NULL;
        data_filter_info->filter_process = (gint32)(((float)index_i / (float)item_num) * 100);
    }

    result_data_jso = json_object_new_object();
    if (0 == json_object_array_length(result_array_jso)) {
        json_object_object_add(result_data_jso, PROPERTY_DA_REPORT_DATA, NULL);
        json_object_put(result_array_jso);
    } else {
        json_object_object_add(result_data_jso, PROPERTY_DA_REPORT_DATA, result_array_jso);
    }
    data_filter_info->filter_status = RF_TASK_COMPLETED;
    json_object_to_file(data_filter_info->file_name, result_data_jso);

    
    (void)dal_set_file_owner(data_filter_info->file_name, REDFISH_USER_UID, APPS_USER_GID);
    (void)dal_set_file_mode(data_filter_info->file_name, S_IRUSR | S_IRUSR | S_IRGRP | S_IWGRP);

    json_object_put(result_data_jso);
    return NULL;

exit:
    do_info_if_true(NULL != result_array_jso, json_object_put(result_array_jso));
    data_filter_info->filter_status = RF_TASK_EXCEPTION;
    return NULL;
}

LOCAL gint32 data_filter_status_monitor(const char *obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    DATA_FILTERINF_NEED_INFO *data_filter_info = (DATA_FILTERINF_NEED_INFO *)monitor_fn_data->user_data;

    switch (data_filter_info->filter_status) {
        case RF_TASK_RUNNING:
            monitor_fn_data->task_state = RF_TASK_RUNNING;
            monitor_fn_data->task_progress = data_filter_info->filter_process;
            break;

        case RF_TASK_COMPLETED:
            // 设置task_monitor_info
            monitor_fn_data->task_state = RF_TASK_COMPLETED;
            monitor_fn_data->task_progress = 100;
            monitor_fn_data->extended_info = json_object_new_object();
            json_object_object_add(monitor_fn_data->extended_info, RF_MONITOR_EXTENDED_FILENAME,
                json_object_new_string(data_filter_info->file_name));
            (void)create_message_info(MSGID_SUCCESS, NULL, message_obj);
            break;

        default:
            monitor_fn_data->task_state = RF_TASK_EXCEPTION;
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);
            break;
    }

    return RF_OK;
}

LOCAL void data_filter_free_func(void *user_data)
{
    DATA_FILTERINF_NEED_INFO *data_filter_info = (DATA_FILTERINF_NEED_INFO *)user_data;

    (void)vos_rm_filepath(data_filter_info->file_name);
    json_object_put(data_filter_info->body_jso);
    g_free(user_data);
    data_filter_info = NULL;
    return;
}

LOCAL gint32 act_da_data_filtering(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    json_object *body_jso = NULL;
    gulong task_id = 0;
    DATA_FILTERINF_NEED_INFO *data_filter_info = NULL;

    goto_label_do_info_if_fail((NULL != i_paras) && (NULL != o_message_jso) && (NULL != o_result_jso) &&
        (NULL != i_paras->val_jso),
        err_exit, debug_log(DLOG_ERROR, "input param error."));

    if ((i_paras->user_role_privilege & USERROLE_READONLY) == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    return_val_do_info_if_fail(TRUE ==
        json_object_object_get_ex(i_paras->val_jso, RF_PROPERTY_ACQUISITION_ITEMS, &body_jso),
        HTTP_BAD_REQUEST, debug_log(DLOG_DEBUG, "%s: Get AcquisitionItems object failed.", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_MISSING, NULL, o_message_jso, RF_PROPERTY_ACQUISITION_ITEMS));

    data_filter_info = (DATA_FILTERINF_NEED_INFO *)g_malloc0(sizeof(DATA_FILTERINF_NEED_INFO));
    goto_label_do_info_if_fail((NULL != data_filter_info), err_exit,
        debug_log(DLOG_DEBUG, "%s: g_malloc0 error.", __FUNCTION__));

    gint32 ret = strcpy_s(data_filter_info->user_name, SESSION_USER_NAME_MAX_LEN + 1, i_paras->user_name);
    goto_label_do_info_if_fail((0 == ret), err_exit, debug_log(DLOG_ERROR, "%s: strncpy_s error.", __FUNCTION__));

    ret = strncpy_s(data_filter_info->client, SESSION_IP_LEN, i_paras->client, strlen(i_paras->client));
    goto_label_do_info_if_fail((ret == 0), err_exit, debug_log(DLOG_ERROR, "%s: strncpy_s error.", __FUNCTION__));

    data_filter_info->is_from_webui = i_paras->is_from_webui;
    data_filter_info->obj_handle = i_paras->obj_handle;
    data_filter_info->user_role_privilege = i_paras->user_role_privilege;
    data_filter_info->body_jso = json_object_get(body_jso);
    data_filter_info->filter_status = RF_TASK_RUNNING;

    TASK_MONITOR_INFO_S *monitor_info = task_monitor_info_new((GDestroyNotify)data_filter_free_func);

    goto_label_do_info_if_fail((NULL != monitor_info), err_exit,
        debug_log(DLOG_ERROR, "%s: task_monitor_info_new failed ", __FUNCTION__));

    monitor_info->user_data = data_filter_info;
    monitor_info->task_progress = TASK_NO_PROGRESS;
    monitor_info->rsc_privilege = USERROLE_READONLY;

    ret = create_new_task("Data Filtering Task", data_filter_status_monitor, monitor_info, NULL, i_paras->uri,
        o_result_jso);
    
    goto_label_do_info_if_fail(RF_OK == ret, err_exit, task_monitor_info_free(monitor_info); data_filter_info = NULL;
        debug_log(DLOG_ERROR, "%s: create new task failed", __FUNCTION__));

    ret = vos_task_create(&task_id, "data filtering thread", (TASK_ENTRY)data_filtering_func, data_filter_info,
        DEFAULT_PRIORITY);
    if (ret != RET_OK) {
        // filter_info是任务的user_data, 由redfish框架负责释放，为了避免double free，此处将filter_info置空
        data_filter_info = NULL;
        debug_log(DLOG_ERROR, "%s: vos_task_create failed.", __FUNCTION__);
        goto err_exit;
    }
    

    return HTTP_ACCEPTED;

err_exit:
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    do_info_if_true(NULL != data_filter_info,
        do_info_if_true(NULL != data_filter_info->body_jso, json_object_put(data_filter_info->body_jso));
        g_free(data_filter_info));

    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL gint32 set_data_acquisition_service_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guchar daservice_enabled;
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;

    if (0 == (i_paras->user_role_privilege & USERROLE_DIAGNOSEMGNT)) {
        debug_log(DLOG_ERROR, "%s failed:insufficient privilege", __FUNCTION__);

        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, PROPERTY_DA_SERVICE_ENABLE, o_message_jso,
            PROPERTY_DA_SERVICE_ENABLE);

        return HTTP_FORBIDDEN;
    }

    return_val_do_info_if_fail((NULL != i_paras->val_jso), HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "null type value no supported to config");
        (void)create_message_info(MSGID_PROP_TYPE_ERR, PROPERTY_DA_SERVICE_ENABLE, o_message_jso, RF_NULL_STR,
        PROPERTY_DA_SERVICE_ENABLE));

    daservice_enabled = json_object_get_boolean(i_paras->val_jso);
    return_val_do_info_if_expr((FALSE != daservice_enabled) && (TRUE != daservice_enabled), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "cann't get json object boolean");
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    ret = dal_get_object_handle_nth(CLASS_DA_SERVICE, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "cann't get object handle for %s", CLASS_DA_SERVICE);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    
    input_list = g_slist_append(input_list, g_variant_new_byte(daservice_enabled));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_DA_SERVICE, METHOD_SET_SERVICE_ENABLE, AUTH_DISABLE, AUTH_PRIV_NONE, input_list, NULL);
    

    uip_free_gvariant_list(input_list);

    
    switch (ret) {
        case VOS_OK:
            return HTTP_OK;

        
        case ERRCODE_METHOD_NOT_FOUND:
        case ERRCODE_ROUTE_NOT_FOUND:
            debug_log(DLOG_ERROR, "%s: Modify %s failed, ret=%d.", __FUNCTION__, PROPERTY_DA_SERVICE_ENABLE, ret);
            (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, PROPERTY_DA_SERVICE_ENABLE, o_message_jso,
                PROPERTY_DA_SERVICE_ENABLE);
            return HTTP_BAD_REQUEST;

        default:
            debug_log(DLOG_ERROR, "Set Data Acquisition service enabled failed, ret=%d.", ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
    
}


LOCAL gint32 get_data_acquisition_service_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guchar daservice_enabled = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (0 == (i_paras->user_role_privilege & USERROLE_READONLY)) {
        debug_log(DLOG_ERROR, "%s failed:insufficient privilege", __FUNCTION__);
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, PROPERTY_DA_SERVICE_ENABLE, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    ret = dal_get_object_handle_nth(CLASS_DA_SERVICE, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "cann't get object handle for %s", CLASS_DA_SERVICE);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_DA_SVC_ENABLE, &daservice_enabled);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get Data Acquisition service enabled fail.\n", __FUNCTION__, __LINE__));
    *o_result_jso = json_object_new_boolean(daservice_enabled);

    return HTTP_OK;
}


LOCAL void get_rack_data_acquisition_config(json_object *da_svc_jso)
{
    json_object *odata_id = NULL;
    json_object *rack_da_cfg_jso = NULL;

    return_do_info_if_fail(NULL != da_svc_jso, debug_log(DLOG_ERROR, "%s:param is NULL ", __FUNCTION__));

    return_do_info_if_fail(json_type_object == json_object_get_type(da_svc_jso),
        debug_log(DLOG_ERROR, "%s:param is not json type object", __FUNCTION__));

    rack_da_cfg_jso = json_object_new_object();
    return_do_info_if_fail(NULL != rack_da_cfg_jso,
        debug_log(DLOG_ERROR, "%s: json_object_new_object fail", __FUNCTION__));

    
    odata_id = json_object_new_string(URI_FORMAT_RACK_DA_CONFIG);
    return_do_info_if_fail(NULL != odata_id, (void)json_object_put(rack_da_cfg_jso);
        debug_log(DLOG_ERROR, "%s: json_object_new_string odata_id fail\n", __FUNCTION__));

    json_object_object_add(rack_da_cfg_jso, RFPROP_ODATA_ID, odata_id);

    json_object_object_add(da_svc_jso, PROP_RACK_DA_CONFIG, rack_da_cfg_jso);

    return;
}


LOCAL gint32 get_data_acquisition_service_health(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if ((i_paras->user_role_privilege & USERROLE_READONLY) == 0) {
        debug_log(DLOG_ERROR, "%s failed:insufficient privilege", __FUNCTION__);
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, PROP_DA_SERVICE_HEALTH, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    *o_result_jso = json_object_new_string(OK_STRING);
    return HTTP_OK;
}


gint32 get_data_acquisition_svc_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_msg_jso,
    gchar **o_rsp_body_str)
{
    gint32 ret;
    OBJ_HANDLE rack_mgnt_hnd;

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_rsc_jso) && (NULL != o_err_msg_jso), RF_FAILED,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    
    (void)provider_get_prop_values(i_paras, &o_rsc_jso, o_err_msg_jso, g_dataacquisition_provider,
        G_N_ELEMENTS(g_dataacquisition_provider));
    

    
    ret = dal_get_object_handle_nth(CLASS_RACK_MANAGEMENT, 0, &rack_mgnt_hnd);
    if (VOS_OK == ret) {
        get_rack_data_acquisition_config(o_rsc_jso);
    }

    return VOS_OK;
}


gint32 dataacquisition_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    guint8 data_acquire_support = 0;

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    
    
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROPERTY_PME_SERVICECONFIG_DATA_ACQUISITION_SUPPORT,
        &data_acquire_support);
    return_val_do_info_if_fail(1 == data_acquire_support, HTTP_NOT_FOUND,
        debug_log(DLOG_DEBUG, "%s : this resource not enabled by license", __FUNCTION__));
    

    *prop_provider = g_dataacquisition_provider;
    *count = sizeof(g_dataacquisition_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}
