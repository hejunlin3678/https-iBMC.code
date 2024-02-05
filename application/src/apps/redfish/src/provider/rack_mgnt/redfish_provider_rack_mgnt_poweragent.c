

#include "redfish_provider.h"
LOCAL gint32 get_power_agent_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 act_set_rack_power_capping(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_setpwrcapping_actioninfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_periodprepwrcapping_actioninfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 act_period_pre_rack_power_capping(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_rack_peak_clipping_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_rack_peak_clipping_backup_power(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_rack_peak_clipping_backup_period(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 parse_peak_clipping_request_error(gint32 err_code, const char *property_name, const char *property_value,
    json_object **o_message_jso);

#define RACK_POWER_CAPPING_INFO_FILE "rack_powercapping_info.json"
#define SET_DEV_POWER_PROPERTY_FAILED 1
#define RACK_POWER_CAPPING_NOT_CONFIGURED "NotConfigured"
LOCAL PROPERTY_PROVIDER_S  g_rack_mgnt_power_agent_provider[] = {
    {
        RFPROP_ODATA_ID,
        RF_VALUE_NULL,
        RF_VALUE_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_power_agent_odata_id,
        NULL,
        NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_SET_RACk_POWER_CAPPING_ACTION,
        MAP_PROPERTY_NULL,
        MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        NULL,
        NULL,
        act_set_rack_power_capping,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_PERIOD_PRE_RACk_POWER_CAPPING_ACTION,
        MAP_PROPERTY_NULL,
        MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        NULL,
        NULL,
        act_period_pre_rack_power_capping,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_RACK_PEAK_CLIPPING_ENABLED,
        MAP_PROPERTY_NULL,
        MAP_PROPERTY_NULL,
        USERROLE_POWERMGNT,
        SYS_LOCKDOWN_ALLOW,
        NULL,
        set_rack_peak_clipping_enabled,
        NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_RACK_BACKUP_POWER_WATTS,
        MAP_PROPERTY_NULL,
        MAP_PROPERTY_NULL,
        USERROLE_POWERMGNT,
        SYS_LOCKDOWN_ALLOW,
        NULL,
        set_rack_peak_clipping_backup_power,
        NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_RACK_BACKUP_PERIOD_SECONDS,
        MAP_PROPERTY_NULL,
        MAP_PROPERTY_NULL,
        USERROLE_POWERMGNT,
        SYS_LOCKDOWN_ALLOW,
        NULL,
        set_rack_peak_clipping_backup_period,
        NULL,
        ETAG_FLAG_ENABLE
    }
};

LOCAL PROPERTY_PROVIDER_S  g_rack_mgnt_set_rack_power_capping_actioninfo_provider[] = {
    {
        RFPROP_ODATA_ID,
        MAP_PROPERTY_NULL,
        MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_setpwrcapping_actioninfo_odata_id,
        NULL,
        NULL,
        ETAG_FLAG_ENABLE
    }
};

LOCAL PROPERTY_PROVIDER_S  g_rack_mgnt_period_pre_rack_power_capping_actioninfo_provider[] = {
    {
        RFPROP_ODATA_ID,
        MAP_PROPERTY_NULL,
        MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_periodprepwrcapping_actioninfo_odata_id,
        NULL,
        NULL,
        ETAG_FLAG_ENABLE
    }
};


LOCAL gint32 get_power_agent_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar   str_odata_id[MAX_STRBUF_LEN + 1] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s : NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = snprintf_s(str_odata_id, MAX_STRBUF_LEN + 1, MAX_STRBUF_LEN, URI_FORMAT_POWER_AGENT);
    return_val_do_info_if_fail(ret > 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s :format sting failed, ret is %d", __FUNCTION__, ret));

    *o_result_jso = json_object_new_string((const gchar *)str_odata_id);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);
    return HTTP_OK;
}

LOCAL gint32 check_save_power_capping_conf_result(GSList *output_list, json_object **o_message_jso)
{
    gint32 result;
    const gchar *para_name = NULL;

    result = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
    // 若result != VOS_OK ，则说明校验失败，需要根据实际返回异常信息构造message_info
    if (result != VOS_OK) {
        para_name = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 1), NULL);

        debug_log(DLOG_ERROR, "%s : check rack power capping config failed ,result is %d", __FUNCTION__, result);
        // 若result = RFERR_WRONG_PARAM ，则表示出现参数错误异常，message中需要属性名称、属性的值以及Action的名称
        if (result == PWR_CAP_CONF_PARAM_VAL_OUT_OF_RANGE) {
            (void)create_message_info(MSGID_RACK_PWR_CAP_PARAMETER_VALUE_INVALID, NULL, o_message_jso, para_name);
            return HTTP_BAD_REQUEST;
        }
        // 若result = RFERR_DUPLICATE_PARAM ，则表示出现参数重复，message中需要属性名称以及Action的名称
        if (result == PWR_CAP_CONF_SERIALNUMBER_DUPLICATED) {
            (void)create_message_info(MSGID_RACK_PWR_CAP_SERIALNUMBER_DUPLICATE, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        }
        // 若result = PWR_CAP_CONF_PEAK_CLIPPING_ENABLED ，则表示出关闭功耗封顶时，削峰填谷处于开启状态
        if (result == PWR_CAP_CONF_PEAK_CLIPPING_ENABLED) {
            (void)create_message_info(MSGID_DISABLE_PWR_CAP_FAIL_WITH_PEAK_CLIP_ENABLE, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        }
        // 若result = PWR_CAP_CONF_DEVICE_COMM_LOST ，则表示机柜内有设备通信丢失
        if (result == PWR_CAP_CONF_DEVICE_COMM_LOST) {
            (void)create_message_info(MSGID_OPERATION_FAILED, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        }

        // 其余错误信息说明校验过程中产生内部错误
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return VOS_OK;
}

LOCAL gint32 act_set_rack_power_capping(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_rack_mgmt = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s : NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_object_handle_nth(CLASS_RACK_MANAGEMENT, 0, &obj_rack_mgmt);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s :Get object handle of %s failed. ret is %d\n", __FUNCTION__, CLASS_RACK_MANAGEMENT,
        ret));

    // 将json参数转为string 作为方法调用的入参
    input_list = g_slist_append(input_list, g_variant_new_string(dal_json_object_get_str(i_paras->val_jso)));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_rack_mgmt,
        CLASS_RACK_MANAGEMENT, METHOD_SAVE_POWER_CAPPING_CONFIG, AUTH_ENABLE, i_paras->user_role_privilege, input_list,
        &output_list);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s :call method %s failed ,ret is %d", __FUNCTION__, METHOD_SAVE_POWER_CAPPING_CONFIG,
            ret);
        goto exit;
    }
    ret = check_save_power_capping_conf_result(output_list, o_message_jso);
    if (ret != VOS_OK) {
        uip_free_gvariant_list(input_list);
        uip_free_gvariant_list(output_list);
        return ret;
    }

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_rack_mgmt,
        CLASS_RACK_MANAGEMENT, METHOD_UPDATE_POW_CAP_CFG, AUTH_ENABLE, i_paras->user_role_privilege, NULL, NULL);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s :call method %s failed ,ret is %d", __FUNCTION__, METHOD_UPDATE_POW_CAP_CFG, ret);
    }

