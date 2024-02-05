

#include "ipmcset_netconfig.h"
#include "ipmcset_user_security.h"

#define ETH_SET_ERROR (-5)

#define DEDICATE_PORT_DISPLAY_INDEX 0
#define LOM_PORT_DISPLAY_INDEX 1
#define PCIE_PORT_DISPLAY_INDEX 2
#define AGGREGATION_PORT_DISPLAY_INDEX 3
#define LOM2_PORT_DISPLAY_INDEX 4
#define OCP_PORT_DISPLAY_INDEX 5
#define OCP2_PORT_DISPLAY_INDEX 6
#define MAX_ACTPORT_DISPLAY_INDEX 7
#define UNKNOWN_CARD_TYPE_DISPLAY_INDEX 255
#define SET_CMD_ARGC_NUM_TWO 2

#define NCSI_ETH_TIANCHI 0  // 天池的NCSI对应的eth0
#define NCSI_PCIE_TIANCHI 4
#define NCSI_OCP1_TIANCHI 2
#define NCSI_OCP2_TIANCHI 3

#define DEDICATE_PORT_DISPLAY_INFO "Dedicated network port, without parameter [portid]"
#define PCIE_MGNT_DEDICATE_PORT_DISPLAY_INFO "Dedicated network port"
#define LOM_PORT_DISPLAY_INFO "LOM network port"
#define PCIE_PORT_DISPLAY_INFO "PCIE network port"
#define AGGREGATION_PORT_DISPLAY_INFO "Aggregation network port, without parameter [portid]"
#define LOM2_PORT_DISPLAY_INFO "FlexIO network port"
#define OCP_PORT_DISPLAY_INFO "OCP network port"
#define OCP2_PORT_DISPLAY_INFO "OCP2 network port"


#define SET_IP_TYPE_IP_ADDR "IP address"
#define SET_IP_TYPE_IP_SUBMASK "MASK address"
#define SET_IP_TYPE_IP_GATEWAY "GATEWAY"
#define SET_IP_TYPE_IP_MODE "IP Mode"
#define SET_IP_TYPE_IP6_ADDR "IPV6 address"
#define SET_IP_TYPE_IP6_PREFIX "IPV6 prefix"
#define SET_IP_TYPE_IP6_GATEWAY "GATEWAY6"
#define SET_IP_TYPE_IP6_MODE "IPV6 Mode"
#define SET_IP_TYPE_VLAN_STATE "vlan state"
#define SET_IP_TYPE_NETMODE "net mode"
#define SET_IP_TYPE_ACTIVE_PORT "active port"


LOCAL void get_dedicated_port_switch_display_info(gchar *help_info_buf, guint32 buf_len);
LOCAL gint32 fill_set_vlan_input_list(OBJ_HANDLE obj_handle, gchar *ip_param, GSList **input_list);

LOCAL ACTIVE_PORT_MGNT_S g_active_port_mgnt[MAX_ACTPORT_DISPLAY_INDEX] = {
    {DEDICATE_PORT_DISPLAY_INDEX, PROPERTY_MNGTPORTCAP_DEDICATEDENABLE, "", get_dedicated_port_switch_display_info},
    {LOM_PORT_DISPLAY_INDEX, PROPERTY_MNGTPORTCAP_LOMNCSIENABLE, LOM_PORT_DISPLAY_INFO, NULL},
    {PCIE_PORT_DISPLAY_INDEX, PROPERTY_MNGTPORTCAP_PCIENCSIENABLE, PCIE_PORT_DISPLAY_INFO, NULL},
    {AGGREGATION_PORT_DISPLAY_INDEX, PROPERTY_MNGTPORTCAP_AGGREGATIONENABLE, AGGREGATION_PORT_DISPLAY_INFO, NULL},
    {LOM2_PORT_DISPLAY_INDEX, PROPERTY_MNGTPORTCAP_LOM2NCSIENABLE, LOM2_PORT_DISPLAY_INFO, NULL},
    {OCP_PORT_DISPLAY_INDEX, PROPERTY_MNGTPORTCAP_OCPENABLE, OCP_PORT_DISPLAY_INFO, NULL},
    {OCP2_PORT_DISPLAY_INDEX, PROPERTY_MNGTPORTCAP_OCP2ENABLE, OCP2_PORT_DISPLAY_INFO, NULL}
};


LOCAL gint32 fill_set_vlan_input_list(OBJ_HANDLE obj_handle, gchar *ip_param, GSList **input_list)
{
    gint32 ret = RET_ERR;
    guint8 vlan_state;
    gint32 vlan_id = 0;
    guint16 vlan_id_min = 0;
    guint16 vlan_id_max = 0;
    GSList *output_list = NULL;
    gchar *vlan_param = ip_param;

    if (obj_handle == 0 || ip_param == NULL || input_list == NULL) {
        return RET_ERR;
    }

    if (!strcmp(vlan_param, "off")) {
        vlan_state = DISABLE_ETH;
        vlan_id = 0;
    } else {
        ret = dfl_call_class_method(obj_handle, METHOD_SHELF_GET_BLADE_VLAN_ID_RANGE, NULL, NULL, &output_list);
        if (ret != DFL_OK) {
            g_printf("Get vlan id range fail\r\n");
            uip_free_gvariant_list(output_list);
            return RET_ERR;
        }

        vlan_id_min = g_variant_get_uint16((GVariant *)g_slist_nth_data(output_list, 0));
        vlan_id_max = g_variant_get_uint16((GVariant *)g_slist_nth_data(output_list, 1));

        uip_free_gvariant_list(output_list);

        if ((vlan_id = str_to_int(vlan_param)) == IPMC_ERROR) {
            g_printf("Usage: ipmcset -l bladeN -d vlan -v <off | id(%d~%d)>\r\n", vlan_id_min, vlan_id_max);
            return RET_ERR;
        }

        if ((vlan_id < vlan_id_min) || (vlan_id > vlan_id_max)) {
            g_printf("Vlan id must be between %d and %d.\r\n", vlan_id_min, vlan_id_max);
            return RET_ERR;
        }

        vlan_state = ENABLE_ETH;
    }

    *input_list = g_slist_append(*input_list, (gpointer)g_variant_new_byte(vlan_state));
    *input_list = g_slist_append(*input_list,
        (gpointer)g_variant_new_uint16((guint16)vlan_id)); // 这里vlanid的范围在上文中已做了限制

    return RET_OK;
}


