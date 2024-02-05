#ifndef CLI_INC_IPMCSET_SERVICE_MGMT_H
#define CLI_INC_IPMCSET_SERVICE_MGMT_H

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

gint32 setuser_privilege_judge(guchar privilege, const gchar *rolepriv, gchar *username, guint8 *userid);
gint32 set_user_public_key(guchar user_id, guchar type, const gchar *file_path);
gint32 _ipmc_check_configure_self_priv(const gchar *current_user_name, const gchar *target_user, guchar priv,
    const gchar *role_priv_prop);
gint32 ipmc_check_user_exist(gchar *user_name, guint8 *user_id);
void print_port_cmd_helpinfo(void);
GSList *ipmc_create_inputlist(guchar argc, gchar **argv);
gint32 ipmc_match_opt(gchar *str, struct service_method *opt_list, guint8 *service_pos);
void print_state_cmd_helpinfo(void);
guint8 get_the_bmc_type(void);
gint32 ipmc_find_state_cmd(gchar *options, guint8 *service_pos);
gint32 set_service_print_warning(gchar *service);
gint32 ipmc_judge_service_state(guint8 service_type, guint8 service_state);
gint32 set_rmcp_server(const gchar *ServiceName, const gchar *username, const gchar *ip);
void _ipmc_print_trap_mode_usage(void);
gint32 get_severity_value(guchar argc, gchar **argv, guint8 *trapseverity);
gint32 ipmc_get_trapcommunity(gchar *trap_community, guint32 trap_community_size);
gint32 set_trap_state(guchar state, gchar *str_buf);
gint32 set_trap_dest_state(guchar dest, guchar state, gchar *str_buf);
struct service_method *get_g_bmc_cmd_service_state(void);
struct service_method *get_g_bmc_cmd_service_port(void);

#endif 