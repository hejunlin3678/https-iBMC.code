
#ifdef ARM64_HI1711_ENABLED

#include "redfish_provider.h"
#include "redfish_provider_chassis.h"

#define PARAMETER_AUTONEGO_POSITION     0
#define PARAMETER_DUPLEX_POSITION       1
#define PARAMETER_WORK_RATE_POSITION    2
#define PARAMETER_PKT_LEN_POSITION      3
#define PARAMETER_PVID_POSITION         4
#define PARAMETER_LOOP_MODE_POSITION    5
#define PARAMETER_LINK_STAT_POSITION    6
#define PARAMETER_ENA_STAT_POSITION     7

#define URI_FORMAT_BLADE_LSW_SET_PORT_ENABLE_TARGET \
    "/redfish/v1/Chassis/%s/Switches/%s/Ports/%s/Actions/Oem/Huawei/Port.SetPortEnable"
#define URI_FORMAT_BLADE_LSW_SET_PORT_ENABLE_INFO \
    "/redfish/v1/Chassis/%s/Switches/%s/Ports/%s/SetPortEnableActionInfo"

LOCAL gint32 get_chassis_lanswitch_port_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 get_chassis_lanswitch_port_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 get_chassis_lanswitch_port_maxspeed(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 get_chassis_lanswitch_port_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 _redfish_get_port_info(PROVIDER_PARAS_S* i_paras, json_object* huawei);

LOCAL gint32 redfish_get_lsw_port_name(PROVIDER_PARAS_S *i_paras, gchar *port_name, gsize max_port_name_len);

LOCAL gint32 redfish_check_lsw_port_name(gchar *uri, const gchar *blade_name, OBJ_HANDLE *object_handle);

LOCAL gint32 get_chassis_lanswitch_port_actions(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 lsw_action_set_port_enable(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 get_chassis_lanswitch_port_action_odata_id(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso);

LOCAL gint32 get_chassis_lanswitch_portinfo_odata_id(PROVIDER_PARAS_S* i_paras,
                                                     json_object** o_message_jso, json_object** o_result_jso);
LOCAL gint32 get_chassis_lanswitch_portinfo_oem(PROVIDER_PARAS_S* i_paras,
                                                json_object** o_message_jso, json_object** o_result_jso);
LOCAL gint32 get_chassis_lanswitch_portinfo_id(PROVIDER_PARAS_S* i_paras,
                                               json_object** o_message_jso, json_object** o_result_jso);
LOCAL gint32 get_chassis_lanswitch_portinfo_maxspeed(PROVIDER_PARAS_S* i_paras,
                                                     json_object** o_message_jso, json_object** o_result_jso);
LOCAL gint32 get_chassis_lanswitch_portinfo_actions(PROVIDER_PARAS_S *i_paras,
                                                    json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_chassis_lanswitch_portinfo_action_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
                                                            json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_chassis_lanswitch_port_provider[] = {
    { RFPROP_ODATA_ID, CLASS_LSW_PORT_ATTR_CENTER, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
      get_chassis_lanswitch_port_odata_id, NULL, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_COMMON_OEM, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID,
      get_chassis_lanswitch_port_oem, NULL, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_CHASSIS_LANSWITCH_PORT_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID,
      get_chassis_lanswitch_port_id, NULL, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_CHASSIS_LANSWITCH_OEM_MAXSPEEDGBPS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY,
      SYS_LOCKDOWN_FORBID, get_chassis_lanswitch_port_maxspeed, NULL, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_ACTIONS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
      get_chassis_lanswitch_port_actions, NULL, NULL, ETAG_FLAG_ENABLE },
    // Action
    { RFPROP_CHASSIS_SET_PORT_ENABLE_ACTION, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_BASICSETTING,
      SYS_LOCKDOWN_FORBID, NULL, NULL, lsw_action_set_port_enable, ETAG_FLAG_ENABLE },
};

LOCAL PROPERTY_PROVIDER_S g_chassis_lanswitch_port_action_provider[] = {
    { RFPROP_ODATA_ID, CLASS_LSW_PORT_ATTR_CENTER, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
      get_chassis_lanswitch_port_action_odata_id, NULL, NULL, ETAG_FLAG_ENABLE },
};

LOCAL PROPERTY_PROVIDER_S g_chassis_lanswitch_portinfo_provider[] = {
    { RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
      get_chassis_lanswitch_portinfo_odata_id, NULL, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_COMMON_OEM, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID,
      get_chassis_lanswitch_portinfo_oem, NULL, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_CHASSIS_LANSWITCH_PORT_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID,
      get_chassis_lanswitch_portinfo_id, NULL, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_CHASSIS_LANSWITCH_OEM_MAXSPEEDGBPS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY,
      SYS_LOCKDOWN_FORBID, get_chassis_lanswitch_portinfo_maxspeed, NULL, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_ACTIONS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
      get_chassis_lanswitch_portinfo_actions, NULL, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_CHASSIS_SET_PORT_ENABLE_ACTION, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_BASICSETTING,
      SYS_LOCKDOWN_FORBID, NULL, NULL, lsw_action_set_port_enable, ETAG_FLAG_ENABLE },
};

LOCAL PROPERTY_PROVIDER_S g_chassis_switch_port_action_provider[] = {
    { RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
      get_chassis_lanswitch_portinfo_action_odata_id, NULL, NULL, ETAG_FLAG_ENABLE },
};

gchar *g_statistic_list[LSW_LOCAL_PORT_STATISTIC_CNT] = {
    RFPROP_CHASSIS_LANSWITCH_OEM_TOTALOCTRX, RFPROP_CHASSIS_LANSWITCH_OEM_TOTALOCTTX,
    RFPROP_CHASSIS_LANSWITCH_OEM_UNICASTPKTRX, RFPROP_CHASSIS_LANSWITCH_OEM_MULTICASTPKTRX,
    RFPROP_CHASSIS_LANSWITCH_OEM_BROADCASTPKTRX, RFPROP_CHASSIS_LANSWITCH_OEM_UNICASTPKTTX,
    RFPROP_CHASSIS_LANSWITCH_OEM_MULTICASTPKTTX, RFPROP_CHASSIS_LANSWITCH_OEM_BROADCASTPKTTX,
    RFPROP_CHASSIS_LANSWITCH_OEM_TOTALDISCARDSRX, RFPROP_CHASSIS_LANSWITCH_OEM_TOTALDISCARDSTX,
    RFPROP_CHASSIS_LANSWITCH_OEM_TOTALERRORRX, RFPROP_CHASSIS_LANSWITCH_OEM_TOTALERRORTX
};

gchar *g_port_work_rate_list[LSW_REDFISH_PORT_WORK_RATE_NUM] = {
    LSW_CTRL_PORT_10M_SPEED, LSW_CTRL_PORT_100M_SPEED, LSW_CTRL_PORT_1000M_SPEED,
    LSW_CTRL_PORT_10G_SPEED, LSW_CTRL_PORT_25G_SPEED, LSW_CTRL_PORT_100G_SPEED
};

LOCAL gint32 get_chassis_lanswitch_port_action_chk(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, gchar *blade_name, guint32 blade_len, gchar *port_name, guint32 port_len)
{
    gint32 ret;

    ret = rf_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, check para and priv fail: %d", __FUNCTION__, ret);
        return ret;
    }

    ret = rf_check_format_blade_name(i_paras, blade_name, blade_len);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, check or format blade_name fail: %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = redfish_get_lsw_port_name(i_paras, port_name, port_len);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, get port_name fail: %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_object();
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s, *o_result_jso is NULL.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 get_chassis_lanswitch_port_actions(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar blade_name[MAX_NAME_SIZE] = {0};
    gchar port_name[MAX_LSW_PORT_NAME_LEN] = {0};
    json_object *oem = NULL;
    json_object *huawei = NULL;
    gchar target[MAX_URI_LEN] = {0};
    gchar action_info[MAX_URI_LEN] = {0};
    json_object *action = NULL;

    ret = get_chassis_lanswitch_port_action_chk(i_paras, o_message_jso, o_result_jso, blade_name, sizeof(blade_name),
                                                port_name, sizeof(port_name));
    if (ret != HTTP_OK) {
        debug_log(DLOG_ERROR, "%s, check para and priv fail: %d", __FUNCTION__, ret);
        return ret;
    }

    
    oem = json_object_new_object();
    json_object_object_add(*o_result_jso, RFPROP_OEM, oem);
    if (oem == NULL) {
        debug_log(DLOG_ERROR, "%s, oem is null.", __FUNCTION__);
        return HTTP_OK;
    }

    
    huawei = json_object_new_object();
    json_object_object_add(oem, RFPROP_OEM_HUAWEI, huawei);
    if (huawei == NULL) {
        debug_log(DLOG_ERROR, "%s, obj is null.", __FUNCTION__);
        return HTTP_OK;
    }

    action = json_object_new_object();
    json_object_object_add(huawei, RFPROP_CHASSIS_LSW_SET_PORT_ENABLE, action);
    if (action == NULL) {
        debug_log(DLOG_ERROR, "%s, action is null.", __FUNCTION__);
        return HTTP_OK;
    }

    ret = snprintf_s(target, sizeof(target), sizeof(target) - 1, URI_FORMAT_LSW_SET_PORT_ENABLE_TARGET,
                     blade_name, port_name);
    if (ret <= RET_OK) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
    ret = snprintf_s(action_info, sizeof(action_info), sizeof(action_info) - 1, URI_FORMAT_LSW_SET_PORT_ENABLE_INFO,
                     blade_name, port_name);
    if (ret <= RET_OK) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
    json_object_object_add(action, RFPROP_TARGET, json_object_new_string((const gchar*)target));
    json_object_object_add(action, RFPROP_ACTION_INFO, json_object_new_string((const gchar*)action_info));

    return HTTP_OK;
}

LOCAL gint32 get_enable_status_from_request(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, guint32 *enable)
{
    const gchar *value_str = NULL;
    json_object *port_enable_obj = NULL;

    if (o_message_jso == NULL || i_paras == NULL || i_paras->val_jso == NULL) {
        debug_log(DLOG_ERROR, "[%s] param error", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if ((i_paras->user_role_privilege & USERROLE_BASICSETTING) == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    (void)json_object_object_get_ex(i_paras->val_jso, RFPROP_CHASSIS_LANSWITCH_OEM_PORT_ENABLE, &port_enable_obj);
    value_str = dal_json_object_get_str(port_enable_obj);
    if (value_str == NULL) {
        debug_log(DLOG_ERROR, "[%s] dal_json_object_get_str failed", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if (g_strcmp0(value_str, "Disable") == 0) {
        *enable = LSW_LOCAL_PORT_STATUS_DISABLE;
    } else if (g_strcmp0(value_str, "Enable") == 0) {
        *enable = LSW_LOCAL_PORT_STATUS_ENABLE;
    } else {
        debug_log(DLOG_ERROR, "[%s] enable string error", __FUNCTION__);
        (void)create_message_info(MSGID_GENERAL_ERROR, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }

    return RET_OK;
}

LOCAL gint32 check_if_port_support_enable(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
                                          const gchar *class_name, guint8 *slot_id)
{
    gint32 ret;

    
    if (g_strcmp0(class_name, CLASS_LSW_PORT_ATTR_CENTER) == 0) {
        ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_LSW_CENTER_SLOTID, slot_id);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "[%s] get slot id failed, ret=%d", __FUNCTION__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        if ((*slot_id != SMM1_SLOT_ID) && (*slot_id != SMM2_SLOT_ID)) {
            (void)create_message_info(MSGID_ACT_NOT_SUPPORTED, NULL, o_message_jso,
                                      RFPROP_CHASSIS_SET_PORT_ENABLE_ACTION);
            return HTTP_NOT_IMPLEMENTED;
        }
    } else if (g_strcmp0(class_name, CLASS_LSW_PORT_ATTR_LOCAL_CP) != 0) {
        debug_log(DLOG_ERROR, "[%s] unsupported class:%s", __FUNCTION__, class_name);
        (void)create_message_info(MSGID_ACT_NOT_SUPPORTED, NULL, o_message_jso,
                                  RFPROP_CHASSIS_SET_PORT_ENABLE_ACTION);
        return HTTP_BAD_REQUEST;
    }

    return RET_OK;
}

LOCAL gint32 set_port_enable_by_method(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, guint32 port_enable)
{
    gint32 ret;
    guint8 slot_id = 0;
    GSList *input_list = NULL;
    gchar class_name[MAX_NAME_SIZE] = {0};
    gchar port_name[LSW_PORT_NAME_MAX_LEN] = {0};

    ret = dal_get_property_value_string(i_paras->obj_handle, PROPERTY_LSW_SUB_PORT_NAME, port_name, sizeof(port_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get port name failed, ret=%d", __FUNCTION__, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dfl_get_class_name(i_paras->obj_handle, class_name, sizeof(class_name));
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s] dfl_get_class_name failed, port_name=%s ret=%d", __FUNCTION__, port_name, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = check_if_port_support_enable(i_paras, o_message_jso, (const gchar*)class_name, &slot_id);
    if (ret != RET_OK) {
        return ret;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(port_name));
    input_list = g_slist_append(input_list, g_variant_new_uint32(port_enable));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
                                        i_paras->obj_handle, class_name, METHOD_LSW_SET_PORT_ENABLE, AUTH_DISABLE,
                                        i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        (void)create_message_info(MSGID_GENERAL_ERROR, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }
    (void)create_message_info(MSGID_PORT_ENABLE_OK, NULL, o_message_jso);

    return RET_OK;
}

LOCAL gint32 lsw_action_set_port_enable(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
                                        json_object **o_result_jso)
{
    gint32 ret;
    guint32 port_enable = LSW_LOCAL_PORT_STATUS_INVALID;

    ret = get_enable_status_from_request(i_paras, o_message_jso, &port_enable);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get_enable_status_from_request failed, ret=%d", __FUNCTION__, ret);
        return ret;
    }

    ret = set_port_enable_by_method(i_paras, o_message_jso, port_enable);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] set_port_enable_by_method failed, ret=%d", __FUNCTION__, ret);
        return ret;
    }

    return HTTP_OK;
}


LOCAL gint32 get_chassis_lanswitch_port_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar blade_name_format[MAX_NAME_SIZE] = {0};
    gchar port_name[MAX_LSW_PORT_NAME_LEN] = {0};

    ret = rf_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, check para and priv fail: %d", __FUNCTION__, ret);
        return ret;
    }

    ret = rf_check_format_blade_name(i_paras, blade_name_format, sizeof(blade_name_format));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, check or format blade_name fail: %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = redfish_get_lsw_port_name(i_paras, port_name, sizeof(port_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, get port_name fail: %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, URI_FORMAT_LANSWITCH_PORT, blade_name_format, port_name);
    if (ret <= RET_OK) {
        debug_log(DLOG_ERROR, "%s, snprintf_s fail, ret = %d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const gchar*)uri);
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s, json_object_new_string fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 get_chassis_lanswitch_port_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar port_name[MAX_LSW_PORT_NAME_LEN] = {0};
    gint32 ret;

    ret = rf_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, check para and priv fail: %d", __FUNCTION__, ret);
        return ret;
    }

    ret = redfish_get_lsw_port_name(i_paras, port_name, sizeof(port_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, get port_name fail: %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const gchar*)port_name);
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s, json_object_new_string fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 redfish_get_lsw_port_maxspeed(PROVIDER_PARAS_S *i_paras, gint32 *maxspeed)
{
    gint32 ret;
    guint32 max_speed = 0;

    ret = dal_get_property_value_uint32(i_paras->obj_handle, PROPERTY_LSW_SPEED, &max_speed);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, get PROPERTY_LSW_SPEED fail: %d.", __FUNCTION__, ret);
        return ret;
    }

    *maxspeed = max_speed;
    return RET_OK;
}


LOCAL gint32 get_chassis_lanswitch_port_maxspeed(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 max_speed = 0;
    gint32 ret;

    ret = rf_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, check para and priv fail: %d", __FUNCTION__, ret);
        return ret;
    }

    ret = redfish_get_lsw_port_maxspeed(i_paras, &max_speed);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, get max_speed fail: %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_int(max_speed / LSW_LOCAL_PORT_SPEEDSTRAMS);
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s, json_object_new_int fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 redfish_get_lsw_port_num(GSList *input_list, json_object **tmp_val_jso)
{
    guint64 tmp;
    guint32 loop;

    if (g_slist_length(input_list) < LSW_LOCAL_PORT_STATISTIC_CNT) {
        debug_log(DLOG_ERROR, "%s, input list length error: %d", __FUNCTION__, g_slist_length(input_list));
        return RET_ERR;
    }

    *tmp_val_jso = json_object_new_object();
    if (*tmp_val_jso == NULL) {
        debug_log(DLOG_ERROR, "%s, *tmp_val_jso is NULL.", __FUNCTION__);
        return RET_ERR;
    }

    for (loop = 0; loop < LSW_LOCAL_PORT_STATISTIC_CNT; loop++) {
        tmp = g_variant_get_uint64((GVariant *)g_slist_nth_data(input_list, loop));
        json_object_object_add(*tmp_val_jso, g_statistic_list[loop], json_object_new_int64(tmp));
    }

    return RET_OK;
}

LOCAL gint32 _redfish_get_port_statistic(PROVIDER_PARAS_S *i_paras, json_object *huawei)
{
    gchar port_name[MAX_LSW_PORT_NAME_LEN] = {0};
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    json_object *tmp_val_jso = NULL;
    gchar class_name[MAX_NAME_SIZE] = {0};

    ret = redfish_get_lsw_port_name(i_paras, port_name, sizeof(port_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get port_name failed, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    ret = dfl_get_class_name(i_paras->obj_handle, class_name, sizeof(class_name));
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s] dfl_get_class_name failed, port_name=%s ret=%d", __FUNCTION__, port_name, ret);
        return RET_ERR;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(port_name));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, class_name, METHOD_LSW_GET_PORT_STATISTICS, AUTH_DISABLE,
        i_paras->user_role_privilege, input_list, &output_list);

    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != RET_OK) {
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        debug_log(DLOG_ERROR, "[%s] uip_call_class_method_redfish failed, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    ret = redfish_get_lsw_port_num(output_list, &tmp_val_jso);
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] redfish_get_lsw_port_num failed, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_STATISTICS, tmp_val_jso);
    return RET_OK;
}


LOCAL gint32 get_chassis_lanswitch_port_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    json_object *huawei = NULL;
    gint32 ret;

    ret = rf_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, check para and priv fail: %d", __FUNCTION__, ret);
        return ret;
    }

    *o_result_jso = json_object_new_object();
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s, *o_result_jso is NULL.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    huawei = json_object_new_object();
    
    if (huawei == NULL) {
        json_object_object_add(*o_result_jso, RFPROP_COMMON_HUAWEI, huawei);
        return HTTP_OK;
    }

    
    (void)_redfish_get_port_info(i_paras, huawei);

    
    (void)_redfish_get_port_statistic(i_paras, huawei);

    json_object_object_add(*o_result_jso, RFPROP_COMMON_HUAWEI, huawei);

    return HTTP_OK;
}

LOCAL void _redfish_show_port_autonego(GSList *input_list, json_object* huawei)
{
    guint32 autonego;

    autonego = g_variant_get_uint32((GVariant*)g_slist_nth_data(input_list, PARAMETER_AUTONEGO_POSITION));
    if (autonego == LSW_LOCAL_PORT_AUTONEG_ENABLE) {
        json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_AUTONEG, json_object_new_string("Enable"));
    } else if (autonego == LSW_LOCAL_PORT_AUTONEG_DISABLE) {
        json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_AUTONEG, json_object_new_string("Disable"));
    } else {
        json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_AUTONEG, NULL);
    }

    return;
}

