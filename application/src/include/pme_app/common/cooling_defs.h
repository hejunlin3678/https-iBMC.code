/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved.
 * Description: 散热管理相关的定义。
 * Author: h00282621
 * Create: 2020-3-10
 * Notes: 用于跨模块使用的相关的定义。
 */

#ifndef __COOLING_DEFINE_H__
#define __COOLING_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


#define ERROR_ENVRANGE_NULL 0xf001                 // 温度区间数组为空错误码
#define ERROR_SPDARR_NULL 0xf002                   // 转速数组为空错误码
#define ERROR_ENVARRSIZE_INVALID_SPDARRSIZE 0xf003 // 温度区间数组与转速数组不满足小于1条件错误码
#define ERROR_ENVRANGE_NONASC 0xf004               // 温度区间数组不满足升序条件错误码
#define ERROR_SPDARR_NONASC 0xf005                 // 转速数组不满足升序条件错误码
#define ERROR_SPD_MIN_LOWER 0xf006                 // 转速数组最小值小于20错误码
#define ERROR_INVALID_SMARTMODE 0xf007             // 无效的smart cooling模式错误码
#define ERROR_UNENABLE_SMARTMODE 0xf008            // 不支持smart cooling模式错误码
#define ERROR_PARAMETER_INVALID 0xf009             // 参数错误码
#define ERROR_UNENVTEMP_OBJCET 0xf00a              // 设置环境温度对象非环境温度对象错误码
#define ERROR_ENVTEMP_OBJECT_UNCUSTOMIED 0xf00b    // 设置环境温度对象非用户自定义错误码
#define ERROR_INVALID_TOBJ_VALUE 0xf00c            // 设置不合理的tobj值错误码
#define ERROR_INPUT_LIST_LEN_INVALID 0xf00d        // 入参list链表长度不合理错误码
#define ERROR_ENVARR_LEN_INVALID 0xf00e            // 环境温度数组长度不合理
#define ERROR_ENVARR_VALUE_INVALID 0xf00f          // 环境温度数组值超出指定范围
#define ERROR_SPDARR_VALUE_INVALID 0xf010          // 风扇数组值超出指定范围
#define ERROR_INVALID_TJMAX_VAL 0xf011             // 不合理的Tjmax值错误码
#define ERROR_FUNC_OPERATION 0xf012                // 函数内部操作错误
#define ERROR_SETTING_UNSUPPORT 0xf013             // 当前环境不支持设置功能

/* 偏移地址，与Class中CustomSettingMask定义一致 */
#define COOLING_CUSTOMMODE_INLETTEMP 0
#define COOLING_CUSTOMMODE_CUPTEMP 1
#define COOLING_CUSTOMMODE_OUTTEMP 2
#define COOLING_CUSTOMMODE_DISK_ENV 3
#define COOLING_CUSTOMMODE_DISK_TOBJ 4
#define COOLING_CUSTOMMODE_MEMORY_TOBJ 5
#define COOLING_CUSTOMMODE_PCH_TOBJ 6
#define COOLING_CUSTOMMODE_VRD_TOBJ 7
#define COOLING_CUSTOMMODE_VDDQ_TOBJ 8
#define COOLING_CUSTOMMODE_NPU_HBM_TOBJ 9
#define COOLING_CUSTOMMODE_NPU_AICORE_TOBJ 10
#define COOLING_CUSTOMMODE_NPU_BOARD_TOBJ 11
#define COOLING_CUSTOMMODE_SOCBRD_INLET_TOBJ 12
#define COOLING_CUSTOMMODE_SOCBRD_OUTLET_TOBJ 13

#define SMART_MODE_ENABLE 1 // smart cooling模式使能

#define INLET_LEVEL_MIN 3  // 进风口温度设置温度档数最小值
#define ARRAY_MAXSIZE 64   // 温度区间数组和风扇转速数组最大值
#define FANSPD_STOP_VAL 0  // 风扇停转速度
#define FANSPD_MIN_VAL 20  // 风扇转速最小值
#define FANSPD_MAX_VAL 100 // 风扇转速最大值
#define MAX_ENV_VAL 60     // 进风口温度所能设置的最大值

#define FAN_TYPE_NAME_LENGTH	64
#define ENV_TEMP_INDEX 0x01 // 自定义环境温度对象索引高位
#define CPU_TOBJ_INDEX 0x02 // 自定义CPU目标温度对象索引高位

#define ENV_TEMP_TYPE 2 // 设置环境温度对象类型
#define CPU_TOBJ_TYPE 1 // 设置CPU目标温度对象类型

#define RANGE_PROP_VALUE_NUM 2
#define POLICY_INDEX_TYPE_OFFSET 8

#define CPU_TOBJ_MIN_VAL 50    // CPU所能设置的最小Tobj
#define CPU_TOBJ_MAX_VAL 80    // 从ME读取不合理的值时Tobj所能设置的最大值
#define CPU_TOBJ_INIT_CONST 15 // ME设置CPU Tobj为Tjmax-15
#define CPU_TJMAX_MIN_VAL 50   // Tjmax最小值
#define CPU_TJMAX_MAX_VAL 255  // Tjamx最大值
#define DEFAULT_TOBJ_VAL 75    // 默认配置的Tobj值

#define INLET_OBJ_INDEX 0x0100          // 进风口温度对象的索引
#define DISK_OBJ_INDEX 0x0101           // 硬盘温度对象的索引
#define SMART_MODE_CONDITION_INDEX 0xff // 智能模式条件索引

#define RSP_MAX_LEN 255        // IPMI返回数组的最大长度
#define INLET_INFO_RSP_INDEX 5 // IPMI获取进风口温度信息时的返回信息索引初始值

#define INLET_BUFF_NUM 4 // 二维数组记录的进风口信息第一维索引

#define INLET_TEMP_MIN -128 // 进风口温度最小值
#define INLET_TEMP_MAX 127  // 进风口温度最大值
#define ENV_ARR_FLAG 1      // 进风口温度数组标志位
#define SPD_ARR_FLAG 0      // 风扇转速数组标志位

#define COOLING_MEDIUM_AIR 0
#define COOLING_MEDIUM_LIQUID 1

#define COOLING_NORMAL_STATE      0       // cooling模块继续运行
#define COOLING_UPGRADE_STATE     1       // cooling模块的暂停状态
#define COOLING_ERROR_FAN_SPEED   255     // MM管理框级风扇异常时获取的速度

enum {
    LOW_FAN_SPEED_MODE = 0,
    HIGH_FAN_SPEED_MODE,
    MIDDLE_FAN_SPEED_MODE,
    COOLING_ENERGY_SAVING_MODE = 0x10,
    COOLING_LOW_NOISE_MODE,
    COOLING_HIGH_PERFORMANCE_MODE,
    COOLING_USER_DEFINED_MODE,
    COOLING_LIQUID_MODE
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __COOLING_DEFINE_H__ */
