/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description  : CMP definitions
 * Author       : Arun Raj
 * Create         : 14-10-2009
 * FileName     : ipsi_cmp_def.h
 * Version      : 1
 * Function List: None
 * <author>   <date>          <desc>
 */
#ifndef IPSI_CMP_DEF_H
#define IPSI_CMP_DEF_H

#include "ipsi_types.h"
#include "sec_list.h"
#include "sec_sys.h"
#include "sec_crypto.h"
#include "x509v3_extn.h"
#include "sec_pki_def.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__CMP_BUILD_DLL_)
#define CMPLINKDLL __declspec(dllexport)
#elif defined(__CMP_USR_DLL_)
#define CMPLINKDLL __declspec(dllimport)
#else
#define CMPLINKDLL
#endif

#define IPSI_CMP_FAILURE (-1)
#define IPSI_CMP_SUCCESS 0
#define IPSI_CMP_BREAK 1

#define IPSI_CMP_FALSE 0
#define IPSI_CMP_TRUE 1

// Macro indicating DER encoded buffer
#define IPSI_CMP_BUF_TYPE_ASN1 1

// Macro indicating PEM encoded buffer
#define IPSI_CMP_BUF_TYPE_PEM 2

// Macro indicating that there is no response.
#define IPSI_CMP_NO_RESP 0

// Macro indicating that there is response.
#define IPSI_CMP_RESP 1

// Macro indicating that there is response and poll information.
#define IPSI_CMP_RESP_AND_POLL 2

// Macro indicating that there is only poll information.
#define IPSI_CMP_POLL 3

// Macro to enable extra certs
#define IPSI_CMP_ENABLE_EXTRA_CETRTS 0

// Macro to disable extra certs
#define IPSI_CMP_DISABLE_EXTRA_CETRTS 1

#ifdef __IPSI_ENABLE_ASYNC_CMP__

// Macro to indicate read /write operation is in wait state
#define IPSI_CMP_SEND_RECV_WAIT 1
#endif

// macro to indicate the sign wait state
#define IPSI_CMP_SIGN_WAIT 2

// macro to indicate waiting cert check to confirm
#define IPSI_CMP_CONFIRM_WAIT 3

/**
 * @defgroup IPSI_CMP_CTX_S
 * @ingroup Structures
 * @par Prototype
 * @code
 * typedef struct stCMPCtx IPSI_CMP_CTX_S;
 * @endcode
 * @par Description
 * This structure holds the context
 * repository for CMP.
 *
 */
typedef struct stCMPCtx IPSI_CMP_CTX_S;

/**
 * @defgroup IPSI_CMP_STORE_S
 * @ingroup Structures
 * @par Prototype
 * @code
 * typedef struct stCMPStore IPSI_CMP_STORE_S;
 * @endcode
 * @par Description
 * This structure holds the store
 * repository for CMP.
 *
 */
typedef struct stCMPStore IPSI_CMP_STORE_S;

/**
 * @defgroup IPSI_CMP_POLL_REQ_CTX_S
 * @ingroup Structures
 * @par Prototype
 * @code
 * typedef struct stPollReqCtx IPSI_CMP_POLL_REQ_CTX_S;
 * @endcode
 * @par Description
 * This structure holds the Poll request
 * repository for CMP.
 *
 */
typedef struct stPollReqCtx IPSI_CMP_POLL_REQ_CTX_S;

/**
 * @defgroup IPSI_CMP_RESP_S
 * @ingroup Structures
 * @par Prototype
 * @code
 * typedef struct stCMPResp IPSI_CMP_RESP_S;
 * @endcode
 * @par Description
 * This structure holds the response
 * repository received from CA server for CMP.
 *
 */
typedef struct stCMPResp IPSI_CMP_RESP_S;

/**
 * @defgroup IPSI_CMP_CERT_CONF_CB
 * @ingroup Callback_Functions
 * @par Prototype
 * @code
 * typedef SEC_INT (*IPSI_CMP_CERT_CONF_CB) ( IPSI_CMP_STORE_S *pstCmpStore,
 * SEC_VOID *pAppData);
 *
 * @endcode
 *
 * @par Purpose
 * This function is prototype for certificate confirmation callback.
 *
 * @par Description
 * IPSI_CMP_CERT_CONF_CB defines prototype for
 * certificate confirmation callback.
 *
 * @param[in] pstCmpStore Pointer that store the
 * certificate received from the CA server. [NA/NA]
 * @param[in] pAppData Pointer that holds the user application data.[NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_ERROR|NA]
 *
 */
