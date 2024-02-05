

#include "ipmcget_user_security.h"


gint32 ipmc_get_userinfo(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    gint32 uid_level = 0;

    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    ret = get_userinfo_level(privilege, rolepriv, &uid_level);
    if (ret != TRUE) {
        g_printf("Insufficient privilege level\r\n");
        return FALSE;
    }

    
    ret = print_user_info_list(uid_level);
    if (ret != RET_OK) {
        g_printf("Get user info failed.\r\n");
        return FALSE;
    }

    return TRUE;
}


gint32 ipmc_get_pass_complexity_check_enable(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target)
{
    gint32 ret;
    GVariant *value = NULL;
    guchar enable_state = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcget %s-d passwordcomplexity \r\n", help_default_name);
        return FALSE;
    }

    ret = uip_get_object_property(CLASS_PASSWD_SETTING, CLASS_PASSWD_SETTING, PROPERTY_USER_PASSWD_SETTING, &value);
    if (ret != RET_OK) {
        g_printf("Get password complexity check state failed.\r\n");
        return FALSE;
    } else {
        enable_state = g_variant_get_byte(value);
        g_printf("Password complexity check state : %s\r\n", ((enable_state == 0) ? "disabled" : "enabled"));
    }

    g_variant_unref(value);
    return TRUE;
}


gint32 ipmc_get_minimum_pwd_age(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    GVariant *value = NULL;
    guint32 minimum_age = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcget -d minimumpasswordage\r\n");
        return FALSE;
    }

    ret = uip_get_object_property(CLASS_SECURITY_ENHANCE, CLASS_SECURITY_ENHANCE, PROPERTY_SECURITY_MINIMUMPWDAGE,
        &value);
    if (ret != RET_OK) {
        g_printf("Usage: ipmcget -d minimumpasswordage\r\n");
        return FALSE;
    } else {
        minimum_age = g_variant_get_uint32(value);
        g_printf("Minimum password age: %d\r\n", minimum_age);
    }

    g_variant_unref(value);
    return TRUE;
}


gint32 ipmc_get_usermgnt(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GVariant *value = NULL;
    guint8 status = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcget -t user -d usermgmtbyhost\r\n");
        return FALSE;
    }

    if (dal_get_object_handle_nth(CLASS_SECURITY_ENHANCE, 0, &obj_handle) != RET_OK) {
        g_printf("Failed to query the BMC user management function enabling status on the host side.\r\n");
        return FALSE;
    }

    ret = uip_get_object_property(CLASS_SECURITY_ENHANCE, OBJ_SECURITY_ENHANCE, PROPERTY_ENABLE_USER_MGNT, &value);
    if (ret != RET_OK) {
        g_printf("Failed to query the BMC user management function enabling status on the host side.\r\n");
        return FALSE;
    } else {
        status = g_variant_get_byte(value);
        switch (status) {
            case USER_MGNT_ENABLE:
                g_printf("Enable\r\n");
                break;

            default:
                g_printf("Disable\r\n");
                break;
        }

        g_variant_unref(value);
        return TRUE;
    }
}


gint32 ipmc_get_ssh_password_authentication(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target)
{
    gint32 ret;
    GVariant *value = NULL;
    guchar enable_state = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcget -d sshpasswordauthentication \r\n");
        return FALSE;
    }

    ret = uip_get_object_property(CLASS_SECURITY_ENHANCE, OBJ_SECURITY_ENHANCE,
        PROPERTY_SECURITY_ENHANCE_PASSWORD_AUTHENTICATION, &value);
    if (ret != RET_OK) {
        g_printf("Get SSH password authentication state failed.\r\n");
        return FALSE;
    } else {
        enable_state = g_variant_get_byte(value);
        g_printf("SSH Password Authentication : %s\r\n", ((enable_state == 0) ? "disabled" : "enabled"));
    }

    g_variant_unref(value);
    return TRUE;
}


