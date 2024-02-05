


#ifndef __E2P_CAFE_H__
#define __E2P_CAFE_H__

#include "common.h"

typedef void (*e2p_cafe_record_foreach_func)(const gchar *record_key, CAFE_RECORD_S *record_data, gpointer user_data);

CAFE_DATA_S *e2p_cafe_get_record_by_key(EEPROM_CAFE_SP e2p_cafe, CAFE_DATA_S *key);
gint32 e2p_cafe_clear_all_record(EEPROM_CAFE_SP e2p_cafe);
gint32 e2p_cafe_dump_fru_info(OBJ_HANDLE fru_handle, gchar *write_buf, guint32 max_write_buf_len);
EEPROM_CAFE_SP e2p_cafe_new(void);
gint32 e2p_cafe_free(EEPROM_CAFE_SP e2p_cafe);
gint32 e2p_cafe_update_to_e2p(gint32 area, gint32 filed, EEPROM_CAFE_SP cafe_info, const void *data_buf,
    guint32 buf_len);
gint32 init_tc_header_and_area_offsets(FRU_PRIV_PROPERTY_S *fru_priv);
gint32 init_cafe_info(FRU_PROPERTY_S *fru_prop, EEPROM_CAFE_SP cafe_info);
gint32 frudev_write_to_eeprom_custom(OBJ_HANDLE object_handle, guint32 offset, const gpointer write_buf,
    guint32 length);
gint32 init_fru_info_from_cafe_info(EEPROM_CAFE_SP cafe_info, FRU_INFO_S *fru_info);

gint32 e2p_cafe_record_write_specific_fru(OBJ_HANDLE fru_handle, const gchar *cafe_key_str, CAFE_DATA_S *cafe_data);
gint32 frudata_set_eeprom_scan_status(const guint8 status);
guint8 frudata_get_eeprom_scan_status(void);
void frudata_change_eeprom_scan_status(void);
void set_eeprom_stop_routine(const guint8 is_stop);

#endif
