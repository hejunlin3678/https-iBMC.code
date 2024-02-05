
#include <stdlib.h>
#include "uip.h"
#include "ipmctypedef.h"
#include "ipmc_public.h"
#include "ipmcgethelp.h"
#include "ipmcgetcommand.h"
#include "shelfgetcommand.h"
#include "ipmc_shelf_common.h"
#include "pme/pme.h"
#include "pme_app/pme_app.h"



LOCAL struct helpinfo g_bmc_cmd_list1[] = {
    {
        "faninfo",  "Get fan mode and the percentage of the fan speed", PRIV_USER, PROPERTY_USERROLE_READONLY,
        SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0,
        PROTERY_PME_SERVICECONFIG_COOLING, "1", ipmc_get_faninfo, },
    
    {
        "port80",   "Get the diagnose code of port 80", PRIV_OPERATOR, PROPERTY_USERROLE_DIAGNOSEMGNT,
        SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0,
        PROTERY_PME_SERVICECONFIG_RESOURCEMONITOR, "1", ipmc_get_port80_status, },
    
    
    
    {
        "diaginfo",  "Get diagnostic info of management subsystem", PRIV_OPERATOR, PROPERTY_USERROLE_SECURITYMGNT,
        SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_diagnose_info
    },
    
    {
        "systemcom",         "Get system com data", PRIV_OPERATOR, PROPERTY_USERROLE_DIAGNOSEMGNT, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_X86, "1",
        ipmc_get_systemcom_info
    },
    {
        "blackbox",       "Get black box data", PRIV_OPERATOR, PROPERTY_USERROLE_DIAGNOSEMGNT,  SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_X86, "1",
        ipmc_get_blackbox_info
    },
    
    {
        "bootdevice",  "Get boot device", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_X86, "1", ipmc_get_biosbootmode
    },
    {
        "shutdowntimeout",         "Get graceful shutdown timeout state and value", PRIV_USER,
        PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0,
        PROTERY_PME_SERVICECONFIG_X86, "1", ipmc_get_gracefulshutdowntimeout
    },
    {
        "powerstate",         "Get power state", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_powerstate
    },
    {
        "health",   "Get health status", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_health
    },
    {
        "healthevents",         "Get health events", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_healthevents
    },
    {
        "sel",    "Print System Event Log (SEL)", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_sel
    },
    {
        "operatelog",  "Print operation log", PRIV_OPERATOR, PROPERTY_USERROLE_SECURITYMGNT, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_operate_log
    },
    {
        "version",   "Get iBMC version", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_version
    },
    
    {
        "serialnumber", "Get system serial number", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_X86, "1",
        ipmc_get_serialnumber
    },
    
    {
        "userlist",  "List all user info", PRIV_ADMIN, PROPERTY_USERROLE_USERMGNT, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_userinfo
    },
    {
        "fruinfo",   "Get fru information", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_fruinfo
    },
    {
        "time",  "Get system time", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_system_time
    },
    
    {
        "macaddr",   "Get mac address", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_X86, "1", ipmc_get_macaddr
    },
    
    {
        "serialdir",        "Get currently connected serial direction", PRIV_USER, PROPERTY_USERROLE_READONLY,
        SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_serialdir
    },
    {
        "serialnode", "Get current panel serial direction",
        PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, CLASS_NAME_CHASSIS, 0, PROPERTY_CHASSIS_PANEL_SERIAL_SW_NODE_SUPP, "1", ipmc_get_serialnode
    },
    {
        "rollbackstatus", "Get rollback status", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_rollbackstatus
    },
    {
        "locationid",    "Get location ID", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, CLASS_NAME_PRODUCT, 0, PROPERTY_PRODUCT_SERVERTYPE, "2", ipmc_get_locationID
    },
    {
        "passwordcomplexity", "Get password complexity check enable status", PRIV_USER, PROPERTY_USERROLE_READONLY,
        SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_pass_complexity_check_enable
    },
    {
        "ledinfo",          "Get led information", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_ledinfo
    },
    {
        "ipinfo", "Get ip information", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_NETCONFIG, "1", ipmc_get_ipinfo
    },
    {
        "ethport", "Get usable eth port", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, ETH_CLASS_NAME_ETHGROUP, 0, ETH_GROUP_ATTRIBUTE_NET_MODE_FLAG, "1", ipmc_get_ethport
    },
    {
        "chassisnum", "Get Chassis Number", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, CLASS_NAME_PRODUCT, 0, PROPERTY_PRODUCT_SERVERTYPE, "1", ipmc_get_chassis_number
    },
    {
        "psuinfo",        "Get PSU component information ", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_PSU, "1",
        ipmc_get_psu_info
    },
    
    {
        "autodiscovery",  "Get autodiscovery configuration",           PRIV_USER, PROPERTY_USERROLE_READONLY,
        SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_STATELESS, 1, NULL, NULL,  ipmc_get_autodiscovery_info
    },
    {
        "poweronpermit",  "Get poweronpermit configuration",           PRIV_USER, PROPERTY_USERROLE_READONLY,
        SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_STATELESS, 1, NULL, NULL,  ipmc_get_poweronpermit_info
    },
    
    
    {
        "minimumpasswordage", "Get minimum password age configuration", PRIV_USER, PROPERTY_USERROLE_READONLY,
        SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_minimum_pwd_age
    },
    

    
    {
        "ntpinfo", "Get NTP information", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_ntp_info
    },
    

    {
        "bbuinfo", "Get BBU module information", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, "BBUModule", 1, NULL, NULL, ipmc_get_bbu_module_info
    },

    {
        "npuworkmode", "Get NPU work mode", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_NPU, 1, NULL, NULL, ipmc_get_ascend_work_mode
    },
    {
        "redundancy", "Get redundancy state", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, MSM_NODE_CLASS_NAME, 1, NULL, NULL, vsmm_get_ms_status
    },
    
    {
        "leakagestrategy",  "Get power-off strategy after leakage", PRIV_USER, PROPERTY_USERROLE_READONLY,
        SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_PRODUCT, 0, PROPERTY_PRODUCT_LEAKDET_SUPPORT,
        "1", ipmc_get_pwroff_policy_after_leakage
    },
    {
        "notimeoutstate", "Get CLI session notimeout state", PRIV_USER, PROPERTY_USERROLE_READONLY,
        SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_notimeoutstate
    },
    {
        "lldpinfo", "Get LLDP information", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_LLDP, "1",
        ipmc_get_lldp_info
    },
    {
        "shelfpowerstate", "Get shelf power state", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_SHELF_PAYLOAD, 1, NULL, NULL, smm_get_shelf_powerstate
    },
    {
        "powerbuttonmode", "Get shelf power button mode", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_SHELF_PAYLOAD, 1, NULL, NULL, smm_get_power_button_mode
    },
    {NULL},
};

