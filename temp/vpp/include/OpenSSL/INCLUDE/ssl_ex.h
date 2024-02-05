/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Filename      : ssl_ex.h
 * Author        : h00273581
 * Description   : This is the interface of ssl_ex module.
 * Create        : 2018/11/13
 * Version       : 1.0
*/
#ifndef SSL_EX_H
#define SSL_EX_H

#include <openssl/ssl.h>
#include <openssl/x509v3.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SSL_FILETYPE_PFX_COM 4
#if defined(OPENSSL_SYS_VXWORKS55)
#define CRYPTO_ONCE_STATIC_INIT_EX {0, 0}
#endif

typedef enum _SSL_ex_data_type {
    SSL_EX_DATA_SOCKET_ID = 0,
    SSL_EX_DATA_RESERVED1,
    SSL_EX_DATA_RESERVED2,
    SSL_EX_DATA_RESERVED3,
    SSL_EX_DATA_TYPE_MAX /* max type, this is not a valid type for ex data, only for limit */
} SSL_ex_data_type;

typedef int (*IPSISSLSOCKREAD_CB)(int, unsigned char *, int, void *, int *);
typedef int (*IPSISSLSOCKWRITE_CB)(int, const unsigned char *, int, void *, int *);

typedef struct x509_crl_key X509_CRL_KEY;

/**
 * Func Name:  SSL_CTX_set_cipher_list_ex
 * @defgroup SSL_CTX_set_cipher_list_ex
 * @ingroup sslObjectFunctions
 * @par Prototype
 * @code
 * int SSL_CTX_set_cipher_list_ex(SSL_CTX *ctx, const char *ciphers[], const int num)
 * @endcode
 *
 * @par Purpose
 * This is used to set the specified file descriptor as the input/output facility for the SSL.
 *
 * @par Description
 * SSL_CTX_set_cipher_list_ex ffunction sets the list of available ciphers for SSL context using the cipher set
 * ciphers[]. The list of ciphers is inherited by all SSL objects created from SSL context.
 *
 * @param[in] ctx Pointer to SSL Context. [N/A]
 * @param[in] ciphers Array of ciphers. [N/A]
 * @param[in] num Number of ciphers in an array. [N/A]
 *
 * @retval 1 On success
 * @retval 0 On failure
 *
 * @par Required Header File
 * ssl_ex.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * SSL_CTX_set_cipher_list.
 */
int SSL_CTX_set_cipher_list_ex(SSL_CTX *ctx, const char *ciphers[], const int num);

/**
 * @defgroup SSL_set_cipher_list_ex
 * @ingroup sslObjectFunctions
 * @par Prototype
 * @code
 * int SSL_set_cipher_list_ex(SSL *ssl, const char *ciphers[], const int num)
 * @endcode
 *
 * @par Purpose
 * This is used to set the list of available ciphers for SSL.
 *
 * @par Description
 * SSL_set_cipher_list_ex function sets the list of available ciphers for SSL_S object using the list of ciphers passed.
 *
 * @param[in] ssl Pointer to SSL object. [N/A]
 * @param[in] ciphers Array of ciphers. [N/A]
 * @param[in] num Number of ciphers in the array. [N/A]
 *
 * @retval 1 On success
 * @retval 0 On failure
 *
 * @par Required Header File
 * ssl_ex.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * SSL_CTX_set_cipher_list.
 */
int SSL_set_cipher_list_ex(SSL *ssl, const char *ciphers[], const int num);

/**
 * @defgroup SSL_get_error_queue
 * @ingroup sslFunctions
 * @par Prototype
 * @code
 * int SSL_get_error_queue(void)
 * @endcode
 *
 * @par Purpose
 * This api is used to get the earliest error code from the thread's error queue
 * and log all the error codes from the error queue.
 *
 * @par Description
 * This api returns the earliest error code from the thread's error queue
 * and log all (include the earliest one) the error codes from the error queue
 * via the fix-len log callback function registered by SSL_BINLOG_reg_callback.
 * When invoking the log callback function, the input parameter log_level is
 * set to SSL_BIN_LOG_LEVEL_ERROR, and log_type is set to SSL_BIN_LOG_TYPE_RUN.
 * While logging, the error queue is cleared.
 *
 * @par Parameters
 * N/A
 *
 * @retval int The earliest error code from the thread's error queue.
 *
 * @par Required Header File
 * ssl_ex.h
 * @par Note
 * N/A
 */
int SSL_get_error_queue(void);

/**
 * @defgroup SSL_get_current_cipher_id
 * @ingroup sslFunctions
 * @par Prototype
 * @code
 * unsigned int SSL_get_current_cipher_id(const SSL *ssl)
 * @endcode
 *
 * @par Purpose
 * This api is used to get the current cipher id used by the SSL object.
 *
 * @par Description
 * This api returns the current cipher id used by the SSL object.
 *
 * @param[in] ssl Pointer to SSL object.[N/A]
 *
 * @return unsigned int. On success, the current cipher id used by the SSL object is returned.
 * @return unsigned int. On failure, 0 is returned.
 *
 * @par Required Header File
 * ssl_ex.h
 * @par Note
 * N/A
 */
unsigned int SSL_get_current_cipher_id(const SSL *ssl);

/**
 * @defgroup SSL_set_psk_param
 * @ingroup sslFunctions
 * @par Prototype
 * @code
 * int SSL_set_psk_param(SSL *ssl, const char *pskid, int pskid_len, const unsigned char *psk, int psk_len)
 * @endcode
 *
 * @par Purpose
 * This is used to set the psk parameter to the SSL. the psk parameter is applied
 * on TLS connection client side.
 *
 * @par Description
 * This api sets the psk parameter to the SSL. the psk parameter is applied
 * on TLS connection client side.
 *
 * @param[in] ssl pointer to SSL object [NA/NA]
 * @param[in] pskid pointer to psk identity [NA/NA]
 * @param[in] pskid_len length of psk identity [NA/NA]
 * @param[in] psk pointer to psk [NA/NA]
 * @param[in] psk_len length of psk [NA/NA]
 *
 * @retval int success [1|N/A].
 * @retval int failure [0|N/A].
 *
 * @par Required Header File
 * ssl_ex.h
 * @par Note
 * \n
 * Since this api calls SSL_set_psk_client_callback of openssl on implement, only the
 * last call takes effect if both this api and SSL_set_psk_client_callback are invoked.
 */
int SSL_set_psk_param(SSL *ssl, const char *pskid, int pskid_len, const unsigned char *psk, int psk_len);

/**
 * @defgroup X509_CRL_decode_file
 * @ingroup sslFunctions
 * @par Prototype
 * @code
 * X509_CRL *X509_CRL_decode_file(const char *file, int type)
 * @endcode
 *
 * @par Purpose
 * This api is used to decode X509_CRL object from crl file.
 *
 * @par Description
 * This api decodes the first X509_CRL object from the specific crl file.
 *
 * @param[in] file Pointer to crl file path.[N/A]
 * @param[in] type Crl file format, should be SSL_FILETYPE_PEM or SSL_FILETYPE_ASN1.[N/A]
 *
 * @retval X509_CRL* On success.[Pointer of X509_CRL object|N/A].
 * @retval X509_CRL* On failure.[NULL|N/A].
 *
 * @par Required Header File
 * ssl_ex.h
 * @par Note
 * When success, the returned X509_CRL object should be freed by X509_CRL_free if no longer needed.
 * @par Related Topics
 * X509_CRL_encode\n
 * X509_CRL_get_key\n
 */
X509_CRL *X509_CRL_decode_file(const char *file, int type);

/**
 * @defgroup X509_CRL_encode
 * @ingroup sslFunctions
 * @par Prototype
 * @code
 * char *X509_CRL_encode(X509_CRL *crl, int type, unsigned int *encode_len)
 * @endcode
 *
 * @par Purpose
 * This api is used to encode X509_CRL object to buffer.
 *
 * @par Description
 * This api encodes X509_CRL object to buffer, which's format is specified by parameter type.
 *
 * @param[in] crl Pointer to X509_CRL object.[N/A]
 * @param[in] type Crl buffer format, should be SSL_FILETYPE_PEM or SSL_FILETYPE_ASN1.[N/A]
 * @param[out] encode_len Encoded buffer len.[N/A]
 *
 * @retval char* On success.[Pointer of buffer encoded.|N/A].
 * @retval char* On failure.[NULL|N/A].
 *
 * @par Required Header File
 * ssl_ex.h
 * @par Note
 * When success, the returned crl buffer should be freed by OPENSSL_free if no longer needed.
 * @par Related Topics
 * X509_CRL_decode_file\n
 */
