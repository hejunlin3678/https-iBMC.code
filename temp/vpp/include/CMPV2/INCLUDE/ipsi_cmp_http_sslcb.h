/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description  : SSL callback structures
 * Author       : Prabhat Kumar Gopalika
 * FileName     : ipsi_cmp_http_sslcb.h
 * Create       : 29-01-2009
 * Version      : 1
 * Function List: None
 * <author>   <date>          <desc>
 */
#ifndef IPSI_CMP_HTTP_SSLCB_H
#define IPSI_CMP_HTTP_SSLCB_H

#include "ipsi_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IPSI_CMP_SSL_INPROGRESS (-2)

#define IPSI_CMP_SSL_PEER_CLOSE (-3)

/* Ipv4  */
#define IPSI_CMP_IP_ADDR_TYPE_IPV4 0

/* Ipv6  */
#define IPSI_CMP_IP_ADDR_TYPE_IPV6 1

/* Number of bytes of the IPV4 address */
#define IPSI_CMP_IPV4_BYTE_NUM 4

/* Number of bytes of the IPV6 address */
#define IPSI_CMP_IPV6_BYTE_NUM 16

/* Number of words of the IPV6 address */
#define IPSI_CMP_IPV6_DWORD_NUM 4

#define IPSI_CMP_SSL_SUCCESS 0

#define IPSI_CMP_SSL_ERROR (-1)

#define IPSI_CMP_SSL_ERROR_WANT_READ (-4)

#define IPSI_CMP_SSL_ERROR_WANT_WRITE (-5)

/**
 * @defgroup IPSI_HTTP_IP_ADDR_S
 * @ingroup Callback_Structure
 * @par Prototype
 * @code
 * typedef struct stHttpIpAddr
 * SEC_UINT16 usType;
 * SEC_UINT16 usPort;
 * SEC_UINT uiScopeId;
 * {
 * SEC_UINT   ulIpV4Addr;
 * SEC_UINT8  ucIpV4Addr[IPSI_CMP_IPV4_BYTE_NUM];
 * SEC_UINT   ulIpV6Addr[IPSI_CMP_IPV6_DWORD_NUM];
 * SEC_UINT8  ucIpV6Addr[IPSI_CMP_IPV6_BYTE_NUM];
 * }u;
 * }IPSI_HTTP_IP_ADDR_S;
 * @endcode
 *
 * @par Description
 * This Structure defines the IP addr and the type of address
 *
 * @datastruct usType IPv4 and IPv6 types
 * @datastruct usPort Host byte order
 * @datastruct uiScopeId scope ID used by the ipv6
 * @datastruct u Union that holds the IP addr of different format.
 * @datastruct ulIpV4Addr IPv4 IP Addr.
 * @datastruct ucIpV4Addr[IPSI_CMP_IPV4_BYTE_NUM] IPv4 IP Addr
 * in byte order.
 * @datastruct ulIpV6Addr[IPSI_CMP_IPV6_DWORD_NUM] IPv6 IP Addr
 * in DWORD order.
 * @datastruct ucIpV6Addr[IPSI_CMP_IPV6_BYTE_NUM] IPv6 IP Addr
 * in byte order.
 * @par Note
 * \n
 * The address should be entered according to the network byte order.
 * Otherwise a byte order failure may occur. It is recommended to enter
 * the address according to the ucIpV4Addr mode.
 */
// defines the ip addr and the type of address
typedef struct stHttpIpAddr {
    /* IPv4 and IPv6 types */
    SEC_UINT16 usType;

    /* Host byte order */
    SEC_UINT16 usPort;

    /* scope_id used by the ipv6 */
    SEC_UINT uiScopeId;
    union {
        /* The address should be entered according to the network byte order.
        Otherwise a byte order failure may occur. It is recommended to enter
        the address according to the ucIpV4Addr mode. */
        SEC_UINT ulIpV4Addr;
        SEC_UINT8 ucIpV4Addr[IPSI_CMP_IPV4_BYTE_NUM];
        SEC_UINT ulIpV6Addr[IPSI_CMP_IPV6_DWORD_NUM];
        SEC_UINT8 ucIpV6Addr[IPSI_CMP_IPV6_BYTE_NUM];
    } u;
} IPSI_HTTP_IP_ADDR_S;

