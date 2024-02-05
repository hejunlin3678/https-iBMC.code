



#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "pme/haslib/haslib.h"
#include "exp_board.h"
#include "get_version.h"
#include "pme_app/common/debug_log_macro.h"
#define HW_ACCESS_FAIL_MASK 0x8000


LOCAL gint32 process_each_exp_board(OBJ_HANDLE handle, gpointer data)
{
    gint32 ret = 0;

    
    ret = get_pcb_version(handle, PROPERTY_EXPBOARD_PCBID, PROPERTY_EXPBOARD_PCBVER);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    
    ret = get_logic_version(handle, PROPERTY_EXPBOARD_LOGICVERID, PROPERTY_EXPBOARD_LOGICVER);
    if (ret != RET_OK) {
        return RET_ERR;
    }
    return RET_OK;
}


gint32 exp_board_add_object_callback(OBJ_HANDLE object_handle)
{
    gchar class_nm[MAX_NAME_SIZE] = {0};
    gint32 ret;

    ret = dfl_get_class_name(object_handle, class_nm, MAX_NAME_SIZE);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get object(%s) class name failed.", dfl_get_object_name(object_handle));
        return ret;
    }

    if (!strcmp(CLASS_EXPBOARD_NAME, class_nm)) {
        card_manage_log_operation_log("Exp board", object_handle, PROPERTY_EXPBOARD_SLOT, NULL, CARD_PLUG_IN);
        return process_each_exp_board(object_handle, NULL);
    }

    return RET_ERR;
}


gint32 exp_board_del_object_callback(OBJ_HANDLE object_handle)
{
    gchar class_nm[MAX_NAME_SIZE] = {0};
    gint32 ret;

    ret = dfl_get_class_name(object_handle, class_nm, MAX_NAME_SIZE);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get object(%s) class name failed.", dfl_get_object_name(object_handle));
        return ret;
    }

    if (!strcmp(CLASS_EXPBOARD_NAME, class_nm)) {
        card_manage_log_operation_log("Exp board", object_handle, PROPERTY_EXPBOARD_SLOT, NULL, CARD_PLUG_OUT);
        return RET_OK;
    }

    
    if (!strcmp(CLASS_SAS_EXPBOARD_NAME, class_nm)) {
        card_manage_log_operation_log("SAS Exp board", object_handle, PROPERTY_SAS_EXPBOARD_SLOTID, NULL,
            CARD_PLUG_OUT);
        return RET_OK;
    }

    return RET_ERR;
}


gint32 exp_board_init(void)
{
    (void)dfl_foreach_object(CLASS_EXPBOARD_NAME, process_each_exp_board, NULL, NULL);

    return RET_OK;
}


gint32 hdd_board_method_sync_property(const OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    
    return property_sync_common_handler_ext(obj_handle, caller_info, input_list, output_list, DF_NONE);
}


LOCAL guint16 hdd_board_get_property(OBJ_HANDLE obj_handle, const gchar *prop_name)
{
    gint32 ret;
    guint16 value = 0x0;
    OBJ_HANDLE ref_obj_handle = 0;
    gchar ref_obj_name[MAX_NAME_SIZE] = {0};
    gchar ref_prop_name[MAX_NAME_SIZE] = {0};

    ret = dfl_get_referenced_property(obj_handle, prop_name, ref_obj_name, ref_prop_name, MAX_NAME_SIZE, MAX_NAME_SIZE);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: dfl_get_referenced_property failed, ret: %d", __FUNCTION__, ret);
        return HW_ACCESS_FAIL_MASK;
    }

    ret = dfl_get_object_handle(ref_obj_name, &ref_obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: dfl_get_object_handle failed, ret: %d", __FUNCTION__, ret);
        return HW_ACCESS_FAIL_MASK;
    }

    ret = dal_get_property_value_uint16(ref_obj_handle, ref_prop_name, &value);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_uint16 failed, ret: %d", __FUNCTION__, ret);
        return HW_ACCESS_FAIL_MASK;
    }

    return value;
}


LOCAL gint32 hdd_board_obj_scan(OBJ_HANDLE obj_handle, gpointer user_data)
{
    guint16 value;

    
    value = hdd_board_get_property(obj_handle, PROPERTY_BASEBOARD_RAW_TEMP);
    (void)dal_set_property_value_uint16(obj_handle, PROPERTY_BASEBOARD_SYNC_RAW_TEMP, value, DF_NONE);

    
    value = hdd_board_get_property(obj_handle, PROPERTY_BASEBOARD_IIC_STATE);
    if (value == HW_ACCESS_FAIL_MASK) {
        value = 0; 
    }
    (void)dal_set_property_value_uint16(obj_handle, PROPERTY_BASEBOARD_SYNC_IIC_STATE, value, DF_NONE);
    return RET_OK;
}


