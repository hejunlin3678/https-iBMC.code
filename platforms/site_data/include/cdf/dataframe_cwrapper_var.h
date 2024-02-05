/*******************************************************************************
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 *
 * -----------------------------------------------------------------------------
 *
 * dataframe_cwrapper_var.h
 *
 * Project Code: DOPAI
 * Module Name:
 * Date Created: 2021-08-18
 * Author:
 * Description: CDF Var 对外 C 接口
 *
 * -----------------------------------------------------------------------------
 * Modification History
 * DATE          NAME                       DESCRIPTION
 * -----------------------------------------------------------------------------
 *
 *******************************************************************************/

#ifndef DOPAI_CDATAFRME_VARCWRAPPER_H
#define DOPAI_CDATAFRME_VARCWRAPPER_H
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
 * @brief 对DataFrame进行方差的统计计算
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 对DataFrame进行var的统计计算
 *
 * @attention
 * @param  source [IN]  CDataFrame句柄
 * @param  bias [IN]  表示δ自由度。计算中使用的除数是N-bias，其中N表示元素数。默认情况下，bias为1。
 * @param  dst [OUT]  列名数组
 * @retval #NO_ERROR 成功
 * @retval #NULL 失败
 */
int32_t DOPAI_CDF_Var(DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source, const size_t bias);

#ifdef __cplusplus
}
#endif

#endif // DOPAI_CDATAFRME_VARCWRAPPER_H
