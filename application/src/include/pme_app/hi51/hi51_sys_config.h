/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2001-2021. All rights reserved.
 * Description: hi51 sys config header file
 */
#ifndef _HI51_SYS_CONFIG_H_
#define _HI51_SYS_CONFIG_H_

#ifdef _IN_51_
#include "hi51_type.h"
#include "hi51_i1710_51.h"
#include "hi51_ps_psmi.h"
#include "hi51_ps_pmbus.h"
#include "hi51_ps_pmbus_8100.h"
#include "hi51_ps_ina220.h"
#include "hi51_ps_monitor.h"
#include "hi51_internal_bus.h"
#else
#include <glib.h>
#endif /* end of _IN_51_ */

#include "hi51_i1710.h"
#include "hi51_i2c.h"
#include "hi51_pmbus.h"
#include "hi51_pmbus_8100.h"
#include "hi51_sram.h"
#include "hi51_system.h"
#include "hi51_wdg.h"
#include "hi51_fru.h"
#include "hi51_power_supply.h"

/*
 * 当前电源输出电流最大值为 243.9A （3000瓦白金电源）；
 * c51 与 BMC 间以 16bit 传值，最大值为 65535
 * 65535 / 243.9 = 268.6962，因此缩放因子可选取 100
 */
#define PS_I_SCALING_FACTOR 100

/* 最大支持电源总数 */
#define PS_DEV_CNT                  4

/* 最大支持电源总数, MM920最多可支持8个电源 Fusionpod 最大可支持12个电源 CCPod支持电源数量为24 */
#define PS_DEV_CNT_MAX              24

enum ps_monitor {
    VOUT_STATUS = 0, /* 输出电压状态 */
    IOUT_STATUS, /* 输出电流状态 */
    VIN_STATUS, /* 输入电压状态 */
    TEMPER_STATUS, /* 电源温度传感器的状态 */
    FAN_STATUS, /* 电源风扇的状态 */
    CML_STATUS, /* 命令的状态 */
    MFR_STATUS, /* 厂商自定义的状态 */

    PS_MONITOR_END
};

/* <ID分配> */
/* 任务ID分配 */
#define PS_MONITOR_POWER_TASK_ID    0
#define FRU_FATCH_TASK_ID           1
#define PS_MONITOR_I_V_TASK_ID      2
#define READ_REG_TASK_ID            3

/* 设备ID分配 */
#define PS_DEV_ID_START             0

/* 设备类型分配 */
#define PS_DEV_TYPE_INA220          0
#define PS_DEV_TYPE_PSMI            1
#define PS_DEV_TYPE_PMBUS           2
#define PS_DEV_TYPE_PMBUS_5500      8
#define PS_DEV_TYPE_PMBUS_8100      9

/* 读取ina220的power错误码 */
#define RW_REDAY_STATE              0
#define RW_RIGHT_STATE              1
#define RW_ERROR_STATE              2
#define INVALID_PCA9545             0xffffffff
/* h00272616  DTS2016010404377 */
#define LOWER_LIMIT_POWER           20
#define INVALID_POWER               0xffff

#define C51_I2C_READ_MODE           0
#define C51_I2C_WRITE_MODE          1

/* I2C 访问不到时的错误码 */
#define C51_I2C_OK                  0
#define C51_I2C_FAIL                1

/* SRAM定义 */
/*---------------------------- 系统SRAM地址分配----------------------------*/
/* -----配置----- */
typedef struct tag_ps_sys_cfg_s {
    guint32 present;    /* 设备在位状态设置 */
    guint32 wdg_enable; /* 看门狗使能 */
    guint32 fru_addr[HI51_SYS_DEV_CNT_MAX];
    guint32 fru_bus_id[HI51_SYS_DEV_CNT_MAX];
    guint32 pca9545_addr;
    guint32 pca9545_bus_id;
    guint32 pca9545_channel;
    guint32 is_pca9545_switch; /* 电源是否要切换9545 */
    guint32 psu_assigned_chan[HI51_SYS_DEV_CNT_MAX];
    guint32 fru_source;
    guint32 ps_flag[HI51_SYS_DEV_CNT_MAX];    /* bit1 Input OK;bit0 Output OK */
#ifdef ARM64_HI1711_ENABLED
    guint32 psu_handle[HI51_SYS_DEV_CNT_MAX]; /* 电源MCU CHIP对象句柄 */
    guint32 fru_handle[HI51_SYS_DEV_CNT_MAX]; /* 电源MCU CHIP对象句柄 */
#endif
} PS_SYS_CFG_S;

