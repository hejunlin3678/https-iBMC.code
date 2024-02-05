

#include "pme/pme.h"
#include "common.h"
#include "pme_app/pme_app.h"
#include "elabel/elabel.h"
#include "media/virtual_mmc.h"
#include "protocol.h"
#include "media/custom_mcu.h"

LOCAL gint32 read_bbu_fru_info(guint8 dev_num, FRU_FILE_S *fru_file);
LOCAL gint32 read_pangea_card_fru_info(guint8 dev_num, FRU_FILE_S *fru_file);

LOCAL FRU_LABEL_FUNC_TBL_S g_fruinfo_func_tbl[] = {
    { COMPONENT_TYPE_BATTERY, read_bbu_fru_info },
    { COMPONENT_TYPE_PCIE_CARD, read_pangea_card_fru_info },
};


void change_pangea_card_elabel_format(const guint8 *src_data, guint32 src_data_len, OBJ_HANDLE fru_obj,
    FRU_INFO_S *fru_info)
{
    guint8 buf[FRU_FILE_MAX_LEN] = { 0 }; 
    gint32 ret;

    ret = memcpy_s(buf, sizeof(buf), src_data, src_data_len);
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "memcpy_s data to buf failed, ret=%d, src_data_len=%d, buf_len=%zu", ret, src_data_len,
            sizeof(buf));
        return;
    }

    init_board_fru_from_data((gchar *)buf, FRU_FILE_MAX_LEN, fru_info);
    
    fru_info->has_product = TRUE;
    get_correspond_elabel_data((gchar *)buf, PRODUCT_ELABEL_KEY_ISSUE_NUM, &(fru_info->product.product_version));
    init_extend_elabel_from_data((gchar *)buf, FRU_FILE_MAX_LEN, fru_info);
}


LOCAL gint32 get_fru_dev_info(OBJ_HANDLE object_handle, guint8 *dev_type, guint8 *device_num)
{
    gint32 ret;
    guint8 fru_id = 0;
    OBJ_HANDLE component_handle = 0;

    ret = dal_get_property_value_byte(object_handle, PROPERTY_FRU_ID, &fru_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get %s fru id failed! ret = %d", dfl_get_object_name(object_handle), ret);
        return ret;
    }

    ret = dal_get_specific_object_byte(CLASS_COMPONENT, PROPERTY_COMPONENT_FRUID, fru_id, &component_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get %s component failed! ret = %d", dfl_get_object_name(object_handle), ret);
        return ret;
    }

    ret = dal_get_property_value_byte(component_handle, PROPERTY_COMPONENT_DEVICE_TYPE, dev_type);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get %s device type failed! ret = %d", dfl_get_object_name(object_handle), ret);
        return ret;
    }

    ret = dal_get_property_value_byte(component_handle, PROPERTY_COMPONENT_DEVICENUM, device_num);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get %s dev_num failed! ret = %d", dfl_get_object_name(object_handle), ret);
    }

    return ret;
}


gint32 _write_elabel_to_mcu_custom(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file)
{
    gint32 ret;
    guint8 device_num = 0;
    guint8 dev_type = 0;
    OBJ_HANDLE elabel_handle = 0;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    const gchar *func_name = NULL;

    ret = get_fru_dev_info(object_handle, &dev_type, &device_num);
    if (ret != VOS_OK) {
        return ret;
    }

    ret = dfl_get_referenced_object(object_handle, PROPERTY_FRU_ELABEL_RO, &elabel_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_INFO, "This fru %s is not support elabel", dfl_get_object_name(object_handle));
        return VOS_OK;
    }

    if (dev_type == COMPONENT_TYPE_BATTERY) {
        ret = dal_get_specific_object_byte(CLASS_BBU_MODULE_NAME, PROPERTY_BBU_MODULE_ID, device_num, &obj_handle);
        if (ret != VOS_OK) {
            debug_log(DLOG_ERROR, "Get bbu object failed, ret is %d", ret);
            return VOS_ERR;
        }

        func_name = METHOD_BBU_MODULE_SET_ELABEL;
    }

    if (func_name == NULL) {
        debug_log(DLOG_ERROR, "%s remote method name is null.", dfl_get_object_name(object_handle));
        return RET_ERR;
    }

    input_list = g_slist_append(input_list,
        (gpointer)g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, fru_file, sizeof(fru_file->frudata), sizeof(guint8)));

    ret = write_elabel_data_to_mcu(obj_handle, func_name, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "Write elabel info to mcu failed, obj name is %s", dfl_get_object_name(obj_handle));
    }
    return ret;
}