LOCAL void _redfish_show_port_duplex(GSList *input_list, json_object* huawei)
{
    guint32 duplex;

    duplex = g_variant_get_uint32((GVariant*)g_slist_nth_data(input_list, PARAMETER_DUPLEX_POSITION));
    if (duplex == LSW_LOCAL_PORT_DUPLEX_FULL) {
        json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_DUPLEX, json_object_new_string("Full-Duplex"));
    } else if (duplex == LSW_LOCAL_PORT_DUPLEX_HALF) {
        json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_DUPLEX, json_object_new_string("Half-Duplex"));
    } else {
        json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_DUPLEX, NULL);
    }

    return;
}

LOCAL void _redfish_show_port_work_rate(GSList *input_list, json_object* huawei)
{
    const gchar* workrate = NULL;
    guint8 loop;

    workrate = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, PARAMETER_WORK_RATE_POSITION), NULL);
    for (loop = 0; loop < LSW_REDFISH_PORT_WORK_RATE_NUM; loop++) {
        if (strcmp(g_port_work_rate_list[loop], workrate) == 0) {
            json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_WORKRATE, json_object_new_string(workrate));
            return;
        }
    }
    json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_WORKRATE, NULL);

    return;
}

LOCAL void _redfish_show_port_mtu(GSList *input_list, json_object* huawei)
{
    guint32 mtu;

    mtu = g_variant_get_uint32((GVariant*)g_slist_nth_data(input_list, PARAMETER_PKT_LEN_POSITION));
    if (mtu <= MAX_MTU_NUMBER) {
        json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_MTU, json_object_new_int(mtu));
    } else {
        json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_MTU, NULL);
        debug_log(DLOG_ERROR, "%s failed: get PROPERTY_LSW_MAX_PKT_LEN error", __FUNCTION__);
    }

    return;
}

