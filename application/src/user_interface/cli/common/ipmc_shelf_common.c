
#include <stdio.h>
#include "pme_app/pme_app.h"
#include "uip.h"
#include "ipmctypedef.h"
#include "ipmc_shelf_common.h"
#include "ipmc_public.h"
#include "ipmcgetcommand.h"
#include "ipmcget_netconfig.h"

#define MAX_MATCH_LEN 256



gchar help_default_name[HELP_DEFAULT_NAME_LEN] = { 0 };

gchar product_type_name[HELP_DEFAULT_NAME_LEN] = { "iBMC" };

gchar need_l_shelf[NEED_L_SHELF_LEN] = { 0 };


LOCAL struct helpinfo *g_bmc_cmd_list_l;
void set_bmc_cmd_list_l(struct helpinfo *list)
{
    g_bmc_cmd_list_l = list;
}

LOCAL struct helpinfo *g_bmc_l_smm_t;
void set_bmc_l_smm_t(struct helpinfo *list)
{
    g_bmc_l_smm_t = list;
}

LOCAL struct helpinfo *g_bmc_l_smm_d;
void set_bmc_l_smm_d(struct helpinfo *list)
{
    g_bmc_l_smm_d = list;
}

LOCAL struct helpinfoplus *g_bmc_cmd_list_l_t_bond;
void set_bmc_cmd_list_l_t_bond(struct helpinfoplus *list)
{
    g_bmc_cmd_list_l_t_bond = list;
}

LOCAL struct helpinfoplus *g_bmc_cmd_list_l_d_bond;
void set_bmc_cmd_list_l_d_bond(struct helpinfoplus *list)
{
    g_bmc_cmd_list_l_d_bond = list;
}

LOCAL struct helpinfoplus_plus *g_bmc_l_t_d_bond;
void set_bmc_l_t_d_bond(struct helpinfoplus_plus *list)
{
    g_bmc_l_t_d_bond = list;
}

LOCAL struct helpinfo *g_bmc_cmd_list2;
void set_bmc_cmd_list2(struct helpinfo *list)
{
    g_bmc_cmd_list2 = list;
}

LOCAL struct helpinfoplus *g_bmc_cmd_list_d;
void set_bmc_cmd_list_d(struct helpinfoplus *list)
{
    g_bmc_cmd_list_d = list;
}
LOCAL struct helpinfo *g_bmc_cmd_list1;
void set_g_bmc_cmd_list1(struct helpinfo *list)
{
    g_bmc_cmd_list1 = list;
}


static helpinfo_func g_ipmc_helpinfo;

void set_ipmc_helpinfo_func(helpinfo_func fun)
{
    g_ipmc_helpinfo = fun;
}

LOCAL gint32 ipmc_find_target_cmd(gchar *location, gchar *target, gchar *cmd, gint32 argc, gchar **argv,
    struct helpinfoplus cmd_list_bond);
LOCAL void IpmcListSwiFanPemHelpinfo(gint32 count, gboolean isTce, gboolean isTce4U, gboolean isTce8U);
LOCAL gint32 IsSwiFanPermHelpinfo(const gchar *name);
LOCAL void list_multi_item_by_array(gint32 match_count, gint32 *match_list, struct helpinfo *item);
LOCAL void ipmc_l_helpinfo(void);
LOCAL gint32 ipmc_match_str_list(gchar *str, struct helpinfo *cmd_list, gint32 *match_list, guint32 match_count_len);
LOCAL gint32 blade_str_to_int(gchar *str);
LOCAL gboolean is_helpinfo_list_all_hide(struct helpinfo* cmd_list);


LOCAL gint8 ipmc_get_cmd_list_len(struct helpinfo *cmd_list)
{
    gint32 i = 0;

    while (cmd_list[i].name) {
        i++;
    }

    return i;
}


guchar ipmc_need_l(void)
{
    gint32 ret;
    OBJ_HANDLE obj_handle;
    gchar *obj_name = BMC_PRODUCT_OBJ_NAME_APP;
    GVariant *property_value = NULL;
    guchar l_enable;
    guint8 software_type = 0;

    (void)dal_get_software_type(&software_type);
    
    if (software_type == MGMT_SOFTWARE_TYPE_EM) {
        return 1;
    }

    
    ret = dfl_get_object_handle(obj_name, &obj_handle);
    if (ret != DFL_OK) {
        g_printf("Get object %s failed.\r\n", obj_name);
        return FALSE;
    }

    
    ret = dfl_get_property_value(obj_handle, PROPERTY_SHELF_CLI_ENABLE, &property_value);
    if (ret != DFL_OK) {
        g_printf("Get property %s failed.\r\n", PROPERTY_SHELF_CLI_ENABLE);
        return FALSE;
    }
    l_enable = g_variant_get_byte(property_value);
    g_variant_unref(property_value);

    return l_enable;
}


LOCAL guchar is_smm_location_valid(gchar *location)
{
    gint32 ret;
    guint8 num = 0;
    guchar slave_address = 0;
    guchar part_max_num = 0;
    guint32 fantray_max_num = 0;
    guint8 software_type = 0;
    gint32 board_num = 0;
    OBJ_HANDLE object_handle;

    if (location == NULL) {
        return FALSE;
    }

    (void)dal_get_software_type(&software_type);
    if (software_type == MGMT_SOFTWARE_TYPE_EM) {
        
        if (!(strcmp(location, "smm") && strcmp(location, "shelf") && strcmp(location, "lcd") &&
            strcmp(location, "fantray") && strcmp(location, "pem") && strcmp(location, "blade") &&
            strcmp(location, "swi"))) {
            return TRUE;
        }
    }

    
    if (!strncmp(location, "blade", strlen("blade"))) {
        num = ipmc_get_blade_num(location);
        if (num == 0xff) {
            return FALSE;
        }

        slave_address = ipmc_get_blade_ipmbaddr(num);
        ret = dal_get_specific_object_byte(CLASS_NAME_IPMBETH_BLADE, PROTERY_IPMBETH_BLADE_SLAVEADDR, slave_address,
            &object_handle);
        if (ret != RET_OK) {
            return FALSE;
        }

#define MAX_BLADE_NUM 32
        if (software_type == MGMT_SOFTWARE_TYPE_EM) {
            if (((num < 1 || num > MAX_BLADE_NUM) && (num != SMM1_BMC_BLADE_SLOT_ID) &&
                (num != SMM2_BMC_BLADE_SLOT_ID))) {
                return FALSE;
            }
        }
        return TRUE;
    }
    if (!strncmp(location, "swi", strlen("swi"))) {
#define MAX_SWI_NUM 4
        board_num = blade_str_to_int(location + strlen("swi"));
        num = ipmc_get_blade_num(location);
        if (num == 0xff) {
            return FALSE;
        }

        slave_address = ipmc_get_blade_ipmbaddr(num);
        ret = dal_get_specific_object_byte(CLASS_NAME_IPMBETH_BLADE, PROTERY_IPMBETH_BLADE_SLAVEADDR, slave_address,
            &object_handle);
        if (ret != RET_OK) {
            return FALSE;
        }

        if (board_num < 1 || board_num > MAX_SWI_NUM) {
            return FALSE;
        }
        return TRUE;
    }
    
    if (!(strncmp(location, "fantray", strlen("fantray")))) {
        ret = dfl_get_object_count(CLASS_NAME_SMM_FAN, &fantray_max_num);
        
        if (ret != DFL_OK) {
            return FALSE;
        }

        num = get_fantray_num(location);
        if (num >= 1 && num <= fantray_max_num) {
            return TRUE;
        }
    }
    
    if (!(strncmp(location, "pem", strlen("pem")))) {
        ret = dfl_get_object_handle(OBJ_NAME_AMMETER, &object_handle);
        
        if (ret != DFL_OK) {
            return FALSE;
        }
        ret = dal_get_property_value_byte(object_handle, PROPERTY_WORK_PS_COUNT, &part_max_num);
        if (ret != RET_OK) {
            return FALSE;
        }
        num = get_pem_num(location);
        if (num >= 1 && num <= part_max_num) {
            return TRUE;
        }
    }
    return FALSE;
}


