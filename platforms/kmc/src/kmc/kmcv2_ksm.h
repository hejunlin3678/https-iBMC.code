/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2014-2020. All rights reserved.
 * Description: KMC internal interface header file, which is not open to external systems.
 * Author: x00102361
 * Create: 2014-06-16
 * History: 2018-10-08 Zhang Jie (employee ID: 00316590) Rectification by UK
 * On March 19, 2019, Zhang Jie (employee ID: 00316590) split the original kmcv2_itf.c file into
 * kmcv2_ksf.c/kmcv2_ksm.c/kmcv2_itf.c/kmcv2_cfg.c to meet the requirements of the 2000-line file.
 *                     ITF: interface
 *                     KSM: keystore memory
 *                     KSF: keystore file
 */

#ifndef KMC_SRC_KMC_KMCV2_KSM_H
#define KMC_SRC_KMC_KMCV2_KSM_H

#include "wsecv2_type.h"
#include "kmcv2_itf.h"
#include "kmcv2_pri.h"
#include "cacv2_pri.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


#pragma pack(1)
typedef struct TagKmcMkInfoArray {
    KmcMkInfo *mkInfos;
    int mkCount;
} KmcMkInfoArray;
#pragma pack()


/* Get memory keystore */
KmcKsfMem *KsmGetKeystore(KmcCbbCtx *kmcCtx);

/* set g_keystore */
WsecVoid KsmSetKeystore(KmcCbbCtx *kmcCtx, KmcKsfMem *newKeystore);

/* Obtains the detailed MK information, which corresponds to KmcGetMkDetail. */
unsigned long MemGetMkDetail(KmcCbbCtx *kmcCtx, WsecUint32 domainId, WsecUint32 keyId,
    KmcMkInfo *mkInfo, unsigned char *keyPlaintextBuff, WsecUint32 *keyBuffLen, WsecUint32 flag);

/* Check whether the memory KSF and configuration information is ready. */
unsigned long MemCheckKsfMemAndCfg(KmcCbbCtx *kmcCtx);

/*
 * Check whether the memory KSF and configuration information is ready and whether the domain operation is
 * valid (in V1, only the master operates the shared domain).
 * Both V2 and V3 masters are supported, but the agent can operate only the local domain.
 */
unsigned long MemCheckKsfMemAndCfgDomain(KmcCbbCtx *kmcCtx, WsecUint32 domain, WsecBool *shareDomain);

/*
 * Check whether the domain operation is valid (in V1, only the master operates the shared domain).
 * Both V2 and V3 masters are supported, but the agent can operate only the local domain.
 */
unsigned long MemCheckCfgDomain(KmcCbbCtx *kmcCtx, WsecUint32 domain, WsecBool *shareDomain);

/*
 * Check whether the memory KSF and configuration information is ready and whether the operation
 * permission is available. (Only the master is available in V1, and both V2 and V3 are available.)
 */
unsigned long MemCheckKsfMemAndCfgEx(KmcCbbCtx *kmcCtx);

/* Release the global resource g_kmcCfg g_keystore and remove the hardware root key. */
WsecVoid MemFreeGlobal(KmcCbbCtx *kmcCtx);

/* Release the global resource g_kmcCfg g_keystore and remove the hardware root key. */
WsecVoid MemFreeGlobalAndRemoveHardRk(KmcCbbCtx *kmcCtx);

/* Updating the Root Key */
unsigned long MemUpdateRootKey(KmcCbbCtx *kmcCtx, const unsigned char *entropy, WsecUint32 size, WsecUint16 ksfVersion);

/* Copying the RK Attribute */
WsecVoid MemGetRkAttr(KmcCbbCtx *kmcCtx, KmcRkAttributes *rkAttr);

unsigned long MemTeeGetRootKeyInfo(KmcCbbCtx *kmcCtx, WsecVoid *rkInfo, WsecUint32 *rkInfoLen);

/* Obtains the maximum or minimum domain key ID. */
unsigned long MemGetEndMkId(KmcCbbCtx *kmcCtx, WsecUint32 domainId, WsecUint32 *minId, WsecUint32 *maxId);