char *X509_CRL_encode(X509_CRL *crl, int type, unsigned int *encode_len);

/**
 * @defgroup X509_CRL_get_key
 * @ingroup sslFunctions
 * @par Prototype
 * @code
 * X509_CRL_KEY *X509_CRL_get_key(const X509_CRL *crl)
 * @endcode
 *
 * @par Purpose
 * This api is used to get the key property of X509_CRL object. The key may be used to
 * remove X509_CRL object from SSL_CTX.
 *
 * @par Description
 * This api obtains the key property of X509_CRL object. The key contains key
 * information like issuer and crl number.
 *
 * @param[in] crl Pointer to X509_CRL object.[N/A]
 *
 * @retval X509_CRL_KEY* On success.[Pointer of X509_CRL_KEY|N/A].
 * @retval X509_CRL_KEY* On failure.[NULL|N/A].
 *
 * @par Required Header File
 * ssl_ex.h
 * @par Note
 * When success, the returned X509_CRL_KEY should be freed by X509_CRL_KEY_free if no longer needed.
 * @par Related Topics
 * X509_CRL_decode_file\n
 * X509_CRL_KEY_free\n
 */
X509_CRL_KEY *X509_CRL_get_key(const X509_CRL *crl);

/**
 * @defgroup X509_CRL_KEY_free
 * @ingroup sslFunctions
 * @par Prototype
 * @code
 * void X509_CRL_KEY_free(X509_CRL_KEY *key)
 * @endcode
 *
 * @par Purpose
 * This api is used to free the X509_CRL_KEY.
 *
 * @par Description
 * This api frees the X509_CRL_KEY which may be retrieved by X509_CRL_get_key.
 *
 * @param[in] key Pointer to X509_CRL_KEY.[N/A]
 *
 * @retval void This api does not return any value.[N/A|N/A].
 *
 * @par Required Header File
 * ssl_ex.h
 * @par Note
 * N/A
 * @par Related Topics
 * X509_CRL_get_key\n
 */
void X509_CRL_KEY_free(X509_CRL_KEY *key);

/**
 * Func Name:  SSL_CTX_get_cert_num
 * @defgroup SSL_CTX_get_cert_num
 * @ingroup sslObjectFunctions
 * @par Prototype
 * @code
 * int SSL_CTX_get_cert_num(const SSL_CTX *ctx, X509_LOOKUP_TYPE type)
 * @endcode
 *
 * @par Purpose
 * This is used to get the number of certificate stored in SSL context.
 *
 * @par Description
 * SSL_CTX_get_cert_num function get the number of certificate stored in SSL context.
 *
 * @param[in] ctx Pointer to SSL context. [N/A]
 * @param[in] type Have two value X509_LU_X509 | X509_LU_CRL.
 *
 * @retval >=0 The number of certificate
 * @retval -1 On failure
 *
 * @par Required Header File
 * ssl_ex.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * SSL_getSockId.
 */
int SSL_CTX_get_cert_num(const SSL_CTX *ctx, X509_LOOKUP_TYPE type);

/**
 * @defgroup SSL_CTX_remove_crl
 * @ingroup sslFunctions
 * @par Prototype
 * @code
 * int SSL_CTX_remove_crl(SSL_CTX *ctx, const X509_CRL_KEY *key)
 * @endcode
 *
 * @par Purpose
 * This api is used to remove crl specified by X509_CRL_KEY from SSL context.
 *
 * @par Description
 * This api removes crl specified by X509_CRL_KEY from SSL context. The crl number in
 * X509_CRL_KEY is optional.
 *
 * @param[in] ctx Pointer to SSL context.[N/A]
 * @param[in] key Pointer to X509_CRL_KEY, which may be retrieved by X509_CRL_get_key.[N/A]
 *
 * @retval int On success.[1|N/A].
 * @retval int On failure.[0|N/A].
 *
 * @par Required Header File
 * ssl_ex.h
 * @par Note
 * N/A
 * @par Related Topics
 * X509_CRL_get_key\n
 * SSL_CTX_remove_crl_file\n
 */
int SSL_CTX_remove_crl(SSL_CTX *ctx, const X509_CRL_KEY *key);

/**
 * @defgroup SSL_CTX_remove_crl_by_hash
 * @ingroup sslFunctions
 * @par Prototype
 * @code
 * int SSL_CTX_remove_crl_by_hash(SSL_CTX *ctx, const unsigned char *md)
 * @endcode
 *
 * @par Purpose
 * This api is used to remove crl specified by crl hash from SSL context.
 *
 * @par Description
 * This api removes crl specified by crl hash from SSL context.
 * @param[in] ctx Pointer to SSL context.[N/A]
 * @param[in] md Pointer to CRL HASH, which may be retrieved by X509_CRL_get_hash.[N/A]
 * @param[in] length length of md, must be the SHA_DIGEST_LENGTH(20)
 *
 * @retval int On success.[1|N/A].
 * @retval int On failure.[0|N/A].
 *
 * @par Required Header File
 * ssl_ex.h
 * @par Note
 * N/A
 * @par Related Topics
 * X509_CRL_get_hash\n
 */
int SSL_CTX_remove_crl_by_hash(SSL_CTX *ctx, const unsigned char *md, const unsigned int length);

/**
 * @defgroup SSL_CTX_remove_crl_file
 * @ingroup sslFunctions
 * @par Prototype
 * @code
 * int SSL_CTX_remove_crl_file(SSL_CTX *ctx, const char *crl_file, int type)
 * @endcode
 *
 * @par Purpose
 * This api is used to remove crl specified by crl file path from SSL context.
 *
 * @par Description
 * This api removes crl specified by crl file path from SSL context. On implement,
 * this api decodes all the X509_CRL contained in the crl file, and retrieves
 * X509_CRL_KEY from per the X509_CRL, then use the X509_CRL_KEY to match X509_CRL
 * in the SSL context. All the X509_CRL objects matched with any X509_CRL_KEY will
 * be removed.
 *
 * @param[in] ctx Pointer to SSL context.[N/A]
 * @param[in] file Pointer to crl file path.[N/A]
 * @param[in] type Crl file format, should be SSL_FILETYPE_PEM or SSL_FILETYPE_ASN1.[N/A]
 *
 * @retval int On success.[1|N/A].
 * @retval int On failure.[0|N/A].
 *
 * @par Required Header File
 * ssl_ex.h
 * @par Note
 * N/A
 * @par Related Topics
 * SSL_CTX_remove_crl\n
 */
int SSL_CTX_remove_crl_file(SSL_CTX *ctx, const char *file, int type);

/**
 * @defgroup SSL_CTX_remove_verify_file
 * Function name: SSL_CTX_remove_verify_file
 * @par Description
 *              : Removed trusted CA from the store use file name.
 * Parameters   : [in] ctx      : Pointer to SSL Context.
 *              : [in] file     : the CA file name.
 * Return Value : successful (1)
                : error occurred (others)
 * Note         : None.
*/
int SSL_CTX_remove_verify_file(SSL_CTX *ctx, const char *file);

/**
 * @defgroup SSL_CTX_remove_verifycrlbyhash
 * @ingroup sslFunctions
 * @par Prototype
 * @code
 * int SSL_CTX_remove_verifycrlbyhash(const SSL_CTX *ctx, const unsigned char *md, const unsigned int length)
 * @endcode
 *
 * @par Purpose
 * This api is used to remove verify crl specified by crl hash from SSL context.
 *
 * @par Description
 * This api removes verify crl specified by crl hash from SSL context.
 * @param[in] ctx Pointer to SSL context.[N/A]
 * @param[in] md Pointer to CRL HASH, which may be retrieved by X509_CRL_get_hash.[N/A]
 * @param[in] length length of md, must be the SHA_DIGEST_LENGTH(20)
 *
 * @retval int On success.[1|N/A].
 * @retval int On failure.[0|N/A].
 *
 * @par Required Header File
 * ssl_ex.h
 * @par Note
 * N/A
 * @par Related Topics
 * X509_CRL_get_hash\n
 */
