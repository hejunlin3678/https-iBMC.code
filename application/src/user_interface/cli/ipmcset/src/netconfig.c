

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>

#include "pme_app/pme_app.h"
#include "pme_app/uip/uip.h"
#include "pme_app/uip/uip_vnc.h"
#include "pme_app/uip/ipmc_public.h"
#include "pme_app/uip/ipmctypedef.h"
#include "pme_app/uip/uip_network_config.h"
#include "pme_app/smlib/sml_base.h"
#include "pme_app/smlib/sml_errcodes.h"
#include "pme/common/mscm_vos.h"
#include "pme/ipmi/common_ipmi.h"

#include "ipmcsetcommand.h"
#include "ipmcsethelp.h"
#include "ipmc_shelf_common.h"
#include "cli_sol.h"
#include "ipmcset_power.h"
#include "ipmcset_netconfig.h"
#include "ipmcset_alarm.h"

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

LOCAL const gchar* net_mode_str[] = {
    "Unknown",
    "Manual",
    "Adaptive",
};

LOCAL guint8 g_actport_without_port_map[][2] = {
    {DEDICATE_PORT_DISPLAY_INDEX, NET_TYPE_DEDICATED},
    {AGGREGATION_PORT_DISPLAY_INDEX, NET_TYPE_AGGREGATION},
    {UNKNOWN_CARD_TYPE_DISPLAY_INDEX, UNKOWN_CARD_TYPE}
};

LOCAL guint8 g_actport_with_port_map[][2] = {
    { DEDICATE_PORT_DISPLAY_INDEX, NET_TYPE_DEDICATED }, 
    {LOM_PORT_DISPLAY_INDEX, NET_TYPE_LOM},
    {PCIE_PORT_DISPLAY_INDEX, NET_TYPE_PCIE},
    {LOM2_PORT_DISPLAY_INDEX, NET_TYPE_LOM2},
    {OCP_PORT_DISPLAY_INDEX, NET_TYPE_OCP_CARD},
    {OCP2_PORT_DISPLAY_INDEX, NET_TYPE_OCP2_CARD},
    {UNKNOWN_CARD_TYPE_DISPLAY_INDEX, UNKOWN_CARD_TYPE}
};


LOCAL gint32 _ipmc_judge_net_type_without_portmap(gint32 *net_type, gint32 net_type_index)
{
    gint32 temp_val = 0;
    guint8 i;

    for (i = 0; i < sizeof(g_actport_without_port_map) / sizeof(g_actport_without_port_map[0]); i++) {
        if (net_type_index == g_actport_without_port_map[i][0]) {
            temp_val = g_actport_without_port_map[i][1];
            break;
        }
    }

    if (i == sizeof(g_actport_without_port_map) / sizeof(g_actport_without_port_map[0])) {
        print_activeport_helpinfo();
        return RET_ERR;
    }

    *net_type = temp_val;
    return RET_OK;
}


LOCAL gint32 _ipmc_judge_net_type_with_portmap(gint32 *net_type, gint32 net_type_index)
{
    gint32 temp_val = 0;
    guint8 i;

    for (i = 0; i < sizeof(g_actport_with_port_map) / sizeof(g_actport_with_port_map[0]); i++) {
        if (net_type_index == g_actport_with_port_map[i][0]) {
            temp_val = g_actport_with_port_map[i][1];
            break;
        }
    }

    if (i == sizeof(g_actport_with_port_map) / sizeof(g_actport_with_port_map[0])) {
        print_activeport_helpinfo();
        return RET_ERR;
    }

    *net_type = temp_val;
    return RET_OK;
}


LOCAL gint32 _ipmc_set_activeport_proc(gint32 net_type, gint32 port_num)
{
    gint32 ret;
    gchar obj_name[OBJ_NAME_MAX_LEN] = {0};
    GSList* input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    guint8 group_id = dal_eth_get_out_type_groupid();

    if (port_num >= 0xFF) {
        print_activeport_helpinfo();
        return FALSE;
    }

    
    ret = ipmc_judge_netport_exist((guint8)net_type, (guint8)port_num);
    if (ret != RET_OK) {
        g_printf("Input port does not exist.\n");
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        ret = snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s username fail, ret = %d", __FUNCTION__, ret);
        }
        ret = snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s ip fail, ret = %d", __FUNCTION__, ret);
        }
        
    }

    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)net_type));
    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)port_num));
    
    ret = snprintf_s(obj_name, OBJ_NAME_MAX_LEN, OBJ_NAME_MAX_LEN - 1, "%s%u", ETH_CLASS_NAME_ETHGROUP, group_id);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
    

    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar*)username, (const gchar*)ip, ETH_CLASS_NAME_ETHGROUP,
                                obj_name, ETH_GROUP_METHOD_SET_ACTIVE_PORT, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        
        if (ret == ETH_UNSUPPORT) {
            g_printf("The current netmode is not support to active port.\n");
            return FALSE;
        }
        
        if (ret == ETH_AVAFLAG_DISABLED) {
            g_printf("The current port is disabled.\n");
            return FALSE;
        }
        g_printf("Set active port failed.\r\n");
        return FALSE;
    }

    g_printf("Set active port successfully.\r\n");
    return TRUE;
}


LOCAL gboolean __dedicated_port_vlan_config_supported(void)
{
    gint32 ret;
    OBJ_HANDLE mgnt_port_cap = 0;
    guint8 mgnt_vlan_supported = 0;

    ret = dfl_get_object_handle(OBJ_NAME_MGNTPORTCAP, &mgnt_port_cap);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s Get object(%s) failed[%d].", __FUNCTION__, OBJ_NAME_MGNTPORTCAP, ret);
        return FALSE;
    }

    ret = dal_get_property_value_byte(mgnt_port_cap, PROPERTY_MNGTPORTCAP_DEDICATED_VLAN_ENABLE, &mgnt_vlan_supported);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s %s get %s failed[%d].",
            __FUNCTION__, OBJ_NAME_MGNTPORTCAP, PROPERTY_MNGTPORTCAP_DEDICATED_VLAN_ENABLE, ret);
        return FALSE;
    }

    return mgnt_vlan_supported == 1;
}


