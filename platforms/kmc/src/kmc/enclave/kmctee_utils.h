/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: kmc tee utils interface
 * Author: yangchen
 * Create: 2022-09-30
 */

#ifndef KMC_SRC_KMC_ENCLAVE_KMCTEE_UTILS_H
#define KMC_SRC_KMC_ENCLAVE_KMCTEE_UTILS_H

#include "wsecv2_config.h"
#if WSEC_COMPILE_ENABLE_TEE
#include "wsecv2_ctx.h"
#include "wsecv2_type.h"
#include "tee_client_api.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

unsigned long KmcConvertGpToKmc(TEEC_Result result);

void KmcSetBuffArray(WsecBuffConst **buffArray, WsecUint32 num, ...);

unsigned long KmcConvertParamsToTlv(KmcCbbCtx *kmcCtx, unsigned char *tags, WsecUint32 tagNum,
    WsecBuffConst **buffArray, WsecVoid **tlvBuff, WsecUint32 *totalLen);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
#endif // KMC_SRC_KMC_ENCLAVE_KMCTEE_UTILS_H
