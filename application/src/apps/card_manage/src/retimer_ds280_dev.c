

#include <stdio.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <errno.h>
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "retimer_ds280_dev.h"


LOCAL gint32 retimer_ds280_smbus_read(OBJ_HANDLE obj_handle, guint8 reg, guint8 *data)
{
    gint32 ret;
    OBJ_HANDLE chip_handle;

    if (data == NULL) {
        debug_log(DLOG_ERROR, "Parameter error");
        return RET_ERR;
    }

    
    ret = dfl_get_referenced_object(obj_handle, PROPERTY_RETIMER_REFCHIP, &chip_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get referentce obj (%s) failed(%d).", dfl_get_object_name(obj_handle), ret);
        return ret;
    }

    
    ret = dfl_chip_blkread(dfl_get_object_name(chip_handle), reg, 1, data);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Chip blk read failed(%d).", ret);
    }

    return ret;
}


LOCAL gint32 retimer_ds280_smbus_write(OBJ_HANDLE obj_handle, guint8 reg, guint8 data, guint8 mask)
{
    guint8 dest_value;
    guint8 ori_value;
    gint32 ret;
    OBJ_HANDLE chip_handle;

    
    ret = dfl_get_referenced_object(obj_handle, PROPERTY_RETIMER_REFCHIP, &chip_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get referentce obj (%s) failed(%d).", dfl_get_object_name(obj_handle), ret);
        return ret;
    }

    
    ret = dfl_chip_blkread(dfl_get_object_name(chip_handle), reg, 1, &ori_value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Chip [%s] blk read failed(%d).", dfl_get_object_name(obj_handle), ret);
        return ret;
    }

    dest_value = (ori_value | (data & mask)) & ((data & mask) | (guint8)(~mask)); 

    
    ret = dfl_chip_blkwrite(dfl_get_object_name(chip_handle), reg, 1, &dest_value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Chip [%s] blk write failed(%d).", dfl_get_object_name(obj_handle), ret);
    }
    return ret;
}


LOCAL gint32 retimer_ds280_open_workaround(OBJ_HANDLE obj_handle)
{
    guint8 version = 0;

    gint32 ret = retimer_ds280_smbus_read(obj_handle, CDR_VERSION_ID_OFFSET, &version);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "(%s) open workaround failed(%d).", dfl_get_object_name(obj_handle), ret);
        return ret;
    }

    
    if (version == 0x30) {
        debug_log(DLOG_INFO, "[%s] cdr open workaround.", dfl_get_object_name(obj_handle));
        retimer_ds280_smbus_write(obj_handle, CDR_HEO_VEO_OFFSET, 0xC0, 0xC0);
        retimer_ds280_smbus_write(obj_handle, CDR_POWER_SET_OFFSET, 0x08, 0x08);
        retimer_ds280_smbus_write(obj_handle, CDR_FIR_OFFSET, 0x02, 0x03);
    }

    return RET_OK;
}


