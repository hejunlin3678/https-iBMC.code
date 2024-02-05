/* *****************************************************************************

    Copyright (c) Huawei Technologies Co., Ltd. 2008-2019. All rights reserved.
    Description:   XCAP头文件入口，接口定义
    Function List: NA
    Author:  L36241,W57322
    Create:  2008-4-14

***************************************************************************** */

#ifndef XCAPAPI_H
#define XCAPAPI_H

#include "xcaptypes.h"
#include "xcapbase.h"
#include "xcapssp.h"
#include "xcaperror.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ================================================
             XCAP 接口定义
================================================= */
/* URI */
typedef struct _XcapUri {
    XCAP_CHAR *pcRoot;
    XCAP_CHAR *pcAuid;
    XCAP_CHAR *pcContext; /* 只能为"users"或"global" */
    XCAP_CHAR *pcXui;
    XCAP_CHAR *pcDoc;
    XCAP_CHAR *pcNodeSelector;
} XcapUri;

/* DNS 查询回调 API */
typedef XCAP_ERR_CODE (*PTXcapDnsQuery)(IN XCAP_HANDLE hQueryCb, /* DNS查询句柄 */
    IN XCAP_BOOL blType,                                         /* XCAP_TRUE--同步,XCAP_FALSE--异步 */
    IN const XCAP_UINT8 *pucDomainName,                          /* 输入的域名 */
    OUT XcapIpAddr *pstAddr,                                     /* 同步查询得到的[VPP-25322] */
    OUT XCAP_ULONG *pulQueryId);                                 /* 异步方式下需要，返回查询ID */

typedef XCAP_VOID (*PTXcapOnRequest)(IN XCAP_ULONG ulAppId, /* 上层用户Id */
    IN XCAP_HANDLE hReqHandle,                              /* 协议栈请求标识 */
    IN XCAP_HANDLE hRequestMsg,                             /* 请求消息句柄 */
    IN const XcapUri *pstXcapUri,                           /* URI结构 */
    OUT XCAP_ULONG *pulRequsetId);                          /* 用户标识的请求Id */

typedef XCAP_VOID (*PTXcapOnResponse)(IN XCAP_ULONG ulAppId, /* 上层用户Id */
    IN XCAP_ULONG ulRequestId,                               /* 请求Id */
    IN XCAP_HANDLE hResponseMsg);                            /* 响应消息句柄 */

/* Type of XCAP Request Error */
typedef enum _EN_XCAP_REQ_ERROR_TYPE {
    XCAP_REQ_ERROR_CONNECTION_CLOSE = 0,       /* 该请求的连接被关闭，导致请求失败 */
    XCAP_REQ_ERROR_WAIT_APP_RESPONSE_TIME_OUT, /* 请求等待上层响应超时 */
    XCAP_REQ_ERROR_ENCODE_RESP_FAILURE,        /* 发送响应消息的过程中编码失败 */
    XCAP_REQ_ERROR_ENCODE_REQ_FAILURE,         /* 在发送请求的过程中编码失败(客户端) */
    XCAP_REQ_ERROR_CREATE_CONNECTION_FAILURE,  /* 发送请求时[VPP-25322]链路不成功(客户端) */
    XCAP_REQ_ERROR_SEND_REQ_FAILURE,           /* 发送请求消息失败 */
    XCAP_REQ_ERROR_DECODE_RESP_FAILURE, /* 收到响应消息，但解码失败，导致请求失败(客户端) */
    XCAP_REQ_ERROR_REQ_TIMEOUT,         /* 发送请求后等响应超时(客户端) */
    XCAP_REQ_ERROR_PIPELINE_USER_CLOSE, /* 长连接中，用户主动删除了其中一个事务。该事务之后的所有事务均按此原因上报 */
    XCAP_REQ_ERROR_TLS_UPGRADE_FAILURE,     /* 该请求TLS升级失败 */
    XCAP_REQ_ERROR_REPORT_RESPONSE_FAILURE, /* 协议栈上报响应失败， 连接上的后续事务也按此上报 */
    XCAP_REQ_ERROR_END
} EN_XCAP_REQ_ERROR_TYPE;

/* [VPP-8980][WSP-15][Start] */
/* XCAP instance specific configuration enum */
typedef enum _EN_XCAP_CONFIG_PARA {
    /* For client: To enable receiving response message, when Content-Length
    header is not set and message has body. End of the message will be
    determined by connection closure by server. For successful processing of
    HTTPs/1.1 response message, Connection header must be present in the
    response message. */
    XCAP_CLIENT_RECV_RESP_WITHOUT_CONTENTLENGTH_HEADER = 0,

    /* [VPP-12363][NE40-Perf-Req][Start] */
    /* For client and server both: When this configuration is enabled, XCAP
    server and client threads use event instead of sleep. Thread will wait for
    event XCAP_EVENT_RESUME_PROCESSING. If this event occured, means there is
    something to process. So thread will do necesery procesing and again wait
    for this event. This will help in improving the performance, by reducing
    sleep time of threads.
    */
    XCAP_CLIENT_SERVER_USE_EVENT_FOR_SLEEP = 1,

    /* For client and server both: When this configuration is enabled, XCAP
    server and client do not use socket select function to look read/write/
    err etc events on socket. APP has to give these event to xcap stack from
    SOCK task. PDT should use this feature only on 32 bit systems. This feature
    is mainly for the PDT using VRP sockets, which has async SOCK task to
    look the socket events. */
    XCAP_CLIENT_SERVER_USE_APP_SOCK_TASK_FOR_SELECT = 2,

    /* [vpp-9874][n00742771][start] */
    /* To enable server to create and use socket in same thread.
    This configuration will be valid only when:
    Worker thread configuration is 0 while
     in XcapApiCreateApp  ulWorkThreadNum of pstInitAppParam
    and listen mode is single thread XCAP_LISTEN_THREAD_MODE_SINGLE while
    in XcapApiCreateApp usListenThreadMode of  pstInitAppParam.
    This configuration is not supported for client or client-server instance.
    */
    XCAP_SERVER_CREATE_USE_SOCKET_IN_SAME_THREAD = 3,

    /* [VPP-19223][thr-auto-exit][start] */
    /* For client and server both: When this configuration is enabled,
    xcap stack will wait in XcapApiDeleteApp API untill xcap threads free
    resource and exit by itself. So APP no need to delete thread in
    pfBlockWaitThreadExit callback.
    When used DOPRA and this feature, it is suggested to use XcapApiKickOff API
    from other than VOS root task and after pid init is success. This is to
    avoid hang in XcapApiKickOff API when internal failure happens. When
    internal failure happens in XcapApiKickOff, it will try to delete the
    threads (if created before). So it will wait untill threads exit by itself.
    But it will keep on waiting, since threads are not spawned by DOPRA, untill
    VOS root task is suspended. Since threads are not spawned, threads will
    never auto exit. So XcapApiKickOff API will keep on waiting and hang. */
    XCAP_CLIENT_SERVER_DEL_APP_API_WAIT_TILL_TH_AUTO_EXIT = 4,

    /* [DTS2016060704445][max-alloc-limit][k71093][Start] */
    /* This limit is for max value in Content-Length header and also
    max value for combining chunks size. Default is 100 MB. */
    XCAP_CLIENT_SERVER_MAX_MSG_BODY_SIZE = 5,

    /* This limit is for max header number in the hypertext transfer protocol
    msg. Default max header number is 1000. */
    XCAP_CLIENT_SERVER_MAX_MSG_HDR_NUM = 6,

    XCAP_CONFIG_PARA_END
} EN_XCAP_CONFIG_PARA;

/* [VPP-4472][SOAP-68][d00902689][Start] */
/* XCAP Connect Event Info Enum */
typedef enum _EN_XCAP_CONNECTION_EVENT_TYPE {
    /* connection closed  (for xcap server and client).  Possible reasons can be
      TLS_UPGRADE_FAILED, EPOLL_OP_FAILED,
      SOCKET_SELECT_EXCEPTION, RECV_DATA_FAILED,
      PEER_CLOSE_CONNECTION, CONNECTION_CLOSE_HEADER_IN_MSG,
      IDLE_CONNECTION_TIMEOUT, TRANSACTION_TIMEOUT,
      INTERNAL_ERROR */
    XCAP_CONNECTION_EVENT_CLOSE = 0,

    /* connection made success (only for server and xcap client) */
    XCAP_CONNECTION_EVENT_CREATE_SUCCESS,

    /* connection create failed (only for xcap client). Possible reasons can be
    BIND_ERROR, CONNECT_ERROR, SETSOCKOPT_ERROR, SSL_ERROR,
    SOCKET_SELECT_EXCEPTION, EPOLL_OP_FAILED */
    XCAP_CONNECTION_EVENT_CREATE_FAIL,

    XCAP_CONNECTION_EVENT_END
} EN_XCAP_CONNECTION_EVENT_TYPE;

/* Callback function for reporting the connection event. */
typedef XCAP_VOID (*PTXcapConnectEvent)(IN XCAP_HANDLE hStackHandle, /* XCAP instance handle */
    IN XCAP_HANDLE ulAppId,                                          /* Handle of upper layer user */
    IN XCAP_UINT32 ulEvent,                                          /* EN_XCAP_CONNECTION_EVENT_TYPE */
    IN XCAP_UINT32 ulEventReason,                                    /* Currently unused */
    IN const XcapConnInfoS *pstConnInfo,                             /* XCAP connection information */
    IN XCAP_VOID *pvAuxData);                                        /* For Future use */

