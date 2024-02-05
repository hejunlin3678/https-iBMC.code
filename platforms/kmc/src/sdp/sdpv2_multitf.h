/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: header file of SDP V2 multi instance external interfaces
 * Author: yangchen
 * Create: 2021-08-14
 * History: None
 */

#ifndef KMC_SRC_SDP_SDPV2_MULTITF_H
#define KMC_SRC_SDP_SDPV2_MULTITF_H

#include "wsecv2_type.h"
#include "sdpv3_type.h"
#include "sdpv2_itf.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

unsigned long SdpGetCipherHeaderMul(WsecHandle kmcCtx, const unsigned char *ciphertext, WsecUint32 ciphertextLen,
    SdpCipherHeaderEx *cipherHeader);

unsigned long SdpGetMkDetailByCipherMul(WsecHandle kmcCtx, const unsigned char *cipherData, WsecUint32 cipherDataLen,
    KmcMkInfo *mkInfo);

unsigned long SdpGetCipherDataLenMul(WsecHandle kmcCtx, WsecUint32 plaintextLen, WsecUint32 *ciphertextLenOut);

unsigned long SdpEncryptMul(WsecHandle kmcCtx, WsecUint32 domain, WsecUint32 algId, const unsigned char *plainText,
    WsecUint32 plaintextLen, unsigned char *ciphertext, WsecUint32 *ciphertextLen);

unsigned long SdpDecryptMul(WsecHandle kmcCtx, WsecUint32 domain, const unsigned char *ciphertext,
    WsecUint32 ciphertextLen, unsigned char *plainText, WsecUint32 *plaintextLen);

unsigned long SdpEncryptWithHmacMul(WsecHandle kmcCtx, WsecUint32 domain, WsecUint32 cipherAlgId, WsecUint32 hmacAlgId,
    const unsigned char *plainText, WsecUint32 plaintextLen, unsigned char *ciphertext, WsecUint32 *ciphertextLen);

unsigned long SdpGetCipherDataLenWithHmacMul(WsecHandle kmcCtx, WsecUint32 plaintextLen, WsecUint32 *ciphertextLenOut);

/* according to passwordHashLen get pwdCipherLen */
unsigned long SdpGetPwdCipherLenMul(WsecHandle kmcCtx, WsecUint32 passwordHashLen, WsecUint32 *pwdCipherLen);

unsigned long SdpProtectPwdMul(WsecHandle kmcCtx, WsecUint32 algId, WsecUint32 iter, const unsigned char *plain,
    WsecUint32 plainLen, unsigned char *cipher, WsecUint32 cipherLen);

unsigned long SdpVerifyPwdMul(WsecHandle kmcCtx, const unsigned char *plainText, WsecUint32 plainLen,
    const unsigned char *cipherText, WsecUint32 cipherLen);

unsigned long SdpGetMkDetailByHmacDataMul(WsecHandle kmcCtx, WsecVoid *hmacData, WsecUint32 hmacLen, KmcMkInfo *mkInfo);

unsigned long SdpGetMacLenForEncryptMul(WsecHandle kmcCtx, WsecUint32 cipherAlgId, WsecUint32 hmacAlgId,
    WsecUint32 *macLen);

unsigned long SdpHmacExMul(WsecHandle kmcCtx, WsecUint32 domain, WsecUint32 algId, const unsigned char *plainText,
    WsecUint32 plaintextLen, unsigned char *hmacData, WsecUint32 *hmacLen);

unsigned long SdpVerifyHmacMul(WsecHandle kmcCtx, WsecUint32 domain, const unsigned char *plainText,
    WsecUint32 plaintextLen, const unsigned char *hmacData, WsecUint32 hmacLen);

unsigned long SdpGetHmacLenExMul(WsecHandle kmcCtx, WsecUint32 *hmacLen);

unsigned long SdpEncGetHmacHeaderLenV3Mul(WsecHandle kmcCtx, WsecUint32 *outLen);

unsigned long SdpGetHmacAlgAttrMul(WsecHandle kmcCtx, WsecUint32 domain, WsecUint32 algId,
    SdpHmacAlgAttributesEx *hmacAlgAttributes);
