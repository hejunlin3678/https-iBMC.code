

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
#include "ipmcset_sys_mgmt.h"
#include "ipmcset_alarm.h"
#include "ipmcset_service_mgmt.h"
#include "ipmcset_user_security.h"

#define NO_ARGUMENT 0       // 不需要参数
#define REQUIRED_ARGUMENT 1 // 必须指定参数
#define OPTIONAL_ARGUMENT 2 // 参数可选

#define ETH_SET_ERROR (-5)

#define NOT_SUPPORT_SELFHEALIN 0xff
#define IDLE_STATE 0
#define FPGA_IS_SELFHEALING 1
#define STATE_NOT_SUPPORT_SELFHEALIN 2
#define SELFHEALIN_FAILED 3
#define FPGA_SELFHEALING_COMPLETED 100
#define MAX_SLOT_COUNT 32


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

#define SET_USER_ADD "Add User"
#define SET_USER_DEL "Del User"

typedef struct {
    gchar  ld_name[SML_LD_NAME_LENGTH];
    guint32 array_id;
    guint32 size;
    guint32 pd_count;
    guint16 associated_ld;
    guint8 size_unit;
    guint8 strip_size;
    guint8 read_policy;
    guint8 write_policy;
    guint8 io_policy;
    guint8 access_policy;
    guint8 disk_cache_policy;
    guint8 init_type;
    guint8 block_index;
    guint8 raid_level;
    guint8 span_depth;
    guint8 accelerator;
    guint8 cache_line_size;
    guint8 bgi_state;
    guint8 bootable;
    guint8 sscd_caching_state;
    guint8 *pd_list;
} CLI_LD_INFO;

typedef struct {
    gint32 is_array_id;
    gint32 is_raid_level;
    gint32 is_pd_list;
    gint32 is_cachecade;
    gint32 is_span_depth;
    gint32 is_name;
    gint32 is_size;
    gint32 is_strip_size;
    gint32 is_read_policy;
    gint32 is_write_policy;
    gint32 is_io_policy;
    gint32 is_access_policy;
    gint32 is_disk_cache_policy;
    gint32 is_init_type;
    gint32 is_accelerator;
    gint32 is_associated_ld;
    gint32 is_cache_line_size;
    gint32 is_block_flag;
    gint32 is_bgi_state;
    gint32 is_bootable;
    gint32 is_sscd_caching;
} CLI_LD_FLAGS;



gchar* g_ledstate[] = {
    "off",
    "on",
    "blink",
    "lamptest",
    "localcontrol",
    NULL
};

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

#define DEDICATE_PORT_DISPLAY_INFO "Dedicated network port, without parameter [portid]"
#define PCIE_MGNT_DEDICATE_PORT_DISPLAY_INFO "Dedicated network port"
#define LOM_PORT_DISPLAY_INFO "LOM network port"
#define PCIE_PORT_DISPLAY_INFO "PCIE network port"
#define AGGREGATION_PORT_DISPLAY_INFO "Aggregation network port, without parameter [portid]"
#define LOM2_PORT_DISPLAY_INFO "FlexIO network port"
#define OCP_PORT_DISPLAY_INFO "OCP network port"
#define OCP2_PORT_DISPLAY_INFO "OCP2 network port"

guchar g_cur_upgrade_comp = 0x01;


gint32 ipmc_set_ipaddr_v6(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    errno_t safe_fun_ret;
    struct in6_addr v6ipaddr;
    struct in6_addr gateway;
    guint8 group_id = 0;
    guchar is_same = 0;
    gint32 ret;
    gchar buff[INET6_ADDRSTRLEN] = {0};
    gchar *p = NULL;
    guint32 prefixlen;
    OBJ_HANDLE obj = 0;

    memset_s((gchar *)&v6ipaddr, sizeof(v6ipaddr), 0, sizeof(v6ipaddr));
    memset_s(gateway.s6_addr, sizeof(struct in6_addr), 0x00, sizeof(struct in6_addr));

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (((argc != 2) && (argc != 3)) || strcmp(argv[0], "-v")) {
        g_printf("Usage: ipmcset %s-d ipaddr6 -v <ipaddr6/prefixlen(0~128)> [gateway6]\r\n", help_default_name);
        return FALSE;
    }

    memset_s(v6ipaddr.s6_addr, sizeof(struct in6_addr), 0x00, sizeof(struct in6_addr));
    p = strchr(argv[1], '/');
    if (p == NULL) {
        g_printf("Usage: ipmcset %s-d ipaddr6 -v <ipaddr6/prefixlen(0~128)> [gateway6]\r\n", help_default_name);
        return FALSE;
    }

    memset_s(buff, sizeof(buff), 0x00, sizeof(buff));
    safe_fun_ret = memcpy_s(buff, INET6_ADDRSTRLEN - 1, argv[1], strlen(argv[1]) - strlen(p));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        return FALSE;
    }

    ret = inet_pton(AF_INET6, buff, (void *)v6ipaddr.s6_addr);
    if (ret != 1) {
        g_printf("Input parameter format error.\r\n");
        return FALSE;
    }

    p++;

    if (str_to_int(p) == IPMC_ERROR) {
        g_printf("Usage: ipmcset %s-d ipaddr6 -v <ipaddr6/prefixlen(0~128)> [gateway6]\r\n", help_default_name);
        return FALSE;
    }

    prefixlen = (guint32)str_to_int(p);
    if (prefixlen > 128) {
        g_printf("Usage: ipmcset %s-d ipaddr6 -v <ipaddr6/prefixlen(0~128)> [gateway6]\r\n", help_default_name);
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
            g_printf("IPv6 address or default gateway is invalid.\r\n");
            return FALSE;
        }

        
        ret = ipmc_eth_judge_ipv6_ip_gateway(buff, argv[2], prefixlen, &is_same);
        if ((ret != 0) || (is_same == 0)) {
            g_printf("Failed to verify the IPv6 address and default gateway network segment.\n");
            return FALSE;
        }
    }

    
    ret = dal_eth_get_out_type_object(OUT_ETHERNET, &obj);
    if (ret == RET_OK) {
        (void)dal_get_property_value_byte(obj, ETH_GROUP_ATTRIBUTE_GROUP_ID, &group_id);
    }

    
    if (!set_ipv6_address(group_id, buff)) {
        return FALSE;
    }

    
    if (!set_ipv6_prefix(group_id, (guint8)prefixlen)) {
        return FALSE;
    }

    
    if (argc == 3) {
        if (!set_ipv6_gateway(group_id, argv[2])) {
            return FALSE;
        }
    }

    

    return TRUE;
}


gint32 ipmc_set_maintenance_raidcom(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    gint32 com_channel_option = 0xff;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    gchar controller_name[PROP_VAL_LENGTH] = {0};

    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    if ((argc != 2) || (0 != strcmp(argv[0], "-v"))) {
        print_maintenance_raidcom_usage();
        return FALSE;
    }
    if ((com_channel_option = str_to_int(argv[1])) == IPMC_ERROR) {
        print_maintenance_raidcom_usage();
        return FALSE;
    }
    if (com_channel_option >= 0xff || com_channel_option <= 0) {
        print_maintenance_raidcom_usage();
        return FALSE;
    }

    ret = dal_get_specific_object_byte(CLASS_RAID_CONTROLLER_NAME, PROPERTY_RAID_CONTROLLER_COM_CHANNEL,
        (guint8)com_channel_option, &obj_handle);
    if (ret != RET_OK) {
        print_maintenance_raidcom_usage();
        return FALSE;
    }

    (void)dal_get_property_value_string(obj_handle, PROPERTY_RAID_CONTROLLER_COMPONENT_NAME, controller_name,
        sizeof(controller_name));

    ret = dal_get_specific_object_byte(SOL_CLASS_NAME, PROPERTY_SOL_COM_ID, SERIAL_SELECTOR_RAID, &obj_handle);
    if (ret != RET_OK) {
        g_printf("Get RAID com info failed.\r\n");
        return FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(com_channel_option));

    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, username, ip, SOL_CLASS_NAME, obj_handle,
        METHOD_SWITCH_COM_CHANNEL, input_list, NULL);

    uip_free_gvariant_list(input_list);

    if (ret != RET_OK) {
        g_printf("Set RAID com channel to %s failed.\r\n", controller_name);
        return FALSE;
    }

    g_printf("Set RAID com channel to %s successfully.\r\n", controller_name);
    return TRUE;
}


