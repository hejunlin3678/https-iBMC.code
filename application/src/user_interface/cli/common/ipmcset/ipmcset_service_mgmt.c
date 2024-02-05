

#include "ipmcset_service_mgmt.h"
#include "ipmcset_netconfig.h"
#include "ipmcset_power.h"
#include "ipmcset_user_security.h"

struct service_method g_bmc_cmd_service_port[] = {
    {
        "SSH",  "SSH service port(default value 22)",  SSH_SERVER_PORT_OBJ_NAME, SERVER_PORT_CLASS_NAME,
        SERVER_PORT_METHOD_SET_PORT, OBVIOUS_CMD, HIDE_TYPE_NULL,
        NULL, 0, NULL, NULL, SSH_SERVICE_PORT_INDEX
    },
    {
        "NAT", "hmm ssh NAT service port(default value 30022)", HMMSSHNAT_SERVER_PORT_OBJ_NAME, SERVER_PORT_CLASS_NAME,
        SERVER_PORT_METHOD_SET_PORT, OBVIOUS_CMD, HIDE_TYPE_PROP,
        SERVER_PORT_CLASS_NAME, 0, SERVER_PORT_ATTRIBUTE_NAME, HMMSSHNAT_SERVER_NAME, HMM_SSH_NAT_PORT_INDEX
    },
    
    {
        "NAT2", "hmm ssh NAT2 service port(default value 30122)", HMMSSHNAT_SERVER_PORT_OBJ_NAME2, CLASS_EX_PORT_CONFIG,
        SERVER_PORT_METHOD_SET_PORT, OBVIOUS_CMD, HIDE_TYPE_PROP,
        CLASS_EX_PORT_CONFIG, 0, SERVER_PORT_ATTRIBUTE_NAME, HMMSSHNAT_SERVER_NAME2, HMM_SSH_NAT_PORT_INDEX
    },
    
    {
        "SNMP", "SNMP service port(default value 161)", OBJ_NAME_SNMP, CLASS_SNMP,
        METHOD_SNMP_SETSNMPPORTID, OBVIOUS_CMD, HIDE_TYPE_PROP,
        CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_SNMPD, "1", SNMP_SERVICE_PORT_INDEX
    },

    
    {
        "KVM", "KVM service port(default value 2198)", PROXY_KVM_OBJ_NAME, PROXY_KVM_CLASS_NAME,
        KVM_METHOD_SET_PORT, OBVIOUS_CMD, HIDE_TYPE_PROP,
        CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_KVM, "1", KVM_SERVICE_PORT_INDEX
    },
    {
        "VNC", "VNC service port(default value 5900)", OBJECT_VNC, VNC_CLASS_NAME,
        METHOD_VNC_SET_PORT, OBVIOUS_CMD, HIDE_TYPE_PROP,
        CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_VNC, "1", VNC_SERVICE_PORT_INDEX
    },
    {
        "VMM", "VMM service port(default value 8208)", PROXY_VMM_OBJ_NAME, PROXY_VMM_CLASS_NAME,
        VMM_METHOD_SET_PORT, OBVIOUS_CMD, HIDE_TYPE_PROP,
        CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_KVM, "1", VMM_SERVICE_PORT_INDEX
    },
    {
        "Video", "Video service port(default value 2199)", PROXY_VIDEO_OBJ_NAME, PROXY_VIDEO_CLASS_NAME,
        VIDEO_METHOD_SET_PORT, OBVIOUS_CMD, HIDE_TYPE_PROP,
        CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_KVM, "1", VIDEO_SERVICE_PORT_INDEX
    },
    
