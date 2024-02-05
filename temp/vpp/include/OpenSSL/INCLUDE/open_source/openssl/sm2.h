#ifndef OPENSSL_SM2_H
#define OPENSSL_SM2_H

# include <openssl/opensslconf.h>

# ifndef OPENSSL_NO_SM2_ENVELOP
# include <openssl/err.h>
# include <openssl/evp.h>
# include <openssl/x509.h>

# ifdef __cplusplus
extern "C" {
# endif

typedef struct sm2_enveloped_key_st SM2_ENVELOPED_KEY;

DECLARE_ASN1_FUNCTIONS(SM2_ENVELOPED_KEY)

SM2_ENVELOPED_KEY *SM2_ENVELOPED_KEY_Seal(const EVP_CIPHER *cipher, EVP_PKEY *pkey, X509 *x509,
                                          unsigned char *key, int keylen);
int SM2_ENVELOPED_KEY_Open(SM2_ENVELOPED_KEY *sm2evpkey, EVP_PKEY *pkey, EVP_PKEY **pkout);
# ifdef  __cplusplus
}
# endif
# endif
#endif