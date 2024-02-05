/***************************************************************
 * Copyright (c) Huawei Technologies Co., Ltd. 2006-2019. All rights reserved.
FileName: ipsi_string.h
Author: Ashwini A
Version:1
Description: This file contains all the string related functions.
Create: 2006-12-29
Function List: ipsi_memcmp
ipsi_strncmp
ipsi_strlen
ipsi_strstr
ipsi_strrchr
ipsi_strchr
ipsi_strcmp
ipsi_strnicmp
ipsi_stricmp
ipsi_memchr
ipsi_strtouint32
ipsi_strcasecmp
ipsi_stristr

*****************************************************************/

#ifndef __IPSI_STRING_H__
#define __IPSI_STRING_H__

#include "ipsi_types.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif
/*
 * @defgroup OSAL
 * This section contains the descriptions of all APIs in OSAL.
 */
/*
 * @defgroup ipsi_string_Functions
 * @ingroup OSAL
 * This section contains all the string related functions. .
 *
 */
/*
 * @defgroup ipsi_memcmp
 * @ingroup ipsi_string_Functions
 * @par Prototype
 * @code
 * int ipsi_memcmp(const void *pBuf1, const void *pBuf2, size_t uiCount)
 * @endcode
 *
 * @par Purpose
 * This function is used to compare the characters.
 *
 * @par Description
 * ipsi_memcmp function compares the characters in two buffers.
 *
 * @param[in] pBuf1 First buffer. [NA/NA]
 * @param[in] pBuf2 Second buffer. [NA/NA]
 * @param[in] uiCount Number of characters. [NA/NA]
 * @param[out] NA NA [NA/NA]
 *
 * @retval NA The return value indicates the relationship between the buffers
 * as follows:\n
 *   -buf1 less than buf2.\n
 *   -buf1 identical to buf2. \n
 *   -buf1 greater than buf2. [NA|NA]
 *
 * @par Required Header File
 * ipsi_string.h
 *
 * @par Note
 * \n
 * NA
 *
 * @par Related Topics
 * NA
 */
IPSILINKDLL int ipsi_memcmp(const void *pBuf1, const void *pBuf2, size_t uiCount);

/*
 * @defgroup ipsi_strncmp
 * @ingroup ipsi_string_Functions
 * @par Prototype
 * @code
 * int ipsi_strncmp(const char *pcString1, const char
 *                  *pcString2, size_t uiCount)
 * @endcode
 *
 * @par Purpose
 * This function is used to compare the characters in two strings.
 *
 * @par Description
 * ipsi_strncmp function compares the characters of two strings.
 *
 * @param[in] pcString1 String to compare. [NA/NA]
 * @param[in] pcString2 String to compare. [NA/NA]
 * @param[in] uiCount Number of characters. [NA/NA]
 * @param[out] NA NA [NA/NA]
 *
 * @retval NA The return value indicates the relation between the substrings of
 * string1 and string2 as follows:\n
 *   -string1 substring is less than string2 substring. \n
 *   -string1 substring is identical to string2 substring.\n
 *   -string1 substring is greater than string2 substring.  [NA|NA]
 *
 * @par Required Header File
 * ipsi_string.h
 *
 * @par Note
 * \n
 * NA
 *
 * @par Related Topics
 * NA
 */
IPSILINKDLL int ipsi_strncmp(const char *pcString1, const char *pcString2, size_t uiCount);

/*
 * @defgroup ipsi_strlen
 * @ingroup ipsi_string_Functions
 * @par Prototype
 * @code
 * unsigned int ipsi_strlen(const char *pcString)
 * @endcode
 *
 * @par Purpose
 * This function is used to get the length of a string.
 *
 * @par Description
 * ipsi_strlen function returns the length of a string.
 *
 * @param[in] pcString Null-terminated string. [NA/NA]
 * @param[out] NA NA [NA/NA]
 *
 * @retval NA This function returns the number of characters
 * in string, excluding the terminal NULL. No return value is
 * reserved to indicate the error. [NA|NA]
 *
 * @par Required Header File
 * ipsi_string.h
 *
 * @par Note
 * \n
 * NA
 *
 * NA
 */