LOCAL void hdd_board_scan_task(void)
{
    guint8 software_type = 0;
    guint8 as_status = DEFAULT_AS_STATUS;

    
    (void)dal_get_software_type(&software_type);
    if (software_type != MGMT_SOFTWARE_TYPE_PANGEA_PACIFIC_CTRL) {
        return;
    }

    (void)prctl(PR_SET_NAME, (unsigned long)"HddBoardScanTask");

    for (;;) {
        vos_task_delay(2000); 

        
        (void)get_board_active_state(&as_status);
        if (as_status != ACTIVE_STATE) {
            debug_log(DLOG_DEBUG, "current board is not master.");
            continue;
        }

        (void)dfl_foreach_object(CLASS_HDD_BASEBOARD, hdd_board_obj_scan, NULL, NULL);
    }
}


LOCAL void clear_exp_alarm_led(OBJ_HANDLE handle)
{
    gint32 ret;
    guint8 cur_led_state = 1; 

    
    (void)dal_get_extern_value_byte(handle, PROPERTY_SAS_EXPBOARD_LOCALLED, &cur_led_state, DF_AUTO);
    if (cur_led_state == 1) {
        return;
    }

    
    ret = dal_set_property_value_byte(handle, PROPERTY_SAS_EXPBOARD_ALARMLED, 0, DF_HW);
    if (ret != RET_OK) {
        return;
    }

    
    ret = dal_set_property_value_byte(handle, PROPERTY_SAS_EXPBOARD_LOCALLED, 1, DF_AUTO);
    if (ret != RET_OK) {
        return;
    }

    maintenance_log_v2(MLOG_INFO, FC__PUBLIC_OK, "Clear %s alarm led off sucess!", dfl_get_object_name(handle));
}


LOCAL void set_exp_alarm_led(OBJ_HANDLE handle)
{
    gint32 ret;
    guint8 cur_led_state = 1; 

    
    (void)dal_get_extern_value_byte(handle, PROPERTY_SAS_EXPBOARD_ALARMLED, &cur_led_state, DF_HW);
    if (cur_led_state == 1) {
        return;
    }

    
    ret = dal_set_property_value_byte(handle, PROPERTY_SAS_EXPBOARD_LOCALLED, 0, DF_AUTO);
    if (ret != RET_OK) {
        return;
    }

    
    ret = dal_set_property_value_byte(handle, PROPERTY_SAS_EXPBOARD_ALARMLED, 1, DF_HW);
    if (ret != RET_OK) {
        return;
    }

    maintenance_log_v2(MLOG_INFO, FC__PUBLIC_OK, "Set %s alarm led on sucess!", dfl_get_object_name(handle));
}


LOCAL void update_exp_board_alarm_led(OBJ_HANDLE handle)
{
    gint32 ret;
    guint8 cur_health;
    guint16 i2c_status;

    
    ret = dal_get_property_value_uint16(handle, PROPERTY_SAS_EXPBOARD_SYNC_I2C, &i2c_status);
    if ((ret != RET_OK) || (i2c_status != 0)) {
        debug_log(DLOG_INFO, "Get %s i2c_status(%d) abnoraml, ret=%d.", dfl_get_object_name(handle), i2c_status, ret);
        return;
    }

    
    ret = dal_get_property_value_byte(handle, PROPERTY_SAS_EXPBOARD_HEALTH, &cur_health);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "Get %s health state is failed, ret=%d.", dfl_get_object_name(handle), ret);
        return;
    }

    if (cur_health == 0) {
        clear_exp_alarm_led(handle);
    } else if (cur_health == 1) {
        set_exp_alarm_led(handle);
    } else {
        debug_log(DLOG_INFO, "Exp board health state(%d) is invalid.", cur_health);
    }
}


