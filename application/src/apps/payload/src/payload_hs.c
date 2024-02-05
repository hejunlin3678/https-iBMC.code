
#include <glib.h>
#include <glib/gprintf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <stdbool.h>
#include "pme/common/mscm_vos.h"
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "payload_hop.h"
#include "payload_ipmi.h"
#include "payload_hs.h"
#include "payload_pwr.h"
#include "payload_com.h"
#include "wdt2.h"
#include "payload_storage.h"



FRU_HOTSWAP_S *g_fru_hotswap;
guint32 g_max_fru_id = 0;

LOCAL GMutex g_hotswap_fix_mutex;
guint8 g_hotswap_flag = 1;

LOCAL void hs_save_state(guchar fru_id);
LOCAL void hs_update_cur_state(guchar fru_id);
LOCAL void hs_update_last_state(guchar fru_id);
LOCAL void hs_update_mix_state(guchar fru_id);
LOCAL void hs_update_cause(guchar fru_id);
void hs_do_pwr_on_delay(void);


LOCAL guchar hs_state_m0(guchar fru_id);
LOCAL guchar hs_state_m1(guchar fru_id);
LOCAL guchar hs_state_m2(guchar fru_id);
LOCAL guchar hs_state_m3(guchar fru_id);
LOCAL guchar hs_state_m4(guchar fru_id);
LOCAL guchar hs_state_m5(guchar fru_id);
LOCAL guchar hs_state_m6(guchar fru_id);
LOCAL guchar hs_state_m7(guchar fru_id);

LOCAL void do_after_m0(guchar fru_id);
LOCAL void do_after_m1(guchar fru_id);
LOCAL void do_after_m2(guchar fru_id);
LOCAL void do_after_m3(guchar fru_id);
LOCAL void do_after_m4(guchar fru_id);
LOCAL void do_after_m5(guchar fru_id);
LOCAL void do_after_m6(guchar fru_id);
LOCAL void do_after_m7(guchar fru_id);


LOCAL guchar (*(g_hs_route[MAX_HS_STATE + 1]))(guchar) = {
    hs_state_m0, 
    hs_state_m1, 
    hs_state_m2, 
    hs_state_m3, 
    hs_state_m4, 
    hs_state_m5, 
    hs_state_m6, 
    hs_state_m7
};


LOCAL void (*(g_do_after_route[MAX_HS_STATE + 1]))(guchar) = {
    do_after_m0, 
    do_after_m1, 
    do_after_m2, 
    do_after_m3, 
    do_after_m4, 
    do_after_m5, 
    do_after_m6, 
    do_after_m7
};




LOCAL guchar hs_state_m0(guchar fru_id)
{
    
    if (g_fru_hotswap[fru_id].hotswap.pin_mated) {
        g_printf("move M0 to M1\r\n");
        debug_log(DLOG_ERROR, "move M0 to M1\r\n");
        
        
        
        return HSE(FRU_HS_NORMAL_STATE_CHANGE, FRU_HS_STATE_M1);
    }
    
    return HSE(FRU_HS_NORMAL_STATE_CHANGE, FRU_HS_STATE_M0);
}


LOCAL guchar hs_state_m1(guchar fru_id)
{
    
    if (g_fru_hotswap[fru_id].hotswap.insertion_criteria_met) {
        // 注:满足插入条件情况并没有包含lockbit=0的情况, 因为满足插入条件不仅能使M1->M2，也能使M5->M4
        // 在当前实现中，满足插入条件即扳手合
        // 如果是其他FRU(>0)的满足插入条件 = (fru 0处于M4状态 && fru n扳手合)
        if (g_fru_hotswap[fru_id].hotswap.activate_lockbit == 0) {
            g_printf("move M1 to M2\r\n");
            debug_log(DLOG_ERROR, "move M1 to M2\r\n");
            return HSE(FRU_HS_PROGRAMMATIC_ACTION, FRU_HS_STATE_M2);
        }
    }
    return HSE(FRU_HS_NORMAL_STATE_CHANGE, FRU_HS_STATE_M1);
}


LOCAL guchar hs_state_m2(guchar fru_id)
{
    
    guchar cause;

    
    if (g_fru_hotswap[fru_id].hotswap.activated) {
        g_printf("move M2 to M3\r\n");
        debug_log(DLOG_ERROR, "move M2 to M3\r\n");
        return HSE(FRU_HS_SET_FRU_ACTIVATION, FRU_HS_STATE_M3);
    }

    
    
    if (g_fru_hotswap[fru_id].hotswap.deactivated) {
        
        cause = FRU_HS_SET_FRU_ACTIVATION;
        g_printf("move M2 to M1\r\n");
        debug_log(DLOG_ERROR, "move M2 to M1\r\n");
        return HSE(cause, FRU_HS_STATE_M1);
    } else if (g_fru_hotswap[fru_id].hotswap.extraction_criteria_met) {
        
        cause = FRU_HS_HANDLE_SWITCH;
        g_printf("move M2 to M1\r\n");
        debug_log(DLOG_ERROR, "move M2 to M1\r\n");
        return HSE(cause, FRU_HS_STATE_M1);
    }

    return HSE(FRU_HS_NORMAL_STATE_CHANGE, FRU_HS_STATE_M2);
}


LOCAL guchar hs_state_m3(guchar fru_id)
{
    
    guchar cause;

    
    
    
    if (g_fru_hotswap[fru_id].hotswap.activate_completed) {
        g_printf("move M3 to M4\r\n");
        debug_log(DLOG_ERROR, "move M3 to M4\r\n");
        return HSE(FRU_HS_NORMAL_STATE_CHANGE, FRU_HS_STATE_M4);
    }

    
    
    
    
    if (g_fru_hotswap[fru_id].hotswap.deactivated) {
        
        cause = FRU_HS_SET_FRU_ACTIVATION;
        g_printf("move M3 to M6\r\n");
        debug_log(DLOG_ERROR, "move M3 to M6\r\n");
        return HSE(cause, FRU_HS_STATE_M6);
    } else if (g_fru_hotswap[fru_id].hotswap.extraction_criteria_met) {
        
        cause = FRU_HS_HANDLE_SWITCH;
        g_printf("move M3 to M6\r\n");
        debug_log(DLOG_ERROR, "move M3 to M6\r\n");
        return HSE(cause, FRU_HS_STATE_M6);
    } else if (g_fru_hotswap[fru_id].hotswap.power_failure) {
        
        
        cause = FRU_HS_POWER_FAILURE;
        g_printf("move M3 to M6\r\n");
        debug_log(DLOG_ERROR, "move M3 to M6\r\n");
        return HSE(cause, FRU_HS_STATE_M6);
    }

    return HSE(FRU_HS_NORMAL_STATE_CHANGE, FRU_HS_STATE_M3);
}


LOCAL guchar hs_state_m4(guchar fru_id)
{
    
    guchar cause = FRU_HS_NORMAL_STATE_CHANGE;

    
    if ((g_fru_hotswap[fru_id].hotswap.extraction_criteria_met) ||
        (g_fru_hotswap[fru_id].hotswap.deactivate_lockbit == 0)) {
        if (g_fru_hotswap[fru_id].hotswap.extraction_criteria_met) {
            
            cause = FRU_HS_HANDLE_SWITCH;
        } else if (g_fru_hotswap[fru_id].hotswap.deactivate_lockbit == 0) {
            
            cause = FRU_HS_PROGRAMMATIC_ACTION;
        }

        
        g_printf("move M4 to M5\r\n");
        debug_log(DLOG_ERROR, "move M4 to M5\r\n");
        return HSE(cause, FRU_HS_STATE_M5);
    }

    
    if (g_fru_hotswap[fru_id].hotswap.deactivated || g_fru_hotswap[fru_id].hotswap.setpowerlevel_0 ||
        g_fru_hotswap[fru_id].hotswap.power_failure || g_fru_hotswap[fru_id].hotswap.unexpected_deactivation) {
        if (g_fru_hotswap[fru_id].hotswap.deactivated) {
            
            cause = FRU_HS_SET_FRU_ACTIVATION;
        } else if (g_fru_hotswap[fru_id].hotswap.setpowerlevel_0) {
            
            cause = FRU_HS_SET_FRU_ACTIVATION;
        } else if (g_fru_hotswap[fru_id].hotswap.power_failure) {
            
            cause = FRU_HS_POWER_FAILURE;
        } else if (g_fru_hotswap[fru_id].hotswap.unexpected_deactivation) {
            
            cause = FRU_HS_UNEXPECTED_DEACTIVATION;
        }

        g_printf("move M4 to M6\r\n");
        debug_log(DLOG_ERROR, "move M4 to M6\r\n");
        return HSE(cause, FRU_HS_STATE_M6);
    }

    return HSE(FRU_HS_NORMAL_STATE_CHANGE, FRU_HS_STATE_M4);
}


LOCAL guchar hs_state_m5(guchar fru_id)
{
    
    guchar cause = FRU_HS_NORMAL_STATE_CHANGE;

    
    
    
    
    
    if ((g_fru_hotswap[fru_id].hotswap.insertion_criteria_met && g_fru_hotswap[fru_id].hotswap.deactivate_lockbit) ||
        g_fru_hotswap[fru_id].hotswap.activated) {
        if (g_fru_hotswap[fru_id].hotswap.insertion_criteria_met) {
            
            cause = FRU_HS_HANDLE_SWITCH;
        } else if (g_fru_hotswap[fru_id].hotswap.activated) {
            
            cause = FRU_HS_SET_FRU_ACTIVATION;
        }
        g_printf("move M5 to M4\r\n");
        debug_log(DLOG_ERROR, "move M5 to M4\r\n");
        return HSE(cause, FRU_HS_STATE_M4);
    }

    
    if (g_fru_hotswap[fru_id].hotswap.deactivated || g_fru_hotswap[fru_id].hotswap.setpowerlevel_0 ||
        g_fru_hotswap[fru_id].hotswap.power_failure) {
        if (g_fru_hotswap[fru_id].hotswap.deactivated) {
            
            cause = FRU_HS_SET_FRU_ACTIVATION;
        } else if (g_fru_hotswap[fru_id].hotswap.setpowerlevel_0) {
            
            cause = FRU_HS_SET_FRU_ACTIVATION;
        } else if (g_fru_hotswap[fru_id].hotswap.power_failure) {
            
            cause = FRU_HS_POWER_FAILURE;
        }

        g_printf("move M5 to M6\r\n");
        debug_log(DLOG_ERROR, "move M5 to M6\r\n");
        return HSE(cause, FRU_HS_STATE_M6);
    }

    return HSE(FRU_HS_NORMAL_STATE_CHANGE, FRU_HS_STATE_M5);
}


