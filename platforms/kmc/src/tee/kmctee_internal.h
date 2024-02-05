/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: TA Implementation External Interface Layer
 * Author: liwei
 * Create: 2021-05-03
 */

#ifndef KMC_SRC_TEE_KMCTEE_INTERNAL_H
#define KMC_SRC_TEE_KMCTEE_INTERNAL_H

#include "kmctee_adapt.h"
#include "kmctee_status.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

KmcTaResult KmcTaGetMkCipherLen(WsecUint32 algId, WsecUint32 *mkLen, WsecUint32 hmacAlgId, WsecUint32 *hmacLen);

KmcTaResult KmcTaGetMkCipher(WsecBuffConst *mkInfo, WsecBuff *mkBuff, WsecBuff *hmac);

KmcTaResult KmcTaImpExpMk(const WsecUint32 *algId, const WsecUint32 *type, WsecBuffConst *rkInfo,
    WsecBuffConst *inData, WsecBuffConst *inMk, WsecBuffConst *protectKey,
    WsecBuff *outMk, WsecBuff *outHash, WsecBool isDeriveRk, WsecBuffConst *oldRkInfo);

KmcTaResult KmcTaExpWk(KmcTeeSdpInfo *info, WsecBuffConst *rkInfo, WsecBuffConst *inMk, WsecBuffConst *salt,
    WsecBuff *outWk);

KmcTaResult KmcTaCreateMk(const WsecUint32 *algId, const WsecUint32 *mkLen, WsecBuffConst *rkInfo, WsecBuff *outMk,
    WsecBuff *outHash);

KmcTaResult KmcTaHmac(KmcTeeSdpInfo *info, WsecBuffConst *rkInfo, WsecBuffConst *mk, WsecBuffConst *salt,
    WsecBuffConst *plainText, WsecBuff *hmac);

KmcTaResult KmcTaCreateRk(WsecUint8 *out, unsigned int *outLen, WsecUint8 *inRkInfo, WsecUint32 inRkInfoLen);

KmcTaResult KmcTaGetSupportDHAlg(unsigned char *supArray, WsecUint32 *algLen);

KmcTaResult KmcBatchExportPlainMkHash(WsecBuffConst *rk, WsecBuffConst *inMk, WsecBool isDeriveRk, WsecBuff *outHash);

KmcTaResult ImportPlainMK(WsecUint32 algId, WsecBuffConst *rk, WsecBuffConst *mk, WsecBuff *cipherMK,
                          WsecBuff *hash);
KmcTaResult ImportPlainAsymKey(WsecUint32 algId, WsecBuffConst *rk, WsecBuffConst *protectKey, WsecBuffConst *key,
                               WsecBuff *cipherKey, WsecBuff *hash);
KmcTaResult ExportPlainMk(WsecBuffConst *rk, WsecBuffConst *inMk, WsecBuff *outMk);

KmcTaResult KmcUpdateSrk(WsecBuffConst *rk);

KmcTaResult KmcTaEncryptAsymKey(const WsecUint32 *algId, WsecBuffConst *rkInfo, WsecBuffConst *protectKey,
                                WsecBuffConst *plainPri, WsecBuff *cipherPri);
#if WSEC_COMPILE_ENABLE_ASYM
KmcTaResult KmcTaCreateAsymKey(WsecUint32 algId, WsecUint32 keySpec,  WsecUint32 keyType, WsecBuffConst *rkInfo,
    WsecBuffConst *mkCipher, WsecBuff *pub, WsecBuff *pubHash, WsecBuff *pri, WsecBuff *priHash);

KmcTaResult KmcTaSignOrAsymDecrypt(WsecUint32 mode, WsecUint32 algId, WsecUint32 keySpec, WsecBuffConst *rkInfo,
    WsecBuffConst *mkCipher, WsecBuffConst *pub, WsecBuffConst *pri, WsecBuffConst *inBuff, WsecBuff *outBuff);
#endif // WSEC_COMPILE_ENABLE_ASYM

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // KMC_SRC_TEE_KMCTEE_INTERNAL_H
