
#ifdef ARM64_HI1711_ENABLED

#include "redfish_provider.h"

LOCAL gint32 get_chassis_lanswitch_ports_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 get_chassis_lanswitch_ports_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 get_chassis_lanswitch_ports_odata_id(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso);

LOCAL gint32 get_chassis_meshswitch_ports_odata_id(PROVIDER_PARAS_S* i_paras,
                                                   json_object** o_message_jso, json_object** o_result_jso);

LOCAL gint32 get_chassis_meshswitch_ports_members(PROVIDER_PARAS_S* i_paras,
                                                  json_object** o_message_jso, json_object** o_result_jso);

LOCAL gint32 get_chassis_meshswitch_ports_count(PROVIDER_PARAS_S* i_paras,
                                                json_object** o_message_jso, json_object** o_result_jso);

gint32 redfish_check_lsw_port_validity(OBJ_HANDLE object_handle, gchar *plane_name);

LOCAL PROPERTY_PROVIDER_S g_chassis_lanswitch_ports_provider[] = {
    { RFPROP_MEMBERS_COUNT, CLASS_LSW_PORT_ATTR_CENTER, PROPERTY_LSW_PLANE, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
      get_chassis_lanswitch_ports_count, NULL, NULL, ETAG_FLAG_DISABLE },
    { RFPROP_MEMBERS, CLASS_LSW_PORT_ATTR_CENTER, PROPERTY_LSW_PLANE, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
      get_chassis_lanswitch_ports_members, NULL, NULL, ETAG_FLAG_DISABLE },
    { RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
      get_chassis_lanswitch_ports_odata_id, NULL, NULL, ETAG_FLAG_ENABLE }
};

LOCAL PROPERTY_PROVIDER_S g_chassis_meshswitch_ports_provider[] = {
    { RFPROP_MEMBERS_COUNT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
      get_chassis_meshswitch_ports_count, NULL, NULL, ETAG_FLAG_DISABLE },
    { RFPROP_MEMBERS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
      get_chassis_meshswitch_ports_members, NULL, NULL, ETAG_FLAG_DISABLE },
    { RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
      get_chassis_meshswitch_ports_odata_id, NULL, NULL, ETAG_FLAG_ENABLE }
};

/*****************************************************************************
* Function    : get_chassis_lanswitch_ports_count
* Description : 获取lanswitch ports count属性值
* Return      : gint32
* Others      : 管理平面仅显示面板出端口，业务平面暂时不显示
* History     :
*      Creat by m00446762 for  lsw 2019-8-27
*****************************************************************************/
LOCAL gint32 get_chassis_lanswitch_ports_count(PROVIDER_PARAS_S* i_paras,
                                               json_object** o_message_jso, json_object** o_result_jso)
{
    gint32 ret;
    gint32 count = 0;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    OBJ_HANDLE object_handle = 0;
    const gchar *blade_name = NULL;
    gchar port_name[MAX_LSW_PORT_NAME_LEN] = {0};

    ret = rf_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, check para and priv fail: %d", __FUNCTION__, ret);
        return ret;
    }

    ret = rf_check_lsw_blade_name(i_paras->uri, &blade_name);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, uri blade_name is not match: %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dfl_get_object_list(g_chassis_lanswitch_ports_provider[i_paras->index].pme_class_name, &obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s, get obj_list fail: %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        object_handle = (OBJ_HANDLE)obj_node->data;

        ret = rf_check_lsw_port_by_blade(object_handle, blade_name);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "Check %s port validity fail: %d.", dfl_get_object_name(object_handle), ret);
            continue;
        }

        ret = rf_check_lsw_port_by_position(object_handle, port_name, sizeof(port_name));
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "Check %s port name fail: %d.", dfl_get_object_name(object_handle), ret);
            continue;
        }

        count++;
    }

    
    *o_result_jso = json_object_new_int(count);
    g_slist_free(obj_list);
    return HTTP_OK;
}

