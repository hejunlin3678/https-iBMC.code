

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
#include "ipmcset_alarm.h"

gint32 smm_set_smm_failover(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{


#define RPC_REMOTE_TIMED_OUT (-202)
    gint32 ret;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    gint32 input;
    gsize length = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        g_printf("Permission denied.\r\n");
        return FALSE;
    }
    if ((argc != 2) || (strcmp("-v", argv[0]) != 0) || (strcmp("1", argv[1]) != 0)) {
        g_printf("Usage: ipmcset %s-d failover -v 1\r\n", help_default_name);
        return FALSE;
    }

    input = ipmc_get_user_input("This operation will make SMM switch over to standby.Continue?[Y/N]:");
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

    input_list = g_slist_append(input_list, g_variant_new_uint32(ASM_FAILOVER_OCCURRED_BY_USER_COMMAND));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, ASM_CLASS_NAME,
        ASM_OBJECT_NAME, METHOD_ASM_FAILOVER, input_list, &output_list);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK && ret != RPC_REMOTE_TIMED_OUT) {
        g_printf("Connect to asm fail. ret:%d\r\n", ret);
        return TRUE;
    }
    
    if (ret == RPC_REMOTE_TIMED_OUT) {
        g_printf("Success, try to failover\r\n");
        return TRUE;
    }
    if (g_variant_get_uint32((GVariant *)g_slist_nth_data(output_list, 0)) != 0) {
        g_printf("Failed. %s\r\n", g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 1), &length));
    } else {
        g_printf("%s\r\n", g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 1), &length));
    }

    uip_free_gvariant_list(output_list);

    return TRUE;
}
gint32 vsmm_set_smm_failover(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    gint32 input;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    gsize length = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        g_printf("Permission denied.\r\n");
        return FALSE;
    }
    if (argc != 2) {
        g_printf("Usage: ipmcset %s-d failover -v 1\r\n", help_default_name);
        return FALSE;
    }
    if ((strcmp("-v", argv[0]) != 0) || (strcmp("1", argv[1]) != 0)) {
        g_printf("Usage: ipmcset %s-d failover -v 1\r\n", help_default_name);
        return FALSE;
    }

    input = ipmc_get_user_input("This operation will make local blade switch over to secondary.Continue?[Y/N]:");
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

    input_list = g_slist_append(input_list, g_variant_new_uint32(ASM_FAILOVER_OCCURRED_BY_USER_COMMAND));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, ASM_CLASS_NAME,
        ASM_OBJECT_NAME, METHOD_ASM_FAILOVER, input_list, &output_list);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        g_printf("Connect to msm fail. ret:%d\r\n", ret);
        return RET_ERR;
    }

    if (g_variant_get_uint32((GVariant *)g_slist_nth_data(output_list, 0)) != 0) {
        g_printf("Failed. %s\r\n", g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 1), &length));
    } else {
        g_printf("%s\r\n", g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 1), &length));
    }

    uip_free_gvariant_list(output_list);

    return RET_OK;
}

gint32 smm_set_bladepowercontrol(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
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

    if ((argc != 0x2) || (strcmp(argv[0], "-v"))) {
        print_bladepowercontrol_usage();
        return FALSE;
    }

    if (strlen(argv[1]) == 0) {
        print_bladepowercontrol_usage();
        return RET_ERR;
    }

    ret = vos_str2int(argv[1], 10, &option, NUM_TPYE_INT32);
    if (ret != RET_OK) {
        print_bladepowercontrol_usage();
        return ret;
    }

    if ((option != 0) && (option != 1)) {
        g_printf("Set fail:parameter error.\r\n");
        print_bladepowercontrol_usage();
        return RET_ERR;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }
    iRet = snprintf_s(obj_name, OBJ_NAME_MAX_LEN, OBJ_NAME_MAX_LEN - 1, "%s", CLASS_NAME_SHELF_MANAGE);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    input_list = g_slist_append(input_list, g_variant_new_byte((guchar)option));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_NAME_SHELF_MANAGE,
        obj_name, METHOD_SET_SHELF_RATED_POWER_CTRL_EN, input_list, NULL);
    if (ret != RET_OK) {
        g_printf("Set fail:internal error.\r\n");
        uip_free_gvariant_list(input_list);
        return FALSE;
    }

    g_printf("%s blade power control  successfully.\r\n", (option ? "Enable" : "Disable"));
    uip_free_gvariant_list(input_list);
    return TRUE;
}


