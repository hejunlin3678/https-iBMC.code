

#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "retimer_driver.h"
#include "retimer_manage.h"
#include "retimer_debug.h"


gint32 retimer_set_rx_ctle_proc(GSList *input_list)
{
    gint32 ret;
    guint8 retimer_id;
    SET_RX_CTLE_DEBUG rx_ctle = { 0 };
    gint8 data_info[BUFFER_MAX_LEN] = {};
    OBJ_HANDLE ref_handle;

    if ((input_list != NULL) && (g_slist_length(input_list) < 7)) { 
        return MD_EPARA;
    }

    retimer_id = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));         
    rx_ctle.channel_id = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 1)); 
    rx_ctle.enable = g_variant_get_uint16((GVariant *)g_slist_nth_data(input_list, 2));   
    rx_ctle.gain = g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 3));     
    rx_ctle.boost = g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 4));    
    rx_ctle.squelch = g_variant_get_uint16((GVariant *)g_slist_nth_data(input_list, 5));  
    rx_ctle.zero = g_variant_get_uint16((GVariant *)g_slist_nth_data(input_list, 6));     
    
    ret = dal_get_specific_object_byte(CLASS_RETIMER_NAME, PROPERTY_RETIMER_ID, retimer_id, &ref_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get retimer object handle failed. retimer_id = %d.", retimer_id);
        return RET_ERR;
    }

    ret = retimer_switch_channel(ref_handle, RETIMER_CHN_WITCH_OPEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Retimer switch channel failed. retimer_id = %d.", retimer_id);
        return ret;
    }

    ret = retimer_set_rx_ctle(ref_handle, &rx_ctle, data_info, BUFFER_MAX_LEN);
    if (ret != RET_OK) {
        debug_printf("Write rx ctle failed: %d. retimer_id = %d.", ret, retimer_id);
    } else if ((data_info[SMBUS_READ_DATA_OFFSET + 1] != 0) || (data_info[SMBUS_READ_DATA_OFFSET] != 0)) {
        debug_printf("Write rx ctle failed: %d-%d. retimer_id = %d.", data_info[SMBUS_READ_DATA_OFFSET],
            data_info[SMBUS_READ_DATA_OFFSET + 1], retimer_id);
    } else {
        debug_printf("Write rx ctle success. retimer_id = %d.", retimer_id);
    }

    ret = retimer_switch_channel(ref_handle, RETIMER_CHN_WITCH_CLOSE);
    return ret;
}


gint32 retimer_set_tx_ffe_proc(GSList *input_list)
{
    gint32 ret;
    guint8 retimer_id;
    SET_TX_FFE_DEBUG tx_ffe = { 0 };
    gint8 data_info[BUFFER_MAX_LEN] = {};
    OBJ_HANDLE ref_handle;

    if ((input_list != NULL) && (g_slist_length(input_list) < 7)) { 
        return MD_EPARA;
    }

    retimer_id = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));        
    tx_ffe.channel_id = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 1)); 
    tx_ffe.pre2 = g_variant_get_int16((GVariant *)g_slist_nth_data(input_list, 2));      
    tx_ffe.pre1 = g_variant_get_int16((GVariant *)g_slist_nth_data(input_list, 3));      
    tx_ffe.main0 = g_variant_get_uint16((GVariant *)g_slist_nth_data(input_list, 4));    
    tx_ffe.post1 = g_variant_get_int16((GVariant *)g_slist_nth_data(input_list, 5));     
    tx_ffe.post2 = g_variant_get_int16((GVariant *)g_slist_nth_data(input_list, 6));     
    
    ret = dal_get_specific_object_byte(CLASS_RETIMER_NAME, PROPERTY_RETIMER_ID, retimer_id, &ref_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get retimer object handle failed. retimer_id = %d.", retimer_id);
        return RET_ERR;
    }

    ret = retimer_switch_channel(ref_handle, RETIMER_CHN_WITCH_OPEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Retimer switch channel failed. retimer_id = %d.", retimer_id);
        return ret;
    }

    ret = retimer_set_tx_ffe(ref_handle, &tx_ffe, data_info, BUFFER_MAX_LEN);
    if (ret != RET_OK) {
        debug_printf("Write tx ffe failed: %d. retimer_id = %d.", ret, retimer_id);
    } else if ((data_info[SMBUS_READ_DATA_OFFSET + 1] != 0) || (data_info[SMBUS_READ_DATA_OFFSET] != 0)) {
        debug_printf("Write tx ffe failed: %d-%d. retimer_id = %d.", data_info[SMBUS_READ_DATA_OFFSET],
            data_info[SMBUS_READ_DATA_OFFSET + 1], retimer_id);
    } else {
        debug_printf("Write tx ffe success. retimer_id = %d.", retimer_id);
    }

    ret = retimer_switch_channel(ref_handle, RETIMER_CHN_WITCH_CLOSE);
    return ret;
}


