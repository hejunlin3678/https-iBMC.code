/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Asymmetric cryptographic algorithm interface for KMC
 * Author: liuermeng
 * Create: 2022-08-05
 */


#ifndef CAC_ASYM_PRI_H
#define CAC_ASYM_PRI_H

#include "wsecv2_config.h"
#if WSEC_COMPILE_ENABLE_ASYM

#include <stdint.h>
#include "wsecv2_ctx.h"
#include "wsecv2_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

// rsa: for rsa, e is 0x10001, (n, e) is pub, (n, d) is private
// ecc sm2: pub: LVLV, pri: V
// ec25519: EVP_PKEY_get_raw_public_key EVP_PKEY_get_raw_private_key

// create asymmetric key pair and return pub and private
unsigned long CacCreateAsymKeyPair(KmcCbbCtx *kmcCtx, WsecKeySpec keyType, WsecBuff *pub, WsecBuff *pri);

// get Asymmetric key pubkey or prikey bin len
// for rsa, it's n or d's bytes num
// for ed25519, it's pub or pri key's bytes num
// for sm2 and ecc, it's X or Y or Private's bytes num
unsigned long CacGetAsymKeyPubLen(WsecKeySpec keyType, WsecUint32 *len);

unsigned long CacGetAsymKeyPriLen(KmcCbbCtx *kmcCtx, WsecKeySpec keyType, WsecUint32 *len);


// get sign len
unsigned long CacGetAsymSignLen(WsecKeySpec keyType, WsecUint32 *len);

// sign
unsigned long CacAsymSign(KmcCbbCtx *kmcCtx, WsecKeySpec keyType, WsecAlgId algo, WsecBuffConst *priKey,
    WsecBuffConst *plain, WsecBuff *sign);

// verify
unsigned long CacAsymVerify(KmcCbbCtx *kmcCtx, WsecKeySpec keyType, WsecAlgId algo, WsecBuffConst *pub,
    WsecBuffConst *plain, WsecBuffConst *sign);

// get encrypt ciphertxt len
unsigned long CacAsymGetCipherLen(WsecKeySpec keyType, WsecUint32 *len);

// encrypt
unsigned long CacAsymEncrypt(KmcCbbCtx *kmcCtx, WsecAlgId algo, WsecBuffConst *pub,
    WsecBuffConst *plain, WsecBuff *cipher);

// decrypt
unsigned long CacAsymDecrypt(KmcCbbCtx *kmcCtx, WsecAlgId algo, WsecBuffConst *priKey,
    WsecBuffConst *cipher, WsecBuff *plain);


unsigned long CacConvertTeeSign2Ree(KmcCbbCtx *kmcCtx, WsecBuff *teeSign, WsecUint32 bytes, WsecBuff *reeSign);

unsigned long CacConvertReeCipher2Tee(KmcCbbCtx *kmcCtx, WsecBuff *reeCipher, WsecBuff *teeCipher);

void CacOpensslFree(KmcCbbCtx *kmcCtx, void *ptr);

WsecUint32 CacGetMaxAsymPlainLen(KmcCbbCtx *kmcCtx, WsecUint32 algId, WsecUint16 keySpec);

unsigned long CacAsymPubI2d(KmcCbbCtx *kmcCtx, WsecKeySpec keySpec, WsecBuffConst *pub, WsecBuff *pubDer);
unsigned long CacAsymPubD2i(KmcCbbCtx *kmcCtx, WsecKeySpec keySpec, WsecBuffConst *pubDer, WsecBuff *pub);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif // WSEC_COMPILE_ENABLE_ASYM

#endif