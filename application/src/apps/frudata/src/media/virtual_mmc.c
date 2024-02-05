

#include <math.h>
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "pme_app/dalib/dal_obj_priv_data.h"
#include "media/virtual_mmc.h"
#include "protocol.h"
#include "elabel/elabel.h"
#include "media/port_rate.h"
#include "media/custom_mcu.h"
#include "media/e2p_large.h"

#define MB_ELABEL_ITEM_BOARD_PRODUCT_NAME_INDEX 0x22
#define ELABEL_FUNC_CODE 0x42
#define ELABEL_READ_SUB_FUNC_CODE 0x05
#define ELABEL_WRITE_SUB_FUNC_CODE 0x06
#define ELABEL_UPDATE_SUB_FUNC_CODE 0x07
#define ELABEL_MODBUS_CLEAR_OPERATE 0x00
#define ELABEL_MODBUS_UPDATE_OPERATE 0x01
#define ELABEL_AREA_CHASSIS_INDEX 1
#define FRU_DATA_MAX_LEN 68

typedef gint32 (*INIT_FRU_PRODUCT_DATA)(const guint8 *item_desp, guint32 item_desp_len,
    FRU_PRODUCT_AREA_S *fru_product_area_info);
typedef gint32 (*INIT_FRU_BOARD_DATA)(const guint8 *item_desp, guint32 item_desp_len,
    FRU_BOARD_AREA_S *fru_board_area_info);

typedef struct tag_fru_product_data {
    guint8 field_index;
    INIT_FRU_PRODUCT_DATA __init_fru_product_data;
} FRU_PRODUCT_DATA;

typedef struct tag_fru_board_data {
    guint8 field_index;
    INIT_FRU_BOARD_DATA __init_fru_board_data;
} FRU_BOARD_DATA;

typedef enum {
    MB_ELABEL_ITEM_READ_OK,    
    MB_ELABEL_ITEM_EMPTY,      
    MB_ELABEL_ITEM_READ_FAILED 
} MB_ELABEL_ITEM_OP_STATUS;

LOCAL gint32 __sync_elabel_to_mcu(OBJ_HANDLE object_handle, guint8 update_type);
LOCAL gint32 __write_elabel_to_mcu(OBJ_HANDLE object_handle, guint8 update_type,
    FRU_PRIV_PROPERTY_S *fru_priv_property);
LOCAL gint32 __get_chassis_item_data(ELABEL_CHASSIS_AREA_S *elabel_chassis_area, guint8 field_index, guint8 *buf,
    guint8 buf_len, guint8 *write_buf_len);
LOCAL gint32 __get_board_item_data(ELABEL_BOARD_AREA_S *elabel_board_area, guint8 field_index, guint8 *buf,
    guint8 buf_len, guint8 *write_buf_len);
LOCAL gint32 __get_product_item_data(ELABEL_PRODUCT_AREA_S *elabel_product_area, guint8 field_index, guint8 *buf,
    guint8 buf_len, guint8 *write_buf_len);
LOCAL gint32 __init_fru_product_manufacturer(const guint8 *item_desp, guint32 item_desp_len,
    FRU_PRODUCT_AREA_S *fru_product_area_info);
LOCAL gint32 __init_fru_product_name(const guint8 *item_desp, guint32 item_desp_len,
    FRU_PRODUCT_AREA_S *fru_product_area_info);
LOCAL gint32 __init_fru_product_part_number(const guint8 *item_desp, guint32 item_desp_len,
    FRU_PRODUCT_AREA_S *fru_product_area_info);
LOCAL gint32 __init_fru_product_version(const guint8 *item_desp, guint32 item_desp_len,
    FRU_PRODUCT_AREA_S *fru_product_area_info);
LOCAL gint32 __init_fru_product_serial_number(const guint8 *item_desp, guint32 item_desp_len,
    FRU_PRODUCT_AREA_S *fru_product_area_info);
LOCAL gint32 __init_fru_product_assettag(const guint8 *item_desp, guint32 item_desp_len,
    FRU_PRODUCT_AREA_S *fru_product_area_info);
LOCAL gint32 __init_fru_product_file_id(const guint8 *item_desp, guint32 item_desp_len,
    FRU_PRODUCT_AREA_S *fru_product_area_info);
LOCAL gint32 __init_fru_board_mfgdata(const guint8 *item_desp, guint32 item_desp_len,
    FRU_BOARD_AREA_S *fru_board_area_info);
LOCAL gint32 __init_fru_board_manufacture(const guint8 *item_desp, guint32 item_desp_len,
    FRU_BOARD_AREA_S *fru_board_area_info);
LOCAL gint32 __init_fru_board_product_name(const guint8 *item_desp, guint32 item_desp_len,
    FRU_BOARD_AREA_S *fru_board_area_info);
LOCAL gint32 __init_fru_board_serial_number(const guint8 *item_desp, guint32 item_desp_len,
    FRU_BOARD_AREA_S *fru_board_area_info);
LOCAL gint32 __init_fru_board_part_number(const guint8 *item_desp, guint32 item_desp_len,
    FRU_BOARD_AREA_S *fru_board_area_info);
LOCAL gint32 __init_fru_board_file_id(const guint8 *item_desp, guint32 item_desp_len,
    FRU_BOARD_AREA_S *fru_board_area_info);
LOCAL gint32 __init_fru_board_extra_description(const guint8 *item_desp, guint32 item_desp_len,
    FRU_BOARD_AREA_S *fru_board_area_info);
LOCAL gint32 __init_fru_board_extra_issuenumber(const guint8 *item_desp, guint32 item_desp_len,
    FRU_BOARD_AREA_S *fru_board_area_info);
LOCAL gint32 __init_fru_board_extra_CLEIcode(const guint8 *item_desp, guint32 item_desp_len,
    FRU_BOARD_AREA_S *fru_board_area_info);
LOCAL gint32 __init_fru_board_extra_bom(const guint8 *item_desp, guint32 item_desp_len,
    FRU_BOARD_AREA_S *fru_board_area_info);
LOCAL gint32 __init_fru_board_extra_model(const guint8 *item_desp, guint32 item_desp_len,
    FRU_BOARD_AREA_S *fru_board_area_info);

ELABEL_ITEM_INFO elabel_datalen_table[] = {
    // item id与数据长度对应关系
    {0x10, 0x01},
    {0x11, FRU_MAX_ELABEL_LEN},
    {0x12, FRU_MAX_ELABEL_LEN},
    {0x20, 0x03},
    {0x21, FRU_MAX_ELABEL_LEN},
    {0x22, FRU_MAX_ELABEL_LEN},
    {0x23, FRU_MAX_ELABEL_LEN},
    {0x24, FRU_MAX_ELABEL_LEN},
    {0x25, FRU_MAX_ELABEL_LEN},
    {0x30, FRU_MAX_ELABEL_LEN},
    {0x31, FRU_MAX_ELABEL_LEN},
    {0x32, FRU_MAX_ELABEL_LEN},
    {0x33, FRU_MAX_ELABEL_LEN},
    {0x34, FRU_MAX_ELABEL_LEN},
    {0x35, FRU_MAX_ELABEL_LEN},
    {0x36, FRU_MAX_ELABEL_LEN},
};

ELABEL_ITEM_INFO elabel_datalen_board_extra_table[] = {
    {0x26, FRU_MAX_ELABEL_LEN},
    {0x27, FRU_MAX_ELABEL_LEN},
    {0x28, FRU_MAX_ELABEL_LEN},
    {0x29, FRU_MAX_ELABEL_LEN},
    {0x2A, FRU_MAX_ELABEL_LEN},
};

ELABEL_ITEM_INFO elabel_modbus_datalen_table[] = {
    {0x20, 0x03},
    {0x21, FRU_MAX_ELABEL_LEN},
    {MB_ELABEL_ITEM_BOARD_PRODUCT_NAME_INDEX, FRU_MAX_ELABEL_LEN},
    {0x23, FRU_MAX_ELABEL_LEN},
    {0x24, FRU_MAX_ELABEL_LEN},
    {0x25, FRU_MAX_ELABEL_LEN},
    {0x30, FRU_MAX_ELABEL_LEN},
    {0x31, FRU_MAX_ELABEL_LEN},
    {0x32, FRU_MAX_ELABEL_LEN},
    {0x33, FRU_MAX_ELABEL_LEN},
    {0x34, FRU_MAX_ELABEL_LEN},
    {0x35, FRU_MAX_ELABEL_LEN},
    {0x36, FRU_MAX_ELABEL_LEN},
};

LOCAL FRU_PRODUCT_DATA g_init_fru_product_data[] = {
    {FRU_PRODUCT_MANUFACTURER, __init_fru_product_manufacturer},
    {FRU_PRODUCT_NAME, __init_fru_product_name},
    {FRU_PRODUCT_PARTNUMBER, __init_fru_product_part_number},
    {FRU_PRODUCT_VERSION, __init_fru_product_version},
    {FRU_PRODUCT_SERIALNUMBER, __init_fru_product_serial_number},
    {FRU_PRODUCT_ASSETTAG, __init_fru_product_assettag},
    {FRU_PRODUCT_FRUFILEID, __init_fru_product_file_id},
};

LOCAL FRU_BOARD_DATA g_fru_board_data[] = {
    {FRU_BOARD_MFGDATE, __init_fru_board_mfgdata},
    {FRU_BOARD_MANUFACTURER, __init_fru_board_manufacture},
    {FRU_BOARD_PRODUCTNAME, __init_fru_board_product_name},
    {FRU_BOARD_SERIALNUMBER, __init_fru_board_serial_number},
    {FRU_BOARD_PARTNUMBER, __init_fru_board_part_number},
    {FRU_BOARD_FRUFILEID, __init_fru_board_file_id},
    {FRU_BOARD_EXTEND_DESP, __init_fru_board_extra_description},
    {FRU_BOARD_EXTEND_ISSUENUMBER, __init_fru_board_extra_issuenumber},
    {FRU_BOARD_EXTEND_CLEICODE, __init_fru_board_extra_CLEIcode},
    {FRU_BOARD_EXTEND_BOM, __init_fru_board_extra_bom},
    {FRU_BOARD_EXTEND_MODEL, __init_fru_board_extra_model},
};