/* 设备在位状态 */
#define SRAM_DEV_PRESENT        (SRAM_CONFIG_SYS_BASE)
/* 看门狗配置(该配置须在系统加载前指定) */
#define SRAM_WDG_ENABLE         (SRAM_DEV_PRESENT + sizeof(guint32))

/* FRU */
/* FRU地址 */
#define SRAM_FRU_0_ADDR         (SRAM_WDG_ENABLE + sizeof(guint32))
#define SRAM_FRU_ADDR(x)        (SRAM_FRU_0_ADDR + sizeof(guint32) * (x))
#define SRAM_FRU_BUS_ID_0       SRAM_FRU_ADDR(HI51_SYS_DEV_CNT_MAX)
#define SRAM_FRU_BUS_ID(x)      (SRAM_FRU_BUS_ID_0 + sizeof(guint32) * (x))
#define SRAM_PCA9545_ADDR       SRAM_FRU_BUS_ID(HI51_SYS_DEV_CNT_MAX)
#define SRAM_PCA9545_BUSID      (SRAM_PCA9545_ADDR + sizeof(guint32))
#define SRAM_PCA9545_CH         (SRAM_PCA9545_BUSID + sizeof(guint32))
#define SRAM_PCA9545_SWITCH     (SRAM_PCA9545_CH + sizeof(guint32))
#define SRAM_PSU_ASSIGN_CH0     (SRAM_PCA9545_SWITCH + sizeof(guint32))
#define SRAM_PSU_ASSIGN_CH(x)   (SRAM_PSU_ASSIGN_CH0 + sizeof(guint32) * (x))
#define SRAM_FRU_SOURCE         SRAM_PSU_ASSIGN_CH(HI51_SYS_DEV_CNT_MAX)
#define SRAM_PS_FLAG_0          (SRAM_FRU_SOURCE + sizeof(guint32))
#define SRAM_PS_FLAG(x)         (SRAM_PS_FLAG_0 + sizeof(guint32) * (x))
#define SRAM_I2C_RW_STAT        SRAM_PS_FLAG(HI51_SYS_DEV_CNT_MAX)          // 读取的状态
#define SRAM_I2C_RW_INFO        (SRAM_I2C_RW_STAT + sizeof(guint32))        // 传入51的参数信息
#define SRAM_I2C_RW_OFFSET      (SRAM_I2C_RW_INFO + sizeof(guint32))        // 传入51的offset信息
#define SRAM_I2C_RW_DATA0       (SRAM_I2C_RW_OFFSET + sizeof(guint32))      // 传入51的数据0
#define SRAM_I2C_ACCESS_FAIL(x) (SRAM_I2C_RW_DATA0 + sizeof(guint32) * (x)) // i2c访问失败的状态寄存器

#define PS_MGNT_VER             1 /* 电源管理版本 */
#define MAX_OFFSET_WIDTH        4
#define MAX_C51_RW_LEN          4

/* 51和A9的位字节序不一样 */
#ifdef _IN_51_
typedef struct tag_c51_rw_reg_info_s {
    guint8 chan;
    guint8 addr;
    guint8 offset_width : 4;   /* 目前最多支持4 */
    guint8 rw_flag : 4;        /* 读写标志,如果是1代表写, 0表示读 */
    guint8 rw_len;
    guint8 offset[MAX_OFFSET_WIDTH];
    guint8 rw_buf[MAX_C51_RW_LEN];
} C51_RW_REG_INFO_S;
#else
typedef struct tag_c51_rw_reg_info_s {
    guint8 rw_len;
    guint8 offset_width : 4;   /* 目前最多支持4 */
    guint8 rw_flag : 4;        /* 读写标志,如果是1代表写, 0表示读 */
    guint8 addr;
    guint8 chan;
    guint8 offset[MAX_OFFSET_WIDTH];
    guint8 rw_buf[MAX_C51_RW_LEN];
} C51_RW_REG_INFO_S;
#endif /* end of _IN_51_ */

/* -----数据----- */
/* 任务调度 */
typedef struct tag_ps_sys_task_s {
    guint32 state;
    gchar name[HI51_TASK_NAME_LEN_MAX];
    guint32 sleep_cnt;
    guint32 run_cnt;
}PS_SYS_TASK_S;

typedef struct tag_ps_sys_task_all_s {
    PS_SYS_TASK_S task_state[HI51_SYS_TASK_CNT];
}PS_SYS_TASK_ALL_S;

