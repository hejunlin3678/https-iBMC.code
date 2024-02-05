/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved.
* Description: PME公共头文件统一入口。
* Author: h00188524
* Create: 2013-7-4
* History:
* 1. Author: h00282621
*    Description: 拆分一部分定义到各个特性的头文件。
*/


#ifndef __PME_APP_H__
#define __PME_APP_H__
/* glib support */
#include <string.h>
#include <glib-2.0/glib.h>
#include <glib-2.0/glib/gprintf.h>
#include <sys/prctl.h>

/* PME的输入字符限制定义 */
#include "character_limit/character_limit.h"

/* 电源管理公共头文件 */
#include "power_supply/power_supply.h"

/* PME的属性方法定义 */
#include "property_method/property_method_app.h"
#include "record_wave/record_wave.h"
#include "target_temp_control/TargetTempFanControl.h"
#include "debounced/debounced.h"

/* pam扩展库接口 */
#include "pam_tally_ext/pam_tally_ext.h"
#include "pam_pme/pam_pme.h"

/* APP层公共库 */
#include "common/common_macro.h"
#include "common/fault_code.h"
#include "common/bmc_defs.h"
#include "common/user_defs.h"
#include "common/services_defs.h"
#include "common/dft_defs.h"
#include "common/ipmi_defs.h"
#include "common/network_defs.h"
#include "common/security_defs.h"
#include "common/sessions_defs.h"
#include "common/certificates_defs.h"
#include "common/sensor_defs.h"
#include "common/event_defs.h"
#include "common/syslog_defs.h"
#include "common/snmp_defs.h"
#include "common/smtp_defs.h"
#include "common/fru_defs.h"
#include "common/payload_defs.h"
#include "common/power_defs.h"
#include "common/storage_defs.h"
#include "common/bios_defs.h"
#include "common/kvm_defs.h"
#include "common/vnc_defs.h"
#include "common/upgrade_defs.h"
#include "common/sms_defs.h"
#include "common/time_defs.h"
#include "common/dataacqurisition_defs.h"
#include "common/partition_defs.h"
#include "common/cli_defs.h"
#include "common/component_defs.h"
#include "common/card_defs.h"
#include "common/config_defs.h"
#include "common/filetrans_defs.h"
#include "common/cooling_defs.h"
#include "common/https_defs.h"
#include "common/lcd_defs.h"
#include "common/led_defs.h"
#include "common/rack_defs.h"
#include "common/license_defs.h"
#include "common/ums_defs.h"
#include "common/smm_defs.h"
#include "common/fdm_defs.h"
#include "common/log_defs.h"
#include "common/lsw_defs.h"
#include "common/xpu_defs.h"
#include "common/debug_log_macro.h"
#include "common/cleanup_util.h"

#include "dalib/dalib.h"
#include "dalib/dal_module.h"
#include "dalib/dal_cpld.h"

#include "smbios/mscm_smbios.h"
#include "transfer/mscm_transfer.h"
#include "sqlite3/mscm_sqlite3.h"
#include "crypto/pme_crypto.h"
#include "smm/smm_public.h"
#include "lsw/lsw_public.h"

#include "pcie_mgmt/pcie_common.h"
#include "nvme_vpd/nvme_vpd_common.h"

#include <sys/prctl.h>
#include "ftcputemp/ftcputemp.h"

/* libmodbus库 */
#include "modbus/modbus_cust.h"
/***************************************
***************宏定义******************
***************************************/
#define UNUSED_PARAM __attribute__((__unused__))

