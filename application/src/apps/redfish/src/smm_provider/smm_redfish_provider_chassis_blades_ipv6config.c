
#include <arpa/inet.h>
#include "redfish_provider.h"
#include "redfish_forward.h"

#define MAX_SLAAC_ADDRESS_NUM 15
LOCAL guint8 origin = 0;


LOCAL gint32 get_bmc_lan_config_by_oem_cmd(guint8 slave_addr, guint16 prop_id, gchar *result, guint16 result_len)
{
    errno_t safe_fun_ret = EOK;
    gint32 ret;
    IPMIMSG_BLADE_T request_head;
    gpointer response = NULL;
    const guchar *response_data = NULL;
    guchar data[BUFF_LEN] = { 0 };
    guint32 manu_id = HUAWEI_MFG_ID;
    guint32 huawei_id;
    guint8 prop_val_len;

    return_val_if_expr(NULL == result, VOS_ERR);

    request_head.lun = 0;
    request_head.netfun = NETFN_OEM_REQ;
    request_head.slave_addr = slave_addr;
    request_head.cmd = IPMI_OEM_INTERAL_COMMAND;
    request_head.src_len = 17;

    data[0] = LONGB0(manu_id);
    data[1] = LONGB1(manu_id);
    data[2] = LONGB2(manu_id);
    data[3] = 0x36;
    data[4] = 0x58;
    data[5] = 0x00;
    data[6] = 0x01;
    data[7] = 0xff;
    data[8] = 0x00;
    data[9] = 0x00;
    data[10] = 0x01;
    data[11] = 0x00;
    data[12] = 0x00;
    data[13] = 0x01;
    data[14] = dal_eth_get_out_type_groupid(); 
    data[15] = prop_id & 0xff;
    data[16] = (prop_id >> 8) & 0xff;

    request_head.data = data;

    ret = redfish_ipmi_send_msg_to_blade(&request_head, MAX_POSSIBLE_IPMI_FRAME_LEN, &response, TRUE);
    return_val_do_info_if_expr(ret != VOS_OK, ret,
        debug_log(DLOG_ERROR, "(%s) Send lan config ipmi request to blade%d BMC failed.ret:%d", __func__, slave_addr,
        ret));

    response_data = get_ipmi_src_data((gconstpointer)response);
    if (NULL == response_data) {
        debug_log(DLOG_ERROR, "[%s] NULL pointer!\r\n", __FUNCTION__);
        g_free(response);
        return VOS_ERR;
    }

    if (response_data[0] != COMP_CODE_SUCCESS) {
        debug_log(DLOG_ERROR, "(%s) get ipmi response data failed, netfn:%x cmd:%x, completion_code: %02X", __func__,
            NETFN_OEM_REQ, IPMI_OEM_INTERAL_COMMAND, response_data[0]);
        g_free(response);
        return VOS_ERR;
    }

    huawei_id = MAKE_DWORD(0x00, response_data[3], response_data[2], response_data[1]);
    if (huawei_id != HUAWEI_MFG_ID) {
        debug_log(DLOG_ERROR, "(%s)verify_ipmi_msg_oem_head from blade%d failed.\r\n", __func__, slave_addr);
        g_free(response);
        return VOS_ERR;
    }
    prop_val_len = response_data[8];

    return_val_do_info_if_expr(result_len < prop_val_len, VOS_ERR, g_free(response); debug_log(DLOG_ERROR,
        "(%s)property value length bigger than buffer length,buffer len is %d,property value length is %d.\r\n",
        __func__, result_len, prop_val_len));

    if (0 < prop_val_len) {
        safe_fun_ret = strncpy_s(result, result_len, (const char *)&response_data[9], prop_val_len);
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    }
    g_free(response);

    return VOS_OK;
}

