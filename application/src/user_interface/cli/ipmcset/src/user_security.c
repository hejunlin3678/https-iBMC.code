

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
#include "ipmcset_user_security.h"
#include "ipmcset_netconfig.h"
#include "ipmcset_power.h"
#include "ipmcset_service_mgmt.h"
#include "ipmcset_sys_mgmt.h"
#include "ipmcset_alarm.h"

#define SET_IP_TYPE_IP_ADDR "IP address"
#define SET_IP_TYPE_IP_SUBMASK "MASK address"
#define SET_IP_TYPE_IP_GATEWAY "GATEWAY"
#define SET_IP_TYPE_IP_MODE "IP Mode"
#define SET_IP_TYPE_IP6_ADDR "IPV6 address"
#define SET_IP_TYPE_IP6_PREFIX "IPV6 prefix"
#define SET_IP_TYPE_IP6_GATEWAY "GATEWAY6"
#define SET_IP_TYPE_IP6_MODE "IPV6 Mode"
#define SET_IP_TYPE_VLAN_STATE "vlan state"
#define SET_IP_TYPE_NETMODE "net mode"
#define SET_IP_TYPE_ACTIVE_PORT "active port"

#define SET_USER_ADD "Add User"
#define SET_USER_DEL "Del User"


gint32 ipmc_add_user(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    guint8 user_id = 0;
    guint8 free_user_id = 0;
    gchar oper_log[MAX_OPERATE_LOG_LEN + 1] = {0};
    gchar user_pwd[IPMC_USER_PASSWORD_LEN] = {0};
    guchar flag = 1;
    gchar *user_name = argv[1];

    (void)get_double_certification_enable_flag(&flag);

    if (((argc != 2) && (argc != 3)) || (strcmp(argv[0], "-v")) || ((argc == 3) && (flag == 1))) {
        print_adduser_helpinfo();
        return FALSE;
    }

    if (g_utf8_validate(user_name, -1, NULL) == FALSE) {
        print_adduser_helpinfo();
        return FALSE;
    }

    
    if (strlen(user_name) > USER_NAME_MAX_LEN) {
        user_name[USER_NAME_MAX_LEN] = '*';
        user_name[USER_NAME_MAX_LEN + 1] = '\0';
    }

    ret = snprintf_s(oper_log, sizeof(oper_log), sizeof(oper_log) - 1, "Add user(%s) failed", user_name);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        ipmc_log_operation_log("User", oper_log);
        return FALSE;
    }

    if (ipmc_check_user_passwd() == RET_ERR || ipmc_check_user_name(user_name) == RET_ERR) {
        ipmc_log_operation_log("User", oper_log);
        return FALSE;
    }

    
    if (ipmc_get_empty_user_id(&free_user_id) != RET_OK) {
        g_printf("User full, cannot add more user.\r\n");
        ipmc_log_operation_log("User", oper_log);
        return FALSE;
    }

    
    if (ipmc_check_user_exist(user_name, &user_id) != RET_OK || user_id != IPMC_MAX_USER_NUMBER) {
        g_printf("User %s exists.\r\n", user_name);
        ret = snprintf_s(oper_log, MAX_OPERATE_LOG_LEN + 1, MAX_OPERATE_LOG_LEN, "Add user%u's username (%s) failed",
            free_user_id, user_name);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        }
        ipmc_log_operation_log("User", oper_log);
        return FALSE;
    }

    if (argc == 2) {
        if (ipmc_get_user_password(user_pwd, sizeof(user_pwd)) != RET_OK) {
            ret = snprintf_s(oper_log, sizeof(oper_log), sizeof(oper_log) - 1, "Add user%u's username (%s) failed",
                free_user_id, user_name);
            if (ret <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
            }
            ipmc_log_operation_log("User", oper_log);
            return FALSE;
        }
        
        
        if (ipmc_get_empty_user_id(&free_user_id) != RET_OK) {
            clear_sensitive_info(user_pwd, sizeof(user_pwd));
            g_printf("User full, cannot add more user.\r\n");
            ret = snprintf_s(oper_log, sizeof(oper_log), sizeof(oper_log) - 1, "Add user(%s) failed", user_name);
            if (ret <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
            }
            ipmc_log_operation_log("User", oper_log);
            return FALSE;
        }

        
    } else if ((argc == 3) && (flag == 0)) {
        ret = strncpy_s(user_pwd, sizeof(user_pwd), argv[2], sizeof(user_pwd) - 1);
        if (ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, ret);
        }
        clear_sensitive_info(argv[2], strlen(argv[2]));
    } else {
        print_adduser_helpinfo();
        ret = snprintf_s(oper_log, sizeof(oper_log), sizeof(oper_log) - 1, "Add user(%s) failed", user_name);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        }
        ipmc_log_operation_log("User", oper_log);
        return FALSE;
    }

    if (add_user_name(free_user_id, user_name) != RET_OK) {
        g_printf("Set user name failed.\r\n");
        clear_sensitive_info(user_pwd, sizeof(user_pwd));
        return FALSE;
    }

    if (set_user_password(free_user_id, user_pwd) != RET_OK) {
        (void)set_user_name(free_user_id, "");
        g_printf("Set user password failed.\r\n");
        clear_sensitive_info(user_pwd, sizeof(user_pwd));
        return FALSE;
    }

    clear_sensitive_info(user_pwd, sizeof(user_pwd));
    g_printf("Add user successfully.\r\n");
    return TRUE;
}


gint32 ipmc_change_user_password(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    errno_t safe_fun_ret = EOK;

    gchar user_pwd[IPMC_USER_PASSWORD_LEN] = {0};
    guchar user_id = 0;
    guchar flag = 1;

    (void)get_double_certification_enable_flag(&flag);

    if (((argc != 2) && (argc != 3)) || strcmp(argv[0], "-v") || ((argc == 3) && (flag == 1))) {
        g_printf("Usage: ipmcset %s-d password -v <username>\r\n", help_default_name);
        return FALSE;
    }

    if (setuser_privilege_judge(privilege, rolepriv, argv[1], &user_id) != TRUE) {
        ipmc_log_operation_log("User", "Set user password failed");
        return FALSE;
    }

    if (argc == 2) {
        if (ipmc_get_user_password(user_pwd, sizeof(user_pwd)) != RET_OK) {
            ipmc_log_operation_log("User", "Set user password failed");
            return FALSE;
        }
    } else if ((argc == 3) && (flag == 0)) {
        
        safe_fun_ret = strncpy_s(user_pwd, sizeof(user_pwd), argv[2], sizeof(user_pwd) - 1);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        clear_sensitive_info(argv[2], strlen(argv[2]));
        
    } else {
        g_printf("Usage: ipmcset %s-d password -v <username>\r\n", help_default_name);
        ipmc_log_operation_log("User", "Set user password failed");
        return FALSE;
    }

    if (set_user_password(user_id, user_pwd) != RET_OK) {
        g_printf("Set user password failed.\r\n");
        clear_sensitive_info(user_pwd, sizeof(user_pwd));
        return FALSE;
    }

    clear_sensitive_info(user_pwd, sizeof(user_pwd));
    g_printf("Set user password successfully.\r\n");
    return TRUE;
}


