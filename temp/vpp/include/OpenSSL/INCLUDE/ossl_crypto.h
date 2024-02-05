/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * File: ossl_crypto.h
 * Description: This API is used to computes sign of a given data block. Currently only sign with ed25519 key is
 * supported
 * Author: VPP
 * Create: 2019-8-8
 */
#ifndef OSSL_CRYPTO_H
#define OSSL_CRYPTO_H

#include "openssl/ossl_typ.h"
#include "openssl/evp.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OSSL_TLS_SUCCESS 1
#define OSSL_TLS_FAIL 2

/**
 * Func Name:  OSSL_EVP_PKEY_sign
 * @defgroup OSSL_EVP_PKEY_sign
 * @par Prototype
 * @code
 * int OSSL_EVP_PKEY_sign(const void *pkey, const char *optname[],size_t optnamesize,
 *                                   const char *optvalue[],size_t optvalsize,
 *                                   const unsigned char *tobesigned, size_t tobesignedlen,
 *                                   unsigned char *signature, size_t *signatureSize);
 * @endcode
 *
 * @par Purpose
 * This API is used to computes sign of a given data block.
 *
 * @par Description
 * This API is used to computes sign of a given data block. Currently only sign with ed25519 key is supported.
 *
 * @param[in] pkey Private Key used for signing [N/A]
 * @param[in] optname list of option name. [N/A]
 * @param[in] optnamesize number of option name in optname [N/A]
 * @param[in] optvalue list of option value[N/A]
 * @param[in] optvalsize number of option name in optname  [N/A]
 * @param[in] tobesigned Data to be signed [N/A]
 * @param[in] tobesignedlen The data length [N/A]
 * @param[out] signature Buffer to store signature [N/A]
 * @param[in/out] signatureSize Size of signature buffer, API will update this pointer to signature size [N/A]
 *
 * @retval int On Success [OSSL_TLS_SUCCESS|N/A]
 * @retval int On error, other than OSSL_TLS_SUCCESS [N/A]
 *
 * @par Required Header File
 * ossl_crypto.h
 *
 * @par Note
 * - Currently optname, optnamesize, optvalue and optvalsize are not used, these parameter are future use. Values passed
 * for these parameter are ignored
 * - Signing with only ed25519 keys is supported.
 */
int OSSL_EVP_PKEY_sign(const void *pkey, const char *optname[], size_t optnamesize, const char *optvalue[],
                       size_t optvalsize, const unsigned char *tobesigned, size_t tobesignedlen,
                       unsigned char *signature, size_t *signatureSize);

/**
 * Func Name:  OSSL_EVP_PKEY_verify
 * @defgroup OSSL_EVP_PKEY_verify
 * @par Prototype
 * @code
 * int OSSL_EVP_PKEY_verify(const void *pkey, const char *optname[],size_t optnamesize,
 *                                   const char *optvalue[],size_t optvalsize,
 *                                   const unsigned char *tobesigned, size_t tobesignedlen,
 *                                   unsigned char *signature, size_t *signatureSize);
 * @endcode
 *
 * @par Purpose
 * This API is used to compares sign on a given data block.
 *
 * @par Description
 * This API is used to compares sign on a given data block. Currently only verify with ed25519 key is supported.
 *
 * @param[in] pkey Public key used for verify.[N/A]
 * @param[in] optname list of option name. [N/A]
 * @param[in] optnamesize number of option name in optname [N/A]
 * @param[in] optvalue list of option value[N/A]
 * @param[in] optvalsize number of option name in optname  [N/A]
 * @param[in] tobesigned Data to be signed [N/A]
 * @param[in] tobesignedlen The data length [N/A]
 * @param[in] signature  Signature to verify[N/A]
 * @param[in] signatureSize Length of signature [N/A]
 *
 * @retval int On Success [OSSL_TLS_SUCCESS|N/A]
 * @retval int On error, other than OSSL_TLS_SUCCESS [N/A]
 *
 * @par Required Header File
 * ossl_crypto.h
 *
 * @par Note
 * - Currently optname, optnamesize, optvalue and optvalsize are not used, these parameter are future use. Values passed
 * for these parameter are ignored
 * - Verification with only ed25519 keys is supported.
 */
int OSSL_EVP_PKEY_verify(const void *pkey, const char *optname[], size_t optnamesize, const char *optvalue[],
                         size_t optvalsize, const unsigned char *tobesigned, size_t tobesignedlen,
                         const unsigned char *signature, size_t signatureLength);

