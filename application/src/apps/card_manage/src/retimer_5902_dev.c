

#include <stdio.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <errno.h>
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "retimer_5902_dev.h"


RETIMER_REQ_DATA_HEAD_S g_reqDataHead[] = {
    
    {0x80,   0,       0x5,            0,          16},  
    {0x80,   0,       0x5,            0,          20},  
    {0x80,   0,       0x19,           0,          1},   
    {0x80,   1,       0x19,           0,          1},   
    {0x80,   0,       0x1A,           0,          6},   
    {0x0,    1,       0x18,           0,          6},   
    {0x80,   1,       0x18,           0,          6},   
};


guint8 g_initSuccessFlag[RETIMER_DEVICE_NUM] = {0};


gint32 retimer_smbus_access(OBJ_HANDLE obj_handle, guint8 read_write, guint8 reg, gulong size,
    union i2c_smbus_data *data)
{
    gint32 ret;
    OBJ_HANDLE chip_handle;

    if (data == NULL) {
        debug_log(DLOG_ERROR, "%s:in parameter error", __FUNCTION__);
        return RET_ERR;
    }

    
    ret = dfl_get_referenced_object(obj_handle, PROPERTY_RETIMER_REFCHIP, &chip_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get referentce obj (%s) failed(%d).", dfl_get_object_name(obj_handle), ret);
        return ret;
    }

    
    if (read_write == I2C_SMBUS_WRITE) {
        ret = dfl_chip_blkwrite(dfl_get_object_name(chip_handle), reg, size, data);
    } else {
        ret = dfl_chip_blkread(dfl_get_object_name(chip_handle), reg, size, data);
    }

    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s:Chip blk oprate(%d) failed(%d).", __FUNCTION__, read_write, ret);
    }

    return ret;
}


gint32 retimer_smbus_write_block(OBJ_HANDLE obj_handle, guint8 reg, gulong len, const guint8 *values)
{
    guint32 i;
    union i2c_smbus_data data;
    gint32 nRet;

    if (values == NULL) {
        debug_log(DLOG_ERROR, "%s:in parameter error", __FUNCTION__);
        return RET_ERR;
    }

    if (len > I2C_SMBUS_BLOCK_MAX) {
        len = I2C_SMBUS_BLOCK_MAX;
    }

    for (i = 1; i <= len; i++) {
        data.block[i] = values[i - 1];
    }

    data.block[0] = (guint8)len;
    nRet = retimer_smbus_access(obj_handle, I2C_SMBUS_WRITE, reg, len + 1, &data);
    if (nRet != 0) {
        debug_log(DLOG_ERROR, "%s:i2c smbus access failed:%x", __FUNCTION__, nRet);
        return RET_ERR;
    }
    return RET_OK;
}


gint32 retimer_smbus_read_block(OBJ_HANDLE obj_handle, guint8 reg, gulong len, guint8 *values)
{
    gint32 i;
    union i2c_smbus_data data;
    gint32 nRet;

    if (values == NULL) {
        debug_log(DLOG_ERROR, "%s:in parameter error", __FUNCTION__);
        return RET_ERR;
    }

    if (len > I2C_SMBUS_BLOCK_MAX + 1) {
        len = I2C_SMBUS_BLOCK_MAX;
    }

    nRet = retimer_smbus_access(obj_handle, I2C_SMBUS_READ, reg, len + 1, &data);
    if (nRet != 0) {
        debug_log(DLOG_ERROR, "%s:i2c smbus access failed:%x", __FUNCTION__, nRet);
        return RET_ERR;
    }

    if (data.block[0] > (guint8)len) {
        debug_log(DLOG_ERROR, "%s: Array out of bounds. retimer = %s. data.block[0] = %d, len = %d.", __FUNCTION__,
            dfl_get_object_name(obj_handle), data.block[0], (guint8)len);
        return RET_ERR;
    }

    for (i = 1; i <= data.block[0]; i++) {
        values[i - 1] = data.block[i];
    }
    return RET_OK;
}


gint32 retimer_get_info_by_i2c(OBJ_HANDLE obj_handle, SMBUS_REQ *req, gint32 req_data_len, gint8 *data_info,
    gint32 buf_len)
{
    gint32 ret;
    guint8 data_buf[I2C_SMBUS_BLOCK_MAX] = {0};

    if (data_info == NULL) {
        debug_log(DLOG_ERROR, "%s:in parameter error", __FUNCTION__);
        return RET_ERR;
    }

    ret = retimer_waite_for_idle(obj_handle);
    if (ret != 0) {
        debug_log(DLOG_ERROR, "%s:call retimer_waite_for_idle failed.%d.", __FUNCTION__, ret);
        return RET_ERR;
    }

    ret = retimer_smbus_write_block(obj_handle, RETIMER_CMD_CODE_RW_REQ, DMP_MSG_HEAD_LENGTH + req_data_len,
        (const guint8 *)req);
    if (ret != 0) {
        debug_log(DLOG_ERROR, "%s:call retimer_smbus_write_block failed.%d.", __FUNCTION__, ret);
        return RET_ERR;
    }

    vos_task_delay(100); 

    ret = retimer_waite_for_idle(obj_handle);
    if (ret != 0) {
        debug_log(DLOG_ERROR, "%s:call retimer_waite_for_idle failed.%d.", __FUNCTION__, ret);
        return RET_ERR;
    }

    ret = retimer_smbus_read_block(obj_handle, RETIMER_CMD_CODE_RW_RSP, DMP_MSG_HEAD_LENGTH + RETIMER_MSG_DATA_LEN,
        data_buf);
    if (ret != 0) {
        debug_log(DLOG_ERROR, "%s:call retimer_smbus_read_block failed.%d.", __FUNCTION__, ret);
        return RET_ERR;
    }

    ret = memcpy_s(data_info, buf_len, &data_buf[0],
        *(guint32 *)(void *)&data_buf[DMP_MSG_HEAD_LEN_OFFSET] + DMP_MSG_HEAD_LENGTH);
    if (ret != 0) {
        debug_log(DLOG_ERROR, "%s:memcpy_s error.%d.", __FUNCTION__, ret);
        return RET_ERR;
    }

    return RET_OK;
}


gint32 retimer_get_busy_info(OBJ_HANDLE obj_handle, guint8 reg, guint8 *values)
{
    union i2c_smbus_data data;
    gint32 nRet;

    if (values == NULL) {
        debug_log(DLOG_ERROR, "%s:in parameter error", __FUNCTION__);
        return RET_ERR;
    }

    nRet = retimer_smbus_access(obj_handle, I2C_SMBUS_READ, reg, 1, &data);
    if (nRet != 0) {
        debug_log(DLOG_DEBUG, "%s:i2c smbus access failed:%x", __FUNCTION__, nRet);
        return RET_ERR;
    }
    *values = data.block[0];

    return RET_OK;
}


gint32 retimer_waite_for_idle(OBJ_HANDLE obj_handle)
{
    gint32 ret;
    guint8 status = 0;
    gint32 num_id;
    guint8 retimer_id = 0;
    guint8 software_type = 0;

    
    (void)dal_get_software_type(&software_type);
    if (software_type == MGMT_SOFTWARE_TYPE_PANGEA_MGNT) {
        
        ret = dal_get_property_value_byte(obj_handle, PROPERTY_RETIMER_ID, &retimer_id);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "Get retimer id failed, object = %s.", dfl_get_object_name(obj_handle));
            return RET_ERR;
        }
        if (retimer_id >= RETIMER_DEVICE_NUM) {
            debug_log(DLOG_ERROR, "Get wrong retimer id, object = %s, retimerId = %d.", dfl_get_object_name(obj_handle),
                retimer_id);
            return RET_ERR;
        }
        if (g_initSuccessFlag[retimer_id] == RETIMER_INIT_FAILED) {
            vos_task_delay(10); 
            debug_log(DLOG_ERROR, "No init, object = %s.", dfl_get_object_name(obj_handle));
            return RET_OK;
        }
    }

    
    for (num_id = 0; num_id < RETIMER_MSG_REPEATE_NUM; num_id++) {
        ret = retimer_get_busy_info(obj_handle, RETIMER_CMD_CODE_GETSTAT, &status);
        if ((ret == RET_OK) && (status == RT_IDLE_STATUS)) {
            return RET_OK;
        }
        vos_task_delay(300); 
    }

    debug_log(DLOG_INFO, "%s: status(0x%x), ret(0x%x)", __FUNCTION__, status, ret);
    return RET_ERR;
}


