#ifndef SSL_OFFLOAD_H
#define SSL_OFFLOAD_H
 
#ifndef OPENSSL_NO_TTO
 
#include <stdio.h>
#include <openssl/e_os2.h>
#include "openssl/asn1t.h"
 
#ifdef __cplusplus
extern "C" {
#endif
 
# define SSL_SESS_PARAM_ASN1_VERSION 0x0001
 
#define SSL_SESS_ENC_ALG_AES        1
 
#define SSL_SESS_HMAC_ALG_SHA       1
#define SSL_SESS_HMAC_ALG_SHA256    2
 
#define SSL_SESS_AEAD_ALG_AES_GCM   1
#define SSL_SESS_AEAD_ALG_AES_CCM   2
 
#define SSL_SESS_WITH_ETM           0x00000001UL
#define SSL_SESS_WITH_CCM_8_CIPHER  0x00000002UL
#define SSL_SESS_WITH_SERVER        0x00000004UL
 
typedef struct {
    ASN1_OCTET_STRING *sess_id;
    uint32_t format_version;
    uint32_t ssl_version;
    uint32_t enc_alg_nid;
    uint32_t md_alg_nid;
    uint32_t flags;
    int fd;
    ASN1_OCTET_STRING *r_key;
    ASN1_OCTET_STRING *r_iv;
    ASN1_OCTET_STRING *w_key;
    ASN1_OCTET_STRING *w_iv;
    ASN1_OCTET_STRING *r_mac_key;
    ASN1_OCTET_STRING *w_mac_key;
    ASN1_OCTET_STRING *r_seq_num;
    ASN1_OCTET_STRING *w_seq_num;
    ASN1_OCTET_STRING *r_partial_sock_data;
}SSL_SESS_PARAM_ASN1;
 
typedef struct enc_info_st {
    int alg_id; /* NID_aes_128_cbc, NID_aes_192_cbc, NID_aes_256_cbc, NID_des_ede3_cbc */
    uint8_t *key;
    uint16_t key_len;
}ENC_INFO;
 
int SSL_export_session(SSL *s, uint8_t **out_buf, ENC_INFO *enc_info);
 
#ifdef __cplusplus
}
#endif
 
#endif
 
#endif