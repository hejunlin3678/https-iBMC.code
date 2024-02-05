/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2005-2019. All rights reserved.
 * Description: Contains various crypto interface functions
 *                             ALL RIGHTS RESERVED
 *   Project Code: iPSI
 *   Module Name: sec_crypto.h
 * Create:  2005-07-13
 *   Author: Jimmy Bahuleyan, Sanjay Vasudevan
 *   Descrption: Contains various crypto interface functions
 * ----------------------------------------------------------------------
 *   Modification History
 *   DATE        NAME             DESCRIPTION
 *   --------------------------------------------------------------------
 *   2006-07-31   Siva Kumar.K    Added a new interface and ID's to enable
 *                                SSX31B HW support
 *   2007-08-30   Vinay Gudur   Added new Algorithms - CMAC and AES-Key Wrap.
 */
#ifndef SEC_CRYPTO_H
#define SEC_CRYPTO_H

#include "ipsi_crypto_buildconf.h"
#include "sec_util.h"
#include "sec_crypto_alg_id.h"
#include "sec_crypto_asym.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Block cipher padding types */
/* Add a 1 followed by 0  or many 0 */
#define BLOCK_PADDING_ONEZERO 0x0L

/* Add n to construct a whole block */
#define BLOCK_PADDING_NORMAL 0x1L

/* No padding types */
#define BLOCK_PADDING_NONE 0x2L

#ifdef RSA_DEFAULT_PADDING
#undef RSA_DEFAULT_PADDING
#endif

#ifdef RSA_NO_PADDING
#undef RSA_NO_PADDING
#endif

#ifdef RSA_PKCS1_PADDING
#undef RSA_PKCS1_PADDING
#endif

#ifdef RSA_SSLV23_PADDING
#undef RSA_SSLV23_PADDING
#endif

#ifdef RSA_PKCS1_OAEP_PADDING
#undef RSA_PKCS1_OAEP_PADDING
#endif

/* RSA padding type */
#define RSA_DEFAULT_PADDING 0x0L

/* default type of padding mode */
#define RSA_PKCS1_PADDING 1

/* pad with random bytes containing no 0 */
#define RSA_SSLV23_PADDING 2

/* pad with random bytes containing no 0 and 8 LSB of pad set to 3 - TLSv1 */
#define RSA_NO_PADDING 3

/* no padding */
/* OEAP padding with SHA1 algorithm */
#define RSA_PKCS1_OAEP_PADDING 4

/* OEAP padding with SHA256 algorithm */
#define RSA_PKCS1_OAEP_PADDING_SHA256 0x10L

/* OEAP padding with SHA512 algorithm */
#define RSA_PKCS1_OAEP_PADDING_SHA512 0x20L

/* OAEP padding as in PKCS1 */
/*
  The below GROUPID macros identify CURVEs defined by X9.62
  standard Appendix J  optimal basis is not supported

  The macros should be used as arguments to the
  CRYPT_createECParamsById function
  The Group IDs defined to be zero are
  standard vectors with point decompression which are currently not supported
*/
#define GROUPID_X9_62_C2PNB163V1 1
#define GROUPID_X9_62_C2PNB163V2 2
#define GROUPID_X9_62_C2PNB163V3 3
#define GROUPID_X9_62_C2PNB176W1 0
#define GROUPID_X9_62_C2TNB191V1 4
#define GROUPID_X9_62_C2TNB191V2 5
#define GROUPID_X9_62_C2TNB191V3 6
#define GROUPID_X9_62_C2ONB191V4 0
#define GROUPID_X9_62_C2ONB191V5 0

#define GROUPID_X9_62_C2PNB208W1 0
#define GROUPID_X9_62_C2TNB239V1 7
#define GROUPID_X9_62_C2TNB239V2 8
#define GROUPID_X9_62_C2TNB239V3 9
#define GROUPID_X9_62_C2ONB239V4 0
#define GROUPID_X9_62_C2ONB239V5 0

#define GROUPID_X9_62_C2PNB272W1 0
#define GROUPID_X9_62_C2PNB304W1 0
#define GROUPID_X9_62_C2TNB359V1 10
#define GROUPID_X9_62_C2PNB368W1 0
#define GROUPID_X9_62_C2TNB431R1 11

#define GROUPID_X9_62_PRIME192V1 12
#define GROUPID_X9_62_PRIME192V2 13
#define GROUPID_X9_62_PRIME192V3 14
#define GROUPID_X9_62_PRIME239V1 15
#define GROUPID_X9_62_PRIME239V2 16
#define GROUPID_X9_62_PRIME239V3 17
#define GROUPID_X9_62_PRIME256V1 18
#define GROUPID_X9_62_WAPIPRIME192V3 19

/*
  The below GROUPID macros identify CURVEs defined by SM2  standard
 */
#define GROUPID_SM2_PRIME256 20

/*
 SEC 2 Curves
 */
#define GROUPID_SEC_P256R1 18
#define GROUPID_SEC_P384R1 21
#define GROUPID_SEC_P521R1 22

/*
NIST.FIPS.186-4 Extra Curves
*/
#define GROUPID_NIST_PRIME224V1 23
#define GROUPID_NIST_C2PNB163K 24
#define GROUPID_NIST_C2PNB163B 25
#define GROUPID_NIST_C2TNB233K 26
#define GROUPID_NIST_C2TNB233B 27
#define GROUPID_NIST_C2PNB283K 28
#define GROUPID_NIST_C2PNB283B 29
#define GROUPID_NIST_C2PNB409K 30
#define GROUPID_NIST_C2PNB409B 31
#define GROUPID_NIST_C2PNB571K 32
#define GROUPID_NIST_C2PNB571B 33

// Backward compatibility macros
#define SEC_SHA384_BACK_COMP 1
#define SEC_SHA512_BACK_COMP 2

#define SEC_UNCOMPRESSED_CIPHER 0
#define SEC_COMPRESSED_CIPHER 1

/* Max size of big integers supported in crypto module */
/* NOTE for internal use: The below value must be in sync
with GLOBAL_RNG_STATE_OCTS */
#define PRNG_STATE_OCTS 262UL

/* Size of the random number generator state */
/*
The context that will be used for random number generation using DRBG.
*/
typedef SEC_VOID *IPSI_DRBG_CTX;

/*
The context that will be used for AEAD encrypt/decrypt operations.
*/
typedef SEC_VOID *CRYPT_AEAD_CTX;

/* RSA parameter structure with length and public Exponent */
/**
 * @defgroup RSA_PARA_S
 * @ingroup sec_cryptoStructures
 * @par Prototype
 * @code
 * typedef struct stRSA_PARA
 * {
 *  SEC_UINT32   uiBits;
 *  SEC_BIGINT_S stPubExp;
 * }RSA_PARA_S
 * @endcode
 *
 * @datastruct uiBits Number of bits in the rsa modulus.
 * @datastruct stPubExp Octets of public exponent.
 */
typedef struct stRSA_PARA {
    SEC_UINT32 uiBits;     /* Number of bits in the rsa modulus */
    SEC_BIGINT_S stPubExp; /* octets of public exponent */
} RSA_PARA_S;

/* DSA parameter struct with P, Q, G */
/**
* @defgroup DSA_PARA_S
* @ingroup sec_cryptoStructures
* @par Prototype
* @code
* typedef struct stDSA_PARA
* {
*  SEC_BIGINT_S stP;
*  SEC_BIGINT_S stQ;
*  SEC_BIGINT_S stG;
* }DSA_PARA_S;
* @endcode
*
* @datastruct stP Prime number which is divisible by q-1.
* @datastruct stQ Prime number, 2^159<q<2^160 which
determines the order of the cyclic group.
* @datastruct stG An element in Z*(p) used to create generator.
*/
typedef struct stDSA_PARA {
    SEC_BIGINT_S stP;

    /* prime number which is divisible by q-1 */
    SEC_BIGINT_S stQ;

    /* prime number, 2^159<q<2^160 which determines the order
    of the cyclic group */
    SEC_BIGINT_S stG;

    /* An element in Z*(p) used to create generator */
} DSA_PARA_S;

/* DH parameter struct with P, G, Q */
/**
* @defgroup DH_PARA_S
* @ingroup sec_cryptoStructures
* @par Prototype
* @code
* typedef struct stDH_PARA
* {
* SEC_BIGINT_S stP;
* SEC_BIGINT_S stG;
* SEC_BIGINT_S stQ;
* }DH_PARA_S;
* @endcode
*
* @datastruct stP The prime defining the Galois field.
* @datastruct stG The generator of the multiplicative
subgroup of order q.
* @datastruct stQ The prime factor of p-1.
*/
typedef struct stDH_PARA {
    SEC_BIGINT_S stP;

    /* The prime defining the Galois field */
    SEC_BIGINT_S stG;

    /* The generator of the multiplicative subgroup of order q */
    SEC_BIGINT_S stQ;

    /* The prime factor of p-1 */
} DH_PARA_S;

/* Generic structure to pass parameters for key generation */
/**
 * @defgroup ALG_PARA_S
 * @ingroup sec_cryptoStructures
 * @par Prototype
 * @code
 * typedef struct stALG_PARA
 * {
 *  SEC_UINT32 uiAlgId;
 *  union
 *      {
 *       RSA_PARA_S* rsa;
 *       DSA_PARA_S* dsa;
 *       EC_PARA_S*  ec;
 *       DH_PARA_S*  dh;
 *      }para;
 * }ALG_PARA_S;
 * @endcode
 *
 * @datastruct uiAlgId Algorithm id for which parameters are passed.
 * @datastruct rsa RSA parameter structure.
 * @datastruct dsa DSA parameter structure.
 * @datastruct ec EC parameter structure.
 * @datastruct dh DH parameter structure.
 * @datastruct para Holds the algorithm specific parameter structure.
 */
typedef struct stALG_PARA {
    SEC_UINT32 uiAlgId;

    /* Algorithm id for which parameters are passed */
    union {
        RSA_PARA_S *rsa; /* RSA parameter structure */
        DSA_PARA_S *dsa; /* DSA parameter structure */
        EC_PARA_S *ec;   /* EC parameter structure */
        DH_PARA_S *dh;   /* DH parameter structure */
    } para;              /* Holds the algorithm specific parameter structure */
} ALG_PARA_S;

/* Asymmetric KEY type, public or private */
#define CRYPT_PKEY_UNKNOWN 0UL   /* key type is unknown */
#define CRYPT_PKEY_PUBLIC 0x1UL  /* key type is public */
#define CRYPT_PKEY_PRIVATE 0x2UL /* key type is private */
#define CRYPT_PKEY_PAIR (CRYPT_PKEY_PUBLIC | CRYPT_PKEY_PRIVATE)

/* key type is key-pair */
/* Macro to check key type - public or private */
#define CRYPT_IS_PKEY_PUBLIC(x) (((x) & CRYPT_PKEY_PUBLIC) != 0)
#define CRYPT_IS_PKEY_PRIVATE(x) (((x) & CRYPT_PKEY_PRIVATE) != 0)
#define CRYPT_IS_PKEY_PAIR(x) (((x) & CRYPT_PKEY_PAIR) != 0)
#define CRYPT_IS_PKEY_UNDEF(x) (((x) == CRYPT_PKEY_UNKNOWN) || (((x) & ~(CRYPT_PKEY_PUBLIC | CRYPT_PKEY_PRIVATE)) != 0))

/* Identifies type of field for Elliptic curve algorithms */
#define CRYPT_EC_FIELD_PRIME 0x1UL   /* Prime field */
#define CRYPT_EC_FIELD_C2_POLY 0x2UL /* Binary field */
/* Identifies type of polynomial for a binary field elliptic curve */
#define CRYPT_EC_C2_BASIS_TRI 0x1UL   /* Trinomial */
#define CRYPT_EC_C2_BASIS_PENTA 0x2UL /* Pentanomial */
/* Default key length for RC2 block cipher */
#define RC2_DEFAULT_KEYLEN 16

/* Default key length for RC4 stream cipher */
#define RC4_DEFAULT_KEYLEN 16

/* Default key length for Blowfish block cipher */
#define BF_DEFAULT_KEYLEN 16

/* for undef */
#define UNDEF (-1)
#define MODE_SOFT 0
#define MODE_SSX31B_LINUX 1
#define MODE_SSX31B_VXWORKS 2
#define MODE_WIN32_USBKEY 3

// DRBG Options
#define IPSI_CRYPT_DRBG_DISABLE 0
#define IPSI_CRYPT_DRBG_ENABLE 1

/**
* @defgroup IPSI_CRYPTO_TEST_ID_E
* @ingroup sec_cryptoEnum
* @par Prototype
* @code
* typedef enum
* {
* IPSI_CRYPTO_TEST_INTEGRITY = 1,
* IPSI_CRYPTO_TEST_DIGEST = 2,
* IPSI_CRYPTO_TEST_CIPHER = 3,
* IPSI_CRYPTO_TEST_SIGNATURE = 4,
* IPSI_CRYPTO_TEST_HMAC = 5,
* IPSI_CRYPTO_TEST_DRBG = 6,
* IPSI_CRYPTO_TEST_GCM = 7,
* IPSI_CRYPTO_TEST_CMAC = 8,
* IPSI_CRYPTO_TEST_CTR = 9,
* IPSI_CRYPTO_TEST_PAIRWISE = 12,
* IPSI_CRYPTO_TEST_ECDH : 14
* }

* @endcode
*
* @datastruct IPSI_CRYPTO_TEST_INTEGRITY identifies HMAC integrity test \n
* @datastruct IPSI_CRYPTO_TEST_DIGEST identifies Digest test \n
* @datastruct IPSI_CRYPTO_TEST_CIPHER identifies Symmetric cipher tes \n
* @datastruct IPSI_CRYPTO_TEST_SIGNATURE identifies Public key signature test \n
* @datastruct IPSI_CRYPTO_TEST_HMAC identifies HMAC test \n
* @datastruct IPSI_CRYPTO_TEST_DRBG identifies DRBG test \n
* @datastruct IPSI_CRYPTO_TEST_GCM identifies GCM test \n
* @datastruct IPSI_CRYPTO_TEST_CMAC identifies CMAC test \n
* @datastruct IPSI_CRYPTO_TEST_CTR identifies CTR test \n
* @datastruct IPSI_CRYPTO_TEST_PAIRWISE identifies Keygen pairwise consistency test \n
* @datastruct IPSI_CRYPTO_TEST_ECDH identifies ECDH test \n
*
*/
typedef enum {
    /* HMAC integrity test */
    IPSI_CRYPTO_TEST_INTEGRITY = 1,

    /* Digest test */
    IPSI_CRYPTO_TEST_DIGEST = 2,

    /* Symmetric cipher test */
    IPSI_CRYPTO_TEST_CIPHER = 3,

    /* Public key signature test */
    IPSI_CRYPTO_TEST_SIGNATURE = 4,

    /* HMAC test */
    IPSI_CRYPTO_TEST_HMAC = 5,

    /* DRBG test */
    IPSI_CRYPTO_TEST_DRBG = 6,

    /* GCM test */
    IPSI_CRYPTO_TEST_GCM = 7,

    /*  cmac test */
    IPSI_CRYPTO_TEST_CMAC = 8,

    /* ctr test */
    IPSI_CRYPTO_TEST_CTR = 9,

    /* Keygen pairwise consistency test */
    IPSI_CRYPTO_TEST_PAIRWISE = 12,

    /* ECDH test */
    IPSI_CRYPTO_TEST_ECDH = 14,

    /* CCM test */
    IPSI_CRYPTO_TEST_CCM = 15,
    /* Public key encryption test */
    IPSI_CRYPTO_TEST_ENCRYPTION = 16,
} IPSI_CRYPTO_TEST_ID_E;

#ifndef __IPSI_NO_DH_PARA__

/**
 * @defgroup SEC_DH_PRIME_E
 * @ingroup sec_cryptoEnum
 * @par Prototype
 * @code
 * typedef enum
 * {
 *    GROUPID_RFC2409_1 = 0,
 *    GROUPID_RFC2409_2,
 *    GROUPID_RFC3256_5,
 *    GROUPID_RFC3526_14,
 *    GROUPID_RFC3526_15,
 *    GROUPID_RFC3526_16,
 *    SEC_RFC_PRIME_MAX
 * }SEC_DH_PRIME_E;
 *
 * @endcode
 *
 * @datastruct GROUPID_RFC2409_1=0 Identifies prime of bit length 768
 * defined in rfc 2409
 * @datastruct GROUPID_RFC2409_2 Identifies prime of bit length 1024
 * defined in rfc 2409
 * @datastruct GROUPID_RFC3256_5 Identifies prime of bit length 1536
 * defined in rfc 3526
 * @datastruct GROUPID_RFC3526_14 Identifies prime of bit length 2048
 * defined in rfc 3526
 * @datastruct GROUPID_RFC3526_15 Identifies prime of bit length 3072
 * defined in rfc 3526
 * @datastruct GROUPID_RFC3526_16 Identifies prime of bit length 4096
 * defined in rfc 3526
 * @datastruct SEC_RFC_PRIME_MAX This is the upper limit of SEC_RFC_DH_PRIME_E.
 * In future if, any new prime added in this enum
 * should be defined above SEC_RFC_PRIME_MAX
 */
typedef enum {
    // DH Group 1
    // 768 Bit
    GROUPID_RFC2409_1 = 0,

    // DH Group 2
    // 1024 Bit
    GROUPID_RFC2409_2,

    // DH Group 5
    // 1536 Bit
    GROUPID_RFC3256_5,

    // DH Group 14
    // 2048 Bit
    GROUPID_RFC3526_14,

    // DH Group 15
    // 3072 Bit
    GROUPID_RFC3526_15,

    // DH Group 16
    // 4096 Bit
    GROUPID_RFC3526_16,
    SEC_RFC_PRIME_MAX
} SEC_DH_PRIME_E;

#endif

/*
  stores the state of the random number generator - this
  can be used for serializing the
  state of the rng generator
*/
/**
 * @defgroup PRNG_STATE_S
 * @ingroup sec_cryptoStructures
 * @par Prototype
 * @code
 * typedef struct stPRNG_STATE
 * {
 *  SEC_INT8 aState[PRNG_STATE_OCTS]
 * }PRNG_STATE_S
 * @endcode
 *
 * @datastruct aState[PRNG_STATE_OCTS] Holds the state as a series of octets.
 */
typedef struct stPRNG_STATE {
    SEC_INT8 aState[PRNG_STATE_OCTS];

    /* Holds the state as a series of octets */
} PRNG_STATE_S;

/*
    Func Name:  CRYPT_encryptInit
*/
/**
* @defgroup CRYPT_encryptInit
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_encryptInit(
* CRYPT_CTX* pCtx,
* SEC_UINT32 ulAlgId,
* const SEC_UCHAR* pucKey,
* SEC_UINT32 ulKeyLen,
* const SEC_UCHAR* pucIV,
* SEC_UINT32 ulIVLen);
* @endcode
*
* @par Purpose
* This is used to create symmetric context for encryption.
*
* @par Description
* CRYPT_encryptInit function initializes the
symmetric context structure for symmetric encryption. Key length, blocksize,
* encryption, and decryption function  pointers
are initialized using the Symg_aAlgTable. CarryBlock is allocated memory.
* Feedback is also allocated memory if needed.
*
* @param[in] ulAlgId The algorithm Id [N/A]
* @param[in] pucKey The key value to be used [N/A]
* @param[in] ulKeyLen The length of the key. This length must be according to algorithm used and must match with
* CRYPT_SYM_keyLen(ulAlgId) [N/A]
* @param[in] pucIV InitializationVector(CBC, CFB, OFB algorithm modes) [N/A]
* @param[in] ulIVLen Length of IV. pass 0 if no IV. This length must be according to algorithm used and must match with
* CRYPT_SYM_ivLen(ulAlgId)[N/A]
* @param[out] pCtx Address of the context to be initialized [N/A]
*
* @retval SEC_UINT32 Invalid argument [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 Invalid Algorithm Id [SEC_ERR_INVALID_ALGID|N/A]
* @retval SEC_UINT32 Invalid Padding Mode. Padding mode set on Algorithm
is not defined
* [SEC_CRYPT_ERR_UNDEFINED_PADDING|N/A]
* @retval SEC_UINT32 Invalid Keylength for alg [SEC_ERR_INVALID_KEY_LEN|N/A]
* @retval SEC_UINT32 Invalid IVlength for alg [SEC_ERR_INVALID_IV_LEN|N/A]
* @retval SEC_UINT32 On success [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 On error [SEC_ERR|N/A]
*
* @par Required Header File
* sec_crupto.h
*
* @par Note
* - When using 3DES, application need to take care that K1(first 64 bits), K2(middle 64 bits), and K3(last 64 bits)
*    are different from each other.
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
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_encryptInit(CRYPT_CTX *pCtx, SEC_UINT32 ulAlgId, const SEC_UCHAR *pucKey, SEC_UINT32 ulKeyLen, const SEC_UCHAR *pucIV, SEC_UINT32 ulIVLen);

/*
    Func Name:  CRYPT_encryptUpdate
*/
/**
* @defgroup CRYPT_encryptUpdate
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_encryptUpdate (
* CRYPT_CTX   ctx,
* const SEC_UCHAR*  pucPlainText,
* SEC_UINT32  ulPlainLen,
* SEC_UCHAR*  pucCipherText,
* SEC_UINT32* pulCLen,
* SEC_UINT32 ulCTLen);
* @endcode
*
* @par Purpose
* This is used to encrypt plain text to cipher text
using encryption algorithm.
*
* @par Description
* CRYPT_encryptUpdate function uses the initialized
symmetric context to encrypt plaintext to get ciphertext. It uses
* symmetric encryption. If plain text length is less
than block size of encryption algorithm it carry forwards to
* CRYPT_encryptFinal. If plain text length is greater
than block size of encryption algorithm then multiple block size
* of plain text encrypts and rest carry forward to final block.
*
* @param[in] ctx The context to be used [N/A]
* @param[in] pucPlainText Plaintext to be encrypted [N/A]
* @param[in] ulPlainLen Length of plaintext [N/A]
* @param[out] pucCipherText Encrypted ciphertext [N/A]
* @param[out] pulCLen Length of encrypted ciphertext [N/A]
* @param[in] ulCTLen The pucCipherText buffer size [N/A]
*
* @retval SEC_UINT32 If invalid argument [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 If invalid context [SEC_CRYPT_ERR_INVALID_CTX|N/A]
* @retval SEC_UINT32 On successful return [Success|N/A]
* @retval SEC_UINT32 On other error [SEC_ERR|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* - The safe API corresponding to this is CRYPT_encryptUpdate_sf
*
* @par Related Topics
* N/A
*/
#ifndef HERT_RAT_COMPATIBILITY
CRYPTOLINKDLL SEC_UINT32 CRYPT_encryptUpdate(CRYPT_CTX ctx, const SEC_UCHAR *pucPlainText, SEC_UINT32 ulPlainLen, SEC_UCHAR *pucCipherText, SEC_UINT32 *pulCLen, SEC_UINT32 ulCTLen);
#else
CRYPTOLINKDLL SEC_UINT32 CRYPT_encryptUpdate(CRYPT_CTX ctx, const SEC_UCHAR *pucPlainText, SEC_UINT32 ulPlainLen, SEC_UCHAR *pucCipherText, SEC_UINT32 *pulCLen);
#endif

/*
    Func Name:  CRYPT_encryptUpdate_ex
*/
/**
* @defgroup CRYPT_encryptUpdate_ex
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_encryptUpdate_ex (
* CRYPT_CTX   ctx,
* const SEC_UCHAR*  pucPlainText,
* SEC_UINT32  ulPlainLen,
* SEC_UCHAR*  pucCipherText,
* SEC_UINT32* pulCLen,
* SEC_UINT32 ulCTLen);
* @endcode
*
* @par Purpose
* This is used to encrypt plain text to cipher text
using encryption algorithm.
*
* @par Description
* CRYPT_encryptUpdate_ex function uses the initialized
symmetric context to encrypt plaintext to get ciphertext. It uses
* symmetric encryption. If plain text length is less
than block size of encryption algorithm it carry forwards to
* CRYPT_encryptFinal. If plain text length is greater
than block size of encryption algorithm then multiple block size
* of plain text encrypts and rest carry forward to final block.
*
* @param[in] ctx The context to be used [N/A]
* @param[in] pucPlainText Plaintext to be encrypted [N/A]
* @param[in] ulPlainLen Length of plaintext [N/A]
* @param[out] pucCipherText Encrypted ciphertext [N/A]
* @param[out] pulCLen Length of encrypted ciphertext [N/A]
* @param[in] ulCTLen The pucCipherText buffer size [N/A]
*
* @retval SEC_UINT32 If invalid argument [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 If invalid context [SEC_CRYPT_ERR_INVALID_CTX|N/A]
* @retval SEC_UINT32 On successful return [Success|N/A]
* @retval SEC_UINT32 On other error [SEC_ERR|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* - The safe API corresponding to this is CRYPT_encryptUpdate_sf
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_encryptUpdate_ex(CRYPT_CTX ctx, const SEC_UCHAR *pucPlainText, SEC_UINT32 ulPlainLen, SEC_UCHAR *pucCipherText, SEC_UINT32 *pulCLen, SEC_UINT32 ulCTLen);

/*
    Func Name:  CRYPT_encryptFinal
*/
/**
* @defgroup CRYPT_encryptFinal
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_encryptFinal (
* CRYPT_CTX*  pCtx,
* SEC_UCHAR*  pucCipherText,
* SEC_UINT32* pulCLen,
* SEC_UINT32 ulCTLen);
* @endcode
*
* @par Purpose
* This is used to encrypt only last block of plain text to
cipher text using encryption algorithm.
*
* @par Description
* CRYPT_encryptFinal function completes a symmetric
encryption operation. It frees the symmetric context and outputs
* the last block in case of block ciphers.
If encryptinit succeeds, encryptfinal must be called to free the context.
*
* @param[in] pCtx Address of the context to be used [N/A]
* @param[out] pucCipherText Last Encrypted ciphertext of the session [N/A]
* @param[out] pulCLen Length of last encrypted ciphertext [N/A]
* @param[in] ulCTLen The pucCipherText buffer size [N/A]
*
* @retval SEC_UINT32 If invalid argument [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 If invalid context [SEC_CRYPT_ERR_INVALID_CTX|N/A]
* @retval SEC_UINT32 Invalid pad mode in context
[SEC_CRYPT_ERR_INVALID_PADDING|N/A]
* @retval SEC_UINT32 Invalid plaintext length given
for encryption in update and padding_mode = BLOCK_PADDING_NONE.
* plaintext length was not integral number of blocks)
[SEC_ERR_INVALID_DATA_LEN|N/A]
* @retval SEC_UINT32 On successful return [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 On other error [SEC_ERR|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* - The safe API corresponding to this is CRYPT_encryptFinal_sf
*
* @par Related Topics
* N/A
*/
#ifndef HERT_RAT_COMPATIBILITY
CRYPTOLINKDLL SEC_UINT32 CRYPT_encryptFinal(CRYPT_CTX *pCtx, SEC_UCHAR *pucCipherText, SEC_UINT32 *pulCLen, SEC_UINT32 ulCTLen);
#else
CRYPTOLINKDLL SEC_UINT32 CRYPT_encryptFinal(CRYPT_CTX *pCtx, SEC_UCHAR *pucCipherText, SEC_UINT32 *pulCLen);
#endif

