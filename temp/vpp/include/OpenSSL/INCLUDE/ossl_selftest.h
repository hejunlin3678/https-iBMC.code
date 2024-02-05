/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: Contains selftest API
 * Project Code: VPP
 * Module Name: ossl_selftest.h
 *
 * Create: 2019-06-10
 * Author: Boga Sandeep
 * Descrption: Contains selftest API
*/

#ifndef OSSL_SELFTEST_H
#define OSSL_SELFTEST_H

#ifdef __cplusplus
extern "C" {
#endif

#define SELFTEST_TRUE 1
#define SELFTEST_FALSE 0

/**
 * Func Name:  OSSL_selftest_sha1
 * @defgroup OSSL_selftest_sha1
 * @par Prototype
 * @code
 * unsigned int OSSL_selftest_sha1(void)
 * @endcode
 *
 * @par Purpose
 * This is used to do the self test of the crypto algorithms.
 *
 * @par Description
 * This is used to do the self test of the crypto algorithms.
 * SHA-1 Algorithm
 * @param[in] void [N/A]
 *
 * @retval int If selftest of sha1 algorithm pass  [SELFTEST_TRUE|N/A]
 * @retval int If selftest of sha1 algorithm FAIL  [SELFTEST_FALSE|N/A]
 *
 * @par Required Header File
 * ossl_selftest.h
 *
 * @par Note
 * \n
 */
unsigned int OSSL_selftest_sha1(void);

/**
 * Func Name:  OSSL_selftest_sha2
 * @defgroup OSSL_selftest_sha2
 * @par Prototype
 * @code
 * unsigned int OSSL_selftest_sha2(void)
 * @endcode
 *
 * @par Purpose
 * This is used to do the self test of the crypto algorithms.
 *
 * @par Description
 * This is used to do the self test of the crypto algorithms.
 * SHA-2 Algorithm
 * @param[in] void [N/A]
 *
 * @retval int If selftest of sha2 algorithm pass  [SELFTEST_TRUE|N/A]
 * @retval int If selftest of sha2 algorithm FAIL  [SELFTEST_FALSE|N/A]
 *
 * @par Required Header File
 * ossl_selftest.h
 *
 * @par Note
 * \n
 */
unsigned int OSSL_selftest_sha2(void);


/**
 * Func Name:  OSSL_selftest_rsa
 * @defgroup OSSL_selftest_rsa
 * @par Prototype
 * @code
 * unsigned int OSSL_selftest_rsa(void)
 * @endcode
 *
 * @par Purpose
 * This is used to do the self test of the crypto algorithms.
 *
 * @par Description
 * This is used to do the self test of the crypto algorithms.
 * RSA Signature (2048/3072) known answer test
 * @param[in] void [N/A]
 *
 * @retval int If selftest of rsa algorithm pass  [SELFTEST_TRUE|N/A]
 * @retval int If selftest of rsa algorithm FAIL  [SELFTEST_FALSE|N/A]
 *
 * @par Required Header File
 * ossl_selftest.h
 *
 * @par Note
 * \n
 */
unsigned int OSSL_selftest_rsa(void);

/**
 * Func Name:  OSSL_selftest_dsa
 * @defgroup OSSL_selftest_dsa
 * @par Prototype
 * @code
 * unsigned int OSSL_selftest_dsa(void)
 * @endcode
 *
 * @par Purpose
 * This is used to do the self test of the crypto algorithms.
 *
 * @par Description
 * This is used to do the self test of the crypto algorithms.
 * DSA Signature (2048/3072) known answer test
 * @param[in] void [N/A]
 *
 * @retval int If selftest of dsa algorithm pass  [SELFTEST_TRUE|N/A]
 * @retval int If selftest of dsa algorithm FAIL  [SELFTEST_FALSE|N/A]
 *
 * @par Required Header File
 * ossl_selftest.h
 *
 * @par Note
 * \n
 */
unsigned int OSSL_selftest_dsa(void);

/**
 * Func Name:  OSSL_selftest_cmac
 * @defgroup OSSL_selftest_cmac
 * @par Prototype
 * @code
 * unsigned int OSSL_selftest_cmac(void)
 * @endcode
 *
 * @par Purpose
 * This is used to do the self test of the crypto algorithms.
 *
 * @par Description
 * This is used to do the self test of the crypto algorithms.
 * AES CMAC and Triple DES CMAC
 * @param[in] void [N/A]
 *
 * @retval int If selftest of CMAC algorithm pass  [SELFTEST_TRUE|N/A]
 * @retval int If selftest of CMAC algorithm FAIL  [SELFTEST_FALSE|N/A]
 *
 * @par Required Header File
 * ossl_selftest.h
 *
 * @par Note
 * \n
 */
unsigned int OSSL_selftest_cmac(void);


/**
 * Func Name:  OSSL_selftest_dh
 * @defgroup OSSL_selftest_dh
 * @par Prototype
 * @code
 * unsigned int OSSL_selftest_dh(void)
 * @endcode
 *
 * @par Purpose
 * This is used to do the self test of the crypto algorithms.
 *
 * @par Description
 * This is used to do the self test of the crypto algorithms.
 * DH group 2048bits
 * @param[in] void [N/A]
 *
 * @retval int If selftest of dh algorithm pass  [SELFTEST_TRUE|N/A]
 * @retval int If selftest of dh algorithm FAIL  [SELFTEST_FALSE|N/A]
 *
 * @par Required Header File
 * ossl_selftest.h
 *
 * @par Note
 * \n
 */
unsigned int OSSL_selftest_dh(void);


/**
 * Func Name:  OSSL_selftest_ecdh
 * @defgroup OSSL_selftest_ecdh
 * @par Prototype
 * @code
 * unsigned int OSSL_selftest_ecdh(void)
 * @endcode
 *
 * @par Purpose
 * This is used to do the self test of the crypto algorithms.
 *
 * @par Description
 * This is used to do the self test of the crypto algorithms.
 * ECDH (P256/P384)
 * @param[in] void [N/A]
 *
 * @retval int If selftest of ECDH algorithm pass  [SELFTEST_TRUE|N/A]
 * @retval int If selftest of ECDH algorithm FAIL  [SELFTEST_FALSE|N/A]
 *
 * @par Required Header File
 * ossl_selftest.h
 *
 * @par Note
 * \n
 */
unsigned int OSSL_selftest_ecdh(void);

/**
 * Func Name:  OSSL_selftest_ecdsa
 * @defgroup OSSL_selftest_ecdsa
 * @par Prototype
 * @code
 * unsigned int OSSL_selftest_ecdsa(void)
 * @endcode
 *
 * @par Purpose
 * This is used to do the self test of the crypto algorithms.
 *
 * @par Description
 * This is used to do the self test of the crypto algorithms.
 * ECDSA
 * @param[in] void [N/A]
 *
 * @retval int If selftest of ECDSA algorithm pass  [SELFTEST_TRUE|N/A]
 * @retval int If selftest of ECDSA algorithm FAIL  [SELFTEST_FALSE|N/A]
 *
 * @par Required Header File
 * ossl_selftest.h
 *
 * @par Note
 * \n
 */
unsigned int OSSL_selftest_ecdsa(void);

/**
 * Func Name:  OSSL_selftest_drbg
 * @defgroup OSSL_selftest_drbg
 * @par Prototype
 * @code
 * unsigned int OSSL_selftest_drbg(void)
 * @endcode
 *
 * @par Purpose
 * This is used to do the self test of the crypto algorithms.
 *
 * @par Description
 * This is used to do the self test of the crypto algorithms.
 * DRBG_CTR
 * @param[in] void [N/A]
 *
 * @retval int If selftest of DRBG_CTR algorithm pass  [SELFTEST_TRUE|N/A]
 * @retval int If selftest of DRBG_CTR algorithm FAIL  [SELFTEST_FALSE|N/A]
 *
 * @par Required Header File
 * ossl_selftest.h
 *
 * @par Note
 * \n
 */
unsigned int OSSL_selftest_drbg(void);

/**
 * Func Name:  OSSL_selftest_hmac
 * @defgroup OSSL_selftest_hmac
 * @par Prototype
 * @code
 * unsigned int OSSL_selftest_hmac(void)
 * @endcode
 *
 * @par Purpose
 * This is used to do the self test of the crypto algorithms.
 *
 * @par Description
 * This is used to do the self test of the crypto algorithms.
 * HMAC
 * @param[in] void [N/A]
 *
 * @retval int If selftest of HMAC algorithm pass  [SELFTEST_TRUE|N/A]
 * @retval int If selftest of HMAC algorithm FAIL  [SELFTEST_FALSE|N/A]
 *
 * @par Required Header File
 * ossl_selftest.h
 *
 * @par Note
 * \n
 */
unsigned int OSSL_selftest_hmac(void);

/**
 * Func Name:  OSSL_selftest_aes
 * @defgroup OSSL_selftest_aes
 * @par Prototype
 * @code
 * unsigned int OSSL_selftest_aes(void)
 * @endcode
 *
 * @par Purpose
 * This is used to do the self test of the crypto algorithms.
 *
 * @par Description
 * This is used to do the self test of the crypto algorithms.
 * AES 128/192/256 (ECB, CBC, CFB)
 * @param[in] void [N/A]
 *
 * @retval int If selftest of AES 128/192/256 (ECB, CBC, CFB) algorithm pass  [SELFTEST_TRUE|N/A]
 * @retval int If selftest of AES 128/192/256 (ECB, CBC, CFB) algorithm FAIL  [SELFTEST_FALSE|N/A]
 *
 * @par Required Header File
 * ossl_selftest.h
 *
 * @par Note
 * \n
 */
unsigned int OSSL_selftest_aes(void);

/**
 * Func Name:  OSSL_selftest_aes_gcm
 * @defgroup OSSL_selftest_aes_gcm
 * @par Prototype
 * @code
 * unsigned int OSSL_selftest_aes_gcm(void)
 * @endcode
 *
 * @par Purpose
 * This is used to do the self test of the crypto algorithms.
 *
 * @par Description
 * This is used to do the self test of the crypto algorithms.
 * AES 128/192/256 GCM
 * @param[in] void [N/A]
 *
 * @retval int If selftest of AES 128/192/256 GCM algorithm pass  [SELFTEST_TRUE|N/A]
 * @retval int If selftest of AES 128/192/256 GCM algorithm FAIL  [SELFTEST_FALSE|N/A]
 *
 * @par Required Header File
 * ossl_selftest.h
 *
 * @par Note
 * \n
 */
unsigned int OSSL_selftest_aes_gcm(void);


/**
 * Func Name:  OSSL_selftest_aes_ctr
 * @defgroup OSSL_selftest_aes_ctr
 * @par Prototype
 * @code
 * unsigned int OSSL_selftest_aes_ctr(void)
 * @endcode
 *
 * @par Purpose
 * This is used to do the self test of the crypto algorithms.
 *
 * @par Description
 * This is used to do the self test of the crypto algorithms.
 * AES 128/192/256 CTR
 * @param[in] void [N/A]
 *
 * @retval int If selftest of AES 128/192/256 CTR algorithm pass  [SELFTEST_TRUE|N/A]
 * @retval int If selftest of AES 128/192/256 CTR algorithm FAIL  [SELFTEST_FALSE|N/A]
 *
 * @par Required Header File
 * ossl_selftest.h
 *
 * @par Note
 * \n
 */
unsigned int OSSL_selftest_aes_ctr(void);


/**
 * Func Name:  OSSL_selftest_des
 * @defgroup OSSL_selftest_des
 * @par Prototype
 * @code
 * unsigned int OSSL_selftest_des(void)
 * @endcode
 *
 * @par Purpose
 * This is used to do the self test of the crypto algorithms.
 *
 * @par Description
 * This is used to do the self test of the crypto algorithms.
 * Triple DES (ECB, CBC, CFB)
 * @param[in] void [N/A]
 *
 * @retval int If selftest of Triple DES (ECB, CBC, CFB) algorithm pass  [SELFTEST_TRUE|N/A]
 * @retval int If selftest of Triple DES (ECB, CBC, CFB) algorithm FAIL  [SELFTEST_FALSE|N/A]
 *
 * @par Required Header File
 * ossl_selftest.h
 *
 * @par Note
 * \n
 */
unsigned int OSSL_selftest_des(void);

/**
 * Func Name:  OSSL_CryptAlgSelfTest
 * @defgroup OSSL_CryptAlgSelfTest
 * @par Prototype
 * @code
 * int OSSL_CryptAlgSelfTest(void)
 * @endcode
 *
 * @par Purpose
 * This is used to do the self test of the crypto algorithms.
 *
 * @par Description
 * This is used to do the self test of the crypto algorithms.
 * SHA-1 Algorithm
 * HMAC SHA-1, HMAC-SHA-2 (224, 256, 384, 512)
 * AES CMAC and Triple DES CMAC
 * AES 128/192/256 (ECB, CBC, CFB)
 * Triple DES (ECB, CBC, CFB)
 * AES 128/192/256 GCM
 * AES 128/192/256 CTR
 * RSA Signature (2048/3072) known answer test
 * DSA Signature (2048/3072) known answer test
 *
 * @param[in] void [N/A]
 *
 * @retval int If selftest of all the algorithm pass  [SELFTEST_TRUE|N/A]
 * @retval int If selftest of any algorithm FAIL  [SELFTEST_FALSE|N/A]
 *
 * @par Required Header File
 * ossl_selftest.h
 *
 * @par Note
 * \n
 */
int OSSL_CryptAlgSelfTest(void);

#ifdef __cplusplus
}
#endif /* end of __cplusplus */

#endif /* end of _OSSL_SELFTEST_H */