typedef SEC_INT (*IPSI_CMP_CERT_CONF_CB)(IPSI_CMP_STORE_S *pstCmpStore, SEC_VOID *pAppData);

/**
 * @defgroup IPSI_CMP_SIGN_CB
 * @ingroup Callback_Functions
 * @par Prototype
 * @code
 * typedef SEC_INT (*IPSI_CMP_SIGN_CB)(SEC_UCHAR* pucData, SEC_UINT uiDataLen,
 * SEC_UCHAR *pucSignBuff, SEC_UINT uiSignBuffSize, SEC_UINT *puiSignLen, SEC_VOID* pUserData)
 *
 * @endcode
 *
 * @par Purpose
 * This function is prototype for signature generation callback.
 *
 * @par Description
 * IPSI_CMP_SIGN_CB defines prototype for signature generation callback.
 *
 * @param[in] pucData data to be signed
 * @param[in] uiDataLen length of the data
 * @param[out] pucSignBuff signature buffer
 * @param[in] uiSignBuffSize size of the pucSignBuff
 * @param[in] puiSignLen length of the bytes written in pucSignBuff
 * @param[in] pUserData Pointer that holds the user application data.[NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_ERROR|NA]
 * @retval SEC_INT On wait conditions. [IPSI_CMP_SIGN_WAIT|NA]
 *
 */
typedef SEC_INT (*IPSI_CMP_SIGN_CB)(SEC_UCHAR *pucData, SEC_UINT uiDataLen, SEC_UCHAR *pucSignBuff,
    SEC_UINT uiSignBuffSize, SEC_UINT *puiSignLen, SEC_VOID *pUserData);


/**
* @defgroup IPSI_CMP_PROTALG_TYPE_E
* @ingroup Enums
* @par Prototype
* @code
* typedef enum
* {
*    IPSI_CMP_NO_PROTECTION,
*    IPSI_CMP_SIG_WITH_SHA1 = ALGID_SHA1,
*    IPSI_CMP_SIG_WITH_SHA256 is equal to ALGID_SHA256
*    IPSI_CMP_HMAC_SHA1 = ALGID_HMAC_SHA1,
*    IPSI_CMP_HMAC_SHA256 = ALGID_HMAC_SHA256,
*    IPSI_CMP_SIG_WITH_SHA384 = ALGID_SHA384,
*    IPSI_CMP_SIG_WITH_SHA512 is equal to ALGID_SHA512
* }IPSI_CMP_PROTALG_TYPE_E;

* @endcode
* @par Description
* This emun is used to hold the protection algorithm.
* @datastruct IPSI_CMP_NO_PROTECTION No Protection for request messages.
* @datastruct IPSI_CMP_SIG_WITH_SHA1 Request Message protected with SHA1
* @datastruct IPSI_CMP_SIG_WITH_SHA256 Request Message protected with SHA256
* @datastruct IPSI_CMP_HMAC_SHA1 PBM hmac sha1
* @datastruct IPSI_CMP_HMAC_SHA256 PBM hmac sha256
* @datastruct IPSI_CMP_SIG_WITH_SHA384 Request Message protected with SHA384
* @datastruct IPSI_CMP_SIG_WITH_SHA512 Request Message protected with SHA512
* Message protected with SHA256
*/
typedef enum {
    IPSI_CMP_NO_PROTECTION,
    IPSI_CMP_SIG_WITH_SHA1 = ALGID_SHA1,
    IPSI_CMP_SIG_WITH_SHA256 = ALGID_SHA256,
    IPSI_CMP_HMAC_SHA1 = ALGID_HMAC_SHA1,
    IPSI_CMP_HMAC_SHA256 = ALGID_HMAC_SHA256,
    IPSI_CMP_SIG_WITH_SHA384 = ALGID_SHA384,
    IPSI_CMP_SIG_WITH_SHA512 = ALGID_SHA512,
    IPSI_CMP_SIG_WITH_SM3 = ALGID_SM3
} IPSI_CMP_PROTALG_TYPE_E;

/**
* @defgroup IPSI_CMP_PROT_TYPE_E
* @ingroup Enums
* @par Prototype
* @code
* typedef enum {
*     IPSI_CMP_PROT_SIGN,
*     IPSI_CMP_PROT_HMAC,
*     IPSI_CMP_NO_PROT
* } IPSI_CMP_PROT_TYPE_E;

* @endcode
* @par Description
* This emun is used to hold the protection algorithm type.
* @datastruct IPSI_CMP_PROT_SIGN Request Message protected with signature
* @datastruct IPSI_CMP_PROT_HMAC Request Message protected with HMAC
* @datastruct IPSI_CMP_NO_PROT No Protection type for request messages.
*/
typedef enum {
    IPSI_CMP_PROT_SIGN,
    IPSI_CMP_PROT_HMAC,
    IPSI_CMP_NO_PROT
} IPSI_CMP_PROT_TYPE_E;

