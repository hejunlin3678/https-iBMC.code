

#include <ctype.h>
#include "prepare_fru.h"
#include "common.h"
#include "media/e2p_compact.h"
#include "media/e2p_large.h"
#include "media/virtual_mmc.h"
#include "media/virtual_psu.h"
#include "media/etc.h"
#include "protocol.h"
#include "elabel/elabel.h"
#include "elabel/base.h"
#include "elabel/elabel_init.h"
#include "component/component.h"
#include "component/base.h"
#include "component/connector.h"
#include "component/mezz.h"
#include "update_produtct_info.h"
#include "media/port_rate.h"
#include "pme_app/dalib/dal_obj_priv_data.h"
#include "media/custom_mcu.h"
#include "module_mgnt.h"
#include "intf/ipmi.h"

#define FRU_ID_IS_USED 1
#define FRU_ID_NOT_USED 0
#define FRU_POSITION_LIST_ADD 1
#define FRU_POSITION_LIST_DEL 2
#define PCB_VERSION_BUF_SIZE 3
#define PCB_VER_VAR_OFFSET 1
#define MAX_FRU_SYN_TASK_NAME 32
#define FRU_SYNC_TASK_NAME "fru_syn_task"
#define ELABEL_SYNC_TASK_NAME "elabel_syn_task"
#define SYS_FIELD_SYNC_TASK_NAME "sys_syn_task"
#define FRU_EEP_SCAN_TASK_NAME "EepScanTask"

#define FRU_HEADER_TAG_DEFAULT "FRU"
#define FRU_FILE_MAX_LEN 2048

#define KUNPENG_K_TYPE "kunpeng_k_type"
#define KUNPENG_K_TYPE_INVALID_VALUE 0xff


guint8 g_Kunpeng_K_mechine_type = KUNPENG_K_TYPE_INVALID_VALUE;

#define CREAT_SYN_SEM(semid, count)                                                              \
    do {                                                                                         \
        gint32 ret_val_1 = 0;                                                                    \
        ret_val_1 = vos_thread_counter_sem4_create(&(semid), "SYN", (count));                    \
        if (RET_OK != ret_val_1) {                                                               \
            debug_log(DLOG_ERROR, "error: create update_syn_semid fail (ret = %d).", ret_val_1); \
            break;                                                                               \
        }                                                                                        \
                                                                                                 \
        guint32 i = 0;                                                                           \
        for (; i < (count); i++) {                                                               \
            (void)vos_thread_sem4_p((semid), SEM_WAIT_FOREVER);                                  \
        }                                                                                        \
    } while (0)

LOCAL gint32 __frudev_read_fru_from_file(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file);
LOCAL gint32 __frudev_read_fru_from_eeprom(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file);
LOCAL gint32 __frudev_read_huawei_formate_elabel_from_eeprom(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file);
LOCAL gint32 __frudev_read_fru_from_hi51(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file);
LOCAL gint32 __frudev_read_fru_from_psu(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file);
LOCAL gint32 __frudev_read_fru_from_satellite_ctrl(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file);
LOCAL gint32 __frudev_read_fru_from_eeprom_raw(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file);
LOCAL gint32 __frudev_read_fru_from_eeprom_custom(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file);
LOCAL gint32 __frudev_read_fru_from_mcu(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file);
LOCAL gint32 __frudev_read_fan_fru_from_mcu(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file);
LOCAL gint32 frudev_read_fru_from_mcu_custom(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file);
LOCAL gint32 _frudev_read_fru_from_std_e2p(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file);
LOCAL void change_fru_elabel_format(const guint8 *src_data, guint32 src_data_len, FRU_INFO_S *fru_info);
LOCAL gint32 frudev_read_from_intelligent_fan(OBJ_HANDLE frudev_handle, FRU_FILE_S *fru_file);
LOCAL gint32 frudev_read_fru_from_eeprom_double(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file);
LOCAL void __destroy_fru_private_data_func(gpointer data);
LOCAL gint32 init_Kunpeng_K_mechine_type(void);

typedef struct fruid_used_param_s {
    guint32 used_flag;
    guint32 used_position;
} FRUID_USED_PARAM_S;

// 支持FRU对象热添加，动态计算FruId
LOCAL gboolean g_fru_callback = FALSE;

SEMID g_fru_id_flag = 0;

LOCAL FRUID_USED_PARAM_S g_fruid_used_param[MAX_FRU_ID] = {0};

LOCAL GMutex g_fru_position_change_flag_mutex;

GSList *g_fru_position_list = NULL; // Fru设备所在的position，用于热拔插Fru设备完成时匹配，刷新sdrdev仓库


// fru handle list和fru handle list的锁,用于防止同一个fru对象在同一时间初始化两次，出现时序上的冲突导致数据异常
LOCAL GSList *g_fru_in_initing = NULL;
LOCAL SEMID g_fru_handle_list_lock = 0;

LOCAL FRUDEV_READ_FRU g_frudev_read_fru_pf[] = {
    __frudev_read_fru_from_file,           
    __frudev_read_fru_from_eeprom,         
    NULL,                                  
    __frudev_read_fru_from_file,           
    __frudev_read_fru_from_satellite_ctrl, 
    __frudev_read_fru_from_hi51,           
    __frudev_read_fru_from_mcu,            
    // 有些FRU部件eeprom中存放的是原始fru信息，不包含flag、len、crc这些头部信息，需要填充
    __frudev_read_fru_from_eeprom_raw, 
    
    __frudev_read_fru_from_eeprom_custom,            
    __frudev_read_fan_fru_from_mcu,                  
    __frudev_read_fru_from_psu,                      
    _frudev_read_fru_from_std_e2p,                   
    frudev_read_fru_from_eeprom_double,              
    frudev_read_fru_from_mcu_custom,                 
    frudev_read_from_intelligent_fan,                
    __frudev_read_huawei_formate_elabel_from_eeprom, 
    frudev_read_arcard_fru_custom,                   
};

LOCAL gint32 __frudev_read_fru_from_eeprom_raw(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file)
{
    gint32 ret = frudev_read_from_eeprom_raw(object_handle, fru_file);
    if (ret != RET_OK) {
        return ret;
    }

    
    fillin_fru_head(fru_file, FRU_DATA_LEN);

    return RET_OK;
}


LOCAL gint32 __frudev_read_fru_from_eeprom_custom(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file)
{
    gint32 ret = frudev_read_from_eeprom_custom(object_handle, fru_file);
    if (ret != RET_OK) {
        return ret;
    }

    
    fillin_fru_head(fru_file, FRU_DATA_LEN);

    return RET_OK;
}


LOCAL gint32 __frudev_read_fru_from_file(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file)
{
    return frudev_read_from_file(object_handle, fru_file);
}


LOCAL gint32 read_fru_compatible_with_tianchi(OBJ_HANDLE object_handle, guint16 offset, FRU_FILE_S *fru_file)
{
    return frudev_read_from_eeprom(object_handle, offset, fru_file, sizeof(FRU_FILE_S));
}


LOCAL gint32 __frudev_read_fru_from_eeprom(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file)
{
    return frudev_read_from_eeprom(object_handle, 0, fru_file, sizeof(FRU_FILE_S));
}


LOCAL gint32 __frudev_read_huawei_formate_elabel_from_eeprom(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file)
{
    guint8 origin_fru_info[FRU_FILE_MAX_LEN] = { 0 };
    FRU_INFO_S format_fru_info = { 0 };

    gint32 ret = frudev_read_from_eeprom(object_handle, 0, origin_fru_info, FRU_FILE_MAX_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get fru info from eeprom failed, ret is %d", ret);
    }

    change_fru_elabel_format(origin_fru_info, FRU_FILE_MAX_LEN, &format_fru_info);
    ret = generate_fru_file(EEPROM_FORMAT_V2, &format_fru_info, fru_file);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s generate fru file failed, ret=%d.", dfl_get_object_name(object_handle), ret);
    }
    return ret;
}


LOCAL void init_product_fru_from_data(char *src_data, guint16 data_size, FRU_INFO_S *fru_info)
{
    fru_info->has_board = TRUE;
    fru_info->board.area_ver = 0x01;

    
    get_correspond_elabel_data(src_data, PRODUCT_ELABEL_KEY_BAR_CODE, &(fru_info->product.product_serial_num));

    
    get_correspond_elabel_data(src_data, PRODUCT_ELABEL_KEY_ITEM, &(fru_info->product.product_part_num));

    
    get_manufacture_time(src_data, fru_info);

    
    get_correspond_elabel_data(src_data, PRODUCT_ELABEL_KEY_VEN_NAME, &(fru_info->product.product_manufacturer));

    
    get_correspond_elabel_data(src_data, PRODUCT_ELABEL_KEY_BOARD_TYPE, &(fru_info->product.product_name));
}


LOCAL void change_fru_elabel_format(const guint8 *src_data, guint32 src_data_len, FRU_INFO_S *fru_info)
{
    guint8 buf[FRU_FILE_MAX_LEN] = { 0 }; 

    gint32 ret = memcpy_s(buf, sizeof(buf), src_data, src_data_len);
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "memcpy_s data to buf failed, ret=%d, src_data_len=%d, buf_len=%zu", ret, src_data_len,
            sizeof(buf));
        return;
    }

    init_product_fru_from_data((gchar *)buf, FRU_FILE_MAX_LEN, fru_info);
    fru_info->has_product = TRUE;
    init_extend_elabel_from_data((gchar *)buf, FRU_FILE_MAX_LEN, fru_info);
}


LOCAL gint32 _frudev_read_fru_from_std_e2p(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file)
{
    gint32 ret =
        frudev_read_from_eeprom(object_handle, 0, fru_file->frudata, sizeof(FRU_FILE_S) - sizeof(FRU_FILE_HEADER_S));
    if (ret == RET_OK) {
        fillin_fru_head(fru_file, sizeof(fru_file->frudata));
    }
    return ret;
}

LOCAL gint32 frudev_read_fru_from_mcu_custom(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file)
{
    return read_elabel_from_custom_mcu(object_handle, fru_file);
}


LOCAL gint32 __frudev_read_fru_from_satellite_ctrl(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file)
{
    
    return RET_OK;
}


LOCAL gint32 __frudev_read_fru_from_hi51(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file)
{
    return read_fru_info(object_handle, fru_file, get_fru_by_ps_id);
}


LOCAL gint32 __frudev_read_fru_from_psu(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file)
{
    return read_fru_info(object_handle, fru_file, get_frudata_from_psu);
}


LOCAL gint32 __frudev_read_fru_from_mcu(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file)
{
    guint8 fru_type = 0;

    
    gint32 ret = dal_get_property_value_byte(object_handle, PROPERTY_FRU_TYPE, &fru_type);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "call dal_get_property_value_byte failed, error = %d!.", ret);
        return ret;
    }

    if (fru_type == PS_MODULE) {
        return frudev_read_from_power_mcu(object_handle, fru_file);
    }

    return frudev_read_from_mcu(object_handle, fru_file);
}

LOCAL gint32 __frudev_read_fan_fru_from_mcu(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file)
{
    return frudev_read_fan_from_mcu(object_handle, fru_file);
}


LOCAL gint32 frudev_read_fru_from_eeprom_double(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file)
{
    gint32 ret = fru_block_read(object_handle, PROPERTY_FRUDEV_STORAGE_LOC, 0, sizeof(FRU_FILE_S), fru_file);
    if (ret == RET_OK) {
        debug_log(DLOG_INFO, "%s: Read 1st Eeprom Ok! (ret = %d)", __FUNCTION__, ret);
        return ret;
    } else {
        debug_log(DLOG_ERROR, "%s: Read 1st Eeprom Failed! (ret = %d)", __FUNCTION__, ret);
    }

    ret = fru_block_read(object_handle, PROPERTY_FRUDEV_BACK_STORAGE_LOC, 0, sizeof(FRU_FILE_S), fru_file);
    if (ret == RET_OK) {
        debug_log(DLOG_INFO, "%s: Read 2nd Eeprom Ok! (ret = %d)", __FUNCTION__, ret);
    } else {
        debug_log(DLOG_ERROR, "%s: Read 2nd Eeprom Failed! (ret = %d)", __FUNCTION__, ret);
        return ERROR_CODE_READ_E2P_ERROR;
    }

    return RET_OK;
}


LOCAL gint32 frudev_read_from_intelligent_fan(OBJ_HANDLE frudev_handle, FRU_FILE_S *fru_file)
{
    guint16 length = 0;

    gint32 ret = dal_get_property_value_uint16(frudev_handle, PROPERTY_FRUDEV_SIZE, &length);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Check elabel length fail, length = %d, ret = %d", __FUNCTION__, length, ret);
        return ret;
    }

    ret = dfl_block_read(frudev_handle, PROPERTY_FRUDEV_STORAGE_LOC, FANCHIP_OFFSET_ELABEL, length, fru_file);
    if (ret == DFL_FANCHIP_ELABEL_UNINITIALIZED) {
        debug_log(DLOG_ERROR, "%s: Fru(%s) elabel uninitialized.", __FUNCTION__, dfl_get_object_name(frudev_handle));
        
        return RET_OK;
    } else if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: Get fan board elabel fail, ret = %d", __FUNCTION__, ret);
        return ret;
    }

    return RET_OK;
}