LOCAL void update_sas_exp_sync_data(OBJ_HANDLE handle)
{
    
    guint16 max_temp = 20;
    gint32 ret = dal_get_property_value_uint16(handle, PROPERTY_SAS_EXPBOARD_MAXTEMP, &max_temp);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "[%s]: get sas expboard [%s] max temp failed, ret: %d.", __FUNCTION__,
            dfl_get_object_name(handle), ret);
        return;
    }
    (void)dal_set_property_value_uint16(handle, PROPERTY_SAS_EXPBOARD_SYNC_MAXTEMP, max_temp, DF_NONE);

    
    guint16 board_pg = 0;
    ret = dal_get_property_value_uint16(handle, PROPERTY_SAS_EXPBOARD_PG, &board_pg);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "[%s]: get sas expboard [%s] power good failed, ret: %d.", __FUNCTION__,
            dfl_get_object_name(handle), ret);
        return;
    }
    (void)dal_set_property_value_uint16(handle, PROPERTY_SAS_EXPBOARD_SYNC_PG, board_pg, DF_NONE);

    
    guint16 board_5v_pg = 0;
    ret = dal_get_property_value_uint16(handle, PROPERTY_SAS_EXPBOARD_5V_PG, &board_5v_pg);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "[%s]: get sas expboard [%s] 5v power good failed, ret: %d.", __FUNCTION__,
            dfl_get_object_name(handle), ret);
        return;
    }
    (void)dal_set_property_value_uint16(handle, PROPERTY_SAS_EXPBOARD_SYNC_5V_PG, board_5v_pg, DF_NONE);

    
    guint16 i2c_status = 0;
    ret = dal_get_property_value_uint16(handle, PROPERTY_SAS_EXPBOARD_I2C, &i2c_status);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "[%s]: get sas expboard [%s] i2c status failed, ret: %d.", __FUNCTION__,
            dfl_get_object_name(handle), ret);
        return;
    }
    (void)dal_set_property_value_uint16(handle, PROPERTY_SAS_EXPBOARD_SYNC_I2C, i2c_status, DF_NONE);
    return;
}


LOCAL gint32 send_exp_core_cmd(OBJ_HANDLE obj_handle, CTRL_EXP_CHIP_CMD_E cmd, guint8 *read_buf, guint8 buf_len)
{
    SAS_EXPBOARD_CHIP_OP_S send_data;
    guint8 *recv_buf = NULL;
    OBJ_HANDLE tmp_hnd = 0;

    
    send_data.addr = SAS_EXPBOARD_CORE_CHIP_ADDR;
    send_data.cmd = SAS_EXPBOARD_CORE_CHIP_QUERY_CMD;
    send_data.cnt = SAS_EXPBOARD_CORE_CHIP_DATA_LEN;
    send_data.subCmdLow = cmd;
    send_data.subCmdHigh = SAS_EXPBOARD_CORE_CHIP_CMD_DEFAULT;
    send_data.cmdTag = SAS_EXPBOARD_CORE_CHIP_CMD_TAG;

    
    gint32 ret = dfl_get_referenced_object(obj_handle, PROPERTY_SAS_EXPBOARD_CORECHIP, &tmp_hnd);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "Get chip object of expboard [%s] failed, ret=%d", dfl_get_object_name(obj_handle), ret);
        return ret;
    }

    const gchar *chip_name = dfl_get_object_name(tmp_hnd);

    
    send_data.pec = dal_cal_crc8(0, (guint8 *)&send_data, 6); 

    
    recv_buf = (guint8 *)g_malloc0(READ_DATA_COM_LEN + buf_len);
    if (recv_buf == NULL) {
        debug_log(DLOG_ERROR, "Expboard [%s] recv_buf g_malloc0 failed", dfl_get_object_name(obj_handle));
        return RET_ERR;
    }

    
    ret = dfl_chip_blk_write_read(chip_name, 6, &send_data.cmd, READ_DATA_COM_LEN + buf_len, recv_buf); 
    if (ret != DFL_OK) {
        debug_log_limit_when_fail(DLOG_LIMIT_60, TRUE, "Send cmd to expboard [%s] failed, ret=%d",
                                  dfl_get_object_name(obj_handle), ret);
        goto EXIT;
    }

    
    guint8 crc = dal_cal_crc8(0, recv_buf, READ_DATA_COM_LEN + buf_len - 1); 
    if (crc != recv_buf[READ_DATA_COM_LEN + buf_len - 1]) {
        ret = RET_ERR;
        debug_log_limit_when_fail(DLOG_LIMIT_60, TRUE, "Check expboard [%s] crc failed(calc=0x%x, real=0x%x)",
                                  dfl_get_object_name(obj_handle), crc, recv_buf[buf_len - 1]);
        goto EXIT;
    }

    
    guint16 err_code = MAKE_WORD(recv_buf[3], recv_buf[2]); 
    if (err_code != 0) {
        debug_log(DLOG_ERROR, "Expboard [%s] core chip execute cmd failed, err_code=%d",
                  dfl_get_object_name(obj_handle), err_code);
        goto EXIT;
    }

    ret = memcpy_s(read_buf, buf_len, &recv_buf[4], buf_len); 
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Expboard [%s] memcpy_s data to read_buf failed, ret=%d",
                  dfl_get_object_name(obj_handle), ret);
    }

 EXIT:
    g_free(recv_buf);
    return ret;
}


