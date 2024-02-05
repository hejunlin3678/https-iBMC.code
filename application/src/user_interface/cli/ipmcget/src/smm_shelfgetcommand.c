

#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "pme_app/pme_app.h"
#include "pme_app/uip/uip_network_config.h"
#include "uip.h"
#include "ipmctypedef.h"
#include "shelfgetcommand.h"
#include "ipmcgetcommand.h"
#include "ipmc_shelf_common.h"
#include "ipmcgethelp.h"
#include "ipmc_public.h"
#include "mscm_macro.h"
#include "smm_sol_client.h"

LOCAL gint32 foreach_fantray_presence(OBJ_HANDLE object_handle, gpointer user_data);
LOCAL gint32 foreach_blade_presence(OBJ_HANDLE object_handle, gpointer user_data);
LOCAL void print_get_hotswap_helpinfo(gchar *target);
LOCAL void print_get_removed_event_severity_helpinfo(void);
LOCAL void print_port_info(gpointer data, gpointer user_data);
LOCAL gboolean get_diagnose_process(const gchar *get_phase_method_name, const gchar *username, const gchar *ip,
    gint32 *process);
LOCAL gboolean check_smm_diagnose_progress(const gchar *get_phase_method_name, const gchar *username, const gchar *ip,
    gboolean dump_both_smm);


LOCAL gint32 foreach_fantray_presence(OBJ_HANDLE object_handle, gpointer user_data)
{
    guchar presence = 0;
    (void)dal_get_property_value_byte(object_handle, PROPERTY_FAN_PRESENT, &presence);
    if (presence) {
        g_printf("fantray\r\n");
        return DFL_ERR;
    }
    return DFL_OK;
}


LOCAL gint32 foreach_blade_presence(OBJ_HANDLE object_handle, gpointer user_data)
{
    const gchar *obj_name = NULL;
    guchar presence;

    obj_name = dfl_get_object_name(object_handle);
    if (!(strcmp(obj_name, "SMM1") && strcmp(obj_name, "SMM2"))) {
        return DFL_OK;
    }
    presence = get_blade_presence(object_handle);
    if (presence) {
        g_printf("%s\r\n", obj_name);
    }
    return DFL_OK;
}

LOCAL void print_get_hotswap_helpinfo(gchar *target)
{
    g_printf("Usage:\r\n");
    if (strncmp("blade", target, strlen("blade")) == 0) {
        g_printf("ipmcget -l bladeN -d hotswapstate\r\n");
    } else if (strncmp("swi", target, strlen("swi")) == 0) {
        g_printf("ipmcget -l swiN -d hotswapstate -v <fruid>\r\n");
        g_printf("fru id is 0~3.\r\n");
    }
}

LOCAL void print_get_removed_event_severity_helpinfo(void)
{
    OBJ_HANDLE handle = 0;
    gint32 ret;

    ret = dal_get_specific_object_byte(CLASS_NAME_IPMBETH_BLADE, PROTERY_IPMBETH_BLADE_BLADETYPE, BLADE_TYPE_SWI_MODULE,
        &handle);
    if (ret != DFL_OK) {
        g_printf("Usage: ipmcget -l <smm|blade|pem|fantray> -d removedeventseverity\r\n");
        return;
    }
    g_printf("Usage: ipmcget -l <smm|blade|swi|pem|fantray> -d removedeventseverity\r\n");
}

LOCAL void print_port_info(gpointer data, gpointer user_data)
{
    gsize len = 0;
    const gchar *str = NULL;
    str = g_variant_get_string((GVariant *)data, &len);
    g_printf("%s", str);
}


gint32 ipmc_get_listpresent(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location)
{
    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }
    g_printf("List Present Information:\r\n");
    g_printf("shelf\r\n");
    g_printf("smm\r\n");
    g_printf("pem\r\n");
    (void)dfl_foreach_object(CLASS_NAME_SMM_FAN, foreach_fantray_presence, NULL, NULL);

    if (dfl_foreach_object(CLASS_NAME_IPMBETH_BLADE, foreach_blade_presence, NULL, NULL) != DFL_OK) {
        g_printf("Get blade or swi present state failed.\r\n");
        return FALSE;
    }

    return TRUE;
}


gint32 ipmc_get_blade_presence(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location)
{
    OBJ_HANDLE object_handle;
    gint32 ret;
    gint32 presence;
    guchar board_num;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }
    
    if (!strcmp(location, SHELF_MGMT_DEFAULT_NAME)) {
        g_printf("SMM is present.\r\n");
        return TRUE;
    }

    board_num = get_blade_num(location);
    if (board_num == 0xff) {
        g_printf("ipmcget -l <bladeN|swiN> -d presence\r\n");
        return FALSE;
    }
    
    
    ret = dfl_get_object_handle(location, &object_handle);
    if (ret != DFL_OK) {
        g_printf("get %s presence state failed.\r\n", location);
    }
    
    location[0] = location[0] - ('a' - 'A');
    presence = get_blade_presence(object_handle);
    if (presence == SHELF_PART_PRESENCE) {
        g_printf("%s is present.\r\n", location);
    } else {
        g_printf("%s is not present.\r\n", location);
    }
    return TRUE;
}


