
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

LOCAL void pp_state_rotate(guchar fruid, guchar action);
LOCAL guchar pp_state_oning(guchar fru_device_id, guchar action);
LOCAL guchar pp_state_offing(guchar fru_device_id, guchar action);
LOCAL guchar pp_state_waitting_on(guchar fru_device_id, guchar action);
LOCAL guchar pp_state_waitting_off(guchar fru_device_id, guchar action);
LOCAL guchar pp_state_oning_waitting_off(guchar fru_device_id, guchar action);
LOCAL guchar pp_state_offing_waitting_on(guchar fru_device_id, guchar action);
LOCAL void pp_off_task(guchar fru_device_id);

LOCAL gint32 _check_each_power_on_criteria(OBJ_HANDLE obj_handle, gpointer user_data)
{
    guint8 fruid = *(guint8 *)user_data;
    guint8 is_met = POWER_CTRL_STATUS_FORBID;
    guint8 fruid_tmp = 0;
    guint8 log_record = 0;
    gchar log_buf[MAX_INFO_LEN] = {0};

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_POWER_ON_CRITERIA_FRUID, &fruid_tmp);

    
    if (0xFF == fruid_tmp || fruid == fruid_tmp) {
        (void)dal_get_property_value_string(obj_handle, PROPERTY_POWER_ON_CRITERIA_DESCRIPTION, log_buf,
            sizeof(log_buf));

        (void)dal_get_property_value_byte(obj_handle, PROPERTY_POWER_ON_CRITERIA_IS_MET, &is_met);

        if (POWER_CTRL_STATUS_FORBID == is_met) {
            (void)dal_get_property_value_byte(obj_handle, PROPERTY_POWER_ON_CRITERIA_LOG_RECORD, &log_record);

            
            if (NO_LOG_HAS_BEEN_RECORD == log_record) {
                strategy_log(SLOG_INFO, "Fru%d %s power on criteria unmatch.", fruid, log_buf);
                debug_log(DLOG_ERROR, "Fru%d %s power on criteria unmatch.", fruid, log_buf);
                (void)dal_set_property_value_byte(obj_handle, PROPERTY_POWER_ON_CRITERIA_LOG_RECORD,
                    LOG_HAS_BEEN_RECORD, DF_AUTO);
            }

            return RET_ERR;
        } else {
            
            (void)dal_set_property_value_byte(obj_handle, PROPERTY_POWER_ON_CRITERIA_LOG_RECORD, NO_LOG_HAS_BEEN_RECORD,
                DF_AUTO);
        }

        debug_log(DLOG_DEBUG, "Fru%d %s %s", fruid, log_buf, (POWER_CTRL_STATUS_ALLOW == is_met) ? "success" : "fail");
    }

    return RET_OK;
}


LOCAL void _hop_check_power_on_criteria(guchar fru_device_id)
{
    gint32 ret;
    guint8 log_interval = 0;
    guint8 first_strategy_log_flag = 1;
    guchar acpi = ALL_POWER_GOOD_FAIL;

    
    (void)hop_get_acpi_status(fru_device_id, &acpi);
    if (ALL_POWER_GOOD_OK == acpi) {
        return;
    }

    while (1) {
        ret = dfl_foreach_object(CLASS_POWER_ON_CRITERIA, _check_each_power_on_criteria, &fru_device_id, NULL);
        
        if (ret == ERRCODE_OBJECT_NOT_EXIST) {
            return;
        }

        
        if (first_strategy_log_flag == 1) {
            first_strategy_log_flag = 0;
            strategy_log(SLOG_INFO, "Check fru%d power on criteria.", fru_device_id);
        }

        
        if (ret == DFL_OK) {
            break;
        }

        if (POWER_ON_CRITERIA_LOG_INTERVAL == log_interval) {
            debug_log(DLOG_ERROR, "Wating for fru%d power on criteria met.", fru_device_id);
            log_interval = 0;
        }
        
        vos_task_delay(1000);
        log_interval++;
    }

    
    if (first_strategy_log_flag == 0) {
        strategy_log(SLOG_INFO, "Fru%d power on criteria met, recover power on.", fru_device_id);
    }
}