gint32 ipmc_delete_public_key(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    guchar user_id = 0;

    
    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        g_printf("Usage: ipmcset -d delpublickey -v <username>\r\n");
        return FALSE;
    }

    
    if (setuser_privilege_judge(privilege, rolepriv, argv[1], &user_id) == FALSE) {
        ipmc_log_operation_log("User", "Delete user public key failed");
        return FALSE;
    }

    
    if (set_user_public_key(user_id, USER_PUBLIC_KEY_TYPE_DELETE, "") != RET_OK) {
        g_printf("Delete user public key failed.\r\n");
        return FALSE;
    }

    g_printf("Delete user public key successfully.\r\n");
    return TRUE;
}


gint32 ipmc_delete_user(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 iRet = -1;

    guint8 user_id = 0;
    gchar oper_log[MAX_OPERATE_LOG_LEN + 1] = {0};
    gchar user_name[IPMC_USER_NAME_MAX_LEN] = {0};

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        ipmc_log_operation_log("User", "Delete user failed");
        return FALSE;
    }

    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        g_printf("Usage: ipmcset %s-d deluser -v <username>\r\n", help_default_name);
        return FALSE;
    }

    if (ipmc_check_user_passwd() != RET_OK) {
        ipmc_log_operation_log("User", "Delete user failed");
        return FALSE;
    }

    if (ipmc_check_user_name(argv[1]) != RET_OK) {
        ipmc_log_operation_log("User", "Delete user failed");
        return FALSE;
    }

    
    (void)strncpy_s(user_name, IPMC_USER_NAME_MAX_LEN, argv[1], IPMC_USER_NAME_MAX_LEN - 1);
    

    if (ipmc_check_user_exist(user_name, &user_id) != RET_OK || user_id == IPMC_MAX_USER_NUMBER) {
        g_printf("User %s doesn't exist.\r\n", user_name);
        
        iRet = snprintf_s(oper_log, MAX_OPERATE_LOG_LEN + 1, MAX_OPERATE_LOG_LEN,
            "Delete user%u's username (%s) failed", user_id, user_name);
        if (iRet <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
        }
        
        ipmc_log_operation_log("User", oper_log);
        return FALSE;
    }

    
    if (check_is_only_enabled_admin(user_id) == TRUE) {
        
        g_printf("WARNING: The system must have at least one enabled administrator.\r\n");
        g_printf("This operation will delete the last enabled administrator.\r\n");
        ipmc_log_operation_log("User", "Delete user failed");
        g_printf("Delete user %s failed.\r\n", user_name);
        return FALSE;
    }

    
    gint32 ret = set_user_name(user_id, "");
    switch (ret) {
        case RET_OK:
            g_printf("Delete user %s successfully.\r\n", user_name);
            return TRUE;
        case USER_DELETE_TRAPV3_USER_UNSUPPORT:
            g_printf("%s is a SNMP trap user and cannot be deleted.\r\n", user_name);
            return FALSE;
        case USER_DELETE_EXCLUDE_USER_UNSURPPORT:
            g_printf("%s is an emergency user and cannot be deleted.\r\n", user_name);
            return FALSE;
        default:
            g_printf("Delete user %s failed.\r\n", user_name);
            return FALSE;
    }
    
}


LOCAL gint32 get_certificate_file(gchar **argv, gchar *file_info, size_t file_info_len)
{
    errno_t safe_fun_ret = EOK;
    gint32 ret;

    ret = g_regex_match_simple(CUSTOM_CERT_IMPORT_REGEX, argv[1], G_REGEX_OPTIMIZE, (GRegexMatchFlags)0);
    if (ret != TRUE) {
        g_printf("File path is invalid.\r\n");
        print_ipmort_certificate_usage();
        return FALSE;
    }

    
    
    if ((strstr(argv[1], FILE_URL_HTTPS_HEAD) == NULL) && (strstr(argv[1], FILE_URL_SFTP_HEAD) == NULL) &&
        (strstr(argv[1], FILE_URL_CIFS_HEAD) == NULL) && (strstr(argv[1], FILE_URL_SCP_HEAD) == NULL) &&
        (strstr(argv[1], FILE_URL_NFS_HEAD) == NULL)) {
        
        
        if (strncmp(argv[1], "/", strlen("/")) != 0 || dal_check_real_path(argv[1]) != RET_OK) {
            g_printf("File path is invalid.\r\n");
            print_ipmort_certificate_usage();
            return FALSE;
        }

        
        safe_fun_ret = strncpy_s(file_info, file_info_len, argv[1], file_info_len - 1);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            return FALSE;
        }
    } else {
        ret = get_file_path_from_url(argv[1], FILE_ID_SSL_CER, file_info, file_info_len);
        if (ret != TRUE) {
            g_printf("Import certificate failed.\r\n");
            return FALSE;
        }
    }
    return TRUE;
}


LOCAL gint32 check_certificate_file(guchar privilege, const gchar *rolepriv, gchar **argv, 
    gchar *username, gchar *file_info) 
{
    gint32 ret;

    
    if (vos_get_file_accessible(file_info) == FALSE) {
        g_printf("Import certificate failed.\r\n");
        return FALSE;
    }

    
    gint32 file_len = vos_get_file_length(file_info);
    
    if (file_len > MAX_ENCRYPTED_CERT_FILE_LEN) {
        g_printf("Import certificate failed.\r\n");
        ipmc_delete_file(username, file_info);
        return FALSE;
    }
    

    
    
    if (g_strcmp0(FILE_PATH_SSL_CER_DEFAULT, file_info) != 0) {
        
        ret = dal_copy_file(FILE_PATH_SSL_CER_DEFAULT, file_info);
        
        if (ret != RET_OK) {
            g_printf("Import certificate failed.\r\n");
            ipmc_delete_file(username, file_info);
            return FALSE;
        }
        (void)chmod(FILE_PATH_SSL_CER_DEFAULT, (S_IRUSR | S_IWUSR));
    }
    
    return TRUE;
}


LOCAL void clear_centificate_arg_info(guchar argc, gchar **argv)
{
    for (gint32 i = 0; i < argc; i++) {
        clear_sensitive_info(argv[i], strlen(argv[i]));  // 防止用户用错命令，泄露敏感信息，就全部清除
    }
}



