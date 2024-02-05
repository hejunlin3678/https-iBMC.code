/*******************************************************************************
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 *
 * -----------------------------------------------------------------------------
 *
 * dataframe_cwrapper_round.h
 *
 * Project Code: DOPAI
 * Module Name:
 * Date Created: 2021-08-18
 * Author:
 * Description: CDF Round 对外 C 接口
 *
 * -----------------------------------------------------------------------------
 * Modification History
 * DATE          NAME                       DESCRIPTION
 * -----------------------------------------------------------------------------
 *
 *******************************************************************************/

#ifndef DOPAI_CDATAFRME_ROUNDCWRAPPER_H
#define DOPAI_CDATAFRME_ROUNDCWRAPPER_H
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
 * @brief 对DataFrame进行四舍五入计算
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 对DataFrame进行四舍五入计算
 *
 * @attention
 * @param  source [IN]  CDataFrame句柄
 * @param  round [IN]  第几位小数点处进行四舍五入
 * @param  dst [OUT]  列名数组
 * @retval #NO_ERROR 成功
 * @retval #NULL 失败
 */
int32_t DOPAI_CDF_Round(DOPAI_CDF_CONTEXT *dst, const DOPAI_CDF_CONTEXT *source, const size_t round);

#ifdef __cplusplus
}
#endif

#endif // DOPAI_CDATAFRME_ROUNDCWRAPPER_H