LOCAL void __free_fru_id(OBJ_HANDLE object_handle)
{
    guint8 fru_id = 0;
    gint32 ret = dal_get_property_value_byte(object_handle, PROPERTY_FRU_ID, &fru_id);
    if (ret != RET_OK || fru_id >= MAX_FRU_ID) {
        return;
    }

    (void)vos_thread_sem4_p(g_fru_id_flag, SEM_WAIT_FOREVER);
    g_fruid_used_param[fru_id].used_flag = FRU_ID_NOT_USED;
    g_fruid_used_param[fru_id].used_position = 0;
    (void)vos_thread_sem4_v(g_fru_id_flag);
}

LOCAL void free_priv_prop(FRU_PRIV_PROPERTY_S *tmp_priv)
{
    if (tmp_priv->fru_info != NULL) {
        g_free(tmp_priv->fru_info);
    }
    if (tmp_priv->fru_file != NULL) {
        g_free(tmp_priv->fru_file);
    }
    if (tmp_priv->system_info != NULL) {
        g_free(tmp_priv->system_info);
    }
    if (tmp_priv->elabel_info != NULL) {
        g_free(tmp_priv->elabel_info);
    }
    if (tmp_priv->cafe_info != NULL) {
        g_free(tmp_priv->cafe_info);
    }
    if (tmp_priv->tc_header != NULL) {
        g_free(tmp_priv->tc_header);
    }
    if (tmp_priv->area_offsets != NULL) {
        g_free(tmp_priv->area_offsets);
    }
    g_free(tmp_priv);
}


LOCAL gint32 malloc_for_fru_priv_prop(FRU_PRIV_PROPERTY_S *tmp_priv)
{
    (void)memset_s(tmp_priv, sizeof(FRU_PRIV_PROPERTY_S), 0, sizeof(FRU_PRIV_PROPERTY_S));

    tmp_priv->fru_info = (FRU_INFO_S *)g_malloc0(sizeof(FRU_INFO_S));
    if (tmp_priv->fru_info == NULL) {
        goto malloc_failed;
    }

    tmp_priv->fru_file = (FRU_FILE_S *)g_malloc0(sizeof(FRU_FILE_S));
    if (tmp_priv->fru_file == NULL) {
        goto malloc_failed;
    }

    tmp_priv->system_info = (SYSTEM_INFO_U *)g_malloc0(sizeof(SYSTEM_INFO_U));
    if (tmp_priv->system_info == NULL) {
        goto malloc_failed;
    }

    tmp_priv->elabel_info = (ELABEL_INFO_S *)g_malloc0(sizeof(ELABEL_INFO_S));
    if (tmp_priv->elabel_info == NULL) {
        goto malloc_failed;
    }

    tmp_priv->cafe_info = e2p_cafe_new();
    if (tmp_priv->cafe_info == NULL) {
        goto malloc_failed;
    }

    tmp_priv->tc_header = (TC_FILE_HEADER_S *)g_malloc0(sizeof(TC_FILE_HEADER_S));
    if (tmp_priv->tc_header == NULL) {
        goto malloc_failed;
    }

    tmp_priv->area_offsets = (AREA_OFFSET_S *)g_malloc0(sizeof(AREA_OFFSET_S));
    if (tmp_priv->area_offsets == NULL) {
        goto malloc_failed;
    }

    return RET_OK;

malloc_failed:
    free_priv_prop(tmp_priv);
    return RET_ERR;
}


LOCAL gint32 __new_fru_priv_prop(FRU_PRIV_PROPERTY_S **fru_priv_property)
{
    if (fru_priv_property == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return RET_ERR;
    }

    FRU_PRIV_PROPERTY_S *tmp_priv = (FRU_PRIV_PROPERTY_S *)g_malloc0(sizeof(FRU_PRIV_PROPERTY_S));
    if (tmp_priv == NULL) {
        return RET_ERR;
    }

    gint32 ret = malloc_for_fru_priv_prop(tmp_priv);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    tmp_priv->update_status = SYN_NEED_UPDATE;
    *fru_priv_property = tmp_priv;

    return RET_OK;
}

LOCAL gint32 __create_fru_priv_prop(OBJ_HANDLE object_handle, FRU_PRIV_PROPERTY_S **fru_priv_property)
{
    FRU_PRIV_PROPERTY_S *tmp_fru_priv_property = NULL;

    guchar soft_ware_type = MGMT_SOFTWARE_TYPE_UNKNOW;
    (void)dal_get_software_type(&soft_ware_type);
    
    if (soft_ware_type != MGMT_SOFTWARE_TYPE_EM &&
        (!dal_match_product_id(PRODUCT_ID_PANGEA_V6) || soft_ware_type == MGMT_SOFTWARE_TYPE_PANGEA_ALANTIC_CTRL)) {
        gint32 ret = __new_fru_priv_prop(&tmp_fru_priv_property); 
        if ((ret != RET_OK) || (tmp_fru_priv_property == NULL)) {
            debug_log(DLOG_ERROR, "%s __new_fru_priv_prop failed, ret is %d", __FUNCTION__, ret);
            return RET_ERR;
        }

        ret = new_obj_priv_data(object_handle, tmp_fru_priv_property, __destroy_fru_private_data_func);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "bind data failed, ret = %d!(obj:%s).", ret, dfl_get_object_name(object_handle));
            return ret;
        }
        tmp_fru_priv_property->fru_first_init_flag = TRUE;

        *fru_priv_property = tmp_fru_priv_property;
        return RET_OK;
    }

    gint32 ret = get_obj_priv_data(object_handle, (gpointer *)&tmp_fru_priv_property);
    if ((ret != DFL_OK) || (tmp_fru_priv_property == NULL)) {
        ret = __new_fru_priv_prop(&tmp_fru_priv_property);
        if ((ret != RET_OK) || (tmp_fru_priv_property == NULL)) {
            debug_log(DLOG_ERROR, "%s __new_fru_priv_prop failed, ret is %d", __FUNCTION__, ret);
            return RET_ERR;
        }

        ret = new_obj_priv_data(object_handle, tmp_fru_priv_property, __destroy_fru_private_data_func);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "bind data failed, ret = %d!(obj:%s).", ret, dfl_get_object_name(object_handle));
            return ret;
        }
        tmp_fru_priv_property->fru_first_init_flag = TRUE;
    }

    *fru_priv_property = tmp_fru_priv_property;
    return RET_OK;
}


LOCAL gint32 __allot_fruid(OBJ_HANDLE object_handle, GSList *list, guint8 *fruid)
{
    if (fruid == NULL || list == NULL) {
        debug_log(DLOG_ERROR, "[%s][%d] Invalid parameter!", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    guint8 fru_id_origin = 0;
    (void)dal_get_property_value_byte(object_handle, PROPERTY_FRU_ID, &fru_id_origin);
    if (fru_id_origin > FRU_ID_MAX_AUTO_ALLOC) {
        *fruid = fru_id_origin;
        return RET_OK;
    }

    guint32 fru_pos = 0;
    (void)dal_get_property_value_uint32(object_handle, PROPERTY_FRU_POSITION, &fru_pos);

    guint8 pos_offset = 0;
    GSList *tmp_list = list;

    while (tmp_list) {
        GSList *next = tmp_list->next;
        DFT_COMPONENT_POSN_INFO_S *pos_info = (DFT_COMPONENT_POSN_INFO_S *)tmp_list->data;
        tmp_list = next;
        if (fru_pos != pos_info->position_id) {
            pos_offset++;
            continue;
        }

        
        (void)vos_thread_sem4_p(g_fru_id_flag, SEM_WAIT_FOREVER);

        
        while ((pos_offset < MAX_FRU_ID - 1) && (g_fruid_used_param[pos_offset].used_flag == FRU_ID_IS_USED) &&
            (fru_pos != g_fruid_used_param[pos_offset].used_position)) {
            debug_log(DLOG_ERROR, "%s %s fru_id:%d is used position=%d", __FUNCTION__,
                dfl_get_object_name(object_handle), pos_offset, fru_pos);
            pos_offset++;
        }

        g_fruid_used_param[pos_offset].used_flag = FRU_ID_IS_USED;
        g_fruid_used_param[pos_offset].used_position = fru_pos;

        (void)vos_thread_sem4_v(g_fru_id_flag);
        break;
    }

    *fruid = pos_offset;
    (void)dal_set_property_value_byte(object_handle, PROPERTY_FRU_ID, pos_offset, DF_NONE);

    return RET_OK;
}


LOCAL gint32 init_fru_id(OBJ_HANDLE object_handle, guint8 *fruid)
{
    lock_connector();
    
    init_connectors();

    gint32 ret = __allot_fruid(object_handle, get_connectors(), fruid);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s alloc fruid failed, ret=%d.", dfl_get_object_name(object_handle), ret);
    }

    unlock_connector();
    return ret;
}

LOCAL void __get_locator_record_id(guint8 fru_id, guint16 *record_id)
{
    if (record_id == NULL) {
        debug_log(DLOG_ERROR, "[%s][%d] Invalid parameter!", __FUNCTION__, __LINE__);
        return;
    }

    
    if (fru_id != 0) {
        
        *record_id = 0;
        return;
    }

    
    OBJ_HANDLE obj_handle = 0;
    gint32 ret = dfl_get_object_handle(OBJECT_SDR_DEV, &obj_handle);
    if (ret != DFL_OK) {
        *record_id = 0;
        return;
    }

    (void)dal_get_property_value_uint16(obj_handle, PROPERTY_SDR_TYPE12_RECID, record_id);

    return;
}
LOCAL void __get_fru_pcb_id(OBJ_HANDLE object_handle, guint8 *pcb_id)
{
    gint32 ret;
    for (int i = 0; i < 5; i++) {
        ret = dal_get_property_value_byte(object_handle, PROPERTY_FRU_PCBID, pcb_id);
        if (ret == RET_OK) {
            return;
        }

        vos_task_delay(200);
    }

    debug_log(DLOG_ERROR, "%s get pcbid failed, ret = %d", __FUNCTION__, ret);
    return;
}

LOCAL void __init_fru_pcb_ver(OBJ_HANDLE object_handle, guint8 pcb_id)
{
    if (pcb_id == 0) {
        return;
    }

    gchar buf[PCB_VERSION_BUF_SIZE] = {'.', 'A', '\0'};
    buf[PCB_VER_VAR_OFFSET] += pcb_id - 1;
    (void)dal_set_property_value_string(object_handle, PROPERTY_FRU_PCB_VERSION, buf, DF_NONE);

    return;
}


LOCAL gint32 __get_ipmb_addr(guint8 *ipmb_addr)
{
    GSList *list = NULL;
    gint32 ret = dfl_get_object_list(CLASS_NAME_IPMBADDR, &list);
    if (ret != DFL_OK) {
        *ipmb_addr = 0x20;
        return RET_OK;
    }

    OBJ_HANDLE obj_handle = (OBJ_HANDLE)g_slist_nth_data(list, 0);
    g_slist_free(list);

    return dal_get_property_value_byte(obj_handle, IPMBADDR_PROPERTY_SA, ipmb_addr);
}


LOCAL gint32 __get_board_slot_id(guint8 *slot_id)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret = dfl_get_object_handle(BMC_BOARD_NAME, &obj_handle);
    if (ret != DFL_OK) {
        *slot_id = 0xff;
        return RET_OK;
    }

    return dal_get_property_value_byte(obj_handle, BMC_SLOT_ID_NAME, slot_id);
}

LOCAL gint32 __get_bmc_version(gchar *bmc_ver, guint32 ver_size)
{
    OBJ_HANDLE bmc_global_obj = 0;
    gint32 ret = dfl_get_object_handle(BMC_OBJECT_NAME, &bmc_global_obj);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "call dfl_get_object_handle failed, error = %d!.", ret);
        return ret;
    }

    return dal_get_property_value_string(bmc_global_obj, BMC_MANA_VER_NAME, bmc_ver, ver_size);
}

LOCAL gint32 save_product_info(guint8 device_type)
{
    gint32 ret = save_product_uid(device_type);
    ret += save_product_name(device_type);
    ret += save_product_newname(device_type);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: save to product failed, ret=%d", __FUNCTION__, ret);
        return COMP_CODE_UNKNOWN;
    }
    maintenance_log_v2(MLOG_INFO, FC__PUBLIC_OK, "Set Machine Identification Code to (%hhu) successfully",
                       device_type);
    return ret;
}


LOCAL gint32 recover_device_type_from_eeprom(FRU_PROPERTY_S *fru_property)
{
#define GET_DEVICE_TYPE_DELAY 1000
#define GET_DEVICE_TYPE_E2P_LOOP 3
#define TS2280_TYPE 0x00
#define TS5280_TYPE 0x01
#define TS2280K_TYPE 0x0a
#define TS5280K_TYPE 0x0b

    gint32 ret;
    guint8 device_type = 0;
    guint8 i = 0;
    DEVICE_TYPE_STR_AREA str_area = { 0 };

    if (fru_property->fru_id != 0) {
        return RET_OK;
    }

    do {
        ret = fru_block_read(fru_property->frudev_handle, PROPERTY_FRUDEV_STORAGE_LOC, DEVICE_EEP_TYPE_OFFSET,
            DEVICE_EEP_TYPE_EEP_SIZE, (guint8 *)&str_area);
        if (ret == RET_OK) {
            device_type = str_area.device_type_str[0];
            break;
        }
        vos_task_delay(GET_DEVICE_TYPE_DELAY);
        i++;
    } while (i < GET_DEVICE_TYPE_E2P_LOOP);

    if (ret == RET_OK) {
        if (device_type == TS2280_TYPE || device_type == TS5280_TYPE ||
            device_type == TS2280K_TYPE || device_type == TS5280K_TYPE) {
            ret = init_Kunpeng_K_mechine_type();
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s: init kunpeng device type fail, ret=%d", __FUNCTION__, ret);
                return RET_ERR;
            }
            g_Kunpeng_K_mechine_type = device_type;
            per_save(&g_Kunpeng_K_mechine_type);
        }
    } else {
        // 获取E2P信息失败时，需要取上一次持久化成功的值
        device_type = g_Kunpeng_K_mechine_type;
        debug_log(DLOG_ERROR, "%s: failed to recover device type:%hhu, ret=%d", __FUNCTION__, device_type, ret);
    }

    if (device_type == TS2280K_TYPE || device_type == TS5280K_TYPE) {
        ret = save_product_info(device_type);
    }
    return ret;
}


