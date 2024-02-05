

#include "elabel/elabel_init.h"
#include "elabel/base.h"
#include "elabel/elabel.h"
#include "media/e2p_large.h"
#include "media/etc.h"

#define CHECK_INIT_ELABEL_INFO(elabel_field_src, ealbel_field_dst)                                  \
    do {                                                                                            \
        if ((elabel_field_src).len > sizeof((elabel_field_src).data)) {                             \
            break;                                                                                  \
        }                                                                                           \
        guint16 real_crc = 0;                                                                       \
        (void)memset_s(&(ealbel_field_dst), sizeof(ealbel_field_dst), 0, sizeof(ealbel_field_dst)); \
        real_crc = arith_make_crc_checksum(0, (const guchar *)&(elabel_field_src).len,              \
            (elabel_field_src).len + sizeof(guint16));                                              \
        if (real_crc == (elabel_field_src).crc) {                                                   \
            (void)memcpy_s(&(ealbel_field_dst), sizeof(ealbel_field_dst), &(elabel_field_src),      \
                sizeof(elabel_field_src));                                                          \
        }                                                                                           \
    } while (0)

// 把电子标签数据项复制到V1电子标签中，除了extend域
#define COPY_ELABEL_FIELD_FROM_BAKELABEL(elabel_field, bakelabel_field)                                                \
    do {                                                                                                               \
        if (((bakelabel_field).len > sizeof((elabel_field).data)) ||                                                   \
            ((bakelabel_field).len > sizeof((bakelabel_field).data))) {                                                \
            break;                                                                                                     \
        }                                                                                                              \
        (elabel_field).len = (bakelabel_field).len;                                                                    \
        errno_t safe_fun_ret =                                                                                         \
            memcpy_s((elabel_field).data, sizeof((elabel_field).data), (bakelabel_field).data, (bakelabel_field).len); \
        if (safe_fun_ret != EOK) {                                                                                     \
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);                        \
        }                                                                                                              \
        (elabel_field).crc =                                                                                           \
            arith_make_crc_checksum(0, (const guchar *)&((elabel_field).len), (elabel_field).len + sizeof(guint16));   \
    } while (0)

enum {
    READ_ELABEL_SUCCESS = 0,
    READ_ELABEL_FAILED,
    DISABLE_CUSTOMIZE,        
};

LOCAL void __check_init_elabel_info(const ELABEL_INFO_S *elabel_src, ELABEL_INFO_S *elabel_dst);
LOCAL void __init_elabel_extend(const ELABEL_INFO_S *elabel_src, ELABEL_INFO_S *elabel_dst);
LOCAL void __init_elabel_internal_extend(const ELABEL_INFO_S *elabel_src, ELABEL_INFO_S *elabel_dst);
LOCAL void __init_elabel_chassis_extend(const ELABEL_INFO_S *elabel_src, ELABEL_INFO_S *elabel_dst);
LOCAL void __init_elabel_product_extend(const ELABEL_INFO_S *elabel_src, ELABEL_INFO_S *elabel_dst);
LOCAL void __sync_elabel_info(ELABEL_INFO_S *elabel_dst, FRU_INFO_S *fru_info);
LOCAL void __check_init_elabel_from_eeprom(const ELABEL_INFO_S *elabel_src, ELABEL_INFO_S *elabel_dst,
    FRU_INFO_S *fru_info);
LOCAL void __init_elabel_system(FRU_PRIV_PROPERTY_S *fru_priv);
LOCAL void __init_elabel_chassis_from_fru(const FRU_INFO_S *fru_info, ELABEL_INFO_S *elabel_info);
LOCAL void __init_elabel_board_from_fru(const FRU_INFO_S *fru_info, ELABEL_INFO_S *elabel_info);
LOCAL void __init_elabel_product_from_fru(const FRU_INFO_S *fru_info, ELABEL_INFO_S *elabel_info);
LOCAL void __init_elabel_info_from_fru(const FRU_PROPERTY_S *fru_prop, const FRU_INFO_S *fru_info,
    ELABEL_INFO_S *elabel_info);
LOCAL void __init_elabel_area(FRU_PRIV_PROPERTY_S *fru_priv, ELABEL_INFO_S *elabel_info_tmp);
LOCAL gint32 __frudev_write(FRU_PRIV_PROPERTY_S *fru_priv, ELABEL_INFO_S *elabel_info_tmp);
LOCAL void tc_copy_elabel_field_from_bakelabel(ELABEL_FIELD_72_BYTES_S *elabel_field, gchar *bakelabel_field);
LOCAL void tc_copy_system_version_from_bakelabel(ELABEL_FIELD_72_BYTES_S *elabel_field, guint8 bakelabel_field);