/*
    Func Name:  CRYPT_encryptFinal_ex
*/
/**
* @defgroup CRYPT_encryptFinal_ex
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_encryptFinal_ex (
* CRYPT_CTX*  pCtx,
* SEC_UCHAR*  pucCipherText,
* SEC_UINT32* pulCLen,
* SEC_UINT32 ulCTLen);
* @endcode
*
* @par Purpose
* This is used to encrypt only last block of plain text to
cipher text using encryption algorithm.
*
* @par Description
* CRYPT_encryptFinal_ex function completes a symmetric
encryption operation. It frees the symmetric context and outputs
* the last block in case of block ciphers.
If encryptinit succeeds, encryptfinal must be called to free the context.
*
* @param[in] pCtx Address of the context to be used [N/A]
* @param[out] pucCipherText Last Encrypted ciphertext of the session [N/A]
* @param[out] pulCLen Length of last encrypted ciphertext [N/A]
* @param[in] ulCTLen The pucCipherText buffer size [N/A]
*
* @retval SEC_UINT32 If invalid argument [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 If invalid context [SEC_CRYPT_ERR_INVALID_CTX|N/A]
* @retval SEC_UINT32 Invalid pad mode in context
[SEC_CRYPT_ERR_INVALID_PADDING|N/A]
* @retval SEC_UINT32 Invalid plaintext length given
for encryption in update and padding_mode = BLOCK_PADDING_NONE.
* plaintext length was not integral number of blocks)
[SEC_ERR_INVALID_DATA_LEN|N/A]
* @retval SEC_UINT32 On successful return [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 On other error [SEC_ERR|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* - The safe API corresponding to this is CRYPT_encryptFinal_sf
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_encryptFinal_ex(CRYPT_CTX *pCtx, SEC_UCHAR *pucCipherText, SEC_UINT32 *pulCLen, SEC_UINT32 ulCTLen);

/*
    Func Name:  CRYPT_encrypt
*/
/**
* @defgroup CRYPT_encrypt
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_encrypt (
* SEC_UINT32 ulAlgId,
* const SEC_UCHAR* pucKey,
* SEC_UINT32 ulKeyLen,
* const SEC_UCHAR* pucIV,
* SEC_UINT32 ulIVLen,
* const SEC_UCHAR* pucPlainText,
* SEC_UINT32 ulPlainLen,
* SEC_UCHAR* pucCipherText,
* SEC_UINT32* pulCLen,
* SEC_UINT32 ulCTLen);
* @endcode
*
* @par Purpose
* This is used to encrypt plain text to cipher text
using encryption algorithm.
*
* @par Description
* CRYPT_encrypt function encrypts plain text to cipher text
using encryption algorithm. It creates symmetric context by
* creating algorithm object, padding object, opmode object.
After encryption, symmetric context needs to be freed.
*
* @param[in] ulAlgId The algorithm Id [N/A]
* @param[in] pucKey The key value to be used [N/A]
* @param[in] ulKeyLen The length of the key. This length must be according to algorithm used and must match with
* CRYPT_SYM_keyLen(ulAlgId) [N/A]
* @param[in] pucIV InitializationVector(CBC,CFB,OFB
algorithm modes) [N/A]
* @param[in] ulIVLen Length of IV. pass 0 if no IV. This length must be according to algorithm used and must match with
* CRYPT_SYM_ivLen(ulAlgId)[N/A]
* @param[in] pucPlainText Plaintext to be encrypted [N/A]
* @param[in] ulPlainLen Length of plaintext [N/A]
* @param[out] pucCipherText Encrypted ciphertext [N/A]
* @param[out] pulCLen Length of encrypted ciphertext [N/A]
* @param[in] ulCTLen The pucCipherText buffer size [N/A]
*
* @retval SEC_UINT32 On Memory allocation failure
[SEC_ERR_MALLOC_FAIL|N/A]
* @retval SEC_UINT32 If invalid Algorithm Id [SEC_ERR_INVALID_ALGID|N/A]
* @retval SEC_UINT32 If invalid Padding Mode.
Padding mode set on Algorithm is not defined
* [SEC_CRYPT_ERR_UNDEFINED_PADDING |N/A]
* @retval SEC_UINT32 If invalid Keylength for
algorithm [SEC_ERR_INVALID_KEY_LEN|N/A]
* @retval SEC_UINT32 If invalid IVlength for
algorithm [SEC_ERR_INVALID_IV_LEN|N/A]
* @retval SEC_UINT32 If invalid plaintext length
given for encryption and padding_mode = BLOCK_PADDING_NONE.
* (plaintext length was not integral no. of blocks)
[SEC_ERR_INVALID_DATA_LEN|N/A]
* @retval SEC_UINT32 On successful return [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 On other error [SEC_ERR|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* - When using 3DES, application need to take care that K1(first 64 bits), K2(middle 64 bits), and K3(last 64 bits)
*    are different from each other.
* - The safe API corresponding to this is CRYPT_encrypt_sf
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
#ifndef HERT_RAT_COMPATIBILITY
CRYPTOLINKDLL SEC_UINT32 CRYPT_encrypt(SEC_UINT32 ulAlgId, const SEC_UCHAR *pucKey, SEC_UINT32 ulKeyLen, const SEC_UCHAR *pucIV, SEC_UINT32 ulIVLen, const SEC_UCHAR *pucPlainText, SEC_UINT32 ulPlainLen, SEC_UCHAR *pucCipherText, SEC_UINT32 *pulCLen,
                                       SEC_UINT32 ulCTLen);
#else
CRYPTOLINKDLL SEC_UINT32 CRYPT_encrypt(SEC_UINT32 ulAlgId, const SEC_UCHAR *pucKey, SEC_UINT32 ulKeyLen, const SEC_UCHAR *pucIV, SEC_UINT32 ulIVLen, const SEC_UCHAR *pucPlainText, SEC_UINT32 ulPlainLen, SEC_UCHAR *pucCipherText, SEC_UINT32 *pulCLen);
#endif

/*
    Func Name:  CRYPT_encrypt_ex
*/
/**
* @defgroup CRYPT_encrypt_ex
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_encrypt_ex (
* SEC_UINT32 ulAlgId,
* const SEC_UCHAR* pucKey,
* SEC_UINT32 ulKeyLen,
* const SEC_UCHAR* pucIV,
* SEC_UINT32 ulIVLen,
* const SEC_UCHAR* pucPlainText,
* SEC_UINT32 ulPlainLen,
* SEC_UCHAR* pucCipherText,
* SEC_UINT32* pulCLen,
* SEC_UINT32 ulCTLen);
* @endcode
*
* @par Purpose
* This is used to encrypt plain text to cipher text
using encryption algorithm.
*
* @par Description
* CRYPT_encrypt_ex function encrypts plain text to cipher text
using encryption algorithm. It creates symmetric context by
* creating algorithm object, padding object, opmode object.
After encryption, symmetric context needs to be freed.
*
* @param[in] ulAlgId The algorithm Id [N/A]
* @param[in] pucKey The key value to be used [N/A]
* @param[in] ulKeyLen The length of the key. This length must be according to algorithm used and must match with
* CRYPT_SYM_keyLen(ulAlgId)
* @param[in] pucIV InitializationVector(CBC,CFB,OFB
algorithm modes) [N/A]
* @param[in] ulIVLen Length of IV. pass 0 if no IV. This length must be according to algorithm used and must match with
* CRYPT_SYM_ivLen(ulAlgId)[N/A]
* @param[in] pucPlainText Plaintext to be encrypted [N/A]
* @param[in] ulPlainLen Length of plaintext [N/A]
* @param[out] pucCipherText Encrypted ciphertext [N/A]
* @param[out] pulCLen Length of encrypted ciphertext [N/A]
* @param[in] ulCTLen The pucCipherText buffer size [N/A]
*
* @retval SEC_UINT32 On Memory allocation failure
[SEC_ERR_MALLOC_FAIL|N/A]
* @retval SEC_UINT32 If invalid Algorithm Id [SEC_ERR_INVALID_ALGID|N/A]
* @retval SEC_UINT32 If invalid Padding Mode.
Padding mode set on Algorithm is not defined
* [SEC_CRYPT_ERR_UNDEFINED_PADDING |N/A]
* @retval SEC_UINT32 If invalid Keylength for
algorithm [SEC_ERR_INVALID_KEY_LEN|N/A]
* @retval SEC_UINT32 If invalid IVlength for
algorithm [SEC_ERR_INVALID_IV_LEN|N/A]
* @retval SEC_UINT32 If invalid plaintext length
given for encryption and padding_mode = BLOCK_PADDING_NONE.
* (plaintext length was not integral no. of blocks)
[SEC_ERR_INVALID_DATA_LEN|N/A]
* @retval SEC_UINT32 On successful return [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 On other error [SEC_ERR|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* - When using 3DES, application need to take care that K1(first 64 bits), K2(middle 64 bits), and K3(last 64 bits)
*    are different from each other.
* - The safe API corresponding to this is CRYPT_encrypt_sf
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
CRYPTOLINKDLL SEC_UINT32 CRYPT_encrypt_ex(SEC_UINT32 ulAlgId, const SEC_UCHAR *pucKey, SEC_UINT32 ulKeyLen, const SEC_UCHAR *pucIV, SEC_UINT32 ulIVLen, const SEC_UCHAR *pucPlainText, SEC_UINT32 ulPlainLen, SEC_UCHAR *pucCipherText,
                                          SEC_UINT32 *pulCLen, SEC_UINT32 ulCTLen);
/*
    Func Name:  CRYPT_decryptInit
*/
/**
* @defgroup CRYPT_decryptInit
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_decryptInit (
* CRYPT_CTX*       pCtx,
* SEC_UINT32       ulAlgId,
* const SEC_UCHAR* pucKey,
* SEC_UINT32       ulKeyLen,
* const SEC_UCHAR* pucIV,
* SEC_UINT32       ulIVLen);
* @endcode
*
* @par Purpose
* This is used to create symmetric context for decryption.
*
* @par Description
* CRYPT_decryptInit function initializes the
symmetric context structure for symmetric decryption.
Key length, blocksize,
* decryption, decryption function pointers are
initialized using the Symg_aAlgTable. CarryBlock is allocated memory.
* Feedback is also allocated memory if needed.
*
* @param[in] ulAlgId The algorithm Id [N/A]
* @param[in] pucKey The key value to be used [N/A]
* @param[in] ulKeyLen The length of the key. This length must be according to algorithm used and must match with
* CRYPT_SYM_keyLen(ulAlgId)
* @param[in] pucIV InitializationVector(CBC, CFB,
OFB algorithm modes) [N/A]
* @param[in] ulIVLen Length of IV. pass 0 if no IV. This length must be according to algorithm used and must match with
* CRYPT_SYM_ivLen(ulAlgId)[N/A]
* @param[out] pCtx Address of the context to be initialized [N/A]
*
* @retval SEC_INT32 If invalid argument [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_INT32 If invalid Algorithm Id [SEC_ERR_INVALID_ALGID|N/A]
* @retval SEC_INT32 If invalid Padding Mode.
Padding mode set on Algorithm is not defined
* [SEC_CRYPT_ERR_UNDEFINED_PADDING|N/A]
* @retval SEC_INT32 If invalid Keylength for
algorithm [SEC_ERR_INVALID_KEY_LEN|N/A]
* @retval SEC_INT32 If invalid IVlength for
algorithm [SEC_ERR_INVALID_IV_LEN|N/A]
* @retval SEC_INT32 On successful return [SEC_SUCCESS|N/A]
* @retval SEC_INT32 On other error [SEC_ERR|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* - When using 3DES, application need to take care that K1(first 64 bits), K2(middle 64 bits), and K3(last 64 bits)
*    are different from each other.
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
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_decryptInit(CRYPT_CTX *pCtx, SEC_UINT32 ulAlgId, const SEC_UCHAR *pucKey, SEC_UINT32 ulKeyLen, const SEC_UCHAR *pucIV, SEC_UINT32 ulIVLen);

/*
    Func Name:  CRYPT_decryptUpdate
*/
/**
* @defgroup CRYPT_decryptUpdate
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_decryptUpdate (
* CRYPT_CTX   ctx,
* const SEC_UCHAR*  pucCipherText,
* SEC_UINT32  ulCLen,
* SEC_UCHAR*  pucPlainText,
* SEC_UINT32* pulPLen,
* SEC_UINT32 ulPTLen);
* @endcode
*
* @par Purpose
* This is used to decrypt cipher text to plain
text using decryption algorithm.
*
* @par Description
* CRYPT_decryptUpdate function uses the initialized
symmetric context to decrypt ciphertext to get plaintext.
It uses
* symmetric decryption. If cipher text length is
less than or equal to block size of decryption
algorithm, it carry
* forwards to CRYPT_decryptFinal. If cipher text
length is greater than block size of decryption algorithm then multiple
* block size of cipher text decrypts and rest are
carry forward to final block.
*
* @param[in] ctx The context to be used [N/A]
* @param[in] pucCipherText Encrypted ciphertext [N/A]
* @param[in] ulCLen Ciphertext Length [N/A]
* @param[out] pucPlainText Decrypted plaintext [N/A]
* @param[out] pulPLen Length of decrypted plaintext [N/A]
* @param[in] ulPTLen size of the buffer pucPlainText  [N/A]
*
* @retval SEC_UINT32 If invalid argument [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 If invalid context [SEC_CRYPT_ERR_INVALID_CTX|N/A]
* @retval SEC_UINT32 On successful return [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 On other error [SEC_ERR|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* - The safe API corresponding to this is CRYPT_decryptUpdate_sf
*
* @par Related Topics
* N/A
*/
#ifndef HERT_RAT_COMPATIBILITY
CRYPTOLINKDLL SEC_UINT32 CRYPT_decryptUpdate(CRYPT_CTX ctx, const SEC_UCHAR *pucCipherText, SEC_UINT32 ulCLen, SEC_UCHAR *pucPlainText, SEC_UINT32 *pulPLen, SEC_UINT32 ulPTLen);
#else
CRYPTOLINKDLL SEC_UINT32 CRYPT_decryptUpdate(CRYPT_CTX ctx, const SEC_UCHAR *pucCipherText, SEC_UINT32 ulCLen, SEC_UCHAR *pucPlainText, SEC_UINT32 *pulPLen);
#endif

/*
    Func Name:  CRYPT_decryptUpdate_ex
*/
/**
* @defgroup CRYPT_decryptUpdate_ex
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_decryptUpdate_ex (
* CRYPT_CTX   ctx,
* const SEC_UCHAR*  pucCipherText,
* SEC_UINT32  ulCLen,
* SEC_UCHAR*  pucPlainText,
* SEC_UINT32* pulPLen,
* SEC_UINT32 ulPTLen);
* @endcode
*
* @par Purpose
* This is used to decrypt cipher text to plain
text using decryption algorithm.
*
* @par Description
* CRYPT_decryptUpdate_ex function uses the initialized
symmetric context to decrypt ciphertext to get plaintext.
It uses
* symmetric decryption. If cipher text length is
less than or equal to block size of decryption
algorithm, it carry
* forwards to CRYPT_decryptFinal_ex. If cipher text
length is greater than block size of decryption algorithm then multiple
* block size of cipher text decrypts and rest are
carry forward to final block.
*
* @param[in] ctx The context to be used [N/A]
* @param[in] pucCipherText Encrypted ciphertext [N/A]
* @param[in] ulCLen Ciphertext Length [N/A]
* @param[out] pucPlainText Decrypted plaintext [N/A]
* @param[out] pulPLen Length of decrypted plaintext [N/A]
* @param[in] ulPTLen size of the buffer pucPlainText  [N/A]
*
* @retval SEC_UINT32 If invalid argument [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 If invalid context [SEC_CRYPT_ERR_INVALID_CTX|N/A]
* @retval SEC_UINT32 On successful return [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 On other error [SEC_ERR|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* - The safe API corresponding to this is CRYPT_decryptUpdate_sf
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_decryptUpdate_ex(CRYPT_CTX ctx, const SEC_UCHAR *pucCipherText, SEC_UINT32 ulCLen, SEC_UCHAR *pucPlainText, SEC_UINT32 *pulPLen, SEC_UINT32 ulPTLen);

/*
    Func Name:  CRYPT_decryptFinal
*/
/**
* @defgroup CRYPT_decryptFinal
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_decryptFinal (
* CRYPT_CTX*  pCtx,
* SEC_UCHAR*  pucPlainText,
* SEC_UINT32* pulPLen,
* SEC_UINT32 ulPTLen);
* @endcode
*
* @par Purpose
* This is used to decrypt only last block of cipher
text to plain text using decryption algorithm.
*
* @par Description
* CRYPT_decryptFinal function completes one symmetric
decryption operation. It frees the context. In case of block ciphers
* the remaining of the decrypted plaintext is the output.
In stream ciphers, as all the plaintext would have been
* output in decryptupdate, there will not be any plantext
output. If decryptinit succeeds, decryptfinal must be called
* to free the context.
*
* @param[in] pCtx Address of The context to be used [N/A]
* @param[out] pucPlainText Final decrypted plaintext [N/A]
* @param[out] pulPLen Length of last decrypted plaintext [N/A]
* @param[in] ulPTLen size of the buffer pucPlainText  [N/A]
*
* @retval SEC_UINT32 If invalid argument [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 If invalid context [SEC_CRYPT_ERR_INVALID_CTX|N/A]
* @retval SEC_UINT32 If invalid pad mode in context
[SEC_CRYPT_ERR_UNDEFINED_PADDING|N/A]
* @retval SEC_UINT32 The pad bits are incorrect.
When after decrypting the padding bits dont match the pad mode.
It can be
* due to incorrect Key or IV as that will produce junk
plaintext with random padding bits, which dont match the padding
* mode. [SEC_CRYPT_ERR_INVALID_PADDING|N/A]
* @retval SEC_UINT32 If invalid ciphertext length given for
decryption. (ciphertext length was not integral no. of blocks)
* [SEC_ERR_INVALID_DATA_LEN|N/A]
* @retval SEC_UINT32 On successful return [SEC_SUCCESS|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* - The safe API corresponding to this is CRYPT_decryptFinal_sf
*
* @par Related Topics
* N/A
*/
#ifndef HERT_RAT_COMPATIBILITY
CRYPTOLINKDLL SEC_UINT32 CRYPT_decryptFinal(CRYPT_CTX *pCtx, SEC_UCHAR *pucPlainText, SEC_UINT32 *pulPLen, SEC_UINT32 ulPTLen);
#else
CRYPTOLINKDLL SEC_UINT32 CRYPT_decryptFinal(CRYPT_CTX *pCtx, SEC_UCHAR *pucPlainText, SEC_UINT32 *pulPLen);
#endif

/*
    Func Name:  CRYPT_decryptFinal_ex
*/
/**
* @defgroup CRYPT_decryptFinal_ex
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_decryptFinal_ex (
* CRYPT_CTX*  pCtx,
* SEC_UCHAR*  pucPlainText,
* SEC_UINT32* pulPLen,
* SEC_UINT32 ulPTLen);
* @endcode
*
* @par Purpose
* This is used to decrypt only last block of cipher
text to plain text using decryption algorithm.
*
* @par Description
* CRYPT_decryptFinal_ex function completes one symmetric
decryption operation. It frees the context. In case of block ciphers
* the remaining of the decrypted plaintext is the output.
In stream ciphers, as all the plaintext would have been
* output in decryptupdate, there will not be any plantext
output. If decryptinit succeeds, decryptfinal must be called
* to free the context.
*
* @param[in] pCtx Address of The context to be used [N/A]
* @param[out] pucPlainText Final decrypted plaintext [N/A]
* @param[out] pulPLen Length of last decrypted plaintext [N/A]
* @param[in] ulPTLen size of the buffer pucPlainText  [N/A]
*
* @retval SEC_UINT32 If invalid argument [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 If invalid context [SEC_CRYPT_ERR_INVALID_CTX|N/A]
* @retval SEC_UINT32 If invalid pad mode in context
[SEC_CRYPT_ERR_UNDEFINED_PADDING|N/A]
* @retval SEC_UINT32 The pad bits are incorrect.
When after decrypting the padding bits dont match the pad mode.
It can be
* due to incorrect Key or IV as that will produce junk
plaintext with random padding bits, which dont match the padding
* mode. [SEC_CRYPT_ERR_INVALID_PADDING|N/A]
* @retval SEC_UINT32 If invalid ciphertext length given for
decryption. (ciphertext length was not integral no. of blocks)
* [SEC_ERR_INVALID_DATA_LEN|N/A]
* @retval SEC_UINT32 On successful return [SEC_SUCCESS|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* - The safe API corresponding to this is CRYPT_decryptFinal_sf
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_decryptFinal_ex(CRYPT_CTX *pCtx, SEC_UCHAR *pucPlainText, SEC_UINT32 *pulPLen, SEC_UINT32 ulPTLen);
/*
    Func Name:  CRYPT_decrypt
*/
/**
* @defgroup CRYPT_decrypt
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_decrypt (
* SEC_UINT32       ulAlgId,
* const SEC_UCHAR* pucKey,
* SEC_UINT32       ulKeyLen,
* const SEC_UCHAR* pucIV,
* SEC_UINT32       ulIVLen,
* const SEC_UCHAR*       pucCipherText,
* SEC_UINT32       ulCLen,
* SEC_UCHAR*       pucPlainText,
* SEC_UINT32*      pulPLen,
* SEC_UINT32       ulPTLen);
* @endcode
*
* @par Purpose
* This is used to decrypt cipher text to plain text
using decryption algorithm.
*
* @par Description
* CRYPT_decrypt function decrypts cipher text to plain
text using decryption algorithm. It creates
symmetric context by
* creating algorithm object, padding object,
opmode object. After decryption, symmetric context needs to be freed.
*
* @param[in] ulAlgId The algorithm Id [N/A]
* @param[in] pucKey The key value to be used [N/A]
* @param[in] ulKeyLen The length of the key. This length must be according to algorithm used and must match with
* CRYPT_SYM_keyLen(ulAlgId)
* @param[in] pucIV InitializationVector(CBC FB,OFB modes) [N/A]
* @param[in] pucCipherText To be decrypted ciphertext [N/A]
* @param[in] ulCLen Length of ciphertext [N/A]
* @param[in] ulIVLen Length of IV. pass 0 if no IV. This length must be according to algorithm used and must match with
* CRYPT_SYM_ivLen(ulAlgId)[N/A]
* @param[out] pucPlainText Decrypted Plaintext [N/A]
* @param[out] pulPLen Length of plaintext [N/A]
* @param[in] ulPTLen size of the buffer pucPlainText  [N/A]
*
*
* @retval SEC_UINT32 If invalid Algorithm Id
[SEC_ERR_INVALID_ALGID|N/A]
* @retval SEC_UINT32 If invalid Padding Mode.
Padding mode set on Algorithm is not defined
* [SEC_CRYPT_ERR_UNDEFINED_PADDING|N/A]
* @retval SEC_UINT32 The pad bits are incorrect.
When after decrypting, the padding bits does not match the pad mode.
* It can be due to incorrect Key or IV as that will
produce junk plaintext with random padding bits, which does not match
* the padding mode [SEC_CRYPT_ERR_INVALID_PADDING|N/A]
* @retval SEC_UINT32 If invalid Keylength for algprithm
[SEC_ERR_INVALID_KEY_LEN|N/A]
* @retval SEC_UINT32 If invalid IVlength for algorithm
[SEC_ERR_INVALID_IV_LEN|N/A]
* @retval SEC_UINT32 Invalid ciphertext length given
for decryption in update. (ciphertext length was not integral number
* of blocks) [SEC_ERR_INVALID_DATA_LEN|N/A]
* @retval SEC_UINT32 On successful return [SEC_SUCCESS|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* - When using 3DES, application need to take care that K1(first 64 bits), K2(middle 64 bits), and K3(last 64 bits)
*    are different from each other.
* - The safe API corresponding to this is CRYPT_decrypt_sf
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
*
* @par Related Topics
* N/A
*/
#ifndef HERT_RAT_COMPATIBILITY
CRYPTOLINKDLL SEC_UINT32 CRYPT_decrypt(SEC_UINT32 ulAlgId, const SEC_UCHAR *pucKey, SEC_UINT32 ulKeyLen, const SEC_UCHAR *pucIV, SEC_UINT32 ulIVLen, const SEC_UCHAR *pucCipherText, SEC_UINT32 ulCLen, SEC_UCHAR *pucPlainText, SEC_UINT32 *pulPLen,
                                       SEC_UINT32 ulPTLen);
#else
CRYPTOLINKDLL SEC_UINT32 CRYPT_decrypt(SEC_UINT32 ulAlgId, const SEC_UCHAR *pucKey, SEC_UINT32 ulKeyLen, const SEC_UCHAR *pucIV, SEC_UINT32 ulIVLen, const SEC_UCHAR *pucCipherText, SEC_UINT32 ulCLen, SEC_UCHAR *pucPlainText, SEC_UINT32 *pulPLen);
#endif

