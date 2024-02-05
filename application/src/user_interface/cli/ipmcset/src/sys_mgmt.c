

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
#include "cli_sol.h"
#include "ipmc_shelf_common.h"
#include "ipmcset_sys_mgmt.h"
#include "ipmcset_alarm.h"
#include "ipmcset_netconfig.h"

#define NO_ARGUMENT 0       // 不需要参数
#define REQUIRED_ARGUMENT 1 // 必须指定参数
#define OPTIONAL_ARGUMENT 2 // 参数可选


typedef enum pd_idx_e {
    PD_STATE_IDX = 0,
    PD_HOTSPARE_STATE_IDX = 1,
    PD_HOTSPARE_LD_IDX = 2,
    PD_LOCATE_STATE_IDX = 3,
    PD_NULL_IDX = 4,
    PD_BOOT_PRIORITY_IDX = 5
} PD_IDX_E;


LOCAL void print_composition_help_info(void);



LOCAL void print_composition_help_info(void)
{
    gint32 ret;
    guint8 avalible_val = 0;
    guint8 avalible_id = 0;
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    GSList *comp_list = NULL;
    GSList *comp_iter = NULL;
    OBJ_HANDLE comp_handle = 0;
    guchar node_id = 0;
    guint8 software_type = 0;

    ret = dfl_get_object_list(CLASS_NAME_COMPOSITION, &comp_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: cann't get class for [%s], ret is %d", __FUNCTION__, CLASS_NAME_COMPOSITION, ret);
        return;
    }

    g_printf("Usage: ipmcset -t resource -d composition -v [domain] <mode_id>\r\n");
    print_composition_node_id();

    
    (void)dal_get_software_type(&software_type);
    if (software_type == MGMT_SOFTWARE_TYPE_BLADE) {
        g_printf("  When [domain] is not specified, the configuration will be applied on the current node.\r\n\r\n");
    } else {
        g_printf("  When [domain] is not specified, the configuration will be applied on the first node.\r\n\r\n");
    }
    

    for (comp_iter = comp_list; comp_iter != NULL; comp_iter = comp_iter->next) {
        comp_handle = (OBJ_HANDLE)comp_iter->data;
        (void)dal_get_property_value_byte(comp_handle, PROPERTY_COMPOSITION_SLOT, &node_id);
        g_printf("Available composition setting for domain%d:\n", node_id);

        ret = dal_get_object_list_position(comp_handle, CLASS_NAME_COMPOSITION_SETTING, &obj_list);
        if (ret != RET_OK || g_slist_length(obj_list) == 0) {
            g_printf("%s: composition setting list length for %s is %d", __FUNCTION__, dfl_get_object_name(comp_handle),
                g_slist_length(obj_list));
            continue;
        }

        for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
            
            ret = dal_get_property_value_byte((OBJ_HANDLE)obj_note->data, PROPERTY_COMPOSITION_SETTING_AVAILABLE,
                &avalible_val);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "Get PROPERTY_COMPOSITION_SETTING_AVAILABLE value failed, ret = %d\n", ret);
                continue;
            }

            if (avalible_val == 0) {
                continue;
            }

            ret =
                dal_get_property_value_byte((OBJ_HANDLE)obj_note->data, PROPERTY_COMPOSITION_SETTING_ID, &avalible_id);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "Get PROPERTY_COMPOSITION_SETTING_ID value failed, ret = %d\n", ret);
                continue;
            }
            

            if (avalible_id <= 0 || avalible_id > MAX_COMPOSITION_SETTING_CNT) {
                debug_log(DLOG_ERROR, "invalid composition setting id:%d", avalible_id);
                continue;
            }

            g_printf("Setting mode : %d\n", avalible_id);
            dal_print_resource_compositions_list((OBJ_HANDLE)obj_note->data);
        }

        

        g_printf("\r\n");

        g_slist_free(obj_list);
        obj_list = NULL;
    }

    g_slist_free(comp_list);
    

    return;
}


gint32 ipmc_identify_led(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    gint32 Ledtime = 0;
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    if (argc == 0) {
        
        input_list = g_slist_append(input_list, g_variant_new_byte(TURN_ON_IDENTIFY_LED));
        input_list = g_slist_append(input_list, g_variant_new_byte(TURN_ON_DEFAULT_TIME));
    } else if (argc == 2) {
        if (strcmp(argv[0], "-v")) {
            g_printf("Usage: ipmcset %s-d identify  [-v time | force]    0s<=time<=255s\r\n", help_default_name);
            g_printf("       0s means to turn off identify led.\r\n");
            return FALSE;
        } else if (!(strcmp("force", argv[1]))) {
            
            input_list = g_slist_append(input_list, g_variant_new_byte(TURN_ON_IDENTIFY_LED));
            input_list = g_slist_append(input_list, g_variant_new_byte(0));
        } else if ((Ledtime = str_to_int(argv[1])) != IPMC_ERROR) {
            if (Ledtime == 0) {
                
                input_list = g_slist_append(input_list, g_variant_new_byte(TURN_OFF_IDENTIFY_LED));
                input_list = g_slist_append(input_list, g_variant_new_byte(0));
            } else {
                
                if (Ledtime < 0 || Ledtime > 255) {
                    g_printf("Usage: ipmcset %s-d identify  [-v time | force]    0s<=time<=255s\r\n",
                        help_default_name);
                    g_printf("       0s means to turn off identify led.\r\n");
                    return FALSE;
                }

                input_list = g_slist_append(input_list, g_variant_new_byte(TURN_ON_IDENTIFY_LED));
                input_list = g_slist_append(input_list, g_variant_new_byte((guint8)Ledtime));
            }
        } else {
            g_printf("Usage: ipmcset %s-d identify  [-v time | force]    0s<=time<=255s\r\n", help_default_name);
            g_printf("       0s means to turn off identify led.\r\n");
            return FALSE;
        }
    } else {
        g_printf("Usage: ipmcset %s-d identify  [-v time | force]    0s<=time<=255s\r\n", help_default_name);
        g_printf("       0s means to turn off identify led.\r\n");
        return FALSE;
    }

    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, LED_CLASS_NAME, UID_LED,
        METHOD_UIDLED_STATE, input_list, NULL);
    if (ret != RET_OK) {
        g_printf("Identify UID led failed.\r\n");
        uip_free_gvariant_list(input_list);
        return FALSE;
    }

    g_printf("Identify UID led successfully.\r\n");
    uip_free_gvariant_list(input_list);
    return TRUE;
}


gint32 ipmc_restore_factorysetting(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret = 0;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcset %s-d restore\r\n", help_default_name);
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    g_printf("WARNING: The %s will automatically restart and restore factory settings.\r\n",
        product_type_name);
    
    if (ipmc_check_user_passwd() != RET_OK) {
        ipmc_log_operation_log("CLI", "Recover manufacturer default configuration fail.");
        return FALSE;
    }
    
    
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_LOG, OBJECT_LOG,
        METHOD_ADVANCE_FACTORY_RESTORE, NULL, NULL);
    
    if (ret != RET_OK) {
        if (ret == FM_ERROR_NO_RECOVER_POINT) {
            g_printf("No restore point is set.\r\n");
        } else {
            g_printf("Restore factory setting failed.\r\n");
        }
        return FALSE;
    }
    g_printf("Restore factory setting successfully.\r\n");
    return TRUE;
}


