/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: TEE Asymmetric cryptographic algorithm interface for KMC
 * Author: liuermeng
 * Create: 2022-09-08
 */


#ifndef KMC_SRC_TEE_KMCTEE_ASYM_H
#define KMC_SRC_TEE_KMCTEE_ASYM_H

#include "kmctee_adapt.h"
#if WSEC_COMPILE_ENABLE_ASYM
#include "kmctee_status.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

KmcTaResult KmcTaAsymCreateKey(WsecUint32 keySpec, WsecUint32 keyUsage, WsecBuff *pubKey, WsecBuff *priKey);

KmcTaResult KmcTaAsymSign(WsecUint32 keySpec, WsecUint32 algo, WsecBuff *pubKey, WsecBuff *priKey, WsecBuff *plain,
    WsecBuff *sign);

KmcTaResult KmcTaAsymDecrypt(WsecUint32 keySpec, WsecUint32 algo, WsecBuff *pubKey, WsecBuff *priKey,
    WsecBuff *cipher, WsecBuff *plain);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif // WSEC_COMPILE_ENABLE_ASYM
#endif // KMC_SRC_TEE_KMCTEE_ASYM_H