LOCAL gint32 ipmc_match_location(gchar *str, struct helpinfo *cmd_list)
{
    gint32 list_index = 0;
    gint32 board_num = 0;
    const gchar *p = NULL;
    guint8 sub_len = 0;

    if (str == NULL || cmd_list == NULL) {
        return IPMC_ERROR;
    }

    while (cmd_list[list_index].name && str) {
        
        if (strcmp(cmd_list[list_index].name, str) == 0 && (cmd_list[list_index].flag != UNOBVIOUS_CMD)) {
            return list_index;
        }

        
        p = strchr(cmd_list[list_index].name, 'N');
        if (p != NULL) {
            sub_len = p - cmd_list[list_index].name;
            if ((strncmp(cmd_list[list_index].name, str, sub_len) == 0) &&
                (cmd_list[list_index].flag != UNOBVIOUS_CMD)) {
                if (strlen(str) - sub_len > 2) {
                    return IPMC_ERROR;
                }

                board_num = blade_str_to_int(str + sub_len);
                if (board_num == IPMC_ERROR) {
                    return IPMC_ERROR;
                }

                return list_index;
            }
        }

        list_index++;
    }

    return IPMC_ERROR;
}

LOCAL gint32 ipmc_fuzzy_match_cmdname(gchar *cmd_buf, struct helpinfo *cmd_list)
{
    gint32 list_index = 0;

    if (cmd_buf == NULL || cmd_list == NULL) {
        return IPMC_ERROR;
    }

    while (cmd_list[list_index].name && cmd_buf) {
        if ((strncmp(cmd_buf, cmd_list[list_index].name, strlen(cmd_buf)) == 0 &&
            (cmd_list[list_index].flag != UNOBVIOUS_CMD))) {
            return list_index;
        }
        list_index++;
    }
    return IPMC_ERROR;
}


gint32 ipmc_get_matched_cmd(gchar *cmd_buf, struct helpinfo *cmd_list)
{
    gint32 list_index = 0;
    gint32 name_len;
    gint32 sub_idx;
    gchar *sub_buf = NULL;

    if (cmd_buf == NULL || cmd_list == NULL) {
        return IPMC_ERROR;
    }

    while (cmd_list[list_index].name) {
        sub_buf = strstr(cmd_buf, cmd_list[list_index].name);
        if (sub_buf != NULL) {
            
            name_len = strlen(cmd_list[list_index].name);
            sub_idx = sub_buf - cmd_buf;
            if ((sub_idx == 0 || (sub_idx - 1 > 0 && cmd_buf[sub_idx - 1] == ' ')) &&
                (sub_buf[name_len] == ' ' || sub_buf[name_len] == '\0')) {
                return list_index;
            }
        }
        list_index++;
    }

    return IPMC_ERROR;
}


gint32 ipmc_find_matchlocation(gchar *location, guint32 size)
{
    guchar location_valid;
    gint32 list_index;

    if (location == NULL || size == 0) {
        return FALSE;
    }

    
    list_index = ipmc_match_location(location, g_bmc_cmd_list_l);
    if (list_index == IPMC_ERROR) {
        ipmc_print_helpinfo(1, 1, NULL, 0, NULL);
        return FALSE;
    }

    
    location_valid = is_smm_location_valid(location);
    if (location_valid == FALSE) {
        ipmc_print_helpinfo(1, 1, NULL, 0, NULL);
        return FALSE;
    }

    return TRUE;
}

gchar init_cli_black_cmd_list_1_level(gchar *cmd_buf, struct helpinfo *cmd_list)
{
    gint32 cmd_num;

    if (cmd_buf == NULL || cmd_list == NULL) {
        return FALSE;
    }

    cmd_num = ipmc_get_matched_cmd(cmd_buf, cmd_list);
    if (cmd_num != IPMC_ERROR) {
        
        cmd_list[cmd_num].flag = UNOBVIOUS_CMD;
        return TRUE;
    }

    return FALSE;
}


gchar init_cli_black_cmd_list_2_level(gchar *cmd_buf, struct helpinfo *cmdlist, struct helpinfoplus *cmdlist_plus)
{
    gint32 cmd_num;
    struct helpinfo *cmd_list_plus = NULL;

    if (cmd_buf == NULL || cmdlist == NULL || cmdlist_plus == NULL) {
        return FALSE;
    }

    
    cmd_num = ipmc_get_matched_cmd(cmd_buf, cmdlist);
    if (cmd_num == IPMC_ERROR) {
        return FALSE;
    }

    
    cmd_list_plus = cmdlist_plus[cmd_num].help_info;
    cmd_num = ipmc_get_matched_cmd(cmd_buf, cmd_list_plus);
    if (cmd_num != IPMC_ERROR) {
        
        cmd_list_plus[cmd_num].flag = UNOBVIOUS_CMD;
        return TRUE;
    }

    
    return FALSE;
}


void ipmc_print_d_helpinfo(guchar l_level, gchar *location, guchar t_level, gchar *target)
{
    gint32 i = 0;
    gint32 j = 0;
    gint32 match_list[MAX_MATCH_LEN] = {0};

    g_printf("Input parameter[-d] error\r\n");
    g_printf("    -d <dataitem>\r\n");

    if (l_level) {
        i = ipmc_match_location(location, g_bmc_cmd_list_l);
        if (i == IPMC_ERROR) {
            g_printf("Input parameter[-l] error\r\n");
            ipmc_l_helpinfo();
            return;
        }
        if (t_level) {
            j = ipmc_fuzzy_match_cmdname(target, g_bmc_cmd_list_l_t_bond[i].help_info);
            if (j == IPMC_ERROR) {
                g_printf("    -t <target>\r\n");
                print_cmd_list(g_bmc_cmd_list_l_t_bond[i].help_info);
                return;
            }
            print_cmd_list(g_bmc_l_t_d_bond[i].help_infoplus[j].help_info);
        } else {
            print_cmd_list(g_bmc_cmd_list_l_d_bond[i].help_info);
        }
    } else {
        if (t_level) {
            (void)ipmc_match_str_list(target, g_bmc_cmd_list2, match_list, MAX_MATCH_LEN);
            i = match_list[0];
            print_cmd_list(g_bmc_cmd_list_d[i].help_info);
        } else {
            custom_print_cmd_list(g_bmc_cmd_list1);
        }
    }
}
LOCAL void print_l_help_info(gint32 index_i, gchar *location)
{
    gboolean is_all_cmd_hide;

    is_all_cmd_hide = is_helpinfo_list_all_hide(g_bmc_cmd_list_l_t_bond[index_i].help_info);
    if (!is_all_cmd_hide) {
        g_printf("    -t <target>\r\n");
    }
    if (ipmc_get_cmd_list_len(g_bmc_cmd_list_l_t_bond[index_i].help_info) == 0) {
        g_printf("    Input parameter [-l %s] does not support cmd [-t target]\r\n", location);
    } else {
        print_cmd_list(g_bmc_cmd_list_l_t_bond[index_i].help_info);
    }
    g_printf("\r\n");
    return;
}


