
#include "redfish_provider.h"

LOCAL gint32 get_chassis_networkports_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_networkports_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_networkports_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_networkports_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_chassis_networkports_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_networkports_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS_COUNT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_networkports_count, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_networkports_members, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_networkports_odata_context, NULL, NULL, ETAG_FLAG_ENABLE}
};


gboolean is_lom_hide_phy_port(OBJ_HANDLE businessport_obj)
{
    gint32 ret;
    OBJ_HANDLE ref_netcard_obj = 0;
    guchar card_type = 0;
    guchar port_type = 0;

    
    ret = dfl_get_referenced_object(businessport_obj, BUSY_ETH_ATTRIBUTE_REF_NETCARD, &ref_netcard_obj);
    return_val_if_expr(ret != DFL_OK, FALSE);

    ret = dal_get_property_value_byte(ref_netcard_obj, PROPERTY_NETCARD_CARDTYPE, &card_type);
    return_val_if_expr(ret != DFL_OK || card_type != NET_TYPE_LOM, FALSE);

    ret = dal_get_property_value_byte(businessport_obj, BUSY_ETH_ATTRIBUTE_PORT_TYPE, &port_type);
    return_val_if_expr(ret != DFL_OK || port_type != ETH_PORT_TYPE_PHYSICAL, FALSE);

    return_val_if_expr(g_str_has_prefix(dfl_get_object_name(businessport_obj), DYNAMIC_OBJ_BUSINESS_PORT) == FALSE,
        FALSE);

    return TRUE;
}


gint32 get_networkport_number(OBJ_HANDLE busi_obj, gchar *port_num, guint buf_len)
{
    guchar card_type = 0;
    guchar silk_num = 0;
    gint32 ret;

    return_val_do_info_if_expr(port_num == NULL || buf_len == 0, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: invalid input", __FUNCTION__));
    (void)memset_s(port_num, buf_len, 0, buf_len);

    (void)dal_get_property_value_byte(busi_obj, BUSY_ETH_ATTRIBUTE_CARD_TYPE, &card_type);
    if (card_type == BUSINESSPORT_CARD_TYPE_VIR) {
        ret = dal_get_property_value_string(busi_obj, BUSY_ETH_ATTRIBUTE_OSETH_NAME, port_num, buf_len);
        return_val_do_info_if_expr(ret != VOS_OK, VOS_ERR,
            debug_log(DLOG_ERROR, "%s: get port os eth name fail, ret:%d", __FUNCTION__, ret));
        if (!g_strcmp0(port_num, PROPERTY_VALUE_DEFAULT_STRING)) {
            return RET_ERR;
        }
    } else {
        ret = dal_get_property_value_byte(busi_obj, BUSY_ETH_ATTRIBUTE_SILK_NUM, &silk_num);
        return_val_do_info_if_expr(ret != VOS_OK, VOS_ERR,
            debug_log(DLOG_ERROR, "%s: get port silk num fail, ret:%d", __FUNCTION__, ret));
        if (silk_num == 0xff) { 
            return RET_ERR;
        }
        ret = snprintf_s(port_num, buf_len, buf_len - 1, "%u", silk_num);
        return_val_do_info_if_expr(ret <= 0, VOS_ERR,
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret:%d", __FUNCTION__, ret));
    }

    return VOS_OK;
}


gboolean is_virtual_eth_port(OBJ_HANDLE busi_obj)
{
    guchar port_type = 0;

    (void)dal_get_property_value_byte(busi_obj, BUSY_ETH_ATTRIBUTE_PORT_TYPE, &port_type);
    if (port_type == ETH_PORT_TYPE_VIRTUAL) {
        return TRUE;
    } else {
        return FALSE;
    }
}


gint32 get_networkports_members_count(OBJ_HANDLE netcard_obj, json_object **o_result_jso, gint32 *count)
{
    gint32 ret;
    gchar slot[MAX_RSC_ID_LEN] = {0};
    gchar string_value[FWINV_STR_MAX_LEN] = {0};
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    json_object *obj_jso = NULL;
    OBJ_HANDLE ref_netcard_obj = 0;
    gchar       netport_url[MAX_URL_LEN] = {0};
    gint32 port_count = 0;
    gchar       port_num[PROP_VAL_LENGTH] = {0};

    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d: redfish_get_board_slot failed", __FUNCTION__, __LINE__));

    
    ret = dfl_get_object_list(ETH_CLASS_NAME_BUSY_ETH, &obj_list);
    return_val_if_expr(ret != DFL_OK || obj_list == NULL, VOS_ERR);

    
    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        ret = dfl_get_referenced_object((OBJ_HANDLE)obj_node->data, BUSY_ETH_ATTRIBUTE_REF_NETCARD, &ref_netcard_obj);
        continue_if_expr(ret != DFL_OK);

        continue_if_expr(is_lom_hide_phy_port((OBJ_HANDLE)obj_node->data) == TRUE);
        // 不在networkport资源中呈现虚拟网口
        continue_if_expr(is_virtual_eth_port((OBJ_HANDLE)obj_node->data));

        if (ref_netcard_obj == netcard_obj) {
            ret = get_networkport_number((OBJ_HANDLE)obj_node->data, port_num, sizeof(port_num));
            continue_if_expr(ret != VOS_OK);

            ret = get_network_adapter_id(ref_netcard_obj, string_value, sizeof(string_value));
            continue_if_expr(ret != VOS_OK);

            if (o_result_jso) {
                ret = snprintf_s(netport_url, sizeof(netport_url), sizeof(netport_url) - 1,
                    CHASSIS_NETWORK_ADAPTERS_PORTS_MEMBERID_S, slot, string_value, port_num);
                continue_if_expr(ret <= 0);

                obj_jso = json_object_new_object();
                continue_if_expr(obj_jso == NULL);
                json_object_object_add(obj_jso, RFPROP_ODATA_ID, json_object_new_string(netport_url));
                ret = json_object_array_add(*o_result_jso, obj_jso);
                continue_do_info_if_expr(ret != 0, json_object_put(obj_jso));

                (void)memset_s(string_value, sizeof(string_value), 0, sizeof(string_value));
                (void)memset_s(netport_url, sizeof(netport_url), 0, sizeof(netport_url));
                (void)memset_s(port_num, sizeof(port_num), 0, sizeof(port_num));
            }
            port_count++;
        }
    }
    g_slist_free(obj_list);

    if (count) {
        *count = port_count;
    }

    return VOS_OK;
}


gint32 get_network_adapter_id(OBJ_HANDLE netcard_obj, gchar *network_id, guint32 buf_len)
{
    gint32 ret_val;
    OBJ_HANDLE ref_com_handle = 0;
    guint8 virtual_flag = 0;

    (void)dal_get_property_value_byte(netcard_obj, PROPERTY_NETCARD_VIRTUAL_FLAG, &virtual_flag);
    return_val_if_expr(virtual_flag == 1, VOS_ERR); // 1表示该网卡是虚拟XML
    ret_val = dfl_get_referenced_object(netcard_obj, PROPERTY_NETCARD_REF_COMPONENT, &ref_com_handle);
    return_val_if_expr(ret_val != DFL_OK, VOS_ERR);
    ret_val = get_location_devicename(ref_com_handle, network_id, buf_len);
    return_val_if_expr(ret_val != VOS_OK, VOS_ERR);

    return VOS_OK;
}


LOCAL gint32 get_chassis_networkports_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_NAME_LEN] = {0};
    gchar string_value[FWINV_STR_MAX_LEN] = {0};
    gint32 ret;

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: function  returned err of redfish_slot_id.", __FUNCTION__, __LINE__));

    ret = get_network_adapter_id(i_paras->obj_handle, string_value, sizeof(string_value));
    return_val_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR);

    ret = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, CHASSIS_NETWORK_PORTS_MEMBERID, slot_id, string_value);
    return_val_do_info_if_expr(VOS_OK >= ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : snprintf_s uri fail", __FUNCTION__));

    *o_result_jso = json_object_new_string(uri);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}