#define SRAM_TASK_STATE_OFFSET      0
#define SRAM_TASK_NAME_OFFSET       (SRAM_TASK_STATE_OFFSET + sizeof(guint32))
#define SRAM_TASK_SLEEPCNT_OFFSET   (SRAM_TASK_NAME_OFFSET + HI51_TASK_NAME_LEN_MAX)
#define SRAM_TASK_RUNCNT_OFFSET     (SRAM_TASK_SLEEPCNT_OFFSET + sizeof(guint32))

#define SRAM_TASK_BASE              (SRAM_SYSTEM_DATA_BASE)
#define SRAM_TASK(x)                (SRAM_TASK_BASE + sizeof(PS_SYS_TASK_S) * (x))

#define SRAM_TASK_STATE(x)          (SRAM_TASK(x) + SRAM_TASK_STATE_OFFSET)
#define SRAM_TASK_NAME(x)           (SRAM_TASK(x) + SRAM_TASK_NAME_OFFSET)
#define SRAM_TASK_SLEEPCNT(x)       (SRAM_TASK(x) + SRAM_TASK_SLEEPCNT_OFFSET)
#define SRAM_TASK_RUNCNT(x)         (SRAM_TASK(x) + SRAM_TASK_RUNCNT_OFFSET)

/* 设备管理 */
typedef struct tag_ps_sys_dev_s {
    guint32 type;
    guint32 state;
} PS_SYS_DEV_S;

typedef struct tag_ps_sys_dev_all_s {
    PS_SYS_DEV_S dev_state[HI51_SYS_DEV_CNT];
} PS_SYS_DEV_ALL_S;

#define SRAM_DEV_TYPE_OFFSET        0
#define SRAM_DEV_STATE_OFFSET       (SRAM_DEV_TYPE_OFFSET + sizeof(guint32))

#define SRAM_DEV_BASE               SRAM_TASK_RUNCNT(HI51_SYS_TASK_CNT)
#define SRAM_DEV(x)                 (SRAM_DEV_BASE + sizeof(PS_SYS_DEV_S) * (x))
#define SRAM_DEV_TYPE(x)            (SRAM_DEV(x) + SRAM_DEV_TYPE_OFFSET)
#define SRAM_DEV_STATE(x)           (SRAM_DEV(x) + SRAM_DEV_STATE_OFFSET)

/* FUR数据 */
typedef struct TAG_PS_FRU_S {
    guint32 fru_state;
    guint8 fru_data[FRU_DATA_LEN];
} PS_FRU_S;

typedef struct TAG_PS_FRU_PMBUS_S {
    guint32 fru_state;
    guint8 mfr_id_len;
    guint8 mfr_id[FRU_ITEM_LEN];
    guint8 mfr_model_len;
    guint8 mfr_model[FRU_ITEM_LEN];
    guint8 mfr_version_len;
    guint8 mfr_version[FRU_ITEM_LEN];
    guint8 mfr_date_len;
    guint8 mfr_date[FRU_ITEM_LEN];
    guint8 mfr_sn_len;
    guint8 mfr_sn[FRU_ITEM_LEN];
    guint32 input_type;
    guint8 part_number_len;
    guint8 part_number[FRU_ITEM_LEN];
} PS_FRU_PMBUS_S;

#define FRU_STATE_OFFSET            0
#define FRU_DATA_OFFSET             (FRU_STATE_OFFSET + sizeof(guint32))

#define MFR_ID_LEN_OFF              (FRU_STATE_OFFSET + sizeof(guint32))
#define MFR_ID_OFFSET               (MFR_ID_LEN_OFF + sizeof(guint8))
#define MFR_MODEL_LEN_OFF           (MFR_ID_OFFSET + FRU_ITEM_LEN)
#define MFR_MODEL_OFFSET            (MFR_MODEL_LEN_OFF + sizeof(guint8))
#define MFR_VERSION_LEN_OFF         (MFR_MODEL_OFFSET + FRU_ITEM_LEN)
#define MFR_VERSION_OFFSET          (MFR_VERSION_LEN_OFF + sizeof(guint8))
#define MFR_DATE_LEN_OFF            (MFR_VERSION_OFFSET + FRU_ITEM_LEN)
#define MFR_DATE_OFFSET             (MFR_DATE_LEN_OFF + sizeof(guint8))
#define MFR_SN_LEN_OFF              (MFR_DATE_OFFSET + FRU_ITEM_LEN)
#define MFR_SN_OFFSET               (MFR_SN_LEN_OFF + sizeof(guint8))
#define FUR_INPUT_TYPE_OFFSET       (MFR_SN_OFFSET + FRU_ITEM_LEN)
#define FUR_PART_NUMBER_LEN_OFF     (FUR_INPUT_TYPE_OFFSET + sizeof(guint32))
#define FUR_PART_NUMBER_OFFSET      (FUR_PART_NUMBER_LEN_OFF + sizeof(guint8))
#define MFR_PS_RATE_OFFSET          (FUR_PART_NUMBER_OFFSET + FRU_ITEM_LEN)

