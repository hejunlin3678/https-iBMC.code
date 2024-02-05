/*******************************************************************************
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 *
 * -------------------------------------------------------------------------------
 *
 * cml_c_wrapper.h
 *
 * Project Code: DOPAI
 * Module Name: Executor
 * Date Created: 2020-07-06
 * Author:
 * Description: definition of c API
 *
 * -------------------------------------------------------------------------------
 * Modification History
 * DATE          NAME                       DESCRIPTION
 * -----------------------------------------------------------------------------
 *
 * ******************************************************************************/

#ifndef DOPAI_CML_C_WRAPPER_H_INCLUDE
#define DOPAI_CML_C_WRAPPER_H_INCLUDE

#include <stddef.h>
#include <stdint.h>
#include "cml_common.h"
#include "util_forward_type.h"
#include "util_sal_compiler_reg.h"

/** @defgroup cml_c_wrapper Machine Learning C-API */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup cml_c_wrapper
 *
 * DOPAI_CML_Context: C-API接口下，用户持有的机器学习结构体。
 *
 */
typedef struct TagMachineLearning DOPAI_CML_Context;

/**
 * @ingroup cml_c_wrapper
 *
 * CML_CWrapper_Output: C-API接口下，用户获取推理结果的结构体。
 *
 */
typedef struct Tag_CML_CWrapper_Output {
    const int32_t *label;            /**< 分类标签 */
    const uint8_t *prob;             /**< 分类概率 */
    const uint8_t *value;            /**< 回归结果 */
    const uint32_t *nClasses;        /**< 分类标签数量 */
    size_t nSample;                  /**< 样本数量 */
    size_t nTarget;                  /**< 目标数量(用于多目标推理，预留) */
    DOPAI_CML_DataType type;         /**< 分类时输出的分类概率以及回归时输出的回归结果的数据类型 */
} CML_CWrapper_Output;
/**
 * @ingroup cml_c_wrapper
 *
 * CML_CWrapper_Input: C-API接口下，用户传入推理数据的结构体。
 *
 */
typedef struct Tag_CML_CWrapper_Input {
    uint8_t *data;                                /**< 输入数据 */
    uint8_t *target;                              /**< 输入目标 */
    uint32_t *dataShape;                          /**< 输入数据维度信息 */
    uint32_t *targetShape;                        /**< 输入目标维度信息 */
    size_t dataDim;                               /**< 输入数据维度大小 */
    size_t targetDim;                             /**< 输入目标维度大小 */
    DOPAI_CML_DataType dataType;                  /**< 输入数据类型 */
    DOPAI_CML_DataType targetType;                /**< 输入目标类型 */
} CML_CWrapper_Input;

/**
 * @ingroup cml_c_wrapper
 * @brief
 * 填入用户传入的训练数据和验证数据，用于模型训练和验证。
 * @par 描述：
 * 执行成功后可基于写入的数据进行训练和验证操作。
 * @attention
 * 调用该接口后，所申请的内存由DOPAI_CML_ReleaseSession释放。
 * 用户负责构造正确的输入数据fitInput和evalInput。
 * @param fitInput[IN] 用户传入训练数据
 * @param evalInput[IN] 用户传入验证数据，如果不需要验证集，则传入空指针
 * @param pMachineLearning[IN/OUT] 用户持有的MachineLearning句柄地址，为二级指针
 * @retval #CML_OK 成功
 * @retval #CML_CPU_CWRAPPER_NULLPTR_ERROR 失败，传入的input为空或者pNeuralNetwork为空
 * @retval #CML_CPU_CWRAPPER_CREATE_NDDATA_ERROR 失败，创建数据集失败
 * @retval #CML_CPU_CWRAPPER_RESIZE_ERROR 失败，为数据集分配空间失败
 * @retval #CML_FEED_DATA_ERROR 失败，将用户传入数据数据写入数据集失败
 *
 * @par 依赖:
 * <ul><li>cml_c_wrapper.h:该接口所在的头文件。</li></ul>
 * @since V100R021
 * @see 无
 */
int32_t DOPAI_PUBLIC DOPAI_CML_FeedFitInput(DOPAI_CML_Context *pMachineLearning, const CML_CWrapper_Input *fitInput,
    const CML_CWrapper_Input *evalInput);

