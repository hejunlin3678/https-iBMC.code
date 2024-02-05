/**
 * ***************************************************************************
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description:
          提供CMVP通用模式、操作、算法类型的定义
 * File Name: ipsi_cmvp_def.h
 * Create: 2022-05-08
 * Author:
 *****************************************************************************
 */
#ifndef IPSI_CMVP_DEF_H
#define IPSI_CMVP_DEF_H

#include "ipsi_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 标准模式枚举 */
/**
 * @defgroup IPSI_CMVP_CryptoMode
 * @ingroup ipsi_cmvp_def
 * @par Prototype
 * @code
 * typedef enum {
 *     IPSI_CMVP_MODE_NONAPPROVED = 0,        // 非核准模式, 即无标准模式
 *     IPSI_CMVP_MODE_ISO19790,               // ISO 19790 标准默哀是
 * } IPSI_CMVP_CryptoMode;
 * @endcode
 *
 * @datastruct IPSI_CMVP_MODE_NONAPPROVED= 0 Mode of NonApproved.
 * @datastruct IPSI_CMVP_MODE_ISO19790= 1 Mode of ISO19790.
 */
typedef enum {
    IPSI_CMVP_MODE_NONAPPROVED = 0,        // 非核准模式, 即无标准模式
    IPSI_CMVP_MODE_ISO19790,               // ISO 19790 标准模式
    IPSI_CMVP_MODE_GM0028,                 // GM0028 标准模式
} IPSI_CMVP_CryptoMode;

/* Integrity check switch Enums */
typedef enum {
    IPSI_CRYPTO_CMVP_DISABLE_INTEGRITY_CHECK = 0, // disable check
    IPSI_CRYPTO_CMVP_ENABLE_INTEGRITY_CHECK,      // enable check
} IPSI_CMVP_IntegrityChkSwitch;

/* 密码算法服务操作类型枚举 */
/**
 * @defgroup IPSI_CMVP_SrvOper
 * @ingroup ipsi_cmvp_def
 * @par Prototype
 * @code
 * typedef enum {
 *     IPSI_CMVP_SRVOPER_NOT_DEF = 0,          // 未定义的缺省类型
 *     IPSI_CMVP_SRVOPER_ENC,                  // 加密操作
 *     IPSI_CMVP_SRVOPER_DEC,                  // 解密操作
 *     IPSI_CMVP_SRVOPER_KEY_GEN,              // 公私钥对秘钥生成操作
 *     IPSI_CMVP_SRVOPER_SIGN,                 // 签名操作
 *     IPSI_CMVP_SRVOPER_VERIFY,               // 验签操作
 *     IPSI_CMVP_SRVOPER_MD,                   // 计算 Hash 操作
 *     IPSI_CMVP_SRVOPER_MAC,                  // 计算消息认证码操作
 *     IPSI_CMVP_SRVOPER_KEY_AGREEMENT,        // 秘钥协商计算操作
 *     IPSI_CMVP_SRVOPER_KEY_DERIVE,           // 秘钥派生操作
 *     IPSI_CMVP_SRVOPER_RAND_INIT,            // DRBG 初始化操作
 *     IPSI_CMVP_SRVOPER_RAND_RESEED,          // DRBG 重新播种操作
 *     IPSI_CMVP_SRVOPER_RAND_GEN,             // DRBG 生成随机数操作
 *     IPSI_CMVP_SRVOPER_RAND_UNINIT,          // DRBG 对象析构操作
 *     IPSI_CMVP_SRVOPER_CSP_ZERO,             // CSP 清零操作(保留操作类型)
 * } IPSI_CMVP_SrvOper;
 * @endcode
 *
 * @datastruct IPSI_CMVP_SRVOPER_NOT_DEF= 0 Undefined default type.
 * @datastruct IPSI_CMVP_SRVOPER_ENC= 1 Encryption operation.
 * @datastruct IPSI_CMVP_SRVOPER_DEC= 2 Decryption operation.
 * @datastruct IPSI_CMVP_SRVOPER_KEY_GEN= 3 Generating a Public-Private Key Pair.
 * @datastruct IPSI_CMVP_SRVOPER_SIGN= 4 Signature operation.
 * @datastruct IPSI_CMVP_SRVOPER_VERIFY= 5 Verify operation.
 * @datastruct IPSI_CMVP_SRVOPER_MD= 6 Calculate Hash Operations.
 * @datastruct IPSI_CMVP_SRVOPER_MAC= 7 Calculating Message Authentication Codes.
 * @datastruct IPSI_CMVP_SRVOPER_KEY_AGREEMENT= 8 Key negotiation and calculation.
 * @datastruct IPSI_CMVP_SRVOPER_KEY_DERIVE= 9 Key derivation operation.
 * @datastruct IPSI_CMVP_SRVOPER_RAND_INIT= 10 DRBG Initialization.
 * @datastruct IPSI_CMVP_SRVOPER_RAND_RESEED= 11 DRBG reseed operation.
 * @datastruct IPSI_CMVP_SRVOPER_RAND_GEN= 12 DRBG Generating Random Numbers.
 * @datastruct IPSI_CMVP_SRVOPER_RAND_UNINIT= 13 DRBG Object Destructor Operation.
 * @datastruct IPSI_CMVP_SRVOPER_CSP_ZERO= 14 CSP Clear Operation (Reserved Operation Type).
 */