gint32 retimer_set_reg_info_proc(GSList *input_list)
{
    gint32 ret;
    guint8 retimer_id;
    guint32 reg_addr;
    guint32 reg_data;
    OBJ_HANDLE ref_handle;

    if ((input_list != NULL) && (g_slist_length(input_list) < 3)) { 
        return MD_EPARA;
    }

    retimer_id = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0)); 
    reg_addr = g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 1)); 
    reg_data = g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 2)); 
    
    ret = dal_get_specific_object_byte(CLASS_RETIMER_NAME, PROPERTY_RETIMER_ID, retimer_id, &ref_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get retimer object handle failed. retimer_id = %d.", retimer_id);
        return RET_ERR;
    }

    ret = retimer_switch_channel(ref_handle, RETIMER_CHN_WITCH_OPEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Retimer switch channel failed. retimer_id = %d.", retimer_id);
        return ret;
    }

    ret = retimer_set_reg_info(ref_handle, reg_addr, reg_data);
    if (ret != RET_OK) {
        debug_printf("Write reg failed: %d. retimer_id = %d.", ret, retimer_id);
    } else {
        debug_printf("Write reg success. retimer_id = %d.", retimer_id);
    }

    ret = retimer_switch_channel(ref_handle, RETIMER_CHN_WITCH_CLOSE);
    return ret;
}


gint32 retimer_get_reg_info_proc(GSList *input_list)
{
    gint32 ret;
    guint8 retimer_id;
    guint32 reg_addr;
    gint8 data_info[BUFFER_MAX_LEN] = {};
    OBJ_HANDLE ref_handle;

    if ((input_list != NULL) && (g_slist_length(input_list) < 2)) { 
        return MD_EPARA;
    }

    retimer_id = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0)); 
    reg_addr = g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 1)); 
    
    ret = dal_get_specific_object_byte(CLASS_RETIMER_NAME, PROPERTY_RETIMER_ID, retimer_id, &ref_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get retimer object handle failed. retimer_id = %d.", retimer_id);
        return RET_ERR;
    }

    ret = retimer_switch_channel(ref_handle, RETIMER_CHN_WITCH_OPEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Retimer switch channel failed. retimer_id = %d.", retimer_id);
        return ret;
    }

    ret = retimer_get_reg_info(ref_handle, reg_addr, data_info, BUFFER_MAX_LEN);
    if (ret != RET_OK) {
        debug_printf("Read get reg failed: %d. retimer_id = %d.", ret, retimer_id);
    } else {
        debug_printf("0x%02X  ", *(gint32 *)(void *)&data_info[SMBUS_READ_DATA_OFFSET]);
    }

    ret = retimer_switch_channel(ref_handle, RETIMER_CHN_WITCH_CLOSE);
    return ret;
}


