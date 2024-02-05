/**
 * ****************************************************************************
 * Copyright (c) Huawei Technologies Co., Ltd. 2009-2019. All rights reserved.
 * ALL RIGHTS RESERVED
 * FileName              : sec_kdf.h
 * Version               : 1
 * Author                : Vinay Gudur
 * Creation Date         : 2009-02-26
 * Description           : This file contains interfaces of
 * Create: 2009-02-26
 * Key derivation functions.
 * Function List         : NA
 * History               :
 * <author>   <date>          <desc>
 * *****************************************************************************
 */
#ifndef SEC_KDF_H
#define SEC_KDF_H

#include "ipsi_crypto_buildconf.h"
#include "sec_crypto.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup sec_kdf
 * This section contains the sec_kdf Functions.
 */
/**
 * @defgroup sec_kdfFunctions
 * @ingroup sec_kdf
 * This section contains the sec_kdf Functions.
 */
/*
    Func Name:  dot16kdf_cmac
*/
/**
 * @defgroup dot16kdf_cmac
 * @ingroup sec_kdfFunctions
 * @par Prototype
 * @code
 * int dot16kdf_cmac(
 * unsigned char result[],
 * size_t rlen,
 * const unsigned char key[],
 * size_t klen,
 * const unsigned char msg[],
 * size_t mlen);
 * @endcode
 *
 * @par Purpose
 * This is used to derive a key using a shared secret and a password/passphrase.
 *
 * @par Description
 * dot16kdf_cmac function derives a key using a shared secret and a password/passphrase. This functions uses CMAC
 * (Cipher based message authentication algorithm) to derive a key.
 *
 * @param[in] rlen The key size to be generated in octets [N/A]
 * @param[in] key A non-empty pointer to original key [N/A]
 * @param[in] klen The original key size in octets. If CMAC uses AES128 as the block cipher, then it must be longer
 * than or equal 16 [N/A]
 * @param[in] msg A non-empty pointer to the message string [N/A]
 * @param[in] mlen The message size in octets [N/A]
 * @param[out] result A non-empty pointer to the generated key [N/A]
 *
 * @retval int On success [SEC_SUCCESS|N/A]
 * @retval int On failure [SEC_ERROR|N/A]
 *
 * @par Required Header File
 * sec_kdf.h
 *
 * @par Note
 * \n
 * 1. The memory for result output should be allocated by user based on the desired key length[rlen].
 *
 * @par Related Topic
 * N/A
 */
CRYPTOLINKDLL int dot16kdf_cmac(unsigned char result[], size_t rlen, const unsigned char key[], size_t klen,
    const unsigned char msg[], size_t mlen);

/*
    Func Name:  dot16kdf_sha1
*/
/**
 * @defgroup dot16kdf_sha1
 * @ingroup sec_kdfFunctions
 * @par Prototype
 * @code
 * int dot16kdf_sha1(
 * unsigned char result[],
 * size_t rlen,
 * const unsigned char key[],
 * size_t klen,
 * const unsigned char msg[],
 * size_t mlen);
 * @endcode
 *
 * @par Purpose
 * THis is used to derive a key using a shared secret and a password/passphrase.
 *
 * @par Description
 * dot16kdf_sha1 function derives a key using a shared secret and a password/passphrase. This functions uses SHA1
 * to derive a key.
 *
 * @param[in] rlen The key size to be generated in octets [N/A]
 * @param[in] key A non-empty pointer to original key [N/A]
 * @param[in] klen The original key size in octets, must be longer than or equal 20 [N/A]
 * @param[in] msg A non-empty pointer to the message string [N/A]
 * @param[in] mlen The message size in octets [N/A]
 * @param[out] result A non-empty pointer to the generated key [N/A]
 *
 * @retval int On success [SEC_SUCCESS|N/A]
 * @retval int On failure [SEC_ERROR|N/A]
 *
 * @par Required Header File
 * sec_kdf.h
 *
 * @par Note
 * \n
 * 1. The memory for result output should be allocated by user based on the desired key length[rlen].
 *
 * @par Related Topic
 * N/A
 */
CRYPTOLINKDLL int dot16kdf_sha1(unsigned char result[], size_t rlen, const unsigned char key[], size_t klen,
    const unsigned char msg[], size_t mlen);

/*
 * Func Name:  KDF2_SHA1
 */