struct helpinfo *get_g_bmc_cmd_list1(void)
{
    return g_bmc_cmd_list1;
}

LOCAL struct helpinfo g_bmc_cmd_list2[] = {
    {"fru0",   "Get the information of the fru0", 0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, NULL},
    {"sensor", "Print detailed sensor information", 0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, NULL},
    {"smbios", "Get the information of smbios",     0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, NULL},
    {"trap",   "Get SNMP trap status",              0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_TRAP, "1", NULL},
    {"service", "Get service information",          0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, NULL},
    
    {"maintenance", "Get maintenance information",   0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_NULL, HIDE_TYPE_NULL, 0, NULL, NULL, NULL},
    
    {"powercapping",   "Get shelf power capping info ", 0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_NAME_SHELF_POWER_CAPPING, 1, NULL, NULL, NULL},
    {"syslog",   "Get syslog status",              0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, NULL},
    
    {"user",    "Get the information of user",    0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, NULL},
    
    
    {"securitybanner",    "Get login security banner information", 0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_SECURITY_ENHANCE, 1, NULL, NULL, NULL},
    

    
    {"vnc",    "Get VNC information",    0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_VNC, "1", NULL},
    

    
    {"storage", "Get storage device information",   0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_X86, "1", NULL},
    

    {"config ", "Get configuration information",      0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, 
        HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROPERTY_PME_SERVICECONFIG_CFG_IMPORT_EXPORT_SUPPORT,
        "1", NULL},

    {"vmm",    "Get Virtual Media information",    0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_KVM, "1", NULL},

    
    {"certificate",    "Get SSL certificate information",    0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_HTTPS, "1", NULL},
    

    {"resource",   "Get resource component information", 0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_NAME_ENDPOINT_IDENTIFIER, 1, NULL, NULL, NULL},

    
    {"sol", "Get SOL information", 0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_PRODUCT, 0, PROPERTY_SOL_ENABLE, "1", NULL},
    

    
    {"securityenhance", "Get security enhance information", 0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, NULL},
    
    
    {"otm", "Get OTM information", 0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_PRODUCT, 0, PROPERTY_OTM_DISPLAY_ENABLE, "1", NULL},

    {"asset", "Get asset information", 0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_RACK_ASSET_MGMT, 1, NULL, NULL, NULL},
    {"mesh0",   "Get MESH card 0 information", 0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_LSW_PUBLIC_ATTR_LOCAL_DP, 1, NULL, NULL, NULL},
    {"lsw", "Get LAN switch chip information", 0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM,
     CLASS_LSW_PUBLIC_ATTR_LOCAL_CP, 1, NULL, NULL, NULL},
    {"usbmgmt", "Get usb mgmt service information", 0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM,
     CLASS_USB_MGMT, 1, NULL, NULL, NULL},
    {NULL},
};

struct helpinfo *get_g_bmc_cmd_list2(void)
{
    return g_bmc_cmd_list2;
}

struct helpinfo g_bmc_getcmd_fru0[G_BMC_GETCMD_FRU0] = {
    {
        "shutdowntimeout", "Get graceful shutdown timeout state and value", PRIV_USER, PROPERTY_USERROLE_READONLY,
        SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_X86,
        "1", ipmc_get_gracefulshutdowntimeout
    },
    {
        "health",   "Get fru0 health status", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_health
    },
    {
        "healthevents", "Get fru0 health events", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_healthevents
    },
    {
        "fruinfo",   "Get fru information", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_fruinfo
    },
    {
        "powerstate",         "Get fru0 power state", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_powerstate
    },
    {NULL},
};

struct helpinfo g_bmc_getcmd_lsw[] = {
    {
        "portstatistics",   "Get LAN switch chip port statistics", PRIV_USER, PROPERTY_USERROLE_READONLY,
        SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_LSW_PUBLIC_ATTR_LOCAL_CP, 1, NULL, NULL,
        ipmc_lsw_get_ctrl_portstatistics
    },
    {
        "l2table",   "Get LAN switch chip l2 table", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_LSW_PUBLIC_ATTR_LOCAL_CP, 1, NULL, NULL, ipmc_lsw_get_ctrl_l2table
    },
    {
        "portinfo",   "Get LAN switch chip port information", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_LSW_PUBLIC_ATTR_LOCAL_CP, 1, NULL, NULL, ipmc_lsw_get_ctrl_portinfo
    },
    {
        "chipinfo", "Get LAN switch chip information", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_LSW_PUBLIC_ATTR_LOCAL_CP, 1, NULL, NULL, ipmc_lsw_get_ctrl_chipinfo
    },
    {
        "omchannel", "Get omchannel mode", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_MGMT_SOFTWARE_TYPE, "32", ipmc_get_om_channel_mode
    },
    {
        "cmeshmode", "Get cmesh mode", PRIV_ADMIN, PROPERTY_USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,
        OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_LSW_PUBLIC_ATTR_LOCAL_CP, 1, NULL, NULL, ipmc_get_cmesh_mode
    },
    {NULL},
};

LOCAL struct helpinfo g_bmc_getcmd_mesh[] = {
    {
        "l2table",   "Get MESH card l2 table", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_LSW_PUBLIC_ATTR_LOCAL_DP, 1, NULL, NULL, ipmc_lsw_get_data_l2table
    },
    {
        "portstatistics",   "Get MESH card port statistics", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_LSW_PUBLIC_ATTR_LOCAL_DP, 1, NULL, NULL, ipmc_lsw_get_data_portstatistics
    },
    {
        "portinfo",   "Get MESH card port information", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_LSW_PUBLIC_ATTR_LOCAL_DP, 1, NULL, NULL, ipmc_lsw_get_data_portinfo
    },
    {
        "multiplepath",   "Get MESH multiple path status", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_LSW_PUBLIC_ATTR_LOCAL_DP, 1, NULL, NULL, ipmc_lsw_get_mesh_multiplepath
    },
    {
        "itfportmode",   "Get itf port mode", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_LSW_NODE_MANAGE_DP, 1, NULL, NULL, ipmc_lsw_get_mesh_itfportmode
    },
    {NULL},
};

struct helpinfo g_bmc_sensor_list[] = {
    {
        "list", "List all sensor information", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_allsensorinfo
    },
    {NULL},
};

struct helpinfo g_bmc_smbios_list[] = {
    
    {
        "serialnumber", "Print serialnumber of smbios", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_X86, "1",
        ipmc_get_serialnumber
    },
    
