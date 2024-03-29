/**

 * Copyright (c) Huawei Technologies Co., Ltd. 2005-2019. All rights reserved.
 * Description: This module provides facility to construct a structure which
                 contains the private key info i.e version of the private key
                 It also provaides the facility to encrypt and decrypt the
                 the private key information
                            ALL RIGHTS RESERVED

------------------------------------------------------------------------------

                              pkcs8.h

  Project Code: SeCert
  Module Name: pkcs8

 * Create: 2005-06-30
  Author: Vinay Purohit
  Descrption:  This module provides facility to construct a structure which
                 contains the private key info i.e version of the private key
                 It also provaides the facility to encrypt and decrypt the
                 the private key information *

*/

#ifndef PKCS8_H
#define PKCS8_H

#include "ipsi_pse_build_conf.h"

#include "asn-types.h"
#include "sec_sys.h"
#include "x509_basictypes.h"
#include "sec_crypto.h"
#include "ipsi_types.h"
#include "sec_list.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PKCS8_PRIVKEY_VERSION 0 /* Priv Key structure Version */

/**
 * @defgroup pkcs8
 * This section contains the pkcs8 Structures and Functions.
 */
/**
 * @defgroup pkcs8Structures
 * @ingroup pkcs8
 * This section contains the pkcs8 Structures.
 */
/*
To hold the information about the Private Key
*/
/**
 * @defgroup PKCS8_PRIVKEY_S
 * @ingroup pkcs8Structures
 * @par Prototype
 * @code
 * typedef struct stPKCS8_PrivKeyInfo
 * {
 * 	SEC_UINT32 ulVersion
 * 	SEC_ALG_IDENTIFIER_S* pstAlg
 * 	SEC_AsnOcts_S stPrivKey
 * 	SEC_List_S* pstAttList
 * }PKCS8_PRIVKEY_S
 * @endcode
 *
 * @datastruct ulVersion Version of the Private Key Structure.
 * @datastruct pstAlg PrivKeyAlgorithmIdentifier.
 * @datastruct stPrivKey Encoded value of SEC_PKEY_S.
 * @datastruct pstAttList IMPLICIT Attributes OPTIONAL. In this version PKCS8 Attribute is not defined.
 * So the List will always be empty. It will be used in future when PKCS8 Attribute will be defined.
 */
typedef struct stPKCS8_PrivKeyInfo { /* SEQUENCE */
    SEC_UINT32 ulVersion;            /* Version of the Private Key Structure */
    SEC_ALG_IDENTIFIER_S *pstAlg;    /* PrivKeyAlgorithmIdentifier */
    SEC_AsnOcts_S stPrivKey;         /* Encoded value of SEC_PKEY_S */
    SEC_List_S *pstAttList;          /* IMPLICIT Attributes OPTIONAL. In this version  PKCS8 Attribute is not defined.
                 So the List will always be empty. It will be used in future when PKCS8 Attribute will be
                defined. */
} PKCS8_PRIVKEY_S;

/*
To hold the information about the  PKCS8 Encrypted Key.It is also used to store the digest info of the Authenticated
Safe  of PKCS12. This Structure is part of PKCS12  PFX Structure and represents PKCS8 Encrypted Private Key too.
*/
/**
 * @defgroup PKCS8_ENCRYPTED_PRIVKEY_S
 * @ingroup pkcs8Structures
 * @par Prototype
 * @code
 * typedef struct stSEC_DataAlgInfo
 * {
 * 	SEC_ALG_IDENTIFIER_S* pstAlg
 * 	SEC_AsnOcts_S stData
 * } PKCS8_ENCRYPTED_PRIVKEY_S
 * @endcode
 *
 * @datastruct pstAlg Encryption algorithm identifier.
 * @datastruct stData Encrypted key.
 */