LOCAL gint32 __init_frudev_handle_info(FRU_PROPERTY_S *fru_property, gint is_customize)
{
    (void)dal_get_property_value_uint16(fru_property->frudev_handle, PROPERTY_FRUDEV_SIZE,
        &(fru_property->frudev_inv_size));

    (void)dal_get_property_value_byte(fru_property->frudev_handle, PROPERTY_FRUDEV_STORAGE_TYPE,
        &(fru_property->frudev_type));
    if (fru_property->frudev_type < ARRAY_SIZE(g_frudev_read_fru_pf)) {
        fru_property->frudev_read_fru_pf = g_frudev_read_fru_pf[fru_property->frudev_type];
    }

    (void)tc_update_product_info(fru_property);
    
    (void)recover_device_type_from_eeprom(fru_property);

    
    gint32 ret = get_product_name(is_customize, fru_property->product_name, sizeof(fru_property->product_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "call get_product_name failed, error = %d!.", ret);
        return ret;
    }

    
    ret = __get_ipmb_addr(&(fru_property->ipmb_addr));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "call __get_ipmb_addr failed, error = %d!.", ret);
        return ret;
    }

    
    ret = __get_board_slot_id(&(fru_property->slot_id));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "call _get_board_slotid failed, error = %d!.", ret);
        return ret;
    }

    ret = __get_bmc_version(fru_property->fru_ver, MAX_BMC_VER_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "__get_bmc_version error, ret = %d", ret);
        return ret;
    }

    return RET_OK;
}

LOCAL void init_prop_from_elabel_handle(FRU_PROPERTY_S *fru_property)
{
    (void)dal_get_property_value_byte(fru_property->elabel_handle, PROPERTY_ELABEL_PARSER_PROCOTOL,
        &(fru_property->parser_protocol));

    (void)dal_get_property_value_byte(fru_property->elabel_handle, PROPERTY_ELABEL_IS_SUPPORT_DFT,
        &(fru_property->is_sup_dft));

    (void)dal_get_property_value_byte(fru_property->elabel_handle, PROPERTY_ELABEL_IS_CONTAIN_SYS_AREA,
        &(fru_property->is_cont_sys_area));

    (void)dal_get_property_value_byte(fru_property->elabel_handle, PROPERTY_ELABEL_IS_SUPPORT_CONFIG_AREA,
        &fru_property->is_support_config_area);
}

LOCAL gint32 get_fru_offset(FRU_PROPERTY_S *fru_property)
{
    guint8 eeprom_format = EEPROM_FORMAT_V2;
    if (get_eeprom_format(fru_property->frudev_handle, &eeprom_format) != RET_OK) {
        return RET_ERR;
    }

    if (eeprom_format != EEPROM_FORMAT_TIANCHI) {
        fru_property->fru_offset = 0;
        return RET_OK;
    }

    gint32 ret = frudev_read_from_eeprom(fru_property->frudev_handle, TC_HEADER_FRU_OFFSET, &fru_property->fru_offset,
        sizeof(guint16));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: frudev_read_from_eeprom fail, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    fru_property->fru_offset *= OFFSET_WORD_LENGTH;

    return RET_OK;
}


LOCAL gint32 __init_fru_priv_prop(OBJ_HANDLE object_handle, FRU_PROPERTY_S *fru_property, SYSTEM_INFO_U *system_info)
{
    if (fru_property == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return RET_ERR;
    }

    fru_property->fru_handle = object_handle;

    
    gint32 ret = init_fru_id(object_handle, &(fru_property->fru_id));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s call init_fru_id failed, ret = %d", dfl_get_object_name(object_handle), ret);
        return ret;
    }

    
    fru_property->eeprom_accessor_prop = PROPERTY_FRUDEV_STORAGE_LOC;
    __get_locator_record_id(fru_property->fru_id, &(fru_property->locator_record_id));

    (void)dal_get_property_value_byte(object_handle, PROPERTY_FRU_TYPE, &(fru_property->fru_type));

    __get_fru_pcb_id(object_handle, &(fru_property->pcb_id));

    __init_fru_pcb_ver(object_handle, fru_property->pcb_id);

    (void)dal_get_property_value_byte(fru_property->fru_handle, PROPERTY_FRU_SITE_TYPE, &(fru_property->site_type));

    
    ret = dfl_get_referenced_object(object_handle, PROPERTY_FRU_ELABEL_RO, &(fru_property->elabel_handle));
    if (ret != DFL_OK) {
        debug_log(DLOG_INFO, "%s This fru is not support elabel.", __FUNCTION__);
        return RET_OK;
    }

    init_prop_from_elabel_handle(fru_property);

    
    ret = dfl_get_referenced_object(object_handle, PROPERTY_FRUDEV_RO, &(fru_property->frudev_handle));
    if (ret != DFL_OK) {
        return RET_OK;
    }

    ret = get_fru_offset(fru_property);
    if (ret != RET_OK) {
        return ret;
    }

    ret = __init_frudev_handle_info(fru_property, (fru_property->fru_offset == 0) ?
        system_info->system_area_st.is_customize :
        system_info->tc_system_desc_st.is_customize);
    if (ret != RET_OK) {
        return ret;
    }

    
    fru_property->rimm_obj_handle = 0;
    ret = dfl_get_referenced_object(object_handle, PROPERTY_FRU_ELABEL_RIMM_OBJ, &(fru_property->rimm_obj_handle));
    if (ret != DFL_OK) {
        return RET_OK;
    }

    
    fru_property->dft_operate_type = MCU_WRITE_ELABEL;

    return RET_OK;
}


LOCAL void __check_task_exited(gulong *task_id, const gchar *task_name, guint16 timeout)
{
    if (task_id == NULL) {
        return;
    }

    guint32 idx = 0;
    while (idx < timeout) {
        if (*task_id == 0) {
            break;
        }

        vos_task_delay(100);
        idx++;
    };

    if (idx == timeout) {
        debug_log(DLOG_ERROR, "The task %lu:%s is not terminated in timeout", *task_id, task_name);
    }

    return;
}


LOCAL void __delete_syn_frudata_task(FRU_PRIV_PROPERTY_S **fru_priv_property, gboolean need_rm_eep_task)
{
    gchar task_name[MAX_FRU_SYN_TASK_NAME] = {0};
    FRU_PRIV_PROPERTY_S *p_fru_priv = *fru_priv_property;

    if (p_fru_priv->update_task_id != 0) {
        p_fru_priv->update_task_state = FALSE;
        (void)vos_thread_sem4_v(p_fru_priv->update_syn_semid);
        gint32 ret = snprintf_s(task_name, sizeof(task_name), sizeof(task_name) - 1, "%s%d", FRU_SYNC_TASK_NAME,
            p_fru_priv->fru_property.fru_id);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        }

        __check_task_exited(&p_fru_priv->update_task_id, task_name, 100); // 100：timeout
        p_fru_priv->update_task_id = 0; // 任务退出时会将任务ID清0，此处保留，对功能不影响
    }

    if (p_fru_priv->update_syn_semid != 0) {
        (void)vos_thread_sem4_delete(p_fru_priv->update_syn_semid);
        p_fru_priv->update_syn_semid = 0;
    }

    if (p_fru_priv->update_sys_task_id != 0) {
        p_fru_priv->update_sys_task_state = FALSE;
        (void)vos_thread_sem4_v(p_fru_priv->update_sys_syn_semid);
        __check_task_exited(&p_fru_priv->update_sys_task_id, SYS_FIELD_SYNC_TASK_NAME, 100); // 100：timeout
        p_fru_priv->update_sys_task_id = 0; // 任务退出时会将任务ID清0，此处保留，对功能不影响
    }

    if (p_fru_priv->update_sys_syn_semid != 0) {
        (void)vos_thread_sem4_delete(p_fru_priv->update_sys_syn_semid);
        p_fru_priv->update_sys_syn_semid = 0;
    }

    if (p_fru_priv->elabel_task_id != 0) {
        p_fru_priv->elabel_task_state = FALSE;
        (void)vos_thread_sem4_v(p_fru_priv->elabel_syn_semid);
        gint32 ret = snprintf_s(task_name, sizeof(task_name), sizeof(task_name) - 1, "%s%d", ELABEL_SYNC_TASK_NAME,
            p_fru_priv->fru_property.fru_id);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        }
        __check_task_exited(&p_fru_priv->elabel_task_id, task_name, 100); // 100：timeout
        p_fru_priv->elabel_task_id = 0; // 任务退出时会将任务ID清0，此处保留，对功能不影响
    }

    if (p_fru_priv->elabel_syn_semid != 0) {
        (void)vos_thread_sem4_delete(p_fru_priv->elabel_syn_semid);
        p_fru_priv->elabel_syn_semid = 0;
    }

    if (p_fru_priv->eep_scan_task_id != 0 && need_rm_eep_task == TRUE) {
        p_fru_priv->eep_scan_task_state = FALSE;
        __check_task_exited(&p_fru_priv->eep_scan_task_id, FRU_EEP_SCAN_TASK_NAME, 100); // 100：timeout
        p_fru_priv->eep_scan_task_id = 0; // 任务退出时会将任务ID清0，此处保留，对功能不影响
    }

    return;
}


LOCAL void __destroy_fru_priv_prop(FRU_PRIV_PROPERTY_S **fru_priv_property_pp)
{
    if ((fru_priv_property_pp == NULL) || (*fru_priv_property_pp == NULL)) {
        return;
    }

    FRU_PRIV_PROPERTY_S *fru_priv_property = *fru_priv_property_pp;

    gint32 ret = dal_destroy_fru_data(fru_priv_property->fru_info);
    if (ret == RET_ERR) {
        debug_log(DLOG_ERROR, "%s failed with ret:%d!", __FUNCTION__, ret);
    }

    fru_priv_property->fru_info = NULL;
    if (fru_priv_property->fru_file) {
        g_free(fru_priv_property->fru_file);
        fru_priv_property->fru_file = NULL;
    }
    if (fru_priv_property->system_info) {
        g_free(fru_priv_property->system_info);
        fru_priv_property->system_info = NULL;
    }

    ret = e2p_cafe_free(fru_priv_property->cafe_info);
    if (ret == RET_ERR) {
        debug_log(DLOG_ERROR, "%s failed with ret:%d!", __FUNCTION__, ret);
    }

    g_free(fru_priv_property);
    *fru_priv_property_pp = NULL;

    return;
}


LOCAL void __frudata_async_deconstructor(void *arg)
{
    (void)prctl(PR_SET_NAME, (gulong) "FruAsyncDel");

    if (arg == NULL) {
        return;
    }

    FRU_PRIV_PROPERTY_S *fru_priv_property = (FRU_PRIV_PROPERTY_S *)arg;
    if (fru_priv_property == NULL) {
        return;
    }

    __delete_syn_frudata_task(&fru_priv_property, TRUE);
    __destroy_fru_priv_prop(&fru_priv_property);

    return;
}


LOCAL void __destroy_fru_private_data_func(gpointer data)
{
    if (data == NULL) {
        debug_log(DLOG_ERROR, "%s Input pointer is NULL.", __FUNCTION__);
        return;
    }

    gulong async_del_task_id = 0;
    gint32 ret = vos_task_create(&async_del_task_id, "AsyncDeconstructor", (TASK_ENTRY)__frudata_async_deconstructor,
        data, DEFAULT_PRIORITY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s Create task to delete object private data failed, error = %d.", __FUNCTION__, ret);
    }

    return;
}

LOCAL void __clear_fru_init_data(OBJ_HANDLE object_handle, FRU_PRIV_PROPERTY_S *fru_priv_property)
{
    debug_log(DLOG_ERROR, "init fru property failed for %s", dfl_get_object_name(object_handle));
    if (fru_priv_property == NULL) {
        debug_log(DLOG_ERROR, "%s private data ptr is null.", dfl_get_object_name(object_handle));
        return;
    }

    guint8 soft_ware_type = MGMT_SOFTWARE_TYPE_INVALID;
    (void)dal_get_software_type(&soft_ware_type);

    __delete_syn_frudata_task(&fru_priv_property, FALSE);
    if (!fru_priv_property->fru_first_init_flag) {
        put_obj_priv_data(object_handle, (gpointer)fru_priv_property);
    } else {
        if (soft_ware_type != MGMT_SOFTWARE_TYPE_EM && dal_match_product_id(PRODUCT_ID_PANGEA_V6) == FALSE) {
            put_obj_priv_data(object_handle, (gpointer)fru_priv_property);
        }
        fru_priv_property->fru_first_init_flag = FALSE;
    }
}