IPSILINKDLL unsigned int ipsi_strlen(const char *pcString);

/*
 * @defgroup ipsi_strnlen
 * @ingroup ipsi_string_Functions
 * @par Prototype
 * @code
 * unsigned int ipsi_strnlen(const char *pcString, unsigned int uiCount)
 * @endcode
 *
 * @par Purpose
 * This function is used to get the length of a string.
 *
 * @par Description
 * Obtains the length of the specified string pcString.
 * If the length of the input string is greater than the specified maximum length uiCount,
 * the maximum length is returned. Otherwise, the actual length of the string is returned.
 *
 * @param[in] pcString Null-terminated string. [NA/NA]
 * @param[in] uiCount Maximum length. 0～4294967295 [NA/NA]
 * @param[out] If the operation is successful, the value is returned.
 * If the input string length is greater than the maximum length specified by uiCount,
 * the maximum length is returned. Otherwise, the actual length of the string is returned.
 * If the input string length fails to be entered, the value 0 is returned.[NA/NA]
 *
 * @retval NA This function returns the number of characters
 * in string, excluding the terminal NULL. No return value is
 * reserved to indicate the error. [NA|NA]
 *
 * @par Required Header File
 * ipsi_string.h
 *
 * @par Note
 * \n
 * NA
 *
 * NA
 */
IPSILINKDLL unsigned int ipsi_strnlen(const char *pcString, unsigned int uiCount);

/*
 * @defgroup ipsi_strstr
 * @ingroup ipsi_string_Functions
 * @par Prototype
 * @code
 * const char *ipsi_strstr(const char *pcString, const char *pcStrCharSet)
 * @endcode
 *
 * @par Purpose
 * This function is used to search a string.
 *
 * @par Description
 * ipsi_strstr function finds for a substring.
 *
 * @param[in] pcString Null-terminated string to search. [NA/NA]
 * @param[in] pcStrCharSet Null-terminated character set string to search.
 * [NA/NA]
 * @param[out] NA NA [NA/NA]
 *
 * @retval NA This function returns a pointer to the first
 * occurrence of Strcharset in string or NULL,
 * if strCharSet does not appear in string. If Strcharset points to
 * a string of zero length, the function returns a string. [NA|NA]
 *
 * @par Required Header File
 * ipsi_string.h
 *
 * @par Note
 * \n
 * NA
 *
 * @par Related Topics
 * NA
 */
IPSILINKDLL const char *ipsi_strstr(const char *pcString, const char *pcStrCharSet);

/*
 * @defgroup ipsi_stristr
 * @ingroup ipsi_string_Functions
 * @par Prototype
 * @code
 * const char *ipsi_stristr(const char *pcString,
 *                           const char *pcStrcharset)
 * @endcode
 *
 * @par Purpose
 * This function is used to search a string by ignoring the case.
 *
 * @par Description
 * ipsi_stristr function finds for a substring by ignoring the case.
 *
 * @param[in] pcString Null-terminated string to search. [NA/NA]
 * @param[in] pcStrcharset Null-terminated character set string to search.
 * [NA/NA]
 * @param[out] NA NA [NA/NA]
 *
 * @retval NA This function returns a pointer to the first
 * occurrence of Strcharset in string or NULL,
 * if strCharSet does not appear in string. If Strcharset points to
 * a string of zero length, the function returns a string. [NA|NA]
 *
 * @par Required Header File
 * ipsi_string.h
 *
 * @par Note
 * \n
 * NA
 *
 * @par Related Topics
 * NA
 */
IPSILINKDLL const char *ipsi_stristr(const char *pcString, const char *pcStrcharset);