/**
* @defgroup PfCmpSslInit
* @ingroup Callback_Functions
* @par Prototype
* @code
* typedef SEC_VOID (*PfCmpSslInit)(void);
* @endcode
*
* @par Description
*
* This function does the initialization of the SSL library.
* @param[in] NA NA [NA/NA]

* @retval SEC_VOID This Function does not return any value. [NA|NA]
*/
// This function registers the available ciphers and digests and registers
// the error strings required by SSL library.
typedef SEC_VOID (*PfCmpSslInit)(void);

// This function does the final clean up of the SSL library.
/**
* @defgroup PfCmpSslFini
* @ingroup Callback_Functions
* @par Prototype
* @code
* typedef SEC_VOID (*PfCmpSslFini)(void);
* @endcode
*
* @par Description
* This function does the final clean up of the SSL library.
* @param[in] NA NA [NA/NA]

* @retval SEC_VOID This Function does not return any value. [NA|NA]
*/
typedef SEC_VOID (*PfCmpSslFini)(void);

// This function creates SSL Client context.
/**
* @defgroup PfCmpSslCreatClientCtx
* @ingroup Callback_Functions
* @par Prototype
* @code
* typedef SEC_VOID* (*PfCmpSslCreatClientCtx)(
*          IPSI_HTTP_IP_ADDR_S *pstPeerIpAddr);
* @endcode
*
* @par Description
* This function decrements the reference count of context, and removes the
* pstSSLCtx object pointed to by ctx and frees up the allocated memory,
* if the reference count has reached 0.
* @param[in] pstPeerIpAddr This is a pointer that hold the peer
* IP addr. [NA/NA]

* @retval SEC_VOID This Function does not return any value. [NA|NA]
*

*/
typedef SEC_VOID *(*PfCmpSslCreatClientCtx)(IPSI_HTTP_IP_ADDR_S *pstPeerIpAddr);

// This function decrements the reference count of context, and removes the
// pstSSLCtx object pointed to by ctx and frees up the allocated memory,
// if the reference count has reached 0.
/**
* @defgroup PfCmpSslCtxFree
* @ingroup Callback_Functions
* @par Prototype
* @code
* typedef SEC_VOID (*PfCmpSslCtxFree)(SEC_VOID *pstSSLCtx);
* @endcode
*
* @par Description
* This function frees the SSL Context.
* @param[in] pstSSLCtx This pointer that hold the SSL context. [NA/NA]

* @retval SEC_VOID This Function does not return any value. [NA|NA]
*/
typedef SEC_VOID (*PfCmpSslCtxFree)(SEC_VOID *pstSSLCtx);

/**
* @defgroup PfCmpSslNew
* @ingroup Callback_Functions
* @par Prototype
* @code
* typedef SEC_VOID* (*PfCmpSslNew)(SEC_VOID *pstSSLCtx);
* @endcode
*
* @par Description
* This function creates a new SSL object as framework for TLS/SSL
* enabled functions.
* @param[in] pstSSLCtx This pointer that hold the SSL context. [NA/NA]

* @retval SEC_VOID This Function does not return any value. [NA|NA]
*/
// This function creates a new SSL object as framework for TLS/SSL
// enabled functions.
typedef SEC_VOID *(*PfCmpSslNew)(SEC_VOID *pstSSLCtx);

/**
* @defgroup PfCmpSslSetfd
* @ingroup Callback_Functions
* @par Prototype
* @code
* typedef SEC_INT (*PfCmpSslSetfd)(SEC_VOID *pstSsl,SEC_INT iSock);
* @endcode
*
* @par Description
* This function sets the file descriptor iSock as the input/output facility for
* SSL (encrypted) side of SSL. iSock will typically be the socket file
* descriptor of a network connection.
* @param[in] pstSsl This pointer that hold the SSL Object [NA/NA].
* @param[in] iSock SSL Socket. [NA/NA].

* @retval SEC_INT If  successful. [IPSI_CMP_SUCCESS|NA]
* @retval SEC_INT If failure. [IPSI_CMP_FAILURE|NA]
*/
// This function sets the file descriptor iSock as the input/output facility for
// the SSL (encrypted) side of SSL. iSock will typically be the socket file
// descriptor of a network connection.
typedef SEC_INT (*PfCmpSslSetfd)(SEC_VOID *pstSsl, SEC_INT iSock);

