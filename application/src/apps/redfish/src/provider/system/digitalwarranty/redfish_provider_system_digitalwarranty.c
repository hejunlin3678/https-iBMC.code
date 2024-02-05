

#include "redfish_provider.h"
#include "redfish_provider_system.h"

LOCAL gint32 get_digital_warranty_property_str(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);


LOCAL gint32 get_digital_warranty_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return get_odata_id(i_paras, o_message_jso, o_result_jso, DIGITAL_WARRANTY_ODATAID);
}


LOCAL gint32 get_digital_warranty_unit_type(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    
    ret = rf_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: rf_check_para_and_priv failed, ret= %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    *o_result_jso = json_object_new_string("Device");
    return HTTP_OK;
}


LOCAL gint32 get_digital_warranty_manufacture_date(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gchar value_string[PROP_VAL_MAX_LENGTH + 1] = {0};
    gulong timestamp = 0;
    gint16 time_offset = 0;

    
    ret = rf_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: rf_check_para_and_priv failed, ret= %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = dal_get_object_handle_nth(CLASS_ELABEL, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get object handle failed, ret= %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = dal_get_property_value_string(obj_handle, PROPERTY_ELABEL_BOARD_MFG_TIME, value_string, sizeof(value_string));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get string property failed, ret= %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (rf_string_check(value_string) != RET_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = dal_parse_manufacture_date_to_stamp(value_string, &timestamp);
    if (ret == RET_OK) {
        ret = dal_get_localtime_offset(&time_offset);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: get localtime offset failed, ret= %d.", __FUNCTION__, ret);
            return HTTP_INTERNAL_SERVER_ERROR;
        }

        memset_s(value_string, sizeof(value_string), 0, sizeof(value_string));
        (void)redfish_fill_format_time(value_string, sizeof(value_string), (guint32 *)&timestamp, &time_offset);
    }

    
    *o_result_jso = json_object_new_string((const gchar *)value_string);
    return HTTP_OK;
}


LOCAL gint32 get_digital_warranty_uuid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return redfish_get_uuid(i_paras, o_message_jso, o_result_jso);
}


LOCAL gint32 get_digital_warranty_lifespan(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint8 property_value = 0;
    gchar value_string[PROP_VAL_MAX_LENGTH + 1] = {0};

    
    ret = rf_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: rf_check_para_and_priv failed, ret= %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_object_handle_nth(CLASS_NAME_PRODUCT, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get object handle failed, ret= %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = dal_get_property_value_byte(obj_handle, BMC_PRODUCT_LIFESPAN, &property_value);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get byte property failed, ret= %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_property_value_string(obj_handle, BMC_PRODUCT_STARTPOINT, value_string, sizeof(value_string));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get string property failed, ret= %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = dal_get_property_value_byte(obj_handle, BMC_PRODUCT_LIFESPAN, &property_value);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get byte property failed, ret= %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (property_value == 0) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    *o_result_jso = json_object_new_int(property_value);
    return HTTP_OK;
}

LOCAL gint32 check_digital_warranty_method_result(const gint32 ret, const gchar *prop_name, const gchar *set_str,
    json_object **o_message_jso)
{
    
    switch (ret) {
        case RET_OK:
        case RFERR_SUCCESS:
            return HTTP_OK;

        case RFERR_WRONG_PARAM: 
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, prop_name, o_message_jso, set_str, prop_name);
            return HTTP_BAD_REQUEST;

        case RFERR_INSUFFICIENT_PRIVILEGE: 
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop_name, o_message_jso, prop_name);
            return HTTP_FORBIDDEN;

        default: 
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            debug_log(DLOG_ERROR, "%s: call method fail %d.", __FUNCTION__, ret);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 set_digital_warranty_startpoint(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    const gchar *set_str = NULL;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;

    
    ret = rf_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: rf_check_para_and_priv failed, ret= %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (json_object_get_type(i_paras->val_jso) != json_type_string) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, RFPROP_DIGITAL_WARRANTY_STARTPOINT, o_message_jso,
            dal_json_object_get_str(i_paras->val_jso), RFPROP_DIGITAL_WARRANTY_STARTPOINT);
        return HTTP_BAD_REQUEST;
    }

    
    set_str = dal_json_object_get_str(i_paras->val_jso);
    if (set_str == NULL) {
        debug_log(DLOG_ERROR, "%s: dal_json_object_get_str failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = dal_get_object_handle_nth(CLASS_NAME_PRODUCT, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get object handle failed, ret= %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(set_str));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_NAME_PRODUCT, METHOD_PRODUCT_SET_STARTPOINT, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    return check_digital_warranty_method_result(ret, RFPROP_DIGITAL_WARRANTY_STARTPOINT, set_str, o_message_jso);
}


LOCAL gint32 set_digital_warranty_lifespan(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint32 set_value;
    gchar value_log[MAX_RSC_ID_LEN] = { 0 };
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;

    
    ret = rf_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: rf_check_para_and_priv failed, ret= %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (json_object_get_type(i_paras->val_jso) != json_type_int) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, RFPROP_DIGITAL_WARRANTY_LIFESPAN, o_message_jso,
            dal_json_object_get_str(i_paras->val_jso), RFPROP_DIGITAL_WARRANTY_LIFESPAN);
        return HTTP_BAD_REQUEST;
    }

    
    set_value = json_object_get_int(i_paras->val_jso);

    
    ret = dal_get_object_handle_nth(CLASS_NAME_PRODUCT, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get object handle failed, ret= %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)set_value));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_NAME_PRODUCT, METHOD_PRODUCT_SET_LIFESPAN, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    
    // sizeof(value_log)大小为64，大于格式化后字符串长度，无需判断返回值
    (void)snprintf_truncated_s(value_log, sizeof(value_log), "%d", set_value);

    return check_digital_warranty_method_result(ret, RFPROP_DIGITAL_WARRANTY_LIFESPAN, (const gchar *)value_log,
        o_message_jso);
}

