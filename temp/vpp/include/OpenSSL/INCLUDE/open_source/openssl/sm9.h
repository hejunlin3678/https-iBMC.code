/*
 * Copyright 1995-2022 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */
 
#ifndef HEADER_SM9_H
#define HEADER_SM9_H
 
#ifndef OPENSSL_NO_SM9
 
#include <openssl/bn.h>
#include <openssl/ossl_typ.h>
 
#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief 
 * This structure contains information about SM9 point on SM9 Additive Groups. The SM9 point could be over G1 Group or 
 * G2 Group.
 * SM9_POINT_st is a union structure of G1/G2 point. EC_POINT contains G1 point, point_t contains G2 point.
 * The points of the Group G1 depend on the data structure of the points of the elliptic curve.
 * The points of the Group G2 depend on point_t provided by GmSSL.
 * Details about how the point is generated refering to GM/T 0044.1-2016.
 * Should be created before using and freed after using.
 * 
 * Created by SM9_g2_point_new() or SM9_g1_point_new().
 * Freed by SM9_g2_point_free() or SM9_g1_point_free()
 *    
*/
/**
 * @defgroup SM9_POINT
 * @ingroup SM9 structure
 * @code
 * typedef struct SM9_POINT_st SM9_POINT;
 * @endcode
*/
typedef struct SM9_POINT_st SM9_POINT;
/**
 * @brief 
 * This structure contains information about SM9 signature used by SM9_do_sign() and SM9_do_verify(). h contains hash value of signature, pointS contains point
 * on G1 Group. 
 * SM9_SIG should be created before using SM9_do_sign().
 * Details about how the signature is generated refering to GM/T 0044.2-2016.
 * 
 * Created by SM9_SIG_new()
 * Freed by SM9_SIG_free()
*/
/**
 * @defgroup SM9_SIG
 * @ingroup SM9 structure
 * @code 
 * typedef struct SM9_SIG_st SM9_SIG;
 * @endcode
*/
typedef struct SM9_SIG_st SM9_SIG;
/**
 * @brief 
 * This structure contains information about SM9 point on SM9 Multiplicative Groups.
 * Due to performance optimization, the homomorphic mapping of fixed points needs to be precalculated. 
 * This element depends on fp12_t provided by GmSSL.
 * Details about how the point is generated refering to GM/T 0044.1-2016.
 * Should be created before using and freed after using.
 * 
 * Created by SM9_gt_element_new()
 * Freed by SM9_gt_element_free()
*/
/**
 * @defgroup SM9_GT_ELEMENT
 * @ingroup SM9 structure
 * @code
 * typedef struct SM9_GT_ELEMENT_st SM9_GT_ELEMENT;
 * @endcode
 * @datastruct gtElement Element on GT Group
*/
typedef struct SM9_GT_ELEMENT_st SM9_GT_ELEMENT;
 
/**
 * @brief 
 * This structure contains information about SM9 key. SM9 key will be used in maintaining master keys and user keys, and
 * used in sign/verify ,keys exchange and encrypt/decrypt.
 * Should be created before using and freed after using.
 * 
 * Created by SM9_KEY_new()
 * Freed by SM9_KEY_free()
 * 
*/
/**
 * @defgroup SM9_KEY
 * @ingroup SM9 structure
 * @code 
 * typedef struct SM9_KEY_st SM9_KEY;
 * @endcode
*/
typedef struct SM9_KEY_st SM9_KEY;
 
/**
 * @brief 
 * This enum contains SM9 convert mdoe
 * point convert mode
 * 1. SM9_POINT_CONV_MODE_DIRECT direct convert: X||Y
 * 2. SM9_POINT_CONV_MODE_PC compressed convert, see GM/T 0044.1-2016 chapter 6.2.8
 */
/**
 * @defgroup SM9_POINT_CONVERT_MODE
 * @ingroup SM9 structure
 * @code
 * typedef enum {
 *	 SM9_POINT_CONV_MODE_DIRECT = 1,
 *	 SM9_POINT_CONV_MODE_PC
 * } SM9_POINT_CONVERT_MODE;
 * @endcode
 * 
 * 
*/
typedef enum {
    SM9_POINT_CONV_MODE_DIRECT = 1,
    SM9_POINT_CONV_MODE_PC
} SM9_POINT_CONVERT_MODE;
 
/**
 * @brief This enum contains SM9 hid. SM9 algorithm identifier, should be one byted provided by KGC as 
 * SM9_HID_SIGN, SM9_HID_EXCH or SM9_HID_ENC, indicating the usage of the user private key.
 * SM9 HID
 * 1. sign
 * 2. key-exch
 * 3. enc
 * 
 */
 
/**
 * @defgroup SM9_HID
 * @ingroup SM9 structure
 * @code
 * typedef enum {
 *	 SM9_HID_SIGN = 1,
 *	 SM9_HID_EXCH = 2,
 *	 SM9_HID_ENC  = 3
 * } SM9_HID;
 * @endcode 
*/
typedef enum {
    SM9_HID_SIGN = 1,
    SM9_HID_EXCH = 2,
    SM9_HID_ENC  = 3
} SM9_HID;
 
/**
 * @brief This enum contains SM9 usage, indicates the SM9 KEY struct usage
 * SM9 USAGE
 * 1. sign
 * 3. enc
 * 
 */
 
/**
 * @defgroup SM9_USAGE
 * @ingroup SM9 structure
 * @code
 * typedef enum {
 *	 SM9_USAGE_SIGN = 1,
 *	 SM9_USAGE_ENC = 2,
 * } SM9_USAGE;
 * @endcode 
*/
typedef enum {
    SM9_USAGE_SIGN = 1,
    SM9_USAGE_ENC  = 2
} SM9_USAGE;
 