gint32 retimer_get_info(OBJ_HANDLE obj_handle, SMBUS_REQ *req, gint32 req_data_len, gint8 *data_info, gint32 buf_len)
{
    gint32 ret;
    gint32 num_id;

    for (num_id = 0; num_id < RETIMER_MSG_REPEATE_NUM; num_id++) {
        ret = retimer_get_info_by_i2c(obj_handle, req, req_data_len, data_info, buf_len);
        if (ret == 0) {
            break;
        }
        vos_task_delay(1); 
    }

    return ret;
}


gint32 retimer_request_serdes_main_log(OBJ_HANDLE retimer)
{
    SMBUS_REQ req = { 0 };
    gint8 resp_buff[BUFFER_MAX_LEN] = {0};

    req.lun = CDR5902_REQ_LUN;
    req.opcode = RETIMER_CMD_GET_SERDES_MAIN_INFO;
    req.data[0] = 0;
    req.data[1] = RETIMER_CHANNEL_NUM;

    return retimer_get_info(retimer, &req, 2, resp_buff, BUFFER_MAX_LEN); 
}


gint32 retimer_set_tx_ffe_5902(OBJ_HANDLE obj_handle, SET_TX_FFE_DEBUG *tx_ffe, gint8 *data_info, gint32 buf_len)
{
    SMBUS_REQ req = { 0 };
    gint32 data_start = SET_TX_FFE_DATA_START;

    req.lun = SMBUS_I2C_LUN_VALUE;
    req.arg = SMBUS_I2C_ARG_VALUE;
    req.opcode = SET_TX_FFE_OPCODE;
    req.offset = REQUEST_OFFSET;
    req.length = 12; 

    req.data[0] = tx_ffe->channel_id;
    req.data[1] = 1; 
    *(gint16 *)(void *)&req.data[data_start] = tx_ffe->pre2;
    data_start += sizeof(guint16);
    *(gint16 *)(void *)&req.data[data_start] = tx_ffe->pre1;
    data_start += sizeof(guint16);
    *(gint16 *)(void *)&req.data[data_start] = tx_ffe->main0;
    data_start += sizeof(guint16);
    *(gint16 *)(void *)&req.data[data_start] = tx_ffe->post1;
    data_start += sizeof(guint16);
    *(gint16 *)(void *)&req.data[data_start] = tx_ffe->post2;

    return retimer_get_info(obj_handle, &req, 12, data_info, buf_len); 
}


gint32 retimer_set_rx_ctle_5902(OBJ_HANDLE obj_handle, SET_RX_CTLE_DEBUG *rx_ctle, gint8 *data_info, gint32 buf_len)
{
    SMBUS_REQ req = { 0 };
    gint32 data_start = SET_RX_CTLE_DATA_START;

    req.lun = SMBUS_I2C_LUN_VALUE;
    req.arg = SMBUS_I2C_ARG_VALUE;
    req.opcode = SET_RX_CTLE_OPCODE;
    req.offset = REQUEST_OFFSET;
    req.length = 14; 

    req.data[0] = rx_ctle->channel_id;
    req.data[1] = 1; 
    req.data[data_start] = (gint8)(rx_ctle->enable);
    data_start++;
    req.data[data_start] = (gint8)((rx_ctle->enable) >> SET_RX_CTLE_CHAR_OFFSET);
    data_start++;

    req.data[data_start] = (gint8)(rx_ctle->gain);
    data_start++;
    req.data[data_start] = (gint8)((rx_ctle->gain) >> SET_RX_CTLE_CHAR_OFFSET);
    data_start++;
    req.data[data_start] = (gint8)((rx_ctle->gain) >> SET_RX_CTLE_SHORT_OFFSET);
    data_start++;

    req.data[data_start] = (gint8)(rx_ctle->boost);
    data_start++;
    req.data[data_start] = (gint8)((rx_ctle->boost) >> SET_RX_CTLE_CHAR_OFFSET);
    data_start++;
    req.data[data_start] = (gint8)((rx_ctle->boost) >> SET_RX_CTLE_SHORT_OFFSET);
    data_start++;

    req.data[data_start] = (gint8)(rx_ctle->squelch);
    data_start++;
    req.data[data_start] = (gint8)((rx_ctle->squelch) >> SET_RX_CTLE_CHAR_OFFSET);
    data_start++;

    req.data[data_start] = (gint8)(rx_ctle->zero);
    data_start++;
    req.data[data_start] = (gint8)((rx_ctle->zero) >> SET_RX_CTLE_CHAR_OFFSET);

    return retimer_get_info(obj_handle, &req, 14, data_info, buf_len); 
}


gint32 retimer_set_reg_info_5902(OBJ_HANDLE obj_handle, guint32 regAddr, guint32 data_info)
{
    SMBUS_REQ req = { 0 };
    gint32 ret;
    gint32 num_id;

    req.lun = SMBUS_I2C_LUN_VALUE;
    req.arg = SMBUS_I2C_ARG_VALUE;
    req.opcode = SET_REG_OPCODE;
    req.offset = regAddr;
    req.length = 4; 

    ret = memcpy_s(&req.data[0], sizeof(guint32), &data_info, sizeof(guint32));
    if (ret != 0) {
        debug_log(DLOG_ERROR, "%s:call memcpy_s failed.%d.", __FUNCTION__, ret);
        return RET_ERR;
    }

    for (num_id = 0; num_id < RETIMER_MSG_REPEATE_NUM; num_id++) {
        ret = retimer_waite_for_idle(obj_handle);
        if (ret != 0) {
            vos_task_delay(1); 
            continue;
        }

        ret = retimer_smbus_write_block(obj_handle, RETIMER_CMD_CODE_RW_REQ,
                                        DMP_MSG_HEAD_LENGTH + 4, (const guint8*)&req); 
        if (ret == 0) {
            return RET_OK;
        }
        vos_task_delay(1); 
    }

    debug_log(DLOG_ERROR, "%s:call failed.%d.", __FUNCTION__, ret);
    return ret;
}


gint32 retimer_get_tx_ffe_5902(OBJ_HANDLE obj_handle, guint8 channel_id, gint8 *data_info, gint32 buf_len)
{
    SMBUS_REQ req = { 0 };

    req.lun = SMBUS_I2C_LUN_VALUE;
    req.arg = SMBUS_I2C_ARG_VALUE;
    req.opcode = GET_TX_FFE_OPCODE;
    req.offset = REQUEST_OFFSET;
    req.length = 2; 

    req.data[0] = channel_id;
    req.data[1] = 1; 

    return retimer_get_info(obj_handle, &req, 2, data_info, buf_len); 
}


