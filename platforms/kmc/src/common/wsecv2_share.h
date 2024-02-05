/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2014-2020. All rights reserved.
 * Description: internal interface of the KMC, which is not open to external systems
 * Author: x00102361
 * Create: 2014-06-16
 */

#ifndef KMC_SRC_COMMON_WSECV2_SHARE_H
#define KMC_SRC_COMMON_WSECV2_SHARE_H

#include "wsecv2_type.h"
#include "wsecv2_itf.h"
#include "wsecv2_lock.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * hardwareParam struct :
 * hasSoftLevelRk   : indicate software level rk is uesed
 * hdCtx            : hdCtx.buff to hold hardwareParam for security hardware
 *                    hdctx.len to hold hardwareParam length for security hardware
 **/
typedef struct TagHardwareParam {
    WsecBool hasSoftLevelRk;
    WsecBuffConst hdCtx;
} KmcHardwareParam;

#define HEADER_TAG_LEN_SIZE 5

typedef enum TagKmcTeeStreamType {
    TEE_STREAM_NULL,
    TEE_STREAM_HMAC,
    TEE_STREAM_ENC,
    TEE_STREAM_DEC,
    TEE_STREAM_ENCGCM, /* 16 byte tag */
    TEE_STREAM_DECGCM,
} KmcTeeStreamType;

#pragma pack(1)
typedef struct TagKmcTeeMkHmac {
    WsecUint32 version;
    WsecUint32 algId;
    unsigned char salt[32];
    unsigned char reserve[28];
    WsecUint32 hmacLen;
    unsigned char hmacData[];
} KmcTeeMkHmac;

typedef struct TagKmcTeeMkCipher {
    WsecUint32 version;
    WsecUint32 algId;
    unsigned char iv[16];
    unsigned char tag[16];
    unsigned char salt[32];
    unsigned char reserve[28];
    WsecUint32 cipherLen;
    unsigned char keyCipher[];
} KmcTeeMkCipher;
#pragma pack()

#pragma pack(1)
typedef struct TagKmcTeeMk {
    WsecUint32 algId;      /* Algorithm information , ensure that the algorithm ID is within 255. */
    WsecUint8 iv[16];      /* gcm may is 12 */
    WsecUint8 tag[16];  /* tag */
    WsecUint8 keyCipher[];  /* cipherText */
} KmcTeeMk;
#pragma pack()

/**
 * roleType          : init roleType of kmc see KmcRoleType
 * KmcKsfName        : the KmcKsfName for ksf files. see KmcKsfName
 * enableThirdBackup : enable thrid backup file support
 * deleteKsfOnFailed : delete ksf when init failed(when enableThirdBackup set to true)
 * enableHw          : init with hardware if true is behavior equal to WsecInitializeHw
 * HardwareParam     : the KmcHardwareParam when init with hardware is used,
 *                     when init with software mode hdparm is ignored
 **/
typedef struct TagInitParams {
    WsecUint32 roleType;
    const KmcKsfName *filePathName;
    WsecBool enableThirdBackup;
    WsecBool deleteKsfOnInitFailed;
    WsecBool enableHw;
    KmcHardwareParam hdParm;
    KmcAlgCnfParam algParm;
    WsecBool    enableEnclave;
    const KmcTeeInitParam *teeInitParam;
    WsecBool    haveHw;
    unsigned char keyStoreMode;
    unsigned char userDefineSrk;
    const KmcKsfName *extFilePathName;
} WsecInternalInitParam;

/*
 * reserve: reserved parameter, which is not used currently.
 * Note: This function is not a direct external function and needs to be called using WsecInitializeEx.
 */
unsigned long KmcInitializeEx(WsecHandle kmcCtx, const WsecInternalInitParam *initParam, KmcLockOrNot lockOrNot);

/* Note: This function is not an external function and needs to be triggered by WsecFinalizeEx. */
unsigned long KmcFinalizeEx(WsecHandle kmcCtx);

/* KMC module reset. This function is not an external function and needs to be triggered by WsecResetEx. */
unsigned long KmcResetEx(WsecHandle kmcCtx, WsecBool isHardware, WsecBuffConst hardwareParam, WsecBool isEnclave,
    WsecBool isHw, const KmcTeeInitParam *teeParam);

/* KMC module role setting */
unsigned long KmcSetRoleType(WsecHandle kmcCtx, WsecUint32 roleType);

unsigned long KmcFinalizeMul(WsecHandle kmcCtx);

unsigned long InitializeKmc(WsecHandle kmcCtx, const WsecInternalInitParam *initParam);

unsigned long PrepareInitializeKmc(const WsecHandle kmcCtx, const KmcInitParam *initParam,
    WsecInternalInitParam *internalParam, WsecBool isMultiInit);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* KMC_SRC_COMMON_WSECV2_SHARE_H */