/**
 * @defgroup PKCS7_DIGEST_INFO_S
 * @ingroup pkcs8Structures
 * @par Prototype
 * @code
 * typedef struct stSEC_DataAlgInfo
 * {
 * 	SEC_ALG_IDENTIFIER_S* pstAlg
 * 	SEC_AsnOcts_S stData
 * } PKCS7_DIGEST_INFO_S
 * @endcode
 *
 * @datastruct pstAlg Digest algorithm identifier.
 * @datastruct stData Digest data.
 */
typedef struct stSEC_DataAlgInfo { /* SEQUENCE */
    SEC_ALG_IDENTIFIER_S *pstAlg;  /* Encryption or digest AlgorithmIdentifier */
    SEC_AsnOcts_S stData;          /* EncryptedKey or digest Data */
} PKCS8_ENCRYPTED_PRIVKEY_S, PKCS7_DIGEST_INFO_S;

/**
 * @defgroup pkcs8Functions
 * @ingroup pkcs8
 * This section contains the pkcs8 Functions
 */
/**
* @defgroup PKCS8_createPrivKey
* @ingroup pkcs8Functions
* @par Prototype
* @code
* PKCS8_PRIVKEY_S* PKCS8_createPrivKey(SEC_UINT32 ulVersion,
* SEC_PKEY_S* pstPkey
* )
* @endcode
*
* @par Purpose
* To construct the PKCS8_PRIVKEY_S structure which holds the encoded SEC_PKEY_S.
*
* @par Description
* This function constructs the PKCS8_PRIVKEY_S structure which holds the encoded SEC_PKEY_S.
*
* @param[in] ulVersion Version of the private key.In this version it should be 0.Use PKCS8_PRIVKEY_VERSION for it [N/A]
* @param[in] pstPkey Key Structure contains Key Pair information about the private Key i.e generation algorithm.
* To create SEC_PKEY_S call CRYPT_genKeyPair API [N/A]
* @param[out] N/A N/A [N/A]

* @retval PKCS8_PRIVKEY_S* Structure which contains private key [PKCS8_PRIVKEY_S*|N/A]
* @retval PKCS8_PRIVKEY_S* In case of errors, Any Of the Inputs is invalid i.e Null,Memory allocation failed,Version is
not 0
* and Encode for SEC_PKEY_S fails [SEC_NULL|N/A]
*
* @par Dependency
* pkcs8.h
*
* @par Note
* The keys generated by this API are not backward compatible with old iPSI
* versions such as V1R1, V1R3, V1R5, V1R6.
*
*
* @par Related Topics
* N/A
*
*
*/
PSELINKDLL PKCS8_PRIVKEY_S *PKCS8_createPrivKey(SEC_UINT32 ulVersion, SEC_PKEY_S *pstPrivkey);

/**
 * @defgroup PKCS8_getVersion
 * @ingroup pkcs8Functions
 * @par Prototype
 * @code
 * SEC_UINT32	PKCS8_getVersion(
 * const PKCS8_PRIVKEY_S* pstPrivKeyInfo
 * )
 * @endcode
 *
 * @par Purpose
 * To extract the version of the private key from PKCS8_PRIVKEY_S structure.
 *
 * @par Description
 * This function extracts the version of the private key from PKCS8_PRIVKEY_S structure.
 *
 * @param[in] pstPrivKeyInfo Structure which holds the version of the private key [N/A]
 * @param[out] N/A N/A [N/A]
 *
 * @retval SEC_UINT32 Version of the private key or error code indicating the reason for failure [SEC_UINT32|N/A]
 * @retval SEC_UINT32 If input is absent that is SEC_NULL [SEC_ERR_INVALID_ARG|N/A]
 *
 * @par Dependency
 * pkcs8.h
 *
 * @par Note
 *
 * N/A
 *
 * @par Related Topics
 * N/A
 */
PSELINKDLL SEC_UINT32 PKCS8_getVersion(const PKCS8_PRIVKEY_S *pstPrivKeyInfo);

