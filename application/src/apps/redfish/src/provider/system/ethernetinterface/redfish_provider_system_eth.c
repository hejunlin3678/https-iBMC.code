

#include "redfish_provider.h"
#include "redfish_provider_system.h"

LOCAL gint32 get_system_ethernetinterface_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_ethernetinterface_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_ethernetinterface_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_ethernetinterface_macaddr(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_system_ethernetinterface_macaddr(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_ethernetinterface_default_macaddr(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_ethernetinterface_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_ethernetinterface_vlans_link(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_ethernetinterface_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_vlans_uri_or_context(OBJ_HANDLE obj_handle, gchar *vlans_uri, gint32 uri_len,
    const gchar *uri_template);
LOCAL gint32 __get_system_ethernetinterface_action(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __act_ethernetinterface_configure(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL void __fill_ipv4_prop_list(json_object *ipv4_obj, const gchar *prop_name, gchar *prop_array,
    guint32 prop_array_len);

LOCAL gint32 get_system_ethernetinterface_auto_negotion(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_ethernetinterface_full_duplex(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_ethernetinterface_speed(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_system_eth_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_ethernetinterface_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_ethernetinterface_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_NAME, ETH_CLASS_NAME_BUSY_ETH, BUSY_ETH_ATTRIBUTE_OSETH_NAME, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_ethernetinterface_name, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ETH_MACADDR, ETH_CLASS_NAME_BUSY_ETH, BUSY_ETH_ATTRIBUTE_ACTUAL_MAC_ADDR, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_system_ethernetinterface_macaddr, set_system_ethernetinterface_macaddr, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ETH_PERMANENT_MACADDR, ETH_CLASS_NAME_BUSY_ETH, BUSY_ETH_GROUP_ATTRIBUTE_MAC, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_ethernetinterface_default_macaddr, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_ethernetinterface_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RF_PROPERTY_LINK_STATUS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_ethernetinterface_link_status, NULL, NULL, ETAG_FLAG_ENABLE},
    
    {RF_PROPERTY_ETH_SPEED_MBPS, ETH_CLASS_NAME_BUSY_ETH, BUSY_ETH_MCTP_SPEED, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_ethernetinterface_speed, NULL, NULL, ETAG_FLAG_ENABLE},
    {RF_PROPERTY_ETH_FULL_DUPLEX_ENABLED, ETH_CLASS_NAME_BUSY_ETH, BUSY_ETH_MCTP_DUPLEX_FLAG, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_ethernetinterface_full_duplex, NULL, NULL, ETAG_FLAG_ENABLE},
    {RF_PROPERTY_ETH_AUTO_NEGOTION_ENABLED, ETH_CLASS_NAME_BUSY_ETH, BUSY_ETH_MCTP_NEGOTIATE_FLAG, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_ethernetinterface_auto_negotion, NULL, NULL, ETAG_FLAG_ENABLE},
    
    {RF_PROPERTY_IPV4_ADDR, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_ethernetinterface_ipv4s, NULL, NULL, ETAG_FLAG_ENABLE},
    {RF_PROPERTY_IPV6_ADDR, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_ethernetinterface_ipv6s, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ETH_IPV6_GATEWAY, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_ethernetinterface_ipv6_gateway, NULL, NULL, ETAG_FLAG_ENABLE},
    {RF_PROPERTY_VLANS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_ethernetinterface_vlans_link, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_OEM, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_ethernetinterface_oem, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ACTIONS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, __get_system_ethernetinterface_action, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SYSTEM_ETHERNET_CONFIGURE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, NULL, NULL, __act_ethernetinterface_configure, ETAG_FLAG_ENABLE}
};


gint32 system_eth_etag_del_property(json_object *object)
{   
    SPECIAL_PROP_S prop_info[] = { 
        {3, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RF_PROPERTY_BROAD_CAST_PK_TRANSMITTED, NULL, NULL}},
        {3, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RF_PROPERTY_MUL_PK_TRANSMITTED, NULL, NULL}},
        {3, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RF_PROPERTY_BROAD_CAST_PK_RECV, NULL, NULL}},
        {3, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RF_PROPERTY_UNICAST_PK_RECV, NULL, NULL}},
        {3, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RF_PROPERTY_TOTAL_BYTES_RECV, NULL, NULL}},
        {3, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RF_PROPERTY_TOTAL_BYTES_TRANSMITTED, NULL, NULL}},
        {3, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RF_PROPERTY_MUL_PK_RECV, NULL, NULL}},
        {3, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RF_PROPERTY_UNICAST_PK_TRANSMITTED, NULL, NULL}},
        {3, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, BUSY_ETH_ATTRIBUTE_BANDWIDTH_USAGE, NULL, NULL}},

        {0, {NULL, NULL, NULL, NULL, NULL}}
    };

    return rsc_del_none_etag_affected_property(object, prop_info, G_N_ELEMENTS(prop_info));
}


LOCAL gint32 set_system_ethernetinterface_macaddr(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    GSList *input_list = NULL;
    const gchar *mac_addr_from_user = NULL;

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    OBJ_HANDLE obj_handle = i_paras->obj_handle;
    mac_addr_from_user = dal_json_object_get_str(i_paras->val_jso);
    if (mac_addr_from_user == NULL) {
        debug_log(DLOG_ERROR, "%s: dal_json_object_to_json_string failed", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_TYPE_ERR, NULL, o_message_jso, mac_addr_from_user, RFPROP_ETH_MACADDR);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    
    input_list = g_slist_append(input_list, g_variant_new_byte(SET_ACTUAL_MAC_ADDR_BY_MCTP));
    input_list = g_slist_append(input_list, g_variant_new_string(mac_addr_from_user));
    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        ETH_CLASS_NAME_BUSY_ETH, BUSY_ETH_METHOD_SET_ACTUAL_MAC, AUTH_ENABLE, i_paras->user_role_privilege, input_list,
        NULL);
    uip_free_gvariant_list(input_list);
    input_list = NULL;
    
    switch (ret) {
        case RET_OK:
        case RFERR_SUCCESS:
            ret = HTTP_OK;
            break;
        case RFERR_INSUFFICIENT_PRIVILEGE:
            
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_ETH_MACADDR, o_message_jso,
                RFPROP_ETH_MACADDR);
            ret = RFERR_INSUFFICIENT_PRIVILEGE;
            break;
        default:
            
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            ret = HTTP_INTERNAL_SERVER_ERROR;
            break;
    }
    return ret;
}


LOCAL gint32 get_system_ethernetinterface_speed(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;

    guint32 link_speed = 0xff;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    obj_handle = i_paras->obj_handle;

    (void)dal_get_property_value_uint32(obj_handle, g_system_eth_provider[i_paras->index].pme_prop_name, &link_speed);

    return_val_if_expr(link_speed == INVALID_DATA_DWORD, HTTP_INTERNAL_SERVER_ERROR);

    *o_result_jso = json_object_new_int(link_speed);

    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : json_object_new_string fail", __FUNCTION__));

    return HTTP_OK;
}

LOCAL gint32 get_system_ethernetinterface_full_duplex(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    guint8 byte_value = 0xff;

    return_val_do_info_if_expr(NULL == o_message_jso || NULL == o_result_jso || VOS_OK != provider_paras_check(i_paras),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    obj_handle = i_paras->obj_handle;

    (void)dal_get_property_value_byte(obj_handle, g_system_eth_provider[i_paras->index].pme_prop_name, &byte_value);

    return_val_if_expr(TRUE != byte_value && FALSE != byte_value, HTTP_INTERNAL_SERVER_ERROR);

    *o_result_jso = json_object_new_boolean(byte_value);

    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : json_object_new_string fail", __FUNCTION__));

    return HTTP_OK;
}

LOCAL gint32 get_system_ethernetinterface_auto_negotion(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;

    guint8 byte_value = 0xff;

    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: input value is NULL.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    obj_handle = i_paras->obj_handle;

    (void)dal_get_property_value_byte(obj_handle, g_system_eth_provider[i_paras->index].pme_prop_name, &byte_value);

    return_val_if_expr(TRUE != byte_value && FALSE != byte_value, HTTP_INTERNAL_SERVER_ERROR);

    *o_result_jso = json_object_new_boolean(byte_value);

    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : json_object_new_string fail", __FUNCTION__));

    return HTTP_OK;
}


LOCAL gint32 get_system_ethernetinterface_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;
    gint32 ret;
    gchar odata_context[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__));

    iRet = snprintf_s(odata_context, sizeof(odata_context), sizeof(odata_context) - 1, SYSTEMS_ETH_METADATA, slot_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    *o_result_jso = json_object_new_string(odata_context);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gboolean belong_to_virtualnetcard(OBJ_HANDLE businessport_obj)
{
    gint32 ret;
    OBJ_HANDLE referencd_handle = 0;
    guchar virtual_flag = FALSE;

    ret = dfl_get_referenced_object(businessport_obj, BUSY_ETH_ATTRIBUTE_REF_NETCARD, &referencd_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s : get RefNetCard failed", __FUNCTION__);
        return FALSE;
    }

    ret = dal_get_property_value_byte(referencd_handle, PROPERTY_NETCARD_VIRTUAL_FLAG, &virtual_flag);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : get virtual flag fail", __FUNCTION__);
        return FALSE;
    }

    if (virtual_flag != FALSE) {
        debug_log(DLOG_DEBUG, "%s : RefNetCard is VirtualNetCard", __FUNCTION__);
        return TRUE;
    }
    return FALSE;
}

LOCAL gint32 redfish_check_system_eth_uri_effective(const gchar *i_member_id, OBJ_HANDLE *o_obj_handle)
{
    gint32 ret = 0;
    gint32 match_flag = 1;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guint32 net_dev_function = 0;
    gchar member_id[PROP_VAL_LENGTH] = {0};
    guint8 port_type = 0;

    
    if ((i_member_id == NULL) || (o_obj_handle == NULL)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    
    (void)dfl_get_object_list(ETH_CLASS_NAME_BUSY_ETH, &obj_list);

    
    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        
        // 以太网或IB显示
        (void)dal_get_property_value_uint32((OBJ_HANDLE)obj_node->data, BUSY_ETH_ATTRIBUTE_NETDEV_FUNCTYPE,
            &net_dev_function);
        if ((net_dev_function & NETDEV_FUNCTYPE_ETHERNET) == 0) {
            continue;
        } else if (is_lom_hide_phy_port((OBJ_HANDLE)obj_node->data) == TRUE) {
            continue;
        }

        // 类型既不是物理网口也不是逻辑网口，过滤掉
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, BUSY_ETH_ATTRIBUTE_PORT_TYPE, &port_type);
        // 认为0xff是没bma的情况下，物理网口的值
        if (port_type != ETH_PORT_TYPE_VIRTUAL && port_type != ETH_PORT_TYPE_PHYSICAL && port_type != 0xff) {
            continue;
        }

        if (belong_to_virtualnetcard((OBJ_HANDLE)obj_node->data)) {
            continue;
        }

        ret = redfish_get_sys_eth_memberid((OBJ_HANDLE)obj_node->data, member_id, PROP_VAL_LENGTH);
        if (ret != RET_OK) {
            continue;
        }
        

        match_flag = g_ascii_strcasecmp(i_member_id, member_id);
        if (match_flag == RF_MATCH_OK) {
            break;
        }
    }

    
    
    if (match_flag != RF_MATCH_OK) {
        g_slist_free(obj_list);
        return HTTP_NOT_FOUND;
    }
    
    if (o_obj_handle != NULL && obj_node != NULL) {
        *o_obj_handle = (OBJ_HANDLE)obj_node->data;
    }
    g_slist_free(obj_list);
    return VOS_OK;
}


LOCAL gint32 get_system_ethernetinterface_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    gchar member_id[PROP_VAL_LENGTH] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    obj_handle = i_paras->obj_handle;

    ret = redfish_get_board_slot(slot, sizeof(slot));
    if (VOS_ERR == ret) {
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    ret = redfish_get_sys_eth_memberid(obj_handle, member_id, PROP_VAL_LENGTH);
    return_val_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR);
    
    iRet = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, URI_FORMAT_SYS_ETH, slot, member_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    *o_result_jso = json_object_new_string((const char *)uri);

    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return HTTP_OK;
}


LOCAL gint32 get_system_ethernetinterface_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gchar member_id[PROP_VAL_LENGTH] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    

    obj_handle = i_paras->obj_handle;
    
    ret = redfish_get_sys_eth_memberid(obj_handle, member_id, PROP_VAL_LENGTH);
    
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get member id fail", __FUNCTION__, __LINE__));
    
    
    
    *o_result_jso = json_object_new_string(member_id);
    return HTTP_OK;
}


LOCAL gint32 get_system_ethernetinterface_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    // guint8 port_num = 0;
    gchar eth_name[PROP_VAL_LENGTH] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    obj_handle = i_paras->obj_handle;

    (void)dal_get_property_value_string(obj_handle, g_system_eth_provider[i_paras->index].pme_prop_name, eth_name,
        PROP_VAL_LENGTH);

    
    if (0 == g_strcmp0(PROPERTY_VALUE_DEFAULT_STRING, eth_name)) {
        (void)memset_s(eth_name, PROP_VAL_LENGTH, 0, PROP_VAL_LENGTH);
        // PROP_VAL_LENGTH大小为64，大于格式化后字符串长度，无需判断返回值
        (void)snprintf_s(eth_name, PROP_VAL_LENGTH, PROP_VAL_LENGTH - 1, "%s", FIXED_ETH_NAME);
    }

    *o_result_jso = json_object_new_string(eth_name);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_string fail", __FUNCTION__));

    return HTTP_OK;
}

LOCAL gint32 get_system_ethernetinterface_default_macaddr(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gchar mac_addr[MACADDRESS_LEN + 1] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    obj_handle = i_paras->obj_handle;

    
    (void)dal_get_property_value_string(obj_handle, BUSY_ETH_GROUP_ATTRIBUTE_MAC, mac_addr, sizeof(mac_addr));

    // 全0的mac地址优化为 null
    return_val_if_expr(0 == g_strcmp0((const gchar *)mac_addr, RF_INIT_VAL_MACADDR), HTTP_INTERNAL_SERVER_ERROR);

    // N/A的mac地址也显示null
    return_val_if_expr(0 == g_strcmp0((const gchar *)mac_addr, PROPERTY_VALUE_DEFAULT_STRING),
        HTTP_INTERNAL_SERVER_ERROR);

    *o_result_jso = json_object_new_string(mac_addr);
    return HTTP_OK;
}


LOCAL gint32 get_system_ethernetinterface_macaddr(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gchar mac_addr[MACADDRESS_LEN + 1] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    obj_handle = i_paras->obj_handle;

    
    (void)dal_get_property_value_string(obj_handle, BUSY_ETH_ATTRIBUTE_ACTUAL_MAC_ADDR, mac_addr, sizeof(mac_addr));

    
    if (0 == g_strcmp0(mac_addr, RF_INIT_VAL_MACADDR) || 0 == g_strcmp0(mac_addr, PROPERTY_VALUE_DEFAULT_STRING)) {
        (void)memset_s(mac_addr, sizeof(mac_addr), 0, sizeof(mac_addr));
        (void)dal_get_property_value_string(obj_handle, BUSY_ETH_GROUP_ATTRIBUTE_MAC, mac_addr, sizeof(mac_addr));
    } // 当前MAC没有有效值，默认输出永久MAC地址

    

    // 全0的mac地址优化为 null
    return_val_if_expr(0 == g_strcmp0((const gchar *)mac_addr, RF_INIT_VAL_MACADDR), HTTP_INTERNAL_SERVER_ERROR);

    // N/A的mac地址也显示null
    return_val_if_expr(0 == g_strcmp0((const gchar *)mac_addr, PROPERTY_VALUE_DEFAULT_STRING),
        HTTP_INTERNAL_SERVER_ERROR);

    *o_result_jso = json_object_new_string(mac_addr);
    return HTTP_OK;
}


gint32 get_system_ethernetinterface_link_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gchar link_status_str[PROP_VAL_LENGTH] = {0};
    guint8 link_status = 0xff;
    guchar link_status_src = 0;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    obj_handle = i_paras->obj_handle;

    
    (void)dal_get_property_value_string(obj_handle, BUSY_ETH_ATTRIBUTE_LINK_STATUS_STR, link_status_str,
        PROP_VAL_LENGTH);

    
    (void)dal_get_property_value_byte(obj_handle, BUSY_ETH_ATTRIBUTE_LINK_SRC, &link_status_src);

    
    if (0 == g_strcmp0(link_status_str, PROPERTY_VALUE_DEFAULT_STRING)) {
        // 获取硬链接
        (void)dal_get_property_value_byte(obj_handle, BUSY_ETH_ATTRIBUTE_LINK_STATUS, &link_status);
        // 只有硬链接是  0(未插网线)的时候才显示，否则显示null，因为1的话，无法schema的值对应
        return_val_if_fail(0 == link_status, HTTP_INTERNAL_SERVER_ERROR);

        (void)memset_s(link_status_str, sizeof(link_status_str), 0, sizeof(link_status_str));

        (void)strncpy_s(link_status_str, sizeof(link_status_str), RF_VALUE_NOLINK, strlen(RF_VALUE_NOLINK));
    }
    

    *o_result_jso = json_object_new_string((const char *)link_status_str);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : json_object_new_string fail", __FUNCTION__));

    return HTTP_OK;
}

