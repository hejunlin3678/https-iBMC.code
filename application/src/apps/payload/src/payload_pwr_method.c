
#include <stdio.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <stdbool.h>
#include "pme/common/mscm_vos.h"
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "pme_app/dalib/dal_m3_msg_intf.h"
#include "payload_hop.h"
#include "payload_ipmi.h"
#include "payload_hs.h"
#include "payload_com.h"
#include "payload_pwr.h"
#include "payload_storage.h"
#include "wdt2.h"


LOCAL gint32 pp_called_by_mach2mach_interface(GSList *caller_info)
{
    GVariant *tmp_gv = NULL;
    const gchar *interface;
    tmp_gv = (GVariant *)g_slist_nth_data(caller_info, 0);
    interface = g_variant_get_string(tmp_gv, NULL);
    if (strcmp(interface, CALLER_INFO_SNMP) == 0 || strcmp(interface, CALLER_INFO_IPMI) == 0) {
        return RET_OK;
    } else {
        return RET_ERR;
    }
}

#ifdef ARM64_HI1711_ENABLED
gint32 check_bios_measured_result(void)
{
    OBJ_HANDLE obj_handle = 0;
    guint8 ctrl_result = 0;
    guint8 is_tcm_enable = TCM_DEFAULT;
    gint32 ret = dal_get_bmc_tpcm_enable(&is_tcm_enable);
    debug_log(DLOG_INFO, "ret = %d, is_tcm_enbale = %u", ret, is_tcm_enable);

    if (ret == RET_OK && is_tcm_enable == TCM_ENABLE) {
        (void)dfl_get_object_handle(TPCM_OBJECT_BIOS, &obj_handle);
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_TPCM_CTRL_COMMAND, &ctrl_result);
    }
    
    return (ctrl_result == 0 || ctrl_result == TCM_DEFAULT) ? RET_OK : RET_ERR;
}
#endif


gint32 pp_method_pwr_on_ext(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret;

    
    (void)pp_set_imuforbid_pwron_flag(FALSE);
    ret = pp_method_pwr_on(obj_handle, caller_info, input_list, output_list);
    return ret;
}


gint32 pp_method_pwr_on(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 comp_code;
    gint32 retv;

#ifdef ARM64_HI1711_ENABLED
    if (check_bios_measured_result() != RET_OK) {
        method_operation_log(caller_info, NULL, "Set FRU%u to power on failed, bios measured failed",
            CHASSIS_FRU_ID);
        return COMP_CODE_STATUS_INVALID;
    }
#endif

    // 如果已经上电，直接返回OK
    if (HARD_POWER_ON == hop_get_pwr_signal(CHASSIS_FRU_ID)) {
        if ((is_support_storage_payload() == TRUE) && (get_offing_state_flag() == TRUE)) {
            debug_log(DLOG_ERROR, "Device is in the process of powered off.");
            return POWER_ON_FAILED_BY_IN_OFFING;
        }

        debug_log(DLOG_DEBUG, "%s: It's already in power on state.", __FUNCTION__);
        // 当前处于on/m6,用户下发上电命令，则恢复状态为on/m4，此处增加特殊处理
        if (FRU_HS_STATE_M6 == hs_get_cur_state(CHASSIS_FRU_ID)) {
            (void)hse_unnormal_status_recovery(CHASSIS_FRU_ID);
        }
        method_operation_log(caller_info, NULL, PL_OP_LOG_PWR_ON_T, CHASSIS_FRU_ID);
        return RET_OK;
    }
    // 控制系统上电
    comp_code =
        pp_chassis_pwr_on(SRC_CHAN_NUM_RPC, (caller_info == NULL) ? RESTART_CAUSE_OEM : RESTART_CAUSE_CHASSISCONTROL);
    
    if (output_list != NULL && comp_code == COMP_CODE_SUCCESS) {
        *output_list = g_slist_append(*output_list, g_variant_new_byte(comp_code));
    }
    if (comp_code == COMP_CODE_SUCCESS) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_PWR_ON_T, CHASSIS_FRU_ID);
        retv = RET_OK;
    } else {
        method_operation_log(caller_info, NULL, "FRU%u power on failed\r\n", CHASSIS_FRU_ID);
        debug_log(DLOG_ERROR, "call power on method fail(Ret:%u).\r\n", comp_code);
        retv = comp_code;
        if (comp_code == POWER_ON_FAILED_BY_BIOS_UP && pp_called_by_mach2mach_interface(caller_info) == RET_OK) {
            // 如果是自定义的错误码，并且是由机机接口调用。则返回IPMI规范定义的错误码
            retv = COMP_CODE_STATUS_INVALID;
        }
    }
    return retv;
}


gint32 pp_method_pwr_off(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    guchar comp_code;
    gint32 retv = RET_ERR;

    // 如果已经下电，直接返回OK
    if (HARD_POWER_OFF == hop_get_pwr_signal(CHASSIS_FRU_ID)) {
        pwr_on_task_command(DELETE_TASK);
        method_operation_log(caller_info, NULL, PL_OP_LOG_PWR_OFF_T, CHASSIS_FRU_ID);
        return RET_OK;
    }
    // 取消已经开启的安全重启或POWER循环
    pp_set_cycle_progress_state(CHASSIS_FRU_ID, FALSE);
    pp_set_cycle_interval(CHASSIS_FRU_ID, 0);
    pp_save_cycle_param();
    // 控制系统软下电
    comp_code = pp_fru_pwr_ctrl(CHASSIS_FRU_ID, POWER_OFF);
    if (comp_code == COMP_CODE_SUCCESS) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_PWR_OFF_T, CHASSIS_FRU_ID);
        retv = RET_OK;
    } else {
        method_operation_log(caller_info, NULL, "FRU%u power off graceful failed\r\n", CHASSIS_FRU_ID);
        debug_log(DLOG_ERROR, "call power off method fail.\r\n");
    }

    return retv;
}


gint32 pp_method_force_pwr_off(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    guchar comp_code;
    gint32 retv = RET_ERR;

    // 取消已经开启的安全重启或POWER循环
    pp_set_cycle_progress_state(CHASSIS_FRU_ID, FALSE);
    pp_set_cycle_interval(CHASSIS_FRU_ID, 0);
    pp_save_cycle_param();

    
    hop_set_pwr_off_abort(TRUE);
    
    comp_code = pp_fru_pwr_ctrl(CHASSIS_FRU_ID, POWER_OFF_FORCE);
    if (comp_code == COMP_CODE_SUCCESS) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_PWR_OFF_T1, CHASSIS_FRU_ID);
        retv = RET_OK;
    } else {
        method_operation_log(caller_info, NULL, "FRU%u power off force failed\r\n", CHASSIS_FRU_ID);
        debug_log(DLOG_ERROR, "call force power off method fail.\r\n");
    }

    return retv;
}


gint32 pp_method_pwr_cycle(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    guchar comp_code;
    gint32 retv = RET_ERR;

#ifdef ARM64_HI1711_ENABLED
    if (check_bios_measured_result() != RET_OK) {
        method_operation_log(caller_info, NULL, "pp_method_pwr_cycle failed, bios measured failed");
        return COMP_CODE_STATUS_INVALID;
    }
#endif

    // 如果系统未上电，直接返回ERR
    if (HARD_POWER_OFF == hop_get_pwr_signal(CHASSIS_FRU_ID)) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_COLD_RST_F, CHASSIS_FRU_ID);
        debug_log(DLOG_ERROR, "power off and can't cold reset action.\r\n");
        return COMP_CODE_STATUS_INVALID;
    }
    // 控制系统power cycle
    comp_code = pp_chassis_pwr_cycle(SRC_CHAN_NUM_RPC,
        (caller_info == NULL) ? RESTART_CAUSE_OEM : RESTART_CAUSE_CHASSISCONTROL);
    if (comp_code == COMP_CODE_SUCCESS) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_COLD_RST_T, CHASSIS_FRU_ID);
        retv = RET_OK;
    } else {
        method_operation_log(caller_info, NULL, PL_OP_LOG_COLD_RST_F, CHASSIS_FRU_ID);
        debug_log(DLOG_ERROR, "call cold reset method fail.\r\n");
    }

    return retv;
}


