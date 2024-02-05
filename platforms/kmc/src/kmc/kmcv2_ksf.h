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

#ifndef KMC_SRC_KMC_KMCV2_KSF_H
#define KMC_SRC_KMC_KMCV2_KSF_H

#include "wsecv2_type.h"
#include "kmcv2_pri.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define KMC_RMK_DOUBLE_LEN  ((KMC_RMK_LEN) * 2) /* EK+IK */
#define KMC_MAX_MK_RECORD_LEN WSEC_MAX_HARD_CIPHERTEXT_LEN

const unsigned char *KsfGetksfFlag(WsecVoid);

/* Checking the KSF */
unsigned long KsfCheckKeyStore(KmcCbbCtx *kmcCtx, WsecBool rewriteOnCheckFail,
    unsigned long *rewriteErrorCode, KmcKsfMem *ksfMem);

/* Safely Erasing KSF */
unsigned long SecureEraseKeystore(KmcCbbCtx *kmcCtx);

/* KSF Get KSF Update Times */
unsigned long GetKsfUpdateNumberFromKeystore(KmcCbbCtx *kmcCtx, const char *keystoreFile, WsecUint32 *updateCounter);

/* Number of Times the KSF Obtains Shared MK Updates */
unsigned long GetSharedMkUpdateNumberFromKsf(KmcCbbCtx *kmcCtx, const char *keystoreFile, WsecUint32 *updateCounter);

/* KSM generates KSF */
unsigned long GenKsf(KmcCbbCtx *kmcCtx, KmcKsfMem *ksfMem, const char *keystoreFile, const char *callBy);

/* Secure KSF writing (dual-copy DR considered) */
unsigned long WriteKsfSafety(KmcCbbCtx *kmcCtx, WsecBool shareDomainMkChanged, const KmcKsfRk *rkNew, KmcKsfMem *ksfMem,
    const char *callBy);

/*
 * Securely reads Keystore data.
 * The system supports two Keystore files. If an error occurs during data reading, the system reads another file.
 * 1. Preferentially select the data that can be correctly read from the file.
 * 2. If all the values are correctly read, the number of KSF updates is large.
 * 3. If the master node detects a file exception, it uses the normal file to overwrite the abnormal file.
 * 4. If the file is an agent file, the file is directly read. If one file is normal, the file can be loaded.
 * Note: This function writes the successfully read Keystore data into the global variable keystore.
 */
unsigned long ReadKsfSafety(KmcCbbCtx *kmcCtx, WsecUint32 role);
/*
 * Reads data from a specified Keystore file (KSF for short).
 * Note: WSEC_ERR_KMC_READ_MK_FAIL indicates that an error occurs in the process of reading the media key,
 * However, the read MK is correctly output to keystoreData->mkArray.
 */
unsigned long ReadKsf(KmcCbbCtx *kmcCtx, const char *keystoreFile, const char *callBy, KmcKsfMem *ksfMem,
    WsecBool *isAsym);

/*
 * KmcKsfHardRk indicates the root key. (The root key is removed when Update, RewriteOnCheckfail,
 * or Erase is executed. The root key is not removed when a new key is created because it is empty.)
 */
WsecVoid KmcRmvFreeKsfHardRk(KmcKsfHardRk *hardRk);

unsigned long IsMemAndKsfCnfSame(KmcCbbCtx *kmcCbbCtx, WsecBool *isSame);

unsigned long ReadKsfData(KmcCbbCtx *kmcCtx, WsecHandle keystore, WsecHandle *hashCtx, WsecUint32 ctxNum,
    WsecVoid *buff, WsecUint32 len);

unsigned long ReadKsfDataWithoutDigest(KmcCbbCtx *kmcCtx, WsecHandle keystore, WsecVoid *buff, WsecUint32 len);

unsigned long ReadLvInKsf(KmcCbbCtx *kmcCtx, WsecHandle keystore, WsecHandle *hashCtx, WsecUint32 ctxNum,
    WsecUint32 maxLen, WsecVoid **buff, WsecUint32 *len);

unsigned long WriteKsfData(KmcCbbCtx *kmcCtx, WsecHandle keystore, WsecHandle *hashCtx, WsecUint32 ctxNum,
    const WsecVoid *buff, WsecUint32 len);

WsecBool CheckKsfExistsNoneEmpty(KmcCbbCtx *kmcCtx, const char *file);

unsigned long CheckKsfHashAndHmac(KmcCbbCtx *kmcCtx, const KmcKsfHmac *calcHmac, WsecUint32 hashLen,
    const KmcKsfHmac *ksfHmac, WsecUint32 hmacLen);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* KMC_SRC_KMC_KMCV2_KSF_H */
