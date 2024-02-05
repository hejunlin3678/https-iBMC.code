

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <pwd.h>
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
#include "pme/ipmi/common_ipmi.h"

#include "shelfsetcommand.h"
#include "ipmc_shelf_common.h"
#include "ipmcsethelp.h"
#include "cli_sol.h"
#include "smm_sol_client.h"
#include "ipmcsetcommand.h"
#include "ipmcset_netconfig.h"
#include "ipmcset_power.h"
#include "ipmcset_alarm.h"

LOCAL gint32 get_blade_obj_handle(gchar *location, OBJ_HANDLE *obj_handle, guint8 *slot_id);
LOCAL void print_blade_activeport_helpinfo(guchar *input_val, guint8 slot_id);
LOCAL gint32 ipmc_judge_blade_mntportcab_valiad(guint8 net_type, guchar *input_val);
LOCAL void print_set_blade_gateway_helpinfo(void);
LOCAL void print_set_blade_gateway6_helpinfo(void);
LOCAL void print_set_blade_ip_helpinfo(void);
LOCAL void print_set_blade_ipv6_helpinfo(void);
LOCAL void print_set_blade_ipmode6_helpinfo(void);
LOCAL void print_set_blade_ipmode_helpinfo(void);

LOCAL void print_ipmi_data(const guint8 *resp_data, guint32 length);

#ifdef ARM64_HI1711_ENABLED

LOCAL gint32 get_slot_id(guint8 *slot_id);
LOCAL gint32 get_smm_port_name(const gchar *input_name, guint8 host_slot_id, gchar *port_name, guint32 port_len,
                               guint8 flag);

#endif

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


LOCAL void print_set_blade_ipmode_helpinfo(void)
{
    guint8 software_type = 0;

    (void)dal_get_software_type(&software_type);

    if (software_type == MGMT_SOFTWARE_TYPE_EM) {
        g_printf("Usage: ipmcset -l <bladeN|swiN> -d ipmode -v <dhcp|static>\r\n");
    } else {
        g_printf("Usage: ipmcset -l <bladeN> -d ipmode -v <dhcp|static>\r\n");
    }
}


gint32 smm_set_blade_ipmode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location)
{
    gint32 ret;
    guchar ipmode = 0;
    guint8 slave_addr;
    guint8 slot_id;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 2 || strcmp(argv[0], "-v")) {
        print_set_blade_ipmode_helpinfo();
        return FALSE;
    }

    if (!strcmp(argv[1], "dhcp")) {
        ipmode = 2;
    } else if (!strcmp(argv[1], "static")) {
        ipmode = 1;
    } else {
        g_printf("Usage: ipmcset -l <bladeN|swiN> -d ipmode -v <dhcp|static>\r\n");
        return FALSE;
    }

    
    slot_id = ipmc_get_blade_num(location);
    if (slot_id == 0xff) {
        print_set_blade_ipmode_helpinfo();
        return FALSE;
    }
    slave_addr = ipmc_get_blade_ipmbaddr(slot_id);
    ret = set_blade_bmc_ip_config_by_type(slave_addr, &ipmode, SET_IP_TYPE_IP_MODE);
    if (ret != RET_OK) {
        return FALSE;
    }

    return TRUE;
}


LOCAL void print_set_blade_ipmode6_helpinfo(void)
{
    guint8 software_type = 0;

    (void)dal_get_software_type(&software_type);

    if (software_type == MGMT_SOFTWARE_TYPE_EM) {
        g_printf("Usage: ipmcset -l <bladeN|swiN> -d ip6mode -v <dhcp|static>\r\n");
    } else {
        g_printf("Usage: ipmcset -l <bladeN> -d ip6mode -v <dhcp|static>\r\n");
    }
}


gint32 smm_set_blade_ip6mode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location)
{
    gint32 ret;
    guchar ip6mode = 0;
    guint8 slot_id;
    guint8 slave_addr;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 2 || strcmp(argv[0], "-v")) {
        print_set_blade_ipmode6_helpinfo();
        return FALSE;
    }

    if (!strcmp(argv[1], "dhcp")) {
        ip6mode = 2;
    } else if (!strcmp(argv[1], "static")) {
        ip6mode = 1;
    } else {
        print_set_blade_ipmode6_helpinfo();
        return FALSE;
    }

    
    slot_id = ipmc_get_blade_num(location);
    if (slot_id == 0xff) {
        print_set_blade_ipmode6_helpinfo();
        return FALSE;
    }

    slave_addr = ipmc_get_blade_ipmbaddr(slot_id);
    ret = set_blade_bmc_ip_config_by_type(slave_addr, &ip6mode, SET_IP_TYPE_IP6_MODE);
    if (ret != RET_OK) {
        return FALSE;
    }

    return TRUE;
}


gint32 smm_set_blade_ipaddr(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location)
{
    gint32 ret;
    struct in_addr ipaddr;
    guint32 ip = 0;
    guint32 mask = 0;
    guint32 gateway = 0;
    gboolean is_true = 0;
    guint8 slave_addr;
    guint8 slot_id;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (((argc != 3) && (argc != 4)) || strcmp(argv[0], "-v")) {
        print_set_blade_ip_helpinfo();
        return FALSE;
    }

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
        return FALSE;
    }

    
    slot_id = ipmc_get_blade_num(location);
    if (slot_id == 0xff) {
        print_set_blade_ip_helpinfo();
        return FALSE;
    }
    slave_addr = ipmc_get_blade_ipmbaddr(slot_id);
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
            g_printf("Setting gateway failed. Please check the IP address, subnet mask, and gateway.\n");
            return FALSE;
        }
    }

    
    ret = set_blade_bmc_ip_config_by_type(slave_addr, argv[1], SET_IP_TYPE_IP_ADDR);
    if (ret != RET_OK) {
        return FALSE;
    }

    
    ret = set_blade_bmc_ip_config_by_type(slave_addr, argv[2], SET_IP_TYPE_IP_SUBMASK);
    if (ret != RET_OK) {
        return FALSE;
    }

    
    if (argc == 4) {
        
        ret = set_blade_bmc_ip_config_by_type(slave_addr, argv[3], SET_IP_TYPE_IP_GATEWAY);
        if (ret != RET_OK) {
            return FALSE;
        }
    }
    

    return TRUE;
}

LOCAL void print_set_blade_ipv6_helpinfo(void)
{
    guint8 software_type = 0;

    (void)dal_get_software_type(&software_type);

    if (software_type == MGMT_SOFTWARE_TYPE_EM) {
        g_printf("Usage: ipmcset -l <bladeN|swiN> -d ipaddr6 -v <ipaddr6/prefixlen(0~128)> [gateway6]\r\n");
    } else {
        g_printf("Usage: ipmcset -l <bladeN> -d ipaddr6 -v <ipaddr6/prefixlen(0~128)> [gateway6]\r\n");
    }
}

LOCAL void print_set_blade_ip_helpinfo(void)
{
    guint8 software_type = 0;

    (void)dal_get_software_type(&software_type);

    if (software_type == MGMT_SOFTWARE_TYPE_EM) {
        g_printf("Usage: ipmcset -l <bladeN|swiN> -d ipaddr -v <ipaddr> <mask> [gateway]\r\n");
    } else {
        g_printf("Usage: ipmcset -l <bladeN> -d ipaddr -v <ipaddr> <mask> [gateway]\r\n");
    }
}