exit:
    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);
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
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}

LOCAL guint32 set_dev_period_power_value(PROVIDER_PARAS_S *i_paras, guint32 aver_power, guint32 peak_power,
    OBJ_HANDLE obj_hnd)
{
    GSList *input_list = NULL;
    guint32 ret;

    
    if (VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s : NULL pointer.", __FUNCTION__);
        return VOS_ERR;
    }

    
    input_list = g_slist_append(input_list, g_variant_new_uint32(aver_power));

    input_list = g_slist_append(input_list, g_variant_new_uint32(peak_power));

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_hnd,
        CLASS_DEVICE_INFO, METHOD_SET_PERIOD_POWER, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s : Get method %s failed ! ret is %d", __FUNCTION__, METHOD_SET_PERIOD_POWER, ret);
        return VOS_ERR;
    }

    return VOS_OK;
}

LOCAL gint32 act_period_pre_rack_power_capping(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint32 ret;
    json_object *json_obj_pre_para = NULL;
    json_object *json_obj_ser_num = NULL;
    json_object *json_obj_aver_power = NULL;
    json_object *json_obj_peak_power = NULL;
    json_object *tmp_jso = NULL;
    const gchar *ser_num = NULL;
    guint32 aver_power = 0;
    guint32 peak_power = 0;
    OBJ_HANDLE obj_hnd = 0;
    guint32 err_flag = 0;
    guint32 len;
    guint32 i;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s : NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    ret = json_object_object_get_ex(i_paras->val_jso, RFPROP_PERIOD_PRE_PARA, &json_obj_pre_para);
    return_val_do_info_if_expr(FALSE == ret, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_ACT_PARA_MISSING, NULL, o_message_jso,
        RFPROP_PERIOD_PRE_RACk_POWER_CAPPING_ACTION, RFPROP_PERIOD_PRE_PARA));

    len = json_object_array_length(json_obj_pre_para);

    for (i = 0; i < len; i++) {
        tmp_jso = json_object_array_get_idx(json_obj_pre_para, i);
        
        ret = json_object_object_get_ex(tmp_jso, RFPROP_SERIAL_NUMBER, &json_obj_ser_num);
        continue_do_info_if_expr(FALSE == ret, err_flag = SET_DEV_POWER_PROPERTY_FAILED;
            debug_log(DLOG_ERROR, "%s :Get json data %s failed!", __FUNCTION__, RFPROP_SERIAL_NUMBER));

        
        ret = json_object_object_get_ex(tmp_jso, RFPROP_AVERAGE_POWER_WATTS, &json_obj_aver_power);
        continue_do_info_if_expr(FALSE == ret, err_flag = SET_DEV_POWER_PROPERTY_FAILED;
            debug_log(DLOG_ERROR, "%s :Get json data %s failed!", __FUNCTION__, RFPROP_AVERAGE_POWER_WATTS));

        ret = json_object_object_get_ex(tmp_jso, RFPROP_PEAK_POWER_WATTS, &json_obj_peak_power);
        continue_do_info_if_expr(FALSE == ret, err_flag = SET_DEV_POWER_PROPERTY_FAILED;
            debug_log(DLOG_ERROR, "%s :Get json data %s failed!", __FUNCTION__, RFPROP_PEAK_POWER_WATTS));

        ser_num = dal_json_object_get_str(json_obj_ser_num);
        aver_power = json_object_get_int(json_obj_aver_power);
        peak_power = json_object_get_int(json_obj_peak_power);

        
        ret = dal_get_specific_object_string(CLASS_DEVICE_INFO, PROPERTY_DEVICE_INFO_SER_NUM, ser_num, &obj_hnd);
        continue_do_info_if_expr(VOS_OK != ret, err_flag = SET_DEV_POWER_PROPERTY_FAILED;
            debug_log(DLOG_ERROR, "%s :No object with giving ser_num %s was found!", __FUNCTION__, ser_num));

        ret = set_dev_period_power_value(i_paras, aver_power, peak_power, obj_hnd);
        if (ret != VOS_OK) {
            err_flag = SET_DEV_POWER_PROPERTY_FAILED;
            continue;
        }
    }

    if (err_flag == SET_DEV_POWER_PROPERTY_FAILED) {
        debug_log(DLOG_ERROR, "%s : Set device period pre power failed ! \r\n", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 set_rack_peak_clipping_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
#define MAX_PROPERTY_LEN 10
    gint32 ret;
    gint32 method_result;
    GSList *input_list = NULL;
    OBJ_HANDLE obj_rack_mgmt = 0;
    guint8 peak_clipping_enabled;
    gchar property_value[MAX_PROPERTY_LEN] = {};

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s : NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if ((i_paras->user_role_privilege & USERROLE_POWERMGNT) == 0) {
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_RACK_PEAK_CLIPPING_ENABLED,
            o_message_jso, RFPROP_RACK_PEAK_CLIPPING_ENABLED);
        return HTTP_FORBIDDEN;
    }

    ret = dal_get_object_handle_nth(CLASS_RACK_MANAGEMENT, 0, &obj_rack_mgmt);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s dal_get_object_handle_nth error, ret is %d", __FUNCTION__, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    peak_clipping_enabled = json_object_get_boolean(i_paras->val_jso) ? ENABLE : DISABLE;
    input_list = g_slist_append(input_list, g_variant_new_byte(peak_clipping_enabled));

    method_result = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        obj_rack_mgmt, CLASS_RACK_MANAGEMENT, METHOD_SET_PEAK_CLIPPING_ENABLED, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, NULL);

    uip_free_gvariant_list(input_list);

    if (method_result == VOS_OK) {
        return HTTP_OK;
    }

    ret = snprintf_s(property_value, MAX_PROPERTY_LEN, MAX_PROPERTY_LEN - 1, "%s",
        peak_clipping_enabled ? "true" : "false");
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s failed, ret:%d", __FUNCTION__, ret);
    }

    return parse_peak_clipping_request_error(method_result, RFPROP_RACK_PEAK_CLIPPING_ENABLED,
        (const gchar*)property_value, o_message_jso);
}