gint32 write_elabel_data_to_mcu(OBJ_HANDLE object_handle, const gchar *method_name, GSList *input_list,
    GSList **output_list)
{
    if (object_handle == 0 || method_name == NULL || input_list == NULL) {
        debug_log(DLOG_INFO, "%s: invalid input prarm.", __FUNCTION__);
        return RET_ERR;
    }

    // 为防止发送写命令过程中MCU复位，因此做重试处理
    guint8 retry_times = 0;
    guint8 write_failed_flag = 0;
    while (dfl_call_class_method(object_handle, method_name, NULL, input_list, output_list) != DFL_OK) {
        debug_log(DLOG_INFO, "%s: update elabel info to mcu failed", __FUNCTION__);
        if (retry_times > 10) { 
            write_failed_flag = 1;
            break;
        }

        vos_task_delay(TASK_DELAY_ONE_SECOND);
        retry_times++;
    }

    // 写失败，返回错误
    if (write_failed_flag == 1) {
        debug_log(DLOG_ERROR, "%s: set elabel info to mcu failed", __FUNCTION__);
        return RET_ERR;
    }
    return RET_OK;
}


gint32 syn_frudata_to_fan_mcu(OBJ_HANDLE object_handle, guint32 offset, guint8 *fru_buf, guint16 length)
{
    
    guint8 fanSlot = 0;
    gint32 ret = dal_get_property_value_byte(object_handle, PROPERTY_FRUDEV_STORAGE_LOC, &fanSlot);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "FAN-[%s]:get fan_slot fail,ret:%d", __FUNCTION__, ret);
        return ret;
    }
    debug_log(DLOG_DEBUG, "FAN-[%s]:fan_slot:%d, get fan_slot success.", __FUNCTION__, fanSlot);

    OBJ_HANDLE fan_object_handle = 0;
    ret = dal_get_specific_object_byte(CLASS_NAME_SMM_FAN, PROPERTY_FAN_SLOT, fanSlot, &fan_object_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "FAN-[%s]:fan_slot:%d, get object failed", __FUNCTION__, fanSlot);
        return ret;
    }

    
    debug_log(DLOG_DEBUG, "FAN-[%s]:fan_slot:%d, fru_buf total length:%d.", __FUNCTION__, fanSlot, length);

    GSList *input_list = NULL;
    input_list = g_slist_append(input_list, g_variant_new_byte(fanSlot));
    input_list = g_slist_append(input_list,
        (gpointer)g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, fru_buf, length, sizeof(guint8)));
    ret = write_elabel_data_to_mcu(fan_object_handle, METHOD_COOLING_WRITE_ELABLE, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "FAN-[%s]:fan_slot:%d, write elabel failed", __FUNCTION__, fanSlot);
        return ret;
    }
    debug_log(DLOG_DEBUG, "FAN-[%s]:fan_slot:%d, write_elabel_data_to_mcu...success..", __FUNCTION__, fanSlot);
    return RET_OK;
}


gint32 syn_frudata_to_mcu(OBJ_HANDLE object_handle, FRU_PROPERTY_S *fru_prop)
{
    if (object_handle == 0 || fru_prop == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter error", __FUNCTION__);
        return RET_ERR;
    }

    // 清除电子标签操作，向MCU发送清除电子标签命令
    // 写操作，直接发送写电子标签命令
    gint32 ret;
    if (fru_prop->dft_operate_type == MCU_CLEAR_ELABEL) {
        ret = __sync_elabel_to_mcu(object_handle, MCU_CLEAR_ELABEL);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: clear elabel info to mcu failed, ret is %d", __FUNCTION__, ret);
        }
    } else {
        FRU_PRIV_PROPERTY_S *fru_priv_property = NULL;
        ret = get_obj_priv_data(object_handle, (gpointer *)&fru_priv_property);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s: call dfl_get_binded_data failed, ret is %d", __FUNCTION__, ret);
            return ret;
        }
        if (fru_priv_property == NULL) {
            debug_log(DLOG_ERROR, "%s: fru_priv_property is NULL", __FUNCTION__);
            return RET_ERR;
        }

        ret = __write_elabel_to_mcu(object_handle, MCU_WRITE_ELABEL, fru_priv_property);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: write elabel info to mcu failed, ret is %d", __FUNCTION__, ret);
        }
        put_obj_priv_data(object_handle, (gpointer)fru_priv_property);
    }

    // 写入/清除电子标签操作成功后，执行更新命令
    if (ret == RET_OK) {
        ret = __sync_elabel_to_mcu(fru_prop->fru_handle, MCU_UPDATE_ELABEL);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: update elabel info to mcu failed, ret is %d", __FUNCTION__, ret);
        }
    }

    return ret;
}


LOCAL gint32 __pack_send_elabel_to_mcu(OBJ_HANDLE component_handle, OBJ_HANDLE *obj_handle, const gchar **func_name,
    guint8 update_type, guint8 dev_type, GSList **input_list)
{
    gint32 ret;
    if (dev_type == COMPONENT_TYPE_ASSET_LOCATE_BRD) {
        *func_name = METHOD_MODBUS_SEND_CMD;
        guint8 device_num = 0;
        (void)dal_get_property_value_byte(component_handle, PROPERTY_COMPONENT_DEVICENUM, &device_num);

        OBJ_HANDLE modbus_slave_obj = 0;
        ret = dal_get_specific_object_byte(CLASS_MODBUS_SLAVE, PROPERTY_MB_SLAVE_ID, device_num, &modbus_slave_obj);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: get modbus slave failed, slaveId is %d", __FUNCTION__, device_num);
            return RET_ERR;
        }
        ret = dfl_get_referenced_object(modbus_slave_obj, PROPERTY_MB_SLAVE_LBUS, obj_handle);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s: get modbus slave %s fail", __FUNCTION__, dfl_get_object_name(modbus_slave_obj));
            return RET_ERR;
        }

        guint32 slave_addr = 0;
        (void)dal_get_property_value_uint32(modbus_slave_obj, PROPERTY_MB_SLAVE_ADDR, &slave_addr);

        const guint8 elable_updata_fixed_len = 2;
        guint8 write_data_index = 0;
        guint8 request_buf[REQ_MAX_DATA_LEN] = {0};
        request_buf[write_data_index++] = elable_updata_fixed_len; // 后面数据长度
        request_buf[write_data_index++] = ELABEL_UPDATE_SUB_FUNC_CODE;
        request_buf[write_data_index++] =
            (update_type == MCU_UPDATE_ELABEL) ? ELABEL_MODBUS_UPDATE_OPERATE : ELABEL_MODBUS_CLEAR_OPERATE;
        *input_list = g_slist_append(*input_list, g_variant_new_uint32(slave_addr));
        *input_list = g_slist_append(*input_list, g_variant_new_byte(ELABEL_FUNC_CODE));
        *input_list = g_slist_append(*input_list,
            g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, request_buf, write_data_index, sizeof(guint8)));
        *input_list = g_slist_append(*input_list, g_variant_new_uint32(MB_RECV_MAX_TMOUT));
    } else {
        *func_name = METHOD_PCIE_CARD_SET_DATA_TO_MCU;
        ret = (dev_type == COMPONENT_TYPE_SOC_BOARD) ?
            dal_get_specific_object_position(component_handle, CLASS_SOC_BOARD, obj_handle) :
            dal_get_specific_object_string(CLASS_PCIE_CARD, PROPERTY_PCIE_CARD_REF_COMPONENT,
                (const gchar *)dfl_get_object_name(component_handle), obj_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: get %s handle fail", __FUNCTION__, dfl_get_object_name(component_handle));
 
            return RET_ERR;
        }

        // 入参封装
        PROTOCOL_HEAD_PARA head_para = { 0 };
        head_para.opcode = REGISTER_KEY_SET_ELABLE;

        *input_list = g_slist_append(*input_list, g_variant_new_uint16(head_para.opcode));
        *input_list = g_slist_append(*input_list, g_variant_new_byte(head_para.data_object_index));
        *input_list = g_slist_append(*input_list, g_variant_new_byte(head_para.cmd_arg));
        *input_list = g_slist_append(*input_list,
            (gpointer)g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, &update_type, sizeof(guint8), sizeof(guint8)));
    }

    return RET_OK;
}


LOCAL gint32 __sync_elabel_to_mcu(OBJ_HANDLE object_handle, guint8 update_type)
{
    if (object_handle == 0) {
        debug_log(DLOG_ERROR, "%s: fru handle is NULL", __FUNCTION__);
        return RET_ERR;
    }

    if (update_type != MCU_UPDATE_ELABEL && update_type != MCU_CLEAR_ELABEL) {
        debug_log(DLOG_ERROR, "%s: the operation do not clear the elabel info, operatio type is %d", __FUNCTION__,
            update_type);
        return RET_ERR;
    }

    // 获取Fru对象的Fru Id
    guint8 fru_id = MAX_FRU_ID;
    (void)dal_get_property_value_byte(object_handle, PROPERTY_FRU_ID, &fru_id);

    OBJ_HANDLE component_handle = 0;
    gint32 ret = dal_get_specific_object_byte(CLASS_COMPONENT, PROPERTY_COMPONENT_FRUID, fru_id, &component_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get Fru component handle failed, Fru(%d)", __FUNCTION__, fru_id);
        return RET_ERR;
    }

    guint8 dev_type = 0;
    OBJ_HANDLE obj_handle = 0;
    const gchar *func_name = NULL;
    GSList *input_list = NULL;
    (void)dal_get_property_value_byte(component_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &dev_type);
    if (__pack_send_elabel_to_mcu(component_handle, &obj_handle, &func_name, update_type, dev_type, &input_list) !=
        RET_OK) {
        return RET_ERR;
    }

    ret = write_elabel_data_to_mcu(obj_handle, func_name, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);

    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: syn elabel info to mcu failed, ret is %d", __FUNCTION__, ret);
        return ret;
    }

    return RET_OK;
}