LOCAL void __check_init_elabel_info(const ELABEL_INFO_S *elabel_src, ELABEL_INFO_S *elabel_dst)
{
    
    CHECK_INIT_ELABEL_INFO(elabel_src->internal, elabel_dst->internal);

    
    CHECK_INIT_ELABEL_INFO(elabel_src->chassis.type, elabel_dst->chassis.type);
    CHECK_INIT_ELABEL_INFO(elabel_src->chassis.part_num, elabel_dst->chassis.part_num);
    CHECK_INIT_ELABEL_INFO(elabel_src->chassis.serial_num, elabel_dst->chassis.serial_num);

    
    CHECK_INIT_ELABEL_INFO(elabel_src->board.mfg_time, elabel_dst->board.mfg_time);
    CHECK_INIT_ELABEL_INFO(elabel_src->board.manufacture, elabel_dst->board.manufacture);
    CHECK_INIT_ELABEL_INFO(elabel_src->board.product_name, elabel_dst->board.product_name);
    CHECK_INIT_ELABEL_INFO(elabel_src->board.serial_num, elabel_dst->board.serial_num);
    CHECK_INIT_ELABEL_INFO(elabel_src->board.part_num, elabel_dst->board.part_num);
    CHECK_INIT_ELABEL_INFO(elabel_src->board.file_id, elabel_dst->board.file_id);

    
    CHECK_INIT_ELABEL_INFO(elabel_src->product.manufacture, elabel_dst->product.manufacture);
    CHECK_INIT_ELABEL_INFO(elabel_src->product.name, elabel_dst->product.name);
    CHECK_INIT_ELABEL_INFO(elabel_src->product.part_num, elabel_dst->product.part_num);
    CHECK_INIT_ELABEL_INFO(elabel_src->product.version, elabel_dst->product.version);
    CHECK_INIT_ELABEL_INFO(elabel_src->product.serial_num, elabel_dst->product.serial_num);
    CHECK_INIT_ELABEL_INFO(elabel_src->product.asset_tag, elabel_dst->product.asset_tag);
    CHECK_INIT_ELABEL_INFO(elabel_src->product.file_id, elabel_dst->product.file_id);
}

LOCAL void __init_elabel_extend(const ELABEL_INFO_S *elabel_src, ELABEL_INFO_S *elabel_dst)
{
    if (elabel_src->extend.len > FRU_EXTEND_DATA_MAX_LEN) {
        debug_log(DLOG_ERROR, "[%s] invlaid extend elabel length : %u", __FUNCTION__, elabel_src->extend.len);
        return;
    }

    
    (void)memset_s(&elabel_dst->extend, sizeof(ELABEL_EXTEND_AREA_S), 0, sizeof(ELABEL_EXTEND_AREA_S));
    guint16 real_crc = arith_make_crc_checksum(0, (const guchar *)&elabel_src->extend.len,
        MIN(elabel_src->extend.len + FRU_EXTERN_LABEL_PREFIX_LEN,
        sizeof(elabel_src->extend.data) + FRU_EXTERN_LABEL_PREFIX_LEN));
    if (real_crc == elabel_src->extend.crc) {
        (void)memcpy_s(&elabel_dst->extend, sizeof(ELABEL_EXTEND_AREA_S), &elabel_src->extend,
            sizeof(ELABEL_EXTEND_AREA_S));
    }

    // 初始化为V2以分号";" 分隔的格式，最后一个'\0'不替换
    split_extend_with_semicolon(&elabel_dst->extend);
}

LOCAL void __init_elabel_internal_extend(const ELABEL_INFO_S *elabel_src, ELABEL_INFO_S *elabel_dst)
{
    if (elabel_src->internal_extend.len > FRU_EXTEND_DATA_MAX_LEN) {
        debug_log(DLOG_ERROR, "[%s] invlaid extend elabel length : %u", __FUNCTION__, elabel_src->internal_extend.len);
        return;
    }

    
    (void)memset_s(&elabel_dst->internal_extend, sizeof(ELABEL_EXTEND_AREA_S), 0, sizeof(ELABEL_EXTEND_AREA_S));
    guint16 internal_crc = arith_make_crc_checksum(0, (const guchar *)&elabel_src->internal_extend.len,
        MIN(elabel_src->internal_extend.len + FRU_EXTERN_LABEL_PREFIX_LEN,
        sizeof(elabel_src->internal_extend.data) + FRU_EXTERN_LABEL_PREFIX_LEN));
    if (internal_crc == elabel_src->internal_extend.crc) {
        (void)memcpy_s(&elabel_dst->internal_extend, sizeof(ELABEL_EXTEND_AREA_S), &elabel_src->internal_extend,
            sizeof(ELABEL_EXTEND_AREA_S));
    }

    // 初始化为V2以分号";" 分隔的格式，最后一个'\0'不替换
    split_extend_with_semicolon(&elabel_dst->internal_extend);
}

LOCAL void __init_elabel_chassis_extend(const ELABEL_INFO_S *elabel_src, ELABEL_INFO_S *elabel_dst)
{
    if (elabel_src->chassis_extend.len > FRU_EXTEND_DATA_MAX_LEN) {
        debug_log(DLOG_ERROR, "[%s] invlaid extend elabel length : %u", __FUNCTION__, elabel_src->chassis_extend.len);
        return;
    }

    
    (void)memset_s(&elabel_dst->chassis_extend, sizeof(ELABEL_EXTEND_AREA_S), 0, sizeof(ELABEL_EXTEND_AREA_S));
    guint16 chassis_crc = arith_make_crc_checksum(0, (const guchar *)&elabel_src->chassis_extend.len,
        MIN(elabel_src->chassis_extend.len + FRU_EXTERN_LABEL_PREFIX_LEN,
        sizeof(elabel_src->chassis_extend.data) + FRU_EXTERN_LABEL_PREFIX_LEN));
    if (chassis_crc == elabel_src->chassis_extend.crc) {
        (void)memcpy_s(&elabel_dst->chassis_extend, sizeof(ELABEL_EXTEND_AREA_S), &elabel_src->chassis_extend,
            sizeof(ELABEL_EXTEND_AREA_S));
    }

    // 初始化为V2以分号";" 分隔的格式，最后一个'\0'不替换
    split_extend_with_semicolon(&elabel_dst->chassis_extend);
}

