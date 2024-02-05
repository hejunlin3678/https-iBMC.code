/*******************************************************************************
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 *
 * -----------------------------------------------------------------------------
 *
 * dataframe_cwrapper_isin.h
 *
 * Project Code: DOPAI
 * Module Name:
 * Date Created: 2021-08-18
 * Author:
 * Description: CDF IsIn 对外 C 接口
 *
 * -----------------------------------------------------------------------------
 * Modification History
 * DATE          NAME                       DESCRIPTION
 * -----------------------------------------------------------------------------
 *
 *******************************************************************************/

#ifndef DOPAI_CDATAFRME_ISINCWRAPPER_H
#define DOPAI_CDATAFRME_ISINCWRAPPER_H
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
 * @brief 判断DataFrame是否包含子DataFrame
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 判断DataFrame是否包含子DataFrame
 *
 * @attention
 * @param  source [IN]  CDataFrame句柄
 * @param  rhs [IN]  子CDataFrame句柄
 * @retval #NO_ERROR 成功
 * @retval #NULL 失败
 */
int32_t DOPAI_CDF_IsIn(DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source, const DOPAI_CDF_CONTEXT *rhs);

#ifdef __cplusplus
}
#endif

#endif // DOPAI_CDATAFRME_ISINCWRAPPER_H
