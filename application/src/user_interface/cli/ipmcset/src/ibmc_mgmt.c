

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
#include "ipmcset_ibmc_mgmt.h"
#include "ipmcset_sys_mgmt.h"
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

#define PORT_TYPE_CAS 3
#define PORT_TYPE_MGMT 4


gint32 ipmc_set_fpga_golden_fw_restore(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target)
{
    gint32 position = 0;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    gchar oper_log[MAX_OPERATE_LOG_LEN + 1] = {0};
    gint32 input;
    gint32 todo = 0;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argv[0] == NULL || strcmp(argv[0], "-v") != 0) {
        print_set_fpga_golden_fw_restore_usage(product_type_name);
        return FALSE;
    }

    gint8 slot_id = (gint8)str_to_int(argv[1]);
    if (argc == 2) {
        if (slot_id == IPMC_ERROR) {
            print_set_fpga_golden_fw_restore_usage(product_type_name);
            return FALSE;
        }
        position = 0;
    } else if (argc == 3) {
        position = (gint32)str_to_int(argv[2]);
        if (slot_id == IPMC_ERROR || position == IPMC_ERROR) {
            print_set_fpga_golden_fw_restore_usage(product_type_name);
            return FALSE;
        }
    } else {
        print_set_fpga_golden_fw_restore_usage(product_type_name);
        return FALSE;
    }

    g_printf("WARNING: This operation may cause unexpected reset of the OS and affect services.\r\n");
    input = check_the_input_operation(CLI_COMFIRM_STRING);
    if (input != RET_OK) {
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    
    gint32 ret = set_fpga_golden_fw_restore(slot_id, position, &todo);
    
    if ((ret == RET_OK) && (todo == TRUE)) {
        snprintf_s(oper_log, MAX_OPERATE_LOG_LEN + 1, MAX_OPERATE_LOG_LEN,
            "The restore of the Golden firmware of the FPGA card %d is starting.\n", slot_id);
        ipmc_log_operation_log("User", oper_log);

        ret = get_fpga_golden_fw_restore_result(slot_id);
        if (ret == RET_OK) {
            snprintf_s(oper_log, MAX_OPERATE_LOG_LEN + 1, MAX_OPERATE_LOG_LEN,
                "The restore of the Golden firmware of the FPGA card %d is successful.\n", slot_id);
            ipmc_log_operation_log("User", oper_log);
            ret = TRUE;
        } else {
            snprintf_s(oper_log, MAX_OPERATE_LOG_LEN + 1, MAX_OPERATE_LOG_LEN,
                "Failed to restore the Golden firmware of the FPGA card %d.\n", slot_id);
            ipmc_log_operation_log("User", oper_log);
            ret = FALSE;
        }
        
    } else {
        g_printf("The device may not support Golden firmware restore.\r\n");
        snprintf_s(oper_log, MAX_OPERATE_LOG_LEN + 1, MAX_OPERATE_LOG_LEN,
            "Failed to restore the Golden firmware of the FPGA card %d.\n", slot_id);
        ipmc_log_operation_log("User", oper_log);
        ret = FALSE;
    }

    return ret;
}


gint32 ipmc_set_maintenance_download(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 download_option = -1;
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    gchar component_name[TEMP_NAME_LENGTH] = {0};

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        ipmc_log_operation_log("Maintenance", "Insufficient privilege");
        return FALSE;
    }

    if ((argc != 2) || (strcmp(argv[0], "-v")) || dal_strtoi(argv[1], &download_option, NUMBER_BASE10) != RET_OK) {
        print_set_maintenance_download_helpinfo();
        return FALSE;
    }

    if (download_option != 1) {
        ipmc_log_operation_log("Maintenance", "Invalid input");
        print_set_maintenance_download_helpinfo();
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    g_printf("Download %s.\r\n", BIOS_FILE_TAR_TMP_PATH);
    
    gint32 ret = snprintf_s(component_name, TEMP_NAME_LENGTH, TEMP_NAME_LENGTH - 1, "%s", "BIOS");
    if (ret <= RET_OK) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        return FALSE;
    }
    g_printf("Downloading %s...\r\n", component_name);

    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)download_option));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, PFN_CLASS_NAME,
        OBJECT_PFN, METHOD_PFN_INIT_DOWNLOAD_FIRMWARE, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        g_printf("Download %s failed.\r\n", component_name);
        return RET_ERR;
    }
    if (ipmc_change_file_owner(BIOS_FILE_TAR_TMP_PATH) != RET_OK) {
        g_printf("Download %s failed.\r\n", component_name);
        return FALSE;
    }

    g_printf("Download %s successfully.\r\n", component_name);
    g_printf("Note: Please remove %s after use.\r\n", BIOS_FILE_TAR_TMP_PATH);
    return TRUE;
}


gint32 ipmc_set_maintenance_upgrade_cpld(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target)
{
    errno_t safe_fun_ret;
    gchar path_info[MAX_FILEPATH_LENGTH + 1] = {0};
    gint32 ret;
    const guint32 arc_cnt = 2;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        ipmc_log_operation_log("Maintenance", "Insufficient privilege");
        return FALSE;
    }

    if ((argc != arc_cnt) || (strcmp(argv[0], "-v"))) {
        g_printf("Usage: ipmcset -t maintenance -d upgradecpld -v <filepath>\r\n");
        g_printf("Filepath      e.g.: /tmp/cpldimage.hpm\r\n");

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

    
    g_printf("WARNING: This operation will forcibly upgrade the CPLD and reset the server, which will interrupt "
        "services for a period of time. The OS will be powered on or off based on the power-on policy.\r\n");
    ret = check_the_input_operation(CLI_COMFIRM_STRING);
    if (ret != RET_OK) {
        return FALSE;
    }

    return maintenance_upgrade_cpld(path_info, strlen(path_info));
}


