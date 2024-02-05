/*

 * Copyright (c) Huawei Technologies Co., Ltd. 2005-2019. All rights reserved.
 * Description: Contains various crypto interface functions
                            ALL RIGHTS RESERVED

  Project Code: iPSI
  Module Name: sec_crypto.h

 * Create:  2005-07-13
  Author: Jimmy Bahuleyan, Sanjay Vasudevan
  Descrption: Contains various crypto interface functions
----------------------------------------------------------------------
  Modification History
  DATE        NAME             DESCRIPTION
  --------------------------------------------------------------------
  2006-07-31   Siva Kumar.K    Added a new interface and ID's to enable
                               SSX31B HW support
  2007-08-30   Vinay Gudur   Added new Algorithms - CMAC and AES-Key Wrap.
*/
#ifndef SEC_CRYPTO_ALG_ID_H
#define SEC_CRYPTO_ALG_ID_H

#ifdef __cplusplus
extern "C" {
#endif

/* TEST callback operation value: */
/* All tests started */
#define IPSI_FIPS_CRYPTO_SELF_TEST_BEGIN 1

/* All tests end: result in id */
#define IPSI_FIPS_CRYPTO_SELF_TEST_END 2

/* One individual test started */
#define IPSI_CRYPTO_SELF_TEST_STARTED 3

/* Individual test success */
#define IPSI_CRYPTO_SELF_TEST_SUCCESS 4

/* Individual test failure */
#define IPSI_CRYPTO_SELF_TEST_FAIL 5

/* Induce failure in test if return zero */
#define IPSI_CRYPTO_SELF_TEST_INDUCE_FAIL 6

#define ALGID_UNKNOWN 0

/* identifies the RC4 algorithm */
#define ALGID_RC4 CID_RC4

/* The following Symmetric Block algorithms belong to the four modes
    ECB - Electronic Code Book
    CBC - Cipher Block Chaining
    OFB - Output FeedBack
    CFB - Cipher FeedBack
 */
/* identifies DES algorithm in ECB mode */
#define ALGID_DES_ECB CID_DES_ECB

/* identifies DES algorithm in CBC mode */
#define ALGID_DES_CBC CID_DES_CBC

/* identifies DES algorithm in OFB mode */
#define ALGID_DES_OFB CID_DES_OFB

/* identifies DES algorithm in CFB mode */
#define ALGID_DES_CFB CID_DES_CFB

/* ID's for supporting SSX31B HW Now in crypto
scb2 algo is not supported.All scb2 symbols we keep because these
symbols are used in pse and ssl */
/* identifies SCB2-128 algorithm in ECB mode */
#define ALGID_SCB2_128_ECB CID_SCB2_128_ECB

/* identifies SCB2-128 algorithm in CBC mode */
#define ALGID_SCB2_128_CBC CID_SCB2_128_CBC

/* identifies SCB2-256 algorithm in ECB mode */
#define ALGID_SCB2_256_ECB CID_SCB2_256_ECB

/* identifies SCB2-256 algorithm in CBC mode */
#define ALGID_SCB2_256_CBC CID_SCB2_256_CBC

/* identifies 2 key triple DES algorithm in ECB mode */
#define ALGID_DES_EDE_ECB CID_DES_EDE_ECB

/* identifies 2 key triple DES algorithm in CBC mode */
#define ALGID_DES_EDE_CBC CID_DES_EDE_CBC

/* identifies 2 key triple DES algorithm in OFB mode */
#define ALGID_DES_EDE_OFB CID_DES_EDE_OFB

/* identifies 2 key triple DES algorithm in CFB mode */
#define ALGID_DES_EDE_CFB CID_DES_EDE_CFB

/* identifies 3 key triple DES algorithm in ECB mode */
#define ALGID_DES_EDE3_ECB CID_DES_EDE3_ECB

/* identifies 3 key triple DES algorithm in CBC mode */
#define ALGID_DES_EDE3_CBC CID_DES_EDE3_CBC

/* identifies 3 key triple DES algorithm in OFB mode */
#define ALGID_DES_EDE3_OFB CID_DES_EDE3_OFB

/* identifies 3 key triple DES algorithm in CFB mode */
#define ALGID_DES_EDE3_CFB CID_DES_EDE3_CFB

/* identifies AES-128 algorithm in ECB mode */
#define ALGID_AES128_ECB CID_AES128_ECB

/* identifies AES-128 algorithm in CBC mode */
#define ALGID_AES128_CBC CID_AES128_CBC

/* identifies AES-128 algorithm in OFB mode */
#define ALGID_AES128_OFB CID_AES128_OFB

/* identifies AES-128 algorithm in CFB mode */
#define ALGID_AES128_CFB CID_AES128_CFB

/* identifies AES-192 algorithm in ECB mode */
#define ALGID_AES192_ECB CID_AES192_ECB

/* identifies AES-192 algorithm in CBC mode */
#define ALGID_AES192_CBC CID_AES192_CBC

/* identifies AES-192 algorithm in OFB mode */
#define ALGID_AES192_OFB CID_AES192_OFB

/* identifies AES-192 algorithm in CFB mode */
#define ALGID_AES192_CFB CID_AES192_CFB

/* identifies AES-256 algorithm in ECB mode */
#define ALGID_AES256_ECB CID_AES256_ECB

/* identifies AES-256 algorithm in CBC mode */
#define ALGID_AES256_CBC CID_AES256_CBC

/* identifies AES-256 algorithm in OFB mode */
#define ALGID_AES256_OFB CID_AES256_OFB

/* identifies AES-256 algorithm in CFB mode */
#define ALGID_AES256_CFB CID_AES256_CFB

/* identifies Kasumi algorithm in ECB mode */
#define ALGID_KASUMI_ECB CID_KASUMI_ECB

/* identifies Kasumi algorithm in CBC mode */
#define ALGID_KASUMI_CBC CID_KASUMI_CBC

/* identifies Kasumi algorithm in CFB mode */
#define ALGID_KASUMI_CFB CID_KASUMI_CFB

/* identifies Kasumi algorithm in OFB mode */
#define ALGID_KASUMI_OFB CID_KASUMI_OFB

/* identifies RC2 algorithm in ECB mode */
#define ALGID_RC2_ECB CID_RC2_ECB

/* identifies RC2 algorithm in CBC mode */
#define ALGID_RC2_CBC CID_RC2_CBC

/* identifies RC2 algorithm in OFB mode */
#define ALGID_RC2_OFB CID_RC2_OFB

/* identifies RC2 algorithm in CFB mode */
#define ALGID_RC2_CFB CID_RC2_CFB

/* identifies Blowfish algorithm in ECB mode */
#define ALGID_BF_ECB CID_BF_ECB

/* identifies Blowfish algorithm in CBC mode */
#define ALGID_BF_CBC CID_BF_CBC

/* identifies Blowfish algorithm in CFB mode */
#define ALGID_BF_CFB CID_BF_CFB

/* identifies Blowfish algorithm in OFB mode */
#define ALGID_BF_OFB CID_BF_OFB

#define LAST_SYM_ALG CID_RC2_CFB

/* identifies SM4 algorithm , do not use to encrypt/decrypt */
#define ALGID_SM4 CID_SM4

/* identifies SM4 algorithm in ECB mode */
#define ALGID_SM4_ECB CID_SM4_ECB

/* identifies SM4 algorithm in CBC mode */
#define ALGID_SM4_CBC CID_SM4_CBC
// to ensure if new sym algorithms are added after RC2 then
// g_AlgTable array and ipsi_algid_to_g_AlgTable_idx should be modified
/* identifies the RSA algorithm */
#define ALGID_RSA CID_RSA

#define ALGID_DSA CID_DSA                 /* identifies the DSA algorithm */
#define ALGID_ECDSA CID_ECDSA             /* identifies the ECDSA algorithm */
#define ALGID_ED25519 CID_ED25519         /* identifies the ECDSA algorithm */
#define ALGID_X25519 CID_X25519           /* identifies the X25519 algorithm */
#define ALGID_DH CID_DH                   /* identifies the Diffie-Hellman algorithm */
#define ALGID_ECDH CID_ECDH               /* identifies the EC Diffie-Hellman algorithm */
#define ALGID_MD4 CID_MD4                 /* identifies MD4 hash algorithm */
#define ALGID_MD5 CID_MD5                 /* identifies the MD5 hash algorithm */
#define ALGID_SHA1 CID_SHA1               /* identifies the SHA1 hash algorithm */
#define ALGID_SHA224 CID_SHA224           /* identifies the SHA224 hash algorithm */
#define ALGID_SHA256 CID_SHA256           /* identifies the SHA256 hash algorithm */
#define ALGID_SHA384 CID_SHA384           /* identifies the SHA384 hash algorithm */
#define ALGID_SHA512 CID_SHA512           /* identifies the SHA512 hash algorithm */
#define ALGID_HMAC_MD4 CID_HMAC_MD4       /* identifies hmac with MD4 */
#define ALGID_HMAC_MD5 CID_HMAC_MD5       /* identifies hmac with MD5 */
#define ALGID_HMAC_SHA1 CID_HMAC_SHA1     /* identifies hmac with SHA1 */
#define ALGID_HMAC_SHA224 CID_HMAC_SHA224 /* identifies hmac with SHA224 */
#define ALGID_HMAC_SHA256 CID_HMAC_SHA256 /* identifies hmac with SHA256 */
#define ALGID_HMAC_SHA384 CID_HMAC_SHA384 /* identifies hmac with SHA384 */
#define ALGID_HMAC_SHA512 CID_HMAC_SHA512 /* identifies hmac with SHA512 */
#define ALGID_CMAC_AES CID_CMAC_AES       /* identifies CMAC-AES */
#define ALGID_XCBC_AES CID_XCBC_AES       /* identifies XCBC-MAC-AES */
#define ALGID_CMAC_TDES CID_CMAC_TDES     /* identifies CMAC-Triple DES */
#define ALGID_CMAC_SM4 CID_CMAC_SM4       /* identifies CMAC SM4 */
#define ALGID_RNG_HW CID_RNG_HW           /* identifies TRNG */
#define ALGID_RNG_SW CID_RNG_SW           /* identifies PRNG */
/* Following ALGIDs are used by the SSL library */
#define ALGID_MD5WITHRSA CID_MD5WITHRSA         /* identifies signature using MD5 and RSA */
#define ALGID_SHA1WITHRSA CID_SHA1WITHRSA       /* identifies signature using SHA1 and RSA */
#define ALGID_SHA1WITHRSAOLD CID_SHA1WITHRSAOLD /* identifies signature using SHA1 and RSA (coresponds to old Oid) */
#define ALGID_DSA_WITH_SHA1 CID_DSAWITHSHA1     /* identifies signature using SHA1 and DSA */
#define ALGID_DSAWITHSHA1_2 CID_DSAWITHSHA1_2   /* identifies signature using SHA1 and DSA */
#define ALGID_ECDSA_WITH_SHA1 CID_ECDSAWITHSHA1 /* identifies signature using SHA1 and ECDSA */
#define ALGID_ECDSA_WITH_SHA224 CID_ECDSAWITHSHA224               /* identifies signature using SHA224 and ECDSA */
#define ALGID_ECDSA_WITH_SHA256 CID_ECDSAWITHSHA256               /* identifies signature using SHA256 and ECDSA */
#define ALGID_ECDSA_WITH_SHA384 CID_ECDSAWITHSHA384               /* identifies signature using SHA384 and ECDSA */
#define ALGID_ECDSA_WITH_SHA512 CID_ECDSAWITHSHA512               /* identifies signature using SHA512 and ECDSA */
#define ALGID_ECDSA192_WITH_SHA256 CID_ECDSA192WITHSHA256         /* identifies signature using SHA1 and ECDSA192 */
#define ALGID_SHA256WITHRSAENCRYPTION CID_SHA256WITHRSAENCRYPTION /* identifies signature using SHA256 and RSA */
#define ALGID_SHA384WITHRSAENCRYPTION CID_SHA384WITHRSAENCRYPTION /* identifies signature using SHA384 and RSA */
#define ALGID_SHA512WITHRSAENCRYPTION CID_SHA512WITHRSAENCRYPTION /* identifies signature using SHA512 and RSA */
#define ALGID_SM3WITHRSAENCRYPTION CID_SM3WITHRSAENCRYPTION /* identifies signature using SM3 and RSA */
#define ALGID_MD5_SHA1 CID_MD5_SHA1
#define ALGID_RSASSAPSS CID_RSASSAPSS /* identifies signature using RSAPSS scheme */
#define ALGID_PBKDF2 CID_PBES2
#define ALGID_SCRYPT CID_SCRYPT
#define ALGID_PBES1 CID_PBES1
#define ALGID_KDF2 CID_KDF2
#define ALGID_DOT16KDF CID_DOT16KDF

/* identifies special case in which hash info is not considered in sign
and verification */
#define ALGID_SM3 CID_SM3                             /* identifies the SM3 hash algorithm */
#define ALGID_HMAC_SM3 CID_HMAC_SM3                   /* identifies hmac with SM3 */
#define ALGID_SM2DSA CID_SM2DSA                       /* Identifies SM2 Digital Signature Algorithm */
#define ALGID_SM2DSA_WITH_SM3 CID_SM2DSAWITHSM3       /* Identifies signature using SM2DSA with SM3 */
#define ALGID_SM2DSA_WITH_SHA1 CID_SM2DSAWITHSHA1     /* Identifies signature using SM2DSA with SHA1 */
#define ALGID_SM2DSA_WITH_SHA256 CID_SM2DSAWITHSHA256 /* Identifies signature using SM2DSA with SHA256 */
#define ALGID_SM2KEP CID_SM2KEP                       /* Identifies SM2 Key Exchange Protocol algorithm */
#define ALGID_SM2PKEA CID_SM2PKEA                     /* Identifies SM2 Public Key Encryption Algorithm */
#define ALGID_ECKEY ALGID_ECDSA

/* Identifies the EC Key, which can be used with various ECC algorithms */
#define ALGID_AES128_GCM CID_AES128_GCM
#define ALGID_AES192_GCM CID_AES192_GCM
#define ALGID_AES256_GCM CID_AES256_GCM

#define ALGID_AES128_CTR CID_AES128_CTR
#define ALGID_AES192_CTR CID_AES192_CTR
#define ALGID_AES256_CTR CID_AES256_CTR

#define ALGID_AES128_CCM CID_AES128_CCM
#define ALGID_AES192_CCM CID_AES192_CCM
#define ALGID_AES256_CCM CID_AES256_CCM

/* Identifies the Key Wrap/Unwrap algorithm id */
#define ALGID_KWRAP_AES CID_KWRAP_AES
#define ALGID_KWRAP_SM4 CID_KWRAP_SM4

#define CRYPT_MASK_PAD(x) ((SEC_UINT32)((x) & 0xFFFF))

/* Macro to mask out the pad info to get alg_id */
#define CRYPT_SET_PAD_MODE(a, m) ((a) |= (((SEC_UINT32)(m)) << 16))

/* Macros to get & set the pad mode of a symmetric algorithm
 the parameter 'a' must be an l-value */
#define CRYPT_RESET_PAD_MODE(a) ((a) &= 0x0000ffff)

/* Macro to reset padding mode */
#define CRYPT_GET_PAD_MODE(a) (((SEC_UINT32)(a)) >> 16)

/* Macro to get padding mode from alg */
/* Specifies the result of a comparison operation */
#define SEC_CRYPT_MATCH 0UL

/* compared values match when comparison function returns SEC_CRYPT_MATCH */
#define SEC_CRYPT_NO_MATCH 1UL

/* compared values do not match when comparison function
returns SEC_CRYPT_MATCH */
/* The maximum size of a digest */
#define MAX_DIGEST_SIZE 64UL

/* Maximum possible size of a digest, same as size of SHA512 digest */
#define MAX_KEY_LENGTH 32UL

/* The maximum size of a symmetric alg key */
#define MAX_BLOCK_LENGTH 16UL

/* The maximum size of a symmetric alg block */
#define MAX_IV_LENGTH MAX_BLOCK_LENGTH

/* The maximum size of a symmetric alg IV */
/* Operating modes for symmetric algorithm */
#define SYMALG_STREAM_CIPHER 0x0UL
#define SYMALG_ECB_MODE 0x1UL
#define SYMALG_CBC_MODE 0x2UL
#define SYMALG_CFB_MODE 0x3UL
#define SYMALG_OFB_MODE 0x4UL
#define INVALID_SYMALG_MODE 0x5UL

#define SYMALG_GCM_MODE 0x6UL
#define SYMALG_CTR_MODE 0x7UL
#define SYMALG_CCM_MODE 0x8UL

#ifdef __cplusplus
}
#endif

#endif /* SEC_CRYPTO_ALG_ID_H */
