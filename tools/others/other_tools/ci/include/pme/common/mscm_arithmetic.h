#ifndef _MSCM_ARITHMETIC_H_
#define _MSCM_ARITHMETIC_H_

#include <stdio.h>
#include <glib.h>

#ifndef __LOCAL_MD5__
 #include <openssl/hmac.h>
 #include <openssl/md5.h>
#endif

#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif /* __cplusplus */

/*************************************************
MACRO  defination
 **************************************************/
#define INVALID_CRC 0xFFFF
#define UUID_LENGTH 16
#define UUID_MAC_LENGTH  6


extern void arith_uuid_create(guint8 uuid[UUID_LENGTH], const guint8 mac[UUID_MAC_LENGTH]);

extern gushort    arith_make_crc_checksum(gushort StartCRC, const guchar *buf,
                                                      gulong len);
extern gulong    arith_make_crc32(gulong crc, const gchar* buf, gulong len);
extern gulong    arith_atoh(const gchar * pStr);
extern gint32    arith_make_file_crc(const gchar* filename, gushort *crc);

extern gchar *arith_bin_2_string( const guint8 *data, guint32 len, gchar *buf );
extern gint32 arith_string_2_bin( const gchar *string, guint8 *data, guint32 len );

// MD5算法接口
extern void    arith_MD5Buffer (const guchar *buffer, gshort len, guchar digest[MD5_DIGEST_LENGTH]);
extern void    arith_MD5String (const gchar *string, guchar digest[MD5_DIGEST_LENGTH]);

/*将字符串传换为整型输出*/
extern gint32    arith_str_to_int(const gchar* str);
extern gint32    arith_MD5File(const gchar * file, guchar digest [MD5_DIGEST_LENGTH], guint32 length);
extern gint32    arith_MD5FileBuffer( FILE *file, gulong offset, gulong len,
    guchar digest[MD5_DIGEST_LENGTH], guint32 length);

//SHA2算法接口


/************************************
 * \brief          Output = HMAC-SHA-1( hmac key, input buffer )
 * \param key      HMAC secret key
 * \param keylen   length of the HMAC key
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   HMAC-SHA-1 result
 *************************************/
void arith_hmac_sha1( guchar *key,  glong keylen, guchar *input,
                                               glong ilen,
                                              guchar *output);
void arith_hmac_sha2( guchar *key,  glong keylen, guchar *input,
                                               glong ilen,
                                              guchar *output);
void arith_hmac_sha1_96( guchar *key,  glong keylen, guchar *input,
                                                   glong ilen,
                                                  guchar *output);
void arith_hmac_sha2_128( guchar *key,  glong keylen,
                   guchar *input,  glong ilen,
                   guchar *output);

/************************************
 * \brief          Checkup routine
 * \return         0 if successful, or 1 if the test failed
 *************************************/
gchar arith_strmac_to_intmac(const gchar *str_mac, guchar *int_mac, gint32 int_len);
gchar arith_intmac_to_strmac(const guchar *int_mac, gchar *str_mac, gint32 str_len);

extern guint8 arith_make_crc8(guint8* buf, guint32 len);


#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* __cplusplus */

#endif /*_MSCM_ARITHMETIC_H_*/

