/* *****************************************************************************

    Copyright (c) Huawei Technologies Co., Ltd. 2008-2019. All rights reserved.

    Description:   interface file in com layer.

    Function List: NA

    Author:  w57322,q57882

    Create:  2008-3-13

***************************************************************************** */
#ifndef XCAPCOMAPI_H
#define XCAPCOMAPI_H

#include "xcaptypes.h"
#include "xcapbase.h"
#include "xcapssp.h"
#include "xcaperror.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* State of Connection event */
typedef enum _EN_XCAP_CONNECT_EVENT {
    XCAP_CONNECT_EVENT_START = 0,
    XCAP_CONNECT_EVENT_ERROR,              /* 连接出现错误，将要关闭 */
    XCAP_CONNECT_EVENT_CLOSE,              /* 连接正常关闭 */
    XCAP_CONNECT_EVENT_CREATE_SUCCESS,     /* 连接[VPP-25322]成功(客户端) */
    XCAP_CONNECT_EVENT_CREATE_FAILURE,     /* 连接[VPP-25322]失败(客户端) */
    XCAP_CONNECT_EVENT_TLSUPGRADE_SUCCESS, /* TLS升级成功 */
    XCAP_CONNECT_EVENT_TLSUPGRADE_FAILURE, /* TLS升级失败 */
    XCAP_CONNECT_EVENT_END
} EN_XCAP_CONNECT_EVENT;

#define XCAP_COM_MAX_LISTEN_ADDR_NUM XCAP_BASE_MAX_LISTEN_ADDR_NUM
#define XCAP_COM_MAX_LINK_PER_THREAD 0xFFFF

/* Contains Listen Address related information */
typedef struct _XcapComListenIpAddr {
    XcapIpAddr stIpAddr;
    XCAP_BOOL bIsTLS; /* 是否为TLS */
#ifdef XCAP_64BIT_ALIGNED
    XCAP_UINT32 ulSpare; /* For 64 bit alignment */
#endif
} XcapComListenIpAddr;

/* Used to set the user parameters, including maximum number of connections
    and sending/receiving buffer. */
typedef struct _XcapComInitAppParam {
    XCAP_UINT16 usServiceMode;      /* 包括客户端、服务端、Both
                                    : EN_XCAP_SERVICE_MODE */
    XCAP_UINT16 usListenThreadMode; /* 单线程、多线程模式
                                              : EN_XCAP_LISTEN_THREAD_MODE */
    XCAP_UINT32 ulWorkThreadNum;    /* 配置工作线程数 */
    XCAP_UINT32 ulMaxLinkPerThread; /* 每个线程支持的最大连接数 */
    XCAP_UINT32 ulLinkSendBuff;     /* 配置用户下所有连接的发送缓存 */
    XCAP_UINT32 ulLinkRecvBuff;     /* 配置用户下所有连接的接收缓存 */
    XCAP_UINT32 ulListenNum;        /* 侦听个数 */
    XcapComListenIpAddr stListenIpAddr[XCAP_COM_MAX_LISTEN_ADDR_NUM];
} XcapComInitAppParam;

/* Linked list of options for storing socket option data */
typedef struct _XcapSockDataListS {
    XcapSockDataS stSockData;
    struct _XcapSockDataListS *pstNext;
} XcapSockDataListS;

/* 上报接收数据（回调） */
/* Not done const, since xcap-com does not maintain/need this pucData. It is for APP use only. */
typedef XCAP_ERR_CODE (*PTXcapComRecvDataIndCallback)(IN XCAP_HANDLE ulAppId, /* 上层用户所标识的句柄 */
    IN XCAP_HANDLE ulAppConnId,                                               /* 上层用户所标识的连接号 */
    IN XCAP_UINT32 ulDataLen,                                                 /* 接收数据的长度 */
    IN XCAP_UINT8 *pucData);                                                  /* 接收数据的指针 */

