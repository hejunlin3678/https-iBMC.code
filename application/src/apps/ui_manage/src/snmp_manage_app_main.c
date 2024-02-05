
#include "snmp_manage_app.h"

#include "ui_manage_proxy_method.h"

#include "pme_app/kmc/kmc.h"
#include "web_manage.h"
#include "snmp_method.h"
#include "snmp_user_method.h"

gint32 snmp_module_init(void);
gint32 snmp_module_start(void);


LOCAL gint32 reboot_request_process(REBOOT_CTRL ctrl)
{
    switch (ctrl) {
        case REBOOT_PREPARE:
            
            snmp_wait_community_rollback();
            
            break;

        
        case REBOOT_FORCE:
        case REBOOT_PERFORM:
            
            service_rollback_web_server_configure();
            
            revert_file_owner_on_exit();
            stop_webserver();
            (void)per_exit();
            break;

        case REBOOT_CANCEL:
            break;

        default:
            break;
    }

    return RET_OK;
}

BEGIN_MODULE_DECLARATION(MODULE_NAME_UI_MANAGE)

// -- 本模块是否需要延时启动，设置启动延时时间(单位:ms)
DELAY_BEFORE_START(0)

// -- 本模块需要管理的类声明: class name(<=32 bytes)
DECLARATION_CLASS(CLASS_SNMP)
DECLARATION_CLASS(CLASS_PRIVILEGEAGENT)
DECLARATION_CLASS(PROXY_KVM_CLASS_NAME)
DECLARATION_CLASS(PROXY_VMM_CLASS_NAME)
DECLARATION_CLASS(PROXY_VIDEO_CLASS_NAME)

DECLARATION_REBOOT_PROCESS(reboot_request_process)

// -- 类方法实现声明: class name(<=32 bytes), property name(<=32 bytes), callback function
DECLARATION_CLASS_METHOD(CLASS_SNMP, METHOD_SNMP_ADDSNMPUSERNAME, AddSnmpUserName)
DECLARATION_CLASS_METHOD(CLASS_SNMP, METHOD_SNMP_DELSNMPUSERNAME, DelSnmpUserName)
DECLARATION_CLASS_METHOD(CLASS_SNMP, METHOD_SNMP_SETSNMPUSERPASSWORD, SetSnmpUserPassword)
DECLARATION_CLASS_METHOD(CLASS_SNMP, METHOD_SNMP_COMPARESNMPUSERPASSWORD, CompareSnmpUserPassword)
DECLARATION_CLASS_METHOD(CLASS_SNMP, METHOD_SNMP_ADDSNMPUSERPASSWORD, AddSnmpUserPassword)
DECLARATION_CLASS_METHOD(CLASS_SNMP, METHOD_SNMP_DELSNMPUSERPASSWORD, DelSnmpUserPassword)
DECLARATION_CLASS_METHOD(CLASS_SNMP, METHOD_SNMP_GETSNMPUSERINFO, GetSnmpUserInfo)
DECLARATION_CLASS_METHOD(CLASS_SNMP, METHOD_SNMP_GETSNMPUSERHEXINFO, GetSnmpUserHexInfo)
DECLARATION_CLASS_METHOD(CLASS_SNMP, METHOD_SNMP_REBOOTSNMP, RebootSnmp)
DECLARATION_CLASS_METHOD(CLASS_SNMP, METHOD_SNMP_SETSNMPROCOMMUNITY, SetSnmpROCommunity)
DECLARATION_CLASS_METHOD(CLASS_SNMP, METHOD_SNMP_SETSNMPRWCOMMUNITY, SetSnmpRWCommunity)
DECLARATION_CLASS_METHOD(CLASS_SNMP, METHOD_SNMP_GETSNMPROCOMMUNITY, GetSnmpROCommunity)
DECLARATION_CLASS_METHOD(CLASS_SNMP, METHOD_SNMP_GETSNMPRWCOMMUNITY, GetSnmpRWCommunity)
DECLARATION_CLASS_METHOD(CLASS_SNMP, METHOD_SNMP_SETSNMPSTATE, SetSnmpState)
DECLARATION_CLASS_METHOD(CLASS_SNMP, METHOD_SNMP_SETSNMPV1VERSIONSTATE, SetSnmpV1VersionState)
DECLARATION_CLASS_METHOD(CLASS_SNMP, METHOD_SNMP_SETSNMPV2CVERSIONSTATE, SetSnmpV2CVersionState)
DECLARATION_CLASS_METHOD(CLASS_SNMP, METHOD_SNMP_SETSNMPV3VERSIONSTATE, SetSnmpV3VersionState)
DECLARATION_CLASS_METHOD(CLASS_SNMP, METHOD_SNMP_SETSNMPVERSIONSTATE, SetSnmpVersionState)
DECLARATION_CLASS_METHOD(CLASS_SNMP, METHOD_SNMP_SETSNMPPORTID, SetSnmpPortID)
DECLARATION_CLASS_METHOD(CLASS_SNMP, METHOD_SNMP_SETSNMPAUTHPROTOCOL, SetSnmpAuthProtocol)
DECLARATION_CLASS_METHOD(CLASS_SNMP, METHOD_SNMP_SETSNMPPRIVPROTOCOL, SetSnmpPrivProtocol)
DECLARATION_CLASS_METHOD(CLASS_SNMP, METHOD_SNMP_DELETENOACCESSSNMPUSER, DeleteNoAccessSnmpUser)
DECLARATION_CLASS_METHOD(CLASS_SNMP, METHOD_SNMP_SETLONGPASSWORDENABLE, SetLongPasswordEnable)
DECLARATION_CLASS_METHOD(CLASS_SNMP, METHOD_SNMP_UPDATE_PASSWORD_BY_RENAME, UpdateSnmpUserPasswordByRename)

