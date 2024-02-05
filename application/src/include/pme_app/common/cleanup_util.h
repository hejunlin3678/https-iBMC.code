/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: 资源自动回收相关的公共接口
 * Author: x00454924
 * Notes: 定义新的资源回收宏时,注意将相同开源组件的资源回收宏放在一起,不要混杂,方便后续重构时，按照组件粒度来拆分头文件
 * History: 2022-05-07 x00454924 创建文件
 */
#ifndef CLEANUP_UTIL_H
#define CLEANUP_UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <openssl/x509v3.h>
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/pkcs12.h>
#include <curl/curl.h>
#include "glib.h"
#include "json/json.h"
#include "libxml/tree.h"
#include "pme/file_manage/file_manage.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef void (*stack_of_x509_node_free)(X509 *);

// 将cleanup拓展的声明格式封装成宏,便于代码中使用
#define _cleanup_(x) __attribute__((__cleanup__(x)))

// 用来定义cleanup拓展使用的资源释放函数,由func定义funcp
#define DEFINE_POINTER_CLEANUP_FUNCTION(type, func) \
    static __inline__ void func##p(type **ptr)      \
    {                                               \
        if (ptr == NULL) {                          \
            return;                                 \
        }                                           \
        if (*ptr) {                                 \
            func(*ptr);                             \
        }                                           \
    }

// 用来定义使用指定的函数来释放节点资源的释放函数
#define DEFINE_POINTER_CLEANUP_WITH_SPECIFIC_FREE_FUNC(type, func, destory_func, destory_func_type)     \
    static __inline__ void func##_by_##destory_func(type **ptr)                                         \
    {                                                                                                   \
        if (ptr == NULL) {                                                                              \
            return;                                                                                     \
        }                                                                                               \
        if (*ptr) {                                                                                     \
            func(*ptr, (destory_func_type)destory_func);                                                \
        }                                                                                               \
    }

// 用于内存资源的生命周期转移,调用TAKE_PTR之后,返回ptr指向地址,并且将ptr指针置成NULL
#define TAKE_PTR(ptr)                     \
    ({                                    \
        __typeof__(ptr) *_pptr_ = &(ptr); \
        __typeof__(ptr)_ptr_ = *_pptr_;   \
        *_pptr_ = NULL;                   \
        _ptr_;                            \
    })

// 用于文件描述符的生命周期转移,调用TAKE_FD之后,返回fd的值,并且将fd置成无效值-1
#define TAKE_FD(fd)        \
    ({                     \
        int *_fd_ = &(fd); \
        int _ret_ = *_fd_; \
        *_fd_ = -1;        \
        _ret_;             \
    })


/* libc */
static inline void closep(void *p)
{
    if (p == NULL) {
        return;
    }

    int *fd = (int *)p;
    if (*fd >= 0) {
        close(*fd);
        *fd = -1;
    }
}

/*
 * Notes：形参实际类型为void**,但是定义为void*类型是为了解决编译器告警,只有void*类型才能接收各种类型的指针参数而不引起编译器告警
 */
static inline void freep(void *p)
{
    void **pp = (void **)p;
    if (pp == NULL) {
        return;
    }

    free(*pp);
}

/*
 * Description: 释放包含敏感信息的字符串
 */
static inline void free_sensitive_str_pointer(gchar **sensitive_str)
{
    if (sensitive_str == NULL || *sensitive_str == NULL) {
        return;
    }

    gsize len = strlen(*sensitive_str);
    (void)memset_s(*sensitive_str, len, 0, len);
    g_free(*sensitive_str);
}

DEFINE_POINTER_CLEANUP_FUNCTION(FILE, fclose)          // 定义fclosep
DEFINE_POINTER_CLEANUP_FUNCTION(FM_FILE_S, fm_fclose)  // 定义fm_fclosep
DEFINE_POINTER_CLEANUP_FUNCTION(DIR, closedir)         // 定义closedirp

#define _cleanup_free_ _cleanup_(freep)
#define _cleanup_sensitive_str_free_ _cleanup_(free_sensitive_str_pointer)

#define _cleanup_fclose_ _cleanup_(fclosep)
#define _cleanup_fm_fclose_ _cleanup_(fm_fclosep)
#define _cleanup_closedir_ _cleanup_(closedirp)
#define _cleanup_close_ _cleanup_(closep)

/* glib */
DEFINE_POINTER_CLEANUP_FUNCTION(GSList, g_slist_free)      // 定义g_slist_freep
DEFINE_POINTER_CLEANUP_FUNCTION(gchar *, g_strfreev)       // 定义g_strfreevp
DEFINE_POINTER_CLEANUP_FUNCTION(GVariant, g_variant_unref) // 定义g_variant_unrefp
// 定义g_slist_free_full_by_g_variant_unref
DEFINE_POINTER_CLEANUP_WITH_SPECIFIC_FREE_FUNC(GSList, g_slist_free_full, g_variant_unref, GDestroyNotify)
// 定义g_slist_free_full_by_g_free
DEFINE_POINTER_CLEANUP_WITH_SPECIFIC_FREE_FUNC(GSList, g_slist_free_full, g_free, GDestroyNotify)
// 定义g_slist_free_full_by_X509_free
DEFINE_POINTER_CLEANUP_WITH_SPECIFIC_FREE_FUNC(GSList, g_slist_free_full, X509_free, GDestroyNotify)

