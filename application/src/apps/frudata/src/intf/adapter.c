

#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "pme_app/dalib/dal_obj_priv_data.h"
#include "intf/adapter.h"
#include "component/component.h"
#include "component/connector.h"
#include "component/base.h"
#include "component/mezz.h"
#include "elabel/elabel.h"
#include "elabel/base.h"
#include "prepare_fru.h"
#include "media/e2p_large.h"
#include "media/e2p_compact.h"

#define WRITE_BMC_FRU_COUNT 5
#define DATA_SYNC_ERR_LOG_INTERVAL (30UL * 1000)
#define EEP_MAX_SIZE (0x4000)
#define OPERATION_LOG_BMC "BMC"
#define SET_EEP_DATA_INPUT_LEN 2

gint32 method_set_ver_change(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "Set version change failed, incorrect input parameter");
        return RET_ERR;
    }

    guint8 cause = (guint8)g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));

    send_ver_change_evt(cause);
    method_operation_log(caller_info, NULL, "Set version change (cause %d) successfully", cause);
    return RET_OK;
}


gint32 method_component_set_health(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "Set health failed, incorrect input parameter");
        return RET_ERR;
    }

    guint8 health = (guint8)g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));

    (void)dal_set_property_value_byte(object_handle, PROPERTY_COMPONENT_HEALTH, health, DF_NONE);
    return RET_OK;
}

gint32 method_fru_elabel_syn_elable(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    TASKID task_id = 0;

    
    (void)vos_task_create(&task_id, "FAinit", (TASK_ENTRY)on_fru_obj_add_event, (void *)object_handle,
        DEFAULT_PRIORITY);

    return RET_OK;
}

gint32 method_elabel_set_product_assettag(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s The input_list is null.", __FUNCTION__);
        return RET_ERR;
    }

    
    GSList *obj_list = NULL;
    (void)dfl_get_object_list(CLASS_FRU, &obj_list);

    guint8 product_ref_fruid = MAX_FRU_ID;
    OBJ_HANDLE ref_handle = 0;
    for (GSList *obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        gint32 ret =
            dfl_get_referenced_object((OBJ_HANDLE)obj_note->data, PROPERTY_FRU_ELABEL_PRODUCT_REF, &ref_handle);
        if (ret == DFL_OK) {
            (void)dal_get_property_value_byte((OBJ_HANDLE)obj_note->data, PROPERTY_FRU_ID, &product_ref_fruid);
            break;
        }
    }
    g_slist_free(obj_list);

    
    guint8 fruid = (g_slist_length(input_list) == 2) ? g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 1)) :
                                                       FRU_ID_MAINBOARD;

    gsize assettag_len = 0;
    const gchar *assettag = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 0), &assettag_len);
    if (assettag == NULL) {
        method_operation_log(caller_info, NULL, "Write fru%d E-label data failed", fruid);
        debug_log(DLOG_ERROR, "%s Invalid input_list, the assettag is null.", __FUNCTION__);
        return RET_ERR;
    }

    gint32 ret = vos_check_incorrect_char(assettag, DEVICE_LOCATION_LIMIT_CHARACTER, assettag_len);
    if (ret != RET_OK) {
        method_operation_log(caller_info, NULL, "Write fru%d E-label data failed", fruid);
        debug_log(DLOG_ERROR, "Input %s(cur:%s,don't have:%s) invalid!", PROPERTY_ELABEL_PRODUCT_ASSET_TAG, assettag,
            DEVICE_LOCATION_LIMIT_CHARACTER);
        return RET_ERR;
    }

    ret = update_elabel_product_assettag(caller_info, fruid, assettag);
    if ((product_ref_fruid != 0xff) && (ret == RET_OK)) {
        ret = update_elabel_product_assettag(caller_info, product_ref_fruid, assettag);
    }

    (void)dal_set_property_value_string(object_handle, PROPERTY_ELABEL_PRODUCT_ASSET_TAG, assettag, DF_NONE);

    return ret;
}