gint32 retimer_get_eye_diag_proc(GSList *input_list)
{
    guint i;
    gint32 ret;
    guint8 retimer_id;
    guint8 channel_id;
    gint8 data_info[BUFFER_MAX_LEN] = {};
    gchar* fileld_name[EYE_DIAG_ITEM_NUM] = {"eye_bot", "eye_top", "eye_left", "eye_right"};
    guint32 tmp_len;
    OBJ_HANDLE ref_handle;

    if ((input_list != NULL) && (g_slist_length(input_list) < 2)) { 
        return MD_EPARA;
    }

    retimer_id = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0)); 
    channel_id = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 1)); 
    
    ret = dal_get_specific_object_byte(CLASS_RETIMER_NAME, PROPERTY_RETIMER_ID, retimer_id, &ref_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get retimer object handle failed. retimer_id = %d.", retimer_id);
        return RET_ERR;
    }

    ret = retimer_switch_channel(ref_handle, RETIMER_CHN_WITCH_OPEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Retimer switch channel failed. retimer_id = %d.", retimer_id);
        return ret;
    }

    ret = retimer_get_eye_diag(ref_handle, channel_id, data_info, BUFFER_MAX_LEN);
    if (ret != RET_OK) {
        debug_printf("Read eye diag failed: %d. retimer_id = %d.", ret, retimer_id);
    } else if ((data_info[SMBUS_READ_DATA_OFFSET + 1] != 0) || (data_info[SMBUS_READ_DATA_OFFSET] != 0)) {
        debug_printf("Read eye diag failed: %d-%d. retimer_id = %d.", data_info[SMBUS_READ_DATA_OFFSET],
            data_info[SMBUS_READ_DATA_OFFSET + 1], retimer_id);
    } else {
        for (i = 0; i < EYE_DIAG_ITEM_NUM; i++) {
            tmp_len = EYE_DIAG_DATA_LEN + EYE_DIAG_OFFSET + i * sizeof(gint16);
            if (tmp_len + 1 >= BUFFER_MAX_LEN) { 
                debug_log(DLOG_ERROR, "Array out of bounds when getting eye diag. retimer_id = %d.", retimer_id);
                return RET_ERR;
            }
            debug_printf("%s: %d", fileld_name[i], *(gint16 *)(void *)&data_info[tmp_len]);
        }
    }

    ret = retimer_switch_channel(ref_handle, RETIMER_CHN_WITCH_CLOSE);
    return ret;
}


gint32 retimer_get_rx_ctle_proc(GSList *input_list)
{
    guint i;
    gint32 ret;
    guint8 retimer_id;
    guint8 channel_id;
    gint8 data_info[BUFFER_MAX_LEN] = {};
    gchar* fileld_name[CTLE_ITEM_NUM] = {
        "Gain1", "Gain2", "Gain3", "Boost1", "Boost2", "Boost3", "Squelch1", "Squelch2", "Zero1", "Zero2"
    };
    guint32 tmp_len;
    OBJ_HANDLE ref_handle;

    if ((input_list != NULL) && (g_slist_length(input_list) < 2)) { 
        return MD_EPARA;
    }

    retimer_id = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0)); 
    channel_id = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 1)); 
    
    ret = dal_get_specific_object_byte(CLASS_RETIMER_NAME, PROPERTY_RETIMER_ID, retimer_id, &ref_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get retimer object handle failed. retimer_id = %d.", retimer_id);
        return RET_ERR;
    }

    ret = retimer_switch_channel(ref_handle, RETIMER_CHN_WITCH_OPEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Retimer switch channel failed. retimer_id = %d.", retimer_id);
        return ret;
    }

    ret = retimer_get_rx_ctle(ref_handle, channel_id, data_info, BUFFER_MAX_LEN);
    if (ret != RET_OK) {
        debug_printf("Read rx ctle failed: %d. retimer_id = %d.", ret, retimer_id);
    } else if ((data_info[SMBUS_READ_DATA_OFFSET + 1] != 0) || (data_info[SMBUS_READ_DATA_OFFSET] != 0)) {
        debug_printf("Read rx ctle failed: %d-%d. retimer_id = %d.", data_info[SMBUS_READ_DATA_OFFSET],
            data_info[SMBUS_READ_DATA_OFFSET + 1], retimer_id);
    } else {
        for (i = 0; i < CTLE_ITEM_NUM; i++) {
            tmp_len = SMBUS_READ_DATA_OFFSET + CTLE_DATA_OFFSET + i;
            if (tmp_len >= BUFFER_MAX_LEN) {
                debug_log(DLOG_ERROR, "Array out of bounds when getting rx ctle. retimer_id = %d.", retimer_id);
                return RET_ERR;
            }
            debug_printf("%s: %d", fileld_name[i], data_info[tmp_len]);
        }
    }

    ret = retimer_switch_channel(ref_handle, RETIMER_CHN_WITCH_CLOSE);
    return ret;
}


