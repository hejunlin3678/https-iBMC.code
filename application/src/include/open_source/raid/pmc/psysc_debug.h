/** @file psysc_debug.h
@copyright Copyright (c) 2021 Microchip Technology Inc. All Rights Reserved.

Description: sysCore header file

*/

#ifndef PSYSC_DEBUG_H
#define PSYSC_DEBUG_H

/** @addtogroup syscore sysCore
 * @{
 */

#include "psysc_types.h"

#if defined(__cplusplus)
extern "C" {
#endif

/** @addtogroup syscore_debug sysCore Debugging/Logging
 * @{
 */

/* Compile-time compilation conditions */
/* @def SC_DISABLE_DEBUGLOG
 * If defined, will remove all SC_DEBUGLOG* calls.
 * Setting this is the same as defining SC_MAX_DEBUGLOG_LEVEL to 0.
 */
/** @def SC_MAX_DEBUGLOG_LEVEL
 * @brief Maximum debug log level macros to keep at compile time.
 * Example, setting to 2 will compile-out SC_DEBUGLOG3 but leave SC_DEBUGLOG1/SC_DEBUGLOG2.
 * Setting this to 0 is the same as defining SC_DISABLE_DEBUGLOG.
 */

/** @def SC_DEBUGLOG_LEVEL
 * @brief Initial debug log level.
 */

#ifndef SC_DEBUGLOG_LEVEL
#if defined(_DEBUG)
#define SC_DEBUGLOG_LEVEL 2
#else
#define SC_DEBUGLOG_LEVEL 0
#endif
#endif

/** Max log level that a mask can uniquely apply.
 *
 * For example,
 * If '10', it means that log levels 1-10 can have uniquely set debug log masks
 * and log levels 11-n will all share the default log mask.
 */
#define kMaxDebugLogLevelMasks 10

/** @def SC_DEBUGLOG3
 * @brief Invoke callback with log message if @ref SC_DEBUGLOG_LEVEL >= 3.
 * @attention A newline is __NOT__ automatically appended to message.
 */
/** @def SC_DEBUGLOG2
 * @brief Invoke callback with log message if @ref SC_DEBUGLOG_LEVEL >= 2.
 * @attention A newline is __NOT__ automatically appended to message.
 */
/** @def SC_DEBUGLOG1
 * @brief Invoke callback with log message if @ref SC_DEBUGLOG_LEVEL >= 1.
 * @attention A newline is __NOT__ automatically appended to message.
 */
/** @def SC_CHECK
 * @brief Invoke callback with log message if condition fails on non-release builds.
 * @attention A newline __is__ automatically appended to message.
 */
/** @def IF_SC_CHECK
 * @brief Same as @ref SC_CHECK but also expects a continue block to exec if the condition passes.
 */
/** @def UNLESS_SC_CHECK
 * @brief Inverse of IF_SC_CHECK(): continue block executes AND SC_CHECK() is called if cond is 'false'
 */

#ifdef SC_MAX_DEBUGLOG_LEVEL
#  if SC_MAX_DEBUGLOG_LEVEL == 0
#    ifndef SC_DISABLE_DEBUGLOG
#      define SC_DISABLE_DEBUGLOG
#    endif
#  endif
#endif

#ifdef SC_DISABLE_DEBUGLOG
#  ifdef SC_MAX_DEBUGLOG_LEVEL
#    undef SC_MAX_DEBUGLOG_LEVEL
#  endif
#  define SC_MAX_DEBUGLOG_LEVEL 0
#else
#  ifndef SC_MAX_DEBUGLOG_LEVEL
#    define SC_MAX_DEBUGLOG_LEVEL 255
#  endif
#endif

#ifdef SC_DISABLE_DEBUGLOG
#  ifdef NO_VARIADIC
#    define SC_DEBUGLOG3
#    define SC_DEBUGDATABUFFER3
#    define SC_DEBUGLOG2
#    define SC_DEBUGDATABUFFER2
#    define SC_DEBUGLOG1
#    define SC_DEBUGDATABUFFER1
#    define SC_DEBUGLOG
#    define SC_DEBUGDATABUFFER
#    define SC_CHECK
#  else /* ! NO_VARIADIC */
#    define SC_DEBUGLOG3(...)
#    define SC_DEBUGDATABUFFER3(...)
#    define SC_DEBUGLOG2(...)
#    define SC_DEBUGDATABUFFER2(...)
#    define SC_DEBUGLOG1(...)
#    define SC_DEBUGDATABUFFER1(...)
#    define SC_DEBUGLOG(...)
#    define SC_DEBUGDATABUFFER(...)
#    define SC_CHECK(...)
#  endif /* NO_VARIADIC */
#  define IF_SC_CHECK(cond, mesg) if (cond)
#  define UNLESS_SC_CHECK(cond, mesg) if (!(cond))
#else /* ! SC_DISABLE_DEBUGLOG */

#  ifdef _DEBUG
    /** @def SC_DEBUGLOG_PREFIX_FMT
     * Prefix format string to use in calls to SC_DEBUG*.
     * Default: "%s:%d: " (for __FILE__, __LINE__)
     * Number of formatters must match number of arguments in @ref  SC_DEBUGLOG_PREFIX_ARGS.
     */
    /** @def SC_DEBUGLOG_PREFIX_ARGS
     * Values for prefix format string used in calls to SC_DEBUG*.
     * Default: __FILE__,__LINE__
     * @see SC_DEBUGLOG_PREFIX_FMT.
     */
#    ifndef  SC_DEBUGLOG_PREFIX_FMT
#      define SC_DEBUGLOG_PREFIX_FMT   "%s:%d: "
#    endif
#    ifndef  SC_DEBUGLOG_PREFIX_ARGS
#      define SC_DEBUGLOG_PREFIX_ARGS  __FILE__,__LINE__
#    endif
#  endif

#  if SC_MAX_DEBUGLOG_LEVEL >= 3
#    ifdef SC_DEBUGLOG_PREFIX_ARGS
#      define SC_DEBUGLOG3(fmt, ...) SC_DebugLog(3, 0, SC_DEBUGLOG_PREFIX_FMT fmt, SC_DEBUGLOG_PREFIX_ARGS, ##__VA_ARGS__)
#    else
#      define SC_DEBUGLOG3(fmt, ...) SC_DebugLog(3, 0, fmt, ##__VA_ARGS__)
#    endif
#    define SC_DEBUGDATABUFFER3(buf, n) SC_LogDataBuffer(3, 0, buf, n)
#  else
#    ifdef NO_VARIADIC
#      define SC_DEBUGLOG3
#      define SC_DEBUGDATABUFFER3
#    else
#      define SC_DEBUGLOG3(...)
#      define SC_DEBUGDATABUFFER3(...)
#    endif
#  endif
#  if SC_MAX_DEBUGLOG_LEVEL >= 2
#    ifdef SC_DEBUGLOG_PREFIX_ARGS
#      define SC_DEBUGLOG2(fmt, ...) SC_DebugLog(2, 0, SC_DEBUGLOG_PREFIX_FMT fmt, SC_DEBUGLOG_PREFIX_ARGS, ##__VA_ARGS__)
#    else
#      define SC_DEBUGLOG2(fmt, ...) SC_DebugLog(2, 0, fmt, ##__VA_ARGS__)
#    endif
#    define SC_DEBUGDATABUFFER2(buf, n) SC_LogDataBuffer(2, 0, buf, n)
#  else
#    ifdef NO_VARIADIC
#      define SC_DEBUGLOG2
#      define SC_DEBUGDATABUFFER2
#    else
#      define SC_DEBUGLOG2(...)
#      define SC_DEBUGDATABUFFER2(...)
#    endif
#  endif
#  if SC_MAX_DEBUGLOG_LEVEL >= 1
#    ifdef SC_DEBUGLOG_PREFIX_ARGS
#      define SC_DEBUGLOG1(fmt, ...) SC_DebugLog(1, 0, SC_DEBUGLOG_PREFIX_FMT fmt, SC_DEBUGLOG_PREFIX_ARGS, ##__VA_ARGS__)
#    else
#      define SC_DEBUGLOG1(fmt, ...) SC_DebugLog(1, 0, fmt, ##__VA_ARGS__)
#    endif
#    define SC_DEBUGDATABUFFER1(buf, n) SC_LogDataBuffer(1, 0, buf, n)
#    define SC_CHECK(cond, fmt, ...) if (!(cond)) { SC_DebugLog(1, 0, "%s:%d: Assertion `"#cond"' failed: " fmt "\n",__FUNCTION__,__LINE__,##__VA_ARGS__); }
#    define IF_SC_CHECK(cond, mesg) SC_CHECK(cond, mesg); if (cond)
#    define UNLESS_SC_CHECK(cond, mesg) SC_CHECK(cond, mesg); if (!(cond))
#  else
#    ifdef NO_VARIADIC
#      define SC_DEBUGLOG1
#      define SC_DEBUGDATABUFFER1
#      define SC_CHECK
#    else
#      define SC_DEBUGLOG1(...)
#      define SC_DEBUGDATABUFFER1(...)
#      define SC_CHECK(...)
#    endif
#    define IF_SC_CHECK(cond, mesg) if (cond)
#    define UNLESS_SC_CHECK(cond, mesg) if (!(cond))
#  endif
#  ifdef SC_DEBUGLOG_PREFIX_ARGS
#    define SC_DEBUGLOG(level, fmt, ...) SC_DebugLog(level, 0, SC_DEBUGLOG_PREFIX_FMT fmt, SC_DEBUGLOG_PREFIX_ARGS, ##__VA_ARGS__)
#  else
#    define SC_DEBUGLOG(level, fmt, ...) SC_DebugLog(level, 0, fmt, ##__VA_ARGS__)
#  endif
#  define SC_DEBUGDATABUFFER(level, buf, n) SC_LogDataBuffer(level, 0, buf, n)
#endif

/** @name Evaluate Assertion
 * If the argument expression of this macro with functional form compares
 * equal to zero (i.e., the expression is false), a message is written to the
 * standard error device and abort is called, terminating the program execution.
 *
 * The specifics of the message shown depend on the particular library
 * implementation.
 * @{
 */
void __SC_assert(const char *expression, const char *message, const char *file, unsigned int line);
#define _SC_assert(expression, message)\
      if(!(expression)) { __SC_assert(#expression, message, __FILE__,__LINE__); }

#if defined(_DEBUG)
#  define SC_assert(expression, message) _SC_assert(expression, message)
#else
#  define SC_assert(expression, message) SC_CHECK(expression, "%s", message)
#endif
/** @} */ /* Evaluation Assertion */

/** Get current default debug log level.
 *
 * @warning Do not call this function from within the registered debug callback.
 *
 * @return Current default debug log level.
 */
SA_BYTE SC_GetDebugLogLevel(void);

/** Set runtime debug log level.
 * All debug messages with level higher then the current debug log level will be
 * sent to the registered debug callback.
 *
 * The default/starting mask is 0xFFFFFFFF.
 *
 * @warning Do not call this function from within the registered debug callback.
 *
 * @param[in] level  Desired debug log level.
 * @return Previous debug log level.
 */
SA_BYTE SC_SetDebugLogLevel(SA_BYTE level);

/** Get current default debug log mask.
 *
 * This applies to debug log levels in the range [@ref kMaxDebugLogLevelMasks+1, n].
 *
 * @warning Do not call this function from within the registered debug callback.
 *
 * @return Current default debug log mask set for log levels [@ref kMaxDebugLogLevelMasks + 1, n].
 */
SA_UINT32 SC_GetDebugLogMask(void);

/** Get current debug log mask for a given log level.
 *
 * @warning Do not call this function from within the registered debug callback.
 *
 * @return Current debug log mask set for log level `level`.
 * @return If `level` is not within the range [1, @ref kMaxDebugLogLevelMasks],
 * returns same as @ref SC_GetDebugLogMask.
 */
SA_UINT32 SC_GetDebugLogLevelMask(SA_BYTE level);

/** Set runtime default debug log mask.
 *
 * All debug messages that intersect with the desired mask will be sent to the
 * registered debug callback.
 *
 * Setting the mask to 0 effectively disables all debug log levels unless set with
 * a unique mask using @ref SC_SetDebugLogLevelMask.
 *
 * This applies to ALL debug log levels and overrides any mask set for log levels
 * [1, @ref kMaxDebugLogLevelMasks].
 *
 * This should be set first before using @ref SC_SetDebugLogLevelMask to set unique
 * masks for log levels [1, @ref kMaxDebugLogLevelMasks].
 *
 * @warning Do not call this function from within the registered debug callback.
 *
 * @see SC_SetDebugLogLevelMask.
 *
 * @param[in] mask  Desired debug log mask.
 * @return Previous default debug log mask.
 */
SA_UINT32 SC_SetDebugLogMask(SA_UINT32 mask);

/** Set runtime debug log mask for a given log level.
 *
 * All debug messages that intersect with with desired mask will be sent to the
 * registered debug callback.
 *
 * A uniq mask can only be set for log levels [1, @ref kMaxDebugLogLevelMasks].
 *
 * Setting the mask to 0 effectively disables the given debug log level (within
 * the allowed range).
 *
 * @warning Do not call this function from within the registered debug callback.
 *
 * @param[in] level Desired debug log level to apply mask (between [1, @ref kMaxDebugLogLevelMasks]).
 * @param[in] mask  Desired debug log mask.
 * @return Previous debug log mask.
 * @return 0 if `level` is outside the range [1, @ref kMaxDebugLogLevelMasks].
 */
SA_UINT32 SC_SetDebugLogLevelMask(SA_BYTE level, SA_UINT32 mask);

/** Write formatted log message to registered callback (if compiled @ref SC_DEBUGLOG_LEVEL is high enough).
 *
 * @warning Do not call this function from within the registered debug callback.
 *
 * @param[in] level  Log level of message.
 * @param[in] mask   Log mask for this message (0 means ignore the registered mask if non-zero).
 * @param[in] fmt    C-string describing format of message.
 *
 * @see SA_SetDebugLogLevel
 * @see SA_SetDebugLogMask
 *
 * Example
 * -------
 * @code
 * #define DBG_MOD1 0x00000001
 * #define DBG_MOD2 0x00000002
 * #define DBG_MOD3 0x00000004
 *
 * SC_SetDebugLogLevel(2);
 * SC_SetDebugLogMask(DBG_MOD1 | DBG_MOD3);
 * SC_SetDebugLogLevelMask(1, DBG_MOD2);
 * // Log level 1 will have mask DBG_MOD2 while all other log levels will use
 * // mask (DBG_MOD1 | DBG_MOD2).
 *
 * SC_DebugLog(2, DBG_MOD1, "[WARNING] %s\n", "reason...");
 * // Calls the registered callback with message "[WARNING] reason...\n"
 * // but only if DBG_MOD1 is included in the debug mask (which it is)
 * // and @ref SC_DEBUGLOG_LEVEL was set to value >= 2 (which it is).
 *
 * SC_DebugLog(1, DBG_MOD2, "[WARNING] %s\n", "reason...");
 * // Will be logged since DBG_MOD2 is set for debug log level 1.
 *
 * SC_DebugLog(1, DBG_MOD1, "[WARNING] %s\n", "reason...");
 * // Will NOT be logged since DBG_MOD1 is not set for debug log level 1.
 *
 * SC_DebugLog(1, 0xFFFFFFFF, "[WARNING] %s\n", "reason...");
 * // Will be logged as long as the debug log mask is not set to 0.
 *
 * SC_DebugLog(1, 0, "[WARNING] %s\n", "reason...");
 * // Will be logged regardless of the current debug log mask (since it's non-zero).
 *
 * SC_SetDebugLogLevelMask(1, 0);
 * // Disable log level 1.
 *
 * SC_DebugLog(1, 0, "[WARNING] %s\n", "reason...");
 * // Will be not be logged because the set mask for log level 1 is now 0.
 *
 * SC_DebugLog(3, 0, "[WARNING] %s\n", "reason...");
 * // Will be not be logged because the current debug log level is 2.
 * @endcode
 */
void SC_DebugLog(SA_BYTE level, SA_UINT32 mask, const char * fmt, ...);

/** Invoke debug callback with an array of memory into a fixed-format table (if SC_DEBUGLOG_LEVEL is high enough).
 *
 * @warning Do not call this function from within the registered debug callback.
 *
 * The central base column shows the hexadecimal buffer offset
 * at which the row starts, and to which the offsets at the top
 * of the table are added, making it easy to find the data
 * associated with a given byte offset: the last hex digit of
 * an offset identifies a column while the leading hex digits
 * identify a row.
 *
 * Data on the left side of the central base offset column is in
 * hexadecimal and in decreasing byte offset order going left to
 * right, to match what you would normally see when printing out,
 * for instance, a quadword integer in which bytes appear in
 * decreasing address order going left to right.
 *
 * Data to the right of the central base offset column is in ASCII
 * and in increasing byte offset order going left to right to match
 * what you would normally see when printing out a string in which
 * bytes appear in increasing address order going left to right.
 *
 * ASCII data that is not printable is represented by a period, so
 * when you see white space it will normally map to a space on the
 * hex side - the exception being trailing white space, which on
 * each side simply represents data past the end of the buffer.
 *
 * @param[in] level        Log level of message (see @ref SC_DebugLog).
 * @param[in] mask         Log mask for this message (0 means ignore registered mask if non-zero) (see @ref SC_DebugLog).
 * @param[in] data_buffer  Data buffer
 * @param[in] size         Number of bytes to write
 *
 * @see SC_DebugLog.
 *
 * Example:
 * --------
 * @code
 * #define DBG_MOD1 0x00000001
 *
 * char * data;
 * SC_LogDataBuffer(1, DBG_MOD1, data, sizeof(data));
 *
 * 0F 0E 0D 0C OB 0A 09 08 07 06 05 04 03 02 01 00 |      | 01234567898BCDEF
 * ----------------------------------------------- + ---- + ----------------
 * 20 20 20 20 20 20 50 48 02 10 00 8B 12 06 00 00 | 0000 | ........HP
 * 20 20 20 20 20 54 59 4A 44 46 30 30 32 31 47 45 | 0010 | EG1200FDJYT
 * @endcode
 */
void SC_LogDataBuffer(SA_BYTE level, SA_UINT32 mask, const void *data_buffer, size_t size);

/** Type for debug/logging callback used for @ref SC_SetDebugCallback.
 *
 * @warning None of the debug functions should be called from within this callback's
 * implementation.
 *
 * @param[in] level  See [Debug Logging Levels](@ref syscore_debug).
 * @param[in] mask   Debug log mask that matched for message (@ref syscore_debug).
 * @param[in] mesg   C-string debug message.
 */
typedef void(*DebugCallbackFunction)(SA_BYTE level,SA_UINT32 mask,const char * mesg);

/** Register a callback for debug log messages.
 *
 * @warning Do not call this function from within the registered debug callback.
 *
 * @param[in] callback Address to callback function (if NULL, logging is disabled).
 * Log messages less-than-or-equal to the current log level
 * will be sent to the callback.
 * @return The previously registered debug callback.
 *
 * Example
 * -------
 * @code
 * void my_log_function(SA_BYTE level, SA_UINT32 mask, const char * mesg)
 * {
 *    if (level == 1)
 *       fprintf(stdout, "%s\n", mesg);
 *    else
 *       fprintf(stderr, "%s\n", mesg);
 * }
 *
 * SC_SetDebugCallback(my_log_function);
 * @endcode
 */
DebugCallbackFunction SC_SetDebugCallback(DebugCallbackFunction callback);

/** @} */

#if defined(__cplusplus)
}    /* extern "C" { */
#endif

/** @} */

#endif /* PSYSC_DEBUG_H */