/* [VPP-12363][NE40-Perf-Req][Start] */
/* Get tick task info callback. Returns XCAP_SUCCESS on success else any other
value on failure */
typedef XCAP_UINT32 (*PTXcapGetTickTaskInfo)(IN XCAP_ULONG ulAppId, /* Application Id */
    OUT XCAP_ULONG *pulThreadId,                                    /* Thread Id */
    OUT XCAP_UINT32 *pulDelay);                                     /* Thread delay */

/* ****************************************************************************
 Callback Name: PTXcapTaskRunFreeCpu

 Description  : If there is possibility of task overrun, then this callback
                will be invoked. APP can decide whether to free cpu or not.
                APP can use one of the below functions in this callback:
                1. ULONG task_run_free_cpu( VOS_UINT32 ulRunMillSecs,
                VOS_UINT32 ulDelayMillSecs)
                This is mostly available on Vxworks platform. It checks if
                task has run for ulRunMillSecs or more time period, then sleep
                for ulDelayMillSecs time period.
                2. Any sleep function.
 Input        : ulAppId: Application Id
                ulTaskType - Specifies the task name which is invoking this
                callback, XCAP_LISTENER_THREAD, XCAP_WORKER_THREAD,
                XCAP_TICK_TASK_THREAD
                ulServiceMode: Service mode EN_XCAP_SERVICE_MODE
 Output       : None
 Return Value : When successful, returns XCAP_SUCCESS
                Any other value indicates failure
**************************************************************************** */
typedef XCAP_ERR_CODE (*PTXcapTaskRunFreeCpu)(IN XCAP_ULONG ulAppId, /* Application Id */
    IN XCAP_UINT32 ulServiceMode,                                    /* EN_XCAP_SERVICE_MODE */
    IN XCAP_UINT32 ulTaskType);                                      /* Task type */

/* [VPP-25237][Set-Listen-Sock-Opt][Start] */
/* Callback function for setting the listen socket options. */
typedef XCAP_ERR_CODE (*PTXcapSetListenSockOpt)(IN XCAP_HANDLE hStackHandle, /* XCAP instance handle */
    IN XCAP_ULONG ulAppId,                                                   /* Handle of upper layer user */
    IN XCAP_VOID *pvAppData,                                                 /* APP data */
    IN XCAP_UINT32 ulListenSockFd,                                           /* Listener socket id */
    IN const XcapIpAddr *pstListenIpAddr, /* Server's listener address and port information */
    IN XCAP_VOID *pvAuxData);             /* For Future use, may be TLS flag,  [VPP-25237][Set-Listen-Sock-Opt][End] */

typedef XCAP_VOID (*PTXcapOnErrorReport)(IN XCAP_ULONG ulAppId, /* 上层用户Id */
    IN XCAP_ULONG ulRequestId,                                  /* 请求Id */
    IN XCAP_UINT32 ulErrType);                                  /* EN_XCAP_REQ_ERROR_TYPE */

/* IP过滤器。返回XCAP_TRUE，则该IP是合法的；返回XCAP_FALSE，则该IP是非法的 */
typedef XCAP_BOOL (*PTXcapIpFilter)(IN XCAP_ULONG ulAppId, /* 上层用户Id */
    IN const XcapIpAddr *pstIpAddr);                       /* [VPP-25322] */

/* 启动定时器 */
typedef XCAP_ERR_CODE (*PTXcapStartRelTimer)(IN XCAP_UINT32 ulLength, IN XCAP_ULONG ulPara,
    OUT XCAP_HANDLE *phTimerHandle);

/* 停止定时器 */
typedef XCAP_VOID (*PTXcapStopRelTimer)(IN const XCAP_HANDLE *phTimerHandle);

/* [VPP-22529][Del-Inst-Api-Invk-RunTime-Safe-Support][Start] */
typedef XCAP_ERR_CODE (*PTXcapStartRelTimerEx)(IN XCAP_HANDLE hStackHandle, IN XCAP_UINT32 ulLength,
    IN XCAP_ULONG ulPara, OUT XCAP_HANDLE *phTimerHandle);

/* pulQueryId: 异步方式下需要，返回查询ID */
typedef XCAP_ERR_CODE (*PTXcapDnsQueryEx)(IN XCAP_HANDLE hStackHandle, IN XCAP_HANDLE hQueryCb, /* DNS查询句柄 */
    IN XCAP_BOOL blType,                /* XCAP_TRUE--同步,XCAP_FALSE--异步 */
    IN const XCAP_UINT8 *pucDomainName, /* 输入的域名 */
    OUT XcapIpAddr *pstAddr,            /* 同步查询得到的[VPP-25322] */
    OUT XCAP_ULONG *pulQueryId);        /* [VPP-22529] [Del-Inst-Api-Invk-RunTime-Safe-Support][End]   */

/* Callback function for differentiating multiple users, including message
    reporting and timer. It is for XcapApiCreateApp. */
typedef struct _XcapUserCallBackFunc {
    PTXcapOnRequest pfOnRequest;         /* 收到请求的回调处理函数 */
    PTXcapOnResponse pfOnResponse;       /* 收到响应的回调处理函数 */
    PTXcapOnErrorReport pfOnErrorReport; /* 通知上层错误的回调处理函数 */
    PTXcapDnsQuery pfDnsQuery;           /* DNS查询接口，可为空 */
    PTXcapIpFilter pfIpFilter;           /* IP过滤接口，可为空 */
    PTXcapStartRelTimer pfStartRelTimer; /* 启动定时器接口，不能为空 */
    PTXcapStopRelTimer pfStopRelTimer;   /* 停止定时器接口，不能为空 */
} XcapUserCallBackFunc;

/* [VPP-22529][Del-Inst-Api-Invk-RunTime-Safe-Support][Start] */
/* Callback function for differentiating multiple users, including message
    reporting and timer. It is for XcapApiCreateAppEx. */
typedef struct _XcapUserCallBackFuncEx {
    PTXcapOnRequest pfOnRequest;             /* 收到请求的回调处理函数 */
    PTXcapOnResponse pfOnResponse;           /* 收到响应的回调处理函数 */
    PTXcapOnErrorReport pfOnErrorReport;     /* 通知上层错误的回调处理函数 */
    PTXcapDnsQueryEx pfDnsQueryEx;           /* DNS查询接口，可为空 */
    PTXcapIpFilter pfIpFilter;               /* IP过滤接口，可为空 */
    PTXcapStartRelTimerEx pfStartRelTimerEx; /* 启动定时器接口，不能为空 */
    PTXcapStopRelTimer pfStopRelTimer;       /* 停止定时器接口，不能为空 */
} XcapUserCallBackFuncEx;                    /* [VPP-22529][Del-Inst-Api-Invk-RunTime-Safe-Support][End] */

/* DNS同步/异步查询 */
typedef enum _EN_XCAP_DNS_QUERY_TYPE {
    XCAP_DNS_QUERY_TYPE_SYN = 0, /* DNS同步查询 */
    XCAP_DNS_QUERY_TYPE_ASY      /* DNS异步查询 */
} EN_XCAP_DNS_QUERY_TYPE;

/* XCAP TLS模式 */
typedef enum _EN_XCAP_TLS_MODE {
    XCAP_TLS_MODE_NONE = 0,         /* 普通TCP模式，不支持TLS升级        */
    XCAP_TLS_MODE_UPGRADE_OPTION,   /* 可选支持TLS升级模式               */
    XCAP_TLS_MODE_UPGRADE_REQUIRED, /* 强制TLS升级模式                   */
    XCAP_TLS_MODE_SERVER_HTTPS,     /* HTTPS模式。服务端使用。
                               客户端需在消息URI中携带"https://" */
    XCAP_TLS_MODE_END
} EN_XCAP_TLS_MODE;

/* Used to store parameters related to Listen Address */
typedef struct _XcapListenIpAddr {
    XcapIpAddr stIpAddr;
    XCAP_UINT32 ulTlsMode; /* TLS模式 EN_XCAP_TLS_MODE */
#ifdef XCAP_64BIT_ALIGNED
    XCAP_UINT32 ulSpare; /* For 64 bit alignment */
#endif
} XcapListenIpAddr;

#define XCAP_MAX_LISTEN_ADDR_NUM XCAP_BASE_MAX_LISTEN_ADDR_NUM

/* Used to set the user parameters, including maximum number of connections
   and sending/receiving buffer. */
typedef struct _XcapInitAppParam {
    XCAP_UINT16 usServiceMode;      /* 包括客户端、服务端、Both: EN_XCAP_SERVICE_MODE */
    XCAP_UINT16 usListenThreadMode; /* 单线程、多线程模式: EN_XCAP_LISTEN_THREAD_MODE */
    XCAP_UINT32 ulWorkThreadNum;    /* 配置工作线程数 */
    XCAP_UINT32 ulMaxLinkPerThread; /* 每个线程支持的最大连接数 */
    XCAP_UINT32 ulLinkSendBuff;     /* 配置用户下所有连接的发送缓存，单位：字节(bytes) */
    XCAP_UINT32 ulLinkRecvBuff;     /* 配置用户下所有连接的接收缓存，单位：字节(bytes) */
    XCAP_UINT32 ulListenNum;        /* 侦听个数,最多可以有4个侦听 */
    XcapListenIpAddr stListenIpAddr[XCAP_MAX_LISTEN_ADDR_NUM];

    XCAP_UINT32 ulDNSQueryType;       /* EN_XCAP_DNS_QUERY_TYPE */
    XCAP_UINT32 ulTransactionTimeOut; /* 事务超时时长，单位:毫秒(ms) */

    /* 新连接如果在这段时间内没有收到任何消息连接就会超时关闭，单位:毫秒(ms) */
    XCAP_UINT32 ulConnWaitTime;
#ifdef XCAP_64BIT_ALIGNED
    XCAP_UINT32 ulSpare; /* For 64 bit alignment */
#endif
} XcapInitAppParam;

