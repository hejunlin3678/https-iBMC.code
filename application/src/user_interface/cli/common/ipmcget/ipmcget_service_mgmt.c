

#include "ipmcget_service_mgmt.h"

LOCAL gint32 ipmc_get_service_ssh(void);
LOCAL gint32 ipmc_get_service_hmmsshnat(void);
LOCAL gint32 ipmc_get_service_hmmsshnat2(void);
LOCAL gint32 ipmc_get_service_snmp(void);
LOCAL gint32 ipmc_get_service_kvm(void);
LOCAL gint32 ipmc_get_service_vnc(void);
LOCAL gint32 ipmc_get_service_vmm(void);
LOCAL gint32 ipmc_get_service_video(void);
LOCAL gint32 ipmc_get_service_web(void);
LOCAL gint32 ipmc_get_service_ipmilan(void);
LOCAL gint32 ipmc_get_service_ssdp(void);

struct get_service g_bmc_get_service_list[] = {
    {
        
        "ssh",  OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_SSH,
        "1", ipmc_get_service_ssh
        
    },
    {
        "nat",  OBVIOUS_CMD, HIDE_TYPE_PROP, SERVER_PORT_CLASS_NAME, 0, SERVER_PORT_ATTRIBUTE_NAME,
        HMMSSHNAT_SERVER_NAME, ipmc_get_service_hmmsshnat
    },
    
    {
        "nat2",  OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_EX_PORT_CONFIG, 0, SERVER_PORT_ATTRIBUTE_NAME,
        HMMSSHNAT_SERVER_NAME2, ipmc_get_service_hmmsshnat2
    },
    
    {
        "snmp",  OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_SNMPD,
        "1", ipmc_get_service_snmp
    },
    {
        "kvm",  OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_KVM,
        "1", ipmc_get_service_kvm
    },
    {
        "vnc",  OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_VNC,
        "1", ipmc_get_service_vnc
    },
    {
        "vmm",  OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_KVM,
        "1", ipmc_get_service_vmm
    },
    {
        "video", OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_KVM,
        "1", ipmc_get_service_video
    },
    {
        "web", OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_HTTPS,
        "1", ipmc_get_service_web
    },
    {
        
        "ipmi lan",  OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_IPMI, "1",
        ipmc_get_service_ipmilan
        
    },
    {
        "ssdp",  OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_SSDP, "1",
        ipmc_get_service_ssdp
    },
    {NULL},
};
struct get_service *get_g_bmc_get_service_list(void)
{
    return g_bmc_get_service_list;
}


LOCAL gint32 get_ntp_enable_status_string(NTP_SYNC_TIME_STATUS *sync_status, gchar *str_enable_status, gint32 sz)
{
    errno_t safe_fun_ret;
    if ((!sync_status) || (!str_enable_status)) {
        return RET_ERR;
    }

    if (sync_status->enable_status == NTP_ENABLED) {
        safe_fun_ret = strcpy_s(str_enable_status, sz, "enabled");
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            return RET_ERR;
        }
    } else if (sync_status->enable_status == NTP_DISABLED) {
        safe_fun_ret = strcpy_s(str_enable_status, sz, "disabled");
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            return RET_ERR;
        }
    } else {
        safe_fun_ret = strcpy_s(str_enable_status, sz, "unknown");
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            return RET_ERR;
        }
    }

    return RET_OK;
}


LOCAL gint32 get_ntp_mode_string(NTP_SYNC_TIME_STATUS *sync_status, gchar *str_mode, gint32 sz)
{
    errno_t safe_fun_ret;
    if ((!sync_status) || (!str_mode)) {
        return RET_ERR;
    }

    if (sync_status->mode == NTP_MODE_MANUAL) {
        safe_fun_ret = strcpy_s(str_mode, sz, "manual");
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            return RET_ERR;
        }
    } else if (sync_status->mode == NTP_MODE_DHCPV4) {
        safe_fun_ret = strcpy_s(str_mode, sz, "dhcpv4");
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            return RET_ERR;
        }
    } else if (sync_status->mode == NTP_MODE_DHCPV6) {
        safe_fun_ret = strcpy_s(str_mode, sz, "dhcpv6");
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            return RET_ERR;
        }
    } else {
        safe_fun_ret = strcpy_s(str_mode, sz, "unknown");
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            return RET_ERR;
        }
    }

    return RET_OK;
}


