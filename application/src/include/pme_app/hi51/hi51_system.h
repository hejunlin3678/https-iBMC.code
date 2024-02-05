/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2001-2020. All rights reserved.
 * Description: hi51 system header file
 */
#ifndef _HI51_SYSTEM_H_
#define _HI51_SYSTEM_H_

/* 系统最大任务数 */
#define HI51_SYS_TASK_CNT           4

/* 系统最大设备数 */
#define HI51_SYS_DEV_CNT            4

/* 最大支持电源总数, MM920最多可支持8个电源 */
#define HI51_SYS_DEV_CNT_MAX        8

/* 任务名称长度 */
#define HI51_TASK_NAME_LEN_MAX      (sizeof(guint32) * 8)

/* FRU缓存大小 */
#define FRU_DATA_LEN                256
#define FRU_ITEM_LEN                (32 - 1)

/* SRAM中收发缓存 */
#define SRAM_TX_LEN_MAX             1024
#define SRAM_RX_LEN_MAX             1024

/* 系统中断向量 */
#define SYS_INTERRUPT_VECTOR    4   /* 注意，该常量不要加括号以及类型尾注 */
#define INTRRUPT_USING          1   /* 注意，该常量不要加括号以及类型尾注 */

/* 任务运行状态定义 */
#define TASK_STATE_INVALID      0 /* 无效任务状态 */
#define TASK_STATE_DORMANT      1 /* 休眠 */
#define TASK_STATE_RUNNING      2 /* 运行 */
#define TASK_STATE_READY        3 /* 就绪 */
#define TASK_STATE_PENDING      4 /* 延迟 */

/* 设备状态定义 */
#define DEV_STATE_PLUGOUT       0 /* 设备拔出 */
#define DEV_STATE_RUNNING       1 /* 设备运行 */
#define DEV_STATE_ERROR         2 /* 设备故障 */

/* 无效值定义 */
#define TASK_ID_INVALID         0xFF  /* 无效任务ID */
#define DEV_ID_INVALID          0xFF  /* 无效设备ID */
#define DEV_TYPE_INVALID        0xFF  /* 无效设备类型 */
#define INT_VECTOR_INVALID      0xFF  /* 无效中断向量 */

/* 中断屏蔽位控制 */
#define ISR_MASK_ENABLE         1 /* 中断使能 */
#define ISR_MASK_DISABLE        0 /* 中断禁止 */

#ifdef _IN_51_

#ifdef USE_SDCC
typedef void (*ISR_FUNC)(void) __using INTRRUPT_USING;
#else
typedef void (*ISR_FUNC)(void);
#endif
typedef void (*TASK_FUNC)(guint8 task_id);
typedef void (*INIT_FUNC)(void);
typedef gint8 (*PROBE_FUNC)(guint8 dev_id);

typedef struct TAG_ISR_MASK_S {
    guint8 vector;
    guint8 state;
} ISR_MASK_S;

typedef struct TAG_TASK_INFO_S {
    guint8 task_id;
    guint8 init_state;
    char name[HI51_TASK_NAME_LEN_MAX];
} TASK_INFO_S;

/* 宏函数 */
#define INC_SYS_REG    INC_VAL32
#define READ_SYS_REG   READ_VAL32_WITH_TYPE
#define WRITE_SYS_REG  WRITE_VAL32

/* 错误码 */
#define SYS_OK  0
#define SYS_ERR (-1)
extern gint8 g_sys_errno;

/* 系统API */
#ifdef USE_SDCC
#define _nop_() \
    __asm\
    nop\
    __endasm
#else
void _nop_ (void);
#endif

#define RETURN_WITH_SLEEP(task_id, ticks) do {\
    EA = 0;\
    WRITE_SYS_REG(SRAM_TASK_SLEEPCNT(task_id), ticks);\
    WRITE_SYS_REG(SRAM_TASK_STATE(task_id), TASK_STATE_PENDING);\
    EA = 1;\
    return;\
} while (0);


#define RETURN_WITH_DORMANT(task_id) do {\
    EA = 0;\
    WRITE_SYS_REG(SRAM_TASK_STATE(task_id), TASK_STATE_DORMANT);\
    EA = 1;\
    return;\
} while (0);

#define TASK_WAKE_UP(task_id) do {\
    EA = 0;\
    WRITE_SYS_REG(SRAM_TASK_STATE(task_id), TASK_STATE_READY);\
    EA = 1;\
} while (0);

#define TASK_DORMANT(task_id) do {\
    EA = 0;\
    WRITE_SYS_REG(SRAM_TASK_STATE(task_id), TASK_STATE_DORMANT);\
    EA = 1;\
} while (0);

void sys_delay1us(guint32 times);
void sys_int_mask(guint8 vector, guint8 enable);

extern __idata DATA_U32_S g_tx_buf;
extern __idata DATA_U32_S g_tx_extern_buf;   /* i2c写时4个字节不能满足要求, 需要增加4个字节 */

#endif /* end of _IN_51_ */

#endif