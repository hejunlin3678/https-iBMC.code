

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
#include "ipmcset_power.h"

#define CHANNEL_NULL 0
#define CHANNEL_CPLD 1
#define CHANNEL_I2C  2

#define ACTION_CLOSE (1 << 0)
#define ACTION_OPEN  (1 << 1)
#define ACTION_RESET (1 << 2)

#define DEFAULT_PS_NUM 2
#define DEFAULT_MAX_FAN_SLOT 14
#define CLI_COMFIRM_STRING "Do you want to continue?[Y/N]:"

LOCAL void printf_set_psu_sleep_config_helpinfo(void);
LOCAL void print_set_powerstate_helpinfo(gchar *target);
LOCAL void print_set_fan_smartmode(void);
LOCAL guint32 get_max_fan_slot(void);
LOCAL gint32 set_shelf_fan_speed(const gchar *username, const gchar *ip, gint32 speed, gint32 max_fan_slot);
LOCAL gint32 set_fan_speed(const gchar *username, const gchar *ip, gint32 slot, gint32 speed);


gint32 shelf_set_psu_sleep_config(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    OBJ_HANDLE object_handle = 0;
    guint8 enable = 0;
    guint8 mode = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    gint32 ret_code;
    guint8 ps_closed_manually_count = 0;
    gint32 i = 0;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if ((argc != 3) && (argc != 2)) {
        printf_set_psu_sleep_config_helpinfo();
        return FALSE;
    } else if (strncmp("-v", argv[0], strlen(argv[0]))) {
        printf_set_psu_sleep_config_helpinfo();
        return FALSE;
    }

    if (argc == 2) {
        if (!strncmp("disable", argv[1], strlen(argv[1]))) {
            enable = 0;
        } else {
            printf_set_psu_sleep_config_helpinfo();
            return FALSE;
        }
    } else if (argc == 3) {
        if (!strncmp("enable", argv[1], strlen(argv[1]))) {
            enable = 1;
            if (!strncmp("0", argv[2], strlen(argv[2]))) {
                mode = 0;
            } else if (!strncmp("1", argv[2], strlen(argv[2]))) {
                mode = 1;
            } else {
                printf_set_psu_sleep_config_helpinfo();
                return FALSE;
            }
        } else {
            printf_set_psu_sleep_config_helpinfo();
            return FALSE;
        }
    }

    ret = dal_get_object_handle_nth(CLASS_POWER_SLEEP, 0, &object_handle);
    if (ret != RET_OK) {
        g_printf("Set psu sleep config failed.\r\n");
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(enable));
    input_list = g_slist_append(input_list, g_variant_new_byte(mode));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_POWER_SLEEP,
        dfl_get_object_name(object_handle), METHOD_SET_PS_SLEEP_CONFIG, input_list, &output_list);
    if (ret != RET_OK) {
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        g_printf("Set psu sleep config failed.\r\n");
        return FALSE;
    }

    ret_code = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
    if (ret_code == PEM_CLOSED_MANULLY) { 
        ps_closed_manually_count = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 1));
        g_printf("The command cannot be executed when the following psu are closed manually:");

        for (i = 0; i < ps_closed_manually_count; i++) {
            g_printf("%d ", 1 + g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, i + 2)));
        }
        g_printf("\r\n");
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

        return FALSE;
    } else if (ret_code == PEM_RETURN_ERROR) { 
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

        g_printf("Set psu sleep config failed.\r\n");
        return FALSE;
    } else if (ret_code == PEM_CONFIG_INVALID) { 
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

        g_printf("Shelf condition dose not satisfy PSU sleep config!\r\n");
        return FALSE;
    } else {
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    }

    g_printf("Success.\r\n");

    return TRUE;
}


gint32 smm_set_powercapping_blade_value(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target, gchar *location)
{
    gint32 ret;

    ret = ipmc_set_powercappinginfo_bladevalue(privilege, rolepriv, argc, argv, target);
    return ret;
}

gint32 smm_set_powercapping_enable(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target,
    gchar *location)
{
    gint32 ret;

    ret = ipmc_set_powercappinginfo_enable(privilege, rolepriv, argc, argv, target);
    return ret;
}


