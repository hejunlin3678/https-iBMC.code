
#include <stdlib.h>
#include "pme_app/uip/uip.h"
#include "pme_app/uip/ipmc_public.h"
#include "ipmcset_service_mgmt.h"
#include "ipmcsetmain.h"
#include "pme_app/uip/ipmctypedef.h"
#include "ipmcsethelp.h"
#include "ipmcsetcommand.h"
#include "shelfsetcommand.h"
#include "pme/common/mscm_vos.h"
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "ipmc_shelf_common.h"


void ipmc_data_info(guchar l_level, guchar level, gchar *location, gchar *target, gchar *cmd, gint32 argc, gchar **argv)
{
    
    
    if (l_level) {
        if (level) {
            
            (void)ipmc_find_l_t_d_cmd(location, target, cmd, argc, argv);
        } else {
            
            
            (void)ipmc_find_l_d_cmd(location, cmd, argc, argv);
        }
    } else if (level) {
        if (ipmc_find_matchtarget(target) == FALSE) {
            return;
        }

        (void)ipmc_find_cmd(target, cmd, argc, argv);
        return;
    } else {
        if (cmd == NULL) {
            g_printf("\r\ninput parameter<-d> error\r\n");
            g_printf("    -d <dataitem>\r\n");
            custom_print_cmd_list(get_g_bmc_cmd_list1());
            return;
        }

        if (ipmc_find_matchcmd(cmd, argc, argv) == FALSE) {
            return;
        }
    }
}


void ipmc_replace_ibmc_string(void)
{
    errno_t safe_fun_ret;
    gchar custom_name[LOG_SPECIAL_STRING_LEN_MAX] = {"iBMC"};
    gint32 ret_val;

    ret_val = dal_get_custom_string_name(custom_name, BMC_CUSTOM_LOGIBMC_NAME, sizeof(custom_name));
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_custom_string_name faild, ret = %d\n", __FUNCTION__, ret_val);
    }

    safe_fun_ret = strncpy_s(product_type_name, SHORT_STRING_LEN, custom_name, strlen(custom_name));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }

    return;
}