/* 连接事件上报（回调） */
typedef XCAP_ERR_CODE (*PTXcapComConnectEventIndCallback)(IN XCAP_HANDLE ulAppId, /* 上层用户所标识的句柄 */
    IN XCAP_HANDLE ulAppConnId,                                                   /* 上层用户所标识的连接号 */
    IN XCAP_UINT32 ulEvent,                                                       /* 连接事件 */
    IN XCAP_UINT32 ulParaLen,                                                     /* 保留参数数据长度 */
    IN XCAP_VOID *pvPara);                                                        /* 保留参数，用于后续扩展 */

/* 新连接上报（回调） */
typedef XCAP_ERR_CODE (*PTXcapComAcceptConnectCallback)(IN XCAP_HANDLE ulAppId, /* 上层用户所标识的句柄 */
    /* XCAP COM层为新连接分配的连接标识 */
    IN XCAP_HANDLE hLinkHandle, IN XCAP_UINT32 ulIsTLS,
    /* 是否为TLS连接 */
    IN const XcapIpAddr *pstLocalAddr,  /* 本段地址 */
    IN const XcapIpAddr *pstRemoteAddr, /* 对端地址 */
    /* 上层为COM新连接分配的连接标识 */
    OUT XCAP_HANDLE *pulAppConnId);

/* 定界函数，其返回值为定界出的长度，小于0表示定界失败，
    等于0表示定界成功数据不全，大于0表示定界后的长度 */
/* Not done const, since xcap-com does not maintain/need this pucStreamData. It is for APP use only. */
typedef XCAP_INT32 (*PTXcapComStreamConvertCallback)(IN XCAP_HANDLE ulAppId, /* 上层用户所标识的句柄 */
    IN XCAP_HANDLE ulAppConnId,                                              /* 上层用户所标识的连接号 */
    IN XCAP_UINT32 ulStreamLength,                                           /* 需要转换的流数据的长度 */
    IN XCAP_UINT8 *pucStreamData);                                           /* 流数据的内容的指针 */

/* 返回XCAP_TRUE，则该IP是合法的；返回XCAP_FALSE，则该IP是非法的 */
typedef XCAP_BOOL (*PTXcapComIpFilter)(IN XCAP_HANDLE ulAppId, /* 上层用户Id */
    IN const XcapIpAddr *pstIpAddr);                           /* [VPP-25322] */

/* 启动定时器 */
typedef XCAP_ERR_CODE (*PTXcapComStartRelTimer)(IN XCAP_UINT32 ulLength, IN XCAP_ULONG ulPara,
    OUT XCAP_HANDLE *phTimerHandle);

/* 停止定时器 */
typedef XCAP_VOID (*PTXcapComStopRelTimer)(IN const XCAP_HANDLE *phTimerHandle);

/* [VPP-22529][Del-Inst-Api-Invk-RunTime-Safe-Support][Start] */
typedef XCAP_ERR_CODE (*PTXcapComStartRelTimerEx)(IN XCAP_HANDLE hStackHandle, IN XCAP_UINT32 ulLength,
    IN XCAP_ULONG ulPara, OUT XCAP_HANDLE *phTimerHandle);

/* [VPP-22529][Del-Inst-Api-Invk-RunTime-Safe-Support][End] */
/* Callback function for differentiating multiple users, including message
    reporting and timer.  It is for XcapComApiCreateApp. */
typedef struct _XcapComUserCallBackFunc {
    PTXcapComRecvDataIndCallback pfXcapComRecvDataIndCallback;
    PTXcapComConnectEventIndCallback pfXcapComConnectEventIndCallback;
    PTXcapComAcceptConnectCallback pfXcapComAcceptConnectCallback;
    PTXcapComStreamConvertCallback pfXcapComStreamConvertCallback;
    PTXcapComIpFilter pfIpFilter; /* IP过滤接口，可为空 */
    PTXcapComStartRelTimer pfStartRelTimer;
    PTXcapComStopRelTimer pfStopRelTimer;
} XcapComUserCallBackFunc;

