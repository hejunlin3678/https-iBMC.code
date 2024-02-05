/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2014-2020. All rights reserved.
 * Description: Encryption algorithm header file, which is not open to external systems.
 * Author: Luan Shipeng l00171031
 * Create: 2014-10-27
 * History: 2018-10-06 Zhang Jie (employee ID: 00316590) Rectification by UK
 */


#ifndef KMC_SRC_CAC_CACV2_PRI_H
#define KMC_SRC_CAC_CACV2_PRI_H

#include <stdint.h>
#include "wsecv2_type.h"
#include "sdpv3_type.h"
#include "wsecv2_ctx.h"
#include "kmctee_msg.h"
#include "kmc_ext_ksf.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define WSEC_MIN(a, b) ((a) < (b) ? (a) : (b))
/*
 * Padding (CBC) or Tag (GCM) causes the ciphertext to be longer than the plaintext,
 * but Padding does not exceed 32 bytes. In this CBB, the tag does not exceed 32 bytes.
 */
#define CAC_CRYPT_TAG_LEN           16
#define CAC_CRYPT_BLOCK_LEN         128
#define CAC_CRYRT_BUFF_PROTECT_SIZE 32
/* Default data sharding size 30k */
#define CAC_CRYRT_TEE_SHARD_LEN     30720

#define WSEC_IS_ENCRYPT_ALGID(id) (CacAlgIdToType(id) == WSEC_ALGTYPE_SYM)
#define WSEC_IS_HASH_ALGID(id) (CacAlgIdToType(id) == WSEC_ALGTYPE_DIGEST)
#define WSEC_IS_HMAC_ALGID(id) (CacAlgIdToType(id) == WSEC_ALGTYPE_HMAC)
#define WSEC_IS_PBKDF_ALGID(id) (CacAlgIdToType(id) == WSEC_ALGTYPE_PBKDF)

typedef struct TagKmcDHParam {
    WsecUint32 dhCurve;
} KmcDHParam;

#pragma pack(1)
typedef struct TagKmcDhApis {
    void* evp_pkey_ctx_new_id_fun;
    void* evp_pkey_paramgen_init_fun;
    void* evp_pkey_ctx_ctrl_fun;
    void* evp_pkey_keygen_init_fun;
    void* evp_pkey_keygen_fun;
    void* evp_pkey_get_raw_private_key_fun;
    void* evp_pkey_get_raw_public_key_fun;
    void* evp_pkey_get0_ec_key_fun;
    void* ec_key_get0_private_key_fun;
    void* ec_key_get0_public_key_fun;
    void* ec_key_get0_group_fun;
    void* bn_ctx_new_fun;
    void* bn_ctx_get_fun;
    void* ec_point_get_affine_coordinates_fun;
    void* bn_bn2bin_fun;
    void* bn_ctx_free_fun;
    void* bn_bn2mpi_fun;
    void* evp_pkey_new_raw_public_key_fun;
    void* evp_pkey_new_raw_private_key_fun;
    void* ec_key_new_by_curve_name_fun;
    void* bn_mpi2bn_fun;
    void* ec_key_set_private_key_fun;
    void* evp_pkey_new_fun;
    void* evp_pkey_set1_ec_key_fun;
    void* bn_free_fun;
    void* ec_group_new_by_curve_name_fun;
    void* ec_point_new_fun;
    void* bn_bin2bn_fun;
    void* ec_point_set_affine_coordinates_fun;
    void* ec_key_set_public_key_fun;
    void* ec_point_free_fun;
    void* ec_group_free_fun;
    void* ec_key_free_fun;
    void* evp_pkey_ctx_new_fun;
    void* evp_pkey_derive_init_fun;
    void* evp_pkey_derive_set_peer_fun;
    void* evp_pkey_derive_fun;
    void* evp_pkey_ctx_free_fun;
    void* evp_pkey_free_fun;
} KmcDhApis;
#pragma pack()

/* Cryptographic algorithms invoked internally: random number/encryption/decryption/hash/derivation/HMAC */
WsecUint32 CacAlgIdToType(WsecUint32 algId);

/* Initializing and Uninit the RNG */
unsigned long CacInitRng(KmcCbbCtx *kmcCtx);
void CacUnInitRng(KmcCbbCtx *kmcCtx);

/* Generates a random number. The caller must ensure the validity of the input parameter. */
unsigned long CacRandom(KmcCbbCtx *kmcCtx, WsecVoid *buff, WsecUint32 buffLen);

