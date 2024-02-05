/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: KMC - Key Management Component - message notify
 * Author: yangdingfu
 * Create: 2021-04-23
 * Notes: New Create
 */
#ifndef KMC_SRC_KMC_KMC_NOTIFY_H
#define KMC_SRC_KMC_KMC_NOTIFY_H

#include "wsecv2_type.h"
#include "wsecv2_array.h"
#include "wsecv2_ctx.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* Notify the app that the MK has changed. */
WsecVoid KmcNotifyMkChanged(KmcCbbCtx *kmcCtx, const KmcMkInfo *mk, WsecUint32 type);

/* Notify the changed Mks */
WsecVoid KmcNotifyMkArrayChanged(KmcCbbCtx *kmcCtx, WsecArray changedArray, WsecBool setActive);

/* Notify the APP that the RK is about to expire. */
WsecVoid KmcNotifyRkExpire(KmcCbbCtx *kmcCtx, const KmcRkAttributes *rkAttributes, int remainLifeDays);

/* Notify update after write keystore file */
WsecVoid KmcNotifyUpdateAfterWriteFile(KmcCbbCtx *kmcCtx);

/* Notify the app that the ASYM has changed. */
WsecVoid KmcNotifyAsymChanged(KmcCbbCtx *kmcCtx, const KmcNovaKeyInfo *asym, WsecUint32 type);

/* Notify the changed ASYMs */
WsecVoid KmcNotifyAsymArrayChanged(KmcCbbCtx *kmcCtx, WsecArray changedArray, WsecBool setActive);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* KMC_SRC_KMC_KMC_NOTIFY_H */