gint32 smm_set_smalert_config(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    OBJ_HANDLE object_handle = 0;
    guint8 enable = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 2) {
        g_printf("Usage: ipmcset [-l smm] -d smalertenable -v value(enable or disable)\r\n");
        return FALSE;
    } else if (strncmp("-v", argv[0], strlen(argv[0]))) {
        g_printf("Usage: ipmcset [-l smm] -d smalertenable -v value(enable or disable)\r\n");
        return FALSE;
    }

    if (!strncmp("enable", argv[1], strlen(argv[1]))) {
        enable = 1;
    } else if (!strncmp("disable", argv[1], strlen(argv[1]))) {
        enable = 0;
    } else {
        g_printf("Please input right value(enable or disable).\r\n");
        return FALSE;
    }

    ret = dal_get_object_handle_nth(CLASS_POWER_ALERT, 0, &object_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get power alert object failed. ret = %d\n", __FUNCTION__, ret);
        g_printf("Set smalert enable failed.\r\n");
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(enable));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_POWER_ALERT,
        dfl_get_object_name(object_handle), METHOD_SET_POWER_ALERT_ENABLE, input_list, &output_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "dfl_call_class_method %s.%s. Ret is %d\n", CLASS_POWER_ALERT,
            METHOD_SET_POWER_ALERT_ENABLE, ret);
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        g_printf("Set power alert enable failed.\r\n");
        return FALSE;
    }

    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

    g_printf("Success.\r\n");

    return TRUE;
}


gint32 smm_set_smalert_status(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    OBJ_HANDLE object_handle = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    gint32 ret_code;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 2) {
        g_printf("Usage: ipmcset [-l smm] -d smalertstatus -v 1\r\n");
        return FALSE;
    } else if (strncmp("-v", argv[0], strlen(argv[0]))) {
        g_printf("Usage: ipmcset [-l smm] -d smalertstatus -v 1\r\n");
        return FALSE;
    }

    if (strncmp("1", argv[1], strlen(argv[1]))) {
        g_printf("Please input right value(1).\r\n");
        return FALSE;
    }

    ret = dal_get_object_handle_nth(CLASS_POWER_ALERT, 0, &object_handle);
    if (ret != RET_OK) {
        g_printf("Set smalert status failed.\r\n");
        return FALSE;
    }

    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_POWER_ALERT,
        dfl_get_object_name(object_handle), METHOD_ALERT_RECOVERY, input_list, &output_list);
    if (ret != RET_OK) {
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        g_printf("Set power alert enable failed.\r\n");
        return FALSE;
    }

    ret_code = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

    if (ret_code == POWER_ALERT_DISABLED) { 
        g_printf("Smalert is disabled.\r\n");
        return TRUE;
    }

    g_printf("Success.\r\n");

    return TRUE;
}

