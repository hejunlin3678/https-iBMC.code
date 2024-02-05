/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: SDP V3 internal header file, which is not open to external systems.
 * Author: x00102361
 * Create: 2019-06-16
 * History: 2018-10-08 Zhang Jie (employee ID: 00316590) Rectification by UK
 */

#ifndef KMC_SRC_SDP_SDPV1_TLV_H
#define KMC_SRC_SDP_SDPV1_TLV_H

#include "wsecv2_util.h"
#include "wsecv2_ctx.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Read TLV */
WsecBool WsecReadTlv(KmcCbbCtx *kmcCtx, WsecHandle stream, WsecVoid *buff, WsecUint32 buffSize, WsecTlv *tlv,
    unsigned long *errCode);

/* Write TLV */
unsigned long WsecWriteTlv(KmcCbbCtx *kmcCtx, WsecHandle stream, WsecUint32 tag, WsecUint32 len, const void *val);

/* Convert to TLV */
void WsecCvtByteOrder4Tlv(WsecTlv *tlv, WsecUint32 direction);

/* Check the result and write the TLV. */
unsigned long CheckResultAndWriteTlv(KmcCbbCtx *kmcCtx, unsigned long result, WsecHandle writeStream,
    const unsigned char *val, WsecUint32 len, WsecUint32 tag);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* KMC_SRC_SDP_SDPV1_TLV_H */