gint32 pp_method_pwr_hard_reset(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    guchar comp_code;
    guchar src_channel = SRC_CHAN_NUM_RPC;
    guchar restart_cause = RESTART_CAUSE_CHASSISCONTROL;
    gint32 retv = RET_ERR;

#ifdef ARM64_HI1711_ENABLED
    if (check_bios_measured_result() != RET_OK) {
        method_operation_log(caller_info, NULL, "pp_method_pwr_hard_reset failed, bios measured failed");
        return COMP_CODE_STATUS_INVALID;
    }
#endif

    // 1 没输入参数
    // 2 两个参数，第一个为src_channel，第二个为restart_cause
    if ((input_list != NULL) && (g_slist_length(input_list) != 2)) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_RST_F, CHASSIS_FRU_ID);
        debug_log(DLOG_ERROR, "input_list para cnt [%u] error.", g_slist_length(input_list));
        return RET_ERR;
    }

    if (caller_info == NULL) {
        (restart_cause = RESTART_CAUSE_OEM);
    }
    // HardResetWithPara方法要求两个参数，第1个为src_channel，第2个为restart_cause
    if (g_slist_length(input_list) == 2) {
        // 当前src_channel未使用，不做解析
        restart_cause = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 1));
        if ((restart_cause <= RESTART_CAUSE_IPMI_MAX) || (restart_cause >= RESTART_CAUSE_OEM_MAX)) {
            method_operation_log(caller_info, NULL, PL_OP_LOG_RST_F, CHASSIS_FRU_ID);
            debug_log(DLOG_ERROR, "restart_cause [%u] error.", restart_cause);
            return RET_ERR;
        }
    }
    // 如果系统未上电，直接返回ERR
    if (hop_get_pwr_signal(CHASSIS_FRU_ID) == HARD_POWER_OFF) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_RST_F, CHASSIS_FRU_ID);
        debug_log(DLOG_ERROR, "power off and can't warm reset action.\r\n");
        return COMP_CODE_STATUS_INVALID;
    }
    // 控制系统hard reset
    comp_code = pp_chassis_hard_reset(src_channel, restart_cause);
    if (comp_code == COMP_CODE_SUCCESS) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_RST_T, CHASSIS_FRU_ID);
        retv = RET_OK;
    } else {
        method_operation_log(caller_info, NULL, PL_OP_LOG_RST_F, CHASSIS_FRU_ID);
        debug_log(DLOG_ERROR, "call warm reset method fail.\r\n");
    }

    return retv;
}


gint32 pp_method_chassis_diag_interrupt(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    // 如果系统未上电，直接返回ERR
    if (HARD_POWER_OFF == hop_get_pwr_signal(CHASSIS_FRU_ID)) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_DIAG_INTR_F, CHASSIS_FRU_ID);
        debug_log(DLOG_ERROR, "power off and can't chassis diag interrupt action.\r\n");
        return COMP_CODE_STATUS_INVALID;
    }
    // 控制系统触发诊断中断
    (void)pp_chassis_nmi_interrupt();
    method_operation_log(caller_info, NULL, PL_OP_LOG_DIAG_INTR_T, CHASSIS_FRU_ID);

    return RET_OK;
}


gint32 pp_method_soft_off(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    guchar comp_code;
    gint32 retv = RET_ERR;

    // 如果系统未上电，直接返回OK
    if (HARD_POWER_OFF == hop_get_pwr_signal(CHASSIS_FRU_ID)) {
        pwr_on_task_command(DELETE_TASK);
        method_operation_log(caller_info, NULL, PL_OP_LOG_SOFT_OFF_T, CHASSIS_FRU_ID);
        return RET_OK;
    }
    // 取消已经开启的安全重启或POWER循环
    pp_set_cycle_progress_state(CHASSIS_FRU_ID, FALSE);
    pp_set_cycle_interval(CHASSIS_FRU_ID, 0);
    pp_save_cycle_param();
    // 控制系统软下电
    comp_code = pp_fru_pwr_ctrl(CHASSIS_FRU_ID, POWER_OFF);
    if (comp_code == COMP_CODE_SUCCESS) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_SOFT_OFF_T, CHASSIS_FRU_ID);
        retv = RET_OK;
    } else {
        method_operation_log(caller_info, NULL, "FRU%u power soft-shutdown failed\r\n", CHASSIS_FRU_ID);
        debug_log(DLOG_ERROR, "call soft off method fail.\r\n");
    }
    return retv;
}


gint32 pp_method_set_pwr_restore_policy(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    guint8 restore_policy;
    gint32 retv;

    if (input_list == NULL) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_REST_POLICY_F);
        
        debug_log(DLOG_ERROR, "input_list can not be null.\r\n");
        return RET_ERR;
    }

    retv = pp_allow_set_power_restore_policy();
    if (retv != DFL_OK) {
        method_operation_log(caller_info, NULL, "The power restore policy cannot be set.");
        return RET_ERR;
    }

    restore_policy = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    if ((restore_policy != POWER_RESTORE_POLICY_ALWAYS_ON) && (restore_policy != POWER_RESTORE_POLICY_PREVIOUS) &&
        (restore_policy != POWER_RESTORE_POLICY_ALWAYS_OFF)) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_REST_POLICY_F);
        
        debug_log(DLOG_ERROR, "Param restore_policy:%d is invalid!\r\n", restore_policy);
        return RET_ERR;
    }
    pp_set_restore_policy(restore_policy);
    method_operation_log(caller_info, NULL, "Set power restore policy to (%s) successfully\r\n",
        pp_policy_to_string(restore_policy));

    return RET_OK;
}

gint32 pp_method_set_power_on_strategy_exceptions(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret_val;
    guint32 power_on_scene_mask;
    GVariant *property_val = NULL;
 
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        goto error_exit;
    }
 
    property_val = (GVariant *)g_slist_nth_data(input_list, 0);
    if (property_val == NULL) {
        goto error_exit;
    }
    power_on_scene_mask = g_variant_get_uint32(property_val);
    g_variant_unref(property_val);

    ret_val =
        dal_set_property_value_uint32(obj_handle, PROPTERY_POWER_ON_STRATEGY_EXCEPTIONS, power_on_scene_mask, DF_SAVE);
    if (ret_val != RET_OK) {
        goto error_exit;
    }
    if (caller_info != NULL) {
        method_operation_log(caller_info, NULL, "Set PowerOnStrategyExceptions successfully");
    }
    return ret_val;
 
error_exit:
    if (caller_info != NULL) {
        method_operation_log(caller_info, NULL, "Set PowerOnStrategyExceptions failed");
    }
    return RET_ERR;
}