int SSL_CTX_remove_verifycrlbyhash(const SSL_CTX *ctx, const unsigned char *md, const unsigned int length);

/**
 * @defgroup SSL_CTX_remove_verify_buffer
 * Function name: SSL_CTX_remove_verify_buffer
 * @par Description
 *              : Removed pem cacertificate from the store use buffer.
 * Parameters   : [in] ctx      : Pointer to SSL Context.
 *              : [in] buf      : the CA buffer.
 *              : [in] len      : the buffer length,it should be valid and it should match with the buffer.
 * Return Value : successful (1)
                : error occurred (others)
 * Note         : None.
*/
int SSL_CTX_remove_verify_buffer(SSL_CTX *ctx, const char *buf, int len);

/**
 * Func Name:  SSL_CTX_remove_verifycrl
 * @defgroup SSL_CTX_remove_verifycrl
 * @ingroup sslContextFunctions
 * @par Prototype
 * @code
 * int SSL_CTX_remove_verifycrl(const SSL_CTX *ctx, const X509_CRL_KEY *key)
 * @endcode
 *
 * @par Purpose
 * This api is used to remove crl specified by X509_CRL_KEY from SSL_CTX context.
 *
 * @par Description
 * This api removes crl specified by X509_CRL_KEY from SSL_CTX context. On implement,
 * this api gets all the X509_CRL contained in the SSL_CTX context, and use the X509_CRL_KEY
 * to match them. All the X509_CRL objects matched with any X509_CRL_KEY will be removed from
 * the SSL_CTX context.
 *
 * @param[in] ctx Pointer to SSL_CTX context.[N/A]
 * @param[in] key Pointer to the crl key matched the crl you want to remove.[N/A]
 *
 * @retval int On success.[1|N/A].
 * @retval int On failure.[0|N/A].
 *
 * @par Required Header File
 * ssl_ex.h
 * @par Note
 * N/A
 * @par Related Topics
 * SSL_CTX_remove_crl\n
 */
int SSL_CTX_remove_verifycrl(const SSL_CTX *ctx, const X509_CRL_KEY *key);

/**
 * @defgroup SSL_CTX_remove_chain_store_buffer
 * Function name: SSL_CTX_remove_chain_store_buffer
 * @par Description
 *              : Removed pem cacertificate from the cert chain store use buffer.
 * Parameters   : [in] ctx      : Pointer to SSL Context.
 *              : [in] buf      : the CA buffer.
 *              : [in] len      : the buffer length,it should be valid and it should match with the buffer.
 * Return Value : successful (1)
                : error occurred (others)
 * Note         : None.
*/
int SSL_CTX_remove_chain_store_buffer(SSL_CTX *ctx, const char *buf, int len);

/**
 * @defgroup SSL_CTX_remove_verify_store_buffer
 * @ingroup sslContextFunctions
 * @par Prototype
 * @code
 * int SSL_CTX_remove_verify_store_buffer(SSL_CTX *ctx, const char *buf, int len)
 * @endcode
 *
 * @par Purpose
 * This api is used to remove CA cacertificate from the verify store use buffer.
 *
 * @par Description
 * This api removes CA cacertificate from the verify store use buffer. On implement,
 * this api read all the cacertificate contained in the buffer, and remove them in the
 * verify store in SSL_CTX context.
 *
 * @param[in] ctx Pointer to SSL context.[N/A]
 * @param[in] buf Pointer to CA buffer.[N/A]
 * @param[in] len the buffer length, it should be valid and it should match with the buffer.[N/A]
 *
 * @retval int On success.[1|N/A].
 * @retval int On failure.[0|N/A].
 *
 * @par Required Header File
 * ssl_ex.h
 * @par Note
 * N/A
 * @par Related Topics
 * SSL_CTX_remove_chain_store_buffer\n
 */
int SSL_CTX_remove_verify_store_buffer(SSL_CTX *ctx, const char *buf, int len);

/**
 * Func Name:  SSL_CTX_load_pfx_certificate_file
 * @defgroup SSL_CTX_load_pfx_certificate_file
 * @ingroup sslContextFunctions
 * @par Prototype
 * @code
 * int SSL_CTX_load_pfx_certificate_file(SSL_CTX *ctx,
 * const char *filename,
 * const char *macpass,
 * const char *encpass)
 *
 * @endcode
 *
 * @par Purpose
 * This is used to load the certificate and private key in PFX format found in a buffer to the SSL context.
 *
 * @par Description
 * SSL_CTX_load_pfx_certificate_file function reads a PFX store buffer that contains a certificate and a private key. To
 * verify
 * user certificate during the authentication, we should set it to the SSL_CTX Context.
 *
 * @param[in] ctx Pointer to SSL_CTX of either client OR server. [N/A]
 * @param[in] filename FileName. [N/A]
 * @param[in] macpass The Mac pass_word of PKCS12 format. [N/A]
 * @param[in] encpass The pass_word for encrypt of PKCS12 format. [N/A]
 *
 * @retval 1 On success
 * @retval 0 On failure
 *
 * @par Required Header File
 * ssl_ex.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * SSL_CTX_load_pfx_certificate_buffer.
 */
int SSL_CTX_load_pfx_certificate_file(SSL_CTX *ctx, const char *filename, const char *macpass, const char *encpass);

/**
 * Func Name:  SSL_CTX_load_pfx_certificate_buffer
 * @defgroup SSL_CTX_load_pfx_certificate_buffer
 * @ingroup sslContextFunctions
 * @par Prototype
 * @code
 * int SSL_CTX_load_pfx_certificate_buffer(SSL_CTX *ctx,
 * const char *buf,
 * unsigned int len,
 * const char *macpass,
 * const char *encpass)
 * @endcode
 *
 * @par Purpose
 * This is used to load the certificate and private key in PFX format found in a buffer to the SSL context.
 *
 * @par Description
 * SSL_CTX_load_pfx_certificate_buffer function reads a PFX store buffer that contains a certificate and a private key.
 * To verify user certificate during the authentication, we should set it to the SSL_CTX Context.
 *
 * @param[in] ctx Pointer to SSL_CTX of either client OR server. [N/A]
 * @param[in] buffer Pointer to the buffer from which the PFX store has to be loaded. [N/A]
 * @param[in] bufferlen The length of the buffer,it should be valid and it should match with the buffer. [N/A]
 * @param[in] macpass The Mac pass_word of PKCS12 format. [N/A]
 * @param[in] encpass The pass_word for encrypt of PKCS12 format. [N/A]
 *
 * @retval 1 On success
 * @retval 0 On failure
 *
 * @par Required Header File
 * ssl_ex.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * SSL_CTX_load_pfx_certificate_file.
 */
int SSL_CTX_load_pfx_certificate_buffer(SSL_CTX *ctx, const char *buffer, unsigned int bufferlen, const char *macpass,
                                        const char *encpass);

/**
 * @defgroup SSL_CTX_use_certificate_chain_buffer
 * Function name: SSL_CTX_use_certificate_chain_buffer
 * @par Description
 *              : This is used to load the certificate chain found in a buffer to the SSL context.
 * Parameters   : [in] ctx      : Pointer to SSL Context.
 *              : [in] buffer      : Pointer to the buffer from which the certificates has to be loaded.
 *              : [in] bufferlen      : The length of the buffer,it should be valid and it should match with the buffer.
 * Return Value : successful (1)
                : error occurred (others)
 * Note         : None.
*/
int SSL_CTX_use_certificate_chain_buffer(SSL_CTX *ctx, char *buffer, int bufferlen);

/**
 * @defgroup SSL_CTX_load_cerificate_key_file
 * Function name: SSL_CTX_load_cerificate_key_file
 * @par Description
 *              : This is used to load the certificate from a file and private.
 * Parameters   : [in] ctx          : Pointer to SSL Context.
 *              : [in] cert_filename: The path of the certificate filename.
 *              : [in] cert_type    : The formatting type of certificate.
 *              : [in] key_filename : The path of the pvtkey filename.
 *              : [in] key_type     : The formatting type of pvtkey.
 * Return Value : successful (1)
                : error occurred (others)
 * Note         : None.
*/
int SSL_CTX_load_cerificate_key_file(SSL_CTX *ctx, const char *cert_filename, int cert_type, const char *key_filename,
                                     int key_type);

