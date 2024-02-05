/* *****************************************************************************

    Copyright (c) Huawei Technologies Co., Ltd. 2007-2019. All rights reserved.

    Description:   system and socket, epoll and so one callback reg.

    Function List: NA

    Author:  NA

    Create:  2007-10-10

***************************************************************************** */
#ifndef XCAPSSP_H
#define XCAPSSP_H

#ifdef XCAP_SSP_USER_REG_SYS_FUNC
#undef XCAP_SSP_USER_REG_SYS_FUNC
#endif

#if defined(XCAP_WIN32) || defined(XCAP_WIN64)
#ifndef FD_SETSIZE
#define FD_SETSIZE 1000
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#include "xcapbase.h"

/* [VPP-7521][d00902689][Start] */
/* [VPP-21246][Start] */
/* [DTS2016052305822][c84019193][23-05-2016] */
#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib")
#endif

#ifdef EWOULDBLOCK
#undef EWOULDBLOCK
#endif

#define EWOULDBLOCK WSAEWOULDBLOCK

#ifdef EINPROGRESS
#undef EINPROGRESS
#endif

#define EINPROGRESS WSAEINPROGRESS

#define XCAP_SSP_USER_REG_SYS_FUNC

#ifdef XCAP_VRP
#undef FD_SETSIZE
#undef NFDBITS
#undef FDSETLEN
#define FD_SETSIZE (23 * 1024)
#define NFDBITS 0x20
#define FDSETLEN ((FD_SETSIZE / NFDBITS) + 1)
#undef EAGAIN
#undef EWOULDBLOCK
#undef EINPROGRESS
#define EAGAIN (-35)
#define EWOULDBLOCK EAGAIN
#define EINPROGRESS (-36)
#endif

#endif

#if defined(XCAP_LINUX) || defined(XCAP_MAC_IOS)

#ifndef XCAP_MAC_IOS
/* [DTS2012120309177] [d00902689] [Start] */
#ifndef XCAP_VRP

/* [DTS2012091103211][K71093][START] */
#ifndef XCAP_VISP
#ifndef FD_SETSIZE
#define FD_SETSIZE 1000
#endif
#endif

/* [DTS2012091103211][K71093][END] */
#endif
#endif

/* [DTS2012120309177] [d00902689] [Start] */
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <errno.h>
#include "xcapbase.h"

/* [DTS2012120309177] [d00902689] [Start] */
#ifdef XCAP_VRP
#undef FD_SETSIZE
#undef NFDBITS
#undef FDSETLEN
#define FD_SETSIZE (23 * 1024)
#define NFDBITS 0x20
#define FDSETLEN ((FD_SETSIZE / NFDBITS) + 1)
#undef EAGAIN
#undef EWOULDBLOCK
#undef EINPROGRESS
#define EAGAIN (-35)
#define EWOULDBLOCK EAGAIN
#define EINPROGRESS (-36)
#endif

/* [DTS2012120309177] [d00902689] [End] */
/* [DTS2012091103211][K71093][START] */
#ifdef XCAP_VISP
#undef FD_SETSIZE
#undef NFDBITS
#undef FDSETLEN

#define FD_SETSIZE 3072
#define NFDBITS 0X20
#define FDSETLEN 97

#define XCAP_VISP_EAGAIN (-35)
#define XCAP_VISP_EWOULDBLOCK XCAP_VISP_EAGAIN
#define XCAP_VISP_EINPROGRESS (-36)
#endif

/* [DTS2012091103211][K71093][END] */
#define XCAP_SSP_USER_REG_SYS_FUNC

#ifdef XCAP_SOLARIS
#include <sys/filio.h>
#include <sys/sockio.h>
#endif

#endif

/* [XCAP-23|DTS-ID:DTS2012101506347][Start][EPoll] */
#if defined(XCAP_LINUX) && !defined(XCAP_SOLARIS) && !defined(XCAP_VISP) && !defined(XCAP_VRP)

#include <sys/epoll.h>
typedef struct epoll_event XcapEpollEvent;
#else

/* Dummy EpollEvent */
#include "xcaptypes.h"
typedef XCAP_VOID XcapEpollEvent;

#endif

/* [XCAP-23|DTS-ID:DTS2012101506347][End][EPoll] */
#ifdef XCAP_VXWORKS

#ifndef XCAP_VRP
#ifndef XCAP_VISP
#ifndef FD_SETSIZE
#define FD_SETSIZE 1000
#endif
#endif
#endif

#include <sockLib.h>
#include <inetLib.h>
#include "xcapbase.h"

#ifdef XCAP_VXWORKS_MIPS
#include <ioLib.h>
#endif
#ifdef XCAP_VXWORKS55_PPC8540
#include <ioLib.h>
#endif
#ifdef XCAP_VXWORKS55_PPC85XX_VRP
#include <ioLib.h>
#endif
#ifdef XCAP_VXWORKS55_ARM_BE
#include <ioLib.h>
#endif

/* [VPP-7521][d00902689][End] */
/* [VPP-21246][End] */
#ifdef XCAP_VRP
/* [VPP-21246][Start] */
#undef FD_SETSIZE
#undef NFDBITS
#undef FDSETLEN

/* [VPP-21246][End] */
#define FD_SETSIZE (23 * 1024)
#define NFDBITS 0x20
#define FDSETLEN (FD_SETSIZE / NFDBITS)
#endif

#ifdef XCAP_VISP

/* [VPP-21246][Start] */
#undef FD_SETSIZE
#undef NFDBITS
#undef FDSETLEN

