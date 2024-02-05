/*******************************************************************************
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 *
 * -----------------------------------------------------------------------------
 *
 * dataframe_cwrapper_skew.h
 *
 * Project Code: DOPAI
 * Module Name:
 * Date Created: 2021-08-18
 * Author:
 * Description: CDF Skew 对外 C 接口
 *
 * -----------------------------------------------------------------------------
 * Modification History
 * DATE          NAME                       DESCRIPTION
 * -----------------------------------------------------------------------------
 *
 *******************************************************************************/

#ifndef DOPAI_CDATAFRME_SKEWCWRAPPER_H
#define DOPAI_CDATAFRME_SKEWCWRAPPER_H
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
 * @brief 对DataFrame进行偏度的统计计算
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 对DataFrame进行skew的统计计算
 *
 * @attention
 * @param  source [IN]  CDataFrame句柄
 * @param  skipNa [IN]  是否调过Nan数值，整数的0会被识别为Nan
 * @param  bias [IN]  是否进行无偏估计；是，false，对标Pandas结果；否，true，对标Scipy结果
 * @param  dst [OUT]  列名数组
 * @retval #NO_ERROR 成功
 * @retval #NULL 失败
 */
int32_t DOPAI_CDF_Skew(DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source, bool skipNa, bool bias);

#ifdef __cplusplus
}
#endif

#endif // DOPAI_CDATAFRME_SKEWCWRAPPER_H