/* [XCAP_CODEC_SUPPORT][VPP-26285][Start] */
/* type of hypertext transfer protocol message body */
typedef enum _EN_XCAP_CODEC_HTTP_BODY_TYPE {
    XCAP_CODEC_HTTP_BODY_TYPE_NONE = 0,       /* No body */
    XCAP_CODEC_HTTP_BODY_TYPE_NORMAL,         /* Normal */
    XCAP_CODEC_HTTP_BODY_TYPE_CHUNK,          /* Chunk, not supported  */
    XCAP_CODEC_HTTP_BODY_TYPE_CHUNK_COMBINED, /* Chunk-Combined */
} EN_XCAP_CODEC_HTTP_BODY_TYPE;               /* [XCAP_CODEC_SUPPORT][VPP-26285][End] */

XCAPPUBFUN XCAP_ERR_CODE XcapApiSetSslConnectTimerValue(IN XCAP_HANDLE hStackHandle, IN XCAP_UINT32 ulMaxSslConnectTime,
    IN const XCAP_VOID *pvAuxData);

/* [VPP-25322]用户，并设置与用户相关的参数 */
XCAPPUBFUN XCAP_ERR_CODE XcapApiCreateApp(IN XCAP_ULONG ulAppId, /* 上层用户所标识的句柄 */
    IN const XcapUserCallBackFunc *pstUserCallBackFunc,          /* 区分多个用户的回调函数 */
    IN const XcapInitAppParam *pstInitAppParam, /* 设置该用户参数，包括最大连接数，发送、接收缓存 */
    OUT XCAP_HANDLE *phStackHandle);            /* 协议栈用户句柄 */

/* XCAP 用户开工 */
XCAPPUBFUN XCAP_ERR_CODE XcapApiKickOff(IN XCAP_HANDLE hStackHandle);

/* 删除用户 */
XCAPPUBFUN XCAP_ERR_CODE XcapApiDeleteApp(INOUT XCAP_HANDLE *phStackHandle); /* 协议栈用户句柄 */

/* ******************************************************************************
 Function   : XcapApiDisableUriValidation

 Description: This API allows the user to "Configure URI Validation".

 Input(s)   : hStackHandle            :   XCAP stack handle
    bDisableUriValidation   :   Set : XCAP_TRUE to disable URI validation.
                                By default URI validation will happen
    pvAuxData               :   This is for extensibility purpose.
                                As of now: XCAP_NULL_PTR has to be passed

 Output     :
    pvAuxData   :   This is for extensibility purpose.
                    As of now: XCAP_NULL_PTR has to be passed

 Return     : XCAP_SUCCESS / Other Error code in case of failure
****************************************************************************** */
XCAPPUBFUN XCAP_ERR_CODE XcapApiDisableUriValidation(IN const XCAP_HANDLE hStackHandle,
    IN XCAP_BOOL bDisableUriValidation, IN const XCAP_VOID *pvAuxData);

/* [VPP-25322]根据指定[VPP-25322]添加一个侦听 */
XCAPPUBFUN XCAP_ERR_CODE XcapApiCreateListen(IN XCAP_HANDLE hStackHandle, /* 协议栈用户句柄 */
    IN XCAP_UINT32 ulTlsMode,                                             /* EN_XCAP_TLS_MODE */
    IN const XcapIpAddr *pstLocalAddr);                                   /* 本端侦听地址 */

/* [VPP-25322]根据指定删除一个侦听 */
XCAPPUBFUN XCAP_ERR_CODE XcapApiDeleteListen(IN XCAP_HANDLE hStackHandle, /* 协议栈用户句柄 */
    IN const XcapIpAddr *pstLocalAddr);                                   /* 本端侦听地址 */

/* 字符串转换成结构 [VPP-25322][SensitiveWord] */
XCAPPUBFUN XCAP_ERR_CODE
XcapApiIpStringToAddress(IN XCAP_UINT16 usIpType, /* XCAP_IP_ADDR_TYPE_IPV4 or XCAP_IP_ADDR_TYPE_IPV6 */
    IN const XCAP_CHAR *pcStr,                    /* IP地址字符串 */
    IN XCAP_UINT16 usPort,                        /* 端口号 */
    OUT XcapIpAddr *pstIpAddr);                   /* 传出转化后的结构 [VPP-25322] */

XCAPPUBFUN XCAP_ERR_CODE XcapApiDoRequest(IN XCAP_HANDLE hStackHandle, /* 协议栈句柄 */
    IN XCAP_ULONG ulRequestId,                                         /* 用户的请求标识 */
    IN XCAP_HANDLE hRequestMsg,                                        /* 请求消息句柄 */
    OUT XCAP_HANDLE *phReqHandle);                                     /* 协议栈的请求标识 */

XCAPPUBFUN XCAP_ERR_CODE XcapApiDoResponse(IN XCAP_HANDLE hStackHandle, /* 协议栈句柄 */
    IN XCAP_HANDLE hReqHandle,                                          /* 协议栈的请求标识 */
    IN XCAP_HANDLE hResponseMsg);                                       /* 响应消息句柄 */

XCAPPUBFUN XCAP_ERR_CODE XcapApiCloseTransaction(IN XCAP_HANDLE hStackHandle, /* 协议栈句柄 */
    IN XCAP_HANDLE hReqHandle);                                               /* 事务句柄 */

XCAPPUBFUN XCAP_VOID XcapApiDnsAsyRsp(IN XCAP_HANDLE hDnsHandle, /* DNS查询句柄 */
    IN XCAP_ULONG ulQueryId,                                     /* 查询ID */
    IN XCAP_UINT32 ulQueryResult,                                /* 查询结果 */
    IN const XcapIpAddr *pstAddr);                               /* 异步查询得到的[VPP-25322] */

/* ***************************
  XCAP消息操作函数
*************************** */
XCAPPUBFUN XCAP_ERR_CODE XcapApiCreateMsg(IN XCAP_BOOL bIsRequest, OUT XCAP_HANDLE *phMsg);

XCAPPUBFUN XCAP_VOID XcapApiDestroyMsg(INOUT XCAP_HANDLE *phMsg);

XCAPPUBFUN XCAP_ERR_CODE XcapApiCloneMsg(IN XCAP_HANDLE hMsg, OUT XCAP_HANDLE *phClonedMsg);

XCAPPUBFUN XCAP_ERR_CODE XcapApiEncodeMsg(IN XCAP_HANDLE hMsg, OUT XCAP_CHAR **ppMsg);

XCAPPUBFUN XCAP_VOID XcapApiUnEscapeString(INOUT XCAP_CHAR *pStr);

XCAPPUBFUN XCAP_ERR_CODE XcapApiGetMsgType(IN const XCAP_HANDLE hMsg, OUT XCAP_BOOL *pbIsRequest);

/* XCAP请求响应消息的操作接口 */
XCAPPUBFUN XCAP_CHAR *XcapApiGetVersion(IN const XCAP_HANDLE hMsg);

XCAPPUBFUN XCAP_ERR_CODE XcapApiSetVersion(IN const XCAP_HANDLE hMsg, IN const XCAP_CHAR *pcVersion);

XCAPPUBFUN XCAP_CHAR *XcapApiGetMethod(IN const XCAP_HANDLE hMsg);

XCAPPUBFUN XCAP_ERR_CODE XcapApiSetMethod(IN const XCAP_HANDLE hMsg, IN const XCAP_CHAR *pcMethod);

XCAPPUBFUN XCAP_CHAR *XcapApiGetUri(IN const XCAP_HANDLE hMsg);

XCAPPUBFUN XCAP_ERR_CODE XcapApiSetUri(IN const XCAP_HANDLE hMsg, IN const XCAP_CHAR *pcUri);

XCAPPUBFUN XCAP_ERR_CODE XcapApiSetUriByStruct(IN const XCAP_HANDLE hMsg, IN const XcapUri *pstUri);

XCAPPUBFUN XCAP_ERR_CODE XcapApiGetUriStruct(IN const XCAP_HANDLE hMsg, INOUT XcapUri *pstUri);

/* [DTS2016060704445][max-alloc-limit][k71093][start] */
XCAPPUBFUN XCAP_ERR_CODE XcapApiSetMaxHeaderNum(IN const XCAP_HANDLE hMsg,
    IN XCAP_UINT32 ulMaxHdrNum); /* [DTS2016060704445][max-alloc-limit][k71093][end] */

XCAPPUBFUN XCAP_ERR_CODE XcapApiGetHeaderNum(IN const XCAP_HANDLE hMsg, OUT XCAP_UINT32 *pulHeaderNum);

XCAPPUBFUN XCAP_ERR_CODE XcapApiGetHeaderByIndex(IN const XCAP_HANDLE hMsg, IN XCAP_UINT32 ulIndex,
    OUT XCAP_CHAR **ppcName, OUT XCAP_CHAR **ppcValue);

