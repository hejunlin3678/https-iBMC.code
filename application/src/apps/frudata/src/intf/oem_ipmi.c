

#include "intf/oem_ipmi.h"
#include "prepare_fru.h"
#include "media/port_rate.h"
#include "media/e2p_compact.h"


LOCAL gint32 set_board_port_speed_single_cpu(gconstpointer msg_data, gpointer user_data)
{
    
    gint32 ret;
    const BOARD_SET_PORT_SPEED_REQ_S *rev_data = (const BOARD_SET_PORT_SPEED_REQ_S *)get_ipmi_src_data(msg_data);
    if (rev_data == NULL) {
        debug_log(DLOG_ERROR, "Get ipmi src data failed!");
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    if (rev_data->port_speed == INVALID_VALUE8) {
        ret = dal_write_cpld_byte(1, CPLD_BOARD_PORT_SPEED_ADDR, 0);
        debug_log(DLOG_ERROR, "The port_speed is invalid, set 0 to cpld ret[%d]", ret);
        return ipmi_send_simple_response(msg_data, COMP_CODE_OUTOF_RANGE);
    }

    ret = set_board_port_speed(rev_data->port_speed);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set baord_port_speed to cpld failed, ret=%d.", ret);
        ipmi_operation_log(msg_data, "Set baord port speed to cpld failed.");
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }

    ipmi_operation_log(msg_data, "Set baord port speed successfully.");
    return ipmi_send_simple_response(msg_data, COMP_CODE_SUCCESS);
}


LOCAL gint32 get_board_port_speed_single_cpu(gconstpointer msg_data, gpointer user_data)
{
    
    const guint8 *rev_data = (const guint8 *)get_ipmi_src_data(msg_data);
    if (rev_data == NULL) {
        debug_log(DLOG_ERROR, "Get ipmi src data failed!");
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    BOARD_GET_PORT_SPEED_S resp_data = { 0 };
    resp_data.comp_code = COMP_CODE_SUCCESS;
    resp_data.port_speed = get_board_port_speed();
    return ipmi_send_response(msg_data, sizeof(resp_data), (guint8 *)&resp_data);
}


typedef enum { 
    PANGEA_PORT_SPEED_ARCTIC_1GE = 0x01, 
    PANGEA_PORT_SPEED_ARCTIC_10GE = 0x02, 
    PANGEA_PORT_SPEED_10GE = 0x03, 
    PANGEA_PORT_SPEED_25GE = 0x04 
} PANGEA_PORT_SPEED_INFO;


LOCAL gint32 set_lom_port_speed(gconstpointer msg_data, gpointer user_data)
{
    
    gint32 ret = RET_ERR;
    const BOARD_SET_PORT_SPEED_REQ_S *rev_data = (const BOARD_SET_PORT_SPEED_REQ_S *)get_ipmi_src_data(msg_data);
    if (rev_data == NULL) {
        debug_log(DLOG_ERROR, "get ipmi src data failed.");
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    switch (rev_data->port_speed) {  
        case PANGEA_PORT_SPEED_ARCTIC_1GE:
        case PANGEA_PORT_SPEED_ARCTIC_10GE:
        case PANGEA_PORT_SPEED_10GE:
        case PANGEA_PORT_SPEED_25GE:
            ret = set_cpu_lom_speed(rev_data->cpuid, rev_data->port_speed);
            break;
        default:
            debug_log(DLOG_ERROR, "the port speed is invalid, speed=%d.", rev_data->port_speed);
            return ipmi_send_simple_response(msg_data, COMP_CODE_OUTOF_RANGE);
    }

    if (ret != RET_OK) {
        ipmi_operation_log(msg_data, "Set board port speed failed.");
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }
    ipmi_operation_log(msg_data, "Set board port speed successfully.");
    return ipmi_send_simple_response(msg_data, COMP_CODE_SUCCESS);
}


gint32 ipmi_set_board_port_speed(gconstpointer msg_data, gpointer user_data)
{
    
    if (msg_data == NULL) {
        debug_log(DLOG_ERROR, "input parameter is null.");
        return RET_ERR;
    }
    guint8 lens = get_ipmi_src_data_len(msg_data);
    
    if (get_cpu_num() == 1 && lens == 6) { 
        return set_board_port_speed_single_cpu(msg_data, user_data);
    }
    if (get_cpu_num() > 1 && lens == 7) { 
        return set_lom_port_speed(msg_data, user_data);
    }
    ipmi_operation_log(msg_data, "Set board port speed failed.");
    return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
}


LOCAL gint32 get_lom_port_speed(gconstpointer msg_data, gpointer user_data)
{
    
    const GET_MULTIPLE_CPUS_PORT_SPEED_S *rev_data =
        (const GET_MULTIPLE_CPUS_PORT_SPEED_S *)get_ipmi_src_data(msg_data);
    if (rev_data == NULL) {
        debug_log(DLOG_ERROR, "get ipmi src data failed.");
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    if (rev_data->cpuid >= get_cpu_num()) {
        debug_log(DLOG_ERROR, "get cpuid is not valid, cpuid=%d.", rev_data->cpuid);
        return ipmi_send_simple_response(msg_data, COMP_CODE_OUTOF_RANGE);
    }

    BOARD_GET_PORT_SPEED_S resp_data = { 0 };
    resp_data.comp_code = COMP_CODE_SUCCESS;
    resp_data.port_speed = get_lom_port_speed_by_cpuid(rev_data->cpuid);
    return ipmi_send_response(msg_data, sizeof(resp_data), (guint8 *)&resp_data);
}


gint32 ipmi_get_board_port_speed(gconstpointer msg_data, gpointer user_data)
{
    
    if (msg_data == NULL) {
        debug_log(DLOG_ERROR, "input parameter is null.");
        return RET_ERR;
    }
    guint8 lens = get_ipmi_src_data_len(msg_data);
    
    if (get_cpu_num() == 1 && lens == 5) { 
        return get_board_port_speed_single_cpu(msg_data, user_data);
    }
    if (get_cpu_num() > 1 && lens == 6) { 
        return get_lom_port_speed(msg_data, user_data);
    }
    debug_log(DLOG_ERROR, "get board speed data is out of range.");
    return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
}

#ifdef DFT_ENABLED

LOCAL guint8 get_optical_scan_status(void)
{
    GSList *output_list = NULL;
    OBJ_HANDLE obj_handle;
    guint8 status = 0xff;

    gint32 ret = dal_get_object_handle_nth(CLASS_OPTICAL_MODULE, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s get opticalmodule handle failed ret(%d)", __FUNCTION__, ret);
        return status;
    }

    ret = dfl_call_class_method(obj_handle, METHOD_OPT_MDL_GET_SCAN_STATUS, NULL, NULL, &output_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s call method(%s) failed ret(%d)", __FUNCTION__, METHOD_OPT_MDL_GET_SCAN_STATUS, ret);
        return status;
    }

    status = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 0)); 
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

    return status;
}

LOCAL gint32 set_optical_scan_status(guint8 status)
{
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle;

    gint32 ret = dal_get_object_handle_nth(CLASS_OPTICAL_MODULE, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s get opticalmodule handle failed ret(%d)", __FUNCTION__, ret);
        return ret;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(status));
    ret = dfl_call_class_method(obj_handle, METHOD_OPT_MDL_SET_SCAN_STATUS, NULL, input_list, NULL);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s call method(%s) failed ret(%d)", __FUNCTION__, METHOD_OPT_MDL_SET_SCAN_STATUS, ret);
        return RET_ERR;
    }

    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);

    return ret;
}