DECLARATION_CLASS_METHOD(CLASS_SNMP, COMMON_PROPERTY_SYNC_METHOD, snmp_config_sync_handler)


DECLARATION_CLASS_METHOD(CLASS_SNMP, METHOD_SNMP_SETSNMPUSERPRIVILEGE, SetSnmpUserPrivilege)


DECLARATION_CLASS_METHOD(CLASS_SNMP, METHOD_SNMP_SETSNMPV1V2CPERMIT, snmp_method_set_permit_rule)


DECLARATION_CLASS_METHOD(CLASS_SNMP, METHOD_SNMP_SET_SYS_CONTACT, SetSnmpStadardSysContact)
DECLARATION_CLASS_METHOD(CLASS_SNMP, METHOD_SNMP_SET_SYS_NAME, SetSnmpStadardSysName)
DECLARATION_CLASS_METHOD(CLASS_SNMP, METHOD_SNMP_SET_SYS_LOCATION, SetSnmpStadardSysLocation)


DECLARATION_CLASS_METHOD(CLASS_SNMP, METHOD_SNMP_SET_IS_UPDATE_ENGINEID, SetIsUpdateEngineID)


DECLARATION_CLASS_METHOD(CLASS_SNMP, METHOD_SNMP_SET_RW_COMMUNITY_STATE, SetSnmpRWCommunityState)


DECLARATION_CLASS_METHOD(CLASS_SNMP, METHOD_SNMP_SET_HOSTNAME_SYNC_FLAG, SetHostNameSyncFlag)
DECLARATION_CLASS_METHOD(CLASS_SNMP, METHOD_SNMP_UPDATE_CIPHERTEXT, kmc_update_snmp_comm)
DECLARATION_CLASS_METHOD(CLASS_PRIVILEGEAGENT, METHOD_PRIVILEGEAGENT_COPYFILE, CopyFile)
DECLARATION_CLASS_METHOD(CLASS_PRIVILEGEAGENT, METHOD_PRIVILEGEAGENT_MODIFY_FILE_MODE, ModifyFileMode)
DECLARATION_CLASS_METHOD(CLASS_PRIVILEGEAGENT, METHDO_PRIVILEGEAGENT_MODIFY_FILE_OWN, ModifyFileOwn)
DECLARATION_CLASS_METHOD(CLASS_PRIVILEGEAGENT, METHDO_PRIVILEGEAGENT_REVERT_FILE_OWN, RevertFileOwn)
DECLARATION_CLASS_METHOD(CLASS_PRIVILEGEAGENT, METHOD_PRIVILEGEAGENT_DELETE_FILE, DeleteFile)
DECLARATION_CLASS_METHOD(CLASS_PRIVILEGEAGENT, METHOD_PRIVILEGEAGENT_CREATE_FILE, CreateFile)
#ifdef ARM64_HI1711_ENABLED
DECLARATION_CLASS_METHOD(CLASS_PRIVILEGEAGENT, METHOD_PRIVILEGEAGENT_SEND_RECV_MSG_WITH_M3, send_recv_msg_with_m3)
#endif


