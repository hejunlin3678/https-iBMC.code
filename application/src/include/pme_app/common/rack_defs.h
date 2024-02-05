/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved.
 * Description: 机柜管理相关的定义。
 * Author: z00382105
 * Create: 2020-7-15
 * Notes: 用于跨模块使用的相关的定义。
 */


#ifndef __RACK_DEFINE_H__
#define __RACK_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define CONNECTOR_UNIT              "UnitConnector"     /* UnitInfo的连接器类型 */
/* BEGIN: Modified by zhangfuhai 00382105, 2019/6/14  机柜标签配置相关宏定义,将点灯状态相关的定义移至asset_locator.h以便rack_manage.c调用 PN:UADP115155  */
/* BEGIN: Modified by zhangfuhai 00382105, 2019/6/14 机柜配置标签Type类型宏定义  PN:UADP115155  */
#define RACK_TAG_TYPE_STR                "RackTag"
#define RACK_TAG_CONFIG_FAILED              1
#define RACK_TAG_CONFIG_SUCCESS             0
#define RACK_TAG_INNER_ERROR_MESSAGE     "The operation failed due to an internal error."
#define RACK_TAG_TYPE_INDEX_STR          "100"

/* Modified by zhangfuhai 00382105, 2019/11/20   PN:UADP439587 */
#define CHECK_RACK_PWR_CAP_CONF_ERR_BASE        4000
#define PWR_CAP_CONF_PARAM_VAL_OUT_OF_RANGE     (CHECK_RACK_PWR_CAP_CONF_ERR_BASE + 1)
#define PWR_CAP_CONF_SERIALNUMBER_DUPLICATED    (CHECK_RACK_PWR_CAP_CONF_ERR_BASE + 2)
#define PWR_CAP_CONF_PEAK_CLIPPING_ENABLED      (CHECK_RACK_PWR_CAP_CONF_ERR_BASE + 3)
#define PWR_CAP_CONF_DEVICE_COMM_LOST           (CHECK_RACK_PWR_CAP_CONF_ERR_BASE + 4)

#define RACK_INFO_AREA_EEP_OFFSET 0
#define RACK_INFO_AREA_LENGTH_OFFSET 2
#define RACK_INFO_AREA_CRC_LEN 2
#define RACK_EEP_NNR_CONFIG_OFFSET (RACK_INFO_AREA_EEP_OFFSET)
#define RACK_EEP_NNR_CONFIG_EEP_SIZE 16
#define RACK_EEP_NNR_CONFIG_EEP_DATA_LEN 12
#define RACK_EEP_PS_CNT_OFFSET (RACK_EEP_NNR_CONFIG_OFFSET + RACK_EEP_NNR_CONFIG_EEP_SIZE)
#define RACK_EEP_PS_CNT_SIZE 16
#define RACK_EEP_PS_CNT_DATA_LEN 12
#define RACK_EEP_BATTERY_CNT_OFFSET (RACK_EEP_PS_CNT_OFFSET + RACK_EEP_PS_CNT_SIZE)
#define RACK_EEP_BATTERY_CNT_SIZE 16
#define RACK_EEP_BATTERY_CNT_DATA_LEN 12
#define RACK_EEP_MODULE_CNT_SIZE 16
#define RACK_EEP_MODULE_CNT_DATA_LEN 12
#define RACK_EEP_UCOUNT_OFFSET (RACK_EEP_BATTERY_CNT_OFFSET + RACK_EEP_BATTERY_CNT_SIZE)
#define RACK_EEP_UCOUNT_SIZE 16
#define RACK_EEP_UCOUNT_DATA_LEN 12
#define RACK_EEP_EMPTY_SN_OFFSET (RACK_EEP_UCOUNT_OFFSET + RACK_EEP_UCOUNT_SIZE)
#define RACK_EEP_EMPTY_SN_SIZE 64
#define RACK_EEP_EMPTY_SN_LEN 60
#define RACK_EEP_TYPICAL_CONF_SN_OFFSET (RACK_EEP_EMPTY_SN_OFFSET + RACK_EEP_EMPTY_SN_SIZE)
#define RACK_EEP_TYPICAL_CONF_SN_SIZE 64
#define RACK_EEP_TYPICAL_CONF_SN_LEN 60
#define RACK_EEP_RACK_MODEL_OFFSET (RACK_EEP_TYPICAL_CONF_SN_OFFSET + RACK_EEP_TYPICAL_CONF_SN_SIZE)
#define RACK_EEP_RACK_MODEL_SIZE 32
#define RACK_EEP_RACK_MODEL_LEN 28
#define RACK_EEP_WORKLOAD_NEEDED_PS_CNT_OFFSET (RACK_EEP_RACK_MODEL_OFFSET + RACK_EEP_RACK_MODEL_SIZE)
#define RACK_EEP_WORKLOAD_NEEDED_PS_CNT_SIZE 16
#define RACK_EEP_WORKLOAD_NEEDED_PS_CNT_LEN 12
#define RACK_EEP_BASIC_RACK_SN_OFFSET (RACK_EEP_WORKLOAD_NEEDED_PS_CNT_OFFSET + RACK_EEP_WORKLOAD_NEEDED_PS_CNT_SIZE)
#define RACK_EEP_BASIC_RACK_SN_SIZE (64)
#define RACK_EEP_BASIC_RACK_SN_LEN (60)
#define RACK_EEP_RACK_FUNCTION_OFFSET (RACK_EEP_BASIC_RACK_SN_OFFSET + RACK_EEP_BASIC_RACK_SN_SIZE)
#define RACK_EEP_RACK_FUNCTION_SIZE 32
#define RACK_EEP_RACK_FUNCTION_LEN 28
#define TOKEN_INVALID                   0
#define TOKEN_VALID                     1
/* UnitInfo中Detected字段含义定义 */
#define TAG_DETECTED            1       //标签已经被扫描到
#define TAG_NOT_DETECTED        0       //标签没有被扫描到