    {
        "systemname",   "Get system name", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_systemname
    },
    {NULL},
};

struct helpinfo g_bmc_service_list[] = {
    {
        "list", "List all service information", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_service_list
    },
    {NULL},
};
// 新需求修改：AR586D337A-B0EF-49da-B718-473CC926DAA9
struct helpinfo g_bmc_cmd_bios_print_maintenance[] = {
    
    {
        "biosprint", "Show BIOS print switch information", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_PRODUCT, 0, PROTERY_PME_SERVICECONFIG_BIOSPRINT, "1",
        ipmc_get_maintenance_biosprint
    },
    {
        "coolingpowermode", "Get cooling power mode", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, COOLINGCLASS, 0, COOLING_PROPERTY_POWER_MODE_ENABLE, "1",
        ipmc_get_maintenance_coolingpowermode
    },
    
    {
        "raidcom", "Get RAID com channel information", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, SOL_CLASS_NAME, 0, PROPERTY_SOL_COM_ID, "6", ipmc_get_maintenance_raidcom
    },
    
    {
        "poweronlock", "Get power on lock information", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_PAYLOAD, 0, NULL, NULL, ipmc_get_maintenance_ps_on_lock_info
    },
    
    {
        "ipinfo", "Get the maintenance IP information of dedicated port", PRIV_USER, PROPERTY_USERROLE_READONLY,
        SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, ETH_CLASS_NAME_ETHGROUP, 0, ETH_GROUP_ATTRIBUTE_OUT_TYPE, "8",
        ipmc_get_maintenance_ipinfo
    },
    {
        "psusupplysource", "Query PSU power supply information", PRIV_USER, PROPERTY_USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID, OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0,
        PROTERY_PME_PSU_SOURCE_SWITCH_SUPPORT, "1", ipmc_get_psu_supply_source
    },
    {NULL},
    
};

struct helpinfo g_bmc_trap_list[] = {
    {
        "state",    "Get trap enable status", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_TRAP, "1", ipmc_get_trapenable
    },
    {
        "severity",   "Get trap send severity", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_TRAP, "1", ipmc_get_trap_severity
    },
    {
        "version",  "Get SNMP trap version", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_TRAP, "1", ipmc_get_trap_version
    },
    {
        "trapiteminfo",   "Get trapitem information", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_TRAP, "1",
        ipmc_get_trapiteminfo
    },
    
    {
        "user",  "Get SNMP trap user",  PRIV_ADMIN, PROPERTY_USERROLE_USERMGNT, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_TRAP, "1", ipmc_get_trap_user
    },
    
    
    {
        "mode",  "Get trap mode", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_TRAP, "1", ipmc_get_trap_mode
    },
    
    {NULL},
};

struct helpinfo g_bmc_powercapping_list[] = {
    {
        "info",    "Get shelf power capping info", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_NAME_SHELF_POWER_CAPPING, 1, NULL, NULL, ipmc_get_powercapping_info
    },
    {NULL},
};

struct helpinfo g_bmc_usbmgmt_list[] = {
    {
        "info",    "Get USB management service information", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_USB_MGMT, 1, PROPERTY_USB_MGMT_SUPPORT, "1", ipmc_get_usbmgmt_info
    },
    {NULL},
};

struct helpinfo g_bmc_syslog_list[] = {
    {
        "state",    "Get syslog enable status", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_syslogenable
    },
    {
        "auth",  "Get syslog auth type", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_syslog_auth
    },
    {
        "protocol",  "Get syslog protocol", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_syslog_protocol
    },
    {
        "identity",   "Get syslog identity", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_syslog_identity
    },
    {
        "severity",   "Get syslog send severity", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_syslog_severity
    },
    {
        "rootcertificate",   "Get syslog root certificate information", PRIV_USER, PROPERTY_USERROLE_READONLY,
        SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_syslog_root_CA_info
    },
    {
        "clientcertificate",   "Get syslog client certificate information", PRIV_USER, PROPERTY_USERROLE_READONLY,
        SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_syslog_client_CA_info
    },
    {
        "iteminfo",   "Get syslog dest item information", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_syslogiteminfo
    },
    {NULL},
};


struct helpinfo g_bmc_user_list[] = {
    {
        "list",   "List all user info", PRIV_ADMIN, PROPERTY_USERROLE_USERMGNT, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_userinfo
    },
    {
        "passwordcomplexity", "Get password complexity check enable status", PRIV_USER, PROPERTY_USERROLE_READONLY,
        SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_pass_complexity_check_enable
    },
    {
        "sshpasswordauthentication",  "Get SSH password authentication status", PRIV_ADMIN, PROPERTY_USERROLE_USERMGNT,
        SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_ssh_password_authentication
    },
    
    {
        "usermgmtbyhost",  "Get user management status on the host side", PRIV_USER, PROPERTY_USERROLE_READONLY,
        SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_usermgnt
    },
    
    {NULL},
};



struct helpinfo g_bmc_cmd_security_banner_list[] = {
    {
        "info", "Get login security banner information", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_SECURITY_ENHANCE, 1, NULL, NULL, ipmc_get_security_banner_info
    },
    {NULL},
};


struct helpinfo g_bmc_cmd_config_list[] = {
    {
        "export", "export configuration", PRIV_ADMIN, PROPERTY_USERROLE_USERMGNT, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROPERTY_PME_SERVICECONFIG_CFG_IMPORT_EXPORT_SUPPORT, 
        "1", ipmc_get_config_export
    },
    {NULL},
};


struct helpinfo g_bmc_cmd_storage_device_list[] = {
    {
        "ctrlinfo",     "Get the information of RAID controller", PRIV_USER, PROPERTY_USERROLE_READONLY,
        SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_X86,
        "1", ipmc_get_raid_controller_info,
    },
    {
        "ldinfo",     "Get the information of logical drive", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_X86, "1",
        ipmc_get_logical_drive_info,
    },
    {
        "pdinfo",     "Get the information of physical drive", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_X86, "1",
        ipmc_get_physical_drive_info,
    },
    {
        "arrayinfo", "Get the information of disk array", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_X86, "1",
        ipmc_get_disk_array_info,
    },
    {NULL},
};


struct helpinfo g_bmc_cmd_vmm_list[] = {
    {
        "info", "Get virtual media information", PRIV_OPERATOR, PROPERTY_USERROLE_VMMMGNT, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_KVM,   "1",
        ipmc_get_vmm_info
    },
    {NULL},
};



struct helpinfo g_bmc_cmd_vnc_list[] = {
    {
        "info", "Get VNC information", PRIV_OPERATOR, PROPERTY_USERROLE_KVMMGNT, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_VNC,   "1", ipmc_get_vnc_info
    },
    {NULL},
};