void ipmc_print_helpinfo(guchar need_l, guchar l_level, gchar *location, guchar t_level, gchar *target)
{
    gint32 index_i = 0;
    gint32 index_j = 0;
    guchar MgmtSoftWareType = 0;
    gint32 ret = 0;

    ret = dal_get_software_type(&MgmtSoftWareType);
    if (ret != DFL_OK) {
        return;
    }
    
    if (need_l) {
        
        
        if (!l_level && !t_level) {
            print_shelf_cli_helpinfo();
            ipmc_l_helpinfo();
            
            
            if (MgmtSoftWareType == MGMT_SOFTWARE_TYPE_EM) {
                g_printf("\r\n    -t <target>\r\n");
                print_cmd_list(g_bmc_l_smm_t);
                g_printf("\r\n    -d <dataitem>\r\n");
                print_cmd_list(g_bmc_l_smm_d);
            }
            
            else {
                g_printf("\r\n    -t <target>\r\n");
                print_cmd_list(g_bmc_cmd_list2);
                g_printf("\r\n    -d <dataitem>\r\n");
                custom_print_cmd_list(g_bmc_cmd_list1);
            }
        }
        
        else if (l_level && !t_level) {
            index_i = ipmc_match_location(location, g_bmc_cmd_list_l);
            if (index_i == IPMC_ERROR) {
                g_printf("Input parameter[-l] error\r\n");
                ipmc_l_helpinfo();
                return;
            }
            g_printf("Input parameter error\r\n");
            print_shelf_cli_helpinfo();

            // only MM920 accept -l location -t target
            if (MgmtSoftWareType == MGMT_SOFTWARE_TYPE_EM) {
                print_l_help_info(index_i, location);
            }
            g_printf("    -d <dataitem>\r\n");
            print_cmd_list(g_bmc_cmd_list_l_d_bond[index_i].help_info);
        }
        
        else if (l_level && t_level) {
            index_i = ipmc_match_location(location, g_bmc_cmd_list_l);
            if (index_i == IPMC_ERROR) {
                g_printf("Input parameter[-l] error\r\n");
                ipmc_l_helpinfo();
                return;
            }

            
            index_j = ipmc_fuzzy_match_cmdname(target, g_bmc_cmd_list_l_t_bond[index_i].help_info);
            if (index_j == IPMC_ERROR) {
                print_l_help_info(index_i, location);
                return;
            }
            g_printf("    -d <dataitem>\r\n");
            print_cmd_list((g_bmc_l_t_d_bond[index_i].help_infoplus[index_j].help_info));
        } else {
            g_ipmc_helpinfo(t_level, target);
        }
    } else {
        
        g_ipmc_helpinfo(t_level, target);
    }
}


LOCAL gboolean is_helpinfo_list_all_hide(struct helpinfo* cmd_list)
{
    gint32 i = 0;
    while (cmd_list[i].name) {
        if (cmd_list[i].flag == OBVIOUS_CMD) {
            return FALSE;
        }
        i++;
    }
    return TRUE;
}


gint32 ipmc_find_l_t_d_cmd(gchar *location, gchar *target, gchar *cmd, gint32 argc, gchar **argv)
{
    gint32 i = 0;
    gint32 j = 0;
    guint8 software_type = 0;

    (void)dal_get_software_type(&software_type);
    if (software_type == MGMT_SOFTWARE_TYPE_EM) {
        
        i = ipmc_match_location(location, g_bmc_cmd_list_l);
        if (i == IPMC_ERROR) {
            ipmc_print_helpinfo(1, 1, NULL, 0, NULL);
            return FALSE;
        }

        
        j = ipmc_fuzzy_match_cmdname(target, g_bmc_cmd_list_l_t_bond[i].help_info);
        if (j == IPMC_ERROR) {
            g_printf("    -t <target>\r\n");
            print_cmd_list(g_bmc_cmd_list_l_t_bond[i].help_info);
            return FALSE;
        }

        return ipmc_find_target_cmd(location, target, cmd, argc, argv, g_bmc_l_t_d_bond[i].help_infoplus[j]);
    } else {
        // MM630,MM650,MM810暂时没l t参数
        ipmc_print_helpinfo(1, 1, location, 0, NULL);
        return FALSE;
    }
}


gint32 ipmc_find_l_d_cmd(gchar *location, gchar *cmd, gint32 argc, gchar **argv)
{
    gint32 match_count;
    gint32 match_list[MAX_MATCH_LEN] = {0};
    gint32 i;
    gint32 j = 0;
    gint32 ret;
    
    i = ipmc_match_location(location, g_bmc_cmd_list_l);
    
    // 清理codedex
    if (i == IPMC_ERROR) {
        ipmc_print_helpinfo(1, 1, NULL, 0, NULL);
        return FALSE;
    }
    

    match_count = ipmc_match_str_list(cmd, g_bmc_cmd_list_l_d_bond[i].help_info, match_list, MAX_MATCH_LEN);
    
    if (match_count == 0) {
        g_printf("Input parameter[-d] error\r\n");
        g_printf("    -d <dataitem>\r\n");
        print_cmd_list(g_bmc_cmd_list_l_d_bond[i].help_info);
        return FALSE;
    }
    
    if (match_count == 1) {
        j = match_list[0];
        ret = cli_system_lockdown_check(g_bmc_cmd_list_l_d_bond[i].help_info[j].syslock_allow);
        if (ret != RET_OK) {
            g_printf("Error: Unable to complete the operation because the server is in the lockdown mode.\r\n");
            return TRUE;
        }

        return g_bmc_cmd_list_l_d_bond[i].help_info[j].func(g_bmc_cmd_list_l_d_bond[i].help_info[j].privilege,
            g_bmc_cmd_list_l_d_bond[i].help_info[j].rolepriv, (guchar)argc, argv, location);
    }
    
    if (match_count > 1) {
        g_printf("\r\n    -d <dataitem>\r\n");
        list_multi_item_by_array(match_count, match_list, g_bmc_cmd_list_l_d_bond[i].help_info);
        return FALSE;
    }
    return TRUE;
}


gint32 ipmc_find_match_l_target(gchar *location, gchar *target)
{
    gint32 match_count = 0;
    gint32 match_list[MAX_MATCH_LEN] = { 0 };
    gint32 index_i = 0;
    struct helpinfo *cmdlist_plus = NULL;
    guint8 software_type = 0;

    (void)dal_get_software_type(&software_type);

    if (software_type == MGMT_SOFTWARE_TYPE_EM) {
        index_i = ipmc_match_location(location, g_bmc_cmd_list_l);
        if (index_i == IPMC_ERROR) {
            return FALSE;
        }

        if (ipmc_get_cmd_list_len(g_bmc_cmd_list_l_t_bond[index_i].help_info) == 0) {
            g_printf("Input parameter [-l %s] does not support cmd [-t target]\r\n", location);
            return FALSE;
        }

        cmdlist_plus = g_bmc_cmd_list_l_t_bond[index_i].help_info;
        match_count = ipmc_match_str_list(target, cmdlist_plus, match_list, MAX_MATCH_LEN);
        
        if (match_count == 1) {
            return TRUE;
        }
        
        else if (match_count == 0) {
            g_printf("Input parameter[-t] error\r\n");
            g_printf("    -t <target>\r\n");
            print_cmd_list(cmdlist_plus);
        }
        
        else {
            g_printf("    -t <target>\r\n");
            list_multi_item_by_array(match_count, match_list, cmdlist_plus);
        }
    } else {
        // MM630/MM650/MM810没有-l xxx -t的命令
        ipmc_print_helpinfo(1, 1, location, 0, NULL);
    }

    return FALSE;
}