gint32 ipmc_set_ntp_alternative_server(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target)
{
    gint32 ret;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    gchar alternative_server[IP_DN_ADDR_MAX_LEN] = {0};
    errno_t securec_rv;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return RET_ERR;
    }
    if ((argc != 2) || ((strcmp(argv[0], "-v") != 0) || (strlen(argv[1]) == 0))) {
        g_printf("Usage: ipmcset -t ntp -d alternativeserver -v <addr>\r\n");
        return RET_ERR;
    }

    securec_rv = strncpy_s(alternative_server, IP_DN_ADDR_MAX_LEN, argv[1], IP_DN_ADDR_MAX_LEN - 1);
    if (securec_rv != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, securec_rv);
        return RET_ERR;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        (void)snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        (void)snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    ret = set_ntp_alternative_server(OPERATOR_LOG_CLI, username, ip, (const gchar *)alternative_server);
    if (ret != RET_OK) {
        g_printf("Set NTP alternative server (%s) failed.\r\n", alternative_server);
        return RET_ERR;
    }

    g_printf("Set NTP alternative server (%s) successfully.\r\n", alternative_server);

    return RET_OK;
}


gint32 ipmc_set_ntp_enable_status(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    guint8 enable_status = 0;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return RET_ERR;
    }
    if (argc != 2 || strcmp(argv[0], "-v") != 0) {
        g_printf("Usage: ipmcset -t ntp -d status -v <enabled|disabled>\r\n");
        return RET_ERR;
    }

    if (strcasecmp(argv[1], "enabled") == 0) {
        enable_status = NTP_ENABLED;
    } else if (strcasecmp(argv[1], "disabled") == 0) {
        enable_status = NTP_DISABLED;
    } else {
        g_printf("Usage: ipmcset -t ntp -d status -v <enabled|disabled>\r\n");
        return RET_ERR;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    
    ret = set_ntp_enable_status(OPERATOR_LOG_CLI, username, ip, enable_status);
    if (ret != RET_OK) {
        g_printf("Set NTP enable status (%s) failed.\r\n", (enable_status) ? "enabled" : "disabled");
        return RET_ERR;
    }

    g_printf("Set NTP enable status (%s) successfully.\r\n", (enable_status) ? "enabled" : "disabled");

    return RET_OK;
}


gint32 ipmc_set_ntp_extra_server(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    gchar extra_server[MAX_EXTRA_NTP_SERVER_IP_LEN] = {0};
    errno_t securec_rv;

    if ((setcommand_privilege_judge(privilege, rolepriv) == FALSE)) {
        return RET_ERR;
    }
    if ((argc != IPMCSET_CMD_ARGC) || (strcmp(argv[0], "-v") != 0) || (strlen(argv[IPMCSET_FIRST_ARGV]) == 0)) {
        g_printf("Usage: ipmcset -t ntp -d extraserver -v <addr>\r\n");
        return RET_ERR;
    }

    securec_rv =
        strncpy_s(extra_server, MAX_EXTRA_NTP_SERVER_IP_LEN, argv[IPMCSET_FIRST_ARGV], MAX_EXTRA_NTP_SERVER_IP_LEN - 1);
    if (securec_rv != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, securec_rv);
        return RET_ERR;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        (void)snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        (void)snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    ret = set_ntp_extra_server(OPERATOR_LOG_CLI, username, ip, (const gchar *)extra_server);
    if (ret != RET_OK) {
        g_printf("Set NTP extra server (%s) failed.\r\n", extra_server);
        return RET_ERR;
    }

    g_printf("Set NTP extra server (%s) successfully.\r\n", extra_server);

    return RET_OK;
}


gint32 ipmc_set_ntp_auth_enable_status(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target)
{
    gint32 ret;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    guint8 auth_enable_status = 0;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return RET_ERR;
    }
    if ((argc != 2) || (strcmp(argv[0], "-v") != 0)) { // 2个参数
        g_printf("Usage: ipmcset -t ntp -d authstatus -v <enabled|disabled>\r\n");
        return RET_ERR;
    }

    if (strcasecmp(argv[1], "enabled") == 0) {
        auth_enable_status = NTP_ENABLED;
    } else if (strcasecmp(argv[1], "disabled") == 0) {
        auth_enable_status = NTP_DISABLED;
    } else {
        g_printf("Usage: ipmcset -t ntp -d authstatus -v <enabled|disabled>\r\n");
        return RET_ERR;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    
    ret = set_ntp_auth_enable_status(OPERATOR_LOG_CLI, username, ip, auth_enable_status);
    if (ret != RET_OK) {
        g_printf("Set NTP enable status (%s) failed.\r\n", (auth_enable_status) ? "enabled" : "disabled");
        return RET_ERR;
    }

    g_printf("Set NTP enable status (%s) successfully.\r\n", (auth_enable_status) ? "enabled" : "disabled");

    return RET_OK;
}


