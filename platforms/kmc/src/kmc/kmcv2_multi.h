/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: KMC MultiInstance interface tool func header file
 * Author: yangchen
 * Create: 2021-8-14
 * History: None
 */

#ifndef KMC_INCLUDE_KMCV2_MULTI_H
#define KMC_INCLUDE_KMCV2_MULTI_H

#include "wsecv2_type.h"
#include "wsecv2_ctx.h"
#include "kmcv2_ksm.h"
#include "kmcv2_pri.h"
#include "kmc_sync.h"
#include "cacv2_pri.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define KMC_LOG_DOMAIN_PRIVACY(kmcCtx, domainId)                      \
WSEC_LOG_E3(kmcCtx, "DomainId(%u) is privacy(%d ~ %d)",               \
    (domainId), KMC_PRI_DOMAIN_ID_MIN, KMC_PRI_DOMAIN_ID_MAX)

#define KMC_IS_VALID_MK_FROM(domainKeyFrom) \
    WSEC_IS2(domainKeyFrom, KMC_MK_GEN_BY_INNER, KMC_MK_GEN_BY_IMPORT)

#define KMC_IS_PRI_DOMAIN(domainId) \
    WSEC_IN_SCOPE(domainId, KMC_PRI_DOMAIN_ID_MIN, KMC_PRI_DOMAIN_ID_MAX)

#define KMC_IS_VALID_DOMAIN_TYPE(domainType) \
    WSEC_IS3(domainType, KMC_DOMAIN_TYPE_SHARE, KMC_DOMAIN_TYPE_LOCAL, KMC_DOMAIN_TYPE_IGNORE)

#define KMC_CFG_IS_ROOT_KEY_VALID(rkCfg) \
    (((rkCfg)->validity > 0) && ((rkCfg)->rmkIter > 0))

#define MIN_RANGE_ADVANCE_DAY 0
#define MAX_RANGE_ADVANCE_DAY 3650000

/* Both the master and agent can use KMC. */
WsecBool CanUseKmc(KmcCbbCtx *kmcCtx);
/* Only the master node can manage KMCs. */
WsecBool CanManageKmc(KmcCbbCtx *kmcCtx);
/* Obtains the MK based on the domain ID and key ID. */
unsigned long GetMkDetail(KmcCbbCtx *kmcCtx, WsecUint32 domainId, WsecUint32 keyId, KmcMkInfo *mkInfo,
    unsigned char *keyPlaintextBuff, WsecUint32 *keyBuffLen, WsecUint32 flag);
/* Deinitializes the KMC. */
unsigned long FinalizeKmc(KmcCbbCtx *kmcCtx, KmcLockOrNot lockOrNot, WsecBool forceFinalize, WsecBool rmvHardRk);
/*
 * Re-generate the root key, but the MK remains unchanged.
 * If entropy is specified, the entropy value is also involved in the root key generation.
 */
unsigned long UpdateRootKey(KmcCbbCtx *kmcCtx, const unsigned char *entropy, WsecUint32 size,
    KmcLockOrNot lockOrNot);
/*
 * Obtain the current maximum/minimum MK ID in a specified domain. endKeyId is the obtained value.
 * endKeyId is written only when the operation is successful.
 */
unsigned long GetEndMkId(KmcCbbCtx *kmcCtx, KmcLockOrNot lockOrNot, WsecUint32 domainId, WsecUint32 *endKeyId,
    WsecUint32 endType);
/* Obtains the notification status. */
WsecUint32 GetNotifyStatus(WsecUint32 status);
/* Check the initial input parameters and check whether they are initialized. */
unsigned long KmcInitilizeExPrepare(KmcCbbCtx *kmcCtx, const WsecInternalInitParam *initParam);

unsigned long CloneKsfName(KmcCbbCtx *kmcCtx, KmcKsfName *fileName);

unsigned long MultiKmcRmvMk(KmcCbbCtx *kmcCtx, WsecUint32 domainId, WsecUint32 keyId);

unsigned long MultiKmcRegisterMk(KmcCbbCtx *kmcCtx, WsecUint32 domainId, WsecUint32 keyId, WsecBuffConst *hmac,
    WsecBuffConst *keyBuff, ImpExpMkType mkType);

unsigned long BatchCheckDomainInfo(KmcCbbCtx *kmcCtx, const KmcOpMkParam *opMkParam,
    WsecBool *shareDomainMkChanged);

unsigned long MultiKmcCreateMk(KmcCbbCtx *kmcCtx, const KmcOpMkParam *opMkParam);

unsigned long MultiKmcUpdateRootKey(KmcCbbCtx *kmcCtx, const unsigned char *keyEntropy, WsecUint32 size);