gint32 ipmc_get_security_banner_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    OBJ_HANDLE object_handle = 0;
    guint8 enable = 0;
    gchar content[MOTD_FILE_MAX_LEN + 1] = {0};

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc) {
        g_printf("Usage: ipmcget -t securitybanner -d info\r\n");
        return FALSE;
    }

    ret = dal_get_object_handle_nth(CLASS_SECURITY_ENHANCE, 0, &object_handle);
    if (ret != RET_OK) {
        g_printf("Command error: object not exist.\r\n");
        return FALSE;
    }

    ret = dal_get_property_value_byte(object_handle, PROPERTY_SECURITY_BANNERSTATE, &enable);
    if (ret != RET_OK) {
        g_printf("Command error: get property failed.\r\n");
        return FALSE;
    }

    ret = dal_get_property_value_string(object_handle, PROPERTY_SECURITY_BANNERCONTENT, content, sizeof(content));
    if (ret != RET_OK) {
        g_printf("Command error: get property failed.\r\n");
        return FALSE;
    }

    g_printf("Login security banner information state: %s.\r\n", enable == STATE_ENABLE ? "enabled" : "disabled");

    g_printf("\r\n");

    g_printf("Login security banner information:\r\n");

    g_printf("%s\r\n", content);

    return TRUE;
}


gint32 ipmc_get_certificate_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    OBJ_HANDLE obj_handle;
    GSList *owner_property_name_list = NULL;
    GSList *owner_property_value_list = NULL;
    GSList *issuer_property_name_list = NULL;
    GSList *issuer_property_value_list = NULL;

    const gchar *owner_common_name = NULL;
    const gchar *owner_org_unit = NULL;
    const gchar *owner_org_name = NULL;
    const gchar *owner_location = NULL;
    const gchar *owner_state = NULL;
    const gchar *owner_country = NULL;
    const gchar *issuer_common_name = NULL;
    const gchar *issuer_org_unit = NULL;
    const gchar *issuer_org_name = NULL;
    const gchar *issuer_location = NULL;
    const gchar *issuer_state = NULL;
    const gchar *issuer_country = NULL;

    const gchar *owner_start_time = NULL;
    const gchar *owner_expiration = NULL;
    const gchar *owner_serial_number = NULL;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcget -t certificate -d info \r\n");
        return FALSE;
    }

    
    ret = dal_get_specific_object_byte(SSL_CLASEE_NAME, SSL_CERT_ID, 0x02, &obj_handle);
    if (ret != RET_OK) {
        g_printf("Get owner SSL object handle Failed. \n");
        return FALSE;
    }

    owner_property_name_list = g_slist_append(owner_property_name_list, SSL_COMMON_NAME);
    owner_property_name_list = g_slist_append(owner_property_name_list, SSL_ORG_UNIT);
    owner_property_name_list = g_slist_append(owner_property_name_list, SSL_ORG_NAME);
    owner_property_name_list = g_slist_append(owner_property_name_list, SSL_LOCATION);
    owner_property_name_list = g_slist_append(owner_property_name_list, SSL_STATE);
    owner_property_name_list = g_slist_append(owner_property_name_list, SSL_COUNRTY);
    owner_property_name_list = g_slist_append(owner_property_name_list, SSL_START_TIME);
    owner_property_name_list = g_slist_append(owner_property_name_list, SSL_EXPIRATION);
    owner_property_name_list = g_slist_append(owner_property_name_list, SSL_SN);
    ret = dfl_multiget_property_value(obj_handle, owner_property_name_list, &owner_property_value_list);
    if (ret != DFL_OK) {
        g_printf("Get owner SSL multiget property Failed. \n");
        g_slist_free(owner_property_name_list);
        return FALSE;
    }

    owner_common_name = g_variant_get_string((GVariant *)g_slist_nth_data(owner_property_value_list, 0), 0);
    owner_org_unit = g_variant_get_string((GVariant *)g_slist_nth_data(owner_property_value_list, 1), 0);
    owner_org_name = g_variant_get_string((GVariant *)g_slist_nth_data(owner_property_value_list, 2), 0);
    owner_location = g_variant_get_string((GVariant *)g_slist_nth_data(owner_property_value_list, 3), 0);
    owner_state = g_variant_get_string((GVariant *)g_slist_nth_data(owner_property_value_list, 4), 0);
    owner_country = g_variant_get_string((GVariant *)g_slist_nth_data(owner_property_value_list, 5), 0);
    owner_start_time = g_variant_get_string((GVariant *)g_slist_nth_data(owner_property_value_list, 6), 0);
    owner_expiration = g_variant_get_string((GVariant *)g_slist_nth_data(owner_property_value_list, 7), 0);
    owner_serial_number = g_variant_get_string((GVariant *)g_slist_nth_data(owner_property_value_list, 8), 0);

    
    ret = dal_get_specific_object_byte(SSL_CLASEE_NAME, SSL_CERT_ID, 0x03, &obj_handle);
    if (ret != RET_OK) {
        g_printf("Get issuer SSL handle Failed. \n");
        g_slist_free(owner_property_name_list);
        uip_free_gvariant_list(owner_property_value_list);

        return FALSE;
    }

    issuer_property_name_list = g_slist_append(issuer_property_name_list, SSL_COMMON_NAME);
    issuer_property_name_list = g_slist_append(issuer_property_name_list, SSL_ORG_UNIT);
    issuer_property_name_list = g_slist_append(issuer_property_name_list, SSL_ORG_NAME);
    issuer_property_name_list = g_slist_append(issuer_property_name_list, SSL_LOCATION);
    issuer_property_name_list = g_slist_append(issuer_property_name_list, SSL_STATE);
    issuer_property_name_list = g_slist_append(issuer_property_name_list, SSL_COUNRTY);
    ret = dfl_multiget_property_value(obj_handle, issuer_property_name_list, &issuer_property_value_list);
    if (ret != DFL_OK) {
        g_printf("Get issuer SSL multiget property Failed. \n");
        g_slist_free(issuer_property_name_list);
        g_slist_free(owner_property_name_list);
        uip_free_gvariant_list(owner_property_value_list);
        return FALSE;
    }

    issuer_common_name = g_variant_get_string((GVariant *)g_slist_nth_data(issuer_property_value_list, 0), 0);
    issuer_org_unit = g_variant_get_string((GVariant *)g_slist_nth_data(issuer_property_value_list, 1), 0);
    issuer_org_name = g_variant_get_string((GVariant *)g_slist_nth_data(issuer_property_value_list, 2), 0);
    issuer_location = g_variant_get_string((GVariant *)g_slist_nth_data(issuer_property_value_list, 3), 0);
    issuer_state = g_variant_get_string((GVariant *)g_slist_nth_data(issuer_property_value_list, 4), 0);
    issuer_country = g_variant_get_string((GVariant *)g_slist_nth_data(issuer_property_value_list, 5), 0);

    g_printf("SSL Certificate Information: \n");
    g_printf("Issued     To: CN=%s, OU=%s, O=%s, L=%s, S=%s, C=%s\n", owner_common_name, owner_org_unit, owner_org_name,
        owner_location, owner_state, owner_country);
    g_printf("Issued     By: CN=%s, OU=%s, O=%s, L=%s, S=%s, C=%s\n", issuer_common_name, issuer_org_unit,
        issuer_org_name, issuer_location, issuer_state, issuer_country);
    g_printf("Valid    From: %s\n", owner_start_time);
    g_printf("Valid      To: %s\n", owner_expiration);
    g_printf("Serial Number: %s\n", owner_serial_number);

    g_slist_free(owner_property_name_list);
    uip_free_gvariant_list(owner_property_value_list);

    g_slist_free(issuer_property_name_list);
    uip_free_gvariant_list(issuer_property_value_list);

    return TRUE;
}


