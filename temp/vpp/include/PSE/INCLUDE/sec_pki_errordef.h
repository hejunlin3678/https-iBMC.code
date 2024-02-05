/* *****************************************
 * Copyright (c) Huawei Technologies Co., Ltd. 2009-2019. All rights reserved.
FileName: sec_pki_errordef.h
Version: 1
Author:  Mallesh

Description: This file declares the error
 * Create: 2009-4-24
codes
******************************************** */
#ifndef SEC_PKI_ERRORDEF_H
#define SEC_PKI_ERRORDEF_H

#include "ipsi_pse_build_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SEC_PKI_ERR_RANGE 1000

#define SEC_PKI_ERR_INIT 1000

// System Error: Include mem allocation failure,socket  closed ,
// file open error......
#define SEC_PKI_ERR_SYSTEM_INIT (SEC_PKI_ERR_INIT + 1)

// MAX Total System error limit
#define SEC_PKI_ERR_SYSTEM_LIMIT (SEC_PKI_ERR_SYSTEM_INIT + SEC_PKI_ERR_RANGE - 1)

#define SEC_PKI_ERR_CONTEXT_OBJECT_INIT (SEC_PKI_ERR_SYSTEM_LIMIT + 1)

// MAX Total Context error limit
#define SEC_PKI_ERR_CONTEXT_OBJECT_LIMIT (SEC_PKI_ERR_CONTEXT_OBJECT_INIT + SEC_PKI_ERR_RANGE - 1)

#define SEC_PKI_ERR_COMMON_INIT (SEC_PKI_ERR_CONTEXT_OBJECT_LIMIT + 1)

// MAX Total Common error limit
#define SEC_PKI_ERR_COMMON_LIMIT (SEC_PKI_ERR_COMMON_INIT + SEC_PKI_ERR_RANGE - 1)

#define SEC_PKI_ERR_LOAD_CERT_KEY_INIT (SEC_PKI_ERR_COMMON_LIMIT + 1)

// MAX Total cert error limit
#define SEC_PKI_ERR_LOAD_CERT_KEY_LIMIT (SEC_PKI_ERR_LOAD_CERT_KEY_INIT + SEC_PKI_ERR_RANGE - 1)

#define SEC_PKI_ERR_CRL_INIT (SEC_PKI_ERR_LOAD_CERT_KEY_LIMIT + 1)

// Max Total crl error limit
#define SEC_PKI_ERR_CRL_LIMIT (SEC_PKI_ERR_CRL_INIT + SEC_PKI_ERR_RANGE - 1)

#define SEC_PKI_ERR_OCSP_INIT (SEC_PKI_ERR_CRL_LIMIT + 1)

// Max Total ocsp error limit
#define SEC_PKI_ERR_OCSP_LIMIT (SEC_PKI_ERR_OCSP_INIT + SEC_PKI_ERR_RANGE - 1)

// Syetem errors
typedef enum SEC_PKI_system_errors {
    // Malloc fails
    SEC_PKI_ERR_MALLOC_FAILED = SEC_PKI_ERR_SYSTEM_INIT,

    // crypt memory allocation fails
    SEC_PKI_ERR_CRYPT_NEW_FAILED,

    // crypt copy fails
    SEC_PKI_ERR_CRYPT_COPY_FAILED,

    // crypt copy param fails
    SEC_PKI_ERR_CRYPT_COPY_PARAM_FAILED,

    // Realloc fails
    SEC_PKI_ERR_REALLOC_FAILED,

    SEC_PKI_ERR_MEMCPY_FAILED,

    SEC_PKI_ERR_MEMSET_FAILED,

    SEC_PKI_ERR_MEMMOVE_FAILED,

    // crypt random generation failed
    SEC_PKI_ERR_RANDOM_NUMBER_GEN_FAILED,

    // Please add all error between init and max
    SEC_PKI_ERR_SYSTEM_MAX
} SEC_PKI_SYSTEM_ERRORS;