/**
 * Func Name:  SSL_set_fd_ex
 * @defgroup SSL_set_fd_ex
 * @ingroup sslObjectFunctions
 * @par Prototype
 * @code
 * int SSL_set_fd_ex(SSL *s, int fd)
 * @endcode
 *
 * @par Purpose
 * This is used to set the specified file descriptor as the input/output facility for the SSL.
 *
 * @par Description
 * SSL_set_fd_ex function sets the file descriptor fd as the input/output facility for the SSL(encrypted) side of SSL.
 * fd will typically be the socket file descriptor of a network connection.
 *
 * @param[in] s Pointer to SSL object. [N/A]
 * @param[in] fd In client side: The socket descriptor which is obtained from the socket() call.\n
 * In Server side: The new descriptor which is obtained from the accept call. [N/A]
 *
 * @retval 1 On success
 * @retval 0 On failure
 *
 * @par Required Header File
 * ssl_ex.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * SSL_free_ex.
 */
int SSL_set_fd_ex(SSL *s, int fd);

/**
 * Func Name:  SSL_free_ex
 * @defgroup SSL_free_ex
 * @ingroup sslObjectFunctions
 * @par Prototype
 * @code
 * void SSL_free_ex(SSL *s)
 * @endcode
 *
 * @par Purpose
 * This is used to free an SSL object.
 *
 * @par Description
 * SSL_free_ex function frees an SSL object.
 *
 * @param[in] s Pointer to SSL object [N/A]
 *
 * @retval void This does not return any value [N/A|N/A]
 *
 * @par Required Header File
 * ssl_ex.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * SSL_set_fd_ex.
 */
void SSL_free_ex(SSL *s);

/**
 * Func Name:   SSL_set_rsocket_data
 * @defgroup SSL_set_rsocket_data
 * @ingroup sslFunctions
 * @par Prototype
 * @code
 * int SSL_set_rsocket_data(SSL *s, void *app_data)
 * @endcode
 * @par Purpose
 * This is used to set Application data for Socket Read callback(SSL_set_ipsi_rsock_cb).
 *
 * @par Description
 * SSL_set_rsocket_data function sets the Application data for Socket Read callback(SSL_set_ipsi_rsock_cb).
 *
 * @param[in] s The SSL descriptor [N/A]
 * @param[in] app_data The application that needs to be set [N/A]
 *
 * @retval 1 On success
 * @retval 0 On failure
 *
 * @par Required Header File
 * ssl_ex.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * SSL_set_ipsi_rsock_cb.
 */
int SSL_set_rsocket_data(SSL *s, void *app_data);

/**
 * Func Name:   SSL_set_wsocket_data
 * @defgroup SSL_set_wsocket_data
 * @ingroup sslFunctions
 * @par Prototype
 * @code
 * int SSL_set_wsocket_data(SSL *s, void *app_data)
 * @endcode
 * @par Purpose
 * This is used to set Application data for Socket Write callback(SSL_set_ipsi_wsock_cb).
 *
 * @par Description
 * SSL_set_wsocket_data function sets the Application data for Socket Write callback(SSL_set_ipsi_wsock_cb).
 *
 * @param[in] s The SSL descriptor [N/A]
 * @param[in] app_data The application that needs to be set [N/A]
 *
 * @retval 1 On success
 * @retval 0 On failure
 *
 * @par Required Header File
 * ssl_ex.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * SSL_set_ipsi_wsock_cb.
 */
int SSL_set_wsocket_data(SSL *s, void *app_data);

/**
 * Func Name:   SSL_set_ipsi_rsock_cb
 * @defgroup SSL_set_ipsi_rsock_cb
 * @ingroup sslObjectFunctions
 * @par Prototype
 * @code
 * int SSL_set_ipsi_rsock_cb(SSL *s,
 * IPSISSLSOCKREAD_CB cb)
 * @endcode
 *
 * @par Purpose
 * This is used to set the callback which will be used to read from a socket.
 *
 * @par Description
 * SSL_set_ipsi_rsock_cb function sets the callback which will be used to read from a socket.
 *
 * @param[in] s The ssl descriptor [N/A]
 * @param[in] cb A pointer to callback function [N/A]
 *
 * @retval 1 On success
 * @retval 0 On failure
 *
 * @par Required Header File
 * ssl_ex.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * SSL_set_rsocket_data.
 */
int SSL_set_ipsi_rsock_cb(SSL *s, IPSISSLSOCKREAD_CB cb);

/**
 * Func Name:   SSL_set_ipsi_wsock_cb
 * @defgroup SSL_set_ipsi_wsock_cb
 * @ingroup sslObjectFunctions
 * @par Prototype
 * @code
 * int SSL_set_ipsi_wsock_cb(SSL *s,
 * IPSISSLSOCKWRITE_CB cb)
 * @endcode
 *
 * @par Purpose
 * This is used to set the callback which will be used to write into a socket.
 *
 * @par Description
 * SSL_set_ipsi_wsock_cb function sets the callback which will be used to write into a socket.
 *
 * @param[in] s The ssl descriptor [N/A]
 * @param[in] cb A pointer to callback function [N/A]
 *
 * @retval 1 On success
 * @retval 0 On failure
 *
 * @par Required Header File
 * ssl_ex.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * SSL_set_wsocket_data.
 */
int SSL_set_ipsi_wsock_cb(SSL *s, IPSISSLSOCKWRITE_CB cb);

/**
 * Func Name: SSL_set_verify_app_data
 * @defgroup SSL_set_verify_app_data
 * @ingroup sslObjectFunctions
 * @par Prototype
 * @code
 * int SSL_set_verify_app_data(SSL *s, void *arg)
 * @endcode
 *
 * @par Purpose
 * This is used to set the verify callback application data in SSL object.\n
 *
 * @par Description
 * SSL_set_verify_app_data function sets the verify callback application
 * data in SSL object. SSL library will not do any validation or memory
 * allocation/deallocation for the application data. \n
 *
 * @param[in] s Pointer to SSL object [N/A|N/A]
 * @param[in] arg Pointer to verify callback application data [N/A|N/A]
 *
 * @retval 1 On success
 * @retval 0 On failure
 *
 * @par Required Header File
 * ssl_ex.h
 *
 * @par Note
 * \n
 * NA
 *
 * @par Related Topics
 * SSL_get_verify_app_data.
 */
int SSL_set_verify_app_data(SSL *s, void *arg);

/**
 * Func Name: SSL_get_verify_app_data
 * @defgroup SSL_get_verify_app_data
 * @ingroup sslObjectFunctions
 * @par Prototype
 * @code
 * void *SSL_get_verify_app_data(const SSL *s)
 * @endcode
 *
 * @par Purpose
 * This is used to get the verify callback application data from SSL object.\n
 *
 * @par Description
 * SSL_get_verify_app_data function returns the verify callback application
 * data from SSL object. SSL library will not do any validation or memory
 * allocation/deallocation for the application data. \n
 *
 * @param[in] s Pointer to SSL object [N/A|N/A]
 *
 * @retval void* Pointer to application data will be returned [N/A]
 * or\n
 * NULL will be returned in following conditions: \n
 * 1. If the input is invalid (NULL pointer is passed as input)\n
 * 2. If pointer to the application data is not set.
 * [Pointer to application data or NULL|N/A]
 *
 * @par Required Header File
 * ssl_ex.h
 *
 * @par Note
 * \n
 * NA
 *
 * @par Related Topics
 * SSL_set_verify_app_data.
 */
void *SSL_get_verify_app_data(const SSL *s);

/**
 * Func Name:  SSL_CTX_load_cacertificate_buffer
 * @defgroup SSL_ctxLoadCACertificateBuffer
 * @ingroup sslContextFunctions
 * @par Prototype
 * @code
 * int SSL_CTX_load_cacertificate_buffer(SSL_CTX *ctx,
 * char *buffer,
 * int bufferlen,
 * int type)
 * @endcode
 *
 * @par Purpose
 * This is used to load the CA certificate found in a buffer to the SSL context.
 *
 * @par Description
 * SSL_CTX_load_cacertificate_buffer function reads a buffer that contains the certificate in
 * "PEM","PFX", or "DER"format. To verify user certificate during the authentication, we
 * should set it to the SSL Context.
 *
 * @param[in] ctx Pointer to SSL_CTX of either client OR server. [N/A]
 * @param[in] buffer Pointer to the buffer from which the certificates has to be loaded. [N/A]
 * @param[in] bufferlen The length of the buffer,it should be valid and it should match with the buffer. [N/A]
 * @param[in] type Type in which the certificates are stored in the buffer. The formats in which certificates are
 * stored is one of the following:\n
 * SSL_FILETYPE_PFX \n
 * SSL_FILETYPE_PEM \n
 * SSL_FILETYPE_PFX_COM [N/A]
 *
 * @retval 1 On success
 * @retval 0 On failure
 *
 * @par Required Header File
 * ssl_ex.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * SSL_CTX_load_cacertificate_file \n
 */