gint32 ipmc_main(gint32 argc, gchar **argv)
{
    errno_t safe_fun_ret = EOK;
    gint32 argflag;
    gchar target[SHORT_STRING_LEN] = {0};
    gchar location[SHORT_STRING_LEN] = {0};
    gchar *target_tmp = NULL;
    gchar *location_tmp = NULL;
    gchar *cmd = NULL;
    guchar l_level = 0;
    guchar t_level = 0;
    guchar need_l;
    gint32 argv_invalid_len_loop = 0;
    guchar MgmtSoftWareType = 0;

    need_l = ipmc_need_l();
    if (argv == NULL) {
        return FALSE;
    }
    if (argc == 1) {
        ipmc_print_helpinfo(need_l, l_level, NULL, t_level, NULL);
        return TRUE;
    }
    
    ipmc_replace_ibmc_string();

    (void)dal_get_software_type(&MgmtSoftWareType);

    
    if (need_l) {
        
        if (strcmp(argv[1], "-l") && strcmp(argv[1], "-t") && strcmp(argv[1], "-d") && strcmp(argv[1], "-h")) {
            ipmc_print_helpinfo(need_l, l_level, NULL, t_level, NULL);
            return TRUE;
        }

        
        if (strcmp(argv[1], "-l") && strcmp(argv[1], "-h") && (MgmtSoftWareType == MGMT_SOFTWARE_TYPE_EM)) {
            l_level++;
            safe_fun_ret =
                strncpy_s(location, SHORT_STRING_LEN, SHELF_MGMT_DEFAULT_NAME, strlen(SHELF_MGMT_DEFAULT_NAME));
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
            safe_fun_ret = strncpy_s(help_default_name, SHORT_STRING_LEN, SHELF_MGMT_HELP_DEFAULT_NAME,
                strlen(SHELF_MGMT_HELP_DEFAULT_NAME));
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
            safe_fun_ret =
                strncpy_s(product_type_name, SHORT_STRING_LEN, PRODUCT_TYPE_SMM_NAME, strlen(PRODUCT_TYPE_SMM_NAME));
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
        }

        
        if (MgmtSoftWareType == MGMT_SOFTWARE_TYPE_EM) {
            (void)strncpy_s(location, SHORT_STRING_LEN, SHELF_MGMT_DEFAULT_NAME, strlen(SHELF_MGMT_DEFAULT_NAME));
            (void)strncpy_s(need_l_shelf, SHORT_STRING_LEN, NEED_L_SHELF, strlen(NEED_L_SHELF));
            if (!strcmp(argv[1], "-l") && !strcmp(location, "smm")) {  // 1，2：分别为命令行参数标号
                safe_fun_ret = strncpy_s(help_default_name, SHORT_STRING_LEN, SHELF_MGMT_HELP_DEFAULT_NAME,
                    strlen(SHELF_MGMT_HELP_DEFAULT_NAME));
                if (safe_fun_ret != EOK) {
                    debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
                }
                safe_fun_ret = strncpy_s(product_type_name, SHORT_STRING_LEN, PRODUCT_TYPE_SMM_NAME,
                    strlen(PRODUCT_TYPE_SMM_NAME));
                if (safe_fun_ret != EOK) {
                    debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
                }
            }
        }
        
    } else {
        if (strcmp(argv[1], "-t") && strcmp(argv[1], "-d") && strcmp(argv[1], "-h")) {
            ipmc_print_helpinfo(need_l, l_level, NULL, t_level, NULL);
            return TRUE;
        }
    }
    
    while ((argflag = getopt(argc, (gchar **)argv, OPTION_STRING)) != IPMC_ERROR) {
        switch (argflag) {
            case 'h':
                
                ipmc_print_helpinfo(need_l, l_level, location, t_level, target);
                
                return TRUE;
            case 'l':
                
                l_level++;
                
                location_tmp = NULL;
                
                location_tmp = strdup(optarg);
                if (location_tmp == NULL) {
                    g_printf("location_tmp strdup error.\r\n");
                    return FALSE;
                }
                if (strlen(location_tmp) > SHORT_STRING_LEN - 1) {
                    free(location_tmp);
                    ipmc_print_helpinfo(need_l, l_level, NULL, t_level, NULL);
                    return FALSE;
                }
                
                if (strncpy_s(location, SHORT_STRING_LEN, location_tmp, strlen(location_tmp)) != EOK) {
                    debug_log(DLOG_ERROR, "%s: strncpy_s fail.\n", __FUNCTION__);
                }
                free(location_tmp);
                if (ipmc_find_matchlocation(location, sizeof(location)) == FALSE) {
                    return FALSE;
                }
                
                
                if (optind < argc && argv[optind] != NULL && strcmp(argv[optind], "-t") && strcmp(argv[optind], "-d") &&
                    (strcmp(argv[optind], "-h") || NULL != argv[optind + 1])) {
                    ipmc_print_helpinfo(need_l, l_level, location, 0, NULL);
                    return FALSE;
                }
                break;
                
            case 'd':
                
                
                if (l_level != 0 && (argc > 3) && strcmp(argv[1], "-d") && strcmp(argv[3], "-d") &&
                    strcmp(argv[5], "-d")) {
                    ipmc_print_helpinfo(need_l, l_level, location, t_level, target);
                    return TRUE;
                }

                if (l_level == 0 && argc > 3 && strcmp(argv[1], "-d") && strcmp(argv[3], "-d")) {
                    ipmc_print_helpinfo(need_l, l_level, location, t_level, target);
                    return TRUE;
                }

                
                
                cmd = NULL;
                
                cmd = strdup(optarg);
                if (cmd == NULL) {
                    g_printf("cmd strdup error.\r\n");
                    return FALSE;
                }
                if (strlen(cmd) > SHORT_STRING_LEN - 1) {
                    free(cmd);
                    ipmc_print_d_helpinfo(l_level, location, t_level, target);
                    return FALSE;
                }
                
                for (argv_invalid_len_loop = optind; argv_invalid_len_loop < argc; argv_invalid_len_loop++) {
                    if (argv[argv_invalid_len_loop] != NULL &&
                        strlen(argv[argv_invalid_len_loop]) > MAX_CLI_ARGV_LENGTH) {
                        ipmc_print_helpinfo(need_l, l_level, location, t_level, target);
                        free(cmd);
                        return FALSE;
                    }
                }
                
                ipmc_data_info(l_level, t_level, location, target, cmd, (argc - optind), &argv[optind]);
                free(cmd);
                return TRUE;
            case 't':
                t_level++;
                
                target_tmp = NULL;
                
                target_tmp = strdup(optarg);
                if (target_tmp == NULL) {
                    g_printf("target_tmp strdup error.\r\n");
                    return FALSE;
                }
                if (strlen(target_tmp) > SHORT_STRING_LEN - 1) {
                    free(target_tmp);
                    ipmc_print_helpinfo(need_l, l_level, location, t_level, NULL);
                    return FALSE;
                }
                
                if (strncpy_s(target, SHORT_STRING_LEN, target_tmp, strlen(target_tmp)) != EOK) {
                    debug_log(DLOG_ERROR, "%s: strncpy_s fail.\n", __FUNCTION__);
                }
                
                free(target_tmp);
                target_tmp = NULL;
                if (optind < argc && argv[optind] != NULL && strcmp(argv[optind], "-d") &&
                    (strcmp(argv[optind], "-h") || NULL != argv[optind + 1])) {
                    print_shelf_cli_helpinfo();
                    return FALSE;
                }
                
                if (l_level != 0 && ipmc_find_match_l_target(location, target) == FALSE) {
                    return TRUE;
                }
                if (l_level == 0 && ipmc_find_matchtarget(target) == FALSE) {
                    return TRUE;
                }

                break;

            default:
                
                if (MgmtSoftWareType == MGMT_SOFTWARE_TYPE_EM) {
                    if (optopt == 'l' && ipmc_find_matchlocation("-h", strlen("-h")) == FALSE) {
                        return TRUE;
                    }
                    if (optopt == 't' && ipmc_find_match_l_target(location, "-h") == FALSE) {
                        return TRUE;
                    }
                    if (optopt == 'd') {
                        ipmc_data_info(l_level, t_level, location, target, "-h", (argc - optind), &argv[optind]);
                        free(cmd);
                        return TRUE;
                    }
                }
                ipmc_print_helpinfo(need_l, l_level, location, t_level, target);
                return TRUE;
        }
    }
    
    ipmc_print_helpinfo(need_l, l_level, location, t_level, target);

    return TRUE;
}