guchar pp_is_power_on_criteria_met(guchar fru_device_id)
{
    gint32 ret;

    ret = dfl_foreach_object(CLASS_POWER_ON_CRITERIA, _check_each_power_on_criteria, &fru_device_id, NULL);
    
    if (ret == ERRCODE_OBJECT_NOT_EXIST) {
        return TRUE;
    }
    if (ret == DFL_OK) {
        return TRUE;
    } else {
        return FALSE;
    }
}


LOCAL void pp_on_task(guchar fru_device_id)
{
    guchar power_state;
    guchar power_ctrl_count;
    guchar ret;

    (void)prctl(PR_SET_NAME, (gulong)"PPOnTask");

    if (0 == g_power_operate_sem4) {
        debug_log(DLOG_ERROR, "%s:g_power_operate_sem4 is 0\n", __func__);
        return;
    }
    (void)vos_thread_sem4_p(g_power_operate_sem4, SEM_WAIT_FOREVER);
    (void)_hop_check_power_on_criteria(fru_device_id);
    power_state = PAYLOAD_POWER_STATE_UNSPECIFIED;
    // 上电，清除VRD生效后的动作
    pp_set_vrd_validate_method(METHOD_VRD_SET_VALIDATE_ACTION, DO_DEFAULT_AFTER_VALIDATE_VRD);
    debug_log(DLOG_ERROR, "Set action after validate vrd(default).");
    // 有可能发生上电不成功的情况，所以需要多尝试几次
    for (power_ctrl_count = 0; power_ctrl_count < POWER_ON_RETRY_NUM; power_ctrl_count++) {
        // hop是hardware operation的缩写，是实际的上下电操作(控制硬件信号去执行)
        ret = hop_on(fru_device_id);
        if (ret == TRUE) {
            // 上电成功
            power_state = PAYLOAD_POWER_STATE_ON;

            // 更新ac lost之前的电源状态
            pp_set_pwr_state_before_ac_lost(fru_device_id, PAYLOAD_POWER_STATE_ON);

            // 上电成功后给hotswap模块发消息，通知其进行状态迁移
            hse_activate_completed(fru_device_id);
            break;
        } else if (ret == FRU_HOP_ON_FAILED_BY_BIOS_ERR) {
            debug_log(DLOG_ERROR, "pp_on_task:hop_on fail due to bios verify failed");
            power_ctrl_count = POWER_ON_RETRY_NUM;
        } else {
            debug_log(DLOG_ERROR, "pp_on_task:hop_on fail\r\n");
        }
    }

    
    if (power_ctrl_count == POWER_ON_RETRY_NUM) {
        // 发送上电失败消息到hotswap，进行状态迁移
        hse_pwr_failure(fru_device_id, 1);

        // 如果在电源恢复策略执行中发现电源控制失败事件，需要记录运行日志
        if (TRUE == pp_is_pwr_restore_in_progress(fru_device_id)) {
            strategy_log(SLOG_ERROR, "Do power restore policy fail\r\n");
        }
    }
    // 无论上电操作成功与否，都已经完成，发送操作完成事件，促使上下电
    // 状态机转换到对应状态
    pp_send_evt(fru_device_id, PAYLOAD_CTRL_FINISHED, power_state);
    // 无论上电操作成功与否，都已经完成，退出电源恢复策略执行状态
    pp_set_pwr_restore_in_progress(fru_device_id, FALSE);
    (void)vos_thread_sem4_v(g_power_operate_sem4);
}


LOCAL void pp_action_off(guchar fru_device_id)
{
    TASKID taskID;

    if (vos_task_create(&taskID, "POFF", (TASK_ENTRY)pp_off_task, INT8_TO_POINTER(fru_device_id), DEFAULT_PRIORITY) !=
        0) {
        pp_send_evt(fru_device_id, PAYLOAD_CTRL_FINISHED, PAYLOAD_POWER_STATE_UNSPECIFIED);
    }
}


LOCAL void pp_action_on(guchar fru_device_id)
{
    TASKID taskID;
    guchar flag = 0;

    if (RET_OK == get_vddq_check_forcepwroff_flag(&flag)) {
        if (flag != 0) {
            debug_log(DLOG_ERROR, "get_vddq_check_forcepwroff_flag failed, flag=%d, can not power on", flag);
            return;
        }
    }

    if (vos_task_create(&taskID, "PON", (TASK_ENTRY)pp_on_task, INT8_TO_POINTER(fru_device_id), DEFAULT_PRIORITY) !=
        0) {
        pp_send_evt(fru_device_id, PAYLOAD_CTRL_FINISHED, PAYLOAD_POWER_STATE_UNSPECIFIED);
    }
}