LOCAL PROPERTY_PROVIDER_S g_system_digital_warranty_provider[] = {
    { RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_digital_warranty_odata_id, NULL, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_DIGITAL_WARRANTY_PRODUCTNAME, CLASS_NAME_BMC, PROPERTY_BMC_SYSTEMNAME, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_digital_warranty_property_str, NULL, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_DIGITAL_WARRANTY_SN, CLASS_ELABEL, PROPERTY_ELABEL_SYS_SERIAL_NUM, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_digital_warranty_property_str, NULL, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_DIGITAL_WARRANTY_MANUFACTUREDATE, CLASS_ELABEL, PROPERTY_ELABEL_BOARD_MFG_TIME, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_digital_warranty_manufacture_date, NULL, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_DIGITAL_WARRANTY_UUID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_digital_warranty_uuid, NULL, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_DIGITAL_WARRANTY_UNITTYPE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_digital_warranty_unit_type, NULL, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_DIGITAL_WARRANTY_STARTPOINT, CLASS_NAME_PRODUCT, BMC_PRODUCT_STARTPOINT, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_digital_warranty_property_str, set_digital_warranty_startpoint, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_DIGITAL_WARRANTY_LIFESPAN, CLASS_NAME_PRODUCT, BMC_PRODUCT_LIFESPAN, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_digital_warranty_lifespan, set_digital_warranty_lifespan, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_DIGITAL_WARRANTY_FIRST_POWER_ON_TIME, CLASS_NAME_PRODUCT, BMC_PRODUCT_FIRSTPOWERONTIME, USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID, get_digital_warranty_property_str, NULL, NULL, ETAG_FLAG_ENABLE}
};

LOCAL gint32 get_digital_warranty_property_str(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gchar value_string[PROP_VAL_MAX_LENGTH + 1] = {0};
    PROPERTY_PROVIDER_S *self = NULL;

    
    ret = rf_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: rf_check_para_and_priv failed, ret= %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    self = &g_system_digital_warranty_provider[i_paras->index];

    
    ret = dal_get_object_handle_nth(self->pme_class_name, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get object handle failed, ret= %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = dal_get_property_value_string(obj_handle, self->pme_prop_name, value_string, sizeof(value_string));
    if (ret != RET_OK) {
        debug_log(DLOG_MASS, "%s: get string property failed, ret= %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (rf_string_check(value_string) != RET_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    *o_result_jso = json_object_new_string((const gchar *)value_string);
    return HTTP_OK;
}


gint32 system_digital_warranty_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    
    if (i_paras == NULL || prop_provider == NULL || count == NULL) {
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (redfish_check_system_uri_valid(i_paras->uri) == FALSE) {
        debug_log(DLOG_ERROR, "%s: invalid uri is %s", __FUNCTION__, i_paras->uri);
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_system_digital_warranty_provider;
    *count = G_N_ELEMENTS(g_system_digital_warranty_provider);
    return VOS_OK;
}
