
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

#define PROPERTY_PAYLOAD_SYS_STSRTUP_STATUS "SysStartupStatus"

LOCAL FRU_PAYLOAD_S g_fru_payload;


LOCAL guint8 g_aclost_tmp = 0x0;
LOCAL guchar g_power_on_by_ipmi_flag = FALSE;

LOCAL TASKID g_composition_config = 0;


LOCAL void pp_execute_host_checker(void);

LOCAL void dft_set_fanlevel(void);

GET_POWER_INFO_S g_get_power_info;


LOCAL guchar g_power_normal_off_flag = FALSE; 


void pp_set_power_off_normal_flag(guchar value)
{
    g_power_normal_off_flag = value;
    return;
}


guchar pp_get_power_off_normal_flag(void)
{
    return g_power_normal_off_flag;
}


FRU_PAYLOAD_S *pp_relate_fru_payload_info(void)
{
    return &g_fru_payload;
}

void pp_set_power_on_by_ipmi_flag(guchar value)
{
    g_power_on_by_ipmi_flag = value;
    return;
}

guchar pp_get_power_on_by_ipmi_flag(void)
{
    return g_power_on_by_ipmi_flag;
}


void pp_on_evt(guchar fruid, PPWR_CTRL_EVT_S power_ctrl_event)
{
    debug_log_limit_when_fail(DLOG_LIMIT_60, TRUE, "pp_on_evt: action=%d, power_state=%d\r\n", power_ctrl_event->action,
        power_ctrl_event->power_state);
    FRU_PAYLOAD_S *fru_payload = pp_relate_fru_payload_info();
    fru_payload->m_pwr_ctrl_cb[fruid].m_action = power_ctrl_event->action;
    // 未知电源状态消息，不更新fru的电源状态
    if (power_ctrl_event->power_state != PAYLOAD_POWER_STATE_UNSPECIFIED) {
        fru_payload->m_pwr_ctrl_cb[fruid].m_pwr_state = power_ctrl_event->power_state;
        pp_update_fru_pwr_state(fruid);
    }
    g_free(power_ctrl_event);
}


void pp_send_evt(guchar fruid, guchar action, guchar power_state)
{
    PPWR_CTRL_EVT_S pwr_ctrl_evt;

    // 只有可管理fru才有电源控制状态
    if (fruid >= hs_get_fru_num()) {
        debug_log(DLOG_ERROR, "%s: fruid(%u) out of range.", __FUNCTION__, fruid);
        return;
    }
    if (FALSE == hs_is_fru_managed(fruid)) {
        debug_log(DLOG_DEBUG, "%s: fruid(%u) can not be managed.", __FUNCTION__, fruid);
        return;
    }
    pwr_ctrl_evt = (PPWR_CTRL_EVT_S)g_malloc0(sizeof(PWR_CTRL_EVT_S));
    if (pwr_ctrl_evt == NULL) {
        debug_log(DLOG_ERROR, "%s: malloc pwr_ctrl_evt failed.", __FUNCTION__);
        return;
    }
    pwr_ctrl_evt->action = action;
    pwr_ctrl_evt->power_state = power_state;
    FRU_PAYLOAD_S *fru_payload = pp_relate_fru_payload_info();
    debug_log(DLOG_DEBUG, "pwr_ctrl_evt: action=%d, power_state=%d\r\n", pwr_ctrl_evt->action,
        pwr_ctrl_evt->power_state);
    if (RET_OK != vos_queue_send(fru_payload->m_power_ctrl_evt_qid[fruid], (gpointer)pwr_ctrl_evt)) {
        g_free(pwr_ctrl_evt);
        debug_log(DLOG_ERROR, "pp_send_evt: que send failed.\r\n");
    }
}


static void pp_update_sys_startup_status(void)
{
    gint32 result;
    OBJ_HANDLE object_handle = 0x00;
    result = dfl_get_binded_object(CLASS_PAYLOAD, CHASSIS_FRU_ID, &object_handle);
    debug_log(DLOG_ERROR, "BIOS startup failed, the alarm SysBiosStartupFailedMntr triggered.");
    if (result != RET_OK) {
        debug_log(DLOG_ERROR, "get ChassisPayload object fail!(result=%d)", result);
        return;
    }
    
    result = dal_set_property_value_byte(object_handle, PROPERTY_PAYLOAD_SYS_STSRTUP_STATUS, 1, DF_NONE);
    if (result != RET_OK) {
        debug_log(DLOG_ERROR, "set ChassisPayload.SysStartupStatus[0x%08x] fail!(result=%d)", 1, result);
    }
    // 连续设置两条相同属性间延时500ms
    vos_task_delay(500);
    
    result = dal_set_property_value_byte(object_handle, PROPERTY_PAYLOAD_SYS_STSRTUP_STATUS, 0, DF_NONE);
    if (result != RET_OK) {
        debug_log(DLOG_ERROR, "set ChassisPayload.SysStartupStatus[0x%08x] fail!(result=%d)", 0, result);
    }
}


LOCAL void pp_set_power_timeout_count(guint8 type)
{
    gint32 result;
    P_PAYLOAD_S p_payload = NULL;

    result = dfl_get_data_by_alias(CLASS_PAYLOAD, CHASSIS_FRU_ID, (gpointer *)&p_payload);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d data by alias failed!\r\n", CHASSIS_FRU_ID);
        return;
    }
    if (type == TIMEOUT_COUNT_CLEAR) {
        p_payload->m_pwr_timeout_count = 0;
    } else if (type == TIMEOUT_COUNT_INCREASE) {
        p_payload->m_pwr_timeout_count += 1;
    }

    return;
}


void pp_start_host_checker(void)
{
    guint32 host_start_timeout;
    gint32 result;
    P_PAYLOAD_S p_payload = NULL;

    result = dfl_get_data_by_alias(CLASS_PAYLOAD, CHASSIS_FRU_ID, (gpointer *)&p_payload);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d data by alias failed!\r\n", CHASSIS_FRU_ID);
        return;
    }

    // 先设置host运行状态未down */
    pp_set_host_down();

    // 如果上一个计时任务还未结束，重新启动计数 */
    if (p_payload->m_host_check_timer != INVALID_TIMER_NUM) {
        (void)vos_timer_restart(p_payload->m_host_check_timer);
    } else {
		
        if (p_payload->m_pwr_timeout_count >= 3) { // 3表示BMC重启系统的最大次数
            g_printf("BMC has already restarted the host 3 times due to host checker, "
                "therefore don't start the host checker this time.\r\n");
            debug_log(DLOG_ERROR, "BMC has already restarted the host 3 times due to host checker, "
                "therefore don't start the host checker this time.");
                
            pp_update_sys_startup_status();
            pp_set_power_timeout_count(TIMEOUT_COUNT_CLEAR);
            return;
        }
        // 产品配置不同,启动所需的时间也不同，所以此项可以配置 */
        host_start_timeout = hop_get_host_start_timeout();
        // 创建定时器任务 */
        (void)vos_timer_create(&p_payload->m_host_check_timer, (TIMERFUN)pp_execute_host_checker, host_start_timeout,
            0);
    }
    g_printf("detect a host reset occured, start the host checker...\r\n");
    debug_log(DLOG_ERROR, "detect a host reset occured, start the host checker...");
    return;
}


LOCAL void pp_execute_host_checker(void)
{
    // 如果计时结束，host仍然为运行完成，说明host可能已经发生了死机之类的故障
    // 这时候则需要对其进行电源循环操作，使其从故障中恢复
    if (pp_is_host_up() == FALSE) {
        gint32 result;
        OBJ_HANDLE object_handle = 0;
        GVariant *property_data = NULL;
        guint8 boot_tmout_action = WATCHDOG_TIMEOUT_PWRCYCLE; 

        
        (void)hop_ctrl_bios_switch(CHASSIS_FRU_ID, SW_BIOS_ACTION_TIMER);
        
        result = dfl_get_binded_object(CLASS_PAYLOAD, CHASSIS_FRU_ID, &object_handle);
        if (result == DFL_OK) {
            result = dfl_get_property_value(object_handle, PROPERTY_PAYLOAD_BOOT_TMOUT_ACTION, &property_data);
            if (result == DFL_OK) {
                boot_tmout_action = g_variant_get_byte(property_data);
                g_variant_unref(property_data);
            }
        }

        if (boot_tmout_action == WATCHDOG_TIMEOUT_PWRCYCLE) {
            
            if (pp_fru_pwr_ctrl(CHASSIS_FRU_ID, POWER_CYCLE) == COMP_CODE_SUCCESS) {
                pp_set_restart_action(SRC_CHAN_NUM_RPC, RESTART_CAUSE_OEM);
                pp_set_restart_cause();
                pp_set_power_timeout_count(TIMEOUT_COUNT_INCREASE);
            }
            g_printf("host checker timeout,system will power cycle!\r\n");
            debug_log(DLOG_ERROR, "host checker timeout,system will power cycle!\r\n");
        }
    } else {
        pp_set_power_timeout_count(TIMEOUT_COUNT_CLEAR);
        g_printf("host start successfully, host checker exit.\r\n");
        debug_log(DLOG_ERROR, "host start successfully, host checker exit.\r\n");
    }
}


LOCAL void process_powercycle_persistent_data(guint32 fruid)
{
    guint8 acpi_status = HARD_POWER_ON;
    OBJ_HANDLE obj_handle = 0;

    if (pp_is_cycle_in_progress(fruid) == TRUE) {
        (void)dal_get_specific_object_byte(CLASS_PAYLOAD, PROPERTY_PAYLOAD_FRUID, fruid, &obj_handle);
        (void)dal_get_extern_value_byte(obj_handle, PROPERTY_PAYLOAD_ACPI_STATUS, &acpi_status, DF_AUTO);
        if (acpi_status == HARD_POWER_ON && get_offing_state_flag() != TRUE) {
            // 当前上电状态,则清除掉全局变量标志、清除持久化数据,以免误执行powercycle影响X86业务
            pp_set_cycle_progress_state(fruid, FALSE);
            pp_set_cycle_interval(fruid, 0);
            pp_save_cycle_param();
            debug_log(DLOG_ERROR, "A power cycle of FRU%u is ignored because FRU%u is already powered on.",
                fruid, fruid);
            maintenance_log_v2(MLOG_INFO, FC__PUBLIC_OK,
                "A power cycle of FRU%u is ignored because FRU%u is already powered on.", fruid, fruid);
        } else {
            // 当前下电,则继续执行powercycle,继续执行上电
            maintenance_log_v2(MLOG_INFO, FC__PUBLIC_OK,
                "A power cycle of FRU%u is not completed and needs to be processed according to persistance data.",
                fruid);
            debug_log(DLOG_ERROR,
                "A power cycle of FRU%u is not completed and can not clear persistance data.", fruid);
        }
    }
}