gint32 ipmc_set_ntp_group_key(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    errno_t safe_fun_ret;
    gint32 ret;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    gchar group_key_path[MAX_FILEPATH_LENGTH + 1] = {0};

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return RET_ERR;
    }
    if ((argc != 2) || (strcmp(argv[0], "-v") != 0) || (strlen(argv[1]) == 0)) { // 2个参数
        g_printf("Usage: ipmcset -t ntp -d groupkey -v <filepath>\r\n");
        return RET_ERR;
    }

    if (strlen(argv[1]) > MAX_FILEPATH_LENGTH) {
        g_printf("File path is too long.\r\n");
        return RET_ERR;
    }

    safe_fun_ret = strncpy_s(group_key_path, MAX_FILEPATH_LENGTH + 1, argv[1], MAX_FILEPATH_LENGTH);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    ret = import_ntp_group_key(OPERATOR_LOG_CLI, username, ip, (const gchar *)group_key_path);
    if (ret != RET_OK) {
        g_printf("Import NTP group key failed.\r\n");
        return RET_ERR;
    }

    g_printf("Set NTP group key successfully.\r\n");

    return RET_OK;
}

gint32 ipmc_set_ntp_mode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    guint8 mode = 0;
    gchar str_mode[PARAMETER_LEN + 1] = {0};
    const gchar *str_manual = "manual";
    const gchar *str_dhcpv4 = "dhcpv4";
    const gchar *str_dhcpv6 = "dhcpv6";

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return RET_ERR;
    }
    if (argc != 2 || strcmp(argv[0], "-v") != 0) {
        g_printf("Usage: ipmcset -t ntp -d mode -v <manual|dhcpv4|dhcpv6>\r\n");
        return RET_ERR;
    }

    if (strcasecmp(argv[1], str_manual) == 0) {
        mode = NTP_MODE_MANUAL;
        strncpy_s(str_mode, PARAMETER_LEN + 1, str_manual, strlen(str_manual));
    } else if (strcasecmp(argv[1], str_dhcpv4) == 0) {
        mode = NTP_MODE_DHCPV4;
        strncpy_s(str_mode, PARAMETER_LEN + 1, str_dhcpv4, strlen(str_dhcpv4));
    } else if (strcasecmp(argv[1], str_dhcpv6) == 0) {
        mode = NTP_MODE_DHCPV6;
        strncpy_s(str_mode, PARAMETER_LEN + 1, str_dhcpv6, strlen(str_dhcpv6));
    } else {
        g_printf("Usage: ipmcset -t ntp -d mode -v <manual|dhcpv4|dhcpv6>\r\n");
        return RET_ERR;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    ret = set_ntp_mode(OPERATOR_LOG_CLI, username, ip, mode);
    if (ret != RET_OK) {
        g_printf("Set NTP mode (%s) failed.\r\n", str_mode);
        return RET_ERR;
    }

    g_printf("Set NTP mode (%s) successfully.\r\n", str_mode);

    return RET_OK;
}


gint32 ipmc_set_ntp_preferred_server(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    gchar preferred_server[IP_DN_ADDR_MAX_LEN] = {0};
    errno_t securec_rv;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return RET_ERR;
    }
    if ((argc != 2) || ((strcmp(argv[0], "-v") != 0) && (strlen(argv[1]) != 0))) {
        g_printf("Usage: ipmcset -t ntp -d preferredserver -v <addr>\r\n");
        return RET_ERR;
    }

    securec_rv = strncpy_s(preferred_server, IP_DN_ADDR_MAX_LEN, argv[1], IP_DN_ADDR_MAX_LEN - 1);
    if (securec_rv != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, securec_rv);
        return RET_ERR;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        (void)snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        (void)snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    ret = set_ntp_preferred_server(OPERATOR_LOG_CLI, username, ip, (const gchar *)preferred_server);
    if (ret != RET_OK) {
        g_printf("Set NTP preferred server (%s) failed.\r\n", preferred_server);
        return RET_ERR;
    }

    g_printf("Set NTP preferred server (%s) successfully.\r\n", preferred_server);

    return RET_OK;
}


LOCAL gint32 get_year_month_day(gchar *input, guint16 *year, guint8 *month, guint8 *day)
{
    gchar *buf_temp = NULL;
    gchar *temp = NULL;

    temp = strtok_r(input, "-", &buf_temp);
    if (temp == NULL) {
        return RET_ERR;
    }

    gsize len = strlen(temp);
    if ((len != 4) || check_is_digint(temp, 4)) {
        return RET_ERR;
    }

    gint32 num = -1;
    if (dal_strtoi(temp, &num, NUMBER_BASE10) != RET_OK || num < 0 || num > USHRT_MAX) {
        debug_log(DLOG_ERROR, "%s: dal_strtoi failed", __FUNCTION__);
        return RET_ERR;
    }
    *year = num;

    temp = NULL;
    temp = strtok_r(buf_temp, "-", &buf_temp);
    if (temp == NULL) {
        return RET_ERR;
    }

    len = strlen(temp);
    if ((len > 2) || check_is_digint(temp, len)) {
        return RET_ERR;
    }

    num = -1;
    if (dal_strtoi(temp, &num, NUMBER_BASE10) != RET_OK || num < 0 || num > UCHAR_MAX) {
        debug_log(DLOG_ERROR, "%s: dal_strtoi failed", __FUNCTION__);
        return RET_ERR;
    }
    *month = num;

    temp = NULL;
    if (buf_temp == NULL) {
        return RET_ERR;
    }

    len = strlen(buf_temp);
    if ((len > 2) || check_is_digint(buf_temp, len)) {
        return RET_ERR;
    }

    num = -1;
    if (dal_strtoi(buf_temp, &num, NUMBER_BASE10) != RET_OK || num < 0 || num > UCHAR_MAX) {
        debug_log(DLOG_ERROR, "%s: dal_strtoi failed", __FUNCTION__);
        return RET_ERR;
    }
    *day = num;

    return RET_OK;
}


