/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: KMC malloc free memcmp default function.
 * Author: yangchen
 * Create: 2022-10-28
 */

#ifndef KMC_SRC_COMMON_WSECV2_DEFAULT_CB_H
#define KMC_SRC_COMMON_WSECV2_DEFAULT_CB_H

#include "wsecv2_type.h"
#include "securec.h"

#ifdef __cplusplus
extern "C" {
#endif

WsecVoid *KmcDefaultMalloc(size_t size);

WsecVoid KmcDefaultFree(WsecVoid *memBuff);

int KmcDefaultMemCmp(const WsecVoid *buffA, const WsecVoid *buffB, size_t count);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* KMC_SRC_COMMON_WSECV2_DEFAULT_CB_H */
