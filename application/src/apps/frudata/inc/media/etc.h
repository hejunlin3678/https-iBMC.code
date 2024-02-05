


#ifndef __FRU_MEDIA_ETC_H__
#define __FRU_MEDIA_ETC_H__

#include "common.h"

gint32 frudev_write_to_file(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file);
gint32 frudev_read_from_file(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file);
gint32 frudev_read_file_withoffset(OBJ_HANDLE object_handle, guint32 offset, gpointer read_buf, guint32 length);
gint32 frudev_write_file_withoffset(OBJ_HANDLE object_handle, guint32 offset, const gpointer write_buf, guint32 length);
#endif 
