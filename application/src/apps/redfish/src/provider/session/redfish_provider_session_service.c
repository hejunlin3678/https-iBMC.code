

#include "redfish_provider.h"

LOCAL PROPERTY_PROVIDER_S *get_self_provider_session_service(PROVIDER_PARAS_S *i_paras);


LOCAL gint32 get_sessionservice_timeout(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    guint32 timeout = 0;
    PROPERTY_PROVIDER_S *self = get_self_provider_session_service(i_paras);

    
    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != o_result_jso) && (NULL != self),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__));
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    
    ret = dal_get_object_handle_nth(self->pme_class_name, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_NOT_FOUND,
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri));
    
    ret = dal_get_property_value_uint32(obj_handle, self->pme_prop_name, &timeout);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    
    *o_result_jso = json_object_new_int(timeout);
    return HTTP_OK;
}


LOCAL gint32 get_sessionservice_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    json_object *huawei_jso = NULL;
    guint8 mode = 0;
    gint32 timeout = 0;
    OBJ_HANDLE obj_handle = 0;
    json_object *property = NULL;

    return_val_do_info_if_expr((NULL == o_message_jso) || (NULL == o_result_jso) ||
        (VOS_OK != provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__));

    return_val_do_info_if_expr(!i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:json_object_new_object fail", __FUNCTION__));

    huawei_jso = json_object_new_object();
    return_val_do_info_if_expr(NULL == huawei_jso, HTTP_INTERNAL_SERVER_ERROR, json_object_put(*o_result_jso);
        *o_result_jso = NULL; debug_log(DLOG_ERROR, "%s:json_object_new_object fail", __FUNCTION__));

    (void)dal_get_object_handle_nth(CLASS_SESSION, 0, &obj_handle);
    (void)dal_get_property_value_int32(obj_handle, PROPERTY_SESSION_TIMEOUT, &timeout);
    property = json_object_new_int(timeout / 60);
    json_object_object_add(huawei_jso, RFPROP_SESSIONSERVICE_WEBTIMEOUT, property);

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_SESSION_MODE, &mode);
    property = json_object_new_string(mode ? PROPVAL_MODE_EXCLUSIVE : PROPVAL_MODE_SHARED);
    json_object_object_add(huawei_jso, RFPROP_SESSIONSERVICE_WEBMODE, property);

    json_object_object_add((*o_result_jso), RFPROP_COMMON_HUAWEI, huawei_jso);

    return HTTP_OK;
}


gint32 SetSessionserviceTimeout(const OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    guint32 timeout;
    gint32 ret;

    
    return_val_do_info_if_fail(obj_handle && input_list, VOS_ERR,
        proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "Set redfish session timeout value failed"));

    timeout = g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 0));

    
    
    

    
    ret = dal_save_property_value_uint32_remote(obj_handle, PROPERTY_REDFISH_SESSION_TIMEOUT, timeout, DF_SAVE);

    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "Set redfish session timeout value failed"));

    proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH,
        "Set redfish session timeout to (%d) seconds successfully", timeout);

    return VOS_OK;
}

LOCAL gint32 set_sessionservice_timeout(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    guint32 timeout;
    GSList *caller_info = NULL;
    GSList *input_list = NULL;
    PROPERTY_PROVIDER_S *self = get_self_provider_session_service(i_paras);

    
    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != self), HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    
    return_val_do_info_if_fail((i_paras->user_role_privilege & USERROLE_BASICSETTING), HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, self->property_name, o_message_jso,
        self->property_name));

    
    ret = dal_get_object_handle_nth(self->pme_class_name, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_NOT_FOUND,
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri));

    
    
    return_val_do_info_if_fail((NULL != i_paras->val_jso), HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_TYPE_ERR, self->property_name, o_message_jso, RF_VALUE_NULL,
        self->property_name));
    
    return_val_do_info_if_fail(json_type_int == json_object_get_type(i_paras->val_jso), HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, self->property_name, o_message_jso,
        dal_json_object_get_str(i_paras->val_jso), self->property_name));
    

    timeout = json_object_get_int(i_paras->val_jso);

    
    
    
    caller_info =
        g_slist_append(caller_info, g_variant_new_string(dal_rf_get_op_log_module_name(i_paras->is_from_webui)));
    
    caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->user_name)),
    caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->client));
    input_list = g_slist_append(input_list, g_variant_new_uint32(timeout));

    
    ret = SetSessionserviceTimeout(obj_handle, caller_info, input_list, NULL);

    
    uip_free_gvariant_list(caller_info);
    uip_free_gvariant_list(input_list);
    

    return_val_if_expr(VOS_OK == ret, HTTP_OK);

    
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    return HTTP_INTERNAL_SERVER_ERROR;
}