gint32 ipmc_set_certificate_import(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gchar file_info[MAX_FILEPATH_LENGTH + 1] = {0};
    GSList *input_list = NULL;
    gint32 ret;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    gchar *pw_tmp = NULL;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        clear_centificate_arg_info(argc, argv);
        return FALSE;
    }

    if ((argc < 3) || (argc > 4) || (strcmp(argv[0], "-v")) || (strcmp(argv[2], "1"))) {
        clear_centificate_arg_info(argc, argv);
        print_ipmort_certificate_usage();
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    if (get_certificate_file(argv, file_info, sizeof(file_info)) == FALSE) {
        clear_centificate_arg_info(argc, argv);
        return FALSE;
    }

    if (check_certificate_file(privilege, rolepriv, argv, username, file_info) == FALSE) {
        clear_centificate_arg_info(argc, argv);
        return FALSE;
    }

    
    if (argc == 3) {
        pw_tmp = "";
    } else {
        pw_tmp = argv[3];
    }

    input_list = g_slist_append(input_list, g_variant_new_string(pw_tmp));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, SSL_CLASEE_NAME,
        SSL_OBJECT_NAME, SSL_METHOD_SNMP_SETCUSTOMCERTFILEPASS, input_list, NULL);
    uip_free_gvariant_list(input_list);
    input_list = NULL;
    clear_sensitive_info(pw_tmp, strlen(pw_tmp));
    clear_centificate_arg_info(argc, argv);

    if (ret != RET_OK) {
        (void)dal_delete_file(FILE_PATH_SSL_CER_DEFAULT);
        ipmc_delete_file(username, file_info);
        g_printf("Import certificate failed.\r\n");
        return FALSE;
    }

    
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, SSL_CLASEE_NAME,
        SSL_OBJECT_NAME, SSL_METHOD_SNMP_IMPORTCUSTOMCERT, NULL, NULL);
    ipmc_delete_file(username, file_info);
    if (ret != RET_OK) {
        g_printf("Import certificate failed.\r\n");
        return FALSE;
    }

    g_printf("Import certificate successfully\r\n");
    return TRUE;
}


gint32 ipmc_set_emergency_user(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    guint8 user_id;
    GVariant *property_value = NULL;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        ipmc_log_operation_log("SecurityEnhance", "Insufficient privilege");
        return FALSE;
    }

    if ((argc != 2) || ((argv[0]) && strcmp(argv[0], "-v"))) {
        g_printf("Usage: ipmcset -d emergencyuser -v <username>\r\n");
        return FALSE;
    }

    if (argv[1] == NULL) {
        g_printf("Usage: ipmcset -d emergencyuser -v <username>\r\n");
        return FALSE;
    }

    property_value = g_variant_new_string(argv[1]);
    ret = dfl_get_specific_object(CLASS_USER, PROPERTY_USER_NAME, property_value, &obj_handle);
    g_variant_unref(property_value);
    property_value = NULL;
    if (ret != DFL_OK) {
        if (ret == ERRCODE_OBJECT_NOT_EXIST) {
            ipmc_log_operation_log("SecurityEnhance", "Set emergency user failed");
            g_printf("User doesn't exist.\r\n");
            return FALSE;
        }
        ipmc_log_operation_log("SecurityEnhance", "Set emergency user failed");
        g_printf("Get object handle failed.\r\n ");
        return FALSE;
    }

    ret = uip_get_object_property(CLASS_USER, dfl_get_object_name(obj_handle), PROPERTY_USER_ID, &property_value);
    if (ret != RET_OK) {
        ipmc_log_operation_log("SecurityEnhance", "Set emergency user failed");
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
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_SECURITY_ENHANCE,
        OBJ_SECURITY_ENHANCE, METHOD_SECURITY_ENHANCE_SETEXCLUDEUSER, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        g_printf("Set emergency user to (%s) failed.\r\n", argv[1]);
        return FALSE;
    }

    g_printf("Set emergency user to (%s) successfully.\r\n", argv[1]);

    return TRUE;
}



gint32 ipmc_set_inactive_time_limit(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;
    gchar custom_name[LOG_SPECIAL_STRING_LEN_MAX] = {"iBMC"};

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        ipmc_log_operation_log("SecurityEnhance", "Insufficient privilege");
        return FALSE;
    }

    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        print_set_inactive_user_timeout_helpinfo();
        return FALSE;
    }
    
    if (dal_get_custom_string_name(custom_name, BMC_CUSTOM_LOGIBMC_NAME, sizeof(custom_name)) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_custom_string_name failed.", __FUNCTION__);
    }

    ret = dal_get_object_handle_nth(CLASS_SECURITY_ENHANCE, 0, &obj_handle);
    if (ret != RET_OK) {
        ipmc_log_operation_log("SecurityEnhance", "Set inactive user timelimit failed");
        g_printf("Get object failed.\r\n");
        goto out;
    }

    if (!((str_to_ul(argv[1]) == 0) || (str_to_ul(argv[1]) <= SECURITYENHANCE_INACTIVEUSERTIME_MAX &&
        str_to_ul(argv[1]) >= SECURITYENHANCE_INACTIVEUSERTIME_MIN))) {
        g_printf("Input parameter is out of range.\n");
        ipmc_log_operation_log("SecurityEnhance", "Set inactive user timelimit failed");
        print_set_inactive_user_timeout_helpinfo();
        return FALSE;
    }

    if (str_to_ul(argv[1]) != 0) {
        
        g_printf("WARNING: This operation could lead to %s ", custom_name);
        g_printf("users be disabled when users' inactive time is overdue.\r\n");
        if (check_the_input_operation(CLI_COMFIRM_STRING) != RET_OK) {
            return FALSE;
        }
    }

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_uint32(str_to_ul(argv[1])));
    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    ret = uip_call_class_method(OPERATOR_LOG_CLI, username, ip, CLASS_SECURITY_ENHANCE, OBJ_SECURITY_ENHANCE,
        METHOD_SECURITY_ENHANCE_SETUSERINACTTIMELIMIT, input_list, NULL);
    uip_free_gvariant_list(input_list);

out:
    g_printf("Set inactive user timelimit %s.\r\n", (ret == RET_OK) ? "successfully" : "failed");

    return TRUE;
}



gint32 ipmc_set_masterkey_update_interval(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target)
{
#define NUMOFARGV 2
    gint32 ret;
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;
    guint32 interval_days = 0;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        ipmc_log_operation_log("SecurityEnhance", "Insufficient privilege");
        return FALSE;
    }

    if ((argc != NUMOFARGV) || (strcmp(argv[0], "-v"))) {
        print_set_update_interval_helpinfo();
        return FALSE;
    }

    ret = dal_get_object_handle_nth(CLASS_SECURITY_ENHANCE, 0, &obj_handle);
    if (ret != RET_OK) {
        ipmc_log_operation_log("SecurityEnhance", "Set masterkey update interval failed");
        g_printf("Get object failed.\r\n");
        goto out;
    }

    interval_days = str_to_ul(argv[1]);
    if (!((interval_days == SECURITYENHANCE_AUTOUPDATEINTERVAL_DEFAULT) ||
        (interval_days <= SECURITYENHANCE_AUTOUPDATEINTERVAL_MAX &&
        interval_days >= SECURITYENHANCE_AUTOUPDATEINTERVAL_MIN))) {
        ipmc_log_operation_log("SecurityEnhance", "Set masterkey update interval failed");
        g_printf("Input parameter is out of range.\n");
        print_set_update_interval_helpinfo();
        return FALSE;
    }

    if (interval_days != SECURITYENHANCE_AUTOUPDATEINTERVAL_DEFAULT) {
        g_printf("WARNING: This operation enables the BMC to automatically update the master key when the update "
            "interval is reached.\r\n");
        ret = check_the_input_operation(CLI_COMFIRM_STRING);
        if (ret != RET_OK) {
            return FALSE;
        }
    }

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_uint32(interval_days));
    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    ret = uip_call_class_method(OPERATOR_LOG_CLI, username, ip, CLASS_SECURITY_ENHANCE, OBJ_SECURITY_ENHANCE,
        METHOD_SECURITY_ENHANCE_SETMASTERKEYUPDATEINTERVAL, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: call method %s failed, ret is %d", __FUNCTION__,
            METHOD_SECURITY_ENHANCE_SETMASTERKEYUPDATEINTERVAL, ret);
    }