gint32 smm_set_powercapping_fail_action(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target, gchar *location)
{
    gint32 ret;

    ret = ipmc_set_powercappinginfo_failaction(privilege, rolepriv, argc, argv, target);
    return ret;
}


gint32 smm_set_powercapping_mode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target,
    gchar *location)
{
    gint32 ret;

    ret = ipmc_set_powercappinginfo_mode(privilege, rolepriv, argc, argv, target);
    return ret;
}


gint32 smm_set_powercapping_threshold(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target,
    gchar *location)
{
    gint32 ret;

    ret = ipmc_set_powercappinginfo_threshold(privilege, rolepriv, argc, argv, target);
    return ret;
}


gint32 smm_set_powercapping_value(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target,
    gchar *location)
{
    gint32 ret;

    ret = ipmc_set_powercappinginfo_value(privilege, rolepriv, argc, argv, target);
    return ret;
}


gint32 smm_set_poweroninterval(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 iRet;
    gchar obj_name[OBJ_NAME_MAX_LEN] = {0};
    gint32 ret;
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    gint32 time_vlaue = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        print_poweroninterval_usage();
        return FALSE;
    }

    if (strlen(argv[1]) == 0) {
        print_poweroninterval_usage();
        return RET_ERR;
    }

    ret = vos_str2int(argv[1], 10, &time_vlaue, NUM_TPYE_INT32);
    if (ret != RET_OK) {
        print_poweroninterval_usage();
        return ret;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }
    iRet = snprintf_s(obj_name, OBJ_NAME_MAX_LEN, OBJ_NAME_MAX_LEN - 1, "%s", CLASS_NAME_SHELF_MANAGE);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    input_list = g_slist_append(input_list, g_variant_new_int32(time_vlaue));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_NAME_SHELF_MANAGE,
        obj_name, METHOD_SET_STAGGER_POWER_DELAY_TIME, input_list, NULL);
    if (ret != RET_OK) {
        if (ret == USER_PARAMETER_OUT_OF_RANGE) {
            g_printf("Set fail:parameter error.\r\n");
            print_poweroninterval_usage();
        } else {
            g_printf("Set fail:internal error.\r\n");
        }
        uip_free_gvariant_list(input_list);
        return FALSE;
    }

    g_printf("Set power on interval time successfully.\r\n");
    uip_free_gvariant_list(input_list);
    return TRUE;
}


