/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @file pqc_hybrid.h
 * Description: PQC后量子混合模块接口头文件
 * Author: MOSI VPP
 * Create: 2022-11-14
 */
#ifndef OSSL_CRYPTO_PQC_HYBRID_H
#define OSSL_CRYPTO_PQC_HYBRID_H

# include <openssl/opensslconf.h>
# include <openssl/e_os2.h>
# include <openssl/pqc.h>
# include <openssl/evp.h>

#ifndef OPENSSL_NO_PQCHYBRID
#ifdef __cplusplus
extern "C" {
#endif
typedef struct pqc_hybrid_key PQC_HYBRID_KEY;

PQC_HYBRID_KEY *PQC_HYBRID_KEY_new(int pqc_nid, int classical_curve_id, PQC_KEY *pqc_key, EVP_PKEY *ckey, EVP_PKEY *skey);

void PQC_HYBRID_KEY_free(PQC_HYBRID_KEY *key);

int PQC_HYBRID_KEY_get_pqc_nid(PQC_HYBRID_KEY *hkey);

int PQC_HYBRID_KEY_get_curve_id(PQC_HYBRID_KEY *hkey);

int PQC_HYBRID_KEY_set0_pqc_key(PQC_HYBRID_KEY *hkey, PQC_KEY *key);

int PQC_HYBRID_KEY_set0_classical_ckey(PQC_HYBRID_KEY *hkey, EVP_PKEY *ckey);

int PQC_HYBRID_KEY_set0_classical_skey(PQC_HYBRID_KEY *hkey, EVP_PKEY *skey);

PQC_KEY *PQC_HYBRID_KEY_get0_pqc_key(PQC_HYBRID_KEY *hkey);

EVP_PKEY *PQC_HYBRID_KEY_get0_classical_ckey(PQC_HYBRID_KEY *hkey);

EVP_PKEY *PQC_HYBRID_KEY_get0_classical_skey(PQC_HYBRID_KEY *hkey);

int PQC_HYBRID_keygen(PQC_HYBRID_KEY *hkey);
int PQC_HYBRID_encaps(PQC_HYBRID_KEY *hkey);
int PQC_HYBRID_decaps(PQC_HYBRID_KEY *hkey);

int PQC_HYBRID_pk_concat(PQC_HYBRID_KEY *hkey, unsigned char **hybrid_point);
int PQC_HYBRID_pk_extract(const unsigned char *hybrid_str, int hybrid_len, PQC_HYBRID_KEY *hkey);
int PQC_HYBRID_ct_concat(PQC_HYBRID_KEY *hkey, unsigned char **hybrid_point);
int PQC_HYBRID_ct_extract(const unsigned char *hybrid_str, int hybrid_len, PQC_HYBRID_KEY *hkey);

# ifdef  __cplusplus
}
# endif
#endif /* OPENSSL_NO_PQCHYBRID */
#endif