#define SRAM_FRU_INFO_BASE          SRAM_DEV(HI51_SYS_DEV_CNT)
#define SRAM_FRU_INFO(x)            (SRAM_FRU_INFO_BASE + sizeof(PS_FRU_S) * (x))

#define SRAM_FRU_STATE(x)           (SRAM_FRU_INFO(x) + FRU_STATE_OFFSET)
#define SRAM_FRU_DATA(x)            (SRAM_FRU_INFO(x) + FRU_DATA_OFFSET)

#define SRAM_MFR_ID(x)              (SRAM_FRU_INFO(x) + MFR_ID_LEN_OFF)
#define SRAM_MFR_MODEL(x)           (SRAM_FRU_INFO(x) + MFR_MODEL_LEN_OFF)
#define SRAM_MFR_VERSION(x)         (SRAM_FRU_INFO(x) + MFR_VERSION_LEN_OFF)
#define SRAM_MFR_DATE(x)            (SRAM_FRU_INFO(x) + MFR_DATE_LEN_OFF)
#define SRAM_MFR_SN(x)              (SRAM_FRU_INFO(x) + MFR_SN_LEN_OFF)
#define SRAM_FRU_INPUT_TYPE(x)      (SRAM_FRU_INFO(x) + FUR_INPUT_TYPE_OFFSET)
#define SRAM_FRU_PART_NUMBER(x)     (SRAM_FRU_INFO(x) + FUR_PART_NUMBER_LEN_OFF)
#define SRAM_PS_RATE(x)             (SRAM_FRU_INFO(x) + MFR_PS_RATE_OFFSET)

/* 通用数据收发缓存 51<-->外设 */
#define SRAM_TX_BUF                 SRAM_FRU_INFO(HI51_SYS_DEV_CNT)
#define SRAM_RX_BUF                 (SRAM_TX_BUF + SRAM_TX_LEN_MAX)

#define SYS_START_FLAG              0x14031915
#define SRAM_SYSTEM_START_LAB       (SRAM_RX_BUF + SRAM_RX_LEN_MAX)

#define I2C_ERROR_NUM               (SRAM_SYSTEM_START_LAB + sizeof(guint32))
#define I2C_TX_NUM                  (I2C_ERROR_NUM + sizeof(guint32))
#define DEBUG_REG_0                 (I2C_TX_NUM + sizeof(guint32))
#define DEBUG_REG_1                 (DEBUG_REG_0 + sizeof(guint32))
#define DEBUG_REG_2                 (DEBUG_REG_1 + sizeof(guint32))
#define DEBUG_REG_3                 (DEBUG_REG_2 + sizeof(guint32))
#define DEBUG_REG_4                 (DEBUG_REG_3 + sizeof(guint32))
#define DEBUG_REG_5                 (DEBUG_REG_4 + sizeof(guint32))
#define DEBUG_REG_6                 (DEBUG_REG_5 + sizeof(guint32))
#define DEBUG_REG_7                 (DEBUG_REG_6 + sizeof(guint32))
#define DEBUG_INT_CNT               (DEBUG_REG_7 + sizeof(guint32))
#define READ_CNT_MAX                (DEBUG_INT_CNT + sizeof(guint32))
#define WRITE_CNT_MAX               (READ_CNT_MAX + sizeof(guint32))
#define I2C_TMOUT_CNT               (WRITE_CNT_MAX + sizeof(guint32))
#define INA220_CFG_RDBAK(x)         (I2C_TMOUT_CNT + sizeof(guint32) + sizeof(guint32) * (x))
#define INA220_PARA_RDBAK(x)        (INA220_CFG_RDBAK(PS_DEV_CNT) + sizeof(guint32) * (x))
#define PMBUS_BLOCK_TMP             (INA220_PARA_RDBAK(PS_DEV_CNT))

