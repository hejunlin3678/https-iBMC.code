/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: TA Implementation External Interface Layer
 * Author: liwei
 * Create: 2021-05-03
 */

#ifndef KMC_SRC_TEE_KMCTEE_PROTECT_H
#define KMC_SRC_TEE_KMCTEE_PROTECT_H

#include "kmctee_status.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

KmcTaResult KmcTaEncrypt(KmcTeeSdpInfo *info, WsecBuffConst *rkInfo, WsecBuffConst *mk, WsecBuffConst *salt,
                         WsecBuffConst *iv, WsecBuffConst *plainText, WsecBuff *cipherText);

KmcTaResult KmcTaDecrypt(KmcTeeSdpInfo *info, WsecBuffConst *rkInfo, WsecBuffConst *mk, WsecBuffConst *salt,
                         WsecBuffConst *iv, WsecBuffConst *cipherText, WsecBuff *plainText);

KmcTaResult KmcTaSkEncryptMk(KmcTeeSdpInfo *info, WsecBuffConst *rkSalt, WsecBuffConst *salt, WsecBuffConst *protectKey,
                             WsecBuffConst *skEncByRk, WsecBuffConst *inKey, WsecBuff *outKey);

KmcTaResult KmcTaSkDecryptKey(KmcTeeSdpInfo *info, WsecUint32 *alg, WsecBuffConst *rkSalt, WsecBuffConst *salt,
                              WsecBuffConst *protectKey, WsecBuffConst *skEncByRk, WsecBuffConst *inKey, WsecBuff *outKey);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // KMC_SRC_TEE_KMCTEE_PROTECT_H