gint32 set_blade_bmc_ip_config_by_type(guchar slave_addr, void *ip_param, gchar *type)
{
    errno_t safe_fun_ret = EOK;
    gint32 ret;
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = { 0 };
    gchar ip[PARAMETER_LEN + 1] = { 0 };
    GSList *caller_info = NULL;
    gchar *method_name = NULL;
    OBJ_HANDLE obj_handle = 0;
    guchar active_port[2] = {0};
    gint32 i;
    gchar blade_name[SHORT_STRING_LEN] = { 0 };

    if (ip_param == NULL || type == NULL) {
        return RET_ERR;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    ret = dal_get_specific_object_byte(CLASS_NAME_IPMBETH_BLADE, PROTERY_IPMBETH_BLADE_SLAVEADDR, slave_addr,
        &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get %s obj failed. %s=%d, ret:%d", __func__, CLASS_NAME_IPMBETH_BLADE,
            PROTERY_IPMBETH_BLADE_SLAVEADDR, slave_addr, ret);
        return RET_ERR;
    }

    
    (void)strncpy_s(blade_name, sizeof(blade_name), dfl_get_object_name(obj_handle), sizeof(blade_name) - 1);
    

    caller_info = g_slist_append(caller_info, g_variant_new_string(OPERATOR_LOG_CLI));
    caller_info = g_slist_append(caller_info, g_variant_new_string(username)),
    caller_info = g_slist_append(caller_info, g_variant_new_string(ip));

    input_list = g_slist_append(input_list, g_variant_new_byte(slave_addr));
    if (!strcmp(type, SET_IP_TYPE_IP_ADDR)) {
        method_name = METHOD_SET_BLADE_BMC_IPADDR;
        input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)ip_param));
    } else if (!strcmp(type, SET_IP_TYPE_IP_SUBMASK)) {
        method_name = METHOD_SET_BLADE_BMC_SUBNETMASK;
        input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)ip_param));
    } else if (!strcmp(type, SET_IP_TYPE_IP_GATEWAY)) {
        method_name = METHOD_SET_BLADE_BMC_GATEWAY;
        input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)ip_param));
    } else if (!strcmp(type, SET_IP_TYPE_IP_MODE)) {
        method_name = METHOD_SET_BLADE_BMC_IPMODE;
        input_list = g_slist_append(input_list, g_variant_new_byte(*((guchar *)ip_param)));
    } else if (!strcmp(type, SET_IP_TYPE_IP6_ADDR)) {
        method_name = METHOD_SET_BLADE_BMC_IP6ADDR;
        input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)ip_param));
    } else if (!strcmp(type, SET_IP_TYPE_IP6_PREFIX)) {
        method_name = METHOD_SET_BLADE_BMC_IP6PREFIX;
        input_list = g_slist_append(input_list, g_variant_new_byte(*((guchar *)ip_param)));
    } else if (!strcmp(type, SET_IP_TYPE_IP6_GATEWAY)) {
        method_name = METHOD_SET_BLADE_BMC_IP6GATEWAY;
        input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)ip_param));
    } else if (!strcmp(type, SET_IP_TYPE_IP6_MODE)) {
        method_name = METHOD_SET_BLADE_BMC_IP6MODE;
        input_list = g_slist_append(input_list, g_variant_new_byte(*((guchar *)ip_param)));
    }
    
    else if (!strcmp(type, SET_IP_TYPE_VLAN_STATE)) {
        method_name = METHOD_SET_BLADE_BMC_VLAN_INFO;
        // 根据参数值填充vlan方法的input_list
        ret = fill_set_vlan_input_list(obj_handle, (gchar *)ip_param, &input_list);
        if (ret != RET_OK) {
            uip_free_gvariant_list(input_list);
            uip_free_gvariant_list(caller_info);
            return RET_ERR;
        }
    } else if (!strcmp(type, SET_IP_TYPE_NETMODE)) {
        method_name = METHOD_SET_BLADE_BMC_NETMODE;
        input_list = g_slist_append(input_list, g_variant_new_byte(*((guchar *)ip_param)));
    } else if (!strcmp(type, SET_IP_TYPE_ACTIVE_PORT)) {
        method_name = METHOD_SET_BLADE_BMC_ACTIVE_PORT;
        safe_fun_ret = memcpy_s(active_port, sizeof(active_port), ip_param, sizeof(active_port));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        input_list = g_slist_append(input_list, g_variant_new_byte(active_port[0]));
        input_list = g_slist_append(input_list, g_variant_new_byte(active_port[1]));
    }
    
    
    for (i = 0; i < 3; i++) {
        ret = dfl_call_class_method(obj_handle, method_name, caller_info, input_list, NULL);
        if (ret == 0xc3) { // 超时则重试
            // MM810给节点设置ip、gw时,如果节点正在压缩日志会使cpu占用率高(时间1-2s),此处判断节点返回超时则最多重试2次
            vos_task_delay(3000);
            continue;
        }
        break; // 其他返回码则退出
    }
    

    if (ret != RET_OK) {
        
        if (!strcmp(type, SET_IP_TYPE_ACTIVE_PORT) && ret == RMCP_INVALID_DATA_FIELD) {
            // Port not exist
            g_printf("The port of %s entered does not exist.\r\n", blade_name);
        } else if (!strcmp(type, SET_IP_TYPE_ACTIVE_PORT) && (-3) == ret) { // 返回 -3是网口模式不支持手动切换网口
            // 网络模式是自适应
            g_printf("The current netmode of %s does not support activation of ports.\r\n", blade_name);
        } else {
            print_blade_err_info(blade_name, ret);
            g_printf("Set %s %s failed.\r\n", blade_name, type);
        }
        
        uip_free_gvariant_list(input_list);
        uip_free_gvariant_list(caller_info);
        return RET_ERR;
    }

    g_printf("Set %s %s successfully.\r\n", blade_name, type);
    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(caller_info);
    return RET_OK;
}

LOCAL void get_dedicated_port_switch_display_info(gchar *help_info_buf, guint32 buf_len)
{
    gint32 ret;
    OBJ_HANDLE obj_handle_tmp = OBJ_HANDLE_COMMON;

    if ((help_info_buf == NULL) || (buf_len == 0)) {
        return;
    }

    ret = dal_get_specific_object_byte(ETH_CLASS_NAME_MGNTPORTCAP, PROPERTY_MNGTPORTCAP_PCIEMGNT_ENABLE, ENABLE,
        &obj_handle_tmp);
    if (ret != RET_OK) {
        
        (void)strncpy_s(help_info_buf, buf_len, DEDICATE_PORT_DISPLAY_INFO, strlen(DEDICATE_PORT_DISPLAY_INFO));
    } else {
        
        (void)strncpy_s(help_info_buf, buf_len, PCIE_MGNT_DEDICATE_PORT_DISPLAY_INFO,
            strlen(PCIE_MGNT_DEDICATE_PORT_DISPLAY_INFO));
    }

    return;
}


