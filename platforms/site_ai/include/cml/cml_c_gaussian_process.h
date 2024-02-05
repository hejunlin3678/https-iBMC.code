/*******************************************************************************
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 *
 * -------------------------------------------------------------------------------
 *
 * cml_c_gaussian_process.h
 *
 * Project Code: DOPAI
 * Module Name: Machine Learning
 * Date Created: 2021-07-21
 * Author:
 * Description: definition of gpr-c API
 *
 * -------------------------------------------------------------------------------
 * Modification History
 * DATE          NAME                       DESCRIPTION
 * -----------------------------------------------------------------------------
 *
 * ******************************************************************************/

#ifndef DOPAI_CML_C_GAUSSIAN_PROCESS_H_INCLUDE
#define DOPAI_CML_C_GAUSSIAN_PROCESS_H_INCLUDE

#include "cml_c_wrapper.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct Tag_DOPAI_CML_C_GPR_Output {
    CML_CWrapper_Output cmlOutput;         /**< CML基础推理结果 */
    const double *cov;                      /**< 方差 */
} DOPAI_CML_C_GPR_Output;

typedef struct Tag_DOPAI_CML_GP_Rprop {
    // optimization will be stopped when 2-norm of hyper parameters gradient less than epsStop
    double epsStop;
    // initial value of learningrate, should be in (0, 1)
    double deltaStart;
    // the minimum value of learningrate to prevent underflow, should be in [1e-7, 1e-4]
    double deltaMin;
    // the maximum value of learningrate to prevent overflow, should be in [5, 20]
    double deltaMax;
    // determines how much learningrate will be updated in negative direction, should be in (0, 1)
    double etaMinus;
    // determines how much learningrate will be updated in positive direction, should be in (1, 3)
    double etaPlus;
    // maximum number of iterations, shuould be in [1, 10000]
    uint32_t maxStep;
} DOPAI_CML_GP_Rprop;

typedef enum DOPAI_CML_GP_KernelType {
    DOPAI_CML_GP_RBFKernel = 0,
    DOPAI_CML_GP_LinearKernel = 1,
    DOPAI_CML_GP_NoiseKernel = 2
} DOPAI_CML_GP_KernelType;

typedef struct Tag_DOPAI_CML_GP_KernelParams {
    // for all kernels, initial hypper parameters should be in [0, 1]
    double (*rbfKernelParams)[2];
    size_t rbfKernelCounts;
    double (*linearKernelParams)[1];
    size_t linearKernelCounts;
    double (*noiseKernelParams)[1];
    size_t noiseKernelCounts;
} DOPAI_CML_GP_KernelParams;

typedef struct Tag_DOPAI_CML_GPR_Params {
    DOPAI_CML_GP_KernelType *kernel;                /**< 计算协方差矩阵采用的核, 至少提供一个 */
    size_t kernelCounts;                            /**< 核的数量 */
    DOPAI_CML_GP_Rprop *rpropParams;                /**< 用于优化核函数超参的rprop迭代参数 */
    DOPAI_CML_GP_KernelParams *kernelParams;        /**< 核函数超参的初始值 */
} DOPAI_CML_GPR_Params;

/**
 * @ingroup cml_c_gaussian_process
 * @brief
 * 根据传入参数创建句柄，生成模型以及创建会话。
 * @par 描述：
 * 基于创建的会话进行后续数据填入和训练。
 * @attention
 * 该接口需要DOPAI_CML_ReleaseSession接口与之配套使用, 负责内存释放。
 * @param gprParams[IN] 用户传入训练参数
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
 * @par 依赖:
 * <ul><li>cml_c_gaussian_process.h:该接口所在的头文件。</li></ul>
 * @since V100R021
 * @see 无
 */
int32_t DOPAI_PUBLIC DOPAI_CML_GPR_LoadModel(DOPAI_CML_Context **pMachineLearning,
    const DOPAI_CML_GPR_Params *gprParams, const ScheduleConfig *config);

/**
 * @ingroup cml_c_gaussian_process
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
 * <ul><li>cml_c_gaussian_process.h:该接口所在的头文件。</li></ul>
 * @since V100R021
 * @see 无
 */
int32_t DOPAI_PUBLIC DOPAI_CML_GPR_GetResults(const DOPAI_CML_Context *pMachineLearning,
    DOPAI_CML_C_GPR_Output *output);

#ifdef __cplusplus
}
#endif

#endif