LOCAL gint32 get_ntp_sync_status_string(NTP_SYNC_TIME_STATUS *sync_status, gchar *str_sync, gint32 sz)
{
    errno_t safe_fun_ret;
    if ((!sync_status) || (!str_sync)) {
        return RET_ERR;
    }

    if (sync_status->is_sync_sucessful == 0) {
        safe_fun_ret = strcpy_s(str_sync, sz, "failed");
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            return RET_ERR;
        }
    } else if (sync_status->is_sync_sucessful == 1) {
        safe_fun_ret = strcpy_s(str_sync, sz, "successful");
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            return RET_ERR;
        }
    } else {
        safe_fun_ret = strcpy_s(str_sync, sz, "unknown");
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            return RET_ERR;
        }
    }

    return RET_OK;
}

LOCAL void get_ntp_auth_status_string(NTP_SYNC_TIME_STATUS *sync_status, gchar *str_auth, gint32 sz)
{
    errno_t safe_fun_ret;
    if ((!sync_status) || (!str_auth)) {
        debug_log(DLOG_ERROR, "%s: input para is null.", __FUNCTION__);
        return;
    }

    if (sync_status->auth_enable == NTP_AUTH_ENABLED) {
        safe_fun_ret = strcpy_s(str_auth, sz, "enabled");
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            return;
        }
    } else if (sync_status->auth_enable == NTP_AUTH_DISABLED) {
        safe_fun_ret = strcpy_s(str_auth, sz, "disabled");
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            return;
        }
    } else {
        safe_fun_ret = strcpy_s(str_auth, sz, "unknown");
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            return;
        }
    }

    return;
}


LOCAL void get_ntp_group_key_string(NTP_SYNC_TIME_STATUS *sync_status, gchar *str_group_key, gint32 sz)
{
    errno_t safe_fun_ret;
    if ((!sync_status) || (!str_group_key)) {
        debug_log(DLOG_ERROR, "%s: input para is null.", __FUNCTION__);
        return;
    }

    if (sync_status->group_key_status == NTP_GROUP_KEY_IMPORTED) {
        safe_fun_ret = strcpy_s(str_group_key, sz, "imported");
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            return;
        }
    } else if (sync_status->group_key_status == NTP_GROUP_KEY_UNIMPORTED) {
        safe_fun_ret = strcpy_s(str_group_key, sz, "not imported");
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            return;
        }
    } else {
        safe_fun_ret = strcpy_s(str_group_key, sz, "unknown");
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            return;
        }
    }

    return;
}

