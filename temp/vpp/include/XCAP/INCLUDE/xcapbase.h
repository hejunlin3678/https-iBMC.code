/* *****************************************************************************

    Copyright (c) Huawei Technologies Co., Ltd. 2008-2019. All rights reserved.


    Description:   basic  type define.


    Function List: NA


    Author:  L36241,W57322


    Create: 2008-4-14

***************************************************************************** */
#ifndef XCAPBASE_H
#define XCAPBASE_H

/* Added since xcapbase.h file is dependent on xcaptypes.h */
#include "xcaptypes.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* XCAP 协议栈版本号 */
#ifndef XCAP_VERSION
#define XCAP_VERSION ""
#endif

#define XCAP_GET_VPP_STACK_VER XCAP_VERSION
#define XCAP_GET_VPP_STACK_TEST_VER XCAP_VERSION

/* [DTS2018101104911][12-10-2018][2nd and 4th param changes][Start] */
/* String data structure */
typedef struct _XcapString {
    XCAP_UINT32 ulLen; /* Length of the string */
#ifdef XCAP_64BIT_ALIGNED
    XCAP_UINT32 ulSpare; /* for 64 bit alignment */
#endif
    XCAP_CHAR *pcBuf; /* Pointer to the string */
} XcapString;

/* [DTS2018101104911][12-10-2018][End] */
#define XCAP_IP_ADDR_TYPE_IPV4 0 /* Ipv4  */
#define XCAP_IP_ADDR_TYPE_IPV6 1 /* Ipv6  */

#define XCAP_IPV4_BYTE_NUM 4  /* IPV4地址的字节数 */
#define XCAP_IPV6_BYTE_NUM 16 /* IPV6地址的字节数 */
#define XCAP_IPV4_DWORD_NUM 1 /* IPV4地址的字数   */
#define XCAP_IPV6_DWORD_NUM 4 /* IPV6地址的字数   */

/* [DTS2013050204972][SOAP-65][n00742771][Start] */
/* Maximum thread delay in Millisecond for XcapApiSetThreadDelay API */
#define XCAP_THREAD_MAX_DELAY_MILLISECOND 2000

/* Maximum thread delay in Microsecond for XcapApiSetThreadDelay API */
#define XCAP_THREAD_MAX_DELAY_MICROSECOND 2000000

/* [DTS2013050204972][SOAP-65][n00742771][End] */
/* Exports global variables & functions for windows shared library generation */
#define XCAPPUBFUN
#define XCAPPUBVAR

/* Windows platform with MS compiler */
#if defined(_WIN32) && defined(_MSC_VER)
#undef XCAPPUBFUN
#undef XCAPPUBVAR
#ifdef XCAP_DYNAMIC
#define XCAPPUBFUN __declspec(dllexport)
#define XCAPPUBVAR __declspec(dllexport)
#else
#define XCAPPUBFUN
#define XCAPPUBVAR
#endif
#endif

/* Structure to store Address */
typedef struct _XcapIpAddr {
    XCAP_UINT16 usType;    /* IPv4,IPv6类型 */
    XCAP_UINT16 usPort;    /* 主机字节序 */
    XCAP_UINT32 uiScopeId; /* ipv6使用的scope_id */
    union {
        XCAP_UINT32 ulIpV4Addr; /* 按照网络字节序填写，否则会带来字节序的问题，
                                   建议按照ucIpV4Addr方式填写 */
        XCAP_UINT8 ucIpV4Addr[XCAP_IPV4_BYTE_NUM];
        XCAP_UINT32 ulIpV6Addr[XCAP_IPV6_DWORD_NUM];
        XCAP_UINT8 ucIpV6Addr[XCAP_IPV6_BYTE_NUM];
    } u;
} XcapIpAddr;

/* Structure to fill socket data */
typedef struct _XcapSockDataS {
    XCAP_INT32 iLevel;
    XCAP_INT32 iOptName;
    XCAP_VOID *pvOptVal;
    XCAP_INT32 iOptLen;
#ifdef XCAP_64BIT_ALIGNED
    XCAP_UINT32 ulSpare;
#endif
} XcapSockDataS;

/* [VPP-11827][d00902689][Start] */
typedef enum _EN_XCAP_STAT_REASON {
    /* Encoding failed due to invalid msg having https protocol error. */
    XCAP_NO_ERR = 0,

    /* Encoding failed due to invalid msg having https protocol error. */
    XCAP_ENCODE_ERR = 1,

    /* Decoding failedd due to invalid msg having https protocol error. */
    XCAP_DECODE_ERR = 2,

    /* Send /recv failure at transport level (socket send /recv function) */
    XCAP_TRANSPORT_ERR = 3,

    /* Link closed */
    XCAP_LINK_CLOSE = 4,

    /* Internal error like malloc failure, timer-start failure */
    XCAP_INTERNAL_ERR = 5,

    /* There is no request messsage for the received response msg. */
    XCAP_NO_REQ_ERR = 6,

    /* 1XX response msg which are ignored by stack. */
    XCAP_1XX_RSP = 7,

    /* Traffic control */
    XCAP_TRAFFIC_CONTROL = 8,

    /* User delete (XcapApiCloseTransaction) */
    XCAP_USER_DELETE = 9,

    /* Ruest timeout error of client */
    XCAP_REQ_TIMEOUT_ERR = 10,

    /* APP response time out error of server */
    XCAP_APP_RSP_TIMEOUT_ERR = 11,

    /* Errors happened during TLS upgrade operations */
    XCAP_TLS_UPGRADE_ERR = 12,

    /* For server: OPTIONS (mandatory TLS upgrade) request are handled
    internally by stack and hence not reported to APP.
    For client: NA */
    XCAP_REQ_HNDLED_INTERNALLY = 13,

    /* For Client: 101 rsp (mandatory TLS upgrade) handled internally by stack,
    hence not reported to APP.
    For server: NA */
    XCAP_RSP_HNDLED_INTERNALLY = 14,

    XCAP_STAT_REASON_END
} EN_XCAP_STAT_REASON;

