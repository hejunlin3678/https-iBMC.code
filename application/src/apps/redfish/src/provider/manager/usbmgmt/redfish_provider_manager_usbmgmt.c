
#include "redfish_provider.h"

static gint32 get_manager_usbmgmt_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
static gint32 get_property_usbmgmt_service_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
static gint32 get_property_usbmgmt_usb_device_presence(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
static gint32 set_property_usbmgmt_service_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_usbmgmt_uncompress_password(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, 
    json_object** o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_manager_usbmgmt_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, 
        get_manager_usbmgmt_odata_id, NULL, NULL, ETAG_FLAG_ENABLE
    },
    {RFPROP_MANAGER_USBMGMT_SERVICE_ENABLED, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, 
        SYS_LOCKDOWN_FORBID, get_property_usbmgmt_service_enabled, set_property_usbmgmt_service_enabled, 
        NULL, ETAG_FLAG_ENABLE
    },
    {RFPROP_MANAGER_USBMGMT_USB_DEVICE_PRESENCE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, 
        SYS_LOCKDOWN_NULL, get_property_usbmgmt_usb_device_presence, NULL, NULL, ETAG_FLAG_ENABLE
    },
    {RFPROP_MANAGER_USBMGMT_USB_UNCOMPRESS_PASSWORD, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, 
        SYS_LOCKDOWN_FORBID, NULL, set_usbmgmt_uncompress_password, NULL, ETAG_FLAG_ENABLE
    }
};


static gint32 get_manager_usbmgmt_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    ret = get_odata_id(i_paras, o_message_jso, o_result_jso, USBMGMT_URI_ODATAID);
    if (ret != HTTP_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


static gint32 get_property_usbmgmt_service_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guchar is_enabled = FALSE;

    
    ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        return ret;
    }

    ret = dal_get_object_handle_nth(CLASS_USB_MGMT, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : get obj %s failed, ret=%d", __FUNCTION__, CLASS_USB_MGMT, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_USB_MGMT_ENABLE, &is_enabled);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : get %s value failed, ret=%d", __FUNCTION__, PROPERTY_USB_MGMT_ENABLE, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    *o_result_jso = json_object_new_boolean(is_enabled);
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s : json_object_new_boolean fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return HTTP_OK;
}


static gint32 get_property_usbmgmt_usb_device_presence(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guchar device_status = 0;

    
    ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        return ret;
    }

    ret = dal_get_object_handle_nth(CLASS_USB_MGMT, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : get obj %s failed, ret=%d", __FUNCTION__, CLASS_USB_MGMT, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_USB_MGMT_DEVICE_STATUS, &device_status);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : get %s value failed, ret=%d", __FUNCTION__, PROPERTY_USB_MGMT_DEVICE_STATUS, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    *o_result_jso = json_object_new_boolean(((device_status == 0) ? FALSE : TRUE));
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s : json_object_new_boolean fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return HTTP_OK;
}


static gint32 set_property_usbmgmt_service_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint32 len;
    OBJ_HANDLE obj_handle = 0;
    gchar value_log[MAX_RSC_ID_LEN] = { 0 };
    json_bool property_state;
    GSList *input_list = NULL;

    
    ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        return ret;
    }
    
    property_state = json_object_get_boolean(i_paras->val_jso);

    ret = dal_get_object_handle_nth(CLASS_USB_MGMT, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : get %s obj fail, ret=%d", __FUNCTION__, CLASS_USB_MGMT, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    input_list = g_slist_append(input_list, g_variant_new_byte((guchar)property_state));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_USB_MGMT, METHOD_USB_MGMT_SET_ENABLE, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    
    len = snprintf_s(value_log, sizeof(value_log), sizeof(value_log) - 1, "%d", property_state);
    if (len <= 0) {
        debug_log(DLOG_ERROR, "%s : snprintf_s fail, ret=%d", __FUNCTION__, ret);
    }

    return check_redfish_call_method_result(ret, o_message_jso, RFPROP_MANAGER_USBMGMT_SERVICE_ENABLED, value_log);
}