// This function frees an SSL object.
/**
* @defgroup PfCmpSslFree
* @ingroup Callback_Functions
* @par Prototype
* @code
* typedef SEC_VOID (*PfCmpSslFree)(SEC_VOID *pstSsl);
* @endcode
*
* @par Description
* This function frees an SSL object.
* @param[in] pstSsl This pointer that hold the SSL object. [NA/NA]

* @retval SEC_VOID This Function does not return any value. [NA|NA]
*/
typedef SEC_VOID (*PfCmpSslFree)(SEC_VOID *pstSsl);

// This function initiates connection from a TLS/SSL client in the
// TLS/SSL handshake.
// The communication channel must already have been set and assigned to the SSL
/**
* @defgroup PfCmpSslConnect
* @ingroup Callback_Functions
* @par Prototype
* @code
* typedef SEC_INT (*PfCmpSslConnect)(SEC_VOID *pstSsl);
* @endcode
*
* @par Description
* This function initiates connection from a TLS/SSL client in the
* TLS/SSL handshake. The communication channel must already
* have been set and assigned to the SSL.
* @param[in] pstSsl This pointer that hold the SSL object. [NA/NA]

* @retval SEC_INT If successful. [IPSI_CMP_SUCCESS|NA]
* @retval SEC_INT If there is read and write error. [IPSI_CMP_SSL_ERROR_WANT_READ or IPSI_CMP_SSL_ERROR_WANT_WRITE |NA]
* @retval SEC_INT If connection is failure. [IPSI_CMP_FAILURE|NA]

* @par Note
* \n
* -# PfCmpSslConnect function returns IPSI_CMP_SSL_ERROR_WANT_READ/IPSI_CMP_SSL_ERROR_WANT_WRITE error,
* when SSL connect API returns SSL_ERROR_WANT_READ/SSL_ERROR_WANT_WRITE error.To get the above error user needs
* to register IPSI_CMP_httpRegSSLIsInitFinishedFunc function with CMP library.
*/
// by setting an underlying socket.
typedef SEC_INT (*PfCmpSslConnect)(SEC_VOID *pstSsl);

// This function tries to read iNum bytes from the specified SSL into the
// buffer pBuf.
/**
* @defgroup PfCmpSslRead
* @ingroup Callback_Functions
* @par Prototype
* @code
* typedef SEC_INT (*PfCmpSslRead)(SEC_VOID *pstSsl,
*                                 SEC_VOID *pBuf,
*                                 SEC_INT iBufLen);
* @endcode
*
* @par Description
* This function tries to read iNum bytes from the specified SSL into the
* buffer pBuf.
* @param[in] pstSsl This pointer that hold the SSL object. [NA/NA]
* @param[in] pBuf This pointer that hold the buffer. [NA/NA]
* @param[in] iBufLen Buffer length. [NA/NA]

* @retval SEC_INT If read is successful. [Greater than 0|NA]
* @retval SEC_INT if read operation was not successful,
*    because either an error has occurred or an action must be taken by the calling process.[IPSI_CMP_FAILURE|NA]
* @retval SEC_INT If read fails for read operation.[IPSI_CMP_SSL_ERROR_WANT_READ|NA]
* @retval SEC_INT If read fails for write operation.[IPSI_CMP_SSL_ERROR_WANT_WRITE|NA]
* @retval SEC_INT If read fails for some other reason. [IPSI_CMP_SSL_PEER_CLOSE|NA]
*
* @par Note
* \n
* -# PfCmpSslRead function returns IPSI_CMP_SSL_ERROR_WANT_READ/IPSI_CMP_SSL_ERROR_WANT_WRITE error,
* when SSL read API returns SSL_ERROR_WANT_READ/SSL_ERROR_WANT_WRITE error.To get the above error user needs
* to register IPSI_CMP_httpRegSSLIsInitFinishedFunc function with CMP library.
* -# PfCmpSslRead function returns IPSI_CMP_SSL_PEER_CLOSE error, when SSL read
*  API returns SSL_ERROR_SYSCALL / SSL_ERROR_ZERO_RETURN.
*
*/
typedef SEC_INT (*PfCmpSslRead)(SEC_VOID *pstSsl, SEC_VOID *pBuf, SEC_INT iBufLen);