LOCAL gint32 get_hour_minute_second(gchar *input, guint8 *hour, guint8 *minute, guint8 *second)
{
    gchar *buf_temp = NULL;
    gchar *temp = NULL;

    temp = strtok_r(input, ":", &buf_temp);
    if (temp == NULL) {
        return RET_ERR;
    }

    gsize len = strlen(temp);
    if ((len > 2) || check_is_digint(temp, len)) {
        return RET_ERR;
    }

    gint32 num = -1;
    if (dal_strtoi(temp, &num, NUMBER_BASE10) != RET_OK || num < 0 || num > UCHAR_MAX) {
        debug_log(DLOG_ERROR, "%s: dal_strtoi failed", __FUNCTION__);
        return RET_ERR;
    }
    *hour = num;

    temp = NULL;
    temp = strtok_r(buf_temp, ":", &buf_temp);
    if (temp == NULL) {
        return RET_ERR;
    }

    len = strlen(temp);
    if ((len > 2) || check_is_digint(temp, len)) {
        return RET_ERR;
    }

    num = -1;
    if (dal_strtoi(temp, &num, NUMBER_BASE10) != RET_OK || num < 0 || num > UCHAR_MAX) {
        debug_log(DLOG_ERROR, "%s: dal_strtoi failed", __FUNCTION__);
        return RET_ERR;
    }
    *minute = num;

    if (buf_temp == NULL) {
        return RET_ERR;
    }
    len = strlen(buf_temp);
    if ((len > 2) || check_is_digint(buf_temp, len)) {
        return RET_ERR;
    }

    num = -1;
    if (dal_strtoi(buf_temp, &num, NUMBER_BASE10) != RET_OK || num < 0 || num > UCHAR_MAX) {
        debug_log(DLOG_ERROR, "%s: dal_strtoi failed", __FUNCTION__);
        return RET_ERR;
    }
    *second = num;

    return RET_OK;
}

LOCAL gint32 check_set_time_valid(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv)
{
    OBJ_HANDLE ntp_handle = 0;
    guchar ntp_enable = 0;

    (void)dfl_get_object_handle(OBJECT_NTP, &ntp_handle);
    (void)dal_get_property_value_byte(ntp_handle, PROPERTY_NTP_ENABLE_STATUS, &ntp_enable);
    if (ntp_enable == NTP_ENABLED) {
        g_printf("Set the time failed due to NTP is enabled.\r\n");
        ipmc_log_operation_log("BMC", "Set the time failed");
        return RET_ERR;
    }

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return RET_ERR;
    }

    if ((argc != 3) || (strcmp(argv[0], "-v"))) {
        print_rtc_time_usage();
        return RET_ERR;
    }

    return RET_OK;
}


gint32 ipmc_set_on_board_rtc_time(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    guint16 year;
    guint8 month;
    guint8 day;
    guint8 hour;
    guint8 minute;
    guint8 second;
    struct tm sttime = { 0 };

    if (check_set_time_valid(privilege, rolepriv, argc, argv) != RET_OK) {
        return FALSE;
    }

    if (get_year_month_day(argv[1], &year, &month, &day) != RET_OK) {
        print_rtc_time_usage();
        return FALSE;
    }

    if (get_hour_minute_second(argv[2], &hour, &minute, &second) != RET_OK) {
        print_rtc_time_usage();
        return FALSE;
    }

    gint32 ret = check_time_data(year, month, day, hour, minute, second);
    if (ret == TIME_FORMAT_ERR) {
        g_printf("Time format incorrect,please try again.\r\n");
        ipmc_log_operation_log("BMC", "Set the time failed");
        return RET_OK;
    }

    sttime.tm_year = year - YEAR_1900;
    sttime.tm_mon = month - 1;
    sttime.tm_mday = day;
    sttime.tm_hour = hour;
    sttime.tm_min = minute;
    sttime.tm_sec = second;

    time_t timestamp = mktime(&sttime);
    if (timestamp < 0 || timestamp > UINT32_MAX) {
        g_printf("Set the time failed.\r\n");
        ipmc_log_operation_log("BMC", "Set the time failed");
        return RET_OK;
    }
    ret = set_on_board_rtc_time((guint32)timestamp);
    if (ret == RET_OK) {
        g_printf("Set the time successfully.\r\n");
    } else {
        g_printf("Set the time failed.\r\n");
    }

    return ret;
}


