/*******************************************************************************
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 *
 * -------------------------------------------------------------------------------
 *
 * cml_c_pca.h
 *
 * Project Code: DOPAI
 * Module Name: c_wrapper
 * Date Created: 2021-11-3
 * Author:
 * Description: definition of pca c API
 *
 * -------------------------------------------------------------------------------
 * Modification History
 * DATE          NAME                       DESCRIPTION
 * -----------------------------------------------------------------------------
 *
 *******************************************************************************/

#ifndef DOPAI_CML_C_PCA_H_INCLUDE
#define DOPAI_CML_C_PCA_H_INCLUDE

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "util_forward_type.h"
#include "util_sal_compiler_reg.h"
#include "cml_c_wrapper.h"

/* * @defgroup cml_c_pca CML-PCA C接口 */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup cml_c_pca
 *
 * Tag_DOPAI_CML_C_PCA_Output: C-API接口下，用户获取推理结果的结构体。
 *
 */
typedef struct Tag_DOPAI_CML_C_PCA_Output {
    const float *singularValues; /**< 数据的奇异值, 推理过程该字段为空 */
    const float *reducedDimData; /**< 降维后的数据, 按一维数组展开 */
    uint32_t componentNum;       /**< 数据降维后的特征数量, 推理过程为0，无意义 */
} DOPAI_CML_C_PCA_Output;

/**
 * @ingroup cml_c_pca
 *
 * Tag_DOPAI_CML_PCA_Params: C-API接口下，用户设置PCA算法参数的结构体。
 *
 */
typedef struct Tag_DOPAI_CML_PCA_Params {
    uint32_t nComponents; /**< number of components to keep, 0 means auto. */
    bool isMean;          /**< whether the input data is zero mean. */
    bool isWhiten;        /**< whether whiten the result. */
} DOPAI_CML_PCA_Params;

/**
 * @ingroup cml_c_pca
 * @brief
 * 根据传入参数创建句柄，生成模型以及创建会话。
 * @par 描述：
 * 基于创建的会话进行后续数据填入和训练。
 * @attention
 * 该接口需要DOPAI_CML_ReleaseSession接口与之配套使用, 负责内存释放。
 * @param pcaParams[IN] 用户传入训练参数
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
 * @par 依赖: <ul><li>cml_c_pca.h:该接口所在的头文件。</li></ul>
 * @since V100R021
 * @see 无
 */
int32_t DOPAI_PUBLIC DOPAI_CML_PCA_LoadModel(DOPAI_CML_Context **pMachineLearning,
    const DOPAI_CML_PCA_Params *pcaParams, const ScheduleConfig *config);

/**
 * @ingroup cml_c_pca
 * @brief
 * 获取训练结果。
 * @par 描述：
 * output指针本身由用户申请用户释放。
 * output各成员用户无需为其申请内存以及初始化。
 * 传入output各成员指针均为空，获取推理结果是将pMachineLearning保存推理结果的指针赋值给output各成员，二者共享同一片内存，
 * 因此用户一旦调用DOPAI_CML_ReleaseSession接口，将无法通过output获取推理结果。
 * 若返回值不为CML_OK，则需调用DOPAI_CML_ReleaseSession接口释放之前申请的内存。
 * @attention
 * @param output[IN] 用户传入用于保存推理结果
 * @param pMachineLearning[IN/OUT] 用户持有的MachineLearning句柄地址，为二级指针
 * @retval #CML_OK 成功
 * @retval #CML_CPU_CWRAPPER_NULLPTR_ERROR 失败，传入的参数为空
 * @retval #CML_CPU_CWRAPPER_MODEL_TYPE_ERROR 失败，模型类型错误
 *
 * @par 依赖: <ul><li>cml_c_pca.h:该接口所在的头文件。</li></ul>
 * @since V100R021
 * @see 无
 */
int32_t DOPAI_PUBLIC DOPAI_CML_PCA_GetFitResults(const DOPAI_CML_Context *pMachineLearning,
    DOPAI_CML_C_PCA_Output *output);

/**
 * @ingroup cml_c_pca
 * @brief
 * 获取转换结果。
 * @par 描述：
 * output指针本身由用户申请用户释放。
 * output各成员用户无需为其申请内存以及初始化。
 * 传入output各成员指针均为空，获取推理结果是将pMachineLearning保存推理结果的指针赋值给output各成员，二者共享同一片内存，
 * 因此用户一旦调用DOPAI_CML_ReleaseSession接口，将无法通过output获取推理结果。
 * 若返回值不为CML_OK，则需调用DOPAI_CML_ReleaseSession接口释放之前申请的内存。
 * @attention
 * @param output[IN] 用户传入用于保存推理结果
 * @param pMachineLearning[IN/OUT] 用户持有的MachineLearning句柄地址，为二级指针
 * @retval #CML_OK 成功
 * @retval #CML_CPU_CWRAPPER_NULLPTR_ERROR 失败，传入的参数为空
 * @retval #CML_CPU_CWRAPPER_MODEL_TYPE_ERROR 失败，模型类型错误
 *
 * @par 依赖: <ul><li>cml_c_pca.h:该接口所在的头文件。</li></ul>
 * @since V100R021
 * @see 无
 */
int32_t DOPAI_PUBLIC DOPAI_CML_PCA_GetTransformResults(const DOPAI_CML_Context *pMachineLearning,
    DOPAI_CML_C_PCA_Output *output);

#ifdef __cplusplus
}
#endif

#endif