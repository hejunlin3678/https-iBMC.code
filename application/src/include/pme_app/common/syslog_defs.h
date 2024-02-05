/* 
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved. 
* Description: syslog管理相关的定义。
* Author: h00282621 
* Create: 2020-3-10 
* Notes: 用于跨模块使用的相关的定义。
*/

#ifndef __SYSLOG_DEFINE_H__
#define __SYSLOG_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define SYSLOG_ROOT_CERT_ID         0
#define SYSLOG_CLIENT_CERT_ID       1

/*网络传输协议类型:UDP:1 TCP(no TLS):2 TCP(TLS):3*/
#define SYSLOG_MGNT_NET_PTL_UDP     0x01
#define SYSLOG_MGNT_NET_PTL_TCP     0x02
#define SYSLOG_MGNT_NET_PTL_TLS     0x03

#define SYSLOG_MGNT_AUTH_TYPE_ONE   0x01
#define SYSLOG_MGNT_AUTH_TYPE_TWO   0x02

#define SYSLOG_CERT_TYPE_HOST    1
#define SYSLOG_CERT_TYPE_CLIENT  2


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __SYSLOG_DEFINE_H__ */