gint32 ipmc_rollback(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcset -d rollback\r\n");
        return FALSE;
    }

    
    ret = is_continue_operate();
    if (ret != RET_OK) {
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_NAME_UPGRADE,
        OBJ_NAME_UPGRADE, METHOD_UPGRADE_ROLLBACK, NULL, NULL);
    if (ret != RET_OK) {
        g_printf("Set rollback failed.\r\n");
        return FALSE;
    }

    g_printf("Set rollback successfully, system will reboot soon.\r\n");
    return TRUE;
}


gint32 ipmc_set_ascend_mode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    OBJ_HANDLE object_handle;
    guint8 ret;
    guchar current_power_state;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    OBJ_HANDLE obj_hd;
    guint8 ascend_mode;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 2 || strcmp(argv[0], "-v")) {
        print_workmode_usage();
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    ascend_mode = (guint8)str_to_int(argv[1]);
    if (ascend_mode != NPU_WORK_MODE_AMP && ascend_mode != NPU_WORK_MODE_SMP) {
        print_workmode_usage();
        return FALSE;
    }

    ret = dfl_get_object_handle(OBJECT_CHASSISPAYLOAD, &object_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s : get %s obj failed, ret=%d", __FUNCTION__, OBJECT_CHASSISPAYLOAD, ret);
        return FALSE;
    }

    ret = dal_get_property_value_byte(object_handle, PROPERTY_PAYLOAD_CHASSPWR_STATE, &current_power_state);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : get value %s failed, ret=%d", __FUNCTION__, PROPERTY_PAYLOAD_CHASSPWR_STATE, ret);
        return FALSE;
    }
    if (current_power_state == PAYLOAD_POWER_STATE_ON) {
        g_printf("Can't set AMP/SMP during the power on, please power off the system.\r\n");
        return FALSE;
    }

    // RPC设置两块板的工作模式属性，两块板需要同时生效
    ret = dfl_get_object_list(CLASS_NPU, &obj_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Get NPU obj list failed, ret=%d", __FUNCTION__, ret);
        return FALSE;
    }

    for (obj_node = obj_list; obj_node != NULL; obj_node = obj_node->next) {
        obj_hd = (OBJ_HANDLE)obj_node->data;
        ret = set_ascend_mode(obj_hd, (const gchar *)username, (const gchar *)ip, ascend_mode);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: set work mode for %s failed, ret:%d", __FUNCTION__,
                dfl_get_object_name(obj_hd), ret);
            (void)dfl_foreach_object(CLASS_NPU, ascend_mode_info, NULL, NULL);
            g_printf("Please make sure NPU boards are in same mode then turn on the power!\r\n");
            g_slist_free(obj_list);
            return FALSE;
        }
    }

    g_slist_free(obj_list);

    (void)dfl_foreach_object(CLASS_NPU, ascend_mode_info, NULL, NULL);

    return TRUE;
}

LOCAL void ipaddr_submask_pton(gchar *str_ipaddr, guint32 *ipaddr, gchar *str_submask, guint32 *submask)
{
    (void)inet_pton(AF_INET, str_ipaddr, (void *)ipaddr);
    (void)inet_pton(AF_INET, str_submask, (void *)submask);
}


gint32 ipmc_set_autodiscovery_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    guchar state = 0;
    guint32 port = 0;
    guint32 option_val = 0;
    gchar str_ipaddr[PARAMETER_LEN] = {0};
    gchar str_submask[PARAMETER_LEN] = {0};
    gchar net_segment[PARAMETER_LEN] = {0};
    guint32 ipaddr = 0;
    guint32 submask = 0;
    struct sockaddr_in bcast = { 0 };

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (_get_outer_eth_ipaddr_mask(str_ipaddr, sizeof(str_ipaddr), str_submask, sizeof(str_submask)) != RET_OK) {
        g_printf("failed to get ip & mask\r\n");
        return FALSE;
    }

    ipaddr_submask_pton(str_ipaddr, &ipaddr, str_submask, &submask);
    bcast.sin_addr.s_addr = (ipaddr & submask) | (~submask);

    if ((ipaddr == INADDR_NONE) || (submask == INADDR_NONE)) {
        g_printf("failed to get ip & mask\r\n");
        return RET_ERR;
    }

    _cleanup_gfree_ gchar *sinaddr = dal_inet_ntop_ipv4(bcast.sin_addr);
    if ((argc < 2) || ((argv[0]) && strcmp(argv[0], "-v"))) {
        g_printf("Usage: ipmcset -d autodiscovery -v <enable/disable> [option(0/1)] [netport]\r\n");
        g_printf("Options are:\r\n");
        g_printf("      0       broadcast to 255.255.255.255\r\n");
        g_printf("      1       broadcast to %s\r\n", sinaddr);
        return FALSE;
    }

    if (argv[1] == NULL) {
        g_printf("Usage: ipmcset -d autodiscovery -v <enable/disable> [option(0/1)] [netport]\r\n");
        g_printf("Options are:\r\n");
        g_printf("      0       broadcast to 255.255.255.255\r\n");
        g_printf("      1       broadcast to %s\r\n", sinaddr);
        return FALSE;
    }

    // 处理enable
    if (strcmp(argv[1], "enable") == 0) {
        state = TRUE;
    } else if (strcmp(argv[1], "disable") == 0) {
        state = FALSE;
    } else {
        g_printf("Usage: ipmcset -d autodiscovery -v <enable/disable> [option(0/1)] [netport]\r\n");
        g_printf("Options are:\r\n");
        g_printf("      0       broadcast to 255.255.255.255\r\n");
        g_printf("      1       broadcast to %s\r\n", sinaddr);
        return FALSE;
    }

    // 处理option
    if (argc >= 3) {
        option_val = str_to_int(argv[2]);
        if (option_val == 0) {
            
            (void)strncpy_s(net_segment, sizeof(net_segment), "255.255.255.255", strlen("255.255.255.255"));
            
        } else if (option_val == 1) {
            
            (void)strncpy_s(net_segment, sizeof(net_segment), sinaddr, sizeof(net_segment) - 1);
            
        } else {
            g_printf("Input parameter format error.\r\n");
            return FALSE;
        }
    }

    // 处理port
    if (argc >= 4) {
        port = (guint32)str_to_int(argv[3]);
        if ((port > 65535) || (port < 1)) {
            g_printf("Input parameter format error.\r\n");
            return FALSE;
        }
    }

    set_autodiscovery(state, net_segment, option_val, port);

    return TRUE;
}


gint32 ipmc_set_keyboard_layout(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    errno_t safe_fun_ret = EOK;
    gint32 ret;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    guint8 str_layout[MAX_VNC_LAYOUT_NAME_LENGTH+1] = {0};
    const gchar *str_en = "en";
    const gchar *str_jp = "jp";
    const gchar *str_de = "de";

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 2 || strcmp(argv[0], "-v") != 0) {
        g_printf("Usage: ipmcset -t vnc -d keyboardlayout -v <en|jp|de>\r\n");
        return RET_ERR;
    }

    if (strcasecmp(argv[1], str_en) == 0) {
        safe_fun_ret = memcpy_s(str_layout, MAX_VNC_LAYOUT_NAME_LENGTH + 1, str_en, strlen(str_en));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        }
    } else if (strcasecmp(argv[1], str_jp) == 0) {
        safe_fun_ret = memcpy_s(str_layout, MAX_VNC_LAYOUT_NAME_LENGTH + 1, str_jp, strlen(str_jp));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        }
    } else if (strcasecmp(argv[1], str_de) == 0) {
        safe_fun_ret = memcpy_s(str_layout, MAX_VNC_LAYOUT_NAME_LENGTH + 1, str_de, strlen(str_de));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        }
    } else {
        g_printf("Usage: ipmcset -t vnc -d keyboardlayout -v <en|jp|de>\r\n");
        return RET_ERR;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    ret = set_vnc_keyboard_layout(OPERATOR_LOG_CLI, username, ip, (const gchar *)str_layout);
    if (ret != RET_OK) {
        g_printf("Failed to set VNC keyboard layout to (%s).\r\n", str_layout);
        return RET_ERR;
    }

    g_printf("Set VNC keyboard layout to (%s) successfully.\r\n", str_layout);

    return RET_OK;
}


