/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: Implementation of utils of SDP
 * Author: y00440103
 * Create: 2020-05-27
 */

#ifndef KMC_SRC_SDP_SDP_UTILS_H
#define KMC_SRC_SDP_SDP_UTILS_H

#include "wsecv2_type.h"
#include "kmcv2_pri.h"
#include "kmcv2_cfg.h"
#include "sdpv3_type.h"
#include "sdpv1_itf.h"
#include "sdpv2_itf.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* Indicates the first two members of KmcKeyType: KMC_KEY_TYPE_ENCRPT KMC_KEY_TYPE_INTEGRITY. */
#define SDP_V1_SUPPORT_KEY_TYPE_COUNT 2
#define SDP_MAX_SALT_LENGTH KMC_SALTLEN_MAX

#define   SDP_SYMM_SALT_TAG 1 /* Cipher Extended salt tag */
#define   SDP_HMAC_SALT_TAG 2 /* hmac Extended salt tag */
#define   SDP_PWD_SALT_TAG 4 /* pwd Extended salt tag */

typedef struct TagSdpHmacCtxParam {
    WsecUint32    version;
    WsecHandle    cacCtx;     /* Context of the CAC adaptation layer */
} SdpHmacCtxParam;

/* HMAC context */
typedef struct TagSdpHmacCtx {
    SdpHmacCtxParam hmacCtxParam;
    SdpHmacHeader hmacHeader; /* Recorded HMAC header of the HMAC calculated by the big data platform. */
} SdpHmacCtx;

/* HMAC context */
#pragma pack(1)
typedef struct TagSdpHmacCtxEx {
    SdpHmacCtxParam hmacCtxParam;
    SdpHmacExHeaderEx hmacHeader; /* Recorded HMAC header of the HMAC calculated by the big data platform. */
} SdpHmacCtxEx;
#pragma pack()

typedef struct TagSdpHeaderId {
    WsecUint32 domainId;
    WsecUint32 algId;
    WsecBool hasHmac;
    WsecUint32 hmacAlgId;
} SdpHeaderId;

/* The type of header that currently needs to be extended by the sdp */
typedef enum {
    SDP_HEADER_TYPE_UNKNOWN, /* unknown header type */
    SDP_CIPHER_SYMM_HEADER, /* SdpCipherHeaderEx header type */
    SDP_CIPHER_HMAC_HEADER, /* SdpHmacHeaderEx header type */
    SDP_BOD_SYMM_HEADER, /* SdpBodCipherHeaderEx header type, using operation ciphertext header stage */
    SDP_BOD_HMAC_HEADER, /* SdpBodCipherHeaderEx header type, using operation hmac header stage */
    SDP_HMACEX_HEADER, /* SdpHmacExHeaderEx header type */
    SDP_PWD_HEADER /* SdpPwdHeaderEx header type */
} CfgSdpHeaderType;

typedef struct TagSdpCommonHeader {
    void *header; /* beginning address of header, including tlv */
    WsecUint32 headerLen; /* sizeof(header) + tlv */
    WsecUint32 type; /* from CfgSdpHeaderType */
} SdpCommonHeader;

/* same as SdpCommonHeader, except that the header track is const */
typedef struct TagSdpCommonHeaderConst {
    const void *header;
    WsecUint32 headerLen;
    WsecUint32 type;
} SdpCommonHeaderConst;

typedef struct TagSdpTlvBaseAddr {
    unsigned char tag; /* tlv tag */
    void *baseAddr; /* beginning address of tlv */
    WsecUint32 len; /* tlv len */
} SdpTlvBaseAddr;

typedef struct TagParseTlvArray {
    unsigned char *tags; /* parse tag array */
    WsecBuffExt *buffs; /* parse buff array */
    unsigned char *founds; /* parse founds array */
    WsecUint32 num; /* need to parse the number */
} ParseTlvArray;

typedef struct TagConcatHeadInfo {
    void *header; /* header addr */
    WsecUint32 oriHeaderLen; /* just header len, not include tlv */
    WsecUint32 restExtLen; /* rest tlv len */
    WsecUint32 filledLen; /* already filled in header */
} ConcatHeadInfo;

typedef struct TagConcatHeadInfoConst {
    const void *header; /* header addr */
    WsecUint32 oriHeaderLen; /* just header len, not include tlv */
    WsecUint32 restExtLen; /* rest tlv len */
    WsecUint32 filledLen; /* already filled in header */
} ConcatHeadInfoConst;

/* Check whether alg id is symmetric encryption alg */
WsecBool CheckIsSymAlg(KmcCbbCtx *kmcCtx, WsecUint32 algId);

/* Check whether alg id is hamc alg */
WsecBool CheckIsHmcAlg(KmcCbbCtx *kmcCtx, WsecUint32 algId);

/* Check whether alg id is valid */
WsecBool CheckIsAlgValid(KmcCbbCtx *kmcCtx, WsecUint32 cipherAlgId, WsecUint32 hmacAlgId);

/* Check whether is GCM alg id */
WsecBool CheckIsGcmAlgId(WsecUint32 algId);

/* Check whether is CBC alg id */
WsecBool CheckIsCbcAlgId(WsecUint32 algId);