LOCAL void get_property_value(GVariant *value, gchar *value_string, guint32 value_string_len)
{
    gint32 ret = -1;
    const GVariantType *type = NULL;

    if (value == NULL || value_string == NULL) {
        return;
    }

    type = g_variant_get_type(value);
    if (g_variant_type_equal(type, G_VARIANT_TYPE_BOOLEAN) == TRUE) {
        ret = snprintf_s(value_string, value_string_len, value_string_len - 1, "%c",
            (gchar)g_variant_get_boolean(value));
    } else if (g_variant_type_equal(type, G_VARIANT_TYPE_BYTE) == TRUE) {
        ret = snprintf_s(value_string, value_string_len, value_string_len - 1, "%d", g_variant_get_byte(value));
    } else if (g_variant_type_equal(type, G_VARIANT_TYPE_INT16) == TRUE) {
        ret = snprintf_s(value_string, value_string_len, value_string_len - 1, "%d", g_variant_get_int16(value));
    } else if (g_variant_type_equal(type, G_VARIANT_TYPE_UINT16) == TRUE) {
        ret = snprintf_s(value_string, value_string_len, value_string_len - 1, "%u", g_variant_get_uint16(value));
    } else if (g_variant_type_equal(type, G_VARIANT_TYPE_INT32) == TRUE) {
        ret = snprintf_s(value_string, value_string_len, value_string_len - 1, "%d", g_variant_get_int32(value));
    } else if (g_variant_type_equal(type, G_VARIANT_TYPE_UINT32) == TRUE) {
        ret = snprintf_s(value_string, value_string_len, value_string_len - 1, "%u", g_variant_get_uint32(value));
    } else if (g_variant_type_equal(type, G_VARIANT_TYPE_INT64) == TRUE) {
        ret = snprintf_s(value_string, value_string_len, value_string_len - 1, "%" G_GINT64_FORMAT,
            g_variant_get_int64(value));
    } else if (g_variant_type_equal(type, G_VARIANT_TYPE_UINT64) == TRUE) {
        ret = snprintf_s(value_string, value_string_len, value_string_len - 1, "%" G_GUINT64_FORMAT,
            g_variant_get_uint64(value));
    } else if (g_variant_type_equal(type, G_VARIANT_TYPE_STRING) == TRUE) {
        ret = snprintf_s(value_string, value_string_len, value_string_len - 1, "%s", g_variant_get_string(value, 0));
    } else {
        return;
    }

    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }

    return;
}