gint32 smm_set_powerstate(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    OBJ_HANDLE object_handle = 0;
    guint8 operation = 0;
    guint8 slot_id;
    guint8 fru_id = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    guint8 slave_addr;
    gint32 ret_code;
    guint8 presence = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (((argc != 2) && (argc != 3)) || strncmp("-v", argv[0], strlen(argv[0]))) {
        print_set_powerstate_helpinfo(target);
        return FALSE;
    }

    slot_id = get_blade_num(target);
    if (slot_id == 0xff) {
        print_set_powerstate_helpinfo(target);
        return FALSE;
    }

    if ((strncmp("blade", target, strlen("blade")) == 0) && (argc == 2)) {
        if (!strcmp("poweron", argv[1])) {
            operation = 1;
            fru_id = 0;
        } else if (!strcmp("poweroff", argv[1])) {
            operation = 0;
            fru_id = 0;
        } else if (!strcmp("forcepoweroff", argv[1])) {
            operation = 2;
            fru_id = 0;
        } else {
            print_set_powerstate_helpinfo(target);
            return FALSE;
        }
    } else if ((strncmp("swi", target, strlen("swi")) == 0) && (argc == 3)) {
        if ((strlen(argv[1]) != 1) || (argv[1][0] < '1') || (argv[1][0] > '3')) {
            print_set_powerstate_helpinfo(target);
            return FALSE;
        }
        fru_id = (guint8)(argv[1][0] - '0');
        if (!strcmp("poweron", argv[2])) {
            operation = 1;
        } else if (!strcmp("poweroff", argv[2])) {
            operation = 0;
        } else {
            print_set_powerstate_helpinfo(target);
            return FALSE;
        }
    } else {
        print_set_powerstate_helpinfo(target);
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    
    slave_addr = get_blade_ipmbaddr(slot_id);
    (void)dal_get_specific_object_byte((const gchar *)CLASS_NAME_IPMBETH_BLADE,
        (const gchar *)PROTERY_IPMBETH_BLADE_SLAVEADDR, (guchar)slave_addr, &object_handle);

    
    (void)dal_get_property_value_byte(object_handle, (const gchar *)PROTERY_IPMBETH_BLADE_PRESENCE, &presence);
    
    target[0] = target[0] - ('a' - 'A');
    if (presence == 0) {
        g_printf("%s is not present.\r\n", target);
        return FALSE;
    }

    if (strncmp("Swi", target, strlen("Swi")) == 0) {
        g_printf("WARNING: All service planes (Base, Fabric, and FC) will be powered %s at the same time.\r\n",
            operation ? "on" : "off");
    } else {
        g_printf("WARNING: The operation may have many adverse effects.\r\n");
    }

    ret = check_the_input_operation(CLI_COMFIRM_STRING);
    if (ret != RET_OK) {
        return FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(slave_addr));
    input_list = g_slist_append(input_list, g_variant_new_byte(fru_id));
    input_list = g_slist_append(input_list, g_variant_new_byte(operation));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_NAME_IPMBETH_BLADE,
        (const gchar *)dfl_get_object_name(object_handle), METHOD_SET_BLADE_BMC_POWERTSATE, input_list, &output_list);
    if (ret != RET_OK) {
        g_printf("Set power state failed.\r\n");
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        return FALSE;
    }

    ret_code = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    if (ret_code == 1) {
        g_printf("The FRU or target does not exist.\r\n");
        return FALSE;
    }

    g_printf("Success.\r\n");

    return TRUE;
}

LOCAL gint32 smm_set_ps_switch_poweron_or_poweroff(guint8 slot, guint8 ps_switch,
    OBJ_HANDLE object_handle, gchar *target)
{
    gint32 ret;
    gint32 ret_code;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    gchar accessor_name[PARAMETER_LEN] = {0};
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(slot - 1));
    input_list = g_slist_append(input_list, g_variant_new_byte(ps_switch));
    ret = dfl_get_property_accessor(object_handle, PROPERTY_POWER_MGNT_SWITCH, accessor_name, PARAMETER_LEN);
    if (ret != DFL_OK) {
        g_printf("Do not support set pem switch.\r\n");
        return FALSE;
    }

    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_POWER_MGNT,
        dfl_get_object_name(object_handle), METHOD_SET_PS_SWITCH, input_list, &output_list);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);

    if (ret != RET_OK) {
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        g_printf("Set pem switch failed.\r\n");
        return FALSE;
    }

    ret_code = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

    if (ret_code == PEM_NOT_PRESENT) {
        g_printf("Pem%u is not present.\r\n", slot);
        return FALSE;
    }
    if (ret_code != PEM_RETURN_OK) {
        g_printf("Set pem switch failed.\r\n");
        return FALSE;
    }
    g_printf("Success.\r\n");
    return TRUE;
}

LOCAL void explain_ps_reset_retcode(gint32 ret_code, guint8 slot, OBJ_HANDLE object_handle)
{
    guint16 delay_time_interval = 0;
    (void)dal_get_property_value_uint16(object_handle, PROPERTY_POWER_MGNT_DELAY_TIME_INTERVAL, &delay_time_interval);

    switch (ret_code) {
        case PEM_NOT_PRESENT:
            g_printf("PSU%u is not present.\r\n", slot);
            return;
        case PEM_RESET_LESS_THAN_TIMEINT:
            g_printf("No more than %us since the last operation\r\n", delay_time_interval);
            return;
        case PEM_RETURN_OK:
            g_printf("Success.\r\n");
            return;
        default:
            g_printf("Set psu reset failed.\r\n");
            return;
    }
}