LOCAL guchar hs_state_m6(guchar fru_id)
{
    
    guchar cause = FRU_HS_NORMAL_STATE_CHANGE;

    
    pp_printf_abnormal_pwrstate();
    
    if (g_fru_hotswap[fru_id].hotswap.unnormal_status_recovery) {
        g_printf("move M6 to M4\r\n");
        debug_log(DLOG_ERROR, "move M6 to M4\r\n");
        return HSE(FRU_HS_CAUSE_UNKNOWN, FRU_HS_STATE_M4);
    }

    
    
    
    if (g_fru_hotswap[fru_id].hotswap.deactivate_completed || g_fru_hotswap[fru_id].hotswap.power_failure) {
        if (g_fru_hotswap[fru_id].hotswap.deactivate_completed) {
            
            cause = FRU_HS_NORMAL_STATE_CHANGE;
        } else if (g_fru_hotswap[fru_id].hotswap.power_failure) {
            
            cause = FRU_HS_POWER_FAILURE;
        }

        
        
        if (HARD_POWER_ON == hop_get_pwr_signal(fru_id)) {
            return HSE(FRU_HS_CAUSE_UNKNOWN, FRU_HS_STATE_M6);
        } else {
            g_printf("move M6 to M1\r\n");
            debug_log(DLOG_ERROR, "move M6 to M1\r\n");
            return HSE(cause, FRU_HS_STATE_M1);
        }
    }

    return HSE(FRU_HS_NORMAL_STATE_CHANGE, FRU_HS_STATE_M6);
}


LOCAL guchar hs_state_m7(guchar fru_id)
{
    
    guchar hotswap_state = FRU_HS_STATE_M7;

    
    
    
    if (!g_fru_hotswap[fru_id].hotswap.communication_lost) {
        hotswap_state = g_fru_hotswap[fru_id].hotswap.last_hotswap_state;
    }

    return HSE(FRU_HS_COMMUNICATION_LOST, hotswap_state);
}


LOCAL void do_after_m0(guchar fru_id)
{
    
    return;
}


LOCAL void do_after_m1(guchar fru_id)
{
    switch (g_fru_hotswap[fru_id].hotswap.cur_hotswap_state) {
        case FRU_HS_STATE_M2:
            if (TRUE == hs_is_auto_management()) {
                
                g_printf("send activate event at M1\r\n");
                debug_log(DLOG_ERROR, "send activate event at M1\r\n");
                (void)hse_fru_activate(fru_id, HOTSWAP_ACTIVATE_FRU, TRUE);
            }
            payload_switch_loadline(OPEN_LOADLINE);
            break;

        default:
            break;
    }
}


LOCAL void do_after_m2(guchar fru_id)
{
    switch (g_fru_hotswap[fru_id].hotswap.cur_hotswap_state) {
        case FRU_HS_STATE_M1:
            
            g_fru_hotswap[fru_id].hotswap.deactivated = 0;
            break;

        case FRU_HS_STATE_M3:
            g_fru_hotswap[fru_id].hotswap.activated = 0;

            if (hs_is_auto_management() == TRUE) {
                bbu_running_state_operate(OPEN_BBU);
                g_printf("call pp_fru_pwr_ctrl(fru_id:%u, POWER_ON)\r\n", fru_id);
                debug_log(DLOG_ERROR, "call pp_fru_pwr_ctrl(fru_id:%u, POWER_ON)\r\n", fru_id);
                pp_fru_pwr_ctrl(fru_id, POWER_ON);
            }
            prepare_before_poweron();
            break;
        default:
            break;
    }
}


LOCAL void do_after_m3(guchar fru_id)
{
    switch (g_fru_hotswap[fru_id].hotswap.cur_hotswap_state) {
        case FRU_HS_STATE_M4:
            
            g_fru_hotswap[fru_id].hotswap.activate_completed = 0;
            break;
        default:
            break;
    }
}


LOCAL void do_after_m4(guchar fru_id)
{
    guchar ret;

    switch (g_fru_hotswap[fru_id].hotswap.cur_hotswap_state) {
        
        
        case FRU_HS_STATE_M5:
            
            ret = hse_fru_activate(fru_id, HOTSWAP_DEACTIVATE_FRU, TRUE);
            if (ret != COMP_CODE_SUCCESS) {
                debug_log(DLOG_DEBUG, "%s:line:%d hse_fru_activate fail!\n", __FUNCTION__, __LINE__);
            }
            break;

        case FRU_HS_STATE_M6:
            g_fru_hotswap[fru_id].hotswap.unexpected_deactivation = 0;
            break;
        default:
            break;
    }
}


LOCAL void do_after_m5(guchar fru_id)
{
    switch (g_fru_hotswap[fru_id].hotswap.cur_hotswap_state) {
        case FRU_HS_STATE_M4:
            
            
            g_fru_hotswap[fru_id].hotswap.activated = 0;
            break;
        default:
            break;
    }
}


LOCAL void do_after_m6(guchar fru_id)
{
    switch (g_fru_hotswap[fru_id].hotswap.cur_hotswap_state) {
        case FRU_HS_STATE_M1: // M6->M1

            
            
            g_fru_hotswap[fru_id].hotswap.power_failure = 0;

            
            g_fru_hotswap[fru_id].hotswap.setpowerlevel_0 = 0;

            
            g_fru_hotswap[fru_id].hotswap.deactivate_completed = 0;

            
            g_fru_hotswap[fru_id].hotswap.deactivated = 0;

            
            g_fru_hotswap[fru_id].hotswap.unexpected_deactivation = 0;

            
            wdt_stop();

            
            prepare_after_poweroff();
            pp_set_pwr_off_lock(fru_id, TRUE);

            break;

        case FRU_HS_STATE_M4:

            g_fru_hotswap[fru_id].hotswap.deactivate_lockbit = 1;

            g_fru_hotswap[fru_id].hotswap.activate_completed = 0;

            g_fru_hotswap[fru_id].hotswap.deactivate_completed = 0;

            g_fru_hotswap[fru_id].hotswap.deactivated = 0;

            g_fru_hotswap[fru_id].hotswap.unnormal_status_recovery = 0;
            break;

        default:
            break;
    }
}


LOCAL void do_after_m7(guchar fru_id)
{
    
    return;
}


guchar hse_fru_activate(guchar fruid, guchar activate, guchar flag)
{
    
    FRU_EVENT_S fru_event;

    if ((fruid >= hs_get_fru_num()) || (FALSE == hs_is_fru_managed(fruid))) {
        return COMP_CODE_OUTOF_RANGE;
    }

    if (FALSE == hop_is_fru_present(fruid)) {
        
        return COMP_CODE_DES_UNAVAILABLE;
    }

    
    if (FRU_HS_STATE_M1 == hs_get_cur_state(fruid)) {
        return COMP_CODE_STATUS_INVALID;
    }

    if ((activate != HOTSWAP_ACTIVATE_FRU) && (activate != HOTSWAP_DEACTIVATE_FRU)) {
        return COMP_CODE_INVALID_FIELD;
    }

    fru_event.fruid = fruid;
    fru_event.event = FRU_ACTIVATED_DEACTIVATED;
    fru_event.value = activate;
    fru_event.casue = FRU_HS_SET_FRU_ACTIVATION;
    fru_event.count = 0;
    g_printf("hse_fru_activate:sending %s event\r\n", (activate == HOTSWAP_ACTIVATE_FRU) ? "active" : "deactive");
    debug_log(DLOG_ERROR, "hse_fru_activate:sending %s event",
        (activate == HOTSWAP_ACTIVATE_FRU) ? "active" : "deactive");
    hs_send_evt(fruid, &fru_event);

    if (activate == HOTSWAP_DEACTIVATE_FRU) {
        if (flag == TRUE) {
            
            pp_set_cycle_progress_state(CHASSIS_FRU_ID, FALSE);
            pp_set_cycle_interval(CHASSIS_FRU_ID, 0);
            pp_save_cycle_param();
            g_printf("hse_fru_activate:pp_fru_pwr_ctrl(POWER_OFF)\r\n");
            debug_log(DLOG_ERROR, "hse_fru_activate:pp_fru_pwr_ctrl(POWER_OFF)\r\n");
            (void)pp_fru_pwr_ctrl(fruid, POWER_OFF);
        }
    }

    return COMP_CODE_SUCCESS;
}