LOCAL guchar pp_state_no_action(guchar fru_device_id, guchar action)
{
    
    switch (action) {
        case PAYLOAD_CTRL_POWERON:
            debug_log(DLOG_DEBUG, "\r\npp_state_no_action:PAYLOAD_CTRL_POWERON \r\n");
            pp_action_on(fru_device_id); // 启动上电
            return PAYLOAD_CTRL_STATE_ONING;

        case PAYLOAD_CTRL_POWEROFF:
            debug_log(DLOG_DEBUG, "\r\npp_state_no_action:PAYLOAD_CTRL_POWEROFF \r\n");
            pp_action_off(fru_device_id); // 启动下电
            return PAYLOAD_CTRL_STATE_OFFING;

        default:
            break;
    }

    return PAYLOAD_CTRL_STATE_NOACTION;
}


LOCAL void pp_off_task(guchar fru_device_id)
{
    guchar power_state;
    guchar ret_val;

    (void)prctl(PR_SET_NAME, (gulong)"PPOffTask");

    power_state = PAYLOAD_POWER_STATE_UNSPECIFIED;
    
    FRU_PAYLOAD_S *fru_payload = pp_relate_fru_payload_info();
    if (fru_payload->m_pwr_ctrl_cb[fru_device_id].m_pwr_state == PAYLOAD_POWER_STATE_ON) {
        ret_val = hop_off(fru_device_id);
        if ((PP_LONG_PUSH_POWRE_OFF_T | PP_SHORT_PUSH_POWRE_OFF_T) & ret_val) {
            // 下电成功
            power_state = PAYLOAD_POWER_STATE_OFF;

            // 更新ac lost之前的电源状态
            pp_set_pwr_state_before_ac_lost(fru_device_id, PAYLOAD_POWER_STATE_OFF);

            // 向hotswap模块发送去激活完成消息，促使热插拔状态机转换
            hse_deactivate_completed(fru_device_id);
        }
    }

    // 无论下电操作是否成功，都已经完成，发送操作完成事件，促使上下电
    // 状态机转换到对应状态
    pp_send_evt(fru_device_id, PAYLOAD_CTRL_FINISHED, power_state);
}


LOCAL guchar pp_state_oning(guchar fru_device_id, guchar action)
{
    
    switch (action) {
        case PAYLOAD_CTRL_POWEROFF:
            return PAYLOAD_CTRL_STATE_ONINGWAITINGOFF;

        case PAYLOAD_CTRL_FINISHED:
            return PAYLOAD_CTRL_STATE_NOACTION;

        default:
            break;
    }
    return PAYLOAD_CTRL_STATE_ONING;
}


LOCAL guchar pp_state_offing(guchar fru_device_id, guchar action)
{
    
    switch (action) {
        case PAYLOAD_CTRL_POWERON:
            return PAYLOAD_CTRL_STATE_OFFINGWAITINGON;

        case PAYLOAD_CTRL_FINISHED:
            return PAYLOAD_CTRL_STATE_NOACTION;

        default:

            break;
    }

    return PAYLOAD_CTRL_STATE_OFFING;
}


LOCAL guchar pp_state_waitting_on(guchar fru_device_id, guchar action)
{
    
    switch (action) {
        case PAYLOAD_CTRL_POWEROFF:
            return PAYLOAD_CTRL_STATE_NOACTION;

        case PAYLOAD_CTRL_FINISHED:
            pp_action_on(fru_device_id); // 启动上电
            return PAYLOAD_CTRL_STATE_ONING;

        default:
            break;
    }

    return PAYLOAD_CTRL_STATE_WAITINGON;
}


LOCAL guchar pp_state_waitting_off(guchar fru_device_id, guchar action)
{
    
    switch (action) {
        case PAYLOAD_CTRL_POWERON:
            return PAYLOAD_CTRL_STATE_NOACTION;

        case PAYLOAD_CTRL_FINISHED:
            pp_action_off(fru_device_id); // 启动下电
            return PAYLOAD_CTRL_STATE_OFFING;

        default:
            break;
    }

    return PAYLOAD_CTRL_STATE_WAITINGOFF;
}