LOCAL void set_logical_drive_name(OBJ_HANDLE ld_obj, const gchar *ld_name, const gchar *username, const gchar *ip)
{
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    const gchar *error_message = NULL;

    if (ld_name == NULL || username == NULL || ip == NULL) {
        return;
    }

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_string(ld_name));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_LOGICAL_DRIVE_NAME,
        dfl_get_object_name(ld_obj), METHOD_LOGICAL_DRIVE_SET_NAME, input_list, &output_list);
    if (ret == RET_OK) {
        g_printf("Set logical drive name successfully.\r\n");
    } else {
        if (output_list != NULL) {
            error_message = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
            g_printf("Set logical drive name failed. Error code : 0x%04X (%s).\r\n", ret,
                error_message != NULL ? error_message : "No details");
        } else {
            g_printf("Set logical drive name failed. Error code : 0x%04X.\r\n", ret);
        }
    }

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    return;
}


LOCAL void set_logical_drive_read_policy(OBJ_HANDLE ld_obj, guint8 ld_rp, gchar *username, gchar *ip)
{
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    const gchar *error_message = NULL;

    if (username == NULL || ip == NULL) {
        return;
    }

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(ld_rp));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_LOGICAL_DRIVE_NAME,
        dfl_get_object_name(ld_obj), METHOD_LOGICAL_DRIVE_SET_READ_POLICY, input_list, &output_list);
    if (ret == RET_OK) {
        g_printf("Set logical drive read policy successfully.\r\n");
    } else {
        if (output_list != NULL) {
            error_message = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
            g_printf("Set logical drive read policy failed. Error code : 0x%04X (%s).\r\n", ret,
                error_message != NULL ? error_message : "No details");
        } else {
            g_printf("Set logical drive read policy failed. Error code : 0x%04X.\r\n", ret);
        }
    }

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    return;
}


LOCAL void set_logical_drive_write_policy(OBJ_HANDLE ld_obj, guint8 ld_wp, gchar *username, gchar *ip)
{
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    const gchar *error_message = NULL;

    if (username == NULL || ip == NULL) {
        return;
    }

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(ld_wp));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_LOGICAL_DRIVE_NAME,
        dfl_get_object_name(ld_obj), METHOD_LOGICAL_DRIVE_SET_WRITE_POLICY, input_list, &output_list);
    if (ret == RET_OK) {
        g_printf("Set logical drive write policy successfully.\r\n");
    } else {
        if (output_list != NULL) {
            error_message = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
            g_printf("Set logical drive write policy failed. Error code : 0x%04X (%s).\r\n", ret,
                error_message != NULL ? error_message : "No details");
        } else {
            g_printf("Set logical drive write policy failed. Error code : 0x%04X.\r\n", ret);
        }
    }

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    return;
}


LOCAL void set_logical_drive_io_policy(OBJ_HANDLE ld_obj, guint8 ld_iop, gchar *username, gchar *ip)
{
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    const gchar *error_message = NULL;

    if (username == NULL || ip == NULL) {
        return;
    }

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(ld_iop));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_LOGICAL_DRIVE_NAME,
        dfl_get_object_name(ld_obj), METHOD_LOGICAL_DRIVE_SET_IO_POLICY, input_list, &output_list);
    if (ret == RET_OK) {
        g_printf("Set logical drive IO policy successfully.\r\n");
    } else {
        if (output_list != NULL) {
            error_message = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
            g_printf("Set logical drive IO policy failed. Error code : 0x%04X (%s).\r\n", ret,
                error_message != NULL ? error_message : "No details");
        } else {
            g_printf("Set logical drive IO policy failed. Error code : 0x%04X.\r\n", ret);
        }
    }

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    return;
}


LOCAL void set_logical_drive_access_policy(OBJ_HANDLE ld_obj, guint8 ld_ap, gchar *username, gchar *ip)
{
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    const gchar *error_message = NULL;

    if (username == NULL || ip == NULL) {
        return;
    }

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(ld_ap));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_LOGICAL_DRIVE_NAME,
        dfl_get_object_name(ld_obj), METHOD_LOGICAL_DRIVE_SET_ACCESS_POLICY, input_list, &output_list);
    if (ret == RET_OK) {
        g_printf("Set logical drive access policy successfully.\r\n");
    } else if (ret == SML_ERR_REBOOT_REQUIRED) {
        g_printf("Set logical drive access policy successfully, please reboot the OS to take effect.\r\n");
    } else {
        if (output_list != NULL) {
            error_message = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
            g_printf("Set logical drive access policy failed. Error code : 0x%04X (%s).\r\n", ret,
                error_message != NULL ? error_message : "No details");
        } else {
            g_printf("Set logical drive access policy failed. Error code : 0x%04X.\r\n", ret);
        }
    }

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    return;
}


LOCAL void set_logical_drive_disk_cache_policy(OBJ_HANDLE ld_obj, guint8 ld_dcp, gchar *username, gchar *ip)
{
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    const gchar *error_message = NULL;

    if (username == NULL || ip == NULL) {
        return;
    }

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(ld_dcp));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_LOGICAL_DRIVE_NAME,
        dfl_get_object_name(ld_obj), METHOD_LOGICAL_DRIVE_SET_DISK_CACHE_POLICY, input_list, &output_list);
    if (ret == RET_OK) {
        g_printf("Set logical drive disk cache policy successfully.\r\n");
    } else {
        if (output_list != NULL) {
            error_message = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
            g_printf("Set logical drive disk cache policy failed. Error code : 0x%04X (%s).\r\n", ret,
                error_message != NULL ? error_message : "No details");
        } else {
            g_printf("Set logical drive disk cache policy failed. Error code : 0x%04X.\r\n", ret);
        }
    }

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    return;
}


LOCAL void set_logical_drive_bgi_state(OBJ_HANDLE ld_obj, guint8 ld_bgi, gchar *username, gchar *ip)
{
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    const gchar *error_message = NULL;

    if (username == NULL || ip == NULL) {
        return;
    }

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(ld_bgi));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_LOGICAL_DRIVE_NAME,
        dfl_get_object_name(ld_obj), METHOD_LOGICAL_DRIVE_SET_BGI, input_list, &output_list);
    if (ret == RET_OK) {
        g_printf("Set logical drive BGI state successfully\r\n");
    } else {
        if (output_list != NULL) {
            error_message = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
            g_printf("Set logical drive BGI state failed. Error code : 0x%04X (%s).\r\n", ret,
                error_message != NULL ? error_message : "No details");
        } else {
            g_printf("Set logical drive BGI state failed. Error code : 0x%04X.\r\n", ret);
        }
    }

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    return;
}


LOCAL void set_logical_drive_bootable(OBJ_HANDLE ld_obj, guint8 boot_priority, gchar *username, gchar *ip)
{
    gint32 ret;
    GSList *output_list = NULL;
    const gchar *error_message = NULL;
    GSList *input_list = NULL;

    if (username == NULL || ip == NULL) {
        return;
    }

    
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(boot_priority));

    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_LOGICAL_DRIVE_NAME,
        dfl_get_object_name(ld_obj), METHOD_LOGICAL_DRIVE_SET_BOOTABLE, input_list, &output_list);
    if (ret == RET_OK) {
        g_printf("Set logical drive as boot device successfully.\r\n");
    } else {
        if (output_list != NULL) {
            error_message = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
            g_printf("Set logical drive as boot device failed. Error code : 0x%04X (%s).\r\n", ret,
                error_message != NULL ? error_message : "No details");
        } else {
            g_printf("Set logical drive as boot device failed. Error code : 0x%04X.\r\n", ret);
        }
    }

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    return;
}