// enum to  hold the context and object errors
typedef enum SEC_PKI_context_object_errors {
    // create local store fail
    SEC_PKI_ERR_LOCALSTORE_CREATION_FAILED = SEC_PKI_ERR_CONTEXT_OBJECT_INIT,

    // ctx/obj name is not set
    SEC_PKI_ERR_NAME_NOT_SET,

    // buf length is not sufficient
    SEC_PKI_ERR_BUF_LEN_NOT_SUFFICIENT,

    // Creation of SEC_List_S Failed
    SEC_PKI_ERR_CREATE_LIST_FAILED,

    // Add to SEC_List_S Failed
    SEC_PKI_ERR_ADD_TO_LIST_FAILED,

    // cert chain validate fail
    SEC_PKI_ERR_VALIDATE_CERT_CHAIN,

    // In use cert not set in the object
    SEC_PKI_ERR_INUSE_CERT_NOT_SET,

    // In use cert not set in the object
    SEC_PKI_ERR_VERIFY_DEPTH_NOT_SET,

    // Local store copy failed
    SEC_PKI_ERR_LOCALSTORE_COPY_FAILED,

    // copy preshared cert list failed
    SEC_PKI_ERR_PRESHARED_CERT_LIST_COPY_FAILED,

    SEC_PKI_ERR_CONTEXT_OBJECT_MAX
} SEC_PKI_CONTEXT_ERRORS_S;

