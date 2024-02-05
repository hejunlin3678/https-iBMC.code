/** @file psysc_synchronization.h

@copyright Copyright (c) 2021 Microchip Technology Inc. All Rights Reserved.

Description: sysCore synchronization header

*/

#ifndef PSYSC_SYNCHRONIZATION_H
#define PSYSC_SYNCHRONIZATION_H

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

#ifdef SC_DISABLE_SYNC_API
#  ifdef NO_VARIADIC
#    define SC_MutexCreate
#    define SC_MutexLock
#    define SC_MutexUnlock
#    define SC_MutexDelete
#    define SC_RecursiveMutexLock
#    define SC_RecursiveMutexUnlock
#    define SC_CondVariableCreate
#    define SC_CondVariableDelete
#    define SC_CondVariableSignal
#    define SC_CondVariableBroadcast
#    define SC_CondVariableWait
#    define SC_DECL_MUTEX
#    define SC_EXTERN_MUTEX
#  else
#    define SC_MutexCreate(...)
#    define SC_MutexLock(...)
#    define SC_MutexUnlock(...)
#    define SC_MutexDelete(...)
#    define SC_RecursiveMutexLock(...)
#    define SC_RecursiveMutexUnlock(...)
#    define SC_CondVariableCreate(...)
#    define SC_CondVariableDelete(...)
#    define SC_CondVariableSignal(...)
#    define SC_CondVariableBroadcast(...)
#    define SC_CondVariableWait(...)
#    define SC_DECL_MUTEX(...)
#    define SC_EXTERN_MUTEX(...)
#  endif /* NO VARIADIC */
#else /* !SC_DISABLE_SYNC_API */

#ifndef DOXYGEN_SHOULD_SKIP_THIS
typedef void* PMUTEX_LOCK;
typedef void* PCOND_VARIABLE;
#endif

#define SC_DECL_MUTEX(name) PMUTEX_LOCK name = 0;
#define SC_EXTERN_MUTEX(name) extern PMUTEX_LOCK name;

/** @defgroup syscore_sync_mutex Mutices
 * @{
 */

/** @name Create/Initialize/Destroy A Mutex
 * @pre User must initialize a mutex using @ref SC_MutexCreate before using the mutex.
 * @post Before mutex leaves scope, the user should cleanup & free the mutex using @ref SC_MutexDelete.
 * @attention Re-creating a mutex before destroying it will leak memory.
 * @attention Using a mutex that has not been initialized or has been destroyed is undefined.
 * @attention Destroying a locked mutex is undefined.
 *
 * See [Locking/Unlocking a Mutex](@ref syscore_sync_mutex) on locking/unlocking the mutex.
 *
 * Example:
 * @code
 * PMUTEX_LOCK lock;
 * if (SC_MutexCreate(&lock))
 * {
 *    // Use mutex to protect critical sections...
 *    SC_MutexDelete(lock);
 * }
 * else
 * {
 *   fprintf(stderr, "Failed to create/initialized mutex\n");
 * }
 * @endcode
 * @{ */
#ifndef DOXYGEN_SHOULD_SKIP_THIS
/** Should not be called directly; instead invoke @ref SC_MutexCreate. */
SA_BOOL _SC_MutexCreate(PMUTEX_LOCK *mutex);
/** Should not be called directly; instead invoke @ref SC_MutexDelete. */
void _SC_MutexDelete(PMUTEX_LOCK mutex);
#endif
/** Allocate & initialize a mutex (see [Create/Initialize/Destroy A Mutex](@ref syscore_sync_mutex)). */
#define SC_MutexCreate(m) _SC_MutexCreate(m)
/** Destroy and free a mutex (see [Create/Initialize/Destroy A Mutex](@ref syscore_sync_mutex)). */
#define SC_MutexDelete(m) _SC_MutexDelete(m)
/** @} */