LOCAL gint32 ipmc_find_target_cmd(gchar *location, gchar *target, gchar *cmd, gint32 argc, gchar **argv,
    struct helpinfoplus cmd_list_bond)
{
    gint32 match_count;
    gint32 match_list[MAX_MATCH_LEN] = {0};
    gint32 j = 0;
    gint32 ret;

    match_count = ipmc_match_str_list(cmd, cmd_list_bond.help_info, match_list, MAX_MATCH_LEN);
    
    if (match_count == 0) {
        g_printf("Input parameter[-d] error\r\n");
        g_printf("    -d <dataitem>\r\n");
        print_cmd_list(cmd_list_bond.help_info);
        return FALSE;
    }

    
    if (match_count == 1) {
        j = match_list[0];
        // 系统锁定检查
        ret = cli_system_lockdown_check(cmd_list_bond.help_info[j].syslock_allow);
        if (ret != RET_OK) {
            g_printf("Error: Unable to complete the operation because the server is in the lockdown mode.\r\n");
            return TRUE;
        }

        
        if (!strcmp(location, SHELF_MGMT_DEFAULT_NAME)) {
            return cmd_list_bond.help_info[j].func(cmd_list_bond.help_info[j].privilege,
                cmd_list_bond.help_info[j].rolepriv, (guchar)argc, argv, target);
        } else {
            return cmd_list_bond.help_info[j].func_l(cmd_list_bond.help_info[j].privilege,
                cmd_list_bond.help_info[j].rolepriv, (guchar)argc, argv, target, location);
        }
    }

    
    if (match_count > 1) {
        g_printf("\r\n    -d <dataitem>\r\n");
        list_multi_item_by_array(match_count, match_list, cmd_list_bond.help_info);
        return FALSE;
    }

    return TRUE;
}



void list_special_item(guint64 flag, struct helpinfo *item)
{
    guint32 i = 0;
    guint64 count = 0;

    if (item == NULL) {
        g_printf("%s:item is NULL\n", __func__);
        return;
    }

    while (flag) {
        if (flag & ((guint64)1 << i)) {
            count = (guint64)1 << i;
            flag = flag ^ count;
            if (item[i].flag == OBVIOUS_CMD) {
                g_printf("       %-23s%-50s\r\n", item[i].name, item[i].desc);
            }
        }

        i++;
    }
}


void custom_print_cmd_list(struct helpinfo* cmd_list)
{
    gint32 index = 0;
    gint32 ret;

    if (cmd_list == NULL) {
        debug_log(DLOG_ERROR, "%s: ptr is null.\n", __func__);
        return;
    }

    while (cmd_list[index].name != NULL) {
        if (OBVIOUS_CMD == cmd_list[index].flag) {
            ret = dal_cmd_custom_specail_string(cmd_list, index);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s: dal_cmd_custom_specail_string fial ret = %d\n", __func__, ret);
                break;
            }
        }
        index++;
    }
    return;
}

void print_cmd_list(struct helpinfo *cmd_list)
{
    gint32 i = 0;

    while (cmd_list[i].name) {
        if (cmd_list[i].flag == OBVIOUS_CMD) {
            
            g_printf("       %-30s%-50s\r\n", cmd_list[i].name, cmd_list[i].desc);
            
        }

        i++;
    }
}


LOCAL void list_multi_item_by_array(gint32 match_count, gint32 *match_list, struct helpinfo *item)
{
    gint32 i;
    gint32 list_index = 0;

    if (item == NULL) {
        g_printf("%s:item is NULL\n", __func__);
        return;
    }
    for (i = 0; i < match_count; i++) {
        list_index = match_list[i];
        if (item[list_index].flag == OBVIOUS_CMD) {
            g_printf("       %-23s%-50s\r\n", item[list_index].name, item[list_index].desc);
        }
    }
    return;
}


LOCAL gint32 ipmc_match_str_list(gchar *str, struct helpinfo *cmd_list, gint32 *match_list, guint32 match_count_len)
{
    gint32 i = 0;
    gint32 match_count = 0;
    
    if (str == NULL || cmd_list == NULL || match_list == NULL || match_count_len == 0) {
        return match_count;
    }

    while (cmd_list[i].name) {
        if ((strcmp(str, cmd_list[i].name) == 0) && (cmd_list[i].flag != UNOBVIOUS_CMD)) {
            *match_list = i;
            match_count++;
            return match_count;
        }
        i++;
    }
    i = 0;
    
    while (cmd_list[i].name && ((guint32)match_count < match_count_len)) {
        if ((strncmp(str, cmd_list[i].name, strlen(str)) == 0) && (cmd_list[i].flag != UNOBVIOUS_CMD)) {
            *match_list++ = i;
            match_count++;
        }
        i++;
    }
    return match_count;
}


LOCAL void ipmc_l_helpinfo(void)
{
    g_printf("    -l <location>\r\n");
    guint32 blade_num = 0;
    gint32 count = 0;
    guchar MgmtSoftWareType = 0;
    gint32 ret;
    gboolean isTce = FALSE;
    gboolean isTce4U = FALSE;
    gboolean isTce8U = FALSE;

    ret = dal_get_software_type(&MgmtSoftWareType);
    if (ret != DFL_OK) {
        return;
    }
    
    if (MgmtSoftWareType == MGMT_SOFTWARE_TYPE_EM) {
        isTce = dal_match_product_id(PRODUCT_ID_TCE);
        isTce4U = dal_match_backplane_type(PRODUCT_ID_TCE, NORMAL_BACKPLANE_4U);
        isTce8U = dal_match_backplane_type(PRODUCT_ID_TCE, NORMAL_BACKPLANE_8U);
        while (g_bmc_cmd_list_l[count].name) {
            
            if (g_bmc_cmd_list_l[count].flag != OBVIOUS_CMD) {
                count++;
                continue;
            }
            if (strcmp("shelf", g_bmc_cmd_list_l[count].name) == 0) {
                g_printf("       %-30s%-50s\r\n", g_bmc_cmd_list_l[count].name,
                    "Shelf information,      option: shelf");
            } else if (strcmp("smm", g_bmc_cmd_list_l[count].name) == 0) {
                g_printf("       %-30s%-50s\r\n", g_bmc_cmd_list_l[count].name,
                    "Management board,       option: smm (default)");
            } else if (strcmp("blade", g_bmc_cmd_list_l[count].name) == 0) {
                g_printf("       %-30s%-50s\r\n", g_bmc_cmd_list_l[count].name,
                    "Blade server,           option: blade");
            } else if (strcmp("bladeN", g_bmc_cmd_list_l[count].name) == 0) {
                
                if (isTce4U) {
                    
                    g_printf("       %-30s%-50s\r\n", g_bmc_cmd_list_l[count].name,
                        "Blade server,           option: bladeN (1<=N<=4)");
                } else if (isTce8U) {
                    
                    g_printf("       %-30s%-50s\r\n", g_bmc_cmd_list_l[count].name,
                        "Blade server,           option: bladeN (1<=N<=8)");
                } else {
                    g_printf("       %-30s%-50s\r\n", g_bmc_cmd_list_l[count].name,
                        "Blade server,           option: bladeN (1<=N<=32 or N=37,38)");
                }
            } else if (IsSwiFanPermHelpinfo(g_bmc_cmd_list_l[count].name) == RET_OK) {
                IpmcListSwiFanPemHelpinfo(count, isTce, isTce4U, isTce8U);
            } else if (strcmp("lcd", g_bmc_cmd_list_l[count].name) == 0) {
                g_printf("       %-30s%-50s\r\n", g_bmc_cmd_list_l[count].name, "Liquid crystal display, option: lcd");
            }
            
            else {
                g_printf("Invalid Location List Setup.\r\n");
                return;
            }
            count++;
        }
    }
    
    else {
        while (g_bmc_cmd_list_l[count].name) {
            
            if (g_bmc_cmd_list_l[count].flag != OBVIOUS_CMD) {
                count++;
                continue;
            }

            if (strcmp("bladeN", g_bmc_cmd_list_l[count].name) == 0) {
                (void)dfl_get_object_count(CLASS_NAME_IPMBETH_BLADE, &blade_num);
                g_printf("       %-30s%-50s(1<=N<=%d)\r\n", g_bmc_cmd_list_l[count].name, g_bmc_cmd_list_l[count].desc,
                    blade_num);
            }

            count++;
        }
    }
    return;
}