/*---------------------------- 用户SRAM地址分配----------------------------*/
/* -----配置----- */
typedef struct tag_ps_i2c_cfg_s {
    guint32 i2c_enable;
    guint32 i2c_speed;  /* i2c 速率 */
} PS_I2C_CFG_S;

typedef struct tag_ina220_para_s {
    guint32 ina220_cfg;
    guint32 ina220_multi_para;
} INA220_PARA_S;

typedef struct tag_ps_user_cfg_s {
    PS_I2C_CFG_S i2c_cfg[I2C_BUS_CNT];
#ifdef ARM64_HI1711_ENABLED
    guint32 source_type;  /* 功率获取来源 0--CHIP(INA220);1--PSU */
    guint32 protocol[PS_DEV_CNT_MAX];
    guint32 slot_addr[PS_DEV_CNT_MAX]; /* 电源I2C地址 */
    guint32 i2c_bus[PS_DEV_CNT_MAX]; /* 电源I2C总线 */
    guint32 pw_in_divisor[PS_DEV_CNT_MAX];
    guint32 pw_in_multiplier[PS_DEV_CNT_MAX];
    INA220_PARA_S ina220_para[PS_DEV_CNT_MAX];
    guint32 power_source_type[PS_DEV_CNT_MAX];
#else
    guint32 source_type;  /* 功率获取来源 0--CHIP(INA220);1--PSU */
    guint32 protocol[HI51_SYS_DEV_CNT_MAX];
    guint32 slot_addr[HI51_SYS_DEV_CNT_MAX]; /* 电源I2C地址 */
    guint32 i2c_bus[HI51_SYS_DEV_CNT_MAX]; /* 电源I2C总线 */
    guint32 pw_in_divisor[HI51_SYS_DEV_CNT_MAX];
    guint32 pw_in_multiplier[HI51_SYS_DEV_CNT_MAX];
    INA220_PARA_S ina220_para[HI51_SYS_DEV_CNT_MAX];      
#endif
} PS_USER_CFG_S;

/* I2C */
#define PS_I2C_ENABLE_OFFSET    0
#define PS_I2C_SPEED_OFFSET     (sizeof(guint32))
#define SRAM_PS_I2C_CONFIG_0    SRAM_CONFIG_USER_BASE
#define SRAM_PS_I2C_CONFIG(x)   (SRAM_PS_I2C_CONFIG_0 + sizeof(PS_I2C_CFG_S) * (x))
#define SRAM_PS_I2C_ENABLE(x)   (SRAM_PS_I2C_CONFIG(x) + PS_I2C_ENABLE_OFFSET) /* 使能 */
#define SRAM_PS_I2C_SPEED(x)    (SRAM_PS_I2C_CONFIG(x) + PS_I2C_SPEED_OFFSET) /* 速率 */

/* 电源
FusionPod产品支持12个电源模块，修改原有的PS_DEV_CNT_MAX 8->12，
而FusionPod不属于C51管理，不应当改动此处，因此此处的宏定义值 使用 HI51_SYS_DEV_CNT_MAX=8
*/
#define SRAM_PS_POWER_SRC       SRAM_PS_I2C_CONFIG(I2C_BUS_CNT) /* 功率信息源 ： 0--CHIP(INA220);1--PSU */
#define SRAM_PS_PROTOCOL_0      (SRAM_PS_POWER_SRC + sizeof(guint32))
/* 电源协议 */ /* 0--PSMI; 1--PMBUS ;0xff--未知 */
#define SRAM_PS_PROTOCOL(x)     (SRAM_PS_PROTOCOL_0 + sizeof(guint32) * (x))
#define SRAM_PS_SLOT_0_ADDR     SRAM_PS_PROTOCOL(HI51_SYS_DEV_CNT_MAX) /* 电源地址 */
#define SRAM_PS_SLOT_ADDR(x)    (SRAM_PS_SLOT_0_ADDR + sizeof(guint32) * (x))
#define SRAM_PS_BUS_0_ADDR      SRAM_PS_SLOT_ADDR(HI51_SYS_DEV_CNT_MAX) /* 电源地址 */
#define SRAM_PS_BUS_ADDR(x)     (SRAM_PS_BUS_0_ADDR + sizeof(guint32) * (x))
#define SRAM_PS_POWER_IN_DIVISOR_0      SRAM_PS_BUS_ADDR(HI51_SYS_DEV_CNT_MAX) /* 电源输入功率除数 */
#define SRAM_PS_POWER_IN_DIVISOR(x)     (SRAM_PS_POWER_IN_DIVISOR_0 + sizeof(guint32) * (x))
#define SRAM_PS_POWER_IN_MULTI_0        SRAM_PS_POWER_IN_DIVISOR(HI51_SYS_DEV_CNT_MAX) /* 电源输入功率乘数 */
#define SRAM_PS_POWER_IN_MULTI(x)       (SRAM_PS_POWER_IN_MULTI_0 + sizeof(guint32) * (x))

