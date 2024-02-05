/* **********************************
 * Copyright (c) Huawei Technologies Co., Ltd. 2010-2019. All rights reserved.
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
#ifndef IPSI_CRYPTO_BUILDCONF_MBSC_SMALL_CRYPTO_H
#define IPSI_CRYPTO_BUILDCONF_MBSC_SMALL_CRYPTO_H

#ifdef __IPSI_MBSC_SMALL_CRYPTO_VXWORKS_WRLINUX_NO_DRBG__

#define __IPSI_NO_DRBG__

/* Symmetric core implementation algorithm tailoring */
#define __IPSI_NO_DOUBLE_DES_DEC__
#define __IPSI_NO_KASUMI__
#define __IPSI_NO_DOUBLE_DES_ENC__
#define __IPSI_NO_RC2__

/* Symmetric encryption algorithm tailoring through crypto_func.c */
#define __IPSI_NO_DES_ECB_ENC__
#define __IPSI_NO_DES_CBC_ENC__
#define __IPSI_NO_DES_CFB_ENC__
#define __IPSI_NO_DES_OFB_ENC__
#define __IPSI_NO_DES_EDE_ECB_ENC__
#define __IPSI_NO_DES_EDE_CBC_ENC__
#define __IPSI_NO_DES_EDE3_ECB_ENC__
#define __IPSI_NO_DES_EDE3_CBC_ENC__
#define __IPSI_NO_DES_EDE3_CFB_ENC__
#define __IPSI_NO_DES_EDE3_OFB_ENC__
#define __IPSI_NO_DES_EDE_CFB_ENC__
#define __IPSI_NO_DES_EDE_OFB_ENC__
#define __IPSI_NO_AES128_CFB_ENC__
#define __IPSI_NO_AES128_OFB_ENC__
#define __IPSI_NO_AES192_CFB_ENC__
#define __IPSI_NO_AES192_OFB_ENC__
#define __IPSI_NO_AES256_CFB_ENC__
#define __IPSI_NO_AES256_OFB_ENC__
#define __IPSI_NO_RC2__
#define __IPSI_NO_KASUMI_CBC_ENC__
#define __IPSI_NO_KASUMI_CFB_ENC__
#define __IPSI_NO_KASUMI_ECB_ENC__
#define __IPSI_NO_KASUMI_OFB_ENC__
#define __IPSI_NO_RC4_ENC__

#define __IPSI_NO_BF_ECB_ENC__
#define __IPSI_NO_BF_OFB_ENC__
#define __IPSI_NO_BF_CBC_ENC__
#define __IPSI_NO_BF_CFB_ENC__

/* Symmetric decryption algorithm tailoring through crypto_func.c */
#define __IPSI_NO_DES_ECB_DCR__
#define __IPSI_NO_DES_CBC_DCR__
#define __IPSI_NO_DES_CFB_DCR__
#define __IPSI_NO_DES_OFB_DCR__
#define __IPSI_NO_DES_EDE_ECB_DCR__
#define __IPSI_NO_DES_EDE_CFB_DCR__
#define __IPSI_NO_DES_EDE_OFB_DCR__
#define __IPSI_NO_DES_EDE_CBC_DCR__
#define __IPSI_NO_DES_EDE3_ECB_DCR__
#define __IPSI_NO_DES_EDE3_CBC_DCR__
#define __IPSI_NO_DES_EDE3_CFB_DCR__
#define __IPSI_NO_DES_EDE3_OFB_DCR__
#define __IPSI_NO_AES128_OFB_DCR__
#define __IPSI_NO_AES192_CFB_DCR__
#define __IPSI_NO_AES128_CFB_DCR__
#define __IPSI_NO_AES192_OFB_DCR__
#define __IPSI_NO_AES256_CFB_DCR__
#define __IPSI_NO_AES256_OFB_DCR__
#define __IPSI_NO_RC2__
#define __IPSI_NO_RC4_DCR__

#define __IPSI_NO_BF_ECB_DCR__
#define __IPSI_NO_BF_CBC_DCR__
#define __IPSI_NO_BF_CFB_DCR__
#define __IPSI_NO_BF_OFB_DCR__

/* Symmetric opmode tailoring */
#define __IPSI_NO_SYM_CFB_DEC__
#define __IPSI_NO_SYM_CFB_ENC__
#define __IPSI_NO_SYM_OFB_ENC__
#define __IPSI_NO_SYM_OFB_DEC__

/* Removing the Blowfish Core implementation as well */
#define __IPSI_NO_BLOWFISH__

