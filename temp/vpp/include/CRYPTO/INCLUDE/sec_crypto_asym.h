/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: Contains various crypto interface functions
 * Project Code: iPSI
 * Module Name: sec_crypto_asym.h
 * Create:  2020-01-27
 * Author: Kiran Kumar Reddy
 */
#ifndef SEC_CRYPTO_ASYM_H
#define SEC_CRYPTO_ASYM_H

#include "ipsi_crypto_buildconf.h"
#include "sec_util.h"
#include "sec_crypto_alg_id.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * NOTE for internal use: MAX_INT_DIGITS should not longer than (MAX_IPSI_BN_DIGITS * BN_DIGIT_LEN),
 * otherwise the actual max will be (MAX_IPSI_BN_DIGITS * BN_DIGIT_LEN).
 */
#define MAX_INT_DIGITS 516UL

/**
 * @defgroup IPSI_ECDH_PRIMITIVE_E
 * @ingroup sec_cryptoEnum
 * @par Prototype
 * @code
 * typedef enum
 * {
 * IPSI_EC_STANDARD_DH_PRIMITIVE,
 * IPSI_EC_COFACTOR_DH_PRIMITIVE
 * } IPSI_ECDH_PRIMITIVE_E
 *
 * @endcode
 *
 * @dataenum IPSI_EC_STANDARD_DH_PRIMITIVE=0 Identifies ecdh shared key generation without cofactor
 * @dataenum IPSI_EC_COFACTOR_DH_PRIMITIVE Identifies ecdh shared key generation with cofactor muliplication
 */
typedef enum {
    IPSI_EC_STANDARD_DH_PRIMITIVE,
    IPSI_EC_COFACTOR_DH_PRIMITIVE
} IPSI_ECDH_PRIMITIVE_E;

/* Global flag to denote ssx31B device is being used */
/* Class of cryptographic algorithms supported.
Used to check for a valid crypto algorithm */
/**
 * @defgroup SEC_ALG_CATEGORY_E
 * @ingroup sec_cryptoEnum
 * @par Prototype
 * @code
 * typedef enum
 * {
 * CRYPT_ALG,
 * CRYPT_SYM,
 * CRYPT_ASYM,
 * CRYPT_DIGEST,
 * CRYPT_SIGN
 * }SEC_ALG_CATEGORY_E;
 * @endcode
 *
 * @datastruct CRYPT_ALG Identifies any crypto algorithm,
 * sym, asym, hash or sign.
 * @datastruct CRYPT_SYM Identifies a symmetric algorithm.
 * @datastruct CRYPT_ASYM Identifies an asymmetric algorithm.
 * @datastruct CRYPT_DIGEST Identifies a hash algorithm.
 * @datastruct CRYPT_SIGN Identifies a sign algorithm.
 */
typedef enum {
    CRYPT_ALG,    /* identifies any crypto algorithm, sym, asym, hash or sign */
    CRYPT_SYM,    /* identifies a symmetric algorithm */
    CRYPT_ASYM,   /* identifies an asymmetric algorithm */
    CRYPT_DIGEST, /* identifies a hash algorithm */
    CRYPT_SIGN    /* identifies a sign algorithm */
} SEC_ALG_CATEGORY_E;

/* The crypto key struct for asymmetric algorithms */
/**
 * @defgroup SEC_PKEY_S
 * @ingroup sec_cryptoStructures
 * @par Prototype
 * @code
 * typedef struct stSEC_PKEY
 * {
 * SEC_INT32 type
 * SEC_VOID* pKey
 * SEC_INT references
 * }SEC_PKEY_S
 * @endcode
 *
 * @datastruct type Contains algorithm id -
 * do not manipulate fields directly.
 * @datastruct pKey Points to the key structure.
 * @datastruct references Reference count. Object
 * is freed only if reference drops to 0.
 */
typedef struct stSEC_PKEY {
    SEC_INT32 type;

    /* Contains algorithm id - don't manipulate fields directly */
    SEC_VOID *pKey;     /* Points to the key struct */
    SEC_INT references; /* Reference count, object is freed only if ref drops to 0 */
} SEC_PKEY_S;

/*
  The context that will be used for symmetric
  encrypt/decrypt/open/seal/hash/sign operations.
  For all operations, this context has to be initialized
  with a call to the appropriate Init function.
*/
typedef SEC_VOID *CRYPT_CTX;

/*
The context that will be used for sym encrypt/decrypt operations.
*/
typedef SEC_VOID *CRYPT_SYM_CTX;

/*
This type defines the context that will be used for SM2 key exchange protocol operation.
*/
typedef SEC_VOID *CRYPT_SM2_KEP_CTX;

/**
 * @defgroup sec_DRBG
 * This section contains sec_DRGB, Structures and Functions.
 */
/**
 * @defgroup sec_DRBG_Structures
 * @ingroup sec_DRBG
 * This section contains the sec_DRBG Structures.
 */
/**
 * @defgroup sec_DRBG_Functions
 * @ingroup sec_DRBG
 * This section contains the sec_DRBG Functions.
 */
/**
 * @defgroup DRBG_HLF DRBG High Level Functions
 * @ingroup sec_DRBG_Functions
 * This section contains high level DRBG functions
 */
/**
 * @defgroup sec_crypto
 * This section contains sec_crypto, Structures and Functions.
 */
/**
 * @defgroup sec_cryptoEnum
 * @ingroup sec_crypto
 * This section contains the sec_crypto Enum.
 */
/**
 * @defgroup sec_cryptoStructures
 * @ingroup sec_crypto
 * This section contains the sec_crypto Structures.
 */
/**
 * @defgroup sec_cryptoFunctions
 * @ingroup sec_crypto
 * This section contains the sec_crypto Functions.
 */
/**
 * @defgroup HLF High Level Functions
 * @ingroup sec_cryptoFunctions
 * This section contains high level crypto functions
 */
/**
 * @defgroup LLF Low Level Functions
 * @ingroup sec_cryptoFunctions
 * This section contains low level crypto functions
 */
/* The big integer representation */
/**
 * @defgroup SEC_BIGINT_S
 * @ingroup sec_cryptoStructures
 * @par Prototype
 * @code
 * typedef struct stBIGINT
 * {
 * SEC_UINT32 uiLength
 * SEC_INT8   aVal[MAX_INT_DIGITS]
 * }SEC_BIGINT_S
 * @endcode
 *
 * @datastruct uiLength Number of valid octets which are
 * stored below in aVal.
 * @datastruct aVal[MAX_INT_DIGITS] Holds the value of big
 * integer with MSB @ aVal[0].
 */
typedef struct stBIGINT {
    SEC_UINT32 uiLength;

    /* number of valid octets which are stored below in aVal */
    SEC_INT8 aVal[MAX_INT_DIGITS];

    /* holds the value of big integer with MSB @ aVal[0] */
} SEC_BIGINT_S;

/* Representation of a Elliptic Curve Point (x,y)
in compressed or normal form */
/**
 * @defgroup EC_POINT_S
 * @ingroup sec_cryptoStructures
 * @par Prototype
 * @code
 * typedef struct stEC_POINT
 * {
 * SEC_BIGINT_S stX;
 * SEC_BIGINT_S stY;
 * SEC_BOOL     bYCompress;
 * }EC_POINT_S;
 * @endcode
 *
 * @datastruct stX Variable represents big interger.
 * @datastruct stY Variable represents big interger.
 * @datastruct bYCompress If true, stY = {1, {1}} or {1, {0}}.
 */
typedef struct stEC_POINT {
    SEC_BIGINT_S stX;    /* variable represents big interger */
    SEC_BIGINT_S stY;    /* variable represents big interger */
    SEC_BOOL bYCompress; /* if true, stY = {1, {1}} or {1, {0}} */
} EC_POINT_S;

/*
  This type defines the structure which store Z and lenght of Z
*/
/**
 * @defgroup IPSI_SM2_Z_S
 * @ingroup sec_cryptoStructures
 * @par Prototype
 * @code
 * typedef struct stSM2Z
 * {
 * SEC_UCHAR *pucZ
 * SEC_UINT32 uiZLen
 * }IPSI_SM2_Z_S
 * @endcode
 *
 * @datastruct pucZ Hold the Z value of specific user
 * @datastruct uiZLen Length of Z value
 */
typedef struct stSM2Z {
    SEC_UCHAR *pucZ;
    SEC_UINT32 uiZLen;
} IPSI_SM2_Z_S;

/*
  This type defines the structure which lists the different items
  that is requried in the CRYPT_sm2KEPGenKey function.
*/
/**
 * @defgroup IPSI_SM2_KEP_GENKEY_OP_DATA_S
 * @ingroup sec_cryptoStructures
 * @par Prototype
 * @code
 * typedef struct stSM2KEPGenKeyOpData
 * {
 * CRYPT_SM2_KEP_CTX ctx;
 * IPSI_SM2_Z_S stZA;
 * IPSI_SM2_Z_S stZB;
 * SEC_PKEY_S *pRemotePubKey;
 * EC_POINT_S *pRemoteR;
 * }IPSI_SM2_KEP_GENKEY_OP_DATA_S;
 * @endcode
 *
 * @datastruct ctx SM2 key exchange protocol context
 * @datastruct pstZA User identifier ZA as per standard.
 * Here ZA is the identifier of the peer who initiates the key exchange protocol
 * @datastruct pstZB User identifier ZB as per standard.
 * Here ZB is the identifier of the peer who receives EC point R and
 * public key first during the key exchange protocol.
 * @datastruct pRemotePubKey Contains the EC public key from the remote side
 * @datastruct pRemoteR Contains the EC point R received from the remote side
 */
typedef struct stSM2KEPGenKeyOpData {
    CRYPT_SM2_KEP_CTX ctx;
    IPSI_SM2_Z_S stZA;
    IPSI_SM2_Z_S stZB;
    SEC_PKEY_S *pRemotePubKey;
    EC_POINT_S *pRemoteR;
} IPSI_SM2_KEP_GENKEY_OP_DATA_S;

/*
  Enum to identify the hash operation to be considered during SM2 KEP operation.
*/
/**
 * @defgroup IPSI_SM2_KEP_HASHTYPE_E
 * @ingroup sec_cryptoEnum
 * @par Prototype
 * @code
 * typedef enum
 * {
 * IPSI_SM2_HASH_SB,
 * IPSI_SM2_HASH_S1,
 * IPSI_SM2_HASH_SA,
 * IPSI_SM2_HASH_S2
 * } IPSI_SM2_KEP_HASHTYPE_E;
 *
 * @endcode
 *
 * @dataenum IPSI_SM2_HASH_SB Identifies hash type Sb
 * @dataenum IPSI_SM2_HASH_S1 Identifies hash type S1
 * @dataenum IPSI_SM2_HASH_SA Identifies hash type Sa
 * @dataenum IPSI_SM2_HASH_S2 Identifies hash type S2
 */
typedef enum {
    IPSI_SM2_HASH_SB,
    IPSI_SM2_HASH_S1,
    IPSI_SM2_HASH_SA,
    IPSI_SM2_HASH_S2
} IPSI_SM2_KEP_HASHTYPE_E;

/* Parameter for a prime field - a prime number */
typedef struct stBIGINT EC_PRIME_S;

/* Trinomial with MSB & LSB set. k is power of 2^k */
/**
 * @defgroup EC_BASIS_TRI_S
 * @ingroup sec_cryptoStructures
 * @par Prototype
 * @code
 * typedef struct stEC_BASIS_TRI
 * {
 * SEC_UINT k;
 * }EC_BASIS_TRI_S
 * @endcode
 *
 * @datastruct k k is power of 2.
 */
typedef struct stEC_BASIS_TRI {
    SEC_UINT k; /* k is power of 2 */
} EC_BASIS_TRI_S;

/* Pentanomial with MSB & LSB set. k1, k2, k3 represents 2^k1+2^k2+2^k3 */
/**
 * @defgroup EC_BASIS_PENT_S
 * @ingroup sec_cryptoStructures
 * @par Prototype
 * @code
 * typedef struct stEC_BASIS_PENT
 * {
 * SEC_UINT k1;
 * SEC_UINT k2;
 * SEC_UINT k3;
 * }EC_BASIS_PENT_S;
 * @endcode
 *
 * @datastruct k1 k1 is power of 2.
 * @datastruct k2 k2 is power of 2.
 * @datastruct k3 k3 is power of 2.
 */
typedef struct stEC_BASIS_PENT {
    SEC_UINT k1; /* k1 is power of 2 */
    SEC_UINT k2; /* k2 is power of 2 */
    SEC_UINT k3; /* k3 is power of 2 */
} EC_BASIS_PENT_S;

/* Parameter for a Binary field - can be a trinomial or pentanomial */
/**
 * @defgroup EC_BINARY_S
 * @ingroup sec_cryptoStructures
 * @par Prototype
 * @code
 * typedef struct stEC_BINARY
 * {
 * SEC_UINT32 degree;
 * SEC_UINT32 basisType;
 * union
 * {
 * EC_BASIS_TRI_S  stTri;
 * EC_BASIS_PENT_S stPenta;
 * }Basis;
 * }EC_BINARY_S;
 * @endcode
 *
 * @datastruct degree Degree of the polynomial.
 * @datastruct basisType Determines whether Basis is trinomial or pentanomial.
 * @datastruct stTri Trinomial with MSB & LSB set.
 * @datastruct stPenta Pentanomial with MSB & LSB set.
 * @datastruct Basis Holds the polynomial's non-zero terms.
 */
typedef struct stEC_BINARY {
    SEC_UINT32 degree; /* degree of the polynomial */
    SEC_UINT32 basisType;

    /* determines whether Basis is trinomial or pentanomial */
    union {
        EC_BASIS_TRI_S stTri;    /* Trinomial with MSB & LSB set */
        EC_BASIS_PENT_S stPenta; /* Pentanomial with MSB & LSB set */
    } Basis;

    /* holds the polynomial's non-zero terms */
} EC_BINARY_S;

/* parameters for a Ellitic curve algorithm */
/**
* @defgroup EC_PARA_S
* @ingroup sec_cryptoStructures
* @par Prototype
* @code
* typedef struct stEC_PARA
* {
*  SEC_UINT32 fieldType;
*  SEC_UINT32 paramId;
*  union
*      {
*       EC_PRIME_S  stPrime;
*       EC_BINARY_S stBinary;
*      }fieldParam;
*  SEC_BIGINT_S stCoefA;
*  SEC_BIGINT_S stCoefB;
*  EC_POINT_S   stBase;
*  SEC_BIGINT_S stOrder;
*  SEC_BIGINT_S* pCoFactor;
* }EC_PARA_S;
* @endcode
*
* @datastruct fieldType Determines whether field is prime or binary.
* @datastruct paramId Contains the curve id if this is a set of
parameters from X.9-62.
* @datastruct stPrime Parameter for a prime field - a prime number.
* @datastruct stBinary Parameter for a binary field - can be a
trinomial or pentanomial.
* @datastruct fieldParam Holds the polynomial or prime
describing the field.
* @datastruct stCoefA Coefficient 'a' of the curve equation.
* @datastruct stCoefB Coefficient 'b' of the curve equation.
* @datastruct stBase The base point of the Curve.
* @datastruct stOrder The order of the base point.
* @datastruct pCoFactor Optional field, The integer h = #E(Fq)/n,
can be left NULL.
*/
typedef struct stEC_PARA {
    SEC_UINT32 fieldType;

    /* determines whether field is prime or binary */
    SEC_UINT32 paramId;

    /* contains the curve id if this is a set of parameters from X.9-62 */
    union {
        EC_PRIME_S stPrime;   /* parameter for a prime field - a prime number */
        EC_BINARY_S stBinary; /* parameter for a binary field - can be a trinomial or pentanomial */
    } fieldParam;             /* Holds the polynomial or prime describing the field */
    SEC_BIGINT_S stCoefA;     /* Coefficient 'a' of the curve equation */
    SEC_BIGINT_S stCoefB;     /* Coefficient 'b' of the curve equation */
    EC_POINT_S stBase;        /* The base point of the Curve */
    SEC_BIGINT_S stOrder;     /* The order of the base point */
    SEC_BIGINT_S *pCoFactor;  /* Optional field, The integer h = #E(Fq)/n, can be left NULL */
} EC_PARA_S;

/**
 * @defgroup IPSI_SYM_CIPHER_DIRECTION_E
 * @ingroup sec_cryptoEnum
 * @par Prototype
 * @code
 * typedef enum enSymCipherDirection
 * {
 * IPSI_SYM_CIPHER_ENCRYPT,
 * IPSI_SYM_CIPHER_DECRYPT
 * } IPSI_SYM_CIPHER_DIRECTION_E
 * @endcode
 *
 * @dataenum IPSI_SYM_CIPHER_ENCRYPT Identifies that data provided is for encryption.
 * @dataenum IPSI_SYM_CIPHER_DECRYPT Identifies that data provided is for decryption.
 */
typedef enum enSymCipherDirection {
    IPSI_SYM_CIPHER_ENCRYPT,
    IPSI_SYM_CIPHER_DECRYPT
} IPSI_SYM_CIPHER_DIRECTION_E;

/**
 * @defgroup IPSI_AEAD_SETUP_DATA_S
 * @ingroup sec_cryptoStructures
 * @par Prototype
 * @code
 * typedef struct stAEADSessionSetUpData
 * {
 * SEC_UINT32 uiAEADAlgID
 * IPSI_SYM_CIPHER_DIRECTION_E uiAEADDirection
 * const SEC_UCHAR *pucKey
 * SEC_UINT32 uiKeyLen
 * SEC_VOID *pCipherSetupData
 * }IPSI_AEAD_SETUP_DATA_S
 * @endcode
 *
 * @datastruct uiAEADAlgID AEAD algorithm ID. Supported algorithms are ALGID_AES128_GCM,ALGID_AES192_GCM,
 * ALGID_AES256_GCM,ALGID_AES128_CCM, AGID_AES192_CCM & ALGID_AES256_CCM.
 * @datastruct uiAEADDirection Identifies the cipher direction whether encryption or decryption.
 * Supported values are IPSI_SYM_CIPHER_ENCRYPT and IPSI_SYM_CIPHER_DECRYPT.
 * @datastruct pucKey Pointer to the key structure used in AEAD operations.
 * @datastruct uiKeyLen Key length.
 * @datastruct pCipherSetupData No use for now. Added for future extensability.
 *
 */
typedef struct stAEADSessionSetUpData {
    SEC_UINT32 uiAEADAlgID;
    IPSI_SYM_CIPHER_DIRECTION_E uiAEADDirection;
    const SEC_UCHAR *pucKey;
    SEC_UINT32 uiKeyLen;
    SEC_VOID *pCipherSetupData;
} IPSI_AEAD_SETUP_DATA_S;

/**
 * @defgroup IPSI_SYM_SETUP_DATA_S
 * @ingroup sec_cryptoStructures
 * @par Prototype
 * @code
 * typedef struct stSymSessionSetUpData
 * {
 * SEC_UINT32 uiSymAlgID
 * IPSI_SYM_CIPHER_DIRECTION_E uiSymDirection
 * SEC_UCHAR *pucKey
 * SEC_UINT32 uiKeyLen
 * SEC_VOID *pCipherSetupData
 * }IPSI_SYM_SETUP_DATA_S
 * @endcode
 *
 * @datastruct uiSymAlgID symmetric algorithm ID.
 * Currently supported algorithm is only AES CTR (ALGID_AES128_CTR, ALGID_AES192_CTR & ALGID_AES256_CTR).
 * @datastruct uiSymDirection Identifies the cipher direction whether encryption or decryption.
 * Supported values are IPSI_SYM_ENCRYPT and IPSI_SYM_DECRYPT.
 * @datastruct pucKey Pointer to the key structure used in AEAD operations.
 * @datastruct uiKeyLen Key length.
 * @datastruct pCipherSetupData No use for now. Added for future extensability.
 *
 */
typedef struct stSymSessionSetUpData {
    SEC_UINT32 uiSymAlgID;
    IPSI_SYM_CIPHER_DIRECTION_E uiSymDirection;
    SEC_UCHAR *pucKey;
    SEC_UINT32 uiKeyLen;
    SEC_VOID *pCipherSetupData;
} IPSI_SYM_SETUP_DATA_S;

/* Enum to indicate whether data is complete or partial. User to decide to output tag or not. */
/**
 * @defgroup IPSI_SYM_DATATYPE_E
 * @ingroup sec_cryptoEnum
 * @par Prototype
 * @code
 * typedef enum enAeadDataType
 * {
 * IPSI_SYM_DATA_TYPE_FULL,
 * IPSI_SYM_DATA_TYPE_PARTIAL,
 * IPSI_SYM_DATA_TYPE_LAST_PARTIAL
 * }IPSI_SYM_DATATYPE_E;
 * @endcode
 *
 * @dataenum IPSI_SYM_DATA_TYPE_FULL Identifies that data provided to encryption/decryption operation is FULL and
 * in case of AEAD opertions, provides tag result in the same operation.
 * @dataenum IPSI_SYM_DATA_TYPE_PARTIAL Identifies that data provided to encryption/decryption operation is partial
 * and in case of AEAD operations, does not provide the tag result.
 * @dataenum IPSI_SYM_DATA_TYPE_LAST_PARTIAL Identifies that call to encryption/decryption operation is final and
 * in case of AEAD operations, provides the tag result.
 */
typedef enum enSymDataType {
    IPSI_SYM_DATA_TYPE_FULL,        // return hash result immediately
    IPSI_SYM_DATA_TYPE_PARTIAL,     // do not return hash result, and maintain the hash state internal
    IPSI_SYM_DATA_TYPE_LAST_PARTIAL // the call to aeadOp contains the final data and therefore return the hash
} IPSI_SYM_DATATYPE_E;

/**
 * @defgroup IPSI_SYM_OP_DATA_S
 * @ingroup sec_cryptoStructures
 * @par Prototype
 * @code
 * typedef struct stSymOpData
 * {
 * CRYPT_SYM_CTX ctx
 * IPSI_SYM_DATATYPE_E enDataType
 * SEC_UCHAR *pucIV
 * SEC_UINT32 uiIvLen
 * SEC_VOID *pCipherOpData
 * }IPSI_SYM_OP_DATA_S
 * @endcode
 *
 * @datastruct ctx Pointer to the symmetric operation session context.
 * @datastruct enDataType Identifies the data type whether FULL or PARTIAL.
 * @datastruct pucIV Pointer to the Initialization Vector used in symmetric encryption/decryption operations.
 * @datastruct uiIvLen Initialization Vector length.
 * @datastruct pCipherOpData No use for now. Added for future extensability.
 *
 */
typedef struct stSymOpData {
    CRYPT_SYM_CTX ctx;
    IPSI_SYM_DATATYPE_E enDataType;
    SEC_UCHAR *pucIV;
    SEC_UINT32 uiIvLen;
    SEC_VOID *pCipherOpData;
} IPSI_SYM_OP_DATA_S;