LOCAL void print_config_vlan_helpinfo(guint16 vlan_min, guint16 vlan_max, gboolean dedicated_supported)
{
    if (dal_is_dedicated_pcie_mgnt_port_existed()) {
        g_printf("Usage: ipmcset -d vlan -v <off | id(%d~%d)>\r\n", vlan_min, vlan_max);
    } else {
        g_printf("Usage: ipmcset -d vlan -v <off | id(%d~%d)> [port type]\r\n", vlan_min, vlan_max);
        g_printf("Port types are:\r\n");
        g_printf("    0      :  NCSI shared port\r\n");
        if (dedicated_supported) {
            g_printf("    1      :  Dedicated port\r\n");
        }
    }
}


LOCAL gint32 __get_vlan_range(const gchar *obj_name, guint16 *vlan_min, guint16 *vlan_max)
{
    gint32 ret;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;

    property_name_list = g_slist_append(property_name_list, ETH_GROUP_ATTRIBUTE_VLAN_MIN);
    property_name_list = g_slist_append(property_name_list, ETH_GROUP_ATTRIBUTE_VLAN_MAX);
    ret = uip_multiget_object_property(ETH_CLASS_NAME_ETHGROUP, obj_name, property_name_list, &property_value);
    g_slist_free(property_name_list);
    if (ret != RET_OK) {
        g_printf("Get vlan id critical value failed.\n");
        return RET_ERR;
    }

    *vlan_min = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value, 0));
    *vlan_max = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value, 1));
    uip_free_gvariant_list(property_value);

    return RET_OK;
}


LOCAL gint32 __set_vlan_id(const gchar *obj_name, guint32 vlan_id, guint32 vlan_state, guint32 vlan_port)
{
    gint32 ret;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    GSList *input_list = NULL;

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    input_list = g_slist_append(input_list, g_variant_new_uint16(vlan_id));
    input_list = g_slist_append(input_list, g_variant_new_uint32((guint32)vlan_state));
    input_list = g_slist_append(input_list, g_variant_new_uint32(vlan_port));

    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, ETH_CLASS_NAME_ETHGROUP,
        obj_name, ETH_GROUP_METHOD_SET_VLAN_ID, input_list, NULL);
    uip_free_gvariant_list(input_list);

    return ret;
}


LOCAL gint32 __get_vlan_id_param(gchar **argv, guint32 *vlan_id, guint32 *vlan_state)
{
    if (!strcmp(argv[1], "off")) {
        *vlan_state = DISABLE_ETH;
        *vlan_id = 0;
    } else {
        if ((*vlan_id = str_to_int(argv[1])) == IPMC_ERROR) {
            return RET_ERR;
        }

        *vlan_state = ENABLE_ETH;
    }

    return RET_OK;
}


LOCAL gint32 __get_vlan_port_param(guchar argc, gchar **argv, guint32 *vlan_port, gboolean dedicated_supported)
{
    if (argc == 3) {
        if (dal_is_dedicated_pcie_mgnt_port_existed()) {    // 机柜不支持port参数
            return RET_ERR;
        }
        if ((*vlan_port = str_to_int(argv[2])) == IPMC_ERROR)  { 
            return RET_ERR;
        }
        if ((*vlan_port >= VLAN_PORT_TYPE_NUM) ||
            (!dedicated_supported && *vlan_port == VLAN_PORT_CONFIG_DEDICATED)) {
            return RET_ERR;
        }
    } else {
        *vlan_port = VLAN_PORT_CONFIG_NCSI;
    }

    return RET_OK;
}

gint32 ipmc_set_activeport(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    
    if ((argc < 2) || (argc > 3) || strcmp(argv[0], "-v")) {
        goto error_exit;
    }
    gint32 port_num = 0;
    gint32 net_type = 0;
    OBJ_HANDLE pcie_mgnt_obj_handle = OBJ_HANDLE_COMMON;
    gint32 net_type_index = str_to_int(argv[1]);
    gboolean ar_card_support = dal_check_ar_card_support();

    (void)dal_get_specific_object_byte(ETH_CLASS_NAME_MGNTPORTCAP, PROPERTY_MNGTPORTCAP_PCIEMGNT_ENABLE, ENABLE,
        &pcie_mgnt_obj_handle);

    
    if ((!ar_card_support) && (ipmc_judge_mntportcab_valiad(net_type_index) != RET_OK)) {
        goto error_exit;
    }

    
    if (argc == 2) {
        if ((ar_card_support) || (_ipmc_judge_net_type_without_portmap(&net_type, net_type_index) != RET_OK)) {
            goto error_exit;
        }
        
        if ((net_type_index == DEDICATE_PORT_DISPLAY_INDEX) && (pcie_mgnt_obj_handle != OBJ_HANDLE_COMMON)) {
            goto error_exit;
        }
        
    } else if (argc == 3) {
        if (_ipmc_judge_net_type_with_portmap(&net_type, net_type_index) != RET_OK) {
            return FALSE;
        }

        if (!ar_card_support) {
            if ((_netport_switch_get_net_type_and_port(argc, argv, &net_type, &port_num) == FALSE) ||
                ((pcie_mgnt_obj_handle == OBJ_HANDLE_COMMON) && (net_type_index == DEDICATE_PORT_DISPLAY_INDEX))) {
                    goto error_exit;
            }

            if ((net_type == NET_TYPE_DEDICATED) && (port_num == 0)) {
                goto error_exit;
            }
        } else {
            
            port_num = str_to_int(argv[2]);
            if (net_type != NET_TYPE_DEDICATED) {
                g_printf("Input port is not dedicated port.\n");
                return FALSE;
            }
        }
    }
    return _ipmc_set_activeport_proc(net_type, port_num);

error_exit:
    print_activeport_helpinfo();
    return FALSE;
}