gint32 smm_set_blade_ip6addr(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location)
{
    errno_t safe_fun_ret;
    struct in6_addr v6ipaddr;
    struct in6_addr gateway;
    guchar is_same = 0;
    gint32 ret;
    gchar buff[INET6_ADDRSTRLEN] = { 0 };
    gchar *p = NULL;
    guint8 prefixlen;
    guint8 slot_id;
    guint8 slave_addr;

    memset_s((gchar *)&v6ipaddr, sizeof(v6ipaddr), 0, sizeof(v6ipaddr));
    memset_s(gateway.s6_addr, sizeof(struct in6_addr), 0x00, sizeof(struct in6_addr));

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if ((((argc != 2) && (argc != 3)) || strcmp(argv[0], "-v"))) {
        print_set_blade_ipv6_helpinfo();
        return FALSE;
    }

    memset_s(v6ipaddr.s6_addr, sizeof(struct in6_addr), 0x00, sizeof(struct in6_addr));
    p = strchr(argv[1], '/');
    if (p == NULL) {
        print_set_blade_ipv6_helpinfo();
        return FALSE;
    }

    
    slot_id = ipmc_get_blade_num(location);
    if (slot_id == 0xff) {
        print_set_blade_ipv6_helpinfo();
        return FALSE;
    }

    slave_addr = ipmc_get_blade_ipmbaddr(slot_id);

    memset_s(buff, sizeof(buff), 0x00, sizeof(buff));
    safe_fun_ret = memcpy_s(buff, INET6_ADDRSTRLEN - 1, argv[1], strlen(argv[1]) - strlen(p));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }

    ret = inet_pton(AF_INET6, buff, (void *)v6ipaddr.s6_addr);
    if (ret <= 0) {
        g_printf("Input parameter format error.\r\n");
        return FALSE;
    }

    p++;
    if (str_to_int(p) == IPMC_ERROR) {
        g_printf("Usage: ipmcset -l <bladeN|swiN> -d ipaddr6 -v <ipaddr6/prefixlen(0~128)> [gateway6]\r\n");
        return FALSE;
    }

    prefixlen = (guint8)str_to_int(p);
    if (prefixlen > 128) {
        g_printf("Usage: ipmcset -l <bladeN|swiN> -d ipaddr6 -v <ipaddr6/prefixlen(0~128)> [gateway6]\r\n");
        return FALSE;
    }

    if (argc == 3) {
        ret = inet_pton(AF_INET6, argv[2], (void *)gateway.s6_addr);
        if (ret <= 0) {
            g_printf("Input parameter format invalid.\r\n");
            return FALSE;
        }

        ret = ipmc_ipv6_addr_valid_check(argv[2], buff);
        if (ret != 0) {
            g_printf("IPv6 address or gateway is invalid.\r\n");
            return FALSE;
        }

        
        ret = ipmc_eth_judge_ipv6_ip_gateway(buff, argv[2], prefixlen, &is_same);
        if ((ret != 0) || (is_same == 0)) {
            g_printf("Failed to verify the IPv6 address and default gateway network segment.\n");
            return FALSE;
        }
    }

    
    if (set_blade_bmc_ip_config_by_type(slave_addr, buff, SET_IP_TYPE_IP6_ADDR) != RET_OK) {
        return FALSE;
    }

    
    
    (void)vos_task_delay(1000);
    
    
    if (set_blade_bmc_ip_config_by_type(slave_addr, &prefixlen, SET_IP_TYPE_IP6_PREFIX) != RET_OK) {
        return FALSE;
    }

    (void)vos_task_delay(1000);
    if (argc == 3) {
        
        if (set_blade_bmc_ip_config_by_type(slave_addr, argv[2], SET_IP_TYPE_IP6_GATEWAY) != RET_OK) {
            return FALSE;
        }
        (void)vos_task_delay(1000);
    }

    return TRUE;
}


LOCAL void print_set_blade_gateway6_helpinfo(void)
{
    guint8 software_type = 0;

    (void)dal_get_software_type(&software_type);

    if (software_type == MGMT_SOFTWARE_TYPE_EM) {
        g_printf("Usage: ipmcset -l <bladeN|swiN> -d gateway6 -v <gateway6>\r\n");
    } else {
        g_printf("Usage: ipmcset -l <bladeN> -d gateway6 -v <gateway6>\r\n");
    }
}


gint32 smm_set_blade_gateway6(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location)
{
    gint32 ret;
    struct in6_addr v6ipaddr;
    guint8 slot_id;
    guint8 slave_addr;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        print_set_blade_gateway6_helpinfo();
        return FALSE;
    }

    memset_s(v6ipaddr.s6_addr, sizeof(struct in6_addr), 0x00, sizeof(struct in6_addr));
    ret = inet_pton(AF_INET6, argv[1], (void *)v6ipaddr.s6_addr);
    if (ret <= 0) {
        g_printf("Input parameter format invalid.\r\n");
        return FALSE;
    }

    
    slot_id = ipmc_get_blade_num(location);
    if (slot_id == 0xff) {
        print_set_blade_gateway6_helpinfo();
        return FALSE;
    }

    slave_addr = ipmc_get_blade_ipmbaddr(slot_id);
    ret = set_blade_bmc_ip_config_by_type(slave_addr, argv[1], SET_IP_TYPE_IP6_GATEWAY);
    if (ret != TRUE) {
        return FALSE;
    }

    return TRUE;
}


LOCAL void print_set_blade_gateway_helpinfo(void)
{
    guint8 software_type = 0;

    (void)dal_get_software_type(&software_type);

    if (software_type == MGMT_SOFTWARE_TYPE_EM) {
        g_printf("Usage: ipmcset -l <bladeN|swiN> -d gateway -v <gateway>\r\n");
    } else {
        g_printf("Usage: ipmcset -l <bladeN> -d gateway -v <gateway>\r\n");
    }
}


gint32 smm_set_blade_gateway(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location)
{
    gint32 ret;
    struct in_addr ipaddr;
    guint8 slot_id;
    guint8 slave_addr;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        print_set_blade_gateway_helpinfo();
        return FALSE;
    }

    ret = inet_pton(AF_INET, (const gchar *)argv[1], (void *)&ipaddr.s_addr);
    if (ret <= 0) {
        g_printf("Input parameter format error.\r\n");
        return FALSE;
    }

    
    slot_id = ipmc_get_blade_num(location);
    if (slot_id == 0xff) {
        print_set_blade_gateway_helpinfo();
        return FALSE;
    }

    slave_addr = ipmc_get_blade_ipmbaddr(slot_id);
    ret = set_blade_bmc_ip_config_by_type(slave_addr, argv[1], SET_IP_TYPE_IP_GATEWAY);
    if (ret != RET_OK) {
        return FALSE;
    }

    return TRUE;
}