/* [VPP-21246][End] */
#define FD_SETSIZE 3072
#define NFDBITS 0X20
#define FDSETLEN 97

#define XCAP_VISP_EAGAIN (-35)
#define XCAP_VISP_EWOULDBLOCK XCAP_VISP_EAGAIN
#define XCAP_VISP_EINPROGRESS (-36)
#endif

#define XCAP_SSP_USER_REG_SYS_FUNC
#endif

/* 用户注册系统函数接口定义，默认使用操作系统接口 */
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#ifdef XCAP_SSP_USER_REG_SYS_FUNC

/* 内存操作函数原型 */
typedef XCAP_VOID *(*PTXcapOsMemAlloc)(IN XCAP_UINT32 ulSize);

typedef XCAP_VOID (*PTXcapOsMemFree)(IN XCAP_VOID *pvMem);

typedef XCAP_VOID *(*PTXcapOsMemChr)(IN const XCAP_VOID *pvBuf, IN XCAP_INT32 c, IN XCAP_ULONG ulCount);

/* 字符串操作函数原型 */
typedef XCAP_UINT32 (*PTXcapOsStrLen)(IN const XCAP_CHAR *pcStr);

typedef XCAP_INT32 (*PTXcapOsStrCmp)(IN const XCAP_CHAR *pcStr1, IN const XCAP_CHAR *pcStr2);

typedef XCAP_INT32 (*PTXcapOsStrICmp)(IN const XCAP_CHAR *pcStr1, IN const XCAP_CHAR *pcStr2);

typedef XCAP_CHAR *(*PTXcapOsStrChr)(IN const XCAP_CHAR *pcStr, IN XCAP_INT32 c);

typedef XCAP_CHAR *(*PTXcapOsStrStr)(IN const XCAP_CHAR *pcStr, IN const XCAP_CHAR *pcStrSearch);

typedef XCAP_INT32 (*PTXcapOsAtoi)(IN const XCAP_CHAR *pcStr);

typedef XCAP_VOID (*PTXcapOsSleep)(IN XCAP_UINT32 ulMilliseconds);

/* 线程相关函数原型 */
/* Not done const, since OS and DOPRA thread entry function prototypes do not have const argument. */
typedef XCAP_VOID (*PTXcapThreadProc)(IN XCAP_VOID *pvPara);

/* 成功返回: XCAP_SUCCESS
   错误返回: XCAP_FAILURE */
typedef XCAP_ERR_CODE (*PTXcapOsCreateThread)(IN const PTXcapThreadProc pfThreadProc,
    IN XCAP_VOID *pvPara, /* Not done const, since OS and DOPRA create
                              task APIs do not take const argument. */
    OUT XCAP_HANDLE *phThread);

typedef XCAP_VOID (*PTXcapOsBlockWaitThreadExit)(INOUT XCAP_HANDLE hThread);

/* 必须为递归锁, 即在同一线程中可锁多次而不会导致死锁
   成功返回: XCAP_SUCCESS
   错误返回: XCAP_FAILURE */
typedef XCAP_ERR_CODE (*PTXcapOsCreateLock)(OUT XCAP_HANDLE *phMutex);

typedef XCAP_VOID (*PTXcapOsDestroyLock)(IN XCAP_HANDLE hMutex);

typedef XCAP_VOID (*PTXcapOsLock)(IN XCAP_HANDLE hMutex);

typedef XCAP_VOID (*PTXcapOsUnlock)(IN XCAP_HANDLE hMutex);

/* Socket函数原型 */
/* Socket函数返回值定义 */
#define XCAP_SOCK_INVALID_SOCKET (XCAP_SOCKET)(~0)
#define XCAP_SOCK_SOCKET_ERROR (-1)

#ifdef XCAP_VISP

#define XCAP_AF_INET 2

#define XCAP_AF_INET6 29

#define XCAP_SOCK_STREAM 1

#define XCAP_SOMAXCONN 0X5

#define XCAP_SOCK_CMD_FIONBIO 0X8004667E

/* [DTS2012120309177] [d00902689] [Start] */
/* [VPP-21246][Start] */
#elif defined(XCAP_VRP) && (defined(XCAP_LINUX) || defined(XCAP_WIN32) || \
    defined(XCAP_WIN64) || defined(XCAP_VXWORKS))

/* [VPP-21246][End] */
#define XCAP_AF_INET 2
#define XCAP_AF_INET6 29
#define XCAP_SOCK_STREAM 1
#define XCAP_SOMAXCONN 0X5
#define XCAP_SOCK_CMD_FIONBIO 0X8004667E

/* [DTS2012120309177] [d00902689] [End] */
/* [VPP-13804][Defect-Fix][Start] */
#undef in_addr
#define in_addr vrp_in_addr

#undef sockaddr_in
#define sockaddr_in vrp_sockaddr_in

#define XCAP_VRP_SOCKADDR_SIN_ZERO_SIZE 8

typedef struct vrp_in_addr {
    /* [VPP-28366]: Added XCAP_64BIT_BE compile macro for 64 bit Big Endian
    platforms to fix the incorrect handling of 32 Bit IP address resolution.
    Issue: s_addr was defined as unsigned long [unsigned long is 8 Bytes in
    64 Bit, while it is 4 bytes in 32 Bit Env].
    PDT manipulates s_addr as 32 Bit data (accesses first four bytes
    of eight byte data) and get incorrect results in 64Bit BE Platform.
    If any new PDT platform get this issue, compile the lib using the new
    build macro XCAP_64BIT_BE. */
#ifdef XCAP_64BIT_BE
    unsigned int s_addr; /* Address of the socket */
#else
    unsigned long s_addr; /* Address of the socket */
#endif
} vrp_in_addr;

