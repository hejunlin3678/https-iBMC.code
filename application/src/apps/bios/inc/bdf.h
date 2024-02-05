
#ifndef BDF_H
#define BDF_H

#include "pme_app/pme_app.h"
#include "bios_ipmi.h"
#include "pme_app/xmlparser/xmlparser.h"
#include "boot_options.h"

#define BIOS_FILE_NAME_MAX_LEN 128
#define BIOS_FILE_NO_MORE_DATA 0x1
#define BIOS_FILE_MORE_DATA 0x0

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

gint32 bios_write_bdf_data_to_bmc(const void *req_msg, gpointer user_data);
void init_bdf_comm_mutex(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 

#endif 