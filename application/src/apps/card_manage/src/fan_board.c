

#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "fan_board.h"
#include "get_version.h"


LOCAL guint8 fan_board_get_fan_ctrl_flag(void)
{
    OBJ_HANDLE cooling_handle = 0;
    gint32 ret;
    guint8 flag = ENABLE;   
    GSList *output_list = NULL;

    ret = dal_get_object_handle_nth(COOLINGCLASS, 0, &cooling_handle);
    if (ret != RET_OK) {
        return flag;
    }

    ret = dfl_call_class_method(cooling_handle, METHOD_COOLING_GET_FAN_CTRL_FLAG, NULL, NULL, &output_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, call "METHOD_COOLING_GET_FAN_CTRL_FLAG" method failed, ret %d", __FUNCTION__, ret);
        return flag;
    }

    flag = g_variant_get_byte((GVariant*)g_slist_nth_data(output_list, 0));
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

    return flag;
}


LOCAL gint32 process_each_fan_board(OBJ_HANDLE handle, gpointer data)
{
    gint32 ret;

    if (fan_board_get_fan_ctrl_flag() != ENABLE) {
        debug_log(DLOG_ERROR, "[%s] Current board can not support to ctrl the fan board(%s).", __FUNCTION__,
            dfl_get_object_name(handle));
        return RET_OK;
    }

    
    ret = get_pcb_version(handle, PROPERTY_FANBOARD_PCBID, PROPERTY_FANBOARD_PCBVER);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Get fan board(%s) pcb version fail, ret=%d", __FUNCTION__,
            dfl_get_object_name(handle), ret);
    }

    
    ret = get_logic_version(handle, PROPERTY_FANBOARD_LOGICVERID, PROPERTY_FANBOARD_LOGICVER);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Get fan board(%s) logic version fail, ret=%d", __FUNCTION__,
            dfl_get_object_name(handle), ret);
    }

    return RET_OK;
}


gint32 fan_board_init(void)
{
    (void)dfl_foreach_object(CLASS_FANBOARD_NAME, process_each_fan_board, NULL, NULL);

    return RET_OK;
}


gint32 fan_board_reload_info(void)
{
    guint32 obj_count = 0;
    guint8 *verids = NULL;
    guint8 verid_tmp = 0;
    guchar count = 0;
    guchar retry_limit = 0;

    if (dfl_get_object_count(CLASS_FANBOARD_NAME, &obj_count) != DFL_OK) {
        return RET_ERR;
    }
    if (obj_count == 0) {
        return RET_ERR;
    }
    verids = (guint8 *)g_malloc(obj_count);
    if (verids == NULL) {
        return RET_ERR;
    }
    (void)memset_s(verids, obj_count, 0, obj_count);

    for (retry_limit = 0; retry_limit < 100; retry_limit++) {
        guint32 obj_i = 0;
        sleep(1);

        for (obj_i = 0; obj_i < obj_count; obj_i++) {
            OBJ_HANDLE handle = 0;
            if (dal_get_object_handle_nth(CLASS_FANBOARD_NAME, obj_i, &handle) != DFL_OK) {
                g_free(verids);
                return RET_ERR;
            }

            verid_tmp = 0;
            (void)dal_get_property_value_byte(handle, PROPERTY_FANBOARD_LOGICVERID, &verid_tmp);
            if (verid_tmp != verids[obj_i]) {
                count = 0;
            }
            verids[obj_i] = verid_tmp;
        }
        debug_log(DLOG_DEBUG, "wait fan reboot:%d", retry_limit);
        count++;
        if (count >= 5) {
            break;
        }
    }
    g_free(verids);

    (void)dfl_foreach_object(CLASS_FANBOARD_NAME, process_each_fan_board, NULL, NULL);
    return RET_OK;
}


LOCAL gint32 fan_board_sync_time_foreach(OBJ_HANDLE handle, gpointer data)
{
    gint32 ret;
    guint32 mcu_option;
    struct tm local_time = { 0 };
    FAN_BOARD_SD5000_TIME fan_board_time;

    
    ret = dal_get_property_value_uint32(handle, PROPERTY_FANBOARD_MCU_OPTION, &mcu_option);
    if ((ret != RET_OK) || ((mcu_option & ENABLE_FANBOARD_TIME_MASK) == 0)) {
        return RET_OK;
    }

    
    if (dal_localtime_r(NULL, &local_time) == RET_ERR) {
        debug_log(DLOG_DEBUG, "%s: get system time fail.", __FUNCTION__);
        return RET_OK;
    }

    fan_board_time.year = (guint8)(local_time.tm_year + YEAR_1900 - YEAR_2000);
    fan_board_time.month = (guint8)local_time.tm_mon + 1;
    fan_board_time.day = (guint8)local_time.tm_mday;
    fan_board_time.hour = (guint8)local_time.tm_hour;
    fan_board_time.min = (guint8)local_time.tm_min;
    fan_board_time.sec = (guint8)local_time.tm_sec;

    
    ret = dfl_block_write(handle, PROPERTY_FANBOARD_TIME, FAN_TIME_WR, sizeof(fan_board_time), &fan_board_time);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: obj(%s) sync time fail(%d).", __FUNCTION__, dfl_get_object_name(handle), ret);
    }

    return ret;
}


