/*******************************************************************************
*
*  Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
*
* -------------------------------------------------------------------------------
*
*                              util_sal_mem_reg.h
*
*  Project Code: DOPAI
*   Module Name: util
*  Date Created: 2020-02-17
*        Author: wutao
*   Description: DOPAI Abstraction Memory layer Resigter Interface
*
* -------------------------------------------------------------------------------
*  Modification History
*  DATE          NAME                       DESCRIPTION
*  -----------------------------------------------------------------------------
*
*******************************************************************************/

#ifndef DOPAI_UTIL_SAL_MEM_REG_H
#define DOPAI_UTIL_SAL_MEM_REG_H

#include <stddef.h>
#include "util_error_code.h"

/** @defgroup MemoryReg DOPAI内存注册接口 */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup MemoryReg
 *
 * DOPAI_SAL_MemCb: 内存回调函数结构体定义
 */
typedef struct Tag_DOPAI_SAL_MemCb {
    void *(*mMallocAlign)(size_t size, size_t align); /**< 申请对齐内存的回调函数 */
    void  (*mFreeAlign)(void *ptr);                   /**< 释放对齐内存的回调函数 */
    void *(*mMalloc)(size_t size);                    /**< 申请内存的回调函数 */
    void  (*mFree)(void *ptr);                        /**< 释放存的回调函数 */
} DOPAI_SAL_MemCb;

/**
 * @ingroup MemoryReg
 * @brief 用于向DOPAI注册内存的接口
 * @par 描述：
 * 当用户需要使用自有内存模块接管SiteAI内存模块时, 用自定义的函数指针构成DOPAI_SAL_MemCb, 并调用接口完成内存注册
 * @attention
 * 无
 * @param memCb[IN] 内存回调函数结构体
 * @retval #UTIL_NO_ERROR 成功
 * @retval #UTIL_MEMREG_REG_FAIL 失败, 回调函数中包含空指针
 * @par 依赖: <ul><li>util_sal_mem_reg.h:该接口所在的头文件。</li></ul>
 * @since V100R021
 * @see 无
 */
UtilErrorCode DOPAI_SAL_MemRegister(const DOPAI_SAL_MemCb *memCb);

#ifdef __cplusplus
}
#endif

#endif /* DOPAI_SAL_MEM_REG_H */
