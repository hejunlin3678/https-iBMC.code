/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: KMC asym interface
 * Author: liwei
 * Create: 2022-9-19
 * Notes: New Create
 */

#ifndef KMC_ASYM_AYSM_UTILS_H
#define KMC_ASYM_AYSM_UTILS_H

#include "wsecv2_config.h"
#if WSEC_COMPILE_ENABLE_ASYM

#include "kmc_ext_ksm.h"
#include "kmc_ext_ksf.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef enum {
    WSEC_ASYM_ALG_RSA = 1,
    WSEC_ASYM_ALG_EC,
    WSEC_ASYM_ALG_ED,
    WSEC_ASYM_ALG_SM2,
} WsecAsymAlgType;

typedef struct TagKmcNovaKeyInfoArray {
    KmcNovaKeyInfo *keyInfos;
    int keyCount;
} KmcNovaKeyInfoArray;


unsigned long UpdateAsymProtectKey(KmcCbbCtx *kmcCtx);

unsigned long KmcAsymSign(KmcCbbCtx *kmcCtx, WsecUint32 domainId, WsecUint32 algId, WsecUint32 algType,
    KmcMemNovaKey *asymRes, WsecBuffConst *plainBuff, WsecBuff *signBuff);

unsigned long KmcGetPubKeyByIDHash(KmcCbbCtx *kmcCtx, WsecUint32 domainId, WsecUint32 keyId,
    const unsigned char *pubHash, WsecUint32 pubHashLen, unsigned char *pubKey, WsecUint32 *pubKeyLen);

KmcMemNovaKey *SearchNovaKeyByKeyId(KmcCbbCtx *kmcCtx, unsigned char ksfTag, WsecUint32 domainId, WsecUint32 keyId);

WsecVoid MemCheckIfAsymExpired(KmcCbbCtx *kmcCtx, KmcNovaKeyInfo *novaKey, KmcUseExpiredAsymNotify *notify);

unsigned long KmcGetActiveAsymKey(KmcCbbCtx *kmcCtx, WsecUint32 domainId, WsecUint32 algType, KmcMemNovaKey *asymKey);

unsigned long KmcAsymDecrypt(KmcCbbCtx *kmcCtx, WsecUint32 algId, WsecUint32 domainId, WsecUint32 keyId,
    const unsigned char *pubIndex, WsecUint32 pubIndexLen, WsecBuffConst *cipher, WsecBuff *plain);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // WSEC_COMPILE_ENABLE_ASYM

#endif // KMC_ASYM_AYSM_UTILS_H
