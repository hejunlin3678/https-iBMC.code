

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
#include "ipmcset_service_mgmt.h"
#include "ipmcset_sys_mgmt.h"
#include "ipmcset_netconfig.h"
#include "ipmcset_user_security.h"
#include "ipmcset_alarm.h"


gint32 ipmc_add_public_key(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    errno_t safe_fun_ret = EOK;
    gchar file_info[MAX_FILEPATH_LENGTH + 1] = {0};
    guchar user_id = 0;
    gint32 ret;
    
    
    if (argc != 3 || strcmp(argv[0], "-v") != 0) {
        print_add_pulickey_usage();
        return FALSE;
    }

    
    if (setuser_privilege_judge(privilege, rolepriv, argv[1], &user_id) != TRUE) {
        ipmc_log_operation_log("User", "Add user public key failed");
        return FALSE;
    }

    
    if ((strstr(argv[2], FILE_URL_HTTPS_HEAD) == NULL) && (strstr(argv[2], FILE_URL_SFTP_HEAD) == NULL) &&
        (strstr(argv[2], FILE_URL_NFS_HEAD) == NULL) && (strstr(argv[2], FILE_URL_CIFS_HEAD) == NULL) &&
        (strstr(argv[2], FILE_URL_SCP_HEAD) == NULL)) {
        
        if (strncmp(argv[2], "/", strlen("/")) != 0 || dal_check_real_path(argv[2]) != RET_OK) {
            g_printf("File path is invalid.\r\n");
            print_add_pulickey_usage();
            return FALSE;
        }

        if (strlen(argv[2]) > MAX_FILEPATH_LENGTH) {
            g_printf("File path is too long.\r\n");
            print_add_pulickey_usage();
            return FALSE;
        }

        safe_fun_ret = strncpy_s(file_info, MAX_FILEPATH_LENGTH + 1, argv[2], MAX_FILEPATH_LENGTH);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        
    } else {
        ret = get_file_path_from_url(argv[2], PUBLICK_KEY_FILE_ID, file_info, sizeof(file_info));
        clear_sensitive_info(argv[2], strlen(argv[2]));
        if (ret != TRUE) {
            g_printf("Add user public key failed.\r\n");
            return FALSE;
        }
    }

    if (set_user_public_key(user_id, USER_PUBLIC_KEY_TYPE_FILE, file_info) != RET_OK) {
        g_printf("Add user public key failed.\r\n");
        return FALSE;
    }
    

    g_printf("Add user public key successfully.\r\n");
    return TRUE;
}


gint32 ipmc_set_service_port(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    guint8 service_pos = 0;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if ((argc < 3) || (argc > 4) || (strcmp(argv[0], "-v")) ||
        ((argc == 4) && (strcasecmp(argv[1], "rmcp")))) {   // 只有rmcp才能配置2个port
        print_port_cmd_helpinfo();
        return FALSE;
    }

    gint32 ret = ipmc_match_opt(argv[1], get_g_bmc_cmd_service_port(), &service_pos);
    if (ret != RET_OK) {
        print_port_cmd_helpinfo();
        return FALSE;
    }

    input_list = ipmc_create_inputlist(argc, argv);
    if (input_list == NULL) {
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    
    struct service_method *service = &(get_g_bmc_cmd_service_port()[service_pos]);
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, service->ClassName,
        service->ObjName, service->Method, input_list, NULL);
    uip_free_gvariant_list(input_list);

    
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "call method[%s.%s] failed, ret is %d", service->ObjName, service->Method, ret);
        g_printf("Set service port failed.\r\n");
        return FALSE;
    }
    
    gint32 port = -1;
    if (dal_strtoi(argv[2], &port, NUMBER_BASE10) != RET_OK) {
        debug_log(DLOG_ERROR, "dal_strtoi failed");
    }

    if ((strcasecmp(argv[1], "rmcp") == 0) && (argc == 4)) {
        gint32 ex_port = -1;    // rmcp设置了2个端口
        if (dal_strtoi(argv[3], &ex_port, NUMBER_BASE10) != RET_OK) {
            debug_log(DLOG_ERROR, "dal_strtoi failed");
        }
        g_printf("Set %s service port to %d,%d successfully.\r\n", argv[1], port, ex_port);
    } else {
        g_printf("Set %s service port to %d successfully.\r\n", argv[1], port);
    }

    return TRUE;
}


