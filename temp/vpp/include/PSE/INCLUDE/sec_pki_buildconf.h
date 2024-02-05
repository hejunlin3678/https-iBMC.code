/* **********************************
 * Copyright (c) Huawei Technologies Co., Ltd. 2008-2019. All rights reserved.
 * FileName: sec_pki_buildconf.h
 * Author: Prabhat Kumar Gopalika
 * Version: 1

 * Description: This file controls the build configurations of PKI.
 * Create: 2008-07-08
********************************** */
#ifndef SEC_PKI_BUILDCONF_H
#define SEC_PKI_BUILDCONF_H

#include "ipsi_pse_build_conf.h"

#ifndef IPSI_SEC_NO_PKI

// Enable Peer cert cache
#ifdef __SEC_PKI_NO_PEER_CACHE__
#undef __SEC_PKI_NO_PEER_CACHE__
#endif

// Enable multiple local cert support
#ifdef __SEC_PKI_NO_MULTIPLE_LOCAL_CERT__
#undef __SEC_PKI_NO_MULTIPLE_LOCAL_CERT__
#endif

// Enable Preshared Peer cert
#ifdef __SEC_PKI_NO_PRESHARED_PEER_CERT__
#undef __SEC_PKI_NO_PRESHARED_PEER_CERT__
#endif

// Enable certificate bundle
#ifdef __IPSI_SEC_NO_CERT_BUNDLE__
#undef __IPSI_SEC_NO_CERT_BUNDLE__
#endif

#endif

#endif // IPSI_SEC_NO_PKI