struct helpinfo g_bmc_cmd_certificate_list[] = {
    {
        "info", "Get server certificate information", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_HTTPS,   "1",
        ipmc_get_certificate_info
    },
    {NULL},
};



struct helpinfo g_bmc_cmd_securityenhance_info[] = {
    {
        "inactivetimelimit", "Get inactive user timelimit(day)", PRIV_USER, PROPERTY_USERROLE_READONLY,
        SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_inactive_timelimit
    },
    {
        "masterkeyupdateinterval", "Get master key automatic update interval(day)", PRIV_USER,
        PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL,
        ipmc_get_update_interval
    },
    {NULL},
};


struct helpinfo g_bmc_cmd_resource_list[] = {
    {
        "components",    "Get resource components information", PRIV_USER, PROPERTY_USERROLE_READONLY,
        SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_NAME_ENDPOINT_IDENTIFIER, 1, NULL, NULL,
        ipmc_get_resource_components_list
    },

    {
        "composition",    "Get resource composition information", PRIV_USER, PROPERTY_USERROLE_READONLY,
        SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_NAME_COMPOSITION, 1, NULL, NULL,
        ipmc_get_resource_compositions_list
    },
    {NULL},
};

struct helpinfo g_bmc_cmd_sol[] = {
    {
        "session", "Get the information of SOL session", PRIV_ADMIN, PROPERTY_USERROLE_USERMGNT, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_PRODUCT, 0, PROPERTY_SOL_ENABLE, "1", ipmc_get_sol_session
    },
    {
        "info", "Get the information of SOL", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, CLASS_NAME_PME_PRODUCT, 0, PROPERTY_SOL_ENABLE, "1", ipmc_get_sol_info
    },
    {NULL},
};



struct helpinfo g_bmc_cmd_otm[] = {
    {
        "info",    "Get OTM manufacture information", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_PME_PRODUCT, 0, PROPERTY_OTM_DISPLAY_ENABLE, "1", ipmc_get_otm_info
    },

    {
        "status",    "Get OTM hardware status", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, CLASS_NAME_PME_PRODUCT, 0, PROPERTY_OTM_DISPLAY_ENABLE, "1", ipmc_get_otm_status
    },
    {NULL},
};

struct helpinfo g_bmc_cmd_asset[] = {
    {
        "rackinfo",    "Get rack level asset information", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_RACK_ASSET_MGMT, 1, NULL, NULL, ipmc_get_rack_asset_info
    },
    {
        "unitinfo",    "Get unit level asset information", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_RACK_ASSET_MGMT, 1, NULL, NULL, ipmc_get_unit_asset_info
    },
    {NULL},
};

LOCAL struct helpinfoplus g_bmc_cmd_list_d[] = {
    {g_bmc_getcmd_fru0},
    {g_bmc_sensor_list},
    {g_bmc_smbios_list},
    {g_bmc_trap_list},
    {g_bmc_service_list},
    {g_bmc_cmd_bios_print_maintenance},
    {g_bmc_powercapping_list},
    {g_bmc_syslog_list},
    {g_bmc_user_list},
    {g_bmc_cmd_security_banner_list},
    {g_bmc_cmd_vnc_list},
    {g_bmc_cmd_storage_device_list},
    {g_bmc_cmd_config_list},
    {g_bmc_cmd_vmm_list},
    {g_bmc_cmd_certificate_list},
    {g_bmc_cmd_resource_list},
    {g_bmc_cmd_sol},
    
    {g_bmc_cmd_securityenhance_info},
    
    
    {g_bmc_cmd_otm},

    {g_bmc_cmd_asset},
    {g_bmc_getcmd_mesh},
    {g_bmc_getcmd_lsw},
    {g_bmc_usbmgmt_list},
    {NULL},
}; 

struct helpinfoplus *get_g_bmc_cmd_list_d(void)
{
    return g_bmc_cmd_list_d;
}



LOCAL struct helpinfo g_bmc_cmd_list_l[] = {
    { "shelf",       "Shelf information,      option: shelf", 0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL },
    { "smm",         "Management board,       option: smm (default)", 0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_SMM, 1, NULL, NULL, NULL },
    { "blade",       "Blade server,           option: blade", 0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL },
    { "bladeN",       "Blade server,           option: bladeN", 0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_ENABLE, "1", NULL },
    { "swi",         "Switch board,           option: swi", 0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL },
    { "swiN",         "Switch board,           option: swiN (1<=N<=4)", 0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL },
    { "fantray",     "Fan module,             option: fantray", 0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL },
    { "fantrayN",     "Fan module,             option: fantrayN (1<=N<=14)", 0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL },
    { "pem",         "Power entry module,     option: pem", 0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL },
    { "pemN",         "Power entry module,     option: pemN (1<=N<=6)", 0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL },
    { NULL },
};
struct helpinfo *get_g_bmc_cmd_list_l(void)
{
    return g_bmc_cmd_list_l;
}