gint32 smm_set_blade_vlan_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location)
{
    guint16 vlan_id_min;
    guint16 vlan_id_max;
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GSList *output_list = NULL;
    guint8 slot_id;
    guint8 slave_addr;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    ret = check_blade_present_communication_status(location);
    if (ret != IPMC_OK) {
        return FALSE;
    }

    ret = get_blade_obj_handle(location, &obj_handle, NULL);
    if (ret != RET_OK) {
        return FALSE;
    }

    ret = dfl_call_class_method(obj_handle, METHOD_SHELF_GET_BLADE_VLAN_ID_RANGE, NULL, NULL, &output_list);
    if (ret != DFL_OK) {
        g_printf("Get vlan id range fail\r\n");
        uip_free_gvariant_list(output_list);
        return RET_ERR;
    }

    vlan_id_min = g_variant_get_uint16((GVariant *)g_slist_nth_data(output_list, 0));
    vlan_id_max = g_variant_get_uint16((GVariant *)g_slist_nth_data(output_list, 1));

    uip_free_gvariant_list(output_list);

    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        g_printf("Usage: ipmcset -l bladeN -d vlan -v <off | id(%d~%d)>\r\n", vlan_id_min, vlan_id_max);
        return FALSE;
    }

    slot_id = ipmc_get_blade_num(location);
    if (slot_id == 0xff) {
        return FALSE;
    }

    slave_addr = ipmc_get_blade_ipmbaddr(slot_id);
    ret = set_blade_bmc_ip_config_by_type(slave_addr, argv[1], SET_IP_TYPE_VLAN_STATE);
    if (ret != TRUE) {
        return FALSE;
    }

    return TRUE;
}


gint32 smm_set_blade_net_mode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location)
{
    gint32 net_mode = 0;
    gint32 ret;
    guint8 slot_id;
    guint8 slave_addr;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        print_netmode_usage(TRUE);
        return FALSE;
    }

    if ((net_mode = str_to_int(argv[1])) == IPMC_ERROR) {
        print_netmode_usage(TRUE);
        return FALSE;
    }

    if (net_mode != 1 && net_mode != 2) {
        print_netmode_usage(TRUE);
        return FALSE;
    }

    slot_id = ipmc_get_blade_num(location);
    if (slot_id == 0xff) {
        return FALSE;
    }
    slave_addr = ipmc_get_blade_ipmbaddr(slot_id);
    ret = set_blade_bmc_ip_config_by_type(slave_addr, &net_mode, SET_IP_TYPE_NETMODE);
    if (ret != TRUE) {
        return FALSE;
    }

    return TRUE;
}


LOCAL gint32 ipmc_judge_blade_mntportcab_valiad(guint8 net_type, guchar *input_val)
{
    guint8 dedicate_enable;
    guint8 lomncsi_enable;
    guint8 pciencsi_enable;
    guint8 aggregation_enable;
    guint8 lom2ncsi_enable;

    if (input_val == NULL) {
        return RET_ERR;
    }

    dedicate_enable = input_val[0];
    aggregation_enable = input_val[1];
    lomncsi_enable = input_val[2];
    pciencsi_enable = input_val[3];
    lom2ncsi_enable = input_val[4];

    switch (net_type) {
        case NET_TYPE_DEDICATED:
            if (dedicate_enable == 0) {
                return RET_ERR;
            }

            break;

        case NET_TYPE_LOM:
            if (lomncsi_enable == 0) {
                return RET_ERR;
            }

            break;

        case NET_TYPE_PCIE:
            if (pciencsi_enable == 0) {
                return RET_ERR;
            }

            break;

        case NET_TYPE_AGGREGATION:
            if (aggregation_enable == 0) {
                return RET_ERR;
            }

            break;

        case NET_TYPE_LOM2:
            if (lom2ncsi_enable == 0) {
                return RET_ERR;
            }

            break;

        default:
            g_printf("Input net type error.\r\n");
            return RET_ERR;
    }

    return RET_OK;
}




LOCAL void print_blade_activeport_helpinfo(guchar *input_val, guint8 slot_id)
{
    guint8 dedicate_enable;
    guint8 lomncsi_enable;
    guint8 pciencsi_enable;
    guint8 aggregation_enable;
    guint8 lom2ncsi_enable;

    if (input_val == NULL) {
        return;
    }

    dedicate_enable = input_val[0];
    aggregation_enable = input_val[1];
    lomncsi_enable = input_val[2];
    pciencsi_enable = input_val[3];
    lom2ncsi_enable = input_val[4];

    if (dedicate_enable == 0 && lomncsi_enable == 0 && pciencsi_enable == 0 && aggregation_enable == 0 &&
        lom2ncsi_enable == 0) {
        g_printf("The blade%d system is not support to active port.\r\n", slot_id);
        return;
    }

    g_printf("Usage: ipmcset -l bladeN -d activeport -v <option> [portid]\r\n");
    g_printf("Options are:\r\n");

    if (dedicate_enable == 1) {
        g_printf("    0      :  Dedicated network port, without parameter [portid]\r\n");
    }

    if (lomncsi_enable == 1) {
        g_printf("    1      :  LOM network port\r\n");
    }

    if (pciencsi_enable == 1) {
        g_printf("    2      :  PCIE network port\r\n");
    }

    if (aggregation_enable == 1) {
        g_printf("    3      :  Aggregation network port, without parameter [portid]\r\n");
    }

    if (lom2ncsi_enable == 1) {
        
        g_printf("    4      :  FlexIO network port\r\n");
        
    }
}



LOCAL gint32 get_blade_obj_handle(gchar *location, OBJ_HANDLE *obj_handle, guint8 *slot_id)
{
    guint8 slave_addr;
    gint32 ret;
    guint8 slot_id_inner;

    if (location == NULL || obj_handle == NULL) {
        return RET_ERR;
    }

    
    slot_id_inner = ipmc_get_blade_num(location);
    if (slot_id_inner == 255) {
        g_printf("Invalid blade name.\r\n");
        return RET_ERR;
    }
    slave_addr = ipmc_get_blade_ipmbaddr(slot_id_inner);

    // 上层已做过合法性检查
    ret =
        dal_get_specific_object_byte(CLASS_NAME_IPMBETH_BLADE, PROTERY_IPMBETH_BLADE_SLAVEADDR, slave_addr, obj_handle);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    if (slot_id) {
        *slot_id = slot_id_inner;
    }

    return RET_OK;
}


