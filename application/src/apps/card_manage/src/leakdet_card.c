

#include "leakdet_card.h"
#include "pme_app/dalib/dal_obj_priv_data.h"

#define ASSERT_DBD_COUNTS   300
#define DEASSERT_DBD_COUNTS 3
#define LEAKAGE_LIMIT_TIMES 3
#define SCAN_INIT_FLAG      255 // 触发或消除动作后，清除此前滑窗数据，将对应数组元素初始化为该值
#define WATERDETCAB_PRESENT_SIGNAL 1
#define WATERDETCAB_ABSENT_SIGNAL  0
#define NO_LEAKAGE_SIGNAL   0
#define LEAKAGE_SIGNAL      1

typedef struct tag_leakage_mntr {
    OBJ_HANDLE handle;
    guint8 waterdetcab_alarm; // 当前是否处于水浸绳不在位告警状态，0 否，1 是
    guint8 waterdetcab_read_data;
    guint8 waterdetcab_absent[DEASSERT_DBD_COUNTS]; // 存储告警阶段水浸绳检测信号
    guint8 waterdetcab_present[ASSERT_DBD_COUNTS]; // 存储未告警阶段水浸绳检测信号
    guint32 waterdetcab_scan_times;
    guint32 waterdetcab_present_times;
    guint8 leakage_alarm; // 当前是否处于漏液告警状态，0 否，1 是
    guint8 leakage_read_data;
    guint8 leakage_status[DEASSERT_DBD_COUNTS]; // 存储告警阶段漏液检测信号
    guint8 leakage_no_status[ASSERT_DBD_COUNTS]; // 存储未告警阶段漏液检测信号
    guint32 leakage_scan_times;
    guint32 leakage_times;
} LEAKAGE_MNTR;

LOCAL gint8 g_waterdetcab_debug_mode = 0;
LOCAL gint8 g_waterdetcab_debug_read = 0;
LOCAL gint8 g_leakage_debug_mode = 0;
LOCAL gint8 g_leakage_debug_read = 0;

LOCAL void deassert_waterdetcab_alarm(LEAKAGE_MNTR* mntr);
LOCAL void assert_waterdetcab_alarm(LEAKAGE_MNTR* mntr);
LOCAL void deassert_leakage_alarm(LEAKAGE_MNTR* mntr);
LOCAL void assert_leakage_alarm(LEAKAGE_MNTR* mntr);
LOCAL gint32 leakage_scan(OBJ_HANDLE obj_handle, gpointer user_data);
LOCAL gint32 leakage_mntr_new(OBJ_HANDLE obj_handle);
LOCAL void monitor_leakage(void);


gint32 ctrl_magvalve_state(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret;
    guint8 magvalve_state;

    if (input_list == NULL) {
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        return RET_ERR;
    }

    if (g_slist_length(input_list) != 1) {
        debug_log(DLOG_ERROR, "%s : input_list length must be 1", __FUNCTION__);
        return RET_ERR;
    }

    magvalve_state = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    if (magvalve_state != DISABLE && magvalve_state != ENABLE) {
        debug_log(DLOG_ERROR, "%s : input magnetic valve state[%d] is illegal", __FUNCTION__, magvalve_state);
        return RET_ERR;
    }

    ret = dal_set_property_value_byte(object_handle, PROPERTY_LEAK_MAGVALVE_CTRL, magvalve_state, DF_HW);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : Failed to Control magnetic valve disable/enable register, magvalve_state=%d",
            __FUNCTION__, magvalve_state);
        method_operation_log(caller_info, NULL, "%s magnetic valve failed",
            (magvalve_state == DISABLE) ? "Disable" : "Enable");
        return ret;
    }

    method_operation_log(caller_info, NULL, "%s magnetic valve successfully",
        (magvalve_state == DISABLE) ? "Disable" : "Enable");
    return RET_OK;
}


