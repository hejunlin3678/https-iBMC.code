/*******************************************************************************
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 *
 * -----------------------------------------------------------------------------
 *
 * dataframe_cwrapper_rollingmean.h
 *
 * Project Code: DOPAI
 * Module Name:
 * Date Created: 2021-08-18
 * Author:
 * Description: CDF RollingMean 对外 C 接口
 *
 * -----------------------------------------------------------------------------
 * Modification History
 * DATE          NAME                       DESCRIPTION
 * -----------------------------------------------------------------------------
 *
 *******************************************************************************/

#ifndef DOPAI_CDATAFRME_ROLLINGMEANCWRAPPER_H
#define DOPAI_CDATAFRME_ROLLINGMEANCWRAPPER_H
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
 * @brief 对DataFrame进行RollingMean计算
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 对DataFrame进行RollingMean计算
 *
 * @attention
 * @param  source [IN]  CDataFrame句柄
 * @param  dst [OUT]  CDataFrame句柄
 * @param  window [IN]  窗口的大小
 * @param  center [IN]  是否取中间值
 * @param  minPeriods [IN]  最小的观察数值个数
 * @retval #NO_ERROR 成功
 * @retval #NULL 失败
 */
int32_t DOPAI_CDF_RollingMean(DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source, const uint32_t window,
    const bool center, const uint32_t minPeriods);

#ifdef __cplusplus
}
#endif

#endif // DOPAI_CDATAFRME_ROLLINGMEANCWRAPPER_H
