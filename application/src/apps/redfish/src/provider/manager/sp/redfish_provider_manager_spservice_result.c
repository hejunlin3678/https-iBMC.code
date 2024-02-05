
#include "redfish_provider.h"
#include "redfish_http.h"
#include "redfish_provider_manager.h"

LOCAL PROPERTY_PROVIDER_S  g_manager_sp_result_provider[] = {
};


LOCAL gulong g_sp_mntr_task_id = 0;
LOCAL guint32 g_sp_status = SP_RET_IDLE;
LOCAL json_object *g_sp_result = NULL;
guint32 g_sp_timeout = SP_DEFAULT_TIMEOUT;
guint8 g_sp_finished = 0;


void set_sp_service_result_status(guint32 status)
{
    g_sp_status = status;
}

gint32 manager_sp_result_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    return_val_do_info_if_expr(FALSE == rf_support_sp_service(), HTTP_NOT_FOUND,
        debug_log(DLOG_MASS, "%s, %d: not support", __FUNCTION__, __LINE__));

    if (redfish_check_manager_uri_valid(i_paras->uri)) {
        *prop_provider = g_manager_sp_result_provider;
        
        *count = 0;
        ;
        return VOS_OK;
    }

    return HTTP_NOT_FOUND;
}


gint32 _sp_get_connect_state(void)
{
    gint32 ret;
    OBJ_HANDLE sms_handle;
    guint8 sms_healthstate;
    guint8 sms_registerstate;
    gchar sms_type[PROP_VAL_MAX_LENGTH] = {0};

    ret = dal_get_specific_object_byte(CLASS_SMS, PROPERTY_SMS_ID, 1, &sms_handle);
    return_val_if_expr(ret != VOS_OK, VOS_ERR);

    ret = dal_get_property_value_byte(sms_handle, PROPERTY_SMS_HEALTHSTATE, &sms_healthstate);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "get sms_healthstate of PROPERTY_SMS_ID(1) fail\n"));

    ret = dal_get_property_value_byte(sms_handle, PROPERTY_SMS_REGISTERSTATE, &sms_registerstate);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "get sms_registerstate of PROPERTY_SMS_ID(1) fail\n"));

    ret = dal_get_property_value_string(sms_handle, PROPERTY_SMS_TYPE, sms_type, PROP_VAL_MAX_LENGTH);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "get sms_registerstate of PROPERTY_SMS_ID(1) fail\n"));

    if (0 == g_strcmp0(PROPERTY_VALUE_SMS_TYPE_SP, sms_type) && SMS_HEALTH_STATE_OK == sms_healthstate &&
        SMS_REGISTER_OK == sms_registerstate) {
        return SMS_BMA_GLOBAL_STATE_OK;
    } else {
        return SMS_BMA_GLOBAL_STATE_NOTOK;
    }
}


LOCAL gint32 _sp_result_foward_request_string(gchar *request_string, gint32 length)
{
    int iRet;
    errno_t safe_fun_ret;
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    const gchar *src_string = NULL;
    gchar foward_string_uri[MAX_CHARACTER_NUM] = {0};

    guchar id_byte = 0;

    json_object *request_obj = NULL;

    ret = dal_get_object_handle_nth(CLASS_SMS, 0, &obj_handle);
    return_val_if_expr(ret != VOS_OK, VOS_ERR);

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SMS_ID, &id_byte);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_ERROR, "[%s]: dal_get_property_value_byte failed", __FUNCTION__));

    iRet = snprintf_s(foward_string_uri, MAX_CHARACTER_NUM, MAX_CHARACTER_NUM - 1, SP_FORWARD_SPRESULT_URI, id_byte);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    request_obj = json_object_new_object();
    return_val_do_info_if_expr(NULL == request_obj, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d:   NULL.\n", __FUNCTION__, __LINE__));

    json_object_object_add(request_obj, REQUEST_METHOD, json_object_new_string(HTTP_GET));
    json_object_object_add(request_obj, RELATIVE_URI, json_object_new_string((const char *)foward_string_uri));

    src_string = dal_json_object_get_str(request_obj);

    safe_fun_ret = strncpy_s(request_string, length, src_string, strlen(src_string));
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

    json_object_put(request_obj);

    return VOS_OK;
}