LOCAL void pp_per_data_init(void)
{
    guint32 fru_num;
    PER_S per_data;
    gint32 result;
    guchar i;
    gchar tmpkey[32] = {0}; 
    PAYLOAD_S *p_payload = NULL;

    fru_num = hs_get_fru_num();
    for (i = 0; i < fru_num; i++) {
        result = dfl_get_data_by_alias(CLASS_PAYLOAD, i, (gpointer *)&p_payload);
        if (result != DFL_OK) {
            continue;
        }
        
        
        
        (void)snprintf_s(tmpkey, sizeof(tmpkey), sizeof(tmpkey) - 1, "ACLostPwrStat%u", i);
        per_data.key = tmpkey;
        per_data.mode = PER_MODE_POWER_OFF;
        per_data.data = (guint8 *)&p_payload->m_pwr_state_before_ac_lost;
        per_data.len = fru_num * sizeof(guint8);
        (void)per_init(&per_data, 1);
        
        
        (void)memset_s(tmpkey, sizeof(tmpkey), 0, sizeof(tmpkey));
        (void)snprintf_s(tmpkey, sizeof(tmpkey), sizeof(tmpkey) - 1, "RestorePolicy%u", i);
        per_data.key = tmpkey;
        per_data.mode = PER_MODE_POWER_OFF;
        per_data.data = (guint8 *)&p_payload->m_pwr_restore_policy;
        per_data.len = sizeof(p_payload->m_pwr_restore_policy);
        (void)per_init(&per_data, 1);
        
        
        (void)memset_s(tmpkey, sizeof(tmpkey), 0, sizeof(tmpkey));
        (void)snprintf_s(tmpkey, sizeof(tmpkey), sizeof(tmpkey) - 1, "RestartCause%u", i);
        per_data.key = tmpkey;
        per_data.mode = PER_MODE_RESET;
        per_data.data = (guint8 *)&p_payload->m_restart_cause;
        per_data.len = sizeof(p_payload->m_restart_cause);
        (void)per_init(&per_data, 1);
        
        
        (void)memset_s(tmpkey, sizeof(tmpkey), 0, sizeof(tmpkey));
        (void)snprintf_s(tmpkey, sizeof(tmpkey), sizeof(tmpkey) - 1, "PwrCycleCB%u", i);
        per_data.key = tmpkey;
        per_data.mode = PER_MODE_RESET;
        per_data.data = (guint8 *)g_fru_payload.m_pwr_cycle_cb;
        per_data.len = fru_num * sizeof(PWR_CYCLE_CB_S);
        (void)per_init(&per_data, 1);

        process_powercycle_persistent_data(i);

        
        
        (void)memset_s(tmpkey, sizeof(tmpkey), 0, sizeof(tmpkey));
        (void)snprintf_s(tmpkey, sizeof(tmpkey), sizeof(tmpkey) - 1, "PowerOffLock%u", i);
        per_data.key = tmpkey;
        per_data.mode = PER_MODE_RESET;
        per_data.data = (guint8 *)g_fru_payload.m_pwr_off_lock;
        per_data.len = fru_num * sizeof(guint8);
        (void)per_init(&per_data, 1);
    }
}

void payload_get_aclost_status(guchar *ACLostStatus)
{
    debug_log(DLOG_DEBUG, "payload_get_aclost_status g_aclost_tmp = %d!\n", g_aclost_tmp);
    *ACLostStatus = g_aclost_tmp;

    return;
}


void payload_set_aclost_status(guchar ACLostStatus)
{
    g_aclost_tmp = ACLostStatus;

    return;
}

LOCAL void pp_save_orign_aclost_status(void)
{
    guint32 fru_num;
    gint32 result;
    guchar i;
    PAYLOAD_S *p_payload = NULL;

    fru_num = hs_get_fru_num();

    for (i = 0; i < fru_num; i++) {
        result = dfl_get_data_by_alias(CLASS_PAYLOAD, i, (gpointer *)&p_payload);
        if (result != DFL_OK) {
            continue;
        }

        if (i == CHASSIS_FRU_ID) {
            g_aclost_tmp = p_payload->m_pwr_state_before_ac_lost;
            break;
        }
    }
}

void pp_set_power_cycle_interval(guchar seconds)
{
    gint32 result;
    P_PAYLOAD_S p_payload = NULL;

    result = dfl_get_data_by_alias(CLASS_PAYLOAD, CHASSIS_FRU_ID, (gpointer *)&p_payload);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d data by alias failed!\r\n", CHASSIS_FRU_ID);
        return;
    }
    p_payload->m_power_cycle_interval = seconds * 1000; 
}


guint32 pp_get_power_cycle_interval(void)
{
    gint32 result;
    P_PAYLOAD_S p_payload = NULL;
    guint32 product_version = 0;

    result = dfl_get_data_by_alias(CLASS_PAYLOAD, CHASSIS_FRU_ID, (gpointer *)&p_payload);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d data by alias failed!\r\n", CHASSIS_FRU_ID);
        result = dal_get_product_version_num(&product_version);
        if (result != RET_OK) {
            debug_log(DLOG_ERROR, "%s : Failed to get product version num, result = %d", __FUNCTION__, result);
            return (guint32)(CYCLE_INTERVAL_6S * 1000); 
        }
        return (product_version >= (guint32)PRODUCT_VERSION_V5) ? (guint32)(CYCLE_INTERVAL_14S * 1000)
                                                                : (guint32)(CYCLE_INTERVAL_6S * 1000); 
    }
    return p_payload->m_power_cycle_interval;
}


guchar pp_is_power_ctrl_failed(void)
{
    gint32 result;
    P_PAYLOAD_S p_payload = NULL;

    result = dfl_get_data_by_alias(CLASS_PAYLOAD, CHASSIS_FRU_ID, (gpointer *)&p_payload);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d data by alias failed!\r\n", CHASSIS_FRU_ID);
        return TRUE;
    }
    return (guchar)((p_payload->m_power_ctrl_fault.on_fault || p_payload->m_power_ctrl_fault.off_fault) ? TRUE : FALSE);
}


void pp_set_power_ctrl_fault(guchar fru_device_id, gint32 action, guchar isFailed)
{
    gint32 result;
    P_PAYLOAD_S p_payload = NULL;

    result = dfl_get_data_by_alias(CLASS_PAYLOAD, fru_device_id, (gpointer *)&p_payload);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d data by alias failed!\r\n", fru_device_id);
        return;
    }
    if (fru_device_id == CHASSIS_FRU_ID) {
        
        switch (action) {
            case POWER_ON:
                p_payload->m_power_ctrl_fault.on_fault = isFailed ? TRUE : FALSE;
                break;
            case POWER_OFF:
                p_payload->m_power_ctrl_fault.off_fault = isFailed ? TRUE : FALSE;
                break;
            default:
                return;
        }
        
        pp_update_ctrl_fault();
    }
}


LOCAL void pp_update_pwr_off_timeout_en(void)
{
    guint32 temp_data;
    gint32 result;
    OBJ_HANDLE object_handle;
    GVariant *property_data = NULL;
    P_PAYLOAD_S p_payload = NULL;
    result = dfl_get_data_by_alias(CLASS_PAYLOAD, CHASSIS_FRU_ID, (gpointer *)&p_payload);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d data by alias failed!\r\n", CHASSIS_FRU_ID);
        return;
    }
    
    result = dfl_get_binded_object(CLASS_PAYLOAD, CHASSIS_FRU_ID, &object_handle);
    if (result == DFL_OK) {
        
        temp_data = p_payload->m_power_off_timeout_en;
        property_data = g_variant_new_uint32(temp_data);
        result = dfl_set_property_value(object_handle, PROPERTY_PAYLOAD_PWROFF_TM_EN, property_data, DF_SAVE);
        g_variant_unref(property_data);
        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "set ChassisPayload object PowerOffTimeoutEN fail!\r\n");
        }
    } else {
        debug_log(DLOG_ERROR, "get ChassisPayload object fail!\r\n");
    }
}


LOCAL void pp_update_pwr_off_timeout(void)
{
    gint32 result;
    guint32 temp_data;
    GVariant *property_data = NULL;
    P_PAYLOAD_S p_payload = NULL;
    OBJ_HANDLE object_handle;
    result = dfl_get_data_by_alias(CLASS_PAYLOAD, CHASSIS_FRU_ID, (gpointer *)&p_payload);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d data by alias failed!\r\n", CHASSIS_FRU_ID);
        return;
    }

    
    result = dfl_get_binded_object(CLASS_PAYLOAD, CHASSIS_FRU_ID, &object_handle);
    if (result == DFL_OK) {
        
        temp_data = p_payload->m_power_off_timeout;
        property_data = g_variant_new_uint32(temp_data);
        result = dfl_set_property_value(object_handle, PROPERTY_PAYLOAD_PWROFF_TM, property_data, DF_SAVE);
        g_variant_unref(property_data);
        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "set ChassisPayload object PowerOffTimeout fail!(result=%d)\r\n", result);
        }
    } else {
        debug_log(DLOG_ERROR, "get ChassisPayload object fail!(result=%d)\r\n", result);
    }
}

LOCAL void pp_update_pwr_delay_mode(void)
{
    guint8 temp_data;
    OBJ_HANDLE object_handle;
    GVariant *property_data = NULL;
    P_PAYLOAD_S p_payload = NULL;
    gint32 result;
    result = dfl_get_data_by_alias(CLASS_PAYLOAD, CHASSIS_FRU_ID, (gpointer *)&p_payload);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d data by alias failed!\r\n", CHASSIS_FRU_ID);
        return;
    }
    result = dfl_get_binded_object(CLASS_PAYLOAD, CHASSIS_FRU_ID, &object_handle);
    if (result == DFL_OK) {
        
        temp_data = p_payload->m_pwr_delay_mode;
        property_data = g_variant_new_byte(temp_data);
        result = dfl_set_property_value(object_handle, PROPERTY_PAYLOAD_PWRDELAY_MODE, property_data, DF_SAVE);
        g_variant_unref(property_data);
        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "set ChassisPayload object PowerDelayMode fail!(result=%d)\r\n", result);
        }
    } else {
        debug_log(DLOG_ERROR, "get ChassisPayload object fail!(result=%d)\r\n", result);
    }
}


LOCAL void pp_update_pwr_delay_count(void)
{
    gint32 result;
    guint32 temp_data;
    OBJ_HANDLE object_handle;
    GVariant *property_data = NULL;
    P_PAYLOAD_S p_payload = NULL;

    result = dfl_get_data_by_alias(CLASS_PAYLOAD, CHASSIS_FRU_ID, (gpointer *)&p_payload);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d data by alias failed!\r\n", CHASSIS_FRU_ID);
        return;
    }
    result = dfl_get_binded_object(CLASS_PAYLOAD, CHASSIS_FRU_ID, &object_handle);
    if (result == DFL_OK) {
        
        temp_data = p_payload->m_pwr_delay_count;
        property_data = g_variant_new_uint32(temp_data);
        result = dfl_set_property_value(object_handle, PROPERTY_PAYLOAD_PWRDELAY_COUNT, property_data, DF_SAVE);
        g_variant_unref(property_data);
        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "set ChassisPayload object PowerDelayCount fail!(result=%d)\r\n", result);
        }
    } else {
        debug_log(DLOG_ERROR, "get ChassisPayload object fail!(result=%d)\r\n", result);
    }
}