gint32 ipmc_get_inactive_timelimit(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    GVariant *value = NULL;
    guint32 inact_user_timeout = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcget -t securityenhance -d inactivetimelimit\r\n");
        return FALSE;
    }

    ret = uip_get_object_property(CLASS_SECURITY_ENHANCE, OBJ_SECURITY_ENHANCE, PROPERTY_SECURITY_USER_INACT_TIME_LIMIT,
        &value);
    if (ret != RET_OK) {
        g_printf("Usage: ipmcget -t securityenhance -d inactivetimelimit\r\n");
        return FALSE;
    } else {
        inact_user_timeout = g_variant_get_uint32(value);
        g_printf("User inactive timelimit: %d\r\n", inact_user_timeout);
    }

    g_variant_unref(value);
    return TRUE;
}


gint32 ipmc_get_update_interval(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    GVariant *value = NULL;
    guint32 update_interval = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcget -t securityenhance -d masterkeyupdateinterval\r\n");
        return FALSE;
    }

    ret = uip_get_object_property(CLASS_SECURITY_ENHANCE, OBJ_SECURITY_ENHANCE,
        PROPERTY_SECURITY_MASTERKEY_UPDATE_INTERVAL, &value);
    if (ret != RET_OK) {
        g_printf("Usage: ipmcget -t securityenhance -d masterkeyupdateinterval\r\n");
        return FALSE;
    } else {
        update_interval = g_variant_get_uint32(value);
        g_printf("Master key update interval: %d\r\n", update_interval);
    }

    g_variant_unref(value);
    return TRUE;
}
