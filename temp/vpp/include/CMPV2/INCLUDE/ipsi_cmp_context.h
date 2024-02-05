/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description  : CMP context APIs
 * Author       : VPP
 * FileName     : ipsi_cmp_context.h
 * Create       : 14-10-2009
 * Version      : 1
 * Function List: IPSI_CMP_libraryInit
 * IPSI_CMP_ctxNew
 * IPSI_CMP_ctxFree
 * IPSI_CMP_libraryFini
 * IPSI_CMP_ctxSetTransMethod
 * IPSI_CMP_ctxGetTransMethod
 * IPSI_CMP_ctxSetProtectionAlgType
 * IPSI_CMP_ctxGetProtectionAlgType
 * IPSI_CMP_ctxSetSenderName
 * IPSI_CMP_ctxGetSenderName
 * IPSI_CMP_ctxSetRecipientName
 * IPSI_CMP_ctxGetRecipientName
 * IPSI_CMP_ctxSetCertConfCB
 * IPSI_CMP_ctxDoPKCS10CertReq
 * IPSI_CMP_ctxDoCRMFCertReq
 * <author>   <date>          <desc>
 */
#ifndef IPSI_CMP_CONTEXT_H
#define IPSI_CMP_CONTEXT_H

#include "ipsi_types.h"
#include "sec_list.h"
#include "sec_sys.h"
#include "x509v3_extn.h"
#include "cmp.h"
#include "sec_pki_def.h"
#include "ipsi_cmp_method.h"
#include "ipsi_cmp_def.h"
#include "pem.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup CMPv2
 * This section contains all the CMPv2 Enums, Structures, and Functions.
 */
/**
 * @defgroup Context_Functions
 * @ingroup CMPv2
 * This section contains all the Context Functions.
 */
/**
 * @defgroup Structures
 * @ingroup CMPv2
 * This section contains all the Structures.
 */
/**
 * @defgroup Enums
 * @ingroup CMPv2
 * This section contains all the Enums.
 */
/**
 * @defgroup Callback_Functions
 * @ingroup CMPv2
 * This section contains all the Callback Functions.
 */
/**
 * @defgroup HTTP_Functions
 * @ingroup CMPv2
 * This section contains all the HTTP Functions.
 */
/**
 * @defgroup Response_Functions
 * @ingroup CMPv2
 * This section contains all the Response Functions.
 */
/**
 * @defgroup Store_Functions
 * @ingroup CMPv2
 * This section contains all the Store Functions.
 */
/**
 * @defgroup Poll_Functions
 * @ingroup CMPv2
 * This section contains all the Store Functions.
 */
/**
 * @defgroup Callback_Structure
 * @ingroup CMPv2
 * This section contains all the Store Functions.
 */
/**
 * @defgroup IPSI_CMP_libraryInit
 * @ingroup Context_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_libraryInit(void)
 * @endcode
 * @par Purpose
 * This function is used to initialize the CMP library.
 *
 * @par Description
 * IPSI_CMP_libraryInit function initializes the error
 * strings and locks. All APIs should be called
 * after calling this API.
 *
 * @param[in] NA NA [NA/NA]
 * @param[out] NA NA [NA/NA]
 *
 * @retval SEC_INT If initialization is successful. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT If initialization is failure. [IPSI_CMP_FAILURE|NA]
 */
CMPLINKDLL SEC_INT IPSI_CMP_libraryInit(void);

/**
 * @defgroup IPSI_CMP_ctxNew
 * @ingroup Context_Functions
 * @par Prototype
 * @code
 * IPSI_CMP_CTX_S * IPSI_CMP_ctxNew(void)
 * @endcode
 *
 * @par Purpose
 * This function is used to create a new CMP context.
 *
 * @par Description
 * IPSI_CMP_ctxNew creates a new IPSI_CMP_CTX_S context.
 * This is repository that holds the data of whole session.
 *
 * @param[in] NA NA [NA/NA]
 * @param[out] NA NA [NA/NA]
 *
 * @retval IPSI_CMP_CTX_S*  If the creation of a new IPSI_CMP_CTX_S context is
 * successful.[NA|NA]
 * @retval SEC_NULL If the creation of a new IPSI_CMP_CTX_S
 * context is failure.[SEC_NULL|NA]
 *
 *
 * @par Memory Handling
 * \n
 * This API will allocate memory for IPSI_CMP_CTX_S and
 * returns to the application. To free this memory user
 * has to call IPSI_CMP_ctxFree function.
 * Same context can not be used in multiple threads.
 */
CMPLINKDLL IPSI_CMP_CTX_S *IPSI_CMP_ctxNew(void);

/**
 * @defgroup IPSI_CMP_ctxFree
 * @ingroup Context_Functions
 * @par Prototype
 * @code
 * SEC_VOID IPSI_CMP_ctxFree ( IPSI_CMP_CTX_S *pstContext);
 * @endcode
 *
 * @par Purpose
 * This function is used to free the CMP context structure.
 *
 * @par Description
 * This function is used to free the CMP context structure.
 * This also frees the memory allocated for IPSI_CMP_CTX_S.
 *
 * @param[in] pstContext Pointer to IPSI_CMP_CTX_S.[NA/NA]
 * @param[out] NA NA [NA/NA]
 *
 * @retval SEC_VOID This function does not return any value. [NA|NA]
 *
 * @par Note
 * \n
 * Same context can not be freed in multiple threads.
 */
CMPLINKDLL SEC_VOID IPSI_CMP_ctxFree(IPSI_CMP_CTX_S *pstContext);

/**
 * @defgroup IPSI_CMP_libraryFini
 * @ingroup Context_Functions
 * @par Prototype
 * @code
 * SEC_VOID IPSI_CMP_libraryFini()
 * @endcode
 * @par Purpose
 * This function is used to perform the final cleanup of the CMP library.
 *
 * @par Description
 * IPSI_CMP_libraryFini function final cleanup of the CMP library.
 *
 * @param[in] NA NA [NA/NA]
 * @param[out] NA NA [NA/NA]
 *
 * @retval NA This function does not return any value.[NA|NA]
 *
 * @par Note
 * \n
 * User should not call any CMP related APIs after FINI function.
 */
CMPLINKDLL SEC_VOID IPSI_CMP_libraryFini(void);

/**
 * @defgroup IPSI_CMP_ctxSetTransMethod
 * @ingroup Context_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_ctxSetTransMethod( IPSI_CMP_CTX_S *pstContext,
 * IPSI_CMP_TRANS_METH_S *pstTransMethod)
 * @endcode
 *
 * @par Purpose
 * This function is used to set the transport method type.
 *
 * @par Description
 * IPSI_CMP_ctxSetTransMethod function is used to set the transport method type
 * currently CMP support Hyper Text Transport Protocol
 * as the transport media for sending and receiving the messages.
 *
 * @param[in] pstContext Pointer to IPSI_CMP_CTX_S.[NA/NA]
 * @param[in] pstTransMethod Pointer to specific transport method. [NA/NA]
 * @param[out] NA NA [NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 */
CMPLINKDLL SEC_INT IPSI_CMP_ctxSetTransMethod(IPSI_CMP_CTX_S *pstContext, IPSI_CMP_TRANS_METH_S *pstTransMethod);