LOCAL gboolean check_is_allow_access(OBJ_HANDLE handle)
{
    guint8 flag = 0xff;
    gint32 ret = dal_get_property_value_byte(handle, PROPERTY_IS_ALLOW_ACCESS, &flag);
    if (ret != RET_OK) {
        return FALSE;
    }

    return (flag == 1) ? TRUE : FALSE;
}


LOCAL gint32 __check_init_in_standby_smm(OBJ_HANDLE handle)
{
    // Get fru type'
    guint8 fru_type = 0;
    (void)dal_get_property_value_byte(handle, PROPERTY_FRU_TYPE, &fru_type);
    if (fru_type != FRU_TYPE_POWER && fru_type != FRU_TYPE_BACKPLANE && fru_type != FRU_TYPE_FANTRAY &&
        fru_type != FRU_TYPE_EPM && fru_type != FRU_TYPE_FAN_BOARD && fru_type != FRU_TYPE_EXP) {
        // 非以上部件，备用板可以进行初始化
        return RET_OK;
    }

    
    if (dal_match_software_type(MGMT_SOFTWARE_TYPE_PANGEA_ALANTIC_CTRL)) {
        return RET_OK;
    }

    // 获取主备状态
    OBJ_HANDLE asm_handle = 0;
    gint32 ret = dfl_get_object_handle(ASM_OBJECT_NAME, &asm_handle);
    if (ret != DFL_OK) { // 获取主备失败，不读取
        debug_log(DLOG_ERROR, "[%s] Get asm object failed, err code %d", __func__, ret);
        return RET_ERR;
    }

    guint8 as_status = ASM_STAND_BY;
    (void)dal_get_property_value_byte(asm_handle, PROPERTY_ASM_AS_STATUS, &as_status);
    if (as_status != ASM_ACTIVE) {
        debug_log(DLOG_INFO, "[%s] This is stand by smm, can not init fru[%s]", __func__, dfl_get_object_name(handle));
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 _write_elabel_to_intelligent_fan(OBJ_HANDLE frudev_handle, FRU_FILE_S *fru_file)
{
    guint16 length = 0;

    
    gint32 ret = dal_get_property_value_uint16(frudev_handle, PROPERTY_FRUDEV_SIZE, &length);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Check elabel length fail, length = %d, ret = %d", __FUNCTION__, length, ret);
        return ret;
    }

    
    ret = fru_block_write(frudev_handle, PROPERTY_FRUDEV_STORAGE_LOC, FANCHIP_OFFSET_ELABEL, length, fru_file);
    if (ret != FRUDATA_OK) {
        debug_log(DLOG_ERROR, "%s: Set fan board(%s) elabel fail, ret = %d", __FUNCTION__,
            dfl_get_object_name(frudev_handle), ret);
    }

    return ret;
}


LOCAL gint32 write_fru_file(guint8 eeprom_fromat, FRU_PROPERTY_S *fru_prop, FRU_FILE_S *fru_file)
{
    if (fru_file == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return SYN_DFL_FAIL;
    }

    gint32 ret = 0;
    guint32 length = (eeprom_fromat == EEPROM_FORMAT_TIANCHI) ? FRU_MAX_LEN :
                (fru_file->header.fru_head.len + sizeof(FRU_FILE_HEADER_S));

    
    switch (fru_prop->frudev_type) {
        case STORAGE_TYPE_FILE:
            ret = frudev_write_file_withoffset(fru_prop->frudev_handle, fru_prop->fru_offset, fru_file, length);
            break;
        case INTELLIGENT_NON_REPOSITY:
            ret = frudev_write_to_file(fru_prop->frudev_handle, fru_file);
            break;
        case STORAGE_TYPE_EEPROM:
            ret = frudev_write_to_eeprom(fru_prop->frudev_handle, fru_prop->fru_offset, fru_file, length);
            break;
        case STORAGE_TYPE_STD_E2P:
            ret = frudev_write_to_eeprom(fru_prop->frudev_handle, 0, fru_file->frudata, fru_file->header.fru_head.len);
            break;
        case STORAGE_TYPE_MCU:
            ret = syn_frudata_to_mcu(fru_prop->fru_handle, fru_prop);
            break;
        case STORAGE_TYPE_MCU_FAN:
            ret = syn_frudata_to_fan_mcu(fru_prop->frudev_handle, 0, (guint8 *)(fru_file),
                fru_file->header.fru_head.len + sizeof(FRU_FILE_HEADER_S));
            break;
        case STORAGE_TYPE_E2P_CUSTOM:
            ret = frudev_write_to_eeprom_custom(fru_prop->frudev_handle, 0, (guint8 *)fru_file,
                fru_file->header.fru_head.len + sizeof(FRU_FILE_HEADER_S));
            break;
        case STORAGE_TYPE_E2P_DOUBLE:
            ret = frudev_write_to_eeprom_double(fru_prop->frudev_handle, 0, fru_file,
                fru_file->header.fru_head.len + sizeof(FRU_FILE_HEADER_S));
            break;
        case STORAGE_TYPE_MCU_CUSTOM:
            ret = _write_elabel_to_mcu_custom(fru_prop->fru_handle, fru_file);
            break;
        case STORAGE_TYPE_MCU_INTELLIGENT_FAN:
            ret = _write_elabel_to_intelligent_fan(fru_prop->frudev_handle, fru_file);
            break;
        case INTELLIGENT_REPOSITY:
        case STORAGE_TYPE_FLASH:
        default:
            debug_log(DLOG_INFO, "Fru does not support syn!.");

            return SYN_FRU_OK;
    }

    return ret;
}

LOCAL gint32 __init_fru_file(FRU_PRIV_PROPERTY_S *fru_priv_property)
{
    gchar fru_dev_class_name[MAX_NAME_SIZE] = {0};
    if (fru_priv_property->fru_property.frudev_handle != 0) {
        gint32 ret =
            dfl_get_class_name(fru_priv_property->fru_property.frudev_handle, fru_dev_class_name, MAX_NAME_SIZE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s dfl_get_class_name error, ret = %d", __FUNCTION__, ret);
            return ret;
        }
    }

    guchar soft_ware_type = MGMT_SOFTWARE_TYPE_UNKNOW;
    (void)dal_get_software_type(&soft_ware_type);

    guint8 fru_type = 0;
    (void)dal_get_property_value_byte(fru_priv_property->fru_property.fru_handle, PROPERTY_FRU_TYPE, &fru_type);

    if ((g_strcmp0(fru_dev_class_name, CLASS_CAFE_DEV) != 0) && (fru_type != FRU_TYPE_BACKPLANE)) {
        return RET_OK;
    }

    FRU_INFO_S tmp_fru_info = { 0 };
    gint32 ret = RET_ERR;
    if (fru_type == FRU_TYPE_BACKPLANE && !dal_match_product_id(PRODUCT_ID_PANGEA_V6)) {
        ret = init_smm_backplane_fru_file(&(fru_priv_property->fru_property), &tmp_fru_info);
    }

    if (g_strcmp0(fru_dev_class_name, CLASS_CAFE_DEV) == 0) {
        
        init_default_fru_info(&(fru_priv_property->fru_property), &tmp_fru_info);
        ret = init_fru_info_from_cafe_info(fru_priv_property->cafe_info, &tmp_fru_info);
    }
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR,
            "%s init fru info error, ret = %d, fru_dev_class_name is %s, soft_ware_type is %d, fru_type is %d",
            __FUNCTION__, ret, fru_dev_class_name, soft_ware_type, fru_type);
        return ret;
    }

    
    FRU_FILE_S tmp_fru_file = { { .fru_head = { 0 } }, { 0 } };
    (void)generate_fru_file(fru_priv_property->eeprom_format, &tmp_fru_info, &tmp_fru_file);
    ret = write_fru_file(fru_priv_property->eeprom_format, &(fru_priv_property->fru_property), &tmp_fru_file);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: write fru file failed, ret is %d", __FUNCTION__, ret);
    }

    return ret;
}


LOCAL gint32 __read_fru_from_eeprom(FRU_PROPERTY_S *fru_prop, FRU_FILE_S *fru_file, guint16 fru_data_len)
{
    if (fru_prop->frudev_read_fru_pf == NULL) {
        debug_log(DLOG_ERROR, "parameter error = %d!", fru_prop->frudev_type);
        return ERROR_CODE_PARAMETER_INVALID;
    }

    gint32 ret;
    for (gint32 retry_times = 3; retry_times > 0; retry_times--) {
        if (fru_prop->frudev_type == STORAGE_TYPE_MCU || fru_prop->frudev_type == STORAGE_TYPE_MCU_CUSTOM) {
            ret = fru_prop->frudev_read_fru_pf(fru_prop->fru_handle, fru_file);
        } else if (fru_prop->frudev_type == STORAGE_TYPE_EEPROM) {
            ret = read_fru_compatible_with_tianchi(fru_prop->frudev_handle, fru_prop->fru_offset, fru_file);
        } else {
            ret = fru_prop->frudev_read_fru_pf(fru_prop->frudev_handle, fru_file);
        }

        if (ret == RET_OK) {
            debug_log(DLOG_DEBUG, "%s frudev_read_fru_pf success!", dfl_get_object_name(fru_prop->frudev_handle));
            break;
        }

        
        if (retry_times > 1) {
            debug_log(DLOG_ERROR, "Warning: %s frudev_read_fru_pf failed!",
                dfl_get_object_name(fru_prop->frudev_handle));
            continue;
        }

        (void)dal_set_property_value_byte(fru_prop->frudev_handle, PROPERTY_FRUDEV_HEALTH, FRU_HEALTH_FAIL, DF_NONE);
        debug_log(DLOG_ERROR, "Warning: %s __read_fru_from_eeprom failed!",
            dfl_get_object_name(fru_prop->frudev_handle));
        return ret;
    }

    if (fru_prop->frudev_type == STORAGE_TYPE_STD_E2P) {
        fillin_fru_head(fru_file, fru_data_len);
    }

    return RET_OK;
}


LOCAL void _frudev_multi_write_eeprom(OBJ_HANDLE object_handle, guint8 *multi_record, guint32 length)
{
    guint8 retry_times = 0;
    guint32 offset = ELABEL_MULTI_AREA_OFFSET;
    if (multi_record == NULL) {
        debug_log(DLOG_ERROR, "[%s]Fru multi record area Parameter is NULL.", __FUNCTION__);
        return;
    }

    while (frudev_write_to_eeprom(object_handle, offset, multi_record, length) != FRUDATA_OK) {
        debug_log(DLOG_INFO, "[%s]update internal/chassis/product extend elabel info fail!, offset=%d.", __FUNCTION__,
            offset);
        if (retry_times > WRITE_EEPROM_RETRY_TIMES) {
            break;
        } else {
            vos_task_delay(TASK_DELAY_ONE_SECOND);
            retry_times++;
        }
    }
    return;
}

LOCAL gint32 __check_fru_from_eeprom(FRU_FILE_S *fru_file, FRU_PROPERTY_S *fru_prop)
{
    FRU_FILE_S *fru_tmp = (FRU_FILE_S *)g_malloc0(sizeof(FRU_FILE_S));
    if (fru_tmp == NULL) {
        return SYN_FRU_OK; // 弱校验，返回成功
    }

    guint8 eeprom_format = (fru_prop->fru_offset == 0) ? EEPROM_FORMAT_V2 : EEPROM_FORMAT_TIANCHI;
    guint16 fru_data_len = (eeprom_format == EEPROM_FORMAT_TIANCHI) ? FRU_MAX_LEN : fru_file->header.fru_head.len;
    gint32 check_result = SYN_FRU_OK;
    for (gint32 read_retry_times = 3; read_retry_times > 0; read_retry_times--) {
        (void)memset_s(fru_tmp, sizeof(FRU_FILE_S), 0, sizeof(FRU_FILE_S));
        gint32 ret = __read_fru_from_eeprom(fru_prop, fru_tmp, fru_data_len);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s __read_fru_from_eeprom fail!", dfl_get_object_name(fru_prop->frudev_handle));
            ret = SYN_OPEN_FILE_FAIL;
            vos_task_delay(TASK_DELAY_200_MS);
            continue;
        }
        ret = check_fru_file(eeprom_format, fru_tmp);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Warning: %s fru file is destroyed! ret = %d.",
                dfl_get_object_name(fru_prop->frudev_handle), ret);
            if (ret == ERROR_CODE_HEADER_ERROR && fru_prop->frudev_type == STORAGE_TYPE_EEPROM) {
                try_fix_fru_header(fru_prop->frudev_handle);
                vos_task_delay(TASK_DELAY_200_MS);
                continue;
            }
            ret = SYN_OPEN_FILE_FAIL;
            vos_task_delay(TASK_DELAY_200_MS);
            continue;
        }

        debug_log(DLOG_DEBUG, "%s read fru from eeprom success!", dfl_get_object_name(fru_prop->frudev_handle));

        
        guint16 fru_len = (eeprom_format == EEPROM_FORMAT_TIANCHI) ?
            FRU_MAX_LEN :
            (fru_file->header.fru_head.len + sizeof(FRU_FILE_HEADER_S));
        if (memcmp(fru_tmp, fru_file, fru_len) != 0) {
            debug_log(DLOG_ERROR, "%s memcmp fru fail!", dfl_get_object_name(fru_prop->frudev_handle));
            ret = SYN_WRITE_E2P_FAIL;
            break;
        }

        debug_log(DLOG_DEBUG, "%s memcmp fru success!", dfl_get_object_name(fru_prop->frudev_handle));
        vos_task_delay(TASK_DELAY_200_MS);
    }

    g_free(fru_tmp);
    return check_result;
}