LOCAL gint32 set_chassis_id_check_input(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv)
{
    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        ipmc_log_operation_log(CLASS_NAME_CHASSIS, "Set chassis id failed.");
        return FALSE;
    }

    // Check input
    if (argc != 2) {
        g_printf("Usage: ipmcset -l shelf -d chassisid -v value\r\nValue: range 0 ~ 999999. 1000000 is "
            "default value\r\n");
        return FALSE;
    }
    if (strcmp("-v", argv[0]) != 0) {
        g_printf("Usage: ipmcset -l shelf -d chassisid -v value\r\nValue: range 0 ~ 999999. 1000000 is "
            "default value\r\n");
        return FALSE;
    }
    if (strlen(argv[1]) == 0) {
        g_printf("Usage: ipmcset -l shelf -d chassisid -v value\r\nValue: range 0 ~ 999999. 1000000 is "
            "default value\r\n");
        return FALSE;
    }
    return TRUE;
}
gint32 smm_set_chassis_id(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    gint32 id;
    gchar *end_pos = NULL;
    OBJ_HANDLE handle;
    gchar username[PARAMETER_LEN + 1] = { 0 };
    gchar ip[PARAMETER_LEN + 1] = { 0 };
    GSList *input_list = NULL;
    gchar oper_log[MAX_OPERATE_LOG_LEN + 1] = {0};

    if (set_chassis_id_check_input(privilege, rolepriv, argc, argv) == FALSE) {
        debug_log(DLOG_ERROR, "%s: call set_chassis_id_check_input fail", __FUNCTION__);
        return FALSE;
    }

    errno = 0;
    id = strtol(argv[1], &end_pos, 10);
    if (*end_pos != '\0' || id < 0 || id > 999999 || (errno == ERANGE)) {
        g_printf("Value is not right, range 0 ~ 999999.\r\n");
        (void)snprintf_s(oper_log, MAX_OPERATE_LOG_LEN + 1, MAX_OPERATE_LOG_LEN, "Set chassis id(%d) failed.", id);
        ipmc_log_operation_log(CLASS_NAME_CHASSIS, oper_log);
        return FALSE;
    }

    // Get handle
    ret = dal_get_object_handle_nth(CLASS_NAME_CHASSIS, 0, &handle);
    if (ret != RET_OK) {
        g_printf("Set chassis id failed.\r\n");
        return FALSE;
    }

    // Set chassis id
    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_uint32((guint32)id));
    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, username, ip, CLASS_NAME_CHASSIS, handle,
        METHOD_CHASSIS_SET_CHASSIS_NUMBER, input_list, NULL);
    if (ret != RET_OK) {
        g_printf("Set chassis id failed.\r\n");
        uip_free_gvariant_list(input_list);
        return FALSE;
    }
    g_printf("Set chassis id successfully.\r\n");
    uip_free_gvariant_list(input_list);
    return TRUE;
}

gint32 smm_set_chassis_name(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    gchar *name = NULL;
    OBJ_HANDLE handle;
    gchar username[PARAMETER_LEN + 1] = { 0 };
    gchar ip[PARAMETER_LEN + 1] = { 0 };
    GSList *input_list = NULL;
    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    // Check input
    if (argc == 2 && strcmp("-v", argv[0]) == 0) { // Set chassis name
        name = argv[1];
    } else { // Wrong format
        g_printf("Usage: ipmcset -l shelf -d chassisname -v string\r\nString: name of shelf, string length range 0 ~ "
            "20\r\n");
        return FALSE;
    }
    if (strlen(name) > 20) {
        g_printf("Value is not right, string length range 0 ~ 20.\r\n");
        return FALSE;
    }
    // Get handle
    ret = dal_get_object_handle_nth(CLASS_NAME_CHASSIS, 0, &handle);
    if (ret != RET_OK) {
        g_printf("Set chassis name failed.\r\n");
        return FALSE;
    }
    // Set chassis name
    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_string(name));
    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, username, ip, CLASS_NAME_CHASSIS, handle,
        METHOD_CHASSIS_SET_CHASSIS_NAME, input_list, NULL);
    if (ret != RET_OK) {
        g_printf("Set chassis name failed.\r\n");
        uip_free_gvariant_list(input_list);
        return FALSE;
    }
    g_printf("Set chassis name ok.\r\n");
    uip_free_gvariant_list(input_list);
    return TRUE;
}