/**
 * @defgroup IPSI_CMP_ctxGetTransMethod
 * @ingroup Context_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_ctxGetTransMethod(const IPSI_CMP_CTX_S *pstContext,
 * IPSI_CMP_TRANS_METH_S **ppstTransMethod)
 * @endcode
 *
 * @par Purpose
 * This function is used to get the transport method type.
 *
 * @par Description
 * IPSI_CMP_ctxGetTransMethod function is used to get the transport method
 * which is previously set in the context.
 *
 * @param[in] pstContext Pointer to IPSI_CMP_CTX_S.[NA/NA]
 * @param[out] ppstTransMethod Stores the transport method.[NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 */
CMPLINKDLL SEC_INT IPSI_CMP_ctxGetTransMethod(const IPSI_CMP_CTX_S *pstContext,
    IPSI_CMP_TRANS_METH_S **ppstTransMethod);

/**
 * @defgroup IPSI_CMP_ctxSetProtectionAlgType
 * @ingroup Context_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_ctxSetProtectionAlgType( IPSI_CMP_CTX_S *pstContext,
 * IPSI_CMP_PROTALG_TYPE_E enProtAlgType)
 * @endcode
 *
 * @par Purpose
 * This function is used to set the protection algorithm type.
 *
 * @par Description
 * IPSI_CMP_ctxSetProtectionAlgType function is used to set
 * the protection algorithm type for CMP messages may be Password based MAC
 * or Signature.
 *
 * @param[in] pstContext Pointer to IPSI_CMP_CTX_S.[NA/NA]
 * @param[in] enProtAlgType Type of protection algorithm used for
 * calculation of protection bits.[NA/NA]
 * @param[out] NA NA [NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * 	- signature based and Password Based MAC protection is supported.\n
 * 	- Only following algorithms are supported
 * 	- IPSI_CMP_SIG_WITH_SHA1 (Signature Protection RSA WITH SHA1)
 * - IPSI_CMP_SIG_WITH_SHA256 ((Signature Protection RSA WITH SHA256).
 * - IPSI_CMP_SIG_WITH_SHA384 (Signature Protection RSA WITH SHA384)
 * - IPSI_CMP_SIG_WITH_SHA512 ((Signature Protection RSA WITH SHA512).
 * - IPSI_CMP_HMAC_SHA1 (Password Based MAC with  HMAC SHA1)
 * - IPSI_CMP_HMAC_SHA256 (Password Based MAC with HMAC SHA256). \n
 * 	- IPSI_CMP_NO_PROTECTION can be used to set for CMP requests without
 * protection.\n
 * 	- If no protection bits are supplied then this field
 * must be omitted. if protection bits are supplied then
 * this field must be supplied.
 * - Password based MAC is only supported for IR messages. If set, CR and KUR requests will fail
 * - When protection algorithm is changed from password based mac to any other type, it will free
 * the PBM Parameters. Application will have to set PSK parameters again.
 */
CMPLINKDLL SEC_INT IPSI_CMP_ctxSetProtectionAlgType(IPSI_CMP_CTX_S *pstContext, IPSI_CMP_PROTALG_TYPE_E enProtAlgType);

/**
 * @defgroup IPSI_CMP_ctxGetProtectionAlgType
 * @ingroup Context_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_ctxGetProtectionAlgType(const IPSI_CMP_CTX_S *pstContext,
 * IPSI_CMP_PROTALG_TYPE_E *penOutProtAlgType)
 * @endcode
 *
 * @par Purpose
 * This function is used to get the protection algorithm type.
 *
 * @par Description
 * IPSI_CMP_ctxGetProtectionAlgType function is used to get the protection
 * algorithm type set in the context for CMP messages.
 *
 * @param[in] pstContext Pointer to IPSI_CMP_CTX_S.[NA/NA]
 * @param[out] penOutProtAlgType Pointer that contains the algorithm type.[NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * The default algorithm type is IPSI_CMP_SIG_WITH_SHA256.
 */
CMPLINKDLL SEC_INT IPSI_CMP_ctxGetProtectionAlgType(const IPSI_CMP_CTX_S *pstContext,
    IPSI_CMP_PROTALG_TYPE_E *penOutProtAlgType);

/**
 * @defgroup IPSI_CMP_ctxSetSenderName
 * @ingroup Context_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_ctxSetSenderName(
 * IPSI_CMP_CTX_S *pstContext,
 * SEC_GEN_NAME_S *pstGeneralName)
 * @endcode
 *
 * @par Purpose
 * This function is used to set the sender name.
 *
 * @par Description
 * IPSI_CMP_ctxSetSenderName function is used to set the sender name
 * to be used for the CMP messages.
 *
 * @param[in] pstContext Pointer to IPSI_CMP_CTX_S.[NA/NA]
 * @param[in] pstGeneralName Pointer to hold the sender name or client
 * name.[NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * 	- If nothing about the sender is known to the sending entity
 * then the "sender" field MUST contain a "NULL" value.
 * 	- Input general name should be of type GENERALNAME_IPADDRESS (or)
 * GENERALNAME_DIRECTORYNAME (or) GENERALNAME_DNSNAME
 */
CMPLINKDLL SEC_INT IPSI_CMP_ctxSetSenderName(IPSI_CMP_CTX_S *pstContext, SEC_GEN_NAME_S *pstGeneralName);

/**
 * @defgroup IPSI_CMP_ctxGetSenderName
 * @ingroup Context_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_ctxGetSenderName(
 * const IPSI_CMP_CTX_S *pstContext,
 * SEC_GEN_NAME_S **dpstGeneralName)
 * @endcode
 *
 * @par Purpose
 * This function is used to get the sender name.
 *
 * @par Description
 * IPSI_CMP_ctxGetSenderName function is used to get the sender
 * name that will be used for the CMP messages.
 *
 * @param[in] pstContext Pointer to IPSI_CMP_CTX_S.[NA/NA]
 * @param[out] dpstGeneralName Pointer to hold the sender
 * name or client name.[NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * Memory allocated for the output general name dpstGeneralName,
 * should be freed by calling the function X509_freeGeneralName.
 */
CMPLINKDLL SEC_INT IPSI_CMP_ctxGetSenderName(const IPSI_CMP_CTX_S *pstContext, SEC_GEN_NAME_S **dpstGeneralName);

/**
 * @defgroup IPSI_CMP_ctxSetRecipientName
 * @ingroup Context_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_ctxSetRecipientName(
 * IPSI_CMP_CTX_S *pstContext,
 * SEC_GEN_NAME_S *pstGeneralName)
 * @endcode
 *
 * @par Purpose
 * This function is used to set Recipient Name(CA server name).
 *
 * @par Description
 * IPSI_CMP_ctxSetRecipientName function is used to set the
 * recipient name to be used for the CMP messages
 *
 * @param[in] pstContext Pointer to IPSI_CMP_CTX_S.[NA/NA]
 * @param[in] pstGeneralName Pointer to hold the
 * recipient name.[NA/NA]
 * @param[out] NA NA [NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * This name is used to verify the protection on the message.
 */
CMPLINKDLL SEC_INT IPSI_CMP_ctxSetRecipientName(IPSI_CMP_CTX_S *pstContext, SEC_GEN_NAME_S *pstGeneralName);