int SSL_CTX_load_cacertificate_buffer(SSL_CTX *ctx, char *buffer, int bufferlen, int type);

int SSL_CTX_load_chaincertificate_buffer(SSL_CTX *ctx, char *buffer, int bufferlen, int type, int ref_store);

int SSL_CTX_load0_chaincertificate_buffer(SSL_CTX *ctx, char *buffer, int bufferlen, int type);

/**
 * Func Name:  SSL_CTX_load1_chaincertificate_buffer
 * @defgroup SSL_ctxLoadCACertificateBuffer
 * @ingroup sslContextFunctions
 * @par Prototype
 * @code
 * int SSL_CTX_load1_chaincertificate_buffer(SSL_CTX *ctx,
 * char *buffer,
 * int bufferlen,
 * int type,
 * int useref)
 * @endcode
 *
 * @par Purpose
 * This is used to load the CA certificate found in a buffer to the SSL context.
 * It will use the reference of certificate if it has already loaded in SSL_CTX cert_store before
 *
 * @par Description
 * SSL_CTX_load_cacertificate_buffer function reads a buffer that contains the certificate in
 * "PEM","PFX", or "DER"format. To verify user certificate during the authentication, we
 * should set it to the SSL Context.
 *
 * @param[in] ctx Pointer to SSL_CTX of either client OR server. [N/A]
 * @param[in] buffer Pointer to the buffer from which the certificates has to be loaded. [N/A]
 * @param[in] bufferlen The length of the buffer,it should be valid and it should match with the buffer. [N/A]
 * @param[in] type Type in which the certificates are stored in the buffer. The formats in which certificates are
 * stored is one of the following:\n
 * SSL_FILETYPE_PFX \n
 * SSL_FILETYPE_PEM \n
 * SSL_FILETYPE_PFX_COM [N/A]
 *
 * @retval 1 On success
 * @retval 0 On failure
 *
 * @par Required Header File
 * ssl_ex.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * SSL_CTX_load_chaincertifcate_buffer \n
 */
int SSL_CTX_load1_chaincertificate_buffer(SSL_CTX *ctx, char *buffer, int bufferlen, int type);

/**
 * Func Name:  SSL_CTX_load1_verifycertificate_buffer
 * @defgroup SSL_CTX_load1_verifycertificate_buffer
 * @ingroup sslContextFunctions
 * @par Prototype
 * @code
 * int SSL_CTX_load1_verifycertificate_buffer(SSL_CTX *ctx,
 * char *buffer,
 * int bufferlen,
 * int type)
 *
 * @endcode
 *
 * @par Purpose
 * This is used to load the CA certificate found in a buffer to the SSL context.
 * It will use the reference of certificate if it has already loaded in SSL_CTX cert_store before
 *
 * @par Description
 * SSL_CTX_load1_verifycertificate_buffer function reads a buffer that contains the CA certificate in "PEM","PFX", or
 * "DER"format. It will use the reference of certificate if it has already loaded in SSL_CTX cert_store before.
 * To verify user certificate during the authentication, we should set it to the SSL Context.
 *
 * @param[in] ctx Pointer to SSL_CTX of either client OR server. [N/A]
 * @param[in] buffer Pointer to the buffer from which the certificates has to be loaded. [N/A]
 * @param[in] bufferlen The length of the buffer,it should be valid and it should match with the buffer. [N/A]
 * @param[in] type Type in which the certificates are stored in the buffer.
 *
 * @retval 1 On success
 * @retval 0 On failure
 *
 * @par Required Header File
 * ssl_ex.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * SSL_CTX_load1_chaincertificate_buffer.
 */
int SSL_CTX_load1_verifycertificate_buffer(SSL_CTX *ctx, char *buffer, int bufferlen, int type);

/**
 * Func Name:  SSL_CTX_load_use_certificate_buffer
* @defgroup SSL_CTX_load_use_certificate_buffer
* @ingroup sslContextFunctions
* @par Prototype
* @code
* int SSL_CTX_load_use_certificate_buffer(SSL_CTX *ctx,
* const char *buffer,
* int bufferlen,
* int type)

* @endcode
*
* @par Purpose
* This is used to load the certificate found in a specified buffer to the SSL context.
*
* @par Description
* SSL_CTX_load_use_certificate_buffer function loads the certificate into ctx. It may be either client or server
* (End Entity) certificate. The data in the buffer may be any of the following formats:\n
* SSL_FILETYPE_PFX\n
* SSL_FILETYPE_PEM\n
* SSL_FILETYPE_PFX_COM.
*
* @param[in] ctx SSL Context structure to which the list of certificate has to be stored. This can be used by
* either the server or the client. [N/A]
* @param[in] buffer Pointer to the buffer from which the certificates has to be loaded. [N/A]
* @param[in] bufferlen The length of the buffer,it should be valid and it should match with the buffer. [N/A]
* @param[in] type Type in which the certificates are stored in the buffer. The formats in which certificates are stored
* is one of the following SSL_FILETYPE_PFX_COM, SSL_FILETYPE_PEM, SSL_FILETYPE_ASN1. [N/A]
*
* @retval 1 On success
* @retval 0 On failure
*
* @par Required Header File
* ssl_ex.h
*
* @par Note
* \n
* N/A
*
* @par Related Topics
* SSL_CTX_load_use_certificate_file.
*/
int SSL_CTX_load_use_certificate_buffer(SSL_CTX *ctx, const char *buffer, int bufferlen, int type);

/**
 * @defgroup SSL_load_use_certificate_buffer
 * @ingroup sslContextFunctions
 * @par Prototype
 * @code
 * int SSL_load_use_certificate_buffer(SSL *ssl, const char *buffer, int bufferlen, int type)
 * @endcode
 *
 * @par Purpose
 * This is used to load the certificate found in a specified buffer to the SSL context.
 *
 * @par Description
 * SSL_load_use_certificate_buffer function loads the certificate into ctx.
 * The data in the buffer may be any of the following formats:\n
 * SSL_FILETYPE_PFX\n
 * SSL_FILETYPE_PEM\n
 * SSL_FILETYPE_PFX_COM.
 *
 * @param[in] ssl SSL Context structure to which the list of certificate has to be stored. This can be used by
 * either the server or the client. [N/A]
 * @param[in] buffer Pointer to the buffer from which the certificates has to be loaded. [N/A]
 * @param[in] bufferlen The length of the buffer,it should be valid and it should match with the buffer. [N/A]
 * @param[in] type Type in which the certificates are stored in the buffer. The formats in which certificates are stored
 * is one of the following SSL_FILETYPE_PFX_COM, SSL_FILETYPE_PEM, SSL_FILETYPE_ASN1. [N/A]
 *
 * @retval 1 On success
 * @retval 0 On failure
 *
 * @par Required Header File
 * ssl_ex.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * SSL_CTX_load_use_certificate_buffer
 * SSL_CTX_load_use_certificate_file.
 */
int SSL_load_use_certificate_buffer(SSL *ssl, const char *buffer, int bufferlen, int type);

/**
 * Func Name:  SSL_CTX_load_key_buffer
 * @defgroup SSL_CTX_load_key_buffer
 * @ingroup sslContextFunctions
 * @par Prototype
 * @code
 * int SSL_CTX_load_key_buffer(SSL_CTX *ctx,
 * char *buffer,
 * int bufferlen,
 * int type)
 * @endcode
 *
 * @par Purpose
 * This is used to add the first private key found in a buffer to the SSL context.
 *
 * @par Description
 * SSL_CTX_load_key_buffer function adds the first private key found in buffer to ctx. The buffer type of the key must
 * be specified from the known types SSL_FILETYPE_PEM, SSL_FILETYPE_PFX, and SSL_FILETYPE_ASN1.
 *
 * @param[in] ctx Pointer to SSL Context. [N/A]
 * @param[in] buffer Pointer to the buffer from which the private key has to be loaded. [N/A]
 * @param[in] bufferlen  The length of the buffer,it should be valid and it should match with the buffer. [N/A]
 * @param[in] type The formatting type could be one of the following\n
 * SSL_FILETYPE_ASN1\n
 * SSL_FILETYPE_PEM\n
 * SSL_FILETYPE_PFX_COM. [N/A]
 *
 * @retval 1 On success
 * @retval 0 On failure
 *
 * @par Required Header File
 * ssl_ex.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * SSL_CTX_load_use_certificate_buffer.
 */
