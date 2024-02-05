/*******************************************************************************
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 *
 * -----------------------------------------------------------------------------
 *
 * dataframe_cwrapper_insertrow.h
 *
 * Project Code: DOPAI
 * Module Name:
 * Date Created: 2021-08-18
 * Author:
 * Description: CDF InsertRow 对外 C 接口
 *
 * -----------------------------------------------------------------------------
 * Modification History
 * DATE          NAME                       DESCRIPTION
 * -----------------------------------------------------------------------------
 *
 *******************************************************************************/
#ifndef DOPAI_CDATAFRME_INSERTROWCWRAPPER_H
#define DOPAI_CDATAFRME_INSERTROWCWRAPPER_H
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
 * @brief 对DataFrame进行插行
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 根据index插入行，不对原DataFrame进行修改
 *
 * @attention
 * @param  dst [OUT]  列名数组
 * @param  source [IN]  CDataFrame句柄
 * @param  rhs [IN]  插入的CDataFrame
 * @param  index [IN]  插入的序号
 * @retval #NO_ERROR 成功
 * @retval #NULL 失败
 */
int32_t DOPAI_CDF_InsertRow(
    DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *rhs, const DOPAI_CDF_CONTEXT *source, uint32_t index);
#ifdef __cplusplus
}
#endif
#endif // DOPAI_CDATAFRME_IDMAXCWRAPPER_H
