/* ******************************************************************************
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 *
 * -------------------------------------------------------------------------------
 *
 * util_forward_type.h
 *
 * Project Code: DOPAI
 * Module Name: interpreter
 * Date Created: 2020-02-15
 * Author: wutao
 * Description: 后端类型及配置项定义
 *
 * -------------------------------------------------------------------------------
 * Modification History
 * DATE          NAME                       DESCRIPTION
 * -----------------------------------------------------------------------------
 *
 * ***************************************************************************** */

#ifndef FORWARD_TYPE_H
#define FORWARD_TYPE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DOPAI_FORWARD_CPU = 0,
    /*
     Firtly find the first available backends not equal to CPU
     If no other backends, use cpu
     */
    DOPAI_FORWARD_OPENCL = 1,
    DOPAI_FORWARD_DAVINCI = 2,
    DOPAI_FORWARD_AUTO = 3,

    /* User can use API from Backend.hpp to add or search Backend */
    DOPAI_FORWARD_USER_0 = 4,
    DOPAI_FORWARD_USER_1 = 5,
    DOPAI_FORWARD_USER_2 = 6,
    DOPAI_FORWARD_USER_3 = 7,

    DOPAI_FORWARD_ALL
} DOPAIForwardType;

// ScheduleConfig: 用户config参数, 用于选择后端和线程数
typedef struct TagScheduleConfig {
    DOPAIForwardType type;       /**< 后端类型 */
    int32_t numThread;           /**< 线程数 */
    DOPAIForwardType backupType; /**< 备用后端类型 */
} ScheduleConfig;

#ifdef __cplusplus
}
#endif

#endif /* ForwardType_h */
