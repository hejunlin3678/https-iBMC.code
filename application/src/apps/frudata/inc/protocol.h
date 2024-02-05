


#ifndef __FRU_PROTOCOL_H__
#define __FRU_PROTOCOL_H__

#include "common.h"

typedef gint32 (*PS_GET_FRU)(guint8 ps_id, PS_FRU_S *fru_data);

void init_default_fru_info(FRU_PROPERTY_S *fru_prop, FRU_INFO_S *fru_info);

gint32 get_eeprom_format(OBJ_HANDLE frudev_handle, guint8 *eeprom_format);

gint32 generate_fru_file(guint8 eeprom_format, FRU_INFO_S *fru_info, FRU_FILE_S *fru_file);

void transform_fru_data(guint8 ps_id, guint8 *fru_data, guint8 *output_data, guint32 output_size);

gint32 fru_block_write(OBJ_HANDLE obj_handle, const gchar *property_name, gint32 offset, gsize length,
    gconstpointer write_buf);

gint32 fru_block_read(OBJ_HANDLE obj_handle, const gchar *property_name, gint32 offset, gsize length,
    gpointer out_data);

gint32 read_fru_info(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file, PS_GET_FRU get_ps_fru_callback);

void fillin_fru_head(FRU_FILE_S *fru_file, gint32 data_length);

void fillin_eeprom_fru_head(guint8 eeprom_format, FRU_INFO_S *fru_info, FRU_FILE_S *fru_file, gint32 data_length);

gint32 check_fru_file(guint8 eeprom_format, const FRU_FILE_S *fru_file);

void create_fru_file(FRU_PROPERTY_S *fru_prop, FRU_FILE_S *fru_file);

#endif 
