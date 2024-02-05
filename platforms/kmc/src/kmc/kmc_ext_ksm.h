/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: KMC ext ksm interface
 * Author: zhanghao
 * Create: 2022-8-24
 * Notes: New Create
 */

#ifndef KMC_SRC_EXT__KSM_H
#define KMC_SRC_EXT__KSM_H

#include "wsecv2_type.h"
#include "wsecv2_ctx.h"
#include "kmc_ext_ksf.h"
#include "wsecv2_sync_type.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

WsecUint32 GetHashCtxNum(const KmcKsfMem *ksfMem);

WsecVoid FreeNovaKsfMem(WsecHandle kmcCtx, WsecVoid *element, WsecUint32 elementSize);

WsecVoid FreeKsfNovaKsfHeader(KmcCbbCtx *kmcCtx, KmcNovaKsfHeader *novaKsf);

unsigned long AddNovaKsfMemToArray(KmcCbbCtx *kmcCtx, KmcKsfMem *ksfMem, KmcNovaKsfMem *novaKsfMem);

WsecVoid FreeMemNovaKey(WsecHandle kmcCtx, WsecVoid *element, WsecUint32 elementSize);

WsecVoid FreeKsfNovaKey(WsecHandle kmcCtx, KmcKsfNovaKey *ksfNovaKey);

int CompareNovaKeyForArr(const WsecVoid *p1, const WsecVoid *p2);

unsigned long CloneMemNovaKey(KmcCbbCtx *kmcCtx, WsecBool withCache, const KmcMemNovaKey *src, KmcMemNovaKey **des);

unsigned long AddNovaKeyToArray(KmcCbbCtx *kmcCtx, KmcKsfMem *keystore, KmcNovaKsfMem *novaKsfMem,
    KmcMemNovaKey *memNovaKey, WsecBool alreadyProtected);

WsecVoid FreeKeyPair(WsecHandle kmcCtx, KmcNovaKeyPair *keyPair);

unsigned long CopyNovaKey(KmcCbbCtx *kmcCtx, const KmcNovaKey *keyA, KmcNovaKey *keyB);

unsigned long CopyNovaKeyPair(KmcCbbCtx *kmcCtx, const KmcNovaKeyPair *keyPairA, KmcNovaKeyPair *keyPairB);

unsigned long CheckResidualNovaKeyInDomain(KmcCbbCtx *kmcCtx, const KmcCfgDomainInfo *domainInfo);

unsigned long CheckNovaKeyLen(KmcCbbCtx *kmcCtx, const KmcCfgKeyType *keyTypeCfg);

unsigned long ExtMemCheckKeyTypeOverFlow(KmcCbbCtx *kmcCtx, WsecArray keyTypeArray, const KmcCfgKeyType *keyType);

WsecBool IsAsymKeyType(WsecUint16 keyType);

KmcNovaKsfMem *KmcGetNovaKsfMemByTag(const KmcKsfMem *ksfMem, unsigned char ksfTag);

int MemGetNovaCountByDomainAndTag(KmcCbbCtx *kmcCtx, WsecUint32 domainId, unsigned char ksfTag);

unsigned long CloneExtKsfMem(KmcCbbCtx *kmcCtx, WsecBool withAsym, WsecBool withCache, const KmcKsfMem *src,
    KmcKsfMem *des, KmcRecvSyncInfo *syncInfo, WsecBool isAsymFilter);
unsigned long CloneExtKsfMemOrCreate(KmcCbbCtx *kmcCtx, const KmcKsfMem *src, KmcKsfMem *des);
unsigned long ExtMemGetNovaKeyMinMax(KmcCbbCtx *kmcCtx, WsecArray novaKeyArray, WsecUint32 domainId,
    WsecUint32 *minId, WsecUint32 *maxId);

unsigned long KmcCreateNovaKsfMemByTag(KmcCbbCtx *kmcCtx, KmcKsfMem *ksfMem, unsigned char ksfTag,
    KmcNovaKsfMem **novaKsfMem);

KmcMemNovaKey *SearchAsymByKeyId(WsecArray novaKeyArray, WsecUint32 domainId, WsecUint32 keyId, int *keyIndex);

unsigned long KsmCheckKeyNumOverFlow(KmcCbbCtx *kmcCtx, const KmcKsfMem *keystore, const KmcExtMem *extMem, int addCount);

int KsmGetAsymNovaKeyCount(const KmcExtMem *extMem);

KmcNovaKsfMem *KsmGetAsymKsfMem(KmcCbbCtx *kmcCtx, KmcKsfMem *ksfMem);

/* Get asym by domainId and pubHash/priHash */
unsigned long PriGetMemAsymByHash(KmcCbbCtx *kmcCtx, const KmcNovaKsfMem *novaKsfMem, WsecUint32 domainId,
    WsecBuffConst *pubHash, WsecBuffConst *priHash, KmcMemNovaKey **foundAsym, int *index);

unsigned long CreateMemAysmFromPlainKey(KmcCbbCtx *kmcCtx, KmcNovaKeyInfo *aysmInfo, WsecBuff *pub, WsecBuff *pri,
    KmcKsfMem *ksfMem);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // KMC_SRC_EXT__KSM_H