/*
 * @defgroup ipsi_strrchr
 * @ingroup ipsi_string_Functions
 * @par Prototype
 * @code
 * char *ipsi_strrchr( const char *pcString, int iCh )
 * @endcode
 *
 * @par Purpose
 * This function is used to scan a string for the last occurrence of a
 * character.
 *
 * @par Description
 * ipsi_strrchr function finds for last occurrence of a
 * character.
 *
 * @param[in] pcString Null-terminated string to search. [NA/NA]
 * @param[in] iCh      Character to be located.[NA/NA]
 *
 * @retval This function returns a pointer to the last occurrence of iCh in
 * pcString, or NULL if iCh is not found.
 *
 * @par Required Header File
 * ipsi_string.h
 *
 * @par Note
 * \n
 * NA
 *
 * @par Related Topics
 * NA
 */
IPSILINKDLL char *ipsi_strrchr(const char *pcString, int iCh);

/*
 * @defgroup ipsi_strchr
 * @ingroup ipsi_string_Functions
 * @par Prototype
 * @code
 * char *ipsi_strchr( const char *pcString, int iCh )
 * @endcode
 *
 * @par Purpose
 * This function is used to scan a string for the first occurrence of a
 * character.
 *
 * @par Description
 * ipsi_strrchr function finds for first occurrence of a
 * character.
 *
 * @param[in] pcString Null-terminated string to search. [NA/NA]
 * @param[in] iCh      Character to be located.[NA/NA]
 *
 * @retval This function returns a pointer to the first occurrence of iCh in
 * pcString, or NULL if iCh is not found.
 *
 * @par Required Header File
 * ipsi_string.h
 *
 * @par Note
 * \n
 * NA
 *
 * @par Related Topics
 * NA
 */
IPSILINKDLL char *ipsi_strchr(const char *pcString, int iCh);

/*
 * @defgroup ipsi_strcmp
 * @ingroup ipsi_string_Functions
 * @par Prototype
 * @code
 * int ipsi_strcmp(const char *pcString1, const char *pcString2)
 * @endcode
 *
 * @par Purpose
 * This function is used to compare the characters in two strings.
 *
 * @par Description
 * ipsi_strcmp function compares the characters of two strings.
 *
 * @param[in] pcString1 String to compare. [NA/NA]
 * @param[in] pcString2 String to compare. [NA/NA]
 * @param[out] NA NA [NA/NA]
 *
 * @retval NA The return value indicates the relation between the substrings of
 * string1 and string2 as follows:\n
 *   -string1 substring is less than string2 substring. \n
 *   -string1 substring is identical to string2 substring.\n
 *   -string1 substring is greater than string2 substring.  [NA|NA]
 *
 * @par Required Header File
 * ipsi_string.h
 *
 * @par Note
 * \n
 * NA
 *
 * @par Related Topics
 * NA
 */
IPSILINKDLL int ipsi_strcmp(const char *pcString1, const char *pcString2);

/*
 * @defgroup ipsi_memchr
 * @ingroup ipsi_string_Functions
 * @par Prototype
 * @code
 * void *ipsi_memchr(const void *pBuf, int character, size_t uiCount)
 * @endcode
 *
 * @par Purpose
 * This function is used to find characters in a buffer.
 *
 * @par Description
 * ipsi_memchr function finds characters in a buffer.
 *
 * @param[in] pBuf Pointer to buffer. [NA/NA]
 * @param[in] character Character to look for. [NA/NA]
 * @param[in] uiCount Number of characters to check [NA/NA]
 * @param[out] NA NA [NA/NA]
 *
 * @retval NA If successful, returns a pointer to first location of
 * character in buffer. Otherwise, returns NULL.
 *
 * @par Required Header File
 * ipsi_string.h
 *
 * @par Note
 * \n
 * NA
 *
 * @par Related Topics
 * NA
 */
IPSILINKDLL void *ipsi_memchr(const void *pBuf, int character, size_t uiCount);

