/** @file pstorc_utils.h
@copyright Copyright (c) 2021 Microchip Technology Inc. All Rights Reserved.

Description: storageCore utility functions header file

*/

#ifndef PSTORC_UTILS_H
#define PSTORC_UTILS_H

/************************//**
 * @addtogroup storcore
 * @{
 ***************************/

/****************************************//**
 * @defgroup storc_utils Utility
 * @{
 *******************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/** Returns @ref kTrue/@ref kFalse if C string `s` is non-empty and contains only digit characters. */
SA_BOOL SA_IsDigitString(const char * s);

/** Compare 2 C strings ignoring case.
 * Works exactly like C [strcmp](http://www.cplusplus.com/reference/cstring/strcmp/)
 * except case is ignored when comparing
 * @param[in] a  C string to compare.
 * @param[in] b  C string to compare.
 * @return  Returns an integral value indicating the relationship between the strings:
 * @return  <0 the first character that does not match has a lower value in a than in b
 * @return  0  the contents of both strings are equal
 * @return  >0 the first character that does not match has a greater value in a than in b.
 *
 * __Example__
 * @code{.c}
 * int main ()
 * {
 *   char str[][5] = { "R2D2" , "C3PO" , "r2A6" };
 *   int n;
 *   puts ("Looking for R2 astromech droids...");
 *   for (n=0 ; n<3 ; n++)
 *     if (stricmp (str[n],"r2d2") == 0)
 *     {
 *       printf ("found %s\n",str[n]);
 *     }
 *   return 0;
 * }
 * // Outputs:
 * // Looking for R2 astromech droids...
 * // found R2D2
 * @endcode
 */
int SA_stricmp(char const *a, char const *b);

/** Compare 2 C strings ignoring case.
 * Works exactly like C [strncmp](http://www.cplusplus.com/reference/cstring/strncmp/)
 * except case is ignored when comparing
 * @param[in] a  C string to compare.
 * @param[in] b  C string to compare.
 * @param[in] n  Max number of characters to compare.
 * @return  Returns an integral value indicating the relationship between the strings:
 * @return  <0 the first character that does not match has a lower value in a than in b
 * @return  0  the contents of both strings are equal
 * @return  >0 the first character that does not match has a greater value in a than in b.
 *
 * __Example__
 * @code{.c}
 * int main ()
 * {
 *   char str[][5] = { "R2D2" , "C3PO" , "r2A6" };
 *   int n;
 *   puts ("Looking for R2 astromech droids...");
 *   for (n=0 ; n<3 ; n++)
 *     if (strnicmp (str[n],"r2xx",2) == 0)
 *     {
 *       printf ("found %s\n",str[n]);
 *     }
 *   return 0;
 * }
 * // Outputs:
 * // Looking for R2 astromech droids...
 * // found R2D2
 * // found r2A6
 * @endcode
 */
int SA_strnicmp(char const *a, char const *b, size_t n);

/** Finds the first character equal to one of the characters in the given character sequence.
 *
 * @deprecated Use @ref SA_find_first_of_safe instead.
 * @attention This is an unsafe function which is a security risk. Please use the safe version of this function (@ref SA_find_first_of_safe).
 * The search considers only the interval [pos, SA_strlen(str)).
 * If the character is not present in the interval, (size_t)-1 will be returned.
 *
 * @param[in] str   NULL-terminated string to search in.
 * @param[in] chars NULL-terminated string identifying characters to search for.
 * @param[in] pos   Position at which to begin searching (i.e. 0 for start of `str`)
 * @return Position of the found character or (size_t)-1 if no such character is found.
 *
 * __Example__
 * @code{.c}
 * int main()
 * {
 *     // the test string
 *     const char str[] = "Hello World!";
 *
 *     // strings and chars to search for
 *     const char* search_cstr = "Good Bye!";
 *
 *     printf("%lu\n", SA_find_first_of(str, search_cstr, 0));
 *     // 'x' is not in "Hello World', thus it will return (size_t)-1
 *     printf("%lu\n", SA_find_first_of(str, "x", 0));
 *
 *    // Possible output:
 *    //
 *    // 1
 *    // 18446744073709551615
 * }
 * @endcode
 */