/* XCBC tailoring */
#define __IPSI_NO_AES_XCBC__
#define __IPSI_NO_XCBC_AES__

/* Key wrap tailoring */
#define __IPSI_NO_KEY_WRAP_UNWRAP__

/* Hash tailoring */
#define __IPSI_NO_MD4__

/* Dot16 KDF tailoring */
#define __IPSI_NO_DOT16_KDF__

#define __IPSI_NO_SHA224__
#define __IPSI_NO_SHA256__
#define __IPSI_NO_SHA384__
#define __IPSI_NO_SHA512__

#define __IPSI_NO_CPLUS_SHA224__
#define __IPSI_NO_CPLUS_SHA384__
#define __IPSI_NO_CPLUS_SHA512__
#define __IPSI_NO_CORE_SHA224__
#define __IPSI_NO_CORE_SHA384__
#define __IPSI_NO_CORE_SHA512__

/* HMAC tailoring */
#define __IPSI_NO_HMAC_MD4__
#define __IPSI_NO_HMAC_MD5__
#define __IPSI_NO_HMAC_SHA1__
#define __IPSI_NO_CORE_HMAC_SHA224__
#define __IPSI_NO_CORE_HMAC_SHA384__
#define __IPSI_NO_CORE_HMAC_SHA512__
#define __IPSI_NO_CPLUS_HMAC_SHA224__
#define __IPSI_NO_CPLUS_HMAC_SHA384__
#define __IPSI_NO_CPLUS_HMAC_SHA512__
#define __IPSI_NO_HMAC_SHA224__
#define __IPSI_NO_HMAC_SHA384__
#define __IPSI_NO_HMAC_SHA512__

/* Asymmetric tailoring */
/* Small crypto macros */
#define __IPSI_NO_BN_ENC_DEC__
#define __IPSI_NO_XBN__
#define __IPSI_NO_SMALL_BN__

/* CMAC tailoring */
#define __IPSI_NO_CMAC__
#define __IPSI_NO_CMAC_AES__
#define __IPSI_NO_CMAC_TDES__

#define __IPSI_NO_SMALL_SHA256__
#define __IPSI_NO_SMALL_ES__
#define __IPSI_NO_SMALL_SSA__
#define __IPSI_NO_SMALL_KEY_ENC__
#define __IPSI_NO_SMALL_PKEY__
#define __IPSI_NO_KEY_ENC_DCR__
#define __IPSI_NO_ASYM_KEY_GEN__

/* RSA Encode/Decode */
#define __IPSI_NO_RSA_KEY_ENC_DEC__
#define __IPSI_NO_RSA_KEY_ENC__
#define __IPSI_NO_RSA_PUB_KEY_ENC__
#define __IPSI_NO_RSA_PVT_KEY_ENC__
#define __IPSI_NO_RSA_PUB_KEY_DEC__
#define __IPSI_NO_RSA_KEY_DEC__
#define __IPSI_NO_RSA_PUB_KEY_DEC__
#define __IPSI_NO_RSA_PVT_KEY_DEC__
#define __IPSI_NO_RSA_KEY_PAIR_DEC__
#define __IPSI_NO_RSA_KEY_PAIR_ENC__

/* Asymmetric RSA Encryption/Decryption Flag */
#define __IPSI_NO_RSA_ENC_DEC__
#define __IPSI_NO_RSA_ENC__
#define __IPSI_NO_RSA_DEC__
#define __IPSI_NO_RSA_GEN__
#define __IPSI_NO_RSA_PAD__

/* Asymmetric Key Flag */
#define __IPSI_NO_ASYM_KEY__
#define __IPSI_NO_ASYM_RSA_KEY__
#define __IPSI_NO_ASYM_DSA_KEY__
#define __IPSI_NO_ASYM_EC_KEY__
#define __IPSI_NO_ASYM_DH_KEY__
#define __IPSI_RSA_NOEXTENDEDKEYPAIR__
#define __IPSI_RSA_NOTIMER__
#define __IPSI_NO_ASYM_ECDH_KEY__
#define __IPSI_NO_ASYM_KEY_GEN__
#define __IPSI_NO_PKEY__
#define __IPSI_NO_KEY_ENC_DCR__