gint32 pp_allow_set_power_restore_policy(void)
{
    OBJ_HANDLE obj_handle = 0;
    guint8 prop_val = 0;
    gint32 retv;

    retv = dal_get_object_handle_nth(CLASS_PAYLOAD, 0, &obj_handle);
    if (retv != DFL_OK) {
        debug_log(DLOG_ERROR, "get obj_handle of Payload fail. retv = %d", retv);
        return RET_OK;
    }
 
    retv = dal_get_property_value_byte(obj_handle, PROPERTY_POWER_REST_POLICY, &prop_val);
    if (retv != DFL_OK) {
        debug_log(DLOG_ERROR, "get prop_val of PowerRestorePolicy fail. retv = %d", retv);
        return RET_OK;
    }

    if (prop_val == POWER_RESTORE_POLICY_ALWAYS_ON_RO) {
        debug_log(DLOG_ERROR, "do not support set power restore policy.");
        return RET_ERR;
    }

    return RET_OK;
}


gint32 pp_method_exe_pwr_restore_policy(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    debug_log(DLOG_INFO, "Execute power restore policy.");

    pp_do_pwr_restore_process();
    return RET_OK;
}


gint32 pp_method_poweronlock_clear(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret;
    guchar clear = 0;
    ret = dal_set_property_value_byte(obj_handle, PROPERTY_PAYLOAD_PWRON_TM_SRC, clear, DF_AUTO);
    if (ret != RET_OK) {
        method_operation_log(caller_info, NULL, "%s", "Clear power on lock failed\r\n");
        return ret;
    }
    method_operation_log(caller_info, NULL, "%s", "Clear power on lock successfully\r\n");
    return ret;
}


gint32 pp_method_set_pwr_cycle_interval(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    guint8 cycle_interval;

    if (input_list == NULL) {
        method_operation_log(caller_info, NULL, "Set power cycle interval failed\r\n");
        
        debug_log(DLOG_ERROR, "input_list can not be null.\r\n");
        return RET_ERR;
    }
    cycle_interval = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    pp_set_power_cycle_interval(cycle_interval);
    method_operation_log(caller_info, NULL, "Set power cycle interval to (%u) seconds successfully\r\n",
        cycle_interval);
    return RET_OK;
}

LOCAL gint32 get_power_off_time_en_state(void)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret_val;
    GVariant *property_data = NULL;
    guint32 off_time_en_state;

    ret_val = dfl_get_object_handle(OBJECT_CHASSISPAYLOAD, &obj_handle);
    if (ret_val != DFL_OK) {
        
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
        return RET_ERR;
    }
    ret_val = dfl_get_property_value(obj_handle, PROPERTY_PAYLOAD_PWROFF_TM_EN, &property_data);
    if (ret_val != DFL_OK) {
        
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
        return RET_ERR;
    }

    off_time_en_state = g_variant_get_uint32(property_data);
    g_variant_unref(property_data);

    if (off_time_en_state == 1) {
        return POWER_OFF_TIMEOUT_ENABLE;
    } else if (off_time_en_state == 0) {
        return POWER_OFF_TIMEOUT_DISABLE;
    } else {
        debug_log(DLOG_ERROR, "power off status (%d) : invalid value.", off_time_en_state);
        return RET_ERR;
    }
}

gint32 pp_method_set_pwr_off_timeout(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    guint32 pwr_off_timeout;
    gint32 ret;
    guint64 temp_value;

    ret = get_power_off_time_en_state();
    if (ret == RET_ERR) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_PWR_OFF_TM_F);
        return RET_ERR;
    }
    if (input_list == NULL) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_PWR_OFF_TM_F);
        
        debug_log(DLOG_ERROR, "input_list can not be null.\r\n");
        return RET_ERR;
    }
    pwr_off_timeout = g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 0));
    temp_value = (guint64)pwr_off_timeout * PAYLOAD_TIME_UNIT;
    if (G_MAXUINT32 < temp_value) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_PWR_OFF_TM_F);
        debug_log(DLOG_ERROR, "pp_judge_poweroff_tm_valid fail.\r\n");
        return RET_ERR;
    }
    if (PAYLOAD_SUCCESS != pp_judge_poweroff_tm_valid(pwr_off_timeout * PAYLOAD_TIME_UNIT)) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_PWR_OFF_TM_F);
        debug_log(DLOG_ERROR, "pp_judge_poweroff_tm_valid fail.\r\n");
        return RET_ERR;
    }
    pp_set_pwr_off_timeout(pwr_off_timeout);
    method_operation_log(caller_info, NULL, "Set graceful shutdown timeout to (%u) seconds successfully\r\n",
        pwr_off_timeout);

    return RET_OK;
}


gint32 pp_method_set_pwr_off_timeout_en(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    guint32 pwr_off_timeout_en;

    if (input_list == NULL) {
        method_operation_log(caller_info, NULL, "Set power off timeout failed\r\n");
        
        debug_log(DLOG_ERROR, "input_list can not be null.\r\n");
        return RET_ERR;
    }

    pwr_off_timeout_en = g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 0));
    if ((pwr_off_timeout_en != TRUE) && (pwr_off_timeout_en != FALSE)) {
        method_operation_log(caller_info, NULL, "Set power off timeout failed\r\n");
        debug_log(DLOG_ERROR, "pwr_off_timeout_en:%u is invalid.\r\n", pwr_off_timeout_en);
        return RET_ERR;
    }

    pp_set_pwr_off_timeout_en(pwr_off_timeout_en);

    method_operation_log(caller_info, NULL, "Set power off timeout to (%s) successfully\r\n",
        (pwr_off_timeout_en == TIMEOUT_ENABLE) ? "enable" : "disable");
    return RET_OK;
}


gint32 pp_set_host_start(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    if (FALSE == pp_is_host_up()) {
        pp_set_host_up();
    }

    return RET_OK;
}


gint32 pp_set_powerstate_after_bmc_reset(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    guint8 powerstate_after_boot;
    gint32 ret;

    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s: input_list is null", __FUNCTION__);
        return RET_ERR;
    }
    powerstate_after_boot = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));

    ret = dal_set_property_value_byte(obj_handle, PROPERTY_POWERSTATE_AFTER_BMC_RESET, powerstate_after_boot,
        DF_SAVE_PERMANENT);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:Set %s failed(ret = %d).", __FUNCTION__, PROPERTY_POWERSTATE_AFTER_BMC_RESET, ret);
    }

    return ret;
}


