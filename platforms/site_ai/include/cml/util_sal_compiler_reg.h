/*******************************************************************************
*
*  Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
*
* -------------------------------------------------------------------------------
*
*                              util_sal_compiler_reg.h
*
* Project Code: DOPAI
* Module Name: util
* Date Created: 2020-02-17
* Author: wutao
* Description: DOPAI Abstraction Compiler layer Resigter Interface
*
* -------------------------------------------------------------------------------
*  Modification History
*  DATE          NAME                       DESCRIPTION
*  -----------------------------------------------------------------------------
*
*******************************************************************************/

#ifndef DOPAI_SAL_COMPILER_REG_H
#define DOPAI_SAL_COMPILER_REG_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DOPAI_PUBLIC                     __attribute__((visibility("default")))
#define DOPAI_LIKELY(x)                  __builtin_expect((x), 1)
#define DOPAI_UNLIKELY(x)                __builtin_expect((x), 0)
#define DOPAI_PREFETCH(addr)             __builtin_prefetch((addr), 0, 3)

#if defined(_MSC_VER)
#define DOPAI_UNUSED
#else
#define DOPAI_UNUSED                     __attribute__((unused))
#endif

#ifdef __cplusplus
}
#endif

#endif /* DOPAI_SAL_COMPLIE_REG_H */