gint32 method_fru_set_mezzcard_macaddr(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret = RET_ERR;
    
    if (!is_fru_initialized(object_handle)) {
        debug_log(DLOG_ERROR, "FRU(%s) has not initialized.", dfl_get_object_name(object_handle));
        goto ERR_EXIT;
    }

    
    guint8 fruid = MAX_FRU_ID;
    (void)dal_get_property_value_byte(object_handle, PROPERTY_FRU_ID, &fruid);

    OBJ_HANDLE mezz_obj = 0;
    (void)dal_get_specific_object_byte(CLASS_MEZZCARD_NAME, PROPERTY_MEZZCARD_FRU_ID, fruid, &mezz_obj);

    guchar mac_addr[MAX_MAC_NUM][MAX_MAC_STR_LEN] = {0};
    guint8 mac_len[MAX_MAC_NUM] = {0};
    ret = read_mezz_card_mac_addr(mezz_obj, PROPERTY_MEZZCARD_MAC_ADDR, mac_addr, mac_len, MAX_MAC_NUM);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "read_mezz_card_mac_addr failed , ret %d ", ret);
        goto ERR_EXIT;
    }

    MACADDR_AREA_S mac_data[MAX_MAC_NUM] = {0};
    for (gint32 i = 0; i < MAX_MAC_NUM; i++) {
        if (mac_len[i] != 0) {
            mac_data[i].len = mac_len[i] + 2;
            mac_data[i].mac_id = i + 1;
            mac_data[i].interface_type = 2;
            errno_t safe_fun_ret = memmove_s(mac_data[i].data, sizeof(mac_data[i].data), mac_addr[i], mac_len[i]);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
            mac_data[i].crc = arith_make_crc_checksum(0, (const guchar *)&mac_data[i].len,
                MIN(sizeof(guint16) + mac_data[i].len, sizeof(MACADDR_AREA_S) - sizeof(guint16)));
        }
    }

    FRU_PRIV_PROPERTY_S *fru_priv_data = NULL;
    ret = get_obj_priv_data(object_handle, (gpointer *)(&fru_priv_data));
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "call dfl_get_binded_object failed, error = %d.", ret);
        goto ERR_EXIT;
    }

    ret = frudev_write_to_eeprom(fru_priv_data->fru_property.frudev_handle,
        fru_priv_data->area_offsets->mezz_mac_offset, mac_data, MAX_MAC_STR_LEN * MAX_MAC_NUM);
    put_obj_priv_data(object_handle, (gpointer)fru_priv_data);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "frudev_write_to_eeprom failed, error = %d.", ret);
        goto ERR_EXIT;
    }

ERR_EXIT:
    if (ret != RET_OK) {
        maintenance_log_v2(MLOG_ERROR, FC_BMC_MEZZ_MAC_FAIL, "Write mezzcard MAC to eeprom fail!\n");
    }
    return ret;
}

gint32 method_fru_init_from_mcu(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    if (object_handle == 0) {
        debug_log(DLOG_ERROR, "%s: fru object handle is NULL", __FUNCTION__);
        return RET_ERR;
    }

    if (is_fru_initialized(object_handle)) {
        return RET_OK;
    }

    return init_fru(object_handle, NULL);
}

LOCAL gint32 update_ocp_pcb_ver(OBJ_HANDLE object_handle)
{
#define ELABEL_EXTEND_AREA_COUNT 3
#define PCB_VER_LEN 3
    if (object_handle == 0) {
        debug_log(DLOG_ERROR, "%s:parameter is NULL", __FUNCTION__);
        return RET_ERR;
    }
    gint32 ret;
    gchar buf[PCB_VER_LEN] = {'.', 'A', '\0'};
    ELABEL_INFO_S elabel_info_tmp;
    guint8 fru_type;
    ret = dal_get_property_value_byte(object_handle, PROPERTY_FRU_TYPE, &fru_type);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:get property %s failed,ret=%d.", __FUNCTION__, PROPERTY_FRU_TYPE, ret);
        return ret;
    }
    if (fru_type == FRU_TYPE_OCP3) {
        FRU_PRIV_PROPERTY_S *fru_priv = NULL;
        gint32 ret = dfl_get_binded_data(object_handle, (gpointer *)(&fru_priv));
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s: get binded object failed, error = %d.", __FUNCTION__, ret);
            return ret;
        }
        ret = get_elabel(fru_priv, &elabel_info_tmp);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:get elabel fail,ret=%d.", __FUNCTION__, ret);
            return ret;
        }
        ret = memcpy_s(buf, sizeof(buf), elabel_info_tmp.product.version.data, PCB_VER_LEN - 1);
        if (ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, ret);
            return ret;
        }
        ret = dal_set_property_value_string(object_handle, PROPERTY_FRU_PCB_VERSION, (const gchar *)buf, DF_NONE);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s:set ocp pcb version failed, ret: %d", __FUNCTION__, ret);
        }
    }
    debug_log(DLOG_ERROR, "%s:set ocp pcb version ret=%d", __FUNCTION__, ret);
    return ret;
}