/* Hash Algorithm Implementation */
unsigned long CacDigest(KmcCbbCtx *kmcCtx, WsecUint32 algId, const WsecVoid *data, WsecUint32 dataLen,
    WsecVoid *digestBuff, WsecUint32 *digestLen);

/* Initialize the hash operation, transfer the ID, and obtain the handle. */
unsigned long CacDigestInit(KmcCbbCtx *kmcCtx, WsecHandle *ctx, WsecUint32 algId);

/* The input data is hashed. The data can be input multiple times. */
unsigned long CacDigestUpdate(KmcCbbCtx *kmcCtx, const WsecHandle ctx, const WsecVoid *data, WsecUint32 dataLen);

/* End the hash operation and obtain the hash result. */
unsigned long CacDigestFinal(KmcCbbCtx *kmcCtx, WsecHandle *ctx, WsecVoid *digestBuff, WsecUint32 *buffLen);

/* Releases hash handles. */
WsecVoid CacDigestReleaseCtx(KmcCbbCtx *kmcCtx, WsecHandle *ctx);

/* Calculates the input data based on the specified algorithm, key, and HMAC and returns the calculation result. */
unsigned long CacHmac(KmcCbbCtx *kmcCtx, WsecUint32 algId, const WsecVoid *key, WsecUint32 keyLen,
    const WsecVoid *data, WsecUint32 dataLen, WsecVoid *hmacBuff, WsecUint32 *hmacLen);

/* Initialize the HMAC, transfer the ID and key, and obtain the handle. */
unsigned long CacHmacInit(KmcCbbCtx *kmcCtx, WsecHandle *ctx, WsecUint32 algId, const WsecVoid *key, WsecUint32 keyLen);

/* The input data is HMAC. The data can be input for multiple times. */
unsigned long CacHmacUpdate(KmcCbbCtx *kmcCtx, WsecHandle ctx, const WsecVoid *data, WsecUint32 dataLen);

/* End the HMAC operation and obtain the HAMC result. */
unsigned long CacHmacFinal(KmcCbbCtx *kmcCtx, WsecHandle *ctx, WsecVoid *hmacBuff, WsecUint32 *hmacLen);

/* Releases the HMAC handle. */
WsecVoid CacHmacReleaseCtx(KmcCbbCtx *kmcCtx, WsecHandle *ctx);

/* Obtains the HMAC length based on the HMAC algorithm ID. */
WsecUint32 CacHMACSize(WsecUint32 algId);

/* Specify the algorithm ID, password, salt value, and iteration times to obtain the derived key. */
unsigned long CacPbkdf2(KmcCbbCtx *kmcCtx, WsecUint32 kdfAlg, const WsecVoid *kdfPassword, WsecUint32 passwordLen,
    const Pbkdf2ParamConst *pbkdf2Param, WsecUint32 deriveKeyLen, WsecVoid *derivedKey);

/* Specify the algorithm ID, key, IV, and plain text, and encrypt them to obtain the cipher text. */
unsigned long CacEncrypt(KmcCbbCtx *kmcCtx, WsecUint32 algId, const WsecVoid *key, WsecUint32 keyLen,
    const WsecVoid *iv, WsecUint32 ivLen, const WsecVoid *plaintext, WsecUint32 plaintextLen,
    WsecVoid *ciphertext, WsecUint32 *ciphertextLen);

/* Specify the algorithm ID, key, IV, and ciphertext, and decrypt them to obtain the plaintext. */
unsigned long CacDecrypt(KmcCbbCtx *kmcCtx, WsecUint32 algId, const WsecVoid *key, WsecUint32 keyLen,
    const WsecVoid *iv, WsecUint32 ivLen, const WsecVoid *ciphertext, WsecUint32 ciphertextLen, WsecVoid *plaintext,
    WsecUint32 *plaintextLen);

/* Obtain the block length of the symmetric encryption algorithm based on the algorithm ID. */
WsecUint32 CacSymmBlockSize(WsecUint32 algId);

/* Obtains the IV length of the symmetric encryption algorithm based on the algorithm ID. */
WsecUint32 CacSymmIvLen(WsecUint32 algId);

/* Obtain the key length of the symmetric encryption algorithm based on the algorithm ID. */
WsecUint32 CacSymmKeyLen(WsecUint32 algId);

