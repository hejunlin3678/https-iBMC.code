
#ifndef SMBIOS_H
#define SMBIOS_H

#include "pme_app/pme_app.h"
#include "bios_ipmi.h"
#include "pme_app/xmlparser/xmlparser.h"
#include "boot_options.h"
#include "verify.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

gint32 smbios_set_version(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 smbios_set_skunumber(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 smbios_set_family(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 bios_clear_smbios(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 smbios_set_smbios_status_default_value(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);
gint32 bios_write_smbios_data(const void *req_msg, gpointer user_data);
void init_smbios_wr_data_cfg_mutex(void);

gint32 bios_registry_version_update(guchar is_check_data_file);
gint32 bios_set_boot_stage(guint8 flag);
void clear_clp_config_file(OBJ_HANDLE obj_handle, guint8 is_force);
gint32 judge_manu_id_valid_bios(const guint8 *manufacture_id);
gint32 construt_and_respond_msg(const void *req_msg, guint8 *pdata, gint32 data_len);
gint32 respmsg_with_customermanuid(const void *req_msg, guint8 *pdata, gint32 size, gint32 data_len);
void clear_data_operate_res(BIOS_DATA_OPERATE_S *data_operate_addr);
void update_smbios_status(guint8 value);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 

#endif 