/**
 * @defgroup IPSI_CMP_ctxGetRecipientName
 * @ingroup Context_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_ctxGetRecipientName(
 * const IPSI_CMP_CTX_S *pstContext,
 * SEC_GEN_NAME_S **dpstGeneralName)
 * @endcode
 *
 * @par Purpose
 * This function is used to get the recipient name.
 *
 * @par Description
 * IPSI_CMP_ctxGetRecipientName function is used to get the recipient
 * name that will be used for the CMP messages.
 *
 * @param[in] pstContext  Pointer to IPSI_CMP_CTX_S.[NA/NA]
 * @param[out] dpstGeneralName Recipient name.[NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * Memory allocated for the output general name dpstGeneralName,
 * should be freed by calling the function X509_freeGeneralName
 */
CMPLINKDLL SEC_INT IPSI_CMP_ctxGetRecipientName(const IPSI_CMP_CTX_S *pstContext, SEC_GEN_NAME_S **dpstGeneralName);

/**
* @defgroup IPSI_CMP_ctxSetCertConfCB
* @ingroup Context_Functions
* @par Prototype
* @code
* SEC_INT IPSI_CMP_ctxSetCertConfCB(
*           IPSI_CMP_CTX_S *pstContext,
*           IPSI_CMP_CERT_CONF_CB pfCallback,
*           SEC_VOID *pAppData)
* @endcode
*
* @par Purpose
* This function is used to set the certificate confirmation callback.
*
* @par Description
* This function is used to set the certificate confirmation callback
* function in the context repository for the particular session.
*
* @param[in] pstContext Pointer to IPSI_CMP_CTX_S.[NA/NA]
* @param[in] pfCallback Certificate confirmation callback function.[NA/NA]
* @param[in] pAppData Pointer that holds user application data. [NA/NA]
*
* @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
* @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
*
* @par Note
*	- The client may accept this response from the
* CA and send a confirmation message, or
* may reject the certificate(s) with either an error
* message detailing why the rejection took
* place or no confirmation at all.
*	- A rejection implies to all certificates in the response
* as the protocol does
* not provide any mechanism by which selective
* rejection may not occur.

*/
CMPLINKDLL SEC_INT IPSI_CMP_ctxSetCertConfCB(IPSI_CMP_CTX_S *pstContext, IPSI_CMP_CERT_CONF_CB pfCallback,
    SEC_VOID *pAppData);

/**
 * @defgroup IPSI_CMP_ctxDoPKCS10CertReq
 * @ingroup Context_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_ctxDoPKCS10CertReq(
 * IPSI_CMP_CTX_S *pstContext,
 * SEC_UCHAR *pucReqBuf,
 * SEC_UINT32 uiReqLen,
 * SEC_INT iType,
 * SEC_CHAR *pcPasswd,
 * SEC_UINT uiPwdLen,
 * SEC_INT *piRespStatus,
 * PEM_WARNING_CODE_E *peWarningCode,
 * IPSI_CMP_POLL_REQ_CTX_S **dpstOutPollReqCtx);
 * @endcode
 *
 * @par Purpose
 * This function is used to send PKCS10 Cert Request..
 *
 * @par Description
 * IPSI_CMP_ctxDoPKCS10CertReq function is used to send certificate request
 * in PKCS10 format to the CA server.
 *
 * @param[in] pstContext Pointer to IPSI_CMP_CTX_S.[NA/NA]
 * @param[in] pucReqBuf Encoded PKCS10 Certificate Request Buffer.[NA/NA]
 * @param[in] uiReqLen Request Buffer length.[NA/NA]
 * @param[in] iType Encoded buffer type.
 * where it can be IPSI_CMP_BUF_TYPE_ASN1 indicating DER encoded buffer
 * or IPSI_CMP_BUF_TYPE_PEM indicating PEM encoded buffer.[NA/NA]
 * @param[in] pcPasswd PEM encoded buffer password when the input iType is
 * IPSI_CMP_BUF_TYPE_PEM. [NA/NA]
 * @param[in] uiPwdLen Password length. [NA/NA]
 * @param[out] dpstOutPollReqCtx Pointer that hold the poll request
 * context structure.[NA/NA]
 * @param[out] peWarningCode Pointer for warning code, if decoding pkcs10 required MD5 then it will be updated with
 * IPSI_USE_MD5 , otherwise it will be IPSI_NORMAL
 * @param[out] piRespStatus Output response status indicating one of
 * the following values:
 * 	- IPSI_CMP_NO_RESP
 * 	- IPSI_CMP_RESP
 * 	- IPSI_CMP_RESP_AND_POLL [NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * 	- Only signature based protection generated using RSAwithSHA1
 * (or) RSAwithSHA256, as the protection algorithm is supported. This is
 * applicable for both CMP messages sent and received.
 * 	- If protection algorithm is set to the value other than
 * IPSI_CMP_NO_PROTECTION in the CMP context and if response message
 * received from server does not have protection, then the API will fail.
 * 	- Application should pass correct buffer len (pkcs10 request length - SEC_UINT32 uiReqLen) which
 * corresponds to the actual content length inside the buffer. For PEM buffers, it is expected that it should be
 * null terminated and the buffer length should also include the size of null termination character.
 * 	- For PEM buffer, the default data size is limited to 256KB. If application needs to set size more than 256KB,
 * then need to call API IPSI_setPemMaxSize.
 * - If application does not set seeds and entropy on frequent intervals, it can lead to vulnerable keys or random
 * value. It is recommended to set seeds and entropy on frequent intervals or update the version that migrates the
 * random implementation based on DRBG.
 * - Before calling this API, please make sure that DRBG is initialized for random number generation by calling
 * IPSI_CRYPT_rand_init. If application don't want to use DRBG then appplication should call
 * IPSI_CRYPT_enable_drbg(0) to disable the DRBG Random feature.
 * - DRBG is enabled by default.
 */
CMPLINKDLL SEC_INT IPSI_CMP_ctxDoPKCS10CertReq(IPSI_CMP_CTX_S *pstContext, SEC_UCHAR *pucReqBuf, SEC_UINT32 uiReqLen,
    SEC_INT iType, SEC_CHAR *pcPasswd, SEC_UINT uiPwdLen, SEC_INT *piRespStatus, PEM_WARNING_CODE_E *peWarningCode,
    IPSI_CMP_POLL_REQ_CTX_S **dpstOutPollReqCtx);

