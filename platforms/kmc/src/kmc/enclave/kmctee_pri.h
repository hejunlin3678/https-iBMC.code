/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: kmc tee need struct
 * Author: liwei
 * Create: 2021-05-03
 */
#ifndef KMC_SRC_KMC_ENCLAVE_KMCTEE_PRI_H
#define KMC_SRC_KMC_ENCLAVE_KMCTEE_PRI_H

#include "wsecv2_config.h"
#if WSEC_COMPILE_ENABLE_TEE
#include "wsecv2_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define MAX_TEE_CMD_NUM 50

#define FIRST_PARAM 1
#define SECOND_PARAM 2
#define THIRD_PARAM 3
#define FOURTH_PARAM 4
#define SIXTH_PARAM 6
#define FIFTH_PARAM 5
#define SEVENTH_PARAM 7

#pragma pack(1)
typedef struct TagKmcTeeSdpInfo {
    WsecUint32 algId;
    WsecUint32 padOrWhichKey;  /* enc, dec is padding, hmac is which key */
    WsecUint32 pbkdfAlgId;
    WsecUint32 workKeyLen;
    WsecUint32 streamType; /* stream Type */
    WsecUint32 iter;
} KmcTeeSdpInfo;
#pragma pack()

#pragma pack(1)
typedef struct TagStreamInfo {
    WsecUint64 ctx;
    WsecUint32 streamType;
} StreamInfo;
#pragma pack()

typedef struct TagTlvArray {
    WsecUint32 num;  /* number */
    unsigned char *tags;  /* tag array */
    WsecBuffConst **paramArr;  /* fill buff array */
    WsecBuffConst *parseArr;  /* parse buff array */
    unsigned char *founds;  /* parse founds array */
} TlvArray;

typedef enum {
    TEE_CMD_CREATE_RK = 1,
    TEE_CMD_CREATE_MK,
    TEE_CMD_IMPEXP_MK,
    TEE_CMD_EXP_WK,
    TEE_CMD_HMAC,
    TEE_CMD_FREE_CTX,
    TEE_CMD_TA2TA_GET_PROTECTED_MK_LEN,
    TEE_CMD_TA2TA_GET_PROTECTED_MK,
    TEE_CMD_CREATE_DH_PAIR,
    TEE_CMD_AGENT_CREATE_DH_SHAREKEY,
    TEE_CMD_MASTER_CREATE_DH_SHAREKEY,
    TEE_CMD_MASTER_DH_ENCRYPT_MK,
    TEE_CMD_AGENT_DH_DECRYPT_MK,
    TEE_CMD_BATCH_GET_MK_HASH,
    TEE_CMD_SUPPORT_DH_ALG,
    TEE_CMD_UPDATE_SRK,
    TEE_CMD_ASYM_ENCRYPT, // 加密非对称私钥的
    TEE_CMD_CREATE_ASYM_KEY,
    TEE_CMD_SIGN,
    TEE_CMD_ASYM_DECRYPT, /* 20 */
    TEE_CMD_ENCRYPT = MAX_TEE_CMD_NUM + 1, /* 51 */
    TEE_CMD_DECRYPT,
    TEE_CMD_INIT_STREAM,
    TEE_CMD_UPDATE_STREAM,
    TEE_CMD_FINAL_STREAM, /* 55 */
} KmcCommandTeeId;

#pragma pack(1)
typedef struct TagKmcTeeSKHeader {
    WsecUint32 algId;
} KmcTeeSKHeader;
#pragma pack()

/* tlv type */
#define     KMC_TEE_SDPINFO_TYPE     1   /* KmcTeeSdpInfo */
#define     KMC_TEE_RK_TYPE          2
#define     KMC_TEE_MK_TYPE          3
#define     KMC_TEE_SALT_TYPE        4
#define     KMC_TEE_IV_TYPE          5
#define     KMC_TEE_ALG_TYPE         6
#define     KMC_TEE_DATA_TYPE        7
#define     KMC_TEE_IMP_EXP_TYPE     8     /* ImportFlag */
#define     KMC_TEE_STREAM_TYPE      9    /* StreamInfo */
#define     KMC_TEE_DHCURVE_TYPE     10
#define     KMC_TEE_DHOBJ_TYPE       11
#define     KMC_TEE_UPDATESRK_TYPE   12
#define     KMC_TEE_PROTECT_TYPE     13

#define     KMC_TEE_ASYM_SPEC_TYPE    14
#define     KMC_TEE_ASYM_KEY_USE_TYPE 15
#define     KMC_TEE_ASYM_KEY_TYPE     16
#define     KMC_TEE_OLD_RK_TYPE       17
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif // WSEC_COMPILE_ENABLE_TEE
#endif // KMC_SRC_KMC_ENCLAVE_KMCTEE_PRI_H