DECLARATION_CLASS_METHOD(CLASS_PRIVILEGEAGENT, METHOD_PRIVILEGEAGENT_CHECK_DIR, CheckDir)
ON_CLASS_PROPERTY_EVENT(CLASS_USER, PROPERTY_USER_SNMPPRIVACYPROTOCOL, UpdateSnmpPrivacyProtocol, NULL)

ON_CLASS_PROPERTY_EVENT(CLASS_ENCRYPTKEY, PROPERTY_ENCRYPTKEY_WORKKEY, UpdateSnmpCommunity, NULL)
ON_CLASS_PROPERTY_EVENT(PFN_CLASS_NAME, PFN_UPGRADED_FLAG, RollbackSnmpCommunity, NULL)
ON_CLASS_PROPERTY_EVENT(PFN_CLASS_NAME, PFN_PREROLLBACK_FLAG, RollbackSnmpCommunity, NULL)

ON_CLASS_PROPERTY_EVENT(CLASS_SECURITY_ENHANCE, PROPERTY_TLS_VERSION, ConfigureVirtualHost, NULL)
ON_CLASS_PROPERTY_EVENT(CLASS_SECURITY_ENHANCE, PROPERTY_SECURITY_ANTI_DNS_REBIND,
    HostNameDomainNameDNSRebindChangeCallback, NULL)
ON_CLASS_PROPERTY_EVENT(BMC_CLASEE_NAME, BMC_HOST_NAME, HostNameDomainNameDNSRebindChangeCallback, NULL)
ON_CLASS_PROPERTY_EVENT(CLASS_NAME_DNSSETTING, PROPERTY_DNSSETTING_DOMAINNAME,
    HostNameDomainNameDNSRebindChangeCallback, NULL)
ON_CLASS_PROPERTY_EVENT(MUTUAL_AUTH_CLASS_NAME, MUTUAL_AUTH_STATE, ConfigureVirtualHost, NULL)
ON_CLASS_PROPERTY_EVENT(MUTUAL_AUTH_CLASS_NAME, MUTUAL_AUTH_OCSP, ConfigureVirtualHost, NULL)
ON_CLASS_PROPERTY_EVENT(MUTUAL_AUTH_CLASS_NAME, MUTUAL_AUTH_CRL, ConfigureVirtualHost, NULL)
ON_CLASS_PROPERTY_EVENT(MUTUAL_AUTH_ROOT_CLASS_NAME, MUTUAL_AUTH_ROOT_CERT_HASHID, MutualAuthRootCertChangeCallback,
    NULL)
ON_CLASS_PROPERTY_EVENT(MUTUAL_AUTH_ROOT_CLASS_NAME, MUTUAL_AUTH_ROOT_CERT_HASHVALUE, MutualAuthRootCertChangeCallback,
    NULL)
ON_CLASS_PROPERTY_EVENT(MUTUAL_AUTH_ROOT_CLASS_NAME, MUTUAL_AUTH_ROOT_CERT_CRL_FILEPATH,
    MutualAuthRootCertChangeCallback, NULL)

ON_CLASS_PROPERTY_EVENT(SERVER_PORT_CLASS_NAME, SERVER_PORT_ATTRIBUTE_STATE, ConfigureVirtualHost, NULL)
ON_CLASS_PROPERTY_EVENT(SERVER_PORT_CLASS_NAME, SERVER_PORT_ATTRIBUTE_PORT, ConfigureVirtualHost, NULL)
ON_CLASS_PROPERTY_EVENT(ETH_CLASS_NAME_ETH, ETH_ATTRIBUTE_VLAN_ID, ConfigureVirtualHost, NULL)

ON_CLASS_PROPERTY_EVENT(CLASS_SECURITY_ENHANCE, PROPERTY_SECURITY_SSL_CIPHER_SUITES, ConfigureSSLCipherSuite, NULL)

ON_CLASS_PROPERTY_EVENT(SSL_CLASEE_NAME, SSL_CERT_STATUS, CertStatusChange, NULL)

