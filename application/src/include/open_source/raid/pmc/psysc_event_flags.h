/** @file psysc_event_flags.h

@copyright Copyright (c) 2014-2017 Microsemi Corporation. All Rights Reserved.

Description: sysCore event flags header

*/

#ifndef PSYSC_EVENT_FLAGS_H
#define PSYSC_EVENT_FLAGS_H

/** @addtogroup syscore
 * @{
 */

/** @defgroup syscore_sync Synchronization
 * @brief
 * @details
 * @{
 */

#if defined(__cplusplus)
extern "C" {
#endif

#ifdef SC_DISABLE_EVENT_FLAGS

#ifdef NO_VARIADIC
#  define SC_EventFlagsCreate
#  define SC_EventFlagsDelete
#  define SC_EventFlagsGet
#  define SC_EventFlagsSet
#else
#  define SC_EventFlagsCreate(...)
#  define SC_EventFlagsDelete(...)
#  define SC_EventFlagsGet(...)
#  define SC_EventFlagsSet(...)
#endif

#else /* !SC_DISABLE_EVENT_FLAGS */

#ifndef DOXYGEN_SHOULD_SKIP_THIS
typedef void* PEVENT_FLAGS;
#endif

/** @defgroup syscore_sync_event_flags Event Flags
 * @{
 */

/** @name Create/Initialize/Destroy Event Flag Groups
 * @pre User must initialize an event flag group using @ref SC_EventFlagsCreate before using it.
 * @post Before event flag group leaves scope, the user should cleanup & free the event flag group using @ref SC_EventFlagsDelete.
 * @attention Re-creating an event flag before destroying it will leak memory.
 * @attention Using an event flag that has not been initialized or has been destroyed is undefined.
 * @attention Destroying an event flag while it is being waited on is undefined.
 *
 * Example:
 * @code
 * PEVENT_FLAGS event_flags;
 * if (SC_EventFlagsCreate(&event_flags))
 * {
 *    // Set event flags...
 *    SC_EventFlagsCreate(event_flags);
 * }
 * else
 * {
 *   fprintf(stderr, "Failed to create/initialized event flags\n");
 * }
 * @endcode
 * @{ */
#ifndef DOXYGEN_SHOULD_SKIP_THIS
/** Should not be call directly; instead invoke @ref SC_EventFlagsCreate. */
SA_BOOL _SC_EventFlagsCreate(PEVENT_FLAGS *event_flags);
/** Should not be call directly; instead invoke @ref SC_EventFlagsDelete. */
SA_BOOL _SC_EventFlagsDelete(PEVENT_FLAGS event_flags);
#endif
/** Create/initialize an event flag group. (see [Create/Initialize/Destroy  Event Flag Groups](@ref syscore_sync_event_flags)). */
#define SC_EventFlagsCreate(e) _SC_EventFlagsCreate(e)
/** Destroy & free an event flag. (see [Create/Initialize/Destroy Event Flag Groups](@ref syscore_sync_event_flags)). */
#define SC_EventFlagsDelete(e) _SC_EventFlagsDelete(e)
/** @} */

/** @name Set Event Flags
 *
 * @pre The event flag group has been initialized using @ref SC_EventFlagsCreate.
 *
 * # Setting Event Flags #
 *
 * Set the event flags and wake all threads that are waiting on the event flags.
 *
 * Setting event flags is done with the following options:
 * - @ref kEventFlagsSetOR - Set the request flags by OR-ing with current flags.
 * - @ref kEventFlagsSetAND - Set the request flags by AND-ing with current flags.
 *
 * # Getting Event Flags #
 *
 * Block and waits for event flags to be set in the event flag group.
 *
 * Options are:
 * - @ref kEventFlagsGetOR - Wake/return requested flags if ANY requested flags are set.
 * - @ref kEventFlagsGetORClear - Same as @ref kEventFlagsGetOR but clears all requested flags.
 * - @ref kEventFlagsGetAND - Wake/return requested flags only if all requested flags are set.
 * - @ref kEventFlagsGetANDClear - Same as @ref kEventFlagsGetAND but clears all requested flags.
 *
 * - @ref kEventFlagsGetWait - Wait/block until requested flags are set.
 * - @ref kEventFlagsGetNoWait - Return immediately and do not block until requested flags are set.
 *
 * ### Example ###
 * @code
 * PEVENT_FLAGS event_flags;
 * SC_EventFlagsCreate(&event_flags);
 *
 * #define TASK1_FLAG 0x000000001
 * #define TASK2_FLAG 0x000000002
 * #define EXIT_FLAG  0x000000004
 *
 * {  // Some thread 1
 *    // That sets flags to tell thread 2 to wake up and perform actions or exit.
 *    // Signal thread to perform task 1 and task 2
 *    SC_EventFlagsSet(event_flags, TASK1_FLAG | TASK2_FLAG, kEventFlagsSetOR);
 *    // Signal thread to exit
 *    SC_EventFlagsSet(event_flags, EXIT_FLAG, kEventFlagsSetOR);
 * }
 *
 * {  // ... meanwhile, some other thread 2...
 *    // Wait for event flags to appear and perform tasks based on the flags.
 *    while (1)
 *    {
 *       SA_DWORD flags = 0;
 *       SC_EventFlagsGet(event_flags, 0x0000000F, kEventFlagsGetORClear | kEventFlagsGetWait, &flags);
 *       if (flags & EXIT_FLAG)
 *       {
 *          break;
 *       }
 *       if (flags & TASK1_FLAG)
 *       {
 *          // Do task 1
 *       }
 *       if (flags & TASK2_FLAG)
 *       {
 *          // Do task 2
 *       }
 * }
 * @endcode
 * @{ */
#ifndef DOXYGEN_SHOULD_SKIP_THIS
/** Should not be call directly; instead invoke @ref SC_EventFlagsSet. */
SA_BOOL _SC_EventFlagsSet(PEVENT_FLAGS event_flags, SA_DWORD flags_to_set, SA_DWORD set_options);
#endif

/** Set additional event flags in @ref SC_EventFlagsSet by OR-ing with the current event flags. */
#define kEventFlagsSetOR        0x00000000
/** Set additional event flags in @ref SC_EventFlagsSet by AND-ing with the current event flags. */
#define kEventFlagsSetAND       0x00000002
/** Set the request event flags with the desired options. (see [Get/Set Event Flags](@ref syscore_sync_event_flags)). */
#define SC_EventFlagsSet(event_flags, flags_to_set, set_options) \
   _SC_EventFlagsSet(event_flags, flags_to_set, set_options)
/** @} */

/** @name Get Event Flags
 *
 * See [Set Event Flags](@ref syscore_sync_event_flags).
 * @{ */
#ifndef DOXYGEN_SHOULD_SKIP_THIS
/** Should not be called directly; instead invoke @ref SC_EventFlagsGet. */
SA_BOOL _SC_EventFlagsGet(PEVENT_FLAGS event_flags, SA_DWORD requested_flags, SA_DWORD get_options, SA_DWORD *actual_flags);
#endif

/** Get event flags from @ref SC_EventFlagsGet if any requested flags are set. */
#define kEventFlagsGetOR        0x00000001
/** Same as @ref kEventFlagsGetOR except clear the flags after getting them. */
#define kEventFlagsGetORClear   0x00000002
/** Get event flags from @ref SC_EventFlagsGet if all requested flags are set. */
#define kEventFlagsGetAND       0x00000004
/** Same as @ref kEventFlagsGetAND except clear the flags after getting them. */
#define kEventFlagsGetANDClear  0x00000008

/** Wait/block until requested flags are set. */
#define kEventFlagsGetWait      0x80000000
/** Return immediately and do not block until requested flags are set. */
#define kEventFlagsGetNoWait    0x40000000
/** Wait and get the request event flags with the desired options. (see [Get/Set Event Flags](@ref syscore_sync_event_flags)). */
#define SC_EventFlagsGet(event_flags, requested_flags, get_options, actual_flags) \
   _SC_EventFlagsGet(event_flags, requested_flags, get_options, actual_flags)
/** @} */

/** @} */ /* syscore_sync_event_flags */

#endif /* SC_DISABLE_EVENT_FLAGS */

#if defined(__cplusplus)
}    /* extern "C" { */
#endif

/** @} */ /* event_flags */

/** @} */ /* syscore */

#endif /* PSYSC_SYNCHRONIZATION_H */

