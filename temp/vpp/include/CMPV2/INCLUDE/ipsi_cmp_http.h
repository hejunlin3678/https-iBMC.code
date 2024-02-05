/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description  : HTTP Method Interface
 * Author       : Prabhat Kumar Gopalika
 * Create       : 16-12-2009
 * FileName     : ipsi_cmp_http.h
 * Version      : 1
 * Function List: IPSI_CMP_httpInit
 * IPSI_CMP_httpRegSockFuncs
 * IPSI_CMP_httpFini
 * IPSI_CMP_httpNew
 * IPSI_CMP_httpMethodInit
 * IPSI_CMP_httpFree
 * IPSI_CMP_httpSetTimeOut
 * IPSI_CMP_httpGetTimeOut
 * IPSI_CMP_httpSetServerIP
 * IPSI_CMP_httpGetServerIP
 * IPSI_CMP_httpSetServerPort
 * IPSI_CMP_httpGetServerPort
 * IPSI_CMP_httpSetSourceAddr
 * IPSI_CMP_httpSetSockOpt
 * IPSI_CMP_httpSetServerURI
 * IPSI_CMP_httpGetServerURI
 * IPSI_CMP_httpSetVersion
 * <author>   <date>          <desc>
 */
#ifndef IPSI_CMP_HTTP_H
#define IPSI_CMP_HTTP_H

#include "ipsi_types.h"
#include "sec_sys.h"
#include "ipsi_cmp_def.h"
#include "ipsi_cmp_method.h"
#include "ipsi_cmp_http_sslcb.h"