// This function tries to write iBufLen bytes to the specified SSL from the buffer pBuf.
/**
* @defgroup PfCmpSslWrite
* @ingroup Callback_Functions
* @par Prototype
* @code
* typedef SEC_INT (*PfCmpSslWrite)(SEC_VOID *pstSsl,
*                                  const SEC_VOID *pBuf,
*                                  SEC_INT iBufLen);
* @endcode
*
* @par Description
* This function tries to write iBufLen bytes to the specified SSL from the
* buffer pBuf.
* @param[in] pstSsl This pointer that hold the SSL object. [NA/NA]
* @param[in] pBuf This pointer that hold the buffer. [NA/NA]
* @param[in] iBufLen Buffer length. [NA/NA]

* @retval SEC_INT If write is successful. [Greater than 0|NA]
* @retval SEC_INT If write operation is not successful,
*   because either an error occurred or action must be taken by the calling process.[IPSI_CMP_FAILURE|NA]
* @retval SEC_INT If write fails for read operation.[IPSI_CMP_SSL_ERROR_WANT_READ|NA]
* @retval SEC_INT If write fails for write operation.[IPSI_CMP_SSL_ERROR_WANT_WRITE|NA]

* @par Note
* \n
* -# PfCmpSslWrite function returns IPSI_CMP_SSL_ERROR_WANT_READ/IPSI_CMP_SSL_ERROR_WANT_WRITE error,
* when SSL write API returns SSL_ERROR_WANT_READ/SSL_ERROR_WANT_WRITE error.To get the above error user needs
* to register IPSI_CMP_httpRegSSLIsInitFinishedFunc function with CMP library.
*
*/
typedef SEC_INT (*PfCmpSslWrite)(SEC_VOID *pstSsl, const SEC_VOID *pBuf, SEC_INT iBufLen);

/**
* @defgroup PfCmpSslPending
* @ingroup Callback_Functions
* @par Prototype
* @code
* typedef SEC_INT (*PfCmpSslPending)(const SEC_VOID *pstSsl);
* @endcode
*
* @par Description
* This function is used to obtain number of pending readable bytes
* buffered in an SSL object.
* @param[in] pstSsl This pointer that hold the SSL object. [NA/NA]

* @retval SEC_INT If successful. [IPSI_CMP_SUCCESS|NA]
* @retval SEC_INT If failure. [IPSI_CMP_FAILURE|NA]
*/
// This function is used to obtain number of readable bytes buffered in an
// SSL object.
typedef SEC_INT (*PfCmpSslPending)(const SEC_VOID *pstSsl);

/**
* @defgroup PfCmpSslShutdown
* @ingroup Callback_Functions
* @par Prototype
* @code
* typedef SEC_INT (*PfCmpSslShutdown)(SEC_VOID *pstSsl);
* @endcode
*
* @par Description
* This function used to SSL Shutdown.
* The shutdown procedure consists of 2 steps:
*	- Step 1: Sending of "close notify" shutdown alert.
*	- Step 2: Reception to peer's "close notify" shutdown alert.
* @param[in] pstSsl This pointer that hold the SSL object. [NA/NA]

* @retval SEC_INT If Shutdown is successful. [IPSI_CMP_SUCCESS|NA]
* @retval SEC_INT If Shutdown is failure. [IPSI_CMP_FAILURE|NA]

*/
/**
 * The shutdown procedure consists of 2 steps:
 * - Step 1: The sending of the "close notify" shutdown alert.
 * - Step 2: The reception of the peer's "close notify" shutdown alert.
 */