gint32 ipmc_set_time_zone(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    errno_t safe_fun_ret;
    gchar time_zone[TIMEZONE_MAX_LEN] = {0};
    gchar time_zone_tmp[TIMEZONE_MAX_LEN] = {0};
    gchar result;

    
    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 2 || strcmp(argv[0], "-v")) {
        print_set_timezone_helpinfo();
        return FALSE;
    }

    if (strlen(argv[1]) + 1 > sizeof(time_zone_tmp)) {
        print_set_timezone_helpinfo();
        return FALSE;
    }

    safe_fun_ret = strncpy_s(time_zone_tmp, sizeof(time_zone_tmp), argv[1], sizeof(time_zone_tmp) - 1);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }

    
    if (time_zone_tmp[0] == '-' || time_zone_tmp[0] == '+' || (time_zone_tmp[0] >= '0' && time_zone_tmp[0] <= '9')) {
        safe_fun_ret = strncat_s(time_zone, sizeof(time_zone), TIME_ZONE_UTC, strlen(TIME_ZONE_UTC));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        if (time_zone_tmp[0] >= '0' && time_zone_tmp[0] <= '9') {
            safe_fun_ret = strncat_s(time_zone, sizeof(time_zone), "+", 1);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
        }
        safe_fun_ret = strncat_s(time_zone, sizeof(time_zone), time_zone_tmp, strlen(time_zone_tmp));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
    } else if (strncasecmp(time_zone_tmp, TIME_ZONE_UTC, strlen(TIME_ZONE_UTC)) == RET_OK ||
        strncasecmp(time_zone_tmp, TIME_ZONE_GMT, strlen(TIME_ZONE_GMT)) == RET_OK) {
        safe_fun_ret = strncpy_s(time_zone, sizeof(time_zone), time_zone_tmp, sizeof(time_zone) - 1);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
    } else {
        
        safe_fun_ret = strncpy_s(time_zone, sizeof(time_zone), time_zone_tmp, sizeof(time_zone) - 1);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        
        result = ipmc_weak_match_timezone(time_zone);
        if (result != RET_OK) {
            return FALSE;
        }
    }

    result = ipmc_set_timezone(time_zone);
    if (result == FALSE) {
        return FALSE;
    }

    g_printf("Set time zone successfully.\r\n");
    return TRUE;
}


gint32 ipmc_set_usbmgmt_enable_state(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    guchar is_enable;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if ((argc != SET_CMD_ARGC_NUM_TWO) || (strcmp(argv[0], "-v") != 0)) {
        g_printf(SET_USB_MGMT_ENABLE_STATE_CMD_HELP_INFO);
        return FALSE;
    }

    if (strcasecmp(argv[1], "enabled") == 0) {
        is_enable = TRUE;
    } else if (strcasecmp(argv[1], "disabled") == 0) {
        is_enable = FALSE;
    } else {
        g_printf(SET_USB_MGMT_ENABLE_STATE_CMD_HELP_INFO);
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    if (check_usb_mgmt_support_state(SET_USB_MGMT_ENABLE_STATE_CMD_FAIL_INFO) == FALSE) {
        return FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(is_enable));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_USB_MGMT,
        OBJ_USB_MGMT, METHOD_USB_MGMT_SET_ENABLE, input_list, NULL);
    uip_free_gvariant_list(input_list);

    if (ret != RET_OK) {
        g_printf(SET_USB_MGMT_ENABLE_STATE_CMD_FAIL_INFO);
        return FALSE;
    }

    g_printf(SET_USB_MGMT_ENABLE_STATE_CMD_SUCC_INFO);
    return TRUE;
}


gint32 ipmc_upgrade_component(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    GVariant *value = NULL;
    gchar path_info[MAX_FILEPATH_LENGTH + 1] = {0};
    guint8 upgrade_flag;
    guint8 forced_upgrade_flag;
    guint8 reset_flag = BMC_AUTO_VALID;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    ret = get_update_filepath(argc, argv, path_info, sizeof(path_info), &reset_flag);
    if (ret != RET_OK) {
        print_upgrade_usage();
        return FALSE;
    }

    
    ret = uip_get_object_property(PFN_CLASS_NAME, OBJECT_PFN, PFN_UPGRADE_INFLAG_NAME, &value);
    if (ret != RET_OK) {
        g_printf("Get upgrade flag failed.\r\n");
        return FALSE;
    }

    upgrade_flag = g_variant_get_byte(value);
    g_variant_unref(value);
    if (upgrade_flag != 0) {
        g_printf("An upgrade operation is in progress, please wait.\r\n");
        return FALSE;
    }

    g_printf("Please make sure the %s is working while upgrading.\r\n", product_type_name);

    forced_upgrade_flag = 0;
    ret = send_update_command(path_info, forced_upgrade_flag, reset_flag);
    if (ret != RET_OK) {
        g_printf("Upgrade failed.\r\n");
        (void)fflush(stdout);
        return FALSE;
    }

    (void)fflush(stdout);
    return TRUE;
}


gint32 ipmc_upgrade_pcieswitch_configuration(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target)
{
    gint32 ret;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    GSList *input_list = NULL;
    OBJ_HANDLE handle_8725 = 0;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        g_printf("Usage: ipmcset -d pcieswitch2upgrade -v <filepath>\r\n");
        g_printf("filepath      e.g.: /tmp/pcieswitch2.bin\r\n");
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    ret = dal_get_specific_object_byte(PCIE_SWITCH_CLASEE_NAME, PROPERTY_PCIE_SWITCH_TYPE, PCIE_SWITCH_8725_TYPE_CODE,
        &handle_8725);
    if (ret != RET_OK) {
        g_printf("Upgrade pcie switch2 configuration file failed.\r\n");
        return FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(argv[1]));

    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
        PCIE_SWITCH_CLASEE_NAME, handle_8725, PCIE_SWITCH_METHOD_WRITE_8725_CFG, input_list, NULL);

    uip_free_gvariant_list(input_list);

    if (ret != RET_OK) {
        if (ret == 0xff) {
            g_printf("An upgrade operation is in progress, please wait.\r\n");
        } else {
            g_printf("Upgrade pcie switch2 configuration file failed.\r\n");
        }

        return FALSE;
    }

    g_printf("Upgrade pcie switch2 configuration file successfully.\r\n");
    return TRUE;
}


