/*******************************************************************************
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 *
 * -----------------------------------------------------------------------------
 *
 * dataframe_cwrapper_sortvalue.h
 *
 * Project Code: DOPAI
 * Module Name:
 * Date Created: 2021-08-18
 * Author:
 * Description: CDF SortValue 对外 C 接口
 *
 * -----------------------------------------------------------------------------
 * Modification History
 * DATE          NAME                       DESCRIPTION
 * -----------------------------------------------------------------------------
 *
 *******************************************************************************/

#ifndef DOPAI_CDATAFRME_SORTVALUECWRAPPER_H
#define DOPAI_CDATAFRME_SORTVALUECWRAPPER_H
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
 * @brief 对数据进行排序
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 对数据进行排序
 *
 * @attention 无
 * @param  source [IN]  CDataFrame句柄
 * @param  dst [OUT]  CDataFrame句柄
 * @param  ascending [IN] 对数据进行升序排列或者降序排列
 * @retval #SLICED_DATAFRAME_AS_DST 失败
 */
int32_t DOPAI_CDF_SortValue(DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source, bool ascending);

#ifdef __cplusplus
}
#endif

#endif // DOPAI_CDATAFRME_SORTVALUECWRAPPER_H
