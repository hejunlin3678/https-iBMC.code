/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: header file of asym sdp external interfaces
 * Author: zhanghao
 * Create: 2022-9-20
 * History: new create
 */
#ifndef KMC_SRC_SDP_ASYM_ITF_H
#define KMC_SRC_SDP_ASYM_ITF_H

#include "wsecv2_config.h"
#if WSEC_COMPILE_ENABLE_ASYM

#include "wsecv2_type.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define SDP_MAX_PLAIN_LEN   32768 // 32K
#define SDP_MAX_SIGN_LEN    1088   // 512 + sizeof(KmcAsymHeader) + KMC_TLV_EXT_LEN_MAX

#pragma pack(1)
typedef struct KmcCryptoParamTag {
    WsecUint32      domainId;
    WsecUint32      algId; /* value in WsecAlgId */
    WsecUint32      extLen;
    unsigned char   ext[];
} KmcCryptoParam;
#pragma pack(0)

#pragma pack(1)
typedef struct KmcAsymSignParamTag {
    WsecUint16      keySpec; /* value in WsecKeySpec */
    WsecUint32      extLen;
    unsigned char   ext[];
} KmcAsymSignParam;
#pragma pack(0)

#pragma pack(1)
typedef struct KmcAsymVerifyParamTag {
    WsecUint32      domainId;
    unsigned char   resv[12];
} KmcAsymVerifyParam;
#pragma pack(0)

#pragma pack(1)
typedef struct KmcVerifyWithKeyParamTag {
    WsecUint32      keyLen;
    unsigned char   *pubKey;
    WsecUint32      extLen;
    unsigned char   ext[];
} KmcVerifyWithKeyParam;
#pragma pack(0)

#pragma pack(1)
typedef struct KmcAsymHeaderTag {
    WsecUint32      version;
    WsecUint32      domainId;
    WsecUint32      keyId;
    WsecUint32      algId;
    WsecUint16      keySpec;
    unsigned char   priIndex[8];
    unsigned char   pubIndex[8];
    WsecUint32      cryptoLen;
    unsigned char   resv[16];
    WsecUint32      extLen;
    unsigned char   ext[];
} KmcAsymHeader;
#pragma pack(0)

#pragma pack(1)
typedef struct KmcEncWithKeyParamTag {
    WsecUint32      algId;
    WsecUint32      keyLen;
    unsigned char   *pubkey;
    WsecUint32      extLen;
    unsigned char   ext[];
} KmcEncWithKeyParam;
#pragma pack(0)

#pragma pack(1)
typedef struct KmcAsymEncParamTag {
    WsecUint16      keySpec; /* value in WsecKeySpec */
    WsecUint32      plainLen;
    WsecUint32      extLen;
    unsigned char   ext[];
} KmcAsymEncParam;
#pragma pack(0)

/**
 * @brief      get signature len by key spec (must be a value in WsecKeySpec)
 * @param[in]  signParam configuration of keySpec, need allocate memory by caller.
 * @param[out] signatureLen return signature len when success.
 * @return     WSEC_SUCCESS on success, other error code on failure.
 * @note
 *  - Memory operation: Allocate and release memory by itself, doesn't need invoker release.
 *  - Thread safe:      Thread-safe function.
 *  - OS difference:    No.
 *  - Time consuming:   No.
 */
unsigned long SdpGetSignatureLen(const KmcAsymSignParam *signParam, WsecUint32 *signatureLen);

/**
 * @brief      Find the activate key of domainId and uses the specified algorithm for signature.
 * @param[in]    signParam configuration the domain and signature algorithm(must be a value in WsecAlgId and use Sign).
 * @param[in]    plainText Value to be signed, need allocate memory by caller.
 * @param[in]    textLen Length of the value to be signed.
 * @param[out]   signature Signature result, need allocate memory by caller.
 * @param[outin] signLen The input parameter is the length of the signature value buff. The output parameter is
    the actual length of the signature value.
 * @return     WSEC_SUCCESS on success, other error code on failure.
 * @note
 *  - Memory operation: Allocate and release memory by itself, doesn't need invoker release.
 *  - Thread safe:      Thread-safe function.
 *  - OS difference:    No.
 *  - Time consuming:   Yes, related to thread, cryptographic computation operation and TEE operations (only TEE mode).
 */
unsigned long SdpAsymSign(const KmcCryptoParam *signParam, const unsigned char *plainText, WsecUint32 textLen,
    unsigned char *signature, WsecUint32 *signLen);

/**
 * @brief      Obtain the raw signature value from the signature value of the ciphertext header.
 * @param[in]    signature the signature value of the ciphertext header.
 * @param[in]    signLen Length of the signature value.
 * @param[out]   rawSign Raw signature value result, need allocate memory by caller.
 * @param[outin] rawSignLen The input parameter is the length of the raw signature value buff. The output parameter is
    the actual length of the raw signature value.
 * @return     WSEC_SUCCESS on success, other error code on failure.
 * @note
 *  - Memory operation: Allocate and release memory by itself, doesn't need invoker release.
 *  - Thread safe:      Thread-safe function.
 *  - OS difference:    No.
 *  - Time consuming:   No.
 */