gint32 ipmc_get_hotswapstate(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location)
{
    OBJ_HANDLE obj_handle = 0;
    GVariant *property_data = NULL;
    gint32 ret_val;
    gchar *object_name = NULL;
    gint32 board_num;
    guchar board_ipmbaddr;
    guchar m_state;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    guint8 fru_id = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    
    if (!strcmp(location, SHELF_MGMT_DEFAULT_NAME)) {
        if (argc != 0) {
            g_printf("Too many parameters.\r\n");
            return FALSE;
        }
        object_name = OBJECT_HOTSWAP;
        location = "SMM";

        ret_val = dfl_get_object_handle(object_name, &obj_handle);
        if (ret_val != DFL_OK) {
            g_printf("Get %s hotswap state failed.\r\n", object_name);
            return FALSE;
        }

        ret_val = dfl_get_property_value(obj_handle, PROPERTY_HS_CRU_STATE, &property_data);
        if (ret_val != RET_OK) {
            g_printf("Get %s hotswap state failed.\r\n", object_name);
            return FALSE;
        }

        m_state = g_variant_get_byte(property_data);
        g_variant_unref(property_data);
        g_printf("%s hot swap state is M%d\r\n", location, m_state);

        return TRUE;
    } else if (strncmp("swi", (const gchar *)location, strlen("swi")) == 0) {
        if ((argc != 2) || strncmp("-v", argv[0], strlen(argv[0]))) {
            print_get_hotswap_helpinfo(location);
            return FALSE;
        }
        if ((strlen(argv[1]) == 1) && (argv[1][0] >= '0') && (argv[1][0] <= '3')) {
            fru_id = (guint8)(argv[1][0] - '0');
        } else {
            print_get_hotswap_helpinfo(location);
            return FALSE;
        }
    } else {
        if (argc != 0) {
            g_printf("Too many parameters.\r\n");
            return FALSE;
        }
    }

    board_num = get_blade_num(location);
    if (board_num == 0xff) {
        print_get_hotswap_helpinfo(location);
        return FALSE;
    }
    board_ipmbaddr = get_blade_ipmbaddr(board_num);
    input_list = g_slist_append(input_list, g_variant_new_byte(board_ipmbaddr));
    input_list = g_slist_append(input_list, g_variant_new_byte(fru_id));
    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }
    ret_val = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
        CLASS_NAME_IPMBETH_BLADE, location, METHOD_IPMBETH_BLADE_GETFRUHOTSWAPSTATE, input_list, &output_list);
    if (ret_val != RET_OK) {
        g_printf("Get blade fru hotswapstate failed.\r\n");
        uip_free_gvariant_list(input_list);
        return FALSE;
    }
    
    location[0] = location[0] - ('a' - 'A');
    m_state = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 0));
    if (strncmp("Swi", (const gchar *)location, strlen("Swi")) == 0) {
        g_printf("%s:fru%d hot swap state is M%d.\r\n", location, fru_id, m_state);
    } else {
        g_printf("%s hot swap state is M%d.\r\n", location, m_state);
    }
    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);
    return TRUE;
}


gint32 ipmc_get_smalert_config(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    OBJ_HANDLE object_handle = 0;
    guint8 alert_enable = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcget [-l smm] -d smalertconfig\r\n");
        return FALSE;
    }

    ret = dal_get_object_handle_nth(CLASS_POWER_ALERT, 0, &object_handle);
    if (ret != RET_OK) {
        g_printf("Get smalert enable failed.\r\n");
        return FALSE;
    }

    ret = dal_get_property_value_byte(object_handle, PROPERTY_POWER_ALERT_ENABLE, &alert_enable);
    if (ret != RET_OK) {
        g_printf("Get smalert enable failed.\r\n");
        return FALSE;
    }

    if (alert_enable == 0) { 
        g_printf("Smalert is disabled.\r\n");
    } else { 
        g_printf("Smalert is enabled.\r\n");
    }

    return TRUE;
}


gint32 ipmc_get_smalert_status(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    OBJ_HANDLE object_handle = 0;
    guint8 alert_status = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcget [-l smm] -d smalertstatus\r\n");
        return FALSE;
    }

    ret = dal_get_object_handle_nth(CLASS_POWER_ALERT, 0, &object_handle);
    if (ret != RET_OK) {
        g_printf("Get smalert status failed.\r\n");
        return FALSE;
    }

    ret = dal_get_property_value_byte(object_handle, PROPERTY_POWER_ALERT_STATE, &alert_status);
    if (ret != RET_OK) {
        g_printf("Get smalert status failed.\r\n");
        return FALSE;
    }

    if (alert_status == 0) { 
        g_printf("Smalert status is ok.\r\n");
    } else { 
        g_printf("Smalert status is not ok.\r\n");
    }

    return TRUE;
}


gint32 smm_sol_get_timeout(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    OBJ_HANDLE handle;
    gint32 timeout;
    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Usage: ipmcget %s-d soltimeout\r\n", help_default_name);
        return FALSE;
    }
    ret = dal_get_object_handle_nth(CLASS_NAME_SMM_SOL, 0, &handle);
    if (ret != RET_OK) {
        g_printf("Get timeout failed.\r\n");
        return FALSE;
    }
    ret = dal_get_property_value_int32(handle, PROPERTY_SMM_SOL_TIMEOUT, &timeout);
    if (ret != RET_OK) {
        g_printf("Get timeout failed.\r\n");
        return FALSE;
    }
    g_printf("Timeout : %d second.\r\n", timeout);
    return TRUE;
}


gint32 ipmc_get_deviceid(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location)
{
    gchar ret;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcget -l <bladeN|swiN> -d deviceid\r\n");
        return IPMC_ERROR;
    }

    ret = ipmi_get_deviceid(location);
    if (ret != COMP_CODE_SUCCESS) {
        g_printf("ipmi_get_deviceid failed.\r\n");
        return FALSE;
    }

    return TRUE;
}

gint32 smm_get_health_status(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    guint32 minor = 0;
    guint32 major = 0;
    guint32 critical = 0;
    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Usage: ipmcget %s-d health\r\n", help_default_name);
        return FALSE;
    }
    ret = smm_get_health_events_count(&minor, &major, &critical);
    if (ret != RET_OK) {
        g_printf("Get smm health status failed.\r\n");
        return FALSE;
    }
    if (minor == 0 && major == 0 && critical == 0) {
        g_printf("Smm in health state.\r\n");
        return TRUE;
    }
    g_printf("SMM Events:\r\n");
    if (minor != 0) {
        g_printf("%u minor events.\r\n", minor);
    }
    if (major != 0) {
        g_printf("%u major events.\r\n", major);
    }
    if (critical != 0) {
        g_printf("%u critical events.\r\n", critical);
    }
    return TRUE;
}

gint32 smm_get_health_event(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    guint32 data[6] = { 0 };
    guint32 minor = 0;
    guint32 major = 0;
    guint32 critical = 0;
    guint8 as_status = 0;
    guint32 i;
    guint32 *filter = NULL;
    guint32 filter_size;
    guint32 active_filter[] = { 0 };
    
    guint32 standby_filter[] = { 0x12000000, 0x10000000, 0x2a000000, 0x1a000000, 0x17000000 };

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Usage: ipmcget %s-d healthevents\r\n", help_default_name);
        return FALSE;
    }
    
    ret = smm_get_health_events_count(&minor, &major, &critical);
    if (ret != RET_OK) {
        g_printf("Get smm health status failed.\r\n");
        return FALSE;
    }
    if (minor == 0 && major == 0 && critical == 0) {
        g_printf("Smm in health state.\r\n");
        return TRUE;
    }
    
    data[0] = 0;
    data[2] = 0xff000000;
    data[3] = 0;
    g_printf("%-10s | %-20s | %-12s | %-12s | %s\r\n", "Event Num", "Event Time", "Alarm Level", "Event Code",
        "Event Description");
    (void)get_board_active_state(&as_status);
    if (as_status == ACTIVE_STATE) { // 主用板
        filter = active_filter;
        filter_size = sizeof(active_filter) / sizeof(guint32);
    } else { // 备用板
        filter = standby_filter;
        filter_size = sizeof(standby_filter) / sizeof(guint32);
    }
    
    for (i = 0; i < filter_size; ++i) {
        data[1] = filter[i];
        ret = foreach_smm_health_event(print_single_smm_health_event, (gpointer)data);
        if (ret != RET_OK) {
            g_printf("Get smm health event failed.\r\n");
            return FALSE;
        }
    }
    return TRUE;
}

