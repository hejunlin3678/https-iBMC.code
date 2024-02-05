/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: GP encryption and decryption, hmac, hash algorithm implementation
 * Author: liwei
 * Create: 2021-05-03
 */

#ifndef KMC_SRC_TEE_KMCTEE_GP_H
#define KMC_SRC_TEE_KMCTEE_GP_H

#include "kmctee_adapt.h"
#include "kmctee_status.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

KmcTaResult TaGenerateRandom(void *buffer, WsecUint32 len);

KmcTaResult TaFreeOperation(TEE_OperationHandle op);

KmcTaResult TaFreeObjectHandle(TEE_ObjectHandle object);

KmcTaResult TaIsAlgoSupported(WsecUint32 algId, WsecUint32 element);

KmcTaResult TaAllocateOperation(TEE_OperationHandle *op, WsecUint32 algorithm, WsecUint32 mode,
    WsecUint32 maxKeySize);

KmcTaResult TaSetKey(TEE_OperationHandle op, const WsecUint8 *key, WsecUint32 keyLen, WsecUint32 objectType);

KmcTaResult TaAeDecryptFinal(TEE_OperationHandle operation, WsecBuffConst *src, WsecBuff *dest,
    WsecBuffConst *tagBuff);

KmcTaResult TaAeEncryptFinal(TEE_OperationHandle operation, WsecBuffConst *src, WsecBuff *dest,
    WsecBuff *tagBuff);

KmcTaResult TaAeUpdate(TEE_OperationHandle operation, WsecBuffConst *src, WsecBuff *dest);

KmcTaResult TaAeInit(TEE_OperationHandle operation, WsecBuffConst *iv, WsecUint32 tagLen, WsecUint32 aadLen,
    WsecUint32 payloadLen);

KmcTaResult TaDigestDoFinal(TEE_OperationHandle op, const void *chunk, WsecUint32 chunkLen, void *hash,
    WsecUint32 *hashLen);

KmcTaResult TaDigestUpdate(TEE_OperationHandle op, const void *chunk, WsecUint32 chunkSize);

KmcTaResult TaMacInit(TEE_OperationHandle operation, void *iv, WsecUint32 ivLen);

KmcTaResult TaMacUpdate(TEE_OperationHandle operation, const void *chunk, WsecUint32 chunkSize);

KmcTaResult TaMacComputeFinal(TEE_OperationHandle operation, const void *message, WsecUint32 messageLen, void *mac,
    WsecUint32 *macLen);

KmcTaResult TaCipherInit(TEE_OperationHandle operation, WsecBuffConst *iv);

KmcTaResult TaCipherUpdate(TEE_OperationHandle operation, WsecBuffConst *src, WsecBuff *dest);

KmcTaResult TaCipherDoFinal(TEE_OperationHandle operation, WsecBuffConst *src, WsecBuff *dest);

WsecBool TaCheckParam(WsecUint32 paramTypes, WsecUint32 valid0, WsecUint32 valid1, WsecUint32 valid2,
    WsecUint32 valid3);

KmcTaResult TaDhAlgSupported(WsecUint32 algId, WsecUint32 element);

KmcTaResult TaGenerateKeyPair(WsecUint32 curveType, TEE_ObjectHandle keyPair,
    TEE_Attribute *params, WsecUint32 keySize, WsecUint32 keyLen, WsecUint8 *outPub, WsecUint32 *pubLen);

KmcTaResult TaDeriveShareKey(WsecUint32 type, TEE_ObjectHandle objectHandle, WsecUint8 *inPub,
    WsecUint32 inPubLen, WsecBuff *skBuff);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // KMC_SRC_TEE_KMCTEE_GP_H