/**
 * @defgroup PKCS8_extractPrivKey
 * @ingroup pkcs8Functions
 * @par Prototype
 * @code
 * SEC_PKEY_S* PKCS8_extractPrivKey(
 * PKCS8_PRIVKEY_S* pstPrivKeyInfo
 * )
 * @endcode
 *
 * @par Purpose
 * To get the private key from the PKCS8_PRIVKEY_S structure.
 *
 * @par Description
 * This function is used to get the private key from the PKCS8_PRIVKEY_S structure.
 *
 * @param[in] pstPrivKeyInfo Pointer to structure which holds the private key value [N/A]
 * @param[out] N/A N/A [N/A]
 *
 * @retval SEC_PKEY_S* Pointer to structure of crypto Private Key Structure or SEC_NULL in case of error
 * SEC_PKEY_S* need to be free by CRYPT_PKEY_free
 * [SEC_PKEY_S*|N/A]
 * @retval SEC_PKEY_S* In case of errors, One or more of the Inputs are invalid\n
 * Malloc failed\n
 * Decode for SEC_PKEY_S fails.\n
 * PKCS8_PRIVKEY_S contains invalid ALGID [SEC_NULL|N/A]
 *
 * @par Dependency
 * pkcs8.h
 *
 * @par Note
 * It decodes the SEC_PKEY_S present in the PKCS8_PRIVKEY_S structure.
 *
 * @par Related Topics
 * N/A
 */
PSELINKDLL SEC_PKEY_S *PKCS8_extractPrivKey(PKCS8_PRIVKEY_S *pstPrivKeyInfo);

/**
 * @defgroup PKCS8_extractPrivKeyPFX
 * @ingroup pkcs8Functions
 * @par Prototype
 * @code
 * SEC_PKEY_S* PKCS8_extractPrivKeyPFX(
 * PKCS8_PRIVKEY_S* pstPrivKeyInfo
 * )
 * @endcode
 *
 * @par Purpose
 * To get the DSA private key from the PKCS8_PRIVKEY_S structure, PFX format files only.
 *
 * @par Description
 * This function is used to get the DSA private key from the given PKCS8_PRIVKEY_S structure.
 * This API is to be used when the private key is passed in the form of a PFX Certificate.
 *
 * @param[in] pstPrivKeyInfo Pointer to structure which holds the private key value [N/A]
 * @param[out] N/A N/A [N/A]
 *
 * @retval SEC_PKEY_S* Pointer to structure of crypto Private Key Structure or SEC_NULL in case of error
 * [SEC_PKEY_S*|N/A]
 * @retval SEC_PKEY_S* In case of errors, One or more of the Inputs are invalid\n
 * Malloc failed\n
 * Decode for SEC_PKEY_S fails.\n
 * PKCS8_PRIVKEY_S contains invalid ALGID [SEC_NULL|N/A]
 *
 * @par Dependency
 * pkcs8.h
 *
 * @par Note
 * It decodes the SEC_PKEY_S present in the PKCS8_PRIVKEY_S structure.
 * To be used only if the extracted Private key structure was from a PFX store.
 *
 * @par Related Topics
 * N/A
 */
PSELINKDLL SEC_PKEY_S *PKCS8_extractPrivKeyPFX(PKCS8_PRIVKEY_S *pstPrivKeyInfo);

/**
 * @defgroup PKCS8_checkVersion
 * @ingroup pkcs8Functions
 * @par Prototype
 * @code
 * SEC_UINT32	PKCS8_checkVersion(
 * const PKCS8_PRIVKEY_S* pstPrivKeyInfo
 * )
 * @endcode
 *
 * @par Purpose
 * To check the version of the private key from private key info.
 *
 * @par Description
 * This function checks the version of the private key from private key info.
 *
 * @param[in] pstPrivKeyInfo Structure which holds the version of the private key [N/A]
 * @param[out] N/A N/A [N/A]
 *
 * @retval SEC_UINT32 Success if the version of the private key matches with PVT_KEY_VERSION [SEC_UINT32|N/A]
 * @retval SEC_UINT32 If it does not Match [SEC_ERR_INVALID_VERSION|N/A]
 * @retval SEC_UINT32 If the input is absent [SEC_ERR_INVALID_ARG|N/A]
 *
 * @par Dependency
 * pkcs8.h
 *
 * @par Note
 *
 * N/A
 *
 * @par Related Topics
 * N/A
 */
