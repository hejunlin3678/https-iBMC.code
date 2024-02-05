

#include "protocol.h"
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "common_macro.h"

#define READ_PS_FRU_FAIL_TIMES 20

LOCAL guint16 __assemble_internal(guint8 *out_put, guint32 out_size, FRU_INTERNAL_AREA_S *internal);
LOCAL guint16 __assemble_chassis(guint8 *out_put, guint32 out_size, FRU_CHASSIS_AREA_S *chassis);
LOCAL guint16 __assemble_board(guint8 *out_put, guint32 out_size, FRU_BOARD_AREA_S *board);
LOCAL guint16 __assemble_product(guint8 *out_put, guint32 out_size, FRU_PRODUCT_AREA_S *product);
LOCAL gboolean __get_fru_manufacturer_info(C51_FRU_INFO_S *c51_fru_info, FRU_PRODUCT_AREA_S *product_area);
LOCAL gboolean __get_fru_model_info(C51_FRU_INFO_S *c51_fru_info, FRU_PRODUCT_AREA_S *product_area);
LOCAL gboolean __get_fru_version_info(guint8 ps_id, C51_FRU_INFO_S *c51_fru_info, FRU_PRODUCT_AREA_S *product_area);
LOCAL gboolean __get_fru_serial_number_info(C51_FRU_INFO_S *c51_fru_info, FRU_PRODUCT_AREA_S *product_area);
LOCAL gboolean get_fru_part_number_info(C51_FRU_INFO_S *c51_fru_info, FRU_PRODUCT_AREA_S *product_area);
LOCAL void __get_fru_data_info(guint8 ps_id, C51_FRU_INFO_S *c51_fru_info, FRU_PRODUCT_AREA_S *product_area);
LOCAL guint16 __assemble_multi(guint8 *out_put, gint32 out_size, GList *multi_record);


LOCAL guint16 __assemble_internal(guint8 *out_put, guint32 out_size, FRU_INTERNAL_AREA_S *internal)
{
    if (out_put == NULL || internal == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return 0;
    }

    if (out_size < sizeof(guint16)) {
        debug_log(DLOG_ERROR, "%s out of bounds!", __FUNCTION__);
        return 0;
    }

    *out_put = internal->len & 0xff;
    *(out_put + 1) = (internal->len & 0xff00) >> OFFSET_8_BIT;

    if (memmove_s(out_put + sizeof(guint16), out_size - sizeof(guint16), internal->data, internal->len) != EOK) {
        return 0;
    }

    return MULITPLE_PARA * (((internal->len + sizeof(guint16)) / MULITPLE_PARA) +
        (((internal->len + sizeof(guint16)) % MULITPLE_PARA) > 0 ? 1 : 0));
}


LOCAL guint16 __assemble_chassis(guint8 *out_put, guint32 out_size, FRU_CHASSIS_AREA_S *chassis)
{
    if (out_put == NULL || chassis == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return 0;
    }

    guint16 chassis_offset = 0;
    out_put[chassis_offset++] = chassis->area_ver;
    out_put[chassis_offset++] = 0; 
    out_put[chassis_offset++] = chassis->chassis_type;

    FILL_BUF_FROM_LABEL(out_put, out_size, chassis->chassis_part_num, chassis_offset);
    FILL_BUF_FROM_LABEL(out_put, out_size, chassis->chassis_serial_num, chassis_offset); 
    
    GList *tmp_chassis_list = chassis->custom_info;

    while (tmp_chassis_list) {
        GList *next = tmp_chassis_list->next;
        FRU_ELABEL_FORMAT_S *tmp_elabel = (FRU_ELABEL_FORMAT_S *)tmp_chassis_list->data;
        if (tmp_elabel->len == 1) {
            tmp_elabel->len = 2;
            tmp_elabel->content[1] = 0;
        }

        FILL_BUF_FROM_LABEL_POINTER(out_put, out_size, tmp_elabel, chassis_offset);
        tmp_chassis_list = next;
    }

    FILL_PAD(out_put, out_size, chassis_offset, out_put);

    out_put[CHASSIS_LEN_OFFSET] = chassis_offset / MULITPLE_PARA;
    out_put[chassis_offset - 1] = vos_calc_check_sum(out_put, chassis_offset - 1);

    return chassis_offset;
}


