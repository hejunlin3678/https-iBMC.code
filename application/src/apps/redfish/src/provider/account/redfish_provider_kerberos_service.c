

#include "redfish_provider.h"

LOCAL gint32 get_kerberos_enable(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 set_kerberos_enable(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);


LOCAL PROPERTY_PROVIDER_S kerberosservice_provider[] = {
    {
        PROPERTY_KRB_SERVICE_ENABLED,
        KRBCOMMON_CLASS_NAME, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        get_kerberos_enable,
        set_kerberos_enable,
        NULL, ETAG_FLAG_ENABLE
    }
};

gboolean is_kerberos_support(void)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    guint8 kerberos_support;
    GVariant *property_value = NULL;

    ret = dfl_get_object_handle(OBJ_NAME_PME_SERVICECONFIG, &obj_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "dfl_get_object_handle failed ret:%d\n",  ret);
        return FALSE;
    }
    ret = dfl_get_property_value(obj_handle, PROPERTY_KERBEROS_SUPPORT, &property_value);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "dfl_get_property_value failed ret:%d\n", ret);
        return FALSE;
    }
    kerberos_support = g_variant_get_byte(property_value);
    g_variant_unref(property_value);

    
    if (kerberos_support == 1) {
        return TRUE;
    }
    return FALSE;
}

/*****************************************************************************
 功能描述  : 获取kerberos使能信息
 输入参数  : PROVIDER_PARAS_S *i_paras
 输出参数  : json_object **o_message_jso
                          json_object **o_result_jso
 返 回 值  : 无

 修改历史      :
  1.日    期   : 2019年02月11日
    作    者   : zhouchaochao z00416979
    修改内容   : 新生成函数
**************************************************************************** */
LOCAL gint32 get_kerberos_enable(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    guchar krb_enabled = 0;
    OBJ_HANDLE obj_handle;

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_READONLY, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = dfl_get_object_handle(KRBCOMMON_CLASS_NAME, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: Get KRBCommon object failed.", __FUNCTION__, __LINE__));
    
    ret = dal_get_property_value_byte(obj_handle, KRBCOMMON_ATTRIBUTE_ENABLE, &krb_enabled);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: Get prop Enable failed.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_boolean(krb_enabled == 0 ? FALSE : TRUE);

    return HTTP_OK;
}

LOCAL gint32 set_kerberos_enable(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gboolean service_enabled;
    OBJ_HANDLE obj_handle;
    GSList *input_list = NULL;

    
    return_val_do_info_if_fail(VOS_OK == provider_paras_check(i_paras), HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    
    ret = dfl_get_object_handle(KRBCOMMON_CLASS_NAME, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s, %d: Get KRBCommon obj handle failed.", __FUNCTION__, __LINE__));

    
    service_enabled = json_object_get_boolean(i_paras->val_jso);
    input_list = g_slist_append(input_list, g_variant_new_byte((TRUE == service_enabled) ? 1 : 0));

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        KRBCOMMON_CLASS_NAME, KRBCOMMON_METHOD_SET_KERBEROS_ENABLE, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, NULL);
    uip_free_gvariant_list(input_list);

    
    if (VOS_OK == ret) {
        return HTTP_OK;
    }
    
    return_val_do_info_if_expr(RFERR_INSUFFICIENT_PRIVILEGE == ret, RFERR_INSUFFICIENT_PRIVILEGE,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, PROPERTY_KRB_SERVICE_ENABLED, o_message_jso,
        PROPERTY_KRB_SERVICE_ENABLED));
    
    return_val_do_info_if_expr(RFERR_WRONG_PARAM == ret, RFERR_WRONG_PARAM,
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri));
    
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    return HTTP_INTERNAL_SERVER_ERROR;
}

gint32 kerberosservice_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    if (!is_kerberos_support()) {
        return HTTP_NOT_FOUND;
    }
    
    return_val_if_fail(NULL != i_paras && NULL != prop_provider && NULL != count, HTTP_INTERNAL_SERVER_ERROR);

    *prop_provider = kerberosservice_provider;
    *count = sizeof(kerberosservice_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}