XCAPPUBFUN XCAP_ERR_CODE XcapApiGetHeaderByName(IN const XCAP_HANDLE hMsg, IN const XCAP_CHAR *pcName,
    OUT XCAP_CHAR **ppcValue);

XCAPPUBFUN XCAP_ERR_CODE XcapApiDelHeaderByName(IN const XCAP_HANDLE hMsg, IN const XCAP_CHAR *pcName);

XCAPPUBFUN XCAP_ERR_CODE XcapApiAddHeader(IN const XCAP_HANDLE hMsg, IN const XCAP_CHAR *pcName,
    IN const XCAP_CHAR *pcValue);

XCAPPUBFUN XCAP_ERR_CODE XcapApiGetBody(IN const XCAP_HANDLE hMsg, OUT XCAP_UINT32 *pulBodyLen,
    OUT XCAP_UINT8 **ppucBody);

XCAPPUBFUN XCAP_ERR_CODE XcapApiSetBody(IN const XCAP_HANDLE hMsg, IN XCAP_UINT32 ulBodyLen,
    IN const XCAP_UINT8 *pucBody);

/* [DTS2016060704445][max-alloc-limit][k71093][start] */
XCAPPUBFUN XCAP_ERR_CODE XcapApiSetMaxBodySize(IN const XCAP_HANDLE hMsg, IN XCAP_UINT32 ulMaxBodySize);

/* [DTS2016060704445][max-alloc-limit][k71093][end] */
XCAPPUBFUN XCAP_ERR_CODE XcapApiGetRespCode(IN const XCAP_HANDLE hMsg, OUT XCAP_UINT32 *pulCode,
    OUT XCAP_CHAR **ppcReasonPhrase);

XCAPPUBFUN XCAP_ERR_CODE XcapApiSetRespCode(IN const XCAP_HANDLE hMsg, IN XCAP_UINT32 ulCode,
    IN const XCAP_CHAR *pcReasonPhrase);

XCAPPUBFUN XCAP_ERR_CODE XcapApiSetRequestTlsMode(IN XCAP_HANDLE hMsg,
    IN XCAP_UINT32 ulTlsMode); /* TLS升级模式。EN_XCAP_TLS_MODE */

XCAPPUBFUN XCAP_ERR_CODE XcapApiSetRequestDestAddr(IN XCAP_HANDLE hHttpMsg, IN const XCAP_CHAR *pcAddr,
    IN XCAP_BOOL bIsHttps);

/* [IR20191127007811][4-12-2019] - Start */
XCAPPUBFUN XCAP_ERR_CODE XcapApiSetMsgCleanse(IN const XCAP_HANDLE hHttpMsg, IN XCAP_BOOL bCleanseFlag);
XCAPPUBFUN XCAP_ERR_CODE XcapApiGetMsgCleanse(IN const XCAP_HANDLE hHttpMsg, IN XCAP_BOOL *pbCleanseFlag);

/* [IR20191127007811][4-12-2019] - End */
/* Used to get the User Static Information. */
typedef struct _XcapUserStatisticInfo {
    XCAP_UINT32 ulOnlineLink;  /* 统计--此线程在线的连接数, 和COM模块保持一致 */
    XCAP_UINT32 ulOnlineTrans; /* 统计--在线使用的事务节点 */

    XCAP_UINT32 ulSendReq;  /* 统计--客户端:从上层传入的请求个数  */
    XCAP_UINT32 ulRecvResp; /* 统计--客户端:上报给上层的响应个数  */

    XCAP_UINT32 ulRecvReq;  /* 统计--服务端:上报给上层的请求个数  */
    XCAP_UINT32 ulSendResp; /* 统计--服务端:从上层传入的响应个数  */

    XCAP_UINT32 ulDiscardRequest; /* 统计--服务端:IP过滤掉的请求个数    */
#ifdef XCAP_64BIT_ALIGNED
    XCAP_UINT32 ulSpare; /* For 64 bit alignment */
#endif
} XcapUserStatisticInfo;

/* 获取统计信息 */
XCAPPUBFUN XCAP_VOID XcapApiGetStatistic(IN XCAP_HANDLE hStackHandle, /* 协议栈用户句柄     */
    INOUT XcapUserStatisticInfo *pstStatistic);

/* 统计信息显示，包含内部的统计信息，使用注册的XcapSysDebugSend打印 */
XCAPPUBFUN XCAP_VOID XcapApiDisplayStat(IN XCAP_HANDLE hStackHandle); /* 协议栈用户句柄     */

XCAPPUBFUN XCAP_VOID XcapApiClearStat(IN XCAP_HANDLE hStackHandle); /* 协议栈用户句柄     */

/* ****************************************************************************
  Function name         : XcapApiSetSockOptPerRequest

  Function description  : This XCAP API has to be invoked on Client side for
                          explicitly configuring DSCP / other socket options.

                          User may invoke this API multiple times for
                          configuring several options.

                          This API should be invoked before sending
                          Request message.

  Input parameters      :
                            IN      hRequestMsg -- Request Message handle
                            IN     * pstSockData -- Socket option configuration
                            INOUT  * pvAuxData -- Used for extensibility purpose

  Return value          : XCAP_SUCCESS / Others on failure
**************************************************************************** */
XCAPPUBFUN XCAP_ERR_CODE XcapApiSetSockOptPerRequest(IN XCAP_HANDLE hRequestMsg, IN const XcapSockDataS *pstSockData,
    IN const XCAP_VOID *pvAuxData);

XCAPPUBFUN XCAP_ERR_CODE XcapApiGetRemAddrFrmReqHdl(IN XCAP_HANDLE hStackHandle, IN XCAP_HANDLE hReqHandle,
    OUT XcapIpAddr *pstRemoteAddr);

XCAPPUBFUN XCAP_ERR_CODE XcapApiSetRequestSrcAddr(IN XCAP_HANDLE hStackHandle, /* Application stack handle */
    IN const XcapIpAddr *pstLocalIpAddr,                                       /* Local Address */
    IN const XCAP_VOID *pvAuxData);                                            /* For extensibility */

XCAPPUBFUN XCAP_ERR_CODE XcapApiSetAcceptQueueSize(IN XCAP_HANDLE hStackHandle, /* Application stack handle */
    IN XCAP_UINT32 ulMaxConnInQueue,                                            /* Max outstanding connections in
                                                                                   server connection queue */
    IN const XCAP_VOID *pvAuxData);                                             /* For extensibility */

/* [VPP-3693][SOAP-67][n00742771][Start] */
/* [XCAP-23|DTS-ID:DTS2012101506347][Start][EPoll] */
XCAPPUBFUN XCAP_ERR_CODE XcapApiSetInitParameter(IN EN_XCAP_INIT_PARA enInitParam, /* Initialization parameter type */
    IN const XCAP_VOID *pvValue); /* Points to the value corresponding to EN_XCAP_INIT_PARA type. */

/* pvValue: Points to the value corresponding to EN_XCAP_INIT_PARA type. */
XCAPPUBFUN XCAP_ERR_CODE XcapApiGetInitParameter(IN EN_XCAP_INIT_PARA enInitParam, /* Initialization parameter type */
    OUT XCAP_VOID *pvValue); /* [XCAP-23|DTS-ID:DTS2012101506347][End][EPoll] */

/* [DTS2013050204972][SOAP-65][n00742771][Start] */
/* ulDelay: Points to the value corresponding to enConfigThDelayPara type. */
XCAPPUBFUN XCAP_ERR_CODE XcapApiSetThreadDelay(IN XCAP_HANDLE hStackHandle, /* XCAP instance handle */
    IN EN_XCAP_CONFIG_THREAD_DELAY enConfigThDelayParam,                    /* Thread delay
                                                                               Parameter type */
    IN XCAP_UINT32 ulDelay); /* [DTS2013050204972][SOAP-65][n00742771][End]
                              */
/* pstLocalAddr: Local Address of server on whichrequest message is received.
    Memory allocation for this has to be done by the caller of the API */
XCAPPUBFUN XCAP_ERR_CODE XcapApiGetLocalAddrFrmReqHdl(IN XCAP_HANDLE hStackHandle, /* XCAP instance handle */
    IN XCAP_HANDLE hReqHandle, OUT XCAP_UINT32 *pulServerSocketFd,                 /* Accepted socket descriptor
                                                                                      of server on which request
                                                                                      message is received */
    OUT XcapIpAddr *pstLocalAddr);

/* [VPP-4472][SOAP-68][d00902689][Start] */
XCAPPUBFUN XCAP_ERR_CODE XcapApiRegConnectEventFunc(IN XCAP_HANDLE hStackHandle,
    IN const PTXcapConnectEvent pfConnectEvent);

/* [VPP-8980][WSP-15][Start] */
/* pvValue: Points to the value corresponding to EN_XCAP_CONFIG_PARA type. */
XCAPPUBFUN XCAP_ERR_CODE XcapApiSetConfigParam(IN XCAP_HANDLE hStackHandle, /* XCAP instance handle */
    IN EN_XCAP_CONFIG_PARA enConfigParam,                                   /* Configuration parameter type */
    IN const XCAP_VOID *pvValue);                                           /* [VPP-8980][WSP-15][End] */

#ifndef XCAP_HERT_LIB_SIZE_REDUCE
/* [VPP-11827][d00902689][Start] */
XCAPPUBFUN XCAP_ERR_CODE XcapApiRegStatisticFunc(IN XCAP_HANDLE hStackHandle, /* XCAP instance handle */
    IN const PTXcapStat pfStat);
