

#include <math.h>
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "common.h"
#include "media/virtual_psu.h"


#define PMBUS_CMD_MFR_ID 0x99
#define PMBUS_CMD_MFR_MODEL 0x9A
#define PMBUS_CMD_MFR_REVISION 0x9B
#define PMBUS_CMD_MFR_DATE 0x9D
#define PMBUS_CMD_MFR_SERIAL 0x9E
#define PMBUS_CMD_PART_NUMBER 0xDE

#define PMBUS_ADDR_WR_LEN 1
#define PMBUS_CMD_LEN 1
#define PMBUS_ADDR_RD_LEN 1
#define PMBUS_BYTE_CNT_LEN 1
#define PMBUS_CS_LEN 1
#define PMBUS_BUFFER_MAX_LEN 32
#define PMBUS_RETRY_CNT 5

#define PMBUS_ADDR_WR_OFF 0
#define PMBUS_CMD_OFF (PMBUS_ADDR_WR_OFF + PMBUS_ADDR_WR_LEN)
#define PMBUS_REQ_DATA_OFF (PMBUS_CMD_OFF + PMBUS_CMD_LEN)
#define PMBUS_ADDR_RD_OFF (PMBUS_CMD_OFF + PMBUS_CMD_LEN)
#define PMBUS_RESP_DATA_OFF (PMBUS_ADDR_RD_OFF + PMBUS_ADDR_RD_LEN)
#define PMBUS_BYTE_CNT_OFF (PMBUS_RESP_DATA_OFF)

#define PMBUS_GET_ADDR_WR(a) ((a) & (~1UL))
#define PMBUS_GET_ADDR_RD(a) ((a) | 1UL)

LOCAL gint32 __pmbus_get_dynamic_block(OBJ_HANDLE handle, guint8 command, guint8 *p_out, gsize out_len);


LOCAL gint32 __pmbus_get_dynamic_block(OBJ_HANDLE handle, guint8 command, guint8 *p_out, gsize out_len)
{
    guint32 addr_tmp = 0;
    (void)dal_get_property_value_uint32(handle, PROPERTY_CHIP_ADDR, &addr_tmp);

    guint8 recv_buff[PMBUS_BUFFER_MAX_LEN] = {0};
    recv_buff[PMBUS_ADDR_WR_OFF] = PMBUS_GET_ADDR_WR(addr_tmp);
    recv_buff[PMBUS_CMD_OFF] = command;
    recv_buff[PMBUS_ADDR_RD_OFF] = PMBUS_GET_ADDR_RD(addr_tmp);

    guint8 checksum;
    gsize checksum_offset;
    gint8 trytime;
    guint8 read_len = PMBUS_CS_LEN;
    gint32 ret;
    gboolean read_success = FALSE;

    for (trytime = 1; trytime <= PMBUS_RETRY_CNT; trytime++) {
        
        vos_task_delay(10);

        
        ret = dfl_chip_blkread(dfl_get_object_name(handle), command, 1, recv_buff + PMBUS_BYTE_CNT_OFF);
        if (ret != DFL_OK) {
            continue;
        }

        
        read_len = PMBUS_BYTE_CNT_LEN + recv_buff[PMBUS_BYTE_CNT_OFF] + PMBUS_CS_LEN;
        if (read_len > sizeof(recv_buff) - PMBUS_RESP_DATA_OFF) {
            debug_log(DLOG_ERROR, "%s: invalid read len: %d", __FUNCTION__, read_len);
            continue;
        }

        vos_task_delay(10); 
        ret = dfl_chip_blkread(dfl_get_object_name(handle), command, read_len, recv_buff + PMBUS_RESP_DATA_OFF);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s: read block from %s failed, ret = %d, try:%d", __FUNCTION__,
                dfl_get_object_name(handle), ret, trytime);
            continue;
        }

        checksum_offset = PMBUS_RESP_DATA_OFF + PMBUS_BYTE_CNT_LEN + recv_buff[PMBUS_BYTE_CNT_OFF];
        checksum = dal_cal_crc8(0, (guint8 *)recv_buff, checksum_offset);

        debug_log(DLOG_DEBUG, "read_len:%d, recv_crc:%02x, calc_crc:%02x, retry:%d ", read_len,
            recv_buff[checksum_offset], checksum, trytime);
        if (recv_buff[checksum_offset] == checksum) {
            read_success = TRUE;
            break;
        }
    }

    if (read_success == FALSE) {
        return RET_ERR;
    }
    if (memcpy_s(p_out, out_len, recv_buff + PMBUS_BYTE_CNT_OFF, read_len - PMBUS_CS_LEN) != EOK) {
        return RET_ERR;
    }
    return RET_OK;
}


