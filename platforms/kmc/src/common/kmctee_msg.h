/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: kmc ca ta common struct
 * Author: gaoyu
 * Create: 2022-08-31
 */
#ifndef KMC_SRC_KMC_ENCLAVE_KMCTEE_MSG_H
#define KMC_SRC_KMC_ENCLAVE_KMCTEE_MSG_H

#include "wsecv2_config.h"
#include "wsecv2_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define KMC_TEE_HASH_DATA_LEN 32
#define DEFAULT_IV_LEN 16

typedef enum TagTeeHmacUseKey {
    HMAC_TEE_ROOT_KEY,
    HMAC_TEE_WORK_KEY
} TeeHmacUseKey;

typedef enum TagImpExpMkType {
    USAGE_PLAIN_MK_IMPORT,
    USAGE_SHARE_MK_IMPORT,
    USAGE_SHARE_MK_EXPORT,
    USAGE_ROOT_KEY_UPDATE,
    USAGE_PLAIN_MK_EXPORT,
    USAGE_CIPHER_MK_IMPORT,
    USAGE_ALG_CFG_UPDATE,
    USAGE_PLAIN_AYSM_IMPORT,
    USAGE_PROTECT_KEY_UPDATE,
    USAGE_RK_PROTECT_KEY_UPDATE
} ImpExpMkType;

#define KMC_ECC_256_KEY_LEN 32
#define KMC_ECC_384_KEY_LEN 48
#define KMC_ECC_521_KEY_LEN 66
#define KMC_SM2_KEY_LEN 32

typedef enum {
    KMC_KEY_TYPE_SYM = 1,
    KMC_KEY_TYPE_ASYM = 2
} KmcSyncKeyType;

#pragma pack(1)
typedef struct TagKmcSyncEncKey {
    unsigned char iv[32]; // 32 bytes extensibility
    WsecUint32 ivLen;
    KmcNovaKeyInfo keyInfo;
    unsigned char keyType; // sym and asym KmcSyncKeyType
    WsecUint32 priKeyLen; // sym mk len, asym private key len
    WsecUint32 pubKeyLen; // asym public key len
    unsigned char key[]; // sym mk / asym private key + asym public key
} KmcSyncEncKey;

typedef struct TagKmcTeeAsymKeyInfo {
    KmcNovaKeyInfo keyInfo;
    unsigned char keyType; // sym and asym KmcSyncKeyType
    unsigned char priHash[KMC_TEE_HASH_DATA_LEN];
    unsigned char pubHash[KMC_TEE_HASH_DATA_LEN];
    WsecUint32 priKeyLen;
    WsecUint32 pubKeyLen;
    unsigned char key[];
} KmcTeeAsymKeyInfo;

typedef struct TagKmcBatchElement {
    KmcNovaKeyInfo keyInfo;
    unsigned char keyType; // sym and asym KmcSyncKeyType
    WsecUint32 priKeyLen;
    WsecUint32 pubKeyLen;
    WsecUint8 key[];
} KmcBatchElement;
#pragma pack()

#define WSEC_SYNC_NOVA_PUBLIC_KEY_PLAIN_LEN_MAX (512)
#define WSEC_SYNC_NOVA_PRIVATE_KEY_PLAIN_LEN_MAX (1032)
#define WSEC_SYNC_NOVA_PRIVATE_KEY_REE_CIPHER_LEN_MAX (WSEC_SYNC_NOVA_PRIVATE_KEY_PLAIN_LEN_MAX + 16)
#define WSEC_SYNC_NOVA_PRIVATE_KEY_TEE_CIPHER_LEN_MAX (WSEC_SYNC_NOVA_PRIVATE_KEY_PLAIN_LEN_MAX + sizeof(KmcTeeMk) + 16)
#define WSEC_SYNC_NOVA_KEY_CIPHER_LEN_MAX (WSEC_SYNC_NOVA_PUBLIC_KEY_PLAIN_LEN_MAX + WSEC_SYNC_NOVA_PRIVATE_KEY_TEE_CIPHER_LEN_MAX)

#define KMC_DH_TMP_KEY_LEN 164

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif // KMC_SRC_KMC_ENCLAVE_KMCTEE_MSG_H
