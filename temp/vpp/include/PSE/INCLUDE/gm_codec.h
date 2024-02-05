/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * FileName    : gm_codec.h
 * Description : 国密编解码相关接口
 * Author: vpp
 * Create: 2022-08-17
 */

#ifndef GM_CODEC_H
#define GM_CODEC_H

#include "sec_sys.h"
#include "asn-types.h"
#include "sec_list.h"
#include "sec_error.h"
#include "sec_crypto.h"
#include "x509_basictypes.h"
#include "x509v3_extn.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SYMM_MAX_KEY_LEN 32

/**
 * @defgroup gm
 * This section contains the gm Enums, Structures, and Functions.
 */
/**
 * @defgroup gmEnum
 * @ingroup gm
 * This section contains the gm Enum.
 */
 /**
 * @defgroup gmFunctions
 * @ingroup gm
 * This section contains the gm Functions.
 */
 /**
 * @defgroup gmStructures
 * @ingroup gm
 * This section contains the gm Structures.
 */
/**
 * @defgroup SM2_CIPHERTEXT_E
 * @ingroup gmEnum
 * @par Prototype
 * @code
 * typedef enum
 * {
 *   SM2_CIPHERTEXT_C123 = 0,
 *   SM2_CIPHERTEXT_C132,
 *   SM2_CIPHERTEXT_MAX
 * } SM2_CIPHERTEXT_E;
 * @endcode
 *
 * @datastruct SM2_CIPHERTEXT_C123 = 0 国密加密密文的拼接顺序是C1|C2|C3
 * @datastruct SM2_CIPHERTEXT_C132 = 1 国密加密密文的拼接顺序是C1|C3|C2
 */
typedef enum {
    SM2_CIPHERTEXT_C123 = 0,
    SM2_CIPHERTEXT_C132,
    SM2_CIPHERTEXT_MAX
} SM2_CIPHERTEXT_E;
/**
 * @defgroup SEC_sm2PkeaCipherTextEncode
 * @ingroup gmFunctions
 * @par Prototype
 * @code
 * SEC_UCHAR *SEC_sm2PkeaCipherTextEncode(SEC_UCHAR *pucCipher, SEC_UINT32 ulCLen, SM2_CIPHERTEXT_E eCipherMode,
 *     SEC_UINT32 *pulEncLen)
 * @endcode
 *
 * @par Purpose

 * @par Description
 * 将Cipher的加密数据转换成DER码流输出
 *
 * @param[in] pucCipher 加密数据
 * @param[in] ulCLen pucCipher指向的长度
 * @param[in] eCipherMode SM2_CIPHERTEXT_E枚举
 * @param[out] pulEncLen 返回值指针指向的长度，若返回指针为空，则该值在SEC_sm2PkeaCipherTextEncode不被赋值
 *
 * @retval 成功 [返回指针不为空]
 * @retval 失败 [返回指针空]
 * @par Dependency
 *
 * @par Note
 * 加密数据中的C1数据不支持压缩
 * @li Memory operation: alloc、free、size
 *    申请了pulEncLen指向长度的内存，该内存由调用者调用 ipsi_free 释放内存
 * @li Thread safe:
 *     线程安全函数
 * @li OS difference:
 *     无OS差异
 * @li Time consuming:
 *     不耗时
 * @par Related Topics
 * N/A
 *
 */
SEC_UCHAR *SEC_sm2PkeaCipherTextEncode(SEC_UCHAR *pucCipher, SEC_UINT32 ulCLen, SM2_CIPHERTEXT_E eCipherMode,
    SEC_UINT32 *pulEncLen);
/**
 * @defgroup SEC_sm2PkeaCipherTextDecode
 * @ingroup gmFunctions
 * @par Prototype
 * @code
 * SEC_UCHAR *SEC_sm2PkeaCipherTextDecode(SEC_UCHAR *pucEncBuf, SEC_UINT32 ulEncLen, SM2_CIPHERTEXT_E eCipherMode,
 *     SEC_UINT32 *pulDecodeLen)
 * @endcode
 *
 * @par Purpose
 * @par Description
 * 将eCipherMode格式的加密数据编码成DER码流
 *
 * @param[in] pucEncBuf 加密数据的DER码流
 * @param[in] ulEncLen pucEncBuf指向长度
 * @param[in] eCipherMode SM2_CIPHERTEXT_E枚举成员
 * @param[out] pulDecodeLen 返回值指针指向的长度，若返回指针为空，则该值在SEC_sm2PkeaCipherTextDecode不被赋值
 *
 * @retval 成功 [返回指针不为空]
 * @retval 失败 [返回指针空]
 * @par Dependency
 *
 * @par Note
 * 加密数据中的C1数据不支持压缩输出
 * @li Memory operation: alloc、free、size
 *    申请了pulLenDecoded指向长度的内存，该内存由调用者调用 ipsi_free 释放内存
 * @li Thread safe:
 *     线程安全函数
 * @li OS difference:
 *     无OS差异
 * @li Time consuming:
 *     不耗时
 * @par Related Topics
 * N/A
 *
 */