LOCAL void IpmcListSwiFanPemHelpinfo(gint32 count, gboolean isTce, gboolean isTce4U, gboolean isTce8U)
{
    if (strcmp("swi", g_bmc_cmd_list_l[count].name) == 0) {
        if (!isTce) {
            
            g_printf("       %-30s%-50s\r\n", g_bmc_cmd_list_l[count].name, "Switch board,           option: swi");
        }
    } else if (strcmp("swiN", g_bmc_cmd_list_l[count].name) == 0) {
        if (!isTce) {
            
            g_printf("       %-30s%-50s\r\n", g_bmc_cmd_list_l[count].name,
                "Switch board,           option: swiN (1<=N<=4)");
        }
    } else if (strcmp("fantray", g_bmc_cmd_list_l[count].name) == 0) {
        g_printf("       %-30s%-50s\r\n", g_bmc_cmd_list_l[count].name, "Fan module,             option: fantray");
    } else if (strcmp("fantrayN", g_bmc_cmd_list_l[count].name) == 0) {
        if (isTce4U) {
            
            g_printf("       %-30s%-50s\r\n", g_bmc_cmd_list_l[count].name,
                "Fan module,             option: fantrayN (1<=N<=8)");
        } else if (isTce8U) {
            
            g_printf("       %-30s%-50s\r\n", g_bmc_cmd_list_l[count].name,
                "Fan module,             option: fantrayN (1<=N<=16)");
        } else {
            g_printf("       %-30s%-50s\r\n", g_bmc_cmd_list_l[count].name,
                "Fan module,             option: fantrayN (1<=N<=14)");
        }
    } else if (strcmp("pem", g_bmc_cmd_list_l[count].name) == 0) {
        g_printf("       %-30s%-50s\r\n", g_bmc_cmd_list_l[count].name, "Power entry module,     option: pem");
    } else if (strcmp("pemN", g_bmc_cmd_list_l[count].name) == 0) {
        if (isTce4U) {
            
            g_printf("       %-30s%-50s\r\n", g_bmc_cmd_list_l[count].name,
                "Power entry module,     option: pemN (1<=N<=4)");
        } else if (isTce8U) {
            
            g_printf("       %-30s%-50s\r\n", g_bmc_cmd_list_l[count].name,
                "Power entry module,     option: pemN (1<=N<=8)");
        } else {
            g_printf("       %-30s%-50s\r\n", g_bmc_cmd_list_l[count].name,
                "Power entry module,     option: pemN (1<=N<=6)");
        }
    }
}

LOCAL gint32 IsSwiFanPermHelpinfo(const gchar *name)
{
    if ((strcmp("swi", name) == 0) || (strcmp("swiN", name) == 0)) {
        return RET_OK;
    }
    if ((strcmp("fantray", name) == 0) || (strcmp("fantrayN", name) == 0)) {
        return RET_OK;
    }
    if ((strcmp("pem", name) == 0) || (strcmp("pemN", name) == 0)) {
        return RET_OK;
    }
    return RET_ERR;
}


gint32 get_blade_presence(OBJ_HANDLE object_handle)
{
    gint32 ret;
    GVariant *property_value = NULL;
    gint32 presence;
    
    ret = dfl_get_property_value(object_handle, PROTERY_IPMBETH_BLADE_PRESENCE, &property_value);
    if (ret != DFL_OK) {
        g_printf("Get property %s failed.\r\n", PROTERY_IPMBETH_BLADE_PRESENCE);
        return FALSE;
    }
    presence = g_variant_get_byte(property_value);
    g_variant_unref(property_value);
    property_value = NULL;
    return presence;
}


gint32 get_ipmbeth_blade_name(guint8 slave_addr, gchar *smm_name, guint8 name_len)
{
    errno_t safe_fun_ret;
    GVariant *property_value = NULL;
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    const gchar *obj_name = NULL;

    property_value = g_variant_new_byte(slave_addr);
    ret =
        dfl_get_specific_object(CLASS_NAME_IPMBETH_BLADE, PROTERY_IPMBETH_BLADE_SLAVEADDR, property_value, &obj_handle);
    g_variant_unref(property_value);
    if (ret != DFL_OK) {
        g_printf("Find blade object failed.\r\n");
        return RET_ERR;
    }

    obj_name = dfl_get_object_name(obj_handle);
    safe_fun_ret = strncpy_s(smm_name, name_len, obj_name, strlen(obj_name));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }

    return RET_OK;
}


LOCAL guchar get_blade_present_state(guchar slave_addr)
{
    OBJ_HANDLE obj_handle = 0;
    guchar present = 0;

    (void)dal_get_specific_object_byte(CLASS_NAME_IPMBETH_BLADE, PROTERY_IPMBETH_BLADE_SLAVEADDR, slave_addr,
        &obj_handle);

    (void)dal_get_property_value_byte(obj_handle, PROTERY_IPMBETH_BLADE_PRESENCE, &present);

    return present;
}


LOCAL gint32 get_blade_communication_status(guchar slave_addr)
{
    OBJ_HANDLE obj_handle = 0;
    guchar com_lost = 0;
    guchar re_lost = 0;

    (void)dal_get_specific_object_byte(CLASS_NAME_IPMBETH_BLADE, PROTERY_IPMBETH_BLADE_SLAVEADDR, slave_addr,
        &obj_handle);

    (void)dal_get_property_value_byte(obj_handle, PROTERY_IPMBETH_BLADE_HEARTBEAT, &com_lost);
    (void)dal_get_property_value_byte(obj_handle, PROTERY_IPMBETH_BLADE_REBOOTFLAG, &re_lost);

    if (com_lost == PROTERY_IPMBETH_BLADE_HEARTBEAT_LOST || re_lost == PROTERY_IPMBETH_BLADE_HEARTBEAT_LOST) {
        return IPMC_ERROR;
    }

    return IPMC_OK;
}


