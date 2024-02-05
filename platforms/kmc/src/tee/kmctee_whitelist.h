/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Header file dependency of TA entry function
 * Author: liwei
 * Create: 2021-05-03
 */

#ifndef KMC_SRC_TEE_KMCTEE_WHITELIST_H
#define KMC_SRC_TEE_KMCTEE_WHITELIST_H

#include "kmctee_adapt.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

TEE_Result SetAllowTaCaCaller(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif // KMC_SRC_TEE_KMCTEE_WHITELIST_H