LOCAL void _redfish_show_port_pvid(GSList *input_list, json_object* huawei)
{
    guint32 pvid;

    pvid = g_variant_get_uint32((GVariant*)g_slist_nth_data(input_list, PARAMETER_PVID_POSITION));
    if (pvid != 0) {
        json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_PVID, json_object_new_int(pvid));
    } else {
        json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_PVID, NULL);
        debug_log(DLOG_ERROR, "%s failed: get PROPERTY_CENTER_PVID error", __FUNCTION__);
    }

    return;
}

LOCAL void _redfish_show_port_loop_mode(GSList *input_list, json_object* huawei)
{
    guint32 loop_mode;

    loop_mode = g_variant_get_uint32((GVariant*)g_slist_nth_data(input_list, PARAMETER_LOOP_MODE_POSITION));
    if (loop_mode == 0) {
        json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_LOOPMODE, json_object_new_string("NonLoop"));
    } else {
        json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_LOOPMODE, json_object_new_string("Loop"));
    }

    return;
}

LOCAL void _redfish_show_port_link_status(GSList *input_list, json_object* huawei)
{
    guint32 link_status;

    link_status = g_variant_get_uint32((GVariant*)g_slist_nth_data(input_list, PARAMETER_LINK_STAT_POSITION));
    if (link_status > LINK_UP) {
        json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_LINKSTATUS, NULL);
        debug_log(DLOG_ERROR, "%s failed: get link_status error", __FUNCTION__);
    } else if (link_status == LINK_UP) {
        json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_LINKSTATUS, json_object_new_string("LinkUp"));
    } else {
        json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_LINKSTATUS, json_object_new_string("LinkDown"));
    }

    return;
}