LOCAL void get_ipv4_addr_mask_gateway(GVariant *property, json_object **o_result_jso)
{
    GVariantIter tmp_iter;
    GVariant *node = NULL;
    const gchar *node_str = NULL;
    gchar **split_string = NULL;
    gchar address[IPV4_IP_STR_SIZE + 2] = {0};
    gchar subnetmask[IPV4_MASK_STR_SIZE + 2] = {0};
    gchar gateway[MAX_URI_LENGTH] = {0};
    gchar address_origin[PROP_VAL_LENGTH] = {0};
    char *flag = NULL;
    json_object *temp = NULL;
    json_object *object = NULL;
    gint32 ret = 0;
    guint32 str_len = 0;

    return_do_info_if_expr(NULL == o_result_jso, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    (void)g_variant_iter_init(&tmp_iter, property);

    while (NULL != (node = g_variant_iter_next_value(&tmp_iter))) {
        node_str = g_variant_get_string(node, NULL);
        if ((NULL == node_str) || (0 == strlen(node_str))) {
            debug_log(DLOG_MASS, "%s: g_variant_get_string failed or str is empty", __FUNCTION__);
            do_info_if_true(NULL != node, g_variant_unref(node); node = NULL);
            continue;
        }

        
        split_string = g_strsplit(node_str, ";", 0);
        continue_do_info_if_fail(NULL != split_string,
            debug_log(DLOG_ERROR, "%s: g_strsplit %s fail", __FUNCTION__, node_str);
            g_variant_unref(node); node = NULL);

        str_len = g_strv_length(split_string);
        continue_do_info_if_fail(4 == str_len, debug_log(DLOG_ERROR, "%s: g_strsplit %s fail", __FUNCTION__, node_str);
            g_variant_unref(node); node = NULL; g_strfreev(split_string));

        
        ret = snprintf_s(subnetmask, IPV4_MASK_STR_SIZE + 2, IPV4_MASK_STR_SIZE + 1, "%s", split_string[0]);
        continue_do_info_if_fail(VOS_OK < ret, debug_log(DLOG_ERROR, "%s: snprintf_s subnet mask fail", __FUNCTION__);
            g_strfreev(split_string); g_variant_unref(node); node = NULL);

        
        ret = snprintf_s(address_origin, sizeof(address_origin), sizeof(address_origin) - 1, "%s", split_string[1]);
        continue_do_info_if_fail(VOS_OK < ret, debug_log(DLOG_ERROR, "%s: snprintf_s subnet mask fail", __FUNCTION__);
            g_strfreev(split_string); g_variant_unref(node); node = NULL);
        

        ret = snprintf_s(address, IPV4_IP_STR_SIZE + 2, IPV4_IP_STR_SIZE + 1, "%s", split_string[3]);
        continue_do_info_if_fail(VOS_OK < ret, debug_log(DLOG_ERROR, "%s: snprintf_s address fail", __FUNCTION__);
            g_strfreev(split_string); g_variant_unref(node); node = NULL);

        
        ret = snprintf_s(gateway, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, "%s", split_string[2]);
        continue_do_info_if_fail(VOS_OK < ret, debug_log(DLOG_ERROR, "%s: snprintf_s gateway fail", __FUNCTION__);
            g_strfreev(split_string); g_variant_unref(node); node = NULL);
        flag = strstr(gateway, " ");
        do_info_if_true(NULL != flag, (*flag = '\0'));

        g_strfreev(split_string);
        g_variant_unref(node);
        node = NULL;

        object = json_object_new_object();
        continue_do_info_if_fail(NULL != object, debug_log(DLOG_ERROR, "%s: new object fail", __FUNCTION__));

        
        if (g_strcmp0(PROPERTY_VALUE_DEFAULT_STRING, address)) {
            temp = json_object_new_string((const char *)address);
            do_info_if_true(NULL == temp, debug_log(DLOG_ERROR, "%s: new address fail", __FUNCTION__));
        }
        json_object_object_add(object, RF_PROPERTY_ADDR, temp);
        temp = NULL;

        if (g_strcmp0(PROPERTY_VALUE_DEFAULT_STRING, subnetmask)) {
            temp = json_object_new_string((const char *)subnetmask);
            do_info_if_true(NULL == temp, debug_log(DLOG_ERROR, "%s: new subnet mask fail", __FUNCTION__));
        }
        json_object_object_add(object, RF_PROPERTY_SUB_MASK, temp);
        temp = NULL;

        if (g_strcmp0(PROPERTY_VALUE_DEFAULT_STRING, gateway)) {
            temp = json_object_new_string((const char *)gateway);
            do_info_if_true(NULL == temp, debug_log(DLOG_ERROR, "%s: new gateway fail", __FUNCTION__));
        }
        json_object_object_add(object, RF_PROPERTY_GATEWAY, temp);
        temp = NULL;

        
        if (g_strcmp0(PROPERTY_VALUE_DEFAULT_STRING, address_origin)) {
            temp = json_object_new_string((const char *)address_origin);
            do_info_if_true(NULL == temp, debug_log(DLOG_ERROR, "%s: new gateway fail", __FUNCTION__));
        }
        json_object_object_add(object, RF_PROPERTY_ADDR_ORIGIN, temp);
        temp = NULL;
        

        ret = json_object_array_add(*o_result_jso, object);
        do_info_if_true(VOS_OK != ret, debug_log(DLOG_ERROR, "%s: array add fail", __FUNCTION__);
            (void)json_object_put(object));
        object = NULL;
    }

    return;
}

gint32 get_system_ethernetinterface_ipv4s(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    GVariant *ipv4_info = NULL;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    obj_handle = i_paras->obj_handle;

    *o_result_jso = json_object_new_array();
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_array fail", __FUNCTION__));

    ret = dfl_get_property_value(obj_handle, BUSY_ETH_ATTRIBUTE_IPV4INFO, &ipv4_info);
    // 返回200，是要显示空数组
    return_val_do_info_if_expr((DFL_OK != ret) || (NULL == ipv4_info), HTTP_OK,
        debug_log(DLOG_ERROR, "%s: dfl_get_property_value ipv4info fail", __FUNCTION__));

    get_ipv4_addr_mask_gateway(ipv4_info, o_result_jso);

    g_variant_unref(ipv4_info);
    return HTTP_OK;
}


LOCAL void get_ipv6_oem_gateway(const gchar *gateway_str, json_object *jso_obj)
{
    gint32 ret;
    gchar **split_string = NULL;
    json_object *oem_jso = NULL;
    json_object *huawei = NULL;
    json_object *gateway_arr_jso = NULL;
    json_object *gateway_jso = NULL;
    guint i;

    return_do_info_if_expr(jso_obj == NULL, debug_log(DLOG_ERROR, "%s, %d: NULL point", __FUNCTION__, __LINE__));
    oem_jso = json_object_new_object();
    return_do_info_if_expr(oem_jso == NULL,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_json fail", __FUNCTION__, __LINE__));

    huawei = json_object_new_object();
    return_do_info_if_expr(huawei == NULL, json_object_put(oem_jso);
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_json fail", __FUNCTION__, __LINE__));
    ret = json_object_object_add(oem_jso, RFPROP_COMMON_HUAWEI, huawei);
    return_do_info_if_expr(ret != VOS_OK, json_object_put(huawei); json_object_put(oem_jso);
        debug_log(DLOG_ERROR, "%s: add object to json fail", __FUNCTION__));

    gateway_arr_jso = json_object_new_array();
    return_do_info_if_expr(gateway_arr_jso == NULL, json_object_put(oem_jso);
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_array fail", __FUNCTION__, __LINE__));
    ret = json_object_object_add(huawei, RF_PROPERTY_GATEWAY, gateway_arr_jso);
    return_do_info_if_expr(ret != VOS_OK, json_object_put(oem_jso); json_object_put(gateway_arr_jso);
        debug_log(DLOG_ERROR, "%s: add gateway to object fail", __FUNCTION__));

    goto_label_do_info_if_expr(gateway_str == NULL, exit,
        debug_log(DLOG_ERROR, "%s: gateway str is null point", __FUNCTION__));

    
    split_string = g_strsplit(gateway_str, " ", 0);
    goto_label_do_info_if_expr(split_string == NULL, exit,
        debug_log(DLOG_ERROR, "%s: g_strsplit %s fail", __FUNCTION__, gateway_str));

    for (i = 0; i < g_strv_length(split_string); i++) {
        // 过滤无效网关取值
        continue_if_expr(split_string[i] == NULL || strlen(split_string[i]) == 0 ||
            g_strcmp0(PROPERTY_VALUE_DEFAULT_STRING, split_string[i]) == 0);
        gateway_jso = json_object_new_string((const gchar *)split_string[i]);
        continue_do_info_if_expr(gateway_jso == NULL,
            debug_log(DLOG_ERROR, "%s :json_object_new_string %s fail", __FUNCTION__, split_string[i]));
        ret = json_object_array_add(gateway_arr_jso, gateway_jso);
        do_info_if_true(ret != VOS_OK, json_object_put(gateway_jso);
            debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail", __FUNCTION__, __LINE__));
    }

    g_strfreev(split_string);
exit:
    ret = json_object_object_add(jso_obj, RFPROP_COMMON_OEM, oem_jso);
    return_do_info_if_expr(ret != VOS_OK, json_object_put(oem_jso);
        debug_log(DLOG_ERROR, "%s: add oem to json fail", __FUNCTION__));
    return;
}


LOCAL void get_ipv6_addr_prefix(GVariant *property, json_object **address_prefix)
{
    GVariantIter tmp_iter;
    GVariant *node = NULL;
    const gchar *node_str = NULL;
    gchar **split_string = NULL;
    gchar address[IPV6_IP_STR_SIZE + 2] = {0};
    gchar address_origin[PROP_VAL_LENGTH] = {0};
    gchar address_state[PROP_VAL_LENGTH] = {0};
    gchar prefix_str[PROP_VAL_LENGTH] = {0};
    gchar gateway_str[PROP_VAL_MAX_LENGTH] = {0};
    gint32 prefix_len = 0;
    json_object *temp = NULL;
    json_object *object = NULL;
    gint32 ret;
    guint32 str_len = 0;

    return_do_info_if_expr((NULL == property) || (NULL == address_prefix),
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    (void)g_variant_iter_init(&tmp_iter, property);

    while (NULL != (node = g_variant_iter_next_value(&tmp_iter))) {
        node_str = g_variant_get_string(node, NULL);
        if ((NULL == node_str) || (0 == strlen(node_str))) {
            debug_log(DLOG_MASS, "%s: g_variant_get_string failed or empty string.", __FUNCTION__);
            g_variant_unref(node);
            node = NULL;
            continue;
        }
        
        split_string = g_strsplit(node_str, ";", 0);
        continue_do_info_if_fail(NULL != split_string, g_variant_unref(node); node = NULL;
            debug_log(DLOG_ERROR, "%s: g_strsplit %s fail", __FUNCTION__, node_str));

        // 长度和预期不符，continue
        str_len = g_strv_length(split_string);
        continue_do_info_if_fail(5 == str_len, g_variant_unref(node); node = NULL; g_strfreev(split_string);
            debug_log(DLOG_ERROR, "%s: g_strsplit %s fail", __FUNCTION__, node_str));

        ret = snprintf_s(gateway_str, PROP_VAL_MAX_LENGTH, PROP_VAL_MAX_LENGTH - 1, "%s", split_string[4]);
        continue_do_info_if_fail(VOS_OK < ret, g_variant_unref(node); node = NULL; g_strfreev(split_string);
            debug_log(DLOG_ERROR, "%s: snprintf_s gateway fail", __FUNCTION__));

        ret = snprintf_s(address, IPV6_IP_STR_SIZE + 2, IPV6_IP_STR_SIZE + 1, "%s", split_string[3]);
        continue_do_info_if_fail(VOS_OK < ret, g_variant_unref(node); node = NULL; g_strfreev(split_string);
            debug_log(DLOG_ERROR, "%s: snprintf_s address fail", __FUNCTION__));

        ret = snprintf_s(prefix_str, PROP_VAL_LENGTH, PROP_VAL_LENGTH - 1, "%s", split_string[1]);
        continue_do_info_if_fail(VOS_OK < ret, g_variant_unref(node); node = NULL; g_strfreev(split_string);
            debug_log(DLOG_ERROR, "%s: snprintf_s prefix fail", __FUNCTION__));

        ret = snprintf_s(address_origin, PROP_VAL_LENGTH, PROP_VAL_LENGTH - 1, "%s", split_string[2]);
        continue_do_info_if_fail(VOS_OK < ret, g_variant_unref(node); node = NULL; g_strfreev(split_string);
            debug_log(DLOG_ERROR, "%s: snprintf_s address fail", __FUNCTION__));

        ret = snprintf_s(address_state, PROP_VAL_LENGTH, PROP_VAL_LENGTH - 1, "%s", split_string[0]);
        continue_do_info_if_fail(VOS_OK < ret, g_variant_unref(node); node = NULL; g_strfreev(split_string);
            debug_log(DLOG_ERROR, "%s: snprintf_s address fail", __FUNCTION__));
        g_strfreev(split_string);
        g_variant_unref(node);
        node = NULL;

        object = json_object_new_object();
        continue_do_info_if_fail(NULL != object, debug_log(DLOG_ERROR, "%s: new object fail", __FUNCTION__));

        if (g_strcmp0(PROPERTY_VALUE_DEFAULT_STRING, address)) {
            temp = json_object_new_string((const char *)address);
            do_info_if_true(NULL == temp, debug_log(DLOG_ERROR, "%s: new string fail", __FUNCTION__));
        }
        json_object_object_add(object, RF_PROPERTY_ADDR, temp);
        temp = NULL;

        // 前缀长度可以为0
        if (g_strcmp0(prefix_str, PROPERTY_VALUE_DEFAULT_STRING)) {
            ret = vos_str2int(prefix_str, 10, &prefix_len, NUM_TPYE_INT32);
            continue_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s: prefix_str str2int fail", __FUNCTION__);
                (void)json_object_put(object));

            temp = json_object_new_int(prefix_len);
            do_info_if_true(NULL == temp, debug_log(DLOG_ERROR, "%s: new int fail", __FUNCTION__));
        }
        json_object_object_add(object, RF_PROPERTY_PREF_LEN, temp);
        temp = NULL;

        if (g_strcmp0(PROPERTY_VALUE_DEFAULT_STRING, address_origin)) {
            temp = json_object_new_string((const char *)address_origin);
            do_info_if_true(NULL == temp, debug_log(DLOG_ERROR, "%s: new string fail", __FUNCTION__));
        }
        json_object_object_add(object, RF_PROPERTY_ADDR_ORIGIN, temp);
        temp = NULL;

        if (g_strcmp0(PROPERTY_VALUE_DEFAULT_STRING, address_state)) {
            temp = json_object_new_string((const char *)address_state);
            do_info_if_true(NULL == temp, debug_log(DLOG_ERROR, "%s: new string fail", __FUNCTION__));
        }
        json_object_object_add(object, RF_PROPERTY_ADDR_STATE, temp);
        temp = NULL;

        get_ipv6_oem_gateway((const gchar *)gateway_str, object);

        ret = json_object_array_add(*address_prefix, object);
        do_info_if_true(VOS_OK != ret, debug_log(DLOG_ERROR, "%s: array add fail", __FUNCTION__);
            (void)json_object_put(object));

        object = NULL;
    }

    return;
}


gint32 get_system_ethernetinterface_ipv6s(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    GVariant *ipv6_info = NULL;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    obj_handle = i_paras->obj_handle;

    *o_result_jso = json_object_new_array();
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_array fail", __FUNCTION__));

    ret = dfl_get_property_value(obj_handle, BUSY_ETH_ATTRIBUTE_IPV6INFO, &ipv6_info);
    // 返回200，是要显示空数组
    return_val_do_info_if_expr((DFL_OK != ret) || (NULL == ipv6_info), HTTP_OK,
        debug_log(DLOG_ERROR, "%s: dfl_get_property_value ipv6info fail", __FUNCTION__));

    get_ipv6_addr_prefix(ipv6_info, o_result_jso);

    g_variant_unref(ipv6_info);
    return HTTP_OK;
}


LOCAL void get_ipv6_default_gateway(GVariant *property, json_object **gateway_jso)
{
    gint32 ret = 0;
    GVariantIter tmp_iter;
    GVariant *node = NULL;
    const gchar *node_str = NULL;
    gchar **split_string = NULL;
    gchar gateway[MAX_URI_LENGTH] = {0};
    char *flag = NULL;
    guint32 str_len = 0;

    return_do_info_if_expr((NULL == property) || (NULL == gateway_jso),
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    (void)g_variant_iter_init(&tmp_iter, property);

    while (NULL != (node = g_variant_iter_next_value(&tmp_iter))) {
        node_str = g_variant_get_string(node, NULL);
        if ((NULL == node_str) || (0 == strlen(node_str))) {
            debug_log(DLOG_MASS, "%s: g_variant_get_string failed or str is empty", __FUNCTION__);
            g_variant_unref(node);
            continue;
        }
        
        split_string = g_strsplit(node_str, ";", 0);
        continue_do_info_if_fail(NULL != split_string, g_variant_unref(node);
            debug_log(DLOG_ERROR, "%s: g_strsplit %s fail", __FUNCTION__, node_str));

        // 长度和预期不符，continue
        str_len = g_strv_length(split_string);
        continue_do_info_if_fail(5 == str_len, g_variant_unref(node); g_strfreev(split_string);
            debug_log(DLOG_ERROR, "%s: g_strsplit %s fail", __FUNCTION__, node_str));

        ret = snprintf_s(gateway, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, "%s", split_string[4]);
        continue_do_info_if_fail(VOS_OK < ret, g_variant_unref(node); g_strfreev(split_string);
            debug_log(DLOG_ERROR, "%s: snprintf_s address fail", __FUNCTION__));

        flag = strstr(gateway, " ");
        do_info_if_true(NULL != flag, (*flag = '\0'));

        g_strfreev(split_string);
        g_variant_unref(node);

        
        break_if_expr(g_strcmp0(PROPERTY_VALUE_DEFAULT_STRING, gateway) && strlen(gateway));

        (void)memset_s(gateway, sizeof(gateway), 0, sizeof(gateway));
    }
    *gateway_jso = NULL;

    if (strlen(gateway)) {
        *gateway_jso = json_object_new_string((const char *)gateway);
        do_info_if_true(NULL == *gateway_jso, debug_log(DLOG_ERROR, "%s: new gateway json fail", __FUNCTION__));
    }
    return;
}


gint32 get_system_ethernetinterface_ipv6_gateway(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    GVariant *ipv6_info = NULL;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    obj_handle = i_paras->obj_handle;

    ret = dfl_get_property_value(obj_handle, BUSY_ETH_ATTRIBUTE_IPV6INFO, &ipv6_info);
    return_val_do_info_if_expr((DFL_OK != ret) || (NULL == ipv6_info), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: dfl_get_property_value ipv6info fail", __FUNCTION__));

    get_ipv6_default_gateway(ipv6_info, o_result_jso);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR, g_variant_unref(ipv6_info));

    g_variant_unref(ipv6_info);

    return HTTP_OK;
}


LOCAL gint32 get_system_ethernetinterface_vlans_link(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gchar vlans_uri[MAX_URI_LENGTH] = {0};
    json_object *uri = NULL;
    gint32 ret;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    obj_handle = i_paras->obj_handle;

    ret = get_vlans_uri_or_context(obj_handle, vlans_uri, sizeof(vlans_uri), URI_FORMAT_SYS_ETH_VLANS);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get vlans uri fail", __FUNCTION__));

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_object fail.", __FUNCTION__));

    uri = json_object_new_string((const char *)vlans_uri);
    do_info_if_true(NULL == uri, debug_log(DLOG_ERROR, "%s: json_object_new_string fail.", __FUNCTION__));

    json_object_object_add(*o_result_jso, RFPROP_ODATA_ID, uri);

    return HTTP_OK;
}


LOCAL const gchar *redfish_get_interface_type(OBJ_HANDLE business_obj)
{
    const gchar *interface_type = NULL;
    guint8 port_type = 0;

    (void)dal_get_property_value_byte(business_obj, BUSY_ETH_ATTRIBUTE_PORT_TYPE, &port_type);

    // 0xff认为是没bma时物理网口的值
    if ((ETH_PORT_TYPE_PHYSICAL == port_type) || (0xff == port_type)) {
        interface_type = RF_PROPERTY_ETH_PORT_TYPE_PHYSICAL;
    } else if (ETH_PORT_TYPE_VIRTUAL == port_type) {
        interface_type = RF_PROPERTY_ETH_PORT_TYPE_VIRTUAL;
    } else {
        debug_log(DLOG_ERROR, "%s : invalid port type is %d", __FUNCTION__, port_type);
    }

    return interface_type;
}

LOCAL json_object *redfish_get_port_bandwidth_usage(OBJ_HANDLE obj_handle)
{
    guint32 ret;
    guint16 port_bandwidth_usage = 0;
    json_object *josn_value = NULL;

    ret = dal_get_property_value_uint16(obj_handle, BUSY_ETH_ATTRIBUTE_BANDWIDTH_USAGE, &port_bandwidth_usage);
    return_val_do_info_if_fail(VOS_OK == ret, NULL,
        debug_log(DLOG_ERROR, "%s, %d: get property value fail.\n", __FUNCTION__, __LINE__));

    if (port_bandwidth_usage <= 10000) {
        josn_value = ex_json_object_new_double(((gdouble)port_bandwidth_usage) / 100, "%.2f");
        do_info_if_true(NULL == josn_value, debug_log(DLOG_ERROR, "%s: ex_json_object_new_double fail.", __FUNCTION__));
    }

    return josn_value;
}


LOCAL void __get_eth_interface_bdf(OBJ_HANDLE obj_handle, json_object *huawei_jso)
{
    gchar eth_bdf[PROP_VAL_LENGTH] = {0};
    json_object *eth_bdf_jso = NULL;

    return_if_expr(0 == obj_handle || NULL == huawei_jso);

    (void)dal_get_property_value_string(obj_handle, BUSY_ETH_ATTRIBUTE_BDF, eth_bdf, sizeof(eth_bdf));

    if (g_strcmp0((const gchar *)eth_bdf, PROPERTY_VALUE_DEFAULT_STRING)) {
        eth_bdf_jso = json_object_new_string(eth_bdf);
    }

    json_object_object_add(huawei_jso, RFPROP_ETH_BDF, eth_bdf_jso);

    return;
}


void get_system_eth_link(OBJ_HANDLE obj_handle, json_object *huawei_jso, const gchar *str_template)
{
    int iRet = -1;
    gint32 ret;
    OBJ_HANDLE net_card_obj = 0;
    OBJ_HANDLE net_card_com = 0;
    json_object *related_port = NULL;
    gchar location_str[PROP_VAL_LENGTH] = {0};
    gchar device_name_str[PROP_VAL_LENGTH] = {0};
    gchar eth_mem_id[PROP_VAL_LENGTH] = {0};
    gchar slot_id[PROP_VAL_LENGTH] = {0};
    gchar url[MAX_URL_LEN] = {0};
    gchar port_num[PROP_VAL_LENGTH * 2] = {0};
    guchar card_type = 0;

    return_if_expr(0 == obj_handle || NULL == huawei_jso || NULL == str_template);

    ret = dfl_get_referenced_object(obj_handle, BUSY_ETH_ATTRIBUTE_REF_NETCARD, &net_card_obj);
    if (ret != DFL_OK) {
        // 没有引用的就是动态生成的逻辑网口
        goto exit;
    }

    ret = dfl_get_referenced_object(net_card_obj, PROPERTY_NETCARD_REF_COMPONENT, &net_card_com);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: %s netcard not ref component", __FUNCTION__, dfl_get_object_name(net_card_obj));
        goto exit;
    }

    related_port = json_object_new_object();
    if (related_port == NULL) {
        debug_log(DLOG_ERROR, "%s: alloc memory failed", __FUNCTION__);
        goto exit;
    }

    // 判断CardType是否为OS上报的虚拟网口
    ret = dal_get_property_value_byte(obj_handle, BUSY_ETH_ATTRIBUTE_CARD_TYPE, &card_type);
    goto_label_do_info_if_expr(ret != VOS_OK, exit, json_object_put(related_port); related_port = NULL;
        debug_log(DLOG_ERROR, "%s: get property card type fail, ret:%d", __FUNCTION__, ret));

    ret = get_networkport_number(obj_handle, port_num, sizeof(port_num));
    goto_label_do_info_if_expr(ret != VOS_OK, exit, json_object_put(related_port); related_port = NULL);
    (void)redfish_get_board_slot(slot_id, sizeof(slot_id));

    // OS上报的虚拟网口单独处理
    if (card_type == BUSINESSPORT_CARD_TYPE_VIR) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
        iRet = snprintf_s(url, sizeof(url), sizeof(url) - 1, str_template, slot_id, port_num);
#pragma GCC diagnostic pop
        goto_label_do_info_if_expr(iRet <= 0, exit, json_object_put(related_port); related_port = NULL;
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret:%d", __FUNCTION__, ret));
        json_object_object_add(related_port, ODATA_ID, json_object_new_string((const char *)url));
        goto exit;
    }

    // 非虚拟网口
    (void)dal_get_property_value_string(net_card_com, PROPERTY_COMPONENT_LOCATION, location_str, sizeof(location_str));
    (void)dal_get_property_value_string(net_card_com, PROPERTY_COMPONENT_DEVICE_NAME, device_name_str,
        sizeof(device_name_str));
    (void)dal_clear_string_blank(location_str, sizeof(location_str));
    (void)dal_clear_string_blank(device_name_str, sizeof(device_name_str));
    iRet = snprintf_s(eth_mem_id, sizeof(eth_mem_id), sizeof(eth_mem_id) - 1, "%s%sPort%s", location_str,
        device_name_str, port_num);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    iRet = snprintf_s(url, sizeof(url), sizeof(url) - 1, str_template, slot_id, eth_mem_id);
#pragma GCC diagnostic pop
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    json_object_object_add(related_port, ODATA_ID, json_object_new_string((const char *)url));
exit:

    json_object_object_add(huawei_jso, RFPROP_ETH_RELATED_PORT, related_port);

    return;
}


LOCAL void __get_eth_is_onboot(OBJ_HANDLE obj_handle, json_object *huawei_jso)
{
    guint8 is_onboot = 0;
    json_object *is_onboot_jso = NULL;

    return_if_expr(0 == obj_handle || NULL == huawei_jso);

    (void)dal_get_property_value_byte(obj_handle, BUSY_ETH_ATTRIBUTE_IS_ONBOOT, &is_onboot);

    if (0 == is_onboot || 1 == is_onboot) {
        is_onboot_jso = json_object_new_boolean(is_onboot);
    }

    json_object_object_add(huawei_jso, RF_PROPERTY_IS_ONBOOT, is_onboot_jso);
}

LOCAL void __add_property_jso_array(OBJ_HANDLE obj_handle, const gchar *property, const gchar *rf_property,
    json_object *o_result_jso)
{
    gsize i;
    gsize len = 0;
    gint32 ret;
    GVariant *gv_port_info = NULL;
    const guint8 *tmp_array = NULL;
    json_object *tmp_jso = NULL;
    json_object *array_jso = NULL;

    ret = dfl_get_property_value(obj_handle, property, &gv_port_info);
    return_do_info_if_expr(VOS_OK != ret, debug_log(DLOG_ERROR, "%s: get %s.%s failed, ret=%d.", __FUNCTION__,
        dfl_get_object_name(obj_handle), property, ret));
    tmp_array = (const guint8 *)g_variant_get_fixed_array(gv_port_info, &len, sizeof(guint8));
    return_do_info_if_expr(NULL == tmp_array || len <= 0, g_variant_unref(gv_port_info);
        debug_log(DLOG_ERROR, "%s: g_variant_get_fixed_array failed.", __FUNCTION__));
    array_jso = json_object_new_array();

    
    // 如果数组只有一个元素且是0，则认为是默认值，返回空数组
    return_do_info_if_expr(1 == len && 0 == tmp_array[0], json_object_object_add(o_result_jso, rf_property, array_jso);
        g_variant_unref(gv_port_info));
    

    for (i = 0; i < len; i++) {
        tmp_jso = json_object_new_int(tmp_array[i]);
        (void)json_object_array_add(array_jso, tmp_jso);
    }

    json_object_object_add(o_result_jso, rf_property, array_jso);

    g_variant_unref(gv_port_info);
}


LOCAL void __get_eth_interface_info(OBJ_HANDLE obj_handle, json_object *huawei_jso)
{
    return_if_expr(0 == obj_handle || NULL == huawei_jso);

    
    // AlignmentErrors
    rf_add_property_jso_string(obj_handle, BUSY_ETH_ATTRIBUTE_ALIGNMENTERRORS, RF_PROPERTY_ETH_ALIGNMENT_ERRORS,
        huawei_jso);
    

    // update cos2up
    __add_property_jso_array(obj_handle, BUSY_ETH_ATTRIBUTE_COS2UP, RF_PROPERTY_UP2COS, huawei_jso);

    // update pgid
    __add_property_jso_array(obj_handle, BUSY_ETH_ATTRIBUTE_UP_PGID, RF_PROPERTY_PGID, huawei_jso);

    // update pgpct
    __add_property_jso_array(obj_handle, BUSY_ETH_ATTRIBUTE_PGPCT, RF_PROPERTY_PGPCT, huawei_jso);

    // update strict
    __add_property_jso_array(obj_handle, BUSY_ETH_ATTRIBUTE_STRICT, RF_PROPERTY_PGSTRICT, huawei_jso);

    // updata pfcup
    rf_add_property_jso_uint16(obj_handle, BUSY_ETH_ATTRIBUTE_pfcmap, RF_PROPERTY_PFCUP, huawei_jso);

    // updata TotalBytesReceived
    rf_add_property_jso_string(obj_handle, BUSY_ETH_MCTP_TOTAL_BYTES_RX, RF_PROPERTY_TOTAL_BYTES_RECV, huawei_jso);

    // updata TotalBytesTransmitted
    rf_add_property_jso_string(obj_handle, BUSY_ETH_MCTP_TOTAL_BYTES_TX, RF_PROPERTY_TOTAL_BYTES_TRANSMITTED,
        huawei_jso);

    // updata UnicastPacketsReceived
    rf_add_property_jso_string(obj_handle, BUSY_ETH_MCTP_TOTAL_UNI_PKT_RX, RF_PROPERTY_UNICAST_PK_RECV, huawei_jso);

    // updata MulticastPacketsReceived
    rf_add_property_jso_string(obj_handle, BUSY_ETH_MCTP_TOTAL_MUL_PKT_RX, RF_PROPERTY_MUL_PK_RECV, huawei_jso);

    // updata BroadcastPacketsReceived
    rf_add_property_jso_string(obj_handle, BUSY_ETH_MCTP_TOTAL_BC_PKT_RX, RF_PROPERTY_BROAD_CAST_PK_RECV, huawei_jso);

    // updata UnicastPacketsTransmitted
    rf_add_property_jso_string(obj_handle, BUSY_ETH_MCTP_TOTAL_UNI_PKT_TX, RF_PROPERTY_UNICAST_PK_TRANSMITTED,
        huawei_jso);

    // updata MulticastPacketsTransmitted
    rf_add_property_jso_string(obj_handle, BUSY_ETH_MCTP_TOTAL_MUL_PKT_TX, RF_PROPERTY_MUL_PK_TRANSMITTED, huawei_jso);

    // updata BroadcastPacketsTransmitted
    rf_add_property_jso_string(obj_handle, BUSY_ETH_MCTP_TOTAL_BC_PKT_TX, RF_PROPERTY_BROAD_CAST_PK_TRANSMITTED,
        huawei_jso);

    // updata FcsReceiveErrors
    rf_add_property_jso_string(obj_handle, BUSY_ETH_MCTP_FCS_RX_ERRS, RF_PROPERTY_FCS_RECV_ERR, huawei_jso);

    // updata RuntPacketsReceived
    rf_add_property_jso_string(obj_handle, BUSY_ETH_MCTP_RUNT_PKT_RX, RF_PROPERTY_RUNT_PK_RECV, huawei_jso);

    // updata JabberPacketsReceived
    rf_add_property_jso_string(obj_handle, BUSY_ETH_MCTP_JABBER_PKT_RX, RF_PROPERTY_JABBER_PK_RECV, huawei_jso);

    // updata SingleCollisionTransmitFrames
    rf_add_property_jso_string(obj_handle, BUSY_ETH_MCTP_SINGLE_COLLISION_TX_FRAMES,
        RF_PROPERTY_SINGLE_COLLISION_TRANSMIT_FRAMES, huawei_jso);

    // updata MultipleCollisionTransmitFrames
    rf_add_property_jso_string(obj_handle, BUSY_ETH_MCTP_MULTIPLE_COLLISION_TX_FRAMES,
        RF_PROPERTY_MUL_COLLISION_TRANSMIT_FRAMES, huawei_jso);

    // updata LateCollisionFrames
    rf_add_property_jso_string(obj_handle, BUSY_ETH_MCTP_LATE_COLLISION_FRAMES, RF_PROPERTY_LATE_COLLISION_FRAMES,
        huawei_jso);

    // updata ExcessiveCollisionFrames
    rf_add_property_jso_string(obj_handle, BUSY_ETH_MCTP_EXCESSIVE_COLLISION_FRAMES,
        RF_PROPERTY_EXCESSIVE_COLLISION_FRAMES, huawei_jso);
}


LOCAL void __get_eth_connswitch_info(OBJ_HANDLE obj_handle, json_object *huawei_jso)
{
    return_if_expr(0 == obj_handle || NULL == huawei_jso);

    // 交换机名称
    rf_add_property_jso_string(obj_handle, BUSY_ETH_LLDP_SWITCH_NAME, RF_PROPERTY_PORT_CONNEC_SWITCH_NAME, huawei_jso);

    // 交换机ID
    rf_add_property_jso_string(obj_handle, BUSY_ETH_LLDP_CHASSISID_SUBDESP, RF_PROPERTY_PORT_CONNEC_SWITCH_ID,
        huawei_jso);

    // 交换机连接端口ID
    rf_add_property_jso_string(obj_handle, BUSY_ETH_LLDP_PORTID_SUBDESP, RF_PROPERTY_PORT_CONNEC_SWITCH_PORT_ID,
        huawei_jso);

    // 交换机OS侧VLAN ID
    rf_add_property_jso_uint16(obj_handle, BUSY_ETH_LLDP_OS_VLAN_ID, RF_PROPERTY_PORT_CONNEC_SWITCH_OS_VLAN_ID,
        huawei_jso);
}


LOCAL void get_chassis_netport_link(OBJ_HANDLE busi_obj, json_object *huawei_jso)
{
    gint32  ret = 0;
    gchar   slot_id[PROP_VAL_LENGTH] = {0};
    gchar   port_link[MAX_URI_LENGTH] = {0};
    json_object *related_port = NULL;

    return_if_expr(huawei_jso == NULL);
    related_port = json_object_new_object();
    goto_label_do_info_if_expr(related_port == NULL, exit,
        debug_log(DLOG_ERROR, "%s: create json object fail", __FUNCTION__));

    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    goto_label_do_info_if_expr(ret != VOS_OK, exit, debug_log(DLOG_ERROR, "%s: get board slot fail", __FUNCTION__));
    ret = get_network_port_url(busi_obj, (const gchar *)slot_id, CHASSIS_NETWORK_ADAPTERS_PORTS_MEMBERID_S, port_link,
        sizeof(port_link));
    goto_label_do_info_if_expr(ret != VOS_OK, exit,
        debug_log(DLOG_DEBUG, "%s: get network port url fail", __FUNCTION__));
    json_object_object_add(related_port, ODATA_ID, json_object_new_string((const gchar *)port_link));

exit:
    do_if_expr(ret != VOS_OK && related_port != NULL, json_object_put(related_port); related_port = NULL);
    json_object_object_add(huawei_jso, RFPROP_ETH_RELATED_PORT, related_port);
    return;
}


LOCAL gint32 get_system_ethernetinterface_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    json_object *huawei = NULL;
    json_object *port_type_jso = NULL;
    const gchar *port_type = NULL;
    json_object *port_bandwidth_usage_jso = NULL;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_object fail.", __FUNCTION__));

    huawei = json_object_new_object(); // new失败返回空对象
    return_val_do_info_if_expr(NULL == huawei, HTTP_OK,
        debug_log(DLOG_ERROR, "%s: json_object_new_object fail.", __FUNCTION__));

    json_object_object_add(*o_result_jso, RFPROP_COMMON_HUAWEI, huawei);

    // add网口类型
    port_type = redfish_get_interface_type(i_paras->obj_handle);
    if (NULL != port_type) {
        port_type_jso = json_object_new_string(port_type);
        do_info_if_true(NULL == port_type_jso, debug_log(DLOG_ERROR, "%s: json_object_new_string fail.", __FUNCTION__));
    }
    json_object_object_add(huawei, RFPROP_INTERFACE_TYPE, port_type_jso);

    // add网络带宽占用率
    port_bandwidth_usage_jso = redfish_get_port_bandwidth_usage(i_paras->obj_handle);
    json_object_object_add(huawei, BUSY_ETH_ATTRIBUTE_BANDWIDTH_USAGE, port_bandwidth_usage_jso);

    
    // 获取bdf
    __get_eth_interface_bdf(i_paras->obj_handle, huawei);

    
    (void)rf_add_property_jso_string(i_paras->obj_handle, PROPERTY_DA_METRIC_DISPLAY_NAME,
        RFPROP_ETH_NETWORK_ADAPTER_NAME, huawei);
    

    // 获取指向chassis的网口的链接
    get_chassis_netport_link(i_paras->obj_handle, huawei);
    

    
    __get_eth_is_onboot(i_paras->obj_handle, huawei);
    
    
    __get_eth_interface_info(i_paras->obj_handle, huawei);
    

    
    __get_eth_connswitch_info(i_paras->obj_handle, huawei);
    

    return HTTP_OK;
}


LOCAL gint32 __get_system_ethernetinterface_action(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;
    OBJ_HANDLE obj_handle = 0;
    gchar eth_action_url[MAX_URL_LEN] = {0};
    gchar slot[PROP_VAL_LENGTH] = {0};
    gchar member_id[PROP_VAL_LENGTH] = {0};
    json_object *oem_jso = NULL;
    json_object *huawei_jso = NULL;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    obj_handle = i_paras->obj_handle;
    (void)redfish_get_board_slot(slot, sizeof(slot));
    (void)redfish_get_sys_eth_memberid(obj_handle, member_id, sizeof(member_id));
    iRet = snprintf_s(eth_action_url, sizeof(eth_action_url), sizeof(eth_action_url) - 1, URI_FORMAT_SYS_ETH, slot,
        member_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__));

    oem_jso = json_object_new_object();
    return_val_do_info_if_expr(NULL == oem_jso, HTTP_OK,
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__));

    json_object_object_add(*o_result_jso, RFPROP_COMMON_OEM, oem_jso);
    huawei_jso = json_object_new_object();
    return_val_do_info_if_expr(NULL == huawei_jso, HTTP_OK,
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__));
    json_object_object_add(oem_jso, RFPROP_COMMON_HUAWEI, huawei_jso);
    rf_add_action_prop(huawei_jso, eth_action_url, RFPROP_SYSTEM_ETHERNET_CONFIGURE);

    return HTTP_OK;
}


LOCAL void __fill_ipv4_prop_list_and_check_strncat_return(json_object *array_iter, const gchar *prop_name,
    gchar *prop_array, guint32 prop_array_len, gint32 count, gint32 ipv4_arr_length)
{
    errno_t safe_fun_ret = EOK;
    __fill_ipv4_prop_list(array_iter, prop_name, prop_array, prop_array_len);
    if (count < ipv4_arr_length - 1) {
        safe_fun_ret = strncat_s(prop_array, prop_array_len, prop_name, strlen(prop_name));
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    }
}


LOCAL gint32 __fill_act_eth_config_input_list(json_object *body_checked, GSList **input_list,
    json_object **o_message_jso)
{
    errno_t safe_fun_ret = EOK;
    gint32 ret;
    json_object *array_iter = NULL;
    json_object *tmp_jso = NULL;
    guint8 onboot_byte;
    const gchar *link_status = NULL;
    gint32 ipv4_arr_length;
    gint32 count = 0;
    gchar address[PROP_VAL_MAX_LENGTH] = {0};
    gchar netmask[PROP_VAL_MAX_LENGTH] = {0};
    gchar origin[PROP_VAL_MAX_LENGTH] = {0};
    gchar gateway[PROP_VAL_MAX_LENGTH] = {0};

    if (NULL == body_checked || NULL == input_list || NULL == o_message_jso) {
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = json_object_object_get_ex(body_checked, RF_PROPERTY_IS_ONBOOT, &tmp_jso);
    if (TRUE == ret) {
        onboot_byte = (TRUE == json_object_get_boolean(tmp_jso)) ? TRUE : FALSE;
    } else {
        onboot_byte = G_MAXUINT8;
    }
    tmp_jso = NULL;

    ret = json_object_object_get_ex(body_checked, RF_PROPERTY_LINK_STATUS, &tmp_jso);
    if (TRUE == ret) {
        link_status = dal_json_object_get_str(tmp_jso);
    } else {
        link_status = PROPERTY_VALUE_DEFAULT_STRING;
    }
    tmp_jso = NULL;

    ret = json_object_object_get_ex(body_checked, RF_PROPERTY_IPV4_ADDR, &tmp_jso);
    goto_label_do_info_if_fail(TRUE == ret, exit,
        debug_log(DLOG_DEBUG, "%s: json_object_object_get_ex %s failed", __FUNCTION__, RF_PROPERTY_IPV4_ADDR));

    ipv4_arr_length = json_object_array_length(tmp_jso);
    if (ipv4_arr_length > 0) {
        for (count = 0; count < ipv4_arr_length; count++) {
            array_iter = json_object_array_get_idx(tmp_jso, count);
            continue_if_expr(NULL == array_iter);
            __fill_ipv4_prop_list_and_check_strncat_return(array_iter, RF_PROPERTY_ADDR, address, sizeof(address),
                count, ipv4_arr_length);
            __fill_ipv4_prop_list_and_check_strncat_return(array_iter, RF_PROPERTY_SUB_MASK, netmask, sizeof(netmask),
                count, ipv4_arr_length);
            __fill_ipv4_prop_list_and_check_strncat_return(array_iter, RF_PROPERTY_ADDR_ORIGIN, origin, sizeof(origin),
                count, ipv4_arr_length);
            __fill_ipv4_prop_list_and_check_strncat_return(array_iter, RF_PROPERTY_GATEWAY, gateway, sizeof(gateway),
                count, ipv4_arr_length);
        }
    } else {
        safe_fun_ret =
            strncpy_s(address, sizeof(address), PROPERTY_VALUE_DEFAULT_STRING, strlen(PROPERTY_VALUE_DEFAULT_STRING));
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
        safe_fun_ret =
            strncpy_s(netmask, sizeof(netmask), PROPERTY_VALUE_DEFAULT_STRING, strlen(PROPERTY_VALUE_DEFAULT_STRING));
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
        safe_fun_ret =
            strncpy_s(gateway, sizeof(gateway), PROPERTY_VALUE_DEFAULT_STRING, strlen(PROPERTY_VALUE_DEFAULT_STRING));
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
        safe_fun_ret =
            strncpy_s(origin, sizeof(origin), PROPERTY_VALUE_DEFAULT_STRING, strlen(PROPERTY_VALUE_DEFAULT_STRING));
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    }

exit:
    debug_log(DLOG_DEBUG, "%s: address = %s", __FUNCTION__, address);
    debug_log(DLOG_DEBUG, "%s: netmask = %s", __FUNCTION__, netmask);
    debug_log(DLOG_DEBUG, "%s: origin = %s", __FUNCTION__, origin);
    debug_log(DLOG_DEBUG, "%s: gateway = %s", __FUNCTION__, gateway);
    debug_log(DLOG_DEBUG, "%s: onboot_byte = %d", __FUNCTION__, onboot_byte);
    debug_log(DLOG_DEBUG, "%s: link_status = %s", __FUNCTION__, link_status);

    *input_list = g_slist_append(*input_list, g_variant_new_string(address));
    *input_list = g_slist_append(*input_list, g_variant_new_string(netmask));
    *input_list = g_slist_append(*input_list, g_variant_new_string(origin));
    *input_list = g_slist_append(*input_list, g_variant_new_string(gateway));
    *input_list = g_slist_append(*input_list, g_variant_new_byte(onboot_byte));
    *input_list = g_slist_append(*input_list, g_variant_new_string(link_status));

    return VOS_OK;
}


LOCAL gint32 __act_ethernetinterface_configure(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;
    gint32 ret;
    gint32 result_val = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    guint32 free_state_flag = 0;
    TASK_MONITOR_INFO_S *act_eth_config_info = NULL;
    gchar oration_name[PROP_VAL_LENGTH] = {0};
    gchar memberid[PROP_VAL_LENGTH] = {0};

    // URL示例:/redfish/v1/Systems/1/EthernetInterfaces/mainboardNIC1Port4/Actions/Oem/Huawei/EthernetInterface.Configure
    if (NULL == i_paras || NULL == o_message_jso) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    
    if (0 == g_strcmp0(dal_json_object_get_str(i_paras->val_jso), JSON_NULL_OBJECT_STR)) {
        (void)create_message_info(MSGID_MALFORMED_JSON, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }
    

    ret = __fill_act_eth_config_input_list(i_paras->val_jso, &input_list, o_message_jso);
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s: __fill_act_eth_config_input_list failed, ret = %d", __FUNCTION__, ret);
        result_val = ret;
        goto error_exit;
    }

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, ETH_CLASS_NAME_BUSY_ETH, BUSY_ETH_METHOD_SET_IPV4_INFO, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, &output_list);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    switch (ret) {
        case RFERR_INSUFFICIENT_PRIVILEGE:
            result_val = HTTP_FORBIDDEN;
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
            goto error_exit;

        case VOS_OK:
            free_state_flag = g_variant_get_uint32((GVariant *)g_slist_nth_data(output_list, 0));
            g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
            debug_log(DLOG_DEBUG, "%s: free_state_flag = %d", __FUNCTION__, free_state_flag);
            break;

        
        case VOS_ERR_NOTSUPPORT:
            result_val = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_CONFIG_IP_FAILED, NULL, o_message_jso, RF_MSGDIS_BMA_NOT_PRESENT);
            goto error_exit;

        default:
            debug_log(DLOG_ERROR, "%s:uip_call_class_method_redfish failed,method=%s,ret=%d", __FUNCTION__,
                BUSY_ETH_METHOD_SET_IPV4_INFO, ret);
            result_val = HTTP_INTERNAL_SERVER_ERROR;
            goto error_exit;
    }

    (void)redfish_get_sys_eth_memberid(i_paras->obj_handle, memberid, sizeof(memberid));
    iRet = snprintf_s(oration_name, sizeof(oration_name), sizeof(oration_name) - 1, "Set %s IPv4", memberid);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    act_eth_config_info = task_info_new_function(i_paras->obj_handle, free_state_flag, BUSY_ETH_ATTRIBUTE_CONFIG_STATE,
        BUSY_ETH_ATTRIBUTE_CONFIG_RESULT, oration_name, i_paras->uri, HTTP_ACTION);
    if (NULL == act_eth_config_info) {
        debug_log(DLOG_DEBUG, "%s: task_info_new_function failed", __FUNCTION__);
        result_val = HTTP_INTERNAL_SERVER_ERROR;
        goto error_exit;
    }

    act_eth_config_info->rsc_privilege = USERROLE_BASICSETTING;
    ret = create_new_task(oration_name, ipinfo_config_status_monitor_fn, act_eth_config_info, i_paras->val_jso,
        i_paras->uri, o_result_jso);
    if (RF_OK != ret) {
        debug_log(DLOG_DEBUG, "%s: create_new_task failed", __FUNCTION__);
        task_monitor_info_free(act_eth_config_info);
        result_val = HTTP_INTERNAL_SERVER_ERROR;
        goto error_exit;
    }
    result_val = HTTP_ACCEPTED;

error_exit:

    if (HTTP_INTERNAL_SERVER_ERROR == result_val && NULL != o_message_jso) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    }

    return result_val;
}