/* Asymmetric Sign Verify Flag */
#define __IPSI_NO_ASYM_SIGN__
#define __IPSI_NO_ASYM_RSA_SIGN__
#define __IPSI_NO_ASYM_DSA_SIGN__
#define __IPSI_NO_ASYM_ECDSA_SIGN__
#define __IPSI_NO_ASYM_SM2DSA_SIGN__
#define __IPSI_NO_ASYM_ECDSA_SIGN_VRF__
#define __IPSI_NO_ASYM_SM2DSA_SIGN_VRF__
#define __IPSI_NO_ASYM_SSL_SIGN_VRF__
#define __IPSI_NO_ASYM_SIGN_VRF__
#define __IPSI_NO_ASYM_SIGN_VER__
#define __IPSI_NO_ASYM_RSA_SIGN_VRF__
#define __IPSI_NO_ASYM_DSA_SIGN_VRF__

/* Asymmetric others */
#define __IPSI_NO_RSA__
#define __IPSI_NO_DSA__
#define __IPSI_NO_DH__
#define __IPSI_NO_EC__
#define __IPSI_NO_CORE_BN__
#define __IPSI_NO_CORE_DH__
#define __IPSI_NO_ECDH__
#define __IPSI_NO_ECDSA__
#define __IPSI_NO_XBN__
#define __IPSI_NO_DH_PARA__
#define __IPSI_NO_RSA_MGF__
#define __IPSI_NO_EC_POLY_CURVE__

/* Others */
#define __IPSI_NO_FEEDBACK__
#define __IPSI_NO_SET_IV__
#define __IPSI_NO_CRYPTO_ALG_UTILS__
#define __IPSI_NO_OCTEON_IMPL__

#define __IPSI_NO_CRYPTO_LOG__
#define __CRYPTO_NO_SEC_LIST__
#define __IPSI_NO_CORE_DES__
#define __IPSI_NO_SYM_KEY_GEN__
#define __IPSI_NO_OAEP_CLASS__
#define __IPSI_NO_ERROR_LOG__
#define __IPSI_NO_BIN_LOG__
#define __IPSI_NO_SEC_LOG__

#define __IPSI_NO_LOCK__
#define __IPSI_NO_BN__
#define __IPSI_NO_CRYPTO_UTILS__
#define __IPSI_NO_AVL__
#define __IPSI_NO_SEC_BIG_INT__
#define __IPSI_NO_HMAC__
#define __IPSI_NO_BN__

#define __IPSI_NO_DOT16KDF_CMAC__
#define __IPSI_NO_DOT16KDF_SHA1__
#define __IPSI_NO_CPLUS_HMAC_SHA256__
#define __IPSI_NO_CORE_AES_ENC__
#define __IPSI_NO_SEC_DEF__
#define __IPSI_NO_PID__
#define __IPSI_NO_BITOP__
#define __IPSI_NO_CORE_SHA256_INTERFACE__
#define __IPSI_NO_ASYM_COMMON_IFACE__
#define __IPSI_NO_HMAC_COMMON_IFACE__
#define __IPSI_CRYPTO_NO_ASSERTS__
#define __IPSI_NO_EC_PARAM_POINTS__
#define __IPSI_NO_CRYPTO_FUNC__
#define __IPSI_NO_MEM_CALLOC__
#define __IPSI_NO_ASYM_KEY_IMPL__
#define __IPSI_NO_BLOWFISH__

#define __IPSI_NO_KDF_SM3__
#define __IPSI_NO_SM3__
#define __IPSI_NO_HMAC_SM3__
#define __IPSI_NO_SM2__
#define __IPSI_NO_SM2DSA__
#define __IPSI_NO_SCRYPT
#define __IPSI_NO_OSSL_WRAP_BN__
#define __IPSI_NO_KDF_SHA224__
#define __IPSI_NO_KDF_SHA256__
#define __IPSI_NO_KDF_SHA384__
#define __IPSI_NO_KDF_SHA512__

#define __IPSI_NO_CORE_AES_GCM__
#define __IPSI_NO_AES_GCM_ENC__
#define __IPSI_NO_AES_GCM_DEC__
#define __IPSI_NO_CORE_AES_CCM__
#define __IPSI_NO_AEAD_DEC__
#define __IPSI_NO_AEAD__
#define __IPSI_NO_AEAD_IFACE__
#define __IPSI_NO_AES_CCM_ENC__
#define __IPSI_NO_AES_CCM_DEC__
#define __IPSI_NO_AEAD_ENC__