gint32 smm_set_chassis_location(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gchar *chassis_location = NULL;
    OBJ_HANDLE handle;
    gchar username[PARAMETER_LEN + 1] = { 0 };
    gchar ip[PARAMETER_LEN + 1] = { 0 };
    GSList *input_list = NULL;
    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        ipmc_log_operation_log(CLASS_NAME_CHASSIS, "Set chassis location failed.");
        return FALSE;
    }
    if (argc == 2 && strcmp("-v", argv[0]) == 0) { // Set chassis name
        chassis_location = argv[1];
    } else { // Wrong format
        g_printf("Usage: ipmcset -l shelf -d location -v string\r\nString: location of shelf, string length range 0 ~ "
            "20\r\n");
        return FALSE;
    }
    if (strlen(chassis_location) > 20) {
        g_printf("Value is not right, string length range 0 ~ 20.\r\n");
        ipmc_log_operation_log(CLASS_NAME_CHASSIS, "Set chassis location failed.");
        return FALSE;
    }
    if (dal_check_string_is_valid_ascii(chassis_location) != RET_OK) { // 不支持设置非ascii码字符
        g_printf("Value is not right, string should be valid ascii.\r\n");
        ipmc_log_operation_log(CLASS_NAME_CHASSIS, "Set chassis location failed.");
        return FALSE;
    }
    // Get handle
    gint32 ret = dal_get_object_handle_nth(CLASS_NAME_CHASSIS, 0, &handle);
    if (ret != RET_OK) {
        g_printf("Set chassis location failed.\r\n");
        return FALSE;
    }
    // Set chassis location
    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_string(chassis_location));
    input_list = g_slist_append(input_list, g_variant_new_byte(1)); // 写EEPROM标志位
    input_list = g_slist_append(input_list, g_variant_new_byte(1)); // 分发标志位
    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, username, ip, CLASS_NAME_CHASSIS, handle,
        METHOD_CHASSIS_SET_CHASSIS_LOCATION, input_list, NULL);
    if (ret != RET_OK) {
        g_printf("Set chassis location failed.\r\n");
        uip_free_gvariant_list(input_list);
        return FALSE;
    }
    g_printf("Set chassis location ok.\r\n");
    uip_free_gvariant_list(input_list);
    return TRUE;
}

gint32 smm_set_shelf_serialnumber(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target,
    gchar *location)
{
    errno_t safe_fun_ret;
    gint32 result;
    gchar *serialnumber = NULL;
    gpointer response = NULL;
    const guint8 *resp_src_data = NULL;
    IPMI_REQ_MSG_HEAD_S req_msg_head;
    guint8 reqdata[128] = { 0 };

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    // Check input
    if (argc == 2 && strcmp("-v", argv[0]) == 0) { // Set chassis name
        serialnumber = argv[1];
    } else { // Wrong format
        g_printf("Usage: ipmcset -l shelf -t fru -d shelfserialnumber -v string\r\nString: Serial number of shelf, "
            "string length range 0 ~ 20\r\n");
        return FALSE;
    }

    if (strlen(serialnumber) > 20) {
        g_printf("Value is not right, string length range 0 ~ 20.\r\n");
        return FALSE;
    }
    if (dal_check_string_is_valid_ascii(serialnumber) != RET_OK) { // 不支持设置非ascii码字符
        g_printf("Value is not right, string should be valid ascii.\r\n");
        return FALSE;
    }

    req_msg_head.target_type = IPMI_SMM;
    req_msg_head.target_instance = 1;
    req_msg_head.netfn = 0x30;
    req_msg_head.lun = 0;
    req_msg_head.cmd = 0x90;
    req_msg_head.src_len = 6 + strlen(serialnumber);
    reqdata[0] = 0x04; // 4:write elable data
    reqdata[1] = 0x01; // fru_id:1
    reqdata[2] = 0x01; // 1:chassis info area
    reqdata[3] = 0x02; // 2:Chassis Serial number
    reqdata[4] = 0x00; // 0:end of data
    reqdata[5] = strlen(serialnumber);
    safe_fun_ret =
        memcpy_s(&reqdata[6], sizeof(reqdata) - 6, serialnumber, strlen(serialnumber)); 
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        return FALSE;
    }

    
    result = ipmi_send_request(&req_msg_head, reqdata, MAX_POSSIBLE_IPMI_FRAME_LEN, &response, TRUE);
    if (result != RET_OK) {
        g_printf("Write serialnumber failed. %x\r\n", result);
        return FALSE;
    }

    
    resp_src_data = get_ipmi_src_data(response);
    if (resp_src_data == NULL) {
        g_free(response);
        g_printf("Write serialnumber failed.\r\n");
        return FALSE;
    }
    
    if ((resp_src_data[0]) != COMP_CODE_SUCCESS) {
        g_printf("Write serialnumber failed. %d\r\n", resp_src_data[0]);
        g_free(response);
        return FALSE;
    }
    g_free(response);
    response = NULL;

    req_msg_head.src_len = 3;
    reqdata[0] = 0x06; // 6:update elable data
    reqdata[1] = 0x01; // fru_id:1
    reqdata[2] = 0xaa; // 0xaa:initiate update
    
    result = ipmi_send_request(&req_msg_head, reqdata, MAX_POSSIBLE_IPMI_FRAME_LEN, &response, TRUE);
    if (result != RET_OK) {
        g_printf("Update serialnumber failed.\r\n");
        return FALSE;
    }

    
    resp_src_data = get_ipmi_src_data(response);
    if (resp_src_data == NULL) {
        g_free(response);
        g_printf("Update serialnumber failed.\r\n");
        return FALSE;
    }
    
    if ((resp_src_data[0]) != COMP_CODE_SUCCESS) {
        g_free(response);
        g_printf("Update serialnumber failed.\r\n");
        return FALSE;
    }
    g_free(response);

    g_printf("Success.\r\n");
    return TRUE;
}