// enum to  hold the context and object errors
typedef enum SEC_PKI_common_errors {
    // Invalid arguments
    SEC_PKI_ERR_INVALID_ARGS = SEC_PKI_ERR_COMMON_INIT,

    // Input time is invalid (or) not supported
    SEC_PKI_ERR_INVALID_TIME,

    // No flag is set
    SEC_PKI_ERR_FLAG_NOT_SET,

    // Time is not set
    SEC_PKI_ERR_TIME_NOT_SET,

    // create lock fail
    SEC_PKI_ERR_CTX_LOCK_CREATION_FAILED,

    // Invalid flag value
    SEC_PKI_ERR_INVALID_FLAG,

    // certificate encoding failed
    SEC_PKI_ERR_CERT_ENCODING_FAILED,

    // certificate decoding failed
    SEC_PKI_ERR_CERT_DECODING_FAILED,

    // Unable to get algorithm id from certificate
    SEC_PKI_ERR_UNABLE_TO_GET_ALG_ID,

    // Unable to add certificate list to output chain
    SEC_PKI_ERR_UNABLE_TO_ADD_CERT_LIST,

    // Failed to create list
    SEC_PKI_ERR_FAILED_TO_CREATE_LIST,

    // Duplicate certificate creation failed
    SEC_PKI_ERR_DUP_CERT_CREATION_FAILED,

    // Adding certificate to the list failed
    SEC_PKI_ERR_ADD_CERT_TO_LIST_FAILED,

    // Adding certificate list to chain failed
    SEC_PKI_ERR_ADD_CERT_LIST_TO_CHAIN_FAILED,

    // No matching certificate chain found
    SEC_PKI_ERR_NO_MATCH_CERT_CHAIN,

    // Invalid input certificate chain list
    SEC_PKI_ERR_INVALID_LIST,

    // Not an end entity certificate
    SEC_PKI_ERR_NOT_END_ENTITY_CERT,

    // No matching hash found
    SEC_PKI_ERR_NO_MATCHING_HASH,

    // Failed to add to list
    SEC_PKI_ERR_FAILED_TO_ADD_TO_LIST,

    // Encoding of the subject name failed
    SEC_PKI_ERR_ENCODE_SUBJECT_NAME_FAILED,

    // CA request list is empty
    SEC_PKI_ERR_CA_REQ_LIST_EMPTY,

    // No issuer found
    SEC_PKI_NO_ISSUER_FOUND,

    // No matching subject name found
    SEC_PKI_ERR_NO_MATCHING_SUB_NAME,

    // Decoding of Name Failed
    SEC_PKI_ERR_DECODE_NAME_FAILED,

    // No end entity certificate found
    SEC_PKI_ERR_FAILED_TO_FIND_END_ENTITY_CERT,

    // Certificate chain could not formed
    SEC_PKI_ERR_CERT_CHAIN_COULD_NOT_BE_FORMED,

    // End entity list could not be formed
    SEC_PKI_ERR_END_ENTITY_LIST_COULD_NOT_BE_FORMED,

    // No local certificates found in the store
    SEC_PKI_ERR_NO_LOCAL_CERTS,

    // Creation of extended certificate failed
    SEC_PKI_ERR_EXTENDED_CERT_CREATION_FAILED,

    // Verification in terms of time failed.
    SEC_PKI_ERR_VALIDITY_CHECK_FAILED,

    // Time verification of one certificate failed
    SEC_PKI_ERR_CERT_VERIFY_FAILED,

    // Time verification of crl failed
    SEC_PKI_ERR_CRL_VERIFY_FAILED,

    // Getting the system time failed
    SEC_PKI_ERR_SYS_TIME_FAILED,

    // Cross CA not found
    SEC_PKI_ERR_CROSS_CA_NOT_FOUND,

    // No matching hash found in cache
    SEC_PKI_NO_MATCHING_HASH_FOUND,

    // Issuer and Subject name not matching
    SEC_PKI_ERR_ISSUER_SUBJECT_NAME_MISMATCH,

    // Peer cache is empty
    SEC_PKI_ERR_PEER_CACHE_NO_CERT,

    // No pre shared default cert found in the preshared peer store
    SEC_PKI_ERR_NO_PRE_DHARED_DFLT_CERT,

    // No preshared certificates found in the store
    SEC_PKI_ERR_NO_PRE_SHARED_CERTS,

    // The Object Lock creation failed
    SEC_PKI_ERR_OBJ_LOCK_CREATION_FAILED,

    // 无效的数字信封格式
    SEC_PKI_ERR_ENVELOPED_FORMAT_ERR,
    // 数字信封解密失败
    SEC_PKI_ERR_ENVELOPED_DECRYPT_FAILED,
    // 信封密钥对不匹配
    SEC_PKI_ERR_ENVELOPED_PKEY_NOT_MATCH_ERR,

    SEC_PKI_ERR_COMMON_MAX
} SEC_PKI_COMMON_ERRORS_S;