gint32 system_ethernetinterface_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret;
    guchar board_type = 0;
    OBJ_HANDLE obj_handle = 0;

    
    ret = redfish_get_x86_enable_type(&board_type);
    return_val_if_fail(VOS_OK == ret, HTTP_NOT_FOUND);
    return_val_if_fail(DISABLE != board_type, HTTP_NOT_FOUND);
    

    
    if (redfish_check_system_uri_valid(i_paras->uri)) {
        
        debug_log(DLOG_DEBUG, "%s: member_id = %s", __FUNCTION__, i_paras->member_id);
        ret = redfish_check_system_eth_uri_effective(i_paras->member_id, &obj_handle);
        return_val_if_fail(VOS_OK == ret, HTTP_NOT_FOUND);

        i_paras->obj_handle = obj_handle;

        *prop_provider = g_system_eth_provider;
        *count = sizeof(g_system_eth_provider) / sizeof(PROPERTY_PROVIDER_S);
        
        return VOS_OK;
    }

    return HTTP_NOT_FOUND;
}

LOCAL gint32 get_system_ethernetinterface_acinfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL PROPERTY_PROVIDER_S g_system_eth_acinfo_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_ethernetinterface_acinfo_odata_id, NULL, NULL, ETAG_FLAG_ENABLE}
};