#define __IPSI_NO_SYM_CTR_ENC_DEC__
#define __IPSI_NO_SYM_IFACE__
#define __IPSI_NO_RSA_SIGN_WITH_PVT_EXP__
#define __IPSI_NO_RNG__
#define __IPSI_NO_RSA_PSS_PAD__
#define __IPSI_NO_SAFE_API__
#define __IPSI_NO_AES256_CBC_ENC__
#define __IPSI_NO_AES192_CBC_ENC__
#define __IPSI_NO_AES128_CBC_ENC__
#define __IPSI_NO_AES256_CBC_DCR__
#define __IPSI_NO_AES192_CBC_DCR__
#define __IPSI_NO_AES128_CBC_DCR__
#define __IPSI_NO_EXPOSED_DIGEST__
#define __IPSI_DISABLE_DISPLAY_CRYPTO_VERSION__

/* 国密对称算法 */
#define IPSI_NO_SM4
#define IPSI_NO_SM4_ECB
#define IPSI_NO_SM4_CBC
#define __IPSI_NO_CMAC_SM4__

#endif // __IPSI_MBSC_SMALL_CRYPTO_VXWORKS_WRLINUX_NO_DRBG__

#ifdef __IPSI_MBSC_SMALL_CRYPTO_VXWORKS_WRLINUX__

/* Symmetric core implementation algorithm tailoring */
#define __IPSI_NO_DOUBLE_DES_DEC__
#define __IPSI_NO_DOUBLE_DES_ENC__
#define __IPSI_NO_KASUMI__
#define __IPSI_NO_RC2__

/* Symmetric encryption algorithm tailoring through crypto_func.c */
#define __IPSI_NO_DES_ECB_ENC__
#define __IPSI_NO_DES_CBC_ENC__
#define __IPSI_NO_DES_CFB_ENC__
#define __IPSI_NO_DES_OFB_ENC__
#define __IPSI_NO_DES_EDE_ECB_ENC__
#define __IPSI_NO_DES_EDE_CBC_ENC__
#define __IPSI_NO_DES_EDE_CFB_ENC__
#define __IPSI_NO_DES_EDE_OFB_ENC__
#define __IPSI_NO_DES_EDE3_ECB_ENC__
#define __IPSI_NO_DES_EDE3_CBC_ENC__
#define __IPSI_NO_DES_EDE3_CFB_ENC__
#define __IPSI_NO_DES_EDE3_OFB_ENC__
#define __IPSI_NO_AES128_CFB_ENC__
#define __IPSI_NO_AES128_OFB_ENC__
#define __IPSI_NO_AES192_CFB_ENC__
#define __IPSI_NO_AES192_OFB_ENC__
#define __IPSI_NO_AES256_CFB_ENC__
#define __IPSI_NO_AES256_OFB_ENC__
#define __IPSI_NO_KASUMI_ECB_ENC__
#define __IPSI_NO_RC2__
#define __IPSI_NO_KASUMI_CBC_ENC__
#define __IPSI_NO_KASUMI_CFB_ENC__
#define __IPSI_NO_KASUMI_OFB_ENC__
#define __IPSI_NO_RC4_ENC__
#define __IPSI_NO_BF_CBC_ENC__
#define __IPSI_NO_BF_ECB_ENC__
#define __IPSI_NO_BF_CFB_ENC__
#define __IPSI_NO_BF_OFB_ENC__

/* Symmetric decryption algorithm tailoring through crypto_func.c */
#define __IPSI_NO_DES_ECB_DCR__
#define __IPSI_NO_DES_CBC_DCR__
#define __IPSI_NO_DES_CFB_DCR__
#define __IPSI_NO_DES_OFB_DCR__
#define __IPSI_NO_DES_EDE_CBC_DCR__
#define __IPSI_NO_DES_EDE_CFB_DCR__
#define __IPSI_NO_DES_EDE_OFB_DCR__
#define __IPSI_NO_DES_EDE3_ECB_DCR__
#define __IPSI_NO_DES_EDE3_CBC_DCR__
#define __IPSI_NO_DES_EDE3_CFB_DCR__
#define __IPSI_NO_DES_EDE_ECB_DCR__
#define __IPSI_NO_DES_EDE3_OFB_DCR__
#define __IPSI_NO_AES128_CFB_DCR__
#define __IPSI_NO_AES128_OFB_DCR__
#define __IPSI_NO_AES192_CFB_DCR__
#define __IPSI_NO_AES192_OFB_DCR__
#define __IPSI_NO_AES256_CFB_DCR__
#define __IPSI_NO_AES256_OFB_DCR__
#define __IPSI_NO_RC2__
#define __IPSI_NO_RC4_DCR__

