
#include <stdlib.h>
#include <pwd.h>
#include "ipmctypedef.h"
#include "pme_app/uip/uip.h"
#include "pme_app/uip/ipmc_public.h"
#include "pme_app/smlib/sml_base.h"
#include "ipmcsethelp.h"
#include "ipmcsetcommand.h"
#include "shelfsetcommand.h"
#include "ipmc_shelf_common.h"
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "ipmcsetmain.h"


LOCAL struct helpinfo g_bmc_cmd_list1[] = {
    {
        "fanmode",          "Set fan mode,you can choose manual or auto", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0,
        PROTERY_PME_SERVICECONFIG_COOLING, "1", ipmc_set_fanmode
    },
    {
        "fanlevel",         "Set fanlevel",                      PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0,
        PROTERY_PME_SERVICECONFIG_COOLING, "1", ipmc_set_fanlevel
    },
    {
        "reset",            "Reboot iBMC system",                         PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_stop_watchdog
    },
    
    {
        "identify",         "Operate identify led",                       PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, LED_CLASS_NAME, 0, LED_ID, "4", ipmc_identify_led
    },
    
    {
        "upgrade",          "Upgrade component",                          PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_upgrade_component
    },
    {
        "clearcmos",        "Clear CMOS",                                 PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_X86,
        "1", ipmc_clear_cmos
    },
    {
        "bootdevice",       "Set boot device",                            PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_X86,
        "1", ipmc_set_bootdevice
    },
    {
        "shutdowntimeout",  "Set graceful shutdown timeout state and value",        PRIV_OPERATOR,
        PROPERTY_USERROLE_POWERMGNT, SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0,
        PROTERY_PME_SERVICECONFIG_X86, "1", ipmc_set_gracefulshutdowntimeout
    },
    {
        "frucontrol",       "Fru control",                                PRIV_OPERATOR, PROPERTY_USERROLE_POWERMGNT,
        SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_control_fru
    },
    {
        "powerstate",       "Set power state",                            PRIV_OPERATOR, PROPERTY_USERROLE_POWERMGNT,
        SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_powerstate
    },
    {
        "sel",              "Clear SEL",                                  PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_clear_sel
    },
    {
        "adduser",          "Add user",                                   PRIV_ADMIN, PROPERTY_USERROLE_USERMGNT,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_add_user
    },
    {
        "password",         "Modify user password",                       PRIV_ADMIN, PROPERTY_USERROLE_USERMGNT,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_change_user_password
    },
    {
        "deluser",          "Delete user",                                PRIV_ADMIN, PROPERTY_USERROLE_USERMGNT,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_delete_user
    },
    {
        "privilege",        "Set user privilege",                         PRIV_ADMIN, PROPERTY_USERROLE_USERMGNT,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_userprivilege
    },
    {
        "npuworkmode",        "Set NPU work mode",                         PRIV_ADMIN, PROPERTY_USERROLE_USERMGNT,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_NPU, 1, NULL, NULL, ipmc_set_ascend_mode
    },
    {
        "serialdir",        "Set serial direction",                       PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_serialdir
    },
    {
        "serialnode", "Set panel serial direction",
        PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID, OBVIOUS_CMD,
        HIDE_TYPE_PROP, CLASS_NAME_CHASSIS, 0, PROPERTY_CHASSIS_PANEL_SERIAL_SW_NODE_SUPP, "1", ipmc_set_serialnode
    },
    {
        "printscreen",      "Print current screen to iBMC",               PRIV_OPERATOR, PROPERTY_USERROLE_DIAGNOSEMGNT,
        SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_KVM,
        "1", ipmc_set_printscreen
    },
    {
        "rollback",         "Perform a manual rollback",                  PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_rollback
    },
    {
        "timezone",         "Set time zone",                              PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP_REVERSE, BMC_PRODUCT_NAME_APP, 0, BMC_TIMESRC_NAME, "2",
        ipmc_set_time_zone
    },
    { 
        "passwordcomplexity", "Set password complexity check enable state", PRIV_OPERATOR,
      
        PROPERTY_USERROLE_SECURITYMGNT, SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL,
        ipmc_set_pass_complexity_check_enable
    },
    {
        "ipaddr",   "Set ip address", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_NETCONFIG, "1",
        ipmc_set_ipaddr
    },
    {
        "backupipaddr",   "Set backup ip address", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_PRODUCT, 0, BMC_PRODUCT_VIRTUAL_NETWORK_ENABLE, "1",
        ipmc_set_backup_ipaddr
    },
    {
        "ipmode",   "Set ip mode", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_NETCONFIG, "1",
        ipmc_set_ipmode
    },
    {
        "gateway",   "Set gateway", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_NETCONFIG, "1",
        ipmc_set_gateway
    },
    {
        "ipaddr6",   "Set ipv6 address", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_NETCONFIG, "1",
        ipmc_set_ipaddr_v6
    },
    {
        "ipmode6",   "Set ipv6 mode", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_NETCONFIG, "1",
        ipmc_set_ipmode_v6
    },
    {
        "gateway6",  "Set ipv6 gateway", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_NETCONFIG, "1",
        ipmc_set_gateway_v6
    },
    {
        "netmode",            "Set net mode", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,  SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, ETH_CLASS_NAME_ETHGROUP, 0, ETH_GROUP_ATTRIBUTE_NET_MODE_FLAG, "1",
        ipmc_set_netmode
    },
    {
        "activeport",        "Set EthGroup active port", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP, ETH_CLASS_NAME_ETHGROUP, 0, ETH_GROUP_ATTRIBUTE_NET_MODE_FLAG,
        "1", ipmc_set_activeport
    },
    {
        "vlan",        "Set sideband vlan", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,  SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, ETH_CLASS_NAME_ETHGROUP, 0, ETH_GROUP_ATTRIBUTE_NET_MODE_FLAG, "1",
        ipmc_set_vlan_state
    },
    {
        "restore",        "Restore factory setting", PRIV_ADMIN, PROPERTY_USERROLE_USERMGNT,  SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_restore_factorysetting
    },
    
    {
        "pcieswitch2upgrade",        "Upgrade pcie switch2 configuration file", PRIV_OPERATOR,
        PROPERTY_USERROLE_BASICSETTING,  SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP, PCIE_SWITCH_CLASEE_NAME, 0,
        PROPERTY_PCIE_SWITCH_TYPE, "1", ipmc_upgrade_pcieswitch_configuration
    },
    
    {
        "notimeout",        "Set no timeout state", PRIV_OPERATOR, PROPERTY_USERROLE_SECURITYMGNT,  SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_notimeout_state
    },
    { 
        "emergencyuser",        "Set emergency user", PRIV_OPERATOR, PROPERTY_USERROLE_SECURITYMGNT,
      
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_emergency_user
    },
    {
        "time",        "set the time", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,  SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_RTC, "1",
        ipmc_set_on_board_rtc_time
    },
    
    
    {
        "autodiscovery",  "Set autodiscovery configuration",  PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_STATELESS, 1, NULL, NULL,
        ipmc_set_autodiscovery_info
    },
    {
        "poweronpermit",  "Set poweronpermit configuration",  PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_STATELESS, 1, NULL, NULL,
        ipmc_set_poweronpermit_info
    },
    
    
    
    {
        "minimumpasswordage", "Set minimum password age configuration", PRIV_OPERATOR, PROPERTY_USERROLE_SECURITYMGNT,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_pwd_minimum_age
    },
    
    
    {
        "crl",  "Upload CRL file", PRIV_OPERATOR, PROPERTY_USERROLE_SECURITYMGNT,  SYS_LOCKDOWN_FORBID, OBVIOUS_CMD,
        HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_upload_crl,
    },
    
    
    {
        "psuworkmode",  "Set PSU work mode", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,  SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_POWER, "1",
        ipmc_set_psu_workmode
    },
    

    
#ifdef SECURITY_ENHANCED_COMPATIBLE_BOARD_V4
    {
        "clearlog",              "Clear Log",                                  PRIV_OPERATOR,
        PROPERTY_USERROLE_SECURITYMGNT,  SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL,
        ipmcset_clear_log
    },
#endif
    

    
    {
        "fpgagoldenfwrestore",       "FPGA golden firmware restore",                           PRIV_OPERATOR,
        PROPERTY_USERROLE_SECURITYMGNT,  SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL,
        ipmc_set_fpga_golden_fw_restore
    },
    
    {
        "failover", "Primary/Secondary failover", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, MSM_NODE_CLASS_NAME, 1, NULL, NULL, vsmm_set_smm_failover
    },
    
    {
        "leakagestrategy",        "Set power-off strategy after leakage", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_PRODUCT, 0, PROPERTY_PRODUCT_LEAKDET_SUPPORT,
        "1", ipmc_set_pwroff_policy_after_leakage
    },

    {
        "shelfpowercontrol",       "Shelf power state control", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_SHELF_PAYLOAD, 1, NULL, NULL, smm_shelf_power_control
    },
    {
        "powerbuttonmode",       "Set shelf power button mode", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_SHELF_PAYLOAD, 1, NULL, NULL,
        smm_set_power_button_mode
    },
    {
        "psuswitch", "Set psu switch", PRIV_OPERATOR, PROPERTY_USERROLE_POWERMGNT, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_POWER_MGNT, 1, NULL, NULL, ipmc_set_ps_switch
    },
    {NULL},
};

