/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2015-2018. All rights reserved.
 * Description: �����������͡�OS�ص�����־�������뼰�־û�.
 * Author: AdaptiveLM team
 * Create: 2015-07-14
 */
#ifndef ALM_INTF_BASE_H
#define ALM_INTF_BASE_H

#include <stdio.h>
#include <limits.h>

#include "alm_intf_constant.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* if __cplusplus */
#endif /* ifdef __cplusplus */

#if (defined(ALM_PACK_4) && defined(ALM_PACK_8))
#error Can not support both align 4 and align 8
#endif

/************************************************************************
 * ������������
 * ***********************************************************************
 *
 * Define True and False��
 *
 ************************************************************************/
#ifndef ALM_TRUE
#define ALM_TRUE 1 /* ֻ��֤��0������֤Ϊ1 */
#endif

#ifndef ALM_FALSE
#define ALM_FALSE 0
#endif

/*
 * For const������const�����䴦����Ϊʹ��const
 * ����߱������Դ���ļ�飬��ǿ���͵Ĵ���
 * ��Ϊ���ṩ�Բ�ͬ�����������䣬����ʹ��һ����
 * ���д���
 */
#ifndef CONST
#define CONST const
#endif

/************************************************************************
 *
 * Define ALM_STATIC
 *
 ************************************************************************/
#ifndef ALM_STATIC
#ifdef HUTAF_LLT_BUILD
#define ALM_STATIC
#else
#define ALM_STATIC static
#endif
#endif

/************************************************************************
 *
 * Define NULL pointer
 *
 ************************************************************************/
#ifndef ALM_NULL
#ifdef __cplusplus
#define ALM_NULL 0
#else
#define ALM_NULL ((void *)0)
#endif
#endif

/************************************************************************
 *
 * Define ALM_VOID
 *
 ************************************************************************/
#define ALM_VOID void

/************************************************************************
 * Define ALM_UINT8, ALM_INT8
 ************************************************************************/
#if UCHAR_MAX == 0xff
typedef unsigned char ALM_UINT8;
typedef char ALM_INT8;
#else
#error This machine has no 8 bit type
#endif

/************************************************************************
 *
 * Define ALM_BOOL
 *
 ************************************************************************/
typedef ALM_UINT8 ALM_BOOL;

/************************************************************************
 * Define ALM_UCHAR, ALM_CHAR
 ************************************************************************/
typedef ALM_UINT8 ALM_UCHAR;
typedef ALM_INT8 ALM_CHAR;

/************************************************************************
 * Define ALM_UINT16, ALM_INT16
 ************************************************************************/
#if UINT_MAX == 0xffff
typedef unsigned int ALM_UINT16;
typedef int ALM_INT16;
#define MAX_ALM_UINT16 65535
#define MAX_ALM_INT16 32767
#define MIN_ALM_INT16 (-32768)
#elif USHRT_MAX == 0xffff
typedef unsigned short ALM_UINT16;
typedef short ALM_INT16;
#define MAX_ALM_UINT16 65535
#define MAX_ALM_INT16 32767
#define MIN_ALM_INT16 (-32768)
#else
#error This machine has no 16 bit type
#endif

/************************************************************************
 * Define ALM_UINT32, ALM_INT32
 ************************************************************************/
#if UINT_MAX == 0xffffffffUL
typedef unsigned int ALM_UINT32;
typedef int ALM_INT32;
#define MAX_ALM_UINT32 4294967295UL
#define MAX_ALM_INT32 2147483647
#define MIN_ALM_INT32 (-2147483647 - 1)
#elif ULONG_MAX == 0xffffffffUL
typedef unsigned long ALM_UINT32;
typedef long ALM_INT32;
#define MAX_ALM_UINT32 4294967295UL
#define MAX_ALM_INT32 2147483647
#define MIN_ALM_INT32 (-2147483647 - 1)
#elif USHRT_MAX == 0xffffffffUL
typedef unsigned short ALM_UINT32;
typedef short ALM_INT32;
#define MAX_ALM_UINT32 4294967295UL
#define MAX_ALM_INT32 2147483647
#define MIN_ALM_INT32 (-2147483647 - 1)
#else
#error This machine has no 32 bit type
#endif

#ifndef ALM_HAVE64
#ifndef WIN32
#if ULONG_MAX > 0xffffffffUL
#if ULONG_MAX == 0xffffffffffffffffUL
typedef unsigned long ALM_UINT64;
typedef long ALM_INT64;
#define ALM_HAVE64 1
#endif
#elif defined(ULONG_LONG_MAX) || defined(ULLONG_MAX) || defined(ULONGLONG_MAX) || defined(__LONG_LONG_MAX__)
typedef unsigned long long ALM_UINT64;
typedef long long ALM_INT64;
#define ALM_HAVE64 1
#endif
#endif
#endif