/**
 * @defgroup IPSI_CMP_PROT_S
 * @ingroup Structures
 * @par Prototype
 * @code
 * typedef struct {
 *     IPSI_CMP_PROTALG_TYPE_E uiAlgType;
 *     IPSI_CMP_PROT_TYPE_E enProtType;
 *     SEC_UINT32 uiRSAProtAlg;
 *     SEC_UINT32 uiECProtAlg;
 * } IPSI_CMP_PROT_S;
 * @endcode
 * @par Description
 * contains different specific protection algorithm type
 * @datastruct enAlgType hold the protection algorithm.
 * @datastruct enProtType hold the protection algorithm type, HMAC or signature.
 * @datastruct uiRSAProtAlg hold the RSA signature protection algorithm id.
 * @datastruct uiECProtAlg hold the ECDSA signature protection algorithm id.
 */
typedef struct {
    IPSI_CMP_PROTALG_TYPE_E enAlgType;
    IPSI_CMP_PROT_TYPE_E enProtType;
    SEC_UINT32 uiRSAProtAlg;
    SEC_UINT32 uiECProtAlg;
} IPSI_CMP_PROT_S;

/**
 * @defgroup IPSI_CMP_X509_NAME_TYPE_E
 * @ingroup Enums
 * @par Prototype
 * @code
 * typedef enum enCmpName
 * {
 * IPSI_CMP_TYPE_DNSNAME = GENERALNAME_DNSNAME,
 * IPSI_CMP_TYPE_IPADDRESS = GENERALNAME_IPADDRESS,
 * } IPSI_CMP_X509_NAME_TYPE_E
 * @endcode
 * @par Description
 * This emun is used to hold client name and IP addr.
 * @datastruct IPSI_CMP_TYPE_DNSNAME client Name.
 * @datastruct IPSI_CMP_TYPE_IPADDRESS IP addr.
 */
typedef enum enCmpName {
    IPSI_CMP_TYPE_DNSNAME = GENERALNAME_DNSNAME,     /* ID type FQDN */
    IPSI_CMP_TYPE_IPADDRESS = GENERALNAME_IPADDRESS, /* ID type IPV4 address */
} IPSI_CMP_X509_NAME_TYPE_E;

/**
* @defgroup IPSI_CMP_DATA_BUF_S
* @ingroup Structures
* @par Prototype
* @code
* typedef struct stCmpDataBuf
* {
*    char *buf
*    size_t len
}IPSI_CMP_DATA_BUF_S
* @endcode
* @par Description
* This structure is used to hold the buffer data.
* @datastruct buf Pointer that holds the character data.
* @datastruct len Length of the data.
*/
typedef struct stCmpDataBuf {
    char *buf;

    size_t len;
} IPSI_CMP_DATA_BUF_S;

/**
 * @defgroup IPSI_CMP_MSG_TYPE_E
 * @ingroup Enums
 * @par Prototype
 * @code
 * typedef enum
 * {
 * IPSI_CMP_REQ_MSG,
 * IPSI_CMP_RESP_MSG
 * }IPSI_CMP_MSG_TYPE_E
 * @endcode
 * @par Description
 * This enum is used to hold message type. i.e. Request or Response message
 * @datastruct IPSI_CMP_REQ_MSG Request message.
 * @datastruct IPSI_CMP_RESP_MSG Response message.
 */
typedef enum {
    IPSI_CMP_REQ_MSG,
    IPSI_CMP_RESP_MSG
} IPSI_CMP_MSG_TYPE_E;

/**
 * @defgroup IPSI_CMP_OPTION_E
 * @ingroup Enums
 * @par Prototype
 * @code
 * @endcode
 * @par Description
 * This enum is used to hold cmp option
 * @datastruct IPSI_CMP_CLEAR_OPTIONS Clear all the options
 * @datastruct IPSI_CMP_NO_SER_AUTH Disables the extra cert validation
 * @datastruct IPSI_CMP_NO_CA_PUB_PROT_VAL capub field certificates of the cmp message will
 * not be considered for CMP message protection verification.
 * @datastruct IPSI_CMP_VAL_CA_PUB capubs field certificate (First Certificate only) will be validated and if
 * certificate validation is successful then the validated certificate will be used for protection verification.
 * @datastruct IPSI_CMP_SENDER_RECIPIENT_NAME_VALIDATION This option will enable the sender and recipient name
 * validation on the recieved packet. The validation will be done before the protection check.
 * @datastruct IPSI_CMP_GM_DOUBLE_CERT 打开国密选项.
 */