LOCAL void get_chassis_lanswitch_ports_members_check(GSList* object_list, json_object **o_result_jso,
                                                     const gchar *blade_name, guint32 name_len)
{
    gint32 ret;
    GSList* obj_node = NULL;
    json_object* obj_jso = NULL;
    OBJ_HANDLE object_handle;
    gchar lsw_uri[MAX_URI_LENGTH] = {0};
    gchar port_name[MAX_LSW_PORT_NAME_LEN] = {0};
    gchar blade_name_format[MAX_NAME_SIZE] = {0};
    
    *o_result_jso = json_object_new_array();
    for (obj_node = object_list; obj_node; obj_node = obj_node->next) {
        object_handle = (OBJ_HANDLE)obj_node->data;
        ret = rf_check_lsw_port_by_blade(object_handle, blade_name);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "Check %s port validity fail: %d.", dfl_get_object_name(object_handle), ret);
            continue;
        }

        ret = rf_format_const_string_upper_head(blade_name, name_len, blade_name_format, MAX_NAME_SIZE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s, format upper head fail: %d.", __FUNCTION__, ret);
            continue;
        }

        ret = rf_check_lsw_port_by_position(object_handle, port_name, sizeof(port_name));
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "Check %s port name fail: %d.", dfl_get_object_name(object_handle), ret);
            continue;
        }

        ret = snprintf_s(lsw_uri, sizeof(lsw_uri), sizeof(lsw_uri) - 1, URI_FORMAT_LANSWITCH_PORT, blade_name_format,
                         port_name);
        if (ret <= RET_OK) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail: %d", __FUNCTION__, ret);
            continue;
        }

        obj_jso = json_object_new_object();
        if (obj_jso == NULL) {
            debug_log(DLOG_ERROR, "%s: obj_jso is NULL", __FUNCTION__);
            continue;
        }
        json_object_object_add(obj_jso, RFPROP_ODATA_ID, json_object_new_string(lsw_uri));
        json_object_array_add(*o_result_jso, obj_jso);
    }

    return;
}


LOCAL gint32 get_chassis_lanswitch_ports_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    GSList* object_list = NULL;
    const gchar *blade_name = NULL;

    ret = rf_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, check para and priv fail: %d", __FUNCTION__, ret);
        return ret;
    }

    ret = rf_check_lsw_blade_name(i_paras->uri, &blade_name);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, uri blade_name is not match: %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dfl_get_object_list(g_chassis_lanswitch_ports_provider[i_paras->index].pme_class_name, &object_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s, get object_list fail: %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    get_chassis_lanswitch_ports_members_check(object_list, o_result_jso, blade_name, strlen(blade_name));

    g_slist_free(object_list);
    return HTTP_OK;
}


LOCAL gint32 get_chassis_lanswitch_ports_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar blade_name_info[MAX_NAME_SIZE] = {0};

    ret = rf_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, check para and priv fail: %d", __FUNCTION__, ret);
        return ret;
    }

    ret = rf_check_format_blade_name(i_paras, blade_name_info, sizeof(blade_name_info));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, check or format blade_name fail: %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, URI_FORMAT_LANSWITCH_PORTS, blade_name_info);
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

LOCAL gint32 get_chassis_switch_obj_list(gchar* uri, gchar *blade_name, guint32 blade_length, GSList** obj_list)
{
    gint32 ret;

    ret = rf_get_uri_info_by_position(uri, PLANE_NAME_POSITION, blade_name, blade_length);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, get uri info fail: %d.", __FUNCTION__, ret);
        return ret;
    }

    if (g_ascii_strcasecmp(blade_name, "MESH0") == 0) {
        ret = dfl_get_object_list(CLASS_LSW_PORT_ATTR_LOCAL_DP, obj_list);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s, get obj_list fail: %d.", __FUNCTION__, ret);
            return ret;
        }
    } else {
        ret = dfl_get_object_list(CLASS_LSW_PORT_ATTR_LOCAL_CP, obj_list);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s, get obj_list fail: %d.", __FUNCTION__, ret);
            return ret;
        }
    }
    return VOS_OK;
}

LOCAL gint32 get_chassis_meshswitch_ports_members_chk(PROVIDER_PARAS_S* i_paras, gchar *slot, guint32 slot_len,
                                                      json_object** o_message_jso, json_object** o_result_jso)
{
    gint32 ret;
    ret = rf_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, check para and priv fail: %d", __FUNCTION__, ret);
        return ret;
    }

    ret = rf_get_uri_info_by_position(i_paras->uri, BLADE_NAME_POSITION, slot, slot_len);
    if (ret != RET_OK) {
        return ret;
    }
    format_string_as_upper_head(slot, strlen(slot));
    return RET_OK;
}

LOCAL gint32 get_chassis_one_port_member(OBJ_HANDLE object_handle, json_object **o_result_jso,
                                         const gchar *blade_name, const gchar *switch_name)
{
    gint32 ret;
    gchar port_name[MAX_LSW_PORT_NAME_LEN] = {0};
    gchar lsw_uri[MAX_URI_LENGTH] = {0};
    json_object* tmp_obj_jso = NULL;

    ret = rf_check_port_id_and_type(object_handle, switch_name);
    if (ret != RET_OK) {
        return ret;
    }

    ret = dal_get_property_value_string(object_handle, PROPERTY_LSW_SUB_PORT_NAME, port_name, sizeof(port_name));
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s] dal_get_property_value_string failed, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    ret = snprintf_s(lsw_uri, sizeof(lsw_uri), sizeof(lsw_uri) - 1, URI_PATTERN_CHASSIS_MESHSWITCH_PORTS,
                     blade_name, switch_name, port_name);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "[%s] snprintf_s failed, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    tmp_obj_jso = json_object_new_object();
    if (tmp_obj_jso == NULL) {
        debug_log(DLOG_ERROR, "[%s] json_object_new_object failed", __FUNCTION__);
        return RET_ERR;
    }
    json_object_object_add(tmp_obj_jso, RFPROP_ODATA_ID, json_object_new_string(lsw_uri));
    json_object_array_add(*o_result_jso, tmp_obj_jso);

    return RET_OK;
}