gint32 check_blade_present_communication_status(gchar *location)
{
    errno_t safe_fun_ret;
    gint32 ret;
    guint8 slave_addr;
    guint8 slot_id;
    guint8 blade_present;
    gchar blade_name[NAME_LEN] = { 0 };

    if (location == NULL) {
        return IPMC_ERROR;
    }
    safe_fun_ret = strncpy_s(blade_name, sizeof(blade_name), location, strlen(location));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    blade_name[0] = blade_name[0] - ('a' - 'A');

    
    slot_id = ipmc_get_blade_num(location);
    if (slot_id == 255) {
        g_printf("Invalid blade name.\r\n");
        return IPMC_ERROR;
    }
    slave_addr = ipmc_get_blade_ipmbaddr(slot_id);
    blade_present = get_blade_present_state(slave_addr);
    if (blade_present == 0) {
        g_printf("%s is not present.\r\n", blade_name);
        return IPMC_ERROR;
    }

    ret = get_blade_communication_status(slave_addr);
    if (ret == IPMC_ERROR) {
        g_printf("%s communication lost.\r\n", blade_name);
        return IPMC_ERROR;
    }

    return IPMC_OK;
}

LOCAL gint32 blade_str_to_int(gchar *str)
{
    gchar *pfind = NULL;
    gint32 val = 0;
    if (str == NULL) {
        return IPMC_ERROR;
    }
    pfind = str;
    while (*pfind != '\0') {
        if ((*pfind < '0') || (*pfind > '9')) {
            return IPMC_ERROR;
        }
        if (val > (INT_MAX / 10)) {
            return IPMC_ERROR;
        }
        val = val * 10 + (*pfind - '0');
        pfind++;
    }
    return val;
}

gboolean is_support_bios_configuration(void)
{
    guchar arm_enable = DISABLE;
    guchar x86_enable = DISABLE;

    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_ARM, &arm_enable);
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_X86, &x86_enable);

    // 当前配置如果为x86_enable使能或者arm_enable使能，则说明当前设备支持bios配置
    if ((x86_enable == ENABLE) || (arm_enable == ENABLE)) {
        return TRUE;
    }

    return FALSE;
}


guchar ipmc_get_blade_num(gchar *plocation)
{
    size_t loc_len;
    gint32 board_num = 0;

    if (plocation == NULL) {
        return 0xff;
    }

    loc_len = strlen(plocation);
    if (!(strncmp("blade", plocation, strlen("blade"))) && (loc_len > 5) && (loc_len < 8)) {
        
        board_num = blade_str_to_int(plocation + strlen("blade"));
        
        // if条件之一值bladex长度小于8，所以数字只能是1位或者2位
        return (guchar)board_num;
        
    } else if (!(strncmp("swi", plocation, strlen("swi"))) && (loc_len > 3) && (loc_len < 5)) {
        board_num = blade_str_to_int(plocation + strlen("swi"));
        
        return (guchar)(board_num + 32);
        
    }
    return 0xff;
}


guchar ipmc_get_blade_ipmbaddr(guchar num)
{
    if ((0x80 + 2 * num) > 255) {
        return 255;
    }

    
    return (guchar)(0x80 + 2 * num);
    
}

gint32 cli_system_lockdown_check(guint8 allow)
{
    gint32 ret;
    guint8 sys_lockdown_state = 0;
    guint8 sys_lockdown_support = 0;

    
    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROPERTY_PME_SERVICECONFIG_SYSTEM_LOCKDOWN_SUPPORT,
        &sys_lockdown_support);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get system lockdown support failed\n");
        return RET_ERR;
    }
    if (sys_lockdown_support == PME_SERVICE_UNSUPPORT) {
        return RET_OK;
    }

    ret = dal_get_func_ability(CLASS_SECURITY_ENHANCE, PROPERTY_SECURITY_SYSTEMLOCKDOWNSTATE, &sys_lockdown_state);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get system lockdown status failed\n");
        return RET_ERR;
    }
    if (sys_lockdown_state == DISABLE) {
        return RET_OK;
    }

    return allow == SYS_LOCKDOWN_FORBID ? RET_ERR : RET_OK;
}


gint32 ascend_mode_info(OBJ_HANDLE obj_handle, gpointer data)
{
    guint8 board_id = 0;
    guint8 work_mode = 0;
    guint8 npu_id = 0;
    OBJ_HANDLE board_handle;
    gchar* work_mode_str[] = {
        "AMP",
        "SMP"
    };

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_NPU_ID, &npu_id);
    if (npu_id % NPU_NUMBER_ON_BOARD == 1) {
        (void)dfl_get_referenced_object(obj_handle, PROPERTY_NPU_DBOARD, &board_handle);
        (void)dal_get_property_value_byte(board_handle, PROPERTY_BOARD_ID, &board_id);

        (void)dal_get_extern_value_byte(obj_handle, PROPERTY_NPU_MODE, &work_mode, DF_AUTO);
        if (work_mode != 0 && work_mode != 1) {
            return RET_ERR;
        }

        g_printf("------------------ NPU BOARD WORK MODE ------------------\r\n");
        g_printf("%-30s: %d\r\n", "NPU BOARD ID ", board_id);
        g_printf("%-30s: %s\r\n", "WORK MODE", work_mode_str[work_mode]);
    }
    return RET_OK;
}


gchar **ipmc_clear_sensitive_info(gint32 argc, gchar **argv)
{
    gint32 i;
    gint32 value_pos = argc;
    gint32 ret;
    const gint32 MAX_PARA_NUM = 32;
    const gint32 MAX_PARA_STRING_LEN = 1024;
    if (argc > MAX_PARA_NUM || argv == NULL) {
        g_printf("input para error.\r\n");
        return NULL;
    }

    // argv参数后面多一个空指针，申请的时候也多申请一个空指针
    gchar **argv_inner = (gchar **)g_malloc0((argc + 1) * sizeof(gchar *));
    if (argv_inner == NULL) {
        g_printf("memory alloc error.\r\n");
        return NULL;
    }

    for (i = 0; i < argc; i++) {
        if (argv[i] == NULL || strlen(argv[i]) > MAX_PARA_STRING_LEN) {
            g_printf("input para error.\r\n");
            goto EXIT_ERR;
        }
        argv_inner[i] = (gchar *)g_malloc0(strlen(argv[i]) + 1);
        if (argv_inner[i] == NULL) {
            g_printf("memory alloc error.\r\n");
            goto EXIT_ERR;
        }
        // 将命令行中参数拷贝到argv_inner供后续接口使用
        ret = strncpy_s(argv_inner[i], strlen(argv[i]) + 1, argv[i], strlen(argv[i]));
        if (ret != EOK) {
            g_printf("memory copy error.\r\n");
            goto EXIT_ERR;
        }
        *(argv_inner[i] + strlen(argv[i])) = '\0';
        // 找到-v参数，-v参数后的用户参数进行脱敏
        if (strcmp(argv[i], "-v") == 0) {
            value_pos = i;
        }
        if (i > value_pos) {
            (void)memset_s((void *)argv[i], strlen(argv[i]), '*', strlen(argv[i]));
        }
    }

    return argv_inner;

EXIT_ERR:
    i = 0;
    while (i < argc && argv_inner[i] != NULL) {
        free(argv_inner[i]);
        argv_inner[i] = NULL;
        i++;
    }

    free(argv_inner);
    argv_inner = NULL;

    return NULL;
}


