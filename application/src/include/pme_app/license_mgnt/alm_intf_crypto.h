/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: 定义AdaptiveLM使用到的算法相关的回调函数
 * Author: AdaptiveLM team
 * Create: 2022-05-26
 */
#ifndef ALM_INTF_CRYPTO_H
#define ALM_INTF_CRYPTO_H

#include "alm_intf_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* if __cplusplus */
#endif /* ifdef __cplusplus */

#if defined(ALM_PACK_4)
#pragma pack(4)
#elif defined(ALM_PACK_8)
#pragma pack(8)
#endif


#define ALM_CRYPTO_AES_BLOCKBITS        128

typedef enum {
    ALM_CRYPTO_AES_MODE_ECB = 0,
    ALM_CRYPTO_AES_MODE_CBC = 1,
} ALM_CRYPTO_AES_MODE_ENUM;

typedef enum {
    ALM_CRYPTO_AES_PADDING_NO = 0,
} ALM_CRYPTO_AES_PADDING_ENUM;

typedef struct {
    ALM_BOOL bEncrypt;
    ALM_UINT32 uiBlockBits;
    ALM_ENUM(ALM_CRYPTO_AES_MODE_ENUM) eMode;
    ALM_ENUM(ALM_CRYPTO_AES_PADDING_ENUM) ePadding;

    CONST ALM_INT8 *pcKey;
    ALM_UINT32 uiKeyLen;
    CONST ALM_INT8 *pcIv;
    ALM_UINT32 uiIvLen;
    CONST ALM_INT8 *pcIn;
    ALM_UINT32 uiInLen;

    ALM_INT8 *pcOut;
    ALM_UINT32 uiOutBufLen;
    ALM_UINT32 uiOutRealLen;
} ALM_CRYPTO_AES_STRU;

/* AES函数
 * 秘钥长度固定为128位
 * 操作成功，返回ALM_OK；操作失败，返回其他值，即不返回ALM_OK
 */
typedef ALM_INT32 (*ALM_ADAPTER_CRYPTO_AES_FUNC)(IN OUT ALM_CRYPTO_AES_STRU *pPara);


typedef struct {
    CONST ALM_INT8 *pcIn;
    ALM_UINT32 uiInLen;
} ALM_CRYPTO_SHA_SOURCE_STRU;

typedef enum {
    ALM_CRYPTO_SHA_TYPE_SHA1 = 0,
    ALM_CRYPTO_SHA_TYPE_SHA256 = 1,
} ALM_CRYPTO_SHA_TYPE_ENUM;

typedef struct {
    ALM_ENUM(ALM_CRYPTO_SHA_TYPE_ENUM) eType;

    ALM_CRYPTO_SHA_SOURCE_STRU *pSource;
    ALM_UINT32 uiSourceNum;

    ALM_INT8 *pcOut;
    ALM_UINT32 uiOutBufLen;
    ALM_UINT32 uiOutRealLen;
} ALM_CRYPTO_SHA_STRU;

/* SHA函数
 * 操作成功，返回ALM_OK；操作失败，返回其他值，即不返回ALM_OK
 */
typedef ALM_INT32 (*ALM_ADAPTER_CRYPTO_SHA_FUNC)(IN OUT ALM_CRYPTO_SHA_STRU *pPara);


typedef enum {
    ALM_CRYPTO_ECDSA_TYPE_ECC163 = 0,
    ALM_CRYPTO_ECDSA_TYPE_ECC233 = 1,
    ALM_CRYPTO_ECDSA_TYPE_ECC283 = 2,
} ALM_CRYPTO_ECDSA_TYPE_ENUM;

typedef struct {
    ALM_ENUM(ALM_CRYPTO_ECDSA_TYPE_ENUM) eType;

    CONST ALM_INT8 *pcHashValue;
    ALM_UINT32 uiHashValueLen;

    CONST ALM_INT8 *pcR;
    ALM_UINT32 uiRLen;
    CONST ALM_INT8 *pcS;
    ALM_UINT32 uiSLen;

    /* 二元域多项式表达的EC算法不可约多项式维度 */
    ALM_UINT32 uiEcbpDeg;
    /* 公钥Q点的x坐标, Q = d*G */
    CONST ALM_INT8 *pcEcbpQx;
    ALM_UINT32 uiEcbpQxLen;
    /* 公钥Q点的y坐标, Q = d*G */
    CONST ALM_INT8 *pcEcbpQy;
    ALM_UINT32 uiEcbpQyLen;
} ALM_CRYPTO_ECDSA_STRU;

