

#include "ipmcget_service_mgmt.h"

const struct valstr ipmc_boot_mode[] = {
    { NO_OVERRIDE,     "No override"                                          },
    { FORCE_PEX,     "Force PXE"                                            },
    { FORCE_HARD_DRIVE,    "Force boot from default Hard-drive"                   },
    { FORCE_SAFE_MODE,    "Force boot from default Hard-drive, request Safe Mode"},
    { FORCE_DIAGNOSTIC_PARTITION, "Force boot from default Diagnostic Partition"         },
    { FORCE_CD_DVD,     "Force boot from default CD/DVD"                       },
    { FORCE_BIOS_SETUP,    "Force boot into BIOS Setup"                           },
    { FORCE_FLOPPY_REMOVABLE_MEDIA, "Force boot from Floppy/primary removable media"       },
    { 0xFF,                         NULL                                                   },
};


gint32 ipmc_get_biosbootmode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    guint8 boot_option;
    guint8 boot_option_flag;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    property_name_list = g_slist_append(property_name_list, PROTERY_BIOS_BOOTOPTION);
    property_name_list = g_slist_append(property_name_list, PROTERY_BIOS_BOOTOPTIONFLAG);
    ret = uip_multiget_object_property(CLASS_NAME_BIOS, OBJ_NAME_BIOS, property_name_list, &property_value);
    if (ret != RET_OK) {
        g_slist_free(property_name_list);
        g_printf("Get boot device failed.\r\n");
        return FALSE;
    }

    boot_option = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 0));
    boot_option_flag = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 1));
    if (boot_option_flag == 0) {
        g_printf("%-8s device:  Unspecified\r\n", "Boot");
        g_printf("%-10s type:  %s\r\n", "Effective", "Once");
        g_slist_free(property_name_list);
        uip_free_gvariant_list(property_value);
        return FALSE;
    }

    g_printf("%-8s device:  %s\r\n", "Boot", val_to_str(boot_option, ipmc_boot_mode));
    g_printf("%-10s type:  %s\r\n", "Effective", (boot_option_flag == 1) ? "Once" : "Permanent");
    g_slist_free(property_name_list);
    uip_free_gvariant_list(property_value);
    return TRUE;
}


gint32 ipmc_get_ntp_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    NTP_SYNC_TIME_STATUS sync_status;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return RET_ERR;
    }
    if (argc != 0) {
        g_printf("Usage: ipmcget -d ntpinfo\r\n");
        return RET_ERR;
    }

    (void)memset_s(&sync_status, sizeof(NTP_SYNC_TIME_STATUS), 0, sizeof(NTP_SYNC_TIME_STATUS));

    ret = query_sync_time_status(OPERATOR_LOG_CLI, "", "", &sync_status);
    if (ret != RET_OK) {
        g_printf("Get NTP information failed.\r\n");
        return RET_ERR;
    }

    ret = print_ntp_info(&sync_status);
    return ret;
}


gint32 ipmc_get_service_list(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 i = 0;
    gint32 ret = TRUE;
    guint8 javavideo_enable = 0;

    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROPERTY_PME_SERVICECONFIG_JAVA_VIDEO_ENABLE,
        &javavideo_enable);

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    g_printf("%-15s | %-15s | %s\r\n", "service name", "state", "port");

    while ((get_g_bmc_get_service_list()[i].ServiceName != NULL) && (ret == TRUE)) {
        if (javavideo_enable != TRUE) {
            if (strcmp("video", get_g_bmc_get_service_list()[i].ServiceName) == 0) {
                i++;
                continue;
            }
        }
        if (get_g_bmc_get_service_list()[i].flag != UNOBVIOUS_CMD) {
            ret = get_g_bmc_get_service_list()[i].func();
        }

        i++;
    }

    return ret;
}