/* [VPP-22529][Del-Inst-Api-Invk-RunTime-Safe-Support][Start] */
/* Callback function for differentiating multiple users, including message
    reporting and timer. It is for XcapComApiCreateAppEx. */
typedef struct _XcapComUserCallBackFuncEx {
    PTXcapComRecvDataIndCallback pfXcapComRecvDataIndCallback;
    PTXcapComConnectEventIndCallback pfXcapComConnectEventIndCallback;
    PTXcapComAcceptConnectCallback pfXcapComAcceptConnectCallback;
    PTXcapComStreamConvertCallback pfXcapComStreamConvertCallback;
    PTXcapComIpFilter pfIpFilter; /* IP过滤接口，可为空 */
    PTXcapComStartRelTimerEx pfStartRelTimerEx;
    PTXcapComStopRelTimer pfStopRelTimer;
} XcapComUserCallBackFuncEx;

/* [VPP-22529][Del-Inst-Api-Invk-RunTime-Safe-Support][End] */
XCAP_ERR_CODE XcapComApiSetSslConnectTimerValue(IN XCAP_HANDLE hStackHandle, IN XCAP_UINT32 ulMaxSslConnectTime,
    IN const XCAP_VOID *pvAuxData);

/* [VPP-25322]用户，并设置与用户相关的参数 */
XCAP_ERR_CODE XcapComApiCreateApp(IN XCAP_ULONG ulAppId,   /* 上层用户所标识的句柄 */
    IN const XcapComUserCallBackFunc *pstUserCallBackFunc, /* 区分多个用户的回调函数 */
    IN const XcapComInitAppParam *pstInitAppParam,         /* 设置该用户参数，包括最大连接数，发送、接收缓存 */
    OUT XCAP_HANDLE *phStackHandle);                       /* 协议栈用户句柄 */

/* 删除用户 */
XCAP_ERR_CODE XcapComApiDeleteApp(INOUT XCAP_HANDLE *phStackHandle); /* 协议栈用户句柄 */

/* 建立连接 */
XCAP_ERR_CODE XcapComApiCreateConnect(IN XCAP_HANDLE hStackHandle, /* 协议栈用户句柄 */
    IN XCAP_HANDLE ulAppConnId,                                    /* 上层用户所标识的连接号 */
    IN XCAP_UINT32 ulIsTLS,                                        /* 是否为TLS连接 */
    IN const XcapIpAddr *pstLocalAddr,                             /* 本段地址 */
    IN const XcapIpAddr *pstRemoteAddr,                            /* 对端地址 */
    OUT XCAP_HANDLE *hLinkHandle);                                 /* XCAP COM层返回的连接标识 */

/* 删除连接 */
XCAP_ERR_CODE XcapComApiDeleteConnect(IN XCAP_HANDLE hStackHandle, /* 协议栈用户句柄 */
    IN XCAP_HANDLE hLinkHandle,                                    /* XCAP COM层连接标识 */
    IN XCAP_BOOL bIsGraceClose); /* 是否优雅关闭，1表示优雅关闭，0表示强制关闭(设置linger属性) */

/* 建立侦听 */
XCAP_ERR_CODE XcapComApiCreateListen(IN XCAP_HANDLE hStackHandle, /* 协议栈用户句柄 */
    IN XCAP_BOOL bIsTLS,                                          /* 是否为TLS连接 */
    IN const XcapIpAddr *pstLocalAddr);                           /* 本端地址 */

/* 删除侦听 */
XCAP_ERR_CODE XcapComApiDeleteListen(IN XCAP_HANDLE hStackHandle, /* 协议栈用户句柄 */
    IN const XcapIpAddr *pstLocalAddr);                           /* 本端地址 */

