/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2010-2019. All rights reserved.
 * Description: Contains interface functions for DES
 * ALL RIGHTS RESERVED
 * Project Code: iPSI
 * Module Name: sec_des.h
 * Create: 2010-10-08
 * Author: Prabhat Kumar Gopalika, Kaushik Sarkar
 * Descrption: Contains interface functions for DES
 * ----------------------------------------------------------------------
 * Modification History
 * DATE        NAME             DESCRIPTION
 * --------------------------------------------------------------------
 */
#ifndef SEC_DES_H
#define SEC_DES_H

#include "ipsi_crypto_buildconf.h"

#include "sec_sys.h"
#include "ipsi_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IPSI_DES_CTX_SIZE 128
#define IPSI_DES_BLOCK_SIZE 8

/**
 * @defgroup ipsi_des_ctx_t
 * @ingroup sec_cryptoStructures
 * @par Prototype
 * @code
 * typedef struct{
 * SEC_UCHAR data[IPSI_DES_CTX_SIZE]
 * }ipsi_des_ctx_t
 * @endcode
 *
 * @datastruct data[IPSI_DES_CTX_SIZE] Define the DES Context structure,
 * user does not need to care what is inside the context
 */
typedef struct {
    SEC_UCHAR data[IPSI_DES_CTX_SIZE]; /* !< The context data to encrypt block */
} ipsi_des_ctx_t;