/**
 * @brief 
 * This enum contains convert form details of how the SM9 point convert to other data structures, 
 * see GM/T 0044.1-2016 chapter 6.2.8
 * 
 * 1. PC == 02/03, compressed
 * 2. PC == 04, uncompressed
 * 3. PC == 06/07， hybrid
 */
 
/**
 * @defgroup SM9_POINT_CONVERT_FORM
 * @ingroup SM9 structure
 * @code 
 * typedef enum {
 * 	 SM9_POINT_CONVERSION_COMPRESSED_0  = 2,
 *	 SM9_POINT_CONVERSION_COMPRESSED_1  = 3,
 *	 SM9_POINT_CONVERSION_UNCOMPRESSED  = 4,
 *	 SM9_POINT_CONVERSION_HYBRID_0      = 6,
 *	 SM9_POINT_CONVERSION_HYBRID_1      = 7
 * } SM9_POINT_CONVERT_FORM;
 * @endcode 
 * 
 */
typedef enum {
    SM9_POINT_CONVERSION_COMPRESSED_0  = 2,
    SM9_POINT_CONVERSION_COMPRESSED_1  = 3,
    SM9_POINT_CONVERSION_UNCOMPRESSED  = 4,
    SM9_POINT_CONVERSION_HYBRID_0      = 6,
    SM9_POINT_CONVERSION_HYBRID_1      = 7
} SM9_POINT_CONVERT_FORM;
 
/**
 * @defgroup  i2d_SM9_SIG
 * @ingroup SM9 functions
 * @par Prototype
 * int i2d_SM9_SIG(const SM9_KEY *key, const SM9_SIG *sig, unsigned char **pp);
 * @code
 * int i2d_SM9_SIG(const SM9_KEY *key, const SM9_SIG *sig, unsigned char **pp);
 * @endcode
 * @par Purpose
 * To convert SM9 signature to ASN1 form string
 * @par Description
 * This function coverts SM9 signature to ASN1 form string. The 
 * @param[in] 
 * key SM9 key used for sign.
 * sig SM9 signature after signing.
 * @param[out] 
 * pp points to the end of memory been used.
 *
 * @retval 
 * The length of the memory of pp has been used is returned for success, and pp refers to the end of the memory 
 * has been used.
 * 0 is returned for failure.
 * @par Dependency
 * @par Note
 * @Memory operation: alloc、free、size  
 * size of(SM9_SIG_ex) will be malloced and freed internally in function
 * @Thread safe:
 * Thread safe function 
 * @OS difference:
 * With no OS difference
 * @Time consuming:
 * With no time consuming
 *
 * @par Related Topics
 * key and sig should be created before using this function.
 * pp should be malloced with enough memory before using this function, at least 104 byte.
 * N/A
 */
 
int i2d_SM9_SIG(const SM9_KEY *key, const SM9_SIG *sig, unsigned char **pp);
 
/**
 * @defgroup d2i_SM9_SIG
 * @ingroup SM9 functions
 * @par Prototype
 * SM9_SIG *d2i_SM9_SIG(const SM9_KEY *key, const unsigned char **pp, long len);
 * @code
 * SM9_SIG *d2i_SM9_SIG(const SM9_KEY *key, const unsigned char **pp, long len);
 * @endcode
 * @par Purpose
 * To convert an octet string to SM9_SIG.
 * @par Description
 * To convert an octet string to SM9_SIG.
 * @param[in]
 * key SM9_KEY used for signning.
 * pp ASN1 form string contains SM9 signature with length len.
 * len length of pp
 * @param[out] [N/A]
 *
 * @retval SM9_SIG* SM9_SIG converted from octet string
 * @retval NULL if input string is not of valid ASN1 form, or group element in key is not valid.
 * @par Dependency
 * @par Note
 * @Memory operation: alloc、free、size  
 * size of(SM9_SIG_ex) will be malloced and freed internally in function
 * @Thread safe:
 * Thread safe function 
 * @OS difference:
 * With no OS difference
 * @Time consuming:
 * With no time consuming
 *
 * @par Related Topics
 * retval SM9_SIG* should be freed with SM9_SIG_free() by user
 * N/A
 
 */
SM9_SIG *d2i_SM9_SIG(const SM9_KEY *key, const unsigned char **pp, long len);
 
/**
 * @defgroup SM9_g2_point_new
 * @ingroup SM9 functions
 * @par Prototype
 * SM9_POINT *SM9_g2_point_new(void);
 * @code
 * SM9_POINT *SM9_g2_point_new(void);
 * @endcode
 * @par Purpose
 * Create a new SM9 g2 Group point
 * @par Description
 * Create a new SM9 g2 Group point
 * @param[in] [N/A]
 * @param[out] [N/A]
 *
 * @retval SM9_POINT* pointer to the SM9_POINT
 * @retval NULL if malloc memory failed
 * @par Dependency [N/A]
 * @par Note
 * @Memory operation: alloc、free、size  
 * sizeof(SM9_POINT) will be alloced, should be freed with SM9_g2_point_free() by user.
 * @Thread safe:
 * Thread safe function 
 * @OS difference:
 * With no OS difference
 * @Time consuming:
 * With no time consuming
 *
 * @par Related Topics
 * retval SM9_POINT* should be freed with SM9_g2_point_free() by user.
 * N/A
 
 */
SM9_POINT *SM9_g2_point_new(void);
 