LOCAL gint32 get_system_ethernetinterface_acinfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar odata_id[MAX_URL_LEN] = {0};
    gchar member_id[MAX_RSC_ID_LEN] = {0};

    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (i_paras->is_satisfy_privi != TRUE) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = redfish_get_sys_eth_memberid(i_paras->obj_handle, member_id, sizeof(member_id));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: redfish_get_sys_eth_memberid failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = rf_get_rsc_odata_prop(URI_FORMAT_SYS_ETH_ACINFO, member_id, odata_id, sizeof(odata_id));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get odata_id prop failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string(odata_id);

    return HTTP_OK;
}


gint32 system_ethernetinterface_acinfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    
    gint32 ret;
    guchar board_type = 0;
    OBJ_HANDLE obj_handle = 0;
    gchar url_temp[MAX_URL_LEN] = {0};
    gchar *flag = NULL;

    ret = redfish_get_x86_enable_type(&board_type);
    return_val_if_fail(VOS_OK == ret, HTTP_NOT_FOUND);
    return_val_if_fail(DISABLE != board_type, HTTP_NOT_FOUND);

    

    if (redfish_check_system_uri_valid(i_paras->uri)) {
        debug_log(DLOG_DEBUG, "%s: old_member_id = %s", __FUNCTION__, i_paras->member_id);
        (void)strncpy_s(url_temp, sizeof(url_temp), i_paras->uri, sizeof(url_temp) - 1);
        flag = g_strrstr(url_temp, SLASH_FLAG_STR);
        return_val_if_expr(NULL == flag, HTTP_NOT_FOUND);
        *flag = '\0';
        flag = NULL;

        flag = g_strrstr(url_temp, SLASH_FLAG_STR);
        return_val_if_expr(NULL == flag, HTTP_NOT_FOUND);
        (void)memset_s(i_paras->member_id, sizeof(i_paras->member_id), 0, sizeof(i_paras->member_id));
        (void)strncpy_s(i_paras->member_id, sizeof(i_paras->member_id), flag + 1, sizeof(i_paras->member_id) - 1);
        debug_log(DLOG_DEBUG, "%s: new_member_id = %s", __FUNCTION__, i_paras->member_id);

        ret = redfish_check_system_eth_uri_effective(i_paras->member_id, &obj_handle);
        return_val_if_fail(VOS_OK == ret, HTTP_NOT_FOUND);

        i_paras->obj_handle = obj_handle;

        *prop_provider = g_system_eth_acinfo_provider;
        *count = sizeof(g_system_eth_acinfo_provider) / sizeof(PROPERTY_PROVIDER_S);
        return VOS_OK;
    }

    return HTTP_NOT_FOUND;
}

