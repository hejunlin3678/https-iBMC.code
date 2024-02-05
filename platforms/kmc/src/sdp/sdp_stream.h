/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: This header file is public functions of stream enc and dec.
 * Author: yangchen
 * Create: 2022-04-13
 * History: None
 */

#ifndef KMC_SRC_SDP_SDP_STREAM_H
#define KMC_SRC_SDP_SDP_STREAM_H

#include "wsecv2_ctx.h"
#include "wsecv2_type.h"
#include "sdpv3_type.h"
#include "sdpv1_itf.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* v1 */
unsigned long MultiSdpEncryptInitV1(KmcCbbCtx *kmcCtx, WsecUint32 domain, WsecUint32 cipherAlgId, WsecUint32 hmacAlgId,
    WsecHandle *ctx, SdpBodCipherHeader *bodCipherHeader);

unsigned long MultiSdpEncryptUpdateV1(KmcCbbCtx *kmcCtx, WsecHandle *ctx, const unsigned char *plainText,
    WsecUint32 plainLen, unsigned char *cipherText, WsecUint32 *cipherLen);

unsigned long MultiSdpEncryptFinalV1(WsecHandle kmcCtx, WsecHandle *ctx, unsigned char *cipherText,
    WsecUint32 *cipherLen, unsigned char *hmacText, WsecUint32 *hmacLen);

unsigned long MultiSdpDecryptInitV1(KmcCbbCtx *kmcCtx, WsecUint32 domain, WsecHandle *ctx,
    const SdpBodCipherHeader *bodCipherHeader);

unsigned long MultiSdpDecryptUpdateV1(KmcCbbCtx *kmcCtx, WsecHandle *ctx, const unsigned char *cipherText,
    WsecUint32 cipherLen, unsigned char *plainText, WsecUint32 *plainLen);

unsigned long MultiSdpDecryptFinalV1(KmcCbbCtx *kmcCtx, WsecHandle *ctx, const unsigned char *hmacText,
    WsecUint32 hmacLen, unsigned char *plainText, WsecUint32 *plainLen);

/* v2 */
unsigned long MultiSdpEncryptInitV2(KmcCbbCtx *kmcCtx, WsecUint32 domain, WsecUint32 cipherAlgId, WsecUint32 hmacAlgId,
    WsecHandle *ctx, WsecBuff *bodHeader);

unsigned long MultiSdpEncryptUpdateV2(KmcCbbCtx *kmcCtx, WsecHandle *ctx, const unsigned char *plainText,
    WsecUint32 plainLen, unsigned char *cipherText, WsecUint32 *cipherLen);

unsigned long MultiSdpEncryptFinalV2(KmcCbbCtx *kmcCtx, WsecHandle *ctx, unsigned char *cipherText,
    WsecUint32 *cipherLen, unsigned char *hmacText, WsecUint32 *hmacLen);

unsigned long MultiSdpDecryptInitV2(KmcCbbCtx *kmcCtx, WsecUint32 domain, WsecHandle *ctx,
    const WsecVoid *bodCipherHeader, WsecUint32 bodHeaderLen);

unsigned long MultiSdpDecryptUpdateV2(KmcCbbCtx *kmcCtx, WsecHandle *ctx, const unsigned char *cipherText,
    WsecUint32 cipherLen, unsigned char *plainText, WsecUint32 *plainLen);

unsigned long MultiSdpDecryptFinalV2(KmcCbbCtx *kmcCtx, WsecHandle *ctx, const unsigned char *hmacText,
    WsecUint32 hmacLen, unsigned char *plainText, WsecUint32 *plainLen);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* KMC_SRC_SDP_SDP_STREAM_H */
