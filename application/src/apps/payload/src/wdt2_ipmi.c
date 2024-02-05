
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include "pme/common/mscm_vos.h"
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "wdt2.h"
#include "wdt2_ipmi.h"


gint32 wdt_ipmi_get_timer(const void *req_msg, gpointer user_data)
{
    IPMIMSG_GET_WATCHDOG_TIMER_TYPE_RESP_S resp_data = { 0 };
    // 输入参数检查
    if (req_msg == NULL) {
        return RET_ERR;
    }
    // 初始化响应消息缓存
    (void)memset_s((void *)&resp_data, sizeof(resp_data), 0, sizeof(resp_data));
    // 构造响应
    WATCHDOG2_S watchdog = get_watchdog_info();
    resp_data.comp_code = COMP_CODE_SUCCESS;
    resp_data.not_log = watchdog.not_log;
    resp_data.not_stop = watchdog.running;
    resp_data.timer_use = watchdog.timer_use;
    resp_data.pre_tmout_int = watchdog.pre_tmout_int;
    resp_data.tmout_action = watchdog.tmout_action;
    resp_data.pre_tmout_interval = watchdog.pre_tmout_interval;
    resp_data.timer_expir_flag_clear_oem = watchdog.timer_expir_flag_oem;
    resp_data.timer_expir_flag_clear_sms_os = watchdog.timer_expir_flag_sms_os;
    resp_data.timer_expir_flag_clear_osload = watchdog.timer_expir_flag_osload;
    resp_data.timer_expir_flag_clear_bios_post = watchdog.timer_expir_flag_bios_post;
    resp_data.timer_expir_flag_clear_bios_frb2 = watchdog.timer_expir_flag_bios_frb2;
    resp_data.initial_cnt_l = LOW(watchdog.initial_countdown);
    resp_data.initial_cnt_h = HIGH(watchdog.initial_countdown);
    resp_data.present_cnt_l = LOW(watchdog.current_countdown);
    resp_data.present_cnt_h = HIGH(watchdog.current_countdown);

    // 返回响应消息
    return ipmi_send_response(req_msg, sizeof(resp_data), (guint8 *)&resp_data);
}


LOCAL bool wdtreset_need_write_log(void)
{
    bool ret = TRUE;
    WATCHDOG2_S watchdog = get_watchdog_info();
    
    if ((watchdog.running) && (watchdog.current_countdown >= watchdog.initial_countdown / 2)) {
        ret = FALSE;
    }
    return ret;
}


gint32 wdt_ipmi_reset_timer(const void *req_msg, gpointer user_data)
{
    bool log_flag = FALSE;
    WATCHDOG2_S watchdog = get_watchdog_info();
    
    if (watchdog.pre_tmout_int_en && watchdog.pre_tmout_int_f) {
        ipmi_operation_log(req_msg, "Reset watchdog timer failed\r\n");

        debug_log(DLOG_ERROR, "Watchdog2 status invalid\r\n");
        return ipmi_send_simple_response(req_msg, COMP_CODE_STATUS_INVALID);
    }

    
    if (watchdog.initial_countdown == 0) {
        ipmi_operation_log(req_msg, "Reset watchdog timer failed\r\n");
        debug_log(DLOG_ERROR, "Watchdog not init\r\n");
        return ipmi_send_simple_response(req_msg, COMP_CODE_WATCHDOG_NOT_INIT);
    }

    log_flag = wdtreset_need_write_log();
    // 重设看门狗状态
    wdt_restart();

    if (log_flag == TRUE) {
        ipmi_operation_log(req_msg, "Reset watchdog timer successfully\r\n");
    }

    // 返回响应
    return ipmi_send_simple_response(req_msg, COMP_CODE_SUCCESS);
}

LOCAL bool wdtset_need_write_log(const IPMIMSG_SET_WATCHDOG_TIMER_TYPE_S *wtdset)
{
    bool ret = TRUE;
    WATCHDOG2_S watchdog = get_watchdog_info();
    static IPMIMSG_SET_WATCHDOG_TIMER_TYPE_S wdt_last;
    if (wtdset == NULL) {
        return RET_ERR;
    }
    if (!memcmp(wtdset, &wdt_last, sizeof(wdt_last)) && (watchdog.running) &&
        (watchdog.current_countdown >= watchdog.initial_countdown / 2)) { 
        ret = FALSE;
    }
    wdt_last = *wtdset;
    return ret;
}

gint32 wdt_ipmi_set_timer(const void *req_msg, gpointer user_data)
{
    guchar result;
    const guint8 *req_src_data = NULL;
    const IPMIMSG_SET_WATCHDOG_TIMER_TYPE_S *set_timer_req = NULL;
    bool write_log = FALSE;
    // 入参判断
    if (req_msg == NULL) {
        return RET_ERR;
    }
    // 设置看门狗参数
    req_src_data = get_ipmi_src_data(req_msg);
    if (req_src_data == NULL) {
        ipmi_operation_log(req_msg, "get_ipmi_src_data req_src_data is NULL.\n");
        debug_log(DLOG_ERROR, "get_ipmi_src_data  req_src_data is NULL.\n");
        return ipmi_send_simple_response(req_msg, COMP_CODE_UNKNOWN);
    }
    set_timer_req = (const IPMIMSG_SET_WATCHDOG_TIMER_TYPE_S *)req_src_data;

    write_log = wdtset_need_write_log(set_timer_req);
    result = wdt_set_timer(set_timer_req);
    if (write_log == TRUE) {
        maintenance_log_v2(MLOG_INFO, FC__PUBLIC_OK,
            "Set watchdog timer to (RAW:%02x-%02x-%02x-%02x-%02x-%02x) successfully", req_src_data[0], req_src_data[1],
            req_src_data[2],  
            req_src_data[3],  
            req_src_data[4],  
            req_src_data[5]); 
        ipmi_operation_log(req_msg, 
            "Set watchdog timer use to (%s), action to (%s), timeout to (%d) seconds, %s successfully",
            wdt_timer_use_to_str(set_timer_req->timer_use), wdt_timeout_action_to_str(set_timer_req->tmout_action),
            MAKE_WORD(set_timer_req->initial_cnt_h, set_timer_req->initial_cnt_l) / 10, 
            (set_timer_req->not_stop ? "keep running" : "stop running"));
    }
    // 返回ipmi响应
    return ipmi_send_simple_response(req_msg, result);
}