/**
 * @defgroup IPSI_CMP_ctxDoCRMFCertReq
 * @ingroup Context_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_ctxDoCRMFCertReq(
 * IPSI_CMP_CTX_S *pstContext,
 * SEC_UCHAR *pucReqBuf,
 * SEC_UINT32 uiReqLen,
 * SEC_INT iType,
 * SEC_CHAR *pcPasswd,
 * SEC_UINT uiPwdLen,
 * SEC_INT *piRespStatus,
 * IPSI_CMP_POLL_REQ_CTX_S **dpstOutPollReqCtx);
 *
 * @endcode
 *
 * @par Purpose
 * This function is used to send CRMF Certificate Requests.
 *
 * @par Description
 * IPSI_CMP_ctxDoCRMFCertReq function is used to send CRMF
 * Certificate Requests to the CA Server.
 *
 * @param[in] pstContext Pointer to IPSI_CMP_CTX_S.[NA/NA]
 * @param[in] pcReqBuf Pointer that holds CRMF
 * Certificate Request Buffer.[NA/NA]
 * @param[in] uiReqLen Length of the Request message.[NA/NA]
 * @param[in] iType Encoded buffer type.
 * where it can be IPSI_CMP_BUF_TYPE_ASN1 indicating DER encoded buffer.[NA/NA]
 * @param[in] pcPasswd PEM encoded buffer password when the input iType is
 * IPSI_CMP_BUF_TYPE_PEM.[NA/NA]
 * @param[in] uiPwdLen Password length.[NA/NA]
 * @param[out] dpstOutPollReqCtx Pointer that hold the poll request
 * context structure.[NA/NA]
 * @param[out] piRespStatus Output response status indicating one of
 * the following values:
 * 	- IPSI_CMP_NO_RESP
 * 	- IPSI_CMP_RESP
 * 	- IPSI_CMP_RESP_AND_POLL [NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * 	- Two cerficate request can be sent at a time.
 * 	- Only DER encoded CRMF certificate request buffer is supported.
 * - If application does not set seeds and entropy on frequent intervals, it can lead to vulnerable keys or random
 * value.It is recommended to set seeds and entropy on frequent intervals or update the version that migrates the
 * random implementation based on DRBG.
 * - Before calling this API, please make sure that DRBG is initialized for random number generation by calling
 * IPSI_CRYPT_rand_init. If application don't want to use DRBG then appplication should call
 * IPSI_CRYPT_enable_drbg(0) to disable the DRBG Random feature.
 * - DRBG is enabled by default.
 */
CMPLINKDLL SEC_INT IPSI_CMP_ctxDoCRMFCertReq(IPSI_CMP_CTX_S *pstContext, SEC_UCHAR *pucReqBuf, SEC_UINT32 uiReqLen,
    SEC_INT iType, SEC_CHAR *pcPasswd, SEC_UINT uiPwdLen, SEC_INT *piRespStatus,
    IPSI_CMP_POLL_REQ_CTX_S **dpstOutPollReqCtx);

/**
 * @defgroup IPSI_CMP_ctxDoKeyUpdateReq
 * @ingroup Context_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_ctxDoKeyUpdateReq(
 * IPSI_CMP_CTX_S *pstContext,
 * SEC_UCHAR *pucReqBuf,
 * SEC_UINT32 uiReqLen,
 * SEC_INT iType,
 * SEC_CHAR *pcPasswd,
 * SEC_UINT uiPwdLen,
 * SEC_INT *piRespStatus,
 * IPSI_CMP_POLL_REQ_CTX_S **dpstOutPollReqCtx);
 * @endcode
 *
 * @par Purpose
 * This function is used to send key update.
 *
 * @par Description
 * IPSI_CMP_ctxDoKeyUpdateReq function is used to send Key update request(s)
 * to the CA Server.
 *
 * @param[in] pstContext Pointer to IPSI_CMP_CTX_S.[NA/NA]
 * @param[in] pcReqBuf Pointer that holds KEY
 * update request Buffer.[NA/NA]
 * @param[in] uiReqLen Length of the Request message.[NA/NA]
 * @param[in] iType Encoded buffer type.
 * where it can be IPSI_CMP_BUF_TYPE_ASN1 indicating DER encoded buffer.[NA/NA]
 * @param[in] pcPasswd PEM encoded buffer password when the input iType is
 * IPSI_CMP_BUF_TYPE_PEM. [NA/NA]
 * @param[in] uiPwdLen Password length.[NA/NA]
 * @param[out] dpstOutPollReqCtx Pointer that hold the poll request
 * context structure.[NA/NA]
 * @param[out] piRespStatus Output response status indicating one of
 * the following values:
 * 	- IPSI_CMP_NO_RESP
 * 	- IPSI_CMP_RESP
 * 	- IPSI_CMP_RESP_AND_POLL [NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * 	- An update is a replacement certificate containing either a new
 * subject public key or the current subject public key.
 * 	- Only DER encoded buffer is supported
 * - If application does not set seeds and entropy on frequent intervals, it can lead to vulnerable keys or random
 * value. It is recommended to set seeds and entropy on frequent intervals or update the version that migrates the
 * random implementation based on DRBG.
 * - Before calling this API, please make sure that DRBG is initialized for random number generation by calling
 * IPSI_CRYPT_rand_init. If application don't want to use DRBG then appplication should call
 * IPSI_CRYPT_enable_drbg(0) to disable the DRBG Random feature.
 * - DRBG is enabled by default.
 */
CMPLINKDLL SEC_INT IPSI_CMP_ctxDoKeyUpdateReq(IPSI_CMP_CTX_S *pstContext, SEC_UCHAR *pucReqBuf, SEC_UINT32 uiReqLen,
    SEC_INT iType, SEC_CHAR *pcPasswd, SEC_UINT uiPwdLen, SEC_INT *piRespStatus,
    IPSI_CMP_POLL_REQ_CTX_S **dpstOutPollReqCtx);

/**
 * @defgroup IPSI_CMP_ctxDoPollReq
 * @ingroup Context_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_ctxDoPollReq(
 * IPSI_CMP_CTX_S *pstContext,
 * IPSI_CMP_POLL_REQ_CTX_S *pstPollReqCtx,
 * SEC_INT *piRespStatus)
 * @endcode
 *
 * @par Purpose
 * This function is used to send the poll request.
 *
 * @par Description
 * IPSI_CMP_ctxDoPollReq function is used to send poll request
 * to the CA Server.
 *
 * @param[in] pstContext Pointer to IPSI_CMP_CTX_S.[NA/NA]
 * @param[in] pstPollReqCtx Pointer the holds the
 * Poll request context structure.[NA/NA]
 * @param[out] piRespStatus Output response status indicating one of
 * the following values:
 * 	- IPSI_CMP_NO_RESP
 * 	- IPSI_CMP_RESP
 * 	- IPSI_CMP_RESP_AND_POLL
 * 	- IPSI_CMP_POLL [NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * 	- Messages intended to handle scenarios in which the
 * client needs to poll the server in order to determine the status
 * of the request.
 * 	- Call IPSI_CMP_respFree before IPSI_CMP_ctxDoPollReq or this API
 * will fail.
 * - If application does not set seeds and entropy on frequent intervals, it can lead to vulnerable keys or random
 * value. It is recommended to set seeds and entropy on frequent intervals or update the version that migrates the
 * random implementation based on DRBG.
 * - Before calling this API, please make sure that DRBG is initialized for random number generation by calling
 * IPSI_CRYPT_rand_init. If application don't want to use DRBG then appplication should call
 * IPSI_CRYPT_enable_drbg(0) to disable the DRBG Random feature.
 * - DRBG is enabled by default.
 */
CMPLINKDLL SEC_INT IPSI_CMP_ctxDoPollReq(IPSI_CMP_CTX_S *pstContext, IPSI_CMP_POLL_REQ_CTX_S *pstPollReqCtx,
    SEC_INT *piRespStatus);