int SSL_CTX_load_key_buffer(SSL_CTX *ctx, char *buffer, int bufferlen, int type);

/**
 * @defgroup SSL_load_key_buffer
 * @ingroup sslContextFunctions
 * @par Prototype
 * @code
 * int SSL_load_key_buffer(SSL *ssl, const char *buffer, int bufferlen, int type)
 * @endcode
 *
 * @par Purpose
 * This is used to add the first private key found in a buffer to the SSL context.
 *
 * @par Description
 * This API adds the first private key found in buffer to ctx. The buffer type of the key must
 * be specified from the known types: SSL_FILETYPE_PEM, SSL_FILETYPE_PFX, and SSL_FILETYPE_ASN1.
 *
 * @param[in] ssl Pointer to SSL Context. [N/A]
 * @param[in] buffer Pointer to the buffer from which the private key has to be loaded. [N/A]
 * @param[in] bufferlen  The length of the buffer,it should be valid and it should match with the buffer. [N/A]
 * @param[in] type The formatting type could be one of the following\n
 * SSL_FILETYPE_ASN1\n
 * SSL_FILETYPE_PEM\n
 * SSL_FILETYPE_PFX_COM. [N/A]
 *
 * @retval 1 On success
 * @retval 0 On failure
 *
 * @par Required Header File
 * ssl_ex.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * SSL_CTX_load_key_buffer
 * SSL_CTX_load_use_certificate_buffer.
 */
int SSL_load_key_buffer(SSL *ssl, const char *buffer, int bufferlen, int type);

/**
 * Func Name:  SSL_CTX_load_crl_buffer
 * @defgroup SSL_CTX_load_crl_buffer
 * @ingroup sslContextFunctions
 * @par Prototype
 * @code
 * int SSL_CTX_load_crl_buffer(SSL_CTX *ctx,
 * char *buffer,
 * int buflen,
 * int type,
 * int flags)
 * @endcode
 *
 * @par Purpose
 * This is used to load the CRL found in a buffer to the SSL context.  Also updated CRL
 * file also can be loaded.
 *
 * @par Description
 * SSL_CTX_load_crl_buffer function reads a buffer that contains a CRL (Certificate Revocation List) in "PEM",or "DER"
 * format.
 *
 * @param[in] ctx Pointer to SSL_CTX of either client OR server. [N/A]
 * @param[in] buffer Pointer to the buffer from which the CRL has to be loaded. [N/A]
 * @param[in] bufferlen The length of the buffer,it should be valid and it should match with the buffer. [N/A]
 * @param[in] type The formatting type could be one of the following:\n
 * SSL_FILETYPE_ASN1.\n
 * SSL_FILETYPE_PEM. [N/A]
 * @param[in] flags The flag that is supported. It can be one of the following:\n
 * X509_V_FLAG_CRL_CHECK,\n
 * X509_V_FLAG_CRL_CHECK|X509_V_FLAG_CRL_CHECK_ALL [N/A]
 *
 * @retval 1 On success
 * @retval 0 On failure
 *
 * @par Required Header File
 * ssl_ex.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * SSL_CTX_load_crl_file
 */
int SSL_CTX_load_crl_buffer(SSL_CTX *ctx, char *buffer, int bufferlen, int type, int flags);

/**
 * @defgroup SSL_CTX_load1_verifycrl
 * @ingroup sslContextFunctions
 * @par Prototype
 * @code
 * int SSL_CTX_load1_verifycrl(const SSL_CTX *ctx, const X509_CRL_KEY *key)
 * @endcode
 *
 * @par Purpose
 * This is used to load the CRL in verifty cert store in the SSL_CTX context, which matched the crl key.
 *
 * @par Description
 * SSL_CTX_load_crl_buffer function reads the cert store that load a CRL (match the X509_CRL_KEY key) in "PEM",or "DER"
 * format. If cert store load the CRL matched the key before, the CRL will be referenced.
 *
 * @param[in] ctx Pointer to SSL_CTX of either client OR server. [N/A]
 * @param[in] key Pointer to the crl key matched the crl you want to load.[N/A]
 *
 * @retval 1 On success
 * @retval 0 On failure
 *
 * @par Required Header File
 * ssl_ex.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * SSL_CTX_load_crl_file
 */
int SSL_CTX_load1_verifycrl(const SSL_CTX *ctx, const X509_CRL_KEY *key);

/**
 * Func Name:  SSL_CTX_load1_verifycrlbyhash
 * @defgroup SSL_CTX_load1_verifycrlbyhash
 * @ingroup sslContextFunctions
 * @par Prototype
 * @code
 *
 * int SSL_CTX_load1_verifycrlbyhash(const SSL_CTX *ctx, const unsigned char *md, const unsigned int length)
 *
 * @endcode
 *
 * @par Purpose
 * This is used to load the CRL by hash found in a buffer to the SSL context.  Also updated CRL
 * file also can be loaded.
 *
 * @par Description
 * SSL_CTX_load1_verifycrlbyhash function reads a buffer that contains a CRL
 * (Certificate Revocation List) in "PEM",or "DER"
 * format.
 *
 * @param[in] ctx Pointer to SSL_CTX of either client OR server. [N/A]
 * @param[in] md CRL hash . [N/A]
 * @param[in] length the length of CRL hash. [N/A]
 *
 *
 * @retval 1 On success
 * @retval 0 On failure
 *
 * @par Required Header File
 * ssl_ex.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * SSL_CTX_load_crl_file
 */
int SSL_CTX_load1_verifycrlbyhash(const SSL_CTX *ctx, const unsigned char *md, const unsigned int length);

/**
 * Func Name:  SSL_CTX_load_crl_file
 * @defgroup SSL_CTX_load_crl_file
 * @ingroup sslContextFunctions
 * @par Prototype
 * @code
 * int SSL_CTX_load_crl_file(SSL_CTX *ctx,
 * const char *filename,
 * int type,
 * unsigned int flags)
 * @endcode
 *
 * @par Purpose
 * This is used to load the CRL file to the SSL context. Also updated CRL
 * file also can be loaded.
 *
 * @par Description
 * SSL_CTX_load_crl_file function reads a file that contains a CRL (Certificate Revocation List) in "PEM",or "DER"
 * format. To verify user certificate during the authentication, we should set it to the SSL Context.
 *
 * @param[in] ctx Pointer to SSL_CTX of either client OR server. [N/A]
 * @param[in] filename FileName. [N/A]
 * @param[in] type The formatting type could be one of the following:\n
 * SSL_FILETYPE_ASN1\n
 * SSL_FILETYPE_PEM. [N/A]
 * @param[in] flags The flag that is supported. It can be one of the following:\n
 * X509_V_FLAG_CRL_CHECK,\n
 * X509_V_FLAG_CRL_CHECK|X509_V_FLAG_CRL_CHECK_ALL [N/A]
 *
 * @retval 1 On success
 * @retval 0 On failure
 *
 * @par Required Header File
 * ssl_ex.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * SSL_CTX_load_crl_buffer
 */
int SSL_CTX_load_crl_file(SSL_CTX *ctx, const char *filename, int type, unsigned int flags);

/**
 * Func Name:  SSL_CTX_load_cacertificate_file
 * @defgroup SSL_CTX_load_cacertificate_file
 * @ingroup sslContextFunctions
 * @par Prototype
 * @code
 * int SSL_CTX_load_cacertificate_file(SSL_CTX *ctx,
 * const char *filename,
 * int type)
 * @endcode
 *
 * @par Purpose
 * This is used to load the CA certificate into the SSL context.
 *
 * @par Description
 * SSL_CTX_load_cacertificate_file function reads a file that contains the certificate in "PEM","PFX",
 * or "DER" format. To verify user certificate during the authentication,
 * we should set it to the SSL context.
 *
 * @param[in] ctx Pointer to SSL_CTX of either client or server. [N/A]
 * @param[in] filename FileName. [N/A]
 * @param[in] type The formatting type could be one of the following:\n
 * SSL_FILETYPE_ASN1 \n
 * SSL_FILETYPE_PEM \n
 * SSL_FILETYPE_PFX_COM [N/A]
 *
 * @retval 1 On success
 * @retval 0 On failure
 *
 * @par Required Header File
 * ssl_ex.h
 *
 * @par Note
 * \n
 * 1. This function will allow only version 3 CA certficate to load, and it
 *    allows any verison of root certificate.
 * \n\n
 *
 * @par Related Topics
 * SSL_CTX_load_cacertificate_buffer \n
 */
