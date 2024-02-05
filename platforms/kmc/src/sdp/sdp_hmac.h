/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2014-2020. All rights reserved.
 * Description: header file of the external interfaces compatible with SDP V1
 * Author: z00518183
 * Create: 2014-06-16
 * History: 2018-10-08 Zhang Jie (employee ID: 00316590) UK rectification, compatible with SDP V1
 */

#ifndef KMC_SRC_SDP_HMAC_H
#define KMC_SRC_SDP_HMAC_H

#include "wsecv2_type.h"
#include "wsecv2_ctx.h"
#include "sdpv1_itf.h"
#include "sdpv2_itf.h"
#include "sdpv3_type.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef struct TagWsecBuffTypeExt {
    WsecUint32 type;
    unsigned char *buff;
    WsecUint32 *len;
} WsecBuffTypeExt;

unsigned long SdpGetHmacAlgAttrInternal(KmcCbbCtx *kmcCtx, WsecUint32 domain, WsecUint32 algId,  WsecUint32 version,
    WsecBuffTypeExt *hmacHeader);

unsigned long SdpGetHmacLenInternal(KmcCbbCtx *kmcCtx, WsecUint32 version, WsecUint32 *hmacLen);

unsigned long SdpHmacInternal(KmcCbbCtx *kmcCtx, WsecUint32 domain, WsecUint32 algId, WsecBuffConst *plainText,
    WsecUint32 version, WsecBuffExt *hmacData);

unsigned long SdpVerifyHmacV1(KmcCbbCtx *kmcCtx, WsecUint32 domain, const unsigned char *plainText,
    WsecUint32 plaintextLen, const unsigned char *hmacData, WsecUint32 hmacLen);

unsigned long SdpVerifyHmacV2(KmcCbbCtx *kmcCtx, const unsigned char *plainText, WsecUint32 plaintextLen,
    const unsigned char *hmacData, WsecUint32 hmacLen);

unsigned long SdpHmacInitV1(KmcCbbCtx *kmcCtx, WsecUint32 domain, const SdpHmacAlgAttributes *hmacAlgAttributes,
    WsecHandle *ctx);

unsigned long SdpHmacInitV2(KmcCbbCtx *kmcCtx, WsecUint32 domain, const void *hmacAlgAttr, WsecUint32 attrLen,
    WsecHandle *ctx);

unsigned long SdpHmacUpdateInternal(KmcCbbCtx *kmcCtx, WsecHandle *ctx, WsecUint32 version,
    const unsigned char *plainText, WsecUint32 plaintextLen);

unsigned long SdpHmacFinalInternal(KmcCbbCtx *kmcCtx, WsecHandle *ctx, WsecUint32 version, unsigned char *hmacData,
    WsecUint32 *hmacLen);

unsigned long SdpFileHmacInternal(KmcCbbCtx *kmcCtx, WsecUint32 domain, const char *file,
    const SdpHmacAlgAttributes *hmacAlgAttributes, WsecBuffConst *hmacExBuff,
    WsecBuffExt *hmac);

unsigned long SdpVerifyFileHmacInternal(KmcCbbCtx *kmcCtx, WsecUint32 domain, const char *file,
    const SdpHmacAlgAttributes *hmacAlgAttributes, WsecBuffConst *hmacAlgAttributesEx,
    WsecBuffConst *hmac);

unsigned long SdpGetMkDetailByHmacDataInternal(KmcCbbCtx *kmcCtx, WsecVoid *hmacData, WsecUint32 hmacLen,
    WsecUint32 version, KmcMkInfo *mkInfo);

unsigned long MultiSdpHmacInitV2(KmcCbbCtx *kmcCtx, WsecUint32 domain, const void *hmacAlgAttr,
    WsecUint32 attrLen, WsecHandle *ctx);

unsigned long MultiSdpHmacUpdateV2(KmcCbbCtx *kmcCtx, WsecHandle *ctx, const unsigned char *plainText,
    WsecUint32 plaintextLen);

unsigned long MultiSdpHmacFinalV2(KmcCbbCtx *kmcCtx, WsecHandle *ctx, unsigned char *hmacData, WsecUint32 *hmacLen);

unsigned long CheckSdpHmacHeader(KmcCbbCtx *kmcCtx, WsecUint32 version, WsecUint32 headerVersion, WsecUint32 algId,
    WsecUint32 iter);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* KMC_SRC_SDP_SDPV1_ITF_H */
