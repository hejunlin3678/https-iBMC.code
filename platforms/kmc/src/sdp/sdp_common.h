/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: This header file is public functions of sdpv1 and sdpv2.
 * Author: yangchen
 * Create: 2021-04-16
 * History: None
 */

#ifndef KMC_SRC_SDP_SDP_COMMON_H
#define KMC_SRC_SDP_SDP_COMMON_H

#include "wsecv2_type.h"
#include "wsecv2_ctx.h"
#include "sdpv3_type.h"
#include "sdp_utils.h"
#include "cacv2_pri.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define KMC_PLAIN_BLOCK_MAX_LEN 4096
#define SDP_CIPHER_FILE_VER 1
#define SDP_CIPHER_FILE_VER2 2

typedef struct TagCipherParam {
    unsigned char *key;
    WsecUint32     keyLen;
    WsecUint32     ivLen;
    WsecBuff       salt;
} CipherParam;

typedef struct TagHmacParam {
    unsigned char *hmacKey;
    WsecUint32     hmacKeyLen;
    WsecBuff       salt;
} HmacParam;

typedef struct TagSdpMkInfo {
    unsigned char *key;
    WsecUint32     keyLen;
    WsecUint32     keyId;
    unsigned char  hash[WSEC_MK_HASH_REC_LEN]; // key hash len is 8 byte
} SdpMkInfo;

#pragma pack(1)
typedef struct TagSdpCipherAndHmacHeaders {
    SdpCipherHeaderEx cipherHeader;
    SdpHmacHeaderEx   hmacHeader;
} SdpCipherAndHmacHeaders;
#pragma pack()

typedef struct TagSdpCipherCtxParams {
    WsecUint32      version;      /* Version of CipherCtx */
    WsecHandle      cacCtx;       /* Context of the CAC adaptation layer */
    WsecHandle      hmacCtx;      /* integrity context */
} SdpCipherCtxParams;

/* Context definition for big data encryption and decryption */
typedef struct TagSdpCipherCtxEx {
    SdpCipherCtxParams     sdpCtxParam;
    SdpCipherHeaderEx      cipherHeader; /* Encryption and decryption context */
} SdpCipherCtxEx;

/* Context definition for big data encryption and decryption */
typedef struct TagSdpCipherCtx {
    SdpCipherCtxParams sdpCtxParam;
    SdpCipherHeader cipherHeader; /* Encryption and decryption context */
} SdpCipherCtx;

/* Free sdpCtx by version */
WsecVoid SdpFreeCipherCtx(KmcCbbCtx *kmcCtx, WsecHandle *sdpCtx);

/* Get and check version from bodHeader */
unsigned long DoCheckSdpVersion(KmcCbbCtx *kmcCtx, const WsecVoid *header, WsecUint32 headerLen, WsecUint32 *version);

/* Get cipherLen by plainTextLen */
unsigned long GetCipherDataLen(KmcCbbCtx *kmcCtx, WsecUint32 version, WsecUint32 plaintextLen,
    WsecUint32 *ciphertextLenOut);

/* Check input Params before SdpEncryptUpdate(Ex) */
unsigned long EncUpdateCheckInputParams(const WsecHandle *ctx, const unsigned char *plainText, WsecUint32 plainLen,
    const unsigned char *cipherText, const WsecUint32 *cipherLen);

/* Check input Params before SdpDecryptUpdate(Ex) */
unsigned long DecUpdateCheckInputParams(const unsigned char *cipherText, WsecUint32 cipherLen,
    const unsigned char *plainText, const WsecUint32 *plainLen);

/* Check CipherHeader params */
unsigned long CheckSdpCipherHeader(KmcCbbCtx *kmcCtx, WsecUint32 version, WsecUint32 headerVersion, WsecUint32 hmacFlag,
    WsecUint32 cipherAlgId, WsecUint32 iter);

/* File Header Structure Byte Order */
void SdpCvtByteOrder4CipherFileHdr(SdpCipherFileHeader *fileHeader, WsecUint32 direction);

unsigned long DoEncryptAesFinal(KmcCbbCtx *kmcCtx, WsecHandle *ctx, WsecUint32 algId, unsigned char *cipherText,
    WsecUint32 *cipherLen, unsigned char *hmacText, WsecUint32 *hmacLen);