ON_CLASS_PROPERTY_EVENT(CLASS_SMS, PROPERTY_SMS_READY_BOB_CHANNEL, IpAddrChangeCallback, NULL)
ON_CLASS_PROPERTY_EVENT(ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_ATTRIBUTE_IP_VERSION, IpAddrChangeCallback, NULL)
ON_CLASS_PROPERTY_EVENT(ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_ATTRIBUTE_IP_ADDR, IpAddrChangeCallback, NULL)
ON_CLASS_PROPERTY_EVENT(ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_ATTRIBUTE_IPV6_ADDR, IpAddrChangeCallback, NULL)
ON_CLASS_PROPERTY_EVENT(ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_ATTRIBUTE_SLAAC_IPV6_ADDR, IpAddrChangeCallback, NULL)
ON_CLASS_PROPERTY_EVENT(ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_ATTRIBUTE_IPV6_LOCAL_LINK, IpAddrChangeCallback, NULL)
ON_CLASS_PROPERTY_EVENT(ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_ATTRIBUTE_ACTIVE_PORT, IpAddrChangeCallback, NULL)
ON_CLASS_PROPERTY_EVENT(ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_ATTRIBUTE_TYPE, IpAddrChangeCallback, NULL)
ON_CLASS_PROPERTY_EVENT(ASM_CLASS_NAME, PROPERTY_ASM_AS_STATUS, IpAddrChangeCallback, NULL)
ON_CLASS_PROPERTY_EVENT(CLASS_USB_MGMT, PROPERTY_USB_MGMT_USB_NETWORK_IP_ADDR, IpAddrChangeCallback, NULL)


DECLARATION_CLASS_METHOD(PROXY_KVM_CLASS_NAME, KVM_METHOD_SET_ENABLE, proxy_method_set_kvm_enable)
DECLARATION_CLASS_METHOD(PROXY_KVM_CLASS_NAME, KVM_METHOD_SET_PORT, proxy_method_set_kvm_port)
DECLARATION_CLASS_METHOD(PROXY_KVM_CLASS_NAME, KVM_METHOD_SCREEN_SWITCH, proxy_method_set_screen_switch)
#ifdef COMPATIBILITY_ENABLED
DECLARATION_CLASS_METHOD(PROXY_KVM_CLASS_NAME, KVM_METHOD_SET_ENCRYPT_STATE, proxy_method_set_kvm_encrypt_state)
#endif
DECLARATION_CLASS_METHOD(PROXY_KVM_CLASS_NAME, KVM_METHOD_SET_TIMEOUT, proxy_method_set_kvm_timeout)

DECLARATION_CLASS_METHOD(PROXY_KVM_CLASS_NAME, KVM_METHOD_SET_LOCAL_KVM_STATE, proxy_method_set_local_kvm_state)

DECLARATION_CLASS_METHOD(PROXY_VMM_CLASS_NAME, VMM_METHOD_SET_ENABLE, proxy_method_set_vmm_enable)
DECLARATION_CLASS_METHOD(PROXY_VMM_CLASS_NAME, VMM_METHOD_SET_PORT, proxy_method_set_vmm_port)

#ifdef SECURITY_ENHANCED_COMPATIBLE_BOARD_V1
DECLARATION_CLASS_METHOD(PROXY_VMM_CLASS_NAME, VMM_METHOD_SET_CRYPT_ENABLE, proxy_method_set_vmm_encrypt_state)
#endif
DECLARATION_CLASS_METHOD(PROXY_VIDEO_CLASS_NAME, VIDEO_METHOD_SET_ENABLE, proxy_method_set_video_enable)
DECLARATION_CLASS_METHOD(PROXY_VIDEO_CLASS_NAME, VIDEO_METHOD_SET_PORT, proxy_method_set_video_port)


DECLARATION_CLASS_METHOD(PROXY_VMM_CLASS_NAME, VMM_METHOD_DRVIVER_OPERATION, proxy_method_driver_operation)
DECLARATION_CLASS_METHOD(PROXY_VMM_CLASS_NAME, VMM_METHOD_FORMAT_DEVICE, proxy_method_format_device)

// -- 本模块dump信息导出回调处理函数
END_MODULE_DECLARATION()

