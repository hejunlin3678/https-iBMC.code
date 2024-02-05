/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 */
#ifndef KMC_SRC_TEE_KMCTEE_SYNC_H
#define KMC_SRC_TEE_KMCTEE_SYNC_H

#include "kmctee_status.h"

#define KMC_ECDH_X25519_KEY_SIZE 256
#define KMC_ECC_256_KEY_SIZE 256
#define KMC_ECC_384_KEY_SIZE 384
#define KMC_ECC_521_KEY_SIZE 521
#define KMC_ECC_MAX_SHARE_KEY_SIZE 100

KmcTaResult KmcMaterCreateKeyPair(void *sessionContext, WsecUint32 *curveType, WsecBuff *outPubkey, uint64_t *object);

KmcTaResult KmcAgentCreateShareKey(WsecBuffConst *rk, WsecUint32 *alg, WsecUint32 *type, WsecBuff *inPub,
                                   WsecBuff *outPub, WsecBuff *outSkEncByRk);

KmcTaResult KmcMasterCreateShareKey(void *sessionContext, WsecBuffConst *rk, WsecUint32 *alg, WsecUint32 *curveType,
                                    uint64_t *ctx, WsecBuff *inPub, WsecBuff *outSkEncByRk);

#endif // KMC_SRC_TEE_KMCTEE_SYNC_H
