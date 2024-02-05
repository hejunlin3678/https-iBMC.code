
#include "redfish_provider.h"
#include <arpa/inet.h>

LOCAL gint32 redfish_check_manager_eth_uri_effective(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE *obj_handle);
LOCAL gint32 get_manager_ethernetinterface_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_manager_ethernetinterface_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_manager_ethernetinterface_macaddr(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_manager_fqdn(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_manager_ethernetinterface_VLAN_info(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_manager_ethernetinterface_ipv4_addresses(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_manager_ethernetinterface_ipv6_defaultgateway(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_manager_ethernetinterface_ipv6_addresses(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_manager_ethernetinterface_ipv6_static_addresses(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_manager_ethernetinterface_nameserver(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_manager_ethernetinterface_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_manager_bmc_host_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_manager_fqdn(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 set_manager_ethernetinterface_VLAN_info(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_manager_ethernetinterface_ipv4_addresses(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_manager_ethernetinterface_ipv6_addresses(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_manager_ethernetinterface_nameserver(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_manager_ethernetinterface_ipv6_defaultgateway(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_manager_ethernetinterface_ipv6_static_addresses(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_manager_ethernetinterface_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_manager_ethernetinterface_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_manager_ethernetinterface_link(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_manager_ethernetinterface_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 redfish_smm_set_ip_mask_gateway(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    OBJ_HANDLE obj_handle);
LOCAL void get_adaptive_info_jso(OBJ_HANDLE obj_handle, guchar port_number, json_object *netport_array);
LOCAL PROPERTY_PROVIDER_S g_manager_eth_provider[] = {
    {RFPROP_MANAGER_ETH_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_ethernetinterface_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_ETH_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_ethernetinterface_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_ETH_MACADDR, ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_ATTRIBUTE_MAC, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_ethernetinterface_macaddr, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_BMC_HOSTNAME, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_manager_bmc_hostname, set_manager_bmc_host_name, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_BMC_FQDN, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_manager_fqdn, set_manager_fqdn, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_ETH_VLAN, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_manager_ethernetinterface_VLAN_info, set_manager_ethernetinterface_VLAN_info, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_ETH_IPV4ADDR, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_manager_ethernetinterface_ipv4_addresses, set_manager_ethernetinterface_ipv4_addresses, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_ETH_IPV6_GATEWAY, ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_ATTRIBUTE_IPV6_GATEWAY, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_manager_ethernetinterface_ipv6_defaultgateway, set_manager_ethernetinterface_ipv6_defaultgateway, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_ETH_IPV6_ADDR, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_manager_ethernetinterface_ipv6_addresses, set_manager_ethernetinterface_ipv6_addresses, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_ETH_IPV6_STATIC_ADDR, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_manager_ethernetinterface_ipv6_static_addresses, set_manager_ethernetinterface_ipv6_static_addresses, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_ETH_NAMESERVER, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_manager_ethernetinterface_nameserver, set_manager_ethernetinterface_nameserver, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_ethernetinterface_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_OEM, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_manager_ethernetinterface_oem, set_manager_ethernetinterface_oem, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_ETH_LINK, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_ethernetinterface_link, NULL, NULL, ETAG_FLAG_ENABLE},
};


LOCAL gint32 get_manager_ethernetinterface_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
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

    iRet = snprintf_s(odata_context, sizeof(odata_context), sizeof(odata_context) - 1, MANAGERS_ETH_METADATA, slot_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    *o_result_jso = json_object_new_string(odata_context);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 redfish_check_manager_eth_uri_effective(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE *obj_handle)
{
    int iRet = -1;
    gint32 ret;
    gint32 match_flag = 1;
    gchar mac_addr[MACADDRESS_LEN + 1] = {0};
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar slot[MAX_RSC_ID_LEN] = {0};
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guint8 out_type = 0;

    
    if ((VOS_OK != provider_paras_check(i_paras)) || (NULL == obj_handle)) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    
    ret = dfl_get_object_list(ETH_CLASS_NAME_ETHGROUP, &obj_list);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get obj_list fail.\n", __FUNCTION__, __LINE__));

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, ETH_GROUP_ATTRIBUTE_OUT_TYPE, &out_type);

        continue_if_expr((out_type != OUTTER_GROUP_TYPE) && (out_type != FLOATIP_ETHERNET));

        (void)memset_s(mac_addr, sizeof(mac_addr), 0, sizeof(mac_addr));
        
        get_ethernetinterface_id((OBJ_HANDLE)obj_node->data, out_type, mac_addr, sizeof(mac_addr));
        do_if_expr(0 == strlen(mac_addr), continue);

        iRet = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, "/redfish/v1/Managers/%s/EthernetInterfaces/%s", slot,
            mac_addr);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

        match_flag = g_ascii_strcasecmp(i_paras->uri, uri);
        if (RF_MATCH_OK == match_flag) {
            *obj_handle = (OBJ_HANDLE)obj_node->data;
            g_slist_free(obj_list);
            return VOS_OK;
        }
    }

    g_slist_free(obj_list);
    return HTTP_NOT_FOUND;
}

LOCAL gint32 get_manager_ethernetinterface_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;
    gchar *id_lower = NULL;
    gint32 ret;
    gchar slot[MAX_RSC_ID_LEN] = {0};
    gchar uri[MAX_URI_LENGTH] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    
    id_lower = g_ascii_strdown(i_paras->member_id, strlen(i_paras->member_id));
    return_val_if_expr(NULL == id_lower, HTTP_INTERNAL_SERVER_ERROR);
    iRet = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, RFPROP_MANAGER_ETH, slot, id_lower);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    g_free(id_lower);
    *o_result_jso = json_object_new_string(uri);
    

    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail ", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 get_manager_ethernetinterface_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar *id_lower = NULL;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    
    id_lower = g_ascii_strdown(i_paras->member_id, strlen(i_paras->member_id));
    return_val_if_expr(NULL == id_lower, HTTP_INTERNAL_SERVER_ERROR);
    

    *o_result_jso = json_object_new_string(id_lower);
    g_free(id_lower);

    return HTTP_OK;
}

LOCAL gint32 get_manager_ethernetinterface_macaddr(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar macaddr[MACADDRESS_LEN + 1] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    (void)dal_get_property_value_string(i_paras->obj_handle, ETH_GROUP_ATTRIBUTE_MAC, macaddr, MACADDRESS_LEN + 1);

    *o_result_jso = json_object_new_string(macaddr);

    return HTTP_OK;
}

gint32 get_manager_bmc_hostname(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gchar hostname[HOST_NAME_MAX_LEN + 1] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s,%d NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    
    (void)dal_get_object_handle_nth(BMC_CLASEE_NAME, 0, &obj_handle);
    
    (void)dal_get_property_value_string(obj_handle, BMC_HOST_NAME, hostname, sizeof(hostname));
    *o_result_jso = json_object_new_string(hostname);
    return HTTP_OK;
}


LOCAL gint32 get_manager_fqdn(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    int iRet;
    OBJ_HANDLE obj_handle_bmc = 0;
    OBJ_HANDLE obj_handle_dns = 0;
    gchar hostname[HOST_NAME_MAX_LEN + 1] = {0};
    gchar domainname[DNS_DOMAINNAME_MAX_LEN + 1] = {0};
    gchar fqdn[DNS_DOMAINNAME_MAX_LEN + HOST_NAME_MAX_LEN + 2] = {0};

    
    if (VOS_OK != provider_paras_check(i_paras) || NULL == o_message_jso || NULL == o_result_jso) {
        debug_log(DLOG_ERROR, "%s %d.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    (void)dal_get_object_handle_nth(BMC_CLASEE_NAME, 0, &obj_handle_bmc);
    (void)dal_get_object_handle_nth(CLASS_NAME_DNSSETTING, 0, &obj_handle_dns);
    
    (void)dal_get_property_value_string(obj_handle_bmc, BMC_HOST_NAME, hostname, sizeof(hostname));
    (void)dal_get_property_value_string(obj_handle_dns, PROPERTY_DNSSETTING_DOMAINNAME, domainname, sizeof(domainname));

    iRet = snprintf_s(fqdn, sizeof(fqdn), sizeof(fqdn) - 1, "%s.%s", hostname, domainname);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    *o_result_jso = json_object_new_string(fqdn);
    return HTTP_OK;
}


LOCAL gint32 get_manager_ethernetinterface_VLAN_info(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    guint32 vlan_state = 0;
    guint16 vlan_id = 0;
    guint8 net_mode_flag = 0;
    guint8 software_type = 0;

    if (VOS_OK != provider_paras_check(i_paras) || NULL == o_message_jso || NULL == o_result_jso) {
        debug_log(DLOG_ERROR, "%s %d", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    (void)dal_get_software_type(&software_type);
     
    if ((MGMT_SOFTWARE_TYPE_EM == software_type) &&
        (!dal_is_support_eth_function_specific_propery(PROPERTY_VLAN_STATUS, 0))) {
        return HTTP_INTERNAL_SERVER_ERROR; // 使返回值为null
    }


    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    obj_handle = i_paras->obj_handle;

    if (!dal_is_support_eth_function_specific_propery(PROPERTY_VLAN_STATUS, 0)) {
        (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_NET_MODE_FLAG, &net_mode_flag);
        
        return_val_if_expr(1 != net_mode_flag, HTTP_INTERNAL_SERVER_ERROR);
    }
    
    (void)dal_get_property_value_uint32(obj_handle, ETH_GROUP_ATTRIBUTE_VLAN_STATE, &vlan_state);
    (void)dal_get_property_value_uint16(obj_handle, ETH_GROUP_ATTRIBUTE_VLAN_ID, &vlan_id);
    
    *o_result_jso = json_object_new_object();
    json_object_object_add(*o_result_jso, RFPROP_MANAGER_ETH_VLAN_STATE, json_object_new_boolean(vlan_state));

    if (FALSE == vlan_state) {
        
        json_object_object_add(*o_result_jso, RFPROP_MANAGER_ETH_VLAN_ID, json_object_new_int(0));
    } else {
        
        json_object_object_add(*o_result_jso, RFPROP_MANAGER_ETH_VLAN_ID, json_object_new_int(vlan_id));
    }

    return HTTP_OK;
}

LOCAL gint32 get_manager_ethernetinterface_ipv4_addresses(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gchar address[IPV4_IP_STR_SIZE + 1] = {0};
    gchar subnetmask[IPV4_MASK_STR_SIZE + 1] = {0};
    guint8 origin = 0;
    gchar gateway[IPV4_GATEWAY_STR_SIZE + 1] = {0};
    json_object *obj_json_ipv4 = NULL;
    gchar *ipmode = NULL;
    gint32 ret = 0;
    guint8 software_type = 0;

    
    if (VOS_OK != provider_paras_check(i_paras) || NULL == o_message_jso || NULL == o_result_jso) {
        debug_log(DLOG_ERROR, "%s%d", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    obj_handle = i_paras->obj_handle;

    
    (void)dal_get_property_value_string(obj_handle, ETH_GROUP_ATTRIBUTE_IP_ADDR, address, sizeof(address));
    (void)dal_get_property_value_string(obj_handle, ETH_GROUP_ATTRIBUTE_SUB_MASK, subnetmask, sizeof(subnetmask));
    (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_IP_MODE, &origin);
    (void)dal_get_property_value_string(obj_handle, ETH_GROUP_ATTRIBUTE_GATEWAY, gateway, sizeof(gateway));
    (void)dal_get_software_type(&software_type);
    
    if (MGMT_SOFTWARE_TYPE_EM ==
        software_type) { // 管理板只有一个网关，存储在ethgroup0上 //ip地址为0.0.0.0时，显示为null
        (void)memset_s(gateway, sizeof(gateway), 0, sizeof(gateway));
        ret = get_manager_defaultip_group(&obj_handle);
        if (ret != VOS_OK) {
            debug_log(DLOG_ERROR, "[%s] Get object[%s] handle failed, err code %d", __func__, OUT_ETHGROUP_OBJ_NAME,
                ret);
            return HTTP_INTERNAL_SERVER_ERROR;
        } else {
            (void)dal_get_property_value_string(obj_handle, ETH_GROUP_ATTRIBUTE_GATEWAY, gateway, sizeof(gateway));
        }

        if (g_ascii_strcasecmp(address, RFPROP_INVALID_IP_ADDR) == 0) {
            (void)memset_s(address, sizeof(address), 0, sizeof(address));
        }
        if (g_ascii_strcasecmp(subnetmask, RFPROP_INVALID_IP_ADDR) == 0) {
            (void)memset_s(subnetmask, sizeof(subnetmask), 0, sizeof(subnetmask));
        }
    }

    
    if (IP_STATIC_MODE == origin) {
        ipmode = IP_MODE_STATIC_STRING;
    } else if (IP_DHCP_MODE == origin) {
        ipmode = IP_MODE_DHCP_STRING;
    }

    
    *o_result_jso = json_object_new_array();
    obj_json_ipv4 = json_object_new_object();
    json_object_object_add(obj_json_ipv4, RFPROP_MANAGER_ETH_IPADDR,
        0 == strlen(address) ? NULL : json_object_new_string(address));
    json_object_object_add(obj_json_ipv4, RFPROP_MANAGER_ETH_IPMASK,
        0 == strlen(subnetmask) ? NULL : json_object_new_string(subnetmask));
    json_object_object_add(obj_json_ipv4, RFPROP_MANAGER_ETH_IPGATEWAY,
        0 == strlen(gateway) ? NULL : json_object_new_string(gateway));
    json_object_object_add(obj_json_ipv4, RFPROP_MANAGER_ETH_IPMODE, json_object_new_string(ipmode));
    json_object_array_add(*o_result_jso, obj_json_ipv4);

    return HTTP_OK;
}


LOCAL gint32 get_manager_ethernetinterface_ipv6_defaultgateway(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gchar ipv6_gateway[IPV6_GATEWAY_STR_SIZE + 1] = {0};
    gint32 ret = 0;
    guint8 software_type = 0;

    
    if (VOS_OK != provider_paras_check(i_paras) || NULL == o_message_jso || NULL == o_result_jso) {
        debug_log(DLOG_ERROR, "%s%d", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    (void)dal_get_software_type(&software_type);
    
    
    if (MGMT_SOFTWARE_TYPE_EM == software_type) {
        ret = get_manager_defaultip_group(&obj_handle);
        if (VOS_OK != ret) {
            debug_log(DLOG_ERROR, "[%s] Get object[%s] failed, err code %d", __func__, OUT_ETHGROUP_OBJ_NAME, ret);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
    } else {
        obj_handle = i_paras->obj_handle;
    }
    

    
    (void)dal_get_property_value_string(obj_handle, ETH_GROUP_ATTRIBUTE_IPV6_GATEWAY, ipv6_gateway,
        sizeof(ipv6_gateway));

    if (0 == strlen(ipv6_gateway)) {
        return HTTP_INTERNAL_SERVER_ERROR;
    } else {
        *o_result_jso = json_object_new_string(ipv6_gateway);
    }

    return HTTP_OK;
}


LOCAL gint32 get_manager_ethernetinterface_ipv6_addresses(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    GVariant *property_value_addr = NULL;
    GVariant *property_value_prefix = NULL;
    gsize len_addr = 0;
    gsize len_prefix = 0;
    const gchar **ip_addr = NULL;
    const guint8 *prefix = NULL;

    gchar address[IPV6_IP_STR_SIZE + 1] = {0};
    guint8 origin = 0;
    guint8 prefixlen = 0;
    gsize i;

    if (VOS_OK != provider_paras_check(i_paras) || NULL == o_message_jso || NULL == o_result_jso) {
        debug_log(DLOG_ERROR, "%s%d.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    
    OBJ_HANDLE obj_handle = i_paras->obj_handle;

    
    (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_IPV6_MODE, &origin);
    *o_result_jso = json_object_new_array();
    
    (void)dal_get_property_value_string(obj_handle, ETH_GROUP_ATTRIBUTE_IPV6_ADDR, address, sizeof(address));
    (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_IPV6_PREFIX, &prefixlen);

    json_object *obj_json = json_object_new_object();
    json_object_object_add(obj_json, RFPROP_MANAGER_ETH_IPADDR,
        0 == strlen(address) ? NULL : json_object_new_string(address));
    json_object_object_add(obj_json, RFPROP_MANAGER_ETH_IPV6_PREFIXLEN,
        (0 == prefixlen) ? NULL : json_object_new_int(prefixlen));
    json_object_object_add(obj_json, RFPROP_MANAGER_ETH_IPMODE,
        (IP_DHCP_MODE == origin) ? json_object_new_string(IPV6_MODE_DHCP) :
                                   json_object_new_string(IP_MODE_STATIC_STRING));
    json_object_array_add(*o_result_jso, obj_json);
    obj_json = NULL;
    
    (void)dal_get_property_value_string(obj_handle, ETH_GROUP_ATTRIBUTE_IPV6_LOCAL_LINK, address, sizeof(address));
    obj_json = json_object_new_object();
    json_object_object_add(obj_json, RFPROP_MANAGER_ETH_IPADDR, json_object_new_string(address));
    json_object_object_add(obj_json, RFPROP_MANAGER_ETH_IPV6_PREFIXLEN, json_object_new_int(DEFAULT_IPV6_PREFIX_LEN));
    json_object_object_add(obj_json, RFPROP_MANAGER_ETH_IPMODE, json_object_new_string(IPV6_MODE_LINKLOCAL));
    json_object_array_add(*o_result_jso, obj_json);
    
    gint32 ret = dfl_get_property_value(obj_handle, ETH_GROUP_ATTRIBUTE_SLAAC_IPV6_ADDR, &property_value_addr);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get o_obj_handle fail.\n", __FUNCTION__, __LINE__));
    return_val_do_info_if_fail(NULL != property_value_addr, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property_data NULL pointer.\n", __FUNCTION__, __LINE__));
    ip_addr = g_variant_get_strv(property_value_addr, &len_addr);
    return_val_do_info_if_fail(ip_addr != NULL, HTTP_INTERNAL_SERVER_ERROR, g_variant_unref(property_value_addr);
        debug_log(DLOG_DEBUG, "[%s:%d]  slaac_addr NULL \n", __FUNCTION__, __LINE__));
    
    ret = dfl_get_property_value(obj_handle, ETH_GROUP_ATTRIBUTE_SLAAC_IPV6_PREFIX, &property_value_prefix);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR, g_free(ip_addr);
        ip_addr = NULL;
        g_variant_unref(property_value_addr);
        debug_log(DLOG_ERROR, "%s, %d: get o_obj_handle fail.\n", __FUNCTION__, __LINE__));
    return_val_do_info_if_fail(property_value_prefix != NULL, HTTP_INTERNAL_SERVER_ERROR, g_free(ip_addr);
        ip_addr = NULL;
        g_variant_unref(property_value_addr);
        debug_log(DLOG_ERROR, "%s, %d: get property_data NULL pointer.\n", __FUNCTION__, __LINE__));
    prefix = (const guint8 *)g_variant_get_fixed_array(property_value_prefix, &len_prefix, sizeof(guint8));
    return_val_do_info_if_fail(NULL != prefix, HTTP_INTERNAL_SERVER_ERROR, g_free(ip_addr);
        ip_addr = NULL;
        g_variant_unref(property_value_prefix); g_variant_unref(property_value_addr);
        debug_log(DLOG_DEBUG, "[%s:%d]  slaac prefix length NULL \n", __FUNCTION__, __LINE__));

    // 加入json对象
    for (i = 0; i < len_addr; i++) {
        
        continue_if_expr_true(0 == strlen(ip_addr[i]));
        obj_json = json_object_new_object();
        
        json_object_object_add(obj_json, RFPROP_MANAGER_ETH_IPADDR, json_object_new_string(ip_addr[i]));
        json_object_object_add(obj_json, RFPROP_MANAGER_ETH_IPMODE, json_object_new_string(IPV6_MODE_SLAAC));
        json_object_object_add(obj_json, RFPROP_MANAGER_ETH_IPV6_PREFIXLEN, json_object_new_int(prefix[i]));
        json_object_array_add(*o_result_jso, obj_json);
    }

    
    g_free(ip_addr);
    
    g_variant_unref(property_value_addr);
    g_variant_unref(property_value_prefix);
    return HTTP_OK;
}

LOCAL gint32 get_manager_ethernetinterface_ipv6_static_addresses(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    json_object *obj_json_ipv6 = NULL;
    guchar prefix = 0;
    guchar ipmode = 0;
    gchar address[IPV6_IP_STR_SIZE + 1] = {0};

    if (VOS_OK != provider_paras_check(i_paras) || NULL == o_message_jso || NULL == o_result_jso) {
        debug_log(DLOG_ERROR, "%s %d.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    
    obj_handle = i_paras->obj_handle;

    
    (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_IPV6_MODE, &ipmode);
    *o_result_jso = json_object_new_array();
    obj_json_ipv6 = json_object_new_object();

    
    if (IP_STATIC_MODE == ipmode) {
        (void)dal_get_property_value_string(obj_handle, ETH_GROUP_ATTRIBUTE_IPV6_ADDR, address, sizeof(address));
        (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_IPV6_PREFIX, &prefix);
        json_object_object_add(obj_json_ipv6, RFPROP_MANAGER_ETH_IPADDR,
            0 == strlen(address) ? NULL : json_object_new_string(address));
        json_object_object_add(obj_json_ipv6, RFPROP_MANAGER_ETH_IPV6_PREFIXLEN,
            0 == prefix ? NULL : json_object_new_int(prefix));
        json_object_array_add(*o_result_jso, obj_json_ipv6);
    } else {
        json_object_object_add(obj_json_ipv6, RFPROP_MANAGER_ETH_IPADDR, NULL);
        json_object_object_add(obj_json_ipv6, RFPROP_MANAGER_ETH_IPV6_PREFIXLEN, NULL);
        json_object_array_add(*o_result_jso, obj_json_ipv6);
    }

    return HTTP_OK;
}


LOCAL gint32 get_manager_ethernetinterface_nameserver(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gchar main_domain[DNS_DOMAINSERVER_MAX_LEN] = {0};
    gchar backup_domain[DNS_DOMAINSERVER_MAX_LEN] = {0};
    gchar tertiary_domain[DNS_DOMAINSERVER_MAX_LEN] = {0};

    if ((provider_paras_check(i_paras) != VOS_OK) || (o_message_jso == NULL) || (o_result_jso == NULL)) {
        debug_log(DLOG_ERROR, "%s %d.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    (void)dal_get_object_handle_nth(CLASS_NAME_DNSSETTING, 0, &obj_handle);
    
    (void)dal_get_property_value_string(obj_handle, PROPERTY_DNSSETTING_PRIMARYDOMAIN, main_domain,
        sizeof(main_domain));
    (void)dal_get_property_value_string(obj_handle, PROPERTY_DNSSETTING_BACKUPDOMAIN, backup_domain,
        sizeof(backup_domain));
    (void)dal_get_property_value_string(obj_handle, PROPERTY_DNSSETTING_TERTIARYDOMAIN, tertiary_domain,
        sizeof(tertiary_domain));
    
    *o_result_jso = json_object_new_array();
    json_object_array_add(*o_result_jso, json_object_new_string(main_domain));
    json_object_array_add(*o_result_jso, json_object_new_string(backup_domain));
    json_object_array_add(*o_result_jso, json_object_new_string(tertiary_domain));

    return HTTP_OK;
}


void redfish_get_ip_version(OBJ_HANDLE obj_handle, json_object **huawei)
{
    guint8 ip_ver = 0;
    const gchar *ip_ver_str = NULL;
    json_object *version = NULL;

    return_do_info_if_expr(NULL == huawei, debug_log(DLOG_ERROR, "%s : NULL pointer", __FUNCTION__));

    (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_IP_VERSION, &ip_ver);

    switch (ip_ver) {
        case ENABLE_IPV4_ONLY:
            ip_ver_str = IP_VER_IPV4;
            break;

        case ENABLE_IPV6_ONLY:
            ip_ver_str = IP_VER_IPV6;
            break;

        case ENABLE_IPV4_AND_IPV6:
            ip_ver_str = IP_VER_IPV4_ADN_IPV6;
            break;

        default:
            
            debug_log(DLOG_DEBUG, "%s : invalid ip version value is %d", __FUNCTION__, ip_ver);
            
    }

    return_do_info_if_expr(NULL == ip_ver_str, json_object_object_add(*huawei, RFPROP_MANAGER_ETH_OEM_IPVER, version));

    version = json_object_new_string(ip_ver_str);
    do_info_if_true(NULL == version, debug_log(DLOG_ERROR, "%s : json_object_new_string fail", __FUNCTION__));

    json_object_object_add(*huawei, RFPROP_MANAGER_ETH_OEM_IPVER, version);

    return;
}


LOCAL void redfish_get_net_mode(OBJ_HANDLE obj_handle, json_object **huawei)
{
    guint8 net_mode = 0;
    const gchar *net_mode_str = NULL;
    json_object *net_mode_jso = NULL;
    guint8 netmode_flag = 0;
    guint8 ret;
    guchar board_type = 0;

    return_do_info_if_expr(NULL == huawei, debug_log(DLOG_ERROR, "%s : NULL pointer", __FUNCTION__));

    
    ret = redfish_get_board_type(&board_type);
    return_do_info_if_fail(VOS_OK == ret,
        json_object_object_add(*huawei, RFPROP_MANAGER_ETH_OEM_NET_MODE, net_mode_jso);
        debug_log(DLOG_ERROR, "%s, %d: get board_type fail.\n", __FUNCTION__, __LINE__));
    return_do_info_if_fail(BOARD_SWITCH != board_type,
        json_object_object_add(*huawei, RFPROP_MANAGER_ETH_OEM_NET_MODE, net_mode_jso));
    

    
    (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_NET_MODE_FLAG, &netmode_flag);

    if (RF_UNSUPPORT_ADAPTIVE_NET_MODE == netmode_flag) {
        debug_log(DLOG_MASS, "%s : not support adaptive", __FUNCTION__);
        json_object_object_add(*huawei, RFPROP_MANAGER_ETH_OEM_NET_MODE, net_mode_jso);
        return;
    }

    (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_NET_MODE, &net_mode);

    switch (net_mode) {
        case NET_MODE_MANUAL:
            net_mode_str = NET_MODE_STRING_FIXED;
            break;

        case NET_MODE_AUTO:
            net_mode_str = NET_MODE_STRING_AUTO;
            break;

        default:
            
            debug_log(DLOG_DEBUG, "%s : invalid net mode value is %d", __FUNCTION__, net_mode);
            
    }

    return_do_info_if_expr(NULL == net_mode_str,
        json_object_object_add(*huawei, RFPROP_MANAGER_ETH_OEM_NET_MODE, net_mode_jso));

    net_mode_jso = json_object_new_string(net_mode_str);
    do_info_if_true(NULL == net_mode_jso, debug_log(DLOG_ERROR, "%s : json_object_new_string fail", __FUNCTION__));

    json_object_object_add(*huawei, RFPROP_MANAGER_ETH_OEM_NET_MODE, net_mode_jso);

    return;
}

LOCAL void redfish_get_ncsi_mode(OBJ_HANDLE obj_handle, json_object **huawei)
{
    guint8 ncsi_mode = 0;
    const gchar *ncsi_mode_str = NULL;
    json_object *ncsi_mode_jso = NULL;
    guint8 netmode_flag = 0;
    guint8 ret;
    guchar board_type = 0;

    return_do_info_if_expr(huawei == NULL, debug_log(DLOG_ERROR, "%s : NULL pointer", __FUNCTION__));

    ret = redfish_get_board_type(&board_type);
    return_do_info_if_fail(ret == VOS_OK,
        json_object_object_add(*huawei, RFPROP_MANAGER_ETH_OEM_NCSI_MODE, ncsi_mode_jso);
        debug_log(DLOG_ERROR, "%s, %d: get board_type fail.", __FUNCTION__, __LINE__));
    return_do_info_if_fail(board_type != BOARD_SWITCH,
        json_object_object_add(*huawei, RFPROP_MANAGER_ETH_OEM_NCSI_MODE, ncsi_mode_jso));

    
    (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_NET_MODE_FLAG, &netmode_flag);

    if (netmode_flag == RF_UNSUPPORT_ADAPTIVE_NET_MODE) {
        debug_log(DLOG_MASS, "%s : not support adaptive", __FUNCTION__);
        json_object_object_add(*huawei, RFPROP_MANAGER_ETH_OEM_NCSI_MODE, ncsi_mode_jso);
        return;
    }

    (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_NCSI_MODE, &ncsi_mode);

    switch (ncsi_mode) {
        case RF_DISABLE:
            ncsi_mode_str = MANUAL_STRING;
            break;

        case RF_ENABLE:
            ncsi_mode_str = AUTO_STRING;
            break;

        default:
            debug_log(DLOG_DEBUG, "%s : invalid net mode value is %d", __FUNCTION__, ncsi_mode);
    }

    return_do_info_if_expr(ncsi_mode_str == NULL,
        json_object_object_add(*huawei, RFPROP_MANAGER_ETH_OEM_NCSI_MODE, ncsi_mode_jso));

    ncsi_mode_jso = json_object_new_string(ncsi_mode_str);
    do_info_if_true(ncsi_mode_jso == NULL, debug_log(DLOG_ERROR, "%s : json_object_new_string fail", __FUNCTION__));

    json_object_object_add(*huawei, RFPROP_MANAGER_ETH_OEM_NCSI_MODE, ncsi_mode_jso);

    return;
}


LOCAL gchar *net_type_int2str(guint8 type)
{
    gchar *type_str = NULL;

    switch (type) {
        case NET_TYPE_LOM:
            type_str = NET_TYPE_LOM_STR;
            break;

        case NET_TYPE_DEDICATED:
        case NET_TYPE_CABINET_VLAN:
            type_str = NET_TYPE_DEDICATED_STR;
            break;

        case NET_TYPE_PCIE:
            type_str = NET_TYPE_PCIE_STR;
            break;

        case NET_TYPE_AGGREGATION:
            type_str = NET_TYPE_AGGREGATION_STR;
            break;

        case NET_TYPE_LOM2:
            type_str = NET_TYPE_LOM2_STR;
            break;
        case NET_TYPE_OCP_CARD:
            type_str = NET_TYPE_OCP_STR;
            break;
        case NET_TYPE_OCP2_CARD:
            type_str = NET_TYPE_OCP2_STR;
            break;

        default:
            
            debug_log(DLOG_DEBUG, "%s:  invalid netport type is %d", __FUNCTION__, type);
            
    }

    return type_str;
}

LOCAL gint32 get_bussiness_port_silk_num(OBJ_HANDLE eth_group_handle, guint8 *silk_num)
{
    gint32 ret;
    guint8 active_port = 0;
    guint8 ncsi_port = 0;
    guint8 eth_num = 0;
    guint8 port_num = 0;
    GSList *business_net = NULL;
    GSList *node = NULL;

    (void)dal_get_property_value_byte(eth_group_handle, ETH_GROUP_ATTRIBUTE_ACTIVE_PORT, &active_port);
    (void)dal_get_property_value_byte(eth_group_handle, ETH_GROUP_ATTRIBUTE_NCSI_PORT, &ncsi_port);

    ret = dfl_get_object_list(ETH_CLASS_NAME_BUSY_ETH, &business_net);
    return_val_do_info_if_expr((NULL == business_net) || (DFL_OK != ret), ret,
        debug_log(DLOG_ERROR, "%s: get BusinessPort obj list fail or null pointer", __FUNCTION__));

    for (node = business_net; node; node = g_slist_next(node)) {
        (void)dal_get_property_value_byte((OBJ_HANDLE)node->data, BUSY_ETH_ATTRIBUTE_ETH_NUM, &eth_num);
        (void)dal_get_property_value_byte((OBJ_HANDLE)node->data, BUSY_ETH_ATTRIBUTE_PORT_NUM, &port_num);
        if ((eth_num == active_port) && (port_num == ncsi_port)) {
            (void)dal_get_property_value_byte((OBJ_HANDLE)node->data, BUSY_ETH_ATTRIBUTE_SILK_NUM, silk_num);
            break;
        }
    }

    g_slist_free(business_net);

    
    return (node == NULL) ? VOS_ERR : VOS_OK;
}


LOCAL gint32 get_slik_num(OBJ_HANDLE obj_handle, guint8 *silk_num)
{
    gint32 ret;
    guint8 type = 0;
    OBJ_HANDLE eth_obj = 0;

    return_val_do_info_if_expr(silk_num == NULL, VOS_ERR, debug_log(DLOG_ERROR, "%s: NULL Pointer", __FUNCTION__));

    (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_TYPE, &type);
    if ((NET_TYPE_LOM != type) && (NET_TYPE_PCIE != type) && (NET_TYPE_LOM2 != type) && (NET_TYPE_OCP_CARD != type) &&
        (NET_TYPE_OCP2_CARD != type)) {
        ret = dal_get_specific_object_byte(CLASS_NAME_ETH_FUNCTION, PROPERTY_FUNCTION_TYPE,
            FN_TYPE_OUTTER_OM, &eth_obj);
        if (ret == RET_OK) {
            *silk_num = dal_eth_get_cur_plane_eth_num();
        } else {
            ret = dal_get_dedicate_or_aggregation_port_silk_num(obj_handle, silk_num);
        }
    } else {
        ret = get_bussiness_port_silk_num(obj_handle, silk_num);
    }

    return ret;
}

LOCAL void redfish_get_current_manager_netport(OBJ_HANDLE obj_handle, json_object **huawei)
{
    gint32 ret;
    json_object *manager_netport = NULL;
    json_object *type = NULL;
    json_object *silk_port = NULL;
    guint8 type_int = 0;
    gchar *type_str = NULL;
    guint8 slik_num = 0;
    guint8 netmode_flag = 0;
    guchar board_type = 0;

    return_do_info_if_expr(NULL == huawei, debug_log(DLOG_ERROR, "%s : NULL pointer", __FUNCTION__));

    
    ret = redfish_get_board_type(&board_type);
    return_do_info_if_fail(VOS_OK == ret,
        json_object_object_add(*huawei, RFPROP_MANAGER_ETH_OEM_MANAGER_PORT, manager_netport);
        debug_log(DLOG_ERROR, "%s, %d: get board_type fail.\n", __FUNCTION__, __LINE__));
    return_do_info_if_fail((BOARD_SWITCH != board_type),
        json_object_object_add(*huawei, RFPROP_MANAGER_ETH_OEM_MANAGER_PORT, manager_netport));
    

    
    (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_NET_MODE_FLAG, &netmode_flag);

    if (RF_UNSUPPORT_ADAPTIVE_NET_MODE == netmode_flag) {
        debug_log(DLOG_MASS, "%s : netport not support adaptive", __FUNCTION__);
        json_object_object_add(*huawei, RFPROP_MANAGER_ETH_OEM_MANAGER_PORT, manager_netport);
        return;
    }

    
    manager_netport = json_object_new_object();
    return_do_info_if_expr(NULL == manager_netport,
        debug_log(DLOG_ERROR, "%s : json_object_new_object fail", __FUNCTION__);
        json_object_object_add(*huawei, RFPROP_MANAGER_ETH_OEM_MANAGER_PORT, manager_netport));

    
    (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_TYPE, &type_int);
    type_str = net_type_int2str(type_int);

    type = json_object_new_string((const char *)type_str);
    do_if_expr(NULL == type, debug_log(DLOG_ERROR, "%s : json_object_new_string fail", __FUNCTION__));

    json_object_object_add(manager_netport, RFPROP_MANAGER_ETH_OEM_PORT_TYPE, type);

    
    ret = get_slik_num(obj_handle, &slik_num);
    
    do_if_fail(VOS_OK == ret, debug_log(DLOG_DEBUG, "%s : get slik port fail", __FUNCTION__));
    
    silk_port = json_object_new_int((int32_t)slik_num);
    do_if_expr(NULL == silk_port, debug_log(DLOG_ERROR, "%s : json_object_new_string fail", __FUNCTION__));

    json_object_object_add(manager_netport, RFPROP_MANAGER_ETH_OEM_PORT_SLIK, silk_port);

    json_object_object_add(*huawei, RFPROP_MANAGER_ETH_OEM_MANAGER_PORT, manager_netport);

    return;
}


LOCAL gint32 get_eth_obj_by_netcard_type(guint8 type, OBJ_HANDLE *obj_handle)
{
    gint32 result = VOS_ERR;
    gint32 ret;
    OBJ_HANDLE eth_obj = 0;
    GSList *eth_obj_list = NULL;
    GSList *node = NULL;
    guint8 eth_type = 0;

    return_val_do_info_if_expr(NULL == obj_handle, result, debug_log(DLOG_ERROR, "%s : NULL pointer", __FUNCTION__));

    ret = dfl_get_object_list(ETH_CLASS_NAME_ETH, &eth_obj_list);
    return_val_do_info_if_expr((VOS_OK != ret) || (NULL == eth_obj_list), result,
        debug_log(DLOG_ERROR, "%s: get eth obj list fail", __FUNCTION__));

    for (node = eth_obj_list; node; node = g_slist_next(node)) {
        eth_obj = (OBJ_HANDLE)node->data;

        (void)dal_get_property_value_byte(eth_obj, ETH_ATTRIBUTE_TYPE, &eth_type);

        if (type == eth_type) {
            *obj_handle = eth_obj;
            result = VOS_OK;
            break;
        }
    }

    g_slist_free(eth_obj_list);

    return result;
}


LOCAL gchar *bmc_link_status_int2str(guint8 link_status)
{
#define BMC_LINK_STATUS_CONNECTED "Connected"
#define BMC_LINK_STATUS_DISCONNECTED "Disconnected"

    gchar *link_status_str = NULL;

    
    switch (link_status) {
        case 0:
            link_status_str = BMC_LINK_STATUS_DISCONNECTED;
            break;

        case 1:
            link_status_str = BMC_LINK_STATUS_CONNECTED;
            break;

        default:
            
            debug_log(DLOG_DEBUG, "%s: invalid link status is %d", __FUNCTION__, link_status);
            
    }

    return link_status_str;
}

LOCAL void _get_netport_info_with_eth_handle(OBJ_HANDLE eth_obj, guchar port_num, json_object *info_array_jso)
{
    json_object *type_jso = NULL;
    json_object *port_number_jso = NULL;
    json_object *link_status_jso = NULL;
    json_object *info_item_jso = NULL;
    gint32 link_status_int = 0;
    gchar *link_status_str = NULL;
    gint32 ret;
    guchar eth_type = 0;
    gchar *type_str = NULL;

    (void)dal_get_property_value_byte(eth_obj, ETH_ATTRIBUTE_TYPE, &eth_type);

    info_item_jso = json_object_new_object();
    return_do_info_if_expr(NULL == info_item_jso,
        debug_log(DLOG_ERROR, "%s : json_object_new_object fail", __FUNCTION__));

    type_str = net_type_int2str(eth_type);
    type_jso = json_object_new_string((const char *)type_str);
    do_info_if_true(NULL == type_jso, debug_log(DLOG_ERROR, "%s : json_object_new_string fail", __FUNCTION__));

    
    (void)dal_get_property_value_int32(eth_obj, ETH_ATTRIBUTE_LINK_STATUS, &link_status_int);
    
    link_status_str = bmc_link_status_int2str((guint8)link_status_int);
    
    if (link_status_str != NULL) {
        link_status_jso = json_object_new_string((const char *)link_status_str);
        do_info_if_true(NULL == link_status_jso,
            debug_log(DLOG_ERROR, "%s : json_object_new_string fail", __FUNCTION__));
    }

    

    port_number_jso = json_object_new_int((gint32)port_num);
    do_info_if_true(NULL == port_number_jso, debug_log(DLOG_ERROR, "%s : json_object_new_int fail", __FUNCTION__));

    json_object_object_add(info_item_jso, RFPROP_MANAGER_ETH_OEM_PORT_TYPE, type_jso);
    json_object_object_add(info_item_jso, RFPROP_MANAGER_ETH_OEM_PORT_SLIK, port_number_jso); // 专用网口的port_number写1
    json_object_object_add(info_item_jso, RFPROP_MANAGER_ETH_OEM_LINK_STATUS, link_status_jso);

    ret = json_object_array_add(info_array_jso, info_item_jso);
    return_do_info_if_fail(VOS_OK == ret, (void)json_object_put(info_item_jso);
        debug_log(DLOG_ERROR, "%s : json_object_array_add fail", __FUNCTION__));

    return;
}


LOCAL void get_multi_dedicat_eth_info(json_object** netport_array, get_eth_info_func p_get_info_fun)
{
    GSList* eth_obj_list = NULL;
    gint32 ret = dfl_get_object_list(ETH_CLASS_NAME_ETH, &eth_obj_list);
    if ((ret != RET_OK) || (eth_obj_list == NULL)) {
        debug_log(DLOG_ERROR, "%s: get eth obj list fail", __FUNCTION__);
        return;
    }
    for (GSList* obj_node = eth_obj_list; obj_node; obj_node = g_slist_next(obj_node)) {
        OBJ_HANDLE eth_obj = (OBJ_HANDLE)obj_node->data;
        guint8 eth_type = 0;
        (void)dal_get_property_value_byte(eth_obj, ETH_ATTRIBUTE_TYPE, &eth_type);

        if (eth_type == NET_TYPE_DEDICATED) {
            guchar port_num = 1;
            (void)dal_get_property_value_byte(eth_obj, ETH_ATTRIBUTE_ETH_ID, &port_num);
            (*p_get_info_fun)(eth_obj, port_num, *netport_array);
        }
    }

    g_slist_free(eth_obj_list);
}

LOCAL void get_multi_dedicated_object_or_adaptive_info(guint8 adaptive_flag, json_object **netport_array)
{
    gint32 ret;
    guint8 out_group_id = dal_eth_get_out_type_groupid();
    guchar eth_type;
    guchar port_id;
    guchar out_type;
    guint8 group_id;
    GSList *eth_list = NULL;
    GSList *eth_list_iter = NULL;
    OBJ_HANDLE obj_handle_tmp;

    if (netport_array == NULL) {
        debug_log(DLOG_ERROR, "%s : NULL pointer", __FUNCTION__);
        return;
    }

    ret = dfl_get_object_list(ETH_CLASS_NAME, &eth_list);
    if (ret != DFL_OK) {
        return;
    }

    for (eth_list_iter = eth_list; eth_list_iter != NULL; eth_list_iter = eth_list_iter->next) {
        obj_handle_tmp = (OBJ_HANDLE)eth_list_iter->data;
        eth_type = 0;
        port_id = 0;
        out_type = 0;
        group_id = 0;

        (void)dal_get_property_value_byte(obj_handle_tmp, ETH_ATTRIBUTE_TYPE, &eth_type);
        (void)dal_get_property_value_byte(obj_handle_tmp, ETH_ATTRIBUTE_OUT_TYPE, &out_type);
        (void)dal_get_property_value_byte(obj_handle_tmp, ETH_ATTRIBUTE_GROUP_ID, &group_id);

        continue_if_expr((eth_type != NET_TYPE_DEDICATED) || (out_type != OUT_ETHERNET) || (out_group_id != group_id));

        
        (void)dal_get_property_value_byte(obj_handle_tmp, ETH_ATTRIBUTE_ETH_ID, &port_id);
        
        
        if (adaptive_flag == 1) {
            get_adaptive_info_jso(obj_handle_tmp, port_id, *netport_array);
        } else {
            
            _get_netport_info_with_eth_handle(obj_handle_tmp, port_id, *netport_array);
        }
    }

    return;
}


LOCAL void get_object_info(guint8 type_int, json_object **netport_array)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    guchar port_num = 1;
    OBJ_HANDLE tmp_handle = OBJ_HANDLE_COMMON;

    return_do_info_if_expr(NULL == netport_array, debug_log(DLOG_ERROR, "%s : NULL pointer", __FUNCTION__));

    if (type_int == NET_TYPE_DEDICATED && dal_check_ar_card_support()) {
        get_multi_dedicat_eth_info(netport_array, _get_netport_info_with_eth_handle);
        return;
    }

    
    ret = get_eth_obj_by_netcard_type((gint8)type_int, &obj_handle);
    
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_DEBUG, "%s : get eth object fail", __FUNCTION__));

    if (dal_get_specific_object_byte(ETH_CLASS_NAME_MGNTPORTCAP, PROPERTY_MNGTPORTCAP_PCIEMGNT_ENABLE, ENABLED,
        &tmp_handle) == VOS_OK) {
        (void)dal_get_property_value_byte(obj_handle, ETH_ATTRIBUTE_ETH_ID, &port_num);
    }

    _get_netport_info_with_eth_handle(obj_handle, port_num, *netport_array);

    return;
}

LOCAL void get_dedicated_object_info(json_object **netport_array)
{
    
    if (dal_is_support_eth_define_specific_propery(PROPERTY_SUPPORT_DETACH_ETHGROUP)) {
        get_multi_dedicated_object_or_adaptive_info(0, netport_array); // 0: 获取所有可用管理网口
        return;
    }

    get_object_info((guint8)NET_TYPE_DEDICATED, netport_array);
    return;
}


LOCAL gint32 verify_lom_pcie_lom2_supported(guint8 type)
{
    OBJ_HANDLE mgnport_handle = 0;
    guint8 pcie_ncsi_enable = 0;
    guint8 lom2_ncsi_enable = 0;
    guint8 ocp_ncsi_enable = 0;

    (void)dal_get_object_handle_nth(ETH_CLASS_NAME_MGNTPORTCAP, 0, &mgnport_handle);

    if (type == NET_TYPE_PCIE) {
        
        (void)dal_get_property_value_byte(mgnport_handle, PROPERTY_MNGTPORTCAP_PCIENCSIENABLE, &pcie_ncsi_enable);
        return_val_do_info_if_fail(pcie_ncsi_enable, VOS_ERR,
            debug_log(DLOG_MASS, "%s: pcis ncsi Cable not present", __FUNCTION__));
    }
    if (type == NET_TYPE_LOM2) {
        
        (void)dal_get_property_value_byte(mgnport_handle, PROPERTY_MNGTPORTCAP_LOM2NCSIENABLE, &lom2_ncsi_enable);
        
        return_val_do_info_if_fail(lom2_ncsi_enable, VOS_ERR,
            debug_log(DLOG_MASS, "%s: pcis flexIO Cable not present", __FUNCTION__));
    }
    if (type == NET_TYPE_OCP_CARD) {
        
        (void)dal_get_property_value_byte(mgnport_handle, PROPERTY_MNGTPORTCAP_OCPENABLE, &ocp_ncsi_enable);
        return_val_do_info_if_fail(ocp_ncsi_enable, VOS_ERR,
            debug_log(DLOG_MASS, "%s: ocp ncsi not support", __FUNCTION__));
    }
    if (type == NET_TYPE_OCP2_CARD) {
        
        (void)dal_get_property_value_byte(mgnport_handle, PROPERTY_MNGTPORTCAP_OCP2ENABLE, &ocp_ncsi_enable);
        if (ocp_ncsi_enable != 1) {
            debug_log(DLOG_MASS, "%s: ocp ncsi not support", __FUNCTION__);
            return RET_ERR;
        }
    }

    return VOS_OK;
}


LOCAL gint32 get_businessport_ncsi_supported_status(OBJ_HANDLE busy_eth_obj, guint8 *status)
{
    gint32 retv;
    OBJ_HANDLE netcard_handle = 0;

    if ((0 == busy_eth_obj) || (NULL == status)) {
        debug_log(DLOG_ERROR, "Input parameter error. \r\n");
        return VOS_ERR;
    }

    
    retv = dfl_get_referenced_object(busy_eth_obj, BUSY_ETH_ATTRIBUTE_REF_NETCARD, &netcard_handle);
    return_val_do_info_if_fail(VOS_OK == retv, VOS_ERR,
        debug_log(DLOG_ERROR, "Get port referenced object failed. \r\n"));

    retv = dal_get_property_value_byte(netcard_handle, PROPERTY_NETCARD_NCSI_SUPPORTED, status);
    return_val_do_info_if_fail(VOS_OK == retv, VOS_ERR,
        debug_log(DLOG_ERROR, "Get netcard property NcsiSupported value failed. \r\n"));

    return VOS_OK;
}


LOCAL void get_each_object_info(guint8 type_int, json_object **netport_array)
{
#define NCSI_SUPPORTED 1
    OBJ_HANDLE obj_handle = 0;
    guint8 port = 0;
    guint8 link_status_int = 0;
    gchar *link_status_str = NULL;
    json_object *type = NULL;
    json_object *port_number = NULL;
    json_object *link_status = NULL;
    json_object *object = NULL;
    GSList *node = NULL;
    gchar *type_str = NULL;
    gint32 ret;
    GSList *business_obj_list = NULL;
    guint8 card_type = 0;
    guint8 ncsi_state = 0;
    guint8 available_flag = 0;

    return_do_info_if_expr(NULL == netport_array, debug_log(DLOG_ERROR, "%s : NULL pointer", __FUNCTION__));

    ret = verify_lom_pcie_lom2_supported(type_int);
    return_if_expr(ret != VOS_OK);

    type_str = net_type_int2str(type_int);
    return_if_expr(NULL == type_str);

    ret = dfl_get_object_list(ETH_CLASS_NAME_BUSY_ETH, &business_obj_list);
    return_do_info_if_expr((DFL_OK != ret) || (NULL == business_obj_list),
        debug_log(DLOG_DEBUG, "%s: get BusinessPort object list fail", __FUNCTION__));

    for (node = business_obj_list; node; node = g_slist_next(node)) {
        obj_handle = (OBJ_HANDLE)node->data;

        
        (void)dal_get_property_value_byte(obj_handle, BUSY_ETH_ATTRIBUTE_CARD_TYPE, &card_type);
        continue_if_expr(card_type != type_int);

        ret = get_businessport_ncsi_supported_status(obj_handle, &ncsi_state);
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "eth_get_eth_link_status failed:obj_name is %s\n", dfl_get_object_name(obj_handle)));
        continue_if_expr(NCSI_SUPPORTED != ncsi_state);

        (void)dal_get_property_value_byte(obj_handle, BUSY_ETH_ATTRIBUTE_SILK_NUM, &port);
        (void)dal_get_property_value_byte(obj_handle, BUSY_ETH_ATTRIBUTE_LINK_STATUS, &link_status_int);
        link_status_str = bmc_link_status_int2str(link_status_int);

        object = json_object_new_object();
        continue_do_info_if_fail(NULL != object,
            debug_log(DLOG_ERROR, "%s : json_object_new_object fail", __FUNCTION__));

        
        if (NULL != link_status_str) {
            link_status = json_object_new_string((const char *)link_status_str);
            do_info_if_true(NULL == link_status,
                debug_log(DLOG_ERROR, "%s : json_object_new_string fail", __FUNCTION__));
        }

        

        
        (void)dal_get_property_value_byte(obj_handle, BUSY_ETH_ATTRIBUTE_AVAILABLEFLAG,
            &available_flag); // json_object_put接口会判空
        
        continue_do_info_if_expr(
            ((type_int == NET_TYPE_PCIE) || (type_int == NET_TYPE_OCP_CARD) || (type_int == NET_TYPE_OCP2_CARD)) &&
            (available_flag == AVAILABLE_FLAG_VIRTUAL),
            json_object_put(object);
            json_object_put(link_status); link_status = NULL);
        
        do_info_if_true(AVAILABLE_FLAG_DISABLED == available_flag, json_object_put(link_status);
            link_status = json_object_new_string("Disabled"));
        do_info_if_true(AVAILABLE_FLAG_DISABLED == available_flag, json_object_put(link_status);
            link_status = json_object_new_string("Disabled"));
        
        port_number = json_object_new_int((int32_t)port);
        do_info_if_true(NULL == port_number, debug_log(DLOG_ERROR, "%s : json_object_new_int fail", __FUNCTION__));

        type = json_object_new_string((const char *)type_str);
        do_info_if_true(NULL == type, debug_log(DLOG_ERROR, "%s : json_object_new_string fail", __FUNCTION__));

        json_object_object_add(object, RFPROP_MANAGER_ETH_OEM_PORT_TYPE, type);
        json_object_object_add(object, RFPROP_MANAGER_ETH_OEM_PORT_SLIK, port_number);
        json_object_object_add(object, RFPROP_MANAGER_ETH_OEM_LINK_STATUS, link_status);
        link_status = NULL;

        ret = json_object_array_add(*netport_array, object);
        do_info_if_true(0 != ret, debug_log(DLOG_ERROR, "%s : json_object_array_add fail", __FUNCTION__);
            (void)json_object_put(object));
    }

    g_slist_free(business_obj_list);

    return;
}


LOCAL gint32 verify_dedicated_exist(void)
{
    OBJ_HANDLE eth_obj = 0;
    guint8 out_type = 0;
    gint32 ret;

    
    ret = dal_get_specific_object_byte(CLASS_NAME_ETH_FUNCTION, PROPERTY_FUNCTION_TYPE, FN_TYPE_OUTTER_OM, &eth_obj);
    if (ret == RET_OK) {
        return RET_OK;
    }
    ret = get_eth_obj_by_netcard_type((guint8)NET_TYPE_DEDICATED, &eth_obj);
    
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, debug_log(DLOG_DEBUG, "%s : get eth obj fail", __FUNCTION__));
    

    
    (void)dal_get_property_value_byte(eth_obj, ETH_ATTRIBUTE_OUT_TYPE, &out_type);
    return_val_do_info_if_fail(OUT_ETHERNET == out_type, VOS_ERR,
        debug_log(DLOG_MASS, "%s : is not out type eth", __FUNCTION__));

    return VOS_OK;
}


LOCAL gint32 verify_aggregation_exist(void)
{
    OBJ_HANDLE eth_obj = 0;
    OBJ_HANDLE obj_handle = 0;
    guint8 out_type = 0;
    guint8 aggregation_enable = 0;
    guint8 ret;

    
    ret = get_eth_obj_by_netcard_type((guint8)NET_TYPE_AGGREGATION, &eth_obj);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, debug_log(DLOG_MASS, "%s : get eth obj fail", __FUNCTION__));

    (void)dal_get_property_value_byte(eth_obj, ETH_ATTRIBUTE_OUT_TYPE, &out_type);
    return_val_do_info_if_fail(OUT_ETHERNET == out_type, VOS_ERR,
        debug_log(DLOG_MASS, "%s : is not out type eth", __FUNCTION__));

    (void)dal_get_object_handle_nth(ETH_CLASS_NAME_MGNTPORTCAP, 0, &obj_handle);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_MNGTPORTCAP_AGGREGATIONENABLE, &aggregation_enable);

    return_val_do_info_if_fail(aggregation_enable, VOS_ERR,
        debug_log(DLOG_MASS, "%s : aggregation port not enabled", __FUNCTION__));

    return VOS_OK;
}

LOCAL void _get_pcie_mgnt_netport_info(json_object *netport_array)
{
    gint32 ret;
    OBJ_HANDLE obj_handle_tmp;
    GSList *eth_list = NULL;
    GSList *eth_list_iter = NULL;
    guchar eth_type;
    guchar port_id;
    guchar out_type;

    ret = dal_get_specific_object_byte(ETH_CLASS_NAME_MGNTPORTCAP, PROPERTY_MNGTPORTCAP_PCIEMGNT_ENABLE, ENABLED,
        &obj_handle_tmp);
    return_if_expr(ret != VOS_OK);

    ret = dfl_get_object_list(ETH_CLASS_NAME, &eth_list);
    return_if_expr(ret != VOS_OK);

    for (eth_list_iter = eth_list; eth_list_iter != NULL; eth_list_iter = eth_list_iter->next) {
        obj_handle_tmp = (OBJ_HANDLE)eth_list_iter->data;
        eth_type = 0;
        port_id = 0;
        out_type = 0;

        (void)dal_get_property_value_byte(obj_handle_tmp, ETH_ATTRIBUTE_TYPE, &eth_type);
        (void)dal_get_property_value_byte(obj_handle_tmp, ETH_ATTRIBUTE_OUT_TYPE, &out_type);

        continue_if_expr((eth_type != NET_TYPE_CABINET_VLAN) || (out_type != OUT_ETHERNET));

        
        (void)dal_get_property_value_byte(obj_handle_tmp, ETH_ATTRIBUTE_ETH_ID, &port_id);
        _get_netport_info_with_eth_handle(obj_handle_tmp, port_id, netport_array);
    }

    g_slist_free(eth_list);

    return;
}


LOCAL void redfish_get_all_bmc_netport(OBJ_HANDLE obj_handle, json_object **huawei)
{
    gint32 ret;
    json_object *netport_array = NULL;
    guint8 netmode_flag = 0;
    guchar board_type = 0;

    return_do_info_if_expr(NULL == huawei, debug_log(DLOG_ERROR, "%s : NULL pointer", __FUNCTION__));

    netport_array = json_object_new_array();
    return_do_info_if_expr(NULL == netport_array,
        debug_log(DLOG_ERROR, "%s : json_object_new_array fail", __FUNCTION__);
        json_object_object_add(*huawei, RFPROP_MANAGER_ETH_OEM_NET_PORT, netport_array));

    
    ret = redfish_get_board_type(&board_type);
    return_do_info_if_fail(VOS_OK == ret,
        json_object_object_add(*huawei, RFPROP_MANAGER_ETH_OEM_NET_PORT, netport_array);
        debug_log(DLOG_ERROR, "%s, %d: get board_type fail.\n", __FUNCTION__, __LINE__));
    return_do_info_if_fail((BOARD_SWITCH != board_type),
        json_object_object_add(*huawei, RFPROP_MANAGER_ETH_OEM_NET_PORT, netport_array));
    

    
    (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_NET_MODE_FLAG, &netmode_flag);

    if (RF_UNSUPPORT_ADAPTIVE_NET_MODE == netmode_flag) {
        debug_log(DLOG_MASS, "%s : netport not support adaptive", __FUNCTION__);
        json_object_object_add(*huawei, RFPROP_MANAGER_ETH_OEM_NET_PORT, netport_array);
        return;
    }

    
    ret = verify_dedicated_exist();
    if (ret == VOS_OK) {
        _get_pcie_mgnt_netport_info(netport_array);
        get_dedicated_object_info(&netport_array);
    }

    
    ret = verify_aggregation_exist();
    if (VOS_OK == ret) {
        get_object_info((guint8)NET_TYPE_AGGREGATION, &netport_array);
    }

    
    (void)get_each_object_info(LOM_NCSI_TYPE, &netport_array);
    (void)get_each_object_info(PCIE_NCSI_TYPE, &netport_array);
    (void)get_each_object_info(NET_TYPE_LOM2, &netport_array);
    (void)get_each_object_info(NET_TYPE_OCP_CARD, &netport_array);
    (void)get_each_object_info(NET_TYPE_OCP2_CARD, &netport_array);

    json_object_object_add(*huawei, RFPROP_MANAGER_ETH_OEM_NET_PORT, netport_array);

    return;
}

LOCAL void get_adaptive_info_jso(OBJ_HANDLE obj_handle, guchar port_number, json_object *netport_array)
{
    json_object *type_jso = NULL;
    json_object *port_number_jso = NULL;
    json_object *adaptive_jso = NULL;
    json_object *object = NULL;
    guint8 adaptive_flag = 0;
    gint32 ret;
    guchar eth_type = 0;

    if (netport_array == NULL) {
        debug_log(DLOG_ERROR, "%s : NULL pointer", __FUNCTION__);

        return;
    }

    object = json_object_new_object();
    if (object == NULL) {
        debug_log(DLOG_ERROR, "%s : json_object_new_object fail", __FUNCTION__);

        return;
    }

    dal_get_property_value_byte(obj_handle, ETH_ATTRIBUTE_TYPE, &eth_type);
    type_jso = json_object_new_string(net_type_int2str(eth_type));

    
    (void)dal_get_property_value_byte(obj_handle, ETH_ATTRIBUTE_ADAPTIVE_FLAG, &adaptive_flag);
    adaptive_jso = json_object_new_boolean((json_bool)adaptive_flag);

    port_number_jso = json_object_new_int((gint32)port_number);

    json_object_object_add(object, RFPROP_MANAGER_ETH_OEM_PORT_TYPE, type_jso);
    json_object_object_add(object, RFPROP_MANAGER_ETH_OEM_PORT_SLIK, port_number_jso); // 专用网口的port_number写1
    json_object_object_add(object, RFPROP_MANAGER_ETH_OEM_ADAPTIVE, adaptive_jso);

    ret = json_object_array_add(netport_array, object);
    if (ret != 0) {
        (void)json_object_put(object);
        debug_log(DLOG_ERROR, "%s : json_object_array_add fail", __FUNCTION__);

        return;
    }

    return;
}


LOCAL void get_dedicated_adaptive_info(json_object **netport_array)
{
    OBJ_HANDLE obj_handle = 0;
    OBJ_HANDLE obj_handle_port_cap;
    gint32 ret;
    guchar port_number = 0;

    return_do_info_if_expr(NULL == netport_array, debug_log(DLOG_ERROR, "%s : NULL pointer", __FUNCTION__));
    
    if (dal_is_support_eth_define_specific_propery(PROPERTY_SUPPORT_DETACH_ETHGROUP)) {
        get_multi_dedicated_object_or_adaptive_info(1, netport_array); // 1: 获取专用网口的自适应情况
        return;
    }

    if (dal_check_ar_card_support()) {
        get_multi_dedicat_eth_info(netport_array, get_adaptive_info_jso);
        return;
    }

    
    ret = get_eth_obj_by_netcard_type((gint8)NET_TYPE_DEDICATED, &obj_handle);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_MASS, "%s : get eth object fail", __FUNCTION__));

    if (dal_get_specific_object_byte(ETH_CLASS_NAME_MGNTPORTCAP, PROPERTY_MNGTPORTCAP_PCIEMGNT_ENABLE, ENABLE,
        &obj_handle_port_cap) == VOS_OK) {
        (void)dal_get_property_value_byte(obj_handle, ETH_ATTRIBUTE_ETH_ID, &port_number);
    } else {
        port_number = 1; // 专用网口的port_number写1
    }

    get_adaptive_info_jso(obj_handle, port_number, *netport_array);

    return;
}

LOCAL void _get_pcie_mgnt_port_adaptive_info(json_object *netport_array)
{
    gint32 ret;
    OBJ_HANDLE obj_handle_tmp;
    GSList *obj_handle_list = NULL;
    GSList *obj_list_iter = NULL;
    guchar eth_type;
    guchar out_type;
    guchar eth_id;

    ret = dal_get_specific_object_byte(ETH_CLASS_NAME_MGNTPORTCAP, PROPERTY_MNGTPORTCAP_PCIEMGNT_ENABLE, ENABLE,
        &obj_handle_tmp);
    if (ret != VOS_OK) {
        return;
    }

    ret = dfl_get_object_list(ETH_CLASS_NAME_ETH, &obj_handle_list);
    if (ret != VOS_OK) {
        return;
    }

    for (obj_list_iter = obj_handle_list; obj_list_iter != NULL; obj_list_iter = obj_list_iter->next) {
        obj_handle_tmp = (OBJ_HANDLE)obj_list_iter->data;
        eth_type = 0;
        out_type = 0;
        eth_id = 0;

        (void)dal_get_property_value_byte(obj_handle_tmp, ETH_ATTRIBUTE_TYPE, &eth_type);
        (void)dal_get_property_value_byte(obj_handle_tmp, ETH_ATTRIBUTE_OUT_TYPE, &out_type);
        if ((eth_type == NET_TYPE_CABINET_VLAN) && (out_type == OUTTER_GROUP_TYPE)) {
            (void)dal_get_property_value_byte(obj_handle_tmp, ETH_ATTRIBUTE_ETH_ID, &eth_id);
            get_adaptive_info_jso(obj_handle_tmp, eth_id, netport_array);
        } else {
            continue;
        }
    }

    g_slist_free(obj_handle_list);

    return;
}


LOCAL void get_each_object_adaptive_info(guint8 type_int, json_object **netport_array)
{
#define NCSI_SUPPORTED 1
    GSList *business_valid_obj = NULL;
    OBJ_HANDLE business_obj = 0;
    GSList *node = NULL;
    gchar *type_str = NULL;
    json_object *type = NULL;
    guint8 slik_number = 0;
    json_object *port_number = NULL;
    guint8 adaptive_flag = 0;
    json_object *adaptive = NULL;
    json_object *object = NULL;
    gint32 ret;
    guint8 card_type = 0;
    guint8 ncsi_state = 0;

    return_do_info_if_expr(NULL == netport_array, debug_log(DLOG_ERROR, "%s : NULL pointer", __FUNCTION__));

    ret = verify_lom_pcie_lom2_supported(type_int);
    return_if_expr(ret != VOS_OK);

    type_str = net_type_int2str(type_int);
    return_if_expr(NULL == type_str);

    ret = dfl_get_object_list(ETH_CLASS_NAME_BUSY_ETH, &business_valid_obj);
    return_do_info_if_expr((DFL_OK != ret) || (NULL == business_valid_obj),
        debug_log(DLOG_DEBUG, "%s: get BusinessPort object list fail", __FUNCTION__));

    for (node = business_valid_obj; node; node = g_slist_next(node)) {
        business_obj = (OBJ_HANDLE)node->data;

        (void)dal_get_property_value_byte(business_obj, BUSY_ETH_ATTRIBUTE_CARD_TYPE, &card_type);
        
        continue_if_expr(card_type != type_int);

        ret = get_businessport_ncsi_supported_status(business_obj, &ncsi_state);
        continue_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "eth_get_eth_link_status failed:obj_name is %s\n",
            dfl_get_object_name(business_obj)));
        continue_if_expr(NCSI_SUPPORTED != ncsi_state);

        object = json_object_new_object();
        continue_do_info_if_fail(NULL != object, debug_log(DLOG_ERROR, "%s : new object fail", __FUNCTION__));

        type = json_object_new_string((const char *)type_str);
        do_info_if_true(NULL == type, debug_log(DLOG_ERROR, "%s : new type json string fail", __FUNCTION__));

        (void)dal_get_property_value_byte(business_obj, BUSY_ETH_ATTRIBUTE_SILK_NUM, &slik_number);
        port_number = json_object_new_int((int32_t)slik_number);
        do_info_if_true(NULL == port_number, debug_log(DLOG_ERROR, "%s : new port number json int fail", __FUNCTION__));

        (void)dal_get_property_value_byte(business_obj, BUSY_ETH_ATTRIBUTE_ADAPTIVE_FLAG, &adaptive_flag);
        adaptive = json_object_new_boolean((json_bool)adaptive_flag);
        do_info_if_true(NULL == adaptive, debug_log(DLOG_ERROR, "%s : new adaptive json boolean fail", __FUNCTION__));

        json_object_object_add(object, RFPROP_MANAGER_ETH_OEM_PORT_TYPE, type);
        json_object_object_add(object, RFPROP_MANAGER_ETH_OEM_PORT_SLIK, port_number);
        json_object_object_add(object, RFPROP_MANAGER_ETH_OEM_ADAPTIVE, adaptive);

        type = NULL;
        port_number = NULL;
        adaptive = NULL;

        ret = json_object_array_add(*netport_array, object);
        do_info_if_true(VOS_OK != ret, debug_log(DLOG_ERROR, "%s : add object to array fail", __FUNCTION__);
            (void)json_object_put(object));

        object = NULL;
    }

    g_slist_free(business_valid_obj);
    return;
}


LOCAL void redfish_get_netport_adaptive(OBJ_HANDLE group_obj, json_object **huawei)
{
    gint32 ret;
    guint8 net_mode = 0;
    guint8 adaptive_flag = 0;
    json_object *netport_array = NULL;
    guchar board_type = 0;

    return_do_info_if_expr(NULL == huawei, debug_log(DLOG_ERROR, "%s : NULL pointer", __FUNCTION__));

    netport_array = json_object_new_array();
    return_do_info_if_expr(NULL == netport_array,
        debug_log(DLOG_ERROR, "%s : new netport adaptive array fail", __FUNCTION__);
        json_object_object_add(*huawei, RFPROP_MANAGER_ETH_OEM_ADAPTIVEPORT, netport_array));

    
    ret = redfish_get_board_type(&board_type);
    return_do_info_if_fail(VOS_OK == ret,
        json_object_object_add(*huawei, RFPROP_MANAGER_ETH_OEM_ADAPTIVEPORT, netport_array);
        debug_log(DLOG_ERROR, "%s, %d: get board_type fail.\n", __FUNCTION__, __LINE__));
    return_do_info_if_fail((BOARD_SWITCH != board_type),
        json_object_object_add(*huawei, RFPROP_MANAGER_ETH_OEM_ADAPTIVEPORT, netport_array));
    

    
    (void)dal_get_property_value_byte(group_obj, ETH_GROUP_ATTRIBUTE_NET_MODE_FLAG, &adaptive_flag);
    (void)dal_get_property_value_byte(group_obj, ETH_GROUP_ATTRIBUTE_NET_MODE, &net_mode);
    return_do_info_if_expr(RF_UNSUPPORT_ADAPTIVE_NET_MODE == adaptive_flag,
        json_object_object_add(*huawei, RFPROP_MANAGER_ETH_OEM_ADAPTIVEPORT, netport_array));

    
    ret = verify_dedicated_exist();
    if (VOS_OK == ret) {
        _get_pcie_mgnt_port_adaptive_info(netport_array);
        get_dedicated_adaptive_info(&netport_array);
    }

    
    (void)get_each_object_adaptive_info(LOM_NCSI_TYPE, &netport_array);
    (void)get_each_object_adaptive_info(PCIE_NCSI_TYPE, &netport_array);
    (void)get_each_object_adaptive_info(NET_TYPE_LOM2, &netport_array);
    (void)get_each_object_adaptive_info(NET_TYPE_OCP_CARD, &netport_array);
    (void)get_each_object_adaptive_info(NET_TYPE_OCP2_CARD, &netport_array);

    json_object_object_add(*huawei, RFPROP_MANAGER_ETH_OEM_ADAPTIVEPORT, netport_array);

    return;
}


void redfish_get_dns_address_origin(json_object **huawei)
{
    guint8 dns_address_origin = 0;
    guint8 dns_ip_ver = 0;
    gchar *dns_address_origin_str = NULL;
    OBJ_HANDLE dns_obj_handle = 0;
    json_object *dns_address_origin_jso = NULL;

    return_do_info_if_expr(NULL == huawei, debug_log(DLOG_ERROR, "%s : NULL pointer", __FUNCTION__));

    (void)dal_get_object_handle_nth(CLASS_NAME_DNSSETTING, 0, &dns_obj_handle);

    (void)dal_get_property_value_byte(dns_obj_handle, PROPERTY_DNSSETTING_DNSMODE, &dns_address_origin);

    switch (dns_address_origin) {
        case DNS_ADD_ORIGIN_MANUAL:
            dns_address_origin_str = DNS_ADD_ORIGIN_MANUAL_STR;
            break;

        case DNS_ADD_ORIGIN_AUTO:
            (void)dal_get_property_value_byte(dns_obj_handle, PROPERTY_DNSSETTING_DNSIPVER, &dns_ip_ver);

            if (DNS_IP_VER_IPV4 == dns_ip_ver) {
                dns_address_origin_str = DNS_IP_VER_IPV4_STR;
            } else if (DNS_IP_VER_IPV6 == dns_ip_ver) {
                dns_address_origin_str = DNS_IP_VER_IPV6_STR;
            } else {
                
                debug_log(DLOG_DEBUG, "%s : invalid dns ip version value is %d", __FUNCTION__, dns_ip_ver);
                
                json_object_object_add(*huawei, RFPROP_MANAGER_ETH_OEM_DNS_MODE, dns_address_origin_jso);
                return;
            }

            break;

        default:
            
            debug_log(DLOG_DEBUG, "%s : invalid dns address origin value is %d", __FUNCTION__, dns_address_origin);
            
    }

    return_do_info_if_expr(NULL == dns_address_origin_str,
        json_object_object_add(*huawei, RFPROP_MANAGER_ETH_OEM_DNS_MODE, dns_address_origin_jso));

    dns_address_origin_jso = json_object_new_string((const char *)dns_address_origin_str);
    do_info_if_true(NULL == dns_address_origin_jso,
        debug_log(DLOG_ERROR, "%s : json_object_new_string fail", __FUNCTION__));

    json_object_object_add(*huawei, RFPROP_MANAGER_ETH_OEM_DNS_MODE, dns_address_origin_jso);

    return;
}

LOCAL void __redfish_get_switch_connections(PROVIDER_PARAS_S *i_paras, json_object **huawei)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = OBJ_HANDLE_COMMON;
    (void)dal_get_object_handle_nth(ETH_CLASS_NAME_MGNTPORTCAP, 0, &obj_handle);
    GSList* output_list = NULL;
    const gchar* json_str = NULL;
    gsize json_str_len = 0;

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        ETH_CLASS_NAME_MGNTPORTCAP, METHOD_MGNTPORTCAP_GET_SW_CONNS, AUTH_ENABLE, i_paras->user_role_privilege,
        NULL, &output_list);
    if (ret != RET_OK) {
        return;
    }

    json_str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), &json_str_len);
    json_object_object_add(*huawei, RFPROP_MANAGER_ETH_OEM_SW_CONNS, json_tokener_parse(json_str));
    uip_free_gvariant_list(output_list);
    return;
}


LOCAL gint32 get_manager_ethernetinterface_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    json_object *huawei = NULL;
    guint8 software_type = 0;

    if (VOS_OK != provider_paras_check(i_paras) || NULL == o_message_jso || NULL == o_result_jso) {
        debug_log(DLOG_ERROR, "%s : NULL pointer", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    
    obj_handle = i_paras->obj_handle;

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : json_object_new_object fail", __FUNCTION__));

    huawei = json_object_new_object();
    return_val_do_info_if_expr(NULL == huawei, HTTP_OK,
        debug_log(DLOG_ERROR, "%s : json_object_new_object fail", __FUNCTION__);
        json_object_object_add(*o_result_jso, RFPROP_COMMON_HUAWEI, huawei));

    
    redfish_get_ip_version(obj_handle, &huawei);

    (void)dal_get_software_type(&software_type);
    
    if (MGMT_SOFTWARE_TYPE_EM != software_type) { // 不使用此属性
        
        redfish_get_net_mode(obj_handle, &huawei);

        
        redfish_get_ncsi_mode(obj_handle, &huawei);

        
        redfish_get_current_manager_netport(obj_handle, &huawei);

        
        redfish_get_all_bmc_netport(obj_handle, &huawei);

        
        redfish_get_netport_adaptive(obj_handle, &huawei);
    }
    

    
    redfish_get_dns_address_origin(&huawei);

    
    __redfish_get_switch_connections(i_paras, &huawei);

    json_object_object_add(*o_result_jso, RFPROP_COMMON_HUAWEI, huawei);

    return HTTP_OK;
}


LOCAL gint32 get_manager_ethernetinterface_link(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    json_object *oem = NULL;
    json_object *huawei = NULL;
    json_object *system_eth = NULL;
    json_object *odata_id = NULL;
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    gchar sys_eth_uri[MAX_URI_LENGTH] = {0};
    guchar board_type = 0;
    guint8 software_type = 0;

    if (VOS_OK != provider_paras_check(i_paras) || NULL == o_message_jso || NULL == o_result_jso) {
        debug_log(DLOG_ERROR, "%s : NULL pointer", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    (void)dal_get_software_type(&software_type);
    
    if (MGMT_SOFTWARE_TYPE_EM == software_type) { // 不使用此属性
        return HTTP_INTERNAL_SERVER_ERROR;        // 使返回值为null
    }

    
    ret = redfish_get_x86_enable_type(&board_type);
    return_val_do_info_if_fail((VOS_OK == ret), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: systems ethernetinterface link get board_type fail.\n", __FUNCTION__, __LINE__));
    return_val_if_fail((DISABLE != board_type), HTTP_INTERNAL_SERVER_ERROR);
    

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot, MAX_RSC_NAME_LEN);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : get slot fail", __FUNCTION__));

    ret = snprintf_s(sys_eth_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, RFPROP_SYSTEM_ETHS, slot);
    return_val_do_info_if_expr(VOS_OK >= ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : snprintf_s uri fail", __FUNCTION__));

    odata_id = json_object_new_string((const char *)sys_eth_uri);
    return_val_do_info_if_expr(NULL == odata_id, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : json_object_new_string fail", __FUNCTION__));

    system_eth = json_object_new_object();
    return_val_do_info_if_expr(NULL == system_eth, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : json_object_new_object fail", __FUNCTION__);
        (void)json_object_put(odata_id));
    json_object_object_add(system_eth, RFPROP_MANAGER_ETH_ODATA_ID, odata_id);

    huawei = json_object_new_object();
    return_val_do_info_if_expr(NULL == huawei, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : json_object_new_object fail", __FUNCTION__);
        (void)json_object_put(system_eth));
    json_object_object_add(huawei, RF_CLASS_ETHERNETINTERFACE, system_eth);

    oem = json_object_new_object();
    return_val_do_info_if_expr(NULL == oem, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : json_object_new_object fail", __FUNCTION__);
        (void)json_object_put(huawei));
    json_object_object_add(oem, RFPROP_COMMON_HUAWEI, huawei);

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : json_object_new_object fail", __FUNCTION__);
        (void)json_object_put(oem));
    json_object_object_add(*o_result_jso, RFPROP_COMMON_OEM, oem);

    return HTTP_OK;
}


LOCAL gint32 set_manager_bmc_host_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;

    if (VOS_OK != provider_paras_check(i_paras) || NULL == o_message_jso) {
        debug_log(DLOG_ERROR, "%s%d NULL Pointer!", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_MANAGER_BMC_HOSTNAME, o_message_jso,
        RFPROP_MANAGER_BMC_HOSTNAME));

    (void)dal_get_object_handle_nth(BMC_CLASEE_NAME, 0, &obj_handle);
    
    return_val_do_info_if_expr(NULL == dal_json_object_get_str(i_paras->val_jso), HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_TYPE_ERR, RFPROP_MANAGER_BMC_HOSTNAME, o_message_jso, RF_VALUE_NULL,
        RFPROP_MANAGER_BMC_HOSTNAME));
    input_list = g_slist_append(input_list, g_variant_new_string(dal_json_object_get_str(i_paras->val_jso)));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        BMC_CLASEE_NAME, METHOD_BMC_HOSTNAME, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
            break;

        case REDFISH_VOS_ERROR:
            (void)create_message_info(MSGID_INVALID_HOSTNAME, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;

        default: 
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 set_manager_fqdn(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar *hostname = NULL;
    gchar *domainname = NULL;
    gchar **split_string = NULL;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    guint8 dns_mode = 0;

    
    return_val_do_info_if_expr((provider_paras_check(i_paras) != VOS_OK || o_message_jso == NULL),
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_MANAGER_BMC_FQDN, o_message_jso,
        RFPROP_MANAGER_BMC_FQDN));
    return_val_do_info_if_expr(NULL == i_paras->val_jso, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_TYPE_ERR, RFPROP_MANAGER_BMC_FQDN, o_message_jso, RF_VALUE_NULL,
        RFPROP_MANAGER_BMC_FQDN));

    
    (void)dal_get_object_handle_nth(CLASS_NAME_DNSSETTING, 0, &obj_handle);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_DNSSETTING_DNSMODE, &dns_mode);
    
    return_val_do_info_if_fail(DNS_ADD_ORIGIN_MANUAL == dns_mode, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_MODIFY_FAILED_WITH_AUTO_DNS, RFPROP_MANAGER_BMC_FQDN, o_message_jso));
    
    
    return_val_do_info_if_expr(0 == strlen(dal_json_object_get_str(i_paras->val_jso)), HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFPROP_MANAGER_BMC_FQDN, o_message_jso,
        dal_json_object_get_str(i_paras->val_jso), RFPROP_MANAGER_BMC_FQDN));
    split_string = g_strsplit(dal_json_object_get_str(i_paras->val_jso), ".", 2);
    return_val_do_info_if_expr(NULL == split_string, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFPROP_MANAGER_BMC_FQDN, o_message_jso,
        dal_json_object_get_str(i_paras->val_jso), RFPROP_MANAGER_BMC_FQDN));
    
    hostname = split_string[0];
    domainname = split_string[1];
    return_val_do_info_if_expr(NULL == domainname, HTTP_BAD_REQUEST, g_strfreev(split_string);
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFPROP_MANAGER_BMC_FQDN, o_message_jso,
        dal_json_object_get_str(i_paras->val_jso), RFPROP_MANAGER_BMC_FQDN));
    
    (void)dal_get_object_handle_nth(CLASS_NAME_BMC, 0, &obj_handle);
    input_list = g_slist_append(input_list, g_variant_new_string(hostname));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        BMC_CLASEE_NAME, METHOD_BMC_HOSTNAME, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    input_list = NULL;
    obj_handle = 0;

    switch (ret) {
        case VOS_OK:
            break;

            
        case COMP_CODE_INVALID_CMD:
            g_strfreev(split_string);
            (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, RFPROP_MANAGER_BMC_FQDN, o_message_jso,
                RFPROP_MANAGER_BMC_FQDN);
            return HTTP_NOT_IMPLEMENTED;

            
        case REDFISH_VOS_ERROR:
            g_strfreev(split_string);
            (void)create_message_info(MSGID_PROP_NOT_IN_LIST, RFPROP_MANAGER_BMC_FQDN, o_message_jso,
                dal_json_object_get_str(i_paras->val_jso), RFPROP_MANAGER_BMC_FQDN);
            return HTTP_BAD_REQUEST;

        default: 
            g_strfreev(split_string);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    (void)dal_get_object_handle_nth(CLASS_NAME_DNSSETTING, 0, &obj_handle);
    input_list = g_slist_append(input_list, g_variant_new_string(domainname));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_NAME_DNSSETTING, METHOD_DNSSETTING_DOMAINNAME, AUTH_DISABLE, i_paras->user_role_privilege, input_list,
        NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
            break;

            
        case COMP_CODE_INVALID_CMD:
            g_strfreev(split_string);
            (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, RFPROP_MANAGER_BMC_FQDN, o_message_jso,
                RFPROP_MANAGER_BMC_FQDN);
            return HTTP_NOT_IMPLEMENTED;

            
        default:
            g_strfreev(split_string);
            (void)create_message_info(MSGID_PROP_NOT_IN_LIST, RFPROP_MANAGER_BMC_FQDN, o_message_jso,
                dal_json_object_get_str(i_paras->val_jso), RFPROP_MANAGER_BMC_FQDN);
            return HTTP_BAD_REQUEST;
    }

    g_strfreev(split_string);
    return HTTP_OK;
}

LOCAL gint32 set_manager_ethernetinterface_VLAN_info(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    json_object *value_id = NULL;
    json_object *value_state = NULL;
    guint16 id = 0;
    gint32 json_int = 0;
    guint32 state = 0;
    guint32 state_old = 0;
    gchar id_string[50] = {0};
    guint8 net_mode_flag = 0;

    const gchar *state_error_mes = "VLAN/VLANEnable";
    const gchar *id_error_mes = "VLAN/VLANId";

    if (VOS_OK != provider_paras_check(i_paras) || NULL == o_message_jso) {
        debug_log(DLOG_ERROR, "%s%d NULL Pointer!", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_expr(dal_check_if_mm_board(), HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, RFPROP_MANAGER_ETH_VLAN, o_message_jso,
        RFPROP_MANAGER_ETH_VLAN));

    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_MANAGER_ETH_VLAN, o_message_jso,
        RFPROP_MANAGER_ETH_VLAN));
    
    obj_handle = i_paras->obj_handle;

    
    (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_NET_MODE_FLAG, &net_mode_flag);
    return_val_do_info_if_expr(0 == net_mode_flag, HTTP_NOT_IMPLEMENTED,
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, RFPROP_MANAGER_ETH_VLAN, o_message_jso,
        RFPROP_MANAGER_ETH_VLAN));

    if (json_object_object_get_ex(i_paras->val_jso, RFPROP_MANAGER_ETH_VLAN_STATE, &value_state)) {
        state = json_object_get_boolean(value_state);
        if (json_object_object_get_ex(i_paras->val_jso, RFPROP_MANAGER_ETH_VLAN_ID, &value_id)) {
            
            
            if (json_object_get_type(value_id) != json_type_int) {
                (void)create_message_info(MSGID_PROP_TYPE_ERR, id_error_mes, o_message_jso,
                    dal_json_object_to_json_string(value_id), id_error_mes);
                return HTTP_BAD_REQUEST;
            }

            if (state == 0) {
                (void)create_message_info(MSGID_VLAN_INFO_CONFLICT, NULL, o_message_jso);
                return HTTP_BAD_REQUEST;
            }
            
            json_int = json_object_get_int(value_id);
            return_val_do_info_if_expr(json_int < 0 || json_int > G_MAXUINT16, HTTP_BAD_REQUEST,
                (void)create_message_info(MSGID_PROP_NOT_IN_LIST, RFPROP_MANAGER_ETH_VLAN, o_message_jso,
                dal_json_object_to_json_string(value_id), RFPROP_MANAGER_ETH_VLAN));
            id = (guint16)json_int;
            
        } else {
            return_val_do_info_if_expr(NULL == value_state, HTTP_BAD_REQUEST,
                (void)create_message_info(MSGID_PROP_TYPE_ERR, state_error_mes, o_message_jso, RF_VALUE_NULL,
                state_error_mes));

            
            if (state == 0) {
                id = 0;
            } else {
                (void)dal_get_property_value_uint32(obj_handle, ETH_GROUP_ATTRIBUTE_VLAN_STATE, &state_old);
                (void)dal_get_property_value_uint16(obj_handle, ETH_GROUP_ATTRIBUTE_VLAN_ID, &id);
                
                return_val_do_info_if_expr(state_old != 1, HTTP_BAD_REQUEST, \
                    (void)create_message_info(MSGID_PROP_MISSING, id_error_mes, o_message_jso, id_error_mes));
                state = 1;
            }
        }
    } else if (json_object_object_get_ex(i_paras->val_jso, RFPROP_MANAGER_ETH_VLAN_ID, &value_id)) {
        
        return_val_do_info_if_expr(json_type_double == json_object_get_type(value_id), HTTP_BAD_REQUEST,
            (void)create_message_info(MSGID_PROP_TYPE_ERR, id_error_mes, o_message_jso,
            dal_json_object_to_json_string(value_id), id_error_mes));

        
        if (json_object_object_get_ex(i_paras->val_jso, RFPROP_MANAGER_ETH_VLAN_ID, &value_id)) {
            state = 1;
            json_int = json_object_get_int(value_id);
            return_val_do_info_if_expr(json_int < 0 || json_int > G_MAXUINT16, HTTP_BAD_REQUEST,
                (void)create_message_info(MSGID_PROP_NOT_IN_LIST, RFPROP_MANAGER_ETH_VLAN_ID, o_message_jso,
                dal_json_object_to_json_string(value_id), RFPROP_MANAGER_ETH_VLAN_ID));
            id = (guint16)json_int;
        }

        
    } else {
        return HTTP_BAD_REQUEST;
    }

    
    input_list = g_slist_append(input_list, g_variant_new_uint16(id));
    input_list = g_slist_append(input_list, g_variant_new_uint32(state));

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_METHOD_SET_VLAN_ID, AUTH_ENABLE, i_paras->user_role_privilege, input_list,
        NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
            break;

        case REDFISH_ETH_ERR:
            
            (void)snprintf_s(id_string, sizeof(id_string), sizeof(id_string) - 1, "%u", id);
            
            (void)create_message_info(MSGID_PROP_NOT_IN_LIST, id_error_mes, o_message_jso, id_string, id_error_mes);
            return HTTP_BAD_REQUEST;

        case COMP_CODE_INVALID_CMD:
            (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, RFPROP_MANAGER_ETH_VLAN, o_message_jso,
                RFPROP_MANAGER_ETH_VLAN);
            return HTTP_NOT_IMPLEMENTED;

        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}

LOCAL gint32 redfish_ipv4mode_stringtonum(json_object *ipmodestring, guint8 *ipmode)
{
    if (0 == g_strcmp0(dal_json_object_get_str(ipmodestring), IP_MODE_STATIC_STRING)) {
        *ipmode = IP_STATIC_MODE;
    } else if (0 == g_strcmp0(dal_json_object_get_str(ipmodestring), IP_MODE_DHCP_STRING)) {
        *ipmode = IP_DHCP_MODE;
    } else {
        *ipmode = 0;
    } 

    return VOS_OK;
}

LOCAL gint32 redfish_check_ip(json_object *input_ip, json_object *o_message_jso, const gchar *error_mes,
    const gchar *prop)
{
    json_object *message = NULL;

    if (json_type_string != json_object_get_type(input_ip)) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, error_mes, &message, RF_VALUE_NULL, error_mes);
        (void)json_object_array_add(o_message_jso, message);
        return VOS_ERR;
    }

    if (0 == g_strcmp0(prop, RFPROP_MANAGER_ETH_IPMASK) &&
        VOS_OK == redfish_subnet_mask_check(dal_json_object_get_str(input_ip))) {
        return VOS_OK;
    } else if (0 != g_strcmp0(prop, RFPROP_MANAGER_ETH_IPMASK) &&
        VOS_OK == vos_ipv4_addr_valid_check((const guchar *)dal_json_object_get_str(input_ip))) {
        return VOS_OK;
    } else {
        if (0 == g_strcmp0(prop, RFPROP_MANAGER_ETH_IPADDR)) {
            (void)create_message_info(MSGID_INVALID_IPV4ADDRESS, error_mes, &message,
                dal_json_object_get_str(input_ip), error_mes);
            (void)json_object_array_add(o_message_jso, message);
        } else if (0 == g_strcmp0(prop, RFPROP_MANAGER_ETH_IPMASK)) {
            (void)create_message_info(MSGID_INVALID_SUBNETMASK, error_mes, &message,
                dal_json_object_get_str(input_ip), error_mes);
            (void)json_object_array_add(o_message_jso, message);
        } else if (0 == g_strcmp0(prop, RFPROP_MANAGER_ETH_IPGATEWAY)) {
            (void)create_message_info(MSGID_INVALID_IPV4GATEWAY, error_mes, &message,
                dal_json_object_get_str(input_ip), error_mes);
            (void)json_object_array_add(o_message_jso, message);
        }

        return VOS_ERR;
    }
}


LOCAL gint32 __parse_set_ip_result_code(gint32 ret, json_object **o_message_jso, guint8 log_flag, const gchar *input_ip)
{
    gint32 response_code = HTTP_BAD_REQUEST;
    const gchar *ipv4_error_mes = "IPv4Addresses/0";
    json_object *message = NULL;

    if (VOS_OK == ret) {
        response_code = HTTP_OK;
        return response_code;
    } else if (COMP_CODE_INVALID_CMD == ret) {
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, RFPROP_MANAGER_ETH_IPV4ADDR, &message,
            RFPROP_MANAGER_ETH_IPV4ADDR);
        (void)json_object_array_add(*o_message_jso, message);
        return HTTP_NOT_IMPLEMENTED;
    } else if (ERROR_PARAMETER_INVALID == ret) {
        (void)create_message_info(MSGID_INVALID_IPV4ADDRESS, "IPv4Addresses/0/Address", &message, input_ip,
            "IPv4Addresses/0/Address");
        (void)json_object_array_add(*o_message_jso, message);
    } else {
        if ((log_flag & 07) != 0) {
            (void)create_message_info(MSGID_IPV4_ADDRESS_CONFLCT, ipv4_error_mes, &message);
            (void)json_object_array_add(*o_message_jso, message);
        }
    }

    return response_code;
}


LOCAL gint32 redfish_smm_set_ip_mask_gateway(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    OBJ_HANDLE obj_handle)
{
    errno_t safe_fun_ret = EOK;
    gint32 ret;
    gchar ipaddr[IPV4_IP_STR_SIZE + 1] = { 0 };
    guint32 ip_val = 0;
    gchar ipmask[IPV4_MASK_STR_SIZE + 1] = { 0 };
    guint32 mask_val = 0;
    gchar ipgateway[IPV4_GATEWAY_STR_SIZE + 1] = { 0 };
    guint32 gateway_val = 0;
    guint8 log_flag = 0;
    OBJ_HANDLE eth0_handle = 0;

    json_object *ip_info = json_object_array_get_idx(i_paras->val_jso, 0);
    json_object *ip = NULL;
    json_object *mask = NULL;
    json_object *gateway = NULL;
    json_object *message = NULL;
    gint32 response_code = HTTP_BAD_REQUEST;
    

    const gchar *address_error_mes = "IPv4Addresses/0/Address";
    const gchar *mask_error_mes = "IPv4Addresses/0/SubnetMask";
    const gchar *gateway_error_mes = "IPv4Addresses/0/Gateway";
    guint8 mode = 0;
    guint8 as_status = 0;
    GSList *input_list = NULL;
    gint32 set_ip;
    gint32 set_mask;
    gint32 set_gateway;
    
    set_ip = json_object_object_get_ex(ip_info, RFPROP_MANAGER_ETH_IPADDR, &ip);
    set_mask = json_object_object_get_ex(ip_info, RFPROP_MANAGER_ETH_IPMASK, &mask);
    set_gateway = json_object_object_get_ex(ip_info, RFPROP_MANAGER_ETH_IPGATEWAY, &gateway);
    
    (void)dal_get_property_value_string(obj_handle, ETH_GROUP_ATTRIBUTE_IP_ADDR, ipaddr, IPV4_IP_STR_SIZE + 1);
    (void)dal_get_property_value_string(obj_handle, ETH_GROUP_ATTRIBUTE_SUB_MASK, ipmask, IPV4_MASK_STR_SIZE + 1);
    ret = get_manager_defaultip_group(&eth0_handle);
    if (ret != VOS_OK) {
        create_message_info(MSGID_INTERNAL_ERR, NULL, &message);
        (void)json_object_array_add(*o_message_jso, message);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    (void)dal_get_property_value_string(eth0_handle, ETH_GROUP_ATTRIBUTE_GATEWAY, ipgateway, IPV4_GATEWAY_STR_SIZE + 1);
    // 1:系统中ip\mask为空时,必须同时设置ip和mask或不设置
    if ((strlen(ipaddr) == 0 || g_strcmp0(ipaddr, "0.0.0.0") == 0) ||
        (strlen(ipmask) == 0 || g_strcmp0(ipmask, "0.0.0.0") == 0)) {
        if (set_ip && !set_mask) {
            json_object_object_del(ip_info, ETH_GROUP_ATTRIBUTE_IP_ADDR); // 删除ip
            create_message_info(MSGID_IP_NOT_EXIST_VALUE_NOT_EMPTY, NULL, &message);
            (void)json_object_array_add(*o_message_jso, message);
            set_ip = 0;
        } else if (!set_ip && set_mask) {
            json_object_object_del(ip_info, ETH_GROUP_ATTRIBUTE_SUB_MASK); // 删除mask
            create_message_info(MSGID_IP_NOT_EXIST_VALUE_NOT_EMPTY, NULL, &message);
            (void)json_object_array_add(*o_message_jso, message);
            set_mask = 0;
        }
    }
    
    if (set_ip && redfish_check_ip(ip, *o_message_jso, address_error_mes, RFPROP_MANAGER_ETH_IPADDR) == VOS_OK) {
        safe_fun_ret = strncpy_s(ipaddr, sizeof(ipaddr), dal_json_object_get_str(ip), json_object_get_string_len(ip));
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
        log_flag = log_flag | 0x01;
    }
    
    if (set_mask && redfish_check_ip(mask, *o_message_jso, mask_error_mes, RFPROP_MANAGER_ETH_IPMASK) == VOS_OK) {
        safe_fun_ret =
            strncpy_s(ipmask, sizeof(ipmask), dal_json_object_get_str(mask), json_object_get_string_len(mask));
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
        log_flag = log_flag | 0x02;
    }
    
    if (log_flag & 0x01) {
        (void)inet_pton(AF_INET, ipaddr, &ip_val);
        ip_val = htonl(ip_val);
        (void)inet_pton(AF_INET, ipmask, &mask_val);
        mask_val = htonl(mask_val);
        if ((ip_val & (~mask_val)) == 0 || ((ip_val | mask_val) == 0xFFFFFFFF) || (mask_val >= (ip_val | mask_val)) ||
            (ip_val & mask_val) == 0) {
            log_flag = log_flag & 0xfc; // ip出错时，mask也认为不合法，需要用户重新输入ip和mask
            (void)create_message_info(MSGID_INVALID_IPV4ADDRESS, address_error_mes, &message, ipaddr,
                address_error_mes);
            (void)json_object_array_add(*o_message_jso, message);
        }
    }
    
    if (log_flag & 0x02) {
        (void)inet_pton(AF_INET, ipaddr, &ip_val);
        ip_val = htonl(ip_val);
        (void)inet_pton(AF_INET, ipmask, &mask_val);
        mask_val = htonl(mask_val);
        (void)inet_pton(AF_INET, ipgateway, &gateway_val);
        gateway_val = htonl(gateway_val);
        if ((ip_val & (~mask_val)) == 0 || ((ip_val | mask_val) == 0xFFFFFFFF) || (mask_val >= (ip_val | mask_val)) ||
            (ip_val & mask_val) == 0) {
            log_flag = log_flag &
                0xfc; // Mask出错时，不允许设置ip。防止ip与mask同时为空的场景下，设置ip和错误mask，mask被删除后，ip设置成功。
            (void)create_message_info(MSGID_INVALID_SUBNETMASK, mask_error_mes, &message, ipmask, mask_error_mes);
            (void)json_object_array_add(*o_message_jso, message);
        }
        if (strlen(ipgateway) != 0) { // 存在gateway时，需要纳入监测
            if ((gateway_val & (~mask_val)) == 0 || ((gateway_val | mask_val) == 0xFFFFFFFF)) {
                log_flag = log_flag &
                    0xfc; // Mask出错时，不允许设置ip。防止ip与mask同时为空的场景下，设置ip和错误mask，mask被删除后，ip设置成功。
                (void)create_message_info(MSGID_INVALID_SUBNETMASK, mask_error_mes, &message, ipmask, mask_error_mes);
                (void)json_object_array_add(*o_message_jso, message);
            }
        }
    }
    
    (void)get_board_active_state(&as_status);
    if (as_status != ASM_ACTIVE) {                                         // 备用
        if (set_gateway) {                                                 // 设置了gateway
            json_object_object_del(ip_info, RFPROP_MANAGER_ETH_IPGATEWAY); // 删除gateway
            (void)create_message_info(MSGID_NOT_ALLOWED_ON_STANDBY_MM, "IPv4Addresses/0/Gateway", &message);
            (void)json_object_array_add(*o_message_jso, message);
            set_gateway = 0;
        }
    }
    
    if (set_gateway &&
        redfish_check_ip(gateway, *o_message_jso, gateway_error_mes, RFPROP_MANAGER_ETH_IPGATEWAY) == VOS_OK) {
        safe_fun_ret = strncpy_s(ipgateway, sizeof(ipgateway), dal_json_object_get_str(gateway),
            json_object_get_string_len(gateway));
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
        log_flag = log_flag | 0x04;
    }
    // 处理DHCP 模式只设置掩码不设置IP
    (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_IP_MODE, &mode);

    if ((log_flag & 02) == 2 && (log_flag & 01) != 1 && IP_DHCP_MODE == mode) {
        do_val_if_expr((log_flag & 02) == 2,
            (void)create_message_info(MSGID_IPV4_MASK_CONFLICT_DHCP, mask_error_mes, &message, mask_error_mes);
            (void)json_object_array_add(*o_message_jso, message));

        response_code = HTTP_BAD_REQUEST;
        return response_code;
    }
    
    return_val_do_info_if_expr(log_flag == 0, response_code, (response_code = HTTP_BAD_REQUEST));
    if (log_flag & 0x01) {
        input_list = NULL;
        input_list = g_slist_append(input_list, g_variant_new_string(ipaddr));
        ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
            ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_METHOD_SET_IPV4_ADDR, AUTH_ENABLE, i_paras->user_role_privilege,
            input_list, NULL);
        if (ret == VOS_OK) {
            response_code = HTTP_OK;
        }
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    }
    if (log_flag & 0x02) {
        input_list = NULL;
        input_list = g_slist_append(input_list, g_variant_new_string(ipmask));
        ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
            ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_METHOD_SET_IPV4_MASK, AUTH_ENABLE, i_paras->user_role_privilege,
            input_list, NULL);
        if (ret == VOS_OK) {
            response_code = HTTP_OK;
        }
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    }
    if (log_flag & 0x04) {
        // 只设置EthGroup0的Gateway
        input_list = NULL;
        input_list = g_slist_append(input_list, g_variant_new_string(ipgateway));
        ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, eth0_handle,
            ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_METHOD_SET_IPV4_DEFAULT_GATEWAY, AUTH_ENABLE,
            i_paras->user_role_privilege, input_list, NULL);
        if (ret == VOS_OK) {
            response_code = HTTP_OK;
        } else {
            (void)create_message_info(MSGID_INVALID_IPV4GATEWAY, gateway_error_mes, &message, ipgateway,
                gateway_error_mes);
            (void)json_object_array_add(*o_message_jso, message);
        }
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    }
    return response_code;
}


LOCAL gint32 redfish_set_ip_mask_gateway(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, OBJ_HANDLE obj_handle)
{
    errno_t safe_fun_ret = EOK;
    gint32 ret;
    gchar ipaddr[IPV4_IP_STR_SIZE + 1] = {0};
    gchar ipmask[IPV4_MASK_STR_SIZE + 1] = {0};
    gchar ipgateway[IPV4_GATEWAY_STR_SIZE + 1] = {0};
    guint8 log_flag = 0;

    json_object *ip_info = json_object_array_get_idx(i_paras->val_jso, 0);
    json_object *ip = NULL;
    json_object *mask = NULL;
    json_object *gateway = NULL;
    gint32 response_code;
    json_object *message = NULL;
    

    const gchar *address_error_mes = "IPv4Addresses/0/Address";
    const gchar *mask_error_mes = "IPv4Addresses/0/SubnetMask";
    const gchar *gateway_error_mes = "IPv4Addresses/0/Gateway";
    guint8 mode = 0;

    
    if (json_object_object_get_ex(ip_info, RFPROP_MANAGER_ETH_IPADDR, &ip) &&
        VOS_OK == redfish_check_ip(ip, *o_message_jso, address_error_mes, RFPROP_MANAGER_ETH_IPADDR)) {
        safe_fun_ret = strncpy_s(ipaddr, sizeof(ipaddr), dal_json_object_get_str(ip), json_object_get_string_len(ip));
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
        log_flag = log_flag | 0x01;
    } else {
        (void)memset_s(ipaddr, sizeof(ipaddr), 0, sizeof(ipaddr));
        (void)dal_get_property_value_string(obj_handle, ETH_GROUP_ATTRIBUTE_IP_ADDR, ipaddr, sizeof(ipaddr));
    }

    
    if (json_object_object_get_ex(ip_info, RFPROP_MANAGER_ETH_IPMASK, &mask) &&
        VOS_OK == redfish_check_ip(mask, *o_message_jso, mask_error_mes, RFPROP_MANAGER_ETH_IPMASK)) {
        safe_fun_ret =
            strncpy_s(ipmask, sizeof(ipmask), dal_json_object_get_str(mask), json_object_get_string_len(mask));
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
        log_flag = log_flag | 0x02;
    } else {
        (void)memset_s(ipmask, sizeof(ipmask), 0, sizeof(ipmask));
        (void)dal_get_property_value_string(obj_handle, ETH_GROUP_ATTRIBUTE_SUB_MASK, ipmask, sizeof(ipmask));
    }

    
    if (json_object_object_get_ex(ip_info, RFPROP_MANAGER_ETH_IPGATEWAY, &gateway) &&
        VOS_OK == redfish_check_ip(gateway, *o_message_jso, gateway_error_mes, RFPROP_MANAGER_ETH_IPGATEWAY)) {
        safe_fun_ret = strncpy_s(ipgateway, sizeof(ipgateway), dal_json_object_get_str(gateway),
            json_object_get_string_len(gateway));
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
        log_flag = log_flag | 0x04;
    } else {
        (void)memset_s(ipgateway, sizeof(ipgateway), 0, sizeof(ipgateway));
        (void)dal_get_property_value_string(obj_handle, ETH_GROUP_ATTRIBUTE_GATEWAY, ipgateway, sizeof(ipgateway));
    }

    // 处理DHCP 模式只设置网关或者掩码
    (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_IP_MODE, &mode);

    if ((log_flag & 01) != 1 && IP_DHCP_MODE == mode) {
        do_val_if_expr((log_flag & 02) == 2,
            (void)create_message_info(MSGID_IPV4_MASK_CONFLICT_DHCP, mask_error_mes, &message, mask_error_mes);
            (void)json_object_array_add(*o_message_jso, message));

        do_val_if_expr((log_flag & 04) == 4, (void)create_message_info(MSGID_IPV4_GATEWAY_CONFLICT_DHCP,
            gateway_error_mes, &message, gateway_error_mes);
            (void)json_object_array_add(*o_message_jso, message));
        response_code = HTTP_BAD_REQUEST;
        return response_code;
    }

    
    return_val_do_info_if_expr(0 == log_flag, response_code, (response_code = HTTP_BAD_REQUEST));
    GSList *input_list = NULL;
    input_list = g_slist_append(input_list, g_variant_new_string(ipaddr));
    input_list = g_slist_append(input_list, g_variant_new_string(ipmask));
    input_list = g_slist_append(input_list, g_variant_new_string(ipgateway));
    input_list = g_slist_append(input_list, g_variant_new_byte(log_flag));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_METHOD_SET_IPV4_MASK_GATEWAY, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, NULL);

    uip_free_gvariant_list(input_list);
    input_list = NULL;

    return_val_do_info_if_expr(VOS_OK == ret, response_code, (response_code = HTTP_OK));

    if ((log_flag & 01) == 1 && (log_flag & 04) != 4) {
        (void)memset_s(ipgateway, sizeof(ipgateway), 0, sizeof(ipgateway));
        safe_fun_ret = strncpy_s(ipgateway, sizeof(ipgateway), ipaddr, sizeof(ipgateway) - 1);
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

        input_list = g_slist_append(input_list, g_variant_new_string(ipaddr));
        input_list = g_slist_append(input_list, g_variant_new_string(ipmask));
        input_list = g_slist_append(input_list, g_variant_new_string(ipgateway));
        input_list = g_slist_append(input_list, g_variant_new_byte(log_flag));

        ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
            ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_METHOD_SET_IPV4_MASK_GATEWAY, AUTH_ENABLE, i_paras->user_role_privilege,
            input_list, NULL);
        uip_free_gvariant_list(input_list);
    }

    response_code = __parse_set_ip_result_code(ret, o_message_jso, log_flag, ipaddr);

    return response_code;
}