struct helpinfo *get_g_bmc_cmd_list1(void)
{
    return g_bmc_cmd_list1;
}

LOCAL struct helpinfo g_bmc_cmd_list2[] = {
    { "fru0",    "Operate with fru0",    0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, NULL },
    { "trap",    "Operate SNMP trap",    0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_TRAP, "1", NULL },
    { "service", "Operate with service", 0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, NULL },
    { "user",    "Operate with user",    0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, NULL },
    {"maintenance", "Operate with maintenance", 0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, NULL},
    { "sensor", "Operate with sensor",   0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, NULL },
    {"powercapping",   "Operate shelf power capping ", 0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_NAME_SHELF_POWER_CAPPING, 1, NULL, NULL, NULL},

    
    {"securitybanner", "Operate login security banner information", 0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_SECURITY_ENHANCE, 1, NULL, NULL, NULL},
    

    {"syslog",    "Operate syslog",    0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, NULL},
    
    {"ntp",    "Operate ntp",    0, NULL, SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, NULL},
    
    
    {"vnc", "Operate vnc",      0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_VNC, "1", NULL},
    
    
    {"storage",    "Configure storage device",    0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_X86, "1", NULL},
    
    {"config ", "Operate configuration",      0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, 
        CLASS_NAME_PME_SERVICECONFIG, 0, PROPERTY_PME_SERVICECONFIG_CFG_IMPORT_EXPORT_SUPPORT, 
        "1", NULL},

    {"vmm", "Operate virtual media",      0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_KVM, "1", NULL},

    {"certificate ", "Operate certificate",    0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_HTTPS, "1", NULL},

    { "resource",    "Operate resource composition",    0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_NAME_COMPOSITION, 1, NULL, NULL, NULL },

    
    {"sol", "Operate SOL", 0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_PRODUCT, 0, PROPERTY_SOL_ENABLE, "1", NULL},
    
    
    {"securityenhance ", "Operate security enhance",    0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, NULL},
    

    
    {"precisealarm", "Operate with precise alarm",   0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, NULL},
    
    {"usbmgmt",   "Operate USB mgmt service", 0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM,
     CLASS_USB_MGMT, 1, NULL, NULL, NULL},
    {"mesh0", "Operate MESH card 0 configuration", 0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM,
     CLASS_LSW_PUBLIC_ATTR_LOCAL_DP, 1, NULL, NULL, NULL},
    {"lsw", "Operate LAN switch chip configuration", 0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM,
     CLASS_LSW_PUBLIC_ATTR_LOCAL_CP, 1, NULL, NULL, NULL},
    {"lldp", "Operate lldp", 0, NULL, SYS_LOCKDOWN_NULL, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM_AND_ACTIVE,
     CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_LLDP, "1", NULL},
    {NULL},
};

struct helpinfo *get_g_bmc_cmd_list2(void)
{
    return g_bmc_cmd_list2;
}

struct helpinfo g_bmc_cmd_fru0[] = {
    {
        "shutdowntimeout", "Set graceful shutdown timeout state and value", PRIV_OPERATOR, PROPERTY_USERROLE_POWERMGNT,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_X86,
        "1", ipmc_set_gracefulshutdowntimeout
    },
    {
        "frucontrol",  "Control fru0",            PRIV_OPERATOR, PROPERTY_USERROLE_POWERMGNT, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_control_fru
    },
    {
        "powerstate",  "Set fru0 power state",          PRIV_OPERATOR, PROPERTY_USERROLE_POWERMGNT, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_powerstate
    },
    {
        "sel",    "Clear SEL",            PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_clear_sel
    },
    {NULL},
};

struct helpinfo g_bmc_cmd_trap[] = {
    {
        "state",    "Set trap enable state",   PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,  SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_TRAP, "1",
        ipmc_set_trapenable
    },
    {
        "severity",    "Set trap send severity",   PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,  SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_TRAP, "1",
        ipmc_set_trap_severity
    },
    {
        "version",    "Set SNMP trap version",   PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,  SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_TRAP, "1",
        ipmc_set_trap_version
    },
    {
        "community", "Set SNMP trap community", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,  SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_TRAP, "1",
        ipmc_set_trapcommunity
    },

    {
        "address",   "Set trap address",   PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,  SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_TRAP, "1",
        ipmc_set_trapdestaddress
    },
    {
        "port",      "Set trap port",      PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,  SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_TRAP, "1",
        ipmc_set_trapport
    },
    {
        "user",  "Set SNMP trap user",  PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,  SYS_LOCKDOWN_FORBID, OBVIOUS_CMD,
        HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_TRAP, "1", ipmc_set_trap_user
    },
    
    {
        "mode",    "Set SNMP trap mode",   PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_TRAP, "1",
        ipmc_set_trap_mode
    },
    
    {NULL},
};

struct helpinfo g_bmc_cmd_service[] = {
    {
        "state", "Set service enable state", PRIV_OPERATOR, PROPERTY_USERROLE_SECURITYMGNT,  SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_service_state
    },
    {
        "port",  "Set service port",         PRIV_OPERATOR, PROPERTY_USERROLE_SECURITYMGNT,  SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_service_port
    },
    {NULL},
};


struct helpinfo g_bmc_cmd_user[] = {
    {
        "adduser",          "Add user",                                   PRIV_ADMIN, PROPERTY_USERROLE_USERMGNT,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_add_user
    },
    {
        "password",         "Modify user password",                       PRIV_ADMIN, PROPERTY_USERROLE_USERMGNT,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_change_user_password
    },
    {
        "deluser",          "Delete user",                                PRIV_ADMIN, PROPERTY_USERROLE_USERMGNT,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_delete_user
    },
    {
        "privilege",        "Set user privilege",                         PRIV_ADMIN, PROPERTY_USERROLE_USERMGNT,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_userprivilege
    },
    {
        "passwordcomplexity", "Set password complexity check enable state", PRIV_OPERATOR,
        PROPERTY_USERROLE_SECURITYMGNT, SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL,
        ipmc_set_pass_complexity_check_enable
    },
    {
        "emergencyuser",    "Set emergency user",                         PRIV_ADMIN, PROPERTY_USERROLE_USERMGNT,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_emergency_user
    },
    
    {
        "lock",             "Set user lock state",                        PRIV_ADMIN, PROPERTY_USERROLE_USERMGNT,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_user_lock
    },
    
    {
        "unlock",           "Set user unlock state",                     PRIV_ADMIN, PROPERTY_USERROLE_USERMGNT,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_user_unlock
    },
    {
        "addpublickey",     "Add SSH public key",                         PRIV_ADMIN, PROPERTY_USERROLE_USERMGNT,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SSH_PUBLICKEY_ENABLE, "1",
        ipmc_add_public_key
    },
    {
        "delpublickey",     "Delete SSH public key",                      PRIV_ADMIN, PROPERTY_USERROLE_USERMGNT,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SSH_PUBLICKEY_ENABLE, "1",
        ipmc_delete_public_key
    },
    {
        "sshpasswordauthentication", "Set SSH password authentication state",   PRIV_ADMIN, PROPERTY_USERROLE_USERMGNT,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SSH_PUBLICKEY_ENABLE, "1",
        ipmc_set_ssh_password_authentication
    },
    
