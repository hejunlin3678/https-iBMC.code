#ifndef IPMCGET__IBMC_MGMT_H
#define IPMCGET__IBMC_MGMT_H

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

gint32 print_bbu_module_info(OBJ_HANDLE obj_handle, gpointer data);
gint32 get_port_adapter_max_num_and_flg(guint8 *flag, guint32* max_port_num);
void print_otm_manufacture_info(guint8 is_local_flag, guint8 port_id);
void print_otm_hardware_status(guint8 is_local_flag, guint8 port_id);
gint32 print_rack_location_info(OBJ_HANDLE obj_rack_asset);
void process_unit_info(guint8 unit_num);
gint32 ipmc_get_timezone(gchar *time_zone, guint32 sz);
void print_time_offset(gint16 timeOffset);
#endif 