/**
 * Func Name:  OSSL_EVP_PKEY_pvt_encode
 * @defgroup OSSL_EVP_PKEY_pvt_encode
 * @par Prototype
 * @code
 * int OSSL_EVP_PKEY_pvt_encode(const void *pkey, const unsigned char *keytype, size_t keybuffsize,
 *                                           unsigned char ** buff , size_t * len);
 * @endcode
 *
 * @par Purpose
 * This API is used to encode the private key.
 *
 * @par Description
 * This API is used to encode the private key.
 *
 * @param[in] pkey Private key to encoded.[N/A]
 * @param[out] keytype Type of private key encoded.[N/A]
 * @param[in] keybuffsize Size of keytype buffer.[N/A]
 * @param[out] buff The buffer containing the DER encoded buffer [N/A]
 * @param[out] len Length of the DER generated in buffer [N/A]
 *
 * @retval int On Success [OSSL_TLS_SUCCESS|N/A]
 * @retval int On error, other than OSSL_TLS_SUCCESS [N/A]
 *
 * @par Required Header File
 * ossl_crypto.h
 *
 * @par Note
 * - Encoding of only ed25519 private keys is supported.
 * - API will allocate the memory for the encoded PrivateKey Info, PrivateKey is sensitive data, to free this memory
 * Suggest application call the OPENSSL_clear_free function.
 */
int OSSL_EVP_PKEY_pvt_encode(const void *pkey, const char *keytype, size_t keybuffsize, unsigned char **buff,
                             size_t *len);
/**
 * Func Name:  OSSL_EVP_PKEY_pub_encode
 * @defgroup OSSL_EVP_PKEY_pub_encode
 * @par Prototype
 * @code
 * int OSSL_EVP_PKEY_pub_encode(const void *pkey, const unsigned char *keytype, size_t keybuffsize,
 *                               unsigned char ** buff , size_t * len);
 * @endcode
 *
 * @par Purpose
 * This API is used to encode the public key as PublicKeyInfo.
 *
 * @par Description
 * This API is used to encode the publick key as PublicKeyInfo.
 *
 * @param[in] pkey Public key to encoded.[N/A]
 * @param[out] keytype Type of private key encoded.[N/A]
 * @param[in] keybuffsize Size of keytype buffer.[N/A]
 * @param[out] buff The buffer containing the DER encoded PublicKeyInfo. [N/A]
 * @param[out] len Length of the DER generated in buffer [N/A]
 *
 * @retval int On Success [OSSL_TLS_SUCCESS|N/A]
 * @retval int On error, other than OSSL_TLS_SUCCESS [N/A]
 *
 * @par Required Header File
 * ossl_crypto.h
 *
 * @par Note
 * - Encoding of only ed25519 public keys is supported.
 * - API will allocate the memory for the encoded PubKey Info, to free this memory
 * application has to call the OPENSSL_free function.
 */
int OSSL_EVP_PKEY_pub_encode(const void *pkey, const char *keytype, size_t keybuffsize, unsigned char **buff,
                             size_t *len);

/**
 * Func Name:  OSSL_EVP_PKEY_pvt_decode
 * @defgroup OSSL_EVP_PKEY_pvt_decode
 * @par Prototype
 * @code
 * int OSSL_EVP_PKEY_pvt_decode(const char * keytype, unsigned char * buff , size_t len, void **pkey);
 * @endcode
 *
 * @par Purpose
 * This API is used to decode the encoded the private key.
 *
 * @par Description
 * This API is used to decode the encoded the private key.
 *
 *
 * @param[in] keytype Type of enocded private key.[N/A]
 * @param[in] buff The buffer containing the DER encoded PublicKeyInfo. [N/A]
 * @param[in] len Length of the DER encoded data in buff [N/A]
 * @param[out] pkey decode private key  [N/A]
 *
 * @retval int On Success [OSSL_TLS_SUCCESS|N/A]
 * @retval int On error, other than OSSL_TLS_SUCCESS [N/A]
 *
 * @par Required Header File
 * ossl_crypto.h
 *
 * @par Note
 * - Decoding of only ed25519 private keys is supported.
 * - API will allocate the memory for the decoded private key, to free this memory
 * application has to call the OSSL_EVP_PKEY_free function.
 */
int OSSL_EVP_PKEY_pvt_decode(const char *keytype, unsigned char *buff, size_t len, void **pkey);

/**
 * Func Name:  OSSL_EVP_PKEY_pub_decode
 * @defgroup OSSL_EVP_PKEY_pub_decode
 * @par Prototype
 * @code
 * int OSSL_EVP_PKEY_pub_decode(const char * keytype,unsigned char * buff , size_t len, void **pkey)
 * @endcode
 *
 * @par Purpose
 * This API is used to decode the encoded PublicKeyInfo.
 *
 * @par Description
 * This API is used to decode the encoded PublicKeyInfo.
 *
 * @param[in] keytype Type of encoded public key.[N/A]
 * @param[in] buff The buffer containing the DER encoded PublicKeyInfo. [N/A]
 * @param[in] len Length of the DER encoded data in buff  [N/A]
 * @param[out] pkey decode public key  [N/A]
 *
 * @retval int On Success [OSSL_TLS_SUCCESS|N/A]
 * @retval int On error, other than OSSL_TLS_SUCCESS [N/A]
 *
 * @par Required Header File
 * ossl_crypto.h
 *
 * @par Note
 * - Decoding of only ed25519 public keys is supported.
 * - API will allocate the memory for the decoded public key, to free this memory
 * application has to call the OSSL_EVP_PKEY_free function.
 */