LOCAL guint16 __assemble_board(guint8 *out_put, guint32 out_size, FRU_BOARD_AREA_S *board)
{
    if (out_put == NULL || board == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return 0;
    }

    guint16 offset = 0;
    out_put[offset++] = board->area_ver;
    out_put[offset++] = 0; 
    out_put[offset++] = board->lang;

    if (out_size <= offset) {
        return offset;
    }

    OBJ_HANDLE obj_handle = 0;
    guint8 elabel_format = 0;
    (void)dfl_get_object_handle(OBJECT_BMC, &obj_handle);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_BMC_ELABELMANUFORMAT, &elabel_format);
    guint8 mfg_format_len = 3;  // 旧格式长度为3
    if (elabel_format == 1) {
        mfg_format_len = 4;     // 新格式长度为4
    }
    errno_t safe_fun_ret =
        memmove_s(&out_put[offset], out_size - offset, board->mfg_date_time, mfg_format_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return 0;
    }
    offset += mfg_format_len;

    FILL_BUF_FROM_LABEL(out_put, out_size, board->board_manufacturer, offset);
    FILL_BUF_FROM_LABEL(out_put, out_size, board->board_product_name, offset);
    FILL_BUF_FROM_LABEL(out_put, out_size, board->board_serial_num, offset);
    FILL_BUF_FROM_LABEL(out_put, out_size, board->board_part_num, offset);
    FILL_BUF_FROM_LABEL(out_put, out_size, board->file_id, offset);

    
    GList *tmp_list = board->extension_label;

    while (tmp_list) {
        GList *next = tmp_list->next;
        FRU_ELABEL_FORMAT_S *tmp_elabel = (FRU_ELABEL_FORMAT_S *)tmp_list->data;

        // 防止出现C1结束符
        if (tmp_elabel->len == 1) {
            tmp_elabel->len = 2; 
            tmp_elabel->content[1] = 0;
        }

        FILL_BUF_FROM_LABEL_POINTER(out_put, out_size, tmp_elabel, offset);
        tmp_list = next;
    }

    
    FILL_PAD(out_put, out_size, offset, out_put);

    out_put[BOARD_LEN_OFFSET] = offset / MULITPLE_PARA;
    out_put[offset - 1] = vos_calc_check_sum(out_put, offset - 1);

    return offset;
}


LOCAL guint16 __assemble_product(guint8 *out_put, guint32 out_size, FRU_PRODUCT_AREA_S *product)
{
    if (out_put == NULL || product == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return 0;
    }

    guint16 product_offset = 0;
    out_put[product_offset++] = product->area_ver;
    out_put[product_offset++] = 0; 
    out_put[product_offset++] = product->lang;

    FILL_BUF_FROM_LABEL(out_put, out_size, product->product_manufacturer, product_offset);
    FILL_BUF_FROM_LABEL(out_put, out_size, product->product_name, product_offset);
    FILL_BUF_FROM_LABEL(out_put, out_size, product->product_part_num, product_offset);
    FILL_BUF_FROM_LABEL(out_put, out_size, product->product_version, product_offset);
    FILL_BUF_FROM_LABEL(out_put, out_size, product->product_serial_num, product_offset);
    FILL_BUF_FROM_LABEL(out_put, out_size, product->product_asset_tag, product_offset);
    FILL_BUF_FROM_LABEL(out_put, out_size, product->file_id, product_offset);
    
    GList *tmp_product_list = product->custom_info;

    while (tmp_product_list) {
        GList *next = tmp_product_list->next;
        FRU_ELABEL_FORMAT_S *tmp_elabel = (FRU_ELABEL_FORMAT_S *)tmp_product_list->data;
        if (tmp_elabel->len == 1) {
            tmp_elabel->len = 2;
            tmp_elabel->content[1] = 0;
        }

        FILL_BUF_FROM_LABEL_POINTER(out_put, out_size, tmp_elabel, product_offset);
        tmp_product_list = next;
    }

    
    FILL_PAD(out_put, out_size, product_offset, out_put);

    out_put[PRODUCT_LEN_OFFSET] = product_offset / MULITPLE_PARA;
    out_put[product_offset - 1] = vos_calc_check_sum(out_put, product_offset - 1);

    return product_offset;
}