LOCAL gint32 set_rack_peak_clipping_backup_power(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
#define MAX_PROPERTY_LEN 10
    gint32 ret;
    GSList *input_list = NULL;
    OBJ_HANDLE obj_rack_mgmt = 0;
    guint32 backup_power;
    gchar property_value[MAX_PROPERTY_LEN] = {};

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s : NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if ((i_paras->user_role_privilege & USERROLE_POWERMGNT) == 0) {
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_RACK_BACKUP_POWER_WATTS,
            o_message_jso, RFPROP_RACK_BACKUP_POWER_WATTS);
        return HTTP_FORBIDDEN;
    }

    ret = dal_get_object_handle_nth(CLASS_RACK_MANAGEMENT, 0, &obj_rack_mgmt);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s dal_get_object_handle_nth error, ret is %d", __FUNCTION__, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    backup_power = json_object_get_int(i_paras->val_jso);

    input_list = g_slist_append(input_list, g_variant_new_uint32(backup_power));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_rack_mgmt,
        CLASS_RACK_MANAGEMENT, METHOD_SET_PEAK_CLIPPING_BACKUP_POWER, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, NULL);

    uip_free_gvariant_list(input_list);

    if (ret == VOS_OK) {
        return HTTP_OK;
    }

    (void)snprintf_s(property_value, MAX_PROPERTY_LEN, MAX_PROPERTY_LEN - 1, "%u", backup_power);

    return parse_peak_clipping_request_error(ret, RFPROP_RACK_BACKUP_POWER_WATTS, property_value, o_message_jso);
}


