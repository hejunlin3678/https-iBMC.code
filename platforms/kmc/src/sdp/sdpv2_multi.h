/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: header file of SDP v2 and v2 multi instance common funcs
 * Author: yangchen
 * Create: 2021-08-30
 * History: None
 */


#ifndef KMC_SRC_SDP_SDPV2_MULTI_H
#define KMC_SRC_SDP_SDPV2_MULTI_H

#include "wsecv2_type.h"
#include "sdpv2_itf.h"
#include "sdpv3_type.h"
#include "wsecv2_ctx.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

unsigned long DoSdpVerifyHmacEx(KmcCbbCtx *kmcCtx, const SdpCipherHeaderEx *cipherHeader,
    const unsigned char *plainText, WsecUint32 plaintextLen,
    const unsigned char *hmacData, WsecUint32 hmacLen);

unsigned long MultiSdpGetCipherHeaderV2(KmcCbbCtx *kmcCtx, const unsigned char *ciphertext,
    WsecUint32 ciphertextLen, SdpCipherHeaderEx *cipherHeader);

unsigned long MultiSdpGetCipherDataLenV2(KmcCbbCtx *kmcCtx, WsecUint32 plaintextLen, WsecUint32 *ciphertextLenOut,
    WsecUint32 *headerLen);

unsigned long MultiSdpGetCipherDataLenWithHmacV2(KmcCbbCtx *kmcCtx, WsecUint32 plaintextLen,
    WsecUint32 *ciphertextLenOut);

unsigned long MultiSdpEncryptV2(KmcCbbCtx *kmcCtx, WsecUint32 domain, WsecUint32 algId,
    const unsigned char *plainText, WsecUint32 plaintextLen,
    unsigned char *ciphertext, WsecUint32 *ciphertextLen);

unsigned long DoSdpEncryptWithHmac(KmcCbbCtx *kmcCtx, WsecUint32 version, WsecUint32 domain,
    WsecUint32 cipherAlgId, WsecUint32 hmacAlgId, const unsigned char *plainText, WsecUint32 plaintextLen,
    unsigned char *ciphertext, WsecUint32 *ciphertextLen);

unsigned long MultiSdpDecrypt(KmcCbbCtx *kmcCtx, WsecUint32 domain, const unsigned char *ciphertext,
    WsecUint32 ciphertextLen, unsigned char *plainText, WsecUint32 *plaintextLen);

unsigned long SdpDecryptDataV2(KmcCbbCtx *kmcCtx, const unsigned char *ciphertext, WsecUint32 ciphertextLen,
    unsigned char *plainText, WsecUint32 *plaintextLen, WsecBuff *cipherHeader);

unsigned long MultiKmcGetMkDetailByCipher(KmcCbbCtx *kmcCtx, const unsigned char *cipherData,
    WsecUint32 cipherDataLen, KmcMkInfo *mkInfo);

unsigned long MultiSdpFileDecryptV2(KmcCbbCtx *kmcCtx, WsecUint32 domain, const char *cipherFile,
    const char *plainFile, const CallbackSetFileDateTime setFileDateTime);

unsigned long MultiSdpVerifyFileHmacV2(KmcCbbCtx *kmcCtx, WsecUint32 domain, const char *file,
    const void *hmacAlgAttr, WsecUint32 attrLen, const WsecVoid *hmacData, WsecUint32 hmacLen);

unsigned long SdpEncGetBodCipherHeaderLenV3(KmcCbbCtx *kmcCtx, WsecUint32 cipherAlgId, WsecUint32 hmacAlgId,
    WsecUint32 *outLen);

unsigned long CheckSdpEncryptInitV3Param(KmcCbbCtx *kmcCtx, SdpCipherCnfParam *cipherCnf, WsecUint32 inputLen,
    WsecUint32 *outLen);

unsigned long GetAndCheckSdpDecryptV2TlvLen(KmcCbbCtx *kmcCtx, const unsigned char *ciphertext,
    WsecUint32 ciphertextLen, WsecUint32 *headerLen);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* KMC_SRC_SDP_SDPV2_MULTI_H */
