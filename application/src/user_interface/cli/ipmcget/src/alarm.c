

#include "ipmcget_alarm.h"

const gchar* trap_severitystr[] = {
    "normal",
    "minor",
    "major",
    "critical",
};
const gchar* syslog_severitystr[] = {
    "normal",
    "minor",
    "major",
    "critical",
};
const gchar* syslog_identitystr[] = {
    "board serial number",
    "product asset tag ",
    "host name",
    "chassis name",
    "chassis location",
    "chassis serial number",
};


gint32 ipmc_get_allsensorinfo(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    guint32 discrete_sensor_count = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcget -t sensor -d list\r\n");
        return FALSE;
    }

    g_printf("%-10s | %-16s | %-10s | %-12s | %-6s | %-10s | %-10s | %-10s | %-8s | %-10s | %-10s | %-6s | %-6s\r\n",
        "sensor id", "sensor name", "value", "unit", "status", "lnr", "lc", "lnc", "unc", "uc", "unr", "phys", "nhys");
    ret = dfl_foreach_object(CLASS_THRESHOLD_SENSOR, print_analog_sensor_info, NULL, NULL);
    if (ret != DFL_OK) {
        g_printf("Get analog sensor list failed.!\r\n");
        return FALSE;
    }

    
    (void)dfl_get_object_count(CLASS_DISCERETE_SENSOR, &discrete_sensor_count);
    if (discrete_sensor_count == 0) {
        return TRUE;
    }
    

    ret = dfl_foreach_object(CLASS_DISCERETE_SENSOR, print_discrete_sensor_info, NULL, NULL);
    if (ret != DFL_OK) {
        g_printf("Get discrete sensor list failed.\r\n");
        return FALSE;
    }

    return TRUE;
}


gint32 ipmc_get_trapenable(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0 && (argc != 2 || strcmp(argv[0], "-v"))) {
        print_ipmcget_trap_dest_state();
        return FALSE;
    }

    // 查询Trap dest使能
    if (argc == 2) {
        gint32 dest = -1;
        if (argv[1] == NULL || index(argv[1], '.') != NULL || dal_strtoi(argv[1], &dest, NUMBER_BASE10) != RET_OK
            || dest < 1 || dest > 4) {
            print_ipmcget_trap_dest_state();
            return FALSE;
        }

        ret = get_trap_dest_state(dest);
    } else {
        ret = get_trap_state();
    }

    return ret;
}


gint32 ipmc_get_trapiteminfo(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcget -t trap -d trapiteminfo\r\n");
        return FALSE;
    }

    g_printf("\n%-15s | %-10s | %-10s | %-20s\r\n", "TrapItem Num", "state", "port", "alert address");
    ret = dfl_foreach_object(CLASS_TRAP_ITEM_CONFIG, print_trapitem_info, NULL, NULL);
    if (ret != DFL_OK) {
        g_printf("Get trapitem information failed.\r\n");
        return FALSE;
    }

    return TRUE;
}


gint32 ipmc_get_trap_version(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    guchar version = 0;
    gchar ver[PROTERY_VALUE_LEN] = {0};
    GVariant *value = NULL;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc) {
        g_printf("Usage: ipmcget -t trap -d version\r\n");
        return FALSE;
    }

    ret = uip_get_object_property(CLASS_TRAP_CONFIG, OBJECT_TRAP_CONFIG, PROPERTY_TRAP_VERSION, &value);
    if (ret != RET_OK) {
        g_printf("Get trap version failed.\r\n ");
        return FALSE;
    } else {
        version = g_variant_get_byte(value);
        if (version == TRAP_VER_V1) {
            
            (void)strncpy_s(ver, PROTERY_VALUE_LEN, "V1", strlen("V1"));
            
        } else if (version == TRAP_VER_V2C) {
            
            (void)strncpy_s(ver, PROTERY_VALUE_LEN, "V2C", strlen("V2C"));
            
        } else if (version == TRAP_VER_V3) {
            
            (void)strncpy_s(ver, PROTERY_VALUE_LEN, "V3", strlen("V3"));
            
        } else {
            g_printf("Get trap version failed.\r\n");
            g_variant_unref(value);
            return FALSE;
        }

        g_printf("Trap version : %s\r\n", ver);
    }

    g_variant_unref(value);
    return TRUE;
}