gint32 pp_set_pwr_button_lock(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    guint8 button_lock;
    guint8 forbid_poweron_flag = 0;
    guint8 persist_mode = PERSIST_LOCK;
    guchar force_poweroff_flag = 0;

    if (input_list == NULL) {
        if (caller_info != NULL) {
            method_operation_log(caller_info, NULL, PL_OP_LOG_BTN_LOCK_F);
        }
        
        debug_log(DLOG_ERROR, "input_list can not be null.\r\n");
        return RET_ERR;
    }
    button_lock = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    if ((button_lock != TRUE) && (button_lock != FALSE)) {
        if (caller_info != NULL) {
            method_operation_log(caller_info, NULL, PL_OP_LOG_BTN_LOCK_F);
        }
        debug_log(DLOG_ERROR, "button_lock:%u(Max:%u) is invalid.\r\n", button_lock, TRUE);
        return RET_ERR;
    }
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_PAYLOAD_FORBID_POWERON_FLAG, &forbid_poweron_flag);
    if (RET_OK == get_vddq_check_forcepwroff_flag(&force_poweroff_flag)) {
        if (((forbid_poweron_flag == 1) || (force_poweroff_flag == 1)) && (button_lock == FALSE)) {
            if (caller_info != NULL) {
                method_operation_log(caller_info, NULL, PL_OP_LOG_BTN_LOCK_F);
            }
            debug_log(DLOG_ERROR, "Forbid power on, can not set power button lock to off.\r\n");
            return RET_ERR;
        }
    }
    
    if (TRUE == pp_is_pwr_button_test_in_progress()) {
        if (caller_info != NULL) {
            method_operation_log(caller_info, NULL, PL_OP_LOG_BTN_LOCK_F);
        }
        debug_log(DLOG_ERROR, "pwr button test is in progress,and don't support open button lock.\r\n");
        return RET_ERR;
    }

    
    if (g_slist_length(input_list) == 2) {
        persist_mode = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 1));
        if ((persist_mode != PERSIST_LOCK) && (persist_mode != NO_PER_LOCK) &&
            (persist_mode != SAVE_TEMPORARY_POWER_BUTTON_LOCK)) {
            if (caller_info != NULL) {
                method_operation_log(caller_info, NULL, PL_OP_LOG_BTN_LOCK_F);
            }
            debug_log(DLOG_ERROR, "persist_mode:%u is invalid.\r\n", persist_mode);
            return RET_ERR;
        }
    }
    (void)hop_set_pwr_button_lock(button_lock, persist_mode);
    if (caller_info != NULL) {
        method_operation_log(caller_info, NULL, "Set power button lock to (%s) successfully\r\n",
            button_lock == TRUE ? "on" : "off");
    }

    return RET_OK;
}


gint32 pp_get_pwr_button_lock(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    guint8 button_lock;
    gint32 result;
    OBJ_HANDLE object_handle;
    GVariant *property_data = NULL;

    if (output_list == NULL) {
        
        debug_log(DLOG_ERROR, "output_list can not be null.\r\n");
        return RET_ERR;
    }
    result = dfl_get_object_handle(OBJECT_CHASSISPAYLOAD, &object_handle);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "dfl_get_object_handle fail!(result=%d)\r\n", result);
        return RET_ERR;
    }
    
    result = dfl_get_property_value(object_handle, PROPERTY_PAYLOAD_PWR_BTN_LOCK, &property_data);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "dfl_get_property_value fail!(result=%d)\r\n", result);
        return RET_ERR;
    }
    button_lock = g_variant_get_byte(property_data);
    g_variant_unref(property_data);
    *output_list = g_slist_append(*output_list, g_variant_new_byte(button_lock));

    return RET_OK;
}


gint32 pp_method_set_pwr_ctrl(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    GVariant *property_data = NULL;
    guchar pwrctrl;
    gint32 ret_val;

    if (input_list == NULL) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_PWR_CTRL_F);
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        return RET_ERR;
    }
    property_data = (GVariant *)g_slist_nth_data(input_list, 0);
    if (property_data == NULL) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_PWR_CTRL_F);
        debug_log(DLOG_ERROR, DB_STR_GET_DFL_INPUT_ERR);
        return RET_ERR;
    }
    pwrctrl = g_variant_get_byte(property_data);
    if ((pwrctrl != TRUE) && (pwrctrl != FALSE)) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_PWR_CTRL_F);
        debug_log(DLOG_ERROR, DB_STR_GET_DFL_INPUT_ERR);
        return RET_ERR;
    }
    // 不要持久化自主上电属性
    ret_val = dfl_set_property_value(obj_handle, PROPERTY_PAYLOAD_PWRON_CTRL, property_data, DF_NONE);
    if (ret_val != DFL_OK) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_PWR_CTRL_F);
        
        debug_log(DLOG_ERROR, DB_STR_SET_PROPERTY_ERR, ret_val);
        return ret_val;
    }
    method_operation_log(caller_info, NULL, PL_OP_LOG_PWR_CTRL_T, pwrctrl);
    return ret_val;
}


gint32 pp_method_fru_active(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    GVariant *property_value = NULL;
    guchar fruid;
    guchar active;
    guchar flag;
    gint32 ret_val;

    if (input_list == NULL) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_FRU_ACTIVE_F);
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        return RET_ERR;
    }
    active = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    flag = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 1));
    if ((active != HOTSWAP_ACTIVATE_FRU) && (active != HOTSWAP_DEACTIVATE_FRU)) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_FRU_ACTIVE_F);
        debug_log(DLOG_ERROR, DB_STR_GET_DFL_INPUT_ERR);
        return RET_ERR;
    }
    if ((flag != TRUE) && (flag != FALSE)) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_FRU_ACTIVE_F);
        debug_log(DLOG_ERROR, DB_STR_GET_DFL_INPUT_ERR);
        return RET_ERR;
    }
    ret_val = dfl_get_property_value(obj_handle, PROPERTY_PAYLOAD_FRUID, &property_value);
    if (ret_val != DFL_OK) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_FRU_ACTIVE_F);
        debug_log(DLOG_ERROR, DB_STR_GET_DFL_INPUT_ERR);
        return RET_ERR;
    }
    fruid = g_variant_get_byte(property_value);
    g_variant_unref(property_value);
    if (COMP_CODE_SUCCESS != hse_fru_activate(fruid, active, flag)) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_FRU_ACTIVE_F);
        debug_log(DLOG_ERROR, DB_STR_GET_DFL_INPUT_ERR);
        return RET_ERR;
    }
    method_operation_log(caller_info, NULL, PL_OP_LOG_FRU_ACTIVE_T, active, flag);

    return ret_val;
}


gint32 pp_method_set_pwr_level(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    GVariant *property_data = NULL;
    guint8 fruid;
    guint8 level;
    guint8 option;
    gint32 ret_val;

    if (input_list == NULL) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_PWR_LEVEL_F);
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        return RET_ERR;
    }
    property_data = (GVariant *)g_slist_nth_data(input_list, 0);
    if (property_data == NULL) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_PWR_LEVEL_F);
        debug_log(DLOG_ERROR, DB_STR_GET_DFL_INPUT_ERR);
        return RET_ERR;
    }
    fruid = g_variant_get_byte(property_data);
    property_data = (GVariant *)g_slist_nth_data(input_list, 1);
    if (property_data == NULL) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_PWR_LEVEL_F);
        debug_log(DLOG_ERROR, DB_STR_GET_DFL_INPUT_ERR);
        return RET_ERR;
    }
    level = g_variant_get_byte(property_data);
    property_data = (GVariant *)g_slist_nth_data(input_list, 2); 
    if (property_data == NULL) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_PWR_LEVEL_F);
        debug_log(DLOG_ERROR, DB_STR_GET_DFL_INPUT_ERR);
        return RET_ERR;
    }
    option = g_variant_get_byte(property_data);

    // 为了兼容旧SMM软件，需要区分刀片与服务器单板
    // 应用当前功率，控制FRU上电
    ret_val = pp_action_according_to_powerlevel(fruid, level, METHOD_SET_POWER);
    if (ret_val != RET_OK) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_PWR_LEVEL_F);
        debug_log(DLOG_ERROR, "pp_action_according_to_powerlevel fail(PowerLevel:%u)!\r\n", level);
        return RET_ERR;
    }
    method_operation_log(caller_info, NULL, "Set power level to (RAW:%02xh-%02xh-%02xh) successfully\r\n", fruid, level,
        option);

    return ret_val;
}