/**
 * @ingroup cml_c_wrapper
 * @brief
 * 执行训练。
 * @par 描述：
 * 有监督且不带验证集的训练接口。
 * @attention
 * @param func[IN] 回调函数，如果不需要验证集，则传入空指针
 * @param pMachineLearning[IN/OUT] 用户持有的MachineLearning句柄地址，为二级指针
 * @retval #CML_OK 成功
 * @retval #CML_CPU_CWRAPPER_NULLPTR_ERROR 失败，传入pNeuralNetwork为空
 *
 * @par 依赖:
 * <ul><li>cml_c_wrapper.h:该接口所在的头文件。</li></ul>
 * @since V100R021
 * @see 无
 */
int32_t DOPAI_PUBLIC DOPAI_CML_FitSession(const DOPAI_CML_Context *pMachineLearning, DOPAICMLCallback func);

/**
 * @ingroup cml_c_wrapper
 * @brief
 * 执行训练。
 * @par 描述：
 * 有监督带验证集的训练接口。
 * @attention
 * @param func[IN] 回调函数，如果不需要验证集，则传入空指针
 * @param pMachineLearning[IN/OUT] 用户持有的MachineLearning句柄地址，为二级指针
 * @retval #CML_OK 成功
 * @retval #CML_CPU_CWRAPPER_NULLPTR_ERROR 失败，传入pNeuralNetwork为空
 *
 * @par 依赖:
 * <ul><li>cml_c_wrapper.h:该接口所在的头文件。</li></ul>
 * @since V100R021
 * @see 无
 */
int32_t DOPAI_PUBLIC DOPAI_CML_FitSessionWithEval(const DOPAI_CML_Context *pMachineLearning, DOPAICMLCallback func);

/**
 * @ingroup cml_c_wrapper
 * @brief
 * 执行训练。
 * @par 描述：
 * 无监督训练接口。
 * @attention
 * @param pMachineLearning[IN/OUT] 用户持有的MachineLearning句柄地址，为二级指针
 * @retval #CML_OK 成功
 * @retval #CML_CPU_CWRAPPER_NULLPTR_ERROR 失败，传入pNeuralNetwork为空
 *
 * @par 依赖:
 * <ul><li>cml_c_wrapper.h:该接口所在的头文件。</li></ul>
 * @since V100R021
 * @see 无
 */
int32_t DOPAI_PUBLIC DOPAI_CML_FitSessionUnsupervised(const DOPAI_CML_Context *pMachineLearning);

/**
 * @ingroup cml_c_wrapper
 * @brief
 * 执行训练。
 * @par 描述：
 * 无监督训练加推理接口。
 * @attention
 * @param pMachineLearning[IN/OUT] 用户持有的MachineLearning句柄地址，为二级指针
 * @retval #CML_OK 成功
 * @retval #CML_CPU_CWRAPPER_NULLPTR_ERROR 失败，传入pNeuralNetwork为空
 *
 * @par 依赖:
 * <ul><li>cml_c_wrapper.h:该接口所在的头文件。</li></ul>
 * @since V100R021
 * @see 无
 */
int32_t DOPAI_PUBLIC DOPAI_CML_FitPredictSession(const DOPAI_CML_Context *pMachineLearning);

/**
 * @ingroup cml_c_wrapper
 * @brief
 * 将用户传入的数据写入sessionInput，最终用于模型推理的输入。
 * @par 描述：
 * 执行成功后可基于写入的数据进行相应推理操作。
 * @attention
 * 调用该接口后，所申请的内存由DOPAI_CML_ReleaseSession释放。
 * 用户负责构造正确的输入数据input。
 * @param input[IN] 用户传入数据
 * @param pMachineLearning[IN/OUT] 用户持有的MachineLearning句柄地址，为二级指针
 * @retval #CML_OK 成功
 * @retval #CML_CPU_CWRAPPER_NULLPTR_ERROR 失败，传入的input为空或者pNeuralNetwork为空
 * @retval #CML_CPU_CWRAPPER_INPUTDIM_ERROR 失败，传入的input维度信息不合法
 * @retval #CML_CPU_CWRAPPER_CREATE_NDDATA_ERROR 失败，创建sessionInput失败
 * @retval #CML_CPU_CWRAPPER_RESIZE_ERROR 失败，为sessionInput分配空间失败
 * @retval #CML_FEED_DATA_ERROR 失败，将input数据写入sessionInput失败
 *
 * @par 依赖:
 * <ul><li>cml_c_wrapper.h:该接口所在的头文件。</li></ul>
 * @since V100R020
 * @see 无
 */