/* Statistics types.
The below equations might be incorrect, during processing of request/response,
since some stat counter has to be updated yet. Below equation will be correct,
when xcap client/server is idle for the period more than link timeout period of
last link.

--For Client
1. Total Number of request messages sent to network =
XCAP_CLIENT_STAT_SEND_REQ_SUCCESS_TO_NETWORK
 + XCAP_CLIENT_STAT_SEND_INTERMEDIATE_REQ_INTERNALLY_SUCCESS

2. XCAP_CLIENT_STAT_SEND_REQ_FROM_APP =
XCAP_CLIENT_STAT_SEND_REQ_SUCCESS_TO_NETWORK + XCAP_CLIENT_STAT_SEND_REQ_FAIL

3. XCAP_CLIENT_STAT_REPORT_RSP_SUCCESS_TO_APP =
XCAP_CLIENT_STAT_RECV_RSP_SUCCESS_FROM_NETWORK - XCAP_CLIENT_STAT_RSP_DISCARD

Below equation will be correct, only when xcap client is idle for the period
more than transaction timeout period from lastly sent request.
4. XCAP_CLIENT_STAT_SEND_REQ_SUCCESS_TO_NETWORK =
XCAP_CLIENT_STAT_REPORT_RSP_SUCCESS_TO_APP + XCAP_CLIENT_STAT_REQ_DISCARD

--For Server
1. Total Number of response messages sent to network =
XCAP_SERVER_STAT_SEND_RSP_SUCCESS_TO_NETWORK
 + XCAP_SERVER_STAT_SEND_ERR_RSP_INTERNALLY_SUCCESS
 + XCAP_SERVER_STAT_SEND_INFORM_RSP_INTERNALLY_SUCCESS

2. XCAP_SERVER_STAT_SEND_RSP_FROM_APP =
XCAP_SERVER_STAT_SEND_RSP_SUCCESS_TO_NETWORK + XCAP_SERVER_STAT_SEND_RSP_FAIL
+ XCAP_SERVER_STAT_RSP_DISCARD

3. XCAP_SERVER_STAT_REPORT_REQ_SUCCESS_TO_APP =
XCAP_SERVER_STAT_RECV_REQ_SUCCESS_FROM_NETWORK
- XCAP_SERVER_STAT_REQ_DISCARD_BEFORE_REPORT_APP

Below equation will be correct, only when APP has sent response for all
request reported to APP by XCAP server and xcap server is idle for the period
more than transaction timeout period, from lastly reported request.
4. XCAP_SERVER_STAT_REPORT_REQ_SUCCESS_TO_APP =
XCAP_SERVER_STAT_SEND_RSP_SUCCESS_TO_NETWORK
+ XCAP_SERVER_STAT_REQ_DISCARD_AFTER_REPORT_APP */
typedef enum _EN_XCAP_STAT_TYPE {
    /* ------------CLIENT Statistics parameters-------------------------- */
    /* Statistics--Client: Number of requests sent from the upper layer.
    Stat callback with this type will be invoked just after XcapApiDoRequest/
    XcapHttpApiDoRequest API is invoked by APP. */
    XCAP_CLIENT_STAT_SEND_REQ_FROM_APP = 0,

    /* Statistics--Client: Actual number of requests sent to network
    successfully, which is sent by the APP. It includes only the number of
    request sent by the APP (upper layer). */
    XCAP_CLIENT_STAT_SEND_REQ_SUCCESS_TO_NETWORK = 1,

    /* Statistics--Client: Number of responses received from network.
    After receiving complete https response message and parse is success, stat
    callback will be invoked with this stat type, just after parse success and
    before doing other operations.
    If received message is invalid https message and parse failed, stat should
    not be incremented, hence stat callback will not be invoked.  */
    XCAP_CLIENT_STAT_RECV_RSP_SUCCESS_FROM_NETWORK = 2,

    /* Statistics--Client: Number of responses reported to the
    APP (upper layer). Stat callback with this type will be invoked, just after
    stack invokes PTXcapOnResponse/PTXcapHttpOnResponse callback. */
    XCAP_CLIENT_STAT_REPORT_RSP_SUCCESS_TO_APP = 3,

    /* Statistics--Client: Number of send-failure happened for the request sent
    by APP, due to below reasons. It does not count failure of internally sent
    request msgs by stack.
    1. XCAP_ENCODE_ERR: Invalid msg having https protocol error.
    2. XCAP_INTERNAL_ERR: Internal error like malloc failure, timer-start
       failure
    3. XCAP_TRANSPORT_ERR: Transport level error (socket send function error).
    4. XCAP_REQ_TIMEOUT_ERR: Request timeout error (waiting to process for
       long time and hence not sent to network)
    5. XCAP_TLS_UPGRADE_ERR: Upgrading to TLS connection failed, before sending
       the request.
    6. XCAP_LINK_CLOSE: Number of request failed to send due to link close.
       Link will be closed
       -- when other request msg or rsp msg has connection = close header set.
       -- when internal error happen when processing response msg and link
       closed. Other unsent requests will get this error.
       -- One request got timed out and link closed. Other unsent requests will
       get this error. */
    XCAP_CLIENT_STAT_SEND_REQ_FAIL = 4,

    /* Statistics--Client: Number of response recv failed, due to below reasons:
    1. XCAP_TRANSPORT_ERR: Transport level error (socket recv function error).
    2. XCAP_INTERNAL_ERR: Internal error like malloc failure, timer-start
       failure
    3. XCAP_DECODE_ERR: Invalid msg having https protocol error. */
    XCAP_CLIENT_STAT_RECV_RSP_FAIL = 5,

    /* Statistics--Client: Number of responses discarded and not reported to APP
    due to below reasons:
    1. XCAP_NO_REQ_ERR: No request messsage for the response.
    2. XCAP_1XX_RSP: 100 response msgs which are ignored by client
       without reporting to APP. Stack wait for other than 100 rsp again. It
       does not discard the request.
    3. XCAP_INTERNAL_ERR: Internal error like malloc failure, timer-start
       failure
    4. XCAP_TLS_UPGRADE_ERR: Not able to upgrade to TLS connection
    5. XCAP_RSP_HNDLED_INTERNALLY: 101 rsp (mandatory TLS upgrade) handled
    internally by stack, hence not reported to APP. */
    XCAP_CLIENT_STAT_RSP_DISCARD = 6,

    /* Statistics--Client: Number of requests discarded due to below reasons:
    1. XCAP_REQ_TIMEOUT_ERR: Timeout happened, since stack did not
       receive response message on time from peer.
    2. XCAP_LINK_CLOSE: Number of request discarded due to link close.
       Link will be closed
       -- when other request msg or rsp msg has connection = close header set.
       -- when internal error happen when processing response msg and link
          closed.
       In such case, all req msgs for which rsp yet to receive,
       are discarded.
    3. XCAP_USER_DELETE: APP has invoked XcapApiCloseTransaction/
       XcapHttpApiCloseTransaction to delete the transaction. */
    XCAP_CLIENT_STAT_REQ_DISCARD = 7,

    /* Statistics--Client: Number of intermidiate request messages are created
    and sent successfully by stack internally. Req method: OPTIONS  */
    XCAP_CLIENT_STAT_SEND_INTERMEDIATE_REQ_INTERNALLY_SUCCESS = 8,

    /* Statistics--Client: Number of intermidiate request message created,
    internally by stack, but failed to send. Req method: OPTIONS  */
    XCAP_CLIENT_STAT_SEND_INTERMEDIATE_REQ_INTERNALLY_FAIL = 9,

    /* ------------SERVER Statistics parameters-------------------------- */
    /* Statistics--Server: Number of responses sent from the upper layer.
    Stat callback with this type will be invoked just after XcapApiDoResponse/
    XcapHttpApiDoResponse API is invoked by APP. */
    XCAP_SERVER_STAT_SEND_RSP_FROM_APP = 21,

    /* Statistics--Server: Actual number of responses sent to network
    successfully, which is sent by the APP. It includes only the number of
    responses sent by APP (upper layer). */
    XCAP_SERVER_STAT_SEND_RSP_SUCCESS_TO_NETWORK = 22,

    /* Statistics--Server: Number of requests received from network.
    After receiving complete https request message and parse is success, stat
    callback will be invoked with this stat type, just after parse success and
    before doing other operations.
    If received message is invalid https message and parse failed, stat should
    not be incremented, hence stat callback will not be invoked.  */
    XCAP_SERVER_STAT_RECV_REQ_SUCCESS_FROM_NETWORK = 23,

    /* Statistics--Server: Number of requests reported to the
    APP (upper layer). Stat callback with this type will be invoked, just after
    stack invokes PTXcapOnRequest/PTXcapHttpOnRequest callback. */
    XCAP_SERVER_STAT_REPORT_REQ_SUCCESS_TO_APP = 24,

    /* Statistics--Server: Number of send-failure happened for the responses
    sent by APP, due to below reasons. It does not count failure of internally
    sent response msgs by stack.
    1. XCAP_ENCODE_ERR: Invalid msg having https protocol error.
    2. XCAP_INTERNAL_ERR: Internal error like malloc failure, timer-start
       failure
    3. XCAP_TRANSPORT_ERR: Transport level error (socket send function error).
    4. XCAP_LINK_CLOSE: Number of response failed to send due to link close.
       Link will be closed
       -- when other request msg or rsp msg has connection = close header set.
       -- when internal error happen while processing response msg and link
       closed. Other unsent responses will get this error.

    --When send-response API returns error, then APP may try to send the rsp
    message again. Hence for this case, stack does not create and send error
    response msg internally and bErrRspSent flag will be set to XCAP_FALSE.
    --When send-response API returns success, stack may or may not have sent
    response msg to network. It depends on asyn case (worker thread > 0)
    or syn case (worker thread = 0) and other factor like order of response.
    If stack has not sent the response msg to network while send-response API
    is invoked and later when it tried to send, it failed. In this case, stack
    tries to send error response message internally and discard the request.
    When error response msg is sent successfully, bErrRspSent flag will
    be set to XCAP_TRUE. But when internal error response send failed,
    bErrRspSent flag will be set to XCAP_FALSE. */
    XCAP_SERVER_STAT_SEND_RSP_FAIL = 25,

    /* Statistics--Server:  Number of request recv failed, due to below reasons:
    1. XCAP_TRANSPORT_ERR: Transport level error (socket recv function error).
    2. XCAP_INTERNAL_ERR: Internal error like malloc failure, timer-start
       failure
    3. XCAP_DECODE_ERR: Invalid msg having https protocol error.

    For some errors, stack sends response msg to client internally.
    If error rsp sent successfully by stack, bErrRspSent flag will be XCAP_TRUE.
    Otherwise ErrRspSent flag will be XCAP_FALSE. */
    XCAP_SERVER_STAT_RECV_REQ_FAIL = 26,

    /* Statistics--Server: Server has received and parsed the request msg
    successfully, but not reported it to APP by request-indication callback and
    the msg is discarded. Reasons are as below:
    1. XCAP_TRAFFIC_CONTROL: APP wants to discard the req msg to
       control traffic (flow control)
    2. XCAP_INTERNAL_ERR: Internal error like malloc failure, timer-start
       failure
    3. XCAP_TLS_UPGRADE_ERR: Errors happened during TLS upgrate operations
    4. XCAP_REQ_HNDLED_INTERNALLY: OPTIONS (mandatory TLS upgrade) request
    are handled internally by stack and hence not reported to APP.

    For some error, stack send response msg to client internally.
    If error rsp sent successfully by stack, bErrRspSent flag will be XCAP_TRUE.
    Otherwise bErrRspSent flag will be XCAP_FALSE. */
    XCAP_SERVER_STAT_REQ_DISCARD_BEFORE_REPORT_APP = 27,

    /* Statistics--Server: Number of requests which were reported to APP,
       are discarded due to below reasons
    1. XCAP_LINK_CLOSE: Link closed. Link will be closed
       --when other request msg or rsp msg has connection = close header set.
       --when transport/internal error happened for other request and link
       closed. In such case, all req msgs for which rsp not sent yet, are
       discarded.
    2. XCAP_APP_RSP_TIMEOUT_ERR:  APP did not send response on time.
    3. XCAP_USER_DELETE: APP has invoked XcapApiCloseTransaction/
       XcapHttpApiCloseTransaction to delete the transaction.

    For some error, stack send response msg to client internally.
    If error rsp sent successfully by stack, bErrRspSent flag will be XCAP_TRUE.
    Otherwise bErrRspSent flag will be XCAP_FALSE. */
    XCAP_SERVER_STAT_REQ_DISCARD_AFTER_REPORT_APP = 28,

    /* Statistics--Server: Number of responses from the APP are discarded due
    to below reasons:
    1. XCAP_1XX_RSP: 1XX response msg which are ignored by server. stack just
    returns success from send-response api. It does not discard the request
    and expect APP to send other than 1xx response msg. */
    XCAP_SERVER_STAT_RSP_DISCARD = 29,

    /* Statistics--Server: Number of error response messages created and sent
    successfully by stack internally. Rsp code: 400, 408, 500, 501, 503  */
    XCAP_SERVER_STAT_SEND_ERR_RSP_INTERNALLY_SUCCESS = 30,

    /* Statistics--Server: Number of error response messages created internally
    by stack, but failed to send. Rsp code: 400, 408, 500, 501, 503  */
    XCAP_SERVER_STAT_SEND_ERR_RSP_INTERNALLY_FAIL = 31,

    /* Statistics--Server: Number of information response message created
    and sent successfully by stack internally. Rsp code: 100, 101  */
    XCAP_SERVER_STAT_SEND_INFORM_RSP_INTERNALLY_SUCCESS = 32,

    /* Statistics--Server: Number of information response message created
    internally by stack, but failed to send. Rsp code: 100, 101 */
    XCAP_SERVER_STAT_SEND_INFORM_RSP_INTERNALLY_FAIL = 33,

    XCAP_STAT_END
} EN_XCAP_STAT_TYPE;