gint32 retimer_ds280_config(OBJ_HANDLE obj_handle, CDR_DS280_CFG *cdr_cfg)
{
    gint32 ret = 0;
    guint8 main0 = abs(cdr_cfg->ffe_main);
    guint8 post1 = abs(cdr_cfg->ffe_post1);
    guint8 pre1 = abs(cdr_cfg->ffe_pre1);
    guint8 channel = 1 << cdr_cfg->cdr_chan;

    
    ret += retimer_ds280_smbus_write(obj_handle, CDR_BROADCAST_ENABLE_OFFSET, CDR_SINGLE_CHANNEL_SET, 0x3);
    
    ret += retimer_ds280_smbus_write(obj_handle, CDR_CHANNEL_SET_OFFSET, channel, 0xff); 
    ret += retimer_ds280_smbus_write(obj_handle, CDR_CHANNEL_RESET_OFFSET, 0x04, 0x04);
    ret += retimer_ds280_smbus_write(obj_handle, CDR_RESET_OFFSET, 0x0c, 0x0c);

    
    ret += retimer_ds280_open_workaround(obj_handle);

    
    ret += retimer_ds280_smbus_write(obj_handle, CDR_STANDARD_DATA_RATE_OFFSET, cdr_cfg->lane_speed, 0xF0); 
    
    ret += retimer_ds280_smbus_write(obj_handle, CDR_RX_ADAPT_MODE_OFFSET, 0x40, 0x60);
    
    ret += retimer_ds280_smbus_write(obj_handle, CDR_DFE_OFFSET, 0x02, 0x0A);
    
    ret += retimer_ds280_smbus_write(obj_handle, 0x7F, 0x01, 0x07);
    ret += retimer_ds280_smbus_write(obj_handle, 0x7D, 0x0F, 0x0F);
    ret += retimer_ds280_smbus_write(obj_handle, 0x7F, 0x10, 0x10);

    ret += retimer_ds280_smbus_write(obj_handle, CDR_MAIN_CURSOR_OFFSET, 0x80, 0x80);
    ret += retimer_ds280_smbus_write(obj_handle, CDR_MAIN_CURSOR_OFFSET, 0x00, 0x40);
    ret += retimer_ds280_smbus_write(obj_handle, CDR_POST_CURSOR_OFFSET, 0x40, 0x40);
    ret += retimer_ds280_smbus_write(obj_handle, CDR_PREV_CURSOR_OFFSET, 0x40, 0x40);
    ret += retimer_ds280_smbus_write(obj_handle, CDR_MAIN_CURSOR_OFFSET, main0, 0x1F); 
    ret += retimer_ds280_smbus_write(obj_handle, CDR_POST_CURSOR_OFFSET, post1, 0x0F); 
    ret += retimer_ds280_smbus_write(obj_handle, CDR_PREV_CURSOR_OFFSET, pre1, 0x0F);  
    ret += retimer_ds280_smbus_write(obj_handle, CDR_RESET_OFFSET, 0x00, 0x0C);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "(%s) Retimer ds280 config failed(%d).", dfl_get_object_name(obj_handle), ret);
    }
    return ret;
}


gint32 retimer_get_reg_info_ds280(OBJ_HANDLE obj_handle, guint32 reg_addr, gint8 *data_info, gint32 buf_len)
{
    guint8 data = 0;

    if (buf_len <= SMBUS_READ_DATA_OFFSET) {
        debug_log(DLOG_ERROR, "Buffer parameter is too short.");
        return RET_ERR;
    }

    gint32 ret = retimer_ds280_smbus_read(obj_handle, (guint8)reg_addr, &data);
    debug_log(DLOG_DEBUG, "[%s] data = 0x%x", dfl_get_object_name(obj_handle), data);
    if (ret == RET_OK) {
        data_info[SMBUS_READ_DATA_OFFSET] = data;
    }

    return ret;
}