void hse_activate_completed(guchar fruid)
{
    
    FRU_EVENT_S fru_event;

    if ((fruid >= hs_get_fru_num()) || (FALSE == hs_is_fru_managed(fruid))) {
        return;
    }

    
    
    if (hs_get_cur_state(fruid) < FRU_HS_STATE_M2) {
        g_printf("hse_activate_completed:hse_fru_activate_policy\r\n");
        debug_log(DLOG_ERROR, "hse_activate_completed:hse_fru_activate_policy\r\n");
        (void)hse_fru_activate_policy(fruid, 1, 0);
    }

    
    if (hs_get_cur_state(fruid) < FRU_HS_STATE_M3) {
        
        fru_event.fruid = fruid;
        fru_event.event = FRU_ACTIVATED_DEACTIVATED;
        fru_event.value = 1;
        fru_event.casue = FRU_HS_SET_FRU_ACTIVATION;
        fru_event.count = 0;
        g_printf("hse_activate_completed:hs_send_evt(FRU_ACTIVATED_DEACTIVATED)\r\n");
        debug_log(DLOG_ERROR, "hse_activate_completed:hs_send_evt(FRU_ACTIVATED_DEACTIVATED)\r\n");
        hs_send_evt(fruid, &fru_event);
    }

    
    fru_event.fruid = fruid;
    fru_event.event = FRU_ACTIVATED_COMPLETED;
    fru_event.value = 1;
    fru_event.casue = FRU_HS_NORMAL_STATE_CHANGE;
    fru_event.count = 0;
    g_printf("hse_activate_completed:hs_send_evt(FRU_ACTIVATED_COMPLETED)\r\n");
    debug_log(DLOG_ERROR, "hse_activate_completed:hs_send_evt(FRU_ACTIVATED_COMPLETED)\r\n");
    hs_send_evt(fruid, &fru_event);
}


void hse_deactivate_completed(guchar fruid)
{
    
    FRU_EVENT_S fru_event;

    if ((fruid >= hs_get_fru_num()) || (FALSE == hs_is_fru_managed(fruid))) {
        return;
    }

    
    
    if (FRU_HS_STATE_M4 == hs_get_cur_state(fruid)) {
        debug_log(DLOG_ERROR, "cur_state is M4 and power drop unexpectly.\r\n");
        hse_unexpected_deactivation(fruid);
    }

    
    fru_event.fruid = fruid;
    fru_event.event = FRU_DEACTIVATED_COMPLETED;
    fru_event.value = 1;
    fru_event.casue = FRU_HS_NORMAL_STATE_CHANGE;
    fru_event.count = 0;
    hs_send_evt(fruid, &fru_event);
}


void hse_pwr_level0(guchar fruid)
{
    
    FRU_EVENT_S fru_event;

    if ((fruid >= hs_get_fru_num()) || (FALSE == hs_is_fru_managed(fruid))) {
        return;
    }

    
    fru_event.fruid = fruid;
    fru_event.event = FRU_SETPOWERLEVEL0;
    fru_event.value = 1;
    fru_event.casue = FRU_HS_SET_FRU_ACTIVATION;
    fru_event.count = 0;
    hs_send_evt(fruid, &fru_event);
}


gint32 hse_unnormal_status_recovery(guchar fruid)
{
    
    FRU_EVENT_S fru_event;

    if (fruid >= hs_get_fru_num()) {
        debug_log(DLOG_ERROR, "%s: fruid(%u) out of range.", __FUNCTION__, fruid);
        return RET_ERR;
    }

    if (FALSE == hs_is_fru_managed(fruid)) {
        debug_log(DLOG_DEBUG, "%s: fruid(%u) can not be managed.", __FUNCTION__, fruid);
        return RET_ERR;
    }

    if (FRU_HS_STATE_M6 == g_fru_hotswap[fruid].hotswap.cur_hotswap_state) {
        g_printf("hse_unnormal_status_recovery: send event.\r\n");
        debug_log(DLOG_ERROR, "hse_unnormal_status_recovery: send event.\r\n");
        
        fru_event.fruid = fruid;
        fru_event.event = FRU_UNNOMAL_STATE_RECOVERY;
        fru_event.value = TRUE;
        fru_event.casue = FRU_HS_CAUSE_UNKNOWN;
        fru_event.count = 0;
        hs_send_evt(fruid, &fru_event);

        return RET_OK;
    } else {
        debug_log(DLOG_ERROR, "hse_unnormal_status_recovery: send event failed");
        return RET_ERR;
    }
}


void hse_communication_lost(guchar fruid, guchar value)
{
    
    FRU_EVENT_S fru_event;

    if ((fruid >= hs_get_fru_num()) || (FALSE == hs_is_fru_managed(fruid))) {
        return;
    }

    
    fru_event.fruid = fruid;
    fru_event.event = FRU_COMMUNICATION_LOST;
    fru_event.value = value;
    fru_event.casue = FRU_HS_COMMUNICATION_LOST;
    fru_event.count = 0;
    hs_send_evt(fruid, &fru_event);
}


void hse_pwr_failure(guchar fruid, guchar value)
{
    
    FRU_EVENT_S fru_event;

    if ((fruid >= hs_get_fru_num()) || (FALSE == hs_is_fru_managed(fruid))) {
        return;
    }

    
    fru_event.fruid = fruid;
    fru_event.event = FRU_POWER_FAILURE;
    fru_event.value = value;
    fru_event.casue = FRU_HS_POWER_FAILURE;
    fru_event.count = 0;
    hs_send_evt(fruid, &fru_event);
}


void hse_unexpected_deactivation(guchar fruid)
{
    
    FRU_EVENT_S fru_event;

    if ((fruid >= hs_get_fru_num()) || (FALSE == hs_is_fru_managed(fruid))) {
        return;
    }

    
    fru_event.fruid = fruid;
    fru_event.event = FRU_UNEXPECTED_DEACTIVATION;
    fru_event.value = 1;
    fru_event.casue = FRU_HS_UNEXPECTED_DEACTIVATION;
    fru_event.count = 0;
    hs_send_evt(fruid, &fru_event);
}


guchar hse_fru_activate_policy(guchar fruid, guchar mask, guchar bits)
{
    
    FRU_EVENT_S fru_event;

    if (fruid >= hs_get_fru_num()) {
        debug_log(DLOG_ERROR, "%s: fruid(%u) out of range.", __FUNCTION__, fruid);
        return COMP_CODE_OUTOF_RANGE;
    }

    if (FALSE == hs_is_fru_managed(fruid)) {
        debug_log(DLOG_DEBUG, "%s: fruid(%u) can not be managed.", __FUNCTION__, fruid);
        return COMP_CODE_OUTOF_RANGE;
    }

    if (FALSE == hop_is_fru_present(fruid)) {
        
        debug_log(DLOG_ERROR, "%s: fru is not present.", __FUNCTION__);
        return COMP_CODE_DES_UNAVAILABLE;
    }

    fru_event.fruid = fruid;
    fru_event.casue = FRU_HS_PROGRAMMATIC_ACTION;
    fru_event.count = 0;

    if (mask & PBIT0) {
        
        if (FRU_HS_STATE_M6 == g_fru_hotswap[fruid].hotswap.cur_hotswap_state) {
            if (HARD_POWER_ON == hop_get_pwr_signal(fruid)) {
                (void)hse_unnormal_status_recovery(fruid);
            }
        } else {
            fru_event.value = bits & PBIT0;
            fru_event.event = FRU_SET_M1_LOCKBIT;
            hs_send_evt(fruid, &fru_event);

            if (fru_event.value == 0) {
                pp_set_pwr_off_lock(fruid, FALSE);
            }
        }
    }

    if (mask & PBIT1) {
        // 下电后，用户取消下电，当前处于on/m6,为允许再次下发短按，此处增加特殊处理
        if (FRU_HS_STATE_M6 == g_fru_hotswap[fruid].hotswap.cur_hotswap_state) {
            if (HARD_POWER_ON == hop_get_pwr_signal(fruid)) {
                g_printf("hse_fru_activate_policy:pp_fru_pwr_ctrl(POWER_OFF)\r\n");
                debug_log(DLOG_ERROR, "hse_fru_activate_policy:pp_fru_pwr_ctrl(POWER_OFF)\r\n");
                (void)pp_fru_pwr_ctrl(fruid, POWER_OFF);
            }
        } else {
            fru_event.value = (bits & PBIT1) >> 1;
            fru_event.event = FRU_SET_M4_LOCKBIT;
            hs_send_evt(fruid, &fru_event);
        }
    }

    return COMP_CODE_SUCCESS;
}


LOCAL void on_evt_pin_mated(guchar fru_id, guchar pin_mated)
{
    
    
    g_fru_hotswap[fru_id].hotswap.pin_mated = pin_mated;
}


LOCAL void on_evt_insertion_criteria(guchar fru_id, guchar criteria)
{
    
    g_fru_hotswap[fru_id].hotswap.insertion_criteria_met = criteria;

    
    if (criteria) {
        
        g_fru_hotswap[fru_id].hotswap.extraction_criteria_met = 0;
        
        g_fru_hotswap[fru_id].hotswap.activate_lockbit = 0;
        
        
        g_fru_hotswap[fru_id].hotswap.deactivate_lockbit = 1;
    }
}


LOCAL void on_evt_extraction_criteria(guchar fru_id, guchar criteria)
{
    
    g_fru_hotswap[fru_id].hotswap.extraction_criteria_met = criteria;

    
    if (criteria) {
        
        g_fru_hotswap[fru_id].hotswap.insertion_criteria_met = 0;

        
        g_fru_hotswap[fru_id].hotswap.deactivate_lockbit = 0;
    }
}

LOCAL void on_evt_activation(guchar fru_id, guchar actvication)
{
    
    guchar hotswapstate;

    hotswapstate = g_fru_hotswap[fru_id].hotswap.cur_hotswap_state;

    if (actvication) {
        
        
        
        if (hotswapstate != FRU_HS_STATE_M4) {
            g_fru_hotswap[fru_id].hotswap.activated = 1;
            g_fru_hotswap[fru_id].hotswap.deactivated = 0;
            g_fru_hotswap[fru_id].hotswap.deactivate_lockbit = 1;
        }
    } else {
        
        
        
        if (hotswapstate != FRU_HS_STATE_M1) {
            g_fru_hotswap[fru_id].hotswap.activated = 0;
            g_fru_hotswap[fru_id].hotswap.deactivated = 1;
            g_fru_hotswap[fru_id].hotswap.activate_lockbit = 1;
        }
    }
}