LOCAL void set_logical_drive_sscd_caching_state(OBJ_HANDLE ld_obj, guint8 ld_sscd_caching, gchar *username, gchar *ip)
{
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    const gchar *error_message = NULL;

    if (username == NULL || ip == NULL) {
        return;
    }

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(ld_sscd_caching));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_LOGICAL_DRIVE_NAME,
        dfl_get_object_name(ld_obj), METHOD_LOGICAL_DRIVE_SET_CACHECADE, input_list, &output_list);
    if (ret == RET_OK) {
        g_printf("Set logical drive SSCD caching state successfully.\r\n");
    } else {
        if (output_list != NULL) {
            error_message = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
            g_printf("Set logical drive SSCD caching state failed. Error code : 0x%04X (%s).\r\n", ret,
                (error_message != NULL) ? error_message : "No details");
        } else {
            g_printf("Set logical drive SSCD caching state failed. Error code : 0x%04X.\r\n", ret);
        }
    }

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);
}

LOCAL void set_logical_drive_acceletor(OBJ_HANDLE ld_obj, guint8 ld_accelerator, gchar *username, gchar *ip)
{
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    const gchar *error_message = NULL;

    if (username == NULL || ip == NULL) {
        return;
    }

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(ld_accelerator));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_LOGICAL_DRIVE_NAME,
        dfl_get_object_name(ld_obj), METHOD_LOGICAL_DRIVE_SET_ACCELERATION_METHOD, input_list, &output_list);
    if (ret == RET_OK) {
        g_printf("Set logical drive acceleration method successfully.\r\n");
    } else {
        if (output_list != NULL) {
            error_message = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
            g_printf("Set logical drive acceleration method failed. Error code : 0x%04X (%s).\r\n", ret,
                (error_message != NULL) ? error_message : "No details");
        } else {
            g_printf("Set logical drive acceleration method failed. Error code : 0x%04X.\r\n", ret);
        }
    }

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);
}

LOCAL void set_logical_drive_capacity(OBJ_HANDLE ld_obj, guint32 capacity, guint8 capacity_unit, gchar *username,
    gchar *ip)
{
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    const gchar *error_message = NULL;

    if (username == NULL || ip == NULL) {
        return;
    }

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_uint32(capacity));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(capacity_unit));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_LOGICAL_DRIVE_NAME,
        dfl_get_object_name(ld_obj), METHOD_LOGICAL_DRIVE_SET_CAPACITY, input_list, &output_list);
    if (ret == RET_OK) {
        g_printf("Set logical drive capacity successfully.\r\n");
    } else {
        if (output_list != NULL) {
            error_message = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
            g_printf("Set logical drive capacity failed. Error code : 0x%04X (%s).\r\n", ret,
                (error_message != NULL) ? error_message : "No details");
        } else {
            g_printf("Set logical drive capacity failed. Error code : 0x%04X.\r\n", ret);
        }
    }

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);
}

LOCAL void set_logical_drive_strip_size(OBJ_HANDLE ld_obj, guint8 strip_size, gchar *username, gchar *ip)
{
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    const gchar *error_message = NULL;

    if (username == NULL || ip == NULL) {
        return;
    }

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(strip_size));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_LOGICAL_DRIVE_NAME,
        dfl_get_object_name(ld_obj), METHOD_LOGICAL_DRIVE_SET_STRIP_SIZE, input_list, &output_list);
    if (ret == RET_OK) {
        g_printf("Set logical drive strip size successfully.\r\n");
    } else {
        if (output_list != NULL) {
            error_message = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
            g_printf("Set logical drive strip size failed. Error code : 0x%04X (%s).\r\n", ret,
                error_message != NULL ? error_message : "No details");
        } else {
            g_printf("Set logical drive strip size failed. Error code : 0x%04X.\r\n", ret);
        }
    }

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);
}