LOCAL gint32 fan_board_sync_temp_foreach(OBJ_HANDLE handle, gpointer data)
{
    guint8 temp_status = 0;

    (void)dal_get_extern_value_byte(handle, PROPERTY_FANBOARD_TEMP_STATUS, &temp_status, DF_COPY);
    (void)dal_set_property_value_byte(handle, PROPERTY_FANBOARD_SYNC_TEMP_STATUS, temp_status, DF_NONE);

    return RET_OK;
}


LOCAL void fan_board_scan_task(void)
{
    const guint32 ctrl_scan_interval = 1000;
    const guint32 sync_time_interval = 60000;
    const guint32 sync_temp_interval = 1000;
    guint32 obj_count = 0;
    guint32 pre_sync_time_tick = 0;
    guint32 pre_sync_temp_tick = 0;
    guint32 cur_tick;
    guint32 product_id = 0;

    
    (void)dal_get_product_id(&product_id);
    if (product_id != PRODUCT_ID_PANGEA_V6) {
        return;
    }

    (void)prctl(PR_SET_NAME, (unsigned long)"FanBoardScanTask");

    for (;;) {
        vos_task_delay(ctrl_scan_interval);

        
        (void)dfl_get_object_count(CLASS_FANBOARD_NAME, &obj_count);

        
        if (obj_count == 0 || fan_board_get_fan_ctrl_flag() != ENABLE) {
            continue;
        }

        cur_tick = vos_tick_get();
        if (cur_tick - pre_sync_time_tick >= sync_time_interval) {
            
            (void)dfl_foreach_object(CLASS_FANBOARD_NAME, fan_board_sync_time_foreach, NULL, NULL);
            pre_sync_time_tick = cur_tick;
        }

        if (cur_tick - pre_sync_temp_tick >= sync_temp_interval) {
            
            (void)dfl_foreach_object(CLASS_FANBOARD_NAME, fan_board_sync_temp_foreach, NULL, NULL);
            pre_sync_temp_tick = cur_tick;
        }
    }
}


gint32 fan_board_start(void)
{
    TASKID task_id_scan = 0;
    gint32 ret;

    ret = vos_task_create(&task_id_scan, "ScanFanBoard", (TASK_ENTRY)fan_board_scan_task, NULL, DEFAULT_PRIORITY);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s : create fanboard scan task thread failed(%d).", __FUNCTION__, ret);
    }

    return RET_OK;
}


gint32 fan_board_add_object_callback(OBJ_HANDLE object_handle)
{
    
    if (!dal_match_software_type(MGMT_SOFTWARE_TYPE_PANGEA_PACIFIC_CTRL)) {
        card_manage_log_operation_log("FAN board", object_handle, PROPERTY_FANBOARD_SLOT, NULL, CARD_PLUG_IN);
    }
    return process_each_fan_board(object_handle, NULL);
}


gint32 fan_board_del_object_callback(OBJ_HANDLE object_handle)
{
    
    if (!dal_match_software_type(MGMT_SOFTWARE_TYPE_PANGEA_PACIFIC_CTRL)) {
        card_manage_log_operation_log("FAN board", object_handle, PROPERTY_FANBOARD_SLOT, NULL, CARD_PLUG_OUT);
    }
    return RET_OK;
}