/* TLS升级 */
XCAP_ERR_CODE XcapComApiTLSUpgrade(IN XCAP_HANDLE hStackHandle, /* 协议栈用户句柄 */
    IN XCAP_HANDLE hLinkHandle);                                /* XCAP COM层连接标识 */

/* 发送数据，其返回值为发送的长度 */
XCAP_ERR_CODE XcapComApiSendData(IN XCAP_HANDLE hStackHandle, /* 协议栈用户句柄 */
    IN XCAP_HANDLE hLinkHandle,                               /* XCAP COM层连接标识 */
    IN XCAP_UINT32 ulLength,                                  /* 发送的消息的长度 */
    IN const XCAP_UINT8 *pucData);                            /* 发送的消息的内容的指针 */

/* [VPP-22529][Del-Inst-Api-Invk-RunTime-Safe-Support][Start] */
XCAP_ERR_CODE XcapComApiSetAcceptQueueSize(IN XCAP_HANDLE hStackHandle, /* Application stack handle */
    IN XCAP_UINT32 ulMaxConnInQueue,                                    /* Max outstanding connections in server
                                                                           connection queue */
    IN const XCAP_VOID *pvAuxData);                                     /* For extensibility */

/* [VPP-22529][Del-Inst-Api-Invk-RunTime-Safe-Support][End] */
/* COM用户启动 */
XCAP_ERR_CODE XcapComApiKickOff(IN XCAP_HANDLE hStackHandle);

XCAP_VOID XcapComApiTickTask(IN XCAP_HANDLE hStackHandle);

/* [XCAP-23|DTS-ID:DTS2012101506347][Start][EPoll] */
/* ****************************************************************************
 Function Name: XcapComApiSetInitParameter

 Description  : To set the initialisation parameters for XCAP-COM.
 Input        : enInitParam
                pvValue
 Output       : None
 Return Value : XCAP_ERR_CODE

  History        :
    1.Date         : 2012-10-12
      Author       : d00902689
      Modification : Function Added
**************************************************************************** */
XCAP_ERR_CODE XcapComApiSetInitParameter(IN EN_XCAP_INIT_PARA enInitParam, /* Initialization parameter type */
    IN const XCAP_VOID *pvValue); /* Points to the value corresponding to EN_XCAP_INIT_PARA type. */

/* ****************************************************************************
 Function Name: XcapComApiGetInitParameter

 Description  : To get the initialisation parameters for XCAP-COM.
 Input        : enInitParam
 Output       : pvValue
 Return Value : XCAP_ERR_CODE

  History        :
    1.Date         : 2012-10-12
      Author       : d00902689
      Modification : Function Added
**************************************************************************** */
XCAP_ERR_CODE XcapComApiGetInitParameter(IN EN_XCAP_INIT_PARA enInitParam, /* Initialization parameter type */
    OUT XCAP_VOID *pvValue); /* Points to the value corresponding to EN_XCAP_INIT_PARA type. */

/* [XCAP-23|DTS-ID:DTS2012101506347][End][EPoll] */
/* [DTS2013050204972][SOAP-65][n00742771][Start] */
XCAP_ERR_CODE XcapComApiSetThreadDelay(IN XCAP_HANDLE hStackHandle, /* XCAP instance gandle */
    IN EN_XCAP_CONFIG_THREAD_DELAY enConfigThDelayParam,            /* Thread delay Parameter type */
    IN XCAP_UINT32 ulDelay); /* Points to the value corresponding to enConfigThDelayPara type. */

/* [DTS2013050204972][SOAP-65][n00742771][End] */
/* [VPP-22529][Del-Inst-Api-Invk-RunTime-Safe-Support][Start] */
XCAP_ERR_CODE XcapComApiRegStartTimerExFunc(IN XCAP_HANDLE hStackHandle, IN PTXcapComStartRelTimerEx pfStartRelTimerEx);

