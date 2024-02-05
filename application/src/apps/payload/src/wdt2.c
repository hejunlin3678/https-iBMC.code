
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include "pme/common/mscm_vos.h"
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "payload_hop.h"
#include "payload_ipmi.h"
#include "payload_pwr.h"
#include "payload_hs.h"
#include "payload_com.h"
#include "wdt2.h"

static WATCHDOG2_S g_watchdog = { 0 };
static gulong g_watchdog_tick = 0;

static WATCHDOG2_KICKING_INFO_S g_wdt_kicking_info = { 0 };
static TASKID g_watchdog_task;
static guint8 g_ipc_debug_flag = 0; // 仅仅用于IPC通道是否正常的调试

LOCAL void wdt_count_down_task(void);


WATCHDOG2_KICKING_INFO_S get_wdt_kicking_info(void)
{
    return g_wdt_kicking_info;
}

gulong get_watchdog_tick(void)
{
    return g_watchdog_tick;
}

WATCHDOG2_S get_watchdog_info(void)
{
    return g_watchdog;
}

gboolean is_watchdog_running(void)
{
    if (g_watchdog.running == 0) {
        return false;
    }
    return true;
}


const gchar *wdt_timer_use_to_str(guchar timer_use)
{
    const gchar *timer_use_name[] = {"Reserved", "BIOS FRB2", "BIOS/POST", "OS Load", "SMS/OS", "OEM"};

    if (timer_use >= ARRAY_SIZE(timer_use_name)) {
        return "Unknown";
    }

    return timer_use_name[timer_use];
}


const gchar *wdt_timeout_action_to_str(guchar timeout_action)
{
    const gchar *timeout_action_name[] = {"No action", "Hard Reset", "Power Down", "Power Cycle"};

    if (timeout_action >= ARRAY_SIZE(timeout_action_name)) {
        return "Unknown";
    }

    return timeout_action_name[timeout_action];
}


gint32 wdt_hs_evt_process(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name, GVariant *property_value)
{
    return RET_OK;
}

void wdt_task_init(void)
{
    g_watchdog_task = 0;
}


void wdt_kicking_info_init(void)
{
    g_wdt_kicking_info.total_cnt = 0;
    g_wdt_kicking_info.timestamp = 0;
}


LOCAL inline void wdt_kicking_info_update(void)
{
    g_wdt_kicking_info.total_cnt++;
    g_wdt_kicking_info.timestamp = time(NULL);
}


LOCAL void wdt_save(void)
{
    
    per_save((guint8 *)&g_watchdog);
}


LOCAL void wdt_per_data_init(void)
{
    PER_S per_data;
    gint32 ret_val;

    
    per_data.key = WATCH_DOG2;
    per_data.mode = PER_MODE_RESET;
    per_data.data = (guint8 *)&g_watchdog;
    per_data.len = sizeof(g_watchdog);
    ret_val = per_init(&per_data, 1);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, "Watchdog2 persistance fail!\r\n");
    } else {
        g_watchdog_tick = vos_tick_get();
    }
}


void wdt_restore(void)
{
    
    wdt_per_data_init();
}


void wdt_create_timer(void)
{
    if (!g_watchdog_task) {
        (void)vos_task_create(&g_watchdog_task, "watchdog", (TASK_ENTRY)wdt_count_down_task, 0, DEFAULT_PRIORITY);
    }
}


LOCAL gint32 wdt_obj_init_foreach(OBJ_HANDLE object_handle, gpointer user_data)
{
    // 更新NotLog,因为在框架里会调用rpc，所以要将变量的副本传递给框架
    if (dal_set_property_value_byte(object_handle, PROPERTY_WD_NOTLOG, g_watchdog.not_log, DF_NONE) != RET_OK) {
        return RET_OK;
    }
    // 更新TimerUse
    if (dal_set_property_value_byte(object_handle, PROPERTY_WD_TIMER_USE, g_watchdog.timer_use, DF_NONE) != RET_OK) {
        return RET_OK;
    }
    // 更新Running
    if (dal_set_property_value_byte(object_handle, PROPERTY_WD_RUNNING, g_watchdog.running, DF_NONE) != RET_OK) {
        return RET_OK;
    }
    // 更新PreTmoutIntF
    if (dal_set_property_value_byte(object_handle, PROPERTY_WD_PRE_TM_INTR_F, g_watchdog.pre_tmout_int_f, DF_NONE) !=
        RET_OK) {
        return RET_OK;
    }
    // 更新PreTmoutInt
    if (dal_set_property_value_byte(object_handle, PROPERTY_WD_PRE_TM_INTR, g_watchdog.pre_tmout_int, DF_NONE) !=
        RET_OK) {
        return RET_OK;
    }
    // 更新PreTmoutIntEn
    if (dal_set_property_value_byte(object_handle, PROPERTY_WD_PRE_TM_INTR_EN, g_watchdog.pre_tmout_int_en, DF_NONE) !=
        RET_OK) {
        return RET_OK;
    }
    // 更新TmoutAction
    if (dal_set_property_value_byte(object_handle, PROPERTY_WD_TM_ACTION, g_watchdog.tmout_action, DF_NONE) != RET_OK) {
        return RET_OK;
    }
    // 给Watchdog2对象绑定一个别名，方便后续更新属性时的操作
    if (dfl_bind_object_alias(object_handle, (guint32)WDT2_ALIAS) != DFL_OK) {
        return RET_OK;
    }

    // 初始化用于检查IPC通道是否正常的标志值
    if (dal_set_property_value_byte(object_handle, PROPERTY_IS_IPC_COMM_NORMAL, 0x01, DF_NONE) != RET_OK) {
        return RET_OK;
    }
    
    return RET_OK;
}