/**
 * @defgroup KDF2_SHA1
 * @ingroup sec_kdfFunctions
 * @par Prototype
 * @code
 * int KDF2_SHA1(
 * unsigned char result[],
 * size_t rlen,
 * const unsigned char key[],
 * size_t klen,
 * const unsigned char msg[],
 * size_t mlen);
 * @endcode
 *
 * @par Purpose
 * This is a key derivation function to derive a key using a shared secret information.
 *
 * @par Description
 * KDF2_SHA1 is a key derivation function to derive a key using a shared secret information.
 *
 * @param[in] rlen Intended length in octets of the keying material. rlen shall be an integer, at most (2^32 - 1). [N/A]
 * @param[in] key The shared secret value, an octet string of (essentially) arbitrary length. [N/A]
 * @param[in] klen Shared secret value length. [N/A]
 * @param[in] msg Other information for key derivation, an octet string of (essentially) arbitrary length
 * (may be the empty string). [N/A]
 * @param[in] mlen Other information length. [N/A]
 * @param[out] result Key material, an octet string of length rlen. [N/A]
 *
 * @retval int On success [SEC_SUCCESS|N/A]
 * @retval int On failure [SEC_ERROR|N/A]
 *
 * @par Required Header File
 * sec_kdf.h
 *
 * @par Note
 * \n
 * 1. The memory for result output should be allocated by user based on the desired key length[rlen].
 *
 * @par Related Topic
 * N/A
 */
CRYPTOLINKDLL int KDF2_SHA1(unsigned char result[], size_t rlen, const unsigned char key[], size_t klen,
    const unsigned char msg[], size_t mlen);

/*
 * Func Name:  iPsi_KDF2
 */
/**
 * @defgroup iPsi_KDF2
 * @ingroup sec_kdfFunctions
 * @par Prototype
 * @code
 * int iPsi_KDF2(SEC_UINT32 ulKdfDigestAlg,
 * unsigned char result[],
 * size_t rlen,
 * const unsigned char key[],
 * size_t klen,
 * const unsigned char msg[],
 * size_t mlen);
 * @endcode
 *
 * @par Purpose
 * This is a key derivation function to derive a key using a shared secret information.
 *
 * @par Description
 * iPsi_KDF2 is a key derivation function to derive a key using a shared secret information.
 * Supported hash algorithms are SHA1, SHA224, SHA256, SHA384, SHA512 and SM3
 *
 * @param[in] ulKdfDigestAlg Hash algorithm
 * @param[in] rlen Intended length in octets of the keying material. rlen shall be an integer, at most (2^32 - 1). [N/A]
 * @param[in] key The shared secret value, an octet string of (essentially) arbitrary length. [N/A]
 * @param[in] klen Shared secret value length. [N/A]
 * @param[in] msg Other information for key derivation, an octet string of (essentially) arbitrary length
 * (may be the empty string). [N/A]
 * @param[in] mlen Other information length. [N/A]
 * @param[out] result Key material, an octet string of length rlen. [N/A]
 *
 * @retval int On success [SEC_SUCCESS|N/A]
 * @retval int On failure [SEC_ERROR|N/A]
 *
 * @par Required Header File
 * sec_kdf.h
 *
 * @par Note
 * \n
 * 1. The memory for result output should be allocated by user based on the desired key length[rlen].
 *
 * @par Related Topic
 * N/A
 */
CRYPTOLINKDLL SEC_INT iPsi_KDF2(SEC_UINT32 ulKdfDigestAlg, unsigned char result[], size_t rlen, const char key[],
    size_t klen, const unsigned char msg[], size_t mlen);

#if !(defined(__IPSI_NO_SEC_64BIT) || defined(__IPSI_NO_SCRYPT))

/*
 * Func Name:  ipsi_scrypt
 */