gint32 ipmc_set_service_state(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    guint8 state = 0;
    guint8 mode = 0;
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    guint8 service_pos = 0;
    struct service_method *service = NULL;
    OBJ_HANDLE obj_handle = 0;
    guint8 mutual_state = 0;
    guint8 board_type = 0;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 3 || strcmp(argv[0], "-v")) {
        print_state_cmd_helpinfo();
        return FALSE;
    }
    if (ipmc_find_state_cmd(argv[1], &service_pos) != RET_OK) {
        print_state_cmd_helpinfo();
        return FALSE;
    }

    if (strcmp(argv[2], "enabled") == 0) {
        state = 1;

        
        // 检测是否是打开非安全协议，则打印提示信息，需要用户确认是否打开
        ret = set_service_print_warning(argv[1]);
        if (ret != RET_OK) {
            return ret;
        }
        

        
        if (strcasecmp(argv[1], "ssh") == 0) {
            (void)dal_get_object_handle_nth(MUTUAL_AUTH_CLASS_NAME, 0, &obj_handle);
            (void)dal_get_property_value_byte(obj_handle, MUTUAL_AUTH_STATE, &mutual_state);

            if (mutual_state == 1) {
                g_printf("Set %s service state(%s) failed.\r\nSSH and two-factor authentication cannot be enabled at "
                    "the same time.\r\n",
                    argv[1], argv[2]);
                return FALSE;
            }
        }

        
    } else if (strcmp(argv[2], "disabled") == 0) {
        state = 0;
    } else {
        print_state_cmd_helpinfo();
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    
    (void)dal_rf_get_board_type(&board_type);

    // 如果是后插板只剩下一个HTTPS服务，就算关闭，可以通过SOL访问，不提示此消息
    if (board_type != BOARD_SWITCH) {
        
        ret = ipmc_judge_service_state(get_g_bmc_cmd_service_state()[service_pos].ServiceIndex, state);
        if (ret != RET_OK) {
            return FALSE;
        }
    }

    

    
    if (strcasecmp(argv[1], "rmcp") == 0) {
        input_list = g_slist_append(input_list, g_variant_new_byte(IPMI_LAN_RMCP));
    } else if (strcasecmp(argv[1], "rmcp+") == 0) {
        input_list = g_slist_append(input_list, g_variant_new_byte(IPMI_LAN_RMCPPLUS));
    }

    

    if (strcasecmp(argv[1], "ssdp") == 0) {
        (void)dal_get_object_handle_nth(CLASS_SSDP_CONFIG, 0, &obj_handle);
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_SSDP_CONFIG_MODE, &mode);

        if (state == SERVICE_STATE_ENABLE) {
            state = mode | (SERVICE_STATE_ENABLE << SSDP_RECEIVE_BIT);
        } else {
            
            state = mode & ((SERVICE_STATE_DISABLE << SSDP_RECEIVE_BIT) + 1);
        }
    }

    
    service = &(get_g_bmc_cmd_service_state()[service_pos]);
    input_list = g_slist_append(input_list, g_variant_new_byte(state));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, service->ClassName,
        service->ObjName, service->Method, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        g_printf("Set %s service state(%s) failed.\r\n", argv[1], argv[2]);
        return FALSE;
    }

    g_printf("Set %s service state(%s) successfully.\r\n", argv[1], argv[2]);
    (void)set_rmcp_server(argv[1], (const gchar *)username, (const gchar *)ip);

    return TRUE;
}



gint32 ipmc_set_snmp_privacy_password(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 iRet;
    errno_t safe_fun_ret;
    guchar user_id = 0;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    gint32 ret;
    gchar obj_name[OBJ_NAME_MAX_LEN] = {0};
    gchar user_pwd[IPMC_USER_PASSWORD_LEN] = {0};
    gchar user_name[IPMC_USER_NAME_MAX_LEN] = {0};

    
    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        print_set_snmp_privacy_help();
        ipmc_log_operation_log("User", "Set snmp privacy password failed");
        return FALSE;
    }

    if (strlen(argv[1]) >= IPMC_USER_NAME_MAX_LEN) {
        print_set_snmp_privacy_help();
        ipmc_log_operation_log("User", "Set snmp privacy password failed");
        return FALSE;
    }

    
    safe_fun_ret = strncpy_s(user_name, sizeof(user_name), argv[1], sizeof(user_name) - 1);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }

    if (setuser_privilege_judge(privilege, rolepriv, user_name, &user_id) == FALSE) {
        ipmc_log_operation_log("User", "Set snmp privacy password failed");
        return FALSE;
    }

    
    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    if (ipmc_get_user_password(user_pwd, sizeof(user_pwd)) != RET_OK) {
        ipmc_log_operation_log("User", "Set snmp privacy password failed");
        return FALSE;
    }

    
    input_list = g_slist_append(input_list, g_variant_new_byte(user_id));
    input_list = g_slist_append(input_list, g_variant_new_string(user_pwd));
    iRet = snprintf_s(obj_name, OBJ_NAME_MAX_LEN, OBJ_NAME_MAX_LEN - 1, "%s%u", CLASS_USER, user_id);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_USER, obj_name,
        METHOD_USER_SETSNMPPRIVACYPASSWORD, input_list, &output_list);
    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);
    if (ret == RET_OK) {
        
        if (strlen(user_pwd) < SNMPV3_PASSWORD_MIN_LENGTH) {
            g_printf(
                "The password is shorter than eight characters, and the user cannot use the SNMPv3 interface.\r\n");
        }
        
        g_printf("Set snmp privacy password successfully.\r\n");
    } else {
        
        if ((ret == USER_PASS_COMPLEXITY_FAIL) || (ret == USER_SET_PASSWORD_TOO_WEAK)) {
            print_password_complexity_usage();
        }

        
        g_printf("Set snmp privacy password failed.\r\n");
    }

    dal_clear_sensitive_info(user_pwd, sizeof(user_pwd));

    return ret;
}