/**
 * @defgroup SM9_g2_point_free
 * @ingroup SM9 functions
 * @par Prototype
 * void SM9_g2_point_free(SM9_POINT *point);
 * @code
 * void SM9_g2_point_free(SM9_POINT *point);
 * @endcode
 * @par Purpose
 * To free SM9_POINT memory.
 * @par Description
 * * To free SM9_POINT memory.
 * @param[in]
 * point SM9 point of G1/G2 Group point to be freed.
 * @param[out] N/A N/A [N/A]
 *
 * @retval [N/A]
 * 
 * @par Dependency
 * @par Note
 * @Memory operation: alloc、free、size  
 * Memory of sizeof(SM9_POINT) will be freed in function.
 * @Thread safe:
 * Thread safe function.
 * @OS difference:
 * With no OS difference.
 * @Time consuming:
 * With no time consuming.
 *
 * @par Related Topics
 * Input param point should be malloced with SM9_g2_point_new() by user.
 * N/A
 */
void SM9_g2_point_free(SM9_POINT *point);
 
/**
 * @defgroup SM9_g2_point_octs2point
 * @ingroup SM9 functions
 * @par Prototype
 * int SM9_g2_point_octs2point(SM9_POINT *point, SM9_POINT_CONVERT_MODE mode, const unsigned char *buf, size_t len);
 * @code
 * int SM9_g2_point_octs2point(SM9_POINT *point, SM9_POINT_CONVERT_MODE mode, const unsigned char *buf, size_t len);
 * @endcode
 * @par Purpose
 * To convert octets string to SM9 g2 point.
 * @par Description
 * This function is used o convert octets string to SM9 g2 point. 
 * point should be malloced with SM9_g2_point_new() by user.
 * mode should be SM9_POINT_CONV_MODE_DIRECT or SM9_POINT_CONV_MODE_PC.
 * buf should be octet string with length to be len.
 * len maintains the length of buf.
 * @param[in]
 * point SM9_POINT g2 point malloced with SM9_g2_point_new() by user.
 * mode enum SM9_POINT_CONVERT_MODE, should be SM9_POINT_CONV_MODE_DIRECT or SM9_POINT_CONV_MODE_PC.
 * buf octet string with length to be len. If using SM9_POINT_CONV_MODE_PC, unsigned char element at index 0 should 
 * maintains SM9_POINT_CONVERT_FORM. Only SM9_POINT_CONVERSION_UNCOMPRESSED is supported now.
 * len length of buf. 128 byte when using SM9_POINT_CONV_MODE_DIRECT, 129 byte when using SM9_POINT_CONV_MODE_PC with 
 * form to be SM9_POINT_CONVERSION_UNCOMPRESSED
 * @param[out] N/A N/A [N/A]
 *
 * @retval 
 * 1 is returned if conversion is successful
 * 0 is returned if there is failure.
 * 
 * @par Dependency
 * @par Note
 * @Memory operation: alloc、free、size  
 * No memroy alloced or freed
 * @Thread safe:
 * Thread safe function 
 * @OS difference:
 * With no OS difference
 * @Time consuming:
 * With no time consuming
 *
 * @par Related Topics
 * Only SM9_POINT_CONVERSION_UNCOMPRESSED is supported now when using SM9_POINT_CONV_MODE_PC.
 * N/A
 */
 
 
int SM9_g2_point_octs2point(SM9_POINT *point, SM9_POINT_CONVERT_MODE mode, const unsigned char *buf, size_t len);
 
/**
 * @defgroup SM9_g2_point_point2octs
 * @ingroup SM9 functions 
 * @par Prototype
 * int SM9_g2_point_point2octs(SM9_POINT *point, SM9_POINT_CONVERT_MODE mode,
    SM9_POINT_CONVERT_FORM form, unsigned char *buf, size_t *len);
 * @code
 * int SM9_g2_point_point2octs(SM9_POINT *point, SM9_POINT_CONVERT_MODE mode,
    SM9_POINT_CONVERT_FORM form, unsigned char *buf, size_t *len);
 * @endcode
 * @par Purpose
 * To convert SM9 g2 Group point to octet string.
 * @par Description
 * To convert SM9 g2 Group point to octet string.
 * @param[in]
 * point SM9_POINT to be converted. Should be malloced with SM9_g2_point_new() before using.
 * mode SM9_POINT_CONVERT_MODE, should be SM9_POINT_CONV_MODE_DIRECT or SM9_POINT_CONV_MODE_PC.
 * form SM9_POINT_CONVERT_FORM, used when using SM9_POINT_CONV_MODE_PC mode, only SM9_POINT_CONVERSION_UNCOMPRESSED is 
 * supported by now.
 * @param[out]
 * buf unsigned char* pointer holds the memory of output param.
 * len size_t maintains length of buf.
 *
 * @retval 
 * 1 if conversion if successful
 * 0 if there is failure.
 * @par Dependency
 * @par Note
 * @Memory operation: alloc、free、size  
 * No memroy alloced or freed
 * @Thread safe:
 * Thread safe function 
 * @OS difference:
 * With no OS difference
 * @Time consuming:
 * With no time consuming
 *
 * @par Related Topics
 * only SM9_POINT_CONVERSION_UNCOMPRESSED is supported by now.
 * N/A
 */
 
 
int SM9_g2_point_point2octs(SM9_POINT *point, SM9_POINT_CONVERT_MODE mode,
    SM9_POINT_CONVERT_FORM form, unsigned char *buf, size_t *len);
 