void wdt_obj_init(void)
{
    
    if (dfl_foreach_object(CLASS_WATCHDOG, wdt_obj_init_foreach, NULL, NULL) != DFL_OK) {
        debug_log(DLOG_ERROR, "foreach Watchdog2 objects fail\r\n");
    }

    return;
}


LOCAL void wdt_update_timer_expire_flag(PWATCHDOG2_S p)
{
    switch (p->timer_use) {
        case WATCHDOG_TIMER_BIOSFRB2:
            p->timer_expir_flag_bios_frb2 = 1;
            break;
        case WATCHDOG_TIMER_BIOSPOST:
            p->timer_expir_flag_bios_post = 1;
            break;
        case WATCHDOG_TIMER_OSLOAD:
            p->timer_expir_flag_osload = 1;
            break;
        case WATCHDOG_TIMER_SMSOS:
            p->timer_expir_flag_sms_os = 1;
            break;
        case WATCHDOG_TIMER_OEM:
            p->timer_expir_flag_oem = 1;
            break;
        default:
            break;
    }
}


LOCAL gint32 wdt_object_handle(OBJ_HANDLE *object_handle)
{
    gint32 result;
    result = dfl_get_binded_object(CLASS_WATCHDOG, WDT2_ALIAS, object_handle);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "get Watchdog2 object fail!\r\n");
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 wdt_notlog_handle(OBJ_HANDLE object_handle, PWATCHDOG2_S p)
{
    gint32 result;
    guint8 old_data;
    guint8 temp_data;
    GVariant *property_data = NULL;
    result = dfl_get_property_value(object_handle, PROPERTY_WD_NOTLOG, &property_data);
    if (result != DFL_OK) {
        return result;
    }
    old_data = g_variant_get_byte(property_data);
    g_variant_unref(property_data);

    if (old_data != p->not_log) {
        // 更新NotLog
        temp_data = p->not_log;
        property_data = g_variant_new_byte(temp_data);
        result = dfl_set_property_value(object_handle, PROPERTY_WD_NOTLOG, property_data, DF_NONE);
        g_variant_unref(property_data);
        if (result != DFL_OK) {
            return result;
        }
    }
    return RET_OK;
}

LOCAL gint32 wdt_timer_use_handle(OBJ_HANDLE object_handle, PWATCHDOG2_S p)
{
    gint32 result;
    guint8 old_data;
    guint8 temp_data;
    GVariant *property_data = NULL;
    
    const gchar* timer_use_int2str[6] = {
        NULL,
        WATCHDOG_TIMER_BIOSFRB2_STR,
        WATCHDOG_TIMER_BIOSPOST_STR,
        WATCHDOG_TIMER_OSLOAD_STR,
        WATCHDOG_TIMER_SMSOS_STR,
        WATCHDOG_TIMER_OEM_STR
    };
    const gchar *timer_use_str = NULL;
    result = dfl_get_property_value(object_handle, PROPERTY_WD_TIMER_USE, &property_data);
    if (result != DFL_OK) {
        return result;
    }
    old_data = g_variant_get_byte(property_data);
    g_variant_unref(property_data);

    if (old_data != p->timer_use) {
        // 更新TimerUse
        temp_data = p->timer_use;
        property_data = g_variant_new_byte(temp_data);
        result = dfl_set_property_value(object_handle, PROPERTY_WD_TIMER_USE, property_data, DF_NONE);
        g_variant_unref(property_data);
        if (result != DFL_OK) {
            return result;
        }
    }

    if ((p->timer_use > WATCHDOG_TIMER_OEM) || (p->timer_use < WATCHDOG_TIMER_BIOSFRB2)) {
        timer_use_str = "";
    } else {
        timer_use_str = timer_use_int2str[p->timer_use];
    }
    result = dal_set_property_value_string(object_handle, PROPERTY_WD_TIMER_USE_STR, timer_use_str, DF_NONE);
    if (result != RET_OK) {
        debug_log(DLOG_ERROR, "%s:set TimerUseStr fail,result=%d", __FUNCTION__, result);
    }
    return RET_OK;
}

