/*******************************************************************************
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 *
 * -----------------------------------------------------------------------------
 *
 * dataframe_cwrapper_replace.h
 *
 * Project Code: DOPAI
 * Module Name:
 * Date Created: 2021-08-18
 * Author:
 * Description: CDF Replace 对外 C 接口
 *
 * -----------------------------------------------------------------------------
 * Modification History
 * DATE          NAME                       DESCRIPTION
 * -----------------------------------------------------------------------------
 *
 *******************************************************************************/

#ifndef DOPAI_CDATAFRME_REPLACECWRAPPER_H
#define DOPAI_CDATAFRME_REPLACECWRAPPER_H
#include <time.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "dataframe_c_type.h"
#include "dataframe_c_wrapper.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 对DataFrame替换Bool
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 对DataFrame替换Bool
 *
 * @attention
 * @param  source [IN]  CDataFrame句柄
 * @param  dst [OUT]  列名数组
 * @param  srcNum [IN]  原值
 * @param  replaceNum [IN]  替换值
 * @retval #NO_ERROR 成功
 * @retval #NULL 失败
 */
int32_t DOPAI_CDF_ReplaceBool(DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source, bool srcNum, bool replaceNum);

/**
 * @brief 对DataFrame替换Int8
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 对DataFrame替换Int8
 *
 * @attention
 * @param  source [IN]  CDataFrame句柄
 * @param  dst [OUT]  列名数组
 * @param  srcNum [IN]  原值
 * @param  replaceNum [IN]  替换值
 * @retval #NO_ERROR 成功
 * @retval #NULL 失败
 */
int32_t DOPAI_CDF_ReplaceInt8(
    DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source, int8_t srcNum, int8_t replaceNum);

/**
 * @brief 对DataFrame替换Int16
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 对DataFrame替换Int16
 *
 * @attention
 * @param  source [IN]  CDataFrame句柄
 * @param  dst [OUT]  列名数组
 * @param  srcNum [IN]  原值
 * @param  replaceNum [IN]  替换值
 * @retval #NO_ERROR 成功
 * @retval #NULL 失败
 */
int32_t DOPAI_CDF_ReplaceInt16(
    DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source, int16_t srcNum, int16_t replaceNum);

/**
 * @brief 对DataFrame替换Int32
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 对DataFrame替换Int32
 *
 * @attention
 * @param  source [IN]  CDataFrame句柄
 * @param  dst [OUT]  列名数组
 * @param  srcNum [IN]  原值
 * @param  replaceNum [IN]  替换值
 * @retval #NO_ERROR 成功
 * @retval #NULL 失败
 */
int32_t DOPAI_CDF_ReplaceInt32(
    DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source, int32_t srcNum, int32_t replaceNum);

/**
 * @brief 对DataFrame替换Int64
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 对DataFrame替换Int64
 *
 * @attention
 * @param  source [IN]  CDataFrame句柄
 * @param  dst [OUT]  列名数组
 * @param  srcNum [IN]  原值
 * @param  replaceNum [IN]  替换值
 * @retval #NO_ERROR 成功
 * @retval #NULL 失败
 */
int32_t DOPAI_CDF_ReplaceInt64(
    DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source, int64_t srcNum, int64_t replaceNum);

/**
 * @brief 对DataFrame替换Uint8
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 对DataFrame替换Uint8
 *
 * @attention
 * @param  source [IN]  CDataFrame句柄
 * @param  dst [OUT]  列名数组
 * @param  srcNum [IN]  原值
 * @param  replaceNum [IN]  替换值
 * @retval #NO_ERROR 成功
 * @retval #NULL 失败
 */
int32_t DOPAI_CDF_ReplaceUint8(
    DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source, uint8_t srcNum, uint8_t replaceNum);

/**
 * @brief 对DataFrame替换Uint16
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 对DataFrame替换Uint16
 *
 * @attention
 * @param  source [IN]  CDataFrame句柄
 * @param  dst [OUT]  列名数组
 * @param  srcNum [IN]  原值
 * @param  replaceNum [IN]  替换值
 * @retval #NO_ERROR 成功
 * @retval #NULL 失败
 */
int32_t DOPAI_CDF_ReplaceUint16(
    DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source, uint16_t srcNum, uint16_t replaceNum);

/**
 * @brief 对DataFrame替换Uint32
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 对DataFrame替换Uint32
 *
 * @attention
 * @param  source [IN]  CDataFrame句柄
 * @param  dst [OUT]  列名数组
 * @param  srcNum [IN]  原值
 * @param  replaceNum [IN]  替换值
 * @retval #NO_ERROR 成功
 * @retval #NULL 失败
 */
int32_t DOPAI_CDF_ReplaceUint32(
    DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source, uint32_t srcNum, uint32_t replaceNum);

/**
 * @brief 对DataFrame替换Uint64
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 对DataFrame替换Uint64
 *
 * @attention
 * @param  source [IN]  CDataFrame句柄
 * @param  dst [OUT]  列名数组
 * @param  srcNum [IN]  原值
 * @param  replaceNum [IN]  替换值
 * @retval #NO_ERROR 成功
 * @retval #NULL 失败
 */
int32_t DOPAI_CDF_ReplaceUint64(
    DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source, uint64_t srcNum, uint64_t replaceNum);

/**
 * @brief 对DataFrame替换Float
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 对DataFrame替换Float
 *
 * @attention
 * @param  source [IN]  CDataFrame句柄
 * @param  dst [OUT]  列名数组
 * @param  srcNum [IN]  原值
 * @param  replaceNum [IN]  替换值
 * @retval #NO_ERROR 成功
 * @retval #NULL 失败
 */
int32_t DOPAI_CDF_ReplaceFloat(DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source, float srcNum, float replaceNum);

/**
 * @brief 对DataFrame替换Double
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 对DataFrame替换Double
 *
 * @attention
 * @param  source [IN]  CDataFrame句柄
 * @param  dst [OUT]  列名数组
 * @param  srcNum [IN]  原值
 * @param  replaceNum [IN]  替换值
 * @retval #NO_ERROR 成功
 * @retval #NULL 失败
 */
int32_t DOPAI_CDF_ReplaceDouble(
    DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source, double srcNum, double replaceNum);

/**
 * @brief 对DataFrame替换String
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 对DataFrame替换String
 *
 * @attention
 * @param  source [IN]  CDataFrame句柄
 * @param  dst [OUT]  列名数组
 * @param  srcNum [IN]  原字符串
 * @param  replaceNum [IN]  替换字符串
 * @retval #NO_ERROR 成功
 * @retval #NULL 失败
 */
int32_t DOPAI_CDF_ReplaceString(
    DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source, const char *srcNum, const char *replaceNum);

/**
 * @brief 对DataFrame进行行列转换
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 对DataFrame的行转换为列，列转换为行
 *
 * @attention
 * @param  source [IN]  CDataFrame句柄
 * @param  dst [OUT]  列名数组
 * @retval #NO_ERROR 成功
 * @retval #NULL 失败
 */

#ifdef __cplusplus
}
#endif

#endif // DOPAI_CDATAFRME_REPLACECWRAPPER_H