BEGIN_IPMI_COMMAND()
ADD_IPMI_COMMAND_V2(NETFN_OEM_REQ, 0x94, "data[3]=21h,data[4]=02h", RMCP_OPERATOR | IPMI_SYS_LOCKDOWN_FORBID,
    PROPERTY_USERROLE_KVMMGNT, 0xff, 0x09, proxy_ipmi_set_kvm_encry_info, NULL)
ADD_IPMI_COMMAND_V2(NETFN_OEM_REQ, 0x94, "data[3]=21h,data[4]=03h", RMCP_OPERATOR | IPMI_SYS_LOCKDOWN_FORBID,
    PROPERTY_USERROLE_VMMMGNT, 0x09, 0x09, proxy_ipmi_set_vmm_encry_info, NULL)
// 查询设置SNMP sysName
ADD_IPMI_COMMAND_V2(NETFN_OEM_REQ, IPMI_OEM_INTERAL_COMMAND, "data[3]=5ah,data[4]=10h,data[5]=00h",
    RMCP_OPERATOR | IPMI_SYS_LOCKDOWN_FORBID, PROPERTY_USERROLE_BASICSETTING, 0x48, 0x09, ipmi_cmd_set_snmp_sys_name,
    NULL)
ADD_IPMI_COMMAND_V2(NETFN_OEM_REQ, IPMI_OEM_INTERAL_COMMAND, "data[3]=5bh,data[4]=10h,data[5]=00h",
    RMCP_OPERATOR | IPMI_SYS_LOCKDOWN_ALLOW, PROPERTY_USERROLE_BASICSETTING, 0x08, 0x06, ipmi_cmd_get_snmp_sys_name,
    NULL)
END_IPMI_COMMAND()


// -- 模块初始化及处理，必填
BEGIN_MODULE_MAIN()
#ifdef ITEST
iTest_Init(argc, argv);
#endif
// -- 本模块实现的初始化函数，可选
MODULE_INIT(snmp_module_init)
MODULE_START(snmp_module_start)
#ifdef ITEST
start_tshell_main(argc, argv);
#endif
END_MODULE_MAIN()

FM_REGISTER_S g_snmp_fm[] =
{
    {SNMP_SHORT_FILE_NAME,   SNMP_PWD_FILE,  FM_PROTECT_POWER_OFF_E, 0, NULL, NULL, 0, "Snmp", METHOD_SNMP_REBOOTSNMP},
    
    {HTTPS_TLS_SHORT_FILE_NAME,   HTTPS_TLS_FILE,  FM_PROTECT_POWER_OFF_E, 0, NULL, NULL, (guint64)APPS_USER_GID << 32 | FM_PRIVILEGE_RGRP, NULL, NULL},
    
    {HTTP_CONF_NAME,   HTTP_CONF,  FM_PROTECT_POWER_OFF_E, 0, NULL, NULL, (guint64)APPS_USER_GID << 32 | FM_PRIVILEGE_RGRP, NULL, NULL},
    {HTTPS_CONF_NAME,   HTTPS_CONF,  FM_PROTECT_POWER_OFF_E, 0, NULL, NULL, (guint64)APPS_USER_GID << 32 | FM_PRIVILEGE_RGRP, NULL, NULL},
    {HTTPS_CIPHER_SUITE_CONF_SHORT_NAME, HTTPS_CIPHER_SUITE_CONF, FM_PROTECT_POWER_OFF_E, 0, NULL, NULL, (guint64)APPS_USER_GID << 32 | FM_PRIVILEGE_RGRP, NULL, NULL},
    {HTTPS_VERIFY_CLIENT_CONF_NAME, HTTPS_VERIFY_CLIENT_CONF, FM_PROTECT_POWER_OFF_E, 0, NULL, NULL,
     (((guint64)APPS_USER_GID << 32) | FM_PRIVILEGE_RGRP), NULL, NULL},
};

LOCAL TASKID g_supervisory_task = 0;

