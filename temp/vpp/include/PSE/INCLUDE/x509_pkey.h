/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2005-2019. All rights reserved.
                            ALL RIGHTS RESERVED
     Description: Contains prototypes for the encode - decode functions for
                converting PKEY struct to DER (as per pkix1alg88) and back
    Author: Jimmy Bahuleyan
 * Create: 2005-07-17
        sec_pkixkey.h
    Project Code: SeCert
    Module Name: Common Module
   @brief: Contains prototypes for the encode - decode functions for
                converting PKEY struct to DER (as per pkix1alg88) and back
*/
#ifndef X509_PKEY_H
#define X509_PKEY_H

#include "ipsi_pse_build_conf.h"

#include "sec_crypto.h"
#include "sec_sys.h"
#include "ipsi_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup x509_pkey
 * This section contains the x509_pkey Functions.
 */
/**
 * @defgroup x509_pkeyFunctions
 * @ingroup x509_pkey
 * This section contains the x509_pkey Functions.
 */
/**
 * @defgroup SEC_encodePrivKey
 * @ingroup x509_pkeyFunctions
 * @par Prototype
 * @code
 * SEC_UCHAR* SEC_encodePrivKey (const SEC_PKEY_S* pstKey, SEC_UINT32* pulLen);
 * @endcode
 *
 * @par Purpose
 * To encode a given PKEY struct to the ASN struct in pkix1alg88 key struct has a private key.
 *
 * @par Description
 * This function encodes a given PKEY struct to the ASN struct in pkix1alg88
 * key struct has a private key.
 * @param[in] pstKey Key to be encoded [N/A]
 * @param[out] pulLen Length of the DER code generated in buffer [N/A]
 *
 * @retval SEC_UCHAR* The buffer containing the DER code [SEC_UCHAR*|N/A]
 * @retval SEC_UCHAR* If encode fails [NULL|N/A]
 *
 * @par Dependency
 * x509_pkey.h
 *
 * @par Note
 * \n
 * The buffer allocated by this API for return value should be freed by using function "ipsi_free".
 *
 * @par Related Topics
 * N/A
 *
 */
PSELINKDLL SEC_UCHAR *SEC_encodePrivKey(const SEC_PKEY_S *pKey, SEC_UINT32 *pulLen);

/**
 * @defgroup SEC_encodePubKey
 * @ingroup x509_pkeyFunctions
 * @par Prototype
 * @code
 * SEC_UCHAR* SEC_encodePubKey(const SEC_PKEY_S* pstKey, SEC_UINT32* pulLen)
 * @endcode
 *
 * @par Purpose
 * To encode a given PKEY struct to the ASN struct in pkix1alg88 key struct has a public key.
 *
 * @par Description
 * This function encodes a given PKEY struct to the ASN struct in pkix1alg88
 * key struct has a public key.
 *
 * @param[in] pstKey Key to be encoded [N/A]
 * @param[out] pulLen Length of the DER code generatedin buffer [N/A]
 *
 * @retval SEC_UCHAR* The buffer containing the DER code [SEC_UCHAR*|N/A]
 * @retval SEC_UCHAR* If encode fails [NULL|N/A]
 *
 * @par Dependency
 * x509_pkey.h
 *
 * @par Note
 * \n
 * The buffer allocated by this API for return value should be freed by using function "ipsi_free".
 *
 * @par Related Topics
 * N/A
 */
PSELINKDLL SEC_UCHAR *SEC_encodePubKey(const SEC_PKEY_S *pKey, SEC_UINT32 *pulLen);

/**
 * @defgroup SEC_decodePrivKey
 * @ingroup x509_pkeyFunctions
 * @par Prototype
 * @code
 * SEC_PKEY_S* SEC_decodePrivKey( SEC_UINT32 ulAlg, SEC_UCHAR* pucBuf, SEC_UINT32 ulLen, SEC_UINT32* pulLen)
 * @endcode
 *
 * @par Purpose
 * To decode a PKEY struct given a ASN struct as per pkix1alg88 key struct has a private key.
 *
 * @par Description
 * This function decodes to a PKEY struct given a ASN struct as per pkix1alg88
 * key struct has a private key.
 *
 * @param[in] ulAlg The algorithm type [N/A]
 * @param[in] pucBuf The buffer containing the DER code [N/A]
 * @param[in] ulLen Buffer length [N/A]
 * @param[in] pulLen Bytes used in decoding [N/A]
 * @param[out] N/A N/A [N/A]
 *
 * @retval SEC_PKEY_S* The key struct [SEC_PKEY_S*|N/A]
 * @retval SEC_PKEY_S* If decode fails [NULL|N/A]
 *
 * @par Dependency
 * x509_pkey.h
 *
 * @par Note
 * \n
 * The key structure returned by this API should be freed by using function "CRYPT_PKEY_free".
 *
 * @par Related Topics
 * N/A
 */
/* This function is used by SSL. Hence the import define is added here */
PSELINKDLL SEC_PKEY_S *SEC_decodePrivKey(SEC_UINT32 ulAlg, SEC_UCHAR *pucBuf, SEC_UINT32 ulLen, SEC_UINT32 *pulLen);

