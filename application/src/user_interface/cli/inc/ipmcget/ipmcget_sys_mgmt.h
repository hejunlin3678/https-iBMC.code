#ifndef IPMCGET__SYS_MGMT_H
#define IPMCGET__SYS_MGMT_H

#include <math.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "pme_app/smlib/sml_base.h"
#include <pme_app/uip/uip_network_config.h>
#include "uip.h"
#include "ipmctypedef.h"
#include "ipmcgetcommand.h"
#include "ipmcgethelp.h"
#include "ipmc_public.h"
#include "ipmc_shelf_common.h"
#include <pme_app/smm/smm_public.h>

#define ELABLE_PROPERTIES_NUM 19
#define EVENT_ALARM_LEVEL 4

struct fru_primary_props {
    guint8 fru_id;
    guint8 fru_type;
    gchar *fru_name;
    gsize fru_name_len;
    gchar *ref_obj_elabel;
    gsize ref_obj_elabel_len;
};

enum {
    FRU_ID_IDX = 0,
    FRU_TYPE_IDX,
    FRU_NAME_IDX,
    REF_OBJ_ELABEL_IDX,
    CHASSIS_TYPE_IDX = 0,
    CHASSIS_PART_NUM_IDX,
    CHASSIS_SERIAL_NUM_IDX,
    CHASSIS_EXTEND_LABLE_IDX,
    BOARD_MFG_DATE_IDX,
    BOARD_MANU_IDX,
    BOARD_PRODUCT_NAME_IDX,
    BOARD_SERIAL_NUM_IDX,
    BOARD_PART_NUM_IDX,
    BOARD_FRU_FILE_ID_IDX,
    EXTEND_LABEL_IDX,
    PRODUCT_MANU_IDX,
    PRODUCT_NAME_IDX,
    PRODUCT_PART_NUM_IDX,
    PRODUCT_VER_IDX,
    PRODUCT_SERIAL_NUM_IDX,
    PRODUCT_ASSET_TAG,
    PRODUCT_FRU_FILE_IDX,
    PRODUCT_EXTEND_LABEL_IDX
};

void gvar_array_to_byte(GVariant *gvar, guint8 *buf, guint32 len);
gint32 get_file_transfer_state(const gchar *class_name, const gchar *obj_name, const gchar *property_name);
gint32 ipmc_get_timeoffset(gint16 *time_offset);
gint32 get_sel_list(void);
void get_sel_info(void);
gint32 get_sel_suggestion(gint32 selid);
void print_passthrough_card_version(void);
gint32 __extract_fru_basic_props(OBJ_HANDLE obj_handle, struct fru_primary_props *fru_basic_props, gint32 *p_retval);
void __destruct_fru_basic_props(struct fru_primary_props *p);
gint32 __extract_fru_props(OBJ_HANDLE obj_handle, struct fru_primary_props *fru_basic_props, GSList **property_value);
gint32 __print_fru_props(GSList *fru_prop_value, const gchar **fru_prop_names, const gint32 props_len);
void array_object_handle_by_fruid(FRU_OBJECT_S *fru_object, guint32 count);
extern const gchar *event_alarm_level[EVENT_ALARM_LEVEL];

#endif 