LOCAL gint32 check_ld_name_is_valid(gchar *ld_name, guint8 check_set)
{
    guint32 idx = 0;
    guint32 name_length;

    if (ld_name == NULL) {
        return RET_ERR;
    }

    name_length = strlen(ld_name);
    while (idx < MIN(2, name_length) && ld_name[idx] == '-') { // 选项名称的前面有一个或两个'-'
        idx++;
    }

    if (idx == 0 || idx == name_length) {
        return RET_OK;
    }

    switch (check_set) {
        case 0:
            if ((strcmp(&ld_name[idx], UIP_LD_OPTION_RAID_LEVEL) == 0 ||
                strcmp(&ld_name[idx], UIP_LD_OPTION_PD_LIST) == 0 || strcmp(&ld_name[idx], UIP_LD_OPTION_SIZE) == 0 ||
                strcmp(&ld_name[idx], UIP_LD_OPTION_STRIP_SIZE) == 0 ||
                strcmp(&ld_name[idx], UIP_LD_OPTION_SPAN_DEPTH) == 0 ||
                strcmp(&ld_name[idx], UIP_LD_OPTION_CACHECADE_FLAG) == 0 ||
                strcmp(&ld_name[idx], UIP_LD_OPTION_INIT_TYPE) == 0)) {
                return RET_ERR;
            }
            break;

        case 1:
            if (strcmp(&ld_name[idx], UIP_LD_OPTION_ARRAY_ID) == 0 || strcmp(&ld_name[idx], UIP_LD_OPTION_SIZE) == 0 ||
                strcmp(&ld_name[idx], UIP_LD_OPTION_STRIP_SIZE) == 0 ||
                strcmp(&ld_name[idx], UIP_LD_OPTION_INIT_TYPE) == 0) {
                return RET_ERR;
            }
            break;

        case 2:
            if (strcmp(&ld_name[idx], UIP_LD_OPTION_BGI_STATE) == 0 ||
                strcmp(&ld_name[idx], UIP_LD_OPTION_CACHECADE_STATE) == 0 ||
                strcmp(&ld_name[idx], UIP_LD_OPTION_BOOTABLE_FLAG) == 0) {
                return RET_ERR;
            }
            break;

        default:
            break;
    }

    if ((strcmp(&ld_name[idx], UIP_LD_OPTION_NAME) == 0 || strcmp(&ld_name[idx], UIP_LD_OPTION_READ_POLICY) == 0 ||
        strcmp(&ld_name[idx], UIP_LD_OPTION_WRITE_POLICY) == 0 || strcmp(&ld_name[idx], UIP_LD_OPTION_IO_POLICY) == 0 ||
        strcmp(&ld_name[idx], UIP_LD_OPTION_ACCESS_POLICY) == 0 ||
        strcmp(&ld_name[idx], UIP_LD_OPTION_DISK_CACHE_POLICY) == 0)) {
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 check_mandatory_params_for_cachecade(guint8 type_id, CLI_LD_FLAGS *ld_flags)
{
    if (ld_flags->is_cachecade == 0) {
        return RET_OK;
    }

    if (dal_test_controller_vendor(type_id, VENDER_PMC)) {
        if (ld_flags->is_associated_ld == 0) {
            g_printf("Error : missing option '-associatedld'.\r\n");
            return RET_ERR;
        }

        if (ld_flags->is_write_policy == 0) {
            g_printf("Error : missing option '-wp'.\r\n");
            return RET_ERR;
        }
    } else {
        // 博通卡没特殊要求
    }

    return RET_OK;
}

LOCAL gint32 check_mandatory_params_for_create_ld(guint8 type_id, CLI_LD_FLAGS *ld_flags)
{
    if (ld_flags->is_raid_level == 0) {
        g_printf("Error : missing option '-rl'.\r\n");
        return RET_ERR;
    }

    if (ld_flags->is_pd_list == 0) {
        g_printf("Error : missing option '-pd'.\r\n");
        return RET_ERR;
    }

    if (check_mandatory_params_for_cachecade(type_id, ld_flags) != RET_OK) {
        return RET_ERR;
    }

    return RET_OK;
}

LOCAL gint32 check_mandatory_params_for_add_ld(guint8 type_id, CLI_LD_FLAGS *ld_flags)
{
    if (ld_flags->is_array_id == 0) {
        g_printf("Error : missing option '-array'.\r\n\n");
        return RET_ERR;
    }

    if (dal_test_controller_vendor(type_id, VENDER_PMC)) {
        if (ld_flags->is_raid_level == 0) {
            g_printf("Error : missing option '-rl'.\r\n\n");
            return RET_ERR;
        }
    }

    if (check_mandatory_params_for_cachecade(type_id, ld_flags) != RET_OK) {
        return RET_ERR;
    }

    return RET_OK;
}

LOCAL void free_pd_list(guint8** pd_list)
{
    if (pd_list != NULL && *pd_list != NULL) {
        g_free(*pd_list);
        *pd_list = NULL;
    }
}

LOCAL void init_ld_info(CLI_LD_INFO *ld_info)
{
    (void)memset_s(ld_info, sizeof(CLI_LD_INFO), 0, sizeof(CLI_LD_INFO));

    ld_info->array_id = 0xFFFF;
    ld_info->size = 0xFFFFFFFF;
    ld_info->associated_ld = 0xFFFF;
    ld_info->size_unit = 0xFF;
    ld_info->strip_size = 0xFF;
    ld_info->read_policy = 0xFF;
    ld_info->write_policy = 0xFF;
    ld_info->io_policy =  0xFF;
    ld_info->access_policy = 0xFF;
    ld_info->disk_cache_policy = 0xFF;
    ld_info->init_type = 0xFF;
    ld_info->block_index = 0xFF;
    ld_info->raid_level = 0xFF;
    ld_info->span_depth = 0xFF;
    ld_info->accelerator = 0xFF;
    ld_info->cache_line_size = 0xFF;
    return;
}

LOCAL gboolean is_too_many_options_for_cachecade_ld(guint8 type_id, CLI_LD_FLAGS *ld_flags)
{
    gboolean flag;

    if (dal_test_controller_vendor(type_id, VENDER_PMC)) {
        flag = (ld_flags->is_span_depth || ld_flags->is_name || ld_flags->is_strip_size ||
            ld_flags->is_read_policy || ld_flags->is_io_policy || ld_flags->is_access_policy ||
            ld_flags->is_disk_cache_policy || ld_flags->is_init_type || ld_flags->is_accelerator ||
            ld_flags->is_block_flag);

        return flag;
    }

    flag = (ld_flags->is_array_id || ld_flags->is_span_depth || ld_flags->is_size ||
        ld_flags->is_strip_size || ld_flags->is_read_policy || ld_flags->is_io_policy ||
        ld_flags->is_access_policy || ld_flags->is_disk_cache_policy || ld_flags->is_init_type ||
        ld_flags->is_accelerator || ld_flags->is_associated_ld || ld_flags->is_cache_line_size);

    return flag;
}

LOCAL gboolean is_too_many_options_for_common_ld(guint8 type_id, CLI_LD_FLAGS *ld_flags)
{
    gboolean flag;

    if (dal_test_controller_vendor(type_id, VENDER_PMC)) {
        flag = (ld_flags->is_read_policy || ld_flags->is_write_policy || ld_flags->is_io_policy ||
            ld_flags->is_access_policy || ld_flags->is_disk_cache_policy ||
            ld_flags->is_associated_ld || ld_flags->is_cache_line_size ||
            ld_flags->is_block_flag);

        return flag;
    }

    flag = ((ld_flags->is_array_id && (ld_flags->is_raid_level || ld_flags->is_span_depth)) ||
        ld_flags->is_accelerator || ld_flags->is_associated_ld || ld_flags->is_cache_line_size);

    return flag;
}

LOCAL gint32 call_create_common_ld_method(OBJ_HANDLE obj_handle, const gchar *username, const gchar *ip,
    CLI_LD_INFO *ld_info, GSList **output_list)
{
    GSList *input_list = NULL;

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE,
        ld_info->pd_list, ld_info->pd_count, sizeof(guint8)));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(ld_info->raid_level));

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(ld_info->span_depth));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_string(ld_info->ld_name));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_uint32(ld_info->size));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(ld_info->size_unit));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(ld_info->strip_size));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(ld_info->read_policy));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(ld_info->write_policy));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(ld_info->io_policy));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(ld_info->access_policy));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(ld_info->disk_cache_policy));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(ld_info->init_type));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(ld_info->accelerator));

    gint32 ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
        CLASS_RAID_CONTROLLER_NAME, dfl_get_object_name(obj_handle), METHOD_RAID_CONTROLLER_CREATE_COMMON_LD,
        input_list, output_list);
    uip_free_gvariant_list(input_list);
    return ret;
}

LOCAL gint32 call_create_cachecade_ld_method(OBJ_HANDLE obj_handle, const gchar *username, const gchar *ip,
    CLI_LD_INFO *ld_info, GSList **output_list)
{
    GSList *input_list = NULL;

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE,
        ld_info->pd_list, ld_info->pd_count, sizeof(guint8)));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(ld_info->raid_level));

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_string(ld_info->ld_name));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(ld_info->write_policy));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_uint16(ld_info->array_id));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_uint32(ld_info->size));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(ld_info->size_unit));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_uint16(ld_info->associated_ld));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(ld_info->cache_line_size));

    gint32 ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
        CLASS_RAID_CONTROLLER_NAME, dfl_get_object_name(obj_handle), METHOD_RAID_CONTROLLER_CREATE_CACHECADE_LD,
        input_list, output_list);
    uip_free_gvariant_list(input_list);
    return ret;
}

LOCAL gint32 call_add_ld_method(OBJ_HANDLE obj_handle, const gchar *username, const gchar *ip,
    CLI_LD_INFO *ld_info, GSList **output_list)
{
    GSList *input_list = NULL;

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_uint16(ld_info->array_id));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(ld_info->block_index));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(ld_info->raid_level));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(ld_info->span_depth));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_string(ld_info->ld_name));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_uint32(ld_info->size));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(ld_info->size_unit));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(ld_info->strip_size));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(ld_info->read_policy));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(ld_info->write_policy));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(ld_info->io_policy));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(ld_info->access_policy));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(ld_info->disk_cache_policy));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(ld_info->init_type));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(ld_info->accelerator));

    gint32 ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar*)username, (const gchar*)ip,
        CLASS_RAID_CONTROLLER_NAME, dfl_get_object_name(obj_handle),
        METHOD_RAID_CONTROLLER_ADD_LD, input_list, output_list);

    uip_free_gvariant_list(input_list);
    return ret;
}