LOCAL void on_evt_activated_completed(guchar fru_id, guchar actvication)
{
    
    guchar hotswapstate;

    hotswapstate = g_fru_hotswap[fru_id].hotswap.cur_hotswap_state;

    if (actvication) {
        
        if (hotswapstate != FRU_HS_STATE_M4) {
            g_fru_hotswap[fru_id].hotswap.activate_completed = 1;
        }
    } else {
        g_fru_hotswap[fru_id].hotswap.activate_completed = 0;
    }
}


LOCAL void on_evt_deactivated_completed(guchar fru_id, guchar deactvication)
{
    
    guchar hotswapstate;

    hotswapstate = g_fru_hotswap[fru_id].hotswap.cur_hotswap_state;

    if (deactvication) {
        
        if (hotswapstate != FRU_HS_STATE_M1) {
            g_fru_hotswap[fru_id].hotswap.deactivate_completed = 1;
        }
    } else {
        g_fru_hotswap[fru_id].hotswap.deactivate_completed = 0;
    }
}


LOCAL void on_evt_power_level0(guchar fru_id, guchar level)
{
    
    guchar hotswapstate;

    hotswapstate = g_fru_hotswap[fru_id].hotswap.cur_hotswap_state;

    if (level) {
        
        
        if (hotswapstate != FRU_HS_STATE_M1) {
            g_fru_hotswap[fru_id].hotswap.setpowerlevel_0 = 1;
            g_fru_hotswap[fru_id].hotswap.activate_lockbit = 1;
        }
    } else {
        g_fru_hotswap[fru_id].hotswap.setpowerlevel_0 = 0;
    }
}


LOCAL void on_evt_power_failure(guchar fru_id, guchar failure)
{
    
    guchar hotswapstate;

    hotswapstate = g_fru_hotswap[fru_id].hotswap.cur_hotswap_state;
    if (failure) {
        
        
        if (hotswapstate != FRU_HS_STATE_M1) {
            g_fru_hotswap[fru_id].hotswap.power_failure = 1;
            g_fru_hotswap[fru_id].hotswap.activate_lockbit = 1;
        }
    } else {
        g_fru_hotswap[fru_id].hotswap.power_failure = 0;
    }
}


LOCAL void on_evt_unexpected_deactivation(guchar fru_id, guchar value)
{
    
    guchar hotswapstate;
    hotswapstate = g_fru_hotswap[fru_id].hotswap.cur_hotswap_state;

    if (value) {
        
        
        if (hotswapstate != FRU_HS_STATE_M1) {
            g_fru_hotswap[fru_id].hotswap.unexpected_deactivation = 1;
            g_fru_hotswap[fru_id].hotswap.activate_lockbit = 1;
        }
    } else {
        g_fru_hotswap[fru_id].hotswap.unexpected_deactivation = 0;
    }
}

LOCAL void on_evt_unnormal_status_recovery(guchar fru_id, guchar value)
{
    
    g_fru_hotswap[fru_id].hotswap.unnormal_status_recovery = value;

    debug_log(DLOG_ERROR, "on_evt_unnormal_status_recovery: recv event.\r\n");
}


LOCAL void on_evt_m1_lockbit(guchar fru_id, guchar value)
{
    
    g_fru_hotswap[fru_id].hotswap.activate_lockbit = value;
}


LOCAL void on_evt_m4_lockbit(guchar fru_id, guchar value)
{
    
    g_fru_hotswap[fru_id].hotswap.deactivate_lockbit = value;
}


LOCAL void on_evt_communication_lost(guchar fru_id, guchar value)
{
    
    g_fru_hotswap[fru_id].hotswap.communication_lost = value;
}


LOCAL void on_hs_evt(guchar fru_id, PFRU_EVENT_S fru_event)
{
    
    
    switch (fru_event->event) {
            
        case FRU_PIN_MATED:
            on_evt_pin_mated(fru_id, fru_event->value);
            break;

            
        case FRU_ACTIVATED_DEACTIVATED:
            on_evt_activation(fru_id, fru_event->value);
            break;

            
        case FRU_INSERTION_CRITERIA_MET:
            on_evt_insertion_criteria(fru_id, fru_event->value);
            break;

            
        case FRU_EXTRACTION_CRITERIA_MET:
            on_evt_extraction_criteria(fru_id, fru_event->value);
            break;

            
        case FRU_SET_M1_LOCKBIT:
            on_evt_m1_lockbit(fru_id, fru_event->value);
            break;

            
        case FRU_SET_M4_LOCKBIT:
            on_evt_m4_lockbit(fru_id, fru_event->value);
            break;

            
        case FRU_ACTIVATED_COMPLETED:
            on_evt_activated_completed(fru_id, fru_event->value);
            break;

            
        case FRU_DEACTIVATED_COMPLETED:
            on_evt_deactivated_completed(fru_id, fru_event->value);
            break;

            
        case FRU_SETPOWERLEVEL0:
            on_evt_power_level0(fru_id, fru_event->value);
            break;

            
        case FRU_COMMUNICATION_LOST:
            on_evt_communication_lost(fru_id, fru_event->value);
            break;

            
        case FRU_POWER_FAILURE:
            on_evt_power_failure(fru_id, fru_event->value);
            break;

            
        case FRU_UNEXPECTED_DEACTIVATION:
            on_evt_unexpected_deactivation(fru_id, fru_event->value);
            break;

            
        case FRU_UNNOMAL_STATE_RECOVERY:
            on_evt_unnormal_status_recovery(fru_id, fru_event->value);
            break;

        default:
            debug_log(DLOG_ERROR, "hs_process_task: FRU#%d error event received.\r\n", fru_id);
            break;
    }

    g_free(fru_event);
}


LOCAL void hs_state_rotate(guchar fru_id)
{
    guchar hotswap_cause;
    guchar cause;
    guchar last_hotswapstate;
    guchar hotswapstate = g_fru_hotswap[fru_id].hotswap.cur_hotswap_state;

    
    if (fru_id && (!g_fru_hotswap[fru_id].hotswap.pin_mated)) {
        hotswapstate = FRU_HS_STATE_M0;
        cause = FRU_HS_EXTRACTION;
        
    } else if ((g_fru_hotswap[fru_id].hotswap.cur_hotswap_state != FRU_HS_STATE_M0) &&
        (g_fru_hotswap[fru_id].hotswap.cur_hotswap_state != FRU_HS_STATE_M7) &&
        (g_fru_hotswap[fru_id].hotswap.communication_lost)) {
        hotswapstate = FRU_HS_STATE_M7;
        cause = FRU_HS_COMMUNICATION_LOST;
    } else {
        if (hotswapstate <= FRU_HS_STATE_M7) {
            
            hotswap_cause = g_hs_route[hotswapstate](fru_id);
            hotswapstate = (guchar)(hotswap_cause & 0xf);
            cause = (guchar)(hotswap_cause >> HALF_BYTE);
        } else {
            
            hotswapstate = FRU_HS_STATE_M0;
            cause = FRU_HS_CAUSE_UNKNOWN;
        }
    }

    if (hotswapstate != g_fru_hotswap[fru_id].hotswap.cur_hotswap_state) {
        last_hotswapstate = g_fru_hotswap[fru_id].hotswap.cur_hotswap_state;

        
        g_fru_hotswap[fru_id].hotswap.last_hotswap_state = g_fru_hotswap[fru_id].hotswap.cur_hotswap_state;
        g_fru_hotswap[fru_id].hotswap.cur_hotswap_state = hotswapstate;

        
        hs_update_cur_state(fru_id);

        
        hs_update_last_state(fru_id);

        
        hs_update_mix_state(fru_id);

        
        
        if (last_hotswapstate <= FRU_HS_STATE_M7) {
            
            g_do_after_route[last_hotswapstate](fru_id);
        }

        g_fru_hotswap[fru_id].hotswap.hotswap_cause = cause;
        
        hs_update_cause(fru_id);
        
        hs_save_state(fru_id);

        
        g_printf("\r\nManaged FRU %d state changed,cause is %-2d.......................M%d->M%d\r\n", fru_id, cause,
            g_fru_hotswap[fru_id].hotswap.last_hotswap_state, hotswapstate);
        debug_log(DLOG_ERROR, "%s : Managed FRU %d state changed,cause is %-2d, M%d->M%d", __FUNCTION__, fru_id, cause,
            g_fru_hotswap[fru_id].hotswap.last_hotswap_state, hotswapstate);
    }
}


LOCAL void hs_process_task(void *param)
{
    guint32 result;
    gpointer p_evt_msg = NULL;
    guchar fru_id = POINTER_TO_UINT8(param);
    (void)prctl(PR_SET_NAME, (gulong)"HsProcess");

    if ((fru_id >= hs_get_fru_num()) || (FALSE == hs_is_fru_managed(fru_id))) {
        return;
    }

    g_printf("Managed FRU %d current state is ...............................M%d\r\n", fru_id,
        g_fru_hotswap[fru_id].hotswap.cur_hotswap_state);

    
    hs_update_cur_state(fru_id);
    
    hs_update_last_state(fru_id);
    
    if (g_fru_hotswap[fru_id].hotswap.cur_hotswap_state != 0) {
        hs_update_mix_state(fru_id);
    } else {
        g_printf("M0, continue!!\r\n");
    }
    
    hs_update_cause(fru_id);

    for (;;) {
        
        
        result = vos_queue_receive(g_fru_hotswap[fru_id].hotswap_event_qid, &p_evt_msg, 100);
        if (result == RET_OK) {
            
            on_hs_evt(fru_id, (PFRU_EVENT_S)p_evt_msg);
            hs_save_state(fru_id);
        } else {
            
            vos_task_delay(100);
            continue;
        }
        hs_state_rotate(fru_id);
    }
}