/*
    Func Name:  CRYPT_decrypt_ex
*/
/**
* @defgroup CRYPT_decrypt_ex
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_decrypt_ex (
* SEC_UINT32       ulAlgId,
* const SEC_UCHAR* pucKey,
* SEC_UINT32       ulKeyLen,
* const SEC_UCHAR* pucIV,
* SEC_UINT32       ulIVLen,
* const SEC_UCHAR*       pucCipherText,
* SEC_UINT32       ulCLen,
* SEC_UCHAR*       pucPlainText,
* SEC_UINT32*      pulPLen,
* SEC_UINT32       ulPTLen);
* @endcode
*
* @par Purpose
* This is used to decrypt cipher text to plain text
using decryption algorithm.
*
* @par Description
* CRYPT_decrypt_ex function decrypts cipher text to plain
text using decryption algorithm. It creates
symmetric context by
* creating algorithm object, padding object,
opmode object. After decryption, symmetric context needs to be freed.
*
* @param[in] ulAlgId The algorithm Id [N/A]
* @param[in] pucKey The key value to be used [N/A]
* @param[in] ulKeyLen The length of the key. This length must be according to algorithm used and must match with
* CRYPT_SYM_keyLen(ulAlgId)
* @param[in] pucIV InitializationVector(CBC FB,OFB modes) [N/A]
* @param[in] pucCipherText To be decrypted ciphertext [N/A]
* @param[in] ulCLen Length of ciphertext [N/A]
* @param[in] ulIVLen Length of IV. pass 0 if no IV. This length must be according to algorithm used and must match with
* CRYPT_SYM_ivLen(ulAlgId)[N/A]
* @param[out] pucPlainText Decrypted Plaintext [N/A]
* @param[out] pulPLen Length of plaintext [N/A]
* @param[in] ulPTLen size of the buffer pucPlainText  [N/A]
*
* @retval SEC_UINT32 If invalid Algorithm Id
[SEC_ERR_INVALID_ALGID|N/A]
* @retval SEC_UINT32 If invalid Padding Mode.
Padding mode set on Algorithm is not defined
* [SEC_CRYPT_ERR_UNDEFINED_PADDING|N/A]
* @retval SEC_UINT32 The pad bits are incorrect.
When after decrypting, the padding bits does not match the pad mode.
* It can be due to incorrect Key or IV as that will
produce junk plaintext with random padding bits, which does not match
* the padding mode [SEC_CRYPT_ERR_INVALID_PADDING|N/A]
* @retval SEC_UINT32 If invalid Keylength for algprithm
[SEC_ERR_INVALID_KEY_LEN|N/A]
* @retval SEC_UINT32 If invalid IVlength for algorithm
[SEC_ERR_INVALID_IV_LEN|N/A]
* @retval SEC_UINT32 Invalid ciphertext length given
for decryption in update. (ciphertext length was not integral number
* of blocks) [SEC_ERR_INVALID_DATA_LEN|N/A]
* @retval SEC_UINT32 On successful return [SEC_SUCCESS|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* - When using 3DES, application need to take care that K1(first 64 bits), K2(middle 64 bits), and K3(last 64 bits)
*    are different from each other.
* - The safe API corresponding to this is CRYPT_decrypt_sf
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
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_decrypt_ex(SEC_UINT32 ulAlgId, const SEC_UCHAR *pucKey, SEC_UINT32 ulKeyLen, const SEC_UCHAR *pucIV, SEC_UINT32 ulIVLen, const SEC_UCHAR *pucCipherText, SEC_UINT32 ulCLen, SEC_UCHAR *pucPlainText, SEC_UINT32 *pulPLen,
                                          SEC_UINT32 ulPTLen);

/*
    Func Name:  CRYPT_digestInit
*/
/**
* @defgroup CRYPT_digestInit
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_digestInit (
* CRYPT_CTX* pCtx,
* SEC_UINT32 ulAlgType);
* @endcode
*
* @par Purpose
* This is used to initialize the context for a digest operation.
*
* @par Description
* CRYPT_digestInit function initializes the context for
a digest operation. This function must be called before any
* Update or Final operations. This method of digesting
is used when data is present in multiple buffers or not available
* all at once.
*
* @param[in] ulAlgType The algorithm id for digest [N/A]
* @param[out] pCtx The context which will be filled [N/A]
*
* @retval SEC_UINT32 If context is initialized [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 If algorithm undefined [SEC_ERR_INVALID_ALGID|N/A]
* @retval SEC_UINT32 If memory alloc for
context failed [SEC_ERR_MALLOC_FAIL|N/A]
* @retval SEC_UINT32 If arguments are missing [SEC_ERR_INVALID_ARG|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* - API supports following unsafe algorithms:
*   - ALGID_MD4 (CID_MD4)
*   - ALGID_MD5 (CID_MD5)
*   - ALGID_SHA1 (CID_SHA1) \n
*   As these algorithms are unsafe, if application are using these algorithms, application must take care of any
*   security issues that may happen due to usage of these algorithms.
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_digestInit(CRYPT_CTX *pCtx, SEC_UINT32 ulAlgType);

/*
    Func Name:  CRYPT_digestUpdate
*/
/**
* @defgroup CRYPT_digestUpdate
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_digestUpdate (
* CRYPT_CTX  ctx,
* const SEC_UCHAR* pucData,
* SEC_UINT32 ulDataLen);
* @endcode
*
* @par Purpose
* This is used to perform digest operation on chunks of data.
*
* @par Description
* CRYPT_digestUpdate function performs digest operation on
chunks of data. This method of digesting is used when data is
* present in multiple buffers or not available all at once.
A digestInit must have been called before calling this
* function.
*
* @param[in] ctx The context for digest [N/A]
* @param[in] pucData The input data [N/A]
* @param[in] ulDataLen The input data length [N/A]
*
* @retval SEC_UINT32 If partial digest is calculated [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 If context values are not correct
[SEC_CRYPT_ERR_INVALID_CTX|N/A]
* @retval SEC_UINT32 If arguments are missing [SEC_ERR_INVALID_ARG|N/A]
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
CRYPTOLINKDLL SEC_UINT32 CRYPT_digestUpdate(CRYPT_CTX ctx, const SEC_UCHAR *pucData, SEC_UINT32 ulDataLen);

/*
    Func Name:  CRYPT_digestFinal
*/
/**
* @defgroup CRYPT_digestFinal
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_digestFinal (
* CRYPT_CTX*  pCtx,
* SEC_UCHAR*  pucDigest,
* SEC_UINT32* pulDigestLen,
* SEC_UINT32 ulInpDigestLen);
* @endcode
*
* @par Purpose
* This is used to complete digest operation on remaining data, and is
called at the end of digest operation.
*
* @par Description
* CRYPT_digestFinal function completes digest operation on remaining data, and
is called at the end of digest operation.
* A digestInit must have been called before calling this function. This
function calculates the digest and frees the
* context. The memory for digest must already have been allocated.
*
* @param[in] pCtx The context for digest [N/A]
* @param[out] pucDigest The digest [N/A]
* @param[out] pulDigestLen The output digest length [N/A]
* @param[in] ulInpDigestLen Size of buffer pucDigest [N/A]

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
* - The safe API corresponding to this is CRYPT_digestFinal_sf
*
* @par Related Topics
* N/A
*/
// This macro is added to keep compatibility with HERT RAT upgrade  , this will be removed once HERT solution migrated
// to new API
#ifndef HERT_RAT_COMPATIBILITY
CRYPTOLINKDLL SEC_UINT32 CRYPT_digestFinal(CRYPT_CTX *pCtx, SEC_UCHAR *pucDigest, SEC_UINT32 *pulDigestLen, SEC_UINT32 ulInpDigestLen);
#else
CRYPTOLINKDLL SEC_UINT32 CRYPT_digestFinal(CRYPT_CTX *pCtx, SEC_UCHAR *pucDigest, SEC_UINT32 *pulDigestLen);
#endif

/*
    Func Name:  CRYPT_digestFinal_ex
*/
/**
* @defgroup CRYPT_digestFinal_ex
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_digestFinal_ex (
* CRYPT_CTX*  pCtx,
* SEC_UCHAR*  pucDigest,
* SEC_UINT32* pulDigestLen,
* SEC_UINT32 ulInpDigestLen);
* @endcode
*
* @par Purpose
* This is used to complete digest operation on remaining data, and is
called at the end of digest operation.
*
* @par Description
* CRYPT_digestFinal_ex function completes digest operation on remaining data, and
is called at the end of digest operation.
* A digestInit must have been called before calling this function. This
function calculates the digest and frees the
* context. The memory for digest must already have been allocated.
*
* @param[in] pCtx The context for digest [N/A]
* @param[out] pucDigest The digest [N/A]
* @param[out] pulDigestLen The output digest length [N/A]
* @param[in] ulInpDigestLen Size of buffer pucDigest [N/A]

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
* - The safe API corresponding to this is CRYPT_digestFinal_sf
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_digestFinal_ex(CRYPT_CTX *pCtx, SEC_UCHAR *pucDigest, SEC_UINT32 *pulDigestLen, SEC_UINT32 ulInpDigestLen);

/*
    Func Name:  CRYPT_digest
*/
/**
* @defgroup CRYPT_digest
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_digest (
* SEC_UINT32  ulAlgType,
* const SEC_UCHAR*  pucData,
* SEC_UINT32  ulDataLen,
* SEC_UCHAR*  pucDigest,
* SEC_UINT32* pulDigestLen,
* SEC_UINT32 ulInpDigestLen);
* @endcode
*
* @par Purpose
* This is used to compute digest of a given data block.
*
* @par Description
* CRYPT_digest function computes digest of a given data block. Calls init,
update, and final. This function is used when
* data is present all at once. There is no need of calling Init, Update,
Final and digest can be calculated in one go.
* Also, context is not required.
*
* @param[in] ulAlgType Digest algorithm [N/A]
* @param[in] pucData The data [N/A]
* @param[in] ulDataLen The data length [N/A]
* @param[out] pucDigest The digest [N/A]
* @param[out] pulDigestLen The digest length [N/A]
* @param[in] ulInpDigestLen Size of buffer pucDigest [N/A]
*
* @retval SEC_UINT32 If digest is calculated [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 If context values got corrupted internally
[SEC_CRYPT_ERR_INVALID_CTX|N/A]
* @retval SEC_UINT32 If arguments are missing [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 If algorithm undefined [SEC_ERR_INVALID_ALGID|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* - The safe API corresponding to this is CRYPT_digest_sf
* - API supports following unsafe algorithms:
*   - ALGID_MD4 (CID_MD4)
*   - ALGID_MD5 (CID_MD5)
*   - ALGID_SHA1 (CID_SHA1) \n
*   As these algorithms are unsafe, if application are using these algorithms, application must take care of any
*   security issues that may happen due to usage of these algorithms.
* @par Related Topics
* N/A
*/
// This macro is added to keep compatibility with HERT RAT upgrade,
// this will be removed once HERT solution migrated to new API
#ifndef HERT_RAT_COMPATIBILITY
CRYPTOLINKDLL SEC_UINT32 CRYPT_digest(SEC_UINT32 ulAlgType, const SEC_UCHAR *pucData, SEC_UINT32 ulDataLen, SEC_UCHAR *pucDigest, SEC_UINT32 *pulDigestLen, SEC_UINT32 ulInpDigestLen);
#else
CRYPTOLINKDLL SEC_UINT32 CRYPT_digest(SEC_UINT32 ulAlgType, const SEC_UCHAR *pucData, SEC_UINT32 ulDataLen, SEC_UCHAR *pucDigest, SEC_UINT32 *pulDigestLen);
#endif

/*
    Func Name:  CRYPT_digest_ex
*/
/**
* @defgroup CRYPT_digest_ex
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_digest_ex (
* SEC_UINT32  ulAlgType,
* const SEC_UCHAR*  pucData,
* SEC_UINT32  ulDataLen,
* SEC_UCHAR*  pucDigest,
* SEC_UINT32* pulDigestLen,
* SEC_UINT32 ulInpDigestLen);
* @endcode
*
* @par Purpose
* This is used to compute digest of a given data block.
*
* @par Description
* CRYPT_digest_ex function computes digest of a given data block. Calls init,
update, and final. This function is used when
* data is present all at once. There is no need of calling Init, Update,
Final and digest can be calculated in one go.
* Also, context is not required.
*
* @param[in] ulAlgType Digest algorithm [N/A]
* @param[in] pucData The data [N/A]
* @param[in] ulDataLen The data length [N/A]
* @param[out] pucDigest The digest [N/A]
* @param[out] pulDigestLen The digest length [N/A]
* @param[in] ulInpDigestLen Size of buffer pucDigest [N/A]
*
* @retval SEC_UINT32 If digest is calculated [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 If context values got corrupted internally
[SEC_CRYPT_ERR_INVALID_CTX|N/A]
* @retval SEC_UINT32 If arguments are missing [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 If algorithm undefined [SEC_ERR_INVALID_ALGID|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* - The safe API corresponding to this is CRYPT_digest_sf
* - API supports following unsafe algorithms:
*   - ALGID_MD4 (CID_MD4)
*   - ALGID_MD5 (CID_MD5)
*   - ALGID_SHA1 (CID_SHA1) \n
*   As these algorithms are unsafe, if application are using these algorithms, application must take care of any
*   security issues that may happen due to usage of these algorithms.
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_digest_ex(SEC_UINT32 ulAlgType, const SEC_UCHAR *pucData, SEC_UINT32 ulDataLen, SEC_UCHAR *pucDigest, SEC_UINT32 *pulDigestLen, SEC_UINT32 ulInpDigestLen);
/*
    Func Name:  CRYPT_hmacInit
*/
/**
* @defgroup CRYPT_hmacInit
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_hmacInit (
* CRYPT_CTX* pCtx,
* SEC_UINT32 ulAlgType,
* const SEC_UCHAR * pucKey,
* SEC_UINT32 ulKlen);
* @endcode
*
* @par Purpose
* This is used to initialize the context for a hmac operation.
*
* @par Description
* CRYPT_hmacInit function initializes the context for a hmac operation.
This function must be called before any Update
* or Final operations. This method of MACing is used when data is present
in multiple buffers or not available all at once.
*
* @param[in] ulAlgType The algorithm id for HMAC [N/A]
* @param[in] pucKey Key for hmac [N/A]
* @param[in] ulKlen Key length [N/A]
* @param[out] pCtx The context which will be filled [N/A]
*
* @retval SEC_UINT32 If context is initialized [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 If algorithm undefined [SEC_ERR_INVALID_ALGID|n/A]
* @retval SEC_UINT32 If memory alloc for context failed
[SEC_ERR_MALLOC_FAIL|N/A]
* @retval SEC_UINT32 If arguments are missing [SEC_ERR_INVALID_ARG|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* - API supports following unsafe algorithms:
*   - ALGID_HMAC_MD4 (CID_HMAC_MD4)
*   - ALGID_HMAC_MD5 (CID_HMAC_MD5)
*   - ALGID_HMAC_SHA1(CID_HMAC_SHA1) \n
*   As these algorithms are unsafe, if application are using these algorithms, application must take care of any
*   security issues that may happen due to usage of these algorithms.
* - CRYPT_hmacCtxFree should be called to free context when CRYPT_hmacInit return failure
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_hmacInit(CRYPT_CTX *pCtx, SEC_UINT32 ulAlgType, const SEC_UCHAR *pucKey, SEC_UINT32 ulKlen);

/*
    Func Name:  CRYPT_cmacInit
*/
/**
* @defgroup CRYPT_cmacInit
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_cmacInit (
* CRYPT_CTX* pCtx,
* SEC_UINT32 ulAlgType,
* const SEC_UCHAR * pucKey,
* SEC_UINT32 ulKlen);
* @endcode
*
* @par Purpose
* This is used to initialize the context for a cmac operation.
*
* @par Description
* CRYPT_cmacInit function initializes the context for a cmac operation.
This function must be called before any Update
* or Final operations. This method of MACing is used when data is
present in multiple buffers or not available all at once.
*
* @param[in] ulAlgType The algorithm id for CMAC [N/A]
* @param[in] pucKey Key for cmac [N/A]
* @param[in] ulKlen Key length [N/A]
* @param[out] pCtx The context which will be filled [N/A]
*
* @retval SEC_UINT32 If context is initialized [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 If algorithm undefined [SEC_ERR_INVALID_ALGID|N/A]
* @retval SEC_UINT32 If memory alloc for context
failed [SEC_ERR_MALLOC_FAIL|N/A]
* @retval SEC_UINT32 If arguments are missing [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 If the key length is invalid [SEC_ERR_INVALID_KEY_LEN|N/A]
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
CRYPTOLINKDLL SEC_UINT32 CRYPT_cmacInit(CRYPT_CTX *pCtx, SEC_UINT32 ulAlgType, const SEC_UCHAR *pucKey, SEC_UINT32 ulKlen);

/*
    Func Name:  CRYPT_hmacUpdate
*/
/**
* @defgroup CRYPT_hmacUpdate
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_hmacUpdate (
* CRYPT_CTX ctx,
* const SEC_UCHAR * pucData,
* SEC_UINT32 ulDataLen);
* @endcode
*
* @par Purpose
* This is used to perform hmac operation on chunks of data.
*
* @par Description
* CRYPT_hmacUpdate function performs hmac operation on
chunks of data. This method of MACing is used when data is present
* in multiple buffers or not available all at once.
A hmacInit must have been called before calling this function.
*
* @param[in] ctx The context for HMAC [N/A]
* @param[in] pucData The input data [N/A]
* @param[in] ulDataLen The input data length [N/A]
*
* @retval SEC_UINT32 If partial digest is calculated
[SEC_SUCCESS|N/A]
* @retval SEC_UINT32 If context values are not correct
[SEC_CRYPT_ERR_INVALID_CTX|N/A]
* @retval SEC_UINT32 If arguments are missing [SEC_ERR_INVALID_ARG|N/A]
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
CRYPTOLINKDLL SEC_UINT32 CRYPT_hmacUpdate(CRYPT_CTX ctx, const SEC_UCHAR *pucData, SEC_UINT32 ulDataLen);

/*
    Func Name:  CRYPT_cmacUpdate
*/
/**
* @defgroup CRYPT_cmacUpdate
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_cmacUpdate (
* CRYPT_CTX ctx,
* const SEC_UCHAR * pucData,
* SEC_UINT32 ulDataLen);
* @endcode
*
* @par Purpose
* This is used to perform cmac operation on chunks of data.
*
* @par Description
* CRYPT_cmacUpdate function performs cmac operation on chunks of data.
This method of MACing is used when data is present
* in multiple buffers or not available all at once. A cmacInit must
have been called before calling this function.
*
* @param[in] ctx The context for CMAC [N/A]
* @param[in] pucData The input data [N/A]
* @param[in] ulDataLen The input data length [N/A]
*
* @retval SEC_UINT32 If partial digest is calculated [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 If context values are not
correct [SEC_CRYPT_ERR_INVALID_CTX|N/A]
* @retval SEC_UINT32 If arguments are missing [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 If any operational error [SEC_ERR|N/A]
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
CRYPTOLINKDLL SEC_UINT32 CRYPT_cmacUpdate(CRYPT_CTX ctx, const SEC_UCHAR *pucData, SEC_UINT32 ulDataLen);

/*
    Func Name:  CRYPT_hmacFinal
*/
/**
* @defgroup CRYPT_hmacFinal
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_hmacFinal (
* CRYPT_CTX*  pCtx,
* SEC_UCHAR*  pucMAC,
* SEC_UINT32* pulMACLen,
* SEC_UINT32 ulInpMACLen);
* @endcode
*
* @par Purpose
* This is used to complete hmac operation on remaining data.
Called at the end of MAC operation.
*
* @par Description
* CRYPT_hmacFinal function completes hmac operation on
remaining data. It is called at the end of MAC operation. A hmacInit
* must have been called before calling this function.
This function calculates the MAC and frees the context. The memory
* for MAC must have already been allocated.
*
* @param[in] pCtx The context for digest [N/A]
* @param[out] pucMAC The hashed MAC [N/A]
* @param[out] pulMACLen The MAC length [N/A]
* @param[in] ulInpMACLen The pucMAC  buffer size [N/A]
*
* @retval SEC_UINT32 If digest is calculated [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 If context values are not correct
[SEC_CRYPT_ERR_INVALID_CTX|N/A]
* @retval SEC_UINT32 If arguments are missing [SEC_ERR_INVALID_ARG|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* - The safe API corresponding to this is CRYPT_hmacFinal_sf
*
* @par Related Topics
* N/A
*/
#ifndef HERT_RAT_COMPATIBILITY
CRYPTOLINKDLL SEC_UINT32 CRYPT_hmacFinal(CRYPT_CTX *pCtx, SEC_UCHAR *pucMAC, SEC_UINT32 *pulMACLen, SEC_UINT32 ulInpMACLen);
#else
CRYPTOLINKDLL SEC_UINT32 CRYPT_hmacFinal(CRYPT_CTX *pCtx, SEC_UCHAR *pucMAC, SEC_UINT32 *pulMACLen);
#endif

/*
    Func Name:  CRYPT_hmacFinal_ex
*/
/**
* @defgroup CRYPT_hmacFinal_ex
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_hmacFinal_ex (
* CRYPT_CTX*  pCtx,
* SEC_UCHAR*  pucMAC,
* SEC_UINT32* pulMACLen,
* SEC_UINT32 ulInpMACLen);
* @endcode
*
* @par Purpose
* This is used to complete hmac operation on remaining data.
Called at the end of MAC operation.
*
* @par Description
* CRYPT_hmacFinal_ex function completes hmac operation on
remaining data. It is called at the end of MAC operation. A hmacInit
* must have been called before calling this function.
This function calculates the MAC and frees the context. The memory
* for MAC must have already been allocated.
*
* @param[in] pCtx The context for digest [N/A]
* @param[out] pucMAC The hashed MAC [N/A]
* @param[out] pulMACLen The MAC length [N/A]
* @param[in] ulInpMACLen The pucMAC  buffer size [N/A]
*
* @retval SEC_UINT32 If digest is calculated [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 If context values are not correct
[SEC_CRYPT_ERR_INVALID_CTX|N/A]
* @retval SEC_UINT32 If arguments are missing [SEC_ERR_INVALID_ARG|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* - The safe API corresponding to this is CRYPT_hmacFinal_sf
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_hmacFinal_ex(CRYPT_CTX *pCtx, SEC_UCHAR *pucMAC, SEC_UINT32 *pulMACLen, SEC_UINT32 ulInpMACLen);

/*
    Func Name:  CRYPT_cmacFinal
*/
/**
* @defgroup CRYPT_cmacFinal
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_cmacFinal (
* CRYPT_CTX* pCtx,
* SEC_UCHAR* pucMAC,
* SEC_UINT32* pulMACLen);
* @endcode
*
* @par Purpose
* This is used to complete cmac operation on remaining data.
Called at the end of MAC operation.
*
* @par Description
* CRYPT_cmacFinal function completes cmac operation on
remaining data. It is called at the end of MAC operation. A cmacInit
* must have been called before calling this function.
This function calculates the MAC and frees the context. The memory
* for MAC must have already been allocated.
*
* @param[in] pCtx The context for digest [N/A]
* @param[out] pucMAC The hashed MAC [N/A]
* @param[out] pulMACLen The MAC length [N/A]
*
* @retval SEC_UINT32 If digest is calculated [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 If context values are not
correct [SEC_CRYPT_ERR_INVALID_CTX|N/A]
* @retval SEC_UINT32 If arguments are missing [SEC_ERR_INVALID_ARG|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* - The safe API corresponding to this is CRYPT_cmacFinal_sf
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_cmacFinal(CRYPT_CTX *pCtx, SEC_UCHAR *pucMAC, SEC_UINT32 *pulMACLen);

/*
    Func Name:  CRYPT_hmac
*/
/**
* @defgroup CRYPT_hmac
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_hmac (
* SEC_UINT32 ulAlgType,
* const SEC_UCHAR * pucKey,
* SEC_UINT32 ulKeyLen,
* const SEC_UCHAR * pucData,
* SEC_UINT32 ulDataLen,
* SEC_UCHAR * pucMAC,
* SEC_UINT32 * pulMACLen,
* SEC_UINT32 ulInpMACLen);
* @endcode
*
* @par Purpose
* This is used to compute hmac of a given data block.
*
* @par Description
* CRYPT_hmac function computes hmac of a given data block.
Calls init, update, and final. This function is used when
* data is present all at once. There is no need of calling
Init, Update, Final and MAC can be calculated in one go.
* Context is not needed here.
*
* @param[in] ulAlgType HMAC algorithm [N/A]
* @param[in] pucKey The key [N/A]
* @param[in] ulKeyLen The key length [N/A]
* @param[in] pucData The data [N/A]
* @param[in] ulDataLen The data length [N/A]
* @param[out] pucMAC The hashed MAC [N/A]
* @param[out] pulMACLen The digest length [N/A]
* @param[in] ulInpMACLen The pucMAC  buffer size [N/A]
*
* @retval SEC_UINT32 If digest is calculated [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 If context values are not correct
[SEC_CRYPT_ERR_INVALID_CTX|N/A]
* @retval SEC_UINT32 If arguments are missing [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 If memory alloc for context
failed [SEC_ERR_MALLOC_FAIL|N/A]
* @retval SEC_UINT32 If algorithm undefined [SEC_ERR_INVALID_ALGID|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* - The safe API corresponding to this is CRYPT_hmac_sf
* - API supports following unsafe algorithms:
*   - ALGID_HMAC_MD4 (CID_HMAC_MD4)
*   - ALGID_HMAC_MD5 (CID_HMAC_MD5)
*   - ALGID_HMAC_SHA1(CID_HMAC_SHA1) \n
*   As these algorithms are unsafe, if application are using these algorithms, application must take care of any
*   security issues that may happen due to usage of these algorithms.
* @par Related Topics
* N/A
*/
// This macro is added to keep compatibility with HERT RAT upgrade  , this will be removed once HERT solution migrated
// to new API
#ifndef HERT_RAT_COMPATIBILITY
CRYPTOLINKDLL SEC_UINT32 CRYPT_hmac(SEC_UINT32 ulAlgType, const SEC_UCHAR *pucKey, SEC_UINT32 ulKeyLen, const SEC_UCHAR *pucData, SEC_UINT32 ulDataLen, SEC_UCHAR *pucMAC, SEC_UINT32 *pulMACLen, SEC_UINT32 ulInpMACLen);
#else
CRYPTOLINKDLL SEC_UINT32 CRYPT_hmac(SEC_UINT32 ulAlgType, const SEC_UCHAR *pucKey, SEC_UINT32 ulKeyLen, const SEC_UCHAR *pucData, SEC_UINT32 ulDataLen, SEC_UCHAR *pucMAC, SEC_UINT32 *pulMACLen);
#endif

