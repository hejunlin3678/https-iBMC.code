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
#ifndef IPSI_CRYPTO_BUILDCONF_IWF_HERT_ANDROID_H
#define IPSI_CRYPTO_BUILDCONF_IWF_HERT_ANDROID_H

#ifdef __IPSI_HERT_REDUCED_CRYPTO__

#define __IPSI_NO_KDF_SM3__
#define __IPSI_NO_SM3__
#define __IPSI_NO_HMAC_SM3__
#define __IPSI_NO_SM2__
#define __IPSI_NO_SM2DSA__
#define __IPSI_NO_KDF_SHA256__
#define __IPSI_NO_KDF_SHA224__
#define __IPSI_NO_KDF_SHA384__
#define __IPSI_NO_KDF_SHA512__
#define __IPSI_NO_SCRYPT
#define __IPSI_NO_OSSL_WRAP_BN__
#define __IPSI_NO_RC2__
#define __IPSI_NO_AES_XCBC__
#define __IPSI_NO_KEY_WRAP_UNWRAP__
#define __IPSI_ONLY_SSL_EC_CURVE__

#define __IPSI_NO_SYM_CTR_ENC_DEC__
#define __IPSI_NO_SYM_IFACE__
#define __IPSI_NO_DES_EDE3_CBCM__
#define __IPSI_NO_RSA_SIGN_WITH_PVT_EXP__

/* Removing the Blowfish Core implementation as well */
#define __IPSI_NO_BLOWFISH__

#define __IPSI_NO_BF_ECB_DCR__
#define __IPSI_NO_BF_CBC_DCR__
#define __IPSI_NO_BF_OFB_DCR__
#define __IPSI_NO_BF_CFB_DCR__

#define __IPSI_NO_BF_ECB_ENC__
#define __IPSI_NO_BF_CBC_ENC__
#define __IPSI_NO_BF_CFB_ENC__
#define __IPSI_NO_BF_OFB_ENC__

#define __IPSI_NO_DOT16KDF_SHA1__
#define __IPSI_NO_CORE_AES__
#define __IPSI_NO_CORE_DES__
#define __IPSI_NO_HMAC_SHA224__
#define __IPSI_NO_CORE_SHA224__

#define __IPSI_NO_SAFE_API__

#ifdef __IPSI_HERT_REDUCED_NO_AEAD_NO_EC_CRYPTO__
#define __IPSI_NO_CORE_AES_GCM__
#define __IPSI_NO_AES_GCM_ENC__
#define __IPSI_NO_AES_GCM_DEC__
#define __IPSI_NO_CORE_AES_CCM__
#define __IPSI_NO_AES_CCM_ENC__
#define __IPSI_NO_AES_CCM_DEC__
#define __IPSI_NO_AEAD_ENC__
#define __IPSI_NO_AEAD__
#define __IPSI_NO_AEAD_DEC__
#define __IPSI_NO_AEAD_IFACE__
#define __IPSI_NO_EC_PARAM_POINTS__
#endif

#ifdef __IPSI_HERT_CRYPTO_REDUCED_PPC8321__

#define __CRYPTO_NO_SEC_LIST__
#define __IPSI_NO_SEC_LOG__
#define __IPSI_NO_DEBUG_SWITCH__

#endif

#define __IPSI_NO_EC_POLY_CURVE__
#define __IPSI_NO_EC_GF__

/* 国密对称算法 */
#define IPSI_NO_SM4
#define IPSI_NO_SM4_ECB
#define IPSI_NO_SM4_CBC
#define __IPSI_NO_CMAC_SM4__

#endif // __IPSI_HERT_REDUCED_CRYPTO__

#endif // IPSI_CRYPTO_BUILDCONF_IWF_HERT_ANDROID_H