void hs_send_evt(guchar fru_id, PFRU_EVENT_S pevent)
{
    errno_t safe_fun_ret;
    gint32 ret_val;
    PFRU_EVENT_S p_evt_msg;

    if ((fru_id >= hs_get_fru_num()) || (FALSE == hs_is_fru_managed(fru_id))) {
        return;
    }

    p_evt_msg = (PFRU_EVENT_S)g_malloc0(sizeof(FRU_EVENT_S));
    if (p_evt_msg == NULL) {
        return;
    }

    safe_fun_ret = memmove_s(p_evt_msg, sizeof(FRU_EVENT_S), pevent, sizeof(FRU_EVENT_S));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }

    debug_log(DLOG_DEBUG, "hs_send_evt:pevent=%d", p_evt_msg->event);

    ret_val = vos_queue_send(g_fru_hotswap[fru_id].hotswap_event_qid, (gpointer)p_evt_msg);
    if (ret_val != RET_OK) {
        g_free(p_evt_msg);
        debug_log(DLOG_ERROR, "hs_send_evt: fru:%u que send failed(Ret:%d).\r\n", fru_id, ret_val);
    }
}


gint32 hs_dump_cmd(GSList *input_list)
{
    guchar i;
    guint32 fru_num;

    fru_num = hs_get_fru_num();

    for (i = 0; i < fru_num; i++) {
        debug_printf("----------------------------------------\r\n");
        debug_printf("FRU%02d Hotswap Info:\r\n", i);
        debug_printf("Current hotswap state:M%d\r\n", g_fru_hotswap[i].hotswap.cur_hotswap_state);
        debug_printf("Last hotswap state:M%d\r\n", g_fru_hotswap[i].hotswap.last_hotswap_state);

        
        debug_printf("\r\n\r\n");
        debug_printf("pin_mated = %d\r\n", g_fru_hotswap[i].hotswap.pin_mated);
        debug_printf("activated = %d\r\n", g_fru_hotswap[i].hotswap.activated);
        debug_printf("deactivated = %d\r\n", g_fru_hotswap[i].hotswap.deactivated);
        debug_printf("insertion_criteria_met = %d\r\n", g_fru_hotswap[i].hotswap.insertion_criteria_met);
        debug_printf("extraction_criteria_met = %d\r\n", g_fru_hotswap[i].hotswap.extraction_criteria_met);
        debug_printf("activate_completed = %d\r\n", g_fru_hotswap[i].hotswap.activate_completed);
        debug_printf("deactivate_completed = %d\r\n", g_fru_hotswap[i].hotswap.deactivate_completed);
        debug_printf("setpowerlevel_0 = %d\r\n", g_fru_hotswap[i].hotswap.setpowerlevel_0);
        debug_printf("communication_lost = %d\r\n", g_fru_hotswap[i].hotswap.communication_lost);
        debug_printf("power_failure = %d\r\n", g_fru_hotswap[i].hotswap.power_failure);
        debug_printf("unexpected_deactivation = %d\r\n", g_fru_hotswap[i].hotswap.unexpected_deactivation);
        debug_printf("activate_lockbit = %d\r\n", g_fru_hotswap[i].hotswap.activate_lockbit);
        debug_printf("deactivate_lockbit = %d\r\n", g_fru_hotswap[i].hotswap.deactivate_lockbit);
        debug_printf("power_alloced = %d\r\n", g_fru_hotswap[i].hotswap.power_alloced);
        debug_printf("----------------------------------------\r\n");
    }
    return 0;
}


LOCAL void hs_get_key(guchar fru_id, gchar *key)
{
    if (!key) {
        return;
    }

    (void)snprintf_s(key, HOTSWAP_TEMP_BUF_LEN, HOTSWAP_TEMP_BUF_LEN - 1, "FRU%uHSS", fru_id);
}


LOCAL void hs_update_cur_state(guchar fru_id)
{
    gint32 result;
    guint8 temp_data;
    OBJ_HANDLE object_handle;
    GVariant *property_data = NULL;

    
    result = dfl_get_binded_object(CLASS_HOTSWAP, fru_id, &object_handle);
    if (result == DFL_OK) {
        
        temp_data = g_fru_hotswap[fru_id].hotswap.cur_hotswap_state;
        property_data = g_variant_new_byte(temp_data);
        result = dfl_set_property_value(object_handle, PROPERTY_HS_CRU_STATE, property_data, DF_NONE);
        g_variant_unref(property_data);
        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "set Hotswap%d object CurrentHotswapState fail!(result=%d)\r\n", fru_id, result);
        }
    } else {
        debug_log(DLOG_ERROR, "get Hotswap%d object fail!(result=%d)\r\n", fru_id, result);
    }
}


LOCAL void hs_update_last_state(guchar fru_id)
{
    gint32 result;
    guint8 temp_data;
    OBJ_HANDLE object_handle;
    GVariant *property_data = NULL;

    
    result = dfl_get_binded_object(CLASS_HOTSWAP, fru_id, &object_handle);
    if (result == DFL_OK) {
        
        temp_data = g_fru_hotswap[fru_id].hotswap.last_hotswap_state;
        property_data = g_variant_new_byte(temp_data);
        result = dfl_set_property_value(object_handle, PROPERTY_HS_LAST_STATE, property_data, DF_NONE);
        g_variant_unref(property_data);
        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "set Hotswap%d object LastHotswapState fail!(result=%d)\r\n", fru_id, result);
        }
    } else {
        debug_log(DLOG_ERROR, "get Hotswap%d object fail!(result=%d)\r\n", fru_id, result);
    }
}


LOCAL void hs_update_mix_state(guchar fru_id)
{
    gint32 result;
    guint32 temp_data;
    OBJ_HANDLE object_handle;
    GVariant *property_data = NULL;

    
    result = dfl_get_binded_object(CLASS_HOTSWAP, fru_id, &object_handle);
    if (result == DFL_OK) {
        
        temp_data = MAKE_DWORD(0x01, fru_id, g_fru_hotswap[fru_id].hotswap.last_hotswap_state,
            g_fru_hotswap[fru_id].hotswap.cur_hotswap_state);

        property_data = g_variant_new_uint32(temp_data);
        result = dfl_set_property_value(object_handle, PROPERTY_HS_MIX_STATE, property_data, DF_NONE);
        g_variant_unref(property_data);
        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "set Hotswap%d object MixHotswapState fail!(result=%d)\r\n", fru_id, result);
        }
    } else {
        debug_log(DLOG_ERROR, "get Hotswap%d object fail!(result=%d)\r\n", fru_id, result);
    }
}


LOCAL void hs_update_cause(guchar fru_id)
{
    gint32 result;
    guint8 temp_data;
    OBJ_HANDLE object_handle;
    GVariant *property_data = NULL;

    
    result = dfl_get_binded_object(CLASS_HOTSWAP, fru_id, &object_handle);
    if (result == DFL_OK) {
        
        temp_data = g_fru_hotswap[fru_id].hotswap.hotswap_cause;
        property_data = g_variant_new_byte(temp_data);
        result = dfl_set_property_value(object_handle, PROPERTY_HS_CAUSE, property_data, DF_NONE);
        g_variant_unref(property_data);
        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "set Hotswap%d object HotswapCause fail!(result=%d)\r\n", fru_id, result);
        }
    } else {
        debug_log(DLOG_ERROR, "get Hotswap%d object fail!(result=%d)\r\n", fru_id, result);
    }
}


LOCAL void hs_save_state(guchar fru_id)
{
    per_save((guint8 *)&g_fru_hotswap[fru_id].hotswap);
}


gint32 hs_com_lost_evt_process(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value)
{
    
    hse_communication_lost(0, 0);
    return RET_OK;
}


void hs_do_default_mode_delay(gboolean is_logged)
{
    guint32 delay;
    guint8 ser_type = 0;
    gdouble delay_sec;

    
    (void)pp_get_server_type(&ser_type);

    if (ser_type == SERVER_TYPE_CHASSIS) {
        
        (void)vos_get_random_array((guchar *)&delay, sizeof(delay));
        delay = delay % DELAY_TWO_SECOND;
        vos_task_delay(delay);
    } else {
        
        delay = hop_get_fru_site_num(CHASSIS_FRU_ID) * DELAY_HALF_SECOND;
        vos_task_delay(delay);
    }
    delay_sec = (delay * 1.0) / DELAY_ONE_SECOND;
    if (is_logged) {
        g_printf(LOG_POWER_ON_DELAY, pp_delay_mode_to_string(DELAY_MODE_SHORT), delay_sec);
        debug_log(DLOG_ERROR, LOG_POWER_ON_DELAY, pp_delay_mode_to_string(DELAY_MODE_SHORT), delay_sec);
    } else {
        
        g_printf("Delay %0.1f Seconds\r\n", delay_sec);
    }
}


LOCAL void hs_do_half_mode_delay(guint32 delay_count)
{
    gdouble delay_sec = 0;

    guint32 tmp = 0;
    (void)vos_get_random_array((guchar *)&tmp, sizeof(tmp));
    debug_log(DLOG_DEBUG, "hs_do_half_mode_delay  tmp is %d", tmp);

    if (tmp % INT_TWO) {
        
        if (delay_count > 0) {
            vos_task_delay(delay_count * HUNDRED_MILLISECONDS);
            delay_sec = delay_count / DOUBLE_TEN;
        } else {
            
            vos_task_delay(INT_THIRTY * DELAY_ONE_SECOND);
            delay_sec = DOUBLE_THIRTY;
        }
    }
    g_printf(LOG_POWER_ON_DELAY, pp_delay_mode_to_string(DELAY_MODE_HALF), delay_sec);
    debug_log(DLOG_ERROR, LOG_POWER_ON_DELAY, pp_delay_mode_to_string(DELAY_MODE_HALF), delay_sec);
}


