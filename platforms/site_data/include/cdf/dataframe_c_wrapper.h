/* ******************************************************************************
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 *
 * -------------------------------------------------------------------------------
 *
 * CWrapper.h
 *
 * Project Code: DOPAI
 * Module Name:
 * Date Created: 2020-08-31
 * Author:
 * Description: CDF 对外C接口
 *
 * -------------------------------------------------------------------------------
 * Modification History
 * DATE          NAME                       DESCRIPTION
 * -----------------------------------------------------------------------------
 *
 * ***************************************************************************** */

#ifndef DOPAI_CDF_CWRAPPER_H_INCLUDE
#define DOPAI_CDF_CWRAPPER_H_INCLUDE
#include <time.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "dataframe_c_type.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Tag_DOPAI_CDF_CONTEXT {
    struct DOPAI_CDF *dataframe;
    struct DOPAI_CDF_REF *dataframeRef;
} DOPAI_CDF_CONTEXT;

typedef struct Tag_DOPAI_CDF_InputAttr {
    size_t shape[2];                 /**< 输入数据data的维度 */
    DOPAI_CDATAFRAME_TYPEID *typeID; /**< 输入数据的data的数据类型 */
} DOPAI_CDF_CWrapper_InputAttr;

typedef struct Tag_DOPAI_CDF_CWrapper_Output {
    void **data; /**< 输出结果，暂时将类型定为int32_t */
    size_t row;  /**< 输出结果的行数 */
    size_t col;  /**< 输出结果的列数 */
} DOPAI_CDF_CWrapper_Output;

typedef struct Tag_DOPAI_CDF_GroupBy {
    DOPAI_CDF_CONTEXT *context;
    uint32_t len;
} DOPAI_CDF_GROUPBY;

typedef struct Tag_DOPAI_CDF_MergePlace {
    uint32_t *leftOn;
    uint32_t *rightOn;
    uint32_t leftLen;
    uint32_t rightLen;
} DOPAI_CDF_MergePlace;

/**
 * @brief 创建一个空的DataFrame句柄
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 创建一个空的DataFrame句柄
 *
 * @attention
 * @retval 非NULL 成功
 * @retval #NULL 失败
 */
DOPAI_CDF_CONTEXT *DOPAI_CDF_CreateDF(void);

/**
 * @brief 释放DataFrame句柄
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 释放DataFrame句柄
 *
 * @attention
 * @param  source [IN]  CDataFrame句柄地址
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t DOPAI_CDF_Release(DOPAI_CDF_CONTEXT **dst);
/**
 * @brief 对DataFrame进行切片
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 对DataFrame进行切片
 *
 * @attention
 * @param  source [IN]  CDataFrame句柄
 * @param  row [IN]  行增长方向选取的index数组
 * @param  col [IN]  列增长方向选取的index数组
 * @param  rowLen [IN]  row参数的数组长度
 * @param  colLen [IN]  col参数的数组长度
 * @retval #非NULL CDataFrame切片句柄
 * @retval #NULL 失败
 */
DOPAI_CDF_CONTEXT *DOPAI_CDF_Slice(
    const DOPAI_CDF_CONTEXT *source, const size_t *row, const size_t *col, uint32_t rowLen, uint32_t colLen);