    {
        "HTTP", "web server HTTP service port(default value 80)", WEBHTTP_SERVER_PORT_OBJ_NAME, SERVER_PORT_CLASS_NAME,
        SERVER_PORT_METHOD_SET_PORT, OBVIOUS_CMD, HIDE_TYPE_PROP,
        CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_HTTPS, "1", WEB_HTTP_SERVICE_PORT_INDEX
    },
    {
        "HTTPS",  "web server HTTPS service port(default value 443)",  WEBHTTPS_SERVER_PORT_OBJ_NAME,
        SERVER_PORT_CLASS_NAME, SERVER_PORT_METHOD_SET_PORT, OBVIOUS_CMD, HIDE_TYPE_PROP,
        CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_HTTPS, "1", WEB_HTTPS_SERVICE_PORT_INDEX
    },
    {
        "RMCP", "RMCP/RMCP+ service port1 and port2(default value 623 and 664)", RMCP_OBJ_NAME, RMCP_CLASS_NAME,
        METHOD_SET_RMCP_PORT, OBVIOUS_CMD, HIDE_TYPE_PROP,
        CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_IPMI, "1", IPMI_SERVICE_PORT1_INDEX
    },
    {NULL},
};

struct service_method *get_g_bmc_cmd_service_port(void)
{
    return g_bmc_cmd_service_port;
}

struct service_method g_bmc_cmd_service_state[] = {
    {
        "SSH", "SSH service state", SSH_SERVER_PORT_OBJ_NAME, SERVER_PORT_CLASS_NAME,
        SERVER_PORT_METHOD_SET_STATE, OBVIOUS_CMD, HIDE_TYPE_NULL,
        NULL, 0, NULL, NULL, SSH_SERVICE_STATE_INDEX
    },

    {
        "NAT", "hmm ssh NAT service state", HMMSSHNAT_SERVER_PORT_OBJ_NAME, SERVER_PORT_CLASS_NAME,
        SERVER_PORT_METHOD_SET_STATE, OBVIOUS_CMD, HIDE_TYPE_PROP,
        SERVER_PORT_CLASS_NAME, 0, SERVER_PORT_ATTRIBUTE_NAME, HMMSSHNAT_SERVER_NAME, 0xFF
    },
    
    {
        "NAT2", "hmm ssh NAT2 service state", HMMSSHNAT_SERVER_PORT_OBJ_NAME2, CLASS_EX_PORT_CONFIG,
        SERVER_PORT_METHOD_SET_STATE, OBVIOUS_CMD, HIDE_TYPE_PROP,
        CLASS_EX_PORT_CONFIG, 0, SERVER_PORT_ATTRIBUTE_NAME, HMMSSHNAT_SERVER_NAME2, 0xFF
    },
    
    {
        "SNMP", "SNMP service state", OBJ_NAME_SNMP, CLASS_SNMP,
        METHOD_SNMP_SETSNMPSTATE, OBVIOUS_CMD, HIDE_TYPE_PROP,
        CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_SNMPD, "1", 0xFF
    },
    
    {
        "KVM", "KVM service state", PROXY_KVM_OBJ_NAME, PROXY_KVM_CLASS_NAME,
        KVM_METHOD_SET_ENABLE, OBVIOUS_CMD, HIDE_TYPE_PROP,
        CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_KVM, "1", 0xFF
    },
    {
        "VNC", "VNC service state", OBJECT_VNC, VNC_CLASS_NAME,
        METHOD_VNC_SET_ENABLE_STATE, OBVIOUS_CMD, HIDE_TYPE_PROP,
        CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_VNC, "1", 0xFF
    },
    {
        "VMM", "VMM service state", PROXY_VMM_OBJ_NAME, PROXY_VMM_CLASS_NAME,
        VMM_METHOD_SET_ENABLE, OBVIOUS_CMD, HIDE_TYPE_PROP,
        CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_KVM, "1", 0xFF
    },
    {
        "Video", "Video service state", PROXY_VIDEO_OBJ_NAME, PROXY_VIDEO_CLASS_NAME,
        VIDEO_METHOD_SET_ENABLE, OBVIOUS_CMD, HIDE_TYPE_PROP,
        CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_KVM, "1", 0xFF
    },
    
