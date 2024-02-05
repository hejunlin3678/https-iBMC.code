

#include "ipmcset_user_security.h"
#include "ipmcset_netconfig.h"
#include "ipmcset_service_mgmt.h"
#include "ipmcset_sys_mgmt.h"

#define SET_USER_ADD "Add User"
#define SET_USER_DEL "Del User"

typedef struct {
    gint32 priv_num;
    gchar priv_str[OBJ_NAME_MAX_LEN];
} USER_PRIVILEGE_MAP;

LOCAL gboolean is_need_reset_passwd(guint32 old_mask, guint32 mask);


gint32 ipmc_get_empty_user_id(guint8 *user_id)
{
    gint32 ret;
    guint32 i;
    gchar user_name[IPMC_USER_NAME_MAX_LEN] = {0};

    if (user_id == NULL) {
        g_printf("Input null pointer.\r\n");
        return RET_ERR;
    }

    ret = get_user_info();
    if (ret == FALSE) {
        g_printf("Get user info failed.\r\n");
        return RET_ERR;
    }

    
    for (i = DEFAULT_USER_ID; i < IPMC_MAX_USER_NUMBER; i++) {
        if (!strcmp(user_name, g_user_pri_info[i].username)) {
            break;
        }
    }

    if (i == IPMC_MAX_USER_NUMBER) {
        return RET_ERR;
    }

    *user_id = i;
    return RET_OK;
}


gint32 add_user_name(guchar user_id, const gchar *user_name)
{
    gint32 iRet;
    gchar obj_name[OBJ_NAME_MAX_LEN] = {0};
    gint32 ret;
    gint32 result;
    GSList *input_list = NULL;
    GSList *output_list = NULL;

    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if (user_name == NULL) {
        g_printf("User name null pointer.\r\n");
        return RET_ERR;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    
    input_list = g_slist_append(input_list, g_variant_new_byte(user_id));
    input_list = g_slist_append(input_list, g_variant_new_string(user_name));
    input_list = g_slist_append(input_list, g_variant_new_string("")); // 添加用户前期望该user id是空闲的
    
    iRet = snprintf_s(obj_name, OBJ_NAME_MAX_LEN, OBJ_NAME_MAX_LEN - 1, "%s%u", CLASS_USER, user_id);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    

    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_USER, obj_name,
        METHOD_USER_SETUSERNAME, input_list, &output_list);
    if (ret != RET_OK) {
        uip_free_gvariant_list(input_list);
        return RET_ERR;
    }

    result = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
    if (result != RET_OK) {
        uip_free_gvariant_list(input_list);
        uip_free_gvariant_list(output_list);
        
        return result;
        
    }

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);
    return RET_OK;
}


gint32 set_user_password(guchar user_id, const gchar *user_passwd)
{
    gint32 iRet;
    gchar obj_name[OBJ_NAME_MAX_LEN] = {0};
    gint32 ret;
    gint32 result;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if (user_passwd == NULL) {
        g_printf("User passwd null pointer.\r\n");
        return RET_ERR;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    
    input_list = g_slist_append(input_list, g_variant_new_byte(user_id));
    input_list = g_slist_append(input_list, g_variant_new_string(user_passwd));
    input_list = g_slist_append(input_list, g_variant_new_byte(IPMC_USER_PASSWORD_LEN - 1));
    input_list = g_slist_append(input_list, g_variant_new_byte(SET_PASSWORD));
    
    iRet = snprintf_s(obj_name, OBJ_NAME_MAX_LEN, OBJ_NAME_MAX_LEN - 1, "%s%u", CLASS_USER, user_id);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_USER, obj_name,
        METHOD_USER_SETPASSWORD, input_list, &output_list);
    if (ret != RET_OK) {
        uip_free_gvariant_list(input_list);
        return RET_ERR;
    }

    result = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
    if (result != RET_OK) {
        if (result == USER_CANNT_SET_SAME_PASSWORD) {
            g_printf("Failed to set the password because the new password is the same as the old password.\n");
        } else if (result == UUSER_USERPASS_TOO_LONG) {
            g_printf("Failed to set the password because the new password is too long.\n");
        } else if (result == COMP_CODE_INVALID_FIELD) {
            g_printf("Failed to set the password because the new password contains invalid characters.\n");
        } else if (result == USER_TIME_LIMIT_UNREASONABLE) {
            g_printf("Failed to set the password because the minimum password age has not expired.\n");
        } else if ((result == USER_PASS_COMPLEXITY_FAIL) || (result == USER_SET_PASSWORD_TOO_WEAK)) {
            print_password_complexity_usage();
        }

        uip_free_gvariant_list(input_list);
        uip_free_gvariant_list(output_list);
        return RET_ERR;
    }

    
    if (strlen(user_passwd) < SNMPV3_PASSWORD_MIN_LENGTH) {
        g_printf("The password is shorter than eight characters, and the user cannot use the SNMPv3 interface.\r\n");
    }
    
    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);
    return RET_OK;
}


