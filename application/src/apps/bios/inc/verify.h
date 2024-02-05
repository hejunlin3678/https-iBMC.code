

#ifndef __BIOS_VERIFY_H__
#define __BIOS_VERIFY_H__

#include <syslog.h>
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "bios.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 
#ifdef ARM64_HI1711_ENABLED
gint32 method_verify_bios_firmware(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 ipmi_get_bios_gold_valid(const void *msg_data, gpointer user_data);
gint32 method_set_bios_fw_verify_result(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
void update_bios_bootguard_status(void);

void set_bios_reported_flag(gint32 flag);
gint32 get_bios_reported_flag(void);

#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif 