/* INA220(非插拔电源) */
#define INA220_CFG_OFFSET       0
#define INA220_MULTI_PARA_OFFSET     (sizeof(guint32))

#define SRAM_PS_INA220_PARA_BASE SRAM_PS_POWER_IN_MULTI(HI51_SYS_DEV_CNT_MAX)
#define SRAM_PS_INA220_PARA(x)  (SRAM_PS_INA220_PARA_BASE + sizeof(INA220_PARA_S) * (x))
/* INA220 Configuration Register (00h) 16 bits */
#define SRAM_PS_INA220_CFG(x)   (SRAM_PS_INA220_PARA(x) + INA220_CFG_OFFSET)
/* INA220 Calibration Register (05h) 16 bits */
#define SRAM_PS_MULTI_PARA(x)   (SRAM_PS_INA220_PARA(x) + INA220_MULTI_PARA_OFFSET)

typedef struct tag_ps_para_s {
    /* 系统配置 */
    PS_SYS_CFG_S sys;
    guint32 rev0[(SRAM_CONFIG_SYS_LENGTH - sizeof(PS_SYS_CFG_S)) / sizeof(guint32)];

    /* 用户配置 */
    PS_USER_CFG_S user;
    guint32 rev1[(SRAM_CONFIG_USER_LENGTH - sizeof(PS_USER_CFG_S)) / sizeof(guint32)];
} PS_PARA_S;

/* ----数据---- */
/* SRAM监测数据区 */
typedef struct tag_ps_monitor_one_s {
    guint32 power; /* 单电源功率 */
    guint32 event; /* 电源健康事件 */
#ifdef ARM64_HI1711_ENABLED
    guint64 energy; /* 所有电源累计总能耗，瓦秒为单位存储。32位无符号数仅约1193kwh */
#else
    guint32 energy_low32; /* 所有电源累计总能耗，数据的低32位, 单位: 瓦秒 */
    guint32 energy_high32; /* 所有电源累计总能耗, 数据的高32位，单位: 瓦秒 */
#endif
    guint32 aver_power; /* 平均功率 */
    guint32 vin; /* 输入电压 */
    guint32 vout; /* 输出电压 */
    guint32 iin; /* 输入电流 */
    guint32 iout; /* 输出电流 */
    guint32 power_out; /* 输出功率 */
    guint32 env_temp; /* 环境温度 */
    guint32 chip_temp; /* 电源内部器件温度(原边) */
    guint32 chip_temp2; /* 电源内部器件温度(副边) */
    guint32 input_type; /* 输入模式 0--DC；1--AC；2--AC/DC */
    guint32 ps_rate;    /* 电源额定功耗:PMBUS 电源会动态刷新 */
    guint32 power_type; /* 电源类型  0--AC；1--48VDC；2--HVDC ；3--AC&240HVDC */
} PS_MONITOR_ONE_S;

typedef struct tag_ps_monitor_status_s {
    guint32 vout_status;     /* 输出电压状态 */
    guint32 iout_status;     /* 输出电流状态 */
    guint32 vin_status;      /* 输入电压状态 */
    guint32 temper_status;   /* 电源温度传感器的状态 */
    guint32 fan_status;      /* 电源风扇的状态 */
    guint32 cml_status;      /* 命令的状态 */
    guint32 mfr_status;      /* 厂商自定义的状态 */
} PS_MONITOR_STATUS_S;

typedef union tag_ps_monitor_status_u {
    guint32 byte[sizeof(PS_MONITOR_STATUS_S) / sizeof(guint32)];
    PS_MONITOR_STATUS_S ps_monitor_status;
} PS_MONITOR_STATUS_U;

#define SRAM_MONITOR_BASE_ADDR  SRAM_DATA_51_BASE_ADDR

/* 总体电源数据 */
#define SRAM_PS_POWER_ALL       SRAM_MONITOR_BASE_ADDR /* 总功率 */
#define SRAM_PS_ENERGY_LOW32    (SRAM_PS_POWER_ALL + sizeof(guint32)) /* 总能耗低32位 */
#define SRAM_PS_ENERGY_HIGH32   (SRAM_PS_ENERGY_LOW32 + sizeof(guint32)) /* 总能耗高32位 */
#define SRAM_PS_POWER_AVER      (SRAM_PS_ENERGY_HIGH32 + sizeof(guint32)) /* 总平均功率 */