LOCAL gint32 del_event_from_monitor_foreach(OBJ_HANDLE handle, gpointer user_data)
{
    gint32 result = 0;
    guint8 state = 0;
    gchar policy_name[32] = {0};
    gchar username[PARAMETER_LEN + 1] = { 0 };
    gchar ip[PARAMETER_LEN + 1] = { 0 };

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    (void)dal_get_property_value_string(handle, PROPERTY_EVENT_MONITOR_POLICY_OBJ, policy_name, sizeof(policy_name));

    
    (void)dal_get_property_value_byte(handle, PROPERTY_EVENT_MONITOR_STATE, &state);
    if ((state == 1) && (!strcmp(user_data, policy_name))) {
        result = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, username, ip, CLASS_EVENT_MONITOR, handle,
            METHOD_EVENT_MONITOR_DEL_EVENT_FROM_MONITOR, NULL, NULL);
        if (result != RET_OK) {
            return result;
        }
    }

    return RET_OK;
}

LOCAL void print_set_removed_event_severity_helpinfo(void)
{
    OBJ_HANDLE handle = 0;
    gint32 ret;

    ret = dal_get_specific_object_byte(CLASS_NAME_IPMBETH_BLADE, PROTERY_IPMBETH_BLADE_BLADETYPE,
                                       BLADE_TYPE_SWI_MODULE, &handle);
    if (ret != DFL_OK) {
        g_printf("Usage: ipmcset -l <smm|blade|pem|fantray> -d removedeventseverity -v <value>\r\n");
    } else {
        g_printf("Usage: ipmcset -l <smm|blade|swi|pem|fantray> -d removedeventseverity -v <value>\r\n");
    }
    g_printf("Values are:\r\n");
    g_printf("\t0\tNormal\r\n\t1\tMinor\r\n\t2\tMajor\r\n\t3\tCritical\r\n");
}