gint32 set_user_name(guchar user_id, const gchar *user_name)
{
    gint32 iRet;
    gchar obj_name[OBJ_NAME_MAX_LEN] = {0};
    gint32 ret;
    gint32 result;
    GSList *input_list = NULL;
    GSList *output_list = NULL;

    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if (user_name == NULL) {
        g_printf("User name null pointer.\r\n");
        return RET_ERR;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    
    input_list = g_slist_append(input_list, g_variant_new_byte(user_id));
    input_list = g_slist_append(input_list, g_variant_new_string(user_name));
    
    iRet = snprintf_s(obj_name, OBJ_NAME_MAX_LEN, OBJ_NAME_MAX_LEN - 1, "%s%u", CLASS_USER, user_id);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    

    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_USER, obj_name,
        METHOD_USER_SETUSERNAME, input_list, &output_list);
    if (ret != RET_OK) {
        uip_free_gvariant_list(input_list);
        return RET_ERR;
    }

    result = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
    if (result != RET_OK) {
        uip_free_gvariant_list(input_list);
        uip_free_gvariant_list(output_list);
        
        return result;
        
    }

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);
    return RET_OK;
}


gint32 check_is_only_enabled_admin(guint8 user_id)
{
    guint8 id;
    guint8 tmp_id = 0;
    OBJ_HANDLE obj_handle = 0;
    guchar user_roleid = 0;
    guint8 is_enable = 0;
    gint32 enabled_admin_num = 0;

    
    for (id = 2; id < IPMC_MAX_USER_NUMBER; id++) {
        if (dal_get_specific_object_byte(CLASS_USER, PROPERTY_USER_ID, id, &obj_handle) != RET_OK) {
            continue;
        }
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_USER_ROLEID, &user_roleid);
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_USER_ISENABLE, &is_enable);

        if (user_roleid == PRIVILEGE_LEVEL_ADMINISTRATOR && is_enable) {
            enabled_admin_num += 1;
            tmp_id = id;
        }
        if (enabled_admin_num > 1) {
            return FALSE;
        }
    }

    if (tmp_id == user_id) {
        return TRUE;
    }

    return FALSE;
}


gint32 get_user_login_interface_value(guint32 state, guchar argc, gchar **argv, guint32 *out)
{
    gint32 count;
    guint32 value;
    guint32 dest[USER_LOGIN_INTERFACE_MAX] = {0};
    guint32 n = 0;
    guint32 i;

    // 取原值
    value = *out;

    for (count = 3; count < argc; count++) {
        dest[n] = (guint32)str_to_int(argv[count]);

        if ((dest[n] > USER_LOGIN_INTERFACE_MAX) || (dest[n] < 1) || (strlen(argv[count]) != 1)) {
            return RET_ERR;
        }

        // 遍历检查，不能重复设置接口
        for (i = 0; i < n; i++) {
            if (dest[i] == dest[n]) {
                return RET_ERR;
            }
        }

        n++;
    }

    for (i = 0; i < n; i++) {
        // 改成从 0 开始
        dest[i] = dest[i] - 1;

        if (state == 0) {
            // 关闭
            value = USER_LOGIN_INTERFACE_CLR_BIT(value, dest[i]);
        } else {
            // 开启
            value = USER_LOGIN_INTERFACE_SET_BIT(value, dest[i]);
        }
    }

    *out = value;
    return RET_OK;
}

