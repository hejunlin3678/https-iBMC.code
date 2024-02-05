
#ifdef ARM64_HI1711_ENABLED

#include "redfish_provider.h"
#include "redfish_provider_chassis.h"

LOCAL gint32 get_chassis_lanswitch_ports(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 get_chassis_lanswitch_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 get_chassis_lanswitch_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 get_chassis_meshswitch_ports(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso);
LOCAL gint32 get_chassis_meshswitch_oem(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso);
LOCAL gint32 get_chassis_meshswitch_odata_id(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_chassis_lanswitch_provider[] = {
    { RFPROP_CHASSIS_LANSWITCH_PORTS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
      get_chassis_lanswitch_ports, NULL, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_COMMON_OEM, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID,
      get_chassis_lanswitch_oem, NULL, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
      get_chassis_lanswitch_odata_id, NULL, NULL, ETAG_FLAG_ENABLE }
};

LOCAL PROPERTY_PROVIDER_S g_chassis_meshswitch_provider[] = {
    { RFPROP_CHASSIS_LANSWITCH_PORTS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
      get_chassis_meshswitch_ports, NULL, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_COMMON_OEM, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID,
      get_chassis_meshswitch_oem, NULL, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
      get_chassis_meshswitch_odata_id, NULL, NULL, ETAG_FLAG_ENABLE }
};

/*****************************************************************************
* Function    : get_chassis_lanswitch_ports
* Description : 获取lanswitch 管理资源的ports属性值
* Return      : gint32
*      Creat by m00446762 for  lsw 2019-8-27
*****************************************************************************/
LOCAL gint32 get_chassis_lanswitch_ports(PROVIDER_PARAS_S* i_paras,
                                         json_object** o_message_jso, json_object** o_result_jso)
{
    gchar uri[MAX_URI_LENGTH] = {0};
    gint32 ret;
    gchar blade_format[MAX_NAME_SIZE] = {0};

    ret = rf_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, check para and priv fail: %d", __FUNCTION__, ret);
        return ret;
    }

    ret = rf_check_format_blade_name(i_paras, blade_format, sizeof(blade_format));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, check or format blade_name fail: %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, URI_FORMAT_LANSWITCH_PORTS, blade_format);
    if (ret <= RET_OK) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    json_object_object_add(*o_result_jso, RFPROP_ODATA_ID, json_object_new_string(uri));
    return HTTP_OK;
}


LOCAL gint32 get_chasis_port_status(PROVIDER_PARAS_S *i_paras, json_object *huawei)
{
    gint32 ret;
    guint8 id = 0;
    const gchar *blade_name = NULL;
    guint8 node_status = 0;
    OBJ_HANDLE obj_handle = 0;
    OBJ_HANDLE tmp_handle = 0;

    ret = rf_check_lsw_blade_name(i_paras->uri, &blade_name);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, uri blade_name is not match: %d.", __FUNCTION__, ret);
        return ret;
    }

    ret = dfl_get_object_handle(blade_name, &tmp_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s] Failed : dfl_get_object_handle failed: %d", __FUNCTION__, ret);
        return ret;
    }

    ret = dal_get_property_value_byte(tmp_handle, PROPERTY_IPMBETH_BLADE_ID, &id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Failed : get IPMBETH_BLADE_ID failed: %d.", __FUNCTION__, ret);
        return ret;
    }

    ret = dal_get_specific_object_byte(CLASS_LSW_NODE_MANAGE_CENTER, PROPERTY_LSW_NODE_SLOTID, id, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_MASS, "Failed : %s get LSW_NODE_SLOTID %d failed: %d.", CLASS_LSW_NODE_MANAGE_CENTER, id, ret);
        return ret;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_LSW_NODE_VALID, &node_status);
    if (ret == RET_OK) {
        if (node_status == LSW_NODE_VALID) {
            json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_STATUS, json_object_new_string("Valid"));
        } else {
            json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_STATUS, json_object_new_string("InValid"));
        }
    } else {
        json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_STATUS, NULL);
    }

    return RET_OK;
}

LOCAL gint32 get_chassis_lanswitch_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    json_object *huawei = NULL;

    ret = rf_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, rf_check_para_and_priv fail: %d", __FUNCTION__, ret);
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

    
    (void)get_chasis_port_status(i_paras, huawei);

    json_object_object_add(*o_result_jso, RFPROP_COMMON_HUAWEI, huawei);
    return HTTP_OK;
}