gint32 smm_get_blade_health_event(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    errno_t safe_fun_ret = EOK;
    gint32 ret;
    guint8 ipmb_addr = 0;
    gint32 blade_num = 0;
    guint32 data[6] = { 0 };
    guint32 presence = 0;
    OBJ_HANDLE handle = 0;
    gchar blade_name[MAX_NAME_SIZE] = { 0 };
    gchar blade_name_lower[MAX_NAME_SIZE] = { 0 };

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        if (strcmp("smm", target) == 0) {
            g_printf("Usage: ipmcget %s-d othersmmhealthevents\r\n", help_default_name);
        } else {
            g_printf("Usage: ipmcget -l <bladeN|swiN> -d healthevents\r\n");
        }
        return FALSE;
    }
    // 获取ipmb地址
    if (strcmp("smm", target) == 0) {
        (void)get_other_board_present(&presence);
        if (presence != BLADE_PRESENCE) {
            g_printf("Other smm is not present.\r\n");
            return TRUE;
        }
        ret = get_other_smm_slave_addr(&ipmb_addr);
        if (ret != RET_OK) {
            g_printf("Get other smm health event failed.\r\n");
            return FALSE;
        }
        (void)strncpy_s(blade_name, sizeof(blade_name), "Other smm", strlen("Other smm"));
        (void)strncpy_s(blade_name_lower, sizeof(blade_name_lower), "other smm", strlen("other smm"));
    } else {
        blade_num = get_blade_num(target);
        if (blade_num == 0xff) {
            g_printf("Usage: ipmcget -l <bladeN|swiN> -d healthevents\r\n");
            return FALSE;
        }
        safe_fun_ret = strncpy_s(blade_name, sizeof(blade_name), target, sizeof(blade_name) - 1);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        blade_name[0] = g_ascii_toupper(blade_name[0]);
        safe_fun_ret = strncpy_s(blade_name_lower, sizeof(blade_name_lower), target, sizeof(blade_name_lower) - 1);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        ret = dfl_get_object_handle(target, &handle);
        if (ret != DFL_OK) {
            g_printf("Get %s info failed.\r\n", target);
            return FALSE;
        }
        presence = get_blade_presence(handle);
        if (presence != BLADE_PRESENCE) {
            g_printf("%s is not present.\r\n", blade_name);
            return TRUE;
        }
        ipmb_addr = get_blade_ipmbaddr(blade_num);
    }
    // 是否存在告警
    ret = foreach_blade_health_event(count_single_blade_health_event, ipmb_addr, (gpointer)data);
    if (ret != RET_OK) {
        g_printf("Get %s health event failed.\r\n", blade_name_lower);
        return FALSE;
    }
    if (data[0] == 0 && data[1] == 0 && data[2] == 0) {
        g_printf("%s in health state.\r\n", blade_name);
        return TRUE;
    }
    // 打印
    data[0] = 0;
    g_printf("%-10s | %-20s | %-12s | %-12s | %s\r\n", "Event Num", "Event Time", "Alarm Level", "Event Code",
        "Event Description");
    ret = foreach_blade_health_event(print_single_blade_health_event, ipmb_addr, (gpointer)data);
    if (ret != RET_OK) {
        g_printf("Get %s health event failed.\r\n", blade_name_lower);
        return FALSE;
    }
    return TRUE;
}

gint32 smm_get_blade_health_status(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    errno_t safe_fun_ret = EOK;
    gint32 ret;
    guint8 ipmb_addr = 0;
    gint32 blade_num = 0;
    guint32 data[3] = { 0 };
    guint32 presence = 0;
    OBJ_HANDLE handle = 0;
    gchar blade_name[MAX_NAME_SIZE] = { 0 };
    gchar blade_name_lower[MAX_NAME_SIZE] = { 0 };

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        if (strcmp("smm", target) == 0) {
            g_printf("Usage: ipmcget %s-d othersmmhealth\r\n", help_default_name);
        } else {
            g_printf("Usage: ipmcget -l <bladeN|swiN> -d health\r\n");
        }
        return FALSE;
    }
    // 获取ipmb地址
    if (strcmp("smm", target) == 0) {
        (void)get_other_board_present(&presence);
        if (presence != BLADE_PRESENCE) {
            g_printf("Other smm is not present.\r\n");
            return TRUE;
        }
        ret = get_other_smm_slave_addr(&ipmb_addr);
        if (ret != RET_OK) {
            g_printf("Get other smm health event failed.\r\n");
            return FALSE;
        }
        (void)strncpy_s(blade_name, sizeof(blade_name), "Other smm", strlen("Other smm"));
        (void)strncpy_s(blade_name_lower, sizeof(blade_name_lower), "other smm", strlen("other smm"));
    } else { 
        blade_num = get_blade_num(target);
        if (blade_num == 0xff) {
            g_printf("Usage: ipmcget -l <bladeN|swiN> -d health\r\n");
            return FALSE;
        }
        safe_fun_ret = strncpy_s(blade_name, sizeof(blade_name), target, sizeof(blade_name) - 1);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        blade_name[0] = g_ascii_toupper(blade_name[0]);
        safe_fun_ret = strncpy_s(blade_name_lower, sizeof(blade_name_lower), target, sizeof(blade_name_lower) - 1);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        ret = dfl_get_object_handle(target, &handle);
        if (ret != RET_OK) {
            g_printf("Get %s info failed.\r\n", target);
            return FALSE;
        }
        presence = get_blade_presence(handle);
        if (presence != BLADE_PRESENCE) {
            g_printf("%s is not present.\r\n", blade_name);
            return TRUE;
        }
        ipmb_addr = get_blade_ipmbaddr(blade_num);
    }
    // 统计告警数目
    ret = foreach_blade_health_event(count_single_blade_health_event, ipmb_addr, (gpointer)data);
    if (ret != RET_OK) {
        g_printf("Get %s health state failed.\r\n", blade_name_lower);
        return FALSE;
    }
    if (data[0] == 0 && data[1] == 0 && data[2] == 0) {
        g_printf("%s in health state.\r\n", blade_name);
        return TRUE;
    }
    g_printf("%s Events:\r\n", blade_name);
    if (data[0] != 0) { // 一般告警
        g_printf("%u minor events.\r\n", data[0]);
    }
    if (data[1] != 0) { // 严重告警
        g_printf("%u major events.\r\n", data[1]);
    }
    if (data[2] != 0) { // 致命告警
        g_printf("%u critical events.\r\n", data[2]);
    }
    return TRUE;
}


