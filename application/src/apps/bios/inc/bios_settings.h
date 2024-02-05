
#ifndef BIOS_SETTINGS_H
#define BIOS_SETTINGS_H

#include "pme_app/pme_app.h"
#include "bios_ipmi.h"
#include "pme_app/xmlparser/xmlparser.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

gint32 bios_set_json_file(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 bios_get_json_file(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 bios_set_print_flag(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 bios_reset(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 clear_bios_setting_file(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 bios_get_setting_effective_status(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

gint32 bios_regist_check_boottypeorder(json_object *object);
gint32 bios_regist_check_object(json_object *attributes, gchar *name, json_object *value, gboolean export_flag);
gint32 bios_regist_get_value(json_object *attributes, const gchar *name, const gchar *poperty,
    json_object **name_value_jso, json_object **item_jso);
gint32 bios_regist_compare_object(json_object *obj_jso_a, json_object *obj_jso_b, const gchar *flag);

gint32 bios_get_server_type(guint8 *server_type);
gint32 set_flash_id(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 

#endif 