LOCAL gint32 wdt_running_handle(OBJ_HANDLE object_handle, PWATCHDOG2_S p)
{
    gint32 result;
    guint8 old_data;
    guint8 temp_data;
    GVariant *property_data = NULL;
    result = dfl_get_property_value(object_handle, PROPERTY_WD_RUNNING, &property_data);
    if (result != DFL_OK) {
        return result;
    }
    old_data = g_variant_get_byte(property_data);
    g_variant_unref(property_data);

    if (old_data != p->running) {
        // 更新Running
        temp_data = p->running;
        property_data = g_variant_new_byte(temp_data);
        result = dfl_set_property_value(object_handle, PROPERTY_WD_RUNNING, property_data, DF_NONE);
        g_variant_unref(property_data);
        if (result != DFL_OK) {
            return result;
        }
    }
    return RET_OK;
}


LOCAL gint32 wdt_pre_time_out_intf_handle(OBJ_HANDLE object_handle, PWATCHDOG2_S p)
{
    gint32 result;
    guint8 old_data;
    guint8 temp_data;
    GVariant *property_data = NULL;
    result = dfl_get_property_value(object_handle, PROPERTY_WD_PRE_TM_INTR_F, &property_data);
    if (result != DFL_OK) {
        return result;
    }
    old_data = g_variant_get_byte(property_data);
    g_variant_unref(property_data);

    if (old_data != p->pre_tmout_int_f) {
        
        temp_data = p->pre_tmout_int_f;
        property_data = g_variant_new_byte(temp_data);
        result = dfl_set_property_value(object_handle, PROPERTY_WD_PRE_TM_INTR_F, property_data, DF_NONE);
        g_variant_unref(property_data);
        if (result != DFL_OK) {
            return result;
        }
    }
    return RET_OK;
}


LOCAL gint32 wdt_pre_time_out_int_handle(OBJ_HANDLE object_handle, PWATCHDOG2_S p)
{
    GVariant *property_data = NULL;
    gint32 result = dfl_get_property_value(object_handle, PROPERTY_WD_PRE_TM_INTR, &property_data);
    if (result != DFL_OK) {
        return result;
    }
    guint8 old_data = g_variant_get_byte(property_data);
    g_variant_unref(property_data);

    if (old_data != p->pre_tmout_int) {
        
        guint8 temp_data = p->pre_tmout_int;
        property_data = g_variant_new_byte(temp_data);
        result = dfl_set_property_value(object_handle, PROPERTY_WD_PRE_TM_INTR, property_data, DF_NONE);
        g_variant_unref(property_data);
        if (result != DFL_OK) {
            return result;
        }
    }
    return RET_OK;
}

LOCAL gint32 wdt_pre_time_out_int_en_handle(OBJ_HANDLE object_handle, PWATCHDOG2_S p)
{
    GVariant *property_data = NULL;
    gint32 result = dfl_get_property_value(object_handle, PROPERTY_WD_PRE_TM_INTR_EN, &property_data);
    if (result != DFL_OK) {
        return result;
    }
    guint8 old_data = g_variant_get_byte(property_data);
    g_variant_unref(property_data);

    if (old_data != p->pre_tmout_int_en) {
        
        guint8 temp_data = p->pre_tmout_int_en;
        property_data = g_variant_new_byte(temp_data);
        result = dfl_set_property_value(object_handle, PROPERTY_WD_PRE_TM_INTR_EN, property_data, DF_NONE);
        g_variant_unref(property_data);
        if (result != DFL_OK) {
            return result;
        }
    }
    return RET_OK;
}

LOCAL gint32 wdt_pre_time_out_action_handle(OBJ_HANDLE object_handle, PWATCHDOG2_S p)
{
    GVariant *property_data = NULL;
    gint32 result = dfl_get_property_value(object_handle, PROPERTY_WD_TM_ACTION, &property_data);
    if (result != DFL_OK) {
        return result;
    }
    guint8 old_data = g_variant_get_byte(property_data);
    g_variant_unref(property_data);

    if (old_data != p->tmout_action) {
        
        guint8 temp_data = p->tmout_action;
        property_data = g_variant_new_byte(temp_data);
        result = dfl_set_property_value(object_handle, PROPERTY_WD_TM_ACTION, property_data, DF_NONE);
        g_variant_unref(property_data);
        if (result != DFL_OK) {
            return result;
        }
    }
    return RET_OK;
}

LOCAL gint32 wdt_oem_expir_flag_handle(OBJ_HANDLE object_handle, PWATCHDOG2_S p)
{
    GVariant *property_data = NULL;
    gint32 result = dfl_get_property_value(object_handle, PROPERTY_WD_OEM_EXP_F, &property_data);
    if (result != DFL_OK) {
        return result;
    }
    guint8 old_data = g_variant_get_byte(property_data);
    g_variant_unref(property_data);

    if (old_data != p->timer_expir_flag_oem) {
        
        guint8 temp_data = p->timer_expir_flag_oem;
        property_data = g_variant_new_byte(temp_data);
        result = dfl_set_property_value(object_handle, PROPERTY_WD_OEM_EXP_F, property_data, DF_NONE);
        g_variant_unref(property_data);
        if (result != DFL_OK) {
            return result;
        }
    }
    return RET_OK;
}