LOCAL void get_exp_core_data(OBJ_HANDLE obj_handle)
{
    guint16 exp_board_id;
    guint8 core_temp[READ_TEMP_LEN] = {0};
    guint8 core_status;
    guint16 temp_value;

    
    gint32 ret = dal_get_property_value_uint16(obj_handle, PROPERTY_SAS_EXPBOARD_BOARDID, &exp_board_id);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "Get expboard [%s] board id failed, ret: %d",
                  dfl_get_object_name(obj_handle), ret);
        return;
    }

    
    if ((exp_board_id != ARCTIC_SAS_EXPBOARD_V2_BOARDID) &&
        (exp_board_id != PACIFIC_SAS_EXPBOARD_V2_BOARDID) && (exp_board_id != PAC1220V6_SAS_EXPBOARD_V2_BOARDID)) {
        debug_log(DLOG_DEBUG, "Expboard [%s] doesn't have core data, board id: %d",
                  dfl_get_object_name(obj_handle), exp_board_id);
        return;
    }

    
    ret = send_exp_core_cmd(obj_handle, READ_TEMP_CMD, core_temp, READ_TEMP_LEN);
    if (ret != RET_OK) {
        debug_log_limit_when_fail(DLOG_LIMIT_60, TRUE, "Get expboard [%s] core temp failed, ret=%d",
                                  dfl_get_object_name(obj_handle), ret);
        
        temp_value = SENSOR_INVALID_READING;
        ret = dal_set_property_value_uint16(obj_handle, PROPERTY_SAS_EXPBOARD_CORETEMP, temp_value, DF_NONE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Set expboard [%s] core default temp failed, core temp=%d, ret=%d",
                      dfl_get_object_name(obj_handle), temp_value, ret);
        }
        return;
    }

    
    temp_value = MAKE_WORD(core_temp[1], core_temp[0]); 
    ret = dal_set_property_value_uint16(obj_handle, PROPERTY_SAS_EXPBOARD_CORETEMP, temp_value, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set expboard [%s] core temp failed, core temp=%d, ret=%d",
                  dfl_get_object_name(obj_handle), temp_value, ret);
    }

    
    ret = send_exp_core_cmd(obj_handle, READ_STATUS_CMD, &core_status, READ_STATUS_LEN);
    if (ret != RET_OK) {
        debug_log_limit_when_fail(DLOG_LIMIT_60, TRUE, "Get expboard [%s] core status failed, ret=%d",
                                  dfl_get_object_name(obj_handle), ret);
        return;
    }

    
    ret = dal_set_property_value_byte(obj_handle, PROPERTY_SAS_EXPBOARD_CORESTATUS, core_status, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set expboard [%s] core status failed, core status=%d, ret=%d",
                  dfl_get_object_name(obj_handle), core_status, ret);
    }
}


LOCAL void set_exp_core_data_na(OBJ_HANDLE obj_handle)
{
    gint32 ret = dal_set_property_value_uint16(obj_handle, PROPERTY_SAS_EXPBOARD_CORETEMP, SENSOR_NA_READING, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set expboard [%s] core temp NA failed, core temp=%d, ret=%d",
                  dfl_get_object_name(obj_handle), SENSOR_NA_READING, ret);
    }
}



LOCAL void update_exp_core_data(OBJ_HANDLE obj_handle)
{
    gint32 ret;
    guint8 power_status;

    
    if (is_smm_active() == FALSE) {
        return;
    }

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SAS_EXPBOARD_CORE_CHIP_PG, &power_status);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get expboard [%s] power state of the core chip failed, ret: %d",
                  dfl_get_object_name(obj_handle), ret);
        return;
    }

    
    if (power_status == SAS_EXPBOARD_CORE_CHIP_POWER_ON) {
        get_exp_core_data(obj_handle);
    } else {
        
        set_exp_core_data_na(obj_handle);
    }
}


LOCAL gint32 update_sas_exp_info(OBJ_HANDLE handle, gpointer data)
{
    (void)data;

    if (!is_smm_active()) { 
        
        if (!dal_match_software_type(MGMT_SOFTWARE_TYPE_PANGEA_ARCTIC_CTRL)) {
            debug_log(DLOG_DEBUG, "this is slave board, can't opt expboard.");
            return RET_OK;
        }
    }

    guint8 presence = 0xff;
    gint32 ret = dal_get_property_value_byte(handle, PROPERTY_SAS_EXPBOARD_PRESENCE, &presence);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get sas expboard [%s] presence failed, ret %d",
                  dfl_get_object_name(handle), ret);
        return RET_OK;
    }

    if (presence != 1) {
        debug_log(DLOG_DEBUG, "expboard [%s] presence state(%u) error, do nothing.",
            dfl_get_object_name(handle), presence);
        return RET_OK;
    }

    update_sas_exp_sync_data(handle);
    update_exp_board_alarm_led(handle);
    update_exp_core_data(handle);

    return RET_OK;
}