/** @name Locking/Unlocking a Mutex (Protecting a Critical Section).
 * @pre Mutex has been initialized using @ref SC_MutexCreate.
 * @post The acquiring thread must always release the lock (using @ref SC_MutexUnlock) when done
 * with the lock to allow blocked threads to continue execution.
 * @attention Unlocking a mutex that has not been locked is undefined.
 * @attention The order of which blocked threads acquire the lock is undefined.
 * @warning Locking a mutex that has already been acquired will lead to a dead lock!
 * @warning Failing to unlock a mutex may cause other threads to dead lock!
 * @warning Mixing calls to @ref SC_MutexLock and @ref SC_RecursiveMutexLock is undefined.
 * @warning Mixing calls to @ref SC_MutexUnlock and @ref SC_RecursiveMutexUnlock is undefined.
 *
 * Example:
 * @code
 * PMUTEX_LOCK lock;
 * SC_MutexCreate(&lock);
 * if (!SC_MutexLock(&lock))
 * {
 *    fprintf(stderr, "Failed to lock mutex\n");
 *    return 1;
 * }
 *
 * // Do some critical section...
 *
 * if  (!SC_MutexUnlock(&lock))
 * {
 *    fprintf(stderr, "Failed to unlock mutex\n");
 *    return 2;
 * }
 * @endcode
 * @{
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS
/** Should not be called directly; instead invoke @ref SC_MutexLock. */
SA_BOOL _SC_MutexLock(PMUTEX_LOCK mutex);
/** Should not be called directly; instead invoke @ref SC_MutexUnlock. */
SA_BOOL _SC_MutexUnlock(PMUTEX_LOCK mutex);
#endif
/** Lock a mutex: all other threads will block until lock is released. (see [Locking/Unlocking a Mutex](@ref syscore_sync_mutex)). */
#define SC_MutexLock(m) _SC_MutexLock(m)
/** Unlock a mutex: one of the blocking threads will acquire the lock. (see [Locking/Unlocking a Mutex](@ref syscore_sync_mutex)). */
#define SC_MutexUnlock(m) _SC_MutexUnlock(m)
/** @} */

/** @name Locking/Unlocking a Recursive Mutex (Protecting a Critical Call Stack).
 * Behavior matches that of or [normal mutex](@ref syscore_sync_mutex) except
 * the acquiring thread may recursively re-lock the mutex.
 *
 * The acquiring thread must unlock the mutex an equal number of times to avoid
 * other threads blocking indefinitely.
 *
 * @pre Mutex has been initialized using @ref SC_MutexCreate.
 * @post The acquiring thread must always release the lock (using @ref SC_RecursiveMutexUnlock)
 * (the same number of times it has been locked) when done with the lock to
 * allow blocked threads to continue execution.
 * @attention Unlocking a mutex that has not been locked is undefined.
 * @attention The order of which blocked threads acquire the lock is undefined.
 * @warning Failing to unlock a mutex may cause other threads to dead lock!
 * @warning Mixing calls to @ref SC_MutexLock and @ref SC_RecursiveMutexLock is undefined.
 * @warning Mixing calls to @ref SC_MutexUnlock and @ref SC_RecursiveMutexUnlock is undefined.
 *
 * Example:
 * @code
 * PMUTEX_LOCK lock;
 * SC_MutexCreate(&lock);
 * if (!SC_RecursiveMutexLock(&lock))
 * {
 *    fprintf(stderr, "Failed to lock mutex\n");
 *    return 1;
 * }
 *
 * {
 *    // Do some critical section...
 *    SC_RecursiveMutexLock(&lock); //=> Increase Lock Depth: 2
 *    {
 *       SC_RecursiveMutexLock(&lock); //=> Increase Lock Depth: 3
 *       // Do some critical section...
 *       SC_RecursiveMutexUnlock(&lock); //=> Decrease Lock Depth: 2
 *    }
 *    SC_RecursiveMutexUnlock(&lock); //=> Decrease Lock Depth: 1
 *    // Do some critical section...
 * }
 *
 * if  (!SC_RecursiveMutexUnlock(&lock)) //=> Decrease Lock Depth: 0 -> Release lock.
 * {
 *    fprintf(stderr, "Failed to unlock mutex\n");
 *    return 2;
 * }
 * @endcode
 * @{
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS
/** Should not be call directly; instead invoke @ref SC_RecursiveMutexLock. */
SA_BOOL _SC_RecursiveMutexLock(PMUTEX_LOCK mutex);
/** Should not be call directly; instead invoke @ref SC_RecursiveMutexUnlock. */
SA_BOOL _SC_RecursiveMutexUnlock(PMUTEX_LOCK mutex);
#endif
/** Lock a recursive mutex; or increment lock depth: all other threads will block until lock is released. (see [Locking/Unlocking a Recursive Mutex](@ref syscore_sync_mutex)). */
#define SC_RecursiveMutexLock(m) _SC_RecursiveMutexLock(m)
/** Decrement lock depth, if 0, release lock: one of the blocking threads will acquire the lock. (see [Locking/Unlocking a Recursive Mutex](@ref syscore_sync_mutex)). */
#define SC_RecursiveMutexUnlock(m) _SC_RecursiveMutexUnlock(m)
/** @} */

/** @} */ /* syscore_sync_mutex */

/** @defgroup syscore_sync_cond_var Condition Variables
 * @{
 */

/** @name Create/Initialize/Destroy A Condition Variable
 * @pre User must initialize a condition variable using @ref SC_CondVariableCreate before using it.
 * @post Before condition variable leaves scope, the user should cleanup & free the condition variable using @ref SC_CondVariableDelete.
 * @attention Re-creating a condition variable before destroying it will leak memory.
 * @attention Using a condition variable that has not been initialized or has been destroyed is undefined.
 * @attention Destroying a condition variable while it is being waited on is undefined.
 *
 * Example:
 * @code
 * PCOND_VARIABLE cond;
 * if (SC_CondVariableCreate(&cond))
 * {
 *    // wait/signal condition...
 *    SC_CondVariableDelete(cond);
 * }
 * else
 * {
 *   fprintf(stderr, "Failed to create/initialized condition variable\n");
 * }
 * @endcode
 * @{ */
