

#ifndef __BIOS_PWD_H__
#define __BIOS_PWD_H__

#include "pme_app/pme_app.h"
#include "pme_app/kmc/kmc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

gint32 method_bios_change_password(OBJ_HANDLE bios, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 ipmi_bios_change_pwd(const void *req_msg, gpointer user_data);
void init_bios_rollback_mutex(void);
void wait_encrypted_biospwd_rollback(void);
gint32 roll_back_bios_pwd(OBJ_HANDLE obj_handle, gpointer user_data, gchar *property_name, GVariant *property_value);
gint32 kmc_update_bios_ciphertext(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif 