LOCAL gint32 set_rack_peak_clipping_backup_period(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
#define MAX_PROPERTY_LEN 10
    gint32 ret;
    GSList *input_list = NULL;
    OBJ_HANDLE obj_rack_mgmt = 0;
    guint32 backup_period;
    gchar property_value[MAX_PROPERTY_LEN] = {};

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s : NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if ((i_paras->user_role_privilege & USERROLE_POWERMGNT) == 0) {
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_RACK_BACKUP_PERIOD_SECONDS,
            o_message_jso, RFPROP_RACK_BACKUP_PERIOD_SECONDS);
        return HTTP_FORBIDDEN;
    }

    ret = dal_get_object_handle_nth(CLASS_RACK_MANAGEMENT, 0, &obj_rack_mgmt);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s dal_get_object_handle_nth error, ret is %d", __FUNCTION__, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    backup_period = json_object_get_int(i_paras->val_jso);
    input_list = g_slist_append(input_list, g_variant_new_uint32(backup_period));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_rack_mgmt,
        CLASS_RACK_MANAGEMENT, METHOD_SET_PEAK_CLIPPING_BACKUP_PERIOD, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, NULL);

    uip_free_gvariant_list(input_list);

    if (ret == VOS_OK) {
        return HTTP_OK;
    }

    (void)snprintf_s(property_value, MAX_PROPERTY_LEN, MAX_PROPERTY_LEN - 1, "%u", backup_period);

    return parse_peak_clipping_request_error(ret, RFPROP_RACK_BACKUP_POWER_WATTS, property_value, o_message_jso);
}


