
#ifndef SET_CLP_TOOLS_H
#define SET_CLP_TOOLS_H

#include "pme_app/pme_app.h"
#include "bios_ipmi.h"
#include "pme_app/xmlparser/xmlparser.h"
#include "write_clp_json.h"
#include "clp_config.h"
#include "boot_options.h"

typedef struct tag_def_str {
    gint32 iRet;
    gint16 board_id;
} DEF_STR_S;
typedef struct tag_def_index {
    guint8 i_index;
    guint8 chip_index_id;
    guint8 port_index_id;
} DEF_INDEX_ID;

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

gint32 _check_pfs_vlan_trunkrange(const gchar *data_buf, gboolean check_flag);
gint32 _check_ports_valid(guint16 board_id, json_object *ports_json, json_object **error_info);
void _initial_config_unit(CLP_RESPONSE_UNIT *config_unit_p);
gint32 _set_header_info(guint16 board_id, json_object *port_json, CLP_RESPONSE_UNIT *config_unit_p, gint32 port_index);
gint32 _write_tmp_file_and_copy(const void *ptr, size_t size, const gchar *devicename);
void _data_string_concat(const gchar *str_val, gint32 str_val_len, gchar *data_string);
gint32 _check_current_file(const gchar *devicename);
gint32 _create_temp_file_and_set_default(void);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 

#endif 