gint32 add_ipmi_snmp_reset_passwd(guint32 old_mask, guint32 mask, gchar *user_name)
{
    guint8 user_id = IPMC_MAX_USER_NUMBER;

    gchar user_pwd[IPMC_USER_PASSWORD_LEN] = {0};
    if (user_name == NULL) {
        return RET_ERR;
    }

    // 如果有增加IPMI或者SNMP权限需要重置用户密码
    if (is_need_reset_passwd(old_mask, mask)) {
        g_printf("Reset user[%s]\'s ", user_name);
        if (ipmc_get_user_password(user_pwd, sizeof(user_pwd)) != RET_OK) {
            return RET_ERR;
        }
    } else {
        return RET_OK;
    }

    // 重置密码
    if (ipmc_check_user_exist(user_name, &user_id) != RET_OK || user_id == IPMC_MAX_USER_NUMBER) {
        clear_sensitive_info(user_pwd, sizeof(user_pwd));
        g_printf("User %s doesn't exist.\r\n", user_name);
        return RET_ERR;
    }

    if (set_user_password(user_id, user_pwd) != RET_OK) {
        clear_sensitive_info(user_pwd, sizeof(user_pwd));
        g_printf("Reset user password failed.\r\n");
        return RET_ERR;
    }

    clear_sensitive_info(user_pwd, sizeof(user_pwd));
    return RET_OK;
}


LOCAL gboolean is_need_reset_passwd(guint32 old_mask, guint32 mask)
{
    // 新增ipmi权限
    if ((USER_LOGIN_INTERFACE_GET_BIT(old_mask, USER_LOGIN_INTERFACE_IPMI_OFFSET) == 0) &&
        (USER_LOGIN_INTERFACE_GET_BIT(mask, USER_LOGIN_INTERFACE_IPMI_OFFSET) == 1)) {
        return TRUE;
    }

    return FALSE;
}


