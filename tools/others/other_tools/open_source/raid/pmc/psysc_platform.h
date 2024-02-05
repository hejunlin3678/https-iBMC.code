/** @file psysc_platform.h
@copyright Copyright (c) 2021 Microchip Technology Inc. All Rights Reserved.

Description: sysCore header file

*/

#ifndef PSYSC_PLATFORM_H
#define PSYSC_PLATFORM_H

/** @addtogroup syscore
 * @{
 */

/** @defgroup syscore_platform Platform
 * @brief
 * @details
 * @{
 */

/** @def SC_INLINE
 * @brief Platform-independent inline specifier for functions. */

/** @def SC_UNUSED
 * @brief Platform-independent unused  attribute specifier (if supported on platform). */

/** @def SA_memcpy
 * @brief Platform-independent call to [memcpy](http://www.cplusplus.com/reference/cstring/memcpy/)-like function. */
/** @def SA_memset
 * @brief Platform-independent call to [memset](http://www.cplusplus.com/reference/cstring/memset/)-like function. */
/** @def SA_memcmp
 * @brief Platform-independent call to [memcmp](http://www.cplusplus.com/reference/cstring/memcmp/)-like function. */

/** @def SA_strlen
 * @brief Platform-independent call to [strlen](http://www.cplusplus.com/reference/cstring/strlen/)-like function. */
/** @def SA_strcmp
 * @brief Platform-independent call to [strcmp](http://www.cplusplus.com/reference/cstring/strcmp/)-like function. */
/** @def SA_strncmp
 * @brief Platform-independent call to [strncmp](http://www.cplusplus.com/reference/cstring/strncmp/)-like function. */
/** @def SA_strcpy
 * @brief Platform-independent call to [strcpy](http://www.cplusplus.com/reference/cstring/strcpy/)-like function. */
/** @def SA_strncpy
 * @brief Platform-independent call to [strncpy](http://www.cplusplus.com/reference/cstring/strncpy/)-like function. */
/** @def SA_strcat
 * @brief Platform-independent call to [strcat](http://www.cplusplus.com/reference/cstring/strcat/)-like function. */
/** @def SA_strncat
 * @brief Platform-independent call to [strncat](http://www.cplusplus.com/reference/cstring/strncat/)-like function. */
/** @def SA_strstr
 * @brief Platform-independent call to [strstr](http://www.cplusplus.com/reference/cstring/strstr/)-like function */
/** @def SA_sprintf
 * @brief Platform-independent call to [sprintf](http://www.cplusplus.com/reference/cstring/sprintf/)-like function */

/** @def SA_atoi
 * @brief Platform-independent call to [atoi](http://www.cplusplus.com/reference/cstdlib/atoi/)-like function. */

/** @def SA_atof
 * @brief Platform-independent call to [atof](http://www.cplusplus.com/reference/cstdlib/atof/)-like function. */

/** @def SA_tolower
 * @brief Platform-independent call to [tolower](http://www.cplusplus.com/reference/cctype/tolower/)-like function. */

/** @def SA_malloc
 * @brief Platform-independent call to [malloc](http://www.cplusplus.com/reference/cstdlib/malloc/)-like function. */
/** @def SA_realloc
 * @brief Platform-independent call to [realloc](http://www.cplusplus.com/reference/cstdlib/realloc/)-like function. */
/** @def SA_free
 * @brief Platform-independent call to [free](http://www.cplusplus.com/reference/cstdlib/free/)-like function. */
/** @def SA_gets_s
 * @brief Platform-independent call to [gets_s](https://en.cppreference.com/w/c/io/gets)-like function. */
/** @def SA_sprintf_s
 * @brief Platform-independent call to [sprintf_s](https://en.cppreference.com/w/c/io/fprintf)-like function. */
/** @def SA_vsnprintf_s
 * @brief Platform-independent call to [vsnprintf_s](https://en.cppreference.com/w/c/io/vfprintf)-like function.
 * For Windows, please see https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/vsnprintf-s-vsnprintf-s-vsnprintf-s-l-vsnwprintf-s-vsnwprintf-s-l?view=vs-2019
 */
/** @def SA_sscanf_s
 * @brief Platform-independent call to [sscanf_s](https://en.cppreference.com/w/c/io/fscanf)-like function. */
