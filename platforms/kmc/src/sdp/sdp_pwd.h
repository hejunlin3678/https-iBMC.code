/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: This header file is functions of PWD
 * Author: yangchen
 * Create: 2021-04-16
 * History: None
 */

#ifndef KMC_SRC_SDP_SDP_PWD_H
#define KMC_SRC_SDP_SDP_PWD_H

#include "wsecv2_type.h"
#include "wsecv2_ctx.h"
#include "sdpv3_type.h"
#include "sdpv1_itf.h"
#include "sdpv2_itf.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* Ciphertext structure version */
#define SDP_PWD_CIPHER_VER 1
#define SDP_PWD_CIPHER_VER2 2

void SdpCvtByteOrderForSdpPwdHeaderEx(SdpPwdHeaderEx *headerEx, WsecUint32 direction);

void SdpCvtByteOrderForSdpPwdHeader(SdpPwdHeader *header, WsecUint32 direction);

unsigned long CheckPwdParams(KmcCbbCtx *kmcCtx, const unsigned char *plainText, WsecUint32 plainLen,
    const unsigned char *cipherText, WsecUint32 cipherLen, WsecUint32 pwdHeaderLen, WsecUint32 algId, WsecUint32 iter);

unsigned long SdpVerifyPwdPrepare(KmcCbbCtx *kmcCtx, const unsigned char *plainText, WsecUint32 plainLen,
    const unsigned char *cipherText, WsecUint32 cipherLen, SdpPwdHeader **header);

unsigned long DoSdpVerifyPwd(KmcCbbCtx *kmcCtx, WsecUint32 algId, WsecUint32 cipherLen, unsigned char **tempCipher,
    const unsigned char *plainText, WsecUint32 plainLen, const WsecBuffExt *saltData, int iter);

unsigned long SdpFillPwdCipherTextHeader(KmcCbbCtx *kmcCtx, WsecUint32 algId, WsecUint32 iter,
    unsigned char *cipherText, WsecUint32 cipherLen, WsecBuffExt *saltData, WsecUint32 version,
    WsecUint32 totalHeaderLen);

unsigned long MultiSdpVerifyPwdV1(KmcCbbCtx *kmcCtx, const unsigned char *plainText, WsecUint32 plainLen,
    const unsigned char *cipherText, WsecUint32 cipherLen);

unsigned long MultiSdpProtectPwdV2(KmcCbbCtx *kmcCtx, WsecUint32 algId, WsecUint32 iter, const unsigned char *plain,
    WsecUint32 plainLen, unsigned char *cipher, WsecUint32 cipherLen);

unsigned long MultiSdpVerifyPwdV2(KmcCbbCtx *kmcCtx, const unsigned char *plainText, WsecUint32 plainLen,
    const unsigned char *cipherText, WsecUint32 cipherLen);

unsigned long MultiSdpGetPwdCipherLen(KmcCbbCtx *kmcCtx, WsecUint32 passwordHashLen, WsecUint32 *pwdCipherLen);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* KMC_SRC_SDP_SDP_PWD_H */