gint32 ipmc_get_trap_severity(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    guchar data = 0;
    guint32 i = 0;
    GVariant *value = NULL;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc) {
        g_printf("Usage: ipmcget -t trap -d severity\r\n");
        return FALSE;
    }

    ret = uip_get_object_property(CLASS_TRAP_CONFIG, OBJECT_TRAP_CONFIG, PROPERTY_TRAP_SEND_SEVERITY, &value);
    if (ret != RET_OK) {
        g_printf("Get trap severity failed.\r\n ");
        return FALSE;
    } else {
        data = g_variant_get_byte(value);
        g_printf("Trap severity : ");

        if (data == 0) {
            g_printf(" %s ", "none");
        } else if (data == 0x0F) {
            g_printf(" %s ", "all");
        } else {
            for (i = 0; i < 4; i++) {
                if (data & (0x01 << i)) {
                    g_printf(" %s ", trap_severitystr[i]);
                }
            }
        }
    }

    g_printf("\r\n");

    g_variant_unref(value);
    return TRUE;
}


gint32 ipmc_get_trap_user(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    guint8 user_id;
    const gchar *user_name = NULL;
    GVariant *property_value = NULL;
    OBJ_HANDLE obj_handle = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc) {
        g_printf("Usage: ipmcget -t trap -d user\r\n");
        return FALSE;
    }

    ret = uip_get_object_property(CLASS_TRAP_CONFIG, OBJECT_TRAP_CONFIG, PROPERTY_TRAPV3_USERID, &property_value);
    if (ret != RET_OK) {
        g_printf("Get trap user id failed.\r\n ");
        return FALSE;
    }

    user_id = g_variant_get_byte(property_value);
    g_variant_unref(property_value);
    property_value = NULL;
    property_value = g_variant_new_byte(user_id);
    ret = dfl_get_specific_object(CLASS_USER, PROPERTY_USER_ID, property_value, &obj_handle);
    g_variant_unref(property_value);
    property_value = NULL;
    if (ret != DFL_OK) {
        if (ret == ERRCODE_OBJECT_NOT_EXIST) {
            g_printf("User id is not exist.\r\n ");
            return FALSE;
        }

        g_printf("Get object handle failed.\r\n ");
        return FALSE;
    }

    ret = uip_get_object_property(CLASS_USER, dfl_get_object_name(obj_handle), PROPERTY_USER_NAME, &property_value);
    if (ret != RET_OK) {
        g_printf("Get trap user name failed.\r\n ");
        return FALSE;
    }

    user_name = g_variant_get_string(property_value, 0);
    g_printf("Trap user : %s\r\n", user_name);
    g_variant_unref(property_value);
    return TRUE;
}



gint32 ipmc_get_trap_mode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    errno_t safe_fun_ret = EOK;
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guchar mode = 0;
    gchar trap_mode[PROTERY_VALUE_LEN] = {0};

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc) {
        g_printf("Usage: ipmcget -t trap -d mode\r\n");
        return FALSE;
    }

    ret = dal_get_object_handle_nth(CLASS_TRAP_CONFIG, 0, &obj_handle);
    if (ret != RET_OK) {
        g_printf("Get trap mode failed.\r\n ");
        return FALSE;
    } else {
        memset_s(trap_mode, PROTERY_VALUE_LEN, 0, PROTERY_VALUE_LEN);

        (void)dal_get_property_value_byte(obj_handle, PROPERTY_TRAP_MODE, &mode);
        if (mode == TRAP_MODE_EVT) {
            safe_fun_ret = strncpy_s(trap_mode, PROTERY_VALUE_LEN, TRAP_MODE_EVT_STR, strlen(TRAP_MODE_EVT_STR));
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
        } else if (mode == TRAP_MODE_OID) {
            safe_fun_ret = strncpy_s(trap_mode, PROTERY_VALUE_LEN, TRAP_MODE_OID_STR, strlen(TRAP_MODE_OID_STR));
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
        } else if (mode == TRAP_MODE_ACC) {
            safe_fun_ret = strncpy_s(trap_mode, PROTERY_VALUE_LEN, TRAP_MODE_ACC_STR, strlen(TRAP_MODE_ACC_STR));
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
        } else {
            g_printf("Get trap mode failed.\r\n");
            return FALSE;
        }

        g_printf("Trap mode : %s\r\n", trap_mode);
    }

    return TRUE;
}