gint32 ipmc_set_maintenance_ps_on_lock_clear(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target)
{
    gint32 ret;
    gchar ip[PARAMETER_LEN + 1] = {0};
    gchar username[PARAMETER_LEN + 1] = {0};

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if ((argc != 2) || (strcmp(argv[0], "-v")) || (strcmp(argv[1], "clear"))) {
        g_printf("Usage: ipmcset -t maintenance -d poweronlock -v clear\r\n");
        return VOS_ERR;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    ret = is_continue_operate();
    if (ret != RET_OK) {
        return FALSE;
    }

    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_CHASSISPAYLOAD,
        OBJECT_CHASSISPAYLOAD, METHOD_PS_ON_LOCK_CLEAR, NULL, NULL);
    if (ret != RET_OK) {
        g_printf("Clear power on lock failed.\r\n");
        return ret;
    }

    g_printf("Clear power on lock successfully.\r\n");
    return ret;
}



gint32 ipmc_set_maintenance_resetiME(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    OBJ_HANDLE meinfo_handle;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcset -t maintenance -d resetiME\r\n");
        return FALSE;
    }

    
    ret = is_continue_operate();
    if (ret != RET_OK) {
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        (void)snprintf_s(username, sizeof(username), sizeof(username) - 1, "%s", "unknown");
        (void)snprintf_s(ip, sizeof(ip), sizeof(ip) - 1, "%s", "unknown");
    }
    ret = ipmc_get_resetiME_support();
    if (ret != RET_OK) {
        g_printf("Reset iME is unsupported!\r\n");
        return FALSE;
    }
    (void)dal_get_object_handle_nth(CLASS_ME_INFO, 0, &meinfo_handle);
    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
        CLASS_ME_INFO, meinfo_handle, METHOD_MEINFO_RESET_IME, NULL, NULL);
    if (ret != RET_OK) {
        g_printf("Reset iME failed.\r\n");
        return FALSE;
    }

    g_printf("Reset iME successfully, the iME will restart soon.\r\n");
    return TRUE;
}



gint32 ipmc_set_maintenance_upgrade_bios(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target)
{
    errno_t safe_fun_ret;
    GVariant *value = NULL;
    gchar path_info[MAX_FILEPATH_LENGTH + 1] = {0};
    guint8 upgrade_flag;
    guint8 forced_upgrade_flag;
    guint8 power_state = 0;
    OBJ_HANDLE obj_handle = 0;
    gchar name[LOG_SPECIAL_STRING_LEN_MAX] = {"iBMC"};

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        ipmc_log_operation_log("Maintenance", "Insufficient privilege");
        return FALSE;
    }

    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        g_printf("Usage: ipmcset -t maintenance -d upgradebios -v <filepath>\r\n");
        g_printf("Filepath      e.g.: /tmp/biosimage.hpm\r\n");

        return FALSE;
    }

    
    if (strlen(argv[1]) > MAX_FILEPATH_LENGTH) {
        g_printf("File path is too long.\r\n");
        return FALSE;
    }

    if (dal_check_real_path(argv[1]) != RET_OK) {
        g_printf("File path is invalid.\r\n");
        return FALSE;
    }
    

    safe_fun_ret = strncpy_s(path_info, MAX_FILEPATH_LENGTH + 1, argv[1], MAX_FILEPATH_LENGTH);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        return FALSE;
    }

    
    
    if (vos_get_file_accessible(path_info) != TRUE) {
        g_printf("Filepath %s does not exist\r\n", path_info);
        g_printf("Upgrade failed.\r\n");

        ipmc_log_operation_log("Maintenance", "Invalid input");

        return FALSE;
    }

    

    
    if (dfl_get_object_handle(OBJECT_FRUPAYLOAD, &obj_handle) != DFL_OK) {
        g_printf("Get upgrade object failed.\r\n");
        ipmc_log_operation_log("Maintenance", "Upgrade BIOS failed");
        return FALSE;
    }

    if (dal_get_property_value_byte(obj_handle, PROPERTY_FRUPAYLOAD_POWERSTATE, &power_state) != RET_OK) {
        g_printf("Get upgrade property failed.\r\n");
        ipmc_log_operation_log("Maintenance", "Upgrade BIOS failed");
        return FALSE;
    }

    if (power_state) {
        g_printf("Please power off OS first, and then upgrade BIOS again.\r\n");
        ipmc_log_operation_log("Maintenance", "Upgrade BIOS failed");
        return FALSE;
    }
    

    
    if (uip_get_object_property(PFN_CLASS_NAME, OBJECT_PFN, PFN_UPGRADE_INFLAG_NAME, &value) != RET_OK) {
        g_printf("Get upgrade flag failed.\r\n");

        ipmc_log_operation_log("Maintenance", "Upgrade BIOS failed");
        return FALSE;
    }

    upgrade_flag = g_variant_get_byte(value);
    g_variant_unref(value);

    if (upgrade_flag != 0) {
        g_printf("An upgrade operation is in progress, please wait.\r\n");

        ipmc_log_operation_log("Maintenance", "Upgrade BIOS failed");
        return FALSE;
    }

    
    if (dal_get_custom_string_name(name, BMC_CUSTOM_LOGIBMC_NAME, sizeof(name)) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_custom_string_name fail.", __FUNCTION__);
    }

    g_printf("Please make sure the %s is working while upgrading.\r\n", name);

    forced_upgrade_flag = FORCE_UPGRADE_BIOS;
    if (send_update_command(path_info, forced_upgrade_flag, NO_RESET_FLAG) != RET_OK) {
        g_printf("Upgrade failed.\r\n");

        (void)fflush(stdout);
        return FALSE;
    }

    (void)fflush(stdout);
    return TRUE;
}


gint32 ipmc_set_notimeout_state(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gchar ip[PARAMETER_LEN + 1] = {0};
    gchar username[PARAMETER_LEN + 1] = {0};
    GSList *input_list = NULL;
    guint32 cli_session_timeout = DEFAULT_CLI_SESSION_TIMEOUT;
    const gint8 argvnum = 2;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        ipmc_log_operation_log("BMC", "Insufficient privilege");
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    if (((argc != argvnum) && (argc != argvnum + 1)) || (g_strcmp0(argv[0], "-v") != 0)) {
        print_set_cli_session_timeout_helpinfo();
        return FALSE;
    }

    if (g_strcmp0(argv[1], "enabled") == 0) {
        cli_session_timeout = 0;
    } else if (g_strcmp0(argv[1], "disabled") == 0) {
        if (argc == argvnum + 1) {
            cli_session_timeout = str_to_ul(argv[2]);
            if (cli_session_timeout == 0 || cli_session_timeout > MAX_CLI_SESSION_TIMEOUT) {
                print_set_cli_session_timeout_helpinfo();
                return FALSE;
            }
        }
    } else {
        print_set_cli_session_timeout_helpinfo();
        return FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_uint32(cli_session_timeout));
    gint32 ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar*)username, (const gchar*)ip, CLASS_NAME_BMC,
        OBJ_NAME_BMC, METHOD_BMC_SET_NOTIMEOUT, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: call method %s failed, ret is %d", __FUNCTION__, METHOD_BMC_SET_NOTIMEOUT, ret);
        g_printf("Set notimeout state failed.\r\n");
        return FALSE;
    }
    g_printf("Set notimeout state successfully.\r\n");
    return TRUE;
}


