

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
#include "ipmcset_alarm.h"


LOCAL guint8 get_fan_ctrl_flag(void)
{
    OBJ_HANDLE cooling_handle = 0;
    gint32 ret;
    guint8 flag = TRUE;   
    GSList *output_list = NULL;
    
    ret = dal_get_object_handle_nth(COOLINGCLASS, 0, &cooling_handle);
    if (ret != RET_OK) {
        return flag;
    }

    ret = dfl_call_class_method(cooling_handle, METHOD_COOLING_GET_FAN_CTRL_FLAG, NULL, NULL, &output_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, call "METHOD_COOLING_GET_FAN_CTRL_FLAG" method failed, ret %d", __FUNCTION__, ret);
        return flag;
    }

    flag = g_variant_get_byte((GVariant*)g_slist_nth_data(output_list, 0));
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

    return flag;
}

LOCAL gint32 check_fan_num(void)
{
    guint8 fan_num = 0;
    OBJ_HANDLE obj_handle;

    gint32 ret = dal_get_object_handle_nth(OBJ_PRODUCT_COMPONENT, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:find object fail.", __FUNCTION__);
        return RET_ERR;
    }
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_COMPOENT_FAN_NUM, &fan_num);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:get fan num fail.", __FUNCTION__);
        return RET_ERR;
    }

    return RET_OK;
}

LOCAL gboolean check_pre_condition(guchar privilege, const gchar *rolepriv)
{
    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (check_fan_control_support() != RET_OK) {
        return FALSE;
    }
    if (check_fan_num() != RET_OK) {
        return FALSE;
    }

    return TRUE;
}

LOCAL gint32 get_fanlevel_fanid(guchar argc, gchar **argv, guint32 *input_fan_level, guint32 *fanid)
{
    gint32 ret;
    GVariant *property_value = NULL;
    guint8 cooling_mode;

    
    if ((argc == 2) && (strcmp(argv[0], "-v") == IPMC_OK) && (str_to_int(argv[1]) != IPMC_ERROR)) {
        *input_fan_level = (guint32)str_to_int(argv[1]);
    } else if ((argc == 3) && (strcmp(argv[0], "-v") == IPMC_OK) && (str_to_int(argv[1]) != IPMC_ERROR) &&
        (str_to_int(argv[2]) != IPMC_ERROR)) {
        *input_fan_level = (guint32)str_to_int(argv[1]);
        *fanid = (guint32)str_to_int(argv[2]);
    } else {
        return RET_ERR;
    }
    ret = uip_get_object_property(COOLINGCLASS, COOLINGCLASS, COOLING_PROPERTY_MODE, &property_value);
    if (ret == RET_OK) {
        cooling_mode = g_variant_get_byte(property_value);
        g_variant_unref(property_value);
        if (cooling_mode == AUTO_FAN_MODE) {
            g_printf("Set fan(%d) level(%d) failed.\r\nCurrent Mode:   Auto\r\n", *fanid, *input_fan_level);
            return RET_ERR;
        }
    }
    return RET_OK;
}

LOCAL gint32 check_fanlevel_fanid_access(guint32 input_fan_level, guint32 fanid,
    guint8 max_fan_slot, guint8 min_fan_slot)
{
    GVariant *value = NULL;

    if (!(fanid == 0xff || (fanid >= min_fan_slot && fanid <= max_fan_slot))) {
        print_fanlevel_usage(min_fan_slot, max_fan_slot);
        return RET_ERR;
    }

    
    gint32 ret = uip_get_object_property(COOLINGCLASS, COOLINGCLASS, COOLING_PROPERTY_DFTLEVEL, &value);
    if (ret != RET_OK) {
        g_printf("Get fan min_level failed.\r\n");
        return ret;
    }

    guint8 min_fan_level = g_variant_get_byte(value);
    g_variant_unref(value);

    if (input_fan_level < min_fan_level) {
        g_printf("Fanlevel must not be lower than %d percent.\r\n", min_fan_level);
        return RET_ERR;
    }

    if (input_fan_level > 100) { 
        g_printf("Fanlevel must not be higher than 100 percent.\r\n");
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 ipmc_set_fanlevel_by_active_method(GSList *input_list)
{
    gint32 ret;
    OBJ_HANDLE handle = 0;

    ret = dal_get_object_handle_nth(COOLINGCLASS, 0, &handle);
    if (ret != DFL_OK) {
        g_printf("Set fan level failed, get cooling object handle fail.\r\n");
        return FALSE;
    }
    if (dal_board_comm_by_can()) {  // 使用CAN总线的产品通过CAN总线转发
        ret = dal_forward_method_to_active_by_can(handle,
            METHOD_COOLING_SINGLE_LEVEL, NULL, input_list, NULL, 500);  
    } else {
        ret = call_other_smm_method(dfl_get_object_name(handle),
            METHOD_COOLING_SINGLE_LEVEL, NULL, input_list, NULL);
    }

    return ret;
}

LOCAL gint32 check_fan_mode(guint32 fanid, guint32 input_fan_level)
{
    gint32 ret;
    guint8 fan_mode = 0;
    OBJ_HANDLE obj_handle = 0;

    (void)dfl_get_object_handle(COOLINGCLASS, &obj_handle);
    ret = dal_get_property_value_byte(obj_handle, COOLING_PROPERTY_MODE, &fan_mode);
    if (ret == RET_OK && fan_mode == AUTO_FAN_MODE) {
        g_printf("Set fan(%d) level(%d) failed.\r\nCurrent Mode:   Auto\r\n", fanid, input_fan_level);
        return RET_ERR;
    }

    return RET_OK;
}


gint32 ipmc_set_fanlevel(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    GSList *input_list = NULL;
    guint32 input_fan_level = 0;
    guint32 fanid = 0xff;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    guint8 max_fan_slot = 0;
    guint8 min_fan_slot = INVALID_DATA_BYTE;
    gint32 ret;
    if (!check_pre_condition(privilege, rolepriv)) {
        return FALSE;
    }
    get_fan_slot_range(&min_fan_slot, &max_fan_slot);
    if (get_fanlevel_fanid(argc, argv, &input_fan_level, &fanid) != RET_OK) {
        print_fanlevel_usage(min_fan_slot, max_fan_slot);
        return FALSE;
    }

    if (check_fan_mode(fanid, input_fan_level) != RET_OK) {
        return FALSE;
    }

    if (check_fanlevel_fanid_access(input_fan_level, fanid, max_fan_slot, min_fan_slot) != RET_OK) {
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }
    
    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)input_fan_level));
    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)fanid));
    if (get_fan_ctrl_flag() == FALSE) {
        operation_log("CLI", username, ip, NULL, "Redirect cmd to the other board to set fan level.");
        ret = ipmc_set_fanlevel_by_active_method(input_list);
    } else {
        ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar*)username, (const gchar*)ip,
            COOLINGCLASS, COOLINGCLASS, METHOD_COOLING_SINGLE_LEVEL, input_list, NULL);
    }
    if (ret != RET_OK) {
        g_printf("Set fan level failed.\r\n");
        uip_free_gvariant_list(input_list);
        return FALSE;
    }

    if (fanid == 0xff) {
        g_printf("Set fan level successfully.\r\n");
    } else {
        g_printf("Set fan(%d) level to (%d) successfully.\r\n", fanid, input_fan_level);
    }

    (void)get_fanmode_and_fanlevel(fanid, (guint8)input_fan_level);
    uip_free_gvariant_list(input_list);
    return TRUE;
}