gint32 ipmc_get_syslogenable(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    gint32 dest = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0 && (argc != 2 || strcmp(argv[0], "-v"))) {
        
        print_ipmcget_syslog_dest_state();
        
        return FALSE;
    }

    // 查询syslog dest使能
    if (argc == 2) {
        if ((argv[1] != NULL) && (index(argv[1], '.') != NULL)) {
            
            print_ipmcget_syslog_dest_state();
            
            return FALSE;
        }

        dest = str_to_int(argv[1]);
        if ((dest < 1) || (dest > 4) || (strlen(argv[1]) != 1)) {
            
            print_ipmcget_syslog_dest_state();
            
            return FALSE;
        }

        ret = get_syslog_dest_state(dest - 1);
    }

    else {
        ret = get_syslog_state();
    }

    return ret;
}


gint32 ipmc_get_syslog_auth(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    guint8 value = 0;
    OBJ_HANDLE obj_handle = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc) {
        g_printf("Usage: ipmcget -t syslog -d auth\r\n");
        return FALSE;
    }

    if (dal_get_object_handle_nth(CLASS_SYSLOG_CONFIG, 0, &obj_handle) != RET_OK) {
        g_printf("Unknown error.\r\n");
        return FALSE;
    }

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_SYSLOG_MGNT_AUTH_TYPE, &value);
    g_printf("Syslog auth type: ");

    if (value == 1) {
        g_printf("%s ", "one way authentication");
    } else if (value == 2) {
        g_printf("%s ", "mutual authentication");
    } else {
        g_printf("%s ", "unknown");
    }

    g_printf("\r\n");

    return TRUE;
}


gint32 ipmc_get_syslog_protocol(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    guint8 value = 0;
    OBJ_HANDLE obj_handle = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc) {
        g_printf("Usage: ipmcget -t syslog -d protocol\r\n");
        return FALSE;
    }

    if (dal_get_object_handle_nth(CLASS_SYSLOG_CONFIG, 0, &obj_handle) != RET_OK) {
        g_printf("Unknown error.\r\n");
        return FALSE;
    }

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_SYSLOG_MGNT_NET_PROTOCOL, &value);
    g_printf("Syslog protocol: ");

    if (value == 1) {
        g_printf("UDP ");
    } else if (value == 2) {
        g_printf("TCP ");
    } else if (value == 3) {
        g_printf("TLS ");
    } else {
        g_printf(" %s ", "unknown");
    }

    g_printf("\r\n");

    return TRUE;
}


#define BOARD_SN 1
#define HOST_NAME 3
#define CHASSIS_SERIAL_NUMBER 6
gint32 ipmc_get_syslog_identity(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    guint32 value = 0;
    OBJ_HANDLE obj_handle = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc) {
        g_printf("Usage: ipmcget -t syslog -d identity\r\n");
        return FALSE;
    }

    if (dal_get_object_handle_nth(CLASS_SYSLOG_CONFIG, 0, &obj_handle) != RET_OK) {
        g_printf("Unknown error.\r\n");
        return FALSE;
    }

    (void)dal_get_property_value_uint32(obj_handle, PROPERTY_SYSLOG_MGNT_MSG_IDENTITY, &value);
    guint8 software_type = 0;

    (void)dal_get_software_type(&software_type);
    if (software_type == MGMT_SOFTWARE_TYPE_EM) {
        if (value < HOST_NAME || value > CHASSIS_SERIAL_NUMBER) {
            g_printf("Syslog identity: unknown \r\n");
        } else {
            g_printf("Syslog identity: %s \r\n", syslog_identitystr[value - 1]);
        }
    } else {
        if (value < BOARD_SN || value > HOST_NAME) {
            g_printf("Syslog identity: unknown \r\n");
        } else {
            g_printf("Syslog identity: %s \r\n", syslog_identitystr[value - 1]);
        }
    }

    return TRUE;
}


