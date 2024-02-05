/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: KMC extKsf interface
 * Author: zhanghao
 * Create: 2022-8-19
 * Notes: New Create
 */

#ifndef KMC_SRC_COMMON_WSECV2_EXT_TYPE_H
#define KMC_SRC_COMMON_WSECV2_EXT_TYPE_H

#include "wsecv2_type.h"
#include "wsecv2_ctx.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define KMC_MAX_EXT_KSF_NUM     10
#define KMC_MAX_EXT_SUB_NUM     64
#define KMC_MAX_VALUE_LEN       512
#define KMC_HAS_EXT             0x01 // 是否在在某一扩展结构
#define WSEC_MAX_DER_PUBKEY_LEN 1024

#define WSEC_NOVA_KEY_PLAIN_LEN_MAX 512
#define WSEC_NOVA_PROTECT_KEY 32
#define WSEC_NOVA_KEY_PRI_LEN_MAX 1048 // 512 * 2 + len * 2 + BLOCK_SIZE

#define KMC_NOVA_KSF_TAG_EXT_KSF_HEADER             0x01 // 标识ext扩展结构
#define KMC_NOVA_KSF_TAG_NOVA_KSF_HEADER_EXT        0x02 // 标识novaKsf头扩展结构
#define KMC_NOVA_KSF_TAG_NOVA_KSF_FOOTER_EXT        0x03 // 标识novaKsf尾扩展结构
#define KMC_NOVA_KSF_TAG_ASYM_HEADER                0x04
#define KMC_NOVA_KSF_TAG_NOVA_KEY_HEADER_EXT        0x05 // 标识novaKey头扩展结构
#define KMC_NOVA_KSF_TAG_KSF_HEADER_EXT             0x06 // 标识对称部分头扩展结构
#define KMC_NOVA_KSF_TAG_KSF_FOOTER_EXT             0x07 // 标识对称部分尾扩展结构

/* sub tag:
 * 0-127(0x00 - 0x7F)   : plaintext
 * 128-191(0x80 - 0xBF) : Root key encryption
 * 192-255(0xC0 - 0xFF) : Protect key encryption
 */
#define KMC_PLAIN_SUB_TAG_START 0x00

#define KMC_RK_ENC_SUB_TAG_START 0x80

#define KMC_PK_ENC_SUB_TAG_START 0xC0

#pragma pack(1)
typedef struct TagKmcExtKsfHeader {
    unsigned char   extKsfTag;
    WsecUint32      ksfNum;
    unsigned char   reserve[40];
} KmcExtKsfHeader;
#pragma pack()

#pragma pack(1)
typedef struct TagKmcSubTlv {
    unsigned char   subTag;
    WsecUint32      cipherAlgId;
    WsecUint32      ivLen;
    unsigned char   *iv;
    WsecUint32      tagLen;
    unsigned char   *tag;
    WsecUint32      valueLen;
    unsigned char   *subValue;
} KmcSubTlv;
#pragma pack()

#pragma pack(1)
typedef struct TagKmcNovaKsfHeader {
    unsigned char   novaKsfTag;
    WsecUint32      keyNum;
    unsigned char   protectKeyTag;
    WsecUint32      protectKeyLen;
    unsigned char   *protectKey;
    WsecUint32      cipherAlgId;
    WsecUint32      ivLen;
    unsigned char   *iv;
    WsecUint32      tagLen;
    unsigned char   *tag;
    unsigned char   headerExt;
    unsigned char   footerExt;
    unsigned char   reserve[18];
} KmcNovaKsfHeader;
#pragma pack()

#pragma pack(1)
typedef struct TagKmcNovaKeyHeader {
    unsigned char   keyTag;
    KmcNovaKeyInfo  keyInfo;        /* Basic key information */
    WsecUint32      cipherAlgId;    /* Encryption Method */
    unsigned char   headerExt;
    unsigned char   reserve[19];
    WsecUint32      ivLen;
    unsigned char   *iv;
    WsecUint32      tagLen;
    unsigned char   *tag;
} KmcNovaKeyHeader;
#pragma pack()

#pragma pack(1)
typedef struct TagKmcNovaKey {
    unsigned char   keyHash[WSEC_MK_HASH_REC_LEN];
    WsecUint32      keyLen;
    unsigned char   *key;
} KmcNovaKey;
#pragma pack()

#pragma pack(1)
typedef struct TagKmcNovaKeyPair {
    KmcNovaKey      pubKey;
    KmcNovaKey      priKey;
} KmcNovaKeyPair;
#pragma pack()

#pragma pack(1)
typedef struct TagKmcNovaKsfMem {
    unsigned char   novaKsfTag;
    unsigned char   protectKeyTag;
    WsecUint32      protectKeyLen;
    unsigned char   *protectKey;
    WsecArray       keyArray;
    WsecArray       headerSubs;
    WsecArray       footerSubs;
} KmcNovaKsfMem;
#pragma pack()

#pragma pack(1)
typedef struct TagKmcMemNovaKey {
    KmcNovaKeyInfo  keyInfo;     /* Basic key information */
    WsecArray       headerSubs;
    KmcNovaKeyPair  keyPair;
} KmcMemNovaKey;
#pragma pack()

#pragma pack(1)
typedef struct TagKmcKsfNovaKey {
    KmcNovaKeyHeader keyHeader;
    KmcNovaKeyPair   keyPair;
} KmcKsfNovaKey;
#pragma pack()


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // KMC_SRC_COMMON_WSECV2_EXT_TYPE_H