int32_t DOPAI_PUBLIC DOPAI_CML_FeedInput(DOPAI_CML_Context *pMachineLearning, const CML_CWrapper_Input *input);
/**
 * @ingroup cml_c_wrapper
 * @brief
 * 用户传入后端类型和线程数，创建模型推理的会话。
 * @par 描述：
 * 执行成功后，基于创建的会话进行相应推理操作。
 * @attention
 * 该接口需要DOPAI_CML_ReleaseSession接口与之配套使用, 负责内存释放。
 * @param file[IN] 模型的文件路径, 需要 '/0'结尾
 * @param config[IN] 用于传入的用于创建会话的参数，包括后端类型和线程数
 * @param pMachineLearning[IN/OUT] 用户持有的MachineLearning句柄地址，为二级指针
 * @retval #CML_OK 成功
 * @retval #CML_CPU_CWRAPPER_CREATE_CONTEXT_ERROR 失败，创建DOPAI_CML_Context失败
 * @retval #CML_CPU_CWRAPPER_MODEL_TYPE_ERROR 失败，获取模型类型失败
 * @retval #CML_CPU_CWRAPPER_CREATE_SESSIONOUTPUT_ERROR 失败，创建sessionOutput失败
 * @retval #CML_CPU_CWRAPPER_NULLPTR_ERROR 失败，传入的config为空或者pNeuralNetwork为空或者file为空
 * @retval #CML_CPU_CWRAPPER_CREATESESSION_ERROR 失败，创建会话失败
 * @retval #CML_CPU_CWRAPPER_LOAD_MODEL_ERROR 失败，句柄为空，DOPAI_CML_LoadFromFile加载模型失败
 *
 * @par 依赖:
 * <ul><li>cml_c_wrapper.h:该接口所在的头文件。</li></ul>
 * @since V100R020
 * @see 无
 */
int32_t DOPAI_PUBLIC DOPAI_CML_CreateSessionFromFile(DOPAI_CML_Context **pMachineLearning, const ScheduleConfig *config,
    const char *file);

/**
 * @ingroup cml_c_wrapper
 * @brief
 * 用户传入后端类型和线程数，从内存创建模型推理的会话。
 * @par 描述：
 * 执行成功后，基于创建的会话进行相应推理操作。
 * @attention
 * 该接口需要DOPAI_CML_ReleaseSession接口与之配套使用, 负责内存释放。
 * @param buffer[IN] 模型的二进制流
 * @param bufferSize[IN] 模型的二进制流大小
 * @param config[IN] 用于传入的用于创建会话的参数，包括后端类型和线程数
 * @param pMachineLearning[IN/OUT] 用户持有的MachineLearning句柄地址，为二级指针
 * @retval #CML_OK 成功
 * @retval #CML_CPU_CWRAPPER_CREATE_CONTEXT_ERROR 失败，创建DOPAI_CML_Context失败
 * @retval #CML_CPU_CWRAPPER_MODEL_TYPE_ERROR 失败，获取模型类型失败
 * @retval #CML_CPU_CWRAPPER_CREATE_SESSIONOUTPUT_ERROR 失败，创建sessionOutput失败
 * @retval #CML_CPU_CWRAPPER_NULLPTR_ERROR 失败，传入的config为空或者pNeuralNetwork为空或者file为空
 * @retval #CML_CPU_CWRAPPER_CREATESESSION_ERROR 失败，创建会话失败
 * @retval #CML_CPU_CWRAPPER_LOAD_MODEL_ERROR 失败，句柄为空，DOPAI_CML_LoadFromBuffer加载模型失败
 * @retval #CML_CPU_CWRAPPER_UNPACK_MODEL_ERROR 失败，从内存反序列化模型失败
 *
 * @par 依赖:
 * <ul><li>cml_c_wrapper.h:该接口所在的头文件。</li></ul>
 * @since V100R021
 * @see 无
 */
int32_t DOPAI_PUBLIC DOPAI_CML_CreateSessionFromBuffer(DOPAI_CML_Context **pMachineLearning,
    const ScheduleConfig *config, const uint8_t *buffer, size_t bufferSize);

/**
 * @ingroup cml_c_wrapper
 * @brief
 * 释放申请的内存。
 * @par 描述：
 * 无
 * @attention
 * 执行该方法后，用户持有的pMachineLearning置空，且其内存被释放。用户调用与其相关的任何接口均视为非法。
 * @param pMachineLearning[OUT] 用户持有的MachineLearning句柄地址，为二级指针
 * @retval #CML_OK 成功
 * @retval #CML_CPU_CWRAPPER_NULLPTR_ERROR 失败，传入的pNeuralNetwork为空
 *
 * @par 依赖:
 * <ul><li>cml_c_wrapper.h:该接口所在的头文件。</li></ul>
 * @since V100R020
 * @see 无
 */
