#ifndef __UTILITY__H_
#define __UTILITY__H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


#define PME_CRYPTO_DEBUG(format, ...) \
    do { \
        printf(" %s:%d:%s: ", __FILE__, __LINE__, __func__); \
        printf(format "\n", ## __VA_ARGS__); \
    } while(0)

#define RSA_PKCS1_PADDING	1
#define RSA_SSLV23_PADDING	2
#define RSA_NO_PADDING		3
#define RSA_PKCS1_OAEP_PADDING	4
#define RSA_X931_PADDING	5

extern int pme_encrypt_file(const char *out_file, const char *in_file,int padding_mode);
extern int pme_sign_file(const char *out, const char *in);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif/* __UTILITY__H_ */