LOCAL gint32 wdt_sms_expir_flag_handle(OBJ_HANDLE object_handle, PWATCHDOG2_S p)
{
    GVariant *property_data = NULL;
    gint32 result = dfl_get_property_value(object_handle, PROPERTY_WD_SMS_EXP_F, &property_data);
    if (result != DFL_OK) {
        return result;
    }
    guint8 old_data = g_variant_get_byte(property_data);
    g_variant_unref(property_data);

    if (old_data != p->timer_expir_flag_sms_os) {
        
        guint8 temp_data = p->timer_expir_flag_sms_os;
        property_data = g_variant_new_byte(temp_data);
        result = dfl_set_property_value(object_handle, PROPERTY_WD_SMS_EXP_F, property_data, DF_NONE);
        g_variant_unref(property_data);
        if (!(result == DFL_OK)) {
            return result;
        }
    }
    return RET_OK;
}

LOCAL gint32 wdt_os_load_expir_flag_handle(OBJ_HANDLE object_handle, PWATCHDOG2_S p)
{
    GVariant *property_data = NULL;
    gint32 result = dfl_get_property_value(object_handle, PROPERTY_WD_OSLOAD_EXP_F, &property_data);
    if (result != DFL_OK) {
        return result;
    }
    guint8 old_data = g_variant_get_byte(property_data);
    g_variant_unref(property_data);

    if (old_data != p->timer_expir_flag_osload) {
        
        guint8 temp_data = p->timer_expir_flag_osload;
        property_data = g_variant_new_byte(temp_data);
        result = dfl_set_property_value(object_handle, PROPERTY_WD_OSLOAD_EXP_F, property_data, DF_NONE);
        g_variant_unref(property_data);
        if (result != DFL_OK) {
            return result;
        }
    }
    return RET_OK;
}

LOCAL gint32 wdt_post_expir_flag_handle(OBJ_HANDLE object_handle, PWATCHDOG2_S p)
{
    GVariant *property_data = NULL;
    gint32 result = dfl_get_property_value(object_handle, PROPERTY_WD_POST_EXP_F, &property_data);
    if (result != DFL_OK) {
        return result;
    }
    guint8 old_data = g_variant_get_byte(property_data);
    g_variant_unref(property_data);

    if (old_data != p->timer_expir_flag_bios_post) {
        
        guint8 temp_data = p->timer_expir_flag_bios_post;
        property_data = g_variant_new_byte(temp_data);
        result = dfl_set_property_value(object_handle, PROPERTY_WD_POST_EXP_F, property_data, DF_NONE);
        g_variant_unref(property_data);
        if (!(result == DFL_OK)) {
            return result;
        }
    }
    return RET_OK;
}


LOCAL gint32 wdt_frb_expir_flag_handle(OBJ_HANDLE object_handle, PWATCHDOG2_S p)
{
    GVariant *property_data = NULL;
    gint32 result = dfl_get_property_value(object_handle, PROPERTY_WD_FRB_EXP_F, &property_data);
    if (result != DFL_OK) {
        return result;
    }
    guint8 old_data = g_variant_get_byte(property_data);
    g_variant_unref(property_data);

    if (old_data != p->timer_expir_flag_bios_frb2) {
        
        guint8 temp_data = p->timer_expir_flag_bios_frb2;
        property_data = g_variant_new_byte(temp_data);
        result = dfl_set_property_value(object_handle, PROPERTY_WD_FRB_EXP_F, property_data, DF_NONE);
        g_variant_unref(property_data);
        if (result != DFL_OK) {
            return result;
        }
    }
    return RET_OK;
}