int32_t DOPAI_PUBLIC DOPAI_CML_ReleaseSession(DOPAI_CML_Context **pMachineLearning);
/**
 * @ingroup cml_c_wrapper
 * @brief
 * 获取模型推理的结果。
 * @par 描述：
 * 执行成功后，模型推理的结果保存在output。
 * @attention
 * output指针本身由用户申请用户释放。
 * output各成员用户无需为其申请内存以及初始化。
 * 传入output各成员指针均为空，获取推理结果是将pMachineLearning保存推理结果的指针赋值给output各成员，二者共享同一片内存，
 * 因此用户一旦调用DOPAI_CML_ReleaseSession接口，将无法通过output获取推理结果。
 * @param output[IN] 用户传入数据结构，用于保存推理结果
 * @param pMachineLearning[IN/OUT] 用户持有的MachineLearning句柄地址，为二级指针
 * @retval #CML_OK 成功
 * @retval #CML_CPU_CWRAPPER_NULLPTR_ERROR 失败，传入的output为空或者pNeuralNetwork为空或者sessionOutput为空
 *
 * @par 依赖:
 * <ul><li>cml_c_wrapper.h:该接口所在的头文件。</li></ul>
 * @since V100R020
 * @see 无
 */
int32_t DOPAI_PUBLIC DOPAI_CML_GetResults(const DOPAI_CML_Context *pMachineLearning, CML_CWrapper_Output *output);
/**
 * @ingroup cml_c_wrapper
 * @brief
 * 执行模型推理。
 * @par 描述：
 * 无
 * @attention
 * @param pMachineLearning[IN/OUT] 用户持有的MachineLearning句柄地址，为二级指针
 * @retval #CML_NO_ERROR 成功
 * @retval #CML_CPU_CWRAPPER_NULLPTR_ERROR 失败，传入的pNeuralNetwork为空
 *
 * @par 依赖:
 * <ul><li>cml_c_wrapper.h:该接口所在的头文件。</li></ul>
 * @since V100R020
 * @see 无
 */
int32_t DOPAI_PUBLIC DOPAI_CML_RunSession(const DOPAI_CML_Context *pMachineLearning);

/**
 * @ingroup cml_c_wrapper
 * @brief
 * 获取待导出模型的大小。
 * @par 描述：
 * 获取序列化当前session存储的算法模型的大小
 * @attention
 * 需要先执行该方法后，获取到导出模型需要的大小，再分配内存，导出模型。
 * @param pMachineLearning[IN/OUT] 用户持有的MachineLearning句柄地址，为二级指针
 * @param modelSize[OUT] 待导出的模型大小
 * @retval #CML_NO_ERROR 成功
 * @retval #CML_CPU_CWRAPPER_NULLPTR_ERROR 失败，传入的pNeuralNetwork为空
 * @retval #CML_DUMP_MODEL_ERROR 失败，模型异常，无法获取模型大小
 * @par 依赖:
 * <ul><li>cml_c_wrapper.h:该接口所在的头文件。</li></ul>
 * @since V100R020
 * @see 无
 */
int32_t DOPAI_PUBLIC DOPAI_CML_GetDumpModelSize(const DOPAI_CML_Context *pMachineLearning, size_t *modelSize);
/**
 * @ingroup cml_c_wrapper
 * @brief
 * 执行算法模型的存储到内存功能
 * @par 描述：
 * 序列化存储当前session存储的算法模型于指定内存空间中
 * @attention
 * 由用户分配 modelSize 大小的 buffer 空间，用于模型导出
 * @param pMachineLearning[IN/OUT] 用户持有的MachineLearning句柄地址，为二级指针
 * @param buffer[OUT] 导出模型到缓存
 * @param modelSize[IN] 模型缓存空间大小
 * @retval #CML_NO_ERROR 成功
 * @retval #CML_CPU_CWRAPPER_NULLPTR_ERROR 失败，传入的pNeuralNetwork为空
 * @retval #CML_DUMP_MODEL_ERROR 失败，导出模型失败或buffer空间不足
 * @par 依赖:
 * <ul><li>cml_c_wrapper.h:该接口所在的头文件。</li></ul>
 * @since V100R020
 * @see 无
 */
int32_t DOPAI_PUBLIC DOPAI_CML_DumpModel(const DOPAI_CML_Context *pMachineLearning, uint8_t *buffer,
    const int32_t modelSize);
#ifdef __cplusplus
}
#endif

#endif
