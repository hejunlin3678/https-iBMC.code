/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2014-2019. All rights reserved.
 * Description: Contains DRBG and FIPS testability APIs
 * ALL RIGHTS RESERVED
 * Project Code: iPSI
 * Module Name: ipsi_crypto_drbg_ext.h
 * Create: 2014-08-21
 * Author: Prabhat Kumar Gopalika
 * Descrption: Contains DRBG and FIPS testability APIs
 * ----------------------------------------------------------------------
 * Modification History
 * DATE        NAME             DESCRIPTION
 * --------------------------------------------------------------------
 */
#ifndef IPSI_CRYPTO_DRBG_EXT_H
#define IPSI_CRYPTO_DRBG_EXT_H

#include "ipsi_types.h"
#include "ipsi_thread.h"
#include "sec_sys.h"
#include "sec_crypto.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Testability Testability Functions
 * This section contains the testability API which should be used only for testing purpose. These APIs should not be
 * used for commercial releases.
 */
/*
Func Name:  IPSI_CRYPT_drbg_new
*/
/**
 * @defgroup IPSI_CRYPT_drbg_new
 * @ingroup Testability
 * @par Prototype
 * @code
 * IPSI_DRBG_CTX *IPSI_CRYPT_drbg_new(SEC_INT type, SEC_UINT flags)
 * @endcode
 *
 * @par Purpose
 * This function will allocate memory for a DRBG context and initializes it.
 *
 * @par Description
 * IPSI_CRYPT_drbg_new will allocate memory for a DRBG context and initializes it.
 *
 * @param[in] type (Control Input) The Algorithm ID Type. Based on this Algorithm ID type the DRBG internal algorithm
 * will be decided.  [N/A]
 * @param[in] flags (Control Input) The Flag variable which will provide additional flag input during init.
 *
 * @retval IPSI_DRBG_CTX*  (Data Output) If allocation fails [0|N/A]
 * @retval IPSI_DRBG_CTX* (Status Output) Otherwise [New DRBG context|N|A]
 *
 * @par Required Header File
 * ipsi_crypto_drbg_ext.h
 *
 * @par Note
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL IPSI_DRBG_CTX *IPSI_CRYPT_drbg_new(SEC_INT type, SEC_UINT flags);

/*
Func Name:  IPSI_CRYPT_drbg_free
*/
/**
 * @defgroup IPSI_CRYPT_drbg_free
 * @ingroup Testability
 * @par Prototype
 * @code
 * SEC_VOID IPSI_CRYPT_drbg_free(IPSI_DRBG_CTX *dctx)
 * @endcode
 *
 * @par Purpose
 * This function frees up a DRBG_CTX. After this call the DRBG_CTX pointer is no longer valid.
 *
 * @par Description
 * IPSI_CRYPT_drbg_free frees up a DRBG_CTX. After this call the DRBG_CTX pointer is no longer valid.
 * The underlying DRBG is first uninstantiated.
 *
 * @param[in] dctx (Data Input) The DRBG Context.  [N/A]
 *
 * @retval SEC_VOID This function does not return any value [N/A|N/A]
 *
 * @par Required Header File
 * ipsi_crypto_drbg_ext.h
 *
 * @par Note
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_VOID IPSI_CRYPT_drbg_free(IPSI_DRBG_CTX *dctx);

/*
Func Name:  IPSI_CRYPT_drbg_generate
*/
/**
 * @defgroup IPSI_CRYPT_drbg_generate
 * @ingroup Testability
 * @par Prototype
 * @code
 * SEC_UINT32 IPSI_CRYPT_drbg_generate(IPSI_DRBG_CTX *dctx, SEC_UCHAR *out, size_t outlen,
 * SEC_INT prediction_resistance, const SEC_UCHAR *adin, size_t adinlen)
 * @endcode
 *
 * @par Purpose
 * This function attempts to generate "outlen" bytes of random data from the DRBG using optional additional
 * input "adin" of length "adinlen".
 *
 * @par Description
 * IPSI_CRYPT_drbg_generate attempts to generate "outlen" bytes of random data from the DRBG, using optional additional
 * input "adin" of length "adinlen". If the "prediction_resistance" parameter is non-zero a prediction resistance
 * request will be made and internal reseeding of the DRBG and requesting entropy as required by SP800-90 is performed.
 * If an attempt is made to request too much data for a single request or to supply additional input of an invalid
 * length a non-fatal error is returned. If an internal request for entropy fails a fatal error occurs and the DRBG
 * is placed in an error state. The caller must un-instantiate and re-instantiate the DRBG before attempting
 * further calls.
 *
 * @param[out] dctx (Data Output) The DRBG context.  [N/A]
 * @param[out] out (Data Output) Generated random data from DRBG.  [N/A]
 * @param[in] outlen (Data Input) Length of out buffer.  [N/A]
 * @param[in] prediction_resistance (Data Input) Prediction resistance.  [N/A]
 * @param[in] adin (Data Input) The additional input.  [N/A]
 * @param[in] adinlen (Data Input) The additional input length.  [N/A]
 *
 * @retval SEC_UINT32 (Status Output) On Success [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 (Status Output)  Invalid argument passed [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 (Status Output) DRBG health check failure for the selected DRBG mechanism and type
 * [SEC_CRYPT_ERR_DRBG_HEALTH_TEST_FAIL|N/A]
 * @retval SEC_UINT32 (Status Output)  During DRBG generate, DRBG already found in error state
 * [SEC_CRYPT_ERR_DRBG_GENERATE_IN_ERROR_STATE|N/A]
 * @retval SEC_UINT32 (Status Output)  During DRBG generate, DRBG found to be not instantiated
 * [SEC_CRYPT_ERR_DRBG_GENERATE_NOT_INSTANTIATED|N/A]
 * @retval SEC_UINT32 (Status Output)  Attempt is made to request too much data for a single request
 * [SEC_CRYPT_ERR_REQUEST_TOO_LARGE_FOR_DRBG|N/A]
 * @retval SEC_UINT32 (Status Output)  Additional input is of invalid length
 * [SEC_CRYPT_ERR_DRBG_ADDITIONAL_INPUT_TOO_LONG|N/A]
 * @retval SEC_UINT32 (Status Output)  During DRBG reseed, DRBG already found in error state
 * [SEC_CRYPT_ERR_DRBG_RESEED_IN_ERROR_STATE|N/A]
 * @retval SEC_UINT32 (Status Output)  During DRBG reseed, DRBG found to be not instantiated
 * [SEC_CRYPT_ERR_DRBG_RESEED_NOT_INSTANTIATED|N/A]
 * @retval SEC_UINT32 (Status Output)  Error retrieving entropy during DRBG Reseed
 * [SEC_CRYPT_ERR_DRBG_RESEED_RETRIEVING_ENTROPY|N/A]
 * @retval SEC_UINT32 (Status Output)  DRBG mechanism Reseed error[SEC_CRYPT_DRBG_RESEED_ERROR|N/A]
 * @retval SEC_UINT32 (Status Output)  DRBG mechanism Generate error[SEC_CRYPT_DRBG_GENERATE_ERROR|N/A]
 *
 * @retval SEC_UINT32 (Status Output) DRBG generate failed [SEC_ERR|N/A]
 *
 * @par Required Header File
 * ipsi_crypto_drbg_ext.h
 *
 * @par Note
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 IPSI_CRYPT_drbg_generate(IPSI_DRBG_CTX *dctx, SEC_UCHAR *out, size_t outlen,
    SEC_INT prediction_resistance, const SEC_UCHAR *adin, size_t adinlen);

/*
Func Name:  IPSI_CRYPT_drbg_get_blocklength
*/
/**
 * @defgroup IPSI_CRYPT_drbg_get_blocklength
 * @ingroup Testability
 * @par Prototype
 * @code
 * size_t IPSI_CRYPT_drbg_get_blocklength(IPSI_DRBG_CTX *dctx)
 * @endcode
 *
 * @par Purpose
 * This function returns the DRBG block length
 *
 * @par Description
 * IPSI_CRYPT_drbg_get_blocklength returns the DRBG block length.
 *
 * @param[in] dctx (Data Input) The DRBG context.  [N/A]
 *
 * @retval SEC_UINT32 (Data Output) DRBG block length [DRBG block length|N|A]
 *
 * @par Required Header File
 * ipsi_crypto_drbg_ext.h
 *
 * @par Note
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL size_t IPSI_CRYPT_drbg_get_blocklength(const IPSI_DRBG_CTX *dctx);

/*
Func Name:  IPSI_CRYPT_drbg_get_strength
*/
/**
 * @defgroup IPSI_CRYPT_drbg_get_strength
 * @ingroup Testability
 * @par Prototype
 * @code
 * SEC_INT IPSI_CRYPT_drbg_get_strength(IPSI_DRBG_CTX *dctx)
 * @endcode
 *
 * @par Purpose
 * This function returns the security strength of the DRBG in bits..
 *
 * @par Description
 * IPSI_CRYPT_drbg_get_strength returns the security strength of the DRBG in bits..
 *
 * @param[in] dctx (Data Input) The DRBG context.  [N/A]
 *
 * @retval SEC_INT (Data Output) DRBG strength [DRBG strength|N|A]
 *
 * @par Required Header File
 * ipsi_crypto_drbg_ext.h
 *
 * @par Note
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT IPSI_CRYPT_drbg_get_strength(const IPSI_DRBG_CTX *dctx);

/*
    Func Name:  IPSI_CRYPT_get_default_drbg
*/
/**
 * @defgroup IPSI_CRYPT_get_default_drbg
 * @ingroup Testability
 * @par Prototype
 * @code
 * IPSI_DRBG_CTX *IPSI_CRYPT_get_default_drbg(SEC_VOID)
 * @endcode
 *
 * @par Purpose
 * This function can be used to get Default DRBG context Instance
 *
 * @par Description
 * For any new instance of DRBG, the DRBG Context may be created further which is required for all
 * DRBG operations in this context.
 *
 * @param[in] None [N/A]
 *
 * @retval IPSI_DRBG_CTX* (Data Output) Pointer to default DRBG context If succesfull [IPSI_DRBG_CTX*|N/A]
 * @retval IPSI_DRBG_CTX* (Status Output) If Failure [NULL|N/A]
 *
 * @par Required Header File
 * ipsi_crypto_drbg_ext.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL IPSI_DRBG_CTX *IPSI_CRYPT_get_default_drbg(SEC_VOID);

/*
    Func Name:  IPSI_CRYPT_drbg_init
*/
/**
 * @defgroup IPSI_CRYPT_drbg_init
 * @ingroup Testability
 * @par Prototype
 * @code
 * SEC_UINT32 IPSI_CRYPT_drbg_init(IPSI_DRBG_CTX *dctx, SEC_INT type, SEC_UINT flags)
 * @endcode
 *
 * @par Purpose
 * This function is used to Initialize the DRBG Context, which is required for all DRBG operations.
 *
 * @par Description
 * For any new instance of DRBG, the DRBG context may be created further which is required for all
 * DRBG operations in this context.
 *
 * @param[in] dctx (Data Input) Pointer to DRBG Context obtained from IPSI_CRYPT_get_default_drbg API [N/A]
 * @param[in] type (Control Input) The Algorithm ID Type. Based on this Algorithm ID type the DRBG internal algorithm
 * will be decided. The DRBG mechanisms, parameters and strengths are summarized below:
 * - Hash DRBG
 * - ALGID_SHA1	 => Strength = 128
 * - ALGID_SHA224 	 => Strength = 192
 * - ALGID_SHA256 	 => Strength = 256
 * - ALGID_SHA384 	 => Strength = 256
 * - ALGID_SHA512 	 => Strength = 256
 * - HMAC DRBG
 * - ALGID_HMAC_SHA1	   => Strength = 128
 * - ALGID_HMAC_SHA224 	 => Strength = 192
 * - ALGID_HMAC_SHA256 	 => Strength = 256
 * - ALGID_HMAC_SHA384 	 => Strength = 256
 * - ALGID_HMAC_SHA512 	 => Strength = 256
 * - CTR DRBG
 * - ALGID_AES128_CTR	   => Strength = 128
 * - ALGID_AES192_CTR  	 => Strength = 192
 * - ALGID_AES256_CTR 		 => Strength = 256
 * [N/A]
 * @param[in] flags (Control Input) The Flag variable which will provide additional flag input during init.
 * For example if flags variable is set with IPSI_CRYPT_DRBG_FLAG_TEST the DRBG Test feature will be enabled.
 * For CTR DRBG the flag IPSI_CRYPT_DRBG_FLAG_CTR_USE_DF enables the use of a derivation function.
 * If this flag is not set a derivation function is not used. [N/A]
 *
 * @retval SEC_UINT32 (Status Output)  On API Invocation is Success [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 (Status Output)  Invalid argument passed [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 (Status Output) DRBG health check failure for the selected DRBG mechanism and type
 * [SEC_CRYPT_ERR_DRBG_HEALTH_TEST_FAIL|N/A]
 * @retval SEC_UINT32 (Status Output) Invalid or unsupported DRBG type [SEC_CRYPT_ERR_UNSUPPORTED_DRBG_TYPE|N/A]
 * @retval SEC_UINT32 (Status Output) Internal error during initialization of DRBG
 * [SEC_CRYPT_ERR_ERROR_INITIALISING_DRBG|N/A]
 *
 * @par Required Header File
 * ipsi_crypto_drbg_ext.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 IPSI_CRYPT_drbg_init(IPSI_DRBG_CTX *dctx, SEC_INT type, SEC_UINT flags);

/*
    Func Name:  IPSI_CRYPT_drbg_instantiate
*/
/**
* @defgroup IPSI_CRYPT_drbg_instantiate
* @ingroup Testability
* @par Prototype
* @code
*  SEC_UINT32 IPSI_CRYPT_drbg_instantiate(IPSI_DRBG_CTX *dctx, const SEC_UCHAR *pers, size_t perslen)
* @endcode
*
* @par Purpose
* This function is used to Instantiate the DRBG Module
*
* @par Description
* After obtaining the default DRBG context and Initializing the context, the DRBG Module need to be Instantiated
* for the DRBG operations. This function instantiates a DRBG with the supplied personalization string pers.
*
* @param[in] dctx (Data Input) Pointer to DRBG Context obtained from IPSI_CRYPT_get_default_drbg API  [N/A]
* @param[in] pers (Data Input) The Personalization String  [N/A]
* @param[in] perslen (Data Input) The Personalization String Length [N/A]
*
* @retval SEC_UINT32 (Status Output)  On API Invocation is Success [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 (Status Output)  Invalid argument passed [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 (Status Output)  Personalization string is of invalid length
* [SEC_CRYPT_ERR_PERSONALISATION_STRING_TOO_LONG|N/A]
* @retval SEC_UINT32 (Status Output)  DRBG not initialized [SEC_CRYPT_ERR_DRBG_NOT_INITIALISED|N/A]
* @retval SEC_UINT32 (Status Output)  During DRBG instatiate, DRBG already found in error state
* [SEC_CRYPT_ERR_DRBG_INSTANTIATE_IN_ERROR_STATE|N/A]
* @retval SEC_UINT32 (Status Output)  During DRBG instatiate, DRBG already found to be instantiated
* [SEC_CRYPT_ERR_DRBG_ALREADY_INSTANTIATED|N/A]
* @retval SEC_UINT32 (Status Output)  Error retrieving entropy during DRBG Instantiate
* [SEC_CRYPT_ERR_DRBG_INSTANTIATE_RETRIEVING_ENTROPY|N/A]
* @retval SEC_UINT32 (Status Output)  Error retrieving nonce during DRBG Instantiate
* [SEC_CRYPT_ERR_RETRIEVING_NONCE|N/A]
* @retval SEC_UINT32 (Status Output)  DRBG mechanism Instantiation error[SEC_CRYPT_ERR_DRBG_INSTANTIATION|N/A]

* @retval SEC_UINT32 (Status Output) API invocation is Failure [SEC_ERR|N/A]
*
* @par Required Header File
* ipsi_crypto_drbg_ext.h
*
* @par Note
* \n
* N/A
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 IPSI_CRYPT_drbg_instantiate(IPSI_DRBG_CTX *dctx, const SEC_UCHAR *pers, size_t perslen);

/*
    Func Name:  IPSI_CRYPT_drbg_set_callbacks
*/
/**
 * @defgroup IPSI_CRYPT_drbg_set_callbacks
 * @ingroup Testability
 * @par Prototype
 * @code
 * SEC_UINT32 IPSI_CRYPT_drbg_set_callbacks(IPSI_DRBG_CTX *dctx,
 * size_t (*get_entropy)(IPSI_DRBG_CTX *ctx, SEC_UCHAR **pout,
 * SEC_INT entropy, size_t min_len, size_t max_len),
 * SEC_VOID (*cleanup_entropy)(IPSI_DRBG_CTX *ctx, SEC_UCHAR *out, size_t olen),
 * size_t entropy_blocklen,
 * size_t (*get_nonce)(IPSI_DRBG_CTX *ctx, SEC_UCHAR **pout,
 * SEC_INT entropy, size_t min_len, size_t max_len),
 * SEC_VOID (*cleanup_nonce)(IPSI_DRBG_CTX *ctx, SEC_UCHAR *out, size_t olen));
 * @endcode
 *
 * @par Purpose
 * This function is used to Set the DRBG Functionality Callbacks.
 *
 * @par Description
 * For the DRBG Module, many input are needed from the application mainly the Entropy Input and Nonce. In order to
 * obtain these input from the user, the callbacks are called by the DRBG module, Hence this API is used to set
 * these application callbacks in the DRBG module.
 * The IPSI_DRBG_CTX must be in an uninstantiated state to set the callbacks: i.e. the callbacks cannot be set
 * on an instantiated DRBG.
 *
 * @param[in] dctx (Data Input) Pointer to DRBG Context obtained from IPSI_CRYPT_get_default_drbg API  [N/A]
 * @param[in] (*get_entropy) (Data Input) Pointer to the Get Entropy Callback.
 * The callbacks get_entropy request "entropy" bits of entropy in a buffer of between min_len and max_len bytes.
 * The function should set *pout to the buffer containing the entropy and return the length in bytes of the buffer.
 * If the source of entropy is unable to satisfy the request it MUST return zero. This will place the DRBG in an error
 * condition due to the entropy source failure. [N/A]
 * @param[in] (*cleanup_entropy)  (Data Input) Pointer to the Cleanup Entropy Callback
 * The callback cleanup_entropy are called after the entropy buffers have been used and can be utilized to zeroize
 * the buffers. The "out" and "olen" parameters contains the same value returned by the get_entropy function. [N/A]
 * @param[in] entropy_blocklen  (Data Input) This is used to specify the block length of the underlying entropy source.
 * This is used for the continuous RNG test on the entropy source. The block length must be greater than one
 * and maximum value can be upto 2^16 (i.e., 65536 bytes). Trying to set block length as more than 2^16 will
 * lead to invalid argument failure[N/A]
 * @param[in] (*get_nonce)  (Data Input) Pointer to the Get NonceCallback
 * The callbacks get_nonce request "entropy" bits of entropy in a buffer of between min_len and max_len bytes.
 * The function should set *pout to the buffer containing the entropy and return the length in bytes of the buffer.
 * If the source of entropy is unable to satisfy the request it MUST return zero. This will place the DRBG in an error
 * condition due to the entropy source failure. [N/A]
 * @param[in] (*cleanup_nonce)  (Data Input) Pointer to the Cleanup NonceCallback
 * The callback cleanup_nonce are called after the entropy buffers have been used and can be utilized to zeroize
 * the buffers. The "out" and "olen" parameters contains the same value returned by the get_nonce function. [N/A]
 *
 * @retval SEC_UINT32 (Status Output)  On API Invocation is Success [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 (Status Output)  Invalid argument passed [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 (Status Output) DRBG not in Un-initialized state [SEC_ERR|N/A]
 *
 * @par Required Header File
 * ipsi_crypto_drbg_ext.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 IPSI_CRYPT_drbg_set_callbacks(IPSI_DRBG_CTX *dctx,
    size_t (*get_entropy)(IPSI_DRBG_CTX *ctx, SEC_UCHAR **pout, SEC_INT entropy, size_t min_len, size_t max_len),
    SEC_VOID (*cleanup_entropy)(IPSI_DRBG_CTX *ctx, SEC_UCHAR *out, size_t olen), size_t entropy_blocklen,
    size_t (*get_nonce)(IPSI_DRBG_CTX *ctx, SEC_UCHAR **pout, SEC_INT entropy, size_t min_len, size_t max_len),
    SEC_VOID (*cleanup_nonce)(IPSI_DRBG_CTX *ctx, SEC_UCHAR *out, size_t olen));

/*
    Func Name:  IPSI_CRYPT_drbg_set_rand_callbacks
*/
/**
 * @defgroup IPSI_CRYPT_drbg_set_rand_callbacks
 * @ingroup Testability
 * @par Prototype
 * @code
 * SEC_UINT32 IPSI_CRYPT_drbg_set_rand_callbacks(IPSI_DRBG_CTX *dctx,
 * size_t (*get_adin)(IPSI_DRBG_CTX *ctx, SEC_UCHAR **pout),
 * SEC_VOID (*cleanup_adin)(IPSI_DRBG_CTX *ctx, SEC_UCHAR *out, size_t olen),
 * SEC_INT (*rand_seed_cb)(IPSI_DRBG_CTX *ctx, const SEC_VOID *buff, SEC_INT num),
 * SEC_INT (*rand_add_cb)(IPSI_DRBG_CTX *ctx,
 * const SEC_VOID *buff, SEC_INT num, double entropy));
 * @endcode
 *
 * @par Purpose
 * This function is used to Set the RAND Functionality Callbacks.
 *
 * @par Description
 * For the DRBG-RAND Module, many input are needed from the application mainly the getting AdditionalInput,
 * Rand Seed and Rand Additional CB. In order to obtain these input from the user, the callbacks are called
 * by the RAND module, Hence this API is used to set these application callbacks in the RAND module.
 *
 * @param[in] dctx  (Data Input) Pointer to DRBG Context obtained from IPSI_CRYPT_get_default_drbg API  [N/A]
 * @param[in] (*get_adin)  (Data Input) Pointer to the Get Additional Input Callback
 * The get_adin callback is used to retrieve optional additional data used whenever a request for
 * random data is made using IPSI_CRYPT_rand_bytes(). When this operation is complete cleanup_adin
 * is called to release the buffer.
 * Sets "additional input" when generating random data. This could be for ex the current PID, a
 * time value and a counter. [N/A]
 * @param[in] (*cleanup_adin)  (Data Input) Pointer to the Cleanup Additional Input Callback[N/A]
 * @param[in] (*rand_seed_cb) (Data Input) Pointer to the Get Rand Seed Callback
 * The callback rand_seed_cb is called directly whenever IPSI_CRYPT_rand_seed() is called. This is
 * entirely application defined and could be used for example to add seed information to the entropy source.
 * If the operation is success it must return 1.[N/A]
 * @param[in] (*rand_add_cb) (Data Input) Pointer to the Additional Rand Input Callback
 * The callback rand_add_cb is called directly whenever IPSI_CRYPT_rand_add() is called. This is
 * entirely application defined and could be used for example to add seed information to the entropy source.
 * If the operation is success it must return 1.[N/A]
 *
 * @retval SEC_UINT32 (Status Output)  On API Invocation is Success [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 (Status Output)  Invalid argument passed [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 (Status Output) DRBG not in Un-initialized state [SEC_ERR|N/A]
 *
 * @par Required Header File
 * ipsi_crypto_drbg_ext.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 IPSI_CRYPT_drbg_set_rand_callbacks(IPSI_DRBG_CTX *dctx,
    size_t (*get_adin)(IPSI_DRBG_CTX *ctx, SEC_UCHAR **pout),
    SEC_VOID (*cleanup_adin)(IPSI_DRBG_CTX *ctx, SEC_UCHAR *out, size_t olen),
    SEC_INT (*rand_seed_cb)(IPSI_DRBG_CTX *ctx, const SEC_VOID *buff, SEC_INT num),
    SEC_INT (*rand_add_cb)(IPSI_DRBG_CTX *ctx, const SEC_VOID *buff, SEC_INT num, double entropy));

/*
    Func Name:  IPSI_CRYPT_drbg_uninstantiate
*/
/**
 * @defgroup IPSI_CRYPT_drbg_uninstantiate
 * @ingroup Testability
 * @par Prototype
 * @code
 * SEC_UINT32 IPSI_CRYPT_drbg_uninstantiate(IPSI_DRBG_CTX *dctx)
 * @endcode
 *
 * @par Purpose
 * This function is used to UnInstantiate the DRBG Module
 *
 * @par Description
 * After obtaining the default DRBG context and Initializing the context, The DRBG Module need to be Instantiated
 * for the DRBG operations. This function instantiate the DRBG module so that DRBG random bytes can be obtained from
 * DRBG context handle. Further after the usage of DRBG context for obtaining DRBG random bytes, the DRBG module
 * can be UnInstantiated. This function is used for uninstantiating the DRBG context
 *
 * @param[in] dctx (Data Input) Pointer to DRBG Context obtained from IPSI_CRYPT_get_default_drbg API  [N/A]
 *
 * @retval SEC_UINT32 (Status Output)  On API Invocation is Success [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 (Status Output)  Invalid argument passed [SEC_ERR_INVALID_ARG|N/A]
 *
 * @par Required Header File
 * ipsi_crypto_drbg_ext.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 IPSI_CRYPT_drbg_uninstantiate(IPSI_DRBG_CTX *dctx);

/*
    Func Name:  IPSI_CRYPT_drbg_health_check
*/
/**
 * @defgroup IPSI_CRYPT_drbg_health_check
 * @ingroup Testability
 * @par Prototype
 * @code
 * SEC_UINT32 IPSI_CRYPT_drbg_health_check(IPSI_DRBG_CTX *dctx)
 * @endcode
 *
 * @par Purpose
 * This function initiates a health check on the global default DRBG Module
 *
 * @par Description
 *
 * This function initiates a health check on the global default DRBG Module. In addition health checks are also
 * performed when a DRBG is first initiated when a DRBG is reseeded and every health_check_interval calls to the
 * generate function. A health check is performed on a test instantiation using the same mechanism and parameters.
 * After instantiation of the global default DRBG, the DRBG Module can be tested by the application for the
 * "Health Test". This test runs standard Known Answer Tests and checks for the same mechanism of global DRBG module.
 *
 * @param[in] dctx (Data Input) Pointer to DRBG Context obtained from IPSI_CRYPT_get_default_drbg API  [N/A]
 *
 * @retval SEC_UINT32 (Status Output)  On API Invocation is Success [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 (Status Output)  Invalid argument passed [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 (Status Output) API invocation is Failure [SEC_ERR|N/A]
 *
 * @par Required Header File
 * ipsi_crypto_drbg_ext.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 IPSI_CRYPT_drbg_health_check(IPSI_DRBG_CTX *dctx);

/*
    Func Name:  IPSI_CRYPT_drbg_set_check_interval
*/
/**
* @defgroup IPSI_CRYPT_drbg_set_check_interval
* @ingroup Testability
* @par Prototype
* @code
*  SEC_VOID IPSI_CRYPT_drbg_set_check_interval(IPSI_DRBG_CTX *dctx, SEC_INT interval)
* @endcode
*
* @par Purpose
* This function is used to Set the Interval for Running the Health Test in the DRBG Module
*
* @par Description
* Health checks are performed when a DRBG is first initiated and every health_check_interval calls to the
* generate function. This interval is by default 2^24 but can be modified by using this function.

* @param[in] dctx (Data Input) Pointer to DRBG Context obtained from IPSI_CRYPT_get_default_drbg API  [N/A]
* @param[in] interval (Data Input) The Interval of the health tests [N/A]
*
* @retval SEC_VOID This function does not return any value [N/A|N/A]
*
* @par Required Header File
* ipsi_crypto_drbg_ext.h
*
* @par Note
* \n
* N/A
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_VOID IPSI_CRYPT_drbg_set_check_interval(IPSI_DRBG_CTX *dctx, SEC_INT interval);

/*
    Func Name:  IPSI_CRYPT_drbg_set_reseed_interval
*/
/**
 * @defgroup IPSI_CRYPT_drbg_set_reseed_interval
 * @ingroup Testability
 * @par Prototype
 * @code
 * SEC_VOID IPSI_CRYPT_drbg_set_reseed_interval(IPSI_DRBG_CTX *dctx, SEC_UINT interval)
 * @endcode
 *
 * @par Purpose
 * This function is used to Set the Interval for reseed in the DRBG Module
 *
 * @par Description
 * This function modifies the reseed interval value. The default is 2^24 generate operations. These values
 * are lower than the maximums specified in SP800-90.
 *
 * @param[in] dctx (Data Input) Pointer to DRBG Context [N/A]
 * @param[in] interval (Data Input) The Interval of the reseed [N/A]
 *
 * @retval SEC_VOID This function does not return any value [N/A|N/A]
 *
 * @par Required Header File
 * ipsi_crypto_drbg_ext.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_VOID IPSI_CRYPT_drbg_set_reseed_interval(IPSI_DRBG_CTX *dctx, SEC_UINT interval);

/*
    Func Name:  IPSI_CRYPT_drbg_set_app_data
*/
/**
 * @defgroup IPSI_CRYPT_drbg_set_app_data
 * @ingroup Testability
 * @par Prototype
 * @code
 * SEC_VOID IPSI_CRYPT_drbg_set_app_data(IPSI_DRBG_CTX *dctx, SEC_VOID *app_data)
 * @endcode
 *
 * @par Purpose
 * This function is used to Set the Application Specific Data in the DRBG Module.
 *
 * @par Description
 * Set an application defined pointer value. The meaning of this pointer is application defined and might for
 * example contain a pointer to a handle representing the entropy source and the get_entropy function
 * could retrieve and make use of that pointer.
 *
 * @param[in] dctx (Data Input) Pointer to DRBG Context obtained from IPSI_CRYPT_get_default_drbg API  [N/A]
 * @param[in] app_data (Data Input) The Application Data Pointer [N/A]
 *
 * @retval SEC_VOID This function does not return any value [N/A|N/A]
 *
 * @par Required Header File
 * ipsi_crypto_drbg_ext.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_VOID IPSI_CRYPT_drbg_set_app_data(IPSI_DRBG_CTX *dctx, SEC_VOID *app_data);

/*
Func Name:  IPSI_CRYPT_drbg_stick
*/
/**
 * @defgroup IPSI_CRYPT_drbg_stick
 * @ingroup Testability
 * @par Prototype
 * @code
 * SEC_VOID IPSI_CRYPT_drbg_stick(SEC_INT onoff)
 * @endcode
 *
 * @par Purpose
 * This function is called to control the behaviour of next DRBG generate operation
 *
 * @par Description
 * IPSI_CRYPT_drbg_stick is called to control the behaviour of next DRBG generate operation.
 * If IPSI_CRYPT_drbg_stick is called with onoff equal to 1, then the X9.31 PRNG output is copied to the
 * stored last block to ensure the test will fail on the next generate operation.
 *
 * @param[in] onOff (Data Input) Control the DRBG generation behaviour. 1 - Fail generation. 0 - Do not fail [N/A]
 *
 * @retval SEC_VOID This function does not return any value [N/A|N/A]
 *
 * @par Required Header File
 * ipsi_crypto_drbg_ext.h
 *
 * @par Note
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_VOID IPSI_CRYPT_drbg_stick(SEC_INT onoff);

/*
Func Name:  IPSI_CRYPT_signDsaSetKVal_test
*/
/**
* @defgroup IPSI_CRYPT_signDsaSetKVal_test
* @ingroup Testability
* @par Prototype
* @code
* SEC_UINT32 IPSI_CRYPT_signDsaSetKVal_test(CRYPT_CTX *pCtx,const SEC_BIGINT_S *pstKVal)
* @endcode
*
* @par Purpose
* This function is used to set the random number (k) value used while generating the Signature.
*
* @par Description
* IPSI_CRYPT_signDsaSetKVal_test is used to set 'k' value used while generating the signature
* so that user can chose own Random Number rather than generating internally.
* No validation is done on Random Number or its length. This API should be used for test purpose only.
* This API can be used as a testability API to verify the Signature generation as per FIPS standard.
* This API should be used only for DSA algorithm
*
* @param[in] ctx (Data Input) The context for signing [N/A]
* @param[in] pstKVal (Data Input) pointer to the Random Number (k) as per test vector [N/A]
*
* @retval SEC_UINT32 (Status Output) On Success [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 (Status Output) Invalid Arguments [SEC_ERR_INVALID_ARG|N/A]
*
* @par Required Header File
* ipsi_crypto_drbg_ext.h
*
* @par Note
* \n
* -# CRYPT_signInit should be called before calling IPSI_CRYPT_signDsaSetKVal_test.
*      IPSI_CRYPT_signDsaSetKVal_test API call should be followed with calls to
*      CRYPT_signUpdate and CRYPT_signFinal
* -# Application should pass the context object as first parameter. Example:
        CRYPT_CTX ctx;
        CRYPT_signInit (&ctx, pKey, uiHashId);
        CRYPT_signDsaSetKVal_test(ctx,pstKVal);
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 IPSI_CRYPT_signDsaSetKVal_test(CRYPT_CTX *pCtx, const SEC_BIGINT_S *pstKVal);

/*
Func Name:  IPSI_CRYPT_signEcdsaSetKVal_test
*/
/**
* @defgroup IPSI_CRYPT_signEcdsaSetKVal_test
* @ingroup Testability
* @par Prototype
* @code
* SEC_UINT32 IPSI_CRYPT_signEcdsaSetKVal_test(CRYPT_CTX *pCtx, const SEC_BIGINT_S *pstKVal)
* @endcode
*
* @par Purpose
* This function is used to set the random number (k) value used while generating the Signature.
*
* @par Description
* IPSI_CRYPT_signEcdsaSetKVal_test is used to set 'k' value used while generating the signature
* so that user can chose own Random Number rather than generating internally.
* No validation is done on Random Number or its length. This API should be used for test purpose only.
* This API can be used as a testability API to verify the Signature generation as per FIPS standard.
* This API should be used only for ECDSA algorithm.
*
* @param[in] ctx (Data Input) The context for signing [N/A]
* @param[in] pstKVal (Data Input) pointer to the Random Number (k) as per test vector [N/A]
*
* @retval SEC_UINT32 (Status Output) On Success [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 (Status Output) Invalid Arguments [SEC_ERR_INVALID_ARG|N/A]
*
* @par Required Header File
* ipsi_crypto_drbg_ext.h
*
* @par Note
* \n
* -# CRYPT_signInit should be called before calling IPSI_CRYPT_signEcdsaSetKVal_test.
*      IPSI_CRYPT_signEcdsaSetKVal_test API call should be followed with calls to
*      CRYPT_signUpdate and CRYPT_signFinal
* -# Application should pass the context object as first parameter. Example:
        CRYPT_CTX ctx;
        CRYPT_signInit (&ctx, pKey, uiHashId);
        CRYPT_signEcdsaSetKVal_test(ctx,pstKVal);
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 IPSI_CRYPT_signEcdsaSetKVal_test(CRYPT_CTX *pCtx, const SEC_BIGINT_S *pstKVal);

/*
Func Name:  IPSI_CRYPT_genKeyPairByPrvKeySet_test
*/
/**
 * @defgroup IPSI_CRYPT_genKeyPairByPrvKeySet_test
 * @ingroup Testability
 * @par Prototype
 * @code
 * SEC_UINT32 IPSI_CRYPT_genKeyPairByPrvKeySet_test(const ALG_PARA_S *pAlgInfo,SEC_PKEY_S *pKeyPair,
 * const SEC_BIGINT_S *pstPrvKey)
 * @endcode
 *
 * @par Purpose
 * This function is used to perform the Key Pair Generation test by explicitly passing the
 * Private Key as per FIPS Test vector. This API should be used for test purpose only.
 *
 * @par Description
 * IPSI_CRYPT_genKeyPairByPrvKeySet_test will generate Key pair by using the private key provided.
 * This API will generate the Public Key using the Private Key provided. This API can be used as a
 * testability API to verify the Key Pair generation as per FIPS standard.
 * This API supports only ECDSA and DSA algorithms.
 *
 * @param[in] algInfo (Control Input) Algorithm information structure [N/A]
 * @param[in] pstPrvKey (Data Input) Pointer to private key value from test vector [N/A]
 * @param[out] pKeyPair (Data Output) The generated key pair structure [N/A]
 *
 * @retval SEC_UINT32 (Status Output) On Success [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 (Status Output) Invalid Arguments [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 (Status Output) If memory allocation failure [SEC_ERR_MALLOC_FAIL|N/A]
 * @retval SEC_UINT32 (Status Output) If key size not supported [SEC_ERR_INVALID_KEY_LEN|N/A]
 * @retval SEC_UINT32 (Status Output) If library is not initialized [SEC_ERR_INITLIB|N/A]
 * @retval SEC_UINT32 (Status Output) Incorrect Algorithm ID [SEC_ERR_INVALID_ALGID|N/A]
 * @retval SEC_UINT32 (Status Output) On Failure [SEC_ERR_KEY_GEN_FAILED|N/A]
 * @retval SEC_UINT32 (Status Output) On other errors [SEC_ERR|N/A]
 *
 * @par Required Header File
 * ipsi_crypto_drbg_ext.h
 *
 * @par Note
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 IPSI_CRYPT_genKeyPairByPrvKeySet_test(const ALG_PARA_S *pAlgInfo, SEC_PKEY_S *pKeyPair,
    const SEC_BIGINT_S *pstPrvKey);

#ifdef __cplusplus
}
#endif /* end of __cplusplus */
#endif /* end of _IPSI_DRBG_CRYPTO_EXT_H_ */