LOCAL gint32 __get_component_ref_mcu_handle(OBJ_HANDLE component_handle, OBJ_HANDLE *mcu_obj_handle)
{
    guint8 dev_type = 0;
    (void)dal_get_property_value_byte(component_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &dev_type);
    if (dev_type == COMPONENT_TYPE_SOC_BOARD) {
        gint32 ret = dal_get_specific_object_position(component_handle, CLASS_SOC_BOARD, mcu_obj_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: get BCU failed, component_obj name is %s", __FUNCTION__,
                dfl_get_object_name(component_handle));
            return ret;
        }
        return RET_OK;
    }
    if (dev_type != COMPONENT_TYPE_ASSET_LOCATE_BRD) {
        gint32 ret = dal_get_specific_object_string(CLASS_PCIE_CARD, PROPERTY_PCIE_CARD_REF_COMPONENT,
            (const gchar *)dfl_get_object_name(component_handle), mcu_obj_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: get pcie card failed, component_obj name is %s", __FUNCTION__,
                dfl_get_object_name(component_handle));
            return RET_ERR;
        }

        return RET_OK;
    }

    guint8 device_num = 0;
    (void)dal_get_property_value_byte(component_handle, PROPERTY_COMPONENT_DEVICENUM, &device_num);

    OBJ_HANDLE modbus_slave_obj = 0;
    gint32 ret = dal_get_specific_object_byte(CLASS_MODBUS_SLAVE, PROPERTY_MB_SLAVE_ID, device_num, &modbus_slave_obj);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get modbus slave failed, slaveId is %d", __FUNCTION__, device_num);
        return RET_ERR;
    }
    ret = dfl_get_referenced_object(modbus_slave_obj, PROPERTY_MB_SLAVE_LBUS, mcu_obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get refence modbus obj failed, slave bus is %s", __FUNCTION__,
            dfl_get_object_name(modbus_slave_obj));
        return RET_ERR;
    }

    return RET_OK;
}

LOCAL gint32 __get_modbus_slave_addr(OBJ_HANDLE component_handle, guint32 *slave_addr)
{
    guint8 dev_type = 0;
    (void)dal_get_property_value_byte(component_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &dev_type);
    if (dev_type != COMPONENT_TYPE_ASSET_LOCATE_BRD) {
        return RET_OK;
    }

    guint8 device_num = 0;
    (void)dal_get_property_value_byte(component_handle, PROPERTY_COMPONENT_DEVICENUM, &device_num);

    OBJ_HANDLE modbus_slave_obj = 0;
    gint32 ret = dal_get_specific_object_byte(CLASS_MODBUS_SLAVE, PROPERTY_MB_SLAVE_ID, device_num, &modbus_slave_obj);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get modbus slave failed, slaveId is %d", __FUNCTION__, device_num);
        return RET_ERR;
    }

    (void)dal_get_property_value_uint32(modbus_slave_obj, PROPERTY_MB_SLAVE_ADDR, slave_addr);

    return RET_OK;
}

LOCAL gint32 __get_write_buf_for_mcu(FRU_INFO_S *fru_info, ELABEL_INFO_S *elabel_info, guint8 fru_area_index,
    guint8 fru_field_index, guint8 *write_buf, guint8 *write_len)
{
    gint32 ret = RET_ERR;

    switch (fru_area_index) {
        case FRU_AREA_CHASSISINFO:
            if (fru_info->has_chassis) {
                ret = __get_chassis_item_data(&elabel_info->chassis, fru_field_index, write_buf, FRU_DATA_MAX_LEN - 1,
                    write_len);
            }
            break;

        case FRU_AREA_BOARDINFO:
            if (fru_info->has_board) {
                ret = __get_board_item_data(&elabel_info->board, fru_field_index, write_buf, FRU_DATA_MAX_LEN - 1,
                    write_len);
            }
            break;

        case FRU_AREA_PRODUCTINFO:
            if (fru_info->has_product) {
                ret = __get_product_item_data(&elabel_info->product, fru_field_index, write_buf, FRU_DATA_MAX_LEN - 1,
                    write_len);
            }
            break;

        default:
            debug_log(DLOG_ERROR, "%s: fru_area_index is not valid, fru_area_index is %d", __FUNCTION__,
                fru_area_index);
            return RET_ERR;
    }

    return ret;
}


LOCAL gint32 __traverse_write_elabel_to_mcu(OBJ_HANDLE obj_handle, OBJ_HANDLE component_handle,
    ELABEL_INFO_S *elabel_info, FRU_INFO_S *fru_info, guint32 slave_addr)
{
    guint8 request_buf[REQ_MAX_DATA_LEN] = {0};
    PROTOCOL_HEAD_PARA head_para = { 0 };
    const guint8 hex_value = 16;
    const guint8 elabel_modbus_fixed_len = 8;
    for (guint8 item_index = 0; item_index < sizeof(elabel_datalen_table) / sizeof(ELABEL_ITEM_INFO); item_index++) {
        // 处理前清空数据
        GSList *input_list = NULL;
        guint8 write_buf[FRU_DATA_MAX_LEN] = {0};
        guint8 write_len = 0;

        // 获取电子标签的域id以及field id
        guint8 item_id = elabel_datalen_table[item_index].item_id;
        guint8 fru_area_index = item_id / hex_value;
        guint8 fru_field_index = item_id % hex_value;

        // 写入数据封装
        // smbus协议第一个字节表示发送写命令，第二个字节开始表示写入的数据
        write_buf[0] = MCU_WRITE_ELABEL;

        gint32 ret =
            __get_write_buf_for_mcu(fru_info, elabel_info, fru_area_index, fru_field_index, write_buf + 1, &write_len);
        if (ret != RET_OK || write_len < 0) {
            continue;
        }

        const gchar *func_name = NULL;

        // 入参链表封装modbus协议数据封装
        guint8 dev_type = 0;
        (void)dal_get_property_value_byte(component_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &dev_type);
        if (dev_type == COMPONENT_TYPE_ASSET_LOCATE_BRD) {
            // modbus协议暂时不支持chassis域
            if (fru_area_index == ELABEL_AREA_CHASSIS_INDEX) {
                continue;
            }

            func_name = METHOD_MODBUS_SEND_CMD;
            guint8 write_data_index = 0;
            (void)memset_s(request_buf, sizeof(request_buf), 0, sizeof(request_buf));

            // data length
            request_buf[write_data_index++] = elabel_modbus_fixed_len + write_len; // 后面数据长度
            // sub function code
            request_buf[write_data_index++] = ELABEL_WRITE_SUB_FUNC_CODE;
            // elabel selector
            request_buf[write_data_index++] = item_id;
            // offset
            request_buf[write_data_index++] = 0;
            request_buf[write_data_index++] = 0;
            // total length
            request_buf[write_data_index++] = 0;
            request_buf[write_data_index++] = write_len;
            // write data length
            request_buf[write_data_index++] = 0;
            request_buf[write_data_index++] = write_len;

            // write data modbus write_buf第一个字节不处理，跳过
            errno_t safe_fun_ret = memcpy_s(&(request_buf[write_data_index]),
                sizeof(request_buf) - write_data_index, write_buf + 1, write_len);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            }
            input_list = g_slist_append(input_list, g_variant_new_uint32(slave_addr));
            input_list = g_slist_append(input_list, g_variant_new_byte(ELABEL_FUNC_CODE));
            input_list = g_slist_append(input_list, (gpointer)g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE,
                request_buf, request_buf[0] + 1, sizeof(guint8)));
            input_list = g_slist_append(input_list, g_variant_new_uint32(MB_RECV_MAX_TMOUT));
        } else {
            func_name = METHOD_PCIE_CARD_SET_DATA_TO_MCU;
            (void)memset_s(&head_para, sizeof(PROTOCOL_HEAD_PARA), 0, sizeof(PROTOCOL_HEAD_PARA));
            head_para.opcode = REGISTER_KEY_SET_ELABLE;
            head_para.cmd_arg = item_id;
            input_list = g_slist_append(input_list, g_variant_new_uint16(head_para.opcode));
            input_list = g_slist_append(input_list, g_variant_new_byte(head_para.data_object_index));
            input_list = g_slist_append(input_list, g_variant_new_byte(head_para.cmd_arg));
            input_list = g_slist_append(input_list,
                (gpointer)g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, write_buf, write_len + 1, sizeof(guint8)));
        }

        ret = write_elabel_data_to_mcu(obj_handle, func_name, input_list, NULL);
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "%s: write elabel info failed, obj name is %s, item id is %d", __FUNCTION__,
                dfl_get_object_name(obj_handle), item_id);
            return ret;
        }
    }

    return RET_OK;
}


LOCAL gint32 __write_elabel_to_mcu(OBJ_HANDLE object_handle, guint8 update_type, FRU_PRIV_PROPERTY_S *fru_priv_property)
{
    if (object_handle == 0 || fru_priv_property == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL", __FUNCTION__);
        return RET_ERR;
    }

    if (update_type != MCU_WRITE_ELABEL) {
        debug_log(DLOG_ERROR, "%s: the operation do not write the elabel info, operatio type is %d", __FUNCTION__,
            update_type);
        return RET_ERR;
    }

    // 获取Fru对象的Fru Id
    guint8 fru_id = MAX_FRU_ID;
    (void)dal_get_property_value_byte(object_handle, PROPERTY_FRU_ID, &fru_id);

    OBJ_HANDLE component_handle = 0;
    gint32 ret = dal_get_specific_object_byte(CLASS_COMPONENT, PROPERTY_COMPONENT_FRUID, fru_id, &component_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get Fru component handle failed, Fru(%d)", __FUNCTION__, fru_id);
        return RET_ERR;
    }

    OBJ_HANDLE obj_handle = 0;
    ret = __get_component_ref_mcu_handle(component_handle, &obj_handle);
    if (ret != RET_OK) {
        return ret;
    }

    guint32 slave_addr = 0;
    ret = __get_modbus_slave_addr(component_handle, &slave_addr);
    if (ret != RET_OK) {
        return ret;
    }

    ELABEL_INFO_S *elabel_info = fru_priv_property->elabel_info;
    FRU_INFO_S *fru_info = fru_priv_property->fru_info;
    if (elabel_info == NULL || fru_info == NULL) {
        debug_log(DLOG_ERROR, "%s: elabel info is NULL", __FUNCTION__);
        return RET_ERR;
    }

    // 遍历向MCU写入电子标签信息
    return __traverse_write_elabel_to_mcu(obj_handle, component_handle, elabel_info, fru_info, slave_addr);
}


