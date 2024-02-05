


#ifndef __FRUDATA_H__
#define __FRUDATA_H__
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "common.h"

gint32 frudata_init(void);
gint32 fru_elabel_init(void);
gint32 frudata_start(void);
gint32 frudata_exit(REBOOT_CTRL ctrl);
gint32 create_cpld_scan_task(OBJ_HANDLE object_handle, gpointer user_data);
gint32 method_fru_stop_shared_cpld_scan(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

#endif