LOCAL gint32 change_forbid_poweron_flag(OBJ_HANDLE obj_handle, guint8 forbid_poweron_flag, guint8 mask)
{
    guint8 flag_value = 0;

    gint32 ret_val = dal_get_property_value_byte(obj_handle, PROPERTY_PAYLOAD_FORBID_POWERON_FLAG, &flag_value);
    if (ret_val != RET_OK) {
        
        debug_log(DLOG_ERROR, "Get forbid power on flag value failed, ret=%d", ret_val);
        return ret_val;
    }

    if (forbid_poweron_flag == 0) {
        
        switch (mask) {
            case BIOS_FORBID_POWER_ON_MASK:
                flag_value &= NBIT0;
                break;
            case CPLD_FORBID_POWER_ON_MASK:
                flag_value &= NBIT1;
                break;
            case VRD_FORBID_POWER_ON_MASK:
                flag_value &= NBIT2;
                break;
            default:
                debug_log(DLOG_ERROR, "Incorrect mask %u, change forbid poweron flag failed", mask);
                return RET_ERR;
        }
    } else {
        
        switch (mask) {
            case BIOS_FORBID_POWER_ON_MASK:
                flag_value |= PBIT0;
                break;
            case CPLD_FORBID_POWER_ON_MASK:
                flag_value |= PBIT1;
                break;
            case VRD_FORBID_POWER_ON_MASK:
                flag_value |= PBIT2;
                break;
            default:
                debug_log(DLOG_ERROR, "Incorrect mask %u, change forbid poweron flag failed", mask);
                return RET_ERR;
        }
    }

    ret_val = dal_set_property_value_byte(obj_handle, PROPERTY_PAYLOAD_FORBID_POWERON_FLAG, flag_value, DF_NONE);
    debug_log(DLOG_ERROR, "Set forbid power on flag(val %d), ret %d.", flag_value, ret_val);
    return ret_val;
}


gint32 pp_method_set_forbid_poweron_flag(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret_val;
    guint8 forbid_poweron_flag;
    guint8 mask;

    if (input_list == NULL) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_FRU_ACTIVE_F);
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        return RET_ERR;
    }
    
    forbid_poweron_flag = g_variant_get_byte(g_slist_nth_data(input_list, 0));
    if (forbid_poweron_flag != UPGRADE_ALLOW_POWER_ON && forbid_poweron_flag != UPGRADE_BIOS_FORBID_POWER_ON &&
        forbid_poweron_flag != UPGRADE_CPLD_FORBID_POWER_ON && forbid_poweron_flag != UPGRADE_VRD_FORBID_POWER_ON) {
        debug_log(DLOG_ERROR, "Input forbid poweron flag : %u invalid", forbid_poweron_flag);
        return RET_ERR;
    }

    
    mask = g_variant_get_byte(g_slist_nth_data(input_list, 1));
    if (mask != BIOS_FORBID_POWER_ON_MASK && mask != CPLD_FORBID_POWER_ON_MASK &&
        mask != VRD_FORBID_POWER_ON_MASK) {
        debug_log(DLOG_ERROR, "Input mask : %u invalid", mask);
        return RET_ERR;
    }

    
    ret_val = change_forbid_poweron_flag(obj_handle, forbid_poweron_flag, mask);
    return ret_val;
}


gint32 pp_method_set_board_powerdrop_flag(const OBJ_HANDLE obj_hnd, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret;

    if ((is_support_storage_payload() == TRUE) && (hop_get_pwr_signal(CHASSIS_FRU_ID) != HARD_POWER_ON)) {
        debug_log(DLOG_ERROR, "System is standby state, no need to record power drop event.");
        return RET_OK;
    }

    ret = hop_set_power_before_ac_lost(1);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set BoardPowerDrop flag failed.\n");
        return ret;
    }

    return ret;
}


gint32 pp_method_clear_restart_value(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret1;
    gint32 ret2;

    ret1 = dal_set_property_value_uint32(obj_handle, PROPERTY_PAYLOAD_RESTART_VALUE, 0, DF_NONE);
    if (ret1 != RET_OK) {
        debug_log(DLOG_ERROR, "set ChassisPayload.RestartValue (0) failed, ret : %d\n", ret1);
    }

    ret2 = dal_set_property_value_uint32(obj_handle, PROPERTY_PAYLOAD_OEM_RESTART_VALUE, 0, DF_NONE);
    if (ret2 != RET_OK) {
        debug_log(DLOG_ERROR, "set ChassisPayload.OEMRestartValue (0) failed, ret : %d\n", ret2);
    }

    return (ret1 == RET_OK && ret2 == RET_OK) ? RET_OK : RET_ERR;
}


LOCAL void update_pwr_on_delay_mode(guint8 value)
{
    gint32 result;
    P_PAYLOAD_S p_payload = NULL;

    result = dfl_get_data_by_alias(CLASS_PAYLOAD, CHASSIS_FRU_ID, (gpointer *)&p_payload);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d data by alias failed!\r\n", CHASSIS_FRU_ID);
        return;
    }
    p_payload->m_pwr_delay_mode = value;
    return;
}


gint32 pp_method_set_pwrdelay_mode(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    guint32 ret;
    guint8 pwr_delay_mode;
    guint8 pwr_current_delay_mode;

    if (input_list == NULL || caller_info == NULL) {
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        return RET_ERR;
    }

    pwr_delay_mode = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    if (pwr_delay_mode >= DELAY_MODE_INVALID) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_PWR_DELAY_MODE_F);
        debug_log(DLOG_ERROR, "pwr_delay_mode:%d is invalid.\r\n", pwr_delay_mode);
        return RET_ERR;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_PAYLOAD_PWRDELAY_MODE, &pwr_current_delay_mode);
    if (ret != RET_OK) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_PWR_DELAY_MODE_F);
        debug_log(DLOG_ERROR, "%s: get %s.%s failed, ret=%d.", __FUNCTION__, dfl_get_object_name(obj_handle),
            PROPERTY_PAYLOAD_PWRDELAY_MODE, ret);
        return RET_ERR;
    }

    if (pwr_delay_mode == pwr_current_delay_mode) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_PWR_DELAY_MODE_T, pp_delay_mode_to_string(pwr_delay_mode));
        return RET_OK;
    }

    // 更新当前PowerDelayMode状态
    ret = dal_set_property_value_byte(obj_handle, PROPERTY_PAYLOAD_PWRDELAY_MODE, pwr_delay_mode, DF_SAVE);
    if (ret != RET_OK) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_PWR_DELAY_MODE_F);
        debug_log(DLOG_ERROR, "set ChassisPayload object PowerDelayMode fail!(result=%d)\r\n", ret);
        return RET_ERR;
    }
    method_operation_log(caller_info, NULL, PL_OP_LOG_PWR_DELAY_MODE_T, pp_delay_mode_to_string(pwr_delay_mode));
    update_pwr_on_delay_mode(pwr_delay_mode);
    return RET_OK;
}


LOCAL void update_pwr_on_delay_count(guint32 value)
{
    gint32 result;
    P_PAYLOAD_S p_payload = NULL;

    result = dfl_get_data_by_alias(CLASS_PAYLOAD, CHASSIS_FRU_ID, (gpointer *)&p_payload);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d data by alias failed!\r\n", CHASSIS_FRU_ID);
        return;
    }
    p_payload->m_pwr_delay_count = value;
    return;
}


