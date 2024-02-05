#ifndef _UI_MANAGE_PROXY_METHOD_H_
#define _UI_MANAGE_PROXY_METHOD_H_


#define RELY_ERR 2


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

#define INSTALLABLE_BMA_PATH               "/data/ibma/Linux"
#define SP_DEVICE_INFO_BACKUP_FILE_PATH    "/data/var/run/sp_deviceinfo.json.bak"
#define DATA_LOG_FILE_PATH                 "/data/var/log/pme"

extern gint32 proxy_method_set_kvm_enable(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 proxy_method_set_kvm_port(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 proxy_method_set_screen_switch(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 proxy_method_set_kvm_encrypt_state(const OBJ_HANDLE object_handle, GSList *caller_info,
    GSList *input_list, GSList **output_list);
extern gint32 proxy_method_set_kvm_timeout(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 proxy_method_set_local_kvm_state(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 proxy_method_set_vmm_enable(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 proxy_method_set_vmm_port(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 proxy_method_set_vmm_encrypt_state(const OBJ_HANDLE object_handle, GSList *caller_info,
    GSList *input_list, GSList **output_list);
extern gint32 proxy_method_set_video_enable(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 proxy_method_set_video_port(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

extern gint32 proxy_method_driver_operation(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

extern gint32 proxy_method_format_device(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

extern gint32 proxy_ipmi_set_kvm_encry_info(gconstpointer msg, gpointer user_data);
extern gint32 proxy_ipmi_set_vmm_encry_info(gconstpointer msg, gpointer user_data);

extern void ui_manage_proxy_property_init(void);


gint32 CopyFile(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 CheckDir(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 ModifyFileOwn(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 ModifyFileMode(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 DeleteFile(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 CreateFile(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
#ifdef ARM64_HI1711_ENABLED
gint32 send_recv_msg_with_m3(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
#endif
gint32 RevertFileOwn(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 
#endif
