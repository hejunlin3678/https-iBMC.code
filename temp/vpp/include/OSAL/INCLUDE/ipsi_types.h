/*
Copyright (c) Huawei Technologies Co., Ltd. 2006-2019. All rights reserved.
FileName: ipsi_types.h
Author: Jayaraghavendran K
Version:1
Description: This file contains abstraction for data types add system functions.
Create: 2006-12-29
*****************************************************************/

#ifndef __IPSI_TYPES_H__
#define __IPSI_TYPES_H__

#ifndef IPSEC_VISP
#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <stdarg.h>

#ifdef __TI_COMPILER_VERSION__
#include <file.h>
#else
#include <sys/types.h>
#endif
#endif

/*
    While building the ipsi library in Windows __IPSIBUILDDLL__ should be
    defined. For a windows application to use the exported functions from
    ipsi dll, __IPSIUSEDLL__ should to be defined.
*/
#ifdef __IPSIBUILDDLL__
#define IPSILINKDLL __declspec(dllexport)
#else
#ifdef __IPSIUSEDLL__
#define IPSILINKDLL __declspec(dllimport)
#else
#define IPSILINKDLL
#endif
#endif

#if defined(__GNUC__) && defined(__linux__)
#define IPSIFORMAT(archeType, strIndex, firstToCheck) __attribute__((format(archeType, strIndex, firstToCheck)))
#else
#define IPSIFORMAT(archeType, strIndex, firstToCheck)
#endif

#ifdef IPSEC_VISP
#ifndef size_t
#define size_t ULONG
#endif  /* size_t */
#endif  // IPSEC_VISP

#if defined(WIN32) && !defined(SEC_DOPRA)
typedef __int64 ipsi_time_t;
#else
#ifdef SEC_DOPRA
typedef long int ipsi_time_t;
#else
typedef long long ipsi_time_t;
#endif
#endif

#ifndef IPSI_ASSERT
#ifdef DEBUG
#include <assert.h>
#define IPSI_ASSERT(f) assert(f)
#else
#define IPSI_ASSERT(f) ((void)0)
#endif
#endif

#define IPSI_SUCCESS 0
#define IPSI_FAILURE (-1)

#define SEC_OK 0
#define SEC_ERR 1
#define SEC_ERROR (-1)
#define SEC_NULL 0
#define SEC_TRUE 1
#define SEC_FALSE 0
#define SEC_SUCCESS 0

#define SEC_VOID void
#define IPSI_ULONG_MAX ULONG_MAX

typedef unsigned char SEC_UINT8;

typedef signed char SEC_INT8;

typedef char SEC_CHAR;

typedef unsigned char SEC_UCHAR;

typedef unsigned short SEC_UINT16;

typedef signed short SEC_INT16;

typedef signed short SEC_WCHAR;

#if defined(__LP64__) || defined(__64BIT__)

typedef unsigned int SEC_UINT32;

typedef signed int SEC_INT32;

typedef unsigned int SEC_BOOL;

#else

typedef unsigned long SEC_UINT32;

typedef signed long SEC_INT32;

typedef unsigned long SEC_BOOL;

#endif

typedef int SEC_INT;

typedef unsigned int SEC_UINT;

// Do not use this varaible for any other data type i.e. any 32 bit operations
typedef signed long SEC_LONG;

typedef unsigned long SEC_ULONG;

typedef ipsi_time_t SEC_TIME_T;

#ifdef SEC_DOPRA
#ifndef IPSEC_VISP
typedef void *SEC_FILE_F;
#else
typedef void *SEC_FILE_F;
#endif
#else
typedef FILE SEC_FILE_F;
#endif

#ifdef WIN32
typedef __int64 SEC_INT64;
#elif defined(SEC_DOPRA)
typedef long SEC_INT64;
#else
typedef off_t SEC_INT64;
#endif

#ifndef __IPSI_NO_SEC_64BIT

#if defined(_MSC_VER)
typedef unsigned __int64 SEC_UINT64;
#else
#if defined(__linux__)
#include <stdint.h>
#else
#if defined(__vxworks__)
#if !defined(VXWORKS_VERSION) && !defined(__INCvxTypesh)
#error "not able to identify vxworks platform"
#endif
#endif

#if defined(VXWORKS_VERSION) || defined(__INCvxTypesh)
#include <vxWorks.h>
#else
#include <inttypes.h>
#endif
#endif

