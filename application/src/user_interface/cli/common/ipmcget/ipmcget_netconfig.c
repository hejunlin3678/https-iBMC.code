

#include "ipmcget_netconfig.h"
#include "pme_app/dalib/dalib.h"

const gchar* ip_mode_str[IP_MODE_STR_NUM] = {
    "unspecified",
    "static",
    "dhcp",
    "address loaded by BIOS or system software",
    "address obtained by BMC running other address assignment protocol",
};
const gchar* net_type_str[NET_TYPE_STR_NUM] = {
    "Unknown",
    "LOM",
    "Dedicated",
    "PCIE",
    "Aggregation",
    
    "FlexIO",
    
    
    "Dedicated",
    
    "Dedicated", 
    "Unknown",
    "Unknown",
    "OCP",
    "OCP2"
};
const gchar* net_mode_str[NET_MODE_STR_NUM] = {
    "Unknown",
    "Manual",
    "Adaptive",
};
#define LINK_STATUS_MAX 3
const gchar* link_status_str[LINK_STATUS_MAX] = {
    "Link_Down",
    "Link_Up",
    "Disabled",
};
enum en_link_status {
    link_status_down = 0,
    link_status_up,
    link_status_disabled,
};


gint32 print_mac_info(OBJ_HANDLE obj_handle)
{
    gint32 ret;
    gint32 rettemp;
    guint8 silk_num = 0;
    gchar macaddr[COMPONENT_DEVICE_NAME_LEN] = {0};
    gchar device_name[COMPONENT_DEVICE_NAME_LEN] = {0};
    gchar com_name[COMPONENT_DEVICE_NAME_LEN] = {0};
    gchar com_prop[COMPONENT_DEVICE_NAME_LEN] = {0};
    OBJ_HANDLE com_handle = 0;
    OBJ_HANDLE card_handle = 0;
    ret = dal_get_property_value_string(obj_handle, BUSY_ETH_GROUP_ATTRIBUTE_MAC, macaddr, sizeof(macaddr));
    if (ret != RET_OK) {
        return RET_ERR;
    }

    if ((g_strcmp0(macaddr, MAC_ADDR_DEFAULT) == 0) || (g_strcmp0(macaddr, INVALID_DATA_STRING) == 0)) {
        (void)memset_s(macaddr, sizeof(macaddr), 0, sizeof(macaddr));
        ret = dal_get_property_value_string(obj_handle, BUSY_ETH_ATTRIBUTE_ACTUAL_MAC_ADDR, macaddr, sizeof(macaddr));
        if (ret != RET_OK) {
            return RET_ERR;
        }
        if (g_strcmp0(macaddr, INVALID_DATA_STRING) == 0) {
            // 重置为默�?mac
            (void)memset_s(macaddr, sizeof(macaddr), 0, sizeof(macaddr));
            ret = strncpy_s(macaddr, sizeof(macaddr), MAC_ADDR_DEFAULT, strlen(MAC_ADDR_DEFAULT));
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
                return RET_ERR;
            }
        }
    }

    ret = dfl_get_referenced_property(obj_handle, BUSY_ETH_ATTRIBUTE_FRU_DEVICEID, com_name, com_prop,
        COMPONENT_DEVICE_NAME_LEN, COMPONENT_DEVICE_NAME_LEN);
    rettemp = dfl_get_object_handle(com_name, &com_handle);
    if (ret == DFL_OK) {
        ret = rettemp;
    }

    if (ret != DFL_OK) {
        ret = dfl_get_referenced_object(obj_handle, BUSY_ETH_ATTRIBUTE_REF_NETCARD, &card_handle);
        if (ret == DFL_OK) {
            ret = dfl_get_referenced_object(card_handle, PROPERTY_NETCARD_REF_COMPONENT, &com_handle);
            if (ret == DFL_OK) {
                ret = dal_get_property_value_string(com_handle, PROPERTY_COMPONENT_DEVICE_NAME, device_name,
                    sizeof(device_name));
            }
        }
        
    } else {
        ret =
            dal_get_property_value_string(com_handle, PROPERTY_COMPONENT_DEVICE_NAME, device_name, sizeof(device_name));
    }

    // 如果获取丝印失败，则表明1、没有Component 2、网口关联不到网卡 3、属性配错或dfl接口错误
    if (ret != RET_OK) {
        snprintf_s(device_name, sizeof(device_name), sizeof(device_name) - 1, "%s", "PCIe Card");
    }

    ret = dal_get_property_value_byte(obj_handle, BUSY_ETH_ATTRIBUTE_SILK_NUM, &silk_num);
    if (ret == RET_OK && silk_num != 0xff) {
        g_printf("%-13s | %-4s%-6d | %s\r\n", device_name, "Port", silk_num, macaddr);
    } else {
        g_printf("%-13s | %-10s | %s\r\n", device_name, "N/A", macaddr);
    }
    return RET_OK;
}