/**
 * @defgroup SM9_g1_point_new
 * @ingroup SM9 functions
 * @par Prototype
 * SM9_POINT *SM9_g1_point_new(const SM9_KEY *key);
 * @code
 * SM9_POINT *SM9_g1_point_new(const SM9_KEY *key);
 * @endcode
 * @par Purpose
 * To create SM9 g1 Group point with SM9_KEY.
 * @par Description
 * To create SM9 g1 Group point with SM9_KEY. key should be created with SM9_KEY_new() by user.
 * @param[in]
 * key SM9_KEY* SM9 g1 Group point created with SM9_KEY_new().
 * @param[out] N/A N/A [N/A]
 *
 * @retval 
 * SM9_POINT * if create successful
 * NULL if there is failure
 * @par Dependency
 * @par Note
 * @Memory operation: alloc、free、size  
 * Memory with sizeof(SM9_POINT) is malloced, should be freed by user with SM9_g1_point_free().
 * @Thread safe:
 * Thread safe function 
 * @OS difference:
 * With no OS difference
 * @Time consuming:
 * With no time consuming
 *
 * @par Related Topics
 * retval should be freed by user with SM9_g1_point_free() after using.
 * N/A
 */
 
 
SM9_POINT *SM9_g1_point_new(const SM9_KEY *key);
 
/**
 * @defgroup SM9_g1_point_free
 * @ingroup SM9 functions
 * @par Prototype
 * void SM9_g1_point_free(SM9_POINT *p);
 * @code
 * void SM9_g1_point_free(SM9_POINT *p);
 * @endcode
 * @par Purpose
 * To free SM9_POINT malloced by SM9_g1_point_new().
 * @par Description
 * To free SM9_POINT malloced by SM9_g1_point_new().
 * @param[in]
 * key SM9_KEY* SM9_POINT malloced by SM9_g1_point_new().
 * @param[out] N/A N/A [N/A]
 *
 * @retval [N/A]
 * 
 * @par Dependency
 * @par Note
 * @Memory operation: alloc、free、size  
 * Memory of sizeof(SM9_POINT) will be freed
 * @Thread safe:
 * Thread safe function 
 * @OS difference:
 * With no OS difference
 * @Time consuming:
 * With no time consuming
 *
 * @par Related Topics
 * N/A
 */
void SM9_g1_point_free(SM9_POINT *p);
 
/**
 * @defgroup SM9_g1_point_octs2point
 * @ingroup SM9 functions
 * @par Prototype
 * int SM9_g1_point_octs2point(SM9_POINT *point, SM9_KEY *key, SM9_POINT_CONVERT_MODE mode,
    const unsigned char *buf, size_t len, BN_CTX *ctx);
 * @code
 * int SM9_g1_point_octs2point(SM9_POINT *point, SM9_KEY *key, SM9_POINT_CONVERT_MODE mode,
    const unsigned char *buf, size_t len, BN_CTX *ctx);
 * @endcode
 * @par Purpose
 * To convert from octet string to SM9 g1 Group point.
 * @par Description
 * To convert from octet string to SM9 g1 Group point.
 * point SM9_POINT * malloced with SM9_g1_point_new() by user.
 * key SM9_KEY* malloced with SM9_KEY_new() by user.
 * mode SM9_POINT_CONVERT_MODE, should be SM9_POINT_CONV_MODE_DIRECT or SM9_POINT_CONV_MODE_PC.
 * buf octet string with length to be len.If using SM9_POINT_CONV_MODE_PC, unsigned char element at index 0 
 * should maintains SM9_POINT_CONVERT_FORM. Only SM9_POINT_CONVERSION_UNCOMPRESSED is supported now.
 * len length of buf. 64 byte when using SM9_POINT_CONV_MODE_DIRECT, 65 byte when using SM9_POINT_CONV_MODE_PC with 
 * form to be SM9_POINT_CONVERSION_UNCOMPRESSED
 * ctx BN_CTX * created by BN_CTX_new().
 * @param[in]
 * point SM9_POINT * malloced with SM9_g1_point_new() by user.
 * key SM9_KEY* malloced with SM9_KEY_new() by user.
 * mode SM9_POINT_CONVERT_MODE, should be SM9_POINT_CONV_MODE_DIRECT or SM9_POINT_CONV_MODE_PC.
 * buf octet string with length to be len.If using SM9_POINT_CONV_MODE_PC, unsigned char element at index 0 
 * should maintains SM9_POINT_CONVERT_FORM. Only SM9_POINT_CONVERSION_UNCOMPRESSED is supported now.
 * len length of buf. 64 byte when using SM9_POINT_CONV_MODE_DIRECT, 65 byte when using SM9_POINT_CONV_MODE_PC with 
 * form to be SM9_POINT_CONVERSION_UNCOMPRESSED
 * ctx BN_CTX * created by BN_CTX_new().
 * @param[out] N/A N/A [N/A]
 *
 * @retval 
 * 1 if conversion is successful
 * 0 if conversion is failed.
 * @par Dependency
 * @par Note
 * @Memory operation: alloc、free、size  
 * No memroy alloced or freed
 * @Thread safe:
 * Thread safe function 
 * @OS difference:
 * With no OS difference
 * @Time consuming:
 * With no time consuming
 *
 * @par Related Topics
 * N/A
 
 */
int SM9_g1_point_octs2point(SM9_POINT *point, SM9_KEY *key, SM9_POINT_CONVERT_MODE mode,
    const unsigned char *buf, size_t len, BN_CTX *ctx);
 