int SSL_CTX_load_cacertificate_file(SSL_CTX *ctx, const char *filename, int type);

/**
* @defgroup SSL_CTX_load_cross_certificate_buffer
* @ingroup sslContextFunctions
* @par Prototype
* @code
* int SSL_CTX_load_cross_certificate_buffer(SSL_CTX *ctx,
* char *buffer,
* int bufferlen,
* int type)
* @endcode
*
 @par Purpose
* This is used to load the Cross CA certificate found in a buffer to
* the SSL context.
*
* @par Description
* SSL_CTX_load_cross_certificate_buffer function reads a buffer that contains
* the certificate in "PEM","PFX" or "DER"format.
* To verify user certificate during the authentication, we should set it
* to the SSL Context.
*
* @param[in] ctx Pointer to SSL_CTX of either client OR server. [N/A]
* @param[in] buffer Pointer to the buffer from which the certificates has to
* be loaded. [N/A]
* @param[in] bufferlen The length of the buffer,it should be valid and it should match with the buffer. [N/A]
* @param[in] type Type in which the certificates are stored in the buffer.
* The formats in which certificates are stored is one of the following:\n
* SSL_FILETYPE_PFX_COM \n
* SSL_FILETYPE_PEM \n
* SSL_FILETYPE_ASN1 [N/A]
*
* @retval 1 On success
* @retval 0 On failure
*
* @par Required Header File
* ssl_ex.h
*
* @par Note
* This interface will attempt to load other types of trusted CAs, if included.
* If there is a certificate successfully loaded, interface will return success with value 1.\n
*
* @par Related Topics
* SSL_CTX_load_cross_certificate_file \n
*/
int SSL_CTX_load_cross_certificate_buffer(SSL_CTX *ctx, char *buffer, int bufferlen, int type);

/**
 * @defgroup SSL_CTX_load1_verifycrosscertificate_buffer
 * @ingroup sslContextFunctions
 * @par Prototype
 * @code
 * int SSL_CTX_load1_verifycrosscertificate_buffer(SSL_CTX *ctx, char *buffer, int bufferlen, int type)
 * @endcode
 *
 * @par Purpose
 * This is used to load the Cross CA certificate found in a buffer to the SSL_CTX context.
 * It will use the reference of the Cross CA certificate if it has already loaded in SSL_CTX cert_store before
 *
 * @par Description
 * This API reads a buffer that contains the CA certificate in "PEM","PFX", or "DER"format.
 * To verify user certificate during the authentication, we should set it to the SSL Context.
 *
 * @param[in] ctx Pointer to SSL_CTX of either client OR server. [N/A]
 * @param[in] buffer Pointer to the buffer from which the certificates has to be loaded. [N/A]
 * @param[in] bufferlen The length of the buffer,it should be valid and it should match with the buffer. [N/A]
 * @param[in] type Type in which the certificates are stored in the buffer.
 *
 * @retval 1 On success
 * @retval 0 On failure
 *
 * @par Required Header File
 * ssl_ex.h
 *
 * @par Note
 * This interface will attempt to load other types of trusted CAs, if included.
 * If there is a certificate successfully loaded, interface will return success with value 1.\n
 *
 * @par Related Topics
 * SSL_CTX_load_cross_certificate_file \n
 */
int SSL_CTX_load1_verifycrosscertificate_buffer(SSL_CTX *ctx, char *buffer, int bufferlen, int type);

/**
* @defgroup SSL_CTX_load_cross_certificate_file
* @ingroup sslContextFunctions
* @par Prototype
* @code
* int SSL_CTX_load_cross_certificate_file(SSL_CTX *ctx,
* const char *filename,
* int type)
* @endcode
*
 @par Purpose
* This is used to load the Cross CA certificate found in a file to
* the SSL context.
*
* @par Description
* SSL_CTX_load_cross_certificate_file function reads a file that contains
* the certificate in "PEM","PFX" or "DER"format.
* To verify user certificate during the authentication, we should set it
* to the SSL Context.
*
* @param[in] ctx Pointer to SSL_CTX_S of either client OR server. [N/A]
* @param[in] filename Pointer to the file from which the certificates has to
* be loaded. [N/A]
* @param[in] type Type in which the certificates are stored in the buffer.
* The formats in which certificates are stored is one of the following:\n
* SSL_FILETYPE_PFX_COM,\n
* SSL_FILETYPE_PEM,\n
* SSL_FILETYPE_ASN1. [N/A]
*
* @retval 1 On success
* @retval 0 On failure
*
* @par Required Header File
* ssl_ex.h
*
* @par Note
* This interface will attempt to load other types of trusted CAs, if included.
* If there is a certificate successfully loaded, interface will return success with value 1.\n
*
* @par Related Topics
* SSL_CTX_load_cross_certificate_buffer
*/
int SSL_CTX_load_cross_certificate_file(SSL_CTX *ctx, const char *filename, int type);

/**
 * Func Name:  SSL_CTX_load_use_certificate_file
 * @defgroup SSL_CTX_load_use_certificate_file
 * @ingroup sslObjectFunctions
 * @par Prototype
 * @code
 * int SSL_CTX_load_use_certificate_file(SSL_CTX *ctx,
 * const char *filename,
 * int type)
 * @endcode
 *
 * @par Purpose
 * This is used to add the first certificate found in a file to SSL object.
 *
 * @par Description
 * SSL_CTX_load_use_certificate_file function adds the first certificate found in filename to ssl. The format of the
 * certificate must be specified from the known types SSL_FILETYPE_PEM, SSL_FILETYPE_ASN1, and SSL_FILETYPE_PFX_COM.
 *
 * @param[in] ctx Pointer to SSL object. [N/A]
 * @param[in] filename The path of filename. [N/A]
 * @param[in] type The formatting type could be one of the following\n
 * SSL_FILETYPE_ASN1\n
 * SSL_FILETYPE_PEM\n
 * SSL_FILETYPE_PFX_COM. [N/A]
 *
 * @retval 1 On success
 * @retval 0 On failure
 *
 * @par Required Header File
 * ssl_ex.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * SSL_CTX_load_use_certificate_buffer.
 */
int SSL_CTX_load_use_certificate_file(SSL_CTX *ctx, const char *filename, int type);

/**
 * Func Name:  SSL_CTX_load_key_file
 * @defgroup SSL_CTX_load_key_file
 * @ingroup sslObjectFunctions
 * @par Prototype
 * @code
 * int SSL_CTX_load_key_file(SSL_CTX *ctx, const char *filename, int type)
 * @endcode
 *
 * @par Purpose
 * This is used to add the first private key found in a file to SSL object.
 *
 * @par Description
 * SSL_CTX_load_key_file function adds the first private key found in filename to SSL object. The formatting type of the
 * private key must be specified from the known types SSL_FILETYPE_PEM, SSL_FILETYPE_PFX_COM, and SSL_FILETYPE_ASN1.
 *
 * @param[in] ctx Pointer to SSL object. [N/A]
 * @param[in] filename The path of filename. [N/A]
 * @param[in] type The formatting type could be one of the following\n
 * SSL_FILETYPE_ASN1\n
 * SSL_FILETYPE_PEM\n
 * SSL_FILETYPE_PFX_COM. [N/A]
 *
 * @retval 1 On success
 * @retval 0 On failure
 *
 * @par Required Header File
 * ssl_ex.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * SSL_checkPrivateKey.
 */
int SSL_CTX_load_key_file(SSL_CTX *ctx, const char *filename, int type);