gint32 check_port_virtual_netcard(OBJ_HANDLE obj_handle)
{
    guint8 virtualflag = 0;
    gint32 ret;
    OBJ_HANDLE ref_handle;

    
    ret = dfl_get_referenced_object(obj_handle, BUSY_ETH_ATTRIBUTE_REF_NETCARD, &ref_handle);
    if ((ret != DFL_OK)) {
        debug_log(DLOG_ERROR, "%s, get ref handle fail, result is %d\n", dfl_get_object_name(obj_handle), ret);
        return ret;
    }

    
    ret = dal_get_property_value_byte(ref_handle, PROPERTY_NETCARD_VIRTUAL_FLAG, &virtualflag);
    if ((ret != RET_OK)) {
        debug_log(DLOG_ERROR, "get virtual NetCard Flag fail, result is %d\n", ret);
        return ret;
    }

    
    return (virtualflag == 0x1) ? DFL_OK : DFL_ERR;
}


LOCAL gint32 get_ethgroup_id(OBJ_HANDLE obj_handle)
{
    gint32 ret;
    guint8 group_id;
    ;
    GVariant *value = NULL;

    ret = dfl_get_property_value(obj_handle, PROPERTY_COMPONENT_GROUPID, &value);
    if (ret != DFL_OK) {
        return RET_ERR;
    }

    group_id = g_variant_get_byte(value);
    g_printf("EthGroup ID               :  %u\r\n", group_id + 1);
    g_variant_unref(value);
    return RET_OK;
}


gint32 is_support_ncsi(guint8 net_type)
{
    if ((net_type == NET_TYPE_LOM) || (net_type == NET_TYPE_PCIE) || (net_type == NET_TYPE_LOM2) ||
        (net_type == NET_TYPE_OCP_CARD) || (net_type == NET_TYPE_OCP2_CARD)) {
        return RET_OK;
    }
    return RET_ERR;
}

LOCAL gboolean _is_support_pcie_mgnt_net_type(void)
{
    OBJ_HANDLE obj_handle_tmp = OBJ_HANDLE_COMMON;
    gint32 ret;

    ret = dal_get_specific_object_byte(ETH_CLASS_NAME_MGNTPORTCAP, PROPERTY_MNGTPORTCAP_PCIEMGNT_ENABLE, ENABLE,
        &obj_handle_tmp);
    return (ret == RET_OK) ? TRUE : FALSE;
}


LOCAL gint32 get_ethgroup_activeport(OBJ_HANDLE obj_handle)
{
    guint8 net_type = 0;
    guint8 eth_num = 0;
    guint8 ncsi_port = 0;
    guint8 silk_num = 0xFF;

    
    gint32 ret = dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_TYPE, &net_type);
    if (ret != RET_OK) {
        g_printf("Net Type                  :  \r\n");
        return RET_ERR;
    }

    
    g_printf("Net Type                  :  %s\r\n", (net_type > NET_TYPE_MAX - 1) ? "Unknown" : net_type_str[net_type]);
    

    

    if (is_support_ncsi(net_type) == RET_OK) {
        ret = dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_NCSI_PORT, &ncsi_port);
        if (ret != RET_OK) {
            g_printf("Net Port                  :  Unknown\r\n");
            return RET_ERR;
        }

        ret = dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_ACTIVE_PORT, &eth_num);
        if (ret != RET_OK) {
            g_printf("Net Port                  :  Unknown\r\n");
            return RET_ERR;
        }

        ret = dal_get_business_silkport(net_type, eth_num, ncsi_port, &silk_num);
        if (ret != RET_OK) {
            g_printf("Net Port                  :  Unknown\r\n");
            return RET_ERR;
        }

        if (silk_num == 0xFF) {
            g_printf("Net Port                  :  Unknown\r\n");
            return RET_ERR;
        }

        g_printf("Net Port                  :  %u\r\n", silk_num);
    } else if (_is_support_pcie_mgnt_net_type() == TRUE) {
        ret = dal_get_dedicate_or_aggregation_port_silk_num(obj_handle, &silk_num);
        if (ret == RET_OK) {
            g_printf("Net Port                  :  %u\r\n", silk_num);
        } else {
            g_printf("Net Port                  :  Unknown\r\n");
        }
    }

    if (net_type == NET_TYPE_DEDICATED) {
        ret = dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_ACTIVE_PORT, &eth_num);
        if (ret == RET_OK) {
            g_printf("Active Port               :  eth%u\r\n", eth_num);
        } else {
            g_printf("Active Port               :  Unknown\r\n");
        }
    }

    return RET_OK;
    
}

