/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: KMC internal interface header file, which is not open to external systems.
 * Author: g00517475
 * Create: 2022-10-28
 */

#ifndef KMC_SRC_KMC_KMC_IMEXPORT_UTILS_H
#define KMC_SRC_KMC_KMC_IMEXPORT_UTILS_H

#include "wsecv2_type.h"
#include "wsecv2_array.h"
#include "wsecv2_sync_type.h"
#include "kmcv2_pri.h"
#include "cacv2_pri.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

unsigned long AddAndSwapAsymKeyWithKsfMem(KmcCbbCtx *kmcCtx, KmcKsfMem *ksfMem, KmcNovaKsfMem *novaKsfMem,
                                          WsecArray addKeyArr, WsecArray *dummyKeyArr);

/* Add Asym key to target KsfMem incremantaly, orignal MK not overwrite. Noting to do if the key is already exists. */
unsigned long AddAsymKeyToKsmByIncremental(KmcCbbCtx *kmcCtx, KmcKsfMem *targetKsfMem, WsecArray addKeyArray,
    KmcKsfMem *importKsfMem);

/* Add asym key to target KsfMem by full replacement, origin key will be overwrite by addMkArray */
unsigned long AddAsymKeyToKsmByFullReplace(KmcCbbCtx *kmcCtx, KmcKsfMem *targetKsfMem, WsecArray addKeyArray,
    KmcKsfMem *importKsfMem);

/* Filter the MK based on domainId and domainType. */
unsigned long FilterMasterKeys(KmcCbbCtx *kmcCtx, const WsecArray src, KmcNovaKsfMem *novaKsfMem, WsecUint32 domainId,
    WsecUint32 domainType, WsecArray *filterByDomainDst, WsecArray *filterDomainTypeDst,
    WsecArray *filterAsymKeyArrByDomain, WsecArray *filterAsymKeyByDomainType);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* KMC_SRC_KMC_KMC_IMEXPORT_UTILS_H */