typedef struct vrp_sockaddr_in {
    unsigned char sin_len;                          /* zy */
    unsigned char sin_family;                       /* must be AF_INET */
    unsigned short sin_port;                        /* 16-bit port number */
    struct vrp_in_addr sin_addr;                    /* 32-bit Address */
    char sin_zero[XCAP_VRP_SOCKADDR_SIN_ZERO_SIZE]; /* must be 0 */
} vrp_sockaddr_in;

/* [VPP-13804][Defect-Fix][End] */
#else

/* af */
#define XCAP_AF_INET AF_INET
#define XCAP_AF_INET6 AF_INET6

/* type */
#define XCAP_SOCK_STREAM SOCK_STREAM

#define XCAP_SOMAXCONN SOMAXCONN

/* cmd */
#define XCAP_SOCK_CMD_FIONBIO FIONBIO

#endif

/* protocol */
#define XCAP_SOCK_PROTOCOL 0

/* 成功返回: SOCKET
   错误返回: XCAP_SOCK_INVALID_SOCKET */
typedef XCAP_SOCKET (*PTXcapOsSocket)(IN XCAP_INT32 af, IN XCAP_INT32 type, IN XCAP_INT32 protocol);

/* 成功返回: XCAP_SUCCESS
   错误返回: XCAP_SOCK_SOCKET_ERROR */
typedef XCAP_ERR_CODE (*PTXcapOsBind)(IN XCAP_SOCKET sockfd, IN struct sockaddr *addr, /* Not done const since xcap
                                                        supports visp and vrp socket and it does not take const. Also
                                                                                          adapter may need modify addr
                                                        for vrp and visp socket. */
    IN XCAP_INT32 addrlen);

/* 成功返回: XCAP_SUCCESS
   错误返回: XCAP_SOCK_SOCKET_ERROR */
typedef XCAP_ERR_CODE (*PTXcapOsListen)(IN XCAP_SOCKET sockfd, IN XCAP_INT32 backlog);

/* 成功返回: SOCKET
   错误返回: XCAP_SOCK_INVALID_SOCKET */
typedef XCAP_SOCKET (*PTXcapOsAccept)(IN XCAP_SOCKET sockfd, OUT struct sockaddr *addr, INOUT XCAP_INT32 *addrlen);

/* 成功返回: XCAP_SUCCESS
   错误返回: XCAP_SOCK_SOCKET_ERROR */
typedef XCAP_ERR_CODE (*PTXcapOsConnect)(IN XCAP_SOCKET sockfd, IN struct sockaddr *addr, /* Not done const since xcap
                                                        supports visp and vrp socket and it does not take const. Also
                                                                                             adapter may need modify
                                                        addr for vrp and visp socket. */
    IN XCAP_INT32 addrlen);

/* 成功返回: XCAP_SUCCESS
   错误返回: XCAP_SOCK_SOCKET_ERROR */
typedef XCAP_ERR_CODE (*PTXcapOsCloseSocket)(IN XCAP_SOCKET sockfd);

/* 成功返回: XCAP_SUCCESS
   错误返回: XCAP_SOCK_SOCKET_ERROR */
/* parg: Not done const since xcap supports visp and vrp socket and it does not take const.
    Also adapter may need modify parg for vrp and visp socket. */
typedef XCAP_ERR_CODE (*PTXcapOsIoctlSocket)(IN XCAP_SOCKET sockfd, IN XCAP_INT32 cmd, IN XCAP_INT32 *parg);

/* flags */
#define XCAP_SOCK_DEFT_FLAGS 0

/* 成功返回: 已发送字节数
   错误返回: XCAP_SOCK_SOCKET_ERROR */
typedef XCAP_INT32 (*PTXcapOsSend)(IN XCAP_SOCKET sockfd, IN const XCAP_CHAR *buf, IN XCAP_INT32 len,
    IN XCAP_INT32 flags);

/* 成功返回: 已接收字节数
   错误返回: XCAP_SOCK_SOCKET_ERROR */
typedef XCAP_INT32 (*PTXcapOsRecv)(IN XCAP_SOCKET sockfd, OUT XCAP_CHAR *buf, IN XCAP_INT32 len, IN XCAP_INT32 flags);

#ifdef XCAP_VISP

#define XCAP_SOCK_LEVEL_SOL_SOCKET 0XFFFF

#define XCAP_SOCK_OPT_SO_REUSEADDR 0X00000004

#define XCAP_SOCK_OPT_SO_LINGER 0X00000080

#define XCAP_SOCK_OPT_SO_SNDBUF 0X1001

#define XCAP_SOCK_OPT_SO_RCVBUF 0X1002

/* [VPP-4472][SOAP-68][d00902689][Start] */
#define XCAP_SOCK_OPT_SO_ERROR 0x00001007

/* [VPP-4472][SOAP-68][d00902689][End] */
#undef fd_set
#define fd_set vrp_fd_set

/* NOTE : Re-definition of VISP fd_set structure

typedef struct vrp_fd_set
{
    LONG fds_bits[VRP_FDSETLEN]
}vrp_fd_set

 */
typedef struct vrp_fd_set {
    signed long fds_bits[FDSETLEN];
} vrp_fd_set;

/* [DTS2012120309177] [d00902689] [Start] */
/* [VPP-21246][Start] */
#elif defined(XCAP_VRP) && (defined(XCAP_LINUX) || defined(XCAP_WIN32) || \
    defined(XCAP_WIN64) || defined(XCAP_VXWORKS))

