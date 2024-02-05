
#include <stdio.h>
#include <stdlib.h>
#include "uip.h"
#include "ipmc_public.h"
#include "ipmcget_service_mgmt.h"
#include "ipmcgetmain.h"
#include "ipmctypedef.h"
#include "ipmcgethelp.h"
#include "ipmcgetcommand.h"
#include "shelfgetcommand.h"
#include "ipmc_shelf_common.h"
#include "pme/pme.h"
#include "pme_app/pme_app.h"


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
    } else {
        if (cmd == NULL) {
            g_printf("input parameter<-d> error");
            g_printf("    -d <dataitem>\r\n");
            custom_print_cmd_list(get_g_bmc_cmd_list1());
            print_cmd_list(g_bmc_getcmd_fru0);
            return;
        }

        if (ipmc_find_matchcmd(cmd, argc, argv) == FALSE) {
            return;
        }
    }
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
    guchar t_level = 0;
    guchar l_level = 0;
    guchar need_l;
    gint32 argv_invalid_len_loop = 0;

    guchar MgmtSoftWareType = 0;

    
    if (argv == NULL) {
        return FALSE;
    }
    
    need_l = ipmc_need_l();
    if (argc == 1) {
        ipmc_print_helpinfo(need_l, l_level, NULL, t_level, NULL);
        return TRUE;
    }
    if (argc > MAX_PARAMETER) {
        ipmc_print_helpinfo(need_l, l_level, NULL, t_level, NULL);
        return TRUE;
    }
    
    
    if (need_l) {
        
        if (strcmp(argv[1], "-l") && strcmp(argv[1], "-t") && strcmp(argv[1], "-d") && strcmp(argv[1], "-h")) {
            ipmc_print_helpinfo(need_l, l_level, NULL, t_level, NULL);
            return TRUE;
        }
        (void)dal_get_software_type(&MgmtSoftWareType);

        
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
            safe_fun_ret = strncpy_s(need_l_shelf, SHORT_STRING_LEN, NEED_L_SHELF, strlen(NEED_L_SHELF));
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
            if (!strcmp(argv[1], "-l") && !strcmp(location, "smm")) {
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
                    break;
                }
                if (strlen(location_tmp) > SHORT_STRING_LEN - 1) {
                    free(location_tmp);
                    ipmc_print_helpinfo(need_l, l_level, NULL, t_level, NULL);
                    return FALSE;
                }
                
                safe_fun_ret = strncpy_s(location, SHORT_STRING_LEN, location_tmp, strlen(location_tmp));
                if (safe_fun_ret != EOK) {
                    debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
                }
                free(location_tmp);
                if (ipmc_find_matchlocation(location, sizeof(location)) == FALSE) {
                    return FALSE;
                }
                
                
                if (optind < argc && argv[optind] != NULL && strcmp(argv[optind], "-t") && strcmp(argv[optind], "-d") &&
                    (strcmp(argv[optind], "-h") || argv[optind + 1] != NULL)) {
                    
                    ipmc_print_helpinfo(need_l, l_level, location, 0, NULL);
                    
                    return FALSE;
                }
                break;
                
            case 'd':
                
                
                if (l_level != 0) {
                    
                    if ((argc > 3) && strcmp(argv[1], "-d") && strcmp(argv[3], "-d") && strcmp(argv[5], "-d")) {
                        
                        ipmc_print_helpinfo(need_l, l_level, location, t_level, target);
                        return TRUE;
                    }
                    
                } else if (argc > 3 && strcmp(argv[1], "-d") && strcmp(argv[3], "-d")) {
                    
                    ipmc_print_helpinfo(need_l, l_level, location, t_level, target);
                    return TRUE;
                }
                
                
                cmd = NULL;
                
                cmd = strdup(optarg);
                if (cmd == NULL) {
                    break;
                }
                if (strlen(cmd) > SHORT_STRING_LEN - 1) {
                    g_free(cmd);
                    ipmc_print_d_helpinfo(l_level, location, t_level, target);
                    return FALSE;
                }
                
                for (argv_invalid_len_loop = optind; argv_invalid_len_loop < argc; argv_invalid_len_loop++) {
                    if (argv[argv_invalid_len_loop] != NULL &&
                        strlen(argv[argv_invalid_len_loop]) > MAX_CLI_ARGV_LENGTH) {
                        free(cmd);
                        ipmc_print_helpinfo(need_l, l_level, location, t_level, target);
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
                    target_tmp = NULL;
                    ipmc_print_helpinfo(need_l, l_level, location, t_level, NULL);
                    return FALSE;
                }
                
                
                safe_fun_ret = strncpy_s(target, SHORT_STRING_LEN, target_tmp, strlen(target_tmp));
                if (safe_fun_ret != EOK) {
                    debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
                }
                
                
                free(target_tmp);
                

                if (optind < argc && argv[optind] != NULL && strcmp(argv[optind], "-d") &&
                    (strcmp(argv[optind], "-h") || argv[optind + 1] != NULL)) {
                    print_shelf_cli_helpinfo();
                    return FALSE;
                }

                if (l_level != 0) {
                    if (ipmc_find_match_l_target(location, target) == FALSE) {
                        return TRUE;
                    }
                } else {
                    if (ipmc_find_matchtarget(target) == FALSE) {
                        return TRUE;
                    }
                }
                break;

            default:
                
                if (MgmtSoftWareType == MGMT_SOFTWARE_TYPE_EM) {
                    if (optopt == 'l') {
                        if (ipmc_find_matchlocation("-h", strlen("-h")) == FALSE) {
                            return TRUE;
                        }
                    } else if (optopt == 't') {
                        if (ipmc_find_match_l_target(location, "-h") == FALSE) {
                            return TRUE;
                        }
                    } else if (optopt == 'd') {
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
    gint32 iRet = -1;
    const GVariantType *type = NULL;

    if (value == NULL || value_string == NULL) {
        return;
    }

    type = g_variant_get_type(value);
    
    if (g_variant_type_equal(type, G_VARIANT_TYPE_BOOLEAN) == TRUE) {
        snprintf_s(value_string, value_string_len, value_string_len - 1, "%c", (gchar)g_variant_get_boolean(value));
    } else if (g_variant_type_equal(type, G_VARIANT_TYPE_BYTE) == TRUE) {
        snprintf_s(value_string, value_string_len, value_string_len - 1, "%d", g_variant_get_byte(value));
    } else if (g_variant_type_equal(type, G_VARIANT_TYPE_INT16) == TRUE) {
        snprintf_s(value_string, value_string_len, value_string_len - 1, "%d", g_variant_get_int16(value));
    } else if (g_variant_type_equal(type, G_VARIANT_TYPE_UINT16) == TRUE) {
        snprintf_s(value_string, value_string_len, value_string_len - 1, "%u", g_variant_get_uint16(value));
    } else if (g_variant_type_equal(type, G_VARIANT_TYPE_INT32) == TRUE) {
        snprintf_s(value_string, value_string_len, value_string_len - 1, "%d", g_variant_get_int32(value));
    } else if (g_variant_type_equal(type, G_VARIANT_TYPE_UINT32) == TRUE) {
        snprintf_s(value_string, value_string_len, value_string_len - 1, "%u", g_variant_get_uint32(value));
    } else if (g_variant_type_equal(type, G_VARIANT_TYPE_INT64) == TRUE) {
        snprintf_s(value_string, value_string_len, value_string_len - 1, "%" G_GINT64_FORMAT,
            g_variant_get_int64(value));
    } else if (g_variant_type_equal(type, G_VARIANT_TYPE_UINT64) == TRUE) {
        snprintf_s(value_string, value_string_len, value_string_len - 1, "%" G_GUINT64_FORMAT,
            g_variant_get_uint64(value));
    } else if (g_variant_type_equal(type, G_VARIANT_TYPE_STRING) == TRUE) {
        iRet = snprintf_s(value_string, value_string_len, value_string_len - 1, "%s", g_variant_get_string(value, 0));
        if (iRet <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
        }
        
    } else {
        return;
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
    guint8 *cmd_flag)
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

    if (flag == 0) {
        *cmd_flag = UNOBVIOUS_CMD;
    }

    g_slist_free(obj_list);
    return;
}


LOCAL gboolean __is_shield_special_get_cmd(const char *cmd_name, guint8 *cmd_flag) 
{
    OBJ_HANDLE obj_handle = 0;
    
    gint32 ret = dal_get_object_handle_nth(CLASS_LSW_PUBLIC_ATTR_LOCAL_CP, 0, &obj_handle);
    if ((g_ascii_strcasecmp(cmd_name, "lswportinfo") == 0) && (ret == RET_OK)) {
        *cmd_flag = UNOBVIOUS_CMD;
        return TRUE;
    }

    // PCIe形态的NPU芯片不支持此命令，不要在命令行显示
    if ((g_ascii_strcasecmp(cmd_name, "npuworkmode") == 0) && (dal_is_pcie_npu() == TRUE)) {
        *cmd_flag = UNOBVIOUS_CMD;
        return TRUE;
    }

    return FALSE;
}

LOCAL void init_cli_cmd_list(struct helpinfo *cmd_list)
{
    gint32 cmd_num = 0;
    guchar MgmtSoftWareType = 0;
    gint32 ret = 0;
    guint8 asstatus = 0;
    if (cmd_list == NULL) {
        return;
    }

    (void)dal_get_software_type(&MgmtSoftWareType);
    while (cmd_list[cmd_num].name) {
        if (__is_shield_special_get_cmd(cmd_list[cmd_num].name, &cmd_list[cmd_num].flag) == TRUE) {
            cmd_num++;
            continue;
        }
        if (cmd_list[cmd_num].HideType == HIDE_TYPE_OBJ_NUM) {
            init_cli_cmd_by_obj_num(cmd_list[cmd_num].DepClassName, cmd_list[cmd_num].DepObjectNum,
                &cmd_list[cmd_num].flag);
        } else if (cmd_list[cmd_num].HideType == HIDE_TYPE_OBJ_NUM_AND_ACTIVE) {
            
            if (MgmtSoftWareType == MGMT_SOFTWARE_TYPE_EM) {
                ret = get_board_active_state(&asstatus);
                if (ret != DFL_OK) {
                    g_printf("get active state failed.\r\n");
                    return;
                }
                if (asstatus == STANDBY_STATE) {
                    cmd_list[cmd_num].flag = UNOBVIOUS_CMD;
                    cmd_num++;
                    continue;
                }
            }
            init_cli_cmd_by_obj_num(cmd_list[cmd_num].DepClassName, cmd_list[cmd_num].DepObjectNum,
                &cmd_list[cmd_num].flag);
        } else if (cmd_list[cmd_num].HideType == HIDE_TYPE_PROP) {
            init_cli_cmd_by_prop(cmd_list[cmd_num].DepClassName, cmd_list[cmd_num].DepPropertyName,
                cmd_list[cmd_num].DepPropetyValue, &cmd_list[cmd_num].flag);
        } else if (cmd_list[cmd_num].HideType == HIDE_TYPE_PROP_AND_ACTIVE) {
            
            if (MgmtSoftWareType == MGMT_SOFTWARE_TYPE_EM) {
                ret = get_board_active_state(&asstatus);
                if (ret != DFL_OK) {
                    g_printf("get active state failed.\r\n");
                    return;
                }
                if (asstatus == STANDBY_STATE) {
                    cmd_list[cmd_num].flag = UNOBVIOUS_CMD;
                    cmd_num++;
                    continue;
                }
            }
            init_cli_cmd_by_prop(cmd_list[cmd_num].DepClassName, cmd_list[cmd_num].DepPropertyName,
                cmd_list[cmd_num].DepPropetyValue, &cmd_list[cmd_num].flag);
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

LOCAL void init_cli_service_list(struct get_service *service_list)
{
    gint32 cmd_num = 0;

    if (service_list == NULL) {
        return;
    }

    while (service_list[cmd_num].ServiceName) {
        if (service_list[cmd_num].HideType == HIDE_TYPE_OBJ_NUM) {
            init_cli_cmd_by_obj_num(service_list[cmd_num].DepClassName, service_list[cmd_num].DepObjectNum,
                &service_list[cmd_num].flag);
        } else if (service_list[cmd_num].HideType == HIDE_TYPE_PROP) {
            init_cli_cmd_by_prop(service_list[cmd_num].DepClassName, service_list[cmd_num].DepPropertyName,
                service_list[cmd_num].DepPropetyValue, &service_list[cmd_num].flag);
        } else {
            cmd_num++;

            continue;
        }
        cmd_num++;
    }
    return;
}
LOCAL void cli_cmd_init(void)
{
    
    init_cli_cmd_list(get_g_bmc_cmd_list1());

    
    init_cli_cmd_list(get_g_bmc_cmd_list2());

    
    init_cli_cmd_list_d(get_g_bmc_cmd_list_d());

    
    init_cli_service_list(get_g_bmc_get_service_list());

    
    
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


LOCAL void init_cli_black_cmd_list_l_t_d(gchar *cmd_buf)
{
    gchar *cmd_buf_tmp = NULL;
    gint32 index_i;
    gchar ret;

    index_i = ipmc_get_matched_cmd(cmd_buf, get_g_bmc_cmd_list_l());
    if (index_i == IPMC_ERROR) {
        g_printf("Cli black list(ipmcget -l) match failed.\r\n");
        return;
    }

    
    if (get_g_bmc_cmd_list_l()[index_i].flag == UNOBVIOUS_CMD) {
        return;
    }

    cmd_buf_tmp = strstr(cmd_buf, "-t");
    if (cmd_buf_tmp != NULL) {
        cmd_buf_tmp = strstr(cmd_buf, "-d");
        if (cmd_buf_tmp == NULL) {
            ret = init_cli_black_cmd_list_2_level(cmd_buf, get_g_bmc_cmd_list_l(), get_g_bmc_cmd_list_l_t_bond());
            if (ret == FALSE) {
                g_printf("Cli black list(ipmcget -l xxx -t xxx) match failed.\r\n");
            }
            return;
        }
        ret = init_cli_black_cmd_list_2_level(cmd_buf, get_g_bmc_cmd_list_l_t_bond()[index_i].help_info,
            get_g_bmc_l_t_d_bond()[index_i].help_infoplus);
        if (ret == FALSE) {
            g_printf("Cli black list(ipmcget -l xxx -t xxx -d xxx) match failed.\r\n");
        }
        return;
    }
    cmd_buf_tmp = strstr(cmd_buf, "-d");
    if (cmd_buf_tmp != NULL) {
        ret = init_cli_black_cmd_list_2_level(cmd_buf, get_g_bmc_cmd_list_l(), get_g_bmc_cmd_list_l_d_bond());
        if (ret == FALSE) {
            g_printf("Cli black list(ipmcget -l xxx -d xxx) match failed.\r\n");
        }
        return;
    }

    ret = init_cli_black_cmd_list_1_level(cmd_buf, get_g_bmc_cmd_list_l());
    if (ret == FALSE) {
        g_printf("Cli black list(ipmcget -l) match failed.\r\n");
    }
    return;
}


LOCAL void init_cli_black_cmd_list_t_d(gchar *cmd_buf)
{
    gchar *cmd_buf_tmp = NULL;
    gchar ret;

    cmd_buf_tmp = strstr(cmd_buf, "-d");
    if (cmd_buf_tmp != NULL) {
        ret = init_cli_black_cmd_list_2_level(cmd_buf, get_g_bmc_cmd_list2(), get_g_bmc_cmd_list_d());
        if (ret == FALSE) {
            g_printf("Cli black list(ipmcget -t xxx -d xxx) match failed.\r\n");
        }
        return;
    }

    ret = init_cli_black_cmd_list_1_level(cmd_buf, get_g_bmc_cmd_list2());
    if (ret == FALSE) {
        g_printf("Cli black list(ipmcget -t) match failed.\r\n");
    }
    return;
}


LOCAL void shield_blcklist_get(void)
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
        
        cmd_buf = strstr(cmd_buf, "ipmcget");
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
                g_printf("Cli black list(ipmcget -d) match failed.\r\n");
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

    
    shield_blcklist_get();
    

    rc = ipmc_main(argc, argv_inner);

#ifdef ITEST
    start_tshell_main(argc, argv_inner);
#endif

    while (i < argc && argv_inner[i] != NULL) {
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