LOCAL void hs_do_all_mode_delay(guint32 delay_count)
{
    gdouble delay_sec;
    if (delay_count > 0) {
        vos_task_delay(delay_count * HUNDRED_MILLISECONDS);
        delay_sec = delay_count / DOUBLE_TEN;
    } else {
        vos_task_delay(INT_THIRTY * DELAY_ONE_SECOND);
        delay_sec = DOUBLE_THIRTY;
    }
    g_printf(LOG_POWER_ON_DELAY, pp_delay_mode_to_string(DELAY_MODE_ALL), delay_sec);
    debug_log(DLOG_ERROR, LOG_POWER_ON_DELAY, pp_delay_mode_to_string(DELAY_MODE_ALL), delay_sec);
}


LOCAL void hs_do_random_mode_delay(guint32 delay_count)
{
    guint32 delay;
    gdouble delay_sec;
    if (delay_count > 0) {
        
        (void)vos_get_random_array((guchar *)&delay, sizeof(delay));
        delay_count = delay % delay_count;
        vos_task_delay(delay_count * HUNDRED_MILLISECONDS);
        delay_sec = delay_count / DOUBLE_TEN;
    } else {
        
        vos_task_delay(INT_THIRTY * DELAY_ONE_SECOND);
        delay_sec = DOUBLE_THIRTY;
    }

    g_printf(LOG_POWER_ON_DELAY, pp_delay_mode_to_string(DELAY_MODE_RANDOM), delay_sec);
    debug_log(DLOG_ERROR, LOG_POWER_ON_DELAY, pp_delay_mode_to_string(DELAY_MODE_RANDOM), delay_sec);
}


void hs_do_pwr_on_delay(void)
{
    guint32 delay_count = 0;
    guint8 delay_mode = 0;

    
    pp_get_pwr_on_delay_param(&delay_mode, &delay_count);

    
    if (delay_count > MAX_POWER_DELAY_COUNT) {
        delay_count = MAX_POWER_DELAY_COUNT;
    }

    switch (delay_mode) {
            
        case DELAY_MODE_SHORT:
            hs_do_default_mode_delay(TRUE);
            break;

            
        case DELAY_MODE_HALF:
            hs_do_half_mode_delay(delay_count);
            break;

            
        case DELAY_MODE_ALL:
            hs_do_all_mode_delay(delay_count);
            break;

            
        case DELAY_MODE_RANDOM:
            hs_do_random_mode_delay(delay_count);
            break;

        default:
            break;
    }

    return;
}


LOCAL gint32 hs_fru_obj_foreach(OBJ_HANDLE object_handle, gpointer user_data)
{
    gint32 result;
    guint8 fru_id;
    GVariant *property_data = NULL;

    result = dfl_get_property_value(object_handle, "MaxSupportFru", &property_data);
    if (result != DFL_OK) {
        return RET_OK;
    }
    fru_id = g_variant_get_byte(property_data);
    g_variant_unref(property_data);

    
    if (g_max_fru_id < fru_id) {
        g_max_fru_id = fru_id;
    }
    
    result = dfl_bind_object_alias(object_handle, (guint32)fru_id);
    if (result != DFL_OK) {
        return RET_OK;
    }

    debug_log(DLOG_DEBUG, "CurSupportFru:%u,MaxSupportFru:%u.\r\n", fru_id, g_max_fru_id);

    return RET_OK;
}


LOCAL void hs_detect_max_fru_id(void)
{
    
    if (DFL_OK != dfl_foreach_object(CLASS_FRU_STATICS, hs_fru_obj_foreach, NULL, NULL)) {
        debug_log(DLOG_ERROR, "foreach FruStatics objects fail\r\n");
    }
    return;
}


gint32 hs_is_fru_managed(guchar fru_id)
{
    gint32 result;
    OBJ_HANDLE object_handle;
    GVariant *property_data = NULL;
    GSList *obj_list = NULL;
    GSList *node = NULL;
    guint8 is_managed = FALSE;
    guint8 get_fru_id;

    result = dfl_get_object_list(CLASS_FRU, &obj_list);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, result);
        return is_managed;
    }
    for (node = obj_list; node; node = node->next) {
        object_handle = (OBJ_HANDLE)node->data;

        result = dfl_get_property_value(object_handle, PROPERTY_FRU_ID, &property_data);
        if (result != DFL_OK) {
            g_slist_free(obj_list);
            debug_log(DLOG_ERROR, "dfl_get_property_value fail!(result=%d)\r\n", result);
            return is_managed;
        }

        get_fru_id = g_variant_get_byte(property_data);
        g_variant_unref(property_data);
        property_data = NULL;

        if (get_fru_id == fru_id) {
            result = dfl_get_property_value(object_handle, PROPERTY_FRU_IS_MANAGED, &property_data);
            if (result != DFL_OK) {
                is_managed = FALSE;
                debug_log(DLOG_ERROR, "get Fru%d object IsManaged fail!(result=%d)\r\n", fru_id, result);
            } else {
                is_managed = g_variant_get_byte(property_data);
                g_variant_unref(property_data);
            }
            break;
        }
    }
    g_slist_free(obj_list);

    return is_managed;
}


LOCAL void hs_restore_fru_hotswap_state(guchar fru_id)
{
    if (hs_is_fru_managed(fru_id) == TRUE) {
        
        if (hop_get_imana_reset_type() == SOFT_RESET) {
            
            if ((g_fru_hotswap[fru_id].hotswap.cur_hotswap_state != FRU_HS_STATE_M1) &&
                (g_fru_hotswap[fru_id].hotswap.cur_hotswap_state != FRU_HS_STATE_M4)) {
                
                if (g_fru_hotswap[fru_id].hotswap.cur_hotswap_state != FRU_HS_STATE_M7) {
                    g_printf("g_fru_hotswap[%d].hotswap.cur_hotswap_state=%d\r\n", fru_id,
                        g_fru_hotswap[fru_id].hotswap.cur_hotswap_state);
                    g_printf("g_fru_hotswap[%d].hotswap.last_hotswap_state=%d\r\n", fru_id,
                        g_fru_hotswap[fru_id].hotswap.last_hotswap_state);
                    g_fru_hotswap[fru_id].hotswap.cur_hotswap_state = g_fru_hotswap[fru_id].hotswap.last_hotswap_state;
                    
                    hs_update_cur_state(fru_id);
                    
                    hs_update_mix_state(fru_id);
                }
                
                if (pp_get_pwr_state(fru_id) == PAYLOAD_POWER_STATE_ON) {
                    
                    hse_activate_completed(fru_id);
                } else {
                    hse_deactivate_completed(fru_id);
                }
            } else {
                if (g_fru_hotswap[fru_id].hotswap.last_hotswap_state <= FRU_HS_STATE_M7) {
                    
                    g_do_after_route[g_fru_hotswap[fru_id].hotswap.last_hotswap_state](fru_id);
                }
            }
        }
    }
}


void hs_state_restore(void)
{
    guchar fru_id;
    guint32 fru_num;

    fru_num = hs_get_fru_num();

    
    for (fru_id = 0; fru_id < fru_num; fru_id++) {
        hs_restore_fru_hotswap_state(fru_id);
    }
}


guchar hs_get_activate_policy(guchar fruid, guchar *policy)
{
    if (!policy) {
        return COMP_CODE_UNKNOWN;
    }

    if ((fruid >= hs_get_fru_num()) || (FALSE == hs_is_fru_managed(fruid))) {
        return COMP_CODE_OUTOF_RANGE;
    }

    if (FALSE == hop_is_fru_present(fruid)) {
        
        return COMP_CODE_DES_UNAVAILABLE;
    }

    *policy = (guchar)((g_fru_hotswap[fruid].hotswap.deactivate_lockbit << 1) |
        g_fru_hotswap[fruid].hotswap.activate_lockbit);

    return COMP_CODE_SUCCESS;
}


guchar hs_get_cur_state(guchar fruid)
{
    if ((fruid >= hs_get_fru_num()) || (FALSE == hs_is_fru_managed(fruid))) {
        return 0;
    }

    return g_fru_hotswap[fruid].hotswap.cur_hotswap_state;
}


gint32 hs_get_stateless_auto_power_on(void)
{
    gint32 ret;
    OBJ_HANDLE obj_handle;
    GSList *obj_list = NULL;
    GVariant *property_data = NULL;
    guchar autopoweron;

    
    
    ret = dfl_get_object_list(CLASS_STATELESS, &obj_list);
    if (ret != DFL_OK || obj_list == NULL) {
        return TRUE;
    }

    obj_handle = (OBJ_HANDLE)g_slist_nth_data(obj_list, 0);
    g_slist_free(obj_list);

    ret = dfl_get_property_value(obj_handle, PROPERTY_STATELESS_AUTO_POWER_ON, &property_data);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "dfl_get_property_value %s fail!(result=%d)\r\n", PROPERTY_STATELESS_AUTO_POWER_ON, ret);
        return FALSE;
    }

    autopoweron = g_variant_get_byte(property_data);
    g_variant_unref(property_data);

    return (autopoweron == FALSE ? FALSE : TRUE);
}


gint32 hs_is_auto_management(void)
{
    gint32 result;
    OBJ_HANDLE object_handle;
    GVariant *property_data = NULL;
    guchar auto_management = TRUE;

    
    if (FALSE == hs_get_stateless_auto_power_on()) {
        return FALSE;
    }

    
    
    result = dfl_get_binded_object(CLASS_CHASSISPAYLOAD, 0x00, &object_handle);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "dfl_get_binded_object %s fail!(result=%d)\r\n", CLASS_CHASSISPAYLOAD, result);
        return auto_management;
    }

    result = dfl_get_property_value(object_handle, PROPERTY_PAYLOAD_PWRON_CTRL, &property_data);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "dfl_get_property_value %s fail!(result=%d)\r\n", PROPERTY_PAYLOAD_PWRON_CTRL, result);
        return auto_management;
    }
    auto_management = g_variant_get_byte(property_data);
    g_variant_unref(property_data);

    return auto_management == FALSE ? TRUE : FALSE;
}


