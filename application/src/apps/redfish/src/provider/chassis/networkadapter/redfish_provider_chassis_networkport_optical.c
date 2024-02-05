

#include "redfish_provider.h"

LOCAL gint32 _get_chassis_opticalmodule_odata_uri(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _get_chassis_opticalmodule_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _redfish_get_property_string(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _redfish_get_property_object(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _redfish_get_property_array(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _get_chassis_opticalmodule_speed(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _redfish_get_opticalmodule_wave_length(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _redfish_get_opticalmodule_transceiver_type(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _redfish_get_opticalmodule_medium_mode(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _redfish_get_opticalmodule_production_date(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _redfish_get_property_uint16(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _redfish_get_property_boolean(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _get_chassis_direct_opticalmodule_list_odata_uri(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso);
LOCAL gint32 _get_chassis_direct_opticalmodule_list_members_uri(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso);
LOCAL gint32 _get_chassis_direct_opticalmodule_odata_uri(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_chassis_opticalmodule_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        _get_chassis_opticalmodule_odata_uri, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        _get_chassis_opticalmodule_odata_uri, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_STATUS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY,
        SYS_LOCKDOWN_NULL, _get_chassis_opticalmodule_status, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OPTICAL_MODULE_MANUFACTURER, CLASS_OPTICAL_MODULE, PROPERTY_OPT_MDL_VENDOR_NAME,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, _redfish_get_property_string, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OPTICAL_MODULE_PARTNUMBER, CLASS_OPTICAL_MODULE, PROPERTY_OPT_MDL_PART_NUMBER, USERROLE_READONLY,
        SYS_LOCKDOWN_NULL, _redfish_get_property_string, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OPTICAL_MODULE_SERIALNUMBER, CLASS_OPTICAL_MODULE, PROPERTY_OPT_MDL_SERIAL_NUMBER,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, _redfish_get_property_string, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OPTICAL_MODULE_PRODUCTIONDATE, CLASS_OPTICAL_MODULE, PROPERTY_OPT_MDL_MANUFACTURE_DATE, USERROLE_READONLY,
        SYS_LOCKDOWN_NULL, _redfish_get_opticalmodule_production_date, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OPTICAL_MODULE_TYPE, CLASS_OPTICAL_MODULE, PROPERTY_OPT_MDL_TRANSCEIVER_TYPE, USERROLE_READONLY,
        SYS_LOCKDOWN_NULL, _redfish_get_opticalmodule_transceiver_type, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OPTICAL_MODULE_PACKAGING_TYPE, CLASS_OPTICAL_MODULE, PROPERTY_OPT_MDL_TYPE, USERROLE_READONLY,
        SYS_LOCKDOWN_NULL, _redfish_get_property_string, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OPTICAL_MODULE_WAVE_LENGTH, CLASS_OPTICAL_MODULE, PROPERTY_OPT_MDL_WAVE_LENGTH,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, _redfish_get_opticalmodule_wave_length, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OPTICAL_MODULE_MEDIUMMODE, CLASS_OPTICAL_MODULE, PROPERTY_OPT_MDL_MEDIUM_MODE,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, _redfish_get_opticalmodule_medium_mode, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OPTICAL_MODULE_SPEED, CLASS_OPTICAL_MODULE, PROPERTY_OPT_MDL_SPEED,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, _get_chassis_opticalmodule_speed, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OPTICAL_MODULE_VOLTAGE, CLASS_OPTICAL_MODULE, MAP_PROPERTY_NULL, USERROLE_READONLY,
        SYS_LOCKDOWN_NULL, _redfish_get_property_object, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OPTICAL_MODULE_RXPOWER, CLASS_OPTICAL_MODULE, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        _redfish_get_property_array, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OPTICAL_MODULE_TXPOWER, CLASS_OPTICAL_MODULE, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        _redfish_get_property_array, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OPTICAL_MODULE_INDENTIFIER, CLASS_OPTICAL_MODULE, PROPERTY_OPT_MDL_INDENTIFIER,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, _redfish_get_property_string, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OPTICAL_MODULE_DEVICE_TYPE, CLASS_OPTICAL_MODULE, PROPERTY_OPT_MDL_DEVICE_TYPE,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, _redfish_get_property_string, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OPTICAL_MODULE_CONNECTOR_TYPE, CLASS_OPTICAL_MODULE, PROPERTY_OPT_MDL_CONNECTOR_TYPE,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, _redfish_get_property_string, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OPTICAL_MODULE_TRANSFER_DISTANCE, CLASS_OPTICAL_MODULE, PROPERTY_OPT_MDL_TRANSFER_DISTANCE,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, _redfish_get_property_uint16, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OPTICAL_MODULE_RXLOS_STATE, CLASS_OPTICAL_MODULE, PROPERTY_OPT_MDL_RX_LOS_STATE, USERROLE_READONLY,
        SYS_LOCKDOWN_NULL, _redfish_get_property_boolean, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OPTICAL_MODULE_TXFAULT_STATE, CLASS_OPTICAL_MODULE, PROPERTY_OPT_MDL_TX_Fult_STATE, USERROLE_READONLY,
        SYS_LOCKDOWN_NULL, _redfish_get_property_boolean, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OPTICAL_MODULE_TEMPERATURE, CLASS_OPTICAL_MODULE, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        _redfish_get_property_object, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OPTICAL_MODULE_BIAS, CLASS_OPTICAL_MODULE, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, _redfish_get_property_array, NULL, NULL, ETAG_FLAG_ENABLE}
};

LOCAL PROPERTY_PROVIDER_S g_chassis_direct_opticalmodule_list__provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        _get_chassis_direct_opticalmodule_list_odata_uri, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY,
        SYS_LOCKDOWN_NULL, _get_chassis_direct_opticalmodule_list_odata_uri, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS_COUNT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        _get_chassis_direct_opticalmodule_list_members_uri, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY,
        SYS_LOCKDOWN_NULL, _get_chassis_direct_opticalmodule_list_members_uri, NULL, NULL, ETAG_FLAG_ENABLE},
};

LOCAL PROPERTY_PROVIDER_S g_chassis_direct_opticalmodule_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY,
        SYS_LOCKDOWN_NULL, _get_chassis_direct_opticalmodule_odata_uri, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY,
        SYS_LOCKDOWN_NULL, _get_chassis_direct_opticalmodule_odata_uri, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_STATUS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        _get_chassis_opticalmodule_status, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OPTICAL_MODULE_MANUFACTURER, CLASS_OPTICAL_MODULE, PROPERTY_OPT_MDL_VENDOR_NAME, USERROLE_READONLY,
        SYS_LOCKDOWN_NULL, _redfish_get_property_string, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OPTICAL_MODULE_PARTNUMBER, CLASS_OPTICAL_MODULE, PROPERTY_OPT_MDL_PART_NUMBER,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, _redfish_get_property_string, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OPTICAL_MODULE_SERIALNUMBER, CLASS_OPTICAL_MODULE, PROPERTY_OPT_MDL_SERIAL_NUMBER,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, _redfish_get_property_string, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OPTICAL_MODULE_PRODUCTIONDATE, CLASS_OPTICAL_MODULE, PROPERTY_OPT_MDL_MANUFACTURE_DATE, USERROLE_READONLY,
        SYS_LOCKDOWN_NULL, _redfish_get_opticalmodule_production_date, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OPTICAL_MODULE_TYPE, CLASS_OPTICAL_MODULE, PROPERTY_OPT_MDL_TRANSCEIVER_TYPE, USERROLE_READONLY,
        SYS_LOCKDOWN_NULL, _redfish_get_opticalmodule_transceiver_type, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OPTICAL_MODULE_PACKAGING_TYPE, CLASS_OPTICAL_MODULE, PROPERTY_OPT_MDL_TYPE,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, _redfish_get_property_string, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OPTICAL_MODULE_WAVE_LENGTH, CLASS_OPTICAL_MODULE, PROPERTY_OPT_MDL_WAVE_LENGTH,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, _redfish_get_opticalmodule_wave_length, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OPTICAL_MODULE_MEDIUMMODE, CLASS_OPTICAL_MODULE, PROPERTY_OPT_MDL_MEDIUM_MODE, USERROLE_READONLY,
        SYS_LOCKDOWN_NULL, _redfish_get_opticalmodule_medium_mode, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OPTICAL_MODULE_SPEED, CLASS_OPTICAL_MODULE, PROPERTY_OPT_MDL_SPEED, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        _get_chassis_opticalmodule_speed, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OPTICAL_MODULE_VOLTAGE, CLASS_OPTICAL_MODULE, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        _redfish_get_property_object, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OPTICAL_MODULE_RXPOWER, CLASS_OPTICAL_MODULE, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, _redfish_get_property_array, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OPTICAL_MODULE_TXPOWER, CLASS_OPTICAL_MODULE, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, _redfish_get_property_array, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OPTICAL_MODULE_INDENTIFIER, CLASS_OPTICAL_MODULE, PROPERTY_OPT_MDL_INDENTIFIER,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, _redfish_get_property_string, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OPTICAL_MODULE_DEVICE_TYPE, CLASS_OPTICAL_MODULE, PROPERTY_OPT_MDL_DEVICE_TYPE, USERROLE_READONLY,
        SYS_LOCKDOWN_NULL, _redfish_get_property_string, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OPTICAL_MODULE_CONNECTOR_TYPE, CLASS_OPTICAL_MODULE, PROPERTY_OPT_MDL_CONNECTOR_TYPE, USERROLE_READONLY,
        SYS_LOCKDOWN_NULL, _redfish_get_property_string, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OPTICAL_MODULE_TRANSFER_DISTANCE, CLASS_OPTICAL_MODULE, PROPERTY_OPT_MDL_TRANSFER_DISTANCE,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, _redfish_get_property_uint16, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OPTICAL_MODULE_RXLOS_STATE, CLASS_OPTICAL_MODULE, PROPERTY_OPT_MDL_RX_LOS_STATE,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, _redfish_get_property_boolean, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OPTICAL_MODULE_TXFAULT_STATE, CLASS_OPTICAL_MODULE, PROPERTY_OPT_MDL_TX_Fult_STATE,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, _redfish_get_property_boolean, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OPTICAL_MODULE_TEMPERATURE, CLASS_OPTICAL_MODULE, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, _redfish_get_property_object, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OPTICAL_MODULE_BIAS, CLASS_OPTICAL_MODULE, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, _redfish_get_property_array, NULL, NULL, ETAG_FLAG_ENABLE}
};


LOCAL gint32 _get_direct_opticalmodule_odate_prop(OBJ_HANDLE obj_handle, const gchar* property_name,
    gchar* uri, guint32 uri_len)
{
    gint32 ret;
    guint8 silk_num = 0;
    OBJ_HANDLE port_obj = 0;
    gchar slot_id[MAX_RSC_ID_LEN] = {0};

    if (property_name == NULL || uri == NULL || uri_len <= 1) {
        debug_log(DLOG_ERROR, "input parameters error");
        return RET_ERR;
    }

    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get slot id fail. ret(%d)", ret);
        return ret;
    }

    ret = dfl_get_referenced_object(obj_handle, PROPERTY_OPT_MDL_REF_PORT_OBJ, &port_obj); 
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get opticalmodule(%s) referenced port object fail. ret(%d)",
            dfl_get_object_name(obj_handle), ret);
        return ret;
    }
    (void)dal_get_property_value_byte(port_obj, PROPERTY_PORT_ADAPTER_PORT_NO, &silk_num);

    if (!g_strcmp0(property_name, RFPROP_ODATA_ID)) {
        ret = snprintf_s(uri, uri_len, uri_len - 1, URI_FORMAT_CHASSIS_DIRECT_OPTICAL_MODULE, slot_id, silk_num);
    } else if (!g_strcmp0(property_name, RFPROP_ODATA_CONTEXT)) {
        ret = snprintf_s(uri, uri_len, uri_len - 1, CHASSIS_DIRECT_OPTICAL_MODULE_METADATA, slot_id, silk_num);
    } else {
        return RET_ERR;
    }
    
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "Snprintf_s fail, ret(%d)", ret);
        return ret;
    }
    return RET_OK;
}


LOCAL gint32 _get_opticalmodule_odata_prop(OBJ_HANDLE obj_handle, const gchar *uri_template, gchar *uri,
    guint32 uri_len)
{
    int iRet;
    gint32 ret;
    gchar slot_id[MAX_RSC_ID_LEN] = {0};
    OBJ_HANDLE businessport_obj = 0;
    OBJ_HANDLE netcard_obj = 0;
    guint8 silk_num = 0;
    gchar member_id[MAX_RSC_ID_LEN] = {0};
    OBJ_HANDLE component_obj = 0;
    gchar card_location[PROP_VAL_LENGTH] = {0};
    gchar device_name[PROP_VAL_LENGTH] = {0};

    return_val_do_info_if_expr((NULL == uri_template) || (NULL == uri), VOS_ERR,
        debug_log(DLOG_ERROR, "%s :NULL pointer", __FUNCTION__));

    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, debug_log(DLOG_ERROR, "%s :get slot id fail.", __FUNCTION__));

    // 获取光模块关联的businessport句柄
    ret = dfl_get_referenced_object(obj_handle, PROPERTY_OPT_MDL_REF_PORT_OBJ, &businessport_obj);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s :get referenced businessport object fail.", __FUNCTION__));
    (void)dal_get_property_value_byte(businessport_obj, BUSY_ETH_ATTRIBUTE_SILK_NUM, &silk_num);

    // 获取businessport关联的netcard对象句柄
    ret = dfl_get_referenced_object(businessport_obj, BUSY_ETH_ATTRIBUTE_REF_NETCARD, &netcard_obj);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s :get referenced netcard object fail.", __FUNCTION__));

    // 获取网卡关联的部件类
    ret = dfl_get_referenced_object(netcard_obj, PROPERTY_NETCARD_REF_COMPONENT, &component_obj);
    return_val_do_info_if_fail(DFL_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get referenced component object fail", __FUNCTION__));
    (void)dal_get_property_value_string(component_obj, PROPERTY_COMPONENT_LOCATION, card_location, PROP_VAL_LENGTH);
    (void)dal_get_property_value_string(component_obj, PROPERTY_COMPONENT_DEVICE_NAME, device_name, PROP_VAL_LENGTH);
    (void)dal_clear_string_blank(card_location, sizeof(card_location));
    (void)dal_clear_string_blank(device_name, sizeof(device_name));
    iRet = snprintf_s(member_id, sizeof(member_id), sizeof(member_id) - 1, "%s%s", card_location, device_name);
    return_val_do_info_if_expr(iRet <= 0, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    return_val_do_info_if_expr(uri_len == 0, VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed:input param error, uri_len is 0.", __FUNCTION__));
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    iRet = snprintf_s(uri, uri_len, uri_len - 1, uri_template, slot_id, member_id, silk_num);
#pragma GCC diagnostic pop
    return_val_do_info_if_expr(iRet <= 0, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    return VOS_OK;
}


LOCAL gint32 _get_chassis_opticalmodule_odata_uri(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar uri[MAX_URI_LENGTH] = {0};
    const gchar *uri_template = NULL;

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s : NULL pointer.", __FUNCTION__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    if (!g_strcmp0(g_chassis_opticalmodule_provider[i_paras->index].property_name, RFPROP_ODATA_ID)) {
        uri_template = URI_FORMAT_CHASSIS_OPTICAL_MODULE;
    } else if (!g_strcmp0(g_chassis_opticalmodule_provider[i_paras->index].property_name, RFPROP_ODATA_CONTEXT)) {
        uri_template = CHASSIS_OPTICAL_MODULE_METADATA;
    } else {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = _get_opticalmodule_odata_prop(i_paras->obj_handle, uri_template, uri, sizeof(uri));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : get odata prop fail.", __FUNCTION__));

    *o_result_jso = json_object_new_string((const char *)uri);
    return_val_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 _get_chassis_direct_opticalmodule_list_odata_uri(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso)
{
    gint32 ret;
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};

    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s Get slot id fail. ret(%d)", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (!g_strcmp0(g_chassis_direct_opticalmodule_list__provider[i_paras->index].property_name, RFPROP_ODATA_ID)) {
        ret = snprintf_s(uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, URI_FORMAT_CHASSIS_DIRECT_OPTICAL, slot_id);
    } else if (!g_strcmp0(g_chassis_direct_opticalmodule_list__provider[i_paras->index].property_name,
        RFPROP_ODATA_CONTEXT)) {
        ret = snprintf_s(uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, CHASSIS_DIRECT_OPTICAL_MODULE_CONTEXT, slot_id);
    } else {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (ret <= 0) {
        debug_log(DLOG_ERROR, "Odata(%s) sprintf_s fail. ret(%d)",
            g_chassis_direct_opticalmodule_list__provider[i_paras->index].property_name, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    *o_result_jso = json_object_new_string((const char *) uri);
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "Memory alloc fail. ");
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    return HTTP_OK;
}


LOCAL void _get_chassis_direct_opticalmodule_uri(guint32 *count, json_object** o_result_jso,
    guint8 flag, GSList* opticalmodule_list)
{
    gint32 ret;
    GSList* node = NULL;
    json_object *obj = NULL;
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};
    guint8 silk_num = 0xff;
    OBJ_HANDLE port_obj = 0;

    guint8 present = 0;

    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get slot id fail. ret(%d)", ret);
        return;
    }
    for (node = opticalmodule_list; node; node = g_slist_next(node)) {
        
        (void)dal_get_property_value_byte((OBJ_HANDLE)node->data, PROPERTY_OPT_MDL_PRESENT, &present);
        if (present != 1) {
            continue;
        }
        *count += 1;
        if (!flag) {
            
            (void)dfl_get_referenced_object((OBJ_HANDLE)node->data, PROPERTY_OPT_MDL_REF_PORT_OBJ, &port_obj);
            (void)dal_get_property_value_byte(port_obj, PROPERTY_PORT_ADAPTER_PORT_NO, &silk_num);
            ret = snprintf_s(uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1,
                URI_FORMAT_CHASSIS_DIRECT_OPTICAL_MODULE, slot_id, silk_num);
            if (ret <= 0) {
                debug_log(DLOG_ERROR, "Sprintf_s fail. ret(%d)", ret);
                continue;
            }

            obj = json_object_new_object();
            if (obj == NULL) {
                debug_log(DLOG_ERROR, "Memory alloc fail. ");
                return;
            }
            json_object_object_add(obj, RFPROP_ODATA_ID, json_object_new_string(uri));
            json_object_array_add(*o_result_jso, obj);
        }
    }
}


LOCAL gint32 _get_chassis_direct_opticalmodule_list_members_uri(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso)
{
    gint32 ret;
    guint8 flag;
    guint32 count = 0;
    GSList* opticalmodule_list = NULL;

    if (!g_strcmp0(g_chassis_direct_opticalmodule_list__provider[i_paras->index].property_name, RFPROP_MEMBERS)) {
        flag = 0;
        *o_result_jso = json_object_new_array();
        if (*o_result_jso == NULL) {
            debug_log(DLOG_ERROR, "Memory alloc fail. ");
            return HTTP_INTERNAL_SERVER_ERROR;
        }
    } else if (!g_strcmp0(g_chassis_direct_opticalmodule_list__provider[i_paras->index].property_name,
        RFPROP_MEMBERS_COUNT)) {
        flag = 1;
    } else {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    ret = dfl_get_object_list(CLASS_OPTICAL_MODULE, &opticalmodule_list);
    if (ret != DFL_OK || opticalmodule_list == NULL) {
        debug_log(DLOG_ERROR, "Get optical list fail. ret(%d)", ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    _get_chassis_direct_opticalmodule_uri(&count, o_result_jso, flag, opticalmodule_list);

    if (flag) {
        *o_result_jso = json_object_new_int(count);
    }
    g_slist_free(opticalmodule_list);
    return HTTP_OK;
}


LOCAL gint32 _get_chassis_direct_opticalmodule_odata_uri(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso)
{
    gint32 ret;
    gchar uri[MAX_URI_LENGTH] = {0};

    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "NULL pointer");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    if (g_strcmp0(g_chassis_opticalmodule_provider[i_paras->index].property_name, RFPROP_ODATA_ID) &&
        g_strcmp0(g_chassis_opticalmodule_provider[i_paras->index].property_name, RFPROP_ODATA_CONTEXT)) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = _get_direct_opticalmodule_odate_prop(i_paras->obj_handle,
        g_chassis_opticalmodule_provider[i_paras->index].property_name, uri, sizeof(uri));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get odata prop fail. ret(%d)", ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const char *) uri);
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "Memory alloc fail. ");
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    return HTTP_OK;
}


LOCAL gint32 _get_chassis_opticalmodule_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint8 health = 0xff;
    gchar *state_str = RF_STATE_ENABLED; // 只有在位时才显示，所有一定是Enabled

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s : NULL pointer.", __FUNCTION__));

    (void)dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_OPT_MDL_HEALTH, &health);

    ret = get_resource_status_property(&health, NULL, state_str, o_result_jso, FALSE);
    return_val_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 _redfish_get_property_string(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar property[PROP_VAL_MAX_LENGTH] = {0};
    gint32 ret;

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s : NULL pointer.", __FUNCTION__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    (void)dal_get_property_value_string(i_paras->obj_handle,
        g_chassis_opticalmodule_provider[i_paras->index].pme_prop_name, property, sizeof(property));

    ret = check_string_val_effective((const gchar *)property);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s:invalid string", __FUNCTION__));

    
    
    dal_trim_string(property, PROP_VAL_MAX_LENGTH);
    

    *o_result_jso = json_object_new_string((const char *)property);
    return_val_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 _redfish_get_property_uint16(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint16 property_value;

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s : NULL pointer.", __FUNCTION__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    (void)dal_get_property_value_uint16(i_paras->obj_handle,
        g_chassis_opticalmodule_provider[i_paras->index].pme_prop_name, &property_value);

    return_val_do_info_if_expr(0xFFFF == property_value, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s:invalid value", __FUNCTION__));

    *o_result_jso = json_object_new_int((int32_t)property_value);
    return_val_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 _redfish_get_property_boolean(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint8 property_value;

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s : NULL pointer.", __FUNCTION__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    (void)dal_get_property_value_byte(i_paras->obj_handle,
        g_chassis_opticalmodule_provider[i_paras->index].pme_prop_name, &property_value);
    return_val_do_info_if_expr(0 != property_value && 1 != property_value, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s:invalid value is %d", __FUNCTION__, property_value));

    *o_result_jso = json_object_new_boolean((int32_t)property_value);
    return_val_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 _redfish_get_opticalmodule_wave_length(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar property[PROP_VAL_MAX_LENGTH] = {0};
    gint32 ret;
    gint32 wave_len = 0;

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s : NULL pointer.", __FUNCTION__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    (void)dal_get_property_value_string(i_paras->obj_handle,
        g_chassis_opticalmodule_provider[i_paras->index].pme_prop_name, property, sizeof(property));

    ret = check_string_val_effective((const gchar *)property);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s:invalid string", __FUNCTION__));

    // 去掉字符串中的非数字部分
    ret = redfish_strip_to_int(property, sizeof(property));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:invalid wave_length format", __FUNCTION__));

    ret = vos_str2int((const gchar *)property, 10, &wave_len, NUM_TPYE_INT32);
    return_val_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR);

    *o_result_jso = json_object_new_int(wave_len);
    return_val_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 _redfish_get_opticalmodule_transceiver_type(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar property[PROP_VAL_MAX_LENGTH] = {0};
    gchar **str_arr = NULL;
    guint32 str_arr_len;
    guint32 i;
    json_object *member_arr = NULL;

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s : NULL pointer.", __FUNCTION__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_array();
    return_val_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR);

    (void)dal_get_property_value_string(i_paras->obj_handle,
        g_chassis_opticalmodule_provider[i_paras->index].pme_prop_name, property, sizeof(property));

    
    return_val_if_fail(VOS_OK == check_string_val_effective(property), HTTP_OK);
    

    // 属性示例100BASE-LX;1000BASE-T   或  100BASE-LX
    str_arr = g_strsplit((const gchar *)property, ";", 0);
    
    return_val_if_expr(NULL == str_arr, HTTP_OK);
    
    str_arr_len = g_strv_length(str_arr);

    for (i = 0; i < str_arr_len; i++) {
        do_if_expr(strlen(str_arr[i]), (member_arr = json_object_new_string((const char *)str_arr[i])));
        (void)json_object_array_add(*o_result_jso, member_arr);
        member_arr = NULL;
    }
    g_strfreev(str_arr);

    return HTTP_OK;
}


LOCAL gint32 _redfish_get_opticalmodule_medium_mode(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar pme_prop[PROP_VAL_MAX_LENGTH] = {0};     // 存放从pme查出来的值
    gchar redfish_prop[PROP_VAL_MAX_LENGTH] = {0}; // 存放redfish要对外显示的属性值
    gint32 ret;

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s : NULL pointer.", __FUNCTION__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    (void)dal_get_property_value_string(i_paras->obj_handle,
        g_chassis_opticalmodule_provider[i_paras->index].pme_prop_name, pme_prop, sizeof(pme_prop));

    ret = check_string_val_effective((const gchar *)pme_prop);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s:invalid string", __FUNCTION__));

    if (!g_strcmp0((const char *)pme_prop, PME_PROPVAL_SM)) {
        (void)strncpy_s(redfish_prop, sizeof(redfish_prop), RFVALUE_SM, strlen(RFVALUE_SM));
    } else if (!g_strcmp0((const char *)pme_prop, PME_PROPVAL_MM)) {
        (void)strncpy_s(redfish_prop, sizeof(redfish_prop), RFVALUE_MM, strlen(RFVALUE_MM));
    } else {
        
        debug_log(DLOG_DEBUG, "%s:invalid medium mode is %s.", __FUNCTION__, pme_prop);
        
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const gchar *)redfish_prop);
    return_val_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 _redfish_get_opticalmodule_production_date(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    errno_t safe_fun_ret;
    gchar pme_prop[PROP_VAL_MAX_LENGTH] = {0};     // 存放从pme查出来的值
    gchar redfish_prop[PROP_VAL_MAX_LENGTH] = {0}; // 存放redfish要对外显示的属性值
    gint32 ret;

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s : NULL pointer.", __FUNCTION__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    (void)dal_get_property_value_string(i_paras->obj_handle,
        g_chassis_opticalmodule_provider[i_paras->index].pme_prop_name, pme_prop, sizeof(pme_prop));

    ret = check_string_val_effective((const gchar *)pme_prop);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s:invalid string", __FUNCTION__));

    
    safe_fun_ret = strncat_s(redfish_prop, sizeof(redfish_prop), "20", strlen("20"));
    return_val_do_info_if_expr(safe_fun_ret != EOK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    safe_fun_ret = strncat_s(redfish_prop, sizeof(redfish_prop), pme_prop, 2);
    return_val_do_info_if_expr(safe_fun_ret != EOK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    safe_fun_ret =
        strncat_s(redfish_prop, sizeof(redfish_prop), RF_TIME_FORMAT_CONNECT, strlen(RF_TIME_FORMAT_CONNECT));
    return_val_do_info_if_expr(safe_fun_ret != EOK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    safe_fun_ret = strncat_s(redfish_prop, sizeof(redfish_prop), pme_prop + 2, 2);
    return_val_do_info_if_expr(safe_fun_ret != EOK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    safe_fun_ret =
        strncat_s(redfish_prop, sizeof(redfish_prop), RF_TIME_FORMAT_CONNECT, strlen(RF_TIME_FORMAT_CONNECT));
    return_val_do_info_if_expr(safe_fun_ret != EOK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    safe_fun_ret = strncat_s(redfish_prop, sizeof(redfish_prop), pme_prop + 4, 2);
    return_val_do_info_if_expr(safe_fun_ret != EOK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

    *o_result_jso = json_object_new_string((const gchar *)redfish_prop);
    return_val_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}

LOCAL gint32 _redfish_get_array_index(const gchar *prop_name, gint32 *arr_index)
{
    gint32 object_type = 0xff;

    return_val_if_expr(NULL == arr_index, VOS_ERR);

    if (0 == g_strcmp0(prop_name, RFPROP_OPTICAL_MODULE_VOLTAGE)) {
        object_type = RFVALUE_OPTICAL_MODULE_VOLT;
    } else if (0 == g_strcmp0(prop_name, RFPROP_OPTICAL_MODULE_TEMPERATURE)) {
        object_type = RFVALUE_OPTICAL_MODULE_TEMP;
    } else if (0 == g_strcmp0(prop_name, RFPROP_OPTICAL_MODULE_BIAS)) {
        object_type = RFVALUE_OPTICAL_MODULE_BIAS;
    } else if (0 == g_strcmp0(prop_name, RFPROP_OPTICAL_MODULE_RXPOWER)) {
        object_type = RFVALUE_OPTICAL_MODULE_RXPOWER;
    } else if (0 == g_strcmp0(prop_name, RFPROP_OPTICAL_MODULE_TXPOWER)) {
        object_type = RFVALUE_OPTICAL_MODULE_TXPOWER;
    } else {
        
        debug_log(DLOG_DEBUG, "%s : invalid property name is %s.", __FUNCTION__, prop_name);
        
        return VOS_ERR;
    }

    *arr_index = object_type;

    return VOS_OK;
}


LOCAL gint32 _redfish_get_property_object(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 object_type = 0xff;
    gdouble prop_val;
    json_object *prop = NULL;
    gint32 ret;

    optical_modul_prop_object_s prop_obj[] = {
        {PROPERTY_OPT_MDL_VCC_CURRENT, PROPERTY_OPT_MDL_VCC_LOW_ALARM, PROPERTY_OPT_MDL_VCC_HIGH_ALARM, RFPROP_OPTICAL_MODULE_READINGVOLTS},
        
        {PROPERTY_OPT_MDL_TEMP_CURRENT, PROPERTY_OPT_MDL_TEMP_LOW_ALARM, PROPERTY_OPT_MDL_TEMP_HIGH_ALARM, RFPROP_OPTICAL_MODULE_READINGCELSIUS}
        
    };

    return_val_do_info_if_fail(
        (o_result_jso != NULL && o_message_jso != NULL && provider_paras_check(i_paras) == VOS_OK),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s : NULL pointer.", __FUNCTION__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: new json object fail", __FUNCTION__));

    ret = _redfish_get_array_index(g_chassis_opticalmodule_provider[i_paras->index].property_name, &object_type);
    return_val_if_fail(ret == VOS_OK, HTTP_OK);

    
    return_val_if_expr((guint32)object_type >= G_N_ELEMENTS(prop_obj), HTTP_OK);
    

    (void)dal_get_property_value_double(i_paras->obj_handle, prop_obj[object_type].current_prop, &prop_val);
    if (prop_val != INVALID_DOUBLE_VALUE_ALL_1 && ((guint16)prop_val & SENSOR_NA_READING) != SENSOR_NA_READING &&
        ((guint16)prop_val & SENSOR_INVALID_READING) != SENSOR_INVALID_READING && prop_val != 0) {
        prop = ex_json_object_new_double(prop_val, "%.2f");
    }
    json_object_object_add(*o_result_jso, prop_obj[object_type].current_rf_prop, prop);
    prop_val = 0.0;
    prop = NULL;

    (void)dal_get_property_value_double(i_paras->obj_handle, prop_obj[object_type].lower_c_prop, &prop_val);
    do_if_fail(prop_val == INVALID_DOUBLE_VALUE_ALL_1, (prop = ex_json_object_new_double(prop_val, "%.2f")));
    json_object_object_add(*o_result_jso, RFPROP_OPTICAL_MODULE_LOWER_CRIT, prop);
    prop_val = 0.0;
    prop = NULL;

    (void)dal_get_property_value_double(i_paras->obj_handle, prop_obj[object_type].upper_c_prop, &prop_val);
    do_if_fail(prop_val == INVALID_DOUBLE_VALUE_ALL_1, (prop = ex_json_object_new_double(prop_val, "%.2f")));
    json_object_object_add(*o_result_jso, RFPROP_OPTICAL_MODULE_UPPER_CRIT, prop);

    return HTTP_OK;
}


LOCAL gint32 _redfish_get_property_array(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint32 object_type = 0xff;
    GVariant *property_val = NULL;
    const gdouble *current_val = NULL;
    gdouble lower_critical;
    gdouble upper_critical;
    guint32 len_ad = 0;
    gsize parm_temp = 0;
    guint32 i;
    json_object *object = NULL;
    json_object *prop_temp = NULL;

    optical_modul_prop_object_s prop_obj[] = {
        {PROPERTY_OPT_MDL_RX_POWER_CURRENT, PROPERTY_OPT_MDL_RX_POWER_LOW_ALARM, PROPERTY_OPT_MDL_RX_POWER_HIGH_ALARM, RFPROP_OPTICAL_MODULE_READINGMILLIWATTS},
        {PROPERTY_OPT_MDL_TX_POWER_CURRENT, PROPERTY_OPT_MDL_TX_POWER_LOW_ALARM, PROPERTY_OPT_MDL_TX_POWER_HIGH_ALARM, RFPROP_OPTICAL_MODULE_READINGMILLIWATTS},
        
        {PROPERTY_OPT_MDL_TX_BIAS_CURRENT, PROPERTY_OPT_MDL_TX_BIAS_LOW_ALARM, PROPERTY_OPT_MDL_TX_BIAS_HIGH_ALARM, RFPROP_OPTICAL_MODULE_READINGMILLIAMPERES}
        
    };

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s : NULL pointer.", __FUNCTION__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_array();
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: new json array fail", __FUNCTION__));

    ret = _redfish_get_array_index(g_chassis_opticalmodule_provider[i_paras->index].property_name, &object_type);
    return_val_if_fail(VOS_OK == ret, HTTP_OK);

    
    return_val_if_expr((guint32)object_type >= G_N_ELEMENTS(prop_obj), HTTP_OK);
    
    // 获取当前属性值
    ret = dfl_get_property_value(i_paras->obj_handle, prop_obj[object_type].current_prop, &property_val);
    if (DFL_OK == ret && NULL != property_val) {
        current_val = (const gdouble *)g_variant_get_fixed_array(property_val, &parm_temp, sizeof(gdouble));
        len_ad = (guint32)parm_temp;
    }

    // 获取低门限严重告警
    (void)dal_get_property_value_double(i_paras->obj_handle, prop_obj[object_type].lower_c_prop, &lower_critical);

    // 获取高门限严重告警
    (void)dal_get_property_value_double(i_paras->obj_handle, prop_obj[object_type].upper_c_prop, &upper_critical);

    for (i = 0; i < len_ad; i++, object = NULL) {
        object = json_object_new_object();
        continue_do_info_if_expr(NULL == object, debug_log(DLOG_ERROR, "%s: new object fail.i = %d.", __FUNCTION__, i));

        do_if_expr(NULL != current_val && INVALID_DOUBLE_VALUE_ALL_1 != current_val[i],
            (prop_temp = ex_json_object_new_double(current_val[i], "%.2f")));
        json_object_object_add(object, prop_obj[object_type].current_rf_prop, prop_temp);
        prop_temp = NULL;

        do_if_expr(INVALID_DOUBLE_VALUE_ALL_1 != lower_critical,
            (prop_temp = ex_json_object_new_double(lower_critical, "%.2f")));
        json_object_object_add(object, RFPROP_OPTICAL_MODULE_LOWER_CRIT, prop_temp);
        prop_temp = NULL;

        do_if_expr(INVALID_DOUBLE_VALUE_ALL_1 != upper_critical,
            (prop_temp = ex_json_object_new_double(upper_critical, "%.2f")));
        json_object_object_add(object, RFPROP_OPTICAL_MODULE_UPPER_CRIT, prop_temp);
        prop_temp = NULL;

        ret = json_object_array_add(*o_result_jso, object);
        do_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s : array add fail.i = %d.", __FUNCTION__, i);
            json_object_put(object));
    }
    do_if_fail(NULL == property_val, g_variant_unref(property_val));

    return HTTP_OK;
}


LOCAL gint32 _get_chassis_opticalmodule_speed(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    errno_t safe_fun_ret = EOK;
    GVariant *prop_val = NULL;
    gint32 ret;
    const gchar **prop_arr = NULL;
    gsize arr_len = 0;
    gsize i;
    json_object *json_prop = NULL;
    gchar temp_str[PROP_VAL_LENGTH] = {0};
    gint32 speed = 0;

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s : NULL pointer.", __FUNCTION__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_array();
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : new array faild", __FUNCTION__));

    ret = dfl_get_property_value(i_paras->obj_handle, g_chassis_opticalmodule_provider[i_paras->index].pme_prop_name,
        &prop_val);
    return_val_do_info_if_fail(DFL_OK == ret, HTTP_OK,
        debug_log(DLOG_ERROR, "%s : get speed faild", __FUNCTION__)); // 失败返回空数组

    prop_arr = g_variant_get_strv(prop_val, &arr_len);
    return_val_do_info_if_expr(NULL == prop_arr, HTTP_OK, g_variant_unref(prop_val);
        debug_log(DLOG_ERROR, "%s : get str array faild", __FUNCTION__));

    
    return_val_do_info_if_expr(1 == arr_len && 0 == strlen(prop_arr[0]), HTTP_OK, g_free(prop_arr);
        g_variant_unref(prop_val));
    

    for (i = 0; i < arr_len; i++) {
        safe_fun_ret = strncpy_s(temp_str, sizeof(temp_str), (const char *)prop_arr[i], sizeof(temp_str) - 1);
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

        // 去掉字符串中的非数字部分
        ret = redfish_strip_to_int(temp_str, sizeof(temp_str));
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s:invalid opticalmodule_speed format", __FUNCTION__));

        ret = vos_str2int(temp_str, 10, &speed, NUM_TPYE_INT32);
        continue_do_info_if_fail(VOS_OK == ret, json_object_array_add(*o_result_jso, NULL));

        json_prop = json_object_new_int(speed);
        ret = json_object_array_add(*o_result_jso, json_prop);
        do_if_fail(VOS_OK == ret, json_object_put(json_prop);
            debug_log(DLOG_ERROR, "%s : array add fail.i = %" G_GSIZE_FORMAT, __FUNCTION__, i));

        (void)memset_s(temp_str, sizeof(temp_str), 0, sizeof(temp_str));
        json_prop = NULL;
        speed = 0;
    }
    g_free(prop_arr);
    g_variant_unref(prop_val);

    return HTTP_OK;
}


LOCAL gint32 _redfish_check_opticalmodule_uri_valid(gchar *uri, OBJ_HANDLE *obj_handle)
{
    gint32 ret;
    gint32 result = VOS_ERR;
    GSList *opticalmodule_list = NULL;
    GSList *viropticalmodule_list = NULL;
    GSList *node = NULL;
    guint8 present = 0xff;
    gchar optical_uri[MAX_URI_LENGTH] = {0};

    return_val_if_expr(NULL == obj_handle, VOS_ERR);

    // URI示例 : /redfish/v1/Chassis/Blade8/NetworkAdapters/MainboardNIC1/NetworkPorts/1/OpticalModule

    ret = dfl_get_object_list(CLASS_OPTICAL_MODULE, &opticalmodule_list);
    
    do_info_if_true(DFL_OK != ret, debug_log(DLOG_DEBUG, "%s: get OpticalModule object list fail", __FUNCTION__));
    ret = dfl_get_object_list(CLASS_VIRTUAL_OPTICAL_MODULE, &viropticalmodule_list);
    do_info_if_true(DFL_OK != ret, debug_log(DLOG_DEBUG, "%s: get VirOpticalModule object list fail", __FUNCTION__));
    return_val_if_expr(NULL == opticalmodule_list && NULL == viropticalmodule_list, VOS_ERR);
    opticalmodule_list = g_slist_concat(opticalmodule_list, viropticalmodule_list);
    

    for (node = opticalmodule_list; node; node = g_slist_next(node)) {
        // 不在位不显示
        (void)dal_get_property_value_byte((OBJ_HANDLE)node->data, PROPERTY_OPT_MDL_PRESENT, &present);
        continue_if_fail(1 == present);

        ret = _get_opticalmodule_odata_prop((OBJ_HANDLE)node->data, URI_FORMAT_CHASSIS_OPTICAL_MODULE, optical_uri,
            sizeof(optical_uri));
        continue_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_DEBUG, "%s: get opticalmodule uri fail", __FUNCTION__));

        if (0 == g_ascii_strcasecmp((const char *)uri, (const char *)optical_uri)) {
            result = VOS_OK;
            *obj_handle = (OBJ_HANDLE)node->data;
            break;
        }
    }
    g_slist_free(opticalmodule_list);

    return result;
}


LOCAL gint32 _redfish_check_direct_opticalmodule_uri_valid(gchar* uri, OBJ_HANDLE* obj_handle)
{
    GSList* opticalmodule_list = NULL;
    GSList* node = NULL;
    guint8 present = 0xff;
    gchar optical_uri[MAX_URI_LENGTH] = {0};

    if (obj_handle == NULL) {
        debug_log(DLOG_DEBUG, "NULL pointer.");
        return RET_ERR;
    }

    

    gint32 result = dfl_get_object_list(CLASS_OPTICAL_MODULE, &opticalmodule_list);
    if (result != DFL_OK || opticalmodule_list == NULL) {
        debug_log(DLOG_DEBUG, "Get OpticalModule object list fail, ret(%d)", result);
        return result;
    }

    for (node = opticalmodule_list; node; node = g_slist_next(node)) {
        
        (void)dal_get_property_value_byte((OBJ_HANDLE)node->data, PROPERTY_OPT_MDL_PRESENT, &present);
        if (present != 1) {
            continue;
        }

        result = _get_direct_opticalmodule_odate_prop((OBJ_HANDLE)node->data,
            RFPROP_ODATA_ID, optical_uri, sizeof(optical_uri));
        if (result != RET_OK) {
            debug_log(DLOG_DEBUG, "Get opticalmodule(%s) uri fail, ret(%d)",
                dfl_get_object_name((OBJ_HANDLE)node->data), result);
            continue;
        }

        if (g_ascii_strcasecmp((const char *)uri, (const char *)optical_uri) == 0) {
            result = RET_OK;
            *obj_handle = (OBJ_HANDLE)node->data;
            break;
        }
    }
    g_slist_free(opticalmodule_list);
    
    return result;
}


gint32 chassis_opticalmodule_etag_del_property(json_object *object)
{
    SPECIAL_PROP_S value[] = {
        {2, {RFPROP_OPTICAL_MODULE_RXPOWER, RFPROP_OPTICAL_MODULE_READINGMILLIWATTS, NULL, NULL, NULL}},
        {2, {RFPROP_OPTICAL_MODULE_TXPOWER, RFPROP_OPTICAL_MODULE_READINGMILLIWATTS, NULL, NULL, NULL}},
        {2, {RFPROP_OPTICAL_MODULE_TEMPERATURE, RFPROP_OPTICAL_MODULE_READINGCELSIUS, NULL, NULL, NULL}},
        {2, {RFPROP_OPTICAL_MODULE_BIAS, RFPROP_OPTICAL_MODULE_READINGMILLIAMPERES, NULL, NULL, NULL}},
        {2, {RFPROP_OPTICAL_MODULE_VOLTAGE, RFPROP_OPTICAL_MODULE_READINGVOLTS, NULL, NULL, NULL}},
        {0, {NULL, NULL, NULL, NULL, NULL}}
    };
    return rsc_del_none_etag_affected_property(object, value, G_N_ELEMENTS(value));
}


gint32 chassis_opticalmodule_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    
    OBJ_HANDLE chassis_comp_handle = 0;

    if (FALSE == redfish_check_chassis_uri_valid(i_paras->uri, &chassis_comp_handle) || (0 == chassis_comp_handle)) {
        return HTTP_NOT_FOUND;
    }
    return_val_if_fail(VOS_OK == check_enclosure_component_type(chassis_comp_handle, FALSE), HTTP_NOT_FOUND);
    

    // 检查URI中可变化的值
    ret = _redfish_check_opticalmodule_uri_valid(i_paras->uri, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s: invalid uri is %s", __FUNCTION__, i_paras->uri));

    i_paras->obj_handle = obj_handle;

    *prop_provider = g_chassis_opticalmodule_provider;
    *count = sizeof(g_chassis_opticalmodule_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}


gint32 chassis_direct_opticalmodule_list_provider_entry(PROVIDER_PARAS_S* i_paras,
    PROPERTY_PROVIDER_S** prop_provider, guint32* count)
{
    OBJ_HANDLE chassis_comp_handle = 0;

    if (redfish_check_chassis_uri_valid(i_paras->uri, &chassis_comp_handle) == FALSE || chassis_comp_handle == 0
        || check_enclosure_component_type(chassis_comp_handle, FALSE) != RET_OK) {
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_chassis_direct_opticalmodule_list__provider;
    *count = sizeof(g_chassis_direct_opticalmodule_list__provider) / sizeof(PROPERTY_PROVIDER_S);
    return RET_OK;
}


gint32 chassis_direct_opticalmodule_provider_entry(PROVIDER_PARAS_S* i_paras,
    PROPERTY_PROVIDER_S** prop_provider, guint32* count)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    OBJ_HANDLE chassis_comp_handle = 0;

    if (redfish_check_chassis_uri_valid(i_paras->uri, &chassis_comp_handle) == FALSE || chassis_comp_handle == 0
        || check_enclosure_component_type(chassis_comp_handle, FALSE) != RET_OK) {
        return HTTP_NOT_FOUND;
    }

    
    ret = _redfish_check_direct_opticalmodule_uri_valid(i_paras->uri, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Invalid uri is %s, ret(%d)", i_paras->uri, ret);
        return HTTP_NOT_FOUND;
    }

    i_paras->obj_handle = obj_handle;

    *prop_provider = g_chassis_direct_opticalmodule_provider;
    *count = sizeof(g_chassis_direct_opticalmodule_provider) / sizeof(PROPERTY_PROVIDER_S);

    return RET_OK;
}