/*
    Func Name:  CRYPT_signInit
*/
/**
* @defgroup CRYPT_signInit
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_signInit(
* CRYPT_CTX*  pCtx,
* SEC_PKEY_S* pKey,
* SEC_UINT32  ulAlgType)
* @endcode
*
* @par Purpose
* This is used to initialize the context for a sign operation.
*
* @par Description
* CRYPT_signInit function initializes the context for a sign
operation. This function must be called before any Update
* or Final operation. This method of signing is used when
data is present in multiple buffers or not available all at
* once. The key must be a private key or a key pair.
*
* @param[in] ulAlgType The algorithm id for digest [N/A]
* @param[in] pKey The private key for signing [N/A]
* @param[out] pCtx The context which will be filled [N/A]
*
* @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 Invalid Arguments [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 Invalid Key [SEC_CRYPT_ERR_INVALID_PKEY|N/A]
* @retval SEC_UINT32 Unsupported Key [SEC_CRYPT_ERR_UNDEFINED_KEY_OPERATION|N/A]
* @retval SEC_UINT32 Memory allocation failure [SEC_ERR_MALLOC_FAIL|N/A]
* @retval SEC_UINT32 Library not initialized [SEC_ERR_INITLIB|N/A]
* @retval SEC_UINT32 Incorrect Algorithm ID [SEC_ERR_INVALID_ALGID|N/A]
* @retval SEC_UINT32 On Failure [SEC_ERR|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* - SM2 Signing is not supported by this API
* - API supports signature generation with following unsafe algorithms:
*   - ALGID_MD5 (CID_MD5)
*   - ALGID_SHA1(CID_SHA1) \n
*   As these algorithms are unsafe, if application are using these algorithms, application must take care of any
*   security issues that may happen due to usage of these algorithms.
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_signInit(CRYPT_CTX *pCtx, SEC_PKEY_S *pKey, SEC_UINT32 ulAlgType);

/*
    Func Name:  CRYPT_signUpdate
*/
/**
* @defgroup CRYPT_signUpdate
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_signUpdate(
* CRYPT_CTX  ctx,
* const SEC_UCHAR* pucData,
* SEC_UINT32 ulDataLen)
* @endcode
*
* @par Purpose
* This is used to perform sign operation on chunks of data.
*
* @par Description
* CRYPT_signUpdate function performs sign operation on chunks
of data. This method of signing is used when data is
* present in multiple buffers or not available all at once.
A signInit must have been called before calling this function.
*
* @param[in] ctx The context for signing [N/A]
* @param[in] pucData The input data [N/A]
* @param[in] ulDataLen The input data length [N/A]
*
* @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 Invalid Arguments [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 Incorrect context [SEC_CRYPT_ERR_INVALID_CTX|N/A]
* @retval SEC_UINT32 On Failure [SEC_ERR|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* - SM2 Signing is not supported by this API
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_signUpdate(CRYPT_CTX ctx, const SEC_UCHAR *pucData, SEC_UINT32 ulDataLen);

/*
    Func Name:  CRYPT_signFinal
*/
/**
* @defgroup CRYPT_signFinal
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_signFinal(
* const CRYPT_CTX*  pCtx,
* SEC_UCHAR*  pucSig,
* SEC_UINT32* pulSigLen)
* @endcode
*
* @par Purpose
* This is used to complete sign operation on remaining data,
performs sign. Called at the end of sign operation.
*
* @par Description
* CRYPT_signFinal function completes sign operation on remaining
data, performs sign. Called at the end of sign operation.
* A signInit must have been called before calling this function.
This function calculates the signature and frees the
* context. The memory for the signature must have been already
allocated.
*
* @param[in] pCtx The context for sign [N/A]
* @param[out] pucSig The signed hash [N/A]
* @param[in/out] pulSigLen The Sign length [N/A]
*
* @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 Invalid Arguments [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 Incorrect context [SEC_CRYPT_ERR_INVALID_CTX|N/A]
* @retval SEC_UINT32 Memory allocation failure [SEC_ERR_MALLOC_FAIL|N/A]
* @retval SEC_UINT32 If operation cannot be performed on key
[SEC_CRYPT_ERR_UNDEFINED_KEY_OPERATION|N/A]
* @retval SEC_UINT32 Key size not supported [SEC_ERR_INVALID_KEY_LEN|N/A]
* @retval SEC_UINT32 Library not initialized [SEC_ERR_INITLIB|N/A]
* @retval SEC_UINT32 Incorrect Algorithm ID [SEC_ERR_INVALID_ALGID|N/A]
* @retval SEC_UINT32 On Failure [SEC_ERR|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* - pucSign buffer size should be equal to or greater than the key size in bytes.
*    CRYPT_PKEY_size can be used to get the key size.
*
* - SM2 Signing is not supported by this API
* - If application does not set seeds and entropy on frequent intervals, it can lead to vulnerable keys or random
*   value. It is recommended to set seeds and entropy on frequent intervals or update the version that migrates the
*   random implementation based on DRBG.
* - Before calling this API, please make sure that DRBG is initialized for random number generation by calling
*   IPSI_CRYPT_rand_init. If application don't want to use DRBG then appplication should call
*   IPSI_CRYPT_enable_drbg(0) to disable the DRBG Random feature.
* - DRBG is enabled by default.
* - The safe API corresponding to this is CRYPT_signFinal_sf
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_signFinal(const CRYPT_CTX *pCtx, SEC_UCHAR *pucSig, SEC_UINT32 *pulSigLen);

/*
    Func Name:  CRYPT_sign
*/
/**
 * @defgroup CRYPT_sign
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_sign(
 * SEC_UINT32  ulAlg,
 * SEC_PKEY_S* pKey,
 * const SEC_UCHAR*  pucData,
 * SEC_UINT32  pulDataLen,
 * SEC_UCHAR*  pucSign,
 * SEC_UINT32* pulSignLen);
 * @endcode
 *
 * @par Purpose
 * This is used to compute sign of a given data block.
 *
 * @par Description
 * CRYPT_sign function computes sign of a given data block.
 * Calls init, update, and final. This function is used when data
 * is present all at once. There is no need of calling Init,
 * Update, Final and signature can be calculated in one go.
 * Context is not needed here.
 *
 * @param[in] ulAlg Digest algorithm [N/A]
 * @param[in] pKey The private key [N/A]
 * @param[in] pucData The data [N/A]
 * @param[in] pulDataLen The data length [N/A]
 * @param[out] pucSign The signature [N/A]
 * @param[in/out] pulSignLen The sign length [N/A]
 *
 * @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 Invalid Arguments [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 Memory allocation failure [SEC_ERR_MALLOC_FAIL|N/A]
 * @retval SEC_UINT32 If operation cannot be performed on key
 * [SEC_CRYPT_ERR_UNDEFINED_KEY_OPERATION|N/A]
 * @retval SEC_UINT32 Invalid Key [SEC_CRYPT_ERR_INVALID_PKEY|N/A]
 * @retval SEC_UINT32 Key size not supported [SEC_ERR_INVALID_KEY_LEN|N/A]
 * @retval SEC_UINT32 Library not initialized [SEC_ERR_INITLIB|N/A]
 * @retval SEC_UINT32 Incorrect Algorithm ID [SEC_ERR_INVALID_ALGID|N/A]
 * @retval SEC_UINT32 On Failure [SEC_ERR|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * - pucSign buffer size should be equal to or greater than the key size in bytes.
 * CRYPT_PKEY_size can be used to get the key size.
 *
 * - If the signing is done using the library version V3R3 or later, using the
 * hash algorithm SHA384 and SHA512, then verification done by
 * the older version (version before V3R3) will fail. The problem in hash
 * info of SHA384 and SHA512 in the older version is fixed in V3R3.
 *
 * - SM2 Signing is not supported by this API
 * - If application does not set seeds and entropy on frequent intervals, it can lead to vulnerable keys or random
 * value. It is recommended to set seeds and entropy on frequent intervals or update the version that migrates the
 * random implementation based on DRBG.
 *
 * - The safe API corresponding to this is CRYPT_sign_sf
 * - API supports signature generation with following unsafe algorithms:
 * - ALGID_MD5 (CID_MD5)
 * - ALGID_SHA1(CID_SHA1) \n
 * As these algorithms are unsafe, if application are using these algorithms, application must take care of any
 * security issues that may happen due to usage of these algorithms.
 * - Before calling this API, please make sure that DRBG is initialized for random number generation by calling
 * IPSI_CRYPT_rand_init. If application don't want to use DRBG then appplication should call
 * IPSI_CRYPT_enable_drbg(0) to disable the DRBG Random feature.
 * - DRBG is enabled by default.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_sign(SEC_UINT32 ulAlg, SEC_PKEY_S *pKey, const SEC_UCHAR *pucData, SEC_UINT32 pulDataLen,
    SEC_UCHAR *pucSign, SEC_UINT32 *pulSignLen);

/**
 * @defgroup CRYPT_signDec
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_signDec(
 * const SEC_UCHAR *pucSgn,
 * SEC_UINT32 pulSgnLen,
 * SEC_UCHAR *pulSgnR,
 * SEC_UINT32 *pulSgnRLen,
 * SEC_UCHAR *pulSgnS,
 * SEC_UINT32 *pulSgnSLen);
 * @endcode
 *
 * @par Purpose
 * This is used to Decode the Encoded DSA/ECDSA/SM2DSA Signature
 * buffer into r and s values.
 * @par Description
 * CRYPT_signDec function Decode the Encoded DSA/ECDSA/SM2DSA Signature buffer into
 * corresponding r and s value
 *
 * @param[in] pucSgn The signature [N/A]
 * @param[in] pulSgnLen The sign length [N/A]
 * @param[out] pulSgnR The r value [N/A]
 * @param[in/out] pulSgnRLen The length of r value [N/A]
 * @param[out] pulSgnS The s value [N/A]
 * @param[out] pulSgnSLen The length of s value [N/A]
 *
 * @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 Invalid Arguments [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 On Failure [SEC_ERR|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * - This function decode the signature and provide R and S parameter. While generation of signature, if zero is
 * appended before R and S value, decoded value R and S provided by this function will not remove the leading zero.
 * - It is recommended to use CRYPT_signDec_ex.
 *
 * - The safe API corresponding to this is CRYPT_signDec_sf
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_signDec(const SEC_UCHAR *pucSgn, SEC_UINT32 pulSgnLen, SEC_UCHAR *pulSgnR,
    SEC_UINT32 *pulSgnRLen, SEC_UCHAR *pulSgnS, SEC_UINT32 *pulSgnSLen);

/**
 * @defgroup CRYPT_signDec_ex
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_signDec_ex(
 * const SEC_UCHAR *pucSgn,
 * SEC_UINT32 pulSgnLen,
 * SEC_UCHAR *pulSgnR,
 * SEC_UINT32 *pulSgnRLen,
 * SEC_UCHAR *pulSgnS,
 * SEC_UINT32 *pulSgnSLen);
 * @endcode
 *
 * @par Purpose
 * This is used to Decode the Encoded DSA/ECDSA/SM2DSA Signature
 * buffer into r and s values.
 * @par Description
 * CRYPT_signDec_ex function Decode the Encoded DSA/ECDSA/SM2DSA Signature buffer into
 * corresponding r and s value
 *
 * @param[in] pucSgn The signature [N/A]
 * @param[in] pulSgnLen The sign length [N/A]
 * @param[out] pulSgnR The r value [N/A]
 * @param[in/out] pulSgnRLen The length of r value [N/A]
 * @param[out] pulSgnS The s value [N/A]
 * @param[out] pulSgnSLen The length of s value [N/A]
 *
 * @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 Invalid Arguments [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 On Failure [SEC_ERR|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * - This function decode the signature and provide R and S parameter. While generation of signature, if zero is
 * appended before R and S value, decoded value R and S provided by this function will remove the leading zero.
 *
 * - The safe API corresponding to this is CRYPT_signDec_sf
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_signDec_ex(const SEC_UCHAR *pucSgn, SEC_UINT32 pulSgnLen, SEC_UCHAR *pulSgnR,
    SEC_UINT32 *pulSgnRLen, SEC_UCHAR *pulSgnS, SEC_UINT32 *pulSgnSLen);

/**
 * @defgroup CRYPT_signEnc
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_signEnc(
 * SEC_UCHAR *pucSgn,
 * SEC_UINT32 *pulSgnLen,
 * const SEC_UCHAR *pulSgnR,
 * SEC_UINT32 pulSgnRLen,
 * const SEC_UCHAR *pulSgnS,
 * SEC_UINT32 pulSgnSLen);
 * @endcode
 *
 * @par Purpose
 * This is used to Encode DSA/ECDSA/SM2DSA r and s signature values.
 *
 * @par Description
 * CRYPT_signEnc function Encodes the DSA/ECDSA/SM2DSA r and s signature values
 * into Signature buffer
 *
 * @param[out] pucSgn The signature [N/A]
 * @param[in/out] pulSgnLen The sign length [N/A]
 * @param[in] pulSgnR The r value [N/A]
 * @param[in] pulSgnRLen The length of r value [N/A]
 * @param[in] pulSgnS The s value [N/A]
 * @param[in] pulSgnSLen The length of s value [N/A]
 *
 * @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 Invalid Arguments [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 On Memory allocation failure [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 On Failure [SEC_ERR|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * - While encoding of R and S, if first character of R or S is more than 127 , zero is not appended before encoding R
 * or S value.
 * - It is recommended to use CRYPT_signEnc_ex.
 * - The safe API corresponding to this is CRYPT_signEnc_sf
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_signEnc(SEC_UCHAR *pucSgn, SEC_UINT32 *pulSgnLen, const SEC_UCHAR *pulSgnR,
    SEC_UINT32 pulSgnRLen, const SEC_UCHAR *pulSgnS, SEC_UINT32 pulSgnSLen);

/**
 * @defgroup CRYPT_signEnc_ex
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_signEnc_ex(
 * SEC_UCHAR *pucSgn,
 * SEC_UINT32 *pulSgnLen,
 * const SEC_UCHAR *pulSgnR,
 * SEC_UINT32 pulSgnRLen,
 * const SEC_UCHAR *pulSgnS,
 * SEC_UINT32 pulSgnSLen);
 * @endcode
 *
 * @par Purpose
 * This is used to Encode DSA/ECDSA/SM2DSA r and s signature values.
 *
 * @par Description
 * CRYPT_signEnc_ex function Encodes the DSA/ECDSA/SM2DSA r and s signature values
 * into Signature buffer
 *
 * @param[out] pucSgn The signature [N/A]
 * @param[in/out] pulSgnLen The sign length [N/A]
 * @param[in] pulSgnR The r value [N/A]
 * @param[in] pulSgnRLen The length of r value [N/A]
 * @param[in] pulSgnS The s value [N/A]
 * @param[in] pulSgnSLen The length of s value [N/A]
 *
 * @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 Invalid Arguments [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 On Memory allocation failure [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 On Failure [SEC_ERR|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * -While encoding of R and S, if first character of R or S is more than 127 , zero is appended before encoding R or
 * S value.
 * - The safe API corresponding to this is CRYPT_signEnc_sf
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_signEnc_ex(SEC_UCHAR *pucSgn, SEC_UINT32 *pulSgnLen, const SEC_UCHAR *pulSgnR,
    SEC_UINT32 pulSgnRLen, const SEC_UCHAR *pulSgnS, SEC_UINT32 pulSgnSLen);

/*
    Func Name:  CRYPT_verifyInit
*/
/**
* @defgroup CRYPT_verifyInit
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_verifyInit(
* CRYPT_CTX*  pCtx,
* SEC_PKEY_S* pKey,
* SEC_UINT32  ulAlg)
* @endcode
*
* @par Purpose
* This is used to initialize the context for a sign verification operation.
*
* @par Description
* CRYPT_verifyInit function initializes the context for a
sign verification operation. This function must be called before
* any Update or Final operations. This method of verification
is used when data is present in multiple buffers or not
* available all at once. The key must be a public key or a key pair.
*
* @param[in] ulAlg The algorithm id for digest [N/A]
* @param[in] pKey The public key [N/A]
* @param[out] pCtx The context which will be filled [N/A]
*
* @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 Invalid Arguments [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 Invalid Key [SEC_CRYPT_ERR_INVALID_PKEY|N/A]
* @retval SEC_UINT32 Memory allocation failure [SEC_ERR_MALLOC_FAIL|N/A]
* @retval SEC_UINT32 Library not initialized [SEC_ERR_INITLIB|N/A]
* @retval SEC_UINT32 Incorrect Algorithm ID [SEC_ERR_INVALID_ALGID|N/A]
* @retval SEC_UINT32 On Failure [SEC_ERR|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* - SM2 Verification is not supported by this API
* - API supports signature verification with following unsafe algorithms:
*   - ALGID_MD5 (CID_MD5)
*   - ALGID_SHA1(CID_SHA1) \n
*   As these algorithms are unsafe, if application are using these algorithms, application must take care of any
*   security issues that may happen due to usage of these algorithms.
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_verifyInit(CRYPT_CTX *pCtx, SEC_PKEY_S *pKey, SEC_UINT32 ulAlg);

/*
    Func Name:  CRYPT_verifyUpdate
*/
/**
* @defgroup CRYPT_verifyUpdate
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_verifyUpdate(
* CRYPT_CTX ctx,
* const SEC_UCHAR* pucData,
* SEC_UINT32 pulDataLen)
* @endcode
*
* @par Purpose
* This is used to perform sign verification operation on chunks of data.
*
* @par Description
* CRYPT_verifyUpdate function performs sign verification
operation on chunks of data. This method of verification is
* used when data is present in multiple buffers or not
available all at once. A verifyInit should have been called
* before calling this function.
*
* @param[in] ctx The context for verification [N/A]
* @param[in] pucData The input data [N/A]
* @param[in] pulDataLen The input data length [N/A]
*
* @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 Invalid Arguments [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 Incorrect context [SEC_CRYPT_ERR_INVALID_CTX|N/A]
* @retval SEC_UINT32 On Failure [SEC_ERR|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* - SM2 Verification is not supported by this API
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_verifyUpdate(CRYPT_CTX ctx, const SEC_UCHAR *pucData, SEC_UINT32 pulDataLen);

/*
    Func Name:  CRYPT_verifyFinal
*/
/**
* @defgroup CRYPT_verifyFinal
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_verifyFinal(
* CRYPT_CTX* pCtx,
* const SEC_UCHAR* pucSign,
* SEC_UINT32 pulSignLen)
* @endcode
*
* @par Purpose
* This is used to complete sign verification operation on
data and is called at the end of verify operation.
*
* @par Description
* CRYPT_verifyFinal function completes sign verification
operation on data and is called at the end of verify operation.
* A verifyInit must have been called before calling this
function. This function checks the signature and frees the
* context.
*
* @param[in] pCtx The context for verification [N/A]
* @param[in] pucSign The signed hash [N/A]
* @param[in] pulSignLen The Sign length [N/A]
*
* @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 Invalid Arguments [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 Incorrect context [SEC_CRYPT_ERR_INVALID_CTX|N/A]
* @retval SEC_UINT32 Memory allocation failure [SEC_ERR_MALLOC_FAIL|N/A]
* @retval SEC_UINT32 If operation cannot be performed on key
[SEC_CRYPT_ERR_UNDEFINED_KEY_OPERATION|N/A]
* @retval SEC_UINT32 Key size not supported [SEC_ERR_INVALID_KEY_LEN|N/A]
* @retval SEC_UINT32 Library not initialized [SEC_ERR_INITLIB|N/A]
* @retval SEC_UINT32 Incorrect Algorithm ID [SEC_ERR_INVALID_ALGID|N/A]
* @retval SEC_UINT32 On Failure [SEC_ERR|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* - SM2 Verification is not supported by this API
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_verifyFinal(CRYPT_CTX *pCtx, const SEC_UCHAR *pucSign, SEC_UINT32 pulSignLen);

/*
    Func Name:  CRYPT_verify
*/
/**
* @defgroup CRYPT_verify
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_verify(
* SEC_UINT32 ulAlg,
* SEC_PKEY_S* pKey,
* const SEC_UCHAR* pucDta,
* SEC_UINT32 ulDtaLen,
* const SEC_UCHAR* pucSig,
* SEC_UINT32 ulSigLen);
* @endcode
*
* @par Purpose
* This is used to compare sign on a given data block. Calls
init, update, final.
*
* @par Description
* CRYPT_verify function compares sign on a given data block.
Calls init, update, final. This function is used when data
* is present all at once. There is no need of calling Init,
Update, Final and verification can be done in one go.
* Context is not needed here.
*
* @param[in] ulAlg Digest algorithm [N/A]
* @param[in] pKey The public key [N/A]
* @param[in] pucDta The data [N/A]
* @param[in] ulDtaLen The data length [N/A]
* @param[in] pucSig The signature [N/A]
* @param[in] ulSigLen The sign length [N/A]
*
* @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 Invalid Arguments [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 Invalid Key [SEC_CRYPT_ERR_INVALID_PKEY|N/A]
* @retval SEC_UINT32 Memory allocation failure [SEC_ERR_MALLOC_FAIL|N/A]
* @retval SEC_UINT32 If operation cannot be performed on key
[SEC_CRYPT_ERR_UNDEFINED_KEY_OPERATION|N/A]
* @retval SEC_UINT32 Key size not supported [SEC_ERR_INVALID_KEY_LEN|N/A]
* @retval SEC_UINT32 Library not initialized [SEC_ERR_INITLIB|N/A]
* @retval SEC_UINT32 Incorrect Algorithm ID [SEC_ERR_INVALID_ALGID|N/A]
* @retval SEC_UINT32 On Failure [SEC_ERR|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
*
* - If the signing is done using the library version V3R3 or later, using the
* hash algorithm SHA384 and SHA512, then verification done by
* the older version (version before V3R3) will fail. Their is problem in hash
* info of SHA384 and SHA512 in the older version is fixed in V3R3.
*
* - SM2 Sign Verification is not supported by this API
* - API supports signature verification with following unsafe algorithms:
*   - ALGID_MD5 (CID_MD5)
*   - ALGID_SHA1(CID_SHA1) \n
*   As these algorithms are unsafe, if application are using these algorithms, application must take care of any
*   security issues that may happen due to usage of these algorithms.
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_verify(SEC_UINT32 ulAlg, SEC_PKEY_S *pKey, const SEC_UCHAR *pucDta, SEC_UINT32 ulDtaLen,
    const SEC_UCHAR *pucSig, SEC_UINT32 ulSigLen);

/*
    Func Name:  CRYPT_sealInit
*/
/**
* @defgroup CRYPT_sealInit
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_sealInit(
* CRYPT_CTX* pCtx,
* SEC_UINT32 ulAlgId,
* SEC_UINT32 uiKeyArrLen,
* SEC_PKEY_S* aPubKey[],
* const SEC_UCHAR* pucIV,
* SEC_UINT32 ulIVLen,
* SEC_CHAR* aCEKey[],
* SEC_UINT32 aCEKeyLen[],
* SEC_UINT32 ulPadding);
* @endcode
*
* @par Purpose
* This is used to encrypt symmetric key with asymmetric key.
*
* @par Description
* CRYPT_sealInit function generates a random key for the
symmetric algorithm. Then it encrypts the symmetric key with
* each of the asymmetric keys passed in the aPubKey array.
The encrypted symmetric key is put in the array aCEKey and
* the length of the encrypted key is put in aCEKeyLen array.
It then initializes the symmetric context structure as in
* CRYPT_encryptinit.
*
* @param[in] ulAlgId The algorithm Id [N/A]
* @param[in] uiKeyArrLen Length of the three arrays (aPubKey,
aCEKey, and aCEKeyLen). The maximum value of this parameter can be 0xFFFFF [N/A]
* @param[in] aPubKey[] Array of public keys of asymmetric algorithms [N/A]
* @param[in] pucIV InitializationVector(CBC,CFB*,OFB algorithm modes) [N/A]
* @param[in] ulIVLen Length of IV. pass 0 if no IV [N/A]
* @param[out] pCtx Address of The context to be initialized [N/A]
* @param[out] aCEKey[] Array of encrypted the symmetric key.
This has to be freed by the user using ipsi_free() [N/A]
* @param[out] aCEKeyLen[] Array of length of encrypted key [N/A]
* @param[in] ulPadding Padding mode used for public key encryption [N/A]
*
* @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 Invalid Arguments [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 Memory allocation failure [SEC_ERR_MALLOC_FAIL|N/A]
* @retval SEC_UINT32 Incorrect Algorithm ID [SEC_ERR_INVALID_ALGID|N/A]
* @retval SEC_UINT32 Invalid Padding Mode. Padding mode set
on Algorithm is not defined
* [SEC_CRYPT_ERR_UNDEFINED_PADDING|N/A]
* @retval SEC_UINT32 Invalid IV length [SEC_ERR_INVALID_IV_LEN|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* - As memory allocation for aCEKey is done inside the library
* user is responsible for freeing the memory using ipsi_free().
* - If application does not set seeds and entropy on frequent intervals, it can lead to vulnerable keys or random
*   value. It is recommended to set seeds and entropy on frequent intervals or update the version that migrates the
*   random implementation based on DRBG.
* - Before calling this API, please make sure that DRBG is initialized for random number generation by calling
*   IPSI_CRYPT_rand_init. If application don't want to use DRBG then appplication should call
*   IPSI_CRYPT_enable_drbg(0) to disable the DRBG Random feature.
* - DRBG is enabled by default.
* - API supports following unsafe algorithms:
*   - ALGID_RC4 (CID_RC4)
*   - ALGID_DES_ECB (CID_DES_ECB)
*   - ALGID_DES_CBC (CID_DES_CBC)
*   - ALGID_DES_OFB (CID_DES_OFB)
*   - ALGID_DES_CFB (CID_DES_CFB)
*   - ALGID_DES_EDE_ECB (CID_DES_EDE_ECB)
*   - ALGID_DES_EDE_CBC (CID_DES_EDE_CBC)
*   - ALGID_DES_EDE_OFB (CID_DES_EDE_OFB)
*   - ALGID_DES_EDE_CFB (CID_DES_EDE_CFB)
*   - ALGID_DES_EDE3_ECB (CID_DES_EDE3_ECB)
*   - ALGID_DES_EDE3_CBC (CID_DES_EDE3_CBC)
*   - ALGID_DES_EDE3_OFB (CID_DES_EDE3_OFB)
*   - ALGID_DES_EDE3_CFB (CID_DES_EDE3_CFB)
*   - ALGID_KASUMI_ECB (CID_KASUMI_ECB)
*   - ALGID_KASUMI_CBC (CID_KASUMI_CBC)
*   - ALGID_KASUMI_OFB (CID_KASUMI_OFB)
*   - ALGID_KASUMI_CFB (CID_KASUMI_CFB)
*   - ALGID_RC2_ECB (CID_RC2_ECB)
*   - ALGID_RC2_CBC (CID_RC2_CBC)
*   - ALGID_RC2_OFB (CID_RC2_OFB)
*   - ALGID_RC2_CFB (CID_RC2_CFB) \n
*   As these algorithms are unsafe, if application are using these algorithms, application must take care of any
*   security issues that may happen due to usage of these algorithms.
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_sealInit(CRYPT_CTX *pCtx, SEC_UINT32 ulAlgId, SEC_UINT32 uiKeyArrLen,
    SEC_PKEY_S *aPubKey[], const SEC_UCHAR *pucIV, SEC_UINT32 ulIVLen, SEC_CHAR *aCEKey[], SEC_UINT32 aCEKeyLen[],
    SEC_UINT32 ulPadding);

/*
    Func Name:  CRYPT_sealUpdate
*/
/**
* @defgroup CRYPT_sealUpdate
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_sealUpdate(
* CRYPT_CTX ctx,
* const SEC_UCHAR * pucPlainText,
* SEC_UINT32 ulPlainLen,
* SEC_UCHAR * pucCipherText,
* SEC_UINT32 * pulCLen,
* SEC_UINT32 ulCTLen);
* @endcode
*
* @par Purpose
* This uses the initialized context to encrypt plaintext to
get ciphertext using symmetric encryption.
*
* @par Description
* CRYPT_sealUpdate function uses the initialized context to
encrypt plaintext to get ciphertext using symmetric encryption.
* Please refer to CRYPT_encryptUpadte function.
*
* @param[in] ctx The context to be used [N/A]
* @param[in] pucPlainText Plaintext to be encrypted [N/A]
* @param[in] ulPlainLen Length of plaintext [N/A]
* @param[out] pucCipherText Encrypted ciphertext [N/A]
* @param[out] pulCLen Length of encrypted ciphertext [N/A]
* @param[in] ulCTLen Size of pucCipherText [N/A]
*
* @retval SEC_UINT32 Invalid context [SEC_CRYPT_ERR_INVALID_CTX|N/A]
* @retval SEC_UINT32 On successful return [SEC_SUCCESS|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* - The safe API corresponding to this is CRYPT_sealUpdate_sf
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_sealUpdate(CRYPT_CTX ctx, const SEC_UCHAR *pucPlainText, SEC_UINT32 ulPlainLen,
    SEC_UCHAR *pucCipherText, SEC_UINT32 *pulCLen, SEC_UINT32 ulCTLen);

/*
    Func Name:  CRYPT_sealFinal
*/
/**
* @defgroup CRYPT_sealFinal
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_sealFinal(
* CRYPT_CTX* pCtx,
* SEC_UCHAR* pucCphrTxt,
* SEC_UINT32* pulCphrTxtLen,
* SEC_UINT32 ulCphrTxtLen)
* @endcode
*
* @par Purpose
* This is used to complete the seal operation.
*
* @par Description
* CRYPT_sealFinal function completes the seal operation.
It frees the symmetric context. In case of block ciphers, the
* output of this function is the last block. If sealinit
succeeds, sealfinal must be called to free the context.
*
* @param[in] pCtx Address of Context to be used [N/A]
* @param[out] pucCphrTxt Final encrypted ciphertext [N/A]
* @param[out] pulCphrTxtLen Length of the last encrypted ciphertext [N/A]
* @param[in] ulCphrTxtLen Size of pucCipherText [N/A]
*
* @retval SEC_UINT32 Invalid context [SEC_CRYPT_ERR_INVALID_CTX|N/A]
* @retval SEC_UINT32 Invalid pad mode in context
[SEC_CRYPT_ERR_UNDEFINED_PADDING|N/A]
* @retval SEC_UINT32 Invalid plaintext length given
for encryption in update and padding_mode = BLOCK_PADDING_NONE.
* (plaintext length was not integral no. of blocks)
[SEC_ERR_INVALID_DATA_LEN|N/A]
* @retval SEC_UINT32 On successful return [SEC_UINT32 SEC_SUCCESS|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* - If application does not set seeds and entropy on frequent intervals, it can lead to vulnerable keys or random
*   value. It is recommended to set seeds and entropy on frequent intervals or update the version that migrates the
*   random implementation based on DRBG.
* - The safe API corresponding to this is CRYPT_sealFinal_sf
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_sealFinal(CRYPT_CTX *pCtx, SEC_UCHAR *pucCphrTxt, SEC_UINT32 *pulCphrTxtLen,
    SEC_UINT32 ulCphrTxtLen);

/*
    Func Name:  CRYPT_seal
*/
/**
* @defgroup CRYPT_seal
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_seal(
* SEC_UINT32 ulAlgId,
* SEC_UINT32 uiKeyArrLen,
* SEC_PKEY_S* aPubKey[],
* const SEC_UCHAR* pucIV,
* SEC_UINT32 ulIVLen,
* const SEC_UCHAR* pucPlainText,
* SEC_UINT32 ulPlainLen,
* SEC_UCHAR* pucCipherText,
* SEC_UINT32* pulCLen,
* SEC_UINT32 ulCTLen,
* SEC_CHAR*  aCEKey[],
* SEC_UINT32 aCEKeyLen[],
* SEC_UINT32 ulPadding);
* @endcode
*
* @par Purpose
* This is used to generate a random key for symmetric algorithm.
*
* @par Description
* CRYPT_seal function generates a random key for symmetric algorithm.
Then, it encrypts the symmetric key with each of
* the asymmetric keys passed in the aPubKey array.
The encrypted symmetric key is added to the aCEKey array and the length
* of the encrypted key is added to the aCEKeyLen array.
It then encrypts the plaintext using the generated of the
* symmetric key to obtain the ciphertext using symmetric encryption.
*
* @param[in] ulAlgId Algorithm ID [N/A]
* @param[in] uiKeyArrLen Length of the three arrays (aPubKey,
aCEKey, and aCEKeyLen). The maximum value of this parameter can be 0xFFFFF [N/A]
* @param[in] aPubKey[] Array of public keys of asymmetric algorithms [N/A]
* @param[in] pucIV Initialization Vector(CBC,CFB,OFB modes) [N/A]
* @param[in] ulIVLen Length of IV (0 if IV is not present) [N/A]
* @param[in] pucPlainText Plaintext to be encrypted [N/A]
* @param[in] ulPlainLen Length of plaintext [N/A]
* @param[out] pucCipherText Encrypted ciphertext [N/A]
* @param[out] pulCLen Length of encrypted ciphertext [N/A]
* @param[out] aCEKey[] Array of encrypted symmetric key.
This has to be freed by the user using ipsi_free() [N/A]
* @param[out] aCEKeyLen[] Array of encrypted key length [N/A]
* @param[in] ulCTLen Size of pucCipherText [N/A]
* @param[in] ulPadding Padding mode used for public key encryption [N/A]
*
* @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 Invalid Arguments [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 Memory allocation failure [SEC_ERR_MALLOC_FAIL|N/A]
* @retval SEC_UINT32 Incorrect Algorithm ID [SEC_ERR_INVALID_ALGID|N/A]
* @retval SEC_UINT32 Invalid Padding Mode. Padding mode set on
Algorithm is not defined
* [SEC_CRYPT_ERR_UNDEFINED_PADDING|N/A]
* @retval SEC_UINT32 Invalid IV length [SEC_ERR_INVALID_IV_LEN|N/A]
* @retval SEC_UINT32 Invalid plaintext length given for
encryption and padding_mode : BLOCK_PADDING_NONE.
* (plaintext length was not integral no. of blocks)
[SEC_ERR_INVALID_DATA_LEN|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* - As memory allocation for aCEKey is done inside the
library, user is responsible for freeing the memory using ipsi_free().
* - If application does not set seeds and entropy on frequent intervals, it can lead to vulnerable keys or random
*   value. It is recommended to set seeds and entropy on frequent intervals or update the version that migrates the
*   random implementation based on DRBG.
* - Before calling this API, please make sure that DRBG is initialized for random number generation by calling
*   IPSI_CRYPT_rand_init. If application don't want to use DRBG then appplication should call
*   IPSI_CRYPT_enable_drbg(0) to disable the DRBG Random feature.
* - DRBG is enabled by default.
* - The safe API corresponding to this is CRYPT_seal_sf
* - API supports following unsafe algorithms:
*   - ALGID_RC4 (CID_RC4)
*   - ALGID_DES_ECB (CID_DES_ECB)
*   - ALGID_DES_CBC (CID_DES_CBC)
*   - ALGID_DES_OFB (CID_DES_OFB)
*   - ALGID_DES_CFB (CID_DES_CFB)
*   - ALGID_DES_EDE_ECB (CID_DES_EDE_ECB)
*   - ALGID_DES_EDE_CBC (CID_DES_EDE_CBC)
*   - ALGID_DES_EDE_OFB (CID_DES_EDE_OFB)
*   - ALGID_DES_EDE_CFB (CID_DES_EDE_CFB)
*   - ALGID_DES_EDE3_ECB (CID_DES_EDE3_ECB)
*   - ALGID_DES_EDE3_CBC (CID_DES_EDE3_CBC)
*   - ALGID_DES_EDE3_OFB (CID_DES_EDE3_OFB)
*   - ALGID_DES_EDE3_CFB (CID_DES_EDE3_CFB)
*   - ALGID_KASUMI_ECB (CID_KASUMI_ECB)
*   - ALGID_KASUMI_CBC (CID_KASUMI_CBC)
*   - ALGID_KASUMI_OFB (CID_KASUMI_OFB)
*   - ALGID_KASUMI_CFB (CID_KASUMI_CFB)
*   - ALGID_RC2_ECB (CID_RC2_ECB)
*   - ALGID_RC2_CBC (CID_RC2_CBC)
*   - ALGID_RC2_OFB (CID_RC2_OFB)
*   - ALGID_RC2_CFB (CID_RC2_CFB) \n
*   As these algorithms are unsafe, if application are using these algorithms, application must take care of any
*   security issues that may happen due to usage of these algorithms.
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_seal(SEC_UINT32 ulAlgId, SEC_UINT32 uiKeyArrLen, SEC_PKEY_S *aPubKey[],
    const SEC_UCHAR *pucIV, SEC_UINT32 ulIVLen, const SEC_UCHAR *pucPlainText, SEC_UINT32 ulPlainLen,
    SEC_UCHAR *pucCipherText, SEC_UINT32 *pulCLen, SEC_UINT32 ulCTLen, SEC_CHAR *aCEKey[], SEC_UINT32 aCEKeyLen[],
    SEC_UINT32 ulPadding);

/*
    Func Name:  CRYPT_openInit
*/
/**
* @defgroup CRYPT_openInit
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_openInit(
* CRYPT_CTX* pCtx,
* SEC_UINT32 ulAlgId,
* SEC_PKEY_S* pstPvtKey,
* const SEC_UCHAR* pucCEKey,
* SEC_UINT32 ulCEKeyLen,
* const SEC_UCHAR* pucIV,
* SEC_UINT32 ulIVLen,
* SEC_UINT32 ulPadding);
* @endcode
*
* @par Purpose
* This is used to decrypt the encrypted symmetric key.
*
* @par Description
* CRYPT_openInit function decrypts the encrypted symmetric key.
The decrypted symmetric key is used to initialize the
* context for further decryption. It initializes  the symmetric
context structure as in decryptinit for symmetric
* decryption.
*
* @param[in] ulAlgId Algorithm ID [N/A]
* @param[in] pstPvtKey Private key of asymmetric algorithm [N/A]
* @param[in] pucCEKey Encrypted symmetric key [N/A]
* @param[in] ulCEKeyLen Length of encrypted symmetric key [N/A]
* @param[in] pucIV InitializationVector(CBC,CFB,OFB modes) [N/A]
* @param[in] ulIVLen Length of IV (0 if IV is not present) [N/A]
* @param[out] pCtx Address of Context to be initialized [N/A]
* @param[in] ulPadding Padding mode used for private key decryption [N/A]
*
* @retval SEC_UINT32 Memory allocation failure [SEC_ERR_MALLOC_FAIL|N/A]
* @retval SEC_UINT32 Invalid Algorithm Id [SEC_ERR_INVALID_ALGID|N/A]
* @retval SEC_UINT32 Invalid Padding Mode. Padding mode set on
Algorithm is not defined
* [SEC_CRYPT_ERR_UNDEFINED_PADDING|N/A]
* @retval SEC_UINT32 Invalid Keylength for algorithm
[SEC_ERR_INVALID_KEY_LEN|N/A]
* @retval SEC_UINT32 Invalid Argument [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 Invalid IV length [SEC_ERR_INVALID_IV_LEN|N/A]
* @retval SEC_UINT32 On successful return [SEC_SUCCESS|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* - API supports following unsafe algorithms:
*   - ALGID_RC4 (CID_RC4)
*   - ALGID_DES_ECB (CID_DES_ECB)
*   - ALGID_DES_CBC (CID_DES_CBC)
*   - ALGID_DES_OFB (CID_DES_OFB)
*   - ALGID_DES_CFB (CID_DES_CFB)
*   - ALGID_DES_EDE_ECB (CID_DES_EDE_ECB)
*   - ALGID_DES_EDE_CBC (CID_DES_EDE_CBC)
*   - ALGID_DES_EDE_OFB (CID_DES_EDE_OFB)
*   - ALGID_DES_EDE_CFB (CID_DES_EDE_CFB)
*   - ALGID_DES_EDE3_ECB (CID_DES_EDE3_ECB)
*   - ALGID_DES_EDE3_CBC (CID_DES_EDE3_CBC)
*   - ALGID_DES_EDE3_OFB (CID_DES_EDE3_OFB)
*   - ALGID_DES_EDE3_CFB (CID_DES_EDE3_CFB)
*   - ALGID_KASUMI_ECB (CID_KASUMI_ECB)
*   - ALGID_KASUMI_CBC (CID_KASUMI_CBC)
*   - ALGID_KASUMI_OFB (CID_KASUMI_OFB)
*   - ALGID_KASUMI_CFB (CID_KASUMI_CFB)
*   - ALGID_RC2_ECB (CID_RC2_ECB)
*   - ALGID_RC2_CBC (CID_RC2_CBC)
*   - ALGID_RC2_OFB (CID_RC2_OFB)
*   - ALGID_RC2_CFB (CID_RC2_CFB) \n
*   As these algorithms are unsafe, if application are using these algorithms, application must take care of any
*   security issues that may happen due to usage of these algorithms.
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_openInit(CRYPT_CTX *pCtx, SEC_UINT32 ulAlgId, SEC_PKEY_S *pstPvtKey,
    const SEC_UCHAR *pucCEKey, SEC_UINT32 ulCEKeyLen, const SEC_UCHAR *pucIV, SEC_UINT32 ulIVLen, SEC_UINT32 ulPadding);

/*
    Func Name:  CRYPT_openUpdate
*/
/**
* @defgroup CRYPT_openUpdate
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_openUpdate(
* CRYPT_CTX ctx,
* const SEC_UCHAR* pucCipherText,
* SEC_UINT32 pulCLen,
* SEC_UCHAR* pucPlainText,
* SEC_UINT32* pulPLen,
* SEC_UINT32 ulPTLen);
* @endcode
*
* @par Purpose
* This uses the initialized context to decrypt ciphertext
into plaintext for symmetric decryption.
*
* @par Description
* CRYPT_openUpdate function uses the initialized context to
decrypt ciphertext into plaintext for symmetric decryption.
* Please refer to CRYPT_decryptUpdate_ex function.
*
* @param[in] ctx Context to be used [N/A]
* @param[in] pucCipherText Encrypted ciphertext [N/A]
* @param[in] pulCLen length of Encrypted ciphertext [N/A]
* @param[out] pucPlainText Decrypted plaintext [N/A]
* @param[out] pulPLen Length of decrypted plaintext [N/A]
* @param[in] ulPTLen The size of pucPlainText  buffer [N/A]
*
* @retval SEC_UINT32 Invalid context [SEC_CRYPT_ERR_INVALID_CTX|N/A]
* @retval SEC_UINT32 Invalid Argument [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 Invalid IV length [SEC_ERR_INVALID_IV_LEN|N/A]
* @retval SEC_UINT32 On successful return [SEC_SUCCESS|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* - The safe API corresponding to this is CRYPT_openUpdate_sf
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_openUpdate(CRYPT_CTX ctx, const SEC_UCHAR *pucCipherText, SEC_UINT32 pulCLen,
    SEC_UCHAR *pucPlainText, SEC_UINT32 *pulPLen, SEC_UINT32 ulPTLen);

/*
    Func Name:  CRYPT_openFinal
*/
/**
* @defgroup CRYPT_openFinal
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_openFinal(
* CRYPT_CTX* pCtx,
* SEC_UCHAR* pucPlainText,
* SEC_UINT32* pulPLen,
* SEC_UINT32 ulPTLen)
* @endcode
*
* @par Purpose
* This is used to complete open operation.

* @par Description
* CRYPT_openFinal function completes one open operation.
It frees the symmetric context. In case of block ciphers, the
* output of this function is the last block. If openinit succeeds,
openfinal must be called to free the context.
*
* @param[in] pCtx Address of Context to be used [N/A]
* @param[out] pucPlainText Final decrypted plaintext [N/A]
* @param[out] pulPLen Length of final decrypted plaintext [N/A]
* @param[in] ulPTLen The size of pucPlainText  buffer [N/A]
*
* @retval SEC_UINT32 Invalid context [SEC_CRYPT_ERR_INVALID_CTX|N/A]
* @retval SEC_UINT32 Invalid pad mode in context
[SEC_CRYPT_ERR_UNDEFINED_PADDING|N/A]
* @retval SEC_UINT32 When pad bits are incorrect that is after decrypting,
if the padding bits dont match the pad mode.
* It can be due to incorrect Key or IV as that will produce junk
plaintext with random padding bits, which do not
* match the padding mode [SEC_CRYPT_ERR_INVALID_PADDING|N/A]
* @retval SEC_UINT32 Invalid ciphertext length given for decryption
in update.
* (ciphertext length was not integral no. of blocks)
[SEC_ERR_INVALID_DATA_LEN|N/A]
* @retval SEC_UINT32 On successful return [SEC_SUCCESS|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* - The safe API corresponding to this is CRYPT_openFinal_sf
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_openFinal(CRYPT_CTX *pCtx, SEC_UCHAR *pucPlainText, SEC_UINT32 *pulPLen,
    SEC_UINT32 ulPTLen);

/*
    Func Name:  CRYPT_open
*/
/**
* @defgroup CRYPT_open
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_open(
* SEC_UINT32 ulAlgId,
* SEC_PKEY_S* pstPvtKey,
* const SEC_UCHAR* pucCEKey,
* SEC_UINT32 ulCEKeyLen,
* const SEC_UCHAR* pucIV,
* SEC_UINT32 ulIVLen,
* const SEC_UCHAR* pucCipherText,
* SEC_UINT32 pulCLen,
* SEC_UCHAR* pucPlainText,
* SEC_UINT32* pulPLen,
* SEC_UINT32 ulPTLen,
* SEC_UINT32 ulPadding);
* @endcode
*
* @par Purpose
* This is used to decrypt the encrypted symmetric key.
It then uses the decrypted symmetric key to decrypt the ciphertext
* into plaintext using symmetric decryption.
*
* @par Description
* CRYPT_open function decrypts the encrypted symmetric key.
It then uses the decrypted symmetric key to decrypt the
* ciphertext into plaintext using symmetric decryption.
*
* @param[in] ulAlgId Algorithm ID [N/A]
* @param[in] pstPvtKey Private key of asymmetric algorithm [N/A]
* @param[in] pucCEKey Encrypted symmetric key [N/A]
* @param[in] ulCEKeyLen Length of encrypted symmetric key [N/A]
* @param[in] pucIV InitializationVector(CBC,CFB,OFB modes) [N/A]
* @param[in] ulIVLen Length of IV. pass 0 if no IV [N/A]
* @param[in] pucCipherText Ciphertext to be decrypted [N/A]
* @param[in] pulCLen Length of  ciphertext [N/A]
* @param[out] pucPlainText Decrypted Plaintext [N/A]]
* @param[out] pulPLen Length of plaintext [N/A]
* @param[in] ulPTLen The size of pucPlainText  buffer [N/A]
* @param[in] ulPadding Padding mode used for private key decryption [N/A]
*
* @retval SEC_UINT32 If Memory allocation fails [SEC_ERR_MALLOC_FAIL|N/A]
* @retval SEC_UINT32 If invalid Algorithm Id [SEC_ERR_INVALID_ALGID|N/A]
* @retval SEC_UINT32 Invalid Padding Mode. Padding mode set on
Algorithm is not defined
* [SEC_CRYPT_ERR_UNDEFINED_PADDING|N/A]
* @retval SEC_UINT32 The pad bits are incorrect. When after
decrypting the padding bits dont match the pad mode. It can
* be due to incorrect Key or IV as that will produce junk
plaintext with random padding bits, which do not match the
* padding mode [SEC_CRYPT_ERR_INVALID_PADDING|N/A]
* @retval SEC_UINT32 Invalid ciphertext length given for
decryption (ciphertext length was not integral number of blocks)
* [SEC_ERR_INVALID_DATA_LEN|N/A]
* @retval SEC_UINT32 Invalid Keylength for algorithm
[SEC_ERR_INVALID_KEY_LEN|N/A]
* @retval SEC_UINT32 Invalid Argument [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 Invalid IV length [SEC_ERR_INVALID_IV_LEN|N/A]
* @retval SEC_UINT32 On successful return [SEC_SUCCESS|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* - The safe API corresponding to this is CRYPT_open_sf
* - API supports following unsafe algorithms:
*   - ALGID_RC4 (CID_RC4)
*   - ALGID_DES_ECB (CID_DES_ECB)
*   - ALGID_DES_CBC (CID_DES_CBC)
*   - ALGID_DES_OFB (CID_DES_OFB)
*   - ALGID_DES_CFB (CID_DES_CFB)
*   - ALGID_DES_EDE_ECB (CID_DES_EDE_ECB)
*   - ALGID_DES_EDE_CBC (CID_DES_EDE_CBC)
*   - ALGID_DES_EDE_OFB (CID_DES_EDE_OFB)
*   - ALGID_DES_EDE_CFB (CID_DES_EDE_CFB)
*   - ALGID_DES_EDE3_ECB (CID_DES_EDE3_ECB)
*   - ALGID_DES_EDE3_CBC (CID_DES_EDE3_CBC)
*   - ALGID_DES_EDE3_OFB (CID_DES_EDE3_OFB)
*   - ALGID_DES_EDE3_CFB (CID_DES_EDE3_CFB)
*   - ALGID_KASUMI_ECB (CID_KASUMI_ECB)
*   - ALGID_KASUMI_CBC (CID_KASUMI_CBC)
*   - ALGID_KASUMI_OFB (CID_KASUMI_OFB)
*   - ALGID_KASUMI_CFB (CID_KASUMI_CFB)
*   - ALGID_RC2_ECB (CID_RC2_ECB)
*   - ALGID_RC2_CBC (CID_RC2_CBC)
*   - ALGID_RC2_OFB (CID_RC2_OFB)
*   - ALGID_RC2_CFB (CID_RC2_CFB) \n
*   As these algorithms are unsafe, if application are using these algorithms, application must take care of any
*   security issues that may happen due to usage of these algorithms.
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_open(SEC_UINT32 ulAlgId, SEC_PKEY_S *pstPvtKey, const SEC_UCHAR *pucCEKey,
    SEC_UINT32 ulCEKeyLen, const SEC_UCHAR *pucIV, SEC_UINT32 ulIVLen, const SEC_UCHAR *pucCipherText,
    SEC_UINT32 pulCLen, SEC_UCHAR *pucPlainText, SEC_UINT32 *pulPLen, SEC_UINT32 ulPTLen, SEC_UINT32 ulPadding);

/*
    Func Name:  CRYPT_pubKeyEncrypt
*/
/**
* @defgroup CRYPT_pubKeyEncrypt
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_pubKeyEncrypt(
* SEC_PKEY_S* pKey,
* const SEC_UCHAR*  pucPlainText,
* SEC_UINT32  ulPlainLen,
* SEC_UINT32  ulPadMode,
* SEC_UCHAR*  pucCipherText,
* SEC_UINT32* pulCLen);
* @endcode
*
* @par Purpose
* This is used to perform a public key encryption operation.
*
* @par Description
* CRYPT_pubKeyEncrypt function performs a public key
encryption operation. The key passed to the function must be a
* public key or a key pair. The data size must not e
xceed size of key and for padding modes DEFAULT, PKCS1 and SSLv23,
* data size must not be > (key size - 11 bytes), and for OAEP,
data size must be < (key size - 41 bytes) for SHA1
data size must be < (key size - 65 bytes) for SHA256,
data size must be < (key size - 129 bytes) for SHA512.
*
* @param[in] pKey Public key to be used [N/A]
* @param[in] pucPlainText Data buffer [N/A]
* @param[in] ulPlainLen Data buffer length [N/A]
* @param[in] ulPadMode Padding mode used [N/A]
* @param[out] pucCipherText Encrypted data buffer [N/A]
* @param[in/out] pulCLen Encrypted data buffer length [N/A]
*
* @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 Invalid Arguments [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 Key size not supported [SEC_ERR_INVALID_KEY_LEN|N/A]
* @retval SEC_UINT32 Invalid Key [SEC_CRYPT_ERR_INVALID_PKEY|N/A]
* @retval SEC_UINT32 Key is neither Pair nor public
[SEC_CRYPT_ERR_INVALID_PKEY_TYPE|N/A]
* @retval SEC_UINT32 Data size larger than key
[SEC_CRYPT_ERR_LARGE_DATA_SIZE|N/A]
* @retval SEC_UINT32 Undefined padding mode
[SEC_CRYPT_ERR_UNDEFINED_PADDING|N/A]
* @retval SEC_UINT32 If operation cannot be
performed on key [SEC_CRYPT_ERR_UNDEFINED_KEY_OPERATION|N/A]
* @retval SEC_UINT32 Library not initialized [SEC_ERR_INITLIB|N/A]
* @retval SEC_UINT32 Incorrect Algorithm ID [SEC_ERR_INVALID_ALGID|N/A]
* @retval SEC_UINT32 On Failure [SEC_ERR|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* - pucCipherText buffer size should be equal to or greater than the key size in bytes.
*    CRYPT_PKEY_size can be used to get the key size.
* - In case of no padding, if the data size is less than the key size then zero will be prepended to the data
      to create the data size equal to key size.
* - It is recommended that RSA_SSLV23_PADDING should be used only with SSL protocol.
* - If application does not set seeds and entropy on frequent intervals, it can lead to vulnerable keys or random
*   value. It is recommended to set seeds and entropy on frequent intervals or update the version that migrates the
*   random implementation based on DRBG.
* - For OEAP padding method, use padding mode RSA_PKCS1_OAEP_PADDING for SHA1 as hash, RSA_PKCS1_OAEP_PADDING_SHA256
*    for SHA256 as hash & RSA_PKCS1_OAEP_PADDING_SHA512 for SHA512 as hash.
* - The safe API corresponding to this is CRYPT_pubKeyEncrypt_sf
* - Before calling this API, please make sure that DRBG is initialized for random number generation by calling
*   IPSI_CRYPT_rand_init. If application don't want to use DRBG then appplication should call
*   IPSI_CRYPT_enable_drbg(0) to disable the DRBG Random feature.
* - DRBG is enabled by default.
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_pubKeyEncrypt(SEC_PKEY_S *pKey, const SEC_UCHAR *pucPlainText, SEC_UINT32 ulPlainLen,
    SEC_UINT32 ulPadMode, SEC_UCHAR *pucCipherText, SEC_UINT32 *pulCLen);

/*
    Func Name:  CRYPT_privKeyDecrypt
*/
/**
* @defgroup CRYPT_privKeyDecrypt
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_privKeyDecrypt (
* SEC_PKEY_S* pKey,
* const SEC_UCHAR*  pucCipherText,
* SEC_UINT32  ulCipherLen,
* SEC_UINT32  ulPadMode,
* SEC_UCHAR*  pucPlainText,
* SEC_UINT32* pulPLen);
* @endcode
*
* @par Purpose
* This is used to perform a private key decryption operation.
*
* @par Description
* CRYPT_privKeyDecrypt function performs a private key
decryption operation. The key must be a private key or a key pair.
*
* @param[in] pKey Private key to be used [N/A]
* @param[in] pucCipherText Cipher buffer [N/A]
* @param[in] ulCipherLen Cipher buffer length [N/A]
* @param[in] ulPadMode Padding mode used [N/A]
* @param[out] pucPlainText Data buffer [N/A]
* @param[in/out] pulPLen Data buffer length [N/A]
*
* @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 Invalid Arguments [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 Key size not supported [SEC_ERR_INVALID_KEY_LEN|N/A]
* @retval SEC_UINT32 Invalid Key [SEC_CRYPT_ERR_INVALID_PKEY|N/A]
* @retval SEC_UINT32 Key is neither Pair nor Private
[SEC_CRYPT_ERR_INVALID_PKEY_TYPE|N/A]
* @retval SEC_UINT32 Data size larger than key
[SEC_CRYPT_ERR_LARGE_DATA_SIZE|N/A]
* @retval SEC_UINT32 Undefined padding mode
[SEC_CRYPT_ERR_UNDEFINED_PADDING|N/A]
* @retval SEC_UINT32 If operation cannot be
performed on key [SEC_CRYPT_ERR_UNDEFINED_KEY_OPERATION|N/A]
* @retval SEC_UINT32 Library not initialized [SEC_ERR_INITLIB|N/A]
* @retval SEC_UINT32 Incorrect Algorithm ID [SEC_ERR_INVALID_ALGID|N/A]
* @retval SEC_UINT32 On Failure [SEC_ERR|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* - pucPlainText buffer size should be equal to or greater than the key size in bytes.
*    CRYPT_PKEY_size can be used to get the key size.
* - In case of no padding, the zero bytes are prepended during CRYPT_pubKeyEncrypt will not be removed.
* - It is recommended that RSA_SSLV23_PADDING should be used only with SSL protocol.
* - For OEAP padding method, use padding mode RSA_PKCS1_OAEP_PADDING for SHA1 as hash, RSA_PKCS1_OAEP_PADDING_SHA256
*    for SHA256 as hash & RSA_PKCS1_OAEP_PADDING_SHA512 for SHA512 as hash.
* - The safe API corresponding to this is CRYPT_privKeyDecrypt_sf
* - Before calling this API, please make sure that DRBG is initialized for random number generation by calling
*   IPSI_CRYPT_rand_init. If application don't want to use DRBG then appplication should call
*   IPSI_CRYPT_enable_drbg(0) to disable the DRBG Random feature.
* - DRBG is enabled by default.
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_privKeyDecrypt(SEC_PKEY_S *pKey, const SEC_UCHAR *pucCipherText, SEC_UINT32 ulCipherLen,
    SEC_UINT32 ulPadMode, SEC_UCHAR *pucPlainText, SEC_UINT32 *pulPLen);

/*
    Func Name:  CRYPT_PKEY_new
*/
/**
 * @defgroup CRYPT_PKEY_new
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_PKEY_S *CRYPT_PKEY_new
 * @endcode
 *
 * @par Purpose
 * This is used to create a new uninitialized key structure.
 *
 * @par Description
 * CRYPT_PKEY_new function creates a new uninitialized key structure.
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_PKEY_S* If Memory allocation fails [0|N/A]
 * @retval SEC_PKEY_S* Otherwise [New key structure|N|A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * CRYPT_PKEY_free should be used to free the memory create by the function CRYPT_PKEY_new
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_PKEY_S *CRYPT_PKEY_new(void);

/*
    Func Name:  CRYPT_PKEY_free
*/
/**
 * @defgroup CRYPT_PKEY_free
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_VOID CRYPT_PKEY_free(
 * SEC_PKEY_S* pKey)
 * @endcode
 *
 * @par Purpose
 * This is used to free the key structure.
 *
 * @par Description
 * CRYPT_PKEY_free function frees the key structure.
 *
 * @param[in] pKey Key to be freed [N/A]
 *
 * @retval SEC_VOID This function does not return any value [N/A|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_VOID CRYPT_PKEY_free(SEC_PKEY_S *pKey);

/*
    Func Name:  CRYPT_PKEY_type
*/
/**
 * @defgroup CRYPT_PKEY_type
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_PKEY_type(
 * const SEC_PKEY_S* pKey)
 * @endcode
 *
 * @par Purpose
 * This is used to return the algorithm type of a key.
 *
 * @par Description
 * CRYPT_PKEY_type function returns the algorithm type of a key.
 *
 * @param[in] pKey Key to be used [N/A]
 *
 * @retval SEC_UINT32 If key is valid [Algorithm ID stored in the key|N/A]
 * @retval SEC_UINT32 If key is invalid [SEC_ERR_INVALID_ARG|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_PKEY_type(const SEC_PKEY_S *pKey);

/*
    Func Name:  CRYPT_PKEY_bits
*/
/**
 * @defgroup CRYPT_PKEY_bits
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_PKEY_bits(
 * const SEC_PKEY_S* pKey)
 * @endcode
 *
 * @par Purpose
 * This returns the size of the asymmetric key in bits.
 *
 * @par Description
 * CRYPT_PKEY_bits function returns the size of the asymmetric key in bits.
 *
 * @param[in] pKey Key to be used [N/A]
 *
 * @retval SEC_UINT32 If key is valid [Number of bits in the key|N/A]
 * @retval SEC_UINT32 If key is invalid [0|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_PKEY_bits(const SEC_PKEY_S *pKey);

/*
    Func Name:  CRYPT_PKEY_size
*/
/**
 * @defgroup CRYPT_PKEY_size
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_PKEY_size(
 * const SEC_PKEY_S* pKey)
 * @endcode
 *
 * @par Purpose
 * This returns the size of the asymmetric key in bytes.
 *
 * @par Description
 * CRYPT_PKEY_size function returns the size of the asymmetric key in bytes.
 *
 * @param[in] pKey Key to be used [N/A]
 *
 * @retval SEC_UINT32 If key is valid [Number of bytes in the key|N/A]
 * @retval SEC_UINT32 If key is invalid [0|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_PKEY_size(const SEC_PKEY_S *pKey);

/*
    Func Name:  CRYPT_PKEY_cmp
*/
/**
 * @defgroup CRYPT_PKEY_cmp
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_PKEY_cmp(
 * const SEC_PKEY_S* pKeyA,
 * const SEC_PKEY_S* pKeyB)
 * @endcode
 *
 * @par Purpose
 * This is used to compare two keys.
 *
 * @par Description
 * CRYPT_PKEY_cmp function compares the two keys.
 *
 * @param[in] pKeyA First key to be used [N/A]
 * @param[in] pKeyB Second key to be used [N/A]
 *
 * @retval SEC_UINT32 When keys match [SEC_CRYPT_MATCH|N/A]
 * @retval SEC_UINT32 When keys do not match [SEC_CRYPT_NO_MATCH|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_PKEY_cmp(const SEC_PKEY_S *pKeyA, const SEC_PKEY_S *pKeyB);

/*
    Func Name:  CRYPT_PKEY_copy
*/
/**
* @defgroup CRYPT_PKEY_copy
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_PKEY_copy(
* const SEC_PKEY_S* pSrc,
* SEC_PKEY_S* pDest)
* @endcode
*
* @par Purpose
* This is used to copy a key structure to another.
*
* @par Description

* CRYPT_PKEY_copy function copies a key structure to another.
This function does not copy the parameters.
*
* @param[in] pSrc Source key [N/A]
* @param[in] pDest Destination key [N/A]
*
* @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 If invalid Key [SEC_CRYPT_ERR_INVALID_PKEY|N/A]
* @retval SEC_UINT32 If invalid Arguments [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 On Memory allocation failure [SEC_ERR_MALLOC_FAIL|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* N/A
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_PKEY_copy(const SEC_PKEY_S *pSrc, SEC_PKEY_S *pDest);

/*
 Func Name:  CRYPT_PKEY_copyPubKey
*/
/**
* @defgroup CRYPT_PKEY_copyPubKey
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_PKEY_copyPubKey(
* const SEC_PKEY_S* pKey,
* SEC_PKEY_S* pOutPubKey)
* @endcode
*
* @par Purpose
* This is used to copy  the public key structure from the given key pair
* to another SEC_PKEY_S structure.
*
* @par Description

* CRYPT_PKEY_copyPubKey function copies the public key from the
* given key pair to another.
*
* @param[in] pKey Source key [N/A]
* @param[in] pOutPubKey Destination key [N/A]
*
* @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 If invalid Key [SEC_CRYPT_ERR_INVALID_PKEY|N/A]
* @retval SEC_UINT32 If invalid AlgorithmId [SEC_ERR_INVALID_ALGID]
* @retval SEC_UINT32 If invalid Arguments [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 On Memory allocation failure [SEC_ERR_MALLOC_FAIL|N/A]
* @par Required Header File
* sec_crypto.h
*
* @par Note
* The memory for the key pOutPubKey must be allocated
* before calling this function. Hence the same should be freed by the user.
* This API is applicable only for RSA, ECDSA and ECDH algorithm.
* N/A
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_PKEY_copyPubKey(const SEC_PKEY_S *pKey, SEC_PKEY_S *pOutPubKey);

/*
    Func Name:  CRYPT_PKEY_copyPrivKey
*/
/**
* @defgroup CRYPT_PKEY_copyPrivKey
* @ingroup HLF High Level Functions
* @par Prototype
* @code

* SEC_UINT32 CRYPT_PKEY_copyPrivKey(
* const SEC_PKEY_S* pKey,
* SEC_PKEY_S* pOutPrvKey)
* @endcode
*

* @par Purpose
* This function is used to copy the private key from the
* given key pair to another SEC_PKEY_S structure.
*
* @par Description

* CRYPT_PKEY_copyPrivKey function copies the private key from the given
* key pair to another.
*
* @param[in] pKey Source key [N/A]
* @param[in] pOutPrvKey Destination key [N/A]
*
* @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 If invalid Key [SEC_CRYPT_ERR_INVALID_PKEY|N/A]
* @retval SEC_UINT32 If invalid AlgorithmId [SEC_ERR_INVALID_ALGID]
* @retval SEC_UINT32 If invalid Arguments [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 On Memory allocation failure [SEC_ERR_MALLOC_FAIL|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* The memory for the key pOutPrvKey must be allocated
* before calling this function. Hence the same should be freed by the user.
* This API is applicable only for RSA, ECDSA and ECDH algorithm.
* N/A
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_PKEY_copyPrivKey(const SEC_PKEY_S *pKey, SEC_PKEY_S *pOutPrvKey);

/*
    Func Name:  CRYPT_PKEY_checkParam
*/
/**
* @defgroup CRYPT_PKEY_checkParam
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_PKEY_checkParam(
* const SEC_PKEY_S* pkey)
* @endcode
*
* @par Purpose
* This is used to check whether parameters are defined for a key structure.
*
* @par Description
* CRYPT_PKEY_checkParam function checks whether parameters are defined
for a key structure.
*
* @param[in] pKey Key to be checked [N/A]
*
* @retval SEC_UINT32  If parameters are defined [SEC_TRUE|N/A]
* @retval SEC_UINT32 If parameters are not defined [SEC_FALSE|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* N/A
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_PKEY_checkParam(const SEC_PKEY_S *pKey);

/*
    Func Name:  CRYPT_PKEY_cmpParam
*/
/**
 * @defgroup CRYPT_PKEY_cmpParam
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_PKEY_cmpParam(
 * const SEC_PKEY_S* pKeyA,
 * const SEC_PKEY_S* pKeyB)
 * @endcode
 *
 * @par Purpose
 * This is used to compare the parameters of two key structures.
 *
 * @par Description
 * CRYPT_PKEY_cmpParam function compares the parameters of two key structures.
 *
 * @param[in] pKeyA First key to be compared [N/A]
 * @param[in] pKeyB Second key to be compared [N/A]
 *
 * @retval SEC_UINT32 When parameters match [SEC_CRYPT_MATCH|N/A]
 * @retval SEC_UINT32 When parameters do not match [SEC_CRYPT_NO_MATCH|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_PKEY_cmpParam(const SEC_PKEY_S *pKeyA, const SEC_PKEY_S *pKeyB);

/*
    Func Name:  CRYPT_PKEY_copyParam
*/
/**
* @defgroup CRYPT_PKEY_copyParam
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_PKEY_copyParam(
* const SEC_PKEY_S* pSrc,
* const SEC_PKEY_S* pDest)
* @endcode
*
* @par Purpose
* This is used to copy the parameters of a key structure to
another key structure.
*
* @par Description
* CRYPT_PKEY_copyParam function copies the parameters of a key
structure to another key structure.
*
* @param[in] pSrc Source key [N/A]
* @param[in] pDest Destination key [N/A]
*
* @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 If algorithm not supported
[SEC_CRYPT_ERR_UNDEFINED_KEY_OPERATION|N/A]
* @retval SEC_UINT32 Invalid Key [SEC_CRYPT_ERR_INVALID_PKEY|N/A]
* @retval SEC_UINT32 Invalid Arguments [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 On Memory allocation failure [SEC_ERR_MALLOC_FAIL|N/A]
* @retval SEC_UINT32 If Keys are of different type
[SEC_ERR_KEYPAIR_MISMATCH|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* N/A
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_PKEY_copyParam(const SEC_PKEY_S *pSrc, const SEC_PKEY_S *pDest);

/*
    Func Name:  CRYPT_PKEY_privORpub
*/
/**
* @defgroup CRYPT_PKEY_privORpub
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_PKEY_privORpub(
* const SEC_PKEY_S* pKey)
* @endcode
*
* @par Purpose
* This is used to check whether a given key is a private key,
public key, or a key pair.
*
* @par Description
* CRYPT_PKEY_privORpub function checks whether a given key is
a private key, public key, or a key pair.
*
* @param[in] pKey Key to be checked [N/A]
*
* @retval SEC_UINT32 If Private key [CRYPT_PKEY_PRIVATE|N/A]
* @retval SEC_UINT32 If Public key [CRYPT_PKEY_PUBLIC|N/A]
* @retval SEC_UINT32 If Key Pair [CRYPT_PKEY_PAIR|N/A]
* @retval SEC_UINT32 If unknown Key type [SEC_ERR_INVALID_ALGID|N/A]
* @retval SEC_UINT32 If invalid Key [SEC_CRYPT_ERR_INVALID_PKEY|N/A]
* @retval SEC_UINT32 If invalid Arguments [SEC_ERR_INVALID_ARG|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* N/A
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_PKEY_privORpub(const SEC_PKEY_S *pKey);

/*
    Func Name:  CRYPT_PKEY_checkKeyPair
*/
/**
* @defgroup CRYPT_PKEY_checkKeyPair
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_PKEY_checkKeyPair(
* SEC_PKEY_S* pPubKey,
* SEC_PKEY_S* pPrivKey)
* @endcode
*
* @par Purpose
* This is used to check whether a given key-pair is well-formed.
*
* @par Description
* CRYPT_PKEY_checkKeyPair function checks whether a given key-pair
is well-formed.
*
* @param[in] pPubKey Public key to be used [N/A]
* @param[in] pPrivKey Private key to be used [N/A]
*
* @retval SEC_UINT32 If keys are proper [SEC_TRUE|N/A]
* @retval SEC_UINT32 If keys are not proper or Library is
not initialized [SEC_FALSE|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* - This function is used by the PSE Library.
* - This API do not support ALGID_ECDH (EC Diffie-Hellman) and ALGID_DH(Diffie-Hellman) key types.
* - Before calling this API, please make sure that DRBG is initialized for random number generation by calling
*   IPSI_CRYPT_rand_init. If application don't want to use DRBG then appplication should call
*   IPSI_CRYPT_enable_drbg(0) to disable the DRBG Random feature.
* - DRBG is enabled by default.
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_PKEY_checkKeyPair(SEC_PKEY_S *pPubKey, SEC_PKEY_S *pPrivKey);

/*
    Func Name:  CRYPT_PKEY_checkKeyPairEx
*/
/**
* @defgroup CRYPT_PKEY_checkKeyPairEx
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_BOOL CRYPT_PKEY_checkKeyPairEx(
* SEC_PKEY_S* pPubKey,
* SEC_PKEY_S* pPrivKey)
* @endcode
*
* @par Purpose
* This is used to check whether a given public key and
private key are of the same pair.
*
* @par Description
* CRYPT_PKEY_checkKeyPairEx function checks whether a
given public key and private key are of the same pair.
*
* @param[in] pPubKey Public key to be used [N/A]
* @param[in] pPrivKey Private key to be used [N/A]
*
* @retval SEC_BOOL If keys belong to a pair [SEC_TRUE|N/A]
* @retval SEC_BOOL If keys does not belong to a pair or
Library is not initialized [SEC_FALSE|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* - This function is used by the PSE Library.
* - This API do not support ALGID_ECDH (EC Diffie-Hellman) and ALGID_DH(Diffie-Hellman) key types.
* - If application does not set seeds and entropy on frequent intervals, it can lead to vulnerable keys or random
*   value. It is recommended to set seeds and entropy on frequent intervals or update the version that migrates the
*   random implementation based on DRBG.
* - Before calling this API, please make sure that DRBG is initialized for random number generation by calling
*   IPSI_CRYPT_rand_init. If application don't want to use DRBG then appplication should call
*   IPSI_CRYPT_enable_drbg(0) to disable the DRBG Random feature.
* - DRBG is enabled by default.
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_BOOL CRYPT_PKEY_checkKeyPairEx(SEC_PKEY_S *pPubKey, SEC_PKEY_S *pPrivKey);

/*
    Func Name:  CRYPT_PKEY_getAlgParams
*/
/**
* @defgroup CRYPT_PKEY_getAlgParams
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_VOID* CRYPT_PKEY_getAlgParams(
* const SEC_PKEY_S* pKey)
* @endcode
*
* @par Purpose
* This is used to retrieve the params of a key.
*
* @par Description
* CRYPT_PKEY_getAlgParams function retrieves the params of a key.
This is the pointer to the params stored in the key,
* so must not be freed by user. The returned pointer is a pointer
to an algorithm specific parameter struct like
* DSA_PARA_S.
*
* @param[in] pKey Key struct [N/A]
*
* @retval SEC_VOID* If key is valid [The parameter struct
corresponding to the key|N/A]
* @retval SEC_VOID* If key doesnot have params or arguments
are missing [0|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* N/A
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_VOID *CRYPT_PKEY_getAlgParams(const SEC_PKEY_S *pKey);

/*
    Func Name:  CRYPT_PKEY_setAlgParams
*/
/**
* @defgroup CRYPT_PKEY_setAlgParams
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_PKEY_setAlgParams(
* const SEC_PKEY_S* pKey,
* SEC_VOID* pParams)
* @endcode
*
* @par Purpose
* This is used to copy the parameters of an algorithm to the key.
*
* @par Description
* CRYPT_PKEY_setAlgParams function copies the parameters of
an algorithm to the key. The pParams parameter argument must
* contain an algorithm specific parameter structure like
RSA_PARA_S. After setting params, the input parameter pParams
* can be freed.
*
* @param[in] pKey Key struct [N/A]
* @param[in] pParams Parameter structure [N/A]
*
* @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 If algorithm is not supported
[SEC_CRYPT_ERR_UNDEFINED_KEY_OPERATION|N/A]
* @retval SEC_UINT32 Invalid Key [SEC_CRYPT_ERR_INVALID_PKEY|N/A]
* @retval SEC_UINT32 Invalid Arguments [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 On Memory allocation failure [SEC_ERR_MALLOC_FAIL|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* N/A
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_PKEY_setAlgParams(const SEC_PKEY_S *pKey, SEC_VOID *pParams);

/*
    Func Name:  IPSI_CRYPT_rsaBlindingAndConstTimeOff
*/
/**
 * @defgroup IPSI_CRYPT_rsaBlindingAndConstTimeOff
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_VOID IPSI_CRYPT_rsaBlindingAndConstTimeOff()
 * @endcode
 *
 * @par Purpose
 * This is used to switch off the RSA blinding feature and constant time
 * operations which helps in improving the RSA performance.
 * By default, this feature is not disabled.
 *
 * @par Description
 * IPSI_CRYPT_rsaBlindingAndConstTimeOff switch off the RSA blinding feature and
 * constant time operations.
 *
 * @par Parameters
 * N/A
 *
 * @retval
 * N/A
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * - RSA blinding feature is to avoid timing attacks on RSA. Before switching
 * off this feature, user should consider the possibilities of this attack
 * in their scenario.
 *
 * -It is a global function, application need to make sure that the function will not be called in parallel
 * with the RSA operations.
 *
 * @par Related Topics
 * IPSI_CRYPT_rsaBlindingAndConstTimeOn
 */