LOCAL void __task_syn_fru(FRU_PRIV_PROPERTY_S *argv1)
{
    (void)prctl(PR_SET_NAME, (gulong) "FruSynTask");

    OBJ_HANDLE obj_handle = (OBJ_HANDLE)argv1;
    FRU_PRIV_PROPERTY_S *fru_pro = NULL;
    gint32 ret = get_obj_priv_data(obj_handle, (gpointer *)&fru_pro);
    if (ret != RET_OK) {
        return;
    }
    fru_pro->update_task_state = TRUE;
    while (fru_pro->update_task_state) {
        (void)vos_thread_sem4_p(fru_pro->update_syn_semid, SEM_WAIT_FOREVER);

        
        if (!fru_pro->update_task_state) {
            continue;
        }

        fru_pro->update_status = SYN_UPDATING;

        
        (void)memset_s(fru_pro->fru_file, sizeof(FRU_FILE_S), 0, sizeof(FRU_FILE_S));
        
        gint32 ret = generate_fru_file(fru_pro->eeprom_format, fru_pro->fru_info, fru_pro->fru_file);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, " generate the %s fru file ret=%d",
                dfl_get_object_name(fru_pro->fru_property.frudev_handle), ret);
        }

        
        
        ret = write_fru_file(fru_pro->eeprom_format, &(fru_pro->fru_property), fru_pro->fru_file);

        
        fru_pro->update_fault = ret;
        if (ret == RET_OK && is_open_fru_read_back() == 1) {
            fru_pro->update_fault = __check_fru_from_eeprom(fru_pro->fru_file, &(fru_pro->fru_property));
        }

        
        if (fru_pro->update_fault == SYN_FRU_OK) {
            (void)set_elabel_property(&(fru_pro->fru_property), fru_pro->fru_info);
        }
        
        if (!fru_pro->upgrate_state) {
            send_ver_change_evt(0x00);
        } else {
            send_ver_change_evt(0x02);
            fru_pro->upgrate_state = FALSE;
        }

        fru_pro->update_status = SYN_UPDAT_COMPLETED;
    }

    // 任务退出时将任务ID清0，避免内存清理的任务执行太快，导致非法地址访问
    fru_pro->update_task_id = 0;
    put_obj_priv_data(obj_handle, (gpointer)fru_pro);
}


LOCAL void _frudev_write_eeprom(FRU_PRIV_PROPERTY_S *fru_priv_property, ELABEL_INFO_S *elabel_tmp, guint32 extendsize)
{
    if (fru_priv_property == NULL || elabel_tmp == NULL) {
        debug_log(DLOG_ERROR, "[%s]Frudev write eeprom Parameter is NULL.", __FUNCTION__);
        return;
    }

    if (!is_support_extern_lable(fru_priv_property->fru_property.fru_id)) {
        return;
    }

    if (elabel_tmp->internal_extend.valid_flag) {
        write_extend_elabel(fru_priv_property->fru_property.frudev_handle, 0, &(elabel_tmp->internal_extend),
            extendsize);
    }
    if (elabel_tmp->chassis_extend.valid_flag) {
        write_extend_elabel(fru_priv_property->fru_property.frudev_handle, extendsize, &(elabel_tmp->chassis_extend),
            extendsize);
    }
    if (elabel_tmp->product_extend.valid_flag) {
        write_extend_elabel(fru_priv_property->fru_property.frudev_handle, (2 * extendsize),
            &(elabel_tmp->product_extend), extendsize);
    }
    if (elabel_tmp->multi_record) {
        _frudev_multi_write_eeprom(fru_priv_property->fru_property.frudev_handle, elabel_tmp->multi_record,
            sizeof(elabel_tmp->multi_record));
    }

    return;
}


LOCAL gboolean check_condition_to_sync_elabel(FRU_PRIV_PROPERTY_S *fru_priv_property)
{
    
    if (!fru_priv_property->elabel_task_state) {
        return FALSE;
    }

    guint8 frudev_type = fru_priv_property->fru_property.frudev_type;
    // 存储介质为MCU时，不执行任何写操作
    if ((frudev_type == STORAGE_TYPE_MCU) || (frudev_type == STORAGE_TYPE_MCU_FAN) ||
        (frudev_type == STORAGE_TYPE_E2P_V4) || (frudev_type == STORAGE_TYPE_E2P_DOUBLE) ||
        (frudev_type == STORAGE_TYPE_MCU_CUSTOM) || (frudev_type == STORAGE_TYPE_MCU_INTELLIGENT_FAN)) {
        return FALSE;
    }

    return TRUE;
}


LOCAL void __task_syn_elabel(void *argv1)
{
    OBJ_HANDLE obj_handle = (OBJ_HANDLE)argv1;
    FRU_PRIV_PROPERTY_S *fru_priv_property = NULL;
    if (get_obj_priv_data(obj_handle, (gpointer *)&fru_priv_property) != RET_OK) {
        return;
    }
    guint8 retry_times = 0;
    fru_priv_property->elabel_task_state = TRUE;

    while (fru_priv_property->elabel_task_state) {
        (void)vos_thread_sem4_p(fru_priv_property->elabel_syn_semid, SEM_WAIT_FOREVER);

        if (check_condition_to_sync_elabel(fru_priv_property) == FALSE) {
            continue;
        }

        guint8 frudev_type = fru_priv_property->fru_property.frudev_type;
        if ((fru_priv_property->eeprom_format == EEPROM_FORMAT_TIANCHI) || (frudev_type == STORAGE_TYPE_FILE)) {
            continue;
        }

        ELABEL_INFO_S *elabel_tmp = (ELABEL_INFO_S *)g_malloc0(sizeof(ELABEL_INFO_S));
        if (elabel_tmp == NULL) {
            continue;
        }
        (void)memcpy_s(elabel_tmp, sizeof(ELABEL_INFO_S), fru_priv_property->elabel_info, sizeof(ELABEL_INFO_S));

        split_extend_with_null(&elabel_tmp->internal_extend);

        
        split_extend_with_null(&elabel_tmp->extend);
        split_extend_with_null(&elabel_tmp->chassis_extend);
        split_extend_with_null(&elabel_tmp->product_extend);

        while (frudev_write_to_eeprom(fru_priv_property->fru_property.frudev_handle, ELABEL_INFO_BLOCK_OFFSET,
            elabel_tmp, ELABEL_INFO_AREA_SIZE) != RET_OK) {
            debug_log(DLOG_INFO, "%s update elabel info fail!.", __FUNCTION__);
            if (retry_times > 10) { 
                retry_times = 0;
                break;
            }
            vos_task_delay(TASK_DELAY_ONE_SECOND);
            retry_times++;
        }

        
        check_elabel_from_eeprom(fru_priv_property, elabel_tmp);

        if (fru_priv_property->fru_property.is_support_config_area != 0) {
            g_free(elabel_tmp);
            continue;
        }
        
        _frudev_write_eeprom(fru_priv_property, elabel_tmp, ELABEL_INFO_EXTEND_SIZE);

        g_free(elabel_tmp);
    }

    // 任务退出时将任务ID清0，避免内存清理的任务执行太快，导致非法地址访问
    fru_priv_property->elabel_task_id = 0;
    put_obj_priv_data(obj_handle, (gpointer)fru_priv_property);
}

LOCAL gint32 __create_update_fru_task(OBJ_HANDLE object_handle, FRU_PRIV_PROPERTY_S *fru_priv_property)
{
    
    CREAT_SYN_SEM(fru_priv_property->update_syn_semid, 5);

    
    gchar task_name[MAX_FRU_SYN_TASK_NAME] = {0};
    gint32 ret = snprintf_s(task_name, sizeof(task_name), sizeof(task_name) - 1, "%s%d", FRU_SYNC_TASK_NAME,
        fru_priv_property->fru_property.fru_id);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
    ret = vos_task_create(&(fru_priv_property->update_task_id), task_name, (TASK_ENTRY)__task_syn_fru,
        (void *)object_handle, DEFAULT_PRIORITY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s create fru_syn_task fail(ret = %d)", dfl_get_object_name(object_handle), ret);
        return ret;
    }

    
    
    
    if ((fru_priv_property->fru_info->board.area_ver != 0x01) ||
        (fru_priv_property->fru_info->product.area_ver != 0x01)) {
        fru_priv_property->fru_info->board.area_ver = 0x01;
        fru_priv_property->fru_info->product.area_ver = 0x01;
        (void)vos_thread_sem4_v(fru_priv_property->update_syn_semid); 
    }

    
    CREAT_SYN_SEM(fru_priv_property->elabel_syn_semid, 5);

    guint8 fru_type = 0;
    (void)dal_get_property_value_byte(object_handle, PROPERTY_FRU_TYPE, &fru_type);

    guchar soft_ware_type = MGMT_SOFTWARE_TYPE_UNKNOW;
    ret = dal_get_software_type(&soft_ware_type);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s]dal_get_software_type fail.", __FUNCTION__);
        return ret;
    }

    if (soft_ware_type == MGMT_SOFTWARE_TYPE_EM && fru_type == FRU_TYPE_BACKPLANE) {
        return RET_OK;
    }

    (void)snprintf_s(task_name, sizeof(task_name), sizeof(task_name) - 1, "elabel_syn_task%d",
        fru_priv_property->fru_property.fru_id);
    ret = vos_task_create(&(fru_priv_property->elabel_task_id), task_name, (TASK_ENTRY)__task_syn_elabel,
        (void *)object_handle, DEFAULT_PRIORITY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s create %s fail(ret = %d)", dfl_get_object_name(object_handle), task_name, ret);
        return ret;
    }

    return RET_OK;
}

LOCAL void customize_handle_system_info(FRU_PRIV_PROPERTY_S *fru_priv)
{
    
    gint is_customize = (fru_priv->eeprom_format == EEPROM_FORMAT_TIANCHI) ?
        fru_priv->system_info->tc_system_desc_st.is_customize :
        fru_priv->system_info->system_area_st.is_customize;
    if (is_customize != DEFAULT_PRODUCT_NAME && is_customize != NEW_PRODUCT_NAME) {
        return;
    }

    if (fru_priv->eeprom_format == EEPROM_FORMAT_TIANCHI) {
        (void)memset_s(fru_priv->system_info->tc_system_desc_st.sys_product_name, SYS_PRODUCT_NAME_MAX_LEN, 0,
            SYS_PRODUCT_NAME_MAX_LEN);
        (void)get_product_name(fru_priv->system_info->tc_system_desc_st.is_customize,
            fru_priv->fru_property.product_name, sizeof(fru_priv->fru_property.product_name));
        debug_log(DLOG_DEBUG, "product_name %s.", fru_priv->fru_property.product_name);
        (void)strncpy_s((gchar *)fru_priv->system_info->tc_system_desc_st.sys_product_name, SYS_PRODUCT_NAME_MAX_LEN,
            fru_priv->fru_property.product_name, FRU_MAX_ELABEL_LEN - 1);
    } else {
        (void)memset_s(fru_priv->system_info->system_area_st.sys_product_name.data, FRU_MAX_ELABEL_LEN, 0,
            FRU_MAX_ELABEL_LEN);
        (void)get_product_name(fru_priv->system_info->system_area_st.is_customize, fru_priv->fru_property.product_name,
            sizeof(fru_priv->fru_property.product_name));
        debug_log(DLOG_DEBUG, "product_name %s.", fru_priv->fru_property.product_name);
        (void)strncpy_s((gchar *)fru_priv->system_info->system_area_st.sys_product_name.data, FRU_MAX_ELABEL_LEN,
            fru_priv->fru_property.product_name, FRU_MAX_ELABEL_LEN - 1);
        fru_priv->system_info->system_area_st.sys_product_name.len =
            (guint8)strlen(fru_priv->fru_property.product_name);
    }
}


LOCAL void __task_syn_sys_area(void *argv1)
{
    (void)prctl(PR_SET_NAME, (gulong) "SynSysAreaTask");

    OBJ_HANDLE obj_handle = (OBJ_HANDLE)argv1;
    FRU_PRIV_PROPERTY_S *fru_priv_property = NULL;
    gint32 ret = get_obj_priv_data(obj_handle, (gpointer *)&fru_priv_property);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s get priv data failed.", dfl_get_object_name(obj_handle));
        return;
    }
    fru_priv_property->update_sys_task_state = TRUE;

    while (fru_priv_property->update_sys_task_state) {
        (void)vos_thread_sem4_p(fru_priv_property->update_sys_syn_semid, SEM_WAIT_FOREVER);

        
        if (!fru_priv_property->update_sys_task_state) {
            continue;
        }

        customize_handle_system_info(fru_priv_property);

        if (update_system_area(fru_priv_property) != RET_OK) {
            debug_log(DLOG_ERROR, "update system area elabel fail!.");
            vos_task_delay(TASK_DELAY_ONE_SECOND);
            continue;
        }
    }

    // 任务退出时将任务ID清0，避免内存清理的任务执行太快，导致非法地址访问
    fru_priv_property->update_sys_task_id = 0;
    put_obj_priv_data(obj_handle, (gpointer)fru_priv_property);
}