LOCAL gint32 redfish_get_blade_SLAAC_address_ipmi(guint8 slave_addr, gint32 i, gchar *result, guint8 *prefix_length,
    guint16 result_len)
{
    gint32 ret;
    errno_t safe_fun_ret;
    IPMIMSG_BLADE_T request_head;
    gpointer response = NULL;
    const guchar *response_data = NULL;
    guchar data[4] = { 0 };
    struct in6_addr ipv6_addr_digit;
    gchar ipv6_addr_temp[BUFF_LEN] = { 0 };

    return_val_if_expr(NULL == result, VOS_ERR);

    request_head.lun = 0;
    request_head.netfun = NETFN_TRANSPORT_REQ;
    request_head.slave_addr = slave_addr;
    request_head.cmd = IPMI_GET_LAN_CONFIGURATION_PARAMETERS;
    request_head.src_len = 4;

    data[0] = 0x01;
    data[1] = IPMI_IPV6_SLAAC_IP_OFFSET;
    data[2] = i;
    data[3] = 0x00;

    request_head.data = data;

    ret = redfish_ipmi_send_msg_to_blade(&request_head, 512, &response, TRUE);
    return_val_do_info_if_expr(VOS_OK != ret, ret,
        debug_log(DLOG_ERROR, "(%s) Send lan config ipmi request to %u BMC failed.ret:%d", __func__, slave_addr, ret));
    response_data = get_ipmi_src_data(response);
    return_val_do_info_if_expr(NULL == response_data, VOS_ERR,
        debug_log(DLOG_ERROR, "(%s) Response data is null", __func__);
        g_free(response));

    return_val_do_info_if_expr(response_data[0] != COMP_CODE_SUCCESS, ret, ret = response_data[0]; g_free(response);
        response = NULL);
    ret = (response_data[4] | response_data[5] | response_data[6] | response_data[7] | response_data[8] |
        response_data[9] | response_data[10] | response_data[11] | response_data[12] | response_data[13] |
        response_data[14] | response_data[15] | response_data[16] | response_data[17] | response_data[18] |
        response_data[19]);
    if (ret == 0) {
        (void)memset_s(ipv6_addr_temp, sizeof(ipv6_addr_temp), 0, sizeof(ipv6_addr_temp));
        g_free(response);
        response = NULL;
        return VOS_ERR;
    } else {
        (void)memset_s(ipv6_addr_temp, sizeof(ipv6_addr_temp), 0, sizeof(ipv6_addr_temp));
        debug_log(DLOG_DEBUG,
            "%s get blade%d mac Response: %02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X",
            __func__, slave_addr, response_data[4], response_data[5], response_data[6], response_data[7],
            response_data[8], response_data[9], response_data[10], response_data[11], response_data[12],
            response_data[13], response_data[14], response_data[15], response_data[16], response_data[17],
            response_data[18], response_data[19]);

        safe_fun_ret = snprintf_s(ipv6_addr_temp, sizeof(ipv6_addr_temp), sizeof(ipv6_addr_temp) - 1,
            "%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X", response_data[4],
            response_data[5], response_data[6], response_data[7], response_data[8], response_data[9], response_data[10],
            response_data[11], response_data[12], response_data[13], response_data[14], response_data[15],
            response_data[16], response_data[17], response_data[18], response_data[19]);
        do_val_if_expr(safe_fun_ret <= 0,
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    }

    
    (void)inet_pton(AF_INET6, ipv6_addr_temp, &ipv6_addr_digit);
    (void)memset_s(ipv6_addr_temp, sizeof(ipv6_addr_temp), 0, sizeof(ipv6_addr_temp));

    (void)inet_ntop(AF_INET6, &ipv6_addr_digit, ipv6_addr_temp, sizeof(ipv6_addr_temp));
    safe_fun_ret = snprintf_s(result, result_len, result_len - 1, "%s", ipv6_addr_temp);
    do_val_if_expr(safe_fun_ret <= 0,
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    *prefix_length = response_data[20];

    g_free(response);
    response = NULL;

    return VOS_OK;
}
LOCAL void smm_get_one_slaac_ipv6(guint8 slave_addr, const gchar *blade_name, json_object *obj)
{
    gint32 i;
    guint8 slaac_ip_index = 0;
    gchar result[BUFF_LEN] = { 0 };
    guint8 prefix_length = 0;
    gint32 iRet = 0;
    gchar slaac_ipv6_addr[BUFF_LEN] = { 0 };
    json_object *component1_obj = NULL;
    json_object *obj_tmp = NULL;
    errno_t safe_fun_ret = EOK;
    if (2 == origin) { // dhcp
        slaac_ip_index = 1;
    } else if (1 == origin) { // static
        slaac_ip_index = 0;
    } else {
        debug_log(DLOG_ERROR, "%s unknow ipmode6", __FUNCTION__);
    }
    component1_obj = json_object_new_array();
    for (i = 0; i < MAX_SLAAC_ADDRESS_NUM; i++) {
        iRet = redfish_get_blade_SLAAC_address_ipmi(slave_addr, slaac_ip_index, result, &prefix_length, sizeof(result));
        if (iRet != VOS_OK) {
            break;
        }
        safe_fun_ret = strcpy_s(slaac_ipv6_addr, sizeof(slaac_ipv6_addr), (const gchar *)result);
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
        obj_tmp = json_object_new_object();
        json_object_object_add(obj_tmp, RFPROP_IPCONFIG_ID, json_object_new_int(slaac_ip_index));
        json_object_object_add(obj_tmp, RFPROP_IPCONFIG_IPADDR,
            slaac_ipv6_addr[0] != '\0' ? json_object_new_string((const gchar *)slaac_ipv6_addr) : NULL);
        json_object_object_add(obj_tmp, RFPROP_IPCONFIG_PREFIX_LENGTH_IPV6, json_object_new_int(prefix_length));
        slaac_ip_index++;
        json_object_array_add(component1_obj, obj_tmp);
    }
    json_object_object_add(obj, RFPROP_IPCONFIG_SLAAC_IP_IPV6, component1_obj);
}
LOCAL void ipv6_ipmode_value_to_string(guint8 mode, gchar **ipmode)
{
    
    if (mode == IP_STATIC_MODE) {
        *ipmode = IP_MODE_STATIC_STRING;
    } else if (mode == IP_DHCP_MODE) {
        *ipmode = IPV6_MODE_DHCP;
    }
}

LOCAL void smm_get_one_ipconfig_ipv6(guint8 slave_addr, gchar *blade_name, json_object *component_obj)
{
    gchar address[IPV6_IP_STR_SIZE + 1] = {0};
    guint8 prefix_len;
    gchar gateway[IPV6_GATEWAY_STR_SIZE + 1] = {0};
    gchar *ipmode = NULL;
    gchar result[BUFF_LEN] = { 0 };
    gchar mac_addr[MACADDRESS_LEN] = {0};
    gchar eth_uri[ETHS_URI_LEN + MACADDRESS_LEN + 2] = {0};
    errno_t safe_fun_ret;
    debug_log(DLOG_DEBUG, "%s get blade%d ipv6 config", __FUNCTION__, slave_addr);

    
    (void)memset_s(result, sizeof(result), 0, sizeof(result));
    (void)redfish_get_blade_bmc_ip_config_by_cmd(slave_addr, NETFN_TRANSPORT_REQ, IPMI_GET_LAN_CONFIGURATION_PARAMETERS,
        IPMI_IPV6_MODE_OFFSET, result, sizeof(result));
    origin = result[0];
    ipv6_ipmode_value_to_string(origin, &ipmode);

    
    (void)memset_s(result, sizeof(result), 0, sizeof(result));
    (void)redfish_get_blade_bmc_ip_config_by_cmd(slave_addr, NETFN_TRANSPORT_REQ, IPMI_GET_LAN_CONFIGURATION_PARAMETERS,
        IPMI_IPV6_PREFIX_OFFSET, result, sizeof(result));
    prefix_len = result[0];

    
    (void)memset_s(result, sizeof(result), 0, sizeof(result));
    (void)get_bmc_lan_config_by_oem_cmd(slave_addr, (guint16)0x0009, result, sizeof(result));
    safe_fun_ret = strcpy_s(address, sizeof(address), (const gchar *)result);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

    
    (void)memset_s(result, sizeof(result), 0, sizeof(result));
    (void)get_bmc_lan_config_by_oem_cmd(slave_addr, (guint16)0x000b, result, sizeof(result));
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
        if (dal_is_support_eth_function_specific_propery(PROPERTY_VLAN_STATUS, 0)) {
            safe_fun_ret = snprintf_s(eth_uri, sizeof(eth_uri), sizeof(eth_uri) - 1, RFPROP_MANAGER_ETH, blade_name,
                MGNT_DEFAULT_IP_STR);
        } else {
            safe_fun_ret =
                snprintf_s(eth_uri, sizeof(eth_uri), sizeof(eth_uri) - 1, RFPROP_MANAGER_ETH, blade_name, mac_addr);
        }
        do_val_if_expr(safe_fun_ret <= 0,
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    }

    debug_log(DLOG_DEBUG, "component_obj:%s, %s, %s, %s, %d, %s", blade_name, eth_uri, address, gateway, prefix_len,
        ipmode);
    json_object_object_add(component_obj, RFPROP_IPCONFIG_ID, json_object_new_string(blade_name));
    json_object_object_add(component_obj, ODATA_ID,
        eth_uri[0] != '\0' ? json_object_new_string((const gchar *)eth_uri) : NULL);
    json_object_object_add(component_obj, RFPROP_IPCONFIG_IPADDR,
        address[0] != '\0' ? json_object_new_string((const gchar *)address) : NULL);
    json_object_object_add(component_obj, RFPROP_IPCONFIG_GATEWAY_IPV6,
        gateway[0] != '\0' ? json_object_new_string((const gchar *)gateway) : NULL);
    json_object_object_add(component_obj, RFPROP_IPCONFIG_PREFIX_LENGTH_IPV6, json_object_new_int(prefix_len));
    json_object_object_add(component_obj, RFPROP_IPCONFIG_IPMODE,
        ipmode != NULL ? json_object_new_string(ipmode) : NULL);
}


LOCAL gint32 get_one_ipv6config_info(OBJ_HANDLE handle, gpointer user_data)
{
    errno_t safe_fun_ret;
    json_object *components_obj = (json_object *)user_data;
    json_object *obj = NULL;
    guint8 blade_type = 0;
    guint8 presence = 0;
    guint8 hot_swap = 0;
    const gchar *blade_name = NULL;
    guint8 slave_addr = 0;
    gchar blade_name_f[32] = { 0 };

    
    (void)dal_get_property_value_byte(handle, PROTERY_IPMBETH_BLADE_PRESENCE, &presence);
    if (presence != 0x01) {
        return VOS_OK;
    }
    (void)dal_get_property_value_byte(handle, PROTERY_IPMBETH_BLADE_HOTSWAP, &hot_swap);
    if (hot_swap == FRU_HS_STATE_M7 || hot_swap == FRU_HS_STATE_M0) {
        return VOS_OK;
    }
    blade_name = dfl_get_object_name(handle);
    safe_fun_ret = strncpy_s(blade_name_f, sizeof(blade_name_f), blade_name, sizeof(blade_name_f) - 1);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

    (void)dal_get_property_value_byte(handle, PROTERY_IPMBETH_BLADE_BLADETYPE, &blade_type);
    if (blade_type == 0x03) { // 管理板
        return VOS_OK;
    } else {
        format_string_as_upper_head(blade_name_f, strlen(blade_name_f));
    }

    (void)dal_get_property_value_byte(handle, PROTERY_IPMBETH_BLADE_SLAVEADDR, &slave_addr);

    obj = json_object_new_object();
    return_val_do_info_if_fail(obj, VOS_OK,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_object fail.\n", __FUNCTION__, __LINE__));
    smm_get_one_ipconfig_ipv6(slave_addr, blade_name_f, obj);
    smm_get_one_slaac_ipv6(slave_addr, blade_name_f, obj);
    json_object_array_add(components_obj, obj);

    return VOS_OK;
}


gint32 smm_get_ipv6_config(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    json_object *msg_obj = NULL;
    json_object *components_obj = NULL;

    gint32 ret;
    gchar *slotid_str = NULL;
    guint8 slotid = 0;
    OBJ_HANDLE object_handle = 0;
    
    if (NULL == o_rsc_jso || NULL == o_err_array_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "[%s] NULL pointer.", __func__);
        create_message_info(MSGID_INTERNAL_ERR, NULL, &msg_obj);
        json_object_array_add(o_err_array_jso, msg_obj);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_READONLY, HTTP_FORBIDDEN, {
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, &msg_obj);
        json_object_array_add(o_err_array_jso, msg_obj);
    });

    if (i_paras->uri_params) {
        slotid_str = (gchar *)g_hash_table_lookup(i_paras->uri_params, "slot");
    }
    if (slotid_str) {
        gint32 value = -1;
        if (dal_strtoi(slotid_str, &value, DECIMAL_NUM) != RET_OK || value < 0 || value > UCHAR_MAX) {
            debug_log(DLOG_ERROR, "%s:dal_strtoi failed", __FUNCTION__);
            create_message_info(MSGID_INTERNAL_ERR, NULL, &msg_obj);
            json_object_array_add(o_err_array_jso, msg_obj);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        slotid = (guint8)value;
        return_val_do_info_if_expr((slotid == 0 || slotid > 38), HTTP_INTERNAL_SERVER_ERROR,
            create_message_info(MSGID_INTERNAL_ERR, NULL, &msg_obj);
            json_object_array_add(o_err_array_jso, msg_obj));
        components_obj = json_object_new_array();
        if (components_obj == NULL) {
            debug_log(DLOG_ERROR, "%s, %d: json_object_new_array fail.\n", __FUNCTION__, __LINE__);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        ret = dal_get_specific_object_byte((const gchar *)CLASS_NAME_IPMBETH_BLADE,
            (const gchar *)PROTERY_IPMBETH_BLADE_SLAVEADDR, get_blade_ipmbaddr(slotid), &object_handle);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST, {
            (void)json_object_put(components_obj);
            create_message_info(MSGID_ACT_PARA_UNKNOWN, NULL, &msg_obj, "get account", "slot");
            json_object_array_add(o_err_array_jso, msg_obj);
        });

        (void)get_one_ipv6config_info(object_handle, components_obj);
        json_object_object_add(o_rsc_jso, RFPROP_IPCONFIG_COMPONENTS_IPV6, components_obj);
        return VOS_OK;
    }
    components_obj = json_object_new_array();
    if (components_obj == NULL) {
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_array fail.\n", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    ret = dfl_foreach_object(CLASS_NAME_IPMBETH_BLADE, get_one_ipv6config_info, components_obj, NULL);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST, (void)json_object_put(components_obj);
        debug_log(DLOG_ERROR, "[%s] dfl_foreach_object ret=%d.", __func__, ret);
        create_message_info(MSGID_INTERNAL_ERR, NULL, &msg_obj); json_object_array_add(o_err_array_jso, msg_obj););

    json_object_object_add(o_rsc_jso, RFPROP_IPCONFIG_COMPONENTS_IPV6, components_obj);

    return VOS_OK;
}