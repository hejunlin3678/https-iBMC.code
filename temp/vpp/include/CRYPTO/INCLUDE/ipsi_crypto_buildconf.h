/* **********************************
 * Copyright (c) Huawei Technologies Co., Ltd. 2010-2021. All rights reserved.
FileName: ipsi_crypto_buildconf.h
Author: Prabhat Kumar Gopalika
Version: 1

Description: This file controls the build configurations of crypto.
 * Create: 2010-04-08

        History               :
                <author>   <date>          <desc>
                Kaushik		2010-10-15	Addes VPPSEC and IKE Enhancement
                                        Macros
***************************************************************************** */
#ifndef IPSI_CRYPTO_BUILDCONF_H
#define IPSI_CRYPTO_BUILDCONF_H

/*
 * gobal variable in SRE OS is specical. There is no thread in SRE. Gobal variable is default shared by multi-process.
 * So you should use OS_MODIFIER_FOR_SLAVE_PRIVATE_DATA macro before global var when it need to be privated.
 */
#ifdef __IPSI_HERT_REDUE_FOR_SRE__
#define OS_MODIFIER_FOR_SLAVE_PRIVATE_DATA __attribute__((section(".slave_private_data")))
#else
#define OS_MODIFIER_FOR_SLAVE_PRIVATE_DATA
#endif // __IPSI_HERT_REDUE_FOR_SRE__

#ifdef __IPSI_NO_BIN_LOG__
#undef __IPSI_NO_BIN_LOG__
#endif

#ifdef __IPSI_NO_CPLUS_SHA224__
#undef __IPSI_NO_CPLUS_SHA224__
#endif

#ifdef __IPSI_NO_CORE_SHA224__
#undef __IPSI_NO_CORE_SHA224__
#endif

#ifdef __IPSI_NO_CPLUS_SHA256__
#undef __IPSI_NO_CPLUS_SHA256__
#endif

// Enable DH Parameter Generation support
#ifdef __IPSI_NO_DH_PARA__
#undef __IPSI_NO_DH_PARA__
#endif
#ifdef __IPSI_NO_CORE_SHA256__
#undef __IPSI_NO_CORE_SHA256__
#endif

#ifdef __IPSI_NO_CPLUS_SHA384__
#undef __IPSI_NO_CPLUS_SHA384__
#endif

#ifdef __IPSI_NO_CORE_SHA384__
#undef __IPSI_NO_CORE_SHA384__
#endif

#ifdef __IPSI_NO_CPLUS_SHA512__
#undef __IPSI_NO_CPLUS_SHA512__
#endif

#ifdef __IPSI_NO_CORE_SHA512__
#undef __IPSI_NO_CORE_SHA512__
#endif

#ifdef __IPSI_NO_CPLUS_HMAC_SHA224__
#undef __IPSI_NO_CPLUS_HMAC_SHA224__
#endif

#ifdef __IPSI_NO_CORE_HMAC_SHA224__
#undef __IPSI_NO_CORE_HMAC_SHA224__
#endif

#ifdef __IPSI_NO_CPLUS_HMAC_SHA256__
#undef __IPSI_NO_CPLUS_HMAC_SHA256__
#endif

#ifdef __IPSI_NO_CORE_HMAC_SHA256__
#undef __IPSI_NO_CORE_HMAC_SHA256__
#endif

#ifdef __IPSI_NO_CPLUS_HMAC_SHA384__
#undef __IPSI_NO_CPLUS_HMAC_SHA384__
#endif

#ifdef __IPSI_NO_CORE_HMAC_SHA384__
#undef __IPSI_NO_CORE_HMAC_SHA384__
#endif

#ifdef __IPSI_NO_CPLUS_HMAC_SHA512__
#undef __IPSI_NO_CPLUS_HMAC_SHA512__
#endif

#ifdef __IPSI_NO_CORE_HMAC_SHA512__
#undef __IPSI_NO_CORE_HMAC_SHA512__
#endif

// Enable AES XCBC MAC Support
#ifdef __IPSI_NO_AES_XCBC__
#undef __IPSI_NO_AES_XCBC__
#endif

// BEGIN: Macros for disabling VPPSEC specific functions
#ifdef __IPSI_NO_CORE_AES__
#undef __IPSI_NO_CORE_AES__
#endif