/**
 * @defgroup SSL_CTX_add_cipher_to_list
 * @ingroup sslFunctions
 * @par Prototype
 * @code
 * int SSL_CTX_add_cipher_to_list(SSL_CTX *ctx, const char *cipher)
 * @endcode
 *
 * @par Purpose
 * This is used to add a cipher(string) to the current cipher list.
 *
 * @par Description
 * This api add a cipher to the current cipher list, the cipher that you want to add will
 * be appended in front of the current cipher list. And then call SSL_CTX_set_cipher_list
 * to update the new cipher list.
 *
 * @param[in] ctx pointer to SSL context [NA/NA]
 * @param[in] cipher which would be added [NA/NA]
 *
 * @retval int success [1|N/A].
 * @retval int failure [0|N/A].
 *
 * @par Required Header File
 * ssl_ex.h
 * @par Note
 * \n
 * N/A
 */
int SSL_CTX_add_cipher_to_list(SSL_CTX *ctx, const char *cipher);

/**
 * Func Name:   SSL_get_verify_result_ex
 * @defgroup SSL_getVerifyResult
 * @ingroup sslObjectFunctions
 * @par Prototype
 * @code
 * int SSL_get_verify_result_ex(const SSL *ssl)
 * @endcode
 *
 * @par Purpose
 * This is used to get verify result from the SSL object.
 *
 * @par Description
 * SSL_get_verify_result_ex function returns iVerifyResult from the SSL object. The value will be one of a X509_V_ERR_ *
 * macros, which are called verify result codes. A list of all the verify result codes is provided in the section, Error
 * Codes.
 *
 * @param[in] ssl Pointer to SSL structure [N/A]
 *
 * @retval int success [1|N/A].
 * @retval int failure [0|N/A].
 *
 * @par Required Header File
 * ssl_ex.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * N/A.
 */
int SSL_get_verify_result_ex(const SSL *ssl);

/**
 * @defgroup SSL_set_ex_data_ex
 * @ingroup sslFunctions
 * @par Prototype
 * @code
 * int SSL_set_ex_data_ex(SSL *ssl, SSL_ex_data_type type, void *data)
 * @endcode
 *
 * @par Purpose
 * This api is used to set extra data to SSL object.
 *
 * @par Description
 * This api sets extra data to the SSL object, which's data type is specified by type.
 *
 * @param[in] ssl Pointer to SSL object.[N/A]
 * @param[in] type Type of extra data, see enum SSL_ex_data_type, shouldn't be larger than
 * SSL_EX_DATA_TYPE_MAX.[N/A]
 * @param[in] data Pointer to extra data, which will be set to ssl object. No any check
 * is done about this parameter when set. [N/A]
 *
 * @retval int On success.[1|N/A].
 * @retval int On failure.[0|N/A].
 *
 * @par Required Header File
 * ssl_ex.h
 * @par Note
 * If the extra data is memory allocated by the caller, then the caller needs freeing
 * the memory when no longer needed.
 * @par Related Topics
 * SSL_get_ex_data_ex\n
 */
int SSL_set_ex_data_ex(SSL *ssl, SSL_ex_data_type type, void *data);

/**
 * @defgroup SSL_get_ex_data_ex
 * @ingroup sslFunctions
 * @par Prototype
 * @code
 * void *SSL_get_ex_data_ex(const SSL *ssl, SSL_ex_data_type type)
 * @endcode
 *
 * @par Purpose
 * This api is used to get extra data from the SSL object.
 *
 * @par Description
 * This api obtains extra data from the SSL object, which data type is specified by type.
 *
 * @param[in] ssl pointer to SSL object.[N/A]
 * @param[in] type extra data type, see enum SSL_ex_data_type, shouldn't be equal to
 * or larger than SSL_EX_DATA_TYPE_MAX.[N/A]
 *
 * @retval void* On success, the extra data pointer set by SSL_set_ex_data_ex is returned.[Pointer to extra data|N/A].
 * @retval void* On failure.[NULL|N/A].
 *
 * @par Required Header File
 * ssl_ex.h
 * @par Note
 * If the extra data is memory allocated by the caller, then the caller needs freeing
 * the memory when no longer needed.
 * @par Related Topics
 * SSL_set_ex_data_ex\n
 */
void *SSL_get_ex_data_ex(const SSL *ssl, SSL_ex_data_type type);
/**
 * @defgroup SSL_free_bio_method
 * @ingroup sslFunctions
 * @par Prototype
 * @code
 * void SSL_free_bio_method(void)
 * @endcode
 *
 * @par Purpose
 * This api is used to free the bio method created by ssl_create_bio_method or SSL_set_fd_ex.
 *
 * @par Description
 * This api is used to free the bio method created by ssl_create_bio_method or SSL_set_fd_ex.
 *
 * @par Required Header File
 * ssl_ex.h
 * @par Note
 * @par Related Topics
 * SSL_free_bio_method
 */
void SSL_free_bio_method(void);

/**
 * @defgroup OSSL_CertVerifyLevelOneEnableSha1Callback
 * @ingroup sslFunctions
 * @par Prototype
 * @code
 * int OSSL_CertVerifyLevelOneEnableSha1Callback(int ok, X509_STORE_CTX *ctx)
 * @endcode
 *
 * @par Purpose
 * This api is a verify callback for enble sha1 type certificate when security level is 1.
 *
 * @par Description
 * This api is a verify callback for enble sha1 type certificate when security level is 1.
 * @par Note
 * Thread safefy: NA, this is a callback which no private data exists
 * Memory: No input/output object memory operation
 * API cpu consumption: Minor
 * OS support: ALL
 *
 * @par Required Header File
 * ssl_ex.h
 * @par Note
 * @par Related Topics
 * OSSL_CertVerifyLevelOneEnableSha1Callback
 */
int OSSL_CertVerifyLevelOneEnableSha1Callback(int preverify_ok, X509_STORE_CTX *ctx);

/**
 * @defgroup OSSL_SslSecurityLevelOneEnableSha1Callback
 * @ingroup sslFunctions
 * @par Prototype
 * @code
 * int OSSL_SslSecurityLevelOneEnableSha1Callback(
    const SSL *s, const SSL_CTX *ctx, int op, int bits, int nid, void *other, void *ex)
 * @endcode
 *
 * @par Purpose
 * This api is a ssl security check callback for enable sha1/md5_sha1 algorithm when level is 1.
 *
 * @par Description
 * This api is a ssl security check callback for enable sha1/md5_sha1 algorithm when level is 1.
 * @par Note
 * Thread safefy: NA, this is a callback which no private data exists
 * Memory: No input/output object memory operation
 * API cpu consumption: Minor
 * OS support: ALL
 * @par Attention
 * if the callback is called by OpenSSL, either parameter SSL or SSL_CTX is not NULL
 * if the callback is called by other user, user should assure either SSL or SSL_CTX is not NULL
 *
 * @par Required Header File
 * ssl_ex.h
 * @par Note
 * @par Related Topics
 * OSSL_SslSecurityLevelOneEnableSha1Callback
 */
int OSSL_SslSecurityLevelOneEnableSha1Callback(
    const SSL *s, const SSL_CTX *ctx, int op, int bits, int nid, void *other, void *ex);

/**
 * @defgroup X509_STORE_CTX_get1_issuer_timevalid
 * @ingroup sslFunctions
 * @par Prototype
 * @code
 * int X509_STORE_CTX_get1_issuer_timevalid(X509 **issuer, X509_STORE_CTX *ctx, X509 *x)
 * @endcode
 *
 * @par Purpose
 * This api is used to get cert issuer whose certificate time is valid.
 *
 * @par Description
 * This api is used to called by X509_STORE_set_get_issuer and call X509_STORE_CTX_get1_issuer.
 *
 * @par Required Header File
 * ssl_ex.h
 * @par Note
 * @par Related Topics
 * X509_STORE_set_get_issuer
 */
int X509_STORE_CTX_get1_issuer_timevalid(X509 **issuer, X509_STORE_CTX *ctx, X509 *x);

/**
 * @defgroup X509_STORE_Clear
 * @ingroup sslFunctions
 * @par Prototype
 * @code
 * void X509_STORE_Clear(X509_STORE* store);
 * @endcode
 *
 * @par Purpose
 * This api is used to clear certificate in store, delete CA\CRL
 *
 * @par Required Header File
 * ssl_ex.h
 * @par Note
 * @par Related Topics
 * None
 */
void X509_STORE_delete_all_certificate(X509_STORE *store);

#ifdef __cplusplus
}
#endif
#endif
