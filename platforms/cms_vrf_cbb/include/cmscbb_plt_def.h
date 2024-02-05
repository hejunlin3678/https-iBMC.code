/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2015-2018. All rights reserved.
 * Description: Signature Verify CBB Library
 * Author: tangzonglei
 * Create: 2015
 * History: 2018/11/23 yebin code rule fixes
 */

/*
 *    File Name            :     cmscbb_sys_def.h
 *    Brief                :     system function definition File
 *    Author               :     liuhao
 *    Date                 :     2015/01/14 9:03
 */
#ifndef H_CMSCBB_PLT_DEF_H
#define H_CMSCBB_PLT_DEF_H
#include "cmscbb_config.h"

#ifdef __cplusplus
extern "C" {
#endif /* --cplusplus */

#ifdef CMSCBB_ENABLE_LOG

/* Define log output level */
#define CMSCBB_LOG_LEVEL_DEBUG 4 /* Output debugging, information, alarms, and error logs */
#define CMSCBB_LOG_LEVEL_INFO 3 /* Output information, alarms, and error logs */
#define CMSCBB_LOG_LEVEL_WARNING 2 /* Output Alarms and error logs */
#define CMSCBB_LOG_LEVEL_ERROR 1 /* Output error Log */
#define CMSCBB_LOG_LEVEL_NONE 0 /* Do not output log */

#ifndef CMSCBB_LOG_LEVEL1
#define CMSCBB_LOG_LEVEL1 CMSCBB_LOG_LEVEL_INFO
#endif

/* log type */
typedef enum {
    CMSCBB_LOG_TYPE_ERROR = 0,
    CMSCBB_LOG_TYPE_WARNING = 1,
    CMSCBB_LOG_TYPE_INFO = 2,
    CMSCBB_LOG_TYPE_DEBUG = 3,
} CMSCBB_LOG_TYPE;
#endif /* CMSCBB_ENABLE_LOG */

/*
 * crypto definition
 */
/* sigcbb Digest algorithm definition */
    #define	CMSCBB_HASH_UNKONW (-1)
    #define	CMSCBB_HASH_SHA256 43
#if CMSCBB_SUPPORT_SHA384
    #define	CMSCBB_HASH_SHA384 44
#endif
#if CMSCBB_SUPPORT_SHA512
    #define	CMSCBB_HASH_SHA512 45
#endif
    #define CMSCBB_HASH_SM3    46

/* sigcbb Digest algorithm definition */
    #define	CMSCBB_ENC_RSA     53
    #define	CMSCBB_ENC_RSA_PSS 62
    #define CMSCBB_ENC_SHA256WITHRSAENCRYPTION 63
#if CMSCBB_SUPPORT_SHA384
    #define CMSCBB_ENC_SHA384WITHRSAENCRYPTION 64
#endif
#if CMSCBB_SUPPORT_SHA512
    #define CMSCBB_ENC_SHA512WITHRSAENCRYPTION 65
#endif
    #define	CMSCBB_ENC_ECDSA   400
    #define CMSCBB_ENC_SHA256WITHECDSA 412
#if CMSCBB_SUPPORT_SHA384
    #define CMSCBB_ENC_SHA384WITHECDSA 413
#endif
#if CMSCBB_SUPPORT_SHA512
    #define CMSCBB_ENC_SHA512WITHECDSA 414
#endif

/* sigcbb EC curve name definition */
#define	CMSCBB_NIST_CURVE_P256   407
#define	CMSCBB_NIST_CURVE_P384   420
#define	CMSCBB_NIST_CURVE_P512   421
#define	CMSCBB_BRAINPOOL_CURVE_P256   408
#define	CMSCBB_BRAINPOOL_CURVE_P384   422
#define	CMSCBB_BRAINPOOL_CURVE_P512   423

#define CMSCBB_ENC_SM2   500
#define CMSCBB_CURVE_SM2   503
#define CMSCBB_ENC_SM2WITHSM3   505

/* SIGCBB algorithm middle-tier context data definition */
typedef void* CMSCBB_CRYPTO_MD_CTX;
typedef void* CMSCBB_CRYPTO_VRF_CTX;

/* Define Maximum summary length */
#ifndef CMSCBB_MAX_DIGEST_SIZE
#define CMSCBB_MAX_DIGEST_SIZE 64
#endif

/* Define the maximum length of encryption */
#ifndef CMSCBB_MAX_CRYPT_SIZE
#define CMSCBB_MAX_CRYPT_SIZE 516
#endif

/* Maximum public key data length */
#ifndef CMSCBB_MAX_PUBKEY_SIZE
#define CMSCBB_MAX_PUBKEY_SIZE (1024 << 8)
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* H_CMSCBB_PLT_DEF_H */