gint32 print_ntp_info(NTP_SYNC_TIME_STATUS *sync_status)
{
    gchar str_enable_status[PARAMETER_LEN] = {0};
    gchar str_mode[PARAMETER_LEN] = {0};
    gchar str_sync[PARAMETER_LEN] = {0};
    gchar str_auth[PARAMETER_LEN] = {0};
    gchar str_group_key[PARAMETER_LEN] = {0};
    gint32 ret;
    guchar ntp_count = 0;
    OBJ_HANDLE obj_handle;

    if (sync_status == NULL) {
        return RET_ERR;
    }
    ret = dal_get_object_handle_nth(CLASS_NTP, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, Failed to get ntp object,ret:%d.\r\n", __FUNCTION__, ret);
        return RET_ERR;
    }
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_NTP_SERVER_COUNT, &ntp_count);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s Failed to call dal_get_property_value_byte.ret:%d\r\n", __FUNCTION__, ret);
        return RET_ERR;
    }
    get_ntp_enable_status_string(sync_status, str_enable_status, PARAMETER_LEN);
    get_ntp_mode_string(sync_status, str_mode, PARAMETER_LEN);
    get_ntp_sync_status_string(sync_status, str_sync, PARAMETER_LEN);
    get_ntp_auth_status_string(sync_status, str_auth, PARAMETER_LEN);
    get_ntp_group_key_string(sync_status, str_group_key, PARAMETER_LEN);

    g_printf("Status             : %s\r\n", str_enable_status);
    g_printf("Mode               : %s\r\n", str_mode);
    g_printf("Preferred Server   : %s\r\n", sync_status->ntp_server_list[NTP_SERVER_PREFERRED]);
    g_printf("Alternative Server : %s\r\n", sync_status->ntp_server_list[NTP_SERVER_ALTERNATIVE]);
    g_printf("Extra Server       : %s\r\n", sync_status->ntp_server_list[NTP_SERVER_THIRD]);
    if (ntp_count == MAX_NTP_SERVER_COUNT) {
        g_printf("                   : %s\r\n", sync_status->ntp_server_list[NTP_SERVER_FOURTH]);
        g_printf("                   : %s\r\n", sync_status->ntp_server_list[NTP_SERVER_FIFTH]);
        g_printf("                   : %s\r\n", sync_status->ntp_server_list[NTP_SERVER_SIXTH]);
    }
    g_printf("Synchronize        : %s\r\n", str_sync);
    g_printf("Auth Enable        : %s\r\n", str_auth);
    g_printf("Group Key          : %s\r\n", str_group_key);

    return RET_OK;
}

LOCAL gint32 ipmc_get_service_ssh(void)
{
    gint32 ret;
    guint8 state;
    const guint16 *port = NULL;
    gsize port_num = 0;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;

    
    property_name_list = g_slist_append(property_name_list, SERVER_PORT_ATTRIBUTE_STATE);
    property_name_list = g_slist_append(property_name_list, SERVER_PORT_ATTRIBUTE_PORT);
    ret = uip_multiget_object_property(SERVER_PORT_CLASS_NAME, SSH_SERVER_PORT_OBJ_NAME, property_name_list,
        &property_value);
    g_slist_free(property_name_list);
    if (ret != RET_OK) {
        g_printf("Get SSH info failed.\r\n");
        return FALSE;
    }

    state = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 0));
    port = (const guint16 *)g_variant_get_fixed_array((GVariant *)g_slist_nth_data(property_value, 1), &port_num,
        sizeof(guint16));
    if (port == NULL) {
        g_printf("Get SSH port failed.\r\n");
        uip_free_gvariant_list(property_value);
        return FALSE;
    }

    
    g_printf("%-15s | %-15s | %d\r\n", "SSH", ((state == 0) ? "Disabled" : "Enabled"), *port);
    uip_free_gvariant_list(property_value);

    return TRUE;
}

LOCAL gint32 ipmc_get_service_hmmsshnat(void)
{
    gint32 ret;
    guint8 state;
    const guint16 *port = NULL;
    gsize port_num = 0;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;

    
    property_name_list = g_slist_append(property_name_list, SERVER_PORT_ATTRIBUTE_STATE);
    property_name_list = g_slist_append(property_name_list, SERVER_PORT_ATTRIBUTE_PORT);
    ret = uip_multiget_object_property(SERVER_PORT_CLASS_NAME, HMMSSHNAT_SERVER_PORT_OBJ_NAME, property_name_list,
        &property_value);
    g_slist_free(property_name_list);
    if (ret != RET_OK) {
        g_printf("Get NAT info failed.\r\n");
        return FALSE;
    }

    state = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 0));
    port = (const guint16 *)g_variant_get_fixed_array((GVariant *)g_slist_nth_data(property_value, 1), &port_num,
        sizeof(guint16));
    if (port == NULL) {
        g_printf("Get NAT port failed.\r\n");
        uip_free_gvariant_list(property_value);
        return FALSE;
    }

    
    g_printf("%-15s | %-15s | %d\r\n", "NAT", ((state == 0) ? "Disabled" : "Enabled"), *port);
    uip_free_gvariant_list(property_value);

    return TRUE;
}


