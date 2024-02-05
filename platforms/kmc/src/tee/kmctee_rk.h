/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: KMC TA internal tool function
 * Author: liwei
 * Create: 2021-05-03
 */

#ifndef KMC_SRC_TEE_KMCTEE_RK_H
#define KMC_SRC_TEE_KMCTEE_RK_H

#include "kmctee_adapt.h"
#include "kmctee_status.h"
#include "kmctee_msg.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void TaCleanKmcSrk(void);

void TaUpdateKmcSrk(WsecUint8 *rkKey, WsecUint32 rkLen);

void TaGetKmcSrk(WsecUint8 *rkKey, WsecUint32 rkLen);

void TaSetHasGenSrk(void);

WsecBool TaGetHasGenSrk(void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // KMCTEE_RK_H
