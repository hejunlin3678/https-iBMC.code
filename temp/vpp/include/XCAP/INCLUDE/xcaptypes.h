/* *****************************************************************************

    Copyright (c) Huawei Technologies Co., Ltd. 2008-2019. All rights reserved.

    Description:   data type define

    Function List: NA

    Author:  L36241,W57322

    Create:  2008-4-10

***************************************************************************** */
#ifndef XCAPTYPES_H
#define XCAPTYPES_H

#if defined(XCAP_WIN32) || defined(XCAP_WIN64)
#include <stdio.h>
#else
#ifndef XCAP_VXWORKS_NO_STDINT_H
#include <stdint.h>
#endif
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#if defined(__GNUC__) && defined(__linux__)
#define XCAPFORMAT(archeType, strIndex, firstToCheck) __attribute__((format(archeType, strIndex, firstToCheck)))
#else
#define XCAPFORMAT(archeType, strIndex, firstToCheck)
#endif

#if (((defined(XCAP_VXWORKS)) || (defined(_MSC_VER) && (_MSC_VER == 1200))) && !defined(_STDINT) && \
    !defined(_STDINT_H)) ||                                                                         \
    (defined(XCAP_OS_MINGW))
#ifndef VPP_UINTPTR_T_DEFINED /* Other VPP components shall not define the type multiple times */
#define VPP_UINTPTR_T_DEFINED
typedef unsigned long uintptr_t;
#endif
#endif

#define IN

#define OUT

#define INOUT

#define XCAP_VOID void

typedef unsigned char XCAP_UINT8;

typedef unsigned short XCAP_UINT16;

typedef unsigned int XCAP_UINT32;

typedef unsigned long XCAP_ADDR_T;

#if defined(XCAP_WIN64) || defined(WIN64)
typedef uintptr_t XCAP_ULONG;
typedef intptr_t XCAP_LONG;
#else
typedef unsigned long XCAP_ULONG;
typedef signed long XCAP_LONG;
#endif

typedef signed char XCAP_INT8;

typedef signed short XCAP_INT16;

typedef signed int XCAP_INT32;

typedef unsigned int XCAP_BOOL; /* Size is 4 bytes */

typedef char XCAP_CHAR;

typedef const char XCAP_CCHAR;

typedef uintptr_t XCAP_HANDLE;

typedef XCAP_INT32 XCAP_ERR_CODE;

typedef XCAP_UINT32 XCAP_SOCKET;

#define XCAP_NULL_UINT8 0xFF

#define XCAP_NULL_UINT16 0xFFFF

#define XCAP_NULL_UINT32 0xFFFFFFFF

#define XCAP_NULL_ULONG (XCAP_ULONG)(~0L)

#define XCAP_NULL_HANDLE (XCAP_HANDLE)(~0L)

#define XCAP_NULL 0L

#define XCAP_NULL_PTR 0L

#define XCAP_FALSE 0
#define XCAP_TRUE 1

#define XCAP_SUCCESS 0
#define XCAP_FAILURE (-1)

#define XCAP_INVALID_INT32 0x7FFFFFFF
#define XCAP_INVALID_UINT32 0xFFFFFFFFU

/* DTS2017021511654[27-03-17] max data send is restricted
to 1 gb to prevent overflow */
#define XCAP_MAX_DATA_SEND 0x40000000

#ifdef VPP_ENABLE_STATIC
#define XCAP_STATIC static
#else
#define XCAP_STATIC
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* _XCAP_TYPES_H_ */