gint32 pp_method_set_pwrdelay_count(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    guint32 ret;
    guint32 pwr_delay_count;
    guint32 pwr_current_delay_count;

    if (input_list == NULL || caller_info == NULL) {
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        return RET_ERR;
    }

    pwr_delay_count = g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 0));

    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_PAYLOAD_PWRDELAY_COUNT, &pwr_current_delay_count);
    if (ret != RET_OK) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_PWR_DELAY_COUNT_F);
        debug_log(DLOG_ERROR, "%s: get %s.%s failed, ret=%d.", __FUNCTION__, dfl_get_object_name(obj_handle),
            PROPERTY_PAYLOAD_PWRDELAY_COUNT, ret);
        return RET_ERR;
    }

    if (pwr_delay_count == pwr_current_delay_count) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_PWR_DELAY_COUNT_T, pwr_delay_count / DOUBLE_TEN);
        return RET_OK;
    }

    ret = dal_set_property_value_uint32(obj_handle, PROPERTY_PAYLOAD_PWRDELAY_COUNT, pwr_delay_count, DF_SAVE);
    if (ret != RET_OK) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_PWR_DELAY_COUNT_F);
        debug_log(DLOG_ERROR, "set ChassisPayload object PowerDelayCount fail!(result=%d)\r\n", ret);
        return RET_ERR;
    }
    method_operation_log(caller_info, NULL, PL_OP_LOG_PWR_DELAY_COUNT_T, pwr_delay_count / DOUBLE_TEN);
    update_pwr_on_delay_count(pwr_delay_count);
    return RET_OK;
}


gint32 pp_method_power_cycle(OBJ_HANDLE obj_handle, GSList* caller_info, GSList* input_list, GSList** output_list)
{
    // 如果系统未上电，直接返回ERR
    if (hop_get_pwr_signal(CHASSIS_FRU_ID) == HARD_POWER_OFF) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_PC_F, CHASSIS_FRU_ID);
        debug_log(DLOG_ERROR, "power off and can't power cycle action.");
        return COMP_CODE_STATUS_INVALID;
    }
    
    if (is_support_storage_payload() == TRUE) {
        sim_acdown_for_power_cycle();
        pp_set_restart_action(
            SRC_CHAN_NUM_RPC, (caller_info == NULL) ? RESTART_CAUSE_OEM : RESTART_CAUSE_CHASSISCONTROL);
        pp_set_restart_cause();
        method_operation_log(caller_info, NULL, PL_OP_LOG_PC_T, CHASSIS_FRU_ID);
    }
    return RET_OK;
}


gint32 pp_method_cold_reset(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 result;

    // 如果系统未上电，直接返回ERR
    if (HARD_POWER_OFF == hop_get_pwr_signal(CHASSIS_FRU_ID)) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_RST_F, CHASSIS_FRU_ID);
        debug_log(DLOG_ERROR, "power off and can't warm reset action.\r\n");
        return COMP_CODE_STATUS_INVALID;
    }

    // 控制系统cold reset
    // 疑问:通过obj_handle能否取到this对象，再取对象的FruID属性?
    result = hop_reset(CHASSIS_FRU_ID);
    if (result == COMP_CODE_CMD_INVALID) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_RST_F, CHASSIS_FRU_ID);
        return COMP_CODE_CMD_INVALID;
    }
    if (result == COMP_CODE_SUCCESS) {
        // 记录重启原因
        pp_set_restart_action(SRC_CHAN_NUM_RPC,
            (caller_info == NULL) ? RESTART_CAUSE_OEM : RESTART_CAUSE_CHASSISCONTROL);
        pp_set_restart_cause();
        method_operation_log(caller_info, NULL, PL_OP_LOG_RST_T, CHASSIS_FRU_ID);
        return RET_OK;
    } else {
        method_operation_log(caller_info, NULL, PL_OP_LOG_RST_F, CHASSIS_FRU_ID);
        return RET_ERR;
    }
}


gint32 pp_method_warm_reset(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    debug_log(DLOG_ERROR, "call warm reset method fail.\r\n");
    method_operation_log(caller_info, NULL, PL_OP_LOG_HARD_RST_F, CHASSIS_FRU_ID);

    // 暂时不支持
    return RET_ERR;
}


gint32 pp_method_fru_diag_interrupt(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 result;

    // 如果系统未上电，直接返回ERR
    if (HARD_POWER_OFF == hop_get_pwr_signal(CHASSIS_FRU_ID)) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_DIAG_INTR_F, CHASSIS_FRU_ID);
        debug_log(DLOG_ERROR, "power off and can't diag interrupt action.\r\n");
        return COMP_CODE_STATUS_INVALID;
    }
    result = hop_diag_interrupt(CHASSIS_FRU_ID);
    if (result == COMP_CODE_SUCCESS) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_DIAG_INTR_T, CHASSIS_FRU_ID);
        return RET_OK;
    } else {
        method_operation_log(caller_info, NULL, PL_OP_LOG_DIAG_INTR_F, CHASSIS_FRU_ID);
        return RET_ERR;
    }
}


gint32 pp_method_graceful_reboot(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 result;

    // 如果系统未上电，直接返回ERR
    if (HARD_POWER_OFF == hop_get_pwr_signal(CHASSIS_FRU_ID)) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_COLD_RST_F, CHASSIS_FRU_ID);
        debug_log(DLOG_ERROR, "power off and can't graceful reboot action.\r\n");
        return COMP_CODE_STATUS_INVALID;
    }

    // 控制系统cold reset
    // 疑问1:通过obj_handle能否取到this对象，再取对象的FruID属性?
    // 疑问2:rpc能否支持耗时过程调用?
    result = hop_powercycle(CHASSIS_FRU_ID);
    if (result == COMP_CODE_SUCCESS) {
        method_operation_log(caller_info, NULL, PL_OP_LOG_COLD_RST_T, CHASSIS_FRU_ID);
        // 记录重启原因
        pp_set_restart_action(SRC_CHAN_NUM_RPC,
            (caller_info == NULL) ? RESTART_CAUSE_OEM : RESTART_CAUSE_CHASSISCONTROL);
        pp_set_restart_cause();
        return RET_OK;
    } else {
        method_operation_log(caller_info, NULL, PL_OP_LOG_COLD_RST_F, CHASSIS_FRU_ID);
        debug_log(DLOG_ERROR, "call graceful reboot method fail.\r\n");
        return RET_ERR;
    }
}


LOCAL gint32 pp_method_fru_control_warm_reset(guchar fru_device_id, GSList *caller_info, gchar *actionssuc,
    guint32 ssuc_len, gchar *actionsfail, guint32 fail_len)
{
    gint32 ret;
    guchar restart_cause = (caller_info == NULL) ? RESTART_CAUSE_OEM : RESTART_CAUSE_CHASSISCONTROL;

    
    if (actionssuc == NULL || actionsfail == NULL || ssuc_len == 0 || fail_len == 0) {
        debug_log(DLOG_ERROR, "%s %d failed:input param error.", __FUNCTION__, __LINE__);
        return COMP_CODE_INVALID_FIELD;
    }
    ret = snprintf_s(actionssuc, ssuc_len, ssuc_len - 1, PL_OP_LOG_HARD_RST_T, fru_device_id);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
    ret = snprintf_s(actionsfail, fail_len, fail_len - 1, PL_OP_LOG_HARD_RST_F, fru_device_id);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }

    
    if (PAYLOAD_POWER_STATE_OFF == pp_get_pwr_state(fru_device_id)) {
        debug_log(DLOG_ERROR, " %s %d   fail\n", __FUNCTION__, __LINE__);
        
        ret = COMP_CODE_STATUS_INVALID;
        return ret;
    }

    ret = hop_fru_warm_reset(fru_device_id, SRC_CHAN_NUM_RPC, restart_cause);
    
    if (ret == COMP_CODE_STATUS_INVALID) {
        ret = COMP_CODE_UNSUPPORT;
        debug_log(DLOG_ERROR, " %s %d   fail\n", __FUNCTION__, __LINE__);
        return ret;
    }
    return ret;
}