LOCAL gint32 get_net_mode(OBJ_HANDLE obj_handle)
{
    gint32 ret;
    guint8 net_mode;
    GVariant *value = NULL;

    ret = dfl_get_property_value(obj_handle, ETH_GROUP_ATTRIBUTE_NET_MODE, &value);
    if (ret != DFL_OK) {
        g_printf("Net Mode                  :  \r\n");
        return RET_ERR;
    }

    net_mode = g_variant_get_byte(value);
    g_printf("Net Mode                  :  %s\r\n", (net_mode > 2) ? "Unknown" : net_mode_str[net_mode]);
    g_variant_unref(value);

    return RET_OK;
}

LOCAL gint32 get_eth_mac(OBJ_HANDLE obj_handle)
{
    gint32 ret;
    const gchar *mac = NULL;
    GVariant *value = NULL;

    ret = dfl_get_property_value(obj_handle, ETH_GROUP_ATTRIBUTE_MAC, &value);
    if (ret != DFL_OK) {
        g_printf("MAC Address               :  \r\n");
        return RET_ERR;
    } else {
        mac = g_variant_get_string(value, 0);
        if (mac) {
            g_printf("MAC Address               :  %s\r\n", mac);
        } else {
            g_printf("MAC Address               :  \r\n");
        }
    }

    g_variant_unref(value);
    return RET_OK;
}


LOCAL void __print_eth_vlan_info(OBJ_HANDLE eth, guchar eth_type)
{
#define ETH_VLAN_INFO_BUFF_SIZE 64
    gint32 ret;
    guint32 vlan_state = 0;
    guint16 vlan_id = 0;
    gchar tmp_buf[ETH_VLAN_INFO_BUFF_SIZE] = {0};

    ret = dal_get_property_value_uint32(eth, ETH_ATTRIBUTE_VLAN_STATE, &vlan_state);
    if (ret != DFL_OK) {
        goto ERR_OUT;
    }

    ret = dal_get_property_value_uint16(eth, ETH_ATTRIBUTE_VLAN_ID, &vlan_id);
    if (ret != DFL_OK) {
        goto ERR_OUT;
    }

    if (eth_type == NET_TYPE_CABINET_VLAN) {
        ret = snprintf_s(tmp_buf, sizeof(tmp_buf), sizeof(tmp_buf) - 1, "%s", "VLAN State");
    } else {
        ret = snprintf_s(tmp_buf, sizeof(tmp_buf), sizeof(tmp_buf) - 1,
            "%s VLAN State", dal_get_vlan_name_by_eth_type(eth_type));
    }
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s failed[%d]", __FUNCTION__, ret);
        goto ERR_OUT;
    }
    g_printf("%-25s :  %s\r\n", tmp_buf, vlan_state ? "enabled" : "disabled");

    if (vlan_state == ENABLE_ETH) {
        if (eth_type == NET_TYPE_CABINET_VLAN) {
            ret = snprintf_s(tmp_buf, sizeof(tmp_buf), sizeof(tmp_buf) - 1, "%s", "VLAN ID");
        } else {
            ret = snprintf_s(tmp_buf, sizeof(tmp_buf), sizeof(tmp_buf) - 1,
                "%s VLAN ID", dal_get_vlan_name_by_eth_type(eth_type));
        }
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s failed[%d]", __FUNCTION__, ret);
            goto ERR_OUT;
        }
        g_printf("%-25s :  %u\r\n", tmp_buf, vlan_id);
    }

    return;
