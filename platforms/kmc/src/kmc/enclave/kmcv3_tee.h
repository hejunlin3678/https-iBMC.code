/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: kmc tee interface
 * Author: liwei
 * Create: 2021-05-03
 */

#ifndef KMC_SRC_KMC_ENCLAVE_KMCV3_TEE_H
#define KMC_SRC_KMC_ENCLAVE_KMCV3_TEE_H

#include "wsecv2_config.h"
#if WSEC_COMPILE_ENABLE_TEE
#include <stdint.h>
#include "wsecv2_ctx.h"
#include "wsecv2_type.h"
#include "kmcv2_pri.h"
#include "tee_client_api.h"
#include "kmctee_pri.h"
#include "cacv2_pri.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* CA-TA max transmission length */
#define TLV_TAG_NUM_EIGHT 8
#define TLV_TAG_NUM_SEVEN 7
#define TLV_TAG_NUM_SIX 6
#define TLV_TAG_NUM_FIVE 5
#define TLV_TAG_NUM_FOUR 4
#define TLV_TAG_NUM_THREE 3

typedef struct TagTeeContext {
    TEEC_UUID uuid;
    TEEC_Context ctx;
    TEEC_Session session;
} TeeContext;

unsigned long KmcCaInitTee(KmcCbbCtx *kmcCtx, const KmcTeeInitParam *teeParam);

unsigned long KmcCaFinalTee(KmcCbbCtx *kmcCtx);

unsigned long KmcCaCreateRk(KmcCbbCtx *kmcCtx, WsecBuff *inRkInfo, WsecBuff *outRkInfo);

unsigned long KmcCaImpExpMk(KmcCbbCtx *kmcCtx, WsecUint32 algId, WsecUint32 flag, const WsecBuffConst *inRk,
    WsecBuffConst *protectKey, WsecBuffConst *oldRk, const WsecBuffConst *inData, const WsecBuffConst *inKey,
    WsecBuff *outKey, WsecBuff *outHash, WsecUint32 isDeriveRk);

unsigned long KmcCaExpWk(KmcCbbCtx *kmcCtx, const KmcTeeSdpInfo *info, WsecBuffConst *rkInfo, WsecBuffConst *inMk,
    WsecBuffConst *salt, WsecBuff *outWk);

unsigned long KmcCaBatchGetMkHash(KmcCbbCtx *kmcCtx, const WsecBuffConst *inRk, const WsecBuffConst *inMk,
    WsecUint32 isDeriveRk, WsecBuff *mkHash);

unsigned long KmcCaUpdateSrk(KmcCbbCtx *kmcCtx, const KmcKsfMem *ksfMem);

unsigned long KmcCaUpdImpMk(KmcCbbCtx *kmcCtx, ImpExpMkType mkType, KmcKsfMem *ksfMem, WsecBuffConst *oldRk);
unsigned long KmcCaImpUpdAsym(KmcCbbCtx *kmcCtx, ImpExpMkType mkType, KmcKsfMem *ksfMem, WsecBuffConst *protectKey,
    WsecBuffConst *oldKey, KmcNovaKsfMem *novaKsfMem);

WsecVoid KmcCaFreeCtx(KmcCbbCtx *kmcCtx, WsecHandle *ctx);

unsigned long KmcCaCreateMk(KmcCbbCtx *kmcCtx, WsecUint32 algId, WsecUint32 mkLen, WsecBuffConst *rkInfo,
    WsecBuff *outMk, WsecBuff *outHash);

unsigned long KmcCaMCreateDHKeyPair(KmcCbbCtx *kmcCtx, const KmcDHParam *dhParam, WsecBuff *outDHPub,
                                    uint64_t *object);
unsigned long KmcCaACreateDHShareKey(KmcCbbCtx *kmcCtx, const KmcDHParam *dhParam, WsecBuffConst *inDHPub,
                                     WsecBuff *outDHPub, WsecBuff *outSKEnc);
unsigned long KmcCaMCreateDHShareKey(KmcCbbCtx *kmcCtx, const KmcDHParam *dhParam, uint64_t object,
    WsecBuffConst *inDHPub, WsecBuff *outSKEnc);
unsigned long KmcCaSkEncryptMk(KmcCbbCtx *kmcCtx, const KmcTeeSdpInfo *info, WsecBuffConst *rk, WsecBuffConst *salt,
    WsecBuffConst *protectKey, WsecBuffConst *skEnc, WsecBuffConst *inMK, WsecBuff *outMK);
unsigned long KmcCaSkDecryptMk(KmcCbbCtx *kmcCtx, const KmcTeeSdpInfo *info, WsecUint32 alg, WsecBuffConst *rk,
    WsecBuffConst *salt, WsecBuffConst *protectKey, WsecBuffConst *skEnc, WsecBuffConst *inMK, WsecBuff *outMK);
WsecVoid KmcCaFreeDhCtx(KmcCbbCtx *kmcCtx, WsecHandle ctx);

unsigned long KmcCaGetSupportDHAlg(KmcCbbCtx *kmcCtx, unsigned char *algArr, WsecUint32 *algLen);

#if WSEC_COMPILE_ENABLE_ASYM
unsigned long KmcCaCreateAsymKey(KmcCbbCtx *kmcCtx, WsecUint32 algId, WsecUint32 keySpec, WsecUint32 keyType,
    WsecBuffConst *rkInfo, WsecBuffConst *mkCipher, WsecBuff *asymKeyInfo, WsecBuff *lenBuff);

unsigned long KmcCaSign(KmcCbbCtx *kmcCtx, WsecUint32 algId, WsecUint32 keySpec, WsecBuffConst *rkInfo,
    WsecBuffConst *mkCipher, WsecBuffConst *pub, WsecBuffConst *pri, WsecBuffConst *plain, WsecBuff *sign);

unsigned long KmcCaAsymDecrypt(KmcCbbCtx *kmcCtx, WsecUint32 algId, WsecUint32 keySpec, WsecBuffConst *rkInfo,
    WsecBuffConst *mkCipher, WsecBuffConst *pub, WsecBuffConst *pri, WsecBuffConst *cipher, WsecBuff *plain);
#endif // WSEC_COMPILE_ENABLE_ASYM

unsigned long KmcCaEncryptAsymKey(KmcCbbCtx *kmcCtx, WsecUint32 algId, WsecBuffConst *rkInfo, WsecBuffConst *mkCipher,
                                  WsecBuffConst *plainKey, WsecBuff *outBuff);
unsigned long KmcCaUpdateNovaKey(KmcCbbCtx *kmcCtx, WsecBuffConst *newRk, WsecBuffConst *oldRk, KmcKsfMem *ksfMem);
unsigned long KmcTeeUpdateRootKey(KmcCbbCtx *kmcCtx, KmcKsfMem *ksfMem, WsecBuffConst *oldRk);
unsigned long KmcUpGradeToTee(KmcCbbCtx *kmcCtx, KmcKsfMem *ksfMem);
unsigned long KmcNovaKeyUpGradeTee(KmcCbbCtx *kmcCtx, WsecBuffConst *rk, KmcKsfMem *ksfMem);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
#endif // KMC_SRC_KMC_ENCLAVE_KMCV3_TEE_H