/**
 * @defgroup SEC_decodePubKey
 * @ingroup x509_pkeyFunctions
 * @par Prototype
 * @code
 * SEC_PKEY_S* SEC_decodePubKey( SEC_UINT32 ulAlg, SEC_UCHAR* pucBuf, SEC_UINT32 ulLen, SEC_UINT32* pulLen)
 * @endcode
 *
 * @par Purpose
 * To decode a PKEY struct given a ASN struct as per pkix1alg88
 * key struct has a public key.
 *
 * @par Description
 * This function decodes to a PKEY struct given a ASN struct as per pkix1alg88
 * key struct has a public key.
 *
 * @param[in] ulAlg The algorithm type [N/A]
 * @param[in] pucBuf The buffer containing the DER code [N/A]
 * @param[in] ulLen Buffer length [N/A]
 * @param[in] pulLen Bytes used in decoding [N/A]
 * @param[out] N/A N/A [N/A]
 *
 * @retval SEC_PKEY_S* The key struct [SEC_PKEY_S*|N/A]
 * @retval SEC_PKEY_S* If decode fails [NULL|N/A]
 *
 * @par Dependency
 * x509_pkey.h
 *
 * @par Note
 * \n
 * The key structure returned by this API should be freed by using function "CRYPT_PKEY_free".
 *
 * @par Related Topics
 * N/A
 */
PSELINKDLL SEC_PKEY_S *SEC_decodePubKey(SEC_UINT32 uiAlg, SEC_UCHAR *pucBuf, SEC_UINT32 ulLen, SEC_UINT32 *pulLen);

/**
 * @defgroup SEC_encodePubKeyInfo
 * @ingroup x509Functions
 * @par Prototype
 * @code
 * SEC_UCHAR *SEC_encodePubKeyInfo(const SEC_PKEY_S *pstKey, SEC_UINT32 *pulLen);
 * )
 * @endcode
 * @par Purpose
 * To encode the public in as Public Key Info
 * @par Description
 * This function create PublicKeyInfo structure using  SEC_PKEY_S and encode the PublicKeyInfo structure.
 * The PublicKeyInfo structure  contains the Public Key and Algorithm Identifier which specifies the algorithm
 * used to generate the key.  Encoded length is reflected in puiEncodedLen parameter after encoding.

 * @param[in] pstKey Key to be encoded as public key info [N/A]
 * @param[out] puiEncodedLen The length of the encoded string [N/A]
 * @retval SEC_UCHAR* Pointer to the encoded string [SEC_UCHAR*|N/A]
 * @retval SEC_UCHAR* On all failure conditions SEC_NULL is returned. The failure conditons can be one of the following:
 * Input parameter is NULL, Memory allocation fails or Encoding of the Public Key fails [SEC_NULL|N/A]
 * @par Dependency
 * x509.h
 *
 * @par Memory Handling
 * ipsi will allocate the memory for the encoded PubKey Info, to free this memory
 * application has to call the ipsi_free function.
 *
 * @par Note
 *
 * N/A
 *
 * @par Related Topics
 * N/A
*/
PSELINKDLL SEC_UCHAR *SEC_encodePubKeyInfo(const SEC_PKEY_S *pstKey, SEC_UINT32 *pulLen);

/**
 * @defgroup SEC_decodePubKeyInfo
 * @ingroup x509Functions
 * @par Prototype
 * @code
 * SEC_PUBKEY_INFO_S* SEC_decodePubKeyInfo(SEC_UCHAR* pucBuffer,
 * SEC_UINT32 uiEncodedLength,
 * SEC_UINT32* puiDecodedLen
 * )
 * @endcode
 * @par Purpose
 * To generate a SEC_PKEY from the encoded PublicKeyInfo data.
 * @par Description
 * This function returns pointer to SEC_PKEY structure by decoding using the input parameter passed to this
 * function.
 * @param[in] pucBuffer The encoded PublicKeyInfo data that has to be decoded [N/A]
 * @param[in] uiEncodedLength The length of the encoded character stream [N/A]
 * @param[out] puiDecodedLen Number of Bytes that is decoded [N/A]
 * @retval SEC_PKEY* On Successful execution pointer to SEC_PKEY structure is returned after decoding
 * [SEC_PKEY_S*|N/A]
 * @retval SEC_PKEY* On all failure conditions SEC_NULL is returned. The failure conditons can be one of the
 * following:
 * Input parameter is NULL, Memory allocation fails or Decoding of input data fails [SEC_NULL|N/A]
 * @par Dependency
 * x509.h
 *
 * @par Memory Handling
 * ipsi will allocate the memory for the SEC_PKEY_S structure, to free this memory
 * application has to call the CRYPT_PKEY_free function.
 *
 * @par Note
 *
 * N/A
 *
 * @par Related Topics
 * N/A
 */
PSELINKDLL SEC_PKEY_S *SEC_decodePubKeyInfo(SEC_UCHAR *pucBuf, SEC_UINT32 ulLen, SEC_UINT32 *pulLen);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