/* [VPP-21246][End] */
#define XCAP_SOCK_LEVEL_SOL_SOCKET 0XFFFF
#define XCAP_SOCK_OPT_SO_REUSEADDR 0X00000004
#define XCAP_SOCK_OPT_SO_LINGER 0X00000080
#define XCAP_SOCK_OPT_SO_SNDBUF 0x00001001
#define XCAP_SOCK_OPT_SO_RCVBUF 0x00001002

/* [VPP-4472][SOAP-68][d00902689][Start] */
#define XCAP_SOCK_OPT_SO_ERROR 0x00001007

/* [VPP-4472][SOAP-68][d00902689][End] */
#undef fd_set
#define fd_set vrp_fd_set

/* define typedef vrp_fd_set */
typedef struct vrp_fd_set {
    signed long fds_bits[FDSETLEN];
} vrp_fd_set;

/* [DTS2012120309177] [d00902689] [End] */
#else

/* level */
#define XCAP_SOCK_LEVEL_SOL_SOCKET SOL_SOCKET

/* optname */
#define XCAP_SOCK_OPT_SO_REUSEADDR SO_REUSEADDR
#define XCAP_SOCK_OPT_SO_LINGER SO_LINGER
#define XCAP_SOCK_OPT_SO_SNDBUF SO_SNDBUF
#define XCAP_SOCK_OPT_SO_RCVBUF SO_RCVBUF

/* [VPP-4472][SOAP-68][d00902689][Start] */
#define XCAP_SOCK_OPT_SO_ERROR SO_ERROR

/* [VPP-4472][SOAP-68][d00902689][End] */
#endif

/* 成功返回: XCAP_SUCCESS
   错误返回: XCAP_SOCK_SOCKET_ERROR */
typedef XCAP_ERR_CODE (*PTXcapOsSetSockOpt)(IN XCAP_SOCKET sockfd, IN XCAP_INT32 level, IN XCAP_INT32 optname,
    IN XCAP_CHAR *optval, /* Not done const since xcap supports visp and vrp
socket and it does not take const. Also
                             adapter may need modify optval for vrp and visp
                             socket. */
    IN XCAP_INT32 optlen);

/* [VPP-4472][SOAP-68][d00902689][Start] */
typedef XCAP_ERR_CODE (*PTXcapOsGetSockName)(IN XCAP_SOCKET sockfd, OUT struct sockaddr *addr,
    INOUT XCAP_INT32 *addrlen);

typedef XCAP_ERR_CODE (*PTXcapOsGetSockOpt)(IN XCAP_SOCKET sockfd, IN XCAP_INT32 level, IN XCAP_INT32 optname,
    OUT XCAP_CHAR *optval, INOUT XCAP_INT32 *optlen);

/* [VPP-4472][SOAP-68][d00902689][End] */
/* 成功返回: 已准备好的SOCKET个数, 如超时则返回0(与操作系统select接口同)
   错误返回: XCAP_SOCK_SOCKET_ERROR */
typedef XCAP_INT32 (*PTXcapOsSelect)(IN XCAP_INT32 nfds, INOUT fd_set *readfds, INOUT fd_set *writefds,
    INOUT fd_set *exceptfds, INOUT struct timeval *timeout);

typedef XCAP_INT32 (*PTXcapOsGetErrorNo)(XCAP_VOID);

/* [XCAP-23|DTS-ID:DTS2012101506347][Start][EPoll] */
/* ****************************************************************************
 Callback Name: PTXcapOsEpollCreate

 Description  : Creates epoll instance
 Input        : size - This size is just a hint to the kernel about how to
                       dimension internal structures.
 Output       : NA
 Return Value : Non-negative file descriptor (epoll instance) on SUCCESS,
                otherwise XCAP_FAILURE
**************************************************************************** */
typedef XCAP_INT32 (*PTXcapOsEpollCreate)(IN XCAP_INT32 size);

/* ****************************************************************************
 Callback Name: PTXcapOsEpollCtl

 Description  : Performs control operations over epoll instance
 Input        : epfd  - Epoll file descriptor
                op    - Operation to be performed on the target file descriptor
                fd    - Target file descriptor
                event - Pointer to the object linked to the target file
                        descriptor. epoll_event structure is defined in
                        <sys/epoll.h>
 Output       : NA
 Return Value : Returns XCAP_SUCCESS when successful, otherwise XCAP_FAILURE
**************************************************************************** */
typedef XCAP_ERR_CODE (*PTXcapOsEpollCtl)(IN XCAP_INT32 epfd, IN XCAP_INT32 op, IN XCAP_INT32 fd,
    IN XcapEpollEvent *event); /* Not made const to keep it same as linux epoll_ctl */

/* ****************************************************************************
 Callback Name: PTXcapOsEpollWait

 Description  : Waits for events on the epoll instances referred to by the epoll
                file descriptor.
 Input        : epfd       - Epoll file descriptor
                maxevents  - Maximum number of events
                timeout    - Maximum call wait time
 Output       : events     - Pointer to the memory area whih contain the events
                             that will be available for the caller.
 Return Value : When successful, returns the number of file descriptors ready
                for the requested I/O , or zero if no file descriptor became
                ready during the requested timeout milliseconds. Otherwise
                returns XCAP_FAILURE.
**************************************************************************** */
typedef XCAP_INT32 (*PTXcapOsEpollWait)(IN XCAP_INT32 epfd, OUT XcapEpollEvent *events, IN XCAP_INT32 maxevents,
    IN XCAP_INT32 timeout);