gint32 test_leak_detect_alarm(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret;
    guint8 alarm_state;

    if (input_list == NULL) {
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        return RET_ERR;
    }

    if (g_slist_length(input_list) != 1) {
        debug_log(DLOG_ERROR, "%s : input_list length must be 1", __FUNCTION__);
        return RET_ERR;
    }

    alarm_state = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    if (alarm_state != LIQUID_DETECT_ALARM_ASSERT && alarm_state != LIQUID_DETECT_ALARM_DEASSERT) {
        debug_log(DLOG_ERROR, "%s : input liquid detect alarm state[%d] is illegal", __FUNCTION__, alarm_state);
        return RET_ERR;
    }

    ret = dal_set_property_value_byte(object_handle, PROPERTY_LEAK_TEST, alarm_state, DF_HW);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : Failed to test liquid detect alarm asserted/deasserted register, alarm_state=%d",
            __FUNCTION__, alarm_state);
        method_operation_log(caller_info, NULL, "Test leakage alarm %s failed",
            (alarm_state == LIQUID_DETECT_ALARM_ASSERT) ? "asserted" : "deasserted");
        return ret;
    }

    method_operation_log(caller_info, NULL, "Test leakage alarm %s successfully",
        (alarm_state == LIQUID_DETECT_ALARM_ASSERT) ? "asserted" : "deasserted");
    return RET_OK;
}


gint32 waterdetcab_debug(GSList* input_list)
{
#define PARMA_NUM 2
    if (input_list == NULL) {
        debug_printf("input parma is null.\n");
        return RET_ERR;
    }
    if (g_slist_length(input_list) != PARMA_NUM) {
        debug_printf("Usage: start waterdetcab debug [debug_mode 1:open 0:close] [signal 1:present 0:abesent].\n");
        return RET_ERR;
    }
    gint8 mode = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    gint8 read = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 1));
    if ((mode != 0 && mode != 1) || (read != 0 && read != 1)) {
        debug_printf("input error.\n");
        return RET_ERR;
    }
    g_waterdetcab_debug_mode = mode;
    g_waterdetcab_debug_read = read;
    debug_printf("success.\n");
    return RET_OK;
}


gint32 leakage_debug(GSList* input_list)
{
#define PARMA_NUM 2
    if (input_list == NULL) {
        debug_printf("input parma is null.\n");
        return RET_ERR;
    }
    if (g_slist_length(input_list) != PARMA_NUM) {
        debug_printf("Usage: start leakage debug [debug_mode 1:open 0:close] [signal 1:leakage 0:no_leakage].\n");
        return RET_ERR;
    }
    gint8 mode = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    gint8 read = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 1));
    if ((mode != 0 && mode != 1) || (read != 0 && read != 1)) {
        debug_printf("input error.\n");
        return RET_ERR;
    }
    g_leakage_debug_mode = mode;
    g_leakage_debug_read = read;
    debug_printf("success.\n");
    return RET_OK;
}


LOCAL void deassert_waterdetcab_alarm(LEAKAGE_MNTR* mntr)
{
    guint32 index = (mntr->waterdetcab_scan_times - 1) % DEASSERT_DBD_COUNTS;
    guint8 clear_flag = 0;
    mntr->waterdetcab_absent[index] = mntr->waterdetcab_read_data;
    for (gsize i = 0; i < DEASSERT_DBD_COUNTS; i++) {
        if (mntr->waterdetcab_absent[i] == WATERDETCAB_PRESENT_SIGNAL) {
            clear_flag++;
        }
    }

    if (clear_flag == DEASSERT_DBD_COUNTS) {
        debug_log(DLOG_ERROR, "%s: clear water detection cable absent alarm", __FUNCTION__);
        gint32 ret = dal_set_property_value_byte(mntr->handle,
            PROPERTY_LEAK_WATERDET_CABLE_PRESENCE, WATERDETCAB_PRESENT_SIGNAL, DF_NONE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: clear water detection cable absent alarm failed, ret = %d", __FUNCTION__, ret);
        }
        // 消除告警后，下一次滑窗开始前清除之前数据
        mntr->waterdetcab_scan_times = 0;
        mntr->waterdetcab_alarm = 0;
        for (gsize j = 0; j < DEASSERT_DBD_COUNTS; j++) {
            mntr->waterdetcab_absent[j] = SCAN_INIT_FLAG;
        }
    }
    return;
}