CRYPTOLINKDLL SEC_VOID IPSI_CRYPT_rsaBlindingAndConstTimeOff(void);

/*
    Func Name:  IPSI_CRYPT_rsaBlindingAndConstTimeOn
*/
/**
* @defgroup IPSI_CRYPT_rsaBlindingAndConstTimeOn
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_VOID IPSI_CRYPT_rsaBlindingAndConstTimeOn()
* @endcode
*
* @par Purpose
* This is used to switch on the RSA blinding feature and constant time
* operations. No need to call this function if user does not disable this
* feature using API IPSI_CRYPT_rsaBlindingAndConstTimeOff.
*
* @par Description
* IPSI_CRYPT_rsaBlindingAndConstTimeOn switches on the RSA blinding feature and
* constant time operations.
*
* @par Parameters
* N/A
*
* @retval
* N/A

* @par Required Header File
* sec_crypto.h
*
* @par Note
*  - No need to call this function if user does not disable this feature using API
* IPSI_CRYPT_rsaBlindingAndConstTimeOff.
*
* -It is a global function, application need to make sure that the function will not be called in parallel
*    with the RSA operations.
*
* @par Related Topics
* IPSI_CRYPT_rsaBlindingAndConstTimeOff
*/
CRYPTOLINKDLL SEC_VOID IPSI_CRYPT_rsaBlindingAndConstTimeOn(void);

