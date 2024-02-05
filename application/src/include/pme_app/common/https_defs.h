/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved.
 * Description: HPPTS服务相关的定义。
 * Author: h00282621
 * Create: 2020-3-10
 * Notes: 用于跨模块使用的相关的定义。
 */


#ifndef __HTTPS_DEFINE_H__
#define __HTTPS_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


#define WEB_TYPE_HTTP 0
#define WEB_TYPE_HTTPS 1

#define VHOST_IP_LEN 256

#define VHOST_STOP 0
#define VHOST_START 1
#define VHOST_RESTART 2


#define VHOST_ENABLE 1
#define VHOST_DISABLE 0

#define VHOST_DEFAULT_OUTER_IP "_default_"
#define VHOST_DEFAULT_OUTER_PORT 443
#define VHOST_DEFAULT_INNER_PORT 40443
#define VHOST_DEFAULT_SMS_IPADDR "fe80::9e7d:a3ff:fe28:6ffa"
#define VHOST_DEFAULT_SMS_CDEV_IPADDR "0:0:0:0:0:0:0:1"
#define VHOST_DEFAULT_SMS_PORT 40443

#define HTTP_CONF_NAME "http_configure"
#define HTTPS_CONF_NAME "https_configure"
#define HTTPS_VERIFY_CLIENT_CONF_NAME "ssl_verify_client"
#define HTTP_CONF "/data/opt/pme/web/conf/nginx_http_server.conf"
#define HTTPS_CONF "/data/opt/pme/web/conf/nginx_https_server.conf"
#define HTTPS_VERIFY_CLIENT_CONF "/data/opt/pme/web/conf/nginx_ssl_verify_client.conf"
#define HTTPS_CIPHER_SUITE_CONF "/data/opt/pme/web/conf/nginx_ssl_ciphersuite.conf"
#define HTTPS_CIPHER_SUITE_CONF_SHORT_NAME "ssl_ciphersuite"

#define SERVER_BEGAIN "server {\r\n"

#define SERVER_END_HTTPS "include nginx_https_server_ext.conf;\r\n}"

#define SERVER_END_HTTP "server_name localhost;\r\n\
location = / {\r\n\
    rewrite ^(.*)$  https://$host:%d$1 permanent;\r\n\
}\r\n\
error_page 500 502 503 504  /50x.html;\r\n\
location = /50x.html {\r\n\
    root /opt/pme/web/htdocs/error_page;\r\n\
}\r\n\
error_page 404 /404.html;\r\n\
location = /404.html {\r\n\
    root /opt/pme/web/htdocs/error_page;\r\n\
}\r\n\
}"

#define VIRTUAL_HOST_EXT "ssl_verify_client %s;\r\n\
%s\r\n\
ssl_ocsp %s;\r\n\
%s\r\n"

#define VERIFY_ROOT_CERT "/data/opt/pme/conf/mutual_auth/rootcert.crt"
#define VERIFY_CRL_CERT "/data/opt/pme/conf/mutual_auth/rootcert.crl"

#define VIRTUAL_HOST_CRL_CONF "ssl_crl /data/opt/pme/conf/mutual_auth/rootcert.crl;"
#define VIRTUAL_HOST_CRT_CONF "ssl_crl /data/opt/pme/conf/mutual_auth/rootcert.crt;"
#define VIRTUAL_HOST_CA_CONF  "ssl_client_certificate /data/opt/pme/conf/mutual_auth/rootcert.crt;"

#define HTTPS_SERVER_CA_FILE_PATH_TEMPLATE "/data/opt/pme/conf/https_transfer/ca/rootcert%u.crt"
#define HTTPS_SERVER_CA_BACKFILE_PATH_TEMPLATE "rootcert%u.crt.bak"
#define HTTPS_SERVER_CRL_FILE_PATH_TEMPLATE "/data/opt/pme/conf/https_transfer/crl/rootcert%u.crl"
#define HTTPS_SERVER_CRL_BACKFILE_PATH_TEMPLATE "rootcert%u.crl.bak"
#define HTTPS_SERVER_ROOT_CA_ID_MAX 8
#define HTTPS_SERVER_ROOT_CA_ID_MIN 1
#define HTTPS_FILE_TRANSFER_SERVER_ROOTCA_ID_MAX 8
#define HTTPS_FILE_TRANSFER_SERVER_ROOTCA_ID_MIN 5

#define RF_EVT_SUBSCRIBER_ROOTCA_COUNT 4
#define RF_EVT_SUBSCRIBER_ROOTCA_ID_MIN 1
#define RF_EVT_SUBSCRIBER_ROOTCA_ID_MAX 4
#define CERT_ID_AUTO_SEARCH_FLAG 0
#define CERT_USAGE_EVT_SUBSCRIPTION 0
#define CERT_USAGE_FILE_TRANSFER 1

#define MAX_CURL_REQ_URI_LEN        MAX_EVT_DESTINATION_LEN  /* 长度需和最大事件订阅监听地址长度一致 */
#define MAX_CURL_HEADERS_SIZE       MAX_EVT_HEADERS_SIZE
typedef struct tag_curl_req_info {
    /* 请求方法名，取值为HTTP_PATCH之类的常量字符串宏 */
    const gchar*          method_name;

    /* 请求URI */
    gchar                 req_uri[MAX_CURL_REQ_URI_LEN];

    /* 请求body，内存上层分配 */
    gchar*          req_body;

    /* 请求头链表 */
    struct curl_slist*    request_headers;
} CURL_REQ_INFO;

typedef struct tag_curl_response_info {
    /* HTTP响应状态码 */
    long                   response_code;

    /* HTTP响应头对象，key:value形式 */
    struct json_object*    response_header_jso;

    /* HTTP响应体，内存需由上层释放 */
    gchar*                 response_body;
} CURL_RESPONSE_INFO;

typedef struct tag_curl_option_info {
    /* 进行消息通信的本地IPv4/IPv6 */
    const gchar* src_ip;
    /* 是否校验对端ssl证书 */
    gboolean verify_peer_ssl;
    /* 可使用的服务器根证书ID的集合 */
    guint8* certs;
    /* 可使用的服务器根证书数量,跟certs数组元素需保持一致 */
    guint8 certs_cnt;
} CURL_OPT_INFO;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HTTPS_DEFINE_H__ */