LOCAL gint32 __create_syn_frudata_task(OBJ_HANDLE object_handle, FRU_PRIV_PROPERTY_S *fru_priv_property)
{
    if (fru_priv_property == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return RET_ERR;
    }

    
    if (fru_priv_property->fru_property.is_sup_dft) {
        gint32 ret = __create_update_fru_task(object_handle, fru_priv_property);
        if (ret != RET_OK) {
            return ret;
        }
    }

    
    if (fru_priv_property->fru_property.is_cont_sys_area == 0) {
        return RET_OK;
    }

    
    CREAT_SYN_SEM(fru_priv_property->update_sys_syn_semid, 5);

    
    gchar task_name[MAX_FRU_SYN_TASK_NAME] = {0};
    (void)snprintf_s(task_name, sizeof(task_name), sizeof(task_name) - 1, SYS_FIELD_SYNC_TASK_NAME);
    gint32 ret = vos_task_create(&(fru_priv_property->update_sys_task_id), task_name, (TASK_ENTRY)__task_syn_sys_area,
        (void *)object_handle, DEFAULT_PRIORITY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "create fru_sys_syn_task fail(ret = %d)", ret);
        return ret;
    }

    return ret;
}


LOCAL void eep_set_status(OBJ_HANDLE frudev_handle, OBJ_HANDLE fru_handle, gint32 ret, guint8 *count,
    gboolean *is_state_ok)
{
    if (ret == DFL_OK) {
        if (*is_state_ok == FALSE) {
            debug_log(DLOG_INFO, "read eeprom(%s) success.", dfl_get_object_name(frudev_handle));
            *is_state_ok = TRUE;
        }
        (void)dal_set_property_value_byte(fru_handle, PROPERTY_FRU_EEP_STATUS, 0, DF_NONE);
        *count = 0;
        vos_task_delay(TASK_DELAY_ONE_SECOND * 3);
    } else {
        if (*is_state_ok == TRUE) {
            debug_log(DLOG_ERROR, "read eeprom(%s) fail: %d", dfl_get_object_name(frudev_handle), ret);
            *is_state_ok = FALSE;
        }

        if ((*count)++ >= 5) {
            (void)dal_set_property_value_byte(fru_handle, PROPERTY_FRU_EEP_STATUS, 1, DF_NONE);
            *count = 0;
        }
    }

    if (dal_is_cpld_validating() == TRUE) {
        
        vos_task_delay(TASK_DELAY_ONE_SECOND * 120);
    }
}

LOCAL void eep_scan_task(void *argv1)
{
    (void)prctl(PR_SET_NAME, (gulong) "EepScanTask");

    OBJ_HANDLE obj_handle = (OBJ_HANDLE)argv1;
    FRU_PRIV_PROPERTY_S *fru_priv = NULL;
    if (get_obj_priv_data(obj_handle, (gpointer *)&fru_priv) != RET_OK) {
        return;
    }
    OBJ_HANDLE frudev_handle = fru_priv->fru_property.frudev_handle;
    OBJ_HANDLE fru_handle = fru_priv->fru_property.fru_handle;
    gchar  fru_dev_class_name[MAX_NAME_SIZE] = {0};
    gint32 ret;
    guint8 count = 0;
    gboolean is_state_ok = FALSE;

    fru_priv->eep_scan_task_state = TRUE;
    (void)dfl_get_class_name(frudev_handle, fru_dev_class_name, MAX_NAME_SIZE);
    while (fru_priv->eep_scan_task_state) {
        
        vos_task_delay(TASK_DELAY_ONE_SECOND * 5);
        if (frudata_get_eeprom_scan_status() == SCAN_STOPPED) {
            continue;
        }
        frudata_change_eeprom_scan_status();

        guint8 value = 0;
        if (g_strcmp0(fru_dev_class_name, CLASS_SHARE_FRUDEV) == 0) {
            ret = lock_share_eeprom(frudev_handle);
            if (ret != RET_OK) {
                debug_log(DLOG_DEBUG, "lock_share_eeprom %s fail: %d", dfl_get_object_name(frudev_handle), ret);
                continue;
            }
            ret = dfl_block_read(frudev_handle, fru_priv->fru_property.eeprom_accessor_prop, 0, sizeof(guint8), &value);
            unlock_share_eeprom(frudev_handle);
        } else {
            ret = dfl_block_read(frudev_handle, fru_priv->fru_property.eeprom_accessor_prop, 0, sizeof(guint8), &value);
        }

        if (fru_priv->fru_property.frudev_type == STORAGE_TYPE_E2P_DOUBLE) {
            ret = dfl_block_read(frudev_handle, PROPERTY_FRUDEV_BACK_STORAGE_LOC, 0, sizeof(guint8), &value);
        }

        eep_set_status(frudev_handle, fru_handle, ret, &count, &is_state_ok);
        
        }

    // 任务退出时将任务ID清0，避免内存清理的任务执行太快，导致非法地址访问
    fru_priv->eep_scan_task_id = 0;
    put_obj_priv_data(obj_handle, (gpointer)fru_priv);
}


LOCAL void __create_eep_scan_task(OBJ_HANDLE object_handle, FRU_PRIV_PROPERTY_S *fru_priv)
{
    guint8 fru_type = 0;
    if (fru_priv == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return;
    }

    guint8 frudev_type = fru_priv->fru_property.frudev_type;
    (void)dal_get_property_value_byte(fru_priv->fru_property.fru_handle, PROPERTY_FRU_TYPE, &fru_type);

    if ((frudev_type == STORAGE_TYPE_MCU) || (frudev_type == STORAGE_TYPE_MCU_FAN) ||
        (frudev_type == STORAGE_TYPE_MCU_CUSTOM) || (frudev_type == STORAGE_TYPE_MCU_INTELLIGENT_FAN) ||
        (dal_check_if_vsmm_supported() && dal_get_vsmm_ms_status() == 0 && fru_type == FRU_TYPE_BACKPLANE)) {
        debug_log(DLOG_INFO, "%s: do not support eep scan!", __FUNCTION__);
        return;
    }

    gchar acc_name[MAX_NAME_SIZE] = {0};
    gint32 ret = dfl_get_property_accessor(fru_priv->fru_property.frudev_handle,
        fru_priv->fru_property.eeprom_accessor_prop, acc_name, MAX_NAME_SIZE);
    if (ret == DFL_OK) {
        debug_log(DLOG_INFO, "creat eep task (%s)", dfl_get_object_name(object_handle));
        (void)vos_task_create(&(fru_priv->eep_scan_task_id), FRU_EEP_SCAN_TASK_NAME, (TASK_ENTRY)eep_scan_task,
            (void *)object_handle, DEFAULT_PRIORITY);
    }

    return;
}

LOCAL gint32 __validattion_check_fru_info(FRU_PROPERTY_S *fru_prop, FRU_FILE_S *fru_file)
{
    if (fru_file == NULL || fru_prop == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return ERROR_CODE_PARAMETER_INVALID;
    }

    if (fru_prop->frudev_read_fru_pf == NULL) {
        debug_log(DLOG_ERROR, "parameter error = %d.", fru_prop->frudev_type);
        return ERROR_CODE_PARAMETER_INVALID;
    }
    return RET_OK;
}


LOCAL gint32 __read_fru_file(FRU_PROPERTY_S *fru_prop, FRU_FILE_S *fru_file)
{
    if (__validattion_check_fru_info(fru_prop, fru_file) != RET_OK) {
        return ERROR_CODE_PARAMETER_INVALID;
    }

    gint32 ret;
    fru_prop->frudev_crc_state = TRUE;
    for (gint32 retry_times = 5; retry_times > 0; retry_times--) {
        if (fru_prop->frudev_type == STORAGE_TYPE_MCU || fru_prop->frudev_type == STORAGE_TYPE_MCU_CUSTOM) {
            ret = fru_prop->frudev_read_fru_pf(fru_prop->fru_handle, fru_file);
        } else if (fru_prop->frudev_type == STORAGE_TYPE_EEPROM) {
            ret = read_fru_compatible_with_tianchi(fru_prop->frudev_handle, fru_prop->fru_offset, fru_file);
        } else {
            ret = fru_prop->frudev_read_fru_pf(fru_prop->frudev_handle, fru_file);
        }

        if (ret != RET_OK) {
            if (retry_times > 1) {
                debug_log(DLOG_ERROR, "Warning: frudev_read_fru_pf failed!");
                vos_task_delay(DELAY_ONE_SECOND);
                continue;
            }

            (void)dal_set_property_value_byte(fru_prop->frudev_handle, PROPERTY_FRUDEV_HEALTH, FRU_HEALTH_FAIL,
                DF_NONE);
            return ret;
        }

        guint8 eeprom_format = (fru_prop->fru_offset == 0) ? EEPROM_FORMAT_V2 : EEPROM_FORMAT_TIANCHI;
        ret = check_fru_file(eeprom_format, fru_file);
        if (ret == RET_OK) {
            return RET_OK;
        }

        if (ret == ERROR_CODE_HEADER_ERROR && fru_prop->frudev_type == STORAGE_TYPE_EEPROM) {
            try_fix_fru_header(fru_prop->frudev_handle);
        } else {
            debug_log(DLOG_ERROR, "Warning: %s fru file is destroyed!.", dfl_get_object_name(fru_prop->frudev_handle));
        }

        vos_task_delay(DELAY_ONE_SECOND);
    }

    
    if ((fru_prop->is_sup_dft == 0) || (ret == RET_OK)) {
        return RET_OK;
    }

    if (ret != ERROR_CODE_CRC_DIFFERENT) {
        create_fru_file(fru_prop, fru_file);
        return RET_OK;
    }

    
    fru_prop->frudev_crc_state = FALSE;
    (void)dal_set_property_value_byte(fru_prop->frudev_handle, PROPERTY_FRUDEV_HEALTH, FRU_HEALTH_FAIL, DF_NONE);

    return RET_OK;
}

LOCAL void __check_fru_file(FRU_PRIV_PROPERTY_S *fru_priv_property)
{
    gint32 ret = check_fru_file(fru_priv_property->eeprom_format, fru_priv_property->fru_file);
    if (ret == RET_OK) {
        return;
    }

    debug_log(DLOG_ERROR, "%s check_fru_file failed, ret = %d.", __FUNCTION__, ret);
    if (ret == ERROR_CODE_HEADER_ERROR && fru_priv_property->fru_property.frudev_type == STORAGE_TYPE_EEPROM) {
        try_fix_fru_header(fru_priv_property->fru_property.frudev_handle);
    }
}

LOCAL gint32 __init_fru_device(OBJ_HANDLE object_handle, FRU_PRIV_PROPERTY_S *fru_priv_property)
{
    gint32 ret = init_cafe_info(&(fru_priv_property->fru_property), fru_priv_property->cafe_info);
    if (ret == RET_OK) {
        debug_log(DLOG_DEBUG, "%s init cafe info successfully", __FUNCTION__);
    }

    ret = init_tc_header_and_area_offsets(fru_priv_property);
    if (ret != RET_OK) {
        return ret;
    }

    ret = __init_fru_file(fru_priv_property);
    if (ret == RET_OK) {
        debug_log(DLOG_INFO, "%s __init_fru_file successfully", __FUNCTION__);
    }

    
    __create_eep_scan_task(fru_priv_property->fru_property.fru_handle, fru_priv_property);

    if (fru_priv_property->fru_property.elabel_handle == 0) {
        debug_log(DLOG_INFO, "%s: elabel_handle is not available.", dfl_get_object_name(object_handle));
        return RET_OK;
    }

    
    ret = __read_fru_file(&(fru_priv_property->fru_property), fru_priv_property->fru_file);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "call __read_fru_file failed, error = %d!(obj:%s).", ret,
            dfl_get_object_name(object_handle));
        return ret;
    }

    __check_fru_file(fru_priv_property);

    
    (void)create_fru_data(fru_priv_property->fru_info, (fru_priv_property->eeprom_format == EEPROM_FORMAT_TIANCHI) ?
        fru_priv_property->fru_file->header.frudata :
        fru_priv_property->fru_file->frudata);
    (void)handle_extra_elabel_info(fru_priv_property);

    
    (void)set_elabel_property(&(fru_priv_property->fru_property), fru_priv_property->fru_info);

    
    ret = __create_syn_frudata_task(object_handle, fru_priv_property);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "call __create_syn_frudata_task failed, error = %d!(obj:%s).", ret,
            dfl_get_object_name(object_handle));
        return ret;
    }

    
    ret = init_system_info(fru_priv_property);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "call init_system_info failed, error = %d!(obj:%s).", ret,
            dfl_get_object_name(object_handle));
        return ret;
    }

    
    ret = init_elabel_info(fru_priv_property);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "call init_elabel_info failed, error = %d!(obj:%s).", ret,
            dfl_get_object_name(object_handle));
    }

    return ret;
}