LOCAL gint32 wdt_update_timer_object(PWATCHDOG2_S p)
{
    OBJ_HANDLE object_handle = 0;
    
    if (wdt_object_handle(&object_handle) != RET_OK) {
        return RET_ERR;
    }
    
    if (wdt_notlog_handle(object_handle, p) != RET_OK) {
        return RET_ERR;
    }
    
    if (wdt_timer_use_handle(object_handle, p) != RET_OK) {
        return RET_ERR;
    }
    
    if (wdt_running_handle(object_handle, p) != RET_OK) {
        return RET_ERR;
    }
    
    if (wdt_pre_time_out_intf_handle(object_handle, p) != RET_OK) {
        return RET_ERR;
    }
    
    if (wdt_pre_time_out_int_handle(object_handle, p) != RET_OK) {
        return RET_ERR;
    }
    
    if (wdt_pre_time_out_int_en_handle(object_handle, p) != RET_OK) {
        return RET_ERR;
    }
    
    if (wdt_pre_time_out_action_handle(object_handle, p) != RET_OK) {
        return RET_ERR;
    }
    
    if (wdt_oem_expir_flag_handle(object_handle, p) != RET_OK) {
        return RET_ERR;
    }
    
    if (wdt_sms_expir_flag_handle(object_handle, p) != RET_OK) {
        return RET_ERR;
    }
    
    if (wdt_os_load_expir_flag_handle(object_handle, p) != RET_OK) {
        return RET_ERR;
    }
    
    if (wdt_post_expir_flag_handle(object_handle, p) != RET_OK) {
        return RET_ERR;
    }
    
    if (wdt_frb_expir_flag_handle(object_handle, p) != RET_OK) {
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL void wdt_update_restart_cause(PWATCHDOG2_S p)
{
    if ((p->tmout_action == WATCHDOG_TIMEOUT_RESET) || (p->tmout_action == WATCHDOG_TIMEOUT_PWRCYCLE)) {
        pp_set_restart_action(SYS_CHAN_NUM, RESTART_CAUSE_WATCHDOGEXP);
        pp_set_restart_cause();
    }
}


LOCAL void wdt_do_timeout_action(PWATCHDOG2_S p)
{
    guchar comp_code;

    switch (p->tmout_action) {
        case WATCHDOG_TIMEOUT_NOACTION:
            g_printf("wdt timeout with no action\r\n");
            break;

        case WATCHDOG_TIMEOUT_RESET:
            // 控制系统复位
            (void)hop_reset(CHASSIS_FRU_ID);
            g_printf("wdt timeout,do cold reset success\r\n");
            debug_log(DLOG_ERROR, "wdt timeout,do cold reset success\r\n");

            break;

        case WATCHDOG_TIMEOUT_PWROFF:
            // 取消已经开启的安全重启或POWER循环
            pp_set_cycle_progress_state(CHASSIS_FRU_ID, FALSE);
            pp_set_cycle_interval(CHASSIS_FRU_ID, 0);
            pp_save_cycle_param();
            // 控制系统下电
            comp_code = pp_fru_pwr_ctrl(CHASSIS_FRU_ID, POWER_OFF_FORCE);
            if (comp_code == COMP_CODE_SUCCESS) {
                g_printf("wdt timeout,do power off success\r\n");
                debug_log(DLOG_ERROR, "wdt timeout, do power off success\r\n");
            } else {
                maintenance_log_v2(MLOG_ERROR, FC_BMC_WDT_PWR_OFF_FAIL, "Wdt timeout, do power off method fail.\r\n");
            }
            break;

        case WATCHDOG_TIMEOUT_PWRCYCLE:
            // 控制系统power cycle
            comp_code = pp_chassis_pwr_cycle_from_wd(SRC_CHAN_NUM_RPC);
            if (comp_code == COMP_CODE_SUCCESS) {
                g_printf("wdt timeout,do power cycle success\r\n");
                debug_log(DLOG_ERROR, "wdt timeout, do power cycle success\r\n");
            } else {
                maintenance_log_v2(MLOG_ERROR, FC_BMC_WDT_PWR_CYCLE_FAIL,
                    "Wdt timeout, do power cycle method fail.\r\n");
            }
            break;
        default:
            break;
    }
}


LOCAL void wdt_set_tm_flag(guint8 tm_flag)
{
    gint32 result;
    OBJ_HANDLE object_handle = 0;
    GVariant *property_data = NULL;
    guint32 temp_data;
    guint32 new_flag;
    guint8 timer_use;
    guint8 timer_action;

    result = dfl_get_object_handle(OBJECT_WATCHDOG, &object_handle);
    if (result == DFL_OK) {
        
        result = dfl_get_property_value(object_handle, PROPERTY_WD_TIMER_USE, &property_data);
        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "dfl_get_property_value %s fail!(Ret=%d)\r\n", PROPERTY_WD_TIMER_USE, result);
            return;
        }
        timer_use = g_variant_get_byte(property_data);
        g_variant_unref(property_data);
        property_data = NULL;

        
        result = dfl_get_property_value(object_handle, PROPERTY_WD_TM_ACTION, &property_data);
        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "dfl_get_property_value %s fail!(Ret=%d)\r\n", PROPERTY_WD_TM_ACTION, result);
            return;
        }
        timer_action = g_variant_get_byte(property_data);
        g_variant_unref(property_data);
        property_data = NULL;

        
        new_flag = MAKE_DWORD(tm_flag, 0xff, timer_use, timer_action);

        
        result = dfl_get_property_value(object_handle, PROPERTY_WD_TM_FLAG, &property_data);
        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "dfl_get_property_value %s fail!(Ret=%d)\r\n", PROPERTY_WD_TM_FLAG, result);
            return;
        }

        temp_data = g_variant_get_uint32(property_data);
        g_variant_unref(property_data);
        property_data = NULL;

        // 更新看门狗超时状态
        if (temp_data != new_flag) {
            property_data = g_variant_new_uint32(new_flag);
            result = dfl_set_property_value(object_handle, PROPERTY_WD_TM_FLAG, property_data, DF_NONE);
            g_variant_unref(property_data);
            if (result != DFL_OK) {
                debug_log(DLOG_ERROR, "set ChassisPayload object PwrButtonLock fail!(result=%d)\r\n", result);
            }
        }
    } else {
        debug_log(DLOG_ERROR, "get ChassisPayload object fail!(result=%d)\r\n", result);
    }

    return;
}