gint32 ipmc_set_ssh_password_authentication(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target)
{
    guint8 enable_state = 0;
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    
    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        ipmc_log_operation_log("SecurityEnhance", "Insufficient privilege");
        return FALSE;
    }

    
    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        g_printf("Usage: ipmcset -d sshpasswordauthentication -v <enabled|disabled>\r\n");
        return FALSE;
    }

    if (!strcmp(argv[1], "enabled")) {
        enable_state = 1;
    } else if (!strcmp(argv[1], "disabled")) {
        enable_state = 0;
    } else {
        g_printf("Usage: ipmcset -d sshpasswordauthentication -v <enabled|disabled>\r\n");
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    
    input_list = g_slist_append(input_list, g_variant_new_byte(enable_state));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_SECURITY_ENHANCE,
        CLASS_SECURITY_ENHANCE, METHOD_SECURITY_ENHANCE_SETSSHPASSWORDAUTHENTICATION, input_list, &output_list);
    uip_free_gvariant_list(input_list);

    if (ret != RET_OK) {
        g_printf("Set SSH password authentication failed.\r\n");
        return FALSE;
    }

    ret = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
    if (ret != RET_OK) {
        uip_free_gvariant_list(output_list);
        g_printf("Set SSH password authentication failed!\r\n");
        return FALSE;
    }

    uip_free_gvariant_list(output_list);
    g_printf("Set SSH password authentication successfully.\r\n");
    return TRUE;
}



gint32 ipmc_set_trap_mode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    const gchar *p_str_buff = NULL;
    gint32 ret;
    GSList *input_list = NULL;
    guchar l_uiValue = 0;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        ipmc_log_operation_log(CLASS_TRAP_CONFIG, "Set SNMP trap mode failed");
        return FALSE;
    }

    if ((argc != 2) || ((argv[0]) && strcmp(argv[0], "-v"))) {
        _ipmc_print_trap_mode_usage();
        return FALSE;
    }

    if (argv[1] == NULL) {
        _ipmc_print_trap_mode_usage();
        return FALSE;
    }

    if (strcmp(argv[1], "0") == 0) {
        l_uiValue = TRAP_MODE_EVT;
        p_str_buff = TRAP_MODE_EVT_STR;
    } else if (strcmp(argv[1], "1") == 0) {
        l_uiValue = TRAP_MODE_OID;
        p_str_buff = TRAP_MODE_OID_STR;
    } else if (strcmp(argv[1], "2") == 0) {
        l_uiValue = TRAP_MODE_ACC;
        p_str_buff = TRAP_MODE_ACC_STR;
    } else {
        ipmc_log_operation_log(CLASS_TRAP_CONFIG, "Set SNMP trap mode unknown");
        _ipmc_print_trap_mode_usage();
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        (void)snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        (void)snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(l_uiValue));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_TRAP_CONFIG,
        OBJECT_TRAP_CONFIG, METHOD_TRAP_SET_MODE, input_list, NULL);
    if (ret != RET_OK) {
        g_printf("Set trap %s failed.\r\n", p_str_buff);
        uip_free_gvariant_list(input_list);
        return FALSE;
    }

    g_printf("Set trap %s successfully.\r\n", p_str_buff);
    uip_free_gvariant_list(input_list);
    return TRUE;
}



