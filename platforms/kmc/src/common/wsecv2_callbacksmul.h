/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description: KMC internal interfaces are not open to external systems.
 * Author: yangchen
 * Create: 2018-11-08
 */

#ifndef KMC_SRC_COMMON_WSECV2_CALLBACKSMUL_H
#define KMC_SRC_COMMON_WSECV2_CALLBACKSMUL_H

#include "wsecv2_type.h"
#include "wsecv2_ctx.h"

#ifdef __cplusplus
extern "C" {
#endif

unsigned long WsecSetProcLockCallbacksMul(KmcCbbCtx *kmcCtx, const WsecProcLockCallbacksMulti *procLockCallbacks);

unsigned long WsecSetBasicRelyCallbacksMul(KmcCbbCtx *kmcCtx, const WsecBasicRelyCallbacksMulti *basicRelyCallbacks);

unsigned long WsecSetHardwareCallbacksMul(KmcCbbCtx *kmcCtx, const WsecHardwareCallbacksMulti *hardwareCallbacks);


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* KMC_SRC_COMMON_WSECV2_CALLBACKSMUL_H */