gint32 ipmc_weak_match_timezone(const gchar *tz)
{
    gint32 ret;
    const TZ_CITY *tz_list = NULL;
    GSList *output_list = NULL;
    GVariant *property_value = NULL;
    gsize mem_sz = 0;
    guint32 i;
    guint32 j = 0;
    guint32 tz_len;
    guint32 area_len = 0;
    gchar tz_full_name[TIMEZONE_MAX_LEN] = {0};
    guint8 area_match_flag = 0;
    guint8 tz_match_flag = 0;

    ret = uip_call_class_method(NULL, NULL, NULL, CLASS_NAME_BMC, OBJ_NAME_BMC, METHOD_GET_TIMEZONE_LIST, NULL,
        &output_list);
    if (ret != RET_OK) {
        g_printf("Set time zone failed.\r\n");
        return RET_ERR;
    }

    property_value = (GVariant *)g_slist_nth_data(output_list, 0);
    tz_list = (const TZ_CITY *)g_variant_get_fixed_array(property_value, &mem_sz, sizeof(guint8));
    
    if (tz_list == NULL) {
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        return RET_ERR;
    }

    
    tz_len = strlen(tz);
    for (i = 0; i < TIMEZONE_AREA_NUM; ++i) {
        area_len = strlen(tz_list[i].area);
        if (area_len > tz_len) {
            continue;
        }
        if ((tz[area_len] != '\0') && (tz[area_len] != '/')) {
            continue;
        }
        if (strncasecmp(tz_list[i].area, tz, area_len) != RET_OK) {
            continue;
        }

        area_match_flag = 1;
        for (j = 0; j < tz_list[i].city_num; ++j) {
            constr_area_city_to_timezone(tz_list[i].area, tz_list[i].city[j], tz_full_name, TIMEZONE_MAX_LEN);
            if (strcasecmp(tz_full_name, tz) == RET_OK) {
                tz_match_flag = 1;
            }
            if (tz_match_flag == 1) {
                break;
            }
        }

        if (area_match_flag == 1) {
            break;
        }
    }

    if (area_match_flag == 1) {
        if (tz_match_flag != 1) {
            g_printf("Illegal time zone, please set a legal time zone.\r\n");
            print_timezone_city_helpinfo(&tz_list[i]);
            g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
            return RET_ERR;
        } else {
            g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
            return RET_OK;
        }
    } else {
        g_printf("Illegal time zone, please set a legal time zone.\r\n");
        print_timezone_area_helpinfo(tz_list, TIMEZONE_AREA_NUM);
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        return RET_ERR;
    }
}

LOCAL gint32 ipmc_mpath_condition_check(const gchar *username, const gchar *ip, OBJ_HANDLE obj_handle)
{
    GSList* output_list = NULL;
    gint32 ret;
    gint32 check_ret;

    
    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, username, ip,
        CLASS_LSW_PUBLIC_ATTR_LOCAL_DP, obj_handle, METHOD_MESH_MPATH_CONDITION_CHECK, NULL, &output_list);
    if (ret != VOS_OK) {
        g_printf("Check mesh multiple path condition failed, ret(%d).\r\n", ret);
        return FALSE;
    }

    check_ret = g_variant_get_int32((GVariant*)g_slist_nth_data(output_list, 0));
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    if (check_ret != RET_OK) {
        g_printf("Set mesh multiple path mode failed.\r\n");
        g_printf("Node is in relay state or initializing.\r\n");
        return FALSE;
    }

    return TRUE;
}

LOCAL gint32 ipmc_get_username_ip(gchar* username, guint32 username_size, gchar* ip, guint32 ip_size)
{
    gint32 ret;

    if (get_username_ip(username, ip, ip_size) != VOS_OK) {
        ret = snprintf_s(username, username_size, PARAMETER_LEN, "%s", "unknown");
        if (ret < 0) {
            g_printf("get username fail.\r\n");
            return FALSE;
        }
        ret = snprintf_s(ip, ip_size, PARAMETER_LEN, "%s", "unknown");
        if (ret < 0) {
            g_printf("get ip fail.\r\n");
            return FALSE;
        }
    }
    return TRUE;
}

gint32 ipmc_set_mesh_mpath_mode(guchar privilege, const gchar* rolepriv, guchar argc, gchar** argv, gchar* target)
{
    gint32 operate_index;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;
    GSList* input_list = NULL;
    gchar *class_name = CLASS_LSW_PUBLIC_ATTR_LOCAL_DP;
    gchar *class_method = METHOD_DATA_SET_MESH_MPATH;
    gint32 ret;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    
    if ((argc != 2) || (strcmp(argv[0], "-v") != 0)) {
        print_mesh_mpath_mode_usage();
        return FALSE;
    }

    operate_index = str_to_int(argv[1]);
    if ((operate_index != DISABLE) && (operate_index != ENABLE)) {
        print_mesh_mpath_mode_usage();
        return FALSE;
    }

    ret = ipmc_get_username_ip(username, sizeof(username), ip, sizeof(ip));
    if (ret != TRUE) {
        return FALSE;
    }

    ret = dal_get_object_handle_nth((const gchar *)class_name, 0, &obj_handle);
    if (ret != VOS_OK) {
        return FALSE;
    }

    
    ret = ipmc_mpath_condition_check((const gchar*)username, (const gchar*)ip, obj_handle);
    if (ret != TRUE) {
        return FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_uint32(operate_index));

    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, (const gchar*)username, (const gchar*)ip,
        (const gchar *)class_name, obj_handle, (const gchar *)class_method, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != VOS_OK) {
        g_printf("Set mesh multiple path mode %d failed, ret(%d).\r\n", operate_index, ret);
        return FALSE;
    }

    g_printf("Set mesh multiple path mode successfully.\r\n");

    return TRUE;
}

