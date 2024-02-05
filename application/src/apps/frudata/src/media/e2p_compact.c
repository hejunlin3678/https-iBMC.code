

#include <arpa/inet.h>
#include "protocol.h"
#include "media/e2p_large.h"
#include "pme_app/dalib/dal_obj_priv_data.h"
#include "prepare_fru.h"
#include "media/e2p_compact.h"

#define E2P_WRITE_PROTECT_OPEN 1

typedef gint32 (*UPDATE_FRU_BOARD_DATA_FUNC)(CAFE_DATA_S *cafe_data, FRU_BOARD_AREA_S *fru_board_area_info);

typedef gint32 (*UPDATE_FRU_PRODUCT_DATA_FUNC)(CAFE_DATA_S *cafe_data, FRU_PRODUCT_AREA_S *fru_product_area_info);

typedef struct tag_update_fru_board_data {
    gint32 filed_index;
    UPDATE_FRU_BOARD_DATA_FUNC __update_fru_board_data;
} UPDATE_FRU_BOARD_DATA;

typedef struct tag_update_fru_product_data {
    gint32 filed_index;
    UPDATE_FRU_PRODUCT_DATA_FUNC __update_fru_product_data;
} UPDATE_FRU_PRODUCT_DATA;

LOCAL void __e2p_cafe_record_foreach(EEPROM_CAFE_SP e2p_cafe, e2p_cafe_record_foreach_func func, gpointer user_data);
LOCAL gint32 __e2p_cafe_init(guchar cafe_ver, guchar chk_fail_action, guint32 offset, guint32 size,
    EEPROM_CAFE_SP e2p_cafe);
LOCAL gint32 __e2p_cafe_write_record_by_key(EEPROM_CAFE_SP e2p_cafe, CAFE_DATA_S *key, CAFE_DATA_S *value);
LOCAL gint32 __e2p_cafe_get_fru_loc_info_by_cafe_key(const gchar *cafe_key, CAFE_KEY_TO_FRU_LOC_INFO_S **fru_loc_info);
LOCAL gint32 __e2p_cafe_get_fru_loc_info_by_area_filed(gint32 area, gint32 filed,
    CAFE_KEY_TO_FRU_LOC_INFO_S **fru_loc_info);
LOCAL void __e2p_cafe_init_frudev_handle(EEPROM_CAFE_SP e2p_cafe, OBJ_HANDLE frudev_handle);
LOCAL gint32 __e2p_cafe_type_check(EEPROM_CAFE_SP e2p_cafe);
LOCAL gint32 __update_fru_board_mfgdata(CAFE_DATA_S *cafe_data, FRU_BOARD_AREA_S *fru_board_area_info);
LOCAL gint32 __update_fru_board_product_name(CAFE_DATA_S *cafe_data, FRU_BOARD_AREA_S *fru_board_area_info);
LOCAL gint32 __update_fru_board_serial_number(CAFE_DATA_S *cafe_data, FRU_BOARD_AREA_S *fru_board_area_info);
LOCAL gint32 __update_fru_board_part_number(CAFE_DATA_S *cafe_data, FRU_BOARD_AREA_S *fru_board_area_info);
LOCAL gint32 __update_fru_product_name(CAFE_DATA_S *cafe_data, FRU_PRODUCT_AREA_S *fru_product_area_info);
LOCAL gint32 __update_fru_product_part_number(CAFE_DATA_S *cafe_data, FRU_PRODUCT_AREA_S *fru_product_area_info);
LOCAL gint32 __update_fru_product_serial_number(CAFE_DATA_S *cafe_data, FRU_PRODUCT_AREA_S *fru_product_area_info);
LOCAL gint32 __update_fru_product_assettag(CAFE_DATA_S *cafe_data, FRU_PRODUCT_AREA_S *fru_product_area_info);

G_LOCK_DEFINE(g_lock_write_to_cafe_e2p);

guint8 g_eeprom_scan_status = SCAN_STARTED;
G_LOCK_DEFINE(g_lock_eeprom_scan_status);


LOCAL CAFE_KEY_TO_FRU_LOC_INFO_S g_cafe_key_to_fru_loc_info[] = {
    {ChType,                 FRU_AREA_CHASSISINFO,         FRU_CHASSIS_TYPE,              CAFE_INDEX_CHTYPE},
    {ChPN,                   FRU_AREA_CHASSISINFO,         FRU_CHASSIS_PART_NUMBER,       CAFE_INDEX_CHPN},
    {ChSN,                   FRU_AREA_CHASSISINFO,         FRU_CHASSIS_SERIAL_NUMBER,     CAFE_INDEX_CHSN},
    {MDate,                  FRU_AREA_BOARDINFO,           FRU_BOARD_MFGDATE,             CAFE_INDEX_MDATE},
    {BName,                  FRU_AREA_BOARDINFO,           FRU_BOARD_PRODUCTNAME,         CAFE_INDEX_BNAME},
    {BSN,                    FRU_AREA_BOARDINFO,           FRU_BOARD_SERIALNUMBER,        CAFE_INDEX_BSN},
    {BPN,                    FRU_AREA_BOARDINFO,           FRU_BOARD_PARTNUMBER,          CAFE_INDEX_BPN},
    {PName,                  FRU_AREA_PRODUCTINFO,         FRU_PRODUCT_NAME,              CAFE_INDEX_PNAME},
    {PPN,                    FRU_AREA_PRODUCTINFO,         FRU_PRODUCT_PARTNUMBER,        CAFE_INDEX_PPN},
    {PSN,                    FRU_AREA_PRODUCTINFO,         FRU_PRODUCT_SERIALNUMBER,      CAFE_INDEX_PSN},
    {AssetTag,               FRU_AREA_PRODUCTINFO,         FRU_PRODUCT_ASSETTAG,          CAFE_INDEX_ASSETTAG},
    {EncLoc,                 FRU_AREA_INVALID,             FRU_FILED_INVALID,             CAFE_INDEX_ENCLOC},
    {TopoPCIe,               FRU_AREA_INVALID,             FRU_FILED_INVALID,             CAFE_INDEX_TOPOPCIE},
    {ChSubnet,               FRU_AREA_INVALID,             FRU_FILED_INVALID,             CAFE_INDEX_CHSUBNET}
};

LOCAL UPDATE_FRU_BOARD_DATA g_update_fru_board_data[] = {
    {FRU_BOARD_MFGDATE, __update_fru_board_mfgdata},
    {FRU_BOARD_PRODUCTNAME, __update_fru_board_product_name},
    {FRU_BOARD_SERIALNUMBER, __update_fru_board_serial_number},
    {FRU_BOARD_PARTNUMBER, __update_fru_board_part_number},
};

LOCAL UPDATE_FRU_PRODUCT_DATA g_update_fru_product_data[] = {
    {FRU_PRODUCT_NAME, __update_fru_product_name},
    {FRU_PRODUCT_PARTNUMBER, __update_fru_product_part_number},
    {FRU_PRODUCT_SERIALNUMBER, __update_fru_product_serial_number},
    {FRU_PRODUCT_ASSETTAG, __update_fru_product_assettag},
};

LOCAL gint32 __e2p_chip_read(OBJ_HANDLE fru_dev_handle, guchar chip_pos, gint32 offset, gsize length, gpointer out_data)
{
    OBJ_HANDLE chip_handle = 0;
    gint32 ret = dal_get_refobject_handle_nth(fru_dev_handle, PROPERTY_CAFEDEV_CAFE_PAGE_CHIPS, chip_pos, &chip_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get chip handle at [%d] failed", chip_pos);
        return ret;
    }

    const gchar *chip_name = dfl_get_object_name(chip_handle);
    if (chip_name == NULL) {
        debug_log(DLOG_ERROR, "%s:get object name for handle %" OBJ_HANDLE_FORMAT " failed", __FUNCTION__, chip_handle);
        return RET_ERR;
    }

    debug_log(DLOG_INFO, "begin to read data from [%s], offset:%d, length:%" G_GSIZE_FORMAT "\n", chip_name, offset,
        length);
    ret = dfl_chip_blkread(chip_name, offset, length, out_data);

    return ret;
}

LOCAL gint32 __e2p_page_read(OBJ_HANDLE fru_dev_handle, gint32 offset, gsize length, gpointer out_data)
{
    gint32 data_readed = 0;
    gint32 start_page = offset / CAFE_EEPROM_PAGE_SIZE;
    gint32 end_page = (offset + length - 1) / CAFE_EEPROM_PAGE_SIZE;

    for (gint32 i = start_page; i <= end_page; i++) {
        gint32 cur_offset;
        gint32 cur_read_len;
        if (i == start_page) {
            cur_offset = offset % CAFE_EEPROM_PAGE_SIZE;
            cur_read_len = ((offset % CAFE_EEPROM_PAGE_SIZE + length) >= CAFE_EEPROM_PAGE_SIZE) ?
                (CAFE_EEPROM_PAGE_SIZE - offset % CAFE_EEPROM_PAGE_SIZE) :
                length;
        } else if (i == end_page) {
            cur_offset = 0;
            cur_read_len = (offset + length - 1) % CAFE_EEPROM_PAGE_SIZE + 1;
        } else {
            cur_offset = 0;
            cur_read_len = CAFE_EEPROM_PAGE_SIZE;
        }

        gint32 ret = __e2p_chip_read(fru_dev_handle, i, cur_offset, cur_read_len, (guchar *)out_data + data_readed);
        data_readed += cur_read_len;

        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "read data at page [%d] failed, read offset:[%d], read len:[%d]", i, cur_offset,
                cur_read_len);
            return ret;
        }
    }

    return RET_OK;
}

LOCAL gint32 __e2p_read(OBJ_HANDLE fru_dev_handle, gint32 offset, gsize length, gpointer out_data)
{
    gint32 ret = RET_OK;
    debug_log(DLOG_INFO, "begin to read data from dev %s, current offset:%d, len:%" G_GSIZE_FORMAT,
        dfl_get_object_name(fru_dev_handle), offset, length);

    guchar cafe_rw_mode = 0;
    (void)dal_get_property_value_byte(fru_dev_handle, PROPERTY_CAFEDEV_CAFE_RW_NODE, &cafe_rw_mode);
    switch (cafe_rw_mode) {
        case CAFE_RW_MODE_COMMON:
            ret = dfl_block_read(fru_dev_handle, PROPERTY_CAFEDEV_CAFE_ACCESSOR, offset, length, out_data);
            break;

        case CAFE_RW_MODE_PAGE:
            ret = __e2p_page_read(fru_dev_handle, offset, length, out_data);
            break;

        default:
            ret = RET_ERR;
            debug_log(DLOG_ERROR, "%s:unknown cafe rw mode:%d", __FUNCTION__, cafe_rw_mode);
            break;
    }

    debug_log(DLOG_INFO, "read data from dev %s end", dfl_get_object_name(fru_dev_handle));

    return ret;
}