/*
    Func Name:  CRYPT_hmac_ex
*/
/**
* @defgroup CRYPT_hmac_ex
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_hmac_ex (
* SEC_UINT32 ulAlgType,
* const SEC_UCHAR * pucKey,
* SEC_UINT32 ulKeyLen,
* const SEC_UCHAR * pucData,
* SEC_UINT32 ulDataLen,
* SEC_UCHAR * pucMAC,
* SEC_UINT32 * pulMACLen,
* SEC_UINT32 ulInpMACLen);
* @endcode
*
* @par Purpose
* This is used to compute hmac of a given data block.
*
* @par Description
* CRYPT_hmac_ex function computes hmac of a given data block.
Calls init, update, and final. This function is used when
* data is present all at once. There is no need of calling
Init, Update, Final and MAC can be calculated in one go.
* Context is not needed here.
*
* @param[in] ulAlgType HMAC algorithm [N/A]
* @param[in] pucKey The key [N/A]
* @param[in] ulKeyLen The key length [N/A]
* @param[in] pucData The data [N/A]
* @param[in] ulDataLen The data length [N/A]
* @param[out] pucMAC The hashed MAC [N/A]
* @param[out] pulMACLen The digest length [N/A]
* @param[in] ulInpMACLen The pucMAC  buffer size [N/A]
*
* @retval SEC_UINT32 If digest is calculated [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 If context values are not correct
[SEC_CRYPT_ERR_INVALID_CTX|N/A]
* @retval SEC_UINT32 If arguments are missing [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 If memory alloc for context
failed [SEC_ERR_MALLOC_FAIL|N/A]
* @retval SEC_UINT32 If algorithm undefined [SEC_ERR_INVALID_ALGID|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* - The safe API corresponding to this is CRYPT_hmac_sf
* - API supports following unsafe algorithms:
*   - ALGID_HMAC_MD4 (CID_HMAC_MD4)
*   - ALGID_HMAC_MD5 (CID_HMAC_MD5)
*   - ALGID_HMAC_SHA1(CID_HMAC_SHA1) \n
*   As these algorithms are unsafe, if application are using these algorithms, application must take care of any
*   security issues that may happen due to usage of these algorithms.
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_hmac_ex(SEC_UINT32 ulAlgType, const SEC_UCHAR *pucKey, SEC_UINT32 ulKeyLen, const SEC_UCHAR *pucData, SEC_UINT32 ulDataLen, SEC_UCHAR *pucMAC, SEC_UINT32 *pulMACLen, SEC_UINT32 ulInpMACLen);

/*
    Func Name:  CRYPT_cmac
*/
/**
* @defgroup CRYPT_cmac
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_cmac (
* SEC_UINT32 ulAlgType,
* const SEC_UCHAR* pucKey,
* SEC_UINT32  ulKeyLen,
* const SEC_UCHAR*  pucData,
* SEC_UINT32  ulDataLen,
* SEC_UCHAR*  pucMAC,
* SEC_UINT32* pulMACLen);
* @endcode
*
* @par Purpose
* This is used to compute cmac of a given data block.
*
* @par Description
* CRYPT_cmac function computes cmac  of a given data block.
Calls init, update, and final. This function is used when
* data is present all at once. There is no need of calling
Init, Update, Final and MAC can be calculated in one go.
* Context is not needed here.
*
* @param[in] ulAlgType CMAC algorithm [N/A]
* @param[in] pucKey The key [N/A]
* @param[in] ulKeyLen The key length [N/A]
* @param[in] pucData The data [N/A]
* @param[in] ulDataLen The data length [N/A]
* @param[out] pucMAC The hashed MAC [N/A]
* @param[out] pulMACLen The digest length [N/A]
*
* @retval SEC_UINT32 If digest is calculated [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 If context values are not correct
[SEC_CRYPT_ERR_INVALID_CTX|N/A]
* @retval SEC_UINT32 If arguments are missing [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 If memory alloc for context
failed [SEC_ERR_MALLOC_FAIL|N/A]
* @retval SEC_UINT32 If algorithm undefined [SEC_ERR_INVALID_ALGID|N/A]
* @retval SEC_UINT32 If the key length is invalid
[SEC_ERR_INVALID_KEY_LEN|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* - The safe API corresponding to this is CRYPT_cmac_sf
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_cmac(SEC_UINT32 ulAlgType, const SEC_UCHAR *pucKey, SEC_UINT32 ulKeyLen, const SEC_UCHAR *pucData, SEC_UINT32 ulDataLen, SEC_UCHAR *pucMAC, SEC_UINT32 *pulMACLen);

#ifndef __IPSI_NO_AES_XCBC__

/*
    Func Name:  CRYPT_xcbcInit
*/
/**
 * @defgroup CRYPT_xcbcInit
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_xcbcInit (
 *                            CRYPT_CTX* pCtx,
 *                            const SEC_UINT32 ulAlgType,
 *                            const SEC_UCHAR * pucKey,
 *                            const SEC_UINT32 ulKlen);
 * @endcode
 *
 * @par Purpose
 * This is used to initialize the context for a xcbc MAC operation.
 *
 * @par Description
 * CRYPT_xcbcInit function initializes the context for a xcbc MAC operation.
 * This function must be called before any Update
 * or Final operations. This method of MACing is used when data is
 * present in multiple buffers or not available all at once.
 *
 * @param[in] ulAlgType The algorithm id for XCBC MAC [N/A]
 * @param[in] pucKey Key for XCBC MAC [N/A]
 * @param[in] ulKlen Key length [N/A]
 * @param[out] pCtx The context which will be filled [N/A]
 *
 * @retval SEC_UINT32 If context is initialized [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 If algorithm
 * undefined [SEC_ERR_INVALID_ALGID|N/A]
 * @retval SEC_UINT32 If memory alloc
 * for context failed [SEC_ERR_MALLOC_FAIL|N/A]
 * @retval SEC_UINT32 If arguments
 * are missing [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 If the key length is invalid
 * [SEC_ERR_INVALID_KEY_LEN|N/A]
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
CRYPTOLINKDLL SEC_UINT32 CRYPT_xcbcInit(CRYPT_CTX *pCtx, const SEC_UINT32 ulAlgType, const SEC_UCHAR *pucKey, const SEC_UINT32 ulKlen);

/*
    Func Name:  CRYPT_xcbcUpdate
*/
/**
 * @defgroup CRYPT_xcbcUpdate
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_xcbcUpdate (
 *                              CRYPT_CTX ctx,
 *                              const SEC_UCHAR * pucData,
 *                              SEC_UINT32 ulDataLen);
 * @endcode
 *
 * @par Purpose
 * This is used to perform xcbc MAC operation on chunks of data.
 *
 * @par Description
 * CRYPT_xcbcUpdate function performs xcbc MAC operation on chunks of
 * data. This method of MACing is used when data is present
 * in multiple buffers or not available all at once. A xcbcInit must
 * have been called before calling this function.
 *
 * @param[in] ctx The context for xcbc mac [N/A]
 * @param[in] pucData The input data [N/A]
 * @param[in] ulDataLen The input data length [N/A]
 *
 * @retval SEC_UINT32 If partial MAC is calculated [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 If context values are
 * not correct [SEC_CRYPT_ERR_INVALID_CTX|N/A]
 * @retval SEC_UINT32 If arguments
 * are missing [SEC_ERR_INVALID_ARG|N/A]
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
CRYPTOLINKDLL SEC_UINT32 CRYPT_xcbcUpdate(CRYPT_CTX ctx, const SEC_UCHAR *pucData, SEC_UINT32 ulDataLen);

/*
    Func Name:  CRYPT_xcbcFinal
*/
/**
* @defgroup CRYPT_xcbcFinal
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_xcbcFinal (
*                           CRYPT_CTX* pCtx,
*                           SEC_UCHAR* pucMAC,
*                           SEC_UINT32* pulMACLen);
* @endcode
*
* @par Purpose
* This is used to complete xcbc MAC operation on remaining data.
* Called at the end of MAC operation.
*
* @par Description
* CRYPT_xcbcFinal function completes xcbc MAC operation on
* remaining data. It is called at the end of MAC operation. A xcbcInit
* must have been called before calling this function.
* This function calculates the MAC and frees the context. The memory
* for MAC must have already been allocated.
*
* @param[in] pCtx The context for xcbc mac [N/A]
* @param[out] pucMAC The XCBC MAC [N/A]
* @param[out] pulMACLen The MAC length [N/A]
*

* @retval SEC_UINT32 If MAC is calculated [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 If context values are
* not correct [SEC_CRYPT_ERR_INVALID_CTX|N/A]
* @retval SEC_UINT32 If arguments
* are missing [SEC_ERR_INVALID_ARG|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* - The safe API corresponding to this is CRYPT_xcbcFinal_sf
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_xcbcFinal(CRYPT_CTX *pCtx, SEC_UCHAR *pucMAC, SEC_UINT32 *pulMACLen);

/*
    Func Name:  CRYPT_xcbc
*/
/**
* @defgroup CRYPT_xcbc
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_xcbc (SEC_UINT32 ulAlgType,
*                        const SEC_UCHAR* pucKey,
*                        SEC_UINT32 upucKeyLen,
*                        const SEC_UCHAR * pucData,
*                        SEC_UINT32 ulDataLen,
*                        SEC_UCHAR * pucMAC,
*                        SEC_UINT32 * pulMACLen);

* @endcode
*
* @par Purpose
* This is used to compute aes xcbc mac of a given data block.
*
* @par Description
* CRYPT_xcbc function computes aes xcbc mac of a given data
* block. Calls init, update, and final. This function is used when
* data is present all at once. There is no need of calling
* Init, Update, Final and XCBC MAC can be calculated in one go.
* Context is not needed here. The MAC generated will be of length 128 bits.
*
* @param[in] ulAlgType XCBC MAC algorithm [N/A]
* @param[in] pucKey The key [N/A]
* @param[in] upucKeyLen The key length [N/A]
* @param[in] pucData The data [N/A]
* @param[in] ulDataLen The data length [N/A]
* @param[out] pucMAC The XCBC MAC [N/A]
* @param[out] pulMACLen The MAC length [N/A]
*
* @retval SEC_UINT32 If MAC is calculated [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 If context values are
* not correct [SEC_CRYPT_ERR_INVALID_CTX|N/A]
* @retval SEC_UINT32 If arguments
* are missing [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 If memory alloc for context
* failed [SEC_ERR_MALLOC_FAIL|N/A]
* @retval SEC_UINT32 If algorithm
* undefined [SEC_ERR_INVALID_ALGID|N/A]
* @retval SEC_UINT32 If the key length
* is invalid [SEC_ERR_INVALID_KEY_LEN|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
*  - Only AES algorithm is supported.
*     This API can be used for AES-XCBC-MAC-96 algorithm. It will
*     generate XCBC MAC of 128 bits. Leftmost 96 bits will be taken for XCBC-MAC-96
* - The safe API corresponding to this is CRYPT_xcbc_sf
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_xcbc(SEC_UINT32 ulAlgType, const SEC_UCHAR *pucKey, SEC_UINT32 upucKeyLen, const SEC_UCHAR *pucData, SEC_UINT32 ulDataLen, SEC_UCHAR *pucMAC, SEC_UINT32 *pulMACLen);

/*
    Func Name:  CRYPT_XCBC_size
*/
/**
 * @defgroup CRYPT_XCBC_size
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_XCBC_size(
 * SEC_UINT32 ulAlgType)
 * @endcode
 *
 * @par Purpose
 * This is used to retrieve the size of the XCBC MAC output for an algorithm.
 *
 * @par Description
 * CRYPT_XCBC_size function retrieves the size of the XCBC MAC output
 * for an algorithm.
 *
 * @param[in] ulAlgType Algorithm ID for XCBC MAC [N/A]
 *
 * @retval SEC_UINT32 If valid algorithm [Size of the XCBC MAC|N/A]
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
CRYPTOLINKDLL SEC_UINT32 CRYPT_XCBC_size(SEC_UINT32 ulAlgType);

#endif

/*
    Func Name:  CRYPT_random
*/
/**
* @defgroup CRYPT_random
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_random(
* SEC_UCHAR* pBuf,
* SEC_UINT32 ulLen)
* @endcode
*
* @par Purpose
* This is used to generate a random number.
*
* @par Description
* CRYPT_random function generates a random number and stores it
into the supplied buffer.
*
* @param[in] ulLen Size of the buffer in bytes [N/A]
* @param[out] pBuf Buffer to hold the random number [N/A]
*
* @retval SEC_UINT32 On successful return [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 If pBuf is NULL or pucBuf is 0 [SEC_ERR_INVALID_ARG|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* - If application does not set seeds and entropy on frequent intervals, it can lead to vulnerable keys or random
*   value. It is recommended to set seeds and entropy on frequent intervals or update the version that migrates the
*   random implementation based on DRBG.
* - The API should not be called in multiple thread parallely in case of ANSI X9.31. Application should use lock if the
*    API need to be called in multithread.
* - The random number generator based on ANSI X9.31 is out of date. It is not recommended
*    to use X9.31 standard random generator. The DRBG Module must be instantiated for
*    DRBG operations to take effect. Refer IPSI_CRYPT_rand_bytes for more information.
* - Before calling this API, please make sure that DRBG is initialized for random number generation by calling
*   IPSI_CRYPT_rand_init. If application don't want to use DRBG then appplication should call
*   IPSI_CRYPT_enable_drbg(0) to disable the DRBG Random feature.
* - DRBG is enabled by default.
*
* @par Related Topics
* - IPSI_CRYPT_rand_bytes
*
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_random(SEC_UCHAR *pBuf, SEC_UINT32 ulLen);

/*
    Func Name:  CRYPT_randRange
*/
/**
* @defgroup CRYPT_randRange
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT CRYPT_randRange (
* SEC_BIGINT_S* limit0,
* SEC_BIGINT_S* limit1,
* SEC_BIGINT_S* num);
* @endcode
*
* @par Purpose
* This is used to generate a random number and stores it
into the supplied Big Integer structure.
*
* @par Description
* CRYPT_randRange function generates a random number and
stores it into the supplied Big Integer structure. The random
* number will lie between the two limits supplied, where
limit1 != limit0.
*
* @param[in] limit0 One of the limits between which the
random number should lie [N/A]
* @param[in] limit1 One of the limits between which the
random number should lie [N/A]
* @param[out] num The generated random number [N/A]
*
* @retval SEC_UINT On successful generation [SEC_SUCCESS|N/A]
* @retval SEC_UINT If input pointers are NULL or one of the
limits have length 0 or if limit1 == limit0
* [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT If some internal error happens after random number generation [SEC_ERR|N/A]
* @retval SEC_UINT other errors indicate random failure errors [N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* - If application does not set seeds and entropy on frequent intervals, it can lead to vulnerable keys or random
*   value. It is recommended to set seeds and entropy on frequent intervals or update the version that migrates the
*   random implementation based on DRBG.
* - The API should not be called in multiple thread parallely in case of ANSI X9.31. Application should use lock if the
* API need to be called in multithread.
* - Before calling this API, please make sure that DRBG is initialized for random number generation by calling
*   IPSI_CRYPT_rand_init. If application don't want to use DRBG then appplication should call
*   IPSI_CRYPT_enable_drbg(0) to disable the DRBG Random feature.
* - DRBG is enabled by default.
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT CRYPT_randRange(SEC_BIGINT_S *limit0, SEC_BIGINT_S *limit1, SEC_BIGINT_S *num);

/*
    Func Name:  SEC_Rand
*/
/**
* @defgroup SEC_Rand
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 SEC_Rand(SEC_UINT32 ulRange)
* @endcode
*
* @par Purpose
* This is used to generate a random integer in a given range.
*
* @par Description
* SEC_Rand function generates a random integer in a given range.
The random integer generated is in between 0 and user
* input value.
*
* @param[in] ulRange Upper range [N/A]
*
* @retval SEC_UINT32 Generates random integer in a given range
[Random integer|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* - If random number generation is fails, zero will be returned\n
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
CRYPTOLINKDLL SEC_UINT32 SEC_Rand(SEC_UINT32 ulRange);

#ifndef __IPSI_NO_DRBG__
/*
    Func Name:  SEC_Rand_ex
*/
/**
* @defgroup SEC_Rand_ex
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 SEC_Rand_ex(SEC_UINT32 lRange, SEC_UINT32 * uiErrCode)
* @endcode
*
* @par Purpose
* This is used to generate a random integer in a given range.
*
* @par Description
* SEC_Rand_ex function generates a random integer in a given range.
The random integer generated is in between 0 and user
* input value.
*
* @param[in] ulRange Upper range [N/A]
* @param[out] uiErrCode The error code will be updated in this variable.
* \n
*       The error code values are SEC_ERR_INVALID_ARG, SEC_ERR and can be any of
*       random failure values. When DRBG enabled, refer IPSI_CRYPT_rand_bytes for the
*       error values. On Success, this variable will be updated SEC_SUCCESS [N/A]
*
*
* @retval SEC_UINT32 Generates random integer in a given range
[Random integer|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* - If random number generation is fails, zero will be returned\n
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
CRYPTOLINKDLL SEC_UINT32 SEC_Rand_ex(SEC_UINT32 ulRange, SEC_UINT32 *uiErrCode);
#endif // __IPSI_NO_DRBG__

/*
    Func Name:  CRYPT_getRandState
*/
/**
* @defgroup CRYPT_getRandState
* @ingroup HLF High Level Functions
* @image html Security.jpg
* <b> User should use this API to get the random state when the access permission
* of the file needs to be configure. </b>
* @par Prototype
* @code
* PRNG_STATE_S CRYPT_getRandState ();
* @endcode
*
* @par Purpose
* This is used to retrieve the internal state of the random number generator.
*
* @par Description
* CRYPT_getRandState function retrieves the internal state of the
random number generator so that it may be stored in
* a file.
*
* @par Parameters
* N/A
*
* @retval PRNG_STATE_S* On Success [The internal state of the
random number generator|N/A]
* @retval PRNG_STATE_S* If the hardware mode is set [SEC_NULL|N/A]
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
CRYPTOLINKDLL PRNG_STATE_S *CRYPT_getRandState(void);

/*
    Func Name:  CRYPT_setRandState
*/
/**
* @defgroup CRYPT_setRandState
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT CRYPT_setRandState (
* const PRNG_STATE_S* pState);
* @endcode
*
* @par Purpose
* This is used to set the internal state of the random number generator.
*
* @par Description
* CRYPT_setRandState function sets the internal state of the
random number generator so that a saved state may be restored.
*
* @param[in] pState The state which is to be restored [N/A]
*
* @retval SEC_UINT After successful set [SEC_SUCCESS|N/A]
* @retval SEC_UINT If argument is NULL [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT If the hardware mode is set
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
CRYPTOLINKDLL SEC_UINT CRYPT_setRandState(const PRNG_STATE_S *pState);

/*
    Func Name:  CRYPT_genKeyPair
*/
/**
* @defgroup CRYPT_genKeyPair
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_genKeyPair(
* const ALG_PARA_S* pAlgInfo,
* SEC_PKEY_S* pKeyPair)
* @endcode
*
* @par Purpose
* This is used to generate a key pair for a given asymmetric algorithm.
*
* @par Description
* CRYPT_genKeyPair function generates a key pair for a given
asymmetric algorithm. The structure must be added with
* parameters appropriate for the algorithm to be used.
The key structure must be allocated earlier using CRYPT_PKEY_new
* function.
*
* @param[in] pAlgInfo Algorithm information structure [N/A]
* @param[out] pKeyPair The generated key pair structure [N/A]
*
* @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 Invalid Arguments [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 If memory allocation failure [SEC_ERR_MALLOC_FAIL|N/A]
* @retval SEC_UINT32 If key size not supported [SEC_ERR_INVALID_KEY_LEN|N/A]
* @retval SEC_UINT32 If library is not initialized [SEC_ERR_INITLIB|N/A]
* @retval SEC_UINT32 Incorrect Algorithm ID [SEC_ERR_INVALID_ALGID|N/A]
* @retval SEC_UINT32 On Failure [SEC_ERR_KEY_GEN_FAILED|N/A]
* @retval SEC_UINT32 On memcpy_s failure error [SEC_ERR_MEMCPY_FAILED|N/A]
* @retval SEC_UINT32 On other errors [SEC_ERR|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* 1. RSA key pair generation involves finding large prime factors.
*    So this would be slower and as the modulus size increases it will take
*    more time. And the CPU might be hogged during RSA key pair generation.
*    In product scenario where the RSA keys are not generated offline
*    then it advised to use CRYPT_genKeyPair_ex.
* \n\n
* 2. CRYPT_genKeyPair_ex extends CRYPT_genKeyPair functionality.
*    This additionally invokes the callback function set, periodically after the
*    configured time. In the call back function product can clear the watch dog
*    so that the timer starts from the beginning and will avoid reboot of system.
*    Or in the call back function product can relinquish the CPU by having
*    appropriate task_delay.
* \n\n
* 3. If application does not set seeds and entropy on frequent intervals, it can lead to vulnerable keys or random
*    value. It is recommended to set seeds and entropy on frequent intervals or update the version that migrates the
*    random implementation based on DRBG.
* \n\n
* 4. Before calling this API, please make sure that DRBG is initialized for random number generation by calling
*   IPSI_CRYPT_rand_init. If application don't want to use DRBG then appplication should call
*   IPSI_CRYPT_enable_drbg(0) to disable the DRBG Random feature.
* \n\n
* 5. DRBG is enabled by default.
/n
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_genKeyPair(const ALG_PARA_S *pAlgInfo, SEC_PKEY_S *pKeyPair);

#ifndef IPSI_RSA_NOEXTENDEDKEYPAIR__

/**
 * @defgroup IPSI_GEN_KPAIR_CTX_S
 * @ingroup sec_cryptoStructures
 * @par Prototype
 * @code
 * typedef SEC_VOID IPSI_GEN_KPAIR_CTX_S;
 * @endcode
 *
 * @par Description
 * This is the keypair context which is to be passed as first parameter to
 * IPSI_GEN_KEYPAIR_TIME_CB callback. Currently NULL is passed
 * , This Reserved for future use.
 */
typedef SEC_VOID IPSI_GEN_KPAIR_CTX_S;

/*
    Func Name:  IPSI_GEN_KEYPAIR_TIME_CB
*/
/**
 * @defgroup IPSI_GEN_KEYPAIR_TIME_CB
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * typedef SEC_INT32 (*IPSI_GEN_KEYPAIR_TIME_CB)(
 *    IPSI_GEN_KPAIR_CTX_S *, SEC_VOID *);
 *
 * @endcode
 *
 * @par Purpose
 * Callback function called from CRYPT_genKeyPair_ex and CRYPT_genKeyPair_ex2.
 *
 * @par Description
 * This function is called from CRYPT_genKeyPair_ex and CRYPT_genKeyPair_ex2 function after
 * the configured number of seconds or CPU ticks (according to the  API) elapses.
 * When the callback function is called the Application data that is set is
 * passed back as second parameter to the callback.
 * The first parameter to callback function is
 * reserved for future use. Currently the return value from the callback function
 * is ignored and intended only for future use.
 *
 *
 * @retval SEC_INT32 Currently Nothing to be returned [N/A|N/A]
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
typedef SEC_INT32 (*IPSI_GEN_KEYPAIR_TIME_CB)(IPSI_GEN_KPAIR_CTX_S *, SEC_VOID *);

/*
    Func Name:  CRYPT_genKeyPair_ex
*/
/**
 * @defgroup CRYPT_genKeyPair_ex
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_genKeyPair_ex (
 *     const ALG_PARA_S *pAlgInfo, SEC_PKEY_S *pKeyPair,
 *     IPSI_GEN_KEYPAIR_TIME_CB pfGenKeyPairCb,
 *     SEC_UINT32 uTimeSec, SEC_VOID * pvAppData);
 *
 * @endcode
 *
 * @par Purpose
 * This extends CRYPT_genKeyPair functionality. This additionally invokes
 * the callback function set, periodically after the configured time.
 *
 * @par Description
 * CRYPT_genKeyPair_ex function generates a key pair for a given
 * asymmetric algorithm. The structure must be added with
 * parameters appropriate for the algorithm to be used.
 * The key structure must be allocated earlier using CRYPT_PKEY_new
 * function. This function also calls the set callback function after
 * the configured number of seconds elapses.
 * When the callback function is called the Application data that is set is
 * passed back as Second parameter to the callback.
 * The First Parameter to callback function is
 * reserved for future use. Currently the return value from the callback function
 * is ignored and intended only for future use.
 *
 *
 * @param[in] pAlgInfo Algorithm information structure [N/A]
 * @param[out] pKeyPair The generated key pair struct [N/A]
 * @param[in] pfGenKeyPairCb Callback function
 * which wil be called after timeSec seconds[N/A]
 * @param[in] pvAppData Application data passed back
 * to the callback function [N/A]
 * @param[in] utimeSec Time after which the callback function will be
 * called in seconds.
 *
 * @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 Invalid Arguments [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 If memory allocation failure [SEC_ERR_MALLOC_FAIL|N/A]
 * @retval SEC_UINT32 If key size not supported [SEC_ERR_INVALID_KEY_LEN|N/A]
 * @retval SEC_UINT32 If library is not initialized [SEC_ERR_INITLIB|N/A]
 * @retval SEC_UINT32 Incorrect Algorithm ID [SEC_ERR_INVALID_ALGID|N/A]
 * @retval SEC_UINT32 On Failure [SEC_ERR_KEY_GEN_FAILED|N/A]
 * @retval SEC_UINT32 On other errors [SEC_ERR|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * \n
 * 1. It is recommended that the uTimeSec should be set to 1 or more second
 *    lesser than the time required by the application.
 * \n\n
 * 2. Currently this api can be used only for RSA key pair.
 * \n\n
 * 3. RSA key pair generation involves finding large prime factors.
 *    So this would be slower and as the modulus size increases it will take more time.
 *    And the CPU might be hogged during RSA key pair generation. In product scenario
 *    where the RSA keys are not generated offline then it advised
 *    to use CRYPT_genKeyPair_ex.
 * \n\n
 * 4. CRYPT_genKeyPair_ex extends CRYPT_genKeyPair functionality.
 *    This additionally invokes the callback function set, periodically after
 *    the configured time. In the call back function product can clear the watch
 *    dog so that the timer starts from the beginning and will avoid reboot
 *    of system. Or in the call back function product can relinquish the CPU
 *    by having appropriate task_delay.
 * \n\n
 * 5. If application does not set seeds and entropy on frequent intervals, it can lead to vulnerable keys or random
 *    value. It is recommended to set seeds and entropy on frequent intervals or update the version that migrates the
 *   random implementation based on DRBG.
 * \n\n
 * 6. Maximum timeout value is 3600 seconds.
 * \n\n
 * 7. Before calling this API, please make sure that DRBG is initialized for random number generation by calling
 *   IPSI_CRYPT_rand_init. If application don't want to use DRBG then appplication should call
 *   IPSI_CRYPT_enable_drbg(0) to disable the DRBG Random feature.
 * \n\n
 * 8. DRBG is enabled by default.
 * \n
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_genKeyPair_ex(const ALG_PARA_S *pAlgInfo, SEC_PKEY_S *pKeyPair, IPSI_GEN_KEYPAIR_TIME_CB pfGenKeyPairCb, SEC_UINT32 uTimeSec, SEC_VOID *pvAppData);

/*
    Func Name:  CRYPT_genKeyPair_ex2
*/
/**
 * @defgroup CRYPT_genKeyPair_ex2
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_genKeyPair_ex2 (
 *     const ALG_PARA_S *pAlgInfo, SEC_PKEY_S *pKeyPair,
 *     IPSI_GEN_KEYPAIR_TIME_CB pfGenKeyPairCb,
 *     SEC_UINT32 uTimeTick, SEC_VOID * pvAppData);
 *
 * @endcode
 *
 * @par Purpose
 * This extends CRYPT_genKeyPair functionality. This additionally invokes
 * the callback function set, periodically after the configured time in cpu tick.
 *
 * @par Description
 * CRYPT_genKeyPair_ex2 function generates a key pair for a given
 * asymmetric algorithm. The structure must be added with
 * parameters appropriate for the algorithm to be used.
 * The key structure must be allocated earlier using CRYPT_PKEY_new
 * function. This function also calls the set callback function after
 * the configured number of cpu ticks.
 * When the callback function is called the Application data that is set is
 * passed back as second parameter to the callback.
 * The First Parameter to callback function is
 * reserved for future use. Currently the return value from the callback function
 * is ignored and intended only for future use.
 *
 *
 * @param[in] pAlgInfo Algorithm information structure [N/A]
 * @param[out] pKeyPair The generated key pair struct [N/A]
 * @param[in] pfGenKeyPairCb Callback function
 * which wil be called after uTimeTick CPU ticks[N/A]
 * @param[in] pvAppData Application data passed back
 * to the callback function [N/A]
 * @param[in] uTimeTick CPU tick Time after which the callback function will be
 * called in cputicks.
 *
 * @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 Invalid Arguments [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 If memory allocation failure [SEC_ERR_MALLOC_FAIL|N/A]
 * @retval SEC_UINT32 If key size not supported [SEC_ERR_INVALID_KEY_LEN|N/A]
 * @retval SEC_UINT32 If library is not initialized [SEC_ERR_INITLIB|N/A]
 * @retval SEC_UINT32 Incorrect Algorithm ID [SEC_ERR_INVALID_ALGID|N/A]
 * @retval SEC_UINT32 On Failure [SEC_ERR_KEY_GEN_FAILED|N/A]
 * @retval SEC_UINT32 On other errors [SEC_ERR|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * \n
 * 1. It is recommended that the application passes a slightly reduced time input than what is needed
 * (For Ex: If the total wait time needed is 5 seconds, application may pass 4 seconds,
 * in the case of this API, equivalent ticks should be passed).
 * \n\n
 * 2. Currently this api can be used only for RSA key pair.
 * \n\n
 * 3. RSA key pair generation involves finding large prime factors.
 *    So this would be slower and as the modulus size increases it will take more time.
 *    And the CPU might be hogged during RSA key pair generation. In product scenario
 *    where the RSA keys are not generated offline then it advised
 *    to use CRYPT_genKeyPair_ex2 or CRYPT_genKeyPair_ex.
 * \n\n
 * 4. CRYPT_genKeyPair_ex2 extends CRYPT_genKeyPair functionality.
 *    This additionally invokes the callback function set, periodically after
 *    the configured time. In the call back function product can clear the watch
 *    dog so that the timer starts from the beginning and will avoid reboot
 *    of system. Or in the call back function product can relinquish the CPU
 *    by having appropriate task_delay.
 * \n\n
 * 5. If application does not set seeds and entropy on frequent intervals, it can lead to vulnerable keys or random
 *    value. It is recommended to set seeds and entropy on frequent intervals or update the version that migrates the
 *    random implementation based on DRBG.
 * \n\n
 * 6. Before calling this API, please make sure that DRBG is initialized for random number generation by calling
 *   IPSI_CRYPT_rand_init. If application don't want to use DRBG then appplication should call
 *   IPSI_CRYPT_enable_drbg(0) to disable the DRBG Random feature.
 * \n\n
 * 7. DRBG is enabled by default.
 * \n
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_genKeyPair_ex2(const ALG_PARA_S *pAlgInfo, SEC_PKEY_S *pKeyPair, IPSI_GEN_KEYPAIR_TIME_CB pfGenKeyPairCb, SEC_UINT32 uTimeTick, SEC_VOID *pvAppData);

#endif

/*
    Func Name:  CRYPT_ecPointToOcts
*/
/**
* @defgroup CRYPT_ecPointToOcts
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UCHAR *CRYPT_ecPointToOcts(
* const EC_POINT_S* pPt,
* SEC_UINT* puiLen)
* @endcode
*
* @par Purpose
* This is used to convert an EC_POINT_S structure to an octet string.
*
* @par Description
* CRYPT_ecPointToOcts function converts an EC_POINT_S
structure to an octet string, following the method mentioned in
* ANSI X.9 62 for point to octet conversion. The
returned buffer can be freed after use.
*
* @param[in] pPt The point to be converted [N/A]
* @param[out] puiLen The length of the octet string [N/A]
*
* @retval SEC_UCHAR* On error [0|N/A]
* @retval SEC_UCHAR* Otherwise [The octet string|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* Octect string pointer has to be freed by the user using ipsi_free().
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UCHAR *CRYPT_ecPointToOcts(const EC_POINT_S *pPt, SEC_UINT *puiLen);

/*
    Func Name:  CRYPT_octsToECPoint
*/
/**
* @defgroup CRYPT_octsToECPoint
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* EC_POINT_S *CRYPT_octsToECPoint(
* const SEC_UCHAR* pucOcts,
* SEC_UINT uiLen)
* @endcode
*
* @par Purpose
* This is used to convert an octet string to an EC_POINT_S structure.
*
* @par Purpose
* CRYPT_octsToECPoint function converts an octet string to
an EC_POINT_S structure, following the method mentioned in
* ANSI X.9 62 for octet to point conversion. The returned
struct can be freed after use.
*
* @param[in] pucOcts The octet to be converted [N/A]
* @param[in] uiLen The length of the octet string [N/A]
*
* @retval EC_POINT_S* On error [0|N/A]
* @retval EC_POINT_S* Otherwise [The EC point|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* The EC pointer has to be freed by the user using ipsi_free().
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL EC_POINT_S *CRYPT_octsToECPoint(const SEC_UCHAR *pucOcts, SEC_UINT uiLen);

/*
    Func Name:  CRYPT_createECParamsById
*/
/**
* @defgroup CRYPT_createECParamsById
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* EC_PARA_S *CRYPT_createECParamsById(
* SEC_UINT32 uiParamID)
* @endcode
*
* @par Purpose
* This is used to get the param structure for a given
well-known parameter Id.
*
* @par Description
* CRYPT_createECParamsById function returns the param structur
e for a given well-known parameter Id. The parameters returned
* can be freed by user. However, the user has to make sure
that memory is freed using functions from the same library
* from which this function is called.
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
* Pointer has to be freed by the user using ipsi_free().
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL EC_PARA_S *CRYPT_createECParamsById(SEC_UINT32 uiParamID);

/**
 * @defgroup CRYPT_freeECParams
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_VOID CRYPT_freeECParams(EC_PARA_S **dpEcPara)
 * @endcode
 *
 * @par Purpose
 * This function is to free the freeECParams.
 *
 * @par Description
 * This function is to free the freeECParams by providing the value for EC_PARA_S structure.
 *
 * @param[in] dpEcPara pointer to EC_PARA_S, which need to be free.
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
 * CRYPT_createECParamsById_ex
 */