unsigned long DoGetRootKeyInfo(KmcCbbCtx *kmcCtx, KmcRkAttributes *rkAttr);

int MultiKmcGetCount(KmcCbbCtx *kmcCtx);

int MultiKmcGetMkCountByDomain(KmcCbbCtx *kmcCtx, WsecUint32 domainId);

WsecVoid DoRmvMkByCountNotify(KmcCbbCtx *kmcCtx, KmcMkInfoArray rmvMkInfoArray);

WsecVoid BatchRmvMkFinalOperate(KmcCbbCtx *kmcCtx, WsecUint32 rmvDomainCount, KmcRmvMkParam *rmvMkParam,
    KmcMkInfoArray *rmvMkInfoArray, unsigned long flag);

unsigned long BatchRmvCheckDomainInfo(KmcCbbCtx *kmcCtx, WsecUint32 rmvDomainCount, KmcRmvMkParam *rmvMkParam,
    WsecBool *shareDomainMkChanged);

unsigned long DoBatchRmvByCount(KmcCbbCtx *kmcCtx, WsecUint32 rmvDomainCount, KmcRmvMkParam *rmvMkParam,
    WsecBool shareDomainMkChanged);

unsigned long MultiKmcGetMkDetailByHash(KmcCbbCtx *kmcCtx, const unsigned char *hashData, WsecUint32 hashLen,
    KmcMkInfo *mkInfo, unsigned char *keyPlaintextBuff, WsecUint32 *keyBuffLen, WsecUint32 type);

unsigned long MultiKmcGetMkInfoByContext(KmcCbbCtx *kmcCtx, const unsigned char *keyPlaintextBuff,
    WsecUint32 keyLen, KmcMkInfo *mkInfo);

unsigned long MultiKmcGetMkDetail(KmcCbbCtx *kmcCtx, WsecUint32 domainId, WsecUint32 keyId, KmcMkInfo *mkInfo,
    unsigned char *keyPlaintextBuff, WsecUint32 *keyBuffLen, WsecUint32 flag);

unsigned long MultiKmcSetMkMaxCount(KmcCbbCtx *kmcCtx, int count);

unsigned long MultiKmcSetMkStatus(KmcCbbCtx *kmcCtx, WsecUint32 domainId, WsecUint32 keyId, unsigned char status);

unsigned long MultiKmcGetMkStatus(KmcCbbCtx *kmcCtx, WsecUint32 domainId, WsecUint32 keyId, unsigned char *status);

unsigned long DoBatchActiveMkEx(KmcCbbCtx *kmcCtx, KmcOpMkParam *opMkParam, WsecBool shareDomainMkChanged);

unsigned long MultiKmcActivateMk(KmcCbbCtx *kmcCtx, KmcOpMkParam *opMkParam);

unsigned long MultiKmcSetRootKeyCfg(KmcCbbCtx *kmcCtx, const KmcCfgRootKey *rkCfg);

unsigned long MultiKmcGetRootKeyCfg(KmcCbbCtx *kmcCtx, KmcCfgRootKey *rkCfg);

unsigned long MultiKmcRefreshMkMask(KmcCbbCtx *kmcCtx);

unsigned long MultiKmcSecureEraseKeystore(KmcCbbCtx *kmcCtx);

unsigned long MultiKmcAddDomainMul(KmcCbbCtx *kmcCtx, const KmcCfgDomainInfo *domainInfo);

unsigned long MultiKmcRmvDomain(KmcCbbCtx *kmcCtx, WsecUint32 domainId);

unsigned long MultiKmcAddDomainKeyType(KmcCbbCtx *kmcCtx, WsecUint32 domainId, const KmcCfgKeyType *keyTypeCfg);

unsigned long MultiKmcRmvDomainKeyType(KmcCbbCtx *kmcCtx, WsecUint32 domainId, WsecUint16 keyType);

int MultiKmcGetDomainCount(KmcCbbCtx *kmcCtx);

unsigned long MultiKmcReGenerateKsf(KmcCbbCtx *kmcCtx);

unsigned long MultiKmcExportKsf(KmcCbbCtx *kmcCtx, const char *keystoreFile, KmcExportKsfCfg *exportKsfCfg);

unsigned long MultiKmcExportKsfByKeys(KmcCbbCtx *kmcCtx, const char *keystoreFile,
    const KmcExportKsfByKeysCfg *exportKsfCfg);

unsigned long MultiKmcImportKsf(KmcCbbCtx *kmcCtx, const char *keystoreFile, KmcImportKsfCfg *importKsfCfg);

