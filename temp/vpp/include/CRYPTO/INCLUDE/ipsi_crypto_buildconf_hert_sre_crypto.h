/* **********************************
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
FileName: ipsi_crypto_buildconf_hert_sre_crypto.h
Author: lipengzhang
Version: 1

Description: This file controls the build configurations of crypto for hert sreOS
 * Create: 2021-04-06
***************************************************************************** */
#ifndef IPSI_CRYPTO_BUILDCONF_HERT_SRE_CRYPTO_H
#define IPSI_CRYPTO_BUILDCONF_HERT_SRE_CRYPTO_H

#ifdef __IPSI_HERT_REDUE_FOR_SRE__

/* DES tailoring */
#define __IPSI_NO_CORE_DES__
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
#define __IPSI_NO_DSA_IFACE__
#define __IPSI_NO_DOUBLE_DES_DEC__
#define __IPSI_NO_DOUBLE_DES_ENC__

/* AES tailoring */
#define __IPSI_NO_CORE_AES_CCM__
#define __IPSI_NO_CORE_AES_GCM__
#define __IPSI_NO_AES_GCM_ENC__
#define __IPSI_NO_AES_GCM_DEC__
#define __IPSI_NO_AES_CCM_ENC__
#define __IPSI_NO_AES_CCM_DEC__
#define __IPSI_NO_AES128_CFB_ENC__
#define __IPSI_NO_AES128_OFB_ENC__
#define __IPSI_NO_AES192_CFB_ENC__
#define __IPSI_NO_AES192_OFB_ENC__
#define __IPSI_NO_AES256_CFB_ENC__
#define __IPSI_NO_AES256_OFB_ENC__
#define __IPSI_NO_AES128_OFB_DCR__
#define __IPSI_NO_AES192_CFB_DCR__
#define __IPSI_NO_AES128_CFB_DCR__
#define __IPSI_NO_AES192_OFB_DCR__
#define __IPSI_NO_AES256_CFB_DCR__
#define __IPSI_NO_AES256_OFB_DCR__
#define __IPSI_NO_AES128_ECB_ENC__
#define __IPSI_NO_AES192_ECB_ENC__
#define __IPSI_NO_AES256_ECB_ENC__
#define __IPSI_NO_AES128_ECB_DCR__
#define __IPSI_NO_AES192_ECB_DCR__
#define __IPSI_NO_AES256_ECB_DCR__

/* KASUMI tailoring */
#define __IPSI_NO_KASUMI__
#define __IPSI_NO_KASUMI_CBC_ENC__
#define __IPSI_NO_KASUMI_CFB_ENC__
#define __IPSI_NO_KASUMI_ECB_ENC__
#define __IPSI_NO_KASUMI_OFB_ENC__

/* RC4 tailoring */
#define __IPSI_NO_RC4__
#define __IPSI_NO_RC4_ENC__
#define __IPSI_NO_RC4_DCR__

/* RC2 tailoring */
#define __IPSI_NO_RC2__

/* BF tailoring */
#define __IPSI_NO_BF_ECB_ENC__
#define __IPSI_NO_BF_OFB_ENC__
#define __IPSI_NO_BF_CBC_ENC__
#define __IPSI_NO_BF_CFB_ENC__
#define __IPSI_NO_BF_ECB_DCR__
#define __IPSI_NO_BF_CBC_DCR__
#define __IPSI_NO_BF_CFB_DCR__
#define __IPSI_NO_BF_OFB_DCR__

/* Symmetric opmode tailoring */
#define __IPSI_NO_SYM_CFB_DEC__
#define __IPSI_NO_SYM_ECB_DEC__
#define __IPSI_NO_SYM_OFB_DEC__
#define __IPSI_NO_SYM_CTR_ENC_DEC__
#define __IPSI_NO_BLOWFISH__
#define __IPSI_NO_SYM_CFB_ENC__
#define __IPSI_NO_SYM_ECB_ENC__
#define __IPSI_NO_SYM_OFB_ENC__
#define __IPSI_NO_SYM_ECB_MODE__
#define __IPSI_NO_SYM_OFB_MODE__
#define __IPSI_NO_SYM_CFB_MODE__
#define __IPSI_NO_SYM_AES128_CFB_MODE__

/* Key wrap tailoring */
#define __IPSI_NO_KEY_WRAP_UNWRAP__
#define __IPSI_NO_SYM_KEY_GEN__
#define __IPSI_NO_SYM_IFACE__

/* Hash tailoring */
#define __IPSI_NO_MD4__
#define __IPSI_NO_MD5__
#define __IPSI_NO_SHA384__
#define __IPSI_NO_CORE_SHA384__
#define __IPSI_NO_CORE_SHA384_INTERFACE__
#define __IPSI_NO_SHA512__
#define __IPSI_NO_CORE_SHA512__
#define __IPSI_NO_CORE_SHA512_INTERFACE__
#define __IPSI_NO_MD5_SHA1__

/* KDF */
#define __IPSI_NO_KDF_SM3__
#define __IPSI_NO_KDF_SHA224__
#define __IPSI_NO_KDF_SHA256__
#define __IPSI_NO_KDF_SHA384__
#define __IPSI_NO_KDF_SHA512__