    {
        "HTTP", "web server HTTP service state", WEBHTTP_SERVER_PORT_OBJ_NAME, SERVER_PORT_CLASS_NAME,
        SERVER_PORT_METHOD_SET_STATE, OBVIOUS_CMD, HIDE_TYPE_PROP,
        CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_HTTPS, "1", 0xFF
    },
    {
        "HTTPS", "web server HTTPS service state", WEBHTTPS_SERVER_PORT_OBJ_NAME, SERVER_PORT_CLASS_NAME,
        SERVER_PORT_METHOD_SET_STATE, OBVIOUS_CMD, HIDE_TYPE_PROP,
        CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_HTTPS, "1", WEB_HTTPS_SERVICE_STATE_INDEX
    },
    {
        "RMCP", "RMCP service state", RMCP_OBJ_NAME, RMCP_CLASS_NAME,
        METHOD_SET_IPMILAN_PROTOCOL_STATE, OBVIOUS_CMD, HIDE_TYPE_PROP,
        CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_IPMI, "1", RMCP_LAN_SERVICE_STATE_INDEX
    },
    {
        "RMCP+", "RMCP+ service state", RMCP_OBJ_NAME, RMCP_CLASS_NAME,
        METHOD_SET_IPMILAN_PROTOCOL_STATE, OBVIOUS_CMD, HIDE_TYPE_PROP,
        CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_IPMI, "1", RMCP_LANPLUS_SERVICE_STATE_INDEX
    },
    {
        "SSDP", "SSDP service state", CLASS_SSDP_CONFIG, CLASS_SSDP_CONFIG,
        METHOD_SSDP_CONFIG_MODE, OBVIOUS_CMD, HIDE_TYPE_PROP,
        CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_SSDP, "1", 0xFF
    },
    {NULL},
};

struct service_method *get_g_bmc_cmd_service_state(void)
{
    return g_bmc_cmd_service_state;
}

LOCAL gchar *gettrapcommunity(const gchar *prompt);



gint32 setuser_privilege_judge(guchar privilege, const gchar *rolepriv, gchar *username, guint8 *userid)
{
    guint32 i = 0;
    gint32 ret;
    uid_t uid = 0;
    LDAP_USER usr = { 0 };
    OBJ_HANDLE obj_handle = 0;
    guint8 role_privilege = 0;

    
    if ((rolepriv == NULL) || (username == NULL) || (userid == NULL)) {
        return FALSE;
    }

    
    ret = ipmc_get_uid(&uid);
    if (ret != RET_OK) {
        g_printf("Get user info failed.\r\n");
        return RET_ERR;
    }

    
    if (uid >= LDAP_USER_ID_BASE) {
        if (uip_read_ldap_user(uid, &usr) != RET_OK) {
            return FALSE;
        }

        for (i = 0; i < LDAP_USER_ROLE_ID_NUM; i++) {
            // 0 表示结束了
            if (usr.roleid[i] == 0) {
                g_printf("Insufficient privilege level\r\n");
                return FALSE;
            }

            
            obj_handle = 0;
            ret = dal_get_specific_object_byte(CLASS_USERROLE, PROPERTY_USERROLE_ID, usr.roleid[i], &obj_handle);
            if (ret != RET_OK) {
                return FALSE;
            }

            
            (void)dal_get_property_value_byte(obj_handle, rolepriv, &role_privilege);

            if ((role_privilege == 1) && (usr.privilege >= privilege)) {
                break;
            }
        }

        if (i >= LDAP_USER_ROLE_ID_NUM) {
            g_printf("Insufficient privilege level\r\n");
            return FALSE;
        }
    } else {
        
        uid = (uid == 0) ? 502 : uid;
        
        if (uid <= USER_ID_BASE || uid >= USER_ID_BASE + IPMC_MAX_USER_NUMBER) {
            return FALSE;
        }

        if (get_user_info() == FALSE) {
            g_printf("Get user info failed.\r\n");
            return FALSE;
        }

        
        
        if (_ipmc_check_configure_self_priv(g_user_pri_info[uid - USER_ID_BASE].username, username, privilege,
            rolepriv) != RET_OK) {
            return FALSE;
        }

        
    }

    
    if (ipmc_check_user_passwd() != RET_OK) {
        return FALSE;
    }

    
    if (ipmc_check_user_name(username) != RET_OK) {
        return FALSE;
    }

    
    if ((ipmc_check_user_exist(username, userid) != RET_OK) || (*userid == IPMC_MAX_USER_NUMBER)) {
        g_printf("User %s doesn't exist.\r\n", username);
        return FALSE;
    }

    return TRUE;
}


