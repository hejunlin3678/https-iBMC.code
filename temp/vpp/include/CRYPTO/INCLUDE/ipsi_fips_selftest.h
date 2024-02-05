/**
 * ***************************************************************************
 * Copyright (c) Huawei Technologies Co., Ltd. 2014-2019. All rights reserved.
 * FileName     : ipsi_fips_selftest.h
 * Author       : Prabhat
 * Version      : 1
 * Description  : Defines interfaces for FIPS crypto
 * Create:  2014-11-11
 * History:
 * <author>   <date>          <desc>
 * ----------------------------------------------------------------------------
 * ***************************************************************************
 */
#ifndef IPSI_FIPS_SELFTEST_H
#define IPSI_FIPS_SELFTEST_H

#include "ipsi_types.h"
#include "sec_sys.h"
#include "sec_crypto.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
Func Name:  IPSI_CRYPT_selftest
*/
/**
 * @defgroup FIPS_Self_Test_Function FIPS Self Test Function
 * This provides information for FIPS self test Function.
 */
/**
* @defgroup IPSI_CRYPT_selftest
* @ingroup FIPS_Self_Test_Function
* @par Prototype
* @code
* SEC_UINT32 IPSI_CRYPT_selftest()
* @endcode
*
* @par Purpose
* This function runs self tests for all FIPS supported algorithms.
*
* @par Description
* IPSI_CRYPT_selftest API is used to run self tests for all the FIPS supported algorithms in a single function call.
* This API can be used at Power On or during initialization of the CRYPTO library. This API can be used for
*  Power On Self Tests(POST).
* - The following self tests will be performed:
*	- AES (ECB, CBC, CFB)
*	- AES CMAC and Triple DES CMAC
*	- AES GCM
*	- AES CTR
*	- Triple DES (ECB, CBC, CFB)
*	- SHA-1
*	- HMAC SHA-1, HMAC-SHA-2 (224, 256, 384, 512)
*	- ECC CDH (KAS)
*	- ECDSA
*	- DSA
*	- RSA
*	- Hash-DRBG, HMAC-DRBG, CTR-DRBG (AES)
* .

*
* @par Parameters
* N/A
*
* @retval SEC_UINT32 (Status Output) On successful return. [SEC_TRUE|N/A]
* @retval SEC_UINT32 (Status Output) On failure. [SEC_FALSE|N/A]
*
* @par Required Header File
* ipsi_fips_selftest.h
*
* @par Note
*    - If the selftest fails then the application must not call any API which provides FIPS approved
*       algorithm functionality.
*    - If the FIPS crypto module fails and the IPSI_CRYPT_selftest API
*       is called then this API returns SEC_FALSE.
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 IPSI_CRYPT_selftest(void);

/*
Func Name:  IPSI_CRYPT_selftest_aes
*/
/**
 * @defgroup IPSI_CRYPT_selftest_aes
 * @ingroup FIPS_Self_Test_Function
 * @par Prototype
 * @code
 * SEC_UINT32 IPSI_CRYPT_selftest_aes()
 * @endcode
 *
 * @par Purpose
 * This function is used to run AES self tests.
 *
 * @par Description
 * IPSI_CRYPT_selftest_aes is used to run AES self tests in the CPP mode, hence this function can be used
 * to run all the AES self known answer tests supported.
 * This API can also be called during Power On or during initialization of the CRYPTO library and hence can be used for
 * Power On Self Tests[POST].
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_UINT32 (Status Output) On successful return. [SEC_TRUE|N/A]
 * @retval SEC_UINT32 (Status Output) On failure. [SEC_FALSE|N/A]
 *
 * @par Required Header File
 * ipsi_fips_selftest.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for FIPS/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 IPSI_CRYPT_selftest_aes(void);

/*
Func Name:  IPSI_CRYPT_selftest_sha1
*/
/**
 * @defgroup IPSI_CRYPT_selftest_sha1
 * @ingroup FIPS_Self_Test_Function
 * @par Prototype
 * @code
 * SEC_UINT32 IPSI_CRYPT_selftest_sha1()
 * @endcode
 *
 * @par Purpose
 * This function is used to run SHA1 self tests.
 *
 * @par Description
 * IPSI_CRYPT_selftest_sha1 is used to run SHA1 self tests in the CPP mode, hence this function can be used
 * to run all SHA1 supported self Known Answer Tests supported.
 * This API can also be called during Power On or during initialization of the CRYPTO library and hence can be used for
 * Power On Self tests[POST].
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_UINT32 (Status Output) On successful return [SEC_TRUE|N/A]
 * @retval SEC_UINT32 (Status Output) On failure [SEC_FALSE|N/A]
 *
 * @par Required Header File
 * ipsi_fips_selftest.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for FIPS/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 IPSI_CRYPT_selftest_sha1(void);

/*
Func Name:  IPSI_CRYPT_selftest_hmac
*/
/**
 * @defgroup IPSI_CRYPT_selftest_hmac
 * @ingroup FIPS_Self_Test_Function
 * @par Prototype
 * @code
 * SEC_UINT32 IPSI_CRYPT_selftest_hmac()
 * @endcode
 *
 * @par Purpose
 * This function is used to Run HMAC self tests.
 *
 * @par Description
 * IPSI_CRYPT_selftest_hmac is used to run HMAC self tests in the CPP mode, hence this function can be used
 * to run all HMAC supported self Known Answer Tests supported.
 * This API can also be called during Power On or during initialization of the CRYPTO library and hence can be used for
 * POST-Power On Self tests.
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_UINT32 (Status Output) On successful return [SEC_TRUE|N/A]
 * @retval SEC_UINT32 (Status Output) On failure [SEC_FALSE|N/A]
 *
 * @par Required Header File
 * ipsi_fips_selftest.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for FIPS/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 IPSI_CRYPT_selftest_hmac(void);

/*
Func Name:  IPSI_CRYPT_selftest_des
*/
/**
 * @defgroup IPSI_CRYPT_selftest_des
 * @ingroup FIPS_Self_Test_Function
 * @par Prototype
 * @code
 * SEC_UINT32 IPSI_CRYPT_selftest_des()
 * @endcode
 *
 * @par Purpose
 * This function is used to run DES self tests.
 *
 * @par Description
 * IPSI_CRYPT_selftest_des is used to run DES self tests in the CPP mode, hence this function can be used
 * to run all DES supported self Known Answer Tests supported.
 * This API can also be called during Power On or during initialization of the CRYPTO library and hence can be used for
 * POST-Power On Self Tests.
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_UINT32 (Status Output) On successful return [SEC_TRUE|N/A]
 * @retval SEC_UINT32 (Status Output) On failure [SEC_FALSE|N/A]
 *
 * @par Required Header File
 * ipsi_fips_selftest.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for FIPS/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 IPSI_CRYPT_selftest_des(void);

/*
Func Name:  IPSI_CRYPT_selftest_rsa
*/
/**
 * @defgroup IPSI_CRYPT_selftest_rsa
 * @ingroup FIPS_Self_Test_Function
 * @par Prototype
 * @code
 * SEC_UINT32 IPSI_CRYPT_selftest_rsa()
 * @endcode
 *
 * @par Purpose
 * This function is used to run RSA signature self tests.
 *
 * @par Description
 * IPSI_CRYPT_selftest_rsa is used to run RSA signature self tests in the CPP mode.
 * This API can also be called during Power On or during initialization of the CRYPTO library and hence can be used for
 * POST-Power On Self tests.
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_UINT32 (Status Output) On successful return. [SEC_TRUE|N/A]
 * @retval SEC_UINT32 (Status Output) On failure. [SEC_FALSE|N/A]
 *
 * @par Required Header File
 * ipsi_fips_selftest.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for FIPS/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 IPSI_CRYPT_selftest_rsa(void);

/*
Func Name:  IPSI_CRYPT_selftest_cmac
*/
/**
 * @defgroup IPSI_CRYPT_selftest_cmac
 * @ingroup FIPS_Self_Test_Function
 * @par Prototype
 * @code
 * SEC_UINT32 IPSI_CRYPT_selftest_cmac()
 * @endcode
 *
 * @par Purpose
 * This function is used to run CMAC self tests.
 *
 * @par Description
 * IPSI_CRYPT_selftest_cmac is used to run CMAC self tests in the CPP mode, hence this function can be used
 * to run all CMAC supported self Known Answer Tests supported.
 * This API can also be called during Power On or during initialization of the CRYPTO library.
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_UINT32 (Status Output) On successful return [SEC_TRUE|N/A]
 * @retval SEC_UINT32 (Status Output) On failure [SEC_FALSE|N/A]
 *
 * @par Required Header File
 * ipsi_fips_selftest.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for FIPS/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 IPSI_CRYPT_selftest_cmac(void);

/*
Func Name:  IPSI_CRYPT_selftest_aes_gcm
*/
/**
 * @defgroup IPSI_CRYPT_selftest_aes_gcm
 * @ingroup FIPS_Self_Test_Function
 * @par Prototype
 * @code
 * SEC_UINT32 IPSI_CRYPT_selftest_aes_gcm()
 * @endcode
 *
 * @par Purpose
 * This function is used to run AES GCM self tests.
 *
 * @par Description
 * IPSI_CRYPT_selftest_aes_gcm is used to run AES GCM self tests in the CPP mode, hence this function can be used
 * to run all AES GCM supported self Known Answer Tests supported.
 * This API can also be called during Power On or during initialization of the CRYPTO library.
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_UINT32 (Status Output) On successful return. [SEC_TRUE|N/A]
 * @retval SEC_UINT32 (Status Output) On failure. [SEC_FALSE|N/A]
 *
 * @par Required Header File
 * ipsi_fips_selftest.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for FIPS/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 IPSI_CRYPT_selftest_aes_gcm(void);

/*
Func Name:  IPSI_CRYPT_selftest_aes_ctr
*/
/**
 * @defgroup IPSI_CRYPT_selftest_aes_ctr
 * @ingroup FIPS_Self_Test_Function
 * @par Prototype
 * @code
 * SEC_UINT32 IPSI_CRYPT_selftest_aes_ctr()
 * @endcode
 *
 * @par Purpose
 * This function is used to run AES CTR self tests.
 *
 * @par Description
 * IPSI_CRYPT_selftest_aes_ctr is used to run AES CTR self tests in the CPP mode, hence this function can be used
 * to run all the AES CTR self known Answer Tests supported.
 * This API can also be called during power on or during initialization of the CRYPTO library and hence can be used for
 * Power On Self Tests[POST].
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_UINT32 (Status Output) On successful return. [SEC_TRUE|N/A]
 * @retval SEC_UINT32 (Status Output) On failure. [SEC_FALSE|N/A]
 *
 * @par Required Header File
 * ipsi_fips_selftest.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for FIPS/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 IPSI_CRYPT_selftest_aes_ctr(void);

/*
Func Name:  IPSI_CRYPT_selftest_dsa
*/
/**
 * @defgroup IPSI_CRYPT_selftest_dsa
 * @ingroup FIPS_Self_Test_Function
 * @par Prototype
 * @code
 * SEC_UINT32 IPSI_CRYPT_selftest_dsa()
 * @endcode
 *
 * @par Purpose
 * This function is used to run DSA signature generation and verification self test.
 *
 * @par Description
 * IPSI_CRYPT_selftest_dsa is used to run DSA signature generation and verification self test in the CPP mode.
 * This API can also be called during Power On or during initialization of the CRYPTO library and hence can be used for
 * Power On Self tests[POST].
 * Signature is calculated using a known private key and data to be signed. The signature is then verified
 * using the same data used to create the signature. Test is done using 2048 Key and SHA384.
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_UINT32 (Status Output) On successful return [SEC_TRUE|N/A]
 * @retval SEC_UINT32 (Status Output) On failure [SEC_FALSE|N/A]
 *
 * @par Required Header File
 * ipsi_fips_selftest.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for FIPS/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 IPSI_CRYPT_selftest_dsa(void);

/**
 * @defgroup IPSI_CRYPT_test_rsa_set_prime
 * @ingroup Testability
 * @par Prototype
 * @code
 * SEC_VOID IPSI_CRYPT_test_rsa_set_prime(SEC_BIGINT_S *pstP, SEC_BIGINT_S *pstQ)
 * @endcode
 *
 * @par Purpose
 * This function is used to set the p and q value in RSA key.
 *
 * @par Description
 * IPSI_CRYPT_test_rsa_set_prime sets the p and q value in RSA key. The API need to be called before the
 * CRYPT_genKeyPair API.
 *
 * @param[in] *pstP (Data Input) P value [N/A]
 * @param[in] *pstQ (Data Input) Q value [N/A]
 *
 * @retval SEC_VOID This function does not return any value [N/A|N/A]
 *
 * @par Required Header File
 * ipsi_fips_selftest.h
 *
 * @par Note
 * \n
 * - The CRYPT_genKeyPair API always takes the set p and q value which it is set by the IPSI_CRYPT_test_rsa_set_prime,
 * unless p and q are set to NULL using the same IPSI_CRYPT_test_rsa_set_prime(NULL, NULL) API.
 * - User should either set both P and Q values as valid ones or both the values to be NULL.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_VOID IPSI_CRYPT_test_rsa_set_prime(SEC_BIGINT_S *pstP, SEC_BIGINT_S *pstQ);

/**
 * @defgroup IPSI_CRYPT_selftest_ecdh
 * @ingroup FIPS Self Test Function
 * @par Prototype
 * @code
 * SEC_UINT32 IPSI_CRYPT_selftest_ecdh()
 * @endcode
 *
 * @par Purpose
 * This function is used to run ECDH shared secret generation self test.
 *
 * @par Description
 * IPSI_CRYPT_selftest_ecdh is used to run ECDH shared secret generation self test in the FIPS mode.
 * This API can also be called during Power On or during init of the FIPS library - hence can be used for
 * POST-Power On Self tests.
 * Shared secret is calculated using a known host key pair (private key and public key) and a peer public key.
 * The generated shared secret is then verified against a known value.
 *
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_UINT32 (Status Output) On successful return [SEC_TRUE|N/A]
 * @retval SEC_UINT32 (Status Output) On failure [SEC_FALSE|N/A]
 *
 * @par Required Header File
 * ipsi_fips_selftest.h
 *
 * @par Note
 * - This API is exposed for the products or applications which are going for FIPS certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 IPSI_CRYPT_selftest_ecdh(void);

/**
 * @defgroup IPSI_CRYPT_selftest_ecdsa
 * @ingroup FIPS Self Test Function
 * @par Prototype
 * @code
 * SEC_UINT32 IPSI_CRYPT_selftest_ecdsa()
 * @endcode
 *
 * @par Purpose
 * This function is used to run ECDSA signature generation and verification self test.
 *
 * @par Description
 * IPSI_CRYPT_selftest_ecdsa is used to run ECDSA signature generation and verification self test in the FIPS mode.
 * This API can also be called during Power On or during init of the FIPS library - hence can be used for
 * POST-Power On Self tests.
 * Signature is calculated using a known private key and data to be signed. The signature is then verified
 * using the same data used to create the signature.
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_UINT32 (Status Output) On successful return [SEC_TRUE|N/A]
 * @retval SEC_UINT32 (Status Output) On failure [SEC_FALSE|N/A]
 *
 * @par Required Header File
 * ipsi_fips_selftest.h
 *
 * @par Note
 * - This API is exposed for the products or applications which are going for FIPS certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 IPSI_CRYPT_selftest_ecdsa(void);

/**
 * @defgroup IPSI_CRYPT_selftest_aes_ccm
 * @ingroup FIPS_Self_Test_Function
 * @par Prototype
 * @code
 * SEC_UINT32 IPSI_CRYPT_selftest_aes_ccm()
 * @endcode
 *
 * @par Purpose
 * This function is used to run AES CCM self tests.
 *
 * @par Description
 * IPSI_CRYPT_selftest_aes_ccm is used to run AES CCM self tests in the CPP mode/FIPS mode, hence this function can be
 * used to run all AES GCM supported self Known Answer Tests supported.
 * This API can also be called during Power On or during initialization of the CRYPTO library.
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_UINT32 (Status Output) On successful return. [SEC_TRUE|N/A]
 * @retval SEC_UINT32 (Status Output) On failure. [SEC_FALSE|N/A]
 *
 * @par Required Header File
 * ipsi_fips_selftest.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for FIPS/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 IPSI_CRYPT_selftest_aes_ccm(void);
#ifdef __cplusplus
}
#endif /* end of __cplusplus */
#endif /* end of _IPSI_FIPS_SELFTEST_H_ */