LOCAL void exp_board_scan_task(void)
{
    (void)prctl(PR_SET_NAME, (unsigned long)"ExpBoardScanTask");

    while (1) {
        vos_task_delay(2000); 
        (void)dfl_foreach_object(CLASS_SAS_EXPBOARD_NAME, update_sas_exp_info, NULL, NULL);
    }
}


gint32 exp_board_start(void)
{
    gint32 ret;
    TASKID exp_scan_task_id = 0;
    TASKID hdd_scan_task_id = 0;
    ret = vos_task_create(&exp_scan_task_id, "exp_board_scan_task", (TASK_ENTRY)exp_board_scan_task, (void *)NULL,
        DEFAULT_PRIORITY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Create exp board scan task failed, ret=%d", ret);
    }

    ret = vos_task_create(&hdd_scan_task_id, "hdd_board_scan_task", (TASK_ENTRY)hdd_board_scan_task, (void *)NULL,
        DEFAULT_PRIORITY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Create exp board scan task failed, ret=%d", ret);
    }

    return RET_OK;
}


gint32 ipmi_cmd_set_sas_expboard_led(const void *msg_data, gpointer user_data)
{
    gint32 result;
    const guint8 *src_data = NULL;
    OBJ_HANDLE obj_handle;
    const SAS_EXPBOARD_OP_S *expboard_msg = NULL;
    
    if (msg_data == NULL) { 
        return RET_ERR;
    }
    src_data = get_ipmi_src_data(msg_data);
    if (src_data == NULL) { 
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }
    expboard_msg = (const SAS_EXPBOARD_OP_S *)src_data;

    
    if (dal_get_specific_object_byte(CLASS_SAS_EXPBOARD_NAME, PROPERTY_SAS_EXPBOARD_SLOTID, expboard_msg->slot_id,
        &obj_handle) != DFL_OK) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }
    
    if (expboard_msg->sub_op_type == SAS_EXPBOARD_LOCALLED_TYPE) {
        result = dal_set_property_value_byte(obj_handle, PROPERTY_SAS_EXPBOARD_LOCALLED, expboard_msg->state, DF_AUTO);
    } else if (expboard_msg->sub_op_type == SAS_EXPBOARD_ALARMLED_TYPE) {
        result = dal_set_property_value_byte(obj_handle, PROPERTY_SAS_EXPBOARD_ALARMLED, expboard_msg->state, DF_HW);
    } else {
        result = RET_ERR;
    }
    if (result != RET_OK) {
        ipmi_operation_log(msg_data, "ipmi set sasexpboard(0x%x) ledtype(0x%x) to state(0x%x), ret(%d).\n",
            expboard_msg->slot_id, expboard_msg->sub_op_type, expboard_msg->state, result);
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }

    
    ipmi_operation_log(msg_data, "ipmi set sasexpboard(0x%x) ledtype(0x%x) to state(0x%x), ret(%d).\n",
        expboard_msg->slot_id, expboard_msg->sub_op_type, expboard_msg->state, result);
    return ipmi_send_simple_response(msg_data, COMP_CODE_SUCCESS);
}


gint32 ipmi_cmd_set_sas_expboard_powerstate(const void *msg_data, gpointer user_data)
{
    gint32 result;
    const guint8 *src_data = NULL;
    OBJ_HANDLE obj_handle;
    const SAS_EXPBOARD_OP_S *expboard_msg = NULL;
    
    if (msg_data == NULL) { 
        return RET_ERR;
    }
    src_data = get_ipmi_src_data(msg_data);
    if (src_data == NULL) { 
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }
    expboard_msg = (const SAS_EXPBOARD_OP_S *)src_data;

    
    if (expboard_msg->slot_id == 0xff) {
        result = dal_write_cpld_byte(1, SAS_EXPBOARD_POWER_REG_ADDR, expboard_msg->state);
        ipmi_operation_log(msg_data, "ipmi set sasexpboard(0x%x) poweren to (0x%x), ret=(%d).\n",
            expboard_msg->slot_id, expboard_msg->state, result);
        return ipmi_send_simple_response(msg_data, COMP_CODE_SUCCESS);
    }
    if (dal_get_specific_object_byte(CLASS_SAS_EXPBOARD_NAME, PROPERTY_SAS_EXPBOARD_SLOTID, expboard_msg->slot_id,
        &obj_handle) != DFL_OK) {
        return ipmi_send_simple_response(msg_data, USER_PARAMETER_OUT_OF_RANGE);
    }
    result = dal_set_property_value_byte(obj_handle, PROPERTY_SAS_EXPBOARD_SETPOWER, expboard_msg->state, DF_HW);
    if (result != RET_OK) {
        ipmi_operation_log(msg_data, "ipmi set sasexpboard(0x%x) poweren to (0x%x), ret=(%d).\n", expboard_msg->slot_id,
            expboard_msg->state, result);
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }

    
    ipmi_operation_log(msg_data, "ipmi set sasexpboard(0x%x) poweren to (0x%x), ret=(%d).\n", expboard_msg->slot_id,
        expboard_msg->state, result);
    return ipmi_send_simple_response(msg_data, COMP_CODE_SUCCESS);
}