gint32 is_support_usbmgmt_service(void)
{
    gint32 ret;
    guchar is_support = FALSE;
    guchar is_present = FALSE;
    OBJ_HANDLE obj_handle = 0;

    ret = dal_get_object_handle_nth(CLASS_USB_MGMT, 0, &obj_handle);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_USB_MGMT_SUPPORT, &is_support);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : get %s value failed, ret=%d", __FUNCTION__, PROPERTY_USB_MGMT_SUPPORT, ret);
        return RET_ERR;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_USB_MGMT_PRESENCE, &is_present);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : get %s value failed, ret=%d", __FUNCTION__, PROPERTY_USB_MGMT_PRESENCE, ret);
        return RET_ERR;
    }
    if (is_support != TRUE || is_present != TRUE) {
        debug_log(DLOG_DEBUG, "%s : is_support=%d, is_present=%d", __FUNCTION__, is_support, is_present);
        return RET_ERR;
    }

    return RET_OK;
}

LOCAL gint32 check_set_uncompress_pwd_pri(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, 
    json_object** o_result_jso)
{
    
    gint32 ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: check param and privi failed", __FUNCTION__);
        return ret;
    }

    // 管理员权限检才能进行设置
    if (((i_paras->user_role_privilege) & USERROLE_USERMGNT) == 0) {
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_MANAGER_USBMGMT_USB_UNCOMPRESS_PASSWORD, 
            o_message_jso, RFPROP_MANAGER_USBMGMT_USB_UNCOMPRESS_PASSWORD);
        return HTTP_FORBIDDEN;
    }

    return RET_OK;
}


LOCAL gint32 set_usbmgmt_uncompress_password(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, 
    json_object** o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;

    
    gint32 ret = check_set_uncompress_pwd_pri(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        json_string_clear(i_paras->val_jso);
        debug_log(DLOG_ERROR, "%s: check set uncompress pwd pri failed", __FUNCTION__);
        return ret;
    }

    ret = dal_get_object_handle_nth(CLASS_USB_MGMT, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : get obj %s failed, ret=%d", __FUNCTION__, CLASS_USB_MGMT, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        json_string_clear(i_paras->val_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    const gchar *uncompress_password = dal_json_object_get_str(i_paras->val_jso);
    if (uncompress_password == NULL) {
        debug_log(DLOG_ERROR, "%s:dal_json_object_get_str uncompress_password fail.", __FUNCTION__);
        json_string_clear(i_paras->val_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(uncompress_password));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_USB_MGMT, METHOD_USB_MGMT_UNCOMPRESS_PASSWORD, AUTH_ENABLE, i_paras->user_role_privilege, 
        input_list, NULL);

    uip_free_gvariant_list(input_list);
    json_string_clear(i_paras->val_jso);

    switch (ret) {
        case RET_OK:
            return HTTP_OK;

        case USB_PWD_INVALID_LEN:
            (void)create_message_info(MSGID_INVALID_PSWD_LEN, RFPROP_MANAGER_USBMGMT_USB_UNCOMPRESS_PASSWORD, 
                o_message_jso, USB_UNCOMPRESS_PASSWORD_LEN_MIN, USB_UNCOMPRESS_PASSWORD_LEN_MAX);
            return HTTP_BAD_REQUEST;

        default:
            debug_log(DLOG_ERROR, "%s: Call method %s failed(ret = %d)", __FUNCTION__, 
                METHOD_USB_MGMT_UNCOMPRESS_PASSWORD, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


gint32 manager_usbmgmt_provider_entry(PROVIDER_PARAS_S* i_paras, PROPERTY_PROVIDER_S** prop_provider, guint32* count)
{
    gint32 ret;

    
    if ((i_paras == NULL) || (prop_provider == NULL) || (count == NULL)) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = is_support_usbmgmt_service();
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: cannot support usbmgmt service ", __FUNCTION__);
        return HTTP_NOT_FOUND;
    }

    
    ret = redfish_check_manager_uri_valid(i_paras->uri);
    if (ret != TRUE) {
        debug_log(DLOG_DEBUG, "%s: invalid uri is %s", __FUNCTION__, i_paras->uri);
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_manager_usbmgmt_provider;
    *count = sizeof(g_manager_usbmgmt_provider) / sizeof(PROPERTY_PROVIDER_S);
    return RET_OK;
}