LOCAL void pp_update_restore_policy(void)
{
    gint32 result;
    OBJ_HANDLE object_handle;
    GVariant *property_data = NULL;
    P_PAYLOAD_S p_payload = NULL;
    guint8 temp_data;
    result = dfl_get_data_by_alias(CLASS_PAYLOAD, CHASSIS_FRU_ID, (gpointer *)&p_payload);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d data by alias failed!\r\n", CHASSIS_FRU_ID);
        return;
    }
    
    result = dfl_get_binded_object(CLASS_PAYLOAD, CHASSIS_FRU_ID, &object_handle);
    if (result == DFL_OK) {
        
        temp_data = p_payload->m_pwr_restore_policy;
        property_data = g_variant_new_byte(temp_data);
        result = dfl_set_property_value(object_handle, PROPERTY_POWER_REST_POLICY, property_data, DF_SAVE);
        g_variant_unref(property_data);
        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "set ChassisPayload object PowerRestorePolicy fail!(result=%d)\r\n", result);
        }
    } else {
        debug_log(DLOG_ERROR, "get ChassisPayload object fail!(result=%d)\r\n", result);
    }
}


void pp_update_reset_flag(guint8 reset_flag)
{
    gint32 result;
    guint8 temp_data;
    OBJ_HANDLE object_handle;
    GVariant *property_data = NULL;

    debug_log(DLOG_DEBUG, "pp_update_reset_flag(reset_flag=%02xh)\r\n", reset_flag);
    
    result = dfl_get_binded_object(CLASS_PAYLOAD, CHASSIS_FRU_ID, &object_handle);
    if (result == DFL_OK) {
        
        temp_data = reset_flag;
        property_data = g_variant_new_byte(temp_data);
        result = dfl_set_property_value(object_handle, PROPERTY_PAYLOAD_RESET_FLAG, property_data, DF_NONE);
        g_variant_unref(property_data);

        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "set ChassisPayload object ResetFlag fail!(result=%d)\r\n", result);
        }
    } else {
        debug_log(DLOG_ERROR, "get ChassisPayload object fail!(result=%d)\r\n", result);
    }
}


gint32 pp_check_src_from_reset(void)
{
    gint32 result;
    guint8 temp_data;
    OBJ_HANDLE object_handle = 0;
    GVariant *property_data = NULL;

    result = dfl_get_binded_object(CLASS_PAYLOAD, CHASSIS_FRU_ID, &object_handle);
    if (result == DFL_OK) {
        result = dfl_get_property_value(object_handle, PROPERTY_PAYLOAD_SYS_RST_SRC, &property_data);
        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "get ChassisPayload object SysResetSrc fail!(result=%d)\r\n", result);
            return RET_ERR;
        }
        temp_data = g_variant_get_byte(property_data);
        g_variant_unref(property_data);

        return (temp_data == TRUE) ? RET_OK : RET_ERR;
    } else {
        debug_log(DLOG_ERROR, "get ChassisPayload object fail!(result=%d)\r\n", result);
        return RET_ERR;
    }
}


void pp_update_ctrl_fault(void)
{
    gint32 result;
    guint8 temp_data;
    OBJ_HANDLE object_handle;
    GVariant *property_data = NULL;

    // 更新ChassisPayload对象的PowerCtrlFault状态
    result = dfl_get_binded_object(CLASS_PAYLOAD, CHASSIS_FRU_ID, &object_handle);
    if (result == DFL_OK) {
        // 更新当前PowerCtrlFault状态
        temp_data = pp_is_power_ctrl_failed();
        property_data = g_variant_new_byte(temp_data);
        result = dfl_set_property_value(object_handle, PROPERTY_POWER_CTRL_FAULT, property_data, DF_NONE);
        g_variant_unref(property_data);
        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "set ChassisPayload object PowerCtrlFault fail!(result=%d)\r\n", result);
        }
    } else {
        debug_log(DLOG_ERROR, "get ChassisPayload object fail!(result=%d)\r\n", result);
    }
}


LOCAL void pp_update_restart_state(guchar fruid)
{
    gint32 result;
    guint8 temp_data;
    OBJ_HANDLE object_handle;
    GVariant *property_data = NULL;
    P_PAYLOAD_S p_payload = NULL;

    result = dfl_get_data_by_alias(CLASS_PAYLOAD, fruid, (gpointer *)&p_payload);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d data by alias failed!\r\n", fruid);
        return;
    }
    // 更新ChassisPayload对象的RestartState状态
    result = dfl_get_binded_object(CLASS_PAYLOAD, fruid, &object_handle);
    if (result == DFL_OK) {
        // 更新当前RestartState状态
        temp_data = p_payload->m_restart_cause.restart_state;
        property_data = g_variant_new_byte(temp_data);
        result = dfl_set_property_value(object_handle, PROPERTY_PAYLOAD_RESTART_STATE, property_data, DF_NONE);
        g_variant_unref(property_data);
        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "set ChassisPayload object RestartState fail!(result=%d)\r\n", result);
        }
    } else {
        debug_log(DLOG_ERROR, "get ChassisPayload object fail!(result=%d)\r\n", result);
    }
}


LOCAL void pp_update_restart_cause(void)
{
    gint32 result;
    guint8 temp_data;
    GVariant *property_data = NULL;
    P_PAYLOAD_S p_payload = NULL;
    OBJ_HANDLE object_handle;
    result = dfl_get_data_by_alias(CLASS_PAYLOAD, CHASSIS_FRU_ID, (gpointer *)&p_payload);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d data by alias failed!\r\n", CHASSIS_FRU_ID);
        return;
    }

    // 更新ChassisPayload对象的RestartCause状态
    result = dfl_get_binded_object(CLASS_PAYLOAD, CHASSIS_FRU_ID, &object_handle);
    if (result == DFL_OK) {
        // 更新当前RestartState状态
        temp_data = p_payload->m_restart_cause.restart_cause;
        property_data = g_variant_new_byte(temp_data);
        result = dfl_set_property_value(object_handle, PROPERTY_PAYLOAD_RESTART_CAUSE, property_data, DF_NONE);
        g_variant_unref(property_data);
        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "set ChassisPayload object RestartCause fail!(result=%d)\r\n", result);
        }
    } else {
        debug_log(DLOG_ERROR, "get ChassisPayload object fail!(result=%d)\r\n", result);
    }
}


LOCAL void pp_update_restart_chan(void)
{
    gint32 result;
    guint8 temp_data;
    OBJ_HANDLE object_handle;
    GVariant *property_data = NULL;
    P_PAYLOAD_S p_payload = NULL;

    result = dfl_get_data_by_alias(CLASS_PAYLOAD, CHASSIS_FRU_ID, (gpointer *)&p_payload);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d data by alias failed!\r\n", CHASSIS_FRU_ID);
        return;
    }
    // 更新ChassisPayload对象的RestartChannel状态
    result = dfl_get_binded_object(CLASS_PAYLOAD, CHASSIS_FRU_ID, &object_handle);
    if (result == DFL_OK) {
        // 更新当前RestartChannel状态
        temp_data = p_payload->m_restart_cause.restart_chan;
        property_data = g_variant_new_byte(temp_data);
        result = dfl_set_property_value(object_handle, PROPERTY_PAYLOAD_RESTART_CHAN, property_data, DF_NONE);
        g_variant_unref(property_data);
        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "set ChassisPayload object RestartState fail!(result=%d)\r\n", result);
        }
    } else {
        debug_log(DLOG_ERROR, "get ChassisPayload object fail!(result=%d)\r\n", result);
    }
}


void pp_set_pwr_off_timeout(guint32 timeout)
{
    gint32 result;
    P_PAYLOAD_S p_payload = NULL;

    result = dfl_get_data_by_alias(CLASS_PAYLOAD, CHASSIS_FRU_ID, (gpointer *)&p_payload);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d data by alias failed!\r\n", CHASSIS_FRU_ID);
        return;
    }
    if (timeout == 0) {
        p_payload->m_power_off_timeout_en = 0x00;
        pp_update_pwr_off_timeout_en();

        return;
    } else {
        p_payload->m_power_off_timeout_en = 0x01;
        pp_update_pwr_off_timeout_en();
    }
    p_payload->m_power_off_timeout = timeout;
    pp_update_pwr_off_timeout();
}

void pp_set_pwr_on_delay_param(guint8 delay_mode, guint32 delay_count)
{
    gint32 result;
    P_PAYLOAD_S p_payload = NULL;

    result = dfl_get_data_by_alias(CLASS_PAYLOAD, CHASSIS_FRU_ID, (gpointer *)&p_payload);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d data by alias failed!\r\n", CHASSIS_FRU_ID);
        return;
    }

    p_payload->m_pwr_delay_mode = delay_mode;
    p_payload->m_pwr_delay_count = delay_count;
    pp_update_pwr_delay_mode();
    pp_update_pwr_delay_count();
}

void pp_get_pwr_on_delay_param(guint8 *delay_mode, guint32 *delay_count)
{
    gint32 result;
    P_PAYLOAD_S p_payload = NULL;

    result = dfl_get_data_by_alias(CLASS_PAYLOAD, CHASSIS_FRU_ID, (gpointer *)&p_payload);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d data by alias failed!\r\n", CHASSIS_FRU_ID);
        return;
    }

    *delay_mode = p_payload->m_pwr_delay_mode;
    *delay_count = p_payload->m_pwr_delay_count;
}

guint32 pp_get_pwr_off_timeout(guchar fru_device_id)
{
    gint32 result;
    P_PAYLOAD_S p_payload = NULL;
    result = dfl_get_data_by_alias(CLASS_PAYLOAD, fru_device_id, (gpointer *)&p_payload);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d data by alias failed!\r\n", CHASSIS_FRU_ID);
        return TIMEOUT_3M;
    }
    return p_payload->m_power_off_timeout;
}

void pp_set_pwr_off_timeout_en(guint32 timeout_en)
{
    gint32 result;
    P_PAYLOAD_S p_payload = NULL;

    result = dfl_get_data_by_alias(CLASS_PAYLOAD, CHASSIS_FRU_ID, (gpointer *)&p_payload);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d data by alias failed!\r\n", CHASSIS_FRU_ID);
        return;
    }

    p_payload->m_power_off_timeout_en = timeout_en;
    pp_update_pwr_off_timeout_en();
}

guint32 pp_get_pwr_off_timeout_en(guchar fru_device_id)
{
    gint32 result;
    P_PAYLOAD_S p_payload = NULL;

    result = dfl_get_data_by_alias(CLASS_PAYLOAD, fru_device_id, (gpointer *)&p_payload);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d data by alias failed!\r\n", CHASSIS_FRU_ID);
        return TIMEOUT_ENABLE;
    }
    return p_payload->m_power_off_timeout_en;
}

guchar pp_get_restore_policy(void)
{
    gint32 result;
    P_PAYLOAD_S p_payload = NULL;

    result = dfl_get_data_by_alias(CLASS_PAYLOAD, CHASSIS_FRU_ID, (gpointer *)&p_payload);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d data by alias failed!\r\n", CHASSIS_FRU_ID);
        return PAYLOAD_ALWAYS_POWER_UP;
    }
    return (guchar)p_payload->m_pwr_restore_policy;
}


void pp_set_restore_policy(guchar policy)
{
    gint32 result;
    P_PAYLOAD_S p_payload = NULL;

    if ((policy != PAYLOAD_ALWAYS_POWER_UP) && (policy != PAYLOAD_POWER_RESTORED) &&
        (policy != PAYLOAD_ALWAYS_POWER_OFF)) {
        return;
    }
    result = dfl_get_data_by_alias(CLASS_PAYLOAD, CHASSIS_FRU_ID, (gpointer *)&p_payload);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d data by alias failed!\r\n", CHASSIS_FRU_ID);
        return;
    }
    p_payload->m_pwr_restore_policy = policy;
    per_save((guint8 *)&p_payload->m_pwr_restore_policy);
    pp_update_restore_policy();
}


