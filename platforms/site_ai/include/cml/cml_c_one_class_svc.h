/*******************************************************************************
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 *
 * -------------------------------------------------------------------------------
 *
 * cml_c_one_class_svc.h
 *
 * Project Code: DOPAI
 * Module Name: c_wrapper
 * Date Created: 2021-11-8
 * Author:
 * Description: definition of one_class_svc c API
 *
 * -------------------------------------------------------------------------------
 * Modification History
 * DATE          NAME                       DESCRIPTION
 * -----------------------------------------------------------------------------
 *
 *******************************************************************************/

#ifndef DOPAI_CML_C_ONE_CLASS_SVC_H_INCLUDE
#define DOPAI_CML_C_ONE_CLASS_SVC_H_INCLUDE
#include <stddef.h>
#include <stdint.h>
#include "util_forward_type.h"
#include "util_sal_compiler_reg.h"
#include "cml_c_wrapper.h"

/** @defgroup cml_c_one_class_svc CML-ONE_CLASS_SVC C接口 */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup cml_c_one_class_svc
 *
 * Tag_DOPAI_CML_ONE_CLASS_SVC_Params: C-API接口下，用户设置ONE_CLASS_SVC算法参数的结构体。
 *
 */
typedef struct Tag_DOPAI_CML_ONE_CLASS_SVC_Params {
    const char *gammaType; /**< Gamma type for RBF kernel. Should be in ["scale", "auto", "number"],
                                need to end with '/0'. */
    float gamma;           /**< Gamma value if gammaType = "number". Data type is float. */
    float cacheSize;       /**< Kernel cache size (in MB). Data type is float. */
    int32_t maxIter;       /**< Maximum number of iterations. Data type is int.
                            default = -1 (system will give a large number). */
    float tol;             /**< Stopping criteria. Data type is float. */
    float nu;              /**< The upper bound ratio of outliers, as well as the lower bound of
                            support vectors. Data type is float within the range of [0, 1]. */
} DOPAI_CML_ONE_CLASS_SVC_Params;

/**
 * @ingroup cml_c_one_class_svc
 * @brief
 * 根据传入参数创建句柄，生成模型以及创建会话。
 * @par 描述：
 * 基于创建的会话进行后续数据填入和训练。
 * @attention
 * 该接口需要DOPAI_CML_ReleaseSession接口与之配套使用, 负责内存释放。
 * @param one_class_svcParams[IN] 用户传入训练参数
 * @param config[IN] 用户传入创建会话参数
 * @param pMachineLearning[IN/OUT] 用户持有的MachineLearning句柄地址，为二级指针
 * @retval #CML_OK 成功
 * @retval #CML_CPU_CWRAPPER_NULLPTR_ERROR 失败，传入的参数为空
 * @retval #CML_CPU_CWRAPPER_CREATE_CONTEXT_ERROR 失败，创建DOPAI_CML_Context失败
 * @retval #CML_CPU_C_CREATE_GPR_ERROR 失败，创建GPR对象失败
 * @retval #CML_CPU_C_GPR_LOAD_MODEL_ERROR 失败，生成模型失败
 * @retval #CML_CPU_C_GPR_CREATE_SESSION_ERROR 失败，创建会话失败
 * @retval #CML_CPU_CWRAPPER_MODEL_TYPE_ERROR 失败，模型不支持
 * @retval #CML_CPU_CWRAPPER_CREATE_SESSIONOUTPUT_ERROR 失败，创建sessionOutput失败
 * @retval #CML_CPU_CWRAPPER_CREATE_NDDATA_ERROR 失败，创建数据集失败
 *
 * @par 依赖: <ul><li>cml_c_one_class_svc.h:该接口所在的头文件。</li></ul>
 * @since V100R021
 * @see 无
 */
int32_t DOPAI_PUBLIC DOPAI_CML_ONE_CLASS_SVC_LoadModel(DOPAI_CML_Context **pMachineLearning,
    const DOPAI_CML_ONE_CLASS_SVC_Params *one_class_svcParams, const ScheduleConfig *config);

#ifdef __cplusplus
}
#endif

#endif