/* Server statistics information structure */
typedef struct _XcapClientStatInfoS {
    /* Local address. Mainly used for req/rsp msg stat type for network.
    For future use, currently it will be NULL pointer. */
    IN const XcapIpAddr *pstLocalAddr;

    /*  Remote address. Mainly used for req/rsp msg stat type for network.
    Remote Server IP, in case stat type is of client and it is related to msg
    being sent/received to/from network.
    This can be NULL, when remote address details are not available, while
    invoking stat callback for some stat type. */
    IN const XcapIpAddr *pstRemAddr;

    /* Reason for reporting the statistics, EN_XCAP_STAT_REASON.
    Valid only for failure and discard stat types.
    If APP want to check for particular statistics reason, then APP should
    check with logical & operation. For example, if APP want to check whether
    it is inner error reason or not, should check like below:
    if XCAP_INTERNAL_ERR == (ulStatReason & XCAP_INTERNAL_ERR) */
    IN XCAP_UINT32 ulStatReason;

#ifdef XCAP_64BIT_ALIGNED
    XCAP_UINT32 ulSpare; /* For 64 bit allignement */
#endif

    /* Pointer pointing to relevant structure providing more statistical
    information for ulStatType. It is for future use, currently it will be
    NULL pointer. */
    IN XCAP_VOID *pvStatInfo;
} XcapClientStatInfoS;