LOCAL gint32 set_ps_switch_reset(guint8 slot, guint8 ps_switch, OBJ_HANDLE object_handle, gchar *target)
{
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    gchar accessor_name[PARAMETER_LEN] = {0};
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(slot - 1));
    input_list = g_slist_append(input_list, g_variant_new_byte(ps_switch));
    gint32 ret = dfl_get_property_accessor(object_handle, PROPERTY_POWER_MGNT_PS_INFO, accessor_name, PARAMETER_LEN);
    if (ret != DFL_OK) {
        g_printf("Do not support set psu reset.\r\n");
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        return FALSE;
    }
    
    ret = dal_get_object_handle_nth(CLASS_POWER_MGNT, slot - 1, &object_handle);
    if (ret != RET_OK) {
        g_printf("Set psu reset failed.\r\n");
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        return FALSE;
    }

    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_POWER_MGNT,
        dfl_get_object_name(object_handle), METHOD_SET_PS_RESET, input_list, &output_list);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != RET_OK) {
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        g_printf("Set psu reset failed.\r\n");
        return FALSE;
    }

    gint32 ret_code = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

    explain_ps_reset_retcode(ret_code, slot, object_handle);
    return !ret_code;
}

LOCAL gint32 verify_set_ps_switch(OBJ_HANDLE obj_handle, guint8 ps_switch, guint8 slot, gchar *target, gint32 is_smm)
{
    guint8 channel_type = 0;
    gchar* ps_name = is_smm ? "pem" : "psu";

    gint32 ret = dal_get_property_value_byte(obj_handle, PROPERTY_POWER_MGNT_CHANNEL_TYPE, &channel_type);
    if (ret != RET_OK) {
        g_printf("Set %s switch failed.\r\n", ps_name);
        return FALSE;
    }
    
    if (channel_type == CHANNEL_NULL) {
        g_printf("Do not support set %s switch.\r\n", ps_name);
        return FALSE;
    }
    if (channel_type == CHANNEL_CPLD) {
        ret = smm_set_ps_switch_poweron_or_poweroff(slot, ps_switch, obj_handle, target);
    }
    if (channel_type == CHANNEL_I2C) {
        ret = set_ps_switch_reset(slot, ps_switch, obj_handle, target);
    }

    return ret;
}

LOCAL gint32 get_power_sleep_enable_status(void)
{
    gint32 ret;
    guint8 sleep_enable = 0;
    OBJ_HANDLE object_handle = 0;

    ret = dal_get_object_handle_nth(CLASS_POWER_SLEEP, 0, &object_handle);
    if (ret != RET_OK) {
        g_printf("Set pem switch failed.\r\n");
        return ret;
    }
    ret = dal_get_property_value_byte(object_handle, PROPERTY_POWER_SLEEP_ENABLE, &sleep_enable);
    if (ret != RET_OK) {
        g_printf("Set pem switch failed.\r\n");
        return ret;
    }

    if (sleep_enable == 1) {
        g_printf("Set pem switch failed.\r\n");
        g_printf("The command cannot be executed when the psu is in sleep mode.\r\n");
        return RET_ERR;
    }
    return RET_OK;
}

LOCAL void print_usage_according_mask(guint8 actionmask)
{
    g_printf("value are:\r\n");
    if (actionmask & ACTION_CLOSE) {
        g_printf("\t0\tClose\r\n");
    }
    if (actionmask & ACTION_OPEN) {
        g_printf("\t1\tOpen\r\n");
    }
    if (actionmask & ACTION_RESET) {
        g_printf("\t2\tReset\r\n");
    }
}