gint32 set_user_public_key(guchar user_id, guchar type, const gchar *file_path)
{
    gint32 iRet;
    gint32 ret;
    gint32 result;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    gchar obj_name[OBJ_NAME_MAX_LEN] = {0};
    GSList *input_list = NULL;
    GSList *output_list = NULL;

    
    if (file_path == NULL) {
        return RET_ERR;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(user_id));
    input_list = g_slist_append(input_list, g_variant_new_byte(type));
    input_list = g_slist_append(input_list, g_variant_new_string(file_path));

    
    iRet = snprintf_s(obj_name, OBJ_NAME_MAX_LEN, OBJ_NAME_MAX_LEN - 1, "%s%u", CLASS_USER, user_id);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    ret = ipmc_call_method_user_info(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_USER,
        (const gchar *)obj_name, METHOD_USER_SETPUBLICKEY, input_list, &output_list);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    result = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
    uip_free_gvariant_list(output_list);

    if (result != RET_OK) {
        if (result == USER_PUBLIC_KEY_FORMAT_ERROR) {
            g_printf("Incorrect public key format.\r\n");
        }

        return RET_ERR;
    }

    return RET_OK;
}


gint32 _ipmc_check_configure_self_priv(const gchar *current_user_name, const gchar *target_user, guchar priv,
    const gchar *role_priv_prop)
{
    guchar configure_self_priv = DISABLED;

    (void)dal_get_user_role_priv_enable(current_user_name, PROPERTY_USERROLE_CONFIGURE_SELF, &configure_self_priv);

    
    if ((g_strcmp0(current_user_name, target_user) != 0) || (configure_self_priv != ENABLED)) {
        
        if (setcommand_privilege_judge(priv, role_priv_prop) == TRUE) {
            return RET_OK;
        }
    } else {
        return RET_OK;
    }

    return RET_ERR;
}


gint32 ipmc_check_user_exist(gchar *user_name, guint8 *user_id)
{
    gint32 ret;
    guint32 i;

    if (user_name == NULL || user_id == NULL) {
        g_printf("Input null pointer.\r\n");
        return RET_ERR;
    }

    ret = get_user_info();
    if (ret == FALSE) {
        g_printf("Get user info failed.\r\n");
        return RET_ERR;
    }

    
    for (i = DEFAULT_USER_ID; i < G_N_ELEMENTS(g_user_pri_info); i++) {
        if (strcmp(g_user_pri_info[i].username, user_name) == 0) {
            break;
        }
    }

    *user_id = i;
    return RET_OK;
}


void print_port_cmd_helpinfo(void)
{
    gint32 i = 0;
    guint8 ipmi_lan_enabled = 0xff;

    
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_IPMI, &ipmi_lan_enabled);

    if (ipmi_lan_enabled == SERVICE_STATE_DISABLE) {
        g_printf("Usage: ipmcset %s-t service -d port -v <option> <portvalue(1~65535)>\r\n", help_default_name);
    } else {
        g_printf("Usage: ipmcset %s-t service -d port -v <option> <port1value(1~65535)> [port2value(1~65535)]\r\n",
            help_default_name);
    }

    
    g_printf("Options are:\r\n");

    while (g_bmc_cmd_service_port[i].ServiceName) {
        if (g_bmc_cmd_service_port[i].flag == OBVIOUS_CMD) {
            g_printf("       %-23s%-50s\r\n", g_bmc_cmd_service_port[i].ServiceName, g_bmc_cmd_service_port[i].desc);
        }

        i++;
    }
}


