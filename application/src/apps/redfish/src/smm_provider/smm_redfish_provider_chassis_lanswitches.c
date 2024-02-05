
#ifdef ARM64_HI1711_ENABLED

#include "redfish_provider.h"
#include "redfish_provider_chassis.h"

LOCAL gint32 get_chassis_lanswitches_count(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso);

LOCAL gint32 get_chassis_lanswitches_members(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso);

LOCAL gint32 get_chassis_switches_member(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso);
LOCAL gint32 get_chassis_switches_odata_id(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso);
LOCAL gint32 get_chassis_switches_count(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_chassis_lanswitches_provider[] = {
    { RFPROP_MEMBERS_COUNT, CLASS_LSW_PUBLIC_ATTR_CENTER, PROPERTY_LSW_PUBLIC_PLANE_NAME, USERROLE_READONLY,
      SYS_LOCKDOWN_NULL, get_chassis_lanswitches_count, NULL, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_MEMBERS, CLASS_LSW_PUBLIC_ATTR_CENTER, PROPERTY_LSW_PUBLIC_PLANE_NAME, USERROLE_READONLY,
      SYS_LOCKDOWN_NULL, get_chassis_lanswitches_members, NULL, NULL, ETAG_FLAG_ENABLE }
};

LOCAL PROPERTY_PROVIDER_S g_chassis_switches_provider[] = {
    { RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
      get_chassis_switches_odata_id, NULL, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_MEMBERS_COUNT, CLASS_COMPONENT, PROPERTY_COMPONENT_DEVICE_TYPE, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
      get_chassis_switches_count, NULL, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_MEMBERS, CLASS_COMPONENT, PROPERTY_COMPONENT_DEVICE_TYPE, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
      get_chassis_switches_member, NULL, NULL, ETAG_FLAG_ENABLE }
};

LOCAL const gchar *g_plane_class_name[MAX_LSW_PLANE_NUM] = { CLASS_LSW_PUBLIC_ATTR_LOCAL_CP, CLASS_LSW_NODE_MANAGE_DP };
LOCAL const gchar *g_plane_switch_name[MAX_LSW_PLANE_NUM] = {
    RFPROP_CHASSIS_LANSWITCH_LSW_NAME, RFPROP_CHASSIS_LANSWITCH_MESH_NAME
};

gint32 rf_check_blade_name(gchar *uri)
{
    gint32 ret;
    gchar blade_name_uri[MAX_NAME_SIZE] = {0};
    gchar blade_name_xml[MAX_NAME_SIZE] = {0};

    if (uri == NULL) {
        debug_log(DLOG_ERROR, "[%s] uri is NULL", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = rf_get_uri_info_by_position(uri, BLADE_NAME_POSITION, blade_name_uri, MAX_NAME_SIZE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get blade name from uri failed, ret=%d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_rf_get_board_slot(blade_name_xml, MAX_NAME_SIZE);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s] get blade name from xml failed, ret=%d",  __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (g_ascii_strcasecmp(blade_name_uri, blade_name_xml) == 0) {
        return RET_OK;
    }

    return HTTP_NOT_FOUND;
}

gint32 rf_check_switch_name(gchar *uri)
{
    gint32 ret;
    gchar switch_name_uri[MAX_NAME_SIZE] = {0};
    guint32 obj_cnt = 0;

    if (uri == NULL) {
        debug_log(DLOG_ERROR, "[%s] uri is NULL", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = rf_get_uri_info_by_position(uri, PLANE_NAME_POSITION, switch_name_uri, MAX_NAME_SIZE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get switch name from uri failed, ret=%d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (g_ascii_strcasecmp(switch_name_uri, RFPROP_CHASSIS_LANSWITCH_MESH_NAME) == 0) {
        ret = dfl_get_object_count(CLASS_LSW_NODE_MANAGE_DP, &obj_cnt);
    } else if (g_ascii_strcasecmp(switch_name_uri, RFPROP_CHASSIS_LANSWITCH_LSW_NAME) == 0) {
        ret = dfl_get_object_count(CLASS_LSW_PUBLIC_ATTR_LOCAL_CP, &obj_cnt);
    } else {
        debug_log(DLOG_ERROR, "[%s] uri switch_name is not match, ret=%d", __FUNCTION__, ret);
        return HTTP_NOT_FOUND;
    }
    
    if ((ret != DFL_OK) || (obj_cnt == 0)) {
        debug_log(DLOG_ERROR, "[%s] there is no %s on this board, ret=%d", __FUNCTION__, switch_name_uri, ret);
        return HTTP_NOT_FOUND;
    }

    return RET_OK;
}

gint32 rf_check_port_id_and_type(OBJ_HANDLE object_handle, const gchar *switch_name)
{
    gint32 ret;
    guint32 port_id = INVALID_PORT_ID;
    guint32 port_type = 0;

    if ((object_handle == 0) || (switch_name == NULL)) {
        debug_log(DLOG_ERROR, "[%s] param error, object_handle=%lu", __FUNCTION__, object_handle);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (g_ascii_strcasecmp(switch_name, RFPROP_CHASSIS_LANSWITCH_LSW_NAME) == 0) {
        ret = dal_get_property_value_uint32(object_handle, PROPERTY_LSW_PORT_TYPE, &port_type);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "[%s] get %s port type failed, ret=%d",
                      __FUNCTION__, dfl_get_object_name(object_handle), ret);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        
        if ((port_type != LSW_PORT_TYPE_CAS) && (port_type != LSW_PORT_TYPE_MGMT)) {
            
            debug_log(DLOG_DEBUG, "[%s] port_type(%u) is not support, ret=%d", __FUNCTION__, port_type, ret);
            return HTTP_NOT_FOUND;
        }
    }

    ret = dal_get_property_value_uint32(object_handle, PROPERTY_LSW_LOGIC_PORT_NUM, &port_id);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s] get %s logic port number failed, ret=%d",
                  __FUNCTION__, dfl_get_object_name(object_handle), ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    if (port_id == INVALID_PORT_ID) {
        debug_log(DLOG_DEBUG, "[%s] invalid port_id(%u), ret=%d", __FUNCTION__, port_id, ret);
        return HTTP_NOT_FOUND;
    }

    return RET_OK;
}

/*****************************************************************************
* Function    : get_chassis_lanswitches_count
* Description : 获取lanswitch 管理的资源集合count属性值
* Return      : guint32
* History     :
*      Creat by m00446762 for  lsw 2019-8-27
*****************************************************************************/
LOCAL gint32 get_chassis_lanswitches_count(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso)
{
    gint32 ret;
    gint32 count = 0;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    const gchar *blade_name = NULL;

    ret = rf_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, check para and priv fail: %d", __FUNCTION__, ret);
        return ret;
    }

    ret = dfl_get_object_list(CLASS_NAME_IPMBETH_BLADE, &obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s, get obj_list fail: %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        blade_name = dfl_get_object_name((OBJ_HANDLE)obj_node->data);
        if (strncmp(blade_name, BLADE, strlen(BLADE)) == 0) {
            continue;
        }

        count++;
    }

    
    *o_result_jso = json_object_new_int(count);
    g_slist_free(obj_list);
    return HTTP_OK;
}

LOCAL gboolean match_slot_id_by_blade_name(const gchar *blade_name)
{
    guint8 slot_id_uri;
    guint8 slot_id;
    OBJ_HANDLE blade_handle = 0;
    OBJ_HANDLE asm_handle = 0;

    gint32 ret = dfl_get_object_handle(blade_name, &blade_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "Get %s handle fail, ret(%d)", blade_name, ret);
        return FALSE;
    }
    ret = dal_get_property_value_byte(blade_handle, PROPERTY_IPMBETH_BLADE_ID, &slot_id_uri);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "Get blade slot_id fail, ret(%d)", ret);
        return FALSE;
    }

    ret = dal_get_object_handle_nth(ASM_CLASS_NAME, 0, &asm_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "Get ASM handle failed, ret(%d)", ret);
        return FALSE;
    }
    ret = dal_get_property_value_byte(asm_handle, PROPERTY_ASM_SLOT_ID, &slot_id);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "Get property slot id failed, ret(%d)", ret);
        return FALSE;
    }

    if (slot_id_uri != slot_id) {
        return FALSE;
    }

    return TRUE;
}