LOCAL guchar pp_state_oning_waitting_off(guchar fru_device_id, guchar action)
{
    
    switch (action) {
        case PAYLOAD_CTRL_POWERON:
            return PAYLOAD_CTRL_STATE_ONING;

        case PAYLOAD_CTRL_FINISHED:
            return PAYLOAD_CTRL_STATE_WAITINGOFF;

        default:
            break;
    }

    return PAYLOAD_CTRL_STATE_ONINGWAITINGOFF;
}


LOCAL guchar pp_state_offing_waitting_on(guchar fru_device_id, guchar action)
{
    
    switch (action) {
        case PAYLOAD_CTRL_POWEROFF:
            return PAYLOAD_CTRL_STATE_OFFING;

        case PAYLOAD_CTRL_FINISHED:
            return PAYLOAD_CTRL_STATE_WAITINGON;

        default:
            break;
    }

    return PAYLOAD_CTRL_STATE_OFFINGWAITINGON;
}




guchar (*(pp_state_route[7]))(guchar, guchar) = {
    pp_state_no_action,          
    pp_state_oning,              
    pp_state_offing,             
    pp_state_waitting_on,        
    pp_state_waitting_off,       
    pp_state_oning_waitting_off, 
    pp_state_offing_waitting_on, 
};


LOCAL void pp_state_rotate(guchar fruid, guchar action)
{
    guchar ctrl_state;

    // 根据当前状态找到对应的状态机句柄，然后执行对应操作
    FRU_PAYLOAD_S *fru_payload = pp_relate_fru_payload_info();
    ctrl_state = fru_payload->m_pwr_ctrl_cb[fruid].m_ctrl_state;
    fru_payload->m_pwr_ctrl_cb[fruid].m_ctrl_state = pp_state_route[ctrl_state](fruid, action);
    // 刚进入NoAction状态，需要将其动作也设置为NoAction，否则可能会出现错误状态转换
    if (fru_payload->m_pwr_ctrl_cb[fruid].m_ctrl_state == PAYLOAD_CTRL_STATE_NOACTION) {
        fru_payload->m_pwr_ctrl_cb[fruid].m_action = PAYLOAD_CTRL_POWERNOACTION;
    }
    // 状态轮转调试信息输出，只在状态变化时打印
    if (ctrl_state != fru_payload->m_pwr_ctrl_cb[fruid].m_ctrl_state) {
        debug_log(DLOG_DEBUG, "\r\nctr=%d,action=%d,power=%d", fru_payload->m_pwr_ctrl_cb[fruid].m_ctrl_state,
            fru_payload->m_pwr_ctrl_cb[fruid].m_action, fru_payload->m_pwr_ctrl_cb[fruid].m_pwr_state);
    }
}


void pp_stop_host_checker(void)
{
    gint32 result;
    P_PAYLOAD_S p_payload = NULL;

    result = dfl_get_data_by_alias(CLASS_PAYLOAD, CHASSIS_FRU_ID, (gpointer *)&p_payload);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d data by alias failed!\r\n", CHASSIS_FRU_ID);
        return;
    }

    // 如果上一个计时任务还未结束，重新启动计数
    if (p_payload->m_host_check_timer != INVALID_TIMER_NUM) {
        g_printf("detect host already power off, stop the host checker...\r\n");
        debug_log(DLOG_ERROR, "detect host already power off, stop the host checker...\r\n");
        (void)vos_timer_delete(p_payload->m_host_check_timer);
    }

    return;
}