/* 管理软件类型 */
#define MGMT_SOFTWARE_TYPE_CHASSIS_SINGLE          0x00 /* 机架单BMC */
#define MGMT_SOFTWARE_TYPE_CHASSIS_MULTI           0x01 /* 机架多BMC */
#define MGMT_SOFTWARE_TYPE_HIGH_DENSITY            0x10 /* 高密节点BMC */
#define MGMT_SOFTWARE_TYPE_HIGH_DENSITY_CHASSIS_NODE    0x11 /* 高密框式节点BMC，无物理管理节点 */
#define MGMT_SOFTWARE_TYPE_BLADE                   0x20 /* 刀片BMC */
#define MGMT_SOFTWARE_TYPE_COMBINED_BLADE          0x21 /* 组合刀片BMC */
#define MGMT_SOFTWARE_TYPE_PANGEA_PACIFIC_CTRL     0x23 /* PANGEA 控制器太平洋节点BMC */
#define MGMT_SOFTWARE_TYPE_PANGEA_ARCTIC_CTRL      0x24 /* PANGEA 控制器北冰洋节点BMC */
#define MGMT_SOFTWARE_TYPE_PANGEA_ALANTIC_CTRL     0x25 /* PANGEA 刀片BMC */
#define MGMT_SOFTWARE_TYPE_SWITCH_BLADE            0x30 /* 交换BMC */
#define MGMT_SOFTWARE_TYPE_FRAME_PERIPHERAL_MGMT   0x40 /* 机框外围部件管理BMC */
#define MGMT_SOFTWARE_TYPE_CABINET_PERIPHERAL_MGMT 0x41 /* 机柜外围部件管理BMC */
#define MGMT_SOFTWARE_TYPE_EM                      0x50 /* SMM集中管理BMC */
#define MGMT_SOFTWARE_TYPE_CMC                     0x51 /* CMC集中管理BMC */
#define MGMT_SOFTWARE_TYPE_EM_X86_BMC              0x52 /* SMM集中管理上X86 BMC */
#define MGMT_SOFTWARE_TYPE_PANGEA_MGNT             0x53 /* PANGEA SMM集中管理BMC */
#define MGMT_SOFTWARE_TYPE_RM                      0x60 /* iRM软件新增类型, 机柜信息管理BMC */
#define MGMT_SOFTWARE_TYPE_INVALID                 0xff /* 无效软件类型 */

#define CHASSIS_TYPE_RACK     0x00
#define CHASSIS_TYPE_X_SERIAL 0x10
#define CHASSIS_TYPE_BLADE    0x20
#define CHASSIS_TYPE_SWITCH   0x30
#define CHASSIS_TYPE_MM       0x40
#define CHASSIS_TYPE_CMC      0x50

enum {
    SERVER_TYPE_CHASSIS = 0,
    SERVER_TYPE_BLADE
};

enum {
    PRODUCT_TYPE_SERVER = 1,
    PRODUCT_TYPE_BLADE = 4
};



#define SYSTEM_CMD_BUF_LEN 128
#define ACCESSOR_NAME_LEN  128
#define FIRMWARE_VERSION_STRING_LEN 32

#define SYS_LOCKDOWN_FORBID 0 // 开启系统锁定后禁止SET操作
#define SYS_LOCKDOWN_ALLOW  1 // 开启系统锁定后允许SET操作
#define SYS_LOCKDOWN_NULL   2 // 只有GET操作，和系统锁定开关无关



#define continue_if_expr_true(expr) \
        if (expr)                       \
        continue

#define clear_sensitive_info(ptr, length) dal_clear_sensitive_info(ptr, length)

#define change_gid_uid(gid, uid)                             \
    do {                                                     \
        if (VOS_OK != setgid(gid)) {                         \
            debug_log(DLOG_ERROR, "Set group ID failed.\n"); \
        }                                                    \
        if (VOS_OK != setuid(uid)) {                         \
            debug_log(DLOG_ERROR, "Set user ID failed.\n");  \
        }                                                    \
        dfl_update_proxy_flag();                             \
    } while (0)


#define POSITION_BITSIZE 8  // 对象的POSITION每8bit表示在该层的位置
#define INVALID_POSITION         (guint32)(-1) // 无效position
#define INVALID_DEVICE_POSITION  0xffffffff
#define INVALID_OBJ_HANDLE       0
#define OBJ_HANDLE_COMMON        0 // 等同于上面的定义，待优化删除


#define PME_SERVICE_SUPPORT 1    // PME支持该服务
#define PME_SERVICE_UNSUPPORT 0  // PME不支持该服务


/* 多个模块共享的宏定义 */
#define DELAY_FIVE_MICROSECOND            5     // 单位us
#define DELAY_TEN_MILLISECONDS            10    // 单位ms
#define DELAY_FIFTEEN_MILLISECONDS        15    // 单位ms
#define DELAY_THIRTY_MILLISECONDS         30    // 单位ms
#define DELAY_FORTY_MILLISECONDS          40    // 单位ms
#define DELAY_HALF_SECOND                 500   // 单位ms
#define DELAY_ONE_SECOND                  1000  // 单位ms
#define DELAY_TWO_SECOND                  2000  // 单位ms

#define LOG_SPECIAL_STRING_LEN_MAX 32
#define DESC_STRING_MAX_LEN               (50)  //  相应描述的最大长度
#define BUF_COPY_MAX_LEN                  (128) // 拷贝缓冲区最大长度

#define DIFFERS_POSITION_NUM 2
#define DIFFERS_FACTOR_NUM 2
extern void register_remote_client_rpc_functions(void);

#endif /* __PME_APP_H__ */