gint32 debug_get_exp_board_led(GSList *input_list)
{
    guint8 input_len;
    guint8 led_state;
    guint8 exp_board_id;
    guint8 led_type;
    OBJ_HANDLE obj_handle;
    gint32 ret;

    if (input_list == NULL) {
        debug_printf("param is null!\n");
        return MODULE_ERR;
    }

    input_len = g_slist_length(input_list);
    if (input_len > 2) { 
        debug_printf("Usage: get_exp_board_led_state [exp board id]\nGet expander board led, 0(close), 1(ligth)\n");
        return MODULE_ERR;
    }

    
    led_type = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    exp_board_id = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 1));
    if (led_type > 1 || exp_board_id > SAS_EXPBOARD_MAX_NUM) {
        debug_printf("Input parameter is invalid, led type(0/1), expboard id(1~8).\n");
        return MODULE_ERR;
    }

    ret =
        dal_get_specific_object_byte(CLASS_SAS_EXPBOARD_NAME, PROPERTY_SAS_EXPBOARD_SLOTID, exp_board_id, &obj_handle);
    if (ret != RET_OK) {
        debug_printf("Get exp board(%d) handle failed, ret=%d.\n", exp_board_id, ret);
        return MODULE_ERR;
    }

    guint8 presence = 0xff;
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_SAS_EXPBOARD_PRESENCE, &presence);
    if (presence != 1) {
        debug_printf("Get exp board(%d) led status failed, presence=%u.\n", exp_board_id, presence);
        return MODULE_ERR;
    }

    
    gchar *prop_name = ((led_type == 0) ? PROPERTY_SAS_EXPBOARD_LOCALLED : PROPERTY_SAS_EXPBOARD_ALARMLED);
    ret = dal_get_extern_value_byte(obj_handle, prop_name, &led_state, DF_HW | DF_COPY);
    if (ret != RET_OK) {
        debug_printf("Get exp board(%d) led status failed, ret=%d.\n", exp_board_id, ret);
        return MODULE_ERR;
    }

    debug_printf("%d\n", led_state);
    return MODULE_OK;
}


gint32 method_sas_expboard_property_sync(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret;
    ret = property_sync_common_handler_ext(obj_handle, caller_info, input_list, output_list, DF_NONE);
    return ret;
}


gint32 set_sas_expboard_pcie_fault(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    guint8 new_errcode;
    guint8 old_errcode = 0x0;
    gint32 result;

    new_errcode = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_SAS_EXPBOARD_ALARMSTATE, &old_errcode);
    if (old_errcode == new_errcode) {
        return RET_OK;
    }
    result = dal_set_property_value_byte(obj_handle, PROPERTY_SAS_EXPBOARD_ALARMSTATE, new_errcode, DF_NONE);
    debug_log(DLOG_DEBUG, "%s: set obj_handle(%s) alarmstate to (0x%x), result=%d", __FUNCTION__,
        dfl_get_object_name(obj_handle), new_errcode, result);

    return result;
}


