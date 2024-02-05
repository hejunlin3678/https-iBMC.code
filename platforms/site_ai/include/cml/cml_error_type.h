/*******************************************************************************
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * -------------------------------------------------------------------------------
 *
 * cml_error_code.h
 *
 * Project Code: DOPAI
 * Module Name: machine_learning
 * Date Created: 2023-05-23
 * Author:
 * Description: CML错误码类型
 *
 * -------------------------------------------------------------------------------
 * Modification History
 * DATE          NAME                       DESCRIPTION
 * -----------------------------------------------------------------------------
 * 5/23/2023    WangJunfeng                 The lines of raw cml_error_code.h file are more than 500.
 *******************************************************************************/
#ifndef DOPAI_CML_ERRORTYPE_H
#define DOPAI_CML_ERRORTYPE_H

#ifdef __cplusplus
extern "C" {
#endif
/** @defgroup CMLErrorCode CML错误码说明 */
enum CMLErrorType {
    NO_ERROR_TMP = 0x00,                              // 无错误
    NO_EXECUTION = 0x01,                              // 所选算法不支持
    CPU_ADABOOST_CHECKPARAMS_ERROR = 0x02,            // AdaBoost参数错误
    CPU_ADABOOST_RESIZERESULT_ERROR = 0x03,           // AdaBoost的scores内存申请失败
    CPU_GBC_CHECKPARAMS_ERROR = 0x04,                 // GBC参数错误
    CPU_GBC_RESIZERESULTS_ERROR = 0x05,               // GBC输出resize错误
    CPU_GBR_CHECKPARAMS_ERROR = 0x06,                 // GBR参数错误
    CPU_GBR_RESIZERESULTS_ERROR = 0x07,               // GBR输出resize错误
    CPU_IFOREST_CHECKPARAMS_ERROR = 0x08,             // iForest参数错误
    CPU_IFOREST_RESIZERESULTS_ERROR = 0x09,           // iForest输出resize错误
    CPU_KMEANS_RESIZERESULTS_ERROR = 0x0A,            // KMeans输出resize错误
    CPU_KMEANS_CHECKPREDICTPARAS_ERROR = 0x0B,        // KMeans参数错误
    CPU_KMEANSTRAIN_CHECKFITPARAS_ERROR = 0x0C,       // KMeansTrain参数错误
    CPU_KMEANSTRAIN_RESIZERESULTS_ERROR = 0x0D,       // KMeansTrain输出resize错误
    CPU_KMEANSTRAIN_GETTOLERANCE_ERROR = 0x0E,        // KMeansTrain计算阈值tolerance错误
    CPU_KMEANSTRAIN_CREATEKMEANSPARA_ERROR = 0x0F,    // KMeansTrain创建KMeansPara失败
    CPU_KMEANSTRAIN_INITKMEANSTRAINPARA_ERROR = 0x10, // KMeansTrain初始化KMeansPara失败
    CPU_KMEANSTRAIN_INITCENTERS_ERROR = 0x11,         // KMeansTrain初始化中心点失败
    CPU_KMEANSTRAIN_RUNITERATIONS_ERROR = 0x12,       // KMeansTrain聚类失败
    CPU_KMEANSTRAIN_RECORDBESTRESULT_ERROR = 0x13,    // KMeansTrain记录最好结果失败
    CPU_LR_CHECKPARAMS_ERROR = 0x14,                  // LR参数错误
    CPU_LR_RESIZERESULTS_ERROR = 0x15,                // LR输出resize错误
    CPU_LR_COMPUTE_ERROR = 0x16,                      // LR计算结果错误
    CPU_RFC_CHECKPARAMS_ERROR = 0x17,                 // RFC参数错误
    CPU_RFC_RESIZERESULTS_ERROR = 0x18,               // RFC输出resize错误
    CPU_RFC_INITPROBA_ERROR = 0x19,                   // RFC初始化proba参数失败
    CPU_RFR_CHECKPARAMS_ERROR = 0x1A,                 // RFR参数错误
    CPU_RFR_RESIZERESULTS_ERROR = 0x1B,               // RFR输出resize错误
    CPU_SPC_CHECKPARAMS_ERROR = 0x1C,                 // SPC参数错误
    CPU_SPC_GETLAPMAT_ERROR = 0x1D,                   // SPC获取拉普拉斯矩阵错误
    CPU_SPC_GETLAMBDAK_ERROR = 0x1E,                  // SPC获取lambdak错误
    CPU_SPC_RESIZERESULTS_ERROR = 0x1F,               // SPC获取signalSize错误
    CPU_SPC_CALFLDRANDVEC_ERROR = 0x20,               // SPC获取特征矩阵错误
    CPU_PCAFIT_CHECKPARAMS_ERROR = 0x21,              // PCAFit参数错误
    CPU_PCAFIT_MEMORY_NULL = 0x22,                    // PCAFit申请内存失败
    CPU_PCAFIT_EIG_ERROR = 0x23,                      // PCAFit特征分解错误
    CPU_PCA_SGEMM_ERROR = 0x24,                       // PCAFit降维矩阵乘计算错误
    CPU_PCA_RESIZERESULTS_ERROR = 0x25,               // PCAFit输出resize错误
    CPU_PCATRANSFORM_CHECKPARAMS_ERROR = 0x26,        // PCATransform参数错误
    CPU_PCATRANSFORM_RESIZERESULTS_ERROR = 0x27,      // PCATransform输出resize错误
    CPU_DBSCAN_CHECKPARAMS_ERROR = 0x28,              // DBSCAN参数错误
    CPU_DBSCAN_RESIZERESULTS_ERROR = 0x29,            // DBSCAN输出resize错误
    CPU_DBSCAN_FITMETHOD_ERROR = 0x2A,                // DBSCAN无效fitMethod
    CPU_SVC_CHECKPARAMS_ERROR = 0x2C,                 // SVC参数错误
    CPU_SVC_RESIZERESULTS_ERROR = 0x2D,               // SVC输出resize错误
    CPU_SVC_GETCOMPUTESAMPLES_ERROR = 0x2E,           // SVC分割数据错误
    CPU_SVC_COMPUTEKERNEL_ERROR = 0x30,               // SVC核计算错误
    CPU_SVC_GETRESULTLABEL_ERROR = 0x31,              // SVC结果标签计算错误
    CPU_LINEARSVC_CHECKPARAMS_ERROR = 0x32,           // LinearSVC参数错误
    CPU_LINEARSVC_RESIZERESULTS_ERROR = 0x33,         // LinearSVC输出resize错误
    CPU_LINEARSVC_GETCOMPUTESAMPLES_ERROR = 0x34,     // LinearSVC分割数据错误
    CPU_LINEARSVC_COMPUTELINEARKERNEL_ERROR = 0x35,   // LinearSVC线性核计算错误
    CPU_LINEARSVC_GETRESULTLABEL_ERROR = 0x36,        // LinearSVC结果标签计算错误
    CPU_FEED_DATA_ERROR = 0x37,                       // 复制数据失败
    CPU_SPC_RESIZE_ERROR = 0x38,                      // SPC Resize失败
    CPU_CWRAPPER_NULLPTR_ERROR = 0x39,                // CWrapper输入错误
    CPU_CWRAPPER_CREATE_CONTEXT_ERROR = 0x3A,         // CWrapper实例化Context错误
    CPU_CWRAPPER_LOAD_MODEL_ERROR = 0x3B,             // CWrapper加载模型错误
    CPU_CWRAPPER_MODEL_TYPE_ERROR = 0X3C,             // CWrapper模型类型错误
    CPU_CWRAPPER_CREATE_NDDATA_ERROR = 0X3D,          // CWrapper实例化输入错误
    CPU_CWRAPPER_RESIZE_ERROR = 0X3E,                 // CWrapper输入resize错误
    CPU_CWRAPPER_CREATESESSION_ERROR = 0X3F,          // CWrapper创建会话错误
    CPU_CWRAPPER_INPUTDIM_ERROR = 0X40,               // CWrapper输入数据维度错误
    CPU_CWRAPPER_CREATE_SESSIONOUTPUT_ERROR = 0x41,   // CWrapper实例化输出错误
    NOT_IMPLEMENTED = 0x42,                           // Not Implemented yet
    CPU_XGBOOST_INIT_ERROR = 0x43,                    // CPU_XGBOOST_INIT_ERROR
    CPU_XGBOOST_PARAMETER_CHECK_ERROR = 0x44,         // CPU_XGBOOST_PARAMETER_ERROR
    CPU_XGBOOST_SAVE_ERROR = 0x45,                    // CML_CPU_XGBOOST_SAVE_ERROR
    CPU_XGBOOST_PATH_CHECK_ERROR = 0x46,              // CPU_XGBOOST_PATH_CHECK_ERROR
    CPU_KNNC_CHECKPARAMS_ERROR = 0x47,                // KNNC参数错误
    CPU_KNNC_RESIZERESULTS_ERROR = 0x48,              // KNNC输出resize错误
    CPU_LOGISTR_CHECKPARAMS_ERROR = 0x49,             // LogistR输出参数检测错误
    CPU_LOGISTR_RESIZERESULTS_ERROR = 0x4A,           // LogistR输出resize错误
    CPU_LOGISTR_COMPUTE_ERROR = 0x4B,                 // LogistR输出wx计算错误
    CPU_LOGISTR_THREAD_ERROR = 0x4C,                  // LogistR线程执行失败
    CPU_LOGISTR_FIT_ERROR = 0x4D,                     // LogistR训练失败
    CPU_LOGISTR_SAVE_ERROR = 0x4E,                    // LogistR保存模型失败
    CPU_LOGISTR_INIT_ERROR = 0x51,                    // 初始化模型失败
    CPU_XGBOOST_EVAL_DATA_CHECK_ERROR = 0x52,         // xgboost eval data check fail
    CPU_XGBOOST_UPDATE_MODEL_ERROR = 0x53,            // xgboost update model failed
    CPU_XGBOOST_RESIZERESULTS_ERROR = 0x54,           // xgboost预测输出resize错误
    CPU_DATA_FORMAT_ERROR = 0x55,                     // 数据格式错误
    CPU_RFR_SERIALIZE_ERROR = 0x56,                   // RFR序列化模型错误
    CPU_RFC_SERIALIZE_ERROR = 0x57,                   // RFC序列化模型错误
    CPU_IFOREST_SERIALIZE_ERROR = 0x58,               // IForest序列化模型错误
    CPU_ELASTICNET_CHECKFITPARAMS_ERROR = 0x59,       // ElasticNet训练数据错误
    CPU_OCSVC_CHECKPARAMS_ERROR = 0x60,               // OCSVC参数错误
    CPU_OCSVC_RESIZERESULTS_ERROR = 0x61,             // OCSVC输出resize错误
    CPU_OCSVC_GETCOMPUTESAMPLES_ERROR = 0x62,         // OCSVC分割数据错误
    CPU_OCSVC_COMPUTERBFKERNEL_ERROR = 0x63,          // OCSVC高斯核计算错误
    CPU_IFOREST_BUILD_FEATURE_ERROR = 0x64,           // IForest构造特征错误
    SAVE_MODEL_ERROR = 0x65,                          // 保存模型错误
    POLINOMIAL_FEATURES_CHECK_PARAMS_ERROR = 0x66,    // PolynomialFeatures参数检查失败
    POLINOMIAL_FEATURES_CREATE_MEMORY_ERROR = 0x67,   // PolynomialFeatures创建内存失败
    STANDARD_SCALER_CHECK_PARAMS_ERROR = 0x68,        // StandardScaler参数检查失败
    DUMP_MODEL_ERROR = 0x69,                          // 导出模型错误
    CPU_GMM_CHECKPARAMS_ERROR = 0x6A,                 // GMM参数错误
    CPU_GMM_RESIZERESULTS_ERROR = 0x6B,               // GMM输出resize错误
    CPU_BLAS_DGEMM_ERROR = 0x6C,                      // BLAS Dgemm接口失败
    CPU_GMM_ESTIMATE_PROB_ERROR = 0x6D,               // GMM推理，预测概率失败
    CPU_GPR_CHECKPARAMS_ERROR = 0x6E,                 // GPR参数错误
    CPU_GPR_RESIZERESULTS_ERROR = 0x6F,               // GPR输出resize错误
    CPU_GPR_CREATE_KERNEL_ERROR = 0x70,               // GPR创建kernel错误
    CPU_GPR_CHECK_FIT_PARAMS_ERROR = 0x71,            // GPR训练参数错误
    CPU_GPR_TRAIN_ERROR = 0x72,                       // GPR矩阵训练错误
    CPU_GPR_BLAS_ERROR = 0x73,                        // GPR blas计算错误
    CPU_RF_QUANTIZE_CREATE_MEMORY_ERROR = 0x74,       // RF量化分配内存失败
    CPU_AHC_CHECKPARAMS_ERROR = 0x75,                 // AHC参数错误
    CPU_AHC_RESIZERESULTS_ERROR = 0x76,               // AHC输出resize错误
    CPU_KNNC_CHECK_FITPARAMS_ERROR = 0x77,            // knnc训练数据校验错误
    CPU_INIT_BINARYTREE_ERROR = 0x78,                 // 初始化binaryTree错误
    CPU_KNNC_CHECK_FITMODEL_ERROR = 0x79,             // knnc训练模型校验错误
    CPU_CREATE_BINARYTREE_ERROR = 0x7A,               // 创建binaryTree失败
    CPU_CWRAPPER_CREATE_TRAIN_DATA_ERROR = 0X7B,      // 创建训练数据错误
    CPU_CWRAPPER_CREATE_TRAIN_TARGET_ERROR = 0X7C,    // 创建训练目标错误
    CPU_CWRAPPER_CREATE_EVAL_DATA_ERROR = 0X7D,       // 创建验证数据错误
    CPU_CWRAPPER_CREATE_EVAL_TARGET_ERROR = 0X7E,     // 创建验证目标错误
    CPU_C_CREATE_GPR_ERROR = 0X7F,                    // 创建GPR对象失败
    CPU_C_GPR_LOAD_MODEL_ERROR = 0X80,                // GPR生成interpreter失败
    CPU_C_GPR_CREATE_SESSION_ERROR = 0X81,            // GPR创建session失败
    CPU_CWRAPPER_UNPACK_MODEL_ERROR = 0X82,           // cwrapper从内存反序列化ModelT失败
    CPU_CWRAPPER_INVALID_BUFFER = 0X83,               // cwrapper无效buffer
    CPU_CWRAPPER_MODEL_BUFFER_ERROR = 0X84,           // cwrapper创建模型buffer失败
    CPU_GBC_INIT_ERROR = 0x85,                        // GBC INIT 错误
    CPU_GBC_MEMORY_ERROR = 0x86,                      // GBC 内存操作错误
    CPU_GBC_GET_RAW_PRED_ERROR = 0x87,                // GBC 获取初始估值失败
    CPU_GBC_WARM_START_ERROR = 0x88,                  // GBC 热启动失败
    CPU_GBC_TRAIN_ERROR = 0x89,                       // GBC 训练过程失败
    CPU_GBC_GET_RESIDUAL_ERROR = 0x8A,                // GBC 获取残差失败
    CPU_GBDT_COMPRESS_TREE_ERROR = 0x8B,              // GBC 树压缩失败
    CPU_GBR_GET_RAW_PRED_ERROR = 0x8C,                // GBR 获取初始权值失败
    CPU_GBR_WARM_START_ERROR = 0x8D,                  // GBR 热启动失败
    CPU_GBR_TRAIN_ERROR = 0x8E,                       // GBR 训练失败
    CPU_GBR_GET_RESIDUAL_ERROR = 0x8F,                // GBR 获取残差失败
    CPU_CHMOD_FILE_ERROR = 0x91,                      // 设置文件权限失败
    CPU_C_CREATE_XGBOOST_ERROR = 0X92,                // 创建XGBOOST对象失败
    CPU_C_XGBOOST_LOAD_MODEL_ERROR = 0X93,            // XGBOOST生成interpreter失败
    CPU_C_XGBOOST_CREATE_SESSION_ERROR = 0X94,        // XGBOOST创建session失败
    CPU_GNB_CHECKPARAMS_ERROR = 0x95,                 // GNB参数检验错误
    CPU_GNB_RESIZERESULT_ERROR = 0x96,                // GNB输出resize错误
    CPU_GNB_SAVE_ERROR = 0x97,                        // GNB中间产物save失败
    CPU_GNB_MEMORY_NULL = 0x98,                       // GNB申请内存失败
    CPU_OCSVC_SOLVE_ERROR = 0x99,                     // OCSVC训练求解器错误
    CPU_VALUE_ERROR = 0x9A,                           // nan或inf错误
    CPU_BRR_PARAMS_ERROR = 0x9B,                      // BRR参数校验错误
    CPU_BRR_RESULT_ERROR = 0x9C,                      // BRR输出结果错误
    CPU_BRR_MEMORY_ERROR = 0x9D,                      // BRR内存调用失败
    CPU_BRR_FIT_ERROR = 0x9E,                         // BRR训练失败
    CPU_BRR_INITIAL_ERROR = 0x9F,                     // BRR初始化失败
    CPU_BRR_PREDICT_ERROR = 0xA0,                     // BRR 推理失败
    CPU_SVR_CHECK_FITMODEL_ERROR = 0xA6,              // svr训练模型校验错误
    CPU_LINEARSVC_CREATE_KERNEL_ERROR = 0xA7,         // linearSVC创建kernel失败
    CPU_SVC_CREATE_KERNEL_ERROR = 0xAB,               // SVC创建kernel失败
    RFC_FEATURE_IMPORTANCE_ERROR = 0xAC,              // RFC FeatureImportance求解失败
    CPU_PLSR_PARAMS_ERROR = 0xB0,                     // PLSR 参数校验错误
    CPU_PLSR_FIT_ERROR = 0xB1,                        // PLSR 训练错误
    CPU_PLSR_PREDICT_ERROR = 0xB2,                    // PLSR 预测错误
    CPU_PLSR_TRANSFORM_ERROR = 0xB3,                  // PLSR 转换错误
    CPU_PLSR_INITIAL_ERROR = 0xB4,                    // PLSR 初始化错误
};
#ifdef __cplusplus
}
#endif
#endif