gint32 smm_set_ps_switch(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    OBJ_HANDLE object_handle = 0;
    guint8 ps_switch = 0;
    guint8 slot;
    guint8 actionmask = 0;
    gint32 is_smm = TRUE;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    (void)dal_get_specific_object_byte(CLASS_POWER_MGNT, PROPERTY_POWER_MGNT_SLOT, 0, &object_handle);
    (void)dal_get_property_value_byte(object_handle, PROPERTY_POWER_MGNT_ACTION_MASK, &actionmask);

    guint8 max_ps_num = DEFAULT_PS_NUM;
    (void)get_component_ps_max_num(&max_ps_num);

    if ((argc != 2) || strlen(argv[0]) != 2 || (strncmp("-v", argv[0], strlen(argv[0]))) ||
        strlen(argv[1]) > 1 || argv[1][0] > '2' || argv[1][0] < '0' || ((1 << (argv[1][0] - '0')) & actionmask) == 0) {
        g_printf("Usage: ipmcset -l pemN -d pemswitch -v <value>\r\n\tpem1 is the first power source\n\tN is psuid, "
                 "1 <= N <= %u\n", max_ps_num);
        print_usage_according_mask(actionmask);
        return FALSE;
    }

    ret = get_power_sleep_enable_status();
    if (ret != RET_OK) {
        return FALSE;
    }

    slot = get_pem_num(target);
    ret = dal_get_specific_object_byte(CLASS_POWER_MGNT, PROPERTY_POWER_MGNT_SLOT, slot - 1, &object_handle);
    if (ret != RET_OK) {
        g_printf("Set pem switch failed.\r\n");
        return FALSE;
    }

    if (slot > max_ps_num) {
        g_printf("Pem%u does not exist.\r\n", slot);
        return FALSE;
    }

    ps_switch = argv[1][0] - '0';

    g_printf("WARNING: The operation may have many adverse effects.\r\n");
    if (check_the_input_operation(CLI_COMFIRM_STRING) != VOS_OK) {
        return FALSE;
    }

    ret = verify_set_ps_switch(object_handle, ps_switch, slot, target, is_smm);
    return ret;
}

gint32 ipmc_set_ps_switch(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    OBJ_HANDLE object_handle = 0;
    guint8 ps_switch = 0;
    gint32 ret;
    guint8 actionmask = 0;
    gint32 is_smm = FALSE;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    (void)dal_get_specific_object_byte(CLASS_POWER_MGNT, PROPERTY_POWER_MGNT_SLOT, 0, &object_handle);
    (void)dal_get_property_value_byte(object_handle, PROPERTY_POWER_MGNT_ACTION_MASK, &actionmask);

    guint8 max_ps_num = DEFAULT_PS_NUM;
    (void)get_component_ps_max_num(&max_ps_num);

    if ((argc != 3) || strlen(argv[0]) != 2 || (strncmp("-v", argv[0], strlen("-v"))) ||
        strlen(argv[1]) != 4 || (strncmp("psu", argv[1], strlen("psu"))) || argv[1][3] - '0' > max_ps_num ||
        argv[1][3] < '1' || strlen(argv[2]) > 1 || argv[2][0] > '2' || argv[2][0] < '0' ||
        ((1 << (argv[2][0] - '0')) & actionmask) == 0) {
        g_printf("Usage: ipmcset -d psuswitch -v psu<N> <value>\r\n\tpsu1 is the first power source\n\tN is psuid, "
                 "1 <= N <= %u\n", max_ps_num);
        print_usage_according_mask(actionmask);
        return FALSE;
    }

    ps_switch = (guint8)(argv[2][0] - '0');
    guint8 slot = argv[1][3] - '0';

    ret = dal_get_specific_object_byte(CLASS_POWER_MGNT, PROPERTY_POWER_MGNT_SLOT, slot - 1, &object_handle);
    if (ret != RET_OK) {
        g_printf("Set psu reset failed, psu%d is not find\r\n", slot);
        return FALSE;
    }

    g_printf("WARNING: The operation may have many adverse effects.\r\n");
    if (check_the_input_operation(CLI_COMFIRM_STRING) != VOS_OK) {
        return FALSE;
    }
    
    ret = verify_set_ps_switch(object_handle, ps_switch, slot, target, is_smm);
    return ret;
}