/* Statistics callback */ /* [VPP-11827][d00902689][End] */

#endif

/* [VPP-12363][NE40-Perf-Req][Start] */
XCAPPUBFUN XCAP_ERR_CODE XcapApiRegGetTickTaskInfoFunc(IN XCAP_HANDLE hStackHandle, /* XCAP instance handle */
    IN const PTXcapGetTickTaskInfo pfGetTickTaskInfo);                              /* Get tick task info callback */

XCAPPUBFUN XCAP_ERR_CODE XcapApiRegTaskRunFreeCpuFunc(IN XCAP_HANDLE hStackHandle, /* XCAP instance handle */
    IN const PTXcapTaskRunFreeCpu pfTaskRunFreeCpu);
/* Task free cpu callback */ /* [VPP-12363][NE40-Perf-Req][End] */

/* [VPP-22529][Del-Inst-Api-Invk-RunTime-Safe-Support][Start] */
XCAPPUBFUN XCAP_ERR_CODE XcapApiRegStartTimerExFunc(IN XCAP_HANDLE hStackHandle,
    IN const PTXcapStartRelTimerEx pfStartRelTimerEx);

XCAPPUBFUN XCAP_ERR_CODE XcapApiRegDnsQueryExFunc(IN XCAP_HANDLE hStackHandle,
    IN const PTXcapDnsQueryEx pfDnsQueryEx); /* [VPP-22529][Del-Inst-Api-Invk-RunTime-Safe-Support][End] */

/* [VPP-25237][Set-Listen-Sock-Opt][Start] */
XCAPPUBFUN XCAP_ERR_CODE XcapApiRegSetListenSockOptFunc(IN XCAP_HANDLE hStackHandle, IN const XCAP_VOID *pvAppData,
    IN const PTXcapSetListenSockOpt pfSetListenSockOpt);
/* [VPP-25237][Set-Listen-Sock-Opt][End]  */
/* [VPP-22529][Del-Inst-Api-Invk-RunTime-Safe-Support][Start] */
XCAPPUBFUN XCAP_ERR_CODE XcapApiCreateAppEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_ULONG ulAppId,                                                  /* 上层用户所标识的句柄 */
    IN const XcapUserCallBackFuncEx *pstUserCallBackFunc,                   /* 区分多个用户的回调函数 */
    IN const XcapInitAppParam *pstInitAppParam, /* 设置该用户参数，包括最大连接数，发送、接收缓存 */
    OUT XCAP_HANDLE *phStackHandle);            /* 协议栈用户句柄 */

XCAPPUBFUN XCAP_ERR_CODE XcapApiDeleteAppEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    INOUT XCAP_HANDLE *phStackHandle);                                      /* 协议栈用户句柄 */

XCAPPUBFUN XCAP_ERR_CODE XcapApiSetSslConnectTimerValueEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle, IN XCAP_UINT32 ulMaxSslConnectTime, IN const XCAP_VOID *pvAuxData);

XCAPPUBFUN XCAP_ERR_CODE XcapApiDisableUriValidationEx(IN XCAP_HANDLE hXcapInitHandle,
    IN const XCAP_HANDLE hStackHandle, IN XCAP_BOOL bDisableUriValidation, IN const XCAP_VOID *pvAuxData);

XCAPPUBFUN XCAP_ERR_CODE XcapApiKickOffEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle);

XCAPPUBFUN XCAP_ERR_CODE XcapApiDoRequestEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle, IN XCAP_ULONG ulRequestId,                 /* 用户的请求标识 */
    IN XCAP_HANDLE hRequestMsg, OUT XCAP_HANDLE *phReqHandle);              /* 协议栈的请求标识 */

XCAPPUBFUN XCAP_ERR_CODE XcapApiDoResponseEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle, IN XCAP_HANDLE hReqHandle,                  /* 协议栈的请求标识 */
    IN XCAP_HANDLE hResponseMsg);

XCAPPUBFUN XCAP_ERR_CODE XcapApiCloseTransactionEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle, IN XCAP_HANDLE hReqHandle);

XCAPPUBFUN XCAP_VOID XcapApiGetStatisticEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle,                                           /* 协议栈用户句柄     */
    INOUT XcapUserStatisticInfo *pstStatistic);

XCAPPUBFUN XCAP_VOID XcapApiDisplayStatEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle);                                         /* 协议栈用户句柄     */

XCAPPUBFUN XCAP_VOID XcapApiClearStatEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle);                                       /* 协议栈用户句柄     */

XCAPPUBFUN XCAP_ERR_CODE XcapApiCreateListenEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle,                                               /* 协议栈用户句柄 */
    IN XCAP_UINT32 ulTlsMode,                                                  /* EN_XCAP_HTTP_TLS_MODE */
    IN const XcapIpAddr *pstLocalAddr);                                        /* 本端侦听地址 */

XCAPPUBFUN XCAP_ERR_CODE XcapApiDeleteListenEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle,                                               /* 协议栈用户句柄 */
    IN const XcapIpAddr *pstLocalAddr);                                        /* 本端侦听地址 */

XCAPPUBFUN XCAP_ERR_CODE XcapApiGetRemAddrFrmReqHdlEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle, IN XCAP_HANDLE hReqHandle, OUT XcapIpAddr *pstRemoteAddr);

/* pvAuxData为用户自定义数据，它会被透传并在PTXcapSslCreatAndConfigClientCtx回调中作为入参输入 */
XCAPPUBFUN XCAP_ERR_CODE XcapApiSetRequestSrcAddrEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle,                                                    /* Application stack handle */
    IN const XcapIpAddr *pstLocalIpAddr,                                            /* Local Address */
    IN const XCAP_VOID *pvAuxData);                                                 /* For extensibility */

XCAPPUBFUN XCAP_ERR_CODE XcapApiSetAcceptQueueSizeEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle,                                                     /* Application stack handle */
    IN XCAP_UINT32 ulMaxConnInQueue,                                                 /* Max outstanding connections in
                                                                                        server connection queue */
    IN const XCAP_VOID *pvAuxData);                                                  /* For extensibility */

XCAPPUBFUN XCAP_ERR_CODE XcapApiSetThreadDelayEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle,                                                 /* XCAP instance gandle */
    IN EN_XCAP_CONFIG_THREAD_DELAY enConfigThDelayParam,                         /* Thread delay Parameter type */
    IN XCAP_UINT32 ulDelay); /* Points to the value corresponding to enConfigThDelayPara type. */

/* pstLocalAddr: Address of server on which request message is received. Memory allocation for this has to be
    done by the caller of the API */
XCAPPUBFUN XCAP_ERR_CODE XcapApiGetLocalAddrFrmReqHdlEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle,                                                        /* XCAP instance handle */
    IN XCAP_HANDLE hReqHandle, OUT XCAP_UINT32 *pulServerSocketFd,                      /* Accepted socket
                                                                                           descriptor of server on
                                                                                           which request message is
                                                                                           received */
    OUT XcapIpAddr *pstLocalAddr);

XCAPPUBFUN XCAP_ERR_CODE XcapApiRegConnectEventFuncEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle, IN const PTXcapConnectEvent pfConnectEvent);

#ifndef XCAP_HERT_LIB_SIZE_REDUCE
XCAPPUBFUN XCAP_ERR_CODE XcapApiRegStatisticFuncEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle, IN const PTXcapStat pfStat);
#endif

XCAPPUBFUN XCAP_ERR_CODE XcapApiRegGetTickTaskInfoFuncEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle, IN const PTXcapGetTickTaskInfo pfGetTickTaskInfo);

XCAPPUBFUN XCAP_ERR_CODE XcapApiRegTaskRunFreeCpuFuncEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle, IN const PTXcapTaskRunFreeCpu pfTaskRunFreeCpu);

XCAPPUBFUN XCAP_ERR_CODE XcapApiSetConfigParamEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle,                                                 /* XCAP instance handle */
    IN EN_XCAP_CONFIG_PARA enConfigParam,                                        /* Configuration parameter type */
    IN const XCAP_VOID *pvValue); /* Points to the value corresponding to EN_XCAP_CONFIG_PARA type. */

XCAPPUBFUN XCAP_VOID XcapApiDnsAsyRspEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle, IN XCAP_HANDLE hDnsHandle,             /* DNS查询句柄 */
    IN XCAP_ULONG ulQueryId,                                            /* 查询结果 */
    IN XCAP_UINT32 ulQueryResult,                                       /* 查询结果 */
    IN const XcapIpAddr *pstAddr);                                      /* 同步查询得到的[VPP-25322] */
/* [VPP-22529][Del-Inst-Api-Invk-RunTime-Safe-Support][End] */
/* [VPP-25237][Set-Listen-Sock-Opt][Start] */
XCAPPUBFUN XCAP_ERR_CODE XcapApiRegSetListenSockOptFuncEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle, IN const XCAP_VOID *pvAppData, IN const PTXcapSetListenSockOpt pfSetListenSockOpt);
/* [VPP-25237][Set-Listen-Sock-Opt][End] */
/* [XCAP_CODEC_SUPPORT][VPP-26285][Start] */
/* ****************************************************************************
 Function Name: XcapCodecApiResetStreamDelimiter
 Description  : API to reset stream delimiter.
                If XcapCodecApiStreamDelimit has been invoked and indicated that
                hypertext transfer protocol msg is not complete and need to
                invoke again. But due to some error, APP not able to get
                further stream.
                In this case, this XcapCodecApiResetSreamDelimiter api can
                be invoked to reset stream delimiter, so that fresh stream
                can be parsed from begining.
                It is suggested to invoke this api before invoking
                XcapCodecApiStreamDelemit api first time to parse stream.


 Input        : ppvCodecCtx - codec context.
 Output       : None

 Return Value :
               XCAP_SUCCESS:  On success
               XCAP_ERR_CODE
**************************************************************************** */
XCAPPUBFUN XCAP_ERR_CODE XcapCodecApiResetStreamDelimiter(IN XCAP_VOID *pvCodecCtx);