/* ****************************************************************************
 Callback Name: PTXcapOsEpollClose

 Description  : Closes the file descriptor
 Input        : fd - socket fd to be closed
 Output       : NA
 Return Value : XCAP_SUCCESS on success, Otherwise returns XCAP_FAILURE.
**************************************************************************** */
typedef XCAP_ERR_CODE (*PTXcapOsEpollClose)(IN XCAP_INT32 fd);

/* ****************************************************************************
 Callback Name: PTXcapOsEpollGetErrorNo

 Description  : To get the last error number
 Input        : NA
 Output       : NA
 Return Value : Error number
**************************************************************************** */
typedef XCAP_INT32 (*PTXcapOsEpollGetErrorNo)(XCAP_VOID);

/* [XCAP-23|DTS-ID:DTS2012101506347][End][EPoll] */
/* [VPP-12363][NE40-Perf-Req][Start] */
/* ****************************************************************************
 Callback Name: PTXcapOsEventRead

 Description  : Read events received by a task
 Input        : ulEvents - Specifies event IDs to be read, each bit signifies a
                           particular event EN_XCAP_EVENT_TYPE
                ulFlags  - Specifies the operational flags for the event
                           EN_XCAP_EVENT_FLAGS
                ulTimeOutInMillSec - Specifies the timeout in milli seconds,
                           till the read should wait
 Output       : pulRetEvents - Specifies events read by the function
                           EN_XCAP_EVENT_TYPE
 Return Value : When successful, returns XCAP_SUCCESS
                XCAP_ERR_READ_EVENT_TIMEOUT If the read event has timed out.
                Any other value indicates failure.
**************************************************************************** */
typedef XCAP_UINT32 (*PTXcapOsEventRead)(IN XCAP_ULONG ulEvents, IN XCAP_ULONG ulFlags,
    IN XCAP_ULONG ulTimeOutInMillSec, OUT XCAP_ULONG *pulRetEvents);

/* ****************************************************************************
 Callback Name: PTXcapOsEventWrite

 Description  : Write event to a task, so that the task can be informed about
                the event and it can read it.
 Input        : ulTaskID - Specifies the task ID to which the event is to
                           be written.
                ulEvents  - Specifies event IDs to be written. Each bit
                           signifies a particular event EN_XCAP_EVENT_TYPE
 Output       : None
 Return Value : When successful, returns XCAP_SUCCESS
                Any other value indicates failure.
**************************************************************************** */
typedef XCAP_UINT32 (*PTXcapOsEventWrite)(IN XCAP_ULONG ulTaskID, IN XCAP_ULONG ulEvents);

/* ****************************************************************************
 Callback Name: PTXcapOsQueueCreate

 Description  : Create a queue
 Input        : chQueName - Queue name to create, 4 bytes
                XCAP_NULL_PTR to create anonymous queue
                ulQueDepth: Queue length, 0 means to use default length
                ulFlags  - XCAP_Q_PRIOR or XCAP_Q_FIFO
                           XCAP_Q_ASYN or XCAP_Q_SYN
 Output       : pulRetQueID - Queue ID
 Return Value : When successful, returns XCAP_SUCCESS.
                Any other value indicates failure.
**************************************************************************** */
typedef XCAP_UINT32 (*PTXcapOsQueueCreate)(IN const XCAP_CHAR chQueName[4], IN XCAP_ULONG ulQueDepth,
    IN XCAP_ULONG ulFlags, OUT XCAP_ULONG *pulRetQueID);

/* ****************************************************************************
 Callback Name: PTXcapOsQueueDelete

 Description  : Delete a queue
 Input        : ulQueID - Queue ID to delete
 Output       : None
 Return Value : When successful, returns XCAP_SUCCESS.
                Any other value indicates failure.
**************************************************************************** */
typedef XCAP_UINT32 (*PTXcapOsQueueDelete)(IN XCAP_ULONG ulQueID);

/* ****************************************************************************
 Callback Name: PTXcapOsQueueRead

 Description  : Read a msg from queue head
 Input        : ulQueID - Queue ID to read
                ulFlags - XCAP_Q_WAIT or XCAP_Q_NO_WAIT, useful for SYNC Queue
                ulTimeOutInMillSec -  Time to wait for, useful for SYNC queue
                0 means don't wait.
 Output       : ulRetMsg - Pointer to store msg
 Return Value : When successful, returns XCAP_SUCCESS.
                XCAP_ERR_QUEUE_OP_TIMEOUT If the read queue has timed out.
                Any other value indicates failure.
**************************************************************************** */
typedef XCAP_UINT32 (*PTXcapOsQueueRead)(IN XCAP_ULONG ulQueID, OUT XCAP_ULONG ulRetMsg[4], IN XCAP_ULONG ulFlags,
    IN XCAP_ULONG ulTimeOutInMillSec);

/* ****************************************************************************
 Callback Name: PTXcapOsQueueWrite

 Description  : Write a msg to queue
 Input        : ulQueID - Queue ID to write
                ulMsg - Msg to write
                ulFlags - XCAP_Q_WAIT or XCAP_Q_NO_WAIT, useful for SYNC Queue
                ulTimeOutInMillSec -  Time to wait for, useful for SYNC queue
                0 means don't wait.
 Output       : None
 Return Value : When successful, returns XCAP_SUCCESS.
                XCAP_ERR_QUEUE_OP_TIMEOUT If the write queue has timed out.
                Any other value indicates failure.
**************************************************************************** */
typedef XCAP_UINT32 (*PTXcapOsQueueWrite)(IN XCAP_ULONG ulQueID, IN XCAP_ULONG ulMsg[4], /* Not made const, since values
                                                                                            at this address may need
                                                                                            updation in some cases. */
    IN XCAP_ULONG ulFlags, IN XCAP_ULONG ulTimeOutInMillSec);