/* Implementation of the AES_GCM Encryption Algorithm Interface */
unsigned long CacEncryptAesGcm(KmcCbbCtx *kmcCtx, WsecUint32 algId, const unsigned char *key, WsecUint32 keyLen,
    const unsigned char *iv, WsecUint32 ivLen, const unsigned char *plaintext, WsecUint32 plaintextLen,
    unsigned char *ciphertext, WsecUint32 *ciphertextLen, unsigned char *tag, WsecUint32 tagLen);

/* AES_GCM decryption algorithm interface implementation */
unsigned long CacDecryptAesGcm(KmcCbbCtx *kmcCtx, WsecUint32 algId, const unsigned char *key, WsecUint32 keyLen,
    const unsigned char *iv, WsecUint32 ivLen, const unsigned char *ciphertext, WsecUint32 ciphertextLen,
    unsigned char *tag, WsecUint32 tagLen, unsigned char *plaintext, WsecUint32 *plaintextLen);

/* Symmetric stream encryption (start) */
unsigned long CacEncryptInit(KmcCbbCtx *kmcCtx, WsecHandle *ctx, WsecUint32 algID, const unsigned char *key,
    WsecUint32 keyLen, const unsigned char *iv, WsecUint32 ivLen);

/* Symmetric stream encryption (add) */
unsigned long CacEncryptUpdate(KmcCbbCtx *kmcCtx, WsecHandle ctx, const unsigned char *plaintext, WsecUint32 plainLen,
    unsigned char *ciphertext, WsecUint32 *cipherLen);

/* Symmetric stream encryption (end) */
unsigned long CacEncryptFinal(KmcCbbCtx *kmcCtx, WsecHandle *ctx, unsigned char *ciphertext, WsecUint32 *cipherLen);

/* Symmetric decryption of stream data (start) */
unsigned long CacDecryptInit(KmcCbbCtx *kmcCtx, WsecHandle *ctx, WsecUint32 algID, const unsigned char *key,
    WsecUint32 keyLen, const unsigned char *iv, WsecUint32 ivLen);

/* Symmetric decryption of stream data (add) */
unsigned long CacDecryptUpdate(KmcCbbCtx *kmcCtx, WsecHandle ctx, const unsigned char *ciphertext, WsecUint32 cipherLen,
    unsigned char *plaintext, WsecUint32 *plainLen);

/* Symmetric decryption of stream data (end) */
unsigned long CacDecryptFinal(KmcCbbCtx *kmcCtx, WsecHandle *ctx, unsigned char *plaintext, WsecUint32 *plainLen);

/* Releasing the Decryption Environment */
void CacCipherFree(KmcCbbCtx *kmcCtx, WsecHandle *ctx);

/* Check the lib whether support specail hashAlg */
WsecBool CacCheckPriSupportHashAlg(WsecUint32 hashAlg);

/* Check the lib whether support specail hmacAlg */
WsecBool CacCheckPriSupportHmacAlg(WsecUint32 hmacAlg);

/* Check the lib whether support specail kdfAlg */
WsecBool CacCheckPriSupportKdfAlg(WsecUint32 kdfAlg);

/* Check the lib whether support specail symAlg */
WsecBool CacCheckPriSupportSymAlg(WsecUint32 symAlg);

unsigned long CacAsymGenKeypair(KmcCbbCtx *kmcCtx, const KmcDhApis *dhApis, WsecUint32 alg, unsigned char *priKey, WsecUint32 *priKeyLen,
                                unsigned char *pubKey, WsecUint32 *pubKeyLen);

unsigned long CacAsymShareKeyDerive(KmcCbbCtx *kmcCtx, const KmcDhApis *dhApis, WsecUint32 alg,
    const unsigned char *priKey, WsecUint32 priKeyLen, const unsigned char *pubKey, WsecUint32 pubKeyLen,
    unsigned char *shareKey, WsecUint32 *shareKeyLen);

WsecBool CacCheckSupportDHAlg(WsecUint32 alg);
unsigned long CacAsymDhSymbolSupport(KmcCbbCtx *kmcCtx, KmcDhApis *dhApis);

void CacAsymSymbolSupport(KmcAsymFuns *asymFuns);

#pragma pack(1)
typedef struct {
    WsecUint32 kdfAlg;
    WsecUint32 iter;
    WsecUint32 saltLen;
    const unsigned char *salt;
    WsecVoid *srkPrimitive;
    WsecUint32 srkPrimitiveLen;
} BuildRkParam;
#pragma pack()

