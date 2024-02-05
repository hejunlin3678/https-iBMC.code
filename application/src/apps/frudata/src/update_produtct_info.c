

#include "update_produtct_info.h"
#include "common.h"
#include "media/e2p_large.h"
#include "component/module_init.h"
#include "component/base.h"
#include "elabel/elabel.h"
#include "elabel/base.h"
#include "elabel/module_init.h"
#include "prepare_fru.h"
#include "protocol.h"

#define PRODUCT_NAME_EEPROM_AREA_OFFSET 0x0022
#define PRODUCT_NAME_EEPROM_AREA_SIZE 32
#define STANDARD_CHECK_CODE_EEPROM_AREA_OFFSET 0x0008
#define STANDARD_CHECK_CODE_EEPROM_AREA_SIZE 4
#define PRODUCT_ALIAS_EEPROM_AREA_OFFSET 0x0042
#define PRODUCT_ALIAS_EEPROM_AREA_SIZE 32
#define PRODUCT_UID_EEPROM_AREA_OFFSET 0x001e
#define PRODUCT_UID_EEPROM_AREA_SIZE 4
#define TIANCHI_PRODUCT_ID 0
#define TIANCHI_BOARD_ID 0xFF
#define ASCII_MIN 0x20
#define ASCII_MAX 0x7E


LOCAL gint32 verify_tianchi_model(void)
{
    gint32 ret;
    guint8 board_id = 0xFF;
    OBJ_HANDLE obj_product = 0;
    guint32 product_id = 0;

    
    (void)dal_get_func_ability(BMC_BOARD_NAME, BMC_BOARDID_NAME, &board_id);
    if (board_id != TIANCHI_BOARD_ID) {
        debug_log(DLOG_DEBUG, "%s:BoardId mismatch with the Tianchi model", __func__);
        return RET_ERR;
    }

    ret = dal_get_object_handle_nth(CLASS_NAME_PME_PRODUCT, 0, &obj_product);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s:dal_get_object_handle_nth failed", __func__);
        return RET_ERR;
    }
    (void)dal_get_property_value_uint32(obj_product, BMC_PRODUCT_ID_NAME, &product_id);
    if (product_id != TIANCHI_PRODUCT_ID) {
        debug_log(DLOG_DEBUG, "%s:ProductId mismatch with the Tianchi model", __func__);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 verify_standard_check_code(const OBJ_HANDLE object_handle)
{
    gint32 ret = RET_OK;

    guint8 read_buffer[STANDARD_CHECK_CODE_EEPROM_AREA_SIZE] = {0};
    ret = fru_block_read(object_handle, PROPERTY_FRUDEV_STORAGE_LOC, STANDARD_CHECK_CODE_EEPROM_AREA_OFFSET,
        STANDARD_CHECK_CODE_EEPROM_AREA_SIZE, &read_buffer);
    if ((ret != RET_OK)) {
        debug_log(DLOG_ERROR, "%s: read from eep failed, error = %d.", __FUNCTION__, ret);
        return RET_ERR;
    }

    
    if (read_buffer[0] != 0x5A || read_buffer[1] != 0xA5       // 校验第0位为0x5A 第1位为0xA5
        || read_buffer[2] != 0x5A || read_buffer[3] != 0xA5) { // 校验第2位为0x5A 第3位为0xA5
        return RET_ERR;
    }
    return RET_OK;
}

LOCAL gint32 set_product_name_value(const gchar *product_name)
{
    OBJ_HANDLE obj_product = OBJ_HANDLE_COMMON;
    gint32 ret = RET_OK;
    ret = dal_get_object_handle_nth(CLASS_NAME_PME_PRODUCT, 0, &obj_product);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "get PRODUCT obj handle fail, ret = %d !", ret);
        return RET_ERR;
    }

    GSList *input_list = NULL;
    input_list = g_slist_append(input_list, g_variant_new_string(product_name));
    ret = dfl_call_class_method(obj_product, METHOD_PRODUCT_SET_PRODUCT_NAME, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    input_list = NULL;
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "call method METHOD_PRODUCT_SET_PRODUCT_NAME failed, ret = %d!", ret);
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL void generate_product_data_fault(const OBJ_HANDLE fru_handle)
{
    guint8 data_valid_state;

    (void)dal_get_property_value_byte(fru_handle, PROTERY_FRU_DATA_VALID_STATE, &data_valid_state);
    
    if (data_valid_state & 0x01) {
        debug_log(DLOG_ERROR, "%s: product data fault has already triggered", __FUNCTION__);
        return;
    }

    
    (void)dal_set_property_value_byte(fru_handle, PROTERY_FRU_DATA_VALID_STATE, data_valid_state | 0x01, DF_NONE);
    debug_log(DLOG_ERROR, "%s: product data fault triggered", __FUNCTION__);
}