int OSSL_EVP_PKEY_pub_decode(const char *keytype, unsigned char *buff, size_t len, void **pkey);

/**
 *  Func Name:  OSSL_EVP_PKEY_free
 * @defgroup OSSL_EVP_PKEY_pub_decode
 * @par Prototype
 * @code
 * void OSSL_EVP_PKEY_free(void* pkey)
 * @endcode
 *
 * @par Purpose
 * This API is used to free the memory of pkey.
 *
 * @par Description
 * This API is used to free the memory of pkey.
 *
 * @param[in] pkey key to be freed.[N/A]
 *
 * @retval void This does not return any value [N/A|N/A]
 *
 * @par Required Header File
 * ossl_crypto.h
 *
 * @par Note
 * - NA
 */
void OSSL_EVP_PKEY_free(void *pkey);

/**
 *  Func Name:  OSSL_EVP_PKEY_keygen
 * @defgroup OSSL_EVP_PKEY_keygen
 * @par Prototype
 * @code
 * int OSSL_EVP_PKEY_keygen(const char *algname,const char *optname[],size_t optnamesize,
 *                                   const char *optvalue[],size_t optvalsize,
 *                                   void **pkey, EVP_PKEY_gen_cb *pcb);
 *
 * @endcode
 *
 * @par Purpose
 * This API is used to generate the asymetric key.
 *
 * @par Description
 * This API is used to generate the asymetric key.. Currently only ed25519 and x25519 key generation is supported.
 *
 * @param[in] pkey Private Key used for signing [N/A]
 * @param[in] algname Algorithm for which key need to generated [N/A]
 * @param[in] optname list of option name. [N/A]
 * @param[in] optnamesize number of option name in optname [N/A]
 * @param[in] optvalue list of option value[N/A]
 * @param[in] optvalsize number of option name in optname  [N/A]
 * @param[out] pkey generate assymteric keys [N/A]
 * @param[in] pcb callback pointer [N/A]
 *
 * @retval int On Success [OSSL_TLS_SUCCESS|N/A]
 * @retval int On error, other than OSSL_TLS_SUCCESS [N/A]
 *
 * @par Required Header File
 * ossl_crypto.h
 *
 * @par Note
 * - Currently optname, optnamesize, optvalue , optvalsize and pcb are not used, these parameter are future use.
 * Values passed for these parameter will be ignored.
 * - Key generation of ed25519 and x25519 supported.
 * - Need call OSSL_EVP_PKEY_free() API to free the key if the key no use.
 */
int OSSL_EVP_PKEY_keygen(const char *algname, const char *optname[], size_t optnamesize, const char *optvalue[],
                         size_t optvalsize, void **pkey, EVP_PKEY_gen_cb *pcb);

/**
 * Func Name:  OSSL_EVP_PKEY_derive
 * @defgroup OSSL_EVP_PKEY_derive
 * @par Prototype
 * @code
 * int OSSL_EVP_PKEY_derive(EVP_PKEY *pkey, EVP_PKEY *peerkey, const char *optname[],size_t optnamesize,
                                    const char *optvalue[],size_t optvalsize, unsigned char *key, size_t *keylen);
 *
 * @endcode
 *
 * @par Purpose
 * This API is used to generate the shared secret .
 *
 * @par Description
 * This API is used to generate the shared secret. Currently only x25519 shared sceret generation is supported.
 *
 * @param[in] pkey Private Key [N/A]
 * @param[in] peerkey Peer public key [N/A]
 * @param[in] optname list of option name. [N/A]
 * @param[in] optnamesize number of option name in optname [N/A]
 * @param[in] optvalue list of option value[N/A]
 * @param[in] optvalsize number of option name in optname  [N/A]
 * @param[out] key generate shared secret [N/A]
 * @param[out] keylen length of shared secret. [N/A]
 *
 * @retval int On Success [OSSL_TLS_SUCCESS|N/A]
 * @retval int On error, other than OSSL_TLS_SUCCESS [N/A]
 *
 * @par Required Header File
 * ossl_crypto.h
 *
 * @par Note
 * - Currently optname, optnamesize, optvalue and optvalsize are not used, these parameter are future use. Values passed
 * for these parameter will be ignored.
 * - Shared secret genreaton of only x25519 algorithm is supported.
 */