/* [VPP-12363][NE40-Perf-Req][End] */
/* [VPP-22529][Del-Inst-Api-Invk-RunTime-Safe-Support][Start] */
/* ****************************************************************************
 Callback Name: PTXcapOsCreateRwLock

 Description  : This callback function creates a read-write lock, to be used
                for managing synchronization of read-write access to shared
                data.
 Input        : None
 Output       : ppvRwLock - Specifies read-write lock
 Return Value : When successful, returns XCAP_SUCCESS
                Any other value indicates failure.
**************************************************************************** */
typedef XCAP_UINT32 (*PTXcapOsCreateRwLock)(OUT XCAP_VOID **ppvRwLock);

/* ****************************************************************************
 Callback Name: PTXcapOsWriteLock

 Description  : Acquire the write lock.
 Input        : pvRwLock - Specifies read-write lock
                ulTimeOutInMillSec - Specifies the longest time
                (in unit of millisecond) that the caller is willing to wait.
                0 indicates wait forever until acquire lock is success.
 Output       : None
 Return Value : When successful, returns XCAP_SUCCESS
                XCAP_ERR_LOCK_OP_TIMEOUT If the acquire lock has timed out.
                Any other value indicates failure.
**************************************************************************** */
typedef XCAP_UINT32 (*PTXcapOsWriteLock)(IN XCAP_VOID *pvRwLock, IN XCAP_UINT32 ulTimeOutInMillSec);

/* ****************************************************************************
 Callback Name: PTXcapOsReadLock

 Description  : Acquire the read lock.
 Input        : pvRwLock - Specifies read-write lock
                ulTimeOutInMillSec - Specifies the longest time
                (in unit of millisecond) that the caller is willing to wait.
                0 indicates wait forever until acquire lock is success.
 Output       : None
 Return Value : When successful, returns XCAP_SUCCESS
                XCAP_ERR_LOCK_OP_TIMEOUT If the acquire lock has timed out.
                Any other value indicates failure.
**************************************************************************** */
typedef XCAP_UINT32 (*PTXcapOsReadLock)(IN XCAP_VOID *pvRwLock, IN XCAP_UINT32 ulTimeOutInMillSec);

/* ****************************************************************************
 Callback Name: PTXcapOsUnLockRwLock

 Description  : Unlocks the read lock/write lock.
 Input        : pvRwLock - Specifies read-write lock
 Output       : None
 Return Value : When successful, returns XCAP_SUCCESS
                Any other value indicates failure.
**************************************************************************** */
typedef XCAP_UINT32 (*PTXcapOsUnLockRwLock)(IN XCAP_VOID *pvRwLock);

/* ****************************************************************************
 Callback Name: PTXcapOsDestroyRwLock
 Description  : This callback function destroys the read-write lock.
 Input        : pvRwLock
 Output       : None
 Return Value : None
**************************************************************************** */
typedef XCAP_VOID (*PTXcapOsDestroyRwLock)(IN XCAP_VOID *pvRwLock);

/* [VPP-22529][Del-Inst-Api-Invk-RunTime-Safe-Support][End] */
/* 系统基本回调函数结构 */
typedef struct _XcapSysBaseCallBackFunc {
    PTXcapOsMemAlloc pfMemAlloc;
    PTXcapOsMemFree pfMemFree;
    PTXcapOsMemChr pfMemChr;
    PTXcapOsStrLen pfStrLen;
    PTXcapOsStrCmp pfStrCmp;
    PTXcapOsStrICmp pfStrICmp;
    PTXcapOsStrChr pfStrChr;
    PTXcapOsStrStr pfStrStr;
    PTXcapOsAtoi pfAtoi;
    PTXcapOsSleep pfSleep;
} XcapSysBaseCallBackFunc;

/* 系统线程回调函数结构 */
typedef struct _XcapSysThreadCallBackFunc {
    PTXcapOsCreateThread pfCreateThread;
    PTXcapOsBlockWaitThreadExit pfBlockWaitThreadExit;
    PTXcapOsCreateLock pfCreateLock;
    PTXcapOsDestroyLock pfDestroyLock;
    PTXcapOsLock pfLock;
    PTXcapOsUnlock pfUnlock;
} XcapSysThreadCallBackFunc;

/* 系统套接字回调函数结构 */
typedef struct _XcapSysSocketCallBackFunc {
    PTXcapOsSocket pfSocket;
    PTXcapOsBind pfBind;
    PTXcapOsListen pfListen;
    PTXcapOsAccept pfAccept;
    PTXcapOsConnect pfConnect;
    PTXcapOsCloseSocket pfCloseSocket;
    PTXcapOsIoctlSocket pfIoctlSocket;
    PTXcapOsSend pfSend;
    PTXcapOsRecv pfRecv;
    PTXcapOsSetSockOpt pfSetSockOpt;
    PTXcapOsSelect pfSelect;
    PTXcapOsGetErrorNo pfGetErrorNo;
} XcapSysSocketCallBackFunc;

/* [XCAP-23|DTS-ID:DTS2012101506347][Start][EPoll] */
/* Data structure for Epoll */
typedef struct _XcapSysEpollCallBackFunc {
    PTXcapOsEpollCreate pfEpollCreate;         /* Creates epoll instance */
    PTXcapOsEpollCtl pfEpollCtl;               /* Performs control
                          operations over epoll
        instance */
    PTXcapOsEpollWait pfEpollWait;             /* Waits for the events on
                              epoll instance */
    PTXcapOsEpollClose pfEpollClose;           /* Closes epoll instance */
    PTXcapOsEpollGetErrorNo pfEpollGetErrorNo; /* Gets errno */
} XcapSysEpollCallBackFunc;

