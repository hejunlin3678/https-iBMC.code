
#ifndef __REDFISH_PROVIDER_STORAGE_CONTROLLER_H__
#define __REDFISH_PROVIDER_STORAGE_CONTROLLER_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

void get_storage_controller_jbod_state_supported(OBJ_HANDLE controller_handle, json_object *oem_obj);
void get_storage_controller_epd_supported(OBJ_HANDLE controller_handle, json_object *oem_obj);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 

#endif 
