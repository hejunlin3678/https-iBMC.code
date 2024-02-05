

#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "pme_app/dalib/dal_obj_priv_data.h"
#include "intf/event.h"
#include "component/component.h"
#include "component/connector.h"
#include "component/base.h"
#include "elabel/elabel.h"
#include "prepare_fru.h"

#define WRITE_BMC_FRU_COUNT 5


gint32 event_update_fru_locator_record_id(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 result = dal_get_specific_object_byte(CLASS_FRU, PROPERTY_FRU_ID, 0, &obj_handle);
    if (result != RET_OK) {
        debug_log(DLOG_ERROR, "get the fru0 obj failed");
        return RET_OK;
    }

    FRU_PRIV_PROPERTY_S *fru_priv_property = NULL;
    result = get_obj_priv_data(obj_handle, (gpointer *)&(fru_priv_property));
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: call dfl_get_binded_object failed, error = %d", __FUNCTION__, result);
        return RET_OK;
    }

    guint16 local_record_id = g_variant_get_uint16(property_value);
    fru_priv_property->fru_property.locator_record_id = local_record_id;
    put_obj_priv_data(object_handle, (gpointer)fru_priv_property);
    return RET_OK;
}


gint32 event_update_hdd_component(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value)
{
    gchar referencd_object[MAX_NAME_SIZE] = {0};
    gchar referencd_property[MAX_NAME_SIZE] = {0};
    gint32 ret = dfl_get_referenced_property(object_handle, PROPERTY_HDD_DEVICENAME, referencd_object,
        referencd_property, MAX_NAME_SIZE, MAX_NAME_SIZE);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: dfl_get_referenced_property failed", __FUNCTION__);
        return RET_ERR;
    }

    OBJ_HANDLE com_handle;
    ret = dfl_get_object_handle(referencd_object, &com_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: dfl_get_object_handle failed", __FUNCTION__);
        return RET_ERR;
    }

    dfl_unbind_all_alias(com_handle);

    ret = init_component(com_handle, NULL);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: init_component failed", __FUNCTION__);
        return RET_ERR;
    }

    return RET_OK;
}


gint32 event_update_fru_ipmbaddr(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value)
{
    GVariant *value = g_variant_new_byte(FRU_ID_MAINBOARD);
    OBJ_HANDLE fru_obj_handle = 0;
    gint32 ret = dfl_get_specific_object(CLASS_FRU, PROPERTY_FRU_ID, value, &fru_obj_handle);
    g_variant_unref(value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: dfl_get_specific_object of fru 0 failed, error : %d", __FUNCTION__, ret);
        return ret;
    }

    FRU_PRIV_PROPERTY_S *fru_priv_data = NULL;
    ret = get_obj_priv_data(fru_obj_handle, (gpointer *)&fru_priv_data);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: dfl_get_binded_data of fru 0 failed, error : %d", __FUNCTION__, ret);
        return ret;
    }

    guint8 ipmb_addr = g_variant_get_byte(property_value);
    fru_priv_data->fru_property.ipmb_addr = ipmb_addr;
    put_obj_priv_data(object_handle, (gpointer)fru_priv_data);
    return ret;
}


gint32 event_process_as_status_change(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value)
{
    if (property_value == NULL) {
        debug_log(DLOG_ERROR, "The property_value is null.");
        return RET_ERR;
    }

    guint8 as_status = g_variant_get_byte(property_value);

    
    guint32 b_load = (as_status == ACTIVE_STATE) ? TRUE : FALSE;
    if (dal_check_if_vsmm_supported()) {
        b_load = (as_status == MSM_PRIMARY_STATE) ? TRUE : FALSE;
    }
    
    // b_load = FALSE
    TASKID task_id = 0;
    gint32 ret = vos_task_create(&task_id, "FruUpdate", (TASK_ENTRY)update_fru_in_standby_smm,
        UINT32_TO_POINTER(b_load), DEFAULT_PRIORITY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Create fru update task failed, err code %d", ret);
    }

    return RET_OK;
}


LOCAL gint32 update_chassis_product_sn_elabel(const gchar *mmc_elabel)
{
    gint32 ret;

    for (guint32 i = 0; i < WRITE_BMC_FRU_COUNT; i++) {
        ret = send_write_elabel(CHASSIS_FRU_ID, FRU_AREA_PRODUCTINFO, FRU_PRODUCT_SERIALNUMBER, mmc_elabel);
        if (ret != RET_OK) {
            vos_task_delay(1000);
            continue;
        }

        
        ret = send_write_elabel(CHASSIS_FRU_ID, FRU_AREA_SYSTEMINFO, SYSTEM_SERIAL_NUMBER, mmc_elabel);
        if (ret != RET_OK) {
            vos_task_delay(1000);
            continue;
        }

        upgrade_elabel(CHASSIS_FRU_ID);
        debug_log(DLOG_ERROR, "rimm sync product.sn(%s) to bmc success", mmc_elabel);
        return ret;
    }

    debug_log(DLOG_ERROR, "sync rimm product sn to bmc fail %s", mmc_elabel);

    return ret;
}


