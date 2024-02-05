#ifndef IPMCSETHELP_H
#define IPMCSETHELP_H

#include "pme/pme.h"
#include "pme_app/pme_app.h"


#define MAX_SDR_NUMBER 50
#define IPMC_CMD_LIST2 2
#define IPMC_CMD_LIST3 3


#define BACKSPACE 8
#define ENTER 13
#define ALARM 7
#define UPGRADE_FILE_PATH_LEN 128
#define HISTORY_CMD_LEN 128

#define HIDE_TYPE_PROP_REVERSE 4
#define SET_BIOS_DEGUG_INFO_ENABLE 1
#define SET_BIOS_DEGUG_INFO_BY_SETUP_MENU 2
#define SET_BIOS_DEGUG_INFO_ENABLE_ON 1
#define SET_BIOS_DEGUG_INFO_BY_SETUP_MENU_ON 0

#define MAINTENANCE "Maintenance"

#define ADDUSER "adduser"
#define DELUSER "deluser"

extern void ipmc_helpinfo(guchar level, gchar *target);
extern void print_cmd_list(struct helpinfo *cmd_list);
extern void custom_print_cmd_list(struct helpinfo* cmd_list);
extern gint32 ipmc_find_matchtarget(const gchar *target);
extern gint32 ipmc_find_cmd(gchar *target, gchar *cmd, gint32 argc, gchar **argv);
extern gint32 ipmc_find_matchcmd(gchar *cmd, gint32 argc, gchar **argv);
extern void ipmc_list_data(gchar *target);
extern gint64 ipmc_match_str(const gchar *str, struct helpinfo *cmd_list);
extern void ipmc_data_info(guchar l_level, guchar level, gchar *location, gchar *target, gchar *cmd, gint32 argc,
    gchar **argv);
extern void print_vnctimeout_usage(void);
extern void print_fanmode_usage(void);
extern void print_fanlevel_usage(guint8 min_fan_slot, guint8 max_fan_slot);
extern void print_ipmode_usage(void);
extern void print_biosprotect_usage(void);
extern void print_watchdog_usage(void);
extern void print_fru_control(void);
extern void print_power_state_usage(void);
extern void print_shelf_power_control_usage(void);
extern void print_power_button_mode_usage(void);
extern void print_set_ledstate_usage(void);
extern void print_userprivilege_usage(void);
extern void print_workmode_usage(void);
extern void print_boot_device_usage(void);
extern void print_trapseverity_usage(void);
extern void print_upgrade_usage(void);
extern void print_smm_upgrade_usage(void);
extern void print_smm_fantray_upgrade_usage(void);
extern void print_netmode_usage(gboolean is_mm);
extern gint32 get_username_ip(gchar *username, gchar *ip, guint32 ip_size);
extern const gchar *get_bootdevice_name(guchar device);
extern const gchar *get_user_privilege_str(gint32 priv);
extern void print_trapcommunity_plexity_usage(void);
extern void print_SetPasswordComplexityCheckState_helpinfo(void);
extern void print_set_snmp_privacy_help(void);
extern void print_trap_community_usage(void);
extern void print_adduser_helpinfo(void);
extern void print_sensor_test_help(void);
extern void print_set_maintenance_download_helpinfo(void);
extern void print_set_maintenance_biosprint_helpinfo(void);
extern void print_set_maintenance_coolingpowermode_helpinfo(void);

extern void print_set_security_banner_state_help_info(void);
extern void print_set_security_banner_content_help_info(void);


extern void print_eventsource_usage(void);
extern void print_syslogseverity_usage(void);
extern void print_syslogidentity_usage(void);
extern void print_syslogauth_usage(void);
extern void print_syslogprotocol_usage(void);
extern void print_syslog_rootcertificate_usage(void);
extern void print_syslog_clientcertificate_usage(void);

extern void print_syslog_dest_address(void);
extern void print_syslog_dest_state(void);
extern void print_syslog_dest_port(void);
extern void print_syslog_dest_test(void);
extern void print_trap_dest_address(void);
extern void print_trap_dest_state(void);
extern void print_trap_dest_port(void);

extern void print_set_pwd_minimum_age_usage(guint32);

extern void print_user_login_interface_usage(void);


extern void print_add_pulickey_usage(void);


extern void print_import_config_usage(void);

extern void print_set_vmm_connect(void);
extern void print_set_vmm_disconnect(void);

extern void print_ipmort_certificate_usage(void);


extern void print_create_ld_help_info(guint8 type_id);
extern void print_add_ld_help_info(guint8 type_id);
extern void print_modify_ld_help_info(guint8 type_id);
extern void print_set_ctrl_config_usage(void);
extern void print_set_pd_config_usage(void);



extern void print_switch_usermgnt_helpinfo(void);



extern void print_upload_crl_usage(void);


extern void print_weak_pwddic_usage(void);


extern void print_psu_work_mode(guint8 max_ps_num);


extern void print_set_timezone_helpinfo(void);
extern void print_timezone_area_helpinfo(const TZ_CITY *tz_list, guint32 list_len);
extern void print_timezone_city_helpinfo(const TZ_CITY *tz_city);


extern void print_cli_sol_activate_usage(void);
extern void print_sol_timeout_usage(void);



extern void print_set_inactive_user_timeout_helpinfo(void);

void print_set_update_interval_helpinfo(void);
void print_update_master_key_warnninginfo(void);
void print_update_workkey_warnninginfo(guint32 product_version);


extern void print_clear_log_usage(void);



extern void print_precisealarm_mock_help(void);



extern void print_maintenance_raidcom_usage(void);



extern void print_set_fpga_golden_fw_restore_usage(gchar *product_type_name);


void print_set_psu_supply_source_usage(void);

void print_sensor_state_help(void);
extern void print_poweroninterval_usage(void);
extern void print_bladepowercontrol_usage(void);
extern void print_blade_cooling_medium_usage(void);
extern void print_set_stack_state_usage(void);
extern void print_set_outport_mode_usage(void);
extern void print_rtc_time_usage(void);
extern void print_mesh_mpath_mode_usage(void);
extern void print_leak_policy_usage(void);
extern void print_leak_policy_usage(void);
struct helpinfo *get_g_bmc_cmd_list1(void);
struct helpinfo *get_g_bmc_cmd_list2(void);
struct helpinfoplus *get_g_bmc_cmd_list_d(void);
struct helpinfo *get_g_bmc_cmd_list_l(void);
struct helpinfoplus *get_g_bmc_cmd_list_l_t_bond(void);
struct helpinfoplus *get_g_bmc_cmd_list_l_d_bond(void);
struct helpinfoplus_plus *get_g_bmc_l_t_d_bond(void);
struct helpinfo *get_g_bmc_l_smm_t(void);
struct helpinfo *get_g_bmc_l_smm_d(void);

#endif