LOCAL gint32 get_chassis_lanswitch_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar uri_info[MAX_URI_LENGTH] = {0};
    gchar blade_name_format[MAX_NAME_SIZE] = {0};

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

    ret = snprintf_s(uri_info, sizeof(uri_info), sizeof(uri_info) - 1,
        URI_FORMAT_LANSWITCH_COLLECTION_ODATA_ID, blade_name_format);
    if (ret <= RET_OK) {
        debug_log(DLOG_ERROR, "%s, snprintf_s fail, ret = %d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const char*)uri_info);
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s, json_object_new_string fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}

LOCAL gint32 get_chassis_meshswitch_ports(PROVIDER_PARAS_S* i_paras,
                                          json_object** o_message_jso, json_object** o_result_jso)
{
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar blade_name_uri[MAX_NAME_SIZE] = {0};
    gint32 ret;
    gchar slot[MAX_NAME_SIZE] = {0};

    ret = rf_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, check para and priv fail: %d", __FUNCTION__, ret);
        return ret;
    }

    ret = rf_get_uri_info_by_position(i_paras->uri, PLANE_NAME_POSITION, blade_name_uri, MAX_NAME_SIZE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, get uri info fail: %d.", __FUNCTION__, ret);
        return ret;
    }
    format_string_as_upper_head(blade_name_uri, strlen(blade_name_uri));

    ret = rf_get_uri_info_by_position(i_paras->uri, BLADE_NAME_POSITION, slot, sizeof(slot));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, get uri info fail: %d.", __FUNCTION__, ret);
        return ret;
    }
    format_string_as_upper_head(slot, strlen(slot));

    ret = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, URI_PATTERN_CHASSIS_MESHSWITCH, slot, blade_name_uri);
    if (ret <= RET_OK) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    json_object_object_add(*o_result_jso, RFPROP_ODATA_ID, json_object_new_string(uri));
    return HTTP_OK;
}

LOCAL gint32 get_chassis_mesh_status(guint8 *status)
{
    gint32 ret;
    OBJ_HANDLE obj_handle;
    GSList *obj_list_dp = NULL;
    guint8 tmp_status = LSW_NODE_INVALID;

    ret = dfl_get_object_list(CLASS_LSW_NODE_MANAGE_DP, &obj_list_dp);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s] dfl_get_object_list failed, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }
    obj_handle = (OBJ_HANDLE)obj_list_dp->data;

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_LSW_NODE_VALID, &tmp_status);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s] dal_get_property_value_byte failed, ret=%d", __FUNCTION__, ret);
        g_slist_free(obj_list_dp);
        return RET_ERR;
    }
    *status = (tmp_status == LSW_NODE_VALID) ? SW_STATUS_VALID : SW_STATUS_INVALID;

    g_slist_free(obj_list_dp);
    return RET_OK;
}

LOCAL gint32 get_chassis_lsw_status(guint8 *status)
{
    gint32 ret;
    GVariant *tmp_name = NULL;
    const gchar *plane_name = "ControlPlane";
    OBJ_HANDLE obj_handle = 0;

    tmp_name = g_variant_new_string(plane_name);
    ret = dfl_get_specific_object(CLASS_LSW_PUBLIC_ATTR_LOCAL_CP, PROPERTY_LSW_PUBLIC_PLANE_NAME,
        tmp_name, &obj_handle);
    g_variant_unref(tmp_name);
    tmp_name = NULL;
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s] Get %s obj_handle failed, ret=%d", __FUNCTION__, plane_name, ret);
        return RET_ERR;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_LSW_PUBLIC_CHIP_STATUS, status);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s] dal_get_property_value_byte failed, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    return RET_OK;
}

LOCAL gint32 get_chassis_switch_status(PROVIDER_PARAS_S* i_paras, json_object* huawei)
{
    gint32 ret;
    gchar sw_name_uri[MAX_NAME_SIZE] = {0};
    guint8 sw_status = 0;

    ret = rf_get_uri_info_by_position(i_paras->uri, PLANE_NAME_POSITION, sw_name_uri, sizeof(sw_name_uri));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get uri info failed, ret=%d", __FUNCTION__, ret);
        json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_STATUS, NULL);
        return RET_ERR;
    }

    if (g_ascii_strcasecmp(sw_name_uri, RFPROP_CHASSIS_LANSWITCH_MESH_NAME) == 0) {
        ret = get_chassis_mesh_status(&sw_status);
    } else {
        ret = get_chassis_lsw_status(&sw_status);
    }
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get %s status failed, ret=%d", __FUNCTION__, sw_name_uri, ret);
        json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_STATUS, NULL);
        return RET_ERR;
    }

    json_object_object_add(huawei, RFPROP_CHASSIS_LANSWITCH_OEM_STATUS,
        json_object_new_string((sw_status == SW_STATUS_VALID) ? "Valid" : "InValid"));

    return RET_OK;
}