LOCAL struct helpinfo g_bmc_l_smm_d[] = {
    {
        "version",          "Get SMM version", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_version
    },
    {
        "deviceid",          "Get SMM deviceid", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_deviceid
    },
    {
        "health",           "Get SMM health status", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, smm_get_health_status
    },
    {
        "healthevents",     "Get SMM health events", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, smm_get_health_event
    },
    {
        "othersmmhealthevents",     "Get other SMM health events", PRIV_USER, PROPERTY_USERROLE_READONLY,
        SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1",
        smm_get_blade_health_event
    },
    {
        "othersmmhealth",     "Get other SMM health status", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1",
        smm_get_blade_health_status
    },
    {
        "sel",              "Print System Event Log (SEL)", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_sel
    },
    {
        "diaginfo",  "Get diagnostic info of management subsystem", PRIV_OPERATOR, PROPERTY_USERROLE_SECURITYMGNT,
        SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_smm_diagnose_info
    },
    {
        "operatelog",       "Print operation log", PRIV_OPERATOR, PROPERTY_USERROLE_SECURITYMGNT, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_operate_log
    },
    {
        "userlist",         "List all user info", PRIV_ADMIN, PROPERTY_USERROLE_USERMGNT, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_userinfo
    },
    {
        "hotswapstate",     "Get hotswap state",    PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", ipmc_get_hotswapstate
    },
    {
        "listpresent",      "List present parts", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", ipmc_get_listpresent
    },
    {
        "presence",         "Get board presence", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", ipmc_get_blade_presence
    },
    {
        "time",             "Get system time", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_system_time
    },
    {
        "macaddr",          "Get mac address", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_X86, "1", ipmc_get_macaddr
    },
    {
        "passwordcomplexity", "Get password complexity check enable status", PRIV_USER, PROPERTY_USERROLE_READONLY,
        SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_pass_complexity_check_enable
    },
    {
        "ledinfo",          "Get led information", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_ledinfo
    },
    {
        "ipinfo",           "Get ip information", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_NETCONFIG, "1", ipmc_get_ipinfo
    },
    {
        "poweroninterval",      "Get power on interval time", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_NAME_SHELF_MANAGE, 1, NULL, NULL, smm_get_poweroninterval
    },
    {
        "bladepowercontrol",     "Get blade power control switch", PRIV_USER, PROPERTY_USERROLE_READONLY,
        SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_NAME_SHELF_MANAGE, 1, NULL, NULL,
        smm_get_bladepowercontrol
    },
    {
        "soltimeout", "Get sol timeout", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_NAME_SMM_SOL, 1, NULL, NULL, smm_sol_get_timeout
    },
    {
        "solconnectioninfo", "Get sol connection info", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_NAME_SMM_SOL, 1, NULL, NULL, smm_sol_get_connection_info
    },
    
    {
        "floatipinfo", "Get float ip information", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP_AND_ACTIVE, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_NETCONFIG,
        "1", ipmc_get_float_ipinfo
    },
    {
        "othersmmipinfo", "Get other smm ip information", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP_AND_ACTIVE, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_NETCONFIG,
        "1", smm_get_other_smm_ipinfo
    },
    {
        "othersmmfruinfo", "Get other smm fru information", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP_AND_ACTIVE, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1",
        smm_get_other_smm_info
    },
    
    {
        "redundancy", "Get redundancy state", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_NULL, NULL, 0, NULL, NULL, smm_get_as_status
    },
    {
        "datasyncstatus", "Get data sync state", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_NULL, NULL, 0, NULL, NULL, smm_get_data_sync_status
    },
    {
        "smalertconfig",    "Get smalert config", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", ipmc_get_smalert_config
    },
    {
        "smalertstatus",    "Get smalert status", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", ipmc_get_smalert_status
    },
    {
        "slotnumber", "Get slot number", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_NULL, NULL, 0, NULL, NULL, smm_get_slot_number
    },
    {
        "shelfmanagementstate", "Get shelf management state", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP_AND_ACTIVE, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_NETCONFIG,
        "1", smm_get_shelf_management_state
    },
    {
        "coolingmode", "Get blade cooling mode", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP_AND_ACTIVE, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_NETCONFIG, "1",
        smm_get_smm_cooling_medium
    },
    {
        "managementportotminfo", "Get management port optical module info", PRIV_USER, PROPERTY_USERROLE_READONLY,
        SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_NAME_MM_PORT_OTM_INFO, 1, NULL, NULL,
        smm_get_mm_port_optical_module_info
    },
    {
        "stackstate",      "Get stack state", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_NAME_MM_LSW_MGNT, 1, NULL, NULL, smm_get_stack_state
    },
    {
        "outportmode",     "Get out port mode", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_NAME_MM_LSW_MGNT, 1, NULL, NULL, smm_get_outportmode
    },
    {
        "ntpinfo", "Get NTP information", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", ipmc_get_ntp_info
    },
    {
        "removedeventseverity",     "Get smm removed event severity",    PRIV_USER, PROPERTY_USERROLE_READONLY,
        SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_removed_event_severity
    },
    {
        "lswportinfo", "Get lsw port info", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_NULL, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", ipmc_get_lsw_info
    },
    {
        "agetest",       "Get lsw agetest status", PRIV_ADMIN, PROPERTY_USERROLE_USERMGNT, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_DFT_STATUS, 0, PROPERTY_FT_MODE_FLAG, "1", ipmc_get_lsw_agetest_status
    },
    {
        "omchannel", "Get omchannel mode", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM_AND_ACTIVE, CLASS_LSW_PORT_ATTR_CENTER, 1, NULL, NULL, ipmc_get_om_channel_mode
    },
    {
        "lldpinfo", "Get LLDP information", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP_AND_ACTIVE, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_LLDP, "1",
        ipmc_get_lldp_info
    },
    {NULL}
};
struct helpinfo *get_g_bmc_l_smm_d(void)
{
    return g_bmc_l_smm_d;
}


struct helpinfo g_bmc_l_shelf_d[] = {
    {
        "psusleepconfig",     "Get psu sleep config",    PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_NULL, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", ipmc_get_psu_sleep_config
    },
    {
        "psusleepstatus",     "Get psu sleep status",    PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_NULL, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", ipmc_get_psu_sleep_status
    },
    {
        "chassisid",        "Get chassis id", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_NULL, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_get_chassis_id
    },
    {
        "chassisname",      "Get chassis name", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_NULL, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_get_chassis_name
    },
    {
        "location",         "Get chassis location", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_NULL, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_get_chassis_location
    },
    {
        "version",          "Get chassis version", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_NULL, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_get_chassis_version
    },
    {
        "health",           "Get shelf health status", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1",
        smm_get_shelf_health_status
    },
    {
        "healthevents",        "Get shelf health events", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_get_shelf_health_event
    },
    {
        "unhealthylocations",        "Get shelf unhealthy locations", PRIV_USER, PROPERTY_USERROLE_READONLY,
        SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1",
        smm_get_shelf_unhealthy_location
    },
    {
        "realtimepower",        "Get shelf real-time power", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1",
        smm_get_shelf_realtime_power
    },
    {
        "subnet", "Get inner subnet segment", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP_AND_ACTIVE, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_get_inner_subnet
    },
    {NULL},
};

struct helpinfo g_bmc_l_blade_d[] = {
    {
        "removedeventseverity",     "Get blade removed event severity",    PRIV_USER, PROPERTY_USERROLE_READONLY,
        SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1",
        ipmc_get_removed_event_severity
    },
    { NULL },
};

struct helpinfo g_bmc_l_swi_d[] = {
    {
        "removedeventseverity",     "Get swi removed event severity",    PRIV_USER, PROPERTY_USERROLE_READONLY,
        SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, CLASS_NAME_IPMBETH_BLADE, 0, PROTERY_IPMBETH_BLADE_BLADETYPE,
        "192", ipmc_get_removed_event_severity
    },
    { NULL },
};