gint32 ipmc_set_trap_severity(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    GSList *input_list = NULL;
    gint32 ret;
    guint8 trapseverity = 0;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        ipmc_log_operation_log(CLASS_TRAP_CONFIG, "Set SNMP trap severity failed.");
        return FALSE;
    }

    ret = argc < 2 || argc > 5 || strcmp(argv[0], "-v");
    if (ret) {
        print_trapseverity_usage();
        return FALSE;
    }

    ret = get_severity_value(argc, argv, &trapseverity);
    if (ret != RET_OK) {
        print_trapseverity_usage();
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(trapseverity));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_TRAP_CONFIG,
        OBJECT_TRAP_CONFIG, METHOD_TRAP_SET_SEND_SEVERITY, input_list, NULL);
    if (ret != RET_OK) {
        g_printf("Set trap severity failed.\r\n");
        uip_free_gvariant_list(input_list);
        return FALSE;
    }

    g_printf("Set trap severity successfully.\r\n");
    uip_free_gvariant_list(input_list);
    return TRUE;
}

LOCAL gint32 ipmc_set_trap_user_check_input(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv)
{
    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        ipmc_log_operation_log(CLASS_USER, "Set trap user failed.");
        return FALSE;
    }

    if ((argc != 2) || ((argv[0]) && strcmp(argv[0], "-v"))) {
        g_printf("Usage: ipmcset -t trap -d user -v <username>\r\n");
        return FALSE;
    }

    if (argv[1] == NULL) {
        g_printf("Usage: ipmcset -t trap -d user -v <username>\r\n");
        return FALSE;
    }

    
    if (strlen(argv[1]) == 0) {
        g_printf("User %s doesn't exist.\r\n", argv[1]);
        return FALSE;
    }
    return TRUE;
}

gint32 ipmc_set_trap_user(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    guint8 user_id;
    GVariant *property_value = NULL;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    ret = ipmc_set_trap_user_check_input(privilege, rolepriv, argc, argv);
    if (ret != TRUE) {
        return ret;
    }

    

    property_value = g_variant_new_string(argv[1]);
    ret = dfl_get_specific_object(CLASS_USER, PROPERTY_USER_NAME, property_value, &obj_handle);
    g_variant_unref(property_value);
    property_value = NULL;

    if (ret != DFL_OK) {
        if (ret == ERRCODE_OBJECT_NOT_EXIST) {
            
            g_printf("User doesn't exist.\r\n");
            
            return FALSE;
        }

        g_printf("Get object handle failed.\r\n ");
        return FALSE;
    }

    ret = uip_get_object_property(CLASS_USER, dfl_get_object_name(obj_handle), PROPERTY_USER_ID, &property_value);
    if (ret != RET_OK) {
        g_printf("Get user id failed.\r\n ");
        return FALSE;
    }

    user_id = g_variant_get_byte(property_value);
    g_variant_unref(property_value);

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(user_id));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_TRAP_CONFIG,
        OBJECT_TRAP_CONFIG, METHOD_TRAP_SET_TRAPV3_USERID, input_list, NULL);
    uip_free_gvariant_list(input_list);

    if (ret != RET_OK) {
        
        g_printf("Set trap username failed.\r\n");
        
        return FALSE;
    }

    
    g_printf("Set trap username successfully.\r\n");
    

    return TRUE;
}

LOCAL gint32 ipmc_set_trap_version_check_input(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv)
{
    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        ipmc_log_operation_log(CLASS_TRAP_CONFIG, "Set SNMP trap version failed.");
        return FALSE;
    }

    if ((argc != 2) || ((argv[0]) && strcmp(argv[0], "-v"))) {
        g_printf("Usage: ipmcset -t trap -d version -v <V1|V2C|V3>\r\n");
        return FALSE;
    }

    if (argv[1] == NULL) {
        g_printf("Usage: ipmcset -t trap -d version -v <V1|V2C|V3>\r\n");
        return FALSE;
    }
    return TRUE;
}