/* [XCAP-23|DTS-ID:DTS2012101506347][End][EPoll] */
/* [VPP-12363][NE40-Perf-Req][Start] */
/* Data structure for Events callback */
typedef struct _XcapSysEventCallBackFunc {
    PTXcapOsEventRead pfEventRead;   /* Callback function to read events */
    PTXcapOsEventWrite pfEventWrite; /* Callback function to write events */
} XcapSysEventCallBackFunc;

/* Data structure for Queue callback */
typedef struct _XcapQueueFuncS {
    PTXcapOsQueueCreate pfQueueCreate; /* Callback function to create queue */
    PTXcapOsQueueDelete pfQueueDelete; /* Callback function to delete queue */
    PTXcapOsQueueRead pfQueueRead;     /* Callback function to read from queue */
    PTXcapOsQueueWrite pfQueueWrite;   /* Callback function to write to queue */
} XcapSysQueueCallBackFunc;

/* [VPP-12363][NE40-Perf-Req][End] */
/* [VPP-22529][Del-Inst-Api-Invk-RunTime-Safe-Support][Start] */
/* Data structure for Read/Write lock callback */
typedef struct _XcapSysRdWrLockCallBackFunc {
    PTXcapOsCreateRwLock pfCreateRwLock;
    PTXcapOsWriteLock pfWriteLock;
    PTXcapOsReadLock pfReadLock;
    PTXcapOsUnLockRwLock pfUnlockRwlock;
    PTXcapOsDestroyRwLock pfDestroyRwLock;
} XcapSysRdWrLockCallBackFunc;

/* [VPP-22529][Del-Inst-Api-Invk-RunTime-Safe-Support][End] */
/* [XCAP_CODEC_SUPPORT][VPP-26285][Start] */
/* xcap codec SSP callback structure */
typedef struct _XcapCodecSsp {
    PTXcapOsMemAlloc pfMemAlloc;
    PTXcapOsMemFree pfMemFree;
    PTXcapOsMemChr pfMemChr;
    PTXcapOsStrLen pfStrLen;
    PTXcapOsStrCmp pfStrCmp;
    PTXcapOsStrICmp pfStrICmp;
    PTXcapOsStrChr pfStrChr;
    PTXcapOsStrStr pfStrStr;
    PTXcapOsAtoi pfAtoi;
} XcapCodecSsp;

/* xcap codec SMP callback structure */
typedef struct _XcapCodecSmp {
    XCAP_UINT32 ulDebugType; /* XCAP_DBG_PRINT: 0X01, XCAP_DBG_INFO: 0X02,
                                   XCAP_DBG_ERROR: 0X04 , XCAP_DBG_LOG: 0X08 */
#ifdef XCAP_64BIT_ALIGNED
    XCAP_UINT32 ulSpare; /* For 64 bit alignment */
#endif

    PTXcapCodecSysDebugSend pfDebugFunc;
} XcapCodecSmp;

/* xcap codec context configuration structure */
typedef struct _XcapCodecCfg {
    XCAP_ULONG ulAppId; /* App ID */
    XcapCodecSsp stSsp; /* Specifies SSP configuration required by context */
    XcapCodecSmp stSmp; /* Specifies SMP configuration required by context */
} XcapCodecCfg;

/* [XCAP_CODEC_SUPPORT][VPP-26285][End] */
/*
    如果用户不调用以下的注册接口，那么协议栈默认使用操作系统函数。
    用户可以对全部或者部分的系统函数进行注册，协议栈将使用用户提供
    的系统回调函数。
    在调用以下的注册接口时，如果参数中某个函数为NULL，那么协议栈将
    使用对应的默认操作系统函数。这样用户可以仅对感兴趣的系统函数进
    行注册。
*/
/* 注册系统基本函数接口 */
XCAPPUBFUN XCAP_ERR_CODE XcapApiRegSysBaseFunc(IN const XcapSysBaseCallBackFunc *pstSysBaseFunc);

/* 注册系统线程函数接口 */
XCAPPUBFUN XCAP_ERR_CODE XcapApiRegSysThreadFunc(IN const XcapSysThreadCallBackFunc *pstSysThreadFunc);

/* 注册系统套接字函数接口 */
XCAPPUBFUN XCAP_ERR_CODE XcapApiRegSysSocketFunc(IN const XcapSysSocketCallBackFunc *pstSysSocketFunc);

/* Client bind registeration function */
XCAPPUBFUN XCAP_VOID XcapApiRegClientBindFunc(IN const PTXcapOsBind pfClientBind);

/* [XCAP-23|DTS-ID:DTS2012101506347][Start][EPoll] */
/* ****************************************************************************
 Function Name: XcapApiRegSysEpollFunc

 Description  : Registering Epoll callback funcions to the stack.
 Input        : pstSysEpollFunc - pointer to the XcapSysEpollCallBackFunc
                                  instance.
 Output       : None
 Return Value : Returns XCAP_SUCCESS when successful, otherwise
                XCAP_ERR_SMP_PARA_INVALID
**************************************************************************** */
XCAPPUBFUN XCAP_ERR_CODE XcapApiRegSysEpollFunc(IN const XcapSysEpollCallBackFunc *pstSysEpollFunc);