GSList *ipmc_create_inputlist(guchar argc, gchar **argv)
{
    guint32 port;
    guint32 port2 = 0;
    GSList *input_list = NULL;
    OBJ_HANDLE rmcp_obj_handle = 0;
    GVariant *property_value = NULL;

    port = (guint32)str_to_int(argv[2]);
    if (port > 65535 || port < 1) {
        print_port_cmd_helpinfo();
        return NULL;
    }

    if (strcasecmp(argv[1], "rmcp") == 0) {
        if (argc == 4) {
            port2 = (guint32)str_to_int(argv[3]);
            if (port2 > 65535 || port2 < 1) {
                print_port_cmd_helpinfo();
                return NULL;
            }
        } else {
            if (dfl_get_object_handle(RMCP_OBJ_NAME, &rmcp_obj_handle) != DFL_OK) {
                g_printf("Get rmcp object failed.\r\n");
                return NULL;
            }
            if (dfl_get_property_value(rmcp_obj_handle, PROPERTY_RMCP_PORT2, &property_value) != DFL_OK) {
                g_printf("Get rmcp port2 failed.\r\n");
                return NULL;
            }

            
            port2 = g_variant_get_uint16(property_value);
            g_variant_unref(property_value);
        }

        input_list = g_slist_append(input_list, g_variant_new_byte(RMCP_PORT_SAVE));
        input_list = g_slist_append(input_list, g_variant_new_uint16((guint16)port));
        input_list = g_slist_append(input_list, g_variant_new_uint16((guint16)port2));
    }
    
    else if ((strcasecmp(argv[1], "snmp") == 0) || (strcasecmp(argv[1], "kvm") == 0) ||
        (strcasecmp(argv[1], "vmm") == 0) || (strcasecmp(argv[1], "video") == 0) || (strcasecmp(argv[1], "vnc") == 0)) {
        input_list = g_slist_append(input_list, g_variant_new_int32(port));
    } else {
        
        input_list = g_slist_append(input_list, g_variant_new_uint16((guint16)port));
    }

    return input_list;
}


gint32 ipmc_match_opt(gchar *str, struct service_method *opt_list, guint8 *service_pos)
{
    guint8 count = 0;

    if (service_pos == NULL) {
        return RET_ERR;
    }

    while (opt_list[count].ServiceName && str) {
        if ((strcasecmp(str, opt_list[count].ServiceName) == 0) && (opt_list[count].flag != UNOBVIOUS_CMD)) {
            *service_pos = count;
            return RET_OK;
        }

        count++;
    }

    return RET_ERR;
}


void print_state_cmd_helpinfo(void)
{
    gint32 i = 0;
    guint8 bmc_type;
    guint8 software_type = 0;
    (void)dal_get_software_type(&software_type);

    g_printf("Usage: ipmcset %s-t service -d state -v <option> <enabled|disabled>\r\n", help_default_name);
    g_printf("Options are:\r\n");

    bmc_type = get_the_bmc_type();

    while (g_bmc_cmd_service_state[i].ServiceName) {
        if (g_bmc_cmd_service_state[i].flag == OBVIOUS_CMD) {
            if (software_type == MGMT_SOFTWARE_TYPE_EM) {
                g_printf("       %-23s%-50s\r\n", g_bmc_cmd_service_state[i].ServiceName,
                    g_bmc_cmd_service_state[i].desc);
            } else {
                
                if ((strcasecmp("ssh", g_bmc_cmd_service_state[i].ServiceName) != 0) || (bmc_type != SMM_BMC)) {
                    g_printf("       %-23s%-50s\r\n", g_bmc_cmd_service_state[i].ServiceName,
                        g_bmc_cmd_service_state[i].desc);
                }
            }
        }

        i++;
    }
}


guint8 get_the_bmc_type(void)
{
    guint8 software_type = 0;
    guint8 type;

    (void)dal_get_software_type(&software_type);
    type = software_type >> 0x4;
    
    if ((type == 0x4) || (type == 0x5)) {
        return SMM_BMC;
    } else {
        return BLADE_BMC_CLI;
    }
}