ERR_OUT:
    g_printf("Get %6s VLAN information failed.\r\n", dal_get_vlan_name_by_eth_type(eth_type));
}


LOCAL gint32 __print_ncsi_vlan_info(OBJ_HANDLE eth, gpointer user_data)
{
    gint32 ret;
    guint8 *tar_group_id = (guint8 *)user_data;
    guint8 group_id = 0;
    guint8 eth_type = 0;
    guint8 i;
    guint8 ncsi_eth_type[] = {
        NET_TYPE_LOM,
        NET_TYPE_PCIE,
        NET_TYPE_LOM2,
        NET_TYPE_OCP_CARD,
        NET_TYPE_OCP2_CARD,
        NET_TYPE_CABINET_VLAN
    };

    ret = dal_get_property_value_byte(eth, ETH_ATTRIBUTE_GROUP_ID, &group_id);
    if (ret != DFL_OK) {
        return RET_OK;  // dfl_foreach_object调用该函数，返回RET_OK表示继续用下一个eth对象为参数进行调用
    }
    if (*tar_group_id != group_id) {
        return RET_OK;  // dfl_foreach_object调用该函数，返回RET_OK表示继续用下一个eth对象为参数进行调用
    }

    ret = dal_get_property_value_byte(eth, ETH_ATTRIBUTE_TYPE, &eth_type);
    if (ret != DFL_OK) {
        return RET_OK;  // dfl_foreach_object调用该函数，返回RET_OK表示继续用下一个eth对象为参数进行调用
    }

    for (i = 0; i < ARRAY_SIZE(ncsi_eth_type); i++) {
        if (eth_type == ncsi_eth_type[i]) {
            __print_eth_vlan_info(eth, eth_type);
            return RET_ERR; 
        }
    }

    return RET_OK; 
}


LOCAL gint32 __print_dedicated_vlan_info(OBJ_HANDLE eth, gpointer user_data)
{
    gint32 ret;
    guint8 *tar_group_id = (guint8 *)user_data;
    guint8 group_id = 0;
    guint8 eth_type = 0;

    ret = dal_get_property_value_byte(eth, ETH_ATTRIBUTE_GROUP_ID, &group_id);
    if (ret != DFL_OK) {
        return RET_OK;  // dfl_foreach_object调用该函数，返回RET_OK表示继续用下一个eth对象为参数进行调用
    }
    if (*tar_group_id != group_id) {
        return RET_OK;  // dfl_foreach_object调用该函数，返回RET_OK表示继续用下一个eth对象为参数进行调用
    }

    ret = dal_get_property_value_byte(eth, ETH_ATTRIBUTE_TYPE, &eth_type);
    if (ret != DFL_OK) {
        return RET_OK;  // dfl_foreach_object调用该函数，返回RET_OK表示继续用下一个eth对象为参数进行调用
    }
    if (eth_type != NET_TYPE_DEDICATED) {
        return RET_OK;  // dfl_foreach_object调用该函数，返回RET_OK表示继续用下一个eth对象为参数进行调用
    }

    __print_eth_vlan_info(eth, eth_type);
    return RET_ERR; 
}


LOCAL gint32 __get_eth_group_vlan_info(OBJ_HANDLE eth_group)
{
    gint32 ret;
    guint8 group_id = 0;

    ret = dal_get_property_value_byte(eth_group, ETH_GROUP_ATTRIBUTE_GROUP_ID, &group_id);
    if (ret != DFL_OK) {
        g_printf("Get VLAN information failed.\r\n");
        return RET_ERR;
    }

    g_printf("%-25s :\r\n", "VLAN Information");
    (void)dfl_foreach_object(ETH_CLASS_NAME_ETH, __print_ncsi_vlan_info, &group_id, NULL);
    if (!dal_is_dedicated_pcie_mgnt_port_existed()) {
        (void)dfl_foreach_object(ETH_CLASS_NAME_ETH, __print_dedicated_vlan_info, &group_id, NULL);
    }

    return RET_OK;
}