LOCAL gint32 set_fanmode_by_active_board(GSList *input_list)
{
    gint32 ret;
    OBJ_HANDLE handle = 0;

    ret = dal_get_object_handle_nth(COOLINGCLASS, 0, &handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get object handle failed, ret %d", __FUNCTION__, ret);
        return ret;
    }

    if (dal_board_comm_by_can()) {  // 使用CAN总线的产品通过CAN总线转发
        ret = dal_forward_method_to_active_by_can(handle,
            METHOD_COOLING_MODE, NULL, input_list, NULL, 500);  
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: set fanmode by canbus remote method failed, ret %d", __FUNCTION__, ret);
        }
    } else {
        ret = call_other_smm_method(dfl_get_object_name(handle), METHOD_COOLING_MODE, NULL, input_list, NULL);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: set fanmode by smm remote method failed, ret %d", __FUNCTION__, ret);
        }
    }

    return ret;
}


LOCAL gint32 ipmc_set_fanmode_real(guint32 fanmode, guint32 timeout)
{
    gint32 ret;
    guint32 set_timeout = timeout;
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    OBJ_HANDLE handle = 0;

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)fanmode));
    input_list = g_slist_append(input_list, g_variant_new_uint32(timeout));
    if (get_fan_ctrl_flag() == FALSE) { 
        operation_log("CLI", username, ip, NULL, "Redirect cmd to the other board to set fan mode.");
        ret = set_fanmode_by_active_board(input_list);
    } else {
        ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar*)username, (const gchar*)ip,
            COOLINGCLASS, COOLINGCLASS, METHOD_COOLING_MODE, input_list, NULL);
    }

    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        g_printf("Set fan mode failed.\r\n");
        return FALSE;
    }

    g_printf("Set fan mode successfully.\r\n");
    g_printf("Current Mode:       %s\r\n", (fanmode == AUTO_FAN_MODE) ? "auto" : "manual");

    
    if (fanmode == MANUAL_FAN_MODE) {
        ret = dal_get_object_handle_nth(COOLINGCLASS, 0, &handle);
        if (ret != RET_OK) {
            g_printf("Get time out failed, internal error.\r\n");
            return FALSE;
        }

        ret = dal_get_property_value_uint32(handle, COOLING_PROPERTY_TIMEOUT, &timeout);
        if (ret != RET_OK) {
            g_printf("Get time out failed.\r\n");
            return FALSE;
        }
        if (get_fan_ctrl_flag() == FALSE) {
            
            g_printf("Time out    :       %u seconds\r\n", set_timeout);
        } else {
            g_printf("Time out    :       %u seconds\r\n", timeout);
        }
    }

    return TRUE;
}


gint32 ipmc_set_fanmode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    guint32 fanmode;
    guint32 timeout = 0;    

    
    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (check_fan_control_support() != RET_OK) {
        return FALSE;
    }
    

    if (argc < 2 || argc > 3) { 
        goto PRINT_HELP;
    }

    if (strcmp(argv[0], "-v")) {
        goto PRINT_HELP;
    }

    if (str_to_int(argv[1]) == IPMC_ERROR) {
        goto PRINT_HELP;
    }

    fanmode = (guint32)str_to_int(argv[1]);
    if (fanmode != AUTO_FAN_MODE && fanmode != MANUAL_FAN_MODE) {
        goto PRINT_HELP;
    }

    if (fanmode == MANUAL_FAN_MODE) {
        timeout = FAN_DEFAULT_OUTTIME;
    }

    if (argc == 3) {    
        if (fanmode != MANUAL_FAN_MODE || str_to_ul(argv[2]) == (gulong)IPMC_ERROR) { 
            goto PRINT_HELP;
        }

        timeout = (guint32)str_to_ul(argv[2]);  
        if (timeout > MAX_FAN_TIMEOUT) {
            goto PRINT_HELP;
        }

        if (timeout == 0) { 
            timeout = MAX_FAN_TIMEOUT;
        }
    }

    return ipmc_set_fanmode_real(fanmode, timeout);

PRINT_HELP:
    print_fanmode_usage();
    return FALSE;
}


gint32 ipmc_set_gracefulshutdowntimeout(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target)
{
    guint32 timeout_max = 0;
    guint32 timeout_min = 0;
    guint32 timeout_state = 0;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    GSList *input_list = NULL;
    OBJ_HANDLE payload_handle = 0;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    gint32 ret = dal_get_object_handle_nth(CLASS_CHASSISPAYLOAD, 0, &payload_handle);
    if (ret != RET_OK) {
        g_printf("Get shutdown timeout state and critical value failed.\n");
        return FALSE;
    }
    ret = dal_get_property_value_uint32(payload_handle, PROPERTY_PAYLOAD_PWROFF_TM_MAX, &timeout_max);
    if (ret != RET_OK) {
        g_printf("Get shutdown timeout state and critical value failed.\n");
        return FALSE;
    }
    ret = dal_get_property_value_uint32(payload_handle, PROPERTY_PAYLOAD_PWROFF_TM_MIN, &timeout_min);
    if (ret != RET_OK) {
        g_printf("Get shutdown timeout state and critical value failed.\n");
        return FALSE;
    }
    ret = dal_get_property_value_uint32(payload_handle, PROPERTY_PAYLOAD_PWROFF_TM_EN, &timeout_state);
    if (ret != RET_OK) {
        g_printf("Get shutdown timeout state and critical value failed.\n");
        return FALSE;
    }

    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        g_printf("Usage: ipmcset -d shutdowntimeout -v <time>\r\n"
            "Time are : \r\n"
            "    0       : Disabled shutdown timeout state\r\n"
            "    Others  : Enabled shutdown timeout state, timeout range <%u s -- %u s>\r\n",
            timeout_min, timeout_max);
        return FALSE;
    }

    gint32 timeout = str_to_int(argv[1]);
    if (timeout == IPMC_ERROR ||
        ((timeout != 0) && ((timeout_min > (guint32)timeout) || (timeout_max < (guint32)timeout)))) {
        g_printf("Usage: ipmcset -d shutdowntimeout -v <time>\r\n"
            "Time are : \r\n"
            "    0       : Disabled shutdown timeout state\r\n"
            "    Others  : Enabled shutdown timeout state, timeout range <%u s -- %u s>\r\n",
            timeout_min, timeout_max);
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    if (timeout == 0) {
        
        input_list = g_slist_append(input_list, g_variant_new_uint32(0x00));
        ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_CHASSISPAYLOAD,
            OBJECT_CHASSISPAYLOAD, METHOD_SET_PWR_OFF_TM_EN, input_list, NULL);
        uip_free_gvariant_list(input_list);
        if (ret != RET_OK) {
            g_printf("Set shutdown timeout state failed.\r\n");
            return FALSE;
        }
    } else {
        
        if (timeout_state == 0) {
            input_list = g_slist_append(input_list, g_variant_new_uint32(0x01));
            ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
                CLASS_CHASSISPAYLOAD, OBJECT_CHASSISPAYLOAD, METHOD_SET_PWR_OFF_TM_EN, input_list, NULL);
            uip_free_gvariant_list(input_list);
            input_list = NULL;
            if (ret != RET_OK) {
                g_printf("Set shutdown timeout state failed.\r\n");
                return FALSE;
            }
        }

        input_list = g_slist_append(input_list, g_variant_new_uint32((guint32)timeout));
        ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_CHASSISPAYLOAD,
            OBJECT_CHASSISPAYLOAD, METHOD_SET_PWR_OFF_TM, input_list, NULL);
        uip_free_gvariant_list(input_list);
        if (ret != RET_OK) {
            g_printf("Set shutdown timeout value failed.\r\n");
            return FALSE;
        }
    }

    g_printf("Set shutdown timeout successfully.\r\n");
    return TRUE;
}


