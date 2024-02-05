/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: kmc tee enc dec interface
 * Author: liwei
 * Create: 2022-10-10
 */

#ifndef KMC_SRC_KMC_ENCLAVE_KMCV3_TEE_SYM_H
#define KMC_SRC_KMC_ENCLAVE_KMCV3_TEE_SYM_H

#include "wsecv2_config.h"
#if WSEC_COMPILE_ENABLE_TEE
#include "wsecv2_ctx.h"
#include "kmcv2_pri.h"
#include "kmctee_pri.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

unsigned long KmcCaHmac(KmcCbbCtx *kmcCtx, const KmcTeeSdpInfo *info, WsecBuffConst *rk, WsecBuffConst *mk,
    WsecBuffConst *salt, WsecBuffConst *plainText, WsecBuff *hmac);

#ifdef WSEC_DEBUG
unsigned long KmcCaEncrypt(KmcCbbCtx *kmcCtx, KmcTeeSdpInfo *info, WsecBuffConst *rk, WsecBuffConst *mk,
    WsecBuffConst *salt, WsecBuffConst *iv, WsecBuffConst *plainText, WsecBuff *cipherText);

unsigned long KmcCaDecrypt(KmcCbbCtx *kmcCtx, KmcTeeSdpInfo *info, WsecBuffConst *rk, WsecBuffConst *mk,
    WsecBuffConst *salt, WsecBuffConst *iv, WsecBuffConst *cipherText, WsecBuff *plainText);

unsigned long KmcCaStreamInit(KmcCbbCtx *kmcCtx, KmcTeeSdpInfo *info, WsecBuffConst *rkInfo, WsecBuffConst *mk,
    WsecBuffConst *salt, WsecBuffConst *iv, void **ctx);

unsigned long KmcCaStreamUpdate(KmcCbbCtx *kmcCtx, WsecUint32 flag, WsecHandle ctx, WsecBuffConst *src, WsecBuff *dest);

unsigned long KmcCaStreamFinal(KmcCbbCtx *kmcCtx, WsecUint32 flag, WsecHandle *ctx, WsecBuffConst *src, WsecBuff *dest,
    WsecBuff *tag);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
#endif
#endif // KMC_SRC_KMC_ENCLAVE_KMCV3_TEE_SYM_H