gint32 log_ipmcset_user_privilege(guchar argc, gchar **argv, gchar *user_name, gchar *log_info)
{
    gchar priv_str[OBJ_NAME_MAX_LEN] = {0};
    guint32 i;
    guint8 user_id = 0;
    guint32 ret = 0;
    const USER_PRIVILEGE_MAP privilege_map[] = {
        {UUSER_ROLE_ADMIN, USER_ROLE_ADMIN},
        {UUSER_ROLE_OPER, USER_ROLE_OPERATOR},
        {UUSER_ROLE_USER, USER_ROLE_COMMONUSER},
        {UUSER_ROLE_NOACCESS, USER_ROLE_NOACCESS},
        {UUSER_ROLE_CUSt1, USER_ROLE_CUSTOMROLE1},
        {UUSER_ROLE_CUSt2, USER_ROLE_CUSTOMROLE2},
        {UUSER_ROLE_CUSt3, USER_ROLE_CUSTOMROLE3},
        {UUSER_ROLE_CUSt4, USER_ROLE_CUSTOMROLE4}
    };

    
    if (strlen(user_name) > USER_NAME_MAX_LEN) {
        user_name[USER_NAME_MAX_LEN] = '*';
        user_name[USER_NAME_MAX_LEN + 1] = '\0';
    }

    if (argv[2] == NULL) {
        debug_log(DLOG_ERROR, "%s: argv[2] is null", __FUNCTION__);
        return RET_ERR;
    }

    gint32 priv_num = -1;
    if (dal_strtoi(argv[2], &priv_num, NUMBER_BASE10) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_strtoi failed", __FUNCTION__);
        return RET_ERR;
    }

    strcpy_s(priv_str, sizeof(priv_str), "unknown");
    for (i = 0; i < sizeof(privilege_map) / sizeof(USER_PRIVILEGE_MAP); i++) {
        if (priv_num == privilege_map[i].priv_num) {
            memset_s(priv_str, sizeof(priv_str), 0, sizeof(priv_str));
            ret = strcpy_s(priv_str, sizeof(priv_str), privilege_map[i].priv_str);
            if (ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strcpy_s fail, ret = %d", __FUNCTION__, ret);
            }
            break;
        }
    }
    memset_s(log_info, MAX_OPERATE_LOG_LEN + 1, 0, MAX_OPERATE_LOG_LEN + 1);
    snprintf_truncated_s(log_info, MAX_OPERATE_LOG_LEN + 1, "Set user(%s) privilege to (%s) failed", user_name,
        priv_str);

    if (ipmc_check_user_exist(user_name, &user_id) == RET_OK && user_id != IPMC_MAX_USER_NUMBER) {
        memset_s(log_info, MAX_OPERATE_LOG_LEN + 1, 0, MAX_OPERATE_LOG_LEN + 1);
        snprintf_truncated_s(log_info, MAX_OPERATE_LOG_LEN + 1, "Set user(%s|user%u) privilege to (%s) failed",
            user_name, user_id, priv_str);
    }

    return RET_OK;
}


gint32 set_user_privilege(guchar user_id, guchar user_privilege, guchar user_roleid)
{
    gint32 iRet;
    gchar obj_name[OBJ_NAME_MAX_LEN] = {0};
    gint32 ret;
    gint32 result;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(user_id));
    input_list = g_slist_append(input_list, g_variant_new_byte(user_privilege));
    input_list = g_slist_append(input_list, g_variant_new_byte(user_roleid));
    
    iRet = snprintf_s(obj_name, OBJ_NAME_MAX_LEN, OBJ_NAME_MAX_LEN - 1, "%s%u", CLASS_USER, user_id);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    

    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_USER, obj_name,
        METHOD_USER_SETPRIVILEGE, input_list, &output_list);
    if (ret != RET_OK) {
        uip_free_gvariant_list(input_list);
        return RET_ERR;
    }

    result = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
    if (result != RET_OK) {
        uip_free_gvariant_list(input_list);
        uip_free_gvariant_list(output_list);
        return RET_ERR;
    }

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);
    return RET_OK;
}


gint32 set_user_roleid(guchar user_id, guchar user_roleid)
{
    gint32 iRet;
    gchar obj_name[OBJ_NAME_MAX_LEN] = {0};
    gint32 ret;
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(user_roleid));
    
    iRet = snprintf_s(obj_name, OBJ_NAME_MAX_LEN, OBJ_NAME_MAX_LEN - 1, "%s%u", CLASS_USER, user_id);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_USER, obj_name,
        METHOD_USER_SETUSERROLEID, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    return RET_OK;
}


gint32 set_weak_pwddic_state(guint8 state)
{
    gint32 ret;
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    
    if (((state != SERVICE_STATE_DISABLE) && (state != SERVICE_STATE_ENABLE))) {
        g_printf("Set weak password dictionary check status failed.\r\n");
        return FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(state));
    if (input_list == NULL) {
        g_printf("%s weak password dictionary check failed.\r\n",
            (state == SERVICE_STATE_ENABLE) ? "Enable" : "Disable");
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_SECURITY_ENHANCE,
        OBJ_SECURITY_ENHANCE, METHOD_SECURITY_SET_WEAK_PWDDICT_STATE, input_list, NULL);

    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        g_printf("%s weak password dictionary check failed.\r\n",
            (state == SERVICE_STATE_ENABLE) ? "Enable" : "Disable");
        return FALSE;
    }

    g_printf("%s weak password dictionary check successfully.\r\n",
        (state == SERVICE_STATE_ENABLE) ? "Enable" : "Disable");

    return ret;
}