    {"interface", "Set user login interface", PRIV_ADMIN, PROPERTY_USERROLE_USERMGNT,  SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_user_login_interface},
    

    
    {
        "usermgmtbyhost",  "Set user management function on the host side", PRIV_ADMIN, PROPERTY_USERROLE_USERMGNT,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_usermgnt_switch,
    },
    
    
    {
        "snmpprivacypassword",      "Set snmp privacy password",                PRIV_ADMIN, PROPERTY_USERROLE_USERMGNT,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_snmp_privacy_password,
    },
    
    
    {
        "weakpwddic",  "Manage weak password dictionary", PRIV_OPERATOR, PROPERTY_USERROLE_SECURITYMGNT,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0,
        PROPERTY_PRODUCT_WEAK_PWDDICT_SUPPORT, "1", ipmc_set_weak_pwddic,
    },
    
    
    {
        "state", "Enable/Disable user", PRIV_ADMIN, PROPERTY_USERROLE_USERMGNT, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_user_state
    },
    
#ifndef SECURITY_ENHANCED_COMPATIBLE_BOARD_V3
    {
        "firstloginpolicy", "Set user first login password reset policy", PRIV_ADMIN, PROPERTY_USERROLE_USERMGNT,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_user_first_login_policy,
    },
#endif
    {NULL},
};


struct helpinfo g_bmc_cmd_maintenance[] = {
    {"upgradecpld", "Upgrade CPLD", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,  SYS_LOCKDOWN_FORBID,
     OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_X86, "1",
     ipmc_set_maintenance_upgrade_cpld},
    {"upgradebios", "Upgrade BIOS", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,  SYS_LOCKDOWN_FORBID,
     OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_X86, "1", ipmc_set_maintenance_upgrade_bios},
    {"download", "Download files", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,  SYS_LOCKDOWN_ALLOW,
     OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_X86, "1", ipmc_set_maintenance_download},
    // 新需求添加 AR586D337A-B0EF-49da-B718-473CC926DAA9
    
    {"biosprint", "BIOS print set", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
     OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_PRODUCT, 0, PROTERY_PME_SERVICECONFIG_BIOSPRINT, "1", ipmc_set_maintenance_biosprint},
    {"coolingpowermode", "Set cooling power mode", PRIV_ADMIN, PROPERTY_USERROLE_BASICSETTING,  SYS_LOCKDOWN_FORBID,
     OBVIOUS_CMD, HIDE_TYPE_PROP, COOLINGCLASS, 0, COOLING_PROPERTY_POWER_MODE_ENABLE, "1", ipmc_set_maintenance_coolingpowermode},
    
    {"ethlink", "Set ethernet port link status", PRIV_ADMIN, PROPERTY_USERROLE_BASICSETTING,  SYS_LOCKDOWN_FORBID,
     OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_X86, "1", ipmc_set_ethlink},
    
    
    {"resetiME", "Reset ARM iME", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
     OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_ARM, "1", ipmc_set_maintenance_resetiME},
    
    
    {
        "raidcom", "Set raid com switch", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,  SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, SOL_CLASS_NAME, 0, PROPERTY_SOL_COM_ID, "6", ipmc_set_maintenance_raidcom
    },
    
    {
        "poweronlock", "Clear power on lock", PRIV_ADMIN, PROPERTY_USERROLE_BASICSETTING,  SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_PAYLOAD, 0, NULL, NULL, ipmc_set_maintenance_ps_on_lock_clear
    },
    {
        "ipaddr", "Set the maintenance IP address of dedicated port", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP, ETH_CLASS_NAME_ETHGROUP, 0, ETH_GROUP_ATTRIBUTE_OUT_TYPE, "8",
        ipmc_set_maintenance_ipaddr
    },
    {
        "psusupplysource", "Set input A or B for PSUs", PRIV_OPERATOR, PROPERTY_USERROLE_POWERMGNT,  SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_PSU_SOURCE_SWITCH_SUPPORT, "1",
        ipmc_set_psu_supply_source
    },
    {NULL},
    
};


struct helpinfo g_bmc_cmd_sensor[] = {
    {
        "test",    "Sensor test", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,  SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_sensor_test
    },
    {
        "state", 	"Sensor state", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,	SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_sensor_state
    },
    {NULL},
};
struct helpinfo g_bmc_cmd_mesh0[] = {
    {
        "multiplepath", "Set mesh multiple path mode",  PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,  
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_mesh_mpath_mode
    },
    {
        "itfportmode", "Set itf port mode", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_lsw_set_mesh_itfportmode
    },
    {NULL},
};


struct helpinfo g_bmc_cmd_blade_lsw[] = {
    {
        "portenable", "Set LAN switch chip port enable state", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_LSW_PUBLIC_ATTR_LOCAL_CP, 1, NULL, NULL,
        ipmc_set_lsw_port_enable
    },
    {
        "omchannel", "Set omchannel mode", PRIV_ADMIN, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_LSW_PORT_ATTR_CENTER, 1, NULL, NULL, smm_set_om_channel_mode
    },
    {
        "cmeshmode", "Set cmeshmode mode", PRIV_ADMIN, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_LSW_PUBLIC_ATTR_LOCAL_CP, 1, NULL, NULL, smm_set_cmesh_mode
    },
    {NULL},
};


struct helpinfo g_bmc_cmd_precisealarm[] = {
    {
        "mock",             "Precise alarm mock",  PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,  SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_precisealarm_mock
    },
    {NULL},
};


struct helpinfo g_bmc_usbmgmt_list[] = {
    {
        "state", "Set USB management service state", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_USB_MGMT, 1, PROPERTY_USB_MGMT_SUPPORT, "1", ipmc_set_usbmgmt_enable_state
    },
    {NULL},
};

struct helpinfo g_bmc_powercapping_list[] = {
    {
        "mode",    "Set shelf power capping mode", PRIV_OPERATOR, PROPERTY_USERROLE_POWERMGNT, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_NAME_SHELF_POWER_CAPPING, 1, NULL, NULL, ipmc_set_powercappinginfo_mode
    },
    {
        "enable",    "Set shelf power capping state", PRIV_OPERATOR, PROPERTY_USERROLE_POWERMGNT, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_NAME_SHELF_POWER_CAPPING, 1, NULL, NULL, ipmc_set_powercappinginfo_enable
    },
    {
        "shelfvalue",    "Set shelf power capping value", PRIV_OPERATOR, PROPERTY_USERROLE_POWERMGNT,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_NAME_SHELF_POWER_CAPPING, 1, NULL, NULL,
        ipmc_set_powercappinginfo_value
    },
    {
        "threshold",    "Set shelf power capping threshold", PRIV_OPERATOR, PROPERTY_USERROLE_POWERMGNT,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_NAME_SHELF_POWER_CAPPING, 1, NULL, NULL,
        ipmc_set_powercappinginfo_threshold
    },
    {
        "bladevalue",    "Set blade power capping value", PRIV_OPERATOR, PROPERTY_USERROLE_POWERMGNT,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_NAME_SHELF_POWER_CAPPING, 1, NULL, NULL,
        ipmc_set_powercappinginfo_bladevalue
    },
    {
        "failaction",    "Set blade power capping failed action", PRIV_OPERATOR, PROPERTY_USERROLE_POWERMGNT,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_NAME_SHELF_POWER_CAPPING, 1, NULL, NULL,
        ipmc_set_powercappinginfo_failaction
    },
    
    {
        "MSPP",    "Set MSPP enable", PRIV_OPERATOR, PROPERTY_USERROLE_POWERMGNT, SYS_LOCKDOWN_FORBID, OBVIOUS_CMD,
        HIDE_TYPE_OBJ_NUM, CLASS_NAME_MSPP, 1, NULL, NULL, ipmc_set_MSPP_enable
    },
    