LOCAL gint32 pp_method_fru_control_cold_reset(guchar fru_device_id, GSList *caller_info, gchar *actionssuc,
    guint32 ssuc_len, gchar *actionsfail, guint32 fail_len)
{
    gint32 ret;
    guchar restart_cause = (caller_info == NULL) ? RESTART_CAUSE_OEM : RESTART_CAUSE_CHASSISCONTROL;

    
    if (actionssuc == NULL || actionsfail == NULL || ssuc_len == 0 || fail_len == 0) {
        debug_log(DLOG_ERROR, "%s %d failed:input param error.", __FUNCTION__, __LINE__);
        return COMP_CODE_INVALID_FIELD;
    }

    ret = snprintf_s(actionssuc, ssuc_len, ssuc_len - 1, PL_OP_LOG_RST_T, fru_device_id);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
    ret = snprintf_s(actionsfail, fail_len, fail_len - 1, PL_OP_LOG_RST_F, fru_device_id);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }

    
    if (PAYLOAD_POWER_STATE_OFF == pp_get_pwr_state(fru_device_id)) {
        debug_log(DLOG_ERROR, " %s %d   fail\n", __FUNCTION__, __LINE__);
        
        ret = COMP_CODE_STATUS_INVALID;
        return ret;
    }

    ret = hop_reset(fru_device_id);
    
    if (ret != COMP_CODE_SUCCESS) {
        ret = COMP_CODE_UNSUPPORT;
        debug_log(DLOG_ERROR, " %s %d   fail\n", __FUNCTION__, __LINE__);
        return ret;
    }

    if (ret == COMP_CODE_SUCCESS && fru_device_id == 0) {
        // 记录重启原因
        pp_set_restart_action(SRC_CHAN_NUM_RPC, restart_cause);
        pp_set_restart_cause();
    }

    return ret;
}


LOCAL gint32 pp_method_fru_control_graceful_reboot(guchar fru_device_id, GSList *caller_info, gchar *actionssuc,
    guint32 ssuc_len, gchar *actionsfail, guint32 fail_len)
{
    gint32 ret;
    guchar restart_cause = (caller_info == NULL) ? RESTART_CAUSE_OEM : RESTART_CAUSE_CHASSISCONTROL;
    
    if (actionssuc == NULL || actionsfail == NULL || ssuc_len == 0 || fail_len == 0) {
        debug_log(DLOG_ERROR, "%s %d failed:input param error.", __FUNCTION__, __LINE__);
        return COMP_CODE_INVALID_FIELD;
    }
    ret = snprintf_s(actionssuc, ssuc_len, ssuc_len - 1, PL_OP_LOG_COLD_RST_T, fru_device_id);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
    ret = snprintf_s(actionsfail, fail_len, fail_len - 1, PL_OP_LOG_COLD_RST_F, fru_device_id);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }

    
    if (fru_device_id != CHASSIS_FRU_ID) {
        ret = COMP_CODE_UNSUPPORT;
        debug_log(DLOG_ERROR, " %s %d   fail\n", __FUNCTION__, __LINE__);
        return ret;
    }
    
    if (pp_get_pwr_state(CHASSIS_FRU_ID) == PAYLOAD_POWER_STATE_OFF) {
        ret = COMP_CODE_STATUS_INVALID;
        debug_log(DLOG_ERROR, " %s %d   fail\n", __FUNCTION__, __LINE__);
        return ret;
    }

    ret = hop_powercycle(fru_device_id);
    if (ret == COMP_CODE_SUCCESS && fru_device_id == 0) {
        pp_set_restart_action(SRC_CHAN_NUM_RPC, restart_cause);
        pp_set_restart_cause();
    } else {
        
        if (ret == COMP_CODE_STATUS_INVALID) {
            ret = COMP_CODE_UNSUPPORT;
            debug_log(DLOG_ERROR, " %s %d   fail\n", __FUNCTION__, __LINE__);
            return ret;
        }
    }
    return ret;
}


LOCAL gint32 pp_method_fru_control_diagnose_interrupt(guchar fru_device_id, GSList *caller_info, gchar *actionssuc,
    guint32 ssuc_len, gchar *actionsfail, guint32 fail_len)
{
    gint32 ret;

    
    if (actionssuc == NULL || actionsfail == NULL || ssuc_len == 0 || fail_len == 0) {
        debug_log(DLOG_ERROR, "%s %d failed:input param error.", __FUNCTION__, __LINE__);
        return COMP_CODE_INVALID_FIELD;
    }
    ret = snprintf_s(actionssuc, ssuc_len, ssuc_len - 1, PL_OP_LOG_DIAG_INTR_T, fru_device_id);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
    ret = snprintf_s(actionsfail, fail_len, fail_len - 1, PL_OP_LOG_DIAG_INTR_F, fru_device_id);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
    
    
    if (hop_get_pwr_signal(CHASSIS_FRU_ID) == HARD_POWER_OFF) {
        ret = COMP_CODE_STATUS_INVALID;
        debug_log(DLOG_ERROR, "power off and can't diag interrupt action.\r\n");
        return ret;
    }
    ret = hop_diag_interrupt(fru_device_id);
    if (ret == COMP_CODE_STATUS_INVALID) {
        ret = COMP_CODE_UNSUPPORT;
        debug_log(DLOG_ERROR, " %s %d   fail\n", __FUNCTION__, __LINE__);
        return ret;
    }
    return ret;
}


LOCAL gint32 pp_method_fru_control_poweron(guchar fru_device_id, GSList *caller_info, gchar *actionssuc,
    guint32 ssuc_len, gchar *actionsfail, guint32 fail_len)
{
    gint32 ret;
    guchar restart_cause = (caller_info == NULL) ? RESTART_CAUSE_OEM : RESTART_CAUSE_CHASSISCONTROL;

    
    if (actionssuc == NULL || actionsfail == NULL || ssuc_len == 0 || fail_len == 0) {
        debug_log(DLOG_ERROR, "%s %d failed:input param error.", __FUNCTION__, __LINE__);
        return COMP_CODE_INVALID_FIELD;
    }

    ret = snprintf_s(actionssuc, ssuc_len, ssuc_len - 1, PL_OP_LOG_PWR_ON_T, fru_device_id);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
    (void)snprintf_s(actionsfail, fail_len, fail_len - 1, "FRU%u power on failed\r\n", fru_device_id);

    
    ret = pp_check_active_option(ACTIVATE_DEACTIVATE_FRU, fru_device_id);
    
    if (ret == RET_ERR) {
        ret = COMP_CODE_UNSUPPORT;
        debug_log(DLOG_ERROR, " %s %d   fail\n", __FUNCTION__, __LINE__);
        return ret;
    }
    
    ret = pp_fru_pwr_on(SRC_CHAN_NUM_RPC, fru_device_id, restart_cause);
    if (ret == POWER_ON_FAILED_BY_BIOS_UP && RET_OK == pp_called_by_mach2mach_interface(caller_info)) {
        
        ret = COMP_CODE_STATUS_INVALID;
    }
    return ret;
}