LOCAL void _redfish_show_port_enable_status(GSList *input_list, json_object* huawei)
{
    guint32 port_status;

    port_status = g_variant_get_uint32((GVariant*)g_slist_nth_data(input_list, PARAMETER_ENA_STAT_POSITION));
    if (port_status > LSW_LOCAL_PORT_STATUS_ENABLE) {
        json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_PORT_ENABLE, NULL);
        debug_log(DLOG_ERROR, "%s failed: get PROPERTY_LSW_STATUS error", __FUNCTION__);
    } else if (port_status == LSW_LOCAL_PORT_STATUS_ENABLE) {
        json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_PORT_ENABLE, json_object_new_string("Enable"));
    } else {
        json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_PORT_ENABLE, json_object_new_string("Disable"));
    }

    return;
}

LOCAL void redfish_show_mgt_port_link_status(PROVIDER_PARAS_S* i_paras, json_object* huawei)
{
    guint16 link_status;
    guint32 stp_state;
    guint32 port_id = 0; 
    OBJ_HANDLE obj_handle = 0;

    gint32 ret = dal_get_specific_object_uint32(CLASS_LSW_PORT_ATTR_LOCAL_CP, PROPERTY_LSW_LOGIC_PORT_NUM, port_id,
        &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get port%u object failed, ret(%d)", port_id, ret);
        return;
    }
    ret = dal_get_property_value_uint16(obj_handle, PROPERTY_LSW_LINK_STATUS, &link_status);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get port%u link status failed, ret(%d)", port_id, ret);
        return;
    }
    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_LSW_STP_STATE, &stp_state);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get port%u stp state failed, ret(%d)", port_id, ret);
        return;
    }

    if (link_status > 0x1) {
        json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_LINKSTATUS, NULL);
        debug_log(DLOG_ERROR, "Get PROPERTY_LSW_LINK_STATUS error");
    } else if (link_status != 0x1) {
        json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_LINKSTATUS, json_object_new_string("LinkDown"));
    } else if (stp_state == 0x3) { 
        json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_LINKSTATUS, json_object_new_string("LinkUp"));
    } else {
        json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_LINKSTATUS, json_object_new_string("Standby"));
    }

    return;
}

LOCAL gint32 _redfish_get_port_info(PROVIDER_PARAS_S *i_paras, json_object *huawei)
{
    gchar port_name[MAX_LSW_PORT_NAME_LEN] = {0};
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    gchar class_name[MAX_NAME_SIZE] = {0};

    ret = redfish_get_lsw_port_name(i_paras, port_name, sizeof(port_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get port_name failed, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    ret = dfl_get_class_name(i_paras->obj_handle, class_name, sizeof(class_name));
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s] dfl_get_class_name failed, port_name=%s ret=%d", __FUNCTION__, port_name, ret);
        return RET_ERR;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(port_name));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, class_name, METHOD_LSW_GET_PORT_INFO, AUTH_DISABLE,
        i_paras->user_role_privilege, input_list, &output_list);

    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != RET_OK) {
        redfish_show_mgt_port_link_status(i_paras, huawei);
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        debug_log(DLOG_ERROR, "uip_call_class_method_redfish(%s) fail, ret(%d)",
            METHOD_LSW_GET_PORT_INFO, ret);
        return RET_ERR;
    }

    _redfish_show_port_mtu(output_list, huawei);
    _redfish_show_port_pvid(output_list, huawei);
    _redfish_show_port_loop_mode(output_list, huawei);
    _redfish_show_port_link_status(output_list, huawei);
    _redfish_show_port_enable_status(output_list, huawei);
    _redfish_show_port_autonego(output_list, huawei);
    _redfish_show_port_duplex(output_list, huawei);
    _redfish_show_port_work_rate(output_list, huawei);
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    return RET_OK;
}


LOCAL gint32 redfish_check_lsw_port_name(gchar *uri, const gchar *blade_name, OBJ_HANDLE *object_handle)
{
    gint32 ret;
    gchar port_name[MAX_LSW_PORT_NAME_LEN] = {0};
    gchar port_real_name[MAX_LSW_PORT_NAME_LEN] = {0};
    GSList* obj_list = NULL;
    GSList* obj_node = NULL;
    OBJ_HANDLE obj_tmp_handle;

    ret = rf_get_uri_info_by_position(uri, PORT_NAME_POSITION, port_name, MAX_LSW_PORT_NAME_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, get uri info fail: %d.", __FUNCTION__, ret);
        return ret;
    }

    ret = dfl_get_object_list(CLASS_LSW_PORT_ATTR_CENTER, &obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s, get obj_list fail: %d.", __FUNCTION__, ret);
        return ret;
    }

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        obj_tmp_handle = (OBJ_HANDLE)obj_node->data;

        ret = rf_check_lsw_port_by_blade(obj_tmp_handle, blade_name);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "Check %s port validity fail: %d.", dfl_get_object_name(obj_tmp_handle), ret);
            continue;
        }

        ret = rf_check_lsw_port_by_position(obj_tmp_handle, port_real_name, sizeof(port_real_name));
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "Check %s port name fail: %d.", dfl_get_object_name(obj_tmp_handle), ret);
            continue;
        }

        if (g_ascii_strcasecmp(port_name, port_real_name) == 0) {
            *object_handle = obj_tmp_handle;
            g_slist_free(obj_list);
            return RET_OK;
        }
    }

    g_slist_free(obj_list);
    return RET_ERR;
}


LOCAL gint32 redfish_get_lsw_port_name(PROVIDER_PARAS_S *i_paras, gchar *port_name, gsize max_port_name_len)
{
    gint32 ret;
    gchar class_name[MAX_NAME_SIZE] = {0};

    ret = dfl_get_class_name(i_paras->obj_handle, class_name, sizeof(class_name));
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s] dfl_get_class_name failed, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    if (g_strcmp0(class_name, CLASS_LSW_PORT_ATTR_CENTER) == 0) {
        ret = dal_get_property_value_string(i_paras->obj_handle, PROPERTY_LSW_CENTER_PORT_NAME,
                                            port_name, max_port_name_len);
    } else if (g_strcmp0(class_name, CLASS_LSW_PORT_ATTR_LOCAL_CP) == 0) {
        ret = dal_get_property_value_string(i_paras->obj_handle, PROPERTY_LSW_SUB_PORT_NAME,
                                            port_name, max_port_name_len);
    } else {
        debug_log(DLOG_ERROR, "[%s] invalid class_name: %s", __FUNCTION__, class_name);
        return RET_ERR;
    }
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s] get port_name failed, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    return RET_OK;
}