LOCAL void assert_waterdetcab_alarm(LEAKAGE_MNTR* mntr)
{
    guint32 index = (mntr->waterdetcab_scan_times - 1) % ASSERT_DBD_COUNTS;
    guint8 old_data = mntr->waterdetcab_present[index];
    if (old_data == WATERDETCAB_PRESENT_SIGNAL) {
        mntr->waterdetcab_present_times--;
    }
    mntr->waterdetcab_present[index] = mntr->waterdetcab_read_data;
    guint8 new_data = mntr->waterdetcab_present[index];
    if (new_data == WATERDETCAB_PRESENT_SIGNAL) {
        mntr->waterdetcab_present_times++;
    }

    // 数组所有元素均为有效信号且存储在位信号次数为0，则触发水浸绳不在位告警;
    if (mntr->waterdetcab_present[ASSERT_DBD_COUNTS - 1] != SCAN_INIT_FLAG && mntr->waterdetcab_present_times == 0) {
        debug_log(DLOG_ERROR, "%s: trigger water detection cable absent alarm", __FUNCTION__);
        gint32 ret = dal_set_property_value_byte(mntr->handle,
            PROPERTY_LEAK_WATERDET_CABLE_PRESENCE, WATERDETCAB_ABSENT_SIGNAL, DF_NONE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: trigger water detection cable absent alarm failed, ret = %d", __FUNCTION__, ret);
        }
        // 触发告警后，下一次滑窗开始前清除之前数据
        mntr->waterdetcab_scan_times = 0;
        mntr->waterdetcab_alarm = 1;
        for (gsize i = 0; i < ASSERT_DBD_COUNTS; i++) {
            mntr->waterdetcab_present[i] = SCAN_INIT_FLAG;
        }
    }
    return;
}


LOCAL void deassert_leakage_alarm(LEAKAGE_MNTR* mntr)
{
    guint32 index = (mntr->leakage_scan_times - 1) % DEASSERT_DBD_COUNTS;
    guint8 clear_flag = 0;
    mntr->leakage_status[index] = mntr->leakage_read_data ? LEAKAGE_SIGNAL : NO_LEAKAGE_SIGNAL;
    for (gsize i = 0; i < DEASSERT_DBD_COUNTS; i++) {
        if (mntr->leakage_status[i] == NO_LEAKAGE_SIGNAL) {
            clear_flag++;
        }
    }

    if (clear_flag == DEASSERT_DBD_COUNTS) {
        debug_log(DLOG_ERROR, "%s: clear leakage alarm", __FUNCTION__);
        gint32 ret = dal_set_property_value_byte(mntr->handle,
            PROPERTY_LEAK_LEAKAGE_STATUS, NO_LEAKAGE_SIGNAL, DF_NONE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: clear leakage alarm failed, ret = %d", __FUNCTION__, ret);
        }
        // 消除告警后，下一次滑窗开始前清除之前数据
        mntr->leakage_scan_times = 0;
        mntr->leakage_alarm = 0;
        for (gsize j = 0; j < DEASSERT_DBD_COUNTS; j++) {
            mntr->leakage_status[j] = SCAN_INIT_FLAG;
        }
    }
    return;
}