gint32 redfish_subnet_mask_check(const gchar *mask)
{
    guint32 i;
    gint32 flag = 0;
    guint32 mask_digit = 0;
    gint32 ret;

    (void)memset_s(&mask_digit, sizeof(mask_digit), 0, sizeof(mask_digit));
    ret = inet_pton(AF_INET, (const char *)mask, (void *)&mask_digit);
    if (1 != ret) {
        return 1;
    }

    mask_digit = htonl(mask_digit);
    
    if (0 == mask_digit) {
        return 1;
    }

    
    for (i = 0; i < 32; i++) {
        if ((mask_digit >> i) & 0x01) {
            flag = 1;
        } else {
            if (flag) {
                return 1;
            }
        }
    }

    return 0;
}


LOCAL gint32 set_manager_physical_ethernet_ipv4_addresses(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint32 result = HTTP_BAD_REQUEST;
    GSList *input_list = NULL;
    guint8 ipmode = 0;                 
    json_object *value_ipaddrs = NULL; 
    json_object *value_ipaddr = NULL;
    json_object *value_ipmode = NULL;
    json_object *value_ipmask = NULL;
    json_object *value_ipgateway = NULL;
    OBJ_HANDLE obj_handle = 0;
    json_object *message = NULL;
    const gchar *ipmode_error_mes = "IPv4Addresses/0/AddressOrigin";
    guint8 ipinfoexpr;
    guint8 software_type = 0;
    guint8 as_status = ASM_ACTIVE;
    obj_handle = i_paras->obj_handle;
    value_ipaddrs = json_object_array_get_idx(i_paras->val_jso, 0);

    (void)dal_get_software_type(&software_type);
    
    if (MGMT_SOFTWARE_TYPE_EM == software_type) {
        if (g_ascii_strcasecmp(dfl_get_object_name(obj_handle), FLOAT_ETHGROUP_OBJ_NAME) == 0) { // 浮动ip不可设置dhcp
            if (json_object_object_get_ex(value_ipaddrs, RFPROP_MANAGER_ETH_IPMODE, &value_ipmode)) {
                json_object_object_del(value_ipaddrs, RFPROP_MANAGER_ETH_IPMODE); // 删除此属性
                (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, "IPv4Addresses/0/AddressOrigin", &message,
                    "IPv4Addresses/0/AddressOrigin");
                (void)json_object_array_add(*o_message_jso, message);
            }
        }

        (void)get_board_active_state(&as_status);
        if (as_status != ASM_ACTIVE) { // 备用板不可设置ipmode
            if (json_object_object_get_ex(value_ipaddrs, RFPROP_MANAGER_ETH_IPMODE, &value_ipmode)) {
                json_object_object_del(value_ipaddrs, RFPROP_MANAGER_ETH_IPMODE); // 删除此属性
                (void)create_message_info(MSGID_NOT_ALLOWED_ON_STANDBY_MM, "IPv4Addresses/0/AddressOrigin", &message);
                (void)json_object_array_add(*o_message_jso, message);
            }
        }
    }
    

    
    
    if (MGMT_SOFTWARE_TYPE_EM == software_type) {
        ipinfoexpr = json_object_object_get_ex(value_ipaddrs, RFPROP_MANAGER_ETH_IPADDR, &value_ipaddr) ||
            json_object_object_get_ex(value_ipaddrs, RFPROP_MANAGER_ETH_IPMASK, &value_ipmask);
    } else {
        ipinfoexpr = json_object_object_get_ex(value_ipaddrs, RFPROP_MANAGER_ETH_IPADDR, &value_ipaddr) ||
            json_object_object_get_ex(value_ipaddrs, RFPROP_MANAGER_ETH_IPMASK, &value_ipmask) ||
            json_object_object_get_ex(value_ipaddrs, RFPROP_MANAGER_ETH_IPGATEWAY, &value_ipgateway);
    }
    

    if (json_object_object_get_ex(value_ipaddrs, RFPROP_MANAGER_ETH_IPMODE, &value_ipmode)) {
        (void)redfish_ipv4mode_stringtonum(value_ipmode, &ipmode);

        if ((IP_DHCP_MODE == ipmode) && ipinfoexpr) {
            (void)create_message_info(MSGID_IPV4_INFO_CONFLICT_DHCP, NULL, &message);
            (void)json_object_array_add(*o_message_jso, message);
            return HTTP_BAD_REQUEST;
        } else {
            input_list = g_slist_append(input_list, g_variant_new_byte(ipmode));
            ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
                ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_METHOD_SET_IPV4_MODE, AUTH_ENABLE, i_paras->user_role_privilege,
                input_list, NULL);
            uip_free_gvariant_list(input_list);

            switch (ret) {
                case VOS_OK:
                    result = HTTP_OK;
                    break;

                    
                case COMP_CODE_INVALID_CMD:

                    result = HTTP_NOT_IMPLEMENTED;
                    (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, ipmode_error_mes, &message,
                        ipmode_error_mes);
                    (void)json_object_array_add(*o_message_jso, message);
                    break;
                    

                case REDFISH_ETH_ERR:
                case REDFISH_ETH_PARAM_NOT_VALIID:
                    (void)create_message_info(MSGID_PROP_NOT_IN_LIST, ipmode_error_mes, &message,
                        dal_json_object_get_str(value_ipmode), ipmode_error_mes);
                    (void)json_object_array_add(*o_message_jso, message);
                    break;

                default:
                    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message);
                    (void)json_object_array_add(*o_message_jso, message);
                    return HTTP_INTERNAL_SERVER_ERROR;
            }
        }
    }

    
    
    
    if (MGMT_SOFTWARE_TYPE_EM == software_type) {
        ret = redfish_smm_set_ip_mask_gateway(i_paras, o_message_jso, obj_handle);
    }
    
    else {
        ret = redfish_set_ip_mask_gateway(i_paras, o_message_jso, obj_handle);
    }
    if (HTTP_OK == ret) {
        result = HTTP_OK;
    } else if (HTTP_OK == result) {
        return HTTP_OK;
    }

    
    return result;
}