/**
 * @defgroup SM9_g1_point_point2octs
 * @ingroup SM9 functions
 * @par Prototype
 * int SM9_g1_point_point2octs(const SM9_POINT *point, const SM9_KEY *key, SM9_POINT_CONVERT_FORM form,
    SM9_POINT_CONVERT_MODE mode, unsigned char *buf, size_t *len, BN_CTX *ctx);
 * @code
 * int SM9_g1_point_point2octs(const SM9_POINT *point, const SM9_KEY *key, SM9_POINT_CONVERT_FORM form,
    SM9_POINT_CONVERT_MODE mode, unsigned char *buf, size_t *len, BN_CTX *ctx);
 * @endcode
 * @par Purpose
 * To convert from SM9 g1 point to octet string.
 * @par Description
 * To convert from SM9 g1 point to octet string.
 * point SM9_POINT * to be converted, malloced with SM9_g1_point_new() by user.
 * key SM9_KEY * used to create SM9_POINT, malloced with SM9_KEY_new() by user.
 * form SM9_POINT_CONVERT_FORM, used when using SM9_POINT_CONV_MODE_PC mode, only SM9_POINT_CONVERSION_UNCOMPRESSED is 
 * supported by now.
 * mode SM9_POINT_CONVERT_MODE, should be SM9_POINT_CONV_MODE_DIRECT or SM9_POINT_CONV_MODE_PC.
 * buf octet string with length to be len.If using SM9_POINT_CONV_MODE_PC, unsigned char element at index 0 
 * should maintains SM9_POINT_CONVERT_FORM. Only SM9_POINT_CONVERSION_UNCOMPRESSED is supported now.
 * len length of buf. 64 byte when using SM9_POINT_CONV_MODE_DIRECT, 65 byte when using SM9_POINT_CONV_MODE_PC with 
 * form to be SM9_POINT_CONVERSION_UNCOMPRESSED
 * ctx BN_CTX * created by BN_CTX_new().
 * @param[in]
 * point SM9_POINT * to be converted, malloced with SM9_g1_point_new() by user.
 * key SM9_KEY * used to create SM9_POINT, malloced with SM9_KEY_new() by user.
 * form SM9_POINT_CONVERT_FORM, used when using SM9_POINT_CONV_MODE_PC mode, only SM9_POINT_CONVERSION_UNCOMPRESSED is 
 * supported by now.
 * mode SM9_POINT_CONVERT_MODE, should be SM9_POINT_CONV_MODE_DIRECT or SM9_POINT_CONV_MODE_PC.
 * buf octet string with length to be len.If using SM9_POINT_CONV_MODE_PC, unsigned char element at index 0 
 * should maintains SM9_POINT_CONVERT_FORM. Only SM9_POINT_CONVERSION_UNCOMPRESSED is supported now.
 * ctx BN_CTX * created by BN_CTX_new().
 * @param[out]
 * buf octet string with length to be len.If using SM9_POINT_CONV_MODE_PC, unsigned char element at index 0 
 * should maintains SM9_POINT_CONVERT_FORM. Only SM9_POINT_CONVERSION_UNCOMPRESSED is supported now.
 * len length of buf. 64 byte when using SM9_POINT_CONV_MODE_DIRECT, 65 byte when using SM9_POINT_CONV_MODE_PC with 
 * form to be SM9_POINT_CONVERSION_UNCOMPRESSED
 *
 * @retval 
 * 1 if conversion is successful
 * 0 if conversion is failed.
 * @par Dependency
 * @par Note
 * @Memory operation: alloc、free、size  
 * No memroy alloced or freed
 * @Thread safe:
 * Thread safe function 
 * @OS difference:
 * With no OS difference
 * @Time consuming:
 * With no time consuming
 *
 * @par Related Topics
 * N/A
 */
 
int SM9_g1_point_point2octs(const SM9_POINT *point, const SM9_KEY *key, SM9_POINT_CONVERT_FORM form,
    SM9_POINT_CONVERT_MODE mode, unsigned char *buf, size_t *len, BN_CTX *ctx);
 
/**
 * @defgroup SM9_gt_element_new
 * @ingroup SM9 functions
 * @par Prototype
 * SM9_GT_ELEMENT *SM9_gt_element_new(void);
 * @code
 * SM9_GT_ELEMENT *SM9_gt_element_new(void);
 * @endcode
 * @par Purpose
 * To create SM9_GT_ELEMENT.
 * @par Description
* To create SM9_GT_ELEMENT.
 * @param[in] [N/A]
 * @param[out] [N/A]
 *
 * @retval 
 * SM9_GT_ELEMENT* pointer to the gt element created.
 * @par Dependency
 * @par Note
 * @Memory operation: alloc、free、size  
 * Memory of sizeof(SM9_GT_ELEMENT) is malloced, should be freed by user with SM9_gt_element_free().
 * @Thread safe:
 * Thread safe function 
 * @OS difference:
 * With no OS difference
 * @Time consuming:
 * With no time consuming
 *
 * @par Related Topics
 * retval SM9_GT_ELEMENT* should be freed by user with SM9_gt_element_free().
 * N/A
 */
 
SM9_GT_ELEMENT *SM9_gt_element_new(void);
/**
 * @defgroup SM9_gt_element_free
 * @ingroup SM9 functions
 * @par Prototype
 * void SM9_gt_element_free(SM9_GT_ELEMENT *t);
 * @code
 * void SM9_gt_element_free(SM9_GT_ELEMENT *t);
 * @endcode
 * @par Purpose
 * To free SM9_GT_ELEMENT created by SM9_gt_element_new()
 * @par Description
 * To free SM9_GT_ELEMENT created by SM9_gt_element_new()
 * @param[in]   
 * t SM9_GT_ELEMENT* to be freed
 * @param[out]  [N/A]
 *
 * @retval  [N/A]
 * 
 * @par Dependency
 * @par Note
 * @Memory operation: alloc、free、size  
 * Memory of sizeof(SM9_GT_ELEMENT) will be freed.
 * @Thread safe:
 * Thread safe function 
 * @OS difference:
 * With no OS difference
 * @Time consuming:
 * With no time consuming
 *
 * @par Related Topics
 * N/A
 */