gint32 ipmc_set_trap_version(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    errno_t safe_fun_ret = EOK;
    gchar str_buff[TRAP_VERSION_LEN] = {0};
    gint32 ret;
    GSList *input_list = NULL;
    guchar l_uiValue = 0;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    ret = ipmc_set_trap_version_check_input(privilege, rolepriv, argc, argv);
    if (ret != TRUE) {
        return ret;
    }
    memset_s(str_buff, TRAP_VERSION_LEN, 0x00, TRAP_VERSION_LEN);

    if (strcasecmp(argv[1], "V1") == 0) {
        l_uiValue = TRAP_VER_V1;
        safe_fun_ret = memcpy_s(str_buff, TRAP_VERSION_LEN, (gchar *)TRAP_VER_V1_STR, sizeof(TRAP_VER_V1_STR));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
    } else if (strcasecmp(argv[1], "V2C") == 0) {
        l_uiValue = TRAP_VER_V2C;
        safe_fun_ret = memcpy_s(str_buff, TRAP_VERSION_LEN, (gchar *)TRAP_VER_V2C_STR, sizeof(TRAP_VER_V2C_STR));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
    } else if (strcasecmp(argv[1], "V3") == 0) {
        l_uiValue = TRAP_VER_V3;
        safe_fun_ret = memcpy_s(str_buff, TRAP_VERSION_LEN, (gchar *)TRAP_VER_V3_STR, sizeof(TRAP_VER_V3_STR));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
    } else {
        g_printf("Usage: ipmcset -t trap -d version -v <V1|V2C|V3>\r\n");
        ipmc_log_operation_log(CLASS_TRAP_CONFIG, "Set SNMP trap version failed.");
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(l_uiValue));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_TRAP_CONFIG,
        OBJECT_TRAP_CONFIG, METHOD_TRAP_SET_VERSION, input_list, NULL);
    if (ret != RET_OK) {
        g_printf("Set trap %s failed.\r\n", str_buff);
        uip_free_gvariant_list(input_list);
        return FALSE;
    }

    g_printf("Set trap %s successfully.\r\n", str_buff);
    uip_free_gvariant_list(input_list);
    return TRUE;
}


gint32 ipmc_set_trapcommunity(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    gint32 ret;
    GSList *input_list = NULL;
    gchar community[PARAMETER_LEN + 1] = {0};

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        ipmc_log_operation_log(CLASS_TRAP_CONFIG, "Set SNMP trap community failed.");
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcset -t trap -d community\r\n");
        return FALSE;
    }

    if (ipmc_get_trapcommunity(community, sizeof(community)) != RET_OK) {
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    input_list = g_slist_append(input_list, g_variant_new_string(community));
    clear_sensitive_info(community, sizeof(community));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_TRAP_CONFIG,
        OBJECT_TRAP_CONFIG, METHOD_TRAP_SET_COMMUNITY_NAME, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        if (ret == USER_PASS_COMPLEXITY_FAIL) {
            print_trapcommunity_plexity_usage();
        }

        g_printf("Set SNMP trap community failed.\r\n");
        return FALSE;
    }

    g_printf("Set SNMP trap community successfully.\r\n");
    return TRUE;
}

LOCAL gint32 trapdestaddress_check_input(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv)
{
    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        ipmc_log_operation_log(CLASS_TRAP_CONFIG, "Set SNMP trap dest address failed.");
        return FALSE;
    }
    
    if ((argc != 3) || ((argv[0]) && strcmp(argv[0], "-v"))) {
        print_trap_dest_address();
        return FALSE;
    }

    gint32 dest = -1;
    if (dal_strtoi(argv[1], &dest, NUMBER_BASE10) != RET_OK || dest < 1 || dest > 4) {
        print_trap_dest_address();
        return FALSE;
    }

    return TRUE;
}

gint32 ipmc_set_trapdestaddress(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    gchar obj_name[OBJ_NAME_MAX_LEN] = {0};
    GSList *input_list = NULL;
    gchar oper_log[MAX_OPERATE_LOG_LEN + 1] = {0};

    if (trapdestaddress_check_input(privilege, rolepriv, argc, argv) == FALSE) {
        debug_log(DLOG_ERROR, "%s: call trapdestaddress_check_input fail", __FUNCTION__);
        return FALSE;
    }
    

    gint32 dest = -1;
    if (dal_strtoi(argv[1], &dest, NUMBER_BASE10) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_strtoi failed", __FUNCTION__);
        return FALSE;
    }

    if (strlen(argv[2]) != 0) {
        
        ret = dal_check_net_addr_valid(argv[2], NULL);
        if (ret != NET_ADDR_OK) {
            g_printf("Input parameter format invalid.\r\n");
            (void)snprintf_s(oper_log, MAX_OPERATE_LOG_LEN + 1, MAX_OPERATE_LOG_LEN,
                "Set SNMP trap dest%d address failed.", dest);
            ipmc_log_operation_log(CLASS_TRAP_CONFIG, oper_log);
            
            print_trap_dest_address();
            
            return FALSE;
        }
        
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    input_list = g_slist_append(input_list, g_variant_new_string(argv[2]));
    
    ret = snprintf_s(obj_name, OBJ_NAME_MAX_LEN, OBJ_NAME_MAX_LEN - 1, "%s_%d", CLASS_TRAP_ITEM_CONFIG, dest - 1);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
    
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_TRAP_ITEM_CONFIG,
        obj_name, METHOD_TRAP_ITEM_SET_IP_ADDR, input_list, NULL);
    if (ret != RET_OK) {
        g_printf("Set trap dest%d address failed.\r\n", dest);
        uip_free_gvariant_list(input_list);
        return FALSE;
    }

    g_printf("Set trap dest%d address successfully.\r\n", dest);
    uip_free_gvariant_list(input_list);
    return TRUE;
}


