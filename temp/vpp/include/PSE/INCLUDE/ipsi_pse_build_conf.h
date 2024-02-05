/* **********************************
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
FileName: ipsi_pse_build_conf.h
Author: Sreenivasulu D
Version: 1

Description: This file controls the build configurations of PSE.
 * Create: 2012-01-04
********************************** */

#ifndef IPSI_PSE_BUILD_CONF_H
#define IPSI_PSE_BUILD_CONF_H

#ifdef __IPSI_WLAN_SMALL_PSE__

// Disables PKI feature
#ifndef IPSI_SEC_NO_PKI
#define IPSI_SEC_NO_PKI
#endif

// Disables PSE features which are not used in SSL
#ifndef PSE_SSL
#define PSE_SSL
#endif

#ifndef IPSI_SEC_NO_PKI
#define IPSI_SEC_NO_PKI
#endif

#ifndef IPSI_NO_WPKI
#define IPSI_NO_WPKI
#endif

#ifndef __SEC_NO_SCEP__
#define __SEC_NO_SCEP__
#endif

#ifndef __IPSI_SEC_NO_SCEP__
#define __IPSI_SEC_NO_SCEP__
#endif

#ifndef __SEC_NO_OCSP__
#define __SEC_NO_OCSP__
#endif

#ifndef __SEC_PKI_NO_OCSP__
#define __SEC_PKI_NO_OCSP__
#endif

#ifndef __IPSI_SEC_NO_CERT_BUNDLE__
#define __IPSI_SEC_NO_CERT_BUNDLE__
#endif

#ifndef __IPSI_NO_CRYPTO_LOG__
#define __IPSI_NO_CRYPTO_LOG__
#endif

#ifndef IPSI_NO_ERROR_LOG
#define IPSI_NO_ERROR_LOG
#endif

#ifndef SEC_PKI_NO_LOCK
#define SEC_PKI_NO_LOCK
#endif

#ifdef __IPSI_CMPV2__
#undef __IPSI_CMPV2__
#endif

#ifdef __SEC_PKI__
#undef __SEC_PKI__
#endif

#ifndef __IPSI_NO_SM2__
#define __IPSI_NO_SM2__
#define __IPSI_NO_SM2DSA__
#endif

#endif // __IPSI_WLAN_SMALL_PSE__

#ifdef __IPSI_NO_EC__

#ifndef __IPSI_NO_SM2__
#define __IPSI_NO_SM2__
#endif

#ifndef __IPSI_NO_SM2DSA__
#define __IPSI_NO_SM2DSA__
#endif

#endif

#ifdef __IPSI_VRP_SMALL_PSE__
#define __IPSI_SEC_NO_SCEP__
#define __SEC_PKI_NO_OCSP__
#define __SEC_NO_OCSP__
#define __SEC_NO_SCEP__
#define IPSI_NO_WPKI
#define __IPSI_NO_EC__
#define __IPSI_NO_CRMF__
#define __IPSI_NO_CMP__
#define __IPSI_NO_PKCS10__
#define IPSI_SEC_NO_PKI
#define __IPSI_SEC_NO_CERT_BUNDLE__
#undef __SEC_PKI__
#undef __IPSI_CMPV2__
#define __IPSI_LOG_LVL__ 4
#define __LOG_LVL__ 4
#define __IPSI_NO_CRYPTO_LOG__
#define __IPSI_NO_SM2__

#endif // __IPSI_VRP_SMALL_PSE__

#ifdef __IPSI_IWF_SMALL_LIB__
#define __IPSI_SEC_NO_SCEP__
#define __SEC_PKI_NO_OCSP__
#define __SEC_NO_OCSP__
#define __SEC_NO_SCEP__
#define IPSI_NO_WPKI
#define __IPSI_NO_EC__
#define __IPSI_NO_CRMF__
#define __IPSI_NO_CMP__
#define __IPSI_NO_PKCS10__
#define IPSI_SEC_NO_PKI
#define __IPSI_SEC_NO_CERT_BUNDLE__
#undef __SEC_PKI__
#undef __IPSI_CMPV2__
#define __IPSI_LOG_LVL__ 5
#define __LOG_LVL__ 5
#define __IPSI_NO_CRYPTO_LOG__
#define __IPSI_NO_SM2__
#endif // __IPSI_IWF_SMALL_LIB__

#define MAKE_COMPLIER_HAPPY 1

#endif // IPSI_PSE_BUILD_CONF_H