LOCAL void _print_multi_dedicated_active_helpinfo(void)
{
    GSList* eth_list = NULL;
    GSList* eth_list_iter = NULL;
    guint8 eth_type;
    OBJ_HANDLE obj_handle_iter;
    gint32 ret;
    guint8 out_type;
    guint8 eth_id;

    g_printf("    0      :  Dedicated network port, with parameter [portid]\r\n");

    ret = dfl_get_object_list(ETH_CLASS_NAME_ETH, &eth_list);
    if (ret != RET_OK) {
        return;
    }

    g_printf("portids are:\r\n");
    for (eth_list_iter = eth_list; eth_list_iter != NULL; eth_list_iter = eth_list_iter->next) {
        obj_handle_iter = (OBJ_HANDLE)eth_list_iter->data;
        eth_type = 0;
        out_type = 0;
        (void)dal_get_property_value_byte(obj_handle_iter, ETH_ATTRIBUTE_OUT_TYPE, &out_type);
        if (out_type != OUTTER_GROUP_TYPE) {
            continue;
        }

        (void)dal_get_property_value_byte(obj_handle_iter, ETH_ATTRIBUTE_TYPE, &eth_type);
        (void)dal_get_property_value_byte(obj_handle_iter, ETH_ATTRIBUTE_NUM, &eth_id);

        if (eth_type == NET_TYPE_DEDICATED) {
            g_printf("    %d      :  eth%d\r\n", eth_id, eth_id);
        }
    }

    g_slist_free(eth_list);
    return;
}


gint32 ipmc_judge_netport_exist(guint8 net_type, guint8 port_num)
{
    gint32 ret;
    guint32 obj_num;
    guint8 type = 0;
    guint8 num = 0;
    GSList *property_name_list = NULL;
    GSList *property_value_list = NULL;
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;

    if (net_type == NET_TYPE_DEDICATED) {
        if (port_num == 0) {
            return RET_OK;
        }

        return dal_check_dedicated_port_exist(port_num);
    }

    if (net_type == NET_TYPE_AGGREGATION) {
        return RET_OK;
    }

    ret = dfl_get_object_list(ETH_CLASS_NAME_BUSY_ETH, &obj_list);
    if (ret != DFL_OK) {
        return RET_ERR;
    }

    obj_num = g_slist_length(obj_list);
    if (obj_num == 0) {
        return RET_ERR;
    }

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        
        property_name_list = g_slist_append(property_name_list, BUSY_ETH_ATTRIBUTE_CARD_TYPE);
        property_name_list = g_slist_append(property_name_list, BUSY_ETH_ATTRIBUTE_SILK_NUM);
        ret = dfl_multiget_property_value((OBJ_HANDLE)obj_note->data, property_name_list, &property_value_list);
        if (ret != DFL_OK) {
            g_slist_free(obj_list);
            g_slist_free(property_name_list);
            return RET_ERR;
        }

        type = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, 0));
        num = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, 1));

        g_slist_free(property_name_list);
        uip_free_gvariant_list(property_value_list);

        property_name_list = NULL;
        property_value_list = NULL;

        if ((net_type == type) && (port_num == num)) {
            g_slist_free(obj_list);
            return RET_OK;
        }

        
    }

    g_slist_free(obj_list);
    return RET_ERR;
}

LOCAL gboolean check_ocp_preplug_status(guint8 actport_index)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    
    if (actport_index == OCP_PORT_DISPLAY_INDEX) {
        ret = dal_get_specific_object_byte(ETH_CLASS_NAME_BUSY_ETH, BUSY_ETH_ATTRIBUTE_CARD_TYPE,
            NET_TYPE_OCP_CARD, &obj_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "OCP1 is in preplug stutas, ret=%d", ret);
            return FALSE;
        }
    } else {
        ret = dal_get_specific_object_byte(ETH_CLASS_NAME_BUSY_ETH, BUSY_ETH_ATTRIBUTE_CARD_TYPE,
            NET_TYPE_OCP2_CARD, &obj_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "OCP2 is in preplug stutas, ret=%d", ret);
            return FALSE;
        }
    }
    return TRUE;
}


LOCAL gint32 get_actport_enable_tianchi(OBJ_HANDLE obj_handle, guint8 *actport_enable, guint32 *actport_supported)
{
    OBJ_HANDLE eth_obj_handle = 0;
    guint8 ncsi_channel = 0;
    guint8 i;
    
    gint32 ret = dal_get_specific_object_byte(ETH_CLASS_NAME_ETH, ETH_ATTRIBUTE_NUM, NCSI_ETH_TIANCHI, &eth_obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get eth%d fail!(ret=%d)", NCSI_ETH_TIANCHI, ret);
        return ret;
    }
    ret = dal_get_property_value_byte(eth_obj_handle, ETH_ATTRIBUTE_NCSI_NIC_SWITCH_ACCESSOR, &ncsi_channel);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get eth%d ncsi channel fail!(ret=%d)", NCSI_ETH_TIANCHI, ret);
        return ret;
    }

    for (i = 0; i < MAX_ACTPORT_DISPLAY_INDEX; i++) {
        
        if (i != PCIE_PORT_DISPLAY_INDEX && i != OCP_PORT_DISPLAY_INDEX && i != OCP2_PORT_DISPLAY_INDEX) {
            ret = dal_get_property_value_byte(obj_handle, g_active_port_mgnt[i].proper_name, &actport_enable[i]);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "dal_get_property_value_byte %s fail!(ret=%d)",
                    g_active_port_mgnt[i].proper_name, ret);
                return ret;
            }
            *actport_supported += actport_enable[i];
        } else if (i == PCIE_PORT_DISPLAY_INDEX && ncsi_channel == NCSI_PCIE_TIANCHI) {
            ret = dal_get_property_value_byte(obj_handle, g_active_port_mgnt[i].proper_name, &actport_enable[i]);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "get pcie ncsi enable fail!(ret=%d)", ret);
                return ret;
            }
            *actport_supported += actport_enable[i];
        } else if ((i == OCP_PORT_DISPLAY_INDEX && ncsi_channel == NCSI_OCP1_TIANCHI) ||
                   (i == OCP2_PORT_DISPLAY_INDEX && ncsi_channel == NCSI_OCP2_TIANCHI)) {
            
            if (!check_ocp_preplug_status(i)) {
                continue;
            }
            ret = dal_get_property_value_byte(obj_handle, g_active_port_mgnt[i].proper_name, &actport_enable[i]);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "dal_get_property_value_byte %s fail!(ret=%d)",
                    g_active_port_mgnt[i].proper_name, ret);
                return ret;
            }
            *actport_supported += actport_enable[i];
        }
    }
    return RET_OK;
}

