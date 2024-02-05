/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: openssl Asymmetric cryptographic algorithm functions
 * Author: liuermeng
 * Create: 2022-08-05
 */
#ifndef KMC_SRC_CAC_CAC_ASYM_FUNS_H
#define KMC_SRC_CAC_CAC_ASYM_FUNS_H

#include "wsecv2_config.h"
#if WSEC_COMPILE_ENABLE_ASYM

#if defined(WSEC_COMPILE_CAC_OPENSSL) || defined(WSEC_COMPILE_CAC_IPSI)

#include "openssl/ec.h"
#include "cacv2_pri.h"


EVP_PKEY_CTX *CacEvpPkeyCtxNewId(KmcCbbCtx *kmcCtx, int id, ENGINE *e);

int CacEvpPkeyKeygenInit(KmcCbbCtx *kmcCtx, EVP_PKEY_CTX *ctx);

int CacEvpPkeyKeygen(KmcCbbCtx *kmcCtx, EVP_PKEY_CTX *ctx, EVP_PKEY **ppkey);

void CacEvpPkeyFree(KmcCbbCtx *kmcCtx, EVP_PKEY *x);

void CacEvpPkeyCtxFree(KmcCbbCtx *kmcCtx, EVP_PKEY_CTX *ctx);

int CacEvpPkeyGetRawPublicKey(KmcCbbCtx *kmcCtx, const EVP_PKEY *pkey, unsigned char *pub, size_t *len);

int CacEvpPkeyGetRawPrivateKey(KmcCbbCtx *kmcCtx, const EVP_PKEY *pkey, unsigned char *priv, size_t *len);

RSA *CacEvpPkeyGet0RSA(KmcCbbCtx *kmcCtx, EVP_PKEY *pkey);

const BIGNUM *CacRSAGet0n(KmcCbbCtx *kmcCtx, const RSA *d);
const BIGNUM *CacRSAGet0e(KmcCbbCtx *kmcCtx, const RSA *d);
const BIGNUM *CacRSAGet0d(KmcCbbCtx *kmcCtx, const RSA *d);

int CacBNbn2bin(KmcCbbCtx *kmcCtx, const BIGNUM *a, unsigned char *to);

int CacEvpPkeyParamgenInit(KmcCbbCtx *kmcCtx, EVP_PKEY_CTX *ctx);

int CacEvpPkeyCtxCtrl(KmcCbbCtx *kmcCtx, EVP_PKEY_CTX *ctx, int keytype, int optype, int cmd, int p1, void *p2);

BIGNUM *CacBNNew(KmcCbbCtx *kmcCtx);

EC_KEY *CacEvpPkeyGet0EcKey(KmcCbbCtx *kmcCtx, EVP_PKEY *pkey);

int CacEcPointGetAffineCoordinates(KmcCbbCtx *kmcCtx, const EC_GROUP *group, const EC_POINT *p, BIGNUM *x, BIGNUM *y, BN_CTX *ctx);

const EC_GROUP *CacEcKeyGet0Group(KmcCbbCtx *kmcCtx, const EC_KEY *key);

const EC_POINT *CacEcKeyGet0PublicKey(KmcCbbCtx *kmcCtx, const EC_KEY *key);

const BIGNUM *CacEcKeyGet0PrivateKey(KmcCbbCtx *kmcCtx, const EC_KEY *key);

int CacBNBn2binpad(KmcCbbCtx *kmcCtx, const BIGNUM *a, unsigned char *to, int tolen);

void CacBNFree(KmcCbbCtx *kmcCtx, BIGNUM *a);

EVP_MD_CTX *CacEvpMdCtxNew(KmcCbbCtx *kmcCtx);

int CacEvpDigestSignInit(KmcCbbCtx *kmcCtx, EVP_MD_CTX *ctx, EVP_PKEY_CTX **pctx, const EVP_MD *type, ENGINE *e, EVP_PKEY *pkey);

int CacEvpDigestSign(KmcCbbCtx *kmcCtx, EVP_MD_CTX *ctx, unsigned char *sigret, size_t *siglen, const unsigned char *tbs, size_t tbslen);

void CacEvpMdCtxFree(KmcCbbCtx *kmcCtx, EVP_MD_CTX *ctx);

EVP_PKEY *CacEvpPkeyNewRawPrivateKey(KmcCbbCtx *kmcCtx, int type, ENGINE *e, const unsigned char *priv, size_t len);

int CacEvpDigestInitEx(KmcCbbCtx *kmcCtx, EVP_MD_CTX *ctx, const EVP_MD *type, ENGINE *impl);
int CacEvpDigestUpdate(KmcCbbCtx *kmcCtx, EVP_MD_CTX *ctx, const void *d, size_t cnt);
int CacEvpDigestFinalEx(KmcCbbCtx *kmcCtx, EVP_MD_CTX *ctx, unsigned char *md, unsigned int *s);

EVP_MD *CacEvpMdMethDup(KmcCbbCtx *kmcCtx, const EVP_MD *md);
void CacEvpMdMethFree(KmcCbbCtx *kmcCtx, EVP_MD *md);

int CacEvpPkeySignInit(KmcCbbCtx *kmcCtx, EVP_PKEY_CTX *ctx);
int CacEvpPkeySign(KmcCbbCtx *kmcCtx, EVP_PKEY_CTX *ctx, unsigned char *sig, size_t *siglen, const unsigned char *tbs, size_t tbslen);