gint32 ipmi_cmd_set_scan_status(gconstpointer req_msg, gpointer user_data)
{
    gint32 ret;

    
    if (req_msg == NULL) {
        debug_log(DLOG_DEBUG, "%s error: req_msg is null ", __FUNCTION__);
        return RET_ERR;
    }

    const BMC_SET_SCAN_STATUS_REQ_MSG_S *req_data = (const BMC_SET_SCAN_STATUS_REQ_MSG_S *)get_ipmi_src_data(req_msg);
    if (req_data == NULL) {
        debug_log(DLOG_DEBUG, "%s error: req_data is null ", __FUNCTION__);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }

    if (req_data->status != ENABLE && req_data->status != DISABLE) {
        debug_log(DLOG_ERROR, "%s error: status %d is invalid ", __FUNCTION__, req_data->status);
        ipmi_operation_log(req_msg, "Set Set scan status failed.");
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }

    switch (req_data->device_type) {
        case COMPONENT_TYPE_CHASSIS_BACKPLANE:
            ret = frudata_set_eeprom_scan_status(req_data->status);
            break;
        case COMPONENT_TYPE_PORT:
            ret = set_optical_scan_status(req_data->status);
            break;
        default:
            debug_log(DLOG_ERROR, "%s error: device type %d is invalid ", __FUNCTION__, req_data->device_type);
            ipmi_operation_log(req_msg, "Set Set scan status failed.");
            return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }

    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: device type %d set scan status %d failed.", __FUNCTION__, req_data->device_type,
            req_data->status);
        ipmi_operation_log(req_msg, "Set Set scan status failed.");
        return ipmi_send_simple_response(req_msg, COMP_CODE_UNKNOWN);
    }

    debug_log(DLOG_ERROR, "%s: device type %d set scan status %d successfully.", __FUNCTION__, req_data->device_type,
        req_data->status);
    ipmi_operation_log(req_msg, "Set scan status %s successfully.", (req_data->status == ENABLE) ? "start" : "stop");
    return ipmi_send_simple_response(req_msg, COMP_CODE_SUCCESS);
}