gint32 ipmc_find_state_cmd(gchar *options, guint8 *service_pos)
{
    guint8 bmc_type = 0;
    gint32 ret;
    guint8 software_type = 0;
    (void)dal_get_software_type(&software_type);

    ret = ipmc_match_opt(options, g_bmc_cmd_service_state, service_pos);
    if (ret == RET_OK) {
        if (software_type == MGMT_SOFTWARE_TYPE_EM) {
            return RET_OK;
        }
        bmc_type = get_the_bmc_type();
        
        if ((strcasecmp("ssh", options) == 0) && (bmc_type == SMM_BMC)) {
            return RET_ERR;
        }

        return RET_OK;
    } else {
        return RET_ERR;
    }
}


gint32 set_service_print_warning(gchar *service)
{
    gboolean flag;
    gint32 ret = RET_OK;

    // 判断是否是不安全的协议
    flag = (strcasecmp(service, "rmcp") == 0) || (strcasecmp(service, "http") == 0) ||
           (strcasecmp(service, "ftp") == 0);
    // 不安全协议，打印提示信息
    if (flag == TRUE) {
        g_printf("WARNING: Enabling the %s functions may reduce system security.Exercise caution when enabling these "
            "functions.\r\n",
            service);
        ret = check_the_input_operation(CLI_COMFIRM_STRING);
    }

    return ret;
}

gint32 ipmc_judge_service_state(guint8 service_type, guint8 service_state)
{
    gint32 ret;
    gint32 input = 0;
    GSList *input_list = NULL;
    GSList *obj_list = NULL;

    
    if (service_type == 0xFF || service_state == 1) {
        return RET_OK;
    }

    ret = dfl_get_object_list(SERVER_PORT_CLASS_NAME, &obj_list);
    if (ret != DFL_OK) {
        g_printf("Get server port object failed.");
        return RET_ERR;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(service_state));
    input_list = g_slist_append(input_list, g_variant_new_byte(service_type));
    ret = uip_call_class_method(NULL, NULL, NULL, SERVER_PORT_CLASS_NAME,
        dfl_get_object_name((OBJ_HANDLE)obj_list->data), SERVER_PORT_METHOD_STATE_CHECK, input_list, NULL);
    g_slist_free(obj_list);
    uip_free_gvariant_list(input_list);

    if (ret != RET_OK) {
        if (ret == -100) {
            g_printf("WARNING: Disabling the SSH, HTTPS, RMCP and RMCP+ services at the same time, you may fail to "
                "connect to the system.\r\n");
            input = ipmc_get_user_input(CLI_COMFIRM_STRING);
            if ((input == 'n') || (input == 'N')) {
                return RET_ERR;
            } else if ((input == 'y') || (input == 'Y')) {
                return RET_OK;
            } else {
                g_printf("Input character invalid.\r\n");
                return RET_ERR;
            }
        }

        g_printf("Service state check failed.\r\n");
        return RET_ERR;
    }

    return RET_OK;
}