LOCAL void __init_elabel_product_extend(const ELABEL_INFO_S *elabel_src, ELABEL_INFO_S *elabel_dst)
{
    if (elabel_src->product_extend.len > FRU_EXTEND_DATA_MAX_LEN) {
        debug_log(DLOG_ERROR, "[%s] invlaid extend elabel length : %u", __FUNCTION__, elabel_src->product_extend.len);
        return;
    }

    
    (void)memset_s(&elabel_dst->product_extend, sizeof(ELABEL_EXTEND_AREA_S), 0, sizeof(ELABEL_EXTEND_AREA_S));
    guint16 product_crc = arith_make_crc_checksum(0, (const guchar *)&elabel_src->product_extend.len,
        MIN(elabel_src->product_extend.len + FRU_EXTERN_LABEL_PREFIX_LEN,
        sizeof(elabel_src->product_extend.data) + FRU_EXTERN_LABEL_PREFIX_LEN));
    if (product_crc == elabel_src->product_extend.crc) {
        (void)memcpy_s(&elabel_dst->product_extend, sizeof(ELABEL_EXTEND_AREA_S), &elabel_src->product_extend,
            sizeof(ELABEL_EXTEND_AREA_S));
    }

    // 初始化为V2以分号";" 分隔的格式，最后一个'\0'不替换
    split_extend_with_semicolon(&elabel_dst->product_extend);
}

LOCAL void __sync_elabel_info(ELABEL_INFO_S *elabel_dst, FRU_INFO_S *fru_info)
{
    
    if (elabel_dst->board.manufacture.len == 0) {
        sync_elabel_from_fru(&(elabel_dst->board.manufacture), fru_info->board.board_manufacturer);
    }

    if (elabel_dst->board.file_id.len == 0) {
        sync_elabel_from_fru(&(elabel_dst->board.file_id), fru_info->board.file_id);
    }

    if (elabel_dst->product.manufacture.len == 0) {
        sync_elabel_from_fru(&(elabel_dst->product.manufacture), fru_info->product.product_manufacturer);
    }

    if (elabel_dst->product.file_id.len == 0) {
        sync_elabel_from_fru(&(elabel_dst->product.file_id), fru_info->product.file_id);
    }
}


LOCAL void __check_init_elabel_from_eeprom(const ELABEL_INFO_S *elabel_src, ELABEL_INFO_S *elabel_dst,
    FRU_INFO_S *fru_info)
{
    if (elabel_src == NULL || elabel_dst == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return;
    }

    __check_init_elabel_info(elabel_src, elabel_dst);

    __init_elabel_extend(elabel_src, elabel_dst);

    __init_elabel_internal_extend(elabel_src, elabel_dst);

    __init_elabel_chassis_extend(elabel_src, elabel_dst);

    __init_elabel_product_extend(elabel_src, elabel_dst);

    __sync_elabel_info(elabel_dst, fru_info);
}


LOCAL void __init_elabel_system(FRU_PRIV_PROPERTY_S *fru_priv)
{
    
    (void)memset_s((void *)&(fru_priv->elabel_info->system), sizeof(ELABEL_SYSTEM_AREA_S), 0,
        sizeof(ELABEL_SYSTEM_AREA_S));

    if (fru_priv->fru_property.is_cont_sys_area == 0) {
        return;
    }

    if (fru_priv->eeprom_format == EEPROM_FORMAT_TIANCHI) {
        tc_copy_elabel_field_from_bakelabel(&fru_priv->elabel_info->system.manufacture,
            fru_priv->system_info->tc_system_desc_st.sys_mfg_name);
        tc_copy_elabel_field_from_bakelabel(&fru_priv->elabel_info->system.name,
            fru_priv->system_info->tc_system_desc_st.sys_product_name);
        tc_copy_system_version_from_bakelabel(&fru_priv->elabel_info->system.version,
            fru_priv->system_info->tc_system_desc_st.sys_ver);
        tc_copy_elabel_field_from_bakelabel(&fru_priv->elabel_info->system.serial_num,
            fru_priv->system_info->tc_system_desc_st.sys_sn);
    } else {
        COPY_ELABEL_FIELD_FROM_BAKELABEL(fru_priv->elabel_info->system.manufacture,
            fru_priv->system_info->system_area_st.sys_mfg_name);
        COPY_ELABEL_FIELD_FROM_BAKELABEL(fru_priv->elabel_info->system.name,
            fru_priv->system_info->system_area_st.sys_product_name);
        COPY_ELABEL_FIELD_FROM_BAKELABEL(fru_priv->elabel_info->system.version,
            fru_priv->system_info->system_area_st.sys_ver);
        COPY_ELABEL_FIELD_FROM_BAKELABEL(fru_priv->elabel_info->system.serial_num,
            fru_priv->system_info->system_area_st.sys_sn);
    }
}