gint32 ipmc_set_maintenance_coolingpowermode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target)
{
#define COOLING_POWER_MODE_POWER_SAVING 0
#define COOLING_POWER_MODE_HIGH_RELIABILITY 1

    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    OBJ_HANDLE object_handle = 0;
    guint8 node_mode = 0;

    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    gchar* power_dsp[2] = {"Power saving mode", "High reliability mode"};
    gchar custom_name[LOG_SPECIAL_STRING_LEN_MAX] = {"iBMC"};

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        ipmc_log_operation_log(MAINTENANCE, "Insufficient privilege");
        return FALSE;
    }

    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        print_set_maintenance_coolingpowermode_helpinfo();
        return FALSE;
    }

    gint32 option = str_to_int(argv[1]);
    if (option != COOLING_POWER_MODE_POWER_SAVING && option != COOLING_POWER_MODE_HIGH_RELIABILITY) {
        ipmc_log_operation_log(MAINTENANCE, "Invalid input");
        print_set_maintenance_coolingpowermode_helpinfo();
        return FALSE;
    }

    
    if (dal_get_custom_string_name(custom_name, BMC_CUSTOM_LOGIBMC_NAME, sizeof(custom_name)) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_custom_string_name fail.", __FUNCTION__);
    }

    
    if (dfl_get_object_handle(XMLPARTITION_OBJ_NAME, &object_handle) == DFL_OK) {
        if ((dal_get_property_value_byte(object_handle, NODE_MODE, &node_mode) == RET_OK) && (node_mode == 0)) {
            g_printf("Perform the operation on the master(HFC-2) %s.\r\n", custom_name);
            return FALSE;
        }
    }

    
    if (dal_get_object_handle_nth(COOLINGCLASS, 0, &obj_handle) != RET_OK) {
        g_printf("Set cooling power mode failed.\r\n");
        debug_log(DLOG_ERROR, "get Cooling handle failed\n");
        ipmc_log_operation_log(MAINTENANCE, "Set cooling power mode failed.");
        return FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(option));
    gint32 ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, username, ip, COOLINGCLASS, obj_handle,
        METHOD_COOLING_POWER_MODE, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);

    g_printf("Set cooling power mode to [%s] %s.\r\n", power_dsp[option], (ret != RET_OK) ? "failed" : "successfully");

    return (ret != RET_OK) ? FALSE : TRUE;
}


gint32 ipmc_set_MSPP_enable(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    guint8 enable = 0;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        g_printf("Usage: ipmcset -t powercapping -d MSPP -v <enabled|disabled>\r\n");
        return FALSE;
    }

    if (strcasecmp(argv[1], "enabled") == 0) {
        enable = SHELF_MSPP_STATUS_ENABLED;
    } else if (strcasecmp(argv[1], "disabled") == 0) {
        
        enable = SHELF_MSPP_STATUS_DISABLED;
        g_printf("WARNING: This operation may power off the server.\r\n");
        ret = check_the_input_operation("Continue?[Y/N]:");
        if (ret != RET_OK) {
            return FALSE;
        }
    } else {
        g_printf("Usage: ipmcset -t powercapping -d MSPP -v <enabled|disabled>\r\n");
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    
    input_list = g_slist_append(input_list, g_variant_new_byte(enable));

    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_NAME_MSPP,
        OBJ_NAME_MSPP, METHOD_SET_MSPP_ENABLE, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        if (ret == COMP_CODE_STATUS_INVALID) {
            g_printf("Before performing this operation, disable power capping of the chassis.\r\n");
        }

        g_printf("Set MSPP enable state failed.\r\n");
        return FALSE;
    }

    g_printf("Set MSPP state (%s) successfully.\r\n", (enable) ? "enabled" : "disabled");
    return TRUE;
}