void pp_set_pwr_state_before_ac_lost(gchar fru_device_id, guint8 pwr_state)
{
    P_PAYLOAD_S p_payload = NULL;
    gint32 ret;
    guchar power_off_normal;

    ret = dfl_get_data_by_alias(CLASS_PAYLOAD, fru_device_id, (gpointer *)&p_payload);
    if (fru_device_id == 0) {
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "get fru%d data by alias failed!\r\n", fru_device_id);
            return;
        }
    } else {
        if (ret != DFL_OK) {
            debug_log(DLOG_DEBUG, "get fru%d data by alias failed!\r\n", fru_device_id);
            return;
        }
    }

    if ((is_support_storage_payload() == TRUE) && (pwr_state == PAYLOAD_POWER_STATE_OFF)) {
        power_off_normal = pp_get_power_off_normal_flag();
        
        if (power_off_normal == FALSE) {
            debug_log(DLOG_ERROR, "Abnormal power off, no need to save power state.");
            return;
        }
    }

    // 更新ac lost之前的电源状态
    p_payload->m_pwr_state_before_ac_lost = pwr_state;
    per_save((guint8 *)&p_payload->m_pwr_state_before_ac_lost);
    debug_log(DLOG_ERROR, "Persist save powerstate %d, ret %d.", pwr_state, ret);
    payload_set_aclost_status(p_payload->m_pwr_state_before_ac_lost);

    return;
}


LOCAL void pp_update_chassis_pwr_state(void)
{
    gint32 result;
    OBJ_HANDLE object_handle;
    GVariant *property_data = NULL;
    guint8 temp_data;
    guint8 old_power_status;

    // 更新ChassisPayload对象的ChassisPowerState状态
    result = dfl_get_binded_object(CLASS_PAYLOAD, CHASSIS_FRU_ID, &object_handle);
    if (result == DFL_OK) {
        FRU_PAYLOAD_S *fru_payload = pp_relate_fru_payload_info();
        // 更新ChassisPowerState，直接对应于chassis fru的power state
        temp_data = fru_payload->m_pwr_ctrl_cb[CHASSIS_FRU_ID].m_pwr_state;
        // 获取当前内存中电源状态
        result = dfl_get_property_value(object_handle, PROPERTY_PAYLOAD_CHASSPWR_STATE, &property_data);
        if (result != DFL_OK) {
            old_power_status = (HARD_POWER_ON == temp_data) ? HARD_POWER_OFF : HARD_POWER_ON;
            debug_log(DLOG_ERROR, "get ChassisPayload object ChassisPowerState fail!(result=%d)\r\n", result);
        } else {
            old_power_status = g_variant_get_byte(property_data);
            g_variant_unref(property_data);
        }
        if (old_power_status != temp_data) {
            property_data = g_variant_new_byte(temp_data);
            result = dfl_set_property_value(object_handle, PROPERTY_PAYLOAD_CHASSPWR_STATE, property_data, DF_NONE);
            g_variant_unref(property_data);
            if (result != DFL_OK) {
                debug_log(DLOG_ERROR, "set ChassisPayload object ChassisPowerState fail!(result=%d)\r\n", result);
            }
        }
    } else {
        debug_log(DLOG_ERROR, "get ChassisPayload object fail!(result=%d)\r\n", result);
    }
}


LOCAL void recover_hotswap_state_to_m4(guchar fruid, guchar cur_state)
{
    static gulong previous_m6_ms_cnt = 0;
    guint32 power_off_timeout = 0;
    OBJ_HANDLE object_handle = 0;
    guint32 power_off_timeout_en = 0;

#define FORCE_POWEROFF_TIME 6 // 强制下电时间为6s

    (void)dal_get_specific_object_byte(CLASS_PAYLOAD, PROPERTY_PAYLOAD_FRUID, CHASSIS_FRU_ID, &object_handle);
    (void)dal_get_property_value_uint32(object_handle, PROPERTY_PAYLOAD_PWROFF_TM_EN, &power_off_timeout_en);

    if (TIMEOUT_ENABLE == power_off_timeout_en) {
        (void)dal_get_property_value_uint32(object_handle, PROPERTY_PAYLOAD_PWROFF_TM, &power_off_timeout);
    }

    
    if (FRU_HS_STATE_M6 == cur_state) {
        if (previous_m6_ms_cnt == 0) {
            
            previous_m6_ms_cnt = vos_tick_get();
            
        } else if ((vos_tick_get() - previous_m6_ms_cnt) / 1000 > (power_off_timeout + FORCE_POWEROFF_TIME)) {
            
            debug_log(DLOG_ERROR, "unnormal status recovery.");
            (void)hse_unnormal_status_recovery(fruid);
            
            previous_m6_ms_cnt = 0;
        }
    } else {
        previous_m6_ms_cnt = 0;
    }
}


