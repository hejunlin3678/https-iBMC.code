
#ifndef SHELFCOMMAND_SET_H
#define SHELFCOMMAND_SET_H

#include "pme/pme.h"
#include "pme_app/pme_app.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FAN_MIN_SPEED 30
#define FAN_MAX_SPEED 100
#define OM_MODE_MAX_DURATION  24
#define OM_MODE_MAX_OPTION    2
#define STRING_TO_INT_IN_DECIMAL   10
#define CMESH_MODE_MAX_OPTION 1

extern gint32 ipmc_send_ipmicmd(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 smm_set_smm_cooling_medium(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *location);

extern gint32 smm_set_fan_speed(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 smm_set_fan_ctl_mode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);

extern gint32 smm_set_fan_smartmode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 smm_set_smm_failover(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 smm_set_bladepowercontrol(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 smm_set_poweroninterval(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 ipmc_set_lsw_agetest(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);

extern gint32 smm_set_shelf_management_state(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 smm_set_identify_led(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);

gint32 smm_sol_set_timeout(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
gint32 smm_sol_set_activate(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target,
    gchar *location);


extern gint32 smm_set_smalert_config(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 smm_set_smalert_status(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 shelf_set_psu_sleep_config(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 smm_set_ps_switch(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);

extern gint32 smm_set_powercapping_mode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target, gchar *location);
extern gint32 smm_set_powercapping_enable(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target, gchar *location);
extern gint32 smm_set_powercapping_value(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target, gchar *location);
extern gint32 smm_set_powercapping_threshold(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target, gchar *location);
extern gint32 smm_set_powercapping_blade_value(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target, gchar *location);
extern gint32 smm_set_powercapping_fail_action(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target, gchar *location);






gint32 smm_set_chassis_id(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
gint32 smm_set_chassis_name(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
gint32 smm_set_chassis_location(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);



gint32 smm_set_shelf_serialnumber(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target,
    gchar *location);

gint32 smm_set_removed_event_severity(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);
extern gint32 smm_set_powerstate(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
extern gint32 smm_fru_control(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);

extern gint32 smm_set_portstatus(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);

extern gint32 smm_set_othersmm_portstatus(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target);

extern gint32 smm_set_om_channel_mode(guchar privilege, const gchar* rolepriv, guchar argc, gchar** argv,
    gchar* target);

extern gint32 smm_set_cmesh_mode(guchar privilege, const gchar* rolepriv, guchar argc, gchar** argv, gchar* target);

gint32 ipmc_enable_otm_port(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
gint32 smm_set_outport_mode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
gint32 smm_set_stack_state(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
gint32 smm_set_inner_subnet_segment(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
gint32 ipmc_set_ps_switch(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
gint32 vsmm_set_smm_failover(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
gint32 smm_shelf_power_control(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
gint32 smm_set_power_button_mode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
#ifdef __cplusplus
}
#endif

#endif