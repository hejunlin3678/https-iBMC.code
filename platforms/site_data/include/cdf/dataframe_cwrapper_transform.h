/*******************************************************************************
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 *
 * -----------------------------------------------------------------------------
 *
 * dataframe_cwrapper_transform.h
 *
 * Project Code: DOPAI
 * Module Name:
 * Date Created: 2021-08-18
 * Author:
 * Description: CDF Transform 对外 C 接口
 *
 * -----------------------------------------------------------------------------
 * Modification History
 * DATE          NAME                       DESCRIPTION
 * -----------------------------------------------------------------------------
 *
 *******************************************************************************/

#ifndef DOPAI_CDATAFRME_TRANSFORM_CWRAPPER_H
#define DOPAI_CDATAFRME_TRANSFORM_CWRAPPER_H
#include <time.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "dataframe_c_type.h"
#include "dataframe_c_wrapper.h"
#ifdef __cplusplus
extern "C" {
#endif
/** @defgroup CDataFrame DataFrame C Interfaces */
/**
 * @ingroup CDataFrame
 *
 * CDF对外支持的算子
 *
 */
typedef enum {
    DOPAI_CDF_FUNCTOR_MAX = 0,
    DOPAI_CDF_FUNCTOR_MIN,
    DOPAI_CDF_FUNCTOR_STD,
    DOPAI_CDF_FUNCTOR_SUM,
    DOPAI_CDF_FUNCTOR_MEAN,
    DOPAI_CDF_FUNCTOR_ERROR
} DOPAI_CDF_FUNCTOR;

/**
 * @brief 分组操作后进行Transform操作
 *
 * @ingroup CDataFrame
 * @par 描述:
 * 分组操作后进行Transform操作
 *
 * @attention
 * @param  source [IN]  CDataFrame句柄
 * @param  indexes [IN]  分组依据的列下标数组
 * @param  indexLen [IN]  下标数组长度
 * @param  functor [IN]  Transform操作要使用的functor
 * @retval #NO_ERROR 成功，返回一个DataFrame（包含所有分组情况）
 * @retval #NULL 失败
 */
DOPAI_CDF_GROUPBY *DOPAI_CDF_GroupBy_Transform(
    const DOPAI_CDF_CONTEXT *source, const uint32_t *indexes, uint32_t indexLen, DOPAI_CDF_FUNCTOR functor);

#ifdef __cplusplus
}
#endif

#endif // DOPAI_CDATAFRME_TRANSFORM_CWRAPPER_H