size_t SA_find_first_of(const char * str, const char * chars, size_t pos);

/** Finds the first character equal to one of the characters in the given character sequence.
 *
 * The search considers only the interval [pos, SA_strnlen_s(str, str_buffer_size)).
 * If the character is not present in the interval, (size_t)-1 will be returned.
 *
 * @param[in] str                   NULL-terminated string to search in.
 * @param[in] str_buffer_size       size of the str string buffer in bytes.
 * @param[in] chars                 NULL-terminated string identifying characters to search for.
 * @param[in] chars_buffer_size     size of the chars string buffer in bytes.
 * @param[in] pos                   Position at which to begin searching (i.e. 0 for start of `str`)
 * @return Position of the found character or (size_t)-1 if no such character is found.
 *
 * __Example__
 * @code{.c}
 * int main()
 * {
 *     // the test string
 *     const char str[] = "Hello World!";
 *
 *     // strings and chars to search for
 *     const char search_cstr[] = "Good Bye!";
 *
 *     printf("%lu\n", SA_find_first_of_safe(str, sizeof(str), search_cstr, sizeof(search_cstr), 0));
 *     // 'x' is not in "Hello World', thus it will return (size_t)-1
 *     printf("%lu\n", SA_find_first_of_safe(str, 265, "x", 2, 0));
 *
 *    // Possible output:
 *    //
 *    // 1
 *    // 18446744073709551615
 * }
 * @endcode
 */
size_t SA_find_first_of_safe(const char* str, size_t str_buffer_size, const char* chars, size_t chars_buffer_size, size_t pos);


/** Convert byte buffer to ASCII string.
 *
 * @param[in]  buffer_address        Buffer to convert to string.
 * @param[in]  buffer_size           Size of buffer_address in bytes.
 * @param[out] string_address        Address of ASCII string to create.
 * @param[in]  string_size           Size of string_address in bytes.
 * @param[in]  trim_leading_blanks   Trim leading whitespace characters from result string.
 * @param[in]  trim_trailing_blanks  Trim trailing whitespace characters from result string.
 * @param[in]  allow_non_alpha_num   If @ref kFalse, replace non-alpha-numeric characters with 'replace_char'.
 * @param[in]  allow_non_printable   If @ref kFalse, replace non-printable characters (see isprint()) with 'replace_char'.
 * @param[in]  replace_char          Replacement character to use when removing non-alpha-numeric or non-printable
 * characters where '\0' means to remove the characters without replacing.
 * @return Same address as string_buffer.
 */
char *__SA_ConvertCharBufferToString(const char *buffer_address, size_t buffer_size,
                                    char *string_address, size_t string_size,
                                    SA_BOOL trim_leading_blanks,
                                    SA_BOOL trim_trailing_blanks,
                                    SA_BOOL allow_non_alpha_num,
                                    SA_BOOL allow_non_printable,
                                    char replace_char);

/** Convert array of bytes to a string of hex characters.
 *
 * @deprecated Use @ref SA_ConvertBufferToHexStringSafe instead.
 * @warning This is an unsafe function which is a security risk. Please use the safe version of this function (@ref SA_ConvertBufferToHexStringSafe).
 * @pre `dest` is large enough to contain `(2*buffer_size)+1` characters.
 *
 * @param[out] dest         Allocated buffer to write hex string (and terminating NULL).
 * @param[in]  buffer       Array of bytes to read.
 * @param[in]  buffer_size  Number of bytes to read from `buffer` and write to `dest`.
 * @return NULL terminated string of ASCII hex characters, same as `dest`.
 *
 * __Examples__
 * @code{.c}
 * char buf[3] = { 0, 10, 255 };
 * char dest[7];
 * SA_ConvertBufferToHexString(dest, buf, 3); //=> "000AFF"
 * @endcode
 */
char * SA_ConvertBufferToHexString(char * dest, const void *buffer, size_t buffer_size);