gint32 ipmc_set_vlan_state(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gchar obj_name[OBJ_NAME_MAX_LEN] = {0};
    guint8 group_id = dal_eth_get_out_type_groupid();
    guint32 vlan_port = VLAN_PORT_CONFIG_NCSI;
    guint32 vlan_state = 0;
    guint32 vlan_id = 0;
    guint16 vlan_max = 0;
    guint16 vlan_min = 0;
    gboolean dedicated_supported = __dedicated_port_vlan_config_supported();

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    
    gint32 ret = snprintf_s(obj_name, OBJ_NAME_MAX_LEN, OBJ_NAME_MAX_LEN - 1,
        "%s%u", ETH_CLASS_NAME_ETHGROUP, group_id);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        return FALSE;
    }
    

    if (__get_vlan_range(obj_name, &vlan_min, &vlan_max) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get vlan range failed", __FUNCTION__);
        return FALSE;
    }

    if ((argc != 2 && argc != 3) || (strcmp(argv[0], "-v"))) { 
        print_config_vlan_helpinfo(vlan_min, vlan_max, dedicated_supported);
        return FALSE;
    }

    ret = __get_vlan_id_param(argv, &vlan_id, &vlan_state);
    if (ret != RET_OK || ((vlan_state == ENABLE_ETH) && ((vlan_id < vlan_min) || (vlan_id > vlan_max)))) {
        print_config_vlan_helpinfo(vlan_min, vlan_max, dedicated_supported);
        return FALSE;
    }

    if (__get_vlan_port_param(argc, argv, &vlan_port, dedicated_supported) != RET_OK) {
        print_config_vlan_helpinfo(vlan_min, vlan_max, dedicated_supported);
        return FALSE;
    }

    if (__set_vlan_id(obj_name, vlan_id, vlan_state, vlan_port) != RET_OK) {
        if (dal_is_dedicated_pcie_mgnt_port_existed()) {
            g_printf("Set vlan state failed.\r\n");
        } else {
            g_printf("Set %s vlan failed.\r\n", dal_get_vlan_name(vlan_port));
        }
        return FALSE;
    }

    if (dal_is_dedicated_pcie_mgnt_port_existed()) {
        g_printf("Set vlan state successfully.\r\n");
    } else {
        g_printf("Set %s vlan successfully.\r\n", dal_get_vlan_name(vlan_port));
    }

    return TRUE;
}


gint32 ipmc_set_netmode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 iRet;
    gchar obj_name[OBJ_NAME_MAX_LEN] = {0};
    gint32 ret;
    GSList *input_list = NULL;
    guint8 group_id = dal_eth_get_out_type_groupid();
    gint32 net_mode = 0;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        
        print_netmode_usage(FALSE);
        
        return FALSE;
    }

    if ((net_mode = str_to_int(argv[1])) == IPMC_ERROR) {
        
        print_netmode_usage(FALSE);
        
        return FALSE;
    }

    if (net_mode != 1 && net_mode != 2) {
        
        print_netmode_usage(FALSE);
        
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }
    
    iRet = snprintf_s(obj_name, OBJ_NAME_MAX_LEN, OBJ_NAME_MAX_LEN - 1, "%s%u", ETH_CLASS_NAME_ETHGROUP, group_id);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    
    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)net_mode));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, ETH_CLASS_NAME_ETHGROUP,
        obj_name, ETH_GROUP_METHOD_SET_NET_MODE, input_list, NULL);
    if (ret != RET_OK) {
        g_printf("Set net mode %s failed.\r\n", net_mode_str[net_mode]);
        uip_free_gvariant_list(input_list);
        return FALSE;
    }

    g_printf("Set net mode %s successfully.\r\n", net_mode_str[net_mode]);
    uip_free_gvariant_list(input_list);
    return TRUE;
}


gint32 ipmc_set_maintenance_ipaddr(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    OBJ_HANDLE group_obj = 0;
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = { 0 };
    gchar ip[PARAMETER_LEN + 1] = { 0 };

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if ((argc != 3) || (strcmp(argv[0], "-v") != 0)) {
        g_printf("Usage: ipmcset -t maintenance -d ipaddr -v <ipaddr> <mask>\r\n");
        return FALSE;
    }

    if (check_ip_mask(argv[1], argv[2]) == FALSE) {
        g_printf("Input parameter format error.\r\n");
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    
    ret = dal_get_specific_object_byte(ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_ATTRIBUTE_OUT_TYPE, MAINT_DEDICATED_ETHERNET,
        &group_obj);
    if (ret != RET_OK) {
        g_printf("Set the maintenance IP address of dedicated port failed.\r\n");
        return FALSE;
    }

    if (dal_check_ipv4_addr_duplicate(group_obj, argv[1], argv[2]) != RET_OK) {
        g_printf("The IP address segment conflicts with another network.\r\n");
        return FALSE;
    }

    
    input_list = g_slist_append(input_list, g_variant_new_string(argv[1]));
    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
        ETH_CLASS_NAME_ETHGROUP, group_obj, ETH_GROUP_METHOD_SET_IPV4_ADDR, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        if (ret == ETH_PARAM_IP_CONFLICT_ERROR) {
            g_printf("The IP address segment conflicts with another network.\r\n");
        } else {
            g_printf("Set the maintenance IP address of dedicated port failed.\r\n");
        }
        return FALSE;
    }
    g_printf("Set the maintenance IP address of dedicated port successfully.\r\n");

    
    input_list = NULL;
    input_list = g_slist_append(input_list, g_variant_new_string(argv[2]));
    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
        ETH_CLASS_NAME_ETHGROUP, group_obj, ETH_GROUP_METHOD_SET_IPV4_MASK, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        g_printf("Set the maintenance subnet mask of dedicated port failed.\r\n");
        return FALSE;
    }
    g_printf("Set the maintenance subnet mask of dedicated port successfully.\r\n");

    return TRUE;
}


gint32 ipmc_set_ipmode_v6(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 iRet;
    gint32 ret;
    guint8 ipmode = 0;
    guint8 group_id;
    gchar obj_name[OBJ_NAME_MAX_LEN] = {0};
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    GSList *input_list = NULL;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 2 || strcmp(argv[0], "-v")) { 
        g_printf("Usage: ipmcset %s-d ipmode6 -v <dhcp|static>\r\n", help_default_name);
        return FALSE;
    }

    if (!strcmp(argv[1], "dhcp")) {
        ipmode = 2;
    } else if (!strcmp(argv[1], "static")) {
        ipmode = 1;
    } else {
        g_printf("Usage: ipmcset %s-d ipmode6 -v <dhcp|static>\r\n", help_default_name);
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    
    group_id = dal_eth_get_out_type_groupid();

    input_list = g_slist_append(input_list, g_variant_new_byte(ipmode));
    
    iRet = snprintf_s(obj_name, OBJ_NAME_MAX_LEN, OBJ_NAME_MAX_LEN - 1, "%s%u", ETH_CLASS_NAME_ETHGROUP, group_id);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, ETH_CLASS_NAME_ETHGROUP,
        obj_name, ETH_GROUP_METHOD_SET_IPV6_MODE, input_list, NULL);
    if (ret != RET_OK) {
        g_printf("Set IP mode failed.\r\n");
        uip_free_gvariant_list(input_list);
        return FALSE;
    }

    g_printf("Set %s mode successfully.\r\n", (ipmode == 1) ? "static" : "dhcp");

    uip_free_gvariant_list(input_list);
    return TRUE;
}


