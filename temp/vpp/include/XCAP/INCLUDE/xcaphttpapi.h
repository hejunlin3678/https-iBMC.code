/* *****************************************************************************

    Copyright (c) Huawei Technologies Co., Ltd. 2008-2019. All rights reserved.
    Description:   inerface define for HTTP module.
    Function List: NA
    Author:  L36241,W57322
    Create:  2008-4-10

***************************************************************************** */
#ifndef XCAPHTTPAPI_H
#define XCAPHTTPAPI_H

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
             HTTPs 接口定义
================================================= */
/* DNS 查询回调 API */
typedef XCAP_ERR_CODE (*PTXcapHttpDnsQuery)(IN XCAP_HANDLE hQueryCb, /* DNS查询句柄 */
    IN XCAP_BOOL blType,                                             /* XCAP_TRUE--同步,XCAP_FALSE--异步 */
    IN const XCAP_UINT8 *pucDomainName,                              /* 输入的域名 */
    OUT XcapIpAddr *pstAddr,                                         /* 同步查询得到的[VPP-25322] */
    OUT XCAP_ULONG *pulQueryId);                                     /* 异步方式下需要，返回查询ID */

typedef XCAP_VOID (*PTXcapHttpOnRequest)(IN XCAP_HANDLE ulAppId, IN XCAP_HANDLE hReqHandle, IN XCAP_HANDLE hRequestMsg,
    OUT XCAP_ULONG *pulRequsetId);

typedef XCAP_VOID (*PTXcapHttpOnResponse)(IN XCAP_HANDLE ulAppId, IN XCAP_ULONG ulRequestId,
    IN XCAP_HANDLE hResponseMsg);

/* Type of HTTPs Request Error */
typedef enum _EN_XCAP_HTTP_REQ_ERROR_TYPE {
    XCAP_HTTP_REQ_ERROR_CONNECTION_CLOSE = 0,       /* 该请求的连接被关闭，导致请求失败 */
    XCAP_HTTP_REQ_ERROR_WAIT_APP_RESPONSE_TIME_OUT, /* 请求等待上层响应超时 */
    XCAP_HTTP_REQ_ERROR_ENCODE_RESP_FAILURE,        /* 发送响应消息的过程中编码失败 */
    XCAP_HTTP_REQ_ERROR_ENCODE_REQ_FAILURE,         /* 在发送请求的过程中编码失败(客户端) */
    XCAP_HTTP_REQ_ERROR_CREATE_CONNECTION_FAILURE,  /* 发送请求时[VPP-25322]链路不成功(客户端) */
    XCAP_HTTP_REQ_ERROR_SEND_REQ_FAILURE,           /* 发送请求消息失败 */
    XCAP_HTTP_REQ_ERROR_DECODE_RESP_FAILURE,        /* 收到响应消息，但解码失败，导致请求失败(客户端) */
    XCAP_HTTP_REQ_ERROR_REQ_TIMEOUT,                /* 发送请求后等响应超时(客户端) */

    /* 长连接中，用户主动删除了其中一个事务。该事务之后的所有事务均按此原因上报 */
    XCAP_HTTP_REQ_ERROR_PIPELINE_USER_CLOSE,
    XCAP_HTTP_REQ_ERROR_TLS_UPGRADE_FAILURE,     /* 该请求TLS升级失败 */
    XCAP_HTTP_REQ_ERROR_REPORT_RESPONSE_FAILURE, /* 协议栈上报响应失败，连接上的后续事务也按此上报 */
    XCAP_HTTP_REQ_ERROR_END
} EN_XCAP_HTTP_REQ_ERROR_TYPE;

/* [VPP-12363][NE40-Perf-Req][Start] */
/* Get tick task info callback. Returns XCAP_SUCCESS on success else any other
value on failure */
typedef XCAP_UINT32 (*PTXcapHttpGetTickTaskInfo)(IN XCAP_ULONG ulAppId, /* Application Id */
    OUT XCAP_ULONG *pulThreadId,                                        /* Thread Id */
    OUT XCAP_UINT32 *pulDelay);                                         /* Thread delay */

/* ****************************************************************************
 Callback Name: PTXcapHttpTaskRunFreeCpu

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
typedef XCAP_ERR_CODE (*PTXcapHttpTaskRunFreeCpu)(IN XCAP_ULONG ulAppId, /* Application Id */
    IN XCAP_UINT32 ulServiceMode,                                        /* EN_XCAP_SERVICE_MODE */
    IN XCAP_UINT32 ulTaskType);                                          /* Task type */

/* [VPP-12363][NE40-Perf-Req][End] */
typedef XCAP_VOID (*PTXcapHttpOnErrorReport)(IN XCAP_HANDLE ulAppId, IN XCAP_ULONG ulRequestId,
    IN XCAP_UINT32 ulErrType); /* EN_XCAP_HTTP_REQ_ERROR_TYPE */

/* IP过滤器。返回XCAP_TRUE，则该IP是合法的；返回XCAP_FALSE，则该IP是非法的 */
typedef XCAP_BOOL (*PTXcapHttpIpFilter)(IN XCAP_HANDLE ulAppId, IN const XcapIpAddr *pstIpAddr);

/* 流控接口。
   返回XCAP_TRUE， 则会向用户层上报改请求，长连接是否断开由请求的
                    connection:Close头域决定；
   返回XCAP_FALSE，则会向对端回应503 Service Unavailable响应，同时:
                       请求带connection:Close头域，关闭长连接；
                       请求不带connection:Close头域，pbIsCloseConn为真，
                                关闭长连接；
                       请求不带connection:Close头域，pbIsCloseConn为假，
                                不关闭长连接
 */
typedef XCAP_BOOL (*PTXcapHttpTrafficControl)(IN XCAP_HANDLE ulAppId, IN const XcapIpAddr *pstIpAddr,
    IN XCAP_HANDLE hHttpMsg, OUT XCAP_BOOL *pbIsCloseConn);