// 以太网资源结束



LOCAL gint32 get_system_ethernetinterface_vlans_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_ethernetinterface_vlans_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_ethernetinterface_vlans_odata_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_ethernetinterface_vlans_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_system_eth_vlans_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_ethernetinterface_vlans_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_ethernetinterface_vlans_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS_COUNT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_ethernetinterface_vlans_odata_count, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_ethernetinterface_vlans_members, NULL, NULL, ETAG_FLAG_ENABLE}
};


LOCAL gint32 get_vlans_uri_or_context(OBJ_HANDLE obj_handle, gchar *vlans_uri, gint32 uri_len,
    const gchar *uri_template)
{
    gint32 ret;
    gchar member_id[MAX_RSC_ID_LEN] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};

    return_val_do_info_if_expr(NULL == vlans_uri, VOS_ERR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    ret = redfish_get_sys_eth_memberid(obj_handle, member_id, sizeof(member_id));
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_DEBUG, "%s: get eth member id fail", __FUNCTION__));

    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, debug_log(DLOG_ERROR, "%s: get slot id fail", __FUNCTION__));

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    ret = snprintf_s(vlans_uri, uri_len, uri_len - 1, uri_template, slot_id, member_id);
#pragma GCC diagnostic pop

    return_val_do_info_if_fail(ret > 0, VOS_ERR, debug_log(DLOG_ERROR, "%s: snprintf_s vlans uri fail", __FUNCTION__));

    return VOS_OK;
}


LOCAL gint32 get_system_ethernetinterface_vlans_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar vlans_uri[MAX_URI_LENGTH] = {0};

    if ((NULL == o_result_jso) || (NULL == o_message_jso) || (VOS_OK != provider_paras_check(i_paras))) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = get_vlans_uri_or_context(i_paras->obj_handle, vlans_uri, sizeof(vlans_uri), URI_FORMAT_SYS_ETH_VLANS);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get_vlans_uri_by_eth_object fail", __FUNCTION__));
    

    *o_result_jso = json_object_new_string((const char *)vlans_uri);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : json_object_new_string fail", __FUNCTION__));

    return HTTP_OK;
}


LOCAL gint32 get_system_ethernetinterface_vlans_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar vlans_odata_context[MAX_URI_LENGTH] = {0};

    if ((NULL == o_result_jso) || (NULL == o_message_jso) || (VOS_OK != provider_paras_check(i_paras))) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = get_vlans_uri_or_context(i_paras->obj_handle, vlans_odata_context, sizeof(vlans_odata_context),
        VLANS_ODATA_CONTEXT);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get_vlans_uri_by_eth_object fail", __FUNCTION__));
    

    *o_result_jso = json_object_new_string((const char *)vlans_odata_context);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : json_object_new_string fail", __FUNCTION__));

    return HTTP_OK;
}


LOCAL gint32 check_vlans_uri(gchar *uri, OBJ_HANDLE *eth_handle, gboolean is_vlan_url)
{
    errno_t safe_fun_ret = EOK;
    gint32 ret;
    gint32 result = VOS_ERR;
    GSList *eth_obj_list = NULL;
    GSList *node = NULL;
    OBJ_HANDLE obj_handle = 0;
    gchar vlans_uri[MAX_URI_LENGTH] = {0};
    guint32 net_dev_function = 0;
    guint8 port_type = 0;
    gchar slot[MAX_RSC_ID_LEN] = {0};

    ret = redfish_get_board_slot(slot, MAX_RSC_ID_LEN);
    return_val_do_info_if_expr(VOS_OK != ret, result,
        debug_log(DLOG_ERROR, "%s : redfish_get_board_slot fail", __FUNCTION__));

    ret = dfl_get_object_list(ETH_CLASS_NAME_BUSY_ETH, &eth_obj_list);
    return_val_do_info_if_expr((VOS_OK != ret) || (NULL == eth_obj_list), result,
        debug_log(DLOG_ERROR, "%s : get BusinessPort object list fail or value is NULL", __FUNCTION__));

    for (node = eth_obj_list; node; node = g_slist_next(node)) {
        obj_handle = (OBJ_HANDLE)node->data;

        // 以太网或IB显示
        (void)dal_get_property_value_uint32(obj_handle, BUSY_ETH_ATTRIBUTE_NETDEV_FUNCTYPE, &net_dev_function);
        
        
        continue_if_expr((0 == (net_dev_function & NETDEV_FUNCTYPE_ETHERNET)) &&
            (0 == (net_dev_function & NETDEV_FUNCTYPE_IB)));
        
        

        // 类型既不是物理网口也不是逻辑网口，过滤掉
        (void)dal_get_property_value_byte(obj_handle, BUSY_ETH_ATTRIBUTE_PORT_TYPE, &port_type);
        // 认为0xff是没bma的情况下，物理网口的值
        continue_if_expr((ETH_PORT_TYPE_VIRTUAL != port_type) && (ETH_PORT_TYPE_PHYSICAL != port_type) &&
            (0xff != port_type));

        
        ret = get_vlans_uri_or_context(obj_handle, vlans_uri, sizeof(vlans_uri), URI_FORMAT_SYS_ETH_VLANS);
        continue_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_DEBUG, "%s : get vlans_uri fail", __FUNCTION__));
        

        
        // 由于删除vlan的url不能由BMC检查，因为连续删除可能会404，vlan id需要Agentless方法检查
        if (TRUE == is_vlan_url) {
            safe_fun_ret = strncat_s(vlans_uri, sizeof(vlans_uri), SLASH_FLAG_STR, strlen(SLASH_FLAG_STR));
            do_val_if_expr(safe_fun_ret != EOK,
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
            if (0 == g_ascii_strncasecmp(uri, vlans_uri, strlen(vlans_uri))) {
                do_if_fail(NULL == eth_handle, (*eth_handle = obj_handle));
                result = VOS_OK;
                break;
            }
            continue;
        }
        

        if (0 == g_ascii_strcasecmp(uri, vlans_uri)) {
            if (NULL != eth_handle) {
                *eth_handle = obj_handle;
            }

            result = VOS_OK;
            break;
        }
    }

    g_slist_free(eth_obj_list);

    return result;
}


LOCAL gint32 get_vlan_object_list_by_eth_obj(OBJ_HANDLE obj_handle, GSList **vlan_obj_list)
{
    gint32 ret;
    gint32 result = VOS_ERR;
    gchar eth_bdf[PROP_VAL_LENGTH] = {0};
    gchar bdf[PROP_VAL_LENGTH] = {0};
    GSList *all_vlan_list = NULL;
    GSList *node = NULL;

    return_val_do_info_if_expr(NULL == vlan_obj_list, result, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    (void)dal_get_property_value_string(obj_handle, BUSY_ETH_ATTRIBUTE_BDF, eth_bdf, PROP_VAL_LENGTH);
    return_val_do_info_if_fail(g_strcmp0(PROPERTY_VALUE_DEFAULT_STRING, eth_bdf), result,
        debug_log(DLOG_MASS, "%s : invalid eth bdf value", __FUNCTION__));

    ret = dfl_get_object_list(CLASS_BUSINESS_PORT_VLAN, &all_vlan_list); // 有可能没有VLAN对象，日志打印为mass
    return_val_do_info_if_expr((VOS_OK != ret) || (NULL == all_vlan_list), result,
        debug_log(DLOG_MASS, "%s : get BusinessPortVLAN object list fail or value is NULL", __FUNCTION__));

    for (node = all_vlan_list; node; node = g_slist_next(node)) {
        (void)dal_get_property_value_string((OBJ_HANDLE)node->data, PROPERTY_BUSINESS_PORT_VLAN_ETHBDF, bdf,
            PROP_VAL_LENGTH);

        if (0 == g_strcmp0(bdf, eth_bdf)) {
            *vlan_obj_list = g_slist_append(*vlan_obj_list, g_variant_new_int32((OBJ_HANDLE)node->data));
            result = VOS_OK;
        }
    }
    g_slist_free(all_vlan_list);

    return result;
}


LOCAL gint32 get_system_ethernetinterface_vlans_odata_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint32 count = 0;
    GSList *vlan_obj_list = NULL; // 该网口关联的vlan的对象句柄列表

    if ((NULL == o_result_jso) || (NULL == o_message_jso) || (VOS_OK != provider_paras_check(i_paras))) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = get_vlan_object_list_by_eth_obj(i_paras->obj_handle, &vlan_obj_list);
    // 失败了不返回，count为0
    do_info_if_true(VOS_OK != ret, debug_log(DLOG_MASS, "%s : not ref vlan", __FUNCTION__));

    if (VOS_OK == ret) {
        
        count = (gint32)g_slist_length(vlan_obj_list);

        uip_free_gvariant_list(vlan_obj_list);
    }
    
    return_val_do_info_if_expr(0 == count, HTTP_OK, (*o_result_jso = json_object_new_int(0)));
    
    *o_result_jso = json_object_new_int(count);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : json_object_new_int fail", __FUNCTION__));

    return HTTP_OK;
}


LOCAL gint32 get_system_ethernetinterface_vlans_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    GSList *vlan_obj_list = NULL;
    GVariant *gvariant_vlan_obj = NULL;
    OBJ_HANDLE obj_handle = 0;
    guint32 i;
    gchar member_id[PROP_VAL_LENGTH] = {0};
    guint32 vlan_id = 0;
    gchar slot[MAX_RSC_ID_LEN] = {0};
    gchar vlan_uri[MAX_URI_LENGTH] = {0};
    json_object *odata_object = NULL;
    json_object *odata_id = NULL;
    guint32 vlan_obj_num;

    if ((NULL == o_result_jso) || (NULL == o_message_jso) || (VOS_OK != provider_paras_check(i_paras))) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_array();
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : json_object_new_array fail", __FUNCTION__));

    ret = redfish_get_sys_eth_memberid(i_paras->obj_handle, member_id, PROP_VAL_LENGTH); // 失败返回空数组
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_OK,
        debug_log(DLOG_ERROR, "%s : get sys eth member id fail", __FUNCTION__));

    ret = redfish_get_board_slot(slot, MAX_RSC_ID_LEN); // 失败返回空数组
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_OK, debug_log(DLOG_ERROR, "%s : get slot id fail", __FUNCTION__));

    ret = get_vlan_object_list_by_eth_obj(i_paras->obj_handle, &vlan_obj_list); // 若失败返回空数组
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_OK, debug_log(DLOG_MASS, "%s : not ref vlan", __FUNCTION__));

    vlan_obj_num = g_slist_length(vlan_obj_list);

    for (i = 0; i < vlan_obj_num; i++) {
        gvariant_vlan_obj = (GVariant *)g_slist_nth_data(vlan_obj_list, i);
        continue_do_info_if_fail(NULL != gvariant_vlan_obj,
            debug_log(DLOG_ERROR, "%s : the %d value is NULL", __FUNCTION__, i));

        obj_handle = g_variant_get_int32(gvariant_vlan_obj);

        (void)dal_get_property_value_uint32(obj_handle, PROPERTY_BUSINESS_PORT_VLAN_ID, &vlan_id);
        
        ret =
            snprintf_s(vlan_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, URI_FORMAT_SYS_ETH_VLAN, slot, member_id, vlan_id);
        
        continue_do_info_if_fail(VOS_OK < ret, debug_log(DLOG_ERROR, "%s : snprintf_s vlan_uri fail", __FUNCTION__));

        odata_object = json_object_new_object();
        continue_do_info_if_fail(NULL != odata_object,
            debug_log(DLOG_ERROR, "%s : json_object_new_object fail", __FUNCTION__));

        odata_id = json_object_new_string((const char *)vlan_uri);
        do_info_if_true(NULL == odata_id, debug_log(DLOG_ERROR, "%s : json_object_new_string fail", __FUNCTION__));

        json_object_object_add(odata_object, RFPROP_ODATA_ID, odata_id);
        odata_id = NULL;

        ret = json_object_array_add(*o_result_jso, odata_object);
        continue_do_info_if_fail(VOS_OK == ret, (void)json_object_put(odata_object);
            debug_log(DLOG_ERROR, "%s : array add fail", __FUNCTION__));

        odata_object = NULL;
    }
    uip_free_gvariant_list(vlan_obj_list);

    return HTTP_OK;
}


gint32 system_ethernetinterface_vlans_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint8 board_type = 0;

    
    ret = redfish_get_x86_enable_type(&board_type);
    return_val_if_fail(VOS_OK == ret, HTTP_NOT_FOUND);
    return_val_if_fail(DISABLE != board_type, HTTP_NOT_FOUND);

    
    ret = check_vlans_uri(i_paras->uri, &obj_handle, FALSE);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s : check vlans uri(%s) fail,", __FUNCTION__, i_paras->uri));

    // 关联网口的对象句柄
    i_paras->obj_handle = obj_handle;

    *prop_provider = g_system_eth_vlans_provider;
    *count = G_N_ELEMENTS(g_system_eth_vlans_provider);

    return HTTP_OK;
}