void SM9_gt_element_free(SM9_GT_ELEMENT *t);
/**
 * @defgroup SM9_gt_element_calc
 * @ingroup SM9 functions
 * @par Prototype.
 * int SM9_gt_element_calc(SM9_GT_ELEMENT *t, const SM9_POINT *p2, const SM9_POINT *p1, BN_CTX *bn_ctx);
 * @code
 * int SM9_gt_element_calc(SM9_GT_ELEMENT *t, const SM9_POINT *p2, const SM9_POINT *p1, BN_CTX *bn_ctx);
 * @endcode
 * @par Purpose
 * To calculated GT element w on GT group. g = e(P1, Ppub)
 * Details about how the point is calculated refering to GM/T 0044.2-2016.
 * @par Description
 * To calculated GT element w on GT group. g = e(P1, Ppub)
 * Details about how the point is calculated refering to GM/T 0044.2-2016.
 * @param[in]
 * t SM9_GT_ELEMENT* created by user with SM9_gt_element_new().
 * p2 SM9_POINT* g2 Group point created by user with SM9_g2_point_new().
 * p1 SM9_POINT* g1 generator created by user with SM9_key_get_g1generator().
 * bn_ctx BN_CTX* created by user with BN_CTX_new().
 * @param[out]  [N/A]
 *
 * @retval 
 * 1 if conversion is successful
 * 0 if conversion is failed.
 * @par Dependency
 * @par Note
 * @Memory operation: alloc、free、size  
 * No memroy alloced or freed
 * @Thread safe:
 * Thread safe function 
 * @OS difference:
 * With no OS difference
 * @Time consuming:
 * About 20ms
 *
 * @par Related Topics
 * This function is used to precalculate the gt element. 
 * When the master public key and the G1 generator are determined, the gt element is a determined value, 
 * which can be pre-computated once for multiple subsequent computations.
 * N/A
 */
 
int SM9_gt_element_calc(SM9_GT_ELEMENT *t, const SM9_POINT *p2, const SM9_POINT *p1, BN_CTX *bn_ctx);
/**
 * @defgroup SM9_KEY_new
 * @ingroup SM9 functions
 * @par Prototype
 * SM9_KEY *SM9_KEY_new(void);
 * @code
 * SM9_KEY *SM9_KEY_new(void);
 * @endcode
 * @par Purpose
 * To create SM9_KEY for sign/verify,key-exchange or encrpt/decrypt.
 * @par Description
 * To create SM9_KEY for sign/verify,key-exchange or encrpt/decrypt.
 * @param[in]   [N/A]
 * @param[out]  [N/A]
 *
 * @retval 
 * SM9_KEY* pointer to the SM9_KEY been created.
 * Null if there is failure.
 * @par Dependency
 * @par Note
 * @Memory operation: alloc、free、size  
 * Memory of sizeof(SM9_KEY) is malloced.
 * @Thread safe:
 * Thread safe function 
 * @OS difference:
 * With no OS difference
 * @Time consuming:
 * With no time consuming
 *
 * @par Related Topics
 * retval SM9_KEY* should be freed by user with SM9_KEY_free().
 * N/A
 */
SM9_KEY *SM9_KEY_new(void);
 
/**
 * @defgroup SM9_KEY_free
 * @ingroup SM9 functions
 * @par Prototype
 * void SM9_KEY_free(SM9_KEY *r);
 * @code
 * void SM9_KEY_free(SM9_KEY *r);
 * @endcode
 * @par Purpose
 * To free SM9_KEY created by SM9_KEY_new().
 * @par Description
 * To free SM9_KEY created by SM9_KEY_new().
 * @param[in]
 * r SM9_KEY* created by SM9_KEY_new().
 * @param[out]  [N/A]
 *
 * @retval [N/A]
 * 
 * @par Dependency
 * @par Note
 * @Memory operation: alloc、free、size  
 * Memory of sizeof(SM9_KEY) is freed.
 * @Thread safe:
 * Thread safe function 
 * @OS difference:
 * With no OS difference
 * @Time consuming:
 * With no time consuming
 *
 * @par Related Topics
 * input param SM9_KEY* r should be created by SM9_KEY_new().
 * N/A
 */
void SM9_KEY_free(SM9_KEY *r);
 
/**
 * @defgroup SM9_key_set_masterpubkey
 * @ingroup sm9Functions
 * @par Prototype
 * @code
 * int SM9_key_set_masterpubkey(SM9_KEY *key, SM9_POINT *pubs)
 * @endcode
 *
 * @par Purpose
 * Set master public key into SM9 key struct.
 * @par Description
 * Set master public key into SM9 key struct. Master public key is point on G2 group.
 *
 * @param[in] key SM9 key [N/A]
 * @param[in] pubs master public key [N/A]
 *
 * @retval int set success [1|N/A]
 * @retval int set failed [0|N/A]
 *
 * @par Dependency
 * sm9.h
 *
 * @Memory operation: alloc、free、size  
 * N/A
 *
 * @Thread safe:
 * SAFE
 *
 * @OS difference:
 * N/A
 *
 * @Time consuming:
 * N/A
 *
 * @par Note
 * User should free pubs after all sm9 process. Openssl will not free the old pubs 
 * when user call  SM9_key_set_masterpubkey again.
 *
 * @par Related Topics
 * N/A
 */
int SM9_key_set_masterpubkey(SM9_KEY *key, SM9_POINT *pubs);
 