RSA *CacRSANew(KmcCbbCtx *kmcCtx);
int CacRSASet0Key(KmcCbbCtx *kmcCtx, RSA *r, BIGNUM *n, BIGNUM *e, BIGNUM *d);
void CacRSAFree(KmcCbbCtx *kmcCtx, RSA *r);

EVP_PKEY *CacEvpPkeyNew(KmcCbbCtx *kmcCtx);

EC_KEY *CacEcKeyNewByCurveName(KmcCbbCtx *kmcCtx, int nid);
int CacEcKeySetPublicKeyAffineCoordinates(KmcCbbCtx *kmcCtx, EC_KEY *key, BIGNUM *x, BIGNUM *y);
int CacEcKeySetPrivateKey(KmcCbbCtx *kmcCtx, EC_KEY *key, const BIGNUM *prv);
int CacEvpPkeySet1EcKey(KmcCbbCtx *kmcCtx, EVP_PKEY *pkey, EC_KEY *key);
int CacEvpPkeySetAliasType(KmcCbbCtx *kmcCtx, EVP_PKEY *pkey, int type);

int CacEvpDigestVerifyInit(KmcCbbCtx *kmcCtx, EVP_MD_CTX *ctx, EVP_PKEY_CTX **pctx, const EVP_MD *type, ENGINE *e, EVP_PKEY *pkey);

EVP_PKEY *CacEvpPkeyNewRawPublickey(KmcCbbCtx *kmcCtx, int type, ENGINE *e, const unsigned char *pub, size_t len);

int CacEvpDigestVerify(KmcCbbCtx *kmcCtx, EVP_MD_CTX *ctx, const unsigned char *sigret, size_t siglen, const unsigned char *tbs, size_t tbslen);

int CacEvpPkeyVerify(KmcCbbCtx *kmcCtx, EVP_PKEY_CTX *ctx, const unsigned char *sig, size_t siglen, const unsigned char *tbs, size_t tbslen);

int CacEvpPkeyEncryptInit(KmcCbbCtx *kmcCtx, EVP_PKEY_CTX *ctx);
int CacEvpPkeyEncrypt(KmcCbbCtx *kmcCtx, EVP_PKEY_CTX *ctx, unsigned char *out, size_t *outlen, const unsigned char *in, size_t inlen);
int CacEvpPkeyDecryptInit(KmcCbbCtx *kmcCtx, EVP_PKEY_CTX *ctx);
int CacEvpPkeyDecrypt(KmcCbbCtx *kmcCtx, EVP_PKEY_CTX *ctx, unsigned char *out, size_t *outlen, const unsigned char *in, size_t inlen);

ECDSA_SIG *CacECDSASIGNew(KmcCbbCtx *kmcCtx);
int CacECDSASIGSet0(KmcCbbCtx *kmcCtx, ECDSA_SIG *sig, BIGNUM *r, BIGNUM *s);
int Caci2dECDSASIG(KmcCbbCtx *kmcCtx, const ECDSA_SIG *sig, unsigned char **pp);
void CacECDSASIGFree(KmcCbbCtx *kmcCtx, ECDSA_SIG *sig);

int CacBNNumBits(KmcCbbCtx *kmcCtx, const BIGNUM *a);
int CacBNNumBytes(KmcCbbCtx *kmcCtx, const BIGNUM *a);

const EVP_MD *CacEvpSha256(KmcCbbCtx *kmcCtx);
const EVP_MD *CacEvpSha384(KmcCbbCtx *kmcCtx);
const EVP_MD *CacEvpSha512(KmcCbbCtx *kmcCtx);
const EVP_MD *CacEvpSm3(KmcCbbCtx *kmcCtx);

int CacRSAPkeyCtxCtrl(KmcCbbCtx *kmcCtx, EVP_PKEY_CTX *ctx, int optype, int cmd, int p1, void *p2);

EVP_PKEY_CTX *CacEvpPkeyCtxNew(KmcCbbCtx *kmcCtx, EVP_PKEY *pkey, ENGINE *e);

BIGNUM *CacBNbin2bn(KmcCbbCtx *kmcCtx, const unsigned char *s, int len, BIGNUM *ret);

void CacEcKeyFree(KmcCbbCtx *kmcCtx, EC_KEY *key);
int CacEvpPkeyVerifyInit(KmcCbbCtx *kmcCtx, EVP_PKEY_CTX *ctx);
int CacEvpMdCtxReset(KmcCbbCtx *kmcCtx, EVP_MD_CTX *ctx);
int CacEvpVerifyFinal(KmcCbbCtx *kmcCtx, EVP_MD_CTX *ctx,
    const unsigned char *sigbuf, unsigned int siglen, EVP_PKEY *pkey);
int CacEvpSignFinal(KmcCbbCtx *kmcCtx, EVP_MD_CTX *ctx,
    unsigned char *md, unsigned int *s, EVP_PKEY *pkey);
int CacEvpPkeyAssign(KmcCbbCtx *kmcCtx, EVP_PKEY *pkey, int type, void *key);
void CacCryptoFree(KmcCbbCtx *kmcCtx, void *str, const char *file, int line);

int CacAsn1GetObject(KmcCbbCtx *kmcCtx, const unsigned char **pp, long *plength, int *ptag, int *pclass, long omax);

int Caci2dPublicKey(KmcCbbCtx *kmcCtx, EVP_PKEY *a, unsigned char **pp);
EVP_PKEY *Cacd2iPublicKey(KmcCbbCtx *kmcCtx, int type, EVP_PKEY **a, const unsigned char **pp, long length);

#endif
#endif // WSEC_COMPILE_ENABLE_ASYM
#endif