gint32 ipmc_set_powercappinginfo_bladevalue(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target)
{
    gint32 iRet;
    gint32 ret;
    GSList *input_list = NULL;
    OBJ_HANDLE shelf_handle = 0;
    OBJ_HANDLE object_handle = 0;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    gchar obj_name[OBJ_NAME_MAX_LEN] = {0};
    gint32 blade_id = 0;
    guint32 blade_num = 0;
    gint32 value = 0;
    guint8 mode = 0;
    guint8 presence = 0;
    guint16 min_value = 0;
    guint16 max_value = 0;
    OBJ_HANDLE blade_handle = 0;
    GSList *handle_list = NULL;
    GSList *list_item = NULL;
    guchar MgmtSoftWareType = 0;
    (void)dal_get_software_type(&MgmtSoftWareType);

    if ((setcommand_privilege_judge(privilege, rolepriv) == FALSE)) {
        return FALSE;
    }

    
    
    ret = dfl_get_object_list(CLASS_NAME_IPMBETH_BLADE, &handle_list);
    if (ret != DFL_OK) {
        g_printf("Get blade number failed.\r\n ");
        return FALSE;
    }
    for (list_item = handle_list; list_item; list_item = g_slist_next(list_item)) {
        object_handle = (OBJ_HANDLE)list_item->data;
        if (is_compute_node(object_handle) == FALSE) {
            continue;
        }
        blade_num++;
    }
    g_slist_free(handle_list);
    

    if ((argc != 3) || (strcmp(argv[0], "-v"))) {
        g_printf("Usage: ipmcset %s-t powercapping -d bladevalue -v <blade(1~%d)> <value>\r\n", need_l_shelf,
            blade_num);
        return FALSE;
    }

    if ((blade_id = str_to_int(argv[1])) == IPMC_ERROR) {
        g_printf("Usage: ipmcset %s-t powercapping -d bladevalue -v <blade(1~%d)> <value>\r\n", need_l_shelf,
            blade_num);
        return FALSE;
    }

    if (((guint32)blade_id > blade_num) || (blade_id < 1)) {
        g_printf("Usage: ipmcset %s-t powercapping -d bladevalue -v <blade(1~%d)> <value>\r\n", need_l_shelf,
            blade_num);
        return FALSE;
    }

    if ((value = str_to_int(argv[2])) == IPMC_ERROR) {
        g_printf("Usage: ipmcset %s-t powercapping -d bladevalue -v <blade(1~%d)> <value>\r\n", need_l_shelf,
            blade_num);
        return FALSE;
    }

    

    
    ret = dal_get_object_handle_nth(CLASS_NAME_SHELF_POWER_CAPPING, 0, &shelf_handle);
    if (ret != RET_OK) {
        g_printf("Set blade power capping value failed.\r\n ");
        return FALSE;
    }

    ret = dal_get_property_value_byte(shelf_handle, PROTERY_SHELF_POWER_CAPPING_MODE, &mode);
    if (ret != RET_OK) {
        g_printf("Set blade power capping value failed.\r\n ");
        return FALSE;
    }

    
    if (mode != SHELF_PWRCAP_MODE_MODE_MANUAL) {
        g_printf("Set blade power capping value failed : Shelf power capping mode should be manual.\n");
        return FALSE;
    }

    
    
    
    (void)dal_get_specific_object_byte(CLASS_NAME_IPMBETH_BLADE, PROPERTY_IPMBETH_BLADE_ID, blade_id, &blade_handle);
    iRet = snprintf_s(obj_name, OBJ_NAME_MAX_LEN, OBJ_NAME_MAX_LEN - 1, "%s", dfl_get_object_name(blade_handle));
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    

    ret = dfl_get_object_handle(obj_name, &object_handle);
    if (ret != DFL_OK) {
        g_printf("Set blade power capping value failed.\r\n ");
        return FALSE;
    }

    
    ret = dal_get_property_value_byte(object_handle, PROTERY_IPMBETH_BLADE_PRESENCE, &presence);
    if (ret != RET_OK) {
        g_printf("Set blade power capping value failed.\r\n ");
        return FALSE;
    }

    if ((presence != BLADE_PRESENCE)) {
        g_printf("Set blade power capping value failed : Blade is not present.\n");
        return FALSE;
    }

    
    ret = get_maximum_blade_power_capping_value(object_handle, &max_value);
    
    if (MgmtSoftWareType != MGMT_SOFTWARE_TYPE_EM) {
        if (ret != RET_OK) {
            g_printf("Set blade power capping value failed.\r\n ");
            return FALSE;
        }
    } else {
        if (ret != RET_OK) {
            if (max_value & 0x8000) {
                g_printf("There is no remaining power capping value for the blade.\r\n ");
            } else {
                g_printf("Set blade power capping value failed.\r\n ");
            }
            return FALSE;
        }
    }
    

    
    if (max_value > 9999) {
        max_value = 9999; 
    }

    ret = dal_get_property_value_uint16(object_handle, PROTERY_IPMBETH_POWER_CAPPING_BASE_VALUE, &min_value);
    if (ret != RET_OK) {
        g_printf("Set blade power capping value failed.\r\n ");
        return FALSE;
    }

    
    if (MgmtSoftWareType != MGMT_SOFTWARE_TYPE_EM) {
        if ((min_value > max_value)) {
            g_printf("Set blade power capping value failed : The remaining power is not enough.\r\n");
            return FALSE;
        }

        
        if ((value != 0) && ((value > max_value) || (value < min_value))) {
            g_printf("Set blade power capping value failed : Range from [%d, %d]\r\n", min_value, max_value);
            return FALSE;
        }
    } else {
        
        if (value > max_value) {
            if (min_value > max_value) {
                g_printf("Set blade power capping value failed. The max remaining capping power is %d Watts.\r\n",
                    max_value);
            } else {
                g_printf("Set blade power capping value failed : Range from [%d, %d].\r\n", min_value, max_value);
            }
            return FALSE;
        }

        if ((value != 0) && (min_value <= max_value) && (value < min_value)) {
            g_printf("Reference range from [%d, %d].\r\n", min_value, max_value);
            g_printf("The value for Blade Power Cap is lower than the lower limit of the range, power capping may "
                "fail.\r\n");
        } else if ((value != 0) && (min_value > max_value) && (value <= max_value)) {
            g_printf("The value is lower than the lower limit(%d) of the blade, power capping may fail.\r\n",
                min_value);
        }
        
        ret = is_continue_operate();
        if (ret != RET_OK) {
            return FALSE;
        }
    }
    
    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    
    input_list = g_slist_append(input_list, g_variant_new_uint16((guint16)value));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_NAME_IPMBETH_BLADE,
        obj_name, METHOD_SET_BLADE_POWER_CAPPING_VALUE, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        g_printf("Set blade power capping value failed.\r\n ");
        return FALSE;
    }

    g_printf("Set blade power capping value successfully.\r\n");

    return TRUE;
}


gint32 ipmc_set_powercappinginfo_enable(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target)
{
    gint32 ret;
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    guint8 enable = 0;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        g_printf("Usage: ipmcset %s-t powercapping -d enable -v <enabled|disabled>\r\n", need_l_shelf);
        return FALSE;
    }

    if (strcasecmp(argv[1], "enabled") == 0) {
        enable = SHELF_PWRCAP_STATUS_ENABLED;
    } else if (strcasecmp(argv[1], "disabled") == 0) {
        enable = SHELF_PWRCAP_STATUS_DISABLED;
    } else {
        g_printf("Usage: ipmcset %s-t powercapping -d enable -v <enabled|disabled>\r\n", need_l_shelf);
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    
    input_list = g_slist_append(input_list, g_variant_new_byte(enable));

    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
        CLASS_NAME_SHELF_POWER_CAPPING, OBJ_NAME_SHELF_POWER_CAPPING, METHOD_SET_SHELF_POWER_CAPPING_ENABLE, input_list,
        NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        g_printf("Set shelf power capping enable state failed.\r\n");
        return FALSE;
    }

    g_printf("Set shelf power capping enable state (%s) successfully.\r\n", (enable) ? "enabled" : "disabled");
    return TRUE;
}


gint32 ipmc_set_powercappinginfo_mode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    guint8 mode = 0;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        g_printf("Usage: ipmcset %s-t powercapping -d mode -v <value>\r\n", need_l_shelf);
        g_printf("Values are:\r\n");
        g_printf("\t0\tEqual\r\n\t1\tManual\r\n\t2\tProportion\r\n");
        return FALSE;
    }

    if (strcasecmp(argv[1], "0") == 0) {
        mode = SHELF_PWRCAP_MODE_EQUAL;
    } else if (strcasecmp(argv[1], "1") == 0) {
        mode = SHELF_PWRCAP_MODE_MODE_MANUAL;
    } else if (strcasecmp(argv[1], "2") == 0) {
        mode = SHELF_PWRCAP_MODE_MODE_PROPORTION;
    } else {
        g_printf("Usage: ipmcset %s-t powercapping -d mode -v <value>\r\n", need_l_shelf);
        g_printf("Values are:\r\n");
        g_printf("\t0\tEqual\r\n\t1\tManual\r\n\t2\tProportion\r\n");
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    
    input_list = g_slist_append(input_list, g_variant_new_byte(mode));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
        CLASS_NAME_SHELF_POWER_CAPPING, OBJ_NAME_SHELF_POWER_CAPPING, METHOD_SET_SHELF_POWER_CAPPING_MODE, input_list,
        NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        g_printf("Set shelf power capping mode failed.\r\n");
        return FALSE;
    }

    g_printf("Set shelf power capping mode (%s) successfully.\r\n",
        (mode == 0) ? "equal" : ((mode == 1) ? "manual" : "proportion"));
    return TRUE;
}