LOCAL void __init_elabel_chassis_from_fru(const FRU_INFO_S *fru_info, ELABEL_INFO_S *elabel_info)
{
    if (!fru_info->has_chassis) {
        return;
    }

    elabel_info->chassis.type.len = (guint16)sizeof(gchar);
    elabel_info->chassis.type.data[0] = fru_info->chassis.chassis_type;
    elabel_info->chassis.type.crc = arith_make_crc_checksum(0, (const guchar *)&elabel_info->chassis.type.len,
        elabel_info->chassis.type.len + sizeof(guint16));

    sync_elabel_from_fru(&(elabel_info->chassis.part_num), fru_info->chassis.chassis_part_num);
    sync_elabel_from_fru(&(elabel_info->chassis.serial_num), fru_info->chassis.chassis_serial_num);
    // chassis域的extend域
    combine_extension_label(fru_info->chassis.custom_info, (gchar *)elabel_info->chassis_extend.data,
        sizeof(elabel_info->chassis_extend.data), &elabel_info->chassis_extend.num);
    elabel_info->chassis_extend.len = (guint16)strlen((const gchar *)elabel_info->chassis_extend.data);
    elabel_info->chassis_extend.crc = arith_make_crc_checksum(0, (const guchar *)&elabel_info->chassis_extend.len,
        MIN(elabel_info->chassis_extend.len + FRU_EXTERN_LABEL_PREFIX_LEN,
        sizeof(elabel_info->chassis_extend.data) + FRU_EXTERN_LABEL_PREFIX_LEN));
}

LOCAL void __init_elabel_board_from_fru(const FRU_INFO_S *fru_info, ELABEL_INFO_S *elabel_info)
{
    if (!fru_info->has_board) {
        return;
    }
    
    OBJ_HANDLE obj_handle = 0;
    guint8 elabel_format = 0;
    (void)dfl_get_object_handle(OBJECT_BMC, &obj_handle);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_BMC_ELABELMANUFORMAT, &elabel_format);
    if (elabel_format == 1) {
        elabel_info->board.mfg_time.len = (guint16)sizeof(fru_info->board.mfg_date_time);
        errno_t safe_fun_ret = memcpy_s(elabel_info->board.mfg_time.data, sizeof(elabel_info->board.mfg_time.data),
            fru_info->board.mfg_date_time, elabel_info->board.mfg_time.len);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        }
    } else {
        elabel_info->board.mfg_time.len = (guint16)sizeof(fru_info->board.mfg_date_time) - 1;
        errno_t safe_fun_ret = memcpy_s(elabel_info->board.mfg_time.data,
            sizeof(elabel_info->board.mfg_time.data) - 1,
            fru_info->board.mfg_date_time, elabel_info->board.mfg_time.len);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        }
    }

    elabel_info->board.mfg_time.crc = arith_make_crc_checksum(0, (const guchar *)&elabel_info->board.mfg_time.len,
        MIN(elabel_info->board.mfg_time.len + sizeof(guint16),
        sizeof(elabel_info->board.mfg_time.data) + sizeof(guint16)));

    sync_elabel_from_fru(&(elabel_info->board.manufacture), fru_info->board.board_manufacturer);
    sync_elabel_from_fru(&(elabel_info->board.product_name), fru_info->board.board_product_name);
    sync_elabel_from_fru(&(elabel_info->board.part_num), fru_info->board.board_part_num);
    sync_elabel_from_fru(&(elabel_info->board.serial_num), fru_info->board.board_serial_num);
    sync_elabel_from_fru(&(elabel_info->board.file_id), fru_info->board.file_id);

    combine_extension_label(fru_info->board.extension_label, (gchar *)elabel_info->extend.data,
        sizeof(elabel_info->extend.data), &elabel_info->extend.num);
    elabel_info->extend.len = (guint16)strlen((const gchar *)elabel_info->extend.data);
    elabel_info->extend.crc = arith_make_crc_checksum(0, (const guchar *)&elabel_info->extend.len,
        MIN(elabel_info->extend.len + FRU_EXTERN_LABEL_PREFIX_LEN,
        sizeof(elabel_info->extend.data) + FRU_EXTERN_LABEL_PREFIX_LEN));
}

LOCAL void __init_elabel_product_from_fru(const FRU_INFO_S *fru_info, ELABEL_INFO_S *elabel_info)
{
    if (!fru_info->has_product) {
        return;
    }

    sync_elabel_from_fru(&(elabel_info->product.manufacture), fru_info->product.product_manufacturer);
    sync_elabel_from_fru(&(elabel_info->product.name), fru_info->product.product_name);
    sync_elabel_from_fru(&(elabel_info->product.part_num), fru_info->product.product_part_num);
    sync_elabel_from_fru(&(elabel_info->product.version), fru_info->product.product_version);
    sync_elabel_from_fru(&(elabel_info->product.serial_num), fru_info->product.product_serial_num);
    sync_elabel_from_fru(&(elabel_info->product.asset_tag), fru_info->product.product_asset_tag);
    sync_elabel_from_fru(&(elabel_info->product.file_id), fru_info->product.file_id);
    // product域的extend域
    combine_extension_label(fru_info->product.custom_info, (gchar *)elabel_info->product_extend.data,
        sizeof(elabel_info->product_extend.data), &elabel_info->product_extend.num);
    elabel_info->product_extend.len = (guint16)strlen((const gchar *)elabel_info->product_extend.data);
    elabel_info->product_extend.crc = arith_make_crc_checksum(0, (const guchar *)&elabel_info->product_extend.len,
        MIN(elabel_info->product_extend.len + FRU_EXTERN_LABEL_PREFIX_LEN,
        sizeof(elabel_info->product_extend.data) + FRU_EXTERN_LABEL_PREFIX_LEN));
}