gint32 ipmc_set_ipmode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 iRet = -1;
    gint32 ret;
    guint8 ipmode = 0;
    guint8 group_id = 0;
    gchar obj_name[OBJ_NAME_MAX_LEN] = {0};
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    GSList *input_list = NULL;
    OBJ_HANDLE obj = 0;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 2 || strcmp(argv[0], "-v")) {
        g_printf("Usage: ipmcset %s-d ipmode -v <dhcp|static>\r\n", help_default_name);
        return FALSE;
    }

    if (!strcmp(argv[1], "dhcp")) {
        ipmode = 2;
    } else if (!strcmp(argv[1], "static")) {
        ipmode = 1;
    } else {
        g_printf("Usage: ipmcset %s-d ipmode -v <dhcp|static>\r\n", help_default_name);
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    ret = dal_eth_get_out_type_object(OUT_ETHERNET, &obj);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    if (ret == RET_OK) {
        (void)dal_get_property_value_byte(obj, ETH_GROUP_ATTRIBUTE_GROUP_ID, &group_id);
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(ipmode));
    
    iRet = snprintf_s(obj_name, OBJ_NAME_MAX_LEN, OBJ_NAME_MAX_LEN - 1, "%s%u", ETH_CLASS_NAME_ETHGROUP, group_id);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, ETH_CLASS_NAME_ETHGROUP,
        obj_name, ETH_GROUP_METHOD_SET_IPV4_MODE, input_list, NULL);
    if (ret != RET_OK) {
        g_printf("Set IP mode failed.\r\n");
        uip_free_gvariant_list(input_list);
        return FALSE;
    }

    g_printf("Set %s mode successfully.\r\n", (ipmode == 1) ? "static" : "dhcp");
    uip_free_gvariant_list(input_list);
    return TRUE;
}

LOCAL gint32 ipmc_set_ipaddr_check_input(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv)
{
    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        ipmc_log_operation_log(ETH_CLASS_NAME_ETHGROUP, "Set ipaddr failed.");
        return FALSE;
    }
    if (((argc != 3) && (argc != 4)) || strcmp(argv[0], "-v")) {
        g_printf("Usage: ipmcset %s-d ipaddr -v <ipaddr> <mask> [gateway]\r\n", help_default_name);
        return FALSE;
    }

    if (check_ip_mask(argv[1], argv[2]) == FALSE) {
        g_printf("Input parameter format error.\r\n");
        ipmc_log_operation_log(ETH_CLASS_NAME_ETHGROUP, "Set ipaddr failed.");
        return FALSE;
    }
    return TRUE;
}

gint32 ipmc_set_ipaddr(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    struct in_addr ipaddr;
    guint32 ip = 0;
    guint32 mask = 0;
    guint32 gateway = 0;
    guint8 group_id;
    gboolean is_true = 0;
    OBJ_HANDLE group_obj = 0;

    guchar MgmtSoftWareType = 0;
    ret = ipmc_set_ipaddr_check_input(privilege, rolepriv, argc, argv);
    if (ret != TRUE) {
        return ret;
    }
    (void)inet_pton(AF_INET, argv[1], (void *)&ip);
    (void)inet_pton(AF_INET, argv[2], (void *)&mask);

    ip = htonl(ip);
    mask = htonl(mask);

    
    group_id = dal_eth_get_out_type_groupid();

    (void)dal_get_software_type(&MgmtSoftWareType);
    if (argc == 3) { // 设置IP命令带3个参数
        if (MgmtSoftWareType == MGMT_SOFTWARE_TYPE_EM) {
            
            if ((!check_ip_gateway(ip, mask, OUT_ETHERNET))) {
                return FALSE;
            }
        } else {
            (void)dal_get_specific_object_byte(ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_ATTRIBUTE_GROUP_ID, group_id,
                &group_obj);
            if (dal_check_ipv4_addr_duplicate(group_obj, argv[1], argv[2]) != RET_OK) {
                g_printf("The IP address segment conflicts with another network.\r\n");
                return FALSE;
            }
        }
    } else if (argc == 4) { 
        ret = inet_pton(AF_INET, (const gchar *)argv[3], (void *)&ipaddr.s_addr);
        if (ret <= 0) {
            g_printf("Input parameter format error.\r\n");
            return FALSE;
        }

        (void)inet_pton(AF_INET, argv[3], (void *)&gateway);
        gateway = htonl(gateway);
        // Check Ip,Mask,Gateway
        is_true = ((ip & mask) != (gateway & mask));
        is_true = is_true || ((gateway & (~mask)) == 0);
        is_true = is_true || ((gateway | mask) == 0xFFFFFFFF);
        is_true = is_true && (gateway != 0);
        if (is_true) {
            g_printf("Setting failed. Please check the IP address, subnet mask, and gateway.\n");
            return FALSE;
        }

        
        ret = set_ip_mask_gateway_address(group_id, argv[1], argv[2], argv[3]);
        if (ret != RET_OK) {
            return FALSE;
        }

        return TRUE;

        
    }

    

    
    ret = set_ip_address(group_id, argv[1]);
    if (ret != RET_OK) {
        return FALSE;
    }

    
    ret = set_ip_mask(group_id, argv[2]);
    if (ret != RET_OK) {
        return FALSE;
    }

    return TRUE;
}


gint32 ipmc_set_gateway_v6(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    struct in6_addr v6ipaddr;
    guint8 group_id = 0;
    OBJ_HANDLE obj = 0;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        g_printf("Usage: ipmcset %s-d gateway6 -v <gateway6>\r\n", help_default_name);
        return FALSE;
    }

    memset_s(v6ipaddr.s6_addr, sizeof(struct in6_addr), 0x00, sizeof(struct in6_addr));
    ret = inet_pton(AF_INET6, argv[1], (void *)v6ipaddr.s6_addr);
    if (ret <= 0) {
        g_printf("Input parameter format invalid.\r\n");
        return FALSE;
    }

    ret = dal_eth_get_out_type_object(OUT_ETHERNET, &obj);
    if (ret == RET_OK) {
        (void)dal_get_property_value_byte(obj, ETH_GROUP_ATTRIBUTE_GROUP_ID, &group_id);
    }

    
    ret = set_ipv6_gateway(group_id, argv[1]);
    if (ret != TRUE) {
        return FALSE;
    }

    

    return TRUE;
}