LOCAL void assert_leakage_alarm(LEAKAGE_MNTR* mntr)
{
    guint32 index = (mntr->leakage_scan_times - 1) % ASSERT_DBD_COUNTS;
    guint8 old_data = mntr->leakage_no_status[index];
    if (old_data == LEAKAGE_SIGNAL) {
        mntr->leakage_times--;
    }
    mntr->leakage_no_status[index] = mntr->leakage_read_data ? LEAKAGE_SIGNAL : NO_LEAKAGE_SIGNAL;
    guint8 new_data = mntr->leakage_no_status[index];
    if (new_data == LEAKAGE_SIGNAL) {
        mntr->leakage_times++;
    }

    if (mntr->leakage_times == LEAKAGE_LIMIT_TIMES) {
        debug_log(DLOG_ERROR, "%s: trigger leakage alarm", __FUNCTION__);
        gint32 ret = dal_set_property_value_byte(mntr->handle,
            PROPERTY_LEAK_LEAKAGE_STATUS, LEAKAGE_SIGNAL, DF_NONE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: trigger leakage alarm failed, ret = %d", __FUNCTION__, ret);
        }
        // 触发告警后，下一次滑窗开始前清除之前数据
        mntr->leakage_times = 0;
        mntr->leakage_scan_times = 0;
        mntr->leakage_alarm = 1;
        for (gsize j = 0; j < ASSERT_DBD_COUNTS; j++) {
            mntr->leakage_no_status[j] = SCAN_INIT_FLAG;
        }
    }
    return;
}


LOCAL gint32 leakage_scan(OBJ_HANDLE obj_handle, gpointer user_data)
{
    LEAKAGE_MNTR* mntr = NULL;
    gint32 ret = get_obj_priv_data(obj_handle, (gpointer *)&mntr);
    if (ret != RET_OK || mntr == NULL) {
        debug_log(DLOG_DEBUG, "%s: %s get_obj_priv_data failed, ret = %d", __FUNCTION__,
            dfl_get_object_name(obj_handle), ret);
        return RET_OK;
    }

    mntr->waterdetcab_scan_times++;
    if (g_waterdetcab_debug_mode) { // 进入调试模式
        mntr->waterdetcab_read_data = g_waterdetcab_debug_read;
    } else { // 从芯片读取水浸绳在位信号更新告警状态
        ret = dal_get_property_value_byte(mntr->handle, PROPERTY_LEAK_WATERDET_CABLE_PRES_A,
            &mntr->waterdetcab_read_data);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: read waterdetcab signal failed, ret = %d", __FUNCTION__, ret);
            put_obj_priv_data(obj_handle, mntr);
            return RET_OK;
        }
    }
    if (mntr->waterdetcab_alarm) { // 当前处于告警阶段，滑窗时长3s，一直在位则消除告警
        deassert_waterdetcab_alarm(mntr);
    } else { // 当前处于未告警阶段，滑窗时长5min，一直不在位则触发告警
        assert_waterdetcab_alarm(mntr);
    }

    mntr->leakage_scan_times++;
    if (g_leakage_debug_mode) { // 进入调试模式
        mntr->leakage_read_data = g_leakage_debug_read;
    } else { // 从芯片读取漏液检测信号更新告警状态
        ret = dal_get_property_value_byte(mntr->handle, PROPERTY_LEAK_LEAK_ALARM, &mntr->leakage_read_data);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: read leakage signal failed, ret = %d", __FUNCTION__, ret);
            put_obj_priv_data(obj_handle, mntr);
            return RET_OK;
        }
    }
    if (mntr->leakage_alarm) { // 当前处于告警阶段，滑窗时长3s，一直无漏液则消除告警
        deassert_leakage_alarm(mntr);
    } else { // 当前处于未告警阶段，滑窗时长5min，累计3次漏液则触发告警
        assert_leakage_alarm(mntr);
    }
    
    put_obj_priv_data(obj_handle, mntr);
    return RET_OK;
}