#define UNIT_INFO_RW_STR        "ReadWrite"
#define UNIT_INFO_READ_ONLY_STR "ReadOnly"
#define DEVICE_OFFLINE_NORMALLY     0
#define DEVICE_OFFLINE_ABNORMALLY   1
#define RACK_DEFAULT_BACKUP_PERIOD_SECONDS 180  // 默认的机柜备电时间
#define PSU_OUTPUT_LIMIT_POWER_COE 1024     // 电源输出限功率修正系数
#define RACK_EXPECTANT_MAX_PSU_NUMBERS 24       // 机柜内最大的期望的电源个数
#define ERR_CODE_PART_SUCCESS 1                 // 部分成功错误码
#define BBU_CHARGE_MODIFY 1024                  // 电池充电设置修正系数
#define BBU_RATED_CAPACITY 20                   // 电池额定容量
/* BEGIN: Added by l00375984, 2019/6/17   UADP253956 支持U位高度修改冲突检测机制 */
#define SET_RFID_ERR_BASE                   4000
#define SET_RFID_DEV_MODEL_ERR              (-(SET_RFID_ERR_BASE + 1))
#define SET_RFID_DEV_TYPE_ERR               (-(SET_RFID_ERR_BASE + 2))
#define SET_RFID_VENDOR_ERR                 (-(SET_RFID_ERR_BASE + 3))
#define SET_RFID_SER_NUM_ERR                (-(SET_RFID_ERR_BASE + 4))
#define SET_RFID_PART_NUM_ERR               (-(SET_RFID_ERR_BASE + 5))
#define SET_RFID_HEIGHT_ERR                 (-(SET_RFID_ERR_BASE + 6))
#define SET_RFID_STATUS_ERR                 (-(SET_RFID_ERR_BASE + 7))
#define SET_RFID_EEP_SPACE_LACK_ERR         (-(SET_RFID_ERR_BASE + 8))
#define SET_RFID_EEP_INFO_TOO_LONG_ERR      (-(SET_RFID_ERR_BASE + 9))
#define SET_RFID_REMOVED_WHEN_ONLINE_ERR    (-(SET_RFID_ERR_BASE + 10))
#define SET_RFID_SWITCH_COUNT_LIMIT_ERR     (-(SET_RFID_ERR_BASE + 11))

#define SET_UNIT_DIRECTION_ERR_BASE         5000
#define SET_UNIT_DIRECTION_NOT_SUPPORTED    (-(SET_UNIT_DIRECTION_ERR_BASE + 1))

/* 机柜内设备的SSDP发现状态宏定义 */
#define DEVICE_OFF_LINE         0         //设备(的iBMC)未上线(被iRM的SSDP发现)
#define DEVICE_ON_LINE          1         //设备(的iBMC)上线(被iRM的SSDP发现)
#define DEVICE_INIT             0xFF      //设备未定义状态

