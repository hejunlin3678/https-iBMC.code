/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: KMC KmcProcLockKeystore
 * Author: yangchen
 * Create: 2022-10-10
 * History: NA
 */


#ifndef KMC_SRC_KMC_KMCV2_LOCK_H
#define KMC_SRC_KMC_KMCV2_LOCK_H

#include "wsecv2_ctx.h"
#include "wsecv2_type.h"
#include "kmcv2_pri.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

WsecVoid KmcProcLockKeystore(KmcCbbCtx *kmcCtx);

WsecVoid KmcProcUnlockKeystore(KmcCbbCtx *kmcCtx);

WsecVoid KmcThreadLock(KmcCbbCtx *kmcCtx, KmcLockOrNot lockOrNot, KmcLockType type);

WsecVoid KmcThreadUnlock(KmcCbbCtx *kmcCtx, KmcLockOrNot lockOrNot, KmcLockType type);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* KMC_SRC_KMC_KMCV2_LOCK_H */
