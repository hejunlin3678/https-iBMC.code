/*******************************************************************************
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 *
 * -----------------------------------------------------------------------------
 *
 * dataframe_cwrapper_reorder.h
 *
 * Project Code: DOPAI
 * Module Name:
 * Date Created: 2021-08-18
 * Author:
 * Description: CDF Reorder 对外 C 接口
 *
 * -----------------------------------------------------------------------------
 * Modification History
 * DATE          NAME                       DESCRIPTION
 * -----------------------------------------------------------------------------
 *
 *******************************************************************************/

#ifndef DOPAI_CDATAFRME_REORDERCWRAPPER_H
#define DOPAI_CDATAFRME_REORDERCWRAPPER_H
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
 * @brief 对DataFrame进行重排序
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 根据rhs的值为序号对DataFrame进行重排序
 *
 * @attention
 * @param  dst [OUT]  列名数组
 * @param  source [IN]  CDataFrame句柄
 * @param  rhs [IN]  重排序的序号
 * @retval #NO_ERROR 成功
 * @retval #NULL 失败
 */
int32_t DOPAI_CDF_Reorder(DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source, const DOPAI_CDF_CONTEXT *rhs);

#ifdef __cplusplus
}
#endif

#endif // DOPAI_CDATAFRME_REORDERCWRAPPER_H
