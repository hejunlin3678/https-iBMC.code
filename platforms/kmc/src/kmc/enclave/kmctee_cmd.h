/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: kmc tee InvokeCommand interface
 * Author: yangchen
 * Create: 2022-09-30
 */

#ifndef KMC_SRC_KMC_ENCLAVE_KMCTEE_CMD_H
#define KMC_SRC_KMC_ENCLAVE_KMCTEE_CMD_H

#include "wsecv2_config.h"
#if WSEC_COMPILE_ENABLE_TEE
#include "wsecv2_ctx.h"
#include "wsecv2_type.h"
#include "tee_client_api.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

unsigned long KmcInvokeCommandTa(KmcCbbCtx *kmcCtx, WsecUint32 commandID, WsecUint32 flag, WsecBuff *firstInBuff,
    WsecBuff *secondInBuff, WsecBuff *firstOutBuff, WsecBuff *secondOutBuff);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
#endif // KMC_SRC_KMC_ENCLAVE_KMCTEE_CMD_H