LOCAL gint32 get_obj_jso_by_lsw_uri(const gchar *blade_name, json_object **obj_jso)
{
    gchar lsw_uri[MAX_URI_LENGTH] = {0};
    gchar blade_name_format[MAX_NAME_SIZE] = {0};

    if (match_slot_id_by_blade_name(blade_name) != TRUE) {
        return RET_ERR;
    }

    gint32 ret = rf_format_const_string_upper_head(blade_name, strlen(blade_name), blade_name_format, MAX_NAME_SIZE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Format upper head fail, ret(%d)", ret);
    }

    ret = snprintf_s(lsw_uri, sizeof(lsw_uri), sizeof(lsw_uri) - 1,
        URI_FORMAT_LANSWITCH_COLLECTION_ODATA_ID, blade_name_format);
    if (ret <= RET_OK) {
        debug_log(DLOG_ERROR, "snprintf_s fail, ret(%d)", ret);
        return ret;
    }

    *obj_jso = json_object_new_object();
    if (*obj_jso == NULL) {
        debug_log(DLOG_ERROR, "*obj_jso is NULL");
        return ret;
    }

    json_object_object_add(*obj_jso, RFPROP_ODATA_ID, json_object_new_string((const gchar *)lsw_uri));

    return RET_OK;
}


LOCAL gint32 get_chassis_lanswitches_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    json_object *obj_jso = NULL;
    const gchar *blade_name = NULL;

    ret = rf_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, check para and priv fail: %d", __FUNCTION__, ret);
        return ret;
    }

    ret = dfl_get_object_list(CLASS_NAME_IPMBETH_BLADE, &obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s, get obj_list fail: %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    *o_result_jso = json_object_new_array();
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s, *o_result_jso is NULL.", __FUNCTION__);
        g_slist_free(obj_list);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        blade_name = dfl_get_object_name((OBJ_HANDLE)obj_node->data);
        if (strncmp(blade_name, BLADE, strlen(BLADE)) == 0) {
            continue;
        }

        ret = get_obj_jso_by_lsw_uri(blade_name, &obj_jso);
        if (ret != RET_OK) {
            debug_log(DLOG_MASS, "Get obj jso by lsw uri failed, ret(%d)", ret);
            continue;
        }

        json_object_array_add(*o_result_jso, obj_jso);
    }

    g_slist_free(obj_list);
    return HTTP_OK;
}