LOCAL gint32 get_system_ethernetinterface_vlan_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_ethernetinterface_vlan_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_ethernetinterface_vlan_enable(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_ethernetinterface_vlan_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_ethernetinterface_vlan_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_ethernetinterface_vlan_id_prop(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __get_system_ethernetinterface_vlan_action(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __act_ethernetinterface_vlan_configure(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_system_eth_vlan_provider[] = {
    {RFPROP_COMMON_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_ethernetinterface_vlan_id_prop, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_ethernetinterface_vlan_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_ethernetinterface_vlan_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RF_PROPERTY_VLAN_ENABLE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_ethernetinterface_vlan_enable, NULL, NULL, ETAG_FLAG_ENABLE},
    {RF_PROPERTY_VLAN_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_ethernetinterface_vlan_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_OEM, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_ethernetinterface_vlan_oem, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ACTIONS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, __get_system_ethernetinterface_vlan_action, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SYSTEM_ETHERNET_VLAN_CONFIGURE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, NULL, NULL, __act_ethernetinterface_vlan_configure, ETAG_FLAG_ENABLE}
};


LOCAL gint32 get_vlan_uri_or_context(OBJ_HANDLE obj_handle, gchar *vlan_uri, gint32 uri_len, const gchar *uri_template,
    gint32 is_uri)
{
    gint32 ret;
    gchar member_id[MAX_RSC_ID_LEN] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};
    gchar eth_bdf[PROP_VAL_LENGTH] = {0};
    guint32 vlan_id = 0;
    OBJ_HANDLE eth_handle = 0;

    return_val_do_info_if_expr(NULL == vlan_uri, VOS_ERR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    ret = dal_get_property_value_string(obj_handle, PROPERTY_BUSINESS_PORT_VLAN_ETHBDF, eth_bdf, sizeof(eth_bdf));
    return_val_if_fail(DFL_OK == ret, VOS_ERR);

    ret = dal_get_specific_object_string(ETH_CLASS_NAME_BUSY_ETH, BUSY_ETH_ATTRIBUTE_BDF, (const gchar *)eth_bdf,
        &eth_handle);
    return_val_if_fail(DFL_OK == ret, VOS_ERR);

    ret = redfish_get_sys_eth_memberid(eth_handle, member_id, sizeof(member_id));
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get eth member id fail", __FUNCTION__));

    ret = redfish_get_board_slot(slot_id, MAX_RSC_ID_LEN);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, debug_log(DLOG_ERROR, "%s: get slot id fail", __FUNCTION__));

    if (is_uri) {
        ret = dal_get_property_value_uint32(obj_handle, PROPERTY_BUSINESS_PORT_VLAN_ID, &vlan_id);
        return_val_if_fail(DFL_OK == ret, VOS_ERR);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
        ret = snprintf_s(vlan_uri, uri_len, uri_len - 1, uri_template, slot_id, member_id, vlan_id);
#pragma GCC diagnostic pop
        return_val_do_info_if_fail(ret > 0, VOS_ERR, debug_log(DLOG_ERROR, "%s: snprintf_s fail", __FUNCTION__));
    } else {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
        ret = snprintf_s(vlan_uri, uri_len, uri_len - 1, uri_template, slot_id, member_id);
#pragma GCC diagnostic pop
        return_val_do_info_if_fail(ret > 0, VOS_ERR, debug_log(DLOG_ERROR, "%s: snprintf_s fail", __FUNCTION__));
    }

    return VOS_OK;
}


LOCAL gint32 get_system_ethernetinterface_vlan_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar vlan_uri[MAX_URI_LENGTH] = {0};
    gint32 ret;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = get_vlan_uri_or_context(i_paras->obj_handle, vlan_uri, sizeof(vlan_uri), URI_FORMAT_SYS_ETH_VLAN, TRUE);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get vlan uri fail", __FUNCTION__));
    

    *o_result_jso = json_object_new_string((const char *)vlan_uri);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : json_object_new_string fail", __FUNCTION__));

    return HTTP_OK;
}


LOCAL gint32 get_system_ethernetinterface_vlan_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar vlan_odata_context[MAX_URI_LENGTH] = {0};
    gint32 ret;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = get_vlan_uri_or_context(i_paras->obj_handle, vlan_odata_context, sizeof(vlan_odata_context),
        VLAN_ODATA_CONTEXT, FALSE);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get odata context fail", __FUNCTION__));
    

    *o_result_jso = json_object_new_string((const char *)vlan_odata_context);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : json_object_new_string fail", __FUNCTION__));

    return HTTP_OK;
}


LOCAL gint32 get_system_ethernetinterface_vlan_id_prop(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint32 vlan_id = 0;
    gchar vlan_resource_id[PROP_VAL_LENGTH] = {0};

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    (void)dal_get_property_value_uint32(i_paras->obj_handle, PROPERTY_BUSINESS_PORT_VLAN_ID, &vlan_id);

    
    // PROP_VAL_LENGTH大小为64，大于格式化后字符串长度，无需判断返回值
    (void)snprintf_s(vlan_resource_id, PROP_VAL_LENGTH, PROP_VAL_LENGTH - 1, "%u", vlan_id);

    *o_result_jso = json_object_new_string((const char *)vlan_resource_id);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_string fail", __FUNCTION__));

    return HTTP_OK;
}

LOCAL gint32 get_system_ethernetinterface_vlan_enable(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint8 vlan_enable = 0;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    (void)dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_BUSINESS_PORT_VLAN_ENABLE, &vlan_enable);

    return_val_do_info_if_expr((0 != vlan_enable) && (1 != vlan_enable), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s: invalid vlan state value", __FUNCTION__));

    *o_result_jso = json_object_new_boolean((gint32)vlan_enable);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_boolean fail", __FUNCTION__));

    return HTTP_OK;
}


LOCAL gint32 get_system_ethernetinterface_vlan_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint32 vlan_id = 0;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    (void)dal_get_property_value_uint32(i_paras->obj_handle, PROPERTY_BUSINESS_PORT_VLAN_ID, &vlan_id);
    
    return_val_do_info_if_expr(vlan_id > RF_MAX_VLAN_ID, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: invalid vlan id is %d", __FUNCTION__, vlan_id));

    *o_result_jso = json_object_new_int((gint32)vlan_id);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_int fail", __FUNCTION__));

    return HTTP_OK;
}


LOCAL void __get_vlan_ip_info(OBJ_HANDLE obj_handle, json_object *huawei_jso)
{
    gint32 ret;
    GVariant *property_value = NULL;
    json_object *ip_value_array = NULL;

    return_if_expr(0 == obj_handle || NULL == huawei_jso);

    // 获取IPv4信息
    ret = dfl_get_property_value(obj_handle, PROPERTY_BUSINESS_PORT_VLAN_IPV4_INFO, &property_value);
    if (DFL_OK == ret) {
        ip_value_array = json_object_new_array();
        if (NULL != ip_value_array) {
            get_ipv4_addr_mask_gateway(property_value, &ip_value_array);
        }
        g_variant_unref(property_value);
        property_value = NULL;
    }
    json_object_object_add(huawei_jso, RF_PROPERTY_IPV4_ADDR, ip_value_array);
    ip_value_array = NULL;

    // 获取IPv6信息
    ret = dfl_get_property_value(obj_handle, PROPERTY_BUSINESS_PORT_VLAN_IPV6_INFO, &property_value);
    if (DFL_OK == ret) {
        ip_value_array = json_object_new_array();
        if (NULL != ip_value_array) {
            get_ipv6_addr_prefix(property_value, &ip_value_array);
        }
        g_variant_unref(property_value);
    }
    json_object_object_add(huawei_jso, RF_PROPERTY_IPV6_ADDR, ip_value_array);
}


LOCAL gint32 get_system_ethernetinterface_vlan_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar vlan_priority[PROP_VAL_LENGTH] = {0};
    gint32 vlan_priority_enabled = 0xff;
    json_object *priority = NULL;
    json_object *huawei = NULL;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    (void)dal_get_property_value_string(i_paras->obj_handle, PROPERTY_BUSINESS_PORT_VLAN_PRIORITY, vlan_priority,
        PROP_VAL_LENGTH);

    if (0 == g_strcmp0(RFPROP_VAL_ENABLE, vlan_priority)) {
        vlan_priority_enabled = TRUE;
    } else if (0 == g_strcmp0(RFPROP_VAL_DISABLE, vlan_priority)) {
        vlan_priority_enabled = FALSE;
    } else {
        debug_log(DLOG_DEBUG, "%s: invalid vlan priority is %s", __FUNCTION__, vlan_priority);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_object fail", __FUNCTION__));

    huawei = json_object_new_object(); // 失败返回空对象
    return_val_do_info_if_expr(NULL == huawei, HTTP_OK,
        debug_log(DLOG_ERROR, "%s: json_object_new_object fail", __FUNCTION__));

    priority = json_object_new_boolean((gint32)vlan_priority_enabled);
    return_val_do_info_if_expr(NULL == priority, HTTP_OK,
        debug_log(DLOG_ERROR, "%s: json_object_new_boolean fail", __FUNCTION__);
        json_object_object_add(*o_result_jso, RFPROP_COMMON_HUAWEI, huawei));

    json_object_object_add(huawei, RFPROP_SYSTEM_VLAN_PRIORITY, priority);

    
    
    __get_vlan_ip_info(i_paras->obj_handle, huawei);
    

    
    
    __get_eth_is_onboot(i_paras->obj_handle, huawei);
    

    json_object_object_add(*o_result_jso, RFPROP_COMMON_HUAWEI, huawei);

    return HTTP_OK;
}


LOCAL gint32 __get_system_ethernetinterface_vlan_action(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gchar vlan_action_url[MAX_URL_LEN] = {0};
    json_object *oem_jso = NULL;
    json_object *huawei_jso = NULL;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    obj_handle = i_paras->obj_handle;
    (void)get_vlan_uri_or_context(obj_handle, vlan_action_url, sizeof(vlan_action_url), URI_FORMAT_SYS_ETH_VLAN, TRUE);

    *o_result_jso = json_object_new_object();
    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    oem_jso = json_object_new_object();
    if (NULL == oem_jso) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__);
        return HTTP_OK;
    }
    json_object_object_add(*o_result_jso, RFPROP_COMMON_OEM, oem_jso);
    huawei_jso = json_object_new_object();
    if (NULL == huawei_jso) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__);
        return HTTP_OK;
    }
    json_object_object_add(oem_jso, RFPROP_COMMON_HUAWEI, huawei_jso);
    rf_add_action_prop(huawei_jso, vlan_action_url, RFPROP_SYSTEM_ETHERNET_VLAN_CONFIGURE);

    return HTTP_OK;
}


LOCAL gint32 __fill_act_vlan_config_input_list(json_object *body_checked, GSList **input_list,
    json_object **o_message_jso)
{
    gint32 ret;
    json_object *temp_jso = NULL;
    json_object *array_iter = NULL;
    gint32 ipv4_arr_length = 0;
    gint32 i = 0;
    gchar address[PROP_VAL_MAX_LENGTH] = {0};
    gchar netmask[PROP_VAL_MAX_LENGTH] = {0};
    gchar origin[PROP_VAL_MAX_LENGTH] = {0};
    gchar gateway[PROP_VAL_MAX_LENGTH] = {0};
    guint8 vlan_enable;
    guint8 is_onboot;

    if (NULL == body_checked || NULL == input_list || NULL == o_message_jso) {
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = json_object_object_get_ex(body_checked, RF_PROPERTY_VLAN_ENABLE, &temp_jso);
    if (TRUE == ret) {
        vlan_enable = (TRUE == json_object_get_boolean(temp_jso)) ? TRUE : FALSE; // 修改codedex
    } else {
        // 接口未传时，填充默认值
        vlan_enable = G_MAXUINT8;
    }
    *input_list = g_slist_append(*input_list, g_variant_new_byte(vlan_enable));
    temp_jso = NULL;

    ret = json_object_object_get_ex(body_checked, RF_PROPERTY_IS_ONBOOT, &temp_jso);
    if (TRUE == ret) {
        is_onboot = (TRUE == json_object_get_boolean(temp_jso)) ? 1 : 0;
    } else {
        // 接口未传时，填充默认值
        is_onboot = G_MAXUINT8;
    }
    temp_jso = NULL;

    ret = json_object_object_get_ex(body_checked, RF_PROPERTY_IPV4_ADDR, &temp_jso);
    if (TRUE != ret) {
        debug_log(DLOG_DEBUG, "%s: no ipv4 address", __FUNCTION__);
        goto exit;
    }
    ipv4_arr_length = json_object_array_length(temp_jso);
    if (ipv4_arr_length > 0) {
        for (i = 0; i < ipv4_arr_length; i++) {
            array_iter = json_object_array_get_idx(temp_jso, i);
            continue_if_expr(NULL == array_iter);

            __fill_ipv4_prop_list(array_iter, RF_PROPERTY_ADDR, address, sizeof(address));
            do_if_expr(i < ipv4_arr_length - 1, (void)strncat_s(address, sizeof(address), RF_IPV4_PROP_SPLIT_FLAG,
                strlen(RF_IPV4_PROP_SPLIT_FLAG))); // 不是最后一个才需要分割

            __fill_ipv4_prop_list(array_iter, RF_PROPERTY_SUB_MASK, netmask, sizeof(netmask));
            do_if_expr(i < ipv4_arr_length - 1,
                (void)strncat_s(netmask, sizeof(netmask), RF_IPV4_PROP_SPLIT_FLAG, strlen(RF_IPV4_PROP_SPLIT_FLAG)));

            __fill_ipv4_prop_list(array_iter, RF_PROPERTY_ADDR_ORIGIN, origin, sizeof(origin));
            do_if_expr(i < ipv4_arr_length - 1,
                (void)strncat_s(origin, sizeof(origin), RF_IPV4_PROP_SPLIT_FLAG, strlen(RF_IPV4_PROP_SPLIT_FLAG)));

            __fill_ipv4_prop_list(array_iter, RF_PROPERTY_GATEWAY, gateway, sizeof(gateway));
            do_if_expr(i < ipv4_arr_length - 1,
                (void)strncat_s(gateway, sizeof(gateway), RF_IPV4_PROP_SPLIT_FLAG, strlen(RF_IPV4_PROP_SPLIT_FLAG)));
        }
    } else {
        (void)strncpy_s(address, sizeof(address), PROPERTY_VALUE_DEFAULT_STRING, strlen(PROPERTY_VALUE_DEFAULT_STRING));
        (void)strncpy_s(netmask, sizeof(netmask), PROPERTY_VALUE_DEFAULT_STRING, strlen(PROPERTY_VALUE_DEFAULT_STRING));
        (void)strncpy_s(origin, sizeof(origin), PROPERTY_VALUE_DEFAULT_STRING, strlen(PROPERTY_VALUE_DEFAULT_STRING));
        (void)strncpy_s(gateway, sizeof(gateway), PROPERTY_VALUE_DEFAULT_STRING, strlen(PROPERTY_VALUE_DEFAULT_STRING));
    }

exit:
    debug_log(DLOG_DEBUG, "%s: address = %s", __FUNCTION__, address);
    debug_log(DLOG_DEBUG, "%s: netmask = %s", __FUNCTION__, netmask);
    debug_log(DLOG_DEBUG, "%s: origin = %s", __FUNCTION__, origin);
    debug_log(DLOG_DEBUG, "%s: gateway = %s", __FUNCTION__, gateway);
    debug_log(DLOG_DEBUG, "%s: is_onboot = %d", __FUNCTION__, is_onboot);

    *input_list = g_slist_append(*input_list, g_variant_new_string(address));
    *input_list = g_slist_append(*input_list, g_variant_new_string(netmask));
    *input_list = g_slist_append(*input_list, g_variant_new_string(origin));
    *input_list = g_slist_append(*input_list, g_variant_new_string(gateway));
    *input_list = g_slist_append(*input_list, g_variant_new_byte(is_onboot));

    return VOS_OK;
}


LOCAL gint32 __act_ethernetinterface_vlan_configure(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;
    gint32 ret;
    gint32 result_val = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    guint32 free_state_flag = 0;
    TASK_MONITOR_INFO_S *act_vlan_config_info = NULL;
    gchar oration_name[PROP_VAL_LENGTH] = {0};
    gchar team_bdf[PROP_VAL_LENGTH] = {0};
    OBJ_HANDLE obj_handle = 0;
    gchar memberid[PROP_VAL_LENGTH] = {0};
    guint32 vlan_id = 0;

    // URL示例:
    // /redfish/v1/Systems/1/EthernetInterfaces/mainboardNIC1Port4/VLANs/201/Actions/Oem/Huawei/VLanNetworkInterface.Configure
    if (NULL == i_paras || NULL == o_message_jso) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (0 == g_strcmp0(dal_json_object_get_str(i_paras->val_jso), JSON_NULL_OBJECT_STR)) {
        (void)create_message_info(MSGID_MALFORMED_JSON, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }

    ret = __fill_act_vlan_config_input_list(i_paras->val_jso, &input_list, o_message_jso);
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s: __fill_act_vlan_config_input_list failed, ret = %d", __FUNCTION__, ret);
        result_val = ret;
        goto error_exit;
    }

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_BUSINESS_PORT_VLAN, METHOD_BUSINESS_PORT_VLAN_SET_VLAN_PROP, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, &output_list);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    switch (ret) {
        case VOS_OK:
            free_state_flag = g_variant_get_uint32((GVariant *)g_slist_nth_data(output_list, 0));
            g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
            debug_log(DLOG_DEBUG, "%s: free_state_flag = %d", __FUNCTION__, free_state_flag);
            break;

            
        case VOS_ERR_NOTSUPPORT:
            result_val = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_CONFIG_IP_FAILED, NULL, o_message_jso, RF_MSGDIS_BMA_NOT_PRESENT);
            goto error_exit;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            result_val = HTTP_FORBIDDEN;
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
            goto error_exit;

        default:
            debug_log(DLOG_DEBUG, "%s:uip_call_class_method_redfish failed,method=%s,ret=%d", __FUNCTION__,
                METHOD_BUSINESS_PORT_VLAN_SET_VLAN_PROP, ret);
            result_val = HTTP_INTERNAL_SERVER_ERROR;
            goto error_exit;
    }

    (void)dal_get_property_value_string(i_paras->obj_handle, PROPERTY_BUSINESS_PORT_VLAN_ETHBDF, team_bdf,
        sizeof(team_bdf));
    (void)dal_get_property_value_uint32(i_paras->obj_handle, PROPERTY_BUSINESS_PORT_VLAN_ID, &vlan_id);
    (void)dal_get_specific_object_string(ETH_CLASS_NAME_BUSY_ETH, BUSY_ETH_ATTRIBUTE_BDF, team_bdf, &obj_handle);
    (void)redfish_get_sys_eth_memberid(obj_handle, memberid, sizeof(memberid));
    iRet = snprintf_s(oration_name, sizeof(oration_name), sizeof(oration_name) - 1, "Set %s VLAN %u information",
        memberid, vlan_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    act_vlan_config_info =
        task_info_new_function(i_paras->obj_handle, free_state_flag, PROPERTY_BUSINESS_PORT_VLAN_CONFIG_STATE,
        PROPERTY_BUSINESS_PORT_VLAN_CONFIG_RESULT, oration_name, i_paras->uri, HTTP_ACTION);
    if (NULL == act_vlan_config_info) {
        debug_log(DLOG_DEBUG, "%s: task_info_new_function failed", __FUNCTION__);
        result_val = HTTP_INTERNAL_SERVER_ERROR;
        goto error_exit;
    }

    act_vlan_config_info->rsc_privilege = USERROLE_BASICSETTING;
    ret = create_new_task(oration_name, ipinfo_config_status_monitor_fn, act_vlan_config_info, i_paras->val_jso,
        i_paras->uri, o_result_jso);
    if (RF_OK != ret) {
        debug_log(DLOG_DEBUG, "%s: create_new_task failed", __FUNCTION__);
        task_monitor_info_free(act_vlan_config_info);
        result_val = HTTP_INTERNAL_SERVER_ERROR;
        goto error_exit;
    }
    result_val = HTTP_ACCEPTED;

error_exit:

    if (HTTP_INTERNAL_SERVER_ERROR == result_val && NULL != o_message_jso) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    }

    return result_val;
}