LOCAL gboolean verify_eep_write_by_read_back(OBJ_HANDLE frudev_handle, guint32 offset, const guint8 *write_data,
    guint32 length)
{
    gint32 ret;
    guint8 *read_buffer = NULL;

    if ((length > EEP_MAX_SIZE) || (length == 0)) {
        return FALSE;
    }

    read_buffer = (guint8 *)g_malloc0(length);
    if (read_buffer == NULL) {
        debug_log(DLOG_ERROR, "%s: g_malloc0 failed", __FUNCTION__);
        return FALSE;
    }

    ret = frudev_read_from_eeprom(frudev_handle, offset, read_buffer, length);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: read from eep failed, error = %d.", __FUNCTION__, ret);
        g_free(read_buffer);
        return FALSE;
    }

    if (memcmp(read_buffer, write_data, length) != 0) {
        g_free(read_buffer);
        return FALSE;
    }

    g_free(read_buffer);
    return TRUE;
}

LOCAL gint32 _frudev_write_to_eeprom_by_divided(OBJ_HANDLE frudev_handle, guint32 offset, const void *write_buf,
    guint32 length)
{
#define EEPROM_DIVIDED_WRITE_SIZE 8
    gint32 ret;
    guint32 divided_block_count;
    guint32 i;
    guint32 relative_offset;
    guint32 block_offset;
    const guint8 *block_buffer = NULL;
    guint32 len_modulus;

    divided_block_count = length / EEPROM_DIVIDED_WRITE_SIZE;
    for (i = 0; i < divided_block_count; i++) {
        relative_offset = i * EEPROM_DIVIDED_WRITE_SIZE;
        block_offset = offset + relative_offset;
        block_buffer = ((const guint8 *)write_buf) + relative_offset;

        ret = frudev_write_to_eeprom(frudev_handle, block_offset, block_buffer, EEPROM_DIVIDED_WRITE_SIZE);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s: save to eeprom failed, ret=%d, offset=%u, size=%u", __FUNCTION__, ret,
                block_offset, EEPROM_DIVIDED_WRITE_SIZE);
            return ret;
        }
    }

    len_modulus = length % EEPROM_DIVIDED_WRITE_SIZE;
    if (len_modulus == 0) {
        return RET_OK;
    }

    debug_log(DLOG_INFO, "%s: save eeprom size not divided by 8, size=%ul", __FUNCTION__, length);
    relative_offset = i * EEPROM_DIVIDED_WRITE_SIZE;
    block_offset = offset + relative_offset;
    block_buffer = ((const guint8 *)write_buf) + relative_offset;

    ret = frudev_write_to_eeprom(frudev_handle, block_offset, block_buffer, len_modulus);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: save to eeprom failed, ret=%d, offset=%u, size=%u", __FUNCTION__, ret, block_offset,
            len_modulus);
        return ret;
    }
    return RET_OK;
}


gint32 method_ocp_fru_set_pcb_ver(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    if (object_handle == 0) {
        debug_log(DLOG_ERROR, "%s:parameter is NULL", __FUNCTION__);
        return RET_ERR;
    }
    gint32 ret;
    guint8 pcb_id;

    ret = dal_get_property_value_byte(object_handle, PROPERTY_FRU_PCBID, &pcb_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:get property %s failed,ret=%d.", __FUNCTION__, PROPERTY_FRU_PCBID, ret);
        return ret;
    }
    // 外购OCP 3.0卡不需要在web显示PcbVersion信息则配置为 0xfe
    if (pcb_id == 0xfe) {
        debug_log(DLOG_ERROR, "%s:unsupported fru type.", __FUNCTION__);
        return RET_ERR;
    }
    return update_ocp_pcb_ver(object_handle);
}