typedef enum {
    IPSI_CMVP_SRVOPER_NOT_DEF = 0,          // 未定义的缺省类型
    IPSI_CMVP_SRVOPER_ENC,                  // 加密操作
    IPSI_CMVP_SRVOPER_DEC,                  // 解密操作
    IPSI_CMVP_SRVOPER_KEY_GEN,              // 公私钥对秘钥生成操作
    IPSI_CMVP_SRVOPER_SIGN,                 // 签名操作
    IPSI_CMVP_SRVOPER_VERIFY,               // 验签操作
    IPSI_CMVP_SRVOPER_MD,                   // 计算 Hash 操作
    IPSI_CMVP_SRVOPER_MAC,                  // 计算消息认证码操作
    IPSI_CMVP_SRVOPER_KEY_AGREEMENT,        // 秘钥协商计算操作
    IPSI_CMVP_SRVOPER_KEY_DERIVE,           // 秘钥派生操作
    IPSI_CMVP_SRVOPER_RAND_INIT,            // DRBG 初始化操作
    IPSI_CMVP_SRVOPER_RAND_RESEED,          // DRBG 重新播种操作
    IPSI_CMVP_SRVOPER_RAND_GEN,             // DRBG 生成随机数操作
    IPSI_CMVP_SRVOPER_RAND_UNINIT,          // DRBG 对象析构操作
    IPSI_CMVP_SRVOPER_CSP_ZERO,             // CSP 清零操作(保留操作类型)
} IPSI_CMVP_SrvOper;

/* 密码算法类型枚举 */
/**
 * @defgroup IPSI_CMVP_AlgType
 * @ingroup ipsi_cmvp_def
 * @par Prototype
 * @code
 * typedef enum {
 *     IPSI_CMVP_ALG_NOT_DEF = 0,              // 未定义缺省类型
 *     IPSI_CMVP_ALG_CIPHER,                   // 对称加密算法
 *     IPSI_CMVP_ALG_MD,                       // Hash & MAC 算法
 *     IPSI_CMVP_ALG_PKEY,                     // 非对称公钥算法
 *     IPSI_CMVP_ALG_KEY_AGREEMENT,            // 秘钥协商算法
 *     IPSI_CMVP_ALG_KDF,                      // 秘钥派生算法
 *     IPSI_CMVP_ALG_DRBG,                     // 随机数生成算法
 * } IPSI_CMVP_AlgType;
 * @endcode
 *
 * @datastruct IPSI_CMVP_ALG_NOT_DEF= 0 Undefined default type.
 * @datastruct IPSI_CMVP_ALG_CIPHER= 1 symmetric encryption algorithm.
 * @datastruct IPSI_CMVP_ALG_MD= 2 Hash & MAC algorithm.
 * @datastruct IPSI_CMVP_ALG_PKEY= 3 asymmetric public key algorithm.
 * @datastruct IPSI_CMVP_ALG_KEY_AGREEMENT= 4 Key negotiation algorithm.
 * @datastruct IPSI_CMVP_ALG_KDF= 5 Key derivation algorithm.
 * @datastruct IPSI_CMVP_ALG_DRBG= 6 Random number generation algorithm.
 */
typedef enum {
    IPSI_CMVP_ALG_NOT_DEF = 0,              // 未定义缺省类型
    IPSI_CMVP_ALG_CIPHER,                   // 对称加密算法
    IPSI_CMVP_ALG_MD,                       // Hash & MAC 算法
    IPSI_CMVP_ALG_PKEY,                     // 非对称公钥算法
    IPSI_CMVP_ALG_KEY_AGREEMENT,            // 秘钥协商算法
    IPSI_CMVP_ALG_KDF,                      // 秘钥派生算法
    IPSI_CMVP_ALG_DRBG,                     // 随机数生成算法
} IPSI_CMVP_AlgType;

/* 指示信息结构体 */
/**
 * @defgroup IPSI_CMVP_IndicInfo
 * @ingroup ipsi_cmvp_def
 * @par Prototype
 * @code
 * typedef struct {
 *     IPSI_CMVP_AlgType algType;              // 密码算法类型
 *     SEC_INT32 algNid;                       // 密码算法的ID
 *     SEC_INT32 err;                          // 错误信息(保留字, 暂不被使用)
 * } IPSI_CMVP_IndicInfo;
 * @endcode
 *
 * @datastruct algType cryptographic algorithm Type.
 * @datastruct algNid ID of the cryptographic algorithm.
 * @datastruct err Error information (reserved word, not used currently).
 */
typedef struct {
    IPSI_CMVP_AlgType algType;              // 密码算法类型
    SEC_INT32 algNid;                       // 密码算法的ID
    SEC_INT32 err;                          // 错误信息(保留字, 暂不被使用)
} IPSI_CMVP_IndicInfo;

/* 服务信息输出函数钩子原型 */
/**
 * @defgroup IPSI_CMVP_SrvIndicFun
 * @ingroup ipsi_cmvp_def
 * @par Prototype
 * @code
 * typedef SEC_VOID (*IPSI_CMVP_SrvIndicFun)(IPSI_CMVP_SrvOper oper, const IPSI_CMVP_IndicInfo *info);
 * @endcode
 */
typedef SEC_VOID (*IPSI_CMVP_SrvIndicFun)(IPSI_CMVP_SrvOper oper, const IPSI_CMVP_IndicInfo *info);

/* CSP 清零信息输出函数钩子原型, 保留定义, 暂未使用 */
/**
 * @defgroup IPSI_CMVP_ZeroIndicFun
 * @ingroup ipsi_cmvp_def
 * @par Prototype
 * @code
 * typedef SEC_VOID (*IPSI_CMVP_ZeroIndicFun)(const IPSI_CMVP_IndicInfo *info);
 * @endcode
 */
typedef SEC_VOID (*IPSI_CMVP_ZeroIndicFun)(const IPSI_CMVP_IndicInfo *info);

#ifdef __cplusplus
}
#endif /* end of __cplusplus */

#endif // IPSI_CMVP_DEF_H