LOCAL void wdt_do_timeout_log(PWATCHDOG2_S p_wd)
{
    struct tm local_tm;
    gchar time_str_buf[TIME_STR_BUFF_LEN] = {0};
    const gchar *time_str = NULL;

    if (dal_localtime_r(&g_wdt_kicking_info.timestamp, &local_tm) == RET_ERR) {
        debug_log(DLOG_ERROR, "%s trans time[%ld] to local time err\n", __FUNCTION__, g_wdt_kicking_info.timestamp);
        return;
    }

    if (strftime(time_str_buf, sizeof(time_str_buf), TIME_STR_FMT, &local_tm) == 0) {
        debug_log(DLOG_ERROR, "%s strftime err\n", __FUNCTION__);
        time_str = "Unknown";
    } else {
        time_str = time_str_buf;
    }

    maintenance_log_v2(MLOG_INFO, FC_BMC_WDT_TIMEOUT_INFO,
        "Watchdog timer (%s) timeout, do %s. Kicking %u times in total, last at %s\n",
        wdt_timer_use_to_str(p_wd->timer_use), wdt_timeout_action_to_str(p_wd->tmout_action),
        g_wdt_kicking_info.total_cnt, time_str);

    return;
}


gint32 set_ipc_debug_flag(GSList *input_list)
{
    guint32 param_cnt;
    guint8 input_flag;

    if (input_list == NULL) {
        debug_printf("param is NULL");
        return RET_ERR;
    }

    param_cnt = g_slist_length(input_list);
    if (param_cnt != 1) {
        debug_printf("param is not match");
        return RET_ERR;
    }

    input_flag = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    if (input_flag != 0 && input_flag != 1) {
        debug_printf("param is not match");
        return RET_ERR;
    }
    if (input_flag == 0) {
        g_ipc_debug_flag = 0;
    }
    
    if (input_flag == 1) {
        g_ipc_debug_flag = 1;
    }

    return RET_OK;
}


gint32 set_ipc_check_value(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value)
{
    if (g_ipc_debug_flag == 1) {
        return RET_OK;
    }

    if (g_ipc_debug_flag == 0) {
        (void)dal_set_property_value_byte(object_handle, PROPERTY_IS_IPC_COMM_NORMAL, 0x01, DF_NONE);
    }
    
    return RET_OK;
}


LOCAL gboolean is_ipc_channel_normal(void)
{
#define MAX_CHECK_COUNT 5
#define ABNORMAL_THRESHOLD 4
    OBJ_HANDLE object_handle = 0;
    guint32 i;
    guint8 ipc_flag = 0;
    guint8 abnormal_count = 0;
    
    
    if (wdt_object_handle(&object_handle) != RET_OK) {
        return FALSE;
    }

    for (i = 0; i < MAX_CHECK_COUNT; i++) {
        (void)dal_set_property_value_byte(object_handle, PROPERTY_IS_IPC_COMM_NORMAL, 0x02, DF_NONE);
        vos_task_delay(1000UL);
        (void)dal_get_property_value_byte(object_handle, PROPERTY_IS_IPC_COMM_NORMAL, &ipc_flag);
        if (ipc_flag != 0x01) {
            abnormal_count++;
        }
    }
    maintenance_log_v2(MLOG_WARN, FC__PUBLIC_OK,
        "Check the IPC channel %d times, it has been abnormal for %d times", MAX_CHECK_COUNT, abnormal_count);
    if (abnormal_count >= ABNORMAL_THRESHOLD) {
        return FALSE;
    } else {
        return TRUE;
    }
}


LOCAL gboolean is_bmc_in_upgrade(void)
{
    gint32 ret;
    OBJ_HANDLE upgrade_handle = 0;
    guint8 upgrade_status = 0xFF;
    GSList *output_list = NULL;
    
    ret = dal_get_object_handle_nth(PFN_CLASS_NAME, 0, &upgrade_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get the %s object failed, ret = %d", PFN_CLASS_NAME, ret);
        return FALSE;
    }

    ret = dfl_call_class_method(upgrade_handle, METHOD_PFN_GET_UPGRADE_STATUS, NULL, NULL, &output_list);
    if (ret == DFL_OK) {
        upgrade_status = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
    }
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

    if (upgrade_status == UPGRADE_IN_PROGRESS || upgrade_status == UPGRADE_MIDDLE) {
        return TRUE;
    } else {
        return FALSE;
    }
}