LOCAL gint32 __get_chassis_item_data(ELABEL_CHASSIS_AREA_S *elabel_chassis_area, guint8 field_index, guint8 *buf,
    guint8 buf_len, guint8 *write_buf_len)
{
    if (elabel_chassis_area == NULL || buf == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL", __FUNCTION__);
        return RET_ERR;
    }

    errno_t safe_fun_ret = EOK;
    switch (field_index) {
        case FRU_CHASSIS_TYPE:
            buf[0] = elabel_chassis_area->type.data[0];
            *write_buf_len = 1;
            break;

        case FRU_CHASSIS_PART_NUMBER:
            safe_fun_ret =
                memmove_s((void *)buf, buf_len, elabel_chassis_area->part_num.data, elabel_chassis_area->part_num.len);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
                return RET_ERR;
            }
            *write_buf_len = elabel_chassis_area->part_num.len;
            break;

        case FRU_CHASSIS_SERIAL_NUMBER:
            safe_fun_ret = memmove_s((void *)buf, buf_len, elabel_chassis_area->serial_num.data,
                elabel_chassis_area->serial_num.len);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
                return RET_ERR;
            }
            *write_buf_len = elabel_chassis_area->serial_num.len;
            break;

        default:
            debug_log(DLOG_ERROR, "%s failed, unknown chassis field index:%d", __FUNCTION__, field_index);
            return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 __get_board_item_data(ELABEL_BOARD_AREA_S *elabel_board_area, guint8 field_index, guint8 *buf,
    guint8 buf_len, guint8 *write_buf_len)
{
    if (elabel_board_area == NULL || buf == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL", __FUNCTION__);
        return RET_ERR;
    }

    errno_t safe_fun_ret = EOK;

    switch (field_index) {
        case FRU_BOARD_MFGDATE:
            safe_fun_ret =
                memmove_s((void *)buf, buf_len, elabel_board_area->mfg_time.data, elabel_board_area->mfg_time.len);

            *write_buf_len = elabel_board_area->mfg_time.len;
            break;

        case FRU_BOARD_MANUFACTURER:
            safe_fun_ret = memmove_s((void *)buf, buf_len, elabel_board_area->manufacture.data,
                elabel_board_area->manufacture.len);

            *write_buf_len = elabel_board_area->manufacture.len;
            break;

        case FRU_BOARD_PRODUCTNAME:
            safe_fun_ret = memmove_s((void *)buf, buf_len, elabel_board_area->product_name.data,
                elabel_board_area->product_name.len);

            *write_buf_len = elabel_board_area->product_name.len;
            break;

        case FRU_BOARD_SERIALNUMBER:
            safe_fun_ret =
                memmove_s((void *)buf, buf_len, elabel_board_area->serial_num.data, elabel_board_area->serial_num.len);

            *write_buf_len = elabel_board_area->serial_num.len;
            break;

        case FRU_BOARD_PARTNUMBER:
            safe_fun_ret =
                memmove_s((void *)buf, buf_len, elabel_board_area->part_num.data, elabel_board_area->part_num.len);

            *write_buf_len = elabel_board_area->part_num.len;
            break;

        case FRU_BOARD_FRUFILEID:
            safe_fun_ret =
                memmove_s((void *)buf, buf_len, elabel_board_area->file_id.data, elabel_board_area->file_id.len);

            *write_buf_len = elabel_board_area->file_id.len;
            break;

        default:
            debug_log(DLOG_ERROR, "%s failed, unknown board field index:%d", __FUNCTION__, field_index);
            return RET_ERR;
    }

    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 __get_product_item_data(ELABEL_PRODUCT_AREA_S *elabel_product_area, guint8 field_index, guint8 *buf,
    guint8 buf_len, guint8 *write_buf_len)
{
    if (elabel_product_area == NULL || buf == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL", __FUNCTION__);
        return RET_ERR;
    }

    errno_t safe_fun_ret = EOK;

    switch (field_index) {
        case FRU_PRODUCT_MANUFACTURER:
            safe_fun_ret = memmove_s((void *)buf, buf_len, elabel_product_area->manufacture.data,
                elabel_product_area->manufacture.len);

            *write_buf_len = elabel_product_area->manufacture.len;
            break;

        case FRU_PRODUCT_NAME:
            safe_fun_ret =
                memmove_s((void *)buf, buf_len, elabel_product_area->name.data, elabel_product_area->name.len);

            *write_buf_len = elabel_product_area->name.len;
            break;

        case FRU_PRODUCT_PARTNUMBER:
            safe_fun_ret =
                memmove_s((void *)buf, buf_len, elabel_product_area->part_num.data, elabel_product_area->part_num.len);

            *write_buf_len = elabel_product_area->part_num.len;
            break;

        case FRU_PRODUCT_VERSION:
            safe_fun_ret =
                memmove_s((void *)buf, buf_len, elabel_product_area->version.data, elabel_product_area->version.len);

            *write_buf_len = elabel_product_area->version.len;
            break;

        case FRU_PRODUCT_SERIALNUMBER:
            safe_fun_ret = memmove_s((void *)buf, buf_len, elabel_product_area->serial_num.data,
                elabel_product_area->serial_num.len);

            *write_buf_len = elabel_product_area->serial_num.len;
            break;

        case FRU_PRODUCT_ASSETTAG:
            safe_fun_ret = memmove_s((void *)buf, buf_len, elabel_product_area->asset_tag.data,
                elabel_product_area->asset_tag.len);

            *write_buf_len = elabel_product_area->asset_tag.len;
            break;

        case FRU_PRODUCT_FRUFILEID:
            safe_fun_ret =
                memmove_s((void *)buf, buf_len, elabel_product_area->file_id.data, elabel_product_area->file_id.len);

            *write_buf_len = elabel_product_area->file_id.len;
            break;

        default:
            debug_log(DLOG_ERROR, "%s failed:unknown product info field index:%d", __FUNCTION__, field_index);
            return RET_ERR;
    }

    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 __init_default_mcu_fru_info(OBJ_HANDLE obj_handle, FRU_INFO_S *fru_info)
{
    if (obj_handle == 0 || fru_info == NULL) {
        return RET_ERR;
    }

    // 获取bmc version信息初始化电子标签
    OBJ_HANDLE bmc_global_obj = 0;
    gint32 ret = dfl_get_object_handle(BMC_OBJECT_NAME, &bmc_global_obj);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: call dfl_get_object_handle failed, ret = %d", __FUNCTION__, ret);
        return ret;
    }

    gchar fru_ver[MAX_BMC_VER_LEN + 1] = {0};
    (void)dal_get_property_value_string(bmc_global_obj, BMC_MANA_VER_NAME, fru_ver, sizeof(fru_ver));

    fru_info->has_chassis = FALSE;
    fru_info->chassis.area_ver = DEFAULT_AREA_VER; 
    fru_info->chassis.chassis_part_num.len = 0;
    fru_info->chassis.chassis_part_num.type = CHAR_TYPE;
    fru_info->chassis.chassis_serial_num.len = 0;
    fru_info->chassis.chassis_serial_num.type = CHAR_TYPE;

    fru_info->has_board = TRUE;
    fru_info->board.area_ver = DEFAULT_AREA_VER;
    fru_info->board.board_manufacturer.len = (guint8)strlen(DEFAULT_MFG_HUAWEI);
    fru_info->board.board_manufacturer.type = CHAR_TYPE;
    (void)strncpy_s((gchar *)(fru_info->board.board_manufacturer.content), FRU_MAX_ELABEL_LEN, DEFAULT_MFG_HUAWEI,
        strlen(DEFAULT_MFG_HUAWEI));

    fru_info->board.board_product_name.len = 0;
    fru_info->board.board_product_name.type = CHAR_TYPE;
    fru_info->board.board_serial_num.len = 0;
    fru_info->board.board_serial_num.type = CHAR_TYPE;
    fru_info->board.board_part_num.len = 0;
    fru_info->board.board_part_num.type = CHAR_TYPE;
    fru_info->board.file_id.len = (guint8)strlen(fru_ver);
    fru_info->board.file_id.type = CHAR_TYPE;
    (void)strncpy_s((gchar *)(fru_info->board.file_id.content), FRU_MAX_ELABEL_LEN, fru_ver, FRU_MAX_ELABEL_LEN - 1);

    fru_info->has_product = TRUE;
    fru_info->product.area_ver = DEFAULT_AREA_VER;
    fru_info->product.product_manufacturer.len = (guint8)strlen(DEFAULT_MFG_HUAWEI);
    fru_info->product.product_manufacturer.type = CHAR_TYPE;
    (void)strncpy_s((gchar *)(fru_info->product.product_manufacturer.content), FRU_MAX_ELABEL_LEN, DEFAULT_MFG_HUAWEI,
        strlen(DEFAULT_MFG_HUAWEI));

    fru_info->product.product_name.len = 0;
    fru_info->product.product_name.type = CHAR_TYPE;
    fru_info->product.product_part_num.len = 0;
    fru_info->product.product_part_num.type = CHAR_TYPE;
    fru_info->product.product_version.len = 0;
    fru_info->product.product_version.type = CHAR_TYPE;
    fru_info->product.product_serial_num.len = 0;
    fru_info->product.product_serial_num.type = CHAR_TYPE;
    fru_info->product.product_asset_tag.len = 0;
    fru_info->product.product_asset_tag.type = CHAR_TYPE;
    fru_info->product.file_id.len = (guint8)strlen(fru_ver);
    fru_info->product.file_id.type = CHAR_TYPE;
    (void)strncpy_s((gchar *)(fru_info->product.file_id.content), FRU_MAX_ELABEL_LEN, fru_ver, FRU_MAX_ELABEL_LEN - 1);

    return RET_OK;
}


LOCAL gint32 __init_fru_chassis_info_from_mcu(const guint8 *item_desp, guint32 item_desp_len, guint8 field_index,
    FRU_CHASSIS_AREA_S *fru_chassis_area_info)
{
    if (item_desp == NULL || fru_chassis_area_info == NULL) {
        debug_log(DLOG_ERROR, "%s: invalid fru desp or chassis_area_info", __FUNCTION__);
        return RET_ERR;
    }

    errno_t safe_fun_ret = EOK;

    switch (field_index) {
        case FRU_CHASSIS_TYPE:
            fru_chassis_area_info->chassis_type = item_desp[0];
            break;

        case FRU_CHASSIS_PART_NUMBER:
            fru_chassis_area_info->chassis_part_num.len = (guint8)item_desp_len;
            fru_chassis_area_info->chassis_part_num.type = CHAR_TYPE;
            safe_fun_ret = memmove_s((void *)fru_chassis_area_info->chassis_part_num.content, FRU_MAX_ELABEL_LEN,
                item_desp, item_desp_len);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
                return RET_ERR;
            }
            break;

        case FRU_CHASSIS_SERIAL_NUMBER:
            fru_chassis_area_info->chassis_serial_num.len = (guint8)item_desp_len;
            fru_chassis_area_info->chassis_serial_num.type = CHAR_TYPE;
            safe_fun_ret = memmove_s((void *)fru_chassis_area_info->chassis_serial_num.content, FRU_MAX_ELABEL_LEN,
                item_desp, item_desp_len);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
                return RET_ERR;
            }
            break;

        default:
            debug_log(DLOG_ERROR, "%s failed, unknown chassis field index:%d", __FUNCTION__, field_index);
            return RET_ERR;
    }

    return RET_OK;
}

