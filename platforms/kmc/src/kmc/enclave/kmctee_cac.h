/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description:
 * Author: yangchen
 * Create: 2022-04-11
 */
#ifndef KMC_SRC_KMC_ENCLAVE_KMCTEE_CAC_H
#define KMC_SRC_KMC_ENCLAVE_KMCTEE_CAC_H

#include <stdint.h>
#include "wsecv2_config.h"
#if WSEC_COMPILE_ENABLE_TEE
#include "wsecv2_type.h"
#include "wsecv2_itf.h"
#include "cacv2_pri.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

unsigned long KmcCaEnvTeeInit(KmcCbbCtx *kmcCtx, const KmcTeeInitParam *teeParam);

unsigned long KmcCaEnvTeeUninit(KmcCbbCtx *kmcCtx);

unsigned long KmcCaEnvCreateRk(KmcCbbCtx *kmcCtx, unsigned char *rk, WsecUint32 *rkLen, WsecSrkPrimitive *rkPrimitive);

unsigned long KmcCaEnvCreateMk(KmcCbbCtx *kmcCtx, WsecUint32 inMkLen, const BuildRkParam *rkParam,
    BuildMkParam *mkParam);

unsigned long KmcCaEnvRegisterMk(KmcCbbCtx *kmcCtx, WsecUint32 flag, const WsecBuffConst *dataBuff,
    WsecBuffConst *inMk, const BuildRkParam *rkParam, BuildMkParam *mkParam);

unsigned long KmcCaEnvPbkdf2(KmcCbbCtx *kmcCtx, BuildMkParam *mkParam, BuildWkParam *wkParam);

unsigned long KmcCaEnvHmac(KmcCbbCtx *kmcCtx, CacCryptoParam *hmacParam, const unsigned char *inBuff, WsecUint32 inLen,
    unsigned char *outHmac, WsecUint32 *outLen);

unsigned long KmcCaEnvUnprotectData(KmcCbbCtx *kmcCtx, const unsigned char *dataIn, unsigned int inLen,
    unsigned char *dataOut, unsigned int *outLen);

unsigned long KmcCaEnvMCreateDHKeyPair(KmcCbbCtx *kmcCtx, WsecUint32 alg, unsigned char *pubKey, WsecUint32 *pubKeyLen,
                                       uint64_t *object);
unsigned long KmcCaEnvACreateDHShareKey(KmcCbbCtx *kmcCtx, WsecUint32 alg,
                                        const unsigned char *inPubKey, WsecUint32 inPubKeyLen,
                                        unsigned char *outPubKey, WsecUint32 *outPubKeyLen,
                                        unsigned char *shareKey, WsecUint32 *shareKeyLen);
unsigned long KmcCaEnvMCreateDHShareKey(KmcCbbCtx *kmcCtx, WsecUint32 alg, const unsigned char *pubKey,
    WsecUint32 pubKeyLen, unsigned char *shareKey, WsecUint32 *shareKeyLen, uint64_t object);
unsigned long KmcCaEnvSkEncryptMk(KmcCbbCtx *kmcCtx, const KmcSynEncInfo *encInfo, WsecBuffConst *protectKey,
                                  const unsigned char *shareKey, WsecUint32 shareKeyLen,
                                  const unsigned char *inMK, WsecUint32 inMkLen,
                                  unsigned char *outMK, WsecUint32 *outMkLen);
unsigned long KmcCaEnvSkDecryptMk(KmcCbbCtx *kmcCtx, const KmcSynEncInfo *encInfo, WsecBuffConst *protectKey,
                                  const unsigned char *shareKey, WsecUint32 shareKeyLen,
                                  const unsigned char *inMK, WsecUint32 inMkLen,
                                  unsigned char *outMK, WsecUint32 *outMkLen);
unsigned long KmcCaEnvExpWk(KmcCbbCtx *kmcCtx, const EncDecParam *param, const unsigned char *mk, WsecUint32 mkLen,
    unsigned char *key, WsecUint32 *keyLen);
unsigned long KmcCaEnvGetIvLen(WsecUint32 algId, WsecUint32 *ivLen);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif // WSEC_COMPILE_ENABLE_TEE
#endif // KMC_SRC_KMC_ENCLAVE_KMCTEE_CAC_H