LOCAL void clear_product_data_fault(const OBJ_HANDLE fru_handle)
{
    guint8 data_valid_state;

    (void)dal_get_property_value_byte(fru_handle, PROTERY_FRU_DATA_VALID_STATE, &data_valid_state);
    
    if (data_valid_state & 0x01) {
        
        (void)dal_set_property_value_byte(fru_handle, PROTERY_FRU_DATA_VALID_STATE, data_valid_state & 0xFE, DF_NONE);
        debug_log(DLOG_DEBUG, "%s: product data fault cleared", __FUNCTION__);
        return;
    }

    debug_log(DLOG_DEBUG, "%s: product data fault has already cleared", __FUNCTION__);
}


LOCAL gint32 set_on_board_product_name(FRU_PROPERTY_S *fru_property)
{
    gint32 ret = RET_OK;

    
    gchar read_buffer[PRODUCT_NAME_EEPROM_AREA_SIZE + 1] = {0};
    ret = fru_block_read(fru_property->frudev_handle, PROPERTY_FRUDEV_STORAGE_LOC, PRODUCT_NAME_EEPROM_AREA_OFFSET,
        PRODUCT_NAME_EEPROM_AREA_SIZE, read_buffer);
    if ((ret != RET_OK)) {
        debug_log(DLOG_ERROR, "%s: read from eep failed, error = %d.", __FUNCTION__, ret);
        return RET_ERR;
    }

    if (strlen(read_buffer) == 0) {
        debug_log(DLOG_ERROR, "%s: product name parsing error", __FUNCTION__);
        return RET_ERR;
    }
    for (int i = 0; i < PRODUCT_NAME_EEPROM_AREA_SIZE; i++) {
        if ((i < strlen(read_buffer) && (read_buffer[i] < ASCII_MIN || read_buffer[i] > ASCII_MAX))
            || (i >= strlen(read_buffer) && read_buffer[i] != 0)) {
            debug_log(DLOG_ERROR, "%s: product name parsing error", __FUNCTION__);
            return RET_ERR;
        }
    }
    debug_log(DLOG_ERROR, "%s: set product name to %s", __FUNCTION__, read_buffer);

    (void)set_product_name_value(read_buffer);
    return RET_OK;
}

LOCAL gint32 set_product_unique_id(const guint32 product_unique_id)
{
    OBJ_HANDLE obj_product = OBJ_HANDLE_COMMON;
    gint32 ret = RET_OK;
    ret = dal_get_object_handle_nth(CLASS_NAME_PME_PRODUCT, 0, &obj_product);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "get PRODUCT obj handle fail, ret = %d !", ret);
        return RET_ERR;
    }

    GSList *input_list = NULL;
    input_list = g_slist_append(input_list, g_variant_new_uint32(product_unique_id));
    ret = dfl_call_class_method(obj_product, METHOD_PRODUCT_SET_PRODUCT_UID, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    input_list = NULL;
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "call method METHOD_PRODUCT_SET_PRODUCT_UID failed, ret = %d!", ret);
        return RET_ERR;
    }
    return RET_OK;
}

LOCAL gint32 set_product_default_alias_value(const gchar* product_alias)
{
    OBJ_HANDLE obj_product = OBJ_HANDLE_COMMON;
    gint32 ret = RET_OK;
    ret = dal_get_object_handle_nth(CLASS_NAME_PME_PRODUCT, 0, &obj_product);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "get PRODUCT obj handle fail, ret = %d !", ret);
        return RET_ERR;
    }

    GSList *input_list = NULL;
    input_list = g_slist_append(input_list, g_variant_new_string(product_alias));
    ret = dfl_call_class_method(obj_product, BMC_METHOD_SETPRODUCT_DEFAULTALIAS, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    input_list = NULL;
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "call method BMC_METHOD_SETPRODUCT_DEFAULTALIAS failed, ret = %d!", ret);
        return RET_ERR;
    }
    
    debug_log(DLOG_ERROR, "%s: set product default alias to %s", __FUNCTION__, product_alias);
    return RET_OK;
}