/**
 * @defgroup IPSI_CMP_pollReqCtxFree
 * @ingroup Context_Functions
 * @par Prototype
 * @code
 * SEC_VOID IPSI_CMP_pollReqCtxFree(
 * IPSI_CMP_POLL_REQ_CTX_S *pstPollReqCtx)
 * @endcode
 *
 * @par Purpose
 * This function is used to free the Poll Request context.
 *
 * @par Description
 * IPSI_CMP_pollReqCtxFree function is used to free the
 * Poll Request Context free. Memory allocated will be freed when you
 * Call the function.
 *
 * @param[in] pstPollReqCtx Pointer to IPSI_CMP_POLL_REQ_CTX_S.[NA/NA]
 * @param[out] NA NA [NA/NA]
 *
 * @retval SEC_VOID This function does not return any value. [NA|NA]
 */
CMPLINKDLL SEC_VOID IPSI_CMP_pollReqCtxFree(IPSI_CMP_POLL_REQ_CTX_S *pstPollReqCtx);

/**
 * @defgroup IPSI_CMP_ctxGetPKIObject
 * @ingroup Context_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_ctxGetPKIObject(const IPSI_CMP_CTX_S *pstContext,
 * SEC_PKI_OBJ_S **dpstOutPKIObj)
 * @endcode
 *
 * @par Purpose
 * This function is used to get the PKI Object
 *
 * @par Description
 * IPSI_CMP_ctxGetPKIObject function is used to get the PKI object from the CMP
 * context.
 *
 * @param[in] pstContext Pointer to IPSI_CMP_CTX_S.[NA/NA]
 * @param[out] dpstOutPKIObj Pointer to PKI Object.[NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 * @par Note
 * \n
 * PKI object should be freed only at the end of the application,
 * if the user modifies the PKI object after setting it will results in a
 * undefined behavior because CMP library will be referring this pointer.
 */
CMPLINKDLL SEC_INT IPSI_CMP_ctxGetPKIObject(const IPSI_CMP_CTX_S *pstContext, SEC_PKI_OBJ_S **dpstOutPKIObj);

/**
 * @defgroup IPSI_CMP_ctxSetPKIObject
 * @ingroup Context_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_ctxSetPKIObject( IPSI_CMP_CTX_S *pstContext,
 * SEC_PKI_OBJ_S *pstPKIObj)
 * @endcode
 *
 * @par Purpose
 * This function is used to set the PKI Object
 *
 * @par Description
 * IPSI_CMP_ctxSetPKIObject function is used to set the PKI object into the CMP
 * context. PKI object is used for the identity certificate chain retrieval and
 * validation of the identity certificate which is received from the other side.
 *
 * @param[in] pstContext Pointer to IPSI_CMP_CTX_S.[NA/NA]
 * @param[in] pstPKIObj Pointer to PKI Object.[NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * PKI object which is set should be freed only at the end of the application.
 * If the user modifies and sets different PKI object in same CMP context the
 * context will start pointing to the new object, but user has to take care
 * that the setting of object in CMP context is not happening across threads.
 */
CMPLINKDLL SEC_INT IPSI_CMP_ctxSetPKIObject(IPSI_CMP_CTX_S *pstContext, SEC_PKI_OBJ_S *pstPKIObj);

/**
* @defgroup IPSI_CMP_ctxGetErrorMsg
* @ingroup Context_Functions
* @par Prototype
* @code
* SEC_INT IPSI_CMP_ctxGetErrorMsg( IPSI_CMP_CTX_S *pstContext,
                                CMP_ERRMSG_S **dpstErrorMsg)
* @endcode
*
* @par Purpose
* This function is used to get the error message, on receiving
* error response message from the server.
*
* @par Description
* This function is used to get the error message, on receiving
* error response message from the server. This function can be called,
* to get the error details, if the error obtained from the error stack
* is IPSI_CMP_ERR_RECEIVED_ERROR_RESPONSE_MSG.
*
* @param[in] pstContext Pointer to IPSI_CMP_CTX_S.[NA/NA]
* @param[out] dpstErrorMsg Double pointer to error message.[NA/NA]
*
* @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
* @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
*
* @par Note
* \n
* Call CMP_freeErrMsgCnt function to free the memory allocated for
* output error message.
*/
CMPLINKDLL SEC_INT IPSI_CMP_ctxGetErrorMsg(IPSI_CMP_CTX_S *pstContext, CMP_ERRMSG_S **dpstErrorMsg);

/**
* @defgroup IPSI_CMP_ctxSetExtraCertsState
* @ingroup Context_Functions
* @par Prototype
* @code
* SEC_INT IPSI_CMP_ctxSetExtraCertsState( IPSI_CMP_CTX_S *pstContext,
                                SEC_UINT uiState)
* @endcode
*
* @par Purpose
* This function is used to enable or disable addition of extra certs.
*
* @par Description
* This function is used to enable or disable addition of extra certs
* in messages sent to server by client.
*
* @param[in] pstContext Pointer to IPSI_CMP_CTX_S.[NA/NA]
* @param[in] uiState Macro indicating the extra certs addition state,
*	- IPSI_CMP_ENABLE_EXTRA_CETRTS enables addition of extra
* certs.
*	- IPSI_CMP_DISABLE_EXTRA_CETRTS Disables addition of extra
* certs[NA/NA]
*
* @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
* @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
*/
CMPLINKDLL SEC_INT IPSI_CMP_ctxSetExtraCertsState(IPSI_CMP_CTX_S *pstContext, SEC_UINT uiState);

/**
 * @defgroup IPSI_CMP_ctxDoIrCRMFCertReq
 * @ingroup Context_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_ctxDoIrCRMFCertReq(
 * IPSI_CMP_CTX_S *pstContext,
 * SEC_UCHAR *pucReqBuf,
 * SEC_UINT32 uiReqLen,
 * SEC_INT iType,
 * SEC_CHAR *pcPasswd,
 * SEC_UINT uiPwdLen,
 * SEC_INT *piRespStatus,
 * IPSI_CMP_POLL_REQ_CTX_S **dpstOutPollReqCtx);
 *
 * @endcode
 *
 * @par Purpose
 * It is used to send Initialization Requests to server in CRMF format.
 *
 * @par Description
 * IPSI_CMP_ctxDoIrCRMFCertReq function is used to send IR to the CA Server
 * in CRMF format.
 *
 * @param[in] pstContext Pointer to IPSI_CMP_CTX_S.[NA/NA]
 * @param[in] pcReqBuf Pointer that holds CRMF
 * Certificate Request Buffer.[NA/NA]
 * @param[in] uiReqLen Length of the Request message.[NA/NA]
 * @param[in] iType Encoded buffer type.
 * where it can be IPSI_CMP_BUF_TYPE_ASN1 indicating DER encoded buffer.[NA/NA]
 * @param[in] pcPasswd PEM encoded buffer password when the input iType is
 * IPSI_CMP_BUF_TYPE_PEM.[NA/NA]
 * @param[in] uiPwdLen Password length.[NA/NA]
 * @param[out] dpstOutPollReqCtx Pointer that hold the poll request
 * context structure.[NA/NA]
 * @param[out] piRespStatus Output response status indicating one of
 * the following values:
 * 	- IPSI_CMP_NO_RESP
 * 	- IPSI_CMP_RESP
 * 	- IPSI_CMP_RESP_AND_POLL [NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * 	- IR request can be sent for 2 cert at a time.
 * 	- Only DER encoded CRMF certificate request buffer is supported.
 * - Password Based MAC will fail in case of wait is received as response.\n
 * However dpstOutPollReqCtx should not be NULL
 * - Before calling this API, please make sure that DRBG is initialized for random number generation by calling
 * IPSI_CRYPT_rand_init. If application don't want to use DRBG then appplication should call
 * IPSI_CRYPT_enable_drbg(0) to disable the DRBG Random feature.
 * - DRBG is enabled by default.
 */