/**
 * @defgroup ipsi_scrypt
 * @ingroup sec_kdfFunctions
 * @par Prototype
 * @code
 * SEC_UINT32
 * ipsi_scrypt(
 * const SEC_UCHAR *pass,
 * size_t passlen,
 * const SEC_UCHAR *salt,
 * size_t saltlen,
 * SEC_UINT64 N,
 * SEC_UINT32 r,
 * SEC_UINT32 p,
 * SEC_UCHAR *buf,
 * size_t buflen);
 * @endcode
 *
 * @par Purpose
 * This is a password based key derviation function.
 *
 * @par Description
 * ipsi_scrypt implements scrypt which is a password-based key derivation function. Password-based
 * key derivation function can be used for two primary purposes. First, to hash passwords so that
 * an attacker who gains access to a password file does not immediately possess the passwords
 * contained therewithin; and second, to generate cryptographic keys to be used for encrypting
 * and/or authenticating data.
 *
 * Scrypt algorithm uses PBKDF2 with the pseudorandom function HMAC-SHA256 to generate p blocks of
 * length (128 * r) octets from the provided password and salt; these are independently mixed using
 * the mixing function; and the final output is then generated by applying PBKDF2 once again, using
 * the well-mixed blocks as salt.
 *
 * As per the scrypt paper, users of scrypt can tune the parameters N, r, and p according to the amount
 * of memory and computing power available, the latency-bandwidth product of the memory subsystem, and
 * the amount of parallelism desired; at the current time, taking r = 8 and p = 1 appears to yield good
 * results, but as memory latency and CPU parallelism increase it is likely that the optimum values for
 * both r and p will increase.
 *
 * For large N, mixing function take asymptotically longer time and the CPU might be hogged during call
 * to ipsi_scrypt. So it is adviced based on product scenario to set N such that call to ipsi_scrypt
 * provides result in acceptable time.
 *
 * @param[in] pass Passphrase, an octet string
 * @param[in] passlen Length of the password
 * @param[in] salt Salt, an octet string
 * @param[in] saltlen Length of salt
 * @param[in] N_ CPU / memory cost parameter. Must be power of 2. Must be larger than 1 and less
 * than 2^(128 * r / 8).
 * @param[in] r_ Block size parameter. Each block in the mixing function will be 2*r*64 octets
 * @param[in] p_ Parallelization parameter; a positive integer satisfying p <= (2^32 - 1)*32/(128*r)
 * i.e. r * p < 2^30
 * @param[out] result An octet string to hold the derived key
 * @param[in] rlen Intended output length in octets of the derived key; a positive integer satisfying
 * dkLen <= (2^32 - 1)*32
 *
 * @retval SEC_UINT32 If the desired derived key length is large [SEC_CRYPT_ERR_LARGE_DATA_SIZE|N/A]
 * @retval SEC_UINT32 If invalid parameters are passed [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 If parameters passed do not satisfy the above conditions. Or the parameters
 * passed are large and there is not enough memory [SEC_ERR|N/A]
 * @retval SEC_UINT32 On success [SEC_SUCCESS|N/A]
 *
 * @par Required Header File
 * sec_kdf.h
 *
 * @par Note
 * \n
 * 1. The memory for result output should be allocated by user based on the desired key length[rlen].
 *
 * @par Related Topic
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 ipsi_scrypt(const SEC_UCHAR *pass, size_t passlen, const SEC_UCHAR *salt, size_t saltlen,
    SEC_UINT64 N_, SEC_UINT32 r_, SEC_UINT32 p_, SEC_UCHAR *result, size_t rlen);

#endif

#ifndef IPSI_DISABLE_PBKDF
/*
 * Func Name:  PKCS5_deriveKey_PBES1
 */