/* Server statistics information structure */
typedef struct _XcapServerStatInfoS {
    /* Local address. Mainly used for req/rsp msg stat type for network.
    For future use, currently it will be NULL pointer. */
    IN const XcapIpAddr *pstLocalAddr;

    /*  Remote address. Mainly used for req/rsp msg stat type for network.
    Remote Client IP, in case stat type is of server and it is related to msg
    being sent/received to/from network.
    This can be NULL, when remote address details are not available, while
    invoking stat callback for some stat type. */
    IN const XcapIpAddr *pstRemAddr;

    /* Reason for reporting the statistics, EN_XCAP_STAT_REASON.
    Valid only for failure and discard stat types.
    If APP want to check for particular statistics reason, then APP should
    check with logical & operation. For example, if APP want to check whether
    it is inner error reason or not, should check like below:
    if XCAP_INTERNAL_ERR == (ulStatReason & XCAP_INTERNAL_ERR) */
    IN XCAP_UINT32 ulStatReason;

    /*  When request is dropped by server internally OR sending APP rsp failed
    in asyn mode (work thread > 0), server internally sends error rsp to client.
    If error rsp sent successfully by stack, bErrRspSent flag will be XCAP_TRUE.
    Otherwise ErrRspSent flag will be XCAP_FALSE. Please check the stat type
    for more detail. When stat callback is invoked for failure stat type,
    this flag indicates whether error response is sent by server or not. */
    XCAP_BOOL bErrRspSent;

    /* Pointer pointing to relevant structure providing more statistical
    information for ulStatType. It is for future use, currently it will be
    NULL pointer. */
    IN XCAP_VOID *pvStatInfo;
} XcapServerStatInfoS;

