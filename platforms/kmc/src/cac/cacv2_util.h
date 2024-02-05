/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: cac util functions
 * Author: xuhuiyue
 * Create: 2021-05-22
 * History: none
 */

#ifndef KMC_SRC_CAC_CACV2_UTIL_H
#define KMC_SRC_CAC_CACV2_UTIL_H

#include "wsecv2_type.h"
#include "wsecv2_ctx.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* Random number generator access lock */
WsecVoid CacThreadLockRand(KmcCbbCtx *kmcCtx);

/* Random number generator access unlock */
WsecVoid CacThreadUnlockRand(KmcCbbCtx *kmcCtx);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* KMC_SRC_CAC_CACV2_UTIL_H */
