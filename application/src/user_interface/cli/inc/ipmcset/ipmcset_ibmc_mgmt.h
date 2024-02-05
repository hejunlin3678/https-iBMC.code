#ifndef CLI_INC_IPMCSET_IBMC_MGMT_H
#define CLI_INC_IPMCSET_IBMC_MGMT_H

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

gint32 set_fpga_golden_fw_restore(guint8 slot_id, guint32 position, gint32 *todo_flg);
gint32 get_fpga_golden_fw_restore_result(guint8 slot_id);
gint32 maintenance_upgrade_cpld(gchar *path_info, guint32 len);
gint32 check_is_digint(gchar *ptr, guint8 len);
gint32 check_time_data(guint16 Year, guint8 Month, guint8 Date, guint8 Hour, guint8 Minute, guint Second);
gint32 year_is_leap(guint16 Year);
gint32 set_on_board_rtc_time(guint32 timestamp);
gint32 ipmc_weak_match_timezone(const gchar *tz);
gint32 ipmc_set_timezone(const gchar *time_zone);
void constr_area_city_to_timezone(const gchar *area, const gchar *city, gchar *tz, guint32 tz_sz);
guchar check_usb_mgmt_support_state(const gchar *print_str);
gint32 get_update_filepath(guchar argc, gchar **argv, gchar *update_file, guint32 len, guint8 *reset_flag);
guint8 get_upgrade_bmc_reset_flag(guchar argc, gchar **argv);


#endif 