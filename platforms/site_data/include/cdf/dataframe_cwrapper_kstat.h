/*******************************************************************************
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 *
 * -----------------------------------------------------------------------------
 *
 * dataframe_cwrapper_kstat.h
 *
 * Project Code: DOPAI
 * Module Name:
 * Date Created: 2021-08-18
 * Author:
 * Description: CDF Kstat 对外 C 接口
 *
 * -----------------------------------------------------------------------------
 * Modification History
 * DATE          NAME                       DESCRIPTION
 * -----------------------------------------------------------------------------
 *
 *******************************************************************************/

#ifndef DOPAI_CDATAFRME_KSTATCWRAPPER_H
#define DOPAI_CDATAFRME_KSTATCWRAPPER_H
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
 * @brief 对DataFrame进行k-statistic的统计计算
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 对DataFrame进行k-statistic的统计计算
 *
 * @attention
 * @param  source [IN]  CDataFrame句柄
 * @param  k [IN]  k-statistic 的n值
 * @param  dst [OUT]  列名数组
 * @retval #NO_ERROR 成功
 * @retval #NULL 失败
 */
int32_t DOPAI_CDF_Kstat(DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source, uint32_t k);

#ifdef __cplusplus
}
#endif

#endif // DOPAI_CDATAFRME_KSTATCWRAPPER_H