CRYPTOLINKDLL SEC_VOID CRYPT_freeECParams(EC_PARA_S **dpEcPara);

/*
    Func Name:  CRYPT_computeDHKey
*/
/**
* @defgroup CRYPT_computeDHKey
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_computeDHKey(
* const SEC_PKEY_S* pPvtKey,
* const SEC_PKEY_S* pPubKey,
* SEC_UCHAR* pucKey,
* SEC_UINT32* pulKeyLen)
* @endcode
*
* @par Purpose
* This is used to generate a DH shared secret key from
private key and peer public key.
*
* @par Description
* CRYPT_computeDHKey function generates a DH shared secret
key from private key and peer public key. The memory for the
* shared secret must have been allocated prior to calling.
*
* @param[in] pPvtKey Own private key [N/A]
* @param[in] pPubKey Peer public key [N/A]
* @param[out] pucKey Shared secret key [N/A]
* @param[in/out] pulKeyLen Length of generated shared secret key [N/A]
*
* @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 If algorithm is not supported [SEC_CRYPT_ERR_UNDEFINED_KEY_OPERATION|N/A]
* @retval SEC_UINT32 Invalid Key [SEC_CRYPT_ERR_INVALID_PKEY|N/A]
* @retval SEC_UINT32 Invalid Arguments [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 If library is not initialized [SEC_ERR_INITLIB|N/A]
* @retval SEC_UINT32 If memcpy_s fails [SEC_ERR_MEMCPY_FAILED|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* - The safe API corresponding to this is CRYPT_computeDHKey_sf
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_computeDHKey(const SEC_PKEY_S *pPvtKey, const SEC_PKEY_S *pPubKey, SEC_UCHAR *pucKey, SEC_UINT32 *pulKeyLen);

/*
    Func Name:  CRYPT_genDHPara
*/
/**
 * @defgroup CRYPT_genDHPara
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_genDHPara(
 * SEC_UINT32 ulPrimeLen,
 * SEC_UINT32 ulGenrtor,
 * SEC_PKEY_S* pPara)
 * @endcode
 *
 * @par purpose
 * This is used to generate the DH parameters p and g.
 *
 * @par Description
 * CRYPT_genDHPara function generates the DH parameters p & g.
 *
 * @param[in] ulPrimeLen Length of the prime p [N/A]
 * @param[in] ulGenrtor Generator g [N/A]
 * @param[out] pPara Key structure with parameters [N/A]
 *
 * @retval SEC_UINT32 Always [SEC_CRYPT_ERR_UNDEFINED_KEY_OPERATION|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * \n
 * This is unimplemented.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_genDHPara(SEC_UINT32 ulPrimeLen, SEC_UINT32 ulGenrtor, SEC_PKEY_S *pPara);

#ifndef __IPSI_NO_DH_PARA__

/*
    Func Name:  CRYPT_createDHParams
*/
/**
 * @defgroup CRYPT_createDHParams
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_INT CRYPT_createDHParams(DH_PARA_S *pstDhPara,
 *                           SEC_DH_PRIME_E enPrime)
 * @endcode
 *
 * @par Purpose
 * This function is used to get the dh parameters defined in the RFC 2409
 * and 3526.
 *
 * @par Description
 * CRYPT_createDHParams function will update the input argument pstDhPara
 * corresponding to the enum enPrime. The dh parameters updated in the
 * pstDhPara is according to the RFC 2409 or 3526 depending on the enPrime.
 *
 * @param[in] enPrime enum to identify the prime [N/A]
 * @param[out] pstDhPara DH parameter structure [N/A]
 *
 * @retval SEC_INT On Success [SEC_SUCCESS|N/A]
 * @retval SEC_INT If algorithm is not supported [SEC_ERROR|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * \n
 *	- DH Group 1 prime is GROUPID_RFC2409_1
 *	- DH Group 2 prime is GROUPID_RFC2409_2
 *	- DH Group 5 prime is GROUPID_RFC3256_5
 *	- DH Group 14 prime is GROUPID_RFC3526_14
 *	- DH Group 15 prime is GROUPID_RFC3526_15
 *	- DH Group 16 prime is GROUPID_RFC3526_16
 *
 *	- The generator updated in the pstDhPara is always 2.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT CRYPT_createDHParams(DH_PARA_S *pstDhPara, SEC_DH_PRIME_E enPrime);

#endif

/*
    Func Name:  CRYPT_computeECDHKey
*/
/**
* @defgroup CRYPT_computeECDHKey
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_computeECDHKey(
* const SEC_PKEY_S* pPvtKey,
* const SEC_PKEY_S* pPubKey,
* SEC_UCHAR* pucKey,
* SEC_UINT32* pulKeyLen)
* @endcode
*
* @par Purpose
* This is used to generate a ECDH shared secret key.
*
* @par Description
* CRYPT_computeECDHKey function generates a ECDH shared
secret key from private key and peer public key. The memory for
* the shared secret must have been allocated prior to calling.
*
* @param[in] pPvtKey Own private key [N/A]
* @param[in] pPubKey Peer public key [N/A]
* @param[out] pucKey Shared secret key [N/A]
* @param[in/out] pulKeyLen Length of generated shared secret key [N/A]
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
*   - CRYPT_computeECDHKey will only output standard diffie helman primitive.
*     To get the modified diffie helman primitive use CRYPT_computeECDHKey_ex API.
*   - The minimum size of output key buffer should be atleast the input public key size.
* - The safe API corresponding to this is CRYPT_computeECDHKey_sf
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_computeECDHKey(const SEC_PKEY_S *pPvtKey, const SEC_PKEY_S *pPubKey, SEC_UCHAR *pucKey, SEC_UINT32 *pulKeyLen);

#define IPSI_X25519_KEYLEN 32

typedef struct stECX_PUB_KEY {
    SEC_UINT32 uiLen;
    SEC_UCHAR pubKey[IPSI_X25519_KEYLEN];
} ECX_PUB_KEY_S;

typedef struct stECX_PRV_KEY {
    SEC_UINT32 uiLen;
    SEC_UCHAR prvKey[IPSI_X25519_KEYLEN];
} ECX_PRV_KEY_S;

typedef struct stECX_KEY {
    SEC_UINT32 keyType;
    ECX_PUB_KEY_S *pPubKey;
    ECX_PRV_KEY_S *pPrvKey;
} ECX_KEY_S;

#define CRYPT_X25519_GET_PKEY_PUB_ST(pstPKeyPair) ((ECX_KEY_S *)((pstPKeyPair)->pKey))->pPubKey
#define CRYPT_X25519_GET_PUBKEY(pstPKeyPair) ((ECX_KEY_S *)((pstPKeyPair)->pKey))->pPubKey->pubKey
#define CRYPT_X25519_GET_PUBKEY_LEN(pstPKeyPair) ((ECX_KEY_S *)((pstPKeyPair)->pKey))->pPubKey->uiLen
#define CRYPT_X25519_SHARED_KEYLEN IPSI_X25519_KEYLEN

/*
    Func Name:  CRYPT_computeX25519Key
*/
/**
 * @defgroup CRYPT_computeX25519Key
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_computeX25519Key(
 *   const SEC_PKEY_S *pPvtKey,
 *   const SEC_UCHAR *pubKey,
 *   const SEC_UINT32 pubLen,
 *   SEC_UCHAR *pucKey
 *   const SEC_UINT32 pucLen);
 * @endcode
 *
 * @par Purpose
 * This function is used to generate a X25519 shared secret key.
 * @par Description
 * CRYPT_computeX25519Key function generates a X25519 shared
 * secret key from private key and peer public key. The memory for
 * the shared secret must have been allocated prior to calling.
 *
 * @param[in] pPvtKey Own private key sturct pointer [N/A]
 * @param[in] pubKey Peer public key pointer [N/A]
 * @param[in] pubLen Peer public key pointer max buffer size [N/A]
 * @param[out] pucKey Shared secret key pointer [N/A]
 * @param[in] pucLen Shared secret key pointer max buffer size [N/A]
 *
 * @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 Invalid Key [SEC_CRYPT_ERR_INVALID_PKEY|N/A]
 * @retval SEC_UINT32 Invalid Arguments [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 If pPvtKey dosen't have correct keyType or
 * pPrvKey pointer is NULL. [SEC_CRYPT_ERR_INVALID_PKEY_TYPE|N/A]
 * @retval SEC_UINT32 Incorrect Algorithm ID i.e. Key type is not ALGID_X25519 [SEC_ERR_INVALID_ALGID|N/A]
 * @retval SEC_UINT32 X25519 failed. [SEC_ERR_KEY_GEN_FAILED|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 *   - CRYPT_computeX25519Key will only output standard diffie helman X25519 shared key.
 *   - Make sure the input parameter SEC_PKEY_S *pPvtKey contains the private key.
 *   - Make sure the pubKey pointer buffer max size is not less than IPSI_X25519_KEYLEN (pubLen >= 32).
 *   - Make sure the memory for the output parameter SEC_UCHAR *pucKey have been allocated and it's
 *     max buffer size isn't less than IPSI_X25519_KEYLEN (pucLen >= 32).
 *   - puckey Shared secret key length is CRYPT_X25519_SHARED_KEYLEN which is a fixed value.
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_computeX25519Key(const SEC_PKEY_S *pPvtKey, const SEC_UCHAR *pubKey, const SEC_UINT32 pubLen, SEC_UCHAR *pucKey, const SEC_UINT32 pucLen);

/* The RSA public key */
/**
 * @defgroup RSA_PUB_KEY_S
 * @ingroup sec_cryptoStructures
 * @par Prototype
 * @code
 * typedef struct stRSA_PUB_KEY
 * {
 *  SEC_BIGINT_S stN;
 *  SEC_BIGINT_S stE;
 * }RSA_PUB_KEY_S
 * @endcode
 *
 * @datastruct stN Modulus n.
 * @datastruct stE Public exponent e.
 */
typedef struct stRSA_PUB_KEY {
    SEC_BIGINT_S stN;
    SEC_BIGINT_S stE;
} RSA_PUB_KEY_S;

/* The RSA private key */
/**
 * @defgroup RSA_PRV_KEY_S
 * @ingroup sec_cryptoStructures
 * @par Prototype
 * @code
 * typedef struct stRSA_PRV_KEY
 * {
 *  SEC_BIGINT_S stN;
 *  SEC_BIGINT_S stE;
 *  SEC_BIGINT_S stD;
 *  SEC_BIGINT_S stP;
 *  SEC_BIGINT_S stQ;
 *  SEC_BIGINT_S stDP;
 *  SEC_BIGINT_S stDQ;
 *  SEC_BIGINT_S stQInv;
 * }RSA_PRV_KEY_S;
 * @endcode
 *
 * @datastruct stN Modulus.
 * @datastruct stE Public exponent e.
 * @datastruct stD Private exponent.
 * @datastruct stP Prime factor p.
 * @datastruct stQ Prime factor q.
 * @datastruct stDP Exponent dP for CRT.
 * @datastruct stDQ Exponent dQ for CRT.
 * @datastruct stQInv CRT coefficient qInv.
 */
typedef struct stRSA_PRV_KEY {
    SEC_BIGINT_S stN; /* modulus n */
    SEC_BIGINT_S stE; /* public exponent e */
    SEC_BIGINT_S stD;    /* private exponent */
    SEC_BIGINT_S stP;    /* prime factor p */
    SEC_BIGINT_S stQ;    /* prime factor q */
    SEC_BIGINT_S stDP;   /* exponent dP for CRT */
    SEC_BIGINT_S stDQ;   /* exponent dQ for CRT */
    SEC_BIGINT_S stQInv; /* CRT co-efficient qInv */
} RSA_PRV_KEY_S;

/* The RSA key - can have one or both keys of a pair */
/**
 * @defgroup RSA_KEY_S
 * @ingroup sec_cryptoStructures
 * @par Prototype
 * @code
 * typedef struct stRSA_KEY
 * {
 *  SEC_UINT32 keyType;
 *  SEC_UINT32 bits;
 *  union
 *  {
 *   RSA_PUB_KEY_S* pPubKey;
 *   RSA_PRV_KEY_S* pPrvKey;
 *  }key;
 * }RSA_KEY_S;
 * @endcode
 *
 * @datastruct keyType Public or private or pair.
 * @datastruct bits Length in bits of modulus.
 * @datastruct key Union enclosing pointer to the key structure.
 * @datastruct pPubKey Pointer to the RSA public key structure.
 * @datastruct pPrvKey Pointer to the RSA private key structure.
 */
typedef struct stRSA_KEY {
    SEC_UINT32 keyType; /* public or private or pair */
    SEC_UINT32 bits;    /* length in bits of modulus */
    union {
        RSA_PUB_KEY_S *pPubKey;
        RSA_PRV_KEY_S *pPrvKey;
    } key;
} RSA_KEY_S;

/*
                Structures  & definitions needed for SSL
*/
/* All the integers expressed as octet strings. */
#define MAX_RSA_DATA_LEN 1024
#define MAX_RSA_MODULUS_LEN 512
#define MAX_RSA_MODULUS_BITS (MAX_RSA_MODULUS_LEN * 8)
#define MAX_RSA_PRIME_BITS ((MAX_RSA_MODULUS_BITS + 1) / 2)
#define MAX_RSA_PRIME_LEN ((MAX_RSA_PRIME_BITS + 7) / 8)

/**
 * @defgroup RSA_OctKey
 * @ingroup sec_cryptoStructures
 * @par Prototype
 * @code
 * typedef struct
 * {
 *  size_t bits
 *  unsigned char mod[MAX_RSA_MODULUS_LEN]
 *  unsigned char exp[MAX_RSA_MODULUS_LEN]
 * }RSA_OctKey
 * @endcode
 *
 * @datastruct bits Length in bits of modulus.
 * @datastruct mod[MAX_RSA_MODULUS_LEN] Modulus n.
 * @datastruct exp[MAX_RSA_MODULUS_LEN] Public/private exponent e/d.
 */
typedef struct {
    size_t bits; /* length in bits of modulus */
    unsigned char mod[MAX_RSA_MODULUS_LEN];

    /* modulus n */
    unsigned char exp[MAX_RSA_MODULUS_LEN];

    /* public/private exponent e/d */
} RSA_OctKey;

/**
 * @defgroup RSA_OctPrvKeyCRT
 * @ingroup sec_cryptoStructures
 * @par Prototype
 * @code
 * typedef struct
 * {
 *  size_t bits
 *  unsigned char p[MAX_RSA_PRIME_LEN]
 *  unsigned char q[MAX_RSA_PRIME_LEN]
 *  unsigned char dP[MAX_RSA_PRIME_LEN]
 *  unsigned char dQ[MAX_RSA_PRIME_LEN]
 *  unsigned char qInv[MAX_RSA_PRIME_LEN]
 * }RSA_OctPrvKeyCRT
 * @endcode
 *
 * @datastruct bits Length in bits of modulus.
 * @datastruct p[MAX_RSA_PRIME_LEN] Prime factor p.
 * @datastruct q[MAX_RSA_PRIME_LEN] Prime factor q.
 * @datastruct dP[MAX_RSA_PRIME_LEN] Exponent dP for CRT.
 * @datastruct dQ[MAX_RSA_PRIME_LEN] Exponent dQ for CRT.
 * @datastruct qInv[MAX_RSA_PRIME_LEN] CRT co-efficient qInv.
 */
typedef struct {
    size_t bits;                           /* length in bits of modulus */
    unsigned char p[MAX_RSA_PRIME_LEN];    /* prime factor p */
    unsigned char q[MAX_RSA_PRIME_LEN];    /* prime factor q */
    unsigned char dP[MAX_RSA_PRIME_LEN];   /* exponent dP for CRT */
    unsigned char dQ[MAX_RSA_PRIME_LEN];   /* exponent dQ for CRT */
    unsigned char qInv[MAX_RSA_PRIME_LEN]; /* CRT coefficient qInv */
} RSA_OctPrvKeyCRT;

typedef RSA_OctKey RSA_PubKey;
typedef RSA_OctPrvKeyCRT RSA_PrvKey;

/*
    Func Name:  rsa_cvtPKey
*/
/**
 * @defgroup rsa_cvtPKey
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT rsa_cvtPKey (const RSA_KEY_S* pRSAKey,
 *    void* pKey, SEC_UINT32 ulKeyType);
 * @endcode
 *
 * @par Purpose
 * This is used to covert the RSA key from another key.
 *
 * @par Description
 * rsa_cvtPKey converts the RSA key (RSA_KEY_S *) from pkey depending on the key type.
 *
 * @param[in] pRSAKey RSA key to be converted [N/A]
 * @param[in] pKey Key to be used for conversion[N/A]
 * @param[in] ulKeyType Key type, whether CRYPT_PKEY_PUBLIC or CRYPT_PKEY_PRIVATE [N/A]
 *
 * @retval SEC_UINT32 On success [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 Indicates invalid key type [SEC_CRYPT_ERR_INVALID_PKEY|N/A]
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
CRYPTOLINKDLL SEC_UINT rsa_cvtPKey(const RSA_KEY_S *pRSAKey, void *pKey, SEC_UINT32 ulKeyType);

/*
    Func Name:  rsa_padPKCSType1
*/
/**
 * @defgroup rsa_padPKCSType1
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT rsa_padPKCSType1 (
 *    SEC_UCHAR aucCBuf[],
 *    SEC_UINT blkSz,
 *    const SEC_UCHAR* pucBuf,
 *    SEC_UINT blkLen);
 * @endcode
 *
 * @par Purpose
 * This is used to pad the RSA encoded buffer with PKCS Type1.
 *
 * @par Description
 * rsa_padPKCSType1 used to pad the RSA encoded buffer with PKCS Type1 and provide the output .
 *
 * @param[out] aucCBuf Output buffer to be updated with padded data [N/A]
 * @param[in] blkSz RSA key block size [N/A]
 * @param[in] pucBuf Buffer to be padded[N/A]
 * @param[in] blkLen Length of the buffer to be padded [N/A]
 *
 * @retval SEC_UINT32 On success [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 Indicates invalid key length [SEC_ERR_INVALID_KEY_LEN|N/A]
 * @retval SEC_UINT32 Indicates data size is more than key size [SEC_CRYPT_ERR_LARGE_DATA_SIZE|N/A]
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
CRYPTOLINKDLL SEC_UINT rsa_padPKCSType1(SEC_UCHAR aucCBuf[], SEC_UINT blkSz, const SEC_UCHAR *pucBuf, SEC_UINT blkLen);

/*
    Func Name:  rsa_unPadPKCSType2
*/
/**
 * @defgroup rsa_unPadPKCSType2
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT rsa_unPadPKCSType2 (
 *    SEC_UCHAR aucCBuf[],
 *    SEC_UINT blkSz,
 *    SEC_UCHAR* pucBuf,
 *    SEC_UINT* blkLen);
 * @endcode
 *
 * @par Purpose
 * This is used to remove the padding of RSA Type2 from the decoded buffer.
 *
 * @par Description
 * rsa_padPKCSType2 used to remove the padding of RSA Type2 from the decoded buffer.
 *
 * @param[out] aucCBuf Output buffer to be updated with data after removing padding [N/A]
 * @param[in] blkSz RSA key block size [N/A]
 * @param[in] pucBuf Buffer on which padding to be removed[N/A]
 * @param[in/out] blkLen Length of the buffer to be padded [N/A]
 *
 * @retval SEC_UINT32 On success [IPSI_TRUE|N/A]
 * @retval SEC_UINT32 On failure [IPSI_FALSE|N/A]
 * @retval SEC_UINT32 Invalid arguments [SEC_ERR|N/A]
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
CRYPTOLINKDLL SEC_UINT rsa_unPadPKCSType2(SEC_UCHAR aucCBuf[], SEC_UINT blkSz, SEC_UCHAR *pucBuf, SEC_UINT *blkLen);

// PKEY encode decode functions.
/*
    Func Name:  CRYPT_RSAPubKeyDecode
*/
/**
 * @defgroup CRYPT_RSAPubKeyDecode
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_RSAPubKeyDecode(
 *   SEC_PKEY_S *pOutKey,
 *   SEC_CHAR *buf)
 * @endcode
 *
 * @par Purpose
 * This is used to decode the passed buffer and returns the RSA key structure.
 *
 * @par Description
 * CRYPT_RSAPubKeyDecode decodes buffer into RSA public key structure.
 *
 * @param[in] buf The buffer which is to be decoded [N/A]
 * @param[out] pOutKey Key to be updated[N/A]
 *
 * @retval SEC_UINT32 On success [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 Indicates invalid key [SEC_CRYPT_ERR_INVALID_PKEY|N/A]
 * @retval SEC_UINT32 Indicates failure in memory allocation [SEC_ERR_MALLOC_FAIL|N/A]
 * @retval SEC_UINT32 memory copy failure [SEC_ERR_MEMCPY_FAILED|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * - The safe API corresponding to this is CRYPT_RSAPubKeyDecode_ex
 *
 *
 * @par Related Topics
 * CRYPT_RSAPubKeyEncode
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_RSAPubKeyDecode(SEC_PKEY_S *pOutKey, SEC_CHAR *buf);

/*
    Func Name:  CRYPT_RSAPubKeyDecode_ex
*/
/**
 * @defgroup CRYPT_RSAPubKeyDecode_ex
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_RSAPubKeyDecode_ex(
 *   SEC_PKEY_S *pstOutKey,
 *   SEC_CHAR *buf, SEC_UINT uiBuffLen)
 * @endcode
 *
 * @par Purpose
 * This is used to decode the passed buffer and returns the RSA key structure.
 *
 * @par Description
 * CRYPT_RSAPubKeyDecode decodes buffer into RSA public key structure.
 *
 * @param[in] buf The buffer which is to be decoded [N/A]
 * @param[in] uiBuffLen The length of the input buffer [N/A]
 * @param[out] pstOutKey Key to be updated[N/A]
 *
 * @retval SEC_UINT32 On success [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 Indicates invalid key [SEC_CRYPT_ERR_INVALID_PKEY|N/A]
 * @retval SEC_UINT32 Indicates failure in memory allocation [SEC_ERR_MALLOC_FAIL|N/A]
 * @retval SEC_UINT32 Indicates invalid parameters [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 Memory copy failure [SEC_ERR_MEMCPY_FAILED|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * The uiBuffLen should be more than 16 bytes.
 *
 * @par Related Topics
 * CRYPT_RSAPubKeyEncode
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_RSAPubKeyDecode_ex(SEC_PKEY_S *pstOutKey, SEC_CHAR *buf, SEC_UINT uiBuffLen);

/*
    Func Name:  CRYPT_RSAPubKeyEncode
*/
/**
 * @defgroup CRYPT_RSAPubKeyEncode
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_RSAPubKeyEncode(
 * const SEC_PKEY_S *pInKey,
 * SEC_UINT *uiSize,SEC_UCHAR **pcEncodedBuff)
 * @endcode
 *
 * @par Purpose
 * This is used to encode the passed RSA key structure and returns
 * the encoded buffer.
 *
 * @par Description
 * CRYPT_RSAPubKeyEncode encodes RSA public key structure.
 *
 * @param[in]  pInKey Key to be encoded [N/A]
 * @param[out] uiSize Size of encoded buffer [N/A]
 * @param[out] pcEncodedBuff The encoded buffer [N/A]
 *
 * @retval SEC_UINT32 On success [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 If pInKey contains a non RSA key [SEC_ERR_NON_RSA_KEY|N/A]
 * @retval SEC_UINT32 If pInKey does not contain any key [SEC_ERR_NO_KEY|N/A]
 * @retval SEC_UINT32 Indictes failure of memory allocation [SEC_ERR_MALLOC_FAIL|N/A]
 * @retval SEC_UINT32 If pInKey parameters are wrong [SEC_CRYPT_ERR_INVALID_PKEY|N/A]
 * @retval SEC_UINT32 If mem copy fails [SEC_ERR_MEMCPY_FAILED|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * CRYPT_RSAPubKeyDecode
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_RSAPubKeyEncode(const SEC_PKEY_S *pInKey, SEC_UINT *uiSize, SEC_UCHAR **pcEncodedBuff);

/**
 * @defgroup DH_KEY_S
 * @ingroup sec_cryptoStructures
 * @par Prototype
 * @code
 * typedef struct stDH_KEY
 * {
 *  SEC_UINT32 keyType
 *  DH_PARA_S* pPara
 *  SEC_BIGINT_S stPubKey
 *  SEC_BIGINT_S stPrvKey
 * }DH_KEY_S
 * @endcode
 *
 * @datastruct keyType Public or private or pair.
 * @datastruct pPara Optional parameters.
 * @datastruct stPubKey Public key structure.
 * @datastruct stPrvKey Private key structure.
 */
