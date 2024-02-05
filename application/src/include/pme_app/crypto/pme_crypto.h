#ifndef __PME__CRYPTO__H_
#define __PME__CRYPTO__H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


/**
 * dump some bytes.
 */
extern void hexdump(
             FILE *f,
             const char *title,
             const unsigned char *s,
             int l);
/**
 * generate an aes key
 */
extern int pme_gen_aes_key(char *key, size_t *key_len);
extern int pme_encrypt_data(char *out, size_t *out_len, const char *in, size_t in_len, const char *key);
extern int pme_decrypt_data(char *out, int *out_len, const char *in, int in_len, const char *key);
extern int pme_encrypt_file(const char *out_file, const char *in_file);
extern gsize pme_get_aes_key(char *out, const char *in, int in_len, const gchar *buf, gsize buf_len);
extern int pme_sign_file(const char *out, const char *in);
extern int pme_verify_file(const char *file);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif/* __PME__CRYPTO__H_ */