LOCAL gint32 set_product_alias_value(const gchar* product_alias)
{
    OBJ_HANDLE obj_product = OBJ_HANDLE_COMMON;
    gint32 ret = RET_OK;
    gboolean isSaved;
    ret = dal_get_object_handle_nth(CLASS_NAME_PME_PRODUCT, 0, &obj_product);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "get PRODUCT obj handle fail, ret = %d !", ret);
        return RET_ERR;
    }

    // PROPERTY_PRODUCT_ALIAS属性可能未被持久化，如未持久化说明没有被定制，则直接赋值
    isSaved = dfl_is_property_value_saved(obj_product, PROPERTY_PRODUCT_ALIAS, DF_SAVE);
    if (isSaved) {
        return RET_OK;
    }

    GSList *input_list = NULL;
    input_list = g_slist_append(input_list, g_variant_new_string(product_alias));
    ret = dfl_call_class_method(obj_product, BMC_METHOD_SETPRODUCTALIAS_NO_PERSISTENCE, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    input_list = NULL;
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "call method BMC_METHOD_SETPRODUCTALIAS_NO_PERSISTENCE failed, ret = %d!", ret);
        return RET_ERR;
    }
    
    debug_log(DLOG_ERROR, "%s: set product alias to %s", __FUNCTION__, product_alias);
    return RET_OK;
}


LOCAL gint32 set_product_default_alias(FRU_PROPERTY_S *fru_property)
{
    gint32 ret = RET_OK;
 
    
    gchar read_buffer[PRODUCT_ALIAS_EEPROM_AREA_SIZE + 1] = {0};
    ret = fru_block_read(fru_property->frudev_handle, PROPERTY_FRUDEV_STORAGE_LOC, PRODUCT_ALIAS_EEPROM_AREA_OFFSET,
        PRODUCT_ALIAS_EEPROM_AREA_SIZE, read_buffer);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: read from eep failed, error = %d.", __FUNCTION__, ret);
        return RET_ERR;
    }
 
    if (strlen(read_buffer) == 0) {
        debug_log(DLOG_DEBUG, "%s: get product alias from eep valued none", __FUNCTION__);
    }
    
    for (int i = 0; i < PRODUCT_ALIAS_EEPROM_AREA_SIZE; i++) {
        if ((i < strlen(read_buffer) && (read_buffer[i] < ASCII_MIN || read_buffer[i] > ASCII_MAX))
            || (i >= strlen(read_buffer) && read_buffer[i] != 0)) {
            debug_log(DLOG_ERROR, "%s: product alias parsing error", __FUNCTION__);
            return RET_ERR;
        }
    }

    (void)set_product_default_alias_value(read_buffer);
    (void)set_product_alias_value(read_buffer);
    return RET_OK;
}


LOCAL gint32 set_on_board_product_unique_id(const OBJ_HANDLE object_handle)
{
    gint32 ret = RET_OK;

    
    guint8 read_buffer[PRODUCT_UID_EEPROM_AREA_SIZE] = {0};
    ret = fru_block_read(object_handle, PROPERTY_FRUDEV_STORAGE_LOC, PRODUCT_UID_EEPROM_AREA_OFFSET,
        PRODUCT_UID_EEPROM_AREA_SIZE, read_buffer);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: read from eep failed, error = %d.", __FUNCTION__, ret);
        return RET_ERR;
    }

    guint32 unique_id = 0;
    for (int i = 0; i < PRODUCT_UID_EEPROM_AREA_SIZE; i++) {
        unique_id = (unique_id << 8) + read_buffer[i]; // 左移8位
    }
    debug_log(DLOG_ERROR, "%s: set product uid to %d", __FUNCTION__, unique_id);

    (void)set_product_unique_id(unique_id);
    return RET_OK;
}

gint32 tc_update_product_info(FRU_PROPERTY_S *fru_property)
{
    if (fru_property->fru_id != 0) {
        return RET_OK;
    }
    
    if (verify_tianchi_model() != RET_OK) {
        return RET_OK;
    }

    
    if (verify_standard_check_code(fru_property->frudev_handle) != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: The verification code does not meet the Tianchi specifications.", __FUNCTION__);
        return RET_OK;
    }

    gint32 ret = set_on_board_product_name(fru_property);
    if (ret != RET_OK) {
        
        generate_product_data_fault(fru_property->fru_handle);
        return RET_ERR;
    }
    ret = set_product_default_alias(fru_property);
    if (ret != RET_OK) {
        
        generate_product_data_fault(fru_property->fru_handle);
        return RET_ERR;
    }

    (void)set_on_board_product_unique_id(fru_property->frudev_handle);

    
    clear_product_data_fault(fru_property->fru_handle);
    return RET_OK;
}