gint32 ipmc_set_gateway(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    guint8 group_id = 0;
    struct in_addr ipaddr;
    OBJ_HANDLE obj = 0;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        g_printf("Usage: ipmcset %s-d gateway -v <gateway>\r\n", help_default_name);
        return FALSE;
    }

    ret = inet_pton(AF_INET, (const gchar *)argv[1], (void *)&ipaddr.s_addr);
    if (ret <= 0) {
        g_printf("Input parameter format error.\r\n");
        return FALSE;
    }

    ret = dal_eth_get_out_type_object(OUT_ETHERNET, &obj);
    if (ret == RET_OK) {
        (void)dal_get_property_value_byte(obj, ETH_GROUP_ATTRIBUTE_GROUP_ID, &group_id);
    }

    
    ret = set_ip_gateway(group_id, argv[1]);
    if (ret != RET_OK) {
        return FALSE;
    }

    

    return TRUE;
}

LOCAL gint32 get_all_ethnum(guint8 *ethnum_digit, guint8 digit_len, guint8 *ethnum_count)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    OBJ_HANDLE obj_handle = 0;
    guint8 ethnum_tmp = INVALID_DATA_BYTE;

    ret = dfl_get_object_list(ETH_CLASS_NAME_ETH, &obj_list);
    if ((ret != DFL_OK) || (g_slist_length(obj_list) == 0)) {
        g_printf("Cannot found ethernet interface.\r\n");
        ipmc_log_operation_log("Maintenance", "Set ethernet interface status failed.");
        g_slist_free(obj_list);
        return RET_ERR;
    }

    
    *ethnum_count = 0;
    for (obj_node = obj_list; obj_node != NULL; obj_node = obj_node->next) {
        obj_handle = (OBJ_HANDLE)obj_node->data;
        (void)dal_get_property_value_byte(obj_handle, ETH_ATTRIBUTE_NUM, &ethnum_tmp);
        if (*ethnum_count >= digit_len) {
            break;
        }
        ethnum_digit[*ethnum_count] = ethnum_tmp;
        (*ethnum_count)++;
    }
    g_slist_free(obj_list);
    return RET_OK;
}

LOCAL gint32 concat_all_ethnum_to_str(guint8 *ethnum_digit, guint8 ethnum_count, gchar *ethnum_str, guint32 str_size)
{
    gint32 ret;
    guint32 i;

    // 增加虚拟网卡之后，ethX的数字可能是三位数
    for (i = 0; i < ethnum_count; i++) {
        if (strlen(ethnum_str) < str_size) {
            ret = snprintf_s(ethnum_str + strlen(ethnum_str), str_size - strlen(ethnum_str),
                str_size - strlen(ethnum_str) - 1, "%u|", ethnum_digit[i]);
            if (ret <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s failed, ret = %d", __FUNCTION__, ret);
            }
        }
    }

    
    size_t str_len = strlen(ethnum_str);
    if (str_len == 0) {  
        return RET_ERR;
    }

    if (ethnum_str[str_len - 1] == '|') {
        ethnum_str[str_len - 1] = '\0';
    }

    return RET_OK;
}


gint32 ipmc_set_ethlink(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    gint32 port_id = INVALID_DATA_BYTE;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        ipmc_log_operation_log("Maintenance", "Insufficient privilege");
        return FALSE;
    }

    
    if ((argc > 1) && (strlen(argv[1]) > ETH_NAME_MIN_LEN)) {
        port_id = str_to_int(&argv[1][ETH_NAME_MIN_LEN]);
        
        if (port_id < 0 || port_id >= INVALID_DATA_BYTE) {
            g_printf("Invalid ethernet interface.\r\n");
            ipmc_log_operation_log("Maintenance", "Set ethernet interface status failed.");
            return FALSE;
        }
    }

    
    guint8 ethnum_digit[ETH_MAX_NUM] = {INVALID_DATA_BYTE}; 
    guint8 ethnum_count = 0;
    if (get_all_ethnum(ethnum_digit, sizeof(ethnum_digit), &ethnum_count) != RET_OK) {
        return FALSE;
    }

    gchar ethnum_str[ETHNUM_STR_MAX_LENGTH] = {0};
    if (concat_all_ethnum_to_str(ethnum_digit, ethnum_count, ethnum_str, sizeof(ethnum_str)) != RET_OK) {
        return FALSE;
    }

    if ((argc != 3) || (strcmp(argv[0], "-v")) || (strncmp(argv[1], "eth", ETH_NAME_MIN_LEN))) {
        g_printf("Usage: ipmcset -t maintenance -d ethlink -v eth[%s] [%s|%s]\r\n", ethnum_str, ETH_ENABLE,
            ETH_DISABLE);
        return FALSE;
    }

    guint8 action;
    if (strncmp(argv[2], ETH_ENABLE, strlen(ETH_ENABLE)) == 0) {
        action = ENABLE_ETH;
    } else if (strncmp(argv[2], ETH_DISABLE, strlen(ETH_DISABLE)) == 0) {
        action = DISABLE_ETH;
    } else {
        g_printf("Usage: ipmcset -t maintenance -d ethlink -v eth[%s] [%s|%s]\r\n", ethnum_str, ETH_ENABLE,
            ETH_DISABLE);
        return FALSE;
    }

    
    OBJ_HANDLE obj_handle = 0;
    ret = dal_get_specific_object_byte(ETH_CLASS_NAME_ETH, ETH_ATTRIBUTE_NUM, (guint8)port_id, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s dal_get_specific_object_byte failed, ret=%d", __FUNCTION__, ret);
        g_printf("Usage: ipmcset -t maintenance -d ethlink -v eth[%s] [%s|%s]\r\n", ethnum_str, ETH_ENABLE,
            ETH_DISABLE);
        return FALSE;
    }

    gchar obj_name[OBJ_NAME_MAX_LEN + 1] = {0};
    errno_t sec_ret = strcpy_s(obj_name, OBJ_NAME_MAX_LEN + 1, dfl_get_object_name(obj_handle));
    if (sec_ret != EOK || strlen(obj_name) == 0) {
        debug_log(DLOG_ERROR, "%s: strcpy_s failed, ret = %d", __FUNCTION__, sec_ret);
        return FALSE;
    }

    if (eth_is_continue_operate(argv[2], argv[1]) != RET_OK) {
        return TRUE;
    }

    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    GSList *input_list = NULL;
    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)action));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, ETH_CLASS_NAME_ETH,
        obj_name, ETH_METHOD_SET_ETH_ENABLE, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        g_printf("%s %s failed.\r\n", argv[2], argv[1]);
        ipmc_log_operation_log("Maintenance", "Set ethernet interface status failed.");
        return FALSE;
    }

    g_printf("%s %s successfully.\r\n", argv[2], argv[1]);

    return TRUE;
}