gint32 method_update_comp_dev_name(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    
    update_ps_ref_component_device_name(object_handle);

    return RET_OK;
}

gint32 method_read_cafe_record(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "The input_list is null.");
        return RET_ERR;
    }

    const gchar *cafe_key = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 1), NULL);
    if (cafe_key == NULL) {
        debug_log(DLOG_ERROR, "incorrect input_list, the cafe_key is null.");
        return RET_ERR;
    }

    guchar cafe_key_len = (guchar)strlen(cafe_key);
    if (cafe_key_len > CAFE_KEY_MAX_LEN) {
        debug_log(DLOG_ERROR, "invalid input_list, the cafe key len is too long, cafe_key_len is [%d]", cafe_key_len);
        return RET_ERR;
    }

    guchar fru_id = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));

    OBJ_HANDLE fru_handle = 0;
    gint32 ret = dfl_get_binded_object(CLASS_FRU, fru_id, &fru_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get object handle for class[%s] with alias[%d] failed, ret %d", CLASS_FRU, fru_id, ret);
        return ret;
    }

    
    FRU_PRIV_PROPERTY_S *priv_data = NULL;
    ret = get_obj_priv_data(fru_handle, (gpointer *)&priv_data);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get binded data for object[%s] failed, ret %d", dfl_get_object_name(fru_handle), ret);
        return ret;
    }

    gchar cafe_key_dup[CAFE_KEY_MAX_LEN] = {0};
    errno_t safe_fun_ret = memmove_s(cafe_key_dup, CAFE_KEY_MAX_LEN, cafe_key, cafe_key_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        put_obj_priv_data(object_handle, (gpointer)priv_data);
        return RET_ERR;
    }

    CAFE_DATA_S cafe_key_data = { 0 };
    cafe_key_data.data = (guchar *)cafe_key_dup;
    cafe_key_data.len = cafe_key_len;
    cafe_key_data.type = TYPE_8BIT_ASCII;

    CAFE_DATA_S *cafe_val_data = e2p_cafe_get_record_by_key(priv_data->cafe_info, &cafe_key_data);
    put_obj_priv_data(object_handle, (gpointer)priv_data);
    if (cafe_val_data == NULL) {
        debug_log(DLOG_ERROR, "get record[%s] from fru[%s] failed", cafe_key, dfl_get_object_name(fru_handle));
        return RET_ERR;
    }

    GVariant *out_var =
        g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, cafe_val_data->data, cafe_val_data->len, sizeof(guchar));
    if (out_var == NULL) {
        debug_log(DLOG_ERROR, "new fixed array for cafe data failed");
        return RET_ERR;
    }

    *output_list = g_slist_append(*output_list, out_var);

    return RET_OK;
}

gint32 method_write_cafe_record(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "The input_list is null.");
        return RET_ERR;
    }

    guchar fru_id = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    const gchar *cafe_key_str = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 1), NULL);

    gsize n_element = 0;
    const guchar *cafe_val = (const guchar *)g_variant_get_fixed_array((GVariant *)g_slist_nth_data(input_list, 2),
        &n_element, sizeof(guchar));

    OBJ_HANDLE fru_handle = 0;
    gint32 ret = dfl_get_binded_object(CLASS_FRU, fru_id, &fru_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get binded object handle for class [%s] with alias %d failed", CLASS_FRU, fru_id);
        return ret;
    }

    CAFE_DATA_S cafe_data = { 0 };
    cafe_data.data = (guint8 *)g_memdup2((gconstpointer)cafe_val, n_element);
    if (cafe_data.data == NULL) {
        debug_log(DLOG_ERROR, "%s:dup cafe data failed", __FUNCTION__);
        return RET_ERR;
    }

    cafe_data.len = n_element;
    cafe_data.type = TYPE_8BIT_ASCII;

    ret = e2p_cafe_record_write_specific_fru(fru_handle, cafe_key_str, &cafe_data);
    g_free(cafe_data.data);

    GSList *caller_info_new = NULL;
    if (caller_info == NULL) {
        caller_info_new = g_slist_append(caller_info_new, g_variant_new_string(OPERATION_LOG_BMC));
        caller_info_new = g_slist_append(caller_info_new, g_variant_new_string(NA_STR));
        caller_info_new = g_slist_append(caller_info_new, g_variant_new_string(NA_STR));
    } else {
        caller_info_new = caller_info;
    }

    method_operation_log(caller_info_new, NULL, "Write cafe record [%s] %s", cafe_key_str,
        (ret == RET_OK) ? "successfully" : "failed");

    if (caller_info == NULL) {
        g_slist_free_full(caller_info_new, (GDestroyNotify)g_variant_unref);
    }

    return ret;
}