gint32 retimer_get_rx_ctle_5902(OBJ_HANDLE obj_handle, guint8 channel_id, gint8 *data_info, gint32 buf_len)
{
    SMBUS_REQ req = { 0 };
    req.lun = SMBUS_I2C_LUN_VALUE;
    req.arg = SMBUS_I2C_ARG_VALUE;
    req.opcode = GET_RX_CTLE_OPCODE;
    req.offset = REQUEST_OFFSET;
    req.length = 2; 
    req.data[0] = channel_id;
    req.data[1] = 1; 

    return retimer_get_info(obj_handle, &req, 2, data_info, buf_len); 
}


gint32 retimer_get_eye_diag_5902(OBJ_HANDLE obj_handle, guint8 channel_id, gint8 *data_info, gint32 buf_len)
{
    SMBUS_REQ req = { 0 };

    req.lun = SMBUS_I2C_LUN_VALUE;
    req.arg = SMBUS_I2C_ARG_VALUE;
    req.opcode = GET_EYE_DIAG_OPCODE;
    req.offset = REQUEST_OFFSET;
    req.length = 2; 

    req.data[0] = channel_id;
    req.data[1] = 1; 

    return retimer_get_info(obj_handle, &req, 2, data_info, buf_len); 
}


gint32 retimer_get_reg_info_5902(OBJ_HANDLE obj_handle, guint32 regAddr, gint8 *data_info, gint32 buf_len)
{
    SMBUS_REQ req = { 0 };

    req.lun = SMBUS_I2C_LUN_VALUE;
    req.arg = SMBUS_I2C_ARG_VALUE;
    req.opcode = GET_REG_OPCODE;
    req.offset = regAddr;
    req.length = 4; 

    return retimer_get_info(obj_handle, &req, 0, data_info, buf_len); 
}


gint32 retimer_switch_channel(OBJ_HANDLE obj_handle, guint32 flag)
{
    gint32 ret;
    gint32 ret_req = RET_ERR;
    GSList *input_list = NULL;
    if (flag == RETIMER_CHN_WITCH_OPEN) {
        ret_req = dal_set_property_value_byte(obj_handle, PROPERTY_RETIMER_REQ_ACC, RETIMER_BUSY, DF_AUTO);
        vos_task_delay(NOTIFY_DELAY_TIME);
    }
    input_list = g_slist_append(input_list, g_variant_new_byte(flag));
    ret = dfl_call_class_method(obj_handle, METHOD_RETIMER_CHANNEL_SWITCH, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    input_list = NULL;
    if (flag == RETIMER_CHN_WITCH_CLOSE) {
        ret_req = dal_set_property_value_byte(obj_handle, PROPERTY_RETIMER_REQ_ACC, RETIMER_IDLE, DF_AUTO);
    }
    if (ret != RET_OK || ret_req != RET_OK) {
        debug_log(DLOG_ERROR, "%s:retimer_switch_channel fail, ret=%d, ret_req=%d.", __FUNCTION__, ret, ret_req);
    }
    return ret;
}


gint32 retimer_get_waite_flag_5902(OBJ_HANDLE obj_handle, gint8 *data_info, gint32 buf_len)
{
    gint32 ret;
    guint8 status = 0;
    gint32 num_id;

    for (num_id = 0; num_id < RETIMER_MSG_REPEATE_NUM; num_id++) {
        ret = retimer_get_busy_info(obj_handle, RETIMER_CMD_CODE_GETSTAT, &status);
        if ((ret == 0) && (status == RT_IDLE_STATUS)) {
            *(gint32 *)(void *)data_info = (gint32)status;
            return RET_OK;
        }
        vos_task_delay(100); 
    }

    debug_log(DLOG_ERROR, "status(0x%x), ret(0x%x)", status, ret);
    return RET_ERR;
}


LOCAL gint32 retimer_trans_fill_single_frame(guint8 *mem_buf, guint32 buf_len, gboolean is_last_frame, guint32 frame_id,
    RETIMER_DRV_INTF_S *write_data)
{
    gint32 ret;
    guchar header_table_id;
    guint32 valid_data_len;
    guint32 offset;

    
    
    if (is_last_frame == TRUE) {
        header_table_id = SEND_LAST_FIRMWARE_FRAME_HEAD;
        valid_data_len = buf_len - frame_id * CDR5902_SINGLE_DATA_MAX_LEN;
    } else {
        header_table_id = SEND_FIRMWARE_FRAME_HEAD;
        valid_data_len = CDR5902_SINGLE_DATA_MAX_LEN;
    }
    
    g_reqDataHead[header_table_id].length = valid_data_len;
    write_data->data_len = valid_data_len + RETIMER_REQ_LEN_ADD;

    
    offset = frame_id * CDR5902_SINGLE_DATA_MAX_LEN;

    write_data->data[0] = write_data->data_len - 1;
    ret = memcpy_s(write_data->data + 1, RETIMER_REQ_LEN_ADD + CDR5902_SINGLE_DATA_MAX_LEN - 1,
        &g_reqDataHead[header_table_id], RETIMER_REQ_LEN);
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "Safty func memcpy_s failed, ret: %d.", ret);
        return RET_ERR;
    }
    ret = memcpy_s(write_data->data + RETIMER_REQ_LEN_ADD, write_data->data_len - RETIMER_REQ_LEN_ADD, mem_buf + offset,
        valid_data_len);
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "Safty func memcpy_s failed, ret: %d.", ret);
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 retimer_trans_single_frame(OBJ_HANDLE obj_handle, guint8 *mem_buf, guint32 buf_len)
{
    gint32 ret;
    guint32 max_frame_num;
    guint8 *single_frame_buf = NULL;
    guint32 single_frame_len;
    guint32 frame_count;
    RETIMER_DRV_INTF_S write_data;
    gboolean is_last_frame;

    
    
    max_frame_num = (guint32)ceil((gdouble)buf_len / CDR5902_SINGLE_DATA_MAX_LEN);
    single_frame_len = RETIMER_REQ_LEN_ADD + CDR5902_SINGLE_DATA_MAX_LEN;
    single_frame_buf = (guint8 *)g_malloc0(single_frame_len);
    if (single_frame_buf == NULL) {
        return RET_ERR;
    }

    
    write_data.handle = obj_handle;
    write_data.cmd = RETIMER_CMD_CODE_RW_REQ;
    write_data.data = (guchar *)single_frame_buf;
    // data_len
    write_data.retry_count = 0;
    for (frame_count = 0; frame_count < max_frame_num; frame_count++) {
        
        (void)memset_s(single_frame_buf, single_frame_len, 0, single_frame_len);
        is_last_frame = (frame_count == max_frame_num - 1) ? TRUE : FALSE;
        ret = retimer_trans_fill_single_frame(mem_buf, buf_len, is_last_frame, frame_count, &write_data);
        if (ret != RET_OK) {
            g_free(single_frame_buf);
            return ret;
        }

        
        ret = retimer_waite_for_idle(obj_handle);
        if (ret == RET_OK) {
            vos_task_delay(DELAY_BETWEEN_TWO_REQ);
            ret = retimer_write_i2c_data(&write_data);
        }

        
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Retimer(%s) send frame(%d) fail(%d).", dfl_get_object_name(obj_handle), frame_count,
                ret);
            g_free(single_frame_buf);
            return ret;
        }
    }

    g_free(single_frame_buf);
    return ret;
}