gint32 ipmc_set_cli_sol_timeout(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    GSList *input_list = NULL;
    gint32 timeout = -1;
    OBJ_HANDLE obj_handle = 0;
    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 2 || strcmp(argv[0], "-v") != 0) {
        print_sol_timeout_usage();
        return FALSE;
    }
    if ((timeout = str_to_int(argv[1])) == IPMC_ERROR) {
        print_sol_timeout_usage();
        return FALSE;
    }
    if ((timeout < 0) || (timeout > KVM_TIMEOUT_MAX_VALUE)) {
        print_sol_timeout_usage();
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    ret = dal_get_object_handle_nth(CLASS_NAME_SOL_MANAGEMENT, 0, &obj_handle);
    if (ret != RET_OK) {
        return ret;
    }
    input_list = g_slist_append(input_list, g_variant_new_int32(timeout));

    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
        CLASS_NAME_SOL_MANAGEMENT, obj_handle, METHOD_SET_SOL_TIMEOUT, input_list, NULL);
    uip_free_gvariant_list(input_list);

    if (ret != RET_OK) {
        g_printf("Set SOL timeout period failed.\r\n");
        return FALSE;
    }

    g_printf("Set SOL timeout period successfully.\r\n");
    return TRUE;
}


gint32 ipmc_set_cli_sol_deactivate(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    gint32 session_index = 0;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    
    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    
    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        print_sol_deactivate_usage();
        return FALSE;
    }
    if ((session_index = str_to_int(argv[1])) == IPMC_ERROR) {
        print_sol_deactivate_usage();
        return FALSE;
    }
    if ((session_index <= 0) || (session_index > SOL_MAX_CLIENT)) {
        print_sol_deactivate_usage();
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    ret = dal_get_object_handle_nth(CLASS_NAME_SOL_MANAGEMENT, 0, &obj_handle);
    if (ret != RET_OK) {
        return ret;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)session_index));

    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
        CLASS_NAME_SOL_MANAGEMENT, obj_handle, METHOD_DEACTIVATE_SOL, input_list, NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case RET_OK:
            break;
        case ERR_SOL_SESSION_ID_OUT_OF_RANGE:
            g_printf("The SOL session does not exist.\r\n");
            return FALSE;
        case ERR_SOL_SESSION_OPEN_BY_RMCP:
            g_printf("The SOL session can not be closed.\r\n");
            return FALSE;
        case ERR_SOL_SESSION_ALEADY_CLOSED:
            g_printf("The SOL session already be closed.\r\n");
            return FALSE;
        default:
            g_printf("Close SOL session failed.\r\n");
            return FALSE;
    }
    g_printf("Close SOL session successfully.\r\n");
    return TRUE;
}


gint32 ipmc_set_backup_ipaddr(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    struct in_addr ipaddr;
    guint32 ip = 0;
    guint32 mask = 0;
    guchar group_id = -1;
    OBJ_HANDLE obj_handle = 0;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if ((argc != 3) || strcmp(argv[0], "-v")) {
        g_printf("Usage: ipmcset %s-d backupipaddr -v <ipaddr> <mask> \r\n", help_default_name);
        return FALSE;
    }
    
    (void)dal_get_specific_object_byte(ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_ATTRIBUTE_OUT_TYPE, OUT_ETHERNET,
        &obj_handle);

    (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_GROUP_ID, &group_id);

    ret = inet_pton(AF_INET, (const gchar *)argv[1], (void *)&ipaddr.s_addr);
    if (ret <= 0) {
        g_printf("Input parameter format error.\r\n");
        return FALSE;
    }

    ret = inet_pton(AF_INET, (const gchar *)argv[2], (void *)&ipaddr.s_addr);
    if (ret <= 0) {
        g_printf("Input parameter format error.\r\n");
        return FALSE;
    }

    (void)inet_pton(AF_INET, argv[1], (void *)&ip);
    (void)inet_pton(AF_INET, argv[2], (void *)&mask);

    ip = htonl(ip);
    mask = htonl(mask);
    if (mask_judge(mask) != 0) {
        g_printf("Input parameter format error.\r\n");
        return FALSE;
    }

    if (!(ip & (~mask)) || ((ip | mask) == 0xFFFFFFFF) || (mask >= (ip | mask)) || (!(ip & mask))) {
        g_printf("Input parameter format error.\r\n");
        return FALSE;
    }

    ret = set_backup_ipaddr_mask(group_id, argv[1], argv[2]);
    if (ret != RET_OK) {
        return FALSE;
    }

    return TRUE;
}


gint32 ipmc_set_cli_sol_activate(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    guint8 cli_support = 0;
    guint8 com_id = 0;

    
    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    
    if ((argc != 3) || (strcmp(argv[0], "-v"))) {
        print_cli_sol_activate_usage();
        return FALSE;
    }

    gint32 com_index = str_to_int(argv[1]);
    gint32 mode = str_to_int(argv[2]);
    if (com_index == IPMC_ERROR || mode == IPMC_ERROR || com_index > 0xFF) {
        print_cli_sol_activate_usage();
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    
    ret = dal_get_specific_object_byte(SOL_CLASS_NAME, PROPERTY_SOL_COM_INDEX, (guint8)com_index, &obj_handle);
    if (ret != RET_OK) {
        print_cli_sol_activate_usage();
        return FALSE;
    }
    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SOL_CLI_SUPPORT, &cli_support);
    if ((ret != RET_OK) || (cli_support != 1)) {
        print_cli_sol_activate_usage();
        return FALSE;
    }

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SOL_COM_ID, &com_id);
    if (ret != RET_OK) {
        print_cli_sol_activate_usage();
        return FALSE;
    }

    ret = check_sol_com_is_valid(com_id);
    if (ret != RET_OK) {
        print_cli_sol_activate_usage();
        return FALSE;
    }
    
    ret = SOL_INFO.activate_sol(com_id, mode);
    if (ret) {
        ipmc_log_operation_log("CLI", "Connect SOL failed");
        g_printf("Connect SOL failed.\r\n");
        
        return FALSE;
    }
    g_printf("\nSOL connection closed.\r\n");
    return TRUE;
}


gint32 ipmc_del_float_ipaddr(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret = 0;
    gint32 input;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcset %s-d deletefloatip\r\n", help_default_name);
        return FALSE;
    }

    input = ipmc_get_user_input("WARNING: The float IPv4 address will be deleted. Continue? [Y/N]:");
    if ((input == 'y') || (input == 'Y')) {
        ret = delete_float_ip();
        if (ret != RET_OK) {
            g_printf("Delete float ip failed.\r\n");
        } else {
            g_printf("Delete float ip successfully.\r\n");
        }
    }
    return ret;
}


