/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2021. All rights reserved.
 * Description: 电源模块、功耗管理相关的定义。
 * Author: h00282621
 * Create: 2020-3-10
 * Notes: 用于跨模块使用的相关的定义。
 */


#ifndef __POWER_DEFINE_H__
#define __POWER_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define POWER_SUPPLY_UNIT_NAME "PSU"

#define HUAWEI_750W_PLATINUM_PS_FRU "HUAWE 750W PLATINUM PS    " // 4个空格
#define POWER_PROTOCOL_PMBUS 1

// 最大支持电源数
#define MAX_PS_NUM 8
// fpod项目最大电池数量
#define MAX_BBU_NUM_FPOD 27

#define MAX_POWER_CAPPING_VALUE     9999
#define MIN_POWER_CAPPING_VALUE     1
#define AC_PS_NUM_IS_THREE 3
#define INVALID_PS_NUM 0


/* MSPP多节点服务器功耗封顶电源保护模式 */
#define MSPP_DISABLED 0x00
#define MSPP_ENABLED 0x01
#define MSPP_ENABLED_ENHANCED 0x02
#define MSPP_UNSUPPORT 0xFF
#define MSPP_POWER_REDUNDANCY_FAILING 1 // 电源冗余失效
#define MSPP_POWER_REDUNDANCY_NORMAL 0  // 电源冗余正常
#define MSPP_POWERCAPPING 1             // 功耗封顶进行中
#define MSPP_POWERCAPPING_FINISH 0      // 不在功耗封顶进行中
#define PS_MAX_LEN 50

#define PROPERTY_DC_MODE 0
#define PROPERTY_AC_MODE 1
#define PROPERTY_ACDC_MODE 2
#define PROPERTY_ACDC_MODE_UNKNOWN 255
#define STRING_AC_MODE "AC"
#define STRING_DC_MODE "DC"
#define STRING_ACDC_MODE "AC/DC"
#define STRING_NA_MODE "N/A"

#define PSU_SUPPLY_MAIN 0    //主路供电
#define PSU_SUPPLY_BACKUP 1  //备路供电
#define POWER_PEAK_RET_CAPPING_DISABLED             1
#define POWER_PEAK_RET_BATTERY_NOT_CONFIGURATION    2

#define CPU_PT_STATE_OUTOF_RANGE 0x02
#define CPU_PT_STATE_POWER_OFF 0x03

enum {
    PROTOCOL_PSMI,
    PROTOCOL_PMBUS,
    PROTOCOL_TCE_PMBUS = 7,
    PROTOCOL_5500_PMBUS = 8,
    PROTOCOL_8100_PMBUS = 9,
    PROTOCOL_CANBUS = 10,	
    PROTOCOL_INVALID = 0xff,
};

enum {
    BAT_PROTOCOL_I2C,
    BAT_PROTOCOL_CANBUS = 1,
    BAT_PROTOCOL_INVALID = 0xff,
};

// <!-- pmbus协议 1:oem- pmbus; 2:标准 pmbus; 3:扩展pmbus,主要用在Atlas平台; 0xFF:无效-->
enum {
    POWER_TYPE_OEM_PMBUS = 1,
    POWER_TYPE_STANDARD_PMBUS = 2,
    POWER_TYPE_EXTERN_PMBUS = 3,
    POWER_TYPE_OTHER = 0xff,
};

enum  {
    CANBUS_ENABLE_VALUE = 0x55,    
    CANBUS_DISABLE_VALUE = 0xAA,
};

enum {
    PS_IN_WORK_MODE = 0,
    PS_IN_UPGRADE_PREPARE_MODE = 1,
    PS_IN_UPFRADE_READY_MODE = 2,
};

/* 机柜功率封顶状态 */
enum {
    POWER_CAP_NOT_TRIGGERED = 0x00, // 功率封顶未触发
    POWER_CAP_TRIGGERED,            // 功率封顶触发
    POWER_CAP_UNKNOW,
};


#define MINIMUM_ALLOWABLE_POWER 1 /* 允许的最少电源个数 */

#define POWER_CFG_LOAD_BALANCE 0 // 负荷分担
#define POWER_CFG_MASTER_SPARE 1 // 主备
#define POWER_CFG_DEEP_SLEEP 3   // 深度休眠

#define POWER_CFG_ACTIVE_POWER 0  // 主用电源
#define POWER_CFG_STANDBY_POWER 1 // 备用电源

#define POWER_CAP_SUPPORT       1
#define POWER_CAP_NOT_SUPPORT   0

/* 电源模块错误码定义 */
#define ERR_ACTIVE_PS_ONT_PST (-8000)             /* 至少有一个主用电源不在位 */
#define ERR_NO_STANDBY_PS_PST (-8001)             /* 没有一个备用电源在位 */
#define ERR_ACTIVE_PS_ONT_HLS (-8002)             /* 主用电源不健康 */
#define ERR_CURRENT_POWER_HIGN (-8003)            /* 当前功率超过所有主用电源额定功率值的75% */
#define ERR_ACTIVE_PS_NUM_NE (-8004)              /* 主用电源的数量不够 */
#define ERR_PS_NOT_SUPPORT (-8005)                /* 电源不支持主备供电 */
#define ERR_ACTIVE_STANDBY_MODE_UNSUPPORT (-8006) /* 要配置的主备供电模式不支持 */
#define ERR_OPOK_UNSUPPORT (-8007)                /* 要配置的电源OPOK 不健康 */
#define ERR_NAR_VOLT_ERROR (-8008)                /* N+R电源电压不满足 */
#define ERR_NAR_NOT_SUPPORT (-8009)               /* N+R 不支持设置 */
#define ERR_NAR_BALANCE_CONFLICT (-8010)          /* N+R开启，不能设置负载均衡 或 负载均衡不能开启N+R */
#define ERR_NAR_PS_NOT_SUPPORT (-8011)            /* 电源不支持 */
#define ERR_POWER_LIMIT_NOT_SUPPORT (-8012)       /* 产品不支持设置功率封顶 */
#define ERR_NOT_ALL_BBU_ON_CHARGING (-8013)       /* 电池放电时不允许切换供电模式 */