typedef XCAP_VOID (*PTXcapStat)(IN XCAP_HANDLE hStackHandle, /* XCAP instance handle */

    /* Handle of upper layer user */
    IN XCAP_HANDLE ulAppId,

    /* XCAP_TRUE, if callback invoked for reporting client statistics.
    XCAP_FALSE, if callback invoked for reporting server statistics. */
    IN XCAP_BOOL bIsClient,

    /* EN_XCAP_STAT_TYPE */
    IN XCAP_UINT32 ulStatType,

    /* Client statistics information details, It is valid when
    bIsClient is XCAP_TRUE. */
    IN const XcapClientStatInfoS *pstClientStatInfo,

    /* Server statistics information details, It is valid when
    bIsClient is XCAP_FALSE. */
    IN const XcapServerStatInfoS *pstServerStatInfo,

    /* For Future use */
    IN XCAP_VOID *pvAuxData);

/* [VPP-11827][d00902689][End] */
/* [VPP-4472][SOAP-68][d00902689][Start] */
/* Connection Info structure */
typedef struct _XcapConnInfoS {
    XCAP_BOOL bIsServer;     /* Server or client connection */
    XCAP_BOOL bIsPersistent; /* Persistent or Non persistent connection */
    XcapIpAddr stLocAddr;    /* Local address */
    XcapIpAddr stRemAddr;    /* Remote address */
} XcapConnInfoS;

/* [VPP-4472][SOAP-68][d00902689][End] */
/* [DTS2016060704445][max-alloc-limit][k71093][start] */
/* 1 GB */
#define XCAP_MAX_BODY_SIZE_LIMIT 1073741824

/* [DTS2017022313118][2017-04-06][Integer overflow issue] begin */
#define XCAP_MAX_CHUNK_COUNT_LIMIT 2000

/* [DTS2016060704445][max-alloc-limit][k71093][end] */
/* 100 MB */
#define XCAP_DEFAULT_MAX_BODY_SIZE_LIMIT 104857600

#define XCAP_MAX_HDR_NUM_LIMIT 10000

#define XCAP_DEFAULT_MAX_HEADER_NUM 1000

/* 2 MB */
#define XCAP_MAX_SEND_BUF_SIZE 2097152

/* 2 MB */
#define XCAP_MAX_RECV_BUF_SIZE 2097152

/* [DTS2016060704445][max-alloc-limit][k71093][End] */
/* [VPP-12363][NE40-Perf-Req][Start] */
/* Listener thread */
#define XCAP_LISTENER_THREAD 1

/* Worker thread */
#define XCAP_WORKER_THREAD 2

/* Tick task thread */
#define XCAP_TICK_TASK_THREAD 3

/* Event type */
typedef enum _EN_XCAP_EVENT_TYPE {
    /* Thread resume event. When this event received, thread getting the event
    starts processing */
    XCAP_EVENT_RESUME_PROCESSING = (1 << 0)
} EN_XCAP_EVENT_TYPE;

/* Event flags */
/* Defines that if all of the monitored events are received then proceed. */
#define XCAP_EVENT_FLAG_ALL_EVENT 0x00000000

/* Wait till the event/events is/are received till the timeout period */
#define XCAP_EVENT_FLAG_WAIT_ON_EVENT 0x00000000

/* Do not wait for an event irrespective of the timeout period */
#define XCAP_EVENT_FLAG_NO_WAIT 0x00000001

/* Defines that if any of the monitored event is received then proceed */
#define XCAP_EVENT_FLAG_ANY_EVENT 0x00000002

/* Async event structure, same as VRP asynsockset structure */
typedef struct _XcapAsynSockSet {
    /* When any socket event occured, SOCK task will put WspAsynSockSet
    argument to this ulQueueID. */
    XCAP_ULONG ulQueueID;

    /* Pointer related to socket. For example link node */
    XCAP_HANDLE ulPointer;

    /* SOCK task will give XCAP_SE_ASYNC event to ulWakeTaskID task,
    when any socket event occured. */
    XCAP_ULONG ulWakeTaskID;

    /* This structure is exactly same as  asynsockset structure of VRP PDT.
    Dont add other member. */
} XcapAsynSockSet;