gint32 ipmi_cmd_get_scan_status(gconstpointer req_msg, gpointer user_data)
{
    
    if (req_msg == NULL) {
        debug_log(DLOG_DEBUG, "%s error: req_msg is null ", __FUNCTION__);
        return RET_ERR;
    }

    const BMC_GET_SCAN_STATUS_REQ_MSG_S *req_data = (const BMC_GET_SCAN_STATUS_REQ_MSG_S *)get_ipmi_src_data(req_msg);
    if (req_data == NULL) {
        debug_log(DLOG_DEBUG, "%s error: req_data is null ", __FUNCTION__);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }

    BMC_GET_SCAN_STATUS_RSP_MSG_S resp_data;
    switch (req_data->device_type) {
        case COMPONENT_TYPE_CHASSIS_BACKPLANE:
            resp_data.status = ((frudata_get_eeprom_scan_status() == SCAN_STOPPED) ? DISABLE : ENABLE);
            break;
        case COMPONENT_TYPE_PORT:
            resp_data.status = get_optical_scan_status();
            break;
        default:
            debug_log(DLOG_ERROR, "%s error: device type %d is invalid ", __FUNCTION__, req_data->device_type);
            return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }

    resp_data.compcode = COMP_CODE_SUCCESS;
    return ipmi_send_response(req_msg, sizeof(resp_data), (guint8 *)&resp_data);
}
#endif


LOCAL gint32 pwr_chassis_get_elabel(guint8 fru_id, gint16 frame_num, guint8 *data, guint32 len, guint32 *read_len)
{
    gint32 ret;
    guint16 offset;
    OBJ_HANDLE fru_handle;
    OBJ_HANDLE dev_handle;
    LOCAL guint8 origin_fru_data[MAX_ELABLE_LEN];

    if (frame_num <= 0 || frame_num > MAX_ELABEL_FRAME_NUM) {
        debug_log(DLOG_ERROR, "Input error param, invalid frame num(%d).", frame_num);
        return COMP_CODE_OUTOF_RANGE;
    }

    
    ret = dal_get_specific_object_byte(CLASS_FRU, PROPERTY_FRU_ID, fru_id, &fru_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get fru obj(fru id=%d) failed, ret=%d.", fru_id, ret);
        return COMP_CODE_INVALID_FIELD;
    }

    ret = dfl_get_referenced_object(fru_handle, PROPERTY_FRUDEV_RO, &dev_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s get ref fru dev failed, ret=%d.", dfl_get_object_name(fru_handle), ret);
        return COMP_CODE_INVALID_FIELD;
    }

    if (frame_num == 1) { 
        (void)memset_s(origin_fru_data, MAX_ELABLE_LEN, 0, MAX_ELABLE_LEN);
        ret = dfl_block_read(dev_handle, PROPERTY_FRUDEV_STORAGE_LOC, 0, MAX_ELABLE_LEN, origin_fru_data);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "Get power(%d) elabel from eeprom failed.", fru_id);
            return COMP_CODE_CANNOT_RET_DATA;
        }
    }

    
    *read_len = (frame_num == 5) ? (MAX_ELABLE_LEN - GET_ELABEL_DATA_SIZE_PER_FRAME * 4) : len;
    offset = GET_ELABEL_DATA_SIZE_PER_FRAME * (frame_num - 1);

    ret = memcpy_s(data, len, origin_fru_data + offset, *read_len);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "memcpy_s data to dest buf failed, ret=%d.", ret);
        ret = COMP_CODE_INVALID_FIELD;
    }

    return ret;
}


gint32 dft_get_huawei_formate_elabel(const void *msg_data, gpointer user_data)
{
    gint32 ret;
    guint8 fru_id;
    guint32 read_len;
    guint16 frame_num;
    const guint8 *src_data = NULL;
    const IPMIMSG_ELABELHEAD *msg_head = NULL;
    guint8 one_frame_elabel[GET_ELABEL_DATA_SIZE_PER_FRAME + 2] = { 0 }; 

    
    if (msg_data == NULL) {
        return RET_ERR;
    }
    src_data = get_ipmi_src_data(msg_data);
    if (src_data == NULL) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    msg_head = (const IPMIMSG_ELABELHEAD *)src_data;
    frame_num = msg_head->cur_num;

    
    fru_id = msg_head->fru_id;
    
    ret = pwr_chassis_get_elabel(fru_id, frame_num, &one_frame_elabel[2],
        GET_ELABEL_DATA_SIZE_PER_FRAME, &read_len);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get power chassis(fru_id=%d) elabel failed, ret=%d", fru_id, ret);
        return ipmi_send_simple_response(msg_data, ret);
    }

    one_frame_elabel[0] = COMP_CODE_SUCCESS;
    one_frame_elabel[1] = read_len;
    return ipmi_send_response(msg_data, (guint8)sizeof(one_frame_elabel), one_frame_elabel);
}