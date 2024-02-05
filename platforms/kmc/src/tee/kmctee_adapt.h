/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: The optee and itrustee header files depend on the adaptation layer
 * Author: liwei
 * Create: 2021-05-03
 */

#ifndef KMC_SRC_TEE_KMCTEE_ADAPT_H
#define KMC_SRC_TEE_KMCTEE_ADAPT_H

#ifndef WSEC_COMPILE_ENABLE_ASYM
#define WSEC_COMPILE_ENABLE_ASYM 1
#endif

#ifdef WSEC_COMPILE_ITRUSTEE
#include "tee_defines.h"
#include "tee_log.h"
#include "tee_mem_mgmt_api.h"
#include "tee_crypto_api.h"
#include "tee_object_api.h"
#include "tee_property_api.h"
#include "tee_ext_api.h"
#include "tee_crypto_hal.h"

typedef size_t teesz_t;

#endif

#ifdef WSEC_COMPILE_OPTEE
#include "wsecv2_type.h"
#include "tee_api.h"
#include "tee_api_defines_extensions.h"
#include "tee_internal_api.h"

typedef WsecUint32 teesz_t;

#define tlogd DMSG
#define tloge EMSG
#define tlogi IMSG

#define TEE_ALG_INVALID 0x0
#define DERIVE_MAXSALT_LEN 60
#define TEE_ECC_CURVE_25519 0x00000200
#endif

#endif // KMC_SRC_TEE_KMCTEE_ADAPT_H