#pragma pack(1)
typedef struct {
    WsecUint32 domainId;
    WsecUint32 keyId;
    WsecUint32 algId;
    unsigned char *mk;
    WsecUint32 mkLen;
    unsigned char *mkHash;
    WsecUint32 hashLen;
} BuildMkParam;
#pragma pack()

#pragma pack(1)
typedef struct {
    WsecUint32 kdfAlg;
    WsecUint32 iter;
    unsigned char *wk;
    WsecUint32 wkLen;
    unsigned char *salt;
    WsecUint32 saltLen;
} BuildWkParam;
#pragma pack()

#pragma pack(1)
typedef struct {
    WsecUint32 algId;
    WsecUint32 padding;
    WsecUint32 ivLen;
    unsigned char *iv;
} CryptOperParam;
#pragma pack()

#pragma pack(1)
typedef struct {
    WsecUint32 encryptType; /* KmcTeeStreamType */
    BuildRkParam rkParam;
    BuildMkParam mkParam;
    BuildWkParam wkParam;
    CryptOperParam operParam;
} CacCryptoParam;
#pragma pack()

#pragma pack(1)
typedef struct TagEncDecParam {
    WsecUint32 algId;
    WsecUint32 kdfAlg;
    WsecUint32 kdfIter;
    const unsigned char *salt;
    WsecUint32 saltLen;
    WsecUint32 wkLen;
} EncDecParam;
#pragma pack()


typedef struct TagKmcSynEncInfo {
    WsecUint32 cipherAlg;
    WsecUint32 kdfAlg;
    unsigned char salt[32];
    WsecUint32 saltLen;
    WsecUint32 iter;
    WsecUint32 wkLen;
    WsecVoid *shareKey;
    WsecUint32 shareKeyLen;
} KmcSynEncInfo;

unsigned long CacEnvTeeInit(KmcCbbCtx *kmcCtx, const KmcTeeInitParam *teeParam);

unsigned long CacEnvTeeUninit(KmcCbbCtx *kmcCtx);

unsigned long CacEnvCreateRk(KmcCbbCtx *kmcCtx, unsigned char *rk, WsecUint32 *rkLen,  WsecSrkPrimitive *rkPrimitive);

unsigned long CacEnvCreateMk(KmcCbbCtx *kmcCtx, WsecUint32 inMkLen, const BuildRkParam *rkParam, BuildMkParam *mkParam);

unsigned long CacEnvRegisterMk(KmcCbbCtx *kmcCtx, WsecUint32 flag, const WsecBuffConst *dataBuff, WsecBuffConst *inMk,
    const BuildRkParam *rkParam, BuildMkParam *mkParam);

unsigned long CacEnvImpExpMk(KmcCbbCtx *kmcCtx, WsecUint32 algId, WsecUint32 flag, const WsecBuffConst *inRk,
    const WsecBuffConst *inData, const WsecBuffConst *inMk, WsecBuff *outMk, WsecBuff *outHash, WsecUint32 isDeriveRk);

unsigned long CacEnvUpdateKeyWithRkProtectKey(KmcCbbCtx *kmcCtx, WsecUint32 algId, const WsecBuffConst *inRk,
    WsecBuffConst *inProtectKey, WsecBuffConst *oldRk, const WsecBuffConst *oldProtectKey, WsecBuffConst *inMk,
    WsecBuff *outMk, WsecBuff *outHash);

unsigned long CacBatchGetMkHash(KmcCbbCtx *kmcCtx, const WsecBuffConst *inRk, const WsecBuffConst *inMk,
    WsecUint32 isDeriveRk, WsecBuff *mkHash);

unsigned long CacUpdateSrk(KmcCbbCtx *kmcCtx, const KmcKsfMem *ksfMem);

unsigned long CacEnvUpdateMk(KmcCbbCtx *kmcCtx, ImpExpMkType mkType, KmcKsfMem *ksfMem, WsecBuffConst *oldRk);
unsigned long CacEnvImpUpdAsym(KmcCbbCtx *kmcCtx, ImpExpMkType mkType, KmcKsfMem *ksfMem, WsecBuffConst *protectKey,
    WsecBuffConst *oldKey, KmcNovaKsfMem *novaKsfMem);
unsigned long CacEnvUpGradeToTee(KmcCbbCtx *kmcCtx, KmcKsfMem *ksfMem);
unsigned long CacEnvUpdateRootKey(KmcCbbCtx *kmcCtx, KmcKsfMem *ksfMem, WsecBuffConst *oldRk);