LOCAL gint32 get_chassis_switches_odata_id(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
                                           json_object** o_result_jso)
{
    gint32 ret;
    gchar odata_id[MAX_URI_LENGTH] = {0};
    gchar slot[MAX_NAME_SIZE] = {0};

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

    ret = snprintf_s(odata_id, sizeof(odata_id), sizeof(odata_id) - 1, "%s%s/%s",
                     RF_CHASSIS_UI, slot, RFPROP_CHASSIS_LANSWITCH_SUMMARY);
    if (ret <= RET_OK) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }

    *o_result_jso = json_object_new_string((const gchar *)odata_id);
    if (*o_result_jso == NULL) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}

LOCAL guint32 get_chassis_switch_obj_count(void)
{
    gint32 ret, i;
    guint32 obj_cnt;
    guint32 sw_cnt = 0;

    for (i = 0; i < MAX_LSW_PLANE_NUM; i++) {
        obj_cnt = 0;
        ret = dfl_get_object_count(g_plane_class_name[i], &obj_cnt);
        if (ret != DFL_OK) {
            debug_log(DLOG_DEBUG, "[%s] dfl_get_object_count failed, i=%d ret=%d", __FUNCTION__, i, ret);
            continue;
        }
        sw_cnt += obj_cnt;
    }

    return sw_cnt;
}