LOCAL void init_cli_cmd_by_obj_num(const gchar *class_name, guint32 dep_obj_num, guint8 *cmd_flag)
{
    gint32 ret;
    guint32 obj_num = 0;

    if (class_name == NULL || cmd_flag == NULL) {
        return;
    }

    ret = dfl_get_object_count(class_name, &obj_num);
    if (ret != DFL_OK || obj_num < dep_obj_num) {
        *cmd_flag = UNOBVIOUS_CMD;
    }

    return;
}

LOCAL void init_cli_cmd_by_prop(const gchar *class_name, const gchar *property_name, const gchar *property_value,
    guint8 *cmd_flag, guint8 reverse)
{
    gint32 ret;
    guint32 obj_num;
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    GVariant *value = NULL;
    gchar value_string[16] = {0};
    gint32 flag = 0;

    if (class_name == NULL || property_name == NULL || cmd_flag == NULL || property_value == NULL) {
        return;
    }

    ret = dfl_get_object_list(class_name, &obj_list);
    if (ret != DFL_OK) {
        
        *cmd_flag = UNOBVIOUS_CMD;
        
        return;
    }

    obj_num = g_slist_length(obj_list);
    if (obj_num == 0) {
        
        *cmd_flag = UNOBVIOUS_CMD;
        
        return;
    }

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        ret = dfl_get_property_value((OBJ_HANDLE)obj_note->data, property_name, &value);
        if (ret != DFL_OK) {
            g_slist_free(obj_list);
            return;
        }

        get_property_value(value, value_string, sizeof(value_string));
        if (!strcmp(property_value, value_string)) {
            flag = 1;
        }
        g_variant_unref(value);
        value = NULL;
    }

    if ((flag == 0 && reverse == 0) || (flag == 1 && reverse == 1)) {
        *cmd_flag = UNOBVIOUS_CMD;
    }

    g_slist_free(obj_list);
    return;
}


LOCAL gboolean __is_shield_special_set_cmd(const char *cmd_name, guint8 *cmd_flag)
{
    // PCIe形态的NPU芯片不支持此命令，不要在命令行显示
    if ((g_ascii_strcasecmp(cmd_name, "npuworkmode") == 0) && (dal_is_pcie_npu() == TRUE)) {
        *cmd_flag = UNOBVIOUS_CMD;
        return TRUE;
    }

    return FALSE;
}


LOCAL gint32 __shield_mgmt_cmd(guint8 *cmd_flag, guchar mgmt_type, guint8 *mgmt_shield_flag)
{
    gint32 ret;
    guint8 asstatus = 0;

    if (mgmt_type == MGMT_SOFTWARE_TYPE_EM) {
        ret = get_board_active_state(&asstatus);
        if (ret != RET_OK) {
            g_printf("get active state failed.\r\n");
            return RET_ERR;
        }
        if (asstatus == STANDBY_STATE) {
            *cmd_flag = UNOBVIOUS_CMD;
            *mgmt_shield_flag = 1;
            return RET_OK;
        }
    }
    *mgmt_shield_flag = 0;
    return RET_OK;
}