    {NULL},
};
struct helpinfo g_bmc_cmd_lsw[] = {
    #ifdef ARM64_HI1711_ENABLED
        {
            "portenable", "Set LAN switch chip port enablestate", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
            SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM_AND_ACTIVE, CLASS_LSW_PORT_ATTR_CENTER, 1, NULL, NULL,
            smm_set_portstatus
        },
        {
            "othersmmportenable", "Set other smm LAN switch chip port enablestate", PRIV_OPERATOR,
            PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM_AND_ACTIVE,
            CLASS_LSW_PORT_ATTR_CENTER, 1, NULL, NULL, smm_set_othersmm_portstatus
        },
    #endif
        {NULL},
                                  };


struct helpinfo g_bmc_cmd_security_banner[] = {
    {
        "state", "Set login security banner state", PRIV_OPERATOR, PROPERTY_USERROLE_SECURITYMGNT, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_SECURITY_ENHANCE, 1, NULL, NULL, ipmc_set_security_banner_state
    },
    {
        "content", "Set login security banner content", PRIV_OPERATOR, PROPERTY_USERROLE_SECURITYMGNT,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_SECURITY_ENHANCE, 1, NULL, NULL,
        ipmc_set_security_banner_content
    },
    {NULL},
};


struct helpinfo g_bmc_cmd_syslog[] = {
    {
        "state",    "Set syslog enable state",   PRIV_OPERATOR, PROPERTY_USERROLE_SECURITYMGNT, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_syslogenable
    },
    {
        "auth",    "Set syslog auth type",   PRIV_OPERATOR, PROPERTY_USERROLE_SECURITYMGNT, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_syslog_auth
    },
    {
        "protocol",    "Set syslog protocol",   PRIV_OPERATOR, PROPERTY_USERROLE_SECURITYMGNT, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_syslog_protocol
    },
    {
        "identity",    "Set syslog identity",   PRIV_OPERATOR, PROPERTY_USERROLE_SECURITYMGNT, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_syslog_identity
    },
    {
        "severity",    "Set syslog severity",   PRIV_OPERATOR, PROPERTY_USERROLE_SECURITYMGNT, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_syslog_severity
    },
    {
        "rootcertificate",    "Set syslog root certificate",   PRIV_OPERATOR, PROPERTY_USERROLE_SECURITYMGNT,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_syslog_rootcertificate
    },
    {
        "clientcertificate",    "Set syslog client certificate",   PRIV_OPERATOR, PROPERTY_USERROLE_SECURITYMGNT,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_syslog_clientcertificate
    },
    {
        "address",    "Set syslog dest address",   PRIV_OPERATOR, PROPERTY_USERROLE_SECURITYMGNT, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_syslogdestaddress
    },
    {
        "port",    "Set syslog dest port",   PRIV_OPERATOR, PROPERTY_USERROLE_SECURITYMGNT, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_syslogport
    },
    {
        "logtype",    "Set syslog dest log type",   PRIV_OPERATOR, PROPERTY_USERROLE_SECURITYMGNT, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_syslog_eventsource
    },
    {
        "test",    "Test syslog dest send",   PRIV_OPERATOR, PROPERTY_USERROLE_SECURITYMGNT, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_syslogtest
    },
    {NULL},
};


struct helpinfo g_bmc_cmd_ntp[] = {
    {
        "status", "Set NTP enable status", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_ntp_enable_status
    },
    {
        "mode", "Set NTP mode", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_ntp_mode
    },
    {
        "preferredserver", "Set preferred NTP server address", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_ntp_preferred_server
    },
    {
        "alternativeserver", "Set alternative NTP server address", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_ntp_alternative_server
    },
    {
        "extraserver", "Set extra NTP server address", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_ntp_extra_server
    },
    {
        "authstatus", "Enable auth status", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_ntp_auth_enable_status
    },
    {
        "groupkey", "Import NTP group key", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_ntp_group_key
    },
    {NULL},
};

struct helpinfo g_bmc_cmd_lldp[] = {
    {
        "status", "Set LLDP enable status", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP_AND_ACTIVE, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_LLDP, "1", 
        ipmc_set_lldp_enable_status
    },
    {NULL},
};

struct helpinfo g_bmc_cmd_vnc[] = {
    {
        "password", "Set VNC password", PRIV_OPERATOR, PROPERTY_USERROLE_KVMMGNT, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_vnc_pwd
    },
    {
        "timeout", "Set VNC timeout period(min)", PRIV_OPERATOR, PROPERTY_USERROLE_KVMMGNT, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_vnc_timeout
    },
    {
        "ssl", "Set VNC ssl encryption", PRIV_OPERATOR, PROPERTY_USERROLE_KVMMGNT, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_vnc_ssl_state
    },
    {
        "keyboardlayout", "Set VNC keyboard layout", PRIV_OPERATOR, PROPERTY_USERROLE_KVMMGNT, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_keyboard_layout
    },
    {NULL},
};

struct helpinfo g_bmc_cmd_config_list[] = {
    {
        "import", "import configuration", PRIV_ADMIN, PROPERTY_USERROLE_USERMGNT, SYS_LOCKDOWN_FORBID, OBVIOUS_CMD,
        HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROPERTY_PME_SERVICECONFIG_CFG_IMPORT_EXPORT_SUPPORT, 
        "1", ipmc_set_config_import
    },
    {NULL},
};


struct helpinfo g_bmc_cmd_storage[] = {
    {
        "createld",    "Create a logical drive", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_X86, "1",
        ipmc_create_logical_drive
    },
    {
        "addld",       "Add a logical drive on already existing disk array", PRIV_OPERATOR,
        PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG,
        0, PROTERY_PME_SERVICECONFIG_X86, "1", ipmc_add_logical_drive
    },
    {
        "deleteld",    "Delete a logical drive", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_X86, "1",
        ipmc_delete_logical_drive
    },
    {
        "ldconfig",     "Set logical drive configurations", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_X86,
        "1", ipmc_set_logical_drive_config
    },
    {
        "ctrlconfig",   "Set RAID contoller configurations", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_X86,
        "1", ipmc_set_raid_controller_config
    },
    {
        "pdconfig",     "Set physical drive configurations", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_X86,
        "1", ipmc_set_physical_drive_config
    },
    {NULL},
};


struct helpinfo g_bmc_cmd_vmm_list[] = {
    {
        "connect", "connect virtual media", PRIV_OPERATOR, PROPERTY_USERROLE_VMMMGNT, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_KVM, "1", ipmc_set_vmm_connect
    },
    {
        "disconnect", "disconnect virtual media", PRIV_OPERATOR, PROPERTY_USERROLE_VMMMGNT, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_KVM, "1",
        ipmc_set_vmm_disconnect
    },
    {NULL},
};


struct helpinfo g_bmc_cmd_certificate[] = {
    {
        "import", "import certificate", PRIV_ADMIN, PROPERTY_USERROLE_SECURITYMGNT, SYS_LOCKDOWN_FORBID, OBVIOUS_CMD,
        HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_HTTPS, "1",
        ipmc_set_certificate_import
    },
    {NULL},
};



struct helpinfo g_bmc_cmd_securityenhance[] = {
    
    {
        "inactivetimelimit",  "Set inactive user timelimit(day)", PRIV_OPERATOR, PROPERTY_USERROLE_SECURITYMGNT,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_inactive_time_limit
    },
    {
        "masterkeyupdateinterval",  "Set master key automatic update interval(day)", PRIV_OPERATOR,
        PROPERTY_USERROLE_SECURITYMGNT, SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL,
        ipmc_set_masterkey_update_interval
    },
    {
        "updatemasterkey",  "Update master key", PRIV_OPERATOR, PROPERTY_USERROLE_SECURITYMGNT, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_set_update_masterkey
    },
    {NULL},
};


struct helpinfo g_bmc_cmd_resource_composition[] = {
    {
        "composition", "Set resource composition", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_NAME_COMPOSITION, 1, NULL, NULL, ipmc_set_resource_composition
    },
    {NULL},
};