LOCAL gint32 delete_float_ip(PROVIDER_PARAS_S *i_paras)
{
    gint32 ret;
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    guint8 action = DISABLE_ETH;

    if (i_paras == NULL) {
        return VOS_ERR;
    }

    ret = dal_get_specific_object_byte(ETH_CLASS_NAME_ETH, ETH_ATTRIBUTE_OUT_TYPE, FLOATIP_ETHERNET, &obj_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s(%d):find floatip eth object failed.\r\n", __func__, __LINE__);
        return VOS_ERR;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)action));
    ret = uip_call_class_method(dal_rf_get_op_log_module_name(i_paras->is_from_webui), i_paras->user_name,
        i_paras->client, ETH_CLASS_NAME_ETH, dfl_get_object_name(obj_handle), ETH_METHOD_SET_ETH_ENABLE, input_list,
        NULL);
    uip_free_gvariant_list(input_list);
    return ret;
}


LOCAL gint32 need_delete_ip_mask(json_object *array, int is_ipv4)
{
    gint32 array_len;
    json_object *item = NULL;
    json_object *json_ipv6 = NULL;
    json_bool set_ipv6 = 0;
    json_object *json_ipv4 = NULL;
    json_bool set_ipv4 = 0;

    return_val_do_info_if_expr((NULL == array), FALSE,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    guint8 software_type = 0;
    (void)dal_get_software_type(&software_type);
    if (MGMT_SOFTWARE_TYPE_EM != software_type) {
        return FALSE;
    }

    return_val_do_info_if_expr(json_type_array != json_object_get_type(array), FALSE,
        debug_log(DLOG_ERROR, "%s failed:is not json array", __FUNCTION__));

    array_len = json_object_array_length(array);

    // 空数组返回无有效成员
    return_val_do_info_if_expr(0 == array_len, FALSE, debug_log(DLOG_ERROR, "%s failed: array_len is 0", __FUNCTION__));

    // 成员不是对象，退出
    item = json_object_array_get_idx(array, 0);
    return_val_do_info_if_expr(json_type_object != json_object_get_type(item), FALSE,
        debug_log(DLOG_ERROR, "%s failed:is not json array", __FUNCTION__));

    if (is_ipv4) {
        set_ipv4 = json_object_object_get_ex(item, RFPROP_MANAGER_ETH_IPADDR, &json_ipv4);
        if (set_ipv4 && json_ipv4 == NULL) {
            return TRUE;
        }
    } else {
        set_ipv6 = json_object_object_get_ex(item, RFPROP_MANAGER_ETH_IPADDR, &json_ipv6);
        if (set_ipv6 && json_ipv6 == NULL) {
            return TRUE;
        }
        if (set_ipv6 && strlen(dal_json_object_get_str(json_ipv6)) == 0) {
            return TRUE;
        }
    }

    return FALSE;
}

LOCAL gint32 get_ethgroup0_ip_version(guint8 *ipver)
{
    gint32 ret;
    OBJ_HANDLE out_obj_handle = 0;

    ret = get_manager_defaultip_group(&out_obj_handle);
    return_val_do_info_if_expr(ret != VOS_OK, ret,
        debug_log(DLOG_ERROR, "[%s] Get object[%s] failed, err code %d", __FUNCTION__, OUT_ETHGROUP_OBJ_NAME, ret));
    // EMM 的IP版本统一以EthGroup0的为准，当IP版本是IPv4的时候，不能设置IPv6地址
    (void)dal_get_property_value_byte(out_obj_handle, ETH_GROUP_ATTRIBUTE_IP_VERSION, ipver);

    return VOS_OK;
}


LOCAL gint32 set_manager_ethernetinterface_ipv4_addresses(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint8 ipver = 0;
    const gchar *ipv4_info = "IPv4Addresses";
    const gchar *ipv4 = "IPv4";
    guint8 software_type = 0;

    
    return_val_do_info_if_expr((VOS_OK != provider_paras_check(i_paras)) || (NULL == o_message_jso),
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_MANAGER_ETH_IPV4ADDR, o_message_jso,
        RFPROP_MANAGER_ETH_IPV4ADDR); dal_proxy_operation_log(MODULE_NAME_REDFISH, i_paras->user_name,
            i_paras->client, ETH_CLASS_NAME_ETHGROUP, "Set the ipv4 addresses failed"));
    
    obj_handle = i_paras->obj_handle;

    // 当IP版本是IPv6的时候，不能设置IPv4地址
    
    // EMM 的IP版本统一以EthGroup0的为准，当IP版本是IPv6的时候，不能设置IPv4地址
    (void)dal_get_software_type(&software_type);
    if (MGMT_SOFTWARE_TYPE_EM == software_type) {
        return_val_do_info_if_expr(VOS_OK != get_ethgroup0_ip_version(&ipver), HTTP_INTERNAL_SERVER_ERROR,
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    }
    
    else {
        (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_IP_VERSION, &ipver);
    }
    return_val_do_info_if_expr(ENABLE_IPV6_ONLY == ipver, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_MODIFY_FAILED_WITH_IPVER, ipv4_info, o_message_jso, ipv4_info, ipv4));

    ret = rf_check_array_length_valid(i_paras->val_jso, 1, o_message_jso, RFPROP_MANAGER_ETH_IPV4ADDR);
    return_val_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST);

    if (need_delete_ip_mask(i_paras->val_jso, 1)) {
        ret = delete_float_ip(i_paras);
        if (ret == VOS_OK) {
            debug_log(DLOG_ERROR, "%s: delete float IPv4 address success!", __FUNCTION__);
            return HTTP_OK;
        }
        return HTTP_BAD_REQUEST;
    }

    *o_message_jso = json_object_new_array();

    
    ret = set_manager_physical_ethernet_ipv4_addresses(i_paras, o_message_jso, o_result_jso);

    return ret;
}


