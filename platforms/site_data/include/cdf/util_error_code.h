/***********************************************************************
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 *
 * -----------------------------------------------------------------------
 * util_error_code.h
 *
 * Project Code:  NA
 * Module Name :  DOPAI
 * Create Date :  2020-03-04
 * Version     :  Initial Draft
 * Author      :  wutao
 * Description :  定义DOPAI统一错误码生成宏及模块划分
 * -----------------------------------------------------------------------
 * Modification History
 * DATE        NAME             DESCRIPTION
 * ----------------------------------------------------------------------
 * 2020-03-04 wutao         Created
 * 2020-03-25 zhangziyang       Add log error code
 * 2020-03-28 zhangziyang       Change file name
 * ***********************************************************************/
#ifndef UTIL_ERROR_CODE_H
#define UTIL_ERROR_CODE_H

#include <stdlib.h>
#include "dopai_error_code.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup error_code
 * 通用模块生成错误码使用的错误码类型
 */
typedef enum tagUtilErrorType {
    CONFIG_NO_ERROR = 0X00,                 /**< 配置正确 */
    CONFIG_INVALID_JSON_ERROR = 0X01,       /**< 配置错误，无效的json文件 */
    CONFIG_JSON_ITEM_ERROR = 0X02,          /**< 配置错误，json结构错误 */
    CONFIG_CLASS_DEFINE_ERROR = 0X03,       /**< 配置错误，类定义错误 */
    LOG_REG_NO_ERROR = 0X04,                /**< 日志注册成功 */
    LOG_REG_INFO_PTR_NULL = 0X05,           /**< 日志注册失败，注册信息指针为空 */
    LOG_REG_INFO_FUNC_PTR_NULL = 0X06,      /**< 日志注册失败，注册信息的函数指针为空 */
    LOG_RGE_GET_MODE_ID_FAIL = 0X07,        /**< 日志注册失败，获取modeId失败 */
    LOG_REG_REG_FAIL = 0X08,                /**< 日志注册失败，注册回调函数失败, 已使用默认方式记录log */
    LOG_REG_GET_LOG_BUF_FAIL = 0X09,      /**< 日志注册失败，获取日志bufer大小错误 */
    UNZIP_OPEN_FAIL = 0X0A,               /**< 解压失败，打开文件失败 */
    UNZIP_FAIL = 0X0B,                    /**< 解压失败，解压错误 */
    UNZIP_MKDIR_FAIL = 0X0C,              /**< 解压失败，创建目录失败 */
    MEM_REG_REG_FAIL = 0X0D,              /**< 内存接口注册失败 */
    FILE_OPEN_FAIL = 0X0E,                /**< 文件打开失败 */
    FILE_PATH_INVALID = 0X0F,             /**< 文件路径不合法 */
    DECOMPRESS_REG_NO_ERROR = 0X10,       /**< 解压缩注册成功 */
    DECOMPRESS_REG_INFO_PTR_NULL = 0X11,  /**< 解压缩注册失败，注册解压缩信息指针为空 */
    DECOMPRESS_REG_REPEAT = 0X12,         /**< 解压缩册失败，重复注册解压缩 */
    FILE_VERIFY_REG_NO_ERROR = 0X13,      /**< 验签注册成功 */
    FILE_VERIFY_REG_INFO_PTR_NULL = 0X14, /**< 验签注册失败，注册验签信息指针为空 */
    FILE_VERIFY_REG_REPEAT = 0X15,        /**< 验签注册失败，重复注册验签 */
    METRICS_NO_ERROR = 0X16,               /**< metrics参数正确 */
    SAMPLE_SIZE_ERROR = 0X17,              /**< 各参数样本大小不一致 */
    TRUE_LABELS_VALUE_ERROR = 0X18,        /**< 真实标签值超过类别范围 */
    WEIGHTS_VALUE_ERROR = 0X19,            /**< 样本权重值错误 */
    PREDICT_LABELS_ERROR = 0X1A,           /**< 预测标签值超过类别范围 */
    PREDICT_PROBABILITY_SIZE_ERROR = 0X1B, /**< 预测概率列数错误 */
    ONEHOT_LABELS_ERROR = 0X1C,            /**< one-hot标签不正确 */
    ENTROPY_INPUT_INVALID = 0x1D,         /**< 求信息熵的时候输入不合法 */
    MEM_COPY_FAIL = 0x1E,                 /**< 拷贝失败 */
    ENTROPY_CAUCULATE_FAIL = 0x1F,        /**< 信息熵计算失败 */
    CONFIG_INSTANCE_INIT_FAIL = 0x20,     /**< 配置实例初始化失败 */
    DELAY_REG_FAIL = 0X21,                /**< 时延注册失败 */
    RANDOM_REG_FAIL = 0X22,               /**< 随机函数注册失败 */
    SOFTMAX_PROB_ERROR = 0x23,            /**< softmax概率值不正确 */
    METRICS_INDEX_OVERFLOW = 0x24,                /**< 输入下标溢出 */
    AVERAGE_TYPE_ERROR = 0x25,            /**< 混淆矩阵均值类别错误 */
    ENCRYPT_REG_FAIL = 0x26,              /**< 秘钥操作接口注册失败 */
    UNKNOW_ERROR = 0XFF                   /**< 未知错误 */
} UtilErrorType;

