/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: ����AdaptiveLMʹ�õ����㷨��صĻص�����
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

/* AES����
 * ��Կ���ȹ̶�Ϊ128λ
 * �����ɹ�������ALM_OK������ʧ�ܣ���������ֵ����������ALM_OK
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

/* SHA����
 * �����ɹ�������ALM_OK������ʧ�ܣ���������ֵ����������ALM_OK
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

    /* ��Ԫ�����ʽ����EC�㷨����Լ����ʽά�� */
    ALM_UINT32 uiEcbpDeg;
    /* ��ԿQ���x����, Q = d*G */
    CONST ALM_INT8 *pcEcbpQx;
    ALM_UINT32 uiEcbpQxLen;
    /* ��ԿQ���y����, Q = d*G */
    CONST ALM_INT8 *pcEcbpQy;
    ALM_UINT32 uiEcbpQyLen;
} ALM_CRYPTO_ECDSA_STRU;

/* ECDSA���� */
typedef ALM_INT32 (*ALM_ADAPTER_CRYPTO_ECDSA_FUNC)(IN OUT ALM_CRYPTO_ECDSA_STRU *pPara);


typedef enum {
    ALM_CRYPTO_RSA_CRYPTO_TYPE_PUBLICKEY_DECRYPTO = 0,
} ALM_CRYPTO_RSA_CRYPTO_TYPE_ENUM;

typedef struct {
    ALM_ENUM(ALM_CRYPTO_RSA_CRYPTO_TYPE_ENUM) eType;

    CONST ALM_INT8 *pcIn;
    ALM_UINT32 uiInLen;

    /* RSAģ����bit����,�����㷨ǿ�ȵ�һ��ָ�� */
    ALM_UINT32 uiBlockBits;
    /* RSAģ��n   */
    CONST ALM_INT8 *pcModN;
    ALM_UINT32 uiModN;
    /* RSA��֤��Կe */
    CONST ALM_INT8 *pcExpE;
    ALM_UINT32 uiExpE;

    ALM_INT8 *pcOut;
    ALM_UINT32 uiOutBufLen;
    ALM_UINT32 uiOutRealLen;
} ALM_CRYPTO_RSA_STRU;

/* ECDSA���� */
typedef ALM_INT32 (*ALM_ADAPTER_CRYPTO_RSA_FUNC)(IN OUT ALM_CRYPTO_RSA_STRU *pPara);


typedef struct {
    ALM_UINT32 uiBlockBits;
    ALM_ENUM(ALM_CRYPTO_SHA_TYPE_ENUM) eShaType;

    CONST ALM_INT8 *pcPlainHash;
    ALM_UINT32 uiPlainHashLen;

    CONST ALM_INT8 *pcCipherHash;
    ALM_UINT32 uiCipherHashLen;
} ALM_CRYPTO_PSS_STRU;

/* ECDSA���� */
typedef ALM_INT32 (*ALM_ADAPTER_CRYPTO_PSS_FUNC)(IN OUT ALM_CRYPTO_PSS_STRU *pPara);

/* �����㷨�ص�����ͳһ��ʽ */
typedef ALM_INT32 (*ALM_ADAPTER_CRYPTO_UNIFY_FUNC)(IN OUT ALM_VOID *pPara);


typedef struct {
    ALM_ADAPTER_CRYPTO_AES_FUNC fnAes;
    ALM_ADAPTER_CRYPTO_SHA_FUNC fnSha;
    ALM_ADAPTER_CRYPTO_ECDSA_FUNC fnEcdsa;
    ALM_ADAPTER_CRYPTO_RSA_FUNC fnRsa;
    ALM_ADAPTER_CRYPTO_PSS_FUNC fnPss;
} ALM_ADAPTER_CRYPTO_FUNCS_STRU;

/*
 * Ӧ�ûص�ע�ắ����Ӧ�ò�ͨ���ú�����AdaptiveLMע������㷨�ص�������
 * AdaptiveLM�ڴ���ҵ��ʱ����Ҫʹ�õ����������㷨���ֱ�ΪAES��SHA��RSA��ECDSA��PSS��
 * ��AdaptiveLM�ڲ�����ʹ�ü����㷨����ҵ��ʱ���������������ѡ��
 * (1)AdaptiveLMֱ�ӵ��ü����㷨�⺯���������Openssl���������ߵ���AdaptiveLM��ʵ�ֵļ����㷨�⺯����;
 * (2)AdaptiveLM����Ӧ�ò�ע��ļ����㷨�ص�������
 * AdaptiveLM����ֱ�ӵ��ü����㷨�⺯����������Ӧ�ò�ע�ᣬ
 * ��Ӧ�ò�Ҳ����Ҫ����ALM_AdapterCryptoFuncs��AdaptiveLMע������㷨�ص�������
 * ��Ӧ�ò��������ALM_AdapterCryptoFuncs��AdaptiveLM�ṩ�����㷨�ص�����ʱ����Ҫ��֪AdaptiveLM��Ŀ�飬
 * �Ӷ�����AdaptiveLM��Ŀ����ò�Ʒ�ṩ��ʱ�������һ�����ã�
 * ʹ��AdaptiveLM��ʹ�ü����㷨����ҵ��ʱ��AdaptiveLM����Ӧ�ò�ע��ļ����㷨�ص�������
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