LOCAL gint32 redfish_ipv6mode_stringtonum(json_object *ipmodestring, guint8 *ipmode)
{
    if (g_strcmp0(dal_json_object_get_str(ipmodestring), IP_MODE_STATIC_STRING) == 0) {
        *ipmode = IP_STATIC_MODE;
    } else if (g_strcmp0(dal_json_object_get_str(ipmodestring), IPV6_MODE_DHCP) == 0) {
        *ipmode = IP_DHCP_MODE;
    } else {
        *ipmode = 0;
    } 

    return VOS_OK;
}


LOCAL gint32 delete_float_ipv6(PROVIDER_PARAS_S *i_paras)
{
    gchar ipv6_addr[INET6_ADDRSTRLEN] = {0};
    gint32 ret;
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle = 0;

    if (i_paras == NULL) {
        return VOS_ERR;
    }

    ret = dal_get_specific_object_byte(ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_ATTRIBUTE_OUT_TYPE, FLOATIP_ETHERNET,
        &obj_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s(%d):find floatip ethgroup failed.", __func__, __LINE__);
        return VOS_ERR;
    }

    input_list = g_slist_append(input_list, g_variant_new_string((const char *)ipv6_addr));
    ret = uip_call_class_method(dal_rf_get_op_log_module_name(i_paras->is_from_webui), i_paras->user_name,
        i_paras->client, ETH_CLASS_NAME_ETHGROUP, dfl_get_object_name(obj_handle), ETH_GROUP_METHOD_SET_IPV6_ADDR,
        input_list, NULL);
    uip_free_gvariant_list(input_list);
    return ret;
}