LOCAL gint32 parse_peak_clipping_request_error(gint32 err_code, const char *property_name, const char *property_value,
    json_object **o_message_jso)
{
    switch (err_code) {
        case RFERR_SUCCESS:
            return HTTP_OK;

        
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
            return RFERR_INSUFFICIENT_PRIVILEGE;

        
        case RFERR_WRONG_PARAM:
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, property_name, o_message_jso, property_value,
                property_name);
            return HTTP_BAD_REQUEST;

        
        case POWER_PEAK_RET_CAPPING_DISABLED:
            (void)create_message_info(MSGID_RACK_POWER_CAPPING_DISABLED, property_name, o_message_jso);
            return HTTP_BAD_REQUEST;

        
        case POWER_PEAK_RET_BATTERY_NOT_CONFIGURATION:
            (void)create_message_info(MSGID_RACK_BATTERY_NOT_CONFIGURATION, property_name, o_message_jso);
            return HTTP_BAD_REQUEST;

        
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 get_setpwrcapping_actioninfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar   str_odata_id[MAX_STRBUF_LEN + 1] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s : NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = snprintf_s(str_odata_id, MAX_STRBUF_LEN + 1, MAX_STRBUF_LEN, URI_FORMAT_SET_RACk_POWER_CAPPING_ACTIONINFO);
    return_val_do_info_if_fail(ret > 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s :format sting failed, ret is %d", __FUNCTION__, ret));

    *o_result_jso = json_object_new_string((const gchar *)str_odata_id);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);
    return HTTP_OK;
}

LOCAL gint32 get_periodprepwrcapping_actioninfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar   str_odata_id[MAX_STRBUF_LEN + 1] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s : NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = snprintf_s(str_odata_id, MAX_STRBUF_LEN + 1, MAX_STRBUF_LEN,
        URI_FORMAT_PERIOD_PRE_RACk_POWER_CAPPING_ACTIONINFO);
    return_val_do_info_if_fail(ret > 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s :format sting failed, ret is %d", __FUNCTION__, ret));

    *o_result_jso = json_object_new_string((const gchar *)str_odata_id);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);
    return HTTP_OK;
}

