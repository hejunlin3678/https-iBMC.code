
#include "redfish_provider.h"

LOCAL gint32 get_rack_mgnt_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_rack_mgnt_service_provider[] = {
    {
        RFPROP_ODATA_ID,
        RF_VALUE_NULL,
        RF_VALUE_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_rack_mgnt_odata_id,
        NULL,
        NULL,
        ETAG_FLAG_ENABLE
    }
};


LOCAL gint32 get_rack_mgnt_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_string(URI_FORMAT_RACK_MGNT_SERVICE);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);
    return HTTP_OK;
}


gboolean rf_support_rack_mgnt_agent_service(void)
{
    gint32 ret;
    OBJ_HANDLE rack_mgmt_obj_handle = 0;
    guchar support_rack_mgmt_agent_flg = 0;

    // 获取机柜管理对象句柄
    ret = dal_get_object_handle_nth(CLASS_RACK_MANAGEMENT, 0, &rack_mgmt_obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, FALSE,
        debug_log(DLOG_DEBUG, "%s: get rack_mgmt_obj_handle failed. ret=%d\n", __FUNCTION__, ret));

    // 获取代理服务的可用性
    ret = dal_get_property_value_byte(rack_mgmt_obj_handle, PROPERTY_RACK_MANAGEMENT_AGENT_SVC_STATUS,
        &support_rack_mgmt_agent_flg);
    return_val_do_info_if_fail(VOS_OK == ret, FALSE,
        debug_log(DLOG_ERROR, "%s: get support_rack_mgmt_agent_flg value fail. ret=%d\n", __FUNCTION__, ret));

    return (DISABLED != support_rack_mgmt_agent_flg) ? TRUE : FALSE;
}


gint32 get_rack_mgnt_service(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    if (o_rsc_jso == NULL || o_err_array_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return RF_FAILED;
    }

    
    json_object_object_add(o_rsc_jso, RFPROP_SERVICE_ENABLED,
        json_object_new_boolean(rf_support_rack_mgnt_agent_service()));

    return RF_OK;
}


gint32 rack_mgnt_service_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gint32 ret;
    OBJ_HANDLE rack_mgnt_hd;

    
    return_val_if_fail(NULL != i_paras && NULL != prop_provider && NULL != count, HTTP_INTERNAL_SERVER_ERROR);

    ret = dal_get_object_handle_nth(CLASS_RACK_MANAGEMENT, 0, &rack_mgnt_hd);
    return_val_if_fail(VOS_OK == ret, HTTP_NOT_FOUND);

    *prop_provider = g_rack_mgnt_service_provider;
    *count = sizeof(g_rack_mgnt_service_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}