LOCAL gint32 check_vlan_uri(gchar *vlan_uri, const gchar *url_template, OBJ_HANDLE *obj_handle)
{
    int iRet = -1;
    gint32 ret;
    GSList *vlan_obj_list = NULL;
    GSList *node_vlan = NULL;
    gchar bdf_vlan[PROP_VAL_LENGTH] = {0};
    gchar member_id[PROP_VAL_LENGTH] = {0};
    gchar vlan_uri_add[MAX_URI_LENGTH] = {0};
    gchar slot[MAX_RSC_ID_LEN] = {0};
    guint32 vlan_id = 0;
    guint32 net_dev_function = 0;
    OBJ_HANDLE eth_handle = 0;
    gint32 result_val = VOS_ERR;

    ret = redfish_get_board_slot(slot, MAX_RSC_ID_LEN);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s : redfish_get_board_slot fail", __FUNCTION__));

    ret = dfl_get_object_list(CLASS_BUSINESS_PORT_VLAN, &vlan_obj_list);
    if (DFL_OK != ret || NULL == vlan_obj_list) {
        debug_log(DLOG_DEBUG, "%s: dfl_get_object_list failed, ret = %d, class_name = %s", __FUNCTION__, ret,
            CLASS_BUSINESS_PORT_VLAN);
        return VOS_ERR;
    }

    for (node_vlan = vlan_obj_list; node_vlan; node_vlan = g_slist_next(node_vlan)) {
        (void)dal_get_property_value_string((OBJ_HANDLE)node_vlan->data, PROPERTY_BUSINESS_PORT_VLAN_ETHBDF, bdf_vlan,
            sizeof(bdf_vlan));
        ret = dal_get_specific_object_string(ETH_CLASS_NAME_BUSY_ETH, BUSY_ETH_ATTRIBUTE_BDF, bdf_vlan, &eth_handle);
        continue_do_info_if_fail(DFL_OK == ret,
            debug_log(DLOG_INFO, "%s: dal_get_specific_object_string failed, ret = %d, class_name = %s", __FUNCTION__,
            ret, ETH_CLASS_NAME_BUSY_ETH));

        (void)dal_get_property_value_uint32(eth_handle, BUSY_ETH_ATTRIBUTE_NETDEV_FUNCTYPE, &net_dev_function);
        continue_if_expr(0 == (net_dev_function & NETDEV_FUNCTYPE_ETHERNET));

        (void)dal_get_property_value_uint32((OBJ_HANDLE)node_vlan->data, PROPERTY_BUSINESS_PORT_VLAN_ID, &vlan_id);
        (void)redfish_get_sys_eth_memberid(eth_handle, member_id, sizeof(member_id));
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
        iRet = snprintf_s(vlan_uri_add, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, url_template, slot, member_id, vlan_id);
#pragma GCC diagnostic pop
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

        if (0 == g_ascii_strcasecmp(vlan_uri, vlan_uri_add)) {
            *obj_handle = (OBJ_HANDLE)node_vlan->data;
            result_val = VOS_OK;
            break;
        }
    }

    do_if_fail(VOS_OK == result_val, debug_log(DLOG_ERROR, "%s: check vlan url failed", __FUNCTION__));
    g_slist_free(vlan_obj_list);

    return result_val;
}

gint32 system_ethernetinterface_vlan_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret;
    OBJ_HANDLE vlan_obj = 0;
    guint8 board_type = 0;
    const gchar *url_format = NULL;
    const gchar *action_flag = NULL;

    
    ret = redfish_get_x86_enable_type(&board_type);
    return_val_if_fail(VOS_OK == ret, HTTP_NOT_FOUND);
    return_val_if_fail(DISABLE != board_type, HTTP_NOT_FOUND);

    
    action_flag = g_str_case_rstr((const gchar *)i_paras->uri, ACTION_SEGMENT);
    if (action_flag) {
        url_format = URI_FORMAT_SYS_ETH_VLAN_ACTION;
    } else {
        url_format = URI_FORMAT_SYS_ETH_VLAN;
    }
    ret = check_vlan_uri(i_paras->uri, url_format, &vlan_obj);
    
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s : check vlan uri(%s) fail,", __FUNCTION__, i_paras->uri));

    i_paras->obj_handle = vlan_obj;

    *prop_provider = g_system_eth_vlan_provider;
    *count = G_N_ELEMENTS(g_system_eth_vlan_provider);

    return VOS_OK;
}


LOCAL gint32 get_system_vlan_acinfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_system_eth_vlan_acinfo_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_vlan_acinfo_odata_id, NULL, NULL, ETAG_FLAG_ENABLE}
};


LOCAL gint32 get_system_vlan_acinfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar vlan_uri[MAX_URI_LENGTH] = {0};
    gint32 ret;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret =
        get_vlan_uri_or_context(i_paras->obj_handle, vlan_uri, sizeof(vlan_uri), URI_FORMAT_SYS_ETH_VLAN_ACINFO, TRUE);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get vlan uri fail", __FUNCTION__));

    *o_result_jso = json_object_new_string((const char *)vlan_uri);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : json_object_new_string fail", __FUNCTION__));

    return HTTP_OK;
}


gint32 system_vlan_acinfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gint32 ret;
    OBJ_HANDLE vlan_obj = 0;
    guint8 board_type = 0;

    
    ret = redfish_get_x86_enable_type(&board_type);
    return_val_if_fail(VOS_OK == ret, HTTP_NOT_FOUND);
    return_val_if_fail(DISABLE != board_type, HTTP_NOT_FOUND);

    ret = check_vlan_uri(i_paras->uri, URI_FORMAT_SYS_ETH_VLAN_ACINFO, &vlan_obj);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s : check vlan uri(%s) fail,", __FUNCTION__, i_paras->uri));

    i_paras->obj_handle = vlan_obj;

    *prop_provider = g_system_eth_vlan_acinfo_provider;
    *count = G_N_ELEMENTS(g_system_eth_vlan_acinfo_provider);

    return HTTP_OK;
}


LOCAL void __fill_ipv4_prop_list(json_object *ipv4_obj, const gchar *prop_name, gchar *prop_array,
    guint32 prop_array_len)
{
    errno_t securec_rv;
    gint32 ret;
    json_object *prop_jso = NULL;

    if (NULL == ipv4_obj || NULL == prop_name || NULL == prop_array) {
        debug_log(DLOG_ERROR, "%s: NULL Pointer", __FUNCTION__);
        return;
    }

    ret = json_object_object_get_ex(ipv4_obj, prop_name, &prop_jso);
    if (TRUE == ret) {
        
        if (0 == json_object_get_string_len(prop_jso)) {
            securec_rv = strncat_s(prop_array, prop_array_len, PROPERTY_VALUE_DEFAULT_STRING,
                strlen(PROPERTY_VALUE_DEFAULT_STRING));
            do_val_if_expr(securec_rv != EOK,
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, securec_rv));
        } else {
            securec_rv = strncat_s(prop_array, prop_array_len, dal_json_object_get_str(prop_jso),
                json_object_get_string_len(prop_jso));
            do_val_if_expr(securec_rv != EOK,
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, securec_rv));
        }
    } else {
        securec_rv =
            strncat_s(prop_array, prop_array_len, PROPERTY_VALUE_DEFAULT_STRING, strlen(PROPERTY_VALUE_DEFAULT_STRING));
        do_val_if_expr(securec_rv != EOK,
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, securec_rv));
    }
}


LOCAL gint32 __fill_create_vlan_input_list(json_object *body_jso_checked, GSList **input_list,
    json_object **message_info)
{
    GSList *list_value = NULL;
    json_object *message_jso = NULL;
    json_bool vlan_enable = 0;
    gint32 ret;
    json_object *prop_jso = NULL;
    gint32 result_val = HTTP_INTERNAL_SERVER_ERROR;
    json_object *huawei = NULL;
    json_object *ipv4_array = NULL;
    json_object *array_iter = NULL;
    gchar address[PROP_VAL_MAX_LENGTH] = {0};
    gchar netmask[PROP_VAL_MAX_LENGTH] = {0};
    gchar origin[PROP_VAL_MAX_LENGTH] = {0};
    gchar gateway[PROP_VAL_MAX_LENGTH] = {0};
    guint8 is_onboot = 0;
    guint32 array_len;
    guint32 i = 0;

    if (NULL == body_jso_checked || NULL == input_list || NULL == message_info) {
        debug_log(DLOG_ERROR, "%s: NULL Pointer", __FUNCTION__);
        return result_val;
    }

    (void)get_element_boolean(body_jso_checked, RF_PROPERTY_VLAN_ENABLE, &vlan_enable);
    list_value = g_slist_append(list_value, (gpointer)g_variant_new_byte((guchar)vlan_enable));

    ret = json_object_object_get_ex(body_jso_checked, RF_PROPERTY_VLAN_ID, &prop_jso);
    goto_label_do_info_if_fail(TRUE == ret, exit,
        debug_log(DLOG_ERROR, "%s: json_object_object_get_ex failed,key=%s", __FUNCTION__, RF_PROPERTY_VLAN_ID));
    if (json_type_int != json_object_get_type(prop_jso)) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, RF_PROPERTY_VLAN_ID, &message_jso,
            dal_json_object_get_str(prop_jso), RF_PROPERTY_VLAN_ID);
        result_val = HTTP_BAD_REQUEST;
        goto exit;
    }
    
    list_value = g_slist_append(list_value, (gpointer)g_variant_new_uint16((guint16)json_object_get_int(prop_jso)));

    ret = json_object_object_get_ex(body_jso_checked, RFPROP_COMMON_OEM, &prop_jso);
    goto_label_do_info_if_fail(TRUE == ret, nomal_exit,
        debug_log(DLOG_DEBUG, "%s: json_object_object_get_ex failed,key=%s", __FUNCTION__, RFPROP_COMMON_OEM));
    ret = json_object_object_get_ex(prop_jso, RFPROP_COMMON_HUAWEI, &huawei);
    goto_label_do_info_if_fail(TRUE == ret, nomal_exit,
        debug_log(DLOG_DEBUG, "%s: json_object_object_get_ex failed", __FUNCTION__));

    prop_jso = NULL;
    ret = json_object_object_get_ex(huawei, RF_PROPERTY_IS_ONBOOT, &prop_jso);
    if (TRUE == ret) {
        is_onboot = (TRUE == json_object_get_boolean(prop_jso)) ? 1 : 0;
    } else {
        is_onboot = G_MAXUINT8;
    }

    ret = json_object_object_get_ex(huawei, RF_PROPERTY_IPV4_ADDR, &ipv4_array);
    goto_label_do_info_if_fail(TRUE == ret, nomal_exit,
        debug_log(DLOG_DEBUG, "%s: json_object_object_get_ex failed", __FUNCTION__));

    debug_log(DLOG_DEBUG, "%s: %s = %d", __FUNCTION__, RF_PROPERTY_IS_ONBOOT, is_onboot);
    debug_log(DLOG_DEBUG, "%s: %s = %s", __FUNCTION__, RF_PROPERTY_IPV4_ADDR, dal_json_object_get_str(ipv4_array));

    array_len = json_object_array_length(ipv4_array);
    
    if (0 == array_len) {
        (void)strncpy_s(address, sizeof(address), PROPERTY_VALUE_DEFAULT_STRING, strlen(PROPERTY_VALUE_DEFAULT_STRING));
        (void)strncpy_s(netmask, sizeof(netmask), PROPERTY_VALUE_DEFAULT_STRING, strlen(PROPERTY_VALUE_DEFAULT_STRING));
        (void)strncpy_s(origin, sizeof(origin), PROPERTY_VALUE_DEFAULT_STRING, strlen(PROPERTY_VALUE_DEFAULT_STRING));
        (void)strncpy_s(gateway, sizeof(gateway), PROPERTY_VALUE_DEFAULT_STRING, strlen(PROPERTY_VALUE_DEFAULT_STRING));
        goto nomal_exit;
    }

    for (i = 0; i < array_len; i++) {
        array_iter = json_object_array_get_idx(ipv4_array, i);
        continue_if_expr(NULL == array_iter);

        __fill_ipv4_prop_list(array_iter, RF_PROPERTY_ADDR, address, sizeof(address));
        do_if_expr(i < array_len - 1, (void)strncat_s(address, sizeof(address), RF_IPV4_PROP_SPLIT_FLAG,
            strlen(RF_IPV4_PROP_SPLIT_FLAG))); // 不是最后一个才需要分割

        __fill_ipv4_prop_list(array_iter, RF_PROPERTY_SUB_MASK, netmask, sizeof(netmask));
        do_if_expr(i < array_len - 1,
            (void)strncat_s(netmask, sizeof(netmask), RF_IPV4_PROP_SPLIT_FLAG, strlen(RF_IPV4_PROP_SPLIT_FLAG)));

        __fill_ipv4_prop_list(array_iter, RF_PROPERTY_ADDR_ORIGIN, origin, sizeof(origin));
        do_if_expr(i < array_len - 1,
            (void)strncat_s(origin, sizeof(origin), RF_IPV4_PROP_SPLIT_FLAG, strlen(RF_IPV4_PROP_SPLIT_FLAG)));

        __fill_ipv4_prop_list(array_iter, RF_PROPERTY_GATEWAY, gateway, sizeof(gateway));
        do_if_expr(i < array_len - 1,
            (void)strncat_s(gateway, sizeof(gateway), RF_IPV4_PROP_SPLIT_FLAG, strlen(RF_IPV4_PROP_SPLIT_FLAG)));
    }

nomal_exit:
    list_value = g_slist_append(list_value, (gpointer)g_variant_new_string(address));
    list_value = g_slist_append(list_value, (gpointer)g_variant_new_string(netmask));
    list_value = g_slist_append(list_value, (gpointer)g_variant_new_string(origin));
    list_value = g_slist_append(list_value, (gpointer)g_variant_new_string(gateway));
    list_value = g_slist_append(list_value, (gpointer)g_variant_new_byte(is_onboot));

    result_val = VOS_OK;

    *input_list = list_value;
    return result_val;
exit:
    do_if_fail(NULL == message_jso, (*message_info = message_jso));
    do_if_fail(NULL == list_value, g_slist_free_full(list_value, (GDestroyNotify)g_variant_unref));
    return result_val;
}