/**
 * Func Name:  ipsi_des_set_key
 * @defgroup ipsi_des_set_key
 * @ingroup LLF Low Level Functions
 * @par Prototype
 * @code
 * SEC_INT32 ipsi_des_set_key(
 * ipsi_des_ctx_t *ctx,
 * const SEC_UCHAR *key,
 * const SEC_INT klen)
 * @endcode
 *
 * @par Purpose
 * This function is used to set the key for DES encryption/decryption
 * operation.
 *
 * @par Description
 * The ipsi_des_set_key function is used to set the key for the DES
 * operation. This function must be called before calling
 * any of the encryption or decrytion functions.
 *
 * @param[in] key The pointer to the buffer holding the encryption and
 * decryption key for DES operation [N/A]
 * @param[in] klen The length of the key in bytes. The key should
 * be 8 bytes long [N/A]
 * @param[out] ctx The DES context [N/A]
 *
 * @retval SEC_INT32 If DES context is properly set [SEC_SUCCESS|N/A]
 * @retval SEC_INT32 If arguments
 * are invalid [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_INT32 If Key length is not equal to 8
 * [SEC_ERR_INVALID_KEY_LEN|N/A]
 *
 * @par Required Header File
 * sec_des.h
 *
 * @par Note Caller is responsible for allocating memory for ctx.
 * \n
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT32 ipsi_des_set_key(ipsi_des_ctx_t *ctx, const SEC_UCHAR *key, const SEC_INT klen);

/*
    Func Name:  ipsi_des_ncbc_encrypt
*/
/**
 * @defgroup ipsi_des_ncbc_encrypt
 * @ingroup LLF Low Level Functions
 * @par Prototype
 * @code
 * SEC_INT32 ipsi_des_ncbc_encrypt(
 * const ipsi_des_ctx_t *ctx,
 * const SEC_UCHAR *in,
 * SEC_UCHAR *out,
 * const SEC_UINT32 inLen,
 * SEC_UCHAR *iv,
 * const SEC_UINT32 ivLen,
 * const SEC_UINT32 outLen);
 * @endcode
 *
 * @par Purpose
 * This function is used to encrypt data in NCBC mode using DES.
 *
 * @par Description
 * The ipsi_des_ncbc_encrypt function is used to encrypt data in NCBC
 * mode using DES.
 *
 * @param[in] ctx The DES context [N/A]
 * @param[in] in The input plaintext [N/A]
 * @param[out] out The output ciphertext [N/A]
 * @param[in] inLen The length of the input data [N/A]
 * @param[in,out] iv The IV for the NCBC operation, IV will be updated by the CBC
 * operation. [N/A]
 * @param[in] ivLen The length of the input iv data in bytes [N/A]
 * @param[in] outLen The length of the output buffer in bytes. should
 * be ((plaintext length + 7)/8)*8 [N/A]
 *
 * @retval SEC_INT32 If encryption operation is successful [SEC_SUCCESS|N/A]
 * @retval SEC_INT32 If context is invalid [SEC_CRYPT_ERR_INVALID_CTX|N/A]
 * @retval SEC_INT32 If arguments are invalid [SEC_ERR_INVALID_ARG|N/A]
 *
 * @par Required Header File
 * sec_des.h
 *
 * @par Note
 * \n
 * 1. This function updates the IV. Caller should keep a local copy of
 * the original IV before calling this function. The updated IV should
 * be used for encryption of the next data buffer.
 * 2. Caller is responsible for allocating memory for out.
 * 3. The out should point to a data buffer of length
 * ((plaintext length + 7)/8)*8
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT32 ipsi_des_ncbc_encrypt(const ipsi_des_ctx_t *ctx, const SEC_UCHAR *in, SEC_UCHAR *out,
    const SEC_UINT32 inLen, SEC_UCHAR *iv, const SEC_UINT32 ivLen, const SEC_UINT32 outLen);

/*
    Func Name:  ipsi_des_ncbc_decrypt
*/
/**
 * @defgroup ipsi_des_ncbc_decrypt
 * @ingroup LLF Low Level Functions
 * @par Prototype
 * @code
 * SEC_INT32 ipsi_des_ncbc_decrypt(
 * const ipsi_des_ctx_t *ctx,
 * const SEC_UCHAR *in,
 * SEC_UCHAR *out,
 * const SEC_UINT32 inLen,
 * SEC_UCHAR *iv,
 * const SEC_UINT32 ivLen,
 * const SEC_UINT32 outLen);
 * @endcode
 *
 * @par Purpose
 * This function is used to decrypt data in NCBC mode using DES.
 *
 * @par Description
 * The ipsi_des_ncbc_decrypt function is used to decrypt data in NCBC
 * mode using DES.
 *
 * @param[in] ctx The AES context [N/A]
 * @param[in] in The input ciphertext [N/A]
 * @param[out] out The output plaintext [N/A]
 * @param[in] inLen The length of the input data [N/A]
 * @param[in,out] iv The IV for the NCBC operation, IV will be updated by the CBC
 * operation. [N/A]
 * @param[in] ivLen The length of the input iv data in bytes [N/A]
 * @param[in] outLen The length of the output buffer in bytes [N/A]
 *
 * @retval SEC_INT32 If decryption operation is successful [SEC_SUCCESS|N/A]
 * @retval SEC_INT32 If context is invalid [SEC_CRYPT_ERR_INVALID_CTX|N/A]
 * @retval SEC_INT32 If arguments are invalid [SEC_ERR_INVALID_ARG|N/A]
 *
 * @par Required Header File
 * sec_des.h
 *
 * @par Note
 * \n
 * 1. This function updates the IV. Caller should keep a local copy of
 * the original IV before calling this function. The updated IV should
 * be used for decryption of the next data buffer.
 * 2. Caller is responsible for allocating memory for out.
 * 3. Character pointer 'in' should point to a data buffer whose length
 * is a multiple of 8
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT32 ipsi_des_ncbc_decrypt(const ipsi_des_ctx_t *ctx, const SEC_UCHAR *in, SEC_UCHAR *out,
    const SEC_UINT32 inLen, SEC_UCHAR *iv, const SEC_UINT32 ivLen, const SEC_UINT32 outLen);

/*
    Func Name:  ipsi_des_encrypt
*/
/**
 * @defgroup ipsi_des_encrypt
 * @ingroup LLF Low Level Functions
 * @par Prototype
 * @code
 * SEC_INT32 ipsi_des_encrypt(
 * const ipsi_des_ctx_t *ctx,
 * const SEC_UCHAR *in,
 * SEC_UCHAR *out,
 * const SEC_UINT32 inLen,
 * const SEC_UINT32 outLen)
 * @endcode
 *
 * @par Purpose
 * This function is used to encrypt a single block of data using DES.
 *
 * @par Description
 * The ipsi_des_encrypt function is used to encrypt a single block
 * of data using DES.
 *
 * @param[in] ctx The DES context [N/A]
 * @param[in] in The input plaintext. It must be 8 bytes long [N/A]
 * @param[out] out The output ciphertext. it must be 8 bytes long [N/A]
 * @param[in] inLen The length of input buffer; this length should be
 * IPSI_DES_BLOCK_SIZE [N/A]
 * @param[in] outLen The length of output buffer; this length should be
 * IPSI_DES_BLOCK_SIZE [N/A]
 *
 * @retval SEC_INT32 If encryption operation is successful [SEC_SUCCESS|N/A]
 * @retval SEC_INT32 If context is invalid [SEC_CRYPT_ERR_INVALID_CTX|N/A]
 * @retval SEC_INT32 If arguments are invalid [SEC_ERR_INVALID_ARG|N/A]
 *
 * @par Required Header File
 * sec_des.h
 *
 * @par Note
 * \n
 * 1. The buffers pointed by 'in' and 'out' must be 8 bytes long \n
 * 2. User must allocate the memory for 'out' \n
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT32 ipsi_des_encrypt(const ipsi_des_ctx_t *ctx, const SEC_UCHAR *in, SEC_UCHAR *out,
    const SEC_UINT32 inLen, const SEC_UINT32 outLen);

/*
    Func Name:  ipsi_des_decrypt
*/
/**
 * @defgroup ipsi_des_decrypt
 * @ingroup LLF Low Level Functions
 * @par Prototype
 * @code
 * SEC_INT32 ipsi_des_decrypt(
 * const ipsi_des_ctx_t *ctx,
 * const SEC_UCHAR *in,
 * SEC_UCHAR *out,
 * const SEC_UINT32 inLen,
 * const SEC_UINT32 outLen)
 * @endcode
 *
 * @par Purpose
 * This function is used to decrypt a single block of data using DES.
 *
 * @par Description
 * The ipsi_des_decrypt function is used to decrypt a single block
 * of data using DES.
 *
 * @param[in] ctx The DES context [N/A]
 * @param[in] in The input ciphertext. It must be 8 bytes long [N/A]
 * @param[out] out The output plaintext. It must be 8 bytes long [N/A]
 * @param[in] inLen The length of input buffer; this length should be
 * IPSI_DES_BLOCK_SIZE [N/A]
 * @param[in] outLen The length of output buffer; this length should be
 * IPSI_DES_BLOCK_SIZE [N/A]
 *
 * @retval SEC_INT32 If decryption operation is successful [SEC_SUCCESS|N/A]
 * @retval SEC_INT32 If context is invalid [SEC_CRYPT_ERR_INVALID_CTX|N/A]
 * @retval SEC_INT32 If arguments are invalid [SEC_ERR_INVALID_ARG|N/A]
 *
 * @par Required Header File
 * sec_des.h
 *
 * @par Note
 * \n
 * 1. The buffers pointed by 'in' and 'out' must be 8 bytes long \n
 * 2. User must allocate the memory for 'out' \n
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT32 ipsi_des_decrypt(const ipsi_des_ctx_t *ctx, const SEC_UCHAR *in, SEC_UCHAR *out,
    const SEC_UINT32 inLen, const SEC_UINT32 outLen);

#ifdef __cplusplus
}
#endif

#endif /* _IPSI_CORE_DES_H__ */
