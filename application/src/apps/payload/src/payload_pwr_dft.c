
#include <stdio.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <stdbool.h>
#include "pme/common/mscm_vos.h"
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "payload_hop.h"
#include "payload_ipmi.h"
#include "payload_hs.h"
#include "payload_com.h"
#include "payload_pwr.h"
#include "payload_storage.h"
#include "wdt2.h"


LOCAL guint8 g_pwr_button_test_result = TEST_FAILED;       // 默认测试结果是失败的
LOCAL gboolean g_pwr_button_in_test = FALSE;               // 默认未进入测试状态
LOCAL guint8 g_pwr_button_test_status = COMP_CODE_UNSTART; // 默认没有开始测试

guchar pp_is_pwr_button_test_in_progress(void)
{
    return (guchar)g_pwr_button_in_test;
}


#ifdef DFT_ENABLED
LOCAL void check_power_cycle_test_criteria(const gchar *class_name, guchar *ismet)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;

    if ((class_name == NULL) || (ismet == NULL)) {
        debug_log(DLOG_ERROR, "Input parameter can not be NULL.\n");
        return;
    }
    
    ret = dfl_get_object_list(class_name, &obj_list);
    if (ret != DFL_OK) {
        (*ismet = SATISFY);
        debug_log(DLOG_ERROR, "get objlist for class %s  fail.", class_name);
        return;
    }

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        (void)dal_get_property_value_byte((OBJ_HANDLE)(obj_node->data), PROPERTY_POWER_CYCLE_CRITERIA_IS_MET, ismet);
        if (!(*ismet)) {
            debug_log(DLOG_ERROR, "%s: %s Other chip upgrade conflict.\n", __FUNCTION__, class_name);
            break;
        }
    }

    g_slist_free(obj_list);
    return;
}


LOCAL guint32 check_ft_power_timeout(OBJ_HANDLE obj_handle, gulong timeout, guchar expectation_state)
{
    guchar power_status = 0;
    gulong start_time;

    start_time = vos_tick_get();

    do {
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_PAYLOAD_CHASSPWR_STATE, &power_status);
        
        if ((vos_tick_get() - start_time) > (timeout * 1000)) {
            debug_log(DLOG_ERROR, "%s: chassis power state timeout.\n", __FUNCTION__);
            return TEST_FAILED;
        }
        
        vos_task_delay(1000);
    } while (expectation_state != power_status);

    return TEST_SUCCEED;
}