SEC_UCHAR *SEC_sm2PkeaCipherTextDecode(SEC_UCHAR *pucEncBuf, SEC_UINT32 ulEncLen, SM2_CIPHERTEXT_E eCipherMode,
    SEC_UINT32 *pulDecodeLen);

/**
 * @defgroup SYM_ALG_KEY_S
 * @ingroup gmStructures
 * @par Prototype
 * @code
 * typedef struct stSymmAlgKey {
 *    SEC_UCHAR ucKey[SYMM_MAX_KEY_LEN];
 *    SEC_UINT32 uiKeyLen;
 * } SYM_
 * @endcode
 *
 * @datastruct SM2_CIPHERTEXT_C123 = 0 国密加密密文的拼接顺序是C1|C2|C3
 * @datastruct SM2_CIPHERTEXT_C132 = 1 国密加密密文的拼接顺序是C1|C3|C2
 */
typedef struct stSymmAlgKey {
    SEC_UCHAR ucKey[SYMM_MAX_KEY_LEN];
    SEC_UINT32 uiKeyLen;
} SYM_ALG_KEY_S;


/**
 * @defgroup SM2_ENVELOPED_SYM_ALG_E
 * @ingroup gmEnum
 * @par Prototype
 * @code
 * typedef enum {
 *    SM2_ENVELOPED_SYM_ALG_SM4 = 0,
 *    SM2_ENVELOPED_SYM_ALG_AES128,
 *    SM2_ENVELOPED_SYM_ALG_AES192,
 *    SM2_ENVELOPED_SYM_ALG_AES256,
 *    SM2_ENVELOPED_SYM_ALG_MAX
 * } SM2_ENVELOPED_SYM_ALG_E;
 * @endcode
 *
 * @datastruct SM2_ENVELOPED_SYM_ALG_SM4      SM4 ECB 算法
 * @datastruct SM2_ENVELOPED_SYM_ALG_AES128   AES128 ECB 算法
 * @datastruct SM2_ENVELOPED_SYM_ALG_AES192   AES192 ECB 算法
 * @datastruct SM2_ENVELOPED_SYM_ALG_AES256   AES256 ECB 算法
 */
typedef enum {
    SM2_ENVELOPED_SYM_ALG_SM4 = 0,
    SM2_ENVELOPED_SYM_ALG_AES128,
    SM2_ENVELOPED_SYM_ALG_AES192,
    SM2_ENVELOPED_SYM_ALG_AES256,
    SM2_ENVELOPED_SYM_ALG_MAX
} SM2_ENVELOPED_SYM_ALG_E;

/**
 * @defgroup SEC_EncryptSm2EnvlopedKey
 * @ingroup gmFunctions
 * @par Prototype
 * @code
 * SEC_UCHAR *SEC_EncryptSm2EnvlopedKey(SEC_PKEY_S *pstEncryptPubKey, SEC_PKEY_S *pstPairKey,
 *     SM2_ENVELOPED_SYM_ALG_E eSymAlgoID, SYM_ALG_KEY_S *symAlgoParam, SEC_UINT32 *puiEncLen)
 * @endcode
 *
 * @par Purpose
 * @par Description
 * 制作国密信封
 *
 * @param[in] pstEncryptPubKey 加密信封的公钥
 * @param[in] pstPairKey 信封存储的公私钥对
 * @param[in] eSymAlgoID 信封加密对称算法，支持类型见枚举 SM2_ENVELOPED_SYM_ALG_E
 * @param[in] symAlgoParam 对称加密参数，为空时内部通过随机数生成对称密钥；不为空时表示用户传入密钥，外部需保证密钥生成的安全性
 * @param[out] puiEncLen 生成信封数据流长度
 *
 * @retval SEC_UCHAR* 信封der数据流，数据格式遵循GM/T 009-2012 标准中密钥对保护数据格式 [信封der数据流|N/A]
 * @retval SEC_UCHAR* 信封制作失败 [SEC_NULL|N/A]
 * @par Dependency
 *
 * @par Note
 * @li Memory operation: alloc、free、size
 *    申请了puiEncLen指向长度的内存，该内存由调用者调用 ipsi_free 释放内存
 * @li Thread safe:
 *     线程安全函数
 * @li OS difference:
 *     无OS差异
 * @li Time consuming:
 *     50ms in aarch64
 * @par Related Topics
 * N/A
 *
 */