LOCAL gint32 __e2p_chip_write(OBJ_HANDLE dev_handle, guchar chip_handle_pos, gint32 offset, gsize length,
    gpointer in_data)
{
    OBJ_HANDLE chip_handle = 0;
    gint32 ret =
        dal_get_refobject_handle_nth(dev_handle, PROPERTY_CAFEDEV_CAFE_PAGE_CHIPS, chip_handle_pos, &chip_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get chip handle at postion [%d] failed", chip_handle_pos);
        return RET_ERR;
    }

    const gchar *chip_name = dfl_get_object_name(chip_handle);
    if (chip_name == NULL) {
        debug_log(DLOG_ERROR, "get objet name with handle [%" OBJ_HANDLE_FORMAT "] failed", chip_handle);
        return RET_ERR;
    }

    debug_log(DLOG_INFO, "begin to write data to chip %s, offset:%d, size:%" G_GSIZE_FORMAT "\n", chip_name, offset,
        length);

    G_LOCK(g_lock_write_to_cafe_e2p);
    
    guint8 wpflag = E2P_WRITE_PROTECT_OPEN;
    (void)dal_get_property_value_byte(dev_handle, PROPERTY_FRUDEV_ISWP, &wpflag);
    
    if (wpflag == E2P_WRITE_PROTECT_OPEN) {
        ret = dal_frudev_opt_eeprom_wp(dev_handle, PROPERTY_FRUDEV_EEPROM_WP, CLOSE_WP);
        if (ret != RET_OK) {
            G_UNLOCK(g_lock_write_to_cafe_e2p);
            debug_log(DLOG_ERROR, "close wp fail: %d", ret);
            return ERROR_CODE_WRITE_E2P_ERROR;
        }
    }

    // 5： retry times
    for (guint32 i = 0; i < 5; i++) {
        debug_log(DLOG_INFO, "chip name:%s, write length:%" G_GSIZE_FORMAT " ,offset:%d", chip_name, length, offset);
        ret = dfl_chip_blkwrite(chip_name, offset, length, in_data);
        if (ret == DFL_OK) {
            break;
        }
        vos_task_delay(200);
    }

    
    if (wpflag == E2P_WRITE_PROTECT_OPEN) {
        if (dal_frudev_opt_eeprom_wp(dev_handle, PROPERTY_FRUDEV_EEPROM_WP, OPEN_WP) != RET_OK) {
            G_UNLOCK(g_lock_write_to_cafe_e2p);
            debug_log(DLOG_ERROR, "open wp fail");
            return ERROR_CODE_WRITE_E2P_ERROR;
        }
    }

    G_UNLOCK(g_lock_write_to_cafe_e2p);

    debug_log(DLOG_INFO, "write data to dev %s end, ret is %d", chip_name, ret);

    if (ret == DFL_OK) {
        return RET_OK;
    }

    debug_log(DLOG_ERROR, "write eeprom failed!(ret: %d).", ret);
    return ERROR_CODE_WRITE_E2P_ERROR;
}

LOCAL gint32 __e2p_page_write(OBJ_HANDLE dev_handle, gint32 offset, gsize length, gpointer in_data)
{
    gint32 start_page = offset / CAFE_EEPROM_PAGE_SIZE;
    gint32 end_page = (offset + length - 1) / CAFE_EEPROM_PAGE_SIZE;

    debug_log(DLOG_INFO, "e2p page write:start page:%d, end page:%d", start_page, end_page);
    gint32 length_writed = 0;
    for (gint32 i = start_page; i <= end_page; i++) {
        gint32 cur_write_len;
        gint32 cur_offset;
        if (i == start_page) { 
            cur_offset = offset % CAFE_EEPROM_PAGE_SIZE;
            cur_write_len = (offset % CAFE_EEPROM_PAGE_SIZE + length) >= CAFE_EEPROM_PAGE_SIZE ?
                (CAFE_EEPROM_PAGE_SIZE - offset % CAFE_EEPROM_PAGE_SIZE) :
                length;
        } else if (i == end_page) { 
            cur_offset = 0;
            cur_write_len = (offset + length - 1) % CAFE_EEPROM_PAGE_SIZE + 1;
        } else { 
            cur_offset = 0;
            cur_write_len = CAFE_EEPROM_PAGE_SIZE;
        }

        gint32 ret = __e2p_chip_write(dev_handle, i, cur_offset, cur_write_len, (guchar *)in_data + length_writed);
        length_writed += cur_write_len;

        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "write cafe to chip at page [%d] failed", i);
            return ret;
        }
    }

    return RET_OK;
}

LOCAL gint32 __e2p_write(OBJ_HANDLE dev_handle, gint32 offset, gsize length, gpointer in_data)
{
    gint32 ret = RET_ERR;

    guchar cafe_write_mode = 0;
    (void)dal_get_property_value_byte(dev_handle, PROPERTY_CAFEDEV_CAFE_RW_NODE, &cafe_write_mode);

    switch (cafe_write_mode) {
        case CAFE_RW_MODE_COMMON:
            ret = __e2p_chip_write(dev_handle, 0, offset, length, in_data);
            break;

        case CAFE_RW_MODE_PAGE:
            ret = __e2p_page_write(dev_handle, offset, length, in_data);
            break;

        default:
            debug_log(DLOG_ERROR, "unsupported cafe write mode:%d", cafe_write_mode);
            break;
    }

    return ret;
}

LOCAL guint8 __make_checksum(guint8 *data, gint32 size)
{
    guint8 checksum = 0;

    while (size-- > 0) {
        checksum += *data++;
    }

    return (guint8)(-checksum);
}

LOCAL void __iblock_release(EEPROM_CAFE_S *e2p_cafe, INODE_BLOCK_V1_S *iblock)
{
    e2p_cafe->inode_used = g_list_remove(e2p_cafe->inode_used, iblock);
    e2p_cafe->inode_free = g_list_append(e2p_cafe->inode_free, iblock);
}

LOCAL void __iblock_occupy(EEPROM_CAFE_S *e2p_cafe, INODE_BLOCK_V1_S *iblock)
{
    e2p_cafe->inode_free = g_list_remove(e2p_cafe->inode_free, iblock);
    e2p_cafe->inode_used = g_list_append(e2p_cafe->inode_used, iblock);
}

LOCAL gint32 __iblock_reserve(EEPROM_CAFE_S *e2p_cafe, INODE_BLOCK_V1_S **iblock)
{
    GList *list = e2p_cafe->inode_free;

    if (list) {
        *iblock = (INODE_BLOCK_V1_S *)list->data;
    }

    return list ? RET_OK : RET_ERR;
}

LOCAL void __dblocks_release(EEPROM_CAFE_S *e2p_cafe, DATA_BLOCK_V1_S **dblocks, guint8 block_num)
{
    for (guint8 i = 0; i < block_num; i++) {
        e2p_cafe->data_used = g_list_remove(e2p_cafe->data_used, dblocks[i]);
        e2p_cafe->data_free = g_list_append(e2p_cafe->data_free, dblocks[i]);
    }
}

LOCAL void __dblocks_release_by_ids(EEPROM_CAFE_S *e2p_cafe, guint8 *data, guint8 block_num)
{
    struct data_block_v1 *dblocks[sizeof(((INODE_BLOCK_V1_S *)0)->data)];

    for (guint8 i = 0; i < block_num; i++) {
        dblocks[i] = e2p_cafe->dblock + data[i];
    }
    __dblocks_release(e2p_cafe, dblocks, block_num);

    debug_log(DLOG_INFO, "after data block release, free data block num is %d", g_list_length(e2p_cafe->data_free));
}

LOCAL void __dblocks_occupy(EEPROM_CAFE_S *e2p_cafe, struct data_block_v1 **dblocks, guint8 block_num)
{
    for (guint8 i = 0; i < block_num; i++) {
        e2p_cafe->data_free = g_list_remove(e2p_cafe->data_free, dblocks[i]);
        e2p_cafe->data_used = g_list_append(e2p_cafe->data_used, dblocks[i]);
    }
}

LOCAL gint32 __dblocks_reserve(EEPROM_CAFE_S *e2p_cafe, struct data_block_v1 **dblocks, guint8 block_num)
{
    GList *list = e2p_cafe->data_free;

    debug_log(DLOG_INFO, "reserve data block num:%d, data free length:%d", block_num, g_list_length(list));

    while ((block_num > 0) && list) {
        *(dblocks++) = (DATA_BLOCK_V1_S *)list->data;
        list = list->next;

        block_num--;
    }

    return (block_num == 0) ? RET_OK : RET_ERR;
}

LOCAL gint32 __cafe_data_cpy(CAFE_DATA_S *dest, CAFE_DATA_S *src)
{
    if (dest == NULL || src == NULL || src->data == NULL) {
        debug_log(DLOG_ERROR, "%s: imput param error.", __FUNCTION__);
        return RET_ERR;
    }

    dest->type = src->type;
    dest->len = src->len;
    dest->data = (guint8 *)g_malloc0(dest->len);
    if (dest->data == NULL) {
        debug_log(DLOG_ERROR, "%s:alloc new cafe data failed", __FUNCTION__);
        return RET_ERR;
    }

    errno_t safe_fun_ret = memcpy_s(dest->data, dest->len, src->data, src->len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "Call memcpy_s return error");
        return RET_ERR;
    }
    return RET_OK;
}

LOCAL void __metadata_checkpoint(EEPROM_CAFE_S *cafe, JOURNAL_V1_S *journal)
{
    for (guint8 i = 0; i < journal->count; i++) {
        guint8 inode = journal->transactions[i].inode;
        guint8 action = journal->transactions[i].action;
        gint32 offset = cafe->super->iblock_offset * 8;

        if (inode >= cafe->super->iblock_num) {
            break;
        }

        offset += cafe->super->iblock_size * inode;

        switch (action) {
            case INODE_ADD:
                cafe->iblock[inode].flag = INODE_VALID;
                break;
            case INODE_DEL:
                cafe->iblock[inode].flag = INODE_INVALID;
                break;
            default:
                
                break;
        }
        __e2p_write(cafe->frudev_handle, offset, sizeof(cafe->iblock[inode].flag), &cafe->iblock[inode].flag);
    }
}

LOCAL gint32 __journal_transaction_append(JOURNAL_V1_S *journal, guint8 inode, guint8 action)
{
    guint8 count = journal->count;

    count++;
    if (count > ARRAY_SIZE(journal->transactions)) {
        return RET_ERR;
    }

    journal->begin = TXB;
    journal->count = count;
    journal->transactions[count - 1].inode = inode;
    journal->transactions[count - 1].action = action;
    journal->checksum =
        __make_checksum(&journal->count, offsetof(JOURNAL_V1_S, checksum) - offsetof(JOURNAL_V1_S, count));
    journal->end = TXE;

    return RET_OK;
}

LOCAL JOURNAL_V1_S *__journal_alloc_0(EEPROM_CAFE_S *cafe)
{
    JOURNAL_V1_S *journal;
    cafe->journal_index = (cafe->journal_index + 1) % cafe->super->journal_num;
    journal = cafe->journal + cafe->journal_index;
    (void)memset_s(journal, sizeof(JOURNAL_V1_S), 0, sizeof(JOURNAL_V1_S));

    return journal;
}