gint32 get_username_ip(gchar *username, gchar *ip, guint32 ip_size)
{
    gint32 iRet;
    const gchar *user_name_temp = NULL;
    const gchar *ip_temp = NULL;
    guint32 i = 0;
    gint32 space = 0;

    if ((username == NULL) || (ip == NULL)) {
        g_printf("get_username_ip: parameter invalid.\n");
        return RET_ERR;
    }

    
    if ((user_name_temp = g_getenv("LOGNAME")) != NULL) {
        if (strcmp(user_name_temp, RESERVED_ROOT_USER_NAME) == 0) {
            user_name_temp = ACTUAL_ROOT_USER_NAME;
        }
        iRet = snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", user_name_temp);
        if (iRet <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
        }
    } else {
        iRet = snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        if (iRet <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
        }
    }

    
    if ((ip_temp = g_getenv("SSH_CONNECTION")) != NULL) {
        for (i = 0; ip_temp[i] != 0 && i < ip_size; i++) {
            
            
            if (ip_temp[i] == 0x20) {
                // 第一个空格替换为':',第二个空格结束
                if (space == 0) {
                    space = 1;
                    ip[i] = ':';
                } else {
                    break;
                }
            } else {
                ip[i] = ip_temp[i];
            }

            
        }
    } else if ((ip_temp = g_getenv("REMOTEHOST")) != NULL) {
        if (ip_size < 1) {
            debug_log(DLOG_ERROR, "%s failed: param error, ip_size is 0.", __FUNCTION__);
            return RET_ERR;
        }
        iRet = snprintf_s(ip, ip_size, ip_size - 1, "%s", ip_temp);
        if (iRet <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
        }
    } else {
        if (ip_size == 0) {
            debug_log(DLOG_ERROR, "%s failed:input param error, ip_size is 0.", __FUNCTION__);
            return RET_ERR;
        }
        iRet = snprintf_s(ip, ip_size, ip_size - 1, "%s", "127.0.0.1");
        if (iRet <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
        }
    }

    return RET_OK;
}


gint32 upload_file_to_url(const gchar *url, guint8 file_id)
{
    gint32 iRet;
    gchar url_path[URL_MAX_LENGTH + 1] = {0};
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    gint32 ret;
    gint32 status = 0;
    guint16 i;
    const gint32 TASK_DELAY_1S = 1000;
    const gint32 TASK_DELAY_100MS = 100;
    const gint32 TRANSFER_FINISH = 100;
    const gint32 GET_TRANSFER_STATUS_TIMES = 3000;

    
    if (url == NULL) {
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    
    iRet = snprintf_s(url_path, sizeof(url_path), URL_MAX_LENGTH, "upload;%u;%s", file_id, url);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    input_list = g_slist_append(input_list, g_variant_new_string(url_path));
    input_list = g_slist_append(input_list, g_variant_new_string(" "));
    
    ret = ipmc_call_method_user_info(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
        CLASS_NAME_TRANSFERFILE, OBJ_NAME_TRANSFERFILE, METHOD_INITRIAL_FILE_TRANSFER, input_list, NULL);
    clear_sensitive_info(url_path, sizeof(url_path));
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        return FALSE;
    } else {
        (void)vos_task_delay(TASK_DELAY_1S);
    }

    
    g_printf("Uploading...\r\n");

    for (i = 0; i < GET_TRANSFER_STATUS_TIMES; i++) {
        (void)vos_task_delay(TASK_DELAY_100MS);

        ret = uip_call_class_method(NULL, NULL, NULL, CLASS_NAME_TRANSFERFILE, OBJ_NAME_TRANSFERFILE,
            METHOD_GET_TRANSFER_STATUS, NULL, &output_list);
        if (ret != RET_OK) {
            return FALSE;
        }

        status = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
        if (status < 0) {
            if (status == CURL_PEER_SSL_VERIFY_FAILED) {
                g_printf("\r\nVerify peer's certificate or fingerprint failed.\r\n");
            } else {
                g_printf("\r\n");
            }
            uip_free_gvariant_list(output_list);
            return FALSE;
        }

        uip_free_gvariant_list(output_list);

        g_printf("\r%2d%%", status);
        (void)fflush(stdout);

        
        if (status == TRANSFER_FINISH) {
            g_printf("\r\n");
            return TRUE;
        }
    }

    g_printf("\r\n");
    return FALSE;
}

gint32 ipmc_call_method_user_info(const gchar *interface, const gchar *username, const gchar *client,
    const gchar *class_name, const gchar *obj_name, const gchar *method_name, GSList *input_list, GSList **output_list)
{
    gint32 ret;
    gboolean is_local_user = TRUE;
    gchar role_id[IPMC_ROLE_ID_MAX_LEN] = {0};

    ret = ipmc_get_user_roleid(&is_local_user, role_id, IPMC_ROLE_ID_MAX_LEN);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    ret = uip_call_class_method_user_info(interface, username, client, (const gboolean)is_local_user,
        (const gchar *)role_id, class_name, obj_name, method_name, input_list, output_list);
    return ret;
}

gint32 ipmc_get_local_uid_gid(uid_t *uid, gid_t *gid)
{
    gint32 ret;

    ret = ipmc_get_uid(uid);
    if (ret != RET_OK) {
        g_printf("Get user info failed.\r\n");
        return RET_ERR;
    }
    ret = ipmc_get_gid(gid);
    if (ret != RET_OK) {
        g_printf("Get user info failed.\r\n");
        return RET_ERR;
    }

    return RET_OK;
}

gint32 ipmc_change_file_owner(const gchar *file_path)
{
    gint32 ret;
    guint32 uid = 0;
    guint32 gid = 0;

    ret = ipmc_get_local_uid_gid(&uid, &gid);
    if (ret != RET_OK) {
        g_printf("Get user info failed.\r\n");
        return RET_ERR;
    }
    if (uid >= LDAP_USER_ID_BASE) {
        uid = APACHE_UID;
        gid = APPS_USER_GID;
    }
    ret = dal_set_file_owner(file_path, uid, gid);
    if (ret != RET_OK) {
        g_printf("Change file owner failed.\r\n");
        return RET_ERR;
    }
    ret = dal_set_file_mode(file_path, (S_IRUSR | S_IWUSR));
    if (ret != RET_OK) {
        g_printf("Change file mode failed.\r\n");
        return RET_ERR;
    }
    return ret;
}

void ipmc_get_lsw_print(gchar *tmp_buf)
{
    gint32 ret;
    guint32 i = 0;
    gchar *t_buf = NULL;
    gchar *p_next_token = NULL;
    gint32 user_input;

    t_buf = strtok_s(tmp_buf, "\n", &p_next_token);
    while (t_buf != NULL) {
        g_printf("%s\n", t_buf);
        t_buf = strtok_s(NULL, "\n", &p_next_token);
        i++;
        if (i == TABLE_OUTPUT_MAX_ROWS) {
            g_printf("\r\n");
            user_input = ipmc_get_user_input("Input 'q' quit:");
            ret = (user_input == 'q') || (user_input == 'Q');
            if (ret) {
                break;
            }
            i = 0;
        }
    }

    return;
}