/**
 * @ingroup error_code
 * 通用模块错误码
 */
typedef enum tagUtilErrorCode {
    UTIL_NO_ERROR = 0,                             /**< 配置正确 *< 日志注册成功 */
    UTIL_CONFIG_INVALID_JSON_ERROR = MAKE_ERRORCODE(MOD_UTIL, CONFIG_INVALID_JSON_ERROR, 0),  /**< 配置失败,
无效的json文件 HEX: 0x7000100 DEC:117,440,768 */
    UTIL_CONFIG_JSON_ITEM_ERROR = MAKE_ERRORCODE(MOD_UTIL, CONFIG_JSON_ITEM_ERROR, 0),        /**< 配置失败,
json结构错误 HEX: 0x7000200 DEC:117,441,024 */
    UTIL_CONFIG_CLASS_DEFINE_ERROR = MAKE_ERRORCODE(MOD_UTIL, CONFIG_CLASS_DEFINE_ERROR, 0),  /**< 配置失败,
类定义错误 HEX: 0x7000300 DEC:117,441,280 */
    UTIL_LOGREG_INFO_PTR_NULL = MAKE_ERRORCODE(MOD_UTIL, LOG_REG_INFO_PTR_NULL, 0), /**< 日志注册失败,
注册信息指针为空 HEX: 0x7000500 DEC:117,441,792 */
    UTIL_LOGREG_INFO_FUNC_PTR_NULL = MAKE_ERRORCODE(MOD_UTIL, LOG_REG_INFO_FUNC_PTR_NULL, 0), /**<
    日志注册失败, 注册信息的函数指针为空 HEX: 0x7000600 DEC:117,442,048 */
    UTIL_LOGREG_GET_MODE_ID_FAIL = MAKE_ERRORCODE(MOD_UTIL, LOG_RGE_GET_MODE_ID_FAIL, 0),  /**< 日志注册失败,
获取modeId失败 HEX: 0x7000700 DEC:117,442,304 */
    UTIL_LOGREG_REG_FAIL = MAKE_ERRORCODE(MOD_UTIL, LOG_REG_REG_FAIL, 0),                  /**< 日志注册失败,
注册回调函数失败 HEX: 0x7000800 DEC:117,442,560 */
    UTIL_LOG_REG_GET_LOG_BUF_FAIL = MAKE_ERRORCODE(MOD_UTIL, LOG_REG_GET_LOG_BUF_FAIL, 0), /**< 日志注册失败,
获取日志buf大小错误 HEX: 0x7000900 DEC:117,442,816 */
    UTIL_UNZIP_OPEN_FAIL = MAKE_ERRORCODE(MOD_UTIL, UNZIP_OPEN_FAIL, 0),                   /**< 解压失败,
打开文件失败 HEX: 0x7000A00 DEC:117,443,072 */
    UTIL_UNZIP_FAIL = MAKE_ERRORCODE(MOD_UTIL, UNZIP_FAIL, 0),                             /**< 解压失败,
解压错误 HEX: 0x7000B00 DEC:117,443,328 */
    UTIL_UNZIP_MKDIR_FAIL = MAKE_ERRORCODE(MOD_UTIL, UNZIP_MKDIR_FAIL, 0),                 /**< 解压失败,
创建目录失败 HEX: 0x7000C00 DEC:117,443,584 */
    UTIL_MEMREG_REG_FAIL = MAKE_ERRORCODE(MOD_UTIL, MEM_REG_REG_FAIL, 0),                  /**< 内存注册失败,
HEX: 0x7000D00 DEC:117,443,840 */
    UTIL_FILE_OPEN_FAIL = MAKE_ERRORCODE(MOD_UTIL, FILE_OPEN_FAIL, 0),                     /**< 文件打开失败,
HEX: 0x7000E00 DEC:117,444,096 */
    UTIL_FILE_PATH_INVALID = MAKE_ERRORCODE(MOD_UTIL, FILE_PATH_INVALID, 0),               /**< 文件路径不合法
HEX: 0x7000F00 DEC:117,444,352 */
    UTIL_DECOMPRESS_REG_INFO_PTR_NULL = MAKE_ERRORCODE(MOD_UTIL, DECOMPRESS_REG_INFO_PTR_NULL, 0),
    UTIL_DECOMPRESS_REG_REPEAT = MAKE_ERRORCODE(MOD_UTIL, DECOMPRESS_REG_REPEAT, 0),
    UTIL_FILE_VERIFY_REG_INFO_PTR_NULL = MAKE_ERRORCODE(MOD_UTIL, FILE_VERIFY_REG_INFO_PTR_NULL, 0),
    UTIL_FILE_VERIFY_REG_REPEAT = MAKE_ERRORCODE(MOD_UTIL, FILE_VERIFY_REG_REPEAT, 0),
    UTIL_SAMPLE_SIZE_ERROR = MAKE_ERRORCODE(MOD_UTIL, SAMPLE_SIZE_ERROR, 0),
    UTIL_TRUE_LABELS_VALUE_ERROR = MAKE_ERRORCODE(MOD_UTIL, TRUE_LABELS_VALUE_ERROR, 0),
    UTIL_WEIGHTS_VALUE_ERROR = MAKE_ERRORCODE(MOD_UTIL, WEIGHTS_VALUE_ERROR, 0),
    UTIL_PREDICT_LABELS_ERROR = MAKE_ERRORCODE(MOD_UTIL, PREDICT_LABELS_ERROR, 0),
    UTIL_PREDICT_PROBABILITY_SIZE_ERROR = MAKE_ERRORCODE(MOD_UTIL, PREDICT_PROBABILITY_SIZE_ERROR, 0),
    UTIL_ONEHOT_LABELS_ERROR = MAKE_ERRORCODE(MOD_UTIL, ONEHOT_LABELS_ERROR, 0),
    UTIL_ENTROPY_INPUT_INVALID = MAKE_ERRORCODE(MOD_UTIL, ENTROPY_INPUT_INVALID, 0),
    UTIL_MEM_COPY_FAIL = MAKE_ERRORCODE(MOD_UTIL, MEM_COPY_FAIL, 0),
    UTIL_ENTROPY_CAUCULATE_FAIL = MAKE_ERRORCODE(MOD_UTIL, ENTROPY_CAUCULATE_FAIL, 0),
    UTIL_CONFIG_INSTANCE_INIT_FAIL = MAKE_ERRORCODE(MOD_UTIL, CONFIG_INSTANCE_INIT_FAIL, 0),
    UTIL_DELAY_REG_FAIL = MAKE_ERRORCODE(MOD_UTIL, DELAY_REG_FAIL, 0),
    UTIL_RANDOM_REG_FAIL = MAKE_ERRORCODE(MOD_UTIL, RANDOM_REG_FAIL, 0),
    UTIL_SOFTMAX_PROB_ERROR = MAKE_ERRORCODE(MOD_UTIL, SOFTMAX_PROB_ERROR, 0),
    UTIL_METRICS_INDEX_OVERFLOW = MAKE_ERRORCODE(MOD_UTIL, METRICS_INDEX_OVERFLOW, 0),
    UTIL_AVERAGE_TYPE_ERROR = MAKE_ERRORCODE(MOD_UTIL, AVERAGE_TYPE_ERROR, 0),
    UTIL_ENCRYPT_REG_FAIL = MAKE_ERRORCODE(MOD_UTIL, ENCRYPT_REG_FAIL, 0),
    UTIL_UNKNOWN_ERROR = MAKE_ERRORCODE(MOD_UTIL, UNKNOW_ERROR, 0),
} UtilErrorCode;

#ifdef __cplusplus
}
#endif

#endif /* UTIL_ERROR_CODE_H */
