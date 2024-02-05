/*******************************************************************************
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 *
 * -------------------------------------------------------------------------------
 *
 * cml_c_xgboost.h
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
 * ******************************************************************************/

#ifndef DOPAI_CML_C_XGBOOST_H_INCLUDE
#define DOPAI_CML_C_XGBOOST_H_INCLUDE

#include "cml_c_wrapper.h"
#include "cml_algorithm_loss.h"

/** @defgroup c_xgboost CML-XGBoost C接口 */
#ifdef __cplusplus
extern "C"
{
#endif

typedef struct Tag_DOPAI_CML_C_XGBoost_Output {
    CML_CWrapper_Output cmlOutput;         /**< CML基础推理结果 */
    float loss;                     /**< 损失 */
} DOPAI_CML_C_XGBoost_Output;

/**
 * @ingroup c_xgboost
 *
 * DOPAI_CML_XGBoost_Params: C-API接口下，用户获取模型参数的结构体。
 *
 */
typedef struct Tag_DOPAI_CML_XGBoost_Params {
    // method of data preprocess. Hist means using Histogram XGB method, need to end with '/0'
    const char *xgbTreeMethod;
    // growing method of tree.
    const char *xgbGrowPolicy; // choose in ["DepthWise", "LossGuide"], need to end with '/0'
    // Training Data Distribute: Sparse or Dense
    const char *xgbDataDistribute; // choose in ["Dense", "Sparse"], need to end with '/0'
    // While xgbGrowPolicy="LossGuide", if cleanCache is set as false, algorithm will not clean catch memory.
    // As a result, algorithm will be faster and memory usage will increase. Vice versa.
    bool cleanCache;
    // max histogram of each feature， enable while xgbTreeMethod=Hist
    uint32_t maxBin;
    // learning step size for a time
    float learningRate;
    // maximum depth of a tree
    uint8_t maxDepth;
    // maximum leaf node of a tree
    uint32_t maxLeaves;
    // minimum amount of hessian(weight) allowed in a child
    float minChildWeight;
    // minimum amount of Data allowed in a child
    uint32_t minDataInLeaf;
    // L2 regularization factor
    float regLambda;
    // L1 regularization factor
    float regAlpha;
    int numberOfTrees;
    // the random seed used in trainning session;
    // you can set a const seed which ">= 0" to get exact same trainning results
    // "< 0" will use a seed based on current time;
    int seed;
    // 0 < featureFraction <= 1.0; set 1.0 to disable
    float featureFraction;
    // 0 < baggingFraction <= 1.0; set 1.0 to disable
    float baggingFraction;
    // "0" means disable bagging, "k" means perform bagging at every "k" iteration
    uint32_t baggingFreq;
    float priors;
    int32_t objectiveFunc;
    int32_t evalMetricFunc;
    // will stop training if one metric of one validation data doesn't improve in last ``earlyStoppingRounds`` rounds
    // ``= 0`` will only do the metric but not stop
    uint32_t earlyStoppingRounds;
    uint32_t nClasses;
} DOPAI_CML_XGBoost_Params;

 /**
 * @ingroup c_xgboost
 * @brief
 * 根据传入参数创建句柄，生成XGBoost分类模型以及创建会话。
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
 * @par 依赖:
 * <ul><li>cml_c_xgboost.h:该接口所在的头文件。</li></ul>
 * @since V100R021
 * @see 无
 */
int32_t DOPAI_PUBLIC DOPAI_CML_XGBoostC_LoadModel(DOPAI_CML_Context **pMachineLearning,
    const DOPAI_CML_XGBoost_Params *params, const ScheduleConfig *config);

 /**
 * @ingroup c_xgboost
 * @brief
 * 根据传入参数创建句柄，生成XGBoost回归模型以及创建会话。
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
 * @par 依赖:
 * <ul><li>cml_c_xgboost.h:该接口所在的头文件。</li></ul>
 * @since V100R021
 * @see 无
 */
int32_t DOPAI_PUBLIC DOPAI_CML_XGBoostR_LoadModel(DOPAI_CML_Context **pMachineLearning,
    const DOPAI_CML_XGBoost_Params *params, const ScheduleConfig *config);

/**
 * @ingroup c_xgboost
 * @brief
 * 获取推理结果。
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
 * @par 依赖:
 * <ul><li>cml_c_xgboost.h:该接口所在的头文件。</li></ul>
 * @since V100R021
 * @see 无
 */
int32_t DOPAI_PUBLIC DOPAI_CML_XGBoost_GetResults(const DOPAI_CML_Context *pMachineLearning,
    DOPAI_CML_C_XGBoost_Output *output);

/**
 * @ingroup c_xgboost
 * @brief
 * 设置真值，根据GBR模型中的LossFunc，计算Validation结果。
 * @par 描述：
 * Set GroundTruth value for validation.
 * @attention
 * @param pMachineLearning[IN/OUT] 用户持有的MachineLearning句柄地址，为二级指针
 * @param data[IN] 真值，需与inputData一一对应
 * @param size[IN] element Length(bytes) of the data
 * @retval #CML_OK 成功
 * @retval #CML_CPU_CWRAPPER_NULLPTR_ERROR 失败，传入的参数为空
 * @retval #CML_CPU_XGBOOST_RESIZERESULTS_ERROR 失败，设置真值错误
 *
 * @par 依赖:
 * <ul><li>cml_c_xgboost.h:该接口所在的头文件。</li></ul>
 * @since V100R021
 * @see 无
 */
int32_t DOPAI_PUBLIC DOPAI_CML_XGBoost_SetTrueValue(const DOPAI_CML_Context *pMachineLearning, const float &data,
    uint32_t size);

#ifdef __cplusplus
}
#endif

#endif