gint32 lanswitch_port_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gint32 ret;
    const gchar *blade_name = NULL;
    OBJ_HANDLE object_handle;

    ret = rf_check_lsw_resource_legality(i_paras, prop_provider, count);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s, check resource legality fail: %d", __FUNCTION__, ret);
        return ret;
    }

    ret = rf_check_lsw_blade_name(i_paras->uri, &blade_name);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, uri blade_name is not match: %d.", __FUNCTION__, ret);
        return HTTP_NOT_FOUND;
    }

    ret = redfish_check_lsw_port_name(i_paras->uri, blade_name, &object_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, uri port_name is not match: %d.", __FUNCTION__, ret);
        return HTTP_NOT_FOUND;
    }

    i_paras->obj_handle = object_handle;

    *prop_provider = g_chassis_lanswitch_port_provider;
    *count = sizeof(g_chassis_lanswitch_port_provider) / sizeof(PROPERTY_PROVIDER_S);

    return RET_OK;
}

LOCAL gint32 get_chassis_lanswitch_port_action_odata_id(PROVIDER_PARAS_S* i_paras,
                                                        json_object** o_message_jso, json_object** o_result_jso)
{
    gint32 ret;
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar port_name[MAX_LSW_PORT_NAME_LEN] = {0};
    gchar blade_name[MAX_NAME_SIZE] = {0};

    ret = rf_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, check para and priv fail: %d", __FUNCTION__, ret);
        return ret;
    }

    ret = rf_check_format_blade_name(i_paras, blade_name, sizeof(blade_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, check or format blade_name fail: %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = redfish_get_lsw_port_name(i_paras, port_name, sizeof(port_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, get port_name fail: %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, URI_FORMAT_LSW_SET_PORT_ENABLE_INFO, blade_name, port_name);
    if (ret <= RET_OK) {
        debug_log(DLOG_ERROR, "%s, snprintf_s fail, ret = %d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const gchar*)uri);
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s, json_object_new_string fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}

gint32 lanswitch_port_enable_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    const gchar *blade_name = NULL;
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;

    ret = rf_check_lsw_resource_legality(i_paras, prop_provider, count);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s, check resource legality fail: %d", __FUNCTION__, ret);
        return ret;
    }

    ret = rf_check_lsw_blade_name(i_paras->uri, &blade_name);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, uri blade_name is not match: %d.", __FUNCTION__, ret);
        return HTTP_NOT_FOUND;
    }

    ret = redfish_check_lsw_port_name(i_paras->uri, blade_name, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, uri port_name is not match: %d.", __FUNCTION__, ret);
        return HTTP_NOT_FOUND;
    }

    i_paras->obj_handle = obj_handle;
    *prop_provider = g_chassis_lanswitch_port_action_provider;
    *count = sizeof(g_chassis_lanswitch_port_action_provider) / sizeof(PROPERTY_PROVIDER_S);

    return RET_OK;
}

LOCAL gint32 get_port_obj_by_uri_and_class(gchar *uri, const gchar *plane_port_class, OBJ_HANDLE *obj_handle)
{
    gchar port_name[MAX_LSW_PORT_NAME_LEN] = {0};
    gint32 ret;
    OBJ_HANDLE tmp_handle = 0;
    gchar *tmp_name = NULL;
    GVariant* property_value = NULL;
    guint32 port_id = 0;

    ret = rf_get_uri_info_by_position(uri, PORT_NAME_POSITION, port_name, sizeof(port_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get uri info failed, ret=%d", __FUNCTION__, ret);
        return ret;
    }

    tmp_name = g_ascii_strup(port_name, strlen(port_name));
    if (tmp_name == NULL) {
        debug_log(DLOG_ERROR, "[%s] g_ascii_strup failed", __FUNCTION__);
        return RET_ERR;
    }

    property_value = g_variant_new_string((const gchar*)tmp_name);
    g_free(tmp_name);
    tmp_name = NULL;

    ret = dfl_get_specific_object(plane_port_class, PROPERTY_LSW_SUB_PORT_NAME, property_value, &tmp_handle);
    g_variant_unref(property_value);
    property_value = NULL;
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s] dfl_get_specific_object failed, ret=%d", __FUNCTION__, ret);
        return ret;
    }

    ret = dal_get_property_value_uint32(tmp_handle, PROPERTY_LSW_LOGIC_PORT_NUM, &port_id);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s] get property %s failed, ret=%d",
                  __FUNCTION__, dfl_get_object_name(tmp_handle), ret);
        return ret;
    }

    
    if (port_id == 0xffffffff) {
        debug_log(DLOG_DEBUG, "[%s] invalid logic port %s", __FUNCTION__, dfl_get_object_name(tmp_handle));
        return RET_ERR;
    }

    *obj_handle = tmp_handle;
    return RET_OK;
}

LOCAL gint32 get_port_obj_by_uri(gchar *uri, OBJ_HANDLE *obj_handle)
{
    gint32 ret;
    gchar sw_name_uri[MAX_NAME_SIZE] = {0};
    guint8 plane;
    const gchar *plane_port_class[MAX_LSW_PLANE_NUM] = { CLASS_LSW_PORT_ATTR_LOCAL_CP, CLASS_LSW_PORT_ATTR_LOCAL_DP };

    ret = rf_get_uri_info_by_position(uri, PLANE_NAME_POSITION, sw_name_uri, sizeof(sw_name_uri));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get uri info failed, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }
    plane = (g_ascii_strcasecmp(sw_name_uri, RFPROP_CHASSIS_LANSWITCH_LSW_NAME) == 0) ?
            LSW_CONTROL_PLANE_ID : LSW_DATA_PLANE_ID;

    ret = get_port_obj_by_uri_and_class(uri, plane_port_class[plane], obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get plane%d port handle failed, ret=%d", __FUNCTION__, plane, ret);
        return RET_ERR;
    }

    return RET_OK;
}

LOCAL gint32 get_chassis_lanswitch_portinfo_id(PROVIDER_PARAS_S* i_paras,
                                               json_object** o_message_jso, json_object** o_result_jso)
{
    gchar port_name[MAX_LSW_PORT_NAME_LEN] = {0};
    gint32 ret;
    gchar *tmp_name = NULL;

    ret = rf_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, check para and priv fail: %d", __FUNCTION__, ret);
        return ret;
    }

    ret = rf_get_uri_info_by_position(i_paras->uri, PORT_NAME_POSITION, port_name, sizeof(port_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, get uri info fail: %d.", __FUNCTION__, ret);
        return ret;
    }

    tmp_name = g_ascii_strup(port_name, strlen(port_name));
    if (tmp_name == NULL) {
        debug_log(DLOG_ERROR, "%s, g_ascii_strup fail.", __FUNCTION__);
        return VOS_ERR;
    }

    *o_result_jso = json_object_new_string((const gchar*)tmp_name);
    g_free(tmp_name);
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s, json_object_new_string fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}

LOCAL gint32 get_chassis_lanswitch_portinfo_maxspeed(PROVIDER_PARAS_S* i_paras,
                                                     json_object** o_message_jso, json_object** o_result_jso)
{
    guint32 max_speed = 0;
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    ret = rf_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, check para and priv fail: %d", __FUNCTION__, ret);
        return ret;
    }

    ret = get_port_obj_by_uri(i_paras->uri, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, get_port_obj_by_uri fail: %d.", __FUNCTION__, ret);
        return ret;
    }

    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_LSW_SPEED, &max_speed);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, get PROPERTY_LSW_SPEED fail: %d.", __FUNCTION__, ret);
        return ret;
    }

    *o_result_jso = json_object_new_int(max_speed / LSW_LOCAL_PORT_SPEEDSTRAMS);
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s, json_object_new_string fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}