gint32 ipmc_create_logical_drive(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 retval = TRUE;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;
    GSList *output_list = NULL;
    CLI_LD_INFO ld_info;
    CLI_LD_FLAGS ld_flags = {0};

    guint16 ld_id = 0xFFFF;
    const gchar *error_message = NULL;
    const gchar *optstring = ":";
    gint c = 0, idx = 0;

    struct option opts[] = {
        {UIP_LD_OPTION_RAID_LEVEL, REQUIRED_ARGUMENT, &ld_flags.is_raid_level, 1},
        {UIP_LD_OPTION_PD_LIST, REQUIRED_ARGUMENT, &ld_flags.is_pd_list, 1},
        {UIP_LD_OPTION_CACHECADE_FLAG, NO_ARGUMENT, &ld_flags.is_cachecade, 1},
        {UIP_LD_OPTION_SPAN_DEPTH, REQUIRED_ARGUMENT, &ld_flags.is_span_depth, 1},
        {UIP_LD_OPTION_NAME, REQUIRED_ARGUMENT, &ld_flags.is_name, 1},
        {UIP_LD_OPTION_SIZE, REQUIRED_ARGUMENT, &ld_flags.is_size, 1},
        {UIP_LD_OPTION_STRIP_SIZE, REQUIRED_ARGUMENT, &ld_flags.is_strip_size, 1},
        {UIP_LD_OPTION_READ_POLICY, REQUIRED_ARGUMENT, &ld_flags.is_read_policy, 1},
        {UIP_LD_OPTION_WRITE_POLICY, REQUIRED_ARGUMENT, &ld_flags.is_write_policy, 1},
        {UIP_LD_OPTION_IO_POLICY, REQUIRED_ARGUMENT, &ld_flags.is_io_policy, 1},
        {UIP_LD_OPTION_ACCESS_POLICY, REQUIRED_ARGUMENT, &ld_flags.is_access_policy, 1},
        {UIP_LD_OPTION_DISK_CACHE_POLICY, REQUIRED_ARGUMENT, &ld_flags.is_disk_cache_policy, 1},
        {UIP_LD_OPTION_INIT_TYPE, REQUIRED_ARGUMENT, &ld_flags.is_init_type, 1},
        {UIP_LD_OPTION_ACCELERATION_METHOD, REQUIRED_ARGUMENT, &ld_flags.is_accelerator, 1},
        {UIP_LD_OPTION_ASSOCIATED_LD, REQUIRED_ARGUMENT, &ld_flags.is_associated_ld, 1},
        {UIP_LD_OPTION_CACHE_LINE_SIZE, REQUIRED_ARGUMENT, &ld_flags.is_cache_line_size, 1},
        {NULL, NO_ARGUMENT, NULL, 0}
    };

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    
    if (argc < 2 || g_strcmp0(argv[0], "-v") != 0) {   // 保证argv[1]有值，参数个数不能小于2
        g_printf("Error : The first two paramters must be '-v <RAID Controller ID>'.\r\n");
        return FALSE;
    }

    gint32 ctrl_id = str_to_int(argv[1]);
    if (ctrl_id < 0) {
        g_printf("Invalid RAID controller ID.\r\n");
        return FALSE;
    }

    gint32 ret = dal_get_specific_object_byte(CLASS_RAID_CONTROLLER_NAME, PROPERTY_RAID_CONTROLLER_ID, (guint8)ctrl_id,
        &obj_handle);
    if (ret != RET_OK) {
        g_printf("Invalid RAID controller ID.\r\n");
        return FALSE;
    }

    guint8 type_id = 0;
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_RAID_CONTROLLER_TYPE_ID, &type_id);

    optind = 1;
    opterr = 0;

    init_ld_info(&ld_info);
    while ((c = getopt_long_only(argc, argv, optstring, opts, &idx)) != -1) {
        switch (c) {
            case ':':
                g_printf("Error : option '%s' - requires an argument.\r\n", argv[optind - 1]);
                free_pd_list(&ld_info.pd_list);
                print_create_ld_help_info(type_id);
                return FALSE;

            case '?':
                g_printf("Error : invalid option '%s'.\r\n", argv[optind - 1]);
                free_pd_list(&ld_info.pd_list);
                print_create_ld_help_info(type_id);
                return FALSE;

            default:
                break;
        }

        
        if (idx != 2 && idx != 4) {  //  2-是否cachecade，4-名称
            if (optarg == NULL || *optarg == '-') {
                goto EXIT1;
            }
        }

        switch (idx) {
            case 0:
                ret = uip_parse_ld_raid_level(optarg, &ld_info.raid_level);
                break;

            case 1:
                if (ld_info.pd_list != NULL) {
                    g_free(ld_info.pd_list);
                    ld_info.pd_list = NULL;
                }
                ret = uip_parse_ld_pd_list(optarg, &ld_info.pd_list, &ld_info.pd_count);
                break;

            case 2:
                break;

            case 3:
                ret = uip_parse_ld_span_depth(optarg, &ld_info.span_depth);
                if (ret == RET_ERR) {
                    g_printf("Error : option '-%s' - invalid argument '%s'.\r\n", opts[idx].name, optarg);
                    free_pd_list(&ld_info.pd_list);
                    print_create_ld_help_info(type_id);
                    return FALSE;
                }
                if (ld_info.span_depth > SML_MAX_SPAN_DEPTH) {
                    g_printf("Error : option '-%s' - argument is out of range.\r\n", opts[idx].name);
                    free_pd_list(&ld_info.pd_list);
                    return FALSE;
                }
                break;

            case 4:
                
                if (optarg == NULL || check_ld_name_is_valid(optarg, 0) == RET_ERR) {
                    goto EXIT1;
                }
                
                if (dal_check_ld_name_too_long(ctrl_id, strlen(optarg)) == TRUE) {
                    g_printf("Error : logical drive name is too long.\r\n");
                    print_create_ld_help_info(type_id);
                    free_pd_list(&ld_info.pd_list);
                    return FALSE;
                }
                strncpy_s(ld_info.ld_name, sizeof(ld_info.ld_name), optarg,
                    sizeof(ld_info.ld_name) - 1);
                
                ret = dal_check_string_is_valid_ascii(ld_info.ld_name);
                if (ret != RET_OK) {
                    g_printf("Error : logical drive name contains invalid ASCII characters.\r\n");
                    free_pd_list(&ld_info.pd_list);
                    return FALSE;
                }
                
                break;

            case 5:
                ret = uip_parse_ld_size(optarg, &ld_info.size, &ld_info.size_unit);
                break;

            case 6:
                ret = uip_parse_ld_strip_size(optarg, &ld_info.strip_size);
                break;

            case 7:
                ret = uip_parse_ld_read_policy(optarg, &ld_info.read_policy);
                break;

            case 8:
                ret = uip_parse_ld_write_policy(optarg, &ld_info.write_policy);
                break;

            case 9:
                ret = uip_parse_ld_io_policy(optarg, &ld_info.io_policy);
                break;

            case 10:
                ret = uip_parse_ld_access_policy(optarg, &ld_info.access_policy);
                break;

            case 11:  // 11为处理cache_policy
                ret = uip_parse_ld_disk_cache_policy(optarg, &ld_info.disk_cache_policy);
                break;

            case 12:
                ret = uip_parse_ld_init_type(optarg, &ld_info.init_type);
                break;

            case 13:
                ret = uip_parse_ld_accelerator(optarg, &ld_info.accelerator);
                break;

            case 14:
                ret = uip_parse_associated_ld(optarg, &ld_info.associated_ld);
                break;

            case 15:
                ret = uip_parse_cache_line_size(optarg, &ld_info.cache_line_size);
                break;

            default:
                break;
        }

        
        if (idx != 2 && idx != 3 && idx != 4) {    //  2-是否cachecade，3-spandepeth，4-名称
            if (ret != RET_OK) {
                g_printf("Error : option '-%s' - invalid argument '%s'.\r\n", opts[idx].name, optarg);
                print_create_ld_help_info(type_id);
                free_pd_list(&ld_info.pd_list);
                return FALSE;
            }
        }
    }

    
    if (check_mandatory_params_for_create_ld(type_id, &ld_flags) != RET_OK) {
        print_create_ld_help_info(type_id);
        retval = FALSE;
        goto EXIT;
    }

    
    if (ld_flags.is_cachecade) {
        if (is_too_many_options_for_cachecade_ld(type_id, &ld_flags) == TRUE) {
            g_printf("Error : too many options for cachecade logical drive.\r\n");
            retval = FALSE;
            goto EXIT;
        }
    } else {
        if (is_too_many_options_for_common_ld(type_id, &ld_flags) == TRUE) {
            g_printf("Error : too many options for logical drive.\r\n");
            retval = FALSE;
            goto EXIT;
        }
    }

    if ((argc - optind) != 1) {
        g_free(ld_info.pd_list);
        ld_info.pd_list = NULL;
        g_printf("Error : one or more redundant arguments.\r\n");
        return FALSE;
    }

    
    ret = is_continue_operate();
    if (ret != RET_OK) {
        retval = FALSE;
        goto EXIT;
    }

    g_printf("The operation may take a few seconds, Please wait...\r\n");

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    if (ld_flags.is_cachecade == 0) {
        ret = call_create_common_ld_method(obj_handle, username, ip, &ld_info, &output_list);
    } else {
        ld_info.array_id = 0xFFFF;    //  Array id填无效值
        ret = call_create_cachecade_ld_method(obj_handle, username, ip, &ld_info, &output_list);
    }
    if (ret == RET_OK) {
        retval = TRUE;
    } else {
        retval = FALSE;
    }

    if (ret == RET_OK && output_list != NULL) {
        ld_id = g_variant_get_uint16((GVariant *)g_slist_nth_data(output_list, 0));
        g_printf("Create logical drive successfully. Logical drive ID is %d.\r\n", ld_id);
    }
    if (ret != RET_OK && output_list != NULL) {
        error_message = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 1), NULL);
        g_printf("Create logical drive failed. Error code : 0x%04X (%s).\r\n", ret,
            error_message != NULL ? error_message : "No details");
    }
    if (ret != RET_OK && output_list == NULL) {
        g_printf("Create logical drive failed. Error code : 0x%04X.\r\n", ret);
    }