/**
 * @brief 去除DataFrame中有NAN的行
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 去除DataFrame中有NAN的行
 *
 * @attention
 * @param  dst [OUT]  CDataFrame句柄
 * @param  source [IN]  CDataFrame句柄
 * @param  any [IN]  true: 只要某行任一元素为NAN, 去除; false: 某行全为NAN才会被去除
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t DOPAI_CDF_DropNanRow(DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source, const bool any);
/**
 * @brief 去除DataFrame中有NAN的列
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 去除DataFrame中有NAN的列
 *
 * @attention
 * @param  dst [OUT]  CDataFrame句柄
 * @param  source [IN]  CDataFrame句柄
 * @param  any [IN]  true: 只要某列任一元素为NAN, 去除; false: 某列全为NAN才会被去除
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t DOPAI_CDF_DropNanCol(DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source, const bool any);

/**
 * @brief 按照boolFilter去除DataFrame中的列
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 按照boolFilter去除DataFrame中的列
 *
 * @attention
 * @param  dst [OUT]  CDataFrame句柄
 * @param  source [IN]  CDataFrame句柄
 * @param  boolFilter [IN]  BoolFilter为只有一列Bool值类型的DataFrame, 且长度和DataFrame列数相同
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t DOPAI_CDF_DropColByBool(
    DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source, const DOPAI_CDF_CONTEXT *boolFilter);
/**
 * @brief 按照列数组去除DataFrame中的列
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 按照列数组去除DataFrame中的列
 *
 * @attention
 * @param  dst [OUT]  CDataFrame句柄
 * @param  source [IN]  CDataFrame句柄
 * @param  columnIndexList [IN]  列数组
 * @param  length [IN]  列数组长度
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t DOPAI_CDF_DropColByIndex(
    DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source, const uint32_t *columnIndexList, const uint32_t length);
/**
 * @brief 按照列名数组去除DataFrame中的列
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 按照列名数组去除DataFrame中的列
 *
 * @attention
 * @param  dst [OUT]  CDataFrame句柄
 * @param  source [IN]  CDataFrame句柄
 * @param  nameToIndexList [IN]  列名数组
 * @param  length [IN]  列名数组长度
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t DOPAI_CDF_DropColByName(
    DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source, const char **nameToIndexList, const uint32_t length);
/**
 * @brief 按照boolFilter去除DataFrame中的行
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 按照boolFilter去除DataFrame中的行
 *
 * @attention
 * @param  dst [OUT]  CDataFrame句柄
 * @param  source [IN]  CDataFrame句柄
 * @param  boolFilter [IN]  BoolFilter为只有一行Bool值类型的DataFrame, 且长度和DataFrame行数相同
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t DOPAI_CDF_DropRowByBool(
    DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source, const DOPAI_CDF_CONTEXT *boolFilter);
/**
 * @brief 按照列数组去除DataFrame中的行
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 按照行号数组去除DataFrame中的行
 *
 * @attention
 * @param  dst [OUT]  CDataFrame句柄
 * @param  source [IN]  CDataFrame句柄
 * @param  columnIndexList [IN]  行号数组
 * @param  length [IN]  行号数组长度
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t DOPAI_CDF_DropRowByIndex(
    DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source, const uint32_t *rowIndexList, const uint32_t length);
/**
 * @brief 按照数据库join方式合并两个DataFrame
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 按照数据库join方式合并两个DataFrame
 *
 * @attention
 * @param  dst [OUT]  CDataFrame句柄
 * @param  source [IN]  左CDataFrame句柄
 * @param  rdataFrame [IN]  右CDataFrame句柄
 * @param  mergePlace [IN]  merge操作参数包
 * @param  how [IN]  join方法
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t DOPAI_CDF_Merge(DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source, const DOPAI_CDF_CONTEXT *rdataFrame,
    const DOPAI_CDF_MergePlace *mergePlace, const DOPAI_CDATAFRAME_MERGE_JOINTYPE how);
/**
 * @brief 获取DataFrame中的数据( Bool 类型)
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 获取DataFrame中的数据( Bool 类型)
 *
 * @attention
 * @param  output [OUT]  CDataFrame句柄
 * @param  dst [IN]  DOPAI_CDF_CWrapper_Output数据包
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t DOPAI_CDF_GetResultBool(const DOPAI_CDF_CONTEXT *dst, DOPAI_CDF_CWrapper_Output *output);
/**
 * @brief 获取DataFrame中的数据( Int8 类型)
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 获取DataFrame中的数据( Int8 类型)
 *
 * @attention
 * @param  output [OUT]  CDataFrame句柄
 * @param  dst [IN]  DOPAI_CDF_CWrapper_Output数据包
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t DOPAI_CDF_GetResultInt8(const DOPAI_CDF_CONTEXT *dst, DOPAI_CDF_CWrapper_Output *output);
/**
 * @brief 获取DataFrame中的数据( Int16 类型)
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 获取DataFrame中的数据( Int16 类型)
 *
 * @attention
 * @param  output [OUT]  CDataFrame句柄
 * @param  dst [IN]  DOPAI_CDF_CWrapper_Output数据包
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t DOPAI_CDF_GetResultInt16(const DOPAI_CDF_CONTEXT *dst, DOPAI_CDF_CWrapper_Output *output);
/**
 * @brief 获取DataFrame中的数据( Int32 类型)
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 获取DataFrame中的数据( Int32 类型)
 *
 * @attention
 * @param  output [OUT]  CDataFrame句柄
 * @param  dst [IN]  DOPAI_CDF_CWrapper_Output数据包
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t DOPAI_CDF_GetResultInt32(const DOPAI_CDF_CONTEXT *dst, DOPAI_CDF_CWrapper_Output *output);
/**
 * @brief 获取DataFrame中的数据( Int64 类型)
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 获取DataFrame中的数据( Int64 类型)
 *
 * @attention
 * @param  output [OUT]  CDataFrame句柄
 * @param  dst [IN]  DOPAI_CDF_CWrapper_Output数据包
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t DOPAI_CDF_GetResultInt64(const DOPAI_CDF_CONTEXT *dst, DOPAI_CDF_CWrapper_Output *output);
/**
 * @brief 获取DataFrame中的数据( Uint8 类型)
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 获取DataFrame中的数据( Uint8 类型)
 *
 * @attention
 * @param  output [OUT]  CDataFrame句柄
 * @param  dst [IN]  DOPAI_CDF_CWrapper_Output数据包
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t DOPAI_CDF_GetResultUint8(const DOPAI_CDF_CONTEXT *dst, DOPAI_CDF_CWrapper_Output *output);
/**
 * @brief 获取DataFrame中的数据( Uint16 类型)
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 获取DataFrame中的数据( Uint16 类型)
 *
 * @attention
 * @param  output [OUT]  CDataFrame句柄
 * @param  dst [IN]  DOPAI_CDF_CWrapper_Output数据包
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t DOPAI_CDF_GetResultUint16(const DOPAI_CDF_CONTEXT *dst, DOPAI_CDF_CWrapper_Output *output);
/**
 * @brief 获取DataFrame中的数据( Uint32 类型)
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 获取DataFrame中的数据( Uint32 类型)
 *
 * @attention
 * @param  output [OUT]  CDataFrame句柄
 * @param  dst [IN]  DOPAI_CDF_CWrapper_Output数据包
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t DOPAI_CDF_GetResultUint32(const DOPAI_CDF_CONTEXT *dst, DOPAI_CDF_CWrapper_Output *output);
/**
 * @brief 获取DataFrame中的数据( Uint64 类型)
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 获取DataFrame中的数据( Uint64 类型)
 *
 * @attention
 * @param  output [OUT]  CDataFrame句柄
 * @param  dst [IN]  DOPAI_CDF_CWrapper_Output数据包
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t DOPAI_CDF_GetResultUint64(const DOPAI_CDF_CONTEXT *dst, DOPAI_CDF_CWrapper_Output *output);
/**
 * @brief 获取DataFrame中的数据( Float 类型)
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 获取DataFrame中的数据( Float 类型)
 *
 * @attention
 * @param  output [OUT]  CDataFrame句柄
 * @param  dst [IN]  DOPAI_CDF_CWrapper_Output数据包
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t DOPAI_CDF_GetResultFloat(const DOPAI_CDF_CONTEXT *dst, DOPAI_CDF_CWrapper_Output *output);
/**
 * @brief 获取DataFrame中的数据( Double 类型)
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 获取DataFrame中的数据( Double 类型)
 *
 * @attention
 * @param  output [OUT]  CDataFrame句柄
 * @param  dst [IN]  DOPAI_CDF_CWrapper_Output数据包
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t DOPAI_CDF_GetResultDouble(const DOPAI_CDF_CONTEXT *dst, DOPAI_CDF_CWrapper_Output *output);
/**
 * @brief 获取DataFrame中的数据( DateTime 类型)
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 获取DataFrame中的数据( DateTime 类型)
 *
 * @attention
 * @param  output [OUT]  CDataFrame句柄
 * @param  dst [IN]  DOPAI_CDF_CWrapper_Output数据包
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t DOPAI_CDF_GetResultDateTime(const DOPAI_CDF_CONTEXT *dst, DOPAI_CDF_CWrapper_Output *output);
/**
 * @brief 获取DataFrame中的数据( DataFrame 类型)
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 获取DataFrame中的数据( DataFrame 类型)
 *
 * @attention
 * @param  output [OUT]  CDataFrame句柄
 * @param  dst [IN]  DOPAI_CDF_CWrapper_Output数据包
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t DOPAI_CDF_GetResultDataFrame(const DOPAI_CDF_CONTEXT *dst, DOPAI_CDF_CWrapper_Output *output);
/**
 * @brief 去除DataFrame中的数据( String 类型)
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 去除DataFrame中的数据( String 类型)
 *
 * @attention
 * @param  output [OUT]  CDataFrame句柄
 * @param  dst [IN]  DOPAI_CDF_CWrapper_Output数据包
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t DOPAI_CDF_GetResultString(const DOPAI_CDF_CONTEXT *dst, DOPAI_CDF_CWrapper_Output *output);
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

void DOPAI_CDF_ReleaseGroupArray(DOPAI_CDF_GROUPBY **dst);

/**
 * @brief 分组操作
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 分组操作
 *
 * @attention
 * @param  source [IN]  CDataFrame句柄
 * @param  indexes [IN]  分组依据的列下标数组
 * @param  indexLen [IN]  下标数组长度
 * @retval #NO_ERROR 成功，返回n个DataFrame（包含所有分组情况）
 * @retval #NULL 失败
 */