LOCAL void __journal_write(EEPROM_CAFE_S *cafe, JOURNAL_V1_S *journal)
{
    gint32 offset = (gint32)(cafe->super->journal_offset * 8 + (journal - cafe->journal) * cafe->super->journal_size);

    __e2p_write(cafe->frudev_handle, offset, offsetof(JOURNAL_V1_S, end), journal);
    __e2p_write(cafe->frudev_handle, offset + offsetof(JOURNAL_V1_S, end), sizeof(journal->end), &journal->end);
}

LOCAL void __journal_free(EEPROM_CAFE_S *cafe, JOURNAL_V1_S *journal)
{
    gint32 offset = cafe->super->journal_offset * 8 + (journal - cafe->journal) * cafe->super->journal_size;

    (void)memset_s(journal, sizeof(JOURNAL_V1_S), 0xFF, sizeof(JOURNAL_V1_S));
    __e2p_write(cafe->frudev_handle, offset + offsetof(JOURNAL_V1_S, end), sizeof(journal->end), &journal->end);
    __e2p_write(cafe->frudev_handle, offset, offsetof(JOURNAL_V1_S, end), journal);
}

LOCAL void __journal_replay(EEPROM_CAFE_S *cafe, JOURNAL_V1_S *journal)
{
    __metadata_checkpoint(cafe, journal);
    __journal_free(cafe, journal);
}

LOCAL void __journal_replay_all(EEPROM_CAFE_S *cafe)
{
    guint8 i;
    SUPER_BLOCK_V1_S *super = cafe->super;
    JOURNAL_V1_S *journal = cafe->journal;
    guint8 payload_len = offsetof(JOURNAL_V1_S, end) - offsetof(JOURNAL_V1_S, count);

    
    for (i = 0; i < super->journal_num; i++, journal++) {
        if ((journal->begin == TXB) && (journal->end == TXE) && (__make_checksum(&journal->count, payload_len) == 0)) {
            cafe->journal_index = i;
            __journal_replay(cafe, journal);
            break;
        }
    }

    
    for (; i < super->journal_num; i++, journal++) {
        if ((journal->begin == TXB) && (journal->end == TXE) && (__make_checksum(&journal->count, payload_len) == 0)) {
            __journal_free(cafe, journal);
        }
    }
}

LOCAL void __iblocks_init(EEPROM_CAFE_S *cafe)
{
    SUPER_BLOCK_V1_S *super = cafe->super;
    INODE_BLOCK_V1_S *iblock = cafe->iblock;
    gint32 payload_len = sizeof(INODE_BLOCK_V1_S) - offsetof(INODE_BLOCK_V1_S, len);

    debug_log(DLOG_INFO, "%s: payload len is %d", __FUNCTION__, payload_len);

    for (guint8 i = 0; i < super->iblock_num; i++) {
        guint8 chk_sum_result = __make_checksum(&iblock->len, payload_len);
        if (iblock->flag == INODE_VALID) {
            debug_log(DLOG_INFO, "check sum for %s is %d", iblock->key, chk_sum_result);
        }
        if (iblock->flag == INODE_VALID && chk_sum_result == 0) {
            cafe->inode_used = g_list_append(cafe->inode_used, iblock);
        } else {
            cafe->inode_free = g_list_append(cafe->inode_free, iblock);
        }
        iblock++;
    }
}

LOCAL void __dblocks_init(EEPROM_CAFE_S *cafe)
{
    SUPER_BLOCK_V1_S *super = cafe->super;
    struct data_block_v1 *dblock = cafe->dblock;

    for (guint8 i = 0; i < super->dblock_num; i++) {
        cafe->data_free = g_list_append(cafe->data_free, dblock);
        dblock++;
    }

    GList *list = cafe->inode_used;
    while (list) {
        INODE_BLOCK_V1_S *iblock = (INODE_BLOCK_V1_S *)list->data;

        for (guint8 i = 0; i < iblock->size && i < ARRAY_SIZE(iblock->data); i++) {
            if (iblock->data[i] < super->dblock_num) {
                dblock = cafe->dblock + iblock->data[i];
                cafe->data_used = g_list_append(cafe->data_used, dblock);
                cafe->data_free = g_list_remove(cafe->data_free, dblock);
            }
        }
        list = list->next;
    }
}

LOCAL CAFE_DATA_S *__get_record_key(INODE_BLOCK_V1_S *iblock)
{
    CAFE_DATA_S *key = (CAFE_DATA_S *)g_malloc0(sizeof(CAFE_DATA_S));
    if (key == NULL) {
        debug_log(DLOG_ERROR, "%s:alloc new record key failed", __FUNCTION__);
        return NULL;
    }

    key->type = TYPE_8BIT_ASCII; 
    key->len = iblock->len;
    key->data = (guint8 *)g_malloc0(key->len + 1);
    if (key->data == NULL) {
        debug_log(DLOG_ERROR, "%s:alloc new record key data failed", __FUNCTION__);
        g_free(key);
        return NULL;
    }

    errno_t safe_fun_ret = memcpy_s(key->data, key->len + 1, iblock->key, key->len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }
    key->data[key->len] = '\0';

    return key;
}

LOCAL CAFE_DATA_S *__get_record_value(struct data_block_v1 **dblock, guint8 block_num)
{
    CAFE_DATA_S *value = (CAFE_DATA_S *)g_malloc0(sizeof(CAFE_DATA_S));
    if (value == NULL) {
        debug_log(DLOG_ERROR, "%s:alloc new record value failed", __FUNCTION__);
        return NULL;
    }

    guint8 len = 0;
    guint8 offset = 0;
    value->data = NULL;
    for (guint8 i = 0; i < block_num; i++) {
        guint8 size = (guint8)sizeof(dblock[i]->data);
        guint8 *data = dblock[i]->data;

        if (i == 0) {
            value->type = DATA_TYPE(dblock[i]->data[0]);
            value->len = CAFE_DATA_LEN(dblock[i]->data[0]);
            len = value->len + 1; 
            value->data = (guint8 *)g_malloc0(len);

            if (value->data == NULL) {
                debug_log(DLOG_ERROR, "%s:alloc new record value failed", __FUNCTION__);
                g_free(value);
                return NULL;
            }

            data++; 
            size--; 
            offset = 0;
            debug_log(DLOG_INFO, "value type:[%#x], value len:[%d]", value->type, value->len);
        }
        size = size > len ? len : size;
        errno_t safe_fun_ret = memcpy_s(value->data + offset, len, data, size);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        }
        len -= size;
        offset += size;
    }

    guchar chk_result = __make_checksum(value->data, value->len);
    if (value->data == NULL) {
        g_free(value);
        value = NULL;
    } else if (value->data[value->len] != chk_result) {
        debug_log(DLOG_INFO, "%s:chk sum error, expect:[%#x], result:[%#x]", __FUNCTION__, value->data[value->len],
            chk_result);
        g_free(value->data);
        value->data = NULL;
        g_free(value);
        value = NULL;
    }

    return value;
}

LOCAL void __record_destroy(void *data)
{
    CAFE_RECORD_S *record = (CAFE_RECORD_S *)data;
    if (record == NULL) {
        return;
    }

    if (record->key) {
        if (record->key->data) {
            g_free(record->key->data);
        }
        g_free(record->key);
    }
    if (record->value) {
        if (record->value->data) {
            g_free(record->value->data);
        }
        g_free(record->value);
    }
    g_free(record);
}

LOCAL void __record_remove(EEPROM_CAFE_S *cafe, CAFE_DATA_S *key)
{
    gchar str[sizeof(((INODE_BLOCK_V1_S *)0)->key) + 1] = {0};

    errno_t safe_fun_ret = memcpy_s(str, sizeof(str), key->data, key->len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }
    str[sizeof(str) - 1] = '\0';
    g_hash_table_remove(cafe->records, str);
}

LOCAL CAFE_RECORD_S *__record_get(EEPROM_CAFE_S *e2p_cafe, CAFE_DATA_S *key)
{
    gchar str[sizeof(((INODE_BLOCK_V1_S *)0)->key) + 1] = {0};
    errno_t safe_fun_ret = memcpy_s(str, sizeof(str), key->data, key->len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }
    str[sizeof(str) - 1] = '\0';

    debug_log(DLOG_INFO, "begin to find record [%s], current record length is %d", str,
        g_hash_table_size(e2p_cafe->records));

    return (CAFE_RECORD_S *)g_hash_table_lookup(e2p_cafe->records, str);
}

LOCAL CAFE_RECORD_S *__record_add(EEPROM_CAFE_S *e2p_cafe, CAFE_DATA_S *key, CAFE_DATA_S *value, guint8 inode)
{
    CAFE_RECORD_S *record = (CAFE_RECORD_S *)g_malloc0(sizeof(CAFE_RECORD_S));
    if (record == NULL) {
        debug_log(DLOG_ERROR, "%s:alloc new record failed", __FUNCTION__);
        return NULL;
    }

    record->key = (CAFE_DATA_S *)g_malloc0(sizeof(CAFE_DATA_S));
    if (record->key == NULL) {
        debug_log(DLOG_ERROR, "%s:alloc new record key failed", __FUNCTION__);
        g_free(record);
        return NULL;
    }

    record->value = (CAFE_DATA_S *)g_malloc0(sizeof(CAFE_DATA_S));
    if (record->value == NULL) {
        debug_log(DLOG_ERROR, "%s:alloc new record value failed", __FUNCTION__);
        g_free(record->key);
        g_free(record);
        return NULL;
    }

    (void)__cafe_data_cpy(record->key, key);
    (void)__cafe_data_cpy(record->value, value);
    record->inode = inode;

    gchar str[sizeof(((INODE_BLOCK_V1_S *)0)->key) + 1] = {0};
    errno_t safe_fun_ret = memcpy_s(str, sizeof(str), key->data, key->len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }
    str[sizeof(str) - 1] = '\0';

    gchar *hash_table_key = g_strdup(str);
    if (hash_table_key == NULL) {
        debug_log(DLOG_ERROR, "copy string for %s failed", str);
        g_free(record->key);
        g_free(record->value);
        g_free(record);
        return NULL;
    }

    g_hash_table_insert(e2p_cafe->records, (gpointer)hash_table_key, (gpointer)record);

    debug_log(DLOG_INFO, "after insert record for [%s], length is %d", str, g_hash_table_size(e2p_cafe->records));

    return record;
}