gint32 ipmc_set_trapenable(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gchar str_state[TRAP_STATE_LEN] = {0};
    gint32 dest = 0;
    guchar l_uiValue = 0;
    guint32 ret;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        ipmc_log_operation_log(CLASS_TRAP_CONFIG, "Set SNMP trap enable failed.");
        return FALSE;
    }
    
    if (((argc != 2) && (argc != 3)) || ((argv[0]) && strcmp(argv[0], "-v")) || argv[1] == NULL) {
        print_trap_dest_state();
        return FALSE;
    }
    

    if (argc == 2) {
        
        (void)strncpy_s(str_state, TRAP_STATE_LEN, argv[1], TRAP_STATE_LEN - 1);
        
    } else {
        if (dal_strtoi(argv[1], &dest, NUMBER_BASE10) != RET_OK || dest > 4 || dest < 1) {
            print_trap_dest_state();
            return FALSE;
        }

        
        (void)strncpy_s(str_state, TRAP_STATE_LEN, argv[2], TRAP_STATE_LEN - 1);
        
    }

    if (strcmp(str_state, "disabled") == 0) {
        l_uiValue = 0;
    } else if (strcmp(str_state, "enabled") == 0) {
        l_uiValue = 1;
    } else {
        
        print_trap_dest_state();
        
        return FALSE;
    }

    if (argc == 2) {
        ret = set_trap_state(l_uiValue, str_state);
    } else {
        ret = set_trap_dest_state((guchar)dest, l_uiValue, str_state);
    }

    return ret;
}

LOCAL gint32 trapport_check_input(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv)
{
    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        ipmc_log_operation_log(CLASS_TRAP_CONFIG, "Set trap dest port failed");
        return FALSE;
    }
    
    if ((argc != 3) || (strcmp(argv[0], "-v"))) {
        print_trap_dest_port();
        return FALSE;
    }
    if (((argv[1] != NULL) && (index(argv[1], '.') != NULL)) || ((argv[2] != NULL) && (index(argv[2], '.') != NULL))) {
        print_trap_dest_port();
        return FALSE;
    }
    return TRUE;
}

gint32 ipmc_set_trapport(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gchar obj_name[OBJ_NAME_MAX_LEN] = {0};
    GSList *input_list = NULL;
    guint32 l_uiTrapPort;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    gchar *end = NULL;
    gchar oper_log[MAX_OPERATE_LOG_LEN + 1] = {0};

    if (trapport_check_input(privilege, rolepriv, argc, argv) == FALSE) {
        debug_log(DLOG_ERROR, "%s: call trapport_check_input fail", __FUNCTION__);
        return FALSE;
    }
    

    gint32 dest = -1;
    if (dal_strtoi(argv[1], &dest, NUMBER_BASE10) != RET_OK || dest < 1 || dest > 4) {
        print_trap_dest_port();
        return FALSE;
    }

    l_uiTrapPort = strtol(argv[2], &end, 10);
    if ((l_uiTrapPort <= 0) || (l_uiTrapPort > 65535) || (*end != '\0')) {
        (void)snprintf_s(oper_log, MAX_OPERATE_LOG_LEN + 1, MAX_OPERATE_LOG_LEN,
            "Set trap dest%d port(%u) failed", dest, l_uiTrapPort);
        ipmc_log_operation_log(CLASS_TRAP_CONFIG, oper_log);
        
        print_trap_dest_port();
        
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    input_list = g_slist_append(input_list, g_variant_new_uint32(l_uiTrapPort));
    
    gint32 ret =
        snprintf_s(obj_name, OBJ_NAME_MAX_LEN, OBJ_NAME_MAX_LEN - 1, "%s_%d", CLASS_TRAP_ITEM_CONFIG, dest - 1);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
    
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_TRAP_ITEM_CONFIG,
        obj_name, METHOD_TRAP_ITEM_SET_IP_PORT, input_list, NULL);
    if (ret != RET_OK) {
        g_printf("Set trap dest%d port failed.\r\n", dest);
        uip_free_gvariant_list(input_list);
        return FALSE;
    }

    g_printf("Set trap dest%d port successfully.\r\n", dest);
    uip_free_gvariant_list(input_list);
    return TRUE;
}