guint32 hs_get_fru_num(void)
{
    return (g_max_fru_id + 1);
}


LOCAL gint32 hs_obj_init_foreach(OBJ_HANDLE object_handle, gpointer user_data)
{
    gint32 result;
    guint8 fru_id;
    GVariant *property_data = NULL;

    
    result = dfl_get_property_value(object_handle, PROPERTY_HS_FRUID, &property_data);
    if (result != DFL_OK) {
        return RET_OK;
    }
    fru_id = g_variant_get_byte(property_data);
    g_variant_unref(property_data);

    
    result = dfl_bind_object_alias(object_handle, (guint32)fru_id);
    if (result != DFL_OK) {
        return RET_OK;
    }
    debug_log(DLOG_DEBUG, "bind Hotswap object success\r\n");

    return RET_OK;
}


LOCAL void hs_obj_init(void)
{
    
    if (DFL_OK != dfl_foreach_object(CLASS_HOTSWAP, hs_obj_init_foreach, NULL, NULL)) {
        debug_log(DLOG_ERROR, "foreach Hotswap objects fail\r\n");
        return;
    }
}


LOCAL void hs_per_data_init(void)
{
    guchar fru_id;
    gchar key[HOTSWAP_TEMP_BUF_LEN] = {0};
    PER_S per_data;
    guint32 fru_num;
    gint32 ret_val;

    fru_num = hs_get_fru_num();

    
    for (fru_id = 0; fru_id < fru_num; fru_id++) {
        (void)memset_s(key, sizeof(key), 0, sizeof(key));
        hs_get_key(fru_id, (gchar *)key);
        per_data.key = key;
        per_data.mode = PER_MODE_RESET;
        per_data.data = (guint8 *)&g_fru_hotswap[fru_id].hotswap;
        per_data.len = sizeof(g_fru_hotswap[fru_id].hotswap);
        ret_val = per_init(&per_data, 1);
        if (ret_val != RET_OK) {
            debug_log(DLOG_ERROR, "FRU%dHSS persistance fail!\r\n", fru_id);
        }
        debug_log(DLOG_DEBUG, "g_fru_hotswap[%d].hotswap.cur_hotswap_state=%d", fru_id,
            g_fru_hotswap[fru_id].hotswap.cur_hotswap_state);
        debug_log(DLOG_DEBUG, "g_fru_hotswap[%d].hotswap.last_hotswap_state=%d", fru_id,
            g_fru_hotswap[fru_id].hotswap.last_hotswap_state);
    }

    debug_log(DLOG_DEBUG, "FRU%d cur_hotswap_state=%d last_hotswap_state=%d", CHASSIS_FRU_ID,
        g_fru_hotswap[CHASSIS_FRU_ID].hotswap.cur_hotswap_state,
        g_fru_hotswap[CHASSIS_FRU_ID].hotswap.last_hotswap_state);

    return;
}


LOCAL guchar hs_infra_init(void)
{
    guchar fru_id;
    guint32 fru_num;
    guint32 malloc_size;

    
    
    hs_detect_max_fru_id();

    fru_num = hs_get_fru_num();

    
    
    malloc_size = fru_num * sizeof(FRU_HOTSWAP_S);
    g_fru_hotswap = (FRU_HOTSWAP_S *)g_malloc0(malloc_size);
    
    if (g_fru_hotswap == NULL) {
        debug_log(DLOG_ERROR, "hs_infra_init: alloc fru_num:%u(Max:%d) g_fru_hotswap size:%u fail", fru_num,
            g_max_fru_id, malloc_size);
        return PAYLOAD_ERESOURCE;
    }

    
    
    
    for (fru_id = 0; fru_id < fru_num; fru_id++) {
        g_fru_hotswap[fru_id].hotswap.cur_hotswap_state = FRU_HS_STATE_M0;
        g_fru_hotswap[fru_id].hotswap.last_hotswap_state = FRU_HS_STATE_M0;
        g_fru_hotswap[fru_id].hotswap.hotswap_cause = FRU_HS_NORMAL_STATE_CHANGE;
    }

    
    hs_per_data_init();

    
    hs_obj_init();
    return PAYLOAD_SUCCESS;
}


void hs_process_init(void)
{
    guint32 fru_id;
    guint32 fru_num;

    fru_num = hs_get_fru_num();

    for (fru_id = 0; fru_id < fru_num; fru_id++) {
        
        if (TRUE == hs_is_fru_managed(fru_id)) {
            
            if (RET_OK != vos_queue_create(&(g_fru_hotswap[fru_id].hotswap_event_qid))) {
                debug_log(DLOG_ERROR, "hs_process_init: que create failed.\r\n");
                return;
            }
        }
    }
}


void hs_thread_init(void)
{
    TASKID tid;
    guint32 fru_id;
    guint32 fru_num;
    gchar taskName[4] = "HS"; 

    fru_num = hs_get_fru_num();

    for (fru_id = 0; fru_id < fru_num; fru_id++) {
        
        if (TRUE == hs_is_fru_managed(fru_id)) {
            
            
            taskName[3] = (gchar)('0' + fru_id);
            
            (void)vos_task_create(&tid, taskName, (TASK_ENTRY)hs_process_task, UINT32_TO_POINTER(fru_id), 2);
        }
    }
}


LOCAL void hs_checker_init(void)
{
    
    hop_pin_checker_init();
    hop_criteria_checker_init();
    hop_sem4_init();

    return;
}


guchar hs_init(void)
{
    
    if (PAYLOAD_SUCCESS != hs_infra_init()) {
        return PAYLOAD_FAIL;
    }

    return PAYLOAD_SUCCESS;
}


LOCAL gint32 hs_fru_obj_init_foreach(OBJ_HANDLE object_handle, gpointer user_data)
{
    gint32 result;
    guint8 fru_id;
    GVariant *property_data = NULL;

    
    result = dfl_get_property_value(object_handle, PROPERTY_FRU_ID, &property_data);
    if (result != DFL_OK) {
        return RET_OK;
    }
    fru_id = g_variant_get_byte(property_data);
    g_variant_unref(property_data);

    
    result = dfl_bind_object_alias(object_handle, (guint32)fru_id);
    if (result != DFL_OK) {
        return RET_OK;
    }
    debug_log(DLOG_DEBUG, "bind FRU object success\r\n");

    return RET_OK;
}


guchar hs_fru_init(void)
{
    
    if (DFL_OK != dfl_foreach_object(CLASS_FRU, hs_fru_obj_init_foreach, NULL, NULL)) {
        debug_log(DLOG_ERROR, "foreach Fru objects fail\r\n");
        return PAYLOAD_FAIL;
    }
    return PAYLOAD_SUCCESS;
}


void hs_start(void)
{
    
    hs_checker_init();
}


void update_hotswap_fix_flag(void)
{
    g_mutex_lock(&g_hotswap_fix_mutex);
    g_hotswap_flag = 1;
    g_mutex_unlock(&g_hotswap_fix_mutex);
}


LOCAL gint32 add_discrete_eventobj_to_list(OBJ_HANDLE object_handle, GList **p_glist_discreteEvent)
{
#define MAX_NAME_LEN 32
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    const gchar *sensor_name = NULL;
    gchar sensor_name_temp[MAX_NAME_LEN] = {0};

    ret = dfl_get_object_list(CLASS_DISCERETE_EVENT, &obj_list);
    if (ret != DFL_OK || 0 == g_slist_length(obj_list)) {
        debug_log(DLOG_ERROR, "%s : dfl_get_object_list error, classname = %s", __FUNCTION__, CLASS_DISCERETE_EVENT);
        return RET_ERR;
    }

    sensor_name = dfl_get_object_name(object_handle);

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        (void)dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_DIS_EVENT_SENSOR_NAME,
            sensor_name_temp, sizeof(sensor_name_temp));
        if (RET_OK == strcmp(sensor_name, sensor_name_temp)) {
            *p_glist_discreteEvent =
                g_list_append(*p_glist_discreteEvent, g_variant_new_uint64((OBJ_HANDLE)obj_node->data));
        }
        if (*p_glist_discreteEvent == NULL) {
            debug_log(DLOG_ERROR, "%s : add eventobj(%s) to list fail", __FUNCTION__,
                dfl_get_object_name((OBJ_HANDLE)obj_node->data));
        }
    }
    g_slist_free(obj_list);
    if (*p_glist_discreteEvent == NULL) {
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 add_eventobj_by_sensorobj(GList *glist_discreteSensor, GList **p_glist_discreteEvent)
{
    gint32 ret = RET_ERR;
    GList *node = NULL;
    OBJ_HANDLE sensor_handle;
    for (node = glist_discreteSensor; node; node = g_list_next(node)) {
        sensor_handle = (OBJ_HANDLE)g_variant_get_uint64((GVariant *)node->data);
        ret = add_discrete_eventobj_to_list(sensor_handle, p_glist_discreteEvent);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s : add eventobj by sensorobj(%s) to list fail", __FUNCTION__,
                dfl_get_object_name(sensor_handle));
        }
    }
    if (ret != RET_OK) {
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 add_sensorobj_to_list(GList **p_glist_discreteSensor, guint8 sensor_type)
{
    gint32 ret;
    guint8 sensor_type_temp = 0;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;

    ret = dfl_get_object_list(CLASS_DISCERETE_SENSOR, &obj_list);
    if (ret != DFL_OK || g_slist_length(obj_list) == 0) {
        return ret;
    }

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_DIS_SENSOR_SENSOR_TYPE,
            &sensor_type_temp);
        if (sensor_type == sensor_type_temp) { 
            *p_glist_discreteSensor =
                g_list_append(*p_glist_discreteSensor, g_variant_new_uint64((OBJ_HANDLE)obj_node->data));
        }
        if (*p_glist_discreteSensor == NULL) {
            debug_log(DLOG_ERROR, "%s : add sensorobj(%s) to list fail", __FUNCTION__,
                dfl_get_object_name((OBJ_HANDLE)obj_node->data));
        }
    }
    g_slist_free(obj_list);
    if (*p_glist_discreteSensor == NULL) {
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 update_hotswap_fix_list(GList **p_glist_discreteSensor, GList **p_glist_discreteEvent)
{
    gint32 ret;
    if (*p_glist_discreteSensor != NULL) {
        g_list_free_full(*p_glist_discreteSensor, (GDestroyNotify)g_variant_unref);
        *p_glist_discreteSensor = NULL;
    }
    if (*p_glist_discreteEvent != NULL) {
        g_list_free_full(*p_glist_discreteEvent, (GDestroyNotify)g_variant_unref);
        *p_glist_discreteEvent = NULL;
    }
    
    ret = add_sensorobj_to_list(p_glist_discreteSensor, SENSOR_TYPE_FRUHOTSWAP);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : add_sensorobj_to_list fail\r\n", __FUNCTION__);
        return ret;
    }

    
    ret = add_eventobj_by_sensorobj(*p_glist_discreteSensor, p_glist_discreteEvent);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : add_eventobj_by_sensorobj fail\r\n", __FUNCTION__);
        return ret;
    }

    return RET_OK;
}