out:
    g_printf("Set master key automatic update interval %s.\r\n", (ret == RET_OK) ? "successfully" : "failed");

    return TRUE;
}


gint32 ipmc_set_pass_complexity_check_enable(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target)
{
    guint8 enable_state = 0;
    gint32 ret;
    gint32 result;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    gint32 input = 0;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        (void)print_SetPasswordComplexityCheckState_helpinfo();
        return FALSE;
    }

    if (!strcmp(argv[1], "enabled")) {
        enable_state = 1;
    } else if (!strcmp(argv[1], "disabled")) {
        enable_state = 0;
    } else {
        (void)print_SetPasswordComplexityCheckState_helpinfo();
        return FALSE;
    }

    if (enable_state == 0) {
        g_printf("WARNING: Disabling the complexity checking function may reduce the system security.\r\n");
        input = ipmc_get_user_input(CLI_COMFIRM_STRING);
        if ((input == 'n') || (input == 'N')) {
            return FALSE;
        } else if ((input == 'y') || (input == 'Y')) {
        } else {
            g_printf("Input character invalid.\r\n");
            return FALSE;
        }
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(enable_state));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_PASSWD_SETTING,
        CLASS_PASSWD_SETTING, METHOD_PASSWD_SETTING_SETENABLEPWDCOMPLEXITY, input_list, &output_list);
    uip_free_gvariant_list(input_list);

    if (ret != RET_OK) {
        g_printf("Set password complexity check state failed.\r\n");
        return FALSE;
    }

    result = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
    if (result != RET_OK) {
        uip_free_gvariant_list(output_list);
        g_printf("Set password complexity check state failed!\r\n");
        return FALSE;
    }

    uip_free_gvariant_list(output_list);

    g_printf("Set password complexity check state successfully.\r\n");
    return TRUE;
}


gint32 ipmc_set_security_banner_content(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target)
{
    gint32 ret;
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    guchar set_mode;
    OBJ_HANDLE obj_handle = 0;
    guchar state = 0;
    gchar content[MOTD_FILE_MAX_LEN + 1] = {0};

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        ipmc_log_operation_log("SecurityEnhance", "Insufficient privilege");
        return FALSE;
    }

    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        print_set_security_banner_content_help_info();
        return FALSE;
    }

    ret = dal_get_object_handle_nth(CLASS_SECURITY_ENHANCE, 0, &obj_handle);
    if (ret != RET_OK) {
        ipmc_log_operation_log("SecurityEnhance", "Set login security banner content failed");
        g_printf("Get object failed.\r\n");
        goto out;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SECURITY_BANNERSTATE, &state);
    if (ret != RET_OK) {
        ipmc_log_operation_log("SecurityEnhance", "Set login security banner content failed");
        g_printf("Get login security banner state failed.\r\n");
        goto out;
    }

    if (state == STATE_DISABLE) {
        ipmc_log_operation_log("SecurityEnhance", "Set login security banner content failed");
        g_printf("Login security banner state is disabled. "
            "Please set login security banner state to enabled first.\r\n");
        ret = RET_ERR;
        goto out;
    }

    set_mode = (strcmp(argv[1], "default") == 0) ? 1 : 0;
    if (set_mode == 1) {
        ret = dal_get_property_value_string(obj_handle, PROPERTY_SECURITY_D_BANNERCONTENT, content, sizeof(content));
        if (ret != RET_OK) {
            ipmc_log_operation_log("SecurityEnhance", "Set login security banner content failed");
            g_printf("Get login security banner default information failed.\r\n");
            goto out;
        }

        input_list = g_slist_append(input_list, (gpointer)g_variant_new_string(content));
    } else {
        input_list = g_slist_append(input_list, (gpointer)g_variant_new_string(argv[1]));
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    ret = uip_call_class_method(OPERATOR_LOG_CLI, username, ip, CLASS_SECURITY_ENHANCE, OBJ_SECURITY_ENHANCE,
        METHOD_SECURITY_ENHANCE_SETBANNERCONTENT, input_list, NULL);
    uip_free_gvariant_list(input_list);

out:
    g_printf("Set login security banner content %s.\r\n", (ret == RET_OK) ? "successfully" : "failed");

    return TRUE;
}


gint32 ipmc_set_security_banner_state(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    guchar state = 0;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        ipmc_log_operation_log("SecurityEnhance", "Insufficient privilege");
        return FALSE;
    }

    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        print_set_security_banner_state_help_info();

        return FALSE;
    }

    if (strcmp(argv[1], "enabled") == 0) {
        state = STATE_ENABLE;
    } else if (strcmp(argv[1], "disabled") == 0) {
        state = STATE_DISABLE;
    } else {
        ipmc_log_operation_log("SecurityEnhance", "Invalid input");
        print_set_security_banner_state_help_info();
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(state));

    ret = uip_call_class_method(OPERATOR_LOG_CLI, username, ip, CLASS_SECURITY_ENHANCE, OBJ_SECURITY_ENHANCE,
        METHOD_SECURITY_ENHANCE_SETBANNERSTATE, input_list, NULL);

    g_printf("%s login security banner state %s.\r\n", (state == STATE_ENABLE) ? "Enable" : "Disable",
        (ret == RET_OK) ? "successfully" : "failed");

    uip_free_gvariant_list(input_list);

    return TRUE;
}


gint32 ipmc_set_update_masterkey(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    OBJ_HANDLE obj_handle;
    gchar ip[PARAMETER_LEN + 1] = {0};
    gchar username[PARAMETER_LEN + 1] = {0};
    guchar update_status;
    GSList *output_list = NULL;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        ipmc_log_operation_log("SecurityEnhance", "Insufficient privilege");
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcset -t securityenhance -d updatemasterkey\r\n");
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    print_update_master_key_warnninginfo();

    ret = check_the_input_operation(CLI_COMFIRM_STRING);
    if (ret != RET_OK) {
        return FALSE;
    }

    ret = dal_get_object_handle_nth(CLASS_SECURITY_ENHANCE, 0, &obj_handle);
    if (ret != RET_OK) {
        ipmc_log_operation_log("SecurityEnhance", "Update masterkey failed");
        return ret;
    }

    ret = dfl_call_class_method(obj_handle, METHOD_SECURITY_ENHANCE_GETUPDATESTATUS, NULL, NULL, &output_list);
    if (ret != DFL_OK) {
        ipmc_log_operation_log("SecurityEnhance", "Update masterkey failed");
        return ret;
    }

    update_status = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 0));
    uip_free_gvariant_list(output_list);
    if (update_status == MK_UPDATING) {
        g_printf("Master key is being updated.\n");
        debug_log(DLOG_INFO, "%s: Master key is being updated", __FUNCTION__);
        return TRUE;
    }

    if (update_status == MK_UPDATED) {
        g_printf("Failed to update the master key because it can be updated only once within 24 hours.\nTry again in "
            "24 hours.\n");
        debug_log(DLOG_INFO, "%s: Master key has updated", __FUNCTION__);
        return TRUE;
    }

    g_printf("Update master key begin.\r\n");

    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_SECURITY_ENHANCE,
        OBJ_SECURITY_ENHANCE, METHOD_SECURITY_ENHANCE_UPDATEMASTERKEY, NULL, NULL);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: call method %s failed, ret is %d", __FUNCTION__,
            METHOD_SECURITY_ENHANCE_UPDATEMASTERKEY, ret);
    }
    g_printf("Update master key %s.\r\n", (ret == RET_OK) ? "successfully" : "failed");
    return TRUE;
}



