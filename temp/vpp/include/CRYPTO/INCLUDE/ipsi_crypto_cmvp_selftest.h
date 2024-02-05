/**
 * ***************************************************************************
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description:
          提供ISO19790自测试对外接口
 * File Name: ipsi_crypto_cmvp_selftest.h
 * Create: 2022-05-22
 * Author:
 *****************************************************************************
 */
#ifndef IPSI_CRYPTO_CMVP_SELFTEST_H
#define IPSI_CRYPTO_CMVP_SELFTEST_H

#include "sec_sys.h"
#include "ipsi_cmvp_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
Func Name:  IPSI_CRYPTO_CMVP_SelftestAes
*/
/**
 * @defgroup CMVP_Self_Test_Function CMVP Self Test Function
 * This provides information for CMVP self test Function.
 */
/**
* @defgroup IPSI_CRYPTO_CMVP_SelftestAes
* @ingroup CMVP_Self_Test_Function
* @par Prototype
* @code
* SEC_INT32 IPSI_CRYPTO_CMVP_SelftestAes()
* @endcode
*
* @par Purpose
* This function runs self tests for all CMVP supported algorithms.
*
* @par Description
* IPSI_CRYPTO_CMVP_SelftestAes API is used to run self tests for all the CMVP supported algorithms in a single function
* call. This API can be used at Power On or during initialization of the CRYPTO library. This API can be used for
*  Power On Self Tests(POST).
* - The following self tests will be performed:
*	- AES (ECB, CBC, CFB)
*	- AES GCM
*	- AES CTR
*	- AES CCM
*	- SHA-1
*	- SHA-224
*	- SHA-256
*	- SHA-384
*	- SHA-512
*	- HMAC SHA-1, HMAC-SHA-2 (224, 256, 384, 512)
*	- ECDSA
*	- DSA
*	- RSA
*	- DH
*	- ECDH
*	- X25519
*	- SCRYPT
*	- PBKDF2
*	- Hash-DRBG, HMAC-DRBG, CTR-DRBG (AES)
* .

*
* @par Parameters
* N/A
*
* @retval SEC_INT32 (Status Output) On successful return. [SEC_SUCCESS|N/A]
* @retval SEC_INT32 (Status Output) On failure. [SEC_ERROR|N/A]
*
* @par Required Header File
* ipsi_crypto_cmvp_selftest.h
*
* @par Note
*    - If the selftest fails then the application must not call any API which provides CMVP approved
*       algorithm functionality.
*    - If the CMVP crypto module fails and the IPSI_CRYPTO_CMVP_SelftestAes API
*       is called then this API returns SEC_ERROR.
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_INT32 IPSI_CRYPTO_CMVP_Selftest(SEC_VOID);

/*
Func Name:  IPSI_CRYPTO_CMVP_SelftestAes
*/
/**
 * @defgroup IPSI_CRYPTO_CMVP_SelftestAes
 * @ingroup CMVP_Self_Test_Function
 * @par Prototype
 * @code
 * SEC_INT32 IPSI_CRYPTO_CMVP_SelftestAes()
 * @endcode
 *
 * @par Purpose
 * This function is used to run AES self tests.
 *
 * @par Description
 * IPSI_CRYPTO_CMVP_SelftestAes is used to run AES self tests in the CPP mode, hence this function can be used
 * to run all the AES self known answer tests supported.
 * This API can also be called during Power On or during initialization of the CRYPTO library and hence can be used for
 * Power On Self Tests[POST].
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_INT32 (Status Output) On successful return. [SEC_SUCCESS|N/A]
 * @retval SEC_INT32 (Status Output) On failure. [SEC_ERROR|N/A]
 *
 * @par Required Header File
 * ipsi_crypto_cmvp_selftest.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for CMVP/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT32 IPSI_CRYPTO_CMVP_SelftestAes(SEC_VOID);

/*
Func Name:  IPSI_CRYPTO_CMVP_SelftestSha1
*/
/**
 * @defgroup IPSI_CRYPTO_CMVP_SelftestSha1
 * @ingroup CMVP_Self_Test_Function
 * @par Prototype
 * @code
 * SEC_INT32 IPSI_CRYPTO_CMVP_SelftestSha1()
 * @endcode
 *
 * @par Purpose
 * This function is used to run SHA1 self tests.
 *
 * @par Description
 * IPSI_CRYPTO_CMVP_SelftestSha1 is used to run SHA1 self tests in the CPP mode, hence this function can be used
 * to run all SHA1 supported self Known Answer Tests supported.
 * This API can also be called during Power On or during initialization of the CRYPTO library and hence can be used for
 * Power On Self tests[POST].
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_INT32 (Status Output) On successful return [SEC_SUCCESS|N/A]
 * @retval SEC_INT32 (Status Output) On failure [SEC_ERROR|N/A]
 *
 * @par Required Header File
 * ipsi_crypto_cmvp_selftest.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for CMVP/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT32 IPSI_CRYPTO_CMVP_SelftestSha1(SEC_VOID);

/*
Func Name:  IPSI_CRYPTO_CMVP_SelftestSha224
*/
/**
 * @defgroup IPSI_CRYPTO_CMVP_SelftestSha224
 * @ingroup CMVP_Self_Test_Function
 * @par Prototype
 * @code
 * SEC_INT32 IPSI_CRYPTO_CMVP_SelftestSha224()
 * @endcode
 *
 * @par Purpose
 * This function is used to run SHA224 self tests.
 *
 * @par Description
 * IPSI_CRYPTO_CMVP_SelftestSha224 is used to run SHA224 self tests in the CPP mode, hence this function can be used
 * to run all SHA224 supported self Known Answer Tests supported.
 * This API can also be called during Power On or during initialization of the CRYPTO library and hence can be used for
 * Power On Self tests[POST].
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_INT32 (Status Output) On successful return [SEC_SUCCESS|N/A]
 * @retval SEC_INT32 (Status Output) On failure [SEC_ERROR|N/A]
 *
 * @par Required Header File
 * ipsi_crypto_cmvp_selftest.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for CMVP/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT32 IPSI_CRYPTO_CMVP_SelftestSha224(SEC_VOID);

/*
Func Name:  IPSI_CRYPTO_CMVP_SelftestSha256
*/
/**
 * @defgroup IPSI_CRYPTO_CMVP_SelftestSha256
 * @ingroup CMVP_Self_Test_Function
 * @par Prototype
 * @code
 * SEC_INT32 IPSI_CRYPTO_CMVP_SelftestSha256()
 * @endcode
 *
 * @par Purpose
 * This function is used to run SHA256 self tests.
 *
 * @par Description
 * IPSI_CRYPTO_CMVP_SelftestSha256 is used to run SHA256 self tests in the CPP mode, hence this function can be used
 * to run all SHA256 supported self Known Answer Tests supported.
 * This API can also be called during Power On or during initialization of the CRYPTO library and hence can be used for
 * Power On Self tests[POST].
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_INT32 (Status Output) On successful return [SEC_SUCCESS|N/A]
 * @retval SEC_INT32 (Status Output) On failure [SEC_ERROR|N/A]
 *
 * @par Required Header File
 * ipsi_crypto_cmvp_selftest.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for CMVP/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT32 IPSI_CRYPTO_CMVP_SelftestSha256(SEC_VOID);

/*
Func Name:  IPSI_CRYPTO_CMVP_SelftestSha384
*/
/**
 * @defgroup IPSI_CRYPTO_CMVP_SelftestSha384
 * @ingroup CMVP_Self_Test_Function
 * @par Prototype
 * @code
 * SEC_INT32 IPSI_CRYPTO_CMVP_SelftestSha384()
 * @endcode
 *
 * @par Purpose
 * This function is used to run SHA384 self tests.
 *
 * @par Description
 * IPSI_CRYPTO_CMVP_SelftestSha384 is used to run SHA384 self tests in the CPP mode, hence this function can be used
 * to run all SHA384 supported self Known Answer Tests supported.
 * This API can also be called during Power On or during initialization of the CRYPTO library and hence can be used for
 * Power On Self tests[POST].
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_INT32 (Status Output) On successful return [SEC_SUCCESS|N/A]
 * @retval SEC_INT32 (Status Output) On failure [SEC_ERROR|N/A]
 *
 * @par Required Header File
 * ipsi_crypto_cmvp_selftest.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for CMVP/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT32 IPSI_CRYPTO_CMVP_SelftestSha384(SEC_VOID);

/*
Func Name:  IPSI_CRYPTO_CMVP_SelftestSha512
*/
/**
 * @defgroup IPSI_CRYPTO_CMVP_SelftestSha512
 * @ingroup CMVP_Self_Test_Function
 * @par Prototype
 * @code
 * SEC_INT32 IPSI_CRYPTO_CMVP_SelftestSha512()
 * @endcode
 *
 * @par Purpose
 * This function is used to run SHA512 self tests.
 *
 * @par Description
 * IPSI_CRYPTO_CMVP_SelftestSha512 is used to run SHA512 self tests in the CPP mode, hence this function can be used
 * to run all SHA512 supported self Known Answer Tests supported.
 * This API can also be called during Power On or during initialization of the CRYPTO library and hence can be used for
 * Power On Self tests[POST].
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_INT32 (Status Output) On successful return [SEC_SUCCESS|N/A]
 * @retval SEC_INT32 (Status Output) On failure [SEC_ERROR|N/A]
 *
 * @par Required Header File
 * ipsi_crypto_cmvp_selftest.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for CMVP/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT32 IPSI_CRYPTO_CMVP_SelftestSha512(SEC_VOID);

/*
Func Name:  IPSI_CRYPTO_CMVP_SelftestRsa
*/
/**
 * @defgroup IPSI_CRYPTO_CMVP_SelftestRsa
 * @ingroup CMVP_Self_Test_Function
 * @par Prototype
 * @code
 * SEC_INT32 IPSI_CRYPTO_CMVP_SelftestRsa()
 * @endcode
 *
 * @par Purpose
 * This function is used to run RSA signature self tests.
 *
 * @par Description
 * IPSI_CRYPTO_CMVP_SelftestRsa is used to run RSA signature self tests in the CPP mode.
 * This API can also be called during Power On or during initialization of the CRYPTO library and hence can be used for
 * Power On Self tests[POST].
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_INT32 (Status Output) On successful return [SEC_SUCCESS|N/A]
 * @retval SEC_INT32 (Status Output) On failure [SEC_ERROR|N/A]
 *
 * @par Required Header File
 * ipsi_crypto_cmvp_selftest.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for CMVP/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT32 IPSI_CRYPTO_CMVP_SelftestRsa(SEC_VOID);

/*
Func Name:  IPSI_CRYPTO_CMVP_SelftestDsa
*/
/**
 * @defgroup IPSI_CRYPTO_CMVP_SelftestDsa
 * @ingroup CMVP_Self_Test_Function
 * @par Prototype
 * @code
 * SEC_INT32 IPSI_CRYPTO_CMVP_SelftestDsa()
 * @endcode
 *
 * @par Purpose
 * This function is used to run DSA signature self tests.
 *
 * @par Description
 * IPSI_CRYPTO_CMVP_SelftestDsa is used to run DSA signature self tests in the CPP mode.
 * This API can also be called during Power On or during initialization of the CRYPTO library and hence can be used for
 * Power On Self tests[POST].
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_INT32 (Status Output) On successful return [SEC_SUCCESS|N/A]
 * @retval SEC_INT32 (Status Output) On failure [SEC_ERROR|N/A]
 *
 * @par Required Header File
 * ipsi_crypto_cmvp_selftest.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for CMVP/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT32 IPSI_CRYPTO_CMVP_SelftestDsa(SEC_VOID);

/*
Func Name:  IPSI_CRYPTO_CMVP_SelftestEcdsa
*/
/**
 * @defgroup IPSI_CRYPTO_CMVP_SelftestEcdsa
 * @ingroup CMVP_Self_Test_Function
 * @par Prototype
 * @code
 * SEC_INT32 IPSI_CRYPTO_CMVP_SelftestEcdsa()
 * @endcode
 *
 * @par Purpose
 * This function is used to run ECDSA signature self tests.
 *
 * @par Description
 * IPSI_CRYPTO_CMVP_SelftestEcdsa is used to run ECDSA signature self tests in the CPP mode.
 * This API can also be called during Power On or during initialization of the CRYPTO library and hence can be used for
 * Power On Self tests[POST].
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_INT32 (Status Output) On successful return [SEC_SUCCESS|N/A]
 * @retval SEC_INT32 (Status Output) On failure [SEC_ERROR|N/A]
 *
 * @par Required Header File
 * ipsi_crypto_cmvp_selftest.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for CMVP/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT32 IPSI_CRYPTO_CMVP_SelftestEcdsa(SEC_VOID);

/*
Func Name:  IPSI_CRYPTO_CMVP_SelftestX25519
*/
/**
 * @defgroup IPSI_CRYPTO_CMVP_SelftestX25519
 * @ingroup CMVP_Self_Test_Function
 * @par Prototype
 * @code
 * SEC_INT32 IPSI_CRYPTO_CMVP_SelftestX25519()
 * @endcode
 *
 * @par Purpose
 * This function is used to run X25519 shared secret generation self tests.
 *
 * @par Description
 * IPSI_CRYPTO_CMVP_SelftestX25519 is used to run X25519 shared secret generation self tests in the CPP mode.
 * This API can also be called during Power On or during initialization of the CRYPTO library and hence can be used for
 * Power On Self tests[POST].
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_INT32 (Status Output) On successful return [SEC_SUCCESS|N/A]
 * @retval SEC_INT32 (Status Output) On failure [SEC_ERROR|N/A]
 *
 * @par Required Header File
 * ipsi_crypto_cmvp_selftest.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for CMVP/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT32 IPSI_CRYPTO_CMVP_SelftestX25519(SEC_VOID);

/*
Func Name:  IPSI_CRYPTO_CMVP_SelftestDh
*/
/**
 * @defgroup IPSI_CRYPTO_CMVP_SelftestDh
 * @ingroup CMVP_Self_Test_Function
 * @par Prototype
 * @code
 * SEC_INT32 IPSI_CRYPTO_CMVP_SelftestDh()
 * @endcode
 *
 * @par Purpose
 * This function is used to run DH shared secret generation self tests.
 *
 * @par Description
 * IPSI_CRYPTO_CMVP_SelftestDh is used to run DH shared secret generation self tests in the CPP mode.
 * This API can also be called during Power On or during initialization of the CRYPTO library and hence can be used for
 * Power On Self tests[POST].
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_INT32 (Status Output) On successful return [SEC_SUCCESS|N/A]
 * @retval SEC_INT32 (Status Output) On failure [SEC_ERROR|N/A]
 *
 * @par Required Header File
 * ipsi_crypto_cmvp_selftest.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for CMVP/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT32 IPSI_CRYPTO_CMVP_SelftestDh(SEC_VOID);

/*
Func Name:  IPSI_CRYPTO_CMVP_SelftestEcdh
*/
/**
 * @defgroup IPSI_CRYPTO_CMVP_SelftestEcdh
 * @ingroup CMVP_Self_Test_Function
 * @par Prototype
 * @code
 * SEC_INT32 IPSI_CRYPTO_CMVP_SelftestEcdh()
 * @endcode
 *
 * @par Purpose
 * This function is used to run ECDH shared secret generation self tests.
 *
 * @par Description
 * IPSI_CRYPTO_CMVP_SelftestEcdh is used to run ECDH shared secret generation self tests in the CPP mode.
 * This API can also be called during Power On or during initialization of the CRYPTO library and hence can be used for
 * Power On Self tests[POST].
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_INT32 (Status Output) On successful return [SEC_SUCCESS|N/A]
 * @retval SEC_INT32 (Status Output) On failure [SEC_ERROR|N/A]
 *
 * @par Required Header File
 * ipsi_crypto_cmvp_selftest.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for CMVP/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT32 IPSI_CRYPTO_CMVP_SelftestEcdh(SEC_VOID);

/*
Func Name:  IPSI_CRYPTO_CMVP_SelftestHmac
*/
/**
 * @defgroup IPSI_CRYPTO_CMVP_SelftestHmac
 * @ingroup CMVP_Self_Test_Function
 * @par Prototype
 * @code
 * SEC_INT32 IPSI_CRYPTO_CMVP_SelftestHmac()
 * @endcode
 *
 * @par Purpose
 * This function is used to run HMAC self tests.
 *
 * @par Description
 * IPSI_CRYPTO_CMVP_SelftestHmac is used to run HMAC self tests in the CPP mode.
 * This API can also be called during Power On or during initialization of the CRYPTO library and hence can be used for
 * Power On Self tests[POST].
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_INT32 (Status Output) On successful return [SEC_SUCCESS|N/A]
 * @retval SEC_INT32 (Status Output) On failure [SEC_ERROR|N/A]
 *
 * @par Required Header File
 * ipsi_crypto_cmvp_selftest.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for CMVP/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT32 IPSI_CRYPTO_CMVP_SelftestHmac(SEC_VOID);

/*
Func Name:  IPSI_CRYPTO_CMVP_SelftestGmac
*/
/**
 * @defgroup IPSI_CRYPTO_CMVP_SelftestGmac
 * @ingroup CMVP_Self_Test_Function
 * @par Prototype
 * @code
 * SEC_INT32 IPSI_CRYPTO_CMVP_SelftestGmac()
 * @endcode
 *
 * @par Purpose
 * This function is used to run GMAC self tests.
 *
 * @par Description
 * IPSI_CRYPTO_CMVP_SelftestGmac is used to run GMAC self tests in the CPP mode.
 * This API can also be called during Power On or during initialization of the CRYPTO library and hence can be used for
 * Power On Self tests[POST].
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_INT32 (Status Output) On successful return [SEC_SUCCESS|N/A]
 * @retval SEC_INT32 (Status Output) On failure [SEC_ERROR|N/A]
 *
 * @par Required Header File
 * ipsi_crypto_cmvp_selftest.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for CMVP/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT32 IPSI_CRYPTO_CMVP_SelftestGmac(SEC_VOID);

/*
Func Name:  IPSI_CRYPTO_CMVP_SelftestCmac
*/
/**
 * @defgroup IPSI_CRYPTO_CMVP_SelftestCmac
 * @ingroup CMVP_Self_Test_Function
 * @par Prototype
 * @code
 * SEC_INT32 IPSI_CRYPTO_CMVP_SelftestCmac()
 * @endcode
 *
 * @par Purpose
 * This function is used to run CMAC self tests.
 *
 * @par Description
 * IPSI_CRYPTO_CMVP_SelftestCmac is used to run CMAC self tests in the CPP mode.
 * This API can also be called during Power On or during initialization of the CRYPTO library and hence can be used for
 * Power On Self tests[POST].
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_INT32 (Status Output) On successful return [SEC_SUCCESS|N/A]
 * @retval SEC_INT32 (Status Output) On failure [SEC_ERROR|N/A]
 *
 * @par Required Header File
 * ipsi_crypto_cmvp_selftest.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for CMVP/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT32 IPSI_CRYPTO_CMVP_SelftestCmac(SEC_VOID);

/*
Func Name:  IPSI_CRYPTO_CMVP_SelftestScrypt
*/
/**
 * @defgroup IPSI_CRYPTO_CMVP_SelftestScrypt
 * @ingroup CMVP_Self_Test_Function
 * @par Prototype
 * @code
 * SEC_INT32 IPSI_CRYPTO_CMVP_SelftestScrypt()
 * @endcode
 *
 * @par Purpose
 * This function is used to run SCRYPT self tests.
 *
 * @par Description
 * IPSI_CRYPTO_CMVP_SelftestScrypt is used to run SCRYPT self tests in the CPP mode.
 * This API can also be called during Power On or during initialization of the CRYPTO library and hence can be used for
 * Power On Self tests[POST].
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_INT32 (Status Output) On successful return [SEC_SUCCESS|N/A]
 * @retval SEC_INT32 (Status Output) On failure [SEC_ERROR|N/A]
 *
 * @par Required Header File
 * ipsi_crypto_cmvp_selftest.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for CMVP/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT32 IPSI_CRYPTO_CMVP_SelftestScrypt(SEC_VOID);

/*
Func Name:  IPSI_CRYPTO_CMVP_SelftestPbkdf2
*/
/**
 * @defgroup IPSI_CRYPTO_CMVP_SelftestPbkdf2
 * @ingroup CMVP_Self_Test_Function
 * @par Prototype
 * @code
 * SEC_INT32 IPSI_CRYPTO_CMVP_SelftestPbkdf2()
 * @endcode
 *
 * @par Purpose
 * This function is used to run PBKDF2 self tests.
 *
 * @par Description
 * IPSI_CRYPTO_CMVP_SelftestPbkdf2 is used to run PBKDF2 self tests in the CPP mode.
 * This API can also be called during Power On or during initialization of the CRYPTO library and hence can be used for
 * Power On Self tests[POST].
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_INT32 (Status Output) On successful return [SEC_SUCCESS|N/A]
 * @retval SEC_INT32 (Status Output) On failure [SEC_ERROR|N/A]
 *
 * @par Required Header File
 * ipsi_crypto_cmvp_selftest.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for CMVP/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT32 IPSI_CRYPTO_CMVP_SelftestPbkdf2(SEC_VOID);

/*
Func Name:  IPSI_CRYPTO_CMVP_SelftestDrbgCtr
*/
/**
 * @defgroup IPSI_CRYPTO_CMVP_SelftestDrbgCtr
 * @ingroup CMVP_Self_Test_Function
 * @par Prototype
 * @code
 * SEC_INT32 IPSI_CRYPTO_CMVP_SelftestDrbgCtr()
 * @endcode
 *
 * @par Purpose
 * This function is used to run CTR_DRBG self tests.
 *
 * @par Description
 * IPSI_CRYPTO_CMVP_SelftestDrbgCtr is used to run CTR_DRBG self tests in the CPP mode.
 * This API can also be called during Power On or during initialization of the CRYPTO library and hence can be used for
 * Power On Self tests[POST].
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_INT32 (Status Output) On successful return [SEC_SUCCESS|N/A]
 * @retval SEC_INT32 (Status Output) On failure [SEC_ERROR|N/A]
 *
 * @par Required Header File
 * ipsi_crypto_cmvp_selftest.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for CMVP/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT32 IPSI_CRYPTO_CMVP_SelftestDrbgCtr(SEC_VOID);

/*
Func Name:  IPSI_CRYPTO_CMVP_SelftestDrbgGm
*/
/**
 * @defgroup IPSI_CRYPTO_CMVP_SelftestDrbgGm
 * @ingroup CMVP_Self_Test_Function
 * @par Prototype
 * @code
 * SEC_INT32 IPSI_CRYPTO_CMVP_SelftestDrbgGm()
 * @endcode
 *
 * @par Purpose
 * This function is used to run GM0028_DRBG self tests.
 *
 * @par Description
 * IPSI_CRYPTO_CMVP_SelftestDrbgGm is used to run GM0028 SM4 DRBG self tests
 * This API can also be called during Power On or during initialization of the CRYPTO library and hence can be used for
 * Power On Self tests[POST].
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_INT32 (Status Output) On successful return [SEC_SUCCESS|N/A]
 * @retval SEC_INT32 (Status Output) On failure [SEC_ERROR|N/A]
 *
 * @par Required Header File
 * ipsi_crypto_cmvp_selftest.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for GM0028 certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT32 IPSI_CRYPTO_CMVP_SelftestDrbgGm(SEC_VOID);

/*
Func Name:  IPSI_CRYPTO_CMVP_SelftestDrbgHmac
*/
/**
 * @defgroup IPSI_CRYPTO_CMVP_SelftestDrbgHmac
 * @ingroup CMVP_Self_Test_Function
 * @par Prototype
 * @code
 * SEC_INT32 IPSI_CRYPTO_CMVP_SelftestDrbgHmac()
 * @endcode
 *
 * @par Purpose
 * This function is used to run HMAC_DRBG self tests.
 *
 * @par Description
 * IPSI_CRYPTO_CMVP_SelftestDrbgHmac is used to run HMAC_DRBG self tests in the CPP mode.
 * This API can also be called during Power On or during initialization of the CRYPTO library and hence can be used for
 * Power On Self tests[POST].
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_INT32 (Status Output) On successful return [SEC_SUCCESS|N/A]
 * @retval SEC_INT32 (Status Output) On failure [SEC_ERROR|N/A]
 *
 * @par Required Header File
 * ipsi_crypto_cmvp_selftest.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for CMVP/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT32 IPSI_CRYPTO_CMVP_SelftestDrbgHmac(SEC_VOID);

/*
Func Name:  IPSI_CRYPTO_CMVP_SelftestDrbgHash
*/
/**
 * @defgroup IPSI_CRYPTO_CMVP_SelftestDrbgHash
 * @ingroup CMVP_Self_Test_Function
 * @par Prototype
 * @code
 * SEC_INT32 IPSI_CRYPTO_CMVP_SelftestDrbgHash()
 * @endcode
 *
 * @par Purpose
 * This function is used to run HASH_DRBG self tests.
 *
 * @par Description
 * IPSI_CRYPTO_CMVP_SelftestDrbgHash is used to run HASH_DRBG self tests in the CPP mode.
 * This API can also be called during Power On or during initialization of the CRYPTO library and hence can be used for
 * Power On Self tests[POST].
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_INT32 (Status Output) On successful return [SEC_SUCCESS|N/A]
 * @retval SEC_INT32 (Status Output) On failure [SEC_ERROR|N/A]
 *
 * @par Required Header File
 * ipsi_crypto_cmvp_selftest.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for CMVP/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT32 IPSI_CRYPTO_CMVP_SelftestDrbgHash(SEC_VOID);

/*
Func Name:  IPSI_CRYPTO_CMVP_SelftestSm2
*/
/**
 * @defgroup IPSI_CRYPTO_CMVP_SelftestSm2
 * @ingroup CMVP_Self_Test_Function
 * @par Prototype
 * @code
 * SEC_INT32 IPSI_CRYPTO_CMVP_SelftestSm2()
 * @endcode
 *
 * @par Purpose
 * This function is used to run SM2 self tests.
 *
 * @par Description
 * IPSI_CRYPTO_CMVP_SelftestSm2 is used to run SM2 self tests in the CPP mode.
 * This API can also be called during Power On or during initialization of the CRYPTO library and hence can be used for
 * Power On Self tests[POST].
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_INT32 (Status Output) On successful return [SEC_SUCCESS|N/A]
 * @retval SEC_INT32 (Status Output) On failure [SEC_ERROR|N/A]
 *
 * @par Required Header File
 * ipsi_crypto_cmvp_selftest.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for CMVP/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT32 IPSI_CRYPTO_CMVP_SelftestSm2(SEC_VOID);

/*
Func Name:  IPSI_CRYPTO_CMVP_SelftestSm2Dsa
*/
/**
 * @defgroup IPSI_CRYPTO_CMVP_SelftestSm2Dsa
 * @ingroup CMVP_Self_Test_Function
 * @par Prototype
 * @code
 * SEC_INT32 IPSI_CRYPTO_CMVP_SelftestSm2Dsa()
 * @endcode
 *
 * @par Purpose
 * This function is used to run SM2 self tests.
 *
 * @par Description
 * IPSI_CRYPTO_CMVP_SelftestSm2Dsa is used to run SM2 self tests in the CPP mode.
 * Signature is calculated using a known private key and data to be signed. The signature is then verified
 * using the same data used to create the signature.
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_INT32 (Status Output) On successful return [SEC_SUCCESS|N/A]
 * @retval SEC_INT32 (Status Output) On failure [SEC_ERROR|N/A]
 *
 * @par Required Header File
 * ipsi_crypto_cmvp_selftest.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for CMVP/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT32 IPSI_CRYPTO_CMVP_SelftestSm2Dsa(SEC_VOID);

/*
Func Name:  IPSI_CRYPTO_CMVP_SelftestSm2Pkea
*/
/**
 * @defgroup IPSI_CRYPTO_CMVP_SelftestSm2Pkea
 * @ingroup CMVP_Self_Test_Function
 * @par Prototype
 * @code
 * SEC_INT32 IPSI_CRYPTO_CMVP_SelftestSm2Pkea()
 * @endcode
 *
 * @par Purpose
 * This function is used to run SM2 self tests.
 *
 * @par Description
 * IPSI_CRYPTO_CMVP_SelftestSm2Pkea is used to run SM2 self tests in the CPP mode.
 * CipherText is calculated using a known public key and data to be enctypted. The ciphertext is then verified
 * using the same data used to create the ciphertext.
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_INT32 (Status Output) On successful return [SEC_SUCCESS|N/A]
 * @retval SEC_INT32 (Status Output) On failure [SEC_ERROR|N/A]
 *
 * @par Required Header File
 * ipsi_crypto_cmvp_selftest.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for CMVP/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT32 IPSI_CRYPTO_CMVP_SelftestSm2Pkea(SEC_VOID);

/*
Func Name:  IPSI_CRYPTO_CMVP_SelftestSm3
*/
/**
 * @defgroup IPSI_CRYPTO_CMVP_SelftestSm3
 * @ingroup CMVP_Self_Test_Function
 * @par Prototype
 * @code
 * SEC_INT32 IPSI_CRYPTO_CMVP_SelftestSm3()
 * @endcode
 *
 * @par Purpose
 * This function is used to run SM3 self tests.
 *
 * @par Description
 * IPSI_CRYPTO_CMVP_SelftestSm3 is used to run SM3 self tests in the CPP mode.
 * This API can also be called during Power On or during initialization of the CRYPTO library and hence can be used for
 * Power On Self tests[POST].
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_INT32 (Status Output) On successful return [SEC_SUCCESS|N/A]
 * @retval SEC_INT32 (Status Output) On failure [SEC_ERROR|N/A]
 *
 * @par Required Header File
 * ipsi_crypto_cmvp_selftest.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for CMVP/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT32 IPSI_CRYPTO_CMVP_SelftestSm3(SEC_VOID);

/*
Func Name:  IPSI_CRYPTO_CMVP_SelftestSm4
*/
/**
 * @defgroup IPSI_CRYPTO_CMVP_SelftestSm4
 * @ingroup CMVP_Self_Test_Function
 * @par Prototype
 * @code
 * SEC_INT32 IPSI_CRYPTO_CMVP_SelftestSm4()
 * @endcode
 *
 * @par Purpose
 * This function is used to run SM4(CBC & ECB) self tests.
 *
 * @par Description
 * IPSI_CRYPTO_CMVP_SelftestSm4 is used to run SM4(CBC & ECB) self tests in the CPP mode.
 * This API can also be called during Power On or during initialization of the CRYPTO library and hence can be used for
 * Power On Self tests[POST].
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_INT32 (Status Output) On successful return [SEC_SUCCESS|N/A]
 * @retval SEC_INT32 (Status Output) On failure [SEC_ERROR|N/A]
 *
 * @par Required Header File
 * ipsi_crypto_cmvp_selftest.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for CMVP/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT32 IPSI_CRYPTO_CMVP_SelftestSm4(SEC_VOID);

/*
Func Name:  IPSI_CRYPTO_CMVP_SelftestHmacSm3
*/
/**
 * @defgroup IPSI_CRYPTO_CMVP_SelftestHmacSm3
 * @ingroup CMVP_Self_Test_Function
 * @par Prototype
 * @code
 * SEC_INT32 IPSI_CRYPTO_CMVP_SelftestHmacSm3()
 * @endcode
 *
 * @par Purpose
 * This function is used to run HMAC-SM3 self tests.
 *
 * @par Description
 * IPSI_CRYPTO_CMVP_SelftestHmacSm3 is used to run HMAC-SM3 self tests in the CPP mode.
 * This API can also be called during Power On or during initialization of the CRYPTO library and hence can be used for
 * Power On Self tests[POST].
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_INT32 (Status Output) On successful return [SEC_SUCCESS|N/A]
 * @retval SEC_INT32 (Status Output) On failure [SEC_ERROR|N/A]
 *
 * @par Required Header File
 * ipsi_crypto_cmvp_selftest.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for CMVP/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT32 IPSI_CRYPTO_CMVP_SelftestHmacSm3(SEC_VOID);

#ifdef __cplusplus
}
#endif /* end of __cplusplus */

#endif // IPSI_CRYPTO_CMVP_SELFTEST_H