LOCAL void init_elabel_multi_record_from_fru(const FRU_INFO_S *fru_info, ELABEL_INFO_S *elabel_info)
{
    if (!fru_info->has_multi) {
        return;
    }
    errno_t memcoy_s_ret;
    guint32 single_record_len;
    guint32 total_len = 0;
    guint8 *elabel_multi_record = elabel_info->multi_record;
    GList *multi_record = fru_info->multi_record;

    while (multi_record) {
        FRU_MULTI_AREA_S *fru_record = (FRU_MULTI_AREA_S *)multi_record->data;
        single_record_len = SINGLE_MULTI_HEAD_LEN + fru_record->header.len;
        if (total_len + single_record_len > MAX_MULTI_LEN) {
            break;
        }
        memcoy_s_ret =
            memcpy_s(elabel_multi_record + total_len, MAX_MULTI_LEN - total_len, fru_record, single_record_len);
        if (memcoy_s_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, memcoy_s_ret);
            break;
        }
        total_len += single_record_len;
        multi_record = multi_record->next;
    }
}


LOCAL void __init_elabel_info_from_fru(const FRU_PROPERTY_S *fru_prop, const FRU_INFO_S *fru_info,
    ELABEL_INFO_S *elabel_info)
{
    if (fru_info == NULL || elabel_info == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return;
    }

    (void)memset_s(elabel_info, sizeof(ELABEL_INFO_S), 0, sizeof(ELABEL_INFO_S));

    
    __init_elabel_chassis_from_fru(fru_info, elabel_info);

    __init_elabel_board_from_fru(fru_info, elabel_info);

    __init_elabel_product_from_fru(fru_info, elabel_info);

    init_elabel_multi_record_from_fru(fru_info, elabel_info);
}

LOCAL void init_elabel_internal_from_fru(const FRU_INFO_S *fru_info, ELABEL_INFO_S *elabel_info)
{
    if (!fru_info->has_internal) {
        return;
    }

    (void)memset_s((void *)&(elabel_info->internal), sizeof(ELABEL_INTERNAL_AREA_S), 0, sizeof(ELABEL_INTERNAL_AREA_S));

    elabel_info->internal.len = fru_info->internal.len;
    errno_t memcoy_s_ret =
        memcpy_s(elabel_info->internal.data, sizeof(ELABEL_INTERNAL_AREA_S) - sizeof(guint16) - sizeof(guint16),
        fru_info->internal.data, fru_info->internal.len);
    if (memcoy_s_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, memcoy_s_ret);
    }
    elabel_info->internal.crc = arith_make_crc_checksum(0, (const guchar *)&(elabel_info->internal.len),
        MIN(sizeof(guint16) + elabel_info->internal.len, FRU_INTERNAL_AREA_LEN - sizeof(guint16)));
}

LOCAL void init_internal_extend_from_eeprom(FRU_PRIV_PROPERTY_S *fru_priv)
{
    if (fru_priv->fru_property.is_support_config_area) {
        return;
    }

    ELABEL_EXTEND_AREA_S internal_extend = { 0 };
    gint32 ret = frudev_read_from_eeprom(fru_priv->fru_property.frudev_handle, ELABEL_EXTEND_INFO_BLOCK_OFFSET,
        &internal_extend, sizeof(ELABEL_EXTEND_AREA_S));
    if (ret != SYN_FRU_OK) {
        debug_log(DLOG_ERROR, "[%s]read extern elabel from eeprom fail.", __FUNCTION__);
        return;
    }

    (void)memset_s((void *)&(fru_priv->elabel_info->internal_extend), sizeof(ELABEL_EXTEND_AREA_S), 0,
        sizeof(ELABEL_EXTEND_AREA_S));

    if (internal_extend.len > FRU_EXTEND_DATA_MAX_LEN) {
        debug_log(DLOG_ERROR, "[%s] invlaid extend elabel length : %u", __FUNCTION__, internal_extend.len);
        return;
    }

    guint16 real_crc = arith_make_crc_checksum(0, (const guchar *)&internal_extend.len,
        MIN(FRU_EXTERN_LABEL_PREFIX_LEN + internal_extend.len, sizeof(ELABEL_EXTEND_AREA_S) - sizeof(guint16)));
    if (real_crc == internal_extend.crc) {
        errno_t memcoy_s_ret = memcpy_s((void *)&(fru_priv->elabel_info->internal_extend), sizeof(ELABEL_EXTEND_AREA_S),
            &internal_extend, sizeof(ELABEL_EXTEND_AREA_S));
        if (memcoy_s_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, memcoy_s_ret);
        }
    } else {
        debug_log(DLOG_ERROR, "internal extend info is corrupt (fruid:%d)(crc: %x %x).", fru_priv->fru_property.fru_id,
            internal_extend.crc, real_crc);
    }
    // 初始化为V2以分号";" 分隔的格式，最后一个'\0'不替换
    split_extend_with_semicolon(&fru_priv->elabel_info->internal_extend);
}