LOCAL gint32 print_panel_port(OBJ_HANDLE object_handle, gpointer user_data)
{
    gint32 ret;
    guint32 port_type = 0;
    gchar port_name[LSW_PORT_NAME_MAX_LEN] = {0};

    ret = dal_get_property_value_uint32(object_handle, PROPERTY_LSW_PORT_TYPE, &port_type);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s] get port type failed, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }
    
    if ((port_type != PORT_TYPE_CAS) && (port_type != PORT_TYPE_MGMT)) {
        return RET_OK;
    }
    ret = dal_get_property_value_string(object_handle, PROPERTY_LSW_SUB_PORT_NAME, port_name, sizeof(port_name));
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s] get port name failed, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }
    g_printf("%s ", port_name);

    return RET_OK;
}

LOCAL void print_helpinfo_on_port_err(void)
{
    gint32 ret;

    g_printf("Usage: ipmcset -t lsw -d portenable -v <portname> <enable|disable>\r\n");
    g_printf("Correct port: ");

    ret = dfl_foreach_object(CLASS_LSW_PORT_ATTR_LOCAL_CP, print_panel_port, NULL, NULL);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s] dfl_foreach_object failed, ret=%d", __FUNCTION__, ret);
        g_printf("\r\nprint_panel_port failed\r\n");
        return;
    }

    g_printf("\r\n");
}

LOCAL gint32 get_port_info_from_arg(const gchar *arg_port_name, gchar *port_name, size_t max_port_name_len,
                                    OBJ_HANDLE *obj_handle)
{
    gint32 ret;
    gchar *tmp_name = NULL;
    GVariant* property_value = NULL;
    guint32 port_type = 0;

    tmp_name = g_ascii_strup(arg_port_name, strlen(arg_port_name));
    if (tmp_name == NULL) {
        debug_log(DLOG_ERROR, "[%s] g_ascii_strup failed", __FUNCTION__);
        g_printf("unknown error.\r\n");
        return RET_ERR;
    }
    property_value = g_variant_new_string((const gchar*)tmp_name);
    g_free(tmp_name);
    tmp_name = NULL;
    ret = dfl_get_specific_object(CLASS_LSW_PORT_ATTR_LOCAL_CP, PROPERTY_LSW_SUB_PORT_NAME,
                                  property_value, obj_handle);
    g_variant_unref(property_value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s] dfl_get_specific_object failed, ret=%d", __FUNCTION__, ret);
        print_helpinfo_on_port_err();
        return RET_ERR;
    }
    ret = dal_get_property_value_string(*obj_handle, PROPERTY_LSW_SUB_PORT_NAME, port_name, max_port_name_len);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s] get port name failed, ret=%d", __FUNCTION__, ret);
        g_printf("unknown error.\r\n");
        return RET_ERR;
    }
    ret = dal_get_property_value_uint32(*obj_handle, PROPERTY_LSW_PORT_TYPE, &port_type);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s] get port(%s) type failed, ret=%d", __FUNCTION__, port_name, ret);
        g_printf("unknown error.\r\n");
        return RET_ERR;
    }
    
    if ((port_type != PORT_TYPE_CAS) && (port_type != PORT_TYPE_MGMT)) {
        debug_log(DLOG_ERROR, "[%s] port %s does not support configuration, port_type(%u)",
                  __FUNCTION__, port_name, port_type);
        print_helpinfo_on_port_err();
        return RET_ERR;
    }

    return RET_OK;
}

LOCAL gint32 get_enable_from_arg(const gchar *arg_enable_str, guint32 *port_enable)
{
    if (strcmp(arg_enable_str, "enable") == 0) {
        *port_enable = LSW_LOCAL_PORT_STATUS_ENABLE;
    } else if (strcmp(arg_enable_str, "disable") == 0) {
        *port_enable = LSW_LOCAL_PORT_STATUS_DISABLE;
    } else {
        g_printf("Usage: ipmcset -t lsw -d portenable -v <portname> <enable|disable>\r\n");
        return RET_ERR;
    }
    return RET_OK;
}

