/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: KMC internal interface header file, which is not open to external systems.
 * Author: t00449241
 * Create: 2020-05-26
 */

#ifndef KMC_SRC_KMC_KMC_UTILS_H
#define KMC_SRC_KMC_KMC_UTILS_H

#include "wsecv2_type.h"
#include "wsecv2_array.h"
#include "kmcv2_pri.h"
#include "cacv2_pri.h"
#include "wsecv2_sync_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* Allocating Multiple KmcKsfMem */
WsecBool AllocMultiBlock(KmcCbbCtx *kmcCtx, KmcKsfMem **buffs, size_t buffLen, WsecUint32 count);

/* This interface is used to release the dynamic memory of the KmcKsfMem type. The return value is NULL. */
KmcKsfMem *FreeKsfSnapshot(KmcCbbCtx *kmcCtx, KmcKsfMem *data);

WsecVoid FreeKsfMemArraySnapshot(KmcCbbCtx *kmcCtx, KmcKsfMem **readBuff, WsecUint32 buffLen);

/* Cloning KmcKsfMem and Selecting MK , Enclave RK and HardRK;
 * withCache Determines whether to copy the sub cached in the memory. such as subTlv、unknow novaKsf
 */
unsigned long CloneKsfMem(KmcCbbCtx *kmcCtx, WsecBool withMk, WsecBool withAsym, WsecBool withCache,
    WsecBool withHardRk, WsecBool withEnclaveRk, const KmcKsfMem *src, KmcKsfMem **dest);

/* Clone KmcKsfMem, Optional clone All MK or without special mk, Clone HardRk
 * withCache Determines whether to copy the sub cached in the memory. such as subTlv、unknow novaKsf
 */
unsigned long CloneKsfMemWithoutDomain(KmcCbbCtx *kmcCtx, WsecBool withMk, WsecBool withAsym,  WsecBool withCache,
    WsecBool withHardRk, WsecBool withEnclaveRk, const KmcKsfMem *src, KmcKsfMem **des, KmcRecvSyncInfo *syncInfo,
    WsecBool isAsymFilter);

/* Get minMkId and maxMkId  */
unsigned long PriGetEndMkId(KmcCbbCtx *kmcCtx, KmcKsfMem *ksfMem, WsecUint32 domainId, WsecUint32 *minId,
    WsecUint32 *maxId);

/* Get mk by domainId and keyId */
unsigned long PriGetMemMkByDomainIdKeyId(KmcCbbCtx *kmcCtx, const KmcKsfMem *keystore, WsecUint32 domainId,
    WsecUint32 keyId, KmcMemMk **foundMk, int *foundMkIndex);

/* Adds an MK to an array. */
unsigned long AddMkToArray(KmcCbbCtx *kmcCtx, KmcKsfMem *keystore, KmcMemMk *mk, WsecBool alreadyProtected);

/*
 * Callback function for comparing uint32
 * The caller must ensure the validity of input parameters.
 */
int CompareUint32ForArr(const WsecVoid *p1, const WsecVoid *p2);

/*
 * Callback function for comparing the sizes of two KmcMemMk elements during quick array sorting or search.
 * The caller must ensure the validity of input parameters.
 */
int CompareMkForArr(const WsecVoid *p1, const WsecVoid *p2);

/* This method is invoked when array elements are removed. */
WsecVoid OnRemoveMkArr(WsecHandle kmcCtx, WsecVoid *element, WsecUint32 elementSize);

/* Domain+keyid: Obtain the index location. */
int SearchMkByKeyId(const KmcKsfMem *keystore, WsecUint32 domainId, WsecUint32 keyId);

/* Search MK in MK array by mk hash data */
int SearchMkArrayByHash(KmcCbbCtx *kmcCtx, const WsecArray mkArray, const unsigned char *hashData, WsecUint32 hashLen);

int SearchAsymMkArrayByHash(KmcCbbCtx *kmcCtx, const WsecArray mkArray, const unsigned char *priHashData,
                            WsecUint32 priHashLen, const unsigned char *pubHashData, WsecUint32 pubHashLen);

/* Search MK in KmcKsfMem by mk hash data */
int SearchMkByKeyHash(KmcCbbCtx *kmcCtx, const unsigned char *hashData, WsecUint32 hashLen);

/* Search MK by domainId and mk hash */
int SearchMkByDomainIdKeyHash(KmcCbbCtx *kmcCtx, const KmcKsfMem *keystore, WsecUint32 domainId,
    const unsigned char *hashData, WsecUint32 hashLen);

/* Obtain the MK from the value of SearchMkByKeyId or SearchMkByKeyHash. */
unsigned long GetSearchedMk(KmcCbbCtx *kmcCtx, int idx, KmcMemMk **mk);

