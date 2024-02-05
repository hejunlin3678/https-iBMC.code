#ifndef CLI_INC_IPMCSET_SYS_MGMT_H
#define CLI_INC_IPMCSET_SYS_MGMT_H

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
#include "ipmcset_power.h"
#include "cli_sol.h"

typedef struct tag_ld_list {
    guint16 *ids;
    guint32 count;
} LD_LIST_S;

gint32 set_ascend_mode(OBJ_HANDLE object_handle, const gchar *username, const gchar *ip, guint8 ascend_mode);
void set_autodiscovery(guchar state, gchar *netsegment, guint32 option_val, guint32 port);
gint32 ipmc_get_resetiME_support(void);
gint32 send_update_command(gchar *path, guint8 forced_upgrade, guint8 reset_flag);
gint32 get_file_path_from_url(const gchar *url, guint8 file_id, gchar *file_path, gint32 len);
gint32 set_collect_configuration(const gchar *file_path);
gint32 set_physical_drive_config_confirm(gint32 is_cryptoerase);
void set_ctrl_mode(OBJ_HANDLE obj_handle, guint8 mode, const gchar* username, gchar* ip);
void set_pd_state(OBJ_HANDLE obj_handle, guint8 state, gchar *username, gchar *ip);
void set_pd_hotspare_state(OBJ_HANDLE obj_handle, guint8 state,  LD_LIST_S *ld_list, gchar *username, gchar *ip);
void set_pd_locate_state(OBJ_HANDLE obj_handle, guint8 state, guint8 duration, gchar *username, gchar *ip);
void set_pd_crypto_erase(OBJ_HANDLE obj_handle, const gchar *username, const gchar *ip);
void set_ctrl_copyback(OBJ_HANDLE obj_handle, guint8 copyback, const gchar *username, const gchar *ip);
void set_ctrl_jbod(OBJ_HANDLE obj_handle, guint8 jbod, const gchar *username, const gchar *ip);
void set_ctrl_restore(OBJ_HANDLE obj_handle, const gchar *username, const gchar *ip);
void set_ctrl_smarter_copyback(OBJ_HANDLE obj_handle, guint8 smarter_copyback, const gchar *username, const gchar *ip);
void print_set_cli_session_timeout_helpinfo(void);

guint8 get_bmc_valid_mode(void);
void set_pd_bootable(OBJ_HANDLE ld_obj, guint8 boot_param, gchar *username, gchar *ip);


#endif 