LOCAL gint32 get_chassis_networkports_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_NAME_LEN] = {0};
    gchar string_value[FWINV_STR_MAX_LEN] = {0};

    return_val_do_info_if_fail(
        (NULL != o_message_jso && NULL != o_result_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__));

    ret = get_network_adapter_id(i_paras->obj_handle, string_value, sizeof(string_value));
    return_val_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR);

    ret = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, SYSTEMS_NETWORK_ADAPTERS_NETWORKPORTS, slot_id, string_value);
    return_val_do_info_if_expr(VOS_OK >= ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : snprintf_s uri fail", __FUNCTION__));

    *o_result_jso = json_object_new_string(uri);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_chassis_networkports_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 port_count = 0;
    gint32 ret_val;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret_val = get_networkports_members_count(i_paras->obj_handle, NULL, &port_count);
    return_val_if_expr(ret_val != VOS_OK, HTTP_INTERNAL_SERVER_ERROR);

    *o_result_jso = json_object_new_int(port_count);
    return_val_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_chassis_networkports_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_array();
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_array fail.", __FUNCTION__, __LINE__));

    (void)get_networkports_members_count(i_paras->obj_handle, o_result_jso, NULL);

    return HTTP_OK;
}


gint32 chassis_networkports_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret;
    gboolean bool_ret;
    
    guchar x86_enabled = 0;
    gchar               url_tmp[MAX_URL_LEN] = {0};
    gchar *cha_netwokrk_flag = NULL;

    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_X86, &x86_enabled);
    return_val_if_fail(ENABLE == x86_enabled, HTTP_NOT_FOUND);

    
    OBJ_HANDLE chassis_handle;
    bool_ret = redfish_check_chassis_uri_valid(i_paras->uri, &chassis_handle);
    do_if_expr(!bool_ret, return HTTP_NOT_FOUND);
    

    
    ret = check_enclosure_component_type(chassis_handle, FALSE);
    
    return_val_if_fail(VOS_OK == ret, HTTP_NOT_FOUND);

    // URL 示例 : /redfish/v1/Chassis/1/NetworkAdapters/mainboardNIC1(TM210)/NetworkPorts
    // netwokrk_flag 指向 mainboardNIC1(TM210)
    ret = strcpy_s(url_tmp, sizeof(url_tmp), i_paras->uri);
    return_val_if_expr(ret != EOK, HTTP_NOT_FOUND);
    
    cha_netwokrk_flag = g_strrstr(url_tmp, SLASH_FLAG_STR);
    return_val_if_expr(cha_netwokrk_flag == NULL, HTTP_NOT_FOUND);
    *cha_netwokrk_flag = 0;
    cha_netwokrk_flag = g_strrstr(url_tmp, SLASH_FLAG_STR);
    return_val_if_expr(cha_netwokrk_flag == NULL, HTTP_NOT_FOUND);
    ret = check_network_adapter_url(cha_netwokrk_flag + 1, &(i_paras->obj_handle)); // NetCard 对象句柄
    return_val_if_expr(ret != VOS_OK, HTTP_NOT_FOUND);

    *prop_provider = g_chassis_networkports_provider;
    *count = sizeof(g_chassis_networkports_provider) / sizeof(PROPERTY_PROVIDER_S);

    

    return VOS_OK;
}