/* Key ID and Hash to obtain the key content */
unsigned long MemGetMkByIDHash(KmcCbbCtx *kmcCtx, WsecUint32 domainId,
    WsecUint32 keyId,
    const unsigned char *hashData, WsecUint32 hashLen,
    unsigned char *keyPlaintextBuff, WsecUint32 *keyBuffLen);

/* Load the KSF. If the KSF does not exist, create it. */
unsigned long MemLoadDataEx(KmcCbbCtx *kmcCtx, WsecUint32 role, const WsecInternalInitParam *initParam);

/* Removing a Master Key */
unsigned long MemRmvMk(KmcCbbCtx *kmcCtx, WsecUint32 domainId, WsecUint32 keyId, WsecBool shareDomainMkChanged,
    KmcMkInfo *removeMk);

unsigned long MemBatchRmvMkByCount(KmcCbbCtx *kmcCtx, WsecUint32 rmvDomainCount, KmcRmvMkParam *rmvMkParam,
    KmcMkInfoArray *rmvMkInfoArray, WsecBool shareDomainMkChanged);

/* Registering a Master Key */
unsigned long MemRegisterMkEx(KmcCbbCtx *kmcCtx, WsecUint32 domainId,
    WsecUint32 keyId, WsecBool shareDomainMkChanged,
    WsecBuffConst *hmac, WsecBuffConst *keyBuff, ImpExpMkType mkType);

/* Creating keys in batch */
unsigned long MemBatchCreateMkEx(KmcCbbCtx *kmcCtx, const KmcOpMkParam *opMkParam, WsecBool shareDomainMkChanged);

/* Obtaining the Number of Master Keys */
int MemGetMkCount(KmcCbbCtx *kmcCtx);

/* Obtains the master key information of a specified index. */
unsigned long MemGetMkInfo(KmcCbbCtx *kmcCtx, int idx, KmcMkInfo *mkInfo);

/* Obtaining the Number of MKs in a Specified Domain */
int MemGetMkCountByDomain(KmcCbbCtx *kmcCtx, WsecUint32 domainId);

/* Obtain the key content and information using the key hash. */
unsigned long MemGetMkDetailByHash(KmcCbbCtx *kmcCtx, const unsigned char *hashData, WsecUint32 hashLen,
    KmcMkInfo *mkInfo,
    unsigned char *keyPlaintextBuff, WsecUint32 *keyBuffLen, WsecUint32 type);

/* Key content obtaining key info */
unsigned long MemGetMkInfoByContent(KmcCbbCtx *kmcCtx, const unsigned char *keyPlaintextBuff, WsecUint32 keyLen,
    KmcMkInfo *mkInfo);

/* +info (Obtaining the Activation Key) */
unsigned long MemGetActiveMk(KmcCbbCtx *kmcCtx, WsecUint32 domainId, KmcKeyTypesInfo keyTypes, KmcMkInfo *mkInfo,
    unsigned char *keyPlaintextBuff, WsecUint32 *keyBuffLen, WsecUint32 flag);

/* Obtaining the Key Hash Value Based on the Key Domain and ID */
unsigned long MemGetMkHash(KmcCbbCtx *kmcCtx, WsecUint32 domainId, WsecUint32 keyId, unsigned char *hashData,
    WsecUint32 *hashLen);

/* Setting the Status of a Specified Master Key */
unsigned long MemSetMkStatus(KmcCbbCtx *kmcCtx, WsecUint32 domainId, WsecUint32 keyId, WsecBool shareDomainMkChanged,
    unsigned char status, WsecBool *needNotify, KmcMkInfo *notifyMkInfo);

/* Obtaining the Status of a Specified Master Key */
unsigned long MemGetMkStatus(KmcCbbCtx *kmcCtx, WsecUint32 domainId, WsecUint32 keyId, unsigned char *status);

/* Activating a Master Key */
unsigned long MemActivateMk(WsecUint32 domainId, WsecUint32 keyId, WsecBool shareDomainMkChanged,
    WsecArray changedMkArray, KmcMkInfo *mkInfoNotify);