/* Dot16 KDF tailoring */
#define __IPSI_NO_DOT16_KDF__
#define __IPSI_NO_DOT16KDF_CMAC__
#define __IPSI_NO_DOT16KDF_SHA1__

/* HMAC tailoring */
#define __IPSI_NO_HMAC_MD4__
#define __IPSI_NO_HMAC_MD5__
#define __IPSI_NO_HMAC_COMMON_IFACE__
#define __IPSI_NO_HMAC_SM3__
#define __IPSI_NO_CORE_HMAC_SM3__
#define __IPSI_NO_CPLUS_HMAC_SM3__
#define __IPSI_NO_HMAC_SHA384__
#define __IPSI_NO_CORE_HMAC_SHA384__
#define __IPSI_NO_CPLUS_HMAC_SHA384__
#define __IPSI_NO_HMAC_SHA512__
#define __IPSI_NO_CORE_HMAC_SHA512__
#define __IPSI_NO_CPLUS_HMAC_SHA512__

/* CMAC tailoring */
#define __IPSI_NO_CMAC_TDES__

/* Small crypto macros */
#define __IPSI_NO_BN_ENC_DEC__
#define __IPSI_NO_SMALL_BN__
#define __IPSI_NO_SMALL_SHA256__
#define __IPSI_NO_SMALL_ES__
#define __IPSI_NO_SMALL_SSA__
#define __IPSI_NO_SMALL_KEY_ENC__
#define __IPSI_NO_SMALL_PKEY__

/* Asymmetric tailoring */
// RSA Encode/Decode
#define __IPSI_NO_RSA__
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

// Asymmetric RSA Encryption/Decryption Flag
#define __IPSI_NO_RSA_ENC_DEC__
#define __IPSI_NO_RSA_ENC__
#define __IPSI_NO_RSA_DEC__
#define __IPSI_NO_RSA_GEN__
#define __IPSI_NO_RSA_PAD__
#define __IPSI_NO_RSA_MGF__
#define __IPSI_NO_RSA_SIGN_WITH_PVT_EXP__
#define __IPSI_NO_RSA_PSS_PAD__

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
#define __IPSI_NO_BN__
#define __IPSI_NO_CORE_BN__
#define __IPSI_NO_OSSL_WRAP_BN__
#define __IPSI_NO_DSA__
#define __IPSI_NO_EC__
#define __IPSI_NO_EC_PARAM_POINTS__
#define __IPSI_NO_EC_POLY_CURVE__
#define __IPSI_NO_EC_BITARRAY__
#define __IPSI_NO_EC_GF__
#define __IPSI_NO_DH__
#define __IPSI_NO_DH_PARA__
#define __IPSI_NO_CORE_DH__
#define __IPSI_NO_ECDH__
#define __IPSI_NO_ECDSA__
#define __IPSI_NO_XBN__

/* log */
#define __IPSI_NO_ERROR_LOG__
#define __IPSI_NO_SEC_LOG__
#define __IPSI_NO_CRYPTO_LOG__

#define __IPSI_NO_ASYM_COMMON_IFACE__
#define __IPSI_NO_ASYM_KEY_IMPL__

/* 国密 */
#define __IPSI_NO_SM3__
#define __IPSI_NO_SM2__
#define __IPSI_NO_SM2DSA__
#define __IPSI_NO_SM2_ENC__
#define __IPSI_NO_SM2_KEP__
/* 国密对称算法 */
#define IPSI_NO_SM4
#define IPSI_NO_SM4_ECB
#define IPSI_NO_SM4_CBC
#define __IPSI_NO_CMAC_SM4__

/* random */
#define __IPSI_NO_RNG__
#define __IPSI_NO_RNG_SSL__
#define __IPSI_NO_RNG_COREIMPL__
#define __IPSI_NO_DRBG_LOCK__

/* Others */
#define __IPSI_NO_SET_IV__
#define __IPSI_NO_CRYPTO_ALG_UTILS__
#define __IPSI_NO_OCTEON_IMPL__
#define __CRYPTO_NO_SEC_LIST__
#define __IPSI_NO_OAEP_CLASS__
#define __IPSI_NO_DEBUG_SWITCH__
#define __IPSI_NO_LOCK__
#define __IPSI_NO_AVL__
#define __IPSI_NO_SEC_BIG_INT__
#define __IPSI_NO_SEC_DEF__
#define __IPSI_NO_PID__
#define __IPSI_NO_BITOP__
#define __IPSI_CRYPTO_NO_ASSERTS__
#define __IPSI_NO_SCRYPT
#define __IPSI_NO_AEAD__
#define __IPSI_NO_AEAD_ENC__
#define __IPSI_NO_AEAD_DEC__
#define __IPSI_NO_AEAD_IFACE__
#define __IPSI_NO_SAFE_API__
#define __IPSI_NO_EXPOSED_DIGEST__
#define __IPSI_DISABLE_DISPLAY_CRYPTO_VERSION__
#define __IPSI_NO_CTIME_LIB__
#define __IPSI_NO_MASK_GEN_CLASS__
#define __IPSI_NO_EC_UTILS__
#define __IPSI_NO_ASYM_UTILS__

#endif // __IPSI_HERT_REDUE_FOR_SRE__
#endif // IPSI_CRYPTO_BUILDCONF_HERT_SRE_CRYPTO_H