PSELINKDLL SEC_UINT32 PKCS8_checkVersion(const PKCS8_PRIVKEY_S *pstPrivKeyInfo);

/**
 * @defgroup PKCS8_encodePrivKey
 * @ingroup pkcs8Functions
 * @par Prototype
 * @code
 * SEC_UCHAR* PKCS8_encodePrivKey(PKCS8_PRIVKEY_S *pstPrivKey,
 * SEC_UINT32 *pulEncLen
 * )
 * @endcode
 *
 * @par Purpose
 * To encode the PKCS8_PRIVKEY_S structure.
 *
 * @par Description
 * This function is called to Encode the PKCS8_PRIVKEY_S structure.
 *
 * @param[in] pstPrivKey Holds the private key information i.e version,Algo Identifier and the encoded
 * private Key Value [N/A]
 * @param[out] pulEncLen Length of the encoded private key information [N/A]
 *
 * @retval SEC_UCHAR* On successful completion [Encoded PKCS8_PRIVKEY_S structure|N/A]
 * @retval SEC_UCHAR* In case of errors, any Of the Input is invalid\n
 * Malloc fails\n
 * Asn Encode fails\n
 * Buffer Copy Fails [SEC_NULL|N/A]
 *
 * @par Dependency
 * pkcs8.h
 *
 * @par Note
 *
 * N/A
 *
 * @par Related Topics
 * N/A
 */
PSELINKDLL SEC_UCHAR *PKCS8_encodePrivKey(PKCS8_PRIVKEY_S *pstPrivKeyInfo, SEC_UINT32 *pulEncPrivKeyLen);

/**
 * @defgroup PKCS8_encodeEncryptedPrivKey
 * @ingroup pkcs8Functions
 * @par Prototype
 * @code
 * SEC_UCHAR* PKCS8_encodeEncryptedPrivKey(PKCS8_ENCRYPTED_PRIVKEY_S* pstEncryptedPrivKey,
 * SEC_UINT32 *pulEncLen
 * )
 * @endcode
 *
 * @par Purpose
 * To encode the Encrypted PrivKeyInfo structure.
 *
 * @par Description
 * This function is called to Encode the Encrypted PrivKeyInfo structure.
 *
 * @param[in] pstEncryptedPrivKey Holds the encrypted private key information Algo Identifier and the
 * encoded PKCS8_PRIVKEY_S* value [N/A]
 * @param[out] pulEncLen Length of the encoded encrypted private key information [N/A]
 *
 * @retval SEC_UCHAR* On successful completion [Encoded encrypted private key|N/A]
 * @retval SEC_UCHAR* In case of errors, Any Of the Input is invalid
 * Malloc fail\n
 * Asn Encode fails\n
 * Buffer Copy Fails [SEC_NULL|N/A]
 *
 * @par Dependency
 * pkcs8.h
 *
 * @par Note
 *
 * N/A
 *
 * @par Related Topics
 * N/A
 */
PSELINKDLL SEC_UCHAR *PKCS8_encodeEncryptedPrivKey(PKCS8_ENCRYPTED_PRIVKEY_S *pstEncPrivKeyInfo,
    SEC_UINT32 *pulEncPrivKeyLen);

