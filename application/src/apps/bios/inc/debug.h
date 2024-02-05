
#ifndef DEBUG_H
#define DEBUG_H

#include "pme_app/pme_app.h"
#include "bios_ipmi.h"
#include "pme_app/xmlparser/xmlparser.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

gint32 bios_show_boot_options(GSList *input_list);
gint32 bios_show_file_change(GSList *input_list);
gint32 bios_show_data_operations(GSList *input_list);
gint32 show_upload_firmware_info(GSList *input_list);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 

#endif 