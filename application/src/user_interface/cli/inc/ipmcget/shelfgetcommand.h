
#ifndef SHELFCOMMAND_GET_H
#define SHELFCOMMAND_GET_H


#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "pme_app/smlib/sml_common.h"


#ifdef __cplusplus
extern "C" {
#endif



#define CHASSIS_LOCATION_LEN 20
#define CHASSIS_NAME_LEN 20
#define VERSION_LEN 64

extern gint32 ipmc_get_hotswapstate(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *location);
extern gint32 ipmc_get_blade_presence(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *location);
extern gint32 ipmc_get_listpresent(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location);
extern gint32 smm_get_blade_cooling_medium(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *location);
extern gint32 smm_get_fan_speed(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location);
extern gint32 smm_get_fan_log(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 smm_get_fan_presence(guchar priv, const gchar *role, guchar argc, gchar **argv, gchar *location);
extern gint32 smm_get_shelf_fan_speed(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *location);
extern gint32 smm_get_fan_ctl_mode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location);
extern gint32 smm_get_fan_version(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location);
extern gint32 smm_get_fan_fru(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target,
    gchar *location);
extern gint32 smm_get_smm_cooling_medium(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *location);


extern gint32 ipmc_get_pem_presence(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *location);
extern gint32 ipmc_get_pem_health(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location);
extern gint32 ipmc_get_pem_rate_power(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *location);
extern gint32 ipmc_get_pem_real_power(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *location);
extern gint32 ipmc_get_pem_type(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location);
extern gint32 ipmc_get_pem_fault_reason(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *location);
extern gint32 ipmc_get_pem_version(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location);
extern gint32 smm_get_pem_fru(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target,
    gchar *location);
extern gint32 smm_get_power_capping_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target, gchar *location);


gint32 smm_sol_get_timeout(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
gint32 smm_sol_get_connection_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
gint32 smm_sol_get_com_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target,
    gchar *location);

gint32 smm_get_as_status(guchar priv, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
gint32 smm_get_data_sync_status(guchar priv, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
gint32 smm_get_slot_number(guchar priv, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 smm_get_poweroninterval(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *location);
extern gint32 smm_get_bladepowercontrol(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *location);


extern gint32 ipmc_get_smalert_config(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_get_smalert_status(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);


extern gint32 ipmc_get_psu_sleep_config(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *location);
extern gint32 ipmc_get_psu_sleep_status(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *location);


gint32 smm_get_shelf_fru_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target,
    gchar *location);
gint32 smm_get_chassis_id(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
gint32 smm_get_chassis_name(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
gint32 smm_get_chassis_location(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
gint32 smm_get_chassis_version(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);



gint32 smm_get_health_event(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
gint32 smm_get_health_status(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
gint32 smm_get_pem_health_event(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
gint32 smm_get_pem_health_status(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
gint32 smm_get_fantray_health_event(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
gint32 smm_get_fantray_health_status(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
gint32 smm_get_shelf_health_event(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
gint32 smm_get_shelf_health_status(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
gint32 smm_get_shelf_unhealthy_location(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
gint32 smm_get_blade_health_event(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
gint32 smm_get_blade_health_status(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);


extern gint32 smm_get_shelf_realtime_power(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);


extern gint32 smm_get_blade_user_list(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *location);
extern gint32 smm_get_shelf_management_state(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 smm_get_other_smm_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 smm_get_blade_real_time_power(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *location);
extern gint32 smm_get_mm_port_optical_module_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *location);
extern gint32 smm_get_node_fru_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target,
    gchar *location);
extern gint32 smm_get_inner_subnet(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 ipmc_get_smm_diagnose_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);

gint32 smm_get_stack_state(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location);
gint32 smm_get_outportmode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location);


extern gint32 smm_get_power_capping_value_range(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target, gchar *location);
extern gint32 smm_get_blade_power_capping_value_range(guchar privilege, const gchar *rolepriv, guchar argc,
    gchar **argv, gchar *location);



extern gint32 ipmc_get_lsw_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);

extern gint32 ipmc_get_lsw_agetest_status(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *location);



extern gint32 smm_get_blade_version(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *location);

extern gint32 ipmc_get_deviceid(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location);
gchar ipmi_get_deviceid(gchar *location);
void smm_cli_print_deviceid(const guchar *resp_src_data);
void processDeviceID(guchar cmp, guchar *valueSpt);
extern gint32 ipmc_get_removed_event_severity(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *location);

gint32 print_single_smm_health_event(GSList *event_msg, gpointer data);
gint32 print_single_blade_health_event(const ALARM_EVENT_MSG *msg, const guint8 *msg_data, gpointer data);
gint32 print_blade_info(guint8 slave_addr);
gint32 get_pem_presence(gint32 slot);
gint32 vsmm_get_ms_status(guchar priv, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
gint32 ipmc_get_om_channel_mode(guchar privilege, const gchar* rolepriv, guchar argc, gchar** argv, gchar* target);
gint32 ipmc_get_cmesh_mode(guchar privilege, const gchar* rolepriv, guchar argc, gchar** argv, gchar* target);

#ifdef __cplusplus
}
#endif
#endif