SEC_UCHAR *SEC_EncryptSm2EnvlopedKey(SEC_PKEY_S *pstEncryptPubKey, SEC_PKEY_S *pstPairKey,
    SM2_ENVELOPED_SYM_ALG_E eSymAlgoID, SYM_ALG_KEY_S *symAlgoParam, SEC_UINT32 *puiEncLen);

/**
 * @defgroup SEC_DecryptSm2EnvelopedKey
 * @ingroup gmFunctions
 * @par Prototype
 * @code
 * SEC_UINT32 SEC_DecryptSm2EnvelopedKey(SEC_PKEY_S *pstDeCryptPriKey, const SEC_CHAR *pcEnvelopKeyFile,
 *     SEC_INT iKeyFileType, SEC_PKEY_S **ppstPairKey)
 * @endcode
 *
 * @par Purpose
 * @par Description
 * 解密SM2 国密信封
 *
 * @param[in] pstDeCryptPriKey 解密信封的私钥
 * @param[in] pcEnvelopKeyFile 国密信封文件，信封格式遵循GM/T 009-2012 标准中密钥对保护数据格式
 * @param[in] iKeyFileType 信封文件格式，当前仅支持 der格式，对应值 SEC_PKI_FILETYPE_ASN1
 * @param[out] ppstPairKey 从信封中解密出的密钥对，由内部分配空间，调用者需要通过 CRYPT_PKEY_free 释放
 *
 * @retval SEC_UINT32 成功 [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 无效的参数 [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 无效的参数，空指针 [SEC_ERR_NULL_PTR|N/A]
 * @retval SEC_UINT32 文件读取失败，文件不存在或没有读取权限 [SEC_GM_ERR_READ_FILE_FAILED|N/A]
 * @retval SEC_UINT32 国密数字信封解码失败，如信封格式错误 [SEC_GM_ERR_ENVELOPED_DECODE_FAILED|N/A]
 * @retval SEC_UINT32 国密数字信封对称解密失败，信封中对称密钥不正确 [SEC_GM_ERR_ENVELOPED_SYM_DECR_FAILED|N/A]
 * @retval SEC_UINT32 国密数字信封私钥解密失败，使用的私钥无法解密信封 [SEC_GM_ERR_ENVELOPED_ASYM_DECR_FAILED|N/A]
 * @retval SEC_UINT32 国密数字信封中公私钥非匹配密钥对 [SEC_GM_ERR_ENVELOPED_PKEY_NOT_MATCH_ERR|N/A]
 * @retval SEC_UINT32 国密数字信封中对称算法不支持 [SEC_GM_ERR_ENVELOPED_SYM_ALG_ERR|N/A]
 * @retval SEC_UINT32 从国密数字信封获取密钥对失败，内存申请或拷贝错误导致失败 [SEC_GM_ERR_ENVELOPED_CONSTRUCT_PKEY_ERR|N/A]
 * @par Dependency
 *
 * @par Note
 * @li Memory operation: alloc、free、size
 *    申请了ppstKey指向长度的内存，该内存由调用者调用 CRYPT_PKEY_free 释放内存
 * @li Thread safe:
 *     线程安全函数
 * @li OS difference:
 *     无OS差异
 * @li Time consuming:
 *     80ms in aarch64
 * @par Related Topics
 * N/A
 *
 */
SEC_UINT32 SEC_DecryptSm2EnvelopedKey(SEC_PKEY_S *pstDeCryptPriKey, const SEC_CHAR *pcEnvelopKeyFile,
    SEC_INT iKeyFileType, SEC_PKEY_S **ppstPairKey);

#ifdef __cplusplus
}
#endif
#endif
