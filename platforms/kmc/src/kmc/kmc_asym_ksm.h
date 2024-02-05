/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: KMC asym ksm interface
 * Author: zhanghao
 * Create: 2022-9-26
 * Notes: New Create
 */

#ifndef KMC_ASYM_KSM_H
#define KMC_ASYM_KSM_H

#include "wsecv2_config.h"
#if WSEC_COMPILE_ENABLE_ASYM

#include "kmc_asym_type.h"
#include "kmc_ext_ksf.h"
#include "kmc_asym_utils.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

unsigned long MemBatchCreateAsymKey(KmcCbbCtx *kmcCtx, KmcAsymKeyInfoList *keyInfos, WsecBool shareDomainMkChanged);

unsigned long DoBatchActiveAsym(KmcCbbCtx *kmcCtx, const KmcAsymIdxList *keyParamList, WsecBool shareDomainMkChanged);

unsigned long MemGetPubInfoByHash(KmcCbbCtx *kmcCtx, KmcPubKeyIdx *keyParam, KmcPubKeyInfo *pubKeyInfo);

unsigned long KsmGetAsymKeyInfo(KmcCbbCtx *kmcCtx, WsecUint32 domainId, WsecUint32 keyId, KmcAsymKeyInfo *key);

unsigned long KsmGetAsymKeyStatus(KmcCbbCtx *kmcCtx, WsecUint32 domainId, WsecUint32 keyId, WsecUint32 *keyStatus);

unsigned long MemGetActiveInfoByDomain(KmcCbbCtx *kmcCtx, WsecUint32 domainId, KmcAsymActiveInfo *activeInfo);

WsecVoid DoRmvAsymByCountNotify(KmcCbbCtx *kmcCtx, const KmcNovaKeyInfoArray *rmvAsymInfoArr);

unsigned long MemRmvAsym(KmcCbbCtx *kmcCtx, const KmcAsymIdxList *keyIdxList, WsecBool shareDomainMkChanged,
    KmcNovaKeyInfoArray *rmvAsymInfoArr);

unsigned long MemBatchRmvAsymByCount(KmcCbbCtx *kmcCtx, const KmcRmvAsymDomainList *domainList,
    KmcNovaKeyInfoArray *rmvAsymInfoArr, WsecBool shareDomainMkChanged);

unsigned long MemSetAsymStatus(KmcCbbCtx *kmcCtx, const KmcAsymIdx *keyIdx, WsecBool shareDomainMkChanged,
    unsigned char status);

unsigned long MemCheckKeyPair(KmcCbbCtx *kmcCtx, const KmcAsymKeyPair *keyPair);

unsigned long MemGetActiveAsymKeyPair(KmcCbbCtx *kmcCtx, WsecUint32 domainId, WsecUint32 algType, WsecUint16 keyType,
    KmcMemNovaKey *asymRes);

unsigned long MemAsymSign(KmcCbbCtx *kmcCtx, WsecUint32 domainId, WsecUint32 algId, WsecUint32 algType,
    KmcMemNovaKey *asymRes, WsecBuffConst *plainBuff, WsecBuff *signBuff);

unsigned long KsmGetAsymCountByDomain(KmcCbbCtx *kmcCtx, WsecUint32 domainId, int *keyCount);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // WSEC_COMPILE_ENABLE_ASYM

#endif // KMC_ASYM_KSM_H