LOCAL void pp_check_pwr_mutation(guchar fruid)
{
    static guchar previous_status = 0;
    guchar status = 0;
    guchar flag;
    OBJ_HANDLE object_handle;
    guchar board_insert = 0;
    guchar cur_state;

    cur_state = hs_get_cur_state(fruid);
    FRU_PAYLOAD_S *fru_payload = pp_relate_fru_payload_info();
    if ((cur_state > FRU_HS_STATE_M3)) {
        // 定期检测电源状态突变 ON(pwr_stat) OFF(hardware) M4, M6 OFF OFF M4,M6
        // 在上电状态下突然检测到电源失效
        if (HARD_POWER_OFF == hop_get_pwr_signal(fruid)) {
            
            if (previous_status == PAYLOAD_AC_LOST) {
                return;
            }
            pp_send_evt(fruid, PAYLOAD_CTRL_FINISHED, PAYLOAD_POWER_STATE_OFF);
            if (FRU_HS_STATE_M6 == hs_get_cur_state(fruid)) {
                // m6-走正常处理流程；如果M6时电源跌落，则原因为正常流程
                hse_deactivate_completed(fruid);
            } else {
                // m4-走异常掉电处理流程
                // 首先通知热插拔状态机进行状态跟随指示，避免状态不一致
                hse_pwr_failure(fruid, 1);
            }
            // 检测到掉电，则停止监听业务是否启动定时器
            pp_stop_host_checker();
            // 因为在拔掉电源模块时，可能出现在这里判断为AC Lost前为下电的
            // 所以为了提高准确度，改为由硬件提供判断寄存器来检测
            (void)hop_get_power_before_ac_lost(&status);
            // 确实在AC拔掉之前已经下电才更新保存的状态，这样做是为了
            // 解决突然拔掉AC时，由于容性器件导致BMC还能运行一段时间
            // 而产生的检测和记录的power state before ac lost不准确
            // 在实施这种方法前，经常拔掉AC前是上电状态的，记录为下电状态了
            if (status != PAYLOAD_AC_LOST) {
                pp_set_pwr_state_before_ac_lost(fruid, PAYLOAD_POWER_STATE_OFF);
            }

            // 本地提示信息，提醒状态发生突变
            g_printf("Detect fru%d payload power dropped.\n", fruid);
            debug_log(DLOG_ERROR, "%s %d:Detect fru%d payload power dropped.hotswap:M%d m_pwr_state:%d Hardware:%d\n",
                __FUNCTION__, __LINE__, fruid, hs_get_cur_state(fruid), fru_payload->m_pwr_ctrl_cb[fruid].m_pwr_state,
                HARD_POWER_OFF);
        } else { 
            
            
            
            previous_status = 0;

            
            if (fru_payload->m_pwr_ctrl_cb[fruid].m_pwr_state == PAYLOAD_POWER_STATE_OFF) {
                debug_log(DLOG_ERROR,
                    "%s %d:Detect fru%d payload status abnormal, hotswap:M%d m_pwr_state:%d Hardware:%d\n",
                    __FUNCTION__, __LINE__, fruid, hs_get_cur_state(fruid),
                    fru_payload->m_pwr_ctrl_cb[fruid].m_pwr_state, HARD_POWER_ON);
                fru_payload->m_pwr_ctrl_cb[fruid].m_pwr_state = PAYLOAD_POWER_STATE_ON;
                
                pp_send_evt(fruid, PAYLOAD_CTRL_FINISHED, PAYLOAD_POWER_STATE_ON);
                

                
                (void)hse_unnormal_status_recovery(fruid);
                if (fruid == CHASSIS_FRU_ID) {
                    pp_update_chassis_pwr_state();
                    pp_set_pwr_state_before_ac_lost(fruid, PAYLOAD_POWER_STATE_ON);
                }
            } else {
                
                recover_hotswap_state_to_m4(fruid, cur_state);
            }
        }
    } else if (hs_get_cur_state(fruid) <= FRU_HS_STATE_M3) {
        
        if (HARD_POWER_OFF == hop_get_pwr_signal(fruid)) {
            
            
            if (fru_payload->m_pwr_ctrl_cb[fruid].m_pwr_state == PAYLOAD_POWER_STATE_ON) {
                
                (void)hop_get_power_before_ac_lost(&status);
                
                if (status != PAYLOAD_AC_LOST) {
                    pp_set_pwr_state_before_ac_lost(fruid, PAYLOAD_POWER_STATE_OFF);
                }
                pp_send_evt(fruid, PAYLOAD_CTRL_FINISHED, PAYLOAD_POWER_STATE_OFF);
                hse_pwr_failure(fruid, 1);
                debug_log(DLOG_ERROR,
                    "%s %d:Detect fru%d payload power dropped.hotswap:M%d m_pwr_state:%d Hardware:%d\n", __FUNCTION__,
                    __LINE__, fruid, hs_get_cur_state(fruid), fru_payload->m_pwr_ctrl_cb[fruid].m_pwr_state,
                    HARD_POWER_OFF);
            } else {
                
                
                (void)hop_get_power_before_ac_lost(&previous_status);
                
                if (previous_status == PAYLOAD_AC_LOST) {
                    vos_task_delay(6000); 
                    pp_do_pwr_restore_process();
                    hop_set_power_before_ac_lost(0);
                    return;
                }
            }
        } else { 
            (void)hop_get_power_before_ac_lost(&previous_status);

            if (previous_status == PAYLOAD_AC_LOST) {
                hop_set_power_before_ac_lost(0);
            }
            
            flag = (hop_get_pwr_signal(fruid) == HARD_POWER_ON) || (previous_status == PAYLOAD_AC_LOST);
            if (flag == TRUE) {
                if (previous_status == PAYLOAD_AC_LOST) {
                    vos_task_delay(6000); 
                    pp_do_pwr_restore_process();
                    return;
                }
                
                
                
                (void)dal_get_specific_object_byte(CLASS_PAYLOAD, PROPERTY_PAYLOAD_FRUID, CHASSIS_FRU_ID,
                    &object_handle);
                (void)dal_get_property_value_byte(object_handle, PROPERTY_PAYLOAD_STABLE_STATUS, &board_insert);

                
                if (board_insert != 0) {
                    return;
                }
                
                
                
                
                pp_set_pwr_state_before_ac_lost(fruid, PAYLOAD_POWER_STATE_ON);

                
                pp_send_evt(fruid, PAYLOAD_CTRL_FINISHED, PAYLOAD_POWER_STATE_ON);
                hse_activate_completed(fruid);

                
                debug_log(DLOG_DEBUG, "pp_check_pwr_mutation NOW clear power_drop src because system is ON.\r\n");
                
                (void)hop_clear_power_flag(fruid, PROPERTY_PAYLOAD_PWR_DROP_SRC);
                
                (void)hop_set_listen_sel_flag(fruid, PROPERTY_PAYLOAD_PWRSIG_DROP, FALSE);

                g_printf("\r\nDetect fru%d payload power is already on.\n", fruid);
                debug_log(DLOG_ERROR,
                    "%s:Detect fru%d payload power is already on.hotswap:M%d m_pwr_state:%d Hardware:%d\n",
                    __FUNCTION__, fruid, hs_get_cur_state(fruid), fru_payload->m_pwr_ctrl_cb[fruid].m_pwr_state,
                    HARD_POWER_ON);
            }
        }
    }
}