struct helpinfo g_bmc_cmd_sol[] = {
    {
        "activate", "Set SOL activate", PRIV_OPERATOR, PROPERTY_USERROLE_KVMMGNT, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, CLASS_NAME_PME_PRODUCT, 0, PROPERTY_SOL_ENABLE, "1", ipmc_set_cli_sol_activate
    },
    {
        "deactivate", "Close SOL session", PRIV_ADMIN, PROPERTY_USERROLE_USERMGNT, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, CLASS_NAME_PME_PRODUCT, 0, PROPERTY_SOL_ENABLE, "1", ipmc_set_cli_sol_deactivate
    },
    {
        "timeout", "Set SOL timeout period(min)", PRIV_ADMIN, PROPERTY_USERROLE_SECURITYMGNT, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_PRODUCT, 0, PROPERTY_SOL_ENABLE, "1", ipmc_set_cli_sol_timeout
    },
    {NULL},
};


LOCAL struct helpinfoplus g_bmc_cmd_list_d[] = {
    {g_bmc_cmd_fru0},
    {g_bmc_cmd_trap},
    {g_bmc_cmd_service},
    {g_bmc_cmd_user},
    {g_bmc_cmd_maintenance},
    {g_bmc_cmd_sensor},
    {g_bmc_powercapping_list},
    {g_bmc_cmd_security_banner},
    {g_bmc_cmd_syslog},
    {g_bmc_cmd_ntp},
    {g_bmc_cmd_vnc},
    {g_bmc_cmd_storage},
    {g_bmc_cmd_config_list},
    {g_bmc_cmd_vmm_list},
    {g_bmc_cmd_certificate},
    {g_bmc_cmd_resource_composition},
    {g_bmc_cmd_sol},
    
    {g_bmc_cmd_securityenhance},
    
    
    {g_bmc_cmd_precisealarm},
    
    {g_bmc_usbmgmt_list},
    {g_bmc_cmd_mesh0},
    {g_bmc_cmd_blade_lsw},
    {g_bmc_cmd_lldp},
    {NULL},
}; 

struct helpinfoplus *get_g_bmc_cmd_list_d(void)
{
    return g_bmc_cmd_list_d;
}


struct helpinfo g_bmc_l_blade_d[] = {
    
    {
        "removedeventseverity",  "Set blade removed event severity", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1",
        smm_set_removed_event_severity
    },
    
    {NULL},
};


struct helpinfo g_bmc_l_bladeN_d[] = {
    {
        "ipaddr",         "Set blade ip address", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, smm_set_blade_ipaddr
    },
    {
        
        "ipaddr6",         "Set blade ipv6 address", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, smm_set_blade_ip6addr
        
    },
    {
        "gateway",         "Set blade gateway", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, smm_set_blade_gateway
    },
    {
        "gateway6",         "Set blade ipv6 gateway", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, smm_set_blade_gateway6
    },
    {
        "ipmode",         "Set blade ipmode", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, smm_set_blade_ipmode
    },
    {
        
        "ipmode6",         "Set blade ipv6 mode", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, smm_set_blade_ip6mode
        
    },
    {
        "vlan",           "Set blade sideband vlan", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, ETH_CLASS_NAME_ETHGROUP, 0, ETH_GROUP_ATTRIBUTE_NET_MODE_FLAG, "0",
        smm_set_blade_vlan_info
    },
    {
        "netmode",            "Set blade net mode", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, ETH_CLASS_NAME_ETHGROUP, 0, ETH_GROUP_ATTRIBUTE_NET_MODE_FLAG, "0",
        smm_set_blade_net_mode
    },
    {
        "activeport",         "Set blade EthGroup active port", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP, ETH_CLASS_NAME_ETHGROUP, 0, ETH_GROUP_ATTRIBUTE_NET_MODE_FLAG,
        "0", smm_set_blade_active_port
    },
#ifdef DFT_ENABLED
    {
        "ipmicommand",       "Send ipmi msg", PRIV_ADMIN, PROPERTY_USERROLE_USERMGNT, SYS_LOCKDOWN_FORBID, OBVIOUS_CMD,
        HIDE_TYPE_PROP, CLASS_DFT_STATUS, 0, PROPERTY_FT_MODE_FLAG, "1", ipmc_send_ipmicmd
    },
#endif
    
    {
        "powerstate",  "Set blade powerstate", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_set_powerstate
    },
    
    {
        "frucontrol",  "Control blade state", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_fru_control
    },
    {NULL},
};

struct helpinfo g_bmc_l_swi_d[] = {
    
    {
        "removedeventseverity",  "Set swi removed event severity", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_IPMBETH_BLADE, 0, PROTERY_IPMBETH_BLADE_BLADETYPE,
        "192", smm_set_removed_event_severity
    },
    {NULL},
};

struct helpinfo g_bmc_l_swiN_d[] = {
    {
        "ipaddr",         "Set swi ipaddr", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_set_blade_ipaddr
    },
    {
        "ipaddr6",         "Set swi ipv6 addr", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_set_blade_ip6addr
    },
    {
        "gateway",         "Set swi default gateway", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_set_blade_gateway
    },
    {
        "gateway6",         "Set swi default gateway", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1",
        smm_set_blade_gateway6
    },
    {
        "ipmode",         "Set swi ipmode", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_set_blade_ipmode
    },
    {
        "ipmode6",         "Set swi ipv6 mode", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_set_blade_ip6mode
    },
#ifdef DFT_ENABLED
    {
        "ipmicommand",       "Send ipmi msg", PRIV_ADMIN, PROPERTY_USERROLE_USERMGNT, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD,  HIDE_TYPE_PROP, CLASS_DFT_STATUS, 0, PROPERTY_FT_MODE_FLAG, "1", ipmc_send_ipmicmd
    },
#endif
    
    
    {
        "powerstate",  "Set swi powerstate", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_set_powerstate
    },
    {
        "frucontrol",  "Control swi state", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_fru_control
    },

    
    {
        "offlineconfig",  "Set swi offlineconfig", PRIV_ADMIN, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP_AND_ACTIVE, BMC_PRODUCT_NAME_APP, 0,
        PROPERTY_SHELF_CLI_SHIELD, "1", smm_set_offlineconfig
    },

    { NULL },
};

struct helpinfo g_bmc_l_fantrayN_d[] = {
    {
        "fanspeed",  "Set fan speed", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID, OBVIOUS_CMD,
        HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_set_fan_speed
    },
    { NULL },
};

struct helpinfo g_bmc_l_fantray_d[] = {
    {
        "fancontrolmode", "Set fan control mode", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD,  "1",  smm_set_fan_ctl_mode
    },
    {
        "smartcooling", "Set smart cooling mode", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD,  "1",  smm_set_fan_smartmode
    },
    {
        "fanspeed",  "Set fan speed", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID, OBVIOUS_CMD,
        HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_set_fan_speed
    },
    
    {
        "removedeventseverity",  "Set fan removed event severity", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1",
        smm_set_removed_event_severity
    },
    
    {NULL},
};

struct helpinfo g_bmc_l_pem_d[] = {
    
    {
        "removedeventseverity",  "Set pem removed event severity", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1",
        smm_set_removed_event_severity
    },
    

    { NULL },
};

struct helpinfo g_bmc_l_pemN_d[] = {
    {
        "pemswitch",       "Set pem switch", PRIV_OPERATOR, PROPERTY_USERROLE_POWERMGNT, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_set_ps_switch
    },

    {NULL},
};

struct helpinfo g_bmc_l_lcd_d[] = {
    {NULL},
};



LOCAL struct helpinfo g_bmc_cmd_list_l[] = {
    { "shelf",       "Shelf information", 0, NULL, SYS_LOCKDOWN_NULL, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL },
    { "smm",         "Management board", 0, NULL, SYS_LOCKDOWN_NULL, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_SMM, 1, NULL, NULL, NULL },
    { "blade",       "Blade server,           option: blade", 0, NULL, SYS_LOCKDOWN_NULL, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL },
    { "bladeN",       "Blade server,           option: bladeN", 0, NULL, SYS_LOCKDOWN_NULL, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_ENABLE, "1", NULL },
    { "swi",         "Switch board,           option: swi", 0, NULL, SYS_LOCKDOWN_NULL, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL },
    { "swiN",         "Switch board,           option: swiN (1<=N<=4)", 0, NULL, SYS_LOCKDOWN_NULL, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL },
    { "fantray",     "Fan module,             option: fantray", 0, NULL, SYS_LOCKDOWN_NULL, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL },
    { "fantrayN",     "Fan module,             option: fantrayN (1<=N<=14)", 0, NULL, SYS_LOCKDOWN_NULL, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL },
    { "pem",         "Power entry module,     option: pem", 0, NULL, SYS_LOCKDOWN_NULL, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL },
    { "pemN",         "Power entry module,     option: pemN (1<=N<=6)", 0, NULL, SYS_LOCKDOWN_NULL, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL },
    { NULL },
};