LOCAL gint32 __records_init(EEPROM_CAFE_S *cafe)
{
    if (cafe->records == NULL) {
        cafe->records = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, __record_destroy);
    }

    SUPER_BLOCK_V1_S *super = cafe->super;
    GList *list = cafe->inode_used;
    while (list) {
        INODE_BLOCK_V1_S *iblock = (INODE_BLOCK_V1_S *)list->data;
        CAFE_RECORD_S *record = (CAFE_RECORD_S *)g_malloc0(sizeof(CAFE_RECORD_S));
        if (record == NULL) {
            debug_log(DLOG_ERROR, "%s:alloc new record failed", __FUNCTION__);
            return RET_ERR;
        }

        record->key = __get_record_key(iblock);
        if (record->key == NULL) {
            g_free(record);
            continue;
        }
        record->inode = iblock - cafe->iblock; 
        record->value = NULL;

        gchar *hash_table_key = g_strdup((const gchar *)record->key->data);
        if (hash_table_key == NULL) {
            debug_log(DLOG_ERROR, "alloc hash table key failed");
            g_free(record->key->data);
            g_free(record->key);
            g_free(record);
            continue;
        }

        struct data_block_v1 *dblock[ARRAY_SIZE(iblock->data)] = {NULL};
        guint8 i = 0;
        for (; i < iblock->size && i < ARRAY_SIZE(iblock->data); i++) {
            if (iblock->data[i] >= super->dblock_num)
                break;
            dblock[i] = cafe->dblock + iblock->data[i];
        }
        record->value = (i < iblock->size) ? NULL : __get_record_value(dblock, iblock->size);

        g_hash_table_insert(cafe->records, (gpointer)hash_table_key, (gpointer)record);
        list = list->next;
    }

    return RET_OK;
}

LOCAL gint32 __schema_init(EEPROM_CAFE_S *cafe)
{
    SUPER_BLOCK_V1_S *super = cafe->super;

    guint32 size = super->journal_size * super->journal_num;
    cafe->journal = (JOURNAL_V1_S *)g_malloc0(size);
    if (cafe->journal == NULL) {
        debug_log(DLOG_ERROR, "%s:alloc new journal failed", __FUNCTION__);
        return RET_ERR;
    }

    (void)__e2p_read(cafe->frudev_handle, super->journal_offset * 8, size, cafe->journal);
    cafe->journal_index = 0;

    size = super->iblock_size * super->iblock_num;
    cafe->iblock = (INODE_BLOCK_V1_S *)g_malloc0(size);
    if (cafe->iblock == NULL) {
        debug_log(DLOG_ERROR, "%s:alloc new iblock failed", __FUNCTION__);
        g_free(cafe->journal);
        return RET_ERR;
    }

    (void)__e2p_read(cafe->frudev_handle, super->iblock_offset * 8, size, cafe->iblock);

    size = super->dblock_size * super->dblock_num;
    cafe->dblock = (struct data_block_v1 *)g_malloc0(size);
    if (cafe->dblock == NULL) {
        debug_log(DLOG_ERROR, "%s:alloc new dblock failed", __FUNCTION__);
        g_free(cafe->journal);
        g_free(cafe->iblock);
        return RET_ERR;
    }

    (void)__e2p_read(cafe->frudev_handle, super->dblock_offset * 8, size, cafe->dblock);

    __journal_replay_all(cafe);

    __iblocks_init(cafe);
    __dblocks_init(cafe);
    (void)__records_init(cafe);

    return RET_OK;
}

LOCAL void __e2p_cafe_record_format_date(EEPROM_CAFE_SP e2p_cafe)
{
    guchar cafe_key_dup[CAFE_KEY_MAX_LEN] = {0};

    
    errno_t ptr_ret =
        memcpy_s((void *)cafe_key_dup, sizeof(cafe_key_dup), CAFE_INDEX_FORMAT_DATE, strlen(CAFE_INDEX_FORMAT_DATE));
    if (ptr_ret != EOK) {
        debug_log(DLOG_ERROR, "%s failed: copy format data record key [%s] failed", __FUNCTION__,
            CAFE_INDEX_FORMAT_DATE);
        return;
    }

    CAFE_DATA_S cafe_key = { 0 };
    cafe_key.type = TYPE_8BIT_ASCII;
    cafe_key.len = (guint8)strlen(CAFE_INDEX_FORMAT_DATE);
    cafe_key.data = cafe_key_dup;

    
    struct tm t_tm = { 0 };
    gint32 ret = dal_localtime_r(NULL, &t_tm);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s failed: get local time failed, ret is %d", __FUNCTION__, ret);
        return;
    }

    guchar format_date_val[CAFE_FORMAT_DATA_VAL_LEN] = {0};
    format_date_val[0] = t_tm.tm_year % 100; 
    format_date_val[1] = t_tm.tm_mon;
    format_date_val[2] = t_tm.tm_mday;

    CAFE_DATA_S cafe_val = { 0 };
    cafe_val.type = TYPE_8BIT_ASCII;
    cafe_val.len = CAFE_FORMAT_DATA_VAL_LEN;
    cafe_val.data = format_date_val;

    ret = __e2p_cafe_write_record_by_key(e2p_cafe, &cafe_key, &cafe_val);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: write cafe format date record failed, ret is %d", __FUNCTION__, ret);
    }

    return;
}

LOCAL gint32 __e2p_cafe_v1_format(EEPROM_CAFE_SP e2p_cafe, guint32 offset, guint32 size)
{
    if (size < E2P_CAFE_MIN_SIZE) {
        return RET_ERR;
    }

    guint8 *buf = (guchar *)g_malloc0(size);
    if (buf == NULL) {
        debug_log(DLOG_ERROR, "%s:alloc new buf failed", __FUNCTION__);
        return RET_ERR;
    }

    SUPER_BLOCK_V1_S *super = (SUPER_BLOCK_V1_S *)buf;
    errno_t safe_fun_ret = memset_s(buf, size, 0x00, SUPER_BLOCK_SIZE);
    if (safe_fun_ret != EOK) {
        g_free(buf);
        debug_log(DLOG_ERROR, "%s: memset_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }

    (void)memset_s(buf + SUPER_BLOCK_SIZE, size - SUPER_BLOCK_SIZE, 0xFF, size - SUPER_BLOCK_SIZE);
    super->header.magic = htons(0xCAFE);
    super->header.version = 1;

    guint32 prev_end = SUPER_BLOCK_SIZE;
    super->journal_offset = (offset + prev_end + OFFSET_MULTIPLES - 1) / OFFSET_MULTIPLES;
    super->journal_size = (guint8)sizeof(JOURNAL_V1_S);
    super->journal_num = 4;

    prev_end = super->journal_offset * OFFSET_MULTIPLES + super->journal_size * super->journal_num;
    super->iblock_offset = (offset + prev_end + OFFSET_MULTIPLES - 1) / OFFSET_MULTIPLES;
    super->iblock_size = (guint8)sizeof(INODE_BLOCK_V1_S);

    debug_log(DLOG_INFO, "index block size %d", super->iblock_size);

    super->iblock_num = ((size - super->iblock_offset * OFFSET_MULTIPLES) / 3) /
        super->iblock_size; 

    prev_end = super->iblock_offset * OFFSET_MULTIPLES + super->iblock_size * super->iblock_num;
    super->dblock_offset = (offset + prev_end + OFFSET_MULTIPLES - 1) / OFFSET_MULTIPLES;
    super->dblock_size = (guint8)sizeof(struct data_block_v1);
    super->dblock_num = (size - super->dblock_offset * OFFSET_MULTIPLES) / super->dblock_size;

    gint32 metadata_len = sizeof(SUPER_BLOCK_V1_S) - offsetof(SUPER_BLOCK_V1_S, journal_offset);
    super->header.checksum = __make_checksum(&super->journal_offset, metadata_len);

    debug_log(DLOG_INFO, "index num:%d, data block num:%d, write size:%d", super->iblock_num, super->dblock_num, size);

    gint32 ret = __e2p_write(e2p_cafe->frudev_handle, offset, size, buf);

    g_free(buf);

    return ret;
}

LOCAL gint32 __e2p_cafe_v1_init(guint32 offset, guint32 size, EEPROM_CAFE_S *e2p_cafe)
{
    SUPER_BLOCK_V1_S *super = (SUPER_BLOCK_V1_S *)g_malloc0(sizeof(SUPER_BLOCK_V1_S));
    if (super == NULL) {
        debug_log(DLOG_ERROR, "%s:alloc new super block failed", __FUNCTION__);
        return RET_ERR;
    }

    gchar cafe_description_info[CAFE_DESC_MAX_LEN] = {0};
    (void)dal_get_property_value_string(e2p_cafe->frudev_handle, PROPERTY_CAFEDEV_CAFE_DESC, cafe_description_info,
        sizeof(cafe_description_info));

    (void)__e2p_read(e2p_cafe->frudev_handle, offset, sizeof(SUPER_BLOCK_V1_S), super);
    gint32 metadata_len = sizeof(SUPER_BLOCK_V1_S) - sizeof(struct common_header);
    
    guint8 chk_result = __make_checksum(&super->journal_offset, metadata_len);
    debug_log(DLOG_INFO, "%s:super block cafe data checksum is [%d], chk result is %d", __FUNCTION__,
        super->header.checksum, chk_result);
    if (super->header.checksum != chk_result) {
        e2p_cafe->format_flag = 1;
        debug_log(DLOG_ERROR, "%s: Super block header check failed, current header val:[%#x], expect val:[%#x]",
            __FUNCTION__, chk_result, super->header.checksum);
        maintenance_log_v2(MLOG_ERROR, FC_BMC_CAFE_ERR, "Super block header check failed, format cafe eeprom (%s).",
            cafe_description_info);

        (void)__e2p_cafe_v1_format(e2p_cafe, offset, size);
        (void)__e2p_read(e2p_cafe->frudev_handle, offset, sizeof(SUPER_BLOCK_V1_S), super);
    }

    e2p_cafe->super = super;
    e2p_cafe->offset = offset;

    (void)__schema_init(e2p_cafe);

    debug_log(DLOG_INFO,
        "cafe init successfully, current record size [%u], data free size [%u], date used size [%u], index free size "
        "[%d], index used size [%u]",
        g_hash_table_size(e2p_cafe->records), g_list_length(e2p_cafe->data_free), g_list_length(e2p_cafe->data_used),
        g_list_length(e2p_cafe->inode_free), g_list_length(e2p_cafe->inode_used));

    return RET_OK;
}

LOCAL gint32 __key_write(EEPROM_CAFE_S *e2p_cafe, CAFE_DATA_S *key, struct data_block_v1 **dblocks, guint8 dblock_num,
    INODE_BLOCK_V1_S **iblock)
{
    INODE_BLOCK_V1_S *inode = NULL;
    if (key->len > sizeof(inode->key) || __iblock_reserve(e2p_cafe, &inode) < 0) {
        return RET_ERR;
    }

    (void)memset_s(inode, sizeof(*inode), 0, sizeof(*inode));
    inode->len = key->len;
    errno_t ret = memcpy_s(inode->key, sizeof(inode->key), key->data, key->len);
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s failed, ret = %d", __FUNCTION__, ret);
    }

    inode->size = dblock_num;
    for (guint8 i = 0; i < dblock_num; i++) {
        inode->data[i] = dblocks[i] - e2p_cafe->dblock;
    }
    inode->checksum =
        __make_checksum(&inode->len, offsetof(INODE_BLOCK_V1_S, checksum) - offsetof(INODE_BLOCK_V1_S, len));
    inode->flag = INODE_INVALID;

    gint32 offset =
        e2p_cafe->super->iblock_offset * OFFSET_MULTIPLES + (inode - e2p_cafe->iblock) * sizeof(INODE_BLOCK_V1_S);

    debug_log(DLOG_INFO, "begin to write key, valid flag:[%#x], key len:[%d], key:[%s], data size:[%d]", inode->flag,
        inode->len, key->data, inode->size);
    __e2p_write(e2p_cafe->frudev_handle, offset, sizeof(*inode), inode);
    *iblock = inode;

    return RET_OK;
}

LOCAL gint32 __value_write(EEPROM_CAFE_S *e2p_cafe, CAFE_DATA_S *value, struct data_block_v1 **dblocks,
    guint8 *block_num)
{
    guint8 buffer[ARRAY_SIZE(((INODE_BLOCK_V1_S *)0)->data) * sizeof(struct data_block_v1)] = {0};

    
    guint8 total_len = value->len + 2;
    if (total_len > sizeof(buffer)) {
        return CAFE_ERR_DATA_SIZE;
    }

    *block_num = (guint8)((total_len + sizeof(struct data_block_v1) - 1) / sizeof(struct data_block_v1));
    if (__dblocks_reserve(e2p_cafe, dblocks, *block_num) != RET_OK) {
        debug_log(DLOG_ERROR, "reserve data block failed");
        return CAFE_ERR_RESERVE_FAILED;
    }

    guint8 *data = buffer;
    *data++ = ((value->type & 0x03) << 6) | (value->len & 0x3F);
    errno_t safe_fun_ret = memcpy_s(data, sizeof(buffer) - 1, value->data, value->len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }
    data += value->len;
    *data = __make_checksum(value->data, value->len);
    data = buffer;
    for (guint8 i = 0; i < *block_num; i++) {
        guint8 size = (guint8)sizeof(((struct data_block_v1 *)0)->data);
        gint32 offset = e2p_cafe->super->dblock_offset * OFFSET_MULTIPLES +
            (dblocks[i] - e2p_cafe->dblock) * sizeof(struct data_block_v1);

        size = size > total_len ? total_len : size;
        (void)memset_s(dblocks[i]->data, sizeof(dblocks[i]->data), 0, size); 
        safe_fun_ret = memcpy_s(dblocks[i]->data, sizeof(dblocks[i]->data), data, size);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        }
        __e2p_write(e2p_cafe->frudev_handle, offset, sizeof(dblocks[i]->data), dblocks[i]->data);
        data += size;
        total_len -= size;
    }

    return RET_OK;
}