TASK_MONITOR_INFO_S *task_info_new_function(OBJ_HANDLE obj_handle, guint32 free_state_flag, const gchar *state_prop,
    const gchar *result_prop, gchar *operation_name, gchar *rsc_url, const gchar *rf_method)
{
    TASK_MONITOR_INFO_S *task_info = NULL;
    CONFIG_IP_TASK_MONITOR_INFO_S *user_data = NULL;

    task_info = (TASK_MONITOR_INFO_S *)g_malloc0(sizeof(TASK_MONITOR_INFO_S));
    if (NULL == task_info) {
        debug_log(DLOG_ERROR, "%s: g_malloc0 task_info failed, operation_name = %s", __FUNCTION__, operation_name);
        return NULL;
    }

    task_info->user_data = NULL;
    task_info->user_data_destory_func = NULL;

    user_data = (CONFIG_IP_TASK_MONITOR_INFO_S *)g_malloc0(sizeof(CONFIG_IP_TASK_MONITOR_INFO_S));
    if (NULL == user_data) {
        debug_log(DLOG_ERROR, "%s: g_malloc0 user_data failed, operation_name = %s", __FUNCTION__, operation_name);
        g_free(task_info);
        return NULL;
    }

    task_info->user_data = user_data;
    task_info->user_data_destory_func = g_free;

    user_data->obj_handle = obj_handle;
    user_data->free_state_flag = free_state_flag;
    user_data->state_prop = state_prop;
    user_data->result_prop = result_prop;
    user_data->rf_method = rf_method;
    (void)memset_s(user_data->operation_name, sizeof(user_data->operation_name), 0, sizeof(user_data->operation_name));
    errno_t safe_fun_ret =
        memcpy_s(user_data->operation_name, sizeof(user_data->operation_name), operation_name, strlen(operation_name));
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    (void)memset_s(user_data->rsc_url, sizeof(user_data->rsc_url), 0, sizeof(user_data->rsc_url));
    safe_fun_ret = memcpy_s(user_data->rsc_url, sizeof(user_data->rsc_url), rsc_url, strlen(rsc_url));
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

    return task_info;
}


LOCAL gint32 __get_specified_flag_state_value(CONFIG_IP_TASK_MONITOR_INFO_S *user_data, guint16 *state_value)
{
    gint32 ret;
    GVariant *property_value = NULL;
    const guint16 *state_value_array = NULL;
    gsize n_elements = 0;

    if (NULL == user_data || NULL == state_value) {
        debug_log(DLOG_ERROR, "%s: invalid input param", __FUNCTION__);
        return VOS_ERR;
    }

    ret = dfl_get_property_value(user_data->obj_handle, user_data->state_prop, &property_value);
    if (DFL_OK != ret) {
        debug_log(DLOG_ERROR, "%s: dfl_get_property_value failed,prop_name=%s,operation_name=%s", __FUNCTION__,
            user_data->state_prop, user_data->operation_name);
        return VOS_ERR;
    }

    state_value_array = (const guint16 *)g_variant_get_fixed_array(property_value, &n_elements, sizeof(guint16));
    if (NULL == state_value_array) {
        g_variant_unref(property_value);
        debug_log(DLOG_ERROR, "%s: g_variant_get_fixed_array failed,prop_name=%s,operation_name=%s", __FUNCTION__,
            user_data->state_prop, user_data->operation_name);
        return VOS_ERR;
    }

    *state_value = state_value_array[user_data->free_state_flag];
    g_variant_unref(property_value);

    debug_log(DLOG_DEBUG, "%s: %s[%d]=%d", __FUNCTION__, user_data->state_prop, user_data->free_state_flag,
        *state_value);

    return VOS_OK;
}


LOCAL gint32 __get_specified_flag_result_value(CONFIG_IP_TASK_MONITOR_INFO_S *user_data, gchar *result_val,
    guint32 result_len)
{
    gint32 ret;
    GVariant *property_value = NULL;
    const gchar **result_array = NULL;
    gsize array_length = 0;

    if (NULL == user_data || NULL == result_val) {
        debug_log(DLOG_ERROR, "%s: invalid input param", __FUNCTION__);
        return VOS_ERR;
    }

    ret = dfl_get_property_value(user_data->obj_handle, user_data->result_prop, &property_value);
    if (DFL_OK != ret) {
        debug_log(DLOG_ERROR, "%s: dfl_get_property_value failed,prop_name=%s,operation_name=%s", __FUNCTION__,
            user_data->result_prop, user_data->operation_name);
        return VOS_ERR;
    }

    result_array = g_variant_get_strv(property_value, &array_length);
    if (NULL == result_array) {
        g_variant_unref(property_value);
        debug_log(DLOG_ERROR, "%s: g_variant_get_strv failed,prop_name=%s,operation_name=%s", __FUNCTION__,
            user_data->result_prop, user_data->operation_name);
        return VOS_ERR;
    }

    (void)strncpy_s(result_val, result_len, result_array[user_data->free_state_flag], result_len - 1);
    g_free(result_array);
    g_variant_unref(property_value);

    debug_log(DLOG_DEBUG, "%s: %s[%d]=%s", __FUNCTION__, user_data->result_prop, user_data->free_state_flag,
        result_val);

    return VOS_OK;
}


gint32 ipinfo_config_status_monitor_fn(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    int iRet = -1;
    gint32 ret;
    CONFIG_IP_TASK_MONITOR_INFO_S *user_data = NULL;
    guint16 state_value = G_MAXUINT16;
    gchar result_value[MAX_URL_LEN] = {0};
    gchar created_url[MAX_URL_LEN] = {0};

    if (NULL == origin_obj_path || NULL == monitor_fn_data || NULL == message_obj) {
        debug_log(DLOG_ERROR, "%s: invalid input param", __FUNCTION__);
        return RF_FAILED;
    }

    user_data = (CONFIG_IP_TASK_MONITOR_INFO_S *)monitor_fn_data->user_data;

    ret = __get_specified_flag_state_value(user_data, &state_value);
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s: __get_specified_flag_state_value failed,operation_name=%s", __FUNCTION__,
            user_data->operation_name);
        goto error_exit;
    }

    if (state_value <= RF_FINISH_PERCENTAGE) {
        debug_log(DLOG_DEBUG, "%s: state_value = %d,operation_name=%s", __FUNCTION__, state_value,
            user_data->operation_name);
        monitor_fn_data->task_progress = state_value;
        monitor_fn_data->task_state = RF_TASK_RUNNING;
        if (RF_FINISH_PERCENTAGE == state_value) {
            if (0 == g_strcmp0(HTTP_CREATE, user_data->rf_method)) {
                ret = __get_specified_flag_result_value(user_data, result_value, sizeof(result_value));
                goto_label_do_info_if_fail(VOS_OK == ret, error_exit,
                    debug_log(DLOG_ERROR, "%s: __get_specified_flag_result_value failed,operation_name=%s",
                    __FUNCTION__, user_data->operation_name));
                
                iRet = snprintf_s(created_url, sizeof(created_url), sizeof(created_url) - 1, "%s/%s",
                    user_data->rsc_url, result_value);
                do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
                
                (void)create_message_info(MSGID_RESOURCE_CRETED, NULL, message_obj, created_url);
                (void)strncpy_s(monitor_fn_data->create_resourse, sizeof(monitor_fn_data->create_resourse), created_url,
                    sizeof(monitor_fn_data->create_resourse) - 1);
            } else {
                (void)create_message_info(MSGID_SUCCESS, NULL, message_obj);
            }
            monitor_fn_data->task_state = RF_TASK_COMPLETED;
        }
    } else if (HTTP_BAD_REQUEST == state_value) {
        monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        ret = __get_specified_flag_result_value(user_data, result_value, sizeof(result_value));
        goto_label_do_info_if_fail(VOS_OK == ret, error_exit,
            debug_log(DLOG_ERROR, "%s: __get_specified_flag_result_value failed,operation_name=%s", __FUNCTION__,
            user_data->operation_name));
        (void)create_message_info(MSGID_CONFIG_IP_FAILED, NULL, message_obj, result_value);
    } else if (HTTP_NOT_FOUND == state_value) {
        monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, message_obj, user_data->rsc_url);
    } else {
        debug_log(DLOG_ERROR, "%s: invalid state_value = %d,operation_name=%s", __FUNCTION__, state_value,
            user_data->operation_name);
        goto error_exit;
    }

    return RF_OK;

error_exit:

    monitor_fn_data->task_state = RF_TASK_EXCEPTION;
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);
    return RF_OK;
}


gint32 create_vlan(PROVIDER_PARAS_S *i_param, json_object *body_jso_checked, gchar *id, guint32 id_len,
    json_object *message_array_jso)
{
    int iRet;
    gint32 ret;
    gint32 result_val = 0;
    OBJ_HANDLE eth_obj = 0;
    json_object *message_jso = NULL;
    gchar oration_name[PROP_VAL_LENGTH] = {0};
    gchar memberid[PROP_VAL_LENGTH] = {0};
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    guint32 free_state_flag = 0;
    TASK_MONITOR_INFO_S *create_vlan_info = NULL;

    // URL示例:/redfish/v1/Systems/1/EthernetInterfaces/mainboardNIC1Port1/VLANs

    if (i_param == NULL || body_jso_checked == NULL || id == NULL || id_len == 0 || message_array_jso == NULL) {
        debug_log(DLOG_DEBUG, "%s: invalid input param", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    // 首先检查systems id和eth id
    ret = check_vlans_uri(i_param->uri, &eth_obj, FALSE);
    if (ret != RET_OK) {
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_jso, i_param->uri);
        result_val = HTTP_NOT_FOUND;
        goto error_exit;
    }

    
    ret = __fill_create_vlan_input_list(body_jso_checked, &input_list, &message_jso);
    if (ret != RET_OK) {
        result_val = ret;
        goto error_exit;
    }

    
    ret = uip_call_class_method_redfish(i_param->is_from_webui, i_param->user_name, i_param->client, eth_obj,
        ETH_CLASS_NAME_BUSY_ETH, BUSY_ETH_METHOD_CREATE_VLAN, AUTH_ENABLE, i_param->user_role_privilege, input_list,
        &output_list);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    switch (ret) {
        case RFERR_INSUFFICIENT_PRIVILEGE:
            result_val = HTTP_FORBIDDEN;
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, &message_jso);
            goto error_exit;

        case VOS_OK:
            free_state_flag = g_variant_get_uint32((GVariant *)g_slist_nth_data(output_list, 0));
            g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
            debug_log(DLOG_DEBUG, "%s: free_state_flag = %d", __FUNCTION__, free_state_flag);
            break;

            
        case VOS_ERR_NOTSUPPORT:
            result_val = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_CONFIG_IP_FAILED, NULL, &message_jso, RF_MSGDIS_BMA_NOT_PRESENT);
            goto error_exit;

        default:
            debug_log(DLOG_DEBUG, "%s:uip_call_class_method_redfish failed,method=%s,ret=%d", __FUNCTION__,
                BUSY_ETH_METHOD_CREATE_VLAN, ret);
            result_val = HTTP_INTERNAL_SERVER_ERROR;
            goto error_exit;
    }
    (void)redfish_get_sys_eth_memberid(eth_obj, memberid, sizeof(memberid));
    iRet = snprintf_s(oration_name, sizeof(oration_name), sizeof(oration_name) - 1, "%s VLAN Creation", memberid);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    create_vlan_info = task_info_new_function(eth_obj, free_state_flag, BUSY_ETH_ATTRIBUTE_CONFIG_STATE,
        BUSY_ETH_ATTRIBUTE_CONFIG_RESULT, oration_name, i_param->uri, HTTP_CREATE);
    if (create_vlan_info == NULL) {
        debug_log(DLOG_DEBUG, "%s: task_info_new_function failed", __FUNCTION__);
        result_val = HTTP_INTERNAL_SERVER_ERROR;
        goto error_exit;
    }

    create_vlan_info->rsc_privilege = USERROLE_BASICSETTING;
    ret = create_new_task(oration_name, ipinfo_config_status_monitor_fn, create_vlan_info, body_jso_checked,
        i_param->uri, &i_param->val_jso);
    if (ret != RF_OK) {
        debug_log(DLOG_DEBUG, "%s: create_new_task failed", __FUNCTION__);
        task_monitor_info_free(create_vlan_info);
        result_val = HTTP_INTERNAL_SERVER_ERROR;
        goto error_exit;
    }
    result_val = VOS_OK;

error_exit:

    if (result_val == HTTP_INTERNAL_SERVER_ERROR && message_array_jso != NULL) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
    }

    if (message_jso != NULL) {
        (void)json_object_array_add(message_array_jso, message_jso);
    }

    return result_val;
}


gint32 delete_vlan(PROVIDER_PARAS_S *i_param, json_object *o_message_array_jso)
{
    int iRet;
    gint32 ret;
    OBJ_HANDLE eth_obj = 0;
    gint32 result_val = 0;
    json_object *message_jso = NULL;
    guint32 vlan_id = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    guint32 free_state_flag = 0;
    gchar memberid[PROP_VAL_LENGTH] = {0};
    gchar oration_name[PROP_VAL_LENGTH] = {0};
    TASK_MONITOR_INFO_S *delete_vlan_info = NULL;

    // URL示例:/redfish/v1/Systems/1/EthernetInterfaces/mainboardNIC1Port1/VLANs/10
    if (i_param == NULL || o_message_array_jso == NULL) {
        debug_log(DLOG_DEBUG, "%s: invalid input param", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    // 只检查systems id和eth id,vlan id需要类方法校验
    ret = check_vlans_uri(i_param->uri, &eth_obj, TRUE);
    if (ret != RET_OK) {
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_jso, i_param->uri);
        result_val = HTTP_NOT_FOUND;
        goto error_exit;
    }

    
    ret = vos_str2int(i_param->member_id, 10, &vlan_id, NUM_TPYE_UINT32);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s: member id(%s) is not uint32 number", __FUNCTION__, i_param->member_id);
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_jso, i_param->uri);
        result_val = HTTP_NOT_FOUND;
        goto error_exit;
    }

    
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_uint32(vlan_id));
    ret = uip_call_class_method_redfish(i_param->is_from_webui, i_param->user_name, i_param->client, eth_obj,
        ETH_CLASS_NAME_BUSY_ETH, BUSY_ETH_METHOD_DELETE_VLAN, AUTH_ENABLE, i_param->user_role_privilege, input_list,
        &output_list);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    

    switch (ret) {
        case VOS_OK:
            free_state_flag = g_variant_get_uint32((GVariant *)g_slist_nth_data(output_list, 0));
            g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
            debug_log(DLOG_DEBUG, "%s: free_state_flag = %d", __FUNCTION__, free_state_flag);
            break;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            result_val = HTTP_FORBIDDEN;
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, &message_jso);
            goto error_exit;

        
        case VOS_ERR_NOTSUPPORT:
            result_val = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_CONFIG_IP_FAILED, NULL, &message_jso, RF_MSGDIS_BMA_NOT_PRESENT);
            goto error_exit;

        default:
            debug_log(DLOG_DEBUG, "%s:uip_call_class_method_redfish failed,method=%s,ret=%d", __FUNCTION__,
                BUSY_ETH_METHOD_DELETE_VLAN, ret);
            result_val = HTTP_INTERNAL_SERVER_ERROR;
            goto error_exit;
    }
    (void)redfish_get_sys_eth_memberid(eth_obj, memberid, sizeof(memberid));
    iRet = snprintf_s(oration_name, sizeof(oration_name), sizeof(oration_name) - 1, "%s VLAN(%u) Deletion", memberid,
        vlan_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    delete_vlan_info = task_info_new_function(eth_obj, free_state_flag, BUSY_ETH_ATTRIBUTE_CONFIG_STATE,
        BUSY_ETH_ATTRIBUTE_CONFIG_RESULT, oration_name, i_param->uri, HTTP_DELETE);
    if (delete_vlan_info == NULL) {
        debug_log(DLOG_DEBUG, "%s: task_info_new_function failed", __FUNCTION__);
        result_val = HTTP_INTERNAL_SERVER_ERROR;
        goto error_exit;
    }

    delete_vlan_info->rsc_privilege = USERROLE_BASICSETTING;
    ret = create_new_task(oration_name, ipinfo_config_status_monitor_fn, delete_vlan_info, NULL, i_param->uri,
        &i_param->val_jso);
    if (ret != RF_OK) {
        debug_log(DLOG_DEBUG, "%s: create_new_task failed", __FUNCTION__);
        task_monitor_info_free(delete_vlan_info);
        result_val = HTTP_INTERNAL_SERVER_ERROR;
        goto error_exit;
    }
    result_val = VOS_OK;
error_exit:

    if (result_val == HTTP_INTERNAL_SERVER_ERROR && o_message_array_jso != NULL) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
    }

    if (message_jso != NULL) {
        (void)json_object_array_add(o_message_array_jso, message_jso);
    }

    return result_val;
}