gint32 ipmc_set_lsw_port_enable(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    gchar port_name[LSW_PORT_NAME_MAX_LEN] = {0};
    OBJ_HANDLE obj_handle = 0;
    guint32 port_enable = LSW_LOCAL_PORT_STATUS_DISABLE;
    gchar user_name[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    GSList* input_list = NULL;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if ((argc != 3) || (strcmp(argv[0], "-v") != 0)) {  // 该命令携带3个入参，格式为-v <portname> <enable|disable>
        g_printf("Usage: ipmcset -t lsw -d portenable -v <portname> <enable|disable>\r\n");
        return FALSE;
    }

    ret = get_port_info_from_arg((const gchar*)(argv[1]), port_name, sizeof(port_name), &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get_port_info_from_arg failed, ret=%d", __FUNCTION__, ret);
        return FALSE;
    }

    ret = get_enable_from_arg((const gchar*)argv[2], &port_enable); // argv[2]为enable或disable
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get_enable_from_arg failed, ret=%d", __FUNCTION__, ret);
        return FALSE;
    }

    ret = ipmc_get_username_ip(user_name, sizeof(user_name), ip, sizeof(ip));
    if (ret != TRUE) {
        debug_log(DLOG_ERROR, "[%s] ipmc_get_username_ip failed, ret=%d", __FUNCTION__, ret);
        return FALSE;
    }

    g_printf("This operation will change the enable state of LAN switch chip port.\r\n");
    ret = check_the_input_operation(CLI_COMFIRM_STRING);
    if (ret != RET_OK) {
        return FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(port_name));
    input_list = g_slist_append(input_list, g_variant_new_uint32(port_enable));

    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, (const gchar*)user_name, (const gchar*)ip,
                                            CLASS_LSW_PORT_ATTR_LOCAL_CP, obj_handle, METHOD_LSW_SET_PORT_ENABLE,
                                            input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != RET_OK) {
        g_printf("Set port %s failed.\r\n", (port_enable == LSW_LOCAL_PORT_STATUS_ENABLE) ? "enable" : "disable");
        return FALSE;
    }
    g_printf("Success.\r\n");

    return TRUE;
}

gint32 ipmc_set_lldp_enable_status(guchar privilege, const gchar* rolepriv, guchar argc, gchar** argv, gchar* target)
{
    gint32 ret;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    guint8 status;
    OBJ_HANDLE obj_handle = 0;
    GSList* input_list = NULL;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    
    if ((argc != 2) || (strcmp(argv[0], "-v") != 0)) {
        g_printf("Usage: ipmcset -t lldp -d status -v <enabled|disabled>\r\n");
        return FALSE;
    }

    if (strcasecmp(argv[1], "enabled") == 0) {
        status = BMC_ENABLE;
    } else if (strcasecmp(argv[1], "disabled") == 0) {
        status = BMC_DISABLE;
    } else {
        g_printf("Usage: ipmcset -t lldp -d status -v <enabled|disabled>\r\n");
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != VOS_OK) {
        ret = snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        if (ret < 0) {
            g_printf("get username fail.\r\n");
            return FALSE;
        }
        ret = snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        if (ret < 0) {
            g_printf("get ip fail.\r\n");
            return FALSE;
        }
    }

    ret = dal_get_object_handle_nth(CLASS_NAME_LLDP_CONFIG, 0, &obj_handle);
    if (ret != DFL_OK) {
        g_printf("get obj_handle fail, ret(%d).\r\n", ret);
        return FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(status));
    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, (const gchar*)username, (const gchar*)ip,
        CLASS_NAME_LLDP_CONFIG, obj_handle, METHOD_LLDP_CONFIG_LLDP_ENABLE, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != DFL_OK) {
        g_printf("Set LLDP enable status (%s) failed.\r\n", (status == BMC_ENABLE) ? "enabled" : "disabled");
        return FALSE;
    }

    g_printf("Set LLDP enable status (%s) successfully.\r\n",  (status == BMC_ENABLE) ? "enabled" : "disabled");

    return TRUE;
}

LOCAL void print_set_mesh_itf_port_mode_usage(void)
{
    g_printf("Usage: ipmcset -t mesh -d itfportmode -v <option>\r\n");
    g_printf("Options are:\r\n");
    g_printf("    1     Set one trunk group with 2*100GE only\r\n");
    g_printf("    2     Set two trunk groups with 8*25GE only\r\n");
    g_printf("    3     Set high bandwidth 400GE mode, and set two trunk groups with 8*25GE\r\n");
    return;
}

gint32 ipmc_lsw_set_mesh_itfportmode(guchar priv, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint8 port_mode = 0;
    gchar user_name[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    GSList *input_list = NULL;
    const gchar *class_name = CLASS_LSW_NODE_MANAGE_DP;

    if (setcommand_privilege_judge(priv, rolepriv) == FALSE) {
        return FALSE;
    }
    if ((argc != 2) || (strcmp(argv[0], "-v") != 0)) {  // 该命令携带2个入参，格式为-v <option>
        print_set_mesh_itf_port_mode_usage();
        return FALSE;
    }

    ret = dal_get_object_handle_nth(class_name, 0, &obj_handle);
    if (ret != VOS_OK) {
        return FALSE;
    }

    ret = ipmc_get_username_ip(user_name, sizeof(user_name), ip, sizeof(ip));
    if (ret != TRUE) {
        debug_log(DLOG_ERROR, "[%s] ipmc_get_username_ip failed, ret=%d", __FUNCTION__, ret);
        return FALSE;
    }

    
    ret = vos_str2int(argv[1], 10, &port_mode, NUM_TPYE_UCHAR);
    if ((ret != VOS_OK) || (port_mode == 0) || (port_mode >= PORT_MODE_MAX)) {
        print_set_mesh_itf_port_mode_usage();
        return FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(port_mode));
    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, (const gchar*)user_name, (const gchar*)ip,
        class_name, obj_handle, METHOD_LSW_SET_ITF_PORT_MODE, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret == RET_OK) {
        g_printf("Set itf port mode successfully.\r\n");
        return TRUE;
    } else if (ret == RET_ERR_NOT_EXIST) {
        g_printf("Failed to set the itfportmode to %u because the 2*100GE itf ports do not exist.\r\n", port_mode);
    } else {
        g_printf("Set itf port mode %u failed.\r\n", port_mode);
    }

    return FALSE;
}