gint32 ipmc_get_smm_diagnose_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = { 0 };
    gchar ip[PARAMETER_LEN + 1] = { 0 };
    const gchar *dump_start_method_name = NULL;
    const gchar *dump_progress_method_name = NULL;
    guint8 as_status = 0;
    gboolean dump_both_smm = FALSE;
    uid_t uid = 0;
    guint8 is_domain_user;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    ret = get_board_active_state(&as_status);
    if (ret != RET_OK) {
        g_printf("Get smm active standby status failed.\r\n");
        return FALSE;
    }
    ret = ipmc_get_grandp_uid(&uid);
    if (ret != RET_OK) {
        g_printf("Get user info failed.\r\n");
        return FALSE;
    }
    is_domain_user = (uid >= LDAP_USER_ID_BASE) ? TRUE : FALSE;
    input_list = g_slist_append(input_list, g_variant_new_byte(is_domain_user));
    
    if (as_status == ASM_ACTIVE) {
        dump_start_method_name = METHOD_DUMP_DUMPINFO_BOTH_SMM_ASYNC;
        dump_progress_method_name = METHOD_DUMP_GET_BOTH_SMM_DUMP_PHASE;
        dump_both_smm = TRUE;
        input_list = g_slist_append(input_list, g_variant_new_string("HMM"));
    } else {
        dump_start_method_name = METHOD_DUMP_DUMPINFO_ASYNC;
        dump_progress_method_name = METHOD_DUMP_GET_DUMP_PHASE;
    }
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_DUMP, OBJECT_DUMP,
        dump_start_method_name, input_list, NULL);
    uip_free_gvariant_list(input_list);

    if (ret != RET_OK) {
        if (ret == MD_ENOSPACE) {
            g_printf("Failed to collect data due to insufficient memory.\r\n");
        } else if (ret == MD_EONUPGRADE) {
            g_printf("An upgrade is in progress. Please wait.\r\n");
        } else {
            g_printf("Get diagnose info failed.\r\n");
        }

        return FALSE;
    }
    return check_smm_diagnose_progress(dump_progress_method_name, username, ip, dump_both_smm);
}

LOCAL gboolean get_diagnose_process(const gchar *get_phase_method_name, const gchar *username, const gchar *ip,
    gint32 *process)
{
    gint32 ret;
    GSList *output_list = NULL;
    ret = uip_call_class_method(OPERATOR_LOG_CLI, username, ip, CLASS_DUMP, OBJECT_DUMP, get_phase_method_name, NULL,
        &output_list);
    if (ret != RET_OK) {
        g_printf("Get diagnose process info failed.\r\n");
        return FALSE;
    }

    *process = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
    uip_free_gvariant_list(output_list);
    output_list = NULL;
    return TRUE;
}

LOCAL gboolean check_smm_diagnose_progress(const gchar *get_phase_method_name, const gchar *username, const gchar *ip,
    gboolean dump_both_smm)
{
    gint32 ret;
    gint32 process_bar;

    g_printf("Downloading...\r\n");
    for (;;) {
        ret = get_diagnose_process(get_phase_method_name, username, ip, &process_bar);
        if (ret == FALSE) {
            return FALSE;
        }
        if (process_bar < 0 && dump_both_smm) {
            
            switch (process_bar) {
                case SMM_STDBY_DUMP_FAILED: // 备板收集失败，主板收集成功，提示备板收集失败
                    g_printf("\r100%%\r\n");
                    g_printf("Collect standby smm diagnose info failed.\r\n");
                    (void)fflush(stdout);
                    goto OK_OUT;
                    break;
                case SMM_ACTIVE_DUMP_FAILED: // 主板收集失败，备板收集成功，提示主板收集失败
                    g_printf("\r100%%\r\n");
                    g_printf("Collect active smm diagnose info failed.\r\n");
                    (void)fflush(stdout);
                    goto OK_OUT;
                    break;
                default:
                    g_printf("\rDownload diagnose info failed.\r\n");
                    (void)fflush(stdout);
                    return FALSE;
                    break;
            }
        } else if (process_bar < 0 && !dump_both_smm) {
            
            g_printf("\rDownload diagnose info failed.\r\n");
            (void)fflush(stdout);
            return FALSE;
        } else if (process_bar < 100) {
            g_printf("\r%2d%%", process_bar);
            (void)fflush(stdout);
        } else {
            g_printf("\r100%%\r\n");
            (void)fflush(stdout);
            break;
        }

        (void)vos_task_delay(DELAY_TIME);
    }
OK_OUT:
    g_printf("Download diagnose info to /tmp/ successfully.\r\n");
    return TRUE;
}


gint32 smm_get_poweroninterval(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location)
{
    OBJ_HANDLE obj_handle = 0;
    GVariant *property_data = NULL;
    gint32 ret_val;
    guint32 value;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    ret_val = dfl_get_object_handle(OBJ_NAME_SHELF_MANAGE, &obj_handle);
    if (ret_val != DFL_OK) {
        debug_log(DLOG_ERROR, "get object handle failed: ret %d.", ret_val);
        return FALSE;
    }

    ret_val = dfl_get_property_value(obj_handle, PROTERY_SHELF_STAGGR_POWERDELAYTIME, &property_data);
    if (ret_val != DFL_OK) {
        debug_log(DLOG_ERROR, "get property value failed: ret %d.", ret_val);
        return FALSE;
    }

    value = g_variant_get_int32(property_data);
    g_variant_unref(property_data);
    g_printf("Blade power on interval time is %d ms\r\n", value);

    return TRUE;
}


gint32 smm_get_bladepowercontrol(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location)
{
    OBJ_HANDLE obj_handle = 0;
    GVariant *property_data = NULL;
    gint32 ret_val;
    guint8 value;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    ret_val = dfl_get_object_handle(OBJ_NAME_SHELF_MANAGE, &obj_handle);
    if (ret_val != DFL_OK) {
        debug_log(DLOG_ERROR, "get object handle failed: ret %d.", ret_val);
        return FALSE;
    }

    ret_val = dfl_get_property_value(obj_handle, PROTERY_SHELF_RATED_POWER_CTRL_EN, &property_data);
    if (ret_val != DFL_OK) {
        debug_log(DLOG_ERROR, "get property value failed: ret %d.", ret_val);
        return FALSE;
    }

    value = g_variant_get_byte(property_data);
    g_variant_unref(property_data);
    g_printf("Blade power control switch is %s.\r\n", (value ? "enable" : "disable"));

    return TRUE;
}