gint32 ipmc_set_powercappinginfo_value(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target)
{
    gint32 ret;
    GSList *input_list = NULL;
    OBJ_HANDLE object_handle = 0;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    gint32 value = 0;
    guint16 min_value = 0;
    guint16 max_value = 0;
    guint16 actual_min_value = 0; 
    guchar MgmtSoftWareType = 0;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        g_printf("Usage: ipmcset %s-t powercapping -d shelfvalue -v <value>\r\n", need_l_shelf);
        return FALSE;
    }

    if ((value = str_to_int(argv[1])) == IPMC_ERROR) {
        g_printf("Usage: ipmcset %s-t powercapping -d shelfvalue -v <value>\r\n", need_l_shelf);
        return FALSE;
    }

    
    
    ret = dal_get_object_handle_nth(CLASS_NAME_POWERCAPPING, 0, &object_handle);
    if (ret != RET_OK) {
        g_printf("Set shelf power capping value failed.\r\n");
        return FALSE;
    }

    ret = dal_get_sys_all_power_rate(object_handle, &max_value);
    if (ret != RET_OK) {
        g_printf("Set shelf power capping value failed.\r\n");
        return FALSE;
    }

    
    ret = get_minimum_power_capping_value(&min_value, &actual_min_value);
    if (ret != RET_OK) {
        g_printf("Set shelf power capping value failed.\r\n");
        return FALSE;
    }

    
    (void)dal_get_software_type(&MgmtSoftWareType);
    if (MgmtSoftWareType == MGMT_SOFTWARE_TYPE_EM) {
        min_value = actual_min_value;
    }
    

    if (min_value > max_value) {
        g_printf("Set shelf power capping value failed. SysPower(%d) does not satisfy the minimum demand for shelf "
            "power capping : (%d).\r\n",
            max_value, min_value);
        return FALSE;
    }

    
    if (MgmtSoftWareType != MGMT_SOFTWARE_TYPE_EM) {
        if ((value > max_value) || (value < min_value)) {
            g_printf("Set shelf power capping value failed. Range from [%d, %d].\r\n", min_value, max_value);
            g_printf("The lower limit for Shelf Power Cap is the minimum value allowed. However, if the servers are "
                "equipped with high-power PCIe devices such as GPUs and SSDs and Shelf Power Cap is set to a "
                "value close to the lower limit, power capping may fail.\r\n");
            return FALSE;
        }

        g_printf("The value for Shelf Power Cap is in the range of %d-%d.\r\n", min_value, max_value);
        g_printf("The lower limit for Shelf Power Cap is the minimum value allowed. However, if the servers are "
            "equipped with high-power PCIe devices such as GPUs and SSDs and Shelf Power Cap is set to a value "
            "close to the lower limit, power capping may fail.\r\n");
    } else {
        ret = ipmc_set_pcinfo_value_check_value(value, actual_min_value, min_value, max_value);
        
        if (ret != TRUE) {
            return FALSE;
        }
    }
    

    
    ret = is_continue_operate();
    if (ret != RET_OK) {
        return FALSE;
    }

    
    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    
    input_list = g_slist_append(input_list, g_variant_new_uint16((guint16)value));

    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
        CLASS_NAME_SHELF_POWER_CAPPING, OBJ_NAME_SHELF_POWER_CAPPING, METHOD_SET_SHELF_POWER_CAPPING_VALUE, input_list,
        NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        g_printf("Set shelf power capping value failed.\r\n");
        return FALSE;
    }

    g_printf("Set shelf power capping value successfully.\r\n");

    return TRUE;
}


gint32 ipmc_set_powercappinginfo_threshold(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target)
{
    gint32 ret;
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    gint32 threshold = 0;
    OBJ_HANDLE object_handle = 0;
    guint8 max_threshold = 0;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        g_printf("Usage: ipmcset %s-t powercapping -d threshold -v <value>\r\n", need_l_shelf);
        return FALSE;
    }

    if ((threshold = str_to_int(argv[1])) == IPMC_ERROR) {
        g_printf("Usage: ipmcset %s-t powercapping -d threshold -v <value>\r\n", need_l_shelf);
        return FALSE;
    }

    
    
    ret = dal_get_object_handle_nth(CLASS_NAME_SHELF_POWER_CAPPING, 0, &object_handle);
    if (ret != RET_OK) {
        g_printf("Set shelf power capping threshold failed.\r\n ");
        return FALSE;
    }

    ret = dal_get_property_value_byte(object_handle, PROTERY_SHELF_POWER_CAPPING_MAX_THRESHOLD, &max_threshold);
    if (ret != RET_OK) {
        g_printf("Set shelf power capping threshold failed.\r\n ");
        return FALSE;
    }

    if ((threshold > max_threshold) || (threshold < 0)) {
        g_printf("Set shelf power capping threshold failed. Range from [0, %d].\r\n", max_threshold);
        return FALSE;
    }

    

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    
    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)threshold));

    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
        CLASS_NAME_SHELF_POWER_CAPPING, OBJ_NAME_SHELF_POWER_CAPPING, METHOD_SET_SHELF_POWER_CAPPING_THRESHOLD,
        input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        g_printf("Set shelf power capping threshold failed.\r\n ");
        return FALSE;
    }

    g_printf("Set shelf power capping threshold successfully.\r\n");
    return TRUE;
}


gint32 ipmc_set_poweronpermit_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 iRet;
    errno_t safe_fun_ret;
    gint32 ret;
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar userip[PARAMETER_LEN + 1] = {0};
    guchar state = 0;
    guint32 port;
    const gchar *tmpip = NULL;
    gchar ip[PARAMETER_LEN + 1] = {0};
    struct in_addr ipaddr;
    OBJ_HANDLE obj_handle;
    GSList *obj_list = NULL;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;
    gchar poweronstring[256] = {0};
    gchar tmp_autopoweron;
    GVariant *tmp_value = NULL;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if ((argc < 2) || ((argv[0]) && strcmp(argv[0], "-v"))) {
        g_printf("Usage: ipmcset -d poweronpermit -v enable [ip] [port]\r\n");
        g_printf("       ipmcset -d poweronpermit -v disable\r\n");
        return FALSE;
    }

    if (argv[1] == NULL) {
        g_printf("Usage: ipmcset -d poweronpermit -v enable [ip] [port]\r\n");
        g_printf("       ipmcset -d poweronpermit -v disable\r\n");
        return FALSE;
    }

     
    ret = dfl_get_object_list(CLASS_STATELESS, &obj_list);
    if (ret != DFL_OK || obj_list == NULL) {
        g_printf("Get object failed.\n");
        return FALSE;
    }

    
    obj_handle = (OBJ_HANDLE)g_slist_nth_data(obj_list, 0);
    g_slist_free(obj_list);

    // 先读取原来的值。
    
    property_name_list = g_slist_append(property_name_list, PROPERTY_STATELESS_SYS_MANAGER_IP);
    property_name_list = g_slist_append(property_name_list, PROPERTY_STATELESS_SYS_MANAGER_PORT);
    ret = dfl_multiget_property_value(obj_handle, property_name_list, &property_value);
    if (ret != DFL_OK) {
        g_printf("Get poweronpermit information failed.\n");
        g_slist_free(property_name_list);
        return FALSE;
    }

    tmpip = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 0), 0);
    port = g_variant_get_uint32((GVariant *)g_slist_nth_data(property_value, 1));
    
    safe_fun_ret = strncpy_s(ip, sizeof(ip), tmpip, sizeof(ip) - 1);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    

    g_slist_free(property_name_list);
    uip_free_gvariant_list(property_value);

    // 处理enable,
    if (strcmp(argv[1], "enable") == 0) {
        state = TRUE;
    } else if (strcmp(argv[1], "disable") == 0) {
        state = FALSE;
    } else {
        g_printf("Usage: ipmcset -d poweronpermit -v enable [ip] [port]\r\n");
        g_printf("       ipmcset -d poweronpermit -v disable\r\n");
        return FALSE;
    }

    
    // 已经disable，则不需要继续检查了
    if (dfl_get_property_value(obj_handle, PROPERTY_STATELESS_AUTO_POWER_ON, &tmp_value) != DFL_OK) {
        g_printf("Get poweronpermit information failed.\n");
        return FALSE;
    }
    tmp_autopoweron = g_variant_get_byte(tmp_value);
    g_variant_unref(tmp_value);

    if ((argc >= 3) && (state == FALSE)) {
        g_printf("Usage: ipmcset -d poweronpermit -v enable [ip] [port]\r\n");
        g_printf("       ipmcset -d poweronpermit -v disable\r\n");
        return FALSE;
    }

    if ((tmp_autopoweron == TRUE) && (state == FALSE)) {
        g_printf("Disable poweronpermit successfully.\r\n");
        return TRUE;
    }
    

    // 处理ip
    if (argc >= 3) {
        if (inet_pton(AF_INET, (const gchar *)argv[2], (void *)&ipaddr.s_addr) != 1) {
            g_printf("Input parameter format error.\r\n");
            return FALSE;
        }
        
        safe_fun_ret = strncpy_s(ip, sizeof(ip), argv[2], sizeof(ip) - 1);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        
    }

    

    if ((state == TRUE) && (ip[0] == 0)) {
        g_printf("Please input ip address.\r\n");
        return FALSE;
    }
    

    // 处理port
    if (argc >= 4) {
        port = (guint32)str_to_int(argv[3]);
        if ((port > 65535) || (port < 1)) {
            g_printf("Input parameter format error.\r\n");
            return FALSE;
        }
    }

    

    if (argc > 4) {
        g_printf("Usage: ipmcset -d poweronpermit -v enable [ip] [port]\r\n");
        g_printf("       ipmcset -d poweronpermit -v disable\r\n");
        return FALSE;
    }

    
    if (get_username_ip(username, userip, sizeof(userip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "$Supervisor");
        snprintf_s(userip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "IPC");
        
    }

    // 写入
    
    iRet = snprintf_s(poweronstring, sizeof(poweronstring), sizeof(poweronstring) - 1, "%s,%u,%u", ip, port, state);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    
    input_list = g_slist_append(input_list, g_variant_new_string(poweronstring));
    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)userip,
        CLASS_STATELESS, obj_handle, METHOD_STATELESS_SET_POWER_ON_CONTROL_STRING, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        g_printf("Set poweronpermit failed.\r\n");
        return FALSE;
    }

    g_printf("Set poweronpermit successfully.\r\n");

    return TRUE;
}