LOCAL gint32 set_ipv6_prefix(PROVIDER_PARAS_S *i_paras, json_object *value_ipv6prefix, OBJ_HANDLE obj_handle,
    json_object **o_message_jso)
{
    gint32 ret;
    gint32 json_int;
    GSList *input_list = NULL;
    json_object *message = NULL;
    const gchar *prefix_error_mes = "IPv6Addresses/0/PrefixLength";
    const gchar *address_error_mes = "IPv6Addresses/0/Address";
    gint32 result = HTTP_BAD_REQUEST;

    
    json_int = json_object_get_int(value_ipv6prefix);
    return_val_do_info_if_expr(json_int < 0 || json_int > G_MAXUINT8, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, prefix_error_mes, &message,
        dal_json_object_get_str(value_ipv6prefix), prefix_error_mes);
        (void)json_object_array_add(*o_message_jso, message));
    

    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)json_int));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_METHOD_SET_IPV6_PREFIX, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, NULL);
    uip_free_gvariant_list(input_list);
    return_val_if_expr(VOS_OK == ret, HTTP_OK);

    
    return_val_do_info_if_expr((COMP_CODE_INVALID_CMD == ret), HTTP_NOT_IMPLEMENTED,
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, prefix_error_mes, &message, prefix_error_mes);
        (void)json_object_array_add(*o_message_jso, message));
    

    
    return_val_do_info_if_expr(REDFISH_ETH_PARAM_NOT_VALIID == ret, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_MODIFY_FAILED_EMPTY_ADDRESS, prefix_error_mes, &message, prefix_error_mes,
        address_error_mes);
        (void)json_object_array_add(*o_message_jso, message));
    return_val_do_info_if_fail((VOS_OK == ret) || (REDFISH_ETH_PARAM_NOT_VALIID == ret), HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message);
        (void)json_object_array_add(*o_message_jso, message));
    
    return result;
}