struct helpinfo *get_g_bmc_cmd_list_l(void)
{
    return g_bmc_cmd_list_l;
}


LOCAL struct helpinfo g_bmc_l_smm_d[] = {
    {
        "upgrade",          "Upgrade component",                          PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_upgrade_component
    },
    {
        "reset",            "Reboot SMM system",                         PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_stop_watchdog
    },
    
    {
        "resetothersmm",            "Reboot other SMM system",           PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_reset_othersmm
    },
    
    {
        "time",             "Set the time", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_RTC, "1",
        ipmc_set_on_board_rtc_time
    },
    {
        "timezone",         "Set time zone",                              PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP_REVERSE, BMC_PRODUCT_NAME_APP, 0, BMC_TIMESRC_NAME, "2",
        ipmc_set_time_zone
    },
    {
        "ipaddr",           "Set ip address", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_NETCONFIG, "1",
        ipmc_set_ipaddr
    },
    {
        "sel",              "Clear SEL",                                  PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_clear_sel
    },
    {
        "adduser",          "Add user",                                   PRIV_ADMIN, PROPERTY_USERROLE_USERMGNT,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1",
        ipmc_add_user
    },
    {
        "password",         "Modify user password",                       PRIV_ADMIN, PROPERTY_USERROLE_USERMGNT,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1",
        ipmc_change_user_password
    },
    {
        "deluser",          "Delete user",                                PRIV_ADMIN, PROPERTY_USERROLE_USERMGNT,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1",
        ipmc_delete_user
    },
    {
        "privilege",        "Set user privilege",                         PRIV_ADMIN, PROPERTY_USERROLE_USERMGNT,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1",
        ipmc_set_userprivilege
    },
    {
        "ipmode",           "Set ip mode",  PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP_AND_ACTIVE, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_NETCONFIG,
        "1", ipmc_set_ipmode
    },
    {
        "gateway",          "Set gateway",  PRIV_OPERATOR,  PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP_AND_ACTIVE, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_NETCONFIG,
        "1", ipmc_set_gateway
    },
    {
        "ipaddr6",          "Set ipv6 address", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_NETCONFIG, "1",
        ipmc_set_ipaddr_v6
    },
    {
        "ipmode6",          "Set ipv6 mode",    PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP_AND_ACTIVE, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_NETCONFIG,
        "1", ipmc_set_ipmode_v6
    },
    {
        "gateway6",         "Set ipv6 gateway", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP_AND_ACTIVE, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_NETCONFIG,
        "1", ipmc_set_gateway_v6
    },
    {
        "netmode",          "Set net mode", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, ETH_CLASS_NAME_ETHGROUP, 0, ETH_GROUP_ATTRIBUTE_NET_MODE_FLAG, "1",
        ipmc_set_netmode
    },
    {
        "poweroninterval",          "Set power on interval time", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM_AND_ACTIVE, CLASS_NAME_SHELF_MANAGE, 1, NULL, NULL,
        smm_set_poweroninterval
    },
    {
        "bladepowercontrol",          "Set blade power control switch", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM_AND_ACTIVE, CLASS_NAME_SHELF_MANAGE, 1, NULL, NULL,
        smm_set_bladepowercontrol
    },
    {
        "restore",        "Restore factory setting", PRIV_ADMIN, PROPERTY_USERROLE_USERMGNT, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_restore_factorysetting
    },
    {
        "soltimeout",          "Set sol timeout", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_NAME_SMM_SOL, 1, NULL, NULL, smm_sol_set_timeout
    },
    
    {
        "floatipaddr",   "Set float ip address", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP_AND_ACTIVE, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_NETCONFIG,
        "1", ipmc_set_float_ipaddr
    },
    {
        "floatipaddr6",   "Set float ipv6 address", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP_AND_ACTIVE, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_NETCONFIG,
        "1", ipmc_set_float_ipaddr_v6
    },
    {
        "deletefloatip",        "Delete float ip address", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP_AND_ACTIVE, CLASS_NAME_PME_SERVICECONFIG, 0,
        PROTERY_PME_SERVICECONFIG_NETCONFIG, "1", ipmc_del_float_ipaddr
    },
    {
        "deletefloatip6",        "Delete float ipv6 address", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP_AND_ACTIVE, CLASS_NAME_PME_SERVICECONFIG, 0,
        PROTERY_PME_SERVICECONFIG_NETCONFIG, "1", ipmc_del_float_ipaddr_v6
    },
    {
        "othersmmipaddr",   "Set other smm ip address", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP_AND_ACTIVE, CLASS_NAME_PME_SERVICECONFIG, 0,
        PROTERY_PME_SERVICECONFIG_NETCONFIG, "1", smm_set_standby_smm_ipaddr
    },
    {
        "othersmmipaddr6",   "Set other smm ipv6 address", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP_AND_ACTIVE, CLASS_NAME_PME_SERVICECONFIG, 0,
        PROTERY_PME_SERVICECONFIG_NETCONFIG, "1", smm_set_standby_smm_ipaddr_v6
    },
    
    {
        "failover", "Active/Standby failover", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, NULL, NULL,  smm_set_smm_failover
    },
    
    {
        "smalertconfig",       "Set smalert config", PRIV_OPERATOR, PROPERTY_USERROLE_POWERMGNT, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_set_smalert_config
    },
    {
        "smalertstatus",       "Set smalert status", PRIV_OPERATOR, PROPERTY_USERROLE_POWERMGNT, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_set_smalert_status
    },
    
    {
        "shelfmanagementstate", "Set shelf management state", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM_AND_ACTIVE, CLASS_NAME_SHELF_MANAGE, 1, NULL, NULL,
        smm_set_shelf_management_state
    },
    {
        "identify",         "Operate identify led", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_set_identify_led
    },
    {
        "coolingmode",       "Set smm coolingmode", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_set_smm_cooling_medium
    },
#ifdef DFT_ENABLED
    {
        "ipmicommand",       "Send ipmi msg", PRIV_ADMIN, PROPERTY_USERROLE_USERMGNT, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_DFT_STATUS, 0, PROPERTY_FT_MODE_FLAG, "1", ipmc_send_ipmicmd
    },
#endif
    {
        "otmportenable",  "Enable SMM management port", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_enable_otm_port
    },
    {
        "stackstate",          "Set stack state", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM_AND_ACTIVE, CLASS_NAME_MM_LSW_MGNT, 1, NULL, NULL, smm_set_stack_state
    },
    {
        "outportmode",          "Set out port mode", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM_AND_ACTIVE, CLASS_NAME_MM_LSW_MGNT, 1, NULL, NULL, smm_set_outport_mode
    },
    
    {
        "removedeventseverity",  "Set SMM removed event severity", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1",
        smm_set_removed_event_severity
    },
    
    {
        "rollback",         "Perform a manual rollback", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_rollback
    },
#ifdef DFT_ENABLED
    {
        "agetest",       "Set lsw agetest config", PRIV_ADMIN, PROPERTY_USERROLE_USERMGNT, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_DFT_STATUS, 0, PROPERTY_FT_MODE_FLAG, "1", ipmc_set_lsw_agetest
    },
#endif
    {
        "omchannel",       "Set lsw om config", PRIV_ADMIN, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM_AND_ACTIVE, CLASS_LSW_PORT_ATTR_CENTER, 1, NULL, NULL, smm_set_om_channel_mode
    },
    {NULL},
};
struct helpinfo *get_g_bmc_l_smm_d(void)
{
    return g_bmc_l_smm_d;
}


