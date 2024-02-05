/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Asymmetric cryptographic algorithm interface for KMC
 * Author: liuermeng
 * Create: 2022-08-05
 */

#ifndef CAC_ASYM_UTIL_H
#define CAC_ASYM_UTIL_H

#include "wsecv2_config.h"
#if WSEC_COMPILE_ENABLE_ASYM

#include "wsecv2_ctx.h"
#include "wsecv2_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define ED25519_PUB_LEN 32

#define SM2_PUB_LEN (2 * (32 + sizeof(WsecUint32))) // for sm2, pub key is 2 LV, |X|Y|
#define SM2_PRI_LEN (3 * (32 + sizeof(WsecUint32))) // for sm2, private key is 3 LV, |X|Y|Pri|

// for rsa, e is 0x10001, n is public, (n, d) is private
#define DER_EXT_LEN 20
#define RSA_2048_PUB_LEN 256
#define RSA_3072_PUB_LEN 384
#define RSA_4096_PUB_LEN 512
#define RSA_2048_PUB_DER_LEN (256 + DER_EXT_LEN)
#define RSA_3072_PUB_DER_LEN (384 + DER_EXT_LEN)
#define RSA_4096_PUB_DER_LEN (512 + DER_EXT_LEN)
#define RSA_2048_PRI_LEN (2 * RSA_2048_PUB_LEN + 2 * sizeof(WsecUint32))
#define RSA_3072_PRI_LEN (2 * RSA_3072_PUB_LEN + 2 * sizeof(WsecUint32))
#define RSA_4096_PRI_LEN (2 * RSA_4096_PUB_LEN + 2 * sizeof(WsecUint32))

#define ECC_P256_PUB_PRI_BYTES_NUM 32
#define ECC_P384_PUB_PRI_BYTES_NUM 48
#define ECC_P521_PUB_PRI_BYTES_NUM 66
#define SM2_PUBLIC_BYTES_NUM 32
#define ECC_P256_PUB_BYTES_NUM (2 * ECC_P256_PUB_PRI_BYTES_NUM + 2 * sizeof(WsecUint32))
#define ECC_P384_PUB_BYTES_NUM (2 * ECC_P384_PUB_PRI_BYTES_NUM + 2 * sizeof(WsecUint32))
#define ECC_P521_PUB_BYTES_NUM (2 * ECC_P521_PUB_PRI_BYTES_NUM + 2 * sizeof(WsecUint32))
#define MAX_CIPHER_LEN 1024

typedef struct {
    int pad;
    WsecAlgId padMDAlgo;
    WsecAlgId mgf1MDAlgo;
} CacPaddingInfo;

typedef struct {
    WsecAlgId digestAlgo;
    CacPaddingInfo padInfo;
} CacAsymOpParam;

typedef struct {
    WsecBuff x;
    WsecBuff y;
    WsecBuff pri;
} EccKeyParts;

unsigned long CacUtilGetOpParam(WsecAlgId algo, CacAsymOpParam *opParam);

unsigned long CacUtilExtractConstBuf(KmcCbbCtx *kmcCtx, WsecBuffConst *buf, WsecBuffConst *buffers[], WsecUint32 num);

unsigned long CacUtilFillEcPub(KmcCbbCtx *kmcCtx, const WsecBuff *xBuf, const WsecBuff *yBuf, WsecBuff *pub);

unsigned long CacUtilFillEcPri(KmcCbbCtx *kmcCtx, int nid, const WsecBuff *priBuf, WsecBuff *pub, WsecBuff *pri);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif // WSEC_COMPILE_ENABLE_ASYM

#endif