gint32 smm_set_removed_event_severity(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    errno_t safe_fun_ret = EOK;
    gint32 ret = 0;
    OBJ_HANDLE info_object_handle = 0;
    OBJ_HANDLE plcy_object_handle = 0;
    guint8 severity;
    guint8 old_severity = 0;
    guint8 deassert_flag;
    GSList *input_list = NULL;
    gchar policy_name[32] = {0};
    gchar username[PARAMETER_LEN + 1] = { 0 };
    gchar ip[PARAMETER_LEN + 1] = { 0 };

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 2) {
        print_set_removed_event_severity_helpinfo();
        return FALSE;
    } else if (strncmp("-v", argv[0], strlen(argv[0]))) {
        print_set_removed_event_severity_helpinfo();
        return FALSE;
    }

    
    if (strcmp("pem", target) == 0) {
        ret = dfl_get_object_handle(PS_REMOVEED_INFO, &info_object_handle) ||
            dfl_get_object_handle(PS_REMOVEED_POLICY, &plcy_object_handle);
        safe_fun_ret = memcpy_s(policy_name, sizeof(policy_name), PS_REMOVEED_POLICY, strlen(PS_REMOVEED_POLICY));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
    } else if (strcmp("fantray", target) == 0) {
        ret = dfl_get_object_handle(FAN_REMOVEED_INFO, &info_object_handle) ||
            dfl_get_object_handle(FAN_REMOVEED_POLICY, &plcy_object_handle);
        safe_fun_ret = memcpy_s(policy_name, sizeof(policy_name), FAN_REMOVEED_POLICY, strlen(FAN_REMOVEED_POLICY));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
    } else if (strcmp("blade", target) == 0) {
        ret = dfl_get_object_handle(BLADE_REMOVEED_INFO, &info_object_handle) ||
            dfl_get_object_handle(BLADE_REMOVEED_POLICY, &plcy_object_handle);
        safe_fun_ret = memcpy_s(policy_name, sizeof(policy_name), BLADE_REMOVEED_POLICY, strlen(BLADE_REMOVEED_POLICY));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
    } else if (strcmp("swi", target) == 0) {
        ret = dfl_get_object_handle(SWI_REMOVEED_INFO, &info_object_handle) ||
            dfl_get_object_handle(SWI_REMOVEED_POLICY, &plcy_object_handle);
        safe_fun_ret = memcpy_s(policy_name, sizeof(policy_name), SWI_REMOVEED_POLICY, strlen(SWI_REMOVEED_POLICY));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
    } else if (strcmp("smm", target) == 0) {
        ret = dfl_get_object_handle(SMM_REMOVEED_INFO, &info_object_handle) ||
            dfl_get_object_handle(SMM_REMOVEED_POLICY, &plcy_object_handle);
        safe_fun_ret = memcpy_s(policy_name, sizeof(policy_name), SMM_REMOVEED_POLICY, strlen(SMM_REMOVEED_POLICY));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
    } else {
        print_set_removed_event_severity_helpinfo();
        return FALSE;
    }
    

    if (ret != RET_OK) {
        print_set_removed_event_severity_helpinfo();
        return FALSE;
    }

    
    if ((strlen(argv[1]) != 1) || (argv[1][0] > '3') || (argv[1][0] < '0')) {
        print_set_removed_event_severity_helpinfo();
        return FALSE;
    }

    severity = argv[1][0] - '0';
    
    (void)dal_get_property_value_byte(info_object_handle, PROPERTY_EVENT_INFORMATION_SEVERITY, &old_severity);
    if ((severity == 0) && (old_severity != 0)) {
        ret = dfl_foreach_object(CLASS_EVENT_MONITOR, del_event_from_monitor_foreach, policy_name, NULL);
        if (ret != DFL_OK) {
            g_printf("Set event severity failed.\r\n");
            return FALSE;
        }
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    
    input_list = g_slist_append(input_list, g_variant_new_byte(severity));
    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, username, ip, CLASS_EVENT_INFORMATION, info_object_handle,
        METHOD_EVENT_INFORMATION_SET_SEVERITY, input_list, NULL);
    if (ret != RET_OK) {
        uip_free_gvariant_list(input_list);
        g_printf("Set event severity failed.\r\n");
        return FALSE;
    }
    uip_free_gvariant_list(input_list);
    input_list = NULL;

    
    deassert_flag = (severity == 0u) ? 0u : 1u;
    input_list = g_slist_append(input_list, g_variant_new_byte(deassert_flag));
    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, username, ip, CLASS_EVENT_POLICY, plcy_object_handle,
        METHOD_EVENT_POLICY_SET_POLICY_DEASSERT_FLAG, input_list, NULL);
    if (ret != RET_OK) {
        uip_free_gvariant_list(input_list);
        g_printf("Set event severity failed.\r\n");
        return FALSE;
    }
    uip_free_gvariant_list(input_list);
    input_list = NULL;

    g_printf("Success.\r\n");

    return TRUE;
}

LOCAL void print_fru_control_helpinfo(gchar *target)
{
    g_printf("Usage:\r\n");
    if (strncmp("blade", target, strlen("blade")) == 0) {
        g_printf("ipmcset -l bladeN -d frucontrol -v <value>\r\n");
    } else if (strncmp("swi", target, strlen("swi")) == 0) {
        g_printf("ipmcset -l swiN -d frucontrol -v <fruid> <value>\r\n");
        g_printf("fru id is 1~3.\r\n");
    }
    g_printf("Values are:\r\n");
    g_printf(
        "\t0\tForced system reset\r\n\t1\tWarm reset\r\n\t2\tForced power cycle\r\n\t3\tDiagnosis interruption\r\n");
}


