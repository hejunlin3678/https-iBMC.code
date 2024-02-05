#ifndef IPMCGET_ALARM_H
#define IPMCGET_ALARM_H

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

gint32 print_analog_sensor_info(OBJ_HANDLE obj_handle, gpointer data);
gint32 print_discrete_sensor_info(OBJ_HANDLE obj_handle, gpointer data);
gint32 print_trapitem_info(OBJ_HANDLE obj_handle, gpointer user_data);
gint32 get_syslog_state(void);
gint32 get_syslog_dest_state(gint32 dest);
gint32 print_syslogitem_info(OBJ_HANDLE obj_handle, gpointer user_data);
gint32 get_trap_state(void);
gint32 get_trap_dest_state(gint32 dest);
#endif 