LOCAL void get_rack_power_info(json_object *o_rsc_jso)
{
    OBJ_HANDLE obj_rack_mgmt = 0;
    OBJ_HANDLE obj_sys_battery = 0;
    
    guint8 rack_powcap_enabled = 0;
    guint8 rack_capping_strategy = 0;
    guint8 assigned_dev_counts = 0;
    guint32 rack_capping_power = 0;
    
    guint8 rack_powcap_state = 0;
    guint8 peak_clipping_status = DISABLE;
    guint32 backup_power = 0;
    guint32 backup_period = 0;

    
    // CLASS_RACK_MANAGEMENT的对象在上层entry入口处已进行判断，此处无需判断返回值
    (void)dal_get_object_handle_nth(CLASS_RACK_MANAGEMENT, 0, &obj_rack_mgmt);

    (void)dal_get_property_value_byte(obj_rack_mgmt, PROPERTY_RACK_MANAGEMENT_POWER_CAPPING_ENABLED,
        &rack_powcap_enabled);
    (void)dal_get_property_value_uint32(obj_rack_mgmt, PROPERTY_RACK_MANAGEMENT_CAPPING_POWER, &rack_capping_power);
    (void)dal_get_property_value_byte(obj_rack_mgmt, PROPERTY_RACK_MANAGEMENT_CAPPING_STRATEGY, &rack_capping_strategy);
    (void)dal_get_property_value_byte(obj_rack_mgmt, PROPERTY_RACK_MANAGEMENT_ASSIGNED_DEV_COUNTS,
        &assigned_dev_counts);
    (void)dal_get_property_value_byte(obj_rack_mgmt, PROPERTY_RACK_MANAGEMENT_POWER_CAPPING_STATE, &rack_powcap_state);

    
    (void)dal_get_property_value_byte(obj_rack_mgmt, PROPERTY_RACK_MANAGEMENT_PEAK_CLIPPING_ENABLED,
        &peak_clipping_status);

    (void)dal_get_object_handle_nth(CLASS_SYS_BATTERY, 0, &obj_sys_battery);
    (void)dal_get_property_value_uint32(obj_sys_battery, PROPERTY_SYS_BATTERY_BACKUP_POWER, &backup_power);
    (void)dal_get_property_value_uint32(obj_sys_battery, PROPERTY_SYS_BATTERY_BACKUP_PERIOD, &backup_period);

    if (peak_clipping_status == ENABLE) {
        if (backup_period == INVALID_UINT32) {
            backup_period = RACK_DEFAULT_BACKUP_PERIOD_SECONDS;
        }
        if (backup_power == INVALID_UINT32) {
            backup_power = rack_capping_power;
        }
    }

    if (rack_capping_strategy == RF_ALLOCATION_FOR_EQUALIZATION) {
        json_object_object_add(o_rsc_jso, RFPROP_RACK_CAPPING_STRATEGY,
            json_object_new_string(RF_CAPPING_STRATEGY_EQUALIZATION));
    } else if (rack_capping_strategy == RF_ALLOCATION_FOR_DESIGNATION) {
        json_object_object_add(o_rsc_jso, RFPROP_RACK_CAPPING_STRATEGY,
            json_object_new_string(RF_CAPPING_STRATEGY_DESIGNATION));
    } else {
        json_object_object_add(o_rsc_jso, RFPROP_RACK_CAPPING_STRATEGY,
            json_object_new_string(RACK_POWER_CAPPING_NOT_CONFIGURED));
    }

    json_object_object_add(o_rsc_jso, RFPROP_RACK_ENABLE_POWER_CAPPING,
        (rack_powcap_enabled == RF_RACK_POWER_CAPPING_ON) ? json_object_new_string(RF_POWER_CAPPING_ENABLE) :
                                                            json_object_new_string(RF_POWER_CAPPING_DISABLE));
    json_object_object_add(o_rsc_jso, RFPROP_RACK_CAPPING_NUMBERS, json_object_new_int(assigned_dev_counts));
    json_object_object_add(o_rsc_jso, RFPROP_RACK_POWER_CAPPING_VALUE, json_object_new_int(rack_capping_power));
    json_object_object_add(o_rsc_jso, RFPROP_RACK_POWER_CAPPING_STATUS,
        (rack_powcap_state == POWER_CAP_TRIGGERED) ? json_object_new_string("Triggered") :
                                                     json_object_new_string("Untriggered"));
    

    json_object_object_add(o_rsc_jso, RFPROP_RACK_PEAK_CLIPPING_ENABLED,
        peak_clipping_status ? json_object_new_boolean(TRUE) : json_object_new_boolean(FALSE));
    json_object_object_add(o_rsc_jso, RFPROP_RACK_BACKUP_POWER_WATTS,
        (backup_power == INVALID_UINT32) ? NULL : json_object_new_int(backup_power));
    json_object_object_add(o_rsc_jso, RFPROP_RACK_BACKUP_PERIOD_SECONDS,
        (backup_period == INVALID_UINT32) ? NULL : json_object_new_int(backup_period));

    return;
}