#pragma pack(1)
typedef struct tag_power_capping_conf_s {
    guint8 cap_enable;
    guint16 cap_value;
    guint8 shutdown_flag; /* 封顶失败是否下电 */
    guint16 minimum_power;
    guint16 max_cap_value;
    guint8  cap_type; /* 封顶的类型，区分框内是只有1个单板，还是多个单板 */
    guint16 cap_value_local; /* 本板的封顶值 */
} POWER_CAPPING_CONF_S;
#pragma pack()

typedef struct tag_expected_module_count_info {
    guint8 device_type;
    const gchar *info_class_name;
    const gchar *info_prop_name;
    guint32 eep_relative_offset;
}EXPECTED_MODULE_COUNT_INFO_S;

#define BOARD_NOT_SUPPORT_DEEP_SLEEP 1 // 单板不支持深度休眠模式
#define PS_NOT_SUPPORT_DEEP_SLEEP 2    // 电源不支持深度休眠模式

#define MM_BOARD_NOT_SUPPORT_DEEP_SLEEP 3 // 单板版本不支持深度休眠模式
#define BOARD_SEND_IPMI_TIME_OUT 4        // 节点板IPMI发送超时
#define UNKNOW_REASON 0xff

#define DEEP_SLEEP_MODE_ENABLED 1 // 深度休眠模式使能

#define PS_OEM_PMBUS_READ_DC_VERSION_OFFSET 0xe4
#define PS_OEM_PMBUS_READ_PFC_VERSION_OFFSET 0xe5
#define PS_STANDARD_PMBUS_READ_DC_VERSION_OFFSET 0xe4
#define PS_STANDARD_PMBUS_READ_PFC_VERSION_OFFSET 0xe7
#define PS_5500_PMBUS_READ_VERSION_OFFSET 0xd5
#define PS_PSMI_READ_UC_INFO_OFFSET 0x00

#define OS_STATE_POWER_OFF 0
#define OS_STATE_POWER_ON  1

#define NOT_SUPPORT_AC_CYCLE 0
#define AC_CYCLE_BY_CTRL_CPLD 1
#define AC_CYCLE_BY_CTRL_PCA9555 2
// 此种类型和类型2差别在于，类型2是脉冲触发AC，类型3是低电平触发AC，同时又不同于类型1.
#define AC_CYCLE_BY_CTRL_PCA9555_NEGATIVE 3
#define AC_CYCLE_SAFELY_BY_CTRL_CPLD 4  // 需要写保护的AC掉电类型
#define AC_CYCLE_SAFELY_WRITE_VALUE 0x5A      // 打开写保护和触犯AC掉电时需要写入的值

#define PSU_INPUT_STATUS_NORMAL 0        // 正常
#define PSU_INPUT_STATUS_OVERVOLTAGE 1   // 过压
#define PSU_INPUT_STATUS_UNDERVOLTAGE 2  // 欠压
#define PSU_INPUT_STATUS_POWER_OFF 3     // 掉电
#define PSU_INPUT_STATUS_INVALID 100     // 无效状态

#define PS_VOLTAGE_STATUS_NORMAL_STR "Normal"       // 正常
#define PS_VOLTAGE_STATUS_OVER_STR "Overvoltage"    // 过压
#define PS_VOLTAGE_STATUS_UNDER_STR "Undervoltage"  // 欠压
#define PS_VOLTAGE_STATUS_POWER_OFF_STR "Poweroff"  // 掉电

#define BBU_DISCHARGE_STATUS_CHARGING 0         // 充电
#define BBU_DISCHARGE_STATUS_DISCHARGING 1      // 放电
#define BBU_DISCHARGE_STATUS_FULLY_CHARGED 2    // 满电
#define BBU_DISCHARGE_STATUS_IDLE 3             // 空闲
#define BBU_DISCHARGE_STATUS_UNKNOWN 0xFF       // 未知

#define BBU_DISCHARGE_STATUS_CHARGING_STR "Charging"            // 充电
#define BBU_DISCHARGE_STATUS_DISCHARGING_STR "Discharging"      // 放电
#define BBU_DISCHARGE_STATUS_FULLY_CHARGED_STR "FullyCharged"   // 满电
#define BBU_DISCHARGE_STATUS_IDLE_STR "Idle"                    // 空闲
#define BBU_DISCHARGE_STATUS_UNKNOWN_STR "Unknown"              // 未知

#define BBU_CANBUS_INVALID_FLOAT (-99999.0)
#define BBU_CANBUS_INVALID_INT16 (-999)
#define BBU_CANBUS_INVALID_INT32 (-99999)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __POWER_DEFINE_H__ */