LOCAL gint32 __init_fru_board_mfgdata(const guint8 *item_desp, guint32 item_desp_len,
    FRU_BOARD_AREA_S *fru_board_area_info)
{
    OBJ_HANDLE obj_handle = 0;
    guint8 elabel_format = 0;
    (void)dfl_get_object_handle(OBJECT_BMC, &obj_handle);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_BMC_ELABELMANUFORMAT, &elabel_format);
    if (elabel_format == 1) {
        errno_t safe_fun_ret = memmove_s((void *)fru_board_area_info->mfg_date_time,
            sizeof(fru_board_area_info->mfg_date_time), (const void *)item_desp, item_desp_len);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            return RET_ERR;
        }
    } else {
        errno_t safe_fun_ret = memmove_s((void *)fru_board_area_info->mfg_date_time,
            sizeof(fru_board_area_info->mfg_date_time) - 1, (const void *)item_desp,
            sizeof(fru_board_area_info->mfg_date_time) - 1);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            return RET_ERR;
        }
    }

    return RET_OK;
}

LOCAL gint32 __init_fru_board_manufacture(const guint8 *item_desp, guint32 item_desp_len,
    FRU_BOARD_AREA_S *fru_board_area_info)
{
    fru_board_area_info->board_manufacturer.len = (guint8)item_desp_len;
    fru_board_area_info->board_manufacturer.type = CHAR_TYPE;
    errno_t safe_fun_ret = memmove_s((void *)fru_board_area_info->board_manufacturer.content, FRU_MAX_ELABEL_LEN,
        item_desp, item_desp_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }

    return RET_OK;
}

LOCAL gint32 __init_fru_board_product_name(const guint8 *item_desp, guint32 item_desp_len,
    FRU_BOARD_AREA_S *fru_board_area_info)
{
    fru_board_area_info->board_product_name.len = (guint8)item_desp_len;
    fru_board_area_info->board_product_name.type = CHAR_TYPE;
    errno_t safe_fun_ret = memmove_s((void *)fru_board_area_info->board_product_name.content, FRU_MAX_ELABEL_LEN,
        item_desp, item_desp_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }

    return RET_OK;
}

LOCAL gint32 __init_fru_board_serial_number(const guint8 *item_desp, guint32 item_desp_len,
    FRU_BOARD_AREA_S *fru_board_area_info)
{
    fru_board_area_info->board_serial_num.len = (guint8)item_desp_len;
    fru_board_area_info->board_serial_num.type = CHAR_TYPE;
    errno_t safe_fun_ret =
        memmove_s((void *)fru_board_area_info->board_serial_num.content, FRU_MAX_ELABEL_LEN, item_desp, item_desp_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }

    return RET_OK;
}

LOCAL gint32 __init_fru_board_part_number(const guint8 *item_desp, guint32 item_desp_len,
    FRU_BOARD_AREA_S *fru_board_area_info)
{
    fru_board_area_info->board_part_num.len = (guint8)item_desp_len;
    fru_board_area_info->board_part_num.type = CHAR_TYPE;
    errno_t safe_fun_ret =
        memmove_s((void *)fru_board_area_info->board_part_num.content, FRU_MAX_ELABEL_LEN, item_desp, item_desp_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }

    return RET_OK;
}

LOCAL gint32 __init_fru_board_file_id(const guint8 *item_desp, guint32 item_desp_len,
    FRU_BOARD_AREA_S *fru_board_area_info)
{
    fru_board_area_info->file_id.len = (guint8)item_desp_len;
    fru_board_area_info->file_id.type = CHAR_TYPE;
    errno_t safe_fun_ret =
        memmove_s((void *)fru_board_area_info->file_id.content, FRU_MAX_ELABEL_LEN, item_desp, item_desp_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }

    return RET_OK;
}

LOCAL gint32 __init_fru_board_extra_description(const guint8 *item_desp, guint32 item_desp_len,
    FRU_BOARD_AREA_S *fru_board_area_info)
{
    gchar temp[FRU_MAX_ELABEL_LEN + 1] = {0};
    errno_t safe_fun_ret =
        memmove_s(temp, FRU_MAX_ELABEL_LEN + 1, item_desp, item_desp_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }
    __add_extension_label(&(fru_board_area_info->extension_label), "Description", temp);
    return RET_OK;
}

LOCAL gint32 __init_fru_board_extra_issuenumber(const guint8 *item_desp, guint32 item_desp_len,
    FRU_BOARD_AREA_S *fru_board_area_info)
{
    gchar temp[FRU_MAX_ELABEL_LEN + 1] = {0};
    errno_t safe_fun_ret =
        memmove_s(temp, FRU_MAX_ELABEL_LEN + 1, item_desp, item_desp_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }
    __add_extension_label(&(fru_board_area_info->extension_label), "IssueNumber", temp);
    return RET_OK;
}

LOCAL gint32 __init_fru_board_extra_CLEIcode(const guint8 *item_desp, guint32 item_desp_len,
    FRU_BOARD_AREA_S *fru_board_area_info)
{
    gchar temp[FRU_MAX_ELABEL_LEN + 1] = {0};
    errno_t safe_fun_ret =
        memmove_s(temp, FRU_MAX_ELABEL_LEN + 1, item_desp, item_desp_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }
    __add_extension_label(&(fru_board_area_info->extension_label), "CLEICode", temp);
    return RET_OK;
}

LOCAL gint32 __init_fru_board_extra_bom(const guint8 *item_desp, guint32 item_desp_len,
    FRU_BOARD_AREA_S *fru_board_area_info)
{
    gchar temp[FRU_MAX_ELABEL_LEN + 1] = {0};
    errno_t safe_fun_ret =
        memmove_s(temp, FRU_MAX_ELABEL_LEN + 1, item_desp, item_desp_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }
    __add_extension_label(&(fru_board_area_info->extension_label), "BOM", temp);
    return RET_OK;
}

LOCAL gint32 __init_fru_board_extra_model(const guint8 *item_desp, guint32 item_desp_len,
    FRU_BOARD_AREA_S *fru_board_area_info)
{
    gchar temp[FRU_MAX_ELABEL_LEN + 1] = {0};
    errno_t safe_fun_ret =
        memmove_s(temp, FRU_MAX_ELABEL_LEN + 1, item_desp, item_desp_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }
    __add_extension_label(&(fru_board_area_info->extension_label), "Model", temp);
    return RET_OK;
}


LOCAL gint32 __init_fru_board_data_from_mcu(const guint8 *item_desp, guint32 item_desp_len, guint8 field_index,
    FRU_BOARD_AREA_S *fru_board_area_info)
{
    if (item_desp == NULL || fru_board_area_info == NULL) {
        debug_log(DLOG_ERROR, "%s: invalid fru desp or board_area_info", __FUNCTION__);
        return RET_ERR;
    }

    size_t count = sizeof(g_fru_board_data) / sizeof(FRU_BOARD_DATA);
    for (size_t i = 0; i < count; i++) {
        if (g_fru_board_data[i].field_index == field_index) {
            return g_fru_board_data[i].__init_fru_board_data(item_desp, item_desp_len, fru_board_area_info);
        }
    }

    debug_log(DLOG_ERROR, "%s failed, unknown board field index:%d", __FUNCTION__, field_index);
    return RET_ERR;
}

LOCAL gint32 __init_fru_product_manufacturer(const guint8 *item_desp, guint32 item_desp_len,
    FRU_PRODUCT_AREA_S *fru_product_area_info)
{
    fru_product_area_info->product_manufacturer.len = (guint8)item_desp_len;
    fru_product_area_info->product_manufacturer.type = CHAR_TYPE;
    errno_t safe_fun_ret = memmove_s((void *)fru_product_area_info->product_manufacturer.content, FRU_MAX_ELABEL_LEN,
        item_desp, item_desp_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }
    return RET_OK;
}

LOCAL gint32 __init_fru_product_name(const guint8 *item_desp, guint32 item_desp_len,
    FRU_PRODUCT_AREA_S *fru_product_area_info)
{
    fru_product_area_info->product_name.len = (guint8)item_desp_len;
    fru_product_area_info->product_name.type = CHAR_TYPE;
    errno_t safe_fun_ret =
        memmove_s((void *)fru_product_area_info->product_name.content, FRU_MAX_ELABEL_LEN, item_desp, item_desp_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }
    return RET_OK;
}

LOCAL gint32 __init_fru_product_part_number(const guint8 *item_desp, guint32 item_desp_len,
    FRU_PRODUCT_AREA_S *fru_product_area_info)
{
    fru_product_area_info->product_part_num.len = (guint8)item_desp_len;
    fru_product_area_info->product_part_num.type = CHAR_TYPE;
    errno_t safe_fun_ret = memmove_s((void *)fru_product_area_info->product_part_num.content, FRU_MAX_ELABEL_LEN,
        item_desp, item_desp_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }
    return RET_OK;
}

LOCAL gint32 __init_fru_product_version(const guint8 *item_desp, guint32 item_desp_len,
    FRU_PRODUCT_AREA_S *fru_product_area_info)
{
    fru_product_area_info->product_version.len = (guint8)item_desp_len;
    fru_product_area_info->product_version.type = CHAR_TYPE;
    errno_t safe_fun_ret =
        memmove_s((void *)fru_product_area_info->product_version.content, FRU_MAX_ELABEL_LEN, item_desp, item_desp_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }
    return RET_OK;
}

LOCAL gint32 __init_fru_product_serial_number(const guint8 *item_desp, guint32 item_desp_len,
    FRU_PRODUCT_AREA_S *fru_product_area_info)
{
    fru_product_area_info->product_serial_num.len = (guint8)item_desp_len;
    fru_product_area_info->product_serial_num.type = CHAR_TYPE;
    errno_t safe_fun_ret = memmove_s((void *)fru_product_area_info->product_serial_num.content, FRU_MAX_ELABEL_LEN,
        item_desp, item_desp_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }
    return RET_OK;
}

LOCAL gint32 __init_fru_product_assettag(const guint8 *item_desp, guint32 item_desp_len,
    FRU_PRODUCT_AREA_S *fru_product_area_info)
{
    fru_product_area_info->product_asset_tag.len = (guint8)item_desp_len;
    fru_product_area_info->product_asset_tag.type = CHAR_TYPE;
    errno_t safe_fun_ret = memmove_s((void *)fru_product_area_info->product_asset_tag.content, FRU_MAX_ELABEL_LEN,
        item_desp, item_desp_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }
    return RET_OK;
}