gint32 ipmc_set_printscreen(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    guint8 type = 0;
    gint32 ret;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    GSList *input_list = NULL;
    uid_t uid = 0;
    gid_t gid = 0;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if ((argc != 0) && (argc != 2)) {
        g_printf("Usage: ipmcset -d printscreen [-v wakeup] \r\n");
        return FALSE;
    }

    if (argc == 2) {
        if (strcmp(argv[0], "-v")) {
            g_printf("Usage: ipmcset -d printscreen [-v wakeup] \r\n");
            return FALSE;
        }

        if (strcmp("wakeup", argv[1]) == 0) {
            type = 0x01; 
        } else {
            g_printf("Usage: ipmcset -d printscreen [-v wakeup]\r\n");
            return FALSE;
        }
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(type));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, KVM_CLASS_NAME,
        KVM_OBJ_NAME, KVM_METHOD_SAVE_SCREEN, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        if (ret == 0xff) {
            g_printf("Printscreen cannot be captured when the KVM is in private mode.\r\n");
            return FALSE;
        } else {
            g_printf("Set printscreen failed.\r\n");
            return FALSE;
        }
    }

    ret = ipmc_get_local_uid_gid(&uid, &gid);
    if (ret != RET_OK) {
        return FALSE;
    }
    if (uid >= LDAP_USER_ID_BASE) {
        uid = APACHE_UID;
        gid = APPS_USER_GID;
    }

    
    input_list = NULL;
    input_list = g_slist_append(input_list, g_variant_new_string(IMG_PATH));
    input_list = g_slist_append(input_list, g_variant_new_string(IMG_TMP_PATH));
    input_list = g_slist_append(input_list, g_variant_new_uint32(uid));
    input_list = g_slist_append(input_list, g_variant_new_uint32(gid));
    input_list = g_slist_append(input_list, g_variant_new_uint32(0));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_PRIVILEGEAGENT,
        OBJ_NAME_PRIVILEGEAGENT, METHOD_PRIVILEGEAGENT_COPYFILE, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        g_printf("Download print screen image failed.\r\n");
        return FALSE;
    }

    g_printf("Download print screen image to %s successfully.\r\n", IMG_TMP_PATH);
    

    return TRUE;
}


gint32 ipmc_stop_watchdog(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    gint32 input;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcset %s-d reset\r\n", help_default_name);
        return FALSE;
    }

    g_printf("This operation will reboot %s system.\r\n", product_type_name);
    input = ipmc_get_user_input("Continue? [Y/N]:");
    if ((input == 'n') || (input == 'N')) {
        return FALSE;
    } else if ((input == 'y') || (input == 'Y')) {
        ;
    } else {
        g_printf("Input character invalid.\r\n");
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_NAME_BMC,
        OBJ_NAME_BMC, METHD_BMC_REBOOTPME, NULL, NULL);
    
    if (ret != RET_OK && (ret != RPC_UNKNOWN)) {
        g_printf("Reset %s system failed.\r\n", product_type_name);
        return FALSE;
    }
    

    g_printf("Resetting...\r\n");
    return TRUE;
}

LOCAL gint32 check_fru_control_data(gint32 data)
{
    gint32 fru_control_type[] = {FRU_RESET, FRU_GRACE_REBOOT};

    if (dal_is_pangea_enterprise_board()) {
        if (data == POWER_CYCLE) {
            return RET_OK;
        }
    }

    for (gint32 i = 0; i < sizeof(fru_control_type)/sizeof(gint32); i++) {
        if (data == fru_control_type[i]) {
            return RET_OK;
        }
    }

    return RET_ERR;
}


gint32 ipmc_control_fru(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    gchar *target_info = (target != NULL) ? target : "fru0";

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        print_fru_control();
        return FALSE;
    }

    gint32 data = str_to_int(argv[1]);
    if (data == IPMC_ERROR || (check_fru_control_data(data) == RET_ERR)) {
        print_fru_control();
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    
    gint32 ret = is_continue_frucontrol_operate(data);
    if (ret != RET_OK) {
        return FALSE;
    }

    gchar *control_info = (data == FRU_RESET) ? "forced system reset" : "forced power cycle";

    if (data == FRU_RESET) {
        ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_FRUPAYLOAD,
            OBJECT_FRUPAYLOAD, METHOD_COLD_RESET, NULL, NULL);
    } else if (data == FRU_GRACE_REBOOT) {
        ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_FRUPAYLOAD,
            OBJECT_FRUPAYLOAD, METHOD_GRACE_REBOOT, NULL, NULL);
    } else if (data == POWER_CYCLE) {
        ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_FRUPAYLOAD,
            OBJECT_FRUPAYLOAD, METHOD_POWER_CYCLE, NULL, NULL);
    }

    
    if (ret != RET_OK) {
        if (ret == IPMC_UNSUPPORT) {
            g_printf("Set %s Power Control(%s)failed: Command is not supported in present state.\r\n",
                target_info, control_info);
            return FALSE;
        }

        
        if (ret == IPMC_COMMAND_INVALID) {
            g_printf("Set %s Power Control(%s): Command is not effective in present state.\r\n", target_info,
                control_info);
            
            if (data == FRU_RESET) {
                g_printf("The forced restart is invalid when the backup power function is enabled.\r\n");
            }
            return FALSE;
        }
        g_printf("FRU control %s (%s) failed.\r\n", target_info, control_info);
        return FALSE;
    }

    g_printf("FRU control %s (%s) successfully.\r\n", target_info, control_info);
    
    return TRUE;
}


gint32 ipmc_set_bootdevice(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    guint32 device;
    guint8 effective_times = 0;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    GSList *input_list = NULL;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if ((argc > 3) || (argc < 2) || (strcmp(argv[0], "-v"))) {
        print_boot_device_usage();
        return FALSE;
    }

    device = (guint32)str_to_int(argv[1]);

    
    if (argc == 3) {
        if (!strcmp(argv[2], "once")) {
            effective_times = 1;
        } else if (!strcmp(argv[2], "permanent")) {
            effective_times = 2;
        } else {
            print_boot_device_usage();
            return FALSE;
        }
    }
    
    else { 
        effective_times = 1;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)device));

    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_NAME_BIOS,
        OBJ_NAME_BIOS, METHOD_BIOS_BOOTOPTION, input_list, NULL);
    if (ret == BIOS_ERROR_INVALID_INPUT_PARA) {
        print_boot_device_usage();
        return FALSE;
    }
    if (ret != RET_OK) {
        g_printf("Set boot device failed.\r\n");
        uip_free_gvariant_list(input_list);
        return FALSE;
    }

    
    uip_free_gvariant_list(input_list);
    input_list = NULL;
    input_list = g_slist_append(input_list, g_variant_new_byte(effective_times));

    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_NAME_BIOS,
        OBJ_NAME_BIOS, METHOD_BIOS_BOOTOPTION_FLAG, input_list, NULL);
    if (ret != RET_OK) {
        g_printf("Set boot device failed.\r\n");
        uip_free_gvariant_list(input_list);
        return FALSE;
    }

    uip_free_gvariant_list(input_list);

    g_printf("Set boot device successfully.\r\n");
    
    return TRUE;
}


