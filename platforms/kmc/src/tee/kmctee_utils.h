/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: KMC TA internal tool function
 * Author: liwei
 * Create: 2021-05-03
 */

#ifndef KMC_SRC_TEE_KMCTEE_UTILS_H
#define KMC_SRC_TEE_KMCTEE_UTILS_H

#include "kmctee_adapt.h"
#include "kmctee_status.h"
#include "kmctee_msg.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct TagKmcObjectCtx {
    void *op;
    uint32_t magic;
} KmcObjectCtx;


KmcTaResult ParseParamFromTlv(unsigned char *tlvBase, WsecUint32 tlvLen, TlvArray *tlvArray);

size_t SetPointerToValue(const WsecUint32 *value);

void SetValueToPointer(WsecUint32 *pointer, size_t value);

KmcTaResult GetRootKey(const WsecUint8 *rkInfo, WsecUint32 rkInfoLen,
    WsecUint8 *rkKey, WsecUint32 rkLen, WsecBool isAlwaysDeriveRk);

KmcTaResult GetMasterKey(WsecBuffConst *rkInfo, WsecBuffConst *mk, WsecBuff *plainMk, WsecBool isAlwaysDeriveRk);

KmcTaResult GetWorkKey(KmcTeeSdpInfo *info, WsecBuffConst *rkInfo, WsecBuffConst *mk, WsecBuffConst *salt,
    WsecBuff *workKey);

KmcTaResult CheckWorkLen(WsecUint32 algId, WsecUint32 workKeyLen);

KmcTaResult EncDecTeeMk(WsecUint32 algId, WsecUint32 mode, WsecBuffConst *key, WsecBuffConst *src, WsecBuff *dest);

KmcTaResult Hmac(WsecUint32 algId, const WsecUint8 *key, WsecUint32 keyLen, WsecBuffConst *inData, WsecBuff *hmac);

KmcTaResult Digest(WsecUint32 algId, WsecBuffConst *src, WsecBuff *dest);

KmcTaResult EncDecMkWithHmac(EnclaveMkInfo *mkInfo, WsecBuffConst *rkInfo, WsecBuffConst *hmacSalt, WsecBuffConst *src,
    WsecBuff *dest, WsecBuff *hmac);

KmcTaResult Encrypt(WsecUint32 algId, WsecUint32 padding, WsecBuffConst *key, WsecBuffConst *iv, WsecBuffConst *src,
    WsecBuff *dest, WsecBuff *tag);

KmcTaResult Decrypt(WsecUint32 algId, WsecUint32 padding, WsecBuffConst *key, WsecBuffConst *iv,
    WsecBuff *dest, WsecBuffConst *src, WsecBuff *tag);

WsecUint32 KmcAlgId2TeeAlgId(WsecUint32 algId, WsecUint32 padding);

WsecUint32 TeeAlg2Type(WsecUint32 teeAlg);

WsecBool CheckAeAlg(WsecUint32 kmcAlgId);

WsecUint32 GetKeyLenByAlg(WsecBool isWkUse, WsecUint32 algId);

KmcTaResult EncryptDecryptInit(WsecUint32 algId, TEE_OperationHandle operation, WsecBuffConst *iv, WsecUint32 tagLen,
    WsecUint32 aadLen, WsecUint32 payloadLen);

KmcTaResult EncryptDecryptUpdate(WsecUint32 isAE, TEE_OperationHandle operation,
    WsecBuffConst *src, WsecBuff *dest);

KmcTaResult EncryptDecryptFinal(WsecUint32 isAE, WsecUint32 mode, TEE_OperationHandle operation,
    WsecBuffConst *src, WsecBuff *dest, WsecBuff *tag);

KmcTaResult GetGpAlgId(WsecUint32 kmcAlgId, WsecUint32 padding, WsecUint32 *gpAlg);


KmcTaResult KmcTaSupportSymAlg(WsecUint32 algId);

KmcTaResult KmcTaSupportHmacAlg(WsecUint32 algId);

KmcTaResult KmcTaSupportPbkdf2Alg(WsecUint32 algId);

KmcTaResult CheckSdpParamCommon(KmcTeeSdpInfo *info, WsecBuffConst *rkInfo, WsecBuffConst *mk,
    WsecBuffConst *salt, WsecBuffConst *iv);

KmcTaResult KmcKdfAlg2GpHashType(WsecUint32 kdfAlg, WsecUint32 *hashType);

KmcTaResult TaDeriveHrk(const WsecSrkPrimitive *rkPrimitive, WsecBuff *derivedRk);
#ifdef WSEC_COMPILE_OPTEE
void TaPbkdf2Hmac(WsecUint32 *algId);
KmcTaResult TaGenererateFixedLenKey(TEE_ObjectHandle object, uint32_t attributeID, void *buffer, uint32_t *size,
                                    uint32_t keyLen);
#endif

WsecUint32 GetEcInfoFromType(bool isAlg, WsecUint32 kmcType, WsecUint32 *keySize, WsecUint32 *ecType,
    WsecUint32 *pairOrAlg, WsecUint32 *keyLen);

KmcTaResult CheckSdpParam(KmcTeeSdpInfo *info, WsecBuffConst *rkInfo, WsecBuffConst *mk,
                          WsecBuffConst *salt, WsecBuffConst *iv, WsecBuffConst *inBuff, WsecBuff *outBuff);

KmcTaResult EncDecAsymKey(WsecUint32 algId, WsecUint32 mode, WsecBuffConst *rkInfo, WsecBuffConst *protectKey,
    WsecBuffConst *src, WsecBuff *dest);

KmcTaResult EncDecAsymKeyByRootKey(WsecUint32 algId, WsecUint32 mode, WsecBuffConst *rk, WsecBuffConst *protectKey,
    WsecBuffConst *src, WsecBuff *dest);

KmcTaResult TaGeKeyObjectAttrBuff(TEE_ObjectHandle object, uint32_t attributeID, void *buffer, teesz_t *size,
                                  uint32_t keyLen);
uint32_t TaGetKmcMagicNum(void);
void TaSetKmcMagicNum(void);

KmcTaResult TaInitKmcObjectCtx(void *objectHandle, uint64_t *object);
KmcTaResult TaGetKmcObject(uint64_t object, uint64_t *objectCtx);
KmcTaResult TaGetKmcObjectAndFree(uint64_t object);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // KMCTEE_UTILS_H