LOCAL void wdt_timeout_handler(PWATCHDOG2_S p)
{
    g_printf("watchdog2 timeout!!!!!!!!\r\n");
    gint32 ret;
    if (is_ipc_channel_normal() != TRUE) {
        
        if (is_bmc_in_upgrade() == TRUE) {
            debug_log(DLOG_ERROR, "The IPC channel is abnormal, but iBMC is upgrading now");
            return;
        }
        
        
        debug_log(DLOG_ERROR, "The IPC channel is abnormal, will reboot iBMC");
        maintenance_log_v2(MLOG_WARN, FC__PUBLIC_OK, "The IPC channel is abnomal, will reboot iBMC");
        ret = dfl_reboot();
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "dfl_reboot failed, ret=%d", ret);
        }
        return;
    }

    // 根据看门狗状态更新对应的超时标志
    wdt_update_timer_expire_flag(p);

    // 超时标志已更新，及时保存持久化数据
    wdt_save();

    // 更新看门狗属性
    if (wdt_update_timer_object(p) != RET_OK) {
        debug_log(DLOG_ERROR, "wdt update timer err\n");
    }

    // 涉及双BIOS切换的，只有在BIOS POST阶段才切换
    if (p->timer_use == WATCHDOG_TIMER_BIOSPOST) {
        // 查询是否双BIOS，如果是，则切换
        (void)hop_ctrl_bios_switch(CHASSIS_FRU_ID, SW_BIOS_ACTION_WDT);
    }

    // 执行看门狗超时动作
    wdt_do_timeout_action(p);

    wdt_do_timeout_log(p);

    wdt_kicking_info_init();

    // 更新系统复位原因
    wdt_update_restart_cause(p);

    // 触发看门狗超时属性变更，用于记录事件
    if (p->not_log != 1) {
        wdt_set_tm_flag(TRUE);
    }
}


guchar wdt_set_timer(const IPMIMSG_SET_WATCHDOG_TIMER_TYPE_S *pSet)
{
    g_watchdog.pre_tmout_int_f = 0;

    if ((pSet->pre_tmout_int == WATCHDOG_PRE_TIMEOUT_SMI) || (pSet->pre_tmout_int == WATCHDOG_PRE_TIMEOUT_NMI) ||
        (pSet->pre_tmout_int == WATCHDOG_PRE_TIMEOUT_MSG)) {
        g_watchdog.pre_tmout_int_en = 1;
    } else {
        
        g_watchdog.pre_tmout_int_en = 0;
    }
    g_watchdog.not_log = pSet->not_log;
    
    g_watchdog.running &= pSet->not_stop;

    if (g_watchdog.running) {
        wdt_kicking_info_update();
    } else {
        wdt_kicking_info_init();
    }

    // 触发属性变更，清除看门狗事件
    wdt_set_tm_flag(FALSE);

    g_watchdog.timer_use = pSet->timer_use;
    g_watchdog.pre_tmout_int = pSet->pre_tmout_int;
    g_watchdog.tmout_action = pSet->tmout_action;
    g_watchdog.pre_tmout_interval = pSet->pre_tmout_interval;

    g_watchdog.initial_countdown = MAKE_WORD(pSet->initial_cnt_h, pSet->initial_cnt_l);
    g_watchdog.current_countdown = g_watchdog.initial_countdown;
    g_watchdog_tick = vos_tick_get();

    g_watchdog.timer_expir_flag_oem &= (guchar)(~pSet->timer_expir_flag_clear_oem);
    g_watchdog.timer_expir_flag_sms_os &= (guchar)(~pSet->timer_expir_flag_clear_sms_os);
    g_watchdog.timer_expir_flag_osload &= (guchar)(~pSet->timer_expir_flag_clear_osload);
    g_watchdog.timer_expir_flag_bios_post &= (guchar)(~pSet->timer_expir_flag_clear_bios_post);
    g_watchdog.timer_expir_flag_bios_frb2 &= (guchar)(~pSet->timer_expir_flag_clear_bios_frb2);

    wdt_create_timer();

    // 保存看门狗状态
    wdt_save();

    // 更新看门狗属性
    if (wdt_update_timer_object(&g_watchdog) != RET_OK) {
        debug_log(DLOG_ERROR, "wdt update timer err\n");
    }

    return COMP_CODE_SUCCESS;
}


LOCAL void wdt_notify_host(WATCHDOG2_S *p)
{
    WDI_CONFIG wdi_cfg = { 0 };
    gint32 fd_l = 0;
    gint32 ret;

    wdi_cfg.tu = p->timer_use;
    wdi_cfg.pim = p->pre_tmout_int;
    wdi_cfg.ta = p->tmout_action;
    wdi_cfg.wpto = 1;

    debug_log(DLOG_DEBUG, "tu:%u,pim:%u,ta:%u!\n", p->timer_use, p->pre_tmout_int, p->tmout_action);

    fd_l = open(WDI_OPT_INTERFACE, O_WRONLY);
    if (fd_l < 0) {
        debug_log(DLOG_ERROR, "open wdi fail!\n");
        return;
    }

    ret = ioctl(fd_l, WDI_CMD_IRQ, &wdi_cfg);
    if (ret < 0) {
        debug_log(DLOG_ERROR, "This doesn't seem to be a valid device!\n");
    }

    debug_log(DLOG_DEBUG, "wdt_notify_host already send(Ret:%d)!\n", ret);

    close(fd_l);
    return;
}