LOCAL guint8 write_cpld_arcard(guint8 slot, CTRL_CPLD_ARCARD_POWER_E action)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    ret = dal_get_specific_object_byte(CLASS_AR_CARD_NAME, PROPERTY_AR_CARD_SLOT, slot, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get slot(%d) ARCard object failed.", __FUNCTION__, slot);
        return RET_ERR;
    }

    
    if (action == CTRL_ARCARD_RESET) {
        ret = dal_set_property_value_byte(obj_handle, PROPERTY_AR_CARD_RESET_STATE, 1, DF_HW);
        vos_task_delay(50); 
        ret += dal_set_property_value_byte(obj_handle, PROPERTY_AR_CARD_RESET_STATE, 0, DF_HW);
    } else if (action == CTRL_ARCARD_FACTORY_RESET) {
        ret = dal_set_property_value_byte(obj_handle, PROPERTY_AR_CARD_RESET_MODE, 1, DF_HW);
        ret += dal_set_property_value_byte(obj_handle, PROPERTY_AR_CARD_RESET_STATE, 1, DF_HW);
        vos_task_delay(50); 
        ret += dal_set_property_value_byte(obj_handle, PROPERTY_AR_CARD_RESET_STATE, 0, DF_HW);
        ret += dal_set_property_value_byte(obj_handle, PROPERTY_AR_CARD_RESET_MODE, 0, DF_HW);
    } else {
        ret = dal_set_property_value_byte(obj_handle, PROPERTY_AR_CARD_POWERON_STATE, action, DF_HW);
    }
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: slot %d, action %d, ret %d, objname=(%s)", __FUNCTION__, slot, action, ret,
            dfl_get_object_name(obj_handle));
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 method_arcard_action(guint8 slot_id, gchar *act_str, guint32 str_len, guint8 option)
{
    gint32 ret = RET_ERR;
    gint32 ret_str;
    gchar *succss_str = "successfully";
    gchar *failed_str = "failed";

    
    if (act_str == NULL || str_len == 0) {
        debug_log(DLOG_ERROR, "%s %d failed: input param error.", __FUNCTION__, __LINE__);
        return COMP_CODE_INVALID_FIELD;
    }

    
    switch (option) {
        case CTRL_ARCARD_POWER_OFF:
            
            ret = write_cpld_arcard(slot_id, CTRL_ARCARD_POWER_OFF);
            ret_str = snprintf_s(act_str, str_len, str_len - 1, "Power off AR card %u %s\r\n", slot_id,
                ((ret == RET_OK) ? succss_str : failed_str));
            break;

        case CTRL_ARCARD_POWER_ON:
            
            ret = write_cpld_arcard(slot_id, CTRL_ARCARD_POWER_ON);
            ret_str = snprintf_s(act_str, str_len, str_len - 1, "Power on AR card %u %s\r\n", slot_id,
                ((ret == RET_OK) ? succss_str : failed_str));
            break;

        case CTRL_ARCARD_RESET:
            
            ret = write_cpld_arcard(slot_id, CTRL_ARCARD_RESET);
            ret_str = snprintf_s(act_str, str_len, str_len - 1, "Reset AR card %u %s\r\n", slot_id,
                ((ret == RET_OK) ? succss_str : failed_str));
            break;

        case CTRL_ARCARD_FACTORY_RESET:
            
            ret = write_cpld_arcard(slot_id, CTRL_ARCARD_FACTORY_RESET);
            ret_str = snprintf_s(act_str, str_len, str_len - 1, "Factory reset AR card %u %s\r\n", slot_id,
                ((ret == RET_OK) ? succss_str : failed_str));
            break;

        default:
            debug_log(DLOG_ERROR, "%s %d failed: action is not supported.", __FUNCTION__, __LINE__);
            ret_str =
                snprintf_s(act_str, str_len, str_len - 1, "The operation on AR card %u is not supported\r\n", slot_id);
            break;
    }

    
    if (ret_str <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret_str=%d.", __FUNCTION__, ret_str);
    }

    return ret;
}


gint32 method_arcard_control(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    guint8 option;
    guint8 slot_id;
    gint32 ret;
    OBJ_HANDLE o_handle = 0;

    
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "method_arcard_control, input is null.");
        method_operation_log(caller_info, NULL, "Operate AR card failed");
        return COMP_CODE_INVALID_FIELD;
    }

    option = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    slot_id = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 1));

    
    if (option >= CTRL_ARCARD_INVALID_ACTION) {
        debug_log(DLOG_ERROR, "method_arcard_control, unsupported func.");
        method_operation_log(caller_info, NULL, "Operate AR card failed");
        return COMP_CODE_UNSUPPORT;
    }

    
    ret = dal_get_specific_object_byte(CLASS_AR_CARD_NAME, PROPERTY_AR_CARD_SLOT, slot_id, &o_handle);
    if (dal_need_forward_to_active_by_can()) {
        debug_log(DLOG_ERROR, "%s: ARCardControl is forward.", __FUNCTION__);
        method_operation_log(caller_info, NULL, "Forward the AR card control action");
        return dal_forward_method_to_active_by_can(o_handle, METHOD_ARCARD_CONTROL, caller_info, input_list,
            output_list, 500); 
    }

    
    gchar action_str[256] = {0};
    ret = method_arcard_action(slot_id, action_str, sizeof(action_str), option);

    
    method_operation_log(caller_info, NULL, "%s", action_str);

    return ret;
}