DOPAI_CDF_GROUPBY *DOPAI_CDF_GroupBy(const DOPAI_CDF_CONTEXT *source, const uint32_t *indexes, uint32_t indexLen);

/**
 * @brief 对DataFrame进行重新采样操作
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 对DataFrame进行重新采样操作
 *
 * @attention DataFrame必须有时间列，且正确传入信息
 * @param  source [IN]  CDataFrame句柄
 * @param  index [IN]  时间列的下标
 * @param  timeGap [IN]  重新采样时间间隔
 * @param  leftAsLabel [IN]  采样后样本标签选择
 * @param  leftClose [IN]  采样区间开闭情况
 * @retval #NO_ERROR 成功，返回n个DataFrame（包含所有重新采样分组情况）
 * @retval #NULL 失败
 */
DOPAI_CDF_GROUPBY *DOPAI_CDF_Resample(
    const DOPAI_CDF_CONTEXT *source, const uint32_t index, time_t timeGap, bool leftAsLabel, bool leftClose);

/**
 * @brief 对DataFrame列名进行设置
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 对DataFrame列名进行设置
 *
 * @attention
 * @param  source [OUT]  CDataFrame句柄
 * @param  columnNames [IN]  列名数组
 * @param  columnNameNum [IN]  列名数组长度
 * @retval #NO_ERROR 成功
 * @retval #NULL 失败
 */