/*
    Func Name:  IPSI_CRYPT_rsaKeygenConstTimeOff
*/
/**
* @defgroup IPSI_CRYPT_rsaKeygenConstTimeOff
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_VOID IPSI_CRYPT_rsaKeygenConstTimeOff()
* @endcode
*
* @par Purpose
* This is used to switch off the constant time operations while key generation ,which helps in improving the
* RSA performance.
* By default, this feature is not disabled.
*
* @par Description
* IPSI_CRYPT_rsaKeygenConstTimeOff switch off the constant time operation while RSA key generation
*
* @par Parameters
* N/A
*
* @retval
* N/A

* @par Required Header File
* sec_crypto.h
*
* @par Note
* - Contant time operation is to avoid cache timing side channel attack on RSA key generation. Before switching
* off this feature, user should consider the possibilities of this attack in their scenario.
*
* -It is a global function, application need to make sure that the function will not be called in parallel
*    with the RSA operations.
*
*
* @par Related Topics
* IPSI_CRYPT_rsaKeygenConstTimeOn
*/
CRYPTOLINKDLL SEC_VOID IPSI_CRYPT_rsaKeygenConstTimeOff(void);

/*
    Func Name:  IPSI_CRYPT_rsaKeygenConstTimeOn
*/
/**
* @defgroup IPSI_CRYPT_rsaKeygenConstTimeOn
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_VOID IPSI_CRYPT_rsaKeygenConstTimeon()
* @endcode
*
* @par Purpose
* This is used to switch on the RSA key generation with constant time operations. No need to call this function if user
* does not disable this* feature using API IPSI_CRYPT_rsaKeygenConstTimeOff.
*
* @par Description
* IPSI_CRYPT_rsaKeygenConstTimeOn switches on the RSA key generation with constant time operations.
*
* @par Parameters
* N/A
*
* @retval
* N/A

* @par Required Header File
* sec_crypto.h
*
* @par Note
*  - No need to call this function if user does not disable this feature using API
* IPSI_CRYPT_rsaKeygenConstTimeOff.
*
* -It is a global function, application need to make sure that the function will not be called in parallel
*    with the RSA key generations.
*
* @par Related Topics
* IPSI_CRYPT_rsaKeygenConstTimeOff
*/
CRYPTOLINKDLL SEC_VOID IPSI_CRYPT_rsaKeygenConstTimeOn(void);