gint32 ipmc_clear_cmos(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcset -d clearcmos\r\n");
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    
    ret = is_continue_operate();
    if (ret != RET_OK) {
        return FALSE;
    }

    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_NAME_BIOS,
        OBJ_NAME_BIOS, METHOD_BIOS_CLEARCMOS, NULL, NULL);
    if (ret != RET_OK) {
        g_printf("Clear CMOS failed.\r\n");
        return FALSE;
    }

    g_printf("Clear CMOS successfully.\r\n");
    return TRUE;
}

LOCAL gint32 __check_config_import_state(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv)
{
    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        print_import_config_usage();
        return FALSE;
    }

    // 校验是不是xml格式的
    const gchar *xml_suffix = strrchr(argv[1], '.');
    if (xml_suffix == NULL || strcmp(xml_suffix, ".xml") != 0) {
        g_printf("Unsupported file type.\r\n");
        return FALSE;
    }
    return TRUE;
}

LOCAL gint32 __check_file_path_valid(gchar **argv)
{
    
    if (strncmp(argv[1], "/", strlen("/")) != 0 || dal_check_real_path(argv[1]) != RET_OK ||
        g_file_test(argv[1], G_FILE_TEST_IS_REGULAR) == FALSE) {
        g_printf("File path is invalid.\r\n");
        print_import_config_usage();
        return FALSE;
    }

    
    if ((strlen(argv[1]) > MAX_FILEPATH_LENGTH)) {
        g_printf("File path is too long.\r\n");
        print_import_config_usage();
        return FALSE;
    }
    return TRUE;
}


gint32 ipmc_set_config_import(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gchar file_path[MAX_FILEPATH_LENGTH + 1] = {0};
    gint32 ret;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if (__check_config_import_state(privilege, rolepriv, argc, argv) == FALSE) {
        return FALSE;
    }

    
    if (ipmc_check_user_passwd() != RET_OK) {
        ipmc_log_operation_log("CLI", "Import configuration failed");
        goto err_exit;
    }

    
    
    if ((strstr(argv[1], FILE_URL_HTTPS_HEAD) == NULL) && (strstr(argv[1], FILE_URL_SFTP_HEAD) == NULL) &&
        (strstr(argv[1], FILE_URL_NFS_HEAD) == NULL) && (strstr(argv[1], FILE_URL_CIFS_HEAD) == NULL) &&
        (strstr(argv[1], FILE_URL_SCP_HEAD) == NULL)) {
        
        if (__check_file_path_valid(argv) == FALSE) {
            ipmc_log_operation_log("CLI", "Import configuration failed");
            return FALSE;
        }

        (void)strncpy_s(file_path, MAX_FILEPATH_LENGTH + 1, argv[1], MAX_FILEPATH_LENGTH);
        
    } else {
        ret = get_file_path_from_url(argv[1], FILE_ID_CONFIG_EXPORT_IMPORT, file_path, sizeof(file_path));
        clear_sensitive_info(argv[1], strlen(argv[1]));
        if (ret != TRUE) {
            ipmc_log_operation_log("CLI", "Import configuration failed");
            goto err_exit;
        }
    }

    ret = get_username_ip(username, ip, sizeof(ip));
    if (ret != RET_OK) {
        ipmc_log_operation_log("CLI", "Import configuration failed");
        goto err_exit;
    }

    ret = set_collect_configuration(file_path);
    ipmc_delete_file(username, file_path);
    if (ret != TRUE) {
        goto err_exit;
    }

    g_printf("Import configuration successfully.\r\n");
    if (is_support_bios_configuration()) {
        g_printf("Reset OS for the BIOS config to take effect.\r\n");
    }

    return TRUE;

err_exit:
    g_printf("Import configuration failed.\r\n");
    return FALSE;
}


gint32 ipmc_set_maintenance_biosprint(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    OBJ_HANDLE bios_handle = 0;
    GSList *input_list = NULL;
    GSList *caller_info = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    caller_info = g_slist_append(caller_info, g_variant_new_string(OPERATOR_LOG_CLI));
    caller_info = g_slist_append(caller_info, g_variant_new_string(username));
    caller_info = g_slist_append(caller_info, g_variant_new_string(ip));
    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        ipmc_log_operation_log(MAINTENANCE, "Insufficient privilege");
        g_slist_free_full(caller_info, (GDestroyNotify)g_variant_unref);
        return FALSE;
    }

    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        print_set_maintenance_biosprint_helpinfo();
        g_slist_free_full(caller_info, (GDestroyNotify)g_variant_unref);
        return FALSE;
    }

    gint32 biosprint_option = 0;
    if (dal_strtoi(argv[1], &biosprint_option, NUMBER_BASE10) != RET_OK ||
        ((biosprint_option != SET_BIOS_DEGUG_INFO_ENABLE) && (biosprint_option != SET_BIOS_DEGUG_INFO_BY_SETUP_MENU))) {
        ipmc_log_operation_log(MAINTENANCE, "Invalid input");
        print_set_maintenance_biosprint_helpinfo();
        g_slist_free_full(caller_info, (GDestroyNotify)g_variant_unref);
        return FALSE;
    }

    gint32 ret = dal_get_object_handle_nth(BIOS_CLASS_NAME, 0, &bios_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get bios handle failed\n");
        ipmc_log_operation_log(MAINTENANCE, "Set BIOS debug info enable failed");
        g_slist_free_full(caller_info, (GDestroyNotify)g_variant_unref);
        return FALSE;
    }

    if (biosprint_option == SET_BIOS_DEGUG_INFO_ENABLE) {
        g_printf("WARNING: Setting BIOS debug info enable will make system start slow. \r\n");
        gint32 input = ipmc_get_user_input(CLI_COMFIRM_STRING);
        if ((input == 'n') || (input == 'N')) {
            g_slist_free_full(caller_info, (GDestroyNotify)g_variant_unref);
            return FALSE;
        } else if ((input == 'y') || (input == 'Y')) {
        } else {
            g_slist_free_full(caller_info, (GDestroyNotify)g_variant_unref);
            g_printf("Input character invalid.\r\n");
            return FALSE;
        }

        input_list = g_slist_append(input_list, g_variant_new_byte(SET_BIOS_DEGUG_INFO_ENABLE_ON));
        ret = dfl_call_class_method(bios_handle, METHOD_BIOS_SET_PRINT_FLAG, caller_info, input_list, NULL);
        g_slist_free_full(caller_info, (GDestroyNotify)g_variant_unref);

        if (ret != DFL_OK) {
            g_printf("Set BIOS debug info enable failed.\r\n");
            debug_log(DLOG_ERROR, "call bios set printflag property method failed. ret =%d\n", ret);
            g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
            return FALSE;
        }
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        g_printf("Set BIOS debug info enable successfully\r\n");
    } else {
        input_list = g_slist_append(input_list, g_variant_new_byte(SET_BIOS_DEGUG_INFO_BY_SETUP_MENU_ON));
        ret = dfl_call_class_method(bios_handle, METHOD_BIOS_SET_PRINT_FLAG, caller_info, input_list, NULL);
        g_slist_free_full(caller_info, (GDestroyNotify)g_variant_unref);

        if (ret != DFL_OK) {
            g_printf("Set BIOS debug info enabled/disabled by BIOS setup menu failed.\r\n");
            g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
            return FALSE;
        }
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        g_printf("Set BIOS debug info enabled/disabled by BIOS setup menu successfully\r\n");
    }

    return TRUE;
}

