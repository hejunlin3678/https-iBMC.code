#ifndef IPMCGET__POWER_H
#define IPMCGET__POWER_H

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

void get_ps_wokmode(void);
gint32 get_blade_power_capping_info(void);
gint32 get_psu_supply_source(gchar *ret_str, guint16 len);
gboolean check_print_ps_version_condition(void);
#endif 