LOCAL gboolean __get_fru_manufacturer_info(C51_FRU_INFO_S *c51_fru_info, FRU_PRODUCT_AREA_S *product_area)
{
    if (!g_utf8_validate((gchar *)(c51_fru_info->manufacturer.content), FRU_FILED_INVALID, NULL)) {
        product_area->product_manufacturer.len = (guint8)strlen("N/A");
        product_area->product_manufacturer.type = CHAR_TYPE;
        (void)strncpy_s((gchar *)(product_area->product_manufacturer.content), FRU_MAX_ELABEL_LEN, "N/A",
            strlen("N/A"));

        return TRUE;
    }

    product_area->product_manufacturer.len = c51_fru_info->manufacturer.len;
    product_area->product_manufacturer.type = CHAR_TYPE;
    errno_t safe_fun_ret = strncpy_s((gchar *)(product_area->product_manufacturer.content), FRU_MAX_ELABEL_LEN,
        (gchar *)(c51_fru_info->manufacturer.content), c51_fru_info->manufacturer.len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return FALSE;
    }

    return TRUE;
}

LOCAL gboolean __get_fru_model_info(C51_FRU_INFO_S *c51_fru_info, FRU_PRODUCT_AREA_S *product_area)
{
    if (!g_utf8_validate((gchar *)(c51_fru_info->model.content), FRU_FILED_INVALID, NULL)) {
        product_area->product_name.len = (guint8)strlen("N/A");
        product_area->product_name.type = CHAR_TYPE;
        (void)strncpy_s((gchar *)(product_area->product_name.content), FRU_MAX_ELABEL_LEN, "N/A", strlen("N/A"));

        return TRUE;
    }

    product_area->product_name.len = c51_fru_info->model.len;
    product_area->product_name.type = CHAR_TYPE;
    errno_t safe_fun_ret = strncpy_s((gchar *)(product_area->product_name.content), FRU_MAX_ELABEL_LEN,
        (gchar *)(c51_fru_info->model.content), c51_fru_info->model.len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return FALSE;
    }

    return TRUE;
}

LOCAL gint32 fru_get_psu_fw_ver(guchar ps_id, gchar *fw_ver, guint32 ver_len)
{
    gint32 ret;
    OBJ_HANDLE onepower = 0;

    ret = dal_get_specific_object_byte(CLASS_NAME_PS, PROTERY_PS_INDEX, ps_id, &onepower);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s get %s by ps id[%u] failed[%d].", __FUNCTION__, CLASS_NAME_PS, ps_id, ret);
        return RET_ERR;
    }

    ret = dal_get_property_value_string(onepower, PROTERY_PS_FWVERSION, fw_ver, ver_len);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s %s get product_version failed[%d].", __FUNCTION__, dfl_get_object_name(onepower),
            ret);
        return RET_ERR;
    }
    debug_log(DLOG_DEBUG, "%s %s get product version:%s.", __FUNCTION__, dfl_get_object_name(onepower), fw_ver);

    return RET_OK;
}

LOCAL gboolean psu_fru_product_ver_customized(void)
{
    gint32 ret;
    OBJ_HANDLE syspower = 0;
    guint32 psu_fru_config;

    ret = dal_get_object_handle_nth(CLASS_NAME_AMMETER, 0, &syspower);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s get %s handle failed[%d].", __FUNCTION__, CLASS_NAME_AMMETER, ret);
        return FALSE;
    }

    ret = dal_get_property_value_uint32(syspower, PROPERTY_PSU_FRU_CONFIG, &psu_fru_config);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s get %s value failed[%d].", __FUNCTION__, PROPERTY_PSU_FRU_CONFIG, ret);
        return FALSE;
    }

    
    return (psu_fru_config & PSU_FRU_PRODUCT_VERSION_BIT_MAP);
}

LOCAL gboolean __get_fru_version_info(guint8 ps_id, C51_FRU_INFO_S *c51_fru_info, FRU_PRODUCT_AREA_S *product_area)
{
    gchar fw_ver[MAX_PROPERTY_VALUE_LEN] = {0};
    gchar *fw_ver_content = "N/A";
    guint8 fw_ver_len = (guint8)strlen("N/A");
    errno_t safe_fun_ret;

    if (psu_fru_product_ver_customized()) {
        if (fru_get_psu_fw_ver(ps_id, fw_ver, sizeof(fw_ver)) == RET_OK) {
            fw_ver_content = fw_ver;
            fw_ver_len = (guint8)strlen(fw_ver_content);
        }
    } else {
        
        if (g_utf8_validate((gchar *)(c51_fru_info->version.content), FRU_FILED_INVALID, NULL)) {
            fw_ver_content = (gchar *)(c51_fru_info->version.content);
            fw_ver_len = c51_fru_info->version.len;
        } else {
            debug_log(DLOG_ERROR, "%s:Get PSU%d ProductVersion failed.", __FUNCTION__, ps_id);
        }
    }
    product_area->product_version.len = fw_ver_len;
    product_area->product_version.type = CHAR_TYPE;
    safe_fun_ret =
        strncpy_s((gchar *)(product_area->product_version.content), FRU_MAX_ELABEL_LEN - 1, fw_ver_content, fw_ver_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return FALSE;
    }
    return TRUE;
}

