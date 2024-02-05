#ifndef CLI_INC_IPMCSET_POWER_H
#define CLI_INC_IPMCSET_POWER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>

#include "pme_app/pme_app.h"
#include "pme_app/uip/uip.h"
#include "pme_app/uip/uip_vnc.h"
#include "pme_app/uip/ipmc_public.h"
#include "pme_app/uip/ipmctypedef.h"
#include "pme_app/uip/uip_network_config.h"
#include "pme_app/smlib/sml_base.h"
#include "pme_app/smlib/sml_errcodes.h"
#include "pme/common/mscm_vos.h"
#include "pme/ipmi/common_ipmi.h"

#include "ipmcsetcommand.h"
#include "ipmcsethelp.h"
#include "ipmc_shelf_common.h"
#include "cli_sol.h"

gint32 setcommand_privilege_judge(guchar privilege, const gchar *rolepriv);
gint32 check_fan_control_support(void);
void get_fan_slot_range(guint8 *min_fan_slot, guint8 *max_fan_slot);
gint32 get_fanmode_and_fanlevel(guint8 fanid, guint8 fan_level);
gint32 get_maximum_blade_power_capping_value(OBJ_HANDLE current_handle, guint16 *remaind_value);
gint32 is_continue_operate(void);
gint32 get_minimum_power_capping_value(guint16 *demand_value, guint16 *minimum_value);
gint32 get_component_ps_max_num(guint8 *max_ps_num);
guint8 get_powercontrol_normal_and_redundancy(void);
void print_set_psu_workmode_errinfo(gint32 ret);
gint32 is_continue_powerstate_operate(gint32 operate);
gint32 is_continue_frucontrol_operate(gint32 operate);

gint32 ipmc_set_pcinfo_value_check_value(const gint32 value, const guint16 actual_min_value, const guint16 min_value,
    const guint16 max_value);

#endif 