LOCAL gint32 get_chassis_meshswitch_ports_members(PROVIDER_PARAS_S *i_paras,
                                                  json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar switch_name[MAX_NAME_SIZE] = {0};
    GSList* obj_list = NULL;
    GSList* obj_node = NULL;
    gchar blade_name[MAX_NAME_SIZE] = {0};

    ret = get_chassis_meshswitch_ports_members_chk(i_paras, blade_name, sizeof(blade_name),
                                                   o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get_chassis_meshswitch_ports_members_chk failed, ret=%d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = get_chassis_switch_obj_list(i_paras->uri, switch_name, sizeof(switch_name), &obj_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get_chassis_switch_obj_list failed, ret=%d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    format_string_as_upper_head(switch_name, strlen(switch_name));
    *o_result_jso = json_object_new_array();
    if (*o_result_jso == NULL) {
        g_slist_free(obj_list);
        debug_log(DLOG_ERROR, "[%s] json_object_new_array failed", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    for (obj_node = obj_list; obj_node != NULL; obj_node = obj_node->next) {
        (void)get_chassis_one_port_member((OBJ_HANDLE)obj_node->data, o_result_jso,
                                          (const gchar*)blade_name, (const gchar*)switch_name);
    }

    g_slist_free(obj_list);
    return HTTP_OK;
}

LOCAL gint32 get_chassis_meshswitch_ports_count(PROVIDER_PARAS_S* i_paras,
                                                json_object** o_message_jso, json_object** o_result_jso)
{
    gint32 ret;
    gint32 count = 0;
    gchar switch_name[MAX_NAME_SIZE] = {0};
    GSList* obj_list = NULL;
    GSList* obj_node = NULL;

    ret = rf_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] check para and priv failed, ret=%d", __FUNCTION__, ret);
        return ret;
    }

    ret = get_chassis_switch_obj_list(i_paras->uri, switch_name, MAX_NAME_SIZE, &obj_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get_chassis_switch_obj_list failed, ret=%d", __FUNCTION__, ret);
        return ret;
    }

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        ret = rf_check_port_id_and_type((OBJ_HANDLE)obj_node->data, (const gchar*)switch_name);
        if (ret != RET_OK) {
            continue;
        }
        count++;
    }

    
    *o_result_jso = json_object_new_int(count);
    g_slist_free(obj_list);
    return HTTP_OK;
}

LOCAL gint32 get_chassis_meshswitch_ports_odata_id(PROVIDER_PARAS_S* i_paras,
                                                   json_object** o_message_jso, json_object** o_result_jso)
{
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar blade_name_format[MAX_NAME_SIZE] = {0};
    gchar slot[MAX_NAME_SIZE] = {0};
    gint32 ret;

    ret = rf_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, check para and priv fail: %d", __FUNCTION__, ret);
        return ret;
    }

    ret = rf_get_uri_info_by_position(i_paras->uri, BLADE_NAME_POSITION, slot, sizeof(slot));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, get uri info fail: %d.", __FUNCTION__, ret);
        return ret;
    }
    format_string_as_upper_head(slot, strlen(slot));

    ret = rf_get_uri_info_by_position(i_paras->uri, PLANE_NAME_POSITION, blade_name_format, sizeof(blade_name_format));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, get uri info fail: %d.", __FUNCTION__, ret);
        return ret;
    }
    format_string_as_upper_head(blade_name_format, strlen(blade_name_format));

    ret = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, URI_PATTERN_CHASSIS_MESHSWITCH, slot, blade_name_format);
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


gint32 lanswitch_ports_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gint32 ret;
    const gchar *blade_name = NULL;

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

    *prop_provider = g_chassis_lanswitch_ports_provider;
    *count = sizeof(g_chassis_lanswitch_ports_provider) / sizeof(PROPERTY_PROVIDER_S);

    return RET_OK;
}

gint32 chassis_switch_ports_provider_entry(PROVIDER_PARAS_S* i_paras, PROPERTY_PROVIDER_S** prop_provider,
                                           guint32* count)
{
    gint32 ret;

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

    *prop_provider = g_chassis_meshswitch_ports_provider;
    *count = sizeof(g_chassis_meshswitch_ports_provider) / sizeof(PROPERTY_PROVIDER_S);

    return RET_OK;
}

#endif
