#ifndef __DAL_ENCRYPT_H__
#define __DAL_ENCRYPT_H__
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */
#include "pme/pme.h"
#include "pme_app/pme_app.h"

typedef enum _tag_aes_pdding_e
{
    NO_PADDING = 0,
    CMS_PADDING = 1,
} AES_PADDING_E;

gint32 dal_update_encrypt_data(OBJ_HANDLE obj_handle,
                                                               const gchar* priv_data_prop,
                                                               const guint8* old_component,
                                                               const gsize old_component_size,
                                                               const guint8* new_component,
                                                               const gsize new_component_size,
                                                               const gchar* new_workkey);

void dal_wait_encrypted_property_rollback(guchar* property_update_state, guchar* rollback_state);

gint32 dal_get_encryptkey_workkey(const gchar* obj_name,
                                            const gchar* property_name,
                                            gchar* workkey, gint32 work_size);

gint32 dal_get_encryptkey_component(const gchar* obj_name,
                                                         const gchar* property_name,
                                                         guint8* component,
                                                         gint32 component_size);

gint32 dal_set_encryptkey_component(const gchar* obj_name,
                                                        const gchar* property_name,
                                                        const guint8* component,
                                                        gint32 component_size);

gint32 dal_set_encryptkey_workkey(const gchar* obj_name,
                                                    const gchar* property_name,
                                                    const gchar* workkey);
                         
/* BEGIN : Modified for PN:DTS2015042309488  by z00229006, 2015/4/23*/
gint32 decrypt_aes_128_passwd(const guchar *ctPassword, guchar *ptPassword, guchar padding);

gint32 get_plain_aes_128_data(const guint8* component,
                              gint32 component_size,
                              const gchar* encrypt_buffer,
                              gchar* plain_buffer,
                              guint32 plain_buffer_size,
                              guchar padding);
gint32 get_aes_128_workkey(const guint8* component,
                           gint32 component_size,
                           gchar* workkey,
                           gint32 workkey_size,
                           gint32* workkey_len,
                           guchar padding);
gint32 get_encrypt_aes_128_data(const guint8* component,
                                gint32 component_size,
                                const gchar* workkey,
                                const gchar* plain_buffer,
                                gchar* encrypt_buffer,
                                gint32 encrypt_buffer_size,
                                gint32* encrypt_buffer_len,
                                guchar padding);
/* END : Modified for PN:DTS2015042309488  by z00229006, 2015/4/23*/

gint32 get_plain_aes_128_file(const guint8* component, gint32 component_size, gchar* workkey, const gchar* encrypt_file, 
    const gchar* plain_file, guchar padding);

gint32 get_encrypt_aes_128_file(const guint8* component, gint32 component_size, gchar* workkey, const gchar* plain_file,
    const gchar* encrypt_file, guchar padding);
   void arith_aes_cbc_128_decrypt(guchar* key,
                               guchar  iv[16],
                               const guchar* input,
                               guchar* output,
                               gint32  len,
                               guchar  padding);
void arith_aes_cbc_128_encrypt(guchar* key,
                               guchar  iv[16],
                               const guchar* input,
                               guchar* output,
                               gint32  len,
                               guchar  padding);

void arith_aes_128_decrypt(guchar* key,
                           const guchar* input,
                           guchar* output,
                           guchar  padding);
gint32 openssl_aes_cbc_128_encrypt(guchar* key,
                                   guchar  iv[16],
                                   const guchar* input,
                                   guchar* output,
                                   gint32  len,
                                   guchar  padding);
gint32 openssl_aes_cbc_128_decrypt(guchar* key,
                                   guchar  iv[16],
                                   const guchar* input,
                                   guchar* output,
                                   gint32  len,
                                   guchar padding);
void arith_aes_128_encrypt(guchar* key,
                           const guchar* input,
                           guchar* output,
                           guchar padding);
void arith_aes_cbc_128_data_decrypt(guchar* key, guchar iv[16],
                                    const guchar* input,
                                    guchar* output,
                                    gint32 ilen, gint32* olen,
                                    guchar padding);
void arith_aes_cbc_128_data_encrypt(guchar* key, guchar iv[16],
                                    const guchar* input,
                                    guchar* output,
                                    gint32 ilen, gint32* olen,
                                    guchar padding);
void arith_aes_128_data_decrypt(guchar* key, const guchar* input,
                                guchar* output,
                                gint32 ilen, gint32* olen,
                                guchar padding);

void arith_aes_128_data_encrypt(guchar* key, const guchar* input,
                                guchar* output,
                                gint32 ilen, gint32* olen,
                                guchar padding);

void arith_aes_128_data_encrypt_iv(guchar* key, gint32 key_len, const guchar* input,
                                   guchar* output,
                                   gint32 ilen, gint32* olen, guchar* iv,
                                   guchar padding);

void arith_aes_128_data_decrypt_iv(guchar* key, gint32 key_len, const guchar* input,
                                   guchar* output,
                                   gint32 ilen, gint32* olen, guchar* iv,
                                   guchar padding);


                                   gint32 _get_encyptyed_workkey(const gchar *encrypt_content, gchar *workkey, gint32 work_size);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* __DAL_ENCRYPT_H__ */