guchar pp_is_pwr_off_locked(guint8 fru_id)
{
    FRU_PAYLOAD_S *fru_payload = pp_relate_fru_payload_info();
    return fru_payload->m_pwr_off_lock[CHASSIS_FRU_ID];
}


void pp_set_pwr_off_lock(guint8 fru_id, guchar lock)
{
    if ((lock != TRUE) && (lock != FALSE)) {
        debug_log(DLOG_ERROR, "%s: unsupport lock val(%u)", __FUNCTION__, lock);
        return;
    }
    FRU_PAYLOAD_S *fru_payload = pp_relate_fru_payload_info();
    fru_payload->m_pwr_off_lock[fru_id] = lock;
    per_save((guint8 *)fru_payload->m_pwr_off_lock);
    debug_log(DLOG_DEBUG, "fru:%u pp_set_pwr_off_lock:lock=%u", fru_id, lock);
}


void pp_set_restart_action(guchar channel, guchar value)
{
    P_PAYLOAD_S p_payload = NULL;
    gint32 ret;
    ret = dfl_get_data_by_alias(CLASS_PAYLOAD, CHASSIS_FRU_ID, (gpointer *)&p_payload);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d data by alias failed!\r\n", CHASSIS_FRU_ID);
        return;
    }

    debug_log(DLOG_DEBUG, "%s New:%u-%u,Old:%u-%u\n", __FUNCTION__, value, channel,
        p_payload->m_restart_cause.restart_action, p_payload->m_restart_cause.restart_action_chan);
    p_payload->m_restart_cause.restart_action = value;
    p_payload->m_restart_cause.restart_action_chan = channel;
}


void pp_clear_restart_action(void)
{
    P_PAYLOAD_S p_payload = NULL;
    gint32 ret;

    ret = dfl_get_data_by_alias(CLASS_PAYLOAD, CHASSIS_FRU_ID, (gpointer *)&p_payload);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d data by alias failed!\r\n", CHASSIS_FRU_ID);
        return;
    }
    p_payload->m_restart_cause.restart_action = RESTART_CAUSE_UNKNOWN;
    p_payload->m_restart_cause.restart_action_chan = 0;
}


void pp_get_restart_cause(guchar *pchannel, guchar *pvalue)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GVariant *property_value = NULL;

    if (!pchannel || !pvalue) {
        return;
    }
    property_value = g_variant_new_byte(CHASSIS_FRU_ID);
    ret = dfl_get_specific_object(CLASS_PAYLOAD, PROPERTY_PAYLOAD_FRUID, property_value, &obj_handle);
    g_variant_unref(property_value);
    property_value = NULL;
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "get the Payload object(id=%d) failed, result=%d \n", CHASSIS_FRU_ID, ret);
        return;
    }
    ret = dfl_get_property_value(obj_handle, PROPERTY_PAYLOAD_RESTART_CHAN, &property_value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "get the restart chan value of Payload object(id=%d) failed, result=%d \n",
            CHASSIS_FRU_ID, ret);
        return;
    }
    *pchannel = g_variant_get_byte(property_value);
    g_variant_unref(property_value);
    property_value = NULL;
    ret = dfl_get_property_value(obj_handle, PROPERTY_PAYLOAD_RESTART_CAUSE, &property_value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "get the restart cause value of Payload object(id=%d) failed, result=%d \n",
            CHASSIS_FRU_ID, ret);
        return;
    }
    *pvalue = g_variant_get_byte(property_value);
    g_variant_unref(property_value);
}


gint32 payload_get_xcu_restart_cause(guchar *pchannel, guchar *pvalue, guchar fru_id)
{
    gint32 ret;
    OBJ_HANDLE object_handle;
    GVariant *get_value = NULL;
    guchar restart_cause;

    if (!pchannel || !pvalue) {
        return RET_ERR;
    }
    if (fru_id == BASE_FRU_ID || fru_id == FABRIC_FRU_ID) {
        ret = dfl_get_binded_object(CLASS_PAYLOAD, fru_id, &object_handle);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "get fru%d object fail!(result=%d)\r\n", fru_id, ret);
            return GET_BIND_OBJECT_FAIL;
        }
        ret = dfl_get_property_value(object_handle, PROPERTY_PAYLOAD_RESTART_CAUSE, &get_value);
        if (ret != DFL_OK) {
            return GET_BIND_OBJECT_FAIL;
        }
        restart_cause = g_variant_get_byte(get_value);
        g_variant_unref(get_value);
        switch (restart_cause) {
            case RESET_BUTTON:
                *pvalue = PUSH_BUTTON_RESET;
                break;
            case RESET_WATCHDOG:
                *pvalue = WT_EXPIR_RESET;
                break;
            case RESET_BMC_CMD:
                *pvalue = SOFT_REST;
                break;
            case RESET_POWER_UP:
                *pvalue = AUTO_POWER_UP_RESET;
                break;
            default:
                *pvalue = UNKOWN_RESET;
        }
        *pchannel = 0;
    } else {
        return FRU_ID_INVALID;
    }

    return RET_OK;
}

void pp_set_restart_state(guchar fruid, guchar val)
{
    P_PAYLOAD_S p_payload = NULL;
    gint32 ret;

    ret = dfl_get_data_by_alias(CLASS_PAYLOAD, fruid, (gpointer *)&p_payload);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d data by alias failed!\r\n", fruid);
        return;
    }

    p_payload->m_restart_cause.restart_state = val;
    pp_update_restart_state(fruid);
}

void pp_set_host_down(void)
{
    P_PAYLOAD_S p_payload = NULL;
    gint32 ret;

    ret = dfl_get_data_by_alias(CLASS_PAYLOAD, CHASSIS_FRU_ID, (gpointer *)&p_payload);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d data by alias failed!\r\n", CHASSIS_FRU_ID);
        return;
    }

    p_payload->m_is_host_up = FALSE;
}


void pp_set_host_up(void)
{
    P_PAYLOAD_S p_payload = NULL;
    gint32 ret;

    ret = dfl_get_data_by_alias(CLASS_PAYLOAD, CHASSIS_FRU_ID, (gpointer *)&p_payload);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d data by alias failed!\r\n", CHASSIS_FRU_ID);
        return;
    }

    p_payload->m_is_host_up = TRUE;
}


guchar pp_is_host_up(void)
{
    P_PAYLOAD_S p_payload = NULL;
    gint32 ret;

    ret = dfl_get_data_by_alias(CLASS_PAYLOAD, CHASSIS_FRU_ID, (gpointer *)&p_payload);
    
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d data by alias failed!\r\n", CHASSIS_FRU_ID);
        return 0;
    }

    return p_payload->m_is_host_up;
}


guchar pp_is_insert_check_en(guchar fru_device_id)
{
    P_PAYLOAD_S p_payload = NULL;
    gint32 ret;

    ret = dfl_get_data_by_alias(CLASS_PAYLOAD, fru_device_id, (gpointer *)&p_payload);
    
    if (ret != DFL_OK) {
        debug_log(DLOG_MASS, "get fru%d data by alias failed!\r\n", fru_device_id);
        return 0;
    }

    return p_payload->m_check_insert_en;
}

guchar pp_is_pwr_restore_in_progress(guchar fru_device_id)
{
    P_PAYLOAD_S p_payload = NULL;
    gint32 ret;

    ret = dfl_get_data_by_alias(CLASS_PAYLOAD, fru_device_id, (gpointer *)&p_payload);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d data by alias failed!\r\n", fru_device_id);
        return (guchar)RET_ERR;
    }

    return p_payload->m_is_pwr_restore_in_progress;
}


void pp_set_pwr_restore_in_progress(guchar fru_device_id, guchar in_progress)
{
    P_PAYLOAD_S p_payload = NULL;
    gint32 ret;

    ret = dfl_get_data_by_alias(CLASS_PAYLOAD, fru_device_id, (gpointer *)&p_payload);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d data by alias failed!\r\n", fru_device_id);
        return;
    }

    p_payload->m_is_pwr_restore_in_progress = in_progress;
    return;
}


guchar pp_is_cycle_in_progress(guchar fruid)
{
    return g_fru_payload.m_pwr_cycle_cb[fruid].cycle_in_progress;
}


void pp_set_cycle_interval(guchar fruid, guint32 val)
{
    FRU_PAYLOAD_S *fru_payload = pp_relate_fru_payload_info();
    fru_payload->m_pwr_cycle_cb[fruid].interval = val;
}


void pp_set_cycle_progress_state(guchar fruid, guint32 val)
{
    FRU_PAYLOAD_S *fru_payload = pp_relate_fru_payload_info();
    fru_payload->m_pwr_cycle_cb[fruid].cycle_in_progress = val;
}


LOCAL void set_fan_init_default_level(void)
{
    gint32 ret;
    OBJ_HANDLE cooling_handle = 0;
    guchar level_val = 0;
    GSList *input_list = NULL;

    ret = dal_get_object_handle_nth(COOLINGCLASS, 0, &cooling_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_object_handle_nth failed, ret = %d", __FUNCTION__, ret);
        return;
    }
    
    ret = dal_get_property_value_byte(cooling_handle, COOLING_FAN_INIT_DEFAULT_LEVEL, &level_val);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_byte failed, ret = %d", __FUNCTION__, ret);
        return;
    }

    if (level_val < FAN_INIT_DEFAULT_LEVEL_MIN_VAL || level_val > FAN_INIT_DEFAULT_LEVEL_MAX_VAL) {
        debug_log(DLOG_DEBUG, "%s: the init fan level (%d) to be set is not in the range %d to %d", __FUNCTION__,
            level_val, FAN_INIT_DEFAULT_LEVEL_MIN_VAL, FAN_INIT_DEFAULT_LEVEL_MAX_VAL);
        return;
    }

    if (level_val == FAN_INIT_DEFAULT_LEVEL_MAX_VAL) {
        return;
    }

    debug_log(DLOG_DEBUG, "%s: the init fan level to be set is %d", __FUNCTION__, level_val);

    input_list = g_slist_append(input_list, g_variant_new_byte((guchar)level_val));
    ret = dfl_call_class_method(cooling_handle, METHOD_SET_INIT_COOLING_LEVEL, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: dfl_call_class_method failed, ret = %d", __FUNCTION__, ret);
    }
}