LOCAL gint32 set_manager_ethernetinterface_ipv6_addresses(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint8 ipv6mode = 0;
    GSList *input_list = NULL;
    json_object *value_ipv6addrs = NULL;
    json_object *value_ipv6addr = NULL;
    json_object *value_ipv6origin = NULL;
    json_object *value_ipv6prefix = NULL;
    guint8 ipver = 0;
    gchar ipv6_gateway[IPV6_GATEWAY_STR_SIZE + 1] = {0};
    json_object *message = NULL;
    gint32 result = HTTP_BAD_REQUEST;
    const gchar *json_str = NULL;
    const gchar *prefix_error_mes = "IPv6Addresses/0/PrefixLength";
    const gchar *address_error_mes = "IPv6Addresses/0/Address";
    const gchar *ipmode_error_mes = "IPv6Addresses/0/AddressOrigin";
    const gchar *ipv6_info = "IPv6Addresses";
    const gchar *ipv6 = "IPv6";
    guint8 software_type = 0;
    guint8 as_status = ASM_ACTIVE;

    return_val_do_info_if_expr((provider_paras_check(i_paras) != VOS_OK || o_message_jso == NULL),
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_MANAGER_ETH_IPV6_ADDR, o_message_jso,
        RFPROP_MANAGER_ETH_IPV6_ADDR));
    
    obj_handle = i_paras->obj_handle;

    
    (void)dal_get_software_type(&software_type);
    if (MGMT_SOFTWARE_TYPE_EM == software_type) {
        return_val_do_info_if_expr(VOS_OK != get_ethgroup0_ip_version(&ipver), HTTP_INTERNAL_SERVER_ERROR,
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    } 
    else {
        
        // 当IP版本是IPv4的时候，不能设置IPv6地址
        (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_IP_VERSION, &ipver);
    }
    return_val_do_info_if_expr(ENABLE_IPV4_ONLY == ipver, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_MODIFY_FAILED_WITH_IPVER, ipv6_info, o_message_jso, ipv6_info, ipv6));

    

    
    ret = rf_check_array_length_valid(i_paras->val_jso, 1, o_message_jso, RFPROP_MANAGER_ETH_IPV6_ADDR);
    return_val_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST);
    

    do_info_if_true(need_delete_ip_mask(i_paras->val_jso, 0) == TRUE,
        return_val_do_info_if_expr(delete_float_ipv6(i_paras) == VOS_OK, HTTP_OK,
        debug_log(DLOG_ERROR, "%s: delete float IPv6 address success!", __FUNCTION__));
        return HTTP_BAD_REQUEST);

    value_ipv6addrs = json_object_array_get_idx(i_paras->val_jso, 0);
    return_val_do_info_if_expr(NULL == value_ipv6addrs, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_TYPE_ERR, RFPROP_MANAGER_ETH_IPV6_ADDR, o_message_jso, RF_VALUE_NULL,
        RFPROP_MANAGER_ETH_IPV6_ADDR));
    *o_message_jso = json_object_new_array();
    
    if (MGMT_SOFTWARE_TYPE_EM == software_type) {
        if (g_ascii_strcasecmp(dfl_get_object_name(obj_handle), FLOAT_ETHGROUP_OBJ_NAME) == 0) { // 浮动ip不可设置dhcp
            // 删除此属性
            do_info_if_true(json_object_object_get_ex(value_ipv6addrs, RFPROP_MANAGER_ETH_IPMODE, &value_ipv6origin),
                json_object_object_del(value_ipv6addrs, RFPROP_MANAGER_ETH_IPMODE);
                (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, "IPv6Addresses/0/AddressOrigin", &message,
                "IPv6Addresses/0/AddressOrigin");
                (void)json_object_array_add(*o_message_jso, message));
        }

        (void)get_board_active_state(&as_status);
        // 备用板不可设置ipmode, 删除此属性
        do_info_if_true((as_status != ASM_ACTIVE &&
            json_object_object_get_ex(value_ipv6addrs, RFPROP_MANAGER_ETH_IPMODE, &value_ipv6origin)),
            json_object_object_del(value_ipv6addrs, RFPROP_MANAGER_ETH_IPMODE);
            (void)create_message_info(MSGID_NOT_ALLOWED_ON_STANDBY_MM, "IPv6Addresses/0/AddressOrigin", &message);
            (void)json_object_array_add(*o_message_jso, message));
    }

    
    if (json_object_object_get_ex(value_ipv6addrs, RFPROP_MANAGER_ETH_IPMODE, &value_ipv6origin)) {
        (void)redfish_ipv6mode_stringtonum(value_ipv6origin, &ipv6mode);
        return_val_do_info_if_expr((ipv6mode == IP_DHCP_MODE) &&
            (json_object_object_get_ex(value_ipv6addrs, RFPROP_MANAGER_ETH_IPADDR, &value_ipv6addr) ||
            json_object_object_get_ex(value_ipv6addrs, RFPROP_MANAGER_ETH_IPV6_PREFIXLEN, &value_ipv6prefix)),
            HTTP_BAD_REQUEST, (void)create_message_info(MSGID_IPV6_INFO_CONFLICT_DHCPV6, NULL, &message);
            (void)json_object_array_add(*o_message_jso, message););

        input_list = g_slist_append(input_list, g_variant_new_byte(ipv6mode));
        ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
            ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_METHOD_SET_IPV6_MODE, AUTH_ENABLE, i_paras->user_role_privilege,
            input_list, NULL);
        uip_free_gvariant_list(input_list);
        input_list = NULL;
        
        return_val_do_info_if_expr((COMP_CODE_INVALID_CMD == ret), HTTP_NOT_IMPLEMENTED,
            (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, ipmode_error_mes, &message, ipmode_error_mes);
            (void)json_object_array_add(*o_message_jso, message));
        

        do_info_if_true(REDFISH_ETH_PARAM_NOT_VALIID == ret, (void)create_message_info(MSGID_PROP_NOT_IN_LIST,
            ipmode_error_mes, &message, dal_json_object_get_str(value_ipv6origin), ipmode_error_mes);
            (void)json_object_array_add(*o_message_jso, message); goto break_ipmode);

        switch (ret) {
            case VOS_OK:
                result = HTTP_OK;
                break;

            default: 
                (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message);
                (void)json_object_array_add(*o_message_jso, message);
                return HTTP_INTERNAL_SERVER_ERROR;
        }
    }

break_ipmode:

    
    if (json_object_object_get_ex(value_ipv6addrs, RFPROP_MANAGER_ETH_IPADDR, &value_ipv6addr)) {
        do_info_if_true(value_ipv6addr == NULL, result = ((result == HTTP_OK) ? HTTP_OK : HTTP_BAD_REQUEST);
            (void)create_message_info(MSGID_PROP_TYPE_ERR, address_error_mes, &message, RF_VALUE_NULL,
            address_error_mes);
            (void)json_object_array_add(*o_message_jso, message); goto break_ipaddr);
        
        do_info_if_true(0 == json_object_get_string_len(value_ipv6addr),
            result = ((result == HTTP_OK) ? HTTP_OK : HTTP_BAD_REQUEST);
            (void)create_message_info(MSGID_EMPTY_IPV6_ADDRESS, address_error_mes, &message, address_error_mes);
            (void)json_object_array_add(*o_message_jso, message); goto break_ipaddr);
        
        
        json_str = dal_json_object_get_str(value_ipv6addr);
        do_info_if_true(json_str == NULL, result = ((result == HTTP_OK) ? HTTP_OK : HTTP_BAD_REQUEST);
            (void)create_message_info(MSGID_PROP_TYPE_ERR, address_error_mes, &message, RF_VALUE_NULL,
            address_error_mes);
            (void)json_object_array_add(*o_message_jso, message); goto break_ipaddr);

        (void)dal_get_property_value_string(obj_handle, ETH_GROUP_ATTRIBUTE_IPV6_GATEWAY, ipv6_gateway,
            sizeof(ipv6_gateway));
        do_info_if_true(0 == g_strcmp0(json_str, ipv6_gateway),
            result = ((result == HTTP_OK) ? HTTP_OK : HTTP_BAD_REQUEST);
            (void)create_message_info(MSGID_IPV6_ADDRESS_CONFLICT_GATEWAY, address_error_mes, &message, json_str,
            address_error_mes, RFPROP_MANAGER_ETH_IPV6_GATEWAY);
            (void)json_object_array_add(*o_message_jso, message); goto break_ipaddr);
        
        input_list = g_slist_append(input_list, g_variant_new_string(json_str));
        ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
            ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_METHOD_SET_IPV6_ADDR, AUTH_ENABLE, i_paras->user_role_privilege,
            input_list, NULL);
        uip_free_gvariant_list(input_list);
        input_list = NULL;

        
        return_val_do_info_if_expr((COMP_CODE_INVALID_CMD == ret), HTTP_NOT_IMPLEMENTED,
            (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, address_error_mes, &message, address_error_mes);
            (void)json_object_array_add(*o_message_jso, message));
        

        do_info_if_true(REDFISH_ETH_PARAM_NOT_VALIID == ret,
            result = ((result == HTTP_OK) ? HTTP_OK : HTTP_BAD_REQUEST);
            (void)create_message_info(MSGID_INVALID_IPV6ADDRESS, address_error_mes, &message, json_str,
            address_error_mes);
            (void)json_object_array_add(*o_message_jso, message); goto break_ipaddr);

        switch (ret) {
            case VOS_OK:
                result = HTTP_OK;
                break;

            default:
                (void)create_message_info(MSGID_GENERAL_ERROR, NULL, &message);
                (void)json_object_array_add(*o_message_jso, message);
                return HTTP_INTERNAL_SERVER_ERROR;
        }
    }

break_ipaddr:

    
    if (json_object_object_get_ex(value_ipv6addrs, RFPROP_MANAGER_ETH_IPV6_PREFIXLEN, &value_ipv6prefix)) {
        do_info_if_true(json_type_int != json_object_get_type(value_ipv6prefix),
            result = (HTTP_OK == result) ? HTTP_OK : HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_PROP_TYPE_ERR, prefix_error_mes, &message,
            dal_json_object_to_json_string(value_ipv6prefix), prefix_error_mes);
            (void)json_object_array_add(*o_message_jso, message); goto break_prefix);
        
        ipv6mode = 0;
        (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_IPV6_MODE, &ipv6mode);
        
        goto_label_do_info_if_expr(IP_DHCP_MODE == ipv6mode, break_prefix,
            result = ((HTTP_OK == result) ? HTTP_OK : HTTP_BAD_REQUEST);
            (void)create_message_info(MSGID_IPV6_PREFIX_CONFLICT_DHCPV6, prefix_error_mes, &message,
            prefix_error_mes);
            (void)json_object_array_add(*o_message_jso, message));
        

        result = set_ipv6_prefix(i_paras, value_ipv6prefix, obj_handle, o_message_jso);
    }

break_prefix:
    return result;
}

LOCAL gint32 set_manager_ethernetinterface_ipv6_static_addresses(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle;
    gint32 json_int = 0;
    const gchar *json_str = NULL;
    json_object *ipv6_static = NULL;
    json_object *ipv6_static_address = NULL;
    json_object *ipv6_static_prefix = NULL;
    GSList *input_list = NULL;
    gint32 result = HTTP_BAD_REQUEST;
    json_object *message = NULL;
    guint8 ipv6mode = 0;
    const gchar *prefix_error_mes = "IPv6StaticAddresses/0/PrefixLength";
    const gchar *address_error_mes = "IPv6StaticAddresses/0/Address";
    const gchar *ipv6_static_error = "IPv6StaticAddresses";
    gchar ipv6_gateway[IPV6_GATEWAY_STR_SIZE] = {0};
    guint8 ipver = 0;
    const gchar *ipv6 = "IPv6";

    return_val_do_info_if_expr((provider_paras_check(i_paras) != VOS_OK || o_message_jso == NULL),
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_MANAGER_ETH_IPV6_STATIC_ADDR, o_message_jso,
        RFPROP_MANAGER_ETH_IPV6_STATIC_ADDR));

    
    ret = rf_check_array_length_valid(i_paras->val_jso, 1, o_message_jso, RFPROP_MANAGER_ETH_IPV6_STATIC_ADDR);
    return_val_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST);
    

    
    obj_handle = i_paras->obj_handle;

    
    // 当IP版本是IPv4的时候，不能设置IPv6地址

    (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_IP_VERSION, &ipver);
    return_val_do_info_if_expr(ENABLE_IPV4_ONLY == ipver, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_MODIFY_FAILED_WITH_IPVER, ipv6_static_error, o_message_jso,
        ipv6_static_error, ipv6));

    

    ipv6_static = json_object_array_get_idx(i_paras->val_jso, 0);

    
    return_val_do_info_if_expr(NULL == ipv6_static, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_TYPE_ERR, RFPROP_MANAGER_ETH_IPV6_STATIC_ADDR, o_message_jso,
        RF_VALUE_NULL, RFPROP_MANAGER_ETH_IPV6_STATIC_ADDR));
    *o_message_jso = json_object_new_array();
    

    if (json_object_object_get_ex(ipv6_static, RFPROP_MANAGER_ETH_IPADDR, &ipv6_static_address)) {
        do_info_if_true(ipv6_static_address == NULL, (void)create_message_info(MSGID_PROP_TYPE_ERR, address_error_mes,
            &message, RF_VALUE_NULL, address_error_mes);
            (void)json_object_array_add(*o_message_jso, message); goto break_static_ipaddr);
        
        do_info_if_true(0 == json_object_get_string_len(ipv6_static_address),
            result = ((result == HTTP_OK) ? HTTP_OK : HTTP_BAD_REQUEST);
            (void)create_message_info(MSGID_EMPTY_IPV6_ADDRESS, address_error_mes, &message, address_error_mes);
            (void)json_object_array_add(*o_message_jso, message); goto break_static_ipaddr);
        

        
        json_str = dal_json_object_get_str(ipv6_static_address);
        do_info_if_true(NULL == json_str, result = ((result == HTTP_OK) ? HTTP_OK : HTTP_BAD_REQUEST);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message);
            (void)json_object_array_add(*o_message_jso, message); goto break_static_ipaddr);
        

        
        (void)dal_get_property_value_string(obj_handle, ETH_GROUP_ATTRIBUTE_IPV6_GATEWAY, ipv6_gateway,
            sizeof(ipv6_gateway));
        do_info_if_true(0 == g_strcmp0(json_str, ipv6_gateway),
            result = ((result == HTTP_OK) ? HTTP_OK : HTTP_BAD_REQUEST);
            (void)create_message_info(MSGID_IPV6_ADDRESS_CONFLICT_GATEWAY, address_error_mes, &message, json_str,
            address_error_mes, RFPROP_MANAGER_ETH_IPV6_GATEWAY);
            (void)json_object_array_add(*o_message_jso, message); goto break_static_ipaddr);
        
        input_list = g_slist_append(input_list, g_variant_new_string(json_str));
        ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
            ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_METHOD_SET_IPV6_ADDR, AUTH_ENABLE, i_paras->user_role_privilege,
            input_list, NULL);
        uip_free_gvariant_list(input_list);
        input_list = NULL;

        
        do_info_if_true((COMP_CODE_INVALID_CMD == ret), (result = HTTP_NOT_IMPLEMENTED);
            (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, address_error_mes, &message, address_error_mes);
            (void)json_object_array_add(*o_message_jso, message); goto break_static_ipaddr);
        

        do_info_if_true(REDFISH_ETH_PARAM_NOT_VALIID == ret,
            result = ((result == HTTP_OK) ? HTTP_OK : HTTP_BAD_REQUEST);
            (void)create_message_info(MSGID_INVALID_IPV6ADDRESS, address_error_mes, &message, json_str,
            address_error_mes);
            (void)json_object_array_add(*o_message_jso, message); goto break_static_ipaddr);

        switch (ret) {
            case VOS_OK:
                result = HTTP_OK;
                break;

            default:
                (void)create_message_info(MSGID_GENERAL_ERROR, NULL, &message);
                (void)json_object_array_add(*o_message_jso, message);
                return HTTP_INTERNAL_SERVER_ERROR;
        }
    }

break_static_ipaddr:

    if (json_object_object_get_ex(ipv6_static, RFPROP_MANAGER_ETH_IPV6_PREFIXLEN, &ipv6_static_prefix)) {
        do_info_if_true(NULL == ipv6_static_prefix, result = (HTTP_OK == result) ? HTTP_OK : HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_PROP_TYPE_ERR, prefix_error_mes, &message, RF_VALUE_NULL, prefix_error_mes);
            (void)json_object_array_add(*o_message_jso, message); goto break_static_ipprefix);
        goto_label_do_info_if_expr(json_type_double == json_object_get_type(ipv6_static_prefix), break_static_ipprefix,
            result = (HTTP_OK == result) ? HTTP_OK : HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_PROP_TYPE_ERR, prefix_error_mes, &message,
            dal_json_object_to_json_string(ipv6_static_prefix), prefix_error_mes);
            (void)json_object_array_add(*o_message_jso, message));
        
        (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_IPV6_MODE, &ipv6mode);
        
        goto_label_do_info_if_expr(IP_DHCP_MODE == ipv6mode, break_static_ipprefix,
            result = ((HTTP_OK == result) ? HTTP_OK : HTTP_BAD_REQUEST);
            (void)create_message_info(MSGID_IPV6_PREFIX_CONFLICT_DHCPV6, prefix_error_mes, &message,
            prefix_error_mes);
            (void)json_object_array_add(*o_message_jso, message));
        

        
        json_int = json_object_get_int(ipv6_static_prefix);
        goto_label_do_info_if_expr(json_int < 0 || json_int > G_MAXUINT8, break_static_ipprefix,
            result = (HTTP_OK == result) ? HTTP_OK : HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_PROP_NOT_IN_LIST, prefix_error_mes, &message,
            dal_json_object_to_json_string(ipv6_static_prefix), prefix_error_mes);
            (void)json_object_array_add(*o_message_jso, message));
        

        input_list = g_slist_append(input_list, g_variant_new_byte((guint8)json_int));
        ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
            ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_METHOD_SET_IPV6_PREFIX, AUTH_ENABLE, i_paras->user_role_privilege,
            input_list, NULL);
        uip_free_gvariant_list(input_list);

        switch (ret) {
            case VOS_OK:
                result = HTTP_OK;
                break;

                
            case COMP_CODE_INVALID_CMD:
                result = HTTP_NOT_IMPLEMENTED;
                (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, prefix_error_mes, &message,
                    prefix_error_mes);
                (void)json_object_array_add(*o_message_jso, message);
                break;
                

                
            case REDFISH_ETH_PARAM_NOT_VALIID:
                (void)create_message_info(MSGID_MODIFY_FAILED_EMPTY_ADDRESS, prefix_error_mes, &message,
                    prefix_error_mes, address_error_mes);
                (void)json_object_array_add(*o_message_jso, message);
                break;

            default: 
                (void)create_message_info(MSGID_GENERAL_ERROR, NULL, &message);
                (void)json_object_array_add(*o_message_jso, message);
                
                return HTTP_INTERNAL_SERVER_ERROR;
        }
    }

break_static_ipprefix:
    return result;
}

LOCAL gint32 set_manager_ethernetinterface_ipv6_defaultgateway(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    guint8 ipv6mode = 0;
    gchar ipv6_addr[IPV6_IP_STR_SIZE + 1] = {0};
    const gchar *ipv6_address_mes = "IPv6Addresses/0/Address";
    guint8 ipver = 0;
    const gchar *ipv6 = "IPv6";
    guint8 as_status = 0;
    guint8 software_type = 0;

    if (provider_paras_check(i_paras) != VOS_OK || o_message_jso == NULL) {
        debug_log(DLOG_ERROR, "%s%d NULL Pointer!", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_MANAGER_ETH_IPV6_GATEWAY, o_message_jso,
        RFPROP_MANAGER_ETH_IPV6_GATEWAY));


    (void)dal_get_software_type(&software_type);
    if (MGMT_SOFTWARE_TYPE_EM == software_type) {
        ret = get_manager_defaultip_group(&obj_handle);
        if (VOS_OK != ret) {
            debug_log(DLOG_ERROR, "[%s] Get object[%s] failed, err code %d", __func__, OUT_ETHGROUP_OBJ_NAME, ret);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
    } else {
        obj_handle = i_paras->obj_handle;
    }
    

    
    // 当IP版本是IPv4的时候，不能设置IPv6地址

    (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_IP_VERSION, &ipver);
    return_val_do_info_if_expr(ENABLE_IPV4_ONLY == ipver, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_MODIFY_FAILED_WITH_IPVER, RFPROP_MANAGER_ETH_IPV6_GATEWAY, o_message_jso,
        RFPROP_MANAGER_ETH_IPV6_GATEWAY, ipv6));

    

    
    return_val_do_info_if_expr(dal_json_object_get_str(i_paras->val_jso) == NULL, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_TYPE_ERR, RFPROP_MANAGER_ETH_IPV6_GATEWAY, o_message_jso, RF_VALUE_NULL,
        RFPROP_MANAGER_ETH_IPV6_GATEWAY));
    
    if (MGMT_SOFTWARE_TYPE_EM == software_type) {
        
        get_board_active_state(&as_status);
        if (as_status != ASM_ACTIVE) { // 备用
            (void)create_message_info(MSGID_NOT_ALLOWED_ON_STANDBY_MM, "IPv6DefaultGateway", o_message_jso);
            return HTTP_BAD_REQUEST;
        }
    }

    
    (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_IPV6_MODE, &ipv6mode);


    if (MGMT_SOFTWARE_TYPE_EM != software_type) {
        return_val_do_info_if_expr(IP_DHCP_MODE == ipv6mode, HTTP_BAD_REQUEST,
            (void)create_message_info(MSGID_IPV6_GATEWAY_CONFLICT_DHCPV6, RFPROP_MANAGER_ETH_IPV6_GATEWAY,
            o_message_jso, RFPROP_MANAGER_ETH_IPV6_GATEWAY));
    }
    
    
    
    (void)dal_get_property_value_string(obj_handle, ETH_GROUP_ATTRIBUTE_IPV6_ADDR, ipv6_addr, sizeof(ipv6_addr));
    return_val_do_info_if_expr(0 == g_strcmp0(dal_json_object_get_str(i_paras->val_jso), ipv6_addr), HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_IPV6_ADDRESS_CONFLICT_GATEWAY, RFPROP_MANAGER_ETH_IPV6_GATEWAY,
        o_message_jso, dal_json_object_get_str(i_paras->val_jso), RFPROP_MANAGER_ETH_IPV6_GATEWAY, ipv6_address_mes));


    if (MGMT_SOFTWARE_TYPE_EM != software_type) {
        
        return_val_do_info_if_fail(strlen(ipv6_addr), HTTP_BAD_REQUEST,
            (void)create_message_info(MSGID_MODIFY_FAILED_EMPTY_ADDRESS, RFPROP_MANAGER_ETH_IPV6_GATEWAY,
            o_message_jso, RFPROP_MANAGER_ETH_IPV6_GATEWAY, ipv6_address_mes));
        
    }
    
    input_list = g_slist_append(input_list, g_variant_new_string(dal_json_object_get_str(i_paras->val_jso)));
    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_METHOD_SET_IPV6_DEFAULT_GATEWAY, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
            break;

            
        case COMP_CODE_INVALID_CMD:
            (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, RFPROP_MANAGER_ETH_IPV6_GATEWAY, o_message_jso,
                RFPROP_MANAGER_ETH_IPV6_GATEWAY);
            return HTTP_NOT_IMPLEMENTED;
            

        case (guint8)REDFISH_ETH_PARAM_NOT_VALIID:

        default:
            (void)create_message_info(MSGID_INVALID_IPV6GATEWAY, RFPROP_MANAGER_ETH_IPV6_GATEWAY, o_message_jso,
                dal_json_object_get_str(i_paras->val_jso), RFPROP_MANAGER_ETH_IPV6_GATEWAY);
            return HTTP_BAD_REQUEST;
    }

    return HTTP_OK;
}