LOCAL gint32 get_chassis_switches_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
                                        json_object **o_result_jso)
{
    gint32 ret;
    guint32 sw_cnt;

    ret = rf_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] check para and priv failed, ret=%d", __FUNCTION__, ret);
        return ret;
    }

    sw_cnt = get_chassis_switch_obj_count();

    *o_result_jso = json_object_new_int((gint32)sw_cnt);
    return HTTP_OK;
}

LOCAL gint32 get_chassis_switches_member(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
                                         json_object **o_result_jso)
{
    gint32 ret, i;
    gchar lsw_uri[MAX_URI_LENGTH] = {0};
    json_object *object_jso = NULL;
    gchar slot_id[MAX_NAME_SIZE] = {0};
    guint32 obj_cnt;

    ret = rf_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] check para and priv failed, ret=%d", __FUNCTION__, ret);
        return ret;
    }

    ret = rf_get_uri_info_by_position(i_paras->uri, BLADE_NAME_POSITION, slot_id, sizeof(slot_id));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get uri info failed, ret=%d", __FUNCTION__, ret);
        return ret;
    }
    format_string_as_upper_head(slot_id, strlen(slot_id));

    
    *o_result_jso = json_object_new_array();
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "[%s] json_object_new_array failed", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    for (i = 0; i < MAX_LSW_PLANE_NUM; i++) {
        obj_cnt = 0;
        ret = dfl_get_object_count(g_plane_class_name[i], &obj_cnt);
        if (ret != DFL_OK) {
            debug_log(DLOG_DEBUG, "[%s] dfl_get_object_count failed, i=%d ret=%d", __FUNCTION__, i, ret);
            continue;
        }

        if (obj_cnt > 0) {
            ret = snprintf_s(lsw_uri, sizeof(lsw_uri), sizeof(lsw_uri) - 1,
                             URI_FORMAT_CHASSIS_SWITCH, slot_id, g_plane_switch_name[i]);
            if (ret <= 0) {
                debug_log(DLOG_ERROR, "[%s] snprintf_s failed, i=%d ret=%d", __FUNCTION__, i, ret);
                continue;
            }

            object_jso = json_object_new_object();
            if (object_jso == NULL) {
                debug_log(DLOG_ERROR, "[%s] object_jso is NULL, i=%d ret=%d", __FUNCTION__, i, ret);
                continue;
            }
            json_object_object_add(object_jso, RFPROP_ODATA_ID, json_object_new_string((const gchar *)lsw_uri));
            json_object_array_add(*o_result_jso, object_jso);
            object_jso = NULL;
        }
    }

    return HTTP_OK;
}

gint32 chassis_switches_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gint32 ret;
    guint32 sw_cnt;

    if ((i_paras == NULL) || (prop_provider == NULL) || (count == NULL)) {
        debug_log(DLOG_ERROR, "[%s] param error", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    sw_cnt = get_chassis_switch_obj_count();
    if (sw_cnt == 0) {
        debug_log(DLOG_DEBUG, "[%s] no switch object found", __FUNCTION__);
        return HTTP_NOT_FOUND;
    }

    ret = rf_check_blade_name(i_paras->uri);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] rf_check_blade_name failed: %d", __FUNCTION__, ret);
        return ret;
    }

    *prop_provider = g_chassis_switches_provider;
    *count = sizeof(g_chassis_switches_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}

/*****************************************************************************
* Function    : lanswitch_collection_provider_entry
* Description : lanswitch 资源集合入口函数
* Return      : gint32
* History     :
*      Creat by m00446762 for  lsw 2019-10-5
*****************************************************************************/
gint32 lanswitch_collection_provider_entry(PROVIDER_PARAS_S* i_paras, PROPERTY_PROVIDER_S** prop_provider,
    guint32* count)
{
    gint32 ret;

    ret = rf_check_lsw_resource_legality(i_paras, prop_provider, count);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s, check resource legality fail: %d", __FUNCTION__, ret);
        return ret;
    }

    *prop_provider = g_chassis_lanswitches_provider;
    *count = sizeof(g_chassis_lanswitches_provider) / sizeof(PROPERTY_PROVIDER_S);

    return RET_OK;
}

#endif
