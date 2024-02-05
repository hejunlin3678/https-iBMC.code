#ifndef CLI_INC_IPMCSET_USER_SECURITY_H
#define CLI_INC_IPMCSET_USER_SECURITY_H

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

gint32 ipmc_get_empty_user_id(guint8 *user_id);
gint32 add_user_name(guchar user_id, const gchar *user_name);
gint32 set_user_password(guchar user_id, const gchar *user_passwd);
gint32 set_user_name(guchar user_id, const gchar *user_name);
gint32 check_is_only_enabled_admin(guint8 user_id);
gint32 get_user_login_interface_value(guint32 state, guchar argc, gchar **argv, guint32 *out);
gint32 add_ipmi_snmp_reset_passwd(guint32 old_mask, guint32 mask, gchar *user_name);
gint32 log_ipmcset_user_privilege(guchar argc, gchar **argv, gchar *user_name, gchar *log_info);
gint32 set_user_privilege(guchar user_id, guchar user_privilege, guchar user_roleid);
gint32 set_user_roleid(guchar user_id, guchar user_roleid);
gint32 set_weak_pwddic_state(guint8 state);
gint32 export_weak_pwddic_config(gchar *argv_path);
gint32 import_weak_pwddic_config(gchar *argv_path);
gint32 get_user_info(void);
extern gchar *getPasswd(const gchar *prompt);

gint32 blade_set_user_password(gchar *location, const gchar *user_name, const gchar *password);
gint32 blade_set_user(gchar *location, const gchar *user_name, gchar *type);

extern user_privilege_info g_user_pri_info[IPMC_MAX_USER_NUMBER];

#endif 