LOCAL void pp_check_pwr_on_tm(guchar fruid)
{
    gint32 ret_val;
    guchar power_tm_flag = 0;
    // 查询寄存器是否有上电超时事件
    ret_val = hop_get_power_flag(fruid, PROPERTY_PAYLOAD_PWRON_TM_SRC, &power_tm_flag);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, "hop_get_power_flag fail(Ret:%d).\r\n", ret_val);
        return;
    }
    if (TRUE == power_tm_flag) {
        
        (void)hop_set_pwr_flag(fruid, PROPERTY_POWER_ON_TM, TRUE);
    }
    return;
}


LOCAL void pp_check_pwr_sig_drop(guchar fruid)
{
    gint32 ret_val;
    guchar pwr_drop_flag = 0;
    // 查询寄存器是否有上电超时事件
    ret_val = hop_get_power_flag(fruid, PROPERTY_PAYLOAD_PWR_DROP_SRC, &pwr_drop_flag);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, "hop_get_power_flag fail(Ret:%d).\r\n", ret_val);
        return;
    }
    
    if (TRUE == pwr_drop_flag) {
        
        (void)hop_set_listen_sel_flag(fruid, PROPERTY_PAYLOAD_PWRSIG_DROP, TRUE);
    } else {
        
        (void)hop_set_pwr_flag(fruid, PROPERTY_POWER_ON_TM, FALSE);
    }
    return;
}