/**
 * @defgroup pkcs5_deriveKey
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32
 * PKCS5_deriveKey_PBES1(SEC_UINT32 ulKDFAlg,
 * SEC_UCHAR *pucPassword, SEC_UINT32 ulPwdLen,
 * SEC_UCHAR *pucSalt, SEC_UINT32 ulSaltLen,
 * SEC_UCHAR *pucDerivedKey, SEC_UINT32 ulDKLen,
 * SEC_INT32 iITer);
 * @endcode
 *
 * @par Purpose
 * To generate the derived key for PKCS5 algorithms for PKCS5_PBES1 type.
 *
 * @par Description
 * This function generates the derived key for PKCS5 algorithms for PKCS5_PBES1_type.
 * The Key is derived using Password,Salt and Iteration
 * Count.Derived Key can be used to encrypt/decrypt a given message and
 * can also be used to generate MAC for a given message.
 *
 *
 * @param[in] ulKDFAlg For PKCS5_PBES1_TYPE, ulKDFAlg can be
 * MD2(not supported now), ALGID_MD5 and ALGID_SHA1
 * @param[in] pucPassword The password which is used as the base key
 * in generating the key [N/A]
 * @param[in] ulPwdLen Length of the password [N/A]
 * @param[in] pucSalt Salt which is a set of octets [N/A]
 * @param[in] ulSaltLen Length of the salt value [N/A]
 * @param[out] pucDerivedKey Generated derived key [N/A]
 * @param[in] ulDKLen Lenth of the derived Key [N/A]
 * @param[in] iITer,Iteration count value.If user gives value equal or smaller than 0,
 * the default value of 2048 will be taken [N/A]
 *
 * @retval SEC_UINT32 On successful generation of the derived key
 * [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 If one of the arguments passed is NULL or
 * invalid [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 If memory allocation fails [SEC_ERR_MALLOC_FAIL|N/A]
 * @retval SEC_UINT32 When key length is more than the specified length.
 * This error message is displayed
 * if the key length is more than 16 for ALGID_MD5
 * or if the key length is more than 20 for ALGID_SHA1[SEC_ERR_INVALID_KEY_LEN|N/A]
 * Derived Key length of zero value is not allowed    *
 * @retval SEC_UINT32 When context values are not correct
 * [SEC_CRYPT_ERR_INVALID_CTX|N/A]
 *
 * @par Required Header File
 * sec_kdf.h
 *
 * @par Note
 *
 * 1. for ALGID_MD5, the DKLEN should not more than 16. for ALGID_SHA1, the DKLEN should not more than 20
 *
 * @par Related Topic
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 PKCS5_deriveKey_PBES1(SEC_UINT32 ulKDFAlg, SEC_UCHAR *pucPassword, SEC_UINT32 ulPwdLen,
    SEC_UCHAR *pucSalt, SEC_UINT32 ulSaltLen, SEC_UCHAR *pucDerivedKey, SEC_UINT32 ulDKLen, SEC_INT32 iITer);

/**
 * Func Name:  PKCS5_deriveKey_PBES2
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 PKCS5_deriveKey_PBES2_Ex(SEC_UINT32 ulKDFAlg,
 * SEC_UCHAR *pucPassword, SEC_UINT32 ulPwdLen,
 * SEC_UCHAR *pucSalt, SEC_UINT32 ulSaltLen,
 * SEC_UINT32 ulDKLen, SEC_INT32 iITer,
 * SEC_UCHAR *pucDerivedKey);
 * @endcode
 *
 * @par Purpose
 * To generate the derived key for PKCS5 algorithms for PKCS5_PBES2 type.
 *
 * @par Description
 * This function generates the derived key for PKCS5 algorithms for PKCS5_PBES1_type.
 * The Key is derived using Password,Salt and Iteration
 * Count.Derived Key can be used to encrypt/decrypt a given message and
 * can also be used to generate MAC for a given message.
 *
 * @param[in] ulKDFAlg For PKCS5_PBES2_TYPE,default value of ulKDFAlg is ALGID_HMAC_SHA1.
 * Presently it can be ALGID_HMAC_SHA1 or ALGID_HMAC_MD5 or ALGID_HMAC_SHA224 or ALGID_HMAC_SHA256
 * or ALGID_HMAC_SHA384 or ALGID_HMAC_SHA512[N/A]
 * @param[in] pucPassword The password which is used as the base key  in generating the key [N/A]
 * @param[in] ulPwdLen Length of the password [N/A]
 * @param[in] pucSalt Salt which is a set of octets [N/A]
 * @param[in] ulSaltLen Length of the salt value [N/A]
 * @param[in] ulDKLen Lenth of the derived Key [N/A]
 * @param[in] iITer,Iteration count value.If user gives value equal or smaller than 0, the default value of 2048 will be
 * taken [N/A]
 * @param[out] pucDerivedKey Generated derived key [N/A]
 *
 * @retval SEC_UINT32 On successful generation of the derived key
 * [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 If one of the arguments passed is NULL or
 * invalid [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 If memory allocation fails [SEC_ERR_MALLOC_FAIL|N/A]
 * @retval SEC_UINT32 When key length is more than the specified length.
 * This error message is displayed
 * if Key Length is larger than 65535 for PBES2[SEC_ERR_INVALID_KEY_LEN|N/A]
 * Derived Key length of zero value is not allowed    *
 * @retval SEC_UINT32 When context values are not correct
 * [SEC_CRYPT_ERR_INVALID_CTX|N/A]
 *
 * @par Required Header File
 * sec_kdf.h
 *
 * @par Note
 *
 * 1. DKLen should not bigger than 65535, passlen & saltLen should not bigger than 1048576
 *
 * @par Related Topic
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 PKCS5_deriveKey_PBES2(SEC_UINT32 ulKDFAlg, SEC_UCHAR *pucPassword, SEC_UINT32 ulPwdLen,
    SEC_UCHAR *pucSalt, SEC_UINT32 ulSaltLen, SEC_UINT32 ulDKLen, SEC_INT32 iIter, SEC_UCHAR *pucDerivedKey);

#endif // #ifndef IPSI_DISABLE_PBKDF

#ifdef __cplusplus
}
#endif
#endif