gint32 ipmc_get_vmm_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    OBJ_HANDLE obj_handle;
    GSList *property_name_list = NULL;
    GSList *property_value_list = NULL;
    guint8 property_index = 0;

    gint32 num;
    const gchar *rulstring = NULL;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcget -t vmm -d info \r\n");
        return FALSE;
    }

    
    if (dal_get_object_handle_nth(VMM_CLASS_NAME, 0, &obj_handle) != RET_OK) {
        g_printf("Unknown error.\r\n");
        return FALSE;
    }

    property_name_list = g_slist_append(property_name_list, VMM_PROPERTY_NUM);
    property_name_list = g_slist_append(property_name_list, VMM_PROPERTY_VMM_CONNECT_URL);
    ret = dfl_multiget_property_value(obj_handle, property_name_list, &property_value_list);
    g_slist_free(property_name_list);
    if (ret != DFL_OK) {
        g_printf("Unknown error.\r\n");
        return FALSE;
    }

    num = g_variant_get_int32((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    rulstring = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, property_index++), 0);

    g_printf("Virtual Media Information: \n");
    g_printf("Maximum Number of Virtual Media Sessions:     1\n");
    g_printf("Number of Activated Sessions            :     %d\n", num);
    g_printf("Activated Sessions URL                  :     \n%s\n", rulstring);
    uip_free_gvariant_list(property_value_list);

    return TRUE;
}


gint32 ipmc_get_vnc_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    OBJ_HANDLE obj_handle;
    GSList *property_name_list = NULL;
    GSList *property_value_list = NULL;
    gint32 timeout;
    guint8 ssl_state;
    gint32 connect_num;
    guint32 password_validity_period;
    const gchar *keyboard_layout = NULL;
    guint8 property_index = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcget -t vnc -d info \r\n");
        return FALSE;
    }

    
    if (dal_get_object_handle_nth(VNC_CLASS_NAME, 0, &obj_handle) != RET_OK) {
        g_printf("Unknown error.\r\n");
        return FALSE;
    }

    property_name_list = g_slist_append(property_name_list, PROPERTY_VNC_TIMEOUT);
    property_name_list = g_slist_append(property_name_list, PROPERTY_VNC_SSL_STATE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_VNC_CONNECT_NUM);
    property_name_list = g_slist_append(property_name_list, PROPERTY_VNC_KEYBOARD_LAYOUT);
    property_name_list = g_slist_append(property_name_list, PROPERTY_VNC_PWD_REMAIN_DAYS);
    ret = dfl_multiget_property_value(obj_handle, property_name_list, &property_value_list);
    g_slist_free(property_name_list);
    if (ret != DFL_OK) {
        g_printf("Get SSL_OBJECT_OWNER object multiget property Failed. \n");
        return FALSE;
    }

    timeout = g_variant_get_int32((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    ssl_state = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    connect_num = g_variant_get_int32((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    keyboard_layout = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, property_index++), NULL);
    password_validity_period =
        g_variant_get_uint32((GVariant *)g_slist_nth_data(property_value_list, property_index++));

    g_printf("Timeout Period(Min)        :   %d\n", timeout);
    g_printf("SSL Encryption             :   %s\n", ssl_state == 0 ? "disabled" : "enabled");
    g_printf("Active Sessions            :   %d\n", connect_num);
    g_printf("Keyboard Layout            :   %s\n", keyboard_layout);

    if (password_validity_period == 0xffff) {
        g_printf("Password Validity(Days)    :   %s\n", "Indefinite");
    } else {
        g_printf("Password Validity(Days)    :   %d\n", password_validity_period);
    }

    uip_free_gvariant_list(property_value_list);
    return TRUE;
}

gint32 ipmc_get_lldp_info(guchar privilege, const gchar* rolepriv, guchar argc, gchar** argv, gchar* target)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint8 lldp_status = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        g_printf("Permission denied.\r\n");
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Usage: ipmcget -d lldpinfo\r\n");
        return FALSE;
    }

    ret = dal_get_object_handle_nth(CLASS_NAME_LLDP_CONFIG, 0, &obj_handle);
    if (ret != DFL_OK) {
        g_printf("Get object handle fail!\r\n");
        return FALSE;
    }

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_LLDP_CONFIG_ENABLE, &lldp_status);
    if (ret != DFL_OK) {
        g_printf("Get object property fail!\r\n");
        return FALSE;
    }

    g_printf("Status : %s\n", (lldp_status == 0) ? "disabled" : "enabled");

    return TRUE;
}
