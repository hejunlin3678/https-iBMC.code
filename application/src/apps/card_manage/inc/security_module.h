

#ifndef __SECURITY_MODULE_H__
#define __SECURITY_MODULE_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 



#define SECURITY_MODULE_MAX_STRING_LENGTH 200


extern gint32 security_module_set_self_test_result(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 security_module_set_specification_type(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 security_module_set_manufacturer_name(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 security_module_set_specification_version(OBJ_HANDLE object_handle, GSList *caller_info,
    GSList *input_list, GSList **output_list);
extern gint32 security_module_set_manufacturer_version(OBJ_HANDLE object_handle, GSList *caller_info,
    GSList *input_list, GSList **output_list);

gint32 securitymodule_add_object_callback(OBJ_HANDLE obj_handle);
gint32 securitymodule_del_object_callback(OBJ_HANDLE obj_handle);



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif 