LOCAL void dft_set_fanlevel(void)
{
    guint8 loop_flg = 0;
    gint32 loop_confirmed_flg = 0;
    guint8 fan_level = 0xFF;
    gint32 ret;
    OBJ_HANDLE cooling_handle = 0;
    OBJ_HANDLE com_handle = 0;
    guchar cnt = 0;
    GSList *local_output_list = NULL;
    GSList *input_list = NULL;

    ret = dfl_get_object_handle(COOLINGCLASS, &cooling_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: dfl_get_object_handle failed, ret = %d", __FUNCTION__, ret);
        return;
    }

    ret = dal_get_property_value_byte(cooling_handle, COOLING_DFT_FAN_DEFAULT_LEVEL, &fan_level);
    if (ret != DFL_OK || fan_level == DFT_FAN_DEFAULT_LEVEL_VAL_INVALID) {
        return;
    }

    if (dfl_get_object_handle(BMC_COM_OBJ_NAME, &com_handle) != DFL_OK) {
        return;
    }

    for (; cnt < DFT_FAN_WAIT_FOR_LOOPBACK; cnt++) {
        ret = dfl_call_class_method(com_handle, METHOD_GET_LOOPBACK_CONFIRMED_FLG, NULL, NULL, &local_output_list);
        if (ret == RET_OK) {
            loop_confirmed_flg = g_variant_get_int32((GVariant *)g_slist_nth_data(local_output_list, 0));
        }

        g_slist_free_full(local_output_list, (GDestroyNotify)g_variant_unref);
        local_output_list = NULL;

        if (loop_confirmed_flg != 0) {
            break;
        }

        vos_task_delay(1000); 
    }

    debug_log(DLOG_DEBUG, "%s: try cnt is %d loop_confirmed_flg is %d", __FUNCTION__, cnt, loop_confirmed_flg);

    if (loop_confirmed_flg == 0) {
        return;
    }

    ret = dal_get_property_value_byte(com_handle, PROPERTY_SOL_COM_LOOPBACK, &loop_flg);
    if (ret != DFL_OK || loop_flg != 1) {
        return;
    }

    debug_log(DLOG_DEBUG, "%s, %d: loop_flg is ok", __FUNCTION__, __LINE__);

    input_list = g_slist_append(input_list, g_variant_new_byte((guchar)fan_level));
    ret = dfl_call_class_method(cooling_handle, METHOD_SET_INIT_COOLING_LEVEL, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: dfl_call_class_method, ret: %d.", __FUNCTION__, ret);
    }
}


void pp_save_cycle_param(void)
{
    FRU_PAYLOAD_S *fru_payload = pp_relate_fru_payload_info();
    per_save((guint8 *)fru_payload->m_pwr_cycle_cb);
}

void pp_do_pwr_restore_process(void)
{
    guchar power_on_restore = 0;
    P_PAYLOAD_S p_payload = NULL;

    gint32 result = dfl_get_data_by_alias(CLASS_PAYLOAD, CHASSIS_FRU_ID, (gpointer *)&p_payload);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d data by alias failed!\r\n", CHASSIS_FRU_ID);
        return;
    }

    
    
    switch (p_payload->m_pwr_restore_policy) {
        case PAYLOAD_ALWAYS_POWER_UP:
        case PAYLOAD_ALWAYS_POWER_UP_RO:
            power_on_restore = TRUE;
            pp_set_restart_action(SRC_CHAN_NUM_RPC, RESTART_CAUSE_ALWAYSRESTORE);  
            pp_set_restart_cause();
            g_printf("Power restore policy.........................................always-on.\r\n");
            debug_log(DLOG_ERROR, "Power restore policy.........................................always-on.\r\n");
            break;

        case PAYLOAD_POWER_RESTORED:
            if (p_payload->m_pwr_state_before_ac_lost == PAYLOAD_POWER_STATE_OFF) {
                debug_log(DLOG_ERROR, "Power restore policy:previous, pwr_state_before_ac_lost = 0.");
                power_on_restore = FALSE;
            } else {
                power_on_restore = TRUE;
                pp_set_restart_action(SRC_CHAN_NUM_RPC, RESTART_CAUSE_RESTOREPREVIOUS);
                pp_set_restart_cause();
            }

            g_printf("Power restore policy.........................................previous.\r\n");
            debug_log(DLOG_ERROR, "Power restore policy.........................................previous.\r\n");
            break;

        case PAYLOAD_ALWAYS_POWER_OFF:

            p_payload->m_pwr_state_before_ac_lost = PAYLOAD_ALWAYS_POWER_OFF;
            per_save((guint8 *)&p_payload->m_pwr_state_before_ac_lost);

            power_on_restore = FALSE;
            g_printf("Power restore policy.........................................always-off.\r\n");
            debug_log(DLOG_ERROR, "Power restore policy.........................................always-off.\r\n");
            break;

        default:
            break;
    }

    if (power_on_restore == FALSE) {
        
        pp_set_pwr_off_lock(CHASSIS_FRU_ID, TRUE);
    } else {
        
        set_fan_init_default_level();
        dft_set_fanlevel();

        
        pp_set_pwr_restore_in_progress(CHASSIS_FRU_ID, TRUE);

        if (hs_is_auto_management() == TRUE) {
            hs_do_pwr_on_delay(); 
            
            pp_set_pwr_off_lock(CHASSIS_FRU_ID, FALSE);
            (void)pp_fru_pwr_ctrl(CHASSIS_FRU_ID, POWER_ON);
        } else {
            
            hs_do_default_mode_delay(FALSE);
            pp_set_pwr_off_lock(CHASSIS_FRU_ID, FALSE);
            
            (void)hse_fru_activate_policy(CHASSIS_FRU_ID, 1, 0);
        }
    }
}


gint32 pp_dump_cmd(GSList *input_list)
{
    guchar i;
    guint32 fru_num;
    gint32 result;
    P_PAYLOAD_S p_payload = NULL;

    
    fru_num = hs_get_fru_num();

    FRU_PAYLOAD_S *fru_payload = pp_relate_fru_payload_info();
    for (i = 0; i < fru_num; i++) {
        debug_printf("----------------------------------------\r\n");
        debug_printf("FRU%02d payload power info:\r\n", i);
        debug_printf("m_pwr_off_lock = %d\r\n", fru_payload->m_pwr_off_lock[i]);
        debug_printf("m_pwr_ctrl_cb.m_pwr_state = %d\r\n", fru_payload->m_pwr_ctrl_cb[i].m_pwr_state);
        debug_printf("m_pwr_ctrl_cb.m_action = %d\r\n", fru_payload->m_pwr_ctrl_cb[i].m_action);
        debug_printf("m_pwr_ctrl_cb.m_ctrl_state = %d\r\n", fru_payload->m_pwr_ctrl_cb[i].m_ctrl_state);
        debug_printf("m_pwr_cycle_cb.interval = %d\r\n", fru_payload->m_pwr_cycle_cb[i].interval);
        debug_printf("m_pwr_cycle_cb.cycle_in_progress = %d\r\n", fru_payload->m_pwr_cycle_cb[i].cycle_in_progress);
        debug_printf("----------------------------------------\r\n");
        result = dfl_get_data_by_alias(CLASS_PAYLOAD, i, (gpointer *)&p_payload);
        if (result != DFL_OK) {
            continue;
        }
        debug_printf("----------------------------------------\r\n");
        debug_printf("FRU%02d info:\r\n", i);
        debug_printf("m_power_off_timeout = %d\r\n", p_payload->m_power_off_timeout);
        debug_printf("m_pwr_delay_mode = %d\r\n", p_payload->m_pwr_delay_mode);
        debug_printf("m_pwr_delay_count = %d\r\n", p_payload->m_pwr_delay_count);
        debug_printf("m_power_off_timeout_en = %d\r\n", p_payload->m_power_off_timeout_en);
        debug_printf("m_check_insert_en = %d\r\n", p_payload->m_check_insert_en);
        debug_printf("m_power_cycle_interval = %d\r\n", p_payload->m_power_cycle_interval);
        debug_printf("m_is_host_up = %d\r\n", p_payload->m_is_host_up);
        debug_printf("m_is_pwr_restore_in_progress = %d\r\n", p_payload->m_is_pwr_restore_in_progress);
        debug_printf("m_pwr_off_lock = %d\r\n", p_payload->m_pwr_off_lock);
        debug_printf("m_pwr_restore_policy = %d\r\n", p_payload->m_pwr_restore_policy);
        debug_printf("m_pwr_state_before_ac_lost = %d\r\n", p_payload->m_pwr_state_before_ac_lost);
        debug_printf("----------------------------------------\r\n");
    }
    return RET_OK;
}


guint8 pp_action_according_to_powerlevel(guint8 fru_id, guint8 pow_level, guint8 set_power_type)
{
    guchar comp_code = 0;
    guint8 server_type = 0;
    OBJ_HANDLE obj_product_handle = 0;
    OBJ_HANDLE obj_bmc_handle = 0;
    OBJ_HANDLE obj_state_less_handle = 0;
    guint8 blade_manage_mode = 0;
    OBJ_HANDLE obj_fru0_handle = 0;
    guint8 autopoweron = TRUE;

    
    if (g_get_power_info.max_power_level < pow_level) {
        debug_log(DLOG_ERROR, "pow_level:%d(Should <= %u) invalid!\r\n", pow_level, g_get_power_info.max_power_level);
        return COMP_CODE_OUTOF_RANGE;
    }

    
    if (FRU_HS_STATE_M0 == hs_get_cur_state(fru_id)) {
        
        debug_log(DLOG_ERROR, "cur_state is M0,not support set_power_level.\r\n");
        return COMP_CODE_STATUS_INVALID;
    }

    
    if (pow_level == POWER_LEVEL_POWER_OFF) {
        // 取消已经开启的安全重启或POWER循环
        pp_set_cycle_progress_state(CHASSIS_FRU_ID, FALSE);
        pp_set_cycle_interval(CHASSIS_FRU_ID, 0);
        pp_save_cycle_param();
        // 控制系统强制下电
        comp_code = pp_fru_pwr_ctrl(fru_id, POWER_OFF_FORCE);
        if (comp_code == COMP_CODE_SUCCESS) {
            hse_pwr_level0(fru_id);
        }
    } else if (pow_level == POWER_LEVEL_DONT_CHANGE) {
        ;
    } else {
        (void)dal_get_object_handle_nth(CLASS_NAME_PME_PRODUCT, 0, &obj_product_handle);
        (void)dal_get_property_value_byte(obj_product_handle, PROPERTY_MGMT_SOFTWARE_TYPE, &server_type);

        
        if (MGMT_SOFTWARE_TYPE_BLADE == server_type) {
            
            if (FRU_HS_STATE_M3 == hs_get_cur_state(fru_id)) {
                (void)dal_get_object_handle_nth(BMC_CLASEE_NAME, 0, &obj_bmc_handle);
                (void)dal_get_property_value_byte(obj_bmc_handle, PROPERTY_BLADE_MANAGE_MODE, &blade_manage_mode);
                (void)dal_get_specific_object_byte(CLASS_PAYLOAD, PROPERTY_PAYLOAD_FRUID, CHASSIS_FRU_ID,
                    &obj_fru0_handle);

                (void)dal_get_object_handle_nth(CLASS_STATELESS, 0, &obj_state_less_handle);
                (void)dal_get_property_value_byte(obj_state_less_handle, PROPERTY_STATELESS_AUTO_POWER_ON,
                    &autopoweron);
                if ((blade_manage_mode == ESIGHT_BLADE_MANAGE_MODE) && (set_power_type == IPMI_SET_POWER) &&
                    (autopoweron != TRUE)) {
                    
                    (void)dal_set_property_value_byte(obj_fru0_handle, PROPERTY_BLADE_POWER_ON_PERMIT,
                        PERMIT_BLADE_POWER_ON, DF_NONE);
                } else {
                    
                    
                    (void)dal_set_property_value_byte(obj_fru0_handle, PROPERTY_BLADE_POWER_ON_PERMIT,
                        FORBID_BLADE_POWER_ON, DF_NONE);
                    
                    comp_code = pp_fru_pwr_ctrl(fru_id, POWER_ON);
                    g_get_power_info.cur_power_level = pow_level;
                }
            }
        } else {
            
            
            comp_code = pp_fru_pwr_ctrl(fru_id, POWER_ON);
            g_get_power_info.cur_power_level = pow_level;
        }
    }
    return comp_code;
}