unsigned long MemBatchActivateMk(KmcCbbCtx *kmcCtx, KmcOpMkParam *opMkParam, WsecArray changedMkArray,
    WsecBool shareDomainMkChanged);

/* Update the memory information protection mask. */
unsigned long MemRefreshMkMaskEx(KmcCbbCtx *kmcCtx);

/* Obtains the content ID and hash of the activation key in the current domain. */
unsigned long MemGetActiveMkWithHash(KmcCbbCtx *kmcCtx, WsecUint32 domainId, unsigned char *keyBuff,
    WsecUint32 *keyBuffLen, WsecUint32 *keyId, unsigned char *keyHash, size_t hashLen);

/* Adding a Domain to the Memory */
unsigned long MemAddDomainEx(KmcCbbCtx *kmcCtx, const KmcCfgDomainInfo *domainInfo);

/* Generate all current MK KSFs. */
unsigned long MemGenerateKsfAll(KmcCbbCtx *kmcCtx, const char *keystoreFile);

/* Generate an MK KSF in a specified domain. */
unsigned long MemGenerateKsfByDomain(KmcCbbCtx *kmcCtx, WsecUint32 domainId, const char *keystoreFile);

/* Generate active and standby KMC KSFs. */
unsigned long MemReGenerateKsf(KmcCbbCtx *kmcCtx);

/* Generating a KSF of a Specified Version */
unsigned long MemGenerateV1V2V3Ksf(KmcCbbCtx *kmcCtx, WsecUint16 ksfVersion, const char *keystoreFile);

/* Exports the memory key to a specified KSF file (internal interface). */
unsigned long MemExportKsf(KmcCbbCtx *kmcCtx, const char *keystoreFile, KmcExportKsfCfg *exportKsfCfg);

/* Exports special MKs filter by domainIds and keyIds to keystore file. */
unsigned long MemExportByKeys(KmcCbbCtx *kmcCtx, const char *keystoreFile, const KmcExportKsfByKeysCfg *exportKsfCfg);

/* Specify the keystore file, import it to the memory MK, and synchronize it to the active and standby KSFs. */
unsigned long MemImportKsf(KmcCbbCtx *kmcCtx, const char *keystoreFile, KmcImportKsfCfg *importKsfCfg);

/* Checking the KSF */
unsigned long MemCheckKeyStore(KmcCbbCtx *kmcCtx, WsecBool rewriteOnCheckFail, unsigned long *rewriteErrorCode);

/* Number of KSF updates obtained from the memory */
WsecVoid MemGetKsfUpdateNumFromMem(KmcCbbCtx *kmcCtx, WsecUint32 *updateCounter);

/* Obtain the number of shared MK updates from the memory. */
WsecVoid MemGetSharedMkUpdateNumFromMem(KmcCbbCtx *kmcCtx, WsecUint32 *updateCounter);

/* Obtaining the Current KSF Version */
unsigned long MemGetKsfVersion(KmcCbbCtx *kmcCtx, WsecUint16 *ksfVersion);

unsigned long CheckRmvMkCount(KmcCbbCtx *kmcCtx, WsecUint32 domainId, int rmvMkCount);

WsecVoid MemGetCfgKsfCnf(KmcCbbCtx *kmcCtx, KmcCnfId *ids);

WsecVoid MemCheckIfMkExpired(KmcCbbCtx *kmcCtx, KmcMkInfo *mkFound, KmcUseExpiredMkNotify *mkNotify);

unsigned long NewKeyPrepare(KmcCbbCtx *kmcCtx, WsecUint32 domainId, WsecUint16 keyType, const KmcKsfMem *ksfMem,
    KmcDomainCfg **domainCfg, KmcCfgKeyType **keyTypeCfg);

/* Check domainKeyFrom adn keyType in Batch operate */
unsigned long BatchCheckKeyFromAndKeyType(KmcCbbCtx *kmcCtx, const KmcOpMkParam *opMkParam, WsecBool isAsym);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* KMC_SRC_KMC_KMCV2_KSM_H */
