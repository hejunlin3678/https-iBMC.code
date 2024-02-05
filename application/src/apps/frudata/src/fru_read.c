

#include "protocol.h"
#include "prepare_fru.h"
#include "media/custom_mcu.h"


gint32 frudev_read_arcard_fru_custom(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file)
{
    gint32 ret;
    guint8 frudata[FRU_AREA_MAX_LEN] = { 0 };
    FRU_INFO_S fru_info = { 0 };

    if (fru_file == NULL) {
        debug_log(DLOG_ERROR, "%s: (%s) fruinfo param error.", __FUNCTION__, dfl_get_object_name(object_handle));
        return RET_ERR;
    }

    ret = (gint32)fru_block_read(object_handle, PROPERTY_FRUDEV_STORAGE_LOC, 0, FRU_AREA_MAX_LEN, frudata);
    if (ret == RET_OK) {
        change_pangea_card_elabel_format(frudata, FRU_AREA_MAX_LEN, object_handle, &fru_info);
        (void)generate_fru_file(EEPROM_FORMAT_V2, &fru_info, fru_file);
    } else {
        debug_log(DLOG_ERROR, "%s: read arcard(%s) eeprom failed, ret=%d.", __FUNCTION__,
            dfl_get_object_name(object_handle), ret);
    }

    return ret;
}