struct helpinfo g_bmc_l_shelf_d[] = {
    {
        "psusleepconfig",       "Set psu sleep config", PRIV_OPERATOR, PROPERTY_USERROLE_POWERMGNT, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", shelf_set_psu_sleep_config
    },
    {
        "chassisid",        "Set chassis id", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_set_chassis_id
    },
    {
        "chassisname",      "Set chassis name", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_set_chassis_name
    },
    {
        "location",         "Set chassis location", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_set_chassis_location
    },
    {
        "subnet",   "Set inner subnet segment", PRIV_OPERATOR, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_PROP_AND_ACTIVE, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_NETCONFIG,
        "1", smm_set_inner_subnet_segment
    },
    {NULL},
};

LOCAL struct helpinfo g_bmc_l_smm_t[] = {
    {"sensor", "Operate with sensor",   0, NULL, SYS_LOCKDOWN_NULL, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, NULL},
    {"service", "Operate with service", 0, NULL, SYS_LOCKDOWN_NULL, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL},
    {"user",    "Operate with user",    0, NULL, SYS_LOCKDOWN_NULL, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL},
    {"ntp",    "Operate ntp",           0, NULL, SYS_LOCKDOWN_NULL, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL},
    {"syslog", "Operate syslog",        0, NULL, SYS_LOCKDOWN_NULL, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL},
    { "trap",    "Operate SNMP trap",    0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP_AND_ACTIVE, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_TRAP, "1", NULL },
    {"precisealarm", "Operate with precise alarm",        0, NULL, SYS_LOCKDOWN_NULL, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL},
    {"lsw", "Operate LAN switch chip configuration", 0, NULL, SYS_LOCKDOWN_NULL, OBVIOUS_CMD,
     HIDE_TYPE_OBJ_NUM_AND_ACTIVE, CLASS_LSW_PORT_ATTR_CENTER, 1, NULL, NULL, NULL},
    {"lldp",    "Operate lldp", 0, NULL, SYS_LOCKDOWN_NULL, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM_AND_ACTIVE, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_LLDP, "1", NULL},
    {"securityenhance ", "Operate security enhance",    0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL},
    {NULL},
};
struct helpinfo *get_g_bmc_l_smm_t(void)
{
    return g_bmc_l_smm_t;
}


struct helpinfo g_bmc_l_shelf_t[] = {
    {"powercapping",   "Set shelf power capping configuration",    0, NULL, SYS_LOCKDOWN_NULL, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_NAME_SHELF_POWER_CAPPING, 1, NULL, NULL, NULL},
    {"fru",   "Set shelf fruinfo",    0, NULL, SYS_LOCKDOWN_NULL, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL },
    {NULL},
};

struct helpinfo g_bmc_l_blade_t[] = {
    { NULL },
};

struct helpinfo g_bmc_l_bladeN_t[] = {
    {"sol",   "sol",    0, NULL, SYS_LOCKDOWN_NULL, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_NAME_SMM_SOL,
     1, NULL, NULL, NULL},
    {NULL},
};

struct helpinfo g_bmc_l_swi_t[] = {
    { NULL },
};

struct helpinfo g_bmc_l_swiN_t[] = {
    {"sol",   "sol",    0, NULL, SYS_LOCKDOWN_NULL, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL},
    {NULL},
};

struct helpinfo g_bmc_l_fantray_t[] = {
    { NULL },
};

struct helpinfo g_bmc_l_fantrayN_t[] = {
    {NULL},
};

struct helpinfo g_bmc_l_pem_t[] = {
    {NULL},
};

struct helpinfo g_bmc_l_pemN_t[] = {
    { NULL },
};

struct helpinfo g_bmc_l_lcd_t[] = {
    {NULL},
};

struct helpinfo g_bmc_l_bladeN_t_sol_d[] = {
    {"activate",    "Activate sol", PRIV_OPERATOR, PROPERTY_USERROLE_KVMMGNT, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
     HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL, smm_sol_set_activate},
    {NULL},
};
struct helpinfo g_bmc_l_swiN_t_sol_d[] = {
    {"activate",    "Activate sol", PRIV_OPERATOR, PROPERTY_USERROLE_KVMMGNT, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL, smm_sol_set_activate},
    {NULL},
};


struct helpinfo g_bmc_l_shelf_t_powercapping_d[] = {
    {"mode",       "Set shelf power capping mode",          PRIV_OPERATOR, PROPERTY_USERROLE_POWERMGNT,
     SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL, smm_set_powercapping_mode },
    {"enable",     "Set shelf power capping state",         PRIV_OPERATOR, PROPERTY_USERROLE_POWERMGNT,
     SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL, smm_set_powercapping_enable },
    {"shelfvalue", "Set shelf power capping value",         PRIV_OPERATOR, PROPERTY_USERROLE_POWERMGNT,
     SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL, smm_set_powercapping_value },
    {"threshold",  "Set shelf power capping threshold",     PRIV_OPERATOR, PROPERTY_USERROLE_POWERMGNT,
     SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL, smm_set_powercapping_threshold },
    {"bladevalue", "Set blade power capping value",         PRIV_OPERATOR, PROPERTY_USERROLE_POWERMGNT,
     SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL, smm_set_powercapping_blade_value },
    {"failaction", "Set blade power capping failed action", PRIV_OPERATOR, PROPERTY_USERROLE_POWERMGNT,
     SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL, smm_set_powercapping_fail_action },
    {NULL},
};

struct helpinfo g_bmc_l_shelf_t_fru_d[] = {
    { "shelfserialnumber", "Set shelf serial number", PRIV_OPERATOR, PROPERTY_USERROLE_POWERMGNT, SYS_LOCKDOWN_FORBID,
      OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL, smm_set_shelf_serialnumber },
    { NULL },
};


struct helpinfoplus g_bmc_l_smm_t_d_bond[] = {
    {g_bmc_cmd_sensor},
    {g_bmc_cmd_service},
    {g_bmc_cmd_user},
    {g_bmc_cmd_ntp},
    {g_bmc_cmd_syslog},
    {g_bmc_cmd_trap},
    {g_bmc_cmd_precisealarm},
    {g_bmc_cmd_lsw},
    {g_bmc_cmd_lldp},
    {g_bmc_cmd_securityenhance},
    {NULL},
};

struct helpinfoplus g_bmc_l_shelf_t_d_bond[] = {
    {g_bmc_l_shelf_t_powercapping_d},
    {g_bmc_l_shelf_t_fru_d},
    {NULL},
};

struct helpinfoplus g_bmc_l_blade_t_d_bond[] = {
    { NULL },
};

struct helpinfoplus g_bmc_l_swi_t_d_bond[] = {
    { NULL },
};

struct helpinfoplus g_bmc_l_fantray_t_d_bond[] = {
    { NULL },
};

struct helpinfoplus g_bmc_l_pem_t_d_bond[] = {
    { NULL },
};

struct helpinfoplus g_bmc_l_bladeN_t_d_bond[] = {
    {g_bmc_l_bladeN_t_sol_d},
    {NULL},
};

struct helpinfoplus g_bmc_l_swiN_t_d_bond[] = {
    {g_bmc_l_swiN_t_sol_d},
    {NULL},
};

struct helpinfoplus g_bmc_l_fantrayN_t_d_bond[] = {
    {NULL},
};

struct helpinfoplus g_bmc_l_pemN_t_d_bond[] = {
    {NULL},
};

struct helpinfoplus g_bmc_l_lcd_t_d_bond[] = {
    {NULL},
};


LOCAL struct helpinfoplus g_bmc_cmd_list_l_d_bond[] = {
    {g_bmc_l_shelf_d},
    {g_bmc_l_smm_d},
    {g_bmc_l_blade_d},
    {g_bmc_l_bladeN_d},
    {g_bmc_l_swi_d},
    {g_bmc_l_swiN_d},
    {g_bmc_l_fantray_d},
    {g_bmc_l_fantrayN_d},
    {g_bmc_l_pem_d},
    {g_bmc_l_pemN_d},
    {NULL},
}; 

struct helpinfoplus *get_g_bmc_cmd_list_l_d_bond(void)
{
    return g_bmc_cmd_list_l_d_bond;
}


