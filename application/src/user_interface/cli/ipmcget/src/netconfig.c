

#include "ipmcget_netconfig.h"
#include "ipmcget_sys_mgmt.h"


gint32 ipmc_get_macaddr(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    OBJ_HANDLE busi_handle;
    guint32 net_functype = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcget %s-d macaddr \r\n", help_default_name);
        return FALSE;
    }

    ret = dfl_get_object_list(ETH_CLASS_NAME_BUSY_ETH, &obj_list);
    if (ret != DFL_OK) {
        if (ret == ERRCODE_OBJECT_NOT_EXIST) {
            g_printf("System has no LOM or PCIE network port.\r\n");
        } else {
            g_printf("Get object list failed.\r\n");
        }

        return FALSE;
    }

    g_printf("%-13s | %-10s | %s\r\n", "Net Card Name", "Port Name", "Mac Address");

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        
        busi_handle = (OBJ_HANDLE)obj_note->data;
        (void)dal_get_property_value_uint32(busi_handle, BUSY_ETH_ATTRIBUTE_NETDEV_FUNCTYPE, &net_functype);
        // mac查询仅支持eth类型的网口，FC、IB不支持
        if (net_functype != NETDEV_FUNCTYPE_ETHERNET) {
            continue;
        }
        

        
        if (check_port_virtual_netcard(busi_handle) == DFL_OK) {
            continue;
        }
        

        
        (void)print_mac_info(busi_handle);
        
    }

    g_slist_free(obj_list);
    return TRUE;
}


gint32 ipmc_get_ipinfo(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    ret = dfl_foreach_object(ETH_CLASS_NAME_ETHGROUP, print_ip_info, NULL, NULL);
    if (ret != DFL_OK) {
        g_printf("Get IP information failed.\r\n");
        return FALSE;
    }

    return TRUE;
}


gint32 ipmc_get_ethport(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    guint8 groupid = dal_eth_get_out_type_groupid();

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    g_printf("%-11s | %-10s | %-8s | %-10s\r\n", "Type", "Name", "Port ID", "Link Status");
    
    ret = get_dedicated_network_port(groupid);
    if (ret != RET_OK) {
        g_printf("Get dedicated network port failed!\r\n");
        return FALSE;
    }

    
    ret = get_ncsi_network_port();
    if (ret != RET_OK) {
        g_printf("Get lom network port failed!\r\n");
        return FALSE;
    }

    return TRUE;
}


gint32 ipmc_get_autodiscovery_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    guchar enable;
    const gchar *net_segment = NULL;
    guint32 net_port;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;
    OBJ_HANDLE obj_handle;
    GSList *obj_list = NULL;
    gchar str_ipaddr[PARAMETER_LEN] = {0};
    gchar str_submask[PARAMETER_LEN] = {0};
    guint32 ipaddr = 0;
    guint32 submask = 0;
    struct sockaddr_in bcast = { 0 };
    gchar str_bcast[PARAMETER_LEN] = {0};

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

     
    gint32 ret = dfl_get_object_list(CLASS_STATELESS, &obj_list);
    if (ret != DFL_OK || obj_list == NULL) {
        g_printf("Get object failed.\n");
        return RET_ERR;
    }

    
    obj_handle = (OBJ_HANDLE)g_slist_nth_data(obj_list, 0);
    g_slist_free(obj_list);

    
    property_name_list = g_slist_append(property_name_list, PROPERTY_STATELESS_ENABLE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_STATELESS_BROADCAST_NET_SEGMENT);
    property_name_list = g_slist_append(property_name_list, PROPERTY_STATELESS_BROADCAST_NET_PORT);
    ret = dfl_multiget_property_value(obj_handle, property_name_list, &property_value);
    g_slist_free(property_name_list);
    property_name_list = NULL;
    if (ret != DFL_OK) {
        g_printf("Get autodiscovery information failed.\n");
        return RET_ERR;
    }

    enable = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 0));
    net_segment = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 1), 0);
    net_port = g_variant_get_uint32((GVariant *)g_slist_nth_data(property_value, 2));
    
    (void)strncpy_s(str_bcast, PARAMETER_LEN, net_segment, PARAMETER_LEN - 1);
    

    if (strcmp(net_segment, "255.255.255.255") != 0) {
        if (_get_outer_eth_ipaddr_mask(str_ipaddr, sizeof(str_ipaddr), str_submask, sizeof(str_submask)) != RET_OK) {
            g_printf("failed to get ip & mask\r\n");
            uip_free_gvariant_list(property_value);
            return FALSE;
        }

        (void)inet_pton(AF_INET, str_ipaddr, (void *)&ipaddr);
        (void)inet_pton(AF_INET, str_submask, (void *)&submask);
        bcast.sin_addr.s_addr = (ipaddr & submask) | (~submask);

        if ((ipaddr == INADDR_NONE) || (submask == INADDR_NONE)) {
            g_printf("failed to get ip & mask\r\n");
            uip_free_gvariant_list(property_value);
            return RET_ERR;
        }

        
        _cleanup_gfree_ gchar *ipaddr = dal_inet_ntop_ipv4(bcast.sin_addr);
        (void)strncpy_s(str_bcast, PARAMETER_LEN, ipaddr, PARAMETER_LEN - 1);
        
    }

    g_printf("State         : %s\n", (enable == TRUE) ? "enabled" : "disabled");
    g_printf("Broadcast     : %s\n", str_bcast);
    g_printf("NetPort       : %d\n", net_port);

    uip_free_gvariant_list(property_value);

    return TRUE;
}