/* 启动定时器 */
typedef XCAP_ERR_CODE (*PTXcapHttpStartRelTimer)(IN XCAP_UINT32 ulLength, IN XCAP_ULONG ulPara,
    OUT XCAP_HANDLE *phTimerHandle);

/* 停止定时器 */
typedef XCAP_VOID (*PTXcapHttpStopRelTimer)(IN const XCAP_HANDLE *phTimerHandle);

/* [VPP-22529][Del-Inst-Api-Invk-RunTime-Safe-Support][Start] */
typedef XCAP_ERR_CODE (*PTXcapHttpStartRelTimerEx)(IN XCAP_HANDLE hStackHandle, IN XCAP_UINT32 ulLength,
    IN XCAP_ULONG ulPara, OUT XCAP_HANDLE *phTimerHandle);

typedef XCAP_ERR_CODE (*PTXcapHttpDnsQueryEx)(IN XCAP_HANDLE hStackHandle, IN XCAP_HANDLE hQueryCb, /* DNS查询句柄 */
    IN XCAP_BOOL blType,                /* XCAP_TRUE--同步,XCAP_FALSE--异步 */
    IN const XCAP_UINT8 *pucDomainName, /* 输入的域名 */
    OUT XcapIpAddr *pstAddr,            /* 同步查询得到的 [VPP-25322] */
    OUT XCAP_ULONG *pulQueryId);        /* 异步方式下需要，返回查询ID */

/* [VPP-22529][Del-Inst-Api-Invk-RunTime-Safe-Support][End] */
/* Callback function for differentiating multiple users, including message
    reporting and timer. It is for XcapHttpApiCreateApp. */
typedef struct _XcapHttpUserCallBackFunc {
    PTXcapHttpOnRequest pfOnRequest;           /* 收到请求的回调处理函数 */
    PTXcapHttpOnResponse pfOnResponse;         /* 收到响应的回调处理函数 */
    PTXcapHttpOnErrorReport pfOnErrorReport;   /* 通知上层错误的回调处理函数 */
    PTXcapHttpDnsQuery pfDnsQuery;             /* DNS查询接口，可为空 */
    PTXcapHttpIpFilter pfIpFilter;             /* IP过滤接口，可为空 */
    PTXcapHttpTrafficControl pfTrafficControl; /* 流控接口，可为空 */
    PTXcapHttpStartRelTimer pfStartRelTimer;
    PTXcapHttpStopRelTimer pfStopRelTimer;
} XcapHttpUserCallBackFunc;

/* [VPP-22529][Del-Inst-Api-Invk-RunTime-Safe-Support][Start] */
/* Callback function for differentiating multiple users, including message
    reporting and timer. It is for XcapHttpApiCreateAppEx. */
typedef struct _XcapHttpUserCallBackFuncEx {
    PTXcapHttpOnRequest pfOnRequest;           /* 收到请求的回调处理函数 */
    PTXcapHttpOnResponse pfOnResponse;         /* 收到响应的回调处理函数 */
    PTXcapHttpOnErrorReport pfOnErrorReport;   /* 通知上层错误的回调处理函数 */
    PTXcapHttpDnsQueryEx pfDnsQueryEx;         /* DNS查询接口，可为空 */
    PTXcapHttpIpFilter pfIpFilter;             /* IP过滤接口，可为空 */
    PTXcapHttpTrafficControl pfTrafficControl; /* 流控接口，可为空 */
    PTXcapHttpStartRelTimerEx pfStartRelTimerEx;
    PTXcapHttpStopRelTimer pfStopRelTimer;
} XcapHttpUserCallBackFuncEx;

/* [VPP-22529][Del-Inst-Api-Invk-RunTime-Safe-Support][End] */
/* DNS同步/异步查询 */
typedef enum _EN_XCAP_HTTP_DNS_QUERY_TYPE {
    XCAP_HTTP_DNS_QUERY_TYPE_SYN = 0, /* DNS同步查询 */
    XCAP_HTTP_DNS_QUERY_TYPE_ASY      /* DNS异步查询 */
} EN_XCAP_HTTP_DNS_QUERY_TYPE;

/* HTTPs TLS模式 */
typedef enum _EN_XCAP_HTTP_TLS_MODE {
    XCAP_HTTP_TLS_MODE_NONE = 0,         /* 普通TCP模式，不支持TLS升级        */
    XCAP_HTTP_TLS_MODE_UPGRADE_OPTION,   /* 可选支持TLS升级模式               */
    XCAP_HTTP_TLS_MODE_UPGRADE_REQUIRED, /* 强制TLS升级模式                   */
    XCAP_HTTP_TLS_MODE_SERVER_HTTPS,     /* HTTPS模式。服务端使用。客户端需在消息URI中携带"https://" */
    XCAP_HTTP_TLS_MODE_END
} EN_XCAP_HTTP_TLS_MODE;