// 大函数拆分
LOCAL void err_info_print(gint32 ret, gchar *target, gchar *oper_string)
{
    if (ret == IPMC_UNSUPPORT) {
        g_printf("Set %s Power Control(%s)failed: Command is not supported in present state.\r\n",
            ((target == NULL) ? "fru0" : target), oper_string);
        return;
    }
    
    else if (ret == POWER_ON_FAILED_BY_BIOS_UP) {
        g_printf("Set %s Power Control(%s)failed: BIOS is being upgraded. Please wait.\r\n",
            ((target == NULL) ? "fru0" : target), oper_string);
        return;
    } else if (ret == POWER_ON_FAILED_BY_CPLD_UP) {
        g_printf("Set %s Power Control(%s)failed: CPLD is being upgraded. Please wait.\r\n",
            ((target == NULL) ? "fru0" : target), oper_string);
        return;
    } else if (ret == POWER_ON_FAILED_BY_VRD_UP) {
        g_printf("Set %s Power Control(%s)failed: VRD is being upgraded. Please wait.\r\n",
            ((target == NULL) ? "fru0" : target), oper_string);
        return;
    }
    

    g_printf("ret %d, Set %s Power Control(%s)failed.\r\n", ret, ((target == NULL) ? "fru0" : target), oper_string);
}



gint32 ipmc_set_powerstate(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    gchar oper_string[PARAMETER_LEN + 1] = {0};
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        print_power_state_usage();
        return FALSE;
    }

    gint32 data = str_to_int(argv[1]);
    if (data == IPMC_ERROR || ((data != 0) && (data != 1) && (data != 2))) {
        print_power_state_usage();
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    
    ret = is_continue_powerstate_operate(data);
    if (ret != RET_OK) {
        return FALSE;
    }

    
    if (data == 0) {
        ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_CHASSISPAYLOAD,
            OBJECT_CHASSISPAYLOAD, METHOD_PWR_OFF, NULL, NULL);
        
        snprintf_s(oper_string, PARAMETER_LEN + 1, PARAMETER_LEN, "normal power off");
    } else if (data == 1) {
        ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_CHASSISPAYLOAD,
            OBJECT_CHASSISPAYLOAD, METHOD_PWR_ON, NULL, NULL);
        snprintf_s(oper_string, PARAMETER_LEN + 1, PARAMETER_LEN, "power on");
    } else {
        ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_CHASSISPAYLOAD,
            OBJECT_CHASSISPAYLOAD, METHOD_FORCE_PWR_OFF, NULL, NULL);
        snprintf_s(oper_string, PARAMETER_LEN + 1, PARAMETER_LEN, "forced power off");
        
    }

    if (ret != RET_OK) {
        (void)err_info_print(ret, target, oper_string);
        return FALSE;
    }

    g_printf("Control %s %s successfully.\r\n", ((target == NULL) ? "fru0" : target), oper_string);
    

    return TRUE;
}


gint32 ipmc_set_psu_supply_source(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    gint32 ret_code = RET_ERR;
    guint8 supply = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    const gint32 expected_argc_num = 2;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    // 若命令行的输入参数不为两个，需打印出帮助信息
    if (argc != expected_argc_num || strcmp(argv[0], "-v") != 0) {
        print_set_psu_supply_source_usage();
        return FALSE;
    }
    ret = vos_str2int(argv[1], BASE_10, &supply, NUM_TPYE_UCHAR);
    if (ret != RET_OK) {
        print_set_psu_supply_source_usage();
        return FALSE;
    }

    if ((supply != PSU_SUPPLY_MAIN) && (supply != PSU_SUPPLY_BACKUP)) {
        print_set_psu_supply_source_usage();
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }
    input_list = g_slist_append(input_list, g_variant_new_byte(supply));

    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_NAME_AMMETER,
        OBJ_NAME_AMMETER, METHOD_SET_PSU_SUPPLY_SOURCE_CFG, input_list, &output_list);

    uip_free_gvariant_list(input_list);

    if (output_list != NULL) {
        ret_code = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
        uip_free_gvariant_list(output_list);
    }

    if (ret != RET_OK || ret_code != RET_OK) {
        g_printf("Failed to deliver the configuration for switching the power supply source.\r\n");
        return FALSE;
    }

    g_printf(
        "The configuration for switching the power supply source is successfully delivered. Please note that:\r\n");
    g_printf("a) The switchover takes 1 to 5 minutes. You can learn about the switchover result from the command "
        "output.\r\n");
    g_printf("b) The switchover configuration needs to be manually cleared. For details, see the operation guide.\r\n");

    return TRUE;
}


