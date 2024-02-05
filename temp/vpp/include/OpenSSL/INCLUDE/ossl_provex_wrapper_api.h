/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Description
 * Author: VPP
 * Create: 2023-03-14
 * Notes: Notes
 */

#ifndef OSSL_PROVEX_WRAPPER_API_H
#define OSSL_PROVEX_WRAPPER_API_H

#include <openssl/crypto.h>
#include <openssl/core.h>
#include <openssl/core_dispatch.h>
#include <openssl/provider.h>

#include "ossl_provex_type_api.h"

#ifdef __cplusplus
extern "C" {
#endif

OSSLX_PROV_HANDLE OSSLX_install_rand_provider_by_method(
    const char *provname, const char *randname, const OSSLX_RAND_METH *meth);

OSSLX_PROV_HANDLE OSSLX_install_seed_provider_by_method(
    const char *provname, const char *seedname, const OSSLX_SEED_METH *meth);

int OSSLX_uninstall_provider_by_handle(OSSLX_PROV_HANDLE provhd);

int OSSLX_uninstall_all_provider(void);

#ifdef __cplusplus
}
#endif

#endif // OSSL_PROVEX_WRAPPER_API_H