/* [VPP-8980][WSP-15][Start] */
/* XCAP_HTTP instance specific configuration enum */
typedef enum _EN_XCAP_HTTP_CONFIG_PARA {
    /* For client: To enable receiving response message, when Content-Length
    header is not set and message has body. End of the message will be
    determined by connection closure by server. For successful processing of
    HTTPs/1.1 response message, Connection header must be present in the
    response message. */
    XCAP_HTTP_CLIENT_RECV_RESP_WITHOUT_CONTENTLENGTH_HEADER = 0,

    /* [VPP-12363][NE40-Perf-Req][Start] */
    /* For client and server both: When this configuration is enabled, XCAP
    server and client threads use event instead of sleep. Thread will wait for
    event XCAP_EVENT_RESUME_PROCESSING. If this event occured, means there is
    something to process. So thread will do necesery procesing and again wait
    for this event. This will help in improving the performance, by reducing
    sleep time of threads.
    */
    XCAP_HTTP_CLIENT_SERVER_USE_EVENT_FOR_SLEEP = 1,

    /* For client and server both: When this configuration is enabled, XCAP
    server and client do not use socket select function to look read/write/
    err etc events on socket. APP has to give these event to xcap stack from
    SOCK task. PDT should use this feature only on 32 bit systems. This feature
    is mainly for the PDT using VRP sockets, which has async SOCK task to
    look the socket events. */
    XCAP_HTTP_CLIENT_SERVER_USE_APP_SOCK_TASK_FOR_SELECT = 2,

    /* [VPP-12363][NE40-Perf-Req][End] */
    /* [vpp-9874][n00742771][start] */
    /* To enable server to create and use socket in same thread.
    This configuration will be valid only when:
    worker thread configuration is 0 while
    in XcapHttpApiCreateApp ulWorkThreadNum of pstInitAppParam
    and listen mode is single thread XCAP_LISTEN_THREAD_MODE_SINGLE while
    XcapHttpApiCreateApp usListenThreadMode of pstInitAppParam.
    This configuration is not supported for client or client-server instance.
    */
    XCAP_HTTP_SERVER_CREATE_USE_SOCKET_IN_SAME_THREAD = 3,

    /* [vpp-9874][n00742771][End] */
    /* [VPP-19223][thr-auto-exit][start] */
    /* For client and server both: When this configuration xcap threads free
    resource and exit by itself. So APP no need to delete thread in
    pfBlockWaitThreadExit callback.
    When used DOPRA and this feature, it is suggested to use XcapHttpApiKickOff
    API from other than VOS root task and after pid init is success. This is to
    avoid hang in XcapHttpApiKickOff API when internal failure happens. When
    internal failure happens in XcapHttpApiKickOff, it will try to delete the
    threads (if created before). So it will wait untill threads exit by itself.
    But it will keep on waiting, since threads are not spawned by DOPRA, untill
    VOS root task is suspended. Since threads are not spawned, threads will
    never auto exit. So XcapHttpApiKickOff API will keep on waiting and hang. */
    XCAP_HTTP_CLIENT_SERVER_DEL_APP_API_WAIT_TILL_TH_AUTO_EXIT = 4,

    /* [VPP-19223][thr-auto-exit][end] */
    /* [DTS2016060704445][max-alloc-limit][k71093][Start] */
    /* This limit is for max value in Content-Length header and also
    max value for combining chunks size. Default is 100 MB. */
    XCAP_HTTP_CLIENT_SERVER_MAX_MSG_BODY_SIZE = 5,

    /* This limit is for max header number in the hypertext transfer protocol
    msg. Default max header number is 1000. */
    XCAP_HTTP_CLIENT_SERVER_MAX_MSG_HDR_NUM = 6,

    /* [DTS2016060704445][max-alloc-limit][k71093][End] */
    XCAP_HTTP_CONFIG_PARA_END
} EN_XCAP_HTTP_CONFIG_PARA;

/* [VPP-8980][WSP-15][End] */
/* [VPP-4472][SOAP-68][d00902689][Start] */
/* HTTPs Connect Event Info Enum */
typedef enum _EN_XCAP_HTTP_CONNECTION_EVENT_TYPE {
    /* connection closed  (for xcap server and client).  Possible reasons can be
      TLS_UPGRADE_FAILED, EPOLL_OP_FAILED,
      SOCKET_SELECT_EXCEPTION, RECV_DATA_FAILED,
      PEER_CLOSE_CONNECTION, CONNECTION_CLOSE_HEADER_IN_MSG,
      IDLE_CONNECTION_TIMEOUT, TRANSACTION_TIMEOUT,
      INTERNAL_ERROR */
    XCAP_HTTP_CONNECTION_EVENT_CLOSE = 0,

    /* connection made success (for xcap server and client) */
    XCAP_HTTP_CONNECTION_EVENT_CREATE_SUCCESS,

    /* connection create failed (only for xcap client). Possible reasons can be
    BIND_ERROR, CONNECT_ERROR, SETSOCKOPT_ERROR, SSL_ERROR,
    SOCKET_SELECT_EXCEPTION, EPOLL_OP_FAILED */
    XCAP_HTTP_CONNECTION_EVENT_CREATE_FAIL,

    XCAP_HTTP_CONNECTION_EVENT_END
} EN_XCAP_HTTP_CONNECTION_EVENT_TYPE;

/* Callback function for reporting the connection event. */
typedef XCAP_VOID (*PTXcapHttpConnectEvent)(IN XCAP_HANDLE hStackHandle, /* XCAP instance handle */
    IN XCAP_HANDLE ulAppId,                                              /* Handle of upper layer user */
    IN XCAP_UINT32 ulEvent,                                              /* EN_XCAP_HTTP_CONNECTION_EVENT_TYPE */
    IN XCAP_UINT32 ulEventReason,                                        /* Currently unused */
    IN const XcapConnInfoS *pstConnInfo,                                 /* XCAP connection information */
    IN XCAP_VOID *pvAuxData);                                            /* For Future use */

/* [VPP-4472][SOAP-68][d00902689][End] */
/* [VPP-25237][Set-Listen-Sock-Opt][Start] */
/* Callback function for setting the listen socket options. */
typedef XCAP_ERR_CODE (*PTXcapHttpSetListenSockOpt)(IN XCAP_HANDLE hStackHandle, /* XCAP instance handle */
    IN XCAP_ULONG ulAppId,                                                       /* Handle of upper layer user */
    IN XCAP_VOID *pvAppData,                                                     /* APP data */
    IN XCAP_UINT32 ulListenSockFd,                                               /* Listener socket id */
    IN const XcapIpAddr *pstListenIpAddr,                                        /* Server's listener address
                                                                                    and port information */
    IN XCAP_VOID *pvAuxData);                                                    /* For Future use, may be TLS flag */