#ifdef __IPSI_NO_CORE_DES__
#undef __IPSI_NO_CORE_DES__
#endif

#ifdef __IPSI_NO_CORE_BN__
#undef __IPSI_NO_CORE_BN__
#endif

#ifdef __IPSI_NO_CORE_DH__
#undef __IPSI_NO_CORE_DH__
#endif

// END: Macros for disabling VPPSEC specific functions
#ifdef __IPSI_NO_RC2__
#undef __IPSI_NO_RC2__
#endif

// BEGIN: Macros for disabling rsa prime generation callbacks
#ifdef __IPSI_RSA_NOEXTENDEDKEYPAIR__
#undef __IPSI_RSA_NOEXTENDEDKEYPAIR__
#endif

#ifdef __IPSI_RSA_NOTIMER__
#undef __IPSI_RSA_NOTIMER__
#endif

#ifdef __IPSI_NO_ASYM_COMMON_IFACE__
#undef __IPSI_NO_ASYM_COMMON_IFACE__
#endif

#ifdef __IPSI_NO_HMAC_COMMON_IFACE__
#undef __IPSI_NO_HMAC_COMMON_IFACE__
#endif

#ifdef __IPSI_CRYPTO_NO_ASSERTS__
#undef __IPSI_CRYPTO_NO_ASSERTS__
#endif

#ifdef __IPSI_NO_DSA_IFACE__
#undef __IPSI_NO_DSA_IFACE__
#endif

#ifdef __IPSI_NO_RNG_COREIMPL__
#undef __IPSI_NO_RNG_COREIMPL__
#endif

#ifdef __IPSI_NO_EC_PARAM_POINTS__
#undef __IPSI_NO_EC_PARAM_POINTS__
#endif

#ifdef __IPSI_NO_SEC_DEF__
#undef __IPSI_NO_SEC_DEF__
#endif

#ifdef __IPSI_NO_PID__
#undef __IPSI_NO_PID__
#endif

#ifdef __IPSI_NO_BITOP__
#undef __IPSI_NO_BITOP__
#endif

#ifdef __IPSI_NO_CRYPTO_FUNC__
#undef __IPSI_NO_CRYPTO_FUNC__
#endif

#ifdef __IPSI_NO_CRYPTO_INIT__
#undef __IPSI_NO_CRYPTO_INIT__
#endif

#ifdef __IPSI_NO_CRYPTO_MISC__
#undef __IPSI_NO_CRYPTO_MISC__
#endif

#ifdef __IPSI_NO_MEM_CALLOC__
#undef __IPSI_NO_MEM_CALLOC__
#endif

// END: Macros for disabling rsa prime generation callbacks
/* Macros for disabling AEAD (AES GCM & CCM ) operations */
#ifdef __IPSI_NO_CORE_AES_GCM__
#undef __IPSI_NO_CORE_AES_GCM__
#endif

#ifdef __IPSI_NO_AES_GCM_ENC__
#undef __IPSI_NO_AES_GCM_ENC__
#endif

#ifdef __IPSI_NO_AES_GCM_DEC__
#undef __IPSI_NO_AES_GCM_DEC__
#endif

#ifdef __IPSI_NO_CORE_AES_CCM__
#undef __IPSI_NO_CORE_AES_CCM__
#endif

#ifdef __IPSI_NO_AES_CCM_ENC__
#undef __IPSI_NO_AES_CCM_ENC__
#endif

#ifdef __IPSI_NO_AES_CCM_DEC__
#undef __IPSI_NO_AES_CCM_DEC__
#endif

#ifdef __IPSI_NO_AEAD_ENC__
#undef __IPSI_NO_AEAD_ENC__
#endif

#ifdef __IPSI_NO_AEAD_DEC__
#undef __IPSI_NO_AEAD_DEC__
#endif

#ifdef __IPSI_NO_AEAD__
#undef __IPSI_NO_AEAD__
#endif

#ifdef __IPSI_NO_SAFE_API__
#undef __IPSI_NO_SAFE_API__
#endif

#include "ipsi_crypto_buildconf_mbsc_small_crypto.h"
#include "ipsi_crypto_buildconf_iwf_hert_android.h"
#include "ipsi_crypto_buildconf_hert_sre_crypto.h"