LOCAL gint32 get_actions_info_by_uri(gchar *uri, gchar *target, size_t target_size,
                                     gchar *action, size_t action_size)
{
    gint32 ret;
    gchar blade_name[MAX_NAME_SIZE] = {0};
    gchar switch_name[MAX_NAME_SIZE] = {0};
    gchar port_name[MAX_LSW_PORT_NAME_LEN] = {0};

    ret = rf_get_uri_info_by_position(uri, BLADE_NAME_POSITION, blade_name, sizeof(blade_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get blade name from uri failed, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }
    format_string_as_upper_head(blade_name, strlen(blade_name));
    ret = rf_get_uri_info_by_position(uri, PLANE_NAME_POSITION, switch_name, sizeof(switch_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get switch name from uri failed, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }
    format_string_as_upper_head(switch_name, strlen(switch_name));
    ret = rf_get_uri_info_by_position(uri, PORT_NAME_POSITION, port_name, sizeof(port_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get port name from uri failed, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }
    format_string_as_upper_head(port_name, strlen(port_name));

    ret = sprintf_s(target, target_size, URI_FORMAT_BLADE_LSW_SET_PORT_ENABLE_TARGET,
                    blade_name, switch_name, port_name);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "[%s] snprintf_s(target) failed, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }
    ret = sprintf_s(action, action_size, URI_FORMAT_BLADE_LSW_SET_PORT_ENABLE_INFO,
                    blade_name, switch_name, port_name);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "[%s] snprintf_s(action) failed, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    return RET_OK;
}

LOCAL gint32 get_chassis_lanswitch_portinfo_actions(PROVIDER_PARAS_S *i_paras,
                                                    json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar target[MAX_URI_LEN] = {0};
    gchar action_info[MAX_URI_LEN] = {0};
    gchar switch_name_uri[MAX_NAME_SIZE] = {0};
    json_object *oem = NULL;
    json_object *huawei = NULL;
    json_object *action = NULL;

    ret = rf_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] check para and priv failed, ret=%d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = rf_get_uri_info_by_position(i_paras->uri, PLANE_NAME_POSITION, switch_name_uri, sizeof(switch_name_uri));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get switch name from uri failed, ret=%d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if (g_ascii_strcasecmp(switch_name_uri, RFPROP_CHASSIS_LANSWITCH_LSW_NAME) != 0) {
        
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = get_actions_info_by_uri(i_paras->uri, target, sizeof(target), action_info, sizeof(action_info));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get_actions_info_by_uri failed, ret=%d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    oem = json_object_new_object();
    json_object_object_add(*o_result_jso, RFPROP_OEM, oem);
    if (oem == NULL) {
        debug_log(DLOG_ERROR, "[%s] json_object_object_add (oem) failed", __FUNCTION__);
        return HTTP_OK;
    }

    
    huawei = json_object_new_object();
    json_object_object_add(oem, RFPROP_OEM_HUAWEI, huawei);
    if (huawei == NULL) {
        debug_log(DLOG_ERROR, "[%s] json_object_object_add (huawei) failed", __FUNCTION__);
        return HTTP_OK;
    }

    action = json_object_new_object();
    json_object_object_add(huawei, RFPROP_CHASSIS_LSW_SET_PORT_ENABLE, action);
    if (action == NULL) {
        debug_log(DLOG_ERROR, "[%s] json_object_object_add (action) failed", __FUNCTION__);
        return HTTP_OK;
    }

    json_object_object_add(action, RFPROP_TARGET, json_object_new_string((const gchar*)target));
    json_object_object_add(action, RFPROP_ACTION_INFO, json_object_new_string((const gchar*)action_info));

    return HTTP_OK;
}

LOCAL gint32 get_chassis_lanswitch_portinfo_action_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
                                                            json_object **o_result_jso)
{
    gint32 ret;
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar switch_name[MAX_NAME_SIZE] = {0};
    gchar blade_name[MAX_NAME_SIZE] = {0};
    gchar port_name[MAX_LSW_PORT_NAME_LEN] = {0};

    ret = rf_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] check para and priv failed, ret=%d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = rf_get_uri_info_by_position(i_paras->uri, BLADE_NAME_POSITION, blade_name, sizeof(blade_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get blade name from uri failed, ret=%d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    format_string_as_upper_head(blade_name, strlen(blade_name));
    ret = rf_get_uri_info_by_position(i_paras->uri, PLANE_NAME_POSITION, switch_name, sizeof(switch_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get switch name from uri failed, ret=%d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    format_string_as_upper_head(switch_name, strlen(switch_name));
    ret = rf_get_uri_info_by_position(i_paras->uri, PORT_NAME_POSITION, port_name, sizeof(port_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get port name from uri failed, ret=%d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    format_string_as_upper_head(port_name, strlen(port_name));

    ret = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, URI_FORMAT_BLADE_LSW_SET_PORT_ENABLE_INFO, blade_name,
                     switch_name, port_name);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "[%s] snprintf_s failed, ret=%d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const char*)uri);
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "[%s] json_object_new_string failed", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}

LOCAL gint32 _redfish_get_portinfo_pvid(PROVIDER_PARAS_S* i_paras, OBJ_HANDLE obj_handle, json_object* huawei)
{
    gchar port_name[MAX_LSW_PORT_NAME_LEN] = {0};
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    guint16 pvid;

    ret = dal_get_property_value_string(obj_handle, PROPERTY_LSW_SUB_PORT_NAME, port_name, sizeof(port_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Failed : get PROPERTY_LSW_SUB_PORT_NAME failed: %d.", __FUNCTION__, ret);
        return ret;
    }

    input_list = g_slist_append(input_list, g_variant_new_string((const gchar*)port_name));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        obj_handle, CLASS_LSW_PORT_ATTR_LOCAL_DP, METHOD_LSW_GET_PORT_PVID, AUTH_DISABLE,
        i_paras->user_role_privilege, input_list, &output_list);

    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, uip_call_class_method_redfish(%s) fail.", __FUNCTION__, METHOD_LSW_GET_PORT_PVID);
        return ret;
    }

    pvid = g_variant_get_uint16((GVariant*)g_slist_nth_data(output_list, 0));
    json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_PVID, json_object_new_int(pvid));

    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    return VOS_OK;
}

LOCAL gint32 _redfish_get_portinfo_mtu(PROVIDER_PARAS_S* i_paras, OBJ_HANDLE obj_handle, json_object* huawei)
{
    gchar port_name[MAX_LSW_PORT_NAME_LEN] = {0};
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    guint32 mtu;

    ret = dal_get_property_value_string(obj_handle, PROPERTY_LSW_SUB_PORT_NAME, port_name, sizeof(port_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Failed : get PROPERTY_LSW_SUB_PORT_NAME failed: %d.", __FUNCTION__, ret);
        return ret;
    }

    input_list = g_slist_append(input_list, g_variant_new_string((const gchar*)port_name));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        obj_handle, CLASS_LSW_PORT_ATTR_LOCAL_DP, METHOD_LSW_GET_PORT_MTU, AUTH_DISABLE,
        i_paras->user_role_privilege, input_list, &output_list);

    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, uip_call_class_method_redfish(%s) fail.", __FUNCTION__, METHOD_LSW_GET_PORT_MTU);
        return ret;
    }

    mtu = g_variant_get_uint32((GVariant*)g_slist_nth_data(output_list, 0));
    json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_MTU, json_object_new_int(mtu));

    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    return VOS_OK;
}

LOCAL gint32 _redfish_get_portinfo_loop_mode(PROVIDER_PARAS_S* i_paras, OBJ_HANDLE obj_handle, json_object* huawei)
{
    gchar port_name[MAX_LSW_PORT_NAME_LEN] = {0};
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    guint32 loop_mode;

    ret = dal_get_property_value_string(obj_handle, PROPERTY_LSW_SUB_PORT_NAME, port_name, sizeof(port_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Failed : get PROPERTY_LSW_SUB_PORT_NAME failed: %d.", __FUNCTION__, ret);
        return ret;
    }

    input_list = g_slist_append(input_list, g_variant_new_string((const gchar*)port_name));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        obj_handle, CLASS_LSW_PORT_ATTR_LOCAL_DP, METHOD_LSW_GET_LOOP_MODE, AUTH_DISABLE,
        i_paras->user_role_privilege, input_list, &output_list);

    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, uip_call_class_method_redfish(%s) fail.", __FUNCTION__, METHOD_LSW_GET_LOOP_MODE);
        return ret;
    }

    loop_mode = g_variant_get_uint32((GVariant*)g_slist_nth_data(output_list, 0));
    if (loop_mode == 0) {
        json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_LOOPMODE, json_object_new_string("NonLoop"));
    } else {
        json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_LOOPMODE, json_object_new_string("Loop"));
    }

    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    return VOS_OK;
}