gint32 smm_fru_control(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
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
    gint32 input;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (((argc != 2) && (argc != 3)) || strncmp("-v", argv[0], strlen(argv[0]))) {
        print_fru_control_helpinfo(target);
        return FALSE;
    }

    slot_id = get_blade_num(target);
    if (slot_id == 0xff) {
        print_fru_control_helpinfo(target);
        return FALSE;
    }

    if ((strncmp("blade", target, strlen("blade")) == 0) && (argc == 2)) {
        
        if ((strlen(argv[1]) != 1) || (argv[strlen(argv[1])][0] < '0') || (argv[strlen(argv[1])][0] > '3')) {
            print_fru_control_helpinfo(target);
            return FALSE;
        }
        operation = (guint8)(argv[1][0] - '0');
        fru_id = 0;
    } else if ((strncmp("swi", target, strlen("swi")) == 0) && (argc == 3)) {
        
        if ((strlen(argv[1]) != 1) || (argv[strlen(argv[1])][0] < '1') || (argv[strlen(argv[1])][0] > '3')) {
            print_fru_control_helpinfo(target);
            return FALSE;
        }
        fru_id = (guint8)(argv[1][0] - '0');

        
        if ((strlen(argv[2]) != 1) || (argv[2][0] < '0') || (argv[2][0] > '3')) {
            print_fru_control_helpinfo(target);
            return FALSE;
        }
        operation = (guint8)(argv[2][0] - '0');
    } else {
        print_fru_control_helpinfo(target);
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

    g_printf("WARNING: The operation may have many adverse effects.\r\n");
    input = ipmc_get_user_input(CLI_COMFIRM_STRING);
    if (input == 'n' || input == 'N') {
        return FALSE;
    }
    if (input != 'y' && input != 'Y') {
        g_printf("Input character invalid.\r\n");
        return FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(fru_id));
    input_list = g_slist_append(input_list, g_variant_new_byte(operation));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_NAME_IPMBETH_BLADE,
        (const gchar *)dfl_get_object_name(object_handle), METHOD_IPMBETH_BLADE_FRU_CONTROL, input_list, &output_list);
    if (ret != RET_OK) {
        g_printf("Set fru control failed.\r\n");
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
    
    if (ret_code == 2) {
        g_printf("The FRU or target does not support the function.\r\n");
        return FALSE;
    }

    g_printf("Success.\r\n");

    return TRUE;
}


gint32 smm_set_identify_led(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gchar username[PARAMETER_LEN + 1] = { 0 };
    gchar ip[PARAMETER_LEN + 1] = { 0 };
    GSList *input_list = NULL;
    gint32 ret = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }
    
    if (argc != 2) {  // 参数数目不等于2
        goto PARAM_ERR_OUT;
    }
    
    if (strcmp(argv[0], "-v")) {
        goto PARAM_ERR_OUT;
    }
    if (strcmp(argv[1], "0") == 0) {
        
        input_list = g_slist_append(input_list, g_variant_new_byte(TURN_OFF_IDENTIFY_LED));
        input_list = g_slist_append(input_list, g_variant_new_byte(0));
    } else if (strcmp(argv[1], "1") == 0) {
        
        input_list = g_slist_append(input_list, g_variant_new_byte(TURN_ON_IDENTIFY_LED));
        input_list = g_slist_append(input_list, g_variant_new_byte(0));
    } else {
        
        goto PARAM_ERR_OUT;
    }
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, LED_CLASS_NAME, UID_LED,
        METHOD_UIDLED_STATE, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        g_printf("Identify UID led failed.\r\n");
        return FALSE;
    }
    g_printf("Identify UID led successfully.\r\n");
    return TRUE;
PARAM_ERR_OUT:
    
    g_printf("Usage: ipmcset %s-d identify -v value(0 or 1)\r\n", help_default_name);
    g_printf("       0 means to turn off identify led\r\n");
    g_printf("       1 means to turn on identify led\r\n");
    return FALSE;
}