LOCAL gint32 redfish_set_nameserver(OBJ_HANDLE obj_handle, guchar from_webui_flag, gchar *user_name, gchar *client,
    json_object *item, json_object *o_message_jso, gint32 i)
{
    int iRet;
    const gchar *method_name = METHOD_DNSSETTING_PRIMARYDOMAIN;
    GSList *input_list = NULL;
    gint32 ret;
    gchar json_pointer[PROP_VAL_LENGTH] = {0};
    json_object *message = NULL;

    return_val_do_info_if_expr((user_name == NULL) || (client == NULL) || (o_message_jso == NULL) || (item == NULL),
        VOS_ERR, debug_log(DLOG_ERROR, "%s : NULL pointer", __FUNCTION__));

    // 失败不影响程序执行
    iRet = snprintf_s(json_pointer, sizeof(json_pointer), sizeof(json_pointer) - 1, "%s/%d",
        RFPROP_MANAGER_ETH_NAMESERVER, i);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    if (i == ALTERNATE_DNS_SERVER_INDEX) {
        method_name = METHOD_DNSSETTING_BACKUPDOMAIN;
    } else if (i == TERTIARY_DNS_SERVER_INDEX) {
        method_name = METHOD_DNSSETTING_TERTIARYDOMAIN;
    }

    
    input_list = g_slist_append(input_list, g_variant_new_string(dal_json_object_get_str(item)));
    ret = uip_call_class_method_redfish(from_webui_flag, user_name, client, obj_handle, CLASS_NAME_DNSSETTING,
        method_name, AUTH_DISABLE, 0, input_list, NULL);
    uip_free_gvariant_list(input_list);
    

    switch (ret) {
        case VOS_OK:
            return VOS_OK;

            
        case COMP_CODE_INVALID_CMD:
            (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, json_pointer, &message, json_pointer);
            (void)json_object_array_add(o_message_jso, message);
            break;
            

        default: 
            (void)create_message_info(MSGID_INVALID_NAMESERVER, json_pointer, &message,
                dal_json_object_get_str(item), json_pointer);
            (void)json_object_array_add(o_message_jso, message);
    }

    return VOS_ERR;
}

LOCAL gint32 set_manager_ethernetinterface_nameserver(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint32 ret;
    OBJ_HANDLE obj_handle = 0;
    json_object *item = NULL;
    gint32 result = HTTP_BAD_REQUEST;
    guint8 dns_mode = 0;
    gint32 i;
    gint32 array_len;

    return_val_do_info_if_expr((provider_paras_check(i_paras) != VOS_OK) || (o_message_jso == NULL),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s : NULL pointer", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_MANAGER_ETH_NAMESERVER, o_message_jso,
        RFPROP_MANAGER_ETH_NAMESERVER));

    
    array_len = json_object_array_length(i_paras->val_jso);
    return_val_do_info_if_expr(array_len == 0, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_ARRAY_REQUIRED_ITEM, RFPROP_MANAGER_ETH_NAMESERVER, o_message_jso,
        RFPROP_MANAGER_ETH_NAMESERVER));

    // 长度限制3
    return_val_do_info_if_expr(array_len > DNS_SERVER_CNT, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_ARRAY_ITEM_EXCEED, RFPROP_MANAGER_ETH_NAMESERVER, o_message_jso,
        RFPROP_MANAGER_ETH_NAMESERVER));

    

    
    (void)dal_get_object_handle_nth(CLASS_NAME_DNSSETTING, 0, &obj_handle);

    
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_DNSSETTING_DNSMODE, &dns_mode);

    
    return_val_do_info_if_expr(DNS_ADD_ORIGIN_AUTO == dns_mode, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_MODIFY_FAILED_WITH_AUTO_DNS, RFPROP_MANAGER_ETH_NAMESERVER,
        o_message_jso));
    

    *o_message_jso = json_object_new_array();
    return_val_do_info_if_expr(NULL == *o_message_jso, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    for (i = 0; i < array_len; i++) {
        item = json_object_array_get_idx(i_paras->val_jso, i);

        
        ret = redfish_set_nameserver(obj_handle, i_paras->is_from_webui, i_paras->user_name, i_paras->client, item,
            *o_message_jso, i);
        do_if_expr(ret == VOS_OK, (result = HTTP_OK));
        
    }

    return result;
}


LOCAL void patch_null_message(json_object *property_jso, json_object **message_array, const gchar *error)
{
    json_object *message = NULL;
    gint32 ret = 0;

    return_do_info_if_expr(NULL == message_array, debug_log(DLOG_ERROR, "%s : NULL pointer", __FUNCTION__));

    if (NULL == property_jso) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, error, &message, RF_NULL_STR, error);
        ret = json_object_array_add(*message_array, message);
        do_info_if_true(VOS_OK != ret, debug_log(DLOG_ERROR, "%s : array add fail", __FUNCTION__);
            (void)json_object_put(message));
    }

    return;
}


gint32 redfish_set_ip_version(OBJ_HANDLE obj_handle, json_object *property_jso, json_object **message_array,
    PROVIDER_PARAS_S *i_paras)
{
    gint32 ret;
    gint32 method_ret = VOS_ERR;
    const gchar *ip_ver_str = NULL;
    guint8 ip_ver = 0;
    GSList *input_slist = NULL;
    json_object *message = NULL;
    const gchar *ipver_error = "Oem/Huawei/IPVersion";

    return_val_do_info_if_expr(NULL == message_array, VOS_ERR,
        debug_log(DLOG_ERROR, "%s : NULL pointer", __FUNCTION__));

    return_val_do_info_if_expr(NULL == property_jso, method_ret,
        patch_null_message(property_jso, message_array, ipver_error));

    ip_ver_str = dal_json_object_get_str(property_jso);
    return_val_do_info_if_expr(NULL == ip_ver_str, method_ret,
        debug_log(DLOG_ERROR, "%s : dal_json_object_get_str return NULL", __FUNCTION__));

    
    if (0 == g_strcmp0(ip_ver_str, IP_VER_IPV4)) {
        ip_ver = ENABLE_IPV4_ONLY;
    } else if (0 == g_strcmp0(ip_ver_str, IP_VER_IPV6)) {
        ip_ver = ENABLE_IPV6_ONLY;
    } else if (0 == g_strcmp0(ip_ver_str, IP_VER_IPV4_ADN_IPV6)) {
        ip_ver = ENABLE_IPV4_AND_IPV6;
    } else {
        
        debug_log(DLOG_DEBUG, "%s : invalid ip version is %s", __FUNCTION__, ip_ver_str);
        
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, ipver_error, &message, ip_ver_str, ipver_error);
        (void)json_object_array_add(*message_array, message);

        return method_ret;
    }

    input_slist = g_slist_append(input_slist, g_variant_new_byte(ip_ver));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_METHOD_SET_IP_VERSION, AUTH_DISABLE, 0, input_slist, NULL);
    uip_free_gvariant_list(input_slist);

    switch (ret) {
        case VOS_OK:
            method_ret = VOS_OK;
            break;

        default:
            debug_log(DLOG_DEBUG, "%s: SetIpVersion return %d", __FUNCTION__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message);
            (void)json_object_array_add(*message_array, message);
    }

    return method_ret;
}


LOCAL gint32 redfish_set_net_mode(OBJ_HANDLE obj_handle, json_object *property_jso, json_object **message_array,
    PROVIDER_PARAS_S *i_paras)
{
    gint32 ret;
    gint32 method_ret = VOS_ERR;
    const gchar *net_mode_str = NULL;
    guint8 net_mode = 0;
    GSList *input_slist = NULL;
    json_object *message = NULL;
    guint8 netmode_flag = 0;
    const gchar *netmode_err = "Oem/Huawei/NetworkPortMode";
    guchar board_type = 0;

    return_val_do_info_if_expr((NULL == property_jso) || (NULL == message_array), VOS_ERR,
        debug_log(DLOG_ERROR, "%s : NULL pointer", __FUNCTION__));

    return_val_do_info_if_expr(NULL == property_jso, method_ret,
        patch_null_message(property_jso, message_array, netmode_err));

    
    ret = redfish_get_board_type(&board_type);
    return_val_do_info_if_fail(VOS_OK == ret, ret,
        debug_log(DLOG_ERROR, "%s, %d: get board_type fail.\n", __FUNCTION__, __LINE__));
    

    
    (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_NET_MODE_FLAG, &netmode_flag);

    if ((RF_UNSUPPORT_ADAPTIVE_NET_MODE == netmode_flag) || (BOARD_SWITCH == board_type)) {
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, netmode_err, &message, netmode_err);
        (void)json_object_array_add(*message_array, message);
        
        return HTTP_NOT_IMPLEMENTED;
        
    }

    net_mode_str = dal_json_object_get_str(property_jso);
    return_val_do_info_if_expr(NULL == net_mode_str, method_ret,
        debug_log(DLOG_ERROR, "%s : dal_json_object_get_str return NULL", __FUNCTION__));

    
    if (0 == g_strcmp0(NET_MODE_STRING_FIXED, net_mode_str)) {
        net_mode = NET_MODE_MANUAL;
    } else if (0 == g_strcmp0(NET_MODE_STRING_AUTO, net_mode_str)) {
        net_mode = NET_MODE_AUTO;
    } else {
        
        debug_log(DLOG_DEBUG, "%s : invalid net mode is %s", __FUNCTION__, net_mode_str);
        
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, netmode_err, &message, net_mode_str, netmode_err);
        (void)json_object_array_add(*message_array, message);

        return method_ret;
    }

    input_slist = g_slist_append(input_slist, g_variant_new_byte(net_mode));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_METHOD_SET_NET_MODE, AUTH_DISABLE, 0, input_slist, NULL);
    uip_free_gvariant_list(input_slist);

    switch (ret) {
        case VOS_OK:
            method_ret = VOS_OK;
            break;

        default:
            debug_log(DLOG_DEBUG, "%s: SetNetMode return %d", __FUNCTION__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message);
            (void)json_object_array_add(*message_array, message);
    }

    return method_ret;
}

LOCAL gint32 redfish_set_ncsi_mode(OBJ_HANDLE obj_handle, json_object *property_jso, json_object **message_array,
    PROVIDER_PARAS_S *i_paras)
{
    gint32 method_ret = VOS_ERR;
    const gchar *ncsi_mode_str = NULL;
    guint8 ncsi_mode = 0;
    GSList *input_slist = NULL;
    json_object *message = NULL;
    guint8 netmode_flag = 0;
    const gchar *ncsimode_err = "Oem/Huawei/NCSIMode";
    guchar board_type = 0;

    return_val_do_info_if_expr((property_jso == NULL) || (message_array == NULL), VOS_ERR,
        debug_log(DLOG_ERROR, "%s : NULL pointer", __FUNCTION__));

    return_val_do_info_if_expr(property_jso == NULL, method_ret,
        patch_null_message(property_jso, message_array, ncsimode_err));

    gint32 ret = redfish_get_board_type(&board_type);
    return_val_do_info_if_fail(ret == VOS_OK, ret,
        debug_log(DLOG_ERROR, "%s, %d: get board_type fail.", __FUNCTION__, __LINE__));

    
    (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_NET_MODE_FLAG, &netmode_flag);

    if ((netmode_flag == RF_UNSUPPORT_ADAPTIVE_NET_MODE) || (board_type == BOARD_SWITCH)) {
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, ncsimode_err, &message, ncsimode_err);
        (void)json_object_array_add(*message_array, message);
        return HTTP_NOT_IMPLEMENTED;
    }

    ncsi_mode_str = dal_json_object_get_str(property_jso);
    return_val_do_info_if_expr(ncsi_mode_str == NULL, method_ret,
        debug_log(DLOG_ERROR, "%s : dal_json_object_get_str return NULL", __FUNCTION__));

    
    if (g_strcmp0(ncsi_mode_str, AUTO_STRING) == 0) {
        ncsi_mode = RF_ENABLE;
    } else if (g_strcmp0(ncsi_mode_str, MANUAL_STRING) == 0) {
        ncsi_mode = RF_DISABLE;
    } else {
        debug_log(DLOG_DEBUG, "%s : invalid ncsi mode is %s", __FUNCTION__, ncsi_mode_str);
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, ncsimode_err, &message, ncsi_mode_str, ncsimode_err);
        (void)json_object_array_add(*message_array, message);
        return method_ret;
    }

    input_slist = g_slist_append(input_slist, g_variant_new_byte(ncsi_mode));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_METHOD_SET_NCSI_MODE, AUTH_DISABLE, 0, input_slist, NULL);
    uip_free_gvariant_list(input_slist);

    if (ret == VOS_OK) {
        method_ret = VOS_OK;
    } else {
        debug_log(DLOG_DEBUG, "%s: SetNcsiMode return %d", __FUNCTION__, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message);
        (void)json_object_array_add(*message_array, message);
    }

    return method_ret;
}


LOCAL gint32 port_type_string2int(const gchar *type, guint8 *type_int)
{
    return_val_do_info_if_expr(NULL == type, VOS_ERR, debug_log(DLOG_ERROR, "%s: NULL Pointer", __FUNCTION__));

    if (g_strcmp0(type, NET_TYPE_LOM_STR) == 0) {
        *type_int = NET_TYPE_LOM;
    } else if (g_strcmp0(type, NET_TYPE_DEDICATED_STR) == 0) {
        *type_int = NET_TYPE_DEDICATED;
    } else if (g_strcmp0(type, NET_TYPE_PCIE_STR) == 0) {
        *type_int = NET_TYPE_PCIE;
    } else if (g_strcmp0(type, NET_TYPE_AGGREGATION_STR) == 0) {
        *type_int = NET_TYPE_AGGREGATION;
    } else if (g_strcmp0(type, NET_TYPE_LOM2_STR) == 0) {
        *type_int = NET_TYPE_LOM2;
    } else if (g_strcmp0(type, NET_TYPE_OCP_STR) == 0) {
        *type_int = NET_TYPE_OCP_CARD;
    } else if (g_strcmp0(type, NET_TYPE_OCP2_STR) == 0) {
        *type_int = NET_TYPE_OCP2_CARD;
    } else {
        debug_log(DLOG_ERROR, "%s : invalid net type is %s", __FUNCTION__, type);
        return VOS_ERR;
    }

    return VOS_OK;
}


LOCAL gint32 redfish_judge_netport_exist(guint8 type, guint8 port_number)
{
    gint32 result = VOS_ERR;
    gint32 ret;
    GSList *business_obj_list = NULL;
    GSList *node = NULL;
    guint8 business_type = 0;
    guint8 business_port = 0;

    return_val_if_expr((NET_TYPE_DEDICATED == type) || (NET_TYPE_AGGREGATION == type), VOS_OK);

    ret = dfl_get_object_list(ETH_CLASS_NAME_BUSY_ETH, &business_obj_list);
    return_val_do_info_if_expr((VOS_OK != ret) || (NULL == business_obj_list), result,
        debug_log(DLOG_ERROR, "%s: get bussiness object list fail", __FUNCTION__));

    for (node = business_obj_list; node; node = g_slist_next(node)) {
        (void)dal_get_property_value_byte((OBJ_HANDLE)node->data, BUSY_ETH_ATTRIBUTE_CARD_TYPE, &business_type);
        (void)dal_get_property_value_byte((OBJ_HANDLE)node->data, BUSY_ETH_ATTRIBUTE_SILK_NUM, &business_port);

        if ((type == business_type) && (port_number == business_port)) {
            result = VOS_OK;
            break;
        }
    }

    g_slist_free(business_obj_list);

    return result;
}

LOCAL gint32 redfish_check_dedicated_port(gint32 slik_port)
{
    gint32 ret;
    OBJ_HANDLE eth_obj = 0;
    guint8 port_num;

    
    ret = dal_get_specific_object_byte(CLASS_NAME_ETH_FUNCTION, PROPERTY_FUNCTION_TYPE, FN_TYPE_OUTTER_OM, &eth_obj);
    if (ret == RET_OK) {
        port_num = dal_eth_get_cur_plane_eth_num();
        return (slik_port == port_num) ? RET_OK : RET_ERR;
    }

    if (slik_port == 1) {
        return RET_OK;
    }

    if (dal_check_dedicated_port_exist(slik_port) == RET_OK) {
        return RET_OK;
    }
    return RET_ERR;
}

LOCAL gint32 process_type_and_portnumber(json_object *property_jso, GSList **input_list, json_object **message_array)
{
    const gchar *type = NULL;
    guint8 type_int = 0;
    gint32 slik_port = 0;
    gint32 ret;
    gint32 result = VOS_ERR;
    gchar port_number_str[PROP_VAL_LENGTH] = {0};
    const gchar *type_err_mes = "Oem/Huawei/ManagementNetworkPort/Type";
    json_object *message = NULL;

    
    (void)get_element_str(property_jso, RFPROP_MANAGER_ETH_OEM_PORT_TYPE, &type);
    (void)port_type_string2int(type, &type_int);
    (void)get_element_int(property_jso, RFPROP_MANAGER_ETH_OEM_PORT_SLIK, &slik_port);

    ret = redfish_judge_mntportcab_valid(type_int);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        (void)create_message_info(MSGID_NOT_ACTIVE_NETCARD, type_err_mes, &message, type);
        (void)json_object_array_add(*message_array, message));

    if ((type_int == NET_TYPE_DEDICATED) || (type_int == NET_TYPE_AGGREGATION)) {
        if (redfish_check_dedicated_port(slik_port) == RET_OK) {
            *input_list = g_slist_append(*input_list, g_variant_new_byte(type_int));
            *input_list = g_slist_append(*input_list, g_variant_new_byte(slik_port));
            result = VOS_OK;
        } else {
            // 非法portnumber的错误
            (void)snprintf_s(port_number_str, PROP_VAL_LENGTH, PROP_VAL_LENGTH - 1, "%d", slik_port);
            (void)create_message_info(MSGID_PORT_NOT_EXIST, NULL, &message, type, port_number_str);
            (void)json_object_array_add(*message_array, message);
        }
    } else {
        
        ret = redfish_judge_netport_exist(type_int, slik_port);
        return_val_do_info_if_fail(VOS_OK == ret, result,
            (void)snprintf_s(port_number_str, PROP_VAL_LENGTH, PROP_VAL_LENGTH - 1, "%d", slik_port);
            (void)create_message_info(MSGID_PORT_NOT_EXIST, NULL, &message, type, port_number_str);
            (void)json_object_array_add(*message_array, message));

        // 板载网口或者PCIE网口或者LOM2
        *input_list = g_slist_append(*input_list, g_variant_new_byte(type_int));
        *input_list = g_slist_append(*input_list, g_variant_new_byte((guint8)slik_port));

        result = VOS_OK;
    }

    return result;
}


LOCAL gint32 process_net_port_obj(json_object *property_jso, json_object **message_array, GSList **input_list)
{
    gint32 ret = 0;
    gint32 result = VOS_ERR;
    const gchar *type = NULL;
    guint8 type_int = 0;
    json_object *message = NULL;
    const gchar *type_err_mes = "Oem/Huawei/ManagementNetworkPort/Type";
    const gchar *slik_err_mes = "Oem/Huawei/ManagementNetworkPort/PortNumber";

    return_val_do_info_if_expr((NULL == message_array) || (NULL == input_list), result,
        debug_log(DLOG_ERROR, "%s : NULL Pointer", __FUNCTION__));

    if ((TRUE == json_object_has_key(property_jso, RFPROP_MANAGER_ETH_OEM_PORT_TYPE)) &&
        (FALSE == json_object_has_key(property_jso, RFPROP_MANAGER_ETH_OEM_PORT_SLIK))) {
        
        (void)get_element_str(property_jso, RFPROP_MANAGER_ETH_OEM_PORT_TYPE, &type);

        ret = port_type_string2int(type, &type_int);
        return_val_if_fail(VOS_OK == ret, result);

        
        if ((type_int == NET_TYPE_DEDICATED) || (type_int == NET_TYPE_AGGREGATION)) {
            ret = redfish_judge_mntportcab_valid(type_int);
            return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
                (void)create_message_info(MSGID_NOT_ACTIVE_NETCARD, type_err_mes, &message, type);
                (void)json_object_array_add(*message_array, message));

            *input_list = g_slist_append(*input_list, g_variant_new_byte(type_int));

            result = VOS_OK;
        } else {
            // 板载网口或者PCIE网口或者lom2
            (void)create_message_info(MSGID_PROP_MISSING, slik_err_mes, &message, slik_err_mes);

            (void)json_object_array_add(*message_array, message);
        }
    } else if ((FALSE == json_object_has_key(property_jso, RFPROP_MANAGER_ETH_OEM_PORT_TYPE)) &&
        (TRUE == json_object_has_key(property_jso, RFPROP_MANAGER_ETH_OEM_PORT_SLIK))) {
        
        (void)create_message_info(MSGID_PROP_MISSING, type_err_mes, &message, type_err_mes);

        (void)json_object_array_add(*message_array, message);
    } else if ((TRUE == json_object_has_key(property_jso, RFPROP_MANAGER_ETH_OEM_PORT_TYPE)) &&
        (TRUE == json_object_has_key(property_jso, RFPROP_MANAGER_ETH_OEM_PORT_SLIK))) {
        
        result = process_type_and_portnumber(property_jso, input_list, message_array);
        
    } else {
        // 传到接口如果没有属性直接返回
        return result;
    }

    return result;
}


LOCAL gint32 redfish_set_manager_netport(OBJ_HANDLE obj_handle, json_object *property_jso, json_object **message_array,
    PROVIDER_PARAS_S *i_paras)
{
#define LINK_DISCONNECTED_ERROR (-5)
    gint32 ret;
    gint result = VOS_ERR;
    json_object *message = NULL;
    GSList *input_list = NULL;
    guint8 netmode_flag = 0;
    const gchar *error_info = "Oem/Huawei/ManagementNetworkPort";
    guchar board_type = 0;

    return_val_do_info_if_expr((NULL == property_jso) || (NULL == message_array), result,
        debug_log(DLOG_ERROR, "%s : NULL pointer", __FUNCTION__));

    
    ret = redfish_get_board_type(&board_type);
    return_val_do_info_if_fail(VOS_OK == ret, ret,
        debug_log(DLOG_ERROR, "%s, %d: get board_type fail.\n", __FUNCTION__, __LINE__));
    

    
    (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_NET_MODE_FLAG, &netmode_flag);

    if ((RF_UNSUPPORT_ADAPTIVE_NET_MODE == netmode_flag) || (BOARD_SWITCH == board_type)) {
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, error_info, &message, error_info);
        (void)json_object_array_add(*message_array, message);
        
        return HTTP_NOT_IMPLEMENTED;
        
    }

    
    ret = process_net_port_obj(property_jso, message_array, &input_list);
    return_val_do_info_if_fail(VOS_OK == ret, result, debug_log(DLOG_MASS, "%s : invalid parameter", __FUNCTION__));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_METHOD_SET_ACTIVE_PORT, AUTH_DISABLE, 0, input_list, NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
            result = VOS_OK;
            break;

        case REDFISH_ETH_UNSUPPORT:
            // 非手动模式
            (void)create_message_info(MSGID_MODIFY_FAILED_WITH_AUTO_NETMODE, error_info, &message);
            (void)json_object_array_add(*message_array, message);
            break;

        case LINK_DISCONNECTED_ERROR:
            (void)create_message_info(MSGID_NET_PORT_NO_LINK, error_info, &message);
            (void)json_object_array_add(*message_array, message);
            break;

        
        case REDFISH_ETH_AVAFLAG_DISABLED:
            (void)create_message_info(MSGID_NET_PORT_DISABLED, error_info, &message);
            (void)json_object_array_add(*message_array, message);
            break;
            

        default:
            debug_log(DLOG_DEBUG, "%s: SetActivePort return %d", __FUNCTION__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message);
            (void)json_object_array_add(*message_array, message);
    }

    return result;
}

