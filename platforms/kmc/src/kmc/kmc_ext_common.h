/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: KMC extKsf common interface
 * Author: zhanghao
 * Create: 2022-9-30
 * Notes: New Create
 */

#ifndef KMC_SRC_KMC_EXT_COMMON_H
#define KMC_SRC_KMC_EXT_COMMON_H

#include "wsecv2_ctx.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

unsigned long KmcImportExtKsfFromTee(KmcCbbCtx *kmcCtx, KmcKsfMem *ksfMem, KmcKsfMem *importKsf);

unsigned long KmcReadKsfByStatus(KmcCbbCtx *kmcCtx, const char *keystoreFile, const char *callBy, KmcKsfMem *ksfMem);

unsigned long CheckHasSymmKey(KmcCbbCtx *kmcCtx, KmcKsfMem *symmKsfMem, KmcKsfMem *asymKsfMem);

unsigned long CheckDomainKeyType(KmcCbbCtx *kmcCtx, WsecUint32 domainId, WsecBool isAsym);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // KMC_SRC_KMC_EXT_COMMON_H