/* [VPP-25237][Set-Listen-Sock-Opt][End] */
/* Used to store parameters related to Listen Address */
typedef struct _XcapHttpListenIpAddr {
    XcapIpAddr stIpAddr;
    XCAP_UINT32 ulTlsMode; /* TLS模式 EN_XCAP_HTTP_TLS_MODE */
#ifdef XCAP_64BIT_ALIGNED
    XCAP_UINT32 ulSpare; /* For 64 bit alignment */
#endif
} XcapHttpListenIpAddr;

#define XCAP_HTTP_MAX_LISTEN_ADDR_NUM XCAP_BASE_MAX_LISTEN_ADDR_NUM

/* Used to set the user parameters, including maximum number of connections
   and sending/receiving buffer. */
typedef struct _XcapHttpInitAppParam {
    XCAP_UINT16 usServiceMode;      /* 包括客户端、服务端、Both: EN_XCAP_SERVICE_MODE */
    XCAP_UINT16 usListenThreadMode; /* 单线程、多线程模式:EN_XCAP_LISTEN_THREAD_MODE */
    XCAP_UINT32 ulWorkThreadNum;    /* 配置工作线程数 */
    XCAP_UINT32 ulMaxLinkPerThread; /* 每个线程支持的最大连接数 */
    XCAP_UINT32 ulLinkSendBuff;     /* 配置用户下所有连接的发送缓存 */
    XCAP_UINT32 ulLinkRecvBuff;     /* 配置用户下所有连接的接收缓存 */
    XCAP_UINT32 ulListenNum;        /* 侦听个数 */
    XcapHttpListenIpAddr stListenIpAddr[XCAP_HTTP_MAX_LISTEN_ADDR_NUM];

    XCAP_UINT32 ulDNSQueryType;       /* EN_XCAP_HTTP_DNS_QUERY_TYPE */
    XCAP_UINT32 ulTransactionTimeOut; /* 事务超时时长  */
    XCAP_UINT32 ulConnWaitTime;       /* 新连接如果在这段时间内没有收到任何消息连接就会超时关闭 */
#ifdef XCAP_64BIT_ALIGNED
    XCAP_UINT32 ulSpare; /* For 64 bit alignment */
#endif
} XcapHttpInitAppParam;

XCAP_ERR_CODE XcapHttpApiSetSslConnectTimerValue(IN XCAP_HANDLE hStackHandle, IN XCAP_UINT32 ulMaxSslConnectTime,
    IN const XCAP_VOID *pvAuxData);

/* [VPP-25322]用户，并设置与用户相关的参数 */
XCAP_ERR_CODE XcapHttpApiCreateApp(IN XCAP_ULONG ulAppId,   /* 上层用户所标识的句柄 */
    IN const XcapHttpUserCallBackFunc *pstUserCallBackFunc, /* 区分多个用户的回调函数 */
    IN const XcapHttpInitAppParam *pstInitAppParam,         /* 设置该用户参数，包括最大连接数，发送、接收缓存 */
    OUT XCAP_HANDLE *phStackHandle);                        /* 协议栈用户句柄 */

/* HTTPs 用户开工 */
XCAP_ERR_CODE XcapHttpApiKickOff(IN XCAP_HANDLE hStackHandle);

/* 删除用户 */
XCAP_ERR_CODE XcapHttpApiDeleteApp(INOUT XCAP_HANDLE *phStackHandle); /* 协议栈用户句柄 */

/* 根据指定[VPP-25322]添加一个侦听 [VPP-25322] */
XCAP_ERR_CODE XcapHttpApiCreateListen(IN XCAP_HANDLE hStackHandle, /* 协议栈用户句柄 */
    IN XCAP_UINT32 ulTlsMode,                                      /* EN_XCAP_HTTP_TLS_MODE */
    IN const XcapIpAddr *pstLocalAddr);                            /* 本端侦听地址 */

/* 根据指定删除一个侦听 [VPP-25322] */
XCAP_ERR_CODE XcapHttpApiDeleteListen(IN XCAP_HANDLE hStackHandle, /* 协议栈用户句柄 */
    IN const XcapIpAddr *pstLocalAddr);                            /* 本端侦听地址 */

/* 字符串转换成结构 [VPP-25322][SensitiveWordRemoval] */
/* usIpType: XCAP_IP_ADDR_TYPE_IPV4 or XCAP_IP_ADDR_TYPE_IPV6 */
XCAP_ERR_CODE XcapHttpApiIpStringToAddress(IN XCAP_UINT16 usIpType, IN const XCAP_CHAR *pcStr, /* IP地址字符串 */
    IN XCAP_UINT16 usPort,                                                                     /* 端口号 */
    OUT XcapIpAddr *pstIpAddr); /* 传出转化后的结构 [VPP-25322] */

XCAP_ERR_CODE XcapHttpApiDoRequest(IN XCAP_HANDLE hStackHandle, IN XCAP_ULONG ulRequestId, /* 用户的请求标识 */
    IN XCAP_HANDLE hRequestMsg, OUT XCAP_HANDLE *phReqHandle);                             /* 协议栈的请求标识 */

XCAP_ERR_CODE XcapHttpApiDoResponse(IN XCAP_HANDLE hStackHandle, IN XCAP_HANDLE hReqHandle, /* 协议栈的请求标识 */
    IN XCAP_HANDLE hResponseMsg);

XCAP_ERR_CODE XcapHttpApiCloseTransaction(IN XCAP_HANDLE hStackHandle, IN XCAP_HANDLE hReqHandle);

XCAP_VOID XcapHttpApiDnsAsyRsp(IN XCAP_HANDLE hQueryCb, /* DNS查询句柄 */
    IN XCAP_ULONG ulQueryId,                            /* 查询结果 */
    IN XCAP_UINT32 ulQueryResult,                       /* 查询结果 */
    IN const XcapIpAddr *pstAddr);                      /* 同步查询得到的 [VPP-25322] */