gint32 smm_set_blade_active_port(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GSList *output_list = NULL;
    guchar input_val[5] = {0};
    guint8 slot_id = 0;
    guint8 net_type = 0;
    gint32 port_num = 0;
    guint8 slave_addr;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    ret = check_blade_present_communication_status(location);
    if (ret != IPMC_OK) {
        return FALSE;
    }

    ret = get_blade_obj_handle(location, &obj_handle, &slot_id);
    if (ret != RET_OK) {
        return FALSE;
    }

    ret = dfl_call_class_method(obj_handle, METHOD_SHELF_GET_BLADE_MNGTPORTCAP, NULL, NULL, &output_list);
    if (ret != DFL_OK) {
        g_printf("Get blade%d MngtPortCap fail\r\n", slot_id);
        uip_free_gvariant_list(output_list);
        return RET_ERR;
    }

    input_val[0] = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 0));
    input_val[1] = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 1));
    input_val[2] = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 2));
    input_val[3] = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 3));
    input_val[4] = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 4));

    uip_free_gvariant_list(output_list);

    if (argc == 2) {
        if (strcmp(argv[0], "-v")) {
            print_blade_activeport_helpinfo(input_val, slot_id);
            return FALSE;
        }
        if ((net_type = str_to_int(argv[1])) == 0) {
            net_type = NET_TYPE_DEDICATED;
        } else if ((net_type = str_to_int(argv[1])) == 3) {
            net_type = NET_TYPE_AGGREGATION;
        } else {
            print_blade_activeport_helpinfo(input_val, slot_id);
            return FALSE;
        }
    } else if (argc == 3) {
        if (strcmp(argv[0], "-v")) {
            print_blade_activeport_helpinfo(input_val, slot_id);
            return FALSE;
        }
        if ((net_type = str_to_int(argv[1])) == 1) {
            net_type = NET_TYPE_LOM;
        } else if ((net_type = str_to_int(argv[1])) == 2) {
            net_type = NET_TYPE_PCIE;
        } else if ((net_type = str_to_int(argv[1])) == 4) {
            net_type = NET_TYPE_LOM2;
        } else {
            print_blade_activeport_helpinfo(input_val, slot_id);
            return FALSE;
        }

        if ((port_num = str_to_int(argv[2])) == IPMC_ERROR) {
            print_blade_activeport_helpinfo(input_val, slot_id);
            return FALSE;
        }
    } else {
        print_blade_activeport_helpinfo(input_val, slot_id);
        return FALSE;
    }

    if (port_num > 0xFF || port_num < 0) {
        print_blade_activeport_helpinfo(input_val, slot_id);
        return FALSE;
    }

    
    ret = ipmc_judge_blade_mntportcab_valiad(net_type, input_val);
    if (ret != RET_OK) {
        print_blade_activeport_helpinfo(input_val, slot_id);
        return FALSE;
    }

    (void)memset_s(input_val, sizeof(input_val), 0, sizeof(input_val));
    input_val[0] = net_type;
    input_val[1] = (guchar)port_num; // 上面已做了范围判断

    slot_id = ipmc_get_blade_num(location);
    if (slot_id == 0xff) {
        return FALSE;
    }

    slave_addr = ipmc_get_blade_ipmbaddr(slot_id);
    ret = set_blade_bmc_ip_config_by_type(slave_addr, input_val, SET_IP_TYPE_ACTIVE_PORT);
    if (ret != TRUE) {
        return FALSE;
    }

    return RET_OK;
}

gint32 ipmc_enable_otm_port(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gchar username[PARAMETER_LEN + 1] = { 0 };
    gchar ip[PARAMETER_LEN + 1] = { 0 };
    GSList *input_list = NULL;
    gint32 input;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    gint32 enable = -1;
    if (argc != 2 || strcmp("-v", argv[0]) != 0 || dal_strtoi(argv[1], &enable, NUMBER_BASE10) != RET_OK
        || (enable != 0 && enable != 1)) {
        g_printf("Usage: ipmcset %s-d otmportenable -v <1/0>\r\n", help_default_name);
        return FALSE;
    }

    g_printf("This operation will set management 10GE port optical module %s.Continue?[Y/N]:\r\n",
        enable == 1 ? "enabled" : "disabled");
    input = ipmc_get_user_input("");
    if (input == 'n' || input == 'N') {
        return FALSE;
    }
    if (input != 'y' && input != 'Y') {
        g_printf("Input character invalid.\r\n");
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)enable));

    gint32 ret =
        uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_NAME_MM_PORT_OTM_INFO,
        OBJECT_NAME_MANAGEMENT_PORT_ADAPTER, METHOD_SET_MM_OTM_PORT_ENABLE, input_list, NULL);
    if (ret != RET_OK) {
        g_printf("Set 10GE management port optical module %s failed.\r\n", enable == 1 ? "enabled" : "disabled");
        uip_free_gvariant_list(input_list);
        return RET_ERR;
    }

    g_printf("Set 10GE management port optical module %s successfully.\r\n", enable == 1 ? "enabled" : "disabled");
    uip_free_gvariant_list(input_list);

    return RET_OK;
}


gint32 ipmc_send_ipmicmd(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gpointer response = NULL;
    gint32 result;
    const guint8 *resp_src_data = NULL;
    IPMI_REQ_MSG_HEAD_S req_msg_head;
    gint32 retry_count = 0;
    gchar **split_str = NULL;
    guint8 data_len = 0;
    guint8 reqdata[128] = {0};
    gint32 i;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    IPMI_MSG_FILTER_COND_T msg_filter_cond = { 0 };
    OBJ_HANDLE object_handle;
    const guint8 *src_data = NULL;
    gsize msg_length = 0;
#define MAX_IPMI_RETRY_COUNT 24

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    
    if ((argc != 4 && argc != 3) || strcmp(argv[0], "-v")) {
        g_printf("Usage: ipmcset [-l location] -d ipmicommand -v netfun cmd \"data1 data2 ...\"\r\n");
        return FALSE;
    }
    
    if (!strcmp(target, "smm") || !strcmp(target, "fantray") || !strcmp(target, "pem") || !strcmp(target, "lsw")) {
        req_msg_head.target_type = IPMI_SMM;
        req_msg_head.target_instance = 1;
    } else if (!strncmp(target, "blade", strlen("blade")) || !strncmp(target, "swi", strlen("swi"))) {
        
        req_msg_head.target_type = 0x08;
        req_msg_head.target_instance = get_blade_num(target);
    } else {
        
        g_printf("location %s is invalid, please check it.\r\n", target);
        return FALSE;
    }

    for (i = 0; i < 10; i++) {
        if (vos_get_file_accessible(SMM_IPMI_CHANNEL_OK_FILE) == TRUE) {
            break;
        }
        sleep(3);
    }

    
    req_msg_head.netfn = str_to_int(argv[1]);
    req_msg_head.lun = 0;
    req_msg_head.cmd = str_to_int(argv[2]);
    
    req_msg_head.src_len = 0;

    
    if (argc == 4) {
        split_str = g_strsplit(argv[3], " ", 256);
        if (split_str == NULL) {
            g_printf("Usage: ipmcset [-l location] -d ipmicommand -v netfun cmd \"data1 data2 ...\"\r\n");
            return FALSE;
        }
        for (i = 0; i < g_strv_length(split_str); i++) {
            
            if (strlen(split_str[i]) == 0) {
                continue;
            }
            reqdata[data_len++] = str_to_int(split_str[i]);
        }
        req_msg_head.src_len = data_len;
        g_strfreev(split_str);
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    (void)dal_get_object_handle_nth(CLASS_NAME_IPMI_MESSAGE_FILTER, 0, &object_handle);
    
    if ((req_msg_head.netfn == NETFN_APP_REQ) && (req_msg_head.cmd == IPMI_SEND_MESSAGE)) {
        msg_filter_cond.dest_lun = reqdata[2] & 0x03;
        msg_filter_cond.dest_netfn = reqdata[2] >> 2;
        msg_filter_cond.src_lun = reqdata[5] & 0x03;
        msg_filter_cond.src_seq = reqdata[5] >> 2;
        msg_filter_cond.cmd = reqdata[6];
        input_list = g_slist_append(input_list, g_variant_new_byte(1));
        input_list = g_slist_append(input_list,
            g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, &msg_filter_cond, sizeof(msg_filter_cond), sizeof(guint8)));
        result = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
            CLASS_NAME_IPMI_MESSAGE_FILTER, dfl_get_object_name(object_handle), METHOD_SET_FILTER_CONDITION, input_list,
            NULL);
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        input_list = NULL;
        if (result != RET_OK) {
            g_printf("Failed.\r\n");
            return FALSE;
        }
    }
