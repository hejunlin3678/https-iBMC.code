/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2014-2020. All rights reserved.
 * Description: KMC internal interface header file, which is not open to external systems.
 * Author: x00102361
 * Create: 2014-06-16
 * History: 2018-10-08 Zhang Jie (employee ID: 00316590) Rectification by UK
 * On March 19, 2019, Zhang Jie (employee ID: 00316590) split the original kmcv2_itf.c file
 * into kmcv2_ksf.c/kmcv2_ksm.c/kmcv2_itf.c/kmcv2_cfg.c to meet the requirements of the 2000-line file.
 *                     ITF: interface
 *                     KSM: keystore memory
 *                     KSF: keystore file
 */

#ifndef KMC_SRC_KMC_KMCV2_CFG_H
#define KMC_SRC_KMC_KMCV2_CFG_H

#include "wsecv2_type.h"
#include "kmcv2_pri.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define KMC_SALTLEN_MIN     16
#define KMC_SALTLEN_MAX     32
#define KMC_TLV_VALUE_LEN_MAX 64
#define KMC_TLV_EXT_LEN_MAX 512
#define KMC_MK_SUPPORT_KEY_TYPE_COUNT 3

/* Data protection algorithms provided by the data protection module */
typedef enum {
    SDP_ALG_ENCRPT,                     /* Encryption */
    SDP_ALG_INTEGRITY,                  /* Integrity protection */
    SDP_ALG_PWD_PROTECT                 /* Password protection */
} KmcSdpAlgType;

/* Obtains the upper limit of the MK. */
int GetMkCountMax(KmcCbbCtx *kmcCtx);

/* Obtaining the Grace Period of an Expired Key */
int CfgGetGraceDaysAfterKeyExpired(KmcCbbCtx *kmcCtx);

/* Checking the KSF */
unsigned long CfgCheckKeyStore(KmcCbbCtx *kmcCtx, WsecBool rewriteOnCheckFail, unsigned long *rewriteErrorCode,
    KmcKsfMem *ksfMem);

/* Updating the KSF Root Key */
unsigned long CfgUpdateRootKey(KmcCbbCtx *kmcCtx, WsecBuffConst *entropyBuff, KmcKsfMem *ksfMem, WsecUint16 ksfVersion,
    WsecBool upgradeV4);

/* Load the KSF. If the KSF does not exist, create it. */
unsigned long CfgReadKsfSafety(KmcCbbCtx *kmcCtx, WsecUint32 role);

/* Whether the configuration is initialized */
WsecBool CfgIsCfgValid(KmcCbbCtx *kmcCtx);

/* Obtains the domain type. If only a failure is returned, the possible cause is that the domain does not exist. */
unsigned long CfgGetDomainType(KmcCbbCtx *kmcCtx, WsecUint32 domain, unsigned char *domainType);

/* Obtains the domain keyTYype. */
unsigned long CfgGetDomainKeyType(KmcCbbCtx *kmcCtx, WsecUint32 domain, WsecUint16 *keyType);

/*
 * KMC configuration data initialization: Obtain the KMC configuration from the app through the callback function.
 * If a configuration item fails to be obtained, the default configuration is used.
 */
unsigned long CfgDataInitEx(KmcCbbCtx *kmcCtx, const WsecInternalInitParam *initParam);

/* Search for the domain configuration and key type configuration based on domainId and keyType. */
WsecBool CfgSearchDomainKeyTypeCfg(KmcCbbCtx *kmcCtx, WsecUint32 domainId, WsecUint16 keyType, KmcDomainCfg **domain,
    KmcCfgKeyType **keyTypeResult);

/* Releases g_kmcCfg. */
void CfgFreeKmcCfg(KmcCbbCtx *kmcCtx);

/* Setting the Maximum Number of Master Keys */
void CfgSetMkMaxCount(KmcCbbCtx *kmcCtx, int count);

/* Setting the Default Root Key */
WsecVoid CfgSetDefaultRootKeyCfg(KmcCbbCtx *kmcCtx, const KmcCfgRootKey *rkCfg);

/* Setting the Root Key */
WsecVoid CfgSetRootKeyCfg(KmcCbbCtx *kmcCtx, const KmcCfgRootKey *rkCfg);

/* Obtaining the Root Key Configuration */
WsecVoid CfgGetRootKeyCfg(KmcCbbCtx *kmcCtx, KmcCfgRootKey *rkCfg);

/* Adding a Domain to the Memory */
unsigned long CfgAddDomainEx(KmcCbbCtx *kmcCtx, const KmcCfgDomainInfo *domainInfo);

/* Deleting a Domain from the Memory */
unsigned long CfgRmvDomainEx(KmcCbbCtx *kmcCtx, WsecUint32 domainId);

/* Adding a Master Key Type to a Specified Domain */
unsigned long CfgAddDomainKeyTypeEx(KmcCbbCtx *kmcCtx, WsecUint32 domainId, const KmcCfgKeyType *keyTypeCfg);

/* Deleting a Master Key from a Specified Domain */
unsigned long CfgRmvDomainKeyTypeEx(KmcCbbCtx *kmcCtx, WsecUint32 domainId, WsecUint16 keyType);

/* Obtaining the Number of Master Keys */
int CfgGetDomainCount(KmcCbbCtx *kmcCtx);

/* Obtains the execution index domain info. */
unsigned long CfgGetDomain(KmcCbbCtx *kmcCtx, int idx, KmcCfgDomainInfo *domainInfo);

unsigned long CfgGetDomainById(KmcCbbCtx *kmcCtx, WsecUint32 domainId, KmcDomainCfg **domain);

WsecUint32 CfgGetHashAlg(KmcCbbCtx *kmcCtx);

WsecUint32 CfgGetHmacAlg(KmcCbbCtx *kmcCtx);

WsecUint32 CfgGetKdfAlg(KmcCbbCtx *kmcCtx);

WsecUint32 CfgGetSymAlg(KmcCbbCtx *kmcCtx);

WsecUint32 CfgGetWorkkeyIter(KmcCbbCtx *kmcCtx);

WsecUint32 CfgGetSaltLen(KmcCbbCtx *kmcCtx);

unsigned long CfgCheckDomainHaveOtherKey(KmcCbbCtx *kmcCtx, WsecUint32 domainId, WsecUint16 expectKeyType);

unsigned long MemGetKeyClassFromSymm(KmcKsfMem *ksfMem, WsecUint32 domainId, WsecUint16 *keyTypeClass);

unsigned long MemGetKeyClassFromNova(KmcKsfMem *ksfMem, WsecUint32 domainId, WsecUint16 *keyTypeClass);

unsigned long MemGetKeyClassByType(WsecUint16 keyType, WsecUint16 *keyTypeClass);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* KMC_SRC_KMC_KMCV2_CFG_H */