LOCAL gint32 get_slaac_ipv6_info(OBJ_HANDLE obj_handle)
{
    gint32 ret;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;
    const gchar **ip_addr = NULL;
    const guint8 *prefix = NULL;
    gsize prefix_count = 0;
    gsize ip_addr_count = 0;
    guint32 i;

    
    property_name_list = g_slist_append(property_name_list, ETH_GROUP_ATTRIBUTE_SLAAC_IPV6_ADDR);
    property_name_list = g_slist_append(property_name_list, ETH_GROUP_ATTRIBUTE_SLAAC_IPV6_PREFIX);
    ret = dfl_multiget_property_value(obj_handle, property_name_list, &property_value);
    if (ret != DFL_OK) {
        g_slist_free(property_name_list);
        return ret;
    }

    
    ip_addr = g_variant_get_strv((GVariant *)g_slist_nth_data(property_value, 0), &ip_addr_count);
    if (ip_addr == NULL) {
        debug_log(DLOG_DEBUG, "[%s:%d]  ip_addr NULL \n", __FUNCTION__, __LINE__);
        g_slist_free(property_name_list);
        uip_free_gvariant_list(property_value);
        return RET_ERR;
    }

    
    prefix = (const guint8 *)g_variant_get_fixed_array((GVariant *)g_slist_nth_data(property_value, 1), &prefix_count,
        sizeof(guint8));
    if (prefix == NULL) {
        g_slist_free(property_name_list);
        uip_free_gvariant_list(property_value);
        g_free(ip_addr);
        g_printf("g_variant_get_fixed_array() fail!\n");
        return RET_ERR;
    }

    for (i = 0; i < ip_addr_count; i++) {
        if (strlen(ip_addr[i]) != 0) {
            g_printf("IPv6 Address %-2d           :  %s/%d\r\n", i + 2, ip_addr[i], prefix[i]);
        }
    }

    

    g_slist_free(property_name_list);
    uip_free_gvariant_list(property_value);
    g_free(ip_addr);

    return RET_OK;
}


LOCAL gint32 get_ipv6_info(OBJ_HANDLE obj_handle)
{
    gsize addr_len = 0;
    gsize gate_len = 0;
    gsize local_len = 0;
    gint32 ret;
    guint8 ip_mode;
    guint8 prefix;
    const gchar *ip_addr = NULL;
    const gchar *gateway = NULL;
    const gchar *linkip = NULL;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;

    
    property_name_list = g_slist_append(property_name_list, ETH_GROUP_ATTRIBUTE_IPV6_MODE);
    property_name_list = g_slist_append(property_name_list, ETH_GROUP_ATTRIBUTE_IPV6_ADDR);
    property_name_list = g_slist_append(property_name_list, ETH_GROUP_ATTRIBUTE_IPV6_GATEWAY);
    property_name_list = g_slist_append(property_name_list, ETH_GROUP_ATTRIBUTE_IPV6_PREFIX);
    property_name_list = g_slist_append(property_name_list, ETH_GROUP_ATTRIBUTE_IPV6_LOCAL_LINK);
    g_printf("IPv6 Information          :\r\n");
    ret = dfl_multiget_property_value(obj_handle, property_name_list, &property_value);
    if (ret != DFL_OK) {
        g_printf("IPv6 Mode                 :  \r\n");
        
        g_printf("IPv6 Address 1            :  \r\n");
        
        g_printf("Default Gateway IPv6      :  \r\n");
        g_printf("Link-Local Address        :  \r\n");

        g_slist_free(property_name_list);

        return RET_ERR;
    }

    
    ip_mode = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 0));
    g_printf("IPv6 Mode                 :  %s\r\n", (ip_mode > IP_MODE_NUM - 1) ? "unsupport" : ip_mode_str[ip_mode]);

    
    ip_addr = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 1), &addr_len);

    
    prefix = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 3));
    if ((ip_addr != NULL) && (addr_len)) {
        
        g_printf("IPv6 Address 1            :  %s/%d\r\n", ip_addr, prefix);
        
    } else {
        
        g_printf("IPv6 Address 1            :  \r\n");
        
    }

    
    gateway = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 2), &gate_len);
    if ((gateway != NULL) && (gate_len)) {
        g_printf("Default Gateway IPv6      :  %s\r\n", gateway);
    } else {
        g_printf("Default Gateway IPv6      :  \r\n");
    }

    
    linkip = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 4), &local_len);
    if ((linkip != NULL) && (local_len)) {
        g_printf("Link-Local Address        :  %s/%d\r\n", linkip, DEFAULT_PREFIX);
    } else {
        g_printf("Link-Local Address        :  \r\n");
    }

    
    (void)get_slaac_ipv6_info(obj_handle);
    

    g_slist_free(property_name_list);
    uip_free_gvariant_list(property_value);

    return RET_OK;
}


