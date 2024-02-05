/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: TA Implementation Key derive
 * Author: liwei
 * Create: 2021-12-27
 */

#ifndef KMC_SRC_TEE_KMCTEE_KEYDERIVE_H
#define KMC_SRC_TEE_KMCTEE_KEYDERIVE_H

#include "kmctee_adapt.h"
#include "kmctee_status.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

KmcTaResult TaPbkdf2(WsecUint32 kdfAlg, WsecBuffConst *mk, WsecBuffConst *salt, WsecUint32 iter,
    WsecBuff *workKey);

KmcTaResult TaDerivedRootKey(WsecUint8 *keyBuf, WsecUint32 keyLen, const WsecUint8 *salt, WsecUint32 saltSize);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // KMC_SRC_TEE_KMCTEE_KEYDERIVE_H