gint32 smm_set_fan_ctl_mode(guchar priv, const gchar *role, guchar argc, gchar **argv, gchar *target)
{
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    GSList *input = NULL;
    gint32 ret;

    if (ipmc_privilege_judge(priv, role) == FALSE) {
        g_printf("permission denied.\n");
        return FALSE;
    }

    if (argc != 2) {
        g_printf("Usage: ipmcset -l fantray -d fancontrolmode -v auto/manual\r\n");
        return FALSE;
    }

    if (strcmp(target, "fantray") || strcmp("-v", argv[0])) {
        g_printf("Usage: ipmcset -l fantray -d fancontrolmode -v auto/manual\r\n");
        return FALSE;
    }

    
    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }
    

    if (!strcasecmp(argv[1], "auto")) {
        input = g_slist_append(input, g_variant_new_byte(AUTO_FAN_MODE));
    } else if (!strcasecmp(argv[1], "manual")) {
        input = g_slist_append(input, g_variant_new_byte(MANUAL_FAN_MODE));
    } else {
        
        g_printf("Unknown mode.\n");
        
        return RET_ERR;
    }
    input = g_slist_append(input, g_variant_new_uint32(MAX_FAN_TIMEOUT)); 
    ret = uip_call_class_method(OPERATOR_LOG_CLI, username, ip, COOLINGCLASS, COOLINGCLASS, METHOD_COOLING_MODE, input,
        NULL);
    uip_free_gvariant_list(input);
    if (ret != RET_OK) {
        g_printf("Set controlmode fail\n");
    } else {
        g_printf("Success.\n");
    }
    return ret;
}

gint32 smm_set_fan_smartmode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    guchar mode = 0;
    gchar username[PARAMETER_LEN + 1] = { 0 };
    gchar ip[PARAMETER_LEN + 1] = { 0 };
    GSList *inputlist = NULL;
    gint32 input;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;
    

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        g_printf("Permission denied.\n");
        return FALSE;
    }
    if (argc != 2) {
        print_set_fan_smartmode();
        return FALSE;
    }
    if (strcmp(target, "fantray") || strcmp("-v", argv[0])) {
        print_set_fan_smartmode();
        return FALSE;
    }

    
    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }
    

    
    property_name_list = g_slist_append(property_name_list, COOLING_PROPERTY_MODE);
    ret = uip_multiget_object_property(COOLINGCLASS, COOLINGCLASS, property_name_list, &property_value);
    g_slist_free(property_name_list);
    property_name_list = NULL;
    if (ret != RET_OK) {
        g_printf("Get fan mode and smart mode failed.\r\n");
        return FALSE;
    } else {
        
        mode = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 0));
        uip_free_gvariant_list(property_value);
        property_value = NULL;

        if (mode == MANUAL_FAN_MODE) {
            g_printf("Current fan control mode: Manual \r\n");
            input = ipmc_get_user_input("This operation will make fan "
                "CONTROL MODE switch over to AUTO.Continue?[Y/N]:");
            
            if (input == 'n' || input == 'N') {
                g_slist_free(property_name_list);
                return FALSE;
            }
            if (input != 'y' && input != 'Y') {
                g_printf("Input character invalid.\r\n");
                g_slist_free(property_name_list);
                return FALSE;
            }
        }

        if (!strcasecmp(argv[1], "1")) {
            inputlist = g_slist_append(inputlist, g_variant_new_byte(COOLING_ENERGY_SAVING_MODE));
        } else if (!strcasecmp(argv[1], "2")) {
            inputlist = g_slist_append(inputlist, g_variant_new_byte(COOLING_LOW_NOISE_MODE));
        } else if (!strcasecmp(argv[1], "3")) {
            inputlist = g_slist_append(inputlist, g_variant_new_byte(COOLING_HIGH_PERFORMANCE_MODE));
        } else {
            
            g_printf("Unsupported option.\r\n");
            
            g_printf("Usage: ipmcset -l fantray -d smartcooling -v MODE\r\n");
            g_printf("\tMODE:\r\n");
            g_printf("\t1:Energy saving\r\n");
            g_printf("\t2:Low noise\r\n");
            g_printf("\t3:High performance\r\n");
            return RET_ERR;
        }
        ret = uip_call_class_method(OPERATOR_LOG_CLI, username, ip, COOLINGCLASS, CLASS_NAME_COOLING_APP,
            METHOD_SMART_COOLING_MODE, inputlist, NULL);
        uip_free_gvariant_list(inputlist);
        if (ret != RET_OK) {
            g_printf("Set controlmode fail.\r\n");
        } else {
            g_printf("Success.\n");
        }
    }

    return TRUE;
}


