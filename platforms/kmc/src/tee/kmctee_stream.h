/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: TA Stream Encryption and Decryption External Implementation
 * Author: liwei
 * Create: 2021-05-03
 */

#ifndef KMC_SRC_TEE_KMCTEE_STREAM_H
#define KMC_SRC_TEE_KMCTEE_STREAM_H

#include "kmctee_status.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

KmcTaResult KmcTaStreamInit(void *sessionContext, KmcTeeSdpInfo *info, WsecBuffConst *rkInfo, WsecBuffConst *mk,
    WsecBuffConst *salt, WsecBuffConst *iv, WsecBuff *ctx);

KmcTaResult KmcTaStreamUpdate(void *sessionContext, StreamInfo *streamInfo, WsecBuffConst *src, WsecBuff *dest);

KmcTaResult KmcTaStreamFinal(void *sessionContext, StreamInfo *streamInfo, WsecBuffConst *src, WsecBuff *dest,
    WsecBuff *tag);

KmcTaResult KmcTaFreeCtx(void *sessionContext, const void *ctx);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // KMC_SRC_TEE_KMCTEE_STREAM_H
