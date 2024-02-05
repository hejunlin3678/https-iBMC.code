
#ifndef FILE_OP_H
#define FILE_OP_H

#include "pme_app/pme_app.h"
#include "bios_ipmi.h"
#include "pme_app/xmlparser/xmlparser.h"
#include "boot_options.h"

#define BIOS_FILE_NAME_MAX_LEN 128
#define BIOS_FILE_NAME_NUM 50
#define BIOS_FILE_NO_MORE_DATA 0x1
#define BIOS_FILE_MORE_DATA 0x0

#define BIOS_SILK_JSON_FILE_NAME        "/data/opt/pme/conf/bios/silkconfig.json"
#define BIOS_SILK_JSON_BAK_FILE_NAME    "/data/opt/pme/conf/bios/silkconfig_bak.json"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

gint32 bios_write_file_to_bmc(const void *req_msg, gpointer user_data);
gint32 bios_read_file_from_bmc(const void *req_msg, gpointer user_data);
gint32 load_file_to_buffer(FILE *file_handle, guint8 **data_buf, guint32 *data_len);
void init_bios_rd_data_cfg_mutex(void);
void init_file_op_comm_mutex(void);


extern guint32 g_rsp_manufactureid_bios;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 

#endif 