gint32 read_elabel_from_custom_mcu(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file)
{
    gint32 ret;
    guint8 device_num = 0;
    guint8 dev_type = 0;
    OBJ_HANDLE elabel_handle = 0;
    guint8 idx;

    if (fru_file == NULL) {
        debug_log(DLOG_INFO, "Input fru ptr is null.");
        return RET_ERR;
    }

    ret = dfl_get_referenced_object(object_handle, PROPERTY_FRU_ELABEL_RO, &elabel_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_INFO, "This fru %s is not support elabel", dfl_get_object_name(object_handle));
        return ret;
    }

    ret = get_fru_dev_info(object_handle, &dev_type, &device_num);
    if (ret != RET_OK) {
        return ret;
    }

    for (idx = 0; idx < sizeof(g_fruinfo_func_tbl) / sizeof(FRU_LABEL_FUNC_TBL_S); idx++) {
        if (dev_type == g_fruinfo_func_tbl[idx].dev_type) {
            ret = g_fruinfo_func_tbl[idx].get_fru_info(device_num, fru_file);
        }
    }

    return ret;
}


LOCAL gint32 get_pangea_card_fru_obj(OBJ_HANDLE card_obj, guint8 dev_num, OBJ_HANDLE *fru_obj)
{
    gint32 ret;
    OBJ_HANDLE net_card_obj = INVALID_OBJ_HANDLE;
    OBJ_HANDLE ref_obj;

    ret = dal_get_specific_position_object_byte(card_obj, CLASS_NETCARD_NAME, PROPERTY_NETCARD_SLOT_ID, dev_num,
        &net_card_obj);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get slot(%d) net card obj failed, ret=%d.", dev_num, ret);
        return ret;
    }

    ret = dfl_get_referenced_object(net_card_obj, PROPERTY_NETCARD_REF_FRU, &ref_obj);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get slot(%d) net card obj ref fru failed, ret=%d.", dev_num, ret);
        return ret;
    }
    *fru_obj = ref_obj;
    return ret;
}


LOCAL gint32 read_pangea_card_frudata_from_mcu(OBJ_HANDLE card_handle, guint8 dev_num, FRU_FILE_S *fru_file)
{
    gint32 result;
    gsize output_data_len;
    OBJ_HANDLE fru_obj = 0;
    GSList *output_list = NULL;
    const guint8 *output_data = NULL;
    FRU_INFO_S fru_info = { 0 };

    result = dfl_call_class_method(card_handle, METHOD_GET_MCU_ELABLE, NULL, NULL, &output_list);
    if (result != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: Get pangea card elabel data failed, result is %d\n", __FUNCTION__, result);
        return result;
    }

    output_data = (const guint8 *)g_variant_get_fixed_array((GVariant *)g_slist_nth_data(output_list, 0),
        &output_data_len, sizeof(guint8));

    result = get_pangea_card_fru_obj(card_handle, dev_num, &fru_obj);
    if (result == RET_OK && output_data_len > 0 && output_data != NULL) {
        change_pangea_card_elabel_format(output_data, output_data_len, fru_obj, &fru_info);
        (void)generate_fru_file(EEPROM_FORMAT_V2, &fru_info, fru_file);
    }

    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    return result;
}


LOCAL gint32 read_pangea_card_fru_info(guint8 dev_num, FRU_FILE_S *fru_file)
{
    OBJ_HANDLE obj_handle;
    gint32 ret;

    ret = dal_get_specific_object_byte(CLASS_PANGEA_PCIE_CARD, PROPERTY_PCIE_CARD_SLOT_ID, dev_num, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get fan object failed, ret is %d", ret);
        return ret;
    }

    ret = read_pangea_card_frudata_from_mcu(obj_handle, dev_num, fru_file);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get fru info from card mcu failed, ret is %d", ret);
    }
    return ret;
}


LOCAL gint32 read_bbu_frudata_from_mcu(OBJ_HANDLE bbu_handle, FRU_FILE_S *fru_file)
{
    gint32 result;
    gsize parm_temp = 0;
    GSList *output_list = NULL;
    const guint8 *output_data = NULL;

    result = dfl_call_class_method(bbu_handle, METHOD_BBU_MODULE_GET_ELABEL, NULL, NULL, &output_list);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: Get bbu elabel data failed, result is %d.", __FUNCTION__, result);
        return result;
    }

    output_data = (const guint8 *)g_variant_get_fixed_array((GVariant *)g_slist_nth_data(output_list, 0), &parm_temp,
        sizeof(guint8));
    if (output_data == NULL) {
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        debug_log(DLOG_ERROR, "%s get fixed array data from list failed.", dfl_get_object_name(bbu_handle));
        return RET_ERR;
    }

    result = memcpy_s(fru_file, sizeof(FRU_FILE_S), output_data, sizeof(FRU_FILE_S)); 
    if (result != EOK) {
        debug_log(DLOG_ERROR, "%s: Copy bbu elabel data failed, result is %d\n", __FUNCTION__, result);
    }
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

    return result;
}


LOCAL gint32 read_bbu_fru_info(guint8 dev_num, FRU_FILE_S *fru_file)
{
    OBJ_HANDLE obj_handle;
    gint32 ret;

    ret = dal_get_specific_object_byte(CLASS_BBU_MODULE_NAME, PROPERTY_BBU_MODULE_ID, dev_num, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get fan object failed, ret is %d", ret);
        return ret;
    }

    ret = read_bbu_frudata_from_mcu(obj_handle, fru_file);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get fru info from bbu mcu failed, ret is %d", ret);
    }
    return ret;
}