LOCAL gint32 pp_fru_payload_obj_init(OBJ_HANDLE object_handle, gpointer user_data)
{
    gint32 result;
    guint8 fru_id;
    guint8 temp_data;
    GVariant *property_data = NULL;

    // 先取对象的fruid
    result = dfl_get_property_value(object_handle, PROPERTY_FRU_PAYLOAD_FRUID, &property_data);
    if (result != DFL_OK) {
        return RET_OK;
    }
    fru_id = g_variant_get_byte(property_data);
    g_variant_unref(property_data);
    // 因为在框架里会调用rpc，所以要将变量的副本传递给框架
    // 由框架来释放变量副本的占用内存
    // 更新PowerState
    FRU_PAYLOAD_S *fru_payload = pp_relate_fru_payload_info();
    temp_data = fru_payload->m_pwr_ctrl_cb[fru_id].m_pwr_state;
    property_data = g_variant_new_byte(temp_data);
    result = dfl_set_property_value(object_handle, PROPERTY_FRUPAYLOAD_POWERSTATE, property_data, DF_NONE);
    g_variant_unref(property_data);
    if (result != DFL_OK) {
        return RET_OK;
    }
    // 给FruPayload对象绑定一个别名，方便后续更新属性时的操作
    result = dfl_bind_object_alias(object_handle, (guint32)fru_id);
    if (result != DFL_OK) {
        return RET_OK;
    }
    debug_log(DLOG_DEBUG, "bind FruPayload object success\r\n");

    return RET_OK;
}

LOCAL gint32 pp_chassis_payload_obj_init(OBJ_HANDLE object_handle, gpointer user_data)
{
    gint32 result;
    guint8 temp_data;
    guchar sys_reset_flag = 0x00;
    GVariant *property_data = NULL;
    P_PAYLOAD_S p_payload = NULL;
    guchar fruid;

    // 因为多FRU很多特性都具备了ChassisPayload的特性，所以为了归一，把多FRU当做多Chassis处理
    result = dfl_get_property_value(object_handle, PROPERTY_PAYLOAD_FRUID, &property_data);
    if (result != DFL_OK) {
        return RET_OK;
    }
    fruid = g_variant_get_byte(property_data);
    g_variant_unref(property_data);
    property_data = NULL;

    result = dfl_get_data_by_alias(CLASS_PAYLOAD, fruid, (gpointer *)&p_payload);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d data by alias failed!\r\n", fruid);
        return RET_OK;
    }

    // 从框架中获取已经保存的下电超时时间
    result = dfl_get_property_value(object_handle, PROPERTY_PAYLOAD_PWROFF_TM, &property_data);
    if (result != DFL_OK) {
        return RET_OK;
    }
    p_payload->m_power_off_timeout = g_variant_get_uint32(property_data);
    g_variant_unref(property_data);
    property_data = NULL;
    // 从框架中获取已经保存的上电延迟模式
    result = dfl_get_property_value(object_handle, PROPERTY_PAYLOAD_PWRDELAY_MODE, &property_data);
    if (result != DFL_OK) {
        return RET_OK;
    }
    p_payload->m_pwr_delay_mode = g_variant_get_byte(property_data);
    g_variant_unref(property_data);
    property_data = NULL;

    // 从框架中获取已经保存的上电延迟计数
    result = dfl_get_property_value(object_handle, PROPERTY_PAYLOAD_PWRDELAY_COUNT, &property_data);
    if (result != DFL_OK) {
        return RET_OK;
    }
    p_payload->m_pwr_delay_count = g_variant_get_uint32(property_data);
    g_variant_unref(property_data);
    property_data = NULL;
    // 从框架中获取插稳检测配置项
    result = dfl_get_property_value(object_handle, PROPERTY_PAYLOAD_INSERT_EN, &property_data);
    if (result != DFL_OK) {
        return RET_OK;
    }
    p_payload->m_check_insert_en = g_variant_get_byte(property_data);
    g_variant_unref(property_data);
    property_data = NULL;
    // 从框架中获取已经保存的下电超时强制下电使能
    result = dfl_get_property_value(object_handle, PROPERTY_PAYLOAD_PWROFF_TM_EN, &property_data);
    if (result != DFL_OK) {
        return RET_OK;
    }
    p_payload->m_power_off_timeout_en = g_variant_get_uint32(property_data);
    g_variant_unref(property_data);
    property_data = NULL;

    
    result = dfl_get_property_value(object_handle, PROPERTY_POWER_REST_POLICY, &property_data);
    if (result != DFL_OK) {
        return RET_OK;
    }
    p_payload->m_pwr_restore_policy = g_variant_get_byte(property_data);
    g_variant_unref(property_data);

    // 因为在框架里会调用rpc，所以要将变量的副本传递给框架
    // 由框架来释放变量副本的占用内存
    // 更新ResetFlag,用于其他模块查询payload复位状态
    (void)hop_get_system_reset_flag(&sys_reset_flag);
    temp_data = sys_reset_flag;
    property_data = g_variant_new_byte(temp_data);
    result = dfl_set_property_value(object_handle, PROPERTY_PAYLOAD_RESET_FLAG, property_data, DF_NONE);
    g_variant_unref(property_data);
    if (result != DFL_OK) {
        return RET_OK;
    }
    // 更新PowerCtrlFault
    temp_data = pp_is_power_ctrl_failed();
    property_data = g_variant_new_byte(temp_data);
    result = dfl_set_property_value(object_handle, PROPERTY_POWER_CTRL_FAULT, property_data, DF_NONE);
    g_variant_unref(property_data);
    if (result != DFL_OK) {
        return RET_OK;
    }
    // 更新ChassisPowerState，直接对应于chassis fru的power state
    FRU_PAYLOAD_S *fru_payload = pp_relate_fru_payload_info();
    temp_data = fru_payload->m_pwr_ctrl_cb[fruid].m_pwr_state;
    property_data = g_variant_new_byte(temp_data);
    result = dfl_set_property_value(object_handle, PROPERTY_PAYLOAD_CHASSPWR_STATE, property_data, DF_NONE);
    g_variant_unref(property_data);
    if (result != DFL_OK) {
        return RET_OK;
    }
    // 更新RestartState
    temp_data = p_payload->m_restart_cause.restart_state;
    property_data = g_variant_new_byte(temp_data);
    result = dfl_set_property_value(object_handle, PROPERTY_PAYLOAD_RESTART_STATE, property_data, DF_NONE);
    g_variant_unref(property_data);
    if (result != DFL_OK) {
        return RET_OK;
    }
    // 更新RestartCause
    temp_data = p_payload->m_restart_cause.restart_cause;
    property_data = g_variant_new_byte(temp_data);
    result = dfl_set_property_value(object_handle, PROPERTY_PAYLOAD_RESTART_CAUSE, property_data, DF_NONE);
    g_variant_unref(property_data);
    if (result != DFL_OK) {
        return RET_OK;
    }
    // 更新RestartChannel
    temp_data = p_payload->m_restart_cause.restart_chan;
    property_data = g_variant_new_byte(temp_data);
    result = dfl_set_property_value(object_handle, PROPERTY_PAYLOAD_RESTART_CHAN, property_data, DF_NONE);
    g_variant_unref(property_data);
    if (result != DFL_OK) {
        return RET_OK;
    }
    return RET_OK;
}

LOCAL void pp_obj_init(void)
{
    // 遍历所有ChassisPayload对象，将当前的ChassisPayload状态(从持久化中恢复出来的)更新到对象属性中
    // 代码中涉及到ChassisPayload对象属性值变化的，也需要调用dfl_set_property_value方法进行更新
    // 理论上说目前的系统只可能存在一个ChassisPayload，但是我们的代码尽量按照有未来有可能出现
    // 的一个BMC管理多个ChassisPayload的情况来设计
    if (DFL_OK != dfl_foreach_object(CLASS_PAYLOAD, pp_chassis_payload_obj_init, NULL, NULL)) {
        debug_log(DLOG_ERROR, "foreach ChassisPayload objects fail\r\n");
        return;
    }

    // 遍历所有FruPayload对象，将当前的FruPayload状态(从持久化中恢复出来的)更新到对象属性中
    // 代码中涉及到FruPayload对象属性值变化的，也需要调用dfl_set_property_value方法进行更新
    if (DFL_OK != dfl_foreach_object(CLASS_FRUPAYLOAD, pp_fru_payload_obj_init, NULL, NULL)) {
        debug_log(DLOG_ERROR, "foreach FruPayload objects fail\r\n");
        return;
    }
}


void pp_ctrl_init(void)
{
    TASKID taskID;
    guint32 fru_num;
    guint32 i;

    fru_num = hs_get_fru_num();
    FRU_PAYLOAD_S *fru_payload = pp_relate_fru_payload_info();
    for (i = 0; i < fru_num; i++) {
        
        if (TRUE == hs_is_fru_managed(i)) {
            
            if (RET_OK != vos_queue_create(&fru_payload->m_power_ctrl_evt_qid[i])) {
                debug_log(DLOG_ERROR, "pp_ctrl_init: que create failed.\r\n");
            }

            
            (void)vos_task_create(&taskID, "PCTRL", (TASK_ENTRY)pp_ctrl_task, UINT32_TO_POINTER(i), 2);
        }
    }
}


LOCAL gint32 pp_init_get_power_info(void)
{
    OBJ_HANDLE object_handle;
    GVariant *property_value = NULL;
    gint32 ret;
    const guchar *power_draw = NULL;
    guint32 i;
    gsize pwr_level_cnt = 0;

    (void)memset_s(&g_get_power_info, sizeof(GET_POWER_INFO_S), 0, sizeof(GET_POWER_INFO_S));

    ret = dfl_get_object_handle(OBJ_FRU_STATICS, &object_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "dfl_get_object_handle fail!(result=%d)\r\n", ret);
        return RET_OK;
    }

    ret = dfl_get_property_value(object_handle, PROPERTY_FRU_POWER_STABLE_TIME, &property_value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "dfl_get_property_value failed, error = %d!", ret);
        return RET_OK;
    }
    g_get_power_info.delay_to_stable = g_variant_get_byte(property_value);
    g_variant_unref(property_value);
    property_value = NULL;
    ret = dfl_get_property_value(object_handle, PROPERTY_FRU_POWER_MUL, &property_value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "dfl_get_property_value failed, error = %d!", ret);
        return RET_OK;
    }

    g_get_power_info.power_multiplier = g_variant_get_byte(property_value);
    g_variant_unref(property_value);
    property_value = NULL;

    ret = dfl_get_property_value(object_handle, PROPERTY_FRU_POWER_DRAW, &property_value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "dfl_get_property_value failed, error = %d!", ret);
        return RET_OK;
    }

    power_draw = (const guchar *)g_variant_get_fixed_array(property_value, &pwr_level_cnt, sizeof(guchar));
    if (power_draw == NULL) {
        g_variant_unref(property_value);
        
        debug_log(DLOG_ERROR, DB_STR_GET_PROPERTY_NULL);
        return RET_OK;
    }
    g_get_power_info.max_power_level = LONGB0(pwr_level_cnt) + 1;
    g_get_power_info.cur_power_level = LONGB0(pwr_level_cnt);

    g_get_power_info.power_draw[0] = power_draw[0];

    for (i = 0; i < LONGB0(pwr_level_cnt); i++) {
        g_get_power_info.power_draw[i + 1] = power_draw[i];
    }

    g_variant_unref(property_value);

    return RET_OK;
}