LOCAL guint8 get_psu_ctrl_flag(void)
{
    guint8 as_status = DEFAULT_AS_STATUS;
    guint8 ps_manage_type = 0xFF;
    OBJ_HANDLE syspower_handle = 0;

    if (dal_board_comm_by_can()) { 
        return TRUE;
    }

    (void)dal_get_object_handle_nth(CLASS_NAME_AMMETER, 0, &syspower_handle);
    (void)dal_get_property_value_byte(syspower_handle, PROPERTY_SYSPOWER_PS_MANAGE_HOST_TYPE, &ps_manage_type);
    if (ps_manage_type == 1) { 
        (void)get_board_active_state(&as_status);
        if (as_status == STANDBY_STATE) {
            return FALSE;
        }
    }
    return TRUE;
}


LOCAL gboolean psu_asm_psid_invalid(guchar argc, gchar **argv, guint8 max_ps_num)
{
    guint8 min = 1;
    guint8 max = max_ps_num;
    if (dal_match_product_id(PRODUCT_ID_PANGEA_V6)) { 
        min--;
        max--;
    }
    for (guint8 i = 2; i < argc; i++) { 
        if ((guint32)str_to_ul(argv[i]) > max ||
            (guint32)str_to_ul(argv[i]) < min) {
            return TRUE;
        }
    }

    return FALSE;
}


LOCAL gint32 set_psu_workmode_check_access(guchar argc, gchar **argv, guint8 max_ps_num)
{
    guint8 deep_sleep_enable = 0;
    OBJ_HANDLE service_handle = 0;

    
    (void)dal_get_object_handle_nth(CLASS_NAME_PME_SERVICECONFIG, 0, &service_handle);
    (void)dal_get_property_value_byte(service_handle, PROTERY_PME_SERVICECONFIG_DEEP_SLEEP_ENABLE, &deep_sleep_enable);
    

    
    if (argc == 2) {
        
        if ((strcmp(argv[1], "0") != 0)) {
            print_psu_work_mode(max_ps_num);
            return FALSE;
        }
    } else if (argc == 3) {
        // 操作码为1，参数2超出范围(psid等于0或大于最大电源数量)
        if (strcmp(argv[1], "1") == 0) {
            if (psu_asm_psid_invalid(argc, argv, max_ps_num)) {
                print_psu_work_mode(max_ps_num);
                return FALSE;
            }
        }
        
        // 支持深度休眠且操作码为2，参数为enable和disable
        else if ((deep_sleep_enable == DEEP_SLEEP_MODE_ENABLED) && (strcmp(argv[1], "2") == 0)) {
            if ((strcmp(argv[2], "enabled") != 0) && (strcmp(argv[2], "disabled") != 0)) {
                print_psu_work_mode(max_ps_num);
                return FALSE;
            }
        }
        
        // 其他的不支持
        else {
            print_psu_work_mode(max_ps_num);
            return FALSE;
        }
    } else { 
        if ((strcmp(argv[1], "1") != 0) || psu_asm_psid_invalid(argc, argv, max_ps_num)) {
            print_psu_work_mode(max_ps_num);
            return FALSE;
        }
    }

    return TRUE;
}


LOCAL gint32 set_psu_deepsleep_mode(gchar **argv, const gchar* username, const gchar* ip)
{
    guint8 state = 0;
    GSList *input_list = NULL;
    gint32 ret;

    
    if (strcmp(argv[2], "enabled") == 0) {
        state = 1;
    } else {
        state = 0;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(state));
    if (get_psu_ctrl_flag() == FALSE) {
        operation_log("CLI", username, ip, NULL, "Redirect cmd to the other board to set PSU work mode.");
        ret = call_other_smm_method(OBJ_NAME_AMMETER, METHOD_SET_DEEP_SLEEP_MODE, NULL, input_list, NULL);
    } else {
        ret = uip_call_class_method(OPERATOR_LOG_CLI, username, ip,
            CLASS_NAME_AMMETER, OBJ_NAME_AMMETER, METHOD_SET_DEEP_SLEEP_MODE, input_list, NULL);
    }

    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        g_printf("%s Power Work Mode Hibernate failed.\r\n",
            (state == DEEP_SLEEP_MODE_ENABLED) ? "Enable" : "Disable");
        return FALSE;
    }

    g_printf("%s Power Work Mode Hibernate successfully.\r\n",
        (state == DEEP_SLEEP_MODE_ENABLED) ? "Enable" : "Disable");
    return TRUE;
}


LOCAL gint32 set_psu_work_mode(guchar argc, gchar **argv, guint32 workmode,
    gchar* username, gchar* ip)
{
    GSList *input_list = NULL;
    gint32 ret;

    input_list = g_slist_append(input_list, g_variant_new_byte(workmode));
    guint8 nar_state = get_powercontrol_normal_and_redundancy();
    if (nar_state == ENABLED) {
        uip_free_gvariant_list(input_list);
        g_printf("The normal and redundancy is enabled through the Redfish interface, this mode cannot be set.\r\n");
        return FALSE;
    }
    input_list = g_slist_append(input_list, g_variant_new_byte(nar_state));
    if (workmode == POWER_CFG_MASTER_SPARE) {
        for (guint8 i = 2; i < argc; i++) { 
            guint32 active_psid = (guint32)str_to_ul(argv[i]);
            if (!dal_match_product_id(PRODUCT_ID_PANGEA_V6) && active_psid > 0) {
                active_psid--;
            }
            input_list = g_slist_append(input_list, g_variant_new_byte((guint8)active_psid));
        }
    }

    if (get_psu_ctrl_flag() == FALSE) {
        operation_log("CLI", username, ip, NULL, "Redirect cmd to the other board to set PSU work mode.");
        ret = call_other_smm_method(OBJ_NAME_AMMETER, METHOD_SET_PLANTFORM_WORKMODE, NULL, input_list, NULL);
    } else {
        ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar*)username, (const gchar*)ip,
            CLASS_NAME_AMMETER, OBJ_NAME_AMMETER, METHOD_SET_PLANTFORM_WORKMODE, input_list, NULL);
    }
    uip_free_gvariant_list(input_list);

    if (ret != RET_OK) {
        print_set_psu_workmode_errinfo(ret);
        g_printf("Set Power Work Mode (%s) failed.\r\n",
            (workmode == POWER_CFG_LOAD_BALANCE) ? "Load Balancing" : "Active Standby");

        return FALSE;
    }

    g_printf("Set Power Work Mode (%s) successfully.\r\n",
        (workmode == POWER_CFG_LOAD_BALANCE) ? "Load Balancing" : "Active Standby");
    return TRUE;
}


gint32 ipmc_set_psu_workmode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    guint8 max_ps_num = 0;

    (void)get_component_ps_max_num(&max_ps_num);

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if ((argc < 2) || strcmp(argv[0], "-v")) { 
        print_psu_work_mode(max_ps_num);
        return FALSE;
    }

    if (set_psu_workmode_check_access(argc, argv, max_ps_num) != TRUE) {
        return FALSE;
    }

    guint32 workmode = (guint32)str_to_ul(argv[1]);

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    if (workmode == 2) {
        return set_psu_deepsleep_mode(argv, username, ip);
    }

    return set_psu_work_mode(argc, argv, workmode, username, ip);
}