/* Get MK by keyId and hash data, if the MK hash data got by keyId not equal input hash data, then get by hash again */
unsigned long GetMkByKeyIdAndHash(KmcCbbCtx *kmcCtx, const KmcDomainKeyPair *keyParam, KmcMemMk **mk);

/* Check the mkdomain is validate */
unsigned long CheckDomainIsNotLocalDomain(KmcCbbCtx *kmcCtx, WsecUint32 mkDomainId);

unsigned long CheckDomainExitsAndNotLocalDomain(KmcCbbCtx *kmcCtx, WsecUint32 mkDomainId);

/* Check KmcKeyTypesInfo match the given keyType */
int TypeInSpecifiedTypes(WsecUint16 keyType, KmcKeyTypesInfo keyTypes);

/* Check KmcKsfMem MK Array Contains unique domain id */
unsigned long CheckMkArrayContainUniqueDomain(KmcCbbCtx *kmcCtx, KmcKsfMem *ksfMem, WsecUint32 *uniqueDomainId);

/* Deriving encKey and HmacKey */
unsigned long DeriveKey(KmcCbbCtx *kmcCtx, const KmcMkfHeaderWithHmac *headerWithHmac, WsecBuffConst passwordBuff,
    WsecBuff encKeyBuff, WsecBuff hmacKeyBuff);

/* Filter the MK list based on domainId. */
unsigned long FilterByDomain(KmcCbbCtx *kmcCtx, const WsecArray src, WsecUint32 domainId, WsecArray *dest);


/* Filter the MK based on domainType. */
unsigned long FilterByDomainType(KmcCbbCtx *kmcCtx, const WsecArray src, WsecBool isAllDomain,
                                 WsecUint32 domainType, WsecArray *dest);


/* Add and swap mk with KsfMem struct */
unsigned long AddAndSwapMkWithKsfMem(KmcCbbCtx *kmcCtx, KmcKsfMem *ksfMem, WsecArray addMkArr,
    WsecArray *dummyMkArr);


/* Create KmcMemMk based on the key information and plaintext key. */
unsigned long CreateMemMkFromInfoAndPlainKey(KmcCbbCtx *kmcCtx, const KmcMkInfo *mkInfo,
    const unsigned char *key, WsecUint32 keyLen,
    KmcKsfMem *ksfMem, ImpExpMkType mkType);

/*
 * If WsecInitializeHw is invoked, the version is converted to V3.
 * (If this interface is invoked for upgrade, rollback is not considered.)
 */
unsigned long ConvertToV3V4IfHardware(KmcCbbCtx *kmcCtx, WsecBool *needWrite);

/* Compare whether mks with specific domainId in destCompareMkArr(g_keystore) same as that in compareMkArr */
WsecBool CompareMkArray(KmcCbbCtx *kmcCtx, const WsecArray compareMkArr, const WsecArray destCompareMkArr,
    WsecUint32 domainId);

WsecBool CompareAsymMkArray(KmcCbbCtx *kmcCtx, const WsecArray compareMkArr, const WsecArray destCompareMkArr,
                            WsecUint32 domainId);

/* Safety init KsfMem array, initing the ksfMem array only it's uninited */
unsigned long InitKsfMemArraySafe(KmcCbbCtx *kmcCtx, KmcKsfMem **targetKsfMem, const WsecArray addMkArray, WsecArray addAsymKeyArray);

/* Add MK to target KsfMem incremantaly, orignal MK not overwrite. Noting to do if the MK is already exists. */
unsigned long AddMkToKsmByIncremental(KmcCbbCtx *kmcCtx, KmcKsfMem *targetKsfMem, WsecArray addMkArray,
    KmcKsfMem *importKsfMem);

/* Add Mk to target KsfMem by full replacement, origin Mk will be overwrite by addMkArray */
unsigned long AddMkToKsmByFullReplace(KmcCbbCtx *kmcCtx, KmcKsfMem *targetKsfMem, WsecArray addMkArray,
    KmcKsfMem *importKsfMem);

/* Creating an MK for a Keystore */
unsigned long CreateMkItemEx(KmcCbbCtx *kmcCtx, KmcKsfMem *keystore, const KmcCfgDomainInfo *domainInfo,
    const KmcCfgKeyType *keyTypeCfg, WsecBuffConst *hmacBuff, WsecBuffConst *keyBuff, WsecUint32 keyId,
    WsecBool setActive, WsecArray changedMkArray, ImpExpMkType mkType);

/* Compare Domain in specific Array by domainId using to order domain list */
int CompareDomainIdForArr(const WsecVoid *p1, const WsecVoid *p2);

/* Notify the changed Mks */
void DoChangeNotify(WsecArray changedArray, WsecBool setActive);

/* Convert inner stored hash short code to Hash Algorithm ID */
unsigned long CvtShortCodeToHashAlg(unsigned char shortCode, WsecUint32 *hashAlg);