LOCAL gint32 __init_fru_product_file_id(const guint8 *item_desp, guint32 item_desp_len,
    FRU_PRODUCT_AREA_S *fru_product_area_info)
{
    fru_product_area_info->file_id.len = (guint8)item_desp_len;
    fru_product_area_info->file_id.type = CHAR_TYPE;
    errno_t safe_fun_ret =
        memmove_s((void *)fru_product_area_info->file_id.content, FRU_MAX_ELABEL_LEN, item_desp, item_desp_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 __init_fru_product_data_from_mcu(const guint8 *item_desp, guint32 item_desp_len, guint8 field_index,
    FRU_PRODUCT_AREA_S *fru_product_area_info)
{
    if (item_desp == NULL || fru_product_area_info == NULL) {
        debug_log(DLOG_ERROR, "%s: invalid fru desp or product_area_info", __FUNCTION__);
        return RET_ERR;
    }

    size_t count = sizeof(g_init_fru_product_data) / sizeof(FRU_PRODUCT_DATA);
    for (size_t i = 0; i < count; i++) {
        if (g_init_fru_product_data[i].field_index == field_index) {
            return g_init_fru_product_data[i].__init_fru_product_data(item_desp, item_desp_len, fru_product_area_info);
        }
    }

    debug_log(DLOG_ERROR, "%s failed:unknown product info field index:%d", __FUNCTION__, field_index);
    return RET_ERR;
}


LOCAL gint32 __init_fru_info_from_mcu(const guint8 *item_desp, guint32 item_desp_len, guint8 area_index,
    guint8 field_index, FRU_INFO_S *fru_info)
{
    if (item_desp == NULL || fru_info == NULL) {
        debug_log(DLOG_ERROR, "%s: invalid fru desp or fru_info", __FUNCTION__);
        return RET_ERR;
    }

    if (item_desp_len > FRU_MAX_ELABEL_LEN) {
        debug_log(DLOG_ERROR, "%s: item_desp_len %d is invalid(max %d)", __FUNCTION__, item_desp_len,
            FRU_MAX_ELABEL_LEN);
        return RET_ERR;
    }

    gint32 ret;

    switch (area_index) {
        case FRU_AREA_CHASSISINFO:
            fru_info->has_chassis = TRUE;
            ret = __init_fru_chassis_info_from_mcu(item_desp, item_desp_len, field_index, &fru_info->chassis);
            break;

        case FRU_AREA_BOARDINFO:
            fru_info->has_board = TRUE;
            ret = __init_fru_board_data_from_mcu(item_desp, item_desp_len, field_index, &fru_info->board);
            break;

        case FRU_AREA_PRODUCTINFO:
            fru_info->has_product = TRUE;
            ret = __init_fru_product_data_from_mcu(item_desp, item_desp_len, field_index, &fru_info->product);
            break;

        default:
            ret = RET_ERR;
            debug_log(DLOG_ERROR, "%s: unknown fru area with %d", __FUNCTION__, area_index);
            break;
    }

    return ret;
}


LOCAL MB_ELABEL_ITEM_OP_STATUS __read_asset_board_fru_item_from_mcu(OBJ_HANDLE mb_handle, OBJ_HANDLE mb_slave_handle,
    guint8 fru_id, guint8 item_id, FRU_INFO_S *fru_info_tmp)
{
    const guint8 elabel_data_len = 4;
    guint8 write_data_index = 0;
    guint8 request_buf[REQ_MAX_DATA_LEN] = {0};
    request_buf[write_data_index++] = elabel_data_len; // 后面数据长度
    // sub function code
    request_buf[write_data_index++] = ELABEL_READ_SUB_FUNC_CODE;
    // elabel selector
    request_buf[write_data_index++] = item_id;
    // data offset
    request_buf[write_data_index++] = 0;
    request_buf[write_data_index++] = 0;

    guint32 slave_addr = 0;
    (void)dal_get_property_value_uint32(mb_slave_handle, PROPERTY_MB_SLAVE_ADDR, &slave_addr);

    // 入参数据封装
    GSList *input_list = NULL;
    input_list = g_slist_append(input_list, g_variant_new_uint32(slave_addr));
    input_list = g_slist_append(input_list, g_variant_new_byte(ELABEL_FUNC_CODE));
    input_list = g_slist_append(input_list,
        (gpointer)g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, request_buf, write_data_index, sizeof(guint8)));
    input_list = g_slist_append(input_list, g_variant_new_uint32(MB_RECV_MAX_TMOUT));

    // 单个字段读取最多重试20次，每次延时100ms
    gint32 ret = 0;
    const guint8 delay_million_time = 100;
    const guint8 elabel_retry_time = 20;
    guint8 retry_time = elabel_retry_time;
    GSList *output_list = NULL;
    while ((ret = dfl_call_class_method(mb_handle, METHOD_MODBUS_SEND_CMD, NULL, input_list, &output_list)) != RET_OK) {
        debug_log(DLOG_DEBUG, "%s : call %s failed, ret is %d ", __FUNCTION__, METHOD_MODBUS_SEND_CMD, ret);
        if (retry_time <= 0) {
            break;
        }

        retry_time--;
        vos_task_delay(delay_million_time);
    }

    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != DFL_OK) {
        // 记录读取失败的item项信息
        debug_log(DLOG_ERROR, "%s: get elabel data failed, fru id is %d, item_id is 0x%x, ret is %d", __FUNCTION__,
            fru_id, item_id, ret);
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        return MB_ELABEL_ITEM_READ_FAILED;
    }

    gsize pram_temp = 0;
    const guint8 *rsp_data = (const guint8 *)g_variant_get_fixed_array((GVariant *)g_slist_nth_data(output_list, 0),
        &pram_temp, sizeof(guint8));
    if (rsp_data == NULL) {
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        debug_log(DLOG_ERROR, "%s: g_variant_get_fixed_array failed, fru id is %d, item_id is 0x%x", __FUNCTION__,
            fru_id, item_id);
        return MB_ELABEL_ITEM_READ_FAILED;
    }

    const guint8 elabel_len_info_size = 2;
    guint32 output_data_len = (guint32)pram_temp;
    
    if (output_data_len < (write_data_index + elabel_len_info_size + elabel_len_info_size)) {
        debug_log(DLOG_ERROR, "%s: invalid frame size [%d], must be greater than [%d]", __FUNCTION__, output_data_len,
            (write_data_index + elabel_len_info_size + elabel_len_info_size));
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        return MB_ELABEL_ITEM_READ_FAILED;
    }

    // 基于请求长度，跳过头部的帧字节，返回数据偏移
    write_data_index += 2;
    guint8 data_offset = write_data_index;
    // data_len
    guint32 data_len =
        ((guint16)rsp_data[data_offset] << 8) | ((guint16)rsp_data[data_offset + 1]); // data_len的高低位处理
    // total len
    data_offset += 2;
    guint32 total_len =
        ((guint16)rsp_data[data_offset] << 8) | ((guint16)rsp_data[data_offset + 1]); // total_len的高低位处理
    if (data_len == 0 || total_len == 0) {
        debug_log(DLOG_INFO, "%s %d: fru id [%d], item id [%d], total len is %d, data len is %d", __FUNCTION__,
            __LINE__, fru_id, item_id, total_len, data_len);
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        return MB_ELABEL_ITEM_EMPTY;
    }

    // 返回数据处理
    data_offset += 2;

    
    if (total_len != data_len || output_data_len != (data_offset + data_len + MB_CRC_LEN) ||
        data_len >= REQ_MAX_DATA_LEN) {
        debug_log(DLOG_ERROR,
            "%s : invalid frame, fru id [%d], item id [%d], data offset is %d, frame len is %d, total len is %d, "
            "data "
            "len is %d",
            __FUNCTION__, fru_id, item_id, data_offset, output_data_len, total_len, data_len);
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        return MB_ELABEL_ITEM_READ_FAILED;
    }

    guint8 data[REQ_MAX_DATA_LEN] = {0};
    errno_t safe_fun_ret = memcpy_s(data, REQ_MAX_DATA_LEN, rsp_data + data_offset, data_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

    debug_log(DLOG_DEBUG, "%s: fru id %d ,item id(0x%x), data len is %u", __FUNCTION__, fru_id, item_id, data_len);

    const guint8 hex_value = 16;
    guint8 fru_area_index = item_id / hex_value;
    guint8 fru_field_index = item_id % hex_value;

    // 读取的电子标签初始化fru_info
    ret = __init_fru_info_from_mcu(data, data_len, fru_area_index, fru_field_index, fru_info_tmp);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: init elabel item failed, item id is %d", __FUNCTION__, item_id);
    }

    return MB_ELABEL_ITEM_READ_OK;
}


