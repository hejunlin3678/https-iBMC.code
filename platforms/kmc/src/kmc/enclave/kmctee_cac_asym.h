/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: TEE Asymmetric Key Operation
 * Author: lanming
 * Create: 2022-09-26
 */
#ifndef KMC_SRC_KMC_ENCLAVE_KMCTEE_CAC_ASYM_H
#define KMC_SRC_KMC_ENCLAVE_KMCTEE_CAC_ASYM_H

#include <stdint.h>
#include "wsecv2_config.h"
#if WSEC_COMPILE_ENABLE_TEE
#include "wsecv2_type.h"
#include "cacv2_pri.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if WSEC_COMPILE_ENABLE_ASYM
unsigned long KmcCaEnvCreateAsymKey(KmcCbbCtx *kmcCtx, WsecUint32 algId, WsecUint32 keySpec, WsecUint32 keyType,
    const BuildRkParam *rkParam, const BuildMkParam *mkParam, WsecBuff *pub, WsecBuff *pubHash, WsecBuff *pri,
    WsecBuff *priHash);

unsigned long KmcCaEnvSign(KmcCbbCtx *kmcCtx, WsecUint32 keySpec, WsecUint32 algId, const BuildRkParam *rkParam,
    const BuildMkParam *mkParam, WsecBuffConst *pub, WsecBuffConst *pri, WsecBuffConst *plain, WsecBuff *sign);

unsigned long KmcCaEnvAsymDecrypt(KmcCbbCtx *kmcCtx, WsecUint32 algId, WsecUint32 keySpec, const BuildRkParam *rkParam,
    const BuildMkParam *mkParam, WsecBuffConst *pub, WsecBuffConst *pri, WsecBuffConst *cipher, WsecBuff *plain);
#endif // WSEC_COMPILE_ENABLE_ASYM

unsigned long KmcCaEnvEncryptAsymKey(KmcCbbCtx *kmcCtx, WsecUint32 algId, WsecBuffConst *rkInfo,
                                     WsecBuffConst *mkCipher, WsecBuffConst *plainKey, WsecBuff *cipher);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif // WSEC_COMPILE_ENABLE_TEE
#endif // KMC_SRC_KMC_ENCLAVE_KMCTEE_CAC_ASYM_H