/* As posix lock apis are not present in NSE environemnt commenting SEC logs and ipsi_lock */
#ifdef __IPSI_NSE_CRYPTO__
#define __IPSI_NO_SEC_LOG__
#define __IPSI_NO_CRYPTO_LOG__
#define __IPSI_NO_DEBUG_SWITCH__
#endif // __IPSI_NSE_CRYPTO__

#ifdef __IPSI_HERT_REDUCED_CRYPTO_FOR_1381__
#define __IPSI_NO_EC_PARAM_POINTS__
#define __IPSI_NO_KDF_SM3__
#define __IPSI_NO_HMAC_SM3__
#define __IPSI_NO_SM3__
#define __IPSI_NO_SM2__
#define __IPSI_NO_SM2DSA__
#define __IPSI_NO_KDF_SHA224__
#define __IPSI_NO_KDF_SHA256__
#define __IPSI_NO_KDF_SHA384__
#define __IPSI_NO_KDF_SHA512__
#define __IPSI_NO_SCRYPT
#define __IPSI_NO_OSSL_WRAP_BN__

#ifndef __IPSI_AES_GCM_128_AND_256__
#define __IPSI_NO_CORE_AES_GCM__
#define __IPSI_NO_AES_GCM_ENC__
#define __IPSI_NO_CORE_AES_CCM__
#define __IPSI_NO_AES_GCM_DEC__
#define __IPSI_NO_AES_CCM_ENC__
#define __IPSI_NO_AES_CCM_DEC__
#define __IPSI_NO_AEAD_ENC__
#define __IPSI_NO_AEAD_DEC__
#define __IPSI_NO_AEAD__
#define __IPSI_NO_AEAD_IFACE__
#endif

#define __IPSI_NO_SYM_CTR_ENC_DEC__
#define __IPSI_NO_SYM_IFACE__
#define __IPSI_NO_DES_EDE3_CBCM__
#define __IPSI_NO_RSA_SIGN_WITH_PVT_EXP__

/* Removing the Blowfish Core implementation as well */
#define __IPSI_NO_BLOWFISH__

#define __IPSI_NO_BF_ECB_DCR__
#define __IPSI_NO_BF_CFB_DCR__
#define __IPSI_NO_BF_OFB_DCR__
#define __IPSI_NO_BF_CBC_DCR__

#define __IPSI_NO_BF_ECB_ENC__
#define __IPSI_NO_BF_CBC_ENC__
#define __IPSI_NO_BF_CFB_ENC__
#define __IPSI_NO_BF_OFB_ENC__

/* Asymmetric RSA Encryption/Decryption Flag */
#define __IPSI_NO_RSA_ENC_DEC__
#define __IPSI_NO_RSA_ENC__
#define __IPSI_NO_RSA_DEC__
#define __IPSI_NO_RSA_GEN__
#define __IPSI_NO_RSA_PAD__

/* RSA Encode/Decode */
#define __IPSI_NO_RSA_KEY_ENC_DEC__
#define __IPSI_NO_RSA_KEY_ENC__
#define __IPSI_NO_RSA_PUB_KEY_ENC__
#define __IPSI_NO_RSA_PVT_KEY_ENC__
#define __IPSI_NO_RSA_PUB_KEY_DEC__
#define __IPSI_NO_RSA_KEY_PAIR_ENC__
#define __IPSI_NO_RSA_KEY_DEC__
#define __IPSI_NO_RSA_PUB_KEY_DEC__
#define __IPSI_NO_RSA_PVT_KEY_DEC__
#define __IPSI_NO_RSA_KEY_PAIR_DEC__

/* Asymmetric Key Flag */
#define __IPSI_NO_ASYM_KEY__
#define __IPSI_NO_ASYM_RSA_KEY__
#define __IPSI_NO_ASYM_DSA_KEY__
#define __IPSI_NO_ASYM_EC_KEY__
#define __IPSI_NO_ASYM_DH_KEY__
#define __IPSI_NO_ASYM_ECDH_KEY__
#define __IPSI_NO_ASYM_KEY_GEN__
#define __IPSI_NO_PKEY__
#define __IPSI_NO_KEY_ENC_DCR__
#define __IPSI_RSA_NOEXTENDEDKEYPAIR__
#define __IPSI_RSA_NOTIMER__