LOCAL gint32 ipmc_get_service_hmmsshnat2(void)
{
    gint32 ret;
    guint8 state;
    const guint16 *port = NULL;
    gsize port_num = 0;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;

    
    property_name_list = g_slist_append(property_name_list, SERVER_PORT_ATTRIBUTE_STATE);
    property_name_list = g_slist_append(property_name_list, SERVER_PORT_ATTRIBUTE_PORT);
    ret = uip_multiget_object_property(SERVER_PORT_CLASS_NAME, HMMSSHNAT_SERVER_PORT_OBJ_NAME2, property_name_list,
        &property_value);
    g_slist_free(property_name_list);
    if (ret != RET_OK) {
        g_printf("Get NAT2 info failed.\r\n");
        return FALSE;
    }

    state = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 0));
    port = (const guint16 *)g_variant_get_fixed_array((GVariant *)g_slist_nth_data(property_value, 1), &port_num,
        sizeof(guint16));
    if (port == NULL) {
        g_printf("Get NAT2 port failed.\r\n");
        uip_free_gvariant_list(property_value);
        return FALSE;
    }

    
    g_printf("%-15s | %-15s | %d\r\n", "NAT2", ((state == 0) ? "Disabled" : "Enabled"), *port);
    uip_free_gvariant_list(property_value);

    return TRUE;
}


LOCAL gint32 ipmc_get_service_snmp(void)
{
    gint32 ret;
    guint8 state;
    gint32 snmp_portid;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;

    
    property_name_list = g_slist_append(property_name_list, PROPERTY_SNMP_STATE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_SNMP_PORTID);
    ret = uip_multiget_object_property(CLASS_SNMP, OBJ_NAME_SNMP, property_name_list, &property_value);
    g_slist_free(property_name_list);
    if (ret != RET_OK) {
        g_printf("Get SNMP info failed.\r\n");
        return FALSE;
    }

    state = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 0));
    snmp_portid = g_variant_get_int32((GVariant *)g_slist_nth_data(property_value, 1));
    
    g_printf("%-15s | %-15s | %d\r\n", "SNMP", ((state == 0) ? "Disabled" : "Enabled"), snmp_portid);
    uip_free_gvariant_list(property_value);

    return TRUE;
}


LOCAL gint32 ipmc_get_service_kvm(void)
{
    gint32 ret;
    guint8 state;
    gint32 kvm_portid;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;

    
    property_name_list = g_slist_append(property_name_list, KVM_PROPERTY_STATE);
    property_name_list = g_slist_append(property_name_list, KVM_PROPERTY_PORT);
    ret = uip_multiget_object_property(KVM_CLASS_NAME, KVM_OBJ_NAME, property_name_list, &property_value);
    g_slist_free(property_name_list);
    if (ret != RET_OK) {
        g_printf("Get KVM info failed.\r\n");
        return FALSE;
    }

    state = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 0));
    kvm_portid = g_variant_get_int32((GVariant *)g_slist_nth_data(property_value, 1));
    
    g_printf("%-15s | %-15s | %d\r\n", "KVM", ((state == 0) ? "Disabled" : "Enabled"), kvm_portid);
    uip_free_gvariant_list(property_value);

    return TRUE;
}