gint32 ipmc_get_sol_session(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;
    gint32 type = -1;
    gint32 id = -1;
    const gchar *ip = NULL;
    const gchar *user_name = NULL;
    const gchar *default_val = "N/A";
    guint32 login_time = 0;
    gchar login_time_srting[PARAMETER_LEN] = {0};
    gchar* type_string[] = {"HMM", "CLI", "IPMI"};
    gint32 mode = 0;
    gchar* mode_string[] = {"Shared", "Private"};

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcget -t sol -d session \r\n");
        return FALSE;
    }

    g_printf("%-8s%-8s%-8s%-28s%-38s%s\r\n", "Index", "Type", "Mode", "LoginTime", "IP", "Name");

    ret = dfl_get_object_list(SOLLINK_CLASS_NAME, &obj_list);
    if (ret != DFL_OK || g_slist_last(obj_list) == 0) {
        return FALSE;
    }

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        
        property_name_list = g_slist_append(property_name_list, SOLLINK_PROPERTY_TYPE);
        property_name_list = g_slist_append(property_name_list, SOLLINK_PROPERTY_ID);
        property_name_list = g_slist_append(property_name_list, SOLLINK_PROPERTY_IP);
        property_name_list = g_slist_append(property_name_list, SOLLINK_PROPERTY_USER_NAME);
        property_name_list = g_slist_append(property_name_list, SOLLINK_PROPERTY_LOGIN_TIME);
        property_name_list = g_slist_append(property_name_list, SOLLINK_PROPERTY_MODE);
        ret = dfl_multiget_property_value((OBJ_HANDLE)obj_note->data, property_name_list, &property_value);
        if (ret != DFL_OK) {
            g_printf("Get SOL session failed.\r\n");
            g_slist_free(property_name_list);
            g_slist_free(obj_list);
            return FALSE;
        }

        type = g_variant_get_int32((GVariant *)g_slist_nth_data(property_value, 0));
        id = g_variant_get_int32((GVariant *)g_slist_nth_data(property_value, 1));
        ip = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 2), NULL);
        user_name = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 3), NULL);
        login_time = g_variant_get_uint32((GVariant *)g_slist_nth_data(property_value, 4));
        mode = g_variant_get_int32((GVariant *)g_slist_nth_data(property_value, 5));
        if ((type >= SOL_SESSION_HMM) && (type <= SOL_SESSION_IPMI) && (login_time != 0) &&
            (mode == SOL_SESSION_SHARED || mode == SOL_SESSION_DEDICATED)) {
            ret = parse_sol_login_time(login_time, login_time_srting, PARAMETER_LEN);
            if (ret != RET_OK) {
                g_printf("%-8d%-8s%-8s%-28s%-38s%s\r\n", id, default_val, default_val, default_val, default_val,
                    default_val);
            } else {
                g_printf("%-8d%-8s%-8s%-28s%-38s%s\r\n", id, type_string[type], mode_string[mode], login_time_srting,
                    ip, user_name);
            }
        } else {
            g_printf("%-8d%-8s%-8s%-28s%-38s%s\r\n", id, default_val, default_val, default_val, default_val,
                default_val);
        }

        g_slist_free(property_name_list);
        uip_free_gvariant_list(property_value);

        property_name_list = NULL;
        property_value = NULL;
    }

    g_slist_free(obj_list);
    return TRUE;
}


gint32 ipmc_get_sol_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gint32 timeout = -1;
    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcget -t sol -d timeout \r\n");
        return FALSE;
    }

    ret = dal_get_object_handle_nth(CLASS_NAME_SOL_MANAGEMENT, 0, &obj_handle);
    if (ret != RET_OK) {
        g_printf("Get SOL information failed.\r\n");
        return FALSE;
    }
    ret = dal_get_property_value_int32(obj_handle, PROPERTY_SOL_MANAGEMENT_TIMEOUT, &timeout);
    if (ret != RET_OK) {
        g_printf("Get SOL information failed.\r\n");
        return FALSE;
    }

    g_printf("Timeout Period(Min)        :   %d\r\n", timeout);
    return TRUE;
}
