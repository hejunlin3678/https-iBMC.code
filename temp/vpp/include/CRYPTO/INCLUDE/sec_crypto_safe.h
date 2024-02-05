/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: Contains various crypto safe interface functions
 * ALL RIGHTS RESERVED
 * Project Code: iPSI
 * Module Name: sec_crypto_safe.h
 * Create: 2017-05-16
 * Author: Prabhat Kumar Gopalika
 * Descrption: Contains various crypto safe interface functions
 * ----------------------------------------------------------------------
 * Modification History
 * DATE        NAME             DESCRIPTION
 * --------------------------------------------------------------------
 */
#ifndef SEC_CRYPTO_SAFE_H
#define SEC_CRYPTO_SAFE_H

#include "ipsi_crypto_buildconf.h"
#include "sec_sys.h"
#include "sec_crypto.h"
#include "sec_util.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* Func Name:  CRYPT_openUpdate_sf
* @defgroup CRYPT_openUpdate_sf
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_openUpdate_sf(
* CRYPT_CTX ctx,
* const SEC_UCHAR* pucCipherText,
* SEC_UINT32 pulCLen,
* SEC_UCHAR* pucPlainText,
* SEC_UINT32* pulPLen);
* @endcode
*
* @par Purpose
* This API uses the initialized context to decrypt ciphertext
into plaintext for symmetric decryption.
*
* @par Description
* CRYPT_openUpdate_sf function uses the initialized context to
decrypt ciphertext into plaintext for symmetric decryption.
* Please refer to CRYPT_decryptUpdate function.
*
* @param[in] ctx Context to be used [N/A]
* @param[in] pucCipherText Encrypted ciphertext [N/A]
* @param[in] pulCLen length of Encrypted ciphertext [N/A]
* @param[out] pucPlainText Decrypted plaintext [N/A]
* @param[in] pulPLen Length of input buffer (pucPlainText). It should be more than or equal to pulCLen [N/A]
* @param[out] pulPLen Length of decrypted plaintext [N/A]
*
* @retval SEC_UINT32 Invalid context [SEC_CRYPT_ERR_INVALID_CTX|N/A]
* @retval SEC_UINT32 Invalid Argument [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 Invalid IV length [SEC_ERR_INVALID_IV_LEN|N/A]
* @retval SEC_UINT32 On successful return [SEC_SUCCESS|N/A]
*
* @par Required Header File
* sec_crypto_safe.h
*
* @par Note
* \n
* N/A
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_openUpdate_sf(CRYPT_CTX ctx, const SEC_UCHAR *pucCipherText, SEC_UINT32 pulCLen,
    SEC_UCHAR *pucPlainText, SEC_UINT32 *pulPLen);

/*
    Func Name:  CRYPT_openFinal_sf
*/
/**
* @defgroup CRYPT_openFinal_sf
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_openFinal_sf(
* CRYPT_CTX* pCtx,
* SEC_UCHAR* pucPlainText,
* SEC_UINT32* pulPLen)
* @endcode
*
* @par Purpose
* This is used to complete open operation.

* @par Description
* CRYPT_openFinal_sf function completes one open operation.
It frees the symmetric context. In case of block ciphers, the
* output of this function is the last block. If openinit succeeds,
openfinal must be called to free the context.
*
* @param[in] pCtx Address of Context to be used [N/A]
* @param[out] pucPlainText Final decrypted plaintext [N/A]
* @param[in] pulPLen Length of input buffer (pucPlainText). It should be more than 128 bytes [N/A]
* @param[out] pulPLen Length of final decrypted plaintext [N/A]
*
* @retval SEC_UINT32 Invalid context [SEC_CRYPT_ERR_INVALID_CTX|N/A]
* @retval SEC_UINT32 Invalid pad mode in context [SEC_CRYPT_ERR_UNDEFINED_PADDING|N/A]
* @retval SEC_UINT32 Invalid arguments [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 When pad bits are incorrect that is after decrypting,
if the padding bits dont match the pad mode.
* It can be due to incorrect Key or IV as that will produce junk
plaintext with random padding bits, which do not
* match the padding mode [SEC_CRYPT_ERR_INVALID_PADDING|N/A]
* @retval SEC_UINT32 Invalid ciphertext length given for decryption
in update.
* (ciphertext length was not integral no. of blocks)
[SEC_ERR_INVALID_DATA_LEN|N/A]
* @retval SEC_UINT32 On successful return [SEC_SUCCESS|N/A]
*
* @par Required Header File
* sec_crypto_safe.h
*
* @par Note
* \n
* N/A
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_openFinal_sf(CRYPT_CTX *pCtx, SEC_UCHAR *pucPlainText, SEC_UINT32 *pulPLen);

/*
    Func Name:  CRYPT_open_sf
*/
/**
* @defgroup CRYPT_open_sf
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_open_sf(
* SEC_UINT32 ulAlgId,
* SEC_PKEY_S* pstPvtKey,
* const SEC_UCHAR* pucCEKey,
* SEC_UINT32 ulCEKeyLen,
* const SEC_UCHAR* pucIV,
* SEC_UINT32 ulIVLen,
* SEC_UCHAR* pucCipherText,
* SEC_UINT32 pulCLen,
* SEC_UCHAR* pucPlainText,
* SEC_UINT32* pulPLen,
* SEC_UINT32 ulPadding);
* @endcode
*
* @par Purpose
* This is used to decrypt the encrypted symmetric key.
It then uses the decrypted symmetric key to decrypt the ciphertext
* into plaintext using symmetric decryption.
*
* @par Description
* CRYPT_open_sf function decrypts the encrypted symmetric key.
It then uses the decrypted symmetric key to decrypt the
* ciphertext into plaintext using symmetric decryption.
*
* @param[in] ulAlgId Algorithm ID [N/A]
* @param[in] pstPvtKey Private key of asymmetric algorithm [N/A]
* @param[in] pucCEKey Encrypted symmetric key [N/A]
* @param[in] ulCEKeyLen Length of encrypted symmetric key [N/A]
* @param[in] pucIV InitializationVector(CBC,CFB,OFB modes) [N/A]
* @param[in] ulIVLen Length of IV. pass 0 if no IV [N/A]
* @param[in] pucCipherText Ciphertext to be decrypted [N/A]
* @param[in] pulCLen Length of  ciphertext [N/A]
* @param[out] pucPlainText Decrypted Plaintext [N/A]
* @param[in] pulPLen Length of input buffer (pucPlainText). It should be more than or equal to pulCLen [N/A]
* @param[out] pulPLen Length of plaintext [N/A]
* @param[in] ulPadding Padding mode used for private key decryption [N/A]
*
* @retval SEC_UINT32 If Memory allocation fails [SEC_ERR_MALLOC_FAIL|N/A]
* @retval SEC_UINT32 If invalid Algorithm Id [SEC_ERR_INVALID_ALGID|N/A]
* @retval SEC_UINT32 Invalid Padding Mode. Padding mode set on Algorithm is not defined
* [SEC_CRYPT_ERR_UNDEFINED_PADDING|N/A]
* @retval SEC_UINT32 The pad bits are incorrect. When after
decrypting the padding bits dont match the pad mode. It can
* be due to incorrect Key or IV as that will produce junk
plaintext with random padding bits, which do not match the
* padding mode [SEC_CRYPT_ERR_INVALID_PADDING|N/A]
* @retval SEC_UINT32 Invalid ciphertext length given for
decryption (ciphertext length was not integral number of blocks)
* [SEC_ERR_INVALID_DATA_LEN|N/A]
* @retval SEC_UINT32 Invalid Keylength for algorithm [SEC_ERR_INVALID_KEY_LEN|N/A]
* @retval SEC_UINT32 Invalid Argument [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 Invalid IV length [SEC_ERR_INVALID_IV_LEN|N/A]
* @retval SEC_UINT32 On successful return [SEC_SUCCESS|N/A]
*
* @par Required Header File
* sec_crypto_safe.h
*
* @par Note
* - API supports following unsafe algorithms:
*   - ALGID_RC4 (CID_RC4)
*   - ALGID_DES_ECB (CID_DES_ECB)
*   - ALGID_DES_CBC (CID_DES_CBC)
*   - ALGID_DES_OFB (CID_DES_OFB)
*   - ALGID_DES_CFB (CID_DES_CFB)
*   - ALGID_DES_EDE_ECB (CID_DES_EDE_ECB)
*   - ALGID_DES_EDE_CBC (CID_DES_EDE_CBC)
*   - ALGID_DES_EDE_OFB (CID_DES_EDE_OFB)
*   - ALGID_DES_EDE_CFB (CID_DES_EDE_CFB)
*   - ALGID_DES_EDE3_ECB (CID_DES_EDE3_ECB)
*   - ALGID_DES_EDE3_CBC (CID_DES_EDE3_CBC)
*   - ALGID_DES_EDE3_OFB (CID_DES_EDE3_OFB)
*   - ALGID_DES_EDE3_CFB (CID_DES_EDE3_CFB)
*   - ALGID_KASUMI_ECB (CID_KASUMI_ECB)
*   - ALGID_KASUMI_CBC (CID_KASUMI_CBC)
*   - ALGID_KASUMI_OFB (CID_KASUMI_OFB)
*   - ALGID_KASUMI_CFB (CID_KASUMI_CFB)
*   - ALGID_RC2_ECB (CID_RC2_ECB)
*   - ALGID_RC2_CBC (CID_RC2_CBC)
*   - ALGID_RC2_OFB (CID_RC2_OFB)
*   - ALGID_RC2_CFB (CID_RC2_CFB) \n
*   As these algorithms are unsafe, if application are using these algorithms, application must take care of any
*   security issues that may happen due to usage of these algorithms.
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_open_sf(SEC_UINT32 ulAlgId, SEC_PKEY_S *pstPvtKey, const SEC_UCHAR *pucCEKey,
    SEC_UINT32 ulCEKeyLen, const SEC_UCHAR *pucIV, SEC_UINT32 ulIVLen, const SEC_UCHAR *pucCipherText,
    SEC_UINT32 pulCLen, SEC_UCHAR *pucPlainText, SEC_UINT32 *pulPLen, SEC_UINT32 ulPadding);

/*
    Func Name:  CRYPT_sealUpdate_sf
*/
/**
* @defgroup CRYPT_sealUpdate_sf
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_sealUpdate_sf(
* CRYPT_CTX ctx,
* const SEC_UCHAR * pucPlainText,
* SEC_UINT32 ulPlainLen,
* SEC_UCHAR * pucCipherText,
* SEC_UINT32 * pulCLen);
* @endcode
*
* @par Purpose
* This uses the initialized context to encrypt plaintext to
get ciphertext using symmetric encryption.
*
* @par Description
* CRYPT_sealUpdate_sf function uses the initialized context to
encrypt plaintext to get ciphertext using symmetric encryption.
* Please refer to CRYPT_encryptUpadte function.
*
* @param[in] ctx The context to be used [N/A]
* @param[in] pucPlainText Plaintext to be encrypted [N/A]
* @param[in] ulPlainLen Length of plaintext [N/A]
* @param[out] pucCipherText Encrypted ciphertext [N/A]
* @param[in] pulCLen Length of input ciphertext buffer (pucCipherText). Length should be greater than or equal to
ulPlainLen  [N/A]
* @param[out] pulCLen Length of encrypted ciphertext [N/A]
*
* @retval SEC_UINT32 Invalid context [SEC_CRYPT_ERR_INVALID_CTX|N/A]
* @retval SEC_UINT32 On successful return [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 Invalid arguments [SEC_ERR_INVALID_ARG|N/A]
*
* @par Required Header File
* sec_crypto_safe.h
*
* @par Note
* \n
* N/A
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_sealUpdate_sf(CRYPT_CTX ctx, const SEC_UCHAR *pucPlainText, SEC_UINT32 ulPlainLen,
    SEC_UCHAR *pucCipherText, SEC_UINT32 *pulCLen);

/*
    Func Name:  CRYPT_sealFinal_sf
*/
/**
* @defgroup CRYPT_sealFinal_sf
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_sealFinal_sf(
* CRYPT_CTX* pCtx,
* SEC_UCHAR* pucCphrTxt,
* SEC_UINT32* pulCphrTxtLen)
* @endcode
*
* @par Purpose
* This is used to complete the seal operation.
*
* @par Description
* CRYPT_sealFinal_sf function completes the seal operation.
It frees the symmetric context. In case of block ciphers, the
* output of this function is the last block. If sealinit
succeeds, sealfinal must be called to free the context.
*
* @param[in] pCtx Address of Context to be used.
* @param[out] pucCphrTxt Final encrypted ciphertext.
* @param[in] pulCLen Length of input ciphertext buffer (pucCphrTxt). Length should be atleast 128 bytes more than the
ulPlainLen.
* @param[out] pulCphrTxtLen Length of the last encrypted ciphertext.
*
* @retval SEC_UINT32 Invalid context [SEC_CRYPT_ERR_INVALID_CTX].
* @retval SEC_UINT32 Invalid pad mode in context,
[SEC_CRYPT_ERR_UNDEFINED_PADDING].
* @retval SEC_UINT32 Invalid plaintext length given
* for encryption in update and padding_mode = BLOCK_PADDING_NONE.
* (plaintext length was not integral no. of blocks)
[SEC_ERR_INVALID_DATA_LEN|N/A]
* @retval SEC_UINT32 On successful return [SEC_UINT32 SEC_SUCCESS|N/A]
* @retval SEC_UINT32 Invalid arguments [SEC_ERR_INVALID_ARG|N/A]
*
* @par Required Header File
* sec_crypto_safe.h
*
* @par Note
* \n
* - If application does not set seeds and entropy on frequent intervals, it can lead to vulnerable keys or random
*   value. It is recommended to set seeds and entropy on frequent intervals or update the version that migrates the
*   random implementation based on DRBG.
*
* @par Related Topics
* N/A
*
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_sealFinal_sf(CRYPT_CTX *pCtx, SEC_UCHAR *pucCphrTxt, SEC_UINT32 *pulCphrTxtLen);

/*
    Func Name:  CRYPT_seal_sf
*/
/**
* @defgroup CRYPT_seal_sf
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_seal_sf(
* SEC_UINT32 ulAlgId,
* SEC_UINT32 uiKeyArrLen,
* SEC_PKEY_S* aPubKey[],
* const SEC_UCHAR* pucIV,
* SEC_UINT32 ulIVLen,
* const SEC_UCHAR* pucPlainText,
* SEC_UINT32 ulPlainLen,
* SEC_UCHAR* pucCipherText,
* SEC_UINT32* pulCLen,
* SEC_CHAR*  aCEKey[],
* SEC_UINT32 aCEKeyLen[],
* SEC_UINT32 ulPadding);
* @endcode
*
* @par Purpose
* This is used to generate a random key for symmetric algorithm.
*
* @par Description
* CRYPT_seal_sf function generates a random key for symmetric algorithm.
Then, it encrypts the symmetric key with each of
* the asymmetric keys passed in the aPubKey array.
The encrypted symmetric key is added to the aCEKey array and the length
* of the encrypted key is added to the aCEKeyLen array.
It then encrypts the plaintext using the generated of the
* symmetric key to obtain the ciphertext using symmetric encryption.
*
* @param[in] ulAlgId Algorithm ID
* @param[in] uiKeyArrLen Length of the three arrays (aPubKey,
aCEKey, and aCEKeyLen)
* @param[in] aPubKey[] Array of public keys of asymmetric algorithms
* @param[in] pucIV Initialization Vector(CBC,CFB,OFB modes)
* @param[in] ulIVLen Length of IV (0 if IV is not present)
* @param[in] pucPlainText Plaintext to be encrypted
* @param[in] ulPlainLen Length of plaintext
* @param[out] pucCipherText Encrypted ciphertext .
* @param[in] pulCLen Length of input ciphertext buffer (pucCipherText). Length should be atleast 128 bytes more than the
ulPlainLen
* @param[out] pulCLen Length of encrypted ciphertext
* @param[out] aCEKey[] Array of encrypted symmetric key.
This has to be freed by the user using ipsi_free()
* @param[out] aCEKeyLen[] Array of encrypted key length
* @param[in] ulPadding Padding mode used for public key encryption [N/A]
*
* @retval SEC_UINT32 On Success [SEC_SUCCESS]
* @retval SEC_UINT32 Invalid Arguments [SEC_ERR_INVALID_ARG]
* @retval SEC_UINT32 Memory allocation failure [SEC_ERR_MALLOC_FAIL]
* @retval SEC_UINT32 Incorrect Algorithm ID [SEC_ERR_INVALID_ALGID]
* @retval SEC_UINT32 Invalid Padding Mode. Padding mode set on
Algorithm is not defined
* [SEC_CRYPT_ERR_UNDEFINED_PADDING]
* @retval SEC_UINT32 Invalid IV length [SEC_ERR_INVALID_IV_LEN]
* @retval SEC_UINT32 Invalid plaintext length given for
encryption and padding_mode : BLOCK_PADDING_NONE.
* (plaintext length was not integral no. of blocks)
[SEC_ERR_INVALID_DATA_LEN]
*
* @par Required Header File
* sec_crypto_safe.h
*
* @par Note
* \n
* - As memory allocation for aCEKey is done inside the
library, user is responsible for freeing the memory using ipsi_free().
* - If application does not set seeds and entropy on frequent intervals, it can lead to vulnerable keys or random
*   value. It is recommended to set seeds and entropy on frequent intervals or update the version that migrates the
*   random implementation based on DRBG.
* - Before calling this API, please make sure that DRBG is initialized for random number generation by calling
*   IPSI_CRYPT_rand_init. If application don't want to use DRBG then appplication should call
*   IPSI_CRYPT_enable_drbg(0) to disable the DRBG Random feature.
* - DRBG is enabled by default.
* - API supports following unsafe algorithms:
*   - ALGID_RC4 (CID_RC4)
*   - ALGID_DES_ECB (CID_DES_ECB)
*   - ALGID_DES_CBC (CID_DES_CBC)
*   - ALGID_DES_OFB (CID_DES_OFB)
*   - ALGID_DES_CFB (CID_DES_CFB)
*   - ALGID_DES_EDE_ECB (CID_DES_EDE_ECB)
*   - ALGID_DES_EDE_CBC (CID_DES_EDE_CBC)
*   - ALGID_DES_EDE_OFB (CID_DES_EDE_OFB)
*   - ALGID_DES_EDE_CFB (CID_DES_EDE_CFB)
*   - ALGID_DES_EDE3_ECB (CID_DES_EDE3_ECB)
*   - ALGID_DES_EDE3_CBC (CID_DES_EDE3_CBC)
*   - ALGID_DES_EDE3_OFB (CID_DES_EDE3_OFB)
*   - ALGID_DES_EDE3_CFB (CID_DES_EDE3_CFB)
*   - ALGID_KASUMI_ECB (CID_KASUMI_ECB)
*   - ALGID_KASUMI_CBC (CID_KASUMI_CBC)
*   - ALGID_KASUMI_OFB (CID_KASUMI_OFB)
*   - ALGID_KASUMI_CFB (CID_KASUMI_CFB)
*   - ALGID_RC2_ECB (CID_RC2_ECB)
*   - ALGID_RC2_CBC (CID_RC2_CBC)
*   - ALGID_RC2_OFB (CID_RC2_OFB)
*   - ALGID_RC2_CFB (CID_RC2_CFB) \n
*   As these algorithms are unsafe, if application are using these algorithms, application must take care of any
*   security issues that may happen due to usage of these algorithms.
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_seal_sf(SEC_UINT32 ulAlgId, SEC_UINT32 uiKeyArrLen, SEC_PKEY_S *aPubKey[],
    const SEC_UCHAR *pucIV, SEC_UINT32 ulIVLen, const SEC_UCHAR *pucPlainText, SEC_UINT32 ulPlainLen,
    SEC_UCHAR *pucCipherText, SEC_UINT32 *pulCLen, SEC_CHAR *aCEKey[], SEC_UINT32 aCEKeyLen[], SEC_UINT32 ulPadding);

/*
    Func Name:  CRYPT_encryptUpdate_sf
*/
/**
* @defgroup CRYPT_encryptUpdate_sf
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_encryptUpdate_sf (
* CRYPT_CTX   ctx,
* const SEC_UCHAR*  pucPlainText,
* SEC_UINT32  ulPlainLen,
* SEC_UCHAR*  pucCipherText,
* SEC_UINT32* pulCLen);
* @endcode
*
* @par Purpose
* This is used to encrypt plain text to cipher text
using encryption algorithm.
*
* @par Description
* CRYPT_encryptUpdate_sf function uses the initialized
symmetric context to encrypt plaintext to get ciphertext. It uses
* symmetric encryption. If plain text length is less
than block size of encryption algorithm it carry forwards to
* CRYPT_encryptUpdate_sf. If plain text length is greater
than block size of encryption algorithm then multiple block size
* of plain text encrypts and rest carry forward to final block.
*
* @param[in] ctx The context to be used
* @param[in] pucPlainText Plaintext to be encrypted
* @param[in] ulPlainLen Length of plaintext
* @param[out] pucCipherText Encrypted ciphertext
* @param[in] pulCLen Length of the input pucCipherText buffer. The length should be equal or more than ulPlainLen.
* @param[out] pulCLen Length of encrypted ciphertext
*
* @retval SEC_UINT32 If invalid argument [SEC_ERR_INVALID_ARG]
* @retval SEC_UINT32 If invalid context [SEC_CRYPT_ERR_INVALID_CTX]
* @retval SEC_UINT32 On successful return [Success]
* @retval SEC_UINT32 On other error [SEC_ERR]
*
* @par Required Header File
* sec_crypto_safe.h
*
* @par Note
* \n
* N/A
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_encryptUpdate_sf(CRYPT_CTX ctx, const SEC_UCHAR *pucPlainText, SEC_UINT32 ulPlainLen,
    SEC_UCHAR *pucCipherText, SEC_UINT32 *pulCLen);

/*
    Func Name:  CRYPT_encryptFinal_sf
*/
/**
* @defgroup CRYPT_encryptFinal_sf
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_encryptFinal_sf (
* CRYPT_CTX*  pCtx,
* SEC_UCHAR*  pucCipherText,
* SEC_UINT32* pulCLen);
* @endcode
*
* @par Purpose
* This is used to encrypt only last block of plain text to cipher text using encryption algorithm.
*
* @par Description
* CRYPT_encryptFinal function completes a symmetric
encryption operation. It frees the symmetric context and outputs
* the last block in case of block ciphers.
If encryptinit succeeds, encryptfinal must be called to free the context.
*
* @param[in] pCtx Address of the context to be used
* @param[out] pucCipherText Last Encrypted ciphertext of the session
* @param[in] pulCLen It contain the length of the input buffer (pucCipherText)
* @param[out] pulCLen Length of last encrypted ciphertext
*
* @retval SEC_UINT32 If invalid argument [SEC_ERR_INVALID_ARG]
* @retval SEC_UINT32 If invalid context [SEC_CRYPT_ERR_INVALID_CTX]
* @retval SEC_UINT32 Invalid pad mode in context [SEC_CRYPT_ERR_INVALID_PADDING]
* @retval SEC_UINT32 Invalid plaintext length given for encryption in update and padding_mode = BLOCK_PADDING_NONE.
* plaintext length was not integral number of blocks) [SEC_ERR_INVALID_DATA_LEN]
* @retval SEC_UINT32 On successful return [SEC_SUCCESS]
* @retval SEC_UINT32 On other error [SEC_ERR]
*
* @par Required Header File
* sec_crypto_safe.h
*
* @par Note
* \n
* N/A
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_encryptFinal_sf(CRYPT_CTX *pCtx, SEC_UCHAR *pucCipherText, SEC_UINT32 *pulCLen);

/*
    Func Name:  CRYPT_encrypt_sf
*/
/**
 * @defgroup CRYPT_encrypt_sf
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_encrypt_sf (
 * SEC_UINT32 ulAlgId,
 * const SEC_UCHAR* pucKey,
 * SEC_UINT32 ulKeyLen,
 * const SEC_UCHAR* pucIV,
 * SEC_UINT32 ulIVLen,
 * const SEC_UCHAR* pucPlainText,
 * SEC_UINT32 ulPlainLen,
 * SEC_UCHAR* pucCipherText,
 * SEC_UINT32* pulCLen);
 * @endcode
 *
 * @par Purpose
 * This is used to encrypt plain text to cipher text using encryption algorithm.
 *
 * @par Description
 * CRYPT_encrypt function encrypts plain text to cipher text using encryption algorithm. It creates symmetric context by
 * creating algorithm object, padding object, opmode object. After encryption, symmetric context needs to be freed.
 *
 * @param[in] ulAlgId The algorithm Id
 * @param[in] pucKey The key value to be used
 * @param[in] ulKeyLen The length of the key
 * @param[in] pucIV InitializationVector(CBC,CFB,OFB algorithm modes)
 * @param[in] ulIVLen Length of IV. pass 0 if no IV
 * @param[in] pucPlainText Plaintext to be encrypted
 * @param[in] ulPlainLen Length of plaintext
 * @param[out] pucCipherText Encrypted ciphertext
 * @param[in] pulCLen Length of input ciphertext buffer. Length should be atleast 128 bytes more than the ulPlainLen
 * @param[out] pulCLen Length of encrypted ciphertext
 *
 * @retval SEC_UINT32 On Memory allocation failure [SEC_ERR_MALLOC_FAIL]
 * @retval SEC_UINT32 If invalid Algorithm Id [SEC_ERR_INVALID_ALGID]
 * @retval SEC_UINT32 If invalid Padding Mode. Padding mode set on Algorithm is not defined
 * [SEC_CRYPT_ERR_UNDEFINED_PADDING ]
 * @retval SEC_UINT32 If invalid Keylength for algorithm [SEC_ERR_INVALID_KEY_LEN]
 * @retval SEC_UINT32 If invalid IVlength for algorithm [SEC_ERR_INVALID_IV_LEN]
 * @retval SEC_UINT32 If invalid plaintext length given for encryption and padding_mode = BLOCK_PADDING_NONE.
 * (plaintext length was not integral no. of blocks) [SEC_ERR_INVALID_DATA_LEN|N/A]
 * @retval SEC_UINT32 On successful return [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 On other error [SEC_ERR|N/A]
 * @retval SEC_UINT32 Invalid arguments [SEC_ERR_INVALID_ARG|N/A]
 *
 * @par Required Header File
 * sec_crypto_safe.h
 *
 * @par Note
 * - When using 3DES, application need to take care that K1(first 64 bits), K2(middle 64 bits), and K3(last 64 bits)
 * are different from each other.
 * - API supports following unsafe algorithms:
 * - ALGID_RC4 (CID_RC4)
 * - ALGID_DES_ECB (CID_DES_ECB)
 * - ALGID_DES_CBC (CID_DES_CBC)
 * - ALGID_DES_OFB (CID_DES_OFB)
 * - ALGID_DES_CFB (CID_DES_CFB)
 * - ALGID_DES_EDE_ECB (CID_DES_EDE_ECB)
 * - ALGID_DES_EDE_CBC (CID_DES_EDE_CBC)
 * - ALGID_DES_EDE_OFB (CID_DES_EDE_OFB)
 * - ALGID_DES_EDE_CFB (CID_DES_EDE_CFB)
 * - ALGID_DES_EDE3_ECB (CID_DES_EDE3_ECB)
 * - ALGID_DES_EDE3_CBC (CID_DES_EDE3_CBC)
 * - ALGID_DES_EDE3_OFB (CID_DES_EDE3_OFB)
 * - ALGID_DES_EDE3_CFB (CID_DES_EDE3_CFB)
 * - ALGID_KASUMI_ECB (CID_KASUMI_ECB)
 * - ALGID_KASUMI_CBC (CID_KASUMI_CBC)
 * - ALGID_KASUMI_OFB (CID_KASUMI_OFB)
 * - ALGID_KASUMI_CFB (CID_KASUMI_CFB)
 * - ALGID_BF_ECB (CID_BF_ECB)
 * - ALGID_BF_CBC (CID_BF_CBC)
 * - ALGID_BF_CFB (CID_BF_CFB)
 * - ALGID_BF_OFB (CID_BF_OFB)
 * - ALGID_RC2_ECB (CID_RC2_ECB)
 * - ALGID_RC2_CBC (CID_RC2_CBC)
 * - ALGID_RC2_OFB (CID_RC2_OFB)
 * - ALGID_RC2_CFB (CID_RC2_CFB) \n
 * As these algorithms are unsafe, if application are using these algorithms, application must take care of any
 * security issues that may happen due to usage of these algorithms.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_encrypt_sf(SEC_UINT32 ulAlgId, const SEC_UCHAR *pucKey, SEC_UINT32 ulKeyLen,
    const SEC_UCHAR *pucIV, SEC_UINT32 ulIVLen, const SEC_UCHAR *pucPlainText, SEC_UINT32 ulPlainLen,
    SEC_UCHAR *pucCipherText, SEC_UINT32 *pulCLen);

/*
    Func Name:  CRYPT_decryptUpdate_sf
*/
/**
 * @defgroup CRYPT_decryptUpdate_sf
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_decryptUpdate_sf (
 * CRYPT_CTX   ctx,
 * const SEC_UCHAR*  pucCipherText,
 * SEC_UINT32  ulCLen,
 * SEC_UCHAR*  pucPlainText,
 * SEC_UINT32* pulPLen);
 * @endcode
 *
 * @par Purpose
 * This is used to decrypt cipher text to plain text using decryption algorithm.
 *
 * @par Description
 * CRYPT_decryptUpdate_sf function uses the initialized symmetric context to decrypt ciphertext to get plaintext.
 * It uses symmetric decryption. If cipher text length is less than or equal to block size of decryption
 * algorithm, it carry forwards to CRYPT_decryptFinal. If cipher text length is greater than block size of decryption
 * algorithm then multiple block size of cipher text decrypts and rest are carry forward to final block.
 *
 * @param[in] ctx The context to be used
 * @param[in] pucCipherText Encrypted ciphertext
 * @param[in] ulCLen Ciphertext Length
 * @param[out] pucPlainText Decrypted plaintext
 * @param[in] pulPLen Length of input pucPlainText buffer. The length should be same as Ciphertext Length (ulCLen)
 * @param[out] pulPLen Length of decrypted plaintext
 *
 * @retval SEC_UINT32 If invalid argument [SEC_ERR_INVALID_ARG]
 * @retval SEC_UINT32 If invalid context [SEC_CRYPT_ERR_INVALID_CTX]
 * @retval SEC_UINT32 On successful return [SEC_SUCCESS]
 * @retval SEC_UINT32 On other error [SEC_ERR]
 *
 * @par Required Header File
 * sec_crypto_safe.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_decryptUpdate_sf(CRYPT_CTX ctx, const SEC_UCHAR *pucCipherText, SEC_UINT32 ulCLen,
    SEC_UCHAR *pucPlainText, SEC_UINT32 *pulPLen);

/*
    Func Name:  CRYPT_decryptFinal_sf
*/
/**
 * @defgroup CRYPT_decryptFinal_sf
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_decryptFinal_sf (
 * CRYPT_CTX*  pCtx,
 * SEC_UCHAR*  pucPlainText,
 * SEC_UINT32* pulPLen);
 * @endcode
 *
 * @par Purpose
 * This is used to decrypt only last block of cipher text to plain text using decryption algorithm.
 *
 * @par Description
 * CRYPT_decryptFinal_sf function completes one symmetric decryption operation. It frees the context. In case of block
 * ciphers the remaining of the decrypted plaintext is the output. In stream ciphers, as all the plaintext would have
 * been output in decryptupdate, there will not be any plantext output. If decryptinit succeeds, decryptfinal must be
 * called to free the context.
 *
 * @param[in] pCtx Address of The context to be used
 * @param[out] pucPlainText Final decrypted plaintext
 * @param[in] pulPLen Length of the input buffer (pucPlainText). It should be minimum 128 byte long
 * @param[out] pulPLen Length of last decrypted plaintext
 *
 * @retval SEC_UINT32 If invalid argument [SEC_ERR_INVALID_ARG]
 * @retval SEC_UINT32 If invalid context [SEC_CRYPT_ERR_INVALID_CTX]
 * @retval SEC_UINT32 If invalid pad mode in context [SEC_CRYPT_ERR_UNDEFINED_PADDING]
 * @retval SEC_UINT32 The pad bits are incorrect. When after decrypting the padding bits dont match
 * the pad mode. It can be due to incorrect Key or IV as that will produce junk plaintext with random padding bits,
 * which dont match the padding mode. [SEC_CRYPT_ERR_INVALID_PADDING]
 * @retval SEC_UINT32 If invalid ciphertext length given for decryption. (ciphertext length was not integral no. of
 * blocks) [SEC_ERR_INVALID_DATA_LEN]
 * @retval SEC_UINT32 On successful return [SEC_SUCCESS]
 *
 * @par Required Header File
 * sec_crypto_safe.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_decryptFinal_sf(CRYPT_CTX *pCtx, SEC_UCHAR *pucPlainText, SEC_UINT32 *pulPLen);

/*
    Func Name:  CRYPT_decrypt_sf
*/
/**
* @defgroup CRYPT_decrypt_sf
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_decrypt_sf (
* SEC_UINT32       ulAlgId,
* const SEC_UCHAR* pucKey,
* SEC_UINT32       ulKeyLen,
* const SEC_UCHAR* pucIV,
* SEC_UINT32       ulIVLen,
* const SEC_UCHAR*       pucCipherText,
* SEC_UINT32       ulCLen,
* SEC_UCHAR*       pucPlainText,
* SEC_UINT32*      pulPLen);
* @endcode
*
* @par Purpose
* This is used to decrypt cipher text to plain text using decryption algorithm.
*
* @par Description
* CRYPT_decrypt_sf function decrypts cipher text to plain
text using decryption algorithm. It creates
symmetric context by
* creating algorithm object, padding object,
opmode object. After decryption, symmetric context needs to be freed.
*
* @param[in] ulAlgId The algorithm Id
* @param[in] pucKey The key value to be used
* @param[in] ulKeyLen The length of the key
* @param[in] pucIV InitializationVector(CBC FB,OFB modes)
* @param[in] pucCipherText To be decrypted ciphertext
* @param[in] ulCLen Length of ciphertext
* @param[in] ulIVLen Length of IV. pass 0 if no IV
* @param[out] pucPlainText Decrypted Plaintext
* @param[in] pulPLen Length of input buffer (pucPlainText). The length should be more than or equal to
* cipher text length (ulCLen)
* @param[out] pulPLen Length of plaintext
*
* @retval SEC_UINT32 If invalid Algorithm Id [SEC_ERR_INVALID_ALGID]
* @retval SEC_UINT32 If invalid Padding Mode. Padding mode set on Algorithm is not defined
* [SEC_CRYPT_ERR_UNDEFINED_PADDING]
* @retval SEC_UINT32 The pad bits are incorrect.  When after decrypting, the padding bits does
* not match the pad mode. It can be due to incorrect Key or IV as that will
* produce junk plaintext with random padding bits, which does not match
* the padding mode [SEC_CRYPT_ERR_INVALID_PADDING|N/A]
* @retval SEC_UINT32 If invalid Keylength for algprithm [SEC_ERR_INVALID_KEY_LEN|N/A]
* @retval SEC_UINT32 If invalid IVlength for algorithm [SEC_ERR_INVALID_IV_LEN|N/A]
* @retval SEC_UINT32 Invalid ciphertext length given for decryption in update. (ciphertext length
* was not integral number of blocks) [SEC_ERR_INVALID_DATA_LEN|N/A]
* @retval SEC_UINT32 On successful return [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 Invalid arguments [SEC_ERR_INVALID_ARG|N/A]
*
* @par Required Header File
* sec_crypto_safe.h
*
* @par Note
* - When using 3DES, application need to take care that K1(first 64 bits), K2(middle 64 bits), and K3(last 64 bits)
*    are different from each other.
* - API supports following unsafe algorithms:
*   - ALGID_RC4 (CID_RC4)
*   - ALGID_DES_ECB (CID_DES_ECB)
*   - ALGID_DES_CBC (CID_DES_CBC)
*   - ALGID_DES_OFB (CID_DES_OFB)
*   - ALGID_DES_CFB (CID_DES_CFB)
*   - ALGID_DES_EDE_ECB (CID_DES_EDE_ECB)
*   - ALGID_DES_EDE_CBC (CID_DES_EDE_CBC)
*   - ALGID_DES_EDE_OFB (CID_DES_EDE_OFB)
*   - ALGID_DES_EDE_CFB (CID_DES_EDE_CFB)
*   - ALGID_DES_EDE3_ECB (CID_DES_EDE3_ECB)
*   - ALGID_DES_EDE3_CBC (CID_DES_EDE3_CBC)
*   - ALGID_DES_EDE3_OFB (CID_DES_EDE3_OFB)
*   - ALGID_DES_EDE3_CFB (CID_DES_EDE3_CFB)
*   - ALGID_KASUMI_ECB (CID_KASUMI_ECB)
*   - ALGID_KASUMI_CBC (CID_KASUMI_CBC)
*   - ALGID_KASUMI_OFB (CID_KASUMI_OFB)
*   - ALGID_KASUMI_CFB (CID_KASUMI_CFB)
*   - ALGID_BF_ECB (CID_BF_ECB)
*   - ALGID_BF_CBC (CID_BF_CBC)
*   - ALGID_BF_CFB (CID_BF_CFB)
*   - ALGID_BF_OFB (CID_BF_OFB)
*   - ALGID_RC2_ECB (CID_RC2_ECB)
*   - ALGID_RC2_CBC (CID_RC2_CBC)
*   - ALGID_RC2_OFB (CID_RC2_OFB)
*   - ALGID_RC2_CFB (CID_RC2_CFB) \n
*   As these algorithms are unsafe, if application are using these algorithms, application must take care of any
*   security issues that may happen due to usage of these algorithms.
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_decrypt_sf(SEC_UINT32 ulAlgId, const SEC_UCHAR *pucKey, SEC_UINT32 ulKeyLen,
    const SEC_UCHAR *pucIV, SEC_UINT32 ulIVLen, const SEC_UCHAR *pucCipherText, SEC_UINT32 ulCLen,
    SEC_UCHAR *pucPlainText, SEC_UINT32 *pulPLen);

/*
    Func Name:  CRYPT_sign_sf
*/
/**
 * @defgroup CRYPT_sign_sf
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_sign_sf(
 * SEC_UINT32  ulAlg,
 * SEC_PKEY_S* pKey,
 * const SEC_UCHAR*  pucData,
 * SEC_UINT32  ulDataLen,
 * SEC_UCHAR*  pucSign,
 * SEC_UINT32* pulSignLen);
 * @endcode
 *
 * @par Purpose
 * This is used to compute sign of a given data block.
 *
 * @par Description
 * CRYPT_sign_sf function computes sign of a given data block.
 * Calls CRYPT_signInit, CRYPT_signUpdate, and CRYPT_signFinal_sf. This function is used when data
 * is present all at once. There is no need of calling Init,
 * Update, Final and signature can be calculated in one go.
 * Context is not needed here.
 *
 * @param[in] ulAlg Digest algorithm
 * @param[in] pKey The private key
 * @param[in] pucData The data
 * @param[in] ulDataLen The data length
 * @param[out] pucSign The signature
 * @param[in] pulSignLen The sign buffer (pucSign) length
 * @param[out] pulSignLen The updated sign length which is written in pucSign
 *
 * @retval SEC_UINT32 On Success [SEC_SUCCESS]
 * @retval SEC_UINT32 Invalid Arguments [SEC_ERR_INVALID_ARG]
 * @retval SEC_UINT32 Memory allocation failure [SEC_ERR_MALLOC_FAIL]
 * @retval SEC_UINT32 If operation cannot be performed on key
 * [SEC_CRYPT_ERR_UNDEFINED_KEY_OPERATION]
 * @retval SEC_UINT32 Invalid Key [SEC_CRYPT_ERR_INVALID_PKEY]
 * @retval SEC_UINT32 Key size not supported [SEC_ERR_INVALID_KEY_LEN]
 * @retval SEC_UINT32 Library not initialized [SEC_ERR_INITLIB]
 * @retval SEC_UINT32 Incorrect Algorithm ID [SEC_ERR_INVALID_ALGID]
 * @retval SEC_UINT32 On Failure [SEC_ERR]
 *
 * @par Required Header File
 * sec_crypto_safe.h
 *
 * @par Note
 * -pucSign buffer size should be at least the value returned by the CRYPT_PKEY_sign_size for the key used for signature
 * otherwise signature generation may lead to failure.
 *
 * - If the signing is done using the library version V3R3 or later, using the
 * hash algorithm SHA384 and SHA512, then verification done by
 * the older version (version before V3R3) will fail. The problem in hash
 * info of SHA384 and SHA512 in the older version is fixed in V3R3.
 *
 * - SM2 Signing is not supported by this API
 * - If application does not set seeds and entropy on frequent intervals, it can lead to vulnerable keys or random
 * value. It is recommended to set seeds and entropy on frequent intervals or update the version that migrates the
 * random implementation based on DRBG.
 * - API supports signature generation with following unsafe algorithms:
 * - ALGID_MD5 (CID_MD5)
 * - ALGID_SHA1(CID_SHA1) \n
 * As these algorithms are unsafe, if application are using these algorithms, application must take care of any
 * security issues that may happen due to usage of these algorithms.
 * - Before calling this API, please make sure that DRBG is initialized for random number generation by calling
 * IPSI_CRYPT_rand_init. If application don't want to use DRBG then appplication should call
 * IPSI_CRYPT_enable_drbg(0) to disable the DRBG Random feature.
 * - DRBG is enabled by default.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_sign_sf(SEC_UINT32 ulAlg, SEC_PKEY_S *pKey, const SEC_UCHAR *pucData,
    SEC_UINT32 ulDataLen, SEC_UCHAR *pucSign, SEC_UINT32 *pulSignLen);

/*
    Func Name:  CRYPT_signFinal_sf
*/
/**
* @defgroup CRYPT_signFinal_sf
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_signFinal_sf(
* const CRYPT_CTX*  pCtx,
* SEC_UCHAR*  pucSig,
* SEC_UINT32* pulSigLen)
* @endcode
*
* @par Purpose
* This is used to complete sign operation on remaining data,
performs sign. Called at the end of sign operation.
*
* @par Description
* CRYPT_signFinal_sf function completes sign operation on remaining
data, performs sign. Called at the end of sign operation.
* A signInit must have been called before calling this function.
This function calculates the signature and frees the
* context. The memory for the signature must have been already
allocated.
*
* @param[in] pCtx The context for sign [N/A]
* @param[out] pucSign The signed hash [N/A]
* @param[in] pulSignLen The sign buffer (pucSign) length [N/A]
* @param[out] pulSignLen The updated sign length which is written in pucSign[N/A]
*
* @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
* @retval SEC_UINT32 Invalid Arguments [SEC_ERR_INVALID_ARG|N/A]
* @retval SEC_UINT32 Incorrect context [SEC_CRYPT_ERR_INVALID_CTX|N/A]
* @retval SEC_UINT32 Memory allocation failure [SEC_ERR_MALLOC_FAIL|N/A]
* @retval SEC_UINT32 If operation cannot be performed on key
[SEC_CRYPT_ERR_UNDEFINED_KEY_OPERATION|N/A]
* @retval SEC_UINT32 Key size not supported [SEC_ERR_INVALID_KEY_LEN|N/A]
* @retval SEC_UINT32 Library not initialized [SEC_ERR_INITLIB|N/A]
* @retval SEC_UINT32 Incorrect Algorithm ID [SEC_ERR_INVALID_ALGID|N/A]
* @retval SEC_UINT32 On Failure [SEC_ERR|N/A]
*
* @par Required Header File
* sec_crypto_safe.h
*
* @par Note
* \n
* -pucSign buffer size should be at least the value returned by the CRYPT_PKEY_sign_size for the key used for signature
otherwise
* signature generation may lead to failure.
*
* - SM2 Signing is not supported by this API
* - If application does not set seeds and entropy on frequent intervals, it can lead to vulnerable keys or random
*   value. It is recommended to set seeds and entropy on frequent intervals or update the version that migrates the
*   random implementation based on DRBG.
* - Before calling this API, please make sure that DRBG is initialized for random number generation by calling
*   IPSI_CRYPT_rand_init. If application don't want to use DRBG then appplication should call
*   IPSI_CRYPT_enable_drbg(0) to disable the DRBG Random feature.
* - DRBG is enabled by default.
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_signFinal_sf(const CRYPT_CTX *pCtx, SEC_UCHAR *pucSign, SEC_UINT32 *pulSignLen);

/*
    Func Name:  CRYPT_signData_sf
*/
/**
 * @defgroup CRYPT_signData_sf
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_signData_sf(
 * SEC_PKEY_S *pKey,
 * const SEC_UCHAR *pucDigest,
 * SEC_UINT32 ulDigestLen,
 * SEC_UCHAR *pucSig,
 * SEC_UINT32 *pulSigLen,
 * SEC_UINT32 ulSigAlgType,
 * SEC_UINT32 ulRsaHashType);
 * @endcode
 *
 * @par Purpose
 * This function is used for data signing where hash is not computed.
 * @par Description
 * This function is used for data signing where hash is not computed.
 * In RSA signing, hash info (hash OID) is considered for the following hash algorithms types.
 * ALGID_SHA1
 * ALGID_SHA256
 * ALGID_SHA384
 * ALGID_SHA512
 * ALGID_MD5
 * For ALGID_MD5_SHA1, hash info (hash OID) is not used.
 * For any other hash algorithm, signing will fail.
 * For any signature algorithm other than RSA, hash algorithm type (ulRsaHashType) is ignored.
 *
 * @param[in] pKey The private key.
 * @param[in] pucDigest Pointer to the hash value to sign i.e. digest operation has to be done
 * before calling this API and the digest output should be passed.
 * @param[in] ulDigestLen Length of the hash value
 * @param[in] ulSigAlgType Signature algorithm to be used. The supported Algorithm ids are:
 * ALGID_SM2DSA
 * ALGID_RSA
 * ALGID_DSA
 * ALGID_ECDSA
 * @param[in] ulRsaHashType Hash algorithm type to be used during RSA signing with RSA_PKCS1_PADDING or
 * SEC_RSA_PSS_PADDING method.
 * This parameter is ignored for signing algorithms other than RSA.
 * @param[out] pucSig Buffer to hold the DER encoded signature
 * @param[in] pulSigLen Length of the input buffer (pucSig)
 * @param[out] pulSigLen Pointer to the length of the returned signature
 *
 * @retval SEC_UINT32 On Success. [SEC_SUCCESS]
 * @retval SEC_UINT32 Invalid arguments. [SEC_ERR_INVALID_ARG]
 * @retval SEC_UINT32 Incorrect Algorithm ID. [SEC_ERR_INVALID_ALGID]
 * @retval SEC_UINT32 Invalid key. [SEC_CRYPT_ERR_INVALID_PKEY]
 * @retval SEC_UINT32 Memory allocation failure. [SEC_ERR_MALLOC_FAIL]
 * @retval SEC_UINT32 Key size not supported. [SEC_ERR_INVALID_KEY_LEN]
 * @retval SEC_UINT32 Mathematical operation failed. [SEC_ERR]
 *
 * @par Required Header File
 * crypto_def.h
 *
 * @par Note
 * -The size of the buffer pointed by pucSig should be atleast equal to the size
 * returned by the API CRYPT_PKEY_sign_size
 * - If the application does not need that the hash will be truncated for the signatures using ECDSA or SM2DSA
 * then the application need to call CRYPT_SET_SIGN_OPTION(ulSigAlgType, SEC_EC_NO_TRUNCATION) before calling the
 * CRYPT_signData_sf. The value of ulSigAlgType will be changed by the CRYPT_SET_SIGN_OPTION, so the
 * varibale should not be used with anyother API except CRYPT_signData_sf. The value of ulSigAlgType can be reset by
 * calling CRYPT_RESET_SIGN_OPTION(ulSigAlgType).
 * - If the application  need that the hash will be truncated for the signatures using DSA
 * then the application need to call CRYPT_SET_SIGN_OPTION(ulSigAlgType, SEC_DSA_TRUNCATION) before calling the
 * CRYPT_signData_sf. The value of ulSigAlgType will be changed by the CRYPT_SET_SIGN_OPTION, so the
 * varibale should not be used with anyother API except CRYPT_signData_sf. The value of ulSigAlgType can be reset by
 * calling CRYPT_RESET_SIGN_OPTION(ulSigAlgType).
 * - If the application  need to use pss padding in the signature genreation using RSA
 * then the application need to call CRYPT_SET_SIGN_OPTION(ulSigAlgType, SEC_RSA_PSS_PADDING) before calling the
 * CRYPT_signData_sf. The value of ulSigAlgType will be changed by the CRYPT_SET_SIGN_OPTION, so the
 * varibale should not be used with anyother API except CRYPT_signData_sf. The value of ulSigAlgType can be reset by
 * calling CRYPT_RESET_SIGN_OPTION(ulSigAlgType).
 * - If application does not set seeds and entropy on frequent intervals, it can lead to vulnerable keys or random
 * value. It is recommended to set seeds and entropy on frequent intervals or update the version that migrates the
 * random implementation based on DRBG.
 * -pucSig buffer size should be at least the value returned by the CRYPT_PKEY_sign_size for the key used for signature
 * otherwise signature generation may lead to failure.
 *
 * - API supports signature generation with following unsafe algorithms:
 * - ALGID_MD5 (CID_MD5)
 * - ALGID_SHA1(CID_SHA1) \n
 * As these algorithms are unsafe, if application are using these algorithms, application must take care of any
 * security issues that may happen due to usage of these algorithms.
 * - Before calling this API, please make sure that DRBG is initialized for random number generation by calling
 * IPSI_CRYPT_rand_init. If application don't want to use DRBG then appplication should call
 * IPSI_CRYPT_enable_drbg(0) to disable the DRBG Random feature.
 * - DRBG is enabled by default.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_signData_sf(SEC_PKEY_S *pKey, const SEC_UCHAR *pucDigest, SEC_UINT32 ulDigestLen,
    SEC_UCHAR *pucSig, SEC_UINT32 *pulSigLen, SEC_UINT32 ulSigAlgType, SEC_UINT32 ulRsaHashType);

/**
 * @defgroup CRYPT_signDec_sf
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_signDec_sf(
 * const SEC_UCHAR *pucSgn,
 * SEC_UINT32 pulSgnLen,
 * SEC_UCHAR *pulSgnR,
 * SEC_UINT32 *pulSgnRLen,
 * SEC_UCHAR *pulSgnS,
 * SEC_UINT32 *pulSgnSLen,
 * SEC_UINT32 uiOption);
 * @endcode
 *
 * @par Purpose
 * This is used to Decode the Encoded DSA/ECDSA/SM2DSA Signature buffer into r and s values.
 * @par Description
 * CRYPT_signDec_sf function Decode the Encoded DSA/ECDSA/SM2DSA Signature buffer into
 * corresponding r and s value
 *
 * @param[in] pucSgn The signature [N/A]
 * @param[in] pulSgnLen The sign length [N/A]
 * @param[out] pulSgnR The r value [N/A]
 * @param[in] pulSgnRLen The length of input buffer (pulSgnR)  [N/A]
 * @param[out] pulSgnRLen The length of r value [N/A]
 * @param[out] pulSgnS The s value [N/A]
 * @param[in] pulSgnSLen The length of input buffer (pulSgnS)  [N/A]
 * @param[out] pulSgnSLen The length of s value [N/A]
 * @param[in] uiOption IPSI_APPEND_ZERO or IPSI_NO_APPEND_ZERO.
 * While generation of signature, if zero is appended before R and S value, decoded value R and S provided by this
 * function will remove the leading zero if IPSI_APPEND_ZERO is set. [N/A]
 *
 * @retval SEC_UINT32 On Success [SEC_SUCCESS|N/A]
 * @retval SEC_UINT32 Invalid Arguments [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 On Failure [SEC_ERR|N/A]
 *
 * @par Required Header File
 * sec_crypto_safe.h
 *
 * @par Note
 * - If the IPSI_APPEND_ZERO option is passed than this function decode the signature and provide R and S parameter.
 * While generation of signature, if zero is appended before R and S value, decoded value R and S provided by this
 * function will remove the leading zero. CRYPT_signDec_sf will work same as CRYPT_signDec_ex if the uiOption is set
 * as IPSI_APPEND_ZERO. The only difference is that it takes the length of R and S as input
 *
 * -CRYPT_signDec_sf will work same as CRYPT_signDec if the uiOption is set
 * as IPSI_NO_APPEND_ZERO. The only difference is that it takes the length of R and S as input
 *
 * @par Related Topics
 * CRYPT_signDec
 * CRYPT_signDec_ex
 * CRYPT_signEnc_sf
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_signDec_sf(const SEC_UCHAR *pucSgn, SEC_UINT32 pulSgnLen, SEC_UCHAR *pulSgnR,
    SEC_UINT32 *pulSgnRLen, SEC_UCHAR *pulSgnS, SEC_UINT32 *pulSgnSLen, SEC_UINT32 uiOption);

/**
 * @defgroup CRYPT_signEnc_sf
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_signEnc_sf(
 * SEC_UCHAR *pucSgn,
 * SEC_UINT32 *pulSgnLen,
 * const SEC_UCHAR *pulSgnR,
 * SEC_UINT32 pulSgnRLen,
 * const SEC_UCHAR *pulSgnS,
 * SEC_UINT32 pulSgnSLen,
 * SEC_UINT32 uiOption);
 * @endcode
 *
 * @par Purpose
 * This is used to Encode DSA/ECDSA/SM2DSA r and s signature values.
 *
 * @par Description
 * CRYPT_signEnc_sf function Encodes the DSA/ECDSA/SM2DSA r and s signature values
 * into Signature buffer
 *
 * @param[out] pucSgn The signature
 * @param[in] pulSgnLen The input buffer (pucSgn) length
 * @param[out] pulSgnLen The sign length
 * @param[in] pulSgnR The r value
 * @param[in] pulSgnRLen The length of r value
 * @param[in] pulSgnS The s value
 * @param[in] pulSgnSLen The length of s value
 * @param[in] uiOption IPSI_APPEND_ZERO or IPSI_NO_APPEND_ZERO.
 * IPSI_APPEND_ZERO - While encoding of R and S, if first character of R or S is more than 127 , zero is appended before
 * encoding R or S value. IPSI_NO_APPEND_ZERO - While encoding of R and S, if first character of R or S is more than 127
 * , zero is not appended before encoding R or S value.
 *
 *
 * @retval SEC_UINT32 On Success [SEC_SUCCESS]
 * @retval SEC_UINT32 Invalid Arguments [SEC_ERR_INVALID_ARG]
 * @retval SEC_UINT32 On Memory allocation failure [SEC_ERR_INVALID_ARG]
 * @retval SEC_UINT32 On Failure [SEC_ERR]
 *
 * @par Required Header File
 * sec_crypto_safe.h
 *
 * @par Note
 * - CRYPT_signEnc_sf will work same as CRYPT_signEnc_ex if the uiOption is set
 * as IPSI_APPEND_ZERO. The only difference is that it takes the length of input buffer (pucSgn) as input
 *
 * -CRYPT_signEnc_sf will work same as CRYPT_signEnc if the uiOption is set
 * as IPSI_NO_APPEND_ZERO. The only difference is that it takes the length of input buffer (pucSgn) as input
 *
 * @par Related Topics
 * CRYPT_signEnc_ex
 * CRYPT_signEnc
 * CRYPT_signDec_sf
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_signEnc_sf(SEC_UCHAR *pucSgn, SEC_UINT32 *pulSgnLen, const SEC_UCHAR *pulSgnR,
    SEC_UINT32 pulSgnRLen, const SEC_UCHAR *pulSgnS, SEC_UINT32 pulSgnSLen, SEC_UINT32 uiOption);

/*
    Func Name:  CRYPT_pubKeyEncrypt_sf
*/
/**
* @defgroup CRYPT_pubKeyEncrypt_sf
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_pubKeyEncrypt_sf(
* SEC_PKEY_S* pKey,
* const SEC_UCHAR*  pucPlainText,
* SEC_UINT32  ulPlainLen,
* SEC_UINT32  ulPadding,
* SEC_UCHAR*  pucCipherText,
* SEC_UINT32* pulCLen);
* @endcode
*
* @par Purpose
* This is used to perform a public key encryption operation.
*
* @par Description
* CRYPT_pubKeyEncrypt_sf function performs a public key
encryption operation. The key passed to the function must be a
* public key or a key pair. The data size must not e
xceed size of key and for padding modes DEFAULT, PKCS1 and SSLv23,
* data size must not be < (key size - 11 bytes), and for OAEP,
* data size must be < (key size - 41 bytes)for SHA1
* data size must be < (key size - 65 bytes) for SHA256
* data size must be < (key size - 129 bytes) for SHA512.
*
* @param[in] pKey Public key to be used
* @param[in] pucPlainText Data buffer
* @param[in] ulPlainLen Data buffer length
* @param[in] ulPadding Padding mode used
* @param[out] pucCipherText Encrypted data buffer
* @param[in] pulCLen Input buffer (pucCipherText) length
* @param[in,out] pulCLen Encrypted data buffer length is the size of pucCipherText, it should be equal to
* or greater than the key size in bytes. Library will update this pointer to  actual cipher text length
*
* @retval SEC_UINT32 On Success [SEC_SUCCESS]
* @retval SEC_UINT32 Invalid Arguments [SEC_ERR_INVALID_ARG]
* @retval SEC_UINT32 Key size not supported [SEC_ERR_INVALID_KEY_LEN]
* @retval SEC_UINT32 Invalid Key [SEC_CRYPT_ERR_INVALID_PKEY]
* @retval SEC_UINT32 Key is neither Pair nor public [SEC_CRYPT_ERR_INVALID_PKEY_TYPE]
* @retval SEC_UINT32 Data size larger than key [SEC_CRYPT_ERR_LARGE_DATA_SIZE]
* @retval SEC_UINT32 Undefined padding mode [SEC_CRYPT_ERR_UNDEFINED_PADDING]
* @retval SEC_UINT32 If operation cannot be
* performed on key [SEC_CRYPT_ERR_UNDEFINED_KEY_OPERATION]
* @retval SEC_UINT32 Library not initialized [SEC_ERR_INITLIB]
* @retval SEC_UINT32 Incorrect Algorithm ID [SEC_ERR_INVALID_ALGID]
* @retval SEC_UINT32 On Failure [SEC_ERR]
*
* @par Required Header File
* sec_crypto_safe.h
*
* @par Note
* \n
* - pucCipherText buffer size should be equal to or greater than the key size in bytes.
*    CRYPT_PKEY_size can be used to get the key size.
* - In case of no padding, if the data size is less than the key size then zero will be prepended to the data
      to create the data size equal to key size.
* - It is recommended that RSA_SSLV23_PADDING should be used only with SSL protocol.
* - If application does not set seeds and entropy on frequent intervals, it can lead to vulnerable keys or random
*   value. It is recommended to set seeds and entropy on frequent intervals or update the version that migrates the
*   random implementation based on DRBG.
* - For OEAP padding method, use padding mode RSA_PKCS1_OAEP_PADDING for SHA1 as hash, RSA_PKCS1_OAEP_PADDING_SHA256
*    for SHA256 as hash & RSA_PKCS1_OAEP_PADDING_SHA512 for SHA512 as hash.
* - Before calling this API, please make sure that DRBG is initialized for random number generation by calling
*   IPSI_CRYPT_rand_init. If application don't want to use DRBG then appplication should call
*   IPSI_CRYPT_enable_drbg(0) to disable the DRBG Random feature.
* - DRBG is enabled by default.
*
* @par Related Topics
* CRYPT_pubKeyEncrypt
* CRYPT_privKeyDecrypt_sf
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_pubKeyEncrypt_sf(SEC_PKEY_S *pKey, const SEC_UCHAR *pucPlainText, SEC_UINT32 ulPlainLen,
    SEC_UINT32 ulPadding, SEC_UCHAR *pucCipherText, SEC_UINT32 *pulCLen);

/**
* @defgroup CRYPT_privKeyDecrypt_sf
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_privKeyDecrypt_sf (
* SEC_PKEY_S* pKey,
* const SEC_UCHAR*  pucCipherText,
* SEC_UINT32  ulCipherLen,
* SEC_UINT32  ulPadding,
* SEC_UCHAR*  pucPlainText,
* SEC_UINT32* pulPLen);
* @endcode
*
* @par Purpose
* This is used to perform a private key decryption operation.
*
* @par Description
* CRYPT_privKeyDecrypt_sf function performs a private key
decryption operation. The key must be a private key or a key pair.
*
* @param[in] pKey Private key to be used
* @param[in] pucCipherText Cipher buffer
* @param[in] ulCipherLen Cipher buffer length
* @param[in] ulPadding Padding mode used
* @param[out] pucPlainText Data buffer [N/A]
* @param[in] pulPLen Length of input buffer (pucPlainText)
* @param[in,out] pulPLen Data buffer length is the size of pucPlainText , it should be equal to
* or greater than the key size in bytes,. Library will update this pointer to  actual plain text length.
*
* @retval SEC_UINT32 On Success [SEC_SUCCESS]
* @retval SEC_UINT32 Invalid Arguments [SEC_ERR_INVALID_ARG]
* @retval SEC_UINT32 Key size not supported [SEC_ERR_INVALID_KEY_LEN]
* @retval SEC_UINT32 Invalid Key [SEC_CRYPT_ERR_INVALID_PKEY]
* @retval SEC_UINT32 Key is neither Pair nor Private [SEC_CRYPT_ERR_INVALID_PKEY_TYPE]
* @retval SEC_UINT32 Data size larger than key [SEC_CRYPT_ERR_LARGE_DATA_SIZE]
* @retval SEC_UINT32 Undefined padding mode [SEC_CRYPT_ERR_UNDEFINED_PADDING]
* @retval SEC_UINT32 If operation cannot be performed on key [SEC_CRYPT_ERR_UNDEFINED_KEY_OPERATION]
* @retval SEC_UINT32 Library not initialized [SEC_ERR_INITLIB]
* @retval SEC_UINT32 Incorrect Algorithm ID [SEC_ERR_INVALID_ALGID]
* @retval SEC_UINT32 On Failure [SEC_ERR]
*
* @par Required Header File
* sec_crypto_safe.h
*
* @par Note
* \n
* - pucPlainText buffer size should be equal to or greater than the key size in bytes.
*    CRYPT_PKEY_size can be used to get the key size.
* - In case of no padding, the zero bytes are prepended during CRYPT_pubKeyEncrypt will not be removed.
* - It is recommended that RSA_SSLV23_PADDING should be used only with SSL protocol.
* - For OEAP padding method, use padding mode RSA_PKCS1_OAEP_PADDING for SHA1 as hash, RSA_PKCS1_OAEP_PADDING_SHA256
*    for SHA256 as hash & RSA_PKCS1_OAEP_PADDING_SHA512 for SHA512 as hash.
* - Before calling this API, please make sure that DRBG is initialized for random number generation by calling
*   IPSI_CRYPT_rand_init. If application don't want to use DRBG then appplication should call
*   IPSI_CRYPT_enable_drbg(0) to disable the DRBG Random feature.
* - DRBG is enabled by default.
* @par Related Topics
* CRYPT_privKeyDecrypt
* CRYPT_pubKeyEncrypt_sf
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_privKeyDecrypt_sf(SEC_PKEY_S *pKey, const SEC_UCHAR *pucCipherText,
    SEC_UINT32 ulCipherLen, SEC_UINT32 ulPadding, SEC_UCHAR *pucPlainText, SEC_UINT32 *pulPLen);

/*
    Func Name:  CRYPT_privKeyEncrypt_sf
*/
/**
 * @defgroup CRYPT_privKeyEncrypt_sf
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_privKeyEncrypt_sf(
 * SEC_PKEY_S* pkey,
 * const SEC_UCHAR*  pucPlainText,
 * SEC_UINT32  ulPlainLen,
 * SEC_UINT32  ulPadding,
 * SEC_UCHAR*  pucCipherText,
 * SEC_UINT32* pulCLen);
 * @endcode
 *
 * @par Purpose
 * This is used to perform a private key encryption operation.
 *
 * @par Description
 * CRYPT_privKeyEncrypt_sf function performs a private key
 * encryption operation. The key passed to the function must be a
 * private key or a key pair. The data size must not exceed size of key and
 * for padding modes PKCS1 - Type1 (Default padding mode) data size must be < (key size - 11 bytes) and
 * for No padding data size should be equal or less than the key size. Supported padding modes are
 * PKCS1 (Type1), No padding and Default Padding. Default Padding will be PKCS1 (Type1) padding.
 * @param[in] pKey Private key to be used
 * @param[in] pucPlainText Data buffer
 * @param[in] ulPlainLen Data buffer length
 * @param[in] ulPadding Padding mode used
 * @param[out] pucCipherText Encrypted data buffer
 * @param[in] pulCLen Input buffer (pucCipherText) length [N/A]
 * @param[out] pulCLen Encrypted data buffer length
 *
 * @retval SEC_UINT32 On Success [SEC_SUCCESS]
 * @retval SEC_UINT32 Invalid Arguments [SEC_ERR_INVALID_ARG]
 * @retval SEC_UINT32 Key size not supported [SEC_ERR_INVALID_KEY_LEN]
 * @retval SEC_UINT32 Invalid Key [SEC_CRYPT_ERR_INVALID_PKEY]
 * @retval SEC_UINT32 Key is neither Pair nor private [SEC_CRYPT_ERR_INVALID_PKEY_TYPE]
 * @retval SEC_UINT32 Data size larger than key [SEC_CRYPT_ERR_LARGE_DATA_SIZE]
 * @retval SEC_UINT32 Undefined padding mode [SEC_CRYPT_ERR_UNDEFINED_PADDING]
 * @retval SEC_UINT32 If operation cannot be performed on key [SEC_CRYPT_ERR_UNDEFINED_KEY_OPERATION]
 * @retval SEC_UINT32 Library not initialized [SEC_ERR_INITLIB]
 * @retval SEC_UINT32 Incorrect Algorithm ID [SEC_ERR_INVALID_ALGID]
 * @retval SEC_UINT32 On Failure [SEC_ERR]
 *
 * @par Required Header File
 * sec_crypto_safe.h
 *
 * @par Note
 * \n
 * - pucCipherText buffer size should be equal to or greater than the key size in bytes.
 * CRYPT_PKEY_size can be used to get the key size.
 * - In case of no padding, if the data size is less than the key size then zero will be prepended to the data
 * to create the data size equal to key size.
 * - If application does not set seeds and entropy on frequent intervals, it can lead to vulnerable keys or random
 * value. It is recommended to set seeds and entropy on frequent intervals or update the version that migrates the
 * random implementation based on DRBG.
 * - Before calling this API, please make sure that DRBG is initialized for random number generation by calling
 * IPSI_CRYPT_rand_init. If application don't want to use DRBG then appplication should call
 * IPSI_CRYPT_enable_drbg(0) to disable the DRBG Random feature.
 * - DRBG is enabled by default.
 *
 * @par Related Topics
 * CRYPT_pubKeyDecrypt_sf
 * CRYPT_privKeyEncrypt
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_privKeyEncrypt_sf(SEC_PKEY_S *pKey, const SEC_UCHAR *pucPlainText, SEC_UINT32 ulPlainLen,
    SEC_UINT32 ulPadding, SEC_UCHAR *pucCipherText, SEC_UINT32 *pulCLen);

/*
    Func Name:  CRYPT_pubKeyDecrypt_sf
*/
/**
 * @defgroup CRYPT_pubKeyDecrypt_sf
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_pubKeyDecrypt (
 * SEC_PKEY_S* pkey,
 * const SEC_UCHAR*  pucCipherText,
 * SEC_UINT32  ulCipherLen,
 * SEC_UINT32  ulPadding,
 * SEC_UCHAR*  pucPlainText,
 * SEC_UINT32* pulPLen);
 * @endcode
 *
 * @par Purpose
 * This is used to perform a public key decryption operation.
 *
 * @par Description
 * CRYPT_pubKeyDecrypt_sf function performs a public key
 * decryption operation. The key must be a public key or a key pair. Supported padding modes are
 * PKCS1 (Type1), No padding and Default Padding. Default Padding will be PKCS1 (Type1) padding.
 *
 * @param[in] pKey Public key to be used
 * @param[in] pucCipherText Cipher buffer
 * @param[in] ulCipherLen Cipher buffer length
 * @param[in] ulPadding Padding mode used
 * @param[out] pucPlainText Data buffer
 * @param[in] pulPLen Input buffer (pucPlainText) length [N/A]
 * @param[out] pulPLen Data buffer length
 *
 * @retval SEC_UINT32 On Success [SEC_SUCCESS]
 * @retval SEC_UINT32 Invalid Arguments [SEC_ERR_INVALID_ARG]
 * @retval SEC_UINT32 Key size not supported [SEC_ERR_INVALID_KEY_LEN]
 * @retval SEC_UINT32 Invalid Key [SEC_CRYPT_ERR_INVALID_PKEY]
 * @retval SEC_UINT32 Key is neither Pair nor Public [SEC_CRYPT_ERR_INVALID_PKEY_TYPE]
 * @retval SEC_UINT32 Data size larger than key [SEC_ERR_DATA_GREATER_THAN_KEY_LEN]
 * @retval SEC_UINT32 Undefined padding mode [SEC_CRYPT_ERR_UNDEFINED_PADDING]
 * @retval SEC_UINT32 If operation cannot be performed on key [SEC_CRYPT_ERR_UNDEFINED_KEY_OPERATION]
 * @retval SEC_UINT32 Library not initialized [SEC_ERR_INITLIB]
 * @retval SEC_UINT32 Incorrect Algorithm ID [SEC_ERR_INVALID_ALGID]
 * @retval SEC_UINT32 On Failure [SEC_ERR]
 *
 * @par Required Header File
 * sec_crypto_safe.h
 *
 * @par Note
 * \n
 * - pucPlainText buffer size should be equal to or greater than the key size in bytes.
 * CRYPT_PKEY_size can be used to get the key size.
 * - In case of no padding, the zero bytes which are prepended during CRYPT_privKeyEncrypt will not be removed.
 *
 * @par Related Topics
 * CRYPT_privKeyEncrypt_sf
 * CRYPT_pubKeyDecrypt
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_pubKeyDecrypt_sf(SEC_PKEY_S *pKey, const SEC_UCHAR *pucCipherText,
    SEC_UINT32 ulCipherLen, SEC_UINT32 ulPadding, SEC_UCHAR *pucPlainText, SEC_UINT32 *pulPLen);

/*
    Func Name:  CRYPT_computeDHKey_sf
*/
/**
 * @defgroup CRYPT_computeDHKey_sf
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_computeDHKey_sf(
 * const SEC_PKEY_S* pPvtKey,
 * const SEC_PKEY_S* pPubKey,
 * SEC_UCHAR* pucKey,
 * SEC_UINT32* pulKeyLen)
 * @endcode
 *
 * @par Purpose
 * This is used to generate a DH shared secret key from private key and peer public key.
 *
 * @par Description
 * CRYPT_computeDHKey_sf function generates a DH shared secret key from private key and peer public key. The memory for
 * the shared secret must have been allocated prior to calling.
 *
 * @param[in] pPvtKey Own private key
 * @param[in] pPubKey Peer public key
 * @param[out] pucKey Shared secret key
 * @param[in] pulKeyLen Length of input buffer (pucKey)
 * @param[out] pulKeyLen Length of generated shared secret key
 *
 * @retval SEC_UINT32 On Success [SEC_SUCCESS]
 * @retval SEC_UINT32 If algorithm is not supported [SEC_CRYPT_ERR_UNDEFINED_KEY_OPERATION]
 * @retval SEC_UINT32 Invalid Key [SEC_CRYPT_ERR_INVALID_PKEY]
 * @retval SEC_UINT32 Invalid Arguments [SEC_ERR_INVALID_ARG]
 * @retval SEC_UINT32 If library is not initialized [SEC_ERR_INITLIB]
 *
 * @par Required Header File
 * sec_crypto_safe.h
 *
 * @par Note
 * - pucKey size should be atleast the size of the private key (pPvtKey). CRYPT_PKEY_size (pPvtKey) can be used to know
 * the minimum size required for pucKey.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_computeDHKey_sf(const SEC_PKEY_S *pPvtKey, const SEC_PKEY_S *pPubKey, SEC_UCHAR *pucKey,
    SEC_UINT32 *pulKeyLen);

/*
    Func Name:  CRYPT_computeECDHKey_sf
*/
/**
 * @defgroup CRYPT_computeECDHKey_sf
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_computeECDHKey_sf(
 * const SEC_PKEY_S *pPvtKey,
 * const SEC_PKEY_S *pPubKey,
 * SEC_UCHAR *pucKey,
 * SEC_UINT32 *pulKeyLen,
 * IPSI_ECDH_PRIMITIVE_E uiEcdhPrimitive);
 * @endcode
 *
 * @par Purpose
 * This function is used to generate a ECDH shared secret key.
 * This function supports both Elliptic Curve Standard and Modified (cofactor) Diffie Hellman Primitive.
 *
 * @par Description
 * This function generates a ECDH shared secret key from private key and peer public key.
 * The memory for the shared secret must have been allocated prior to calling.
 * Based on the enum value passed to uiEcdhPrimitive, this function generates shared secret key using
 * either standard or modified Diffie Hellman primitive.
 * In the EC Standard DH primitive, cofactor is not required and pCoFactor in EC_PARA_S is ignored.
 * In the EC Modified (cofactor) DH primitive, pCoFactor value present in EC_PARA_S is considered
 * and error is returned if pCoFactor is set to NULL.
 *
 * @param[in] pPvtKey Own private key
 * @param[in] pPubKey Peer public key
 * @param[out] pucKey Shared secret key
 * @param[in] pulKeyLen Length of the input buffer (pucKey)
 * @param[out] pulKeyLen Length of generated shared secret key
 * @param[in] uiEcdhPrimitive Identifies the ECDH primitive to be used whether EC Standard DH Primitive or
 * EC Modified (Cofactor) DH Primitive
 *
 * @retval SEC_UINT32 On Success [SEC_SUCCESS]
 * @retval SEC_UINT32 Invalid Key [SEC_CRYPT_ERR_INVALID_PKEY]
 * @retval SEC_UINT32 Invalid Arguments [SEC_ERR_INVALID_ARG]
 * @retval SEC_UINT32 If library is not initialized [SEC_ERR_INITLIB]
 * @retval SEC_UINT32 Incorrect Algorithm ID i.e. Key type is not ALGID_ECDH or ALGID_ECDSA or
 * ALGID_ECKEY [SEC_ERR_INVALID_ALGID]
 * @retval SEC_UINT32 Memory allocation failed [SEC_ERR_MALLOC_FAIL]
 * @retval SEC_UINT32 On other errors [SEC_ERR]
 *
 * @par Required Header File
 * sec_crypto_safe.h
 *
 * @par Note
 * - The minimum size of output key buffer should be atleast the maximum of public key size and private key size.
 * CRYPT_PKEY_size can be used to know the minimum size required for pucKey. \n
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_computeECDHKey_sf(const SEC_PKEY_S *pPvtKey, const SEC_PKEY_S *pPubKey,
    SEC_UCHAR *pucKey, SEC_UINT32 *pulKeyLen, IPSI_ECDH_PRIMITIVE_E uiEcdhPrimitive);

/*
    Func Name:  CRYPT_sm2GenZ_sf
*/
/**
 * @defgroup CRYPT_sm2GenZ_sf
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_sm2GenZ_sf (
 * SEC_UINT32 ulDigestAlg,
 * const SEC_PKEY_S *pKey,
 * const SEC_UCHAR *pucId,
 * SEC_UINT32 uiIdLen,
 * SEC_UCHAR *pucZ,
 * SEC_UINT32 *pulZLen);
 * @endcode
 *
 * @par Purpose
 * This API is used to generate Z value, based on the Unique User ID,
 * Elliptic curve parameters and selected Hash Algorithm.
 * The Z - value has to be used later for various SM2 operations.
 * Logic to calculate ZA (Read as Z value for user A)
 * ZA : HashAlg (ENTLA || IDA || a || b || xG || yG || xA || yA)
 * ENTLA   - Length of unique user identifier string (optional)
 * IDA     - Unique user identifier string (optional)
 * a		- Coefficient 'a' of the Elliptic Curve equation
 * b		- Coefficient 'b' of the Elliptic Curve equation
 * xG      - X value of base point of the Elliptic Curve
 * yG      - Y value of base point of the Elliptic Curve
 * xA      - X value of public key point on the Elliptic Curve for user A
 * yA      - Y value of public key point on the Elliptic Curve for user A
 * @par Description
 * CRYPT_sm2GenZ_sf function generates the Z value specific to a user.
 *
 * @param[in] ulDigestAlg Hash algorithm to be used in Z calculation
 * Supports only 256 bit hash algorithms like ALGID_SHA256 and ALGID_SM3.
 * @param[in] pKey The (Public) or (Public & Private pair) Key, along with EC parameters
 * @param[in] pucId Unique ID of the user
 * Optional parameter. Pass NULL, if not applicable.
 * @param[in] uiIdLen Length of Unique ID of user
 * Optional parameter. Pass zero, if unique ID is NULL. Maximum uIdLen can be (65535/8) = 8191.
 * @param[out] pucZ Z value of the user
 * @param[in] pulZLen Length of input buffer (pucZ)
 * @param[out] pulZLen Length of Z value
 *
 * @retval SEC_UINT32 On Success [SEC_SUCCESS]
 * @retval SEC_UINT32 Invalid arguments passed [SEC_ERR_INVALID_ARG]
 * @retval SEC_UINT32 Incorrect Algorithm ID [SEC_ERR_INVALID_ALGID]
 * @retval SEC_UINT32 Memory allocation failed [SEC_ERR_MALLOC_FAIL]
 * @retval SEC_UINT32 Mathematical operation failed [SEC_ERR]
 * @retval SEC_UINT32 Invalid pKey passed [SEC_CRYPT_ERR_INVALID_PKEY]
 *
 * @par Required Header File
 * sec_crypto_safe.h
 *
 * @par Note
 * The size of the buffer pointed by pucZ should be equal to or more than the length
 * returned by the API CRYPT_MD_SIZE(ulDigestAlg)
 * \n
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_sm2GenZ_sf(SEC_UINT32 ulDigestAlg, const SEC_PKEY_S *pKey, const SEC_UCHAR *pucId,
    SEC_UINT32 uiIdLen, SEC_UCHAR *pucZ, SEC_UINT32 *pulZLen);

/*
    Func Name:  CRYPT_sm2KepGenHash_sf
*/
/**
 * @defgroup CRYPT_sm2KepGenHash_sf
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_sm2KepGenHash_sf(
 * CRYPT_SM2_KEP_CTX ctx,
 * IPSI_SM2_KEP_HASHTYPE_E enHashType,
 * SEC_UCHAR *pucS,
 * SEC_UINT32 *pulSLen)
 * @endcode
 *
 * @par Purpose
 * This function generates the optional hash as mentioned in the standard.
 * @par Description
 * This function generates the optional hash as mentioned in the standard.
 * If we consider the hash generation of user B, this function is equivalent to the step B8
 * and enHashType should be selected as IPSI_SM2_HASH_SB. If we consider the hash generation for user A,
 * this function is equivalent to the step A10 and enHashType should be selected as IPSI_SM2_HASH_SA.
 *
 * @param[in] ctx Contains the input required for Key Generation
 * @param[in] enHashType The hash operation to be considered either IPSI_SM2_HASH_SA or IPSI_SM2_HASH_SB
 * @param[out] pucS Generated hash
 * @param[in] pulSLen Length of input buffer (pucS)
 * @param[out] pulSLen Pointer to the length of generated hash
 *
 * @retval SEC_UINT32 On Success [SEC_SUCCESS]
 * @retval SEC_UINT32 Invalid arguments passed [SEC_ERR_INVALID_ARG]
 * @retval SEC_UINT32 Memory allocation failed [SEC_ERR_MALLOC_FAIL]
 * @retval SEC_UINT32 Mathematical operation failed [SEC_ERR]
 * @retval SEC_UINT32 Invalid SM2 KEP context [SEC_CRYPT_ERR_INVALID_CTX]]
 *
 * @par Required Header File
 * sec_crypto_safe.h
 *
 * @par Note
 * The size of the buffer pointed by pucS should be CRYPT_MD_SIZE(ulHashAlg).
 * ulHashAlg is the hash algorithm set in the API CRYPT_sm2KepInit
 * \n
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_sm2KepGenHash_sf(CRYPT_SM2_KEP_CTX ctx, IPSI_SM2_KEP_HASHTYPE_E enHashType,
    SEC_UCHAR *pucS, SEC_UINT32 *pulSLen);

/*
    Func Name:  CRYPT_sm2PkeaEncrypt_sf
*/
/**
 * @defgroup CRYPT_sm2PkeaEncrypt_sf
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_sm2PkeaEncrypt_sf(
 * SEC_PKEY_S *pKey,
 * SEC_UINT32 ulKdfDigestAlg,
 * SEC_UINT32 ulHashAlg,
 * SEC_UCHAR *pucPlainText,
 * SEC_UINT32 ulPlainLen,
 * SEC_UINT32 uiCipherType,
 * SEC_UCHAR *pucCipherText,
 * SEC_UINT32 *pulCLen);
 * @endcode
 *
 * @par Purpose
 * This function encrypts the input plaintext as per SM2 PKEA standard.
 * @par Description
 * This function encrypts the input plaintext as per SM2 PKEA standard.
 * It takes as input the EC public key and digest algorithm used during kdf calculation
 * and the hash algorithm used for calculation of C3.
 *
 * @param[in] pKey Contains the EC public key
 * @param[in] ulKdfDigestAlg Hash algorithm to be used during KDF operation
 * @param[in] ulHashAlg Hash algorithm to be used for calculation of C3
 * @param[in] pucPlainText Pointer to input data to be encrypted
 * @param[in] ulPlainLen Input data buffer length
 * @param[in] uiCipherType The cipher text consist of C1|| C2 || C3, C1 is a point and the point can
 * be encoded as compressed or uncompressed format and the format will be determined by uiCipherType. The
 * value of uiCipherType can be SEC_UNCOMPRESSED_CIPHER or SEC_COMPRESSED_CIPHER
 * @param[out] pucCipherText  Pointer to the output encrypted data buffer
 * @param[in] pulCLen Length of the input buffer (pucCipherText)
 * @param[out] pulCLen Pointer to the encrypted data length
 *
 * @retval SEC_UINT32 On Success [SEC_SUCCESS]
 * @retval SEC_UINT32 Invalid arguments passed [SEC_ERR_INVALID_ARG]
 * @retval SEC_UINT32 Memory allocation failed [SEC_ERR_MALLOC_FAIL]
 * @retval SEC_UINT32 Incorrect Hash Algorithm ID [SEC_ERR_INVALID_ALGID]
 * @retval SEC_UINT32 Invalid Key [SEC_CRYPT_ERR_INVALID_PKEY]
 * @retval SEC_UINT32 Mathematical operation failed [SEC_ERR]
 *
 * @par Required Header File
 * sec_crypto_safe.h
 *
 * @par Note
 * -The size of the buffer pointed by pucCipherText, should be atleast equal to the
 * size returned by the API CRYPT_sm2PkeaGetOutputSize.
 * - ALGID_MD5 is not a valid algorithm for ulKdfDigestAlg
 * - ALGID_MD4 is not a valid algorithm for ulHashAlg and ulKdfDigestAlg
 * - If application does not set seeds and entropy on frequent intervals, it can lead to vulnerable keys or random
 * value. It is recommended to set seeds and entropy on frequent intervals or update the version that migrates the
 * random implementation based on DRBG.
 * - Before calling this API, please make sure that DRBG is initialized for random number generation by calling
 * IPSI_CRYPT_rand_init. If application don't want to use DRBG then appplication should call
 * IPSI_CRYPT_enable_drbg(0) to disable the DRBG Random feature.
 * - DRBG is enabled by default.
 *
 * \n
 *
 * @par Related Topics
 * CRYPT_sm2PkeaGetOutputSize
 * CRYPT_sm2PkeaDecrypt_sf
 * CRYPT_sm2PkeaEncrypt
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_sm2PkeaEncrypt_sf(SEC_PKEY_S *pKey, SEC_UINT32 ulKdfDigestAlg, SEC_UINT32 ulHashAlg,
    SEC_UCHAR *pucPlainText, SEC_UINT32 ulPlainLen, SEC_UINT32 uiCipherType, SEC_UCHAR *pucCipherText,
    SEC_UINT32 *pulCLen);

/*
    Func Name:  CRYPT_sm2PkeaDecrypt_sf
*/
/**
 * @defgroup CRYPT_sm2PkeaDecrypt_sf
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_sm2PkeaDecrypt_sf(
 * SEC_PKEY_S *pKey,
 * SEC_UINT32 ulKdfDigestAlg,
 * SEC_UINT32 ulHashAlg,
 * SEC_UCHAR *pucCipherText,
 * SEC_UINT32 ulCipherLen,
 * SEC_UCHAR *pucPlainText,
 * SEC_UINT32 *pulPLen);
 * @endcode
 *
 * @par Purpose
 * This function decrypts the cipher text as per SM2 PKEA standard.
 * @par Description
 * This function decrypts the cipher text as per SM2 PKEA standard.
 * It takes as input the EC public key and digest algorithm used during kdf calculation
 * and the hash algorithm used for calculation of C3.
 *
 * @param[in] pKey Contains the EC public key
 * @param[in] ulKdfDigestAlg Hash algorithm to be used during KDF operation
 * @param[in] ulHashAlg Hash algorithm to be used for calculation of C3
 * @param[in] pucCipherText Pointer to the input cipher text
 * @param[in] ulCipherLen Input cipher text length
 * @param[out] pucPlainText  Pointer to the decrypted data buffer
 * @param[out] pulPLen Length of the input buffer (pucPlainText)
 * @param[out] pulPLen Pointer to decrypted data length
 *
 * @retval SEC_UINT32 On Success [SEC_SUCCESS]
 * @retval SEC_UINT32 Invalid arguments passed [SEC_ERR_INVALID_ARG]
 * @retval SEC_UINT32 Memory allocation failed [SEC_ERR_MALLOC_FAIL]
 * @retval SEC_UINT32 Incorrect Hash Algorithm ID [SEC_ERR_INVALID_ALGID]
 * @retval SEC_UINT32 Invalid Key [SEC_CRYPT_ERR_INVALID_PKEY]
 * @retval SEC_UINT32 Mathematical operation failed [SEC_ERR]
 *
 * @par Required Header File
 * sec_crypto_safe.h
 *
 * @par Note
 * - The size of the buffer pointed to by pucPlainText, should be atleast equal to ulCipherLen.
 * - ALGID_MD5 is not a valid algorithm for ulKdfDigestAlg
 * - ALGID_MD4 is not a valid algorithm for ulHashAlg and ulKdfDigestAlg
 * - If application does not set seeds and entropy on frequent intervals, it can lead to vulnerable keys or random
 * value. It is recommended to set seeds and entropy on frequent intervals or update the version that migrates the
 * random implementation based on DRBG.
 * \n
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_sm2PkeaDecrypt_sf(SEC_PKEY_S *pKey, SEC_UINT32 ulKdfDigestAlg, SEC_UINT32 ulHashAlg,
    SEC_UCHAR *pucCipherText, SEC_UINT32 ulCipherLen, SEC_UCHAR *pucPlainText, SEC_UINT32 *pulPLen);

/*
    Func Name:  CRYPT_digestFinal_sf
*/
/**
* @defgroup CRYPT_digestFinal_sf
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_digestFinal_sf (
* CRYPT_CTX*  pCtx,
* SEC_UCHAR*  pucDigest,
* SEC_UINT32* pulDigestLen);
* @endcode
*
* @par Purpose
* This is used to complete digest operation on remaining data, and is
called at the end of digest operation.
*
* @par Description
* CRYPT_digestFinal_sf function completes digest operation on remaining data, and
is called at the end of digest operation.
* A digestInit must have been called before calling this function. This
function calculates the digest and frees the
* context. The memory for digest must already have been allocated.
*
* @param[in] pCtx The context for digest
* @param[out] pucDigest The digest
* @param[in] pulDigestLen The length of input buffer (pucDigest)
* @param[out] pulDigestLen The digest length
*
* @retval SEC_UINT32 If digest is calculated [SEC_SUCCESS]
* @retval SEC_UINT32 If context values are not correct [SEC_CRYPT_ERR_INVALID_CTX]
* @retval SEC_UINT32 If arguments are invalid[SEC_ERR_INVALID_ARG]
*
* @par Required Header File
* sec_crypto_safe.h
*
* @par Note
* - pucDigest buffer length should be more or equal to the value returned by the API CRYPT_MD_size.
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_digestFinal_sf(CRYPT_CTX *pCtx, SEC_UCHAR *pucDigest, SEC_UINT32 *pulDigestLen);

/*
    Func Name:  CRYPT_digest_sf
*/
/**
* @defgroup CRYPT_digest_sf
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_digest_sf (
* SEC_UINT32  ulAlgType,
* const SEC_UCHAR*  pucData,
* SEC_UINT32  ulDataLen,
* SEC_UCHAR*  pucDigest,
* SEC_UINT32* pulDigestLen);
* @endcode
*
* @par Purpose
* This is used to compute digest of a given data block.
*
* @par Description
* CRYPT_digest_sf function computes digest of a given data block. Calls init,
update, and final. This function is used when
* data is present all at once. There is no need of calling Init, Update,
Final and digest can be calculated in one go.
* Also, context is not required.
*
* @param[in] ulAlgType Digest algorithm
* @param[in] pucData The data
* @param[in] ulDataLen The data length
* @param[out] pucDigest The digest
* @param[in] pulDigestLen The length of input buffer (pucDigest) [N/A]
* @param[out] pulDigestLen The digest length
*
* @retval SEC_UINT32 If digest is calculated [SEC_SUCCESS]
* @retval SEC_UINT32 If context values got corrupted internally [SEC_CRYPT_ERR_INVALID_CTX]
* @retval SEC_UINT32 If arguments are missing [SEC_ERR_INVALID_ARG]
* @retval SEC_UINT32 If algorithm undefined [SEC_ERR_INVALID_ALGID]
*
* @par Required Header File
* sec_crypto_safe.h
*
* @par Note
* - pucDigest buffer length should be more or equal to the value returned by the API CRYPT_MD_size.
* - API supports following unsafe algorithms:
*   - ALGID_MD4 (CID_MD4)
*   - ALGID_MD5 (CID_MD5)
*   - ALGID_SHA1(CID_SHA1) \n
*   As these algorithms are unsafe, if application are using these algorithms, application must take care of any
*   security issues that may happen due to usage of these algorithms.
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_digest_sf(SEC_UINT32 ulAlgType, const SEC_UCHAR *pucData, SEC_UINT32 ulDataLen,
    SEC_UCHAR *pucDigest, SEC_UINT32 *pulDigestLen);

/*
    Func Name:  CRYPT_hmacFinal_sf
*/
/**
 * @defgroup CRYPT_hmacFinal_sf
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_hmacFinal_sf (
 * CRYPT_CTX*  pCtx,
 * SEC_UCHAR*  pucMAC,
 * SEC_UINT32* pulMACLen);
 * @endcode
 *
 * @par Purpose
 * This is used to complete hmac operation on remaining data. Called at the end of MAC operation.
 *
 * @par Description
 * CRYPT_hmacFinal_sf function completes hmac operation on
 * remaining data. It is called at the end of MAC operation. CRYPT_hmacInit
 * must have been called before calling this function. This function calculates the MAC and frees the context. The
 * memory for MAC must have already been allocated.
 *
 * @param[in] pCtx The context for digest
 * @param[out] pucMAC The hashed MAC
 * @param[in] pulMACLen The length of input buffer (pucMAC)
 * @param[out] pulMACLen The MAC length
 *
 * @retval SEC_UINT32 If digest is calculated [SEC_SUCCESS]
 * @retval SEC_UINT32 If context values are not correct [SEC_CRYPT_ERR_INVALID_CTX]
 * @retval SEC_UINT32 If arguments are missing [SEC_ERR_INVALID_ARG]
 *
 * @par Required Header File
 * sec_crypto_safe.h
 *
 * @par Note
 * - pucMAC buffer length should be more or equal to the value returned by the API CRYPT_HMAC_size.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_hmacFinal_sf(CRYPT_CTX *pCtx, SEC_UCHAR *pucMAC, SEC_UINT32 *pulMACLen);

/*
    Func Name:  CRYPT_hmac_sf
*/
/**
* @defgroup CRYPT_hmac_sf
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_hmac_sf (
* SEC_UINT32 ulAlgType,
* const SEC_UCHAR * pucKey,
* SEC_UINT32 ulKeyLen,
* const SEC_UCHAR * pucData,
* SEC_UINT32 ulDataLen,
* SEC_UCHAR * pucMAC,
* SEC_UINT32 * pulMACLen);
* @endcode
*
* @par Purpose
* This is used to compute hmac of a given data block.
*
* @par Description
* CRYPT_hmac_sf function computes hmac of a given data block.
Calls init, update, and final. This function is used when
* data is present all at once. There is no need of calling
Init, Update, Final and MAC can be calculated in one go.
* Context is not needed here.
*
* @param[in] ulAlgType HMAC algorithm
* @param[in] pucKey The key
* @param[in] ulKeyLen The key length
* @param[in] pucData The data
* @param[in] ulDataLen The data length
* @param[out] pucMAC The hashed MAC
* @param[in] pulMACLen The length of input buffer (pucMAC)
* @param[out] pulMACLen The digest length
*
* @retval SEC_UINT32 If digest is calculated [SEC_SUCCESS]
* @retval SEC_UINT32 If context values are not correct [SEC_CRYPT_ERR_INVALID_CTX]
* @retval SEC_UINT32 If arguments are missing [SEC_ERR_INVALID_ARG]
* @retval SEC_UINT32 If memory alloc for context failed [SEC_ERR_MALLOC_FAIL]
* @retval SEC_UINT32 If algorithm undefined [SEC_ERR_INVALID_ALGID]
*
* @par Required Header File
* sec_crypto_safe.h
*
* @par Note
* - pucMAC buffer length should be more or equal to the value returned by the API CRYPT_HMAC_size.
* - API supports following unsafe algorithms:
*   - ALGID_HMAC_MD4 (CID_HMAC_MD4)
*   - ALGID_HMAC_MD5 (CID_HMAC_MD5)
*   - ALGID_HMAC_SHA1(CID_HMAC_SHA1) \n
*   As these algorithms are unsafe, if application are using these algorithms, application must take care of any
*   security issues that may happen due to usage of these algorithms.
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_hmac_sf(SEC_UINT32 ulAlgType, const SEC_UCHAR *pucKey, SEC_UINT32 ulKeyLen,
    const SEC_UCHAR *pucData, SEC_UINT32 ulDataLen, SEC_UCHAR *pucMAC, SEC_UINT32 *pulMACLen);

/*
    Func Name:  CRYPT_cmacFinal_sf
*/
/**
 * @defgroup CRYPT_cmacFinal_sf
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_cmacFinal_sf (
 * CRYPT_CTX* pCtx,
 * SEC_UCHAR* pucMAC,
 * SEC_UINT32* pulMACLen);
 * @endcode
 *
 * @par Purpose
 * This is used to complete cmac operation on remaining data. Called at the end of MAC operation.
 *
 * @par Description
 * CRYPT_cmacFinal_sf function completes cmac operation on remaining data. It is called at the end
 * of MAC operation. A CRYPT_cmacInit must have been called before calling this function.
 * This function calculates the MAC and frees the context. The memory
 * for MAC must have already been allocated.
 *
 * @param[in] pCtx The context for digest
 * @param[out] pucMAC The hashed MAC
 * @param[in] pulMACLen Input buffer (pucMAC) length
 * @param[out] pulMACLen The MAC length
 *
 * @retval SEC_UINT32 If digest is calculated [SEC_SUCCESS]
 * @retval SEC_UINT32 If context values are not correct [SEC_CRYPT_ERR_INVALID_CTX]
 * @retval SEC_UINT32 If arguments are missing/incorrect [SEC_ERR_INVALID_ARG]
 *
 * @par Required Header File
 * sec_crypto_safe.h
 *
 * @par Note
 * - pucMAC buffer length should be more or equal to the value returned by the API CRYPT_CMAC_size.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_cmacFinal_sf(CRYPT_CTX *pCtx, SEC_UCHAR *pucMAC, SEC_UINT32 *pulMACLen);

/*
    Func Name:  CRYPT_cmac_sf
*/
/**
* @defgroup CRYPT_cmac_sf
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_cmac_sf (
* SEC_UINT32 ulAlgType,
* const SEC_UCHAR* pucKey,
* SEC_UINT32  ulKeyLen,
* const SEC_UCHAR*  pucData,
* SEC_UINT32  ulDataLen,
* SEC_UCHAR*  pucMAC,
* SEC_UINT32* pulMACLen);
* @endcode
*
* @par Purpose
* This is used to compute cmac of a given data block.
*
* @par Description
* CRYPT_cmac_sf function computes cmac  of a given data block.
Calls init, update, and final. This function is used when
* data is present all at once. There is no need of calling
Init, Update, Final and MAC can be calculated in one go.
* Context is not needed here.
*
* @param[in] ulAlgType CMAC algorithm
* @param[in] pucKey The key
* @param[in] ulKeyLen The key length
* @param[in] pucData The data
* @param[in] ulDataLen The data length
* @param[out] pucMAC The hashed MAC
* @param[in] pulMACLen Input buffer (pucMAC) length
* @param[out] pulMACLen The digest length
*
* @retval SEC_UINT32 If digest is calculated [SEC_SUCCESS]
* @retval SEC_UINT32 If context values are not correct
[SEC_CRYPT_ERR_INVALID_CTX]
* @retval SEC_UINT32 If arguments are missing [SEC_ERR_INVALID_ARG]
* @retval SEC_UINT32 If memory alloc for context
failed [SEC_ERR_MALLOC_FAIL]
* @retval SEC_UINT32 If algorithm undefined [SEC_ERR_INVALID_ALGID]
* @retval SEC_UINT32 If the key length is invalid
[SEC_ERR_INVALID_KEY_LEN]
*
* @par Required Header File
* sec_crypto_safe.h
*
* @par Note
* - pucMAC buffer length should be more or equal to the value returned by the API CRYPT_CMAC_size.
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_cmac_sf(SEC_UINT32 ulAlgType, const SEC_UCHAR *pucKey, SEC_UINT32 ulKeyLen,
    const SEC_UCHAR *pucData, SEC_UINT32 ulDataLen, SEC_UCHAR *pucMAC, SEC_UINT32 *pulMACLen);

/*
    Func Name:  CRYPT_xcbcFinal_sf
*/
/**
* @defgroup CRYPT_xcbcFinal_sf
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_xcbcFinal_sf (
*                           CRYPT_CTX* pCtx,
*                           SEC_UCHAR* pucMAC,
*                           SEC_UINT32* pulMACLen);
* @endcode
*
* @par Purpose
* This is used to complete xcbc MAC operation on remaining data.
* Called at the end of MAC operation.
*
* @par Description
* CRYPT_xcbcFinal_sf function completes xcbc MAC operation on
* remaining data. It is called at the end of MAC operation. A CRYPT_xcbcInit
* must have been called before calling this function.
* This function calculates the MAC and frees the context. The memory
* for MAC must have already been allocated.
*
* @param[in] pCtx The context for xcbc mac
* @param[out] pucMAC The XCBC MAC
* @param[in] pulMACLen The length of input buffer (pucMAC)
* @param[out] pulMACLen The MAC length
*

* @retval SEC_UINT32 If MAC is calculated [SEC_SUCCESS]
* @retval SEC_UINT32 If context values are not correct [SEC_CRYPT_ERR_INVALID_CTX]
* @retval SEC_UINT32 If arguments are missing [SEC_ERR_INVALID_ARG]
*
* @par Required Header File
* sec_crypto_safe.h
*
* @par Note
* - pucMAC buffer length should be more or equal to the value returned by the API CRYPT_XCBC_size.
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_xcbcFinal_sf(CRYPT_CTX *pCtx, SEC_UCHAR *pucMAC, SEC_UINT32 *pulMACLen);

/*
    Func Name:  CRYPT_xcbc_sf
*/
/**
* @defgroup CRYPT_xcbc_sf
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_xcbc_sf (SEC_UINT32 ulAlgType,
*                        const SEC_UCHAR* pucKey,
*                        SEC_UINT32 upucKeyLen,
*                        const SEC_UCHAR * pucData,
*                        SEC_UINT32 ulDataLen,
*                        SEC_UCHAR * pucMAC,
*                        SEC_UINT32 * pulMACLen);

* @endcode
*
* @par Purpose
* This is used to compute aes xcbc mac of a given data block.
*
* @par Description
* CRYPT_xcbc_sf function computes aes xcbc mac of a given data
* block. Calls init, update, and final. This function is used when
* data is present all at once. There is no need of calling
* Init, Update, Final and XCBC MAC can be calculated in one go.
* Context is not needed here. The MAC generated will be of length 128 bits.
*
* @param[in] ulAlgType XCBC MAC algorithm
* @param[in] pucKey The key
* @param[in] upucKeyLen The key length
* @param[in] pucData The data
* @param[in] ulDataLen The data length
* @param[out] pucMAC The XCBC MAC
* @param[in] pulMACLen The length of input buffer (pucMAC)
* @param[out] pulMACLen The MAC length
*
* @retval SEC_UINT32 If MAC is calculated [SEC_SUCCESS]
* @retval SEC_UINT32 If context values are not correct [SEC_CRYPT_ERR_INVALID_CTX]
* @retval SEC_UINT32 If arguments are missing [SEC_ERR_INVALID_ARG]
* @retval SEC_UINT32 If memory alloc for context failed [SEC_ERR_MALLOC_FAIL]
* @retval SEC_UINT32 If algorithm undefined [SEC_ERR_INVALID_ALGID]
* @retval SEC_UINT32 If the key length is invalid [SEC_ERR_INVALID_KEY_LEN]
*
* @par Required Header File
* sec_crypto_safe.h
*
* @par Note
* \n
* Only AES algorithm is supported.
* This API can be used for AES-XCBC-MAC-96 algorithm. It will
* generate XCBC MAC of 128 bits. Leftmost 96 bits will be taken for
* XCBC-MAC-96
*
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_xcbc_sf(SEC_UINT32 ulAlgType, const SEC_UCHAR *pucKey, SEC_UINT32 upucKeyLen,
    const SEC_UCHAR *pucData, SEC_UINT32 ulDataLen, SEC_UCHAR *pucMAC, SEC_UINT32 *pulMACLen);

/*
    Func Name:  CRYPT_genKey_sf
*/
/**
* @defgroup CRYPT_genKey_sf
* @ingroup HLF High Level Functions
* @par Prototype
* @code
* SEC_UINT32 CRYPT_genKey_sf(
* SEC_UINT32 ulAlgId,
* SEC_UCHAR* pucKey,
* SEC_UINT32 uiKeyLen)
* @endcode
*
* @par Purpose
* This is used to generate a random key for the given symmetric algorithm.
*
* @par Description
* CRYPT_genKey function generates a random key for the
given symmetric algorithm. The memory for the key must be allocated
* before calling this function. For RC4 algorithm, the
* keylength will be RC4_DEFAULT_KEYLEN. For RC2 algorithm, the keylength
* will be RC2_DEFAULT_KEYLEN. For other symmetric
* algorithms,it will be their respective keylength.
*
*
* @param[in] ulAlgId Symmetric algorithm
* @param[out] pucKey Generated random key
* @param[in] uiKeyLen length of pucKey buffer
*
* @retval SEC_UINT32 Invalid Symmetric Algorithm Id [SEC_ERR_INVALID_ALGID]
* @retval SEC_UINT32 On successful return [SEC_SUCCESS]
*
* @par Required Header File
* sec_crypto_safe.h
*
* @par Note
* \n
* - If application does not set seeds and entropy on frequent intervals, it can lead to vulnerable keys or random
*   value. It is recommended to set seeds and entropy on frequent intervals or update the version that migrates the
*   random implementation based on DRBG.
* - Before calling this API, please make sure that DRBG is initialized for random number generation by calling
*   IPSI_CRYPT_rand_init. If application don't want to use DRBG then appplication should call
*   IPSI_CRYPT_enable_drbg(0) to disable the DRBG Random feature.
* - DRBG is enabled by default.
* - API supports following unsafe algorithms:
*   - ALGID_RC4 (CID_RC4)
*   - ALGID_DES_ECB (CID_DES_ECB)
*   - ALGID_DES_CBC (CID_DES_CBC)
*   - ALGID_DES_OFB (CID_DES_OFB)
*   - ALGID_DES_CFB (CID_DES_CFB)
*   - ALGID_DES_EDE_ECB (CID_DES_EDE_ECB)
*   - ALGID_DES_EDE_CBC (CID_DES_EDE_CBC)
*   - ALGID_DES_EDE_OFB (CID_DES_EDE_OFB)
*   - ALGID_DES_EDE_CFB (CID_DES_EDE_CFB)
*   - ALGID_DES_EDE3_ECB (CID_DES_EDE3_ECB)
*   - ALGID_DES_EDE3_CBC (CID_DES_EDE3_CBC)
*   - ALGID_DES_EDE3_OFB (CID_DES_EDE3_OFB)
*   - ALGID_DES_EDE3_CFB (CID_DES_EDE3_CFB)
*   - ALGID_KASUMI_ECB (CID_KASUMI_ECB)
*   - ALGID_KASUMI_CBC (CID_KASUMI_CBC)
*   - ALGID_KASUMI_OFB (CID_KASUMI_OFB)
*   - ALGID_KASUMI_CFB (CID_KASUMI_CFB)
*   - ALGID_BF_ECB (CID_BF_ECB)
*   - ALGID_BF_CBC (CID_BF_CBC)
*   - ALGID_BF_CFB (CID_BF_CFB)
*   - ALGID_BF_OFB (CID_BF_OFB)
*   - ALGID_RC2_ECB (CID_RC2_ECB)
*   - ALGID_RC2_CBC (CID_RC2_CBC)
*   - ALGID_RC2_OFB (CID_RC2_OFB)
*   - ALGID_RC2_CFB (CID_RC2_CFB) \n
*   As these algorithms are unsafe, if application are using these algorithms, application must take care of any
*   security issues that may happen due to usage of these algorithms.
* @par Related Topics
* N/A
*/
CRYPTOLINKDLL SEC_UINT32 CRYPT_genKey_sf(SEC_UINT32 ulAlgId, SEC_UCHAR *pucKey, SEC_UINT32 uiKeyLen);

/*
 Func Name:  CRYPT_keyWrap_sf
*/
/**
 * @defgroup CRYPT_keyWrap_sf
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_BOOL CRYPT_keyWrap_sf(
 * SEC_UCHAR* ciphertxt,
 * SEC_UINT32 uiCiphertxtLen,
 * const SEC_UCHAR* puckey,
 * SEC_UINT32 ulKlen,
 * const SEC_UCHAR* plaintext,
 * SEC_UINT32 plaintextLen,
 * const SEC_UCHAR* iv,
 * SEC_UINT32 npt);
 * @endcode
 *
 * @par Purpose
 * This is used to generate an encrypted key.
 *
 * @par Description
 * CRYPT_keyWrap_sf function generats an encrypted key using an original key.
 *
 * @param[in] puckey Pointer to the key
 * @param[in] ulKlen The size of key in octets. The size  must be 16,24 or 32
 * @param[in] plaintext Pointer to the plain text
 * @param[in] plaintextLen Length of the input buffer (plaintext)
 * @param[in] iv Pointer to initial value
 * @param[in] npt A block number of key data plain text
 * @param[in] uiCiphertxtLen Length of the input buffer (ciphertxt)
 * @param[out] ciphertxt Pointer to the cipher text
 *
 * @retval SEC_BOOL On failure [SEC_FALSE]
 * @retval SEC_BOOL On success [SEC_TRUE]
 *
 * @par Required Header File
 * sec_crypto_safe.h
 *
 * @par Note
 * 1. The length of the plaintext and ciphertext must be the multiple of 8 byte blocks,
 * whereas for (n) blocks of plaintext there must be (n+1) blocks of ciphertext. \n
 * 2. The npt parameter value must not exceed 0xfffffff value,otherwise it will fail to
 * generate an encrypted key.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_BOOL CRYPT_keyWrap_sf(SEC_UCHAR *ciphertxt, SEC_UINT32 uiCiphertxtLen, const SEC_UCHAR *puckey,
    SEC_UINT32 ulKlen, const SEC_UCHAR *plaintext, SEC_UINT32 plaintextLen, const SEC_UCHAR *iv, SEC_UINT32 npt);

/*
 Func Name:  CRYPT_keyUnwrap_sf
*/
/**
 * @defgroup CRYPT_keyUnwrap_sf
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_BOOL CRYPT_keyUnwrap_sf(
 * SEC_UCHAR* plaintext,
 * EC_UINT32 plaintextLen,
 * const SEC_UCHAR* puckey,
 * SEC_UINT32 ulKlen,
 * const SEC_UCHAR* ciphertxt,
 * SEC_UINT32 uiCiphertxtLen,
 * const SEC_UCHAR* iv,
 * SEC_UINT32 npt);
 * @endcode
 *
 * @par Purpose
 * This is used to generate an original key using an encrypted key.
 *
 * @par Description
 * CRYPT_keyUnwrap_sf function generats an original key using an encrypted key.
 *
 * @param[in] puckey Pointer to the key
 * @param[in] ulKlen The size of key in octets. The size must be 16,24 or 32
 * @param[in] ciphertxt Pointer to the cipher text
 * @param[in] uiCiphertxtLen Length of the input buffer (ciphertxt)
 * @param[in] iv Pointer to initial value
 * @param[in] npt A block number of key data plain text
 * @param[out] plaintext Pointer to the plain text
 * @param[in] plaintextLen Length of the input buffer (plaintext)
 *
 * @retval SEC_BOOL On failure [SEC_FALSE]
 * @retval SEC_BOOL On success [SEC_TRUE]
 *
 * @par Required Header File
 * sec_crypto_safe.h
 *
 * @par Note
 * 1. The length of the plaintext and ciphertext must be the multiple of 8 byte blocks,
 * whereas for (n) blocks of plaintext there must be (n+1) blocks of ciphertext. \n
 * 2. The npt parameter value must not exceed 0xfffffff value,otherwise it will fail to
 * generate an original key using an excrypted.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_BOOL CRYPT_keyUnwrap_sf(SEC_UCHAR *plaintext, SEC_UINT32 plaintextLen, const SEC_UCHAR *puckey,
    SEC_UINT32 ulKlen, const SEC_UCHAR *ciphertxt, SEC_UINT32 uiCiphertxtLen, const SEC_UCHAR *iv, SEC_UINT32 npt);

/*
    Func Name:  CRYPT_aeadOp_sf
*/
/**
 * @defgroup CRYPT_aeadOp_sf
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * SEC_UINT32 CRYPT_aeadOp_sf(
 * const IPSI_AEAD_OP_DATA_S *pstOpData,
 * const SEC_UCHAR *pucInData,
 * size_t ulInDataLen,
 * SEC_UCHAR *pucOutData,
 * size_t *pulOutDataLen,
 * SEC_UCHAR *pucHashResult,
 * SEC_UINT32 ulHashResultLen,
 * IPSI_AUX_PAR_S *pstAuxPar);
 * @endcode
 *
 * @par Purpose
 * Used to do encryption / decryption operation on AEAD algorithms.
 *
 * @par Description
 * CRYPT_aeadOp_sf is safe version of CRYPT_aeadOp and it is used to do encryption / decryption operation.
 * In case of encryption, both encrypted data and authentication tag will be generated by this function. In case
 * of decryption, it takes encrypted data, authentication tag as input and outputs decrypted
 * text only when both decryption and tag verification are success.
 *
 * @param[in]  pstOpData Pointer to AEAD operation data which holds information
 * required for encryption / decryption [N/A]
 * @param[in]  pucInData Pointer to input data for encryption / decryption operation [N/A]
 * @param[in]  ulInDataLen Input data length [N/A]
 * @param[out]  pucOutData Pointer to output data. The buffer size should be atleast equal to ulInDataLen [N/A]
 * @param[in]  pulOutDataLen The output buffer (pucOutData) size  [N/A]
 * @param[out]  pulOutDataLen Pointer to output data length  [N/A]
 * @param[in]  pucHashResult Pointer to hold authentication tag in case of encryption and input
 * authentication tag in case of decryption[N/A]
 * @param[in]  ulHashResultLen Desired authentication tag length [N/A]
 * @param[in]  pstAuxPar Pass as SEC_NULL. Additional variable for future extensibility [N/A]
 *
 * @retval SEC_UINT32 Invalid argument [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_UINT32 Invalid IV pointer / Invalid IV length [SEC_ERR_INVALID_IV_LEN|N/A]
 * @retval SEC_UINT32 Invalid tag length [SEC_CRYPT_ERR_INVALID_TAG_LEN|N/A]
 * @retval SEC_UINT32 IV length exceeds maximum limit [SEC_CRYPT_ERR_IV_LEN_EXCEED_MAXLIMIT|N/A]
 * @retval SEC_UINT32 AAD length exceeds maximum limit [SEC_CRYPT_ERR_AAD_LEN_EXCEED_MAXLIMIT|N/A]
 * @retval SEC_UINT32 Trying to set AAD after encryption/decryption operation
 * [SEC_CRYPT_ERR_SET_AAD_AFTER_CIPHER_OPERATION|N/A]
 * @retval SEC_UINT32 Set IV failed [SEC_CRYPT_ERR_AEAD_SET_IV_FAILED|N/A]
 * @retval SEC_UINT32 Data for encryption/decryption exceeds maximum limit [SEC_CRYPT_ERR_LARGE_DATA_SIZE|N/A]
 * @retval SEC_UINT32 AEAD decryption operation failure / tag verification failure [SEC_ERR|N/A]
 *
 *
 * @par Required Header File
 * sec_crypto_safe.h
 *
 * @par Note
 *
 * For CCM
 * - Supported lengths for authentication tag:
 * 4 bytes, 6 bytes, 8 bytes, 10 bytes, 12 bytes, 14 bytes, 16 bytes.The tag length required should be passed by user.
 * - The output buffer to hold cipher text/decrypted text should be allocated by user. As per AES-CCM standard,
 * cipher text will contain the encrypted data + tag.So for encryption the output buffer should contain
 * the memory atleast (input lengh + desired tag length).
 * - On encryption success, output data length will be input length + tag length.
 * - On decryption success, output data length will be input length - tag length when data length is more
 * than tag length. In decryption, When cipher data is only tag (which means there is no data to be decrypted,
 * but only tag needs to be verified), user still needs to pass non-NULL buffer for the output data.
 * In case of only tag needs to be verified, the output length will not be updated.
 * - The type support for encryption/decryption is only IPSI_SYM_DATA_TYPE_FULL. If passed other types,
 * API will return error.
 *
 * For GCM
 * - Supported lengths for authentication tag : \n
 * 4 bytes, 8 bytes, 12 bytes, 13 bytes, 14 bytes, 15 bytes, 16 bytes. The tag length should be passed by
 * user and the buffer to hold output tag in case of encryption should be allocated by user.
 *
 * - The output buffer to hold cipher text / decrypted text should be allocated by user. The buffer should
 * contain memory of input length passed.
 * - On encryption / decryption success, output data length will be same as input data length.
 * - The data size need not be multiple of block size (block size of AES GCM is 16 bytes).
 * - If data is being encrypted/decrypted as partial data with IPSI_SYM_DATA_TYPE_PARTIAL type,
 * - IV should be set for the first time only. From the second time, even if IV are passed, they will be ignored.
 * - AAD can be set multiple times. But AAD should be set before sending data for encryption/decryption.
 * - If AAD is set after some encryption/decryption operation, API fails. In this failure case, context can not
 * be reused for further operations.
 * if  set AAD after doing partial encryption/decryption operation, error SEC_CRYPT_ERR_SET_AAD_AFTER_CIPHER_OPERATION
 * will be returned.
 * - The tag & tag length will be validated only if passed IPSI_SYM_DATA_TYPE_LAST_PARTIAL & IPSI_SYM_DATA_TYPE_FULL
 * type. These will be ignored when passed type as IPSI_SYM_DATA_TYPE_PARTIAL.
 * - As per NIST SP800-38D (AES-GCM) standard, decrypted output should be returned to user only when authentication(tag)
 * verification is successful. But when used IPSI_SYM_DATA_TYPE_PARTIAL type, API will return the output data.
 * So, application should check the return value of API when processed with IPSI_SYM_DATA_TYPE_LAST_PARTIAL type,
 * if it is success only, should consider the decrypted data & use it for application operations.
 * - The maximum lengths supported as per SP800-38D standard are :
 * (a) For AAD, length of AAD is less than or equal to (2^64 - 1) in bits.
 * In bytes, it should be less than or equal to 2^61.
 * (b) For IV, length of IV is (1<= len(IV) <= (2^64 - 1)) in bits.
 * In bytes, it should be less than or equal to 2^61.
 * (c) For Data to be encrypted / decrypted, length should be <= 2^39-256 in bits.
 * In bytes, it should be less than or equal to (2^36 - 32).
 * (d) For AAD, IV & length to encrypted / decrypted, the data types are "size_t".
 * So, on 32 bit systems, they support maximum length of size_t (2^32-1), on 64 bit systems,
 * they support maximum length of size_t (2^64 - 1).
 * - When passed the input length to be encrypted / decrypted as zero, no output pointers will be updated.
 * - SEC_ERR_INVALID_ARG will be returned if:
 * - Context is NULL.
 * - If input data length is not zero and any of the pointer of input data, output data, output data length is NULL.
 * - AAD pointer is NULL and AAD length is not zero.
 * - Pointer to tag is NULL.
 * - Invalid data type (other than IPSI_SYM_DATA_TYPE_FULL, IPSI_SYM_DATA_TYPE_PARTIAL,
 * IPSI_SYM_DATA_TYPE_LAST_PARTIAL).
 * - IV is NULL.
 * - Pointer to IPSI_AEAD_OP_DATA_S is NULL.
 * - The maximum value of ulInDataLen can be 512MB.
 *
 * @par Related Topics
 * CRYPT_aeadInitSession \n
 * CRYPT_aeadRemoveSession
 */
CRYPTOLINKDLL SEC_UINT32 CRYPT_aeadOp_sf(const IPSI_AEAD_OP_DATA_S *pstOpData, const SEC_UCHAR *pucInData,
    size_t ulInDataLen, SEC_UCHAR *pucOutData, size_t *pulOutDataLen, SEC_UCHAR *pucHashResult,
    SEC_UINT32 ulHashResultLen, IPSI_AUX_PAR_S *pstAuxPar);

#ifdef __cplusplus
}
#endif

#endif /* _IPSI_SEC_CRYPTO_SAFE_H */
