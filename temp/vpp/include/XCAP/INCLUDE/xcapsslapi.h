/* *****************************************************************************

    Copyright (c) Huawei Technologies Co., Ltd. 2008-2019. All rights reserved.

    Description:   ssl adapt file.

    Function List: NA

    Author:  L36241,W64990

    Create:  2008-5-22

***************************************************************************** */
#ifndef XCAPSSLAPI_H
#define XCAPSSLAPI_H

#include "xcapbase.h"
#include "xcaptypes.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define XCAP_SSL_SUCCESS 0
#define XCAP_SSL_ERROR (-1)
#define XCAP_SSL_INPROGRESS (-2)
#define XCAP_SSL_PEER_CLOSE (-3)

/* [VPP-12363][NE40-Perf-Req][Start] */
#define XCAP_SSL_ERROR_WANT_READ (-4)
#define XCAP_SSL_ERROR_WANT_WRITE (-5)

/* [VPP-12363][NE40-Perf-Req][End] */
#define XCAP_SSL_VOID void
#define XCAP_SSL_CTX void
#define XCAP_SSL void
#define XCAP_SSL_NULL 0L

typedef signed int XCAP_SSL_INT32;

#define XCAP_SSL_TRUE 1
#define XCAP_SSL_FALSE 0

typedef XCAP_SSL_VOID (*PTXcapSslInit)(XCAP_VOID);

typedef XCAP_SSL_VOID (*PTXcapSslFini)(XCAP_VOID);

typedef XCAP_SSL_CTX *(*PTXcapSslCreatClientCtx)(IN const XcapIpAddr *pstPeerIpAddr);

typedef XCAP_SSL_CTX *(*PTXcapSslCreatServerCtx)(IN const XcapIpAddr *pstLocalIpAddr);

typedef XCAP_SSL_VOID (*PTXcapSslCtxFree)(IN XCAP_SSL_CTX *pstSSLCtx);

typedef XCAP_SSL *(*PTXcapSslNew)(IN XCAP_SSL_CTX *pstSSLCtx);

typedef XCAP_SSL_INT32 (*PTXcapSslSetfd)(IN XCAP_SSL *pstSsl, IN XCAP_SSL_INT32 iSock);

typedef XCAP_SSL_VOID (*PTXcapSslFree)(IN XCAP_SSL *pstSsl);

typedef XCAP_SSL_INT32 (*PTXcapSslAccept)(IN XCAP_SSL *pstSsl);

typedef XCAP_SSL_INT32 (*PTXcapSslConnect)(IN XCAP_SSL *pstSsl);

typedef XCAP_SSL_INT32 (*PTXcapSslRead)(IN XCAP_SSL *pstSsl, OUT XCAP_SSL_VOID *pcBuf, IN XCAP_SSL_INT32 iBufLen);

typedef XCAP_SSL_INT32 (*PTXcapSslWrite)(IN XCAP_SSL *pstSsl, IN const XCAP_SSL_VOID *pcBuf, IN XCAP_SSL_INT32 iBufLen);

typedef XCAP_SSL_INT32 (*PTXcapSslPending)(IN const XCAP_SSL *pstSsl);

typedef XCAP_SSL_INT32 (*PTXcapSslShutdown)(IN XCAP_SSL *pstSsl);

/* [VPP-12363][NE40-Perf-Req][Start] */
/* Returns 0 on success, other values on error */
/* pstSsl : SSL context */
/* pbIsInitFinished :XCAP_TRUE if handshake over or renegotiation is over, else XCAP_FALSE */
typedef XCAP_SSL_INT32 (*PTXcapSslIsInitFinished)(IN XCAP_SSL *pstSsl, OUT XCAP_BOOL *pbIsInitFinished);

/* [VPP-12363][NE40-Perf-Req][End] */
/* Contains pointers to SSL CallBack Functions */
typedef struct _XcapSslCallBackFunc {
    PTXcapSslInit pfSslInit;
    PTXcapSslFini pfSslFini;
    PTXcapSslCreatClientCtx pfSslCreatClientCtx;
    PTXcapSslCreatServerCtx pfSslCreatServerCtx;
    PTXcapSslCtxFree pfSslCtxFree;
    PTXcapSslNew pfSslNew;
    PTXcapSslSetfd pfSslSetfd;
    PTXcapSslFree pfSslFree;
    PTXcapSslAccept pfSslAccept;
    PTXcapSslConnect pfSslConnect;
    PTXcapSslRead pfSslRead;
    PTXcapSslWrite pfSslWrite;
    PTXcapSslPending pfSslPending;
    PTXcapSslShutdown pfSslShutdown;
} XcapSslCallBackFunc;

/* pvAuxData为用户自定义数据，它在XcapApiSetRequestSrcAddrEx输入，被透传并在此回调中作为入参输入 */
typedef XCAP_SSL_CTX *(*PTXcapSslCreatAndConfigClientCtx)(
    IN const XcapIpAddr *pstPeerIpAddr, IN const XCAP_VOID *pvAuxData);

/* 注册SSL的回调函数 */
XCAPPUBFUN XCAP_ERR_CODE XcapApiRegSSLFunc(IN const XcapSslCallBackFunc *pstSslCallBackFunc);

/* [VPP-12363][NE40-Perf-Req][Start] */
XCAPPUBFUN XCAP_VOID XcapApiRegSSLIsInitFinishedFunc(IN const PTXcapSslIsInitFinished pfSslIsInitFinished);

/* [VPP-12363][NE40-Perf-Req][End] */

XCAPPUBFUN XCAP_ERR_CODE XcapApiRegSslCreatAndConfigClientCtxFunc(IN const PTXcapSslCreatAndConfigClientCtx
        pstCallBackFunc);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