/*
    Func Name:  CRYPT_privKeyEncrypt
*/
/**
* @defgroup CRYPT_privKeyEncrypt
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_privKeyEncrypt(
* SEC_PKEY_S* pkey,
* const SEC_UCHAR*  pucPlainText,
* SEC_UINT32  ulPlainLen,
* SEC_UINT32  ulPadding,
* SEC_UCHAR*  pucCipherText,
* SEC_UINT32* pulCLen);
* @endcode
*
* @par Purpose
* This is used to perform a private key encryption operation.
*
* @par Description
* CRYPT_privKeyEncrypt function performs a private key
* encryption operation. The key passed to the function must be a
* private key or a key pair. The data size must not exceed size of key and
* for padding modes PKCS1 - Type1 (Default padding mode) data size must be < (key size - 11 bytes) and
* for No padding data size should be equal or less than the key size. Supported padding modes are
* PKCS1 (Type1), No padding and Default Padding. Default Padding will be PKCS1 (Type1) padding.
* @param[in] pKey Private key to be used [N/A]
* @param[in] pucPlainText Data buffer [N/A]
* @param[in] ulPlainLen Data buffer length [N/A]
* @param[in] ulPadding Padding mode used [N/A]
* @param[out] pucCipherText Encrypted data buffer [N/A]
* @param[in/out] pulCLen Encrypted data buffer length [N/A]
*
* @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 Invalid Arguments [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 Key size not supported [SEC_ERR_INVALID_KEY_LEN|N/A]
* @retval SEC_UINT32 Invalid Key [SEC_CRYPT_ERR_INVALID_PKEY|N/A]
* @retval SEC_UINT32 Key is neither Pair nor private
[SEC_CRYPT_ERR_INVALID_PKEY_TYPE|N/A]
* @retval SEC_UINT32 Data size larger than key
[SEC_CRYPT_ERR_LARGE_DATA_SIZE|N/A]
* @retval SEC_UINT32 Undefined padding mode
[SEC_CRYPT_ERR_UNDEFINED_PADDING|N/A]
* @retval SEC_UINT32 If operation cannot be
performed on key [SEC_CRYPT_ERR_UNDEFINED_KEY_OPERATION|N/A]
* @retval SEC_UINT32 Library not initialized [SEC_ERR_INITLIB|N/A]
* @retval SEC_UINT32 Incorrect Algorithm ID [SEC_ERR_INVALID_ALGID|N/A]
* @retval SEC_UINT32 On Failure [SEC_ERR|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* - pucCipherText buffer size should be equal to or greater than the key size in bytes.
*    CRYPT_PKEY_size can be used to get the key size.
* - In case of no padding, if the data size is less than the key size then zero will be prepended to the data
*      to create the data size equal to key size.
* - If application does not set seeds and entropy on frequent intervals, it can lead to vulnerable keys or random
*   value. It is recommended to set seeds and entropy on frequent intervals or update the version that migrates the
*   random implementation based on DRBG.
* - Before calling this API, please make sure that DRBG is initialized for random number generation by calling
*   IPSI_CRYPT_rand_init. If application don't want to use DRBG then appplication should call
*   IPSI_CRYPT_enable_drbg(0) to disable the DRBG Random feature.
* - DRBG is enabled by default.
* - The safe API corresponding to this is CRYPT_privKeyEncrypt_sf
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_privKeyEncrypt(SEC_PKEY_S *pKey, const SEC_UCHAR *pucPlainText, SEC_UINT32 ulPlainLen,
    SEC_UINT32 ulPadding, SEC_UCHAR *pucCipherText, SEC_UINT32 *pulCLen);

/*
    Func Name:  CRYPT_pubKeyDecrypt
*/
/**
* @defgroup CRYPT_pubKeyDecrypt
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_pubKeyDecrypt (
* SEC_PKEY_S* pkey,
* const SEC_UCHAR*  pucCipherText,
* SEC_UINT32  ulCipherLen,
* SEC_UINT32  ulPadding,
* SEC_UCHAR*  pucPlainText,
* SEC_UINT32* pulPLen);
* @endcode
*
* @par Purpose
* This is used to perform a public key decryption operation.
*
* @par Description
* CRYPT_pubKeyDecrypt function performs a public key
* decryption operation. The key must be a public key or a key pair. Supported padding modes are
* PKCS1 (Type1), No padding and Default Padding. Default Padding will be PKCS1 (Type1) padding.
*
* @param[in] pKey Public key to be used [N/A]
* @param[in] pucCipherText Cipher buffer [N/A]
* @param[in] ulCipherLen Cipher buffer length [N/A]
* @param[in] ulPadding Padding mode used [N/A]
* @param[out] pucPlainText Data buffer [N/A]
* @param[in/out] pulPLen Data buffer length [N/A]
*
* @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 Invalid Arguments [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 Key size not supported [SEC_ERR_INVALID_KEY_LEN|N/A]
* @retval SEC_UINT32 Invalid Key [SEC_CRYPT_ERR_INVALID_PKEY|N/A]
* @retval SEC_UINT32 Key is neither Pair nor Public
[SEC_CRYPT_ERR_INVALID_PKEY_TYPE|N/A]
* @retval SEC_UINT32 Data size larger than key
[SEC_ERR_DATA_GREATER_THAN_KEY_LEN|N/A]
* @retval SEC_UINT32 Undefined padding mode
[SEC_CRYPT_ERR_UNDEFINED_PADDING|N/A]
* @retval SEC_UINT32 If operation cannot be
performed on key [SEC_CRYPT_ERR_UNDEFINED_KEY_OPERATION|N/A]
* @retval SEC_UINT32 Library not initialized [SEC_ERR_INITLIB|N/A]
* @retval SEC_UINT32 Incorrect Algorithm ID [SEC_ERR_INVALID_ALGID|N/A]
* @retval SEC_UINT32 On Failure [SEC_ERR|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* - pucPlainText buffer size should be equal to or greater than the key size in bytes.
*    CRYPT_PKEY_size can be used to get the key size.
* - In case of no padding, the zero bytes which are prepended during CRYPT_privKeyEncrypt will not be removed.
* - Before calling this API, please make sure that DRBG is initialized for random number generation by calling
*   IPSI_CRYPT_rand_init. If application don't want to use DRBG then appplication should call
*   IPSI_CRYPT_enable_drbg(0) to disable the DRBG Random feature.
* - DRBG is enabled by default.
* - The safe API corresponding to this is CRYPT_pubKeyDecrypt_sf
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_pubKeyDecrypt(SEC_PKEY_S *pKey, const SEC_UCHAR *pucCipherText, SEC_UINT32 ulCipherLen,
    SEC_UINT32 ulPadding, SEC_UCHAR *pucPlainText, SEC_UINT32 *pulPLen);

/*
    Func Name:  CRYPT_sm2GenZ
*/
/**
 * @defgroup CRYPT_sm2GenZ
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_sm2GenZ (
 * SEC_UINT32 ulDigestAlg,
 * const SEC_PKEY_S *pKey,
 * const SEC_UCHAR *pucId,
 * SEC_UINT32 uiIdLen,
 * SEC_UCHAR *pucZ,
 * SEC_UINT32 *pulZLen,
 * SEC_UINT32 ulInpZLen);
 * @endcode
 *
 * @par Purpose
 * This API is used to generate Z value, based on the Unique User ID,
 * Elliptic curve parameters and selected Hash Algorithm.
 * The Z - value has to be used later for various SM2 operations.
 * Logic to calculate ZA (Read as Z value for user A)
 * ZA : HashAlg (ENTLA || IDA || a || b || xG || yG || xA || yA)
 * ENTLA   - Length of unique user identifier string (optional)
 * IDA     - Unique user identifier string (optional)
 * a		- Coefficient 'a' of the Elliptic Curve equation
 * b		- Coefficient 'b' of the Elliptic Curve equation
 * xG      - X value of base point of the Elliptic Curve
 * yG      - Y value of base point of the Elliptic Curve
 * xA      - X value of public key point on the Elliptic Curve for user A
 * yA      - Y value of public key point on the Elliptic Curve for user A
 * @par Description
 * CRYPT_sm2GenZ function generates the Z value specific to a user.
 *
 * @param[in] ulDigestAlg Hash algorithm to be used in Z calculation [N/A]
 * Supports only 256 bit hash algorithms like ALGID_SHA256 and ALGID_SM3.
 * @param[in] pKey The (Public) or (Public & Private pair) Key, along with EC parameters [N/A]
 * @param[in] pucId Unique ID of the user [N/A]
 * Optional parameter. Pass NULL, if not applicable.
 * @param[in] uiIdLen Length of Unique ID of user [N/A]
 * Optional parameter. Pass zero, if unique ID is NULL. Maximum uIdLen can be (65535/8) = 8191.
 * @param[out] pucZ Z value of the user [N/A]
 * @param[out] pulZLen Length of Z value [N/A]
 * @param[in] ulInpZLen The pucZ buffer size [N/A]
 *
 * @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 Invalid arguments passed [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 Incorrect Algorithm ID [SEC_ERR_INVALID_ALGID|N/A]
 * @retval SEC_UINT32 Memory allocation failed [SEC_ERR_MALLOC_FAIL|N/A]
 * @retval SEC_UINT32 Mathematical operation failed [SEC_ERR|N/A]
 * @retval SEC_UINT32 Invalid pKey passed [SEC_CRYPT_ERR_INVALID_PKEY|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * - The size of the buffer pointed by pucZ should be equal to the length
 * returned by the API CRYPT_MD_SIZE(ulDigestAlg)
 * - The safe API corresponding to this is CRYPT_sm2GenZ_sf
 * \n
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_sm2GenZ(SEC_UINT32 ulDigestAlg, const SEC_PKEY_S *pKey, const SEC_UCHAR *pucId,
    SEC_UINT32 uiIdLen, SEC_UCHAR *pucZ, SEC_UINT32 *pulZLen, SEC_UINT32 ulInpZLen);

/*
    Func Name:  CRYPT_sm2KepInit
*/
/**
* @defgroup CRYPT_sm2KepInit
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_sm2KepInit(
* CRYPT_SM2_KEP_CTX *ctx,
* SEC_PKEY_S *pKey,
* SEC_UINT32 ulKdfDigestAlg,
* SEC_UINT32 ulHashAlg,
* EC_POINT_S **pR)
* @endcode
*
* @par Purpose
* This function will create the context required for Key exchange protocol and return the elliptic curve point R
* @par Description
* This function will create the context required for Key exchange protocol and return the elliptic curve point R.
* This operation is equivalent to the steps A1-A2 mentioned for user A in the standard. It also takes as input
* the kdf digest and hash algorithm id which will be used for all kdf and hash operations involved during the
* key exchange protocol. As per standard if we consider user B point of view, then this function is
* equivalent to the steps B1-B2.
*
* @param[in] pKey Contains the private key and EC parameters [N/A]
* @param[in] ulKdfDigestAlg Hash algorithm to be used during KDF operation.
*   ALGID_MD5 and ALGID_MD4 are not valid algorithms for KDF operation [N/A]
* @param[in] ulHashAlg Hash algorithm to be used for hash operation in Key exchange protocol [N/A]
* @param[out] ctx SM2 key exchange protocol context
* @param[out] pR EC point R [N/A]
*
* @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 Invalid arguments passed [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 Incorrect Algorithm ID [SEC_ERR_INVALID_ALGID|N/A]
* @retval SEC_UINT32 Memory allocation failed [SEC_ERR_MALLOC_FAIL|N/A]
* @retval SEC_UINT32 Mathematical operation failed [SEC_ERR|N/A]
* @retval SEC_UINT32 Invalid pKey passed [SEC_CRYPT_ERR_INVALID_PKEY|N/A]

* @par Required Header File
* sec_crypto.h
*
* @par Note
* - If application does not set seeds and entropy on frequent intervals, it can lead to vulnerable keys or random
*   value. It is recommended to set seeds and entropy on frequent intervals or update the version that migrates the
*   random implementation based on DRBG.
* - Before calling this API, please make sure that DRBG is initialized for random number generation by calling
*   IPSI_CRYPT_rand_init. If application don't want to use DRBG then appplication should call
*   IPSI_CRYPT_enable_drbg(0) to disable the DRBG Random feature.
* - DRBG is enabled by default.
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_sm2KepInit(CRYPT_SM2_KEP_CTX *ctx, SEC_PKEY_S *pKey, SEC_UINT32 ulKdfDigestAlg,
    SEC_UINT32 ulHashAlg, EC_POINT_S **pR);

/*
    Func Name:  CRYPT_sm2KepGenKey
*/
/**
 * @defgroup CRYPT_sm2KepGenKey
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_sm2KepGenKey(
 * const IPSI_SM2_KEP_GENKEY_OP_DATA_S *opData,
 * SEC_UCHAR *pucKey,
 * SEC_UINT32 ulKeyLen)
 * @endcode
 *
 * @par Purpose
 * This function will create the shared secret key using SM2 KEP
 * @par Description
 * This function will create the shared secret key using SM2 Key Exchange Protocol.
 * This operation is equivalent to the steps A4-A8 mentioned for user A in the standard.
 * As per standard if we consider user B point of view, then this function is equivalent to the steps B3-B7.
 *
 * @param[in] opData Contains the input required for Key Generation [N/A]
 * @param[in] ulKeyLen Length of the shared key to be generated [N/A]
 * @param[out] pucKey Shared key to be generated [N/A]
 *
 * @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 Invalid arguments passed [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 Memory allocation failed [SEC_ERR_MALLOC_FAIL|N/A]
 * @retval SEC_UINT32 Mathematical operation failed [SEC_ERR|N/A]
 * @retval SEC_UINT32 Invalid pKey passed [SEC_CRYPT_ERR_INVALID_PKEY|N/A]
 * @retval SEC_UINT32 Invalid SM2 KEP context [SEC_CRYPT_ERR_INVALID_CTX|N/A]]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * - The size of the buffer pointed by pucKey should be as per ulKeyLen.
 * - The Z len of A or B should not be more than 250MB i.e opData->stZA.uiZLen or opData->stZB.uiZLen should not be more
 * than 250MB
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_sm2KepGenKey(const IPSI_SM2_KEP_GENKEY_OP_DATA_S *opData, SEC_UCHAR *pucKey,
    SEC_UINT32 ulKeyLen);

/*
    Func Name:  CRYPT_sm2KepGenHash
*/
/**
 * @defgroup CRYPT_sm2KepGenHash
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_sm2KepGenHash(
 * CRYPT_SM2_KEP_CTX ctx,
 * IPSI_SM2_KEP_HASHTYPE_E enHashType,
 * SEC_UCHAR *pucS,
 * SEC_UINT32 *pulSLen,
 * SEC_UINT32 uiSsize)
 * @endcode
 *
 * @par Purpose
 * This function generates the optional hash as mentioned in the standard.
 * @par Description
 * This function generates the optional hash as mentioned in the standard.
 * If we consider the hash generation of user B, this function is equivalent to the step B8
 * and enHashType should be selected as IPSI_SM2_HASH_SB. If we consider the hash generation for user A,
 * this function is equivalent to the step A10 and enHashType should be selected as IPSI_SM2_HASH_SA.
 *
 * @param[in] ctx Contains the input required for Key Generation [N/A]
 * @param[in] enHashType The hash operation to be considered either IPSI_SM2_HASH_SA or IPSI_SM2_HASH_SB [N/A]
 * @param[out] pucS Generated hash [N/A]
 * @param[out] pulSLen Pointer to the length of generated hash [N/A]
 * @param[out] uiSsize The pucS buffer size [N/A]
 *
 * @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 Invalid arguments passed [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 Memory allocation failed [SEC_ERR_MALLOC_FAIL|N/A]
 * @retval SEC_UINT32 Mathematical operation failed [SEC_ERR|N/A]
 * @retval SEC_UINT32 Invalid SM2 KEP context [SEC_CRYPT_ERR_INVALID_CTX|N/A]]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * - The size of the buffer pointed by pucS should be CRYPT_MD_SIZE(ulHashAlg).
 * ulHashAlg is the hash algorithm set in the API CRYPT_sm2KepInit
 * - The safe API corresponding to this is CRYPT_sm2KepGenHash_sf
 * \n
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_sm2KepGenHash(CRYPT_SM2_KEP_CTX ctx, IPSI_SM2_KEP_HASHTYPE_E enHashType, SEC_UCHAR *pucS,
    SEC_UINT32 *pulSLen, SEC_UINT32 uiSsize);

/*
    Func Name:  CRYPT_sm2KepVerifyHash
*/
/**
 * @defgroup CRYPT_sm2KepVerifyHash
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_sm2KepVerifyHash(
 * CRYPT_SM2_KEP_CTX *ctx,
 * IPSI_SM2_KEP_HASHTYPE_E enHashType,
 * const SEC_UCHAR *pucS,
 * SEC_UINT32 ulSLen,
 * SEC_UINT32 *verifyResult);
 * @endcode
 *
 * @par Purpose
 * This function compares the remote hash with the internally generated hash value.
 * @par Description
 * This function compares the remote hash with the internally generated hash value
 * and returns either success or failure. If we consider hash verification of user A,
 * this function is equivalent to step A9 and enHashType should be selected as IPSI_SM2_HASH_S1.
 * If we consider hash verification of user B, this function is equivalent to step B10
 * and enHashType should be selected as IPSI_SM2_HASH_S2.
 *
 * @param[in] ctx SM2 key exchange protocol context [N/A]
 * @param[in] enHashType The hash operation to be considered either IPSI_SM2_HASH_S1 or IPSI_SM2_HASH_S2 [N/A]
 * @param[in] pucS Pointer to the hash which needs to be verified [N/A]
 * @param[in] ulSLen  Length of the hash to be verified [N/A]
 * @param[out] verifyResult  Verification result - whether success or failure.
 * Set to SEC_TRUE if the hash verification is successful.
 * Set to SEC_FALSE if the hash verification is unsuccessful. [N/A]
 *
 * @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 Invalid arguments passed [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 Memory allocation failed [SEC_ERR_MALLOC_FAIL|N/A]
 * @retval SEC_UINT32 Mathematical operation failed [SEC_ERR|N/A]
 * @retval SEC_UINT32 Invalid SM2 KEP context [SEC_CRYPT_ERR_INVALID_CTX|N/A]]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * \n
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_sm2KepVerifyHash(CRYPT_SM2_KEP_CTX ctx, IPSI_SM2_KEP_HASHTYPE_E enHashType,
    const SEC_UCHAR *pucS, SEC_UINT32 ulSLen, SEC_UINT32 *verifyResult);

/*
    Func Name:  CRYPT_sm2KepRemove
*/
/**
 * @defgroup CRYPT_sm2KepRemove
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * void CRYPT_sm2KepRemove(
 * CRYPT_SM2_KEP_CTX *ctx)
 * @endcode
 *
 * @par Purpose
 * Free the memory allocated held in SM2 key exchange protocol context.
 * @par Description
 * Free the memory allocated held in SM2 key exchange protocol context.
 *
 * @param[in] ctx Memory to be freed [N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * \n
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL void CRYPT_sm2KepRemove(CRYPT_SM2_KEP_CTX *ctx);

/*
    Func Name:  CRYPT_sm2PkeaGetOutputSize
*/
/**
 * @defgroup CRYPT_sm2PkeaGetOutputSize
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_sm2PkeaGetOutputSize(
 * const SEC_PKEY_S *pKey,
 * SEC_UINT32 ulHashAlg,
 * SEC_UINT32 ulMsgLen,
 * SEC_UINT32 uiCipherType,
 * SEC_UINT32 *pulSizeInBytes)
 * @endcode
 *
 * @par Purpose
 * This function returns the size of the output after SM2 PKEA operation.
 * @par Description
 * This function returns the size of the output after SM2 PKEA operation.
 * User need to call this function to obtain the size of the memory which must be allocated for output buffer.
 *
 * @param[in] pKey Contains the EC public key [N/A]
 * @param[in] ulHashAlg Hash algorithm to be used for calculation of C3 during PKEA operation [N/A]
 * @param[in] ulMsgLen  Input data length [N/A]
 * @param[in] uiCipherType The cipher text consist of C1|| C2 || C3, C1 is a point and the point can
 * be encoded as compressed or uncompressed format and the format will be determined by uiCipherType. The
 * value of uiCipherType can be SEC_UNCOMPRESSED_CIPHER or SEC_COMPRESSED_CIPHER [N/A]
 * @param[out] pulSizeInBytes Size of the output in bytes after PKEA operation [N/A]
 *
 * @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 Invalid arguments passed [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 EC Key field is not prime or polynomial [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 EC parameter's prime length or degree is zero [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 Incorrect Hash Algorithm ID [SEC_ERR_INVALID_ALGID|N/A]
 * @retval SEC_UINT32 Invalid pKey passed [SEC_CRYPT_ERR_INVALID_PKEY|N/A]
 * @retval SEC_UINT32 Message length passed is too large i.e. MAX of SEC_UINT32
 * or a value which lead to overflow of the SEC_UINT32 by adding it to the hash size and
 * point length [SEC_ERR_INVALID_DATA_LEN|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * While decrypting the encrypted text the plain text length can be taken as cipher text length,
 * the plain text length will always be less than the cipher text length.
 * \n
 *
 * @par Related Topics
 * CRYPT_sm2PkeaEncrypt
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_sm2PkeaGetOutputSize(const SEC_PKEY_S *pKey, SEC_UINT32 ulHashAlg, SEC_UINT32 ulMsgLen,
    SEC_UINT32 uiCipherType, SEC_UINT32 *pulSizeInBytes);

/*
    Func Name:  CRYPT_sm2PkeaEncrypt
*/
/**
 * @defgroup CRYPT_sm2PkeaEncrypt
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_sm2PkeaEncrypt(
 * SEC_PKEY_S *pKey,
 * SEC_UINT32 ulKdfDigestAlg,
 * SEC_UINT32 ulHashAlg,
 * SEC_UCHAR *pucPlainText,
 * SEC_UINT32 ulPlainLen,
 * SEC_UINT32 uiCipherType,
 * SEC_UCHAR *pucCipherText,
 * SEC_UINT32 *pulCLen);
 * @endcode
 *
 * @par Purpose
 * This function encrypts the input plaintext as per SM2 PKEA standard.
 * @par Description
 * This function encrypts the input plaintext as per SM2 PKEA standard.
 * It takes as input the EC public key and digest algorithm used during kdf calculation
 * and the hash algorithm used for calculation of C3.
 *
 * @param[in] pKey Contains the EC public key [N/A]
 * @param[in] ulKdfDigestAlg Hash algorithm to be used during KDF operation [N/A]
 * @param[in] ulHashAlg Hash algorithm to be used for calculation of C3 [N/A]
 * @param[in] pucPlainText Pointer to input data to be encrypted [N/A]
 * @param[in] ulPlainLen Input data buffer length [N/A]
 * @param[in] uiCipherType The cipher text consist of C1|| C2 || C3, C1 is a point and the point can
 * be encoded as compressed or uncompressed format and the format will be determined by uiCipherType. The
 * value of uiCipherType can be SEC_UNCOMPRESSED_CIPHER or SEC_COMPRESSED_CIPHER [N/A]
 * @param[out] pucCipherText  Pointer to the output encrypted data buffer [N/A]
 * @param[in/out] pulCLen Pointer to the encrypted data length [N/A]
 *
 * @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 Invalid arguments passed [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 Memory allocation failed [SEC_ERR_MALLOC_FAIL|N/A]
 * @retval SEC_UINT32 Incorrect Hash Algorithm ID [SEC_ERR_INVALID_ALGID|N/A]
 * @retval SEC_UINT32 Invalid Key [SEC_CRYPT_ERR_INVALID_PKEY|N/A]
 * @retval SEC_UINT32 Mathematical operation failed [SEC_ERR|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * -The size of the buffer pointed to by pucCipherText, should be atleast equal to the
 * size returned by the API CRYPT_sm2PkeaGetOutputSize.
 * - ALGID_MD5 is not a valid algorithm for ulKdfDigestAlg
 * - ALGID_MD4 is not a valid algorithm for ulHashAlg and ulKdfDigestAlg
 * - If application does not set seeds and entropy on frequent intervals, it can lead to vulnerable keys or random
 * value. It is recommended to set seeds and entropy on frequent intervals or update the version that migrates the
 * random implementation based on DRBG.
 * - Before calling this API, please make sure that DRBG is initialized for random number generation by calling
 * IPSI_CRYPT_rand_init. If application don't want to use DRBG then appplication should call
 * IPSI_CRYPT_enable_drbg(0) to disable the DRBG Random feature.
 * - DRBG is enabled by default.
 * - The safe API corresponding to this is CRYPT_sm2PkeaEncrypt_sf
 *
 * \n
 *
 * @par Related Topics
 * CRYPT_sm2PkeaGetOutputSize
 * CRYPT_sm2PkeaDecrypt
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_sm2PkeaEncrypt(SEC_PKEY_S *pKey, SEC_UINT32 ulKdfDigestAlg, SEC_UINT32 ulHashAlg,
    SEC_UCHAR *pucPlainText, SEC_UINT32 ulPlainLen, SEC_UINT32 uiCipherType, SEC_UCHAR *pucCipherText,
    SEC_UINT32 *pulCLen);

/*
    Func Name:  CRYPT_sm2PkeaDecrypt
*/
/**
 * @defgroup CRYPT_sm2PkeaDecrypt
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_sm2PkeaDecrypt(
 * SEC_PKEY_S *pKey,
 * SEC_UINT32 ulKdfDigestAlg,
 * SEC_UINT32 ulHashAlg,
 * SEC_UCHAR *pucCipherText,
 * SEC_UINT32 ulCipherLen,
 * SEC_UCHAR *pucPlainText,
 * SEC_UINT32 *pulPLen);
 * @endcode
 *
 * @par Purpose
 * This function decrypts the cipher text as per SM2 PKEA standard.
 * @par Description
 * This function decrypts the cipher text as per SM2 PKEA standard.
 * It takes as input the EC public key and digest algorithm used during kdf calculation
 * and the hash algorithm used for calculation of C3.
 *
 * @param[in] pKey Contains the EC public key [N/A]
 * @param[in] ulKdfDigestAlg Hash algorithm to be used during KDF operation [N/A]
 * @param[in] ulHashAlg Hash algorithm to be used for calculation of C3 [N/A]
 * @param[in] pucCipherText Pointer to the input cipher text [N/A]
 * @param[in] ulCipherLen Input cipher text length [N/A]
 * @param[out] pucPlainText  Pointer to the decrypted data buffer [N/A]
 * @param[in/out] pulPLen Pointer to decrypted data length [N/A]
 *
 * @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 Invalid arguments passed [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 Memory allocation failed [SEC_ERR_MALLOC_FAIL|N/A]
 * @retval SEC_UINT32 Incorrect Hash Algorithm ID [SEC_ERR_INVALID_ALGID|N/A]
 * @retval SEC_UINT32 Invalid Key [SEC_CRYPT_ERR_INVALID_PKEY|N/A]
 * @retval SEC_UINT32 Mathematical operation failed [SEC_ERR|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * - The size of the buffer pointed to by pucPlainText, should be atleast equal to ulCipherLen.
 * - ALGID_MD5 is not a valid algorithm for ulKdfDigestAlg
 * - ALGID_MD4 is not a valid algorithm for ulHashAlg and ulKdfDigestAlg
 * - If application does not set seeds and entropy on frequent intervals, it can lead to vulnerable keys or random
 * value. It is recommended to set seeds and entropy on frequent intervals or update the version that migrates the
 * random implementation based on DRBG.
 * - The safe API corresponding to this is CRYPT_sm2PkeaDecrypt_sf
 * \n
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_sm2PkeaDecrypt(SEC_PKEY_S *pKey, SEC_UINT32 ulKdfDigestAlg, SEC_UINT32 ulHashAlg,
    SEC_UCHAR *pucCipherText, SEC_UINT32 ulCipherLen, SEC_UCHAR *pucPlainText, SEC_UINT32 *pulPLen);

/*
    Func Name:  CRYPT_checkECKey
*/
/**
 * @defgroup CRYPT_checkECKey
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_checkECKey(
 * const SEC_PKEY_S *ecKey)
 *
 * @endcode
 *
 * @par Purpose
 * This function checks whether the EC key is valid.
 * @par Description
 * This function checks the whether the EC key is valid. It checks the following:
 * Public Key is on the elliptic curve.
 * Public Key * Order is point at infinity.
 * Base Point * Private Key is Public Key.
 *
 * @param[in] ecKey Input key contains either public key or key pair [N/A]
 *
 * @retval SEC_UINT32 Key verification is successful [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 Memory allocation failed [SEC_ERR_MALLOC_FAIL|N/A]
 * @retval SEC_UINT32 Key verification failed [SEC_ERR|N/A]
 * @retval SEC_UINT32 Invalid Key type [SEC_CRYPT_ERR_INVALID_PKEY|N/A]
 * @retval SEC_UINT32 Invalid arguments passed [SEC_ERR_INVALID_ARG|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * \n
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_checkECKey(const SEC_PKEY_S *ecKey);

/*
    Func Name:  CRYPT_createECParamsById_ex
*/
/**
* @defgroup CRYPT_createECParamsById_ex
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* EC_PARA_S *CRYPT_createECParamsById_ex(
* SEC_UINT32 uiParamID)
* @endcode
*
* @par Purpose
* This is used to get the param structure along with the co-factor for a given
well-known parameter Id.
*
* @par Description
* CRYPT_createECParamsById_ex function returns the param structure
* for a given well-known parameter Id. The parameters returned will contain
* all the parameter including the cofactor and should be freed by user by using IPSI_EC_PARA_FREE.
*
* @param[in] uiParamID The id of the well-known parameters [N/A]
*
* @retval EC_PARA_S* On error [0|N/A]
* @retval EC_PARA_S* Otherwise [The parameter structure
corresponding to id|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* Returned pointer has to be freed by the application using IPSI_EC_PARA_FREE.
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL EC_PARA_S *CRYPT_createECParamsById_ex(SEC_UINT32 uiParamID);

/*
    Func Name:  CRYPT_computeECDHKey_ex
*/
/**
 * @defgroup CRYPT_computeECDHKey_ex
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_computeECDHKey_ex(
 * const SEC_PKEY_S *pPvtKey,
 * const SEC_PKEY_S *pPubKey,
 * SEC_UCHAR *pucKey,
 * SEC_UINT32 *pulKeyLen,
 * IPSI_ECDH_PRIMITIVE_E uiEcdhPrimitive);
 * @endcode
 *
 * @par Purpose
 * This function is used to generate a ECDH shared secret key.
 * This function supports both Elliptic Curve Standard and Modified (cofactor) Diffie Hellman Primitive.
 *
 * @par Description
 * This function is an extenstion to CRYPT_computeECDHKey. This function generates a ECDH shared secret key
 * from private key and peer public key. The memory for the shared secret must have been allocated prior to calling.
 * Based on the enum value passed to uiEcdhPrimitive, this function generates shared secret key using
 * either standard or modified Diffie Hellman primitive.
 * In the EC Standard DH primitive, cofactor is not required and pCoFactor in EC_PARA_S is ignored.
 * In the EC Modified (cofactor) DH primitive, pCoFactor value present in EC_PARA_S is considered
 * and error is returned if pCoFactor is set to NULL.
 *
 * @param[in] pPvtKey Own private key [N/A]
 * @param[in] pPubKey Peer public key [N/A]
 * @param[out] pucKey Shared secret key [N/A]
 * @param[in/out] pulKeyLen Length of generated shared secret key [N/A]
 * @param[in] uiEcdhPrimitive Identifies the ECDH primitive to be used whether EC Standard DH Primitive or
 * EC Modified (Cofactor) DH Primitive [N/A]
 *
 * @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 Invalid Key [SEC_CRYPT_ERR_INVALID_PKEY|N/A]
 * @retval SEC_UINT32 Invalid Arguments [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 If library is not initialized [SEC_ERR_INITLIB|N/A]
 * @retval SEC_UINT32 Incorrect Algorithm ID i.e. Key type is not ALGID_ECDH or ALGID_ECDSA or
 * ALGID_ECKEY [SEC_ERR_INVALID_ALGID|N/A]
 * @retval SEC_UINT32 Memory allocation failed [SEC_ERR_MALLOC_FAIL|N/A]
 * @retval SEC_UINT32 On other errors [SEC_ERR|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * - The minimum size of output key buffer should be atleast the input public key size.
 * - The safe API corresponding to this is CRYPT_computeECDHKey_sf
 * \n
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_computeECDHKey_ex(const SEC_PKEY_S *pPvtKey, const SEC_PKEY_S *pPubKey,
    SEC_UCHAR *pucKey, SEC_UINT32 *pulKeyLen, IPSI_ECDH_PRIMITIVE_E uiEcdhPrimitive);

/*
    Func Name:  CRYPT_PKEY_sign_size
*/
/**
 * @defgroup CRYPT_PKEY_sign_size
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_PKEY_sign_size(const SEC_PKEY_S* pKey)
 * @endcode
 *
 * @par Purpose
 * This function is used to get the maximum signature size for the key passed.
 *
 * @par Description
 * This function is used to get the maximum signature size for the key passed.
 *
 * @param[in] pKey Key to be used [N/A]
 *
 * @retval SEC_UINT32 The maximum signature size for the key passed [Size of signature|N/A]
 * @retval SEC_UINT32 If undefined algorithm [0|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * \n
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_PKEY_sign_size(const SEC_PKEY_S *pKey);

/*
    Func Name:  CRYPT_genKey
*/
/**
* @defgroup CRYPT_genKey
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_genKey(
* SEC_UINT32 ulAlgId,
* SEC_UCHAR* pucKey)
* @endcode
*
* @par Purpose
* This is used to generate a random key for the given symmetric algorithm.
*
* @par Description
* CRYPT_genKey function generates a random key for the
given symmetric algorithm. The memory for the key must be allocated
* before calling this function. For RC4 algorithm, the
* keylength will be RC4_DEFAULT_KEYLEN. For RC2 algorithm, the keylength
* will be RC2_DEFAULT_KEYLEN. For other symmetric
* algorithms,it will be their respective keylength.
*
*
* @param[in] ulAlgId Symmetric algorithm [N/A]
* @param[out] pucKey Generated random key [N/A]
*
* @retval SEC_UINT32 Invalid Symmetric Algorithm Id [SEC_ERR_INVALID_ALGID|N/A]
* @retval SEC_UINT32 On successful return [SEC_SUCCESS|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* - If application does not set seeds and entropy on frequent intervals, it can lead to vulnerable keys or random
*   value. It is recommended to set seeds and entropy on frequent intervals or update the version that migrates the
*   random implementation based on DRBG.
* - Before calling this API, please make sure that DRBG is initialized for random number generation by calling
*   IPSI_CRYPT_rand_init. If application don't want to use DRBG then appplication should call
*   IPSI_CRYPT_enable_drbg(0) to disable the DRBG Random feature.
* - DRBG is enabled by default.
* - The safe API corresponding to this is CRYPT_genKey_sf
* - API supports following unsafe algorithms:
*   - ALGID_RC4 (CID_RC4)
*   - ALGID_DES_ECB (CID_DES_ECB)
*   - ALGID_DES_CBC (CID_DES_CBC)
*   - ALGID_DES_OFB (CID_DES_OFB)
*   - ALGID_DES_CFB (CID_DES_CFB)
*   - ALGID_DES_EDE_ECB (CID_DES_EDE_ECB)
*   - ALGID_DES_EDE_CBC (CID_DES_EDE_CBC)
*   - ALGID_DES_EDE_OFB (CID_DES_EDE_OFB)
*   - ALGID_DES_EDE_CFB (CID_DES_EDE_CFB)
*   - ALGID_DES_EDE3_ECB (CID_DES_EDE3_ECB)
*   - ALGID_DES_EDE3_CBC (CID_DES_EDE3_CBC)
*   - ALGID_DES_EDE3_OFB (CID_DES_EDE3_OFB)
*   - ALGID_DES_EDE3_CFB (CID_DES_EDE3_CFB)
*   - ALGID_KASUMI_ECB (CID_KASUMI_ECB)
*   - ALGID_KASUMI_CBC (CID_KASUMI_CBC)
*   - ALGID_KASUMI_OFB (CID_KASUMI_OFB)
*   - ALGID_KASUMI_CFB (CID_KASUMI_CFB)
*   - ALGID_BF_ECB (CID_BF_ECB)
*   - ALGID_BF_CBC (CID_BF_CBC)
*   - ALGID_BF_CFB (CID_BF_CFB)
*   - ALGID_BF_OFB (CID_BF_OFB)
*   - ALGID_RC2_ECB (CID_RC2_ECB)
*   - ALGID_RC2_CBC (CID_RC2_CBC)
*   - ALGID_RC2_OFB (CID_RC2_OFB)
*   - ALGID_RC2_CFB (CID_RC2_CFB) \n
*   As these algorithms are unsafe, if application are using these algorithms, application must take care of any
*   security issues that may happen due to usage of these algorithms.
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_genKey(SEC_UINT32 ulAlgId, SEC_UCHAR *pucKey);

/*
    Func Name:  CRYPT_randSeed
*/
/**
* @defgroup CRYPT_randSeed
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_randSeed(
* const SEC_UCHAR * pSeed,
* SEC_UINT32 ulLen)
* @endcode
*
* @par Purpose
* This is used to set the given seed to the random number generator.
*
* @par Description
* CRYPT_randSeed function sets the given seed to the
random number generator and uses it for random number generation.
*
* @param[in] pucSeed Buffer to hold the random seed [N/A]
* @param[in] ulLen The size of the buffer [N/A]
*
* @retval SEC_UINT32 On successful set of seed [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 If pucSeed was NULL or ulLen was 0
[SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 If the hardware mode is set
[SEC_HW_ERR_INVALID_OPERATION|N/A]
* @retval SEC_UINT32 If the memcpy_s operation is failed[SEC_ERR_MEMCPY_FAILED|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* - CRYPT_randSeed, CRYPT_getRandState, CRYPT_setRandState, CRYPT_saveRandState, CRYPT_loadRandState,
*    CRYPT_addEntropy, CRYPT_testRand and random number generation should not be called in parallel
* N/A
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_randSeed(const SEC_UCHAR *pucSeed, SEC_UINT32 ulLen);

/*
   Func Name:  CRYPT_addEntropy
*/
/**
* @defgroup CRYPT_addEntropy
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_addEntropy(
* const SEC_UCHAR* pucEntropyBuf,
* SEC_UINT32 ulLen)
* @endcode
*
* @par Purpose
* This is used to add the given entropy data to the random pool.
*
* @par Description
* CRYPT_addEntropy function adds the given entropy data to the random pool.
*
* @param[in] pucEntropyBuf Buffer to hold the entropy [N/A]
* @param[in] ulLen The size of the buffer [N/A]
*
* @retval SEC_UINT32 On successful set of seed [SEC_SUCCESS|N/A].
* @retval SEC_UINT32 If pucEntropyBuf is NULL or ulLen is 0
[SEC_ERR_INVALID_ARG|N/A].
* @retval SEC_UINT32 If the hardware mode is set
[SEC_HW_ERR_INVALID_OPERATION|N/A].
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* - CRYPT_randSeed, CRYPT_getRandState, CRYPT_setRandState, CRYPT_saveRandState, CRYPT_loadRandState,
*    CRYPT_addEntropy, CRYPT_testRand and random number generation should not be called in parallel
* \n
* N/A
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_addEntropy(const SEC_UCHAR *pucEntropyBuf, SEC_UINT32 ulLen);

/*
   Func Name: CRYPT_testRand
*/
/**
* @defgroup CRYPT_testRand
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_testRand (
* SEC_UCHAR* nums,
* SEC_UINT32 len);
* @endcode
*
* @par Purpose
* This is used to test the randomness of a number. This API is deprecated , always return failure.
*
* @par Description
* CRYPT_testRand function tests the randomness of a number. This API is deprecated , always return failure.
*
* @param[in] nums The number whose randomness is to be tested [N/A]
* @param[in] len The size of the number in bytes [N/A]
*
* @retval SEC_UINT32 If randomness is sufficient [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 If randomness is insufficient
or any other error [SEC_ERROR|N/A]
* @retval SEC_UINT32 If the hardware mode is set
[SEC_HW_ERR_INVALID_OPERATION|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* - CRYPT_randSeed, CRYPT_getRandState, CRYPT_setRandState, CRYPT_saveRandState, CRYPT_loadRandState,
*    CRYPT_addEntropy, CRYPT_testRand and random number generation should not be called in parallel
* N/A
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_testRand(SEC_UCHAR *nums, SEC_UINT32 len);

/*
   Func Name: CRYPT_SYM_keyLen
*/
/**
* @defgroup CRYPT_SYM_keyLen
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_SYM_keyLen(
* SEC_UINT32 ulAlgId)
* @endcode
*
* @par Purpose
* This is used to retrieve the key length of an algorithm
*
* @par Description
* CRYPT_SYM_keyLen function retrieves the key length of
an algorithm. If the given algorithm type
* is not supported, it returns 0.
*
* @param[in] ulAlgId Symmetric algorithm id [N/A]
*
* @retval SEC_UINT32 Key length [N/A|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* -As Blowfish supports variable key length (32-448 bits), CRYPT_SYM_keyLen returns 16 bytes as it is widely used.\n
* -As RC4 supports variable key length, CRYPT_SYM_keyLen returns 16 bytes as it is widely used.
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_SYM_keyLen(SEC_UINT32 ulAlgId);

/*
    Func Name: CRYPT_SYM_blockSize
*/
/**
* @defgroup CRYPT_SYM_blockSize
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_SYM_blockSize(
* SEC_UINT32 ulAlgId)
* @endcode
*
* @par Purpose
* This is used to retrieve the block size of an algorithm.
*
* @par Description
* CRYPT_SYM_blockSize function retrieves the block size of
an algorithm. If the given algorithm type is not supported,
* it returns 0.
*
* @param[in] ulAlgId Symmetric algorithm ID [N/A]
*
* @retval SEC_UINT32 Block size of algorithm [Block size|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* N/A
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_SYM_blockSize(SEC_UINT32 ulAlgId);

/*
    Func Name:  CRYPT_SYM_ivLen
*/
/**
* @defgroup CRYPT_SYM_ivLen
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_SYM_ivLen(
* SEC_UINT32 ulAlgId)
* @endcode
*
* @par Purpose
* This is used to retrieve the IV length of an algorithm.
*
* @par Description
* CRYPT_SYM_ivLen function retrieves the IV length of an algorithm.
If the given algorithm type is not supported, it
* returns 0.
*
* @param[in] ulAlgId Symmetric algorithm ID [N/A]
*
* @retval SEC_UINT32 IV length of algorithm [IV length|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* N/A
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_SYM_ivLen(SEC_UINT32 ulAlgId);

/*
    Func Name:  CRYPT_SYM_padType
*/
/**
* @defgroup CRYPT_SYM_padType
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_SYM_padType(
* SEC_UINT32 ulAlgId)
* @endcode
*
* @par Purpose
* This is used to retrieve the Padding type used by an algorithm.
*
* @par Description
* CRYPT_SYM_padType function retrieves the Padding type used by an
algorithm. This function returns the symmetric padding
* type. If the given algorithm type is not supported, it returns 0.
*
* @param[in] ulAlgId Symmetric algorithm ID. The padding type is
written on the higher 16 bytes of the algorithm [N/A]
*
* @retval SEC_UINT32 If invalid algorithm ID [SEC_ERR_INVALID_ALGID|N/A]
* @retval SEC_UINT32 If undefined padding
[SEC_CRYPT_ERR_UNDEFINED_PADDING|N/A]
* @retval SEC_UINT32 If algorithm doesnot support padding [0|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* N/A
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_SYM_padType(SEC_UINT32 ulAlgId);

/*
    Func Name:  CRYPT_setFeedbackSize
*/
/**
* @defgroup CRYPT_setFeedbackSize
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_setFeedbackSize (
* CRYPT_CTX ctx,
* SEC_UINT32 ulFeedbackSize);
* @endcode
*
* @par Purpose
* This is used to set the feedback length.
*
* @par Description
* CRYPT_setFeedbackSize function sets the feedback length in bytes.
It is used in CFB and OFB modes.
*
* @param[in] ulFeedbackSize The feedback length to be set [N/A]
* @param[out] ctx Initialized symmetric context [N/A]
*
* @retval SEC_UINT32 On successful return [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 On failure [SEC_ERR_INVALID_ARG|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* N/A
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_setFeedbackSize(CRYPT_CTX ctx, SEC_UINT32 ulFeedbackSize);

/*
    Func Name:  CRYPT_getFeedbackSize
*/
/**
* @defgroup CRYPT_getFeedbackSize
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_getFeedbackSize (CRYPT_CTX ctx);
* @endcode
*
* @par Purpose
* This is used to get the feedback length.
*
* @par Description
* CRYPT_getFeedbackSize function retrieves the segment size,
which is used for feedback, in bytes. It is used in CFB
* and OFB modes.
*
* @param[in] ctx Initialized symmetric context [N/A]
*
* @retval SEC_UINT32 When used for OFB and CFB modes [Feedbacksize|N/A]
* @retval SEC_UINT32 When used for CBC and ECB modes [0|N/A]
* @retval SEC_UINT32 On failure [SEC_ERR_INVALID_ARG|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* N/A
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_getFeedbackSize(CRYPT_CTX ctx);

/*
    Func Name:  CRYPT_setEffectiveKeyBits
*/
/**
 * @defgroup CRYPT_setEffectiveKeyBits
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_setEffectiveKeyBits(
 * CRYPT_CTX ctx,
 * SEC_INT iEffKeyBits,
 * const SEC_UCHAR * pucKey,
 * SEC_UINT32 ulKeyLen)
 * @endcode
 *
 * @par Purpose
 * Currently sets the Effective Key bits for only RC2 block cipher and
 * generates a new Key based on the effective key bits set.
 *
 * @par Description
 * CRYPT_setEffectiveKeyBits function currently sets the Effective Key bits
 * for only RC2 block cipher and generates a new Key based on
 * the effective key bits set. For RC2, if the Effective key
 * bits set by the user is more than 1024 or less than zero or
 * equal to zero then the Effective Key bits will be
 * considered as equal to 1024. This API should be called
 * once the CRYPT_encryptInit or CRYPT_decryptInit is called.
 *
 * @param[in] pucKey The key value to be used [N/A]
 * @param[in] ulKeyLen The length of the key [N/A]
 * @param[out] ctx Address of the context in which the new Key will be set[N/A]
 * @param[out] iEffKeyBits The effective key bits need to be set[N/A]
 *
 * @retval SEC_UINT32 Invalid argument [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 Invalid Keylength for alg [SEC_ERR_INVALID_KEY_LEN|N/A]
 * @retval SEC_UINT32 Malloc failure [SEC_ERR_MALLOC_FAIL|N/A]
 * @retval SEC_UINT32 On success [SEC_SUCCESS|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_setEffectiveKeyBits(CRYPT_CTX ctx, SEC_INT iEffKeyBits, const SEC_UCHAR *pucKey,
    SEC_UINT32 ulKeyLen);

/*
    Func Name:  CRYPT_SYM_mode
*/
/**
* @defgroup CRYPT_SYM_mode
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_SYM_mode(
* SEC_UINT32 ulAlgId)
* @endcode
*
* @par Purpose
* This is used to get mode which is used in algorithm.
*
* @par Description
* CRYPT_SYM_mode function returns working mode(ECB,CBC,OFB,CFB)
for symmetric algorithm.If given algorithm type is not
* supported, return INVALID_SYMALG_MODE.
*
* @param[in] ulAlgId Symmetric algorithm ID [N/A]
*
* @retval SEC_UINT32 If algorithm Id is valid
[Symmetric working mode(CBC, ECB, CFB, OFB)|N/A]
* @retval SEC_UINT32 If algorithm Id is invalid [INVALID_SYMALG_MODE|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* N/A
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_SYM_mode(SEC_UINT32 ulAlgId);

/*
    Func Name:  CRYPT_name
*/
/**
 * @defgroup CRYPT_name
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_CHAR* CRYPT_name(
 * SEC_UINT32 ulAlgId)
 * @endcode
 *
 * @par Purpose
 * This is used to get name of algorithm.
 *
 * @par Description
 * CRYPT_name function returns name of  algorithm on the basis of algorithm Id.
 *
 * @param[in] ulAlgId The algorithm Id [N/A]
 *
 * @retval SEC_CHAR* If Invalid algorithm Id [SEC_NULL|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_CHAR *CRYPT_name(SEC_UINT32 ulAlgId);

/*
    Func Name:  CRYPT_MD_size
*/
/**
* @defgroup CRYPT_MD_size
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_MD_size(
* SEC_UINT32 ulAlgType)
* @endcode
*
* @par Purpose
* This is used to retrieve the size of the hash output for an algorithm.
*
* @par Description
* CRYPT_MD_size function retrieves the size of the hash
output for an algorithm.
*
* @param[in] ulAlgType Algorithm ID for digest [N/A]
*
* @retval SEC_UINT32 If valid algorithm Id [Size of the digest|N/A]
* @retval SEC_UINT32 If undefined algorithm [0|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
*
* N/A
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_MD_size(SEC_UINT32 ulAlgType);

/*
    Func Name:  CRYPT_MD_blockSize
*/
/**
 * @defgroup CRYPT_MD_blockSize
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_MD_blockSize(
 * SEC_UINT32 ulAlgType)
 * @endcode
 *
 * @par Purpose
 * This is used to retrieve the block size of the hash algorithm.
 *
 * @par Description
 * CRYPT_MD_blockSize function retrieves the block size of the hash algorithm.
 *
 * @param[in] ulAlgType Algorithm ID for Digest [N/A]
 *
 * @retval SEC_UINT32 If algorithm is valid [Block size of the algorithm|N/A]
 * @retval SEC_UINT32 If undefined algorithm [0|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_MD_blockSize(SEC_UINT32 ulAlgType);

/*
    Func Name:  CRYPT_HMAC_hashType
*/
/**
 * @defgroup CRYPT_HMAC_hashType
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_HMAC_hashType(
 * SEC_UINT32 ulAlgType)
 * @endcode
 *
 * @par Purpose
 * This is used to retrieve the hash algorithm used with the HMAC.
 *
 * @par Description
 * CRYPT_HMAC_hashType function retrieves the hash algorithm used with the HMAC.
 *
 * @param[in] ulAlgType Algorithm ID for HMAC [N/A]
 *
 * @retval SEC_UINT32 If algorithm is valid [Algorithm ID of hash algorithm|N/A]
 * @retval SEC_UINT32 If undefined algorithm [ALGID_UNKNOWN|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_HMAC_hashType(SEC_UINT32 ulAlgType);

/*
    Func Name:  CRYPT_HMAC_size
*/
/**
* @defgroup CRYPT_HMAC_size
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_HMAC_size(
* SEC_UINT32 ulAlgType)
* @endcode
*
* @par Purpose
* This is used to retrieve the size of the MAC output for an algorithm.
*
* @par Description
* CRYPT_HMAC_size function retrieves the size of the MAC output
for an algorithm.
*
* @param[in] ulAlgType Algorithm ID for HMAC [N/A]
*
* @retval SEC_UINT32 If valid algorithm [Size of the MAC|N/A]
* @retval SEC_UINT32 If Undefined algorithm [0|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* N/A
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_HMAC_size(SEC_UINT32 ulAlgType);

/*
    Func Name:  CRYPT_CMAC_size
*/
/**
* @defgroup CRYPT_CMAC_size
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_CMAC_size(
* SEC_UINT32 ulAlgType)
* @endcode
*
* @par Purpose
* This is used to retrieve the size of the MAC output for an algorithm.
*
* @par Description
* CRYPT_CMAC_size function retrieves the size of the MAC output
for an algorithm.
*
* @param[in] ulAlgType Algorithm ID for CMAC [N/A]
*
* @retval SEC_UINT32 If valid algorithm [Size of the MAC|N/A]
* @retval SEC_UINT32 If Undefined algorithm [0|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* N/A
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_CMAC_size(SEC_UINT32 ulAlgType);

/*
    Func Name:  CRYPT_getHashFromSignId
*/
/**
* @defgroup CRYPT_getHashFromSignId
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT CRYPT_getHashFromSignId (
* SEC_UINT uiSignAlg);
* @endcode
*
* @par Purpose
* This is used to extract the Hash algorithm AlgId corresponding
to a given signing AlgId.
*
* @par Description
* CRYPT_getHashFromSignId function extracts the Hash algorithm
AlgId corresponding to a given signing AlgId.
*
* @param[in] uiSignAlg The signing Algorithm Id [N/A]
*
* @retval SEC_UINT If sign algorithm is valid [The hash algorithm
id corresponding to the signing id|N/A]
* @retval SEC_UINT If unknown algorithm id [ALGID_UNKNOWN|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* In case of ALGID_ED25519 it wil return ALGID_SHA512. ED Signature algorithm doenst have hash id, but internally
* uses ALGID_SHA512, so API return it to avoid failure when signing.
* This function is used by the SSL library.
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT CRYPT_getHashFromSignId(SEC_UINT uiSignAlg);

/*
    Func Name:  CRYPT_getAsymFromSignId
*/
/**
* @defgroup CRYPT_getAsymFromSignId
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT CRYPT_getAsymFromSignId (
* SEC_UINT uiSignAlg);
* @endcode
*
* @par Purpose
* This is used to extract the Asymmetric algorithm AlgId
corresponding to a given signing AlgId.
*
* @par Description
* CRYPT_getAsymFromSignId function extracts the Asymmetric
algorithm AlgId corresponding to a given signing AlgId.
*
* @param[in] uiSignAlg The signing AlgId [N/A]
*
* @retval SEC_UINT If algorithm IDs are valid [Asymmetric
algid corresponding to the signing id|N/A]
* @retval SEC_UINT For unknown AlgId [ALGID_UNKNOWN|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* This function is used by the SSL library.
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT CRYPT_getAsymFromSignId(SEC_UINT uiSignAlg);

/*
    Func Name:  CRYPT_getSignFromHashAndAsymId
*/
/**
* @defgroup CRYPT_getSignFromHashAndAsymId
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT CRYPT_getSignFromHashAndAsymId (
* SEC_UINT uiAsymAlg,
* SEC_UINT uiHashAlg);
* @endcode
*
* @par Purpose
* This is used to build the signing algid from asymmetric
and hash algorithm ids.
*
* @par Description
* CRYPT_getSignFromHashAndAsymId function builds the signing
algid from asymmetric and hash algorithm ids.
*
* @param[in] uiAsymAlg The asymmetric algorithm Id [N/A]
* @param[in] uiHashAlg The hash algorithm Id [N/A]
*
* @retval SEC_UINT If algorithm ids are valid
[The signing algorithm id corresponding to the asymmetric id and hash id|N/A]
* @retval SEC_UINT If unknown algorithm Id [ALGID_UNKNOWN|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* N/A
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT CRYPT_getSignFromHashAndAsymId(SEC_UINT uiAsymAlg, SEC_UINT uiHashAlg);

/* Next Five functions are related to BigInt representatiosn */
/*
* Func Name:  SEC_assignToBigInt
* @defgroup SEC_assignToBigInt
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_INT SEC_assignToBigInt(
* const SEC_UINT intVal,
* SEC_BIGINT_S* pData)
* @endcode
*
* @par Purpose
* This is used to assign small values to the big integer structure.
*
* @par Description
* SEC_assignToBigInt function assigns small values to the
big integer structure.
*
* @param[in] intVal The integer to be converted [N/A]
* @param[out] pData The big integer to be initialized [N/A]
*
* @retval SEC_INT On successful conversion [SEC_SUCCESS|N/A]
* @retval SEC_INT If conversion fails [SEC_ERROR|N/A]
* @retval SEC_UINT32 memcpy_s operation failed [SEC_ERR_MEMCPY_FAILED|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* N/A
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_INT SEC_assignToBigInt(const SEC_UINT intVal, SEC_BIGINT_S *pData);

/*
    Func Name:  SEC_cmpBigInt
*/
/**
 * @defgroup SEC_cmpBigInt
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_INT SEC_cmpBigInt(
 * SEC_BIGINT_S* pA,
 * SEC_BIGINT_S* pB)
 * @endcode
 *
 * @par Purpose
 * This is used to compare two bigint values.
 *
 * @par Description
 * SEC_cmpBigInt function compares two bigint values.
 *
 * @param[in] pA The first bigint [N/A]
 * @param[in] pB The second bigint [N/A]
 *
 * @retval SEC_INT If pA is greater than pB [Greater than zero|N/A]
 * @retval SEC_INT If pB is greater than pA [Lesser than zero|N/A]
 * @retval SEC_INT If pB is equal to pA [Equal to zero|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT SEC_cmpBigInt(SEC_BIGINT_S *pA, SEC_BIGINT_S *pB);

/*
    Func Name:  SEC_cpyBigInt
*/
/**
 * @defgroup SEC_cpyBigInt
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 SEC_cpyBigInt(
 * SEC_BIGINT_S* pstDestBigInt,
 * const SEC_BIGINT_S* pstSrcBigInt)
 * @endcode
 *
 * @par Purpose
 * This is used to copy one bigint value into another.
 *
 * @par Description
 * SEC_cpyBigInt function copies one bigint value into another.
 *
 * @param[in] pstSrcBigInt The source bigint to be copied [N/A]
 * @param[out] pstDestBigInt The destination bigint [N/A]
 *
 * @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 Invalid Arguments [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 memcpy_s operation failed [SEC_ERR_MEMCPY_FAILED|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 SEC_cpyBigInt(SEC_BIGINT_S *pstDestBigInt, const SEC_BIGINT_S *pstSrcBigInt);

/*
    Func Name:  SEC_cvtToBigInt
*/
/**
* @defgroup SEC_cvtToBigInt
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_INT SEC_cvtToBigInt(
* SEC_CHAR* pHexStr,
* SEC_BIGINT_S* pData)
* @endcode
*
* @par Purpose
* This is used to convert the given hex string to octet representation.
*
* @par Description
* SEC_cvtToBigInt function converts the given hex string to
octet representation. The input string is expected in two's
* complement form. The input string may or may not have a
'0x'as a prefix. For eg: "0x12" and "12" mean the same.
*
* @param[in] pHexStr The hex character string.
* The hex character string should be less than 10MB excluding the NULL character and 0x [N/A]
* @param[out] pData The big integer to be initialized [N/A]
*
* @retval SEC_INT On Successful conversion [SEC_SUCCESS|N/A]
* @retval SEC_INT If conversion fails [SEC_ERROR|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
*  - Eg: SEC_cvtToBigInt ("0x1234aE", &stOcts).\n
* Above call will return stOcts with {3, (int (*)[3])&{\x12\x34\xaE}}.\n
* "0xFF" is a negative number in 8 bits and will be stored as "FF".\n
* "0xFFF" is a positive number in 16 bits and will be stored as "0FFF".\n
* To store "0xFFF" as a negative value give "0xFFFF" (padding leading
4 bits with 1').\n
* "0x0FF" is a positive number in 16 bits and will be stroed as "00FF"
* - The input only having 0x or empty string will not be considered as valid hex string.
*    Hence error will be returned for these inputs.
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_INT SEC_cvtToBigInt(SEC_CHAR *pHexStr, SEC_BIGINT_S *pData);

/*
    Func Name:  SEC_dupBigInt
*/
/**
* @defgroup SEC_dupBigInt
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_BIGINT_S *SEC_dupBigInt(
* const SEC_BIGINT_S* pstBigInt)
* @endcode
*
* @par Purpose
* This is used to create a duplicate bigint.
*
* @par Description
* SEC_dupBigInt function creates a duplicate bigint.
*
* @param[in] pstBigInt The source bigint to be duplicated [N/A]]
*
* @retval SEC_BIGINT_S* On error [0|N/A]
* @retval SEC_BIGINT_S* Otherwise [Pointer to the duplicated bigint|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* Pointer to the duplicated bigint has to be freed by the user
using ipsi_free().
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_BIGINT_S *SEC_dupBigInt(const SEC_BIGINT_S *pstBigInt);

/* context functions */
/*
 * Func Name:  CRYPT_CTX_status
 * @defgroup CRYPT_CTX_status
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_BOOL CRYPT_CTX_status(
 * CRYPT_CTX ctx)
 * @endcode
 *
 * @par Purpose
 * This is used to check if the context is NULL or initialized.
 *
 * @par Description
 * CRYPT_CTX_status function checks if the context is NULL or initialized.
 *
 * @param[in] ctx The symmetric context to be validated [N/A]
 *
 * @retval SEC_BOOL If context is valid [SEC_TRUE|N/A]
 * @retval SEC_BOOL If context is Invalid [SEC_FALSE|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * N/A
 */