/* Convert hash Algorithm ID to inner stored hash short code */
unsigned char CvtHashAlgToShortCode(WsecUint32 hashAlg);

/* Convert inner stored kdf short code to kdf Algorithm ID */
unsigned long CvtShortCodeToKdfAlg(unsigned char shortCode, WsecUint32 *kdfAlg);

/* Convert kdf Algorithm ID to inner stored kdf short code */
unsigned char CvtKdfAlgToShortCode(WsecUint32 kdfAlg);

/* Convert inner stored hamc short code to hmac Algorithm ID */
unsigned long CvtShortCodeToHmacAlg(unsigned char shortCode, WsecUint32 *hmacAlg);

/* Convert hmac Algorithm ID to inner stored hmac short code */
unsigned char CvtHmacAlgToShortCode(WsecUint32 hmacAlg);

/* Get Symmetric algorithm key Len */
unsigned long KmcGetRMKeyByteLen(KmcCbbCtx *kmcCtx, WsecUint32 algId, WsecUint32 *rmkLen);

/* Check whether the internal algorithm is supported. NOTE: not sdp algorithm */
unsigned long KmcCheckInternalDegistAlgIsSupport(KmcCbbCtx *kmcCtx, const KmcKsfRk *rk);

/* Check the alg is whther supported for all of the OPENSSL/Mbedtls/IPSI library */
WsecBool CheckSupportConfigAlg(KmcCbbCtx *kmcCtx, const KmcAlgCnfParam *algParm);

/* Digest init short code */
unsigned long KmcDigestInitWithShortCode(KmcCbbCtx *kmcCtx, WsecHandle *ctx, WsecUint32 ctxNum,
    unsigned char hashShortCode);

unsigned long CreateMkArrEx(KmcCbbCtx *kmcCtx, KmcKsfMem *ksfMem);

unsigned long MakeV3KsfMk(KmcCbbCtx *kmcCtx, KmcKsfMk *mk, KmcMemMk *mkMem, WsecUint32 *mkKsfLen,
    const KmcKsfHardRk *hardRk);

unsigned long MakeV3UserDefineSrkKsfMk(KmcCbbCtx *kmcCtx, KmcKsfMk *mk, KmcMemMk *mkMem, WsecUint32 *mkKsfLen);

unsigned long MakeV4KsfMk(KmcCbbCtx *kmcCtx, KmcMemMk *mkMem, KmcKsfMk *mk);

WsecVoid CvtByteOrderForKsfMk(KmcKsfMk *memMk, WsecUint32 direction, WsecUint16 ksfVersion);

WsecBool CompareMkArrayWithStatus(KmcCbbCtx *kmcCtx, const WsecArray compareMkArr, const WsecArray destCompareMkArr,
    WsecUint32 domainId);

WsecBool CompareAsymMkArrayWithStatus(KmcCbbCtx *kmcCtx, const WsecArray compareMkArr, const WsecArray destCompareMkArr,
                                      WsecUint32 domainId);

int SearchAsymMkArrayByStatus(const WsecArray mkArray, WsecUint32 domainId, int status);

unsigned long KmcDigestInitArr(KmcCbbCtx *kmcCtx, WsecHandle *ctx, WsecUint32 ctxNum, WsecUint32 algId);

unsigned long KmcDigestUpdateArr(KmcCbbCtx *kmcCtx, const WsecHandle *ctx, WsecUint32 ctxNum,
    const WsecVoid *data, WsecUint32 dataLen);

WsecVoid AssignTeeRk(KmcCbbCtx *kmcCtx, const WsecSrkPrimitive *srkPrimitive, WsecUint32 srkPrimitiveLen,
    const WsecUint8 *enclaveRk, WsecUint32 enclaveRkLen, WsecBuffConst *rk);

KmcCompareResult KmcCompareChangedDomain(WsecUint32 compareDomain, const KmcRecvSyncInfo *domainArray);

unsigned long GetAllMkAndNovaKeyCount(KmcCbbCtx *kmcCtx);

unsigned long KmcGetMKRecordLen(KmcCbbCtx *kmcCtx, KmcKsfRk *rkWrite);

unsigned long KmcCheckHashAlgoId(KmcCbbCtx *kmcCtx, unsigned char memHash, WsecBool* flag);

unsigned long KmcCheckHmacAlgoId(KmcCbbCtx *kmcCtx, unsigned char memHmac, WsecBool* flag);

unsigned long KmcCheckKdfAlgoId(KmcCbbCtx *kmcCtx, unsigned char memKdf, WsecBool* flag);

unsigned long GetDomainTypeWithMiss(KmcCbbCtx *kmcCtx, WsecUint32 domainId, WsecUint32 filterDomainType,
                                    unsigned char *type);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* KMC_SRC_KMC_KMC_UTILS_H */