LOCAL gint32 __init_fru_info(OBJ_HANDLE object_handle, FRU_PRIV_PROPERTY_S *fru_priv_property)
{
    guchar soft_ware_type = MGMT_SOFTWARE_TYPE_UNKNOW;
    (void)dal_get_software_type(&soft_ware_type);

    if ((soft_ware_type != MGMT_SOFTWARE_TYPE_EM && !dal_match_product_id(PRODUCT_ID_PANGEA_V6)) ||
        (__check_init_in_standby_smm(object_handle) == RET_OK) || check_is_allow_access(object_handle) == TRUE) {
        gint32 ret = __init_fru_device(object_handle, fru_priv_property);
        if (ret != RET_OK) {
            __clear_fru_init_data(object_handle, fru_priv_property);
            return ret;
        }
        if (fru_priv_property->fru_property.elabel_handle == 0) {
            debug_log(DLOG_INFO, "%s: elabel_handle is not available.", dfl_get_object_name(object_handle));
            return RET_OK;
        }
    }

    
    if ((fru_priv_property->fru_property.fru_id == 0) && (strcmp((const gchar *)fru_priv_property->fru_property.fru_ver,
        (const gchar *)fru_priv_property->elabel_info->board.file_id.data) != 0)) {
        update_file_id(fru_priv_property);
    }

    
    if (fru_priv_property->fru_property.fru_type == MEZZ_CARD) {
        gint32 ret = renew_mezz_mac(fru_priv_property);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "call renew_mezz_mac failed, error = %d!(obj:%s).", ret,
                dfl_get_object_name(object_handle));
        }
    }

    return RET_OK;
}


LOCAL void __fru_check_and_fix_eeprom_fru_head(OBJ_HANDLE object_handle)
{
    OBJ_HANDLE dev_handle = 0;
    gint32 ret = dfl_get_referenced_object(object_handle, PROPERTY_FRUDEV_RO, &dev_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "dfl_get_referenced_object(%s) fail, obj_name=%s, ret=%d", PROPERTY_FRUDEV_RO,
            dfl_get_object_name(object_handle), ret);
        return;
    }

    guint8 storage_type = 0;
    ret = dal_get_property_value_byte(dev_handle, PROPERTY_FRUDEV_STORAGE_TYPE, &storage_type);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Failed to get %s StorageType, ret:%d.", dfl_get_object_name(dev_handle), ret);
        return;
    }

    // StorageType为EEPROM类型才处理
    if (storage_type != STORAGE_TYPE_EEPROM) {
        return;
    }
    // 读取的电子标签头4个字符值
    guint32 fru_header_tag = 0;
    ret = frudev_read_from_eeprom(dev_handle, 0, &fru_header_tag, sizeof(fru_header_tag));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "read devname=%s head from eeprom fail. ret = %d", dfl_get_object_name(dev_handle), ret);
        return;
    }

    if (fru_header_tag == FRU_HEADER_TAG) {
        return;
    }

    guint32 tc_header_tag = 0;
    ret = frudev_read_from_eeprom(dev_handle, TC_STANDARD_CODE_OFFSET, &tc_header_tag, sizeof(tc_header_tag));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "read devname=%s head from eeprom fail. ret = %d", dfl_get_object_name(dev_handle), ret);
        return;
    }

    if (tc_header_tag == TC_STANDARD_CODE) {
        return;
    }

    debug_log(DLOG_ERROR, "devname=%s, read head tag from eeprom tag=0x%x", dfl_get_object_name(dev_handle),
        fru_header_tag);

    // 在0x00处回写 FRU
    ret = frudev_write_to_eeprom(dev_handle, 0, FRU_HEADER_TAG_DEFAULT, sizeof(FRU_HEADER_TAG_DEFAULT));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "write devname=%s head to eeprom fail. ret = %d", dfl_get_object_name(dev_handle), ret);
    }

    return;
}


LOCAL void add_fru_to_list(OBJ_HANDLE fru_obj)
{
    (void)vos_thread_sem4_p(g_fru_handle_list_lock, SEM_WAIT_FOREVER);
    g_fru_in_initing = g_slist_prepend(g_fru_in_initing, (gpointer)(uintptr_t)fru_obj);
    (void)vos_thread_sem4_v(g_fru_handle_list_lock);
}


LOCAL gboolean is_fru_in_list(OBJ_HANDLE fru_obj)
{
    GSList *node = NULL;

    (void)vos_thread_sem4_p(g_fru_handle_list_lock, SEM_WAIT_FOREVER);
    node = g_slist_find(g_fru_in_initing, (gpointer)(uintptr_t)fru_obj);
    (void)vos_thread_sem4_v(g_fru_handle_list_lock);

    return (node == NULL ? FALSE : TRUE);
}


LOCAL void remove_fru_from_list(OBJ_HANDLE fru_obj)
{
    (void)vos_thread_sem4_p(g_fru_handle_list_lock, SEM_WAIT_FOREVER);
    g_fru_in_initing = g_slist_remove(g_fru_in_initing, (gpointer)(uintptr_t)fru_obj);
    (void)vos_thread_sem4_v(g_fru_handle_list_lock);
}

LOCAL gint32 __fru_init_task(OBJ_HANDLE object_handle)
{
    (void)prctl(PR_SET_NAME, (gulong) "FruInitTask");

    if (is_fru_in_list(object_handle) == TRUE) {
        return RET_OK;
    }
    add_fru_to_list(object_handle);

    __fru_check_and_fix_eeprom_fru_head(object_handle);

    do {
        FRU_PRIV_PROPERTY_S *fru_priv_property = NULL;
        gint32 ret = __create_fru_priv_prop(object_handle, &fru_priv_property);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s create fru priv_prop failed, ret is %d", __FUNCTION__, ret);
            break;
        }

        
        ret = __init_fru_priv_prop(object_handle, &(fru_priv_property->fru_property), fru_priv_property->system_info);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s call __init_fru_priv_prop failed, error = %d!(obj_name:%s).", __FUNCTION__, ret,
                dfl_get_object_name(object_handle));
            __clear_fru_init_data(object_handle, fru_priv_property);
            break;
        }

        fru_priv_property->eeprom_format =
            (fru_priv_property->fru_property.fru_offset == 0) ? EEPROM_FORMAT_V2 : EEPROM_FORMAT_TIANCHI;

        
        ret = dfl_bind_object_alias(object_handle, fru_priv_property->fru_property.fru_id);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s call dfl_bind_object_alias failed, error = %d!(obj:%s).", __FUNCTION__, ret,
                dfl_get_object_name(object_handle));
            __clear_fru_init_data(object_handle, fru_priv_property);
            break;
        }

        if (__init_fru_info(object_handle, fru_priv_property) != RET_OK) {
            break;
        }

        fru_priv_property->initialized = TRUE;

        (void)fru_elabel_product_combine(object_handle, NULL);

        if (!fru_priv_property->fru_first_init_flag) {
            put_obj_priv_data(object_handle, (gpointer)fru_priv_property);
        } else {
            fru_priv_property->fru_first_init_flag = FALSE;
        }
    } while (0);

    remove_fru_from_list(object_handle);

    return RET_OK;
}


LOCAL gint32 __update_fru_in_standby_smm_foreach(OBJ_HANDLE handle, gpointer data)
{
    guint8 fru_type = 0;
    (void)dal_get_property_value_byte(handle, PROPERTY_FRU_TYPE, &fru_type);
    if (fru_type != FRU_TYPE_POWER && fru_type != FRU_TYPE_BACKPLANE && fru_type != FRU_TYPE_FANTRAY &&
        fru_type != FRU_TYPE_EPM && fru_type != FRU_TYPE_FAN_BOARD && fru_type != FRU_TYPE_EXP) {
        return RET_OK;
    }
    if (dal_check_if_vsmm_supported() && fru_type == FRU_TYPE_BACKPLANE) {
        return RET_OK;
    }
    guint32 b_load = POINTER_TO_UINT32(data);
    if (b_load) {
        
        (void)frudata_add_object_callback(CLASS_FRU, handle);
        return RET_OK;
    }

    
    FRU_PRIV_PROPERTY_S *fru_priv_property = NULL;
    gint32 ret = get_obj_priv_data(handle, (gpointer *)&fru_priv_property);
    if (ret != DFL_OK || fru_priv_property == NULL) {
        debug_log(DLOG_ERROR, "%s Get fru %s binded data failed, err code %d", __func__, dfl_get_object_name(handle),
            ret);
        return RET_OK;
    }

    __delete_syn_frudata_task(&fru_priv_property, TRUE);

    
    if (fru_priv_property->fru_info == NULL) {
        put_obj_priv_data(handle, (gpointer)fru_priv_property);
        return RET_OK;
    }

    if (fru_priv_property->fru_info->chassis.custom_info != NULL) {
        g_list_free_full(fru_priv_property->fru_info->chassis.custom_info, (GDestroyNotify)g_free);
        fru_priv_property->fru_info->chassis.custom_info = NULL;
    }

    if (fru_priv_property->fru_info->board.extension_label != NULL) {
        g_list_free_full(fru_priv_property->fru_info->board.extension_label, (GDestroyNotify)g_free);
        fru_priv_property->fru_info->board.extension_label = NULL;
    }

    if (fru_priv_property->fru_info->product.custom_info != NULL) {
        g_list_free_full(fru_priv_property->fru_info->product.custom_info, (GDestroyNotify)g_free);
        fru_priv_property->fru_info->product.custom_info = NULL;
    }
    put_obj_priv_data(handle, (gpointer)fru_priv_property);

    return RET_OK;
}


LOCAL gint32 __check_write_eep_status_hook(OBJ_HANDLE object_handle, gpointer user_data)
{
    FRU_PRIV_PROPERTY_S *fru_priv_property = NULL;
    gint32 ret = get_obj_priv_data(object_handle, (gpointer *)(&fru_priv_property));
    if ((ret != DFL_OK) || (fru_priv_property == NULL)) {
        return RET_OK;
    }

    if (fru_priv_property->update_status == SYN_UPDATING) {
        ret = RET_ERR;
    }

    put_obj_priv_data(object_handle, (gpointer)fru_priv_property);
    return ret;
}


LOCAL gint32 __notify_fdm_component_plugin(const gchar *class_name, OBJ_HANDLE object_handle)
{
    OBJ_HANDLE fdm_obj_handle = 0;
    gint32 ret = dfl_get_object_handle(CLASS_FDM_NAME, &fdm_obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s get obj %s failed", __FUNCTION__, CLASS_FDM_NAME);
        return RET_ERR;
    }

    guint8 group_id = 0;
    (void)dal_get_property_value_byte(object_handle, PROPERTY_COMPONENT_GROUPID, &group_id);

    guint8 component_type = 0;
    (void)dal_get_property_value_byte(object_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &component_type);

    guint8 slot_id = 0;
    (void)dal_get_property_value_byte(object_handle, PROPERTY_COMPONENT_DEVICENUM, &slot_id);

    GSList *input_list = NULL;
    input_list = g_slist_append(input_list, g_variant_new_string(class_name));
    input_list = g_slist_append(input_list, g_variant_new_byte(0));
    input_list = g_slist_append(input_list, g_variant_new_byte(group_id));
    input_list = g_slist_append(input_list, g_variant_new_byte(component_type));
    input_list = g_slist_append(input_list, g_variant_new_byte(slot_id));

    ret = dfl_call_class_method(fdm_obj_handle, METHOD_FDM_FRU_HOG_PLUG, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s call class %s method %s failed", __FUNCTION__, class_name, METHOD_FDM_FRU_HOG_PLUG);
        return ret;
    }

    return RET_OK;
}


LOCAL void __fru_position_list_update(guint32 position, guint32 operate)
{
    g_mutex_lock(&g_fru_position_change_flag_mutex);

    guint32 list_position = 0;
    gint32 exist_flag = FALSE;
    for (GSList *fru_posi_node = g_fru_position_list; fru_posi_node; fru_posi_node = fru_posi_node->next) {
        list_position = POINTER_TO_UINT32(fru_posi_node->data);
        if (list_position == position) {
            debug_log(DLOG_INFO, "%s:Fru position(%d) exist in g_fru_position_list.", __FUNCTION__, position);
            exist_flag = TRUE;
            break;
        }
    }

    if (exist_flag == FALSE && operate == FRU_POSITION_LIST_ADD) { // 如果拔插的设备已经存在于链表，不再添加
        g_fru_position_list = g_slist_append(g_fru_position_list, UINT32_TO_POINTER(position));
        g_mutex_unlock(&g_fru_position_change_flag_mutex);
        if (g_fru_position_list == NULL) {
            debug_log(DLOG_ERROR, "%s:Fru hot-plugging, position(%d) append g_fru_position_list failed.", __FUNCTION__,
                position);
        }
    } else if (exist_flag == TRUE &&
        operate == FRU_POSITION_LIST_DEL) { // Fru设备加载/移除完成，这时候再调方法更新sdr仓库
        g_fru_position_list = g_slist_remove(g_fru_position_list, UINT32_TO_POINTER(list_position));
        g_mutex_unlock(&g_fru_position_change_flag_mutex);
        if (g_fru_position_list == NULL) {
            debug_log(DLOG_ERROR, "%s:Fru hot-plugging, position(%d) remove from g_fru_position_list failed.",
                __FUNCTION__, position);
        }

        // 触发sdrdev仓库刷新Fru设备
        OBJ_HANDLE sdrdev_handle = 0;
        gint32 ret = dal_get_object_handle_nth(CLASS_SDR_DEV, 0, &sdrdev_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:get CLASS_SDR_DEV sdrdev_handle failed, ret(%d).", __FUNCTION__, ret);
            return;
        }
        ret = dfl_call_class_method(sdrdev_handle, METHOD_SDR_SET_SDR_VER_CHANGE_FLAG, NULL, NULL, NULL);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s:Fru hot-plugging, METHOD_SDR_SET_SDR_VER_CHANGE_FLAG failed, ret(%d).",
                __FUNCTION__, ret);
        }
    } else {
        g_mutex_unlock(&g_fru_position_change_flag_mutex);
        debug_log(DLOG_INFO, "%s:Fru position(%d) is exist(%d) in list, operation(%d) is not allowed.", __FUNCTION__,
            position, exist_flag, operate);
    }
}