LOCAL gint32 parse_ld_list(gchar *arg, LD_LIST_S *ld_list)
{
#define STRTOUL_BASE_10    10
    gint32 retval = RET_OK;
    guint32 idx;
    gchar *endptr = NULL;
    gchar **str_array = NULL;
    GSList *list = NULL;
    GSList *node = NULL;

    if (arg == NULL || strlen(arg) == 0 || ld_list == NULL) {
        return RET_ERR;
    }

    str_array = g_strsplit(arg, ",", SML_MAX_PHYSICAL_DRIVES);
    if (str_array == NULL) {
        return RET_ERR;
    }

    for (guint32 idx = 0; idx < SML_MAX_PHYSICAL_DRIVES; idx++) {
        if (str_array[idx] == NULL) {
            break;
        }
        // 当物理盘ID字符串最后一个是','时, 认为是非法值
        if (strlen(str_array[idx]) == 0) {
            retval = RET_ERR;
            break;
        }

        gulong str_val = strtoul(str_array[idx], &endptr, STRTOUL_BASE_10);

        if (endptr != NULL && strlen(endptr) == 0 && errno != ERANGE) {
            guint32 pd_id = str_val;
            list = g_slist_append(list, UINT32_TO_POINTER(pd_id));
            continue;
        } else {
            retval = RET_ERR;
            break;
        }
    }

    if (retval == RET_OK && g_slist_length(list) != 0) {
        ld_list->count = g_slist_length(list);

        ld_list->ids = (guint16 *)g_malloc0(ld_list->count);
        if (ld_list->ids != NULL) {
            for (node = list, idx = 0; node; node = g_slist_next(node), idx++) {
                ld_list->ids[idx] = POINTER_TO_UINT32(node->data) & INVALID_DATA_WORD;
            }
        } else {
            retval = RET_ERR;
        }
    }

    g_slist_free(list);
    g_strfreev(str_array);
    return retval;
}


LOCAL gboolean parse_pd_hotspare_ld(char *opt_arg, OBJ_HANDLE obj_handle, const char *opt_name, LD_LIST_S *ld_list)
{
    gint32 ret;
    OBJ_HANDLE tmp_obj_handle = 0;
    gchar ref_ctrl_obj[MAX_NAME_SIZE] = {0};
    guint32 hotspare_ld;
    guint8 ctrl_id = 0;

    ret = parse_ld_list(opt_arg, ld_list);
    if (ret == RET_ERR) {
        g_printf("Error : option '-%s' - invalid argument '%s'.\r\n", opt_name, optarg);
        print_set_pd_config_usage();
        return FALSE;
    }

    for (guint32 i = 0; i < ld_list->count; i++) {
        hotspare_ld = ld_list->ids[i];
        if (hotspare_ld > INVALID_DATA_WORD) {
            g_printf("Error : option '-%s' - argument '%s' is out of range.\r\n", opt_name, optarg);
            return FALSE;
        }

        (void)dal_get_property_value_string(obj_handle, PROPERTY_HDD_REF_RAID_CONTROLLER, ref_ctrl_obj,
            sizeof(ref_ctrl_obj));
        if (strcmp(ref_ctrl_obj, "N/A") == 0) {
            g_printf("The operation is not supported at present state or this physical drive is not managed "
                "through RAID controller.\r\n");
            return FALSE;
        }

        ret = dfl_get_object_handle(ref_ctrl_obj, &tmp_obj_handle);
        if (ret != DFL_OK) {
            g_printf("The operation failed as internal error.\r\n");
            return FALSE;
        }

        (void)dal_get_property_value_byte(tmp_obj_handle, PROPERTY_RAID_CONTROLLER_ID, &ctrl_id);

        ret = dal_get_logical_drive_object_handle(ctrl_id, (guint16)(hotspare_ld & INVALID_DATA_WORD), &tmp_obj_handle);
        if (ret == RET_ERR) {
            g_printf("Error : option '-%s' - invalid argument '%s'.\r\n", opt_name, optarg);
            print_set_pd_config_usage();
            return FALSE;
        }
    }

    return TRUE;
}


LOCAL gboolean check_arg_and_get_pd_handle(guchar argc, gchar **argv, OBJ_HANDLE *obj_handle)
{
#define ARGC_LEAST      3
    OBJ_HANDLE tmp_handle = 0;

    if (argc < ARGC_LEAST || (strcmp(argv[0], "-v") != 0)) {
        print_set_pd_config_usage();
        return FALSE;
    }

    gint32 pd_id = str_to_int(argv[1]);
    if (pd_id < 0) {
        g_printf("Invalid Physical Drive ID.\r\n");
        return FALSE;
    }

    gint32 ret = dal_get_specific_object_byte(CLASS_HDD_NAME, PROPERTY_HDD_ID, (guint8)pd_id, &tmp_handle);
    if (ret != RET_OK) {
        g_printf("Invalid Physical Drive ID.\r\n");
        return FALSE;
    }

    *obj_handle = tmp_handle;

    return TRUE;
}

static gint32 check_hotspare_param(gint32 has_hotspare, guint8 hotspare_state, gint32 has_ld, guint32 ld_count)
{
    if (has_hotspare == 0) {
        return TRUE;
    }

    if ((hotspare_state == PD_HOT_SPARE_DEDICATED || hotspare_state == PD_HOT_SPARE_AUTO_REPLACE) && has_ld == 0) {
        g_printf("Error : option '-ld' is required.\r\n");
        return FALSE;
    }

    if ((hotspare_state == PD_HOT_SPARE_NONE || hotspare_state == PD_HOT_SPARE_GLOBAL) && has_ld != 0) {
        g_printf("Error : invalid option '-ld'\r\n");
        return FALSE;
    }

    if (hotspare_state == PD_HOT_SPARE_DEDICATED && ld_count == 0) {
        g_printf("Error : option '-ld' - invalid quantity of argument\r\n");
        return FALSE;
    }

    if (hotspare_state == PD_HOT_SPARE_AUTO_REPLACE && ld_count != 1) {
        g_printf("Error : option '-ld' - invalid quantity of argument\r\n");
        return FALSE;
    }

    return TRUE;
}

LOCAL void fill_invalid_ld_list(LD_LIST_S *ld_list)
{
    if (ld_list->count != 0) {
        return;
    }

    
    ld_list->ids = (guint16 *)g_malloc0(sizeof(guint16));
    if (ld_list->ids == NULL) {
        return;
    }
    ld_list->ids[0] = INVALID_DATA_WORD;
    ld_list->count = 1;
    return;
}


