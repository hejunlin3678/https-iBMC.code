/*******************************************************************************
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 *
 * -------------------------------------------------------------------------------
 *
 * cml_c_random_forest.h
 *
 * Project Code: DOPAI
 * Module Name: Executor
 * Date Created: 2021-07-12
 * Author:
 * Description: definition of c API
 *
 * -------------------------------------------------------------------------------
 * Modification History
 * DATE          NAME                       DESCRIPTION
 * -----------------------------------------------------------------------------
 *
 *******************************************************************************/

#ifndef DOPAI_CML_C_RANDOM_FOREST_H_INCLUDE
#define DOPAI_CML_C_RANDOM_FOREST_H_INCLUDE
#include <stdbool.h>
#include "cml_c_wrapper.h"

/** @defgroup c_random_forest CML-Random_Forest C接口 */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup c_random_forest
 *
 * DOPAI_CML_Random_Forest_Regression_Params: C-API接口下，用户获取模型参数的结构体。
 *
 */
typedef struct Tag_DOPAI_CML_Random_Forest_Regression_Params {
    uint32_t numOfEstimators;                   /** number of estimators, choose between [1, 1000] */
    const char *criterion;                      /** choose between "mse" and "mae", need to end with '/0' */
    int32_t maxDepth;                           /** max tree depth, -1 means no limit, or choose between [1, 30]" */

    int32_t minSamplesSplitInt;                 /** choose between [2, 50000] */
    float minSamplesSplitFloat;                 /** choose between (0, 1] */

    int32_t minSamplesLeafInt;                  /** choose between [1, 50000] */
    float minSamplesLeafFloat;                  /** choose between (0, 1] */

    float minWeightFractionLeaf;                /** choose between [0, 1] */

    int32_t maxFeaturesInt;                     /** choose between [1, 3000] */
    float maxFeaturesFloat;                     /** choose between (0, 1] */
    const char *maxFeaturesString;              /** choose between "auto", "sqrt" and "log2", need to end with '/0' */

    int32_t maxLeafNodes;                       /** max leaf nodes, -1 means no limit, or choose between [1, 50000]" */
    float minImpurityDecrease;                  /** choose between [0, max range of float) */
    bool bootstrap;                             /** Whether bootstrap samples are used when building trees. If False,
                                                the whole dataset is used to build each tree. */
    int32_t randomState;                        /** Controls both the randomness of the bootstrapping of the samples
                                                used when building trees (if bootstrap==true) and the sampling of the
                                                features to consider when looking for the best split at each node (if
                                                max_features < n_features).  */
    float ccpAlpha;                             /** Complexity parameter used for Minimal Cost-Complexity Pruning.
                                                The subtree with the largest cost complexity that is smaller than
                                                ccpAlpha will be chosen. */
    int32_t maxSamplesInt;                      /** max samples used, -1 means no limit, or choose between [1, 50000] */
    float maxSamplesFloat;                      /** choose between (0, 1] */
} DOPAI_CML_Random_Forest_Regression_Params;

/**
 * @ingroup c_random_forest
 *
 * DOPAI_CML_Random_Forest_Classification_Params: C-API接口下，用户获取模型参数的结构体。
 *
 */
typedef struct Tag_DOPAI_CML_Random_Forest_Classification_Params {
    uint32_t numOfEstimators;                   /** number of estimators, choose between [1, 1000] */
    const char *criterion;                      /** choose between "gini" and "entropy", need to end with '/0' */
    int32_t maxDepth;                           /** max tree depth, -1 means no limit, or choose between [1, 30]" */

    int32_t minSamplesSplitInt;                 /** choose between [2, 50000] */
    float minSamplesSplitFloat;                 /** choose between (0, 1] */

    int32_t minSamplesLeafInt;                  /** choose between [1, 50000] */
    float minSamplesLeafFloat;                  /** choose between (0, 1] */

    float minWeightFractionLeaf;                /** choose between [0, 1] */
    int32_t maxSamplesInt;                      /** max samples used, -1 means no limit, or choose between [1, 50000] */
    float maxSamplesFloat;                      /** choose between (0, 1] */
    int32_t maxFeaturesInt;                     /** choose between [1, 3000] */
    float maxFeaturesFloat;                     /** choose between (0, 1] */
    const char *maxFeaturesString;              /** choose between "auto", "sqrt" and "log2", need to end with '/0' */

    int32_t maxLeafNodes;                       /** max leaf nodes, -1 means no limit, or choose between [1, 50000]" */
    float minImpurityDecrease;                  /** choose between [0, max range of float) */
    bool bootstrap;                             /** Whether bootstrap samples are used when building trees. If False,
                                                the whole dataset is used to build each tree. */
    int32_t randomState;                        /** Controls both the randomness of the bootstrapping of the samples
                                                used when building trees (if bootstrap==true) and the sampling of the
                                                features to consider when looking for the best split at each node (if
                                                max_features < n_features).  */
    float ccpAlpha;                             /** Complexity parameter used for Minimal Cost-Complexity Pruning.
                                                The subtree with the largest cost complexity that is smaller than
                                                ccpAlpha will be chosen. */
} DOPAI_CML_Random_Forest_Classification_Params;

/**
 * @ingroup c_random_forest
 * @brief
 * 根据传入参数创建句柄，生成Random_Forest分类模型以及创建会话。
 * @par 描述：
 * 基于创建的会话进行后续数据填入和训练。
 * @attention
 * 该接口需要DOPAI_CML_ReleaseSession接口与之配套使用, 负责内存释放。
 * @param params[IN] 用户传入训练参数
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
 * @par 依赖: <ul><li>cml_c_random_forest.h:该接口所在的头文件。</li></ul>
 * @since V100R021
 * @see 无
 */
int32_t DOPAI_PUBLIC DOPAI_CML_Random_Forest_Classification_LoadModel(DOPAI_CML_Context **pMachineLearning,
    const DOPAI_CML_Random_Forest_Classification_Params *params, const ScheduleConfig *config);

/**
 * @ingroup c_random_forest
 * @brief
 * 根据传入参数创建句柄，生成Random_Forest回归模型以及创建会话。
 * @par 描述：
 * 基于创建的会话进行后续数据填入和训练。
 * @attention
 * 该接口需要DOPAI_CML_ReleaseSession接口与之配套使用, 负责内存释放。
 * @param params[IN] 用户传入训练参数
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
 * @par 依赖: <ul><li>cml_c_random_forest.h:该接口所在的头文件。</li></ul>
 * @since V100R021
 * @see 无
 */
int32_t DOPAI_PUBLIC DOPAI_CML_Random_Forest_Regression_LoadModel(DOPAI_CML_Context **pMachineLearning,
    const DOPAI_CML_Random_Forest_Regression_Params *params, const ScheduleConfig *config);

#ifdef __cplusplus
}
#endif

#endif