gint32 ipmc_get_syslog_severity(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 i = 0;
    guint32 value = 0;
    OBJ_HANDLE obj_handle = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc) {
        g_printf("Usage: ipmcget -t syslog -d severity\r\n");
        return FALSE;
    }

    if (dal_get_object_handle_nth(CLASS_SYSLOG_CONFIG, 0, &obj_handle) != RET_OK) {
        g_printf("Unknown error.\r\n");
        return FALSE;
    }

    (void)dal_get_property_value_uint32(obj_handle, PROPERTY_SYSLOG_MGNT_MSG_SEVE_MASK, &value);

    g_printf("Syslog severity: ");
    if (value == 0) {
        g_printf(" %s ", "none");
    } else {
        for (i = 0; i < 4; i++) {
            if (value & (0x01 << i)) {
                g_printf(" %s ", syslog_severitystr[i]);
            }
        }
    }

    g_printf("\r\n");

    return TRUE;
}


gint32 ipmc_get_syslog_root_CA_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    gint32 ret = 0;
    const gchar *pstr = NULL;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc) {
        g_printf("Usage: ipmcget -t syslog -d rootcertificate\r\n");
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        (void)snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        (void)snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    if (dal_get_object_handle_nth(CLASS_SYSLOG_CONFIG, 0, &obj_handle) != RET_OK) {
        debug_log(DLOG_ERROR, "%s:dal_get_object_handle_nth failed,ret:0x%x\r\n", __FUNCTION__, ret);
        g_printf("Unknown error.\r\n");
        return FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(SYSLOG_CERT_TYPE_HOST));
    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
        CLASS_SYSLOG_CONFIG, obj_handle, METHOD_SYSLOG_MGNT_GET_CERT_INFO, input_list, &output_list);
    if (ret != RET_OK) {
        g_printf("root certificate not exist.\r\n");
        debug_log(DLOG_ERROR, "%s:uip_call_class_method_with_handle failed,ret:0x%x\r\n", __FUNCTION__, ret);
        uip_free_gvariant_list(input_list);
        return FALSE;
    }

    g_printf("Server Root Certificate:\r\n");
    pstr = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    g_printf("Issued     To: %s\r\n", pstr);
    pstr = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 1), NULL);
    g_printf("Issued     By: %s\r\n", pstr);
    pstr = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 2), NULL);
    g_printf("Valid    From: %s\r\n", pstr);
    pstr = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 3), NULL);
    g_printf("Valid      To: %s\r\n", pstr);
    pstr = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 4), NULL);
    g_printf("Serial Number: %s\r\n", pstr);

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);
    return TRUE;
}


gint32 ipmc_get_syslog_client_CA_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    gint32 ret;
    const gchar *pstr = NULL;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc) {
        g_printf("Usage: ipmcget -t syslog -d clientcertificate\r\n");
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        (void)snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        (void)snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    if (dal_get_object_handle_nth(CLASS_SYSLOG_CONFIG, 0, &obj_handle) != RET_OK) {
        g_printf("Unknown error.\r\n");
        return FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(SYSLOG_CERT_TYPE_CLIENT));
    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
        CLASS_SYSLOG_CONFIG, obj_handle, METHOD_SYSLOG_MGNT_GET_CERT_INFO, input_list, &output_list);
    if (ret != RET_OK) {
        g_printf("Client certificate not exist.\r\n");
        uip_free_gvariant_list(input_list);
        return FALSE;
    }

    g_printf("Syslog Client Certificate Information:\r\n");
    pstr = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    g_printf("Issued     To: %s\r\n", pstr);
    pstr = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 1), NULL);
    g_printf("Issued     By: %s\r\n", pstr);
    pstr = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 2), NULL);
    g_printf("Valid    From: %s\r\n", pstr);
    pstr = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 3), NULL);
    g_printf("Valid      To: %s\r\n", pstr);
    pstr = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 4), NULL);
    g_printf("Serial Number: %s\r\n", pstr);

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);
    return TRUE;
}


gint32 ipmc_get_syslogiteminfo(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcget -t syslog -d iteminfo\r\n");
        return FALSE;
    }

    g_printf("\n%-15s | %-10s | %-10s | %-20s | %-35s\r\n", "Item Num", "state", "port", "dest address", "log type");
    ret = dfl_foreach_object(CLASS_SYSLOG_ITEM_CONFIG, print_syslogitem_info, NULL, NULL);
    if (ret != DFL_OK) {
        g_printf("Get syslog item information failed.\r\n");
        return FALSE;
    }

    return TRUE;
}