LOCAL gint32 _sp_result_request_check_ret(json_object *obj)
{
    errno_t safe_fun_ret;
    json_object *code_obj = NULL;
    guint32 code = 0;
    gint32 ret;
    gchar code_string[MAX_CHARACTER_NUM] = {0};
    const gchar *code_p = NULL;

    ret = json_object_object_get_ex(obj, RESPONSE_STATUS_CODE, &code_obj);
    return_val_do_info_if_expr(TRUE != ret || NULL == code_obj, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d:   json_object_object_get_ex error.\n", __FUNCTION__, __LINE__));

    code_p = dal_json_object_get_str(code_obj);
    return_val_do_info_if_expr(NULL == code_p, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d:   code_p NULL.\n", __FUNCTION__, __LINE__));

    safe_fun_ret = strncpy_s(code_string, MAX_CHARACTER_NUM, code_p, MAX_CHARACTER_NUM - 1);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    ret = redfish_strip_to_int(code_string, sizeof(code_string));
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d:   code_string error.\n", __FUNCTION__, __LINE__));

    ret = vos_str2int(code_string, 10, &code, NUM_TPYE_UINT32);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d:   vos_str2int error.\n", __FUNCTION__, __LINE__));

    if (code != HTTP_OK) {
        return VOS_ERR;
    }

    return VOS_OK;
}


LOCAL void _sp_copy_result(json_object *dst, json_object *src)
{
    int ret;
    json_object *tmp_json = NULL;
// 此处用于消除foreach编译告警"assignment discards 'const' qualifier from pointer target type"
#pragma GCC diagnostic push // require GCC 4.6
#pragma GCC diagnostic ignored "-Wcast-qual"
    json_object_object_foreach0(src, name, val)
    {
        continue_if_expr(0 == g_strcmp0(name, RFOBJ_ID) || 0 == g_strcmp0(name, RFOBJ_NAME) ||
            0 == g_strcmp0(name, ODATA_ID) || 0 == g_strcmp0(name, ODATA_CONTEXT) || 0 == g_strcmp0(name, ODATA_TYPE) ||
            0 == g_strcmp0(name, RFPROP_SPSERVICE_STATUS));

        ret = json_object_deep_copy(val, &tmp_json, NULL);
        if (ret != VOS_OK) {
            debug_log(DLOG_ERROR, "%s: deep copy json failed, ret is %d, key is %s", __FUNCTION__, ret, name);
        }

        ret = json_object_object_add(dst, name, tmp_json);
        if (ret != VOS_OK) {
            debug_log(DLOG_ERROR, "%s: add json_object failed, ret is %d, key is %s", __FUNCTION__, ret, name);
            json_object_put(tmp_json);
        }
        tmp_json = NULL;
    }
#pragma GCC diagnostic pop // require GCC 4.6
}


void get_sp_result_in_mem(json_object *jso_dst)
{
    if (g_sp_result == NULL || jso_dst == NULL) {
        return;
    }

    _sp_copy_result(jso_dst, g_sp_result);
}


LOCAL gint32 _sp_result_request(json_object *out_rsc_js)
{
    gint32 ret;
    gchar request_string[MAX_URL_LEN] = {0};
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    const gchar *out_string = NULL;
    json_object *out_obj = NULL;
    json_object *out_obj_rsp = NULL;
    const guint8 retry_times = HTTPS_REQUEST_FORWARD_BMA_MIN_RETRY;

    (void)_sp_result_foward_request_string(request_string, MAX_URL_LEN);

    ret = dal_get_object_handle_nth(CLASS_SMS, 0, &obj_handle);
    return_val_if_expr(ret != VOS_OK, VOS_ERR);

    input_list = g_slist_append(input_list, g_variant_new_string((const char *)request_string));
    input_list = g_slist_append(input_list, g_variant_new_int32(strlen(request_string)));
    input_list = g_slist_append(input_list, g_variant_new_byte(retry_times));

    ret = dfl_call_class_method(obj_handle, METHOD_SMS_FORWARD_REDFISH_REQUEST, NULL, input_list, &output_list);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);

    if (VOS_OK == ret) {
        out_string = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);

        out_obj = json_tokener_parse(out_string);
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        return_val_if_expr(NULL == out_obj, VOS_ERR);

        return_val_do_info_if_expr(VOS_OK != _sp_result_request_check_ret(out_obj), VOS_ERR, json_object_put(out_obj));

        (void)json_object_object_get_ex(out_obj, RESPONSE_BODY, &out_obj_rsp);

        _sp_copy_result(out_rsc_js, out_obj_rsp);
        json_object_put(out_obj);

        return VOS_OK;
    }

    return VOS_ERR;
}