LOCAL void init_cli_cmd_list(struct helpinfo *cmd_list)
{
    gint32 cmd_num = 0;
    
    gint32 ret = 0;
    guint8 mgmt_shield_flag = 0;
    guchar mgmt_type = 0;

    if (cmd_list == NULL) {
        return;
    }

    (void)dal_get_software_type(&mgmt_type);

    while (cmd_list[cmd_num].name) {
        if (__is_shield_special_set_cmd(cmd_list[cmd_num].name, &cmd_list[cmd_num].flag) == TRUE) {
            cmd_num++;
            continue;
        }
        if (cmd_list[cmd_num].HideType == HIDE_TYPE_OBJ_NUM) {
            init_cli_cmd_by_obj_num(cmd_list[cmd_num].DepClassName, cmd_list[cmd_num].DepObjectNum,
                &cmd_list[cmd_num].flag);
        } else if (cmd_list[cmd_num].HideType == HIDE_TYPE_OBJ_NUM_AND_ACTIVE) {
            ret = __shield_mgmt_cmd(&cmd_list[cmd_num].flag, mgmt_type, &mgmt_shield_flag);
            if (ret != RET_OK) {
                return;
            }
            if (mgmt_shield_flag == 1) {
                cmd_num++;
                continue;
            }
            init_cli_cmd_by_obj_num(cmd_list[cmd_num].DepClassName, cmd_list[cmd_num].DepObjectNum,
                &cmd_list[cmd_num].flag);
        } else if (cmd_list[cmd_num].HideType == HIDE_TYPE_PROP) {
            init_cli_cmd_by_prop(cmd_list[cmd_num].DepClassName, cmd_list[cmd_num].DepPropertyName,
                cmd_list[cmd_num].DepPropetyValue, &cmd_list[cmd_num].flag, 0);
        } else if (cmd_list[cmd_num].HideType == HIDE_TYPE_PROP_REVERSE) {
            init_cli_cmd_by_prop(cmd_list[cmd_num].DepClassName, cmd_list[cmd_num].DepPropertyName,
                cmd_list[cmd_num].DepPropetyValue, &cmd_list[cmd_num].flag, 1);
        } else if (cmd_list[cmd_num].HideType == HIDE_TYPE_PROP_AND_ACTIVE) {
            ret = __shield_mgmt_cmd(&cmd_list[cmd_num].flag, mgmt_type, &mgmt_shield_flag);
            if (ret != RET_OK) {
                return;
            }
            if (mgmt_shield_flag == 1) {
                cmd_num++;
                continue;
            }
            init_cli_cmd_by_prop(cmd_list[cmd_num].DepClassName, cmd_list[cmd_num].DepPropertyName,
                cmd_list[cmd_num].DepPropetyValue, &cmd_list[cmd_num].flag, 0);
        } else if (cmd_list[cmd_num].HideType == HIDE_TYPE_PROP_REVERSE_AND_ACTIVE) {
            ret = __shield_mgmt_cmd(&cmd_list[cmd_num].flag, mgmt_type, &mgmt_shield_flag);
            if (ret != RET_OK) {
                return;
            }
            if (mgmt_shield_flag == 1) {
                cmd_num++;
                continue;
            }
            init_cli_cmd_by_prop(cmd_list[cmd_num].DepClassName, cmd_list[cmd_num].DepPropertyName,
                cmd_list[cmd_num].DepPropetyValue, &cmd_list[cmd_num].flag, 1);
        } else {
            cmd_num++;

            continue;
        }
        cmd_num++;
    }
    return;
}

LOCAL void init_cli_cmd_list_d(struct helpinfoplus *cmd_list)
{
    gint32 count = 0;

    if (cmd_list == NULL) {
        return;
    }

    while (cmd_list[count].help_info) {
        init_cli_cmd_list(cmd_list[count].help_info);
        count++;
    }
    return;
}