LOCAL gint32 retimer_trans_file(OBJ_HANDLE obj_handle)
{
    gint32 ret;
    guint32 file_len;
    guint8 *mem_buf = NULL;
    FILE *fp = NULL;

    
    if (!vos_get_file_accessible((const gchar *)RETIMER_INIT_FILE_PATH)) {
        debug_log(DLOG_ERROR, "Retimer init file is not exist.");
        return RET_ERR;
    }
    file_len = vos_get_file_length((const gchar *)RETIMER_INIT_FILE_PATH);
    if (file_len <= 0) {
        debug_log(DLOG_ERROR, "Retimer init file len is 0.");
        return RET_ERR;
    }
    fp = fopen((const gchar *)RETIMER_INIT_FILE_PATH, "rb");
    if (fp == NULL) {
        debug_log(DLOG_ERROR, "Open file failed.");
        return RET_ERR;
    }

    
    mem_buf = (guint8 *)g_malloc0(file_len);
    if (mem_buf == NULL) {
        (void)fclose(fp);
        return RET_ERR;
    }
    if (fread((void *)mem_buf, sizeof(guint8), file_len, fp) != file_len) {
        g_free(mem_buf);
        (void)fclose(fp);
        return RET_ERR;
    }
    (void)fclose(fp);

    ret = retimer_trans_single_frame(obj_handle, mem_buf, file_len);

    
    g_free(mem_buf);
    return ret;
}


LOCAL gint32 retimer_registor_read_result(OBJ_HANDLE obj_handle, guint32 regAddr, guint32 check_val, guint32 check_num)
{
    gint8 data_info[BUFFER_MAX_LEN] = {};

    for (guint32 i = 0; i < check_num; i++) {
        gint32 ret = retimer_get_reg_info_5902(obj_handle, regAddr, data_info, BUFFER_MAX_LEN);
        if (ret == RET_OK) {
            guint32 reg_value = *(guint32 *)(void *)&data_info[SMBUS_READ_DATA_OFFSET];
            if (reg_value == check_val) {
                return RET_OK;
            } else {
                vos_task_delay(2); 
            }
        } else {
            debug_log(DLOG_ERROR, "Get registor value failed, object = %s.", dfl_get_object_name(obj_handle));
            return RET_ERR;
        }
    }
    return RET_ERR;
}


LOCAL gint32 retimer_reset_5902(OBJ_HANDLE obj_handle)
{
    gint32 ret;

    ret = dal_set_property_value_byte(obj_handle, PROPERTY_RETIMER_I2C_RESET, 0, DF_HW);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Reset retimer %s failed.", dfl_get_object_name(obj_handle));
        return RET_ERR;
    }

    vos_task_delay(10); 
    ret = dal_set_property_value_byte(obj_handle, PROPERTY_RETIMER_I2C_RESET, 1, DF_HW);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Reset retimer %s failed.", dfl_get_object_name(obj_handle));
        return RET_ERR;
    }

    return RET_OK;
}


gint32 retimer_get_temperature_5902(OBJ_HANDLE obj_handle, gint16 *temp_data)
{
    SMBUS_REQ req = { 0 };
    gint32 ret;
    gint8 tmp_data_info[BUFFER_MAX_LEN] = {};
    gint16 result;

    req.lun = SMBUS_I2C_LUN_VALUE;
    req.arg = SMBUS_I2C_ARG_VALUE;
    req.opcode = GET_TEMPERATURE_OPCODE;
    req.offset = REQUEST_OFFSET;
    req.length = 2; 

    ret = retimer_get_info(obj_handle, &req, 0, tmp_data_info, BUFFER_MAX_LEN); 
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get info failed, object = %s.", dfl_get_object_name(obj_handle));
        return RET_ERR;
    }

    result = *(gint16 *)(void *)&tmp_data_info[SMBUS_READ_ERROR_CODE_OFFSET];
    if (result != 0) {
        debug_log(DLOG_ERROR, "Get temperature failed, object = %s, result = %d.", dfl_get_object_name(obj_handle),
            result);
        return RET_ERR;
    }

    
    *temp_data = *(gint16 *)(void *)&tmp_data_info[SMBUS_READ_DATA_OFFSET];
    debug_log(DLOG_INFO, "object = %s, temp_data = %d.", dfl_get_object_name(obj_handle), *temp_data);
    return RET_OK;
}


gint32 retimer_cdr_init(OBJ_HANDLE obj_handle)
{
    gint32 ret;
    guint8 retimer_id = 0;

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_RETIMER_ID, &retimer_id);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get retimer id failed, object = %s.", dfl_get_object_name(obj_handle));
        return RET_ERR;
    }
    if (retimer_id >= RETIMER_DEVICE_NUM) {
        debug_log(DLOG_ERROR, "Get wrong retimer id, object = %s, retimerId = %d.", dfl_get_object_name(obj_handle),
            retimer_id);
        return RET_ERR;
    }
    g_initSuccessFlag[retimer_id] = RETIMER_INIT_FAILED;

    
    ret = retimer_reset_5902(obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Reset 5902 failed, object = %s.", dfl_get_object_name(obj_handle));
        return RET_ERR;
    }
    vos_task_delay(5); 

    
    ret = retimer_registor_read_result(obj_handle, 0x28000030, 0x1, 150); 
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Read 0x28000030 failed, object = %s.", dfl_get_object_name(obj_handle));
        return RET_ERR;
    }

    
    ret = retimer_trans_file(obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Transfor file failed, object = %s.", dfl_get_object_name(obj_handle));
        return RET_ERR;
    }
    vos_task_delay(5); 

    
    ret = retimer_registor_read_result(obj_handle, 0x28000F20, 0x55, 150); 
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Read 0x28000F20 failed, object = %s.", dfl_get_object_name(obj_handle));
        return RET_ERR;
    }
    vos_task_delay(5); 

    
    ret = retimer_set_reg_info_5902(obj_handle, 0x28000034, 0x1);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set 0x28000034 failed, object = %s.", dfl_get_object_name(obj_handle));
        return RET_ERR;
    }
    vos_task_delay(200); 

    
    ret = retimer_registor_read_result(obj_handle, 0x28000038, 0x1, 150); 

    
    g_initSuccessFlag[retimer_id] = RETIMER_INIT_SUCCESS;

    return ret;
}


gint32 retimer_get_heartbeat_5902(OBJ_HANDLE obj_handle)
{
    gint32 ret;
    gint8 data_info[BUFFER_MAX_LEN] = {};
    guint16 error_code;
    SMBUS_REQ req = { 0 };
    guint8 retimer_id = 0;

    req.lun = SMBUS_I2C_LUN_VALUE;
    req.arg = SMBUS_I2C_ARG_VALUE;
    req.opcode = GET_HEART_OPCODE;
    req.offset = REQUEST_OFFSET;
    req.length = 4; 

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_RETIMER_ID, &retimer_id);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get retimer id failed, object = %s.", dfl_get_object_name(obj_handle));
        return RET_ERR;
    }
    if (retimer_id >= RETIMER_DEVICE_NUM) {
        debug_log(DLOG_ERROR, "Get wrong retimer id, object = %s, retimerId = %d.", dfl_get_object_name(obj_handle),
            retimer_id);
        return RET_ERR;
    }

    ret = retimer_get_info(obj_handle, &req, 0, data_info, BUFFER_MAX_LEN); 
    if (ret == RET_OK) {
        error_code = *(guint16 *)(void *)&data_info[SMBUS_READ_ERROR_CODE_OFFSET];
        if (error_code == 0x0) {
            g_initSuccessFlag[retimer_id] = RETIMER_INIT_SUCCESS;
            return RET_OK;
        }
    }

    g_initSuccessFlag[retimer_id] = RETIMER_INIT_FAILED;
    return RET_ERR;
}


