/*******************************************************************************
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 *
 * -----------------------------------------------------------------------------
 *
 * dataframe_cwrapper_typecast.h
 *
 * Project Code: DOPAI
 * Module Name:
 * Date Created: 2021-08-18
 * Author:
 * Description: CDF TypeCast 对外 C 接口
 *
 * -----------------------------------------------------------------------------
 * Modification History
 * DATE          NAME                       DESCRIPTION
 * -----------------------------------------------------------------------------
 *
 *******************************************************************************/

#ifndef DOPAI_CDATAFRME_TYPECASTCWRAPPER_H
#define DOPAI_CDATAFRME_TYPECASTCWRAPPER_H
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
 * @brief 将DataFrame中的类型转为 Bool 类型
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 将DataFrame中的类型转为 Bool 类型
 *
 * @attention
 * @param  dst [OUT]  CDataFrame句柄
 * @param  source [IN]  CDataFrame句柄
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t DOPAI_CDF_TypeCastBool(DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source);
/**
 * @brief 将DataFrame中的类型转为 Int8 类型
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 将DataFrame中的类型转为 Int8 类型
 *
 * @attention
 * @param  dst [OUT]  CDataFrame句柄
 * @param  source [IN]  CDataFrame句柄
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t DOPAI_CDF_TypeCastInt8(DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source);
/**
 * @brief 将DataFrame中的类型转为 Int16 类型
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 将DataFrame中的类型转为 Int16 类型
 *
 * @attention
 * @param  dst [OUT]  CDataFrame句柄
 * @param  source [IN]  CDataFrame句柄
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t DOPAI_CDF_TypeCastInt16(DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source);
/**
 * @brief 将DataFrame中的类型转为 Int32 类型
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 将DataFrame中的类型转为 Int32 类型
 *
 * @attention
 * @param  dst [OUT]  CDataFrame句柄
 * @param  source [IN]  CDataFrame句柄
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t DOPAI_CDF_TypeCastInt32(DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source);
/**
 * @brief 将DataFrame中的类型转为 Int64 类型
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 将DataFrame中的类型转为 Int64 类型
 *
 * @attention
 * @param  dst [OUT]  CDataFrame句柄
 * @param  source [IN]  CDataFrame句柄
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t DOPAI_CDF_TypeCastInt64(DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source);
/**
 * @brief 将DataFrame中的类型转为 Uint8 类型
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 将DataFrame中的类型转为 Uint8 类型
 *
 * @attention
 * @param  dst [OUT]  CDataFrame句柄
 * @param  source [IN]  CDataFrame句柄
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t DOPAI_CDF_TypeCastUint8(DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source);
/**
 * @brief 将DataFrame中的类型转为 Uint16 类型
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 将DataFrame中的类型转为 Uint16 类型
 *
 * @attention
 * @param  dst [OUT]  CDataFrame句柄
 * @param  source [IN]  CDataFrame句柄
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t DOPAI_CDF_TypeCastUint16(DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source);
/**
 * @brief 将DataFrame中的类型转为 Uint32 类型
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 将DataFrame中的类型转为 Uint32 类型
 *
 * @attention
 * @param  dst [OUT]  CDataFrame句柄
 * @param  source [IN]  CDataFrame句柄
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t DOPAI_CDF_TypeCastUint32(DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source);
/**
 * @brief 将DataFrame中的类型转为 Uint64 类型
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 将DataFrame中的类型转为 Uint64 类型
 *
 * @attention
 * @param  dst [OUT]  CDataFrame句柄
 * @param  source [IN]  CDataFrame句柄
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t DOPAI_CDF_TypeCastUint64(DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source);
/**
 * @brief 将DataFrame中的类型转为 Float 类型
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 将DataFrame中的类型转为 Float 类型
 *
 * @attention
 * @param  dst [OUT]  CDataFrame句柄
 * @param  source [IN]  CDataFrame句柄
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t DOPAI_CDF_TypeCastFloat(DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source);
/**
 * @brief 将DataFrame中的类型转为 Double 类型
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 将DataFrame中的类型转为 Double 类型
 *
 * @attention
 * @param  dst [OUT]  CDataFrame句柄
 * @param  source [IN]  CDataFrame句柄
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t DOPAI_CDF_TypeCastDouble(DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source);
/**
 * @brief 将DataFrame中的类型转为 String 类型
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 将DataFrame中的类型转为 String 类型
 *
 * @attention
 * @param  dst [OUT]  CDataFrame句柄
 * @param  source [IN]  CDataFrame句柄
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t DOPAI_CDF_TypeCastString(DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source);
/**
 * @brief 将DataFrame中的类型转为 DataFrame 类型
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 将DataFrame中的类型转为 DataFrame 类型
 *
 * @attention
 * @param  dst [OUT]  CDataFrame句柄
 * @param  source [IN]  CDataFrame句柄
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t DOPAI_CDF_TypeCastDataFrame(DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source);
/**
 * @brief 将DataFrame中的类型转为 DateTime 类型
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 将DataFrame中的类型转为 DateTime 类型
 *
 * @attention
 * @param  dst [OUT]  CDataFrame句柄
 * @param  source [IN]  CDataFrame句柄
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t DOPAI_CDF_TypeCastDateTime(DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source);

#ifdef __cplusplus
}
#endif

#endif // DOPAI_CDATAFRME_TYPECASTCWRAPPER_H