gint32 retimer_get_tx_ffe_ds280(OBJ_HANDLE obj_handle, guint8 channel_id, gint8 *data_info, gint32 buf_len)
{
    gint32 ret = 0;
    guint8 channel;
    guint8 main0 = 0;
    guint8 post1 = 0;
    guint8 pre1 = 0;
    guint8 key = SMBUS_READ_DATA_OFFSET;

    
    channel = 1 << channel_id;
    ret += retimer_ds280_smbus_write(obj_handle, CDR_BROADCAST_ENABLE_OFFSET, CDR_SINGLE_CHANNEL_SET, 0x01);
    ret += retimer_ds280_smbus_write(obj_handle, CDR_CHANNEL_SET_OFFSET, channel, 0xff);
    
    ret += retimer_ds280_smbus_read(obj_handle, CDR_MAIN_CURSOR_OFFSET, &main0); 
    ret += retimer_ds280_smbus_read(obj_handle, CDR_POST_CURSOR_OFFSET, &post1); 
    ret += retimer_ds280_smbus_read(obj_handle, CDR_PREV_CURSOR_OFFSET, &pre1);  
    
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Get tx ffe failed. ret=%d.", dfl_get_object_name(obj_handle), ret);
        return RET_ERR;
    }

    
    if (buf_len <= SMBUS_READ_DATA_OFFSET + 13) { 
        debug_log(DLOG_ERROR, "Buffer parameter is too short.");
        return RET_ERR;
    }

    *(gint16 *)(void *)&data_info[key] = 0x0000;
    key += (guint8)sizeof(gint16);
    data_info[key] = channel_id;
    key++;
    data_info[key] = 0x1;
    key++;
    *(gint16 *)(void *)&data_info[key] = 0;
    key += (guint8)sizeof(gint16);
    *(gint16 *)(void *)&data_info[key] = (pre1 & 0x0F) * (-1);
    key += (guint8)sizeof(gint16);
    *(gint16 *)(void *)&data_info[key] = main0 & 0x1F;
    key += (guint8)sizeof(gint16);
    *(gint16 *)(void *)&data_info[key] = (post1 & 0x0F) * (-1);
    key += (guint8)sizeof(gint16);
    *(gint16 *)(void *)&data_info[key] = 0;

    return RET_OK;
}


gint32 retimer_set_tx_ffe_ds280(OBJ_HANDLE obj_handle, SET_TX_FFE_DEBUG *tx_ffe, gint8 *data_info, gint32 buf_len)
{
    gint32 ret = 0;
    guint8 channel = 1 << tx_ffe->channel_id;
    guint8 tmp_main = abs(tx_ffe->main0);
    guint8 tmp_post = abs(tx_ffe->post1);
    guint8 tmp_pre = abs(tx_ffe->pre1);
    guint8 key = SMBUS_READ_DATA_OFFSET;

    
    ret += retimer_ds280_smbus_write(obj_handle, CDR_BROADCAST_ENABLE_OFFSET, CDR_SINGLE_CHANNEL_SET, 0x3);
    ret += retimer_ds280_smbus_write(obj_handle, CDR_CHANNEL_SET_OFFSET, channel, 0xff);
    
    ret += retimer_ds280_smbus_write(obj_handle, CDR_MAIN_CURSOR_OFFSET, 0x80, 0x80);
    ret += retimer_ds280_smbus_write(obj_handle, CDR_MAIN_CURSOR_OFFSET, 0x00, 0x40);
    ret += retimer_ds280_smbus_write(obj_handle, CDR_POST_CURSOR_OFFSET, 0x40, 0x40);
    ret += retimer_ds280_smbus_write(obj_handle, CDR_PREV_CURSOR_OFFSET, 0x40, 0x40);
    ret += retimer_ds280_smbus_write(obj_handle, CDR_MAIN_CURSOR_OFFSET, tmp_main, 0x1F); 
    ret += retimer_ds280_smbus_write(obj_handle, CDR_POST_CURSOR_OFFSET, tmp_post, 0x0F); 
    ret += retimer_ds280_smbus_write(obj_handle, CDR_PREV_CURSOR_OFFSET, tmp_pre, 0x0F);  
    
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Set tx ffe failed. ret=%d.", dfl_get_object_name(obj_handle), ret);
        return RET_ERR;
    }

    
    if (buf_len <= SMBUS_READ_DATA_OFFSET + 4) { 
        debug_log(DLOG_ERROR, "Buffer parameter is too short.");
        return RET_ERR;
    }

    *(gint16 *)(void *)&data_info[key] = 0x0000;
    key += (guint8)sizeof(gint16);
    data_info[key] = tx_ffe->channel_id;
    key++;
    data_info[key] = 0x1;

    return RET_OK;
}