struct helpinfo g_bmc_l_pemN_d[] = {
    {
        "faultreason",     "Get pem fault reason",    PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", ipmc_get_pem_fault_reason
    },
    {
        "health",     "Get pem status",    PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_get_pem_health_status
    },
    {
        "healthevents",           "Get pem health events", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_get_pem_health_event
    },
    {
        "pemtype",     "Get pem type",    PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", ipmc_get_pem_type
    },
    {
        "presence",     "Get pem presence",    PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", ipmc_get_pem_presence
    },
    {
        "ratingpower",     "Get pem rating power",    PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", ipmc_get_pem_rate_power
    },
    {
        "runtimepower",     "Get pem runtime power",    PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", ipmc_get_pem_real_power
    },
    {
        "version",     "Get pem version",    PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", ipmc_get_pem_version
    },
    { NULL },
};


struct helpinfo g_bmc_l_bladeN_d[] = {
    {
        "hotswapstate",     "Get blade hotswap state",    PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", ipmc_get_hotswapstate
    },
    {
        "presence",          "Get blade presence", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", ipmc_get_blade_presence
    },
    {
        "coolingmode",          "Get blade cooling mode", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1",
        smm_get_blade_cooling_medium
    },
    {
        "health",           "Get blade health status", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1",
        smm_get_blade_health_status
    },
    {
        "healthevents",        "Get blade health events", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_get_blade_health_event
    },
    {
        "userlist",          "Get blade user list", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_get_blade_user_list
    },
    {
        "ipinfo",          "Get blade ip info", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_NULL, NULL, 0, NULL, NULL, smm_get_blade_ip_info
    },
    {
        "realtimepower",          "Get blade real-time power", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1",
        smm_get_blade_real_time_power
    },
    {
        "powercappingvaluerange",          "Get blade power capping value range", PRIV_USER, PROPERTY_USERROLE_READONLY,
        SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1",
        smm_get_blade_power_capping_value_range
    },
    {
        "listpresentfrus",  "Get blade FRUs", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_get_blade_list_frus
    },
    {
        "version",  "Get blade version", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_get_blade_version
    },
    {
        "deviceid",  "Get blade deviceid", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", ipmc_get_deviceid
    },
    {NULL},
};

struct helpinfo g_bmc_l_swiN_d[] = {
    {
        "hotswapstate",     "Get swi hotswap state",    PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", ipmc_get_hotswapstate
    },
    {
        "presence",          "Get swi presence", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", ipmc_get_blade_presence
    },
    {
        "health",           "Get swi health status", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1",
        smm_get_blade_health_status
    },
    {
        "healthevents",        "Get swi health events", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_get_blade_health_event
    },
    {
        "userlist",          "Get swi user list", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_get_blade_user_list
    },
    {
        "ipinfo",          "Get swi ip info", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_get_blade_ip_info
    },
    {
        "listpresentfrus",  "Get swi FRUs", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_get_blade_list_frus
    },
    {
        "version",  "Get swi version", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_get_blade_version
    },
    {
        "realtimepower",          "Get swi real-time power", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1",
        smm_get_blade_real_time_power
    },
    {
        "deviceid",  "Get swi deviceid", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", ipmc_get_deviceid
    },
    { NULL },
};

struct helpinfo g_bmc_l_fantray_d[] = {
    {
        "fancontrolmode", "Get fan control mode", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_get_fan_ctl_mode
    },
    {
        "smartcooling", "Get fan smart cooling mode", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", ipmc_get_fan_smartmode
    },
    {
        "fanspeed", "Get fan speed", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_get_fan_speed
    },
    {
        "presence", "Get fan presence", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_get_fan_presence
    },
    {
        "removedeventseverity",     "Get fan removed event severity",    PRIV_USER, PROPERTY_USERROLE_READONLY,
        SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1",
        ipmc_get_removed_event_severity
    },
    {
        "version", "Get fan version", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_get_fan_version
    },
    {
        "health",           "Get fan health status", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1",
        smm_get_fantray_health_status
    },
    {
        "healthevents",           "Get fan health events", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1",
        smm_get_fantray_health_event
    },
    {
        "coolinglog",           "Get fan cooling log", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_get_fan_log
    },
    { NULL },
};

struct helpinfo g_bmc_l_fantrayN_d[] = {
    {
        "fanspeed", "Get fan speed", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_get_fan_speed
    },
    {
        "presence", "Get fan presence", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_get_fan_presence
    },
    {
        "health",           "Get fan health status", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1",
        smm_get_fantray_health_status
    },
    {
        "healthevents",           "Get fan health events", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1",
        smm_get_fantray_health_event
    },
    {NULL},
};

struct helpinfo g_bmc_l_pem_d[] = {
    {
        "presence",     "Get pem presence",    PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", ipmc_get_pem_presence
    },
    {
        "health",     "Get pem status",    PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_get_pem_health_status
    },
    {
        "healthevents",           "Get pem health events", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", smm_get_pem_health_event
    },
    {
        "version",     "Get pem version",    PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", ipmc_get_pem_version
    },
    {
        "removedeventseverity",     "Get pem removed event severity",    PRIV_USER, PROPERTY_USERROLE_READONLY,
        SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1",
        ipmc_get_removed_event_severity
    },
    {NULL},
};

struct helpinfo g_bmc_l_lcd_d[] = {
    {NULL},
};

LOCAL struct helpinfoplus g_bmc_cmd_list_l_d_bond[] = {
    {g_bmc_l_shelf_d},
    {g_bmc_l_smm_d},
    {g_bmc_l_blade_d },
    {g_bmc_l_bladeN_d},
    {g_bmc_l_swi_d},
    {g_bmc_l_swiN_d },
    {g_bmc_l_fantray_d},
    {g_bmc_l_fantrayN_d },
    {g_bmc_l_pem_d },
    {g_bmc_l_pemN_d},
    {NULL},
}; 

struct helpinfoplus *get_g_bmc_cmd_list_l_d_bond(void)
{
    return g_bmc_cmd_list_l_d_bond;
}


LOCAL struct helpinfo g_bmc_l_smm_t[] = {
    {"sensor",  "Print detailed sensor information", 0, NULL, SYS_LOCKDOWN_NULL, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, NULL},
    {"service", "Get service information",           0, NULL, SYS_LOCKDOWN_NULL, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL},
    {"user",    "Get the information of user",       0, NULL, SYS_LOCKDOWN_NULL, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL },
    {"fru",     "Get fru information",               0, NULL, SYS_LOCKDOWN_NULL, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, NULL},
    {"syslog",  "Get syslog status",                 0, NULL, SYS_LOCKDOWN_NULL, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL},
    {"trap",    "Get SNMP trap status", 0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, 
        HIDE_TYPE_PROP, CLASS_NAME_PME_SERVICECONFIG, 0, PROTERY_PME_SERVICECONFIG_TRAP, "1", NULL},
    {"lsw",     "Get LAN switch chip information", 0, NULL, SYS_LOCKDOWN_NULL, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_LSW_PUBLIC_ATTR_LOCAL_CP, 1, NULL, NULL, NULL},
    {"securityenhance", "Get security enhance information", 0, NULL, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
        HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL},
    {NULL},
};
struct helpinfo *get_g_bmc_l_smm_t(void)
{
    return g_bmc_l_smm_t;
}