unsigned long CacEnvPbkdf2(KmcCbbCtx *kmcCtx, BuildMkParam *mkParam, BuildWkParam *wkParam);

unsigned long CacEnvHmac(KmcCbbCtx *kmcCtx, CacCryptoParam *hmacParam, const unsigned char *inBuff, WsecUint32 inLen,
    unsigned char *outHmac, WsecUint32 *outLen);

WsecVoid CacEnvCipherReleaseCtx(KmcCbbCtx *kmcCtx, WsecHandle ctx);

WsecVoid CacEnvHmacReleaseCtx(KmcCbbCtx *kmcCtx, WsecHandle ctx);

unsigned long CacEnvUnprotectData(KmcCbbCtx *kmcCtx, const unsigned char *dataIn, unsigned int inLen,
    unsigned char *dataOut, unsigned int *outLen);

unsigned long TransAlgAndIter(KmcCbbCtx *kmcCtx, const unsigned char *str, unsigned long len, WsecUint32 *iter,
    unsigned char charKdfAlgId, WsecUint32 *kdfAlgId);

WsecUint32 CacGetKeyLen(WsecUint32 type, WsecUint32 algId);

WsecUint32 CacEnvGetPlainLen(KmcCbbCtx *kmcCtx, WsecUint32 plainLen, WsecUint32 cipherLen);

void CacInitRkParam(BuildRkParam *param, const unsigned char *rkSalt, WsecUint32 rkSaltLen,
    WsecSrkPrimitive *srkPrimitive);

void CacInitMkParam(BuildMkParam *param, unsigned char *mk, WsecUint32 mkLen);

void CacInitAllMkParam(BuildMkParam *param, WsecUint32 algId, unsigned char *mk, WsecUint32 mkLen,
    unsigned char *mkHash, WsecUint32 hashLen);

void CacInitWkParam(BuildWkParam *param, WsecUint32 kdfAlg, unsigned char *wk, WsecUint32 wkLen,
    unsigned char *salt, WsecUint32 saltLen, WsecUint32 iter);

void CacInitCryptOperParam(CryptOperParam *param, WsecUint32 algId,
    unsigned char *iv, WsecUint32 ivLen, WsecUint32 padding);

unsigned long CacEnvCheckSupport(KmcCbbCtx *kmcCtx);

unsigned long CacEnvMasterDhGenKeypair(KmcCbbCtx *kmcCtx, const KmcDhApis *dhApis, WsecUint32 alg, unsigned char *priKey, WsecUint32 *priKeyLen,
    unsigned char *pubKey, WsecUint32 *pubKeyLen, uint64_t *object);

unsigned long CacEnvMasterDhShareKeyDerive(KmcCbbCtx *kmcCtx, const KmcDhApis *dhApis, WsecUint32 alg, const unsigned char *priKey,
    WsecUint32 priKeyLen, unsigned char *pubKey, WsecUint32 pubKeyLen,
    unsigned char *shareKey, WsecUint32 *shareKeyLen, uint64_t object);

unsigned long CacEnvAgentDhShareKeyDerive(KmcCbbCtx *kmcCtx, const KmcDhApis *dhApis, WsecUint32 alg, unsigned char *priKey,
    WsecUint32 *priKeyLen, unsigned char *inPubKey, WsecUint32 inPubKeyLen,
    unsigned char *outPubKey, WsecUint32 *outPubKeyLen,
    unsigned char *shareKey, WsecUint32 *shareKeyLen);

WsecBool CacEnvCheckSupportDHAlg(KmcCbbCtx *kmcCtx, WsecUint32 alg, const unsigned char *algArr, WsecUint32 algLen);

unsigned long CacEnvSkEncryptMk(KmcCbbCtx *kmcCtx, const KmcSynEncInfo *encInfo, WsecBuffConst *protectKey,
    const unsigned char *shareKey, WsecUint32 shareKeyLen,
    const unsigned char *inMK, WsecUint32 inMkLen,
    unsigned char *outMK, WsecUint32 *outMkLen);

unsigned long CacEnvSkDecryptMk(KmcCbbCtx *kmcCtx, const KmcSynEncInfo *encInfo, WsecBuffConst *protectKey,
    const unsigned char *shareKey, WsecUint32 shareKeyLen,
    const unsigned char *inMK, WsecUint32 inMkLen,
    unsigned char *outMK, WsecUint32 *outMkLen);

WsecVoid CacEnvReleaseDHCtx(KmcCbbCtx *kmcCtx, WsecHandle ctx);