/**
 * @defgroup PKCS8_decodePrivKey
 * @ingroup pkcs8Functions
 * @par Prototype
 * @code
 * PKCS8_PRIVKEY_S* PKCS8_decodePrivKey(SEC_UCHAR* pucEncodedPrivKey,
 * SEC_UINT32 ulEncPrivtKeyLen,
 * SEC_UINT32* pulLenDecoded
 * )
 * @endcode
 *
 * @par Purpose
 * To decode the  PKCS8_PRIVKEY_S and returns the structure.
 *
 * @par Description
 * This functin decodes the  PKCS8_PRIVKEY_S and returns the structure.
 *
 * @param[in] pucEncodedPrivKey Encoded Value of the PKCS8_PRIVKEY_S [N/A]
 * @param[in] ulEncPrivtKeyLen Length of the encoded string [N/A]
 * @param[out] pulLenDecoded Length of the decoded Value [N/A]
 *
 * @retval PKCS8_PRIVKEY_S* Decoded Private Key Information or SEC_NULL in case of error [PKCS8_PRIVKEY_S*|N/A]
 * @retval PKCS8_PRIVKEY_S* In case of errors, Any Of the Input is invalid
 * Malloc fails\n
 * Asn decode fails [SEC_NULL|N/A]
 *
 * @par Dependency
 * pkcs8.h
 *
 * @par Note
 *
 * N/A
 *
 * @par Related Topics
 * N/A
 */
PSELINKDLL PKCS8_PRIVKEY_S *PKCS8_decodePrivKey(SEC_UCHAR *pucEncodedPvtKeyInfo, SEC_UINT32 ulEncPrivKeyInfoLen,
    SEC_UINT32 *pulLenDecoded);

/**
 * @defgroup PKCS8_decodeEncryptedPrivKey
 * @ingroup pkcs8Functions
 * @par Prototype
 * @code
 * PKCS8_ENCRYPTED_PRIVKEY_S* PKCS8_decodeEncryptedPrivKey(SEC_UCHAR* pucEncodedKey,
 * SEC_UINT32 ulEncEncryptedPrivKeyLen,
 * SEC_UINT32* pulLenDecoded
 * )
 * @endcode
 *
 * @par Purpose
 * To decode the PKCS8_ENCRYPTED_PRIVKEY_S and return the structure.
 *
 * @par Description
 * This function decodes the PKCS8_ENCRYPTED_PRIVKEY_S and returns the structure.
 *
 * @param[in] pucEncodedKey Encode value of the Private Key [N/A]
 * @param[in] ulEncEncryptedPrivKeyLen Length of the encoded value of the private key [N/A]
 * @param[out] pulLenDecoded Length of the decoded Value [N/A]
 *
 * @retval PKCS8_ENCRYPTED_PRIVKEY_S* On successful completion [Decoded Private Key Information|N/A]
 * @retval PKCS8_ENCRYPTED_PRIVKEY_S* In case of errors, Any Of the Input is invalid\n
 * Malloc fails\n
 * Asn decode fails [SEC_NULL|N/A]
 *
 * @par Dependency
 * pkcs8.h
 *
 * @par Note
 * PKCS8_ENCRYPTED_PRIVKEY_S decoded Encrypted Private Key Information.
 *
 * @par Related Topics
 * N/A
 */
PSELINKDLL PKCS8_ENCRYPTED_PRIVKEY_S *PKCS8_decodeEncryptedPrivKey(SEC_UCHAR *pucEncodedPvtKeyInfo,
    SEC_UINT32 ulEncPrivKeyInfoLen, SEC_UINT32 *pulLenDecoded);