LOCAL gint32 check_hotswap_status(OBJ_HANDLE sensor_handle, OBJ_HANDLE event_handle)
{
    gint8 i;
    guint16 assert_status = 0;
    guint8 cur_hs = 0;
    guint8 temp = 0;
    guint16 status;
    gint32 ret;

    for (i = 0; i < CHECK_NUM; i++) {
        
        ret = dal_get_property_value_uint16(sensor_handle, PROPERTY_DIS_SENSOR_ASSERT_STATUS, &assert_status);
        if (ret != RET_OK) {
            // 获取失败直接跳出循环,返回正常不做修正
            break;
        }
        
        ret = dal_get_property_value_byte(event_handle, PROPERTY_DIS_EVENT_DATA0_REF, &cur_hs);
        if (ret != RET_OK) {
            
            break;
        }

        if (i == 0) {
            temp = cur_hs;
        } else {
            if (cur_hs != temp) {
                
                break;
            }
        }

        status = 1 << cur_hs;
        if (0 != (assert_status & status)) {
            
            break;
        }

        debug_log(DLOG_ERROR,
            "M state may be error(%d): CurrentHotswapState[%u] AssertStatus[0x%04x] sensor_handle=%s, event_handle=%s.",
            i, cur_hs, assert_status, dfl_get_object_name(sensor_handle), dfl_get_object_name(event_handle));
        
        vos_task_delay(1000);
    }

    if (i >= CHECK_NUM) {
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL void change_hotswap_status(OBJ_HANDLE event_handle)
{
    guint8 fru_id = 0xFF;
    OBJ_HANDLE hotswap_handle = 0;
    guint8 cur_hs = 0;
    guint8 last_hs = 0;
    guint32 mix_status;
    guint8 temp = 0;
    guint32 ret;
    gchar referencd_object_name[MAX_NAME_SIZE] = {0};
    gchar referencd_property[MAX_NAME_SIZE] = {0};

    
    ret = dfl_get_referenced_property(event_handle, PROPERTY_DIS_EVENT_DATA2_REF, referencd_object_name,
        referencd_property, MAX_NAME_SIZE, MAX_NAME_SIZE);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s : dfl_get_referenced_object by %s failed, ret = %d.", __FUNCTION__,
            dfl_get_object_name(hotswap_handle), ret);
        return;
    }
    (void)dfl_get_object_handle(referencd_object_name, &hotswap_handle);
    (void)dal_get_property_value_byte(hotswap_handle, PROPERTY_HS_FRUID, &fru_id);
    debug_log(DLOG_DEBUG, "%s: fru_id=%u event_handle = %s, hotswap_handle = %s.", __FUNCTION__, fru_id,
        dfl_get_object_name(event_handle), dfl_get_object_name(hotswap_handle));

    (void)dal_get_property_value_byte(hotswap_handle, PROPERTY_HS_LAST_STATE, &last_hs);
    (void)dal_get_property_value_byte(hotswap_handle, PROPERTY_HS_CRU_STATE, &cur_hs);

    
    
    mix_status = MAKE_DWORD(0x01, fru_id, last_hs, last_hs);
    if (last_hs == cur_hs) {
        
        temp = (cur_hs == 0) ? MAX_HS_STATE : (cur_hs - 1);
        debug_log(DLOG_ERROR, "last==cur[%u] fix last to [%u].\r\n", cur_hs, temp);
        mix_status = MAKE_DWORD(0x01, fru_id, temp, temp);
    }
    ret = dal_set_property_value_uint32(hotswap_handle, PROPERTY_HS_MIX_STATE, mix_status, DF_NONE);
    if ((ret != RET_OK)) {
        debug_log(DLOG_ERROR, "set [%s.MixHotswapState] to [0x%04x] fail, ret=%d.", dfl_get_object_name(hotswap_handle),
            mix_status, ret);
    }
    debug_log(DLOG_ERROR, "%s set %s.MixHotswapState event_handle=%s FRU%u mix_status=0x%X,0x%X M%u->M%u, ret=%u.",
        __FUNCTION__, dfl_get_object_name(hotswap_handle), dfl_get_object_name(event_handle), fru_id, temp, mix_status,
        last_hs, cur_hs, ret);
    
    vos_task_delay(500);

    
    mix_status = MAKE_DWORD(0x01, fru_id, last_hs, cur_hs);
    ret = dal_set_property_value_uint32(hotswap_handle, PROPERTY_HS_MIX_STATE, mix_status, DF_NONE);
    if ((ret != RET_OK)) {
        debug_log(DLOG_ERROR, "set [%s.MixHotswapState] to [0x%04x] fail, ret=%d.", dfl_get_object_name(hotswap_handle),
            mix_status, ret);
    }

    return;
}


LOCAL void fix_hotswap_status(GList *glist_discreteSensor, GList *glist_discreteEvent)
{
    OBJ_HANDLE sensor_handle;
    OBJ_HANDLE event_handle;
    GList *sensor_obj_node = NULL;
    GList *event_obj_node = NULL;
    gint32 check_result;
    const gchar *sensor_name = NULL;
    gchar sensor_name_temp[MAX_NAME_SIZE] = {0};
    OBJ_HANDLE obj_handle_temp;
    gchar classname_tmep[MAX_NAME_SIZE] = {0};
    gint32 ret;
    gchar referencd_object_name[MAX_NAME_SIZE] = {0};
    gchar referencd_object_property[MAX_NAME_SIZE] = {0};

    if (glist_discreteSensor == NULL || glist_discreteEvent == NULL) {
        return;
    }

    
    for (event_obj_node = glist_discreteEvent; event_obj_node; event_obj_node = event_obj_node->next) {
        
        event_handle = (OBJ_HANDLE)g_variant_get_uint64((GVariant *)event_obj_node->data);
        ret = dfl_get_referenced_property(event_handle, PROPERTY_DIS_EVENT_DATA0_REF, referencd_object_name,
            referencd_object_property, MAX_NAME_SIZE, MAX_NAME_SIZE);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "dfl_get_referenced_object by %s failed, ret = %d.",
                dfl_get_object_name(event_handle), ret);
            continue;
        }
        (void)dfl_get_object_handle(referencd_object_name, &obj_handle_temp);
        ret = dfl_get_class_name(obj_handle_temp, classname_tmep, MAX_NAME_SIZE);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "dfl_get_class_name by %s failed, ret = %d.", dfl_get_object_name(obj_handle_temp),
                ret);
            continue;
        }
        if (RET_OK != strcmp(classname_tmep, CLASS_HOTSWAP)) {
            continue;
        }
        
        (void)dal_get_property_value_string(event_handle, PROPERTY_DIS_EVENT_SENSOR_NAME, sensor_name_temp,
            sizeof(sensor_name_temp));
        for (sensor_obj_node = glist_discreteSensor; sensor_obj_node; sensor_obj_node = sensor_obj_node->next) {
            
            sensor_handle = (OBJ_HANDLE)g_variant_get_uint64((GVariant *)sensor_obj_node->data);
            sensor_name = dfl_get_object_name(sensor_handle);
            
            if (strcmp(sensor_name, sensor_name_temp) != RET_OK) {
                continue;
            }
            
            check_result = check_hotswap_status(sensor_handle, event_handle);
            if (check_result != RET_OK) {
                
                (void)change_hotswap_status(event_handle);
            }
        }
    }
    return;
}


void osca_hotswap_fix_task(void)
{
    GList *glist_discreteSensor = NULL;
    GList *glist_discreteEvent = NULL;

    (void)prctl(PR_SET_NAME, (gulong)"OscaHotswapFix");

    for (;;) {
        
        vos_task_delay(5 * 1000);
        
        if (g_hotswap_flag == 1) { 
            g_mutex_lock(&g_hotswap_fix_mutex);
            g_hotswap_flag = 0;
            g_mutex_unlock(&g_hotswap_fix_mutex);
            
            (void)update_hotswap_fix_list(&glist_discreteSensor, &glist_discreteEvent);
        }

        if (glist_discreteSensor == NULL) {
            debug_log(DLOG_DEBUG, "%s : glist_discreteSensor == NULL.\r\n", __FUNCTION__);
            continue;
        }
        if (glist_discreteEvent == NULL) {
            debug_log(DLOG_DEBUG, "%s : glist_discreteEvent == NULL.\r\n", __FUNCTION__);
            continue;
        }

        
        (void)fix_hotswap_status(glist_discreteSensor, glist_discreteEvent);
    }
}