typedef enum {
    IPSI_CMP_CLEAR_OPTIONS = 0,
    IPSI_CMP_NO_SER_AUTH = 1,
    IPSI_CMP_NO_CA_PUB_PROT_VAL = 2,
    IPSI_CMP_VAL_CA_PUB = 4,
    IPSI_CMP_SENDER_RECIPIENT_NAME_VALIDATION = 8,
    IPSI_CMP_GM_DOUBLE_CERT = 16
} IPSI_CMP_OPTION_E;

/**
 * @defgroup IPSI_CMP_CONNECTION_TYPE_E
 * @ingroup Enums
 * @par Prototype
 * @code
 * typedef enum
 * {
 * IPSI_CMP_NON_PERSISTENT_CONNECTION,
 * IPSI_CMP_PERSISTENT_CONNECTION
 * }IPSI_CMP_CONNECTION_TYPE_E
 * @endcode
 * @par Description
 * This enum is used to hold connection type
 * @datastruct IPSI_CMP_NON_PERSISTENT_CONNECTION Non Persistent connection type
 * @datastruct IPSI_CMP_PERSISTENT_CONNECTION  Persistent connection type
 */
typedef enum {
    IPSI_CMP_NON_PERSISTENT_CONNECTION,
    IPSI_CMP_PERSISTENT_CONNECTION
} IPSI_CMP_CONNECTION_TYPE_E;

#ifdef __IPSI_ENABLE_ASYNC_CMP__

/**
 * @defgroup IPSI_CMP_SEND_RECV_STATE_E
 * @ingroup Enums
 * @par Prototype
 * @code
 * typedef enum
 * {
 * IPSI_CMP_IDLE = 0,
 * IPSI_CMP_SEND,
 * IPSI_CMP_RECIEVE,
 * IPSI_CMP_SIGN,
 * }IPSI_CMP_SEND_RECV_STATE_E;
 * @endcode
 * @par Description
 * This enum is used to hold state of cmp
 * @datastruct IPSI_CMP_IDLE Idle state
 * @datastruct IPSI_CMP_SEND  Send state
 * @datastruct IPSI_CMP_RECIEVE  Receive state
 * @datastruct IPSI_CMP_SIGN Sign state
 */
typedef enum {
    IPSI_CMP_IDLE = 0,
    IPSI_CMP_SEND,
    IPSI_CMP_RECIEVE,
    IPSI_CMP_SIGN,
    IPSI_CMP_CONFIRM,
} IPSI_CMP_SEND_RECV_STATE_E;

#endif

/**
* @defgroup IPSI_CMP_MSG_TRACE_CB
* @ingroup Callback_Functions
* @par Prototype
* @code
* typedef SEC_VOID (*IPSI_CMP_MSG_TRACE_CB) (IPSI_CMP_MSG_TYPE_E enMsgType,
*                                            SEC_UCHAR *pcMsg, SEC_UINT uiLen,
*                                            SEC_VOID *pAppData);
*
* @endcode
*
* @par Purpose
* This function is prototype for Message Trc callback.
*
* @par Description
* IPSI_CMP_MSG_TRACE_CB function will be called at the before sending any
* request message and after reciving the response message. Request and response
* message will be passed to this callback function.
*
* @param[in] IPSI_CMP_MSG_TYPE_E Message type [NA/NA]
* @param[in] pcMsg Request or response message.[NA/NA]
* @param[in] uiLen Message length
* @param[in] pAppData Application data, set at the time of setting the callback
             function using IPSI_CMP_ctxSetMsgTraceCB
*
* @retval SEC_VOID This function does not return any value. [NA|NA]
*
*/
typedef SEC_VOID (*IPSI_CMP_MSG_TRACE_CB)(IPSI_CMP_MSG_TYPE_E enMsgType, SEC_UCHAR *pcMsg, SEC_UINT uiLen,
    SEC_VOID *pAppData);

