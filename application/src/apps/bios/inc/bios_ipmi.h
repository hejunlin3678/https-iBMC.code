

#ifndef __BIOS_IPMI_H__
#define __BIOS_IPMI_H__

#include "pme/pme.h"
#include "pme_app/pme_app.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 


#define REPORTED 0x00
#define UNREPORTED 0x80

#define CHECK_SUM_HMM_FILE_NAME "/opt/pme/pram/hmm_checksum.txt"
#define CHECK_SUM_BIOS_FILE_NAME "/opt/pme/pram/bios_checksum.txt"


gint32 bios_clear_smbios(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 bios_dump_info(const gchar *path);
gint32 bios_get_file_changed(const void *req_msg, gpointer user_data);
gint32 bios_get_system_boot_options(const void *req_msg, gpointer user_data);
gint32 bios_init(void);
gint32 bios_start(void);
gint32 bios_set_bios_ver(const void *req_msg, gpointer user_data);
gint32 bios_set_teeos_info(const void *req_msg, gpointer user_data);
gint32 bios_set_file_changed(const void *req_msg, gpointer user_data);
gint32 upload_resource_to_bmc(const void *req_msg, gpointer user_data);
gint32 download_resource_from_bmc(const void *req_msg, gpointer user_data);

gint32 get_file_checksum_from_bmc(const void *req_msg, gpointer user_data);
gint32 set_config_file_sha256(guint8 resoure_id, char *checksum_file_path);
void set_status_by_resource_id(guint8 resource_id, guint8 status);
gint32 bios_safe_system(gchar *command);
gint32 atftp_execute_and_check_result(const gchar *command);

gint32 bios_update_bios_status(const void *req_msg, gpointer user_data);

gint32 bios_update_post_status(const void *req_msg, gpointer user_data);
gint32 bios_set_certificate_overdue_status(const void *req_msg, gpointer user_data);

gint32 bios_set_memory_mismatch(const void *req_msg, gpointer user_data);
gint32 bios_add_log_entry(const void *req_msg, gpointer user_data);
gint32 ipmi_set_system_info_parameters(const void *req_msg, gpointer user_data);
gint32 ipmi_get_system_info_parameters(const void *req_msg, gpointer user_data);
gint32 ipmi_set_bios_config_item(const void *req_msg, gpointer user_data);

gint32 method_get_boot_order(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 bios_set_boot_mode(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 bios_set_boot_mode_sw(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);

gint32 bios_set_boot_mode_sw_enable(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);



gint32 bios_notify_pcie_info(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);



gint32 Bios_Set_Clp_Config(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);


gint32 bios_set_energy_mode_enable(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 bios_set_dynamic_configuration(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 bios_get_default_dynamic_cfg(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 bios_set_demt_state(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);

gint32 bios_set_dynamic_config_result(const void *msg_data, gpointer user_data);
gint32 bios_get_dynamic_configuration(const void *msg_data, gpointer user_data);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif 