int32_t DOPAI_CDF_SetColumnName(const DOPAI_CDF_CONTEXT *source, const char *columnNames[], uint32_t columnNameNum);
/**
 * @brief 从 Bool 类型的数组中读取数据
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 从 Bool 类型的数组中读取数据
 *
 * @attention
 * @param  df [OUT]  CDataFrame句柄
 * @param  data [IN]  数据数组头地址
 * @param  data [IN]  生成的DataFrame属性
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t ReadDataBool(DOPAI_CDF_CONTEXT *df, const bool *data, const DOPAI_CDF_CWrapper_InputAttr *input);
/**
 * @brief 从 Int8 类型的数组中读取数据
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 从 Int8 类型的数组中读取数据
 *
 * @attention
 * @param  df [OUT]  CDataFrame句柄
 * @param  data [IN]  数据数组头地址
 * @param  data [IN]  生成的DataFrame属性
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t ReadDataInt8(DOPAI_CDF_CONTEXT *df, const int8_t *data, const DOPAI_CDF_CWrapper_InputAttr *input);
/**
 * @brief 从 Uint8 类型的数组中读取数据
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 从 Uint8 类型的数组中读取数据
 *
 * @attention
 * @param  df [OUT]  CDataFrame句柄
 * @param  data [IN]  数据数组头地址
 * @param  data [IN]  生成的DataFrame属性
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t ReadDataUint8(DOPAI_CDF_CONTEXT *df, const uint8_t *data, const DOPAI_CDF_CWrapper_InputAttr *input);
/**
 * @brief 从 Int16 类型的数组中读取数据
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 从 Int16 类型的数组中读取数据
 *
 * @attention
 * @param  df [OUT]  CDataFrame句柄
 * @param  data [IN]  数据数组头地址
 * @param  data [IN]  生成的DataFrame属性
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t ReadDataInt16(DOPAI_CDF_CONTEXT *df, const int16_t *data, const DOPAI_CDF_CWrapper_InputAttr *input);
/**
 * @brief 从 Uint16 类型的数组中读取数据
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 从 Uint16 类型的数组中读取数据
 *
 * @attention
 * @param  df [OUT]  CDataFrame句柄
 * @param  data [IN]  数据数组头地址
 * @param  data [IN]  生成的DataFrame属性
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t ReadDataUint16(DOPAI_CDF_CONTEXT *df, const uint16_t *data, const DOPAI_CDF_CWrapper_InputAttr *input);
/**
 * @brief 从 Int32 类型的数组中读取数据
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 从 Int32 类型的数组中读取数据
 *
 * @attention
 * @param  df [OUT]  CDataFrame句柄
 * @param  data [IN]  数据数组头地址
 * @param  data [IN]  生成的DataFrame属性
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t ReadDataInt32(DOPAI_CDF_CONTEXT *df, const int32_t *data, const DOPAI_CDF_CWrapper_InputAttr *input);
/**
 * @brief 从 Uint32 类型的数组中读取数据
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 从 Uint32 类型的数组中读取数据
 *
 * @attention
 * @param  df [OUT]  CDataFrame句柄
 * @param  data [IN]  数据数组头地址
 * @param  data [IN]  生成的DataFrame属性
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t ReadDataUint32(DOPAI_CDF_CONTEXT *df, const uint32_t *data, const DOPAI_CDF_CWrapper_InputAttr *input);
/**
 * @brief 从 Int64 类型的数组中读取数据
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 从 Int64 类型的数组中读取数据
 *
 * @attention
 * @param  df [OUT]  CDataFrame句柄
 * @param  data [IN]  数据数组头地址
 * @param  data [IN]  生成的DataFrame属性
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t ReadDataInt64(DOPAI_CDF_CONTEXT *df, const int64_t *data, const DOPAI_CDF_CWrapper_InputAttr *input);
/**
 * @brief 从 Uint64 类型的数组中读取数据
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 从 Uint64 类型的数组中读取数据
 *
 * @attention
 * @param  df [OUT]  CDataFrame句柄
 * @param  data [IN]  数据数组头地址
 * @param  data [IN]  生成的DataFrame属性
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t ReadDataUint64(DOPAI_CDF_CONTEXT *df, const uint64_t *data, const DOPAI_CDF_CWrapper_InputAttr *input);
/**
 * @brief 从 Float 类型的数组中读取数据
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 从 Float 类型的数组中读取数据
 *
 * @attention
 * @param  df [OUT]  CDataFrame句柄
 * @param  data [IN]  数据数组头地址
 * @param  data [IN]  生成的DataFrame属性
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t ReadDataFloat(DOPAI_CDF_CONTEXT *df, const float *data, const DOPAI_CDF_CWrapper_InputAttr *input);
/**
 * @brief 从 Double 类型的数组中读取数据
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 从 Double 类型的数组中读取数据
 *
 * @attention
 * @param  df [OUT]  CDataFrame句柄
 * @param  data [IN]  数据数组头地址
 * @param  data [IN]  生成的DataFrame属性
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t ReadDataDouble(DOPAI_CDF_CONTEXT *df, const double *data, const DOPAI_CDF_CWrapper_InputAttr *input);
/**
 * @brief 从 String 类型的数组中读取数据
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 从 String 类型的数组中读取数据
 *
 * @attention
 * @param  df [OUT]  CDataFrame句柄
 * @param  data [IN]  数据数组头地址
 * @param  data [IN]  生成的DataFrame属性
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t ReadDataString(DOPAI_CDF_CONTEXT *df, const char **data, const DOPAI_CDF_CWrapper_InputAttr *input);

/**
 * @brief 求DataFrame列唯一值
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 根据colIndex，求DataFrame列唯一值
 *
 * @attention
 * @param  dst [OUT]  列名数组
 * @param  source [IN]  CDataFrame句柄
 * @param  colIndex [IN]  列序号
 * @retval #NO_ERROR 成功
 * @retval #NULL 失败
 */
