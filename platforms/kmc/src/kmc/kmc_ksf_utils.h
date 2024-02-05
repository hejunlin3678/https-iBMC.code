/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: KMC - Key Management Component - KSF Utils
 * Author: yangchen
 * Create: 2022-08-30
 * Notes: NA
 */
#ifndef KMC_SRC_KMC_KMC_KSF_UTIL_H
#define KMC_SRC_KMC_KMC_KSF_UTIL_H

#include "wsecv2_ctx.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

unsigned long TeeBatchGetMkHash(KmcCbbCtx *kmcCtx, KmcKsfMem *ksfMem);

unsigned long CheckRkPrimitive(KmcCbbCtx *kmcCtx, WsecSrkPrimitive *primitives);

unsigned long  KsfRecoverKsfFromOkFile(KmcCbbCtx *kmcCtx, const char *srcFile, char **destFiles, WsecUint32 destFilesNum);

unsigned long KsfRecoverFromAimFile(KmcCbbCtx *kmcCtx, KmcKsfMem * const *readBuff, const WsecUint32 *aimAt);

unsigned long KsfRecoverExtFile(KmcCbbCtx *kmcCtx, const KmcKsfMem *aimAtBuff, KmcKsfMem * const *extBuff,
    WsecUint32 extBlockSize, WsecUint32 aimAt, unsigned long *returnValue);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* KMC_SRC_KMC_KMC_KSF_UTIL_H */