#ifdef __cplusplus
extern "C" {
#endif

// Max URI length
#define IPSI_MAX_URI_LEN 256

#define IPSI_HTTP_VER_ONE_DOT_ONE 1
#define IPSI_HTTP_VER_ONE_DOT_ZERO 2

#define IPSI_HTTP_DFLT_TIMER 1

#define IPSI_CMP_HTTP 0
#define IPSI_CMP_HTTPS 1

#define IPSI_HTTP_CLIENT_SERVER_WAIT_TILL_TH_AUTO_EXIT 4

#define IPSI_XCAP_MAX_LINK_PER_THREAD 32

/**
 * @defgroup IPSI_CMP_HTTP_SOCK_OPT_S
 * @ingroup Structures
 * @par Prototype
 * @code
 * typedef struct stCMPHttpSock
 * {
 * SEC_INT32 iLevel
 * SEC_INT32 iOptName
 * SEC_VOID *pvOptVal
 * SEC_INT32 iOptLen
 * SEC_UINT32 uPadd
 * }IPSI_CMP_HTTP_SOCK_OPT_S
 * @endcode
 * @par Description
 * This structure is used to hold the Socket Option.
 * @datastruct iLevel The socket option level
 * @datastruct iOptName The socket option name
 * @datastruct pvOptVal The socket option value
 * @datastruct iOptLen The length of the socket option value
 * @datastruct uPadd The Reserved Bytes
 */
typedef struct stCMPHttpSock {
    SEC_INT32 iLevel;
    SEC_INT32 iOptName;
    SEC_VOID *pvOptVal;
    SEC_INT32 iOptLen;
    SEC_UINT32 uPadd;
} IPSI_CMP_HTTP_SOCK_OPT_S;

/**
 * @defgroup IPSI_CMP_HTTP_CONFIG_WAIT_TILL_THREAD_AUTOEXIT
 * @ingroup Structures
 * @par Prototype
 * @code
 * typedef struct stCMPHttpConfigWaitTillThAutoExit
 * {
 * SEC_UINT32 bWaitTillThAutoExit;
 * SEC_UINT32 bInvokeDelThClbk;
 * }IPSI_CMP_HTTP_CONFIG_WAIT_TILL_THREAD_AUTOEXIT
 * @endcode
 * @par Description
 * This structure is used to hold the configuration value for IPSI_HTTP_CLIENT_SERVER_WAIT_TILL_TH_AUTO_EXIT feature.
 * @datastruct bWaitTillThAutoExit This flag is used to enable feature of waiting in  IPSI_CMP_httpFree API,
 * until  all threads to be deleted will exit by itself. When it is set to IPSI_CMP_TRUE, HTTP instance will
 * wait in IPSI_CMP_httpFree API (invoked by APP thread) until all threads free
 * resource and exit by itself. So APP no need to delete thread in pfBlockWaitThreadExit callback.
 * @datastruct bInvokeDelThClbk   This flag is valid only when bWaitTillThAutoExit flag is enabled.
 * If bInvokeDelThClbk is enabled(IPSI_CMP_TRUE), then it will invoke pfBlockWaitThreadExit callback.
 * If bInvokeDelThClbk is disabled(IPSI_CMP_FALSE), then it will not invoke pfBlockWaitThreadExit callback.
 */
typedef struct stCMPHttpConfigWaitTillThAutoExit {
    /* Flag to enable feature of waiting in  IPSI_CMP_httpFree API, until  all threads to be
    deleted will exit by itself. When it is set to IPSI_CMP_TRUE, HTTP instance will
    wait in IPSI_CMP_httpFree API (invoked by APP thread) until all threads free
    resource and exit by itself. So APP no need to delete thread in pfBlockWaitThreadExit callback. */
    SEC_UINT32 bWaitTillThAutoExit;

    /* This flag is valid only when bWaitTillThAutoExit flag is enabled.
        If bInvokeDelThClbk is enabled(IPSI_CMP_TRUE), then it will invoke pfBlockWaitThreadExit callback.
        If bInvokeDelThClbk is disabled(IPSI_CMP_FALSE), then it will not invoke pfBlockWaitThreadExit callback. */
    SEC_UINT32 bInvokeDelThClbk;
} IPSI_CMP_HTTP_CONFIG_WAIT_TILL_THREAD_AUTOEXIT;

typedef void *ipsi_cmp_http_sock_id_t;
typedef void *ipsi_cmp_http_sock_addr_t;

/**
 * @defgroup IPSI_CMP_HTTP_SOCK_CREATE_CB
 * @ingroup Callback_Functions
 * @par Prototype
 * @code
 * typedef ipsi_cmp_http_sock_id_t (*IPSI_CMP_HTTP_SOCK_CREATE_CB) (
 * SEC_INT32 af,
 * SEC_INT32 type,
 * SEC_INT32 protocol);
 * @endcode
 *
 * @par Purpose
 * This is the prototype of socket creation callback function.
 *
 * @par Description
 * IPSI_CMP_HTTP_SOCK_CREATE_CB is used as callback function used for socket
 * creation for sending and receiving HTTP messages.
 *
 * @param[in] af Address family [NA/NA]
 * @param[in] type socket type to be created [NA/NA]
 * @param[in] protocol TCP or UDP [NA/NA]
 *
 * @retval ipsi_cmp_http_sock_id_t socket id. [NA|NA]
 */
typedef ipsi_cmp_http_sock_id_t (*IPSI_CMP_HTTP_SOCK_CREATE_CB)(SEC_INT32 af, SEC_INT32 type, SEC_INT32 protocol);

/**
 * @defgroup IPSI_CMP_HTTP_SOCK_BIND_CB
 * @ingroup Callback_Functions
 * @par Prototype
 * @code
 * typedef SEC_INT32 (*IPSI_CMP_HTTP_SOCK_BIND_CB) (
 * ipsi_cmp_http_sock_id_t sockfd,
 * ipsi_cmp_http_sock_addr_t addr,
 * SEC_INT32 namelen);
 * @endcode
 *
 * @par Purpose
 * This is the prototype of socket bind callback function.
 *
 * @par Description
 * IPSI_CMP_HTTP_SOCK_BIND_CB is used as callback function used for binding
 * socket for sending and receiving HTTP messages.
 *
 * @param[in] sockfd Socket descriptor [NA/NA]
 * @param[in] addr address [NA/NA]
 * @param[in] namelen Name length [NA/NA]
 *
 * @retval SEC_INT32 On success [0|NA]
 * @retval SEC_INT32 On failure [-1|NA]
 */
typedef SEC_INT32 (*IPSI_CMP_HTTP_SOCK_BIND_CB)(ipsi_cmp_http_sock_id_t sockfd, ipsi_cmp_http_sock_addr_t addr,
    SEC_INT32 namelen);

/**
 * @defgroup IPSI_CMP_HTTP_SOCK_LISTEN_CB
 * @ingroup Callback_Functions
 * @par Prototype
 * @code
 * typedef SEC_INT32 (*IPSI_CMP_HTTP_SOCK_LISTEN_CB) (
 * ipsi_cmp_http_sock_id_t sockfd,
 * SEC_INT32 backlog);
 * @endcode
 *
 * @par Purpose
 * This is the prototype of socket listen callback function.
 *
 * @par Description
 * IPSI_CMP_HTTP_SOCK_LISTEN_CB is used as callback function used for listening
 * socket for sending and receiving HTTP messages.
 *
 * @param[in] sockfd Socket descriptor [NA/NA]
 * @param[in] backlog Maximum length of queue of pending connections [NA/NA]
 *
 * @retval SEC_INT32 On success [0|NA]
 * @retval SEC_INT32 On failure [-1|NA]
 */
typedef SEC_INT32 (*IPSI_CMP_HTTP_SOCK_LISTEN_CB)(ipsi_cmp_http_sock_id_t sockfd, SEC_INT32 backlog);

/**
 * @defgroup IPSI_CMP_HTTP_SOCK_ACCEPT_CB
 * @ingroup Callback_Functions
 * @par Prototype
 * @code
 * typedef SEC_INT32 (*IPSI_CMP_HTTP_SOCK_ACCEPT_CB) (
 * ipsi_cmp_http_sock_id_t sockfd,
 * ipsi_cmp_http_sock_addr_t addr,
 * SEC_INT32 *addrlen);
 * @endcode
 *
 * @par Purpose
 * This is the prototype of socket accept callback function.
 *
 * @par Description
 * ipsi_cmp_http_sock_id_t is used as callback function used for accepting
 * socket for sending and receiving HTTP messages.
 *
 * @param[in] sockfd Socket descriptor [NA/NA]
 * @param[in] addr An optional pointer to a buffer that receives
 * the address of the connecting entity [NA/NA]
 * @param[in] addrlen An optional pointer to an integer that contains
 * the length of the address addr [NA/NA]
 *
 * @retval SEC_INT32 Descriptor of the new socket on success. [NA|NA]
 * @retval SEC_INT32 On failure [-1|NA]
 */
typedef SEC_INT32 (*IPSI_CMP_HTTP_SOCK_ACCEPT_CB)(ipsi_cmp_http_sock_id_t sockfd, ipsi_cmp_http_sock_addr_t addr,
    SEC_INT32 *addrlen);

/**
 * @defgroup IPSI_CMP_HTTP_SOCK_CONNECT_CB
 * @ingroup Callback_Functions
 * @par Prototype
 * @code
 * typedef SEC_INT32 (*IPSI_CMP_HTTP_SOCK_CONNECT_CB) (
 * ipsi_cmp_http_sock_id_t sockfd,
 * ipsi_cmp_http_sock_addr_t *addr,
 * SEC_INT32 namelen);
 * @endcode
 *
 * @par Purpose
 * This is the prototype of socket connect callback function.
 *
 * @par Description
 * IPSI_CMP_HTTP_SOCK_CONNECT_CB is used as callback function used for connecting
 * socket for sending and receiving HTTP messages.
 *
 * @param[in] sockfd Socket descriptor [NA/NA]
 * @param[in] addr The name of the socket to connect[NA/NA]
 * @param[in] namelen Length of the address [NA/NA]
 *
 * @retval SEC_INT32 On success [0|NA]
 * @retval SEC_INT32 On failure [-1|NA]
 */
typedef SEC_INT32 (*IPSI_CMP_HTTP_SOCK_CONNECT_CB)(ipsi_cmp_http_sock_id_t sockfd, ipsi_cmp_http_sock_addr_t *addr,
    SEC_INT32 namelen);

/**
 * @defgroup IPSI_CMP_HTTP_SOCK_CLOSE_CB
 * @ingroup Callback_Functions
 * @par Prototype
 * @code
 * typedef SEC_INT32 (*IPSI_CMP_HTTP_SOCK_CLOSE_CB) (
 * ipsi_cmp_http_sock_id_t sockfd);
 * @endcode
 *
 * @par Purpose
 * This is the prototype of socket close callback function.
 *
 * @par Description
 * IPSI_CMP_HTTP_SOCK_CLOSE_CB is used as callback function used for closing
 * socket.
 *
 * @param[in] sockfd Socket descriptor [NA/NA]
 *
 * @retval SEC_INT32 On success [0|NA]
 * @retval SEC_INT32 On failure [-1|NA]
 */
typedef SEC_INT32 (*IPSI_CMP_HTTP_SOCK_CLOSE_CB)(ipsi_cmp_http_sock_id_t sockfd);

/**
 * @defgroup IPSI_CMP_HTTP_SOCK_IOCTL_CB
 * @ingroup Callback_Functions
 * @par Prototype
 * @code
 * typedef SEC_INT32 (*IPSI_CMP_HTTP_SOCK_IOCTL_CB) (
 * ipsi_cmp_http_sock_id_t sockfd,
 * SEC_INT32 cmd,
 * SEC_UINT32 *parg);
 * @endcode
 *
 * @par Purpose
 * This is the prototype of socket ioctl callback function.
 *
 * @par Description
 * IPSI_CMP_HTTP_SOCK_IOCTL_CB is used as callback function used for controlling
 * I/O mode of a socket for sending and receiving HTTP messages.
 *
 * @param[in] sockfd Socket descriptor [NA/NA]
 * @param[in] cmd The command to perform on the socket  [NA/NA]
 * @param[in] parg A pointer to a parameter for cmd [NA/NA]
 *
 * @retval SEC_INT32 On success [0|NA]
 * @retval SEC_INT32 On failure [-1|NA]
 */
typedef SEC_INT32 (*IPSI_CMP_HTTP_SOCK_IOCTL_CB)(ipsi_cmp_http_sock_id_t sockfd, SEC_INT32 cmd, SEC_UINT32 *parg);

/**
 * @defgroup IPSI_CMP_HTTP_SOCK_SEND_CB
 * @ingroup Callback_Functions
 * @par Prototype
 * @code
 * typedef SEC_INT32 (*IPSI_CMP_HTTP_SOCK_SEND_CB) (
 * ipsi_cmp_http_sock_id_t sockfd,
 * const SEC_VOID *msg,
 * SEC_INT32 len,
 * SEC_INT32 flags);
 * @endcode
 *
 * @par Purpose
 * This is the prototype of socket send callback function.
 *
 * @par Description
 * IPSI_CMP_HTTP_SOCK_SEND_CB is used as callback function used for sending
 * HTTP messages.
 *
 * @param[in] sockfd Socket descriptor [NA/NA]
 * @param[in] msg Message to send [NA/NA]
 * @param[in] len Message length [NA/NA]
 * @param[in] flags An indicator specifying the way in which
 * the call is made [NA/NA]
 *
 * @retval SEC_INT32 Number of bytes sent on success. [NA|NA]
 * @retval SEC_INT32 On failure [-1|NA]
 */
typedef SEC_INT32 (*IPSI_CMP_HTTP_SOCK_SEND_CB)(ipsi_cmp_http_sock_id_t sockfd, const SEC_VOID *msg, SEC_INT32 len,
    SEC_INT32 flags);

/**
 * @defgroup IPSI_CMP_HTTP_SOCK_RECV_CB
 * @ingroup Callback_Functions
 * @par Prototype
 * @code
 * typedef SEC_INT32 (*IPSI_CMP_HTTP_SOCK_RECV_CB) (
 * ipsi_cmp_http_sock_id_t sockfd,
 * const SEC_VOID *buf,
 * SEC_INT32 len,
 * SEC_INT32 flags);
 * @endcode
 *
 * @par Purpose
 * This is the prototype of socket receive callback function.
 *
 * @par Description
 * IPSI_CMP_HTTP_SOCK_RECV_CB is used as callback function used for receiving
 * HTTP messages.
 *
 * @param[in] sockfd Socket descriptor [NA/NA]
 * @param[in] buf Message to be received [NA/NA]
 * @param[in] len Message length [NA/NA]
 * @param[in] flags An indicator specifying the way in which
 * the call is made [NA/NA]
 *
 * @retval SEC_INT32 Number of bytes received on success. [NA|NA]
 * @retval SEC_INT32 On failure [-1|NA]
 */
typedef SEC_INT32 (*IPSI_CMP_HTTP_SOCK_RECV_CB)(ipsi_cmp_http_sock_id_t sockfd, SEC_VOID *buf, SEC_INT32 len,
    SEC_INT32 flags);

/**
 * @defgroup IPSI_CMP_HTTP_SOCK_SETSOCKOPT_CB
 * @ingroup Callback_Functions
 * @par Prototype
 * @code
 * typedef SEC_INT32 (*IPSI_CMP_HTTP_SOCK_SETSOCKOPT_CB)(
 * ipsi_cmp_http_sock_id_t sockfd,
 * SEC_INT32 level,
 * SEC_INT32 optname,
 * SEC_VOID *optval,
 * SEC_INT32 optlen);
 * @endcode
 *
 * @par Purpose
 * This is the prototype of setsockopt callback function.
 *
 * @par Description
 * IPSI_CMP_HTTP_SOCK_SETSOCKOPT_CB is used as callback function used for setting
 * socket options.
 *
 * @param[in] sockfd Socket descriptor [NA/NA]
 * @param[in] level Socket option level [NA/NA]
 * @param[in] optname Socket option name [NA/NA]
 * @param[in] optval Socket option value [NA/NA]
 * @param[in] optlen Socket option value length [NA/NA]
 *
 * @retval SEC_INT32 On success [0|NA]
 * @retval SEC_INT32 On failure [-1|NA]
 */
typedef SEC_INT32 (*IPSI_CMP_HTTP_SOCK_SETSOCKOPT_CB)(ipsi_cmp_http_sock_id_t sockfd, SEC_INT32 level,
    SEC_INT32 optname, SEC_VOID *optval, SEC_INT32 optlen);

/**
 * @defgroup IPSI_CMP_HTTP_SOCK_SELECT_CB
 * @ingroup Callback_Functions
 * @par Prototype
 * @code
 * typedef SEC_INT32 (*IPSI_CMP_HTTP_SOCK_SELECT_CB) (
 * SEC_INT32 nfds,
 * SEC_VOID* readfds,
 * SEC_VOID* writefds,
 * SEC_VOID* exceptfds,
 * SEC_VOID *timeout);
 * @endcode
 *
 * @par Purpose
 * This is the prototype of socket select callback function.
 *
 * @par Description
 * IPSI_CMP_HTTP_SOCK_SELECT_CB is used as callback function used for getting
 * the status of socket.
 *
 * @param[in] nfds Number of socket descriptors [NA/NA]
 * @param[in] readfds Read file descriptors [NA/NA]
 * @param[in] writefds Write file descriptors [NA/NA]
 * @param[in] exceptfds error file descriptors [NA/NA]
 * @param[in] timeout Maximum time to wait [NA/NA]
 *
 * @retval SEC_INT32 Number of socket handles on success. [NA|NA]
 * @retval SEC_INT32 On failure [-1|NA]
 */
typedef SEC_INT32 (*IPSI_CMP_HTTP_SOCK_SELECT_CB)(SEC_INT32 nfds, SEC_VOID *readfds, SEC_VOID *writefds,
    SEC_VOID *exceptfds, SEC_VOID *timeout);

/**
* @defgroup IPSI_CMP_HTTP_SOCK_GETLASTERROR_CB
* @ingroup Callback_Functions
* @par Prototype
* @code
* typedef SEC_INT32 (*IPSI_CMP_HTTP_SOCK_GETLASTERROR_CB) (
*        SEC_VOID);

* @endcode
*
* @par Purpose
* This is the prototype of get last socket error callback function.
*
* @par Description
* ipsi_cmp_http_sock_id_t is used as callback function used for getting
* last socket error.
*
* @param[in] NA [NA/NA]
* @param[out] NA [NA/NA]
*
* @retval SEC_INT32 Indicates the last socker error happened [NA|NA]
*/
typedef SEC_INT32 (*IPSI_CMP_HTTP_SOCK_GETLASTERROR_CB)(void);

/**
* @defgroup IPSI_CMP_HTTP_SOCK_CALLBACK_FUNC_S
* @ingroup Structures
* @par Prototype
* @code
* typedef struct stSockCallbackFunc
* {
*    IPSI_CMP_HTTP_SOCK_CREATE_CB pfSockCreateCb;
*    IPSI_CMP_HTTP_SOCK_BIND_CB pfSockBindCb;
*    IPSI_CMP_HTTP_SOCK_LISTEN_CB pfSockListenCb;
*    IPSI_CMP_HTTP_SOCK_ACCEPT_CB pfSockAcceptCb;
*    IPSI_CMP_HTTP_SOCK_CONNECT_CB pfSockConnectCb;
*    IPSI_CMP_HTTP_SOCK_CLOSE_CB pfSockCloseCb;
*    IPSI_CMP_HTTP_SOCK_IOCTL_CB pfSockIoctlCb;
*    IPSI_CMP_HTTP_SOCK_SEND_CB pfSockSendCb;
*    IPSI_CMP_HTTP_SOCK_RECV_CB pfSockRecvCb;
*    IPSI_CMP_HTTP_SOCK_SETSOCKOPT_CB pfSockSetOptCb;
*    IPSI_CMP_HTTP_SOCK_SELECT_CB pfSockSelectCb;
*    IPSI_CMP_HTTP_SOCK_GETLASTERROR_CB pfSockGetLastErrorCb;
}IPSI_CMP_HTTP_SOCK_CALLBACK_FUNC_S;

* @endcode
* @par Description
* This structure is used to hold the Socket Related Callback functions.
* @datastruct pfSockCreateCb Socket Create Callback
* @datastruct pfSockBindCb Socket Bind Callback
* @datastruct pfSockListenCb Socket Listen Callback
* @datastruct pfSockAcceptCb Socket Accept Callback
* @datastruct pfSockConnectCb Socket Connect Callback
* @datastruct pfSockCloseCb Socket Close Callback
* @datastruct pfSockIoctlCb Socket Input/Output control Callback
* @datastruct pfSockSendCb Socket Send Callback
* @datastruct pfSockRecvCb Socket Recv Callback
* @datastruct pfSockSetOptCb Socket Set Options Callback
* @datastruct pfSockSelectCb Socket Select Callback
* @datastruct pfSockGetLastErrorCb Socket Getlast Error Callback
*/
typedef struct stSockCallbackFunc {
    IPSI_CMP_HTTP_SOCK_CREATE_CB pfSockCreateCb;
    IPSI_CMP_HTTP_SOCK_BIND_CB pfSockBindCb;
    IPSI_CMP_HTTP_SOCK_LISTEN_CB pfSockListenCb;
    IPSI_CMP_HTTP_SOCK_ACCEPT_CB pfSockAcceptCb;
    IPSI_CMP_HTTP_SOCK_CONNECT_CB pfSockConnectCb;
    IPSI_CMP_HTTP_SOCK_CLOSE_CB pfSockCloseCb;
    IPSI_CMP_HTTP_SOCK_IOCTL_CB pfSockIoctlCb;
    IPSI_CMP_HTTP_SOCK_SEND_CB pfSockSendCb;
    IPSI_CMP_HTTP_SOCK_RECV_CB pfSockRecvCb;
    IPSI_CMP_HTTP_SOCK_SETSOCKOPT_CB pfSockSetOptCb;
    IPSI_CMP_HTTP_SOCK_SELECT_CB pfSockSelectCb;
    IPSI_CMP_HTTP_SOCK_GETLASTERROR_CB pfSockGetLastErrorCb;
} IPSI_CMP_HTTP_SOCK_CALLBACK_FUNC_S;

/**
 * @defgroup PfCmpRspNotifyEnterQueue
 * @ingroup Callback_Functions
 * @par Prototype
 * @code
 * typedef SEC_INT (*PfCmpRspNotifyEnterQueue)(
 * IPSI_CMP_TRANS_METH_S *pstTransMethod,
 * SEC_UCHAR *pucBuf,
 * SEC_UINT uiLen,
 * SEC_VOID *pUserData,
 * SEC_UINT uiErr
 * );
 * @endcode
 *
 * @par Purpose
 * This function is used to notify the user that the response message is received.
 *
 * @par Description
 * PfCmpRspNotifyEnterQueue is used to notify the user that the response message is received.
 * Should only put the message into the message queue, no other operations should be done.
 *
 * @param[in] pstTransMethod Pointer to IPSI_CMP_TRANS_METH_S.[NA/NA]
 * @param[in] pucBuf Pointer to SEC_UCHAR.[NA/NA]
 * @param[in] uiLen Buffer length.[NA/NA]
 * @param[in] pUserData This is a pointer that hold the user defined data.
 *                       It inputs from IPSI_CMP_httpSendDataReq.
 *                       And outputs in this callback.[NA/NA]
 * @param[in] uiErr Error code.[NA/NA]
 *
 * @retval SEC_INT On success.[0|NA]
 * @retval SEC_INT On failure.[-1|NA]
 */
typedef SEC_INT (*PfCmpRspNotifyEnterQueue)(
    IPSI_CMP_TRANS_METH_S *pstTransMethod, SEC_UCHAR *pucBuf, SEC_UINT uiLen, SEC_VOID *pUserData, SEC_UINT uiErr);

/**
 * @defgroup IPSI_CMP_httpInit
 * @ingroup HTTP_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_httpInit( SEC_UINT ulTimer)
 * @endcode
 *
 * @par Purpose
 * This function is used to initialize the http global variables.
 *
 * @par Description
 * IPSI_CMP_httpInit is used to create the global http context list which
 * is used when multiple http method received the response in the
 * callback of http receive response and depending on the parameter
 * it will create the thread of timer if the default timer is chosen.
 *
 * @param[in] ulTimer It is a flag which indicates the default timer
 * should be taken or not. Presently only default value should be used that is,
 * IPSI_HTTP_DFLT_TIMER [NA/NA]
 * @param[out] NA NA [NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 *
 * @par Note
 * \n
 * Application should not call any other HTTP API when IPSI_CMP_httpInit
 * is in progress.
 */
CMPLINKDLL SEC_INT IPSI_CMP_httpInit(SEC_UINT ulTimer);

/**
 * @defgroup IPSI_CMP_httpRegSockFuncs
 * @ingroup HTTP_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_httpRegSockFuncs(
 * IPSI_CMP_HTTP_SOCK_CALLBACK_FUNC_S *pstSockCallbackFunc)
 * @endcode
 *
 * @par Purpose
 * This function is used to register the user defined socket functions.
 *
 * @par Description
 * IPSI_CMP_httpRegSockFuncs is used to register the system socket functions.
 * If User defined socket functions are registered then the default functions
 * will be overwritten.
 * If any of the system functions user does not want to register,
 * then it can be set to NULL for that corresponding function.
 *
 * @param[in] pstSockCallbackFunc The pointer to the system socket
 * callback functions structure. [NA/NA]
 * @param[out] NA NA [NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 */
CMPLINKDLL SEC_INT IPSI_CMP_httpRegSockFuncs(IPSI_CMP_HTTP_SOCK_CALLBACK_FUNC_S *pstSockCallbackFunc);

/**
 * @defgroup IPSI_CMP_httpFini
 * @ingroup HTTP_Functions
 * @par Prototype
 * @code
 * SEC_VOID IPSI_CMP_httpFini(void)
 * @endcode
 *
 * @par Purpose
 * This function is used to perform the final clean up of the http.
 *
 * @par Description
 * IPSI_CMP_httpFini function final clean up of the http and
 * free the allocated memory.
 *
 * @param[in] NA NA [NA/NA]
 * @param[out] NA NA [NA/NA]
 *
 * @retval NA This function does not return any value.[NA|NA]
 *
 * @par Note
 * 	- User should not call any Method related APIs after FINI function.
 * 	- This function must be called before IPSI_CMP_libraryFini().
 * - IPSI_CMP_httpFini will  do the XcapApiDeInit, it will fail if transport methods are not deleted.
 */
CMPLINKDLL SEC_VOID IPSI_CMP_httpFini(void);

/**
 * @defgroup IPSI_CMP_httpNew
 * @ingroup HTTP_Functions
 * @par Prototype
 * @code
 * IPSI_CMP_TRANS_METH_S* IPSI_CMP_httpNew(void)
 * @endcode
 *
 * @par Purpose
 * This function is used to create a new HTTP Method.
 *
 * @par Description
 * IPSI_CMP_httpNew creates a new HTTP transport Method to
 * send and receive the message.
 *
 * @param[in] NA NA [NA/NA]
 * @param[out] NA NA [NA/NA]
 *
 * @retval IPSI_CMP_TRANS_METH_S* If the creation of a new HTTP Method is
 * successful.[NA|NA]
 * @retval NULL If the creation of a new HTTP Method fails.[SEC_NULL|NA]
 *
 * @par Memory Handling
 *
 * The API will allocate memory for IPSI_CMP_TRANS_METH_S and
 * returns to the application. To free this memory user
 * has to call the IPSI_CMP_httpFree function.
 *
 * @par Note
 * \n
 * Same context can not be used in multiple threads.
 * Time out of 60 seconds is taken as default if it is not set.
 * The client version HTTP/1.1 is taken as default value.
 */
CMPLINKDLL IPSI_CMP_TRANS_METH_S *IPSI_CMP_httpNew(void);

/**
 * @defgroup IPSI_CMP_httpMethodInit
 * @ingroup HTTP_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_httpMethodInit(
 * const IPSI_CMP_TRANS_METH_S *pstTransMethod)
 * @endcode
 *
 * @par Purpose
 * This function is used to do the initialize of http context
 * repository.
 *
 * @par Description
 * IPSI_CMP_httpMethodInit used to initialize the context of http
 * repository.
 *
 * @param[in] pstTransMethod Pointer to IPSI_CMP_TRANS_METH_S.[NA/NA]
 * @param[out] NA NA [NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * Timeout value should be set by using IPSI_CMP_httpSetTimeOut before
 * calling the IPSI_CMP_httpMethodInit otherwise default timeout value
 * will be used and cannot be changed after the init is successful.
 */
CMPLINKDLL SEC_INT IPSI_CMP_httpMethodInit(const IPSI_CMP_TRANS_METH_S *pstTransMethod);

/**
 * @defgroup IPSI_CMP_httpFree
 * @ingroup HTTP_Functions
 * @par Prototype
 * @code
 * SEC_VOID IPSI_CMP_httpFree(IPSI_CMP_TRANS_METH_S *pstTransMethod)
 * @endcode
 *
 * @par Purpose
 * This function is used to free the CMP HTTP Method.
 *
 * @par Description
 * This function is used to free the CMP HTTP Method
 * and the memory allocated for the context.
 *
 * @param[in] pstTransMethod Pointer to IPSI_CMP_TRANS_METH_S.[NA/NA]
 * @param[out] NA NA [NA/NA]
 *
 * @retval SEC_VOID This function does not return any value. [NA|NA]
 */
CMPLINKDLL SEC_VOID IPSI_CMP_httpFree(IPSI_CMP_TRANS_METH_S *pstTransMethod);

/**
 * @defgroup IPSI_CMP_httpSetTimeOut
 * @ingroup HTTP_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_httpSetTimeOut(
 * const IPSI_CMP_TRANS_METH_S *pstTransMethod,
 * SEC_UINT uiSecs)
 * @endcode
 *
 * @par Purpose
 * This function is used to set the timeout value.
 *
 * @par Description
 * IPSI_CMP_httpSetTimeOut function is used to set
 * the response time out for the http method.
 * This is the time allotted for the transaction,
 * if it exceeds the timeout value, transaction fails.
 *
 * @param[in] pstTransMethod Pointer to IPSI_CMP_TRANS_METH_S.[NA/NA]
 * @param[in] uiSecs Timeout value in seconds.[NA/NA]
 * @param[out] NA NA [NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * The default timeout value is 60 seconds. The maximum timeout value is 180
 * seconds and the minimum timeout value is 60 seconds.
 * It is an optional field.
 * The connection timeout is 60 second, with every request default connection
 * timeout value is considered. So the timeout value for each request will be
 * connection timeout plus transaction timeout value.
 *
 */
CMPLINKDLL SEC_INT IPSI_CMP_httpSetTimeOut(const IPSI_CMP_TRANS_METH_S *pstTransMethod, SEC_UINT uiSecs);

/**
 * @defgroup IPSI_CMP_httpGetTimeOut
 * @ingroup HTTP_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_httpGetTimeOut(
 * const IPSI_CMP_TRANS_METH_S *pstTransMethod,
 * SEC_UINT *puiSecs)
 * @endcode
 *
 * @par Purpose
 * This function is used to get the timeout value.
 *
 * @par Description
 * IPSI_CMP_httpGetTimeOut function is used to get
 * the response time out for the http method.
 *
 * @param[in] pstTransMethod Pointer to IPSI_CMP_TRANS_METH_S.[NA/NA]
 * @param[out] puiSecs Timeout value in seconds.[NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 */
CMPLINKDLL SEC_INT IPSI_CMP_httpGetTimeOut(const IPSI_CMP_TRANS_METH_S *pstTransMethod, SEC_UINT *puiSecs);

/**
 * @defgroup IPSI_CMP_httpSetServerIP
 * @ingroup HTTP_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_httpSetServerIP(
 * const IPSI_CMP_TRANS_METH_S *pstTransMethod,
 * const SEC_CHAR *pcServerIp)
 * @endcode
 *
 * @par Purpose
 * This function is used to set the server IP.
 *
 * @par Description
 * IPSI_CMP_httpSetServerIP function is used to set
 * the Trusted CA server IP.
 *
 * @param[in] pstTransMethod Pointer to IPSI_CMP_TRANS_METH_S.[NA/NA]
 * @param[in] pcServerIp Trusted CA server IP.[NA/NA]
 * @param[out] NA NA [NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * IPV4 And IPV6 addr are supported.
 * For Ipv4 User need to pass simple string. Ex "10.100.xx.xx" .
 * For IPV6 addr User need to pass the Ipv6 string with '[' & ']'
 * Example : "[a353::3:fdbc]"
 * Server Ip should be passed as string i.e. it should be null termintaed.
 */
CMPLINKDLL SEC_INT IPSI_CMP_httpSetServerIP(const IPSI_CMP_TRANS_METH_S *pstTransMethod, const SEC_CHAR *pcServerIp);

/**
 * @defgroup IPSI_CMP_httpGetServerIP
 * @ingroup HTTP_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_httpGetServerIP(
 * const IPSI_CMP_TRANS_METH_S *pstTransMethod,
 * SEC_CHAR** pcServerIP)
 * @endcode
 *
 * @par Purpose
 * This function is used to get the server IP.
 *
 * @par Description
 * IPSI_CMP_httpGetServerIP function is used to get
 * the Trusted CA server IP.
 *
 * @param[in] pstTransMethod Pointer to IPSI_CMP_TRANS_METH_S.[NA/NA]
 * @param[out] pcServerIp Trusted CA server IP.[NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Memory Handling
 *
 * The API will allocate memory for pcServerIP and
 * returns to the application. To free this memory user
 * has to call the ipsi_free(*pcServerIP) function.
 * IPV4 And IPV6 address are supported.
 */
CMPLINKDLL SEC_INT IPSI_CMP_httpGetServerIP(const IPSI_CMP_TRANS_METH_S *pstTransMethod, SEC_CHAR **pcServerIP);

/**
 * @defgroup IPSI_CMP_httpSetServerPort
 * @ingroup HTTP_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_httpSetServerPort(
 * const IPSI_CMP_TRANS_METH_S *pstTransMethod,
 * SEC_UINT uiPort)
 * @endcode
 *
 * @par Purpose
 * This function is used to set the server port.
 *
 * @par Description
 * IPSI_CMP_httpSetServerPort function is used to set
 * the server port.
 *
 * @param[in] pstTransMethod Pointer to IPSI_CMP_TRANS_METH_S.[NA/NA]
 * @param[in] uiPort Server listening port used
 * for transaction of messages.[NA/NA]
 * @param[out] NA NA [NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * The port number should be in the range of 1 to 65535.
 */
CMPLINKDLL SEC_INT IPSI_CMP_httpSetServerPort(const IPSI_CMP_TRANS_METH_S *pstTransMethod, SEC_UINT uiPort);

/**
 * @defgroup IPSI_CMP_httpGetServerPort
 * @ingroup HTTP_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_httpGetServerPort(
 * const IPSI_CMP_TRANS_METH_S *pstTransMethod,
 * SEC_UINT *puiPort)
 * @endcode
 *
 * @par Purpose
 * This function is used to get the server port.
 *
 * @par Description
 * IPSI_CMP_httpGetServerPort function is used to get
 * the server port.
 *
 * @param[in] pstTransMethod Pointer to IPSI_CMP_TRANS_METH_S.[NA/NA]
 * @param[out] puiPort Pointer that holds the server listening port used
 * for transaction of messages.[NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 */
CMPLINKDLL SEC_INT IPSI_CMP_httpGetServerPort(const IPSI_CMP_TRANS_METH_S *pstTransMethod, SEC_UINT *puiPort);

/**
 * @defgroup IPSI_CMP_httpSetSourceAddr
 * @ingroup HTTP_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_httpSetSourceAddr(
 * const IPSI_CMP_TRANS_METH_S * pstTransMethod,
 * const SEC_CHAR *pcSrcIp)
 * @endcode
 *
 * @par Purpose
 * This function is used to set the source address in http transport method.
 *
 * @par Description
 * IPSI_CMP_httpSetSourceAddr function is used to set
 * the source address in http transport method, same address will be used as
 * source address of HTTP message.
 *
 * @param[in] pstTransMethod Pointer to IPSI_CMP_TRANS_METH_S [NA/NA]
 * @param[in] pcSrcIp IP addr of the client [NA/NA]
 * @param[out] NA NA [NA/NA]
 *
 * @retval SEC_INT On successful execution [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * IPV4 And IPV6 address are supported.
 * For Ipv4 User need to pass simple string. Ex "10.100.xx.xx" .
 * For IPV6 addr User need to pass the Ipv6 string with '[' & ']'
 * Example : "[a353::3:fdbc]"
 * Server Ip should be passed as string i.e. it should be null termintaed.
 */
CMPLINKDLL SEC_INT IPSI_CMP_httpSetSourceAddr(const IPSI_CMP_TRANS_METH_S *pstTransMethod, const SEC_CHAR *pcSrcIp);

/**
 * @defgroup IPSI_CMP_httpSetSockOpt
 * @ingroup HTTP_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_httpSetSockOpt(
 * const IPSI_CMP_TRANS_METH_S * pstTransMethod,
 * IPSI_CMP_HTTP_SOCK_OPT_S *pstSockOptions,
 * SEC_UINT32 uOptionsCount)
 * @endcode
 *
 * @par Purpose
 * This function is used to set the options into the underlying socket.
 *
 * @par Description
 * IPSI_CMP_httpSetSockOpt function is used to set options into the
 * underlying socket used for the HTTP connection.
 *
 * @param[in] pstTransMethod Pointer to IPSI_CMP_TRANS_METH_S [NA/NA]
 * @param[in] pstSockOptions Array of socket options [NA/NA]
 * @param[in] uOptionsCount No of options in the array [NA/NA]
 * @param[out] NA NA [NA/NA]
 *
 * @retval SEC_INT On successful execution [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 */
CMPLINKDLL SEC_INT IPSI_CMP_httpSetSockOpt(const IPSI_CMP_TRANS_METH_S *pstTransMethod,
    IPSI_CMP_HTTP_SOCK_OPT_S *pstSockOptions, SEC_UINT32 uOptionsCount);

/**
 * @defgroup IPSI_CMP_httpSetServerURI
 * @ingroup HTTP_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_httpSetServerURI(
 * const IPSI_CMP_TRANS_METH_S *pstTransMethod,
 * const SEC_CHAR *pcUri)
 * @endcode
 *
 * @par Purpose
 * This function is used to set the server URI.
 *
 * @par Description
 * IPSI_CMP_httpSetServerURI function is used to set
 * the server URI (Uniform Resource Identifier).
 *
 * @param[in] pstTransMethod Pointer to IPSI_CMP_TRANS_METH_S.[NA/NA]
 * @param[in] pcUri Trusted CA server URI.[0-255/NA]
 * @param[out] NA NA [NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * The maximum URI string length will be IPSI_MAX_URI_LEN including the
 * null terminator. Empty URI string is not a valid input.
 */
CMPLINKDLL SEC_INT IPSI_CMP_httpSetServerURI(const IPSI_CMP_TRANS_METH_S *pstTransMethod, const SEC_CHAR *pcUri);

/**
 * @defgroup IPSI_CMP_httpGetServerURI
 * @ingroup HTTP_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_httpGetServerURI(
 * const IPSI_CMP_TRANS_METH_S *pstTransMethod,
 * SEC_CHAR** pcServerURI)
 * @endcode
 *
 * @par Purpose
 * This function is used to get the server URI.
 *
 * @par Description
 * IPSI_CMP_httpGetServerURI function is used to get
 * the server URI.
 *
 * @param[in] pstTransMethod Pointer to IPSI_CMP_TRANS_METH_S.[NA/NA]
 * @param[out] pcServerURI Pointer that holds the URI of
 * Trusted CA server URI.[NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Memory Handling
 *
 * 	- The API will allocate memory for pcServerURI and
 * returns to the application. To free this memory user
 * has to call the ipsi_free(*pcServerURI) function.
 * 	- The proxy may forward the request on to another proxy
 * or directly to the server specified by the absoluteURI.
 * In order to avoid request loops, a
 * proxy must be able to recognize all of its server names, including
 * any aliases, local variations, and the numeric IP addr.
 * An example Request-Line:
 * 		- GET http: // www.w3.org/pub/WWW/TheProject.html HTTP/1.0
 */
CMPLINKDLL SEC_INT IPSI_CMP_httpGetServerURI(const IPSI_CMP_TRANS_METH_S *pstTransMethod, SEC_CHAR **pcServerURI);

/**
* @defgroup IPSI_CMP_httpSetVersion
* @ingroup HTTP_Functions
* @par Prototype
* @code
* SEC_INT IPSI_CMP_httpSetVersion(
*                                  const IPSI_CMP_TRANS_METH_S *pstTransMethod,
*                                  SEC_UINT uiVer)
* @endcode
*
* @par Purpose
* This function is used to set the client version.
*
* @par Description
* IPSI_CMP_httpSetVersion function is used to set
* the client HTTP version.
*
* @param[in] pstTransMethod Pointer to IPSI_CMP_TRANS_METH_S.[NA/NA]
* @param[in] uiVer Client HTTP version (IPSI_HTTP_VER_ONE_DOT_ONE or
* IPSI_HTTP_VER_ONE_DOT_ZERO).[NA/NA]
* @param[out] NA NA [NA/NA]
*
* @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
* @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
*
* @par Note
* \n
*	- The default version is HTTP/1.1. It is an optional field.
*	- If not present, the version can be determined from
* the first line of the body.

*/
CMPLINKDLL SEC_INT IPSI_CMP_httpSetVersion(const IPSI_CMP_TRANS_METH_S *pstTransMethod, SEC_UINT uiVer);

/**
 * @defgroup IPSI_CMP_httpGetVersion
 * @ingroup HTTP_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_httpGetVersion(
 * const IPSI_CMP_TRANS_METH_S *pstTransMethod,
 * SEC_UINT *puiVer)
 * @endcode
 *
 * @par Purpose
 * This function is used to get the client version.
 *
 * @par Description
 * IPSI_CMP_httpGetVersion function is used to get
 * the client HTTP version.
 *
 * @param[in] pstTransMethod Pointer to IPSI_CMP_TRANS_METH_S.[NA/NA]
 * @param[out] puiVer Pointer that holds the client HTTP version number.[NA/NA]
 * @param[out] NA NA [NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 */
CMPLINKDLL SEC_INT IPSI_CMP_httpGetVersion(const IPSI_CMP_TRANS_METH_S *pstTransMethod, SEC_UINT *puiVer);

/**
 * @defgroup IPSI_CMP_httpSetMode
 * @ingroup HTTP_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_httpSetMode(
 * const IPSI_CMP_TRANS_METH_S *pstTransMethod,
 * SEC_UINT uiMode)
 * @endcode
 *
 * @par Purpose
 * This function is used to set the ssl mode or tcp mode that is. HTTP or HTTPS.
 *
 * @par Description
 * IPSI_CMP_httpSetMode function is used to set the client ssl mode or tcp mode.
 * If the mode is set to IPSI_CMP_HTTPS then the ssl callback should be
 * registered. Those callback functions will be used for the transactions.
 *
 * @param[in] pstTransMethod Pointer to IPSI_CMP_TRANS_METH_S.[NA/NA]
 * @param[in] uiMode Flag to indicate that the ssl mode will be enabled or not.
 * IPSI_CMP_HTTPS should be used for enabling the tls mode and
 * IPSI_CMP_HTTP should be used for the normal tcp mode. [NA/NA]
 * @param[out] NA NA [NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * By default IPSI_CMP_HTTP mode is set. If the mode is set to IPSI_CMP_HTTPS
 * after the init and ssl callbacks are not set then any cert request from the
 * server will fail.
 */
CMPLINKDLL SEC_INT IPSI_CMP_httpSetMode(const IPSI_CMP_TRANS_METH_S *pstTransMethod, SEC_UINT uiMode);

/**
 * @defgroup IPSI_CMP_httpSetSSLCallbacks
 * @ingroup HTTP_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_httpSetSSLCallbacks(const IPSI_CMP_SSL_CB_S *pstSslCb)
 * @endcode
 *
 * @par Purpose
 * This function is used to set the ssl callbacks.
 *
 * @par Description
 * IPSI_CMP_httpSetSSLCallbacks function is used to set the client ssl callback
 * functions. Those callback functions will be used for the transactions.
 *
 * @param[in] pstSslCb Pointer to SSL callback structure. [NA/NA]
 * @param[out] NA NA [NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * This function should be called in the main thread before
 * spawning any https thread.
 */
CMPLINKDLL SEC_INT IPSI_CMP_httpSetSSLCallbacks(const IPSI_CMP_SSL_CB_S *pstSslCb);

/**
 * @defgroup IPSI_CMP_httpRegSSLIsInitFinishedFunc
 * @ingroup HTTP_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_httpRegSSLIsInitFinishedFunc(PfCmpSslIsInitFinished  pfSslIsInitFinished)
 * @endcode
 *
 * @par Purpose
 * This function registers SSL callback which is used to check the SSL handshake is in progress or finished
 * in re-negotiation case.
 *
 * @par Description
 * IPSI_CMP_RegSSLIsInitFinishedFunc function registers SSL callback which is used to check the SSL handshake is in
 * progress or finished in re-negotiation case. To remove the registered function, NULL can be passed.
 *
 * @param[in] pfSslIsInitFinished Pointer to SSL callback . [NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * This function should be called in the main thread before
 * spawning any https thread.
 */
CMPLINKDLL SEC_INT IPSI_CMP_httpRegSSLIsInitFinishedFunc(PfCmpSslIsInitFinished pfSslIsInitFinished);

/**
 * @defgroup IPSI_CMP_httpSetTimeOut_ex
 * @ingroup HTTP_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_httpSetTimeOut_ex(
 * const IPSI_CMP_TRANS_METH_S *pstTransMethod,
 * SEC_UINT uiSecs)
 * @endcode
 *
 * @par Purpose
 * This function is used to set the timeout value.
 *
 * @par Description
 * IPSI_CMP_httpSetTimeOut_ex function is used to set
 * the response time out for the http method.
 * This is the time allotted for the transaction,
 * if it exceeds the timeout value, transaction fails.
 *
 * @param[in] pstTransMethod Pointer to IPSI_CMP_TRANS_METH_S.[NA/NA]
 * @param[in] uiSecs Timeout value in seconds.[NA/NA]
 * @param[out] NA NA [NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * The default timeout value is 60 seconds. The maximum timeout value is 180
 * seconds and the minimum timeout value is 10 seconds.
 * It is an optional field.
 * The connection timeout is 60 second, with every request default connection
 * timeout value is considered. So the timeout value for each request will be
 * connection timeout plus transaction timeout value.
 * This API is newly added with minimum value as 10 seconds instead of 60 seconds
 * where minimum value allowed is 60 seconds with API IPSI_CMP_httpSetTimeOut.
 * This API will overwrite the value set by IPSI_CMP_httpSetTimeOut if called after calling IPSI_CMP_httpSetTimeOut.
 *
 */
CMPLINKDLL SEC_INT IPSI_CMP_httpSetTimeOut_ex(const IPSI_CMP_TRANS_METH_S *pstTransMethod, SEC_UINT uiSecs);

/**
 * @defgroup IPSI_CMP_httpSetInstanceNum
 * @ingroup HTTP_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_httpSetInstanceNum(SEC_UINT uiMaxInstNum)
 * @endcode
 *
 * @par Purpose
 * This function sets the maxmimum number of instatiated (HTTP) transport methods.
 *
 * @par Description
 * IPSI_CMP_httpSetInstanceNum function sets the maxmimum number of
 * transport methods instantiated using IPSI_CMP_httpMethodInit.
 *
 * @param[in] uiMaxInstNum Maximum number of instances allowed. [1-65535/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * - This function should be called only in the main thread before IPSI_CMP_httpInit.
 * - The maximum instances does not limit the number of instances created. It means the maximum
 * number of the method that can be instantiated at any given time. If the number of transport
 * methods exceeds this limit then IPSI_CMP_httpMethodInit will fail.
 * - By default, the number of maximum instances is set to 128.
 */
CMPLINKDLL SEC_INT IPSI_CMP_httpSetInstanceNum(SEC_UINT uiMaxInstNum);

/**
* @defgroup IPSI_CMP_httpSetConnectionType
* @ingroup HTTP_Functions
* @par Prototype
* @code
* SEC_INT IPSI_CMP_httpSetConnectionType(const IPSI_CMP_TRANS_METH_S *pstTransMethod,
*                                                           IPSI_CMP_CONNECTION_TYPE_E enConnectionType))
* @endcode
*
* @par Purpose
* This function sets the connection type for HTTP.
*
* @par Description
* IPSI_CMP_httpSetConnectionType function sets the connection type(Persistent or Non-Persistent).
* For Non-Persistent connection, a new connection is created for every single request/response pair.
* For Persistent connection, a single TCP connection is used to send and receive multiple HTTP requests/responses.
*
* @param[in] pstTransMethod Pointer to IPSI_CMP_TRANS_METH_S.[NA/NA]
* @param[in] enConnectionType Type of connection used in HTTP.[NA/NA]
*
* @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
* @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
*
* @par Note
* - IPSI_CMP_httpSetConnectionType should be called only in the main thread before IPSI_CMP_httpMethodInit.

*/
CMPLINKDLL SEC_INT IPSI_CMP_httpSetConnectionType(const IPSI_CMP_TRANS_METH_S *pstTransMethod,
    IPSI_CMP_CONNECTION_TYPE_E enConnectionType);

/**
 * @defgroup IPSI_CMP_httpSetConnectionTimeOut
 * @ingroup HTTP_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_httpSetConnectionTimeOut(
 * const IPSI_CMP_TRANS_METH_S *pstTransMethod,
 * SEC_UINT uiSecs)
 * @endcode
 *
 * @par Purpose
 * This function is used to set the connection timeout value.
 *
 * @par Description
 * IPSI_CMP_httpSetConnectionTimeOut function is used to set
 * the connection time out for the http method.
 * If no message is received by the connection within this period,
 * the connection is closed due to timeout.
 *
 * @param[in] pstTransMethod Pointer to IPSI_CMP_TRANS_METH_S.[NA/NA]
 * @param[in] uiSecs Timeout value in seconds.[10secs-180secs/60secs]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * -The default connection timeout value is 60 seconds.
 * The maximum timeout value is 180 seconds and the minimum
 * timeout value is 10 seconds. \n
 * -IPSI_CMP_httpSetConnectionTimeOut should be called only in the main thread before IPSI_CMP_httpMethodInit.
 *
 */
CMPLINKDLL SEC_INT IPSI_CMP_httpSetConnectionTimeOut(const IPSI_CMP_TRANS_METH_S *pstTransMethod, SEC_UINT uiSecs);

/**
 * @defgroup IPSI_CMP_httpSetMethodConfigParam
 * @ingroup HTTP_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_httpSetMethodConfigParam(const IPSI_CMP_TRANS_METH_S *pstTransMethod,
 * SEC_UINT uiConfig,const void *pvValue)
 * @endcode
 *
 * @par Purpose
 * This function is used to set the HTTP method configuration .
 *
 * @par Description
 * IPSI_CMP_httpSetMethodConfigParam function is used to set HTTP method configuration. \n
 * When uiConfig = IPSI_HTTP_CLIENT_SERVER_WAIT_TILL_TH_AUTO_EXIT,
 * then pvValue should be of type IPSI_CMP_HTTP_CONFIG_WAIT_TILL_THREAD_AUTOEXIT.
 * To enable the feature of making the  IPSI_CMP_httpFree API (invoked by APP thread) to wait
 * until all threads free resources and exits by itself,
 * value of the IPSI_CMP_HTTP_CONFIG_WAIT_TILL_THREAD_AUTOEXIT->bWaitTillThAutoExit should be IPSI_CMP_TRUE.
 * Also, if APP wants CMP stack to invoke pfBlockWaitThreadExit callback when this feature is enabled,
 * then IPSI_CMP_HTTP_CONFIG_WAIT_TILL_THREAD_AUTOEXIT->bInvokeDelThClbk should be set to IPSI_CMP_TRUE.
 *
 * @param[in] pstTransMethod Pointer to IPSI_CMP_TRANS_METH_S.[NA/NA]
 * @param[in] uiConfig Specifies the Configuration parameter type.[NA/NA]
 * @param[in] pvValue Specifies the pointer to the value corresponding to
 * IPSI_CMP_HTTP_CONFIG_WAIT_TILL_THREAD_AUTOEXIT type.[NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * When using DOPRA and this feature, it is suggested to use IPSI_CMP_httpMethodInit API from any other than
 * VOS root task and only after PID init is success. This is to avoid a hang in IPSI_CMP_httpMethodInit API,
 * when internal failure happens.
 * When internal failure happens in IPSI_CMP_httpMethodInit API, it attempts to delete the threads (if created before).
 * So, it waits until the threads exit by itself. However, it keeps waiting since the threads are not spawned by DOPRA,
 * until VOS root task is suspended. Since threads are not spawned, threads will not auto exit.
 * Therefore, IPSI_CMP_httpMethodInit API will keep waiting and will hang.
 * Once this feature is enabled, it cannot be disabled during runtime using this API.
 * By default,this feature is disabled.
 *
 *
 */
CMPLINKDLL SEC_INT IPSI_CMP_httpSetMethodConfigParam(const IPSI_CMP_TRANS_METH_S *pstTransMethod, SEC_UINT uiConfig,
    const void *pvValue);

/**
 * @defgroup IPSI_CMP_httpSetAsynRecvFlag
 * @ingroup HTTP_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_httpSetAsynRecvFlag(
 * IPSI_CMP_TRANS_METH_S *pstTransMethod,
 * SEC_UINT uiFlag
 * );
 * @endcode
 *
 * @par Purpose
 * This function is used to set the HTTP method configuration .
 *
 * @par Description
 * IPSI_CMP_httpSetAsynRecvFlag function is used to set HTTP method configuration. \n
 *
 * @param[in] pstTransMethod Pointer to IPSI_CMP_TRANS_METH_S.[NA/NA]
 * @param[in] uiFlag Asyn flag. 0:close, 1:open. default: 0.[NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 */
CMPLINKDLL SEC_INT IPSI_CMP_httpSetAsynRecvFlag(IPSI_CMP_TRANS_METH_S *pstTransMethod, SEC_UINT uiFlag);

/**
 * @defgroup IPSI_CMP_httpSetTickTimeLen
 * @ingroup HTTP_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_httpSetTickTimeLen(SEC_UINT uiMilSec);
 * @endcode
 *
 * @par Purpose
 * This function is used to set the HTTP tick time len .
 *
 * @par Description
 * IPSI_CMP_httpSetTickTimeLen function is used to set the HTTP tick time len . \n
 * Concurrent modifications are not supported. \n
 *
 * @param[in] uiMilSec HTTP tick time len. [10ms, 60000ms] or 0. default: 1000ms.
 *                     If uiMilSec = 0, the default setting(1000ms) is restored.[NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 */
CMPLINKDLL SEC_INT IPSI_CMP_httpSetTickTimeLen(SEC_UINT uiMilSec);

/**
 * @defgroup IPSI_CMP_httpGetTickTimeLen
 * @ingroup HTTP_Functions
 * @par Prototype
 * @code
 * SEC_UINT IPSI_CMP_httpGetTickTimeLen(SEC_VOID);
 * @endcode
 *
 * @par Purpose
 * This function is used to get the HTTP tick time len .
 *
 * @par Description
 * IPSI_CMP_httpGetTickTimeLen function is used to get the HTTP tick time len . \n
 *
 * @param[in] [NA/NA]
 * @param[out] [NA/NA]
 *
 * @retval SEC_INT HTTP tick time len. [10ms, 60000ms].[NA/NA]
 */
CMPLINKDLL SEC_UINT IPSI_CMP_httpGetTickTimeLen(SEC_VOID);

/**
 * @defgroup IPSI_CMP_httpSetRspNotifyEnterQueueCallback
 * @ingroup HTTP_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_httpSetRspNotifyEnterQueueCallback(PfCmpRspNotifyEnterQueue pstHttpRspNotify);
 * @endcode
 *
 * @par Purpose
 * This function is used to set response notify callback .
 *
 * @par Description
 * IPSI_CMP_httpSetRspNotifyEnterQueueCallback function is used to set response notify callback . \n
 *
 * @param[in] pstHttpRspNotify response notify callback.[NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 */
CMPLINKDLL SEC_INT IPSI_CMP_httpSetRspNotifyEnterQueueCallback(PfCmpRspNotifyEnterQueue pstHttpRspNotify);

/**
 * @defgroup IPSI_CMP_httpSendDataReq
 * @ingroup HTTP_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_httpSendDataReq(
 * IPSI_CMP_TRANS_METH_S *pstTransMethod,
 * const SEC_UCHAR *pucBuf,
 * SEC_UINT uiLen,
 * SEC_VOID *pUserData
 * );
 * @endcode
 *
 * @par Purpose
 * This function is used to send http ocsp request .
 *
 * @par Description
 * IPSI_CMP_httpSendDataReq function is used to send http ocsp request . \n
 *
 * @param[in] pstTransMethod Pointer to IPSI_CMP_TRANS_METH_S.[NA/NA]
 * @param[in] pucBuf Pointer to SEC_UCHAR.[NA/NA]
 * @param[in] uiLen Buffer length.[NA/NA]
 * @param[in] pUserData This is a pointer that hold the user defined data.
 *                      It will output in PfCmpRspNotifyEnterQueue.[NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 */
CMPLINKDLL SEC_INT IPSI_CMP_httpSendDataReq(
    IPSI_CMP_TRANS_METH_S *pstTransMethod, const SEC_UCHAR *pucBuf, SEC_UINT uiLen, SEC_VOID *pUserData);

/**
 * @defgroup IPSI_CMP_httpSetSourceAddrEx
 * @ingroup HTTP_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_httpSetSourceAddrEx(
 * const IPSI_CMP_TRANS_METH_S *pstTransMethod,
 * const SEC_CHAR *pcSrcIp,
 * const SEC_VOID *pvAuxData
 * );
 * @endcode
 *
 * @par Purpose
 * This function is used to set the source address and aux data in http transport method.
 *
 * @par Description
 * IPSI_CMP_httpSetSourceAddrEx function is used to set
 * the source address in http transport method, same address will be used as
 * source address of HTTP message.
 *
 * @param[in] pstTransMethod Pointer to IPSI_CMP_TRANS_METH_S [NA/NA]
 * @param[in] pcSrcIp IP addr of the client [NA/NA]
 * @param[in] pvAuxData This is a pointer that hold the user defined data.
 *                      It will output in PfCmpSslCreatAndConfigClientCtx.[NA/NA]
 * @param[out] NA NA [NA/NA]
 *
 * @retval SEC_INT On successful execution [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * IPV4 And IPV6 address are supported.
 * For Ipv4 User need to pass simple string. Ex "10.100.xx.xx" .
 * For IPV6 addr User need to pass the Ipv6 string with '[' & ']'
 * Example : "[a353::3:fdbc]"
 * Server Ip should be passed as string i.e. it should be null termintaed.
 */
CMPLINKDLL SEC_INT IPSI_CMP_httpSetSourceAddrEx(
    const IPSI_CMP_TRANS_METH_S *pstTransMethod, const SEC_CHAR *pcSrcIp, const SEC_VOID *pvAuxData);

/**
 * @defgroup IPSI_CMP_httpSetSslCreatAndConfigClientCtxCallbacks
 * @ingroup HTTP_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_httpSetSslCreatAndConfigClientCtxCallbacks(const PfCmpSslCreatAndConfigClientCtx pstSslCb);
 * @endcode
 *
 * @par Purpose
 * This function is used to set the creat and config client context callbacks.
 *
 * @par Description
 * IPSI_CMP_httpSetSslCreatAndConfigClientCtxCallbacks function is used
 * to set the creat and config client context callbacks.
 *
 * @param[in] pstSslCb Pointer to SSL callback . [NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * This function should be called in the main thread before
 * spawning any https thread.
 */
CMPLINKDLL SEC_INT IPSI_CMP_httpSetSslCreatAndConfigClientCtxCallbacks(const PfCmpSslCreatAndConfigClientCtx pstSslCb);
#ifdef __cplusplus
}
#endif

#endif /* IPSI_CMP_HTTP_H */