/** @def SA_strcat_s
 * @brief Platform-independent call to [strcat_s](https://en.cppreference.com/w/c/io/gets)-like function. */
/** @def SA_strncat_s
 * @brief Platform-independent call to [strncat_s](https://en.cppreference.com/w/c/string/byte/strncat)-like function. */
/** @def SA_strtok_s
 * @brief Platform-independent call to [strtok_s](https://en.cppreference.com/w/c/string/byte/strtok)-like function.
 * @attention @p strmax is the string length (not including the terminating null-character) of @p str.
 * For Windows, please see https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/strtok-s-strtok-s-l-wcstok-s-wcstok-s-l-mbstok-s-mbstok-s-l?view=vs-2019
 */
/** @def SA_strcpy_s
 * @brief Platform-independent call to [strcpy_s](https://en.cppreference.com/w/c/string/byte/strcpy)-like function. */
/** @def SA_strncpy_s
 * @brief Platform-independent call to [strncpy_s](https://en.cppreference.com/w/c/io/strncpy)-like function. */
/** @def SA_memcpy_s
 * @brief Platform-independent call to [memcpy_s](https://en.cppreference.com/w/c/string/byte/memcpy)-like function. */
/** @def SA_strnlen_s
 * @brief Platform-independent call to [strnlen_s](https://en.cppreference.com/w/c/string/byte/strlen)-like function. */

#if !defined(__UEFI)
#if defined(_MSC_VER)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <ctype.h>
#include <stdarg.h>
#include <time.h>
#elif defined(__GNUC__)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <ctype.h>
#include <stdarg.h>
#include <time.h>
#endif
#endif

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(__UEFI) || defined(OPTIMIZE_SINGLE_THREAD_CLIENT)
#  ifndef SC_DISABLE_SYNC_API
#    define SC_DISABLE_SYNC_API
#  endif
#  ifndef SC_DISABLE_THREAD_API
#    define SC_DISABLE_THREAD_API
#  endif
#  ifndef SC_DISABLE_MESSAGE_QUEUE
#    define SC_DISABLE_MESSAGE_QUEUE
#  endif
#  ifndef SC_DISABLE_EVENT_FLAGS
#    define SC_DISABLE_EVENT_FLAGS
#  endif
#endif

#if defined(_MSC_VER) || defined(__UEFI)

#if defined(__cplusplus)
#define SC_INLINE inline
#else
#define SC_INLINE __inline
#endif
#define SC_UNUSED(x) x


#if defined(__UEFI)
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/PciIo.h>
#include <IndustryStandard/Pci22.h>
#include <Library/PrintLib.h>
#include <Library/UefiLib.h>

#define SA_memcpy CopyMem
#define SA_memset(Buffer,Value,Length) SetMem(Buffer, Length, Value)
#define SA_memcmp CompareMem
#define SA_strlen AsciiStrLen
#define SA_strcmp AsciiStrCmp
#define SA_strncmp AsciiStrnCmp
#define SA_strcpy AsciiStrCpy
#define SA_strncpy AsciiStrnCpy
#define SA_strcat AsciiStrCat
#define SA_strncat AsciiStrnCat
#define SA_strstr AsciiStrStr
#define SA_sprintf AsciiSPrint
#define sprintf AsciiSPrint
UINTN UefiAsciiVSPrint(char *StartOfBuffer,UINTN BufferSize,const char *FormatString,VA_LIST Marker);
#define SA_snprintf AsciiSPrint
#define SA_vsnprintf UefiAsciiVSPrint

VOID StorageCoreSerialTrace(UINT8 level, UINT32 mask, const CHAR8 * mesg);

#ifdef offsetof
#undef offsetof
#endif
#define offsetof OFFSET_OF

#define SA_atoi AsciiStrDecimalToUintn
#define va_list VA_LIST
#define va_start VA_START
#define va_arg VA_ARG
#define va_end VA_END
char SA_tolower(char AsciiChar);
#define SA_qsort(Buf,Count,Size,Fun) PerformQuickSort(Buf,Count,Size,(SORT_COMPARE)Fun)
int SA_isprint(int c);
int SA_isspace(int c);
#define size_t UINTN
#define SA_isdigit(AsciiChar) (AsciiChar >= '0' && AsciiChar <= '9')
#define _getch getchar