LOCAL void init_cli_cmd_list_l_t_d(struct helpinfoplus_plus *cmd_list)
{
    gint32 count = 0;

    if (cmd_list == NULL) {
        return;
    }

    while (cmd_list[count].help_infoplus) {
        init_cli_cmd_list_d(cmd_list[count].help_infoplus);
        count++;
    }
    return;
}

LOCAL void init_cli_service_option_list(struct service_method *option_list)
{
    gint32 opt_num = 0;

    if (option_list == NULL) {
        return;
    }

    while (option_list[opt_num].ServiceName) {
        if (option_list[opt_num].HideType == HIDE_TYPE_OBJ_NUM) {
            init_cli_cmd_by_obj_num(option_list[opt_num].DepClassName, option_list[opt_num].DepObjectNum,
                &option_list[opt_num].flag);
        } else if (option_list[opt_num].HideType == HIDE_TYPE_PROP) {
            init_cli_cmd_by_prop(option_list[opt_num].DepClassName, option_list[opt_num].DepPropertyName,
                option_list[opt_num].DepPropetyValue, &option_list[opt_num].flag, 0);
        } else {
            opt_num++;

            continue;
        }
        opt_num++;
    }
    return;
}

LOCAL void cli_cmd_init(void)
{
    
    init_cli_cmd_list(get_g_bmc_cmd_list1());

    
    init_cli_cmd_list(get_g_bmc_cmd_list2());

    
    init_cli_cmd_list_d(get_g_bmc_cmd_list_d());

    
    init_cli_service_option_list(get_g_bmc_cmd_service_state());
    init_cli_service_option_list(get_g_bmc_cmd_service_port());

    
    init_cli_cmd_list(get_g_bmc_cmd_list_l());
    init_cli_cmd_list_d(get_g_bmc_cmd_list_l_d_bond());
    init_cli_cmd_list_d(get_g_bmc_cmd_list_l_t_bond());
    init_cli_cmd_list_l_t_d(get_g_bmc_l_t_d_bond());
    
    set_g_bmc_cmd_list1(get_g_bmc_cmd_list1());
    set_bmc_cmd_list_l(get_g_bmc_cmd_list_l());
    set_bmc_l_smm_t(get_g_bmc_l_smm_t());
    set_bmc_l_smm_d(get_g_bmc_l_smm_d());
    set_bmc_cmd_list_l_t_bond(get_g_bmc_cmd_list_l_t_bond());
    set_bmc_cmd_list_l_d_bond(get_g_bmc_cmd_list_l_d_bond());
    set_bmc_l_t_d_bond(get_g_bmc_l_t_d_bond());
    set_bmc_cmd_list2(get_g_bmc_cmd_list2());
    set_bmc_cmd_list_d(get_g_bmc_cmd_list_d());
    set_ipmc_helpinfo_func(ipmc_helpinfo);

    return;
}