gint32 event_renew_product_sn_elabel(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value)
{
    if (property_value == NULL) {
        debug_log(DLOG_ERROR, "The property_value is null.");
        return RET_ERR;
    }

    
    gsize str_len = 0;
    const gchar *tmp_string = g_variant_get_string(property_value, &str_len);
    if (str_len == 0) {
        debug_log(DLOG_ERROR, "sync rimm product sn(%s) failed, empty sn", tmp_string);
        return RET_ERR;
    }

    guint8 sync_status = 0;
    get_sync_elabel_state(&sync_status);

    
    if ((sync_status & 0x2) == 0) {
        debug_log(DLOG_DEBUG, "disabled sync rimm product sn to bmc");
        return RET_OK;
    }

    gchar suffix[ELABEL_SUFFIX_MIN_LEN] = {0};
    get_node_index_suffix(suffix, sizeof(suffix));

    gchar mmc_elabel[MMC_MAX_ELABEL_LEN] = {0};
    gint32 ret = snprintf_s(mmc_elabel, sizeof(mmc_elabel), sizeof(mmc_elabel) - 1, "%s%s", tmp_string, suffix);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    
    FRU_PRIV_PROPERTY_S *fru_priv = NULL;
    OBJ_HANDLE fru_handle = 0;
    ret = get_fru_priv_data_by_id(CHASSIS_FRU_ID, &fru_priv, &fru_handle);
    if (ret != RET_OK) {
        return ret;
    }

    if (strcmp((gchar *)fru_priv->fru_info->product.product_serial_num.content, mmc_elabel) == 0) {
        debug_log(DLOG_DEBUG, "no need to sync rimm product sn(%s), since it is the same.", mmc_elabel);
        put_obj_priv_data(fru_handle, (gpointer)fru_priv);
        return RET_OK;
    }

    ret = update_chassis_product_sn_elabel(mmc_elabel);
    put_obj_priv_data(fru_handle, (gpointer)fru_priv);
    return ret;
}


gint32 event_renew_product_assettag_elabel(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value)
{
    if (property_value == NULL) {
        debug_log(DLOG_ERROR, "The property_value is null.");
        return RET_ERR;
    }

    
    gsize str_len = 0;
    const gchar *tmp_string = g_variant_get_string(property_value, &str_len);
    if (str_len == 0) {
        debug_log(DLOG_ERROR, "renew_bmc_product_assettag_elabel(%s) failed, empty sn", tmp_string);
        return RET_ERR;
    }

    guint8 sync_state = 0;
    get_sync_elabel_state(&sync_state);

    
    if ((sync_state & 0x1) == 0) {
        debug_log(DLOG_DEBUG, "disabled sync rimm product asserttag to bmc");
        return RET_OK;
    }

    gchar suffix[ELABEL_SUFFIX_MIN_LEN] = {0};
    get_node_index_suffix(suffix, sizeof(suffix));

    gchar mmc_elabel[MMC_MAX_ELABEL_LEN] = {0};
    gint32 ret = snprintf_s(mmc_elabel, sizeof(mmc_elabel), sizeof(mmc_elabel) - 1, "%s%s", tmp_string, suffix);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    
    FRU_PRIV_PROPERTY_S *fru_priv = NULL;
    OBJ_HANDLE fru_handle = 0;
    ret = get_fru_priv_data_by_id(CHASSIS_FRU_ID, &fru_priv, &fru_handle);
    if (ret != RET_OK) {
        return ret;
    }

    if (strcmp((gchar *)fru_priv->fru_info->product.product_asset_tag.content, mmc_elabel) == 0) {
        debug_log(DLOG_DEBUG, "no need to sync rimm product assettag(%s), since it is the same.", mmc_elabel);
        put_obj_priv_data(fru_handle, (gpointer)fru_priv);
        return RET_ERR;
    }

    for (guint32 i = 0; i < WRITE_BMC_FRU_COUNT; i++) {
        ret = send_write_elabel(CHASSIS_FRU_ID, FRU_AREA_PRODUCTINFO, FRU_PRODUCT_ASSETTAG, mmc_elabel);
        if (ret == RET_OK) {
            upgrade_elabel(CHASSIS_FRU_ID);
            debug_log(DLOG_ERROR, "rimm sync product.assettag(%s) to bmc success", mmc_elabel);
            put_obj_priv_data(fru_handle, (gpointer)fru_priv);
            return ret;
        }

        vos_task_delay(1000);
    }

    debug_log(DLOG_ERROR, "sync rimm product assettag to bmc fail %s", mmc_elabel);
    put_obj_priv_data(fru_handle, (gpointer)fru_priv);
    return ret;
}