/* [XCAP-23|DTS-ID:DTS2012101506347][End][EPoll] */
/* [VPP-4472][SOAP-68][d00902689][Start] */
/* ****************************************************************************
 Function Name: XcapApiRegSysExtraSockFunc

 Description  : Registering Socket related extra informations callback funcion
                to the stack.
 Input        : pfGetSockOptFunc - function pointer.
                pfGetSockName - function pointer.
 Output       : None
 Return Value : XCAP_VOID

  History        :
    1.Date         : 2012-10-12
      Author       : d00902689
      Modification : Function Added
**************************************************************************** */
XCAPPUBFUN XCAP_VOID XcapApiRegSysExtraSockFunc(IN const PTXcapOsGetSockOpt pfGetSockOptFunc,
    IN const PTXcapOsGetSockName pfGetSockName);

/* [VPP-4472][SOAP-68][d00902689][End] */
/* [VPP-12363][NE40-Perf-Req][Start] */
/* ****************************************************************************
 Function Name: XcapApiRegSysEventFunc

 Description  : This  interface is used to register the event callback
                functions.
 Input        : pstEventFuncs - Pointer to event callback structure
 Output       : None
 Return Value : XCAP_ERR_CODE
**************************************************************************** */
XCAPPUBFUN XCAP_ERR_CODE XcapApiRegSysEventFunc(IN const XcapSysEventCallBackFunc *pstSysEventFunc);

/* ****************************************************************************
 Function Name: XcapApiRegSysQueueFunc

 Description  : This  interface is used to register the queue callback
                functions.
 Input        : XcapSysQueueCallBackFunc - Pointer to queue callback structure
 Output       : None
 Return Value : XCAP_ERR_CODE
**************************************************************************** */
XCAPPUBFUN XCAP_ERR_CODE XcapApiRegSysQueueFunc(IN const XcapSysQueueCallBackFunc *pstXcapQueFns);

/* [VPP-12363][NE40-Perf-Req][End] */
/* [VPP-22529][Del-Inst-Api-Invk-RunTime-Safe-Support][Start] */
/* ****************************************************************************
 Function Name: XcapApiRegSysRdWrLockFunc

 Description  : This  interface is used to register the read/write lock
                callback functions.
 Input        : XcapSysRdWrLockCallBackFunc - Pointer to queue callback
                structure
 Output       : None
 Return Value : XCAP_ERR_CODE
**************************************************************************** */
XCAPPUBFUN XCAP_ERR_CODE XcapApiRegSysRdWrLockFunc(IN const XcapSysRdWrLockCallBackFunc *pstSysRdWrLockFunc);

/* [VPP-22529][Del-Inst-Api-Invk-RunTime-Safe-Support][End] */
/* [XCAP_CODEC_SUPPORT][VPP-26285][Start] */
/* New Codec APIs - Start */
/* ****************************************************************************
 Function Name: XcapCodecApiCfgStructInit
 Description  : API to initialize input callback pointers to NULL.

                Before invoking XcapCodecApiCreateCtx api, APP must invoke
                XcapCodecApiCfgStructInit api to init callbacks from
                pstCfg para to NULL pointer, to ensure no impact if in future
                XcapCodecCfg is updated to add new optional callback.

 Input        : pstCfg - configuration structure for
                  SSP (callback for system functions like malloc, free etc) and
                  SMP (callback for maintenance log)
                  All SSP callbacks are mandatory
                  SMP callback is optional.

 Output       : pstCfg - configuration structure.  SSP and SMP callbak pointers
                will be initialized to NULL pointer.

 Return Value :
               XCAP_SUCCESS:  On success
               XCAP_ERR_CODE
**************************************************************************** */
XCAPPUBFUN XCAP_ERR_CODE XcapCodecApiCfgStructInit(INOUT XcapCodecCfg *pstCfg);

/* ****************************************************************************
 Function Name: XcapCodecApiCreateCtx
 Description  : API to create codec context.

                This codec context need to be passed as input to other codec
                apis whose name starts with XcapCodecApi.

                Usage of pvCodecCtx is not thread safe, hence must be used
                from single thread, at a time.

                Context created by this api can be destroyed using
                XcapCodecApiDestroyCtx api.

 Input        : pstCfg - configuration structure for
                  SSP (callback for system functions like malloc, free etc) and
                  SMP (callback for maintenance log)
                  All SSP callbacks are mandatory.
                  SMP callback is optional.
                  Before invoking this api, APP must invoke
                  XcapCodecApiCfgStructInit api to init callbacks from
                  pstCfg para to NULL pointer, to ensure no impact if in future
                  XcapCodecCfg is updated to add new optional callbacks.

 Output       : ppvCodecCtx - codec context.

 Return Value :
               XCAP_SUCCESS:  On success
               XCAP_ERR_CODE
**************************************************************************** */
XCAPPUBFUN XCAP_ERR_CODE XcapCodecApiCreateCtx(IN const XcapCodecCfg *pstCfg, OUT XCAP_VOID **ppvCodecCtx);

/* ****************************************************************************
 Function Name: XcapCodecApiDestroyCtx
 Description  : API to destroy codec context created using
                XcapCodecApiCreateCtx api. This api frees all memory resources
                related to codec context.

 Input        : ppvCodecCtx - codec context.
 Output       : ppvCodecCtx - after successful destroying codec context,
                    ppvCodecCtx will be set to NULL.

 Return Value :
               XCAP_SUCCESS:  On success
               XCAP_ERR_CODE
**************************************************************************** */
XCAPPUBFUN XCAP_ERR_CODE XcapCodecApiDestroyCtx(INOUT XCAP_VOID **ppvCodecCtx);

/* [XCAP_CODEC_SUPPORT][VPP-26285][End] */
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* _XCAP_SSP_H */