gint32 print_blade_bmc_ip_info(guint8 slave_addr)
{
    gint32 ret_val;
    gchar username[PARAMETER_LEN + 1] = { 0 };
    gchar ip[PARAMETER_LEN + 1] = { 0 };
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    const gchar *mac = NULL;
    const gchar *ipaddr = NULL;
    const gchar *submask = NULL;
    const gchar *gateway = NULL;
    const gchar *ip6addr = NULL;
    const gchar *ip6gateway = NULL;
    guint8 ipmode;
    guint8 ip6mode;
    guint8 prefix;
    guint8 net_mode;
    guint8 vlan_state;
    guint16 vlan_id = 0;
    guint8 net_type;
    guint8 silk_num;
    gsize len = 0;
    
    gchar blade_name[OBJ_NAME_MAX_LEN] = { 0 };
    
    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    if (get_ipmbeth_blade_name(slave_addr, blade_name, sizeof(blade_name)) != RET_OK) {
        return FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(slave_addr));

    ret_val = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
        CLASS_NAME_IPMBETH_BLADE, blade_name, METHOD_SHELF_GET_BLADE_LANCONFIG, input_list, &output_list);
    if (ret_val != RET_OK) {
        g_printf("Get blade ip info failed.\r\n");
        uip_free_gvariant_list(input_list);
        uip_free_gvariant_list(output_list);
        return FALSE;
    }

    net_mode = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 0));
    g_printf("Net Mode              :  %s\r\n", (net_mode > 2) ? "Unknown" : net_mode_str[net_mode]);

    
    net_type = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 1));
    silk_num = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 2));
    g_printf("Net Type              :  %s\r\n", (net_type > NET_TYPE_MAX - 1) ? "Unknown" : net_type_str[net_type]);
    if (is_support_ncsi(net_type) == RET_OK) {
        g_printf("Net Port              :  %u\r\n", silk_num);
    }
    

    g_printf("IPv4 Information      :\r\n");
    ipmode = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 3));
    g_printf("IP Mode               :  %s\r\n", (ipmode > IP_MODE_NUM - 1) ? "unsupport" : ip_mode_str[ipmode]);
    ipaddr = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 4), &len);
    if (len) {
        g_printf("IP Address            :  %s\r\n", ipaddr);
    } else {
        g_printf("IP Address            :  \r\n");
    }
    submask = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 5), &len);
    if (len) {
        g_printf("Subnet Mask           :  %s\r\n", submask);
    } else {
        g_printf("Subnet Mask           :  \r\n");
    }
    gateway = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 6), &len);
    if (len) {
        g_printf("Default Gateway       :  %s\r\n", gateway);
    } else {
        g_printf("Default Gateway       :  \r\n");
    }
    mac = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 7), &len);
    if (len) {
        g_printf("MAC Address           :  %s\r\n", mac);
    } else {
        g_printf("MAC Address           :  \r\n");
    }
    g_printf("IPv6 Information      :\r\n");
    ip6mode = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 8));
    g_printf("IPv6 Mode             :  %s\r\n", (ip6mode > IP_MODE_NUM - 1) ? "unsupport" : ip_mode_str[ip6mode]);
    ip6addr = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 9), &len);
    prefix = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 10));
    if (len && prefix) {
        g_printf("IPv6 Address 1        :  %s/%d\r\n", ip6addr, prefix);
    } else {
        g_printf("IPv6 Address 1        :  \r\n");
    }
    ip6gateway = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 11), &len);
    if (len) {
        g_printf("Default Gateway IPv6  :  %s\r\n", ip6gateway);
    } else {
        g_printf("Default Gateway IPv6  :  \r\n");
    }

    
    vlan_state = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 12));
    g_printf("VLAN Information      :\r\n");
    g_printf("VLAN State            :  %s\r\n", vlan_state ? "enabled" : "disabled");

    if (vlan_state == ENABLE_ETH) {
        vlan_id = g_variant_get_uint16((GVariant *)g_slist_nth_data(output_list, 13));
        g_printf("VLAN ID               :  %d\r\n", vlan_id);
    }
    

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);
    return TRUE;
}



gint32 _get_outer_eth_ipaddr_mask(gchar *ipaddr, guint32 iplen, gchar *mask, guint32 masklen)
{
    errno_t safe_fun_ret = EOK;
    gint32 ret;
    GVariant *property_value = NULL;
    GSList *eth_list = NULL;
    GSList *node = NULL;
    OBJ_HANDLE obj_handle;
    guchar type = 0;
    const gchar *tmp = NULL;

    ret = dfl_get_object_list(ETH_CLASS_NAME_ETHGROUP, &eth_list);
    if (ret != DFL_OK) {
        return RET_ERR;
    }

    for (node = eth_list; node; node = g_slist_next(node)) {
        if ((obj_handle = (OBJ_HANDLE)node->data)) {
            
            ret = dfl_get_property_value(obj_handle, ETH_ATTRIBUTE_OUT_TYPE, &property_value);
            if (ret != DFL_OK) {
                debug_log(DLOG_ERROR, "%s: dfl_get_property_value ETH_ATTRIBUTE_OUT_TYPE failed", __FUNCTION__);
                break;
            }

            type = g_variant_get_byte(property_value);
            g_variant_unref(property_value);
            property_value = NULL;

            // 找到外出网口，获取
            if (type == OUT_ETHERNET) {
                // ip
                ret = dfl_get_property_value(obj_handle, ETH_GROUP_ATTRIBUTE_IP_ADDR, &property_value);
                if (ret != DFL_OK) {
                    debug_log(DLOG_ERROR, "%s: dfl_get_property_value ETH_GROUP_ATTRIBUTE_IP_ADDR failed",
                        __FUNCTION__);
                    break;
                }

                tmp = g_variant_get_string(property_value, 0);
                
                safe_fun_ret = strncpy_s(ipaddr, iplen, tmp, iplen - 1);
                if (safe_fun_ret != EOK) {
                    debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
                }
                
                g_variant_unref(property_value);
                property_value = NULL;
                // mask
                ret = dfl_get_property_value(obj_handle, ETH_GROUP_ATTRIBUTE_SUB_MASK, &property_value);
                if (ret != DFL_OK) {
                    debug_log(DLOG_ERROR, "%s: dfl_get_property_value ETH_GROUP_ATTRIBUTE_IP_ADDR failed",
                        __FUNCTION__);
                    break;
                }

                tmp = g_variant_get_string(property_value, 0);
                
                safe_fun_ret = strncpy_s(mask, masklen, tmp, masklen - 1);
                if (safe_fun_ret != EOK) {
                    debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
                }
                
                g_variant_unref(property_value);
                property_value = NULL;

                ret = RET_OK;
                break;
            }
        }
    }

    g_slist_free(eth_list);
    return ret;
}


gint32 parse_sol_login_time(guint32 time_value, gchar *time_buf, gsize buf_len)
{
    struct tm local_time = { 0 };
    gint32 ret;
    time_t tmp_value = time_value;

    if (time_buf == NULL) {
        return RET_ERR;
    }
    if (buf_len == 0) {
        debug_log(DLOG_ERROR, "%s failed:input param error, buf_len is 0.", __FUNCTION__);
        return RET_ERR;
    }

    
    
    ret = dal_localtime_r(&tmp_value, &local_time);
    if (ret != RET_OK) {
        g_printf("Get login time failed.\r\n");
        return RET_ERR;
    }

    if (snprintf_s(time_buf, buf_len, buf_len - 1, "%04d-%02d-%02d %02d:%02d:%02d", (1900 + local_time.tm_year),
        (1 + local_time.tm_mon), local_time.tm_mday, local_time.tm_hour, local_time.tm_min, local_time.tm_sec) < 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s failed", __FUNCTION__);
        return RET_ERR;
    }

    return ret;
}


gboolean is_control_by_other_node(guint8 *out_ctrl_node)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint8 ctrl_node;

    ret = dal_get_object_handle_nth(COOLINGCLASS, 0, &obj_handle);
    if (ret != RET_OK) {
        return FALSE;
    }
    ret = dal_get_extern_value_byte(obj_handle, COOLING_FAN_MANUAL_MODE_CONTROL_NODE, &ctrl_node, DF_AUTO);
    if (ret != RET_OK || ctrl_node == 0) {
        return FALSE;
    }

    if (ctrl_node == dal_get_slotid()) {
        return FALSE;
    }

    *out_ctrl_node = ctrl_node;
    return TRUE;
}