LOCAL gint32 set_web_timeout(PROVIDER_PARAS_S *i_paras, json_object *prop, json_object *message_array)
{
    gint32 ret;
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    gint32 timeout;
    json_object *message_jso = NULL;
    const gchar* prop_name = "Oem/Huawei/WebSessionTimeoutMinutes";

    if (NULL == prop) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, RFPROP_SESSIONSERVICE_WEBTIMEOUT, &message_jso, RF_VALUE_NULL,
            RFPROP_SESSIONSERVICE_WEBTIMEOUT);
        (void)json_object_array_add(message_array, message_jso);
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__);
        return VOS_ERR;
    }

    timeout = json_object_get_int(prop);

    (void)dal_get_object_handle_nth(CLASS_SESSION, 0, &obj_handle);
    input_list = g_slist_append(input_list, g_variant_new_int32(timeout * 60));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SESSION, METHOD_SESSION_SETSESSIONTIMEOUT, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);

    switch (ret) {
        case VOS_OK:
            ret = VOS_OK;
            break;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop_name, &message_jso, prop_name);
            (void)json_object_array_add(message_array, message_jso);
            ret = VOS_ERR;
            break;

        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
            (void)json_object_array_add(message_array, message_jso);
            ret = VOS_ERR;
    }

    return ret;
}

LOCAL gint32 set_web_mode(PROVIDER_PARAS_S *i_paras, json_object *prop, json_object *message_array)
{
    gint32 ret;
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    guint8 mode = 0;
    const gchar *mode_str = NULL;
    json_object *message_jso = NULL;
    const gchar* prop_name = "Oem/Huawei/WebSessionMode";

    if (NULL == prop) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, RFPROP_SESSIONSERVICE_WEBMODE, &message_jso, RF_VALUE_NULL,
            RFPROP_SESSIONSERVICE_WEBMODE);
        (void)json_object_array_add(message_array, message_jso);
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__);
        return VOS_ERR;
    }

    mode_str = dal_json_object_get_str(prop);
    if (strcmp(mode_str, PROPVAL_MODE_SHARED) == 0) {
        mode = 0;
    } else if (strcmp(mode_str, PROPVAL_MODE_EXCLUSIVE) == 0) {
        mode = 1;
    }

    (void)dal_get_object_handle_nth(CLASS_SESSION, 0, &obj_handle);
    input_list = g_slist_append(input_list, g_variant_new_byte(mode));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SESSION, METHOD_SESSION_SETSESSIONMODE, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);

    switch (ret) {
        case VOS_OK:
            ret = VOS_OK;
            break;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop_name, &message_jso, prop_name);
            (void)json_object_array_add(message_array, message_jso);
            ret = VOS_ERR;
            break;

        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
            (void)json_object_array_add(message_array, message_jso);
            ret = VOS_ERR;
    }

    return ret;
}


LOCAL gint32 set_sessionservice_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 result = HTTP_BAD_REQUEST;
    json_object *prop = NULL;
    json_bool ret_bool;
    json_object *huawei_jso = NULL;

    return_val_do_info_if_expr((NULL == o_message_jso) || (NULL == o_result_jso) ||
        (VOS_OK != provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__));

    *o_message_jso = json_object_new_array();
    return_val_do_info_if_expr(NULL == *o_message_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : alloc message array mem fail", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    ret_bool = json_object_object_get_ex(i_paras->val_jso, RFPROP_COMMON_HUAWEI, &huawei_jso);
    return_val_if_fail(ret_bool && (NULL != huawei_jso), HTTP_BAD_REQUEST);

    if (json_object_object_get_ex(huawei_jso, RFPROP_SESSIONSERVICE_WEBTIMEOUT, &prop)) {
        if (set_web_timeout(i_paras, prop, *o_message_jso) == RET_OK) {
            result = HTTP_OK;
        }
    }
    prop = NULL;

    if (json_object_object_get_ex(huawei_jso, RFPROP_SESSIONSERVICE_WEBMODE, &prop)) {
        if (set_web_mode(i_paras, prop, *o_message_jso) == RET_OK) {
            result = HTTP_OK;
        }
    }

    return result;
}


LOCAL PROPERTY_PROVIDER_S sessionservice_provider[] = {
    {
        RFPROP_SESSIONSERVICE_TIMEOUT,
        CLASS_REDFISH, PROPERTY_REDFISH_SESSION_TIMEOUT,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        get_sessionservice_timeout,
        set_sessionservice_timeout,
        NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_COMMON_OEM,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        get_sessionservice_oem,
        set_sessionservice_oem,
        NULL, ETAG_FLAG_ENABLE
    }
};

LOCAL PROPERTY_PROVIDER_S *get_self_provider_session_service(PROVIDER_PARAS_S *i_paras)
{
    return_val_if_fail((NULL != i_paras) && (i_paras->index >= 0) &&
        (i_paras->index < (gint32)(sizeof(sessionservice_provider) / sizeof(PROPERTY_PROVIDER_S))),
        (PROPERTY_PROVIDER_S *)NULL);
    return &sessionservice_provider[i_paras->index];
}


gint32 sessionservice_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    
    return_val_if_fail(NULL != i_paras && NULL != prop_provider && NULL != count, HTTP_INTERNAL_SERVER_ERROR);
    *prop_provider = sessionservice_provider;
    *count = sizeof(sessionservice_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}
