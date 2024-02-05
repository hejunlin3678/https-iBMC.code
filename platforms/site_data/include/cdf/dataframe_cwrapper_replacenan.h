/*******************************************************************************
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 *
 * -----------------------------------------------------------------------------
 *
 * dataframe_cwrapper_replacenan.h
 *
 * Project Code: DOPAI
 * Module Name:
 * Date Created: 2021-08-18
 * Author:
 * Description: CDF ReplaceNan 对外 C 接口
 *
 * -----------------------------------------------------------------------------
 * Modification History
 * DATE          NAME                       DESCRIPTION
 * -----------------------------------------------------------------------------
 *
 *******************************************************************************/

#ifndef DOPAI_CDATAFRME_REPLACENANCWRAPPER_H
#define DOPAI_CDATAFRME_REPLACENANCWRAPPER_H
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
 * @brief 对DataFrame替换Nan值
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 对DataFrame替换Nan值
 *
 * @attention
 * @param  source [IN]  CDataFrame句柄
 * @param  dst [OUT]  CDataFrame句柄
 * @param  replaceNum [IN]  替换值
 * @param  length [IN]  长度
 * @retval #NO_ERROR 成功
 */
int32_t DOPAI_CDF_ReplaceNan(
    DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source, const char *replaceNum, const uint32_t length);

#ifdef __cplusplus
}
#endif

#endif // DOPAI_CDATAFRME_REPLACENANCWRAPPER_H