gint32 smm_sol_get_connection_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    const gchar *info = NULL;
    gsize info_length = 0;
    GSList *output_list = NULL;
    GSList *cur = NULL;
    OBJ_HANDLE handle = 0;
    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Usage: ipmcget %s-d solconnectioninfo\r\n", help_default_name);
        return FALSE;
    }
    ret = dal_get_object_handle_nth(CLASS_NAME_SMM_SOL, 0, &handle);
    if (ret != RET_OK) {
        g_printf("Get SOL connection info failed.\r\n");
        return FALSE;
    }
    ret = dfl_call_class_method(handle, METHOD_SMM_SOL_GET_CONNECTION_INFO, NULL, NULL, &output_list);
    if (ret != DFL_OK || output_list == NULL) {
        g_printf("Call SOL connection info failed.\r\n");
        return FALSE;
    }
    cur = output_list;
    while (cur != NULL) {
        info = g_variant_get_string((GVariant *)cur->data, &info_length);
        g_printf("%s", info);
        cur = cur->next;
    }
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    return TRUE;
}

gint32 smm_get_other_smm_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    guint8 slave_addr = 0;
    gint32 ret_val;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if ((argc != 0)) {
        g_printf("Usage: ipmcget %s-d othersmmfruinfo\r\n", help_default_name);
        return FALSE;
    }

    ret_val = get_other_smm_slave_addr(&slave_addr);
    if (ret_val != RET_OK) {
        g_printf("Get other smm object failed.\r\n");
        return FALSE;
    }

    return print_blade_info(slave_addr);
}

gint32 vsmm_get_ms_status(guchar priv, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *node = NULL;
    OBJ_HANDLE obj_handle;
    guint8 slot_id = 0;
    guint8 master_slot_id;
    guint8 cur_slot_id;
    guint8 present = 0;

    if (ipmc_privilege_judge(priv, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Usage: ipmcget %s-d redundancy \r\n", help_default_name);
        return FALSE;
    }
    ret = dfl_get_object_list(MSM_NODE_CLASS_NAME, &obj_list);
    if (ret != DFL_OK) {
        g_printf("%s: dfl_get_object_list %s failed, ret=%d\r\n", __FUNCTION__, MSM_NODE_CLASS_NAME, ret);
        return FALSE;
    }
    master_slot_id = dal_get_master_vsmm_slot_id();
    cur_slot_id = dal_get_slotid();
    for (node = obj_list; node; node = node->next) {
        obj_handle = (OBJ_HANDLE)node->data;
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_MSM_SLOT_ID, &slot_id);
        if (slot_id == 0) {
            continue;
        }

        (void)dal_get_property_value_byte(obj_handle, PROPERTY_MSM_PRESENT_STATUS, &present);

        if (present == BOARD_PRESENT) {
            g_printf("Blade%d: Present(%s)%s\r\n", slot_id, (slot_id == master_slot_id) ? "Primary" : "Secondary",
                (slot_id == cur_slot_id) ? "*" : "");
        } else {
            g_printf("Blade%d: Not Present\r\n", slot_id);
        }
    }
    g_slist_free(obj_list);
    g_printf("* = The Blade you are currently logged into.\r\n");

    return TRUE;
}