/** Convert array of bytes to a string of hex characters.
 *
 * @pre `dest` is large enough to contain `(2*buffer_size)+1` characters.
 *
 * @param[out] dest                 Allocated buffer to write hex string (and terminating NULL).
 * @param[in]  dest_buffer_size     The size of the destination buffer.
 * @param[in]  buffer               Array of bytes to read.
 * @param[in]  buffer_size          Number of bytes to read from `buffer` and write to `dest`.
 * @return NULL terminated string of ASCII hex characters, same as `dest`.
 *
 * __Examples__
 * @code{.c}
 * char buf[3] = { 0, 10, 255 };
 * char dest[7];
 * SA_ConvertBufferToHexStringSafe(dest, sizeof(dest), buf, 3); //=> "000AFF"
 * @endcode
 */
char * SA_ConvertBufferToHexStringSafe(char * dest, size_t dest_buffer_size, const void *buffer, size_t buffer_size);

/** Convert number to string representing an Array ID.
 *
 * @deprecated Use @ref SA_ConvertNumberToArrayIDSafe instead.
 * @attention This is an unsafe function which is a security risk. Please use the safe version of this function (@ref SA_ConvertNumberToArrayIDSafe).
 * @pre param `buf` must large enough to contain Array ID and terminating NULL.
 *
 * @param[out] buf        String buffer large enough to contain Array ID and terminating NULL.
 * @param[in]  num        Number to convert.
 *
 * @return A pointer to the resulting null-terminated string, same as parameter buf.
 *
 * __Examples__
 * @code{.c}
 * char buf[3] = { 0 };
 * SA_NumberToArrayID(buf, 0);   //=> "A"
 * SA_NumberToArrayID(buf, 1);   //=> "B"
 * SA_NumberToArrayID(buf, 26);  //=> "AA"
 * SA_NumberToArrayID(buf, 27);  //=> "AB"
 * @endcode
 */
char * SA_ConvertNumberToArrayID(char * buf, SA_DWORD num);

/** Convert number to string representing an Array ID.
 *
 * @pre param `buf` must large enough to contain Array ID and terminating NULL.
 *
 * @param[out] buf        String buffer large enough to contain Array ID and terminating NULL.
 * @param[in]  buf_size   Size of buf.
 * @param[in]  num        Number to convert.
 *
 * @return A pointer to the resulting null-terminated string, same as parameter buf.
 *
 * __Examples__
 * @code{.c}
 * char buf[3] = { 0 };
 * SA_ConvertNumberToArrayIDSafe(buf, sizeof(buf), 0);  //=> "A"
 * SA_ConvertNumberToArrayIDSafe(buf, sizeof(buf), 1);  //=> "B"
 * SA_ConvertNumberToArrayIDSafe(buf, sizeof(buf), 26); //=> "AA"
 * SA_ConvertNumberToArrayIDSafe(buf, sizeof(buf), 27); //=> "AB"
 * @endcode
 */
char* SA_ConvertNumberToArrayIDSafe(char* buf, size_t buf_size, SA_DWORD num);

/** Count the number of set bits in a bit array
 *
 * @param[in] bit_array   Array of bits to count.
 *
 * @return The number of bits set in the bit array.
 *
 * __Examples__
 * @code{.c}
 * __SA_PopCount(0x00);    //=> "0"
 * __SA_PopCount(0x11);    //=> "2"
 * __SA_PopCount(0x21);    //=> "2"
 * __SA_PopCount(0x31);    //=> "3"
 * __SA_PopCount(0xFF);    //=> "8"
 * @endcode
 */
SA_WORD __SA_PopCount(SA_DWORD bit_array);

/** Count the number of set bits in an area of memory
* @attention Providing a NULL or empty buffer is undefined.
*
* @param[in] buffer           Pointer to an area of memory.
* @param[in] number_of_bytes  Number of bytes to count.
*
* @return The number of bits set in the specified area of memory.
*
* __Examples__
* @code{.c}
* unsigned char buffer[] = { 0x00, 0x02 };
* __SA_MemoryPopCount(buffer, 1);    //=> "0"
* __SA_MemoryPopCount(buffer, 2);    //=> "1"
* @endcode
*/
SA_DWORD __SA_MemoryPopCount(void *buffer, size_t number_of_bytes);

