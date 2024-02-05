/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: KMC extKsf utils interface
 * Author: zhanghao
 * Create: 2022-9-15
 * Notes: New Create
 */

#ifndef KMC_SRC_KMC_EXT_KSF_UTILS_H
#define KMC_SRC_KMC_EXT_KSF_UTILS_H

#include "wsecv2_type.h"
#include "wsecv2_ctx.h"
#include "kmc_ext_ksf.h"
#include "kmcv2_pri.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

unsigned long ReadLvWithoutDigest(KmcCbbCtx *kmcCtx, WsecHandle ksf, WsecVoid *buff, WsecUint32 *len);

unsigned long WriteLv(KmcCbbCtx *kmcCtx,  WsecHandle ksf, WsecHandle *hashCtx, WsecUint32 ctxNum,
    WsecVoid *buff, WsecUint32 len);

unsigned long WriteLvWithOutHash(KmcCbbCtx *kmcCtx, WsecHandle ksf, WsecVoid *buff, WsecUint32 len);

unsigned long GetIvAndPlainKey(KmcCbbCtx *kmcCtx, WsecUint16 ksfVersion, KmcNovaKsfMem *novaKsfMem, WsecUint32 algId,
    unsigned char **iv, WsecUint32 *ivLen, unsigned char **tag, WsecUint32 *tagLen, unsigned char **key, WsecUint32 *keyLen);

unsigned long CalKsfHmac(KmcCbbCtx *kmcCtx, const KmcKsfRk *rk, const KmcKsfMem *ksfMem, WsecBuff *rmkBuff,
    WsecUint32 hmacAlg, KmcKsfHmac *ksfHmac, WsecUint32 cacHashLen, WsecUint32 *cacHmacLen);

unsigned long ParseTeeMk(KmcCbbCtx *kmcCtx, const unsigned char *teeKey, WsecUint32 teeKeyLen, WsecUint32 *algId,
    unsigned char **iv, WsecUint32 *ivLen, unsigned char **tag, WsecUint32 *tagLen,
    unsigned char *key, WsecUint32 *keyLen);

unsigned long FillTeeMk(KmcCbbCtx *kmcCtx, const unsigned char *key, WsecUint32 keyLen, WsecUint32 algId,
    const unsigned char *iv, WsecUint32 ivLen, const unsigned char *tag, WsecUint32 tagLen,
    unsigned char *teeKey, WsecUint32 *teeKeyLen);

unsigned long KmcEncBySrkWithTag(KmcCbbCtx *kmcCtx, const KmcKsfHardRk *hardRk, WsecUint32 algId,
    const unsigned char *iv, WsecUint32 ivLen, const unsigned char *plaintext, WsecUint32 plaintextLen,
    unsigned char *ciphertext, WsecUint32 *ciphertextLen, unsigned char *tag, WsecUint32 tagLen);

unsigned long KmcDecBySrkWithTag(KmcCbbCtx *kmcCtx, const KmcKsfHardRk *hardRk, WsecUint32 algId,
    const unsigned char *iv, WsecUint32 ivLen, unsigned char *tag, WsecUint32 tagLen,
    const unsigned char *cipher, WsecUint32 cipherLen, unsigned char *plain, WsecUint32 *plainLen);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // KMC_SRC_KMC_EXT_KSF_UTILS_H
