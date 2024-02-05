/******************************************************************************

                  版权所有 (C), 2013-2014, 华为技术有限公司

 ******************************************************************************
  文 件 名   : pme.h
  版 本 号   : 初稿
  部    门   :
  作    者   : h00188524
  生成日期   : 2013年7月4日
  最近修改   :
  功能描述   : pme所有公共库头文件的统一入口点
  函数列表   :
  修改历史   :
  1.日    期   : 2013年7月4日
    作    者   : h00188524
    修改内容   : 创建文件

******************************************************************************/

#ifndef __PME_H__
#define __PME_H__
/* glib support */
#include <string.h>
#include <glib-2.0/glib.h>
#include <glib-2.0/glib/gprintf.h>
#include <glib-2.0/glib/gstdio.h>

/**
 * #include "common/xxxx.h"
 * #include "debug/xxxx.h"
 *          .
 *          .
 *          .
 */

#include "dflib/template.h"

/* file_manage */
#include "file_manage/file_manage.h"
#include "persistance/persistance.h"

/* 公共库头文件 */
#include "dfrpc/df_rpc.h"
#include "dflib/dflerr.h"
#include "dflib/dflib.h"
#include "xml_parser/xml_parser.h"

#include "debug/maint_debug_so.h"
#include "pflash_drv/pflash_driver.h"

/* ??????头?募? */
#include "common/mscm_vos.h"
#include "common/mscm_macro.h"


/* add drivers lib  */
#include "drivers/i2c_base.h"
#include "drivers/cpld_upg.h"
#include "drivers/gpio_base.h"
#include "drivers/wdt.h"
#include "drivers/localbus_base.h"
#include "drivers/ipmb.h"
#include "drivers/bt.h"
#include "drivers/kcs.h"
#include "drivers/sol.h"
#include "drivers/uart.h"
#include "drivers/sys_info.h"
#include "drivers/mdio_base.h"
#include "drivers/sd_raid.h"
#include "drivers/dfx_api.h"
#include "drivers/mctp.h"
#include "drivers/mmc_base.h"



/* PME的属性方法定义  */
#include "property_method/property_method.h"

#include "ipmi/common_ipmi.h"
#include "ipmi/ipmi_oem.h"


/*------Begin:Added by q00164709 at 2013-8-12*/

/* 从例子中引用,需要找到出处头文件--BTD */
/* IPMI命令的DATA字段过滤描述长度 */
#define  IPMI_DATA_FILTER_LEN    128
#define  MODULE_OK               (RET_OK)
#define  MODULE_ERR              (RET_ERR)
#define  MAX_POSSIBLE_IPMI_FRAME_LEN    512
#define  REBOOT_BMC_TIMER_TIME          100 /* Unit:ms */
#define  PICMG_IDENTIFIER               0x00
#define  PICMG_EXTENSION_VERSION        0x22
#define  PME_CHASSIS_ADDR               0x20

enum
{
    BMC_DISABLE = 0,
    BMC_ENABLE
};


/* 异常时格式化输出--BTD, 应用debug模块统一定义公共格式 */
#define  DB_STR_FORMAT_ERR           ("Ret:%d\n")
#define  DB_STR_PARAM_POINTER_NULL   ("Param pointer is null!\n")
#define  DB_STR_DFL_HANDLE_NULL      ("get dfl handle is null!\n")
#define  DB_STR_FILE_NAME_LEN_ERR    ("FileNameLen:%d is err!\n")
#define  DB_STR_GET_DFL_INPUT_ERR    ("get dfl input param is null!\n")
#define  DB_STR_GET_PROPERTY_NULL    ("get property addr is null!\n")
#define  DB_STR_GET_SRC_DATA_FAIL    ("get_ipmi_src_data fail!\n")
#define  DB_STR_MUNU_ID_ERR          ("judge_manu_id_valid fail(Ret:0x%x)!\n")
#define  DB_STR_SET_PROPERTY_ERR     ("dfl_set_property_value fail(Ret:%d)!\n")

#define  COMP_CODE_HPM_IN_PROGRESS   0x80

/* LED状态 */
#define LED_OFF 0    //灭
#define ON_900_OFF_100 3
#define ON_100_OFF_900 4
#define ON_500_OFF_500 5//UID灯临时控制时的状态
#define ON_250_OFF_250 6
#define ON_100_OFF_100 7
#define LED_ON 0xFF   // 亮


/*------End:Added by q00164709 at 2013-8-12*/


#endif /* __PME_H__ */

