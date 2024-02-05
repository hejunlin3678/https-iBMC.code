/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved.
 * Description: 机框管理软件相关的定义。
 * Author: h00282621
 * Create: 2020-3-10
 * Notes: 用于跨模块使用的相关的定义。
 */

#ifndef __SMM_DEFINE_H__
#define __SMM_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define ASM_DUMP_BOOT_HEAD ("\n------Asm Data Begin------\n")

#define VOS_TICK_PER_SECOND 1000

#define MAX_TRY_CHECK_TIME  100    /* 最多重试100次 */
#define DLEAY_CHECK_TIME    50     /* 每次间隔50ms */
#define PROPERTY_SYNC 3       /* PropertySyncCondition对应的限制条件ID是3 */
#define FILE_SYNC 4           /* FileSyncCondition对应的限制条件ID是4 */

#define SLOT_ID_FAULT_WAIT_TIME 120 /* 槽位号故障单板降备等待次数1次/秒 */
#define SLOT_ID_FAULT_FLAG 1        /* 槽位号故障标志 */
#define MAX_TRY_IN_COUNT 10         /* 降备等待条件满足后最多重试10次 */

#define SMM0 0 /* SMM0 */
#define SMM1 1 /* SMM1 */

enum { /* SMM编号匿名枚举，与上面的宏定义重复，待优化 */
    SMM_NUMBER_0 = 0,
    SMM_NUMBER_1
};

enum { /* SMM状态匿名枚举 */
    SMM_STATUS_OFFLINE = 0x0,
    SMM_STATUS_BACKUP = 0x2,
    SMM_STATUS_MASTER = 0x3,
    SMM_STATUS_UNKNOWN = 0xff
};


/* MM板相关 */
#define SM1_MASTER_SM0_ABSENT 0x03
#define SM0_MASTER_SM1_ABSENT 0x0c
#define SM1_MASTER_SM0_PRESENT 0x0b
#define SM0_MASTER_SM1_PRESENT 0x0e
#define BLADE_PRESENCE 0x01
#define BLADE_POWER_OFF 0x00
#define BLADE_POWER_ON 0x01
#define BLADE_POWER_UNKNOWN 0xff
#define BLADE_NOT_PRESENCE 0x0
#define MIN_BLADE_SLAVE_ADDR 0x82
#define MAX_BLADE_SLAVE_ADDR 0x90
#define MAX_BLADE 8

/* MM FRU相关 */
#define SMM_FRU_ID 0
#define SMM_POSITION 0
#define SMM_BACK_PLANE_FRU_ID 1
#define SMM_BACK_PLANE_POSITION 1


#define ALARM_POLICY_NORMAL 0
#define ALARM_POLICY_OPTIONAL 1

/* 主备状态 */
#define ASM_ACTIVE 0
#define ASM_STAND_BY 1

typedef enum tg_timer_state {
    ASM_TIMER_IDLE        = 0x00, /* Idle */
    ASM_TIMER_REGISTERED  = 0x01, /* 软心跳已注册(创建)，但未被启动(激活)。初始状态 */
    ASM_TIMER_RUNING      = 0x02, /* 软心跳被启动(激活) */
    ASM_TIMER_STOP        = 0x03, /* 软心跳被停止 */
    ASM_TIMER_TIME_OUT    = 0x04  /* 软心跳时间耗尽，心跳丢失。最后的状态。 */
}ASM_TIMER_STATE_ENUM;

/* 软心跳数据结构 */
typedef struct stru_soft_heartbeat {
    TASKID     task_id;
    guint32    interval;         /* 软心跳的心跳间隔 10ms一次.硬件电路要求 10ms */
    ASM_TIMER_STATE_ENUM timer_state; /* 软心跳状态:5个枚举状态 */
    guint32    countdown;        /* 软心跳计数器 */
    guint32    prev_tick;        /* 上一次系统tick */
    guint32    tick_chk;         /* 系统tick检查，判断软心跳间隔是否超出范围 */
}SOFT_HEARTBEAT_CB_ST;

typedef struct stru_failover {
    TASKID task_id;              /* 主备倒换任务id */
    TASKID alarm_monitor_task_id; /* 告警监控任务id */
    guint32 allow_failover;      /* 允许倒换标志 */
    guint32 failover_request;    /* 倒换请求标志 */
    TIMERID ptid;                /* 15s清除倒换抑制标志使用的计时器 */
    guint32 use_vos_timer;       /* 当计时器创建失败时，使用线程计时来清除倒换抑制标志 0:使用心跳线程计时，1:使用计时器计时 */
    guint32 last_failover_time;  /* 上次倒换时间戳 */
}FAILOVER_CB_ST;

enum failover_check_type {
    CHECK_BY_PROPERTY_VALUE = 1,
    CHECK_BY_METHOD
};

#define STANDBY_SMM_NOT_SUPPORT_SHELF_COMP_UPGRADE "Standby smm does not support shelf component upgrade.\r\n"
#define STANDBY_SMM_UPGRADE_IN_PROGRESS "An upgrade operation of standby smm is in progress, please wait.\r\n"

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __SMM_DEFINE_H__ */
