/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: basic encryption function, which adapts to the mbedtls interface.
 * Author: gaoyu g00517475
 * Create: 2022-08-03
 */

#ifndef CACV2_ASYM_OPENSSL_H
#define CACV2_ASYM_OPENSSL_H
#include "wsecv2_config.h"
#if defined(WSEC_COMPILE_CAC_OPENSSL) || defined(WSEC_COMPILE_CAC_IPSI)

#include "openssl/ec.h"
#include "openssl/err.h"
#include "openssl/evp.h"

typedef EVP_PKEY_CTX *(* evp_pkey_ctx_new_id)(int id, ENGINE *e);
typedef int (* evp_pkey_paramgen_init)(EVP_PKEY_CTX *ctx);
typedef int (* evp_pkey_ctx_ctrl)(EVP_PKEY_CTX *ctx, int keytype, int optype, int cmd, int p1, void *p2);
typedef int (* evp_pkey_keygen_init)(EVP_PKEY_CTX *ctx);
typedef int (* evp_pkey_keygen)(EVP_PKEY_CTX *ctx, EVP_PKEY **ppkey);
typedef int (* evp_pkey_get_raw_private_key)(const EVP_PKEY *pkey, unsigned char *priv, size_t *len);
typedef int (* evp_pkey_get_raw_public_key)(const EVP_PKEY *pkey, unsigned char *pub, size_t *len);
typedef EC_KEY *(* evp_pkey_get0_ec_key)(EVP_PKEY *pkey);
typedef const BIGNUM *(* ec_key_get0_private_key)(const EC_KEY *key);
typedef const EC_POINT *(* ec_key_get0_public_key)(const EC_KEY *key);
typedef const EC_GROUP *(* ec_key_get0_group)(const EC_KEY *key);
typedef BN_CTX *(* bn_ctx_new)(void);
typedef BIGNUM *(* bn_ctx_get)(BN_CTX *ctx);
typedef int (* ec_point_get_affine_coordinates)(const EC_GROUP *group, const EC_POINT *p, BIGNUM *x, BIGNUM *y, BN_CTX *ctx);
typedef int (* bn_bn2bin)(const BIGNUM *a, unsigned char *to);
typedef void (* bn_ctx_free)(BN_CTX *ctx);
typedef int (* bn_bn2mpi)(const BIGNUM *a, unsigned char *d);
typedef EVP_PKEY *(* evp_pkey_new_raw_public_key)(int type, ENGINE *e, const unsigned char *pub, size_t len);
typedef EVP_PKEY *(* evp_pkey_new_raw_private_key)(int type, ENGINE *e, const unsigned char *priv, size_t len);
typedef EC_KEY *(* ec_key_new_by_curve_name)(int nid);
typedef BIGNUM *(* bn_mpi2bn)(const unsigned char *s, int len, BIGNUM *ret);
typedef int (* ec_key_set_private_key)(EC_KEY *key, const BIGNUM *prv);
typedef EVP_PKEY *(* evp_pkey_new)(void);
typedef int (* evp_pkey_set1_ec_key)(EVP_PKEY *pkey, EC_KEY *key);
typedef void (* bn_free)(BIGNUM *a);
typedef EC_GROUP *(* ec_group_new_by_curve_name)(int nid);
typedef EC_POINT *(* ec_point_new)(const EC_GROUP *group);
typedef BIGNUM *(* bn_bin2bn)(const unsigned char *s, int len, BIGNUM *ret);
typedef int (* ec_point_set_affine_coordinates)(const EC_GROUP *group, EC_POINT *point, const BIGNUM *x, const BIGNUM *y, BN_CTX *ctx);
typedef int (* ec_key_set_public_key)(EC_KEY *key, const EC_POINT *pub);
typedef void (* ec_point_free)(EC_POINT *point);
typedef void (* ec_group_free)(EC_GROUP *group);
typedef void (* ec_key_free)(EC_KEY *key);
typedef EVP_PKEY_CTX *(* evp_pkey_ctx_new)(EVP_PKEY *pkey, ENGINE *e);
typedef int (* evp_pkey_derive_init)(EVP_PKEY_CTX *ctx);
typedef int (* evp_pkey_derive_set_peer)(EVP_PKEY_CTX *ctx, EVP_PKEY *peer);
typedef int (* evp_pkey_derive)(EVP_PKEY_CTX *ctx, unsigned char *key, size_t *pkeylen);
typedef void (* evp_pkey_ctx_free)(EVP_PKEY_CTX *ctx);
typedef void (* evp_pkey_free)(EVP_PKEY *x);

