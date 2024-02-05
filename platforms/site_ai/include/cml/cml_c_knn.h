/*******************************************************************************
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 *
 * -------------------------------------------------------------------------------
 *
 * cml_c_knn.h
 *
 * Project Code: DOPAI
 * Module Name: Executor
 * Date Created: 2020-11-30
 * Author:
 * Description: definition of c API
 *
 * -------------------------------------------------------------------------------
 * Modification History
 * DATE          NAME                       DESCRIPTION
 * -----------------------------------------------------------------------------
 *
 *******************************************************************************/

#ifndef DOPAI_CML_C_KNN_H_INCLUDE
#define DOPAI_CML_C_KNN_H_INCLUDE

#include <stddef.h>
#include <stdint.h>
#include "util_forward_type.h"
#include "util_sal_compiler_reg.h"
#include "cml_c_wrapper.h"

/** @defgroup cml_c_knn CML-KNN C接口 */
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @ingroup cml_c_knn
 *
 * Tag_DOPAI_CML_C_KNN_Params: C-API接口下，用户训练配置结构体。
 *
 */
typedef struct Tag_DOPAI_CML_C_KNN_Params {
    const char *fitMethod;  /**< should be in ["brute", "kd_tree", "ball_tree"], need to end with '/0' */
    uint8_t p;      /**< should be in [0, 10].
                     * 0 means chebyshev distance, 1 means manhattan distance, 2 means euclidean,
                     * others means minkowski distance.
                     * We recommend normalizing the train data.
                     * And a larger value of p (greater than 3) may cause the accuracy of the algorithm to decrease. */
    const char *weights;   /**< should be in ["uniform", "distance"], need to end with '/0' */
    uint32_t nNeighbors;                /**< should be in [1, 500] */
    uint32_t leafSize;                  /**< should be greater than 0 */
} DOPAI_CML_C_KNN_Params;

/**
 * @ingroup cml_c_knn
 *
 * Tag_CML_C_KNN_Output: C-API接口下，用户获取推理结果的结构体。
 *
 */
typedef struct Tag_CML_C_KNN_Output {
    CML_CWrapper_Output cmlOutput; /**< CML基础推理结果 */
    uint32_t nNeighbors;           /**< K近邻数量 */
    const float *topKDistance;     /**< Top K样本距离 */
    const uint32_t *topKIndex;     /**< Top K样本索引 */
    const int32_t *topKLabel;      /**< Top K样本标签 */
} CML_C_KNN_Output;

/**
 * @ingroup cml_c_knn
 * @brief
 * 获取模型推理的结果。
 * @par 描述：
 * 执行成功后，模型推理的结果保存在output。
 * @attention
 * output指针本身由用户申请用户释放。
 * output各成员用户无需为其申请内存以及初始化。
 * 传入output各成员指针均为空，获取推理结果是将pMachineLearning保存推理结果的指针赋值给output各成员，二者共享同一片内存，
 * 因此用户一旦调用DOPAI_CML_ReleaseSession接口，将无法通过output获取推理结果。
 * 若返回值不为CML_OK，则需调用DOPAI_CML_ReleaseSession接口释放之前申请的内存。
 * @param output[IN] 用户传入数据结构，用于保存推理结果
 * @param pMachineLearning[IN/OUT] 用户持有的MachineLearning句柄地址，为二级指针
 * @retval #CML_OK 成功
 * @retval #CML_CPU_CWRAPPER_NULLPTR_ERROR 失败，传入的output为空或者pNeuralNetwork为空或者sessionOutput为空
 *
 * @par 依赖: <ul><li>cml_c_knn.h:该接口所在的头文件。</li></ul>
 * @since V100R020
 * @see 无
 */
int32_t DOPAI_PUBLIC DOPAI_CML_KNN_GetResults(const DOPAI_CML_Context *pMachineLearning, CML_C_KNN_Output *output);


/**
 * @ingroup cml_c_knn
 * @brief
 * 根据传入参数创建句柄，生成模型以及创建会话。
 * @par 描述：
 * 基于创建的会话进行后续数据填入和训练。
 * @attention
 * 该接口需要DOPAI_CML_ReleaseSession接口与之配套使用, 负责内存释放。
 * @param knnParams[IN] 用户传入训练参数
 * @param config[IN] 用户传入创建会话参数
 * @param pMachineLearning[IN/OUT] 用户持有的MachineLearning句柄地址，为二级指针
 * @retval #CML_OK 成功
 * @retval #CML_CPU_CWRAPPER_NULLPTR_ERROR 失败，传入的参数为空
 * @retval #CML_CPU_CWRAPPER_CREATE_CONTEXT_ERROR 失败，创建DOPAI_CML_Context失败
 * @retval #CML_CPU_CWRAPPER_LOAD_MODEL_ERROR 失败，生成模型失败
 * @retval #CML_CPU_CWRAPPER_CREATESESSION_ERROR 失败，创建会话失败
 * @retval #CML_CPU_CWRAPPER_MODEL_TYPE_ERROR 失败，模型不支持
 * @retval #CML_CPU_CWRAPPER_CREATE_SESSIONOUTPUT_ERROR 失败，创建sessionOutput失败
 * @retval #CML_CPU_CWRAPPER_CREATE_NDDATA_ERROR 失败，创建数据集失败
 *
 * @par 依赖: <ul><li>cml_c_knn.h:该接口所在的头文件。</li></ul>
 * @since V100R021
 * @see 无
 */
int32_t DOPAI_PUBLIC DOPAI_CML_KNN_LoadModel(DOPAI_CML_Context **pMachineLearning,
    const DOPAI_CML_C_KNN_Params *knnParams, const ScheduleConfig *config);
#ifdef __cplusplus
}
#endif

#endif