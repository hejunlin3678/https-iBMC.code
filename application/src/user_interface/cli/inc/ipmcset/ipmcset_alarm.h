#ifndef CLI_INC_IPMCSET_ALARM_H
#define CLI_INC_IPMCSET_ALARM_H

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

gint32 get_eventcode_num(gchar *str, guint32 *retval);
void _sensor_stop_all_test(void);
gint32 get_object_by_sensor_name(const gchar *class_name, gchar *property_name, gchar *sensor_name,
    OBJ_HANDLE *obj_handle);
gint32 get_certificate_filepath(guchar argc, gchar **argv, gchar *file, guint32 len);
gint32 get_eventsource_value(guchar argc, gchar **argv, guint8 *idx, guint32 *out);
gint32 get_syslog_severity_value(guchar argc, gchar **argv, guint32 *out);
gint32 set_syslog_state(guchar state, gchar *str_buf);
gint32 set_syslog_dest_state(guchar dest, guchar state, gchar *str_buf);
void ipmc_log_operation_log(const gchar *target, const gchar *log_context);


#endif 