gint32 smm_get_data_sync_status(guchar priv, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    OBJ_HANDLE handle = 0;
    guint8 sync_status = 0xff;
    guint8 as_status = 0xff;
    guint8 remote_present = 0xff;

    if (ipmc_privilege_judge(priv, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Usage: ipmcget %s-d datasyncstatus \r\n", help_default_name);
        return FALSE;
    }

    ret = dal_get_object_handle_nth(ASM_CLASS_NAME, 0, &handle);
    if (ret != DFL_OK) {
        g_printf("Get ASManagement object failed.\r\n");
        return FALSE;
    }
    ret = dal_get_property_value_byte(handle, PROPERTY_ASM_AS_STATUS, &as_status);
    if (ret != RET_OK) {
        g_printf("Get redundancy info failed.\r\n");
        return FALSE;
    }
    ret = dal_get_property_value_byte(handle, PROPERTY_REMOTE_PRESENT_STATUS, &remote_present);
    if (ret != RET_OK) {
        g_printf("Get remote board info failed.\r\n");
        return FALSE;
    }
    if (remote_present != BOARD_PRESENT) {
        g_printf("Only one SMM installed.\r\n");
        return FALSE;
    }
    if (as_status == STANDBY_STATE) {
        g_printf("Local SMM running in standby state. Retry on active SMM.\r\n");
        return FALSE;
    }
    ret = dal_get_property_value_byte(handle, PROPERTY_DATA_SYNC_STATUS, &sync_status);
    if (ret != DFL_OK || sync_status > 1) { // 0同步中 1 同步完成
        g_printf("Failed. Get data sync status failed.\r\n");
        return FALSE;
    }

    if (sync_status == 0) { // 0同步中
        g_printf("Data sync in progress.\r\n");
    } else {
        g_printf("Data sync completed.\r\n");
    }
    return TRUE;
}

gint32 smm_get_as_status(guchar priv, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    OBJ_HANDLE handle = 0;
    guint8 as_status = 0xff;
    guint8 smm_index = 0;
    guint8 local_presence = 0;
    guint8 remote_presence = 0;

    if (ipmc_privilege_judge(priv, rolepriv) == FALSE) {
        return FALSE;
    }
    if ((argc != 0)) {
        g_printf("Usage: ipmcget %s-d redundancy \r\n", help_default_name);
        return FALSE;
    }

    ret = dfl_get_object_handle(ASM_OBJECT_NAME, &handle);
    if (ret != DFL_OK) {
        g_printf("Get ASManagement object failed.\r\n");
        return FALSE;
    }

    ret = dal_get_property_value_byte(handle, PROPERTY_ASM_AS_STATUS, &as_status);
    if (ret != RET_OK) {
        g_printf("Get redundancy info failed.\r\n");
        return FALSE;
    }
    if (as_status >= 2) {
        g_printf("Redundancy illeagel.\r\n");
        return FALSE;
    }

    ret = dal_get_property_value_byte(handle, PROPERTY_LOCAL_PRESENT_STATUS, &local_presence);
    if (ret != RET_OK) {
        g_printf("Get local present info failed.\r\n");
        return FALSE;
    }

    ret = dal_get_property_value_byte(handle, PROPERTY_REMOTE_PRESENT_STATUS, &remote_presence);
    if (ret != RET_OK) {
        g_printf("Get remote present info failed.\r\n");
        return FALSE;
    }

    ret = dfl_get_object_handle(CLASS_SMM, &handle);
    if (ret != DFL_OK) {
        g_printf("Get ASManagement connection info failed.\r\n");
        return FALSE;
    }

    ret = dal_get_property_value_byte(handle, PROPERTY_SMM_INDEX, &smm_index);
    if (ret != RET_OK) {
        g_printf("Get ASManagement connection info failed.\r\n");
        return FALSE;
    }

    g_printf("The Redundancy States of SMMs:\r\n");

    if (smm_index == 1) {
        g_printf("SMM1: %sPresent(%s)*\r\n", (local_presence == BOARD_PRESENT) ? "" : "Not ",
            (as_status == ACTIVE_STATE) ? "active" : "standby");
        g_printf("SMM2: %sPresent(%s)\r\n", (remote_presence == BOARD_PRESENT) ? "" : "Not ",
            (as_status == STANDBY_STATE) ? "active" : "standby");
    } else {
        g_printf("SMM1: %sPresent(%s)\r\n", (remote_presence == BOARD_PRESENT) ? "" : "Not ",
            (as_status == STANDBY_STATE) ? "active" : "standby");
        g_printf("SMM2: %sPresent(%s)*\r\n", (local_presence == BOARD_PRESENT) ? "" : "Not ",
            (as_status == ACTIVE_STATE) ? "active" : "standby");
    }
    g_printf("* = The SMM you are currently logged into.\r\n");

    return TRUE;
}

gint32 smm_get_slot_number(guchar priv, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    OBJ_HANDLE handle = 0;
    gint32 ret;
    guint8 slot_id = 0;

    if (ipmc_privilege_judge(priv, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }
    ret = dfl_get_object_handle(OBJ_NAME_BOARD, &handle);
    if (ret != DFL_OK) {
        g_printf("Get SMM slot number failed.\r\n");
        return FALSE;
    }
    ret = dal_get_property_value_byte(handle, PROPERTY_SLOT_ID, &slot_id);
    if (ret != RET_OK) {
        g_printf("Get SMM slot number failed.\r\n");
        return FALSE;
    }
    g_printf("The slot number of SMM is %u.\r\n", slot_id);
    return TRUE;
}


gint32 smm_get_shelf_management_state(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    guint8 status = 0;
    OBJ_HANDLE shelf_manage = 0;
    gchar username[PARAMETER_LEN + 1] = { 0 };
    gchar ip[PARAMETER_LEN + 1] = { 0 };

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if ((argc != 0)) {
        g_printf("Usage: ipmcget %s-d shelfmanagementstate\r\n", help_default_name);
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    ret = dal_get_object_handle_nth(CLASS_NAME_SHELF_MANAGE, 0, &shelf_manage);
    if (ret != RET_OK) {
        g_printf("Get ShelfManage object failed.ret:%d\n", ret);
        return FALSE;
    }

    (void)dal_get_property_value_byte(shelf_manage, PROTERY_SHELF_MANAGEMENT_STATE, &status);

    g_printf("Shelf Management state %s\r\n", (status == 0) ? "OFF" : "ON");

    return TRUE;
}

gint32 smm_get_smm_cooling_medium(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location)
{
    gchar *board_name = "smm";
    OBJ_HANDLE object_handle;
    gint32 ret;
    guint8 medium = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    ret = dfl_get_object_handle(COOLINGCLASS, &object_handle);
    if (ret != DFL_OK) {
        g_printf("get %s presence state failed.\r\n", board_name);
        return FALSE;
    }

    
    ret = dal_get_property_value_byte(object_handle, COOLING_PROPERTY_MEDIUM, &medium);
    if (ret != RET_OK) {
        g_printf("Get cooling mode failed.\r\n");
        return FALSE;
    }

    if (medium == 0) {
        g_printf("%s is air-cooled.\r\n", board_name);
    } else if (medium == 1) {
        g_printf("%s is liquid-cooled.\r\n", board_name);
    } else {
        g_printf("%s cooling medium(%d) error.\r\n", board_name, medium);
    }

    return TRUE;
}

gint32 smm_get_mm_port_optical_module_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *location)
{
    gint32 ret_val;
    gchar username[PARAMETER_LEN + 1] = { 0 };
    gchar ip[PARAMETER_LEN + 1] = { 0 };
    GSList *property_name_list = NULL;
    GSList *output_list = NULL;
    const gchar *vendor_name = NULL;
    const gchar *vendor_PN = NULL;
    const gchar *vendor_SN = NULL;
    const gchar *date = NULL;
    OBJ_HANDLE otm_port_adapter_handle = 0;
    guint8 port_disable = 0;
    guint8 present = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if ((argc != 0)) {
        g_printf("Usage: ipmcget -d managementportotminfo\r\n");
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    ret_val = dal_get_object_handle_nth(CLASS_NAME_MM_PORT_OTM_INFO, 0, &otm_port_adapter_handle);
    if (ret_val != RET_OK) {
        g_printf("Get optical module object failed.\r\n");
        return FALSE;
    }

    ret_val = dal_get_property_value_byte(otm_port_adapter_handle, PROPERTY_PORT_ADAPTER_PRESENT, &present);
    if (port_disable == OPTICAL_TX_DISABLE) {
        g_printf("Optical module is not present.\r\n");
        return TRUE;
    }

    ret_val = dal_get_property_value_byte(otm_port_adapter_handle, PROPERTY_MM_OTM_PORT_DISABLE, &port_disable);

    property_name_list = g_slist_append(property_name_list, PROPERTY_OPT_MDL_VENDOR_NAME);
    property_name_list = g_slist_append(property_name_list, PROPERTY_OPT_MDL_PART_NUMBER);
    property_name_list = g_slist_append(property_name_list, PROPERTY_OPT_MDL_SERIAL_NUMBER);
    property_name_list = g_slist_append(property_name_list, PROPERTY_OPT_MDL_MANUFACTURE_DATE);

    ret_val =
        uip_multiget_object_property(CLASS_OPTICAL_MODULE, OBJECT_NAME_MM_OPT_MODULE, property_name_list, &output_list);
    g_slist_free(property_name_list);
    if (ret_val != RET_OK) {
        uip_free_gvariant_list(output_list);
        return FALSE;
    }

    vendor_name = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    vendor_PN = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 1), NULL);
    vendor_SN = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 2), NULL);
    date = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 3), NULL);

    g_printf("Management port optical module info               : \r\n");
    g_printf("SFP vendor name                                   : %s\r\n", vendor_name);
    g_printf("Part number                                       : %s\r\n", vendor_PN);
    g_printf("Serial number                                     : %s\r\n", vendor_SN);
    g_printf("Manufactureing date                               : %s\r\n", date);
    g_printf("Port state                                        : %s\r\n",
        (port_disable == OPTICAL_TX_DISABLE) ? "Disable" : "Enable");

    uip_free_gvariant_list(output_list);
    return TRUE;
}