gint32 retimer_set_speed(OBJ_HANDLE obj_handle, guint8 channel_id, guint8 mode, guint8 speed)
{
    gint32 ret;
    gint8 data_info[BUFFER_MAX_LEN] = {};
    guint16 result;
    SMBUS_REQ req = { 0 };

    req.lun = SMBUS_I2C_LUN_VALUE;
    req.arg = SMBUS_I2C_ARG_VALUE;
    req.opcode = SWITCH_SPEED_OPCODE;
    req.offset = REQUEST_OFFSET;
    req.length = 3; 

    req.data[0] = channel_id; 
    req.data[1] = mode;       
    req.data[2] = speed;      

    ret = retimer_get_info(obj_handle, &req, 3, data_info, BUFFER_MAX_LEN); 
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get info failed, object = %s.", dfl_get_object_name(obj_handle));
        return RET_ERR;
    }

    result = *(guint16 *)(void *)&data_info[SMBUS_READ_DATA_OFFSET];
    if (result != 0) {
        debug_log(DLOG_ERROR, "Set speed failed, object = %s.", dfl_get_object_name(obj_handle));
        return RET_ERR;
    }

    return RET_OK;
}


gint32 retimer_serdes_adapt(OBJ_HANDLE obj_handle, guint8 channel_id, guint8 lane_num)
{
    gint32 ret;
    gint8 data_info[BUFFER_MAX_LEN] = {};
    guint16 result;
    SMBUS_REQ req = { 0 };

    req.lun = SMBUS_I2C_LUN_VALUE;
    req.arg = SMBUS_I2C_ARG_VALUE;
    req.opcode = SERDES_ADAPT_OPCODE;
    req.offset = REQUEST_OFFSET;
    req.length = 2; 

    req.data[0] = channel_id; 
    req.data[1] = lane_num;   

    ret = retimer_get_info(obj_handle, &req, 2, data_info, BUFFER_MAX_LEN); 
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get info failed, object = %s.", dfl_get_object_name(obj_handle));
        return RET_ERR;
    }

    result = *(guint16 *)(void *)&data_info[SMBUS_READ_DATA_OFFSET];
    if (result != 0) {
        debug_log(DLOG_ERROR, "Serdes adapt failed, object = %s, result = 0x%X.", dfl_get_object_name(obj_handle),
            result);
        return RET_ERR;
    }

    return RET_OK;
}


gint32 retimer_prbs_send_recv_5902(OBJ_HANDLE obj_handle, PRBS_SEND_RECV *prbs)
{
    gint32 ret;
    gint8 data_info[BUFFER_MAX_LEN] = {};
    guint16 result;
    SMBUS_REQ req = { 0 };

    req.lun = SMBUS_I2C_LUN_VALUE;
    req.arg = SMBUS_I2C_ARG_VALUE;
    req.opcode = PRBS_TX_RX_OPCODE;
    req.offset = REQUEST_OFFSET;
    req.length = 4; 

    req.data[0] = prbs->channel_id; 
    req.data[1] = prbs->lane_num;   
    req.data[2] = prbs->prbs_type;  
    req.data[3] = prbs->prbs_dir;   

    ret = retimer_get_info(obj_handle, &req, 4, data_info, BUFFER_MAX_LEN); 
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get info failed, object = %s.", dfl_get_object_name(obj_handle));
        return RET_ERR;
    }

    result = *(guint16 *)(void *)&data_info[SMBUS_READ_DATA_OFFSET];
    if (result != 0) {
        debug_log(DLOG_ERROR, "Set prbs failed, object = %s, result = 0x%X.", dfl_get_object_name(obj_handle), result);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 retimer_get_lane_result(OBJ_HANDLE obj_handle, guint8 channel_id, guint8 lane_num, guint16 result)
{
    guint8 j;

    for (j = channel_id; j < (channel_id + lane_num); j++) {
        if (result & (0x1 << j)) { 
            debug_log(DLOG_ERROR, "lane (%d) failed. object = %s. ", j, dfl_get_object_name(obj_handle));
            break;
        }
        debug_log(DLOG_ERROR, "lane (%d) succeed. ", j);
    }

    if (j != (channel_id + lane_num)) {
        return RET_ERR;
    } else { 
        return RET_OK;
    }
}


LOCAL gint32 retimer_get_lane_error_cnt(gint8 *data_info, gint32 buf_len, gint8 *tmp_data_info, gint32 len,
    gint32 channel_id)
{
    errno_t safe_fun_ret;
    gint32 i;

    if (buf_len < len) {
        debug_log(DLOG_ERROR, "Buffer parameter is too short.");
    }
    safe_fun_ret = memcpy_s(data_info, buf_len, tmp_data_info, len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "Memcpy_s failed when getting prbs error.");
        return RET_ERR;
    }

    for (i = 0; i < (len / (gint32)sizeof(gint32)); i++) {
        debug_log(DLOG_ERROR, "Successfully get lane(%d) error_cnt = 0x%X.", i + channel_id,
            *(guint32 *)(void *)&data_info[i * sizeof(guint32)]);
    }
    return RET_OK;
}


gint32 retimer_get_prbs_error_5902(OBJ_HANDLE obj_handle, PRBS_ERROR_CHECK *prbs, gint8 *data_info, gint32 buf_len)
{
    gint32 ret;
    gint8 tmp_data_info[BUFFER_MAX_LEN] = {};
    SMBUS_REQ req = { 0 };
    guint16 result;
    guint8 i;

    req.lun = SMBUS_I2C_LUN_VALUE;
    req.arg = SMBUS_I2C_ARG_VALUE;
    req.opcode = CHECK_PRBS_ERR_OPCODE;
    req.offset = REQUEST_OFFSET;
    req.length = 2; 

    req.data[0] = prbs->channel_id; 
    req.data[1] = prbs->lane_num;   

    for (i = 0; i < 3; i++) { 
        ret = retimer_get_info(obj_handle, &req, 2, tmp_data_info, BUFFER_MAX_LEN); 
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Get info failed, object = %s. i = %d", dfl_get_object_name(obj_handle), i);
            continue;
        }

        result = *(guint16 *)(void *)&tmp_data_info[SMBUS_READ_DATA_OFFSET]; 
        debug_log(DLOG_ERROR, "%s get result. result = 0x%X.", dfl_get_object_name(obj_handle), result);
        
        ret = retimer_get_lane_result(obj_handle, prbs->channel_id, prbs->lane_num, result);
        if (ret != RET_OK) {
            continue;
        } else { 
            break;
        }
    }
    
    if (i == 3) {
        debug_log(DLOG_ERROR, "Get info failed, object = %s. i = %d", dfl_get_object_name(obj_handle), i);
        return RET_ERR;
    }
    debug_log(DLOG_ERROR, "%s get error code succeed.", dfl_get_object_name(obj_handle));

    
    return retimer_get_lane_error_cnt(data_info, buf_len, &tmp_data_info[SMBUS_READ_PRBS_ERROR_CNT_OFFSET],
        (gint32)(sizeof(gint32) * prbs->lane_num), (gint32)(prbs->channel_id));
}


gint32 retimer_set_tx_swing_5902(OBJ_HANDLE obj_handle, SET_TX_SWING *tx_swing)
{
    gint32 ret;
    gint8 data_info[BUFFER_MAX_LEN] = {};
    guint16 result;
    SMBUS_REQ req = { 0 };

    req.lun = SMBUS_I2C_LUN_VALUE;
    req.arg = SMBUS_I2C_ARG_VALUE;
    req.opcode = SET_TX_SWING_OPCODE;
    req.offset = REQUEST_OFFSET;
    req.length = 3; 

    req.data[0] = tx_swing->channel_id; 
    req.data[1] = tx_swing->lane_num;   
    req.data[2] = tx_swing->swing;      

    ret = retimer_get_info(obj_handle, &req, 3, data_info, BUFFER_MAX_LEN); 
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get info failed, object = %s.", dfl_get_object_name(obj_handle));
        return RET_ERR;
    }

    result = *(guint16 *)(void *)&data_info[SMBUS_READ_DATA_OFFSET];
    if (result != 0) {
        debug_log(DLOG_ERROR, "%s set tx swing failed. result = 0x%X.", dfl_get_object_name(obj_handle), result);
        return RET_ERR;
    }

    return RET_OK;
}