/* 单个电源数据 */
#define PS_ONE_POWER_OFFSET     0
#define PS_ONE_EVENT_OFFSET         (sizeof(guint32))
#define PS_ONE_ENERGY_LOW32_OFFSET  (sizeof(guint32) * 2)
#define PS_ONE_ENERGY_HIGH32_OFFSET (sizeof(guint32) * 3)
#define PS_ONE_AVER_POWER_OFFSET    (sizeof(guint32) * 4)
#define PS_ONE_VIN_OFFSET           (sizeof(guint32) * 5)
#define PS_ONE_VOUT_OFFSET          (sizeof(guint32) * 6)
#define PS_ONE_IIN_OFFSET           (sizeof(guint32) * 7)
#define PS_ONE_IOUT_OFFSET          (sizeof(guint32) * 8)
#define PS_ONE_POWER_OUT_OFFSET     (sizeof(guint32) * 9)
#define PS_ONE_ENV_TEMP_OFFSET      (sizeof(guint32) * 10)
#define PS_ONE_CHIP_TEMP_OFFSET     (sizeof(guint32) * 11)
#define PS_ONE_CHIP_TEMP2_OFFSET    (sizeof(guint32) * 12)
#define PS_ONE_INPUT_TYPE_OFFSET    (sizeof(guint32) * 13)
#define PS_ONE_PS_RATE_OFFSET       (sizeof(guint32) * 14)
#define PS_ONE_POWER_TYPE_OFFSET    (sizeof(guint32) * 15)

#define SRAM_PS_ONE_BASE                (SRAM_PS_POWER_AVER + sizeof(guint32))
#define SRAM_PS_ONE(x)                  (SRAM_PS_ONE_BASE + ((x) * sizeof(PS_MONITOR_ONE_S)))
#define SRAM_PS_POWER(x)                (SRAM_PS_ONE(x) + PS_ONE_POWER_OFFSET) /* 单电源实时功率 */
#define SRAM_PS_EVENT(x)                (SRAM_PS_ONE(x) + PS_ONE_EVENT_OFFSET) /* 单电源事件 */
#define SRAM_PS_ENERGY_LOW32_ONE(x)     (SRAM_PS_ONE(x) + PS_ONE_ENERGY_LOW32_OFFSET) /* 单电源累计能耗低32位 */
#define SRAM_PS_ENERGY_HIGH32_ONE(x)    (SRAM_PS_ONE(x) + PS_ONE_ENERGY_HIGH32_OFFSET) /* 单电源累计能耗高32位 */
#define SRAM_PS_AVER_POWER_ONE(x)       (SRAM_PS_ONE(x) + PS_ONE_AVER_POWER_OFFSET) /* 单电源平均功率 */
#define SRAM_PS_VIN_ONE(x)              (SRAM_PS_ONE(x) + PS_ONE_VIN_OFFSET) /* 输入电压 */
#define SRAM_PS_VOUT_ONE(x)             (SRAM_PS_ONE(x) + PS_ONE_VOUT_OFFSET) /* 输出电压 */
#define SRAM_PS_IIN_ONE(x)              (SRAM_PS_ONE(x) + PS_ONE_IIN_OFFSET) /* 输入电流 */
#define SRAM_PS_IOUT_ONE(x)             (SRAM_PS_ONE(x) + PS_ONE_IOUT_OFFSET) /* 输出电流 */
#define SRAM_PS_POWER_OUT(x)            (SRAM_PS_ONE(x) + PS_ONE_POWER_OUT_OFFSET) /* 输出电流 */
#define SRAM_PS_ENV_TEMP(x)             (SRAM_PS_ONE(x) + PS_ONE_ENV_TEMP_OFFSET) /* 环境温度 */
#define SRAM_PS_CHIP_TEMP(x)            (SRAM_PS_ONE(x) + PS_ONE_CHIP_TEMP_OFFSET) /* 芯片温度 (原边) */
#define SRAM_PS_CHIP_TEMP2(x)           (SRAM_PS_ONE(x) + PS_ONE_CHIP_TEMP2_OFFSET) /* 芯片温度(副边) */
#define SRAM_PS_INPUT_TYPE(x)           (SRAM_PS_ONE(x) + PS_ONE_INPUT_TYPE_OFFSET) /* 输入模式 0--DC；1--AC */
#define SRAM_PS_PSU_RATE(x)             (SRAM_PS_ONE(x) + PS_ONE_PS_RATE_OFFSET) /* 输入最大功率 */
/* 电源类型  0--AC；1--48VDC；2--HVDC ；3--AC&240HVDC */
#define SRAM_POWER_INPUT_TYPE(x)    (SRAM_PS_ONE(x) + PS_ONE_POWER_TYPE_OFFSET)