LOCAL gint32 set_fan_speed_by_slot(const char *username, const char *ip, int slot, int speed)
{
    int ret;
    OBJ_HANDLE obj_handle = 0;
    guint8 presence = 0;

    ret = dal_get_specific_object_byte(CLASS_NAME_SMM_FAN, PROPERTY_FAN_SLOT, slot, &obj_handle);
    if (ret != DFL_OK) {
        return RET_ERR;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_FAN_PRESENT, &presence);
    if (ret != DFL_OK) {
        return RET_ERR;
    }
    if (presence != BLADE_PRESENCE) {
        g_printf("Fantray%d is absent.\n", slot); 
        return RET_ERR;
    }

    return set_fan_speed(username, ip, slot, speed);
}


gint32 smm_set_fan_speed(guchar priv, const gchar *role, guchar argc, gchar **argv, gchar *target)
{
    gint32 speed = 0;
    glong slot = 0;
    gint32 ret;
    guint8 mode = 0;
    OBJ_HANDLE obj_handle = 0;
    gint32 max_fan_slot = get_max_fan_slot();
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    guint8 min_fan_speed = FAN_MIN_SPEED;

    if (ipmc_privilege_judge(priv, role) == FALSE) {
        g_printf("Permission denied.\r\n");
        return FALSE;
    }

    // check mode
    ret = dal_get_object_handle_nth(COOLINGCLASS, 0, &obj_handle);
    if (ret != RET_OK) {
        g_printf("Connection to Cooling module fail.\r\n");
        return FALSE;
    }

    ret = dal_get_property_value_byte(obj_handle, COOLING_PROPERTY_MODE, &mode);
    if (ret != RET_OK) {
        g_printf("Get cooling mode fail.\r\n");
        return FALSE;
    }
    if (mode == AUTO_FAN_MODE) {
        g_printf("Set fan speed fail. Current fan-ctrl mode: Auto.\r\n");
        return FALSE;
    }

    (void)dal_get_property_value_byte(obj_handle, COOLING_PROPERTY_DFTLEVEL, &min_fan_speed);

    if (argc != 2) {
        g_printf("Usage: ipmcset -l <fantray|fantrayN> -d fanspeed -v speed(%d~100)\r\n", min_fan_speed);
        return FALSE;
    }
    if (strcmp("-v", argv[0])) {
        g_printf("Usage: ipmcset -l <fantray|fantrayN> -d fanspeed -v speed(%d~100)\r\n", min_fan_speed);
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    ret = vos_str2int(argv[1], 10, &speed, NUM_TPYE_INT32);
    if (ret != RET_OK) {
        g_printf("Invalid speed(%d~100).\r\n", min_fan_speed);
        return FALSE;
    }
    if (speed > FAN_MAX_SPEED || speed < min_fan_speed) {
        g_printf("Invalid speed(%d~100).\r\n", min_fan_speed);
        return FALSE;
    }

    // Set shelf.
    if (!strcmp(target, "fantray")) {
        if (set_shelf_fan_speed(username, ip, speed, max_fan_slot)) {
            return FALSE;
        } else {
            g_printf("Success.\r\n");
            return TRUE;
        }
        // Set fantray.
    } else if (!strncmp(target, "fantray", strlen("fantray"))) {
        if (str_to_long(&slot, target + strlen("fantray"), 1, max_fan_slot)) {
            g_printf("Invalid fantray.\r\n");
            return FALSE;
        }
        if (set_fan_speed_by_slot(username, ip, slot, speed)) {
            g_printf("Failed to set fantray speed.\r\n");
            return FALSE;
        } else {
            g_printf("Success.\r\n");
            return TRUE;
        }
    } else {
        g_printf("Parameter error.\r\n");
        return FALSE;
    }
}

gint32 smm_set_smm_cooling_medium(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location)
{
    gint32 ret;
    guint8 medium = 0;
    GSList *input = NULL;
    gchar username[PARAMETER_LEN + 1] = { 0 };
    gchar ip[PARAMETER_LEN + 1] = { 0 };

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        print_blade_cooling_medium_usage();
        return FALSE;
    }

    if (strlen(argv[1]) == 0) {
        print_blade_cooling_medium_usage();
        return RET_ERR;
    }

    ret = vos_str2int(argv[1], 10, &medium, NUM_TPYE_INT32);
    if (ret != RET_OK) {
        print_blade_cooling_medium_usage();
        return ret;
    }

    if ((medium != 0) && (medium != 1)) {
        g_printf("Set fail:parameter error.\r\n");
        print_blade_cooling_medium_usage();
        return RET_ERR;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    input = g_slist_append(input, g_variant_new_byte(medium));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, username, ip, COOLINGCLASS, COOLINGCLASS, METHOD_COOLING_SET_MEDIUM,
        input, NULL);
    if (ret != RET_OK) {
        g_printf("Set %s coolingmode failed.\r\n", location);
    } else {
        g_printf("Success.\r\n");
    }

    uip_free_gvariant_list(input);
    return TRUE;
}

