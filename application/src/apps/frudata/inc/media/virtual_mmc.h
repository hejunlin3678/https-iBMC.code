


#ifndef __FRU_MEDIA_VIRTUAL_MMC_H__
#define __FRU_MEDIA_VIRTUAL_MMC_H__

#include "common.h"

gint32 syn_frudata_to_fan_mcu(OBJ_HANDLE object_handle, guint32 offset, guint8 *fru_buf, guint16 length);
gint32 write_elabel_data_to_mcu(OBJ_HANDLE object_handle, const gchar *method_name, GSList *input_list,
    GSList **output_list);
gint32 syn_frudata_to_mcu(OBJ_HANDLE object_handle, FRU_PROPERTY_S *fru_prop);

gint32 frudev_read_from_mcu(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file);

gint32 frudev_read_fan_from_mcu(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file);

gint32 frudev_read_from_power_mcu(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file);

#endif 