/* ****************************************************************************
 Function Name: XcapCodecApiStreamDelimit
 Description  : This API delimits and decodes hypertext transfer protocol
                message streams.
                This api need to be invoked multiple times until stack
                detects end of msg.
                This api will do on-the-go parsing and will not wait till entire
                hypertext transfer protocol msg is received.
                If input stream does not satisfy syntax/rules as defined by
                hypertext transfer protocol, this api returns error.

 Input        :
    pvCodecCtx - codec context (created by XcapCodecApiCreateCtx api)

    pucStreamData - pointer of stream data which need to be parsed.

    ulStreamLen - length of pucStreamData buffer

    bCombineChunks - to indicate whether chunk bodies should be combined
               together to form single body entity.
               XCAP_TRUE : combine the chunk bodies together and get
               body type as XCAP_CODEC_HTTP_BODY_TYPE_CHUNK_COMBINED in
               XcapApiGetBodyType api.

               XCAP_FALSE: do not combine the chunk bodies and get
               body type as XCAP_CODEC_HTTP_BODY_TYPE_CHUNK in
               XcapApiGetBodyType api.

               Note: currently XCAP_FALSE not supported. app need to pass
               bCombineChunks as XCAP_TRUE.

    hHttpMsg - hypertext transfer protocol msg handle. from this handle, stack
               can know whether input stream is of request msg or response
               message.

    pvAuxData - for future use

 Output       :
    hHttpMsg   - hypertext transfer protocol msg handle. it is valid ony when,
            output *pbMsgComplete is XCAP_TRUE.
            a. it should be created by APP using XcapApiCreateMsgCodecCtxt api,
            before invoking this api.
            b. method, uri, version, headers, status, body etc hypertext
            transfer protocol paras will be set to this hHttpMsg by stack, when
            decoding of the stream is success. these paras will be set as per
            msg type (request/response).
            c. these paras can be obtained by APP using GET apis like
            XcapApiGetMethod, XcapApiGetUri, XcapApiGetHeaderByName etc.
            d. how to obtain msg body?
            - first APP need to get body type using api XcapApiGetBodyType.
            - if body type is none (XCAP_CODEC_HTTP_BODY_TYPE_NONE), means
              there is no body in the hypertext transfer protocol msg.
            - if body type is normal (XCAP_CODEC_HTTP_BODY_TYPE_NORMAL) or
              chunk-combined (XCAP_CODEC_HTTP_BODY_TYPE_CHUNK_COMBINED),
              means there is one body entity in the hypertext transfer protocol
              msg handle. This body can be obtained using XcapApiGetBody api.
            - if body type is chunk (XCAP_CODEC_HTTP_BODY_TYPE_CHUNK), means
              msg has chunked bodies. there can be several chunks.
              these chunks can be obtained by making use of
              XcapApiGetBodyChunkNum and XcapApiGetBodyChunk apis.
            e. if app want to get trailer headers when body type is
            chunk or chunk-combined, then can be obtained by making use
            of XcapApiGetTrailHeaderNum and XcapApiGetTrailHeaderByIndex apis.

     pulCurParsedLen - the length of stream, which is parsed by stack
             successfully.

     pbMsgComplete -
             XCAP_TRUE: pucStreamData has complete hypertext transfer protocol
             msg. in this case, APP no need to invoke XcapCodecApiStreamDelemit
             api again. when pbMsgComplete is XCAP_TRUE, stack resets all its
             internal parsing state.

             XCAP_FALSE: pucStreamData does not have complete hypertext
               transfer protocol msg. in this case, APP need to receive further
               stream from network and add it to the unparsed stream and invoke
               XcapCodecApiStreamDelemit api again.

               unparsed stream start pointer
                          = pucStreamData + (*pulCurParsedLen)
               unparsed stream len
                          = ulStreamLen - (*pulCurParsedLen)

               New pucStreamData len
                          =  len of new stream from network
                                 + unparsed stream len
               New pucStreamData
                          = append (new stream from network and unparsed stream)

               APP need to invoke this api with input New pucStreamData until,
               pbMsgComplete beacomes XCAP_TRUE.

     pvAuxData - for future use


 Return Value :
            XCAP_SUCCESS:  On success
            XCAP_ERR_CODE

**************************************************************************** */
XCAPPUBFUN XCAP_ERR_CODE XcapCodecApiStreamDelimit(IN XCAP_VOID *pvCodecCtx, INOUT XCAP_UINT8 *pucStreamData,
    IN XCAP_UINT32 StreamLen, IN XCAP_BOOL bCombineChunks, INOUT XCAP_HANDLE hHttpMsg, OUT XCAP_UINT32 *pulCurParsedLen,
    OUT XCAP_BOOL *pbMsgComplete, IN const XCAP_VOID *pvAuxData);

/* ****************************************************************************
 Function Name: XcapCodecApiEncodeHttpMsg
 Description  : API to encode hypertext transfer protocol message in string
                format from input msg handle.
 Input        :
    pvCodecCtx - codec context (created by XcapCodecApiCreateCtx)

    hHttpMsg   - hypertext transfer protocol msg handle. it will be converted
            to string as output of this XcapCodecApiEncodeHttpMsg API.
            it should be created using XcapApiCreateMsgCodecCtxt api.
            method, uri, version, headers, status etc hypertext transfer
            protocol paras should be set to this hHttpMsg before invoking
            XcapCodecApiEncodeHttpMsg api. these paras should be set as per msg
            type (request/response). body should not be set to hHttpMsg handle.
            If APP set it, then also will be ignored by
            XcapCodecApiEncodeHttpMsg API.

    bIsChunkBody - flag to indicate whether msg-body should be encoded as normal
            body or chunked body.

            XCAP_TRUE: when set to this,
                a. "Transfer-Encoding: chunk" header will be added to encoded
                msg, if not already set by APP in hHttpMsg.
                b.
                when pucBody is valid pointer:
                  then it will be encoded in chunk format. Also APP need to
                  invoke XcapCodecApiEncodeChunkedBody api cyclically to encode
                  next chunks and last chunk.
                when pucBody is NULL:
                  then it is treated, as there is no body and hence last chunk
                  with size 0 will be encoded. also trailer header will be added
                  if hHttpMsg has it. trailer header can be set using
                  XcapApiAddBodyTrailHeader api before invoking this api.
                c. pcChunkExt if not NULL, then added to chunk.

            XCAP_FALSE: When set to this,
                when pucBody is valid pointer:
                  then it is added to output encoded msg. also stack will add
                  Content-Length: ulBodyLen header by itself.  If this header
                  is set by APP in input hHttpMsg, then it is ignored.
                when pucBody is NULL:
                  then encoded msg will not have body. also stack will add
                  Content-Length: ulBodyLen header by itself.  If this header
                  is set by APP in input hHttpMsg, then it is ignored.
                  - if APP want to send hypertext transfer protocol msg header
                  and body seperately in two Tcp send call, then can set
                  pucBody to NULL and valid non-zero ulBodyLen. Its APP's
                  responsibility to send hypertext transfer protocol msg body
                  to network seperately. It will give better perormance in case
                  of huge body, since avoided memorycpy to copy body in output
                  encoded buffer.
                  - If there is no body, APP can set ulBodyLen to zero. stack
                  will consider it as there is no body and add Content-Length
                  header with value 0 to encoded hypertext transfer protocol msg.

             Note: currently  setting ubIsChunkBody to XCAP_TRUE is not
                 supported. app need to set it to XCAP_FALSE always.

    pcChunkExt - this parameter is valid only when bIsChunkBody is XCAP_TRUE.
              if not NULL, chunk extension will be added to chunk.

              Note: currenly this parameter is not supported, ignored by stack.

    pucBody - body of the hypertext transfer protocol msg. will be used as
            described in bIsChunkBody para.

    ulBodyLen - length of pucBody (will be used as Content-Length header value,
               in case bIsChunkBody = XCAP_FALSE).

    ppucEncBuf - buffer to copy encoded msg.
             case 1 (*ppucEncBuf Not NULL):
             this means memory for output encoded msg is allocated by app.
             if this memory is not sufficient to accomodate
             the encoded msg, then stack will return XCAP_ERR_BUF_SIZE_TOO_SMALL
             error and give approximate required buf size in para pulMsgLen.
             APP need to allocate *pucEncBuf same as *pulMsgLen and need to
             invoke XcapCodecApiEncodeHttpMsg again.

             case 2 (*ppucEncBuf is NULL)::
             xcap will allocate memory from hHttpMsg handle. this memory will be
             freed, when msg handle is deleted.

     pulEncBufLen - length of *pucEncBuf buffer if not NULL. after successful
            encoding the msg, it gives actual msg length.

    pvAuxData - for future use

 Output       :
    ppucEncBuf - encoded hypertext transfer protocol msg.
             case 1: if input *ppucEncBuf Not NULL, then APP
             should free the memory using approriate free function

             case 2: if input  *ppucEncBuf is NULL, this means
             XCAP stack has allocated memory for this buffer from
             hHttpMsg handle. This memory will be freed when APP destoys the
             msg handle.

     pulEncBufLen - length of pucEncBuf buffer. after successful encoding the
            msg, it gives actual msg length.

     pulMsgLen - message length (when api returns XCAP_SUCCESS or
           XCAP_ERR_BUF_SIZE_TOO_SMALL error).  Also pls check the description
           of pucEncBuf parameter. When api returns XCAP_ERR_BUF_SIZE_TOO_SMALL,
           pulMsgLen indicate the approximate length which APP need to allocate.

     pvAuxData - for future use


 Return Value :
            XCAP_SUCCESS:  On success
            XCAP_ERR_BUF_SIZE_TOO_SMALL : as described in pucEncBuf para
            XCAP_ERR_CODE

**************************************************************************** */
XCAPPUBFUN XCAP_ERR_CODE XcapCodecApiEncodeHttpMsg(IN XCAP_VOID *pvCodecCtx, IN XCAP_HANDLE hHttpMsg,
    IN XCAP_BOOL bIsChunkBody, IN const XCAP_CHAR *pcChunkExt, IN const XCAP_UINT8 *pucBody, IN XCAP_UINT32 ulBodyLen,
    OUT XCAP_UINT8 **ppucEncBuf, INOUT XCAP_UINT32 *pulEncBufLen, OUT XCAP_UINT32 *pulMsgLen,
    IN const XCAP_VOID *pvAuxData);