gint32 retimer_get_version(OBJ_HANDLE obj_handle, gint8 *data_info, gint32 buf_len)
{
    errno_t safe_fun_ret;
    gint32 ret;
    guint8 chiptype = 0;
    gint8 tmp_data_info[BUFFER_MAX_LEN] = {};
    guint16 error_code;
    SMBUS_REQ req = { 0 };

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_RETIMER_CHIP_TYPE, &chiptype); 

    req.lun = SMBUS_I2C_LUN_VALUE;
    req.arg = SMBUS_I2C_ARG_VALUE;
    req.opcode = GET_FW_VERSION_OPCODE;
    req.offset = REQUEST_OFFSET;
    
    req.length = ((chiptype == RETIMER_5902_TYPE_L) ? RETIMER_5902_CHIP_L : RETIMER_5902_CHIP_H);

    ret = retimer_get_info(obj_handle, &req, 0, tmp_data_info, BUFFER_MAX_LEN); 
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get info failed, object = %s.", dfl_get_object_name(obj_handle));
        return RETIMER_ACCESS_FAIL;
    }

    error_code = *(guint16 *)(void *)&tmp_data_info[SMBUS_READ_ERROR_CODE_OFFSET];
    if (error_code != 0) {
        debug_log(DLOG_ERROR, "Get info failed, object = %s.", dfl_get_object_name(obj_handle));
        return RET_ERR;
    }

    
    if (buf_len < 20) { 
        debug_log(DLOG_ERROR, "Buffer parameter is too short.");
        return RET_ERR;
    }
    safe_fun_ret = memcpy_s(data_info, buf_len, &tmp_data_info[SMBUS_READ_DATA_OFFSET], req.length);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "Memcpy_s failed when getting version.");
        return RET_ERR;
    }

    return RET_OK;
}


gint32 retimer_check_lane_loss(OBJ_HANDLE obj_handle)
{
    gint8 i;
    gint32 ret;
    guint32 reg_value;
    gint8 data_info[BUFFER_MAX_LEN] = {};
    guint32 tmp_addr[RETIMER_LANE_NUM] = {
        0x220042f0, 0x220046f0, 0x22004af0, 0x22004ef0,
        0x230042f0, 0x230046f0, 0x23004af0, 0x23004ef0
    };

    for (i = 0; i < RETIMER_LANE_NUM; i++) { 
        ret = retimer_get_reg_info_5902(obj_handle, tmp_addr[i], data_info, BUFFER_MAX_LEN);
        if (ret == RET_OK) {
            reg_value = *(guint32 *)(void *)&data_info[SMBUS_READ_DATA_OFFSET];
            if ((reg_value & 0x80) == 0x00) { 
                vos_task_delay(1);            
                continue;
            }
            debug_log(DLOG_INFO, "Get lane[%d] status is loss. object = %s.", i, dfl_get_object_name(obj_handle));
            return RET_ERR;
        }
        debug_log(DLOG_ERROR, "Get registor value failed, object = %s, lane[%d].", dfl_get_object_name(obj_handle), i);
        return RET_ERR;
    }

    return RET_OK;
}


gint32 retimer_set_host_time(OBJ_HANDLE obj_handle)
{
    struct timeval now;
    SMBUS_REQ req = { 0 };
    gint32 ret;
    gint8 tmp_data_info[BUFFER_MAX_LEN] = {};
    gint16 error_code;

    req.lun = SMBUS_I2C_LUN_VALUE;
    req.arg = SMBUS_I2C_ARG_VALUE;
    req.opcode = SET_HOST_TIME_OPCODE;
    req.offset = REQUEST_OFFSET;
    req.length = 8; 

    gettimeofday(&now, NULL);
    *(time_t *)(void *)(req.data) = now.tv_sec;

    ret = retimer_get_info(obj_handle, &req, 8, tmp_data_info, BUFFER_MAX_LEN); 
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get info failed, object = %s.", dfl_get_object_name(obj_handle));
        return RET_ERR;
    }

    error_code = *(gint16 *)(void *)&tmp_data_info[SMBUS_READ_ERROR_CODE_OFFSET];
    if (error_code != 0) {
        debug_log(DLOG_ERROR, "%s set host time failed. error_code = %d.", dfl_get_object_name(obj_handle), error_code);
        return RET_ERR;
    } else if (tmp_data_info[SMBUS_READ_DATA_OFFSET] == 0) {
        debug_log(DLOG_ERROR, "%s get info success, but set host time failed.", dfl_get_object_name(obj_handle));
        return RET_ERR;
    } else {
        return RET_OK;
    }
}


gint32 retimer_set_prbs_register(OBJ_HANDLE obj_handle, guint8 channel_id, guint8 lane_num)
{
    guint8 i;
    gint32 ret;

    guint32 tmp_addr_0[RETIMER_LANE_NUM] = {
        0x2200431c, 0x2200471c, 0x22004b1c, 0x22004f1c,
        0x2300431c, 0x2300471c, 0x23004b1c, 0x23004f1c
    };
    guint32 tmp_addr_1[RETIMER_LANE_NUM] = {
        0x22004330, 0x22004730, 0x22004b30, 0x22004f30,
        0x23004330, 0x23004730, 0x23004b30, 0x23004f30
    };

    if ((channel_id + lane_num) > RETIMER_LANE_NUM) {
        debug_log(DLOG_ERROR, "Array out of bounds when set prbs register.");
        return RET_ERR;
    }

    for (i = 0; i < lane_num; i++) {
        ret = retimer_set_reg_info_5902(obj_handle, tmp_addr_0[i + channel_id],
            0x800d9); 
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Set registor value failed. object = %s, register[%x].",
                dfl_get_object_name(obj_handle), tmp_addr_0[i]);
            return RET_ERR;
        }
    }
    for (i = 0; i < lane_num; i++) {
        ret = retimer_set_reg_info_5902(obj_handle, tmp_addr_1[i + channel_id],
            0x1027a); 
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Set registor value failed. object = %s, register[%x].",
                dfl_get_object_name(obj_handle), tmp_addr_0[i]);
            return RET_ERR;
        }
    }

    return RET_OK;
}


LOCAL gint32 retimer_set_optical_module_power(OBJ_HANDLE optical_handle, guint8 on_off)
{
    gint32 ret;
    GSList *input_list = NULL;

    input_list = g_slist_append(input_list, g_variant_new_byte(on_off));

    ret = dfl_call_class_method(optical_handle, METHOD_OPT_MDL_SET_POWER_STATUS, NULL, input_list, NULL);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s] call method set optical module power on or off failed(%d). on_off = %d.",
            dfl_get_object_name(optical_handle), ret, on_off);
    }

    g_slist_free(input_list);
    input_list = NULL;
    return ret;
}


gint32 retimer_prbs_optical_module_pwr(OBJ_HANDLE obj_handle)
{
    gint32 ret;
    OBJ_HANDLE optical_handle;

    
    ret = dfl_get_referenced_object(obj_handle, PROPERTY_RETIMER_REFOPTICAL, &optical_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get referentce obj (%s) failed(%d).", dfl_get_object_name(obj_handle), ret);
        return ret;
    }

    
    ret = retimer_set_optical_module_power(optical_handle, 0x0);
    if (ret != DFL_OK) {
        return ret;
    }
    vos_task_delay(1000); 

    
    ret = retimer_set_optical_module_power(optical_handle, 0x1);
    if (ret != DFL_OK) {
        return ret;
    }
    vos_task_delay(4000); 

    return RET_OK;
}