EXIT:
    if (ld_info.pd_list != NULL) {
        g_free(ld_info.pd_list);
        ld_info.pd_list = NULL;
    }
    uip_free_gvariant_list(output_list);

    return retval;

EXIT1:
    g_printf("Error : option '-%s' - requires an argument.\r\n",
        (optarg == NULL && argc == optind) ? argv[argc - 1] + 1 : opts[idx].name);
    print_create_ld_help_info(type_id);
    free_pd_list(&ld_info.pd_list);
    return FALSE;
}


gint32 ipmc_add_logical_drive(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 retval;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    guint32 array_id = 0xFFFFFFFF;
    
    CLI_LD_INFO ld_info;
    CLI_LD_FLAGS ld_flags = {0};

    guint16 ld_id = 0xFFFF;
    const gchar *error_message = NULL;
    const gchar *optstring = ":";
    gint c = 0, idx = 0;

    struct option opts[] = {
        {UIP_LD_OPTION_ARRAY_ID, REQUIRED_ARGUMENT, &ld_flags.is_array_id, 1},
        {UIP_LD_OPTION_NAME, REQUIRED_ARGUMENT, &ld_flags.is_name, 1},
        {UIP_LD_OPTION_SIZE, REQUIRED_ARGUMENT, &ld_flags.is_size, 1},
        {UIP_LD_OPTION_STRIP_SIZE, REQUIRED_ARGUMENT, &ld_flags.is_strip_size, 1},
        {UIP_LD_OPTION_READ_POLICY, REQUIRED_ARGUMENT, &ld_flags.is_read_policy, 1},
        {UIP_LD_OPTION_WRITE_POLICY, REQUIRED_ARGUMENT, &ld_flags.is_write_policy, 1},
        {UIP_LD_OPTION_IO_POLICY, REQUIRED_ARGUMENT, &ld_flags.is_io_policy, 1},
        {UIP_LD_OPTION_ACCESS_POLICY, REQUIRED_ARGUMENT, &ld_flags.is_access_policy, 1},
        {UIP_LD_OPTION_DISK_CACHE_POLICY, REQUIRED_ARGUMENT, &ld_flags.is_disk_cache_policy, 1},
        {UIP_LD_OPTION_INIT_TYPE, REQUIRED_ARGUMENT, &ld_flags.is_init_type, 1},
        {UIP_LD_OPTION_BLOCK_FLAG, REQUIRED_ARGUMENT, &ld_flags.is_block_flag, 1},
        {UIP_LD_OPTION_RAID_LEVEL, REQUIRED_ARGUMENT, &ld_flags.is_raid_level, 1},
        {UIP_LD_OPTION_SPAN_DEPTH, REQUIRED_ARGUMENT, &ld_flags.is_span_depth, 1},
        {UIP_LD_OPTION_ACCELERATION_METHOD, REQUIRED_ARGUMENT, &ld_flags.is_accelerator, 1},
        {UIP_LD_OPTION_CACHECADE_FLAG, NO_ARGUMENT, &ld_flags.is_cachecade, 1},
        {UIP_LD_OPTION_ASSOCIATED_LD, REQUIRED_ARGUMENT, &ld_flags.is_associated_ld, 1},
        {UIP_LD_OPTION_CACHE_LINE_SIZE, REQUIRED_ARGUMENT, &ld_flags.is_cache_line_size, 1},
        {NULL, NO_ARGUMENT, NULL, 0}
    };

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    
    if (argc < 2 || (strcmp(argv[0], "-v") != 0)) {    // 保证argv[1]有值，参数个数不能小于2
        g_printf("Error : The first two paramters must be '-v <RAID Controller ID>'.\r\n");
        return FALSE;
    }

    gint32 ctrl_id = str_to_int(argv[1]);
    if (ctrl_id < 0) {
        g_printf("Invalid RAID controller ID.\r\n");
        return FALSE;
    }

    gint32 ret = dal_get_specific_object_byte(CLASS_RAID_CONTROLLER_NAME, PROPERTY_RAID_CONTROLLER_ID, (guint8)ctrl_id,
        &obj_handle);
    if (ret != RET_OK) {
        g_printf("Invalid RAID controller ID.\r\n");
        return FALSE;
    }

    guint8 type_id = 0;
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_RAID_CONTROLLER_TYPE_ID, &type_id);

    optind = 1;
    opterr = 0;

    init_ld_info(&ld_info);
    while ((c = getopt_long_only(argc, argv, optstring, opts, &idx)) != -1) {
        switch (c) {
            case ':':
                g_printf("Error : option '%s' - requires an argument.\r\n", argv[optind - 1]);
                print_add_ld_help_info(type_id);
                return FALSE;

            case '?':
                g_printf("Error : invalid option '%s'.\r\n", argv[optind - 1]);
                print_add_ld_help_info(type_id);
                return FALSE;

            default:
                break;
        }

        
        if (idx != 1 && idx != 14)  {  //  1-名称，14-是否cachecade
            if (optarg == NULL || *optarg == '-') {
                g_printf("Error : option '-%s' - requires an argument.\r\n",
                    (optarg == NULL && argc == optind) ? argv[argc - 1] + 1 : opts[idx].name);
                print_add_ld_help_info(type_id);
                return FALSE;
            }
        }

        switch (idx) {
            case 0:
                ret = uip_parse_ld_array_id(optarg, &array_id);
                if (ret == RET_ERR) {
                    g_printf("Error : option '-%s' - invalid argument '%s'.\r\n", opts[idx].name, optarg);
                    print_add_ld_help_info(type_id);
                    return FALSE;
                }
                if (array_id > 0xFFFF) {
                    g_printf("Error : option '-%s' - argument '%s' is out of range.\r\n", opts[idx].name, optarg);
                    return FALSE;
                }
                break;

            case 1:
                
                if (optarg == NULL || check_ld_name_is_valid(optarg, 1) == RET_ERR) {
                    g_printf("Error : option '-%s' - requires an argument.\r\n",
                        (optarg == NULL && argc == optind) ? argv[argc - 1] + 1 : opts[idx].name);
                    print_add_ld_help_info(type_id);
                    return FALSE;
                }
                
                if (dal_check_ld_name_too_long(ctrl_id, strlen(optarg)) == TRUE) {
                    g_printf("Error : logical drive name is too long.\r\n");
                    print_add_ld_help_info(type_id);
                    return FALSE;
                }
                strncpy_s(ld_info.ld_name, sizeof(ld_info.ld_name), optarg,
                    sizeof(ld_info.ld_name) - 1);
                
                ret = dal_check_string_is_valid_ascii(ld_info.ld_name);
                if (ret != RET_OK) {
                    g_printf("Error : logical drive name contains invalid ASCII characters.\r\n");
                    return FALSE;
                }
                
                break;

            case 2:
                ret = uip_parse_ld_size(optarg, &ld_info.size, &ld_info.size_unit);
                break;

            case 3:
                ret = uip_parse_ld_strip_size(optarg, &ld_info.strip_size);
                break;

            case 4:
                ret = uip_parse_ld_read_policy(optarg, &ld_info.read_policy);
                break;

            case 5:
                ret = uip_parse_ld_write_policy(optarg, &ld_info.write_policy);
                break;

            case 6:
                ret = uip_parse_ld_io_policy(optarg, &ld_info.io_policy);
                break;

            case 7:
                ret = uip_parse_ld_access_policy(optarg, &ld_info.access_policy);
                break;

            case 8:
                ret = uip_parse_ld_disk_cache_policy(optarg, &ld_info.disk_cache_policy);
                break;

            case 9:
                ret = uip_parse_ld_init_type(optarg, &ld_info.init_type);
                break;

            case 10:
                ret = uip_parse_ld_block_index(optarg, &ld_info.block_index);
                break;

            case 11:
                ret = uip_parse_ld_raid_level(optarg, &ld_info.raid_level);
                break;

            case 12:
                ret = uip_parse_ld_span_depth(optarg, &ld_info.span_depth);
                break;

            case 13:
                ret = uip_parse_ld_accelerator(optarg, &ld_info.accelerator);
                break;

            case 14:
                break;

            case 15:
                ret = uip_parse_associated_ld(optarg, &ld_info.associated_ld);
                break;

            case 16:
                ret = uip_parse_cache_line_size(optarg, &ld_info.cache_line_size);
                break;

            default:
                break;
        }

        
        if (idx != 0 && idx != 1 && idx != 14) {  //  0-阵列id，1-名称，14-是否cachecade
            if (ret != RET_OK) {
                g_printf("Error : option '-%s' - invalid argument '%s'.\r\n", opts[idx].name, optarg);
                print_add_ld_help_info(type_id);
                return FALSE;
            }
        }
    }

    if (check_mandatory_params_for_add_ld(type_id, &ld_flags) != RET_OK) {
        print_add_ld_help_info(type_id);
        return FALSE;
    }

    if (ld_flags.is_cachecade) {
        if (is_too_many_options_for_cachecade_ld(type_id, &ld_flags) == TRUE) {
            g_printf("Error : too many options for cachecade logical drive.\r\n");
            return FALSE;
        }
    } else {
        if (is_too_many_options_for_common_ld(type_id, &ld_flags) == TRUE) {
            g_printf("Error : too many options for logical drive.\r\n");
            return FALSE;
        }
    }

    if ((argc - optind) != 1) {
        g_printf("Error : one or more redundant arguments.\r\n");
        return FALSE;
    }

    
    ret = is_continue_operate();
    if (ret != RET_OK) {
        return FALSE;
    }

    g_printf("The operation may take a few seconds, Please wait...\r\n");

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    ld_info.array_id = (guint16)(array_id & 0xFFFF);

    if (ld_flags.is_cachecade == 0) {
        ret = call_add_ld_method(obj_handle, username, ip, &ld_info, &output_list);
    } else {
        guint8 invalid_pd_id = 0xFF;
        ld_info.pd_list = &invalid_pd_id;
        ld_info.pd_count = 1;
        ret = call_create_cachecade_ld_method(obj_handle, username, ip, &ld_info, &output_list);
    }

    if (ret == RET_OK) {
        retval = TRUE;
    } else {
        retval = FALSE;
    }

    if (ret == RET_OK && output_list != NULL) {
        ld_id = g_variant_get_uint16((GVariant *)g_slist_nth_data(output_list, 0));
        g_printf("Add logical drive successfully. Logical drive ID is %d.\r\n", ld_id);
    }
    if (ret != RET_OK && output_list != NULL) {
        error_message = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 1), NULL);
        g_printf("Add logical drive failed. Error code : 0x%04X (%s).\r\n", ret,
            error_message != NULL ? error_message : "No details");
    }
    if (ret != RET_OK && output_list == NULL) {
        g_printf("Add logical drive failed. Error code : 0x%04X.\r\n", ret);
    }

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    return retval;
}