struct helpinfo g_bmc_l_shelf_t[] = {
    { "fru",    "Get the fru information", 0, NULL, SYS_LOCKDOWN_NULL, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL },
    { "powercapping",    "Get shelf power capping info", 0, NULL, SYS_LOCKDOWN_NULL, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL },
    {NULL},
};

struct helpinfo g_bmc_l_blade_t[] = {
    { NULL },
};

struct helpinfo g_bmc_l_swi_t[] = {
    { NULL },
};

struct helpinfo g_bmc_l_fantray_t[] = {
    { "fru",    "Get the fru information of fan",    PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
      OBVIOUS_CMD, HIDE_TYPE_NULL, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL },
    { NULL },
};

struct helpinfo g_bmc_l_pem_t[] = {
    { NULL },
};

struct helpinfo g_bmc_l_bladeN_t[] = {
    {"sol",    "Get the information of sol",    0, NULL, SYS_LOCKDOWN_NULL, OBVIOUS_CMD, HIDE_TYPE_OBJ_NUM, CLASS_NAME_SMM_SOL, 1, NULL, NULL, NULL },
    { "fru",   "Get the blade fru information",    0, NULL, SYS_LOCKDOWN_NULL, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL },
    {NULL},
};

struct helpinfo g_bmc_l_swiN_t[] = {
    {"sol",    "Get the information of sol",    0, NULL, SYS_LOCKDOWN_NULL, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL },
    { "fru",   "Get the swi fru information",    0, NULL, SYS_LOCKDOWN_NULL, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL },
    {NULL},
};

struct helpinfo g_bmc_l_fantrayN_t[] = {
    { "fru",    "Get the fru information of fan",    PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
      OBVIOUS_CMD, HIDE_TYPE_NULL, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL },
    {NULL},
};

struct helpinfo g_bmc_l_pemN_t[] = {
    {"fru",    "Get the fru information of pem",    PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
     OBVIOUS_CMD, HIDE_TYPE_NULL, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL },
    {NULL},
};

struct helpinfo g_bmc_l_lcd_t[] = {
    {NULL},
};

struct helpinfo g_bmc_l_blade_t_sensor_d[] = {
    {NULL},
};

struct helpinfo g_bmc_l_smm_t_fru_d[] = {
    { "fruinfo", "Get smm fru information", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_NULL, NULL, 0, NULL, NULL, ipmc_get_fruinfo },
    { NULL },
};

struct helpinfo g_bmc_l_shelf_t_fru_d[] = {
    { "fruinfo", "Get shelf fru information", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
      HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL, smm_get_shelf_fru_info },
    { NULL },
};

struct helpinfo g_bmc_l_shelf_t_powercapping_d[] = {
    { "info", "Get shelf power capping info", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
      HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL, smm_get_power_capping_info },
    { "shelfvaluerange", "Get shelf power capping value range", PRIV_USER, PROPERTY_USERROLE_READONLY,
      SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL, smm_get_power_capping_value_range },
    { NULL },
};

struct helpinfo g_bmc_l_bladeN_t_fru_d[] = {
    { "fruinfo",    "Get blade fru information", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
      HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL, smm_get_node_fru_info },
    {NULL},
};

struct helpinfo g_bmc_l_swiN_t_fru_d[] = {
    { "fruinfo",    "Get swi fru information", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
      HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL, smm_get_node_fru_info },
    { NULL },
};

struct helpinfo g_bmc_l_fantray_t_fru_d[] = {
    { "fruinfo",     "Get fru information of fan", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
      OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL, smm_get_fan_fru },
    { NULL },
};
struct helpinfo g_bmc_l_bladeN_t_sol_d[] = {
    {"cominfo",     "Get blade com information", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
     HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL, smm_sol_get_com_info },
    {NULL},
};
struct helpinfo g_bmc_l_swiN_t_sol_d[] = {
    {"cominfo",     "Get swi com information", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, OBVIOUS_CMD,
     HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL, smm_sol_get_com_info },
    {NULL},
};

struct helpinfoplus g_bmc_l_smm_t_d_bond[] = {
    {g_bmc_sensor_list},
    {g_bmc_service_list},
    {g_bmc_user_list},
    {g_bmc_l_smm_t_fru_d},
    {g_bmc_syslog_list},
    {g_bmc_trap_list},
    {g_bmc_getcmd_lsw},
    {g_bmc_cmd_securityenhance_info},
    {NULL},
};

struct helpinfoplus g_bmc_l_shelf_t_d_bond[] = {
    {g_bmc_l_shelf_t_fru_d},
    {g_bmc_l_shelf_t_powercapping_d},
    {NULL},
};

struct helpinfoplus g_bmc_l_blade_t_d_bond[] = {
    { NULL },
};

struct helpinfoplus g_bmc_l_swi_t_d_bond[] = {
    { NULL },
};

struct helpinfoplus g_bmc_l_fantrayN_t_d_bond[] = {
    { g_bmc_l_fantray_t_fru_d },
    { NULL },
};

struct helpinfoplus g_bmc_l_pem_t_d_bond[] = {
    { NULL },
};

struct helpinfoplus g_bmc_l_bladeN_t_d_bond[] = {
    {g_bmc_l_bladeN_t_sol_d},
    { g_bmc_l_bladeN_t_fru_d },
    {NULL},
};

struct helpinfoplus g_bmc_l_swiN_t_d_bond[] = {
    {g_bmc_l_swiN_t_sol_d},
    { g_bmc_l_swiN_t_fru_d },
    {NULL},
};

struct helpinfoplus g_bmc_l_fantray_t_d_bond[] = {
    { g_bmc_l_fantray_t_fru_d },
    {NULL},
};
struct helpinfo g_bmc_l_pemN_t_fru_d[] = {
    {"fruinfo",     "Get all fru information of pem", PRIV_USER, PROPERTY_USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
     OBVIOUS_CMD, HIDE_TYPE_PROP, BMC_PRODUCT_NAME_APP, 0, PROPERTY_SHELF_CLI_SHIELD, "1", NULL, smm_get_pem_fru },
    {NULL},
};

struct helpinfoplus g_bmc_l_pemN_t_d_bond[] = {
    {g_bmc_l_pemN_t_fru_d},
    {NULL},
};