LOCAL gint32 __wait_asset_ready(OBJ_HANDLE rack_mgmt_handle)
{
    guint16 wait_time = 0;
    (void)dal_get_property_value_uint16(rack_mgmt_handle, PROPERTY_RACK_ASSET_MGMT_UPGRADE_MCU_TIME, &wait_time);

    
    guint32 count = 0;
    guchar asset_mgmt_status = ASSET_MGMT_SVC_OFF;
    const guint32 max_rack_asset_mgmt_ready_time = 600; 
    while ((count <= wait_time) && (count <= max_rack_asset_mgmt_ready_time)) {
        count++;
        (void)dal_get_property_value_byte(rack_mgmt_handle, PROPERTY_RACK_ASSET_MGMT_SVC_STATUS, &asset_mgmt_status);
        if (asset_mgmt_status == ASSET_MGMT_SVC_ON) {
            break;
        }

        vos_task_delay(1000);
    }

    if (count > wait_time || count > max_rack_asset_mgmt_ready_time) {
        debug_log(DLOG_ERROR, "%s failed: wait rack asset management ready timeout", __FUNCTION__);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 __read_asset_board_frudata_from_mcu(OBJ_HANDLE mb_handle, OBJ_HANDLE mb_slave_handle, guint8 fru_id,
    FRU_INFO_S *fru_info_tmp)
{
    if (fru_info_tmp == NULL) {
        debug_log(DLOG_ERROR, "%s: null pointer", __FUNCTION__);
        return RET_ERR;
    }

    OBJ_HANDLE rack_mgmt_handle = 0;
    gint32 result = dal_get_object_handle_nth(CLASS_RACK_ASSET_MGMT, 0, &rack_mgmt_handle);
    if (result != RET_OK) {
        debug_log(DLOG_ERROR, "%s failed: cann't get object handle for %s", __FUNCTION__, CLASS_RACK_ASSET_MGMT);
        return result;
    }

    
    if (__wait_asset_ready(rack_mgmt_handle) != RET_OK) {
        return RET_ERR;
    }
    debug_log(DLOG_INFO, "%s Begin to sync elabel data for %s", __FUNCTION__, dfl_get_object_name(mb_handle));

    
    MB_ELABEL_ITEM_OP_STATUS op_status = __read_asset_board_fru_item_from_mcu(mb_handle, mb_slave_handle, fru_id,
        MB_ELABEL_ITEM_BOARD_PRODUCT_NAME_INDEX, fru_info_tmp);
    if (op_status == MB_ELABEL_ITEM_EMPTY) {
        debug_log(DLOG_ERROR,
            "%s elabel for (fru id [%d], slave name [%s]) is empty, will not read more elabel content", __FUNCTION__,
            fru_id, dfl_get_object_name(mb_slave_handle));
        return RET_OK;
    }

    guint8 item_index = 0;
    guint8 read_succeed_item_num = 0;
    for (item_index = 0; item_index < sizeof(elabel_modbus_datalen_table) / sizeof(ELABEL_ITEM_INFO); item_index++) {
        guint8 item_id = elabel_modbus_datalen_table[item_index].item_id;
        op_status = __read_asset_board_fru_item_from_mcu(mb_handle, mb_slave_handle, fru_id, item_id, fru_info_tmp);
        if (op_status != MB_ELABEL_ITEM_READ_FAILED) {
            read_succeed_item_num++;
        }
    }

    // 只要有一个读取失败，根据重试机制读取电子标签
    if (item_index != read_succeed_item_num) {
        debug_log(DLOG_INFO, "%s: fru id %d read succeed item num is %d, total num is %d", __FUNCTION__, fru_id,
            read_succeed_item_num, item_index);
        return RET_ERR;
    }

    debug_log(DLOG_ERROR, "%s: fru id %d read all item data succeed", __FUNCTION__, fru_id);
    return RET_OK;
}


LOCAL gint32 __get_opcode_and_objhandle(OBJ_HANDLE card_handle, OBJ_HANDLE *object_handle, PROTOCOL_HEAD_PARA *req_para)
{
    gchar class_name[MAX_NAME_SIZE] = {0};
    OBJ_HANDLE basecard_obj_handle = 0;

    req_para->opcode = REGISTER_KEY_GET_ELABLE;

    // 获取pciecard类失败时，继续获取mcu信息
    gint32 ret = dfl_get_class_name(card_handle, class_name, MAX_NAME_SIZE);
    if (ret != DFL_OK) {
        return RET_OK;
    }
    // 获取类对象为pciecard
    if (g_strcmp0(class_name, CLASS_EXTEND_PCIECARD_NAME) != 0) {
        return RET_OK;
    }
    ret = dfl_get_referenced_object(card_handle, PROPETRY_EXTEND_PCIECARD_BASECARD, &basecard_obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get extend pcie card base, object: %s, ret=%d", __FUNCTION__,
            dfl_get_object_name(card_handle), ret);
        return RET_ERR;
    }

    debug_log(DLOG_INFO, "%s: get extend pcie card base, extend: %s, object_basecard : %s", __FUNCTION__,
        dfl_get_object_name(card_handle), dfl_get_object_name(basecard_obj_handle));

    // 获取sdi5x网络卡的电子标签，私有数据对象句柄用计算卡的pciecard, opcode用0x314
    *object_handle = basecard_obj_handle;
    req_para->opcode = REGISTER_KEY_GET_NETCARD_ELABLE;
    return RET_OK;
}

LOCAL gint32 __read_fru_from_mcu(OBJ_HANDLE card_handle, FRU_INFO_S *fru_info_tmp, ELABEL_ITEM_INFO *elabel_table,
    gint32 len, PROTOCOL_HEAD_PARA *req_para)
{
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    for (guint8 item_index = 0; item_index < len / sizeof(ELABEL_ITEM_INFO); item_index++) {
        // 电子标签域ID
        guint8 item_id = elabel_table[item_index].item_id;
        req_para->cmd_arg = item_id;
        // 入参数据封装
        input_list = g_slist_append(input_list, g_variant_new_uint16(req_para->opcode));
        input_list = g_slist_append(input_list, g_variant_new_byte(req_para->data_object_index));
        input_list = g_slist_append(input_list, g_variant_new_byte(req_para->cmd_arg));
        input_list = g_slist_append(input_list, (gpointer)g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE,
            req_para->input_data, req_para->input_data_len, sizeof(guint8)));
        gint32 result =
            dfl_call_class_method(card_handle, METHOD_PCIE_CARD_GET_DATA_FROM_MCU, NULL, input_list, &output_list);
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        input_list = NULL;
        // 返回对象不存在时，直接返回
        if (result == ERRCODE_OBJECT_NOT_EXIST) {
            debug_log(DLOG_ERROR, "%s: get elabel data failed, obj name is %s, item_id is %d, result is %d",
                __FUNCTION__, dfl_get_object_name(card_handle), req_para->cmd_arg, result);
            return result;
        }

        if (result != DFL_OK) {
            debug_log(DLOG_DEBUG, "%s: get elabel data failed, item_id is %d, result is %d, object %s", __FUNCTION__,
                req_para->cmd_arg, result, dfl_get_object_name(card_handle));
            vos_task_delay(50);
            continue;
        }

        gsize parm_temp = 0;
        const guint8 *output_data = (const guint8 *)g_variant_get_fixed_array(
            (GVariant *)g_slist_nth_data(output_list, 0), &parm_temp, sizeof(guint8));
        guint32 output_data_len = (guint32)parm_temp;
        const guint8 hex_value = 16;
        guint8 fru_area_index = item_id / hex_value;
        guint8 fru_field_index = item_id % hex_value;
        // 读取的电子标签初始化fru_info
        if (output_data_len > 0) {
            result =
                __init_fru_info_from_mcu(output_data, output_data_len, fru_area_index, fru_field_index, fru_info_tmp);
        }

        if (result != RET_OK) {
            debug_log(DLOG_DEBUG, "%s: init elabel item failed, item id is %d", __FUNCTION__, item_id);
        }
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        output_list = NULL;
        // 每读取一次数据，延时50ms
        vos_task_delay(50);
    }
    return RET_OK;
}


LOCAL gint32 __read_card_obj_fru_from_mcu(OBJ_HANDLE card_handle, FRU_INFO_S *fru_info_tmp)
{
    if (fru_info_tmp == NULL) {
        debug_log(DLOG_ERROR, "%s: null pointer", __FUNCTION__);
        return RET_ERR;
    }
    PROTOCOL_HEAD_PARA req_para = { 0 };
    OBJ_HANDLE object_handle = 0;
    gint32 ret = __get_opcode_and_objhandle(card_handle, &object_handle, &req_para);
    if (ret != RET_OK) {
        return RET_ERR;
    }
    if (object_handle != 0) {
        card_handle = object_handle;
    }
    ret = __read_fru_from_mcu(card_handle, fru_info_tmp, elabel_datalen_table, sizeof(elabel_datalen_table), &req_para);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get elable failed, ret =%d", __FUNCTION__, ret);
        return RET_ERR;
    }
    if (req_para.opcode == REGISTER_KEY_GET_NETCARD_ELABLE) {
        ret = __read_fru_from_mcu(card_handle, fru_info_tmp, elabel_datalen_board_extra_table,
            sizeof(elabel_datalen_board_extra_table), &req_para);
    }
    return ret;
}

LOCAL gint32 __get_modbus_slave_obj_handle(OBJ_HANDLE component_handle, guint8 dev_type, OBJ_HANDLE *obj_handle)
{
    guint8 device_num = 0;
    (void)dal_get_property_value_byte(component_handle, PROPERTY_COMPONENT_DEVICENUM, &device_num);
    gint32 ret = dal_get_specific_object_byte(CLASS_MODBUS_SLAVE, PROPERTY_MB_SLAVE_ID, device_num, obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get modbus slave failed, slaveId is %d", __FUNCTION__, device_num);
        return ret;
    }
    return RET_OK;
}

LOCAL gint32 __get_ref_obj_handle(OBJ_HANDLE component_handle, guint8 dev_type, OBJ_HANDLE *obj_handle)
{
    if (dev_type == COMPONENT_TYPE_SOC_BOARD) {
        gint32 ret = dal_get_specific_object_position(component_handle, CLASS_SOC_BOARD, obj_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: get soc board failed, component_obj name is %s", __FUNCTION__,
                dfl_get_object_name(component_handle));
            return ret;
        }
        return RET_OK;
    }
    if (dev_type != COMPONENT_TYPE_ASSET_LOCATE_BRD) {
        gint32 ret = dal_get_specific_object_string(CLASS_PCIE_CARD, PROPERTY_PCIE_CARD_REF_COMPONENT,
            (const gchar *)dfl_get_object_name(component_handle), obj_handle);
        if (ret != RET_OK) {
            ret = dal_get_specific_object_string(CLASS_EXTEND_PCIECARD_NAME, PROPERTY_PCIE_CARD_REF_COMPONENT,
            (const gchar *)dfl_get_object_name(component_handle), obj_handle);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s: get pcie card failed, component_obj name is %s", __FUNCTION__,
                    dfl_get_object_name(component_handle));
                return ret;
            }
        }
        return RET_OK;
    }

    OBJ_HANDLE modbus_slave_obj = 0;
    gint32 ret = __get_modbus_slave_obj_handle(component_handle, dev_type, &modbus_slave_obj);
    if (ret != RET_OK) {
        return ret;
    }

    ret = dfl_get_referenced_object(modbus_slave_obj, PROPERTY_MB_SLAVE_LBUS, obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get refence modbus obj failed, slave bus is %s", __FUNCTION__,
            dfl_get_object_name(modbus_slave_obj));
        return ret;
    }
    return RET_OK;
}

LOCAL gboolean __is_exist_ref_elabel(OBJ_HANDLE object_handle)
{
    OBJ_HANDLE elabel_handle = 0;
    gint32 ret = dfl_get_referenced_object(object_handle, PROPERTY_FRU_ELABEL_RO, &elabel_handle);
    if (ret != DFL_OK) {
        return FALSE;
    }
    return TRUE;
}