#if (defined(__GNUC__) && (__GNUC__ < 3))
typedef unsigned long long SEC_UINT64;
#else
typedef uint64_t SEC_UINT64;
#endif

#endif

#endif  // __IPSI_NO_SEC_64BIT

#ifdef __cplusplus
extern "C" {
#endif

/* From sec_def.h */
#define SEC_StrLen ipsi_strlen
#define SEC_StrNLen ipsi_strnlen
#define SEC_StrStr ipsi_strstr
#define SEC_StrCmp ipsi_strcmp
#define SEC_StrNCmp ipsi_strncmp
#define SEC_StrChr ipsi_strchr
#define SEC_printf ipsi_printf
#define SEC_MemCmp ipsi_memcmp
#define SEC_MemChr ipsi_memchr
#define SEC_StrNiCmp ipsi_strnicmp
#define SEC_StrICmp ipsi_stricmp

// max file path
#define IPSI_MAX_OSAL_PATH 4096


#ifndef __THIS_FILE__
#define __THIS_FILE__ __FILE__
#endif

/*
 * filepath cannocalization and validation of the file path callback
 * inPath provides the input of the application,
 * outPath is the output path after the cannocalization of the inPath
 * uiOutPathLen is the length of the outPath.
 * Application must validate path after the cannocalization
 * and if the verification is successful than copy the cannocalized path in the outPath.
 * outPath must be updated as a string. The size of the outPath is mentioned in uiOutPathLen.
 * The string should be with in the uiOutPathLen
 * including the null character
*/
typedef int (*IPSI_CANONICALIZE_VALIDATE_FILEPATH_CB)(char *inPath, char *outPath, unsigned int uiOutPathLen);

// file permission callback
typedef int (*IPSI_FILE_PERMISSION_CB)(char *);

/*
* @defgroup DATETIME_S
* @ingroup sec_sysStructure
* @par Prototype
* @code
* typedef struct tagDateTime
* {
*   SEC_UINT16 uwYear;
*   SEC_UINT8  ucMonth;
*   SEC_UINT8  ucDay;
*   SEC_UINT8  ucHour;
*   SEC_UINT8  ucMinute;
*   SEC_UINT16 uwMillSecond;
*   SEC_UINT8  ucSecond;
*   SEC_UINT8  ucUTCSign;
*   SEC_UINT8  ucUTCHour;
*   SEC_UINT8  ucUTCMinute;
* }DATETIME_S;
* @endcode
*
* @datastruct uwYear Year,the value domain is from 0 to 65536.
* @datastruct ucMonth Month,the value domain is from 1 to 12.
* @datastruct ucDay Day,the value domain is from 1 to 31.
* @datastruct ucHour Hour,the value domain is from 0 to 23.
* @datastruct ucMinute Minute,the value domain is from 0 to 59.
* @datastruct uwMillSecond Millsecond,the value domain is from 0
to 999, place millsecond before second because of padding.
* @datastruct ucSecond Second,the value domain is from 0 to 59
* @datastruct ucUTCSign Positive or negtive to UTC.\n
* 0 - Positive\n
* 1 - Negetive.
* @datastruct ucUTCHour Hour to UTC,the value domain is from 0~11.
* @datastruct ucUTCMinute Minutes to UTC,the value domain is from 0~59.
*/
typedef struct tagDateTime {
    SEC_UINT16 uwYear;  /* Year,the value domain is from 0 to 65536 */
    SEC_UINT8 ucMonth;  /* Month,the value domain is from 1 to 12 */
    SEC_UINT8 ucDay;    /* Day,the value domain is from 1 to 31 */
    SEC_UINT8 ucHour;   /* Hour,the value domain is from 0 to 23 */
    SEC_UINT8 ucMinute; /* Minute,the value domain is from 0 to 59 */

    /* Millsecond,the value domain is from 0 to 999, place millsecond
    before second because of padding */
    SEC_UINT16 uwMillSecond;
    SEC_UINT8 ucSecond;    /* Second,the value domain is from 0 to 59 */
    SEC_UINT8 ucUTCSign;   /* Positive or negtive to UTC 0:positive 1:negtive */
    SEC_UINT8 ucUTCHour;   /* Hour to UTC,the value domain is from 0~11. */
    SEC_UINT8 ucUTCMinute; /* Minutes to UTC,the value domain is from 0~59. */
} DATETIME_S;

#define SEC_fprintf ipsi_fprintf

/* !<To compare two memory blocks. Uses Dopra function */
#define SEC_MEM_CMP(pDest, pSrc, ulCount) ipsi_memcmp(pDest, pSrc, ulCount)

#define SEC_StrToUint32(str, plnum) ipsi_strtouint32(str, plnum)

#define SEC_CompareSysTime2Sec ipsi_comparesystime2sec
#define SEC_CalRelativeSec ipsi_calrelativesec

/*
* @defgroup DATE_COMPARE_E
* @ingroup asn_typesEnum
* @par Prototype
* @code

*
* @datastruct ERROR_CMP=0   Error in comparison of the two dates.
* @datastruct EQUAL_DATE=1  The two dates are same.
* @datastruct BEFORE_DATE=2 The first date is before the second date.
* @datastruct AFTER_DATE=3  The first date is after the second date.
*/
typedef enum {
    ERROR_CMP,   /* Error in comparison of the two dates */
    EQUAL_DATE,  /* The two dates are same */
    BEFORE_DATE, /* The first date is before the second date */
    AFTER_DATE   /* The first date is after the second date */
} DATE_COMPARE_E;

#ifndef __IPSI_MBSC_SMALL_OSAL__

/* SEC_DOPRA_STRUCT_NO_REDEF needs to be defined to resolve structure redefiintion */
/* error for SYS_T and COMP_TIME_RESULT_S */
#ifndef SEC_DOPRA_STRUCT_NO_REDEF
#ifndef SEC_DOPRA
#ifndef VOS_RTOSCK

/*
 * @defgroup SYS_T
 * @ingroup sec_defStructure
 * @par Prototype
 * @code
 * typedef struct tagSysTime
 * {
 *  SEC_UINT16 uwYear;
 *  SEC_UINT8  ucMonth;
 *  SEC_UINT8  ucDate;
 *  SEC_UINT8  ucHour;
 *  SEC_UINT8  ucMinute;
 *  SEC_UINT8  ucSecond;
 *  SEC_UINT8  ucWeek;
 * }SYS_T;
 * @endcode
 *
 * @datastruct uwYear year.
 * @datastruct ucMonth Month, scope is 1 to 12.
 * @datastruct ucDate Date, scope is 1 to 31.
 * @datastruct ucHour Hour, scope is 0 to 23.
 * @datastruct ucMinute Minute, scope is 0 to 59.
 * @datastruct ucSecond Second, scope is 0 to 59.
 * @datastruct ucWeek Week,scope is 1 to 7.
 */
typedef struct tagSysTime {
    SEC_UINT16 uwYear;  /* uwYear year */
    SEC_UINT8 ucMonth;  /* ucMonth Month, scope is 1 to 12 */
    SEC_UINT8 ucDate;   /* ucDate Date, scope is 1 to 31 */
    SEC_UINT8 ucHour;   /* ucHour Hour, scope is 0 to 23 */
    SEC_UINT8 ucMinute; /* ucMinute Minute, scope is 0 to 59 */
    SEC_UINT8 ucSecond; /* ucSecond Second, scope is 0 to 59 */
    SEC_UINT8 ucWeek;   /* ucWeek Week,scope is 1 to 7 */
} SYS_T;

#endif  // VOS_RTOSCK
/* This enumeration contains time comparision result */
/*
 * @defgroup Comp_Time_Result
 * @ingroup sec_defEnum
 * @par Prototype
 * @code
 * typedef enum Comp_Time_Result
 * {
 *  COMP_TIME_ERROR,
 *  COMP_TIME_LT,
 *  COMP_TIME_EQ,
 *  COMP_TIME_GT
 * }Comp_Time_Result
 * @endcode
 *
 * @datastruct COMP_TIME_ERROR Compare result: error.
 * @datastruct COMP_TIME_LT Compare result: less than.
 * @datastruct COMP_TIME_EQ Compare result: equal to.
 * @datastruct COMP_TIME_GT Compare result: Greater than.
 */
typedef enum Comp_Time_Result {
    COMP_TIME_ERROR, /* Compare result: error        */
    COMP_TIME_LT,    /* Compare result: less than    */
    COMP_TIME_EQ,    /* Compare result: equal to     */
    COMP_TIME_GT     /* Compare result: Greater than */
} COMP_TIME_RESULT_S;

#endif  // SEC_DOPRA
#endif  // SEC_DOPRA_STRUCT_NO_REDEF
#endif  // __IPSI_MBSC_SMALL_OSAL__

#ifdef __cplusplus
}
#endif

#endif  // IPSI_TYPES_H_