/**
 * @defgroup IPSI_CMP_TRANSID_CB
 * @ingroup Callback_Functions
 * @par Prototype
 * @code
 * typedef SEC_VOID (*IPSI_CMP_TRANSID_CB)(IPSI_CMP_CTX_S *pstCtx,
 * SEC_CHAR *pcTransId, SEC_UINT32 uiTransIdBuffSize, SEC_UINT *uiTransIdOutLen, SEC_VOID *pAppData);
 *
 * @endcode
 *
 * @par Purpose
 * This function is prototype for transactionId callback.
 *
 * @par Description
 * IPSI_CMP_TRANSID_CB function will be called while creating CMP request message.If the callback function is set then
 * callback fucntion  will be called while geneartion of the transaction Id.
 *
 * @param[in] pstCtx Pointer to IPSI_CMP_CTX_S.[NA/NA]
 * @param[in] pcTransId Buffer in which transactionId need to updated[NA/NA]
 * @param[in] uiTransIdBuffSize Size of the pcTransId buffer [32 bytes]
 * @param[in] *uiTransIdOutLen Length of transactionId seb by application in pcTransId [8 to 32 bytes]
 * @param[in] pAppData Application data, set at the time of setting the callback  function using
 * 			IPSI_CMP_ctxSetTransactionIdCB
 *
 * @retval SEC_VOID This function does not return any value. [NA|NA]
 *
 */
typedef SEC_VOID (*IPSI_CMP_TRANSID_CB)(IPSI_CMP_CTX_S *pstCtx, SEC_CHAR *pcTransId, SEC_UINT32 uiTransIdBuffSize,
    SEC_UINT *uiTransIdOutLen, SEC_VOID *pAppData);

/**
 * @defgroup IPSI_CMP_SET_SALT_CB
 * @ingroup Callback_Functions
 * @par Prototype
 * @code
 * typedef SEC_VOID (* IPSI_CMP_SET_SALT_CB)(SEC_UCHAR *pucSalt, SEC_UINT32 uiSaltLen, SEC_UINT32 *uiOutSaltLen,
 * SEC_VOID *pAppData);
 *
 * @endcode
 *
 * @par Purpose
 * This is function prototype to set salt for Password Based MAC.
 *
 * @par Description
 * IPSI_CMP_SET_SALT_CB function will be called while generating HMAC protection. .
 *
 * @param[in] pucSalt Buffer to copy the salt. [NA/NA]
 * @param[in] uiSaltLen Length of pucSalt. This Buffer can be max 256 bytes [256]
 * @param[in] pAppData Application Data set while setting salt callback [NA/NA]
 * @param[out] uiOutSaltLen Actual Salt Length set by the application in  pucSalt [NA/NA]
 *
 * @retval SEC_VOID This function does not return any value. If application has to fail the
 * callback, it has to set the  uiOutSaltLen to 0.[NA|NA]
 *
 */
typedef SEC_VOID (*IPSI_CMP_SET_SALT_CB)(SEC_UCHAR *pucSalt, SEC_UINT32 uiSaltLen, SEC_UINT32 *uiOutSaltLen,
    SEC_VOID *pAppData);

/**
 * @defgroup IPSI_CMP_PSK_PARAM_S
 * @ingroup Structures
 * @par Prototype
 * @code
 * typedef struct
 * {
 * SEC_UCHAR * pucPSK
 * SEC_UINT       uiPSKLen
 * SEC_UCHAR * pucRefValue
 * SEC_UINT       uiRefValueLen
 * SEC_UINT32    uiOwfAlg
 * SEC_UINT32    uiIterationCount
 * IPSI_CMP_SET_SALT_CB pfSetSaltCB
 * SEC_VOID *pAppData
 * } IPSI_CMP_PSK_PARAM_S
 * @endcode
 * @par Description
 * This structure is used to pass PSK Parameters
 * @datastruct pucPSK Pre-Shared key.
 * @datastruct uiPSKLen Length of preShared Key. Recommended to be atleast 12 Bytes. Range is 1 to 65535 Bytes
 * @datastruct pucRefValue Reference Number Shared to EE by the CA.
 * @datastruct uiRefValueLen Length of Reference Number. Range is 1 to 65535 Bytes
 * @datastruct uiOwfAlg Digest algorith to generate the protection key.
 * @datastruct uiIterationCount Iteration Count to generate the derived key. Range is 1 to 0xffffff
 * @datastruct pfSetSaltCB Callback to set salt.
 * @datastruct pAppData Application Data to be passed in the callback.
 */
typedef struct {
    SEC_UCHAR *pucPSK;
    SEC_UINT uiPSKLen;
    SEC_UCHAR *pucRefValue;
    SEC_UINT uiRefValueLen;
    SEC_UINT32 uiOwfAlg;
    SEC_UINT32 uiIterationCount;
    IPSI_CMP_SET_SALT_CB pfSetSaltCB;
    SEC_VOID *pAppData;
} IPSI_CMP_PSK_PARAM_S;

#ifdef __cplusplus
}
#endif

#endif /* IPSI_CMP_DEF_H */