gint32 retimer_get_tx_ffe_proc(GSList *input_list)
{
    guint i;
    gint32 ret;
    guint8 retimer_id;
    guint8 channel_id;
    gint8 data_info[BUFFER_MAX_LEN] = {};
    gchar* fileld_name[FFE_ITEM_MAX_NUM] = {"pre2", "pre1", "main", "post1", "post2"};
    guint32 tmp_len;
    OBJ_HANDLE ref_handle;

    if ((input_list != NULL) && (g_slist_length(input_list) < 2)) { 
        return MD_EPARA;
    }

    retimer_id = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0)); 
    channel_id = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 1)); 
    
    ret = dal_get_specific_object_byte(CLASS_RETIMER_NAME, PROPERTY_RETIMER_ID, retimer_id, &ref_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get retimer object handle failed. retimer_id = %d.", retimer_id);
        return RET_ERR;
    }

    ret = retimer_switch_channel(ref_handle, RETIMER_CHN_WITCH_OPEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Retimer switch channel failed. retimer_id = %d.", retimer_id);
        return ret;
    }

    ret = retimer_get_tx_ffe(ref_handle, channel_id, data_info, BUFFER_MAX_LEN);
    if (ret != RET_OK) {
        debug_printf("Read tx ffe failed: %d. retimer_id = %d.", ret, retimer_id);
    } else if ((data_info[SMBUS_READ_DATA_OFFSET + 1] != 0) || (data_info[SMBUS_READ_DATA_OFFSET] != 0)) {
        debug_printf("Read tx ffe failed: %d-%d. retimer_id = %d.", data_info[SMBUS_READ_DATA_OFFSET],
            data_info[SMBUS_READ_DATA_OFFSET + 1], retimer_id);
    } else {
        for (i = 0; i < FFE_ITEM_MAX_NUM; i++) {
            tmp_len = SMBUS_READ_DATA_OFFSET + FFE_DATA_OFFSET + i * sizeof(gint16);
            if (tmp_len + 1 >= BUFFER_MAX_LEN) { 
                debug_log(DLOG_ERROR, "Array out of bounds when getting tx ffe. retimer_id = %d.", retimer_id);
                return RET_ERR;
            }
            debug_printf("%s: %d", fileld_name[i], *(gint16 *)(void *)&data_info[tmp_len]);
        }
    }

    ret = retimer_switch_channel(ref_handle, RETIMER_CHN_WITCH_CLOSE);
    return ret;
}


gint32 retimer_set_channel_proc(GSList *input_list)
{
    gint32 ret;
    guint8 retimer_id;
    guint32 switch_flag;
    OBJ_HANDLE ref_handle;

    if ((input_list != NULL) && (g_slist_length(input_list) < 2)) { 
        return MD_EPARA;
    }

    retimer_id = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));    
    switch_flag = g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 1)); 
    
    ret = dal_get_specific_object_byte(CLASS_RETIMER_NAME, PROPERTY_RETIMER_ID, retimer_id, &ref_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get retimer object handle failed. retimer_id = %d.", retimer_id);
        return RET_ERR;
    }

    ret = retimer_switch_channel(ref_handle, switch_flag);
    if (ret != RET_OK) {
        debug_printf("Set channel failed: %d. retimer_id = %d.", ret, retimer_id);
    } else {
        debug_printf("Set channel success. retimer_id = %d.", retimer_id);
    }

    return RET_OK;
}