gint32 retimer_get_temperature_ds280(OBJ_HANDLE obj_handle, gint16 *temp_data)
{
    gint32 ret = 0;
    guint8 temp_low = 0;
    guint8 temp_high = 0;

    
    DS280_SMBUS_WRITE config_before_read[] = {
        {CDR_BROADCAST_ENABLE_OFFSET, 0x10, 0xFF}, {CDR_RESET_SHARED_REG, 0x40, 0x40},
        {CDR_BROADCAST_ENABLE_OFFSET, 0x20, 0xFF}, {CDR_RESET_SHARED_REG, 0x40, 0x40},
        {CDR_BROADCAST_ENABLE_OFFSET, 0x10, 0xFF}, {CDR_ENABLE_TEMP_DETECTION, 0x0F, 0xFF},
        {CDR_BROADCAST_ENABLE_OFFSET, 0x20, 0xFF}, {CDR_ENABLE_TEMP_DETECTION, 0x0F, 0xFF},
        {CDR_BROADCAST_ENABLE_OFFSET, 0x03, 0xFF}, {CDR_CHANNEL_SET_OFFSET, 0x01, 0x0F},
        {CDR_ENABLE_CHANNEL_TEMP, 0x01, 0xC1}, {CDR_POWER_SET_OFFSET, 0x01, 0x03},
        {CDR_BROADCAST_ENABLE_OFFSET, 0x10, 0xFF}, {CDR_CONFIG_ADC, 0x00, 0x38},
        {CDR_CONFIG_ADC, 0x02, 0x03}, {CDR_CONFIG_ADC, 0x04, 0x04}, {CDR_CONFIG_ADC, 0x00, 0x04},
    };
    for (guint8 i = 0; i < sizeof(config_before_read) / sizeof(DS280_SMBUS_WRITE); i++) {
        ret += retimer_ds280_smbus_write(obj_handle, config_before_read[i].reg, config_before_read[i].data,
            config_before_read[i].mask);
    }
    usleep(200); 
    ret += retimer_ds280_smbus_write(obj_handle, CDR_CONFIG_ADC, 0x00, 0x02);

    
    ret += retimer_ds280_smbus_read(obj_handle, CDR_TEMP_LOW_BYTE, &temp_low);
    ret += retimer_ds280_smbus_read(obj_handle, CDR_TEMP_HIGH_BYTE, &temp_high);

    
    DS280_SMBUS_WRITE config_after_read[] = {
        {CDR_BROADCAST_ENABLE_OFFSET, 0x10, 0xFF}, {CDR_RESET_SHARED_REG, 0x40, 0x40},
        {CDR_BROADCAST_ENABLE_OFFSET, 0x20, 0xFF}, {CDR_RESET_SHARED_REG, 0x40, 0x40},
        {CDR_BROADCAST_ENABLE_OFFSET, 0x03, 0xFF}, {CDR_ENABLE_CHANNEL_TEMP, 0x00, 0xC1},
        {CDR_POWER_SET_OFFSET, 0x00, 0x03}, {CDR_BROADCAST_ENABLE_OFFSET, 0x01, 0x01},
    };
    for (guint8 i = 0; i < sizeof(config_after_read) / sizeof(DS280_SMBUS_WRITE); i++) {
        ret += retimer_ds280_smbus_write(obj_handle, config_after_read[i].reg, config_after_read[i].data,
            config_after_read[i].mask);
    }
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Get tempurature failed. ret=%d.", dfl_get_object_name(obj_handle), ret);
        return RET_ERR;
    }

    debug_log(DLOG_DEBUG, "[%s] low = %d, high = %d.", dfl_get_object_name(obj_handle), temp_low, temp_high);

    
    gint16 data_tmp = (gint16)(guint16)((temp_high & 0x03) * 256 + temp_low); 
    *temp_data = data_tmp * 44 / 100 - 55;                                    

    return ret;
}