void task_os_fast_power_cycle_test(void)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guchar prop_value = 0;
    guint32 i;
    guchar ft_mode_flg = 0;
    gulong pcie_upgrade_s_time;
    gulong pcie_upgrade_t_times = 0;
    gulong vrd_upgrade_s_time;
    gulong vrd_upgrade_t_times = 0;
    guchar IsPowerCtrl = 0;
    guchar ismet = SATISFY;

    (void)prctl(PR_SET_NAME, (gulong)"FastPwrCycle");

    
    for (i = 0; i < 12; i++) {
        (void)dal_get_object_handle_nth(CLASS_DFT_STATUS, 0, &obj_handle);
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_FT_MODE_FLAG, &ft_mode_flg);
        if (ft_mode_flg) {
            break;
        }
        
        vos_task_delay(5 * 1000);
    }

    if (i >= 12) { 
        return;
    }

    (void)dal_get_object_handle_nth(CLASS_PAYLOAD, CHASSIS_FRU_ID, &obj_handle);

    
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_PAYLOAD_PWRON_CTRL, &IsPowerCtrl);
    if (IsPowerCtrl) {
        (void)dal_set_property_value_byte(obj_handle, PROPERTY_PAYLOAD_PWRON_CTRL, 0, DF_NONE);
    }

    
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_CURRENT_POWERCYCLERESULT, &prop_value);

    (void)dal_set_property_value_byte(obj_handle, PROPERTY_LAST_POWERCYCLERESULT, prop_value, DF_SAVE);

    prop_value = 0;

    
    for (i = 0; i < 5; i++) {
        
        
        pcie_upgrade_s_time = vos_tick_get();

        do {
            check_power_cycle_test_criteria(CLASS_POWER_OFF_CRITERIA, &ismet);
            
            vos_task_delay(2000);
        } while (ismet == UN_SATISFY);

        ismet = SATISFY;

        pcie_upgrade_t_times += vos_tick_get() - pcie_upgrade_s_time;
        
        pp_set_cycle_progress_state(CHASSIS_FRU_ID, FALSE);
        pp_set_cycle_interval(CHASSIS_FRU_ID, 0);
        pp_save_cycle_param();

        hop_set_pwr_off_abort(TRUE);

        // 控制系统强制下电
        ret = pp_fru_pwr_ctrl(CHASSIS_FRU_ID, POWER_OFF_FORCE);
        if (ret != COMP_CODE_SUCCESS) {
            (prop_value = TEST_FAILED);
            debug_log(DLOG_ERROR, "%s: chassis force power off fail ret=%d.\n", __FUNCTION__, ret);
            break;
        }

        
        ret = check_ft_power_timeout(obj_handle, POWER_OFF_DELAY_TIMES, HARD_POWER_OFF);
        if (ret == TEST_FAILED) {
            (prop_value = TEST_FAILED);
            debug_log(DLOG_ERROR, "%s: chassis force power off timeout ret=%d.\n", __FUNCTION__, ret);
            break;
        }

        
        vrd_upgrade_s_time = vos_tick_get();

        do {
            check_power_cycle_test_criteria(CLASS_POWER_ON_CRITERIA, &ismet);
            
            vos_task_delay(2000);
        } while (!ismet);

        vrd_upgrade_t_times += vos_tick_get() - vrd_upgrade_s_time;
        
        ret = pp_fru_pwr_on(SRC_CHAN_NUM_RPC, CHASSIS_FRU_ID, RESTART_CAUSE_OEM);
        if (ret != COMP_CODE_SUCCESS) {
            prop_value = TEST_FAILED;
            debug_log(DLOG_ERROR, "%s: chassis power on  fail ret=%d.\n", __FUNCTION__, ret);
            break;
        }

        
        ret = check_ft_power_timeout(obj_handle, POWER_ON_DELAY_TIMES, HARD_POWER_ON);
        if (ret == TEST_FAILED) {
            (prop_value = TEST_FAILED);
            debug_log(DLOG_ERROR, "%s: chassis power on timeout ret=%d.\n", __FUNCTION__, ret);
            break;
        }
    }

    
    (void)dal_set_property_value_byte(obj_handle, PROPERTY_CURRENT_POWERCYCLERESULT, prop_value, DF_SAVE);
    
    (void)dal_set_property_value_byte(obj_handle, PROPERTY_PAYLOAD_PWRON_CTRL, IsPowerCtrl, DF_NONE);
    return;
}