LOCAL void init_cli_cmd_list_t_d_v_level(gchar *cmd_buf)
{
    gint32 cmd_num;
    gchar *cmd_buf_tmp = NULL;
    struct helpinfo *cmd_list = NULL;

    if (cmd_buf == NULL) {
        return;
    }

    
    cmd_num = ipmc_get_matched_cmd(cmd_buf, get_g_bmc_cmd_list2());
    if (cmd_num == IPMC_ERROR) {
        g_printf("Cli black list(ipmcset -t xxx -d) match failed.\r\n");
        return;
    }

    
    cmd_list = get_g_bmc_cmd_list_d()[cmd_num].help_info;
    cmd_num = ipmc_get_matched_cmd(cmd_buf, cmd_list);
    if (cmd_num == IPMC_ERROR) {
        g_printf("Cli black list(ipmcset -t xxx -d xxx) match failed.\r\n");
        return;
    }

    
    if (strcmp(cmd_list[cmd_num].name, "state") == 0) {
        for (cmd_num = 0; get_g_bmc_cmd_service_state()[cmd_num].ServiceName != NULL; cmd_num++) {
            cmd_buf_tmp = strstr(cmd_buf, get_g_bmc_cmd_service_state()[cmd_num].ServiceName);
            if (cmd_buf_tmp != NULL) {
                
                if (g_strcmp0(cmd_buf_tmp, get_g_bmc_cmd_service_state()[cmd_num].ServiceName)) {
                    continue;
                }
                
                
                get_g_bmc_cmd_service_state()[cmd_num].flag = UNOBVIOUS_CMD;
                return;
            }
        }
    }
    
    else if (strcmp(cmd_list[cmd_num].name, "port") == 0) {
        for (cmd_num = 0; get_g_bmc_cmd_service_port()[cmd_num].ServiceName != NULL; cmd_num++) {
            cmd_buf_tmp = strstr(cmd_buf, get_g_bmc_cmd_service_port()[cmd_num].ServiceName);
            if (cmd_buf_tmp != NULL) {
                
                get_g_bmc_cmd_service_port()[cmd_num].flag = UNOBVIOUS_CMD;
                return;
            }
        }
    }

    
    g_printf("Cli black list(ipmcset -t xxx -d xxx -v) match failed.\r\n");
    return;
}


LOCAL void init_cli_black_cmd_list_l_t_d(gchar *cmd_buf)
{
    gchar *cmd_buf_tmp = NULL;
    gint32 index_i;
    gchar ret;

    if (cmd_buf == NULL) {
        return;
    }

    index_i = ipmc_get_matched_cmd(cmd_buf, get_g_bmc_cmd_list_l());
    if (index_i == IPMC_ERROR) {
        g_printf("Cli black list(ipmcset -l) match failed.\r\n");
        return;
    }

    
    if (get_g_bmc_cmd_list_l()[index_i].flag == UNOBVIOUS_CMD) {
        return;
    }

    
    cmd_buf_tmp = strstr(cmd_buf, "-v");
    if (cmd_buf_tmp != NULL) {
        init_cli_cmd_list_t_d_v_level(cmd_buf);
        return;
    }

    
    cmd_buf_tmp = strstr(cmd_buf, "-t");
    if (cmd_buf_tmp != NULL) {
        cmd_buf_tmp = strstr(cmd_buf, "-d");
        if (cmd_buf_tmp == NULL) {
            ret = init_cli_black_cmd_list_2_level(cmd_buf, get_g_bmc_cmd_list_l(), get_g_bmc_cmd_list_l_t_bond());
            if (ret == FALSE) {
                g_printf("Cli black list(ipmcset -l xxx -t xxx) match failed.\r\n");
            }
            return;
        }
        ret = init_cli_black_cmd_list_2_level(cmd_buf, get_g_bmc_cmd_list_l_t_bond()[index_i].help_info,
            get_g_bmc_l_t_d_bond()[index_i].help_infoplus);
        if (ret == FALSE) {
            g_printf("Cli black list(ipmcset -l xxx -t xxx -d xxx) match failed.\r\n");
        }
        return;
    }

    
    cmd_buf_tmp = strstr(cmd_buf, "-d");
    if (cmd_buf_tmp != NULL) {
        ret = init_cli_black_cmd_list_2_level(cmd_buf, get_g_bmc_cmd_list_l(), get_g_bmc_cmd_list_l_d_bond());
        if (ret == FALSE) {
            g_printf("Cli black list(ipmcset -l xxx -d xxx) match failed.\r\n");
        }
        return;
    }

    
    ret = init_cli_black_cmd_list_1_level(cmd_buf, get_g_bmc_cmd_list_l());
    if (ret == FALSE) {
        g_printf("Cli black list(ipmcset -l) match failed.\r\n");
    }
    return;
}


