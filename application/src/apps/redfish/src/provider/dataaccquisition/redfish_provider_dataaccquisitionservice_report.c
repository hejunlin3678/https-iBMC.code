
#include "redfish_provider.h"
#include "redfish_util.h"
#include "pme_app/pme_app.h"

#define MAX_METRIC_NAME_SIZE 100
#define MAX_METRIC_UNIT_SIZE 16
#define MAX_THRESHOLD_SIZE 10
#define MAX_VAL_SIZE 64
#define MAX_TAG_HASH_SIZE 9
#define MAX_ACQUISITION_VAL_LEN 16

LOCAL COMP_TYPE_MAP comp_type_array[] = {{DA_REPORT_COMPONENT_CPU, COMPONENT_TYPE_CPU},
                                         {DA_REPORT_COMPONENT_MEM, COMPONENT_TYPE_MEMORY},
                                         {DA_REPORT_COMPONENT_DISK, COMPONENT_TYPE_HARDDISK},
                                         {DA_REPORT_COMPONENT_PSU, COMPONENT_TYPE_PS},
                                         {DA_REPORT_COMPONENT_FAN, COMPONENT_TYPE_FAN},
                                         {DA_REPORT_COMPONENT_MAIN_BOARD, COMPONENT_TYPE_MAINBOARD},
                                         {DA_REPORT_COMPONENT_CHASSIS, COMPONENT_TYPE_CHASSIS},
                                         {DA_REPORT_COMPONENT_SYSTEM, COMPONENT_TYPE_SYSTEM},
                                         {NULL, INVALID_DATA_BYTE}
                                        };

LOCAL gchar* metric_type_array[] = {DA_REPORT_METRIC_TYPE_VOL,
                                    DA_REPORT_METRIC_TYPE_TEMP,
                                    DA_REPORT_METRIC_TYPE_PERFORMANCE,
                                    DA_REPORT_METRIC_TYPE_HEALTH,
                                    DA_REPORT_METRIC_TYPE_CURRENT,
                                    DA_REPORT_METRIC_TYPE_LOG,
                                    DA_REPORT_METRIC_TYPE_SMART,
                                    NULL
                                   };

LOCAL PROPERTY_PROVIDER_S g_dataacquisition_report_provider[] = {
};