extern gint32 ipmc_del_float_ipaddr_v6(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target)
{
    gint32 ret = 0;
    gint32 input;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcset %s-d deletefloatip6\r\n", help_default_name);
        return FALSE;
    }

    g_printf("WARNING: The float IPv6 address will be deleted. Continue? [Y/N]:\r\n");
    input = ipmc_get_user_input("");
    if ((input == 'y') || (input == 'Y')) {
        ret = delete_float_ipv6();
        if (ret != RET_OK) {
            g_printf("Delete float ipv6 failed.\r\n");
        } else {
            g_printf("Delete float ipv6 successfully.\r\n");
        }
    }
    return ret;
}

LOCAL gint32  ipmc_set_float_ipaddr_check_input(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv)
{
    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        ipmc_log_operation_log(ETH_CLASS_NAME_ETHGROUP, "Set float ipaddr failed.");
        return FALSE;
    }

    if (((argc != 3) && (argc != 4)) || strcmp(argv[0], "-v")) {
        g_printf("Usage: ipmcset %s-d floatipaddr -v <ipaddr> <mask> [gateway] \r\n", help_default_name);
        return FALSE;
    }
    return TRUE;
}

gint32 ipmc_set_float_ipaddr(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    struct in_addr ipaddr;
    guint32 ip = 0;
    guint32 mask = 0;
    guint32 gateway = 0;
    guchar group_id = -1;
    OBJ_HANDLE obj_handle = 0;
    gboolean is_true = 0;
    guint8 out_group_id = -1;
    OBJ_HANDLE out_obj_handle = 0;
    guint8 ipver = 2;

    if (ipmc_set_float_ipaddr_check_input(privilege, rolepriv, argc, argv) != TRUE) {
        return FALSE;
    }
    
    (void)dal_eth_get_out_type_object(OUT_ETHERNET, &out_obj_handle);
    
    // EMM 的IP版本统一以EthGroup0的为准，当IP版本是IPv6的时候，不能设置IPv4地址
    (void)dal_get_property_value_byte(out_obj_handle, ETH_GROUP_ATTRIBUTE_IP_VERSION, &ipver);
    if (ipver == ENABLE_IPV6_ONLY) {
        ipmc_log_operation_log(ETH_CLASS_NAME_ETHGROUP, "Set float ipaddr failed.");
        g_printf("Cannot set IPv4 addresses in IPv6 mode.\r\n");
        return FALSE;
    }
    

    
    (void)dal_get_specific_object_byte(ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_ATTRIBUTE_OUT_TYPE, FLOATIP_ETHERNET,
        &obj_handle);

    (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_GROUP_ID, &group_id);

    ret = inet_pton(AF_INET, (const gchar *)argv[1], (void *)&ipaddr.s_addr);
    if (ret <= 0) {
        g_printf("Input parameter format error.\r\n");
        return FALSE;
    }

    ret = inet_pton(AF_INET, (const gchar *)argv[2], (void *)&ipaddr.s_addr);
    if (ret <= 0) {
        g_printf("Input parameter format error.\r\n");
        return FALSE;
    }

    (void)inet_pton(AF_INET, argv[1], (void *)&ip);
    (void)inet_pton(AF_INET, argv[2], (void *)&mask);

    ip = htonl(ip);
    mask = htonl(mask);
    if (mask_judge(mask) != 0) {
        g_printf("Input parameter format error.\r\n");
        return FALSE;
    }

    if ((!(ip & (~mask))) || ((ip | mask) == 0xFFFFFFFF) || (mask >= (ip | mask)) || (!(ip & mask))) {
        g_printf("Input parameter format error.\r\n");
        ipmc_log_operation_log(ETH_CLASS_NAME_ETHGROUP, "Set float ipaddr failed.");
        return FALSE;
    }
    
    if (argc == 3) {
        if (!check_ip_gateway(ip, mask, FLOATIP_ETHERNET)) {
            return FALSE;
        }
    }
    

    
    if (argc == 4) {
        ret = inet_pton(AF_INET, (const gchar *)argv[3], (void *)&ipaddr.s_addr);
        if (ret <= 0) {
            g_printf("Input parameter format error.\r\n");
            return FALSE;
        }

        (void)inet_pton(AF_INET, argv[3], (void *)&gateway);
        gateway = htonl(gateway);
        // Check Ip,Mask,Gateway
        is_true = ((ip & mask) != (gateway & mask));
        is_true = is_true || ((gateway & (~mask)) == 0);
        is_true = is_true || ((gateway | mask) == 0xFFFFFFFF);
        is_true = is_true && (gateway != 0);
        if (is_true) {
            g_printf("Setting failed. Please check the IP address, subnet mask, and gateway.\n");
            return FALSE;
        }
    }

    

    
    ret = set_ip_address(group_id, argv[1]);
    if (ret != RET_OK) {
        return FALSE;
    }

    
    ret = set_ip_mask(group_id, argv[2]);
    if (ret != RET_OK) {
        return FALSE;
    }

    
    if (argc == 4) {
        (void)dal_get_property_value_byte(out_obj_handle, ETH_GROUP_ATTRIBUTE_GROUP_ID, &out_group_id);

        ret = set_ip_gateway(out_group_id, argv[3]);
        if (ret != RET_OK) {
            return FALSE;
        }
    }
    return TRUE;
}