gint32 export_weak_pwddic_config(gchar *argv_path)
{
    guint8 local_path = FALSE;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    GSList *input_list = NULL;
    gint32 ret;

    if (argv_path == NULL) {
        g_printf("Export file path error.\r\n");
        return FALSE;
    }

    
    if ((strstr(argv_path, FILE_URL_HTTPS_HEAD) == NULL) && (strstr(argv_path, FILE_URL_SFTP_HEAD) == NULL) &&
        (strstr(argv_path, FILE_URL_NFS_HEAD) == NULL) && (strstr(argv_path, FILE_URL_CIFS_HEAD) == NULL) &&
        (strstr(argv_path, FILE_URL_SCP_HEAD) == NULL)) {
        
        if (((strlen(argv_path) == 0) || dal_check_real_path2(argv_path, TMP_PATH_WITH_SLASH) != RET_OK)) {
            g_printf("Export file path error.\r\n");
            return RET_ERR;
        }

        local_path = TRUE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    
    if (local_path == TRUE) {
        input_list = g_slist_append(input_list, g_variant_new_string(argv_path));
    } else {
        input_list = g_slist_append(input_list, g_variant_new_string(WEAK_PWDDICT_FILE_TMP_PATH));
    }

    ret = ipmc_call_method_user_info(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
        CLASS_SECURITY_ENHANCE, OBJ_SECURITY_ENHANCE, METHOD_SECURITY_EXPORT_WEAK_PWDDICT, input_list, NULL);

    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        g_printf("Export weak password dictionary failed.\r\n");
        return FALSE;
    }

    
    if (local_path != TRUE) {
        ret = upload_file_to_url(argv_path, FILE_ID_WEAK_PWD_DICT);
        clear_sensitive_info(argv_path, strlen(argv_path));
        (void)dal_delete_file(WEAK_PWDDICT_FILE_TMP_PATH);
        if (ret != TRUE) {
            g_printf("Export weak password dictionary failed.\r\n");
            return FALSE;
        }
    }

    g_printf("Export weak password dictionary successfully.\r\n");
    return TRUE;
}


