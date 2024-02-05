#ifndef IPMCCOMMAND_GET_H
#define IPMCCOMMAND_GET_H


#include "pme/pme.h"
#include "pme_app/pme_app.h"


#ifdef __cplusplus
extern "C" {
#endif


#define COMP_MEDIA_NOT_PRESENT 0x80




#define TRAPITEM_STATE_LEN 10


#define LED_MODE_LCS 0      // 本地控制
#define LED_MODE_OS 1       // 非本地控制
#define LED_MODE_LAMPTEST 2 // 测试状态

#define POSITIVE_SENSOR_HYSTERESIS 0X01
#define NEGATIVE_SENSOR_HYSTERESIS 0X02
#define SEL_HISTORY_FILE "sensor_alarm_sel.bin"

#define RESOURCE_COMPONENTS_DESCRIPTION_LEN 2048
#define RESOURCE_COMPONENTS_INFO_LEN 256
#define RESOURCE_COMPONENTS_ID_INFO 32
#define GPU_BOARD_PCB_VERSION 3
#define GPU_CPLD_LOGIC_VERSION 16

#define NO_SEL_FILE 0x309114CA

#define SUBJECT_UNKNOWN ("Unknown Sensor")
#define MAX_VERSION_LEN 10

#define IS_CONTAIN_SYSTEM_AREA 1
#define NO_CONTAIN_SYSTEM_AREA 0
#define MAIN_BOARD_FRU_ID 0
#define PRAMETER_INVALID (-1)
#define HOUR_TO_MINUTE 60
#define OM_CHANNEL_MODE_NUM 3


#define SWTICH_CHIP_MAX_PORT 27
#define LSW_CHIP_LOGIC_PORT_MAX 31
#define L2_TBL_SRAM_SIZE 8192
#define PORT_SPEED_MAP_SIZE 3
#define MAC_ADDR_LEN 6
#define TABLE_OUTPUT_MAX_ROWS 40



#define PCIE_CARD_NAME_LEN 64
#define PCIE_CARD_PCB_VERSION 3
#define NO_NEED_PRINTF_VER 0
#define NEED_PRINTF_VER 1
#define FIRMWARE_LOCATION_INFO_LEN 32


#define CLASSE_NAME_LEN 128

#define CURRENT_IS_LOCAL 1
#define CURRENT_IS_REMOTE 0


#define PCIE_PORT_AVAILABLE_FLAG 0x3

#define PCB_VERSION_LEN 10


struct get_service {
    const gchar *ServiceName; 

    guchar flag;
    guint8 HideType; 
    
    const gchar *DepClassName;    
    guint32 DepObjectNum;         
    const gchar *DepPropertyName; 
    const gchar *DepPropetyValue; 
    gint32 (*func)(void);
};


struct card_type_info {
    const gchar *typeDesc; 
    const gchar *title;    
    guint8 type;  
    gboolean  flag;
};

extern gint32 ipmc_get_faninfo(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_get_fan_smartmode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_get_ipmode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_get_ipinfo(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_get_eth1_ipinfo(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_get_userinfo(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);

extern gint32 ipmc_get_ssh_password_authentication(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);

extern gint32 ipmc_get_service_list(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_get_gracefulshutdowntimeout(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_get_health(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_get_healthevents(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_get_sel(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);

gint32 ipmc_print_log(const gchar *log_path);

extern gint32 ipmc_get_operate_log(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_get_biosbootmode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_get_version(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_get_serialnumber(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_get_systemname(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_get_fruinfo(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_get_powerstate(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_get_allsensorinfo(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_get_port80_status(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_get_diagnose_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_get_system_time(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_get_macaddr(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_get_trapenable(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_get_trapiteminfo(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_get_trap_severity(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_get_trap_version(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_get_trapenablemask(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_get_rollbackstatus(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_get_serialdir(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
gint32 ipmc_get_serialnode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_get_locationID(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_get_pass_complexity_check_enable(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_get_ledinfo(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_get_ethport(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_get_chassis_number(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_get_systemcom_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_get_blackbox_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_get_trap_user(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_get_psu_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_get_autodiscovery_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_get_poweronpermit_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_get_powercapping_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_get_maintenance_biosprint(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_get_maintenance_coolingpowermode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_get_maintenance_raidcom(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_get_maintenance_ps_on_lock_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
gint32 ipmc_get_maintenance_ipinfo(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);

extern gint32 ipmc_get_trap_mode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);



extern gint32 ipmc_get_raid_controller_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_get_logical_drive_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_get_physical_drive_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);



extern gint32 ipmc_get_disk_array_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);



extern gint32 ipmc_get_security_banner_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);


extern gint32 ipmc_get_syslogenable(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_get_syslog_auth(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_get_syslog_protocol(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_get_syslog_identity(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_get_syslog_severity(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_get_syslog_root_CA_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_get_syslog_client_CA_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_get_syslogiteminfo(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_get_minimum_pwd_age(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);


extern gint32 ipmc_get_ntp_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);

gint32 ipmc_get_bbu_module_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_get_ascend_work_mode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);

extern gint32 ipmc_get_vnc_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);

extern gint32 ipmc_get_config_export(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);

extern gint32 ipmc_get_vmm_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);

extern gint32 ipmc_get_certificate_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);

extern gint32 ipmc_get_usermgnt(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);

extern gint32 ipmc_get_resource_components_list(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);

extern gint32 ipmc_get_resource_compositions_list(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);


extern gint32 ipmc_get_sol_session(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);

extern gint32 ipmc_get_sol_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);

extern gint32 smm_get_blade_list_frus(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *location);


extern gint32 ipmc_get_inactive_timelimit(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);

gint32 ipmc_get_update_interval(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);


extern gint32 ipmc_get_otm_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_get_otm_status(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);


extern gint32 smm_get_blade_ip_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *location);

gint32 ipmc_get_psu_supply_source(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
gint32 ipmc_get_usbmgmt_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);


extern gint32 ipmc_get_float_ipinfo(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 smm_get_other_smm_ipinfo(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);

extern gint32 ipmc_get_rack_asset_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_get_unit_asset_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);


extern gint32 print_fru_info(OBJ_HANDLE obj_handle);


extern gint32 ipmc_lsw_get_ctrl_portstatistics(guchar privilege, const gchar* rolepriv, guchar argc,
                                               gchar** argv, gchar* target);
extern gint32 ipmc_lsw_get_ctrl_l2table(guchar privilege, const gchar* rolepriv, guchar argc,
                                        gchar** argv, gchar* target);
extern gint32 ipmc_lsw_get_ctrl_portinfo(guchar privilege, const gchar* rolepriv, guchar argc,
                                         gchar** argv, gchar* target);
extern gint32 ipmc_lsw_get_ctrl_chipinfo(guchar privilege, const gchar* rolepriv, guchar argc,
                                         gchar** argv, gchar* target);
extern gint32 ipmc_lsw_get_data_l2table(guchar privilege, const gchar* rolepriv, guchar argc,
                                        gchar** argv, gchar* target);
extern gint32 ipmc_lsw_get_data_portinfo(guchar privilege, const gchar* rolepriv, guchar argc,
                                         gchar** argv, gchar* target);
extern gint32 ipmc_lsw_get_data_portstatistics(guchar privilege, const gchar* rolepriv, guchar argc,
                                               gchar** argv, gchar* target);
extern gint32 ipmc_lsw_get_mesh_multiplepath(guchar privilege, const gchar* rolepriv, guchar argc,
                                             gchar** argv, gchar* target);
extern gint32 ipmc_lsw_get_mesh_itfportmode(guchar privilege, const gchar* rolepriv, guchar argc,
                                            gchar** argv, gchar* target);
extern gint32 ipmc_get_pwroff_policy_after_leakage(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);

guchar ipmc_get_local_data_powerstate(void);

gint32 ipmc_preconditions_no_argc_judge(guchar privilege, const gchar* rolepriv, guchar argc);
gint32 ipmc_get_notimeoutstate(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
gint32 ipmc_get_lldp_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
gint32 smm_get_shelf_powerstate(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
gint32 smm_get_power_button_mode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
#ifdef __cplusplus
}
#endif
#endif