gint32 ipmc_set_user_lock(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    errno_t safe_fun_ret;
    gint32 ret;
    guint8 user_id = 0;
    gchar user_name[IPMC_USER_NAME_MAX_LEN] = {0};
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        g_printf("Usage: ipmcset -t user -d lock -v <username>\r\n");
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    
    if (strcmp(argv[1], username) == 0) { // 锁定自己
        g_printf("The currently logged-in user's account cannot be locked.\r\n");
        ipmc_log_operation_log("User", "Lock user failed");
        return FALSE;
    }

    

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        ipmc_log_operation_log("User", "Lock user failed");
        return FALSE;
    }

    
    if (ipmc_check_user_passwd() != RET_OK) {
        ipmc_log_operation_log("User", "Lock user failed");
        return FALSE;
    }

    if (ipmc_check_user_name(argv[1]) != RET_OK) {
        ipmc_log_operation_log("User", "Lock user failed");
        return FALSE;
    }

    safe_fun_ret = strncpy_s(user_name, IPMC_USER_NAME_MAX_LEN, argv[1], IPMC_USER_NAME_MAX_LEN - 1);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }

    if (ipmc_check_user_exist(user_name, &user_id) != RET_OK || user_id == IPMC_MAX_USER_NUMBER) {
        g_printf("User %s doesn't exist.\r\n", user_name);
        ipmc_log_operation_log("User", "Lock user failed");
        return FALSE;
    }

    ret = dal_get_specific_object_byte(CLASS_USER, PROPERTY_USER_ID, user_id, &obj_handle);
    if (ret != RET_OK) {
        g_printf("User %s doesn't exist.\r\n", user_name);
        ipmc_log_operation_log("User", "Lock user failed");
        return FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(USER_LOCK));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_USER,
        dfl_get_object_name(obj_handle), METHOD_USER_SETUSERLOCKSTATE, input_list, NULL);
    uip_free_gvariant_list(input_list);

    if (ret != RET_OK) {
        g_printf("Lock user:%s failed.\r\n", user_name);
        return FALSE;
    }

    g_printf("Lock user:%s successfully.\r\n", user_name);
    return TRUE;
}



gint32 ipmc_set_user_login_interface(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    GSList *input_list = NULL;
    gint32 ret;
    guint32 mask = 0;
    guint32 old_mask;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;
    guint32 state = 0;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    ret = argc < 4 || argc > 10 || strcmp(argv[0], "-v");
    if (ret) {
        print_user_login_interface_usage();
        return FALSE;
    }

    if (dal_get_specific_object_string(CLASS_USER, PROPERTY_USER_NAME, argv[1], &obj_handle) != RET_OK) {
        g_printf("Wrong user name.\r\n");
        return FALSE;
    }

    if (strcmp(argv[2], "disabled") == 0) {
        state = 0;
    } else if (strcmp(argv[2], "enabled") == 0) {
        state = 1;
    } else {
        print_user_login_interface_usage();
        return FALSE;
    }

    // 获取当前掩码
    (void)dal_get_property_value_uint32(obj_handle, PROPERTY_USER_LOGIN_INTERFACE, &mask);

    mask = mask & USER_LOGIN_INTERFACE_VALUE_MASK;
    old_mask = mask;

    // 根据用户输入，修改掩码
    ret = get_user_login_interface_value(state, argc, argv, &mask);
    if (ret != RET_OK) {
        print_user_login_interface_usage();
        return FALSE;
    }

    // 检查当前用户密码
    if (ipmc_check_user_passwd() != RET_OK) {
        ipmc_log_operation_log("User", "Set user login interface failed");
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    input_list = g_slist_append(input_list, g_variant_new_uint32(mask));
    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_USER,
        obj_handle, METHOD_USER_SET_USER_LOGIN_INTERFACE, input_list, NULL);
    uip_free_gvariant_list(input_list);
    input_list = NULL;

    if (ret != RET_OK) {
        g_printf("Set user login interface failed.\r\n");
        return FALSE;
    }

    // 增加IPMI和SNMP权限时重置密码
    ret = add_ipmi_snmp_reset_passwd(old_mask, mask, argv[1]);
    if (ret != RET_OK) {
        input_list = g_slist_append(input_list, g_variant_new_uint32(old_mask));
        ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
            CLASS_USER, obj_handle, METHOD_USER_SET_USER_LOGIN_INTERFACE, input_list, NULL);
        uip_free_gvariant_list(input_list);
        g_printf("Set user login interface failed.\r\n");
        return FALSE;
    }

    g_printf("Set user login interface successfully.\r\n");
    return TRUE;
}



gint32 ipmc_set_user_state(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    errno_t safe_fun_ret;
    gint32 ret;
    guint8 user_id = 0;
    gchar user_name[IPMC_USER_NAME_MAX_LEN] = {0};
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    guint8 enable_state = 0;

    
    
    if ((argc != 3) || (strcmp(argv[0], "-v"))) {
        g_printf("Usage: ipmcset -t user -d state -v <username> [enabled|disabled]\r\n");
        ipmc_log_operation_log("User", "Enable/Disable user failed");
        return FALSE;
    }

    if (!strcasecmp(argv[2], "enabled")) {
        enable_state = 1;
    } else if (!strcasecmp(argv[2], "disabled")) {
        enable_state = 0;
    } else {
        g_printf("Usage: ipmcset -t user -d state -v <username> [enabled|disabled]\r\n");
        ipmc_log_operation_log("User", "Enable/Disable user failed");
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        ipmc_log_operation_log("User", "Enable/Disable user failed");
        return FALSE;
    }

    
    if (ipmc_check_user_passwd() != RET_OK) {
        ipmc_log_operation_log("User", "Enable/Disable user failed");
        return FALSE;
    }

    if (ipmc_check_user_name(argv[1]) != RET_OK) {
        ipmc_log_operation_log("User", "Enable/Disable user failed");
        return FALSE;
    }

    safe_fun_ret = strncpy_s(user_name, IPMC_USER_NAME_MAX_LEN, argv[1], IPMC_USER_NAME_MAX_LEN - 1);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }

    if (ipmc_check_user_exist(user_name, &user_id) != RET_OK || user_id == IPMC_MAX_USER_NUMBER) {
        g_printf("User %s doesn't exist.\r\n", user_name);
        ipmc_log_operation_log("User", "Enable/Disable user failed");
        return FALSE;
    }

    

    ret = dal_get_specific_object_byte(CLASS_USER, PROPERTY_USER_ID, user_id, &obj_handle);
    if (ret != RET_OK) {
        g_printf("User %s doesn't exist.\r\n", user_name);
        ipmc_log_operation_log("User", "Enable/Disable user failed");
        return FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(enable_state));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_USER,
        dfl_get_object_name(obj_handle), METHOD_USER_SETUSERENABLE, input_list, NULL);
    uip_free_gvariant_list(input_list);

    if (ret != RET_OK) {
        g_printf("%s user:%s failed.\r\n", (enable_state == 1) ? "Enable" : "Disable", user_name);
        return FALSE;
    }

    g_printf("%s user:%s successfully.\r\n", (enable_state == 1) ? "Enable" : "Disable", user_name);
    return TRUE;
}