/* [VPP-22529][Del-Inst-Api-Invk-RunTime-Safe-Support][Start] */
XCAP_VOID XcapHttpApiDnsAsyRspEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle, IN XCAP_HANDLE hQueryCb,        /* DNS查询句柄 */
    IN XCAP_ULONG ulQueryId,                                     /* 查询ID */
    IN XCAP_UINT32 ulQueryResult,                                /* 查询结果 */
    IN const XcapIpAddr *pstAddr);                               /* 同步查询得到的 [VPP-25322] */

/* [VPP-22529][Del-Inst-Api-Invk-RunTime-Safe-Support][End] */
XCAP_ERR_CODE XcapHttpApiDeleteConnection(IN XCAP_HANDLE hStackHandle, IN const XcapIpAddr *pstRemoteAddr);

/* ***************************
  HTTP消息操作函数
*************************** */
XCAP_ERR_CODE XcapApiCreateHttpMsg(IN XCAP_BOOL bIsRequest, OUT XCAP_HANDLE *phHttpMsg);

XCAP_VOID XcapApiDestroyHttpMsg(IN XCAP_HANDLE *phHttpMsg);

XCAP_ERR_CODE XcapApiCloneHttpMsg(IN XCAP_HANDLE hHttpMsg, OUT XCAP_HANDLE *phClonedHttpMsg);

XCAP_ERR_CODE XcapApiGetHttpMsgType(IN const XCAP_HANDLE hHttpMsg, OUT XCAP_BOOL *pbIsRequest);

/* HTTP请求响应消息的操作接口 */
XCAP_CHAR *XcapApiGetHttpVersion(IN const XCAP_HANDLE hHttpMsg);

XCAP_ERR_CODE XcapApiSetHttpVersion(IN const XCAP_HANDLE hHttpMsg, IN const XCAP_CHAR *pcVersion);

XCAP_CHAR *XcapApiGetHttpMethod(IN const XCAP_HANDLE hHttpMsg);

XCAP_ERR_CODE XcapApiSetHttpMethod(IN const XCAP_HANDLE hHttpMsg, IN const XCAP_CHAR *pcMethod);

/* XCAP HTTPs URI Struct */
typedef enum _EN_XCAP_HTTP_URI_TYPE {
    XCAP_HTTP_URI_TYPE_NONE = 0,
    XCAP_HTTP_URI_TYPE_ASTERISK,         /* The uri is "*" */
    XCAP_HTTP_URI_TYPE_ABSOLUTE_URI = 2, /* absolute-URI   */
    XCAP_HTTP_URI_TYPE_PATH_ABSOLUTE,    /* path-absolute  */
    XCAP_HTTP_URI_TYPE_AUTHORITY = 4     /* authority      */
} EN_XCAP_HTTP_URI_TYPE;

/*
host = "[" ( IPv6address | IPvFuture  ) "]" | IPv4address | reg-name
*/
typedef enum _EN_XCAP_HTTP_URI_HOST_TYPE {
    XCAP_HTTP_URI_HOST_TYPE_NONE = 0,
    XCAP_HTTP_URI_HOST_TYPE_IPV4,
    XCAP_HTTP_URI_HOST_TYPE_IPV6 = 2,
    XCAP_HTTP_URI_HOST_TYPE_IPVFUTURE,
    XCAP_HTTP_URI_HOST_TYPE_REGNAME = 4
} EN_XCAP_HTTP_URI_HOST_TYPE;

/* HTTPs URI */
typedef struct _XcapHttpUri {
    XCAP_CHAR *pcScheme;    /* the URI scheme */
    XCAP_CHAR *pcPath;      /* the path string */
    XCAP_CHAR *pcUserinfo;  /* the userinfo part */
    XCAP_CHAR *pcQuery;     /* the query string */
    XCAP_CHAR *pcHost;      /* the host part */
    XCAP_UINT32 ulHostType; /* the type of pcHost,EN_XCAP_HTTP_URI_HOST_TYPE */
    XCAP_UINT16 usPort;     /* the port number */
    XCAP_UINT16 usType;     /* Uri type EN_XCAP_HTTP_URI_TYPE */
} XcapHttpUri;

XCAP_CHAR *XcapApiGetHttpUri(IN const XCAP_HANDLE hHttpMsg);

XCAP_ERR_CODE XcapApiSetHttpUri(IN const XCAP_HANDLE hHttpMsg, IN const XCAP_CHAR *pcHttpUri);

XCAP_ERR_CODE XcapApiSetHttpUriByStruct(IN const XCAP_HANDLE hHttpMsg, IN XCAP_BOOL bNeedEscape, /* 是否需要转义 */
    INOUT XcapHttpUri *pstHttpUri); /* pstHttpUri->ulHostType is also output para */

XCAP_ERR_CODE XcapApiGetHttpUriStruct(IN const XCAP_HANDLE hHttpMsg, OUT XcapHttpUri **ppstHttpUri);

XCAP_ERR_CODE XcapApiGetHttpHeaderNum(IN const XCAP_HANDLE hHttpMsg, OUT XCAP_UINT32 *pulHeaderNum);

/* [DTS2016060704445][max-alloc-limit][k71093][start] */
XCAP_ERR_CODE XcapApiSetMaxHttpHeaderNum(IN const XCAP_HANDLE hMsg, IN XCAP_UINT32 ulMaxHdrNum);

/* [DTS2016060704445][max-alloc-limit][k71093][end] */
XCAP_ERR_CODE XcapApiGetHttpHeaderByIndex(IN const XCAP_HANDLE hHttpMsg, IN XCAP_UINT32 ulIndex,
    OUT XCAP_CHAR **ppcName, OUT XCAP_CHAR **ppcValue);