/*
 * @defgroup ipsi_strnicmp
 * @ingroup ipsi_string_Functions
 * @par Prototype
 * @code
 * int ipsi_strnicmp(const char *pcString1, const char *pcString2,
 *                   size_t uiCount)
 * @endcode
 *
 * @par Purpose
 * This function is used to compare characters in two strings without
 * regard to case.
 *
 * @par Description
 * ipsi_strnicmp function used to compare characters in two strings
 * without regard to case.
 *
 * @param[in] pcString1 String to compare. [NA/NA]
 * @param[in] pcString2 String to compare. [NA/NA]
 * @param[in] uiCount Number of characters to compare [NA/NA]
 * @param[out] NA NA [NA/NA]
 *
 * @retval NA The return value indicates the relation between the
 * substrings of string1 and string2 as follows:\n
 * <0 -string1 substring is less than string2 substring. \n
 * 0 -string1 substring is identical to string2 substring.\n
 * >0 -string1 substring is greater than string2 substring.  [NA|NA]
 *
 * @par Required Header File
 * ipsi_string.h
 *
 * @par Note
 * \n
 * NA
 *
 * @par Related Topics
 * NA
 */
IPSILINKDLL int ipsi_strnicmp(const char *pcString1, const char *pcString2, size_t uiCount);

/*
 * @defgroup ipsi_stricmp
 * @ingroup ipsi_string_Functions
 * @par Prototype
 * @code
 * int ipsi_stricmp(const char *pcString1, const char *pcString2)
 * @endcode
 *
 * @par Purpose
 * This function is used to perform a lowercase comparison of strings.
 *
 * @par Description
 * ipsi_stricmp function used to perform a lowercase comparison of strings.
 *
 * @param[in] pcString1 String to compare. [NA/NA]
 * @param[in] pcString2 String to compare. [NA/NA]
 * @param[out] NA NA [NA/NA]
 *
 * @retval NA The return value indicates the relation between the substrings of
 * string1 and string2 as follows:\n
 * <0 -string1 substring is less than string2 substring. \n
 * 0 -string1 substring is identical to string2 substring.\n
 * >0 -string1 substring is greater than string2 substring.  [NA|NA]
 *
 * @par Required Header File
 * ipsi_string.h
 *
 * @par Note
 * \n
 * NA
 *
 * @par Related Topics
 * NA
 */
IPSILINKDLL int ipsi_stricmp(const char *pcString1, const char *pcString2);

/*
 * @defgroup ipsi_strtouint32
 * @ingroup ipsi_string_Functions
 * @par Prototype
 * @code
 * long ipsi_strtouint32(const char *string, int *piNum)
 * @endcode
 *
 * @par Purpose
 * This function is used to convert strings to int.
 *
 * @par Description
 * ipsi_strtouint32 function used to convert strings to int.
 *
 * @param[in] string String to be converted. [NA/NA]
 * @param[out] piNum pointer to be updated with output. [NA/NA]
 *
 * @retval NA returns 0 in case of success. -1 in case of failure.
 *
 * @par Required Header File
 * ipsi_string.h
 *
 * @par Note
 * \n
 * NA
 *
 * @par Related Topics
 * NA
 */
IPSILINKDLL long ipsi_strtouint32(const char *string, int *piNum);

/*
    @brief          ipsi_filename_comparison : used to perform a lowercase
                        comparison of filename strings.This is because in unix
                                   the lowercase and upper case filenames are
                                   different but in windows, those are same.

    @param          pcString1, pcString2        Strings to compare

    @retval         The return value indicates the relation of the substrings
                    of string1 and string2 as follows.
                    < 0 string1 substring less than string2 substring
                    0 string1 substring identical to string2 substring
                    > 0 string1 substring greater than string2 substring
*/
IPSILINKDLL int ipsi_filename_comparison(const char *pcString1, const char *pcString2);

/*
    @brief          ipsi_strcasecmp : used to compare two strings without case.

    @param          pcStr1 - First String to be compared
                    pcStr2 - Second String to be compared

    @retval         returns 0 in case of match. else difference between the two.
*/
IPSILINKDLL int ipsi_strcasecmp(const char *pcStr1, const char *pcStr2);

#ifdef __cplusplus
}
#endif

#endif