LOCAL struct helpinfoplus g_bmc_cmd_list_l_t_bond[] = {
    {g_bmc_l_shelf_t},
    {g_bmc_l_smm_t},
    {g_bmc_l_blade_t},
    {g_bmc_l_bladeN_t },
    {g_bmc_l_swi_t},
    {g_bmc_l_swiN_t },
    {g_bmc_l_fantray_t},
    {g_bmc_l_fantrayN_t },
    {g_bmc_l_pem_t },
    {g_bmc_l_pemN_t},
    {NULL},
}; 

struct helpinfoplus *get_g_bmc_cmd_list_l_t_bond(void)
{
    return g_bmc_cmd_list_l_t_bond;
}


LOCAL struct helpinfoplus_plus g_bmc_l_t_d_bond[] = {
    {g_bmc_l_shelf_t_d_bond},
    {g_bmc_l_smm_t_d_bond},
    {g_bmc_l_blade_t_d_bond},
    {g_bmc_l_bladeN_t_d_bond},
    {g_bmc_l_swi_t_d_bond},
    {g_bmc_l_swiN_t_d_bond},
    {g_bmc_l_fantray_t_d_bond},
    {g_bmc_l_fantrayN_t_d_bond},
    {g_bmc_l_pem_t_d_bond},
    {g_bmc_l_pemN_t_d_bond},
    {NULL},
};

struct helpinfoplus_plus *get_g_bmc_l_t_d_bond(void)
{
    return g_bmc_l_t_d_bond;
}




gint32 ipmc_find_matchtarget(const gchar *target)
{
    guint32 i = 0;
    gint32 match_count = 0;
    guint64 record;
    gint64 return_record;

    return_record = ipmc_match_str(target, g_bmc_cmd_list2);
    if (return_record != IPMC_ERROR) {
        record = return_record;
        match_count = 1;
    } else {
        record = 0;

        while (g_bmc_cmd_list2[i].name && target) {
            if (!strncmp(target, g_bmc_cmd_list2[i].name, strlen(target)) && UNOBVIOUS_CMD != g_bmc_cmd_list2[i].flag) {
                record |= ((guint64)1 << i);
                match_count++;
            }

            i++;
        }
    }

    if (match_count == 1) {
        return TRUE;
    }

    if (!match_count) {
        g_printf("Input parameter[-t] error\r\n");
        g_printf("    -t <target>\r\n");
        print_cmd_list(g_bmc_cmd_list2);
    }

    if (match_count > 1) {
        g_printf("    -t <target>\r\n");
        list_special_item(record, g_bmc_cmd_list2);
    }

    return FALSE;
}


gint32 ipmc_find_cmd(gchar *target, gchar *cmd, gint32 argc, gchar **argv)
{
    gint32 matchnum = 0;
    guint64 record = 0;
    gint64 return_record;
    guint32 i = 0;
    guint32 j = 0;
    gint32 ret = 0;

    return_record = ipmc_match_str(target, g_bmc_cmd_list2);
    if (return_record != IPMC_ERROR) {
        record = return_record;
        while (record) {
            if (record & ((guint64)1 << i)) {
                break;
            }

            i++;
        }
    }

    while ((g_bmc_cmd_list2[i].name) && target) {
        ret = (return_record != IPMC_ERROR) || (!strncmp(target, g_bmc_cmd_list2[i].name, strlen(target)));
        if (ret) {
            return_record = ipmc_match_str(cmd, g_bmc_cmd_list_d[i].help_info);
            if (return_record != IPMC_ERROR) {
                record = return_record;
                matchnum = 1;
            } else {
                record = 0;

                while (g_bmc_cmd_list_d[i].help_info[j].name && cmd) {
                    if ((!strncmp(cmd, g_bmc_cmd_list_d[i].help_info[j].name, strlen(cmd))) &&
                        (UNOBVIOUS_CMD != g_bmc_cmd_list_d[i].help_info[j].flag)) {
                        matchnum++;
                        record |= ((guint64)1 << j);
                    }

                    j++;
                }
            }

            
            if (matchnum == 0) {
                g_printf("Input parameter[-d] error\r\n");
                g_printf("    -d <dataitem>\r\n");
                print_cmd_list(g_bmc_cmd_list_d[i].help_info);
                return FALSE;
            }

            
            if (matchnum == 1) {
                j = 0;

                while (record) {
                    if (record & ((guint64)1 << j)) {
                        ret = cli_system_lockdown_check(g_bmc_cmd_list_d[i].help_info[j].syslock_allow);
                        if (ret != RET_OK) {
                            g_printf("Error: Unable to complete the operation because the server is in the "
                                "lockdown mode.\r\n");
                            return TRUE;
                        }
                        if (g_bmc_cmd_list_d[i].help_info[j].func(g_bmc_cmd_list_d[i].help_info[j].privilege,
                            g_bmc_cmd_list_d[i].help_info[j].rolepriv, (guchar)argc, argv, target) == TRUE) {
                        }

                        return TRUE;
                    }

                    j++;
                }
            }

            if (matchnum > 1) {
                g_printf("\r\n    -d <dataitem>\r\n");
                list_special_item(record, g_bmc_cmd_list_d[i].help_info);
                return FALSE;
            }

            return TRUE;
        }

        i++;
    }

    return TRUE;
}


gint32 ipmc_find_matchcmd(gchar *cmd, gint32 argc, gchar **argv)
{
    guint32 i = 0;
    gint32 matchnum = 0;
    guint64 record;
    gint64 return_record;
    gint32 ret = 0;

    return_record = ipmc_match_str(cmd, g_bmc_cmd_list1);
    if (return_record != IPMC_ERROR) {
        record = return_record;
        matchnum = 1;
    } else {
        record = 0;

        while (g_bmc_cmd_list1[i].name && cmd) {
            if ((!strncmp(cmd, g_bmc_cmd_list1[i].name, strlen(cmd))) && (g_bmc_cmd_list1[i].flag != UNOBVIOUS_CMD)) {
                record |= ((guint64)1 << i);
                matchnum++;
            }

            i++;
        }
    }

    if (matchnum == 0) {
        g_printf("Input parameter[-d] error\r\n");
        g_printf("    -d <dataitem>\r\n");
        custom_print_cmd_list(g_bmc_cmd_list1);
    }

    if (matchnum == 1) {
        i = 0;
        while (record) {
            if (record & ((guint64)1 << i)) {
                // 系统锁定检查
                ret = cli_system_lockdown_check(g_bmc_cmd_list1[i].syslock_allow);
                if (ret != RET_OK) {
                    g_printf("Error: Unable to complete the operation because the server is in the lockdown mode.\r\n");
                    return TRUE;
                }

                if (g_bmc_cmd_list1[i].func(g_bmc_cmd_list1[i].privilege, g_bmc_cmd_list1[i].rolepriv, (guchar)argc,
                    argv, NULL) == TRUE) {
                }

                return TRUE;
            }

            i++;
        }
    }

    if (matchnum > 1) {
        g_printf("\r\n    -d <dataitem>\r\n");
        list_special_item(record, g_bmc_cmd_list1);
    }

    return FALSE;
}


void ipmc_list_data(gchar *target)
{
    guint32 i = 0;
    guint64 record = 0;
    gint64 return_record;
    return_record = ipmc_match_str(target, g_bmc_cmd_list2);
    if (return_record != IPMC_ERROR) {
        record = return_record;
        while (record) {
            if (record & ((guint64)1 << i)) {
                break;
            }

            i++;
        }
    }

    while ((g_bmc_cmd_list2[i].name) && target) {
        if ((return_record != IPMC_ERROR) || (!strncmp(target, g_bmc_cmd_list2[i].name, strlen(target)))) {
            g_printf("\r\n    -d <dataitem>\r\n");
            print_cmd_list(g_bmc_cmd_list_d[i].help_info);
            return;
        }

        i++;
    }
}


gint64 ipmc_match_str(const gchar *str, struct helpinfo *cmd_list)
{
    guint32 i = 0;
    guint64 record = 0;

    while (cmd_list[i].name && str) {
        if ((strcmp(str, cmd_list[i].name) == 0) && (cmd_list[i].flag != UNOBVIOUS_CMD)) {
            record |= ((guint64)1 << i);
            return record;
        }

        i++;
    }

    return IPMC_ERROR;
}