/* Async socket message structure. APP has to create asyn socket message as
    per this structure and provide to stack by writing to socket queue.
    This structure is same as VRP async message socket structure. */
typedef struct _XcapAsynSockMsg {
    /* Message type: XCAP_ASYN_MESSAGE */
    XCAP_ULONG ulMsgType;

    /* Stack provided pointer */
    XCAP_HANDLE ulPointer;

    /* Socket error */
    XCAP_LONG iError;

    /* Socket descriptor */
    XCAP_UINT16 usFd;

    /* Socket events: XCAP_ASYN_READ, XCAP_ASYN_WRITE, XCAP_ASYN_CONNECT,
    XCAP_ASYN_CLOSE, XCAP_ASYN_PEERCLOSE */
    XCAP_UINT16 usEventType;

    /* This structure is same as VRP async message socket structure, hence
    not alligned for 64 bit. PDT should use
    XCAP_CLIENT_SERVER_USE_APP_SOCK_TASK_FOR_SELECT  feature only on 32 bit
    systems. */
} XcapAsynSockMsg;

/* QUEUE flags - start  */
/* prority */
#define XCAP_Q_PRIOR ((XCAP_ULONG)1) /* VRP VOS_Q_PRIOR */

/* first in first out */
#define XCAP_Q_FIFO ((XCAP_ULONG)1 << 1) /* VRP VOS_Q_FIFO */

/* async queue */
#define XCAP_Q_ASYN ((XCAP_ULONG)1 << 2) /* VRP VOS_Q_ASYN */

/* syn queue */
#define XCAP_Q_SYN ((XCAP_ULONG)1 << 3) /* VRP VOS_Q_SYN */

#define XCAP_Q_WAIT ((XCAP_ULONG)1 << 30) /* VRP VOS_WAIT */

#define XCAP_Q_NO_WAIT ((XCAP_ULONG)1 << 31) /* VRP VOS_NO_WAIT */

/* Event written by VRP SOCK task, after writing socket
    event to sock queue. This value is got from VRP PDT. */
#define XCAP_SE_ASYNC 0x0020

/* Asyn message type used by VRP sock task, when
    write message to socket queue. This value is got from VRP PDT. */
#define XCAP_ASYN_MESSAGE 0X12345678

#define XCAP_ASYN_READ 1      /* Async read event. Value got from VRP PDT. */
#define XCAP_ASYN_WRITE 2     /* Async write event. Value got from VRP PDT. */
#define XCAP_ASYN_CONNECT 3   /* Async connect event. Value got from VRPPDT. */
#define XCAP_ASYN_CLOSE 4     /* Async close event. Value got from VRP PDT. */
#define XCAP_ASYN_ACCEPT 5    /* Async accept event. Value got from VRP PDT. */
#define XCAP_ASYN_PEERCLOSE 6 /* Async peerclose event. Value got from VRP PDT. */

#define XCAP_ASYN_ADD_SOCK 0x10    /* Add socket to sock task. */
#define XCAP_ASYN_DELETE_SOCK 0x20 /* Delete socket from sock task.  */

#define XCAP_ASYN_LISTEN_SOCK 0x30 /* Listen socket */
#define XCAP_ASYN_CLIENT_SOCK 0x40 /* Client socket  */
#define XCAP_ASYN_SERVER_SOCK 0x50 /* Server accepted socket */

typedef XCAP_UINT32 (*PTXcapSockIdToSockTask)(IN XCAP_SOCKET ulSocket, /* Socket Descriptor */
    IN XCAP_UINT32 uiSockeType,    /* XCAP_ASYN_LISTEN_SOCK,XCAP_ASYN_CLIENT_SOCK, XCAP_ASYN_SERVER_SOCK */
    IN XCAP_ULONG ulCmd,           /* XCAP_ASYN_ADD_SOCK or XCAP_ASYN_DELETE_SOCK */
    IN const XCAP_ULONG *pulArgP); /* Argument XcapAsynSockSet */

typedef struct _XcapConfigUseAPPSockTaskForSelect {
    /* Callback to add/delete client socket to APP SOCK task which
    monitors the socket event. This callback will be invoked by WSP
    client when socket is created. */
    PTXcapSockIdToSockTask pfSocketIdToSockTask;

    /* When this flag is set to SOAP_TRUE, wsp client will not use select socket
    function to look the read/write/exception event. If this flag is enabled,
    below operations will be performed by WSP client to look socket events.
    1. When client socket is created, it will be registered to APP SOCK task
    by invoking pfRegSocketIdToSockTask callback.
    2. Then APP SOCK task will start looking the events on this client
    socket. If any of WSP_ASYN_READ, WSP_ASYN_WRITE or WSP_ASYN_PEERCLOSE event
    happened on socket, then APP SOCK task will put the socket event
    details (WspAsynSockSet) into socket queue and write the WSP_SE_ASYNC
    event to wakeup task id.
    3. Wakeup task is nothing but worker thread. When worker thread receives
    WSP_SE_ASYNC event, it reads the socket queue and process the event. */
    XCAP_BOOL bUseAPPSockTaskInsteadSelect;

    /* For 64 bit alignment */
#ifdef XCAP_64BIT_ALIGNED
    XCAP_UINT32 ulSpare;
#endif
} XcapConfigUseAPPSockTaskForSelect;