SEND_IPMI_MSG:
    
    result = ipmi_send_request(&req_msg_head, reqdata, MAX_POSSIBLE_IPMI_FRAME_LEN, &response, TRUE);
    if (result != RET_OK) {
        g_printf("send ipmimsg failed, netfn=0x%x, cmd=0x%x, ret=0x%x.\r\n", req_msg_head.netfn, req_msg_head.cmd,
            result);
        return FALSE;
    }
    
    guint8 rep_len = get_ipmi_src_data_len(response);
    if (rep_len == 0) {
        g_free(response);
        g_printf("get ipmicmd failed, netfn=0x%x, cmd=0x%x, respdata null.\r\n", req_msg_head.netfn, req_msg_head.cmd);
        return FALSE;
    }
    
    resp_src_data = get_ipmi_src_data(response);
    if (resp_src_data == NULL) {
        g_free(response);
        g_printf("get ipmicmd failed, netfn=0x%x, cmd=0x%x, respdata null.\r\n", req_msg_head.netfn, req_msg_head.cmd);
        return FALSE;
    }

    if (resp_src_data[0] == 0xc3) {
        (void)dfl_get_object_handle(target, &object_handle);
        result = get_blade_presence(object_handle);
        if (result == SHELF_PART_PRESENCE && retry_count < MAX_IPMI_RETRY_COUNT) {
            retry_count++;
            g_free(response);
            response = NULL;
            sleep(5);
            goto SEND_IPMI_MSG;
        }
    }

    g_printf("ResponseData:\r\n");
    g_printf("[");
    
    if ((req_msg_head.netfn != NETFN_APP_REQ) || (req_msg_head.cmd != IPMI_SEND_MESSAGE)) {
        (void)print_ipmi_data(resp_src_data, rep_len);
    } else {
        vos_task_delay(1000);
        input_list = g_slist_append(input_list,
            g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, &msg_filter_cond, sizeof(msg_filter_cond), sizeof(guint8)));
        result = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
            CLASS_NAME_IPMI_MESSAGE_FILTER, dfl_get_object_name(object_handle), METHOD_GET_FILTERED_MSG, input_list,
            &output_list);
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        input_list = NULL;
        if (result != RET_OK) {
            g_printf("Failed.\r\n");
            goto CLEAR_FILTER_COND;
        }

        src_data = (const guint8 *)g_variant_get_fixed_array((GVariant *)g_slist_nth_data(output_list, 0), &msg_length,
            sizeof(guint8));
        if (src_data == NULL) {
            g_printf("Failed.\r\n");
            g_free(response);
            return FALSE;
        }

        (void)print_ipmi_data(src_data, msg_length);
    CLEAR_FILTER_COND:
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        output_list = NULL;

        
        input_list = g_slist_append(input_list, g_variant_new_byte(0));
        input_list = g_slist_append(input_list,
            g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, &msg_filter_cond, sizeof(msg_filter_cond), sizeof(guint8)));
        result = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
            CLASS_NAME_IPMI_MESSAGE_FILTER, dfl_get_object_name(object_handle), METHOD_SET_FILTER_CONDITION, input_list,
            NULL);
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        if (result != RET_OK) {
            g_printf("Failed.\r\n");
            g_free(response);
            return FALSE;
        }
    }

    g_free(response);
    return TRUE;
}


gint32 ipmc_set_lsw_agetest(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    GSList *input_list = NULL;
    gint32 total = 0;
    gint32 ret;
    gchar ip[PARAMETER_LEN + 1] = {0};
    gchar username[PARAMETER_LEN + 1] = {0};
    glong result = 0;
    gchar *end = NULL;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        g_printf("Permission denied.\r\n");
        return FALSE;
    }
    if (argc < 2) {
        g_printf("Usage: ipmcset %s-d agetest -v <enable|disable> [value]\r\n", help_default_name);
        return FALSE;
    }
    if (strcmp("-v", argv[0]) != 0) {
        g_printf("Usage: ipmcset %s-d agetest -v <enable|disable> [value]\r\n", help_default_name);
        return FALSE;
    }
    if (strcmp("enable", argv[1]) != 0 && strcmp("disable", argv[1]) != 0) {
        g_printf("Usage: ipmcset %s-d agetest -v <enable|disable> [value]\r\n", help_default_name);
        return FALSE;
    }

    if (strcmp("disable", argv[1]) == 0) {
        if (argc != 2) {
            g_printf("Usage: ipmcset %s-d agetest -v disable\r\n", help_default_name);
            return FALSE;
        }

        total = 0;
    }

    if (strcmp("enable", argv[1]) == 0) {
        if (argc != 3) {
            g_printf("Usage: ipmcset %s-d agetest -v enable agetime\r\n", help_default_name);
            return FALSE;
        }

        result = strtol(argv[2], &end, 10);
        if (end != NULL && *end == 0 && result >= 1 && result <= 24) {
            total = (gint32)result;
        } else {
            g_printf("Please input integer value and make sure agetime within [1-24].\r\n");
            return FALSE;
        }
    }

    input_list = g_slist_append(input_list, g_variant_new_int32(60 * total));
    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_NAME_MM_LSW_MGNT,
        OBJ_NAME_MM_LSW_MGNT, METHOD_SET_LSW_AGETEST, input_list, NULL);
    if (ret != RET_OK) {
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        g_printf("Connecting to lsw module failed.\r\n");
        return FALSE;
    }

    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    g_printf("Success.\r\n");
    return RET_OK;
}


gint32 smm_set_inner_subnet_segment(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    errno_t safe_fun_ret;
    gint32 ret;
    struct in_addr ipaddr;
    
    gchar ip_str[IPV4_IP_STR_SIZE + 1] = { 0 };
    
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    gint32 err_code;
    gchar username[PARAMETER_LEN + 1] = { 0 };
    gchar ip[PARAMETER_LEN + 1] = { 0 };

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 2 || strcmp(argv[0], "-v")) {
        g_printf("Usage: ipmcset -l shelf -d subnet -v <ipaddr>\r\n");
        return FALSE;
    }
    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    
    if (strlen((const gchar *)argv[1]) > IPV4_IP_STR_SIZE) {
        g_printf("Input parameter format error.\r\n");
        return FALSE;
    }
    

    if (dal_is_support_eth_define_specific_propery(PROPERTY_SUPPORT_MULTI_VLAN)) {
        g_printf("Do not support set inner subnet segment.\r\n");
        return FALSE;
    }

    safe_fun_ret = strncpy_s(ip_str, sizeof(ip_str), (const gchar *)argv[1], sizeof(ip_str) - 1);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }

    ret = inet_pton(AF_INET, ip_str, (void *)&ipaddr.s_addr);
    if (ret <= 0) {
        g_printf("Input parameter format error.\r\n");
        return FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)argv[1]));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_NAME_SHELF_MANAGE,
        OBJ_NAME_SHELF_MANAGE, METHOD_SET_SHELF_INNER_SUBNET_SEGMENT, input_list, &output_list);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        g_printf("Set inner subnet segment failed.\r\n");
        return FALSE;
    }

    err_code = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
    uip_free_gvariant_list(output_list);

    if (err_code != RET_OK) {
        g_printf("Set inner subnet segment failed.\r\n");
        if (err_code == ERR_SHELF_INNER_SUBNET_PARAM_ERROR) {
            g_printf("Input parameter error. e.g.XXX.XXX.0.0\r\n");
            g_printf("subnet segment scope: \r\n");
            g_printf("10.0.0.0~10.255.0.0\r\n");
            g_printf("172.16.0.0~172.31.0.0\r\n");
            g_printf("192.168.0.0\r\n");
        }
        if (err_code == ERR_SHELF_INNER_SUBNET_NOT_CHANGE) {
            g_printf("\rThe subnet address is not change.\r\n");
        }
        return FALSE;
    }

    g_printf("Set inner subnet segment successfully.\r\n");

    return TRUE;
}