void print_activeport_helpinfo(void)
{
#define MAX_ACTIVEPORT_SWITCH_HELP_INFO_BUF_LEN 256
    guint8 actport_enable[MAX_ACTPORT_DISPLAY_INDEX] = {0};
    guint32 actport_supported = 0;
    OBJ_HANDLE obj_handle = 0;
    guint8 i;
    gchar extra_display_buf[MAX_ACTIVEPORT_SWITCH_HELP_INFO_BUF_LEN] = {0};

    
    
    gint32 ret = dfl_get_object_handle(ETH_CLASS_NAME_MGNTPORTCAP, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "MngtPortCap failed, result is %d", ret);
        return;
    }

    if (dal_match_product_id(PRODUCT_ID_TIANCHI) == TRUE) {
        if (get_actport_enable_tianchi(obj_handle, actport_enable, &actport_supported) != RET_OK) {
            debug_log(DLOG_ERROR, "get_actport_enable_tianchi fail!");
            return;
        }
    } else {
        for (i = 0; i < MAX_ACTPORT_DISPLAY_INDEX; i++) {
            ret = dal_get_property_value_byte(obj_handle, g_active_port_mgnt[i].proper_name, &actport_enable[i]);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "dal_get_property_value_byte %s fail!(ret=%d)",
                    g_active_port_mgnt[i].proper_name, ret);
                return;
            }
            actport_supported += actport_enable[i];
        }
    }

    // 把各种NCSI的使能开关都加起来，如果为0就表示不支持active port命令
    if (actport_supported == 0) {
        g_printf("The system is not support to active port.\n");
        return;
    }

    g_printf("Usage: ipmcset -d activeport -v <option> [portid]\r\n");
    g_printf("Options are:\r\n");
    if (dal_check_ar_card_support()) {
        _print_multi_dedicated_active_helpinfo();
        return;
    }

    for (i = 0; i < MAX_ACTPORT_DISPLAY_INDEX; i++) {
        if (actport_enable[i] == 1) {
            (void)memset_s(extra_display_buf, sizeof(extra_display_buf), 0, sizeof(extra_display_buf));
            if (g_active_port_mgnt[i].display_extra_info_func != NULL) {
                g_active_port_mgnt[i].display_extra_info_func(extra_display_buf, sizeof(extra_display_buf));
            }

            g_printf("    %d      :  %s%s\r\n", g_active_port_mgnt[i].display_index, g_active_port_mgnt[i].display_info,
                extra_display_buf);
        }
    }
    return;
}

gint32 ipmc_judge_mntportcab_valiad(guint8 net_type_index)
{
    gint32 ret;
    guint8 enable_value = 0;
    OBJ_HANDLE obj_handle = 0;
    guint8 i;

    
    ret = dfl_get_object_handle(ETH_CLASS_NAME_MGNTPORTCAP, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "MngtPortCap failed, result is %d\n", ret);
        return ret;
    }

    for (i = 0; i < MAX_ACTPORT_DISPLAY_INDEX; i++) {
        if (net_type_index == g_active_port_mgnt[i].display_index) {
            (void)dal_get_property_value_byte(obj_handle, g_active_port_mgnt[i].proper_name, &enable_value);
            return (enable_value == 1) ? RET_OK : RET_ERR;
        }
    }
    g_printf("Input net type error.\r\n");
    return RET_ERR;
}

gint32 _netport_switch_get_net_type_and_port(guchar argc, gchar **argv, gint32 *net_type, gint32 *net_port)
{
#define SET_ACTIVE_PORT_PARAM_COUNT 3
    gint32 net_type_tmp;
    gint32 net_port_tmp;
    OBJ_HANDLE tmp_handle = OBJ_HANDLE_COMMON;

    if (argc != SET_ACTIVE_PORT_PARAM_COUNT || (g_strcmp0(argv[0], "-v") != 0) || (net_type == NULL) ||
        (net_port == NULL)) {
        return RET_ERR;
    }

    net_type_tmp = str_to_int(argv[1]);
    if (net_type_tmp == DEDICATE_PORT_DISPLAY_INDEX) {
        if (dal_get_specific_object_byte(ETH_CLASS_NAME_MGNTPORTCAP, PROPERTY_MNGTPORTCAP_PCIEMGNT_ENABLE, ENABLED,
            &tmp_handle) == RET_OK) {
            *net_type = NET_TYPE_DEDICATED;
        } else {
            return FALSE;
        }
    } else if (net_type_tmp == LOM_PORT_DISPLAY_INDEX) {
        *net_type = NET_TYPE_LOM;
    } else if (net_type_tmp == PCIE_PORT_DISPLAY_INDEX) {
        *net_type = NET_TYPE_PCIE;
    } else if (net_type_tmp == LOM2_PORT_DISPLAY_INDEX) {
        
        *net_type = NET_TYPE_LOM2;
        
    } else if (net_type_tmp == OCP_PORT_DISPLAY_INDEX) {
        *net_type = NET_TYPE_OCP_CARD;
    } else if (net_type_tmp == OCP2_PORT_DISPLAY_INDEX) {
        *net_type = NET_TYPE_OCP2_CARD;
    } else {
        return FALSE;
    }

    net_port_tmp = str_to_int(argv[2]);
    if (net_port_tmp == IPMC_ERROR) {
        return FALSE;
    }

    *net_port = net_port_tmp;

    return TRUE;
}


guchar mask_judge(guint32 mask)
{
    
    guint32 i;
    
    gint32 flag = 0;

    if (mask == 0) {
        return 1;
    }

    for (i = 0; i < 32; i++) {
        if ((mask >> i) & 0x01) {
            flag = 1;
        } else {
            if (flag) {
                return 1;
            }
        }
    }

    return 0;
}


gint32 set_ip_address(guint8 group_id, const gchar *ipaddr)
{
    gint32 iRet;
    gchar obj_name[OBJ_NAME_MAX_LEN] = {0};
    gint32 ret;
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    input_list = g_slist_append(input_list, g_variant_new_string(ipaddr));
    
    iRet = snprintf_s(obj_name, OBJ_NAME_MAX_LEN, OBJ_NAME_MAX_LEN - 1, "%s%u", ETH_CLASS_NAME_ETHGROUP, group_id);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, ETH_CLASS_NAME_ETHGROUP,
        obj_name, ETH_GROUP_METHOD_SET_IPV4_ADDR, input_list, NULL);
    if (ret != RET_OK) {
        
        if (ret == ETH_PARAM_IP_CONFLICT_ERROR) {
            g_printf("The IP address segment conflicts with another network.\r\n");
        }
        
        else if (ret == ETH_SET_ERROR) {
            g_printf("Cannot set IPv4 addresses in IPv6 mode.\r\n");
            
        } else {
            g_printf("Set IP address failed.\r\n");
        }

        uip_free_gvariant_list(input_list);
        return RET_ERR;
        
    }

    g_printf("Set IP address successfully.\r\n");
    uip_free_gvariant_list(input_list);
    return RET_OK;
}