/* context status, not exist, initiated or in using */
CRYPTOLINKDLL SEC_BOOL CRYPT_CTX_status(CRYPT_CTX ctx);

/*
    Func Name:  CRYPT_isValidAlgId
*/
/**
* @defgroup CRYPT_isValidAlgId
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_BOOL CRYPT_isValidAlgId(SEC_UINT32 ulAlgId,SEC_ALG_CATEGORY_E enAlgCat)
* @endcode
*
* @par Purpose
* This is used to specify if the given algorithm is a valid algorithm or not.
*
* @par Description
* CRYPT_isValidAlgId function specifies that the given
algorithm is a valid algorithm in the given category of crypto
* algorithms.
*
* @param[in] ulAlgId The algorithm Id [N/A
* @param[in] enAlgCat The type(category)of crypto algorithms [N/A]
*
* @retval SEC_BOOL If algorithm is valid [SEC_TRUE|N/A]
* @retval SEC_BOOL If algorithm is Invalid or library is not
initialized [SEC_FALSE|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* N/A
*
* @par Related Topics
* N/A
*/
/* checks if the given AlgId is a valid algId of the given AlgCategory */
CRYPTOLINKDLL SEC_BOOL CRYPT_isValidAlgId(SEC_UINT32 ulAlgId, SEC_ALG_CATEGORY_E enAlgCat);