typedef struct stDH_KEY {
    SEC_UINT32 keyType;    /* public or private or pair */
    DH_PARA_S *pPara;      /* optional parameters */
    SEC_BIGINT_S stPubKey; /* public key structure */
    SEC_BIGINT_S stPrvKey; /* private key structure */
} DH_KEY_S;

/* DSA key */
/**
 * @defgroup DSA_KEY_S
 * @ingroup sec_cryptoStructures
 * @par Prototype
 * @code
 * typedef struct stDSA_KEY
 * {
 *  SEC_UINT32 keyType
 *  DSA_PARA_S* pPara
 *  SEC_BIGINT_S stPubKey
 *  SEC_BIGINT_S stPrvKey
 * }DSA_KEY_S
 * @endcode
 *
 * @datastruct keyType Public or private or pair.
 * @datastruct pPara Optional parameters.
 * @datastruct stPubKey Public key structure.
 * @datastruct stPrvKey Private key structure.
 */
typedef struct stDSA_KEY {
    SEC_UINT32 keyType;    /* public or private or pair */
    DSA_PARA_S *pPara;     /* optional parameters */
    SEC_BIGINT_S stPubKey; /* public key structure */
    SEC_BIGINT_S stPrvKey; /* private key structure */
} DSA_KEY_S;

/* Generic EC public key */
typedef struct stEC_POINT EC_PUB_KEY_S;

/* Generic EC private key */
typedef struct stBIGINT EC_PRV_KEY_S;

/* Generic EC key */
/**
 * @defgroup EC_KEY_S
 * @ingroup sec_cryptoStructures
 * @par Prototype
 * @code
 * typedef struct stECDSA_KEY
 * {
 *  SEC_UINT32 keyType
 *  EC_PARA_S* pPara
 *  EC_PUB_KEY_S* pPubKey
 *  EC_PRV_KEY_S* pPrvKey
 * }EC_KEY_S
 * @endcode
 *
 * @datastruct keyType Public or private or pair.
 * @datastruct pPara Optional parameters.
 * @datastruct pPubKey Pointer to the ECDSA public key structure.
 * @datastruct pPrvKey Pointer to the ECDSA private key structure.
 *
 */
typedef struct stECDSA_KEY {
    SEC_UINT32 keyType;    /* public or private or pair */
    EC_PARA_S *pPara;      /* EC params structure */
    EC_PUB_KEY_S *pPubKey; /* pointer to the ECDSA public key structure */
    EC_PRV_KEY_S *pPrvKey; /* pointer to the ECDSA private key structure */
} EC_KEY_S;

/* ECDSA key */
typedef EC_PUB_KEY_S ECDSA_PUB_KEY_S;
typedef EC_PRV_KEY_S ECDSA_PRV_KEY_S;
typedef EC_KEY_S ECDSA_KEY_S;

/* ECDH key */
typedef EC_PUB_KEY_S ECDH_PUB_KEY_S;
typedef EC_PRV_KEY_S ECDH_PRV_KEY_S;
typedef EC_KEY_S ECDH_KEY_S;

typedef struct stED_PKEY_S {
    SEC_UINT32 uiLen;
    SEC_UCHAR *pucBuff;
} ED_PKEY_S;

typedef struct stEDDSA_KEY_S {
    SEC_UINT32 keyType;
    ED_PKEY_S stPubKey;
    ED_PKEY_S stPvtKey;
} EDDSA_KEY_S;

#define IPSI_ED25519_KEYLEN 32

/**
 * @defgroup IPSI_AEAD_OP_DATA_S
 * @ingroup sec_cryptoStructures
 * @par Prototype
 * @code
 * typedef struct stAeadOpData
 * {
 *     CRYPT_AEAD_CTX ctx
 *     IPSI_SYM_DATATYPE_E enDataType
 *     const SEC_UCHAR *pucIV
 *     size_t uiIvLen
 *     SEC_UCHAR *pucAad
 *     size_t uiAadLen
 *     SEC_UCHAR *pCipherOpData
 * }IPSI_AEAD_OP_DATA_S
 * @endcode
 *
 * @datastruct ctx Pointer to the AEAD session context.
 * @datastruct enDataType Identifies the data type whether FULL or PARTIAL.
 * @datastruct pucIV Pointer to the Initialization Vector used in AEAD operations.
 * @datastruct uiIvLen Initialization Vector length.
 * @datastruct pucAad Pointer to Associated Authentication Data used in AEAD operations.
 * @datastruct uiAadLen AAD length.
 * @datastruct pCipherOpData No use for now. Added for future extensability.
 *
 */
typedef struct stAeadOpData {
    CRYPT_AEAD_CTX ctx;
    IPSI_SYM_DATATYPE_E enDataType;
    const SEC_UCHAR *pucIV;
    size_t uiIvLen;
    SEC_UCHAR *pucAad;
    size_t uiAadLen;
    SEC_VOID *pCipherOpData;
} IPSI_AEAD_OP_DATA_S;

/* Supported authentication tag lengths for AES GCM */
/**
 * @defgroup IPSI_AEAD_TAG_LEN_E
 * @ingroup sec_cryptoEnum
 * @par Prototype
 * @code
 * typedef enum
 * {
 *     IPSI_AEAD_TAG_LEN_32 = 4,
 *     IPSI_AEAD_TAG_LEN_64 = 8,
 *     IPSI_AEAD_TAG_LEN_96 = 12,
 *     IPSI_AEAD_TAG_LEN_104 = 13,
 *     IPSI_AEAD_TAG_LEN_112 = 14,
 *     IPSI_AEAD_TAG_LEN_120 = 15,
 *     IPSI_AEAD_TAG_LEN_128 = 16,
 * } IPSI_AEAD_TAG_LEN_E;
 * @endcode
 *
 * @dataenum IPSI_AEAD_TAG_LEN_32 Identifies tag length of 4 bytes.
 * @dataenum IPSI_AEAD_TAG_LEN_64 Identifies tag length of 8 bytes.
 * @dataenum IPSI_AEAD_TAG_LEN_96 Identifies tag length of 12 bytes.
 * @dataenum IPSI_AEAD_TAG_LEN_104 Identifies tag length of 13 bytes.
 * @dataenum IPSI_AEAD_TAG_LEN_112 Identifies tag length of 14 bytes.
 * @dataenum IPSI_AEAD_TAG_LEN_120 Identifies tag length of 15 bytes.
 * @dataenum IPSI_AEAD_TAG_LEN_128 Identifies tag length of 16 bytes.
 */
typedef enum {
    IPSI_AEAD_TAG_LEN_32 = 4,
    IPSI_AEAD_TAG_LEN_64 = 8,
    IPSI_AEAD_TAG_LEN_96 = 12,
    IPSI_AEAD_TAG_LEN_104 = 13,
    IPSI_AEAD_TAG_LEN_112 = 14,
    IPSI_AEAD_TAG_LEN_120 = 15,
    IPSI_AEAD_TAG_LEN_128 = 16
} IPSI_AEAD_TAG_LEN_E;

/* Supported authentication tag lengths for AES CCM */
/**
 * @defgroup IPSI_AEAD_CCM_TAG_LEN_E
 * @ingroup sec_cryptoEnum
 * @par Prototype
 * @code
 * typedef enum
 * {
 *     IPSI_AEAD_CCM_TAG_LEN_32 = 4,
 *     IPSI_AEAD_CCM_TAG_LEN_48 = 6,
 *     IPSI_AEAD_CCM_TAG_LEN_64 = 8,
 *     IPSI_AEAD_CCM_TAG_LEN_80 = 10,
 *     IPSI_AEAD_CCM_TAG_LEN_96 = 12,
 *     IPSI_AEAD_CCM_TAG_LEN_112 = 14,
 *     IPSI_AEAD_CCM_TAG_LEN_128 : 16
 * } IPSI_AEAD_TAG_LEN_E;
 * @endcode
 *
 * @dataenum IPSI_AEAD_CCM_TAG_LEN_32 Identifies tag length of 4 bytes.
 * @dataenum IPSI_AEAD_CCM_TAG_LEN_48 Identifies tag length of 6 bytes.
 * @dataenum IPSI_AEAD_CCM_TAG_LEN_64 Identifies tag length of 8 bytes.
 * @dataenum IPSI_AEAD_CCM_TAG_LEN_80 Identifies tag length of 10 bytes.
 * @dataenum IPSI_AEAD_CCM_TAG_LEN_96 Identifies tag length of 12 bytes.
 * @dataenum IPSI_AEAD_CCM_TAG_LEN_112 Identifies tag length of 14 bytes.
 * @dataenum IPSI_AEAD_CCM_TAG_LEN_128 Identifies tag length of 16 bytes.
 */
typedef enum {
    IPSI_AEAD_CCM_TAG_LEN_32 = 4,
    IPSI_AEAD_CCM_TAG_LEN_48 = 6,
    IPSI_AEAD_CCM_TAG_LEN_64 = 8,
    IPSI_AEAD_CCM_TAG_LEN_80 = 10,
    IPSI_AEAD_CCM_TAG_LEN_96 = 12,
    IPSI_AEAD_CCM_TAG_LEN_112 = 14,
    IPSI_AEAD_CCM_TAG_LEN_128 = 16
} IPSI_AEAD_CCM_TAG_LEN_E;

/*
    Func Name:  IPSI_CRYPT_dhConstTimeOn
*/
/**
* @defgroup IPSI_CRYPT_dhConstTimeOn
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_VOID IPSI_CRYPT_dhConstTimeOn()
* @endcode
*
* @par Purpose
* This is used to switch on the DH constant time operations.
* No need to call this function if user does not disable this
* feature using API IPSI_CRYPT_dhConstTimeOff.
*
* @par Description
* IPSI_CRYPT_dhConstTimeOn switches on the DH constant time operations.
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
* - No need to call this function if user does not disable this feature using API
*     IPSI_CRYPT_dhConstTimeOff.
* - It is global function, application need to make sure that the function will not be called in parallel
*    with IPSI_CRYPT_dhConstTimeOff, CRYPT_computeDHKey and CRYPT_genKey for DH keys
*
* @par Related Topics
* IPSI_CRYPT_dhConstTimeOff
*/
CRYPTOLINKDLL SEC_VOID IPSI_CRYPT_dhConstTimeOn(void);

/*
    Func Name:  IPSI_CRYPT_dhConstTimeOff
*/
/**
* @defgroup IPSI_CRYPT_dhConstTimeOff
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_VOID IPSI_CRYPT_dhConstTimeOff()
* @endcode
*
* @par Purpose
* This is used to switch off the DH constant time operations. By default DH constant time operation is enabled.
*
* @par Description
* IPSI_CRYPT_dhConstTimeOff switches off the DH constant time operations.
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
* - It is global function, application need to make sure that the function will not be called in parallel
*    with IPSI_CRYPT_dhConstTimeOn, CRYPT_computeDHKey and CRYPT_genKey for DH keys
*
* @par Related Topics
* IPSI_CRYPT_dhConstTimeOn
*/
CRYPTOLINKDLL SEC_VOID IPSI_CRYPT_dhConstTimeOff(void);

/**
 * @defgroup IPSI_ECP_CB
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * typedef SEC_INT (*IPSI_ECP_CB) (SEC_VOID*);
 *
 * @endcode
 *
 * @par Purpose
 * Callback function called from EC prime operations.
 *
 * @par Description
 * This function is called from EC prime operations like EC signing, computing ECDH key, while doing SM2 operations.
 * In all the cases the curve of the key should be of Prime type
 * The first parameter to callback function is
 * reserved for future use. Currently the return value from the callback function
 * is ignored and intended only for future use.
 *
 *
 * @retval SEC_INT32 Currently Nothing to be returned [N/A|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * N/A
 *
 * @par Related Topics
 * IPSI_CRYPT_setEcpCallback
 */
typedef SEC_INT (*IPSI_ECP_CB)(SEC_VOID *);

/*
    Func Name:  IPSI_CRYPT_setEcpCallback
*/
/**
* @defgroup IPSI_CRYPT_setEcpCallback
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 IPSI_CRYPT_setEcpCallback (IPSI_ECP_CB pCB, SEC_UINT32 lTimeOut,
*                                                     IPSI_AUX_PAR_S *pstAux);
* @endcode
*
* @par Purpose
* This function is used to set the callback function for EC prime multiplication and its timeout value.
*
* @par Description
* IPSI_CRYPT_setEcpCallback will be called at the time of EC prime multiplication (if the timeout is occurred).
* For binary curves this feature is not supported.
*
* @param[in] pCB Callback function which will be called at when the timer expires [N/A]
* @param[in] lTimeOut Timeout value in ticks. If the timeout value is more than LONG_MAX then API will return
* error SEC_ERR_INVALID_ARG. It is recommended to provide the timeout value little less than
* the expected timeout. ex: If the timeout is needed after 100 ticks then set the timeout value less than 60 ticks.
* It should be checked by the application that what timeout value suites their system.[N/A]
* @param[in] pstAux  Auxiliary parameter, reserved for future use. Currently NULL can be passed to this parameter. [N/A]
*
* @retval SEC_UINT32 On success [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 lTimeOut is greater than LONG_MAX [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 lTimeOut is zero and  pCB is not NULL [SEC_ERR_INVALID_ARG|N/A]

* @par Required Header File
* sec_crypto.h
*
* @par Note
* - It is a global function, application need to make sure that the function will not be called in parallel
*    with the EC operations over the prime curves.
* - To remove the timeout callback, application need to pass the pCB as NULL.
* - NULL function pointer with some timeout value is a valid input to this API. It will remove the callback
*    function call.
*
* @par Related Topics
*
*/
CRYPTOLINKDLL SEC_UINT32 IPSI_CRYPT_setEcpCallback(IPSI_ECP_CB pCB, SEC_UINT32 lTimeOut, IPSI_AUX_PAR_S *pstAux);

/**
 * @defgroup IPSI_BN_CB
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * typedef SEC_INT (*IPSI_BN_CB) (SEC_VOID*);
 *
 * @endcode
 *
 * @par Purpose
 * Callback function called from BN operations.
 *
 * @par Description
 * This function is called from BN operations like while computing the DH keys,generating the DH keys.
 * The first parameter to callback function is
 * reserved for future use. Currently the return value from the callback function
 * is ignored and intended only for future use.
 *
 *
 * @retval SEC_INT32 Currently Nothing to be returned [N/A|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * N/A
 *
 * @par Related Topics
 * IPSI_CRYPT_setBnCallback
 */
typedef SEC_INT (*IPSI_BN_CB)(SEC_VOID *);

/*
Func Name:  IPSI_CRYPT_setBnCallback
*/
/**
* @defgroup IPSI_CRYPT_setBnCallback
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 IPSI_CRYPT_setBnCallback (IPSI_BN_CB pCB, SEC_UINT32 lTimeOut, IPSI_AUX_PAR_S *pstAux);
* @endcode
*
* @par Purpose
* This function is used to set the callback function for BN operations and its timeout value.
*
* @par Description
* IPSI_CRYPT_setBnCallback will be called at the time of BN operations (if the timeout is occurred)
*
* @param[in] pCB Callback function which will be called at when the timer expires [N/A]
* @param[in] lTimeOut Timeout value in ticks. If the timeout value is more than LONG_MAX then API will return
* error SEC_ERR_INVALID_ARG. It is recommended to provide the timeout value little less than
* the expected timeout. ex: If the timeout is needed after 100 ticks then set the timeout value less than 60 ticks.
* It should be checked by the application that what timeout value suites their system.[N/A]
* @param[in] pstAux  Auxiliary parameter, reserved for future use. Currently NULL can be passed to this parameter. [N/A]
*
* @retval SEC_UINT32 On success [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 lTimeOut is greater than LONG_MAX [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 lTimeOut is zero and  pCB is not NULL [SEC_ERR_INVALID_ARG|N/A]

* @par Required Header File
* sec_crypto.h
*
* @par Note
* - It is a global function, application need to make sure that the function will not be called in parallel
      and also ensure that no asymetric key (RSA, DSA, DH, SM2, ECDH and ECDSA) operation will be running in any thread.
* - To remove the timeout callback, application need to pass the pCB as NULL.
* - NULL function pointer with some timeout value is a valid input to this API. It will remove the callback
*    function call.
*
* @par Related Topics
* N/A
*
*/
CRYPTOLINKDLL SEC_UINT32 IPSI_CRYPT_setBnCallback(IPSI_BN_CB pCB, SEC_UINT32 lTimeOut, IPSI_AUX_PAR_S *pstAux);

/*
    Func Name:  CRYPT_aeadInitSession
*/
/**
* @defgroup CRYPT_aeadInitSession
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_aeadInitSession(CRYPT_AEAD_CTX *pCtx, const IPSI_AEAD_SETUP_DATA_S *pstSetUpInfo,
*                                                  IPSI_AUX_PAR_S *pstAuxPar)
* @endcode
*
* @par Purpose
* This is used to initialize the AEAD session context structure.
*
* @par Description
* CRYPT_aeadInitSession used to initialize the AEAD session context structure with the algorithm, key, key length
* and cipher direction (encrypt or decrypt). Supported ciphers are ALGID_AES128_GCM, ALGID_AES128_CCM,
* ALGID_AES192_GCM, ALGID_AES192_CCM, ALGID_AES256_GCM & ALGID_AES256_CCM.
*
* @param[in]  pCtx Pointer to AEAD session context [N/A]
* @param[in]  pstSetUpInfo Pointer to AEAD initialization information [N/A]
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
*    - Invalid direction (other than IPSI_SYM_CIPHER_ENCRYPT and IPSI_SYM_CIPHER_DECRYPT for AEAD direction
*       in setup information)
* - Padding is not applicable to AES GCM & AES CCM algorithms as there is no restriction on block size for
* AES GCM & AES CCM algorithm.
* Even if padding mode is set to algorithm id, it will be ignored.
* - SEC_ERR_INVALID_ALGID will be returned if:
*    - If algorithm ID passed is other than ALGID_AES128_GCM, ALGID_AES128_CCM, ALGID_AES192_GCM,
*      ALGID_AES192_CCM, ALGID_AES256_GCM & ALGID_AES256_CCM.
* - SEC_ERR_INVALID_KEY_LEN will be returned if key length passed is not matching with expected key length.
* For ALGID_AES128_GCM & ALGID_AES128_CCM the key length should be 16 bytes, for ALGID_AES192_GCM & ALGID_AES192_CCM
* the key length should be 24 bytes, for ALGID_AES256_GCM & ALGID_AES256_GCM the key length should be 32 bytes.
* - CRYPT_aeadRemoveSession should be called to free context created by CRYPT_aeadInitSession
* @par Related Topics
* CRYPT_aeadOp \n
* CRYPT_aeadRemoveSession
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_aeadInitSession(CRYPT_AEAD_CTX *pCtx, const IPSI_AEAD_SETUP_DATA_S *pstSetUpInfo, IPSI_AUX_PAR_S *pstAuxPar);

/* Encryption / decryption operation using AEAD algorithm. */
/**
 * Func Name:  CRYPT_aeadOp
 * @defgroup CRYPT_aeadOp
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_aeadOp(
 * const IPSI_AEAD_OP_DATA_S *pstOpData,
 * const SEC_UCHAR *pucInData,
 * size_t ulInDataLen,
 * SEC_UCHAR *pucOutData,
 * size_t *pulOutDataLen,
 * size_t ulOutDataLen,
 * SEC_UCHAR *pucHashResult,
 * SEC_UINT32 ulHashResultLen,
 * IPSI_AUX_PAR_S *pstAuxPar);
 * @endcode
 *
 * @par Purpose
 * Used to do encryption / decryption operation on AEAD algorithms.
 *
 * @par Description
 * CRYPT_aeadOp used to do encryption / decryption operation. In case of encryption,
 * both encrypted data and authentication tag will be generated by this function. In case
 * of decryption, it takes encrypted data, authentication tag as input and outputs decrypted
 * text only when both decryption and tag verification are success.
 *
 * @param[in]  pstOpData Pointer to AEAD operation data which holds information
 *                                 required for encryption / decryption [N/A]
 * @param[in]  pucInData Pointer to input data for encryption / decryption operation [N/A]
 * @param[in]  ulInDataLen Input data length [N/A]
 * @param[out]  pucOutData Pointer to output data. The buffer size should be atleast equal to ulInDataLen [N/A]
 * @param[out]  pulOutDataLen Pointer to output data length [N/A]
 * @param[in]  ulOutDataLen The actual size of pucOutData [N/A]
 * @param[in]  pucHashResult Pointer to hold authentication tag in case of encryption and input
 *                                      authentication tag in case of decryption[N/A]
 * @param[in]  ulHashResultLen Desired authentication tag length [N/A]
 * @param[in]  pstAuxPar Pass as SEC_NULL. Additional variable for future extensibility [N/A]
 *
 * @retval SEC_UINT32 Invalid argument [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 Invalid IV pointer / Invalid IV length [SEC_ERR_INVALID_IV_LEN|N/A]
 * @retval SEC_UINT32 Invalid tag length [SEC_CRYPT_ERR_INVALID_TAG_LEN|N/A]
 * @retval SEC_UINT32 IV length exceeds maximum limit [SEC_CRYPT_ERR_IV_LEN_EXCEED_MAXLIMIT|N/A]
 * @retval SEC_UINT32 AAD length exceeds maximum limit [SEC_CRYPT_ERR_AAD_LEN_EXCEED_MAXLIMIT|N/A]
 * @retval SEC_UINT32 Trying to set AAD after encryption/decryption operation
 *                              [SEC_CRYPT_ERR_SET_AAD_AFTER_CIPHER_OPERATION|N/A]
 * @retval SEC_UINT32 Set IV failed [SEC_CRYPT_ERR_AEAD_SET_IV_FAILED|N/A]
 * @retval SEC_UINT32 Data for encryption/decryption exceeds maximum limit [SEC_CRYPT_ERR_LARGE_DATA_SIZE|N/A]
 * @retval SEC_UINT32 AEAD decryption operation failure / tag verification failure [SEC_ERR|N/A]
 * @retval SEC_UINT32 memset_s operation failed [SEC_ERR_MEMSET_FAILED|N/A]
 *
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 *
 * For CCM
 * - Supported lengths for authentication tag:
 * 4 bytes, 6 bytes, 8 bytes, 10 bytes, 12 bytes, 14 bytes, 16 bytes.The tag length required should be passed by user.
 * - The output buffer to hold cipher text/decrypted text should be allocated by user. As per AES-CCM standard,
 * cipher text will contain the encrypted data + tag.So for encryption the output buffer should contain
 * the memory atleast (input lengh + desired tag length).
 * - On encryption success, output data length will be input length + tag length.
 * - On decryption success, output data length will be input length - tag length when data length is more
 *    than tag length. In decryption, When cipher data is only tag (which means there is no data to be decrypted,
 *    but only tag needs to be verified), user still needs to pass non-NULL buffer for the output data.
 *    In case of only tag needs to be verified, the output length will not be updated.
 * - The type support for encryption/decryption is only IPSI_SYM_DATA_TYPE_FULL. If passed other types,
 * API will return error.
 *
 * For GCM
 * - Supported lengths for authentication tag : \n
 * 4 bytes, 8 bytes, 12 bytes, 13 bytes, 14 bytes, 15 bytes, 16 bytes. The tag length should be passed by
 * user and the buffer to hold output tag in case of encryption should be allocated by user.
 *
 * - The output buffer to hold cipher text / decrypted text should be allocated by user. The buffer should
 * contain memory of input length passed.
 * - On encryption / decryption success, output data length will be same as input data length.
 * - The data size need not be multiple of block size (block size of AES GCM is 16 bytes).
 * - If data is being encrypted/decrypted as partial data with IPSI_SYM_DATA_TYPE_PARTIAL type,
 * - IV should be set for the first time only. From the second time, even if IV are passed, they will be ignored.
 * - AAD can be set multiple times. But AAD should be set before sending data for encryption/decryption.
 * - If AAD is set after some encryption/decryption operation, API fails. In this failure case, context can not
 * be reused for further operations.
 * if  set AAD after doing partial encryption/decryption operation, error SEC_CRYPT_ERR_SET_AAD_AFTER_CIPHER_OPERATION
 * will be returned.
 * - The tag & tag length will be validated only if passed IPSI_SYM_DATA_TYPE_LAST_PARTIAL & IPSI_SYM_DATA_TYPE_FULL
 * type. These will be ignored when passed type as IPSI_SYM_DATA_TYPE_PARTIAL.
 * - As per NIST SP800-38D (AES-GCM) standard, decrypted output should be returned to user only when authentication(tag)
 * verification is successful. But when used IPSI_SYM_DATA_TYPE_PARTIAL type, API will return the output data.
 * So, application should check the return value of API when processed with IPSI_SYM_DATA_TYPE_LAST_PARTIAL type,
 * if it is success only, should consider the decrypted data & use it for application operations.
 * - The maximum lengths supported as per SP800-38D standard are :
 *    (a) For AAD, length of AAD is less than or equal to (2^64 - 1) in bits.
 *         In bytes, it should be less than or equal to 2^61.
 *    (b) For IV, length of IV is (1<= len(IV) <= (2^64 - 1)) in bits.
 *         In bytes, it should be less than or equal to 2^61.
 *    (c) For Data to be encrypted / decrypted, length should be <= 2^39-256 in bits.
 *         In bytes, it should be less than or equal to (2^36 - 32).
 *    (d) For AAD, IV & length to encrypted / decrypted, the data types are "size_t".
 *         So, on 32 bit systems, they support maximum length of size_t (2^32-1), on 64 bit systems,
 *         they support maximum length of size_t (2^64 - 1).
 * - When passed the input length to be encrypted / decrypted as zero, no output pointers will be updated.
 * - SEC_ERR_INVALID_ARG will be returned if:
 *     - Context is NULL.
 *     - If input data length is not zero and any of the pointer of input data, output data, output data length is NULL.
 *     - AAD pointer is NULL and AAD length is not zero.
 *     - Pointer to tag is NULL.
 *     - Invalid data type (other than IPSI_SYM_DATA_TYPE_FULL, IPSI_SYM_DATA_TYPE_PARTIAL,
 *       IPSI_SYM_DATA_TYPE_LAST_PARTIAL).
 *     - IV is NULL.
 *     - Pointer to IPSI_AEAD_OP_DATA_S is NULL.
 * - The maximum value of ulInDataLen can be 512MB.
 * - The safe API corresponding to this is CRYPT_aeadOp_sf
 *
 * @par Related Topics
 * CRYPT_aeadInitSession \n
 * CRYPT_aeadRemoveSession
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_aeadOp(const IPSI_AEAD_OP_DATA_S *pstOpData, const SEC_UCHAR *pucInData, size_t ulInDataLen, SEC_UCHAR *pucOutData, size_t *pulOutDataLen, size_t ulOutDataLen, SEC_UCHAR *pucHashResult, SEC_UINT32 ulHashResultLen,
                                      IPSI_AUX_PAR_S *pstAuxPar);

/*
    Func Name:  CRYPT_aeadRemoveSession
*/
/**
* @defgroup CRYPT_aeadRemoveSession
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_VOID CRYPT_aeadRemoveSession(CRYPT_AEAD_CTX ctx)
* @endcode
*
* @par Purpose
* This is used to free the AEAD session context structure.
*
* @par Description
* CRYPT_aeadRemoveSession used to free the AEAD session context structure.
*
* @param[in]  ctx Pointer to AEAD session context [N/A]
*
* @par Return values
* N/A

* @par Required Header File
* sec_crypto.h
*
* @par Note
*
* CRYPT_aeadRemoveSession should be only used to remove the context created with AEAD algorithm.
* Currently, supported AEAD algorithms are ALGID_AES128_GCM, ALGID_AES128_CCM, ALGID_AES192_GCM,
* ALGID_AES192_GCM, ALGID_AES256_GCM & ALGID_AES256_CCM.
* If called with context created with any other algorithm, behavior is undefined.
*
* @par Related Topics
* CRYPT_aeadInitSession \n
* CRYPT_aeadOp
*/
CRYPTOLINKDLL SEC_VOID CRYPT_aeadRemoveSession(CRYPT_AEAD_CTX ctx);