gint32 smm_get_stack_state(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location)
{
    OBJ_HANDLE obj_handle = 0;
    GVariant *property_data = NULL;
    gint32 ret_val;
    guint8 value;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    ret_val = dfl_get_object_handle(OBJ_NAME_MM_LSW_MGNT, &obj_handle);
    if (ret_val != DFL_OK) {
        debug_log(DLOG_ERROR, "get object handle failed: ret %d.", ret_val);
        return FALSE;
    }

    ret_val = dfl_get_property_value(obj_handle, PROTERY_CASCADE_PORT_STATE, &property_data);
    if (ret_val != DFL_OK) {
        debug_log(DLOG_ERROR, "get property value failed: ret %d.", ret_val);
        return FALSE;
    }

    value = g_variant_get_byte(property_data);
    g_variant_unref(property_data);
    g_printf("Stack state is %s.\r\n", (value ? "enable" : "disable"));

    return TRUE;
}


gint32 smm_get_outportmode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location)
{
    OBJ_HANDLE obj_handle = 0;
    GVariant *property_data = NULL;
    gint32 ret_val;
    guint8 value;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    ret_val = dfl_get_object_handle(OBJ_NAME_MM_LSW_MGNT, &obj_handle);
    if (ret_val != DFL_OK) {
        debug_log(DLOG_ERROR, "get object handle failed: ret %d.", ret_val);
        return FALSE;
    }

    ret_val = dfl_get_property_value(obj_handle, PROTERY_OUT_PORT_MODE, &property_data);
    if (ret_val != DFL_OK) {
        debug_log(DLOG_ERROR, "get property value failed: ret %d.", ret_val);
        return FALSE;
    }

    value = g_variant_get_byte(property_data);
    g_variant_unref(property_data);
    g_printf("Out port mode is %s.\r\n", (value ? "from smm" : "from switch"));

    return TRUE;
}


gint32 ipmc_get_removed_event_severity(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *location)
{
    OBJ_HANDLE object_handle = 0;
    guint8 severity = 0;
    gint32 ret = RET_OK;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc > 0) {
        print_get_removed_event_severity_helpinfo();
        return FALSE;
    }

    
    if (strncmp("pem", location, strlen(location)) == 0) {
        ret = dfl_get_object_handle(PS_REMOVEED_INFO, &object_handle);
    } else if (strncmp("fantray", location, strlen(location)) == 0) {
        ret = dfl_get_object_handle(FAN_REMOVEED_INFO, &object_handle);
    } else if (strncmp("blade", location, strlen(location)) == 0) {
        ret = dfl_get_object_handle(BLADE_REMOVEED_INFO, &object_handle);
    } else if (strncmp("swi", location, strlen(location)) == 0) {
        ret = dfl_get_object_handle(SWI_REMOVEED_INFO, &object_handle);
    } else if (strncmp("smm", location, strlen(location)) == 0) {
        ret = dfl_get_object_handle(SMM_REMOVEED_INFO, &object_handle);
    } else {
        print_get_removed_event_severity_helpinfo();
        return FALSE;
    }

    if (ret != DFL_OK) {
        print_get_removed_event_severity_helpinfo();
        return FALSE;
    }

    (void)dal_get_property_value_byte(object_handle, PROPERTY_EVENT_INFORMATION_SEVERITY, &severity);

    switch (severity) {
        case 0:
            g_printf("Normal.\r\n");
            break;
        case 1:
            g_printf("Minor.\r\n");
            break;
        case 2:
            g_printf("Major.\r\n");
            break;
        case 3:
            g_printf("Critical.\r\n");
            break;
        default:
            g_printf("Unknown level.\r\n");
            break;
    }

    return TRUE;
}


gint32 ipmc_get_lsw_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret_val;
    GSList *output_list = NULL;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    ret_val = dfl_get_object_handle(OBJ_NAME_MM_LSW_MGNT, &obj_handle);
    if (ret_val != DFL_OK) {
        g_printf("Get lsw info failed.");
        return FALSE;
    }

    ret_val = dfl_call_class_method(obj_handle, METHOD_GET_LSW_PORT_INFO, NULL, NULL, &output_list);
    if (ret_val != DFL_OK) {
        g_printf("Connecting to lsw module failed.");
        return FALSE;
    }

    g_slist_foreach(output_list, print_port_info, NULL);

    uip_free_gvariant_list(output_list);
    return TRUE;
}


gint32 ipmc_get_lsw_agetest_status(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret_val;
    GSList *output_list = NULL;
    gint32 total;
    gint32 remain;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        g_printf("Permission denied.\r\n");
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    ret_val = dfl_get_object_handle(OBJ_NAME_MM_LSW_MGNT, &obj_handle);
    if (ret_val != DFL_OK) {
        g_printf("Get lsw agetest status failed.");
        return FALSE;
    }

    ret_val = dfl_call_class_method(obj_handle, METHOD_GET_LSW_AGETEST_STATUS, NULL, NULL, &output_list);
    if (ret_val != DFL_OK) {
        g_printf("Connecting to lsw module failed.");
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        return FALSE;
    }

    total = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
    remain = total - g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 1));
    if (remain < 0) {
        remain = 0;
    }

    g_printf("Total: %dh\nRemain: %dh%dm.\r\n", total / 60, remain / 60, remain % 60);

    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    return TRUE;
}