#ifndef SECURITY_ENHANCED_COMPATIBLE_BOARD_V3
LOCAL void print_set_user_first_login_policy_help_info(void)
{
    g_printf("Usage: ipmcset -t user -d firstloginpolicy -v <username> [option]\r\n");
    g_printf("Options are:\r\n");
    g_printf("    %d : %s\r\n", FIRST_LOGIN_PLCY_PROMPT, FIRST_LOGIN_PLCY_PROMPT_STR);
    g_printf("    %d : %s\r\n", FIRST_LOGIN_PLCY_FORCE, FIRST_LOGIN_PLCY_FORCE_STR);
    return;
}

gint32 ipmc_set_user_first_login_policy(guchar privilege, const gchar *rolepriv,
    guchar argc, gchar **argv, gchar *target)
{
#define EXPECTED_ARG_NUM 3
    gchar cur_user[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;
    gchar user_name[PARAMETER_LEN + 1] = {0};
    guint8 user_id = 0;
    guint8 policy_val = FIRST_LOGIN_PLCY_NULL;
    GSList *input_list = NULL;

    
    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        ipmc_log_operation_log("User", "Set user first login policy failed, insufficient privilege");
        return FALSE;
    }

    
    if (get_username_ip(cur_user, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(cur_user, sizeof(cur_user), sizeof(cur_user) - 1, "%s", "unknown");
        snprintf_s(ip, sizeof(ip), sizeof(ip) - 1, "%s", "unknown");
    }

    
    if ((argc != EXPECTED_ARG_NUM) || (strcmp(argv[0], "-v"))) {
        print_set_user_first_login_policy_help_info();
        return FALSE;
    }

    if (!strcasecmp(argv[EXPECTED_ARG_NUM - 1], "1")) {
        policy_val = FIRST_LOGIN_PLCY_PROMPT;
    } else if (!strcasecmp(argv[EXPECTED_ARG_NUM - 1], "2")) {
        policy_val = FIRST_LOGIN_PLCY_FORCE;
    } else {
        print_set_user_first_login_policy_help_info();
        return FALSE;
    }

    
    errno_t safe_ret = strncpy_s(user_name, sizeof(user_name), argv[1], sizeof(user_name) - 1);
    if (safe_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret=%d", __FUNCTION__, safe_ret);
    }

    if (ipmc_check_user_exist(user_name, &user_id) != RET_OK || user_id == IPMC_MAX_USER_NUMBER) {
        g_printf("User %s doesn't exist.\r\n", user_name);
        ipmc_log_operation_log("User", "Set user first login policy failed, input user doesn't exist");
        return FALSE;
    }

    (void)dal_get_specific_object_byte(CLASS_USER, PROPERTY_USER_ID, user_id, &obj_handle);

    input_list = g_slist_append(input_list, g_variant_new_byte(policy_val));
    gint32 ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)cur_user, (const gchar *)ip, CLASS_USER,
        dfl_get_object_name(obj_handle), METHOD_USER_SET_FIRST_LOGIN_PLCY, input_list, NULL);
    uip_free_gvariant_list(input_list);

    g_printf("Set %s first login policy to %s password reset %s.\r\n", user_name,
        (policy_val == FIRST_LOGIN_PLCY_FORCE) ? "force" : "prompt", (ret == RET_OK) ? "successfully" : "failed");

    return (ret == RET_OK) ? TRUE : FALSE;
}
#endif


gint32 ipmc_set_user_unlock(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    errno_t safe_fun_ret;
    guint8 user_id = 0;
    gchar strpwd[IPMC_USER_PASSWORD_LEN] = {0};
    gchar user_name[IPMC_USER_NAME_MAX_LEN] = {0};
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        g_printf("Usage: ipmcset -t user -d unlock -v <username>\r\n");
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        ipmc_log_operation_log("User", "Unlock user failed");
        return FALSE;
    }

    
    if (ipmc_check_user_passwd() != RET_OK) {
        ipmc_log_operation_log("User", "Unlock user failed");
        return FALSE;
    }

    if (ipmc_check_user_name(argv[1]) != RET_OK) {
        ipmc_log_operation_log("User", "Unlock user failed");
        return FALSE;
    }

    safe_fun_ret = strncpy_s(user_name, IPMC_USER_NAME_MAX_LEN, argv[1], IPMC_USER_NAME_MAX_LEN - 1);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }

    if (ipmc_check_user_exist(user_name, &user_id) != RET_OK || user_id == IPMC_MAX_USER_NUMBER) {
        g_printf("User %s doesn't exist.\r\n", user_name);
        ipmc_log_operation_log("User", "Unlock user failed");
        return FALSE;
    }

    
    ret = dal_get_specific_object_byte(CLASS_USER, PROPERTY_USER_ID, user_id, &obj_handle);
    if (ret != RET_OK) {
        g_printf("User %s doesn't exist.\r\n", user_name);
        ipmc_log_operation_log("User", "Unlock user failed");
        return FALSE;
    }

    

    input_list = g_slist_append(input_list, g_variant_new_string(user_name));
    input_list = g_slist_append(input_list, g_variant_new_string(strpwd));
    input_list = g_slist_append(input_list,
        g_variant_new_byte(0x80 | LOG_TYPE_LOCAL)); // 0x80 表示对用户解锁，包括属性标记的锁定和PAM登录失败锁定
    input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)EXCLUDE_LOGIN_IP));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_USER,
        dfl_get_object_name(obj_handle), METHOD_USER_AUTHUSER, input_list, &output_list);
    uip_free_gvariant_list(input_list);

    if (ret != RET_OK) {
        g_printf("Unlock user:%s failed.\r\n", user_name);
        return FALSE;
    }

    ret = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
    uip_free_gvariant_list(output_list);

    if (ret != RET_OK) {
        g_printf("Unlock user:%s failed.\r\n", user_name);
        return FALSE;
    }

    g_printf("Unlock user:%s successfully.\r\n", user_name);
    return TRUE;
}