/** Convert QWORD (unsigned int64) number to __binary__ string.
 *
 * To store any size number the input buffer should be at least 65 bytes.
 * @pre param `buf` is large enough to store converted string with terminating NULL.
 *
 * @param[in]  num        Number to convert.
 * @param[out] buf        String buffer large enough to contain string and terminating NULL.
 *
 * @return A pointer to the resulting null-terminated string, same as parameter buf.
 *
 * __Examples__
 * @code{.c}
 * char buf[65] = { 0 };
 * SA_ultoba(0, buf);  //=> "0"
 * SA_ultoba(26, buf); //=> "11010"
 * @endcode
 */
char * SA_ultoba(SA_QWORD num, char *buf);

/** Convert QWORD (unsigned int64) number to __hex__ string.
 *
 * To store any size number the input buffer should be at least 17 bytes.
 * @pre param `buf` is large enough to store converted string with terminating NULL.
 *
 * @param[in]  num        Number to convert.
 * @param[out] buf        String buffer large enough to contain string and terminating NULL.
 *
 * @return A pointer to the resulting null-terminated string, same as parameter buf.
 *
 * __Examples__
 * @code{.c}
 * char buf[17] = { 0 };
 * SA_ultoxa(0, buf);  //=> "0"
 * SA_ultoxa(26, buf); //=> "1A"
 * @endcode
 */
char * SA_ultoxa(SA_QWORD num, char *buf);

/** Convert QWORD (unsigned int64) number to __octal__ string.
 *
 * To store any size number the input buffer should be at least 23 bytes.
 * @pre param `buf` is large enough to store converted string with terminating NULL.
 *
 * @param[in]  num        Number to convert.
 * @param[out] buf        String buffer large enough to contain string and terminating NULL.
 *
 * @return A pointer to the resulting null-terminated string, same as parameter buf.
 *
 * __Examples__
 * @code{.c}
 * char buf[23] = { 0 };
 * SA_ultooa(0, buf);  //=> "0"
 * SA_ultooa(26, buf); //=> "32"
 * @endcode
 */
char * SA_ultooa(SA_QWORD num, char *buf);

/** Convert QWORD (unsigned int64) number to string.
 *
 * To store any size number the input buffer should be at least 21 bytes.
 * @pre param `buf` is large enough to store converted string with terminating NULL.
 *
 * @param[in]  num        Number to convert.
 * @param[out] buf        String buffer large enough to contain string and terminating NULL.
 *
 * @return A pointer to the resulting null-terminated string, same as parameter buf.
 *
 * __Examples__
 * @code{.c}
 * char buf[21] = { 0 };
 * SA_ultoa(0, buf);  //=> "0"
 * SA_ultoa(26, buf); //=> "26"
 * @endcode
 */
char * SA_ultoa(SA_QWORD num, char *buf);

/** Convert signed 64bt integer number to string.
 *
 * If input is negative, the buffer should be bg enough to also contain
 * a '-' as the first character.
 *
 * To store any size number the input buffer should be at least 21 bytes.
 * @pre param `buf` is large enough to store converted string with terminating NULL.
 *
 * @param[in]  num        Number to convert.
 * @param[out] buf        String buffer large enough to contain string and terminating NULL.
 *
 * @return A pointer to the resulting null-terminated string, same as parameter buf.
 *
 * __Examples__
 * @code{.c}
 * char buf[21] = { 0 };
 * SA_ltoa(0, buf);   //=> "0"
 * SA_ltoa(26, buf);  //=> "26"
 * SA_ltoa(-26, buf); //=> "-26"
 * @endcode
 */
char * SA_ltoa(SA_INT64 num, char *buf);

/** Generate a 8bit CRC from the given buffer.
 * Polynomial: x^8+x^2+x+1
 * (same used in the MCTP over SMBus/I2C transport binding specification).
 *
 * @param[in]  buf   Data to create CRC for.
 * @param[in]  size  Size (in bytes) of `buf`.
 *
 * @return An 8bit CRC value.
 *
 * __Examples__
 * @code{.c}
 * char buf[32] = { 0 };
 * SA_crc8(buf, 32);    //=> ##
 * @endcode
 */