LOCAL void printf_ipv4_info(void)
{
    g_printf("IP Mode                   :  \r\n");
    g_printf("IP Address                :  \r\n");
    g_printf("Subnet Mask               :  \r\n");
    g_printf("Default Gateway           :  \r\n");
    g_printf("Backup IP Address         :  \r\n");
    g_printf("Backup Subnet Mask        :  \r\n");
}


LOCAL void __printf_ipv4_info_normal(GSList *property_value, guint8 status)
{
    guint8 ip_mode = 0;
    const gchar *ip_addr = NULL;
    const gchar *submask = NULL;
    const gchar *gateway = NULL;
    const gchar *backup_ip_addr = NULL;
    const gchar *backup_submask = NULL;
    gsize info_len = 0;

    
    ip_mode = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 0));
    g_printf("IP Mode                   :  %s\r\n",
        (ip_mode > IP_MODE_NUM - 1) ? "unsupport" : ip_mode_str[ip_mode]);

    
    ip_addr = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 1), &info_len);
    if (info_len) {
        g_printf("IP Address                :  %s\r\n", ip_addr);
    } else {
        g_printf("IP Address                :  \r\n");
    }

    
    info_len = 0;
    submask = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 2), &info_len);
    if (info_len) {
        g_printf("Subnet Mask               :  %s\r\n", submask);
    } else {
        g_printf("Subnet Mask               :  \r\n");
    }

    
    info_len = 0;
    gateway = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 3), &info_len);
    if (info_len) {
        g_printf("Default Gateway           :  %s\r\n", gateway);
    } else {
        g_printf("Default Gateway           :  \r\n");
    }

    
    info_len = 0;
    backup_ip_addr = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 4), &info_len);
    if (info_len) {
        g_printf("Backup IP Address         :  %s (%s)\r\n", backup_ip_addr,
            status == ENABLE ? "Activated" : "Deactivated");
    }
    
    info_len = 0;
    backup_submask = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 5), &info_len);
    if (info_len) {
        g_printf("Backup Subnet Mask        :  %s (%s)\r\n", backup_submask,
            status == ENABLE ? "Activated" : "Deactivated");
    }
}

LOCAL gint32 get_ipv4_info(OBJ_HANDLE obj_handle)
{
    gint32 ret;
    GSList *output_list = NULL;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    guint8 status = DISABLE;

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    
    property_name_list = g_slist_append(property_name_list, ETH_GROUP_ATTRIBUTE_IP_MODE);
    property_name_list = g_slist_append(property_name_list, ETH_GROUP_ATTRIBUTE_IP_ADDR);
    property_name_list = g_slist_append(property_name_list, ETH_GROUP_ATTRIBUTE_SUB_MASK);
    property_name_list = g_slist_append(property_name_list, ETH_GROUP_ATTRIBUTE_GATEWAY);
    property_name_list = g_slist_append(property_name_list, ETH_GROUP_ATTRIBUTE_BACKUP_IP_ADDR);
    property_name_list = g_slist_append(property_name_list, ETH_GROUP_ATTRIBUTE_BACKUP_SUB_MASK);

    g_printf("IPv4 Information          :\r\n");
    ret = dfl_multiget_property_value(obj_handle, property_name_list, &property_value);
    if (ret != DFL_OK) {
        (void)printf_ipv4_info();
        g_slist_free(property_name_list);
        return RET_ERR;
    } else {
        ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
            ETH_CLASS_NAME_ETHGROUP, obj_handle, ETH_GROUP_METHOD_VIRTUAL_NETWORK_STATUS, NULL, &output_list);

        status = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 0));
        __printf_ipv4_info_normal(property_value, status);
    }

    g_slist_free(property_name_list);
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    uip_free_gvariant_list(property_value);
    return RET_OK;
}