unsigned long SdpGetHmacAlgAttrV3Mul(WsecHandle kmcCtx, WsecUint32 domain, WsecUint32 algId,
    WsecVoid *hmacAlgAttributes, WsecUint32 *attrLen);

unsigned long SdpHmacInitMul(WsecHandle kmcCtx, WsecUint32 domain, const void *hmacAlgAttributes, WsecUint32 attrLen,
    WsecHandle *ctx);

unsigned long SdpHmacUpdateMul(WsecHandle kmcCtx, WsecHandle *ctx, const unsigned char *plain, WsecUint32 plainLen);

unsigned long SdpHmacFinalMul(WsecHandle kmcCtx, WsecHandle *ctx, unsigned char *hmacData, WsecUint32 *hmacLen);

unsigned long SdpFileHmacMul(WsecHandle kmcCtx, WsecUint32 domain, const char *file,
    const SdpHmacAlgAttributesEx *hmacAlgAttributes, WsecVoid *hmacData, WsecUint32 *hmacLen);

unsigned long SdpFileHmacV3Mul(WsecHandle kmcCtx, WsecUint32 domain, const char *file,
    WsecBuffConst *hmacAlgAttributes, WsecBuff *hmacBuff);

unsigned long SdpVerifyFileHmacMul(WsecHandle kmcCtx, WsecUint32 domain, const char *file,
    const WsecVoid *hmacAlgAttributes, WsecUint32 attrLen, const WsecVoid *hmacData, WsecUint32 hmacLen);

/* Get BodcipherHeaderLen by version before encrypt */
unsigned long SdpEncGetBodHeaderLenV3Mul(WsecHandle kmcCtx, WsecUint32 cipherAlgId, WsecUint32 hmacAlgId,
    WsecUint32 *outLen);

unsigned long SdpEncryptInitMul(WsecHandle kmcCtx, WsecUint32 domain, WsecUint32 cipherAlgId, WsecUint32 hmacAlgId,
    WsecHandle *ctx, SdpBodCipherHeaderEx *bodCipherHeader);

unsigned long SdpEncryptInitV3Mul(WsecHandle kmcCtx, SdpCipherCnfParam *cipherCnf,
    WsecHandle *ctx, WsecBuff *bodCipherHeader);

unsigned long SdpEncryptUpdateMul(WsecHandle kmcCtx, WsecHandle *ctx, const unsigned char *plainText,
    WsecUint32 plainLen, unsigned char *cipherText, WsecUint32 *cipherLen);

unsigned long SdpEncryptFinalMul(WsecHandle kmcCtx, WsecHandle *ctx, unsigned char *cipherText, WsecUint32 *cipherLen,
    unsigned char *hmacText, WsecUint32 *hmacLen);

unsigned long SdpDecryptInitMul(WsecHandle kmcCtx, WsecUint32 domain, WsecHandle *ctx, const WsecVoid *bodCipherHeader,
    WsecUint32 bodHeaderLen);

unsigned long SdpDecryptUpdateMul(WsecHandle kmcCtx, WsecHandle *ctx, const unsigned char *cipherText,
    WsecUint32 cipherLen, unsigned char *plainText, WsecUint32 *plainLen);

unsigned long SdpDecryptFinalMul(WsecHandle kmcCtx, WsecHandle *ctx, const unsigned char *hmacText, WsecUint32 hmacLen,
    unsigned char *plainText, WsecUint32 *plainLen);

unsigned long SdpFileEncryptMul(WsecHandle kmcCtx, WsecUint32 domain, WsecUint32 cipherAlgId, WsecUint32 hmacAlgId,
    const char *plainFile, const char *cipherFile, const CallbackGetFileDateTime getFileDateTime);

unsigned long SdpFileDecryptMul(WsecHandle kmcCtx, WsecUint32 domain, const char *cipherFile, const char *plainFile,
    const CallbackSetFileDateTime setFileDateTime);

unsigned long SdpGetMkDetailByBodCipherHeaderExMul(WsecHandle kmcCtx, WsecVoid *bodCipherHeader,
    WsecUint32 bodCipherLen, KmcMkInfo *mkInfo);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* KMC_SRC_SDP_SDPV2_MULTITF_H */