/*
    Func Name:  CRYPT_saveRandState
*/
/**
* @defgroup CRYPT_saveRandState
* @ingroup HLF High Level Functions
* @image html Security.jpg
* <b> This API is not recommended when the access permission of the file needs to be configure.
*  Instead user should get random state using CRYPT_getRandState API and save the random state in file
*  with the required product access permission. </b>
* @par Prototype
* @code
* SEC_INT CRYPT_saveRandState (const SEC_CHAR *file);
* @endcode
*
* @par Purpose
* This is used to save the random number generator internal
state to a given file.
*
* @par Description
* CRYPT_saveRandState function saves the random number generator
internal state to a given file.
*
* @param[in] file The file name [N/A]
*
* @retval SEC_INT If save is successful [SEC_SUCCESS|N/A]
* @retval SEC_INT If file operation(open, write or close) failed or NULL argument passed [SEC_ERROR|N/A]
* @retval SEC_INT If the hardware mode is set
[SEC_HW_ERR_INVALID_OPERATION|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
*   - This function tries to save the rand state to the file name provided. But, this API doesn't set any specific
*     permissions for the file. If the file is being newly created, then it will have only default permissions.
*     The permissions for this file (whether newly created or existing one) is totally dependent on the behavior
*     of the file system file open APIs (like fopen). If users, want to have restricted access permissions for the
*     Rand State file, then they can use the CRYPT_getRandState API and get the rand state directly and store
*     it in a file of their choice with appropriate access permissions.
*   - No specific validation is done for the filename or the path of the filename passed through the argument
*     "file" to this API. User should themselves handle path canonicalization and validation of path and filename
*     before passing it to this API. Users are strongly suggested to use CRYPT_getRandState API instead of this
*     API and save the rand state to a file of their choice.
*   - CRYPT_randSeed, CRYPT_getRandState, CRYPT_setRandState, CRYPT_saveRandState, CRYPT_loadRandState,
*    CRYPT_addEntropy, CRYPT_testRand and random number generation should not be called in parallel
*
* \n
* N/A
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_INT CRYPT_saveRandState(const SEC_CHAR *file);

/*
    Func Name:  CRYPT_loadRandState
*/
/**
* @defgroup CRYPT_loadRandState
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_INT CRYPT_loadRandState (const SEC_CHAR *file);
* @endcode
*
* @par Purpose
* This is used to load a saved random number genrator
internal state from a given file.
*
* @par Description
* CRYPT_loadRandState function loads a saved random number
genrator internal state from a given file.
*
* @param[in] file The file name [N/A]
*
* @retval SEC_INT If load is successful [SEC_SUCCESS|N/A]
* @retval SEC_INT If file operation(open, read or close) failed, empty file or NULL argument passed  [SEC_ERROR|N/A]
* @retval SEC_INT If the hardware mode is set
[SEC_HW_ERR_INVALID_OPERATION|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* - CRYPT_randSeed, CRYPT_getRandState, CRYPT_setRandState, CRYPT_saveRandState, CRYPT_loadRandState,
*    CRYPT_addEntropy, CRYPT_testRand and random number generation should not be called in parallel
* \n
* N/A
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_INT CRYPT_loadRandState(const SEC_CHAR *file);

/*
    Func Name:  CRYPT_libraryInit
*/
/**
 * @defgroup CRYPT_libraryInit
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_libraryInit()
 * @endcode
 *
 * @par Purpose
 * This is used to initialize the cryptographic library.
 *
 * @par Description
 * CRYPT_libraryInit function initializes the cryptographic library.
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_UINT32 If library initialization is successful [SEC_SUCCESS|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * \n
 * CRYPT_libraryInit is not thread safe API. This API should not be called in multithreads.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_libraryInit(void);

/*
 Func Name:  CRYPT_keyWrap
*/
/**
* @defgroup CRYPT_keyWrap
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_BOOL CRYPT_keyWrap(
* SEC_UCHAR* ciphertxt,
* SEC_UINT32 uiCiphertxtLen,
* const SEC_UCHAR* puckey,
* SEC_UINT32 ulKlen,
* const SEC_UCHAR* plaintext,
* SEC_UINT32 plaintextLen,
* const SEC_UCHAR* iv,
* SEC_UINT32 npt);
* @endcode
*
* @par Purpose
* This is used to generate an encrypted key.
*
* @par Description
* CRYPT_keyWrap function generats an encrypted key using an original key.
*
* @param[in] puckey Pointer to the key [N/A]
* @param[in] ulKlen The size of key in octets. The size
must be 16,24 or 32 [N/A]
* @param[in] plaintext Pointer to the plain text [N/A]
* @param[in] iv Pointer to initial value [N/A]
* @param[in] npt A block number of key data plain text [N/A]
* @param[out] ciphertxt Pointer to the cipher text [N/A]
* @param[in] plaintextLen size of plaintext buffer [N/A]
* @param[in] uiCiphertxtLen size of ciphertxt buffer [N/A]
*
* @retval SEC_BOOL On failure [SEC_FALSE|N/A]
* @retval SEC_BOOL On success [SEC_TRUE|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* 1. The length of the plaintext and ciphertext must be the multiple of 8 byte blocks,
*    whereas for (n) blocks of plaintext there must be (n+1) blocks of ciphertext. \n
* 2. The npt parameter value must not exceed 0xfffffff value,otherwise it will fail to
*    generate an encrypted key.
* - The safe API corresponding to this is CRYPT_keyWrap_sf
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_BOOL CRYPT_keyWrap(SEC_UCHAR *ciphertxt, SEC_UINT32 uiCiphertxtLen, const SEC_UCHAR *puckey,
    SEC_UINT32 ulKlen, const SEC_UCHAR *plaintext, SEC_UINT32 plaintextLen, const SEC_UCHAR *iv, SEC_UINT32 npt);

/*
 Func Name:  CRYPT_HashCtxCopy
*/
/**
 * @defgroup CRYPT_HashCtxCopy
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_BOOL CRYPT_HashCtxCopy(
 * CRYPT_CTX outCtx,
 * CRYPT_CTX inCtx)
 * @endcode
 *
 * @par Purpose
 * This is used to copy a hash context into another context.
 *
 * @par Description
 * CRYPT_HashCtxCopy function copyies a hash context into another context.
 *
 * @param[in] inCtx Pointer to the input hash context [N/A]
 * @param[out] outCtx Pointer to updated hash context [N/A]
 *
 * @retval SEC_BOOL On failure [SEC_FALSE|N/A]
 * @retval SEC_BOOL On success [SEC_TRUE|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_BOOL CRYPT_HashCtxCopy(CRYPT_CTX outCtx, CRYPT_CTX inCtx);

/*
     Func Name:  CRYPT_HASH_ALG
*/
/**
 * @defgroup CRYPT_HASH_ALG
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_HASH_ALG(CRYPT_CTX Ctx)
 * @endcode
 *
 * @par Purpose
 * This is used to get the hash algorithm from hash context.
 *
 * @par Description
 * CRYPT_HASH_ALG function returns the hash algorithm from hash context.
 *
 * @param[in] Ctx Hash context [N/A]
 *
 * @retval SEC_UINT32 Hash Algorithm Id [Hash Algorithm Id|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_HASH_ALG(CRYPT_CTX Ctx);

/*
 Func Name:  CRYPT_keyUnwrap
*/
/**
* @defgroup CRYPT_keyUnwrap
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_BOOL CRYPT_keyUnwrap(
* SEC_UCHAR* plaintext,
* SEC_UINT32 plaintextLen,
* const SEC_UCHAR* puckey,
* SEC_UINT32 ulKlen,
* const SEC_UCHAR* ciphertxt,
* SEC_UINT32 uiCiphertxtLen,
* const SEC_UCHAR* iv,
* SEC_UINT32 npt);
* @endcode
*
* @par Purpose
* This is used to generate an original key using an encrypted key.
*
* @par Description
* CRYPT_keyUnwrap function generats an original key using an encrypted key.
*
* @param[in] puckey Pointer to the key [N/A]
* @param[in] ulKlen The size of key in octets.
The size must be 16,24 or 32 [N/A]
* @param[in] ciphertxt Pointer to the cipher text [N/A]
* @param[in] iv Pointer to initial value [N/A]
* @param[in] npt A block number of key data plain text [N/A]
* @param[out] plaintext Pointer to the plain text [N/A]
* @param[in] plaintextLen size of plaintext buffer [N/A]
* @param[in] uiCiphertxtLen size of ciphertxt buffer [N/A]
*
* @retval SEC_BOOL On failure [SEC_FALSE|N/A]
* @retval SEC_BOOL On success [SEC_TRUE|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* 1. The length of the plaintext and ciphertext must be the multiple of 8 byte blocks,
*    whereas for (n) blocks of plaintext there must be (n+1) blocks of ciphertext. \n
* 2. The npt parameter value must not exceed 0xfffffff value,otherwise it will fail to
*    generate an original key using an excrypted.
* - The safe API corresponding to this is CRYPT_keyUnwrap_sf
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_BOOL CRYPT_keyUnwrap(SEC_UCHAR *plaintext, SEC_UINT32 plaintextLen, const SEC_UCHAR *puckey,
    SEC_UINT32 ulKlen, const SEC_UCHAR *ciphertxt, SEC_UINT32 uiCiphertxtLen, const SEC_UCHAR *iv, SEC_UINT32 npt);

/*
 Func Name:  CRYPT_keyWrapCipher
*/
/**
* @defgroup CRYPT_keyWrapCipher
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_BOOL CRYPT_keyWrapCipher(
* SEC_UINT32 ulAlgType,
* SEC_UCHAR* ciphertxt,
* SEC_UINT32 uiCiphertxtLen,
* const SEC_UCHAR* puckey,
* SEC_UINT32 ulKlen,
* const SEC_UCHAR* plaintext,
* SEC_UINT32 plaintextLen,
* const SEC_UCHAR* iv,
* SEC_UINT32 npt);
* @endcode
*
* @par Purpose
* This is used to generate an encrypted key.
*
* @par Description
* CRYPT_keyWrapCipher function generate an encrypted key using an key encrypting key.
*
* @param[in] ulAlgType Key Wrap algorithm type.
Supprt type: ALGID_KWRAP_AES or ALGID_KWRAP_SM4 [N/A]
* @param[out] ciphertxt Pointer to the cipher text [N/A]
* @param[in] uiCiphertxtLen size of ciphertxt buffer [N/A]
* @param[in] puckey Pointer to the key [N/A]
* @param[in] ulKlen The size of key in octets. The size must be 16 [N/A]
* @param[in] plaintext Pointer to the plain text [N/A]
* @param[in] plaintextLen size of plaintext buffer [N/A]
* @param[in] iv Pointer to initial value [N/A]
* @param[in] npt A block number of key data plain text [N/A]
*
* @retval SEC_BOOL On failure [SEC_FALSE|N/A]
* @retval SEC_BOOL On success [SEC_TRUE|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* 1. The length of the plaintext and ciphertext must be the multiple of 8 byte blocks,
*    whereas for (n) blocks of plaintext there must be (n+1) blocks of ciphertext. \n
* 2. The npt parameter value must not exceed 0xfffffff value,otherwise it will fail to
*    generate an encrypted key.
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_BOOL CRYPT_keyWrapCipher(SEC_UINT32 ulAlgType, SEC_UCHAR *ciphertxt, SEC_UINT32 uiCiphertxtLen,
    const SEC_UCHAR *puckey, SEC_UINT32 ulKlen, const SEC_UCHAR *plaintext, SEC_UINT32 plaintextLen,
    const SEC_UCHAR *iv, SEC_UINT32 npt);

/*
 Func Name:  CRYPT_keyUnwrapCipher
*/
/**
* @defgroup CRYPT_keyUnwrapCipher
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_BOOL CRYPT_keyUnwrapCipher(
* SEC_UINT32 ulAlgType,
* SEC_UCHAR* plaintext,
* SEC_UINT32 plaintextLen,
* const SEC_UCHAR* puckey,
* SEC_UINT32 ulKlen,
* const SEC_UCHAR* ciphertxt,
* SEC_UINT32 uiCiphertxtLen,
* const SEC_UCHAR* iv,
* SEC_UINT32 npt);
* @endcode
*
* @par Purpose
* This is used to generate an original key using an encrypted key.
*
* @par Description
* CRYPT_keyUnwrapCipher function generate an original key using an key encrypting key.
*
* @param[in] ulAlgType Key Unwrap algorithm type.
Supprt type: ALGID_KWRAP_AES or ALGID_KWRAP_SM4 [N/A]
* @param[out] plaintext Pointer to the plain text [N/A]
* @param[in] plaintextLen size of plaintext buffer [N/A]
* @param[in] puckey Pointer to the key [N/A]
* @param[in] ulKlen The size of key in octets. The size must be 16 [N/A]
* @param[in] ciphertxt Pointer to the cipher text [N/A]
* @param[in] uiCiphertxtLen size of ciphertxt buffer [N/A]
* @param[in] iv Pointer to initial value [N/A]
* @param[in] npt A block number of key data plain text [N/A]
*
* @retval SEC_BOOL On failure [SEC_FALSE|N/A]
* @retval SEC_BOOL On success [SEC_TRUE|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* 1. The length of the plaintext and ciphertext must be the multiple of 8 byte blocks,
*    whereas for (n) blocks of plaintext there must be (n+1) blocks of ciphertext. \n
* 2. The npt parameter value must not exceed 0xfffffff value,otherwise it will fail to
*    generate an original key using an excrypted.
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_BOOL CRYPT_keyUnwrapCipher(SEC_UINT32 ulAlgType, SEC_UCHAR *plaintext, SEC_UINT32 plaintextLen,
    const SEC_UCHAR *puckey, SEC_UINT32 ulKlen, const SEC_UCHAR *ciphertxt, SEC_UINT32 uiCiphertxtLen,
    const SEC_UCHAR *iv, SEC_UINT32 npt);

/*
    Func Name:  CRYPT_ctxdigestFinal
*/
/**
* @defgroup CRYPT_ctxdigestFinal
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_ctxdigestFinal (
* const CRYPT_CTX*  pCtx,
* SEC_UCHAR*  pucDigest,
* SEC_UINT32* pulDigestLen,
* SEC_UINT32 ulInpDigestLen);
* @endcode
*
* @par Purpose
* This is used to complete digest operation on remaining data, and is
* called at the end of digest operation.
*
* @par Description
* CRYPT_ctxdigestFinal function completes digest operation on remaining data,
* and is called at the end of digest operation.
* A digestInit must have been called before calling this function. This
function calculates the digest and doesn't free the
* context. The memory for digest must already have been allocated.
*
* @param[in] pCtx The context for digest [N/A]
* @param[out] pucDigest The digest [N/A]
* @param[out] pulDigestLen The digest length [N/A]
* @param[in] ulInpDigestLen The pucDigest buffer size [N/A]
*
* @retval SEC_UINT32 If digest is calculated [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 If context values are not correct
[SEC_CRYPT_ERR_INVALID_CTX|N/A]
* @retval SEC_UINT32 If arguments are invalid[SEC_ERR_INVALID_ARG|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* N/A
*
* @par Related Topics
*	- CRYPT_digestReset
*	- CRYPT_digestFree
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_ctxdigestFinal(const CRYPT_CTX *pCtx, SEC_UCHAR *pucDigest, SEC_UINT32 *pulLen,
    SEC_UINT32 ulInpDigestLen);

/*
    Func Name:  CRYPT_digestReset
*/
/**
* @defgroup CRYPT_digestReset
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_digestReset (
* CRYPT_CTX  pCtx);
* @endcode
*
* @par Purpose
* This is used to reset hash context to initial state.
*
* @par Description
* CRYPT_digestReset function resets hash context to initial state.
* A digestInit must have been called before calling this function.
*
* @param[in] pCtx The context for digest [N/A]

* @retval SEC_UINT32 If reset  is successful [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 If arguments are invalid [SEC_ERR_INVALID_ARG|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* N/A
*
* @par Related Topics
*	- CRYPT_digestFree
*	- CRYPT_ctxdigestFinal
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_digestReset(CRYPT_CTX pCtx);

/*
    Func Name:  CRYPT_digestFree
*/
/**
* @defgroup CRYPT_digestFree
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_VOID CRYPT_digestFree (
* CRYPT_CTX  *pCtx);
* @endcode
*
* @par Purpose
* This is used to free hash contex.
*
* @par Description
* CRYPT_digestFree function frees hash context.
* A digestInit must have been called before calling this function.
*
* @param[in] pCtx The context for digest [N/A]

* @retval SEC_VOID This function does not return any value [N/A|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* N/A
*
* @par Related Topics
*	- CRYPT_digestReset
*	- CRYPT_ctxdigestFinal
*/
CRYPTOLINKDLL SEC_VOID CRYPT_digestFree(CRYPT_CTX *pCtx);

