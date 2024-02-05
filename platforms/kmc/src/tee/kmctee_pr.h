/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Communication analysis layer of TA layer
 * Author: liwei
 * Create: 2021-05-03
 */

#ifndef KMC_SRC_TEE_KMCTEE_PR_H
#define KMC_SRC_TEE_KMCTEE_PR_H
#include "kmctee_adapt.h"
#include "kmctee_status.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


KmcTaResult KmcParseImpExpMk(void *sessionContext, WsecUint32 paramTypes, TEE_Param params[PARAMNUM]);

KmcTaResult KmcParseExpWk(void *sessionContext, WsecUint32 paramTypes, TEE_Param params[PARAMNUM]);

KmcTaResult KmcParseFreeCtx(void *sessionContext, WsecUint32 paramTypes, TEE_Param params[PARAMNUM]);

KmcTaResult KmcParseGetMkCipherLen(void *sessionContext, WsecUint32 paramTypes, TEE_Param params[PARAMNUM]);

KmcTaResult KmcParseCreateMk(void *sessionContext, WsecUint32 paramTypes, TEE_Param params[PARAMNUM]);

KmcTaResult KmcParseStreamInit(void *sessionContext, WsecUint32 paramTypes, TEE_Param params[PARAMNUM]);

KmcTaResult KmcParseStreamUpdate(void *sessionContext, WsecUint32 paramTypes, TEE_Param params[PARAMNUM]);

KmcTaResult KmcParseStreamFinal(void *sessionContext, WsecUint32 paramTypes, TEE_Param params[PARAMNUM]);

KmcTaResult KmcParseGetMkCipher(void *sessionContext, WsecUint32 paramTypes, TEE_Param params[PARAMNUM]);

KmcTaResult KmcParseInitRk(void *sessionContext, WsecUint32 paramTypes, TEE_Param params[PARAMNUM]);

KmcTaResult KmcParseEncrypt(void *sessionContext, WsecUint32 paramTypes, TEE_Param params[PARAMNUM]);

KmcTaResult KmcParseDecrypt(void *sessionContext, WsecUint32 paramTypes, TEE_Param params[PARAMNUM]);

KmcTaResult KmcParseHmac(void *sessionContext, WsecUint32 paramTypes, TEE_Param params[PARAMNUM]);

KmcTaResult KmcParseMasterDHPair(void *sessionContext, WsecUint32 paramTypes, TEE_Param params[PARAMNUM]);

KmcTaResult KmcParseACreateShareKey(void *sessionContext, WsecUint32 paramTypes, TEE_Param params[PARAMNUM]);

KmcTaResult KmcParseMCreateShareKey(void *sessionContext, WsecUint32 paramTypes, TEE_Param params[PARAMNUM]);

KmcTaResult KmcParseSkEncryptKey(void *sessionContext, WsecUint32 paramTypes, TEE_Param params[PARAMNUM]);

KmcTaResult KmcParseSkDecryptKey(void *sessionContext, WsecUint32 paramTypes, TEE_Param params[PARAMNUM]);

KmcTaResult KmcParseBatchMkHash(void *sessionContext, WsecUint32 paramTypes, TEE_Param params[PARAMNUM]);

KmcTaResult KmcParseUpdateSrk(void *sessionContext, WsecUint32 paramTypes, TEE_Param params[PARAMNUM]);

KmcTaResult KmcParseGetSupportDHAlg(void *sessionContext, WsecUint32 paramTypes, TEE_Param params[PARAMNUM]);

KmcTaResult KmcParseEncryptAsymKey(void *sessionContext, WsecUint32 paramTypes, TEE_Param params[PARAMNUM]);

#if WSEC_COMPILE_ENABLE_ASYM
KmcTaResult KmcParseCreateAsymKey(void *sessionContext, WsecUint32 paramTypes, TEE_Param params[PARAMNUM]);

KmcTaResult KmcParseSign(void *sessionContext, WsecUint32 paramTypes, TEE_Param params[PARAMNUM]);

KmcTaResult KmcParseAsymDecrypt(void *sessionContext, WsecUint32 paramTypes, TEE_Param params[PARAMNUM]);
#endif // WSEC_COMPILE_ENABLE_ASYM

#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif // KMC_SRC_TEE_KMCTEE_PR_H