gint32 retimer_get_idle_proc(GSList *input_list)
{
    gint32 ret;
    guint8 retimer_id;
    gint8 data_info[BUFFER_MAX_LEN] = {};
    OBJ_HANDLE ref_handle;

    if ((input_list != NULL) && (g_slist_length(input_list) < 1)) { 
        return MD_EPARA;
    }

    retimer_id = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0)); 
    
    ret = dal_get_specific_object_byte(CLASS_RETIMER_NAME, PROPERTY_RETIMER_ID, retimer_id, &ref_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get retimer object handle failed. retimer_id = %d.", retimer_id);
        return RET_ERR;
    }

    ret = retimer_switch_channel(ref_handle, RETIMER_CHN_WITCH_OPEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Retimer switch channel failed. retimer_id = %d.", retimer_id);
        return ret;
    }

    ret = retimer_get_waite_flag(ref_handle, data_info, BUFFER_MAX_LEN);
    if (ret != RET_OK) {
        debug_printf("Get idle status failed: %d. retimer_id = %d.", ret, retimer_id);
    } else {
        debug_printf("Get idle status success. (0x%X). retimer_id = %d.", data_info[0], retimer_id);
    }

    ret = retimer_switch_channel(ref_handle, RETIMER_CHN_WITCH_CLOSE);
    return ret;
}


gint32 retimer_get_prbs_error_check_proc(GSList *input_list)
{
    gint32 ret;
    guint8 retimer_id;
    PRBS_ERROR_CHECK prbs = { 0 };
    gint8 data_info[BUFFER_MAX_LEN] = {};
    OBJ_HANDLE ref_handle;
    guint8 i;

    if ((input_list != NULL) && (g_slist_length(input_list) < 3)) { 
        return MD_EPARA;
    }

    retimer_id = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));      
    prbs.channel_id = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 1)); 
    prbs.lane_num = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 2));   
    
    ret = dal_get_specific_object_byte(CLASS_RETIMER_NAME, PROPERTY_RETIMER_ID, retimer_id, &ref_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get retimer object handle failed. retimer_id = %d.", retimer_id);
        return RET_ERR;
    }

    ret = retimer_switch_channel(ref_handle, RETIMER_CHN_WITCH_OPEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Retimer switch channel failed when getting prbs error. retimer_id = %d.", retimer_id);
        return ret;
    }

    ret = retimer_get_prbs_error(ref_handle, &prbs, data_info, BUFFER_MAX_LEN);
    if (ret != RET_OK) {
        debug_printf("Get prbs error check failed. retimer_id = %d.", retimer_id);
    } else {
        for (i = 0; i < prbs.lane_num; i++) {
            debug_printf("error_cnt_%d = %d, retimer_id = %d", i + prbs.channel_id,
                *(gint32 *)(void *)&data_info[i * sizeof(gint32)], retimer_id);
        }
    }

    ret = retimer_switch_channel(ref_handle, RETIMER_CHN_WITCH_CLOSE);
    return ret;
}


gint32 retimer_set_prbs_send_recv_proc(GSList *input_list)
{
    gint32 ret;
    guint8 retimer_id;
    PRBS_SEND_RECV prbs = { 0 };
    OBJ_HANDLE ref_handle;

    if ((input_list != NULL) && (g_slist_length(input_list) < 5)) { 
        return MD_EPARA;
    }

    retimer_id = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));      
    prbs.channel_id = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 1)); 
    prbs.lane_num = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 2));   
    prbs.prbs_type = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 3));  
    prbs.prbs_dir = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 4));   
    
    ret = dal_get_specific_object_byte(CLASS_RETIMER_NAME, PROPERTY_RETIMER_ID, retimer_id, &ref_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get retimer object handle failed. retimer_id = %d.", retimer_id);
        return RET_ERR;
    }

    ret = retimer_switch_channel(ref_handle, RETIMER_CHN_WITCH_OPEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Retimer switch channel failed when setting prbs. retimer_id = %d.", retimer_id);
        return ret;
    }

    ret = retimer_prbs_send_recv(ref_handle, &prbs);
    if (ret != RET_OK) {
        debug_printf("Set prbs send recv failed: %d. retimer_id = %d.", ret, retimer_id);
    } else {
        debug_printf("Set prbs send recv success. retimer_id = %d.", retimer_id);
    }

    ret = retimer_switch_channel(ref_handle, RETIMER_CHN_WITCH_CLOSE);
    return ret;
}