LOCAL void __parse_fru_multi_record(GList **multi_record_area, const guint8 *buf, const FRU_INFO_HEADER_S *header,
    gsize buf_size)
{
    if (multi_record_area == NULL || buf == NULL || header == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return;
    }

    guint16 offset = 0;
    GList *tmp = NULL;
    const FRU_MULTI_HEADER_S *multi_header = NULL;

    do {
        if (offset + sizeof(FRU_MULTI_HEADER_S) > buf_size) {
            debug_log(DLOG_ERROR, "%s: read out of bounds, %u>%" G_GSIZE_FORMAT, __FUNCTION__,
                offset + (guint16)sizeof(FRU_MULTI_HEADER_S), buf_size);
            g_list_free_full(tmp, g_free);
            return;
        }

        multi_header = (const FRU_MULTI_HEADER_S *)(buf + offset);
        guint8 single_record_len = (guint8)(multi_header->len + sizeof(FRU_MULTI_HEADER_S)); 
        if (offset + single_record_len > buf_size) {
            debug_log(DLOG_ERROR, "%s: read out of bounds, %u>%" G_GSIZE_FORMAT, __FUNCTION__,
                offset + single_record_len, buf_size);
            g_list_free_full(tmp, g_free);
            return;
        }

        FRU_MULTI_AREA_S *single_record = (FRU_MULTI_AREA_S *)g_malloc0(sizeof(FRU_MULTI_AREA_S));
        if (single_record == NULL) {
            g_list_free_full(tmp, g_free);
            return;
        }

        single_record_len = multi_header->len + (guint8)sizeof(FRU_MULTI_HEADER_S); 
        errno_t safe_fun_ret = memcpy_s(single_record, sizeof(FRU_MULTI_AREA_S), buf + offset, single_record_len);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        }

        tmp = g_list_append(tmp, single_record);

        offset += single_record_len;
    } while (((multi_header->format & 0x80) == 0) &&
        (offset + header->offset.multi * MULITPLE_PARA < FRU_AREA_MAX_LEN));

    *multi_record_area = tmp;

    return;
}

#ifdef DFT_ENABLED

LOCAL gint32 __frudata_dft_item_register(OBJ_HANDLE obj_hnd, gpointer data)
{
    guint32 slot_id = 0;
    (void)dal_get_property_value_uint32(obj_hnd, SLOT_ID, &slot_id);

    guint32 position = *((guint32 *)data);
    if (position != slot_id) {
        return RET_OK;
    }

    gint32 ret = eeprom_self_test_dft_hook(obj_hnd, NULL);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "eeprom register dftitem handle %s result=%d", dfl_get_object_name(obj_hnd), ret);
    }

    return RET_OK;
}
#endif

void init_prepare_fru(void)
{
    init_storage_persis_prop();
    // 信号量初始化
    (void)vos_thread_mutex_sem4_create(&g_fru_id_flag, (gchar *)"fru_id_flag");

    (void)vos_thread_mutex_sem4_create(&g_fru_handle_list_lock, (gchar *)"fru_handle_list_lock");

    g_mutex_init(&g_fru_position_change_flag_mutex);
}


gint32 init_fru(OBJ_HANDLE object_handle, gpointer user_data)
{
    gulong task_id;
    (void)vos_task_create(&task_id, "FruInit", (TASK_ENTRY)__fru_init_task, (void *)object_handle, DEFAULT_PRIORITY);

    return RET_OK;
}

void enable_fru_init(void)
{
    g_fru_callback = TRUE;
}

void disable_fru_init(void)
{
    g_fru_callback = FALSE;
}


void update_fru_in_standby_smm(gpointer data)
{
    (void)prctl(PR_SET_NAME, (gulong) "SMMFruUpdate");

    gint32 ret = dfl_foreach_object(CLASS_FRU, __update_fru_in_standby_smm_foreach, data, NULL);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s] Update fru in stand by smm failed", __func__);
    }
}

gboolean is_fru_initialized(OBJ_HANDLE object_handle)
{
    FRU_PRIV_PROPERTY_S *fru_priv_property = NULL;
    OBJ_HANDLE dev_handle = 0;

    
    gint32 ret = dfl_get_referenced_object(object_handle, PROPERTY_FRUDEV_RO, &dev_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s.%s fru device not exist(%d), skip it", dfl_get_object_name(object_handle),
            PROPERTY_FRUDEV_RO, ret);
        return TRUE;
    }

    ret = get_obj_priv_data(object_handle, (gpointer *)&fru_priv_property);
    if (ret != RET_OK) {
        
        debug_log(DLOG_DEBUG, "dfl_get_binded_data %s failed", dfl_get_object_name(object_handle));
        return FALSE;
    }

    gboolean is_initialized;
    if (fru_priv_property->initialized) {
        debug_log(DLOG_DEBUG, "%s has initialized", dfl_get_object_name(object_handle));
        is_initialized = TRUE;
    } else {
        is_initialized = FALSE;
    }
    put_obj_priv_data(object_handle, (gpointer)fru_priv_property);

    return is_initialized;
}

gint32 on_fru_obj_add_event(OBJ_HANDLE object_handle)
{
    (void)prctl(PR_SET_NAME, (gulong) "FruAddInit");

    enable_fru_init();

    (void)init_fru(object_handle, NULL);

    guint8 retry_times = 0;
    for (;;) {
        vos_task_delay(1000);

        
        if (is_fru_initialized(object_handle)) {
            (void)fru_elabel_manufacture_syn_hook(object_handle, NULL);
            return RET_OK;
        }

        if (retry_times++ >= 30) {
            debug_log(DLOG_ERROR, "check fru(%s) init timeout", dfl_get_object_name(object_handle));
            return RET_ERR;
        }
    }
}


gint32 frudata_add_object_callback(const gchar *class_name, OBJ_HANDLE object_handle)
{
    if (strcmp(class_name, CLASS_FRU) != 0 && strcmp(class_name, CLASS_COMPONENT) != 0) {
        debug_log(DLOG_INFO, "class name = %s.", class_name);
        return ERROR_CODE_PARAMETER_INVALID;
    }

    if (strcmp(class_name, CLASS_COMPONENT) == 0) {
        debug_log(DLOG_INFO, "CLASS_COMPONENT HOT(%s) ADD. ", dfl_get_object_name(object_handle));
        (void)__notify_fdm_component_plugin(class_name, object_handle);
        return send_component_obj_add_msg_to_queue(object_handle, 1);
    }

    debug_log(DLOG_INFO, "FRU HOT(%s) ADD. ", dfl_get_object_name(object_handle));
    gulong task_id;
    (void)vos_task_create(&task_id, "FAinit", (TASK_ENTRY)on_fru_obj_add_event, (void *)(uintptr_t)object_handle,
        DEFAULT_PRIORITY);
    debug_log(DLOG_INFO, "FRU HOT(%s) ADD END. ", dfl_get_object_name(object_handle));

    guint32 position = dfl_get_position(object_handle);
    __fru_position_list_update(position, FRU_POSITION_LIST_ADD);

    gint32 ret = create_cpld_scan_task(object_handle, 0);
    debug_log(DLOG_INFO, "call create_cpld_scan_task ret %d, obj name = [%s]", ret, dfl_get_object_name(object_handle));

    return RET_OK;
}


gint32 frudata_del_object_callback(const gchar *class_name, OBJ_HANDLE object_handle)
{
    if (strcmp(class_name, CLASS_FRU) != RET_OK) {
        debug_log(DLOG_INFO, "class name = %s.", class_name);
        return ERROR_CODE_PARAMETER_INVALID;
    }

    guint32 position = dfl_get_position(object_handle);
    __fru_position_list_update(position, FRU_POSITION_LIST_ADD);
    __free_fru_id(object_handle);

    return RET_OK;
}


gint32 frudata_add_object_complete_callback(guint32 position)
{
#ifdef DFT_ENABLED
    gint32 ret = dfl_foreach_object(DFT_ITEM_CLASS, __frudata_dft_item_register, &position, NULL);
    if ((ret != DFL_OK) && (ret != ERRCODE_OBJECT_NOT_EXIST)) {
        debug_log(DLOG_ERROR, "dfl_foreach_object to register DFT item failed: ret %d.", ret);
    }
#endif

    __fru_position_list_update(position, FRU_POSITION_LIST_DEL);

    return RET_OK;
}


gint32 frudata_del_object_complete_callback(guint32 position)
{
    __fru_position_list_update(position, FRU_POSITION_LIST_DEL);

    return RET_OK;
}


void check_eeprom_write_status(void)
{
    gint32 retry_times = 0;

    for (;;) {
        if (dfl_foreach_object(CLASS_FRU, __check_write_eep_status_hook, NULL, NULL) == DFL_OK) {
            break;
        }

        if (retry_times++ >= 10) {
            break;
        }

        vos_task_delay(1000);
    }

    return;
}


gint32 create_fru_data(FRU_INFO_S *fru_info, const guint8 *fru_buf)
{
    if (fru_info == NULL || fru_buf == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return ERROR_CODE_PARAMETER_INVALID;
    }

    errno_t safe_fun_ret = memmove_s(&fru_info->header, sizeof(FRU_INFO_HEADER_S), fru_buf, sizeof(FRU_INFO_HEADER_S));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }

    if (fru_info->header.offset.chassis > 0) {
        fru_info->has_chassis = TRUE;
        elabel_chassis_area_paser(&(fru_info->chassis), fru_buf + fru_info->header.offset.chassis * MULITPLE_PARA,
            FRU_AREA_MAX_LEN - fru_info->header.offset.chassis * MULITPLE_PARA);
    }

    if (fru_info->header.offset.board > 0) {
        fru_info->has_board = TRUE;
        elabel_board_area_paser(&(fru_info->board), fru_buf + fru_info->header.offset.board * MULITPLE_PARA,
            FRU_AREA_MAX_LEN - fru_info->header.offset.board * MULITPLE_PARA);
    }

    if (fru_info->header.offset.product > 0) {
        fru_info->has_product = TRUE;
        elabel_product_area_paser(&(fru_info->product), fru_buf + fru_info->header.offset.product * MULITPLE_PARA,
            FRU_AREA_MAX_LEN - fru_info->header.offset.product * MULITPLE_PARA);
    }

    if (fru_info->header.offset.multi > 0) {
        fru_info->has_multi = TRUE;
        __parse_fru_multi_record(&(fru_info->multi_record), fru_buf + fru_info->header.offset.multi * MULITPLE_PARA,
            &(fru_info->header), FRU_AREA_MAX_LEN - fru_info->header.offset.multi * MULITPLE_PARA);
    }

    if (fru_info->header.offset.internal > 0) {
        fru_info->has_internal = TRUE;
        elabel_internal_area_paser(&(fru_info->internal), fru_buf + fru_info->header.offset.internal * MULITPLE_PARA,
            &(fru_info->header), FRU_AREA_MAX_LEN - fru_info->header.offset.internal * MULITPLE_PARA);
    }

    return RET_OK;
}


gboolean is_support_fru_id(guint8 fruid)
{
    OBJ_HANDLE obj_handle = 0;
    guint32 support_max_fru = 0;
    gint32 ret = dfl_get_object_count(CONNECTOR_CLASS_NAME, &support_max_fru);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "call dfl_get_object_count failed(objname = %s), error = %d.", CONNECTOR_CLASS_NAME, ret);
        support_max_fru = 1; 
    }
    if (fruid > support_max_fru) {
        // 如果大于连接器数目, 则尝试通过dal_get_specific_object获取
        ret = dal_get_specific_object_byte(CLASS_FRU, PROPERTY_FRU_ID, fruid, &obj_handle);
        if (ret == RET_OK) {
            return TRUE;
        }
        return FALSE;
    }

    return TRUE;
}


gboolean get_fru_callback_flag(void)
{
    return g_fru_callback;
}


void set_fru_callback_flag(gboolean flag)
{
    g_fru_callback = flag;
}

LOCAL gint32 init_Kunpeng_K_mechine_type(void)
{
    gint32 ret = 0;
    PER_S per_reg;

    per_reg.key = KUNPENG_K_TYPE;
    per_reg.mode = PER_MODE_POWER_OFF;
    per_reg.data = &g_Kunpeng_K_mechine_type;
    per_reg.len = sizeof(g_Kunpeng_K_mechine_type);

    ret = per_init(&per_reg, 1);
    if (ret != PER_OK) {
        debug_log(DLOG_ERROR, "Persistance init %s info failed, ret = %d", per_reg.key, ret);
        return RET_ERR;
    }

    ret = per_read(&g_Kunpeng_K_mechine_type, &g_Kunpeng_K_mechine_type,
        sizeof(g_Kunpeng_K_mechine_type), sizeof(g_Kunpeng_K_mechine_type));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "g_Kunpeng_K_mechine_type per_read failed, ret = %d", ret);
        return RET_ERR;
    }

    debug_log(DLOG_INFO, "g_Kunpeng_K_mechine_type = %d", g_Kunpeng_K_mechine_type);
    return RET_OK;
}