/* Check the result of the previous step and write data. */
unsigned long CheckResultAndWriteIfOk(KmcCbbCtx *kmcCtx, unsigned long ret, WsecHandle writeStream,
    const WsecBuff *plainBuff);

/* Safety minus two integers */
unsigned long SdpSafeSubTwo(KmcCbbCtx *kmcCtx, WsecUint32 first, WsecUint32 second, long *remain);

/* Searches for the currently effective MK in a specified domain and derives the working key based on the MK. */
unsigned long GetWorkKey(KmcCbbCtx *kmcCtx, WsecUint32 algId, const KmcActiveKeyParam *keyParam, WsecUint32 *keyId,
    Pbkdf2Param *pbkdf2Param, unsigned char *iv, WsecUint32 ivLen, unsigned char *key, WsecUint32 *keyLen,
    KmcCnfId *ids, WsecUint32 version);

/* Ciphertext Header Byte Sequence Conversion */
void SdpCvtSdpCipherHeaderByteOrder(SdpCipherHeader *header, WsecUint32 direction);

/* Byte order conversion: Convert the HMAC header byte order. */
WsecVoid SdpCvtHmacTextHeaderByteOrder(SdpHmacHeader *header, WsecUint32 direction);

WsecVoid SdpCvtHmacTextHeaderByteOrderEx(SdpHmacExHeaderEx *header, WsecUint32 direction);
unsigned long GetWorkKeyByIDHash(KmcCbbCtx *kmcCtx, WsecUint32 algId, WsecUint32 domain, WsecUint32 keyId,
    unsigned char kdfAlgId, const unsigned char *hashData, WsecUint32 hashLen, const Pbkdf2Param *pbkdf2Param,
    unsigned char *key, WsecUint32 *keyLen);

unsigned long GetWorkKeyByIDEx(KmcCbbCtx *kmcCtx, WsecUint32 domain, WsecUint32 keyId, WsecUint32 algId,
    const unsigned char *hashData, WsecUint32 hashLen, unsigned char *salt, WsecUint32 saltLen, unsigned char *key,
    WsecUint32 *keyLen, WsecUint32 iter, unsigned char kdfAlgId);

WsecVoid SdpCvtHmacTextHeaderByteOrderAll(void *data, WsecUint32 version, WsecUint32 direction);

/* Releasing the HMAC context */
void SdpFreeHmacCtx(KmcCbbCtx *kmcCtx, WsecHandle * const ctx);

void FillSdpHeaderId(SdpHeaderId *ids, WsecUint32 domain, WsecUint32 cipherAlgId, WsecUint32 hmacAlgId);

WsecVoid SdpCvtSdpCipherHeaderByteOrderEx(SdpCipherHeaderEx *header, WsecUint32 direction);

WsecVoid SdpCvtBodCipherHeaderExtLenByteOrderEx(SdpBodCipherHeaderEx *header, WsecUint32 direction);

unsigned long SdpGetEncBodCipherHeaderLenWithExtlen(KmcCbbCtx *kmcCtx, WsecUint32 cipherAlgId, WsecUint32 hmacAlgId,
    WsecUint32 *headerLen);

WsecUint32 GetHmacHeaderLen(KmcCbbCtx *kmcCtx, WsecUint32 version);
WsecUint32 GetVerifyHmacHeaderLen(WsecUint32 version);
WsecUint32 SdpGetRestSaltLen(KmcCbbCtx *kmcCtx, WsecUint32 origLen);

/*
 * According to the header type, calculate the length of the entire header, including tlv, used in the decryption stage
 * The user's incoming header does not need to be converted to endianness
 * */
unsigned long SdpGetTotalLenFromHeader(KmcCbbCtx *kmcCtx, const void *header, WsecUint32 headerLen, WsecUint32 type,
    WsecUint32 *outLen);
/*
 * According to the header type, calculate the length of the tlv
 * */
unsigned long SdpCfgGetTlvLen(KmcCbbCtx *kmcCtx, WsecUint32 type, WsecUint32 *outLen);

/*
 * Fill the salt value into the corresponding header. If the filling length exceeds the length supported by
 * the corresponding header, it will be filled with the tlv extended form.
 * */
unsigned long FillSaltBuffer(KmcCbbCtx *kmcCtx, SdpCommonHeader *header, WsecBuffConst *saltData);

/*
 * Parse the salt value in the header. If there is also a salt value to be parsed in the tlv, it will be parsed
 * and spliced, kmcCtx check depends on the upper layer call
 * */
unsigned long ParseSaltHeader(KmcCbbCtx *kmcCtx, const SdpCommonHeaderConst *header, WsecBuffExt *saltData);

unsigned long FillSdpTlv(KmcCbbCtx *kmcCtx, SdpTlvBaseAddr *baseLine, WsecBuffConst *dataBuff, WsecUint32 *fillLen);

unsigned long ParseSdpTlv(KmcCbbCtx *kmcCtx, const unsigned char *tlvBase, WsecUint32 tlvLen,
    ParseTlvArray *tlvArray);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* KMC_SRC_SDP_SDP_UTILS_H */