LOCAL gint32 get_dedicated_port_num(gint32 port_num, guchar *eth_num)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = OBJ_HANDLE_COMMON;

    
    ret = dal_get_specific_object_byte(CLASS_NAME_ETH_FUNCTION, PROPERTY_FUNCTION_TYPE, FN_TYPE_OUTTER_OM, &obj_handle);
    if (ret == RET_OK) {
        *eth_num = dal_eth_get_cur_plane_eth_num();
        return RET_OK;
    }
    
    
    guint8 dedicat_eth_nums = 0;
    ret = dal_get_specific_obj_num_byte(ETH_CLASS_NAME_ETH, ETH_ATTRIBUTE_TYPE, NET_TYPE_DEDICATED, &dedicat_eth_nums);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get port number failed ret = %d", ret);
        return RET_ERR;
    }
    if (dedicat_eth_nums > 1) { 
        *eth_num = port_num;
        return RET_OK;
    }

    ret = dal_get_specific_object_byte(ETH_CLASS_NAME_MGNTPORTCAP, PROPERTY_MNGTPORTCAP_PCIEMGNT_ENABLE,
        ENABLED, &obj_handle);

    guint8 pciemgnt_cap_enabled = (ret == VOS_OK) ? ENABLE : DISABLE;

    
    return_val_if_expr((pciemgnt_cap_enabled == DISABLE) && (port_num != 1), RET_ERR);

    GSList *eth_list = NULL;
    ret = dfl_get_object_list(ETH_CLASS_NAME_ETH, &eth_list);
    if (ret != VOS_OK) {
        return VOS_ERR;
    }

    for (GSList* eth_list_iter = eth_list; eth_list_iter != NULL; eth_list_iter = eth_list_iter->next) {
        obj_handle = (OBJ_HANDLE)eth_list_iter->data;
        guint8 eth_type = 0;
        guint8 port_id = 0;
        guint8 out_type = 0;

        (void)dal_get_property_value_byte(obj_handle, ETH_ATTRIBUTE_TYPE, &eth_type);
        (void)dal_get_property_value_byte(obj_handle, ETH_ATTRIBUTE_ETH_ID, &port_id);
        (void)dal_get_property_value_byte(obj_handle, ETH_ATTRIBUTE_OUT_TYPE, &out_type);

        check_success_continue(out_type != OUTTER_GROUP_TYPE);

        
        guint8 result_eth_num = 0;
        if (((pciemgnt_cap_enabled == DISABLE) && (eth_type == NET_TYPE_DEDICATED)) ||
            ((pciemgnt_cap_enabled == ENABLE) && ((eth_type == NET_TYPE_DEDICATED) ||
            (eth_type == NET_TYPE_CABINET_VLAN)) && (port_id == port_num))) {
            (void)dal_get_property_value_byte(obj_handle, ETH_ATTRIBUTE_NUM, &result_eth_num);
            *eth_num = result_eth_num;

            g_slist_free(eth_list);
            return VOS_OK;
        }
    }

    g_slist_free(eth_list);
    return VOS_ERR;
}


LOCAL gint32 set_single_port_adaptive(gint32 i, PROVIDER_PARAS_S *i_paras, json_object **message_array,
    json_object *object)
{
    gint32 ret;
    gint32 result = VOS_ERR;
    const gchar *type = NULL;
    gint32 port_number = 0;
    json_bool adaptive = 0;
    json_object *message = NULL;
    gchar type_error[PROP_VAL_LENGTH] = {0};
    gchar port_error[PROP_VAL_LENGTH] = {0};
    gchar adaptive_error[PROP_VAL_LENGTH] = {0};
    gchar object_error[PROP_VAL_LENGTH] = {0};
    GSList *input_list = NULL;
    guint8 type_int = 0;
    gchar port_number_str[PROP_VAL_LENGTH] = {0};
    guchar active_port = NET_TYPE_DEDICATED;

    goto_label_do_info_if_expr(NULL == message_array, __EXIT, debug_log(DLOG_ERROR, "%s : NULL pointer", __FUNCTION__));

    
    ret = snprintf_s(type_error, PROP_VAL_LENGTH, PROP_VAL_LENGTH - 1, "Oem/Huawei/AdaptivePort/%d/Type", i);
    goto_label_do_info_if_expr(VOS_OK >= ret, __EXIT, debug_log(DLOG_ERROR, "%s : snprintf_s type fail", __FUNCTION__));

    ret = snprintf_s(port_error, PROP_VAL_LENGTH, PROP_VAL_LENGTH - 1, "Oem/Huawei/AdaptivePort/%d/PortNumber", i);
    goto_label_do_info_if_expr(VOS_OK >= ret, __EXIT,
        debug_log(DLOG_ERROR, "%s : snprintf_s port number fail", __FUNCTION__));

    ret = snprintf_s(adaptive_error, PROP_VAL_LENGTH, PROP_VAL_LENGTH - 1, "Oem/Huawei/AdaptivePort/%d/Adaptive", i);
    goto_label_do_info_if_expr(VOS_OK >= ret, __EXIT,
        debug_log(DLOG_ERROR, "%s : snprintf_s adaptive flag fail", __FUNCTION__));

    ret = snprintf_s(object_error, PROP_VAL_LENGTH, PROP_VAL_LENGTH - 1, "Oem/Huawei/AdaptivePort/%d", i);
    goto_label_do_info_if_expr(VOS_OK >= ret, __EXIT,
        debug_log(DLOG_ERROR, "%s : snprintf_s object fail", __FUNCTION__));

    
    ret = get_element_str(object, RFPROP_MANAGER_ETH_OEM_PORT_TYPE, &type);
    goto_label_do_info_if_fail(TRUE == ret, __EXIT,
        (void)create_message_info(MSGID_PROP_MISSING, type_error, &message, type_error);
        (void)json_object_array_add(*message_array, message));
    ret = port_type_string2int(type, &type_int);
    goto_label_do_info_if_fail(VOS_OK == ret, __EXIT, debug_log(DLOG_ERROR, "%s : invalid type", __FUNCTION__));

    
    ret = redfish_judge_mntportcab_valid(type_int);
    goto_label_do_info_if_fail(VOS_OK == ret, __EXIT,
        (void)create_message_info(MSGID_NOT_ACTIVE_NETCARD, object_error, &message, type);
        (void)json_object_array_add(*message_array, message));

    
    ret = get_element_int(object, RFPROP_MANAGER_ETH_OEM_PORT_SLIK, &port_number);
    goto_label_do_info_if_fail(TRUE == ret, __EXIT,
        (void)create_message_info(MSGID_PROP_MISSING, type_error, &message, type_error);
        (void)json_object_array_add(*message_array, message));

    
    goto_label_do_info_if_fail(0 <= port_number && port_number <= G_MAXUINT8, __EXIT,
        (void)snprintf_s(port_number_str, PROP_VAL_LENGTH, PROP_VAL_LENGTH - 1, "%d", port_number);
        (void)create_message_info(MSGID_PORT_NOT_EXIST, NULL, &message, type, port_number_str);
        (void)json_object_array_add(*message_array, message));
    

    
    if (NET_TYPE_DEDICATED == type_int) {
        
        active_port = NET_TYPE_DEDICATED;
        ret = get_dedicated_port_num(port_number, &active_port);
        goto_label_do_info_if_expr(ret != VOS_OK, __EXIT,
            (void)snprintf_s(port_number_str, PROP_VAL_LENGTH, PROP_VAL_LENGTH - 1, "%d", port_number);
            (void)create_message_info(MSGID_PORT_NOT_EXIST, NULL, &message, type, port_number_str);
            (void)json_object_array_add(*message_array, message));
        port_number = active_port;
    }

    
    ret = get_element_boolean(object, RFPROP_MANAGER_ETH_OEM_ADAPTIVE, &adaptive);
    goto_label_do_info_if_fail(TRUE == ret, __EXIT,
        (void)create_message_info(MSGID_PROP_MISSING, type_error, &message, type_error);
        (void)json_object_array_add(*message_array, message));

    
    goto_label_do_info_if_expr(NET_TYPE_AGGREGATION == type_int, __EXIT,
        (void)create_message_info(MSGID_CARD_NOT_SUPPORT_ADAPTIVE, object_error, &message);
        (void)json_object_array_add(*message_array, message));

    
    ret = redfish_judge_netport_exist(type_int, (guint8)port_number);
    goto_label_do_info_if_fail(VOS_OK == ret, __EXIT,
        (void)snprintf_s(port_number_str, PROP_VAL_LENGTH, PROP_VAL_LENGTH - 1, "%d", port_number);
        (void)create_message_info(MSGID_PORT_NOT_EXIST, NULL, &message, type, port_number_str);
        (void)json_object_array_add(*message_array, message));

    input_list = g_slist_append(input_list, g_variant_new_byte(type_int));
    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)port_number));
    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)adaptive));

    ret =
        uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, i_paras->obj_handle,
        ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_METHOD_CHOOSE_ADAPTIVE_ETH, AUTH_DISABLE, 0, input_list, NULL);

    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
            result = VOS_OK;
            break;

        default:
            debug_log(DLOG_DEBUG, "%s: %d, set adaptive return %d", __FUNCTION__, i, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message);
            (void)json_object_array_add(*message_array, message);
    }

__EXIT:
    return result;
}


LOCAL gint32 redfish_set_adaptive_port(json_object *property_jso, json_object **message_array,
    PROVIDER_PARAS_S *i_paras)
{
    gint32 result = VOS_ERR;
    guint8 netmode_flag = 0;
    json_object *message = NULL;
    gint32 array_len;
    gint32 i;
    json_object *object = NULL;
    gint32 ret;
    const gchar *error_info = "Oem/Huawei/AdaptivePort";
    guint8 net_mode = 0;
    guchar board_type = 0;

    return_val_do_info_if_expr((NULL == property_jso) || (NULL == message_array), result,
        debug_log(DLOG_ERROR, "%s : NULL pointer", __FUNCTION__));

    
    ret = redfish_get_board_type(&board_type);
    return_val_do_info_if_fail(VOS_OK == ret, ret,
        debug_log(DLOG_ERROR, "%s, %d: get board_type fail.\n", __FUNCTION__, __LINE__));
    

    
    (void)dal_get_property_value_byte(i_paras->obj_handle, ETH_GROUP_ATTRIBUTE_NET_MODE_FLAG, &netmode_flag);

    if ((RF_UNSUPPORT_ADAPTIVE_NET_MODE == netmode_flag) || (BOARD_SWITCH == board_type)) {
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, error_info, &message, error_info);
        (void)json_object_array_add(*message_array, message);
        
        return HTTP_NOT_IMPLEMENTED;
        
    }

    
    (void)dal_get_property_value_byte(i_paras->obj_handle, ETH_GROUP_ATTRIBUTE_NET_MODE, &net_mode);
    return_val_do_info_if_fail(NET_MODE_AUTO == net_mode, result,
        (void)create_message_info(MSGID_NETMODE_NOT_SUPPORT_ADAPTIVE, error_info, &message);
        (void)json_object_array_add(*message_array, message));

    array_len = json_object_array_length(property_jso);

    for (i = 0; i < array_len; i++) {
        object = json_object_array_get_idx(property_jso, i);
        if (NULL == object) {
            gchar error_mes[PROP_VAL_LENGTH] = {0};
            // PROP_VAL_LENGTH大小为64，大于格式化后字符串长度，无需判断返回值
            (void)snprintf_s(error_mes, PROP_VAL_LENGTH, PROP_VAL_LENGTH - 1, "Oem/Huawei/AdaptivePort/%d", i);

            (void)create_message_info(MSGID_PROP_ITEM_TYPE_ERR, error_mes, &message, RF_VALUE_NULL, error_mes);
            (void)json_object_array_add(*message_array, message);

            continue;
        }

        
        continue_if_expr(0 == g_strcmp0(dal_json_object_to_json_string(object), JSON_NULL_OBJECT_STR));

        ret = set_single_port_adaptive(i, i_paras, message_array, object);
        do_info_if_true(VOS_OK == ret, (result = VOS_OK));
    }

    return result;
}


LOCAL gint32 __set_dns_ipver_or_mode(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, guint8 prop_val,
    const gchar *method_name)
{
    gint32 ret;
    GSList *input_list = NULL;

    input_list = g_slist_append(input_list, g_variant_new_byte(prop_val));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_NAME_DNSSETTING, method_name, AUTH_DISABLE, 0, input_list, NULL);
    uip_free_gvariant_list(input_list);

    return ret;
}


gint32 verify_ip_protocol_enabled(OBJ_HANDLE group_obj, const gchar *ip_ver)
{
    guint8 group_ip_ver = 0;

    (void)dal_get_property_value_byte(group_obj, ETH_GROUP_ATTRIBUTE_IP_VERSION, &group_ip_ver);

    if (0 == g_strcmp0(ip_ver, DNS_IP_VER_IPV4_STR)) {
        return_val_if_expr((ENABLE_IPV4_ONLY == group_ip_ver) || (ENABLE_IPV4_AND_IPV6 == group_ip_ver), VOS_OK);
    } else if (0 == g_strcmp0(ip_ver, DNS_IP_VER_IPV6_STR)) {
        return_val_if_expr((ENABLE_IPV6_ONLY == group_ip_ver) || (ENABLE_IPV4_AND_IPV6 == group_ip_ver), VOS_OK);
    }

    return VOS_ERR;
}


gint32 verify_ip_mode(OBJ_HANDLE group_obj, const gchar *ip_ver)
{
    guint8 ip_mode = 0;

    if (0 == g_strcmp0(ip_ver, DNS_IP_VER_IPV4_STR)) {
        (void)dal_get_property_value_byte(group_obj, ETH_GROUP_ATTRIBUTE_IP_MODE, &ip_mode);

        return_val_if_expr(IP_STATIC_MODE == ip_mode, VOS_OK);
    } else if (0 == g_strcmp0(ip_ver, DNS_IP_VER_IPV6_STR)) {
        (void)dal_get_property_value_byte(group_obj, ETH_GROUP_ATTRIBUTE_IPV6_MODE, &ip_mode);

        return_val_if_expr(IP_STATIC_MODE == ip_mode, VOS_OK);
    }

    return VOS_ERR;
}


gint32 redfish_set_dns_mode(json_object *property_jso, json_object **message_array, PROVIDER_PARAS_S *i_paras)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    const gchar *dns_mode_str = NULL;
    guint8 dns_ip_ver = 0xff;
    guint8 dns_mode = 0;
    json_object *message = NULL;
    const gchar *dnsmode_err = "Oem/Huawei/DNSAddressOrigin";
    guint8 ip_ver = 0xff;

    return_val_do_info_if_expr(NULL == message_array, VOS_ERR,
        debug_log(DLOG_ERROR, "%s : NULL pointer", __FUNCTION__));

    return_val_do_info_if_expr(NULL == property_jso, VOS_ERR,
        patch_null_message(property_jso, message_array, dnsmode_err));

    dns_mode_str = dal_json_object_get_str(property_jso);
    return_val_do_info_if_expr(NULL == dns_mode_str, VOS_ERR,
        debug_log(DLOG_ERROR, "%s : dal_json_object_get_str return NULL", __FUNCTION__));

    // 获取DDNS类的对象句柄
    (void)dal_get_object_handle_nth(CLASS_NAME_DNSSETTING, 0, &obj_handle);

    
    if (0 == g_strcmp0(DNS_ADD_ORIGIN_MANUAL_STR, dns_mode_str)) {
        dns_mode = DNS_ADD_ORIGIN_MANUAL;

        ret = __set_dns_ipver_or_mode(i_paras, obj_handle, dns_mode, METHOD_DNSSETTING_DNSMODE);
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            debug_log(DLOG_ERROR, "%s : SetDdnsMode return %d", __FUNCTION__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message);
            (void)json_object_array_add(*message_array, message));
    } else if ((0 == g_strcmp0(DNS_IP_VER_IPV4_STR, dns_mode_str)) ||
        (0 == g_strcmp0(DNS_IP_VER_IPV6_STR, dns_mode_str))) {
        if (0 == g_strcmp0(DNS_IP_VER_IPV4_STR, dns_mode_str)) {
            dns_ip_ver = DNS_IP_VER_IPV4;
        } else {
            dns_ip_ver = DNS_IP_VER_IPV6;
        }

        dns_mode = DNS_ADD_ORIGIN_AUTO;

        // 对应IP版本未使能，返回错误
        ret = verify_ip_protocol_enabled(i_paras->obj_handle, dns_mode_str);
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            (void)create_message_info(MSGID_MODIFY_FAILED_WITH_IPVER, dnsmode_err, &message, dnsmode_err, dns_mode_str);
            (void)json_object_array_add(*message_array, message));

        // 对应绑定的IP版本是手动获取，返回错误
        ret = verify_ip_mode(i_paras->obj_handle, dns_mode_str);
        return_val_do_info_if_expr(VOS_OK == ret, VOS_ERR,
            (void)create_message_info(MSGID_STATIC_IP_MODE, dnsmode_err, &message, dns_mode_str);
            (void)json_object_array_add(*message_array, message));

        // 先获取下绑定的IP协议，如果和内存中的值相同，就不用再设置了
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_DNSSETTING_DNSIPVER, &ip_ver);

        if (dns_ip_ver != ip_ver) {
            // 设置绑定IP协议
            ret = __set_dns_ipver_or_mode(i_paras, obj_handle, dns_ip_ver, METHOD_DNSSETTING_DNSIPVER);
            return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
                debug_log(DLOG_ERROR, "%s : SetDdnsIPVer return %d", __FUNCTION__, ret);
                (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message);
                (void)json_object_array_add(*message_array, message));
        }

        ret = __set_dns_ipver_or_mode(i_paras, obj_handle, dns_mode, METHOD_DNSSETTING_DNSMODE);
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            debug_log(DLOG_ERROR, "%s : SetDdnsMode return %d", __FUNCTION__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message);
            (void)json_object_array_add(*message_array, message));
    } else {
        debug_log(DLOG_ERROR, "%s : invalid dns mode is %s", __FUNCTION__, dns_mode_str);

        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, dnsmode_err, &message, dns_mode_str, dnsmode_err);
        (void)json_object_array_add(*message_array, message);

        return VOS_ERR;
    }

    return VOS_OK;
}


LOCAL gint32 set_manager_ethernetinterface_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret = 0;
    gint32 result;
    OBJ_HANDLE obj_handle = 0;
    json_object *huawei = NULL;
    json_object *property_jso = NULL;
    
    gint32 return_array[NUMBER_NETWORK_PORT_ATTRIBUTES_OEM] = {0};
    gint32 arr_subscript = 0;

    guint8 software_type = 0;

    return_val_do_info_if_expr((provider_paras_check(i_paras) != VOS_OK) || (o_message_jso == NULL),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s : NULL pointer", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    (void)json_object_object_get_ex(i_paras->val_jso, RFPROP_COMMON_HUAWEI, &huawei);
    
    return_val_if_expr((0 == g_strcmp0(dal_json_object_to_json_string(i_paras->val_jso), JSON_NULL_OBJECT_STR)) ||
        (0 == g_strcmp0(dal_json_object_to_json_string(huawei), JSON_NULL_OBJECT_STR)),
        HTTP_BAD_REQUEST);

    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    obj_handle = i_paras->obj_handle;

    *o_message_jso = json_object_new_array();
    return_val_do_info_if_expr(NULL == *o_message_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:json_object_new_array fail", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    
    if (json_object_object_get_ex(huawei, RFPROP_MANAGER_ETH_OEM_IPVER, &property_jso)) {
        ret = redfish_set_ip_version(obj_handle, property_jso, o_message_jso, i_paras);
        
        return_array[arr_subscript++] = ret;
        
    }

    (void)dal_get_software_type(&software_type);
    
    if (MGMT_SOFTWARE_TYPE_EM != software_type) { // 不设置此属性
        property_jso = NULL;

        
        if (json_object_object_get_ex(huawei, RFPROP_MANAGER_ETH_OEM_NET_MODE, &property_jso)) {
            ret = redfish_set_net_mode(obj_handle, property_jso, o_message_jso, i_paras);
            
            return_array[arr_subscript++] = ret;
            
        }

        property_jso = NULL;

        
        if (json_object_object_get_ex(huawei, RFPROP_MANAGER_ETH_OEM_NCSI_MODE, &property_jso)) {
            ret = redfish_set_ncsi_mode(obj_handle, property_jso, o_message_jso, i_paras);
            return_array[arr_subscript++] = ret;
        }

        property_jso = NULL;

        
        if (json_object_object_get_ex(huawei, RFPROP_MANAGER_ETH_OEM_MANAGER_PORT, &property_jso)) {
            ret = redfish_set_manager_netport(obj_handle, property_jso, o_message_jso, i_paras);
            
            return_array[arr_subscript++] = ret;
            
        }

        property_jso = NULL;

        
        if (json_object_object_get_ex(huawei, RFPROP_MANAGER_ETH_OEM_ADAPTIVEPORT, &property_jso)) {
            ret = redfish_set_adaptive_port(property_jso, o_message_jso, i_paras);
            
            return_array[arr_subscript++] = ret;
            
        }
    }
    

    property_jso = NULL;

    
    if (json_object_object_get_ex(huawei, RFPROP_MANAGER_ETH_OEM_DNS_MODE, &property_jso)) {
        ret = redfish_set_dns_mode(property_jso, o_message_jso, i_paras);
        
        return_array[arr_subscript++] = ret;
        
    }

    
    result = return_value_judgment(arr_subscript, return_array);
    

    return result;
}

gint32 manager_ethernetinterface_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret;
    guchar NetConfigEnable = 0;
    OBJ_HANDLE group_obj = 0;
    gchar *if_name = NULL;
    guint8 smm_index = 0;
    gchar smm_name[MAX_NAME_SIZE] = { 0 };
    guint8 software_type = 0;

    
    
    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_NETCONFIG, &NetConfigEnable);
    return_val_do_info_if_expr((VOS_ERR == ret), HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s, %d: get_NetConfigEnable_status fail.\n", __FUNCTION__, __LINE__));
    return_val_if_fail((SERVICE_NETCONFIG_ENABLE == NetConfigEnable), HTTP_NOT_FOUND);


    (void)dal_get_software_type(&software_type);
    if ((MGMT_SOFTWARE_TYPE_EM == software_type) &&
        (!dal_is_support_eth_function_specific_propery(PROPERTY_VLAN_STATUS, 0))) {
        if (!redfish_check_smm_manager_uri_valid(i_paras->uri)) { // 管理板名称错误
            return HTTP_NOT_FOUND;
        }
        get_smm_name_from_redfish_uri(RF_MANAGERS_URI, i_paras->uri, smm_name, sizeof(smm_name));
        
        (void)get_smm_index(&smm_index);
        if (smm_index == smm_name[3] - '0') { // 访问本板
            if_name = g_strrstr(i_paras->uri, "/");
            if (strcasecmp(if_name, "/staticeth2") == 0) { // 静态ip
                dfl_get_object_handle(OUT_ETHGROUP_OBJ_NAME, &group_obj);
            } else if (strcasecmp(if_name, "/floateth2") == 0) { // 浮动ip
                dfl_get_object_handle(FLOAT_ETHGROUP_OBJ_NAME, &group_obj);
            } else { // 错误的网口名
                return HTTP_NOT_FOUND;
            }
        } else { // 对板不在位时，返回资源未找到
            return HTTP_NOT_FOUND;
        }
    }
    
    else {
        
        ret = redfish_check_manager_eth_uri_effective(i_paras, &group_obj);
        return_val_if_fail(VOS_OK == ret, ret);
    }

    i_paras->obj_handle = group_obj;

    *prop_provider = g_manager_eth_provider;
    *count = sizeof(g_manager_eth_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}