/**
 * @defgroup SM9_key_set_usage
 * @ingroup sm9Functions
 * @par Prototype
 * @code
 * int SM9_key_set_usage(SM9_KEY *key, int usage)
 * @endcode
 *
 * @par Purpose
 * Set usage into SM9 key struct.
 * @par Description
 * Set usage into SM9 key struct.
 *
 * @param[in] key SM9 key [N/A]
 * @param[in] usage SM9 key usage [N/A]
 *
 * @retval int set success [1|N/A]
 * @retval int set failed [0|N/A]
 *
 * @par Dependency
 * sm9.h
 *
 * @Memory operation: alloc、free、size  
 * N/A
 *
 * @Thread safe:
 * SAFE
 *
 * @OS difference:
 * N/A
 *
 * @Time consuming:
 * N/A
 *
 * @par Note
 *
 * @par Related Topics
 * N/A
 */
int SM9_key_set_usage(SM9_KEY *key, int usage);
 
/**
 * @defgroup SM9_key_set_precalcsigng
 * @ingroup sm9Functions
 * @par Prototype
 * @code
 * int SM9_key_set_precalcsigng(SM9_KEY *key, SM9_GT_ELEMENT *t)
 * @endcode
 *
 * @par Purpose
 * Set pre-calculated g into SM9 key struct.
 * @par Description
 * Set pre-calculated g into SM9 key struct; g is used for SM9 rate-pairing alg.
 *
 * @param[in] key SM9 key [N/A]
 * @param[in] t pre-calculated g [N/A]
 *
 * @retval int hid set success [1|N/A]
 * @retval int hid set failed [0|N/A]
 *
 * @par Dependency
 * sm9.h
 *
 * @Memory operation: alloc、free、size  
 * N/A
 *
 * @Thread safe:
 * SAFE
 *
 * @OS difference:
 * N/A
 *
 * @Time consuming:
 * N/A
 *
 * @par Note
 * User should free t after all sm9 process. Openssl will not free the old t 
 * when user call  SM9_key_set_precalcsigng again.
 *
 * @par Related Topics
 * N/A
 */
int SM9_key_set_precalcsigng(SM9_KEY *key, SM9_GT_ELEMENT *t);
 
/**
 * @defgroup SM9_key_set_userprikey
 * @ingroup sm9Functions
 * @par Prototype
 * @code
 * int SM9_key_set_userprikey(SM9_KEY *key, SM9_POINT *userpri)
 * @endcode
 *
 * @par Purpose
 * Set user private key into SM9 key struct.
 * @par Description
 * Set user private key into SM9 key struct.
 *
 * @param[in] key SM9 key [N/A]
 * @param[in] userpri user private key, point on G1 group [N/A]
 *
 * @retval int set success [1|N/A]
 * @retval int set failed [0|N/A]
 *
 * @par Dependency
 * sm9.h
 *
 * @Memory operation: alloc、free、size  
 * N/A
 *
 * @Thread safe:
 * SAFE
 *
 * @OS difference:
 * N/A
 *
 * @Time consuming:
 * N/A
 *
 * @par Note
 * User should free the userpri after all sm9 process. Openssl will not free the old userpri 
 * when user call  SM9_key_set_userprikey again.
 *
 * @par Related Topics
 * N/A
 */
int SM9_key_set_userprikey(SM9_KEY *key, SM9_POINT *userpri);
 
/**
 * @defgroup SM9_key_set_hid
 * @ingroup sm9Functions
 * @par Prototype
 * @code
 * int SM9_key_set_hid(SM9_KEY *key, int hid)
 * @endcode
 *
 * @par Purpose
 * Set hid into SM9 key struct
 * @par Description
 * Set hid into SM9 key struct, hid defines in enum SM9_HID
 *
 * @param[in] key SM9 key [N/A]
 * @param[in] hid enum value in SM9_HID [N/A]
 *
 * @retval int set success [1|N/A]
 * @retval int set failed [0|N/A]
 *
 * @par Dependency
 * sm9.h
 *
 * @Memory operation: alloc、free、size  
 * N/A
 *
 * @Thread safe:
 * SAFE
 *
 * @OS difference:
 * N/A
 *
 * @Time consuming:
 * N/A
 *
 * @par Note
 *
 * @par Related Topics
 * N/A
 */
int SM9_key_set_hid(SM9_KEY *key, int hid);
 
/**
 * @defgroup SM9_key_get_g1generator
 * @ingroup sm9Functions
 * @par Prototype
 * @code
 * const SM9_POINT *SM9_key_get_g1generator(const SM9_KEY *key)
 * @endcode
 *
 * @par Purpose
 * Get SM9 g1 generator point
 * @par Description
 * Get SM9 g1 generator point, depend on key->group
 *
 * @param[in] key SM9 key [N/A]
 * @param[out] N/A N/A [N/A]
 .
 * @retval SM9_POINT* g1 generator [SM9_POINT*|N/A]
 *
 * @par Dependency
 * sm9.h
 *
 * @Memory operation: alloc、free、size  
 * N/A
 *
 * @Thread safe:
 * SAFE
 *
 * @OS difference:
 * N/A
 *
 * @Time consuming:
 * N/A
 *
 * @par Note
 *
 * @par Related Topics
 * N/A
 */
const SM9_POINT *SM9_key_get_g1generator(const SM9_KEY *key);
 
/**
 * @defgroup SM9_SIG_new
 * @ingroup sm9Functions
 * @par Prototype
 * @code
 * SM9_SIG *SM9_SIG_new(void)
 * @endcode
 *
 * @par Purpose
 * alloc sig
 * @par Description
 * alloc sig
 *
 * @param[in] N/A N/A [N/A]
 * @param[out] N/A N/A [N/A]
 .
 * @retval SM9_SIG* signature struct pointer[SM9_SIG*|N/A]
 *
 * @par Dependency
 * sm9.h
 *
 * @Memory operation: alloc、free、size  
 * 1.alloc signature struct pointer memory
 *
 * @Thread safe:
 * SAFE
 *
 * @OS difference:
 * N/A
 *
 * @Time consuming:
 * N/A
 *
 * @par Note
 *
 * @par Related Topics
 * N/A
 */