#ifndef __IPSI_NO_DRBG__

/* DRBG external flags */
/* Flag for CTR mode only: use derivation function ctr_df */
#define IPSI_CRYPT_DRBG_FLAG_CTR_USE_DF 0x1

/* PRNG is in test state */
#define IPSI_CRYPT_DRBG_FLAG_TEST 0x2

/**
 * @defgroup IPSI_RAND_SETUP_DATA_S
 * @ingroup sec_DRBG_Structures
 * @par Prototype
 * @code
 * typedef struct stRandDefaultDrbgCtxSetUpData
 * {
 *     SEC_INT type
 *     SEC_UINT flags
 *     SEC_UCHAR *pers
 *     size_t perslen
 *     size_t (*get_entropy)(IPSI_DRBG_CTX *ctx, SEC_UCHAR **pout, SEC_INT entropy, size_t min_len, size_t max_len)
 *       SEC_VOID (*cleanup_entropy)(IPSI_DRBG_CTX *ctx, SEC_UCHAR *out, size_t olen)
 *     size_t entropy_blocklen
 *     size_t (*get_nonce)(IPSI_DRBG_CTX *ctx, SEC_UCHAR **pout, SEC_INT entropy, size_t min_len, size_t max_len)
 *     SEC_VOID (*cleanup_nonce)
 *     size_t (*get_adin) IPSI_DRBG_CTX *ctx, SEC_UCHAR **pout
 *       SEC_VOID (*cleanup_adin)(IPSI_DRBG_CTX *ctx, SEC_UCHAR *out, size_t olen)
 *       SEC_INT (*rand_seed_cb)(IPSI_DRBG_CTX *ctx, const SEC_VOID *buff, SEC_INT num)
 *       SEC_INT (*rand_add_cb)(IPSI_DRBG_CTX *ctx, const SEC_VOID *buff, SEC_INT num, double entropy)
 *     SEC_INT health_check_interval
 *     SEC_UINT reseed_interval
 *     SEC_VOID *app_data
 * }IPSI_RAND_SETUP_DATA_S
 *
 * @endcode
 *
 * @datastruct type  The Algorithm ID Type. Based on this Algorithm ID type the DRBG internal algorithm
 * will be decided. The DRBG mechanisms, parameters and strengths are summarized below:
 * - Hash DRBG
 *     - ALGID_SHA1	 => Strength = 128
 *     - ALGID_SHA224 	 => Strength = 192
 *     - ALGID_SHA256 	 => Strength = 256
 *     - ALGID_SHA384 	 => Strength = 256
 *     - ALGID_SHA512 	 => Strength = 256
 * - HMAC DRBG
 *     - ALGID_HMAC_SHA1	   => Strength = 128
 *     - ALGID_HMAC_SHA224 	 => Strength = 192
 *     - ALGID_HMAC_SHA256 	 => Strength = 256
 *     - ALGID_HMAC_SHA384 	 => Strength = 256
 *     - ALGID_HMAC_SHA512 	 => Strength = 256
 * - CTR DRBG
 *     - ALGID_AES128_CTR	   => Strength = 128
 *     - ALGID_AES192_CTR  	 => Strength = 192
 *     - ALGID_AES256_CTR 		 => Strength = 256
 *
 * @datastruct flags The Flag variable which will provide additional flag input during init.
 * For example if flags variable is set with IPSI_CRYPT_DRBG_FLAG_TEST the DRBG Test feature will be enabled.
 * For CTR DRBG the flag IPSI_CRYPT_DRBG_FLAG_CTR_USE_DF enables the use of a derivation function.
 * If this flag is not set a derivation function is not used.
 *
 * @datastruct get_entropy Pointer to the Get Entropy Callback
 *  The callbacks get_entropy request "entropy" bits of entropy in a buffer of between min_len and max_len bytes.
 *  The function should set *pout to the buffer containing the entropy and return the length in bytes of the buffer.
 *  If the source of entropy is unable to satisfy the request it MUST return zero. This will place the DRBG in an error
 *  condition due to the entropy source failure.
 *
 * @datastruct cleanup_entropy Pointer to the Cleanup Entropy Callback
 * The callback cleanup_entropy are called after the entropy buffers have been used and can be utilized to zeroize
 * the buffers. The "out" and "olen" parameters contains the same value returned by the get_entropy function.
 *
 * @datastruct entropy_blocklen This is used to specify the block length of the underlying entropy source.
 * This is used for the continuous RNG test on the entropy source. The block length must be greater than one
 * and maximum value can be upto 2^16 (i.e., 65536 bytes). Trying to set block length as more than 2^16 will
 * lead to failure
 *
 * @datastruct get_nonce Pointer to the Get NonceCallback
 *  The callbacks get_nonce request "entropy" bits of nonce in a buffer of between min_len and max_len bytes.
 *  The function should set *pout to the buffer containing the nonce and return the length in bytes of the buffer.
 *  If the source of nonce is unable to satisfy the request it MUST return zero. This will place the DRBG in an error
 *  condition due to the nonce source failure.
 *
 * @datastruct cleanup_nonce Pointer to the Cleanup NonceCallback
 * The callback cleanup_nonce are called after the nonce buffers have been used and can be utilized to zeroize
 * the buffers. The "out" and "olen" parameters contains the same value returned by the get_nonce function.
 *
 * @datastruct get_adin Pointer to the Get Additional Input Callback
 * The get_adin callback is used to retrieve optional additional data used whenever a request for
 * random data is made using IPSI_CRYPT_rand_bytes(). When this operation is complete cleanup_adin
 * is called to release the buffer.
 * Sets "additional input" when generating random data. This could be for ex the current PID, a time value and a
 * counter.
 *
 * @datastruct cleanup_adin Pointer to the Cleanup Additional Input Callback
 *
 * @datastruct rand_seed_cb Pointer to the Get Rand Seed Callback
 * The callback rand_seed_cb is called directly whenever IPSI_CRYPT_rand_seed() is called. This is
 * entirely application defined and could be used for example to add seed information to the entropy source.
 * If the operation is success it must return 1.
 *
 * @datastruct rand_add_cb Pointer to the Additional Rand Input Callback
 * The callback rand_add_cb is called directly whenever IPSI_CRYPT_rand_add() is called. This is
 * entirely application defined and could be used for example to add seed information to the entropy source.
 * If the operation is success it must return 1.
 *
 * @datastruct app_data Set an application defined pointer value. The meaning of this pointer is application defined and
 * might for example contain a pointer to a handle representing the entropy source and the get_entropy function
 * could retrieve and make use of that pointer.
 *
 * @datastruct reseed_interval This parameter is optional and if it is set to 0 then default value will be considered.
 * This is used to Set the Interval for reseed in the DRBG Module.  The default is 2^24 generate operations.
 * These values are lower than the maximums specified in SP800-90.
 *
 * @datastruct health_check_interval This parameter is optional and if it is set to 0 then default value will be
 * considered.  Health checks are performed when a DRBG is first initiated, during reseed and every
 * health_check_interval calls to the generate function. This interval is by default 2^24 but can be modified
 * by using this parameter.
 *
 */
typedef struct stRandDefaultDrbgCtxSetUpData {
    SEC_INT type;
    SEC_UINT flags;
    SEC_UCHAR *pers;
    size_t perslen;
    size_t (*get_entropy)(IPSI_DRBG_CTX *ctx, SEC_UCHAR **pout, SEC_INT entropy, size_t min_len, size_t max_len);
    SEC_VOID (*cleanup_entropy)(IPSI_DRBG_CTX *ctx, SEC_UCHAR *out, size_t olen);
    size_t entropy_blocklen;
    size_t (*get_nonce)(IPSI_DRBG_CTX *ctx, SEC_UCHAR **pout, SEC_INT entropy, size_t min_len, size_t max_len);
    SEC_VOID (*cleanup_nonce)(IPSI_DRBG_CTX *ctx, unsigned char *out, size_t olen);
    size_t (*get_adin)(IPSI_DRBG_CTX *ctx, SEC_UCHAR **pout);
    SEC_VOID (*cleanup_adin)(IPSI_DRBG_CTX *ctx, SEC_UCHAR *out, size_t olen);
    SEC_INT (*rand_seed_cb)(IPSI_DRBG_CTX *ctx, const SEC_VOID *buff, SEC_INT num);
    SEC_INT (*rand_add_cb)(IPSI_DRBG_CTX *ctx, const SEC_VOID *buff, SEC_INT num, double entropy);
    SEC_VOID *app_data;
    SEC_UINT reseed_interval;
    SEC_INT health_check_interval;
} IPSI_RAND_SETUP_DATA_S;

/*
    Func Name:  IPSI_CRYPT_drbg_get_app_data
*/
/**
 * @defgroup IPSI_CRYPT_drbg_get_app_data
 * @ingroup DRBG_HLF
 * @par Prototype
 * @code
 *  SEC_VOID *IPSI_CRYPT_drbg_get_app_data(IPSI_DRBG_CTX *dctx)
 * @endcode
 *
 * @par Purpose
 * This function is used to Get the Application Specific Data which has been set in the DRBG Module.
 *
 * @par Description
 * This function is used to Get the Application Specific Data which has been set in the DRBG Module.
 *
 * @param[in] dctx (Data Input) Pointer to DRBG Context  [N/A]
 *
 * @retval SEC_VOID* (Data Output) The Application data set by application in the DRBG Module [SEC_VOID*|N/A]
 * @retval SEC_VOID* (Status Output) Invalid parameter passed [SEC_NULL|N/A]
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
CRYPTOLINKDLL SEC_VOID *IPSI_CRYPT_drbg_get_app_data(const IPSI_DRBG_CTX *dctx);

/*
Func Name:  IPSI_CRYPT_rand_init
*/
/**
 * @defgroup IPSI_CRYPT_rand_init
 * @ingroup DRBG_HLF
 * @par Prototype
 * @code
 * SEC_UINT32 IPSI_CRYPT_rand_init(IPSI_RAND_SETUP_DATA_S *pstSetUpInfo, IPSI_AUX_PAR_S *pstAuxPar)
 * @endcode
 *
 * @par Purpose
 * This function is used to initialize the default global DRBG.
 *
 * @par Description
 * IPSI_CRYPT_rand_init will initialize the default global DRBG
 *
 * @param[in] pstSetUpInfo (Data Input) Pointer to structure containing initialization information. Application need to
 *  update the structure before calling this API[N/A]
 * @param[in] pstAuxPar (Data Input)  Application can pass this as NULL. This parameter is kept for future use.
 * Pass as SEC_NULL. [N/A]
 *
 * @retval SEC_UINT32 (Status Output) On API Invocation is Success [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 (Status Output) Invalid parameters or arguments passed [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 (Status Output) DRBG health check failure for the selected DRBG mechanism
 * and type [SEC_CRYPT_ERR_DRBG_HEALTH_TEST_FAIL|N/A]
 * @retval SEC_UINT32 (Status Output) Invalid or unsupported DRBG type [SEC_CRYPT_ERR_UNSUPPORTED_DRBG_TYPE|N/A]
 * @retval SEC_UINT32 (Status Output) Internal error during initialization of DRBG
 * [SEC_CRYPT_ERR_ERROR_INITIALISING_DRBG|N/A]
 * @retval SEC_UINT32 (Status Output) Personalization string is of invalid length
 * [SEC_CRYPT_ERR_PERSONALISATION_STRING_TOO_LONG|N/A]
 * @retval SEC_UINT32 (Status Output) DRBG not initialized [SEC_CRYPT_ERR_DRBG_NOT_INITIALISED|N/A]
 * @retval SEC_UINT32 (Status Output) During DRBG instantiate, DRBG already found in error state
 * [SEC_CRYPT_ERR_DRBG_INSTANTIATE_IN_ERROR_STATE|N/A]
 * @retval SEC_UINT32 (Status Output) During DRBG instantiate, DRBG already found to be instantiated
 * [SEC_CRYPT_ERR_DRBG_ALREADY_INSTANTIATED|N/A]
 * @retval SEC_UINT32 (Status Output) Error retrieving entropy during DRBG Instantiate
 * [SEC_CRYPT_ERR_DRBG_INSTANTIATE_RETRIEVING_ENTROPY|N/A]
 * @retval SEC_UINT32 (Status Output) Error retrieving nonce during DRBG
 * Instantiate[SEC_CRYPT_ERR_RETRIEVING_NONCE|N/A]
 * @retval SEC_UINT32 (Status Output) DRBG mechanism Instantiation error[SEC_CRYPT_ERR_DRBG_INSTANTIATION|N/A]
 * @retval SEC_UINT32 (Status Output) On other errors [SEC_ERR|N/A]
 *
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * - This function should be called only in main thread at the beginning of the application.
 * - If DRBG is used in multithread application, then  IPSI_CRYPT_enableDrbgMultithread should be called
 *    before IPSI_CRYPT_rand_init
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 IPSI_CRYPT_rand_init(const IPSI_RAND_SETUP_DATA_S *pstSetUpInfo, IPSI_AUX_PAR_S *pstAuxPar);

/*
    Func Name:  IPSI_CRYPT_rand_bytes
*/
/**
* @defgroup IPSI_CRYPT_rand_bytes
* @ingroup DRBG_HLF
* @par Prototype
* @code
*  SEC_INT IPSI_CRYPT_rand_bytes(SEC_UCHAR *out, SEC_INT count)
* @endcode
*
* @par Purpose
* This function is used to Get the Generated Random DRBG Bytes from the default global DRBG Module
*
* @par Description
* After getting the default DRBG context, initializing and instantiating, and setting appropriate callbacks,
* the DRBG Module can be used to obtain the required DRBG random bytes. Hence this API is used for this purpose.
* IPSI_CRYPT_rand_bytes attempts to generate "outlen" bytes of random data from the DRBG, using optional
* additional input. If an attempt is made to request too much data for a single request or to supply additional input
* of an invalid length a non-fatal error is returned. If an internal request for entropy fails a fatal error occurs
* and the DRBG is placed in an error state. The caller must un-instantiate and re-instantiate the DRBG before
* attempting further calls. The maximumm random number generated in one iteration is 65536, so if the count is big than
it will
* loop for more number of times i.e. loop * 65536 = count
*
* @param[out] out (Data Output) The Outputted DRBG Bytes If successful. Buffer should be equal or more than "count"
bytes [N/A]
* @param[in] count (Data Input) The required number of DRBG random bytes.  [N/A]
*
* @retval SEC_UINT32 (Status Output)  On API Invocation is Success [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 (Status Output) API invocation is Failure [SEC_ERR|N/A]

* @retval SEC_UINT32 (Status Output)  Invalid argument passed [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 (Status Output) DRBG health check failure for the selected DRBG mechanism and type
* [SEC_CRYPT_ERR_DRBG_HEALTH_TEST_FAIL|N/A]
* @retval SEC_UINT32 (Status Output)  During DRBG generate, DRBG already found in error state
* [SEC_CRYPT_ERR_DRBG_GENERATE_IN_ERROR_STATE|N/A]
* @retval SEC_UINT32 (Status Output)  During DRBG generate, DRBG found to be not instantiated
* [SEC_CRYPT_ERR_DRBG_GENERATE_NOT_INSTANTIATED|N/A]
* @retval SEC_UINT32 (Status Output)  Additional input is of invalid length
* [SEC_CRYPT_ERR_DRBG_ADDITIONAL_INPUT_TOO_LONG|N/A]
* @retval SEC_UINT32 (Status Output)  Error retrieving entropy during DRBG Reseed
* [SEC_CRYPT_ERR_DRBG_RESEED_RETRIEVING_ENTROPY|N/A]
* @retval SEC_UINT32 (Status Output)  DRBG mechanism Reseed error[SEC_CRYPT_DRBG_RESEED_ERROR|N/A]
* @retval SEC_UINT32 (Status Output)  DRBG mechanism Generate error[SEC_CRYPT_DRBG_GENERATE_ERROR|N/A]
* @retval SEC_UINT32 (Status Output)  DRBG error retrieving additional input
* [SEC_CRYPT_DRBG_ERROR_RETRIEVING_ADDITIONAL_INPUT|N/A]
* @retval SEC_UINT32 (Status Output)  Attempt is made to request too much data for a single request
* [SEC_CRYPT_ERR_REQUEST_TOO_LARGE_FOR_DRBG|N/A]
*
* @par Required Header File
* sec_crypto.h
*
* @par Note
* \n
* When DRBG is initialized with the same input(entropy, nounce, addition input, seed, etc)of previous initialization,
* then the sequence of random number is same as previous initialization.
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 IPSI_CRYPT_rand_bytes(SEC_UCHAR *out, SEC_INT count);

/*
    Func Name:  IPSI_CRYPT_rand_status
*/
/**
 * @defgroup IPSI_CRYPT_rand_status
 * @ingroup DRBG_HLF
 * @par Prototype
 * @code
 * SEC_INT IPSI_CRYPT_rand_status(SEC_VOID)
 * @endcode
 *
 * @par Purpose
 * This function is used to get the status of the default global DRBG Module
 *
 * @par Description
 * This function is used to get the status of the default global DRBG Module.
 *
 *
 * @retval SEC_INT (Status Output) SEC_TRUE If DRBG is ready [SEC_TRUE|N/A]
 * @retval SEC_INT (Status Output) SEC_FALSE If DRBG is not ready [SEC_FALSE|N/A]
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
CRYPTOLINKDLL SEC_INT IPSI_CRYPT_rand_status(void);

/*
    Func Name:  IPSI_CRYPT_rand_strength
*/
/**
 * @defgroup IPSI_CRYPT_rand_strength
 * @ingroup DRBG_HLF
 * @par Prototype
 * @code
 *  SEC_INT IPSI_CRYPT_rand_strength(SEC_VOID)
 * @endcode
 *
 * @par Purpose
 * This function returns the security strength in bits of the global default DRBG Module
 *
 * @par Description
 * This function is used to get the security strength of the global default DRBG in bits
 *
 *
 * @retval SEC_INT (Data Output)  The current random security strength of the DRBG Module in bits [SEC_INT|N/A]
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
CRYPTOLINKDLL SEC_INT IPSI_CRYPT_rand_strength(void);

/*
    Func Name:  IPSI_CRYPT_rand_cleanup
*/
/**
 * @defgroup IPSI_CRYPT_rand_cleanup
 * @ingroup DRBG_HLF
 * @par Prototype
 * @code
 *  SEC_VOID IPSI_CRYPT_rand_cleanup(void)
 * @endcode
 *
 * @par Purpose
 * This function cleans up the default global DRBG
 *
 * @par Description
 * After performing all Default global DRBG operation, Application should call this function during finalization to
 * clean up the DRBG. This function zeroizes any CSPs and returns the DRBG to an uninitialized state.
 *
 *
 * @retval None [N/A]
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
CRYPTOLINKDLL SEC_VOID IPSI_CRYPT_rand_cleanup(void);

/*
    Func Name:  IPSI_CRYPT_rand_seed
*/
/**
 * @defgroup IPSI_CRYPT_rand_seed
 * @ingroup DRBG_HLF
 * @par Prototype
 * @code
 *  SEC_INT IPSI_CRYPT_rand_seed(const SEC_VOID *seed, int seedlen)
 * @endcode
 *
 * @par Purpose
 * This function is used at various points to add data which may be useful for adding entropy to the default
 * global DRBG module
 *
 * @par Description
 * This function is used at various points to add data which may be useful for adding entropy to the default global
 * DRBG. The callback rand_seed_cb is called directly whenever IPSI_CRYPT_rand_seed() is called. The callback is
 * entirely application defined and could be used for example to add seed information to the entropy source. And the
 * callback must return 1 if the operation is success.
 *
 * @param[in] seed (Data Input) Seed Value [N/A]
 * @param[in] seedlen (Data Input) Length of the Seed Value [N/A]
 *
 * @retval SEC_INT (Status Output) On API Invocation is Success [1|N/A]
 * @retval SEC_INT (Status Output) API invocation is Failure [Something other than 1|N/A]
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
CRYPTOLINKDLL SEC_INT IPSI_CRYPT_rand_seed(const SEC_VOID *seed, SEC_INT seedlen);

/*
    Func Name:  IPSI_CRYPT_rand_add
*/
/**
* @defgroup IPSI_CRYPT_rand_add
* @ingroup DRBG_HLF
* @par Prototype
* @code
*  SEC_INT IPSI_CRYPT_rand_add(const SEC_VOID *seed, int seedlen, double add_entropy)
* @endcode
*
* @par Purpose
* This function is used to Add Entropy the default global DRBG module
*
* @par Description
* This function is used at various points to add data which may be useful for adding entropy to the default global DRBG.
* This function is similar to IPSI_CRYPT_rand_seed except that the data supplied has entropy "entropy".
* The callback rand_add_cb is called directly whenever IPSI_CRYPT_rand_add() is called. The callback is
* entirely application defined and could be used for example to add seed information to the entropy source. And the
* callback must return 1 if the operation is success.

*
* @param[in] seed (Data Input)  Seed Value [N/A]
* @param[in] seedlen (Data Input) Length of the Seed Value [N/A]
* @param[in] add_entropy (Data Input) Data supplied has the entropy equivalent to this parameter  [N/A]
*
* @retval SEC_INT (Status Output)  On API Invocation is Success [1|N/A]
* @retval SEC_INT (Status Output) API invocation is Failure [Something other than 1|N/A]
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
CRYPTOLINKDLL SEC_INT IPSI_CRYPT_rand_add(const SEC_VOID *seed, SEC_INT seedlen, double add_entropy);

/*
    Func Name:  IPSI_CRYPT_rand_health_check
*/
/**
 * @defgroup IPSI_CRYPT_rand_health_check
 * @ingroup DRBG_HLF
 * @par Prototype
 * @code
 *  SEC_UINT32 IPSI_CRYPT_rand_health_check()
 * @endcode
 *
 * @par Purpose
 * This function initiates a health check on the global default DRBG Module
 *
 * @par Description
 *
 * This function initiates a health check on the global default DRBG Module. In addition health checks
 * are also performed when a DRBG is first initiated when a DRBG is reseeded and every health_check_interval call to
 * the generate function. A health check is performed on a test instantiation using the same mechanism and parameters.
 * After instantiation of the global default DRBG, the DRBG Module can be tested by the application for the
 * "Health Test". This test runs standard Known Answer Tests and checks for the same mechanism of global DRBG module.
 *
 *
 * @retval SEC_UINT32 (Status Output)  On Success [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 (Status Output)  On Failure [SEC_ERR|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * \n
 * - Before calling this API, please make sure that DRBG is initialized for random number generation by calling
 *   IPSI_CRYPT_rand_init. If application don't want to use DRBG then appplication should call
 *   IPSI_CRYPT_enable_drbg(0) to disable the DRBG Random feature.
 * - DRBG is enabled by default.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 IPSI_CRYPT_rand_health_check(void);

/*
    Func Name:  IPSI_CRYPT_rand_first_error
*/
/**
* @defgroup IPSI_CRYPT_rand_first_error
* @ingroup DRBG_HLF
* @par Prototype
* @code
*  SEC_UINT32 IPSI_CRYPT_rand_first_error(void)
* @endcode
*
* @par Purpose
* This function indicates the error happened in IPSI_CRYPT_rand_bytes().
* This function can be used along with when CRYPT_random() API works in DRBG mode.
*
* @par Description
*
* Whenever random number generation API IPSI_CRYPT_rand_bytes() fails, the return value of
* IPSI_CRYPT_rand_bytes() will indicate the actual error happened.
*
* But when IPSI_CRYPT_rand_bytes() is used in multiple threads, that is multiple threads try to generate
* random numbers. When a error happens in IPSI_CRYPT_rand_bytes() and in a particular thread, all others
* will go into error state for random number generation using IPSI_CRYPT_rand_bytes().
*
* In that case to get the first error happened in DRBG, users should call IPSI_CRYPT_rand_first_error() to
* get the error code. Based on this error code, necessary action should be taken by the main thread and the
* DRBG has to be re-instantiated.

* IPSI_CRYPT_rand_first_error() can be used to get the error code for DRBG used in single thread also.
* IPSI_CRYPT_rand_first_error() will indicate the error code only for critical errors happened during random
* number generation. The list of errors indicated by IPSI_CRYPT_rand_first_error() are given in the retrun
* value section.
*
* IPSI_CRYPT_rand_first_error() can be used along with CRYPT_random() when DRBG mode is enabled
*
* @retval SEC_UINT32 (Status Output)  If no critical error occured[ZERO|N/A]
* @retval SEC_UINT32 (Status Output) DRBG health check failure for the selected DRBG mechanism and type
* [SEC_CRYPT_ERR_DRBG_HEALTH_TEST_FAIL|N/A]
* @retval SEC_UINT32 (Status Output)  During DRBG generate, DRBG already found in error state
* [SEC_CRYPT_ERR_DRBG_GENERATE_IN_ERROR_STATE|N/A]
* @retval SEC_UINT32 (Status Output)  DRBG mechanism Generate error[SEC_CRYPT_DRBG_GENERATE_ERROR|N/A]
* @retval SEC_UINT32 (Status Output)  Error retrieving entropy during DRBG Reseed
* [SEC_CRYPT_ERR_DRBG_RESEED_RETRIEVING_ENTROPY|N/A]
* @retval SEC_UINT32 (Status Output)  DRBG mechanism Reseed error[SEC_CRYPT_DRBG_RESEED_ERROR|N/A]
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
CRYPTOLINKDLL SEC_UINT32 IPSI_CRYPT_rand_first_error(void);

/*
    Func Name:  IPSI_CRYPT_enable_drbg
*/
/**
 * @defgroup IPSI_CRYPT_enable_drbg
 * @ingroup DRBG_HLF
 * @par Prototype
 * @code
 * SEC_UINT32 IPSI_CRYPT_enable_drbg(SEC_UINT32 uiMode)
 * @endcode
 *
 * @par Purpose
 * This function is used to enable or disable the DRBG functionality.
 *
 * @par Description
 * This function needs to be called by the user if DRBG mode needs to be enabled for generating random numbers.
 * IPSI_CRYPT_DRBG_ENABLE needs to be passed to the function to enable DRBG and IPSI_CRYPT_DRBG_DISABLE
 * needs to be passed to disbale DRBG. It is a global setting.
 *
 * @param[in] uiMode (Data Input) Mode indicating to enable or disable
 *           DRBG  [IPSI_CRYPT_DRBG_ENABLE or IPSI_CRYPT_DRBG_DISABLE|N/A]
 *
 * @retval SEC_UINT32 (Status Output)  On Success [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 (Status Output)  On Failure. If input mode is invalid [SEC_ERR|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * -If the application is going to switch between the DRBG and AnsiX9.31 random number than it should call
 *   IPSI_CRYPT_enableDrbgSwitchLock first and than application can call IPSI_CRYPT_enable_drbg to switch between these
 * two. Only one  time call of IPSI_CRYPT_enableDrbgSwitchLock is sufficient in the whole stack. \n N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 IPSI_CRYPT_enable_drbg(SEC_UINT32 uiMode);

#ifndef __IPSI_NO_DRBG_LOCK__
/**
 * @defgroup IPSI_CRYPT_enableDrbgMultithread
 * @ingroup DRBG_HLF
 * @par Prototype
 * @code
 * SEC_UINT32 IPSI_CRYPT_enableDrbgMultithread(SEC_VOID)
 * @endcode
 *
 * @par Purpose
 * This function creates the necessary locks for the use of DRBG in multithread.
 *
 * @par Description
 * This function creates the necessary locks for the use of DRBG in multithread environment.
 * This API should be mandatorily called when DRBG is used in multi thread.
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_UINT32 (Status Output)  On Success [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 (Status Output)  On Failure. If some problem occured in creating the locks [SEC_ERR|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * IPSI_CRYPT_enableDrbgSwitchLock
 */
