/*******************************************************************************
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 *
 * -----------------------------------------------------------------------------
 *
 * dataframe_cwrapper_diff.h
 *
 * Project Code: DOPAI
 * Module Name:
 * Date Created: 2021-08-18
 * Author:
 * Description: CDF Diff 对外 C 接口
 *
 * -----------------------------------------------------------------------------
 * Modification History
 * DATE          NAME                       DESCRIPTION
 * -----------------------------------------------------------------------------
 *
 *******************************************************************************/

#ifndef DOPAI_CDATAFRME_DIFFCWRAPPER_H
#define DOPAI_CDATAFRME_DIFFCWRAPPER_H
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
 * @brief 对DataFrame进行两两相减计算
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 对DataFrame进行两两相减计算
 *
 * @attention
 * @param  source [IN]  CDataFrame句柄
 * @param  shift [IN]  与间隔为shift的数字进行两两相减
 * @param  dst [OUT]  CDataFrame句柄
 * @retval # 0 成功
 * @retval #非0 失败
 */
int32_t DOPAI_CDF_Diff(DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source, const int32_t shift);

#ifdef __cplusplus
}
#endif

#endif // DOPAI_CDATAFRME_DIFFCWRAPPER_H
