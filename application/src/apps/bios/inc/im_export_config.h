
#ifndef IM_EX_PORT_CONFIG_H
#define IM_EX_PORT_CONFIG_H

#include "pme_app/pme_app.h"
#include "bios_ipmi.h"
#include "pme_app/xmlparser/xmlparser.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

gint32 bios_set_config(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 bios_get_config(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);

gint32 copy_bios_file(guint8 resource_id, const gchar *file_name);
gint32 bios_read_version_from_registry(gchar *fileshortname, gchar *version, gint32 sz);
gint32 bios_read_version_from_file(gchar *fileshortname, gchar *version, guint32 ver_size);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 

#endif 