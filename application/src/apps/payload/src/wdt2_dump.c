
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include "pme/common/mscm_vos.h"
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "wdt2.h"
#include "wdt2_dump.h"


gint32 wdt_dump_info(GSList *input_list)
{
    gulong watchdog_tick = get_watchdog_tick();
    WATCHDOG2_S watchdog = get_watchdog_info();
    WATCHDOG2_KICKING_INFO_S wdt_kicking_info = get_wdt_kicking_info();
    debug_printf("----------------------------------------\r\n");
    debug_printf("Watchdog2 Info:\r\n");
    debug_printf("not_log=%d\r\n", watchdog.not_log);
    debug_printf("running=%d\r\n", watchdog.running);
    debug_printf("timer_use=%d\r\n", watchdog.timer_use);
    debug_printf("pre_tmout_int_f=%d\r\n", watchdog.pre_tmout_int_f);
    debug_printf("pre_tmout_int=%d\r\n", watchdog.pre_tmout_int);
    debug_printf("pre_tmout_int_en=%d\r\n", watchdog.pre_tmout_int_en);
    debug_printf("tmout_action=%d\r\n", watchdog.tmout_action);
    debug_printf("pre_tmout_interval=%d\r\n", watchdog.pre_tmout_interval);
    debug_printf("timer_expir_flag_oem=%d\r\n", watchdog.timer_expir_flag_oem);
    debug_printf("timer_expir_flag_sms_os=%d\r\n", watchdog.timer_expir_flag_sms_os);
    debug_printf("timer_expir_flag_osload=%d\r\n", watchdog.timer_expir_flag_osload);
    debug_printf("timer_expir_flag_bios_post=%d\r\n", watchdog.timer_expir_flag_bios_post);
    debug_printf("timer_expir_flag_bios_frb2=%d\r\n", watchdog.timer_expir_flag_bios_frb2);
    debug_printf("initial_countdown=%d\r\n", watchdog.initial_countdown);
    debug_printf("current_countdown=%d\r\n", watchdog.current_countdown);
    debug_printf("g_watchdog_tick=%lu\r\n", watchdog_tick);
    debug_printf("wdt kicking count=%u\r\n", wdt_kicking_info.total_cnt);
    debug_printf("wdt kicking timestamp=%#lx\r\n", wdt_kicking_info.timestamp);
    debug_printf("----------------------------------------\r\n");

    return RET_OK;
}