SM9_SIG *SM9_SIG_new(void);
 
/**
 * @defgroup SM9_SIG_free
 * @ingroup sm9Functions
 * @par Prototype
 * @code
 * void SM9_SIG_free(SM9_SIG *sig)
 * @endcode
 *
 * @par Purpose
 * Free sig 
 * @par Description
 * Free sig 
 *
 * @param[in] sig SM9 signature for msg [N/A]
 *
 * @retval void [void/NA]
 *
 * @par Dependency
 * sm9.h
 *
 * @Memory operation: alloc、free、size  
 * 1.free sig memory, including sig->h and sig->pointS
 *
 * @Thread safe:
 * SAFE
 *
 * @OS difference:
 * N/A
 *
 * @Time consuming:
 * N/A
 *
 * @par Note
 *
 * @par Related Topics
 * N/A
 */
void SM9_SIG_free(SM9_SIG *sig);
 
/**
 * @defgroup SM9_do_sign
 * @ingroup sm9Functions
 * @par Prototype
 * @code
 * int SM9_do_sign(SM9_SIG *sig, const SM9_KEY *key, const EVP_MD *digest, const uint8_t *msg, size_t msg_len)
 * @endcode
 *
 * @par Purpose
 * Do sign to msg.
 * @par Description
 * Get message SM9 signature, use 256 bit digest method(SM3 or SHA256).
 *
 * @param[in] sig SM9 signature for msg [N/A]
 * @param[in] key SM9 key [N/A]
 * @param[in] digest digest method [N/A]
 * @param[in] msg message [N/A]
 * @param[in] msg_len message length [N/A]
 *
 * @retval int verify success [1|N/A]
 * @retval int verify failed, in the case of:\n
 * 1.NULL input\n
 * 2.invalid digest method\n [0|N/A]
 * 3.sign failed\n [0|N/A]
 *
 * @par Dependency
 * sm9.h
 *
 * @Memory operation: alloc、free、size  
 * 1.alloc sig->h and sig->pointS, call SM9_SIG_free to free these memory.
 *
 * @Thread safe:
 * SAFE
 *
 * @OS difference:
 * N/A
 *
 * @Time consuming:
 * N/A
 *
 * @par Note
 * Input sig must be free and realloc if user want to call SM9_do_sign again(can use API SM9_SIG_free/SM9_SIG_new)
 *
 * @par Related Topics
 * N/A
 */
int SM9_do_sign(SM9_SIG *sig, const SM9_KEY *key, const EVP_MD *digest, const uint8_t *msg, size_t msg_len);
 
/**
 * @defgroup SM9_do_verify
 * @ingroup sm9Functions
 * @par Prototype
 * @code
 * int SM9_do_verify(SM9_SIG *sig, const SM9_KEY *key, const EVP_MD *digest,
 *  const uint8_t *msg, size_t msg_len, unsigned char *id, int idLen)
 * @endcode
 *
 * @par Purpose
 * SM9 signature verify.
 * @par Description
 * Verify sm9 signature
 *
 * @param[in] sig SM9 signature for msg [N/A]
 * @param[in] key SM9 key [N/A]
 * @param[in] digest digest method [N/A]
 * @param[in] msg message [N/A]
 * @param[in] msg_len message length [N/A]
 * @param[in] id peer id [N/A]
 * @param[in] idLen peer id length [N/A]
 *
 * @retval int verify success [1|N/A]
 * @retval int verify failed, in the case of:\n
 * 1.NULL input\n
 * 2.invalid digest method\n [0|N/A]
 * 3.verify failed\n [0|N/A]
 *
 * @par Dependency
 * sm9.h
 *
 * @par Note
 *
 * @Memory operation: alloc、free、size  
 * N/A
 *
 * @Thread safe:
 * SAFE
 *
 * @OS difference:
 * N/A
 *
 * @Time consuming:
 * N/A
 *
 * @par Related Topics
 * N/A
 */
int SM9_do_verify(SM9_SIG *sig, const SM9_KEY *key, const EVP_MD *digest,
    const uint8_t *msg, size_t msg_len, unsigned char *id, int idLen);
 
/**
 * @defgroup SM9_sig2octs
 * @ingroup sm9Functions
 * @par Prototype
 * @code
 * int SM9_sig2octs(const SM9_KEY *key, const SM9_SIG *sig, unsigned char *out, unsigned int *outlen)
 * @endcode
 *
 * @par Purpose
 * Convert sm9 signature struct to octs
 * @par Description
 * SM9 signature struct contains h and pointS(defines on GM/T 0044.2-2016), this function is used for
 * converting struct to octs
 *
 * @param[in] key SM9 key [N/A]
 * @param[in] sig SM9 signature [N/A]
 * @param[in] outlen outbuf space len [N/A]
 * @param[out] out signature octs buf [N/A]
 * @param[out] outlen signature octs buf len [N/A]
 *
 * @retval int convert success [1|N/A]
 * @retval int convert failed, in the case of:\n
 * 1.NULL input\n
 * 2.convert failed\n [0|N/A]
 *
 * @par Dependency
 * sm9.h
 *
 * @par Note
 *
 * @Memory operation: alloc、free、size  
 * N/A
 *
 * @Thread safe:
 * SAFE
 *
 * @OS difference:
 * N/A
 *
 * @Time consuming:
 * N/A
 *
 * @par Related Topics
 * N/A
 */
int SM9_sig2octs(const SM9_KEY *key, const SM9_SIG *sig, unsigned char *out, unsigned int *outlen);

#  ifdef  __cplusplus
}
#  endif
#endif
#endif