gint32 print_ip_info(OBJ_HANDLE obj_handle, gpointer user_data)
{
    gint32 ret;
    guint8 out_type = 0;

    
    // veth 不支持cli
    ret = dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_OUT_TYPE, &out_type);
    if (ret != RET_OK) {
        return RET_OK;
    }

    
    if ((out_type == VETH_GROUP_TYPE) || (out_type == FLOATIP_ETHERNET) || (out_type == MAINT_DEDICATED_ETHERNET) ||
        (out_type == RACK_INNER_COMM_ETHERNET) || (out_type == RACK_NODE_INNER_ETHERNET)) {
        return RET_OK;
    }

    

    ret = get_ethgroup_id(obj_handle);
    if (ret != RET_OK) {
        g_printf("Get Ethgroup id failed.\r\n");
        return RET_ERR;
    }

    
    ret = get_net_mode(obj_handle);
    if (ret != RET_OK) {
        g_printf("Get net mode failed.\r\n");
        return RET_ERR;
    }

    ret = get_ethgroup_activeport(obj_handle);
    if (ret != RET_OK) {
        g_printf("Get activeport failed.\r\n");
        return RET_ERR;
    }

    
    ret = get_ipv4_info(obj_handle);
    if (ret != RET_OK) {
        g_printf("Get ipv4 info failed.\r\n");
        return RET_ERR;
    }

    
    ret = get_eth_mac(obj_handle);
    if (ret != RET_OK) {
        g_printf("Get mode failed.\r\n");
        return RET_ERR;
    }

    
    ret = get_ipv6_info(obj_handle);
    if (ret != RET_OK) {
        g_printf("Get ipv6 info failed.\r\n");
        return RET_ERR;
    }

    
    ret = __get_eth_group_vlan_info(obj_handle);
    if (ret != RET_OK) {
        g_printf("Get vlan info failed.\r\n");
        return RET_ERR;
    }

    g_printf("\n");
    return RET_OK;
}


LOCAL void get_ncsi_net_port_by_type(GSList* obj_list, guint8 net_port_type)
{
    gint32 ret;
    GSList* obj_node = NULL;
    GSList* property_name_list = NULL;
    GSList* property_value = NULL;
    guint8 num;
    guint8 type;
    guint8 avail_flag;
    guint8 linkstaus = 0;
    gchar mac_add_old[PROP_VAL_LENGTH] = {0};

    property_name_list = g_slist_append(property_name_list, BUSY_ETH_ATTRIBUTE_SILK_NUM);
    property_name_list = g_slist_append(property_name_list, BUSY_ETH_ATTRIBUTE_CARD_TYPE);
    for (obj_node = obj_list; obj_node != NULL; obj_node = obj_node->next) {
        ret = dfl_multiget_property_value((OBJ_HANDLE)obj_node->data, property_name_list, &property_value);
        if (ret != DFL_OK) {
            continue;
        }

        num = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 0));
        type = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 1));
        uip_free_gvariant_list(property_value);
        property_value = NULL;

        avail_flag = AVAILABLE_FLAG_DISABLED;
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, BUSY_ETH_ATTRIBUTE_AVAILABLEFLAG, &avail_flag);
        if ((type == NET_TYPE_PCIE || type == NET_TYPE_OCP_CARD) && (avail_flag == AVAILABLE_FLAG_VIRTUAL)) {
            continue;
        }

        
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, BUSY_ETH_ATTRIBUTE_LINK_STATUS, &linkstaus);

        // 端口被禁用则置端口状态
        if (snprintf_s(mac_add_old, sizeof(mac_add_old), sizeof(mac_add_old) - 1, "%s", MAC_ADDR_DEFAULT) < 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s failed", __FUNCTION__);
            continue;
        }
        (void)dal_get_property_value_string((OBJ_HANDLE)obj_node->data, BUSY_ETH_GROUP_ATTRIBUTE_MAC, mac_add_old,
            sizeof(mac_add_old));
        if (strcmp(mac_add_old, MAC_ADDR_DEFAULT) == 0 && avail_flag == AVAILABLE_FLAG_DISABLED) {
            linkstaus = link_status_disabled;
        }

        if (type == net_port_type) {
            g_printf("%-11s | %-4s%-6u | %-8u | %-10s\r\n", net_type_str[type], "Port", num, num,
                (linkstaus >= LINK_STATUS_MAX) ? "--" : link_status_str[linkstaus]);
        }
    }
    g_slist_free(property_name_list);
    property_name_list = NULL;
    return;
}