gint32 set_ip_mask(guint8 group_id, const gchar *mask)
{
    gint32 iRet;
    gchar obj_name[OBJ_NAME_MAX_LEN] = {0};
    gint32 ret;
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    
    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }
    
    input_list = g_slist_append(input_list, g_variant_new_string(mask));
    iRet = snprintf_s(obj_name, OBJ_NAME_MAX_LEN, OBJ_NAME_MAX_LEN - 1, "%s%u", ETH_CLASS_NAME_ETHGROUP, group_id);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    

    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, ETH_CLASS_NAME_ETHGROUP,
        obj_name, ETH_GROUP_METHOD_SET_IPV4_MASK, input_list, NULL);
    if (ret != RET_OK) {
        g_printf("Set MASK address failed.\r\n");
        uip_free_gvariant_list(input_list);
        return RET_ERR;
    }

    g_printf("Set MASK address successfully.\r\n");
    uip_free_gvariant_list(input_list);
    return RET_OK;
}


gint32 set_ip_gateway(guint8 group_id, const gchar *gateway)
{
    gint32 iRet;
    gchar obj_name[OBJ_NAME_MAX_LEN] = {0};
    gint32 ret;
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    input_list = g_slist_append(input_list, g_variant_new_string(gateway));
    
    iRet = snprintf_s(obj_name, OBJ_NAME_MAX_LEN, OBJ_NAME_MAX_LEN - 1, "%s%u", ETH_CLASS_NAME_ETHGROUP, group_id);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, ETH_CLASS_NAME_ETHGROUP,
        obj_name, ETH_GROUP_METHOD_SET_IPV4_DEFAULT_GATEWAY, input_list, NULL);
    if (ret != RET_OK) {
        g_printf("Set GATEWAY failed.\r\n");
        uip_free_gvariant_list(input_list);
        return RET_ERR;
    }

    g_printf("Set GATEWAY successfully.\r\n");
    uip_free_gvariant_list(input_list);
    return RET_OK;
}


gint32 ipmc_ipv6_addr_valid_check(gchar *ipv6Str, const gchar *ipv6_addr)
{
    gint32 ret;
    gint32 i = 0;
    struct in6_addr buf;
    guchar ipv6_addr_digit[IPV6_IP_SIZE] = {0};
    guchar temp_ipv6_addr_digit[IPV6_IP_SIZE + 1] = {0};
    gboolean is_true;

    
    if (ipv6Str == NULL) {
        g_printf("ipv6_addr_valid_check:ipv6Str is null ptr!\n");
        return 1;
    }

    
    ret = inet_pton(AF_INET6, ipv6Str, (guchar *)&buf);
    if (ret != 1) {
        return 1;
    }

    // 判断是否为地址未指定 环回地址 本地链路地址  浮动IPv6广播地址
    if (IN6_IS_ADDR_UNSPECIFIED(&buf)) {
        g_printf("ipv6_addr_valid_check:this is unspecified!\n");
        return 1;
    }

    if (IN6_IS_ADDR_LOOPBACK(&buf)) {
        g_printf("ipv6_addr_valid_check:this is loopback address!\n");
        return 1;
    }

    if (IN6_IS_ADDR_LINKLOCAL(&buf)) {
        g_printf("ipv6_addr_valid_check:this is linklocal address!\n");
        return 1;
    }

    if (IN6_IS_ADDR_MULTICAST(&buf)) {
        g_printf("ipv6_addr_valid_check:this is multical address!\n");
        return 1;
    }

    
    is_true = (strlen(ipv6Str) == 0);
    is_true = is_true && (strlen(ipv6_addr) == 0);
    if (is_true == TRUE) {
        return 0;
    }

    if (strlen(ipv6_addr)) {
        (void)inet_pton(AF_INET6, ipv6_addr, ipv6_addr_digit);
        (void)inet_pton(AF_INET6, ipv6Str, temp_ipv6_addr_digit);

        for (i = 0; i < IPV6_IP_SIZE; i++) {
            if (ipv6_addr_digit[i] != temp_ipv6_addr_digit[i]) {
                return 0;
            }
        }

        if (i == IPV6_IP_SIZE) {
            g_printf("ip is same to gateway\n");
            return 1;
        }
    }

    return 0;
}


gint32 set_ipv6_address(guint8 group_id, const gchar *ipv6addr)
{
    gint32 iRet;
    gchar obj_name[OBJ_NAME_MAX_LEN] = {0};
    gint32 ret;
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    input_list = g_slist_append(input_list, g_variant_new_string(ipv6addr));
    
    iRet = snprintf_s(obj_name, OBJ_NAME_MAX_LEN, OBJ_NAME_MAX_LEN - 1, "%s%u", ETH_CLASS_NAME_ETHGROUP, group_id);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, ETH_CLASS_NAME_ETHGROUP,
        obj_name, ETH_GROUP_METHOD_SET_IPV6_ADDR, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        
        if (ret == ETH_SET_ERROR) {
            g_printf("Cannot set IPv6 addresses in IPv4 mode.\r\n");
            return FALSE;
        }

        
        g_printf("Set IPV6 address failed.\r\n");
        return FALSE;
    }

    g_printf("Set IPV6 address successfully.\r\n");
    return TRUE;
}


gint32 ipmc_eth_judge_ipv6_ip_gateway(gchar *ipv6_addr, gchar *ipv6_gateway, guchar prefix, guchar *is_same)
{
    struct in6_addr ipv6_addr_digit;
    struct in6_addr ipv6_gateway_digit;
    guchar part_ipv6_addr_digit = 0;
    guchar part_ipv6_gateway_digit = 0;
    guchar part_count = 0;

    
    gint32 retv = inet_pton(AF_INET6, ipv6_addr, ipv6_addr_digit.s6_addr);
    if (retv != 1) {
        debug_log(DLOG_ERROR, "%s:inet_pton faled:ipv6_addr is %s\n", __func__, ipv6_addr);
        return -1;
    }

    
    retv = inet_pton(AF_INET6, ipv6_gateway, ipv6_gateway_digit.s6_addr);
    if (retv != 1) {
        debug_log(DLOG_ERROR, "%s:inet_pton faled:ipv6_gateway is %s\n", __func__, ipv6_gateway);
        return -1;
    }

    
    gint32 i = 0;

    while (i < (prefix / 8)) {
        if (ipv6_addr_digit.s6_addr[i] != ipv6_gateway_digit.s6_addr[i]) {
            *is_same = 0;
            return RET_OK;
        }

        i++;
    }

    
    if (prefix % 8) {
        if (i < G_N_ELEMENTS(ipv6_gateway_digit.s6_addr)) {
            part_count = (8 - (prefix % 8)); // 剩余的数据需要右移part_count个位
            part_ipv6_addr_digit = (ipv6_addr_digit.s6_addr[i] >> part_count);
            part_ipv6_gateway_digit = (ipv6_gateway_digit.s6_addr[i] >> part_count);
            if (part_ipv6_addr_digit == part_ipv6_gateway_digit) {
                *is_same = 1;
                return RET_OK;
            }
        }
    } else {
        *is_same = 1;
        return RET_OK;
    }

    *is_same = 0;
    return RET_OK;
}