#define _cleanup_gfree_ _cleanup_(freep)
#define _cleanup_gvariant_unref_ _cleanup_(g_variant_unrefp)
#define _cleanup_gstrv_ _cleanup_(g_strfreevp)
#define _cleanup_gslist_ _cleanup_(g_slist_freep)
#define _cleanup_gvariant_slist_full_ _cleanup_(g_slist_free_full_by_g_variant_unref)
#define _cleanup_default_slist_full_ _cleanup_(g_slist_free_full_by_g_free)
#define _cleanup_x509_slist_full_ _cleanup_(g_slist_free_full_by_X509_free)

/* openssl */
DEFINE_POINTER_CLEANUP_FUNCTION(X509, X509_free)                      // 定义X509_freep
DEFINE_POINTER_CLEANUP_FUNCTION(X509_CRL, X509_CRL_free)              // 定义X509_CRL_freep
DEFINE_POINTER_CLEANUP_FUNCTION(X509_STORE, X509_STORE_free)          // 定义X509_STORE_freep
DEFINE_POINTER_CLEANUP_FUNCTION(X509_NAME, X509_NAME_free)            // 定义X509_NAME_freep
DEFINE_POINTER_CLEANUP_FUNCTION(X509_REQ, X509_REQ_free)              // 定义X509_REQ_freep
DEFINE_POINTER_CLEANUP_FUNCTION(SSL, SSL_free)                        // 定义SSL_freep
DEFINE_POINTER_CLEANUP_FUNCTION(SSL_CTX, SSL_CTX_free)                // 定义SSL_CTX_freep
DEFINE_POINTER_CLEANUP_FUNCTION(BIO, BIO_free)                        // 定义BIO_freep
DEFINE_POINTER_CLEANUP_FUNCTION(EVP_PKEY, EVP_PKEY_free)              // EVP_PKEY_freep
DEFINE_POINTER_CLEANUP_FUNCTION(EVP_CIPHER_CTX, EVP_CIPHER_CTX_free)  // 定义EVP_CIPHER_CTX_freep
DEFINE_POINTER_CLEANUP_FUNCTION(PKCS12, PKCS12_free)                  // 定义PKCS12_freep
DEFINE_POINTER_CLEANUP_FUNCTION(X509_STORE_CTX, X509_STORE_CTX_free)  // 定义X509_STORE_CTX_freep
// 定义sk_X509_pop_free_by_X509_free
DEFINE_POINTER_CLEANUP_WITH_SPECIFIC_FREE_FUNC(STACK_OF(X509), sk_X509_pop_free, X509_free, stack_of_x509_node_free)

#define _cleanup_x509_ _cleanup_(X509_freep)
#define _cleanup_x509_crl_ _cleanup_(X509_CRL_freep)
#define _cleanup_x509_store_ _cleanup_(X509_STORE_freep)
#define _cleanup_x509_name_ _cleanup_(X509_NAME_freep)
#define _cleanup_x509_req_ _cleanup_(X509_REQ_freep)
#define _cleanup_ssl_ _cleanup_(SSL_freep)
#define _cleanup_ssl_ctx_ _cleanup_(SSL_CTX_freep)
#define _cleanup_bio_ _cleanup_(BIO_freep)
#define _cleanup_evp_pkey_ _cleanup_(EVP_PKEY_freep)
#define _cleanup_evp_cipher_ctx_ _cleanup_(EVP_CIPHER_CTX_freep)
#define _cleanup_pkcs12_ _cleanup_(PKCS12_freep)
#define _cleanup_x509_store_ctx_ _cleanup_(X509_STORE_CTX_freep)
#define _cleanup_x509_stack_ _cleanup_(sk_X509_pop_free_by_X509_free)

/* json-c */
DEFINE_POINTER_CLEANUP_FUNCTION(json_object, json_object_put) // 定义json_object_putp
#define _cleanup_json_object_ _cleanup_(json_object_putp)

/* curl */
DEFINE_POINTER_CLEANUP_FUNCTION(CURL, curl_easy_cleanup)   // 定义curl_easy_cleanupp
DEFINE_POINTER_CLEANUP_FUNCTION(CURLM, curl_multi_cleanup) // 定义curl_multi_cleanupp

#define _cleanup_curl_easy_ _cleanup_(curl_easy_cleanupp)
#define _cleanup_curl_multi_ _cleanup_(curl_multi_cleanupp)

/* libxml2 */
DEFINE_POINTER_CLEANUP_FUNCTION(xmlDoc, xmlFreeDoc)   // 定义xmlFreeDocp
DEFINE_POINTER_CLEANUP_FUNCTION(xmlNode, xmlFreeNode) // 定义xmlFreeNodep

#define _cleanup_xml_doc_ _cleanup_(xmlFreeDocp)
#define _cleanup_xml_node_ _cleanup_(xmlFreeNodep)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* CLEANUP_UTIL_H */