gint32 ipmc_set_physical_drive_config(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;
    guint8 state = 0xFF;
    guint8 hotspare_state = 0xFF;
    guint8 locate_state = 0xFF;
    guint8 boot_priority = 0;
    gint32 is_state = 0;
    gint32 is_hotspare = 0;
    gint32 is_hotspare_ld = 0;
    gint32 is_locate = 0;
    gint32 is_cryptoerase = 0;
    guint8 sys_lockdown_state = 0;
    gint32 is_bootable = 0;
    LD_LIST_S ld_list = {NULL, 0};
    gint32 ret = RET_ERR;

    const gchar *optstring = ":";
    gint c = 0, idx = 0;

    struct option opts[] = {
        {UIP_PD_OPTION_FW_STATE, REQUIRED_ARGUMENT, &is_state, 1},
        {UIP_PD_OPTION_HOT_SPARE_STATE, REQUIRED_ARGUMENT, &is_hotspare, 1},
        {UIP_PD_OPTION_SPARE_LD, REQUIRED_ARGUMENT, &is_hotspare_ld, 1},
        {UIP_PD_OPTION_LOCATE_STATE, REQUIRED_ARGUMENT, &is_locate, 1},
        {UIP_PD_OPTION_CRYPTO_ERASE, NO_ARGUMENT, &is_cryptoerase, 1},
        {UIP_PD_OPTION_BOOTABLE_FLAG, REQUIRED_ARGUMENT, &is_bootable, 1},
        {NULL, NO_ARGUMENT, NULL, 0}
    };

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    
    if (check_arg_and_get_pd_handle(argc, argv, &obj_handle) != TRUE) {
        return FALSE;
    }

    optind = 1;
    opterr = 0;

    while ((c = getopt_long_only(argc, argv, optstring, opts, &idx)) != -1) {
        switch (c) {
            case ':':
                g_printf("Error : option '%s' - requires an argument.\r\n", argv[optind - 1]);
                g_free(ld_list.ids);
                print_set_pd_config_usage();
                return FALSE;

            case '?':
                g_printf("Error : invalid option '%s'.\r\n", argv[optind - 1]);
                g_free(ld_list.ids);
                print_set_pd_config_usage();
                return FALSE;

            default:
                break;
        }

        if (opts[idx].has_arg == REQUIRED_ARGUMENT) {
            if (optarg == NULL || *optarg == '-') {
                g_printf("Error : option '-%s' - requires an argument.\r\n",
                    (optarg == NULL && argc == optind) ? argv[argc - 1] + 1 : opts[idx].name);
                g_free(ld_list.ids);
                print_set_pd_config_usage();
                return FALSE;
            }
        } else if (opts[idx].has_arg == NO_ARGUMENT) {
            continue;
        }

        switch (idx) {
            case PD_STATE_IDX:
                ret = uip_parse_pd_state(optarg, &state);
                break;

            case PD_HOTSPARE_STATE_IDX:
                ret = uip_parse_pd_hotspare_state(optarg, &hotspare_state);
                break;

            case PD_HOTSPARE_LD_IDX:
                if (parse_pd_hotspare_ld(optarg, obj_handle, opts[idx].name, &ld_list) == FALSE) {
                    g_free(ld_list.ids);
                    return FALSE;
                }
                break;

            case PD_LOCATE_STATE_IDX:
                ret = uip_parse_pd_locate_state(optarg, &locate_state);
                break;
            case PD_NULL_IDX:
                break;
            case PD_BOOT_PRIORITY_IDX:
                ret = uip_parse_boot_priority(optarg, &boot_priority);
                break;

            default:
                break;
        }

        
        if (ret == RET_ERR) {
            g_printf("Error : option '-%s' - invalid argument '%s'.\r\n", opts[idx].name, optarg);
            print_set_pd_config_usage();
            g_free(ld_list.ids);
            return FALSE;
        }
    }

    
    if ((is_state + is_hotspare + is_locate + is_cryptoerase + is_bootable) == 0) {
        g_printf("Error : no valid option.\r\n");
        g_free(ld_list.ids);
        return FALSE;
    }
    

    if (check_hotspare_param(is_hotspare, hotspare_state, is_hotspare_ld, ld_list.count) != TRUE) {
        g_free(ld_list.ids);
        return FALSE;
    }

    if ((argc - optind) != 1) {
        g_printf("Error : one or more redundant arguments.\r\n");
        g_free(ld_list.ids);
        return FALSE;
    }

    // 系统锁定开启后 只允许点灯操作
    (void)dal_get_func_ability(CLASS_SECURITY_ENHANCE, PROPERTY_SECURITY_SYSTEMLOCKDOWNSTATE, &sys_lockdown_state);
    if ((sys_lockdown_state == ENABLE) && (is_state == 1 || is_hotspare == 1 || is_cryptoerase == 1 ||
        is_bootable == 1)) {
        g_printf("Error: this command is not allowed in system lockdown mode.\r\n");
        g_free(ld_list.ids);
        return FALSE;
    }

    
    if (set_physical_drive_config_confirm(is_cryptoerase) != RET_OK) {
        g_free(ld_list.ids);
        return FALSE;
    }

    g_printf("The operation may take a few seconds, Please wait...\r\n");

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    if (is_state == 1) {
        set_pd_state(obj_handle, state, username, ip);
    }
    if (is_hotspare == 1) {
        fill_invalid_ld_list(&ld_list);
        set_pd_hotspare_state(obj_handle, hotspare_state, &ld_list, username, ip);
    }
    
    
    if (is_locate == 1) {
        set_pd_locate_state(obj_handle, locate_state, 0, username, ip);
    }
    
    if (is_cryptoerase == 1) {
        set_pd_crypto_erase(obj_handle, username, ip);
    }
    if (is_bootable == 1) {
        set_pd_bootable(obj_handle, boot_priority, username, ip);
    }
    g_free(ld_list.ids);
    return TRUE;
}