typedef SEC_INT (*PfCmpSslShutdown)(SEC_VOID *pstSsl);

/**
 * @defgroup IPSI_CMP_SSL_INIT_CTX_CB_S
 * @ingroup Callback_Structure
 * @par Prototype
 * @code
 * typedef struct stCmpSslCtxCb
 * {
 * PfCmpSslInit  pfSslInit;
 * PfCmpSslFini  pfSslFini;
 * PfCmpSslCreatClientCtx  pfSslCreatClientCtx;
 * PfCmpSslCtxFree  pfSslCtxFree;
 * }IPSI_CMP_SSL_INIT_CTX_CB_S;
 * @endcode
 *
 * @par Description
 * This fiuction holds initalize and context of SSL
 *
 * @datastruct pfSslInit This is used to initialize the SSL library.
 * @datastruct pfSslFini This is used to perform the final
 * cleanup of the SSL library.
 * @datastruct pfSslCreatClientCtx This is used to create SSL Client context.
 * @datastruct pfSslCtxFree This is used to free an SSL context.
 */
// SSL Init and Context structure
typedef struct stCmpSslCtxCb {
    // This is used to initialize the SSL library.
    PfCmpSslInit pfSslInit;

    // This is used to perform the final cleanup of the SSL library.
    PfCmpSslFini pfSslFini;

    // This is used to create SSL Client context.
    PfCmpSslCreatClientCtx pfSslCreatClientCtx;

    // This is used to free an SSL context.
    PfCmpSslCtxFree pfSslCtxFree;
} IPSI_CMP_SSL_INIT_CTX_CB_S;

/**
 * @defgroup IPSI_CMP_SSL_OBJ_CB_S
 * @ingroup Callback_Structure
 * @par Prototype
 * @code
 * typedef struct stCmpSslObjCb
 * {
 * PfCmpSslNew  pfSslNew;
 * PfCmpSslFree  pfSslFree;
 * PfCmpSslShutdown  pfSslShutdown;
 * }IPSI_CMP_SSL_OBJ_CB_S;
 * @endcode
 *
 * @par Description
 * This sturcture is used as the SSL object.
 *
 * @datastruct pfSslNew This is used to create a new SSL context object.
 * @datastruct pfSslFree This is used to free an SSL object.
 * @datastruct pfSslShutdown This is used to shutdown an SSL connection.
 */
// SSL object structure
typedef struct stCmpSslObjCb {
    // This is used to create a new SSL context object.
    PfCmpSslNew pfSslNew;

    // This is used to free an SSL object.
    PfCmpSslFree pfSslFree;

    // This is used to shutdown an SSL connection.
    PfCmpSslShutdown pfSslShutdown;
} IPSI_CMP_SSL_OBJ_CB_S;

/**
 * @defgroup IPSI_CMP_SSL_CONNECT_CB_S
 * @ingroup Callback_Structure
 * @par Prototype
 * @code
 * typedef struct stCmpSslConnectCb {
 * PfCmpSslSetfd  pfSslSetfd;
 * PfCmpSslConnect  pfSslConnect;
 * PfCmpSslRead  pfSslRead;
 * PfCmpSslWrite  pfSslWrite;
 * PfCmpSslPending  pfSslPending;
 * }IPSI_CMP_SSL_CONNECT_CB_S;
 * @endcode
 *
 * @par Description
 * This structure is used as SSL socket.
 *
 * @datastruct pfSslSetfd This is used to set the specified file descriptor
 * as the input/output facility for the SSL.
 * @datastruct pfSslConnect This is used to initiate connection from a
 * TLS/SSL client in the TLS/SSL handshake.
 * @datastruct pfSslRead This is used to read data from the SSL connection.
 * @datastruct pfSslWrite This is used to write the data into the SSL connection
 * @datastruct pfSslPending This is used to obtain number of
 * pending readable bytes buffered in an SSL object.
 */