CRYPTOLINKDLL SEC_UINT32 IPSI_CRYPT_enableDrbgMultithread(void);

/**
 * @defgroup IPSI_CRYPT_enableDrbgSwitchLock
 * @ingroup DRBG_HLF
 * @par Prototype
 * @code
 * SEC_UINT32 IPSI_CRYPT_enableDrbgSwitchLock(SEC_VOID)
 * @endcode
 *
 * @par Purpose
 * This function creates the necessary locks for the use of IPSI_CRYPT_enable_drbg() API in multithread.
 *
 * @par Description
 * This function creates the necessary locks for the use of IPSI_CRYPT_enable_drbg() API in multithread.
 * Users who are switching between DRBG mode and non-DRBG mode using IPSI_CRYPT_enable_drbg()
 * in multithread environment should mandatorily call this API.
 * But it is strictly advised that users shouldn't use IPSI_CRYPT_enable_drbg() in multithread, and just
 * need to call IPSI_CRYPT_enable_drbg() in the main thread as a one time activity. Switching between DRBG
 * and non-DRBG modes in mutliple threads is strictly not advisable
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_UINT32 (Status Output)  On Success [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 (Status Output)  On Failure. If some problem occured in creating the locks [SEC_ERR|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * IPSI_CRYPT_enableDrbgMultithread
 */
CRYPTOLINKDLL SEC_UINT32 IPSI_CRYPT_enableDrbgSwitchLock(void);

#endif /* end of __IPSI_NO_DRBG_LOCK__ */
#endif /* end of __IPSI_NO_DRBG__ */

#if !(defined(__IPSI_NO_SEC_64BIT) || defined(__IPSI_NO_SCRYPT))

/**
 * @defgroup IPSI_SCRYPT_CB
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * typedef SEC_INT (*IPSI_SCRYPT_CB) (void);
 *
 * @endcode
 *
 * @par Purpose
 * Callback function called from scrypt operations.
 *
 * @par Description
 * This function is called from scrypt operations. Currently the return value from the callback function
 * is ignored and intended only for future use.
 *
 *
 * @retval SEC_INT Currently Nothing to be returned [N/A|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * N/A
 *
 * @par Related Topics
 * IPSI_CRYPT_setScryptCallback
 */
typedef SEC_INT (*IPSI_SCRYPT_CB)(void);

/*
Func Name:  IPSI_CRYPT_setScryptCallback
*/
/**
* @defgroup IPSI_CRYPT_setScryptCallback
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 IPSI_CRYPT_setScryptCallback (IPSI_SCRYPT_CB pCB, SEC_UINT32 lTimeOut);
* @endcode
*
* @par Purpose
* This function is used to set the callback function for BN operations and its timeout value.
*
* @par Description
* IPSI_CRYPT_setScryptCallback will be called at the time of BN operations (if the timeout is occurred)
*
* @param[in] pCB Callback function which will be called at when the timer expires [N/A]
* @param[in] lTimeOut Timeout value in ticks. If the timeout value is more than LONG_MAX then API will return
* error SEC_ERR_INVALID_ARG.[N/A]
*
* @retval SEC_UINT32 On success [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 lTimeOut is greater than LONG_MAX [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 lTimeOut is zero and  pCB is not NULL [SEC_ERR_INVALID_ARG|N/A]

* @par Required Header File
* sec_crypto.h
*
* @par Note
* - It is a global function, application need to make sure that the function will not be called in parallel
*    with any scrypt operation.
* - To remove the timeout callback, application need to pass the pCB as NULL.
* - NULL function pointer with some timeout value is a valid input to this API. It will remove the callback
*    function call.
*
* @par Related Topics
* N/A
*
*/
CRYPTOLINKDLL SEC_UINT32 IPSI_CRYPT_setScryptCallback(IPSI_SCRYPT_CB pCB, SEC_UINT32 lTimeOut);

#endif

/*
Func Name:  IPSI_CRYPT_setRsaKeyGenRetryCount
*/
/**
* @defgroup IPSI_CRYPT_setRsaKeyGenRetryCount
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_VOID IPSI_CRYPT_setRsaKeyGenRetryCount (SEC_UINT32 uiRetryCount);
* @endcode
*
* @par Purpose
* This function is used to set the maximum retry count for the random number generation while doing the rsa key pair
generation.
*
* @par Description
* IPSI_CRYPT_setRsaKeyGenRetryCount will be used to set the maxuimum retry count for the random number generation
* while doing the rsa key pair generation. By default the value is 0 which means that the loop is infinite and it will
break only
* when the random number staisfies the condition required for RSA key.
*
* @param[in] uiRetryCount Iteration count for the random number generation [N/A]
*
* @retval None [N/A]

* @par Required Header File
* sec_crypto.h
*
* @par Note
* - This is a global function and it should not be used in parallel with RSA key pair generation.
* N/A
*
* @par Related Topics
* N/A
*
*/
CRYPTOLINKDLL SEC_VOID IPSI_CRYPT_setRsaKeyGenRetryCount(SEC_UINT32 uiRetryCount);

/*
Func Name:  IPSI_CRYPT_getRsaKeyGenRetryCount
*/
/**
 * @defgroup IPSI_CRYPT_getRsaKeyGenRetryCount
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT IPSI_CRYPT_getRsaKeyGenRetryCount (SEC_VOID);
 * @endcode
 *
 * @par Purpose
 * This function is used to get the retry count for the random number generation while doing the rsa key pair
 * generation.
 *
 * @par Description
 * IPSI_CRYPT_getRsaKeyGenRetryCount will be used to get the retry count for the random number generation
 * while doing the rsa key pair generation. By default the value is 0 which means that the loop is infinite and it will
 * break only when the random number staisfies the condition required for RSA key.
 *
 *
 * @retval SEC_UINT32 retry count value for the random number generation for RSA key generation  [N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * N/A
 *
 * @par Related Topics
 * N/A
 *
 */
CRYPTOLINKDLL SEC_UINT32 IPSI_CRYPT_getRsaKeyGenRetryCount(void);

/*
Func Name:  IPSI_CRYPT_selftest_drbg
*/
/**
 * @defgroup IPSI_CRYPT_selftest_drbg
 * @ingroup DRBG_Self_Test_Function
 * @par Prototype
 * @code
 * SEC_UINT32 IPSI_CRYPT_selftest_drbg()
 * @endcode
 *
 * @par Purpose
 * This function is used to Run DRBG self tests.
 *
 * @par Description
 * IPSI_CRYPT_selftest_drbg is used to Run DRBG self tests in the DRBG mode.
 *
 * @retval SEC_UINT32 (Status Output) On successful return [SEC_TRUE|N/A]
 * @retval SEC_UINT32 (Status Output) On failure [SEC_FALSE|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 IPSI_CRYPT_selftest_drbg(void);

/*
Func Name:  IPSI_CRYPT_selftest_drbg_all
*/
/**
 * @defgroup IPSI_CRYPT_selftest_drbg_all
 * @ingroup DRBG_Self_Test_Function
 * @par Prototype
 * @code
 * SEC_UINT32 IPSI_CRYPT_selftest_drbg_all()
 * @endcode
 *
 * @par Purpose
 * This function is used to Run DRBG self tests.
 *
 * @par Description
 * IPSI_CRYPT_selftest_drbg_all is used to Run DRBG self tests in the DRBG mode.
 * This function performs extensive error checking as required by SP800-90 induce several failure modes and check an
 * error condition is set.
 *
 *
 * @retval SEC_UINT32 (Status Output) On successful return. [SEC_TRUE|N/A]
 * @retval SEC_UINT32 (Status Output) On failure. [SEC_FALSE|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 IPSI_CRYPT_selftest_drbg_all(void);

/**
 * @defgroup IPSI_CRYPTO_TEST_CB
 * @ingroup DRBG_Self_Test_Function
 * @par Prototype
 * @code
 * typedef SEC_INT (*IPSI_CRYPTO_TEST_CB)(SEC_INT iOperation, SEC_INT iTestId, SEC_INT iTestSubId, SEC_VOID *ex);
 *
 * @endcode
 *
 * @par Purpose
 * This is function pointer which is used to set callback  using IPSI_CRYPT_post_set_callback function.
 * This callback will be called during all Self Tests.
 *
 * @par Description
 *  This is function pointer which is used to set callback  using IPSI_CRYPT_post_set_callback function.
 *  This callback will be called during all Self Tests.
 *
 * @param[in] iOperation Test callback operation value  [N/A]
 * @param[in] iTestId The Crypto test Id of type IPSI_CRYPTO_TEST_ID_E [N/A]
 * @param[in] iTestSubId  The crypto test SubID [N/A]
 * @param[in] ex For future purpose  [N/A]
 * @retval SEC_INT The callback should return SEC_TRUE or SEC_FALSE.If the callback return a value other than SEC_FALSE,
 * it will be treated as SEC_TRUE. [N/A|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * N/A
 *
 * @par Related Topics
 * IPSI_CRYPT_post_set_callback
 */
typedef SEC_INT (*IPSI_CRYPTO_TEST_CB)(SEC_INT iOperation, SEC_INT iTestId, SEC_INT iTestSubId, SEC_VOID *ex);

/*
Func Name:  IPSI_CRYPT_post_set_callback
*/
/**
 * @defgroup IPSI_CRYPT_post_set_callback
 * @ingroup DRBG_Self_Test_Function
 * @par Prototype
 * @code
 * void IPSI_CRYPT_post_set_callback(IPSI_CRYPTO_TEST_CB post_cb)
 * @endcode
 *
 * @par Purpose
 * This function is used to Set Call back function pointer for POST Tests.
 *
 * @par Description
 * This function is used to Set Call back function pointer for POST Tests, When set with valid function pointer of
 * type IPSI_CRYPTO_TEST_CB, the set application function pointer
 * callback will be called back during all Self Tests .
 *
 * @param[in] post_cb (Data Input) The Callback function pointer of type function pointer of type
 * IPSI_CRYPTO_TEST_CB. [N/A]
 *
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * - The post_cb should return SEC_TRUE or SEC_FALSE. If the callback return a value other than SEC_FALSE,
 * it will be treated as SEC_TRUE.
 * - This is a global function and should not be called in parallel to any selftest or itself.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_VOID IPSI_CRYPT_post_set_callback(IPSI_CRYPTO_TEST_CB post_cb);

/*
Func Name:  IPSI_CRYPT_drbg_reseed
*/
/**
 * @defgroup IPSI_CRYPT_drbg_reseed
 * @ingroup DRBG_HLF
 * @par Prototype
 * @code
 * SEC_UINT32 IPSI_CRYPT_drbg_reseed(IPSI_DRBG_CTX *dctx, const SEC_UCHAR *adin, size_t adinlen)
 * @endcode
 *
 * @par Purpose
 * This function reseeds the DRBG using optional additional input "adin" of length "adinlen".
 *
 * @par Description
 * IPSI_CRYPT_drbg_reseed reseeds the DRBG using optional additional input "adin" of length "adinlen".
 * If the additional input is of an invalid length for the DRBG mechanism a non-fatal error is returned.
 * The get_entropy callback of the DRBG is called internally to request entropy.
 *
 * @param[out] dctx (Data Output) The DRBG context.  [N/A]
 * @param[in] adin (Data Input) The additional input.  [N/A]
 * @param[in] adinlen (Data Input) The additional input length.  [N/A]
 *
 * @retval SEC_UINT32 (Status Output) On Success [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 (Status Output) Reseeding failed [SEC_ERR|N/A]
 * @retval SEC_UINT32 (Status Output)  During DRBG reseed, DRBG already found in error state
 * [SEC_CRYPT_ERR_DRBG_RESEED_IN_ERROR_STATE|N/A]
 * @retval SEC_UINT32 (Status Output)  During DRBG reseed, DRBG found to be not instantiated
 * [SEC_CRYPT_ERR_DRBG_RESEED_NOT_INSTANTIATED|N/A]
 * @retval SEC_UINT32 (Status Output)  Additional input is of invalid length
 * [SEC_CRYPT_ERR_DRBG_ADDITIONAL_INPUT_TOO_LONG|N/A]
 * @retval SEC_UINT32 (Status Output) DRBG health check failure for the selected DRBG mechanism and type
 * [SEC_CRYPT_ERR_DRBG_HEALTH_TEST_FAIL|N/A]
 * @retval SEC_UINT32 (Status Output)  Error retrieving entropy during DRBG Reseed
 * [SEC_CRYPT_ERR_DRBG_RESEED_RETRIEVING_ENTROPY|N/A]
 * @retval SEC_UINT32 (Status Output)  DRBG mechanism Reseed error[SEC_CRYPT_DRBG_RESEED_ERROR|N/A]
 * @retval SEC_UINT32 (Status Output)  DRBG mechanism Generate error[SEC_CRYPT_DRBG_GENERATE_ERROR|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 IPSI_CRYPT_drbg_reseed(IPSI_DRBG_CTX *dctx, const SEC_UCHAR *adin, size_t adinlen);

/**
 * @defgroup IPSI_CRYPT_setCppMode
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * void IPSI_CRYPT_setCppMode(SEC_UINT iEnable)
 * @endcode
 *
 * @par Purpose
 * This is used to enable or disable CPP mode
 *
 * @par Description
 * IPSI_CRYPT_setCppMode is used to enable or disable CPP mode.
 *
 * @par Parameters
 * @param[in] iEnable To enable or disable the CPP mode. Pass 1 to enable the CPP mode and others to disable the CPP
 * mode.  [N/A]
 *
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * \n
 * IPSI_CRYPT_setCppMode is not thread safe API. This API must not be called in multithreads.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL void IPSI_CRYPT_setCppMode(SEC_UINT iEnable);

/**
 * @defgroup IPSI_CRYPT_setDHSharedKeyPadScheme
 * @ingroup Asymmetric_Function
 * @par Prototype
 * @code
 * SEC_UINT32 IPSI_CRYPT_setDHSharedKeyPadScheme( SEC_INT on)
 * @endcode
 *
 * @par Purpose
 * This is used to set the Padding scheme for DH shared secret key.
 *
 * @par Description
 * By default CRYPT_computeDHKey function generates a DH shared secret
 * key which is padded at MSB so that key length is multiple of 4 bytes. This function
 * is used to switch off or on this padding scheme. When this padding scheme is switched
 * off, there is no padding bytes at MSB and the shared key length need not be multiple
 * of 4 bytes. Pass on = 1 to switch the Pad Scheme On. Pass on = 0 to switch the Pad
 * Scheme Off. By default the Pad Scheme is On and the shared secret is padded at MSB
 * to make the key length multiple of 4 bytes.
 *
 * @param[in] on (Data Input) The flag variable to switch on / off the Pad Scheme [N/A]
 *
 * @retval SEC_VOID This function does not return any value [N/A|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * - This function is not thread-safe. Application should call this function in main thread.
 * And a multi-threaded application should use its own locking mechanism to ensure
 * that the function is used in a multi-thread safe way and no other DH APIs are called.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_VOID IPSI_CRYPT_setDHSharedKeyPadScheme(SEC_INT on);

/**
 * @defgroup CRYPT_hmacCtxFinal
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_hmacCtxFinal(CRYPT_CTX ctx, SEC_UCHAR *pucDigest, SEC_UINT32 *pulLen, SEC_UINT32 ulInpDigestLen)
 * @endcode
 *
 * @par Purpose
 * This is used to generate the final mac without deleting the context.
 *
 * @par Description
 * CRYPT_hmacCtxFinal is used to generate the final mac without deleting the context.
 * After using this API, application can use CRYPT_hmacCtxFree to free the mac context.
 *
 * @param[in] pCtx The context for MAC [N/A]
 * @param[out] pucMAC The hashed MAC [N/A]
 * @param[out] pulMACLen The out MAC length [N/A]
 * @param[in] ulInpMACLen The input MAC length [N/A]
 *
 * @retval SEC_UINT32 If mac is calculated [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 If context values are not correct [SEC_CRYPT_ERR_INVALID_CTX|N/A]
 * @retval SEC_UINT32 If arguments are missing [SEC_ERR_INVALID_ARG|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * - This function is not thread-safe. Application should call this function in main thread.
 * And a multi-threaded application should use its own locking mechanism to ensure
 * that the function is used in a multi-thread safe way and no other DH APIs are called.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_hmacCtxFinal(CRYPT_CTX ctx, SEC_UCHAR *pucMAC, SEC_UINT32 *pulMACLen, SEC_UINT32 ulInpMACLen);

/**
 * @defgroup CRYPT_hmacCtxCopy
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_hmacCtxCopy(CRYPT_CTX dCtx, CRYPT_CTX sCtx)
 * @endcode
 *
 * @par Purpose
 * This is used to copy the sCtx value to dCtx.
 *
 * @par Description
 * CRYPT_hmacCtxCopy is used to copy the mac context.
 *
 * @param[out] sCtx The source context of MAC [N/A]
 * @param[out] dCtx The destination context of MAC [N/A]
 *
 * @retval SEC_UINT32 If mac context is copied [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 If context copy failed [SEC_ERR|N/A]
 * @retval SEC_UINT32 If arguments are wrong [SEC_ERR_INVALID_ARG|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * - This function is not thread-safe. Application should call this function in main thread.
 * And a multi-threaded application should use its own locking mechanism to ensure
 * that the function is used in a multi-thread safe way and no other DH APIs are called.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_hmacCtxCopy(CRYPT_CTX dCtx, CRYPT_CTX sCtx);

/**
 * @defgroup CRYPT_hmacCtxFree
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_VOID CRYPT_hmacCtxFree(CRYPT_CTX *ctx)
 * @endcode
 *
 * @par Purpose
 * This is used to free the conetxt.
 *
 * @par Description
 * CRYPT_hmacCtxFree is used to free the mac context.
 *
 * @param[out] ctx The context of MAC [N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * - This function is not thread-safe. Application should call this function in main thread.
 * And a multi-threaded application should use its own locking mechanism to ensure
 * that the function is used in a multi-thread safe way and no other DH APIs are called.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_VOID CRYPT_hmacCtxFree(CRYPT_CTX *ctx);

/**
 * @defgroup IPSI_RAND_FUNC
 * @ingroup DRBG_HLF
 * @par Prototype
 * @code
 * typedef SEC_INT (*IPSI_RAND_FUNC)( SEC_UCHAR *pucBuf, SEC_UINT32 ulLen);
 *
 * @endcode
 *
 * @par Purpose
 * This is function pointer which is used to set callback  using CRYPT_setRandomFunc function.
 * This callback will be called during for all random number genration.
 *
 * @par Description
 * This is function pointer which is used to set callback  using CRYPT_setRandomFunc function.
 * This callback will be called during for all random number genration.
 *
 *
 * @param[out] pucBuf (Data Output) The Outputted random Bytes If successful. Buffer size should be equal or more than
 * "ulLen" bytes [N/A]
 * @param[in] ulLen (Data Input) The required number of  random bytes.  [N/A]

 * @retval SEC_INT On success [SEC_SUCCESS|N/A]
 * @retval SEC_INT On failure [Other than SEC_SUCCESS|N/A]
 *
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 * - In multi-thread application , callback will be invoked from multiple thread  so callback implementation need to be
 *   multithread safe.
 *
 * @par Related Topics
 * CRYPT_setRandomFunc
 */
typedef SEC_INT (*IPSI_RAND_FUNC)(SEC_UCHAR *pucBuf, SEC_UINT32 ulLen);

/*
Func Name:  IPSI_CRYPT_setRandomFunc
*/
/**
 * @defgroup IPSI_CRYPT_setRandomFunc
 * @ingroup DRBG_HLF
 * @par Prototype
 * @code
 * void IPSI_CRYPT_setRandomFunc(IPSI_RAND_FUNC pRandCb);
 * @endcode
 *
 * @par Purpose
 * This function set passed function pointer as new RNG function. This function will replace existing DRBG
 * implemenation to used passer function pointer.
 *
 * @par Description
 * This function set passed function pointer as new RNG function. This function will replace existing DRBG
 * implemenation to used passer function pointer.
 *
 * @param[in] pRandCb (Data Input) The callback function pointer of type IPSI_RAND_FUNC. [N/A]
 *
 * @retval SEC_VOID This does not return any value [N/A|N/A]

 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 *  - As this API set global function pointer , this should be called once before any other VPP API is used.
 *  - This API is not thread safe , mutst not be called from multiple thread
 *  - API allow to reset the callback by passing NULL in API.
 * @par Related Topics
 * IPSI_RAND_FUNC
 */
CRYPTOLINKDLL SEC_VOID IPSI_CRYPT_setRandomFunc(IPSI_RAND_FUNC pRandCb);

typedef int (*ossl_sign)(const void *pkey, const char *optname[], size_t optnamesize, const char *optvalue[], size_t optvalsize, const unsigned char *tobesigned, size_t tobesignedlen, unsigned char *signature, size_t *signatureSize);

typedef int (*ossl_verify)(const void *pkey, const char *optname[], size_t optnamesize, const char *optvalue[], size_t optvalsize, const unsigned char *tobesigned, size_t tobesignedlen, const unsigned char *signature, size_t signatureLength);

typedef int (*ossl_pvt_encode)(const void *pkey, const char *keytype, size_t keybuffsize, unsigned char **buff, size_t *len);

typedef int (*ossl_pub_encode)(const void *pkey, const char *keytype, size_t keybuffsize, unsigned char **buff, size_t *len);

typedef int (*ossl_pvt_decode)(const char *keytype, unsigned char *buff, size_t len, void **pkey);

typedef int (*ossl_pub_decode)(const char *keytype, unsigned char *buff, size_t len, void **pkey);

typedef void (*ossl_pkey_free)(void *pkey);

typedef void (*ossl_free)(void *ptr, const char *file, int line);

// pse function pointer
typedef SEC_UCHAR *(*sec_pub_encode)(const SEC_PKEY_S *pstKey, SEC_UINT32 *pulLen);
typedef SEC_PKEY_S *(*sec_pub_decode)(SEC_UCHAR *pucBuf, SEC_UINT32 ulLen, SEC_UINT32 *pulLen);
typedef SEC_UCHAR *(*sec_pvt_encode)(const SEC_PKEY_S *pstKey, SEC_UINT32 *pulLen);
typedef SEC_PKEY_S *(*sec_pvt_decode)(SEC_UINT32 ulAlg, SEC_UCHAR *pucBuf, SEC_UINT32 ulLen, SEC_UINT32 *pulLen);

typedef struct key_coversion {
    ossl_sign fp_ossl_sign;
    ossl_verify fp_ossl_verify;
    ossl_pvt_encode fp_ossl_pvt_encode;
    ossl_pub_encode fp_ossl_pub_encode;
    ossl_pvt_decode fp_ossl_pvt_decode;
    ossl_pub_decode fp_ossl_pub_decode;
    ossl_pkey_free fp_ossl_pkey_free;
    ossl_free fpossl_free;  // Directly OPENSSL_free can be register

    sec_pvt_encode fp_sec_pvt_encode;
    sec_pvt_decode fp_sec_pvt_deocde;
    sec_pub_encode fp_sec_pub_encode;
    sec_pub_decode fp_sec_pub_decode;
} IPSI_KEY_CONVERSION_S;

/*
Func Name:  IPSI_CRYPT_setConversionCb
*/
/**
 * @defgroup IPSI_CRYPT_setConversionCb
 * @ingroup DRBG_HLF
 * @par Prototype
 * @code
 * int IPSI_CRYPT_setConversionCb(IPSI_KEY_CONVERSION_S *pstCoversion);
 * @endcode
 *
 * @par Purpose
 * This function sets callback required for ed25119 key conversion, sign and verify.
 *
 * @par Description
 * This function sets callback required for ed25119 key conversion, sign and verify.
 *
 * @param[in] pstCoversion pointer to structure having callback function. [N/A]
 *
 * @retval SEC_INT On success [SEC_SUCCESS|N/A]
 * @retval SEC_INT On failure [SEC_ERR|N/A]
 * @retval SEC_INT Invalid arguments [SEC_ERR_INVALID_ARG] [N/A]
 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 *  - As this API set global function pointer , this should be called once before any other VPP API is used.
 *  - This API is not thread safe , must not be called from multiple thread
 * @par Related Topics
 * IPSI_RAND_FUNC
 */
CRYPTOLINKDLL SEC_INT IPSI_CRYPT_setConversionCb(IPSI_KEY_CONVERSION_S *pstCoversion);

/*
Func Name:  IPSI_CRYPT_convertOsslKey
*/
/**
 * @defgroup IPSI_CRYPT_convertOsslKey
 * @ingroup DRBG_HLF
 * @par Prototype
 * @code
 * int IPSI_CRYPT_convertOsslKey(void* pkey, SEC_PKEY_S** pstKey);
 * @endcode
 *
 * @par Purpose
 * This function convert the openssl key(EVP_PKEY) to SEC_PKEY_S . currently only ed25119 key are supported
 *
 * @par Description
 * This function convert the openssl key(EVP_PKEY) to SEC_PKEY_S . currently only ed25119 key are supported
 *
 * @param[in] pkey pointer to openssl keys (EVP_PKEY) [N/A]
 * @param[in] pstKey converted SEC_PKEY_S pointer. [N/A]
 *
 * @retval SEC_INT On success [SEC_SUCCESS|N/A]
 * @retval SEC_INT On failure [Other than SEC_SUCCESS|N/A]

 * @par Required Header File
 * sec_crypto.h
 *
 * @par Note
 *  - Key conversion of only ed25119 key supported.
 * @par Related Topics
 * IPSI_RAND_FUNC
 */
CRYPTOLINKDLL SEC_INT IPSI_CRYPT_convertOsslKey(SEC_VOID *pkey, SEC_PKEY_S **pstKey);

#ifdef __cplusplus
}
#endif

#endif /* _IPSI_SEC_CRYPTO_H */