LOCAL void __e2p_cafe_record_del_internal(EEPROM_CAFE_S *e2p_cafe, CAFE_DATA_S *key)
{
    CAFE_RECORD_S *record = __record_get(e2p_cafe, key);
    if (record == NULL) {
        return;
    }

    JOURNAL_V1_S *journal = __journal_alloc_0(e2p_cafe);
    (void)__journal_transaction_append(journal, record->inode, INODE_DEL);
    __journal_write(e2p_cafe, journal);
    __journal_replay(e2p_cafe, journal);

    INODE_BLOCK_V1_S *iblock = e2p_cafe->iblock + record->inode;
    __iblock_release(e2p_cafe, iblock);
    __dblocks_release_by_ids(e2p_cafe, iblock->data, iblock->size);

    return;
}

LOCAL gboolean __elable_record_foreach_remove_func(gpointer key, gpointer value, gpointer user_data)
{
    CAFE_RECORD_S *record_data = (CAFE_RECORD_S *)value;
    if (record_data == NULL || record_data->key == NULL) {
        return FALSE;
    }

    CAFE_KEY_TO_FRU_LOC_INFO_S *cafe_loc_info = NULL;
    gint32 ret = __e2p_cafe_get_fru_loc_info_by_cafe_key((const gchar *)key, &cafe_loc_info);
    if (ret != RET_OK || cafe_loc_info->fru_loc_area == FRU_AREA_INVALID ||
        cafe_loc_info->fru_loc_filed == FRU_FILED_INVALID) {
        return FALSE;
    }

    EEPROM_CAFE_S *e2p_cafe = (EEPROM_CAFE_S *)user_data;
    __e2p_cafe_record_del_internal(e2p_cafe, record_data->key);

    return TRUE;
}

LOCAL void *__cafe_clear_elabel_task(void *data)
{
    (void)prctl(PR_SET_NAME, (unsigned long)"CafeClearElabel");

    EEPROM_CAFE_S *e2p_cafe = (EEPROM_CAFE_S *)data;
    g_hash_table_foreach_remove(e2p_cafe->records, __elable_record_foreach_remove_func, (gpointer)e2p_cafe);

    return NULL;
}

LOCAL void __print_single_cafe_record_info(const gchar *cafe_key, CAFE_RECORD_S *record, gchar *write_buf,
    guint32 max_write_buf_len)
{
    if (cafe_key == NULL || record == NULL || record->value->data == NULL || record->value->len == 0) {
        return;
    }

    guint32 cur_buf_size = strlen(write_buf);
    guint32 remained_buf_size = max_write_buf_len - cur_buf_size;
    gint32 ret;
    ret = snprintf_s(write_buf + cur_buf_size, remained_buf_size, remained_buf_size - 1,
        "\r record key: %s, record val len: %d, record val: ", cafe_key, record->value->len);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }

    guint32 i = 0;
    while (i < record->value->len) {
        cur_buf_size = strlen(write_buf);
        if (cur_buf_size >= max_write_buf_len - 1) { // remained_buf_size需要大于1
            break;
        }
        remained_buf_size = max_write_buf_len - cur_buf_size;

        ret = snprintf_s(write_buf + cur_buf_size, remained_buf_size, remained_buf_size - 1, " %#x",
            record->value->data[i]);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        }
        i++;
    }

    cur_buf_size = strlen(write_buf);
    remained_buf_size = max_write_buf_len - cur_buf_size;
    if (remained_buf_size > 1) {
        ret = snprintf_s(write_buf + cur_buf_size, remained_buf_size, remained_buf_size - 1, "\n");
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        }
    }

    return;
}

LOCAL void __e2p_cafe_record_foreach(EEPROM_CAFE_SP e2p_cafe, e2p_cafe_record_foreach_func func, gpointer user_data)
{
    if (e2p_cafe == NULL || e2p_cafe->records == NULL || func == NULL) {
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__);
        return;
    }

    GHashTableIter hash_iter;
    gchar *iter_key = NULL;
    CAFE_RECORD_S *record_data = NULL;
    g_hash_table_iter_init(&hash_iter, e2p_cafe->records);
    while (g_hash_table_iter_next(&hash_iter, (gpointer *)&iter_key, (gpointer *)&record_data) != FALSE) {
        func(iter_key, record_data, user_data);
    }

    return;
}

LOCAL gint32 __e2p_cafe_init(guchar cafe_ver, guchar chk_fail_action, guint32 offset, guint32 size,
    EEPROM_CAFE_S *e2p_cafe)
{
    gchar cafe_description_info[CAFE_DESC_MAX_LEN] = {0};
    (void)dal_get_property_value_string(e2p_cafe->frudev_handle, PROPERTY_CAFEDEV_CAFE_DESC, cafe_description_info,
        sizeof(cafe_description_info));
    struct common_header header = { 0 };
    gint32 ret = __e2p_read(e2p_cafe->frudev_handle, offset, sizeof(struct common_header), &header);
    if (ret != RET_OK) {
        return ret;
    }

    if (ntohs(header.magic) != 0xCAFE) {
        if (chk_fail_action != CAFE_INIT_ACTION_FORMAT) {
            return CAFE_ERR_CAFE_VER;
        }

        e2p_cafe->format_flag = 1;
        debug_log(DLOG_ERROR, "%s: Super block magic number check failed, current val:[%#x], expect val:0xcafe",
            __FUNCTION__, ntohs(header.magic));
        maintenance_log_v2(MLOG_ERROR, FC_BMC_CAFE_ERR,
            "Super block magic number check failed, format cafe eeprom (%s).", cafe_description_info);

        (void)__e2p_cafe_v1_format(e2p_cafe, offset, size);
        (void)__e2p_read(e2p_cafe->frudev_handle, offset, sizeof(struct common_header), &header);
    }

    if (size < E2P_CAFE_MIN_SIZE) {
        return CAFE_ERR_BLK_SIZE;
    }

    if (header.version == 0) {
        header.version = cafe_ver;
    }

    switch (header.version) {
        case 1:
            ret = __e2p_cafe_v1_init(offset, size, e2p_cafe);
            break;
        default:
            ret = CAFE_ERR_CAFE_VER;
            debug_log(DLOG_ERROR, "unsupport cafe version:%d", header.version);
            break;
    }

    
    if (e2p_cafe->format_flag == 1) {
        __e2p_cafe_record_format_date(e2p_cafe);
    }

    return ret;
}

LOCAL gint32 __e2p_cafe_write_record_by_key(EEPROM_CAFE_S *e2p_cafe, CAFE_DATA_S *key, CAFE_DATA_S *value)
{
    guint8 dblock_num = 0;
    INODE_BLOCK_V1_S *iblock = NULL;
    struct data_block_v1 *dblocks[ARRAY_SIZE(((INODE_BLOCK_V1_S *)0)->data)] = {NULL};
    if (e2p_cafe == NULL || __value_write(e2p_cafe, value, dblocks, &dblock_num) != RET_OK ||
        __key_write(e2p_cafe, key, dblocks, dblock_num, &iblock) != RET_OK) {
        return CAFE_ERR_RESERVE_FAILED;
    }

    guint8 inode = iblock - e2p_cafe->iblock;
    JOURNAL_V1_S *journal = __journal_alloc_0(e2p_cafe);
    (void)__journal_transaction_append(journal, inode, INODE_ADD);
    CAFE_RECORD_S *record = __record_get(e2p_cafe, key);

    if (record) {
        (void)__journal_transaction_append(journal, record->inode, INODE_DEL);
    }
    __journal_write(e2p_cafe, journal);
    __journal_replay(e2p_cafe, journal);

    __iblock_occupy(e2p_cafe, iblock);
    __dblocks_occupy(e2p_cafe, dblocks, dblock_num);
    if (record) {
        iblock = e2p_cafe->iblock + record->inode;
        __iblock_release(e2p_cafe, iblock);
        __dblocks_release_by_ids(e2p_cafe, iblock->data, iblock->size);
        __record_remove(e2p_cafe, key);
    }

    (void)__record_add(e2p_cafe, key, value, inode);

    return RET_OK;
}

LOCAL gint32 __e2p_cafe_get_fru_loc_info_by_cafe_key(const gchar *cafe_key, CAFE_KEY_TO_FRU_LOC_INFO_S **fru_loc_info)
{
    guint32 len = G_N_ELEMENTS(g_cafe_key_to_fru_loc_info);
    for (guint32 i = 0; i < len; i++) {
        if (g_strcmp0(cafe_key, g_cafe_key_to_fru_loc_info[i].cafe_key_str) == 0) {
            *fru_loc_info = &g_cafe_key_to_fru_loc_info[i];
            return RET_OK;
        }
    }

    return RET_ERR;
}