CMPLINKDLL SEC_INT IPSI_CMP_ctxDoIrCRMFCertReq(IPSI_CMP_CTX_S *pstContext, SEC_UCHAR *pucReqBuf, SEC_UINT32 uiReqLen,
    SEC_INT iType, SEC_CHAR *pcPasswd, SEC_UINT uiPwdLen, SEC_INT *piRespStatus,
    IPSI_CMP_POLL_REQ_CTX_S **dpstOutPollReqCtx);

/**
 * @defgroup IPSI_CMP_ctxSetMsgTraceCB
 * @ingroup Context_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_ctxSetMsgTraceCB(IPSI_CMP_CTX_S *pstContext,
 * IPSI_CMP_MSG_TRACE_CB pfCallback,
 * SEC_VOID *pAppData)
 *
 * @endcode
 *
 * @par Purpose
 * It is used to set the message Trc callback function.
 *
 * @par Description
 * IPSI_CMP_ctxSetMsgTraceCB function is used to set the Message Trc
 * callback function. If the callback function is set then pfCallback will be
 * called for all the request and response message.
 *
 * @param[in] pstContext Pointer to IPSI_CMP_CTX_S.[NA/NA]
 * @param[in] pfCallback Trc callback function pointer [NA/NA]
 * @param[in] pAppData Application data which is passed to the callback
 * function.[NA/NA]
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * 	- To unset the callback function, NULL should be passed as callback
 * function pointer
 */
CMPLINKDLL SEC_INT IPSI_CMP_ctxSetMsgTraceCB(IPSI_CMP_CTX_S *pstContext, IPSI_CMP_MSG_TRACE_CB pfCallback,
    SEC_VOID *pAppData);

/**
 * @defgroup IPSI_CMP_ctxSetOptions
 * @ingroup Context_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_ctxSetOptions(IPSI_CMP_CTX_S *pstContext,
 * SEC_UINT uiCmpOptions)
 *
 * @endcode
 *
 * @par Purpose
 * It is used to set the CMP options into the CMP context.
 *
 * @par Description
 * Default value set in the CMP context is IPSI_CMP_CLEAR_OPTIONS.
 * Currently following options are supported:
 * 1. IPSI_CMP_CLEAR_OPTIONS - Clears the options which are previsously
 * set in the CMP context.
 * 2. IPSI_CMP_NO_SER_AUTH.- This option will disable the server authentication
 * (for extra cert field of CMP message) in the client side, while verifying the protection
 * on the CMP message server authentication will be bypassed.
 * 3. IPSI_CMP_NO_CA_PUB_PROT_VAL.- This option will disable the protection verification using
 * the capubs of CMP message.
 * 4. IPSI_CMP_VAL_CA_PUB - This option will enable the capubs field certificate
 * (First certificate of capubs fields) to be validate and if validation is passed then
 * the validated certificate will be used for protection verification.
 * 5. IPSI_CMP_SENDER_RECIPIENT_NAME_VALIDATION - This option will enable the sender and recipient name
 * validation on the recieved packet. The validation will be done before the protection check.
 *
 * @param[in] pstContext Pointer to IPSI_CMP_CTX_S.[NA/NA]
 * @param[in] uiCmpOptions Options from the  enum IPSI_CMP_OPTION_E. Different options can be combined
 * together and passed to the API[NA/NA]
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * - Currently only four options are supported
 * IPSI_CMP_CLEAR_OPTIONS, IPSI_CMP_NO_SER_AUTH, IPSI_CMP_NO_CA_PUB_PROT_VAL , IPSI_CMP_VAL_CA_PUB,
 * and IPSI_CMP_SENDER_RECIPIENT_NAME_VALIDATION
 *
 * - IPSI_CMP_NO_CA_PUB_PROT_VAL and IPSI_CMP_VAL_CA_PUB cannot be set together.
 *
 * - If IPSI_CMP_NO_CA_PUB_PROT_VAL option is already set and later IPSI_CMP_VAL_CA_PUB option is passed to the
 * API IPSI_CMP_ctxSetOptions then it will remove the option IPSI_CMP_NO_CA_PUB_PROT_VAL and set the new
 * option IPSI_CMP_VAL_CA_PUB.
 *
 * - If IPSI_CMP_VAL_CA_PUB option is already set and later IPSI_CMP_NO_CA_PUB_PROT_VAL option is passed to the
 * API IPSI_CMP_ctxSetOptions then it will remove the option IPSI_CMP_VAL_CA_PUB and set the new
 * option IPSI_CMP_NO_CA_PUB_PROT_VAL.
 *
 * - IPSI_CMP_CLEAR_OPTIONS option cannot be combined with any other option. It should be passed independently.
 */
CMPLINKDLL SEC_INT IPSI_CMP_ctxSetOptions(IPSI_CMP_CTX_S *pstContext, SEC_UINT uiCmpOptions);

/**
 * @defgroup SEC_getCMPVersion
 * @ingroup Context_Functions
 * @par Prototype
 * @code
 * @endcode
 *
 * @par Purpose
 * This is used to get CMP library version.
 *
 * @par Description
 * SEC_getCMPVersion function returns the text which specifies the version of
 * cmp library being used.
 *
 * @par Parameters
 * N/A
 *
 * @retval const SEC_CHAR* The version of CMP library [NA/NA]
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CMPLINKDLL const SEC_CHAR *SEC_getCMPVersion(void);

/**
 * @defgroup IPSI_CMP_ctxSetPSKParam
 * @ingroup Context_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_ctxSetPSKParam (IPSI_CMP_CTX_S *pstContext, const IPSI_CMP_PSK_PARAM_S *pstPSKParam);
 * @endcode
 *
 * @par Purpose
 * This is used to set pre shared secret and password based mac Parameters
 *
 * @par Description
 * IPSI_CMP_ctxSetPSKParam function is used to set the pre shared secret, reference Number and other password
 * based mac parameters and salt callback
 *
 * @param[in] pstContext Pointer to IPSI_CMP_CTX_S. [NA/NA]
 * @param[in] pstPSKParam Pointer to PSK Parameters. [NA/NA]
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * If callback to generate salt, pfSetSaltCB is set, application has to generate the random number and set the salt. \n
 * If Application does not want to regenerate MAC key, the same salt can be passed over and over again. Max salt
 * acceptable is of 256 bytes. \n
 * If the callback is not set, CMP internally generates a salt of 16Bytes. This salt remains constant throughout the
 * conversation with the server. Same salt Gets used through different IR requests \n
 * Password Based MAC Protection  is supported only for IR messages. CR and KUR messages will fail with Password
 * Based MAC protection \n
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CMPLINKDLL SEC_INT IPSI_CMP_ctxSetPSKParam(IPSI_CMP_CTX_S *pstContext, const IPSI_CMP_PSK_PARAM_S *pstPSKParam);

/**
 * @defgroup IPSI_CMP_ctxFreePSKParam
 * @ingroup Context_Functions
 * @par Prototype
 * @code
 * SEC_VOID IPSI_CMP_ctxFreePSKParam (IPSI_CMP_CTX_S *pstContext);
 * @endcode
 *
 * @par Purpose
 * This is used to cleanse and free  keys and parameters required for password based MAC
 *
 * @par Description
 * IPSI_CMP_ctxFreePSKParam function is used to cleanse keys and parameters required for
 * password based MAC
 *
 * @param[in] pstContext Pointer to IPSI_CMP_CTX_S. [NA/NA]
 * @retval SEC_VOID . [NA|NA]
 *
 * @par Note
 * \n
 * It is advised for application to clean the parameters immediately after use.
 *
 * @par Related Topics
 * N/A
 */