/* Windows */
#else
/* !__UEFI */

#define SA_memcpy memcpy
#define SA_memset memset
#define SA_memcmp memcmp
#define SA_strlen strlen
#define SA_strcmp strcmp
#define SA_strncmp strncmp
#define SA_strcpy strcpy
#define SA_strncpy strncpy
#define SA_strcat strcat
#define SA_strncat strncat
#define SA_strstr strstr
#define SA_sprintf sprintf
#define SA_strchr strchr

#define SA_atoi atoi
#define SA_atof atof
#define SA_atol atol
#define SA_tolower tolower

#define SA_qsort qsort

#define SA_isalnum isalnum
#define SA_isprint isprint
#define SA_isspace isspace
#define SA_isdigit isdigit

#define SA_printf printf
#define SA_sprintf sprintf

#endif
/* END OF _MSC_VER */

#elif defined(__GNUC__) || defined(__ghs__)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <ctype.h>
#include <time.h>

#if defined(SYSCORE_RAIDSTACK)
#include <resource/mem.h>
#endif

#define SC_INLINE inline
#define SC_UNUSED(x) UNUSED_ ## x __attribute__((unused))

#define SA_memcpy memcpy
#define SA_memset memset
#define SA_memcmp memcmp

#define SA_strlen strlen
#define SA_strcmp strcmp
#define SA_strncmp strncmp
#define SA_strcpy strcpy
#define SA_strncpy strncpy
#define SA_strcat strcat
#define SA_strncat strncat
#define SA_strstr strstr
#define SA_sprintf sprintf
#define SA_strchr strchr

#define SA_atoi atoi
#define SA_atof atof
#define SA_atol atol

#define SA_tolower tolower

#define SA_qsort qsort

#define SA_isalnum isalnum
#define SA_isprint isprint
#define SA_isspace isspace
#define SA_isdigit isdigit

#if defined(SYSCORE_RAIDSTACK)
#define SA_printf zprintf
#else
#define SA_printf printf
#endif
#define SA_sprintf sprintf
#endif

#if defined(SYSCORE_RAIDSTACK)
#define SA_free(p) lpram_free(p)
static SC_INLINE void* SA_malloc(size_t n)
{
   if (n == 0) return NULL;
   return lpram_alloc_no_wait(n);
}
#else
#ifndef DOXYGEN_SHOULD_SKIP_THIS
void _SA_free(void *p, const char * file, unsigned line);
void * _SA_malloc(size_t n, const char * file, unsigned line);
void * _SA_realloc(void * p, size_t n, const char * file, unsigned line);
#endif
#define SA_free(p) _SA_free(p, __FILE__, __LINE__)
#define SA_malloc(n) _SA_malloc(n, __FILE__, __LINE__)
#define SA_realloc(p, n) _SA_realloc(p, n, __FILE__, __LINE__)
#endif

/* Declare internal secure string functions here. */
char* _SA_gets_s(char* str, size_t size);
int _SA_sprintf_s(char* buffer, size_t bufsz, const char* format, ...);
int _SA_sscanf_s(const char* buffer, const char* format, ...);
int _SA_vsnprintf_s(char *buffer, size_t bufsz, size_t count, const char *format, va_list arg);
int _SA_strcat_s(char* dest, size_t destsz, const char* src, const char* file, unsigned int line);
int _SA_strcpy_s(char* dest, size_t destsz, const char* src, const char* file, unsigned int line);
int _SA_strncat_s(char* dest, size_t destsz, const char* src, size_t count, const char* file, unsigned int line);
int _SA_strncpy_s(char *dest, size_t dmax, const char *src, size_t count, const char* file, unsigned int line);
size_t _SA_strnlen_s(const char* str, size_t size);
int _SA_memcpy_s(void* dest, size_t destsz, const void* src, size_t count, const char* file, unsigned int line);
char* _SA_strtok_s(char* str, size_t* strmax, const char* delim, char** context);