gint32 ipmc_delete_logical_drive(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    gint32 retval;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    gint32 ctrl_id;
    gint32 ld_id;
    const gchar *error_message = NULL;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    
    if (argc < 3 || (strcmp(argv[0], "-v") != 0)) {
        g_printf("Usage:\r\n");
        g_printf("  ipmcset -t storage -d deleteld -v <RAID Controller ID> <Logical Drive ID>\r\n\n");
        return FALSE;
    }

    ctrl_id = str_to_int(argv[1]);
    if (ctrl_id < 0) {
        g_printf("Invalid RAID controller ID.\r\n");
        return FALSE;
    }

    ret = dal_get_specific_object_byte(CLASS_RAID_CONTROLLER_NAME, PROPERTY_RAID_CONTROLLER_ID, (guint8)ctrl_id,
        &obj_handle);
    if (ret != RET_OK) {
        g_printf("Invalid RAID controller ID.\r\n");
        return FALSE;
    }

    ld_id = str_to_int(argv[2]);
    if (ld_id < 0 || ld_id > 0xFFFF) {
        g_printf("Invalid logical drive ID.\r\n");
        return FALSE;
    }

    
    ret = is_continue_operate();
    if (ret != RET_OK) {
        return FALSE;
    }

    g_printf("The operation may take a few seconds, Please wait...\r\n");

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_uint16((guint16)ld_id));
    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    ret =
        uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_RAID_CONTROLLER_NAME,
        dfl_get_object_name(obj_handle), METHOD_RAID_CONTROLLER_DELETE_LD, input_list, &output_list);
    if (ret == RET_OK) {
        g_printf("Delete logical drive successfully.\r\n");
        retval = TRUE;
    } else {
        if (output_list != NULL) {
            error_message = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
            g_printf("Delete logical drive failed. Error code : 0x%04X (%s).\r\n", ret,
                error_message ? error_message : "No details");
        } else {
            g_printf("Delete logical drive failed. Error code : 0x%04X.\r\n", ret);
        }

        retval = FALSE;
    }

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    return retval;
}