/* Asymmetric Sign Verify Flag */
#define __IPSI_NO_ASYM_SIGN__
#define __IPSI_NO_ASYM_RSA_SIGN__
#define __IPSI_NO_ASYM_DSA_SIGN__
#define __IPSI_NO_ASYM_ECDSA_SIGN__
#define __IPSI_NO_ASYM_SM2DSA_SIGN__
#define __IPSI_NO_ASYM_SIGN_VRF__
#define __IPSI_NO_ASYM_SIGN_VER__
#define __IPSI_NO_ASYM_RSA_SIGN_VRF__
#define __IPSI_NO_ASYM_DSA_SIGN_VRF__
#define __IPSI_NO_ASYM_ECDSA_SIGN_VRF__
#define __IPSI_NO_ASYM_SM2DSA_SIGN_VRF__
#define __IPSI_NO_ASYM_SSL_SIGN_VRF__

/* Asymmetric others */
#define __IPSI_NO_RSA__
#define __IPSI_NO_DSA__
#define __IPSI_NO_DH__
#define __IPSI_NO_EC__
#define __IPSI_NO_ECDH__
#define __IPSI_NO_ECDSA__
#define __IPSI_NO_XBN__
#define __IPSI_NO_CORE_BN__
#define __IPSI_NO_CORE_DH__
#define __IPSI_NO_DH_PARA__
#define __IPSI_NO_RSA_MGF__
#define __IPSI_NO_EC_POLY_CURVE__
#define __IPSI_NO_ASYM_COMMON_IFACE__

/* Tailor BN */
#define __IPSI_NO_BN__
#define __IPSI_NO_SMALL_BN__

#ifndef __IPSI_NO_SHA__
/* Tailor hash & HMAC algorithms */
#define __IPSI_NO_MD4__
#define __IPSI_NO_MD5__

#define __IPSI_NO_SHA512__
#define __IPSI_NO_SHA384__
#define __IPSI_NO_SHA224__
#define __IPSI_NO_CORE_SHA224__
#define __IPSI_NO_CORE_SHA384__
#define __IPSI_NO_CORE_SHA512__
#define __IPSI_NO_CPLUS_SHA384__
#define __IPSI_NO_CPLUS_SHA512__
#define __IPSI_NO_CPLUS_HMAC_SHA384__
#define __IPSI_NO_CORE_HMAC_SHA384__
#define __IPSI_NO_CPLUS_HMAC_SHA512__
#define __IPSI_NO_CORE_HMAC_SHA512__
#define __IPSI_NO_KDF_SHA384__
#define __IPSI_NO_KDF_SHA512__
#define __IPSI_NO_HMAC_SHA384__
#define __IPSI_NO_HMAC_SHA512__
#define __IPSI_NO_HMAC_MD4__
#define __IPSI_NO_CPLUS_HMAC_SHA384__
#define __IPSI_NO_CPLUS_HMAC_SHA512__
#define __IPSI_NO_HMAC_MD5__
#define __IPSI_NO_HMAC_SHA1__
#define __IPSI_NO_HMAC_SHA224__
#define __IPSI_NO_EXPOSED_DIGEST__
#endif

/* XCBC tailoring */
#define __IPSI_NO_AES_XCBC__
#define __IPSI_NO_XCBC_AES__

/* Key wrap tailoring */
#define __IPSI_NO_KEY_WRAP_UNWRAP__

/* Dot16 KDF tailoring */
#define __IPSI_NO_DOT16_KDF__

/* Symmetric operations */
#define __IPSI_NO_KASUMI_CBC_ENC__
#define __IPSI_NO_KASUMI_CFB_ENC__
#define __IPSI_NO_KASUMI_ECB_ENC__
#define __IPSI_NO_KASUMI_OFB_ENC__
#define __IPSI_NO_CORE_DES__
#define __IPSI_NO_RC4_ENC__
#define __IPSI_NO_RC4_DCR__
#define __IPSI_NO_DES_CBC_ENC__
#define __IPSI_NO_DES_CFB_ENC__
#define __IPSI_NO_DES_ECB_ENC__
#define __IPSI_NO_DES_OFB_ENC__
#define __IPSI_NO_DES_EDE_CFB_ENC__
#define __IPSI_NO_DES_EDE_CBC_ENC__
#define __IPSI_NO_DES_EDE_ECB_ENC__
#define __IPSI_NO_DES_EDE_OFB_ENC__
#define __IPSI_NO_DES_EDE3_CBC_ENC__
#define __IPSI_NO_DES_EDE3_CFB_ENC__
#define __IPSI_NO_DES_EDE3_ECB_ENC__
#define __IPSI_NO_DES_EDE3_OFB_ENC__
#define __IPSI_NO_DES_CBC_DCR__
#define __IPSI_NO_DES_CFB_DCR__
#define __IPSI_NO_DES_ECB_DCR__
#define __IPSI_NO_DES_OFB_DCR__
#define __IPSI_NO_DES_EDE_CBC_DCR__
#define __IPSI_NO_DES_EDE_CFB_DCR__
#define __IPSI_NO_DES_EDE_OFB_DCR__
#define __IPSI_NO_DES_EDE_ECB_DCR__
#define __IPSI_NO_DES_EDE3_CBC_DCR__
#define __IPSI_NO_DES_EDE3_CFB_DCR__
#define __IPSI_NO_DES_EDE3_ECB_DCR__
#define __IPSI_NO_DES_EDE3_OFB_DCR__
#define __IPSI_NO_RC2__