unsigned long CacEnvGetSupportDHAlg(KmcCbbCtx *kmcCtx, unsigned char *algArr, WsecUint32 *algLen);
unsigned long CacEnvDhSymbolSupport(KmcCbbCtx *kmcCtx, KmcDhApis *dhApis);

unsigned long CacEnvCheckSupportDh(KmcCbbCtx *kmcCtx, const KmcDhApis *dhApis);

unsigned long CacEnvExpWk(KmcCbbCtx *kmcCtx, const EncDecParam *param, const unsigned char *mk, WsecUint32 mkLen,
    unsigned char *key, WsecUint32 *keyLen);

unsigned long CacEnvUpdateNovaKey(KmcCbbCtx *kmcCtx, WsecBuffConst *newRk, WsecBuffConst *oldRk, KmcKsfMem *ksfMem);

unsigned long CacEnvNovaKeyUpGradeTee(KmcCbbCtx *kmcCtx, WsecBuffConst *rk, KmcKsfMem *ksfMem);

unsigned long CacEnvEncryptAsymKey(KmcCbbCtx *kmcCtx, WsecUint32 algId, WsecBuffConst *rkInfo,
                                   WsecBuffConst *mkCipher, WsecBuffConst *plain, WsecBuff *cipher);

unsigned long CacEnvGetIvLen(WsecUint32 algId, WsecUint32 *ivLen);

WsecBool CacIsAesAlg(WsecUint32 algId);

unsigned long CacEncryptWithTag(KmcCbbCtx *kmcCtx, WsecUint32 algId, const unsigned char *key, WsecUint32 keyLen,
    const unsigned char *iv, WsecUint32 ivLen, const unsigned char *plaintext, WsecUint32 plaintextLen,
    unsigned char *ciphertext, WsecUint32 *ciphertextLen, unsigned char *tag, WsecUint32 tagLen);

unsigned long CacDecryptWithTag(KmcCbbCtx *kmcCtx, WsecUint32 algId, const unsigned char *key, WsecUint32 keyLen,
    const unsigned char *iv, WsecUint32 ivLen, unsigned char *tag, WsecUint32 tagLen,
    const unsigned char *cipher, WsecUint32 cipherLen, unsigned char *plain, WsecUint32 *plainLen);
#if WSEC_COMPILE_ENABLE_ASYM

unsigned long CacEnvCreateAsymKeyPair(KmcCbbCtx *kmcCtx, WsecUint32 algId, WsecUint32 keySpec, WsecUint32 keyType,
    const BuildRkParam *rkParam, const BuildMkParam *mkParam, KmcNovaKeyPair *keyPair);

WsecUint32 CacAsymKeySpecToAlgType(WsecUint16 keySpec);

WsecUint32 CacAsymAlgIdToAlgType(WsecUint32 algId);

WsecUint32 CacEnvGetMaxAsymPlainLen(KmcCbbCtx *kmcCtx, WsecUint32 algId, WsecUint16 keySpec);

unsigned long CacEnvGetSignLen(WsecUint16 keySpec, WsecUint32 *signLen);

unsigned long CacEnvSign(KmcCbbCtx *kmcCtx, WsecUint32 algId, WsecUint16 keySpec, const BuildRkParam *rkParam,
    const BuildMkParam *mkParam, KmcNovaKeyPair *keyPair, WsecBuffConst *plain, WsecBuff *sign);

unsigned long CacEnvVerify(KmcCbbCtx *kmcCtx, WsecUint32 algId, WsecUint16 keySpec, WsecBuffConst *pub,
    WsecBuffConst *plain, WsecBuffConst *sign);

unsigned long CacEnvGetAsymCipherLen(WsecUint16 keySpec, WsecUint32 *cipherLen);

unsigned long CacEnvAsymEncrypt(KmcCbbCtx *kmcCtx, WsecAlgId algo, WsecBuffConst *pub, WsecBuffConst *plain,
    WsecBuff *cipher);

unsigned long CacEnvAsymDecrypt(KmcCbbCtx *kmcCtx, WsecUint32 algId, WsecUint16 keySpec, const BuildRkParam *rkParam,
    const BuildMkParam *mkParam, KmcNovaKeyPair *keyPair, WsecBuffConst *cipher, WsecBuff *plain);

#endif // WSEC_COMPILE_ENABLE_ASYM

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* KMC_SRC_CAC_CACV2_PRI_H */