CMPLINKDLL SEC_VOID IPSI_CMP_ctxFreePSKParam(IPSI_CMP_CTX_S *pstContext);

/**
 * @defgroup IPSI_CMP_ctxSetTransactionIdCB
 * @ingroup Context_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_ctxSetTransactionIdCB(IPSI_CMP_CTX_S *pstContext,
 * IPSI_CMP_TRANSID_CB pfCallback,
 * SEC_VOID *pAppData)
 *
 * @endcode
 *
 * @par Purpose
 * It is used to set the transaction Id callback function.
 *
 * @par Description
 * IPSI_CMP_ctxSetTransactionIdCB function is used to set the transaction Id
 * callback function. If the callback function is set then pfCallback will be
 * called while geneartion of the transaction Id.
 *
 * @param[in] pstContext Pointer to IPSI_CMP_CTX_S.[NA/NA]
 * @param[in] pfCallback Transaction Id callback function pointer [NA/NA]
 * @param[in] pAppData Application data which is passed to the callback
 * function.[NA/NA]
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * 	- To unset the callback function, NULL should be passed as callback
 * function pointer
 */
CMPLINKDLL SEC_INT IPSI_CMP_ctxSetTransactionIdCB(IPSI_CMP_CTX_S *pstContext, IPSI_CMP_TRANSID_CB pfCallback,
    SEC_VOID *pAppData);

/**
* @defgroup IPSI_CMP_ctxErrGetCurMsgType
* @ingroup Context_Functions
* @par Prototype
* @code
* SEC_INT IPSI_CMP_ctxErrGetCurMsgType (IPSI_CMP_CTX_S *pstContext,
                                          SEC_UINT *puiCurrentReqMsgType, SEC_UINT *puiCurrentMsgType);
*
* @endcode
*
* @par Purpose
* It is used to get the current request message type and the current message type when any network error
* or any processing of response message has occured.
*
* @par Description
* This API will update the current request messaage type in puiCurrentReqMsgType and current message type in
* puiCurrentMsgType. Current message type can be same as current request messaage type, if both the values are
* same then that means the error has occured while sending the request, if both rae different then response has been
* recived from the server but error has occured in response.
*
* @param[in] pstContext Pointer to IPSI_CMP_CTX_S.[NA/NA]
* @param[out] puiCurrentReqMsgType Current request message type [NA/NA]
* @param[out] puiCurrentMsgType Current message type.  [NA/NA]
* @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
* @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
*
* @par Note
* \n
*     - This API can be used after the failure of request APIs which are
*       IPSI_CMP_ctxDoCRMFCertReq
*       IPSI_CMP_ctxDoIrCRMFCertReq
*       IPSI_CMP_ctxDoPKCS10CertReq
*       IPSI_CMP_ctxDoKeyUpdateReq
*       IPSI_CMP_ctxDoPollReq

*	- If their is no error in request API(s) and this API is called after doing the request then API will be updated
with
*        the last body type sent and received.
*	- If the error occured before sending the first request message inside the above mentioned API then it is not
*     a network problem and in this case the output parameters will be updated with CMP_BODY_TYPE_INVALID.
*	- This API will not interact with the error module. So it will not clear the error stack and push the
*     new error (if any) to the error stack as other APIs does. This API will return error in the below cases
*        1) If the CMP library is not iniatlized
*        2) Input parameters are NULL
*/
CMPLINKDLL SEC_INT IPSI_CMP_ctxErrGetCurMsgType(IPSI_CMP_CTX_S *pstContext, SEC_UINT *puiCurrentReqMsgType,
    SEC_UINT *puiCurrentMsgType);

#ifdef __IPSI_ENABLE_ASYNC_CMP__

/**
 * @defgroup IPSI_CMP_ctxDoAsyncReqResp
 * @ingroup Context_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_ctxDoAsyncReqResp(IPSI_CMP_CTX_S *pstContext,
 * IPSI_CMP_POLL_REQ_CTX_S **dpstOutPollReqCtx, SEC_INT *piRespStatus)
 * @endcode
 *
 * @par Purpose
 * This function is used to complete the async request.
 *
 * @par Description
 * IPSI_CMP_ctxDoAsyncReqResp function is used to send/receive pending  async request.
 * @param[in] pstContext Pointer to IPSI_CMP_CTX_S.[NA/NA]
 * @param[out] dpstOutPollReqCtx Pointer that hold the poll request
 * context structure.[NA/NA]
 * @param[out] piRespStatus Output response status indicating one of
 * the following values: IPSI_CMP_NO_RESP, IPSI_CMP_RESP, IPSI_CMP_POLL, IPSI_CMP_RESP_AND_POLL.
 * \n
 * [NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * - As this API is generic API, it can be used for any incomplete CMPv2 operation(ex: Asynchronous operation),
 * User need to take care of passing correct value of dpstOutPollReqCtx.
 * If previous call to CMP operation is not returning NULL pointer to dpstOutPollReqCtx,
 * then same need to pass to this API otherwise it should point to NULL.
 * - If application does not set seeds and entropy on frequent intervals, it can lead to vulnerable keys or random
 * value. It is recommended to set seeds and entropy on frequent intervals or update the version that migrates the
 * random implementation based on DRBG.
 */
CMPLINKDLL SEC_INT IPSI_CMP_ctxDoAsyncReqResp(IPSI_CMP_CTX_S *pstContext, IPSI_CMP_POLL_REQ_CTX_S **dpstOutPollReqCtx,
    SEC_INT *piRespStatus);

/**
 * @defgroup IPSI_CMP_ctxGetCMPStatus
 * @ingroup Context_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_ctxGetCMPStatus(IPSI_CMP_CTX_S *pstContext,SEC_UINT * puiCmpStatus)
 * @endcode
 *
 * @par Purpose
 * This function is used to check the status of async request.
 *
 * @par Description
 * IPSI_CMP_ctxGetCMPStatus function is used to check the status of async request.
 * @param[in] pstContext Pointer to IPSI_CMP_CTX_S.[NA/NA]
 * @param[out] puiCmpStatus CMP request status indicating one of
 * the following values:
 * 	- IPSI_CMP_IDLE
 * 	- IPSI_CMP_SEND
 * 	- IPSI_CMP_RECIEVE
 * - IPSI_CMP_SIGN  [NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 */
CMPLINKDLL SEC_INT IPSI_CMP_ctxGetCMPStatus(IPSI_CMP_CTX_S *pstContext, SEC_UINT *puiCmpStatus);