LOCAL gint32 get_fru_running_state(guint8 fruid)
{
    OBJ_HANDLE object_handle;
    guchar fru_running_state = 0xFF;
    gint32 ret;
    guint8 write_data[5] = {0x00, 0x00, 0x00, 0x72, 0xbe}; 
    guint8 out_data[5] = {0};                              
    guint32 status_result;
    OBJ_HANDLE prr_handle;
    OBJ_HANDLE cpld_handle;
    OBJ_HANDLE prr_i2c_handle;
    guint8 i2c_slave_status;
    guint8 i2c_status;

    (void)dal_get_specific_object_byte(CLASS_PAYLOAD, PROPERTY_PAYLOAD_FRUID, fruid, &object_handle);
    ret = dal_get_property_value_byte(object_handle, PROPERTY_PAYLOAD_RUNNING_STATE, &fru_running_state);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get %s failed, ret is %d\n", PROPERTY_PAYLOAD_RUNNING_STATE, ret);
    }

    if (0 == fru_running_state) {
        return FRU_RUNNING_NORMALLY;
    } else if (0xFF == fru_running_state) {
        ret = dfl_get_object_handle("cpld1", &cpld_handle);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "get cpld1 handle failed, ret is %d\n", ret);
            return FRU_RUNNING_NOT_READY;
        }

        ret = dfl_chip_byteread("cpld1", 0x8C, 0xFF, &i2c_status);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "get i2c_status failed, ret is %d\n", ret);
            return FRU_RUNNING_NOT_READY;
        }

        if ((i2c_status & 0x40) != 0) {
            debug_log(DLOG_ERROR, "get i2c_status failed, ret is %d\n", ret);
            return FRU_RUNNING_NOT_READY;
        }

        ret = dfl_get_object_handle("PRRI2C0Slave", &prr_i2c_handle);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "get PRRI2C0Slave handle failed, ret is %d\n", ret);
            return FRU_RUNNING_NOT_READY;
        }

        ret = dfl_block_read(prr_i2c_handle, PROPERTY_UPGRADE_ACTION, 0xFFFF, 0x01, &i2c_slave_status);
        if (ret != DFL_OK || i2c_slave_status != 0) {
            debug_log(DLOG_ERROR, "get destination failed, ret is %d\n", ret);
            return FRU_RUNNING_NOT_READY;
        }

        ret = dfl_get_object_handle("PRRMgntChip", &prr_handle);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "init prr chip failed, ret is %d\n", ret);
            return FRU_RUNNING_NOT_READY;
        }
        
        ret = dfl_chip_blkwrite("PRRMgntChip", 0x00, 5, write_data);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "write prr chip failed, ret is %d\n", ret);
            return FRU_RUNNING_NOT_READY;
        }
        
        ret = dfl_chip_blkread("PRRMgntChip", 0x05, 5, out_data);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "read prr chip failed, ret is %d\n", ret);
            return FRU_RUNNING_NOT_READY;
        }
        
        status_result = MAKE_DWORD(out_data[0], out_data[1], out_data[2], out_data[3]);
        if (status_result == 0) {
            return FRU_RUNNING_NORMALLY;
        }
        return FRU_RUNNING_NOT_READY;
    } else {
        return FRU_RUNNING_NOT_READY;
    }
}


LOCAL void dft_action_exec(OBJ_HANDLE dft_obj, guint32 start_offset, guint32 end_offset)
{
#define INVALID_ADDR 0xFFFF
    gint32 ret;
    GVariant *array_action_gvar = NULL;
    GVariant *action_obj_gvar = NULL;
    const gchar *string = NULL;
    OBJ_HANDLE action_obj = 0;
    guint32 exec_index = 0;
    GVariantIter iter;

    ret = dfl_get_property_value(dft_obj, DFT_ACTION, &array_action_gvar);
    if (ret == DFL_OK && NULL != array_action_gvar) {
        (void)g_variant_iter_init(&iter, array_action_gvar);

        while ((action_obj_gvar = g_variant_iter_next_value(&iter)) != NULL) {
            string = g_variant_get_string(action_obj_gvar, NULL);
            ret = dfl_get_object_handle(string, &action_obj);
            exec_index++;

            if (ret != DFL_OK || exec_index < start_offset) {
                debug_log(DLOG_ERROR, "%s:get the action handle(%s), not execute, result=%d \n", __FUNCTION__, string,
                    ret);
                g_variant_unref(action_obj_gvar);
                action_obj_gvar = NULL;
                continue;
            }

            if (exec_index > end_offset) {
                g_variant_unref(action_obj_gvar);
                break;
            }

            ret = dal_exec_dft_action(action_obj, INVALID_ADDR, 1);
            if (ret != RET_OK) {
                
                debug_log(DLOG_ERROR, "%s:%s test failed(result=%d)\n", __FUNCTION__, string, ret);
            }

            g_variant_unref(action_obj_gvar);
            action_obj_gvar = NULL;
        }
    }

    g_variant_unref(array_action_gvar);
}