LOCAL void wdt_pre_timeout_handler(WATCHDOG2_S *p)
{
    if (p == NULL) {
        return;
    }

    switch (p->pre_tmout_int) {
        case WATCHDOG_PRE_TIMEOUT_NONE:
            break;

        case WATCHDOG_PRE_TIMEOUT_SMI:
            break;

        case WATCHDOG_PRE_TIMEOUT_NMI:
            // 如果系统未上电，直接返回ERR
            if (HARD_POWER_OFF == hop_get_pwr_signal(CHASSIS_FRU_ID)) {
                debug_log(DLOG_ERROR, "power off and can't chassis diag interrupt action.\r\n");
                break;
            }
            // 控制系统触发诊断中断
            (void)pp_chassis_nmi_interrupt();
            maintenance_log_v2(MLOG_INFO, FC__PUBLIC_OK, "Set FRU%u to issue NMI successfully.", CHASSIS_FRU_ID);
            break;

        case WATCHDOG_PRE_TIMEOUT_MSG:
            break;

        default:
            break;
    }
    
    wdt_notify_host(p);
}


void wdt_stop(void)
{
    g_watchdog.running = 0;

    wdt_kicking_info_init();

    
    wdt_save();

    // 触发属性变更，清除看门狗事件
    wdt_set_tm_flag(FALSE);

    // 更新看门狗属性
    if (wdt_update_timer_object(&g_watchdog) != RET_OK) {
        debug_log(DLOG_ERROR, "wdt update timer err\n");
    }
}


void wdt_restart(void)
{
    g_watchdog.current_countdown = g_watchdog.initial_countdown;
    g_watchdog.running = 1;
    g_watchdog_tick = vos_tick_get();

    wdt_kicking_info_update();

    // 保存看门狗状态
    wdt_save();

    // 更新看门狗属性
    if (wdt_update_timer_object(&g_watchdog) != RET_OK) {
        debug_log(DLOG_ERROR, "wdt update timer err\n");
    }
}


LOCAL void wdt_count_down_task(void)
{
    gint32 pretimeout;
    gulong watchdog_tick_diff;
    gulong watchdog_tick_curr;
    gulong cout_diff;

    (void)prctl(PR_SET_NAME, (gulong)"WdtCountDown");

    for (;;) {
        while (!g_watchdog.running) {
            vos_task_delay(100UL);
        }

        while (g_watchdog.current_countdown) {
            if (!g_watchdog.running) {
                break;
            }
            g_watchdog.current_countdown--;
            pretimeout = (gint32)(g_watchdog.current_countdown - g_watchdog.pre_tmout_interval * 10); 
            if (pretimeout <= 0 && g_watchdog.pre_tmout_int_f != 1) {
                g_watchdog.pre_tmout_int_f = 1;
                wdt_pre_timeout_handler(&g_watchdog);
            }
            
            watchdog_tick_curr = vos_tick_get();
            watchdog_tick_diff = watchdog_tick_curr - g_watchdog_tick;
            
            cout_diff = (g_watchdog.initial_countdown - g_watchdog.current_countdown) * 100;
            debug_log(DLOG_DEBUG, "watchdog_tick_diff:%lu initial_countdown*100:%u countdown_time:%lu",
                watchdog_tick_diff, g_watchdog.initial_countdown * 100, 
                cout_diff);

            
            if ((watchdog_tick_diff > cout_diff) && (g_watchdog.current_countdown >= 1)) {
                g_watchdog.current_countdown--;
                debug_log(DLOG_DEBUG, "cout_diff:%u",
                    (g_watchdog.initial_countdown - g_watchdog.current_countdown) * 100); 
            }
            
            
            vos_task_delay(100UL);
        }
        if (g_watchdog.running) {
            
            g_watchdog.running = 0;
            wdt_timeout_handler(&g_watchdog);
        }
    }
}


gint32 wdt_set_timer_method(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    guint8 wdt_rq[7] = {0}; 
    guint8 i;
    const IPMIMSG_SET_WATCHDOG_TIMER_TYPE_S *set_wdt = NULL;
    guchar result;

    if (input_list == NULL) {
        debug_log(DLOG_ERROR, " The input list is NULL \n");
        return RET_ERR;
    }
    
    for (i = 0; i < 6; i++) {
        wdt_rq[i] = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, i));
    }

    set_wdt = (const IPMIMSG_SET_WATCHDOG_TIMER_TYPE_S *)wdt_rq;

    result = wdt_set_timer(set_wdt);
    if (result != COMP_CODE_SUCCESS) {
        debug_log(DLOG_ERROR, "wdt set timer error!\r\n");
    }
    maintenance_log_v2(MLOG_INFO, FC__PUBLIC_OK,
        "Method ----Set the watchdog timer to (RAW:%02x-%02x-%02x-%02x-%02x-%02x) successfully \n", wdt_rq[0],
        wdt_rq[1], wdt_rq[2], 
        wdt_rq[3],            
        wdt_rq[4],            
        wdt_rq[5]);           
    return RET_OK;
}