void pp_update_fru_pwr_state(guint8 fru_id)
{
    gint32 result;
    OBJ_HANDLE object_handle;
    GVariant *property_data = NULL;
    guint8 temp_data;
    guint8 old_power_status;

    // 更新FruPayload对象的PowerState状态
    result = dfl_get_binded_object(CLASS_FRUPAYLOAD, fru_id, &object_handle);
    if (result == DFL_OK) {
        // 更新当前PowerState状态
        FRU_PAYLOAD_S *fru_payload = pp_relate_fru_payload_info();
        temp_data = fru_payload->m_pwr_ctrl_cb[fru_id].m_pwr_state;
        // 获取当前内存中电源状态
        result = dfl_get_property_value(object_handle, PROPERTY_FRUPAYLOAD_POWERSTATE, &property_data);
        if (result != DFL_OK) {
            old_power_status = (HARD_POWER_ON == temp_data) ? HARD_POWER_OFF : HARD_POWER_ON;
            debug_log(DLOG_ERROR, "get FruPayload%d object PowerState fail!(result=%d)\r\n", fru_id, result);
        } else {
            old_power_status = g_variant_get_byte(property_data);
            g_variant_unref(property_data);
        }

        debug_log(DLOG_ERROR, "fruid = %d, old_power_status = %d, temp_data = %d", fru_id, old_power_status, temp_data);

        if (old_power_status != temp_data) {
            property_data = g_variant_new_byte(temp_data);
            result = dfl_set_property_value(object_handle, PROPERTY_FRUPAYLOAD_POWERSTATE, property_data, DF_NONE);
            g_variant_unref(property_data);
            debug_log(DLOG_ERROR, "set FruPayload%d object PowerState=%d!\r\n", fru_id, temp_data);
            if (result != DFL_OK) {
                debug_log(DLOG_ERROR, "set FruPayload%d object PowerState fail!(result=%d)\r\n", fru_id, result);
            }
        }
    } else {
        debug_log(DLOG_ERROR, "get FruPayload%d object fail!(result=%d)\r\n", fru_id, result);
    }
    // 同步更新chassispayload对象的属性
    if (fru_id == CHASSIS_FRU_ID) {
        pp_update_chassis_pwr_state();
    }
}


LOCAL void pp_on_state_changed(guchar fruid)
{
    // 检测host复位状态
    (void)hop_check_host_reset(fruid);
    // 检测电源状态突变
    pp_check_pwr_mutation(fruid);
    // 检测acpi和pwrpg信号的一致性
    (void)hop_check_power_status(fruid);
    // 检测上电是否超时
    pp_check_pwr_on_tm(fruid);
    // 检测是否异常掉电
    pp_check_pwr_sig_drop(fruid);
}


void pp_ctrl_task(guchar fruid)
{
    gpointer pwr_ctrl_evt = NULL;
    guint32 result;

    
    if ((fruid >= hs_get_fru_num()) || (FALSE == hs_is_fru_managed(fruid))) {
        return;
    }

    g_printf("Managed FRU %d Power control task create....................[Success]\r\n", fruid);

    (void)prctl(PR_SET_NAME, (gulong)"PwrCtrlTask");

    
    for (;;) {
        FRU_PAYLOAD_S *fru_payload = pp_relate_fru_payload_info();
        
        result = vos_queue_receive((GAsyncQueue *)(fru_payload->m_power_ctrl_evt_qid[fruid]), &pwr_ctrl_evt,
            MSG_RECV_TIMEOUT_1S);
        if (result == RET_OK) {
            
            pp_on_evt(fruid, (PPWR_CTRL_EVT_S)pwr_ctrl_evt);
        }
        
        pp_state_rotate(fruid, fru_payload->m_pwr_ctrl_cb[fruid].m_action);
        
        pp_on_state_changed(fruid);
    }
}