LOCAL gint32 __e2p_cafe_get_fru_loc_info_by_area_filed(gint32 area, gint32 filed,
    CAFE_KEY_TO_FRU_LOC_INFO_S **fru_loc_info)
{
    guint32 len = G_N_ELEMENTS(g_cafe_key_to_fru_loc_info);
    for (guint32 i = 0; i < len; i++) {
        if ((g_cafe_key_to_fru_loc_info[i].fru_loc_area == area) &&
            (g_cafe_key_to_fru_loc_info[i].fru_loc_filed == filed)) {
            *fru_loc_info = &g_cafe_key_to_fru_loc_info[i];

            return RET_OK;
        }
    }

    return RET_ERR;
}

LOCAL void __e2p_cafe_init_frudev_handle(EEPROM_CAFE_SP e2p_cafe, OBJ_HANDLE frudev_handle)
{
    if (e2p_cafe == NULL || frudev_handle == 0) {
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__);
        return;
    }

    e2p_cafe->frudev_handle = frudev_handle;

    return;
}

LOCAL gint32 __e2p_cafe_type_check(EEPROM_CAFE_SP e2p_cafe)
{
    return (e2p_cafe->frudev_handle != 0) ? RET_OK : RET_ERR;
}

LOCAL gint32 __update_fru_board_mfgdata(CAFE_DATA_S *cafe_data, FRU_BOARD_AREA_S *fru_board_area_info)
{
    OBJ_HANDLE obj_handle = 0;
    guint8 elabel_format = 0;
    (void)dfl_get_object_handle(OBJECT_BMC, &obj_handle);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_BMC_ELABELMANUFORMAT, &elabel_format);
    if (elabel_format == 1) {
        errno_t safe_fun_ret = memmove_s((void *)fru_board_area_info->mfg_date_time,
            sizeof(fru_board_area_info->mfg_date_time), (const void *)cafe_data->data, cafe_data->len);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            return RET_ERR;
        }
    } else {
        errno_t safe_fun_ret = memmove_s((void *)fru_board_area_info->mfg_date_time,
            sizeof(fru_board_area_info->mfg_date_time) - 1, (const void *)cafe_data->data,
            sizeof(fru_board_area_info->mfg_date_time) - 1);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            return RET_ERR;
        }
    }
    return RET_OK;
}

LOCAL gint32 __update_fru_board_product_name(CAFE_DATA_S *cafe_data, FRU_BOARD_AREA_S *fru_board_area_info)
{
    fru_board_area_info->board_product_name.len = cafe_data->len;
    fru_board_area_info->board_product_name.type = CHAR_TYPE;
    errno_t safe_fun_ret = memmove_s((void *)fru_board_area_info->board_product_name.content, FRU_MAX_ELABEL_LEN,
        cafe_data->data, cafe_data->len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }
    return RET_OK;
}

LOCAL gint32 __update_fru_board_serial_number(CAFE_DATA_S *cafe_data, FRU_BOARD_AREA_S *fru_board_area_info)
{
    fru_board_area_info->board_serial_num.len = cafe_data->len;
    fru_board_area_info->board_serial_num.type = CHAR_TYPE;
    errno_t safe_fun_ret = memmove_s((void *)fru_board_area_info->board_serial_num.content, FRU_MAX_ELABEL_LEN,
        cafe_data->data, cafe_data->len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }
    return RET_OK;
}

LOCAL gint32 __update_fru_board_part_number(CAFE_DATA_S *cafe_data, FRU_BOARD_AREA_S *fru_board_area_info)
{
    fru_board_area_info->board_part_num.len = cafe_data->len;
    fru_board_area_info->board_part_num.type = CHAR_TYPE;
    errno_t safe_fun_ret = memmove_s((void *)fru_board_area_info->board_part_num.content, FRU_MAX_ELABEL_LEN,
        cafe_data->data, cafe_data->len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 __update_fru_board_data_from_cafe(CAFE_DATA_S *cafe_data, gint32 filed_index,
    FRU_BOARD_AREA_S *fru_board_area_info)
{
    size_t count = sizeof(g_update_fru_board_data) / sizeof(UPDATE_FRU_BOARD_DATA);
    for (size_t i = 0; i < count; i++) {
        if (g_update_fru_board_data[i].filed_index == filed_index) {
            return g_update_fru_board_data[i].__update_fru_board_data(cafe_data, fru_board_area_info);
        }
    }

    debug_log(DLOG_ERROR, "%s failed, unknown fru board filed index:%d", __FUNCTION__, filed_index);
    return RET_ERR;
}


LOCAL gint32 __update_fru_chassis_info_with_cafe(CAFE_DATA_S *cafe_data, gint32 filed_index,
    FRU_CHASSIS_AREA_S *fru_chassis_area_info)
{
    gint32 ret = RET_OK;
    errno_t safe_fun_ret;

    switch (filed_index) {
        case FRU_CHASSIS_TYPE:
            fru_chassis_area_info->chassis_type = cafe_data->data[0];
            break;

        case FRU_CHASSIS_PART_NUMBER:
            fru_chassis_area_info->chassis_part_num.len = cafe_data->len;
            fru_chassis_area_info->chassis_part_num.type = CHAR_TYPE;
            safe_fun_ret = memmove_s((void *)fru_chassis_area_info->chassis_part_num.content, FRU_MAX_ELABEL_LEN,
                cafe_data->data, cafe_data->len);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
                return RET_ERR;
            }
            break;

        case FRU_CHASSIS_SERIAL_NUMBER:
            fru_chassis_area_info->chassis_serial_num.len = cafe_data->len;
            fru_chassis_area_info->chassis_serial_num.type = CHAR_TYPE;
            safe_fun_ret = memmove_s((void *)fru_chassis_area_info->chassis_serial_num.content, FRU_MAX_ELABEL_LEN,
                cafe_data->data, cafe_data->len);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
                return RET_ERR;
            }
            break;

        default:
            ret = RET_ERR;
            debug_log(DLOG_ERROR, "%s failed, unknown chassis filed index:%d", __FUNCTION__, filed_index);
            break;
    }

    return ret;
}

LOCAL gint32 __update_fru_product_name(CAFE_DATA_S *cafe_data, FRU_PRODUCT_AREA_S *fru_product_area_info)
{
    fru_product_area_info->product_name.len = cafe_data->len;
    fru_product_area_info->product_name.type = CHAR_TYPE;
    errno_t safe_fun_ret = memmove_s((void *)fru_product_area_info->product_name.content, FRU_MAX_ELABEL_LEN,
        cafe_data->data, cafe_data->len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }
    return RET_OK;
}

LOCAL gint32 __update_fru_product_part_number(CAFE_DATA_S *cafe_data, FRU_PRODUCT_AREA_S *fru_product_area_info)
{
    fru_product_area_info->product_part_num.len = cafe_data->len;
    fru_product_area_info->product_part_num.type = CHAR_TYPE;
    errno_t safe_fun_ret = memmove_s((void *)fru_product_area_info->product_part_num.content, FRU_MAX_ELABEL_LEN,
        cafe_data->data, cafe_data->len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }
    return RET_OK;
}

LOCAL gint32 __update_fru_product_serial_number(CAFE_DATA_S *cafe_data, FRU_PRODUCT_AREA_S *fru_product_area_info)
{
    fru_product_area_info->product_serial_num.len = cafe_data->len;
    fru_product_area_info->product_serial_num.type = CHAR_TYPE;
    errno_t safe_fun_ret = memmove_s((void *)fru_product_area_info->product_serial_num.content, FRU_MAX_ELABEL_LEN,
        cafe_data->data, cafe_data->len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }
    return RET_OK;
}

LOCAL gint32 __update_fru_product_assettag(CAFE_DATA_S *cafe_data, FRU_PRODUCT_AREA_S *fru_product_area_info)
{
    fru_product_area_info->product_asset_tag.len = cafe_data->len;
    fru_product_area_info->product_asset_tag.type = CHAR_TYPE;
    errno_t safe_fun_ret = memmove_s((void *)fru_product_area_info->product_asset_tag.content, FRU_MAX_ELABEL_LEN,
        cafe_data->data, cafe_data->len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 __update_fru_product_data_from_cafe(CAFE_DATA_S *cafe_data, gint32 filed_index,
    FRU_PRODUCT_AREA_S *fru_product_area_info)
{
    size_t count = sizeof(g_update_fru_product_data) / sizeof(UPDATE_FRU_PRODUCT_DATA);
    for (size_t i = 0; i < count; i++) {
        if (g_update_fru_product_data[i].filed_index == filed_index) {
            return g_update_fru_product_data[i].__update_fru_product_data(cafe_data, fru_product_area_info);
        }
    }

    debug_log(DLOG_ERROR, "%s failed:unknown product info filed:%d", __FUNCTION__, filed_index);
    return RET_ERR;
}


LOCAL gint32 __update_fru_info_with_cafe(CAFE_DATA_S *cafe_data, gint32 area_index, gint32 filed_index,
    FRU_INFO_S *fru_info)
{
    gint32 ret;

    switch (area_index) {
        case FRU_AREA_BOARDINFO:
            fru_info->has_board = TRUE;
            ret = __update_fru_board_data_from_cafe(cafe_data, filed_index, &fru_info->board);
            break;

        case FRU_AREA_CHASSISINFO:
            fru_info->has_chassis = TRUE;
            ret = __update_fru_chassis_info_with_cafe(cafe_data, filed_index, &fru_info->chassis);
            break;

        case FRU_AREA_PRODUCTINFO:
            fru_info->has_product = TRUE;
            ret = __update_fru_product_data_from_cafe(cafe_data, filed_index, &fru_info->product);
            break;

        default:
            ret = RET_ERR;
            debug_log(DLOG_ERROR, "unknown fru area [%d]", area_index);
            break;
    }

    return ret;
}


LOCAL void __init_fru_info_with_cafe_record(const gchar *record_key, CAFE_RECORD_S *record_data, gpointer user_data)
{
    if ((user_data == NULL) || (record_key == NULL) || (record_data == NULL) || (record_data->value == NULL) ||
        (record_data->value->len == 0)) {
        debug_log(DLOG_ERROR, "invalid cafe hash table key or cafe data");
        return;
    }

    
    CAFE_KEY_TO_FRU_LOC_INFO_S *fru_loc_info = NULL;
    gint32 ret = __e2p_cafe_get_fru_loc_info_by_cafe_key(record_key, &fru_loc_info);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get fru loc info for cafe key [%s] failed", record_key);
        return;
    }

    debug_log(DLOG_INFO, "begin to update fru data with [%s:%s], fru loc[%d:%d]", record_key, record_data->value->data,
        fru_loc_info->fru_loc_area, fru_loc_info->fru_loc_filed);

    FRU_INFO_S *fru_info = (FRU_INFO_S *)user_data;
    ret = __update_fru_info_with_cafe(record_data->value, fru_loc_info->fru_loc_area, fru_loc_info->fru_loc_filed,
        fru_info);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "update fru info for area [%d] filed [%d] with cafe key [%s] failed, ret is %d ",
            fru_loc_info->fru_loc_area, fru_loc_info->fru_loc_filed, record_key, ret);
        return;
    }

    return;
}


