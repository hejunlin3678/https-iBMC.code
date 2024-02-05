#ifndef IPMCGET__NETCONFIG_H
#define IPMCGET__NETCONFIG_H

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
#define NET_MODE_STR_NUM 3
#define NET_TYPE_STR_NUM 12
#define IP_MODE_STR_NUM 5
gint32 print_mac_info(OBJ_HANDLE obj_handle);
gint32 check_port_virtual_netcard(OBJ_HANDLE obj_handle);
gint32 is_support_ncsi(guint8 net_type);
gint32 print_ip_info(OBJ_HANDLE obj_handle, gpointer user_data);
gint32 get_ncsi_network_port(void);
gint32 get_dedicated_network_port(guint8 input_groupid);
extern const gchar *net_mode_str[NET_MODE_STR_NUM];
extern const gchar *net_type_str[NET_TYPE_STR_NUM];
extern const gchar *ip_mode_str[IP_MODE_STR_NUM];

#endif 