LOCAL gint32 ipmc_get_service_vnc(void)
{
    gint32 ret;
    guint8 state;
    gint32 vnc_portid;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;

    
    property_name_list = g_slist_append(property_name_list, PROPERTY_VNC_ENABLE_STATE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_VNC_PORT);
    ret = uip_multiget_object_property(VNC_CLASS_NAME, OBJECT_VNC, property_name_list, &property_value);
    g_slist_free(property_name_list);
    if (ret != RET_OK) {
        g_printf("Failed to obtain VNC information.\r\n");
        return FALSE;
    }

    state = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 0));
    vnc_portid = g_variant_get_int32((GVariant *)g_slist_nth_data(property_value, 1));
    
    g_printf("%-15s | %-15s | %d\r\n", "VNC", ((state == 0) ? "Disabled" : "Enabled"), vnc_portid);
    uip_free_gvariant_list(property_value);

    return TRUE;
}


LOCAL gint32 ipmc_get_service_vmm(void)
{
    gint32 ret;
    guint8 state;
    gint32 vmm_portid;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;

    
    property_name_list = g_slist_append(property_name_list, VMM_PROPERTY_STATE);
    property_name_list = g_slist_append(property_name_list, VMM_PROPERTY_PORT);
    ret = uip_multiget_object_property(VMM_CLASS_NAME, VMM_OBJ_NAME, property_name_list, &property_value);
    g_slist_free(property_name_list);
    if (ret != RET_OK) {
        g_printf("Get VMM info failed.\r\n");
        return FALSE;
    }

    state = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 0));
    vmm_portid = g_variant_get_int32((GVariant *)g_slist_nth_data(property_value, 1));
    
    g_printf("%-15s | %-15s | %d\r\n", "VMM", ((state == 0) ? "Disabled" : "Enabled"), vmm_portid);
    uip_free_gvariant_list(property_value);

    return TRUE;
}


LOCAL gint32 ipmc_get_service_video(void)
{
    gint32 ret;
    guint8 state;
    gint32 video_portid;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;

    
    property_name_list = g_slist_append(property_name_list, VIDEO_PROPERTY_STATE);
    property_name_list = g_slist_append(property_name_list, VIDEO_PROPERTY_PORT);
    ret = uip_multiget_object_property(VIDEO_CLASS_NAME, VIDEO_OBJ_NAME, property_name_list, &property_value);
    g_slist_free(property_name_list);
    if (ret != RET_OK) {
        g_printf("Get Video info failed.\r\n");
        return FALSE;
    }

    state = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 0));
    video_portid = g_variant_get_int32((GVariant *)g_slist_nth_data(property_value, 1));
    
    g_printf("%-15s | %-15s | %d\r\n", "Video", ((state == 0) ? "Disabled" : "Enabled"), video_portid);
    uip_free_gvariant_list(property_value);

    return TRUE;
}