gint32 smm_set_shelf_management_state(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    gchar username[PARAMETER_LEN + 1] = { 0 };
    gchar ip[PARAMETER_LEN + 1] = { 0 };
    GSList *input_list = NULL;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    gint32 status = -1;
    if (argc != 2 || strcmp("-v", argv[0]) != 0 || dal_strtoi(argv[1], &status, NUMBER_BASE10) != RET_OK
        || (status != 0 && status != 1)) {
        g_printf("Usage: ipmcset %s-d shelfmanagementstate -v <1/0>\r\n", help_default_name);
        return FALSE;
    }

    g_printf("This operation will set shelf management state to %s.\r\n", status == 0 ? "OFF" : "ON");
    gint32 input = ipmc_get_user_input("Continue?[Y/N]:");
    if (input == 'n' || input == 'N') {
        return FALSE;
    }
    if (((input != 'y') && (input != 'Y'))) {
        g_printf("Input character invalid.\r\n");
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)status));

    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_NAME_SHELF_MANAGE,
        OBJ_NAME_SHELF_MANAGE, METHOD_SHELF_SET_SHELF_MANAGEMENT_STATE, input_list, NULL);
    if (ret != RET_OK) {
        g_printf("Set shelf management state failed.\r\n");
        uip_free_gvariant_list(input_list);
        return RET_ERR;
    }

    g_printf("Set shelf management state successfully.\r\n");
    uip_free_gvariant_list(input_list);

    return RET_OK;
}

gint32 smm_set_stack_state(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
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
        print_set_stack_state_usage();
        return FALSE;
    }
    if (strlen(argv[1]) == 0) {
        print_set_stack_state_usage();
        return FALSE;
    }

    if (strcmp(argv[1], "enable") == 0) {
        option = 1;
    } else if (strcmp(argv[1], "disable") == 0) {
        option = 0;
    } else {
        g_printf("Set fail:parameter error.\r\n");
        print_set_stack_state_usage();
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
        obj_name, METHOD_SET_CASCADE_PORT_STATE, input_list, NULL);
    if (ret != RET_OK) {
        g_printf("Set fail:internal error.\r\n");
        uip_free_gvariant_list(input_list);
        return FALSE;
    }

    g_printf("%s stack state successfully.\r\n", (option ? "Enable" : "Disable"));
    uip_free_gvariant_list(input_list);
    return TRUE;
}

gint32 smm_shelf_power_control(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    GSList *input_list = NULL;
    gint32 input;
    guint8 control;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        g_printf("Permission denied.\r\n");
        return FALSE;
    }

    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        print_shelf_power_control_usage();
        return FALSE;
    }

    control = (guint8)str_to_ul(argv[1]);
    if ((control != 0) && (control != 1)) {
        print_shelf_power_control_usage();
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    input = ipmc_get_user_input("This operation will make shelf power state change?[Y/N]:");
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

    input_list = g_slist_append(input_list, g_variant_new_byte(control));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_SHELF_PAYLOAD,
        CLASS_SHELF_PAYLOAD, METHOD_SHELF_POWER_CONTROL, input_list, NULL);
    uip_free_gvariant_list(input_list);
    g_printf("Shelf power %s %s.\r\n", control ? "on" : "off", ret == RET_OK ? "successfully" : "failed");
    return ret;
}

gint32 smm_set_power_button_mode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    GSList *input_list = NULL;
    guint8 button_mode;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        g_printf("Permission denied.\r\n");
        return RET_ERR;
    }

    if ((argc != 2) || (strcmp(argv[0], "-v"))) { 
        print_power_button_mode_usage();
        return RET_ERR;
    }

    button_mode = (guint8)str_to_ul(argv[1]);
    if (button_mode != 0) {
        print_power_button_mode_usage();
        return RET_ERR;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(button_mode));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_SHELF_PAYLOAD,
        CLASS_SHELF_PAYLOAD, METHOD_SHELF_PAYLOAD_SET_BUTTON_MODE, input_list, NULL);
    uip_free_gvariant_list(input_list);
    g_printf("Shelf power button mode switch to bmc %s.\r\n", ret == RET_OK ? "successfully" : "failed");
    return ret;
}