/* [VPP-12363][NE40-Perf-Req][End] */
/* [VPP-19223][thr-auto-exit][start] */
typedef struct _XcapConfigDelAppApiWaitTillThAutoExit {
    /* Flag to enable feature of waiting in delete-app API until thread to be
    deleted will exit by itself. */
    XCAP_BOOL bDelAppApiWaitTillThAutoExit;

    /* If bDelAppApiWaitTillThAutoExit flag and bInvokeDelThClbk is enabled,
    then XCAP stack will invoke pfBlockWaitThreadExit callback.
    If bDelAppApiWaitTillThAutoExit flag and bInvokeDelThClbk is disabled, then
    XCAP stack will not invoke pfBlockWaitThreadExit callback. */
    XCAP_BOOL bInvokeDelThClbk;
} XcapConfigDelAppApiWaitTillThAutoExit;

/* [VPP-19223][thr-auto-exit][end] */
/* 服务模式 */
typedef enum _EN_XCAP_SERVICE_MODE {
    XCAP_SERVICE_MODE_CLIENT = 0, /* 客户端 */
    XCAP_SERVICE_MODE_SERVER,     /* 服务端 */
    XCAP_SERVICE_MODE_BOTH        /* 既充当客户端又充当服务端 */
} EN_XCAP_SERVICE_MODE;

/* 侦听模式 */
typedef enum _EN_XCAP_LISTEN_THREAD_MODE {
    XCAP_LISTEN_THREAD_MODE_SINGLE = 0, /* 单线程 */
    XCAP_LISTEN_THREAD_MODE_MUTIL       /* 多线程 */
} EN_XCAP_LISTEN_THREAD_MODE;

/* *****************************
系统消息驱动函数
1.单线程模式下所有的连接事件由此函数驱动
2.多线程模式下驱动用户接收消息并调用回调
***************************** */
/* 一个XCAP用户的最大侦听个数 */
/* NOTE: If value of XCAP_BASE_MAX_LISTEN_ADDR_NUM is not a multiple of 2,
   please takecare of 64BIT ALIGNMENT in structure XcapHttpAppInfo */
#define XCAP_BASE_MAX_LISTEN_ADDR_NUM 4
#define XCAP_MAX_LISTEN_ADDR_NUM XCAP_BASE_MAX_LISTEN_ADDR_NUM

/* [DTS2017022313118][2017-04-06][Integer overflow issue] begin */
#define XCAP_MAX_WORK_THREAD_NUM 1000
/* 上行消息每TICK处理个数 */
#define XCAP_BASE_PROC_UP_MSG_MAX_NUM_PER_TICK 1024

XCAPPUBFUN XCAP_VOID XcapApiTickTask(IN XCAP_HANDLE hStackHandle);

/* [vpp-9874][n00742771][start] */
XCAPPUBFUN XCAP_VOID XcapApiTickTaskEnd(IN XCAP_HANDLE hStackHandle);

/* [vpp-9874][n00742771][End] */
/* [VPP-22529][Del-Inst-Api-Invk-RunTime-Safe-Support][Start] */
XCAPPUBFUN XCAP_VOID XcapApiTickTaskEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle);

XCAPPUBFUN XCAP_VOID XcapApiTickTaskEndEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle);

/* [VPP-22529][Del-Inst-Api-Invk-RunTime-Safe-Support][End] */
/* ********************************
    获取XCAP协议栈版本号
********************************* */
XCAPPUBFUN XCAP_CHAR *XcapApiGetXcapStackVer(XCAP_VOID);

/* Test version of stack */
XCAPPUBFUN XCAP_CHAR *XcapApiGetXcapStackTestVer(XCAP_VOID);

/* *****************************
调试信息输出和控制接口
***************************** */
#define XCAP_DBG_PRINT 0X01
#define XCAP_DBG_INFO 0X02
#define XCAP_DBG_ERROR 0X04
#define XCAP_DBG_LOG 0X08
#define XCAP_DBG_API_FUNC_TRACE 0X10 /* API Function snippet */
#define XCAP_DBG_FUNC_TRACE 0X20     /* Function snippet */

typedef XCAP_VOID (*PTXcapSysDebugSend)(IN XCAP_HANDLE ulAppId, IN XCAP_UINT32 ulDebugType, IN XCAP_UINT32 ulDebugId,
    IN const XCAP_CHAR *pFormat, ...) XCAPFORMAT(printf, 4, 5); /* 4 fmt位置 5 可变参位置 */

/* [XCAP_CODEC_SUPPORT][VPP-26285][Start] */
typedef XCAP_VOID (*PTXcapCodecSysDebugSend)(IN XCAP_VOID *pvCodecCtx, IN XCAP_ULONG ulAppId,
    IN XCAP_UINT32 ulDebugType, IN XCAP_UINT32 ulDebugId, IN const XCAP_CHAR *pFormat, ...)
    XCAPFORMAT(printf, 5, 6); /* 5 fmt位置 6 可变参位置 */

/* [XCAP_CODEC_SUPPORT][VPP-26285][End] */
XCAPPUBFUN XCAP_ERR_CODE XcapApiRegDebugFunc(IN const PTXcapSysDebugSend pfDebugFunc);

/* Command used to enable/disable Maintenance Info information in XcapApiDebugControl */
typedef enum _EN_XCAP_DEBUG_CMD {
    XCAP_DEBUG_CMD_OPEN,
    XCAP_DEBUG_CMD_CLOSE
} EN_XCAP_DEBUG_CMD;

