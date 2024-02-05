
#include "redfish_provider.h"


LOCAL gint32 get_manager_usbstick_ibma_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar slot_uri[PSLOT_URI_LEN] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    gint32 ret;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: redfish_get_board_slot fail.", __FUNCTION__));
    ret = snprintf_s(slot_uri, sizeof(slot_uri), sizeof(slot_uri) - 1, URI_FORMAT_MANAGER_USB_IBMA_ODATAID, slot);
    return_val_do_info_if_fail(0 < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:snprintf_s fail.", __FUNCTION__));
    *o_result_jso = json_object_new_string(slot_uri);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail.", __FUNCTION__));

    return HTTP_OK;
}


LOCAL gint32 redfish_get_usbstick_ibma_connectstatus(guint8 *flag)
{
    gint32 ret;
    OBJ_HANDLE usbstick_handle = 0;
    guint8 support_state = 0xFF;
    guint8 connect_state = 0xFF;
    gchar custom_name[LOG_SPECIAL_STRING_LEN_MAX] = {"iBMA"};

    
    if (dal_get_custom_string_name(custom_name, BMC_CUSTOM_LOGIBMA_NAME, sizeof(custom_name)) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_custom_string_name fail.", __FUNCTION__);
    }

    ret = dal_get_object_handle_nth(CLASS_IBMA_UMS_NAME, 0, &usbstick_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get %s ums object handle failed. ret=%d", __FUNCTION__, custom_name, ret));

    ret = dal_get_property_value_byte(usbstick_handle, PROPERTY_IBMA_UMS_STATE, &support_state);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, get %s state fail. ret=%d", __FUNCTION__, custom_name, ret));
    return_val_do_info_if_fail(IBMA_SUPPORT_STATE_ENABLE == support_state, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s, the %s don't support. state=%d", __FUNCTION__, custom_name, support_state));

    ret = dal_get_property_value_byte(usbstick_handle, PROPERTY_IBMA_UMS_CONNECT_STATE, &connect_state);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, get %s connect state fail. ret=%d", __FUNCTION__, custom_name, ret));

    if ((connect_state == UMS_CONNECT_STATE_CONNECTED) || (connect_state == UMS_CONNECT_STATE_DISCONNECT_FAIL)) {
        *flag = USBSTICK_CONNECTED;
    } else {
        *flag = USBSTICK_DISCONNECTED;
    }

    return VOS_OK;
}


LOCAL gint32 get_usbstick_ibma_inserted(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint8 flag = USBSTICK_DISCONNECTED;
    gint32 ret;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_usbstick_ibma_connectstatus(&flag);
    return_val_if_fail(VOS_OK == ret, ret);

    *o_result_jso =
        (USBSTICK_DISCONNECTED != flag) ? (json_object_new_boolean(TRUE)) : (json_object_new_boolean(FALSE));

    return HTTP_OK;
}


LOCAL gint32 get_usbstick_ibma_connectedvia(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint8 flag = 0;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_usbstick_ibma_connectstatus(&flag);
    return_val_if_fail(VOS_OK == ret, ret);

    if (USBSTICK_DISCONNECTED != flag) {
        *o_result_jso = json_object_new_string(MANAGER_CONNECTVIA_VMM_OEM);
    } else {
        *o_result_jso = json_object_new_string(MANAGER_CONNECTVIA_VMM_NOTCONN);
    }

    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail.", __FUNCTION__));
    return HTTP_OK;
}


LOCAL gint32 get_usbstick_ibma_packet(struct json_object *object)
{
    gint32 ret;
    OBJ_HANDLE iBMAUmsHandle = 0;
    guint8 packetNum = 0;
    gint32 i;
    json_object *jsonObjItem = NULL;
    json_object *jsonPacket = NULL;
    gchar **OSTypeVector = NULL;
    gsize OSTypeLen = 0;
    gchar **versionVector = NULL;
    gsize versionLen = 0;

    ret = dal_get_object_handle_nth(CLASS_IBMA_UMS_NAME, 0, &iBMAUmsHandle);
    return_val_do_info_if_fail(VOS_OK == ret, ret,
        debug_log(DLOG_ERROR, "%s: get the ibma ums object fail, ret=%d.", __FUNCTION__, ret));
    ret = dal_get_property_value_byte(iBMAUmsHandle, PROPERTY_IBMA_UMS_PACKET_NUM, &packetNum);
    return_val_do_info_if_fail(VOS_OK == ret, ret,
        debug_log(DLOG_ERROR, "%s: get the ibma packet number fail, ret=%d.", __FUNCTION__, ret));
    if (packetNum == 0) {
        json_object_object_add(object, PROPERTY_USBSTICK_IBMA_PACKET_INFO, NULL);
        return VOS_OK;
    }
    ret = dal_get_property_value_strv(iBMAUmsHandle, PROPERTY_IBMA_UMS_OS_TYPE, &OSTypeVector, &OSTypeLen);
    return_val_do_info_if_fail(VOS_OK == ret, ret,
        debug_log(DLOG_ERROR, "%s: get the ibma os type fail, ret=%d.", __FUNCTION__, ret));
    if (OSTypeLen != packetNum) {
        g_strfreev(OSTypeVector);
        debug_log(DLOG_ERROR,
            "%s: the os type number isn't equal with packet number. OSTypeLen=%" G_GSIZE_FORMAT ", PacketNum=%u",
            __FUNCTION__, OSTypeLen, packetNum);
        return VOS_ERR;
    }
    ret = dal_get_property_value_strv(iBMAUmsHandle, PROPERTY_IBMA_UMS_VERSION, &versionVector, &versionLen);
    return_val_do_info_if_fail(VOS_OK == ret, ret, g_strfreev(OSTypeVector);
        debug_log(DLOG_ERROR, "%s: get the ibma os type fail, ret=%d.", __FUNCTION__, ret));
    if (versionLen != packetNum) {
        g_strfreev(OSTypeVector);
        g_strfreev(versionVector);
        debug_log(DLOG_ERROR,
            "%s: the version number isn't equal with packet number. versionLen=%" G_GSIZE_FORMAT ", PacketNum=%u",
            __FUNCTION__, versionLen, packetNum);
        return VOS_ERR;
    }
    jsonPacket = json_object_new_array();
    return_val_do_info_if_fail(jsonPacket != NULL, VOS_ERR, g_strfreev(OSTypeVector); g_strfreev(versionVector);
        debug_log(DLOG_ERROR, "%s: new array fail.", __FUNCTION__));

    for (i = 0; i < packetNum; i++) {
        jsonObjItem = json_object_new_object();
        json_object_object_add(jsonObjItem, MANAGER_USBSTICK_IBMA_OSTYPE, json_object_new_string(OSTypeVector[i]));
        json_object_object_add(jsonObjItem, MANAGER_USBSTICK_IBMA_VERSION, json_object_new_string(versionVector[i]));
        ret = json_object_array_add(jsonPacket, jsonObjItem);
        if (ret != VOS_OK) {
            json_object_put(jsonObjItem);
            debug_log(DLOG_ERROR, "%s: json_object_array_add fail. ret=%d, number=%d", __FUNCTION__, ret, i);
        }
    }
    g_strfreev(OSTypeVector);
    g_strfreev(versionVector);
    json_object_object_add(object, PROPERTY_USBSTICK_IBMA_PACKET_INFO, jsonPacket);

    return VOS_OK;
}


LOCAL gint32 get_usbstick_ibma_status(struct json_object *object)
{
    gint32 ret;
    OBJ_HANDLE iBMAUmsHandle = 0;
    guint8 state = 0;
    gchar custom_name[LOG_SPECIAL_STRING_LEN_MAX] = {"iBMA"};

    
    if (dal_get_custom_string_name(custom_name, BMC_CUSTOM_LOGIBMA_NAME, sizeof(custom_name)) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_custom_string_name fail.", __FUNCTION__);
    }

    ret = dal_get_object_handle_nth(CLASS_IBMA_UMS_NAME, 0, &iBMAUmsHandle);
    return_val_do_info_if_fail(VOS_OK == ret, ret,
        debug_log(DLOG_ERROR, "%s: get the %s ums object fail, ret=%d.", __FUNCTION__, custom_name, ret));
    ret = dal_get_property_value_byte(iBMAUmsHandle, PROPERTY_IBMA_UMS_STATE, &state);
    return_val_do_info_if_fail(VOS_OK == ret, ret,
        debug_log(DLOG_ERROR, "%s: get the %s state fail, ret=%d.", __FUNCTION__, custom_name, ret));
    switch (state) {
        case IBMA_SUPPORT_STATE_ENABLE:
            (void)json_object_object_add(object, PROPERTY_USBSTICK_IBMA_STATUS,
                json_object_new_string("iBMAPacketIsAvailable"));
            break;

        case IBMA_SUPPORT_STATE_NO_PARTITION:
            (void)json_object_object_add(object, PROPERTY_USBSTICK_IBMA_STATUS,
                json_object_new_string("iBMAPartitionTableNotExist"));
            break;

        case IBMA_SUPPORT_STATE_NO_PACKET:
            (void)json_object_object_add(object, PROPERTY_USBSTICK_IBMA_STATUS,
                json_object_new_string("iBMAPacketNotExist"));
            break;

        case IBMA_SUPPORT_STATE_INVALID:
            (void)json_object_object_add(object, PROPERTY_USBSTICK_IBMA_STATUS, NULL);
            break;
    }
    return VOS_OK;
}


LOCAL gint32 get_manager_usbstick_ibma_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    gchar ibmausbcontroluri[PSLOT_URI_LEN] = {0};
    gchar ibmausbactioninfo[PSLOT_URI_LEN] = {0};

    json_object *huawei_json = NULL;
    json_object *actions_json = NULL;
    json_object *usbstick_control = NULL;

    gint32 ret;
    gchar custom_name[LOG_SPECIAL_STRING_LEN_MAX] = {"iBMA"};

    
    return_val_do_info_if_fail((NULL != o_result_jso && NULL != o_message_jso && NULL != i_paras),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    return_val_do_info_if_fail(redfish_get_board_slot(slot, sizeof(slot)) == RET_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get slot it failed.", __FUNCTION__));

    
    if (dal_get_custom_string_name(custom_name, BMC_CUSTOM_LOGIBMA_NAME, sizeof(custom_name)) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_custom_string_name failed.", __FUNCTION__);
    }
    
    ret = snprintf_s(ibmausbcontroluri, sizeof(ibmausbcontroluri), sizeof(ibmausbcontroluri) - 1,
        URI_FORMAT_MANAGER_USBCONTROL_IBMA_URI, slot);
    return_val_do_info_if_fail(0 < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: snprintf_s %s uri fail.", __FUNCTION__, custom_name));

    ret = snprintf_s(ibmausbactioninfo, sizeof(ibmausbactioninfo), sizeof(ibmausbactioninfo) - 1,
        URI_FORMAT_MANAGER_USBACTIONINFO_IBMA_URI, slot);
    return_val_do_info_if_fail(0 < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: snprintf_s %s action info fail.", __FUNCTION__, custom_name));

    huawei_json = json_object_new_object();
    return_val_do_info_if_fail(NULL != huawei_json, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_object fail.", __FUNCTION__));
    
    return_val_do_info_if_fail(get_usbstick_ibma_packet(huawei_json) == RET_OK, HTTP_INTERNAL_SERVER_ERROR,
        json_object_put(huawei_json); debug_log(DLOG_ERROR, "%s:get %s packet fail.", __FUNCTION__, custom_name));
    
    ret = get_usbstick_ibma_status(huawei_json);
    return_val_do_info_if_fail(ret == VOS_OK, HTTP_INTERNAL_SERVER_ERROR, json_object_put(huawei_json);
        debug_log(DLOG_ERROR, "%s:get %s status fail.", __FUNCTION__, custom_name));
    
    usbstick_control = json_object_new_object();
    return_val_do_info_if_fail(NULL != usbstick_control, HTTP_INTERNAL_SERVER_ERROR, json_object_put(huawei_json);
        debug_log(DLOG_ERROR, "%s:json_object_new_object fail.", __FUNCTION__));
    json_object_object_add(usbstick_control, RFPROP_TARGET, json_object_new_string(ibmausbcontroluri));
    json_object_object_add(usbstick_control, RFPROP_ACTION_INFO, json_object_new_string(ibmausbactioninfo));
    actions_json = json_object_new_object();
    return_val_do_info_if_fail(NULL != actions_json, HTTP_INTERNAL_SERVER_ERROR, json_object_put(usbstick_control);
        json_object_put(huawei_json); debug_log(DLOG_ERROR, "%s:json_object_new_object fail.", __FUNCTION__));
    json_object_object_add(actions_json, PROPERTY_USBSTICK_CONTROL, usbstick_control);
    json_object_object_add(huawei_json, MANAGER_PROPERTY_ACTIONS, actions_json);

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != (*o_result_jso), HTTP_INTERNAL_SERVER_ERROR, json_object_put(usbstick_control);
        json_object_put(actions_json); json_object_put(huawei_json);
        debug_log(DLOG_ERROR, "%s:json_object_new_object fail.", __FUNCTION__));
    json_object_object_add(*o_result_jso, RFPROP_MANAGER_HUAWEI, huawei_json);

    return HTTP_OK;
}


LOCAL gint32 act_usbstick_ibma_control_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE usbstick_handle = 0;
    guint8 supportState = 0xFF;
    gchar custom_name[LOG_SPECIAL_STRING_LEN_MAX] = {"iBMA"};

    
    if (dal_get_custom_string_name(custom_name, BMC_CUSTOM_LOGIBMA_NAME, sizeof(custom_name)) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_custom_string_name fail.", __FUNCTION__);
    }

    ret = dal_get_object_handle_nth(CLASS_IBMA_UMS_NAME, 0, &usbstick_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: get %s ums object handle failed. ret=%d", __FUNCTION__, custom_name, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    ret = dal_get_property_value_byte(usbstick_handle, PROPERTY_IBMA_UMS_STATE, &supportState);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: get %s state fail. ret=%d", __FUNCTION__, custom_name, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if (supportState != 0) {
        debug_log(DLOG_INFO, "%s: the %s don't support. state=%d", __FUNCTION__, custom_name, supportState);
        (void)create_message_info(MSGID_ACT_NOT_SUPPORTED, NULL, o_message_jso, PROPERTY_USBSTICK_CONTROL_ACT);
        return HTTP_NOT_IMPLEMENTED;
    }

    return act_usbstick_control_oem_process(i_paras, o_message_jso, o_result_jso);
}


LOCAL PROPERTY_PROVIDER_S g_usbstick_ibma_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_usbstick_ibma_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_usbstick_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {MANAGER_PROPERTY_VMM_INSERTED, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_usbstick_ibma_inserted, NULL, NULL, ETAG_FLAG_ENABLE},
    {MANAGER_PROPERTY_VMM_CONNECTVIA, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_usbstick_ibma_connectedvia, NULL, NULL, ETAG_FLAG_ENABLE},
    {MANAGER_PROPERTY_VMM_MEDIATYPE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_usbstick_mediatype, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_OEM, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_usbstick_ibma_oem, NULL, NULL, ETAG_FLAG_ENABLE},
    {PROPERTY_USBSTICK_CONTROL_ACT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_VMMMGNT, SYS_LOCKDOWN_FORBID, NULL, NULL, act_usbstick_ibma_control_oem, ETAG_FLAG_ENABLE},
};


gint32 get_usbstick_ibma_service_support_state(void)
{
    gint32 ret;
    OBJ_HANDLE pme_obj_handle = 0;
    guchar enable = 0;
    gchar custom_name[LOG_SPECIAL_STRING_LEN_MAX] = {"iBMA"};

    
    if (dal_get_custom_string_name(custom_name, BMC_CUSTOM_LOGIBMA_NAME, sizeof(custom_name)) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_custom_string_name fail.", __FUNCTION__);
    }

    // 获取pme支持标志
    ret = dal_get_object_handle_nth(CLASS_NAME_PME_SERVICECONFIG, 0, &pme_obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get pme_obj_handle fail. ret=%d", __FUNCTION__, ret));
    ret = dal_get_property_value_byte(pme_obj_handle, PROTERY_PME_SERVICECONFIG_IBMA_ENABLE, &enable);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get %s enable flag fail. ret=%d", __FUNCTION__, custom_name, ret));
    gint32 bma_flag = get_installable_bma_state();
    ret = (enable == ENABLE) ? ((bma_flag == VOS_OK || bma_flag == BMA_IS_WORKING) ? VOS_OK : VOS_ERR) : VOS_ERR;

    return ret;
}


gint32 manager_provider_usbstick_ibma_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gboolean ret;
    return_val_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count), HTTP_INTERNAL_SERVER_ERROR);

    return_val_if_expr(VOS_OK != get_usbstick_ibma_service_support_state(), HTTP_NOT_FOUND);

    ret = redfish_check_manager_uri_valid(i_paras->uri);
    return_val_do_info_if_expr(TRUE != ret, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s: URI check failed.", __FUNCTION__));

    *prop_provider = g_usbstick_ibma_provider;
    *count = sizeof(g_usbstick_ibma_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}


LOCAL gint32 get_manager_usbactionaction_ibma_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar slot_uri[PSLOT_URI_LEN] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    gint32 ret;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: redfish_get_board_slot fail.", __FUNCTION__));
    ret = snprintf_s(slot_uri, sizeof(slot_uri), sizeof(slot_uri) - 1, URI_FORMAT_MANAGER_USBACTIONINFO_IBMA_URI, slot);
    return_val_do_info_if_fail(0 < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: snprintf_s fail.", __FUNCTION__));
    *o_result_jso = json_object_new_string(slot_uri);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail.", __FUNCTION__));

    return HTTP_OK;
}


LOCAL PROPERTY_PROVIDER_S g_usbstick_ibam_actioninfo_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_usbactionaction_ibma_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
};


gint32 manager_provider_usbstick_ibma_actinfo_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gboolean ret;

    return_val_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count), HTTP_INTERNAL_SERVER_ERROR);

    return_val_if_expr(VOS_OK != get_usbstick_ibma_service_support_state(), HTTP_NOT_FOUND);

    ret = redfish_check_manager_uri_valid(i_paras->uri);
    return_val_do_info_if_expr(TRUE != ret, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s:URI check failed.", __FUNCTION__));

    *prop_provider = g_usbstick_ibam_actioninfo_provider;
    *count = sizeof(g_usbstick_ibam_actioninfo_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}