LOCAL void __update_enc_location_info_from_cafe(OBJ_HANDLE fru_handle, EEPROM_CAFE_SP cafe_info)
{
    guchar comp_device_type = 0;
    gint32 ret = dal_get_property_value_byte(fru_handle, PROPERTY_FRU_TYPE, &comp_device_type);
    if ((ret != RET_OK) || (comp_device_type != COMPONENT_TYPE_BACKPLANE_BOARD)) {
        debug_log(DLOG_ERROR, "get property value from %s failed, ret is %d", PROPERTY_FRU_TYPE, ret);
        return;
    }

    gchar cafe_key[CAFE_KEY_MAX_LEN + 1] = {0};
    (void)strncpy_s(cafe_key, CAFE_KEY_MAX_LEN + 1, CAFE_INDEX_ENCLOC, strlen(CAFE_INDEX_ENCLOC));

    CAFE_DATA_S enc_loc_key = { 0 };
    enc_loc_key.data = (guchar *)cafe_key;
    enc_loc_key.len = (guint8)strlen(cafe_key);

    
    CAFE_DATA_S *enc_loc_val = e2p_cafe_get_record_by_key(cafe_info, &enc_loc_key);
    if (enc_loc_val == NULL) {
        return;
    }

    gchar cafe_val[FRU_MAX_ELABEL_LEN + 1] = {0};
    errno_t str_ret = strncpy_s(cafe_val, sizeof(cafe_val), (const gchar *)enc_loc_val->data, enc_loc_val->len);
    if (str_ret != EOK) {
        debug_log(DLOG_ERROR, "%s:copy location string failed", __FUNCTION__);
        return;
    }

    OBJ_HANDLE obj_handle = 0;
    ret = dal_get_object_handle_nth(OBJ_NAME_BMC, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "cann't get object handle for class [%s], ret is %d", OBJ_NAME_BMC, ret);
        return;
    }

    ret = dal_set_property_value_string(obj_handle, PROPERTY_BMC_LOCATION_INFO, cafe_val, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR,
            "set property value [%s] for property [%s] of handle [%" OBJ_HANDLE_FORMAT "] failed, ret is %d", cafe_val,
            PROPERTY_BMC_LOCATION_INFO, obj_handle, ret);
    }

    return;
}

LOCAL gint32 __validattion_check_e2p_cafe_record(OBJ_HANDLE fru_handle, const gchar *cafe_key_str,
    CAFE_DATA_S *cafe_data, const gchar *fru_handle_name)
{
    if (fru_handle == 0 || cafe_key_str == NULL || cafe_data == NULL || cafe_data->len > FRU_MAX_ELABEL_LEN) {
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__);
        return RET_ERR;
    }

    guint32 key_str_len = strlen(cafe_key_str);
    if (key_str_len > CAFE_KEY_MAX_LEN) {
        debug_log(DLOG_ERROR, "%s failed: cafe key exceeded, current length is %d", __FUNCTION__, key_str_len);
        return RET_ERR;
    }

    if (fru_handle_name == NULL) {
        debug_log(DLOG_ERROR, "get object name for %" OBJ_HANDLE_FORMAT " failed", fru_handle);
        return RET_ERR;
    }
    return RET_OK;
}


gint32 init_fru_info_from_cafe_info(EEPROM_CAFE_SP cafe_info, FRU_INFO_S *fru_info)
{
    __e2p_cafe_record_foreach(cafe_info, __init_fru_info_with_cafe_record, (gpointer)fru_info);

    return RET_OK;
}

gint32 e2p_cafe_update_to_e2p(gint32 area, gint32 filed, EEPROM_CAFE_SP cafe_info, const void *data_buf,
    guint32 buf_len)
{
    
    if (__e2p_cafe_type_check(cafe_info) != RET_OK) {
        return COMP_CODE_SUCCESS;
    }

    CAFE_KEY_TO_FRU_LOC_INFO_S *fru_loc_info = NULL;
    gint32 ret = __e2p_cafe_get_fru_loc_info_by_area_filed(area, filed, &fru_loc_info);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "find fru location info with area [%d] filed [%d] failed", area, filed);
        return COMP_CODE_OUTOF_RANGE;
    }

    CAFE_DATA_S cafe_key = { 0 };
    cafe_key.data = (guchar *)g_strdup(fru_loc_info->cafe_key_str);
    if (cafe_key.data == NULL) {
        debug_log(DLOG_ERROR, "dup string for %s failed", fru_loc_info->cafe_key_str);
        return COMP_CODE_OUTOF_RANGE;
    }

    gchar val_buf[FRU_MAX_ELABEL_LEN + 1] = {0};
    errno_t safe_fun_ret = memmove_s(val_buf, FRU_MAX_ELABEL_LEN + 1, data_buf, buf_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }

    cafe_key.len = (guint8)strlen(fru_loc_info->cafe_key_str);

    CAFE_DATA_S cafe_val = { 0 };
    cafe_val.data = (guint8 *)val_buf;
    cafe_val.len = buf_len;
    cafe_val.type = TYPE_8BIT_ASCII;

    debug_log(DLOG_INFO, "begin to write cafe eeprom, area [%d], filed [%d], cafe key [%s]", area, filed,
        fru_loc_info->cafe_key_str);

    ret = __e2p_cafe_write_record_by_key(cafe_info, &cafe_key, &cafe_val);
    g_free(cafe_key.data);

    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "write cafe info for %s failed, ret is %#x", fru_loc_info->cafe_key_str, ret);
        return COMP_CODE_OUTOF_RANGE;
    }

    return COMP_CODE_SUCCESS;
}

EEPROM_CAFE_SP e2p_cafe_new(void)
{
    return (EEPROM_CAFE_SP)g_malloc0(sizeof(EEPROM_CAFE_S));
}

gint32 e2p_cafe_free(EEPROM_CAFE_SP e2p_cafe)
{
    if (e2p_cafe == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return RET_ERR;
    }

    
    if (e2p_cafe->super) {
        g_free(e2p_cafe->super);
        e2p_cafe->super = NULL;
    }
    
    if (e2p_cafe->journal) {
        g_free(e2p_cafe->journal);
        e2p_cafe->journal = NULL;
    }
    
    if (e2p_cafe->iblock) {
        g_free(e2p_cafe->iblock);
        e2p_cafe->iblock = NULL;
    }

    
    if (e2p_cafe->inode_free) {
        g_list_free(e2p_cafe->inode_free);
        e2p_cafe->inode_free = NULL;
    }
    if (e2p_cafe->inode_used) {
        g_list_free(e2p_cafe->inode_used);
        e2p_cafe->inode_used = NULL;
    }

    
    if (e2p_cafe->dblock) {
        g_free(e2p_cafe->dblock);
        e2p_cafe->dblock = NULL;
    }

    
    if (e2p_cafe->data_free) {
        g_list_free(e2p_cafe->data_free);
        e2p_cafe->data_free = NULL;
    }
    if (e2p_cafe->data_used) {
        g_list_free(e2p_cafe->data_used);
        e2p_cafe->data_used = NULL;
    }

    if (e2p_cafe->records) {
        g_hash_table_destroy(e2p_cafe->records);
        e2p_cafe->records = NULL;
    }

    g_free(e2p_cafe);

    return RET_OK;
}

gint32 e2p_cafe_clear_all_record(EEPROM_CAFE_S *e2p_cafe)
{
    if (e2p_cafe == NULL || e2p_cafe->records == NULL) {
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__);
        return RET_ERR;
    }

    gulong task_id = 0;
    gint32 ret =
        vos_task_create(&task_id, "cafe clear task", (TASK_ENTRY)__cafe_clear_elabel_task, e2p_cafe, DEFAULT_PRIORITY);

    return ret;
}

CAFE_DATA_S *e2p_cafe_get_record_by_key(EEPROM_CAFE_S *e2p_cafe, CAFE_DATA_S *key)
{
    if (key == NULL || key->data == NULL) {
        return NULL;
    }

    CAFE_RECORD_S *record = __record_get(e2p_cafe, key);
    if (record == NULL) {
        return NULL;
    }

    return record->value;
}

gint32 e2p_cafe_dump_fru_info(OBJ_HANDLE fru_handle, gchar *write_buf, guint32 max_write_buf_len)
{
    if (write_buf == NULL) {
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__);
        return RET_ERR;
    }

    const gchar *fru_handle_name = dfl_get_object_name(fru_handle);
    if (fru_handle_name == NULL) {
        debug_log(DLOG_ERROR, "get object name for handle [%" OBJ_HANDLE_FORMAT "] failed", fru_handle);
        return RET_ERR;
    }

    FRU_PRIV_PROPERTY_S *priv_data = NULL;
    gint32 ret = get_obj_priv_data(fru_handle, (gpointer *)&priv_data);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "get binded data for fru handle [%s] failed, ret is %d", fru_handle_name, ret);
        return ret;
    }

    if ((priv_data->cafe_info->records == NULL) || (g_hash_table_size(priv_data->cafe_info->records) == 0)) {
        put_obj_priv_data(fru_handle, (gpointer)priv_data);
        return RET_OK;
    }

    guint32 cur_buf_size = strlen(write_buf);
    if (max_write_buf_len <= cur_buf_size) {
        debug_log(DLOG_ERROR, "%s failed: max_write_buf_len(%d) <= cur_buf_size(%d)", __FUNCTION__, max_write_buf_len,
            cur_buf_size);
        put_obj_priv_data(fru_handle, (gpointer)priv_data);
        return RET_ERR;
    }

    guint32 remained_buf_size = max_write_buf_len - cur_buf_size;
    (void)snprintf_s(write_buf + cur_buf_size, remained_buf_size, remained_buf_size - 1, "fru cafe info:\n");

    GHashTableIter iter;
    g_hash_table_iter_init(&iter, priv_data->cafe_info->records);
    gchar *cafe_key = NULL;
    CAFE_RECORD_S *cafe_record = NULL;
    while (g_hash_table_iter_next(&iter, (gpointer *)&cafe_key, (gpointer *)&cafe_record) != FALSE) {
        __print_single_cafe_record_info(cafe_key, cafe_record, write_buf, max_write_buf_len);
    }

    put_obj_priv_data(fru_handle, (gpointer)priv_data);
    cur_buf_size = strlen(write_buf);
    if (max_write_buf_len <= cur_buf_size) {
        debug_log(DLOG_ERROR, "%s failed: max_write_buf_len(%d) <= cur_buf_size(%d)", __FUNCTION__, max_write_buf_len,
            cur_buf_size);
        return RET_ERR;
    }
    remained_buf_size = max_write_buf_len - cur_buf_size;

    (void)snprintf_s(write_buf + cur_buf_size, remained_buf_size, remained_buf_size - 1, "\n");

    return RET_OK;
}