gint32 ipmc_set_raid_controller_config(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target)
{
    gint32 ret;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;
    gint32 ctrl_id;

    guint8 copyback = 0xFF;
    guint8 smarter_copyback = 0xFF;
    guint8 jbod = 0xFF;
    guint8 mode = 0xFF;

    gint32 is_copyback = 0;
    gint32 is_smarter_copyback = 0;
    gint32 is_jbod = 0;
    gint32 is_restore = 0;
    gint32 is_mode = 0;

    const gchar *optstring = ":";
    gint c = 0, idx = 0;

    struct option opts[] = {
        {UIP_RAID_CONTROLLER_OPTION_COPYBACK_STATE, REQUIRED_ARGUMENT, &is_copyback, 1},
        {UIP_RAID_CONTROLLER_OPTION_SMARTER_COPYBACK_STATE, REQUIRED_ARGUMENT, &is_smarter_copyback, 1},
        {UIP_RAID_CONTROLLER_OPTION_JBOD_STATE, REQUIRED_ARGUMENT, &is_jbod, 1},
        {UIP_RAID_CONTROLLER_OPTION_RESTORE, NO_ARGUMENT, &is_restore, 1},
        {UIP_RAID_CONTROLLER_OPTION_MODE, REQUIRED_ARGUMENT, &is_mode, 1},
        {NULL, NO_ARGUMENT, NULL, 0}
    };

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    
    if (argc < 3 || (strcmp(argv[0], "-v") != 0)) {
        print_set_ctrl_config_usage();
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

    optind = 1;
    opterr = 0;

    while ((c = getopt_long_only(argc, argv, optstring, opts, &idx)) != -1) {
        switch (c) {
            case ':':
                g_printf("Error : option '%s' - requires an argument.\r\n", argv[optind - 1]);
                print_set_ctrl_config_usage();
                return FALSE;

            case '?':
                g_printf("Error : invalid option '%s'.\r\n", argv[optind - 1]);
                print_set_ctrl_config_usage();
                return FALSE;

            default:
                break;
        }

        if (idx != 3) { // restore下标是3，没有参数需要检查
            if (optarg == NULL || *optarg == '-') {
                g_printf("Error : option '-%s' - requires an argument.\r\n",
                    (optarg == NULL && argc == optind) ? (argv[argc - 1] + 1) : opts[idx].name);
                print_set_ctrl_config_usage();
                return FALSE;
            }
        }

        switch (idx) {
            case 0:
                ret = uip_parse_ctrl_config_state(optarg, &copyback);
                break;

            case 1:
                ret = uip_parse_ctrl_config_state(optarg, &smarter_copyback);
                break;

            case 2:
                ret = uip_parse_ctrl_config_state(optarg, &jbod);
                break;

            case 3:
                break;

            case 4:  // 在数组opt中下标为4
                ret = uip_parse_ctrl_config_mode(optarg, &mode);
                break;

            default:
                break;
        }

        if (ret == RET_ERR) {
            g_printf("Error : option '-%s' - invalid argument '%s'.\r\n", opts[idx].name, optarg);
            print_set_ctrl_config_usage();
            return FALSE;
        }
    }

    if (is_restore == 1 && (is_copyback + is_smarter_copyback + is_jbod) != 0) {
        g_printf("Error : option '-restore' can't be specified with others simultaneously.\r\n");
        return FALSE;
    }

    
    if ((is_restore + is_copyback + is_smarter_copyback + is_jbod + is_mode) == 0) {
        g_printf("Error : no valid option.\r\n");
        return FALSE;
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

    if (is_copyback == 1) {
        set_ctrl_copyback(obj_handle, copyback, (const char*)username, (const char*)ip);
    }
    if (is_smarter_copyback == 1) {
        set_ctrl_smarter_copyback(obj_handle, smarter_copyback, (const char*)username, (const char*)ip);
    }
    if (is_jbod == 1) {
        set_ctrl_jbod(obj_handle, jbod, (const char*)username, (const char*)ip);
    }
    if (is_restore == 1) {
        set_ctrl_restore(obj_handle, (const char*)username, (const char*)ip);
    }
    if (is_mode == 1) {
        set_ctrl_mode(obj_handle, mode, username, ip);
    }

    return TRUE;
}


gint32 ipmc_set_resource_composition(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    gint32 data = 0;
    guint8 avalible_val = 0;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    GSList *input_list = NULL;
    OBJ_HANDLE setting_obj_handle = 0;
    OBJ_HANDLE composition_handle = 0;
    
    gint32 comp_slot;
    gchar *mode_str = NULL;
    gint32 i;
    guchar config_status = PCIeTopo_OK;
    gchar *domain_id_index = NULL;
    guint8 software_type = 0;
    OBJ_HANDLE board_handle = 0;
    guint8 slot_id = 0;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        ipmc_log_operation_log("CompositionSetting", "Insufficient privilege");
        return FALSE;
    }

    if ((argc != 2 && argc != 3) || (strcmp(argv[0], "-v"))) {
        goto print_help_quit;
    }

    if (argc == 2) {
        comp_slot = 1;
        
        // 节点板自动补充正确的domain参数
        (void)dal_get_software_type(&software_type);
        if (software_type == MGMT_SOFTWARE_TYPE_BLADE) {
            if (dfl_get_object_handle(BMC_BOARD_OBJ_NAME, &board_handle) == DFL_OK) {
                (void)dal_get_property_value_byte(board_handle, BMC_SLOT_ID_NAME, &slot_id);
                if ((slot_id > 0) && (slot_id <= 2)) {
                    comp_slot = slot_id;
                }
            }
        }
        
        mode_str = argv[1];
    } else {
        domain_id_index = g_strrstr(argv[1], COMPOSITION_DOMAIN_PREFIX);
        if (domain_id_index == NULL) {
            debug_log(DLOG_ERROR, "%s failed: cann't get domain index", __FUNCTION__);
            goto print_help_quit;
        }

        comp_slot = str_to_int(argv[1] + strlen(COMPOSITION_DOMAIN_PREFIX));
        mode_str = argv[2];
    }

    
    if ((data = str_to_int(mode_str)) == IPMC_ERROR || (comp_slot == IPMC_ERROR)) {
        goto print_help_quit;
    }
    

    ret =
        dal_get_specific_object_byte(CLASS_NAME_COMPOSITION, PROPERTY_COMPOSITION_SLOT, comp_slot, &composition_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: cann't find object handle with [%s:%d]", __FUNCTION__, PROPERTY_COMPOSITION_SLOT,
            comp_slot);
        goto print_help_quit;
    }

    ret = dal_get_specific_position_object_byte(composition_handle, CLASS_NAME_COMPOSITION_SETTING,
        PROPERTY_COMPOSITION_SETTING_ID, (guint8)data, &setting_obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get CLASS_NAME_COMPOSITION_SETTING handle failed, ret = %d\n", ret);
        goto print_help_quit;
    }

    ret = dal_get_property_value_byte(setting_obj_handle, PROPERTY_COMPOSITION_SETTING_AVAILABLE, &avalible_val);
    if ((ret != RET_OK) || (avalible_val != 1)) {
        debug_log(DLOG_ERROR, "Get PROPERTY_COMPOSITION_SETTING_AVAILABLE value failed, ret = %d\n", ret);
        goto print_help_quit;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        (void)snprintf_s(username, sizeof(username), sizeof(username) - 1, "%s", "unknown");
        (void)snprintf_s(ip, sizeof(ip), sizeof(ip) - 1, "%s", "unknown");
    }

    
    ret = is_continue_operate();
    if (ret != RET_OK) {
        return FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)data));

    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_NAME_COMPOSITION,
        dfl_get_object_name(composition_handle), METHOD_COMPOSITION_CONFIG_COMPOSITION, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        g_printf("Resource composed failed: %s\r\n", dal_get_pcie_topo_code_string((PCIeTopoCode)ret));
        debug_log(DLOG_ERROR,
            "uip_call_class_method call CLASS_NAME_COMPOSITION METHOD_COMPOSITION_CONFIG_COMPOSITION failed, ret = "
            "%d\n",
            ret);
        return FALSE;
    }

    for (i = 0; i < PCIeTopo_CONFIG_WAIT_RETRY_TIMES; i++) {
        (void)dal_get_property_value_byte(composition_handle, PROPERTY_COMPOSITION_TOPO_CFG_STATUS, &config_status);
        if (config_status == PCIeTopo_CONFIG_TASK_RUNNING) {
            g_printf(".");
            (void)fflush(stdout);
        } else if (config_status == PCIeTopo_CONFIG_FINISHED) {
            
            g_printf(
                "Resource composed successfully. These pending settings will be applied after next power cycle.\r\n");
            
            return TRUE;
        }

        vos_task_delay(1000);
    }

    g_printf("\r\nResource composed failed: %s\r\n", dal_get_pcie_topo_code_string(PCIeTopo_CONFIG_TIMEOUT));

    return FALSE;

print_help_quit:
    print_composition_help_info();

    

    return FALSE;
}


gint32 ipmc_reset_othersmm(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret = 0;
    gint32 input;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar userip[PARAMETER_LEN + 1] = {0};
    guint32 presence = 0;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcset %s-d resetothersmm\r\n", help_default_name);
        return FALSE;
    }

    (void)get_other_board_present(&presence);
    if (presence != BLADE_PRESENCE) {
        g_printf("Other smm is not present.\r\n");
        return TRUE;
    }

    g_printf("This operation will reboot other %s system.\r\n", product_type_name);
    input = ipmc_get_user_input("Continue? [Y/N]:");
    if ((input == 'n') || (input == 'N')) {
        return FALSE;
    }

    if ((input == 'y') || (input == 'Y')) {
        if (get_username_ip(username, userip, sizeof(userip)) != RET_OK) {
            snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
            snprintf_s(userip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        }

        ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)userip, CLASS_SMM,
            OBJECT_SMM, METHOD_RESET_OTHER_SMM, NULL, NULL);
        if (ret != RET_OK) {
            g_printf("Reset other %s system failed.\r\n", product_type_name);
            return FALSE;
        }

        g_printf("Resetting...\r\n");
        return TRUE;
    }

    g_printf("Input character invalid.\r\n");
    return FALSE;
}