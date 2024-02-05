/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @file pqc.h
 * Description: PQC后量子模块接口头文件
 * Author: MOSI VPP
 * Create: 2022-11-14
 */
#ifndef OSSL_CRYPTO_PQC_H
#define OSSL_CRYPTO_PQC_H

# include <openssl/opensslconf.h>
# include <openssl/e_os2.h>

#ifndef OPENSSL_NO_PQC
#ifdef __cplusplus
extern "C" {
#endif
typedef struct pqc_kem_meth_st PQC_KEM_METH;

typedef struct pqc_key_st PQC_KEY;

PQC_KEY *PQC_KEY_new(int alg_id);
void PQC_KEY_free(PQC_KEY *key);

PQC_KEM_METH *PQC_KEY_get0_kem_meth(PQC_KEY *key);
int PQC_KEY_get_nid(PQC_KEY *key);
int PQC_KEY_set1_pk(PQC_KEY *key, const unsigned char *pk, int pk_len);
const unsigned char *PQC_KEY_get0_pk(PQC_KEY *key);
int PQC_KEY_set1_sk(PQC_KEY *key, const unsigned char *sk, int sk_len);
const unsigned char *PQC_KEY_get0_sk(PQC_KEY *key);
int PQC_KEY_set1_ct(PQC_KEY *key, const unsigned char *ct, int ct_len);
const unsigned char *PQC_KEY_get0_ct(PQC_KEY *key);
int PQC_KEY_set1_ss(PQC_KEY *key, const unsigned char *ss, int ss_len);
const unsigned char *PQC_KEY_get0_ss(PQC_KEY *key);
int PQC_KEY_copy(PQC_KEY *orikey, PQC_KEY *destkey);

/* Get a new context. Calls the init routine of the selected algorithm */
PQC_KEM_METH* PQC_KEM_new(const int alg_id);

/* Destroy an old context. Calls the cleanup routine of the selected algorithm*/
void PQC_KEM_free(PQC_KEM_METH* kem);

int PQC_KEM_get_pk_len(PQC_KEM_METH *kem);
int PQC_KEM_get_sk_len(PQC_KEM_METH *kem);
int PQC_KEM_get_ct_len(PQC_KEM_METH *kem);
int PQC_KEM_get_ss_len(PQC_KEM_METH *kem);

int PQC_KEM_keygen(const PQC_KEM_METH *kem, unsigned char *public_key, unsigned char *secret_key);

int PQC_KEM_encaps(const PQC_KEM_METH *kem, unsigned char *ciphertext, unsigned char *shared_secret, const unsigned char *public_key);

int PQC_KEM_decaps(const PQC_KEM_METH *kem, unsigned char *shared_secret, const unsigned char *ciphertext, const unsigned char *secret_key);

# ifdef  __cplusplus
}
# endif
#endif /* OPENSSL_NO_PQC */
#endif
