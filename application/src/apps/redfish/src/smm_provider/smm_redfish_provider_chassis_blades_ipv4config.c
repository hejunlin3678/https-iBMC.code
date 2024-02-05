
#include "redfish_provider.h"
#include "redfish_forward.h"

#define IPV4_IP_STR_SIZE 15
#define IPV4_MASK_STR_SIZE 15
#define IPV4_GATEWAY_STR_SIZE 15

LOCAL void ipmode_value_to_string(guint8 origin, gchar **ipmode)
{
    
    if (IP_STATIC_MODE == origin) {
        *ipmode = IP_MODE_STATIC_STRING;
    } else if (IP_DHCP_MODE == origin) {
        *ipmode = IP_MODE_DHCP_STRING;
    }
}
LOCAL void smm_get_one_ipconfig_ipv4(guint8 slave_addr, gchar *blade_name, json_object *component_obj)
{
    gchar address[IPV4_IP_STR_SIZE + 1] = {0};
    gchar subnetmask[IPV4_MASK_STR_SIZE + 1] = {0};
    guint8 origin;
    gchar gateway[IPV4_GATEWAY_STR_SIZE + 1] = {0};
    gchar *ipmode = NULL;
    gchar result[BUFF_LEN] = { 0 };
    gchar mac_addr[MACADDRESS_LEN] = {0};
    errno_t safe_fun_ret;
    gchar eth_uri[ETHS_URI_LEN + MACADDRESS_LEN + 2] = {0};
    size_t eth_uri_len = 0;

    
    (void)memset_s(result, sizeof(result), 0, sizeof(result));
    (void)redfish_get_blade_bmc_ip_config_by_cmd(slave_addr, NETFN_TRANSPORT_REQ, IPMI_GET_LAN_CONFIGURATION_PARAMETERS,
        IPMI_IPV4_MODE_OFFSET, result, sizeof(result));
    origin = result[0];
    ipmode_value_to_string(origin, &ipmode);

    
    (void)memset_s(result, sizeof(result), 0, sizeof(result));
    (void)redfish_get_blade_bmc_ip_config_by_cmd(slave_addr, NETFN_TRANSPORT_REQ, IPMI_GET_LAN_CONFIGURATION_PARAMETERS,
        IPMI_IPV4_ADDRESS_OFFSET, result, sizeof(result));
    safe_fun_ret = strcpy_s(address, sizeof(address), (const gchar *)result);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

    
    (void)memset_s(result, sizeof(result), 0, sizeof(result));
    (void)redfish_get_blade_bmc_ip_config_by_cmd(slave_addr, NETFN_TRANSPORT_REQ, IPMI_GET_LAN_CONFIGURATION_PARAMETERS,
        IPMI_IPV4_MASK_OFFSET, result, sizeof(result));
    safe_fun_ret = strcpy_s(subnetmask, sizeof(subnetmask), (const gchar *)result);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

    
    (void)memset_s(result, sizeof(result), 0, sizeof(result));
    (void)redfish_get_blade_bmc_ip_config_by_cmd(slave_addr, NETFN_TRANSPORT_REQ, IPMI_GET_LAN_CONFIGURATION_PARAMETERS,
        IPMI_IPV4_GATEWAY_OFFSET, result, sizeof(result));
    safe_fun_ret = strcpy_s(gateway, sizeof(gateway), (const gchar *)result);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

    
    (void)memset_s(result, sizeof(result), 0, sizeof(result));
    (void)redfish_get_blade_bmc_ip_config_by_cmd(slave_addr, NETFN_TRANSPORT_REQ, IPMI_GET_LAN_CONFIGURATION_PARAMETERS,
        IPMI_MAC_ADDRESS_OFFSET, result, sizeof(result));
    if (strlen((gchar *)result) != 0) {
        safe_fun_ret = strcpy_s(mac_addr, sizeof(mac_addr), (const gchar *)result);
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

        eth_uri_len = sizeof(eth_uri);
        if (dal_is_support_eth_function_specific_propery(PROPERTY_VLAN_STATUS, 0)) {
            safe_fun_ret =
                snprintf_s(eth_uri, eth_uri_len, eth_uri_len - 1, RFPROP_MANAGER_ETH, blade_name, MGNT_DEFAULT_IP_STR);
        } else {
            safe_fun_ret = snprintf_s(eth_uri, eth_uri_len, eth_uri_len - 1, RFPROP_MANAGER_ETH, blade_name, mac_addr);
        }
        do_val_if_expr(safe_fun_ret <= 0,
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    }

    debug_log(DLOG_DEBUG, " component_obj: %s, %s,%s, %s, %s, %s", blade_name, eth_uri, address, subnetmask, gateway,
        ipmode);
    json_object_object_add(component_obj, RFPROP_IPCONFIG_ID, json_object_new_string(blade_name));
    json_object_object_add(component_obj, ODATA_ID,
        eth_uri[0] != '\0' ? json_object_new_string((const gchar *)eth_uri) : NULL);
    json_object_object_add(component_obj, RFPROP_IPCONFIG_IPADDR,
        address[0] != '\0' ? json_object_new_string((const gchar *)address) : NULL);
    json_object_object_add(component_obj, RFPROP_IPCONFIG_MASK,
        subnetmask[0] != '\0' ? json_object_new_string((const gchar *)subnetmask) : NULL);
    json_object_object_add(component_obj, RFPROP_IPCONFIG_GATEWAY,
        gateway[0] != '\0' ? json_object_new_string((const gchar *)gateway) : NULL);
    json_object_object_add(component_obj, RFPROP_IPCONFIG_IPMODE,
        ipmode != NULL ? json_object_new_string((const gchar *)ipmode) : NULL);
}


LOCAL gint32 get_one_ipv4config_info(OBJ_HANDLE obj_handle, gpointer user_data)
{
    errno_t safe_fun_ret;
    json_object *components_obj = (json_object *)user_data;
    json_object *obj = NULL;
    const gchar *blade_name_str = NULL;
    guint8 slave_addr = 0;
    gchar blade_name_f[32] = { 0 };
    guint8 blade_type = 0;
    guint8 presence = 0;
    guint8 hot_swap = 0;

    
    (void)dal_get_property_value_byte(obj_handle, PROTERY_IPMBETH_BLADE_PRESENCE, &presence);
    if (presence != 0x01) {
        return VOS_OK;
    }
    blade_name_str = dfl_get_object_name(obj_handle);

    safe_fun_ret = strncpy_s(blade_name_f, sizeof(blade_name_f), blade_name_str, sizeof(blade_name_f) - 1);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

    (void)dal_get_property_value_byte(obj_handle, PROTERY_IPMBETH_BLADE_BLADETYPE, &blade_type);
    if (blade_type == 0x03) { // 管理板
        return VOS_OK;
    } else {
        format_string_as_upper_head(blade_name_f, strlen(blade_name_f));
    }
    (void)dal_get_property_value_byte(obj_handle, PROTERY_IPMBETH_BLADE_HOTSWAP, &hot_swap);
    if (hot_swap == FRU_HS_STATE_M7 || hot_swap == FRU_HS_STATE_M0) {
        return VOS_OK;
    }
    (void)dal_get_property_value_byte(obj_handle, PROTERY_IPMBETH_BLADE_SLAVEADDR, &slave_addr);

    obj = json_object_new_object();
    return_val_do_info_if_fail(obj, VOS_OK,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_object fail.\n", __FUNCTION__, __LINE__));
    smm_get_one_ipconfig_ipv4(slave_addr, blade_name_f, obj);
    json_object_array_add(components_obj, obj);

    debug_log(DLOG_DEBUG, "get_%s_ipconfig_info done", blade_name_f);

    return VOS_OK;
}


gint32 smm_get_ipv4_config(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    json_object *message_obj = NULL;
    json_object *components_obj = NULL;
    gint32 ret = 0;
    gchar *slotid_str = NULL;
    guint8 slotid = 0;
    OBJ_HANDLE object_handle = 0;

    
    if (NULL == o_rsc_jso || NULL == o_err_array_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "[%s] NULL pointer.", __func__);
        create_message_info(MSGID_INTERNAL_ERR, NULL, &message_obj);
        json_object_array_add(o_err_array_jso, message_obj);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_READONLY, HTTP_FORBIDDEN, {
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, &message_obj);
        json_object_array_add(o_err_array_jso, message_obj);
    });

    if (i_paras->uri_params) {
        slotid_str = (gchar *)g_hash_table_lookup(i_paras->uri_params, "slot");
    }
    if (slotid_str) {
        gint32 value = -1;
        if (dal_strtoi(slotid_str, &value, DECIMAL_NUM) != RET_OK || value < 0 || value > UCHAR_MAX) {
            debug_log(DLOG_ERROR, "%s:dal_strtoi failed", __FUNCTION__);
            create_message_info(MSGID_INTERNAL_ERR, NULL, &message_obj);
            json_object_array_add(o_err_array_jso, message_obj);
            return HTTP_INTERNAL_SERVER_ERROR;
        }

        slotid = (guint8)value;
        return_val_do_info_if_expr((slotid == 0 || slotid > 38), HTTP_INTERNAL_SERVER_ERROR,
            create_message_info(MSGID_INTERNAL_ERR, NULL, &message_obj);
            json_object_array_add(o_err_array_jso, message_obj));
        debug_log(DLOG_DEBUG, "%s ,slotid = %d", __FUNCTION__, slotid);
        ret = dal_get_specific_object_byte((const gchar *)CLASS_NAME_IPMBETH_BLADE,
            (const gchar *)PROTERY_IPMBETH_BLADE_SLAVEADDR, get_blade_ipmbaddr(slotid), &object_handle);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST, {
            create_message_info(MSGID_ACT_PARA_UNKNOWN, NULL, &message_obj, "get account", "slot");
            json_object_array_add(o_err_array_jso, message_obj);
        });
        components_obj = json_object_new_array();
        (void)get_one_ipv4config_info(object_handle, components_obj);
        json_object_object_add(o_rsc_jso, RFPROP_IPCONFIG_COMPONENTS_IPV4, components_obj);
        return VOS_OK;
    }
    components_obj = json_object_new_array();
    (void)dfl_foreach_object(CLASS_NAME_IPMBETH_BLADE, get_one_ipv4config_info, components_obj, NULL);
    json_object_object_add(o_rsc_jso, RFPROP_IPCONFIG_COMPONENTS_IPV4, components_obj);

    return VOS_OK;
}