/* BEGIN: Modified for 问题单号:Hardware_013_14 by j00383144, 2016/10/11 */
#define SRAM_PS_STATUS_BASE(x)   (SRAM_PS_ONE(HI51_SYS_DEV_CNT_MAX) + ((x) * sizeof(PS_MONITOR_STATUS_S)))
#define SRAM_PS_VOUT_STATUS(x)   (SRAM_PS_STATUS_BASE(x) + 0x0)
#define SRAM_PS_IOUT_STATUS(x)   (SRAM_PS_STATUS_BASE(x) + 0x4)
#define SRAM_PS_VIN_STATUS(x)    (SRAM_PS_STATUS_BASE(x) + 0x8)
#define SRAM_PS_TEMPER_STATUS(x) (SRAM_PS_STATUS_BASE(x) + 0xc)
#define SRAM_PS_FAN_STATUS(x)    (SRAM_PS_STATUS_BASE(x) + 0x10)
#define SRAM_PS_CML_STATUS(x)    (SRAM_PS_STATUS_BASE(x) + 0x14)

typedef struct tag_ps_monitor_s {
    guint32 power; /* 电源实时输入总功率, 单位W */
#ifdef ARM64_HI1711_ENABLED
    guint64 energy; /* 所有电源累计总能耗，瓦秒为单位存储。32位无符号数仅约1193kwh */
#else
    guint32 energy_low32; /* 单个电源累计总能耗低32位, 单位: 瓦秒 */
    guint32 energy_high32; /* 单个电源累计总能耗高32位, 单位: 瓦秒 */
#endif
    guint32 power_aver;
#ifdef ARM64_HI1711_ENABLED
    PS_MONITOR_ONE_S ps_info_one[PS_DEV_CNT_MAX];
    PS_MONITOR_STATUS_S ps_status_one[PS_DEV_CNT_MAX];
#else
    PS_MONITOR_ONE_S ps_info_one[HI51_SYS_DEV_CNT_MAX];  /* 单电源累计总能耗, 单位Ah */
    PS_MONITOR_STATUS_S ps_status_one[HI51_SYS_DEV_CNT_MAX];
#endif
} PS_MONITOR_S;

typedef enum {
    PS_MONITOR_ONE_PIN,
    PS_MONITOR_ONE_EVENT,
    PS_MONITOR_ONE_AVER_PIN,
    PS_MONITOR_ONE_VIN,
    PS_MONITOR_ONE_VOUT,
    PS_MONITOR_ONE_IIN,
    PS_MONITOR_ONE_IOUT,
    PS_MONITOR_ONE_POUT,
    PS_MONITOR_ONE_ENV_TEMP,
    PS_MONITOR_ONE_CHIP_TEMP,
    PS_MONITOR_ONE_CHIP_TEMP2,
    PS_MONITOR_ONE_INPUT_TYPE,
    PS_MONITOR_ONE_PS_RATE,
    PS_MONITOR_ONE_PTYPE,
    PS_MONITOR_ONE_STS_VOUT,
    PS_MONITOR_ONE_STS_IOUT,
    PS_MONITOR_ONE_STS_VIN,
    PS_MONITOR_ONE_STS_TEMP,
    PS_MONITOR_ONE_STS_FAN,
    PS_MONITOR_ONE_STS_CML,
    PS_MONITOR_ONE_STS_MFR,
} PS_MONITOR_ONE_INDEX;

typedef struct {
    PS_MONITOR_ONE_INDEX index;
    gpointer base_addr;
    gsize step;
} PS_MONITOR_ONE_DATA;

typedef struct tag_ps_unrecov_fault_s {
    guint32 vout_fault_value;
    guint32 vout_fault_cnt;
    guint32 vout_ps_reset_cnt;
    guint32 iout_fault_value;
    guint32 iout_fault_cnt;
    guint32 iout_ps_reset_cnt;
} PS_UNRECOV_FAULT_S;

#ifdef __DEBUG_UART__
void printf_string(gchar *buf);
#endif

#endif