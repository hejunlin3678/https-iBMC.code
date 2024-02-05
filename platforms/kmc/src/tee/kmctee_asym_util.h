/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: TEE Asymmetric cryptographic algorithm interface util for KMC
 * Author: liuermeng
 * Create: 2022-09-08
 */

#ifndef KMC_SRC_TEE_KMCTEE_ASYM_UTIL_H
#define KMC_SRC_TEE_KMCTEE_ASYM_UTIL_H

#include "kmctee_adapt.h"
#if WSEC_COMPILE_ENABLE_ASYM

#include "wsecv2_type.h"
#include "kmctee_status.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define KEY_PARTS_MAX_LEN 1024
#define DIGEST_MAX_LEN 64

#define RSA_2048_BITS 2048
#define RSA_3072_BITS 3072
#define RSA_4096_BITS 4096
#define EC_P256_BITS 256
#define EC_P384_BITS 384
#define EC_P521_BITS 521
#define SM2_BITS 256
#define ED25519_KEY_BITS 256

#define KEY_BYTES(l) (((l) + 7) / 8)

#ifdef WSEC_COMPILE_OPTEE
#define TEE_ATTR_RSA_MGF1_HASH      0xF0000830

typedef enum {
    TEE_DH_HASH_SHA1_mode   = 0,
    TEE_DH_HASH_SHA224_mode = 1,
    TEE_DH_HASH_SHA256_mode = 2,
    TEE_DH_HASH_SHA384_mode = 3,
    TEE_DH_HASH_SHA512_mode = 4,
    TEE_DH_HASH_NumOfModes,
} TEE_DH_HASH_Mode;

#endif

KmcTaResult GetSm2KeyTypeBySpec(WsecUint32 keyUsage, WsecUint32 *keyType);

KmcTaResult ExtractBuf(WsecBuff *buf, WsecBuff *buffers[], WsecUint32 num);

KmcTaResult ExtractEccKeyBuf(WsecUint32 curve, WsecBuff *pubKey, WsecBuff *priKey,
    WsecBuff *x, WsecBuff *y, WsecBuff *pri);

WsecUint32 GetKeyBits(WsecUint32 keyType);

KmcTaResult GetTeeSignInfo(WsecUint32 algo, WsecUint32 *hash, WsecUint32 *teeSignAlgo);

KmcTaResult GetSm2KeyTypeByAlgo(WsecUint32 algo, WsecUint32 *keyType);

KmcTaResult GetTeeCryptAlgo(WsecUint32 algo, WsecUint32 *cryptAlgo);

KmcTaResult CheckKeyBuffLen(WsecUint32 keySpec, WsecBuff *pubKey, WsecBuff *priKey);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif // WSEC_COMPILE_ENABLE_ASYM
#endif // KMC_SRC_TEE_KMCTEE_ASYM_UTIL_H