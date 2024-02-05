#ifndef IPMCGET__COMPONENT_MGMT_H
#define IPMCGET__COMPONENT_MGMT_H

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
void get_raid_controller_type_id(OBJ_HANDLE pd_obj, guint8 *type_id);
void print_raid_controller_info(OBJ_HANDLE raid_obj);
void find_logical_drive(OBJ_HANDLE raid_obj, gint32 ld);
void find_physical_drive(gint32 pd_id);
void find_disk_array(OBJ_HANDLE raid_obj, gint32 array_id);
#endif 