LOCAL gint32 _redfish_get_portinfo_enable_status(PROVIDER_PARAS_S* i_paras, OBJ_HANDLE obj_handle, json_object* huawei)
{
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    guint32 link_status;
    guint32 port_status;
    guint32 port_id = 0;
    OBJ_HANDLE tmp_handle = 0;
    const guint32 link_status_id = 0x2;
    const guint32 port_status_id = 0x3;

    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_LSW_LOGIC_PORT_NUM, &port_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Failed : get PROPERTY_LSW_LOGIC_PORT_NUM failed: %d.", __FUNCTION__, ret);
        return ret;
    }

    ret = dal_get_object_handle_nth(CLASS_LSW_PUBLIC_ATTR_LOCAL_DP, 0, &tmp_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Failed : dal_get_object_handle_nth failed: %d.", __FUNCTION__, ret);
        return ret;
    }

    input_list = g_slist_append(input_list, g_variant_new_uint32(port_id));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        tmp_handle, CLASS_LSW_PUBLIC_ATTR_LOCAL_DP, METHOD_DATA_INFO, AUTH_DISABLE,
        i_paras->user_role_privilege, input_list, &output_list);

    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, uip_call_class_method_redfish(%s) fail.", __FUNCTION__, METHOD_DATA_INFO);
        return ret;
    }

    link_status = g_variant_get_uint32((GVariant*)g_slist_nth_data(output_list, link_status_id));
    if (link_status > LINK_UP) {
        json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_LINKSTATUS, NULL);
    } else if (link_status == LINK_UP) {
        json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_LINKSTATUS, json_object_new_string("LinkUp"));
    } else {
        json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_LINKSTATUS, json_object_new_string("LinkDown"));
    }

    port_status = g_variant_get_uint32((GVariant*)g_slist_nth_data(output_list, port_status_id));
    if (port_status > LSW_LOCAL_PORT_STATUS_ENABLE) {
        json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_PORT_ENABLE, NULL);
    } else if (port_status == LSW_LOCAL_PORT_STATUS_ENABLE) {
        json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_PORT_ENABLE, json_object_new_string("Enable"));
    } else {
        json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_PORT_ENABLE, json_object_new_string("Disable"));
    }

    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    return RET_OK;
}

LOCAL gint32 _redfish_get_portinfo_peer(PROVIDER_PARAS_S* i_paras, OBJ_HANDLE obj_handle, json_object* huawei)
{
    gchar port_name[MAX_LSW_PORT_NAME_LEN] = {0};
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    const gchar* peer_bladeid = NULL;
    const gchar* peer_portid = NULL;

    ret = dal_get_property_value_string(obj_handle, PROPERTY_LSW_SUB_PORT_NAME, port_name, sizeof(port_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Failed : get PROPERTY_LSW_SUB_PORT_NAME failed: %d.", __FUNCTION__, ret);
        return ret;
    }

    input_list = g_slist_append(input_list, g_variant_new_string((const gchar*)port_name));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        obj_handle, CLASS_LSW_PORT_ATTR_LOCAL_DP, METHOD_LSW_GET_PORT_PEER, AUTH_DISABLE,
        i_paras->user_role_privilege, input_list, &output_list);

    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, uip_call_class_method_redfish(%s) fail.", __FUNCTION__, METHOD_LSW_GET_PORT_PEER);
        return ret;
    }

    peer_bladeid = g_variant_get_string((GVariant*)g_slist_nth_data(output_list, 0), NULL);
    json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_PEERBLADEID, json_object_new_string(peer_bladeid));

    peer_portid = g_variant_get_string((GVariant*)g_slist_nth_data(output_list, 1), NULL);
    json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_PEERPORTID, json_object_new_string(peer_portid));

    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    return RET_OK;
}

LOCAL gint32 _redfish_get_portinfo_info(PROVIDER_PARAS_S* i_paras, OBJ_HANDLE obj_handle, json_object* huawei)
{
    gchar port_name[MAX_LSW_PORT_NAME_LEN] = {0};
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;

    ret = dal_get_property_value_string(obj_handle, PROPERTY_LSW_SUB_PORT_NAME, port_name, sizeof(port_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Failed : get PROPERTY_LSW_SUB_PORT_NAME failed: %d.", __FUNCTION__, ret);
        return ret;
    }

    input_list = g_slist_append(input_list, g_variant_new_string((const gchar*)port_name));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        obj_handle, CLASS_LSW_PORT_ATTR_LOCAL_DP, METHOD_LSW_GET_PORT_INFO, AUTH_DISABLE,
        i_paras->user_role_privilege, input_list, &output_list);

    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, uip_call_class_method_redfish(%s) fail.", __FUNCTION__, METHOD_LSW_GET_PORT_INFO);
        return ret;
    }

    _redfish_show_port_autonego(output_list, huawei);
    _redfish_show_port_duplex(output_list, huawei);
    _redfish_show_port_work_rate(output_list, huawei);

    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    return RET_OK;
}

LOCAL gint32 _redfish_get_portinfo_statistic(PROVIDER_PARAS_S* i_paras, OBJ_HANDLE obj_handle, json_object* huawei)
{
    gchar port_name[MAX_LSW_PORT_NAME_LEN] = {0};
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    json_object* tmp_val_jso = NULL;

    ret = dal_get_property_value_string(obj_handle, PROPERTY_LSW_SUB_PORT_NAME, port_name, sizeof(port_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Failed : get PROPERTY_LSW_SUB_PORT_NAME failed: %d.", __FUNCTION__, ret);
        return ret;
    }

    input_list = g_slist_append(input_list, g_variant_new_string((const gchar*)port_name));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        obj_handle, CLASS_LSW_PORT_ATTR_LOCAL_DP, METHOD_LSW_GET_PORT_STATISTICS, AUTH_DISABLE,
        i_paras->user_role_privilege, input_list, &output_list);

    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, uip_call_class_method_redfish(%s) fail.", __FUNCTION__, METHOD_LSW_GET_PORT_PEER);
        return ret;
    }

    ret = redfish_get_lsw_port_num(output_list, &tmp_val_jso);
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, redfish_get_lsw_port_num fail: %d.", __FUNCTION__, ret);
        return ret;
    }

    json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_STATISTICS, tmp_val_jso);
    return RET_OK;
}