SA_UINT8 SA_crc8(const void* buf, SA_DWORD size);

/** Generate a 32bit CRC from the given buffer.
 * Polynomial: x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x+1
 * (same as the one used by IEEE 802.3).
 *
 * @param[in]  buf   Data to create CRC for.
 * @param[in]  size  Size (in bytes) of `buf`.
 *
 * @return A 32bit CRC value.
 *
 * __Examples__
 * @code{.c}
 * char buf[21] = { 0 };
 * SA_crc32(buf, 21);    //=> ##
 * @endcode
 */
SA_DWORD SA_crc32(const void* buf, SA_DWORD size);

/** Return GCD of a and b */
SA_QWORD SA_gcd(SA_QWORD a, SA_QWORD b);

/** Return LCM of two numbers. */
SA_QWORD SA_lcm(SA_QWORD a, SA_QWORD b);

/** Remove duplicate objects from an array.
 * @param[in,out] array  Array to sort.
 * @param[in,out] nitems Number of elments in array, returns number of items in uniq-ified array.
 * @param[in] size       Size (in bytes) of a single element in array.
 * @param[in] cmp        Function that can compare two elements in the array.
 *
 * __Example__
 * @code{.c}
 * int values[] = { 1, 1, 1, 3, 2, 3 };
 * size_t num_values = 6;
 *
 * int cmpfunc (const void * a, const void * b)
 * {
 *   return ( *(int*)a - *(int*)b );
 * }
 *
 * int main()
 * {
 *   int n;
 *
 *   printf("Before uniq-ifying the list is: \n");
 *   for( n = 0 ; n < num_values; n++ )
 *   {
 *     printf("%d ", values[n]);
 *   }
 *
 *   SA_uniq(values, &num_values, sizeof(int), cmpfunc);
 *
 *   printf("\nAfter uniq-ifying the list is: \n");
 *   for( n = 0 ; n < num_values; n++ )
 *   {
 *     printf("%d ", values[n]);
 *   }
 *
 *   return(0);
 * }
 * // Outputs:
 * // Before sorting the list is:
 * // 1 1 1 3 2 3
 * // After sorting the list is:
 * // 1 3 2
 * @endcode
 */
void SA_uniq(void *array, size_t *nitems, size_t size, int (*cmp)(void*,void*));

/** Check host system endianess.
 * @return @ref kTrue iff system is little endian, @ref kFalse otherwise.
 */
SA_BOOL SA_IsHostLittleEndian(void);

/** Reverse endianness of an 8-byte integer.
 */
SA_UINT64 SA_SwapInt64Endianness(SA_UINT64 value);

/** Convert input 8-byte integer to litte endian.
 */
SA_UINT64 SA_ConvertInt64ToLittleEndian(SA_UINT64 value);

/** Convert input 8-byte integer to big endian.
 */
SA_UINT64 SA_ConvertInt64ToBigEndian(SA_UINT64 value);

/** Reverse endianness of an 4-byte integer.
 */
SA_UINT32 SA_SwapInt32Endianness(SA_UINT32 value);

/** Convert input 4-byte integer to litte endian.
 */
SA_UINT32 SA_ConvertInt32ToLittleEndian(SA_UINT32 value);

/** Convert input 4-byte integer to big endian.
 */
SA_UINT32 SA_ConvertInt32ToBigEndian(SA_UINT32 value);

/** Reverse endianness of an 2-byte integer.
 */
SA_UINT16 SA_SwapInt16Endianness(SA_UINT16 value);

/** Convert input 2-byte integer to litte endian.
 */
SA_UINT16 SA_ConvertInt16ToLittleEndian(SA_UINT16 value);

/** Convert input 2-byte integer to big endian.
 */
SA_UINT16 SA_ConvertInt16ToBigEndian(SA_UINT16 value);

#if defined(__cplusplus)
}
#endif

/** @} */

/** @} */

#endif /* PSTORC_UTILS_H */