#define __IPSI_NO_CORE_AES_ENC__
#define __IPSI_NO_CORE_AES_DEC__

#define __IPSI_NO_KASUMI__
#define __IPSI_NO_KEY_WRAP_UNWRAP__

#define __IPSI_NO_KASUMI_OFB_ENC__
#define __IPSI_NO_KASUMI_CFB_ENC__
#define __IPSI_NO_KASUMI_CBC_ENC__
#define __IPSI_NO_KASUMI_ECB_ENC__
#define __IPSI_NO_OCTEON_IMPL__

#define __IPSI_NO_RNG__
#define __IPSI_NO_SEC_BIG_INT__
#define __CRYPTO_NO_SEC_LIST__

#define __IPSI_NO_SEC_LOG__
#define __IPSI_NO_CRYPTO_LOG__
#define __IPSI_NO_DEBUG_SWITCH__
#define __IPSI_NO_RSA_PSS_PAD__
#define __IPSI_NO_SAFE_API__

#define __IPSI_NO_AES128_ECB_DCR__
#define __IPSI_NO_AES192_ECB_DCR__
#define __IPSI_NO_AES256_ECB_DCR__
#define __IPSI_NO_AES128_CFB_DCR__
#define __IPSI_NO_AES128_OFB_DCR__
#define __IPSI_NO_AES192_CFB_DCR__
#define __IPSI_NO_AES192_OFB_DCR__
#define __IPSI_NO_AES256_CFB_DCR__
#define __IPSI_NO_AES256_OFB_DCR__

#define __IPSI_NO_AES128_ECB_ENC__
#define __IPSI_NO_AES192_ECB_ENC__
#define __IPSI_NO_AES256_ECB_ENC__
#define __IPSI_NO_AES128_CFB_ENC__
#define __IPSI_NO_AES128_OFB_ENC__
#define __IPSI_NO_AES192_CFB_ENC__
#define __IPSI_NO_AES192_OFB_ENC__
#define __IPSI_NO_AES256_CFB_ENC__
#define __IPSI_NO_AES256_OFB_ENC__

#define __IPSI_NO_ASYM_KEY_IMPL__
#define __IPSI_NO_ASYM_UTILS__

#define __IPSI_NO_SYM_CFB_DEC__
#define __IPSI_NO_SYM_CFB_ENC__
#define __IPSI_NO_SYM_CFB_MODE__
#define __IPSI_NO_SYM_ECB_DEC__
#define __IPSI_NO_SYM_ECB_ENC__
#define __IPSI_NO_SYM_ECB_MODE__
#define __IPSI_NO_SYM_OFB_DEC__
#define __IPSI_NO_SYM_OFB_ENC__
#define __IPSI_NO_SYM_OFB_MODE__
#define __IPSI_NO_CMAC_SM4__

/* 国密对称算法 */
#define IPSI_NO_SM4
#define IPSI_NO_SM4_ECB
#define IPSI_NO_SM4_CBC

#endif // __IPSI_HERT_REDUCED_CRYPTO_FOR_1381__

#if (defined(__IPSI_RSA_FIPS_COMPLIANCE__) || defined(__IPSI_FIPS_SELFTEST__) || defined(__IPSI_FIPS_SPECIFIC__))
#undef __IPSI_NO_DRBG__
#endif

#endif // IPSI_CRYPTO_BUILDCONF_H