#ifndef DOXYGEN_SHOULD_SKIP_THIS
/** Should not be call directly; instead invoke @ref SC_CondVariableCreate. */
SA_BOOL _SC_CondVariableCreate(PCOND_VARIABLE *cond);
/** Should not be call directly; instead invoke @ref SC_CondVariableDelete. */
void _SC_CondVariableDelete(PCOND_VARIABLE cond);
#endif
/** Create/initialize a condition variable. (see [Create/Initialize/Destroy A Condition Variable](@ref syscore_sync_cond_var)). */
#define SC_CondVariableCreate(c) _SC_CondVariableCreate(c)
/** Destroy & free a condition variable. (see [Create/Initialize/Destroy A Condition Variable](@ref syscore_sync_cond_var)). */
#define SC_CondVariableDelete(c) _SC_CondVariableDelete(c)
/** @} */

/** @name Waiting/Signaling a Condition Variable (Non-Spinlock Semephore).
 * @pre The mutex has been initialized using @ref SC_MutexCreate.
 * @pre The condition variable has been initialized using @ref SC_CondVariableCreate.
 * @post The acquiring thread must always release the lock (using @ref SC_MutexUnlock) when done
 * with the lock to allow blocked threads to continue execution.
 * @attention Waiting on a condition without acquiring the lock is undefined.
 * @warning Waiting on a condition variable recursively will cause a dead lock!
 *
 * Example:
 * @code
 * PMUTEX_LOCK lock;
 * PCOND_VARIABLE cond;
 * SC_MutexCreate(&lock);
 * SC_CondVariableCreate(&cond);
 *
 * { // Some thread 1
 *    // Acquire lock to check condition.
 *    SC_MutexLock(lock);
 *    // Wait on some condition to be updated by another thread.
 *    if (some_condition != can_continue)
 *    {
 *       // Release lock and wait for a signal by other thread...
 *       if (!SC_CondVariableWait(cond, lock))
 *       {
 *          fprintf(stderr, "Thread failed to wait...\n");
 *          return 1;
 *       }
 *       // Reacquire lock to re-check condition.
 *    }
 *    // Unlock mutex and be done...
 *    SC_MutexUnlock(lock);
 * }
 *
 * { // ... meanwhile, some other thread 2...
 *    // Acquire lock to set condition.
 *    SC_MutexLock(lock);
 *    some_condition = can_continue;
 *    // Signal blocked threads to re-check the condition.
 *    if (!SC_CondVariableWait(cond)) // or SC_CondVariableBroadcast(cond)
 *    {
 *       fprintf(stderr, "Thread failed to signal...\n");
 *       return 1;
 *    }
 *    // Release lock to allow signaled thread to reacquire the lock.
 *    SC_MutexLock(lock);
 * }
 * @endcode
 * @{
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS
/** Should not be call directly; instead invoke @ref SC_CondVariableWait. */
SA_BOOL _SC_CondVariableWait(PCOND_VARIABLE cond, PMUTEX_LOCK mutex);
/** Should not be call directly; instead invoke @ref SC_CondVariableSignal. */
SA_BOOL _SC_CondVariableSignal(PCOND_VARIABLE cond);
/** Should not be call directly; instead invoke @ref SC_CondVariableBroadcast. */
SA_BOOL _SC_CondVariableBroadcast(PCOND_VARIABLE cond);
#endif
/** Thread will release the lock, and go to sleep until another thread signals/broadcasts
 * (see [Waiting/Signaling a Condition Variable (Non-Spinlock Semephore)](@ref syscore_sync_cond_var)). */
#define SC_CondVariableWait(c,m) _SC_CondVariableWait(c, m)
/** Thread will signal for a waiting thread to wake up and re-check some condition.
 * (see [Waiting/Signaling a Condition Variable (Non-Spinlock Semephore)](@ref syscore_sync_cond_var)). */
#define SC_CondVariableSignal(c) _SC_CondVariableSignal(c)
/** Thread will signal for all waiting thread to wake up and try to acquire the lock and re-check some condition.
 * (see [Waiting/Signaling a Condition Variable (Non-Spinlock Semephore)](@ref syscore_sync_cond_var)). */
#define SC_CondVariableBroadcast(c) _SC_CondVariableBroadcast(c)
/** @} */

/** @} */ /* syscore_sync_cond_var */

#endif /* !SC_DISABLE_SYNC_API */

#if defined(__cplusplus)
}
#endif

/** @} */

/** @} */

#endif /* PSYSC_SYNCHRONIZATION_H */