LOCAL void __init_elabel_area(FRU_PRIV_PROPERTY_S *fru_priv, ELABEL_INFO_S *elabel_info_tmp)
{
    if (!fru_priv->fru_property.frudev_crc_state) {
        
        __check_init_elabel_from_eeprom(elabel_info_tmp, fru_priv->elabel_info, fru_priv->fru_info);
        debug_log(DLOG_ERROR, "read fru fail, sync elabel from fru fail.");
        __init_elabel_system(fru_priv);
        return;
    }

    
    __init_elabel_info_from_fru(&(fru_priv->fru_property), fru_priv->fru_info, fru_priv->elabel_info);
    debug_log(DLOG_INFO, "sync elabel from fru success.");

    
    (void)memset_s((void *)&(fru_priv->elabel_info->internal), sizeof(ELABEL_INTERNAL_AREA_S), 0,
        sizeof(ELABEL_INTERNAL_AREA_S));
    guint16 real_crc = arith_make_crc_checksum(0, (const guchar *)&elabel_info_tmp->internal.len,
        MIN(sizeof(guint16) + elabel_info_tmp->internal.len, FRU_INTERNAL_AREA_LEN - sizeof(guint16)));
    if (real_crc == elabel_info_tmp->internal.crc) {
        (void)memcpy_s((void *)&(fru_priv->elabel_info->internal), sizeof(ELABEL_INTERNAL_AREA_S),
            &elabel_info_tmp->internal, sizeof(ELABEL_INTERNAL_AREA_S));
    } else {
        debug_log(DLOG_ERROR, "internal info is corrupt (fruid:%d)(crc: %x %x).", fru_priv->fru_property.fru_id,
            elabel_info_tmp->internal.crc, real_crc);
    }

    init_internal_extend_from_eeprom(fru_priv);

    
    __init_elabel_system(fru_priv);
}


gboolean is_support_extern_lable(guint8 fru_id)
{
    gint32 ret;
    GSList *obj_list = NULL;

    // 判断是否是FC扣卡
    ret = dfl_get_object_list(FCCARD_CLASS_FCSWCONFIG, &obj_list);
    if (ret != RET_OK || g_slist_length(obj_list) == 0) {
        return TRUE;
    }
    g_slist_free(obj_list);

    // 判断fru ID
    if (fru_id == FC_FRU_ID) {
        return FALSE;
    } else {
        return TRUE;
    }
}
LOCAL gint32 __frudev_write(FRU_PRIV_PROPERTY_S *fru_priv, ELABEL_INFO_S *elabel_info_tmp)
{
    
    gint32 ret = memcmp(elabel_info_tmp, fru_priv->elabel_info, sizeof(ELABEL_INFO_S));
    if (ret == SYN_FRU_OK) {
        return SYN_FRU_OK;
    }

    ret = frudev_write_to_eeprom(fru_priv->fru_property.frudev_handle, ELABEL_INFO_BLOCK_OFFSET, fru_priv->elabel_info,
        ELABEL_INFO_AREA_SIZE);
    if (ret != SYN_FRU_OK) {
        debug_log(DLOG_ERROR, "write elabel to eeprom fail.");
        return ret;
    }

    if (fru_priv->fru_property.is_support_config_area != 0) {
        return SYN_FRU_OK;
    }

    
    if (is_support_extern_lable(fru_priv->fru_property.fru_id)) {
        ret = frudev_write_to_eeprom(fru_priv->fru_property.frudev_handle, ELABEL_EXTEND_INFO_BLOCK_OFFSET,
            &(fru_priv->elabel_info->internal_extend), ELABEL_EXTEND_INFO_SIZE);
        if (ret != SYN_FRU_OK) {
            debug_log(DLOG_ERROR, "%s write extern elabel to eeprom fail.", __FUNCTION__);
            return ret;
        }
    }

    return SYN_FRU_OK;
}

LOCAL void tc_copy_elabel_field_from_bakelabel(ELABEL_FIELD_72_BYTES_S *elabel_field, gchar *bakelabel_field)
{
    elabel_field->len = (guint16)strlen(bakelabel_field);
    errno_t safe_fun_ret =
        memcpy_s(elabel_field->data, sizeof(elabel_field->data), bakelabel_field, strlen(bakelabel_field));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    elabel_field->crc =
        arith_make_crc_checksum(0, (const guchar *)&(elabel_field->len), elabel_field->len + sizeof(guint16));
}

LOCAL void tc_copy_system_version_from_bakelabel(ELABEL_FIELD_72_BYTES_S *elabel_field, guint8 bakelabel_field)
{
    elabel_field->len = (guint16)sizeof(bakelabel_field);
    elabel_field->data[0] = bakelabel_field;
    elabel_field->crc =
        arith_make_crc_checksum(0, (const guchar *)&(elabel_field->len), elabel_field->len + sizeof(guint16));
}

LOCAL void __update_system_data(SYSTEM_INFO_U *system_info, const FRU_INFO_S *fru_info)
{
    
    if (((fru_info->product.product_serial_num.len > 0) &&
        (fru_info->product.product_serial_num.len <= FRU_MAX_ELABEL_LEN)) &&
        (system_info->system_area_st.sys_sn.len == 0)) {
        errno_t safe_fun_ret = memcpy_s((gchar *)system_info->system_area_st.sys_sn.data, FRU_MAX_ELABEL_LEN,
            (const gchar *)(fru_info->product.product_serial_num.content), fru_info->product.product_serial_num.len);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        }
        system_info->system_area_st.sys_sn.len = fru_info->product.product_serial_num.len;
    }

    return;
}