gint32 retimer_get_eye_diag_ds280(OBJ_HANDLE obj_handle, guint8 channel_id, gint8 *data_info, gint32 buf_len)
{
    gint32 ret = 0;
    guint8 eye_h = 0;
    guint8 eye_v = 0;
    guint8 channel = 1 << channel_id;
    guint8 key = SMBUS_READ_DATA_OFFSET;

    
    ret += retimer_ds280_smbus_write(obj_handle, CDR_BROADCAST_ENABLE_OFFSET, CDR_SINGLE_CHANNEL_SET, 0x3);
    ret += retimer_ds280_smbus_write(obj_handle, CDR_CHANNEL_SET_OFFSET, channel, 0xff);
    
    ret += retimer_ds280_smbus_read(obj_handle, CDR_HORIZONTAL_EYE, &eye_h);
    ret += retimer_ds280_smbus_read(obj_handle, CDR_VERTICAL_EYE, &eye_v);
    
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Get eyediag failed. ret=%d.", dfl_get_object_name(obj_handle), ret);
        return RET_ERR;
    }

    
    *(gint16 *)(void *)&data_info[key] = 0x0000;
    key += (guint8)sizeof(gint16);
    data_info[key] = channel_id;
    key++;
    data_info[key] = 0x1;
    key++;
    *(gint16 *)(void *)&data_info[key] = 0x0000;
    key += (guint8)sizeof(gint16);
    *(gint16 *)(void *)&data_info[key] = (gint16)eye_v;
    key += (guint8)sizeof(gint16);
    *(gint16 *)(void *)&data_info[key] = 0x0000;
    key += (guint8)sizeof(gint16);
    *(gint16 *)(void *)&data_info[key] = (gint16)eye_h;

    return ret;
}


LOCAL gint32 ds280_egress_configuration_sequence(OBJ_HANDLE obj_handle)
{
    gint32 ret = 0;

    ret += retimer_ds280_smbus_write(obj_handle, 0xFC, 0x55, 0xFF); 
    ret += retimer_ds280_smbus_write(obj_handle, 0xFF, 0x01, 0xFF);
    ret += retimer_ds280_smbus_write(obj_handle, 0x0A, 0x0C, 0x0C);
    ret += retimer_ds280_smbus_write(obj_handle, 0x2F, 0x54, 0x70);
    ret += retimer_ds280_smbus_write(obj_handle, 0x31, 0x40, 0x60);
    ret += retimer_ds280_smbus_write(obj_handle, 0x3D, 0x8A, 0xDF);
    ret += retimer_ds280_smbus_write(obj_handle, 0x3E, 0x40, 0x4F);
    ret += retimer_ds280_smbus_write(obj_handle, 0x3F, 0x40, 0x4F);
    ret += retimer_ds280_smbus_write(obj_handle, 0x0A, 0x00, 0x0C);

    ret += retimer_ds280_smbus_write(obj_handle, 0x0A, 0x0C, 0x0C); 
    ret += retimer_ds280_smbus_write(obj_handle, 0x31, 0x40, 0x60);

    ret += retimer_ds280_smbus_write(obj_handle, 0x7F, 0x01, 0x07); 
    ret += retimer_ds280_smbus_write(obj_handle, 0x7D, 0x0F, 0x0F);
    ret += retimer_ds280_smbus_write(obj_handle, 0x7F, 0x10, 0x10);

    ret += retimer_ds280_smbus_write(obj_handle, 0x23, 0x40, 0x40); 
    ret += retimer_ds280_smbus_write(obj_handle, 0x1E, 0x02, 0x0A);
    ret += retimer_ds280_smbus_write(obj_handle, 0x2D, 0x08, 0x08);
    ret += retimer_ds280_smbus_write(obj_handle, 0x03, 0x08, 0xFF);
    ret += retimer_ds280_smbus_write(obj_handle, 0x0A, 0x00, 0x0C);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Config egress configuration sequence failed. ret=%d.",
            dfl_get_object_name(obj_handle), ret);
    }
    return ret;
}