// enum to  hold the certificate errors
typedef enum SEC_PKI_cert_errors {
    // decode failed
    SEC_PKI_ERR_DECODE_FAILED = SEC_PKI_ERR_LOAD_CERT_KEY_INIT,

    // store certificate failed
    SEC_PKI_ERR_STORE_CERTIFICATE_FAILED,

    // extract public key failed
    SEC_PKI_ERR_EXTRACT_PUBKEY_FAILED,

    // Unknown certificate type
    SEC_PKI_ERR_UNKNOWN_CERTIFICATE_TYPE,

    // no certificate
    SEC_PKI_ERR_NO_CERTIFICATE,

    // load from certificate buffer failed
    SEC_PKI_ERR_LOAD_CERT_BUFFER_FAILED,

    // load from certificate file failed
    SEC_PKI_ERR_LOAD_CERT_FILE_FAILED,

    // decode PFX failed
    SEC_PKI_ERR_DECODE_PFX_FAILED,

    // decrypt auth safe failed
    SEC_PKI_ERR_DECRYPT_AUTH_SAFE_FAILED,

    // Get bag from bag list failed
    SEC_PKI_ERR_GET_BAG_FROM_BAGLIST_FAILED,

    // Extract cert from safe bag failed
    SEC_PKI_ERR_EXTRCT_CERT_FRM_SAFEBAG_FAILED,

    // Verify PFX failed
    SEC_PKI_ERR_VERIFY_PFX_FAILED,

    // Extract auth safe from PFX failed
    SEC_PKI_ERR_EXTRACT_AUTHSAFE_FROM_PFX_FAILED,

    // Certificate copy failed
    SEC_PKI_ERR_DUP_CERTIFICATE_FAILED,

    // Read from certificate file failed
    SEC_PKI_ERR_READ_CERTIFICATE_FILE_FAILED,

    // Certificate already present
    SEC_PKI_ERR_CERT_ALREADY_PRESENT,

    // Digest Calculation Failed
    SEC_PKI_ERR_DIGEST_CALC_FAILED,

    // Save to Store Map Failed
    SEC_PKI_ERR_SAVE_TO_MAP_FAILED,

    // Insert to Store Map Failed
    SEC_PKI_ERR_INSERT_TO_MAP_FAILED,

    // public key under same issuer name present already
    SEC_PKI_ERR_SAME_PUBKEY_UNDER_ISSUER,

    // subject name under same issuer name present already
    SEC_PKI_ERR_SAME_SUBNAME_UNDER_ISSUER,

    // Certificate Duplication Failed
    SEC_PKI_CERT_DUP_FAILED,

    // Extended Certificate Duplication Failed
    SEC_PKI_ERR_DUPCERTEXTD_FAILED,

    // Get extended certificate from list failed
    SEC_PKI_ERR_GET_EXTENDED_CERT_LIST_FAILED,

    // Key size not supported
    SEC_PKI_ERR_KEY_SIZE_NOT_SUPPORTED,

    // Loading key failed
    SEC_PKI_ERR_LOAD_KEY_FILE_FAILED,

    // Reading key file failed
    SEC_PKI_ERR_READ_KEY_FILE_FAILED,

    // Loading key buffer failed
    SEC_PKI_ERR_LOAD_KEY_BUFFER_FAILED,

    // No private key loaded in the local store of context/object
    SEC_PKI_ERR_NO_PRVKEY,

    // certificate-key pair match failed
    SEC_PKI_ERR_CERT_KEY_MATCH_FAILED,

    // Storing certificate-key pair failed
    SEC_PKI_ERR_STORE_CERT_KEY_FAILED,

    // Input certificate is not matched with any context/object certificates
    SEC_PKI_CERT_NOT_MATCHED,

    // CA certificate already exists
    SEC_PKI_ERR_CA_ALREADY_EXISTS,

    // Loading End entity certificate not allowed in trust store
    SEC_PKI_ERR_END_ENTITY_CERT_NOT_ALLOWED,

    // Hash of the extended certificate failed
    SEC_PKI_ERR_CACHE_EXTEND_CERT_FAIL,

    // Creation of the extended certificate failed
    SEC_PKI_ERR_CREATE_EXTEND_CERT_FAIL,

    // Certificate check failed
    SEC_PKI_ERR_LOAD_CERT_CHECK_FAILED,

    // Adding privatekey in the list failed
    SEC_PKI_ERR_ADD_IN_LIST_FAILED,

    // Loading self sign certificate not allowed in cross CA store
    SEC_PKI_ERR_SELF_SIGN_CERT_NOT_ALLOWED,

    // CA not found
    SEC_PKI_ERR_CA_NOT_FOUND,

    // No CA certificate in trust store
    SEC_PKI_ERR_EMPTY_CA_LIST,

    // No cross CA certificate in trust store
    SEC_PKI_ERR_EMPTY_CROSS_CA_LIST,

    // Extracting certificate extension content failed
    SEC_PKI_ERR_EXTRACT_EXTN_CON_FAIL,

    // Extract CDP failed
    SEC_PKI_ERR_EXTRACT_CDP_FAIL,

    // Extracting certificate extension failed
    SEC_PKI_ERR_EXTRACT_EXTN_FAIL,

    // URI field is empty
    SEC_PKI_ERR_URI_FIELD_EMPTY,

    // Subject Name not in the certificate
    SEC_PKI_ERR_SUBJECT_NAME_NOT_FOUND,

    // Issuer Name not in the certificate
    SEC_PKI_ERR_ISSUER_NAME_NOT_FOUND,

    // Attibute not printable in the screen
    SEC_PKI_ERR_NOT_PRINTABLE_ON_SCREEN,

    // Attribute not found
    SEC_PKI_ERR_ATTIBUTE_NOT_FOUND,

    // Not a CA certificate
    SEC_PKI_ERR_NOT_CA_CERT,

    // Certificate not found
    SEC_PKI_ERR_CERT_NOT_FOUND,

    // No URL loaded in the local store of context/object
    SEC_PKI_ERR_NO_URL,

    // Certificate already loaded in the local store
    SEC_PKI_ERR_CERT_ALREADY_LOADED,

    // Matching certificate not found in the local store
    SEC_PKI_ERR_CERT_MATCH_NOT_FOUND,

    // Certificate signature verification failed
    SEC_PKI_ERR_SIGNATURE_VERIFY_FAILED,

    // Certificate already loaded in the preshared store
    SEC_PKI_ERR_PRESHARED_CERT_ALREADY_LOADED,

    // Encoding the certificate bundle failed
    SEC_PKI_ENCODE_CERT_BUNDLE_FAILED,

    // Decoding the certificate bundle failed
    SEC_PKI_DECODE_CERT_BUNDLE_FAILED,

    // No Certs are found in the bundle
    SEC_PKI_ERR_NO_CERTS_FOUND,

    // Extracting content from extenstion failed
    SEC_PKI_ERR_EXTRACT_CONTENT_FROM_EXTN,

    // Corresponding Private key is not loaded
    SEC_PKI_ERR_PVT_KEY_NOT_LOADED,

    // IA5String content does not match with ASN.1 IA5String standard
    SEC_PKI_ERR_IA5STRING_NOT_MATCH_STANDARD,

    // UTF8String content does not match with ASN.1 UTF8String standard
    SEC_PKI_ERR_UTF8STRING_NOT_MATCH_STANDARD,

    // NumericString content does not match with ASN.1 NumericString standard
    SEC_PKI_ERR_NUMERICSTRING_NOT_MATCH_STANDARD,

    // PrintableString content does not match with ASN.1 PrintableString standard
    SEC_PKI_ERR_PRINTABLESTRING_NOT_MATCH_STANDARD,

    // VisibleString content does not match with ASN.1 VisibleString standard
    SEC_PKI_ERR_VISIBLESTRING_NOT_MATCH_STANDARD,

    SEC_PKI_ERR_LOAD_CERT_KEY_MAX
} SEC_PKI_CERT_ERRORS_S;

