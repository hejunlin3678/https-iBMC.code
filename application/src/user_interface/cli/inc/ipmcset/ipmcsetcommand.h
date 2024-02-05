#ifndef IPMCCOMMAND_SET_H
#define IPMCCOMMAND_SET_H

#include "pme/pme.h"
#include "pme_app/pme_app.h"


#ifdef __cplusplus
extern "C" {
#endif

#define RESOURCE_COMPOSITINO_IONUM3 1
#define RESOURCE_COMPOSITINO_IONUM4 2
#define RESOURCE_COMPOSITINO_IONUM2_SDINUM1 3
#define RESOURCE_COMPOSITINO_IONUM3_SDINUM1 4

#define DEFAULT_USER_ID 2

#define SET_IN_PROGRESS 0X01
#define SET_COMPLETE 0X00
#define START_WATCHDOG_TIMER 0X00
#define STOP_WATCHDOG_TIMER 0X01


#define IPMC_LED_OFF 0X00
#define IPMC_LED_ON 0X01
#define LED_BLINKING 0X02
#define LED_LOCALCONTROL 0X04

#define FRU_RESET 0X00
#define FRU_GRACE_REBOOT 0X02
#define POWER_CYCLE 0X04

#define FAN_DEFAULT_OUTTIME (30L) // (300L)//(600L)

#define FORCE_MANUAL 1

#define TIME_LENGTH 32





#define DIRECT_PATH "update"

#define MAX_COMPONENT_ID 5

#define FINISH_UPGRADE 0X55
#define INITATE_UPGRADE 0XAA
#define GET_UPGRADE_STATUS 0
#define OPERATION_PREOCESS_0 0
#define OPERATION_PREOCESS_1 1
#define SLEEP_TIME (100 * 1000)
#define OPERATOR_LOG_DETAILMSG_SIZE 512
#define ID_LENGTH 8

#define SEND_USERNAME 0
#define SEND_USERPASSWORD 1


#define SET_PASSWORD 0X02


#define LED_ALL "ledall"

#define UPDATE_MAXFILENUM 5

#define DOWNlOAD_TYPE_SOL 0
#define DOWNLOAD_TYPE_BLACKBOX 1

#define BIOSCOMPNENTID 0x02
#define SENSOR_NAME_MAX_LENGTH 32

#define SMM_BMC 0
#define BLADE_BMC_CLI 1


#define SNMPV3_PASSWORD_MIN_LENGTH 8


#define TEMP_NAME_LENGTH 128

#define PCIE_SWITCH_8725_TYPE_CODE 1

// 下面的宏移至common_macro.h




#define FILE_ID_SSL_CER 0x0E

#define FILE_PATH_SSL_CER_DEFAULT "/dev/shm/custom_cert.pfx" 


#define ETH_ENABLE "enable"
#define ETH_DISABLE "disable"
#define ETH_MAX_NUM 10
#define ETH_NAME_MIN_LEN 3
#define ETHNUM_STR_MAX_LENGTH 256


#define SHELF_PWRCAP_MODE_EQUAL 0
#define SHELF_PWRCAP_MODE_MODE_MANUAL 1
#define SHELF_PWRCAP_MODE_MODE_PROPORTION 2

#define SHELF_PWRCAP_STATUS_DISABLED 0x00
#define SHELF_PWRCAP_STATUS_ENABLED 0x01

#define SHELF_MSPP_STATUS_DISABLED 0x00
#define SHELF_MSPP_STATUS_ENABLED 0x01

#define IPMCSET_CMD_ARGC 2
#define IPMCSET_FIRST_ARGV 1

#define SENSOR_ENABLE   1
#define SENSOR_DISABLE  0
#define FIRST_PARA     1
#define SECOND_PARA    2
#define THIRD_PARA     3

struct service_method {
    const gchar *ServiceName; 
    const gchar *desc;
    const gchar *ObjName;
    const gchar *ClassName; 
    const gchar *Method;    

    guchar flag;
    guint8 HideType; 
    
    const gchar *DepClassName;    
    guint32 DepObjectNum;         
    const gchar *DepPropertyName; 
    const gchar *DepPropetyValue; 
    guint8 ServiceIndex;
};

typedef void (*active_port_switch_help_info_func)(gchar *help_info_buf, guint32 buf_len);

typedef struct active_port_mgnt {
    guint8 display_index;                                      
    const gchar *proper_name;                                  
    const gchar *display_info;                                 
    active_port_switch_help_info_func display_extra_info_func; 
} ACTIVE_PORT_MGNT_S;

#define FILE_ID_WEAK_PWD_DICT 0x1d
#define WEAK_PWDDICT_FILE_TMP_PATH "/tmp/weakdictionary"
#define BIOS_FILE_TAR_TMP_PATH "/tmp/bios.tar.gz"
#define PUBLICK_KEY_FILE_ID 13
#define SET_USB_MGMT_ENABLE_STATE_CMD_HELP_INFO "Usage: ipmcset -t usbmgmt -d state -v <enabled/disabled>\r\n"
#define SET_USB_MGMT_ENABLE_STATE_CMD_FAIL_INFO "Set USB management service state failed.\r\n"
#define SET_USB_MGMT_ENABLE_STATE_CMD_SUCC_INFO "Set USB management service state successfully.\r\n"

#define SET_USB_MGMT_ENABLE_STATE_CMD_HELP_INFO "Usage: ipmcset -t usbmgmt -d state -v <enabled/disabled>\r\n"
#define SET_USB_MGMT_ENABLE_STATE_CMD_FAIL_INFO "Set USB management service state failed.\r\n"
#define SET_USB_MGMT_ENABLE_STATE_CMD_SUCC_INFO "Set USB management service state successfully.\r\n"

extern gint32 ipmc_privilege_judge(guchar privilege, const gchar *rolepriv);
extern gint32 ipmc_set_fanmode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_fanlevel(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_identify_led(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 set_boot_option(guchar para);
extern gint32 ipmc_dubug_upgrade(guchar privilege, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_upgrade_component(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_clear_cmos(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_bootdevice(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_control_watchdog(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_stop_watchdog(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_gracefulshutdowntimeout(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_control_fru(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_powerstate(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_clear_sel(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_ledstate(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_ipaddr(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_backup_ipaddr(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_gateway(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_ipaddr_v6(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_gateway_v6(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_netmode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_vlan_state(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_restore_factorysetting(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_upgrade_pcieswitch_configuration(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_set_notimeout_state(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 set_in_progress(guchar privilege, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_add_user(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_change_user_password(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_delete_user(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_userprivilege(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_ascend_mode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_machinename(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_serialdir(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
gint32 ipmc_set_serialnode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_printscreen(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_trapdestaddress(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_set_trapcommunity(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_trapport(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_trapenable(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_trap_version(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_trap_severity(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_trapenablemask(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_rollback(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_time_zone(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_service_state(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_service_port(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_pass_complexity_check_enable(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_set_user_lock(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_user_unlock(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);

extern gint32 ipmc_add_public_key(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_delete_public_key(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_ssh_password_authentication(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);


extern gint32 ipmc_set_user_state(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);

gint32 ipmc_set_user_first_login_policy(guchar privilege, const gchar *rolepriv,
    guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_activeport(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_ipmode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_ipmode_v6(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_trap_user(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);

extern gint32 ipmc_set_trap_mode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);

extern gint32 ipmc_set_emergency_user(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_set_on_board_rtc_time(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_set_autodiscovery_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_set_poweronpermit_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_set_maintenance_upgrade_bios(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
gint32 ipmc_set_maintenance_upgrade_cpld(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_set_sensor_test(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_sensor_state(guchar privilege, const gchar* rolepriv, guchar argc, gchar** argv, gchar* target);
extern gint32 ipmc_set_maintenance_download(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_set_maintenance_biosprint(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_set_maintenance_coolingpowermode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_set_powercappinginfo_mode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_set_powercappinginfo_enable(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_set_powercappinginfo_value(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_set_powercappinginfo_threshold(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_set_powercappinginfo_bladevalue(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_set_powercappinginfo_failaction(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);

extern gint32 ipmc_set_MSPP_enable(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);

extern gint32 ipmc_set_security_banner_state(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_set_security_banner_content(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_set_syslogenable(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_syslog_auth(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_powercappinginfo_failaction(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_set_pwd_minimum_age(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_set_syslog_auth(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_syslog_protocol(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_set_syslog_identity(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_set_syslog_severity(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_set_syslog_clientcertificate(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_set_syslog_rootcertificate(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_set_syslogdestaddress(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_set_syslogport(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_syslog_eventsource(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_set_syslogtest(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_ethlink(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);


extern gint32 ipmc_set_user_login_interface(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);



extern gint32 ipmc_create_logical_drive(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_add_logical_drive(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_delete_logical_drive(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_set_logical_drive_config(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_set_raid_controller_config(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_set_physical_drive_config(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);


extern gint32 ipmc_set_ntp_enable_status(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_set_ntp_mode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_ntp_preferred_server(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_set_ntp_alternative_server(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_set_ntp_extra_server(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
gint32 ipmc_set_ntp_auth_enable_status(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_set_ntp_group_key(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);

extern gint32 ipmc_set_vnc_pwd(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_vnc_timeout(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_vnc_ssl_state(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);

extern gint32 ipmc_set_keyboard_layout(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_set_vmm_connect(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_vmm_disconnect(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);

extern gint32 ipmc_set_config_import(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);

extern gint32 ipmc_set_certificate_import(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);


extern gint32 ipmc_set_usermgnt_switch(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);


extern gint32 ipmc_set_maintenance_resetiME(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);

extern gint32 ipmc_set_maintenance_raidcom(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_set_maintenance_ps_on_lock_clear(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);

extern gint32 ipmc_set_weak_pwddic(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);


extern gint32 ipmc_upload_crl(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);


extern gint32 ipmc_set_psu_workmode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);


gint32 ipmc_set_psu_supply_source(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);


extern gint32 ipmc_set_snmp_privacy_password(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);


extern gint32 ipmc_set_resource_composition(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);


extern gint32 ipmc_set_cli_sol_activate(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);

extern gint32 ipmc_set_cli_sol_deactivate(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);

extern gint32 ipmc_set_cli_sol_timeout(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);

extern gint32 smm_set_blade_ipaddr(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location);

extern gint32 smm_set_blade_ipmode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location);

extern gint32 smm_set_blade_ip6addr(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *location);

extern gint32 smm_set_blade_ip6mode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *location);

extern gint32 smm_set_blade_gateway(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *location);

extern gint32 smm_set_blade_gateway6(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *location);

extern gint32 smm_set_blade_net_mode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *location);

extern gint32 smm_set_blade_active_port(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *location);

extern gint32 smm_set_blade_vlan_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *location);




extern gint32 ipmc_set_inactive_time_limit(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);

gint32 ipmc_set_masterkey_update_interval(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
gint32 ipmc_set_update_masterkey(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);


extern gint32 ipmcset_clear_log(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);


extern gint32 ipmc_set_precisealarm_mock(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);


extern gint32 ipmc_set_fpga_golden_fw_restore(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);

gint32 ipmc_set_lldp_enable_status(guchar privilege, const gchar* rolepriv, guchar argc, gchar** argv, gchar* target);
extern gint32 ipmc_set_pwroff_policy_after_leakage(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);

extern gint32 ipmc_set_pwroff_policy_after_leakage(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);

gint32 ipmc_set_maintenance_ipaddr(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
gint32 ipmc_set_usbmgmt_enable_state(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);

extern gint32 ipmc_set_float_ipaddr(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_set_float_ipaddr_v6(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_del_float_ipaddr(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_del_float_ipaddr_v6(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);

extern gint32 smm_set_blade_user_password(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *location);
extern gint32 smm_set_blade_user_privilege(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *location);
extern gint32 smm_set_blade_adduser(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *location);
extern gint32 smm_set_blade_deluser(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *location);
extern gint32 smm_set_standby_smm_ipaddr(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 smm_set_standby_smm_ipaddr_v6(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);



extern gint32 ipmc_set_inactive_time_limit(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);



extern gint32 ipmcset_clear_log(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);



extern gint32 ipmc_reset_othersmm(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);


extern gint32 smm_set_offlineconfig(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *location);

gint32 ipmc_set_mesh_mpath_mode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
gint32 ipmc_set_lsw_port_enable(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
gint32 ipmc_lsw_set_mesh_itfportmode(guchar priv, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
#ifdef __cplusplus
}
#endif

#endif