unsigned long MultiKmcCheckKeyStore(KmcCbbCtx *kmcCtx, WsecBool rewriteOnCheckFail, unsigned long *rewriteErrorCode);

unsigned long MultiKmcGetUpdateNumFromMem(WsecHandle kmcCtx, WsecUint32 *updateCounter);
unsigned long MultiKmcGetUpdateNumFromFile(KmcCbbCtx *kmcCtx, const char *keystoreFile, WsecUint32 *updateCounter);

unsigned long MultiKmcGetSharedMkUpdateNumFromMem(KmcCbbCtx *kmcCtx, WsecUint32 *updateCounter);
unsigned long MultiKmcGetSharedMkUpdateNumFromFile(KmcCbbCtx *kmcCtx, const char *keystoreFile,
    WsecUint32 *updateCounter);

unsigned long AgentBatchRecvMk(KmcCbbCtx *kmcCtx, WsecUint32 recvMode, KmcSyncCtx *syncCtx,
    KmcRecvSyncInfo *syncInfo, WsecVoid *param, const CallbackRecvSyncData recvSyncData);

unsigned long MultiKmcAutoUpdateRk(KmcCbbCtx *kmcCtx, int updateDaysBefore);
/* Check whether each MK in a domain expires. If the MK expires at the latest time, the MK expires. */
unsigned long CheckWhetherHasMkToBeUpdated(KmcCbbCtx *kmcCtx, WsecUint32 domainId, const WsecSysTime *nowUTC,
    KmcMkInfo *mkInfo, int *expireRemainday);
/* Search for and check the domain type matching status. */
unsigned long CheckWhetherDomainMkWillUpdate(KmcCbbCtx *kmcCtx, WsecUint32 domainId, KmcMkInfo *mkInfo,
    int *expireRemainDay);

unsigned long MultiKmcAutoCheckDomainLatestMk(KmcCbbCtx *kmcCtx, WsecUint32 domainId, int advanceDay,
    WsecBool *hasMkToBeUpdated, KmcMkInfo *mkInfo, int *expireRemainDay);

unsigned long MultiKmcMasterBatchSendMk(KmcCbbCtx *kmcCtx, const KmcDomainArray *domainArray,
    WsecVoid *param, CallbackSendSyncData sendSyncData);

unsigned long MultiKmcAgentRecvCnf(KmcCbbCtx *kmcCtx, WsecHandle *ctx,
    WsecVoid *param, const CallbackSendSyncData sendSyncData, const CallbackRecvSyncData recvSyncData);

unsigned long MultiKmcMasterSendCnf(KmcCbbCtx *kmcCtx, WsecVoid *param,
    const CallbackSendSyncData sendSyncData, const CallbackRecvSyncData recvSyncData);

unsigned long MultiKmcGetMkHash(WsecHandle kmcCtx, WsecUint32 domainId, WsecUint32 keyId, unsigned char *hashData,
    WsecUint32 *hashLen);

unsigned long GetWkByActiveMk(KmcCbbCtx *kmcCtx, WsecUint32 domainId, WsecUint32 algId, KmcKeyTypesInfo keyTypes,
    KmcMkInfo *mkInfo, unsigned char *key, WsecUint32 *keyLen, WsecUint32 wkLen, KmcCnfId *ids,
    Pbkdf2Param *pbkdf2Param, WsecUint32 isSdpVersion1);

unsigned long GetWkByMkIDHash(KmcCbbCtx *kmcCtx, WsecUint32 domainId, WsecUint32 keyId, WsecUint32 algId,
    WsecUint32 kdfAlg, WsecUint32 iter, const unsigned char *salt, WsecUint32 saltLen, const unsigned char *hashData,
    WsecUint32 hashLen, unsigned char *key, WsecUint32 *keyLen, WsecUint32 wkLen);

unsigned long GetWkByMkDetail(KmcCbbCtx *kmcCtx, WsecUint32 domainId, WsecUint32 keyId, WsecUint32 algId, WsecUint32 kdfAlg,
    WsecUint32 iter, const unsigned char *salt, WsecUint32 saltLen, unsigned char *key, WsecUint32 *keyLen, WsecUint32 wkLen);

unsigned long GetWkByActiveMkWithHash(KmcCbbCtx *kmcCtx, WsecUint32 domainId, WsecUint32 *keyId, WsecUint32 algId,
    unsigned char *salt, WsecUint32 *saltLen, unsigned char *key, WsecUint32 *keyLen, WsecUint32 wkLen,
    unsigned char *hashData, size_t hashLen, KmcCnfId *ids);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* KMC_INCLUDE_KMCV2_MULTI_H */