LOCAL gint32 pp_method_fru_control_poweroff(guchar fru_device_id, GSList *caller_info, gchar *actionssuc,
    guint32 ssuc_len, gchar *actionsfail, guint32 fail_len)
{
    gint32 ret;
    
    if (actionssuc == NULL || actionsfail == NULL || ssuc_len == 0 || fail_len == 0) {
        debug_log(DLOG_ERROR, "%s %d failed:input param error.", __FUNCTION__, __LINE__);
        return COMP_CODE_INVALID_FIELD;
    }

    ret = snprintf_s(actionssuc, ssuc_len, ssuc_len - 1, PL_OP_LOG_PWR_OFF_T, fru_device_id);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
    (void)snprintf_s(actionsfail, fail_len, fail_len - 1, "FRU%u normal power off failed\r\n", fru_device_id);

    
    ret = pp_check_active_option(ACTIVATE_DEACTIVATE_FRU, fru_device_id);
    
    if (ret == RET_ERR) {
        ret = COMP_CODE_UNSUPPORT;
        debug_log(DLOG_ERROR, " %s %d   fail\n", __FUNCTION__, __LINE__);
        return ret;
    }
    
    ret = hse_fru_activate(fru_device_id, HOTSWAP_DEACTIVATE_FRU, TRUE);
    return ret;
}


gint32 pp_method_fru_control(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret;
    guchar option;
    guchar fru_device_id;
    gchar actionssuc[256] = {0};  
    gchar actionsfail[256] = {0}; 

    FRU_CONTROL_FUN frucontrol_fun;
    FRU_CONTROL_FUN frucontrol_fun_arr[FRU_CONTROL_OPTION_NUM];
    (void)memset_s(frucontrol_fun_arr, sizeof(FRU_CONTROL_FUN) * FRU_CONTROL_OPTION_NUM, 0,
        sizeof(FRU_CONTROL_FUN) * FRU_CONTROL_OPTION_NUM);
    frucontrol_fun_arr[FRU_CONTROL_WARM_RESET] = pp_method_fru_control_warm_reset;
    frucontrol_fun_arr[FRU_CONTROL_COLD_RESET] = pp_method_fru_control_cold_reset;
    frucontrol_fun_arr[FRU_CONTROL_GRACEFUL_REBOOT] = pp_method_fru_control_graceful_reboot;
    frucontrol_fun_arr[FRU_CONTROL_ISSUEDIAGNOSTICINTERRUPT] = pp_method_fru_control_diagnose_interrupt;
    frucontrol_fun_arr[FRU_CONTROL_POWER_ON] = pp_method_fru_control_poweron;
    frucontrol_fun_arr[FRU_CONTROL_POWER_OFF] = pp_method_fru_control_poweroff;

    
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "Input_list can not be NULL.\r\n");
        return COMP_CODE_INVALID_FIELD;
    }
    option = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    fru_device_id = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 1));
    
    if ((fru_device_id >= hs_get_fru_num())) {
        debug_log(DLOG_ERROR, "pp_fru_pwr_ctrl fru:%u fail! fru_device_id > hs_get_fru_num \r\n", fru_device_id);
        return COMP_CODE_OUTOF_RANGE;
    }

    if ((FALSE == hs_is_fru_managed(fru_device_id))) {
        debug_log(DLOG_ERROR, "pp_fru_pwr_ctrl fru:%u fail! fru is not managed \r\n", fru_device_id);
        return COMP_CODE_OUTOF_RANGE;
    }

    
    if (option >= FRU_CONTROL_OPTION_NUM) {
        method_operation_log(caller_info, NULL, "%s", actionsfail);
        return COMP_CODE_INVALID_FIELD;
    }

    
    frucontrol_fun = frucontrol_fun_arr[option];
    if (frucontrol_fun == NULL) {
        debug_log(DLOG_ERROR, "pp_fru_pwr_ctrl option fail! fru:%u, option:%u, frucontrol_fun is null\r\n",
            fru_device_id, option);
        return COMP_CODE_UNKNOWN;
    }
    
    ret = frucontrol_fun(fru_device_id, caller_info, actionssuc, sizeof(actionssuc), actionsfail, sizeof(actionsfail));
    
    if (ret == COMP_CODE_SUCCESS) {
        method_operation_log(caller_info, NULL, "%s", actionssuc);
    }
    if (ret != COMP_CODE_SUCCESS) {
        method_operation_log(caller_info, NULL, "%s", actionsfail);
    }

    return ret;
}


gint32 pp_method_ac_cycle(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    GVariant *property_value = NULL;
    gint32 ret;

    
    if (input_list == NULL) {
        method_operation_log(caller_info, NULL, "Set AC cycle failed");
        debug_log(DLOG_ERROR, "%s Input_list can not be NULL.", __FUNCTION__);
        return RET_ERR;
    }

    property_value = (GVariant *)g_slist_nth_data(input_list, 0);
    if (property_value == NULL) {
        method_operation_log(caller_info, NULL, "Set AC cycle failed");
        debug_log(DLOG_ERROR, "%s property_value is null", __FUNCTION__);
        return RET_ERR;
    }
    
    ret = dfl_set_property_value(obj_handle, PROPERTY_PAYLOAD_AC_CYCLE_PROTECT, property_value, DF_AUTO);
    if (ret != DFL_OK) {
        method_operation_log(caller_info, NULL, "Set AC cycle failed");
        debug_log(DLOG_ERROR, "%s Set AC cycle property failed, ret: %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    ret = dfl_set_property_value(obj_handle, PROPERTY_PAYLOAD_AC_CYCLE, property_value, DF_AUTO);
    method_operation_log(caller_info, NULL, "Set AC cycle %s", (ret == DFL_OK) ? "successfully" : "failed");
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s Set AC cycle property failed, ret: %d", __FUNCTION__, ret);
    }

    debug_log(DLOG_ERROR, "Set Ac cycle successfully.");
    return ret;
}


gint32 method_set_pwr_config(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret;
    guint8 config_val;

    config_val = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    debug_log(DLOG_INFO, "%s set power config value:%d.", dfl_get_object_name(obj_handle), config_val);
    ret = dal_set_property_value_byte(obj_handle, PROPERTY_POWER_CONFIG, config_val, DF_AUTO);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s set power config val(%d) failed, ret=%d.", dfl_get_object_name(obj_handle),
            config_val, ret);
    }

    return ret;
}


gint32 pp_method_set_thermal_trip_power_policy(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s: input_list is NULL!", __FUNCTION__);
        return RET_ERR;
    }
    guint8 pwr_policy = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));

    const gchar *policy_str = (pwr_policy == 1) ? "Enable" : "Disable";
    gint32 ret = dal_set_property_value_byte(obj_handle, PROPERTY_THERMAL_TRIP_POWERPOLICY, pwr_policy, DF_SAVE);
    if (ret != RET_OK) {
        method_operation_log(caller_info, NULL, "%s the power on policy after cpu thermal trip failed", policy_str);
        return RET_ERR;
    }

    method_operation_log(caller_info, NULL, "%s the power on policy after cpu thermal trip successfully", policy_str);
    return RET_OK;
}


gint32 pp_method_set_thermal_trip_time_delay(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s: input_list is NULL!", __FUNCTION__);
        return RET_ERR;
    }
    guint32 time_delay = g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 0));
    gint32 ret = dal_set_property_value_uint32(obj_handle, PROPERTY_THERMAL_TRIP_TIMEDELAY, time_delay, DF_SAVE);
    if (ret != RET_OK) {
        method_operation_log(caller_info, NULL,
            "Set delay time to %u seconds for executing power on policy after cpu thermal trip failed", time_delay);
        return RET_ERR;
    }

    method_operation_log(caller_info, NULL,
        "Set delay time to %u seconds for executing power on policy after cpu thermal trip successfully", time_delay);
    return RET_OK;
}