gint32 retimer_set_tx_swing_proc(GSList *input_list)
{
    gint32 ret;
    guint8 retimer_id;
    SET_TX_SWING tx_swing = { 0 };
    OBJ_HANDLE ref_handle;

    if ((input_list != NULL) && (g_slist_length(input_list) < 4)) { 
        return MD_EPARA;
    }

    retimer_id = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));          
    tx_swing.channel_id = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 1)); 
    tx_swing.lane_num = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 2));   
    tx_swing.swing = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 3));      
    
    ret = dal_get_specific_object_byte(CLASS_RETIMER_NAME, PROPERTY_RETIMER_ID, retimer_id, &ref_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get retimer object handle failed. retimer_id = %d.", retimer_id);
        return RET_ERR;
    }

    ret = retimer_switch_channel(ref_handle, RETIMER_CHN_WITCH_OPEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Retimer switch channel failed when setting tx swing. retimer_id = %d.", retimer_id);
        return ret;
    }

    ret = retimer_set_tx_swing(ref_handle, &tx_swing);
    if (ret != RET_OK) {
        debug_printf("Set all lane tx swing failed: %d. retimer_id = %d.", ret, retimer_id);
    } else {
        debug_printf("Set all lane tx swing success. retimer_id = %d.", retimer_id);
    }

    ret = retimer_switch_channel(ref_handle, RETIMER_CHN_WITCH_CLOSE);
    return ret;
}


gint32 retimer_turn_routine_on_or_off_proc(GSList *input_list)
{
    guint8 open_close;

    if ((input_list != NULL) && (g_slist_length(input_list) < 1)) { 
        return RET_ERR;
    }

    open_close = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0)); 
    write_retimer_routine_open_status(open_close);

    return RET_OK;
}


gint32 method_retimer_reg_read_write(GSList *input_list)
{
    OBJ_HANDLE object_handle = 0;
    const gchar *retimer_name = NULL;
    gsize parm_temp = 0;
    gint8 resp_data_buf[RSP_MAX_BUF_LEN] = {0};
    guint32 reg_value;

    if (input_list == NULL) {
        debug_printf("param is null!\n");
        return RET_ERR;
    }
    
    if (g_slist_length(input_list) != 4) {
        debug_printf("Usage: retimer_reg_read_write [retimer name] [opration:0 read, 1 write] [offset] [value]\n\
            Read or write retimer register\n");
        return RET_ERR;
    }
    
    retimer_name = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 0), &parm_temp);
    
    guint32 op = g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 1));
    
    guint32 offset = g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 2));
    
    guint32 value = g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 3));

    gint32 ret = dfl_get_object_handle(retimer_name, &object_handle);
    if (ret != DFL_OK) {
        debug_printf("get %s handle failed", retimer_name);
        return ret;
    }

    if (retimer_get_all_status_ready() != RETIMER_HAS_READY) {
        debug_printf("retimer is not ready\n");
        return RET_OK;
    }
    if (retimer_channel_switch_by_handle(object_handle, RETIMER_CHN_OPEN) != RET_OK) {
        debug_printf("switch channel failed\n");
        return RET_ERR;
    }

    if (!op) {
        ret = retimer_get_reg_info(object_handle, offset, resp_data_buf, RSP_MAX_BUF_LEN);
        if (ret != RET_OK) {
            debug_printf("read %s[0x%08x] failed, ret = %d", retimer_name, offset, ret);
        } else {
            reg_value = *(guint32 *)(void *)&resp_data_buf[SMBUS_READ_DATA_OFFSET];
            debug_printf("value of %s[0x%08x] is 0x%08x", retimer_name, offset, reg_value);
        }
    } else {
        ret = retimer_set_reg_info(object_handle, offset, value);
        if (ret != RET_OK) {
            debug_printf("write 0x%08x to %s[0x%08x] failed, ret = %d", value, retimer_name, offset, ret);
        } else {
            debug_printf("write 0x%08x to %s[0x%08x] successfully", value, retimer_name, offset);
        }
    }

    return RET_OK;
}