// enum to  hold the crl errors
typedef enum SEC_PKI_crl_errors {
    // Reading of CRL file failed
    SEC_PKI_ERR_READ_CRL_FILE_FAILED = SEC_PKI_ERR_CRL_INIT,

    // Loading CRL buffer failed
    SEC_PKI_ERR_LOAD_CRL_BUFFER_FAILED,

    // Decoding of CRL failed
    SEC_PKI_ERR_CRL_DECODE_FAILED,

    // Adding CRL to list failed
    SEC_PKI_ERR_ADD_CRL_TO_LIST_FAILED,

    // Same CRL already exist in CRL store
    SEC_PKI_ERR_CRL_ALREADY_EXIST,

    // Newer CRL already exist in CRL store
    SEC_PKI_ERR_NEWER_CRL_ALREADY_EXIST,

    // CRL time compare failed
    SEC_PKI_ERR_CRL_TIME_CHECK_FAILED,

    // CRL not found
    SEC_PKI_ERR_CRL_NOT_FOUND,

    // Failed to dup CRL
    SEC_PKI_ERR_FAILED_DUP_CRL,

    // No CRL in store
    SEC_PKI_ERR_EMPTY_CRL_LIST,

    // Hash Calculation for CRL failed
    SEC_PKI_ERR_CRL_HASH_CALC_FAILED,

    // No Crls are found in the bundle
    SEC_PKI_ERR_NO_CRLS_FOUND,

    // Base CRL for the cert is not present in the context
    SEC_PKI_ERR_BASE_CRL_FOR_CERT_NOT_PRESENT,

    // Delta CRL for the cert is not present in the context
    SEC_PKI_ERR_DELTA_CRL_FOR_CERT_NOT_PRESENT,

    // Getting Base CRL for the cert failed
    SEC_PKI_ERR_GET_BASE_CRL_FOR_CERT_FAILED,

    // Getting Delta CRL fot the cert failed
    SEC_PKI_ERR_GET_DELTA_CRL_FOR_CERT_FAILED,

    // Maximum pem CRL certificate length
    SEC_PKI_ERR_MAX_PEM_CRL_LENGTH,

    // Max error of CRL
    SEC_PKI_ERR_CRL_MAX
} SEC_PKI_CRL_ERRORS_S;