gint32 method_elabel_property_sync(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret;
    gulong cur_tick;
    static gulong last_tick = 0;

    ret = property_sync_common_handler_ext(obj_handle, caller_info, input_list, output_list, DF_AUTO);
    cur_tick = vos_tick_get();
    if (ret >= RET_OK) {
        last_tick = cur_tick;
    }

    if (cur_tick - last_tick > DATA_SYNC_ERR_LOG_INTERVAL) {
        debug_log(DLOG_ERROR, "[%s] elabel property sync error.", __FUNCTION__);
        last_tick = cur_tick;
    }

    return ret;
}

LOCAL gint32 set_eeprom_data(const OBJ_HANDLE object_handle, guint32 input_offset, const guint8 *input_data,
    gsize input_data_len)
{
    
    if (!is_fru_initialized(object_handle)) {
        debug_log(DLOG_ERROR, "%s: FRU(%s) has not initialized", __FUNCTION__, dfl_get_object_name(object_handle));
        return RET_ERR;
    }

    
    FRU_PRIV_PROPERTY_S *fru_priv_data = NULL;
    gint32 ret = get_obj_priv_data(object_handle, (gpointer *)(&fru_priv_data));
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get binded object failed, error = %d.", __FUNCTION__, ret);
        return ret;
    }

    ret = frudev_write_to_eeprom(fru_priv_data->fru_property.frudev_handle, input_offset, input_data, input_data_len);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: write to eep failed, error = %d.", __FUNCTION__, ret);
        goto EXIT;
    }

    
    if (verify_eep_write_by_read_back(fru_priv_data->fru_property.frudev_handle, input_offset, input_data,
        input_data_len) == TRUE) {
        ret = RET_OK;
        goto EXIT;
    }

    
    debug_log(DLOG_INFO, "divided write eep, off=%u, len=%" G_GSIZE_FORMAT "", input_offset, input_data_len);
    ret = _frudev_write_to_eeprom_by_divided(fru_priv_data->fru_property.frudev_handle, input_offset, input_data,
        input_data_len);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: write to eep by divided failed, error = %d.", __FUNCTION__, ret);
        goto EXIT;
    }
    
    if (verify_eep_write_by_read_back(fru_priv_data->fru_property.frudev_handle, input_offset, input_data,
        input_data_len) == FALSE) {
        debug_log(DLOG_ERROR, "%s: write to eep failed, error = %d.", __FUNCTION__, ret);
        ret = RET_ERR;
    }
EXIT:
    put_obj_priv_data(object_handle, (gpointer)fru_priv_data);
    return ret;
}

gint32 method_fru_set_eeprom_data(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    if (input_list == NULL || g_slist_length(input_list) != SET_EEP_DATA_INPUT_LEN) {
        debug_log(DLOG_ERROR, "%s: input param null.", __FUNCTION__);
        return RET_ERR;
    }

    guint32 input_offset = g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 0));
    gsize input_data_len = 0;
    const guint8 *input_data = (const guint8 *)g_variant_get_fixed_array((GVariant *)(g_slist_nth_data(input_list, 1)),
        &input_data_len, sizeof(guint8));

    if (input_data == NULL || (input_offset > EEP_MAX_SIZE) || (input_data_len > EEP_MAX_SIZE) ||
        (input_data_len == 0) || ((input_offset + (guint32)input_data_len) > EEP_MAX_SIZE)) {
        debug_log(DLOG_ERROR, "%s: input param invalid", __FUNCTION__);
        return RET_ERR;
    }

    return set_eeprom_data(object_handle, input_offset, input_data, input_data_len);
}