LOCAL gboolean __get_fru_serial_number_info(C51_FRU_INFO_S *c51_fru_info, FRU_PRODUCT_AREA_S *product_area)
{
    if (!g_utf8_validate((gchar *)(c51_fru_info->sn.content), FRU_FILED_INVALID, NULL)) {
        product_area->product_serial_num.len = (guint8)strlen("N/A");
        product_area->product_serial_num.type = CHAR_TYPE;
        (void)strncpy_s((gchar *)(product_area->product_serial_num.content), FRU_MAX_ELABEL_LEN, "N/A", strlen("N/A"));

        return TRUE;
    }

    product_area->product_serial_num.len = c51_fru_info->sn.len;
    product_area->product_serial_num.type = CHAR_TYPE;
    errno_t safe_fun_ret = strncpy_s((gchar *)(product_area->product_serial_num.content), FRU_MAX_ELABEL_LEN,
        (gchar *)(c51_fru_info->sn.content), c51_fru_info->sn.len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return FALSE;
    }

    return TRUE;
}

LOCAL gboolean get_fru_part_number_info(C51_FRU_INFO_S *c51_fru_info, FRU_PRODUCT_AREA_S *product_area)
{
    if (!g_utf8_validate((gchar *)(c51_fru_info->part_num.content), FRU_FILED_INVALID, NULL)) {
        product_area->product_part_num.len = (guint8)strlen("N/A");
        product_area->product_part_num.type = CHAR_TYPE;
        (void)strncpy_s((gchar *)(product_area->product_part_num.content), FRU_MAX_ELABEL_LEN, "N/A", strlen("N/A"));
        return TRUE;
    }

    product_area->product_part_num.len = c51_fru_info->part_num.len;
    product_area->product_part_num.type = CHAR_TYPE;
    errno_t safe_fun_ret = strncpy_s((gchar *)(product_area->product_part_num.content), FRU_MAX_ELABEL_LEN,
        (gchar *)(c51_fru_info->part_num.content), c51_fru_info->part_num.len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return FALSE;
    }

    return TRUE;
}


LOCAL void __get_fru_data_info(guint8 ps_id, C51_FRU_INFO_S *c51_fru_info, FRU_PRODUCT_AREA_S *product_area)
{
    if (c51_fru_info == NULL || product_area == NULL) {
        debug_log(DLOG_ERROR, "input parameter error null.");
        return;
    }

    
    if (!__get_fru_manufacturer_info(c51_fru_info, product_area)) {
        return;
    }

    
    if (!__get_fru_model_info(c51_fru_info, product_area)) {
        return;
    }

    
    if (!__get_fru_version_info(ps_id, c51_fru_info, product_area)) {
        return;
    }

    
    __get_fru_serial_number_info(c51_fru_info, product_area);

     
    get_fru_part_number_info(c51_fru_info, product_area);

    return;
}


LOCAL guint16 __assemble_multi(guint8 *out_put, gint32 out_size, GList *multi_record)
{
    if (out_put == NULL || multi_record == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return 0;
    }

    guint16 offset = 0;
    GList *tmp_list = multi_record;

    while (tmp_list) {
        GList *next = tmp_list->next;
        FRU_MULTI_AREA_S *tmp_multi = (FRU_MULTI_AREA_S *)tmp_list->data;
        errno_t safe_fun_ret = memcpy_s(out_put + offset, out_size - offset, tmp_multi,
            tmp_multi->header.len + sizeof(FRU_MULTI_HEADER_S));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        }
        offset += (guint16)(tmp_multi->header.len + sizeof(FRU_MULTI_HEADER_S));
        tmp_list = next;
    }

    return offset;
}