typedef enum SEC_PKI_ocsp_errors {
    // Validation Error
    // Unable to find the OCSP response
    SEC_PKI_OCSP_ERR_UNABLE_TO_FIND_OCSP_RESPONSE = SEC_PKI_ERR_OCSP_INIT,

    /* The OCSP error when unable to find the OCSP
    responder certificate */
    SEC_PKI_OCSP_ERR_UNABLE_TO_FIND_OCSP_RESPONDER,

    /* The OCSP error when the response is found to be unknown */
    SEC_PKI_OCSP_ERR_UNKNOWN_OCSP_RESPONSE,

    /* The OCSP error when Responder validation failed */
    SEC_PKI_OCSP_ERR_OCSP_RESPONDER_VALIDATION_FAILED,

    /* The OCSP error when Responder validation with CRL failed */
    SEC_PKI_OCSP_ERR_OCSP_RESPONDER_VALIDATION_WITH_CRL_FAILED,

    /* The OCSP error when Responder validation with time failed */
    SEC_PKI_OCSP_ERR_OCSP_RESPONDER_VALIDATION_WITH_TIME_FAILED,

    /* The OCSP error when the response is found to be not yet valid */
    SEC_PKI_OCSP_ERR_OCSP_RESP_NOT_YET_VALID,

    /* The OCSP error when the response is found to be expired */
    SEC_PKI_OCSP_ERR_OCSP_RESP_HAS_EXPIRED,

    /* The OCSP error when error happended in
    checking This Update field */
    SEC_PKI_OCSP_ERR_ERROR_IN_OCSP_RESP_THIS_UPDATE_FIELD,

    /* The OCSP error when error happended in
    checking Next Update field */
    SEC_PKI_OCSP_ERR_ERROR_IN_OCSP_RESP_NEXT_UPDATE_FIELD,

    /* The OCSP error when Signature Verification Failed */
    SEC_PKI_OCSP_ERR_OCSP_SIGN_VERIFY_FAILED,

    /* The OCSP error when the certificate is found to be revoked
    using OCSP response */
    SEC_PKI_OCSP_ERR_CERT_REVOKED_BY_OCSP_RESP,

    SEC_PKI_OCSP_ERR_CERT_STATUS_ERROR_BY_OCSP_RESP,

    SEC_PKI_OCSP_ERR_RESPONSE_VERSION_ERROR_BY_OCSP_RESP,

    // Max error of OCSP
    SEC_PKI_ERR_OCSP_MAX
}SEC_PKI_OCSP_ERRORS_S;

// Gives the error code
#define SEC_PKI_ERR_REASON_CODE(err) ((err) & 0x0000FFFF)

// Gives the function code where error is occured
#define SEC_PKI_FUNC_REASON_CODE(err) ((err) >> 16)

#ifdef __cplusplus
}
#endif

#endif
