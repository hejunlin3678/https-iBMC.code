
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include "pme/common/mscm_vos.h"
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "wdt2_main.h"
#include "wdt2.h"


gint32 wdt_init(void)
{
    wdt_task_init();
    wdt_kicking_info_init();
    // 恢复看门狗状态
    wdt_restore();
    // 看门狗对象初始化
    wdt_obj_init();
    return RET_OK;
}


void wdt_start(void)
{
    if (is_watchdog_running()) {
        wdt_create_timer();
    }
    g_printf("\r\nWDT............................................................[ %s  ]\r\n",
        is_watchdog_running() == RET_OK ? "Run " : "Stop");
}