unsigned long SdpGetRawSignature(const unsigned char *signature, WsecUint32 signLen,
    unsigned char *rawSign, WsecUint32 *rawSignLen);

/**
 * @brief      Verify whether the signature result matches the plaintext.
 * @param[in]  plainText Original plaintext.
 * @param[in]  plainLen Length of the original plaintext.
 * @param[in]  signature Signature Value.
 * @param[in]  signLen Length of the signature value.
 * @return     WSEC_SUCCESS on success, other error code on failure.
 * @note
 *  - Memory operation: Allocate and release memory by itself, doesn't need invoker release.
 *  - Thread safe:      Thread-safe function.
 *  - OS difference:    No.
 *  - Time consuming:   Yes, related to thread and cryptographic computation operation.
 */
unsigned long SdpAsymVerify(const unsigned char *plainText, WsecUint32 plainLen,
    const unsigned char *signature, WsecUint32 signLen);

/**
 * @brief      Verify that the plaintext and signature values pass the specified public key.
 * @param[in]  verifyParam The specified public key.
 * @param[in]  plainText Original plaintext.
 * @param[in]  plainLen Length of the original plaintext.
 * @param[in]  signature Signature Value.
 * @param[in]  signLen Length of the signature value.
 * @return     WSEC_SUCCESS on success, other error code on failure.
 * @note
 *  - Memory operation: Allocate and release memory by itself, doesn't need invoker release.
 *  - Thread safe:      Thread-safe function.
 *  - OS difference:    No.
 *  - Time consuming:   Yes, related to cryptographic computation operation.
 */
unsigned long SdpAsymVerifyWithKey(const KmcVerifyWithKeyParam *verifyParam, const unsigned char *plainText,
    WsecUint32 plainLen, const unsigned char *signature, WsecUint32 signLen);

/**
 * @brief      get cipher len by key spec (must be a value in WsecKeySpec)
 * @param[in]  signParam configuration of keySpec, need allocate memory by caller.
 * @param[out] cipherLen return cipher len when success.
 * @return     WSEC_SUCCESS on success, other error code on failure.
 * @note
 *  - Memory operation: Allocate and release memory by itself, doesn't need invoker release.
 *  - Thread safe:      Thread-safe function.
 *  - OS difference:    No.
 *  - Time consuming:   No.
 */
unsigned long SdpGetAsymCipherLen(const KmcAsymEncParam *encParam, WsecUint32 *cipherLen);

/**
 * @brief      Find the activate key of domainId and uses the specified algorithm for encrypt.
 * @param[in]    cryptoParam configuration the domain and enc algorithm(must be a value in WsecAlgId and use enc).
 * @param[in]    plainText Value to be encrypted, need allocate memory by caller.
 * @param[in]    textLen Length of the value to be encrypted.
 * @param[out]   cipherData encrypt result, need allocate memory by caller.
 * @param[outin] cipherLen The input parameter is the length of the cipher value buff. The output parameter is
    the actual length of the cipher value.
 * @return     WSEC_SUCCESS on success, other error code on failure.
 * @note
 *  - Memory operation: Allocate and release memory by itself, doesn't need invoker release.
 *  - Thread safe:      Thread-safe function.
 *  - OS difference:    No.
 *  - Time consuming:   Yes, related to thread and cryptographic computation operation.
 */
unsigned long SdpAsymEncrypt(const KmcCryptoParam *cryptoParam, const unsigned char *plainText, WsecUint32 textLen,
    unsigned char *cipherData, WsecUint32 *cipherLen);

/**
 * @brief      Decrypt the plaintext based on the ciphertext.
 * @param[in]    cipherText Ciphertext value to be decrypted.
 * @param[in]    cipherLen The length of ciphertext value to be decrypted.
 * @param[in]    plainText Decrypted plaintext result, need allocate memory by caller.
 * @param[outin] plainLen The input parameter is the length of the plainText value buff. The output parameter is
    the actual length of the plainText value.
 * @return     WSEC_SUCCESS on success, other error code on failure.
 * @note
 *  - Memory operation: Allocate and release memory by itself, doesn't need invoker release.
 *  - Thread safe:      Thread-safe function.
 *  - OS difference:    No.
 *  - Time consuming:   Yes, related to thread, cryptographic computation operation and TEE operations (only TEE mode).
 */
unsigned long SdpAsymDecrypt(const unsigned char *cipherText, WsecUint32 cipherLen, unsigned char *plainText,
    WsecUint32 *plainLen);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // WSEC_COMPILE_ENABLE_ASYM

#endif // KMC_SRC_SDP_ASYM_ITF_H