gint32 import_weak_pwddic_config(gchar *argv_path)
{
    errno_t safe_fun_ret = EOK;
    gint32 ret;
    gchar file_path[MAX_FILEPATH_LENGTH + 1] = {0};
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    GSList *input_list = NULL;

    if (argv_path == NULL) {
        g_printf("Import file path error.\r\n");
        return FALSE;
    }

    
    
    if ((strstr(argv_path, FILE_URL_HTTPS_HEAD) == NULL) && (strstr(argv_path, FILE_URL_SFTP_HEAD) == NULL) &&
        (strstr(argv_path, FILE_URL_NFS_HEAD) == NULL) && (strstr(argv_path, FILE_URL_CIFS_HEAD) == NULL) &&
        (strstr(argv_path, FILE_URL_SCP_HEAD) == NULL)) {
        
        
        if (g_file_test(argv_path, G_FILE_TEST_IS_REGULAR) != TRUE || dal_check_real_path(argv_path) != RET_OK) {
            g_printf("File path is invalid.\r\n");
            return FALSE;
        }

        if (strlen(argv_path) > MAX_FILEPATH_LENGTH) {
            g_printf("File path is too long.\r\n");
            return FALSE;
        }

        safe_fun_ret = strncpy_s(file_path, MAX_FILEPATH_LENGTH + 1, argv_path, MAX_FILEPATH_LENGTH);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        
    } else {
        ret = get_file_path_from_url(argv_path, FILE_ID_WEAK_PWD_DICT, file_path, sizeof(file_path));
        clear_sensitive_info(argv_path, strlen(argv_path));
        if (ret != TRUE) {
            g_printf("Get file from URL failed.\r\n");
            return FALSE;
        }
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    
    input_list = g_slist_append(input_list, g_variant_new_string(file_path));
    ret = ipmc_call_method_user_info(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
        CLASS_SECURITY_ENHANCE, OBJ_SECURITY_ENHANCE, METHOD_SECURITY_IMPORT_WEAK_PWDDICT, input_list, NULL);
    uip_free_gvariant_list(input_list);
    ipmc_delete_file(username, file_path);

    if (ret != RET_OK) {
        g_printf("Import weak password dictionary failed.\r\n");
        return FALSE;
    }

    g_printf("Import weak password dictionary successfully.\r\n");

    return TRUE;
}

gint32 blade_set_user_password(gchar *location, const gchar *user_name, const gchar *password)
{
    GSList *input_list = NULL;
    guint8 slot_id;
    gint32 ret;
    gchar user[PARAMETER_LEN + 1] = { 0 };
    gchar ip[PARAMETER_LEN + 1] = { 0 };
    guint8 slave_addr;
    
    gchar blade_name[OBJ_NAME_MAX_LEN] = { 0 };
    

    if (get_username_ip(user, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(user, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    slot_id = ipmc_get_blade_num(location);
    if (slot_id == 0xff) {
        return FALSE;
    }
    slave_addr = ipmc_get_blade_ipmbaddr(slot_id);
    if (get_ipmbeth_blade_name(slave_addr, blade_name, sizeof(blade_name)) != RET_OK) {
        return FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(slave_addr));
    input_list = g_slist_append(input_list, g_variant_new_string(user_name));
    input_list = g_slist_append(input_list, g_variant_new_string(password));

    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)user, (const gchar *)ip, CLASS_NAME_IPMBETH_BLADE,
        blade_name, METHOD_SET_BLADE_BMC_USERPWD, input_list, NULL);

    uip_free_gvariant_list(input_list);

    return ret;
}

gint32 blade_set_user(gchar *location, const gchar *user_name, gchar *type)
{
    GSList *input_list = NULL;
    guint8 slot_id;
    gint32 ret = 0;
    gchar user[PARAMETER_LEN + 1] = { 0 };
    gchar ip[PARAMETER_LEN + 1] = { 0 };
    guint8 slave_addr;
    
    gchar blade_name[OBJ_NAME_MAX_LEN] = { 0 };
    

    if (get_username_ip(user, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(user, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    slot_id = ipmc_get_blade_num(location);
    if (slot_id == 0xff) {
        return FALSE;
    }

    slave_addr = ipmc_get_blade_ipmbaddr(slot_id);
    if (get_ipmbeth_blade_name(slave_addr, blade_name, sizeof(blade_name)) != RET_OK) {
        return FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(slave_addr));
    input_list = g_slist_append(input_list, g_variant_new_string(user_name));
    if (!strcmp(type, SET_USER_ADD)) {
        ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)user, (const gchar *)ip, CLASS_NAME_IPMBETH_BLADE,
            blade_name, METHOD_SET_BLADE_BMC_ADDUSER, input_list, NULL);
    } else if (!strcmp(type, SET_USER_DEL)) {
        ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)user, (const gchar *)ip, CLASS_NAME_IPMBETH_BLADE,
            blade_name, METHOD_SET_BLADE_BMC_DELUSER, input_list, NULL);
    }

    uip_free_gvariant_list(input_list);

    return ret;
}


gint32 get_user_info(void)
{
    gint32 iRet = -1;
    guchar i;
    gint32 ret = 0;
    guchar user_priv = 0;
    const gchar *user_name = NULL;
    g_user_pri_info[1].user_pri = 0xf;
    gchar obj_name[OBJ_NAME_MAX_LEN] = {0};
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;
    errno_t sec_ret;

    for (i = 1; i < IPMC_MAX_USER_NUMBER; i++) {
        memset_s(obj_name, OBJ_NAME_MAX_LEN, 0, OBJ_NAME_MAX_LEN);
        
        iRet = snprintf_s(obj_name, OBJ_NAME_MAX_LEN, OBJ_NAME_MAX_LEN - 1, "%s%u", CLASS_USER, i);
        if (iRet <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
        }
        

        
        property_name_list = g_slist_append(property_name_list, PROPERTY_USER_NAME);
        property_name_list = g_slist_append(property_name_list, PROPERTY_USER_PRIVILEGE);
        ret = uip_multiget_object_property(CLASS_USER, obj_name, property_name_list, &property_value);
        g_slist_free(property_name_list);
        property_name_list = NULL;
        if (ret != RET_OK) {
            return FALSE;
        } else {
            
            
            user_name = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 0), 0);
            memset_s(g_user_pri_info[i].username, IPMC_USER_NAME_MAX_LEN, 0, IPMC_USER_NAME_MAX_LEN);
            
            // 用户模块已支持32位用户名,当前模块仍做16位用户名限制,需要将用户名截断
            sec_ret = strncpy_s(g_user_pri_info[i].username, IPMC_USER_NAME_MAX_LEN, user_name,
                                IPMC_USER_NAME_MAX_LEN - 1);
            if (sec_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strcpy_s failed, ret = %d", __FUNCTION__, sec_ret);
            }

            
            user_priv = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 1));
            g_user_pri_info[i].user_pri = user_priv;
            uip_free_gvariant_list(property_value);
            property_value = NULL;
        }
    }

    return TRUE;
}