/**
 * @defgroup PKCS8_encryptPrivKey
 * @ingroup pkcs8Functions
 * @par Prototype
 * @code
 * PKCS8_ENCRYPTED_PRIVKEY_S* PKCS8_encryptPrivKey(PKCS8_PRIVKEY_S* pstPrivKeyInfo,
 * SEC_UCHAR* pucPassword,
 * SEC_UINT32 ulPwdLen,
 * SEC_UINT32 ulPbeAlg,
 * SEC_UINT32 ulEncAlgID,
 * SEC_UINT32 ulKeyDerAlgID
 * );
 * @endcode
 *
 * @par Purpose
 * To encrypt the Private Key Info Structure using supplied Algorithm.
 *
 * @par Description
 * This function encrypts the Private Key Info Structure using supplied Algorithm.User can choose either PBES1 ALgID or
 * CID_PBES2.In case of PBES1 Alg ID last two parameters are optional.
 *
 * @param[in] pstPrivKeyInfo structure holds the private key information.To create PKCS8_PRIVKEY_S
 * call PKCS8_createPrivKey [N/A]
 * @param[in] pucPassword Password used for encryption [N/A]
 * @param[in] ulPwdLen Length of the password [N/A]
 * @param[in] ulPbeAlg Can be any value defined for PBES1 and also can be PKCS5_PBE2.If PBES1 then last two parameters
 * are optional and must be set to 0 i.e.ulEncAlgID=ulKeyDerAlgID=0. For PBES1 following are the AlgID\n
 * CID_PBE_MD5WITHDESCBC\n
 * CID_PBE_SHA1WITHDESCBC\n
 * CID_PBE_SHAWITH128BITRC4\n
 * CID_PBE_SHAWITH40BITRC4\n
 * .CID_PBE_SHAWITH40BIT_RC2CBC\n
 * .CID_PBE_SHAWITH128BIT_RC2CBC\n
 * CID_PBE_SHAWITH2KEY_TRIPLE_DESCBC\n
 * CID_PBE_SHAWITH3KEY_TRIPLE_DESCBC\n
 * CID_PBES2 [N/A]
 * @param[in] ulEncAlgID encryption Algorithm ID ALGID_DES_CBC and ALGID_DES_EDE3_CBC
 * and ALGID_AES128_CBC and ALGID_AES192_CBC and  ALGID_AES256_CBC[N/A]
 * @param[in] ulKeyDerAlgID Algorithm ID for key derivation ALGID_HMAC_SHA1 and ALGID_HMAC_MD5
 * and ALGID_HMAC_SHA224 and ALGID_HMAC_SHA256 and ALGID_HMAC_SHA384 and ALGID_HMAC_SHA512[N/A]
 * @param[out] N/A N/A [N/A]
 *
 * @retval PKCS8_ENCRYPTED_PRIVKEY_S* On successful completion,
 * need to be free by PKCS8_freeEncryptedPrivKey [Encrypted private Key Info|N/A]
 * @retval PKCS8_ENCRYPTED_PRIVKEY_S* In case of errors, Any Of the Input is Null\n
 * Malloc fail\n
 * Invalid CID Passed\n
 * Encode of the PKCS8_PRIVKEY_S fails\n
 * PKCS5 operation (gen PBES1 or genPBES2 or PKCS_encrypt fails) [SEC_NULL|N/A]
 *
 * @par Dependency
 * pkcs8.h
 *
 * @par Note
 * - PKCS8_ENCRYPTED_PRIVKEY_S decoded Encrypted Private Key Information.
 * The keys encrypted by this API are not backward compatible to decrypt
 * with old iPSI versions such as V1R1, V1R3, V1R5, V1R6.
 * - If application does not set seeds and entropy on frequent intervals, it can lead to vulnerable keys or random
 * value. It is recommended to set seeds and entropy on frequent intervals or update the version that migrates the
 * random implementation based on DRBG.
 * - Before calling this API, please make sure that DRBG is initialized for random number generation by calling
 * IPSI_CRYPT_rand_init. If application don't want to use DRBG then appplication should call
 * IPSI_CRYPT_enable_drbg(0) to disable the DRBG Random feature.
 * - DRBG is enabled by default.
 *
 * @par Related Topics
 * N/A
 *
 *
 */
PSELINKDLL PKCS8_ENCRYPTED_PRIVKEY_S *PKCS8_encryptPrivKey(PKCS8_PRIVKEY_S *pstPrivKeyInfo, SEC_UCHAR *pucPassword,
    SEC_UINT32 ulPwdLen, SEC_UINT32 ulPbeAlg, SEC_UINT32 ulEncAlgID, SEC_UINT32 ulKeyDerAlgID);