gint32 fan_board_dump_info(const gchar *path)
{
    gint32 iRet = -1;
    gint32 ret = 0;
    guint16 board_id = 0;
    const gchar *name = NULL;
    const gchar *pcb_ver = NULL;
    const gchar *logic_ver = NULL;
    const gchar *manufacturer = NULL;
    gchar file_name[FAN_BOARD_DUMPINFO_MAX_LEN + 1] = {0};
    gchar fan_board_info[FAN_BOARD_DUMPINFO_MAX_LEN + 1] = {0};
    FILE *fp = NULL;

    guint8 slot_id = 0;
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;
    size_t fwrite_back;

    if (path == NULL) {
        return RET_ERR;
    }

    ret = dfl_get_object_list(CLASS_FANBOARD_NAME, &obj_list);
    if (ret != DFL_OK) {
        if (ret == ERRCODE_OBJECT_NOT_EXIST) {
            return RET_OK;
        }
        return RET_ERR;
    }
    
    // 在dump_dir目录下创建文件
    iRet = snprintf_s(file_name, FAN_BOARD_DUMPINFO_MAX_LEN + 1, FAN_BOARD_DUMPINFO_MAX_LEN, "%s/card_info", path);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    fp = dal_fopen_check_realpath(file_name, "a+", file_name);
    if (fp == NULL) {
        g_slist_free(obj_list);
        return RET_ERR;
    }
    
    (void)fchmod(fileno(fp), COMMON_DUMP_FILE_UMASK);
    

    (void)snprintf_s(fan_board_info, FAN_BOARD_DUMPINFO_MAX_LEN + 1, FAN_BOARD_DUMPINFO_MAX_LEN, "%s",
        "FAN board Info\n");
    fwrite_back = fwrite(fan_board_info, strlen(fan_board_info), 1, fp);
    if (fwrite_back != 1) {
        (void)fclose(fp);
        g_slist_free(obj_list);
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }

    (void)memset_s(fan_board_info, FAN_BOARD_DUMPINFO_MAX_LEN + 1, 0, FAN_BOARD_DUMPINFO_MAX_LEN + 1);
    (void)snprintf_s(fan_board_info, FAN_BOARD_DUMPINFO_MAX_LEN + 1, FAN_BOARD_DUMPINFO_MAX_LEN,
        "%-4s | %-10s | %-10s | %-32s | %-10s | %s\n", "Slot", "BoardId", "Name", "Manufacturer", "PCB Ver",
        "Logic Ver");
    fwrite_back = fwrite(fan_board_info, strlen(fan_board_info), 1, fp);
    if (fwrite_back != 1) {
        (void)fclose(fp);
        g_slist_free(obj_list);
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        property_name_list = g_slist_append(property_name_list, PROPERTY_FANBOARD_SLOT);
        property_name_list = g_slist_append(property_name_list, PROPERTY_FANBOARD_BOARDID);
        property_name_list = g_slist_append(property_name_list, PROPERTY_FANBOARD_NAME);
        property_name_list = g_slist_append(property_name_list, PROPERTY_FANBOARD_MANUFACTURER);
        property_name_list = g_slist_append(property_name_list, PROPERTY_FANBOARD_PCBVER);
        property_name_list = g_slist_append(property_name_list, PROPERTY_FANBOARD_LOGICVER);

        ret = dfl_multiget_property_value((OBJ_HANDLE)obj_note->data, property_name_list, &property_value);
        g_slist_free(property_name_list);
        property_name_list = NULL;
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "Get FAN board information failed!");
            g_slist_free(obj_list);
            (void)fclose(fp);
            return RET_ERR;
        }
        slot_id = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 0));
        board_id = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value, 1));
        name = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 2), 0);
        manufacturer = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 3), 0);
        pcb_ver = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 4), 0);
        logic_ver = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 5), 0);

        (void)memset_s(fan_board_info, FAN_BOARD_DUMPINFO_MAX_LEN + 1, 0, FAN_BOARD_DUMPINFO_MAX_LEN + 1);
        iRet = snprintf_s(fan_board_info, sizeof(fan_board_info), sizeof(fan_board_info) - 1,
            "%-4u | 0x%-4x     | %-10s | %-32s | %-10s | %s\n", slot_id, board_id, name, manufacturer, pcb_ver,
            logic_ver);
        if (iRet <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
        }
        fwrite_back = fwrite(fan_board_info, strlen(fan_board_info), 1, fp);
        if (fwrite_back != 1) {
            (void)fclose(fp);
            g_slist_free(obj_list);
            g_slist_free_full(property_value, (GDestroyNotify)g_variant_unref);
            debug_log(DLOG_ERROR, "fwrite failed!");
            return RET_ERR;
        }

        (void)memset_s(fan_board_info, FAN_BOARD_DUMPINFO_MAX_LEN + 1, 0, FAN_BOARD_DUMPINFO_MAX_LEN + 1);
        g_slist_free_full(property_value, (GDestroyNotify)g_variant_unref);
        property_value = NULL;
    }

    g_slist_free(obj_list);
    (void)snprintf_s(fan_board_info, FAN_BOARD_DUMPINFO_MAX_LEN + 1, FAN_BOARD_DUMPINFO_MAX_LEN, "%s", "\n\n");
    
    fwrite_back = fwrite(fan_board_info, strlen(fan_board_info), 1, fp);
    if (fwrite_back != 1) {
        (void)fclose(fp);
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }
    (void)memset_s(fan_board_info, FAN_BOARD_DUMPINFO_MAX_LEN + 1, 0, FAN_BOARD_DUMPINFO_MAX_LEN + 1);
    // 关闭文件
    (void)fclose(fp);

    return RET_OK;
}