gint32 ipmc_set_logical_drive_config(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;
    CLI_LD_INFO ld_info;
    CLI_LD_FLAGS ld_flags = {0};
    const gchar *optstring = ":";
    gint c = 0, idx = 0;

    struct option opts[] = {
        {UIP_LD_OPTION_NAME, REQUIRED_ARGUMENT, &ld_flags.is_name, 1},
        {UIP_LD_OPTION_READ_POLICY, REQUIRED_ARGUMENT, &ld_flags.is_read_policy, 1},
        {UIP_LD_OPTION_WRITE_POLICY, REQUIRED_ARGUMENT, &ld_flags.is_write_policy, 1},
        {UIP_LD_OPTION_IO_POLICY, REQUIRED_ARGUMENT, &ld_flags.is_io_policy, 1},
        {UIP_LD_OPTION_ACCESS_POLICY, REQUIRED_ARGUMENT, &ld_flags.is_access_policy, 1},
        {UIP_LD_OPTION_DISK_CACHE_POLICY, REQUIRED_ARGUMENT, &ld_flags.is_disk_cache_policy, 1},
        {UIP_LD_OPTION_BGI_STATE, REQUIRED_ARGUMENT, &ld_flags.is_bgi_state, 1},
        {UIP_LD_OPTION_BOOTABLE_FLAG, REQUIRED_ARGUMENT, &ld_flags.is_bootable, 1},
        {UIP_LD_OPTION_CACHECADE_STATE, REQUIRED_ARGUMENT, &ld_flags.is_sscd_caching, 1},
        {UIP_LD_OPTION_ACCELERATION_METHOD, REQUIRED_ARGUMENT, &ld_flags.is_accelerator, 1},
        {UIP_LD_OPTION_SIZE, REQUIRED_ARGUMENT, &ld_flags.is_size, 1},
        {UIP_LD_OPTION_STRIP_SIZE, REQUIRED_ARGUMENT, &ld_flags.is_strip_size, 1},
        {NULL, NO_ARGUMENT, NULL, 0}
    };

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    
    if (argc < 3 || (strcmp(argv[0], "-v") != 0)) {    // 保证argv[1]、argv[2]有值，参数个数不能小于3
        g_printf("Error : The first three paramters must be '-v <RAID Controller ID> <Logical Drive ID>'.\r\n");
        return FALSE;
    }

    gint32 ctrl_id = str_to_int(argv[1]);
    if (ctrl_id < 0) {
        g_printf("Invalid RAID controller ID.\r\n");
        return FALSE;
    }

    gint32 ld_id = str_to_int(argv[2]);  // 参数2是逻辑盘id
    if (ld_id < 0 || ld_id > 0xFFFF) {
        g_printf("Invalid logical drive ID.\r\n");
        return FALSE;
    }

    gint32 ret = dal_get_logical_drive_object_handle(ctrl_id, ld_id, &obj_handle);
    if (ret != RET_OK) {
        return FALSE;
    }

    OBJ_HANDLE ctrl_obj = 0;
    guint8 type_id = 0;
    (void)dal_get_specific_object_byte(CLASS_RAID_CONTROLLER_NAME, PROPERTY_RAID_CONTROLLER_ID, ctrl_id, &ctrl_obj);
    (void)dal_get_property_value_byte(ctrl_obj, PROPERTY_RAID_CONTROLLER_TYPE_ID, &type_id);

    optind = 1;
    opterr = 0;

    init_ld_info(&ld_info);
    while ((c = getopt_long_only(argc, argv, optstring, opts, &idx)) != -1) {
        switch (c) {
            case ':':
                g_printf("Error : option '%s' - requires an argument.\r\n", argv[optind - 1]);
                print_modify_ld_help_info(type_id);
                return FALSE;

            case '?':
                g_printf("Error : invalid option '%s'.\r\n", argv[optind - 1]);
                print_modify_ld_help_info(type_id);
                return FALSE;

            default:
                break;
        }

        
        if (idx != 0) {   // 0-名称
            if (optarg == NULL || *optarg == '-') {
                g_printf("Error : option '-%s' - requires an argument.\r\n",
                    (optarg == NULL && argc == optind) ? argv[argc - 1] + 1 : opts[idx].name);
                print_modify_ld_help_info(type_id);
                return FALSE;
            }
        }

        switch (idx) {
            case 0:
                
                if (optarg == NULL || check_ld_name_is_valid(optarg, 2) == RET_ERR) {
                    g_printf("Error : option '-%s' - requires an argument.\r\n",
                        (optarg == NULL && argc == optind) ? argv[argc - 1] + 1 : opts[idx].name);
                    print_modify_ld_help_info(type_id);
                    return FALSE;
                }
                
                if (dal_check_ld_name_too_long(ctrl_id, strlen(optarg)) == TRUE) {
                    g_printf("Error : logical drive name is too long.\r\n");
                    print_modify_ld_help_info(type_id);
                    return FALSE;
                }
                strncpy_s(ld_info.ld_name, sizeof(ld_info.ld_name), optarg, sizeof(ld_info.ld_name) - 1);
                
                ret = dal_check_string_is_valid_ascii(ld_info.ld_name);
                if (ret != RET_OK) {
                    g_printf("Error : logical drive name contains invalid ASCII characters.\r\n");
                    return FALSE;
                }
                
                break;

            case 1:
                ret = uip_parse_ld_read_policy(optarg, &ld_info.read_policy);
                break;

            case 2:
                ret = uip_parse_ld_write_policy(optarg, &ld_info.write_policy);
                break;

            case 3:
                ret = uip_parse_ld_io_policy(optarg, &ld_info.io_policy);
                break;

            case 4:
                ret = uip_parse_ld_access_policy(optarg, &ld_info.access_policy);
                break;

            case 5:
                ret = uip_parse_ld_disk_cache_policy(optarg, &ld_info.disk_cache_policy);
                break;

            case 6:
                ret = uip_parse_ld_bgi_state(optarg, &ld_info.bgi_state);
                break;

            case 7:
                ret = uip_parse_boot_priority(optarg, &ld_info.bootable);
                break;

            case 8:
                ret = uip_parse_ld_sscd_caching_state(optarg, &ld_info.sscd_caching_state);
                break;

            case 9:
                ret = uip_parse_ld_accelerator(optarg, &ld_info.accelerator);
                break;

            case 10:
                ret = uip_parse_ld_size(optarg, &ld_info.size, &ld_info.size_unit);
                break;

            case 11:
                ret = uip_parse_ld_strip_size(optarg, &ld_info.strip_size);
                break;

            default:
                break;
        }

        
        if (idx != 0) {    // 0-名称
            if (ret != RET_OK) {
                g_printf("Error : option '-%s' - invalid argument '%s'.\r\n", opts[idx].name, optarg);
                print_modify_ld_help_info(type_id);
                return FALSE;
            }
        }
    }

    
    if ((ld_flags.is_name + ld_flags.is_read_policy + ld_flags.is_write_policy + ld_flags.is_io_policy +
        ld_flags.is_access_policy + ld_flags.is_disk_cache_policy + ld_flags.is_bgi_state + ld_flags.is_bootable +
        ld_flags.is_sscd_caching + ld_flags.is_accelerator + ld_flags.is_size + ld_flags.is_strip_size) == 0) {
        g_printf("Error : no valid option.\r\n");
        print_modify_ld_help_info(type_id);
        return FALSE;
    }
    

    if ((argc - optind) != 2) {
        g_printf("Error : one or more redundant arguments.\r\n");
        return FALSE;
    }

    
    ret = is_continue_operate();
    if (ret != RET_OK) {
        return FALSE;
    }

    g_printf("The operation may take a few seconds, Please wait...\r\n");

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    if (ld_flags.is_name == 1) {
        set_logical_drive_name(obj_handle, ld_info.ld_name, username, ip);
    }
    if (ld_flags.is_read_policy == 1) {
        set_logical_drive_read_policy(obj_handle, ld_info.read_policy, username, ip);
    }
    if (ld_flags.is_write_policy == 1) {
        set_logical_drive_write_policy(obj_handle, ld_info.write_policy, username, ip);
    }
    if (ld_flags.is_io_policy == 1) {
        set_logical_drive_io_policy(obj_handle, ld_info.io_policy, username, ip);
    }
    if (ld_flags.is_access_policy == 1) {
        set_logical_drive_access_policy(obj_handle, ld_info.access_policy, username, ip);
    }
    if (ld_flags.is_disk_cache_policy == 1) {
        set_logical_drive_disk_cache_policy(obj_handle, ld_info.disk_cache_policy, username, ip);
    }
    if (ld_flags.is_bgi_state == 1) {
        set_logical_drive_bgi_state(obj_handle, ld_info.bgi_state, username, ip);
    }
    if (ld_flags.is_bootable == 1) {
        set_logical_drive_bootable(obj_handle, ld_info.bootable, username, ip);
    }
    if (ld_flags.is_sscd_caching == 1) {
        set_logical_drive_sscd_caching_state(obj_handle, ld_info.sscd_caching_state, username, ip);
    }

    if (ld_flags.is_accelerator == 1) {
        set_logical_drive_acceletor(obj_handle, ld_info.accelerator, username, ip);
    }

    if (ld_flags.is_size == 1) {
        set_logical_drive_capacity(obj_handle, ld_info.size, ld_info.size_unit, username, ip);
    }

    if (ld_flags.is_strip_size == 1) {
        set_logical_drive_strip_size(obj_handle, ld_info.strip_size, username, ip);
    }

    return TRUE;
}