gint32 get_sp_result_file_src(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    json_object *o_jso = NULL;
    gchar *str_status[] = {"Idle", "Init", "Deploying", "Finished", "Timeout"};
    static guint32 last_sp_status = SP_RET_DEPLOYING;

    if (o_rsc_jso == NULL || o_err_array_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    (void)get_sp_memberid_odata_context_id(o_rsc_jso, MANAGERS_SPSERVICE_RESULT_METADATA, MANAGER_SPSERVICE_RESULT_URI,
        i_paras->member_id);

    
    // 添加部署进度
    o_jso = json_object_new_string(str_status[g_sp_status]);
    return_val_do_info_if_expr(NULL == o_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__));
    json_object_object_add(o_rsc_jso, RFPROP_SPSERVICE_STATUS, o_jso);

    if (g_sp_status != SP_RET_DEPLOYING && g_sp_status != SP_RET_FINISHED) {
        // 空闲、初始化、超时状态下，不去获取部署结果
        return RF_OK;
    }

    if (g_sp_result == NULL) {
        g_sp_result = json_object_new_object();
    }

    if (VOS_OK == _sp_result_request(o_rsc_jso)) {
        // sp侧查询成功
        _sp_copy_result(g_sp_result, o_rsc_jso);
        last_sp_status = g_sp_status;
    } else if (get_sp_file_data(i_paras, UMS_TRANS_FILE_ID_SP_RESULT, RFPROP_MANAGER_SP_RESULT_NAME, o_rsc_jso) ==
        VOS_OK) {
        // 读取u盘文件成功
        _sp_copy_result(g_sp_result, o_rsc_jso);
        last_sp_status = g_sp_status;
    } else {
        
        o_jso = json_object_new_string(str_status[last_sp_status]);
        return_val_do_info_if_expr(o_jso == NULL, HTTP_INTERNAL_SERVER_ERROR, 
            debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__));
        json_object_object_add(o_rsc_jso, RFPROP_SPSERVICE_STATUS, o_jso);
        // 读取缓存
        _sp_copy_result(o_rsc_jso, g_sp_result);
    }
    

    return RF_OK;
}


LOCAL void task_monitor_sp_deploy_status(void *param)
{
    gint32 bma_connect_state = 0;
    guint32 cur_ticket = 0;
    guint32 cur_timeout = 0;
    g_sp_timeout = SP_DEFAULT_TIMEOUT;
    g_sp_finished = 0;

    (void)prctl(PR_SET_NAME, (uintptr_t) "rfSpDeployTask");

    if (g_sp_result) {
        json_object_put(g_sp_result);
    }
    g_sp_result = json_object_new_object();

    while (1) {
        vos_task_delay(1000);
        // SP设置超时时间，此处更新并重新计时
        if (cur_timeout != g_sp_timeout) {
            cur_ticket = 0;
            cur_timeout = g_sp_timeout;
        }

        // 检测到超时，退出监控
        if (++cur_ticket > cur_timeout) {
            g_sp_status = SP_RET_TIMEOUT;
            break;
        }

        // 部署完成，退出监控
        if (g_sp_finished) {
            g_sp_status = SP_RET_FINISHED;
            break;
        }

        // 检测BMA心跳
        bma_connect_state = _sp_get_connect_state();
        if (SMS_BMA_GLOBAL_STATE_OK == bma_connect_state) {
            cur_ticket = 0;
            g_sp_status = SP_RET_DEPLOYING;
        }
    }

    g_sp_mntr_task_id = 0;
}


gint32 create_sp_result_monitor_task(void)
{
    gint32 ret = VOS_OK;

    if (0 == g_sp_mntr_task_id) {
        g_sp_status = SP_RET_INIT;
        ret = vos_task_create(&g_sp_mntr_task_id, "SP result monitor", (TASK_ENTRY)task_monitor_sp_deploy_status,
            (void *)NULL, DEFAULT_PRIORITY);
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, debug_log(DLOG_ERROR, "%s: create task fail", __FUNCTION__));
    }

    return VOS_OK;
}


gint32 start_sp_firmware_upgrade_monitor(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    return create_sp_result_monitor_task();
}