#if WSEC_COMPILE_ENABLE_ASYM
typedef int (* rsa_pkey_ctx_ctrl)(EVP_PKEY_CTX *ctx, int optype, int cmd, int p1, void *p2);
typedef RSA *(* evp_pkey_get0_rsa)(EVP_PKEY *pkey);
typedef int (* bn_num_bits)(const BIGNUM *a);
typedef const BIGNUM *(* rsa_get0_n)(const RSA *d);
typedef const BIGNUM *(* rsa_get0_e)(const RSA *d);
typedef const BIGNUM *(* rsa_get0_d)(const RSA *d);
typedef int (* evp_pkey_paramgen)(EVP_PKEY_CTX *ctx, EVP_PKEY **ppkey);
typedef BIGNUM *(*bn_new)(void);
typedef int (*bn_bn2binpad)(const BIGNUM *a, unsigned char *to, int tolen);
typedef EVP_MD_CTX *(* evp_md_ctx_new)(void);
typedef int (* evp_md_ctx_reset)(EVP_MD_CTX *ctx);
typedef int (* evp_digestsigninit)(EVP_MD_CTX *ctx, EVP_PKEY_CTX **pctx, const EVP_MD *type, ENGINE *e, EVP_PKEY *pkey);
typedef int (* evp_digestsign)(EVP_MD_CTX *ctx, unsigned char *sigret, size_t *siglen,
    const unsigned char *tbs, size_t tbslen);
typedef void (* evp_md_ctx_free)(EVP_MD_CTX *ctx);
typedef RSA *(* rsa_new)(void);
typedef int (* rsa_set0_key)(RSA *r, BIGNUM *n, BIGNUM *e, BIGNUM *d);
typedef void (* rsa_free)(RSA *r);
typedef int (* evp_pkey_assign)(EVP_PKEY *pkey, int type, void *key);
typedef int (* evp_digestinit_ex)(EVP_MD_CTX *ctx, const EVP_MD *type, ENGINE *impl);
typedef int (* evp_digestupdate)(EVP_MD_CTX *ctx, const void *data, size_t count);
typedef int (* evp_digestfinal_ex)(EVP_MD_CTX *ctx, unsigned char *md, unsigned int *size);
typedef int (* evp_pkey_sign_init)(EVP_PKEY_CTX *ctx);
typedef EVP_MD *(* evp_md_meth_dup)(const EVP_MD *md);
typedef void (* evp_md_meth_free)(EVP_MD *md);
typedef int (* evp_pkey_sign)(EVP_PKEY_CTX *ctx, unsigned char *sig, size_t *siglen,
    const unsigned char *tbs, size_t tbslen);
typedef int (* evp_pkey_set_alias_type)(EVP_PKEY *pkey, int type);
typedef int (* evp_digestverifyinit)(EVP_MD_CTX *ctx, EVP_PKEY_CTX **pctx,
    const EVP_MD *type, ENGINE *e, EVP_PKEY *pkey);
typedef int (* evp_digestverify)(EVP_MD_CTX *ctx, const unsigned char *sigret, size_t siglen,
    const unsigned char *tbs, size_t tbslen);
typedef int (* evp_pkey_verify_init)(EVP_PKEY_CTX *ctx);
typedef int (* evp_pkey_verify)(EVP_PKEY_CTX *ctx, const unsigned char *sig, size_t siglen,
    const unsigned char *tbs, size_t tbslen);
typedef int (* ec_key_set_public_key_affine_coordinates)(EC_KEY *key, BIGNUM *x, BIGNUM *y);
typedef int (* evp_pkey_encrypt_init)(EVP_PKEY_CTX *ctx);
typedef int (* evp_pkey_encrypt)(EVP_PKEY_CTX *ctx, unsigned char *out, size_t *outlen,
    const unsigned char *in, size_t inlen);
typedef int (* evp_pkey_decrypt_init)(EVP_PKEY_CTX *ctx);
typedef int (* evp_pkey_decrypt)(EVP_PKEY_CTX *ctx, unsigned char *out, size_t *outlen,
    const unsigned char *in, size_t inlen);
typedef int (* evp_verifyfinal)(EVP_MD_CTX *ctx, const unsigned char *sigbuf, unsigned int siglen, EVP_PKEY *pkey);
typedef int (* evp_signfinal)(EVP_MD_CTX *ctx, unsigned char *md, unsigned int *s, EVP_PKEY *pkey);
typedef ECDSA_SIG *(* ecdsa_sig_new)(void);
typedef int (* ecdsa_sig_set0)(ECDSA_SIG *sig, BIGNUM *r, BIGNUM *s);
typedef int (* i2d_ecdsa_sig)(const ECDSA_SIG *sig, unsigned char **pp);
typedef void (* ecdsa_sig_free)(ECDSA_SIG *sig);
typedef void (* crypto_free)(void *str, const char *file, int line);

typedef const EVP_MD *(* evp_sha256)(void);
typedef const EVP_MD *(* evp_sha384)(void);
typedef const EVP_MD *(* evp_sha512)(void);
typedef const EVP_MD *(* evp_sm3)(void);

typedef int (* asn1_get_object)(const unsigned char **pp, long *plength, int *ptag, int *pclass, long omax);
typedef ASN1_VALUE *(* asn1_item_d2i)(ASN1_VALUE **pval, const unsigned char **in, long len, const ASN1_ITEM *it);
typedef void (* asn1_item_free)(ASN1_VALUE *val, const ASN1_ITEM *it);
typedef int (* i2d_publickey)(EVP_PKEY *a, unsigned char **pp);
typedef EVP_PKEY *(* d2i_publickey)(int type, EVP_PKEY **a, const unsigned char **pp, long length);
#endif // WSEC_COMPILE_ENABLE_ASYM

#endif /* WSEC_COMPILE_CAC_OPENSSL */

#endif
