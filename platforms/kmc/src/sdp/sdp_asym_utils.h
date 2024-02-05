/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: header file of asym sdp external interfaces
 * Author: w30012847
 * Create: 2022-11-01
 * History: new create
 */

#ifndef KMC_SRC_SDP_ASYM_UTILS_H
#define KMC_SRC_SDP_ASYM_UTILS_H

#include "wsecv2_config.h"
#if WSEC_COMPILE_ENABLE_ASYM
#include "wsecv2_ctx.h"
#include "sdp_asym_itf.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

WsecVoid SdpCvtByteOrderForAsymHeader(KmcAsymHeader *header, WsecUint32 direction);

unsigned long SdpGetAsymTotalHeaderLen(KmcCbbCtx *kmcCtx, const void *header, WsecUint32 headerLen, WsecUint32 *outLen);

unsigned long SdpFillAsymHeader(KmcCbbCtx *kmcCtx, KmcAsymHeader **header, WsecUint32 *headerLen,
    const unsigned char *text, WsecUint32 textLen);

unsigned long SdpSignPrepare(KmcCbbCtx *kmcCtx, KmcAsymHeader **header, WsecUint32 *headerLen,
    const unsigned char *signature, WsecUint32 signLen);

unsigned long SdpAsymDecPrepare(KmcCbbCtx *kmcCtx, const unsigned char *cipherText, WsecUint32 cipherLen,
    const unsigned char *plainText, const WsecUint32 *plainLen, KmcAsymHeader **header, WsecUint32 *headerLen,
    WsecUint32 *algType);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // WSEC_COMPILE_ENABLE_ASYM

#endif // KMC_SRC_SDP_ASYM_UTILS_H