gint32 ipmc_set_usermgnt_switch(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 switch_option = 0;
    guint8 s_option;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    gchar oper_log[MAX_OPERATE_LOG_LEN + 1] = {0};
    GSList *input_list = NULL;
    gint32 ret;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        
        snprintf_s(oper_log, MAX_OPERATE_LOG_LEN + 1, MAX_OPERATE_LOG_LEN,
            "Failed to enable or disable the BMC user management function on the host side");
        
        ipmc_log_operation_log("User", oper_log);
        return FALSE;
    }

    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        print_switch_usermgnt_helpinfo();
        return FALSE;
    }

    if ((switch_option = str_to_int(argv[1])) == IPMC_ERROR) {
        print_switch_usermgnt_helpinfo();
        
        snprintf_s(oper_log, MAX_OPERATE_LOG_LEN + 1, MAX_OPERATE_LOG_LEN,
            "Failed to enable or disable the BMC user management function on the host side");
        
        ipmc_log_operation_log("User", oper_log);
        return FALSE;
    }

    if (switch_option < 0 || switch_option > 1) {
        print_switch_usermgnt_helpinfo();
        
        snprintf_s(oper_log, MAX_OPERATE_LOG_LEN + 1, MAX_OPERATE_LOG_LEN,
            "Failed to enable or disable the BMC user management function on the host side");
        
        ipmc_log_operation_log("User", oper_log);
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    s_option = (guint32)switch_option & 0xff;
    input_list = g_slist_append(input_list, g_variant_new_byte(s_option));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_SECURITY_ENHANCE,
        OBJ_SECURITY_ENHANCE, METHOD_SECURITY_ENHANCE_SETENABLEUSERMGNT, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        g_printf("Failed to enable or disable the BMC user management function on the host side.\r\n");
        return FALSE;
    }

    if (s_option == USER_MGNT_ENABLE) {
        g_printf("The BMC user management function is successfully enabled on the host side.\r\n");
    } else {
        g_printf("The BMC user management function is successfully disabled on the host side.\r\n");
    }

    return TRUE;
}


gint32 ipmc_set_userprivilege(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    guint8 user_id = 0;
    guint8 user_privilege = 0;
    guint32 user_roleid;
    gchar log_info[MAX_OPERATE_LOG_LEN + 1] = {0};
    guint32 ret;
    gchar *user_name = argv[1];

    if (argc != 3 || strcmp(argv[0], "-v")) {
        print_userprivilege_usage();
        return FALSE;
    }

    ret = log_ipmcset_user_privilege(argc, argv, user_name, log_info);
    if (ret == RET_ERR) {
        memset_s(log_info, sizeof(log_info), 0, sizeof(log_info));
        (void)strcpy_s(log_info, sizeof(log_info), "Set user privilege failed");
    }

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        ipmc_log_operation_log("User", log_info);
        return FALSE;
    }
    if (ipmc_check_user_passwd() != RET_OK) {
        ipmc_log_operation_log("User", log_info);
        return FALSE;
    }

    if (ipmc_check_user_name(user_name) != RET_OK) {
        ipmc_log_operation_log("User", log_info);
        return FALSE;
    }

    if (ipmc_check_user_exist(user_name, &user_id) != RET_OK || user_id == IPMC_MAX_USER_NUMBER) {
        g_printf("User %s doesn't exist.\r\n", user_name);
        ipmc_log_operation_log("User", log_info);
        return FALSE;
    }

    
    if (check_is_only_enabled_admin(user_id) == TRUE) {
        
        g_printf("WARNING: The system must have at least one enabled administrator.\r\n");
        g_printf("This operation will disable the privilege of the last enabled administrator.\r\n");
        g_printf("Set user privilege failed.\r\n");
        ipmc_log_operation_log("User", log_info);
        return FALSE;
    }

    user_roleid = (guint32)str_to_int(argv[2]);
    if (user_roleid >= USERROLE_USER && user_roleid <= USERROLE_ADMIN) {
        user_privilege = (guint8)user_roleid;
    } else if (user_roleid >= USERROLE_CUST1 && user_roleid <= USERROLE_CUST4) {
        user_privilege = PRIVILEGE_LEVEL_OPERATOR;
    } else if (user_roleid == USERROLE_NOACCESS) {
        user_privilege = PRIVILEGE_LEVEL_NOACCESS;
    } else {
        print_userprivilege_usage();
        ipmc_log_operation_log("User", log_info);
        return FALSE;
    }

    if (set_user_privilege(user_id, user_privilege, (guint8)user_roleid) != RET_OK) {
        g_printf("Set user privilege failed.\r\n");
        return FALSE;
    }

    if (set_user_roleid(user_id, (guint8)user_roleid) != RET_OK) {
        g_printf("Set user roleid failed.\r\n");
        return FALSE;
    }

    
    g_printf("Set user privilege successfully.\r\n");
    return TRUE;
}


gint32 ipmc_set_weak_pwddic(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret = TRUE;

    
    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    
    if (argc == 2) {
        
        if ((strcmp(argv[0], "-v") != 0) || ((strcmp("enabled", argv[1]) != 0) && (strcmp("disabled", argv[1]) != 0) &&
            (strcmp("export", argv[1]) != 0))) {
            print_weak_pwddic_usage();
            return FALSE;
        }

        if ((strcmp("enabled", argv[1]) == 0)) {
            ret = set_weak_pwddic_state(SERVICE_STATE_ENABLE);
        }

        if ((strcmp("disabled", argv[1]) == 0)) {
            ret = set_weak_pwddic_state(SERVICE_STATE_DISABLE);
        }

        if ((strcmp("export", argv[1]) == 0)) {
            ret = export_weak_pwddic_config(WEAK_PWDDICT_FILE_TMP_PATH);
        }
    } else if (argc == 3) {
        if ((strcmp(argv[0], "-v") != 0) || ((strcmp("import", argv[1]) != 0) && (strcmp("export", argv[1]) != 0))) {
            print_weak_pwddic_usage();
            return FALSE;
        }

        if ((strcmp("import", argv[1]) == 0)) {
            ret = import_weak_pwddic_config(argv[2]);
        }

        if ((strcmp("export", argv[1]) == 0)) {
            ret = export_weak_pwddic_config(argv[2]);
        }
    } else {
        print_weak_pwddic_usage();
        return FALSE;
    }

    return ret;
}


LOCAL gint32 check_crl_path_is_valid(const gchar *crl_file_path)
{
    if (g_regex_match_simple("[\\.][cC][rR][lL]$", crl_file_path, G_REGEX_OPTIMIZE, (GRegexMatchFlags)0) != TRUE) {
        debug_log(DLOG_ERROR, "%s: File path is invalid1.", __FUNCTION__);
        return RET_ERR;
    }

    if (vos_get_file_accessible(crl_file_path) == FALSE) {
        debug_log(DLOG_ERROR, "%s: File not found", __FUNCTION__);
        return RET_ERR;
    }

    if (dal_is_directory(crl_file_path) == TRUE) {
        debug_log(DLOG_ERROR, "%s: The path cannot be a directory.", __FUNCTION__);
        return RET_ERR;
    }

    return RET_OK;
}



