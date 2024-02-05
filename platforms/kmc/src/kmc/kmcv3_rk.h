/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: KMC internal interface header file, which is not open to external systems.
 * Author: z00316590
 * Create: 2019-03-16
 */

#ifndef KMC_SRC_KMC_KMCV3_RK_H
#define KMC_SRC_KMC_KMCV3_RK_H

#include "wsecv2_type.h"
#include "kmcv2_pri.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/*
 * Initialize the hardware root key environment and invoke the hardware adaptation layer
 * interface to initialize the hardware environment.
 */
unsigned long KmcHardRkInit(KmcCbbCtx *kmcCtx, const WsecInternalInitParam *initParam);

/* Assign KmcKsfHardRk to another buffer. Note that reference counting does not need to be changed. */
void KmcAssignKsfHardRk(KmcKsfHardRk *src, KmcKsfHardRk *dest);

void KmcAssignKsfTeeRk(WsecVoid *src, WsecUint32 srcLen, WsecVoid *dest, WsecUint32 destLen);
/* KmcKsfHardRk clone. The value of reference counting increases by 1. */
unsigned long KmcHardRkCloneMember(KmcCbbCtx *kmcCtx, const KmcKsfHardRk *src, KmcKsfHardRk *dest);

/* Creating a Hardware Root Key */
unsigned long KmcHardRkNew(KmcCbbCtx *kmcCtx, KmcKsfHardRk *hardRk, KmcTeeRkCreate *enclaveRk,
    WsecSrkPrimitive **primitives);

/*
 * Remove the hardware root key. Note that the count is processed first.
 * If the count is 0, the hardware root key is removed.
 */
WsecVoid KmcHardRkRemove(KmcCbbCtx *kmcCtx, const KmcKsfHardRk *hardRk);

/*
 * Loads the hardware root key. If the hardware root key has been loaded, the counter increases by 1.
 * Otherwise, the hardware root key is loaded.
 */
unsigned long KmcHardRkLoad(KmcCbbCtx *kmcCtx, const KmcKsfHardRk *hardRk);

/*
 * KmcKsfHardRk indicates the root key. (The root key is removed when Update, RewriteOnCheckfail, or Erase is executed.
 * The root key is not removed when a new key is created because it is empty.)
 */
WsecVoid KmcKsfHardRkRmvFree(KmcCbbCtx *kmcCtx, KmcKsfHardRk *hardRk);

/*
 * Unload the hardware root key and release the memory.
 * The root key can still be loaded next time based on persistent information.
 */
WsecVoid KmcKsfHardRkUnloadFree(KmcCbbCtx *kmcCtx, KmcKsfHardRk *hardRk);

/* For hardware-based root keys, KSF V3 re-encrypts software-layer root keys. */
unsigned long KmcHardRkReEncSrk(KmcCbbCtx *kmcCtx, KmcKsfHardRk *hardRk);

/*
 * Use the hardware root key to encrypt information, including the software-layer root key and KSF HMAC KEY.
 * If the software-layer root key is not used, the MK needs to be encrypted.
 */
unsigned long KmcEncByHrk(KmcCbbCtx *kmcCtx, const KmcKsfHardRk *hardRk,
    const unsigned char *plaintext, unsigned int plainLen,
    unsigned char *ciphertext, unsigned int *cipherLen);

/*
 * Use the hardware root key to decrypt information, including the software-layer root key and KSF HMAC KEY.
 * If the software-layer root key is not used, the MK needs to be decrypted.
 */
unsigned long KmcDecByHrk(KmcCbbCtx *kmcCtx, const KmcKsfHardRk *hardRk,
    const unsigned char *ciphertext, unsigned int cipherLen,
    unsigned char *plaintext, unsigned int *plainLen);

/* Encrypt the MK using the root key of the software layer. */
unsigned long KmcEncBySrk(KmcCbbCtx *kmcCtx, const KmcKsfHardRk *hardRk,
    const WsecUint32 algId,
    const unsigned char *iv, unsigned int ivLen,
    const WsecBuffConst *plainBuff,
    unsigned char *ciphertext, unsigned int *cipherLen);

/* Decrypt the MK using the software-layer root key. */
unsigned long KmcDecBySrk(KmcCbbCtx *kmcCtx, const KmcKsfHardRk *hardRk,
    const WsecUint32 algId,
    const unsigned char *iv, unsigned int ivLen,
    const WsecBuffConst *cipherBuff,
    unsigned char *plaintext, unsigned int *plainLen);

/* Calculating the KSFHMAC using the KSF HMAC key */
unsigned long KmcHardRkGetKsfHmac(KmcCbbCtx *kmcCtx, const KmcKsfMem *ksfMem,
    WsecUint32 alg,
    KmcKsfHmac *ksfHmac,
    WsecUint32 hashLen,
    WsecUint32 *hmacLen);

/* Deinitialize the hardware root key adaptation layer. */
unsigned long KmcHardRkUninit(KmcCbbCtx *kmcCtx);

/*
 * Use the memory protection technology to remove the protection
 * software-layer root key and KSF HMAC key from the memory.
 */
unsigned long KmcUnprotectDataForMaskedKey(KmcCbbCtx *kmcCtx);

/* Protecting software-layer root keys and KSF HMAC keys using memory protection technologies */
unsigned long KmcProtectDataForMaskedKey(KmcCbbCtx *kmcCtx);

/* Converts the byte order of the RK information in the KSF. */
WsecVoid CvtByteOrderForKsfRk(KmcKsfRk *rk, WsecUint32 direction, WsecUint16 ksfVersion);

/* Creating a Root Key File */
unsigned long KmcCreateRootKey(const WsecBuffConst *entropy, WsecUint16 ksfVersion, KmcCbbCtx *kmcCtx, KmcKsfRk *rk,
    KmcKsfMem *ksfMem, KmcTeeRkCreate *enclaveRk);

/* Verifying the RK Hash Value to Obtain the Version Number */
unsigned long KmcCheckRk(KmcCbbCtx *kmcCtx, KmcKsfRk *rk, WsecUint16 ksfVersion);

/* Constructs a root key derivation key (RMK) to protect the MK. */
WsecBool KmcMakeRmk(KmcCbbCtx *kmcCtx, const KmcKsfRk *rk, const WsecBuff *rmkBuff);

/* Protecting the Root Key Material Memory */
unsigned long KmcProtectRkMaterials(KmcCbbCtx *kmcCtx, KmcKsfRk *rk);

/* Root key material protection is removed. */
unsigned long KmcUnprotectRkMaterials(KmcCbbCtx *kmcCtx, KmcKsfRk *rk);

unsigned long KsfUpdateRootKey(KmcCbbCtx *kmcCtx, WsecBuffConst *entropyBuff, KmcKsfMem *ksfMem, WsecUint16 ksfVersion,
    WsecBool upgradeV4);

unsigned long KsfUpdateRootKeyMem(KmcCbbCtx *kmcCtx, KmcKsfMem *ksfMem, WsecUint16 ksfVersion, WsecBool isUpgrade);

unsigned long AllocMkRecordAndDeriveRmk(KmcCbbCtx *kmcCtx, const KmcKsfRk *rkBuff, WsecBuff *mkRecord, WsecBuff *rmk);

unsigned long KmcGetRealKey(KmcCbbCtx *kmcCtx, const KmcKsfHardRk *hardRk, WsecUint32 algId, KmcMaskedKey *realKey,
    WsecUint32 *keyLen);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* KMC_SRC_KMC_KMCV3_RK_H */