int OSSL_EVP_PKEY_derive(EVP_PKEY *pkey, EVP_PKEY *peerkey, const char *optname[], size_t optnamesize,
                         const char *optvalue[], size_t optvalsize, unsigned char *key, size_t *keylen);

/**
 * Func Name:  OSSL_EncodeOid
 * @defgroup OSSL_EncodeOid
 * @par Prototype
 * @code
 * int OSSL_EncodeOid(int nid, int sigmd_type, int mgf1md_type, int saltlen, int trailerfield,
                      unsigned char **buff, int *len)
 *
 * @endcode
 *
 * @par Purpose
 * This API is used to generate the encoded oid.
 *
 * @par Description
 * This API is used to generate the encoded oid.
 *
 * @param[in] nid nid to be enbcoded [N/A]
 * @param[in] sigmd_type Hash Algorithm used in RSASSA_PSS signature  [N/A]
 * @param[in] mgf1md_type MaskGenAlgorithm used in RSASSA_PSS signature. [N/A]
 * @param[in] saltlen octet length of the salt used in RSASSA_PSS signature.[N/A]
 * @param[in] trailerfield trailerfield to used in encoding NID_rsassaPss [N/A]
 * @param[out] buff Encoded oid buffer  [N/A]
 * @param[out] len Length of encoded oid [N/A]
 *
 * @retval int On Success [OSSL_TLS_SUCCESS|N/A]
 * @retval int On error, other than OSSL_TLS_SUCCESS [N/A]
 *
 * @par Required Header File
 * ossl_crypto.h
 *
 * @par Note
 * - Support encoding of  NID_ED25519, NID_sha256WithRSAEncryption, NID_sha384WithRSAEncryption,
     NID_sha512WithRSAEncryption,NID_dsa_with_SHA256,NID_ecdsa_with_SHA256, NID_ecdsa_with_SHA384,
     NID_ecdsa_with_SHA512 and NID_rsassaPss
 * - For NID_rsassaPss , application should pass sigmd_type, mgf1md_type, saltlen and trailerfield.For other algorithm,
     these parameter are ignored
 */
int OSSL_EncodeOid(int nid, int sigmd_type, int mgf1md_type, int saltlen, int trailerfield,
                   unsigned char **buff, int *len);

/**
 * Func Name:  OSSL_decode_oid
 * @defgroup OSSL_decode_oid
 * @par Prototype
 * @code
 * int OSSL_DecodeOid(const unsigned char *buff, int len, int *sigmd_type, int *mgf1hash_type, int *saltlen,
                        int *trailerField);
 *
 * @endcode
 *
 * @par Purpose
 * This API is used to decode the oid.
 *
 * @par Description
 * This API is used to decode the oid.
 *
 * @param[int] buff Encoded oid buffer  [N/A]
 * @param[int] len Length of encoded oid [N/A]
 * @param[out] sigmd_type Hash Algorithm used in RSASSA_PSS signature  [N/A]
 * @param[out] mgf1md_type MaskGenAlgorithm used in RSASSA_PSS signature. [N/A]
 * @param[out] saltlen octet length of the salt used in RSASSA_PSS signature.[N/A]
 * @param[out] trailerfield trailerfield to used in encoding NID_rsassaPss [N/A]

 *
 * @retval int On Success [NID_ED25519, NID_sha256WithRSAEncryption, NID_sha384WithRSAEncryption,
               NID_sha512WithRSAEncryption, NID_dsa_with_SHA256, NID_ecdsa_with_SHA256, NID_ecdsa_with_SHA384,
               NID_ecdsa_with_SHA512 and NID_rsassaPss|N/A]
 * @retval int On error, [NID_undef |N/A]
 *
 * @par Required Header File
 * ossl_crypto.h
 *
 * @par Note
 * - Support decoding of  NID_ED25519, NID_sha256WithRSAEncryption, NID_sha384WithRSAEncryption,
     NID_sha512WithRSAEncryption, NID_dsa_with_SHA256, NID_ecdsa_with_SHA256, NID_ecdsa_with_SHA384,
     NID_ecdsa_with_SHA512 and  NID_rsassaPss
 * - sigmd_type, mgf1md_type, saltlen and trailerfield used only for NID_rsassa_Pss , for other algorithm
     these parameter are ignored
 * - For NID_rsassaPss , API will update sigmd_type, mgf1md_type, saltlen and trailerfield
 */
int OSSL_DecodeOid(const unsigned char *buff, int len, int *sigmd_type, int *mgf1hash_type, int *saltlen,
                   int *trailerField);

#ifdef __cplusplus
}
#endif /* end of __cplusplus */
#endif /* end of OSSL_CRYTPO_H */