#define __IPSI_NO_BF_CBC_DCR__
#define __IPSI_NO_BF_CFB_DCR__
#define __IPSI_NO_BF_OFB_DCR__
#define __IPSI_NO_BF_ECB_DCR__

/* Removing the Blowfish Core implementation as well */
#define __IPSI_NO_BLOWFISH__

/* Symmetric opmode tailoring */
#define __IPSI_NO_SYM_CFB_DEC__
#define __IPSI_NO_SYM_CFB_ENC__
#define __IPSI_NO_SYM_OFB_ENC__
#define __IPSI_NO_SYM_OFB_DEC__

/* XCBC tailoring */
#define __IPSI_NO_AES_XCBC__
#define __IPSI_NO_XCBC_AES__

/* Key wrap tailoring */
#define __IPSI_NO_KEY_WRAP_UNWRAP__

/* Dot16 KDF tailoring */
#define __IPSI_NO_DOT16_KDF__

/* Hash tailoring */
#define __IPSI_NO_MD4__

/* HMAC tailoring */
#define __IPSI_NO_HMAC_MD4__
#define __IPSI_NO_HMAC_MD5__

/* CMAC tailoring */
#define __IPSI_NO_CMAC__
#define __IPSI_NO_CMAC_AES__
#define __IPSI_NO_CMAC_TDES__

/* Asymmetric tailoring */
/* Small crypto macros */
#define __IPSI_NO_BN_ENC_DEC__
#define __IPSI_NO_XBN__
#define __IPSI_NO_SMALL_BN__

#define __IPSI_NO_SMALL_ES__
#define __IPSI_NO_SMALL_SSA__
#define __IPSI_NO_SMALL_KEY_ENC__
#define __IPSI_NO_SMALL_PKEY__
#define __IPSI_NO_KEY_ENC_DCR__
#define __IPSI_NO_ASYM_KEY_GEN__

/* Asymmetric RSA Encryption/Decryption Flag */
#define __IPSI_NO_RSA_ENC_DEC__
#define __IPSI_NO_RSA_ENC__
#define __IPSI_NO_RSA_DEC__
#define __IPSI_NO_RSA_PAD__
#define __IPSI_NO_RSA_GEN__

/* RSA Encode/Decode */
#define __IPSI_NO_RSA_KEY_ENC__
#define __IPSI_NO_RSA_KEY_ENC_DEC__
#define __IPSI_NO_RSA_PUB_KEY_ENC__
#define __IPSI_NO_RSA_PVT_KEY_ENC__
#define __IPSI_NO_RSA_PUB_KEY_DEC__
#define __IPSI_NO_RSA_KEY_DEC__
#define __IPSI_NO_RSA_KEY_PAIR_ENC__
#define __IPSI_NO_RSA_PUB_KEY_DEC__
#define __IPSI_NO_RSA_PVT_KEY_DEC__
#define __IPSI_NO_RSA_KEY_PAIR_DEC__

/* Asymmetric Key Flag */
#define __IPSI_NO_ASYM_KEY__
#define __IPSI_NO_ASYM_RSA_KEY__
#define __IPSI_NO_PKEY__
#define __IPSI_NO_ASYM_EC_KEY__
#define __IPSI_NO_ASYM_DSA_KEY__
#define __IPSI_NO_ASYM_DH_KEY__
#define __IPSI_NO_ASYM_ECDH_KEY__
#define __IPSI_NO_ASYM_KEY_GEN__
#define __IPSI_NO_KEY_ENC_DCR__
#define __IPSI_RSA_NOEXTENDEDKEYPAIR__
#define __IPSI_RSA_NOTIMER__

/* Asymmetric Sign Verify Flag */
#define __IPSI_NO_ASYM_RSA_SIGN__
#define __IPSI_NO_ASYM_SIGN__
#define __IPSI_NO_ASYM_DSA_SIGN__
#define __IPSI_NO_ASYM_SM2DSA_SIGN__
#define __IPSI_NO_ASYM_ECDSA_SIGN__
#define __IPSI_NO_ASYM_SIGN_VRF__
#define __IPSI_NO_ASYM_SIGN_VER__
#define __IPSI_NO_ASYM_RSA_SIGN_VRF__
#define __IPSI_NO_ASYM_DSA_SIGN_VRF__
#define __IPSI_NO_ASYM_SM2DSA_SIGN_VRF__
#define __IPSI_NO_ASYM_ECDSA_SIGN_VRF__
#define __IPSI_NO_ASYM_SSL_SIGN_VRF__