gint32 set_rmcp_server(const gchar *ServiceName, const gchar *username, const gchar *ip)
{
    gint32 ret;
    guint8 state;
    guint8 state2;
    guint32 port;
    guint32 port2;
    OBJ_HANDLE rmcp_obj_handle = 0;
    GSList *input_list = NULL;
    GSList *property_name_list = NULL;
    GSList *property_value_list = NULL;

    if ((strcasecmp(ServiceName, "rmcp") != 0) && (strcasecmp(ServiceName, "rmcp+") != 0)) {
        
        return RET_OK;
    }

    if (dfl_get_object_handle(RMCP_OBJ_NAME, &rmcp_obj_handle) != DFL_OK) {
        g_printf("Get rmcp object failed.\r\n");
        return RET_ERR;
    }

    property_name_list = g_slist_append(property_name_list, PROPERTY_LAN_STATE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_LAN_PLUS_STATE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RMCP_PORT1);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RMCP_PORT2);
    ret = dfl_multiget_property_value(rmcp_obj_handle, property_name_list, &property_value_list);
    if (ret != DFL_OK) {
        g_slist_free(property_name_list);
        g_printf("Get rmcp property value failed:%d.\r\n", ret);
        return ret;
    }

    state = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, 0));
    state2 = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, 1));
    port = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value_list, 2));
    port2 = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value_list, 3));

    
    input_list = g_slist_append(input_list, g_variant_new_byte(state | state2));
    input_list = g_slist_append(input_list, g_variant_new_uint16((guint16)port));
    input_list = g_slist_append(input_list, g_variant_new_uint16((guint16)port2));

    ret = uip_call_class_method(OPERATOR_LOG_CLI, username, ip, RMCP_CLASS_NAME, RMCP_OBJ_NAME, METHOD_SET_RMCP_PORT,
        input_list, NULL);
    if (ret != RET_OK) {
        g_printf("Set rmcp server failed:%d.\r\n", ret);
    }

    g_slist_free(property_name_list);
    
    g_slist_free_full(property_value_list, (GDestroyNotify)g_variant_unref);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    

    return ret;
}


void _ipmc_print_trap_mode_usage(void)
{
    g_printf("Usage: ipmcset -t trap -d mode -v <option>\r\n");
    g_printf("Options are:\r\n");
    g_printf("    0     " TRAP_MODE_EVT_STR "  \r\n");
    g_printf("    1     " TRAP_MODE_OID_STR "  \r\n");
    g_printf("    2     " TRAP_MODE_ACC_STR "  \r\n");
}


gint32 get_severity_value(guchar argc, gchar **argv, guint8 *trapseverity)
{
    gint32 count;
    guint8 severity_value = 0;

    if (argv == NULL || trapseverity == NULL) {
        return RET_ERR;
    }

    if (argc == 2) {
        if (!strcmp(argv[1], "none")) {
            *trapseverity = 0;
            return RET_OK;
        } else if (!strcmp(argv[1], "all")) {
            *trapseverity = 0x0F;
            return RET_OK;
        } else {
        }
    }

    for (count = 1; count < argc; count++) {
        if (!strcmp(argv[count], "normal") && !(severity_value & 0x01)) {
            severity_value = severity_value | 0x01;
        } else if (!strcmp(argv[count], "minor") && !(severity_value & 0x02)) {
            severity_value = severity_value | 0x02;
        } else if (!strcmp(argv[count], "major") && !(severity_value & 0x04)) {
            severity_value = severity_value | 0x04;
        } else if (!strcmp(argv[count], "critical") && !(severity_value & 0x08)) {
            severity_value = severity_value | 0x08;
        } else {
            return RET_ERR;
        }
    }

    *trapseverity = severity_value;
    return RET_OK;
}


gint32 ipmc_get_trapcommunity(gchar *trap_community, guint32 trap_community_size)
{
    errno_t safe_fun_ret;
    gchar *community = NULL;
    gint32 ret;

    if (trap_community == NULL) {
        g_printf("Input null pointer.\r\n");
        return RET_ERR;
    }

    if ((community = gettrapcommunity("New Community:")) == NULL) {
        g_printf("Input new community error\r\n");
        return RET_ERR;
    }

    
    safe_fun_ret = strncpy_s(trap_community, trap_community_size, community, strlen(community));
    if (safe_fun_ret != EOK) {
        clear_sensitive_info(community, IPMC_USER_PASSWORD_LEN);
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }
    

    clear_sensitive_info(community, IPMC_USER_PASSWORD_LEN);
    community = NULL;
    if (strlen(trap_community) < SMTP_TRAP_COMMUNITY_MIN_LEN || strlen(trap_community) > SMTP_TRAP_COMMUNITY_MAX_LEN) {
        g_printf("Community length must be between %d to %d.\r\n", SMTP_TRAP_COMMUNITY_MIN_LEN,
            SMTP_TRAP_COMMUNITY_MAX_LEN);
        clear_sensitive_info(trap_community, trap_community_size);
        return RET_ERR;
    }

    ret = vos_check_incorrect_char((const gchar *)trap_community, SMTP_TRAP_COMMUNITY_LIMIT_CHARACTER,
        (gint32)strlen(trap_community));
    if (ret != RET_OK) {
        g_printf("Community string exclude characters:  spaces.\r\n");
        clear_sensitive_info(trap_community, trap_community_size);
        return RET_ERR;
    }

    if ((community = gettrapcommunity("Confirm Community:")) == NULL) {
        g_printf("Confirm community error.\r\n");
        clear_sensitive_info(trap_community, trap_community_size);
        return RET_ERR;
    }

    
    if (strcmp(trap_community, community)) {
        g_printf("Confirm community error.\r\n");
        clear_sensitive_info(trap_community, trap_community_size);
        clear_sensitive_info(community, IPMC_USER_PASSWORD_LEN);
        return RET_ERR;
    }

    clear_sensitive_info(community, IPMC_USER_PASSWORD_LEN);
    return RET_OK;
}