unsigned long DoDecryptAesFinal(KmcCbbCtx *kmcCtx, const WsecHandle *ctx, WsecUint32 algId,
    const unsigned char *hmacText, WsecUint32 hmacLen, unsigned char *plainText, WsecUint32 *plainLen);

WsecBool CheckInputParamFinal(KmcCbbCtx *kmcCtx, WsecUint32 algId, WsecUint32 hmacFlag,
    const unsigned char *hmacText, WsecBool hmacLenValid, const unsigned char *inBuff, const WsecUint32 *inLen);

unsigned long MultiGetMacLenForEncrypt(KmcCbbCtx *kmcCtx, WsecUint32 cipherAlgId, WsecUint32 hmacAlgId,
    WsecUint32 *macLen);

unsigned long FillCipherTextHeaderEx(KmcCbbCtx *kmcCtx, SdpHeaderId *cipherIds, WsecVoid *header, WsecUint32 headerLen,
    WsecUint32 type, CipherParam *cipherParam);

unsigned long DoGetCipherInfoFromBodHeader(KmcCbbCtx *ctx, const unsigned char *bodCipherHeader,
    WsecUint32 bodCipherHeaderLen, WsecUint32 version, SdpBodCipherInfo* cipherInfo);

unsigned long MultiSdpGetMkDetailByBodCipherHeader(KmcCbbCtx *kmcCtx, WsecUint32 version,
    WsecVoid *bodCipherHeader, WsecUint32 bodCipherLen, KmcMkInfo *mkInfo);

unsigned long SdpGetWorkLenWithAlgType(KmcCbbCtx *kmcCtx, WsecUint32 algId, WsecUint32 *workLen);

unsigned long SdpFillEncDecParamV1(KmcCbbCtx *kmcCtx, CacCryptoParam *param, SdpCipherHeader *header, WsecUint32 ivLen,
    unsigned char *key, WsecUint32 keyLen);

unsigned long SdpFillEncParamV2(KmcCbbCtx *kmcCtx, CacCryptoParam *param, SdpCipherHeaderEx *headerEx,
    CipherParam *cipherParam);

unsigned long SdpFillDecParamV2(KmcCbbCtx *kmcCtx, CacCryptoParam *param, SdpCipherHeaderEx *headerEx,
    Pbkdf2Param *pbkdf2Param, WsecUint32 ivLen, unsigned char *key, WsecUint32 keyLen);


/* V1 */
unsigned long MultiGetCipherHeaderV1(KmcCbbCtx *kmcCtx, const unsigned char *ciphertext, WsecUint32 ciphertextLen,
    SdpCipherHeader *cipherHeader);

unsigned long MultiSdpDecryptV1(KmcCbbCtx *kmcCtx, WsecUint32 domain, const unsigned char *cipherText,
    WsecUint32 cipherLen, unsigned char *plainText, WsecUint32 *plainLen);

unsigned long CheckSdpCipherHeaderForDec(KmcCbbCtx *kmcCtx, WsecUint32 domain, const SdpCipherHeader *header);

unsigned long SdpFillCipherTextHeader(KmcCbbCtx *kmcCtx, SdpHeaderId ids, SdpCipherHeader *cipherHeader,
    unsigned char *key, WsecUint32 *keyLength, WsecUint32 *ivLength);

unsigned long SdpEncGetWk(KmcCbbCtx *kmcCtx, WsecUint32 *keyId, SdpHeaderId *cipherIds, CipherParam *cipherParam,
    SdpCipherHeaderEx *cipherHeader, unsigned char *mkHash, WsecUint32 mkHashLen, KmcCnfId *ids);

unsigned long SdpHmacGetWk(KmcCbbCtx *kmcCtx, WsecUint32 *keyId, WsecUint32 version, SdpHeaderId *cipherIds,
    HmacParam *hmacParam, SdpHmacHeaderEx *hmacHeader, unsigned char *mkHash, WsecUint32 mkHashLen, KmcCnfId *ids);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* KMC_SRC_SDP_SDP_COMMON_H */