LOCAL gint32 __deal_fru_data(guint8 ps_id, PS_FRU_S ps_fru, FRU_FILE_S *fru_file)
{
    OBJ_HANDLE syspower_handle = 0;
    gint32 ret = dal_get_object_handle_nth(CLASS_NAME_AMMETER, 0, &syspower_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get syspower obj_handle failed.");
        return RET_OK;
    }

    guint8 fru_type = 0;
    ret = dal_get_property_value_byte(syspower_handle, PROTERY_SYSPOWER_FRU_SOURCE, &fru_type);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get fru source type failed.");
        return RET_OK;
    }

    if (fru_type == FRU_SOURCE_FROM_E2P) {
        errno_t safe_fun_ret = memmove_s(fru_file->frudata, FRU_AREA_MAX_LEN, ps_fru.fru_data, FRU_DATA_LEN);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        }

        return RET_OK;
    }

    
    guint8 output_data[FRU_DATA_LEN] = { 0 };
    (void)transform_fru_data(ps_id, ps_fru.fru_data, output_data, sizeof(output_data));
    errno_t safe_fun_ret = memmove_s(fru_file->frudata, FRU_AREA_MAX_LEN, output_data, FRU_DATA_LEN);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }

    return RET_OK;
}


void fillin_fru_head(FRU_FILE_S *fru_file, gint32 data_length)
{
    
    (void)strncpy_s((gchar *)&(fru_file->header.fru_head), sizeof(fru_file->header.fru_head), "FRU", strlen("FRU"));
    fru_file->header.fru_head.len = data_length;
    fru_file->header.fru_head.crc = arith_make_crc_checksum(0, (const guchar *)fru_file->frudata, data_length);
}

void fillin_eeprom_fru_head(guint8 eeprom_format, FRU_INFO_S *fru_info, FRU_FILE_S *fru_file, gint32 data_length)
{
    if (eeprom_format == EEPROM_FORMAT_TIANCHI) {
        return;
    }

    
    (void)strncpy_s((gchar *)&(fru_file->header.fru_head), sizeof(fru_file->header.fru_head), "FRU", strlen("FRU"));
    fru_file->header.fru_head.len = data_length;
    fru_file->header.fru_head.crc = arith_make_crc_checksum(0, (const guchar *)fru_file->frudata, data_length);
}


void create_fru_file(FRU_PROPERTY_S *fru_prop, FRU_FILE_S *fru_file)
{
    if (fru_file == NULL || fru_prop == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return;
    }

    FRU_INFO_S fru_info = { 0 };
    fru_info.has_board = TRUE;
    fru_info.has_product = TRUE;
    fru_info.board.area_ver = DEFAULT_AREA_VER;
    fru_info.board.board_manufacturer.len = (guint8)strlen(DEFAULT_MFG_HUAWEI);
    fru_info.board.board_manufacturer.type = CHAR_TYPE;
    (void)strncpy_s((gchar *)(fru_info.board.board_manufacturer.content), FRU_MAX_ELABEL_LEN, DEFAULT_MFG_HUAWEI,
        strlen(DEFAULT_MFG_HUAWEI));

    fru_info.board.file_id.len = (guint8)strlen(fru_prop->fru_ver);
    fru_info.board.file_id.type = CHAR_TYPE;
    errno_t safe_fun_ret = strncpy_s((gchar *)(fru_info.board.file_id.content), FRU_MAX_ELABEL_LEN, fru_prop->fru_ver,
        FRU_MAX_ELABEL_LEN - 1);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }

    fru_info.product.area_ver = DEFAULT_AREA_VER;
    fru_info.product.product_manufacturer.len = (guint8)strlen(DEFAULT_MFG_HUAWEI);
    fru_info.product.product_manufacturer.type = CHAR_TYPE;
    (void)strncpy_s((gchar *)(fru_info.product.product_manufacturer.content), FRU_MAX_ELABEL_LEN, DEFAULT_MFG_HUAWEI,
        strlen(DEFAULT_MFG_HUAWEI));

    fru_info.product.file_id.len = (guint8)strlen(fru_prop->fru_ver);
    fru_info.product.file_id.type = CHAR_TYPE;
    safe_fun_ret = strncpy_s((gchar *)(fru_info.product.file_id.content), FRU_MAX_ELABEL_LEN, fru_prop->fru_ver,
        FRU_MAX_ELABEL_LEN - 1);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }

    gint32 ret =
        generate_fru_file((fru_prop->fru_offset == 0) ? EEPROM_FORMAT_V2 : EEPROM_FORMAT_TIANCHI, &fru_info, fru_file);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, " generate the %s fru file ret=%d", dfl_get_object_name(fru_prop->frudev_handle), ret);
    }

    return;
}


