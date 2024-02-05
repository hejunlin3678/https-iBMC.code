

#ifndef __REDFISH_MAIN_H__
#define __REDFISH_MAIN_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 









extern gint32 redfish_init(void);
extern gint32 redfish_start(void);
extern gint32 redfish_reboot_request_process(REBOOT_CTRL ctrl);
extern gint32 redfish_dump_info(const gchar *path);
extern gint32 redfish_add_object_callback(const gchar *class_name, OBJ_HANDLE object_handle);
extern gint32 redfish_del_object_callback(const gchar *class_name, OBJ_HANDLE object_handle);
extern gint32 redfish_add_complete_callback(void);
extern gint32 redfish_del_complete_callback(void);
extern gint32 method_copy_sp_schema(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 kmc_update_redfish_ciphertext(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 update_redfish_encrypted_data(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);
extern gint32 rollback_redfish_encrypted_data(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);
extern gint32 redfish_data_sync(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif 