/* Asymmetric others */
#define __IPSI_NO_RSA__
#define __IPSI_NO_DSA__
#define __IPSI_NO_DH__
#define __IPSI_NO_EC__
#define __IPSI_NO_ECDH__
#define __IPSI_NO_XBN__
#define __IPSI_NO_CORE_BN__
#define __IPSI_NO_CORE_DH__
#define __IPSI_NO_DH_PARA__
#define __IPSI_NO_RSA_MGF__
#define __IPSI_NO_EC_POLY_CURVE__
#define __IPSI_NO_ECDSA__

/* Others */
#define __IPSI_NO_FEEDBACK__
#define __IPSI_NO_SET_IV__
#define __IPSI_NO_CRYPTO_ALG_UTILS__
#define __IPSI_NO_OCTEON_IMPL__

#define __IPSI_NO_SYM_KEY_GEN__
#define __IPSI_NO_OAEP_CLASS__
#define __IPSI_NO_ERROR_LOG__
#define __IPSI_NO_BIN_LOG__
#define __IPSI_NO_SEC_LOG__
#define __IPSI_NO_CRYPTO_LOG__
#define __CRYPTO_NO_SEC_LIST__
#define __IPSI_NO_CORE_DES__

#define __IPSI_NO_LOCK__
#define __IPSI_NO_BN__
#define __IPSI_NO_CRYPTO_UTILS__
#define __IPSI_NO_AVL__
#define __IPSI_NO_SEC_BIG_INT__

#define __IPSI_NO_BN__

#define __IPSI_NO_DOT16KDF_CMAC__
#define __IPSI_NO_DOT16KDF_SHA1__

#define __IPSI_NO_CORE_AES_ENC__

#define __IPSI_NO_ASYM_COMMON_IFACE__
#define __IPSI_NO_HMAC_COMMON_IFACE__
#define __IPSI_CRYPTO_NO_ASSERTS__
#define __IPSI_NO_EC_PARAM_POINTS__
#define __IPSI_NO_SEC_DEF__
#define __IPSI_NO_PID__
#define __IPSI_NO_BITOP__
#define __IPSI_NO_CRYPTO_FUNC__
#define __IPSI_NO_MEM_CALLOC__
#define __IPSI_NO_ASYM_KEY_IMPL__
#define __IPSI_NO_BLOWFISH__

#define __IPSI_NO_KDF_SM3__
#define __IPSI_NO_SM3__
#define __IPSI_NO_KDF_SHA224__
#define __IPSI_NO_SM2__
#define __IPSI_NO_HMAC_SM3__
#define __IPSI_NO_SM2DSA__
#define __IPSI_NO_KDF_SHA256__
#define __IPSI_NO_KDF_SHA384__
#define __IPSI_NO_KDF_SHA512__
#define __IPSI_NO_SCRYPT
#define __IPSI_NO_OSSL_WRAP_BN__

#define __IPSI_NO_AES_GCM_ENC__
#define __IPSI_NO_CORE_AES_GCM__
#define __IPSI_NO_AES_GCM_DEC__
#define __IPSI_NO_CORE_AES_CCM__
#define __IPSI_NO_AES_CCM_ENC__
#define __IPSI_NO_AES_CCM_DEC__
#define __IPSI_NO_AEAD_ENC__
#define __IPSI_NO_AEAD_DEC__
#define __IPSI_NO_AEAD__
#define __IPSI_NO_AEAD_IFACE__

#define __IPSI_NO_RSA_SIGN_WITH_PVT_EXP__
#define __IPSI_NO_RNG__
#define __IPSI_NO_AES256_CBC_ENC__
#define __IPSI_NO_AES192_CBC_ENC__
#define __IPSI_NO_AES128_CBC_ENC__
#define __IPSI_NO_AES256_CBC_DCR__
#define __IPSI_NO_AES192_CBC_DCR__
#define __IPSI_NO_AES128_CBC_DCR__

#define __IPSI_DISABLE_DISPLAY_CRYPTO_VERSION__
#define __IPSI_NO_RSA_PSS_PAD__
#define __IPSI_NO_SAFE_API__

/* 国密对称算法 */
#define IPSI_NO_SM4
#define IPSI_NO_SM4_ECB
#define IPSI_NO_SM4_CBC
#define __IPSI_NO_CMAC_SM4__

#endif // __IPSI_MBSC_SMALL_CRYPTO_VXWORKS_WRLINUX__

#endif // IPSI_CRYPTO_BUILDCONF_MBSC_SMALL_CRYPTO_H