/* ulCommand:XCAP_DEBUG_CMD_OPEN,XCAP_DEBUG_CMD_CLOSE */
/* ulDebugType:XCAP_DBG_PRINT,XCAP_DBG_INFO,XCAP_DBG_ERROR,
    XCAP_DBG_LOG, XCAP_DBG_API_FUNC_TRACE,XCAP_DBG_FUNC_TRACE */
XCAPPUBFUN XCAP_VOID XcapApiDebugControl(IN XCAP_UINT32 ulCommand, IN XCAP_UINT32 ulDebugType);

XCAPPUBFUN XCAP_VOID XcapApiTimeOut(IN XCAP_ULONG ulPara);

XCAP_BOOL XcapBaseIpAddrCmp(IN const XcapIpAddr *pstAddr1, IN const XcapIpAddr *pstAddr2);

/* Define tracing directions. */
typedef enum _EN_XCAP_TRACE_DIR {
    XCAP_TRACE_DIR_INCOMING = 0,
    XCAP_TRACE_DIR_OUTGOING
} EN_XCAP_TRACE_DIR;

/* Description of the traced message. */
typedef struct _XcapTraceDesc {
    XcapIpAddr stLocAddr;
    XcapIpAddr stRemAddr;
    EN_XCAP_TRACE_DIR enTraceDir;
} XcapTraceDesc;

struct _XcapAppInfo;
typedef struct _XcapAppInfo XcapAppInfo;

typedef XCAP_HANDLE (*PTXcapBaseGetAppId)(IN XcapAppInfo *pstAppInfo);

typedef XCAP_VOID (*PTXcapSysTraceSend)(IN XCAP_ULONG ulAppId, IN const XCAP_UINT8 *pucMessage,
    IN XCAP_UINT32 ulMsgLength, IN const XcapTraceDesc *pstTraceDesc);

XCAPPUBFUN XCAP_ERR_CODE XcapApiRegTraceFunc(IN const PTXcapSysTraceSend pfTraceFunc);

XCAPPUBFUN XCAP_ERR_CODE XcapApiSetTraceStatus(IN XCAP_BOOL bEnable);

XCAPPUBFUN XCAP_ERR_CODE XcapApiSetMergedHttpMsgSend(IN XCAP_BOOL bEnable);

/* [VPP-22529][Del-Inst-Api-Invk-RunTime-Safe-Support][Start] */
XCAPPUBFUN XCAP_ERR_CODE XcapApiInstInit(IN XCAP_UINT32 ulMaxInstNum, OUT XCAP_HANDLE *phXcapInitHandle,
    IN const XCAP_VOID *pvAuxData); /* For future expandibility */
XCAPPUBFUN XCAP_ERR_CODE XcapApiInstDeInit(INOUT XCAP_HANDLE *phXcapInitHandle,
    INOUT XCAP_UINT32 *pulStackHandleListSize, INOUT XCAP_HANDLE *phStackHandleList, OUT XCAP_UINT32 *pulActiveInstNum,
    OUT XCAP_BOOL *pbStackDeInited, IN const XCAP_VOID *pvAuxData);   /* For future expandibility */
XCAPPUBFUN XCAP_VOID XcapApiTimeOutEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle, IN XCAP_ULONG ulPara);

/* [VPP-22529][Del-Inst-Api-Invk-RunTime-Safe-Support][End] */
typedef enum _EN_XCAP_INIT_PARA {
    /* [XCAP-23|DTS-ID:DTS2012101506347][EPoll] */
    XCAP_INIT_PARA_EPOLL_SUPPORT = 0, /* To enable the Epoll functionality support */
    /* [VPP-22529][Del-Inst-Api-Invk-RunTime-Safe-Support] */
    /* Configure the maximum instance number which can be created and running at
    a time. Currently, this configuration is effective only when XcapApiInit API
    is invoked.  If maximum instances are configured, but XcapApiInit not
    invoked, then stack can support more instance than configured. Number of
    Locks created depends on this configuration and in direct proportion. */
    XCAP_INIT_PARA_MAX_INSTANCE_NUMBER = 1,

    XCAP_INIT_PARA_BUTT
} EN_XCAP_INIT_PARA;

/* [DTS2013050204972][SOAP-65][n00742771][Start] */
typedef enum _EN_XCAP_CONFIG_THREAD_DELAY {
    /* This is to configure the delay time used by XCAP listener thread in
    sleep callback. The unit of this delay is millisecond. Default value of
    this delay is 10ms. For more details refer user guide. */
    XCAP_LISTENER_THREAD_SLEEP_FUNC_DELAY = 0,

    /* This is to configure the delay time used by XCAP worker thread in sleep
    callback. The unit of this delay is millisecond. Default value of this
    delay is 10ms. For more details refer user guide. */
    XCAP_WORKER_THREAD_SLEEP_FUNC_DELAY = 1,

    /* This is to configure the delay time used by WSP worker thread or
    tick task API in select callback. The unit of this delay is microsecond.
    Default value of this delay is 10000 microseconds. For more details refer
    user guide. */
    XCAP_WORKER_AND_TICKTASK_THREAD_SELECT_FUNC_DELAY = 2,

    XCAP_CONFIG_THREAD_DELAY_BUTT
} EN_XCAP_CONFIG_THREAD_DELAY;

/* [DTS2013050204972][SOAP-65][n00742771][End] */
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* _XCAP_BASE_H_ */
