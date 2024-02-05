/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: KMC extKsf interface
 * Author: zhanghao
 * Create: 2022-8-19
 * Notes: New Create
 */

#ifndef KMC_SRC_KMC_EXT_KSF_H
#define KMC_SRC_KMC_EXT_KSF_H

#include "wsecv2_type.h"
#include "wsecv2_ctx.h"
#include "wsecv2_ext_type.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


unsigned long KmcReadExtKsf(KmcCbbCtx *kmcCtx, WsecHandle ksf, WsecHandle *hashCtx, WsecUint32 ctxNum,
    const KmcKsfRk *rk, WsecBuff *rmkBuff, KmcKsfMem *ksfMem, WsecUint32 *encAlg, const char *callBy, WsecBool *isAsym);

unsigned long KmcWriteExtKsf(KmcCbbCtx *kmcCtx, WsecHandle ksf, WsecHandle *hashCtx, WsecUint32 ctxNum,
    KmcKsfMem *ksfMem, KmcKsfRk *rkWrite, WsecBuff *rmkBuff, const char *call);

unsigned long CopyExtKsfMaster(KmcCbbCtx *kmcCtx, const char *masterFile);

WsecVoid CopyExtKsfBackup(KmcCbbCtx *kmcCtx, const char *masterFile);

unsigned long CloneExtKsfName(KmcCbbCtx *kmcCtx, KmcKsfName *extFileName);

unsigned long KmcImportExtKsf(KmcCbbCtx *kmcCtx, KmcKsfMem *ksfMem, WsecBool *needWrite);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // KMC_SRC_KMC_EXT_KSF_H