gint32 get_power_agent(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    OBJ_HANDLE obj_device_info = 0;
    gint32 ret = VOS_OK;
    GSList *device_info_list = NULL;
    GSList *device_info_node = NULL;
    json_object *devs_power_capp_info_arr_jso = NULL;
    json_object *dev_power_capp_info_jso = NULL;
    guint8 discovered = DEVICE_OFF_LINE;
    gchar           device_sn[MAX_PRO_SN_LEN + 1] = {0};
    guint8 power_cap_status = POWER_CAP_NOT_TRIGGERED;
    guint32 current_power_value = 0;
    guint32 capping_power = 0;
    guint32 power_limit_value = 0;
    guint8 dev_priority = 0;

    if (o_rsc_jso == NULL || o_err_array_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return RF_FAILED;
    }

    get_rack_power_info(o_rsc_jso);
    
    devs_power_capp_info_arr_jso = json_object_new_array();
    return_val_do_info_if_fail(NULL != devs_power_capp_info_arr_jso, RF_FAILED,
        debug_log(DLOG_ERROR, "%s: alloc new json array failed", __FUNCTION__));

    json_object_object_add(o_rsc_jso, RFPROP_DEVICE_POWER_CAPPING_INFO, devs_power_capp_info_arr_jso);

    return_val_do_info_if_expr(dfl_get_object_list(CLASS_DEVICE_INFO, &device_info_list) != VOS_OK, RF_OK,
        debug_log(DLOG_DEBUG, "%s: Get device info list failed", __FUNCTION__));

    for (device_info_node = device_info_list; NULL != device_info_node; device_info_node = device_info_node->next) {
        obj_device_info = (OBJ_HANDLE)device_info_node->data;
        (void)dal_get_property_value_byte(obj_device_info, PROPERTY_DEVICE_INFO_DISCOVERED, &discovered);
        continue_if_expr(DEVICE_OFF_LINE == discovered);

        
        (void)dal_get_property_value_string(obj_device_info, PROPERTY_DEVICE_INFO_SER_NUM, device_sn,
            sizeof(device_sn));
        continue_do_info_if_expr(0 == strlen(device_sn),
            debug_log(DLOG_INFO, "%s : device_sn is empty, iteam is invalid.", __FUNCTION__));

        (void)dal_get_property_value_byte(obj_device_info, PROPERTY_DEVICE_INFO_POW_CAP_STATE, &power_cap_status);
        (void)dal_get_property_value_uint32(obj_device_info, PROPERTY_DEVICE_INFO_CURRENT_POWER, &current_power_value);
        (void)dal_get_property_value_uint32(obj_device_info, PROPERTY_DEVICE_INFO_LIMIT_IN_WATTS, &capping_power);
        (void)dal_get_property_value_uint32(obj_device_info, PROPERTY_DEVICE_INFO_POWER_LIMIT_VALUE,
            &power_limit_value);
        (void)dal_get_property_value_byte(obj_device_info, PROPERTY_DEVICE_INFO_PRIORITY, &dev_priority);

        
        dev_power_capp_info_jso = json_object_new_object();
        json_object_object_add(dev_power_capp_info_jso, RFPROP_DEVICES_SN,
            json_object_new_string((const gchar *)device_sn));
        json_object_object_add(dev_power_capp_info_jso, RFPROP_DEVICE_POWER_CAPPING_STATUS,
            (power_cap_status == POWER_CAP_UNKNOW) ?
            json_object_new_string("Unconfigurable") :
            ((power_cap_status == POWER_CAP_TRIGGERED) ? json_object_new_string("Triggered") :
                                                         json_object_new_string("Untriggered")));
        json_object_object_add(dev_power_capp_info_jso, RFPROP_DEVICE_CURRENT_POWER_VALUE,
            json_object_new_int(current_power_value));
        json_object_object_add(dev_power_capp_info_jso, RFPROP_DEVICE_CAPPING_VALUE,
            json_object_new_int(capping_power));
        json_object_object_add(dev_power_capp_info_jso, RFPROP_DEVICE_POWER_LIMIT_VALUE,
            json_object_new_int(power_limit_value));
        json_object_object_add(dev_power_capp_info_jso, RFPROP_DEVICE_PRIORITY, json_object_new_int(dev_priority));

        ret = json_object_array_add(devs_power_capp_info_arr_jso, dev_power_capp_info_jso);
        continue_do_info_if_expr(VOS_OK != ret,
            debug_log(DLOG_ERROR, "%s :json_object_array_add error  \n", __FUNCTION__);
            json_object_put(dev_power_capp_info_jso));
    }
    g_slist_free(device_info_list);
    device_info_list = NULL;

    return RF_OK;
}