LOCAL gboolean tc_update_system_data(TC_SYSTEM_DESC_S sys_desc, FRU_PROPERTY_S *fru_prop, SYSTEM_INFO_U *system_info,
    gulong update_sys_syn_semid, const FRU_INFO_S *fru_info)
{
    
    errno_t safe_fun_ret;
    if ((sys_desc.crc == arith_make_crc32(0, (const char *)&sys_desc, sizeof(TC_SYSTEM_DESC_S) - sizeof(guint32)))) {
        safe_fun_ret = memcpy_s(system_info, sizeof(SYSTEM_INFO_U), &sys_desc, sizeof(TC_SYSTEM_DESC_S));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        }

        if ((strcmp((const gchar *)fru_prop->product_name,
            (const gchar *)system_info->tc_system_desc_st.sys_product_name)) &&
            (!system_info->tc_system_desc_st.is_customize)) {
            (void)vos_thread_sem4_v(update_sys_syn_semid);
            return FALSE;
        }
    } else {
        (void)memset_s(system_info, sizeof(TC_SYSTEM_DESC_S), 0, sizeof(TC_SYSTEM_DESC_S));
        safe_fun_ret = strncpy_s((gchar *)system_info->tc_system_desc_st.sys_product_name, SYS_PRODUCT_NAME_MAX_LEN,
            fru_prop->product_name, strlen(fru_prop->product_name));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        }
    }

    
    if (((fru_info->product.product_serial_num.len > 0) &&
        (fru_info->product.product_serial_num.len <= SYS_PRODUCT_NAME_MAX_LEN)) &&
        (system_info->system_area_st.sys_sn.len == 0)) {
        safe_fun_ret = memcpy_s((gchar *)system_info->tc_system_desc_st.sys_sn, SYS_SN_MAX_LEN,
            (const gchar *)(fru_info->product.product_serial_num.content), fru_info->product.product_serial_num.len);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        }
    }

    return TRUE;
}


gint32 init_elabel_info(FRU_PRIV_PROPERTY_S *fru_priv)
{
    if (fru_priv == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return SYN_DFL_FAIL;
    }

    if (fru_priv->fru_property.is_sup_dft == 0) {
        return SYN_FRU_OK;
    }

    if (fru_priv->fru_property.frudev_type != STORAGE_TYPE_EEPROM) {
        __init_elabel_info_from_fru(&(fru_priv->fru_property), fru_priv->fru_info, fru_priv->elabel_info);

        
        __init_elabel_system(fru_priv);

        debug_log(DLOG_INFO, "Fru does not support syn.");
        return SYN_FRU_OK;
    }

    if (fru_priv->eeprom_format == EEPROM_FORMAT_TIANCHI) {
        __init_elabel_info_from_fru(&(fru_priv->fru_property), fru_priv->fru_info, fru_priv->elabel_info);

        init_elabel_internal_from_fru(fru_priv->fru_info, fru_priv->elabel_info);

        
        __init_elabel_system(fru_priv);

        return SYN_FRU_OK;
    }

    
    ELABEL_INFO_S elabel_info_tmp = { 0 };
    gint32 ret = get_elabel(fru_priv, &elabel_info_tmp);
    if (ret != RET_OK) {
        return ret;
    }

    // 解决eeprom为空，fru_info被初始化为默认版本号，导致_fru_init_task任务中
    // update_file_id()不被触发，因此不会发送version change事件给smm，smm侧查询的
    // frudata数据中"Board FRU File ID "一直为老的版本号的问题
    if ((fru_priv->fru_property.fru_id == 0) && (strcmp((const gchar *)elabel_info_tmp.board.file_id.data,
        (const gchar *)fru_priv->fru_info->board.file_id.content) != 0)) {
        
        send_ver_change_evt(0x02);
    }

    __init_elabel_area(fru_priv, &elabel_info_tmp);

    return __frudev_write(fru_priv, &elabel_info_tmp);
}


LOCAL gint32 is_atlantic_control_board(void)
{
    guint8 board_id;
    gint32 ret;
    ret = dal_get_board_id(&board_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get board_id failed, ret = %d.", ret);
        return RET_ERR; // 获取失败,默认非大西洋控制板
    }
    // 大西洋board_id为 0x1b
    if (board_id == 0x1b) {
        return RET_OK;
    }
    return RET_ERR;
}


