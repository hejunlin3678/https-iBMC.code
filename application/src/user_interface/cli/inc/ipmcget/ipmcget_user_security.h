#ifndef IPMCGET__USER_SECURITY_H
#define IPMCGET__USER_SECURITY_H

#include <math.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "pme_app/smlib/sml_base.h"
#include "pme_app/smlib/sml.h"
#include <pme_app/uip/uip_network_config.h>
#include "uip.h"
#include "ipmctypedef.h"
#include "ipmcgetcommand.h"
#include "ipmcgethelp.h"
#include "ipmc_public.h"
#include "ipmc_shelf_common.h"
#include <pme_app/smm/smm_public.h>

#define IPMC_PRIVIVI_VALS 9
gint32 get_userinfo_level(guchar privilege, const gchar *rolepriv, gint32 *uid_level);
gint32 print_user_info_list(guint32 uid);
extern const struct valstr ipmc_privlvl_vals[IPMC_PRIVIVI_VALS];

#endif 