/* ****************************************************************************
 Function Name: XcapCodecApiEncodeChunkedBody
 Description  : API to encode body to chunk format.  Before invoking this api,
                APP must invoke XcapCodecApiEncodeHttpMsg API.
                This API must be invoked cycalically to encode all chunk bodies.

                Note: Currenly this api is not supported, since setting
                bIsChunkBody = XCAP_TRUE is not supported in
                XcapCodecApiEncodeHttpMsg api.
 Input        :
    pvCodecCtx - codec context (created by XcapCodecApiCreateCtx api)

    hHttpMsg   - hypertext transfer protocol msg handle (same as passed in
                 XcapCodecApiEncodeHttpMsg API).

    bIsFinished - flag to indicate whether api is invoked for last chunk body.

            XCAP_TRUE: Indicates last chunk is getting encoded.
                a.
                when pucChunkData is valid pointer:
                  then it will be encoded in chunk format. also last chunk
                  with size 0 will be added to encoded msg. So totally two
                  chunks will be added to encoded buffer.
                  pcChunkExt[0] if not NULL, then this chunk-extension will be
                  added to chunk corresponding to pucChunkData.
                  pcChunkExt[1] if not NULL, then this chunk-extension will be
                  added to last chunk of size 0.
                when pucChunkData is NULL:
                  then it is treated as, this api is invoked to encode last
                  chunk only. so last chunk with size 0 will be encoded.
                  so only one chunk will be added to encoded buffer.
                  pcChunkExt[0] if not NULL, then this chunk-extension will be
                  added to encoded chunk.
                  pcChunkExt[1] will be ignored.
                b. trailer header will be added to encoded msg, if hHttpMsg has
                  it. trailer header can be set using XcapApiAddBodyTrailHeader
                  api before invoking XcapCodecApiEncodeHttpMsg api.

            XCAP_FALSE: Indicates chunk getting encoded is not last chunk and
                this api will be invoked again.
                a.
                when pucChunkData is valid pointer:
                  then it will be encoded in chunk format.
                  pcChunkExt[0] if not NULL, then this chunk-extension will be
                  added to chunk corresponding to pucChunkData.
                  pcChunkExt[1] will be ignored.
                when pucChunkData is NULL:
                  api will return error.


    pucChunkData - chunk body of the hypertext transfer protocol msg. will be
            used as described in bIsFinished para.

    ulChunkSize - length of pucChunkData.

    acChunkExt - if acChunkExt[0] and acChunkExt[1] not NULL, then chunk
              extension will be added to chunk, as described in bIsFinished
              para.

    pulEncBufLen- length of pucEncBuf buffer.

    pvAuxData - for future use

 Output       :
    pucEncBuf - encoded hypertext transfer protocol chunk body.
             memory for this buffer should be allocated by APP.
             if this memory is not sufficient to accomodate
             the encoded chunk body, then stack will return
             XCAP_ERR_BUF_SIZE_TOO_SMALL error and give required buf size in
             para pulMsgLen. APP need to allocate pucEncBuf same as pulMsgLen
             and need to invoke XcapCodecApiEncodeChunkedBody again.

     pulEncBufLen - length of pucEncBuf buffer. after successful encoding the
            chunk body, it gives actual data length filled in pucEncBuf.

     pulMsgLen - actual chunk body length (when api returns XCAP_SUCCESS or
           XCAP_ERR_BUF_SIZE_TOO_SMALL error).  Also pls check the description
           of pucEncBuf parameter.

     pvAuxData - for future use


 Return Value :
            XCAP_SUCCESS:  On success
            XCAP_ERR_BUF_SIZE_TOO_SMALL : as described in pucEncBuf para
            XCAP_ERR_CODE

**************************************************************************** */
XCAPPUBFUN XCAP_ERR_CODE XcapCodecApiEncodeChunkedBody(IN const XCAP_VOID *pvCodecCtx, IN XCAP_HANDLE hHttpMsg,
    IN XCAP_BOOL bIsFinished, IN const XCAP_UINT8 *pucChunkData, IN XCAP_UINT32 ulChunkSize,
    IN const XCAP_CHAR *acChunkExt[2], OUT XCAP_UINT32 *pulEncBufLen, OUT XCAP_BOOL *pucEncBuf,
    OUT XCAP_UINT32 *pulMsgLen, IN const XCAP_VOID *pvAuxData);

/* ****************************************************************************
 Function Name: XcapApiCreateMsgCodecCtxt
 Description  : API to create an XCAP hypertext transfer protocol message
             handle (request or response).

                The msg handle created by this api can be passed as input to
                other codec apis whose name starts with XcapCodecApi. Also this
                handle can be passed to other DSM apis like XcapApiGetMethod,
                XcapApiSetMethod etc.

                Usage of phHttpMsg is not thread safe, hence must be used
                from sigle thread at a time.

                Hypertext transfer protocol msg handle (phHttpMsg) created by
                this api can be destroyed using XcapApiDestroyMsg api.

                Only difference in XcapApiCreateMsg and this api is that,
                this api takes codec context as input. Hence operations on
                msg handle created by this api uses callbacks from codec
                context. However operations on msg handle created by
                XcapApiCreateMsg api uses global callbacks registered using api
                XcapApiRegSysBaseFunc and XcapApiRegDebugFunc apis.

 Input        : ppvCodecCtx - codec context.
                bIsRequest - Indicates whether the message is a request message
                  or response message.
                  XCAP_TRUE : request msg
                  XCAP_FALSE: response msg


 Output       : phHttpMsg - Handle of the created XCAP hypertext transfer
                 protocol message.

 Return Value :
               XCAP_SUCCESS:  On success
               XCAP_ERR_CODE
**************************************************************************** */
XCAPPUBFUN XCAP_ERR_CODE XcapApiCreateMsgCodecCtxt(IN XCAP_VOID *pvCodecCtx, IN XCAP_BOOL bIsRequest,
    OUT XCAP_HANDLE *phHttpMsg); /* New Codec APIs - End */

/* New DSM APIs - Start
    Note: These new DSM apis are addition to old DSM apis.
    Old DSM apis are below:
    XcapApiCreateMsg
    XcapApiDestroyMsg
    XcapApiCloneMsg
    XcapApiGetMsgType
    XcapApiGetVersion
    XcapApiGetMethod
    XcapApiGetUri
    XcapApiGetUriStruct
    XcapApiGetHeaderNum
    XcapApiGetHeaderByIndex
    XcapApiGetHeaderByName
    XcapApiGetBody
    XcapApiGetRespCode
    XcapApiSetVersion
    XcapApiSetMethod
    XcapApiSetUri
    XcapApiSetBody
    XcapApiSetRespCode
    XcapApiAddHeader
    XcapApiDelHeaderByName
PDT need to use these old apis as well as below new apis.
For old api information, can refer developer guide. */
/* ****************************************************************************
 Function Name: XcapApiAddBodyTrailHeader
 Description  : API to add trailer header to last chunk of hypertext transfer
                protocol body.

                This api is useful/supported only when msg body is
                getting encoded as chunk and using XcapCodecApiEncodeHttpMsg or
                XcapCodecApiEncodeChunkedBody api (not supported in other apis).

                Note: Currenly this api is not supported, since setting
                bIsChunkBody = XCAP_TRUE is not supported in
                XcapCodecApiEncodeHttpMsg api (sending chunk bodies not
                supported, so setting trailer header also not supported).

                Note: This api is currently not supported, since

 Input        : hHttpMsg - hypertext transfer protocol msg handle.
                pcName   - trailer header name
                pcValue  - trailer header value

 Output       : None

 Return Value :
               XCAP_SUCCESS:  On success
               XCAP_ERR_CODE
**************************************************************************** */
XCAPPUBFUN XCAP_ERR_CODE XcapApiAddBodyTrailHeader(IN XCAP_HANDLE hHttpMsg, IN const XCAP_CHAR *pcName,
    IN const XCAP_CHAR *pcValue);