#ifndef _WIN32
// Linux platform
#include <termio.h>
#ifndef STDIN_FILENO
#define STDIN_FILENO 0
#endif

LOCAL gint32 getch(void)
{
    struct termios cu_time, tm_old;
    gint32 fd = STDIN_FILENO, c;

    if (tcgetattr(fd, &cu_time) < 0) {
        return -1;
    }

    tm_old = cu_time;
    cfmakeraw(&cu_time);

    if (tcsetattr(fd, TCSANOW, &cu_time) < 0) {
        return -1;
    }

    c = fgetc(stdin);
    if (tcsetattr(fd, TCSANOW, &tm_old) < 0) {
        return -1;
    }

    return c;
}

#else
// WIN32 platform
#include <conio.h>
#endif


LOCAL gint32 passwd_judge(gint32 ch)
{
    if (((ch >= 26) && (ch <= 127)) || (ch == BACKSPACE)) {
        return TRUE;
    } else {
        return FALSE;
    }
}

gchar *getPasswd(const gchar *prompt)
{
    gint32 i = 0, ch;
    static gchar p[IPMC_USER_PASSWORD_LEN] = "";

    g_printf("%s", prompt);

    while (((ch = getch()) != -1) && (ch != ENTER) && passwd_judge(ch)) {
        if ((i == (IPMC_USER_PASSWORD_LEN - 1)) && (ch != BACKSPACE) && (ch != 0x7f)) {
            
            
            g_printf("\nThe length of user password is too long.\r\n");
            g_printf("User password should less than 21 characters.\r\n");
            memset_s(p, IPMC_USER_PASSWORD_LEN, 0, IPMC_USER_PASSWORD_LEN);
            return NULL;
            
        }

        if ((ch == BACKSPACE) || (ch == 0x7f)) {
            if (i == 0) {
                (void)putchar(ALARM);
                continue;
            }

            i--;
        } else {
            p[i] = (gchar)ch;
            i++;
        }
    }

    if (ch == -1) {
        memset_s(p, IPMC_USER_PASSWORD_LEN, 0, IPMC_USER_PASSWORD_LEN);
        return NULL;
    }

    p[IPMC_USER_PASSWORD_LEN - 1] = '\0';

    g_printf("\n");
    return p;
}