gint32 get_fru_by_ps_id(guint8 ps_id, PS_FRU_S *fru_data)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret = dal_get_object_handle_nth(CLASS_NAME_AMMETER, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get SysPower object handle failed, ret = %d", ret);
        return RET_ERR;
    }

    GSList *input_list = NULL;
    input_list = g_slist_append(input_list, g_variant_new_byte(ps_id));

    GSList *output_list = NULL;
    ret = dfl_call_class_method(obj_handle, METHOD_GET_FRU_FROM_PSU, NULL, input_list, &output_list);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Call %s.%s failed, ret = %d", CLASS_NAME_AMMETER, METHOD_GET_FRU_FROM_PSU, ret);
        return RET_ERR;
    }

    if (output_list == NULL) {
        debug_log(DLOG_ERROR, "get output_list of %s failed", METHOD_GET_FRU_FROM_PSU);
        return RET_ERR;
    }

    gsize out_size = 0;
    const guint8 *out_buffer = (const guint8 *)g_variant_get_fixed_array((GVariant *)g_slist_nth_data(output_list, 0),
        &out_size, sizeof(guint8));
    if (out_buffer == NULL) {
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        return RET_ERR;
    }
    ret = memcpy_s(fru_data, sizeof(PS_FRU_S), out_buffer, out_size);
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "memcpy_s error:%d", ret);
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        return RET_ERR;
    }
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

    
    if (fru_data->fru_state == FRU_STATE_OK) {
        return RET_OK;
    }

    debug_log(DLOG_ERROR, "fru %d not avaliable yet, state = %d", ps_id, fru_data->fru_state);
    return RET_ERR;
}

LOCAL void __psu_log_handle(guint8 ps_id, gint32 ret, gchar *name, gint8 name_len)
{
    if (strlen(name) > name_len) {
        return;
    }
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get pem[%d] %s fail! ret = %d", ps_id, name, ret);
    }
    return;
}


gint32 get_frudata_from_psu(guint8 ps_id, PS_FRU_S *fru_data)
{
    
    OBJ_HANDLE object_handle = 0;
    gint32 ret = dal_get_object_handle_nth(CLASS_NAME_AMMETER, 0, &object_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get syspower obj_handle failed.");
        return ret;
    }

    guint8 source_type = 0;
    ret = dal_get_property_value_byte(object_handle, PROTERY_SYSPOWER_FRU_SOURCE, &source_type);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get fru source type failed.");
        return ret;
    }
    if (source_type != FRU_SOURCE_FROM_PSU) {
        return DEV_TYPE_INVALID;
    }

    ret = dal_get_specific_object_byte(CLASS_POWER_SLOT, PROPERTY_SLOT_ID, ps_id - 1, &object_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get psuslot object of pem%d failed.ret = %d.", ps_id, ret);
        return ret;
    }

    OBJ_HANDLE psu_handle = 0;
    ret = dfl_get_referenced_object(object_handle, PROPERTY_SLOT_PSU_CHIP, &psu_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "get psuslot chip of pem%d failed.ret = %d.", ps_id, ret);
        return ret;
    }

    
    
    guint8 *p_fru_buffer = (guint8 *)fru_data;
    ret = __pmbus_get_dynamic_block(psu_handle, PMBUS_CMD_MFR_ID, p_fru_buffer + MFR_ID_LEN_OFF, FRU_ITEM_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get pem[%d] manufacturer fail! ret = %d", ps_id, ret);
    }

    
    ret = __pmbus_get_dynamic_block(psu_handle, PMBUS_CMD_MFR_MODEL, p_fru_buffer + MFR_MODEL_LEN_OFF, FRU_ITEM_LEN);
    __psu_log_handle(ps_id, ret, "mode", strlen("mode"));

    
    ret =
        __pmbus_get_dynamic_block(psu_handle, PMBUS_CMD_MFR_REVISION, p_fru_buffer + MFR_VERSION_LEN_OFF, FRU_ITEM_LEN);
    __psu_log_handle(ps_id, ret, "version", strlen("version"));

    
    ret = __pmbus_get_dynamic_block(psu_handle, PMBUS_CMD_MFR_DATE, p_fru_buffer + MFR_DATE_LEN_OFF, FRU_ITEM_LEN);
    __psu_log_handle(ps_id, ret, "data", strlen("data"));

    
    ret = __pmbus_get_dynamic_block(psu_handle, PMBUS_CMD_MFR_SERIAL, p_fru_buffer + MFR_SN_LEN_OFF, FRU_ITEM_LEN);
    __psu_log_handle(ps_id, ret, "sn", strlen("sn"));

    
    ret = __pmbus_get_dynamic_block(psu_handle, PMBUS_CMD_PART_NUMBER, p_fru_buffer + FUR_PART_NUMBER_LEN_OFF,
        FRU_ITEM_LEN);
    __psu_log_handle(ps_id, ret, "part_num", strlen("part_num"));

    fru_data->fru_state = FRU_STATE_OK;

    return RET_OK;
}