LOCAL gint32 get_chassis_meshswitch_oem(PROVIDER_PARAS_S* i_paras,
                                        json_object** o_mess_jso, json_object** o_result_jso)
{
    gint32 ret;
    json_object* huawei = NULL;

    ret = rf_check_para_and_priv(i_paras, o_mess_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] rf_check_para_and_priv failed, ret=%d", __FUNCTION__, ret);
        return ret;
    }

    *o_result_jso = json_object_new_object();
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "[%s] json_object_new_object failed", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    huawei = json_object_new_object();
    
    if (huawei == NULL) {
        json_object_object_add(*o_result_jso, RFPROP_COMMON_HUAWEI, huawei);
        return HTTP_OK;
    }

    ret = get_chassis_switch_status(i_paras, huawei);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get_chassis_switch_status failed, ret=%d", __FUNCTION__, ret);
    }

    json_object_object_add(*o_result_jso, RFPROP_COMMON_HUAWEI, huawei);
    return HTTP_OK;
}

LOCAL gint32 get_chassis_meshswitch_odata_id(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
                                             json_object** o_result_jso)
{
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar slotid[MAX_NAME_SIZE] = {0};
    gchar blade_name_uri[MAX_NAME_SIZE] = {0};
    gint32 ret;

    ret = rf_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, check para and priv fail: %d", __FUNCTION__, ret);
        return ret;
    }

    ret = rf_get_uri_info_by_position(i_paras->uri, BLADE_NAME_POSITION, slotid, sizeof(slotid));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, get uri info fail: %d.", __FUNCTION__, ret);
        return ret;
    }
    format_string_as_upper_head(slotid, strlen(slotid));

    ret = rf_get_uri_info_by_position(i_paras->uri, PLANE_NAME_POSITION, blade_name_uri, sizeof(blade_name_uri));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, get uri info fail: %d.", __FUNCTION__, ret);
        return ret;
    }
    format_string_as_upper_head(blade_name_uri, strlen(blade_name_uri));

    ret = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, URI_FORMAT_CHASSIS_SWITCH, slotid, blade_name_uri);
    if (ret <= RET_OK) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const char*)uri);
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s, json_object_new_string fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return HTTP_OK;
}

gint32 chassis_switch_provider_entry(PROVIDER_PARAS_S* i_paras, PROPERTY_PROVIDER_S** prop_provider, guint32* count)
{
    gint32 ret;

    if ((i_paras == NULL) || (prop_provider == NULL) || (count == NULL)) {
        debug_log(DLOG_ERROR, "[%s] param error", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = rf_check_blade_name(i_paras->uri);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] rf_check_blade_name failed, ret=%d", __FUNCTION__, ret);
        return ret;
    }

    ret = rf_check_switch_name(i_paras->uri);
    if (ret != RET_OK) {
        return ret;
    }

    *prop_provider = g_chassis_meshswitch_provider;
    *count = sizeof(g_chassis_meshswitch_provider) / sizeof(PROPERTY_PROVIDER_S);

    return RET_OK;
}
/*****************************************************************************
* Function    : lanswitch_provider_entry
* Description : lanswitch 资源入口函数
* Return      : gint32
*      Creat by m00446762 for  lsw 2019-10-5
*****************************************************************************/
gint32 lanswitch_provider_entry(PROVIDER_PARAS_S* i_paras, PROPERTY_PROVIDER_S** prop_provider, guint32* count)
{
    gint32 ret;
    const gchar* blade_name = NULL;
#define MAX_LEN 5
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
    if (strncmp(blade_name, "blade", MAX_LEN) == 0) {
        debug_log(DLOG_ERROR, "%s, uri blade_name: %s is not match.\n", __FUNCTION__, blade_name);
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_chassis_lanswitch_provider;
    *count = sizeof(g_chassis_lanswitch_provider) / sizeof(PROPERTY_PROVIDER_S);

    return RET_OK;
}
#endif
