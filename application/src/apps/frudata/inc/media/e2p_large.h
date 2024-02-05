


#ifndef __FRU_MEDIA_E2P_LARGE_H__
#define __FRU_MEDIA_E2P_LARGE_H__

#include "common.h"

gint32 frudev_write_to_eeprom(OBJ_HANDLE object_handle, guint32 offset, const void *write_buf, guint32 length);
void write_eeprom_count_log(OBJ_HANDLE obj_handle, const char *relation_name);
gint32 frudev_write_to_back_eeprom(OBJ_HANDLE object_handle, guint32 offset, const gpointer write_buf, guint32 length);
gint32 frudev_write_to_eeprom_double(OBJ_HANDLE object_handle, guint32 offset, const gpointer write_buf,
    guint32 length);
gint32 frudev_read_from_eeprom(OBJ_HANDLE object_handle, guint32 offset, gpointer read_buf, guint32 length);

gint32 init_smm_backplane_fru_file(FRU_PROPERTY_S *fru_prop, FRU_INFO_S *tmp_fru_info);

gint32 get_back_plane_dev_handle(OBJ_HANDLE *dev_handle);

gint32 get_back_plane_fru_offset(guint16 *fru_offset);

gint32 write_info_to_raw_eeprom(OBJ_HANDLE dev_handle, const void *data, gsize data_length, gsize defined_length,
    gint32 offset, gsize block_size); // defined_length is used to calculate the checksum

gint32 update_chassis_fru_in_raw_eeprom(guint8 area, guint8 field, FRU_PRIV_PROPERTY_S *fru_priv, guint8 *buffer,
    guint16 length);

gint32 set_write_protect_flag(OBJ_HANDLE dev_handle, guint8 wpflag);

gint32 clear_chassis_fru_in_raw_eeprom(OBJ_HANDLE fru_handle, FRU_PRIV_PROPERTY_S *fru_priv);

gint32 frudev_read_from_eeprom_raw(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file);

gint32 frudev_read_from_eeprom_custom(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file);

void try_fix_fru_header(OBJ_HANDLE frudev_handle);

guint32 __calc_time_interval(const gchar *sztime);
gint32 lock_share_eeprom(OBJ_HANDLE object_handle);
void unlock_share_eeprom(OBJ_HANDLE object_handle);
void __add_extension_label(GList **extension_label, gchar *key, const gchar *data);

#ifdef DFT_ENABLED
gint32 shelf_eeprom_test(guint32 command, gsize para_size, gconstpointer para_data, DFT_RESULT_S *dft_data,
    gpointer user_data);
gint32 eeprom_self_test_dft_hook(OBJ_HANDLE handle, gpointer data);

#endif

#endif 