void init_default_fru_info(FRU_PROPERTY_S *fru_prop, FRU_INFO_S *fru_info)
{
    if (fru_prop == NULL || fru_info == NULL) {
        return;
    }

    fru_info->has_board = TRUE;
    fru_info->has_product = TRUE;
    fru_info->board.area_ver = DEFAULT_AREA_VER;
    fru_info->board.board_manufacturer.len = (guint8)strlen(DEFAULT_MFG_HUAWEI);
    fru_info->board.board_manufacturer.type = CHAR_TYPE;
    (void)strncpy_s((gchar *)(fru_info->board.board_manufacturer.content), FRU_MAX_ELABEL_LEN, DEFAULT_MFG_HUAWEI,
        strlen(DEFAULT_MFG_HUAWEI));

    fru_info->board.file_id.len = (guint8)strlen(fru_prop->fru_ver);
    fru_info->board.file_id.type = CHAR_TYPE;
    errno_t safe_fun_ret = strncpy_s((gchar *)(fru_info->board.file_id.content), FRU_MAX_ELABEL_LEN, fru_prop->fru_ver,
        FRU_MAX_ELABEL_LEN - 1);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }

    fru_info->product.area_ver = DEFAULT_AREA_VER;

    fru_info->product.product_manufacturer.len = (guint8)strlen(DEFAULT_MFG_HUAWEI);
    fru_info->product.product_manufacturer.type = CHAR_TYPE;
    (void)strncpy_s((gchar *)(fru_info->product.product_manufacturer.content), FRU_MAX_ELABEL_LEN, DEFAULT_MFG_HUAWEI,
        strlen(DEFAULT_MFG_HUAWEI));

    fru_info->product.file_id.len = (guint8)strlen(fru_prop->fru_ver);
    fru_info->product.file_id.type = CHAR_TYPE;
    safe_fun_ret = strncpy_s((gchar *)(fru_info->product.file_id.content), FRU_MAX_ELABEL_LEN, fru_prop->fru_ver,
        FRU_MAX_ELABEL_LEN - 1);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }

    return;
}

gint32 get_eeprom_format(OBJ_HANDLE frudev_handle, guint8 *eeprom_format)
{
    guint8 storage_type = 0;
    gint32 ret = dal_get_property_value_byte(frudev_handle, PROPERTY_FRUDEV_STORAGE_TYPE, &storage_type);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Failed to get %s StorageType, ret:%d.", dfl_get_object_name(frudev_handle), ret);
        return ret;
    }

    // StorageType为EEPROM类型才处理
    if (storage_type != STORAGE_TYPE_EEPROM) {
        *eeprom_format = EEPROM_FORMAT_V2;
        return RET_OK;
    }

    // 读取的Eeprom的前4个字节
    guint32 fru_header_tag = 0;
    ret = fru_block_read(frudev_handle, PROPERTY_FRUDEV_STORAGE_LOC, 0, sizeof(fru_header_tag), &fru_header_tag);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "read devname=%s head from eeprom fail. ret = %d", dfl_get_object_name(frudev_handle),
            ret);
        return ret;
    }

    guint32 tc_header_tag = 0;
    ret = fru_block_read(frudev_handle, PROPERTY_FRUDEV_STORAGE_LOC, TC_STANDARD_CODE_OFFSET, sizeof(tc_header_tag),
        &tc_header_tag);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "read devname=%s head from eeprom fail. ret = %d", dfl_get_object_name(frudev_handle),
            ret);
        return ret;
    }

    *eeprom_format = (fru_header_tag != FRU_HEADER_TAG && tc_header_tag == TC_STANDARD_CODE) ? EEPROM_FORMAT_TIANCHI :
                                                                                               EEPROM_FORMAT_V2;

    return RET_OK;
}


