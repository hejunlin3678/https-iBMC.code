/******************************************************************************

                  版权所有 (C), 2008-2009, 华为技术有限公司

 ******************************************************************************
  文 件 名   : maint_debug_msg.h
  版 本 号   : 初稿
  部    门   :
  作    者   : licheng
  生成日期   : 2013年8月14日
  最近修改   :
  功能描述   : maint debug内部消息通信
  函数列表   :
  修改历史   :
  1.日    期   : 2013年8月14日
    作    者   : licheng
    修改内容   : 创建文件

******************************************************************************/


#ifndef __MD_MSG_H__
#define __MD_MSG_H__

#ifdef __cplusplus
 #if __cplusplus
    extern "C" {
 #endif
#endif /* __cplusplus */

#include <glib.h>

/*因为客户端使用pid做unix socket的地址文件名，
服务端使用pid取值范围(0~32767)范围以外的值避免冲突。---DTS2018061302019 */
#define MAINT_DEBUG_MOD_PORT    33333
#define INVALID_SOCKFD  (-1)
#define INVALID_PORT    (-1)
#define MSG_SZ_MAX      1024
#define PARA_NUM_MAX    10
#define RESULT_NUM_MAX  30

#ifdef UNIT_TEST
    #define UN_SOCK_PATH    "/tmp/lc_tuhao"
#else
    #define UN_SOCK_PATH    "/opt/pme/pram/maint_debug"
#endif


#pragma pack(1)
typedef struct md_req_s
{
    guint32 seq;
    guint32 para_num;
    gchar *func_name;
    gchar **para_array;
} MD_REQ_S;

typedef struct md_rsp_s
{
    guint32 seq;
    gint32 ret_val;
    guint32 result_num;
    gchar **result_array;
} MD_RSP_S;
#pragma pack(0)


extern void set_msg_printfunc(void (*print_func)(const gchar *fmt, ...));
extern void close_socket(int sockfd);
extern gint32 listen_socket(int lsn_sockfd, int backlog);
extern gint32 accept_socket(int lsn_sockfd);
extern gint32 connect_to_MD_cli(int *sockfd, int port);
extern gint32 connect_to_MD_mod(int *sync_sockfd);
extern void deconnect(int sockfd);
extern gint32 init_listen_sock(int *lsn_sockfd, int *lsn_port);
extern void deinit_listen_sock(int lsn_sockfd);

extern gint32 recv_req(int sockfd, MD_REQ_S *req);
extern gint32 send_rsp(int sockfd, MD_RSP_S *rsp);
extern void free_req_data(MD_REQ_S *req);
extern void free_rsp_data(MD_RSP_S *rsp);
extern gint32 send_req_sync(int sockfd, MD_REQ_S *req, MD_RSP_S *rsp);

extern gint32 send_async_msg(int async_sockfd, gchar *msg, gint msg_len);
extern gint32 recv_async_msg(int async_sockfd, gchar *msg, guint *msg_len);

#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* __cplusplus */

#endif