gint32 smm_set_outport_mode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 iRet;
    gchar obj_name[OBJ_NAME_MAX_LEN] = {0};
    gint32 ret;
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    guint8 option = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 2 || strcmp(argv[0], "-v")) {
        print_set_outport_mode_usage();
        return FALSE;
    }
    if (strlen(argv[1]) != 1) {
        print_set_outport_mode_usage();
        return FALSE;
    }

    ret = vos_str2int(argv[1], 10, &option, NUM_TPYE_INT32);
    if (ret != RET_OK) {
        print_set_outport_mode_usage();
        return ret;
    }
    if ((option != 0) && (option != 1)) {
        g_printf("Set fail:parameter error.\r\n");
        print_set_outport_mode_usage();
        return RET_ERR;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }
    iRet = snprintf_s(obj_name, OBJ_NAME_MAX_LEN, OBJ_NAME_MAX_LEN - 1, "%s", OBJ_NAME_MM_LSW_MGNT);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    input_list = g_slist_append(input_list, g_variant_new_byte((guchar)option));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_NAME_MM_LSW_MGNT,
        obj_name, METHOD_SET_OUT_PORT_MODE, input_list, NULL);
    if (ret != RET_OK) {
        g_printf("Set fail:internal error.\r\n");
        uip_free_gvariant_list(input_list);
        return FALSE;
    }

    g_printf("Set out port successfully.\r\n");
    uip_free_gvariant_list(input_list);
    return TRUE;
}


gint32 smm_sol_set_activate(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target,
    gchar *location)
{
    gint32 ret;
    guint32 blade_number;
    guint32 com_number;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    gchar *usage = "Usage: ipmcset -l <bladeN|swiN> -t sol -d activate -v <com number>\r\n";

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 2) {
        g_printf("%s", usage);
        return FALSE;
    }
    if (strcmp("-v", argv[0]) != 0) {
        g_printf("%s", usage);
        return FALSE;
    }

    blade_number = get_blade_num(location);
    if (blade_number == 0xff) {
        g_printf("%s", usage);
        return FALSE;
    }

    if (strlen(argv[1]) != 1) {
        
        g_printf("Com number is not right\r\n");
        
        return FALSE;
    } else if (argv[1][0] <= '9' && argv[1][0] >= '1') { // Com number is right
        com_number = argv[1][0] - '0';
    } else {
        g_printf("Com number is not right\r\n");
        return FALSE;
    }
    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }
    ret = smm_sol_start_process(ACTIVATE_SOL, blade_number, com_number, username, ip);
    if (ret != RET_OK) {
        g_printf("Connect to sol failed.\r\n");
        return FALSE;
    }

    return TRUE;
}


gint32 smm_sol_set_timeout(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    OBJ_HANDLE handle;
    gint32 ret;
    gint32 timeout;
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    gchar *end_pos = NULL;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 2) {
        g_printf("Usage: ipmcset %s-d soltimeout -v value(range 0~2147483647)\r\n", help_default_name);
        return FALSE;
    }
    if (strcmp("-v", argv[0]) != 0) {
        g_printf("Usage: ipmcset %s-d soltimeout -v value(range 0~2147483647)\r\n", help_default_name);
        return FALSE;
    }
    timeout = strtol(argv[1], &end_pos, 10);
    if (*end_pos != '\0' || timeout < 0 || (timeout == G_MAXINT32 && errno == ERANGE)) {
        g_printf("Value is not right, range 0 ~ 2147483647.\r\n");
        return FALSE;
    }

    ret = dal_get_object_handle_nth(CLASS_NAME_SMM_SOL, 0, &handle);
    if (ret != RET_OK) {
        g_printf("Set timeout failed.\r\n");
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_int32(timeout));
    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, username, ip, CLASS_NAME_SMM_SOL, handle,
        METHOD_SMM_SOL_SET_TIMEOUT, input_list, NULL);
    if (ret != RET_OK) {
        g_printf("Set timeout failed.\r\n");
        return FALSE;
    }

    g_printf("Set timeout ok.\r\n");
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    return TRUE;
}

LOCAL void print_ipmi_data(const guint8 *resp_data, guint32 length)
{
    guint32 i;
    for (i = 0; i < (length); i++) {
        if (i < length - 1) {
            g_printf("%x ", ((const guint8 *)resp_data)[i]);
        } else {
            g_printf("%x]\r\n", ((const guint8 *)resp_data)[i]);
        }
    }
}

LOCAL void print_set_lsw_om_channel_usage(gchar *target)
{
    if (strcmp("smm", target) == 0) {
        g_printf("Usage: ipmcset [-l smm] -d omchannel -v <option> [timeout]\r\n");
    } else {
        g_printf("Usage: ipmcset -t lsw -d omchannel -v <option> [timeout]\r\n");
    }
    g_printf("Options are:\r\n");
    g_printf("    0     Disable\r\n");
    g_printf("    1     Enable OM PXE Install Channel\r\n");
    g_printf("    2     Enable OM Maintenance Channel\r\n");
    g_printf("Optional parameter timeout: 1~24(hour), default 1(hour)\r\n");
    return;
}

LOCAL void print_set_lsw_cmesh_usage(void)
{
    g_printf("Usage: ipmcset -t lsw -d cmeshmode -v <mode>\r\n");
    g_printf("Modes are:\r\n");
    g_printf("    0     Supports BMC communication only\r\n");
    g_printf("    1     VLAN all-pass mode\r\n");
    return;
}

LOCAL gint32 smm_set_om_channel_method(gint32 option, gint32 time_out)
{
    gint32 ret;
    GSList* input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;

    ret = dal_get_object_handle_nth(CLASS_LSW_PORT_ATTR_CENTER, 0, &obj_handle);
    if (ret != RET_OK) {
        g_printf("get object handle fail!\r\n");
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != VOS_OK) {
        ret = snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "N/A");
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "[%s]Username snprintf_s failed, ret(%d)", __FUNCTION__, ret);
        }
        ret = snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "N/A");
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "[%s]Ip snprintf_s failed, ret(%d)", __FUNCTION__, ret);
        }
    }

    input_list = g_slist_append(input_list, g_variant_new_uint32((guint32)option));
    input_list = g_slist_append(input_list, g_variant_new_uint32((guint32)time_out));
    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, username, ip, CLASS_LSW_PORT_ATTR_CENTER,
                                            obj_handle, METHOD_LSW_SET_OM_CHANNEL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != DFL_OK) {
        g_printf("Set omchannel ommode(%d) timeout(%dh) failed!\r\n", option, time_out);
        return FALSE;
    }
    if (option == 0) {
        g_printf("Set omchannel ommode(%d) successfully.\r\n", option);
    } else {
        g_printf("Set omchannel ommode(%d) timeout(%dh) successfully.\r\n", option, time_out);
    }

    return TRUE;
}