/* ECDSA函数 */
typedef ALM_INT32 (*ALM_ADAPTER_CRYPTO_ECDSA_FUNC)(IN OUT ALM_CRYPTO_ECDSA_STRU *pPara);


typedef enum {
    ALM_CRYPTO_RSA_CRYPTO_TYPE_PUBLICKEY_DECRYPTO = 0,
} ALM_CRYPTO_RSA_CRYPTO_TYPE_ENUM;

typedef struct {
    ALM_ENUM(ALM_CRYPTO_RSA_CRYPTO_TYPE_ENUM) eType;

    CONST ALM_INT8 *pcIn;
    ALM_UINT32 uiInLen;

    /* RSA模数的bit长度,衡量算法强度的一个指标 */
    ALM_UINT32 uiBlockBits;
    /* RSA模数n   */
    CONST ALM_INT8 *pcModN;
    ALM_UINT32 uiModN;
    /* RSA验证密钥e */
    CONST ALM_INT8 *pcExpE;
    ALM_UINT32 uiExpE;

    ALM_INT8 *pcOut;
    ALM_UINT32 uiOutBufLen;
    ALM_UINT32 uiOutRealLen;
} ALM_CRYPTO_RSA_STRU;

/* ECDSA函数 */
typedef ALM_INT32 (*ALM_ADAPTER_CRYPTO_RSA_FUNC)(IN OUT ALM_CRYPTO_RSA_STRU *pPara);


typedef struct {
    ALM_UINT32 uiBlockBits;
    ALM_ENUM(ALM_CRYPTO_SHA_TYPE_ENUM) eShaType;

    CONST ALM_INT8 *pcPlainHash;
    ALM_UINT32 uiPlainHashLen;

    CONST ALM_INT8 *pcCipherHash;
    ALM_UINT32 uiCipherHashLen;
} ALM_CRYPTO_PSS_STRU;

/* ECDSA函数 */
typedef ALM_INT32 (*ALM_ADAPTER_CRYPTO_PSS_FUNC)(IN OUT ALM_CRYPTO_PSS_STRU *pPara);

/* 加密算法回调函数统一格式 */
typedef ALM_INT32 (*ALM_ADAPTER_CRYPTO_UNIFY_FUNC)(IN OUT ALM_VOID *pPara);


typedef struct {
    ALM_ADAPTER_CRYPTO_AES_FUNC fnAes;
    ALM_ADAPTER_CRYPTO_SHA_FUNC fnSha;
    ALM_ADAPTER_CRYPTO_ECDSA_FUNC fnEcdsa;
    ALM_ADAPTER_CRYPTO_RSA_FUNC fnRsa;
    ALM_ADAPTER_CRYPTO_PSS_FUNC fnPss;
} ALM_ADAPTER_CRYPTO_FUNCS_STRU;

/*
 * 应用回调注册函数：应用层通过该函数向AdaptiveLM注册加密算法回调函数。
 * AdaptiveLM在处理业务时，需要使用到几个加密算法，分别为AES、SHA、RSA、ECDSA、PSS。
 * 在AdaptiveLM内部，当使用加密算法处理业务时，其大致有两大类选择：
 * (1)AdaptiveLM直接调用加密算法库函数（如调用Openssl函数，或者调用AdaptiveLM自实现的加密算法库函数）;
 * (2)AdaptiveLM调用应用层注册的加密算法回调函数。
 * AdaptiveLM都是直接调用加密算法库函数，不依赖应用层注册，
 * 则应用层也不需要调用ALM_AdapterCryptoFuncs向AdaptiveLM注册加密算法回调函数。
 * 当应用层决定调用ALM_AdapterCryptoFuncs给AdaptiveLM提供加密算法回调函数时，需要告知AdaptiveLM项目组，
 * 从而，当AdaptiveLM项目组给该产品提供库时，会进行一个配置，
 * 使得AdaptiveLM在使用加密算法处理业务时，AdaptiveLM调用应用层注册的加密算法回调函数。
 */
LIC_EXPORT ALM_INT32 ALM_AdapterCryptoFuncs(IN CONST ALM_ADAPTER_CRYPTO_FUNCS_STRU *pstCryptoFuncs);


#if (defined(ALM_PACK_4) || defined(ALM_PACK_8))
#pragma pack()
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* if __cplusplus */
#endif /* ifdef __cplusplus */
#endif /* ALM_INTF_CRYPTO_H */