#define DEVICE_MODEL_DX111              "DX111"
#define DEVICE_MODEL_CE8850_64CQ_EI     "CE8850-64CQ-EI"
#define DEVICE_MODEL_8850_64CQ_EI       "8850-64CQ-EI"
#define DEVICE_MODEL_DX510              "DX510"
#define DEVICE_MODEL_8861_4C_EI         "8861-4C-EI"
#define DEVICE_MODEL_DX511              "DX511"
#define DEVICE_MODEL_DX112_LEFT         "DX112-LEFT"
#define DEVICE_MODEL_DX112_RIGHT        "DX112-RIGHT"
#define DEVICE_MODEL_DX112_FULL         "DX112-FULL"
#define DEVICE_TYPE_NETWORK_STR         "Network"
#define DEVICE_TYPE_SERVER_STR          "Server"
#define RATE_POWER_OF_DX111             500
#define RATE_POWER_OF_CE8850_64CQ_EI    750
#define RATE_POWER_OF_DX510             850
#define RATE_POWER_OF_8861_4C_EI        658
#define RATE_POWER_OF_DX511             1100
#define RATE_POWER_OF_DX112_LEFT        350
#define RATE_POWER_OF_DX112_RIGHT       350
#define RATE_POWER_OF_DX112_FULL        700

/* 机柜备电能力不足告警相关宏定义 */
#define RACK_BACKUP_CAP_SATISFIED       1  
#define RACK_BACKUP_CAP_NOT_SATISFIED   -1

#define BACKUP_CAP_SCENARIO_USER_SETTING    "Value set by user"
#define BACKUP_CAP_SCENARIO_POWERCAP        "Power cap"
#define BACKUP_CAP_SCENARIO_DEFAULT_VALUE   "Default value"
#define SCENARIO_RETED_POWER_NOT_SATISFIED  "Rated power not satisfied"

#define BACKUP_CAP_ALARM_STATE     1
#define BACKUP_CAP_RECOVER_STATE   0

#define PERCENT_TRANS 100.0

/* 机柜资产管理类RackAssetMgmt的Type属性枚举定义*/
typedef enum {
    RACK_ASSET_MGMT_TYPE_MODBUS = 0x00, /* 0，通过modbus和物理资产定位条管理 */
    RACK_ASSET_MGMT_TYPE_NETWORK,       /* 1，通过网络管理 */
} rack_asset_mgmt_type_t;

/* 机柜资产管理类RackAssetMgmt的DirectionType属性枚举定义 */
typedef enum {
    RACK_ASSET_MGMT_DIRECTION_CHANGEABLE = 0x00, /* 0，机柜U位方向可变 */
    RACK_ASSET_MGMT_DIRECTION_FIXED,             /* 1，机柜U位方向固定 */
} rack_asset_mgmt_direction_type_t;

#define RACK_ASSET_MGMT_DIRECTION_CHANGEABLE_STR    "Changeable"
#define RACK_ASSET_MGMT_DIRECTION_FIXED_STR         "Fixed"

/* 机柜资产管理类RackAssetMgmt的UnitOccupyDirection属性枚举定义*/
typedef enum {
    RACK_ASSET_MGMT_UNIT_OCCPUY_DOWNWARD = 0x00, /* 0，U位占用方向向下 */
    RACK_ASSET_MGMT_UNIT_OCCPUY_UPWARD,          /* 1，U位占用方向向上 */
} rack_asset_mgmt_unit_occupy_direction_t;

/* 机柜资产管理服务的可用状态宏定义 */
typedef enum {
    ASSET_MGMT_SVC_OFF = 0, // 资产管理功能不可用，初始状态、资产条拔出时的状态
    ASSET_MGMT_SVC_ON       // 资产管理功能可用，完成第一轮标签扫描和信息获取后的状态
} ASSET_MGMT_STATUS;

/* U位信息类UnitInfo的RWCapability属性枚举定义 */
typedef enum {
    UNIT_INFO_RW = 0x00, /* 0，用户可读写 */
    UNIT_INFO_READ_ONLY, /* 1，用户只读 */
} unit_info_rw_status_t;

/* BEGIN: Modified by 00356691 zhongqiu, 2019/1/26 增加注释，后续需要同步修改controlNTag.h中的相同定义  问题单号:DTS2019012404626 解决启动阶段误报插拔事件问题 */
/*资产标签管理模块相关定义*/
typedef enum {
    TAG_NOT_FOUND = 0x00, /* 未识别到标签 */
    TAG_NORMAL,           /* 识别到标签且读写正常 */
    TAG_READ_FAIL,        /* 识别到标签但读取异常 */
    TAG_WRITE_FAIL,       /* 识别到标签但写入异常 */
    TAG_UNKNOW            /* 标签状态获取异常，初始值 */
} tag_stat_t;

/* 将iRM资产条U位灯颜色枚举变量提取到此处 */
typedef enum {
    U_LED_GREEN = 0,
    U_LED_RED,
    U_LED_YELLOW,
    U_LED_OFF,
    U_LED_EQUIPMENT_TEST,
    U_LED_COLOR_MAX, // 请勿在此后添加灯颜色，此为最后一个成员
} U_LED_COLOR;

/* END: Added by l00375984, 2019/6/17   UADP253956 支持U位高度修改冲突检测机制 */
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __RACK_DEFINE_H__ */
