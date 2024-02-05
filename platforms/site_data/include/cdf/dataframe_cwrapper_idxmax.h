/*******************************************************************************
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 *
 * -----------------------------------------------------------------------------
 *
 * IdxMaxCwrapper.h
 *
 * Project Code: DOPAI
 * Module Name:
 * Date Created: 2021-08-18
 * Author:
 * Description: CDF IdxMax 对外 C 接口
 *
 * -----------------------------------------------------------------------------
 * Modification History
 * DATE          NAME                       DESCRIPTION
 * -----------------------------------------------------------------------------
 *
 *******************************************************************************/

#ifndef DOPAI_CDATAFRME_IDXMAXCWRAPPER_H
#define DOPAI_CDATAFRME_IDXMAXCWRAPPER_H
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
 * @brief 求DataFrame最大值的索引值
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 求DataFrame最大值的索引值
 *
 * @attention
 * @param  dst [OUT]  列名数组
 * @param  source [IN]  CDataFrame句柄
 * @retval #NO_ERROR 成功
 * @retval #NULL 失败
 */
int32_t DOPAI_CDF_IdxMax(DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source);

#ifdef __cplusplus
}
#endif

#endif // DOPAI_CDATAFRME_IDMAXCWRAPPER_H