void retimer_stop_adapt(void)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    OBJ_HANDLE obj_handle;
    guint8 i;
    guint32 tmp_addr[RETIMER_LANE_NUM] = {
        0x22025fa4, 0x22025f6c, 0x22025f34, 0x22025efc,
        0x23025fa4, 0x23025f6c, 0x23025f34, 0x23025efc
    };

    ret = dfl_get_object_list(CLASS_RETIMER_NAME, &obj_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "dfl_get_object_list failed(ret: %d)", ret);
        return;
    }

    for (obj_node = obj_list; obj_node != NULL; obj_node = obj_node->next) {
        obj_handle = (OBJ_HANDLE)obj_node->data;

        for (i = 0; i < RETIMER_LANE_NUM; i++) {
            ret = retimer_set_reg_info_5902(obj_handle, tmp_addr[i], 0x0);
            if (ret != RET_OK) {
                debug_log(DLOG_DEBUG, "Set register failed(ret: %d)", ret);
                g_slist_free(obj_list);
                return;
            }
        }
    }

    g_slist_free(obj_list);
}


LOCAL void retimer_clear_chip_health(OBJ_HANDLE obj_handle)
{
    gint32 num_id;
    SMBUS_REQ req = { 0 };
    gint32 ret;

    req.lun = SMBUS_I2C_LUN_VALUE;
    req.arg = 0x1; 
    req.opcode = GET_HEALTH_OPCODE;
    req.offset = REQUEST_OFFSET;
    req.length = 0x0; 

    for (num_id = 0; num_id < RETIMER_MSG_REPEATE_NUM; num_id++) {
        ret = retimer_waite_for_idle(obj_handle);
        if (ret != RET_OK) {
            vos_task_delay(1); 
            continue;
        }

        ret = retimer_smbus_write_block(obj_handle, RETIMER_CMD_CODE_RW_REQ, DMP_MSG_HEAD_LENGTH + 0,
            (const guint8 *)&req); 
        if (ret == RET_OK) {
            return;
        }
        vos_task_delay(1); 
    }

    debug_log(DLOG_ERROR, "%s clear chip health failed.", dfl_get_object_name(obj_handle));
    return;
}


LOCAL gint32 retimer_get_chip_health(OBJ_HANDLE obj_handle)
{
    SMBUS_REQ req = { 0 };
    gint32 ret;
    gint8 tmp_data_info[BUFFER_MAX_LEN] = {0};
    gint16 error_code;
    gint32 health_state = RET_OK;

    req.lun = SMBUS_I2C_LUN_VALUE;
    req.arg = 0x0; 
    req.opcode = GET_HEALTH_OPCODE;
    req.offset = REQUEST_OFFSET;
    req.length = 0x1; 

    
    ret = retimer_get_info(obj_handle, &req, 0, tmp_data_info, BUFFER_MAX_LEN); 
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get info failed when get chip health, object = %s.", dfl_get_object_name(obj_handle));
        return health_state;
    }

    error_code = *(gint16 *)(void *)&tmp_data_info[SMBUS_READ_ERROR_CODE_OFFSET];
    if (error_code != 0) {
        debug_log(DLOG_ERROR, "%s get chip health failed. error_code = %d.", dfl_get_object_name(obj_handle),
            error_code);
        return health_state;
    }

    if (tmp_data_info[SMBUS_READ_DATA_OFFSET] != 0) {
        debug_log(DLOG_ERROR, "%s chip is not health.", dfl_get_object_name(obj_handle));
        health_state = RET_ERR;
    }

    return health_state;
}


gint32 retimer_get_and_clear_chip_health(OBJ_HANDLE obj_handle)
{
    gint32 ret;

    
    ret = retimer_get_chip_health(obj_handle);

    
    retimer_clear_chip_health(obj_handle);

    return ret;
}


LOCAL gint32 retimer_store_log(OBJ_HANDLE obj_handle, const gchar *filename, gint32 total_len, gint32 single_frame_len)
{
    gint32 ret;
    gint8 resp_buff[BUFFER_MAX_LEN] = {0};
    SMBUS_REQ req = { 0 };
    RetimerRespDataHead *resp = (RetimerRespDataHead *)&resp_buff[0];
    gint32 remain_len = total_len;
    FILE *fp = NULL;

    if (single_frame_len <= 0) {
        return RET_ERR;
    }

    
    fp = dal_fopen_check_realpath(filename, "w+", filename);
    if (fp == NULL) {
        debug_log(DLOG_ERROR, "%s: Create file %s failed.", __FUNCTION__, filename);
        return RET_ERR;
    }
    
    (void)fchown(fileno(fp), ADMINISTRATOR_USER_UID, ADMINISTRATOR_GID);
    (void)fchmod(fileno(fp), (S_IRUSR | S_IWUSR | S_IRGRP));

    req.lun = SMBUS_I2C_LUN_VALUE;
    req.arg = 1; 
    req.opcode = QUERY_LOG_OPCODE;
    req.data[0] = 2; 

    while (remain_len > 0) {
        gint32 frame_len = (remain_len >= single_frame_len) ? single_frame_len : remain_len;
        remain_len -= frame_len;
        debug_log(DLOG_DEBUG, "%s: remain len: %d.", __FUNCTION__, remain_len);

        req.length = frame_len;
        ret = retimer_get_info(obj_handle, &req, 1, resp_buff, BUFFER_MAX_LEN); 
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: retimer request opcode(%u) failed, ret = %d", __FUNCTION__, QUERY_LOG_OPCODE,
                ret);
            break;
        }

        if (fwrite((void *)&(resp->data[0]), sizeof(gchar), frame_len, fp) != frame_len) {
            debug_log(DLOG_ERROR, "%s: write date to file %s failed", __FUNCTION__, filename);
            break;
        }
    }

    debug_log(DLOG_DEBUG, "%s: write log %s finished, total len: %d remain len: %d.", __FUNCTION__, filename, total_len,
        remain_len);

    
    (void)fchmod(fileno(fp), (S_IRUSR | S_IRGRP));

    
    (void)fflush(fp);
    
    (void)fsync(fileno(fp));
    (void)fclose(fp);

    return RET_OK;
}


gint32 retimer_get_and_store_log(OBJ_HANDLE obj_handle, const gchar *filename)
{
    gint32 ret;
    gint8 resp_buff[BUFFER_MAX_LEN] = {0};
    SMBUS_REQ req = { 0 };
    RetimerRespDataHead *resp = NULL;

    resp = (RetimerRespDataHead *)&resp_buff[0];

    req.lun = SMBUS_I2C_LUN_VALUE;
    req.arg = SMBUS_I2C_ARG_VALUE;
    req.opcode = QUERY_LOG_OPCODE;
    req.length = 4;  
    req.data[0] = 2; 

    ret = retimer_get_info(obj_handle, &req, 1, resp_buff, BUFFER_MAX_LEN); 
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: retimer request opcode(%u) failed, ret = %d.", __FUNCTION__, QUERY_LOG_OPCODE, ret);
        return ret;
    }
    debug_log(DLOG_DEBUG, "%s: retimer request opcode(%u) successed.", __FUNCTION__, QUERY_LOG_OPCODE);

    
    guint16 nums = MAKE_WORD(resp->data[1], resp->data[0]); 
    
    guint16 single_len = MAKE_WORD(resp->data[3], resp->data[2]); 
    if (single_len == 0) {
        return RET_ERR;
    }
    
    gint32 total_len = single_len * nums; 
    
    gint32 single_frame_len = 240 / single_len * single_len;

    debug_log(DLOG_DEBUG, "%s: nums %u single_len %u single_frame_len %d.", __FUNCTION__, nums, single_len,
        single_frame_len);

    return retimer_store_log(obj_handle, filename, total_len, single_frame_len);
}