/* [VPP-22529][Del-Inst-Api-Invk-RunTime-Safe-Support][End] */
/* [VPP-22529][Del-Inst-Api-Invk-RunTime-Safe-Support][Start] */
XCAP_ERR_CODE XcapComApiCreateAppEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_ULONG ulAppId, IN const XcapComUserCallBackFuncEx *pstUserCallBackFunc,
    IN const XcapComInitAppParam *pstInitAppParam, OUT XCAP_HANDLE *phStackHandle);

XCAP_ERR_CODE XcapComApiDeleteAppEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    INOUT XCAP_HANDLE *phStackHandle);

XCAP_ERR_CODE XcapComApiSetSslConnectTimerValueEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle, IN XCAP_UINT32 ulMaxSslConnectTime, IN const XCAP_VOID *pvAuxData);

XCAP_ERR_CODE XcapComApiCreateConnectEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle,                                        /* 协议栈用户句柄 */
    IN XCAP_HANDLE ulAppConnId,                                         /* 上层用户所标识的连接号 */
    IN XCAP_UINT32 ulIsTLS,                                             /* 是否为TLS连接 */
    IN const XcapIpAddr *pstLocalAddr,                                  /* 本端地址 */
    IN const XcapIpAddr *pstRemoteAddr,                                 /* 对端地址 */
    OUT XCAP_HANDLE *hLinkHandle);                                      /* XCAP COM层返回的连接标识 */

XCAP_ERR_CODE XcapComApiDeleteConnectEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle,                                        /* 协议栈用户句柄 */
    IN XCAP_HANDLE hLinkHandle,                                         /* XCAP COM层连接标识 */
    IN XCAP_BOOL bIsGraceClose); /* 是否优雅关闭，1表示优雅关闭，0表示强制关闭(设置linger属性) */

XCAP_ERR_CODE XcapComApiCreateListenEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle,                                       /* 协议栈用户句柄 */
    IN XCAP_BOOL bIsTLS,                                               /* 是否为TLS连接 */
    IN const XcapIpAddr *pstLocalAddr);                                /* 本端地址 */

XCAP_ERR_CODE XcapComApiDeleteListenEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle,                                       /* 协议栈用户句柄 */
    IN const XcapIpAddr *pstLocalAddr);                                /* 本端地址 */

XCAP_ERR_CODE XcapComApiTLSUpgradeEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle,                                     /* 协议栈用户句柄 */
    IN XCAP_HANDLE hLinkHandle);                                     /* XCAP COM层连接标识 */

XCAP_ERR_CODE XcapComApiSendDataEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle,                                   /* 协议栈用户句柄 */
    IN XCAP_HANDLE hLinkHandle,                                    /* XCAP COM层连接标识 */
    IN XCAP_UINT32 ulLength,                                       /* 发送的消息的长度 */
    IN const XCAP_UINT8 *pucData);                                 /* 发送的消息的内容的指针 */

XCAP_VOID XcapComApiTickTaskEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle);

XCAP_ERR_CODE XcapComApiKickOffEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle);

XCAP_ERR_CODE XcapComApiSetAcceptQueueSizeEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle,                                             /* Application stack handle */
    IN XCAP_UINT32 ulMaxConnInQueue,                                         /* Max outstanding connections in server
                                                                                connection queue */
    IN const XCAP_VOID *pvAuxData);                                          /* For extensibility */

XCAP_ERR_CODE XcapComApiSetThreadDelayEx(IN XCAP_HANDLE hXcapInitHandle, /* XCAP Inst Init Handle */
    IN XCAP_HANDLE hStackHandle,                                         /* XCAP instance gandle */
    IN EN_XCAP_CONFIG_THREAD_DELAY enConfigThDelayParam,                 /* Thread delay Parameter type */
    IN XCAP_UINT32 ulDelay); /* Points to the value corresponding to enConfigThDelayPara type. */

/* [VPP-22529][Del-Inst-Api-Invk-RunTime-Safe-Support][End] */
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* _XCAP_COM_API_H_ */