XCAP_ERR_CODE XcapApiGetHttpHeaderByName(IN const XCAP_HANDLE hHttpMsg, IN const XCAP_CHAR *pcName,
    OUT XCAP_CHAR **ppcValue);

XCAP_ERR_CODE XcapApiDelHttpHeaderByName(IN const XCAP_HANDLE hHttpMsg, IN const XCAP_CHAR *pcName);

XCAP_ERR_CODE XcapApiAddHttpHeader(IN const XCAP_HANDLE hHttpMsg, IN const XCAP_CHAR *pcName,
    IN const XCAP_CHAR *pcValue);

XCAP_ERR_CODE XcapApiGetHttpBody(IN const XCAP_HANDLE hHttpMsg, OUT XCAP_UINT32 *pulBodyLen,
    OUT XCAP_UINT8 **ppucHttpBody);

/* [DTS2016060704445][max-alloc-limit][k71093][start] */
XCAP_ERR_CODE XcapApiSetMaxHttpBodySize(IN const XCAP_HANDLE hMsg, IN XCAP_UINT32 ulMaxBodySize);

/* [DTS2016060704445][max-alloc-limit][k71093][end] */
XCAP_ERR_CODE XcapApiSetHttpBody(IN const XCAP_HANDLE hHttpMsg, IN XCAP_UINT32 ulBodyLen,
    IN const XCAP_UINT8 *pucHttpBody);

XCAP_ERR_CODE XcapApiGetHttpRespCode(IN const XCAP_HANDLE hHttpMsg, OUT XCAP_UINT32 *pulCode,
    OUT XCAP_CHAR **ppcReasonPhrase);

XCAP_ERR_CODE XcapApiSetHttpRespCode(IN const XCAP_HANDLE hHttpMsg, IN XCAP_UINT32 ulCode,
    IN const XCAP_CHAR *pcReasonPhrase);

XCAP_ERR_CODE XcapApiSetHttpRequestTlsMode(IN XCAP_HANDLE hHttpMsg,
    IN XCAP_UINT32 ulTlsMode); /* TLS升级模式。EN_XCAP_HTTP_TLS_MODE */

XCAP_ERR_CODE XcapHttpApiSetRequestDestAddr(IN XCAP_HANDLE hHttpMsg, /* 用户[VPP-25322]的请求消息句柄 */
    IN const XCAP_CHAR *pcAddr,                                      /* 用户指定的和端口的字符串，[VPP-25322]
                                                                        可以为IPV4、IPV6、域名 */
    IN XCAP_BOOL bIsHttps);                                          /* 用户指定消息发送的模式是否为HTTPS */
/* [IR20191127007811][4-12-2019]  - Start */
XCAP_ERR_CODE XcapApiSetHttpMsgCleanse(IN const XCAP_HANDLE hHttpMsg, IN XCAP_BOOL bCleanseFlag);
XCAP_ERR_CODE XcapApiGetHttpMsgCleanse(IN const XCAP_HANDLE hHttpMsg, IN XCAP_BOOL *pbCleanseFlag);

/* [IR20191127007811][4-12-2019]  - End */
/* Used to get the User Static Information from stack. */
typedef struct _XcapHttpUserStatisticInfo {
    XCAP_UINT32 ulOnlineLink;  /* 统计--此线程在线的连接数,和COM模块保持一致 */
    XCAP_UINT32 ulOnlineTrans; /* 统计--在线使用的事务节点 */

    XCAP_UINT32 ulSendReq;  /* 统计--客户端:从上层传入的请求个数  */
    XCAP_UINT32 ulRecvResp; /* 统计--客户端:上报给上层的响应个数  */

    XCAP_UINT32 ulRecvReq;  /* 统计--服务端:上报给上层的请求个数  */
    XCAP_UINT32 ulSendResp; /* 统计--服务端:从上层传入的响应个数  */

    XCAP_UINT32 ulDiscardRequest; /* 统计--服务端:IP过滤掉的请求个数    */
#ifdef XCAP_64BIT_ALIGNED
    XCAP_UINT32 ulSpare; /* For 64 bit alignment */
#endif
} XcapHttpUserStatisticInfo;

/* 获取统计信息 */
XCAP_VOID XcapHttpApiGetStatistic(IN XCAP_HANDLE hStackHandle, /* 协议栈用户句柄     */
    INOUT XcapHttpUserStatisticInfo *pstStatistic);

/* 统计信息显示，包含内部的统计信息，使用注册的XcapSysDebugSend打印 */
XCAP_VOID XcapHttpApiDisplayStat(IN XCAP_HANDLE hStackHandle); /* 协议栈用户句柄     */

XCAP_VOID XcapHttpApiClearStat(IN XCAP_HANDLE hStackHandle); /* 协议栈用户句柄     */

/* HTTP暂不对外开放此接口 */
XCAP_ERR_CODE XcapApiEncodeHttpMsg(IN XCAP_HANDLE hHttpMsg, OUT XCAP_CHAR **ppMsg);

/* ****************************************************************************
  Function name         : XcapHttpApiSetSockOptPerRequest

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
XCAP_ERR_CODE XcapHttpApiSetSockOptPerRequest(IN XCAP_HANDLE hRequestMsg, IN const XcapSockDataS *pstSockData,
    IN const XCAP_VOID *pvAuxData);

XCAP_ERR_CODE XcapHttpApiGetRemAddrFrmReqHdl(IN XCAP_HANDLE hStackHandle, IN XCAP_HANDLE hReqHandle,
    OUT XcapIpAddr *pstRemoteAddr);

XCAP_ERR_CODE XcapHttpApiSetRequestSrcAddr(IN XCAP_HANDLE hStackHandle, /* Application stack handle */
    IN const XcapIpAddr *pstLocalIpAddr,                                /* Local Address */
    IN const XCAP_VOID *pvAuxData);                                     /* For extensibility */