gint32 generate_fru_file(guint8 eeprom_format, FRU_INFO_S *fru_info, FRU_FILE_S *fru_file)
{
    if (fru_info == NULL || fru_file == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return RET_ERR;
    }

    (void)memset_s(fru_file, FRU_MAX_LEN, 0, FRU_MAX_LEN);
    FRU_INFO_HEADER_S *header = (eeprom_format == EEPROM_FORMAT_TIANCHI) ?
        (FRU_INFO_HEADER_S *)fru_file->header.frudata :
        (FRU_INFO_HEADER_S *)fru_file->frudata;
    header->version = FRU_FILE_VERSION;
    guint32 max_len = (eeprom_format == EEPROM_FORMAT_TIANCHI) ? FRU_MAX_LEN : FRU_AREA_MAX_LEN;

    guint16 offset = (guint16)sizeof(FRU_INFO_HEADER_S);

    if (fru_info->has_internal) {
        header->offset.internal = (offset / MULITPLE_PARA);
        offset += __assemble_internal((guint8 *)header + offset, max_len - offset, &(fru_info->internal));
    }

    if (fru_info->has_chassis) {
        header->offset.chassis = (offset / MULITPLE_PARA);
        offset += __assemble_chassis((guint8 *)header + offset, max_len - offset, &(fru_info->chassis));
    }

    if (fru_info->has_board) {
        header->offset.board = (offset / MULITPLE_PARA);
        offset += __assemble_board((guint8 *)header + offset, max_len - offset, &(fru_info->board));
    }

    if (fru_info->has_product) {
        header->offset.product = (offset / MULITPLE_PARA);
        offset += __assemble_product((guint8 *)header + offset, max_len - offset, &(fru_info->product));
    }

    if (fru_info->has_multi) {
        header->offset.multi = (offset / MULITPLE_PARA);
        offset += __assemble_multi((guint8 *)header + offset, max_len - offset, fru_info->multi_record);
    }

    header->checksum = vos_calc_check_sum((guint8 *)header, sizeof(FRU_INFO_HEADER_S) - 1);

    
    fillin_eeprom_fru_head(eeprom_format, fru_info, fru_file, offset);

    return RET_OK;
}


void transform_fru_data(guint8 ps_id, guint8 *fru_data, guint8 *output_data, guint32 output_size)
{
    if (fru_data == NULL) {
        debug_log(DLOG_ERROR, "frudata is null.");
        return;
    }

    C51_FRU_INFO_S *c51_fru_info = (C51_FRU_INFO_S *)fru_data;

    FRU_INFO_HEADER_S *header = (FRU_INFO_HEADER_S *)output_data;
    header->version = FRU_FILE_VERSION;
    guint16 offset = (guint16)sizeof(FRU_INFO_HEADER_S);

    FRU_PRODUCT_AREA_S product_area = { 0 };
    product_area.area_ver = DEFAULT_AREA_VER;

    __get_fru_data_info(ps_id, c51_fru_info, &product_area);
    
    header->offset.product = (offset / MULITPLE_PARA);
    guint16 internal_len = __assemble_product(output_data + offset, output_size - offset, &product_area);
    offset += internal_len;
    
    FRU_MULTI_AREA_S multi_area = { 0 };
    multi_area.header.len = (guint8)sizeof(PS_PROPERTY_TYPE_INFO_S);
    multi_area.header.format = 0x80; 

    PS_PROPERTY_TYPE_INFO_S *fru_data_info = (PS_PROPERTY_TYPE_INFO_S *)(multi_area.data);
    fru_data_info->overallcapacity = c51_fru_info->ps_rate;
    fru_data_info->input_type = (guint16)c51_fru_info->input_type;
    
    header->offset.multi = (offset / MULITPLE_PARA);
    errno_t safe_fun_ret = memcpy_s(output_data + offset, output_size - offset, (void *)&multi_area,
        multi_area.header.len + sizeof(FRU_MULTI_HEADER_S));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }

    return;
}


gint32 fru_block_write(OBJ_HANDLE obj_handle, const gchar *property_name, gint32 offset, gsize length,
    gconstpointer write_buf)
{
    if (write_buf == NULL) {
        debug_log(DLOG_ERROR, "%s param error.", __func__);
        return ERROR_CODE_WRITE_E2P_ERROR;
    }

    gint32 ret;
    for (guint32 i = 0; i < 5; i++) {
        ret = dfl_block_write(obj_handle, property_name, offset, length, write_buf);
        if (ret == DFL_OK) {
            return RET_OK;
        }

        vos_task_delay(200);
    }

    debug_log(DLOG_ERROR, "write %s.%s eeprom failed!(ret:%d).", dfl_get_object_name(obj_handle), property_name, ret);
    return ERROR_CODE_WRITE_E2P_ERROR;
}