gint32 ipmc_set_vmm_connect(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    GSList *input_list = NULL;
    guint16 i;
    guchar state = 0;
    OBJ_HANDLE obj_handle = 0;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        print_set_vmm_connect();
        return FALSE;
    }

    if (strlen(argv[1]) > MAX_FILEPATH_LENGTH - 1) {
        print_set_vmm_connect();
        return FALSE;
    }

    
    if ((argv[1] != strstr(argv[1], "nfs")) && (argv[1] != strstr(argv[1], "cifs")) &&
        (argv[1] != strstr(argv[1], "https"))) {
        print_set_vmm_connect();
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    
    g_printf("Connect virtual media...\r\n");

    
    input_list = g_slist_append(input_list, g_variant_new_string(argv[1]));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, VMM_CLASS_NAME,
        VMM_OBJ_NAME, VMM_METHOD_SET_CONNECT, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        g_printf("Connect virtual media failed.\r\n");
        return FALSE;
    }

    if (dal_get_object_handle_nth(VMM_CLASS_NAME, 0, &obj_handle) != RET_OK) {
        g_printf("Unknown error.\r\n");
        return FALSE;
    }

    for (i = 0; i < 600; i++) {
        (void)vos_task_delay(1000);

        ret = dal_get_property_value_byte(obj_handle, VMM_PROPERTY_VMM_CONNECT_STATUS, &state);
        if (ret != RET_OK) {
            g_printf("\r\nConnect virtual media failed.\r\n");
            return FALSE;
        }

        if (state == VMM_CLIENT_HTTPS_SSL_VERIFY_FAILED) {
            g_printf("\r\nVerify peer's certificate or fingerprint failed.\r\n");
            return FALSE;
        }

        if (state == VMM_CLIENT_ON_CONNECTION) {
            g_printf(".");
            (void)fflush(stdout);
            continue;
        }

        if (state == VMM_CLIENT_CONNECT_SUCCESSFULLY) {
            g_printf("\r\nConnect virtual media successfully.\r\n");
            return TRUE;
        } else if (state == VMM_CLIENT_ALREADY_CONNECTED) {
            g_printf("\r\nYou cannot use the virtual media because it is being used by another user.\r\n");
            return FALSE;
        } else {
            g_printf("\r\nConnect virtual media failed.\r\n");
            return FALSE;
        }
    }

    return TRUE;
}

gint32 ipmc_set_vmm_disconnect(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle;
    gint32 num = 0;
    gint32 i;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        print_set_vmm_disconnect();
        return FALSE;
    }
    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    
    g_printf("Disconnect virtual media...\r\n");

    
    if (dal_get_object_handle_nth(VMM_CLASS_NAME, 0, &obj_handle) != RET_OK) {
        g_printf("Unknown error.\r\n");
        return FALSE;
    }

    
    (void)dal_get_property_value_int32(obj_handle, VMM_PROPERTY_NUM, &num);
    if (num == 0) {
        g_printf("Disconnect virtual media failed.\r\n");
        return FALSE;
    }

    
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, VMM_CLASS_NAME,
        VMM_OBJ_NAME, VMM_METHOD_DELECT_LINK, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        g_printf("Disconnect virtual media failed.\r\n");
        return FALSE;
    }

    for (i = 0; i < 5; i++) {
        (void)vos_task_delay(1000);
        g_printf(".");
        (void)fflush(stdout);
    }

    for (i = 0; i < 60; i++) {
        (void)vos_task_delay(1000);
        (void)dal_get_property_value_int32(obj_handle, VMM_PROPERTY_NUM, &num);

        if (num == 0) {
            g_printf("\r\nDisconnect virtual media successfully.\r\n");
            return TRUE;
        }

        g_printf(".");
        (void)fflush(stdout);
    }

    g_printf("\r\nDisconnect virtual media failed.\r\n");

    return TRUE;
}