LOCAL void init_product_fru_from_data(char *src_data, guint16 data_size, FRU_INFO_S *fru_info)
{
    fru_info->has_product = TRUE;

    
    get_correspond_elabel_data(src_data, PRODUCT_ELABEL_KEY_BAR_CODE, &(fru_info->product.product_serial_num));

    
    get_correspond_elabel_data(src_data, PRODUCT_ELABEL_KEY_ITEM, &(fru_info->product.product_part_num));

    
    get_correspond_elabel_data(src_data, PRODUCT_ELABEL_KEY_VEN_NAME, &(fru_info->product.product_manufacturer));

    
    get_correspond_elabel_data(src_data, PRODUCT_ELABEL_KEY_ISSUE_NUM, &(fru_info->product.product_version));

    
    get_correspond_elabel_data(src_data, PRODUCT_ELABEL_KEY_BOARD_TYPE, &(fru_info->product.product_name));
}


LOCAL void init_chassis_fru_from_data(char *src_data, guint16 data_size, FRU_INFO_S *fru_info)
{
    fru_info->has_chassis = TRUE;
    fru_info->chassis.area_ver = 0x01;
    fru_info->chassis.chassis_type = 0x01;

    
    get_correspond_elabel_data(src_data, PRODUCT_ELABEL_KEY_BAR_CODE, &(fru_info->chassis.chassis_serial_num));

    
    get_correspond_elabel_data(src_data, PRODUCT_ELABEL_KEY_ITEM, &(fru_info->chassis.chassis_part_num));
}


LOCAL void parse_fru_from_ps_mcu(const guint8 *output_data, guint32 output_data_len, FRU_INFO_S *fru_info)
{
    char buf[FRU_FILE_MAX_LEN] = {0};
    int ret;

    ret = memcpy_s(buf, sizeof(buf), output_data, output_data_len);
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "memcpy_s data to buf failed, ret=%d, output_data_len=%d, buf_len=%zu", ret,
            output_data_len, sizeof(buf));
        return;
    }
    init_chassis_fru_from_data(buf, FRU_FILE_MAX_LEN, fru_info);
    init_board_fru_from_data(buf, FRU_FILE_MAX_LEN, fru_info);
    init_product_fru_from_data(buf, FRU_FILE_MAX_LEN, fru_info);
}


LOCAL gint32 power_obj_fru_from_mcu(guint8 slot_id, FRU_INFO_S *fru_info_tmp)
{
#define HEX_VALUE 16
#define DELAY_FIFTY_MILLION 50

    gint32 result;
    GSList *input_list = NULL;
    GSList *output_list = NULL;

    const guint8 *output_data = NULL;
    guint32 output_data_len = 0;
    gsize parm_temp = 0;
    OBJ_HANDLE ps_handle = 0;

    if (fru_info_tmp == NULL) {
        debug_log(DLOG_ERROR, "%s: null pointer", __FUNCTION__);
        return RET_ERR;
    }

    
    input_list = g_slist_append(input_list, g_variant_new_byte(slot_id));
    result = dal_get_specific_object_byte(CLASS_NAME_PS, PROTERY_PS_INDEX, slot_id, &ps_handle);
    if (result != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get OnePower handle failed, slot(%d)", __FUNCTION__, slot_id);
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        return RET_ERR;
    }

    result = dfl_call_class_method(ps_handle, PROTERY_PS_GET_PS_ELABEL_DATA, NULL, input_list, &output_list);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);

    
    if (result != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get ps%d elabel data failed, obj name is %s, result is %d", __FUNCTION__, slot_id,
            dfl_get_object_name(ps_handle), result);
        return result;
    }

    if (output_list != NULL) {
        output_data = (const guint8 *)g_variant_get_fixed_array((GVariant *)g_slist_nth_data(output_list, 0),
            &parm_temp, sizeof(guint8));
        output_data_len = (guint32)parm_temp;
    }

    
    if (output_data_len > 0 && output_data != NULL) {
        parse_fru_from_ps_mcu(output_data, output_data_len, fru_info_tmp);
    }
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

    return RET_OK;
}


gint32 frudev_read_from_power_mcu(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file)
{
    gint32 ret;
    guint8 ps_id = 0;
    OBJ_HANDLE frudev_handle = 0;
    FRU_INFO_S fru_info_tmp = { 0 };

    // 初始化默认的电子标签信息
    ret = __init_default_mcu_fru_info(object_handle, &fru_info_tmp);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: call __init_default_mcu_fru_info failed", __FUNCTION__);
        return ret;
    }

    ret = dfl_get_referenced_object(object_handle, PROPERTY_FRUDEV_RO, &frudev_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "This fru is not support elabel.");
        return RET_OK;
    }

    (void)dal_get_property_value_byte(frudev_handle, PROPERTY_FRUDEV_STORAGE_LOC, &ps_id);
    ret = power_obj_fru_from_mcu(ps_id, &fru_info_tmp);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: call read_power_obj_elabel_from_mcu failed, result is %d", __FUNCTION__, ret);
        return ret;
    }

    
    generate_fru_file(EEPROM_FORMAT_V2, &fru_info_tmp, fru_file);
    return RET_OK;
}


gint32 frudev_read_from_mcu(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file)
{
    if (object_handle == 0 || fru_file == NULL) {
        debug_log(DLOG_ERROR, "%s: input para is not valid", __FUNCTION__);
        return RET_ERR;
    }

    // 获取Fru对象的Fru Id
    guint8 fru_id = MAX_FRU_ID;
    (void)dal_get_property_value_byte(object_handle, PROPERTY_FRU_ID, &fru_id);

    OBJ_HANDLE component_handle = 0;
    gint32 ret = dal_get_specific_object_byte(CLASS_COMPONENT, PROPERTY_COMPONENT_FRUID, fru_id, &component_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get Fru component handle failed, Fru(%d)", __FUNCTION__, fru_id);
        return RET_ERR;
    }

    // 根据component对象名获取对象名
    guint8 dev_type = 0;
    (void)dal_get_property_value_byte(component_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &dev_type);

    OBJ_HANDLE obj_handle = 0;
    ret = __get_ref_obj_handle(component_handle, dev_type, &obj_handle);
    if (ret != RET_OK) {
        return ret;
    }

    if (!__is_exist_ref_elabel(object_handle)) {
        return RET_OK;
    }

    // 初始化默认的电子标签信息
    FRU_INFO_S fru_info_tmp = { 0 };
    ret = __init_default_mcu_fru_info(object_handle, &fru_info_tmp);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: call __init_default_mcu_fru_info failed", __FUNCTION__);
        return ret;
    }

    if (dev_type == COMPONENT_TYPE_ASSET_LOCATE_BRD) {
        OBJ_HANDLE modbus_slave_obj = 0;
        ret = __get_modbus_slave_obj_handle(component_handle, dev_type, &modbus_slave_obj);
        if (ret != RET_OK) {
            return ret;
        }
        ret = __read_asset_board_frudata_from_mcu(obj_handle, modbus_slave_obj, fru_id, &fru_info_tmp);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: call __read_asset_board_frudata_from_mcu failed, ret is %d", __FUNCTION__, ret);
            return ret;
        }
    } else {
        ret = __read_card_obj_fru_from_mcu(obj_handle, &fru_info_tmp);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: call __read_card_obj_fru_from_mcu failed, ret is %d", __FUNCTION__, ret);
            return ret;
        }
    }

    // fru_file文件生成
    generate_fru_file(EEPROM_FORMAT_V2, &fru_info_tmp, fru_file);

    return RET_OK;
}

LOCAL gint32 __read_cooling_elable_by_fan_slot(guint8 fanSlot, GSList **output_list)
{
    OBJ_HANDLE fan_object_handle = 0;
    gint32 ret = dal_get_specific_object_byte(CLASS_NAME_SMM_FAN, PROPERTY_FAN_SLOT, fanSlot, &fan_object_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s dal_get_specific_object_byte failed, ret = %d", __FUNCTION__, ret);
        return ret;
    }

    
    GSList *input_list = NULL;
    input_list = g_slist_append(input_list, g_variant_new_byte(fanSlot));
    ret = dfl_call_class_method(fan_object_handle, METHOD_COOLING_READ_ELABLE, NULL, input_list, output_list);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s dfl_call_class_method error, ret:%d", __FUNCTION__, ret);
    }

    return ret;
}

LOCAL gint32 __save_cooling_fru_info(GSList *output_list, FRU_FILE_S *fru_file)
{
    gsize parm_temp = 0;
    const guint8 *output_data = (const guint8 *)g_variant_get_fixed_array((GVariant *)g_slist_nth_data(output_list, 0),
        &parm_temp, sizeof(guint8));
    if ((output_data == NULL)) {
        debug_log(DLOG_ERROR, "%s:g_variant_get_fixed_array return NULL error.", __FUNCTION__);
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        return RET_ERR;
    }

    guint32 output_data_len = (guint32)parm_temp;
    gint32 ret = memcpy_s(fru_file, sizeof(FRU_FILE_S), output_data, output_data_len);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "FAN-[%s] memcpy_s failed", __FUNCTION__);
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        return ret;
    }

    debug_log(DLOG_DEBUG, "%s frufile len:%d.sucess", __FUNCTION__, output_data_len);
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

    return RET_OK;
}

LOCAL gint32 __save_default_fru_info(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file)
{
    FRU_INFO_S fru_info_tmp = { 0 };
    gint32 ret = __init_default_mcu_fru_info(object_handle, &fru_info_tmp);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s init default failed", __FUNCTION__);
        return ret;
    }

    ret = generate_fru_file(EEPROM_FORMAT_V2, &fru_info_tmp, fru_file);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s gen file failed", __FUNCTION__);
        return ret;
    }

    guint32 output_data_len = fru_file->header.fru_head.len + sizeof(FRU_FILE_HEADER_S);
    debug_log(DLOG_ERROR, "%s default,len:%d.", __FUNCTION__, output_data_len);

    return RET_OK;
}

gint32 frudev_read_fan_from_mcu(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file)
{
    
    guint8 fanSlot = 0;
    gint32 ret = dal_get_property_value_byte(object_handle, PROPERTY_FRUDEV_STORAGE_LOC, &fanSlot);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:%s,fan_slot failed! ret:%d", __FUNCTION__, dfl_get_object_name(object_handle), ret);
        return ret;
    }

    GSList *output_list = NULL;
    ret = __read_cooling_elable_by_fan_slot(fanSlot, &output_list);
    if (ret == DFL_OK) {
        return __save_cooling_fru_info(output_list, fru_file);
    }

    
    debug_log(DLOG_ERROR, "FAN-[%s]***:fan_slot:%d, read fan fru failed, ret:%d", __FUNCTION__, fanSlot, ret);
    return __save_default_fru_info(object_handle, fru_file);
}