/**
 * @defgroup PKCS8_decryptPrivKey
 * @ingroup pkcs8Functions
 * @par Prototype
 * @code
 * SEC_UINT32	PKCS8_decryptPrivKey(const PKCS8_ENCRYPTED_PRIVKEY_S*	 pstEncryptedPrivKeyInfo,
 * SEC_UCHAR* pucPassword,
 * SEC_UINT32 ulPwdLen,
 * PKCS8_PRIVKEY_S** pstPrivKeyInfo
 * )
 * @endcode
 *
 * @par Purpose
 * To decrypt the encrypted private key PKCS8_ENCRYPTED_PRIVKEY_S.
 *
 * @par Description
 * This function is used to decrypt the encrypted private key  PKCS8_ENCRYPTED_PRIVKEY_S.
 *
 * @param[in] pstEncryptedPrivKeyInfo Strcture contains encrypted Priv Key Info.To Create PKCS8_ENCRYPTED_PRIVKEY_S
 * call PKCS8_encryptPrivKey API [N/A]
 * @param[in] pucPassword Used to decrypt the encrypted information [N/A]
 * @param[in] ulPwdLen Length of the password [N/A]
 * @param[out] pstPrivKeyInfo It Holds the private key information, need to be free by PKCS8_freePrivKey [N/A]
 *
 * @retval SEC_UINT32 In case of successfully decrypted the Priv Key info or Error Code Indicating the
 * reason for failure [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 In case any of the input is invalid or null [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 In case of failing to decode private Key after decryption [SEC_ERR_DECODE_FAILED|N/A]
 * @retval SEC_UINT32 In case of getting SEC_NULL when getting OID [SEC_ERR_NULL_PTR|N/A]
 * @retval SEC_UINT32 If the Algorithm is not supported in PKCS8 specification [SEC_ERR_INVALID_CID|N/A]
 * @retval SEC_UINT32 If the memory allocation fails [SEC_ERR_MALLOC_FAIL|N/A]
 * @retval SEC_UINT32 If the decrypt operation fails on encrypted Priv Key [SEC_ERR_DECRYPT_FAILED|N/A]
 *
 * @par Dependency
 * pkcs8.h
 *
 * @par Note
 * PKCS8_ENCRYPTED_PRIVKEY_S decoded Encrypted Private Key Information.
 *
 * @par Related Topics
 * N/A
 *
 *
 */
PSELINKDLL SEC_UINT32 PKCS8_decryptPrivKey(const PKCS8_ENCRYPTED_PRIVKEY_S *pstEncryptedPrivKeyInfo,
    SEC_UCHAR *pucPassword, SEC_UINT32 ulPwdLen, PKCS8_PRIVKEY_S **pstPrivKeyInfo);

/**
 * @defgroup PKCS8_addAttr
 * @ingroup pkcs8Functions
 * @par Prototype
 * @code
 * SEC_UINT32	PKCS8_addAttr(PKCS8_PRIVKEY_S* pstPrivKeyInfo,
 * SEC_ATTR_S* pstAttr
 * )
 * @endcode
 *
 * @par Purpose
 * To add the PKCS8 attribute to the private key structure.
 *
 * @par Description
 * This function adds the PKCS8 attribute to the private key structure.Currently no attribute is defined
 * for PKCS8 Private Key structure.So this API will be used only in future.
 *
 * @param[in] pstPrivKeyInfo Private Key Structure.A PKCS8 SEC_ATTR_S will be defined in future and
 * this API can then be used [N/A]
 * @param[in] pstAttr The PKCS8 Attribute structure which holds the value and type
 * of the attribute to be added to the given PKCS8 Private key.
 * this API can then be used [N/A]
 * @param[out] N/A N/A [N/A]
 *
 * @retval SEC_UINT32 For the current version it always returns SEC_SUCCESS [SEC_SUCCESS|N/A]
 *
 * @par Dependency
 * pkcs8.h
 *
 * @par Note
 * PKCS8_ENCRYPTED_PRIVKEY_S decoded Encrypted Private Key Information.
 *
 * @par Related Topics
 * N/A
 */