gint32 ipmc_upload_crl(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    errno_t safe_fun_ret = EOK;
    gchar file_info[URL_MAX_LENGTH + 1] = {0};
    GSList *input_list = NULL;
    gint32 ret;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 3 || g_strcmp0(argv[0], "-v") || g_strcmp0(argv[2], "1")) {
        print_upload_crl_usage();
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    if ((strstr(argv[1], FILE_URL_HTTPS_HEAD) == NULL) && (strstr(argv[1], FILE_URL_SFTP_HEAD) == NULL) &&
        (strstr(argv[1], FILE_URL_CIFS_HEAD) == NULL) && (strstr(argv[1], FILE_URL_SCP_HEAD) == NULL) &&
        (strstr(argv[1], FILE_URL_NFS_HEAD) == NULL)) {
        
        if (strncmp(argv[1], "/", strlen("/")) != 0 || dal_check_real_path(argv[1]) != RET_OK) {
            g_printf("File path is invalid.\r\n");
            print_upload_crl_usage();
            return FALSE;
        }

        if (strlen(argv[1]) > MAX_FILEPATH_LENGTH) {
            g_printf("File path is too long.\r\n");
            print_upload_crl_usage();
            return FALSE;
        }
        
        safe_fun_ret = strncpy_s(file_info, URL_MAX_LENGTH + 1, argv[1], URL_MAX_LENGTH);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
    } else {
        ret = get_file_path_from_url(argv[1], FILE_ID_SSL_CER, file_info, sizeof(file_info));
        clear_sensitive_info(argv[1], strlen(argv[1]));
        if (ret != TRUE) {
            g_printf("Import CRL file failed.\r\n");
            return FALSE;
        }
    }

    
    if (check_crl_path_is_valid(file_info) != RET_OK) {
        g_printf("Import CRL file failed.\r\n");
        return FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(file_info));
    ret = ipmc_call_method_user_info(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, PFN_CLASS_NAME,
        OBJECT_PFN, METHOD_PFN_IMPORT_CRL_FILE, input_list, NULL);
    if (ret != RET_OK) {
        uip_free_gvariant_list(input_list);
        ipmc_delete_file(username, file_info);
        g_printf("Import CRL file failed.\r\n");
        return FALSE;
    }

    uip_free_gvariant_list(input_list);

    ipmc_delete_file(username, file_info);

    g_printf("Import CRL file successfully\r\n");
    return TRUE;
}


gint32 smm_set_standby_smm_ipaddr(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    struct in_addr ipaddr;
    guint32 ip = 0;
    guint32 mask = 0;
    guint32 status = FALSE;
    guint8 slave_addr = 0;

    ret = get_other_board_present(&status);
    if (ret != RET_OK) {
        g_printf("Get standby smm present state failed. This operation cannot be performed.\r\n");
        return FALSE;
    }
    if (status == BOARD_NOT_PRESENT) {
        g_printf("Standby smm is not present. This operation cannot be performed.\r\n");
        return FALSE;
    }

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if ((argc != 3) || strcmp(argv[0], "-v")) {
        g_printf("Usage: ipmcset %s-d othersmmipaddr -v <ipaddr> <mask>\r\n", help_default_name);
        return FALSE;
    }

    ret = get_other_smm_slave_addr(&slave_addr);
    if (ret != RET_OK) {
        g_printf("Get other smm object failed.\r\n");
        return FALSE;
    }

    ret = inet_pton(AF_INET, (const gchar *)argv[1], (void *)&ipaddr.s_addr);
    if (ret <= 0) {
        g_printf("Input parameter format error.\r\n");
        return FALSE;
    }

    ret = inet_pton(AF_INET, (const gchar *)argv[2], (void *)&ipaddr.s_addr);
    if (ret <= 0) {
        g_printf("Input parameter format error.\r\n");
        return FALSE;
    }

    (void)inet_pton(AF_INET, argv[1], (void *)&ip);
    (void)inet_pton(AF_INET, argv[2], (void *)&mask);

    ip = htonl(ip);
    mask = htonl(mask);
    if (mask_judge(mask) != 0) {
        g_printf("Input parameter format error.\r\n");
        return FALSE;
    }

    if ((!(ip & (~mask))) || ((ip | mask) == 0xFFFFFFFF) || (mask >= (ip | mask)) || (!(ip & mask))) {
        g_printf("Input parameter format error.\r\n");
        return FALSE;
    }

    
    ret = set_blade_bmc_ip_config_by_type(slave_addr, argv[1], SET_IP_TYPE_IP_ADDR);
    if (ret != RET_OK) {
        return FALSE;
    }

    
    ret = set_blade_bmc_ip_config_by_type(slave_addr, argv[2], SET_IP_TYPE_IP_SUBMASK);
    if (ret != RET_OK) {
        return FALSE;
    }

    return TRUE;
}


gint32 smm_set_standby_smm_ipaddr_v6(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    errno_t safe_fun_ret;
    struct in6_addr v6ipaddr;
    gint32 ret;
    gchar buff[INET6_ADDRSTRLEN] = { 0 };
    gchar *p = NULL;
    guint8 prefixlen;
    guint32 status = FALSE;
    guint8 slave_addr = 0;

    ret = get_other_board_present(&status);
    if (ret != RET_OK) {
        g_printf("Get standby smm present state failed.\r\n");
        return FALSE;
    }

    if (status == BOARD_NOT_PRESENT) {
        g_printf("Standby smm is not present.\r\n");
        return FALSE;
    }

    ret = get_other_smm_slave_addr(&slave_addr);
    if (ret != RET_OK) {
        g_printf("Get other smm object failed.\r\n");
        return FALSE;
    }

    memset_s((gchar *)&v6ipaddr, sizeof(v6ipaddr), 0, sizeof(v6ipaddr));

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if ((argc != 2) || strcmp(argv[0], "-v")) {
        g_printf("Usage: ipmcset %s-d othersmmipaddr6 -v <ipaddr6/prefixlen(0~128)>\r\n", help_default_name);
        return FALSE;
    }

    memset_s(v6ipaddr.s6_addr, sizeof(struct in6_addr), 0x00, sizeof(struct in6_addr));
    p = strchr(argv[1], '/');
    if (p == NULL) {
        g_printf("Usage: ipmcset %s-d othersmmipaddr6 -v <ipaddr6/prefixlen(0~128)>\r\n", help_default_name);
        return FALSE;
    }

    memset_s(buff, sizeof(buff), 0x00, sizeof(buff));
    safe_fun_ret = memcpy_s(buff, INET6_ADDRSTRLEN - 1, argv[1], strlen(argv[1]) - strlen(p));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        return FALSE;
    }

    ret = inet_pton(AF_INET6, buff, (void *)v6ipaddr.s6_addr);
    if (ret <= 0) {
        g_printf("Input parameter format error.\r\n");
        return FALSE;
    }

    p++;
    if (str_to_int(p) == IPMC_ERROR) {
        g_printf("Usage: ipmcset %s-d othersmmipaddr6 -v <ipaddr6/prefixlen(0~128)>\r\n", help_default_name);
        return FALSE;
    }

    prefixlen = (guint8)str_to_int(p);
    if (prefixlen > 128) {
        g_printf("Usage: ipmcset %s-d othersmmipaddr6 -v <ipaddr6/prefixlen(0~128)>\r\n", help_default_name);
        return FALSE;
    }

    
    if (set_blade_bmc_ip_config_by_type(slave_addr, buff, SET_IP_TYPE_IP6_ADDR) != RET_OK) {
        return FALSE;
    }

    
    if (set_blade_bmc_ip_config_by_type(slave_addr, &prefixlen, SET_IP_TYPE_IP6_PREFIX) != RET_OK) {
        return FALSE;
    }

    return TRUE;
}