LOCAL gint32 read_system_from_eeprom(FRU_PROPERTY_S *fru_prop, SYSTEM_INFO_U *system_info, gulong update_sys_syn_semid)
{
    guint32 crc_retry_time = 0;
    const guint32 retry_max_count = 5;
    SYSTEM_AREA_S sys_area = { 0 };
    while (crc_retry_time < retry_max_count) { 
        gint32 ret =
            frudev_read_from_eeprom(fru_prop->frudev_handle, E2P_SYSTEM_AREA_OFF, &sys_area, sizeof(SYSTEM_AREA_S));
        if (fru_prop->frudev_type == STORAGE_TYPE_E2P_DOUBLE && ret != RET_OK) {
            ret = dfl_block_read(fru_prop->frudev_handle, PROPERTY_FRUDEV_BACK_STORAGE_LOC, E2P_SYSTEM_AREA_OFF,
                sizeof(SYSTEM_AREA_S), &sys_area);
        }
        if (ret != READ_ELABEL_SUCCESS) {
            debug_log(DLOG_ERROR, "%s call read system area elable failed, error = %d.",
                dfl_get_object_name(fru_prop->frudev_handle), ret);
            return READ_ELABEL_FAILED;
        }

        guint16 crc_res = arith_make_crc_checksum(0, (guchar *)&sys_area,
            SYS_AREA_ELABEL_NUM * sizeof(DATA_LENGTH_S) + sizeof(gboolean));
        if (sys_area.crc == crc_res) {
            break;
        }
        debug_log(DLOG_ERROR, "%s check system elabel data crc failed, real crc value:%d, cal crc value:%d.",
            dfl_get_object_name(fru_prop->fru_handle), crc_res, sys_area.crc);
        ++crc_retry_time;
        vos_task_delay(200); 
    }

    if (crc_retry_time < retry_max_count) {
        errno_t safe_fun_ret = memcpy_s(system_info, sizeof(SYSTEM_INFO_U), &sys_area, sizeof(SYSTEM_AREA_S));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        }

        if ((strcmp((const gchar *)fru_prop->product_name,
            (const gchar *)system_info->system_area_st.sys_product_name.data)) &&
            (!system_info->system_area_st.is_customize)) {
            debug_log(DLOG_ERROR, "%s disable to customize system product name(%s).",
                dfl_get_object_name(fru_prop->fru_handle), system_info->system_area_st.sys_product_name.data);
            (void)vos_thread_sem4_v(update_sys_syn_semid);
            return DISABLE_CUSTOMIZE;
        }
    } else {
        
        (void)memset_s(system_info, sizeof(SYSTEM_AREA_S), 0, sizeof(SYSTEM_AREA_S));
        errno_t safe_fun_ret = strncpy_s((gchar *)system_info->system_area_st.sys_product_name.data, FRU_MAX_ELABEL_LEN,
            fru_prop->product_name, strlen(fru_prop->product_name));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        }
        system_info->system_area_st.sys_product_name.len = strlen(fru_prop->product_name);
    }
    return READ_ELABEL_SUCCESS;
}


LOCAL gint32 elabel_init_system_info(guint16 system_desc_offset, FRU_PROPERTY_S *fru_prop, SYSTEM_INFO_U *system_info,
    gulong update_sys_syn_semid, const FRU_INFO_S *fru_info)
{
    if (system_info == NULL || fru_prop == NULL || fru_info == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return RET_OK;
    }

    
    if (fru_prop->is_cont_sys_area == 0 || (is_atlantic_control_board() == RET_OK)) {
        return RET_OK;
    }

    
    SYSTEM_AREA_S sys_area = { 0 };
    if (fru_prop->frudev_type == STORAGE_TYPE_FILE) {
        gint32 ret = frudev_read_file_withoffset(fru_prop->frudev_handle, system_desc_offset, &sys_area,
            sizeof(SYSTEM_AREA_S));
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "call frudev_read_from_eeprom failed, error = %d.", ret);
            return ret;
        }
    } else {
        gint32 ret = read_system_from_eeprom(fru_prop, system_info, update_sys_syn_semid);
        if (ret == DISABLE_CUSTOMIZE) {
            return RET_OK;
        } else if (ret != READ_ELABEL_SUCCESS) {
            debug_log(DLOG_ERROR, "%s read system area elable failed, error = %d.",
                dfl_get_object_name(fru_prop->frudev_handle), ret);
            return ret;
        }
    }

    __update_system_data(system_info, fru_info);

    set_one_elabel_property(fru_prop->elabel_handle, PROPERTY_ELABEL_SYS_SERIAL_NUM,
        (const gchar *)system_info->system_area_st.sys_sn.data);
    set_one_elabel_property(fru_prop->elabel_handle, PROPERTY_ELABEL_SYS_NAME,
        (const gchar *)system_info->system_area_st.sys_product_name.data);
    return RET_OK;
}


LOCAL gint32 tc_elabel_init_system_info(guint16 system_desc_offset, FRU_PROPERTY_S *fru_prop,
    SYSTEM_INFO_U *system_info, gulong update_sys_syn_semid, const FRU_INFO_S *fru_info)
{
    if (system_info == NULL || fru_prop == NULL || fru_info == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return RET_OK;
    }

    
    if (fru_prop->is_cont_sys_area == 0) {
        return RET_OK;
    }

    
    TC_SYSTEM_DESC_S sys_desc = { 0 };
    gint32 ret =
        frudev_read_from_eeprom(fru_prop->frudev_handle, system_desc_offset, &sys_desc, sizeof(TC_SYSTEM_DESC_S));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "call frudev_read_from_eeprom failed, error = %d.", ret);
        return ret;
    }

    if (!tc_update_system_data(sys_desc, fru_prop, system_info, update_sys_syn_semid, fru_info)) {
        return RET_OK;
    }

    set_one_elabel_property(fru_prop->elabel_handle, PROPERTY_ELABEL_SYS_SERIAL_NUM,
        (const gchar *)system_info->tc_system_desc_st.sys_sn);
    set_one_elabel_property(fru_prop->elabel_handle, PROPERTY_ELABEL_SYS_NAME,
        (const gchar *)system_info->tc_system_desc_st.sys_product_name);
    return RET_OK;
}


gint32 init_system_info(FRU_PRIV_PROPERTY_S *fru_priv)
{
    if (fru_priv == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return RET_ERR;
    }

    if (fru_priv->eeprom_format == EEPROM_FORMAT_TIANCHI) {
        return tc_elabel_init_system_info(fru_priv->area_offsets->system_desc_offset, &(fru_priv->fru_property),
            fru_priv->system_info, fru_priv->update_sys_syn_semid, fru_priv->fru_info);
    }

    return elabel_init_system_info(fru_priv->area_offsets->system_desc_offset, &(fru_priv->fru_property),
        fru_priv->system_info, fru_priv->update_sys_syn_semid, fru_priv->fru_info);
}