gint32 fru_block_read(OBJ_HANDLE obj_handle, const gchar *property_name, gint32 offset, gsize length, gpointer out_data)
{
    if (out_data == NULL) {
        debug_log(DLOG_ERROR, "[%s]param error.", dfl_get_object_name(obj_handle));
        return ERROR_CODE_READ_E2P_ERROR;
    }

    gint32 ret;
    for (guint32 i = 0; i < 5; i++) {
        ret = dfl_block_read(obj_handle, property_name, offset, length, out_data);
        if (ret == DFL_OK) {
            return RET_OK;
        }

        vos_task_delay(200);
    }

    debug_log(DLOG_ERROR, "read %s.%s eeprom failed!(ret:%d).", dfl_get_object_name(obj_handle), property_name, ret);
    return ERROR_CODE_READ_E2P_ERROR;
}


gint32 read_fru_info(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file, PS_GET_FRU get_ps_fru_callback)
{
    if (fru_file == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return ERROR_CODE_PARAMETER_INVALID;
    }

    gint32 ret;
    PS_FRU_S ps_fru = { 0 };
    guint8 retry_times = 0;
    PS_FRU_S ps_fru_bak = { 0 };
    guint32 match_count = 0;

    guint8 ps_id = 0;
    (void)dal_get_property_value_byte(object_handle, PROPERTY_FRUDEV_STORAGE_LOC, &ps_id);

#ifndef ARM64_HI1711_ENABLED
    gint32 hi51_fd = 0;
    ret = ps_51_open(&hi51_fd);
    if (ret != PSM_OK) {
        debug_log(DLOG_ERROR, "call ps_51_open failed, error = %d.", ret);
        return ret;
    }
#endif

    
    while (retry_times < READ_PS_FRU_FAIL_TIMES) {
        vos_task_delay(TASK_DELAY_ONE_SECOND);
#ifdef ARM64_HI1711_ENABLED
        ret = get_ps_fru_callback(ps_id, &ps_fru);
#else
        (void)get_ps_fru_callback;
        ret = ps_get_fru(hi51_fd, ps_id, &ps_fru);
#endif
        if (ret != RET_OK) {
            retry_times++;
            continue;
        }

        
        if (memcmp((void *)(&ps_fru_bak), (void *)(&ps_fru), sizeof(PS_FRU_S)) == 0) {
            match_count++;
        } else {
            match_count = 0;
            (void)memcpy_s((void *)&ps_fru_bak, sizeof(PS_FRU_S), (void *)&ps_fru, sizeof(PS_FRU_S));
        }

        if (match_count >= 5) { 
            break;
        }

        retry_times++;
    }

#ifndef ARM64_HI1711_ENABLED
    ps_51_close(hi51_fd);
#endif

    if (retry_times == READ_PS_FRU_FAIL_TIMES) {
        debug_log(DLOG_ERROR, "call ps_get_fru failed, error = %d!(ps_id:%d).", ret, ps_id);
        return ret;
    }

    (void)__deal_fru_data(ps_id, ps_fru, fru_file);
    fillin_fru_head(fru_file, FRU_DATA_LEN);

    return RET_OK;
}


gint32 check_fru_file(guint8 eeprom_format, const FRU_FILE_S *fru_file)
{
    if (fru_file == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return RET_ERR;
    }

    
    if (eeprom_format == EEPROM_FORMAT_TIANCHI) {
        return RET_OK;
    }

    
    if (strcmp((const gchar *)(&fru_file->header.fru_head), "FRU") != 0) {
        debug_log(DLOG_ERROR, "FRU header is error.");
        return ERROR_CODE_HEADER_ERROR;
    }

    if (fru_file->header.fru_head.len > sizeof(fru_file->frudata)) {
        debug_log(DLOG_ERROR, " the fru data len=%d is great than buffer len=%" G_GSIZE_FORMAT,
            fru_file->header.fru_head.len, (gsize)sizeof(fru_file->frudata));
        return RET_ERR;
    }

    guint16 l_crc = arith_make_crc_checksum(0, fru_file->frudata, fru_file->header.fru_head.len);
    if (l_crc != fru_file->header.fru_head.crc) {
        debug_log(DLOG_ERROR, "FRU FILE CRC ERROR(calc=%d, file=%d).", l_crc, fru_file->header.fru_head.crc);
        return ERROR_CODE_CRC_DIFFERENT;
    }

    return RET_OK;
}