gint32 frudev_write_to_eeprom_custom(OBJ_HANDLE object_handle, guint32 offset, const gpointer write_buf, guint32 length)
{
    if (write_buf == NULL) {
        debug_log(DLOG_ERROR, "[%s] paramter error.", __FUNCTION__);
        return RET_ERR;
    }

    guint8 fan_slot = 0;
    (void)dal_get_property_value_byte(object_handle, PROPERTY_FRUDEV_STORAGE_LOC, &fan_slot);

    OBJ_HANDLE fan_object_handle = 0;
    gint32 ret = dal_get_specific_object_byte(CLASS_NAME_SMM_FAN, PROPERTY_FAN_SLOT, fan_slot, &fan_object_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get special object fail!", __FUNCTION__);
        return ret;
    }

    
    const guint8 *p_write_buff = (const guint8 *)write_buf;
    const FRU_FILE_S *fru_file = (const FRU_FILE_S *)write_buf;
    guint16 check_sum =
        arith_make_crc_checksum(0, p_write_buff + sizeof(FRU_FILE_HEADER_S), fru_file->header.fru_head.len);
    guchar fan_fru_header[FAN_FRU_HEAD_LEN] = { 0 }; 
    fan_fru_header[0] = check_sum >> 8;
    fan_fru_header[1] = check_sum & 0xff;
    
    fan_fru_header[2] = fru_file->header.fru_head.len >> 8;
    fan_fru_header[3] = fru_file->header.fru_head.len & 0xff;

    
    ret = (gint32)fru_block_write(fan_object_handle, PROPERTY_FAN_FRU_EEPROM, 0, sizeof(fan_fru_header),
        (gconstpointer)fan_fru_header);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "read fan%d eeprom failed!(ret:%d)", fan_slot, ret);
        return ret;
    }

    
    ret = (gint32)fru_block_write(fan_object_handle, PROPERTY_FAN_FRU_EEPROM, FAN_FRU_HEAD_LEN,
        fru_file->header.fru_head.len, (gconstpointer)fru_file->frudata);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "read fan%d eeprom failed!(ret:%d)", fan_slot, ret);
        return ret;
    }

    return ret;
}

LOCAL gint32 init_tc_header(FRU_PRIV_PROPERTY_S *fru_priv)
{
    if (fru_priv->eeprom_format != EEPROM_FORMAT_TIANCHI) {
        return RET_OK;
    }

    if (fru_priv->fru_property.frudev_handle == 0) {
        return RET_ERR;
    }

    guint32 ret =
        frudev_read_from_eeprom(fru_priv->fru_property.frudev_handle, 0, fru_priv->tc_header, sizeof(TC_FILE_HEADER_S));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "init tc_header failed, ret is %d", ret);
        return RET_ERR;
    }

    fru_priv->tc_header->fru_offset *= OFFSET_WORD_LENGTH;
    fru_priv->tc_header->system_desc_offset *= OFFSET_WORD_LENGTH;

    return RET_OK;
}

LOCAL void init_area_offsets(FRU_PRIV_PROPERTY_S *fru_priv)
{
    fru_priv->area_offsets->fru_offset =
        (fru_priv->eeprom_format == EEPROM_FORMAT_TIANCHI) ? fru_priv->tc_header->fru_offset : 0;
    fru_priv->area_offsets->system_desc_offset = (fru_priv->eeprom_format == EEPROM_FORMAT_TIANCHI) ?
        fru_priv->tc_header->system_desc_offset :
        E2P_SYSTEM_AREA_OFF;
    fru_priv->area_offsets->mezz_mac_offset = (fru_priv->eeprom_format == EEPROM_FORMAT_TIANCHI) ?
        (fru_priv->tc_header->system_desc_offset + SYS_DESC_LENGTH) :
        MEZZ_MAC_BLOCK_OFFSET;
    guint16 tc_canbus_dev_offset = fru_priv->tc_header->system_desc_offset + SYS_DESC_LENGTH +
        CANBUS_DEV_EEPROM_INFO_BASIC_OFFSET - MEZZ_MAC_BLOCK_OFFSET;
    fru_priv->area_offsets->canbus_dev_offset =
        (fru_priv->eeprom_format == EEPROM_FORMAT_TIANCHI) ? tc_canbus_dev_offset : CANBUS_DEV_EEPROM_INFO_BASIC_OFFSET;
}

gint32 init_tc_header_and_area_offsets(FRU_PRIV_PROPERTY_S *fru_priv)
{
    if (fru_priv == NULL) {
        debug_log(DLOG_ERROR, "fru_priv is null");
        return RET_ERR;
    }

    gint32 ret = init_tc_header(fru_priv);
    if (ret != RET_OK) {
        return ret;
    }

    init_area_offsets(fru_priv);
    return RET_OK;
}

gint32 init_cafe_info(FRU_PROPERTY_S *fru_prop, EEPROM_CAFE_SP cafe_info)
{
    if (fru_prop->frudev_handle == 0) {
        return RET_OK;
    }

    const gchar *fru_dev_obj_name = dfl_get_object_name(fru_prop->frudev_handle);
    if (fru_dev_obj_name == NULL) {
        debug_log(DLOG_ERROR, "%s:get fru dev object name failed", __FUNCTION__);
        return RET_ERR;
    }

    gchar fru_dev_class_name[MAX_NAME_SIZE] = {0};
    gint32 ret = dfl_get_class_name(fru_prop->frudev_handle, fru_dev_class_name, MAX_NAME_SIZE);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "get class name for object handle [%s] failed, ret is %d", fru_dev_obj_name, ret);
        return RET_ERR;
    }

    if (g_strcmp0(CLASS_CAFE_DEV, fru_dev_class_name) != 0) {
        return RET_ERR;
    }

    __e2p_cafe_init_frudev_handle(cafe_info, fru_prop->frudev_handle);
    fru_prop->eeprom_accessor_prop = PROPERTY_CAFEDEV_CAFE_ACCESSOR;

    
    guint32 super_block_offset = 0;
    (void)dal_get_property_value_uint32(fru_prop->frudev_handle, PROPERTY_CAFEDEV_SUPERBLOCK_OFFSET,
        &super_block_offset);

    guchar cafe_chk_fail_action = 0;
    (void)dal_get_property_value_byte(fru_prop->frudev_handle, PROPERTY_CAFEDEV_CHKFAILACTION, &cafe_chk_fail_action);

    guint32 cafe_blk_size = 0;
    (void)dal_get_property_value_uint32(fru_prop->frudev_handle, PROPERTY_CAFEDEV_CAFEBLK_SIZE, &cafe_blk_size);
    debug_log(DLOG_INFO, "super block offset :%u, cafe block size: %u", super_block_offset, cafe_blk_size);

    guchar cafe_ver = 0;
    (void)dal_get_property_value_byte(fru_prop->frudev_handle, PROPERTY_CAFEDEV_CAFEVER, &cafe_ver);

    
    ret = __e2p_cafe_init(cafe_ver, cafe_chk_fail_action, super_block_offset, cafe_blk_size, cafe_info);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "init cafe info failed, ret is %d", ret);
        return RET_ERR;
    }

    
    __update_enc_location_info_from_cafe(fru_prop->fru_handle, cafe_info);
    return RET_OK;
}

gint32 e2p_cafe_record_write_specific_fru(OBJ_HANDLE fru_handle, const gchar *cafe_key_str, CAFE_DATA_S *cafe_data)
{
    guint32 key_str_len = strlen(cafe_key_str);
    const gchar *fru_handle_name = dfl_get_object_name(fru_handle);

    if (__validattion_check_e2p_cafe_record(fru_handle, cafe_key_str, cafe_data, fru_handle_name) != RET_OK) {
        return RET_ERR;
    }

    FRU_PRIV_PROPERTY_S *priv_data = NULL;
    gint32 ret = get_obj_priv_data(fru_handle, (gpointer *)&priv_data);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get binded data from object handle [%s] failed, ret is %d", fru_handle_name, ret);
        return RET_ERR;
    }

    if (priv_data->cafe_info->frudev_handle == 0) {
        put_obj_priv_data(fru_handle, (gpointer)priv_data);
        return RET_OK;
    }

    gchar cafe_key_dup[CAFE_KEY_MAX_LEN] = {0};
    errno_t safe_fun_ret = memmove_s(cafe_key_dup, CAFE_KEY_MAX_LEN, cafe_key_str, key_str_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        put_obj_priv_data(fru_handle, (gpointer)priv_data);
        return RET_ERR;
    }

    CAFE_DATA_S cafe_key = { 0 };
    cafe_key.data = (guchar *)cafe_key_dup;
    cafe_key.len = key_str_len;
    cafe_key.type = TYPE_8BIT_ASCII;

    
    CAFE_DATA_S *cafe_data_old = e2p_cafe_get_record_by_key(priv_data->cafe_info, &cafe_key);
    if ((cafe_data_old != NULL) && (cafe_data_old->len == cafe_data->len) &&
        (memcmp(cafe_data_old->data, cafe_data->data, cafe_data_old->len) == 0)) {
        debug_log(DLOG_INFO, "%s: val for cafe key [%s] duplicated in eeprom", __FUNCTION__, cafe_key.data);
        put_obj_priv_data(fru_handle, (gpointer)priv_data);
        return RET_OK;
    }

    
    ret = __e2p_cafe_write_record_by_key(priv_data->cafe_info, &cafe_key, cafe_data);
    put_obj_priv_data(fru_handle, (gpointer)priv_data);
    return ret;
}


LOCAL void frudata_set_eeprom_inner_scan_status(const guint8 status)
{
    G_LOCK(g_lock_eeprom_scan_status);
    g_eeprom_scan_status = status;
    G_UNLOCK(g_lock_eeprom_scan_status);
}


LOCAL guint8 frudata_get_eeprom_inner_scan_status(void)
{
    guint8 status;

    G_LOCK(g_lock_eeprom_scan_status);
    status = g_eeprom_scan_status;
    G_UNLOCK(g_lock_eeprom_scan_status);

    return status;
}


void set_eeprom_stop_routine(const guint8 is_stop)
{
    guint8 status = (is_stop ? SCAN_STOPPED : SCAN_STARTED);
    frudata_set_eeprom_inner_scan_status(status);
}


gint32 frudata_set_eeprom_scan_status(const guint8 status)
{
    OBJ_HANDLE dft_obj = 0;
    gint32 ret;
    guint8 dft_enable = DISABLE;
    guint8 set_status;

    ret = dal_get_object_handle_nth(CLASS_DFT_STATUS, 0, &dft_obj);
    if (ret != RET_OK) {
        return ret;
    }

    ret = dal_get_property_value_byte(dft_obj, PROPERTY_DFT_ENABLE, &dft_enable);
    if (ret != RET_OK) {
        return ret;
    }

    if (dft_enable != ENABLE) {
        debug_log(DLOG_ERROR, "%s: current environment is not in dft mode.", __FUNCTION__);
        return RET_ERR;
    }

    set_status = (status == ENABLE) ? SCAN_STARTING : SCAN_STOPPING;
    frudata_set_eeprom_inner_scan_status(set_status);
    return RET_OK;
}


guint8 frudata_get_eeprom_scan_status(void)
{
    return frudata_get_eeprom_inner_scan_status();
}


void frudata_change_eeprom_scan_status(void)
{
    
    if (frudata_get_eeprom_inner_scan_status() == SCAN_STOPPING) {
        frudata_set_eeprom_inner_scan_status(SCAN_STOPPED);
    }

    if (frudata_get_eeprom_inner_scan_status() == SCAN_STARTING) {
        frudata_set_eeprom_inner_scan_status(SCAN_STARTED);
    }
}