gint32 ipmc_set_float_ipaddr_v6(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    errno_t safe_fun_ret;
    struct in6_addr v6ipaddr;
    struct in6_addr gateway;
    guchar group_id = -1;
    gint32 ret;
    gchar v6ip_buff[INET6_ADDRSTRLEN] = {0};
    gchar *p = NULL;
    guint32 prefixlen;
    OBJ_HANDLE obj_handle = 0;
    guint8 out_group_id = -1;
    OBJ_HANDLE out_obj_handle = 0;
    guchar is_same = 0;
    guint8 ipver = 2;

    (void)dal_get_specific_object_byte(ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_ATTRIBUTE_OUT_TYPE, FLOATIP_ETHERNET,
        &obj_handle);

    (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_GROUP_ID, &group_id);

    memset_s((gchar *)&v6ipaddr, sizeof(v6ipaddr), 0, sizeof(v6ipaddr));

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (((argc != 2) && (argc != 3)) || strcmp(argv[0], "-v")) {
        g_printf("Usage: ipmcset %s-d floatipaddr6 -v <ipaddr6/prefixlen(0~128)> [gateway6]\r\n", help_default_name);
        return FALSE;
    }
    
    (void)dal_eth_get_out_type_object(OUT_ETHERNET, &out_obj_handle);
    
    // EMM 的IP版本统一以EthGroup0的为准，当IP版本是IPv4的时候，不能设置IPv6地址
    (void)dal_get_property_value_byte(out_obj_handle, ETH_GROUP_ATTRIBUTE_IP_VERSION, &ipver);
    if (ipver == ENABLE_IPV4_ONLY) {
        g_printf("Cannot set IPv6 addresses in IPv4 mode.\r\n");
        return FALSE;
    }
    

    memset_s(v6ipaddr.s6_addr, sizeof(struct in6_addr), 0x00, sizeof(struct in6_addr));
    p = strchr(argv[1], '/');
    if (p == NULL) {
        g_printf("Usage: ipmcset %s-d floatipaddr6 -v <ipaddr6/prefixlen(0~128)> [gateway6]\r\n", help_default_name);
        return FALSE;
    }

    memset_s(v6ip_buff, sizeof(v6ip_buff), 0x00, sizeof(v6ip_buff));
    safe_fun_ret = memcpy_s(v6ip_buff, INET6_ADDRSTRLEN - 1, argv[1], strlen(argv[1]) - strlen(p));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }

    ret = inet_pton(AF_INET6, v6ip_buff, (void *)v6ipaddr.s6_addr);
    if (ret <= 0) {
        g_printf("Input parameter format error.\r\n");
        return FALSE;
    }

    p++;
    if (str_to_int(p) == IPMC_ERROR) {
        g_printf("Usage: ipmcset %s-d floatipaddr6 -v <ipaddr6/prefixlen(0~128)> [gateway6]\r\n", help_default_name);
        return FALSE;
    }

    prefixlen = (guint32)str_to_int(p);
    if (prefixlen > 128) {
        g_printf("Usage: ipmcset %s-d floatipaddr6 -v <ipaddr6/prefixlen(0~128)> [gateway6]\r\n", help_default_name);
        return FALSE;
    }

    if (argc == 3) {
        ret = inet_pton(AF_INET6, argv[2], (void *)gateway.s6_addr);
        if (ret <= 0) {
            g_printf("Input parameter format invalid.\r\n");
            return FALSE;
        }

        ret = ipmc_ipv6_addr_valid_check(argv[2], v6ip_buff);
        if (ret != 0) {
            g_printf("IPv6 address or default gateway is invalid.\r\n");
            return FALSE;
        }

        
        ret = ipmc_eth_judge_ipv6_ip_gateway(v6ip_buff, argv[2], prefixlen, &is_same);
        if ((ret != 0) || (is_same == 0)) {
            g_printf("Failed to verify the IPv6 address and default gateway network segment.\n");
            return FALSE;
        }
    }
    
    if (!set_ipv6_address(group_id, v6ip_buff)) {
        return FALSE;
    }

    
    if (!set_ipv6_prefix(group_id, (guint8)prefixlen)) {
        return FALSE;
    }

    if (argc == 3) {
        (void)dal_get_property_value_byte(out_obj_handle, ETH_GROUP_ATTRIBUTE_GROUP_ID, &out_group_id);

        if (!set_ipv6_gateway(out_group_id, argv[2])) {
            return FALSE;
        }
    }
    return TRUE;
}


gint32 smm_set_offlineconfig(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location)
{
    guint8 as_status = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    GSList *caller_info = NULL;
    OBJ_HANDLE obj_handle = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 4 || strcmp(argv[0], "-v") || argv[3] == NULL) {
        g_printf("Usage: ipmcset -l <swiN> -d offlineconfig -v <bladetype> <planenum> <filename>\r\n");
        return FALSE;
    }

    
    guint8 slot_id = ipmc_get_blade_num(location);
    if (slot_id == 0xff) {
        g_printf("Usage: ipmcset -l <swiN> -d offlineconfig -v <bladetype> <planenum> <filename>\r\n");
        return FALSE;
    }

    g_printf("\rIn this condition,if offline configuration success,the blade will reboot.\n\r");
    gint32 ret = check_the_input_operation(CLI_COMFIRM_STRING);
    if (ret != RET_OK) {
        return FALSE;
    }

    // 判断是否为主板
    (void)get_board_active_state(&as_status);
    if (as_status != ASM_ACTIVE) {
        g_printf("Current MM not active.\r\n ");
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        sprintf_s(username, PARAMETER_LEN + 1, "%s", "unknown");
        sprintf_s(ip, PARAMETER_LEN + 1, "%s", "unknown");
    }

    glong fru_id = 0;
    if (dal_strtol(argv[2], &fru_id, NUMBER_BASE10) != RET_OK || fru_id > 1) {
        g_printf("Planenum error.\r\n");
        return FALSE;
    }

    caller_info = g_slist_append(caller_info, g_variant_new_string(OPERATOR_LOG_CLI));
    caller_info = g_slist_append(caller_info, g_variant_new_string(username)),
    caller_info = g_slist_append(caller_info, g_variant_new_string(ip));

    guint8 slave_addr = ipmc_get_blade_ipmbaddr(slot_id);
    (void)dal_get_specific_object_byte(CLASS_NAME_IPMBETH_BLADE, PROTERY_IPMBETH_BLADE_SLAVEADDR, slave_addr,
        &obj_handle);

    glong blade_type = 0;
    if (dal_strtol(argv[1], &blade_type, NUMBER_BASE10) != RET_OK) {
        uip_free_gvariant_list(caller_info);
        g_printf("Bladetype error.\r\n");
        return FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)location));
    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)blade_type));
    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)fru_id));
    input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)argv[3]));
    input_list = g_slist_append(input_list, g_variant_new_byte(slave_addr));
    ret = dfl_call_class_method(obj_handle, METHOD_OFFLINE_SWI_CFG_FILE, caller_info, input_list, &output_list);
    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(caller_info);
    if (ret != DFL_OK) {
        g_printf("Offline Loading swiconfig file failed.\r\n");
        return FALSE;
    }

    ret = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
    if (ret == RET_OK) {
        g_printf("Offlineconfig %s cfgfile successfully.\r\n", location);
    } else {
        printf_error_info(location, ret);
    }

    uip_free_gvariant_list(output_list);
    return TRUE;
}
