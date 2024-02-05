/** @file psysc_types.h
@copyright Copyright (c) 2021 Microchip Technology Inc. All Rights Reserved.

Description: sysCore types header file

*/

#ifndef PSYSC_TYPES_H
#define PSYSC_TYPES_H

#include "psysc_platform.h"

#if !defined(__UEFI) && !defined(UEFI_SC_TEST_CLI)
#if defined(_MSC_VER)

#include <crtdefs.h>

#elif defined(__GNUC__)

#include <stdint.h>
#include <stddef.h>

#if defined(__FREEBSD12)
typedef __int64_t __vm_ooffset_t;
typedef __int64_t __vm_pindex_t;
#endif

#elif defined(__ghs__)

#include <stdint.h>

#endif
#endif

/** @addtogroup syscore
 * @{
 */

#if defined(__cplusplus)
extern "C" {
#endif

/** @defgroup syscore_types Types
 * @brief
 * @details
 * @{
 */

/* _MSC_VER is defined by MS Visual Studio */
#if defined(_MSC_VER)    /* MS Visual Studio */

#if defined(_M_X64)         /* Compile for 64-bit Windows */
#define SC_UNALIGNED __unaligned
#elif defined(_M_IX86)      /* Compile for 32-bit Windows */
#define SC_UNALIGNED
#endif
#define SC_PACKED

#define STRUCT_POSTFIX(x)

#elif defined(__GNUC__)  /* GNU C */

#define SC_UNALIGNED
#define SC_PACKED
#define STRUCT_POSTFIX(x) __attribute__((x))

#elif defined(__ghs__)

#define SC_UNALIGNED __packed
#define SC_PACKED __packed
#define STRUCT_POSTFIX(x)

#endif

#if defined(__UEFI) || defined(DOXYGEN_SHOULD_SKIP_THIS)
#define SC_STATIC_ASSERT(COND,MSG)
#else
#define SC_STATIC_ASSERT(COND,MSG) typedef char static_assertion_##MSG[(COND)?1:-1]
#endif

#if defined(_WIN32)      /* Target OS is MS Windows */
#elif defined(__linux)   /* Target OS is Linux */
#elif defined(__UEFI)    /* Target OS is UEFI */
#endif

/** Boolean for C. */
typedef int SA_BOOL;
/** @ref SA_BOOL TRUE. */
#define kTrue 1
/** @ref SA_BOOL FALSE. */
#define kFalse 0

/** @typedef SA_BYTE
 * @brief Unsigned single byte. */
/** @typedef SA_WORD
 * @brief Unsigned double byte */
/** @typedef SA_DWORD
 * @brief Unsigned quad byte  */
/** @typedef SA_QWORD
 * @brief Unsigned quad word */
/** @typedef SA_INT8
 * @brief Signed 8-bit int */
/** @typedef SA_INT16
 * @brief Signed 16-bit int */
/** @typedef SA_INT32
 * @brief Signed 32-bit int */
/** @typedef SA_INT64
 * @brief Signed 64-bit int */

#if defined(__UEFI)

typedef UINT8 SA_UINT8;
typedef UINT16 SA_UINT16;
typedef UINT32 SA_UINT32;
typedef UINT64 SA_UINT64;
typedef INT8  SA_INT8;
typedef INT16 SA_INT16;
typedef INT32 SA_INT32;
typedef INT64 SA_INT64;

#elif defined(_MSC_VER)

typedef unsigned __int8  SA_UINT8;
typedef unsigned __int16 SA_UINT16;
typedef unsigned __int32 SA_UINT32;
typedef unsigned __int64 SA_UINT64;
typedef signed __int8  SA_INT8;
typedef signed __int16 SA_INT16;
typedef signed __int32 SA_INT32;
typedef signed __int64 SA_INT64;

#elif defined(__GNUC__) || defined(__ghs__)

typedef uint8_t SA_UINT8;
typedef uint16_t SA_UINT16;
typedef uint32_t SA_UINT32;
typedef uint64_t SA_UINT64;
typedef int8_t  SA_INT8;
typedef int16_t SA_INT16;
typedef int32_t SA_INT32;
typedef int64_t SA_INT64;

#endif

typedef SA_UINT8 SA_BYTE;
typedef SA_UINT16 SA_WORD;
typedef SA_UINT32 SA_DWORD;
typedef SA_UINT64 SA_QWORD;

/** Reserved type for command fields. */
typedef SA_BYTE SA_RESERVED;

/** Return SA_DWORD (as little-endian) from 4 byte array (value in big-endian).
 * Return swapped-endian 4-byte integer value.
 */
#define BIG_ENDIAN_2_SA_DWORD(beBuffer) ((((SA_DWORD)beBuffer[0]) << 24) + (((SA_DWORD)beBuffer[1]) << 16) + (((SA_DWORD)beBuffer[2]) << 8) + ((SA_DWORD)beBuffer[3]))
/** Get MSByte from big-endian 4-byte integer. */
#define BIG_ENDIAN_BYTE_3(beValue) ((beValue & 0xFF000000) >> 24)
/** Get 2nd MSByte from big-endian 4-byte integer. */
#define BIG_ENDIAN_BYTE_2(beValue) ((beValue & 0x00FF0000) >> 16)
/** Get 3rd MSByte from big-endian 4-byte integer. */
#define BIG_ENDIAN_BYTE_1(beValue) ((beValue & 0x0000FF00) >> 8)
/** Get LSByte from big-endian 4-byte integer. */
#define BIG_ENDIAN_BYTE_0(beValue) (beValue & 0x000000FF)

/** Return SA_QWORD (as little-endian) from 8 byte array (value in big-endian). */
#define BIG_ENDIAN_2_SA_QWORD(beBuffer) ((((SA_QWORD)beBuffer[0]) << 56) + (((SA_QWORD)beBuffer[1]) << 48) + (((SA_QWORD)beBuffer[2]) << 40) + (((SA_QWORD)beBuffer[3]) << 32) + \
   (((SA_QWORD)beBuffer[4]) << 24) + (((SA_QWORD)beBuffer[5]) << 16) + (((SA_QWORD)beBuffer[6]) << 8) + ((SA_QWORD)beBuffer[7]))
/** Return SA_WORD (as little-endian) from 2 byte array (value in big-endian). */
#define BIG_ENDIAN_2_SA_WORD(beBuffer) ((((SA_WORD)beBuffer[0]) << 8) + (SA_WORD)beBuffer[1])

/** @} */ /* syscore_types */

/** @addtogroup syscore_device_handler Device Handlers
 * @{
 */
/** A device descriptor (a.k.a. handle). */
typedef void *DeviceDescriptor;
/** A list of @ref DeviceDescriptor. */
typedef void *DeviceDescriptorList;
/** An iterator for a @ref DeviceDescriptorList. */
typedef void *DeviceDescriptorListIterator;
/** @} */ /* syscore_device_handler */

#if defined(__cplusplus)
}
#endif

/** @} */ /* syscore */

#endif /* PSYSC_TYPES_H */