LOCAL gint32 ipmc_get_service_web(void)
{
    gint32 ret;
    guint8 http_state;
    guint8 https_state;
    const guint16 *port = NULL;
    guint16 http_port;
    guint16 https_port;
    gsize port_num = 0;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;
    guint8 http_enabled = 0xff;

    
    property_name_list = g_slist_append(property_name_list, SERVER_PORT_ATTRIBUTE_STATE);
    property_name_list = g_slist_append(property_name_list, SERVER_PORT_ATTRIBUTE_PORT);
    ret = uip_multiget_object_property(SERVER_PORT_CLASS_NAME, WEBHTTP_SERVER_PORT_OBJ_NAME, property_name_list,
        &property_value);
    g_slist_free(property_name_list);
    property_name_list = NULL;
    if (ret != RET_OK) {
        g_printf("Get HTTP info failed.\r\n");
        return FALSE;
    }

    http_state = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 0));
    port = (const guint16 *)g_variant_get_fixed_array((GVariant *)g_slist_nth_data(property_value, 1), &port_num,
        sizeof(guint16));
    if (port == NULL) {
        g_printf("Get HTTP port failed.\r\n");
        uip_free_gvariant_list(property_value);
        return FALSE;
    }

    http_port = *port;
    uip_free_gvariant_list(property_value);
    property_value = NULL;

    property_name_list = g_slist_append(property_name_list, SERVER_PORT_ATTRIBUTE_STATE);
    property_name_list = g_slist_append(property_name_list, SERVER_PORT_ATTRIBUTE_PORT);
    ret = uip_multiget_object_property(SERVER_PORT_CLASS_NAME, WEBHTTPS_SERVER_PORT_OBJ_NAME, property_name_list,
        &property_value);
    g_slist_free(property_name_list);
    if (ret != RET_OK) {
        g_printf("Get HTTPS info failed.\r\n");
        return FALSE;
    }

    https_state = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 0));
    port = (const guint16 *)g_variant_get_fixed_array((GVariant *)g_slist_nth_data(property_value, 1), &port_num,
        sizeof(guint16));
    if (port == NULL) {
        g_printf("Get HTTPS port failed.\r\n");
        uip_free_gvariant_list(property_value);
        return FALSE;
    }

    https_port = *port;
    uip_free_gvariant_list(property_value);

    
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_HTTP, &http_enabled);
    if (http_enabled != SERVICE_STATE_DISABLE) {
        g_printf("%-15s | %-15s | %d\r\n", "HTTP", ((http_state == 0) ? "Disabled" : "Enabled"), http_port);
    }
    
    g_printf("%-15s | %-15s | %d\r\n", "HTTPS", ((https_state == 0) ? "Disabled" : "Enabled"), https_port);

    return TRUE;
}


LOCAL gint32 ipmc_get_service_ipmilan(void)
{
    gint32 ret;
    guint8 rcmp_state;
    guint8 rcmpplus_state;
    gint32 rmcp_port1;
    gint32 rmcp_port2;
    guint8 service_flag;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;

    
    property_name_list = g_slist_append(property_name_list, PROPERTY_LAN_STATE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_LAN_PLUS_STATE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RMCP_PORT1);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RMCP_PORT2);
    
    property_name_list = g_slist_append(property_name_list, PROPERTY_RMCP_SERVICE_FLAG);
    
    ret = uip_multiget_object_property(RMCP_CLASS_NAME, RMCP_OBJ_NAME, property_name_list, &property_value);
    g_slist_free(property_name_list);
    if (ret != RET_OK) {
        g_printf("Get RMCP info failed.\r\n");
        return FALSE;
    }

    rcmp_state = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 0));
    rcmpplus_state = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 1));
    rmcp_port1 = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value, 2));
    rmcp_port2 = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value, 3));
    
    service_flag = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 4));
    rcmp_state = rcmp_state & service_flag;
    rcmpplus_state = rcmpplus_state & service_flag;
    

    
    g_printf("%-15s | %-15s | %d,%d\r\n", "RMCP", ((rcmp_state == 0) ? "Disabled" : "Enabled"), rmcp_port1, rmcp_port2);
    g_printf("%-15s | %-15s | %d,%d\r\n", "RMCP+", ((rcmpplus_state == 0) ? "Disabled" : "Enabled"), rmcp_port1,
        rmcp_port2);
    uip_free_gvariant_list(property_value);

    return TRUE;
}


LOCAL gint32 ipmc_get_service_ssdp(void)
{
    guint8 mode = 0;
    guint32 port = 0;
    OBJ_HANDLE obj_handle = 0;

    (void)dal_get_object_handle_nth(CLASS_SSDP_CONFIG, 0, &obj_handle);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_SSDP_CONFIG_MODE, &mode);
    (void)dal_get_property_value_uint32(obj_handle, PROPERTY_SSDP_CONFIG_PORT, &port);

    
    g_printf("%-15s | %-15s | %d\r\n", "SSDP",
        ((mode & (SERVICE_STATE_ENABLE << SSDP_RECEIVE_BIT)) ? "Enabled" : "Disabled"), port);

    return TRUE;
}
