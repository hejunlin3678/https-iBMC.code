/***********************************************************************
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 *
 * -----------------------------------------------------------------------
 * dopai_dopai_error_code.h
 *
 * Project Code:  NA
 * Module Name :  DOPAI
 * Create Date :  2020-02-24
 * Version     :  Initial Draft
 * Author      :  wutao
 * Description :  定义DOPAI统一错误码生成宏及模块划分
 * -----------------------------------------------------------------------
 * Modification History
 * DATE        NAME             DESCRIPTION
 * ----------------------------------------------------------------------
 * 2020-02-24 wutao             Created
 * 2020-03-28 zhangziyang       Change file name
 ************************************************************************/

#ifndef DOPAI_ERROR_CODE_HPP
#define DOPAI_ERROR_CODE_HPP

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup error_code 错误码说明 */
#define DOPAI_ERROR_CODE_C

/**
 * @ingroup  error_code
 * 定义DOPAI统一正确返回值
 */
#define DOPAI_OK (0)
/**
 * @ingroup  error_code
 * 定义DOPAI统一错误返回值
 */
#define DOPAI_ERR (-1)

/**
 * @ingroup error_code
 * @brief 生成错误码
 *
 * @par 描述:
 * 根据模块ID、错误类型和附加值生成错误码，生成错误码共32位，具体格式如下：
 * -------------------------------------------------------------------------------
 * |       MOD(8bit)      |         type(16bit)         |   pad(8bit) Optional   |
 * -------------------------------------------------------------------------------
 *
 * @attention
 *
 * @param  module [IN]  模块ID，取值范围见 ModuleErrorCode
 * @param  type [IN]  错误类型，取值范围由各个模块确定
 * @param  pad [IN]  补充项，无补充项时填0
 *
 * @retval #错误码
 *
 * @par 依赖
 * <ul><li>dopai_error_code.h：该接口声明所在的头文件。</li></ul>
 * @since XX
 */
#define MAKE_ERRORCODE(module, type, pad) \
    ((int32_t) (((0xFF & ((uint32_t) (module))) << 24) | ((0xFFFF & ((uint32_t) (type))) << 8) | \
    (0xFF & ((uint32_t) (pad)))))

/**
 * @ingroup error_code
 * ModuleErrorCode用于生成错误码时模块ID的定义
 */
typedef enum tagModuleErrorCode {
    MOD_NN = 0x00,        /**< 神经网络模块ID */
    MOD_CML = 0x01,       /**< 机器学习模块ID */
    MOD_PPL = 0x02,       /**< Pipeline模块ID */
    MOD_CDF = 0x03,       /**< 数据处理模块ID */
    MOD_FL = 0x04,        /**< 联邦学习模块ID */
    MOD_MODEL = 0x05,     /**< 模型管理模块ID */
    MOD_CONVER = 0x06,    /**< 转化工具模块ID */
    MOD_UTIL = 0x07,      /**< 通用工具模块ID */
    MOD_LITE_NN = 0x08,   /**< Lite-NN模块ID */
    MOD_LITE_UTIL = 0x09, /**< 通用工具模块ID */
    MOD_LITE_ML = 0x0A,   /**< 通用工具模块ID */
    MOD_SECURITY = 0x0B,  /**< sucurity模块ID */
    MOD_DSP = 0x0C,       /**< workflow框架 */
    MOD_DAF = 0x0D,       /**< DataAnalysis模块ID */
    MOD_CV = 0x0E         /**< cv模块ID */
} ModuleErrorCode;

#ifdef __cplusplus
}
#endif

#endif /* DOPAI_ERROR_CODE_HPP */