LOCAL gint32 leakage_mntr_new(OBJ_HANDLE obj_handle)
{
    LEAKAGE_MNTR* mntr = (LEAKAGE_MNTR*)g_malloc0(sizeof(LEAKAGE_MNTR));
    if (mntr == NULL) {
        debug_log(DLOG_ERROR, "%s: %s malloc failed", __FUNCTION__, dfl_get_object_name(obj_handle));
        return RET_ERR;
    }

    mntr->handle = obj_handle;
    errno_t safe_ret1 = memset_s(mntr->waterdetcab_present, ASSERT_DBD_COUNTS, SCAN_INIT_FLAG, ASSERT_DBD_COUNTS);
    errno_t safe_ret2 = memset_s(mntr->waterdetcab_absent, DEASSERT_DBD_COUNTS, SCAN_INIT_FLAG, DEASSERT_DBD_COUNTS);
    if (safe_ret1 != EOK || safe_ret2 != EOK) {
        debug_log(DLOG_ERROR, "%s: initialize waterdetcab signal queue failed", __FUNCTION__);
        goto EXIT;
    }
    mntr->waterdetcab_alarm = 0;
    mntr->waterdetcab_scan_times = 0;
    mntr->waterdetcab_present_times = 0;
    mntr->waterdetcab_read_data = 0;

    safe_ret1 = memset_s(mntr->leakage_no_status, ASSERT_DBD_COUNTS, SCAN_INIT_FLAG, ASSERT_DBD_COUNTS);
    safe_ret2 = memset_s(mntr->leakage_status, DEASSERT_DBD_COUNTS, SCAN_INIT_FLAG, DEASSERT_DBD_COUNTS);
    if (safe_ret1 != EOK || safe_ret2 != EOK) {
        debug_log(DLOG_ERROR, "%s: initialize leakage signal queue failed", __FUNCTION__);
        goto EXIT;
    }
    mntr->leakage_alarm = 0;
    mntr->leakage_scan_times = 0;
    mntr->leakage_times = 0;
    mntr->leakage_read_data = 0;

    gint32 ret = new_obj_priv_data(obj_handle, (gpointer)mntr, g_free);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: new %s obj priv data failed, ret = %d", __FUNCTION__,
            dfl_get_object_name(obj_handle), ret);
        goto EXIT;
    }
    return RET_OK;
EXIT:
    g_free(mntr);
    return RET_ERR;
}


LOCAL void monitor_leakage(void)
{
    (void)prctl(PR_SET_NAME, (gulong) "LeakDetect");
    while (TRUE) {
        (void)dfl_foreach_object(CLASS_LEAK_DET_CARD, leakage_scan, NULL, NULL);
        // 扫描周期1000ms
        vos_task_delay(1000);
    }
}


void leak_detect_card_start(void)
{
    GSList* obj_list = NULL;
    TASKID mon_leakage_task = 0;
    gint32 ret = dfl_get_object_list(CLASS_LEAK_DET_CARD, &obj_list);
    if (ret != DFL_OK || obj_list == NULL) {
        debug_log(DLOG_ERROR, "%s: get class LeakageDetectionCard object list failed, ret = %d", __FUNCTION__, ret);
        return;
    }

    for (GSList* node = obj_list; node != NULL; node = node->next) {
        OBJ_HANDLE obj_handle = (OBJ_HANDLE)node->data;
        ret = leakage_mntr_new(obj_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: %s new obj priv data failed, ret = %d", __FUNCTION__,
                dfl_get_object_name(obj_handle), ret);
            continue;
        }
    }

    ret = vos_task_create(&mon_leakage_task, "MonLeakageTask", (TASK_ENTRY)monitor_leakage, NULL, DEFAULT_PRIORITY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: create monitor leakage task failed, ret = %d", __FUNCTION__, ret);
        for (GSList* node = obj_list; node != NULL; node = node->next) {
            OBJ_HANDLE obj_handle_del = (OBJ_HANDLE)node->data;
            delete_obj_priv_data(obj_handle_del);
        }
    }

    g_slist_free(obj_list);
    return;
}