void fan_board_updata_card_info(void)
{
    (void)dfl_foreach_object(CLASS_FANBOARD_NAME, process_each_fan_board, NULL, NULL);
    return;
}


LOCAL void dump_each_mcu_log(OBJ_HANDLE handle, const gchar *path)
{
    gchar file_name[FAN_BOARD_DUMPINFO_MAX_LEN + 1] = {0};
    guint32 mcu_option = 0;
    guint8 slot_id = 0;
    guint8 print_buf[INTELLIGENT_FAN_LOG_LEN] = {0};
    gchar file_name_real[PATH_MAX];

    
    gint32 ret = dal_get_property_value_uint32(handle, PROPERTY_FANBOARD_MCU_OPTION, &mcu_option);
    if ((ret != RET_OK) || ((mcu_option & ENABLE_FANBOARD_LOG_MASK) == 0)) {
        debug_log(DLOG_ERROR, "[%s] fan board(%s) does not support to load mcu log.", __FUNCTION__,
            dfl_get_object_name(handle));
        return;
    }

    
    ret = dal_get_property_value_byte(handle, PROPERTY_FANBOARD_SLOT, &slot_id);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s] Get slot id of fan board(%s) fail(%d).", __FUNCTION__, dfl_get_object_name(handle),
            ret);
        return;
    }

    ret = snprintf_s(file_name, FAN_BOARD_DUMPINFO_MAX_LEN + 1, FAN_BOARD_DUMPINFO_MAX_LEN, "%s/FanBox%u.log", path,
        slot_id);
    if (ret < 0) {
        debug_log(DLOG_ERROR, "[%s] Format fan board(%s) log file name with slot id(%d) fail(%d).", __FUNCTION__,
            dfl_get_object_name(handle), slot_id, ret);
        return;
    }

    
    ret = dfl_block_read(handle, "LogData", 0x35, sizeof(print_buf),
        print_buf); 
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s] Load fan board(%s) log failed(%d)!", __FUNCTION__, dfl_get_object_name(handle),
            ret);
        return;
    }

    if (realpath(file_name, file_name_real) == NULL) {
        debug_log(DLOG_ERROR, "[%s] check file real path failed", __FUNCTION__);
        return;
    }

    
    FILE *fp = g_fopen(file_name_real, "w+");
    if (fp == NULL) {
        debug_log(DLOG_ERROR, "[%s] Create fan board(%s) mcu log file failed!", __FUNCTION__,
            dfl_get_object_name(handle));
        return;
    }

    
    size_t fwrite_back = fwrite(print_buf, sizeof(print_buf), 1, fp);
    if (fwrite_back != 1) {
        debug_log(DLOG_ERROR, "[%s] Write fan board(%s) log file failed(%zu)!", __FUNCTION__,
            dfl_get_object_name(handle), fwrite_back);
    }

    (void)fclose(fp);
}


void fan_mcu_dump_log(const gchar *path)
{
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    gint32 ret = 0;

    if (path == NULL) {
        debug_log(DLOG_ERROR, "[%s] Current log path is null.", __FUNCTION__);
        return;
    }

    if (fan_board_get_fan_ctrl_flag() != ENABLE) {
        debug_log(DLOG_ERROR, "[%s] Current board can not support to ctrl the fan board.", __FUNCTION__);
        return;
    }

    debug_log(DLOG_ERROR, "[%s] Start get fan board mcu log.", __FUNCTION__);
    ret = dfl_get_object_list(CLASS_FANBOARD_NAME, &obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s] Get fan board object list fail(%d).", __FUNCTION__, ret);
        return;
    }

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        dump_each_mcu_log((OBJ_HANDLE)obj_node->data, path);
    }
    g_slist_free(obj_list);
}


gint32 method_sync_fanboard_info(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret;

    ret = property_sync_common_handler_ext(object_handle, caller_info, input_list, output_list, DF_NONE);
    debug_log(DLOG_DEBUG, "%s property_sync ret:%d.", dfl_get_object_name(object_handle), ret);
    return ret;
}