// SSL socket structure
typedef struct stCmpSslConnectCb {
    // This is used to set the specified file descriptor as the input/output
    // facility for the SSL.
    PfCmpSslSetfd pfSslSetfd;

    // This is used to initiate connection from a TLS/SSL client in the TLS/SSL handshake.
    PfCmpSslConnect pfSslConnect;

    // This is used to read data from the SSL connection.
    PfCmpSslRead pfSslRead;

    // This is used to write the data into the SSL connection.
    PfCmpSslWrite pfSslWrite;

    // This is used to obtain number of readable bytes buffered in an SSL object
    PfCmpSslPending pfSslPending;
} IPSI_CMP_SSL_CONNECT_CB_S;

/**
 * @defgroup IPSI_CMP_SSL_CB_S
 * @ingroup Callback_Structure
 * @par Prototype
 * @code
 * typedef struct stCmpSslCb
 * {
 * IPSI_CMP_SSL_INIT_CTX_CB_S stSslCtxCb;
 * IPSI_CMP_SSL_OBJ_CB_S stSslObjCb;
 * IPSI_CMP_SSL_CONNECT_CB_S stSslConnectCb;
 * }IPSI_CMP_SSL_CB_S;
 * @endcode
 *
 * @par Description
 * This structure is used as callback for SSL.
 *
 * @datastruct stSslCtxCb structure that holds the SSL context callback.
 * @datastruct stSslObjCb structure that holds the SSL object callback.
 * @datastruct stSslConnectCb structure that holds the SSL connect callback.
 */
typedef struct stCmpSslCb {
    IPSI_CMP_SSL_INIT_CTX_CB_S stSslCtxCb;
    IPSI_CMP_SSL_OBJ_CB_S stSslObjCb;
    IPSI_CMP_SSL_CONNECT_CB_S stSslConnectCb;
} IPSI_CMP_SSL_CB_S;

/**
* @defgroup PfCmpSslIsInitFinished
* @ingroup Callback_Functions
* @par Prototype
* @code
* typedef SEC_INT32 (*PfCmpSslIsInitFinished)(SEC_VOID         *pstSsl,
*                                                              SEC_UINT32       *pbIsInitFinished);
* @endcode
*
* @par Description
* This function is invoked by stack to check the SSL handshake is in progress or finished
* in renegotiation case. When a connection is configured as SSL, the registered
* callback is invoked by the stack after SSL read and SSL write operation
* , to check whether SSL handshake is in progress or finished in re-negotiation case.

* @param[in] pstSsl This pointer holds the SSL object. [NA/NA]
* @param[in] pbIsInitFinished  SEC_TRUE if handshake over
*                 or renegotiation is over,else SEC_FALSE. [NA/NA]
* @retval SEC_INT If successful. [IPSI_CMP_SSL_SUCCESS|NA]
* @retval SEC_INT If failure. [IPSI_CMP_SSL_ERROR|NA]
*/
typedef SEC_INT32 (*PfCmpSslIsInitFinished)(SEC_VOID *pstSsl, SEC_UINT32 *pbIsInitFinished);

/**
* @defgroup PfCmpSslCreatAndConfigClientCtx
* @ingroup Callback_Functions
* @par Prototype
* @code
* typedef SEC_VOID *(*PfCmpSslCreatAndConfigClientCtx)(IPSI_HTTP_IP_ADDR_S *pstPeerIpAddr, SEC_VOID *pvAuxData);
*
* @endcode
*
* @par Description
* This function creates SSL Client context.
*
* @param[in] pstPeerIpAddr This is a pointer that hold the peer IP addr. [NA/NA]
* @param[in] pvAuxData This is a pointer that hold the user defined data.
*                      It inputs from IPSI_CMP_httpSetSourceAddrEx.
*                      And outputs in this callback.[NA/NA]
* @retval SEC_VOID This Function does not return any value. [NA|NA]
*
*/
typedef SEC_VOID *(*PfCmpSslCreatAndConfigClientCtx)(IPSI_HTTP_IP_ADDR_S *pstPeerIpAddr, SEC_VOID *pvAuxData);

#ifdef __cplusplus
}
#endif

#endif