LOCAL void delete_sys_contact(void)
{
#define SNMP_SYS_CONTACT "sysContact"
    FM_FILE_S *snmpd_fp = NULL;
    glong p_start;
    guint32 buffer_count;
    gchar line_buf[MAX_LINE_SIZE] = { 0 };

    snmpd_fp = fm_fopen(SNMP_SHORT_FILE_NAME, "r+");
    if (snmpd_fp == NULL) {
        debug_log(DLOG_ERROR, "%s: open %s failed", __FUNCTION__, SNMP_SHORT_FILE_NAME);
        return;
    }

    p_start = fm_ftell(snmpd_fp);
    if (p_start < 0) {
        fm_fclose(snmpd_fp);
        return;
    }

    while (fm_fgets(line_buf, MAX_LINE_SIZE, snmpd_fp) != NULL) {
        if (g_str_has_prefix(line_buf, SNMP_SYS_CONTACT) == TRUE) {
            buffer_count = strlen(line_buf);
            (void)fm_delete(buffer_count, p_start, snmpd_fp);

            break;
        }

        p_start = fm_ftell(snmpd_fp);
        if (p_start < 0) {
            break;
        }

        (void)memset_s(line_buf, MAX_LINE_SIZE, 0, MAX_LINE_SIZE);
    }

    fm_fclose(snmpd_fp);
}


#ifdef ENABLE_ASAN
int __lsan_do_recoverable_leak_check(void);
LOCAL void mem_leak_handler(int sig)
{
    debug_log(DLOG_ERROR, "mem_leak_handler start");
    __lsan_do_recoverable_leak_check();
    debug_log(DLOG_ERROR, "mem_leak_handler end");
}
#endif


gint32 snmp_module_init(void)
{
    
    #ifdef ENABLE_ASAN
        if (signal(44, mem_leak_handler) == SIG_ERR) {  // 44 表示执行ASAN检测的信号
            debug_log(DLOG_ERROR, "Could not set signal handler");
        }
    #endif

    init_gcov_signal_for_test();
    gint32 ret = 0;
    TASKID refresh_mk_mask = 0;
    TASKID agent_load_task = 0;
    KmcKsfName *ksf_name = NULL;

    g_mutex_init(&g_snmp_mutex);
    g_mutex_init(&g_snmpcomm_mutex);

    
    g_rollback_state = 0;

    
    
    dal_install_sig_action();
    

    
    ret = vos_queue_create(&g_snmpSendQueue);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:%d:Create snmp queue failed!\n", __FUNCTION__, __LINE__);
    }

    ret = fm_init(g_snmp_fm, GET_ARRAY_ITEMS(g_snmp_fm));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:%d:Fail to fm_init\n", __FUNCTION__, __LINE__);
    }

    // 删掉系统联系人,因为含有huawei字样
    delete_sys_contact();

    ret = snmp_configuration_init();
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:%d:SNMP configuration init failed!\n", __FUNCTION__, __LINE__);
    }

    
    init_ssl_cipher_suites();

    
    if (dal_check_if_mm_board() == FALSE) {
        ksf_name = (KmcKsfName *)g_malloc0(sizeof(KmcKsfName));
        if (ksf_name == NULL) {
            return RET_ERR;
        }

        kmc_get_ksf_actual_path(ksf_name);

        ret = vos_task_create(&agent_load_task, "kmc_agent_load_task", (TASK_ENTRY)kmc_agent_load_task,
            (void *)ksf_name, DEFAULT_PRIORITY);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "create kmc_agent_load_task failed, ret = %d", ret);
            g_free(ksf_name);
            return ret;
        }

        
        ret = vos_task_create(&refresh_mk_mask, "refresh_mk_mask", (TASK_ENTRY)kmc_refresh_mk_mask, NULL,
            DEFAULT_PRIORITY);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "create refresh_mk_mask task failed, ret = %d", ret);
        }
    }

    return RET_OK;
}

gint32 snmp_module_start(void)
{
    gint32 ret = 0;
    GThread *operate_snmpd_command_thread = NULL;

    // 创建snmpd进程启停控制的任务
    operate_snmpd_command_thread = g_thread_new("Operate_snmpd", revceive_operate_snmpd_command, NULL);
    g_thread_unref(operate_snmpd_command_thread);

    
    ret = vos_task_create(&g_supervisory_task, "supervisory_task", (TASK_ENTRY)app_supervisory_task, NULL,
        DEFAULT_PRIORITY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:create supervisory task failed\n", __func__);
        g_printf("%s:create supervisory task failed\n", __func__);
    } else {
        g_printf("%s:create supervisory task success\n", __func__);
    }

    
    
    ui_manage_proxy_property_init();
    

    return RET_OK;
}