int32_t DOPAI_CDF_Unique(DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source, const size_t colIndex);

int32_t DOPAI_CDF_Transpose(DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source);

/**
 * @brief 对DataFrame进行深拷贝
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 对DataFrame进行深拷贝
 *
 * @attention
 * @param  source [IN]  CDataFrame句柄
 * @param  dst [OUT]  列名数组
 * @retval #NO_ERROR 成功
 * @retval #NULL 失败
 */
int32_t DOPAI_CDF_DeepCopy(DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source);

/**
 * @brief 打印DataFrame的信息
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 打印DataFrame的信息
 *
 * @attention
 * @param  source [IN]  CDataFrame句柄
 */
void DOPAI_CDF_Show(const DOPAI_CDF_CONTEXT *source);

/**
 * @brief 对DataFrame进行插列
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 根据index插入列，将对原DataFrame进行修改
 *
 * @attention
 * @param  source [IN]  CDataFrame句柄
 * @param  rhs [IN]  插入的CDataFrame
 * @param  index [IN]  插入的序号
 * @retval #NO_ERROR 成功
 * @retval #NULL 失败
 */
int32_t DOPAI_CDF_InsertColInplace(DOPAI_CDF_CONTEXT *source, const DOPAI_CDF_CONTEXT *rhs, uint32_t index);

#ifdef __cplusplus
}
#endif

#endif