/* ****************************************************************************
 Function Name: XcapApiGetBodyType
 Description  : API to get hypertext transfer protocol body type.

                Currently this api is supported only with the output
                msg handle of XcapCodecApiStreamDelemit api (not supported
                with msg handle given by stack as output in
                request/response indication callbacks).

                Based on the output of this api, APP can invoke
                XcapApiGetBody or XcapApiGetBodyChunk to retrive the hypertext
                transfer protocol body.
                How to obtain msg body?
                - First APP need to get body type using api XcapApiGetBodyType.
                - If body type is none (XCAP_CODEC_HTTP_BODY_TYPE_NONE), means
                  there is no body in the hypertext transfer protocol msg.
                - If body type is normal (XCAP_CODEC_HTTP_BODY_TYPE_NORMAL) or
                  chunk-combined (XCAP_CODEC_HTTP_BODY_TYPE_CHUNK_COMBINED) ,
                  means there is one body entity in the hypertext transfer
                  protocol msg handle. This
                  body can be obtained using XcapApiGetBody api.
                - If body type is chunk (XCAP_CODEC_HTTP_BODY_TYPE_CHUNK), means
                  msg has chunked bodies. There can be several chunks.
                  These chunks can be obtained by making use of
                  XcapApiGetBodyChunkNum and XcapApiGetBodyChunk apis.
                - Also if app want to get trailer headers when body type is
                  chunk or chunk-combined, then can be obtained by making use
                  of XcapApiGetTrailHeaderNum and
                  XcapApiGetTrailHeaderByIndex apis.

                Note:  Currently XCAP_CODEC_HTTP_BODY_TYPE_CHUNK is not given
                as output, since not supported.  XcapCodecApiStreamDelemit api
                always combine chunks. Hence if msg has chunk bodies, then
                XCAP_CODEC_HTTP_BODY_TYPE_CHUNK_COMBINED will be given as
                output.

 Input        : hHttpMsg - hypertext transfer protocol msg handle.

 Output       : pulBodyType - Body type EN_XCAP_CODEC_HTTP_BODY_TYPE  None, Normal, Chunk

 Return Value :XCAP_SUCCESS:  On success
               XCAP_ERR_CODE
**************************************************************************** */
XCAPPUBFUN XCAP_ERR_CODE XcapApiGetBodyType(IN XCAP_HANDLE hHttpMsg, OUT XCAP_UINT32 *pulBodyType);
/* ****************************************************************************
 Function Name: XcapApiGetBodyChunkNum
 Description  : API to get number of chunk bodies.

                Currently this api is supported only with the output
                msg handle of XcapCodecApiStreamDelemit api (not supported
                with msg handle given by stack as output in
                request/response indication callbacks).

                If output of XcapApiGetBodyType api indicates body type is
                chunk :XCAP_CODEC_HTTP_BODY_TYPE_CHUNK, means
                there can be several chunks.
                These chunks can be obtained by making use of
                XcapApiGetBodyChunkNum and XcapApiGetBodyChunk apis.

                Note:  Currently this api is not supported.
                This api is useful only when body type is
                XCAP_CODEC_HTTP_BODY_TYPE_CHUNK. But XcapCodecApiStreamDelemit
                api always combine chunks. Hence if msg has chunk bodies, then
                XCAP_CODEC_HTTP_BODY_TYPE_CHUNK_COMBINED will be given as
                output.

 Input        : hHttpMsg - hypertext transfer protocol msg handle.

 Output       : pulChunkNum - Number of chunk bodies

 Return Value :
               XCAP_SUCCESS:  On success
               XCAP_ERR_CODE
**************************************************************************** */
XCAPPUBFUN XCAP_ERR_CODE XcapApiGetBodyChunkNum(IN XCAP_HANDLE hHttpMsg, OUT XCAP_UINT32 *pulChunkNum);

/* ****************************************************************************
 Function Name: XcapApiGetBodyChunk
 Description  : API to get the body chunk

                Currently this api is supported only with the output
                msg handle of XcapCodecApiStreamDelemit api (not supported
                with msg handle given by stack as output in
                request/response indication callbacks).

                If output of XcapApiGetBodyType api indicates body type is
                chunk :XCAP_CODEC_HTTP_BODY_TYPE_CHUNK, means
                there can be several chunks.
                These chunks can be obtained by making use of
                XcapApiGetBodyChunkNum and XcapApiGetBodyChunk apis.
                APP can invoke this api same number of time as
                number of chunk bodies outputed by XcapApiGetBodyChunkNum api.

                Also if app want to get trailer headers, then can be obtained
                by making use of XcapApiGetTrailHeaderNum and
                XcapApiGetTrailHeaderByIndex apis.

                Note:  Currently this api is not supported.
                This api is useful only when body type is
                XCAP_CODEC_HTTP_BODY_TYPE_CHUNK. But XcapCodecApiStreamDelemit
                api always combine chunks. Hence if msg has chunk bodies, then
                XCAP_CODEC_HTTP_BODY_TYPE_CHUNK_COMBINED will be given as
                output.

 Input        : hHttpMsg - hypertext transfer protocol msg handle.
                ulIndex - chunk index.
                         value should not be greater than chunk number
                         output of XcapApiGetBodyChunkNum api.
                         0 means first chunk, 1 means second chunk and so on

 Output       : ppcChunkExt - chunk extension
                Memory will be from input msg handle, will be freed when msg
                handle is destroyed.

                pulChunkLen - chunk body len

                ppucChunk - chunk body
                Memory will be from input msg handle, will be freed when msg
                handle is destroyed.

 Return Value :
               XCAP_SUCCESS:  On success
               XCAP_ERR_CODE
**************************************************************************** */
XCAPPUBFUN XCAP_ERR_CODE XcapApiGetBodyChunk(IN XCAP_HANDLE hHttpMsg, IN XCAP_UINT32 ulIndex,
    OUT XCAP_CHAR **ppcChunkExt, OUT XCAP_UINT32 *pulChunkLen, OUT XCAP_UINT8 **ppucChunk);

/* ****************************************************************************
 Function Name: XcapApiGetTrailHeaderNum
 Description  : API to get the number of trailer header number

                Currently this api is supported only with the output
                msg handle of XcapCodecApiStreamDelemit api (not supported
                with msg handle given by stack as output in
                request/response indication callbacks).

                If output of XcapApiGetBodyType api indicates body type is
                chunk (XCAP_CODEC_HTTP_BODY_TYPE_CHUNK or
                XCAP_CODEC_HTTP_BODY_TYPE_CHUNK_COMBINED), means
                there can be trailer headers in the last chunk.
                These trailer header can be obtained by making use of
                XcapApiGetTrailHeaderNum and XcapApiGetTrailHeaderByIndex apis.

                Note:  Currently this api is not supported.
                If msg has chunk bodies, then it will be combined together
                in XcapCodecApiStreamDelemit api and trailer headers will be
                ignored.

 Input        : hHttpMsg - hypertext transfer protocol msg handle.


 Output       : pulHeaderNum - trailer header number

 Return Value :
               XCAP_SUCCESS:  On success
               XCAP_ERR_CODE
**************************************************************************** */
XCAPPUBFUN XCAP_ERR_CODE XcapApiGetTrailHeaderNum(IN const XCAP_HANDLE hHttpMsg, OUT XCAP_UINT32 *pulHeaderNum);

/* ****************************************************************************
 Function Name: XcapApiGetTrailHeaderByIndex
 Description  : API to get the trailer header.

                Currently this api is supported only with the output
                msg handle of XcapCodecApiStreamDelemit api (not supported
                with msg handle given by stack as output in
                request/response indication callbacks).

                If output of XcapApiGetBodyType api indicates body type is
                chunk (XCAP_CODEC_HTTP_BODY_TYPE_CHUNK or
                XCAP_CODEC_HTTP_BODY_TYPE_CHUNK_COMBINED), means
                there can be trailer headers in the last chunk.
                These trailer header can be obtained by making use of
                XcapApiGetTrailHeaderNum and XcapApiGetTrailHeaderByIndex apis.
                APP can invoke this api same number of time as
                number of chunk bodies outputed by XcapApiGetTrailHeaderNum api.

                Note:  Currently this api is not supported.
                If msg has chunk bodies, then it will be combined together
                in XcapCodecApiStreamDelemit api and trailer headers will be
                ignored.


 Input        : hHttpMsg - hypertext transfer protocol msg handle.
                ulIndex - trailer header index.
                         value should not be greater than header number
                         output of XcapApiGetTrailHeaderNum api.
                         0 means first trailer header,
                         1 means second trailer header and so on

 Output       : ppcName - trailer header name
                Memory will be from input msg handle, will be freed when msg
                handle is destroyed.

                ppcValue - trailer header value
                Memory will be from input msg handle, will be freed when msg
                handle is destroyed.

 Return Value :
               XCAP_SUCCESS:  On success
               XCAP_ERR_CODE
**************************************************************************** */
XCAPPUBFUN XCAP_ERR_CODE XcapApiGetTrailHeaderByIndex(IN const XCAP_HANDLE hHttpMsg, IN XCAP_UINT32 ulIndex,
    OUT XCAP_CHAR **ppcName, OUT XCAP_CHAR **ppcValue); /* New DSM APIs - End */

/* [XCAP_CODEC_SUPPORT][VPP-26285][End] */
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* _XCAP_API_H_ */