gint32 rack_power_agent_etag_del_property(json_object *object)
{
    SPECIAL_PROP_S value[] = {
        {2, {RFPROP_DEVICE_POWER_CAPPING_INFO, RFPROP_DEVICE_CURRENT_POWER_VALUE, NULL, NULL, NULL}},
        {2, {RFPROP_DEVICE_POWER_CAPPING_INFO, RFPROP_DEVICE_POWER_CAPPING_STATUS, NULL, NULL, NULL}},
        {2, {RFPROP_DEVICE_POWER_CAPPING_INFO, RFPROP_DEVICE_CAPPING_VALUE, NULL, NULL, NULL}},
        {2, {RFPROP_DEVICE_POWER_CAPPING_INFO, RFPROP_DEVICE_POWER_LIMIT_VALUE, NULL, NULL, NULL}},
        {0, {NULL, NULL, NULL, NULL, NULL}}
    };
    return rsc_del_none_etag_affected_property(object, value, G_N_ELEMENTS(value));
}


gint32 rack_mgnt_power_agent_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    
    return_val_if_fail(NULL != i_paras && NULL != prop_provider && NULL != count, HTTP_INTERNAL_SERVER_ERROR);

    return_val_do_info_if_expr(FALSE == rf_support_rack_mgnt_agent_service(), HTTP_NOT_FOUND,
        debug_log(DLOG_MASS, "%s : not support", __FUNCTION__));

    *prop_provider = g_rack_mgnt_power_agent_provider;
    *count = sizeof(g_rack_mgnt_power_agent_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}


gint32 rack_mgnt_set_pwr_capping_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras,
    PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    
    return_val_if_fail(NULL != i_paras && NULL != prop_provider && NULL != count, HTTP_INTERNAL_SERVER_ERROR);

    return_val_do_info_if_expr(FALSE == rf_support_rack_mgnt_agent_service(), HTTP_NOT_FOUND,
        debug_log(DLOG_MASS, "%s : not support", __FUNCTION__));

    *prop_provider = g_rack_mgnt_set_rack_power_capping_actioninfo_provider;
    *count = sizeof(g_rack_mgnt_set_rack_power_capping_actioninfo_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}


gint32 rack_mgnt_period_pre_pwr_capping_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras,
    PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    
    return_val_if_fail(NULL != i_paras && NULL != prop_provider && NULL != count, HTTP_INTERNAL_SERVER_ERROR);

    return_val_do_info_if_expr(FALSE == rf_support_rack_mgnt_agent_service(), HTTP_NOT_FOUND,
        debug_log(DLOG_MASS, "%s : not support", __FUNCTION__));

    *prop_provider = g_rack_mgnt_period_pre_rack_power_capping_actioninfo_provider;
    *count = sizeof(g_rack_mgnt_period_pre_rack_power_capping_actioninfo_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}