gint32 get_ncsi_network_port(void)
{
    gint32 ret;
    GSList* obj_list = NULL;

    ret = dfl_get_object_list(ETH_CLASS_NAME_BUSY_ETH, &obj_list);
    if (ret != DFL_OK) {
        return RET_OK;  
    }

    get_ncsi_net_port_by_type(obj_list, NET_TYPE_LOM);
    get_ncsi_net_port_by_type(obj_list, NET_TYPE_PCIE);
    get_ncsi_net_port_by_type(obj_list, NET_TYPE_LOM2);
    get_ncsi_net_port_by_type(obj_list, NET_TYPE_OCP_CARD);
    get_ncsi_net_port_by_type(obj_list, NET_TYPE_OCP2_CARD);

    g_slist_free(obj_list);
    return RET_OK;
}

LOCAL void get_dedicated_net_port_by_type(GSList* obj_list, guint8 net_port_type, guint8 input_groupid,
    OBJ_HANDLE port_mgnt_handle)
{
    GSList* obj_node = NULL;
    guint8 group_id = 0;
    guint8 eth_type = 0;
    gint32 eth_linkstaus = 0;
    gchar eth_name[MAX_PROPERTY_VALUE_LEN] = {0};
    guint8 eth_id = 0;

    for (obj_node = obj_list; obj_node != NULL; obj_node = obj_node->next) {
        group_id = 0;
        eth_type = 0;
        eth_linkstaus = 0;
        eth_id = 0;
        (void)memset_s(eth_name, sizeof(eth_name), 0, sizeof(eth_name));

        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_GROUPID, &group_id);
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, ETH_ATTRIBUTE_TYPE, &eth_type);
        (void)dal_get_property_value_string((OBJ_HANDLE)obj_node->data, ETH_ATTRIBUTE_NAME, eth_name, sizeof(eth_name));
        (void)dal_get_property_value_int32((OBJ_HANDLE)obj_node->data, ETH_ATTRIBUTE_LINK_STATUS, &eth_linkstaus);
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, ETH_ATTRIBUTE_ETH_ID, &eth_id);
        
        if (eth_type >= NET_TYPE_STR_NUM) {
            continue;
        }
        if (group_id == input_groupid && eth_type == net_port_type) {
            if (port_mgnt_handle == OBJ_HANDLE_COMMON) {
                g_printf("%-11s | %-10s | %-8s | %-10s\r\n", net_type_str[eth_type], eth_name, "na",
                    (eth_linkstaus == 1) ? "Link_Up" : "Link_Down");
            } else {
                g_printf("%-11s | %-4s%-6u | %-8u | %-10s\r\n", net_type_str[eth_type], "Port", eth_id, eth_id,
                    (eth_linkstaus == 1) ? "Link_Up" : "Link_Down");
            }
        }
    }
    return;
}


gint32 get_dedicated_network_port(guint8 input_groupid)
{
    gint32 ret;
    GSList* obj_list = NULL;
    OBJ_HANDLE port_mgnt_handle = OBJ_HANDLE_COMMON;

    ret = dfl_get_object_list(ETH_CLASS_NAME_ETH, &obj_list);
    if (ret != DFL_OK) {
        return RET_OK; 
    }

    (void)dal_get_specific_object_byte(ETH_CLASS_NAME_MGNTPORTCAP, PROPERTY_MNGTPORTCAP_PCIEMGNT_ENABLE, ENABLE,
        &port_mgnt_handle);

    get_dedicated_net_port_by_type(obj_list, NET_TYPE_DEDICATED, input_groupid, port_mgnt_handle);
    get_dedicated_net_port_by_type(obj_list, NET_TYPE_CABINET_VLAN, input_groupid, port_mgnt_handle);
    get_dedicated_net_port_by_type(obj_list, NET_TYPE_AGGREGATION, input_groupid, port_mgnt_handle);

    g_slist_free(obj_list);
    return RET_OK;
}