LOCAL gint32 get_eeprom_data(const OBJ_HANDLE object_handle, guint32 input_offset, guint32 input_length,
    GSList **output_list)
{
    if (input_length >= 0x4000) { // 不超过EEPROM,128Kbit
        debug_log(DLOG_ERROR, "%s: input_length = %d.", __FUNCTION__, input_length);
        return RET_ERR;
    }

    
    if (!is_fru_initialized(object_handle)) {
        debug_log(DLOG_ERROR, "%s: FRU(%s) has not initialized", __FUNCTION__, dfl_get_object_name(object_handle));
        return RET_ERR;
    }

    
    FRU_PRIV_PROPERTY_S *fru_priv_data = NULL;
    gint32 ret = get_obj_priv_data(object_handle, (gpointer *)(&fru_priv_data));
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get binded object failed, error = %d.", __FUNCTION__, ret);
        return ret;
    }

    guint8 *read_buffer = (guint8 *)g_malloc0(input_length);
    if (read_buffer == NULL) {
        debug_log(DLOG_ERROR, "%s: read buffer g_malloc0 failed.", __FUNCTION__);
        return RET_ERR;
    }

    ret = frudev_read_from_eeprom(fru_priv_data->fru_property.frudev_handle, input_offset, read_buffer, input_length);
    if (ret != DFL_OK) {
        g_free(read_buffer);
        debug_log(DLOG_ERROR, "%s: read from eep failed, error = %d.", __FUNCTION__, ret);
        put_obj_priv_data(object_handle, (gpointer)fru_priv_data);
        return ret;
    }

    if (output_list != NULL) {
        *output_list = g_slist_append(*output_list,
            g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, read_buffer, (guint32)input_length, sizeof(guint8)));
    }

    g_free(read_buffer);
    put_obj_priv_data(object_handle, (gpointer)fru_priv_data);
    return ret;
}

gint32 method_fru_get_eeprom_data(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
#define GET_EEP_DATA_INPUT_LEN 2

    if (input_list == NULL || g_slist_length(input_list) != GET_EEP_DATA_INPUT_LEN) {
        debug_log(DLOG_ERROR, "%s: input param null.", __FUNCTION__);
        return RET_ERR;
    }

    
    guint32 input_length = g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 1));
    if (input_length >= 0x4000) { // 不超过EEPROM,128Kbit
        debug_log(DLOG_ERROR, "%s: input_length = %d.", __FUNCTION__, input_length);
        return RET_ERR;
    }

    guint32 input_offset = g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 0));
    if ((input_offset > EEP_MAX_SIZE) || (input_length > EEP_MAX_SIZE) || (input_length == 0) ||
        ((input_offset + (guint32)input_length) > EEP_MAX_SIZE)) {
        debug_log(DLOG_ERROR, "%s: input param invalid", __FUNCTION__);
        return RET_ERR;
    }

    return get_eeprom_data(object_handle, input_offset, input_length, output_list);
}

gint32 method_set_canbus_dev_eeprom_data(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
#define SET_EEP_DATA_INPUT_LEN 2

    if (input_list == NULL || g_slist_length(input_list) != SET_EEP_DATA_INPUT_LEN) {
        debug_log(DLOG_ERROR, "%s: input param null.", __FUNCTION__);
        return RET_ERR;
    }

    guint32 input_offset = g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 0));
    gsize input_data_len = 0;
    const guint8 *input_data = (const guint8 *)g_variant_get_fixed_array((GVariant *)(g_slist_nth_data(input_list, 1)),
        &input_data_len, sizeof(guint8));

    if ((input_offset >= CANBUS_DEV_EEPROM_INFO_MAX_SIZE) || (input_data_len > CANBUS_DEV_EEPROM_INFO_MAX_SIZE) ||
        (input_data_len == 0) || ((input_offset + (guint32)input_data_len) > CANBUS_DEV_EEPROM_INFO_MAX_SIZE)) {
        debug_log(DLOG_ERROR, "%s: input param invalid", __FUNCTION__);
        return RET_ERR;
    }

    
    FRU_PRIV_PROPERTY_S *fru_priv_data = NULL;
    gint32 ret = get_obj_priv_data(object_handle, (gpointer *)(&fru_priv_data));
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get binded object failed, error = %d.", __FUNCTION__, ret);
        return ret;
    }

    guint32 real_offset = input_offset + fru_priv_data->area_offsets->canbus_dev_offset;
    put_obj_priv_data(object_handle, (gpointer)fru_priv_data);
    return set_eeprom_data(object_handle, real_offset, input_data, input_data_len);
}