/*
    Func Name:  CRYPT_symInitSession
*/
/**
* @defgroup CRYPT_symInitSession
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_symInitSession(CRYPT_SYM_CTX *pCtx, IPSI_SYM_SETUP_DATA_S *pstSetUpInfo, IPSI_AUX_PAR_S *pstAuxPar)
* @endcode
*
* @par Purpose
* This is used to initialize the symmetric encryption/decryption session context structure.
*
* @par Description
* CRYPT_symInitSession used to initialize the symmetric encryption/decryption session context structure
* with the algorithm, key, key length and cipher direction (encrypt or decrypt).
*
* @param[in]  pCtx Pointer to symmetric encryption/decryption session context [N/A]
* @param[in]  pstSetUpInfo Pointer to symmetric encryption/decryption initialization information [N/A]
* @param[in]  pstAuxPar Pass as SEC_NULL. Additional variable for future extensibility [N/A]
*
* @retval SEC_UINT32 Invalid argument [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 Invalid algorithm identifier [SEC_ERR_INVALID_ALGID|N/A]
* @retval SEC_UINT32 Memory allocation failure [SEC_ERR_MALLOC_FAIL|N/A]
* @retval SEC_UINT32 Invalid key length [SEC_ERR_INVALID_KEY_LEN|N/A]

* @par Required Header File
* sec_crypto.h
*
* @par Note
*
* - SEC_ERR_INVALID_ARG will be returned if:
*    - Input arguments are NULL.
*    - Input key is NULL.
*    - Invalid direction (other than IPSI_SYM_ENCRYPT and IPSI_SYM_DECRYPT for direction in setup information)
* - Padding is not applicable to AES CTR algorithms as there is no restriction on block size for AES CTR algorithm.
* Even if padding mode is set to algorithm id, it will be ignored.
* - SEC_ERR_INVALID_ALGID will be returned if:
*    - If algorithm ID passed is other than ALGID_AES128_CTR, ALGID_AES192_CTR & ALGID_AES256_CTR.
* - SEC_ERR_INVALID_KEY_LEN will be returned if key length passed is not matching with expected key length.
* For ALGID_AES128_CTR, the key length should be 16 bytes, for ALGID_AES192_CTR, the key length should be 24 bytes,
* for ALGID_AES256_CTR, the key length should be 32 bytes.
*
* @par Related Topics
* CRYPT_symOp \n
* CRYPT_symRemoveSession
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_symInitSession(CRYPT_SYM_CTX *pCtx, IPSI_SYM_SETUP_DATA_S *pstSetUpInfo,
    IPSI_AUX_PAR_S *pstAuxPar);

/*
    Func Name:  CRYPT_symOp
*/
/**
 * @defgroup CRYPT_symOp
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_symOp(
 * const IPSI_SYM_OP_DATA_S *pstOpData,
 * const SEC_UCHAR *pucInData,
 * size_t ulInDataLen,
 * SEC_UCHAR *pucOutData,
 * size_t *pulOutDataLen,
 * IPSI_AUX_PAR_S *pstAuxPar);
 * @endcode
 *
 * @par Purpose
 * Used to do encryption / decryption operation on symmetric algorithms. Currently supported algorithms
 * for this API are ALGID_AES128_CTR, ALGID_AES192_CTR & ALGID_AES256_CTR.
 *
 * @par Description
 * CRYPT_symOp used to do encryption / decryption operation. In case of encryption,
 * encrypted data will be generated by this function. In case of decryption, it takes encrypted data
 * as input and outputs decrypted text.Currently supported algorithms
 * for this API are ALGID_AES128_CTR, ALGID_AES192_CTR & ALGID_AES256_CTR.
 *
 * @param[in]  pstOpData Pointer to operation data which holds information
 * required for encryption / decryption [N/A]
 * @param[in]  pucInData Pointer to input data for encryption / decryption operation [N/A]
 * @param[in]  ulInDataLen Input data length [N/A]
 * @param[out]  pucOutData Pointer to output data [N/A]
 * @param[out]  pulOutDataLen Pointer to output data length [N/A]
 * @param[in]  pstAuxPar Pass as SEC_NULL. Additional variable for future extensibility [N/A]
 *
 * @retval SEC_UINT32 Invalid arguments [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 Invalid IV length [SEC_ERR_INVALID_IV_LEN|N/A]
 * @retval SEC_UINT32 Internal error [SEC_ERR|N/A]
 *
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 *
 * - The output buffer to hold cipher text / decrypted text should be allocated by user. The buffer should
 * contain memory of input length passed.
 * - On encryption / decryption success, output data length will be same as input data length.
 * - The data size need not be multiple of block size for CTR algorithms(block size of AES CTR is 16 bytes).
 * - The counter increment is done on whole IV. If application requirements are there such that only
 * some lower bits of IV to be incremented, application should take care that data blocks passed are with
 * that limit. This implementation does not check counter overflow into rest of the IV when incremented.
 * - To reuse the context for further encryption/decryption because of the same key already set to context,
 * API should be called with IPSI_SYM_DATA_TYPE_LAST_PARTIAL with some data.
 * If there is no data to be processed but want to reuse the context with the same key, application can pass
 * some dummy data for encryption/decryption operation & ignore the output data.
 *
 * - SEC_ERR_INVALID_ARG will be returned if:
 * - Context is NULL.
 * - If input data length is zero, any of the pointer of input data, output data, output data length is NULL.
 * - Invalid data type (other than IPSI_SYM_DATA_TYPE_FULL, IPSI_SYM_DATA_TYPE_PARTIAL,
 * IPSI_SYM_DATA_TYPE_LAST_PARTIAL).
 * - IV is NULL.
 * - Pointer to IPSI_SYM_OP_DATA_S is NULL.
 *
 * @par Related Topics
 * CRYPT_symInitSession \n
 * CRYPT_symRemoveSession
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_symOp(const IPSI_SYM_OP_DATA_S *pstOpData, const SEC_UCHAR *pucInData,
    size_t ulInDataLen, SEC_UCHAR *pucOutData, size_t *pulOutDataLen, IPSI_AUX_PAR_S *pstAuxPar);

/*
    Func Name:  CRYPT_symRemoveSession
*/
/**
* @defgroup CRYPT_symRemoveSession
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_VOID CRYPT_symRemoveSession(CRYPT_SYM_CTX ctx)
* @endcode
*
* @par Purpose
* This is used to free the symmetric encryption/decryption session context structure created using CRYPT_symInitSession.
*
* @par Description
* CRYPT_symRemoveSession used to free the encryption/decryption session context structure
* created using CRYPT_symInitSession.
*
* @param[in]  ctx Pointer to symmetric operation session context [N/A]
*
* @par Return values
* N/A

* @par Required Header File
* sec_crypto.h
*
* @par Note
*
* CRYPT_symRemoveSession should be only used to remove the context created with CRYPT_symInitSession.
* If called with context created with any other API, behavior is undefined.
*
* @par Related Topics
* CRYPT_symInitSession \n
* CRYPT_symOp
*/
CRYPTOLINKDLL SEC_VOID CRYPT_symRemoveSession(CRYPT_SYM_CTX ctx);

/*
    Func Name:  CRYPT_symRemoveSession_ex
*/
/**
* @defgroup CRYPT_symRemoveSession_ex
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_VOID CRYPT_symRemoveSession_ex(CRYPT_SYM_CTX *ctx)
* @endcode
*
* @par Purpose
* This is used to free the symmetric encryption/decryption session context structure created using CRYPT_symInitSession.
*
* @par Description
* CRYPT_symRemoveSession_ex used to free the encryption/decryption session context structure
* created using CRYPT_symInitSession.
*
* @param[in]  ctx Address of pointer to symmetric operation session context [N/A]
*
* @par Return values
* N/A

* @par Required Header File
* sec_crypto.h
*
* @par Note
*
* CRYPT_symRemoveSession_ex should be only used to remove the context created with CRYPT_symInitSession.
* If called with context created with any other API, behavior is undefined. It will set the passed pointer to NULL
*
* @par Related Topics
* CRYPT_symInitSession \n
* CRYPT_symOp
*/
CRYPTOLINKDLL SEC_VOID CRYPT_symRemoveSession_ex(CRYPT_SYM_CTX *ctx);

/**
 * @defgroup IPSI_CRYPT_enableRsaSignWithPrvExp
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_VOID IPSI_CRYPT_enableRsaSignWithPrvExp()
 * @endcode
 *
 * @par Purpose
 * This API enables RSA signing with only the private key components.(D, N and E).
 *
 * @par Description
 * This API is used to enable RSA signing with only the private key components.(D, N and E). If RSA signing with private
 * key  component is enabled, the Crypto library will first check whether p, q ,dp, dq and qinv are available in the
 * RSA private key. If available, the signature will be generated with these values, otherwise the signature generation
 * will be done using D , N and E values. By default, this feature is disabled.
 *
 * @par Parameters
 * N/A
 *
 * @retval
 * N/A
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * On using IPSI_CRYPT_enableRsaSignWithPrvExp, the performance of RSA signature generation will be impacted in two
 * ways:
 * -#	When this API is used and all private key parameters p, q ,dp, dq and qInversion are available, the performance
 * is slightly impacted as Crypto needs to check all these parameters are available. If all these parameters are
 * available, then Crypto will use these to precalculate the value for signing.
 * -#	When this API is used and only the parameters D, N and E of the private key parameters are available, then
 * these parameters are used for signing and the performance of signing will be lower than the normal case as
 * signing does not use precalculated values.
 * -#    It is a global function, application need to make sure that the function will not be called in parallel
 * with the RSA operations.
 *
 * @par Related Topics
 * IPSI_CRYPT_disableRsaSignWithPrvExp
 */
CRYPTOLINKDLL SEC_VOID IPSI_CRYPT_enableRsaSignWithPrvExp(void);

/**
* @defgroup IPSI_CRYPT_disableRsaSignWithPrvExp
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_VOID IPSI_CRYPT_disableRsaSignWithPrvExp()
* @endcode
*
* @par Purpose
* This API disables RSA signing with only the private key components.(D, N and E).

* @par Description
* This API disables RSA signing with only the private key components.(D, N and E).
*
* @par Parameters
* N/A
*
* @retval
* N/A
*
* @par Note
* -# It is a global function, application need to make sure that the function will not be called in parallel
*    with the RSA operations.
*
* @par Required Header File
* sec_crypto.h
*
*
* @par Related Topics
* IPSI_CRYPT_enableRsaSignWithPrvExp
*/
CRYPTOLINKDLL SEC_VOID IPSI_CRYPT_disableRsaSignWithPrvExp(void);

/*
    Func Name:  CRYPT_sm2PkeaDecrypt
*/
/**
 * @defgroup CRYPT_sm2PkeaDecrypt_ex
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_sm2PkeaDecrypt_ex(
 * SEC_PKEY_S *pKey,
 * SEC_UINT32 ulKdfDigestAlg,
 * SEC_UINT32 ulHashAlg,
 * SEC_UCHAR *pucCipherText,
 * SEC_UINT32 ulCipherLen,
 * SEC_UCHAR *pucPlainText,
 * SEC_UINT32 *pulPLen);
 * @endcode
 *
 * @par Purpose
 * This function decrypts the cipher text as per SM2 PKEA standard.
 * @par Description
 * This function decrypts the cipher text as per SM2 PKEA standard.
 * It takes as input the SM2 public key and digest algorithm used during kdf calculation
 * and the hash algorithm used for calculation of C3.
 *
 * @param[in] pKey Contains the EC public key [N/A]
 * @param[in] ulKdfDigestAlg Hash algorithm to be used during KDF operation [N/A]
 * @param[in] ulHashAlg Hash algorithm to be used for calculation of C3 [N/A]
 * @param[in] pucCipherText Pointer to the input cipher text [N/A]
 * @param[in] ulCipherLen Input cipher text length [N/A]
 * @param[out] pucPlainText  Pointer to the decrypted data buffer [N/A]
 * @param[in/out] pulPLen Pointer to decrypted data length [N/A]
 *
 * @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 Invalid arguments passed [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 Memory allocation failed [SEC_ERR_MALLOC_FAIL|N/A]
 * @retval SEC_UINT32 Incorrect Hash Algorithm ID [SEC_ERR_INVALID_ALGID|N/A]
 * @retval SEC_UINT32 Invalid Key [SEC_CRYPT_ERR_INVALID_PKEY|N/A]
 * @retval SEC_UINT32 Mathematical operation failed [SEC_ERR|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * @Memory operation: alloc、free、size
 *    该校验接口在校验过程中会多次申请内存，申请的内存在接口返回前都会自动释放，调用者无需释放内存
 * @Thread safe:
 *    线程安全函数
 * @OS difference:
 *    无OS差异
 * @Time consuming:
 *    0.104 sec in aarch64
 * -The difference between CRYPT_sm2PkeaDecrypt_ex and CRYPT_sm2PkeaDecrypt is the cipher text of
 * CRYPT_sm2PkeaEncrypt_ex is C1||C3||C2 (GBT_32918.4-2016) and the other is  C1||C2||C3
 * -CRYPT_sm2PkeaDecrypt_ex can't use with CRYPT_sm2PkeaEncrypt, and should use with CRYPT_sm2PkeaEncrypt_ex
 * - The size of the buffer pointed to by pucPlainText, should be atleast equal to ulCipherLen.
 * - ALGID_MD5 is not a valid algorithm for ulKdfDigestAlg
 * - ALGID_MD4 is not a valid algorithm for ulHashAlg and ulKdfDigestAlg
 * - If application does not set seeds and entropy on frequent intervals, it can lead to vulnerable keys or random
 * value. It is recommended to set seeds and entropy on frequent intervals or update the version that migrates the
 * random implementation based on DRBG.
 * \n
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_sm2PkeaDecrypt_ex(SEC_PKEY_S *pKey, SEC_UINT32 ulKdfDigestAlg, SEC_UINT32 ulHashAlg,
    SEC_UCHAR *pucCipherText, SEC_UINT32 ulCipherLen, SEC_UCHAR *pucPlainText, SEC_UINT32 *pulPLen);


/*
    Func Name:  CRYPT_sm2PkeaEncrypt_ex
*/
/**
 * @defgroup CRYPT_sm2PkeaEncrypt_ex
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_sm2PkeaEncrypt_ex(
 * SEC_PKEY_S *pKey,
 * SEC_UINT32 ulKdfDigestAlg,
 * SEC_UINT32 ulHashAlg,
 * SEC_UCHAR *pucPlainText,
 * SEC_UINT32 ulPlainLen,
 * SEC_UINT32 uiCipherType,
 * SEC_UCHAR *pucCipherText,
 * SEC_UINT32 *pulCLen);
 * @endcode
 *
 * @par Purpose
 * This function encrypts the input plaintext as per SM2 PKEA standard.
 * @par Description
 * This function encrypts the input plaintext as per SM2 PKEA standard.
 * It takes as input the SM2 public key and digest algorithm used during kdf calculation
 * and the hash algorithm used for calculation of C3.
 *
 * @param[in] pKey Contains the EC public key [N/A]
 * @param[in] ulKdfDigestAlg Hash algorithm to be used during KDF operation [N/A]
 * @param[in] ulHashAlg Hash algorithm to be used for calculation of C3 [N/A]
 * @param[in] pucPlainText Pointer to input data to be encrypted [N/A]
 * @param[in] ulPlainLen Input data buffer length, max length is 4294966998 [N/A]
 * @param[in] uiCipherType The cipher text consist of C1|| C3 || C2, C1 is a point and the point can
 * be encoded as compressed or uncompressed format and the format will be determined by uiCipherType. The
 * value of uiCipherType can be SEC_UNCOMPRESSED_CIPHER or SEC_COMPRESSED_CIPHER [N/A]
 * @param[out] pucCipherText  Pointer to the output encrypted data buffer [N/A]
 * @param[in/out] pulCLen Pointer to the encrypted data length [N/A]
 *
 * @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 Invalid arguments passed [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 Memory allocation failed [SEC_ERR_MALLOC_FAIL|N/A]
 * @retval SEC_UINT32 Incorrect Hash Algorithm ID [SEC_ERR_INVALID_ALGID|N/A]
 * @retval SEC_UINT32 Invalid Key [SEC_CRYPT_ERR_INVALID_PKEY|N/A]
 * @retval SEC_UINT32 Mathematical operation failed [SEC_ERR|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * @Memory operation: alloc、free、size
 *    该校验接口在校验过程中会多次申请内存，申请的内存在接口返回前都会自动释放，调用者无需释放内存
 * @Thread safe:
 *    线程安全函数
 * @OS difference:
 *    无OS差异
 * @Time consuming:
 *    0.208 sec in aarch64
 * -The difference between CRYPT_sm2PkeaEncrypt_ex and CRYPT_sm2PkeaEncrypt is the cipher text of
 * CRYPT_sm2PkeaEncrypt_ex is C1||C3||C2 (GBT_32918.4-2016) and the other is  C1||C2||C3
 * -CRYPT_sm2PkeaEncrypt_ex can't use with CRYPT_sm2PkeaDecrypt, and should use with CRYPT_sm2PkeaDecrypt_ex
 * -The size of the buffer pointed to by pucCipherText, should be atleast equal to the
 * size returned by the API CRYPT_sm2PkeaGetOutputSize.
 * - ALGID_MD5 is not a valid algorithm for ulKdfDigestAlg
 * - ALGID_MD4 is not a valid algorithm for ulHashAlg and ulKdfDigestAlg
 * - If application does not set seeds and entropy on frequent intervals, it can lead to vulnerable keys or random
 * value. It is recommended to set seeds and entropy on frequent intervals or update the version that migrates the
 * random implementation based on DRBG.
 * - Before calling this API, please make sure that DRBG is initialized for random number generation by calling
 * IPSI_CRYPT_rand_init. If application don't want to use DRBG then appplication should call
 * IPSI_CRYPT_enable_drbg(0) to disable the DRBG Random feature.
 * - DRBG is enabled by default.
 *
 * \n
 *
 * @par Related Topics
 * CRYPT_sm2PkeaGetOutputSize
 * CRYPT_sm2PkeaEncrypt_ex
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_sm2PkeaEncrypt_ex(SEC_PKEY_S *pKey, SEC_UINT32 ulKdfDigestAlg, SEC_UINT32 ulHashAlg,
    SEC_UCHAR *pucPlainText, SEC_UINT32 ulPlainLen, SEC_UINT32 uiCipherType, SEC_UCHAR *pucCipherText,
    SEC_UINT32 *pulCLen);

/**
 * @defgroup CRYPT_extractPubKeyFromPrvKey
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_extractPubKeyFromPrvKey(const SEC_PKEY_S *privKey, SEC_PKEY_S **pubKey)
 *
 * @endcode
 *
 * @par Purpose
 * This function extracts ecdsa pubkey from private key.
 * @par Description
 *  This function extracts ecdsa pubkey from private key, and only support ecdsa key.
 *
 * @param[in] privKey ecdsa private key [N/A]
 * @param[out] pubKey ecdsa public key, user should free by CRYPT_PKEY_free() [N/A]
 *
 * @retval SEC_UINT32 Extract pubkey is successful [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 Invalid arguments passed [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 Extract pubkey failed [SEC_ERR|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * This API is only used for ecdsa key, and the privkey should have invalid param.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_extractPubKeyFromPrvKey(const SEC_PKEY_S *privKey, SEC_PKEY_S **pubKey);
#ifdef __cplusplus
}
#endif

#endif /* _IPSI_SEC_CRYPTO_H */