LOCAL gint32 ds280_enable_prbs_generator(OBJ_HANDLE obj_handle)
{
    gint32 ret = 0;

    ret += retimer_ds280_smbus_write(obj_handle, 0x2E, 0x04, 0x04);
    ret += retimer_ds280_smbus_write(obj_handle, 0x30, 0x01, 0x03);
    ret += retimer_ds280_smbus_write(obj_handle, 0x1E, 0x10, 0x10);
    ret += retimer_ds280_smbus_write(obj_handle, 0x79, 0x00, 0x20);
    ret += retimer_ds280_smbus_write(obj_handle, 0x79, 0x20, 0x20);
    ret += retimer_ds280_smbus_write(obj_handle, 0x30, 0x00, 0x08);
    ret += retimer_ds280_smbus_write(obj_handle, 0x30, 0x08, 0x08);
    ret += retimer_ds280_smbus_write(obj_handle, 0xA5, 0x80, 0xE0);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Enable prbs generator failed. ret=%d.", dfl_get_object_name(obj_handle), ret);
    }
    return ret;
}


LOCAL gint32 ds280_disable_prbs_generator(OBJ_HANDLE obj_handle)
{
    gint32 ret = 0;

    ret += retimer_ds280_smbus_write(obj_handle, 0x1E, 0x00, 0x10);
    ret += retimer_ds280_smbus_write(obj_handle, 0x79, 0x00, 0x20);
    ret += retimer_ds280_smbus_write(obj_handle, 0x30, 0x00, 0x08);
    ret += retimer_ds280_smbus_write(obj_handle, 0xA5, 0x20, 0xE0);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Disable prbs generator failed. ret=%d.", dfl_get_object_name(obj_handle), ret);
    }
    return ret;
}


gint32 retimer_prbs_send_recv_ds280(OBJ_HANDLE obj_handle, PRBS_SEND_RECV *prbs)
{
    gint32 ret;

    
    ret = ds280_egress_configuration_sequence(obj_handle);
    if (ret != RET_OK) {
        return ret;
    }

    
    if (prbs->prbs_type != 0) {
        ret = ds280_enable_prbs_generator(obj_handle);
    } else {
        ret = ds280_disable_prbs_generator(obj_handle);
    }

    debug_log(DLOG_DEBUG, "[%s] prbs_type = %d, ret = %d.", dfl_get_object_name(obj_handle), prbs->prbs_type, ret);
    return ret;
}


LOCAL gint32 ds280_enable_prbs_checker(OBJ_HANDLE obj_handle)
{
    gint32 ret = 0;

    ret += retimer_ds280_smbus_write(obj_handle, 0x0D, 0x00, 0x80);
    ret += retimer_ds280_smbus_write(obj_handle, 0x79, 0x40, 0x40);
    ret += retimer_ds280_smbus_write(obj_handle, 0x30, 0x00, 0x08);
    ret += retimer_ds280_smbus_write(obj_handle, 0x30, 0x08, 0x08);
    ret += retimer_ds280_smbus_write(obj_handle, 0x30, 0x10, 0x10);
    ret += retimer_ds280_smbus_write(obj_handle, 0x30, 0x00, 0x10);
    ret += retimer_ds280_smbus_write(obj_handle, 0x82, 0x40, 0x40);
    ret += retimer_ds280_smbus_write(obj_handle, 0x82, 0x00, 0x40);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Enable prbs checker failed. ret=%d.", dfl_get_object_name(obj_handle), ret);
    }
    return ret;
}