XCAP_ERR_CODE XcapHttpApiSetAcceptQueueSize(IN XCAP_HANDLE hStackHandle, /* Application stack handle */
    IN XCAP_UINT32 ulMaxConnInQueue,                                     /* Max outstanding connections in server
                                                                            connection queue */
    IN const XCAP_VOID *pvAuxData);                                      /* For extensibility */

/* [XCAP-23|DTS-ID:DTS2012101506347][Start][EPoll] */
/* ****************************************************************************
 Function Name: XcapHttpApiSetInitParameter

 Description  : To set the initialisation parameters for XCAP_HTTP
 Input        : enInitParam
                pvValue
 Output       : None
 Return Value : XCAP_ERR_CODE

  History        :
    1.Date         : 2012-10-12
      Author       : d00902689
      Modification : Function Added
**************************************************************************** */
XCAP_ERR_CODE XcapHttpApiSetInitParameter(IN EN_XCAP_INIT_PARA enInitParam, /* Initialization parameter type */
    IN const XCAP_VOID *pvValue); /* Points to the value corresponding to EN_XCAP_INIT_PARA type. */

/* ****************************************************************************
 Function Name: XcapHttpApiGetInitParameter

 Description  : To set the initialisation parameters for XCAP_HTTP
 Input        : enInitParam
 Output       : pvValue
 Return Value : XCAP_ERR_CODE

  History        :
    1.Date         : 2012-10-12
      Author       : d00902689
      Modification : Function Added
**************************************************************************** */
XCAP_ERR_CODE XcapHttpApiGetInitParameter(IN EN_XCAP_INIT_PARA enInitParam, /* Initialization parameter type */
    OUT XCAP_VOID *pvValue); /* Points to the value corresponding to EN_XCAP_INIT_PARA type. */

/* [XCAP-23|DTS-ID:DTS2012101506347][End][EPoll] */
/* [DTS2013050204972][SOAP-65][n00742771][Start] */
XCAP_ERR_CODE XcapHttpApiSetThreadDelay(IN XCAP_HANDLE hStackHandle, /* XCAP instance gandle */
    IN EN_XCAP_CONFIG_THREAD_DELAY enConfigThDelayParam,             /* Thread delay Parameter type */
    IN XCAP_UINT32 ulDelay); /* Points to the value corresponding to enConfigThDelayPara type. */

/* [DTS2013050204972][SOAP-65][n00742771][End] */
/* [VPP-3693][SOAP-67][n00742771][Start] */
XCAP_ERR_CODE XcapHttpApiGetLocalAddrFrmReqHdl(IN XCAP_HANDLE hStackHandle, /* XCAP instance handle */
    IN XCAP_HANDLE hReqHandle,
    /* Accepted socket descriptor of server on which request message is
       received */
    OUT XCAP_UINT32 *pulServerSocketFd,
    /* Address of server on which request message is received. Memory
       allocation for this has to be done by the caller of the API */
    OUT XcapIpAddr *pstLocalAddr);

/* [VPP-3693][SOAP-67][n00742771][End] */
/* [VPP-4472][SOAP-68][d00902689][Start] */
XCAP_ERR_CODE XcapHttpApiRegConnectEventFunc(IN XCAP_HANDLE hStackHandle,
    IN const PTXcapHttpConnectEvent pfConnectEvent);

/* [VPP-4472][SOAP-68][d00902689][End] */
XCAP_ERR_CODE XcapHttpApiSetConfigParam(IN XCAP_HANDLE hStackHandle, /* XCAP_HTTP instance handle */
    IN EN_XCAP_HTTP_CONFIG_PARA enConfigParam,                       /* Configuration parameter type */
    IN const XCAP_VOID *pvValue); /* Points to the value corresponding to EN_XCAP_HTTP_CONFIG_PARA type. */

#ifndef XCAP_HERT_LIB_SIZE_REDUCE
/* [VPP-11827][d00902689][Start] */
XCAP_ERR_CODE XcapHttpApiRegStatisticFunc(IN XCAP_HANDLE hStackHandle, /* XCAP instance handle */
    IN const PTXcapStat pfStat);                                       /* Statistics callback */
/* [VPP-11827][d00902689][End] */
#endif

/* [VPP-12363][NE40-Perf-Req][Start] */
XCAP_ERR_CODE XcapHttpApiRegGetTickTaskInfoFunc(IN XCAP_HANDLE hStackHandle,
    IN const PTXcapHttpGetTickTaskInfo pfGetTickTaskInfo);

XCAP_ERR_CODE XcapHttpApiRegTaskRunFreeCpuFunc(IN XCAP_HANDLE hStackHandle, /* XCAP instance handle */
    IN const PTXcapHttpTaskRunFreeCpu pfTaskRunFreeCpu);                    /* Task free cpu callback */

/* [VPP-12363][NE40-Perf-Req][End] */
/* [VPP-22529][Del-Inst-Api-Invk-RunTime-Safe-Support][Start] */
XCAP_ERR_CODE XcapHttpApiRegStartTimerExFunc(IN XCAP_HANDLE hStackHandle,
    IN const PTXcapHttpStartRelTimerEx pfStartRelTimerEx);

XCAP_ERR_CODE XcapHttpApiRegDnsQueryExFunc(IN XCAP_HANDLE hStackHandle, IN const PTXcapHttpDnsQueryEx pfDnsQueryEx);

/* [VPP-22529][Del-Inst-Api-Invk-RunTime-Safe-Support][End] */
/* [VPP-25237][Set-Listen-Sock-Opt][Start] */
XCAP_ERR_CODE XcapHttpApiRegSetListenSockOptFunc(IN XCAP_HANDLE hStackHandle, IN const XCAP_VOID *pvAppData,
    IN const PTXcapHttpSetListenSockOpt pfSetListenSockOpt);