gint32 smm_set_om_channel_mode(guchar privilege, const gchar* rolepriv, guchar argc, gchar** argv, gchar* target)
{
    gint32 ret;
    gint32 option = 0;
    gint32 time_out = 1;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    
    if ((argc < 0x2) || (argc > 0x3) || (strcmp("-v", argv[0]) != 0)) {
        print_set_lsw_om_channel_usage(target);
        return FALSE;
    }

    
    ret = vos_str2int(argv[1], STRING_TO_INT_IN_DECIMAL, &option, NUM_TPYE_INT32);
    if ((ret != VOS_OK) || (option < 0) || (option > OM_MODE_MAX_OPTION)) {
        print_set_lsw_om_channel_usage(target);
        return FALSE;
    }

    
    if (argc == 0x3) {
        
        ret = vos_str2int(argv[0x2], STRING_TO_INT_IN_DECIMAL, &time_out, NUM_TPYE_INT32);
        if ((ret != VOS_OK) || (time_out <= 0) || (time_out > OM_MODE_MAX_DURATION)) {
            print_set_lsw_om_channel_usage(target);
            return FALSE;
        }
    }
    return smm_set_om_channel_method(option, time_out);
}

LOCAL gint32 smm_set_cmesh_mode_method(gchar *mode)
{
    gint32 ret;
    GSList* input_list = NULL;
    GSList* output_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;

    ret = dal_get_object_handle_nth(CLASS_LSW_PUBLIC_ATTR_LOCAL_CP, 0, &obj_handle);
    if (ret != RET_OK) {
        g_printf("get object handle fail!\r\n");
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != VOS_OK) {
        ret = snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "N/A");
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "[%s]Username snprintf_s failed, ret(%d)", __FUNCTION__, ret);
        }
        ret = snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "N/A");
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "[%s]Ip snprintf_s failed, ret(%d)", __FUNCTION__, ret);
        }
    }

    input_list = g_slist_append(input_list, g_variant_new_string(PROPERTY_LSW_PUBLIC_CMESH_MODE));
    input_list = g_slist_append(input_list, g_variant_new_string(mode));
    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, username, ip, CLASS_LSW_PUBLIC_ATTR_LOCAL_CP,
                                            obj_handle, METHOD_CP_SET_CMESH_PARA, input_list, &output_list);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != DFL_OK) {
        g_printf("Set cmeshmode mode(%s) failed!\r\n", mode);
        return FALSE;
    }
    ret = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    if (ret == RET_OK) {
        g_printf("Set cmeshmode mode(%s) successfully\r\n", mode);
    } else {
        g_printf("Set cmeshmode mode(%s) failed\r\n", mode);
        return FALSE;
    }

    return TRUE;
}

gint32 smm_set_cmesh_mode(guchar privilege, const gchar* rolepriv, guchar argc, gchar** argv, gchar* target)
{
    gint32 ret;
    gint32 mode = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    
    if ((argc != 0x2) || (strcmp("-v", argv[0]) != 0)) {
        print_set_lsw_cmesh_usage();
        return FALSE;
    }

    
    ret = vos_str2int(argv[1], STRING_TO_INT_IN_DECIMAL, &mode, NUM_TPYE_INT32);
    if ((ret != RET_OK) || (mode < 0) || (mode > CMESH_MODE_MAX_OPTION)) {
        print_set_lsw_cmesh_usage();
        return FALSE;
    }

    return smm_set_cmesh_mode_method(argv[1]);
}

#ifdef ARM64_HI1711_ENABLED
LOCAL gint32 get_slot_id(guint8* slot_id)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;

    ret = dfl_get_object_handle(BMC_BOARD_OBJ_NAME, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s] Failed : dfl_get_object_handle failed: %d.", __FUNCTION__, ret);
        return RET_ERR;
    }

    ret = dal_get_property_value_byte(obj_handle, BMC_SLOT_ID_NAME, slot_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Failed : dal_get_property_value_byte failed: %d.", __FUNCTION__, ret);
        return RET_ERR;
    }

    return RET_OK;
}

LOCAL gint32 get_smm_port_name(const gchar *input_name, guint8 host_slot_id, gchar *port_name, guint32 port_len,
                               guint8 flag)
{
    gint32 ret;
    GSList* handle_list = NULL;
    GSList* list_item = NULL;
    gchar sub_port_name[LSW_PORT_NAME_MAX_LEN] = {0};
    guint8 slot_id = 0;
    gchar *tmp_name = g_ascii_strup(input_name, strlen(input_name));

    if (tmp_name == NULL) {
        debug_log(DLOG_ERROR, "[%s] tmp_name is null", __FUNCTION__);
        return RET_ERR;
    }

    ret = dfl_get_object_list(CLASS_LSW_PORT_ATTR_CENTER, &handle_list);
    if ((ret != DFL_OK) || (handle_list == NULL)) {
        debug_log(DLOG_ERROR, "%sFailed: dfl_get_object_list failed, ret=%d", __FUNCTION__, ret);
        g_free(tmp_name);
        return RET_ERR;
    }

    for (list_item = handle_list; list_item; list_item = g_slist_next(list_item)) {
        ret = dal_get_property_value_byte((OBJ_HANDLE)list_item->data, PROPERTY_LSW_CENTER_SLOTID, &slot_id);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "[%s] Failed : get PROPERTY_LSW_CENTER_SLOTID failed: %d.", __FUNCTION__, ret);
            goto ERROR_INFO;
        }

        if (((flag == 1) && (slot_id != host_slot_id)) || ((flag == 0) && (slot_id == host_slot_id))) {
            continue;
        }

        ret = dal_get_property_value_string((OBJ_HANDLE)list_item->data, PROPERTY_LSW_SUB_PORT_NAME, sub_port_name,
                                            LSW_PORT_NAME_MAX_LEN);
        if (ret != VOS_OK) {
            debug_log(DLOG_ERROR, "[%s] Failed : get PROPERTY_LSW_SUB_PORT_NAME failed: %d.", __FUNCTION__, ret);
            goto ERROR_INFO;
        }

        if (strcmp(tmp_name, sub_port_name) == 0) {
            ret = strcpy_s(port_name, port_len, dfl_get_object_name((OBJ_HANDLE)list_item->data));
            if (ret != EOK) {
                debug_log(DLOG_ERROR, "[%s] Failed : strcpy_s failed: %d.", __FUNCTION__, ret);
                goto ERROR_INFO;
            }
            ret = RET_OK;
            goto ERROR_INFO;
        }
    }

ERROR_INFO:
    g_slist_free(handle_list);
    g_free(tmp_name);
    tmp_name = NULL;
    return ret;
}