gint32 ipmc_set_pwd_minimum_age(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    guint32 valid_days = 0;
    guint32 minimum_age = 0;
    gint32 value = 0;

    gint32 ret = 0;
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        ipmc_log_operation_log("SecurityEnhance", "Insufficient privilege");
        return FALSE;
    }

    

    (void)dal_get_object_handle_nth(OBJ_SECURITY_ENHANCE, 0, &obj_handle);

    (void)dal_get_property_value_uint32(obj_handle, PROPERTY_PWD_EXPIRED_TIME, &valid_days);

    if (argc != 2 || strcmp(argv[0], "-v")) {
        print_set_pwd_minimum_age_usage(valid_days);
        return FALSE;
    }

    if ((value = str_to_int(argv[1])) == IPMC_ERROR) {
        debug_log(DLOG_INFO, "%s: input error", __FUNCTION__);
        print_set_pwd_minimum_age_usage(valid_days);
        return FALSE;
    }

    if (valid_days == 0) {
        if ((value < 0) || (value > 365)) {
            
            print_set_pwd_minimum_age_usage(valid_days);
            debug_log(DLOG_INFO, "%s: input value range error %d", __FUNCTION__, value);
            return FALSE;
            
        } else {
            minimum_age = (guint32)value;
            goto SET_MINIMUM_AGE;
        }
    } else {
        if ((value < 0) || (value >= (gint32)(valid_days - USER_MODIFY_PASSWORD_WARNING_DAYS))) {
            print_set_pwd_minimum_age_usage(valid_days);
            debug_log(DLOG_INFO, "%s: input value range error %d", __FUNCTION__, value);
            return FALSE;
        } else {
            minimum_age = (guint32)value;
            goto SET_MINIMUM_AGE;
        }
    }

SET_MINIMUM_AGE:
    input_list = g_slist_append(input_list, g_variant_new_uint32(minimum_age));
    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_SECURITY_ENHANCE,
        CLASS_SECURITY_ENHANCE, METHOD_SECURITY_ENHANCE_SETMINIMUMPWDAGE, input_list, NULL);
    uip_free_gvariant_list(input_list);

    if (ret != RET_OK) {
        g_printf("Set minimum password age failed.\r\n");
        return FALSE;
    }

    g_printf("Set minimum password age successfully, minimumpasswordage(%d) days.\r\n", minimum_age);

    return TRUE;
}



gint32 ipmc_set_pwroff_policy_after_leakage(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target)
{
    gint32 ret;
    GSList *input_list = NULL;
    guint8 pwroff_policy;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    OBJ_HANDLE obj_handle;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 2) { // 2表示只支持2个参数
        print_leak_policy_usage();
        return FALSE;
    }

    if (strcmp(argv[0], "-v") != 0) {
        print_leak_policy_usage();
        return FALSE;
    }
    if (str_to_int(argv[1]) == IPMC_ERROR) {
        print_leak_policy_usage();
        return FALSE;
    }

    pwroff_policy = (guint8)str_to_ul(argv[1]);
    if (pwroff_policy != KEEP_PWR_STATUS_AFTER_LEAKAGE && pwroff_policy != POWER_OFF_AFTER_LEAKAGE) {
        print_leak_policy_usage();
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    ret = dfl_get_object_handle(COOLINGCLASS, &obj_handle);
    if (ret != DFL_OK) {
        g_printf("Set power-off strategy after leakage failed.\r\n");
        return FALSE;
    }
    input_list = g_slist_append(input_list, g_variant_new_byte(pwroff_policy));
    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, COOLINGCLASS,
        obj_handle, METHOD_COOLING_SET_LEAK_POLICY, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        g_printf("Set power-off strategy after leakage failed.\r\n");
        return FALSE;
    }

    g_printf("Set power-off strategy after leakage successfully.\r\n");

    return TRUE;
}


gint32 ipmc_set_powercappinginfo_failaction(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target)
{
    gint32 iRet;
    gint32 ret;
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    gchar obj_name[OBJ_NAME_MAX_LEN] = {0};
    gint32 blade_id = 0;
    guint32 blade_num = 0;
    guint8 enable = 0;
    gint32 input = 0;
    GSList *handle_list = NULL;
    GSList *list_item = NULL;
    OBJ_HANDLE blade_handle = 0;

    OBJ_HANDLE object_handle = 0;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    
    ret = dfl_get_object_list(CLASS_NAME_IPMBETH_BLADE, &handle_list);
    if (ret != DFL_OK) {
        g_printf("Get blade number failed.\r\n ");
        return FALSE;
    }
    for (list_item = handle_list; list_item; list_item = g_slist_next(list_item)) {
        object_handle = (OBJ_HANDLE)list_item->data;
        if (is_compute_node(object_handle) == FALSE) {
            continue;
        }
        blade_num++;
    }
    g_slist_free(handle_list);
    

    if ((argc != 3) || (strcmp(argv[0], "-v"))) {
        g_printf("Usage: ipmcset %s-t powercapping -d failaction -v <blade(1~%d)> <0|1>\r\n", need_l_shelf, blade_num);
        g_printf("Options:\r\n");
        g_printf("\t0\tNoAction\r\n\t1\tPowerOff\r\n");

        return FALSE;
    }

    if ((blade_id = str_to_int(argv[1])) == IPMC_ERROR) {
        g_printf("Usage: ipmcset %s-t powercapping -d failaction -v <blade(1~%d)> <0|1>\r\n", need_l_shelf, blade_num);
        g_printf("Options:\r\n");
        g_printf("\t0\tNoAction\r\n\t1\tPowerOff\r\n");

        return FALSE;
    }

    if ((blade_id > blade_num) || (blade_id < 1)) {
        g_printf("Usage: ipmcset %s-t powercapping -d failaction -v <blade(1~%d)> <0|1>\r\n", need_l_shelf, blade_num);
        g_printf("Options:\r\n");
        g_printf("\t0\tNoAction\r\n\t1\tPowerOff\r\n");

        return FALSE;
    }

    if (strcasecmp(argv[2], "1") == 0) {
        enable = SHELF_PWRCAP_STATUS_ENABLED;
    } else if (strcasecmp(argv[2], "0") == 0) {
        enable = SHELF_PWRCAP_STATUS_DISABLED;
    } else {
        g_printf("Usage: ipmcset %s-t powercapping -d failaction -v <blade(1~%d)> <0|1>\r\n", need_l_shelf, blade_num);
        g_printf("Options:\r\n");
        g_printf("\t0\tNoAction\r\n\t1\tPowerOff\r\n");

        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    if (enable == SHELF_PWRCAP_STATUS_ENABLED) {
        input = ipmc_get_user_input("WARNING: The system will power off automatically in "
            "15 seconds after power capping fails. Continue? [Y/N]:");
        
        if ((input != 'y') && (input != 'Y')) {
            return FALSE;
        }
    }

    
    input_list = g_slist_append(input_list, g_variant_new_byte(enable));
    
    
    (void)dal_get_specific_object_byte(CLASS_NAME_IPMBETH_BLADE, PROPERTY_IPMBETH_BLADE_ID, blade_id, &blade_handle);
    iRet = snprintf_s(obj_name, OBJ_NAME_MAX_LEN, OBJ_NAME_MAX_LEN - 1, "%s", dfl_get_object_name(blade_handle));
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    

    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_NAME_IPMBETH_BLADE,
        obj_name, METHOD_SET_BLADE_FAIL_ACTION, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        g_printf("Set blade power capping failed action failed.\r\n");
        return FALSE;
    }

    g_printf("Set blade power capping failed action successfully.\r\n");

    return TRUE;
}