gint32 ipmc_set_vnc_pwd(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    gchar vnc_key[IPMC_USER_PASSWORD_LEN] = {0};
    gchar *key = NULL;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return RET_ERR;
    }

    
    if (argc != 0) {
        g_printf("Usage: ipmcset -t vnc -d password\r\n");
        return RET_ERR;
    }

    

    if (ipmc_check_user_passwd() != RET_OK) {
        ipmc_log_operation_log("VNC", "Check user password failed");
        return RET_ERR;
    }

    if ((key = getPasswd("New VNC Password:")) == NULL) {
        g_printf("Incorrect password.\r\n");
        return RET_ERR;
    }
    
    (void)strncpy_s(vnc_key, IPMC_USER_PASSWORD_LEN, key, IPMC_USER_PASSWORD_LEN - 1);
    

    clear_sensitive_info(key, IPMC_USER_PASSWORD_LEN);

    if ((key = getPasswd("Confirm VNC Password:")) == NULL) {
        clear_sensitive_info(vnc_key, sizeof(vnc_key));
        g_printf("Confirm password error.\r\n");
        return RET_ERR;
    }

    if (strcmp(vnc_key, key)) {
        g_printf("Confirm password error.\r\n");
        clear_sensitive_info(vnc_key, sizeof(vnc_key));
        clear_sensitive_info(key, IPMC_USER_PASSWORD_LEN);
        return RET_ERR;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    clear_sensitive_info(key, IPMC_USER_PASSWORD_LEN);
    ret = set_vnc_pwd(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, (const gchar *)vnc_key);
    clear_sensitive_info(vnc_key, sizeof(vnc_key));

    if (ret == VNC_KEY_ERR2) {
        g_printf("Failed to set VNC password.\r\n");
        g_printf("The password complexity requirements are as follows:\n");
        g_printf("1) Must contain 8 characters.\n");
        g_printf("2) Must contain at least one space or one of the following special characters:\n");
        g_printf("   `~!@#$%%^&*()-_=+\\|[{}];:'\",<.>/?\n");
        g_printf("3) Must contain at least two types of the following characters:\n");
        g_printf("   - Lowercase letters:a to z;\n");
        g_printf("   - Uppercase letters:A to Z;\n");
        g_printf("   - Digits:0 to 9;\n");
        return RET_ERR;
    }

    if (ret == VNC_KEY_ERR1) {
        g_printf("The VNC password length must be between %d to %d.\r\n", VNC_KEY_MIN_LEN, VNC_KEY_MAX_LEN);
        g_printf("Failed to set VNC password.\r\n");
        return RET_ERR;
    }

    if (ret != RET_OK) {
        g_printf("Failed to set VNC password.\r\n");
        return RET_ERR;
    }

    g_printf("Set VNC password successfully.\r\n");

    return RET_OK;
}


gint32 ipmc_set_vnc_ssl_state(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    guint8 ssl_state = 0;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return RET_ERR;
    }
    if (argc != 2 || strcmp(argv[0], "-v") != 0) {
        g_printf("Usage: ipmcset -t vnc -d ssl -v <enabled|disabled>\r\n");
        return RET_ERR;
    }

    if (strcasecmp(argv[1], "enabled") == 0) {
        ssl_state = VNC_ENABLED;
    } else if (strcasecmp(argv[1], "disabled") == 0) {
        ssl_state = VNC_DISABLED;
    } else {
        g_printf("Usage: ipmcset -t vnc -d ssl -v <enabled|disabled>\r\n");
        return RET_ERR;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    

    ret = set_vnc_ssl_state(OPERATOR_LOG_CLI, username, ip, ssl_state);
    if (ret != RET_OK) {
        if (ret == VNC_IN_USE) {
            g_printf("The VNC service is in use.\r\n");
        }
        g_printf("Failed to set VNC SSL encryption state (%s).\r\n", (ssl_state) ? "enabled" : "disabled");
        return RET_ERR;
    }

    g_printf("Set VNC SSL encryption state (%s) successfully.\r\n", (ssl_state) ? "enabled" : "disabled");

    return RET_OK;
}

gint32 ipmc_set_vnc_timeout(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    gint32 timeout = 0xff;
    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return RET_ERR;
    }

    
    if ((argc != 2) || (strcmp(argv[0], "-v") != 0)) {
        print_vnctimeout_usage();
        return RET_ERR;
    }

    if (strlen(argv[1]) == 0) {
        print_vnctimeout_usage();
        return RET_ERR;
    }

    ret = vos_str2int(argv[1], 10, &timeout, NUM_TPYE_INT32);
    if (ret != RET_OK) {
        print_vnctimeout_usage();
        return ret;
    }

    
    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    ret = set_vnc_timeout(OPERATOR_LOG_CLI, username, ip, timeout);
    if (ret == VNC_TIMEOUT_ERR1) {
        g_printf("The VNC timeout period must be between %d to %d.\r\n", VNC_MIN_TIMEOUT, VNC_MAX_TIMEOUT);
        g_printf("Failed to set VNC timeout period.\r\n");
        return RET_ERR;
    }

    if (ret != RET_OK) {
        g_printf("Failed to set VNC timeout period.\r\n");
        return RET_ERR;
    }

    g_printf("Set VNC timeout period successfully.\r\n");

    return RET_OK;
}