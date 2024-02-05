
#ifndef SET_CLP_220_221_H
#define SET_CLP_220_221_H

#include "pme_app/pme_app.h"
#include "bios_ipmi.h"
#include "pme_app/xmlparser/xmlparser.h"
#include "clp_config.h"
#include "set_clp_tools.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

gint32 bios_set_json_to_clp_220(guint16 board_id, const gchar *devicename, const gchar *config_data,
    json_object **error_info);
gint32 bios_set_json_to_clp_221(guint16 board_id, const gchar *devicename, const gchar *config_data,
    json_object **error_info);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 

#endif 