gint32 set_ipv6_prefix(guint8 group_id, guint8 prefixlen)
{
    gint32 iRet;
    gchar obj_name[OBJ_NAME_MAX_LEN] = {0};
    gint32 ret;
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    input_list = g_slist_append(input_list, g_variant_new_byte((guchar)prefixlen));
    
    iRet = snprintf_s(obj_name, OBJ_NAME_MAX_LEN, OBJ_NAME_MAX_LEN - 1, "%s%u", ETH_CLASS_NAME_ETHGROUP, group_id);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, ETH_CLASS_NAME_ETHGROUP,
        obj_name, ETH_GROUP_METHOD_SET_IPV6_PREFIX, input_list, NULL);
    if (ret != RET_OK) {
        g_printf("Set IPV6 prefix failed.\r\n");
        uip_free_gvariant_list(input_list);
        return FALSE;
    }

    g_printf("Set IPV6 prefix successfully.\r\n");
    uip_free_gvariant_list(input_list);
    return TRUE;
}


gint32 set_ipv6_gateway(guint8 group_id, const gchar *ipv6gateway)
{
    gint32 iRet;
    gchar obj_name[OBJ_NAME_MAX_LEN] = {0};
    gint32 ret;
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    input_list = g_slist_append(input_list, g_variant_new_string(ipv6gateway));
    
    iRet = snprintf_s(obj_name, OBJ_NAME_MAX_LEN, OBJ_NAME_MAX_LEN - 1, "%s%u", ETH_CLASS_NAME_ETHGROUP, group_id);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, ETH_CLASS_NAME_ETHGROUP,
        obj_name, ETH_GROUP_METHOD_SET_IPV6_DEFAULT_GATEWAY, input_list, NULL);
    if (ret != RET_OK) {
        g_printf("Set GATEWAY6 failed.\r\n");
        uip_free_gvariant_list(input_list);
        return FALSE;
    }

    g_printf("Set GATEWAY6 successfully.\r\n");
    uip_free_gvariant_list(input_list);

    return TRUE;
}


gint32 get_double_certification_enable_flag(guchar *pflag)
{
    OBJ_HANDLE obj_handle = 0;

    if (pflag == NULL) {
        return RET_ERR;
    }

    if (dal_get_object_handle_nth(CLASS_SECURITY_ENHANCE, 0, &obj_handle) != RET_OK) {
        return RET_ERR;
    }

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_SECURITY_ENHANCE_DOUBLE_CERTIFICATIONENABLE, pflag);

    return RET_OK;
}


gint32 ipmc_check_user_name(gchar *user_name)
{
    gint32 username_len;
    gint32 blank_count = 0;
    gint32 count;
    gint32 ret;

    if (user_name == NULL) {
        g_printf("User name null pointer.\r\n");
        return RET_ERR;
    }

    username_len = strlen(user_name);
    if (username_len >= IPMC_USER_NAME_MAX_LEN) {
        g_printf("The length of user name is too long.\r\n");
        g_printf("User name should less than 17 bytes.\r\n");
        return RET_ERR;
    }

    for (count = 0; count < username_len + 1; count++) {
        if (user_name[count] == 0x20) {
            blank_count++;
        }
    }

    if (blank_count == username_len) {
        g_printf("User name cannot be blank.\r\n");
        return RET_ERR;
    }

    
    if (user_name[0] == '\0' || user_name[0] == '#') {
        g_printf("User name cannot start with a number sign (#).\r\n");
        return RET_ERR;
    }

    
    if (user_name[0] == '-' || user_name[0] == '+') {
        g_printf("User name cannot start with character \"+\" or \"-\".\r\n");
        return RET_ERR;
    }
    

    
    if (!strcmp(user_name, ".") || !strcmp(user_name, "..")) {
        g_printf("User name cannot be just a dot (.) or two consecutive dots (..).\r\n");
        return RET_ERR;
    }

    ret = vos_check_incorrect_char(user_name, USER_USERNAME_LIMIT_CHARACTER, (gint32)strlen(user_name));
    if (ret != RET_OK) {
        g_printf("User name cannot include characters (:,\\<>&\"'/%%) and space.\r\n");
        return RET_ERR;
    }

    return RET_OK;
}


gint32 _auth_user(const gchar *user_name, const gchar *pw, const gchar *ip_addr, guint8 login_type)
{
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    if ((user_name == NULL) || (pw == NULL) || (ip_addr == NULL)) {
        return RET_ERR;
    }

    ret = dal_get_object_handle_nth(CLASS_USER, 0, &obj_handle);
    if (ret != RET_OK) {
        return ret;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(user_name));
    input_list = g_slist_append(input_list, g_variant_new_string(pw));
    input_list = g_slist_append(input_list, g_variant_new_byte(login_type));
    input_list = g_slist_append(input_list, g_variant_new_string(ip_addr));

    ret = uip_call_class_method_with_handle("CLI", user_name, ip_addr, CLASS_USER, obj_handle, METHOD_USER_AUTHUSER,
        input_list, &output_list);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != RET_OK) {
        return ret;
    }

    ret = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));

    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    return ret;
}


gint32 ipmc_get_user_password(gchar *user_password, size_t length)
{
    errno_t safe_fun_ret;
    gchar *pw = NULL;
    gint32 ret;

    if (user_password == NULL) {
        g_printf("Input null pointer.\r\n");
        return RET_ERR;
    }

    if (length < IPMC_USER_PASSWORD_LEN) {
        g_printf("Invalid length = %zd.\r\n", length);
        return RET_ERR;
    }

    
    if ((pw = getPasswd("Password:")) == NULL) {
        g_printf("Incorrect password.\r\n");
        return RET_ERR;
    }

    
    
    safe_fun_ret = strncpy_s(user_password, IPMC_USER_PASSWORD_LEN, pw, IPMC_USER_PASSWORD_LEN - 1);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    
    
    clear_sensitive_info(pw, IPMC_USER_PASSWORD_LEN);
    pw = NULL;

    ret = vos_check_incorrect_char((const gchar *)user_password, USER_PASSWORD_LIMIT_CHARACTER,
        (gint32)strlen(user_password));
    if (ret != RET_OK) {
        g_printf("Input password contains incorrect characters.\r\n");
        g_printf("Correct character contains any digits, letters, space or the following characters: "
            "`~!@#$%%^&*()-_=+\\|[{}];'\",<.>/?\n");
        clear_sensitive_info(user_password, IPMC_USER_PASSWORD_LEN);
        return RET_ERR;
    }

    if ((pw = getPasswd("Confirm password:")) == NULL) {
        g_printf("Confirm password error.\r\n");
        clear_sensitive_info(user_password, IPMC_USER_PASSWORD_LEN);
        return RET_ERR;
    }

    
    if (strcmp(user_password, pw)) {
        g_printf("Confirm password error.\r\n");
        clear_sensitive_info(pw, IPMC_USER_PASSWORD_LEN);
        clear_sensitive_info(user_password, IPMC_USER_PASSWORD_LEN);
        return RET_ERR;
    }

    
    
    if (strlen(user_password) == 0) {
        g_printf("The password cannot be empty.\r\n");
        return RET_ERR;
    }

    

    clear_sensitive_info(pw, IPMC_USER_PASSWORD_LEN);

    
    
    if (dal_str_mul_repeat_div(user_password, strlen(user_password)) != 1) {
        g_printf("WARNING: Using a password consisting of only repeated string may have many adverse effects, such as "
            "aa, ababab, and abcdabcd.\r\n");
        ret = check_the_input_operation(CLI_COMFIRM_STRING);
        if (ret == RET_ERR) {
            clear_sensitive_info(user_password, IPMC_USER_PASSWORD_LEN);
        }

        return ret;
    }

    

    return RET_OK;
}