gint32 method_get_canbus_dev_eeprom_data(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
#define GET_EEP_DATA_INPUT_LEN 2

    if (input_list == NULL || output_list == NULL || g_slist_length(input_list) != GET_EEP_DATA_INPUT_LEN) {
        debug_log(DLOG_ERROR, "%s: input param null.", __FUNCTION__);
        return RET_ERR;
    }

    
    guint32 input_offset = g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 0));
    guint32 input_length = g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 1));
    if (input_offset >= CANBUS_DEV_EEPROM_INFO_MAX_SIZE || input_length > CANBUS_DEV_EEPROM_INFO_MAX_SIZE ||
        input_length == 0 || (input_offset + input_length) > CANBUS_DEV_EEPROM_INFO_MAX_SIZE) {
        debug_log(DLOG_ERROR, "%s: input param invalid, input_offset = %d, input_length = %d.", __FUNCTION__,
            input_offset, input_length);
        return RET_ERR;
    }

    
    FRU_PRIV_PROPERTY_S *fru_priv_data = NULL;
    gint32 ret = get_obj_priv_data(object_handle, (gpointer *)(&fru_priv_data));
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get binded object failed, error = %d.", __FUNCTION__, ret);
        return ret;
    }

    guint32 real_offset = input_offset + fru_priv_data->area_offsets->canbus_dev_offset;
    put_obj_priv_data(object_handle, (gpointer)fru_priv_data);
    return get_eeprom_data(object_handle, real_offset, input_length, output_list);
}

gint32 method_update_mainboard_fru_slot_id(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s: input param null.", __FUNCTION__);
        return RET_ERR;
    }

    if (g_slist_length(input_list) != 1) {
        return RET_ERR;
    }

    GVariant *slot_id = (GVariant *)g_slist_nth_data(input_list, 0);
    if (slot_id == NULL) {
        return RET_ERR;
    }

    FRU_PRIV_PROPERTY_S *fru_priv = NULL;
    OBJ_HANDLE fru_handle = 0;
    gint32 ret = get_fru_priv_data_by_id(FRU_ID_MAINBOARD, &fru_priv, &fru_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : Get FRU0 private data failed, ret=%d", __FUNCTION__, ret);
        return ret;
    }

    fru_priv->fru_property.slot_id = g_variant_get_byte(slot_id);
    put_obj_priv_data(fru_handle, (gpointer)fru_priv);
    return RET_OK;
}

gint32 method_fru_set_eeprom_raw_data(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    OBJ_HANDLE dev_handle = 0;
    gint32 ret = get_back_plane_dev_handle(&dev_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get back plane dev handle failed, err code %d", ret);
        return RET_ERR;
    }

    guint16 fru_offset = 0;
    ret = get_back_plane_fru_offset(&fru_offset);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get back plane fru offset failed, err code %d", ret);
        return ret;
    }

    gconstpointer data = g_variant_get_data((GVariant *)g_slist_nth_data(input_list, 0));
    if (data == NULL) {
        debug_log(DLOG_ERROR, "The input data is NULL.");
        return RET_ERR;
    }
    guint32 data_length = g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 1)); 
    guint32 define_length = g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 2)); 
    gint32 offset = g_variant_get_int32((GVariant *)g_slist_nth_data(input_list, 3)) + fru_offset; 
    guint32 block_size = g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 4)); 

    
    ret = write_info_to_raw_eeprom(dev_handle, data, data_length, define_length, offset, block_size);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Write chassis number to eeprom failed, err code %d", ret);
    }

    return ret;
}


gint32 method_elabel_sync_property(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret;
    ret = property_sync_common_handler_ext(obj_handle, caller_info, input_list, output_list, DF_MEM);
    debug_log(DLOG_DEBUG, "Sync property %s returns %d", dfl_get_object_name(obj_handle), ret);
    return ret;
}