LOCAL gint32 dft_fru_reset_fun(OBJ_HANDLE dft_obj, guint8 fruid, guint8 timeout)
{
#define DETECT_INTERVAL 200
    guchar result;
    guint32 i;
    guint32 count;

    if (PAYLOAD_POWER_STATE_OFF == pp_get_pwr_state(fruid)) {
        debug_log(DLOG_ERROR, "FRU%u is poweroff, and forced system reset failed\n", fruid);
        return RET_ERR;
    }
    result = hop_reset(fruid);
    if (result != COMP_CODE_SUCCESS) {
        debug_log(DLOG_ERROR, "FRU%u forced system reset failed\n", fruid);
        return RET_ERR;
    }
    
    count = ((timeout - 1) * 1000) / DETECT_INTERVAL;
    for (i = 0; i < count; ++i) {
        if (FRU_RUNNING_NOT_READY == get_fru_running_state(fruid)) {
            debug_log(DLOG_INFO, "After reset %d, Fru%d running not ready!\n", i, fruid);
            break;
        }
        vos_task_delay(DETECT_INTERVAL);
    }
    if (i == count) {
        debug_log(DLOG_ERROR, "Detect 10S, Fru%d still running normally!\n", fruid);
        return RET_ERR;
    }
    
    vos_task_delay((count - i) * DETECT_INTERVAL);
    dft_action_exec(dft_obj, 1, 2); 
    if (FRU_RUNNING_NOT_READY == get_fru_running_state(fruid)) {
        debug_log(DLOG_ERROR, "Detect 10S, Fru%d still running not ready!\n", fruid);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL void dft_fru_reset_task(PDFT_FRU_RESET_S parg)
{
    gint32 retv;
    OBJ_HANDLE dft_obj;
    DFT_RESULT_S *dft_test_result = NULL;
    DFT_FRU_RESET_S dft_fru_reset;
    gpointer temp_result = NULL;
    errno_t safe_fun_ret;

    (void)prctl(PR_SET_NAME, (gulong)"DftFruReset");

    if (parg == NULL) {
        return;
    }

    safe_fun_ret = memcpy_s(&dft_fru_reset, sizeof(DFT_FRU_RESET_S), parg, sizeof(DFT_FRU_RESET_S));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    g_free(parg);

    
    dft_obj = (OBJ_HANDLE)(dft_fru_reset.user_data);

    
    retv = dfl_get_binded_data(dft_obj, &temp_result);
    if (retv != DFL_OK || temp_result == NULL) {
        debug_log(DLOG_ERROR, "%s:dfl_get_binded_data error, result is %d", __FUNCTION__, retv);
        return;
    }
    dft_test_result = (DFT_RESULT_S *)temp_result;
    dft_test_result->status = COMP_CODE_TESTING;

    
    retv = dft_fru_reset_fun(dft_obj, dft_fru_reset.fruid, dft_fru_reset.timeout);

    dft_action_exec(dft_obj, 3, 4); 

    
    if (retv == RET_OK) {
        dft_test_result->result = TEST_SUCCEED;
    } else {
        dft_test_result->result = TEST_FAILED;
    }

    dft_test_result->status = COMP_CODE_COMPLETE;
    return;
}


LOCAL gint32 dft_fru_reset_test(guint32 command, gsize para_size, gconstpointer para_data, DFT_RESULT_S *dft_data,
    gpointer user_data)
{
    errno_t safe_fun_ret;
    gint32 retv;
    OBJ_HANDLE handle;
    DFT_RESULT_S *dft_test_result = NULL;
    gulong ulTaskID = 0;
    PDFT_FRU_RESET_S dft_fru_reset;
    guint8 input[2] = {0}; 

    if (user_data == NULL) {
        debug_log(DLOG_ERROR, "%s:user_data is NULL\n", __func__);
        return RET_ERR;
    }

    
    if ((command == TEST_START) || (command == TEST_STOP)) {
        
        if (para_size != 2) {
            debug_log(DLOG_ERROR, "parameter size(%" G_GSIZE_FORMAT ") invalid\n", para_size);
            return RET_ERR;
        }

        safe_fun_ret = memcpy_s(&input[0], sizeof(input), para_data, para_size);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            return RET_ERR;
        }
    }

    handle = (OBJ_HANDLE)user_data;
    
    retv = dfl_get_binded_data(handle, (gpointer *)&dft_test_result);
    if ((retv != DFL_OK) || (dft_test_result == NULL)) {
        debug_log(DLOG_ERROR, "Obj(%s) get binded data failed\n", dfl_get_object_name(handle));
        return RET_ERR;
    }
    switch (command) {
        case TEST_START:
            if ((dft_test_result->status == COMP_CODE_TESTING) ||
                (TRUE == vos_task_exist(ulTaskID, "dft_fru_reset_task"))) {
                return RET_OK;
            }
            dft_fru_reset = (PDFT_FRU_RESET_S)g_malloc0(sizeof(DFT_FRU_RESET_S));
            if (dft_fru_reset == NULL) {
                return RET_ERR;
            }
            dft_fru_reset->user_data = user_data;
            dft_fru_reset->fruid = input[0];
            dft_fru_reset->timeout = input[1];

            retv = vos_task_create(&ulTaskID, "dft_fru_reset_task", (TASK_ENTRY)dft_fru_reset_task, dft_fru_reset,
                DEFAULT_PRIORITY);
            if (retv != RET_OK) {
                g_free(dft_fru_reset);
                debug_log(DLOG_ERROR, "%s:vos_task_create failed\n", __func__);
                return RET_ERR;
            }
            break;

        case TEST_STOP:
            dft_test_result->status = COMP_CODE_UNSTART;
            dft_test_result->result = TEST_NON;
            break;

        case TEST_GET_RESULT:
            dft_data->status = dft_test_result->status;
            dft_data->result = dft_test_result->result;
            (void)memset_s(dft_data->result_str, sizeof(dft_data->result_str), 0, sizeof(dft_data->result_str));
            debug_log(DLOG_DEBUG, "%s:result is %d, status is %d\n", __func__, dft_data->result, dft_data->status);
            break;

        default:
            return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 register_fru_reset_dft_item(OBJ_HANDLE handle, guint32 dft_id)
{
    gint32 retv;
    guint32 slot_id = 0;
    guint32 dev_id = 0;
    DFT_RESULT_S *dft_test_result = NULL;

    retv = dal_get_property_value_uint32(handle, SLOT_ID, &slot_id);
    if (retv != RET_OK) {
        debug_log(DLOG_ERROR, "%s:dal_get_property_value_uint32 failed:obj_name is %s,retv is %d\n", __FUNCTION__,
            dfl_get_object_name(handle), retv);
        return retv;
    }
    retv = dal_get_property_value_uint32(handle, DEV_ID, &dev_id);
    if (retv != RET_OK) {
        debug_log(DLOG_ERROR, "%s:dal_get_property_value_uint32 failed:obj_name is %s,retv is %d\n", __FUNCTION__,
            dfl_get_object_name(handle), retv);
        return retv;
    }
    dft_test_result = (DFT_RESULT_S *)g_malloc(sizeof(DFT_RESULT_S));
    if (dft_test_result == NULL) {
        debug_log(DLOG_ERROR, "%s:malloc bind data failed!", __FUNCTION__);
        return RET_ERR;
    }
    dft_test_result->status = COMP_CODE_UNSTART;
    dft_test_result->result = TEST_NON;
    (void)memset_s(dft_test_result->result_str, sizeof(dft_test_result->result_str), 0, DFT_RESULT_STR_LEN);

    
    retv = dfl_bind_object_data(handle, (gpointer)dft_test_result, g_free);
    if (retv != DFL_OK) {
        g_free((gpointer)dft_test_result);
        debug_log(DLOG_DEBUG, "%s: bind object data failed. retv:%d\n", __FUNCTION__, retv);
        return retv;
    }
    
    retv = dfl_register_dft_item(dft_id, slot_id, dev_id, dft_fru_reset_test, (gpointer)handle);
    debug_log(DLOG_DEBUG, "%s:regist bmc uart item result:%d\n", __FUNCTION__, retv);

    return retv;
}
#endif


static void pp_get_pwr_button_test_result(DFT_RESULT_S *dft_data)
{
    if (dft_data == NULL) {
        return;
    }

    // 只有开始测试后才能返回测试成功
    dft_data->status = g_pwr_button_test_status;
    if (g_pwr_button_in_test == TRUE) {
        dft_data->result = g_pwr_button_test_result;
    } else {
        dft_data->result = TEST_NON;
    }

    (void)memset_s(dft_data->result_str, DFT_RESULT_STR_LEN, 0, DFT_RESULT_STR_LEN);

    return;
}


LOCAL gint32 pp_pwr_button_test(guint32 command, gsize para_size, gconstpointer para_data, DFT_RESULT_S *dft_data,
    gpointer user_data)
{
    
    if (dft_data == NULL) {
        return RET_ERR;
    }

    switch (command) {
        case TEST_START:
            // 开始测试
            g_pwr_button_in_test = TRUE;

            // 启动测试时，需要先清除按钮事件，保证测试不会误报
            (void)hop_clear_pwr_button_event();
            // 启动测试时，需要将电源按钮重新锁住，防止测试过程中下电
            (void)hop_set_pwr_button_lock(TRUE, NO_PER_LOCK);

            // 启动测试时,需要将上次测试的结果清除掉
            g_pwr_button_test_result = TEST_FAILED;
            // 启动测试时，设置为开始测试状态
            g_pwr_button_test_status = COMP_CODE_TESTING;
            break;

        case TEST_STOP:
            // 结束测试
            // 结束测试时，需要将电源按钮解锁，恢复正常功能
            (void)hop_set_pwr_button_lock(FALSE, NO_PER_LOCK);
            g_pwr_button_in_test = FALSE;
            g_pwr_button_test_status = COMP_CODE_UNSTART;
            break;

        case TEST_GET_RESULT:
            // 获取测试结果
            pp_get_pwr_button_test_result(dft_data);
            break;

        default:
            return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 register_pwr_button_test_item(OBJ_HANDLE handle, guint32 dft_id)
{
    guint32 slot_id;
    guint32 dev_id;
    GVariant *property_value = NULL;
    gint32 result;

    // 获取SLOT_ID
    result = dfl_get_property_value(handle, SLOT_ID, &property_value);
    if (result != DFL_OK) {
        return result;
    }

    slot_id = g_variant_get_uint32(property_value);
    g_variant_unref(property_value);
    property_value = NULL;
    // 获取DEV_ID
    result = dfl_get_property_value(handle, DEV_ID, &property_value);
    if (result != DFL_OK) {
        return result;
    }

    dev_id = g_variant_get_uint32(property_value);
    g_variant_unref(property_value);
    
    result = dfl_register_dft_item(dft_id, slot_id, dev_id, pp_pwr_button_test, NULL);
    if (result != DFL_OK) {
        return result;
    }

    return RET_OK;
}


LOCAL gint32 pp_dft_obj_init(OBJ_HANDLE handle, gpointer data)
{
    guint32 dft_id;
    gint32 result;

    
    result = dal_get_property_value_uint32(handle, DFT_ID, &dft_id);
    if (result != RET_OK) {
        return result;
    }

    switch (dft_id) {
        case DFT_POWER_BUTTON_PRESS:
            (void)register_pwr_button_test_item(handle, dft_id);
            break;
#ifdef DFT_ENABLED
        case DFT_FRU_RESET_TEST:
            (void)register_fru_reset_dft_item(handle, dft_id);
            break;
#endif
        case DFT_OS_RESET:
            (void)register_os_reset_dft_item(handle, dft_id);
            break;
        default:
            break;
    }

    return RET_OK;
}


LOCAL guchar get_power_button_power_on_enable_property(guchar fruid)
{
    gint32 ret;
    guchar power_btn_power_on_enable = PWR_BUTTON_ENABLE_POWER_ON;
    OBJ_HANDLE object_handle = 0;
    if (fruid != 0) {
        return power_btn_power_on_enable;
    }
    ret = dfl_get_binded_object(CLASS_PAYLOAD, fruid, &object_handle);
    if (ret != DFL_OK) {
        return power_btn_power_on_enable;
    }
    (void)dal_get_property_value_byte(object_handle, PROPERTY_PAYLOAD_PWR_BTN_PWR_ON_EN, &power_btn_power_on_enable);
    return power_btn_power_on_enable;
}


LOCAL void go_software_power_on_due_to_power_button_event(guchar fruid)
{
    gint32 ret;
    guchar result;
    OBJ_HANDLE object_handle = 0;
    guchar acpi_status = 0;
    gchar hot_swap_state = 0;

    if (fruid != 0) {
        return;
    }
    ret = dfl_get_binded_object(CLASS_PAYLOAD, fruid, &object_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d object fail!(result=%d)\n", fruid, ret);
        return;
    }
    ret = dal_get_extern_value_byte(object_handle, PROPERTY_PAYLOAD_ACPI_STATUS, &acpi_status, DF_AUTO);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get the fru%d %s fail, ret=%d\n", fruid, PROPERTY_PAYLOAD_ACPI_STATUS, ret);
        return;
    }

    ret = dal_get_specific_object_byte(CLASS_HOTSWAP, PROPERTY_HS_FRUID, fruid, &object_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get the fru%d %s object fail, ret=%d\n", fruid, CLASS_HOTSWAP, ret);
        return;
    }
    ret = dal_get_property_type(object_handle, PROPERTY_HS_CRU_STATE, &hot_swap_state);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get the %s %s  fail, ret=%d\n", dfl_get_object_name(object_handle),
            PROPERTY_HS_CRU_STATE, ret);
        return;
    }
    
    if ((0 == acpi_status) && (2 <= hot_swap_state)) {
        result = pp_chassis_pwr_on(SRC_CHAN_NUM_RPC, RESTART_CAUSE_CHASSISCONTROL);
        operation_log("PANEL", "N/A", "127.0.0.1", NULL,
            "Power button pressed and will start to power on, result is %s\n", result == 0 ? "successfully" : "fail");
    } else {
        operation_log("PANEL", "N/A", "127.0.0.1", NULL,
            "Power button pressed but  acpi_status=%d  hot_swap_state=%d, no need to power on\n ", acpi_status,
            hot_swap_state);
    }
}


LOCAL void pp_check_reset_cause_from_pwr_btn(void)
{
    if ((BUT_EVT_TRUE == hop_get_reset_event_from_pwr()) && (BUT_EVT_TRUE == hop_get_pwrbtn_on_flag())) {
        debug_log(DLOG_DEBUG, "check_reset_cause_from_pwr_btn ok.\r\n");

        pp_set_restart_action(SRC_CHAN_NUM_RPC, RESTART_CAUSE_POWERBUTTON);
        pp_set_restart_cause();

        hop_set_pwrbtn_on_flag(BUT_EVT_FALSE);
        hop_set_reset_event_from_pwr(BUT_EVT_FALSE);

        pp_update_restart_value();
        pp_deal_with_restart_cause_flag();
    }
    return;
}


LOCAL void get_cpld_status(guchar *cpld_status)
{
    gint32 ret;
    OBJ_HANDLE handle = 0;
    guchar value = 0;
    guchar value2 = 0;
    guchar value3 = 0;

    if (cpld_status == NULL) {
        return;
    }

    ret = dal_get_specific_object_byte(CLASS_FRU, PROPERTY_FRU_ID, 0, &handle);
    if (ret == RET_OK) {
        
        (void)dal_get_property_value_byte(handle, PROPERTY_FRU_CPLD_STATUS, &value);
        (void)dal_get_property_value_byte(handle, PROPERTY_FRU_CPLD2_STATUS, &value2);
        (void)dal_get_property_value_byte(handle, PROPERTY_FRU_CPLD3_STATUS, &value3);

        *cpld_status = value | value2 | value3;
    }
}


LOCAL void pp_pwr_short_button_access(guchar fruid)
{
    guchar acpi = ALL_POWER_GOOD_FAIL;
    guchar comp_code;

    (void)hop_get_acpi_status(fruid, &acpi);
    if (acpi != ALL_POWER_GOOD_OK) {
        
        go_software_power_on_due_to_power_button_event(fruid);
        debug_log(DLOG_ERROR, "Press short button to power on.");
        return;
    }

    
    pp_set_cycle_progress_state(CHASSIS_FRU_ID, FALSE);
    pp_set_cycle_interval(CHASSIS_FRU_ID, 0);
    pp_save_cycle_param();
    
    comp_code = pp_fru_pwr_ctrl(CHASSIS_FRU_ID, POWER_OFF);
    if (comp_code == COMP_CODE_SUCCESS) {
        debug_log(DLOG_INFO, "Press short button to power off successfully. comp_code = %d", comp_code);
    } else {
        debug_log(DLOG_ERROR, "Press short button to power off failed. comp_code = %d", comp_code);
    }
}


LOCAL void pp_pwr_button_access_end(guchar fruid, guchar power_button_lock_status)
{
    guchar power_acpi_status;

    power_acpi_status = hop_get_pwr_signal(CHASSIS_FRU_ID);
    debug_log(DLOG_DEBUG, "power_acpi_status is %d.", power_acpi_status);
    if (power_acpi_status == HARD_POWER_ON) {
        
        
        (void)hop_clear_power_flag(fruid, PROPERTY_PAYLOAD_PWR_DROP_SRC);
        
        (void)hop_set_listen_sel_flag(fruid, PROPERTY_PAYLOAD_PWRSIG_DROP, FALSE);

        hop_set_pwrbtn_on_flag(BUT_EVT_TRUE);

        
        if (pp_is_pwr_off_locked(fruid) == TRUE) {
            pp_set_pwr_off_lock(fruid, FALSE);
        }
    }
    if (power_button_lock_status != FALSE) {
        operation_log("PANEL", "N/A", "127.0.0.1", NULL,
            "Power button pressed but this action is invalid, power button on the panel is disabled\n");
    } else {
        operation_log("PANEL", "N/A", "127.0.0.1", NULL, "Press power button successfully\n");
    }

    (void)hop_clear_pwr_button_event();
    
    (void)hop_set_pwr_sel_event(OBJECT_CHASSISPAYLOAD, PROPERTY_POWER_BUTTON_FLAG, FALSE);
}


LOCAL void pp_pwr_button_test_task(guchar fruid)
{
    guchar power_button_status = 0;
    guchar power_button_lock_status = 0;
    guchar power_btn_power_on_enable;
    guchar cpld_status = 0;

    (void)prctl(PR_SET_NAME, (gulong)"PwrBtnTest");

    (void)hop_clear_pwr_button_event();
    power_btn_power_on_enable = get_power_button_power_on_enable_property(fruid);
    for (;;) {
        get_cpld_status(&cpld_status);

        if (cpld_status == 1) {
            
            vos_task_delay(100UL);
            continue;
        }
        if (hop_get_pwr_button_event(&power_button_status) == RET_OK &&
            hop_get_pwr_button_lock_stat(&power_button_lock_status) == RET_OK) {
            if (power_button_status == BUT_EVT_TRUE) {
                
                if (g_pwr_button_in_test == TRUE) {
                    debug_log(DLOG_ERROR, "detect power button press!\r\n");
                    (void)hop_clear_pwr_button_event();
                    if (g_pwr_button_test_result == TEST_FAILED) {
                        g_pwr_button_test_status = COMP_CODE_COMPLETE;
                        g_pwr_button_test_result = TEST_SUCCEED;
                    }
                } else {
                    
                    if (power_button_lock_status == FALSE) { // 如果锁上了，不记录 SEL日志
                        if (power_btn_power_on_enable == PWR_BUTTON_DISABLE_POWER_ON) {
                            go_software_power_on_due_to_power_button_event(fruid);
                        } else if (power_btn_power_on_enable == PWR_BUTTON_EN_PWR_ON_OR_OFF) {
                            pp_pwr_short_button_access(fruid);
                        }

                        (void)hop_set_pwr_sel_event(OBJECT_CHASSISPAYLOAD, PROPERTY_POWER_BUTTON_FLAG, TRUE);
                    }
                    
                    vos_task_delay(6000UL);
                    pp_pwr_button_access_end(fruid, power_button_lock_status);
                }
            }
        }

        vos_task_delay(100UL);

        // 检测按钮导致的重启
        pp_check_reset_cause_from_pwr_btn();
    }
}


void pp_dft_init(void)
{
    TASKID taskID;
    gint32 result;
    
    result = dfl_foreach_object(DFT_ITEM_CLASS, pp_dft_obj_init, NULL, NULL);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "pp_dft_init:dfl_foreach_object fail\r\n");
        return;
    }
    // 创建电源按钮检测任务
    (void)vos_task_create(&taskID, "PBTT", (TASK_ENTRY)pp_pwr_button_test_task, 0, DEFAULT_PRIORITY);
}