LOCAL void _redfish_get_mesh_port_all_info(PROVIDER_PARAS_S* i_paras, OBJ_HANDLE obj_handle, json_object* huawei)
{
    gint32 ret;

    
    ret = _redfish_get_portinfo_mtu(i_paras, obj_handle, huawei);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Failed : _redfish_get_portinfo_mtu failed: %d.", __FUNCTION__, ret);
    }

    
    ret = _redfish_get_portinfo_pvid(i_paras, obj_handle, huawei);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Failed : _redfish_get_portinfo_pvid failed: %d.", __FUNCTION__, ret);
    }

    
    ret = _redfish_get_portinfo_loop_mode(i_paras, obj_handle, huawei);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Failed : _redfish_get_portinfo_loop_mode failed: %d.", __FUNCTION__, ret);
    }

    
    ret = _redfish_get_portinfo_enable_status(i_paras, obj_handle, huawei);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Failed : _redfish_get_portinfo_enable_status failed: %d.", __FUNCTION__, ret);
    }

    
    ret = _redfish_get_portinfo_info(i_paras, obj_handle, huawei);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Failed : _redfish_get_portinfo_info failed: %d.", __FUNCTION__, ret);
    }

    
    ret = _redfish_get_portinfo_statistic(i_paras, obj_handle, huawei);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Failed : _redfish_get_portinfo_statistic failed: %d.", __FUNCTION__, ret);
    }

    return;
}

LOCAL void _redfish_get_port_all_info(PROVIDER_PARAS_S* i_paras, OBJ_HANDLE obj_handle, json_object* huawei)
{
    gint32 ret;
    gchar sw_name_uri[MAX_NAME_SIZE] = {0};

    ret = rf_get_uri_info_by_position(i_paras->uri, PLANE_NAME_POSITION, sw_name_uri, sizeof(sw_name_uri));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get uri info failed, ret=%d", __FUNCTION__, ret);
        return;
    }

    if (g_ascii_strcasecmp(sw_name_uri, RFPROP_CHASSIS_LANSWITCH_LSW_NAME) == 0) {
        (void)_redfish_get_port_info(i_paras, huawei);
        (void)_redfish_get_port_statistic(i_paras, huawei);
    } else {
        _redfish_get_mesh_port_all_info(i_paras, obj_handle, huawei);
    }
}

LOCAL gint32 get_chassis_lanswitch_portinfo_oem(PROVIDER_PARAS_S* i_paras,
                                                json_object** o_message_jso, json_object** o_result_jso)
{
    json_object* huawei = NULL;
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gchar port_name[MAX_LSW_PORT_NAME_LEN] = {0};

    ret = rf_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, check para and priv fail: %d", __FUNCTION__, ret);
        return ret;
    }

    ret = get_port_obj_by_uri(i_paras->uri, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, get_port_obj_by_uri fail: %d.", __FUNCTION__, ret);
        return HTTP_NOT_FOUND;
    }

    ret = dal_get_property_value_string(obj_handle, PROPERTY_LSW_SUB_PORT_NAME, port_name, sizeof(port_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Failed : get PROPERTY_LSW_SUB_PORT_NAME failed: %d.", __FUNCTION__, ret);
        return HTTP_NOT_FOUND;
    }

    *o_result_jso = json_object_new_object();
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s, json_object_new_string fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    huawei = json_object_new_object();
    
    if (huawei == NULL) {
        json_object_object_add(*o_result_jso, RFPROP_COMMON_HUAWEI, huawei);
        return HTTP_OK;
    }

    if (strstr(port_name, "MESH") != NULL) {
        ret = _redfish_get_portinfo_peer(i_paras, obj_handle, huawei);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "[%s] Failed : _redfish_get_portinfo_peer failed: %d.", __FUNCTION__, ret);
        }
    }

    
    _redfish_get_port_all_info(i_paras, obj_handle, huawei);

    json_object_object_add(*o_result_jso, RFPROP_COMMON_HUAWEI, huawei);
    return HTTP_OK;
}

LOCAL gint32 get_chassis_lanswitch_portinfo_odata_id(PROVIDER_PARAS_S* i_paras,
                                                     json_object** o_message_jso, json_object** o_result_jso)
{
    gint32 ret;
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar blade_name[MAX_NAME_SIZE] = {0};
    gchar port_name[MAX_LSW_PORT_NAME_LEN] = {0};
    gchar slot[MAX_NAME_SIZE] = {0};

    ret = rf_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s, check para and priv fail: %d", __FUNCTION__, ret);
        return ret;
    }

    ret = rf_get_uri_info_by_position(i_paras->uri, BLADE_NAME_POSITION, slot, sizeof(slot));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, get uri info fail: %d.", __FUNCTION__, ret);
        return ret;
    }
    format_string_as_upper_head(slot, strlen(slot));

    ret = rf_get_uri_info_by_position(i_paras->uri, PLANE_NAME_POSITION, blade_name, sizeof(blade_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, get uri info fail: %d.", __FUNCTION__, ret);
        return ret;
    }
    format_string_as_upper_head(blade_name, strlen(blade_name));

    ret = rf_get_uri_info_by_position(i_paras->uri, PORT_NAME_POSITION, port_name, sizeof(port_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, get uri info fail: %d.", __FUNCTION__, ret);
        return ret;
    }
    format_string_as_upper_head(port_name, strlen(port_name));

    ret = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, URI_PATTERN_CHASSIS_MESHSWITCH_PORTS, slot, blade_name,
                     port_name);
    if (ret <= RET_OK) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail: %d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const char*)uri);
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_string fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}

LOCAL gint32 check_chassis_switch_port_uri(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
                                           guint32 *count)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gchar switch_name[MAX_NAME_SIZE] = {0};

    if ((i_paras == NULL) || (prop_provider == NULL) || (count == NULL)) {
        debug_log(DLOG_ERROR, "[%s] param error", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = rf_check_blade_name(i_paras->uri);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] uri blade_name is not match, ret=%d", __FUNCTION__, ret);
        return ret;
    }

    ret = rf_check_switch_name(i_paras->uri);
    if (ret != RET_OK) {
        return ret;
    }

    ret = get_port_obj_by_uri(i_paras->uri, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] uri port_name is not match, ret=%d", __FUNCTION__, ret);
        return HTTP_NOT_FOUND;
    }

    ret = rf_get_uri_info_by_position(i_paras->uri, PLANE_NAME_POSITION, switch_name, sizeof(switch_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get switch name from uri failed, ret=%d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    ret = rf_check_port_id_and_type(obj_handle, (const gchar*)switch_name);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] rf_check_port_id_and_type failed, ret=%d", __FUNCTION__, ret);
        return ret;
    }

    i_paras->obj_handle = obj_handle;

    return RET_OK;
}

gint32 chassis_switch_port_provider_entry(PROVIDER_PARAS_S* i_paras, PROPERTY_PROVIDER_S** prop_provider,
                                          guint32* count)
{
    gint32 ret;

    ret = check_chassis_switch_port_uri(i_paras, prop_provider, count);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] check_chassis_switch_port_uri failed, ret=%d", __FUNCTION__, ret);
        return ret;
    }

    *prop_provider = g_chassis_lanswitch_portinfo_provider;
    *count = sizeof(g_chassis_lanswitch_portinfo_provider) / sizeof(PROPERTY_PROVIDER_S);

    return RET_OK;
}

gint32 chassis_switch_port_enable_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gint32 ret;
    gchar switch_name_uri[MAX_NAME_SIZE] = {0};

    ret = check_chassis_switch_port_uri(i_paras, prop_provider, count);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] check_chassis_switch_port_uri failed, ret=%d", __FUNCTION__, ret);
        return ret;
    }

    ret = rf_get_uri_info_by_position(i_paras->uri, PLANE_NAME_POSITION, switch_name_uri, sizeof(switch_name_uri));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get switch name from uri failed, ret=%d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if (g_ascii_strcasecmp(switch_name_uri, RFPROP_CHASSIS_LANSWITCH_LSW_NAME) != 0) {
        
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_chassis_switch_port_action_provider;
    *count = sizeof(g_chassis_switch_port_action_provider) / sizeof(PROPERTY_PROVIDER_S);

    return RET_OK;
}

#endif