gint32 retimer_get_log_name(gchar **name_arr, gint32 arr_size, gchar *name_buff, gint32 buff_len)
{
    gint32 file_index;
    gint32 ret;

    
    if (access(name_arr[1], F_OK) != 0) {
        ret = strcpy_s(name_buff, buff_len, name_arr[1]);
        if (ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strcpy failed, ret = %d.", __FUNCTION__, ret);
        }
        return ret;
    }

    
    for (file_index = arr_size - 2; file_index >= 2; file_index--) {
        if (access(name_arr[file_index], F_OK) == 0) {
            ret = rename(name_arr[file_index], name_arr[file_index + 1]);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s: rename from %s to %s failed, ret = %d.", __FUNCTION__, name_arr[file_index],
                    name_arr[file_index + 1], ret);
                return ret;
            }
        }
    }
    
    ret = strcpy_s(name_buff, buff_len, name_arr[2]);
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strcpy failed, ret = %d.", __FUNCTION__, ret);
    }
    return ret;
}


gint32 retimer_get_ram_log(OBJ_HANDLE obj_handle, RetimerPCIeStatus *retimer_pcie_status)
{
    gint32 ret;
    gchar filename[RETIMER_LOG_PATH_MAX_LEN] = {0};

    ret = retimer_get_log_name(retimer_pcie_status->ram_log_name, RETIMER_LOG_NUM + 1, filename, sizeof(filename));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: failed to get log name , ret = %d.", __FUNCTION__, ret);
        return ret;
    }

    
    return retimer_get_and_store_log(obj_handle, filename);
}


void check_get_smm_retimer_logs(OBJ_HANDLE obj_handle)
{
    gint32 ret;

    RetimerPCIeStatus *retimer_pcie_status = NULL;
    ret = dfl_get_binded_data(obj_handle, (gpointer *)&retimer_pcie_status);
    if (ret != DFL_OK || retimer_pcie_status == NULL) {
        debug_log(DLOG_ERROR, "%s: get bind data of %s failed, ret = %d.", __FUNCTION__,
            dfl_get_object_name(obj_handle), ret);
        return;
    }

    debug_log(DLOG_DEBUG, "%s: start to get ram log.", __FUNCTION__);
    
    ret = retimer_get_ram_log(obj_handle, retimer_pcie_status);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: failed to get ram log ret = %d.", __FUNCTION__, ret);
    }
}


gboolean check_retimer_powerstate_is_ok(OBJ_HANDLE obj_handle)
{
    guint8 powerstate = 1;
    OBJ_HANDLE comp_handle = INVALID_OBJ_HANDLE;
    gint32 ret = dfl_get_referenced_object(obj_handle, PROPERTY_RETIMER_REFCOMPONENT, &comp_handle);
    if (ret != RET_OK) {
        return TRUE;
    }

    (void)dal_get_property_value_byte(comp_handle, PROPERTY_COMPONENT_POWERSTATE, &powerstate);
    if (powerstate != 1) {
        debug_log_limit_when_fail(DLOG_LIMIT_60, TRUE, "%s ref powerstate[%d] is not ok.",
            dfl_get_object_name(obj_handle), powerstate);
        return FALSE;
    }
    return TRUE;
}


gboolean retimer_is_need_scan_task(OBJ_HANDLE obj_handle)
{
    if (check_retimer_powerstate_is_ok(obj_handle) == FALSE) {
        return FALSE;
    }

    guint32 manage_mask = 0xff;

    gint32 ret = dal_get_property_value_uint32(obj_handle, PROPERTY_RETIMER_MANAGE_MASK, &manage_mask);
    
    if ((ret != RET_OK) || (manage_mask != RETIMER_IS_SHARE_MODULE)) {
        return TRUE;
    }

    return is_smm_active();
}


LOCAL gint32 retimer_set_lane_power_state(OBJ_HANDLE obj_handle, guint8 channel_id, guint8 power_switch)
{
    gint8 data_info[BUFFER_MAX_LEN] = {0};
    SMBUS_REQ req = { 0 };

    req.lun = SMBUS_I2C_LUN_VALUE;
    req.arg = SMBUS_I2C_ARG_VALUE;
    req.opcode = RETIMER_CMD_POWER_OPT;
    req.offset = REQUEST_OFFSET;
    req.length = 4; 

    req.data[0] = channel_id;   
    req.data[1] = 1;            
    req.data[2] = 0;            
    req.data[3] = power_switch; 

    gint32 ret = retimer_get_info(obj_handle, &req, 4, data_info, BUFFER_MAX_LEN); 
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get info failed, object = %s ret[%d]", dfl_get_object_name(obj_handle), ret);
        return ret;
    }

    guint16 result = *(guint16 *)(void *)&data_info[SMBUS_READ_DATA_OFFSET];
    debug_log(DLOG_ERROR, "%s lane[%d] %s, object = %s", (power_switch == 0) ? "PowerOff" : "PowerOn", channel_id,
        (result == 0) ? "success" : "failed", dfl_get_object_name(obj_handle));

    return (result == 0) ? RET_OK : RET_ERR;
}


LOCAL gint32 retimer_get_lane_power_state(OBJ_HANDLE obj_handle, guint8 channel_id, guint8 *power_state)
{
    guint32 reg_addr;
    const guint8 channel_group = 4; 
    const guint8 macro_offset = 2;  
    gint8 data_info[BUFFER_MAX_LEN] = {0};

    if (channel_id >= RETIMER_CHANNEL_NUM) {
        debug_log(DLOG_ERROR, "Channel id out of range, channel[%d]. %s.", channel_id, dfl_get_object_name(obj_handle));
        return RET_ERR;
    }

    if (channel_id < 8) { 
        reg_addr =
            0x20004008 + (channel_id / channel_group + macro_offset) * 0x1000000 + (channel_id % channel_group) * 0x400;
    } else {
        reg_addr =
            0x20004008 + (channel_id / channel_group - macro_offset) * 0x1000000 + (channel_id % channel_group) * 0x400;
    }

    gint32 ret = retimer_get_reg_info_5902(obj_handle, reg_addr, data_info, BUFFER_MAX_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get registor value failed, object = %s, channel[%d].", dfl_get_object_name(obj_handle),
            channel_id);
        return RET_ERR;
    }

    guint32 reg_value = *(guint32 *)(void *)&data_info[SMBUS_READ_DATA_OFFSET];
    *power_state = reg_value & 0x20; 
    debug_log(DLOG_DEBUG, "Get channel[%d] status is %s. object = %s.", channel_id,
        (*power_state == 0) ? "PowerOff" : "PowerOn", dfl_get_object_name(obj_handle));

    return RET_OK;
}


void retimer_disable_lane(OBJ_HANDLE obj_handle)
{
    if (obj_handle == 0) {
        debug_log(DLOG_ERROR, "%s:obj_handle is null!", __FUNCTION__);
    }

    const guint8 lane_len = 32; 
    guint32 lane = 0x0;
    guint8 power_state = 0;
    gint32 ret = dal_get_property_value_uint32(obj_handle, PROPERTY_RETIMER_DISABLE_LANE, &lane);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "[%s] get %s failed ret[%d]", __FUNCTION__, PROPERTY_RETIMER_DISABLE_LANE, ret);
        return;
    }

    for (guint8 i = 0; i < lane_len; i++) {
        if (!(lane & (0x1 << i))) {
            continue;
        }
        ret = retimer_get_lane_power_state(obj_handle, i, &power_state); 
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "[%s] get power state failed ret[%d]", __FUNCTION__, ret);
            continue;
        }

        if (power_state == 0x20) { 
            retimer_set_lane_power_state(obj_handle, i, 0); 
        }
    }
}