LOCAL void init_cli_black_cmd_list_t_d(gchar *cmd_buf)
{
    gchar *cmd_buf_tmp = NULL;
    gchar ret;

    if (cmd_buf == NULL) {
        return;
    }

    
    cmd_buf_tmp = strstr(cmd_buf, "-v");
    if (cmd_buf_tmp != NULL) {
        init_cli_cmd_list_t_d_v_level(cmd_buf);
        return;
    }

    
    cmd_buf_tmp = strstr(cmd_buf, "-d");
    if (cmd_buf_tmp != NULL) {
        ret = init_cli_black_cmd_list_2_level(cmd_buf, get_g_bmc_cmd_list2(), get_g_bmc_cmd_list_d());
        if (ret == FALSE) {
            g_printf("Cli black list(ipmcset -t xxx -d xxx) match failed.\r\n");
        }
        return;
    }

    
    ret = init_cli_black_cmd_list_1_level(cmd_buf, get_g_bmc_cmd_list2());
    if (ret == FALSE) {
        g_printf("Cli black list(ipmcset -t) match failed.\r\n");
    }
    return;
}


LOCAL void shield_blcklist_set(void)
{
    

    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GVariant *property_value = NULL;
    gsize cmd_list_cnt = 0;
    gchar **cmd_list = NULL;
    gchar *cmd_buf = NULL;
    gchar *cmd_buf_tmp = NULL;
    gint32 cmd_index;

    ret = dal_get_object_handle_nth(CLASS_CLI_BLACK_LIST, 0, &obj_handle);
    if (ret != RET_OK) {
        
        return;
    }

    ret = dfl_get_property_value(obj_handle, PROPERTY_CMD_LIST, &property_value);
    if (ret != DFL_OK) {
        
        g_printf("get cmd list value failed.\r\n");
        return;
    }

    cmd_list = g_variant_dup_strv(property_value, &cmd_list_cnt);
    if (cmd_list == NULL) {
        g_printf("get cmd list number failed.\r\n");
        g_variant_unref(property_value);
        return;
    }

    for (cmd_index = 0; cmd_list[cmd_index] != NULL; cmd_index++) {
        cmd_buf = cmd_list[cmd_index];
        
        cmd_buf = strstr(cmd_buf, "ipmcset");
        if (cmd_buf == NULL) {
            continue;
        }

        
        
        cmd_buf_tmp = strstr(cmd_buf, "-l");
        if (cmd_buf_tmp != NULL) {
            init_cli_black_cmd_list_l_t_d(cmd_buf_tmp);
            continue;
        }
        

        
        cmd_buf_tmp = strstr(cmd_buf, "-t");
        if (cmd_buf_tmp != NULL) {
            init_cli_black_cmd_list_t_d(cmd_buf_tmp);
            continue;
        }

        
        
        cmd_buf_tmp = strstr(cmd_buf, "-d");
        if (cmd_buf_tmp != NULL) {
            
            ret = init_cli_black_cmd_list_1_level(cmd_buf_tmp, get_g_bmc_cmd_list1());
            
            if (ret == FALSE) {
                g_printf("Cli black list(ipmcset -d) match failed.\r\n");
            }
        }
    }
    
    if (cmd_list != NULL) {
        g_strfreev(cmd_list);
    }
    g_variant_unref(property_value);

    return;
}


gint32 main(gint32 argc, gchar **argv)
{
    gint32 rc;
    gint32 i = 0;
    gchar **argv_inner = ipmc_clear_sensitive_info(argc, argv);
    if (argv_inner == NULL) {
        exit(EXIT_FAILURE);
    }

#ifdef ITEST
    iTest_Init(argc, argv_inner);
#endif

    dfl_init("cli");

    cli_cmd_init();

    
    shield_blcklist_set();
    

    rc = ipmc_main(argc, argv_inner);

#ifdef ITEST
    start_tshell_main(argc, argv_inner);
#endif
    while (i < argc && argv_inner[i] != NULL) {
        clear_sensitive_info(argv_inner[i], strlen(argv_inner[i]));
        free(argv_inner[i]);
        argv_inner[i] = NULL;
        i++;
    }

    free(argv_inner);
    argv_inner = NULL;
    
    dfl_exit();

    if (rc == FALSE) {
        exit(EXIT_FAILURE);
    } else {
        exit(EXIT_SUCCESS);
    }
}