#if defined(__UEFI)
/** Secure function defines specific to UEFI go here. */
#define SA_gets_s(buff,size) fgets(buff,size,stdin)
#define SA_sprintf_s AsciiSPrint
#define SA_sscanf_s sscanf
#define SA_strcat_s AsciiStrCatS
#define SA_vsnprintf_s(buffer,buffsz,count,format,arglist) UefiAsciiVSPrint(buffer,buffsz,format,arglist)
#define SA_strncat_s AsciiStrnCatS
#define SA_strcpy_s AsciiStrCpyS
#define SA_strncpy_s AsciiStrnCpyS
#define SA_strnlen_s AsciiStrnLenS
#define SA_memcpy_s(dest,dsize,src,count) CopyMem(dest,src,count)
#define SA_strtok_s(str,strmax,delim,context) _SA_strtok_s(str,strmax,delim,context)
#elif (defined(SYSCORE_BMC) && !defined(USE_SECURE_STRING_FUNC)) || defined(SYSCORE_RAIDSTACK)
/** Secure function defines specific to BMC which are not planning to use secure string functions go here. */
#define SA_gets_s(buff,size)                             fgets(buff,size,stdin)
#define SA_sprintf_s(dest,dsize,format,...)              sprintf(dest,format,##__VA_ARGS__)
#define SA_sscanf_s(buffer,format,...)                   sscanf(buffer,format,##__VA_ARGS__)
#define SA_strcat_s(dest,destsz,src)                     strcat(dest,src)
#define SA_vsnprintf_s(dest,dsize,maxcount,format,arg)   vsnprintf(dest,dsize,format,arg)
#define SA_strncat_s(dest,destsz,src,count)              strncat(dest,src,count)
#define SA_strcpy_s(dest, destsz, src)                   strcpy(dest,src)
#define SA_strncpy_s(dest,destsz,src,count)              strncpy(dest,src,count)
#define SA_memcpy_s(dest,dsize,src,count)                memcpy(dest,src,count)
#define SA_strnlen_s(str,size)                           strlen(str)
#define SA_strtok_s(str,strmax,delim,context)            _SA_strtok_s(str,strmax,delim,context)
#else
/** Secure function defines for rest of the platforms are defined here. */
#define SA_gets_s(str,size)                                 _SA_gets_s(str,size)
#define SA_sprintf_s(buffer,bufsz,format,...)               _SA_sprintf_s(buffer,bufsz,format,##__VA_ARGS__)
#define SA_sscanf_s(buffer,format,...)                      _SA_sscanf_s(buffer,format,##__VA_ARGS__)
#define SA_vsnprintf_s(buffer,buffsz,count,format,arglist)  _SA_vsnprintf_s(buffer,buffsz,count,format,arglist)
#define SA_strcat_s(dest,destsz,src)                        _SA_strcat_s(dest,destsz,src,__FILE__,__LINE__)
#define SA_strncat_s(dest,dsize,src,count)                  _SA_strncat_s(dest,dsize,src,count,__FILE__,__LINE__)
#define SA_strcpy_s(dest,dsize,src)                         _SA_strcpy_s(dest,dsize,src,__FILE__,__LINE__)
#define SA_strncpy_s(dest,dmax,src,count)                   _SA_strncpy_s(dest,dmax,src,count,__FILE__,__LINE__)
#define SA_memcpy_s(dest,dsize,src,count)                   _SA_memcpy_s(dest,dsize,src,count,__FILE__,__LINE__)
#define SA_strnlen_s(str,size)                              _SA_strnlen_s(str,size)
#define SA_strtok_s(str,strmax,delim,context)               _SA_strtok_s(str,strmax,delim,context)
#endif

/** Return the smaller of 2 numeric values. */
#define SA_min(a,b) ((a)>(b) ? b : a)
/** Return the larger of 2 numeric values. */
#define SA_max(a,b) ((a)>(b) ? a : b)

/* *** Transport Support *** */

/** CISS transport for Smart Array, if supported by the driver */
#define ENABLE_CISS_TRANSPORT 1
/** SIS/FIB transport for Smart HBA, if supported by the driver */
#define ENABLE_SISFIB_TRANSPORT 1

#if defined(__cplusplus)
}    /* extern "C" { */
#endif

/** @} */

/** @} */

#endif /* PSYSC_PLATFORM_H */