PSELINKDLL SEC_UINT32 PKCS8_addAttr(PKCS8_PRIVKEY_S *pstPrivKeyInfo, SEC_ATTR_S *pstAttr);

/**
 * @defgroup PKCS8_getAttrList
 * @ingroup pkcs8Functions
 * @par Prototype
 * @code
 * SEC_List_S* PKCS8_getAttrList(const PKCS8_PRIVKEY_S* pstPrivKeyInfo)
 * @endcode
 *
 * @par Purpose
 * To get the list of the attribute from the private key info structure.
 *
 * @par Description
 * This function returns the list of the attribute from the private key info structure.
 * This API is for future use when PKCS8 attribute will be defined.
 *
 * @param[in] pstPrivKeyInfo Contains information about the private Key [N/A]
 * @param[out] N/A N/A [N/A]
 *
 * @retval SEC_List_S* Pointer to list of the attribute defined for Private Key. currently no attribute
 * is defined so it will always be SEC_NULL [SEC_List_S*|N/A]
 * @retval SEC_List_S* If an error occurs [SEC_NULL|N/A]
 *
 * @par Dependency
 * pkcs8.h
 *
 * @par Note
 * PKCS8_ENCRYPTED_PRIVKEY_S decoded Encrypted Private Key Information.
 *
 * @par Related Topics
 * N/A
 */
PSELINKDLL SEC_List_S *PKCS8_getAttrList(const PKCS8_PRIVKEY_S *pstPrivKeyInfo);

/**
 * @defgroup PKCS8_freePrivKey
 * @ingroup pkcs8Functions
 * @par Prototype
 * @code
 * SEC_VOID PKCS8_freePrivKey(PKCS8_PRIVKEY_S* pstPrivKey)
 * @endcode
 *
 * @par Purpose
 * To free the allocated memory to PKCS8_PRIVKEY_S.
 *
 * @par Description
 * This function is used to free the allocated memory to PKCS8_PRIVKEY_S.
 *
 * @param[in] pstPrivKey Private Key Structure [N/A]
 * @param[out] N/A N/A [N/A]
 *
 * @retval SEC_VOID This does not return any value [N/A|N/A]
 *
 * @par Dependency
 * pkcs8.h
 *
 * @par Note
 * PKCS8_ENCRYPTED_PRIVKEY_S decoded Encrypted Private Key Information.
 *
 * @par Related Topics
 * N/A
 */
PSELINKDLL SEC_VOID PKCS8_freePrivKey(PKCS8_PRIVKEY_S *pstPrivKey);

/**
 * @defgroup PKCS8_freeEncryptedPrivKey
 * @ingroup pkcs8Functions
 * @par Prototype
 * @code
 * SEC_VOID PKCS8_freeEncryptedPrivKey(PKCS8_ENCRYPTED_PRIVKEY_S* pstEncryptedPrivKey)
 * @endcode
 *
 * @par Purpose
 * To free the allocated memory to PKCS8_ENCRYPTED_PRIVKEY_S structure.
 *
 * @par Description
 * This function is used to free the allocated memory to PKCS8_ENCRYPTED_PRIVKEY_S structure.
 *
 * @param[in] pstEncryptedPrivKey Encrypted Private Key Structure [N/A]
 * @param[out] N/A N/A [N/A]
 *
 * @retval SEC_VOID This does not return any value [N/A|N/A]
 *
 * @par Dependency
 * pkcs8.h
 *
 * @par Note
 * PKCS8_ENCRYPTED_PRIVKEY_S decoded Encrypted Private Key Information.
 *
 * @par Related Topics
 * N/A
 */
PSELINKDLL SEC_VOID PKCS8_freeEncryptedPrivKey(PKCS8_ENCRYPTED_PRIVKEY_S *pstEncryptedPrivKey);

#ifdef __cplusplus
}
#endif

#endif /* _IPSI_PKCS8_H */