/***************************************************************************
 This was added because for some reason or another, __LONG_LONG_MAX__ is
 not defined on Linux 6.1.  Hopefully this doesn't break older versions of
 Linux but you never know.....
****************************************************************************/
#if (defined(__LINUX__) || defined(__SUN__) || defined(__HPUX__) || defined(__AIX__) || defined(__WRLINUX__))
#ifndef ALM_HAVE64
typedef long long ALM_INT64;
typedef unsigned long long ALM_UINT64;
#define ALM_HAVE64 1
#endif
#endif

#ifndef ALM_HAVE64
#if defined(__MWERKS__)
#if __option(longlong)
typedef unsigned long long ALM_UINT64;
typedef long long ALM_INT64;
#define ALM_HAVE64 1
#endif
#endif
#endif

/************************************************************************
 * :: for msvc
 ************************************************************************/
/* pc-lint error 30:Expected a constant  -- A constant was expected but not
 * obtained.  This could be following #if expression.
 * _UI64_MAX == 0xffffffffffffffffUL is jugding the 64 bits env,
 *  we inhibit this error.
 */
#ifndef __VXWORKS__
#ifndef ALM_HAVE64
#if defined(_UI64_MAX)
#if _UI64_MAX == 0xffffffffffffffffUL
typedef unsigned __int64 ALM_UINT64;
typedef signed __int64 ALM_INT64;
#define ALM_HAVE64 1
#endif
#endif
#endif
#endif

/* ���붨��64λ�������� */
#ifdef ALM_HAVE64
#if !(ALM_HAVE64)
#error "This os may be not support INT64."
#endif
#else
#error "This os may be not support INT64."
#endif

/************************************************************************
 * �����������������Ǻꡣ
 ************************************************************************/
#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef INOUT
#define INOUT
#endif

#ifndef SECUREC_SUPPORT_FORMAT_WARNING
#define SECUREC_SUPPORT_FORMAT_WARNING 0
#endif
#if SECUREC_SUPPORT_FORMAT_WARNING
#define ALM_SECUREC_ATTRIBUTE(x, y) __attribute__((format(printf, (x), (y))))
#else
#define ALM_SECUREC_ATTRIBUTE(x, y)
#endif

/************************************************************************
 * �����ꡣ
 ************************************************************************/
/*
 * For DLL
 */
#if defined(_WIN32)
#if defined(__GNUC__)
#define LIC_PARSER_EXPORT
#else
#if defined(LIC_UT) /* For unit test. */
#define LIC_PARSER_EXPORT
#elif defined(LIC_BUILD_STATIC)
#define LIC_PARSER_EXPORT
#else
#if defined(LIC_BUILD_DLL)
#define LIC_PARSER_EXPORT __declspec(dllexport)
#else
#define LIC_PARSER_EXPORT __declspec(dllimport)
#endif
#endif
#endif
#else
#define LIC_PARSER_EXPORT
#endif /* LIC_WIN32 */

/* ֻ�����ڵ���ģʽ�¶������У��� */
#ifdef ALM_BUILD_CHECK
#ifndef LIC_DEBUG
#error "Do not dim ALM_BUILD_CHECK in release."
#endif
#endif

/*
 * Ϊ�˹̻�ö��������ռ�õ��ֽ������Լ�����java��װ��ö������ͳһʹ��ALM_UINT32��
 * ����˺��Ŀ�����ڲ��컯����ö�ټ�ALM_UINT32���ͣ�Ҳ���ڱ���У�顣
 * ��Ʒ������Ա����ALMö������ֻ�����ΪALM_UINT32���ͣ�����Ҳ��Ӧ�ö������У
 * ���ALM_BUILD_CHECK��
 */
#ifdef ALM_BUILD_CHECK
#define ALM_ENUM(enum_type) enum_type
#else
#define ALM_ENUM(enum_type) ALM_UINT32
#endif

/* ����ö�����ʹ�С */
#define ALM_ENUM_SIZE sizeof(ALM_ENUM(ALM_RET_ENUM))

/**
 * For DLL
 */
#if defined(WIN32)
#if defined(__GNUC__)
#define LIC_EXPORT
#else
#if defined(LIC_BUILD_DLL)
#define LIC_EXPORT __declspec(dllexport)
#else
#define LIC_EXPORT
#endif
#endif
#else
#define LIC_EXPORT
#endif /* win32 */

/**
 * basic.soר��Ϊcompatible.so������api
 */
#if defined(WIN32)
#if defined(__GNUC__)
#define LIC_EXPORT_FOR_COMPATIBLE
#else
#if defined(LIC_BUILD_DLL)
#define LIC_EXPORT_FOR_COMPATIBLE __declspec(dllexport)
#else
#define LIC_EXPORT_FOR_COMPATIBLE
#endif
#endif
#else
#define LIC_EXPORT_FOR_COMPATIBLE
#endif /* win32 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* if __cplusplus */
#endif /* ifdef __cplusplus */

#endif /* ALM_INTF_BASE_H */
