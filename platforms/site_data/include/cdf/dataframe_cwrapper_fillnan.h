/*******************************************************************************
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 *
 * -----------------------------------------------------------------------------
 *
 * dataframe_cwrapper_fillnan.h
 *
 * Project Code: DOPAI
 * Module Name:
 * Date Created: 2021-08-18
 * Author:
 * Description: CDF FillNan 对外 C 接口
 *
 * -----------------------------------------------------------------------------
 * Modification History
 * DATE          NAME                       DESCRIPTION
 * -----------------------------------------------------------------------------
 *
 *******************************************************************************/

#ifndef DOPAI_CDATAFRME_FILLNANCWRAPPER_H
#define DOPAI_CDATAFRME_FILLNANCWRAPPER_H
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
 * @brief 对DataFrame进行填充Nan操作
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 对DataFrame进行填充Nan操作
 *
 * @attention
 * @param  source [IN]  CDataFrame句柄
 * @param  dst [OUT]  CDataFrame句柄
 * @param  fillType [IN]  填充方式，"ffill"或"bfill"
 * @param  len [IN]  必须为6
 * @retval #NO_ERROR 成功
 * @retval #NULL 失败
 */
int32_t DOPAI_CDF_FillNan(DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source, const char *fillType, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif // DOPAI_CDATAFRME_FILLNANCWRAPPER_H