gint32 get_vddq_check_forcepwroff_flag(guchar *flag)
{
    GSList *handle_list = NULL;
    GSList *list_item = NULL;
    OBJ_HANDLE object_handle;
    gint32 result;
    guchar force_pwr_off_flag = 0;
    guchar total_result = 0;

    if (flag == NULL) {
        debug_log(DLOG_ERROR, "flag is NULL\n");
        return RET_ERR;
    }

    result = dfl_get_object_list(CLASS_CHIP_VALUE_CHECKER, &handle_list);
    if (result != DFL_OK) {
        debug_log(DLOG_DEBUG, "Get object VDDQChecker handle list failed: %d.\n ", result);
        return RET_ERR;
    }

    for (list_item = handle_list; list_item; list_item = g_slist_next(list_item)) {
        object_handle = (OBJ_HANDLE)list_item->data;
        (void)dal_get_property_value_byte(object_handle, PROPERTY_VDDQ_FORCE_PWR_OFF_FLAG, &force_pwr_off_flag);
        total_result |= force_pwr_off_flag;
    }

    *flag = total_result;
    g_slist_free(handle_list);
    return RET_OK;
}


void pp_start(void)
{
    TASKID pp_task = 0;
    
    pp_dft_init();

    pp_cycle_init();
    pp_event_check_init();
    pp_high_temperature_check_init();

    (void)vos_task_create(&pp_task, "pp_task_power", pp_task_power, NULL, DEFAULT_PRIORITY);
    (void)refresh_pwrbutton_shield_state();	

#ifdef DFT_ENABLED
    gulong task_id = 0;
    OBJ_HANDLE obj_handle = 0;
    gint32 reset_cause = 0;
    guchar server_type = 0;
    guchar cpu_presence = 0;
    
    (void)dal_get_object_handle_nth(BMC_PRODUCT_NAME_APP, 0, &obj_handle);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_MGMT_SOFTWARE_TYPE, &server_type);

    if ((MGMT_SOFTWARE_TYPE_BLADE == server_type) || (MGMT_SOFTWARE_TYPE_EM_X86_BMC == server_type)) {
        (void)dal_get_specific_object_byte(CLASS_CPU, PROPERTY_CPU_PHYSIC_ID, 1, &obj_handle);
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_CPU_PRESENCE, &cpu_presence);
        (void)dal_get_object_handle_nth(BMC_CLASEE_NAME, 0, &obj_handle);
        (void)dal_get_property_value_int32(obj_handle, BMC_RESET_CAUSE_NAME, &reset_cause);
        if ((RESET_TYPE_POWER_UP == reset_cause) && (cpu_presence)) {
            (void)vos_task_create(&task_id, "TaskOSFastPowerCycleTest", (TASK_ENTRY)task_os_fast_power_cycle_test, NULL,
                DEFAULT_PRIORITY);
        }
    }
#endif
}


LOCAL gint32 pp_payload_obj_init_foreach(OBJ_HANDLE object_handle, gpointer user_data)
{
    gint32 result;
    guint8 fru_id;
    PAYLOAD_S *p_payload = NULL;
    guint32 pwr_cycle_interval = 0;
    guint32 product_version = 0;

    
    result = dal_get_property_value_byte(object_handle, PROPERTY_PAYLOAD_FRUID, &fru_id);
    if (result != RET_OK) {
        return RET_OK;
    }

    
    result = dfl_bind_object_alias(object_handle, (guint32)fru_id);
    if (result != DFL_OK) {
        return RET_OK;
    }
    debug_log(DLOG_DEBUG, "payload bind FRU object success\r\n");
    
    result = dal_get_property_value_uint32(object_handle, PROPERTY_PWR_CYCLE_INTERVAL, &pwr_cycle_interval);
    if (result != RET_OK) {
        debug_log(DLOG_ERROR, "%s : Failed to get power cycle interval, result = %d", __FUNCTION__, result);
        result = dal_get_product_version_num(&product_version);
        if (result != RET_OK) {
            debug_log(DLOG_ERROR, "%s : Failed to get product version num, result = %d", __FUNCTION__, result);
            pwr_cycle_interval = CYCLE_INTERVAL_6S;
        } else {
            pwr_cycle_interval = (product_version >= PRODUCT_VERSION_V5) ? CYCLE_INTERVAL_14S : CYCLE_INTERVAL_6S;
        }
    }

    
    p_payload = (PAYLOAD_S *)g_malloc(sizeof(PAYLOAD_S));
    if (p_payload == NULL) {
        debug_log(DLOG_ERROR, "pp_payload_obj_init_foreach malloc is NULL\n");
        return RET_OK;
    }
    (void)memset_s(p_payload, sizeof(PAYLOAD_S), 0x00, sizeof(PAYLOAD_S));

    p_payload->m_pwr_restore_policy = PAYLOAD_ALWAYS_POWER_UP;
    p_payload->m_power_off_timeout = TIMEOUT_3M;
    p_payload->m_check_insert_en = FALSE;
    p_payload->m_pwr_delay_mode = DELAY_MODE_SHORT;
    p_payload->m_pwr_delay_count = 0;
    p_payload->m_power_off_timeout_en = TIMEOUT_ENABLE;
    p_payload->m_power_cycle_interval = pwr_cycle_interval * 1000; 
    p_payload->m_host_check_timer = INVALID_TIMER_NUM;
    p_payload->m_power_cycle_task_id = 0;
    p_payload->m_is_host_up = TRUE;
    p_payload->m_is_pwr_restore_in_progress = FALSE;
    p_payload->m_restart_cause.restart_action = RESTART_CAUSE_ALWAYSRESTORE;
    p_payload->m_restart_cause.restart_action_chan = 0;
    p_payload->m_restart_cause.restart_state = RESTART_NONE;
    p_payload->m_pwr_timeout_count = 0;
	
    
    result = dfl_bind_object_data(object_handle, p_payload, g_free);
    if (result != DFL_OK) {
        g_free(p_payload);
        debug_log(DLOG_ERROR, "bind payload private property name fail, result is %d\n", result);
        return RET_OK;
    }

    return RET_OK;
}

guchar pp_payload_init(void)
{
    if (DFL_OK != dfl_foreach_object(CLASS_PAYLOAD, pp_payload_obj_init_foreach, NULL, NULL)) {
        debug_log(DLOG_ERROR, "foreach payload objects fail\r\n");
        return PAYLOAD_FAIL;
    }

    return PAYLOAD_SUCCESS;
}


guchar pp_init(void)
{
    guchar fru_id;
    guint32 fru_num;
    guint32 malloc_size;
    P_PAYLOAD_S p_payload = NULL;

    
    fru_num = hs_get_fru_num();

    
    malloc_size = fru_num * sizeof(GAsyncQueue *);
    g_fru_payload.m_power_ctrl_evt_qid = (GAsyncQueue **)g_malloc0(malloc_size);

    if (g_fru_payload.m_power_ctrl_evt_qid == NULL) {
        g_printf("pp_init:m_power_ctrl_evt_qid malloc fail\r\n");
        debug_log(DLOG_ERROR, "pp_init:m_power_ctrl_evt_qid malloc fail\r\n");
        return PAYLOAD_ERESOURCE;
    }

    
    malloc_size = fru_num * sizeof(FRU_PWR_CB_S);
    g_fru_payload.m_pwr_ctrl_cb = (PFRU_PWR_CB_S)g_malloc0(malloc_size);

    if (g_fru_payload.m_pwr_ctrl_cb == NULL) {
        
        g_free(g_fru_payload.m_power_ctrl_evt_qid);
        g_fru_payload.m_power_ctrl_evt_qid = NULL;
        g_printf("pp_init:m_pwr_ctrl_cb malloc fail\r\n");
        debug_log(DLOG_ERROR, "pp_init:m_pwr_ctrl_cb malloc fail\r\n");
        return PAYLOAD_ERESOURCE;
    }

    
    malloc_size = fru_num * sizeof(PWR_CYCLE_CB_S);
    g_fru_payload.m_pwr_cycle_cb = (PPWR_CYCLE_CB_S)g_malloc0(malloc_size);

    if (g_fru_payload.m_pwr_cycle_cb == NULL) {
        
        g_free(g_fru_payload.m_power_ctrl_evt_qid);
        g_free(g_fru_payload.m_pwr_ctrl_cb);
        g_fru_payload.m_power_ctrl_evt_qid = NULL;
        g_fru_payload.m_pwr_ctrl_cb = NULL;

        g_printf("pp_init:m_pwr_cycle_cb malloc fail\r\n");
        debug_log(DLOG_ERROR, "pp_init:m_pwr_cycle_cb malloc fail\r\n");
        return PAYLOAD_ERESOURCE;
    }

    
    malloc_size = fru_num * sizeof(guint8);
    g_fru_payload.m_pwr_off_lock = (guint8 *)g_malloc0(malloc_size);

    if (g_fru_payload.m_pwr_off_lock == NULL) {
        
        g_free(g_fru_payload.m_power_ctrl_evt_qid);
        g_free(g_fru_payload.m_pwr_ctrl_cb);
        g_free(g_fru_payload.m_pwr_cycle_cb);
        g_fru_payload.m_power_ctrl_evt_qid = NULL;
        g_fru_payload.m_pwr_ctrl_cb = NULL;
        g_fru_payload.m_pwr_cycle_cb = NULL;

        g_printf("pp_init:m_pwr_off_lock malloc fail\r\n");
        debug_log(DLOG_ERROR, "pp_init:m_pwr_off_lock malloc fail\r\n");
        return PAYLOAD_ERESOURCE;
    }

    
    for (fru_id = 0; fru_id < fru_num; fru_id++) {
        pp_set_pwr_off_lock(fru_id, TRUE);
    }

    
    pp_per_data_init();
    pp_save_orign_aclost_status();

    if (hop_get_imana_reset_type() == HARD_RESET) {
        
        for (fru_id = 0; fru_id < fru_num; fru_id++) {
            g_fru_payload.m_pwr_ctrl_cb[fru_id].m_pwr_state = hop_get_pwr_signal(fru_id);
            g_fru_payload.m_pwr_ctrl_cb[fru_id].m_action = PAYLOAD_CTRL_POWERNOACTION;
            g_fru_payload.m_pwr_ctrl_cb[fru_id].m_ctrl_state = PAYLOAD_CTRL_STATE_NOACTION;

            
            
            
            pp_set_pwr_off_lock(fru_id, TRUE);
        }
    } else {
        
        
        
        pp_clear_restart_action();

        
        for (fru_id = 0; fru_id < fru_num; fru_id++) {
            (void)dfl_get_data_by_alias(CLASS_PAYLOAD, fru_id, (gpointer *)&p_payload);

            if (p_payload == NULL) {
                debug_log(DLOG_DEBUG, "fru%d payload ctrl block is null\r\n", fru_id);
                continue;
            }

            
            
            g_fru_payload.m_pwr_ctrl_cb[fru_id].m_pwr_state = hop_get_pwr_signal(fru_id);
            pp_set_pwr_state_before_ac_lost(fru_id, g_fru_payload.m_pwr_ctrl_cb[fru_id].m_pwr_state);

            g_fru_payload.m_pwr_ctrl_cb[fru_id].m_action = PAYLOAD_CTRL_POWERNOACTION;
            g_fru_payload.m_pwr_ctrl_cb[fru_id].m_ctrl_state = PAYLOAD_CTRL_STATE_NOACTION;

            if (g_fru_payload.m_pwr_ctrl_cb[fru_id].m_pwr_state == HARD_POWER_OFF) {
                pp_set_pwr_off_lock(fru_id, TRUE);
            }
        }
    }

    
    pp_obj_init();

    
    (void)hop_clear_system_reset_flag();

    
    if (RET_OK != pp_init_get_power_info()) {
        debug_log(DLOG_ERROR, "pp_init_get_power_info failed.\r\n");
    }

    return PAYLOAD_SUCCESS;
}