LOCAL gint32 get_ipv6_floatip_info(OBJ_HANDLE obj_handle)
{
    gsize addr_len = 0;
    gsize local_len = 0;
    gint32 ret;
    guchar prefix = 0;
    const gchar *ip_addr = NULL;
    gchar gateway[IPV6_GATEWAY_STR_SIZE] = { 0 };
    const gchar *linkip = NULL;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;
    OBJ_HANDLE static_obj_handle = 0;

    
    property_name_list = g_slist_append(property_name_list, ETH_GROUP_ATTRIBUTE_IPV6_ADDR);
    property_name_list = g_slist_append(property_name_list, ETH_GROUP_ATTRIBUTE_IPV6_PREFIX);
    property_name_list = g_slist_append(property_name_list, ETH_GROUP_ATTRIBUTE_IPV6_LOCAL_LINK);
    ret = dfl_multiget_property_value(obj_handle, property_name_list, &property_value);
    if (ret != DFL_OK) {
        goto err_exit;
    }

    ret = dal_eth_get_out_type_object(OUT_ETHERNET, &static_obj_handle);
    if (ret != RET_OK) {
        goto err_exit;
    }

    ret = dal_get_property_value_string(static_obj_handle, ETH_GROUP_ATTRIBUTE_IPV6_GATEWAY, gateway, sizeof(gateway));
    if (ret != RET_OK) {
        goto err_exit;
    }

    
    ip_addr = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 0), &addr_len);

    
    prefix = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 1));
    if (ip_addr != NULL && addr_len != 0) {
        g_printf("IPv6 Information      :\r\n");
        g_printf("IPv6 Address          :  %s/%d\r\n", ip_addr, prefix);
    } else {
        g_slist_free(property_name_list);
        uip_free_gvariant_list(property_value);
        g_printf("IPv6 Address does not exist.\r\n");
        return RET_OK;
    }

    g_printf("Default Gateway IPv6  :  %s\r\n", gateway);

    
    linkip = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 2), &local_len);
    if (linkip != NULL) {
        g_printf("Link-Local Address    :  %s/%d\r\n", linkip, DEFAULT_PREFIX);
    }

    g_slist_free(property_name_list);
    uip_free_gvariant_list(property_value);

    return RET_OK;

err_exit:
    g_slist_free(property_name_list);
    uip_free_gvariant_list(property_value);

    return RET_ERR;
}


LOCAL gint32 get_ipv4_floatip_info(OBJ_HANDLE obj_handle)
{
    gint32 ret;
    const gchar *ip_addr = NULL;
    const gchar *submask = NULL;
    gchar gateway[IPV4_GATEWAY_STR_SIZE] = { 0 };
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;
    OBJ_HANDLE static_obj_handle = 0;
    gsize addr_len = 0;
    gsize mask_len = 0;

    property_name_list = g_slist_append(property_name_list, ETH_GROUP_ATTRIBUTE_IP_ADDR);
    property_name_list = g_slist_append(property_name_list, ETH_GROUP_ATTRIBUTE_SUB_MASK);
    ret = dfl_multiget_property_value(obj_handle, property_name_list, &property_value);
    if (ret != DFL_OK) {
        goto err_exit;
    }

    ret = dal_eth_get_out_type_object(OUT_ETHERNET, &static_obj_handle);
    if (ret != RET_OK) {
        goto err_exit;
    }

    ret = dal_get_property_value_string(static_obj_handle, ETH_GROUP_ATTRIBUTE_GATEWAY, gateway, sizeof(gateway));
    if (ret != RET_OK) {
        goto err_exit;
    }

    
    ip_addr = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 0), &addr_len);
    
    if (addr_len == 0 || strcmp(ip_addr, "0.0.0.0") == 0) {
        g_printf("IPv4 address does not exist.\r\n");
        g_slist_free(property_name_list);
        uip_free_gvariant_list(property_value);
        return RET_OK;
    }

    g_printf("IPv4 Information          :\r\n");
    g_printf("IP Address                :  %s\r\n", ip_addr);

    
    submask = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 1), &mask_len);
    if (submask != NULL) {
        g_printf("Subnet Mask               :  %s\r\n", submask);
    }

    g_slist_free(property_name_list);
    uip_free_gvariant_list(property_value);

    g_printf("Default Gateway           :  %s\r\n", gateway);
    return RET_OK;

err_exit:
    g_slist_free(property_name_list);
    uip_free_gvariant_list(property_value);
    return RET_ERR;
}


LOCAL gint32 print_float_ip_info(void)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    ret = dal_get_specific_object_byte(ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_ATTRIBUTE_OUT_TYPE, FLOATIP_ETHERNET,
        &obj_handle);
    if (ret != RET_OK) {
        
        debug_log(DLOG_ERROR, "%s:find floatip ethgroup failed.", __func__);
        
        return RET_ERR;
    }

    
    ret = get_ipv4_floatip_info(obj_handle);
    if (ret != RET_OK) {
        g_printf("Get float ip ipv4 info failed.\r\n");
        return RET_ERR;
    }

    
    ret = get_ipv6_floatip_info(obj_handle);
    if (ret != RET_OK) {
        g_printf("Get float ip ipv6 info failed.\r\n");
        return RET_ERR;
    }

    g_printf("\n");
    return RET_OK;
}


gint32 ipmc_get_float_ipinfo(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcget %s-d floatipinfo\r\n", help_default_name);
        return FALSE;
    }

    ret = print_float_ip_info();
    if (ret != RET_OK) {
        g_printf("Get float IP information failed.\r\n");
        return FALSE;
    }

    return TRUE;
}


gint32 smm_get_other_smm_ipinfo(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    guint8 slave_addr = 0;
    guint32 status = FALSE;
    gint32 ret_val;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Usage: ipmcget %s-d othersmmipinfo\r\n", help_default_name);
        return FALSE;
    }

    ret_val = get_other_board_present(&status);
    if (ret_val != RET_OK) {
        g_printf("Get other smm present state failed.\r\n");
        return FALSE;
    }

    if (status == BOARD_NOT_PRESENT) {
        g_printf("Get other smm ip info failed. Other smm is not present.\r\n");
        return FALSE;
    }

    ret_val = get_other_smm_slave_addr(&slave_addr);
    if (ret_val != RET_OK) {
        g_printf("Get other smm object failed.\r\n");
        return FALSE;
    }

    return print_blade_bmc_ip_info(slave_addr);
}

gint32 smm_get_shelf_powerstate(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    guint8 power_state;
    GVariant *value = NULL;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcget -d shelfpowerstate\r\n");
        return FALSE;
    }

    ret = uip_get_object_property(CLASS_SHELF_PAYLOAD, CLASS_SHELF_PAYLOAD, PROTERY_SHELF_POWER_STATE, &value);
    if (ret != RET_OK) {
        g_printf("Get shelf power state failed.\r\n ");
        return FALSE;
    }

    power_state = g_variant_get_byte(value);
    g_printf("Shelf power state   : %s\r\n", power_state ? "On" : "Off");
    g_variant_unref(value);

    return TRUE;
}


gint32 smm_get_power_button_mode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    guint8 button_mode;
    GVariant *value = NULL;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcget -d powerbuttonmode\r\n");
        return FALSE;
    }

    ret = uip_get_object_property(CLASS_SHELF_PAYLOAD, CLASS_SHELF_PAYLOAD, PROTERY_SHELF_POWER_BUTTON_MODE, &value);
    if (ret != RET_OK) {
        g_printf("Get shelf power button mode failed.\r\n ");
        return FALSE;
    }

    button_mode = g_variant_get_byte(value);
    g_printf("Shelf power button mode   : %s\r\n", button_mode ? "host" : "bmc");
    g_variant_unref(value);

    return TRUE;
}