gint32 ipmc_check_user_passwd(void)
{
    gchar *pw = NULL;
    uid_t uid = 0;
    gchar user_pwd[IPMC_USER_PASSWORD_LEN] = {0};
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    guint8 login_type;
    guchar flag = 1;

    (void)get_double_certification_enable_flag(&flag);

    
    if (flag == 0) {
        return RET_OK;
    }

    gint32 ret = ipmc_get_uid(&uid);
    if (ret != RET_OK) {
        g_printf("Get user info failed.\r\n");
        return RET_ERR;
    }

    if (uid == 0) {
        uid = 502;
    }

    if (uid <= USER_ID_BASE) {
        return RET_ERR;
    }

    if ((pw = getPasswd("Input your password:")) == NULL) {
        g_printf("Incorrect password.\r\n");
        return RET_ERR;
    }

    
    
    (void)strncpy_s(user_pwd, IPMC_USER_PASSWORD_LEN, pw, IPMC_USER_PASSWORD_LEN - 1);
    

    
    while (*pw) {
        *pw++ = 0;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        g_printf("Incorrect password.\r\n");
        
        clear_sensitive_info(user_pwd, sizeof(user_pwd));
        
        return RET_ERR;
    }

    
    if (uid >= LDAP_USER_ID_BASE) {
        
        login_type = 1;
    } else {
        
        login_type = 0;
    }

    if (_auth_user(username, user_pwd, ip, login_type) != RET_OK) {
        g_printf("Incorrect password or locked account.\r\n");
        
        clear_sensitive_info(user_pwd, sizeof(user_pwd));
        
        return RET_ERR;
    }
    
    clear_sensitive_info(user_pwd, sizeof(user_pwd));
    
    
    return RET_OK;
}


gint32 set_backup_ipaddr_mask(guint8 group_id, const gchar *ipaddr, const gchar *mask)
{
    gint32 iRet;
    gchar obj_name[OBJ_NAME_MAX_LEN] = {0};
    gint32 ret;
    GSList *input_list = NULL;
    gchar ip[PARAMETER_LEN + 1] = {0};
    gchar name[PARAMETER_LEN + 1] = {0};

    if (get_username_ip(name, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(name, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    input_list = g_slist_append(input_list, g_variant_new_string(ipaddr));
    input_list = g_slist_append(input_list, g_variant_new_string(mask));

    iRet = snprintf_s(obj_name, OBJ_NAME_MAX_LEN, OBJ_NAME_MAX_LEN - 1, "%s%u", ETH_CLASS_NAME_ETHGROUP, group_id);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }

    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)name, (const gchar *)ip, ETH_CLASS_NAME_ETHGROUP,
        obj_name, ETH_GROUP_METHOD_SET_BACKUP_IP_MAKS, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        g_printf("Set backup IP address failed.\r\n");
        return RET_ERR;
    }

    g_printf("Set backup IP address successfully.\r\n");
    g_printf("Set backup MASK address successfully.\r\n");
    return RET_OK;
}


gint32 check_sol_com_is_valid(guint8 com_id)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint8 temp_com_id = 0;

    ret = dal_get_object_handle_nth(CLASS_NAME_SOL_MANAGEMENT, 0, &obj_handle);
    if (ret != RET_OK) {
        return RET_ERR;
    }
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SOL_MANAGEMENT_COMID, &temp_com_id);
    if (ret != RET_OK) {
        return RET_ERR;
    }
    
    if (temp_com_id != 0xff && com_id != temp_com_id) {
        return RET_ERR;
    }
    return RET_OK;
}


void print_sol_deactivate_usage(void)
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
    gchar* type_string[] = { "HMM", "CLI", "IPMI" };
    gint32 mode = 0;
    gchar* mode_string[] = { "Shared", "Private" };

    g_printf("Usage: ipmcset -t sol -d deactivate -v <index>\r\n");
    g_printf("Index is:\r\n");
    g_printf("%-8s%-8s%-8s%-28s%-38s%s\r\n", "Index", "Type", "Mode", "LoginTime", "IP", "Name");

    ret = dfl_get_object_list(SOLLINK_CLASS_NAME, &obj_list);
    if (ret != DFL_OK || g_slist_last(obj_list) == 0) {
        return;
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
            return;
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
    return;
}


gint32 eth_is_continue_operate(const gchar *string1, const gchar *string2)
{
    gint32 input;

    if ((string1 == NULL) || (string2 == NULL)) {
        return RET_ERR;
    }

    g_printf("WARNING: This operation will %s %s.\r\n", string1, string2);
    input = check_the_input_operation(CLI_COMFIRM_STRING);
    return input;
}


gboolean check_ip_mask(const gchar *str_ip, const gchar *str_mask)
{
    gint32 ret;
    guint32 ip = 0;
    guint32 mask = 0;

    ret = inet_pton(AF_INET, str_ip, &ip);
    if (ret <= 0) {
        return FALSE;
    }

    ret = inet_pton(AF_INET, str_mask, &mask);
    if (ret <= 0) {
        return FALSE;
    }

    ip = htonl(ip);
    mask = htonl(mask);
    if (mask_judge(mask) != 0) {
        return FALSE;
    }

    if ((!(ip & (~mask))) || ((ip | mask) == 0xFFFFFFFF) || (mask >= (ip | mask)) || (!(ip & mask))) {
        return FALSE;
    }

    return TRUE;
}



