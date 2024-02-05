
#include "redfish_provider.h"
#include "redfish_util.h"

LOCAL gint32 get_rack_data_acquisition_config_data(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_rack_data_acquisition_config_data(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_rack_dataacquisition_cfg_provider[] = {
    
    {
        PROP_RACK_DA_CFG_CONTENT,
        MAP_PROPERTY_NULL,
        MAP_PROPERTY_NULL,
        USERROLE_DIAGNOSEMGNT,
        SYS_LOCKDOWN_FORBID,
        get_rack_data_acquisition_config_data,
        set_rack_data_acquisition_config_data,
        NULL,
        ETAG_FLAG_ENABLE
    }
    
};


LOCAL gint32 get_rack_data_acquisition_config_data(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE rack_mgnt_hnd;
    GSList *output_list = NULL;
    const gchar *resp_str = NULL;

    
    if (VOS_OK != provider_paras_check(i_paras) || NULL == o_message_jso || NULL == o_result_jso) {
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (0 == (i_paras->user_role_privilege & USERROLE_READONLY)) {
        debug_log(DLOG_ERROR, "%s failed:insufficient privilege", __FUNCTION__);
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, PROP_RACK_DA_CFG_CONTENT, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = dal_get_object_handle_nth(CLASS_RACK_MANAGEMENT, 0, &rack_mgnt_hnd);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get object handler failed, ret=%d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, rack_mgnt_hnd,
        CLASS_RACK_MANAGEMENT, METHOD_GET_RACK_DA_CFG, AUTH_ENABLE, i_paras->user_role_privilege, NULL, &output_list);
    if (ret != RET_OK || output_list == NULL) {
        debug_log(DLOG_ERROR, "%s: call method %s failed, ret=%d", __FUNCTION__, METHOD_GET_RACK_DA_CFG, ret);
        uip_free_gvariant_list(output_list);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    resp_str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    *o_result_jso = json_tokener_parse(resp_str);
    uip_free_gvariant_list(output_list);

    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_tokener_parse fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 set_rack_data_acquisition_config_data(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet = -1;
    gint32 ret;
    gint32 da_device_num;
    gint32 cfg_idx;
    gint32 item_idx = 0;
    json_object *dev_da_jso = NULL;
    json_object *ser_num_jso = NULL;
    json_object *da_items_jso = NULL;
    json_object *da_item_jso = NULL;
    gint32 da_items_num = 0;
    OBJ_HANDLE rack_mgnt_hnd;
    const gchar *ser_num = NULL;
    const gchar *rack_da_cfg_jso_str = NULL;
    GSList *input_list = NULL;
    gchar           err_msg[TMP_STR_LEN + 1] = {0};

    
    return_val_do_info_if_fail((NULL != i_paras->val_jso), HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "null type value no supported to config");
        (void)create_message_info(MSGID_PROP_TYPE_ERR, PROP_RACK_DA_CFG_CONTENT, o_message_jso, RF_NULL_STR,
        PROP_RACK_DA_CFG_CONTENT));

    
    if (0 == (i_paras->user_role_privilege & USERROLE_DIAGNOSEMGNT)) {
        debug_log(DLOG_ERROR, "%s failed:insufficient privilege", __FUNCTION__);

        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, PROP_RACK_DA_CFG_CONTENT, o_message_jso,
            PROP_RACK_DA_CFG_CONTENT);

        return HTTP_FORBIDDEN;
    }

    
    da_device_num = json_object_array_length(i_paras->val_jso);

    for (cfg_idx = 0; cfg_idx < da_device_num; cfg_idx++) {
        dev_da_jso = json_object_array_get_idx(i_paras->val_jso, cfg_idx);
        
        if (TRUE != json_object_object_get_ex(dev_da_jso, RFPROP_SERIAL_NUMBER, &ser_num_jso)) {
            debug_log(DLOG_ERROR, "%s: property %s not found", __FUNCTION__, PROPERTY_DEVICE_INFO_SER_NUM);
            iRet = snprintf_s(err_msg, sizeof(err_msg), sizeof(err_msg) - 1, "%d/%s", cfg_idx, RFPROP_SERIAL_NUMBER);
            do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
            (void)create_message_info(MSGID_PROP_MISSING, err_msg, o_message_jso, RFPROP_SERIAL_NUMBER);
            return HTTP_BAD_REQUEST;
        }

        ser_num = dal_json_object_get_str(ser_num_jso);

        
        if (TRUE != json_object_object_get_ex(dev_da_jso, PROP_RACK_DA_ITEMS, &da_items_jso)) {
            debug_log(DLOG_ERROR, "%s: property %s not found", __FUNCTION__, PROP_RACK_DA_ITEMS);
            iRet = snprintf_s(err_msg, sizeof(err_msg), sizeof(err_msg) - 1, "%d/%s", cfg_idx, PROP_RACK_DA_ITEMS);
            do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
            (void)create_message_info(MSGID_PROP_MISSING, err_msg, o_message_jso, PROP_RACK_DA_ITEMS);
            return HTTP_BAD_REQUEST;
        }

        
        da_items_num = json_object_array_length(da_items_jso);

        for (item_idx = 0; item_idx < da_items_num; item_idx++) {
            da_item_jso = json_object_array_get_idx(da_items_jso, item_idx);
            if (json_object_get_string_len(da_item_jso) > MAX_URI_LENGTH) {
                debug_log(DLOG_ERROR, "%s: device(%s) da item array member[%d] length invalid", __FUNCTION__, ser_num,
                    item_idx);
                iRet = snprintf_s(err_msg, sizeof(err_msg), sizeof(err_msg) - 1, "%d/%s/%d", cfg_idx,
                    PROP_RACK_DA_ITEMS, item_idx);
                do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
                (void)create_message_info(MSGID_PROP_VAL_EXC_MAXLEN, err_msg, o_message_jso,
                    dal_json_object_get_str(da_item_jso), PROP_RACK_DA_ITEMS, MAX_URI_LENGTH_STR);
                return HTTP_BAD_REQUEST;
            }
        }
    }

    ret = dal_get_object_handle_nth(CLASS_RACK_MANAGEMENT, 0, &rack_mgnt_hnd);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: %s object handler not found", __FUNCTION__, CLASS_RACK_MANAGEMENT));

    rack_da_cfg_jso_str = dal_json_object_get_str(i_paras->val_jso);

    
    input_list = g_slist_append(input_list, g_variant_new_string(rack_da_cfg_jso_str));

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, rack_mgnt_hnd,
        CLASS_RACK_MANAGEMENT, METHOD_UPDATE_RACK_DA_CFG, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);

    uip_free_gvariant_list(input_list);

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
            debug_log(DLOG_ERROR, "%s: Update rack data acquisition config failed. ret=%d\n", __FUNCTION__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


gint32 rack_data_acquisition_cfg_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret;
    OBJ_HANDLE rack_mgnt_hd;

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    ret = dal_get_object_handle_nth(CLASS_RACK_MANAGEMENT, 0, &rack_mgnt_hd);
    return_val_if_fail(VOS_OK == ret, HTTP_NOT_FOUND);

    *prop_provider = g_rack_dataacquisition_cfg_provider;
    *count = sizeof(g_rack_dataacquisition_cfg_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}