LOCAL gint32 ds280_read_prbs_errorcnt(OBJ_HANDLE obj_handle, guint16 *error_cnt)
{
    gint32 ret = 0;
    guint8 err_cnt_high = 0;
    guint8 err_cnt_low = 0;

    
    ret += retimer_ds280_smbus_write(obj_handle, 0x82, 0x80, 0x80);
    ret += retimer_ds280_smbus_read(obj_handle, 0x83, &err_cnt_high);
    ret += retimer_ds280_smbus_read(obj_handle, 0x84, &err_cnt_low);
    ret += retimer_ds280_smbus_write(obj_handle, 0x82, 0x00, 0x80);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Read error count failed. ret=%d.", dfl_get_object_name(obj_handle), ret);
        return RET_ERR;
    }

    
    *error_cnt = ((err_cnt_high & 0x03) << 0x08) | err_cnt_low;
    return RET_OK;
}


LOCAL gint32 ds280_clear_prbs_errorcnt(OBJ_HANDLE obj_handle)
{
    gint32 ret = 0;

    ret += retimer_ds280_smbus_write(obj_handle, 0x82, 0x80, 0x80);
    ret += retimer_ds280_smbus_write(obj_handle, 0x82, 0x00, 0x40);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Clear error count failed. ret=%d.", dfl_get_object_name(obj_handle), ret);
    }
    return ret;
}


LOCAL gint32 ds280_disable_prbs_checker(OBJ_HANDLE obj_handle)
{
    gint32 ret = 0;

    ret += retimer_ds280_smbus_write(obj_handle, 0x0D, 0x80, 0x80);
    ret += retimer_ds280_smbus_write(obj_handle, 0x79, 0x00, 0x40);
    ret += retimer_ds280_smbus_write(obj_handle, 0x30, 0x00, 0x08);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Disable prbs checker failed. ret=%d.", dfl_get_object_name(obj_handle), ret);
    }

    return ret;
}


gint32 retimer_get_prbs_error_ds280(OBJ_HANDLE obj_handle, PRBS_ERROR_CHECK *prbs, gint8 *data_info, gint32 buf_len)
{
    gint32 ret = 0;
    guint16 tmp_error = 0;
    guint8 i;
    guint8 channel;

    if (prbs->channel_id + prbs->lane_num > RETIMER_LANE_NUM) { 
        debug_log(DLOG_ERROR, "[%s] Wrong parameter. ret=%d.", dfl_get_object_name(obj_handle), ret);
        return RET_ERR;
    }

    for (i = 0; i < prbs->lane_num; i++) {
        channel = 1 << (i + prbs->channel_id);

        
        ret += retimer_ds280_smbus_write(obj_handle, CDR_BROADCAST_ENABLE_OFFSET, CDR_SINGLE_CHANNEL_SET, 0x3);
        ret += retimer_ds280_smbus_write(obj_handle, CDR_CHANNEL_SET_OFFSET, channel, 0xff);

        
        ret += ds280_enable_prbs_checker(obj_handle);

        
        ret += ds280_read_prbs_errorcnt(obj_handle, &tmp_error);

        
        ret += ds280_clear_prbs_errorcnt(obj_handle);

        
        ret += ds280_disable_prbs_checker(obj_handle);
        if (ret != RET_OK) {
            return ret;
        }

        
        *(guint32 *)(void *)&data_info[i * sizeof(guint32)] = (guint32)tmp_error;
        debug_log(DLOG_ERROR, "Successfully get lane(%d) error_cnt = 0x%X.", i + prbs->channel_id, tmp_error);
    }

    return RET_OK;
}


gint32 retimer_get_heartbeat_ds280(OBJ_HANDLE obj_handle)
{
    gint32 ret;
    gint8 data_info[BUFFER_MAX_LEN] = {0};
    guint8 i;

    for (i = 0; i < RETIMER_MSG_REPEATE_NUM; i++) {
        
        ret = retimer_get_reg_info_ds280(obj_handle, 0xF0, data_info, BUFFER_MAX_LEN);
        if (ret == RET_OK) {
            debug_log(DLOG_DEBUG, "[%s] version_id = 0x%02X", dfl_get_object_name(obj_handle),
                data_info[SMBUS_READ_DATA_OFFSET]);
            return RET_OK;
        }

        vos_task_delay(100); 
    }

    return RET_ERR;
}