gint32 set_trap_state(guchar state, gchar *str_buf)
{
    gint32 ret;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    GSList *input_list = NULL;

    if (!str_buf) {
        g_printf("set_trap_state:str_buf is null ptr.\n");
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(state));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_TRAP_CONFIG,
        OBJECT_TRAP_CONFIG, METHOD_TRAP_SET_ENABLE, input_list, NULL);
    if (ret != RET_OK) {
        g_printf("Set trap %s failed.\r\n", str_buf);
        uip_free_gvariant_list(input_list);
        return FALSE;
    }

    // PEF包括SNMP Trap、SMTP等，不能通过开关PEF来对Trap使能
    g_printf("Set trap %s successfully.\r\n", str_buf);
    uip_free_gvariant_list(input_list);
    return TRUE;
}


gint32 set_trap_dest_state(guchar dest, guchar state, gchar *str_buf)
{
    gint32 iRet;
    gint32 ret;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    gchar obj_name[OBJ_NAME_MAX_LEN] = {0};
    GSList *input_list = NULL;

    if (str_buf == NULL) {
        g_printf("set_trap_dest_state:str_buf is null ptr.\n");
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(state));
    
    iRet = snprintf_s(obj_name, OBJ_NAME_MAX_LEN, OBJ_NAME_MAX_LEN - 1, "%s_%d", CLASS_TRAP_ITEM_CONFIG, dest - 1);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_TRAP_ITEM_CONFIG,
        obj_name, METHOD_TRAP_ITEM_SET_ENABLE, input_list, NULL);
    if (ret != RET_OK) {
        g_printf("Set trap dest%d %s failed.\r\n", dest, str_buf);
        uip_free_gvariant_list(input_list);
        return FALSE;
    }

    // PEF包括SNMP Trap、SMTP等，不能通过开关PEF来对Trap使能
    g_printf("Set trap dest%d %s successfully.\r\n", dest, str_buf);
    uip_free_gvariant_list(input_list);
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

#define IPMC_TRAP_COMMUNITY_LEN 32

LOCAL gint32 trapcommunity_judge(gint32 ch)
{
    if (((ch >= 26) && (ch <= 127)) || (ch == BACKSPACE)) {
        return TRUE;
    } else {
        return FALSE;
    }
}

LOCAL gchar *gettrapcommunity(const gchar *prompt)
{
    gint32 i = 0, ch;
    static gchar p[IPMC_USER_PASSWORD_LEN] = "";

    g_printf("%s", prompt);

    while (((ch = getch()) != -1) && (ch != ENTER) && trapcommunity_judge(ch)) {
        if ((i == IPMC_TRAP_COMMUNITY_LEN) && (ch != BACKSPACE) && (ch != 0x7f)) {
            
            
            g_printf("\nThe length of trap community is too long.\r\n");
            g_printf("trap community should less than 33 characters.\r\n");
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
        while (i != -1) {
            p[i--] = '\0';
        }

        return NULL;
    }

    p[i] = '\0';
    g_printf("\n");
    return p;
}