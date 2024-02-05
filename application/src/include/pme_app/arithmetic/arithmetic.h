/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : arithmetic.h
  版 本 号   : 初稿
  作    者   : chenzhan
  生成日期   : 2016年4月16日
  最近修改   :
  功能描述   : arithmetic库函数申明
  函数列表   :
  修改历史   :
  1.日    期   : 2016年4月16日
    作    者   : chenzhan
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/


#ifndef __PME_ARITHMETIC_H__
#define __PME_ARITHMETIC_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef struct aes_algo_param {
    guint8 algo_type; // 算法类型
    guint8 padding_style; // 填充方式
    guint8 *key; // 秘钥
    guint32 key_size; // 秘钥长度
    guint8 *iv; // 初始向量
    guint32 iv_size; // 初始向量长度
} AES_ALGO_PARAM_S;

#define AES_GCM_TAG_SIZE 16
#define SMALL_BUFFER_SIZE       256
#define USER_ENPASSWD_LENGTH    32

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

void arith_aes_128_data_encrypt_iv(guchar *key, gint32 key_len, const guchar *input, guchar *output, gint32 ilen,
    gint32 *olen, guchar *iv, guchar padding);

void arith_aes_128_data_decrypt_iv(guchar *key, gint32 key_len, const guchar *input, guchar *output, gint32 ilen,
    gint32 *olen, guchar *iv, guchar padding);

void convert_bytes_to_string(const guchar* input, gsize len, guchar* output);

void convert_string_to_bytes(const guchar *input, gsize input_len, guchar *output);

/* BEGIN : Modified for PN:DTS2015042309488  by z00229006, 2015/4/23*/
gint32 get_aes_128_workkey(const guint8* component,
                           gint32 component_size,
                           gchar* workkey,
                           gint32 workkey_size,
                           gint32* workkey_len,
                           guchar padding);
gint32 get_plain_aes_128_data(const guint8 *component, gint32 component_size, const gchar *encrypt_buffer,
    gchar *plain_buffer, guint32 plain_buffer_size, guchar padding);
gint32 get_encrypt_aes_128_data(const guint8* component,
                                gint32 component_size,
                                const gchar* workkey,
                                const gchar* plain_buffer,
                                gchar* encrypt_buffer,
                                gint32 encrypt_buffer_size,
                                gint32* encrypt_buffer_len,
                                guchar padding);
/* END : Modified for PN:DTS2015042309488  by z00229006, 2015/4/23*/

gint32 get_plain_workkey(const guint8* component,
                               gint32 component_size,
                               const gchar* encrypt_workkey,
                               gint32 encrypt_workkey_len,
                               guint8* plain_workkey,
                               gint32 plain_workkey_size,
                               guchar padding);

gint32 get_plain_aes_128_file(const guint8* component,
                              gint32 component_size,
                              gchar* workkey,
                              const gchar* encrypt_file,
                              const gchar* plain_file,
                              guchar padding);

gint32 get_encrypt_aes_128_file(const guint8* component,
                                gint32 component_size,
                                gchar* workkey,
                                const gchar* plain_file,
                                const gchar* encrypt_file,
                                guchar padding);

gint32 openssl_aes_gcm_128_encrypt(AES_ALGO_PARAM_S *param, const guint8 *input, gint32 input_len, guint8 *output,
    gint32 output_len);
gint32 openssl_aes_gcm_128_decrypt(AES_ALGO_PARAM_S *param, const guint8 *input, gint32 input_len, guint8 *output,
    gint32 output_len);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __PME_ARITHMETIC_H__ */