#endif

/**
 * @defgroup IPSI_CMP_ctxSetAsyncSignature
 * @ingroup Context_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_ctxSetAsyncSignature(IPSI_CMP_CTX_S *pstContext, SEC_UCHAR *pucSignBuff, SEC_UINT uiSignLen)
 * @endcode
 *
 * @par Purpose
 * This function is used to set the signature of cmp request in case of asyncronous signature
 *
 * @par Description
 * IPSI_CMP_ctxSetAsyncSignature is used to set the signature of cmp request in case of asyncronous signature
 * @param[in] pstContext Pointer to IPSI_CMP_CTX_S.[NA/NA]
 * @param[in] pucSignBuff signature buffer.[NA/NA]
 * @param[in] uiSignLen length of signature buffer.[NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 */
CMPLINKDLL SEC_INT IPSI_CMP_ctxSetAsyncSignature(IPSI_CMP_CTX_S *pstContext, SEC_UCHAR *pucSignBuff,
    SEC_UINT uiSignLen);

/**
 * @defgroup IPSI_CMP_ctxSetAsyncConfirmStatus
 * @ingroup Context_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_ctxSetAsyncConfirmStatus(IPSI_CMP_CTX_S *pstContext, IPSI_CMP_STORE_S *pstCmpStore,
 *                                        CERT_RESP_STATUS_E eConfirmStatus)
 * @endcode
 *
 * @par Purpose
 * This function is used to set the confStatus of cmp request in case of asyncronous confirm
 *
 * @par Description
 * IPSI_CMP_ctxSetAsyncConfirmStatus is used to set the confStatus of cmp request in case of asyncronous confirm
 * @param[in] pstContext Pointer to IPSI_CMP_CTX_S.[NA/NA]
 * @param[in] pstCmpStore Pointer to IPSI_CMP_STORE_S.[NA/NA]
 * @param[in] eConfirmStatus CERT_RESP_STATUS_E to confirm status.[NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * @Memory operation: alloc、free、size
 *    无内存申请或释放
 * @Thread safe:
 *    接口属于线程安全，但注意入参pstContext和pstCmpStore并不支持重入
 * @OS difference:
 *    无OS差异
 * @Time consuming:
 *    不耗时
 */
CMPLINKDLL SEC_INT IPSI_CMP_ctxSetAsyncConfirmStatus(IPSI_CMP_CTX_S *pstContext, IPSI_CMP_STORE_S *pstCmpStore,
    CERT_RESP_STATUS_E eConfirmStatus);

/**
 * @defgroup IPSI_CMP_storeGetAsyncConfirmStatus
 * @ingroup Context_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_storeGetAsyncConfirmStatus(IPSI_CMP_STORE_S *pstCmpStore, CERT_RESP_STATUS_E *penConfirmStatus)
 * @endcode
 *
 * @par Purpose
 * This function is used to get the confStatus of cmp request in case of asyncronous confirm
 *
 * @par Description
 * IPSI_CMP_storeGetAsyncConfirmStatus is used to get the confStatus of cmp request in case of asyncronous confirm
 * @param[in] pstCmpStore Pointer to cmpstore.[NA/NA]
 * @param[in] penConfirmStatus pointer to confirm status.[NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * @Memory operation: alloc、free、size
 *    无内存申请或释放
 * @Thread safe:
 *    接口属于线程安全，但注意入参pstCmpStore并不支持重入
 * @OS difference:
 *    无OS差异
 * @Time consuming:
 *    不耗时
 */
CMPLINKDLL SEC_INT IPSI_CMP_storeGetAsyncConfirmStatus(const IPSI_CMP_STORE_S *pstCmpStore,
    CERT_RESP_STATUS_E *penConfirmStatus);

/**
 * @defgroup IPSI_CMP_ctxSetCertConfCB
 * @ingroup Context_Functions
 * @par Prototype
 * @code
 * SEC_UINT32  IPSI_CMP_ctxSetSignCb(IPSI_CMP_CTX_S *pstContext, IPSI_CMP_SIGN_CB pfSignCb,
 * SEC_VOID* pUserData);
 * @endcode
 *
 * @par Purpose
 * This function is used to set the signature generation callback.
 *
 * @par Description
 * IPSI_CMP_ctxSetSignCb is used to set the signature generation callback.
 *
 *
 * @param[in] pstContext Pointer to IPSI_CMP_CTX_S.[NA/NA]
 * @param[in] pfSignCb signature generation callback function.[NA/NA]
 * @param[in] pUserData Pointer that holds user application data. [NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * 1) If signature call back is enabled and also if the private key is loaded into the object/context of CMP,
 * then signing will happen with the privatekey loaded and this callback is ignored.
 * 2) Application need to call SEC_PKI_ctxSetOptions/ SEC_PKI_objSetOptions with option SEC_PKI_IGNORE_PVT_KEY_LOAD
 * which enable application to load the certificate without private key and then call
 * SEC_PKI_ctxLoadDfltLocalCertAndKeyFile/ SEC_PKI_objLoadDfltLocalCertAndKeyFile
 */
CMPLINKDLL SEC_INT IPSI_CMP_ctxSetSignCb(IPSI_CMP_CTX_S *pstContext, IPSI_CMP_SIGN_CB pfSignCb, SEC_VOID *pUserData);

/**
* @defgroup IPSI_CMP_registerLegacyFea
* @ingroup Functions
* @par Prototype
* @code
*  SEC_UINT32 IPSI_CMP_registerLegacyFea(IPSI_CMP_PROT_S *pstProtTable, SEC_UINT32 uiTableLen);
* @endcode
*
* @par Purpose
* This function is used to register the legacy feature the app want use.
*
* @par Description
* IPSI_CMP_registerLegacyFea is used to register the legacy feature the app want use.
*
*
* @param[in] pstProtTable Pointer to IPSI_CMP_PROT_S table, if NULL, means that application want reset the legacy table
* to NULL, the table element should be the type defined in IPSI_CMP_PROT_S, such as enAlgType should be
* IPSI_CMP_PROTALG_TYPE_E, otherwise there will be a arbitrary behavior in the proctection calculation[NA/NA]
* @param[in] uiTableLen the input IPSI_CMP_PROT_S table len, the max len can be set is 6,
* the minmal is 0 means that application want reset the legacy table to NULL.[NA/NA]
*
* @retval SEC_INT On successful execution. [SEC_OK|NA]
* @retval SEC_INT On all failure conditions. [SEC_ERR|NA]
*
* @par Note
* 1) If app want set the protection algorithm id from the pstProtTable set, this api should call before
*     calling IPSI_CMP_ctxSetProtectionAlgType.
* 2) Application who call this API with NULL pstProtTable, will disable IPSI_CMP_SIG_WITH_SHA1 protection algorithm.
* 3) The pstProtTable to be registered should set {0, IPSI_CMP_NO_PROT, 0, 0} as the last element.
*/
CMPLINKDLL SEC_UINT32 IPSI_CMP_registerLegacyFea(IPSI_CMP_PROT_S *pstProtTable, SEC_UINT32 uiTableLen);

#ifdef __cplusplus
}
#endif

#endif /* IPSI_CMP_CONTEXT_H */