LOCAL void printf_set_psu_sleep_config_helpinfo(void)
{
    g_printf("Usage:\r\n");
    g_printf("ipmcset -l shelf -d psusleepconfig -v disable\r\n");
    g_printf("ipmcset -l shelf -d psusleepconfig -v enable <value>\r\n");
    g_printf("values are:\r\n");
    g_printf("\t0\tN+1 mode\r\n\t1\tN+N mode\r\n");
}

LOCAL void print_set_powerstate_helpinfo(gchar *target)
{
    g_printf("Usage:\r\n");
    if (strncmp("blade", target, strlen("blade")) == 0) {
        g_printf("ipmcset -l bladeN -d powerstate -v <poweron|poweroff|forcepoweroff>\r\n");
    } else if (strncmp("swi", target, strlen("swi")) == 0) {
        g_printf("ipmcset -l swiN -d powerstate -v <fruid> <poweron|poweroff>\r\n");
        g_printf("fru id is 1~3.\r\n");
    }
}

LOCAL void print_set_fan_smartmode(void)
{
    g_printf("Usage: ipmcset -l fantray -d smartcooling -v MODE\r\n");
    g_printf("\tMODE:\r\n");
    g_printf("\t1:Energy saving\r\n");
    g_printf("\t2:Low noise\r\n");
    g_printf("\t3:High performance\r\n");

    return;
}


LOCAL guint32 get_max_fan_slot(void)
{
    guint32 slot = DEFAULT_MAX_FAN_SLOT;

    if (!dfl_get_object_count(CLASS_NAME_SMM_FAN, &slot)) {
        return slot;
    }

    return DEFAULT_MAX_FAN_SLOT;
}


LOCAL gint32 set_shelf_fan_speed(const gchar *username, const gchar *ip, gint32 speed, gint32 max_fan_slot)
{
    gint32 i;
    gint32 found_error = 0;

    for (i = 1; i <= max_fan_slot; i++) {
        if (set_fan_speed(username, ip, i, speed)) {
            g_printf("Failed to set fan speed to %d on slot %d.\n", speed, i);
            found_error = 1;
        }
    }

    return found_error;
}


LOCAL gint32 set_fan_speed(const gchar *username, const gchar *ip, gint32 slot, gint32 speed)
{
    GSList *input = NULL;
    gint32 ret;

    input = g_slist_append(input, g_variant_new_byte(speed));
    input = g_slist_append(input, g_variant_new_byte(slot));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, username, ip, COOLINGCLASS, COOLINGCLASS, METHOD_COOLING_SINGLE_LEVEL,
        input, NULL);
    uip_free_gvariant_list(input);
    if (ret != RET_OK) {
        g_printf("Set fan speed fail. ret:%d.\n", ret);
        return RET_ERR;
    }

    return ret;
}