LOCAL gint32 exp_retimer_chip_reset_action(OBJ_HANDLE handle, gpointer data)
{
    guint8 retry;
    gint32 ret;
    guint32 device_type = 0;
    OBJ_HANDLE ref_handle = 0;

    ret = dfl_get_referenced_object(handle, PROPERTY_RETIMER_REFCOMPONENT, &ref_handle);
    if (ret != DFL_OK) {
        
        debug_log(DLOG_ERROR, "%s get retimer refcomponent handle failed, ret(%d)", dfl_get_object_name(handle), ret);
        return RET_OK;
    }
    ret = dal_get_property_value_uint32(ref_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &device_type);
    if (ret != RET_OK || device_type != COMPONENT_TYPE_EPM) {
        
        debug_log(DLOG_ERROR, "%s device type does not as expected, ret(%d), device type(%d)",
            dfl_get_object_name(handle), ret, device_type);
        return RET_OK;
    }

    for (retry = 0; retry < 0x3; retry++) { 
        ret = dal_set_property_value_byte(handle, PROPERTY_RETIMER_I2C_RESET, 0x1, DF_AUTO);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: Reset %s failed, retry count(%d) ret(%d).",
                __FUNCTION__, dfl_get_object_name(handle), retry, ret);
            continue;
        }
        vos_task_delay(50); 

        ret = dal_set_property_value_byte(handle, PROPERTY_RETIMER_I2C_RESET, 0x0, DF_AUTO);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: Unreset %s failed, retry count(%d), ret(%d).",
                __FUNCTION__, dfl_get_object_name(handle), retry, ret);
            continue;
        }
        if (ret == RET_OK) {
            debug_log(DLOG_ERROR, "%s: Reset %s retimer successed.", __FUNCTION__, dfl_get_object_name(handle));
            break;
        }
    }

    return ret;
}


LOCAL gint32 sas_exp_chip_reset_action(OBJ_HANDLE handle, gpointer data)
{
    guint8 retry;
    gint32 ret;

    for (retry = 0; retry < 0x3; retry++) { 
        ret = dal_set_property_value_byte(handle, PROPERTY_SAS_EXPBOARD_CHIP_REST, 0x1, DF_AUTO);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: Reset %s failed, retry count(%d) ret(%d).",
                __FUNCTION__, dfl_get_object_name(handle), retry, ret);
            continue;
        }

        vos_task_delay(50); 

        ret = dal_set_property_value_byte(handle, PROPERTY_SAS_EXPBOARD_CHIP_REST, 0x0, DF_AUTO);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: Unreset %s failed, retry count(%d), ret(%d).",
                __FUNCTION__, dfl_get_object_name(handle), retry, ret);
            continue;
        }

        if (ret == RET_OK) {
            break;
        }
    }

    return ret;
}


LOCAL gint32 process_sas_exp_chip_reset(const guint16 device_num)
{
    gint32 ret;
    OBJ_HANDLE handle;

    if (!is_smm_active()) { 
        return RET_ERR;
    }

    if (device_num == 0xffff) {
        
        ret = dfl_foreach_object(CLASS_SAS_EXPBOARD_NAME, sas_exp_chip_reset_action, NULL, NULL);
        
        ret = dfl_foreach_object(CLASS_RETIMER_NAME, exp_retimer_chip_reset_action, NULL, NULL);
    } else {
        ret = dal_get_specific_object_byte(CLASS_SAS_EXPBOARD_NAME, PROPERTY_SAS_EXPBOARD_SLOTID, device_num, &handle);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: Get expboard object by slotId(%d) failed, ret(%d).",
                __FUNCTION__, device_num, ret);
            return ret;
        }
        ret = sas_exp_chip_reset_action(handle, NULL);

        
        ret = dal_get_specific_object_byte(CLASS_RETIMER_NAME, PROPERTY_RETIMER_ID, device_num, &handle);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: Get retimer object by slotId(%d) failed, ret(%d).",
                __FUNCTION__, device_num, ret);
            return RET_OK;
        }
        debug_log(DLOG_ERROR, "Found retimer (%d) success, object name :(%s).", device_num,
            dfl_get_object_name(handle));
        ret = exp_retimer_chip_reset_action(handle, NULL);
    }

    return ret;
}


gint32 sas_exp_chip_power_ctrl(const guint16 device_num, const guint8 cmd_code, guint8 *comp_code)
{
    gint32 ret = RET_ERR;

    *comp_code = COMP_CODE_UNKNOWN;
    switch (cmd_code) {
        case EXP_CHIP_RESET:
            ret = process_sas_exp_chip_reset(device_num);
            break;
        default:
            
            debug_log(DLOG_ERROR, "%s: invalid cmd_code(%d)", __FUNCTION__, cmd_code);
            *comp_code = COMP_CODE_OUTOF_RANGE;
    }

    if (ret == RET_OK) {
        *comp_code = COMP_CODE_SUCCESS;
    }

    return ret;
}