void pp_update_restart_value(void)
{
    gint32 result;
    guint32 temp_data;
    OBJ_HANDLE object_handle = 0x00;
    P_PAYLOAD_S p_payload = NULL;
    guchar restart_cause = 0;

    result = dfl_get_data_by_alias(CLASS_PAYLOAD, CHASSIS_FRU_ID, (gpointer *)&p_payload);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d data by alias failed!\r\n", CHASSIS_FRU_ID);
        return;
    }
    
    result = dfl_get_binded_object(CLASS_PAYLOAD, CHASSIS_FRU_ID, &object_handle);
    debug_log(DLOG_ERROR, ".... restart cause=%d \n", p_payload->m_restart_cause.restart_cause);

    if (result == DFL_OK) {
        restart_cause = p_payload->m_restart_cause.restart_cause;
        if (p_payload->m_restart_cause.restart_cause > RESTART_CAUSE_IPMI_MAX) {
            
            p_payload->m_restart_cause.restart_cause = RESTART_CAUSE_OEM;
        }
        pp_update_restart_cause();
        pp_update_restart_chan();
        
        temp_data =
            MAKE_DWORD(0x00, p_payload->m_restart_cause.restart_chan, p_payload->m_restart_cause.restart_cause, 0xC7);
        result = dal_set_property_value_uint32(object_handle, PROPERTY_PAYLOAD_RESTART_VALUE, temp_data, DF_NONE);
        if (result != RET_OK) {
            debug_log(DLOG_ERROR, "set ChassisPayload.RestartValue[0x%08x] fail!(result=%d)", temp_data, result);
        }
        // 连续设置两条相同属性间延时500ms
        vos_task_delay(500);

        temp_data =
            MAKE_DWORD(0x01, p_payload->m_restart_cause.restart_chan, p_payload->m_restart_cause.restart_cause, 0xC7);
        result = dal_set_property_value_uint32(object_handle, PROPERTY_PAYLOAD_RESTART_VALUE, temp_data, DF_NONE);
        if (result != RET_OK) {
            debug_log(DLOG_ERROR, "set ChassisPayload.RestartValue[0x%08x] fail!(result=%d)", temp_data, result);
        }

        
        temp_data = MAKE_DWORD(0x00, p_payload->m_restart_cause.restart_chan, restart_cause, 0xC7);
        result = dal_set_property_value_uint32(object_handle, PROPERTY_PAYLOAD_OEM_RESTART_VALUE, temp_data, DF_NONE);
        if (result != RET_OK) {
            debug_log(DLOG_ERROR, "set ChassisPayload.OEMRestartValue[0x%08x] fail!(result=%d)", temp_data, result);
        }
        // 连续设置两条相同属性间延时500ms
        vos_task_delay(500);        
        temp_data = MAKE_DWORD(0x01, p_payload->m_restart_cause.restart_chan, restart_cause, 0xC7);
        result = dal_set_property_value_uint32(object_handle, PROPERTY_PAYLOAD_OEM_RESTART_VALUE, temp_data, DF_NONE);
        if (result != RET_OK) {
            debug_log(DLOG_ERROR, "set ChassisPayload.OEMRestartValue[0x%08x] fail!(result=%d)", temp_data, result);
        }
    } else {
        debug_log(DLOG_ERROR, "get ChassisPayload object fail!(result=%d)\r\n", result);
    }
}


void pp_deal_with_restart_cause_flag(void)
{
    pp_set_restart_action(0, RESTART_CAUSE_UNKNOWN);
    pp_set_restart_cause();
}


gint32 set_chassis_last_power_event(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value)
{
    gint32 result;
    guchar fru_id = 0;
    guchar chassis_power_state = 0;
    guchar chassis_last_power_event = 0;
    static guint32 count = 0;

    
    count++;
    if (count == 1) {
        return RET_OK;
    }

    (void)dal_get_property_value_byte(object_handle, PROPERTY_PAYLOAD_FRUID, &fru_id);

    if (fru_id == CHASSIS_FRU_ID) {
        (void)dal_get_property_value_byte(object_handle, property_name, &chassis_power_state);

        
        if (chassis_power_state == 1) {
            if (TRUE == pp_get_power_on_by_ipmi_flag()) {
                pp_set_power_on_by_ipmi_flag(FALSE);
                (void)dal_get_property_value_byte(object_handle, PROPERTY_PAYLOAD_CHASSIS_LAST_POWER_EVENT,
                    &chassis_last_power_event);
                chassis_last_power_event = (chassis_last_power_event | 0x10);

                result = dal_set_property_value_byte(object_handle, PROPERTY_PAYLOAD_CHASSIS_LAST_POWER_EVENT,
                    chassis_last_power_event, DF_SAVE_TEMPORARY);
                if (result != RET_OK) {
                    debug_log(DLOG_ERROR, "Set %s property %s to %d failed.", dfl_get_object_name(object_handle),
                        PROPERTY_PAYLOAD_CHASSIS_LAST_POWER_EVENT, chassis_last_power_event);
                    return RET_ERR;
                }
            } else {
                (void)dal_get_property_value_byte(object_handle, PROPERTY_PAYLOAD_CHASSIS_LAST_POWER_EVENT,
                    &chassis_last_power_event);
                chassis_last_power_event = (chassis_last_power_event & 0xEF);

                result = dal_set_property_value_byte(object_handle, PROPERTY_PAYLOAD_CHASSIS_LAST_POWER_EVENT,
                    chassis_last_power_event, DF_SAVE_TEMPORARY);
                if (result != RET_OK) {
                    debug_log(DLOG_ERROR, "Set %s property %s to %d failed.", dfl_get_object_name(object_handle),
                        PROPERTY_PAYLOAD_CHASSIS_LAST_POWER_EVENT, chassis_last_power_event);
                    return RET_ERR;
                }
            }
        }
    }

    return RET_OK;
}


gint32 pp_fru_pwr_on(guchar src_chan_num, guchar fru_device_id, guchar restart_cause)
{
    guchar comp_code;
    guint8 forbid_poweron_flag = 0;
    gint32 retv;

    
    if (fru_device_id == 0) {
        retv = hop_get_forbid_poweron_flag(&forbid_poweron_flag);
        if (retv != RET_OK) {
            debug_log(DLOG_ERROR, "hop_get_forbid_poweron_flag failed, retv = %d", retv);
            return COMP_CODE_UNKNOWN;
        }
        if (forbid_poweron_flag == UPGRADE_BIOS_FORBID_POWER_ON) {
            debug_log(DLOG_ERROR, "Forbid power on.\r\n");
            
            return POWER_ON_FAILED_BY_BIOS_UP;
        } else if (forbid_poweron_flag == UPGRADE_CPLD_FORBID_POWER_ON) {
            debug_log(DLOG_ERROR, "CPLD upgrade forbid power on.");
            return POWER_ON_FAILED_BY_CPLD_UP;
        } else if (forbid_poweron_flag == UPGRADE_VRD_FORBID_POWER_ON) {
            debug_log(DLOG_ERROR, "VRD upgrade forbid power on.");
            return POWER_ON_FAILED_BY_VRD_UP;
        }
    }

    if (hs_is_auto_management()) {
        comp_code = pp_fru_pwr_ctrl(fru_device_id, POWER_ON);
    } else {
        comp_code = hse_fru_activate_policy(fru_device_id, 1, 0);
    }

    if (comp_code == COMP_CODE_SUCCESS && fru_device_id == 0) {
        pp_set_restart_action(src_chan_num, restart_cause);
        pp_set_restart_cause();
    }

    return comp_code;
}


gint32 pp_apci_status_change_proc(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value)
{
    guchar fru_device_id = 0;
    guchar acpi_status = 0;

    (void)dal_get_property_value_byte(object_handle, PROPERTY_PAYLOAD_FRUID, &fru_device_id);

    if (fru_device_id != CHASSIS_FRU_ID) {
        return RET_OK;
    }

    (void)dal_get_property_value_byte(object_handle, PROPERTY_PAYLOAD_ACPI_STATUS, &acpi_status);

    if (HARD_POWER_ON == acpi_status) {
        // 直接短按上电按钮或者其他OS上电的命令导致系统上电，此时如果WOL配置是开启的，则需要打开WOL，否则不用打开WOL；
        set_wol_by_saved_value(WOL_STATE_ON);
    }

    return RET_OK;
}


LOCAL void config_composition_power_on(OBJ_HANDLE power_on_object_handle)
{
    (void)prctl(PR_SET_NAME, (gulong)"CfgComPwrOn");

    debug_log(DLOG_ERROR, "Power on after sleep 6 s \n");
    (void)vos_task_delay(CONFIG_TO_FORBID_POWER_ON);
    (void)dal_set_property_value_byte(power_on_object_handle, PROPERTY_POWER_CYCLE_CRITERIA_IS_MET, TRUE, DF_NONE);
    g_composition_config = 0;
    return;
}


gint32 config_composition_forbid_power_on_status(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value)
{
    OBJ_HANDLE power_on_object_handle = 0;
    guint8 prop_val;
    guint8 is_met_state = 0;
    gint32 ret;

    prop_val = g_variant_get_byte(property_value);

    (void)dal_get_specific_object_string(CLASS_POWER_ON_CRITERIA, PROPERTY_POWER_ON_CRITERIA_DESCRIPTION,
        "Composition config", &power_on_object_handle);
    (void)dal_get_property_value_byte(power_on_object_handle, PROPERTY_POWER_ON_CRITERIA_IS_MET, &is_met_state);

    if (is_met_state == TRUE) {
        return RET_OK;
    }

    if (prop_val == PCIeTopo_CONFIG_FINISHED || prop_val == PCIeTopo_CONFIG_TIMEOUT) {
        if (g_composition_config == 0) {
            ret = vos_task_create(&g_composition_config, "Composition config", (TASK_ENTRY)config_composition_power_on,
                (void *)power_on_object_handle, DEFAULT_PRIORITY);
            if (ret != RET_OK) {
                g_composition_config = 0;
                debug_log(DLOG_ERROR, "%s create task failed,ret = %d.\r\n", __FUNCTION__, ret);
                return RET_OK;
            }
        }
    }
    return RET_OK;
}