LOCAL gint32 smm_set_portstatus_check(gchar** argv, guint32 *port_enablestatus, guint8 smm_slot_id,
                                      gchar portname[LSW_PORT_NAME_MAX_LEN], gchar username[PARAMETER_LEN + 1],
                                      gchar *ip, guint32 ip_len)
{
    gint32 ret;

    ret = get_smm_port_name(argv[0x1], smm_slot_id, portname, LSW_PORT_NAME_MAX_LEN, 1);
    
    if ((strncmp(portname, "MGMT", 4) != 0) && (strncmp(portname, "CAS", 3) != 0)) {
        debug_log(DLOG_ERROR, "[%s] Failed : This port (%s) has no permission", __FUNCTION__, portname);
        g_printf("Usage: ipmcset [-l smm] -t lsw -d portenable -v <portname> <enable|disable>\r\n");
        g_printf("Correct port: MGMT CAS_1 CAS_2\n");
        return FALSE;
    }
    if (ret != RET_OK) {
        g_printf("Usage: ipmcset [-l smm] -t lsw -d portenable -v <portname> <enable|disable>\r\n");
        return FALSE;
    }

    if (strcmp(argv[0x2], "enable") == 0) {
        *port_enablestatus = 1;
    } else if (strcmp(argv[0x2], "disable") == 0) {
        *port_enablestatus = 0;
    } else {
        g_printf("Usage: ipmcset [-l smm] -t lsw -d portenable -v <portname> <enable|disable>\r\n");
        return FALSE;
    }

    if (get_username_ip(username, ip, ip_len) != VOS_OK) {
        ret = snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        if (ret <= RET_OK) {
            debug_log(DLOG_ERROR, "username snprintf_s failed, ret %d", ret);
        }
        ret = snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        if (ret <= RET_OK) {
            debug_log(DLOG_ERROR, "ip snprintf_s failed, ret %d", ret);
        }
    }

    g_printf("This operation will change smm lan switch port enable.");
    ret = check_the_input_operation(CLI_COMFIRM_STRING);
    if (ret != RET_OK) {
        return FALSE;
    }
    return TRUE;
}

gint32 smm_set_portstatus(guchar privilege, const gchar* rolepriv, guchar argc, gchar** argv, gchar* target)
{
    gint32 ret;
    GSList* input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    guint32 port_enablestatus;
    guint8 smm_slot_id = 0;
    gchar portname[LSW_PORT_NAME_MAX_LEN] = {0};

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    
    if (argc != 0x3) {
        g_printf("Usage: ipmcset [-l smm] -t lsw -d portenable -v <portname> <enable|disable>\r\n");
        return FALSE;
    }
    if (strcmp("-v", argv[0]) != 0) {
        g_printf("Usage: ipmcset [-l smm] -t lsw -d portenable -v <portname> <enable|disable>\r\n");
        return FALSE;
    }
    ret = get_slot_id(&smm_slot_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Failed : Get slot_id failed: %d.", __FUNCTION__, ret);
        return FALSE;
    }

    ret = smm_set_portstatus_check(argv, &port_enablestatus, smm_slot_id, portname, username, ip, sizeof(ip));
    if (ret != TRUE) {
        debug_log(DLOG_ERROR, "smm_set_portstatus_check failed, ret %d", ret);
        return FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(portname));
    input_list = g_slist_append(input_list, g_variant_new_uint32(port_enablestatus));

    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar*)username, (const gchar*)ip, CLASS_LSW_PORT_ATTR_CENTER,
                                portname, METHOD_LSW_SET_PORT_ENABLE, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != RET_OK) {
        g_printf("Set port enable failed.\r\n");
        return FALSE;
    }

    g_printf("Success.\r\n");
    return TRUE;
}

LOCAL gint32 smm_set_othersmm_portcfg_check(gchar** argv, gchar portname[LSW_PORT_NAME_MAX_LEN], gint32 *portstatus)
{
    gint32 ret;
    guint8 smm_slot_id = 0;

    ret = get_slot_id(&smm_slot_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Failed : Get slot_id failed: %d.", __FUNCTION__, ret);
        return FALSE;
    }

    ret = get_smm_port_name(argv[0x1], smm_slot_id, portname, LSW_PORT_NAME_MAX_LEN, 0);
    
    if ((strncmp(portname, "MGMT", 4) != 0) && (strncmp(portname, "CAS", 3) != 0)) {
        debug_log(DLOG_ERROR, "[%s] Failed : This port (%s) has no permission", __FUNCTION__, portname);
        g_printf("Usage: ipmcset [-l smm] -t lsw -d othersmmportenable -v <portname> <enable|disable>\r\n");
        g_printf("Correct port: MGMT CAS_1 CAS_2\r\n");
        return FALSE;
    }
    if (ret != RET_OK) {
        g_printf("Usage: ipmcset [-l smm] -t lsw -d othersmmportenable -v <portname> <enable|disable>\r\n");
        return FALSE;
    }

    if (strcmp(argv[0x2], "enable") == 0) {
        *portstatus = 1;
    } else if (strcmp(argv[0x2], "disable") == 0) {
        *portstatus = 0;
    } else {
        g_printf("Usage: ipmcset [-l smm] -t lsw -d othersmmportenable -v <portname> <enable|disable>\r\n");
        return FALSE;
    }

    g_printf("This operation will change other smm lan switch port enable.");
    ret = check_the_input_operation(CLI_COMFIRM_STRING);
    if (ret != RET_OK) {
        return FALSE;
    }

    return TRUE;
}

gint32 smm_set_othersmm_portstatus(guchar privilege, const gchar* rolepriv, guchar argc, gchar** argv, gchar* target)
{
    gint32 ret;
    gint32 portstatus = 0;
    GSList* input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    gchar portname[LSW_PORT_NAME_MAX_LEN] = {0};

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    
    if (argc != 0x3) {
        g_printf("Usage: ipmcset [-l smm] -t lsw -d othersmmportenable -v <portname> <enable|disable>\r\n");
        return FALSE;
    }
    if (strcmp("-v", argv[0]) != 0) {
        g_printf("Usage: ipmcset [-l smm] -t lsw -d othersmmportenable -v <portname> <enable|disable>\r\n");
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != VOS_OK) {
        ret = snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        if (ret <= RET_OK) {
            debug_log(DLOG_ERROR, "username snprintf_s failed, ret %d", ret);
        }
        ret = snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        if (ret <= RET_OK) {
            debug_log(DLOG_ERROR, "ip snprintf_s failed, ret %d", ret);
        }
    }

    ret = smm_set_othersmm_portcfg_check(argv, portname, &portstatus);
    if (ret != TRUE) {
        debug_log(DLOG_ERROR, "[%s] smm_set_othersmm_portcfg_check Failed ret=%d.", __FUNCTION__, ret);
        return FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(portname));
    input_list = g_slist_append(input_list, g_variant_new_uint32(portstatus));

    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar*)username, (const gchar*)ip, CLASS_LSW_PORT_ATTR_CENTER,
                                portname, METHOD_LSW_SET_PORT_ENABLE, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != RET_OK) {
        g_printf("Set port enable failed.\r\n");
        return FALSE;
    }

    g_printf("Success.\r\n");
    return TRUE;
}

#endif
