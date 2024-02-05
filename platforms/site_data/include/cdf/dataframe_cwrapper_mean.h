/*******************************************************************************
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 *
 * -----------------------------------------------------------------------------
 *
 * dataframe_cwrapper_mean.h
 *
 * Project Code: DOPAI
 * Module Name:
 * Date Created: 2021-08-18
 * Author:
 * Description: CDF Mean 对外 C 接口
 *
 * -----------------------------------------------------------------------------
 * Modification History
 * DATE          NAME                       DESCRIPTION
 * -----------------------------------------------------------------------------
 *
 *******************************************************************************/

#ifndef DOPAI_CDATAFRME_MEANCWRAPPER_H
#define DOPAI_CDATAFRME_MEANCWRAPPER_H
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
 * @brief 对DataFrame进行平均值的统计计算
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 对DataFrame进行mean的统计计算
 *
 * @attention
 * @param  source [IN]  CDataFrame句柄
 * @param  dst [OUT]  列名数组
 * @retval #NO_ERROR 成功
 * @retval #NULL 失败
 */
int32_t DOPAI_CDF_Mean(DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source);

#ifdef __cplusplus
}
#endif

#endif // DOPAI_CDATAFRME_MEANCWRAPPER_H