gint32 set_ip_mask_gateway_address(guint8 group_id, const gchar *ipaddr, const gchar *mask, const gchar *gateway)
{
    gint32 iRet;

    gchar obj_name[OBJ_NAME_MAX_LEN] = {0};
    gint32 ret;
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    gint8 log_flag = 0;

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    input_list = g_slist_append(input_list, g_variant_new_string(ipaddr));
    input_list = g_slist_append(input_list, g_variant_new_string(mask));
    input_list = g_slist_append(input_list, g_variant_new_string(gateway));
    input_list = g_slist_append(input_list, g_variant_new_byte(log_flag));
    
    iRet = snprintf_s(obj_name, OBJ_NAME_MAX_LEN, OBJ_NAME_MAX_LEN - 1, "%s%u", ETH_CLASS_NAME_ETHGROUP, group_id);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, ETH_CLASS_NAME_ETHGROUP,
        obj_name, ETH_GROUP_METHOD_SET_IPV4_MASK_GATEWAY, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        g_printf("Set IP address failed.\r\n");
        g_printf("Set MASK address failed.\r\n");
        g_printf("Set GATEWAY failed.\r\n");
        return RET_ERR;
    }

    g_printf("Set IP address successfully.\r\n");
    g_printf("Set MASK address successfully.\r\n");
    g_printf("Set GATEWAY successfully.\r\n");

    return RET_OK;
}


gint32 delete_float_ip(void)
{
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    gint32 ret;
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    guint8 action = DISABLE_ETH;

    ret = dal_get_specific_object_byte(ETH_CLASS_NAME_ETH, ETH_ATTRIBUTE_OUT_TYPE, FLOATIP_ETHERNET, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s(%d):find floatip eth object failed.\r\n", __func__, __LINE__);
        return RET_ERR;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    if (!check_ip_gateway(0, 0, FLOATIP_ETHERNET)) {
        return RET_ERR;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)action));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, ETH_CLASS_NAME_ETH,
        dfl_get_object_name(obj_handle), ETH_METHOD_SET_ETH_ENABLE, input_list, NULL);

    uip_free_gvariant_list(input_list);
    return ret;
}


gint32 delete_float_ipv6(void)
{
    gchar ipv6_addr[INET6_ADDRSTRLEN] = {0};
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    gint32 ret;
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle = 0;

    ret = dal_get_specific_object_byte(ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_ATTRIBUTE_OUT_TYPE, FLOATIP_ETHERNET,
        &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s(%d):find floatip ethgroup failed.", __func__, __LINE__);
        return RET_ERR;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    input_list = g_slist_append(input_list, g_variant_new_string(ipv6_addr));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, ETH_CLASS_NAME_ETHGROUP,
        dfl_get_object_name(obj_handle), ETH_GROUP_METHOD_SET_IPV6_ADDR, input_list, NULL);
    uip_free_gvariant_list(input_list);
    return ret;
}


gboolean check_ip_gateway(guint32 ip, guint32 mask, guint8 out_type)
{
    gint32 ret;
    guint32 ip_other = 0;
    guint32 mask_other = 0;
    guint32 gw = 0;
    gchar str_ip[IPV4_IP_STR_SIZE + 1] = { 0 };
    gchar str_mask[IPV4_IP_STR_SIZE + 1] = { 0 };
    gchar str_gw[IPV4_IP_STR_SIZE + 1] = { 0 };
    OBJ_HANDLE obj_handle = 0;
    OBJ_HANDLE obj_handle_other = 0;
    OBJ_HANDLE static_obj_handle;
    gboolean is_true;
    gint32 input = 0;
    guint8 out_type_other = (out_type == FLOATIP_ETHERNET) ? OUT_ETHERNET : FLOATIP_ETHERNET;

    ret = dal_eth_get_out_type_object(out_type, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:find ethgroup failed.out_type:%d, ret:%d", __func__, out_type, ret);
        g_printf("Check subnet segment of ip and gateway failed.\r\n");
        return FALSE;
    }

    ret = dal_eth_get_out_type_object(out_type_other, &obj_handle_other);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:find ethgroup failed.out_type:%d, ret:%d", __func__, out_type_other, ret);
        g_printf("Check subnet segment of ip and gateway failed.\r\n");
        return FALSE;
    }

    static_obj_handle = (out_type == OUT_ETHERNET) ? obj_handle : obj_handle_other;

    (void)dal_get_property_value_string(obj_handle_other, ETH_GROUP_ATTRIBUTE_IP_ADDR, str_ip, sizeof(str_ip));
    (void)dal_get_property_value_string(obj_handle_other, ETH_GROUP_ATTRIBUTE_SUB_MASK, str_mask, sizeof(str_mask));
    (void)dal_get_property_value_string(static_obj_handle, ETH_GROUP_ATTRIBUTE_GATEWAY, str_gw, sizeof(str_gw));

    (void)inet_pton(AF_INET, str_ip, (void *)&ip_other);
    (void)inet_pton(AF_INET, str_mask, (void *)&mask_other);
    (void)inet_pton(AF_INET, str_gw, (void *)&gw);

    if (gw == 0) {
        return TRUE;
    }

    ip_other = htonl(ip_other);
    mask_other = htonl(mask_other);
    gw = htonl(gw);
    is_true = (ip == 0 && ((ip_other & mask_other) != (gw & mask_other)));
    is_true = is_true || (ip_other == 0 && ((ip & mask) != (gw & mask)));
    is_true = is_true || (((ip & mask) != (gw & mask)) && ((ip_other & mask_other) != (gw & mask_other)));
    if (is_true) {
        g_printf("WARNING: The default gateway will be deleted by this operation, for neither static nor float ip is "
            "on the same subnet segment as default gateway.\r\n");
        g_printf(CLI_COMFIRM_STRING "\r\n");
        input = ipmc_get_user_input("");
        if ((input == 'y') || (input == 'Y')) {
            return TRUE;
        } else {
            return FALSE;
        }
    }

    return TRUE;
}

void printf_error_info(gchar *location, gint32 ret)
{
    switch (ret) {
        case ERROR_INPUT_PARAMETER_BLADETYPE:
            g_printf("Wrong blade type.\r\n");
            break;
        case ERROR_INPUT_PARAMETER_FILENAME:
            g_printf("Wrong file name.\r\n");
            break;
        case ERROR_FILE_NOT_EXIST:
            g_printf("The file not exist\r\n");
            break;
        case ERROR_FILE_LENGTH:
            g_printf("The file is empty or the file is too large.\r\n");
            break;
        case ERROR_NOT_SUPPORT:
            g_printf("Current status is not supported.\r\n");
            break;
        case ERROR_HOT_SWAP:
            g_printf("Current hotswap state doesn't support.\r\n");
            break;
        case ERROR_COPY_FILE:
            g_printf("Copy file failed.\r\n");
            break;
        case OFFLINECFG_TIME_OUT:
            g_printf("Offlinecfg time out.\r\n");
            break;
        default:
            g_printf("Offlineconfig %s cfgfile failed.\r\n", location);
            break;
    }
}
