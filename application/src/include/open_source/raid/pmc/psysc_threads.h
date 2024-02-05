/** @file psysc_threads.h

@copyright Copyright (c) 2021 Microchip Technology Inc. All Rights Reserved.

Description: sysCore thread header

*/

#ifndef PSYSC_THREADS_H
#define PSYSC_THREADS_H

/** @addtogroup syscore
 * @{
 */

/** @defgroup syscore_sync_threads Threads
 * @{
 */

#if defined(__cplusplus)
extern "C" {
#endif

#ifdef SC_DISABLE_THREAD_API
#  ifdef NO_VARIADIC
#    define SC_ThreadStart
#    define SC_ThreadHandleCreate
#    define SC_ThreadHandleDestroy
#    define SC_ThreadJoin
#  else
#    define SC_ThreadStart(...)
#    define SC_ThreadHandleCreate(...)
#    define SC_ThreadHandleDestroy(...)
#    define SC_ThreadJoin(...)
#  endif /* NO VARIADIC */
#else /* !SC_DISABLE_THREAD_API */

#if defined(_MSC_VER)
#  include <windows.h>
#  include <process.h>
#  define SC_THREAD_ROUTINE(name, argname) unsigned __stdcall name(void * argname)
#  if defined(SC_UNIT_TEST)
#    define SC_THREAD_EXIT(argname) return (unsigned)0;
#  else
#    define SC_THREAD_EXIT(argname) { _endthreadex((unsigned)0); return (unsigned)0; }
#  endif
typedef unsigned (__stdcall *ThreadRoutine)(void*);
#else
#  define SC_THREAD_ROUTINE(name, argname) void * name(void * argname)
#  define SC_THREAD_EXIT(argname) return argname;
typedef void*(*ThreadRoutine)(void*);
#endif

#ifndef DOXYGEN_SHOULD_SKIP_THIS
typedef void* PTHREAD_HANDLE;
#endif

SA_BOOL _SC_ThreadHandleCreate(PTHREAD_HANDLE *handle);
#define SC_ThreadHandleCreate(handle) _SC_ThreadHandleCreate(handle)

SA_BOOL _SC_ThreadStart(PTHREAD_HANDLE handle, ThreadRoutine routine, void * param);
#define SC_ThreadStart(handle, routine, param) _SC_ThreadStart(handle, routine, param)

SA_BOOL _SC_ThreadJoin(PTHREAD_HANDLE handle);
#define SC_ThreadJoin(handle) _SC_ThreadJoin(handle)

SA_BOOL _SC_ThreadHandleDestroy(PTHREAD_HANDLE handle);
#define SC_ThreadHandleDestroy(handle) _SC_ThreadHandleDestroy(handle)

/** @return The current thread ID. */
SA_DWORD SC_GetCurrentThreadID(void);

#endif /* !SC_DISABLE_THREAD_API */

/** @return The current process ID. */
SA_DWORD SC_GetCurrentProcessID(void);

/** Sleep the current thread */
void SC_Sleep(SA_DWORD milliseconds);

#if defined(__cplusplus)
}    /* extern "C" { */
#endif

/** @} */ /* syscore_sync_threads */

/** @} */

#endif /* PSYSC_THREADS_H */