struct helpinfoplus g_bmc_l_lcd_t_d_bond[] = {
    {NULL},
};


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
    {g_bmc_l_bladeN_t_d_bond },
    {g_bmc_l_swi_t_d_bond},
    {g_bmc_l_swiN_t_d_bond },
    {g_bmc_l_fantray_t_d_bond},
    {g_bmc_l_fantrayN_t_d_bond },
    {g_bmc_l_pem_t_d_bond },
    {g_bmc_l_pemN_t_d_bond},
    {NULL},
};

struct helpinfoplus_plus *get_g_bmc_l_t_d_bond(void)
{
    return g_bmc_l_t_d_bond;
}




void print_shelf_cli_helpinfo(void)
{
    g_printf("Usage: ipmcget [-l location] [-t target] -d dataitem\r\n");
}


void ipmc_helpinfo(guchar level, gchar *target)
{
    guint32 i = 0;
    gint32 match_count = 0;
    guint64 record = 0;
    gint64 return_record = 0;

    if (!level) {
        g_printf("Usage: ipmcget [-t target] -d dataitem [-v value]\r\n");
        g_printf("    -t <target>\r\n");
        print_cmd_list(g_bmc_cmd_list2);
        g_printf("\r\n    -d <dataitem>\r\n");
        custom_print_cmd_list(g_bmc_cmd_list1);
    } else {
        if (target == NULL) {
            g_printf("\r\n    -t <target>\r\n");
            print_cmd_list(g_bmc_cmd_list2);
        } else {
            return_record = ipmc_match_str(target, g_bmc_cmd_list2);
            if (return_record != IPMC_ERROR) {
                record = return_record;
                match_count = 1;
            } else {
                record = 0;

                while (g_bmc_cmd_list2[i].name && target) {
                    if (!strncmp(target, g_bmc_cmd_list2[i].name, strlen(target))) {
                        record |= ((guint64)(1 << i));
                        match_count++;
                    }

                    i++;
                }
            }

            if (!match_count) {
                g_printf("\r\n    -t <target>\r\n\r\n");
                print_cmd_list(g_bmc_cmd_list2);
            }

            
            if (match_count == 1) {
                g_printf("\r\n    -d <dataitem>\r\n");
                i = 0;
                while (record) {
                    if (record & ((guint64)(1 << i))) {
                        print_cmd_list(g_bmc_cmd_list_d[i].help_info);
                        break;
                    }

                    i++;
                }
            }

            
            if (match_count > 1) {
                g_printf("\r\n    -t <target>\r\n\r\n");
                list_special_item(record, g_bmc_cmd_list2);
            }
        }
    }
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
            if ((!strncmp(target, g_bmc_cmd_list2[i].name, strlen(target))) &&
                (g_bmc_cmd_list2[i].flag != UNOBVIOUS_CMD)) {
                record |= (guint64)(1 << i);
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
            if (record & ((guint64)(1 << i))) {
                break;
            }

            i++;
        }
    }

    while (g_bmc_cmd_list2[i].name && target) {
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
                        record |= ((gint64)1 << j);
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
                        return g_bmc_cmd_list_d[i].help_info[j].func(g_bmc_cmd_list_d[i].help_info[j].privilege,
                            g_bmc_cmd_list_d[i].help_info[j].rolepriv, (guchar)argc, argv, target);
                    }

                    j++;
                }
            }

            if (matchnum > 1) {
                g_printf("\r\n    -d <dataitem>\r\n");
                list_special_item(record, g_bmc_cmd_list_d[i].help_info);
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
    guint64 record = 0;
    gint64 return_record;

    return_record = ipmc_match_str(cmd, g_bmc_cmd_list1);
    if (return_record != IPMC_ERROR) {
        record = return_record;
        matchnum = 1;
    } else {
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
                return g_bmc_cmd_list1[i].func(g_bmc_cmd_list1[i].privilege, g_bmc_cmd_list1[i].rolepriv, (guchar)argc,
                    argv, NULL);
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

    while (g_bmc_cmd_list2[i].name && target) {
        if ((!strncmp(target, g_bmc_cmd_list2[i].name, strlen(target))) || (return_record != IPMC_ERROR)) {
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

void printf_sel_helpinfo(void)
{
    g_printf("Usage:ipmcget %s-d sel -v <option>\r\n", help_default_name);
    g_printf("Options are:\r\n");
    g_printf(" list              list sel record.\r\n");
    g_printf(" info              get sel information.\r\n");
    g_printf(" suggestion ID     get sel suggestion based on event number.\r\n");
    return;
}

void print_export_config_usage(void)
{
    g_printf("Usage: ipmcget -t config -d export -v <localpath/URL>\r\n");
    g_printf("Localpath  e.g.: /tmp/config.xml\r\n");
    g_printf("URL            : protocol://[username:password@]IP[:port]/directory/filename\r\n");
    g_printf("    The parameters in the URL are described as follows:\r\n");
    g_printf("        The protocol must be https,sftp,cifs,scp or nfs.\r\n");
    g_printf(
        "        The URL can contain only letters, digits, and special characters. The directory or file name cannot "
        "contain @.\r\n");
    g_printf(
        "        Use double quotation marks (\") to enclose the URL that contains a space or double quotation marks "
        "(\"). Escape the double quotation marks (\") and back slash (\\) contained in the URL.\r\n");
    g_printf("        For example, if you want to enter:\r\n");
    g_printf("        a b\\cd\"\r\n");
    g_printf("        Enter:\r\n");
    g_printf("        \"a b\\\\cd\\\"\"\r\n");

    return;
}


void print_ipmcget_syslog_dest_state(void)
{
    
    g_printf("Usage: ipmcget -t syslog -d state [-v destination]\r\n");
    g_printf(
        "To obtain the syslog settings of a syslog server, you must specify the destination parameter. The value range "
        "of destination is 1 to 4.\r\n");
    g_printf("To obtain the status of syslog function, leave destination unspecified.\r\n");
    g_printf("\r\nexample :\r\n");
    g_printf("        ipmcget -t syslog -d state\r\n");
    g_printf("        ipmcget -t syslog -d state -v 4\r\n");
    
    return;
}
void print_ipmcget_trap_dest_state(void)
{
    
    g_printf("Usage: ipmcget -t trap -d state [-v destination]\r\n");
    g_printf(
        "To obtain the trap settings of a trap server, you must specify the destination parameter. The value range of "
        "destination is 1 to 4.\r\n");
    g_printf("To obtain the status of trap function, leave destination unspecified. \r\n");
    g_printf("\r\nexample :\r\n");
    g_printf("        ipmcget -t trap -d state\r\n");
    g_printf("        ipmcget -t trap -d state -v 4\r\n");
    
    return;
}

