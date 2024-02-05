/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved.
 * Description: 系统上下电管理相关的定义。
 * Author: h00282621
 * Create: 2020-3-10
 * Notes: 用于跨模块使用的相关的定义。
 */

#ifndef __PAYLOAD_DEFINE_H__
#define __PAYLOAD_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


#define PERMIT_BLADE_POWER_ON 0x1 /* HMM允许刀片上电 */
#define FORBID_BLADE_POWER_ON 0x0 /* HMM未允许刀片上电 */

#define IPMI_SET_POWER 0x01   /* IPMI命令设置电源状态 */
#define METHOD_SET_POWER 0x02 /* 方法设置电源状态 */

/* 热插拔状态定义 */
#define FRU_HS_STATE_M0 0
#define FRU_HS_STATE_M1 1
#define FRU_HS_STATE_M2 2
#define FRU_HS_STATE_M3 3
#define FRU_HS_STATE_M4 4
#define FRU_HS_STATE_M5 5
#define FRU_HS_STATE_M6 6
/* ***************************************************
 * BMC能够直接感知FRU状态的，不可能进入M7状态，
 * 只有通过IPMB-x(IPMB-L)总线管理FRU的，才有可能进入M7
 * 以前与SMM失去联系后让FRU进入M7的做法是错误的
 * *************************************************** */
#define FRU_HS_STATE_M7 7

// 延时上电模式数字
typedef enum ITEM_DELAY_MODE_TYPE {
    DELAY_MODE_SHORT = 0,  // 短延时，2s内随机延时，或按槽位延时
    DELAY_MODE_HALF = 1,   // 长延时，随即选取一半的服务器进行延时，延时时间环境变量中读取
    DELAY_MODE_ALL = 2,    // 长延时，所有服务器都进行延时
    DELAY_MODE_RANDOM = 3, // 随机延时模式，即该服务器可指定xx秒(1~xx秒内随机分配一个延时时间进行延时)
    DELAY_MODE_INVALID
} ITEM_DELAY_MODE;

// 延时上电模式字符串
#define POWER_ON_DELAY_DEFAULTMODE "DefaultDelay"
#define POWER_ON_DELAY_HALFMODE "HalfDelay"
#define POWER_ON_DELAY_FIXEDMODE "FixedDelay"
#define POWER_ON_DELAY_RANDOMMODE "RandomDelay"

#define MAX_POWER_DELAY_COUNT 1200 // 最大上电延时时间2分钟，100ms/count
#define MAX_POWER_DELAY_SECONDS 120.0
#define INT_TWO 2
#define INT_TEN 10
#define DOUBLE_TEN 10.0
#define INT_THIRTY 30
#define DOUBLE_THIRTY 30.0
#define HUNDRED_MILLISECONDS 100


#define HARD_POWER_OFF 0
#define HARD_POWER_ON 1

#define SOFT_RESET 0
#define HARD_RESET 1

#define SRC_CHAN_NUM_RPC 0x0D

/* restart cause */
#define RESTART_CAUSE_UNKNOWN 0
#define RESTART_CAUSE_CHASSISCONTROL 1
#define RESTART_CAUSE_RESETBUTTON 2
#define RESTART_CAUSE_POWERBUTTON 3
#define RESTART_CAUSE_WATCHDOGEXP 4
#define RESTART_CAUSE_OEM 5
#define RESTART_CAUSE_ALWAYSRESTORE 6
#define RESTART_CAUSE_RESTOREPREVIOUS 7
#define RESTART_CAUSE_RESETBYPEF 8
#define RESTART_CAUSE_POWERCYCLEBYPEF 9
#define RESTART_CAUSE_SOFTRESET 10
#define RESTART_CAUSE_RTCWAKEUP 11
#define RESTART_CAUSE_RESETBYACSPCOLLECT 12
#define RESTART_CAUSE_IPMI_MAX 0x80 // IPMI规范定义的重启原因当前定义到了0x0B，这里预留到0x80

/* OEM 扩展重启原因定义 */
enum {
    RESTART_CAUSE_OEM_IERR_DIAG_FAIL = 0x81,          // FDM诊断IERR失败时触发热复位
    RESTART_CAUSE_OEM_PCIE_SWITCH_OR_RETIMER_UPGRADE, // 升级PCIE switch或retimer完成后触发热复位
    RESTART_CAUSE_OEM_IERR_DIAG_SUCCESS,              // FDM诊断IERR成功时触发热复位
    RESTART_CAUSE_OEM_MAX
};

/* CPU过温下电后上下电策略 */
enum {
    THERMAL_TRIP_NO_ACTION = 0,
    THERMAL_TRIP_POWER_ON
};


#define PAYLOAD_POWER_STATE_ON 1
#define PAYLOAD_POWER_STATE_OFF 0
#define PAYLOAD_POWER_STATE_UNSPECIFIED 0xFF


#define POWER_RESTORE_POLICY_ALWAYS_OFF 0
#define POWER_RESTORE_POLICY_PREVIOUS 1
#define POWER_RESTORE_POLICY_ALWAYS_ON 2
#define POWER_RESTORE_POLICY_NO_CHANGED 3
#define POWER_RESTORE_POLICY_ALWAYS_ON_RO 0xF2

#define NO_PER_LOCK 0
#define PERSIST_LOCK 1
#define SAVE_TEMPORARY_POWER_BUTTON_LOCK 2

#define ALL_POWER_GOOD_OK 0x01
#define ALL_POWER_GOOD_FAIL 0x00


/* FRU control */
#define FRU_CONTROL_COLD_RESET 0
#define FRU_CONTROL_WARM_RESET 1
#define FRU_CONTROL_GRACEFUL_REBOOT 2
#define FRU_CONTROL_ISSUEDIAGNOSTICINTERRUPT 3
#define FRU_CONTROL_POWER_ON 4
#define FRU_CONTROL_POWER_OFF 5
#define FRU_CONTROL_OPTION_NUM 6 // iBMC支持的frucontrol操作类型的个数
typedef gint32 (*FRU_CONTROL_FUN)(guchar fru_id, GSList *caller_info, gchar *action_ssuc, guint32 ssuc_len,
    gchar *action_fail, guint32 fail_len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __PAYLOAD_DEFINE_H__ */