/* [VPP-25237][Set-Listen-Sock-Opt][End] */
/* [VPP-22529][Del-Inst-Api-Invk-RunTime-Safe-Support][Start] */
XCAP_ERR_CODE XcapHttpApiCreateAppEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_ULONG ulAppId,                                           /* 上层用户所标识的句柄 */
    IN const XcapHttpUserCallBackFuncEx *pstUserCallBackFunc,        /* 区分多个用户的回调函数 */
    IN const XcapHttpInitAppParam *pstInitAppParam, /* 设置该用户参数，包括最大连接数，发送、接收缓存 */
    OUT XCAP_HANDLE *phStackHandle);                /* 协议栈用户句柄 */

XCAP_ERR_CODE XcapHttpApiDeleteAppEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    INOUT XCAP_HANDLE *phStackHandle);                               /* 协议栈用户句柄 */

XCAP_ERR_CODE XcapHttpApiKickOffEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle);

XCAP_ERR_CODE XcapHttpApiSetSslConnectTimerValueEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle, IN XCAP_UINT32 ulMaxSslConnectTime, IN const XCAP_VOID *pvAuxData);

XCAP_ERR_CODE XcapHttpApiDoRequestEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle, IN XCAP_ULONG ulRequestId,          /* 用户的请求标识 */
    IN XCAP_HANDLE hRequestMsg, OUT XCAP_HANDLE *phReqHandle);       /* 协议栈的请求标识 */

XCAP_ERR_CODE XcapHttpApiDeleteConnectionEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle, INOUT const XcapIpAddr *pstRemoteAddr);

XCAP_ERR_CODE XcapHttpApiDoResponseEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle, IN XCAP_HANDLE hReqHandle,           /* 协议栈的请求标识 */
    IN XCAP_HANDLE hResponseMsg);

XCAP_ERR_CODE XcapHttpApiCloseTransactionEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle, IN XCAP_HANDLE hReqHandle);

XCAP_VOID XcapHttpApiGetStatisticEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle,                                    /* 协议栈用户句柄     */
    INOUT XcapHttpUserStatisticInfo *pstStatistic);

XCAP_VOID XcapHttpApiDisplayStatEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle);                                  /* 协议栈用户句柄   */

XCAP_VOID XcapHttpApiClearStatEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle);                                /* 协议栈用户句柄     */

XCAP_ERR_CODE XcapHttpApiCreateListenEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle,                                        /* 协议栈用户句柄 */
    IN XCAP_UINT32 ulTlsMode,                                           /* EN_XCAP_HTTP_TLS_MODE */
    IN const XcapIpAddr *pstLocalAddr);                                 /* 本端侦听地址 */

XCAP_ERR_CODE XcapHttpApiDeleteListenEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle,                                        /* 协议栈用户句柄 */
    IN const XcapIpAddr *pstLocalAddr);                                 /* 本端侦听地址 */

XCAP_ERR_CODE XcapHttpApiGetRemAddrFrmReqHdlEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle, IN XCAP_HANDLE hReqHandle, OUT XcapIpAddr *pstRemoteAddr);

XCAP_ERR_CODE XcapHttpApiSetRequestSrcAddrEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle,                                             /* Application stack handle */
    IN const XcapIpAddr *pstLocalIpAddr,                                     /* Local Address */
    IN const XCAP_VOID *pvAuxData);                                          /* For extensibility */

XCAP_ERR_CODE XcapHttpApiSetAcceptQueueSizeEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle,                                              /* Application stack handle */
    IN XCAP_UINT32 ulMaxConnInQueue,                                          /* Max outstanding connections in
                                                 server connection queue */
    IN const XCAP_VOID *pvAuxData);                                           /* For extensibility */

XCAP_ERR_CODE XcapHttpApiSetThreadDelayEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle,                                          /* XCAP instance gandle */
    IN EN_XCAP_CONFIG_THREAD_DELAY enConfigThDelayParam,                  /* Thread delay Parameter type */
    IN XCAP_UINT32 ulDelay); /* Points to the value corresponding to enConfigThDelayPara type. */

XCAP_ERR_CODE XcapHttpApiGetLocalAddrFrmReqHdlEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle, IN XCAP_HANDLE hReqHandle, OUT XCAP_UINT32 *pulServerSocketFd,
    OUT XcapIpAddr *pstLocalAddr);

XCAP_ERR_CODE XcapHttpApiRegConnectEventFuncEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle, IN const PTXcapHttpConnectEvent pfConnectEvent);

XCAP_ERR_CODE XcapHttpApiRegStatisticFuncEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle, IN const PTXcapStat pfStat);

XCAP_ERR_CODE XcapHttpApiRegGetTickTaskInfoFuncEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle, IN const PTXcapHttpGetTickTaskInfo pfGetTickTaskInfo);

XCAP_ERR_CODE XcapHttpApiRegTaskRunFreeCpuFuncEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle, IN const PTXcapHttpTaskRunFreeCpu pfTaskRunFreeCpu);

XCAP_ERR_CODE XcapHttpApiSetConfigParamEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle,                                          /* XCAP_HTTP instance handle */
    IN EN_XCAP_HTTP_CONFIG_PARA enConfigParam,                            /* Configuration parameter type */
    IN const XCAP_VOID *pvValue); /* Points to the value corresponding to EN_XCAP_HTTP_CONFIG_PARA type. */

/* [VPP-22529][Del-Inst-Api-Invk-RunTime-Safe-Support][End] */
/* [VPP-25237][Set-Listen-Sock-Opt][Start] */
XCAP_ERR_CODE XcapHttpApiRegSetListenSockOptFuncEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle, IN const XCAP_VOID *pvAppData, IN const PTXcapHttpSetListenSockOpt pfSetListenSockOpt);

/* [VPP-25237][Set-Listen-Sock-Opt][End] */
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* _XCAP_HTTP_API_H_ */