LOCAL gint32 check_acquisition_report_time_valid(json_object *filter_jso, gint32* start_time, gint32* end_time)
{
    const gchar *time_str = NULL;
    json_object *start_time_jso = NULL;
    json_object *end_time_jso = NULL;
    json_bool ret_bool;
    gint32 ret;

    ret_bool = json_object_object_get_ex(filter_jso, PROPERTY_DA_REPORT_START_TIME, &start_time_jso);
    if (ret_bool == TRUE) {
        time_str = dal_json_object_get_str(start_time_jso);
        ret = vos_str2int(time_str, BASE_10, start_time, NUM_TPYE_INT32);
        if (ret != RET_OK || *start_time < 0) { 
            debug_log(DLOG_DEBUG, "%s: Start time is %d, check failed.", __FUNCTION__, *start_time);
            return RET_ERR;
        }
        time_str = NULL;
    }

    ret_bool = json_object_object_get_ex(filter_jso, PROPERTY_DA_REPORT_END_TIME, &end_time_jso);
    if (ret_bool == TRUE) {
        time_str = dal_json_object_get_str(end_time_jso);
        ret = vos_str2int(time_str, BASE_10, end_time, NUM_TPYE_INT32);
        if (ret != RET_OK || *end_time < 0) {
            debug_log(DLOG_DEBUG, "%s: End time is %s, check failed.", __FUNCTION__, time_str);
            return RET_ERR;
        }
        time_str = NULL;
    }
    if (start_time_jso != NULL && end_time_jso != NULL && *end_time < *start_time) {
        debug_log(DLOG_ERROR, "%s: The end time is earlier than the start time.", __FUNCTION__);
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 get_data_acquisition_report_filter(json_object *filter_jso, GSList **input_list)
{
    gint32 ret;
    OBJ_HANDLE rack_mgnt_hnd = 0;
    json_object *ser_num_jso = NULL;
    json_object *comp_type_jso = NULL;
    json_object *metric_type_jso = NULL;
    gint32 start_time = 0;
    gint32 end_time = 0;
    gint32 comp_type = INVALID_DATA_BYTE;
    guint32 metric_type = INVALID_DATA_BYTE;
    guint32 metric_bit = 1;
    const gchar *ser_num = NULL;
    const gchar *type = NULL;
    guint32 i_index = 0;
    json_bool ret_bool;

    // 用户不指定任何条件filter_jso为null
    return_val_do_info_if_fail(NULL != input_list, VOS_ERR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));
    ret = check_acquisition_report_time_valid(filter_jso, &start_time, &end_time);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    ret_bool = json_object_object_get_ex(filter_jso, PROPERTY_DA_REPORT_COMP_TYPE, &comp_type_jso);
    if (ret_bool) {
        type = dal_json_object_get_str(comp_type_jso);

        for (i_index = 0; comp_type_array[i_index].type_str; i_index++) {
            break_do_info_if_expr(0 == g_ascii_strcasecmp(type, comp_type_array[i_index].type_str),
                (comp_type = comp_type_array[i_index].type_num));
        }

        return_val_do_info_if_fail(INVALID_DATA_BYTE != comp_type, VOS_ERR,
            debug_log(DLOG_DEBUG, "%s: Component type check failed.", __FUNCTION__));
    }

    ret_bool = json_object_object_get_ex(filter_jso, PROPERTY_DA_REPORT_METRIC_TYPE, &metric_type_jso);
    if (ret_bool) {
        type = dal_json_object_get_str(metric_type_jso);

        for (i_index = 0; metric_type_array[i_index]; i_index++) {
            break_do_info_if_expr(0 == g_ascii_strcasecmp(type, metric_type_array[i_index]), (metric_type = 0);
                (metric_type |= (metric_bit << i_index)));
        }

        return_val_do_info_if_fail(INVALID_DATA_BYTE != metric_type, VOS_ERR,
            debug_log(DLOG_DEBUG, "%s: metric type check failed.", __FUNCTION__));
    }

    *input_list = g_slist_append(*input_list, (gpointer)g_variant_new_byte(comp_type));
    *input_list = g_slist_append(*input_list, (gpointer)g_variant_new_byte(metric_type));
    *input_list = g_slist_append(*input_list, (gpointer)g_variant_new_uint32(start_time));
    *input_list = g_slist_append(*input_list, (gpointer)g_variant_new_uint32(end_time));

    
    ret = dal_get_object_handle_nth(CLASS_RACK_MANAGEMENT, 0, &rack_mgnt_hnd);
    return_val_if_fail(VOS_OK == ret, VOS_OK);

    if (json_object_object_get_ex(filter_jso, PROPERTY_DA_REPORT_SER_NUM, &ser_num_jso)) {
        ser_num = dal_json_object_get_str(ser_num_jso);
        if (ser_num != NULL) {
            debug_log(DLOG_DEBUG, "%s: device serial number is %s", __FUNCTION__, ser_num);
            *input_list = g_slist_append(*input_list, (gpointer)g_variant_new_string(ser_num));
            return VOS_OK;
        }
    }

    
    *input_list = g_slist_append(*input_list, (gpointer)g_variant_new_string(INVALID_DATA_STRING));

    return VOS_OK;
}


LOCAL gint32 get_da_report_data(PROVIDER_PARAS_S *i_paras, json_object *o_message_array_jso, gchar **data_str)
{
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    OBJ_HANDLE rack_mgnt_hnd = 0;
    json_object *message_jso = NULL;
    const gchar *output_str = NULL;

    
    ret = get_data_acquisition_report_filter(i_paras->user_data, &input_list);
    if (ret != VOS_OK) {
        (void)create_message_info(MSGID_QUERY_NOT_SUPPORTED_RSC, NULL, &message_jso);
        json_object_array_add(o_message_array_jso, message_jso);
        debug_log(DLOG_DEBUG, "%s: get_data_acquisition_report_filter failed.", __FUNCTION__);
        return HTTP_BAD_REQUEST;
    }

    
    ret = dal_get_object_handle_nth(CLASS_RACK_MANAGEMENT, 0, &rack_mgnt_hnd);
    if (ret == VOS_OK) {
        
        ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, rack_mgnt_hnd,
            CLASS_RACK_MANAGEMENT, METHOD_QUERY_RACK_DA_DATA, AUTH_DISABLE, i_paras->user_role_privilege, input_list,
            &output_list);
        uip_free_gvariant_list(input_list);

        
        
        if (ret == RFERR_WRONG_PARAM || ret == RFERR_NO_RESOURCE) {
            (void)create_message_info(MSGID_QUERY_NOT_SUPPORTED_RSC, NULL, &message_jso);
            json_object_array_add(o_message_array_jso, message_jso);
            debug_log(DLOG_DEBUG, "%s: rack data acquisition query failed, ret=%d", __FUNCTION__, ret);
            return HTTP_BAD_REQUEST;
        } else if (ret != RET_OK) {
            debug_log(DLOG_MASS, "call method %s failed, ret is %d", METHOD_QUERY_RACK_DA_DATA, ret);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
    } else {
        
        ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, 0,
            CLASS_DA_SERVICE, METHOD_GET_TABLE_INFO, AUTH_DISABLE, i_paras->user_role_privilege, input_list,
            &output_list);
        uip_free_gvariant_list(input_list);
        if (ret == VOS_ERR_OBJ_UNAVAILABLE) {
            debug_log(DLOG_MASS, "%s: call method %s failed, ret is %d", __FUNCTION__, METHOD_GET_TABLE_INFO, ret);
            return HTTP_BAD_REQUEST;
        } else if (ret != VOS_OK) {
            debug_log(DLOG_MASS, "%s: call method %s failed, ret is %d", __FUNCTION__, METHOD_GET_TABLE_INFO, ret);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
    }

    output_str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    *data_str = (output_str == NULL || strlen(output_str) == 0) ? NULL : g_strdup(output_str);
    uip_free_gvariant_list(output_list);
    return VOS_OK;
}


gint32 get_data_acquisition_report(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    gint32 ret;
    gchar buffer[MAX_OEM_PROP_VAL_LEN] = {0};
    gchar *report_data = NULL;
    guint32 rsp_len;

    
    if (provider_paras_check(i_paras) != VOS_OK || o_err_array_jso == NULL || o_rsp_body_str == NULL) {
        debug_log(DLOG_ERROR, "%s: Invalid paramters", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if ((i_paras->user_role_privilege & USERROLE_READONLY) == 0) {
        return HTTP_FORBIDDEN;
    }

    add_val_str_to_rsp_body(RFPROP_ODATA_ID, RF_DA_ACQ_REPORT_URI, FALSE, buffer, sizeof(buffer));
    add_val_str_to_rsp_body(RFPROP_COMMON_ID, RFPROP_VAL_DA_REPORT_ID, FALSE, buffer, sizeof(buffer));
    add_val_str_to_rsp_body(RFPROP_COMMON_NAME, RFPROP_VAL_DA_REPORT_NAME, FALSE, buffer, sizeof(buffer));

    ret = get_da_report_data(i_paras, o_err_array_jso, &report_data);
    if (ret != VOS_OK) {
        return ret;
    }
    if (report_data == NULL) {
        add_val_str_to_rsp_body(PROPERTY_DA_REPORT_DATA, "", TRUE, buffer, sizeof(buffer));
        ret = set_rsp_body_content((const gchar*)buffer, o_rsp_body_str);
        return ((ret == VOS_OK) ? RF_OK : HTTP_INTERNAL_SERVER_ERROR);
    }

    rsp_len = strlen(buffer) + strlen(report_data) + strlen(PROPERTY_DA_REPORT_DATA) + APPEND_BUFFER_LEN;
    *o_rsp_body_str = (gchar *)g_malloc0(rsp_len);
    if (*o_rsp_body_str == NULL) {
        debug_log(DLOG_ERROR, "%s: call g_malloc0 failed", __FUNCTION__);
        g_free(report_data);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = snprintf_s(*o_rsp_body_str, rsp_len, rsp_len - 1, "{%s\"%s\":%s}", buffer, PROPERTY_DA_REPORT_DATA,
        report_data);
    g_free(report_data);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s failed, ret:%d", __FUNCTION__, ret);
        g_free(*o_rsp_body_str);
        *o_rsp_body_str = NULL;
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return RF_OK;
}


gint32 dataacquisition_report_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    guint8 data_acquire_support = 0;

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROPERTY_PME_SERVICECONFIG_DATA_ACQUISITION_SUPPORT,
        &data_acquire_support);
    return_val_do_info_if_fail(1 == data_acquire_support, HTTP_NOT_FOUND,
        debug_log(DLOG_DEBUG, "%s : this resource not enabled by license", __FUNCTION__));

    *prop_provider = g_dataacquisition_report_provider;
    *count = 0;

    return VOS_OK;
}
